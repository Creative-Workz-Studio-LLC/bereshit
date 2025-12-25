-- ============================================================================
-- METADATA
-- ============================================================================
-- Migration: 002 - Log Integration Tables
-- Purpose: Integrate CPI-SI JSONL logs into queryable database
-- Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
--
-- This migration adds tables to store and analyze:
-- - Session events (lifecycle, feedback, delegation)
-- - Tool events (key picks, completions, outcomes)
-- - Enables pattern detection and state machine tuning

-- ============================================================================
-- TABLES
-- ============================================================================

-- Session Events: Lifecycle and feedback tracking
CREATE TABLE IF NOT EXISTS session_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    timestamp DATETIME NOT NULL,
    sequence_num INTEGER NOT NULL,

    -- Event classification
    event_type TEXT NOT NULL,  -- 'session_start', 'feedback_detected', 'door_closing', etc.
    level TEXT NOT NULL,       -- 'INFO', 'WARN', 'ERROR'
    component TEXT NOT NULL,   -- 'session', 'tools', 'context'
    message TEXT,

    -- Health tracking
    health_delta REAL,         -- Change in health from this event
    health_after REAL,         -- Health score after event
    health_level TEXT,         -- 'broken', 'lacking', 'even', 'sound', 'whole', 'perfect'

    -- Context
    trajectory TEXT,           -- 'B.1', 'B.2', 'B.3', 'B.4'
    details TEXT,              -- JSON with event-specific data

    -- Log source
    log_date DATE NOT NULL,    -- Which log file this came from

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    UNIQUE(session_id, timestamp, event_type, sequence_num)
);

CREATE INDEX IF NOT EXISTS idx_session_events_type ON session_events(event_type);
CREATE INDEX IF NOT EXISTS idx_session_events_session ON session_events(session_id);
CREATE INDEX IF NOT EXISTS idx_session_events_timestamp ON session_events(timestamp);
CREATE INDEX IF NOT EXISTS idx_session_events_level ON session_events(level);

-- Tool Events: Key picks and completions
CREATE TABLE IF NOT EXISTS tool_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    timestamp DATETIME NOT NULL,
    sequence_num INTEGER NOT NULL,

    -- Event classification
    event_type TEXT NOT NULL,  -- 'key_picked', 'tool_completed'
    tool_name TEXT NOT NULL,

    -- Key tracking (the 3 keys)
    intended_key INTEGER,      -- -1 (completion), 0 (lateral), +1 (growth)
    intended_key_label TEXT,   -- 'completion', 'lateral', 'growth'

    -- Health tracking
    health_delta REAL,
    health_after REAL,
    health_level TEXT,

    -- Context
    trajectory TEXT,

    -- Log source
    log_date DATE NOT NULL,

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    UNIQUE(session_id, timestamp, event_type, tool_name)
);

CREATE INDEX IF NOT EXISTS idx_tool_events_tool ON tool_events(tool_name);
CREATE INDEX IF NOT EXISTS idx_tool_events_key ON tool_events(intended_key);
CREATE INDEX IF NOT EXISTS idx_tool_events_session ON tool_events(session_id);

-- Feedback Events: User corrections (critical for learning)
CREATE TABLE IF NOT EXISTS feedback_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    timestamp DATETIME NOT NULL,

    -- Feedback classification
    categories TEXT,           -- '[rejection caution]', '[correction]', etc.

    -- Impact
    health_delta REAL NOT NULL,
    k_align_before REAL,
    k_align_after REAL,
    k_delta REAL,

    -- Context
    trajectory TEXT,
    details TEXT,              -- JSON with full feedback context

    -- Log source
    log_date DATE NOT NULL,

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_feedback_session ON feedback_events(session_id);
CREATE INDEX IF NOT EXISTS idx_feedback_timestamp ON feedback_events(timestamp);

-- Log Import Tracking: Prevent duplicate imports
CREATE TABLE IF NOT EXISTS log_imports (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    log_type TEXT NOT NULL,    -- 'session', 'tools', 'context'
    log_date DATE NOT NULL,
    file_path TEXT NOT NULL,

    -- Import stats
    events_imported INTEGER DEFAULT 0,
    events_skipped INTEGER DEFAULT 0,

    imported_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    UNIQUE(log_type, log_date)
);

-- ============================================================================
-- VIEWS
-- ============================================================================

-- Key Distribution Analysis
CREATE VIEW IF NOT EXISTS key_distribution AS
SELECT
    intended_key,
    intended_key_label,
    COUNT(*) AS total_picks,
    AVG(health_delta) AS avg_health_delta,
    trajectory,
    COUNT(*) * 100.0 / SUM(COUNT(*)) OVER (PARTITION BY trajectory) AS pct_of_trajectory
FROM tool_events
WHERE event_type = 'key_picked'
GROUP BY intended_key, intended_key_label, trajectory;

-- Tool Success Patterns
CREATE VIEW IF NOT EXISTS tool_patterns AS
SELECT
    tool_name,
    COUNT(*) AS total_uses,
    SUM(CASE WHEN health_delta > 0 THEN 1 ELSE 0 END) AS positive_outcomes,
    SUM(CASE WHEN health_delta < 0 THEN 1 ELSE 0 END) AS negative_outcomes,
    AVG(health_delta) AS avg_health_delta,
    -- Most common key for this tool
    (SELECT intended_key FROM tool_events te2
     WHERE te2.tool_name = tool_events.tool_name
     GROUP BY intended_key
     ORDER BY COUNT(*) DESC LIMIT 1) AS dominant_key
FROM tool_events
WHERE event_type = 'tool_completed'
GROUP BY tool_name
ORDER BY total_uses DESC;

-- Feedback Patterns: What precedes corrections?
CREATE VIEW IF NOT EXISTS pre_feedback_patterns AS
SELECT
    te.tool_name AS last_tool,
    te.intended_key AS last_key,
    COUNT(*) AS feedback_count,
    AVG(fe.health_delta) AS avg_feedback_impact
FROM feedback_events fe
JOIN tool_events te ON te.session_id = fe.session_id
    AND te.timestamp < fe.timestamp
    AND te.timestamp > datetime(fe.timestamp, '-5 minutes')
WHERE te.event_type = 'tool_completed'
GROUP BY te.tool_name, te.intended_key
ORDER BY feedback_count DESC;

-- Trajectory Time Distribution
CREATE VIEW IF NOT EXISTS trajectory_time AS
SELECT
    trajectory,
    COUNT(*) AS event_count,
    MIN(timestamp) AS first_seen,
    MAX(timestamp) AS last_seen,
    -- Time spent in trajectory
    ROUND((julianday(MAX(timestamp)) - julianday(MIN(timestamp))) * 24 * 60, 1) AS minutes_in_trajectory
FROM session_events
WHERE trajectory IS NOT NULL
GROUP BY trajectory, DATE(timestamp);

-- Session Health Trend
CREATE VIEW IF NOT EXISTS session_health_trend AS
SELECT
    session_id,
    DATE(timestamp) AS date,
    MIN(health_after) AS min_health,
    MAX(health_after) AS max_health,
    AVG(health_after) AS avg_health,
    COUNT(*) AS event_count
FROM session_events
WHERE health_after IS NOT NULL
GROUP BY session_id, DATE(timestamp);

-- ============================================================================
-- VERSION TRACKING
-- ============================================================================

INSERT OR IGNORE INTO schema_version (version, description)
VALUES (2, 'Log integration tables: session_events, tool_events, feedback_events');
