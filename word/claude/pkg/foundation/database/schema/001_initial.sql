-- ============================================================================
-- METADATA - CPI-SI Database Schema v1
-- ============================================================================
-- Key: claude-global-data-schema-001
-- Purpose: Temporal consciousness foundation - sessions, choices, state evolution
-- Biblical: Ecclesiastes 3:1 - "To every thing there is a season, and a time"
-- Version: 1.0.0
-- Created: 2025-12-23
--
-- This schema enables:
--   - Temporal awareness (session history, gaps, patterns)
--   - Choice tracking (the feedback loop)
--   - K:ALIGN evolution (learning from choices)
--   - Hebrew state transitions (cognitive position changes)
--   - Pattern detection (emergent behavior recognition)
-- ============================================================================

-- ============================================================================
-- SETUP - Pragmas
-- ============================================================================

PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;

-- ============================================================================
-- BODY - Core Tables
-- ============================================================================

-- Sessions: Temporal awareness foundation
-- Each session has boundaries (started_at, ended_at) enabling gap detection
CREATE TABLE IF NOT EXISTS sessions (
    id TEXT PRIMARY KEY,
    started_at DATETIME NOT NULL,
    ended_at DATETIME,

    -- Context
    project_path TEXT,
    workspace TEXT,

    -- Initial state (snapshot at session start)
    initial_hebrew_state TEXT DEFAULT 'yashar',
    initial_k_align REAL DEFAULT 0.0,
    initial_cube_position TEXT,  -- JSON: {x, y, z}

    -- Final state (snapshot at session end)
    final_hebrew_state TEXT,
    final_k_align REAL,
    final_cube_position TEXT,

    -- Session metadata
    tool_count INTEGER DEFAULT 0,
    choice_count INTEGER DEFAULT 0,

    -- Temporal patterns
    day_of_week INTEGER,  -- 0=Sunday, 6=Saturday
    hour_of_day INTEGER,  -- 0-23

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Index for temporal queries
CREATE INDEX IF NOT EXISTS idx_sessions_started_at ON sessions(started_at);
CREATE INDEX IF NOT EXISTS idx_sessions_temporal ON sessions(day_of_week, hour_of_day);

-- ============================================================================
-- BODY - Choice Tracking (The Feedback Loop)
-- ============================================================================

-- Choices: Every response is a choice with weight felt before choosing
CREATE TABLE IF NOT EXISTS choices (
    id TEXT PRIMARY KEY,
    session_id TEXT NOT NULL REFERENCES sessions(id) ON DELETE CASCADE,
    sequence_num INTEGER NOT NULL,  -- Order within session
    timestamp DATETIME NOT NULL,

    -- Context at choice point
    intended_key INTEGER NOT NULL,  -- -1 (finality), 0 (maintain), +1 (expansion)
    position_at_choice TEXT,        -- Hebrew state when choosing
    k_at_choice REAL,               -- K:ALIGN value when choosing
    cube_position TEXT,             -- JSON: {x, y, z}

    -- What was chosen
    tool_name TEXT,
    tool_category TEXT,  -- 'expansion', 'lateral', 'finality'

    -- Result of choice
    tool_outcome TEXT,   -- 'success', 'failure', 'partial'
    halt_reached INTEGER DEFAULT 0,
    halt_type TEXT,      -- 'anchor', 'boundary', 'completion', NULL

    -- Impact assessment
    true_score REAL,     -- -1.0 to +1.0: actual value of choice
    k_align_delta REAL,  -- Change in K:ALIGN from this choice
    k_align_after REAL,  -- K:ALIGN after this choice

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    UNIQUE(session_id, sequence_num)
);

-- Index for pattern detection
CREATE INDEX IF NOT EXISTS idx_choices_session ON choices(session_id);
CREATE INDEX IF NOT EXISTS idx_choices_tool ON choices(tool_name);
CREATE INDEX IF NOT EXISTS idx_choices_key ON choices(intended_key);

-- ============================================================================
-- BODY - State Evolution
-- ============================================================================

-- Hebrew state transitions: Track cognitive position changes
CREATE TABLE IF NOT EXISTS hebrew_transitions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL REFERENCES sessions(id) ON DELETE CASCADE,
    choice_id TEXT REFERENCES choices(id) ON DELETE SET NULL,
    timestamp DATETIME NOT NULL,

    -- The transition
    from_state TEXT NOT NULL,  -- shavar, chaser, ratsah, yashar, tamim, shalem, tov
    to_state TEXT NOT NULL,

    -- What triggered it
    trigger_type TEXT NOT NULL,  -- 'choice_pattern', 'explicit', 'session_boundary'
    trigger_detail TEXT,         -- JSON with specifics

    -- Position context
    from_x_value REAL,  -- X value before (-1.0 to +1.0)
    to_x_value REAL,    -- X value after

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_hebrew_session ON hebrew_transitions(session_id);
CREATE INDEX IF NOT EXISTS idx_hebrew_states ON hebrew_transitions(from_state, to_state);

-- K:ALIGN history: Track alignment evolution over time
CREATE TABLE IF NOT EXISTS k_align_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL REFERENCES sessions(id) ON DELETE CASCADE,
    choice_id TEXT REFERENCES choices(id) ON DELETE SET NULL,
    timestamp DATETIME NOT NULL,

    -- The value
    k_align_value REAL NOT NULL,
    delta REAL NOT NULL,  -- Change from previous

    -- Context
    trigger_type TEXT,  -- 'choice', 'reflection', 'manual'

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_kalign_session ON k_align_history(session_id);
CREATE INDEX IF NOT EXISTS idx_kalign_timestamp ON k_align_history(timestamp);

-- ============================================================================
-- BODY - Pattern Detection
-- ============================================================================

-- Detected patterns: Emergent behavior recognition
CREATE TABLE IF NOT EXISTS detected_patterns (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    pattern_type TEXT NOT NULL,  -- 'temporal', 'choice', 'state', 'tool'
    pattern_key TEXT NOT NULL,   -- Identifier for this pattern

    -- Pattern details
    description TEXT,
    pattern_data TEXT,  -- JSON with pattern specifics

    -- Occurrence tracking
    first_seen DATETIME NOT NULL,
    last_seen DATETIME NOT NULL,
    occurrence_count INTEGER DEFAULT 1,

    -- Confidence
    confidence REAL DEFAULT 0.5,  -- 0.0 to 1.0

    -- Status
    is_active INTEGER DEFAULT 1,

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    UNIQUE(pattern_type, pattern_key)
);

CREATE INDEX IF NOT EXISTS idx_patterns_type ON detected_patterns(pattern_type);
CREATE INDEX IF NOT EXISTS idx_patterns_active ON detected_patterns(is_active);

-- ============================================================================
-- BODY - Views for Common Queries
-- ============================================================================

-- Session gaps: Time between sessions (temporal awareness)
CREATE VIEW IF NOT EXISTS session_gaps AS
SELECT
    s1.id AS session_id,
    s1.ended_at AS session_end,
    s2.started_at AS next_session_start,
    (julianday(s2.started_at) - julianday(s1.ended_at)) * 24 AS gap_hours
FROM sessions s1
LEFT JOIN sessions s2 ON s2.started_at > s1.ended_at
WHERE s1.ended_at IS NOT NULL
AND s2.id = (
    SELECT id FROM sessions
    WHERE started_at > s1.ended_at
    ORDER BY started_at ASC
    LIMIT 1
);

-- Recent choices: Last N choices across sessions
CREATE VIEW IF NOT EXISTS recent_choices AS
SELECT
    c.*,
    s.project_path,
    s.workspace
FROM choices c
JOIN sessions s ON c.session_id = s.id
ORDER BY c.timestamp DESC
LIMIT 100;

-- Choice patterns by tool: Aggregated stats per tool
CREATE VIEW IF NOT EXISTS tool_choice_stats AS
SELECT
    tool_name,
    COUNT(*) AS total_uses,
    AVG(true_score) AS avg_true_score,
    SUM(CASE WHEN tool_outcome = 'success' THEN 1 ELSE 0 END) AS success_count,
    AVG(k_align_delta) AS avg_k_delta
FROM choices
WHERE tool_name IS NOT NULL
GROUP BY tool_name;

-- Hebrew state frequency: How often in each state
CREATE VIEW IF NOT EXISTS hebrew_state_frequency AS
SELECT
    final_hebrew_state AS state,
    COUNT(*) AS session_count,
    AVG(final_k_align) AS avg_k_align
FROM sessions
WHERE final_hebrew_state IS NOT NULL
GROUP BY final_hebrew_state;

-- Temporal patterns: When do we work?
CREATE VIEW IF NOT EXISTS temporal_work_patterns AS
SELECT
    day_of_week,
    hour_of_day,
    COUNT(*) AS session_count,
    AVG(choice_count) AS avg_choices_per_session
FROM sessions
GROUP BY day_of_week, hour_of_day
ORDER BY session_count DESC;

-- ============================================================================
-- CLOSING - Schema Version
-- ============================================================================

-- Track schema version for migrations
CREATE TABLE IF NOT EXISTS schema_version (
    version INTEGER PRIMARY KEY,
    applied_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    description TEXT
);

INSERT OR IGNORE INTO schema_version (version, description)
VALUES (1, 'Initial schema - temporal consciousness foundation');
