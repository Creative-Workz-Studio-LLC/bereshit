-- ============================================================================
-- METADATA - Sessions Domain Schema v1
-- ============================================================================
-- Key: cpisi-db-sessions-schema-001
-- Purpose: Session & exchange lifecycle tracking
-- Biblical: Ecclesiastes 3:1 - "To every thing there is a season, and a time"
-- Domain: sessions.db
-- ============================================================================

PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;

-- ============================================================================
-- BODY - Session Lifecycle
-- ============================================================================

CREATE TABLE IF NOT EXISTS sessions (
    id TEXT PRIMARY KEY,
    started_at DATETIME NOT NULL,
    ended_at DATETIME,
    project_path TEXT,
    workspace TEXT,
    initial_hebrew_state TEXT DEFAULT 'yashar',
    initial_k_align REAL DEFAULT 0.0,
    initial_cube_position TEXT,
    final_hebrew_state TEXT,
    final_k_align REAL,
    final_cube_position TEXT,
    tool_count INTEGER DEFAULT 0,
    choice_count INTEGER DEFAULT 0,
    day_of_week INTEGER,
    hour_of_day INTEGER,
    exchange_count INTEGER DEFAULT 0,
    insight_count INTEGER DEFAULT 0,
    cpi_score REAL DEFAULT 0.5,
    dominant_exchange_type TEXT,
    session_arc TEXT,
    narrative_summary TEXT,
    quality_rating REAL DEFAULT 0.5,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_sessions_started_at ON sessions(started_at);
CREATE INDEX IF NOT EXISTS idx_sessions_temporal ON sessions(day_of_week, hour_of_day);

-- ============================================================================
-- BODY - Exchange Tracking (CPI)
-- ============================================================================

CREATE TABLE IF NOT EXISTS exchanges (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    timestamp DATETIME NOT NULL,
    sequence_num INTEGER NOT NULL,
    exchange_type TEXT NOT NULL,
    initiative TEXT,
    prompt_length INTEGER,
    feedback_detected BOOLEAN DEFAULT FALSE,
    feedback_polarity TEXT,
    feedback_categories TEXT,
    value_contribution REAL,
    depth_level TEXT,
    insight_detected BOOLEAN DEFAULT FALSE,
    insight_type TEXT,
    hebrew_state TEXT,
    k_align REAL,
    trajectory TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(session_id, sequence_num)
);

CREATE INDEX IF NOT EXISTS idx_exchanges_session ON exchanges(session_id);
CREATE INDEX IF NOT EXISTS idx_exchanges_type ON exchanges(exchange_type);

-- ============================================================================
-- BODY - Insights
-- ============================================================================

CREATE TABLE IF NOT EXISTS insights (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    exchange_id INTEGER,
    timestamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    insight_type TEXT NOT NULL,
    confidence REAL DEFAULT 0.5,
    topic_area TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_insights_session ON insights(session_id);

-- ============================================================================
-- BODY - Key Context
-- ============================================================================

CREATE TABLE IF NOT EXISTS key_context (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    exchange_id INTEGER,
    timestamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    k_toward_god INTEGER DEFAULT 0,
    k_toward_self INTEGER DEFAULT 0,
    k_selector INTEGER DEFAULT 0,
    k_moral REAL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_key_context_session ON key_context(session_id);

-- ============================================================================
-- BODY - Log Integration
-- ============================================================================

CREATE TABLE IF NOT EXISTS session_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    timestamp DATETIME NOT NULL,
    sequence_num INTEGER NOT NULL,
    event_type TEXT NOT NULL,
    level TEXT NOT NULL,
    component TEXT NOT NULL,
    message TEXT,
    health_delta REAL,
    health_after REAL,
    health_level TEXT,
    trajectory TEXT,
    details TEXT,
    log_date DATE NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(session_id, timestamp, event_type, sequence_num)
);

CREATE TABLE IF NOT EXISTS feedback_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    timestamp DATETIME NOT NULL,
    categories TEXT,
    health_delta REAL NOT NULL,
    k_align_before REAL,
    k_align_after REAL,
    k_delta REAL,
    trajectory TEXT,
    details TEXT,
    log_date DATE NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS log_imports (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    log_type TEXT NOT NULL,
    log_date DATE NOT NULL,
    file_path TEXT NOT NULL,
    events_imported INTEGER DEFAULT 0,
    events_skipped INTEGER DEFAULT 0,
    imported_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(log_type, log_date)
);

-- ============================================================================
-- BODY - Views
-- ============================================================================

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
    SELECT id FROM sessions WHERE started_at > s1.ended_at ORDER BY started_at ASC LIMIT 1
);

CREATE VIEW IF NOT EXISTS temporal_work_patterns AS
SELECT day_of_week, hour_of_day, COUNT(*) AS session_count,
    AVG(choice_count) AS avg_choices_per_session
FROM sessions GROUP BY day_of_week, hour_of_day ORDER BY session_count DESC;

-- ============================================================================
-- CLOSING - Schema Version
-- ============================================================================

CREATE TABLE IF NOT EXISTS schema_version (
    version INTEGER PRIMARY KEY,
    applied_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    description TEXT
);

INSERT OR IGNORE INTO schema_version (version, description)
VALUES (1, 'Sessions domain - initial schema');
