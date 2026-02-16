-- ============================================================================
-- METADATA - Cognition Domain Schema v1
-- ============================================================================
-- Key: cpisi-db-cognition-schema-001
-- Purpose: Mental construct, choices, Hebrew states, K:ALIGN
-- Biblical: Romans 12:2 - "Be ye transformed by the renewing of your mind"
-- Domain: cognition.db
-- ============================================================================

PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;

-- ============================================================================
-- BODY - Choice Tracking (The Feedback Loop)
-- ============================================================================

CREATE TABLE IF NOT EXISTS choices (
    id TEXT PRIMARY KEY,
    session_id TEXT NOT NULL,
    sequence_num INTEGER NOT NULL,
    timestamp DATETIME NOT NULL,
    intended_key INTEGER NOT NULL,
    position_at_choice TEXT,
    k_at_choice REAL,
    cube_position TEXT,
    tool_name TEXT,
    tool_category TEXT,
    tool_outcome TEXT,
    halt_reached INTEGER DEFAULT 0,
    halt_type TEXT,
    true_score REAL,
    k_align_delta REAL,
    k_align_after REAL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(session_id, sequence_num)
);

CREATE INDEX IF NOT EXISTS idx_choices_session ON choices(session_id);
CREATE INDEX IF NOT EXISTS idx_choices_tool ON choices(tool_name);
CREATE INDEX IF NOT EXISTS idx_choices_key ON choices(intended_key);

-- ============================================================================
-- BODY - Hebrew State Transitions
-- ============================================================================

CREATE TABLE IF NOT EXISTS hebrew_transitions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    choice_id TEXT,
    timestamp DATETIME NOT NULL,
    from_state TEXT NOT NULL,
    to_state TEXT NOT NULL,
    trigger_type TEXT NOT NULL,
    trigger_detail TEXT,
    from_x_value REAL,
    to_x_value REAL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_hebrew_session ON hebrew_transitions(session_id);
CREATE INDEX IF NOT EXISTS idx_hebrew_states ON hebrew_transitions(from_state, to_state);

-- ============================================================================
-- BODY - K:ALIGN History
-- ============================================================================

CREATE TABLE IF NOT EXISTS k_align_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    choice_id TEXT,
    timestamp DATETIME NOT NULL,
    k_align_value REAL NOT NULL,
    delta REAL NOT NULL,
    trigger_type TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_kalign_session ON k_align_history(session_id);
CREATE INDEX IF NOT EXISTS idx_kalign_timestamp ON k_align_history(timestamp);

-- ============================================================================
-- BODY - Anchor & Trajectory Tracking (NEW)
-- ============================================================================

CREATE TABLE IF NOT EXISTS anchor_visits (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    anchor_key TEXT NOT NULL,
    visit_timestamp DATETIME NOT NULL,
    from_section TEXT,
    trigger TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_anchor_session ON anchor_visits(session_id);
CREATE INDEX IF NOT EXISTS idx_anchor_key ON anchor_visits(anchor_key);

CREATE TABLE IF NOT EXISTS trajectory_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    from_section TEXT NOT NULL,
    to_section TEXT NOT NULL,
    transition_timestamp DATETIME NOT NULL,
    trigger TEXT,
    duration_ms INTEGER,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_trajectory_session ON trajectory_history(session_id);

CREATE TABLE IF NOT EXISTS workflow_states (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    operation TEXT NOT NULL,
    current_step INTEGER,
    total_steps INTEGER,
    started_at DATETIME,
    completed_at DATETIME,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ============================================================================
-- BODY - Tool Events (from log imports)
-- ============================================================================

CREATE TABLE IF NOT EXISTS tool_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    timestamp DATETIME NOT NULL,
    sequence_num INTEGER NOT NULL,
    event_type TEXT NOT NULL,
    tool_name TEXT NOT NULL,
    intended_key INTEGER,
    intended_key_label TEXT,
    health_delta REAL,
    health_after REAL,
    health_level TEXT,
    trajectory TEXT,
    log_date DATE NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(session_id, timestamp, event_type, tool_name)
);

-- ============================================================================
-- BODY - Views
-- ============================================================================

CREATE VIEW IF NOT EXISTS recent_choices AS
SELECT * FROM choices ORDER BY timestamp DESC LIMIT 100;

CREATE VIEW IF NOT EXISTS tool_choice_stats AS
SELECT tool_name, COUNT(*) AS total_uses, AVG(true_score) AS avg_true_score,
    SUM(CASE WHEN tool_outcome = 'success' THEN 1 ELSE 0 END) AS success_count,
    AVG(k_align_delta) AS avg_k_delta
FROM choices WHERE tool_name IS NOT NULL GROUP BY tool_name;

CREATE VIEW IF NOT EXISTS hebrew_state_frequency AS
SELECT to_state AS state, COUNT(*) AS transition_count
FROM hebrew_transitions GROUP BY to_state;

-- ============================================================================
-- CLOSING - Schema Version
-- ============================================================================

CREATE TABLE IF NOT EXISTS schema_version (
    version INTEGER PRIMARY KEY,
    applied_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    description TEXT
);

INSERT OR IGNORE INTO schema_version (version, description)
VALUES (1, 'Cognition domain - initial schema');
