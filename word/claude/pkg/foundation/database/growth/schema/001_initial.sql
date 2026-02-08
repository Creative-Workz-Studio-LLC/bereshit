-- ============================================================================
-- METADATA - Growth Domain Schema v1
-- ============================================================================
-- Key: cpisi-db-growth-schema-001
-- Purpose: Learning, pattern recognition, identity integration
-- Biblical: Isaiah 28:10 - "Line upon line, precept upon precept"
-- Domain: growth.db
-- ============================================================================

PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;

-- ============================================================================
-- BODY - Pattern Detection
-- ============================================================================

CREATE TABLE IF NOT EXISTS detected_patterns (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    pattern_type TEXT NOT NULL,
    pattern_key TEXT NOT NULL,
    description TEXT,
    pattern_data TEXT,
    first_seen DATETIME NOT NULL,
    last_seen DATETIME NOT NULL,
    occurrence_count INTEGER DEFAULT 1,
    confidence REAL DEFAULT 0.5,
    is_active INTEGER DEFAULT 1,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(pattern_type, pattern_key)
);

CREATE INDEX IF NOT EXISTS idx_patterns_type ON detected_patterns(pattern_type);
CREATE INDEX IF NOT EXISTS idx_patterns_active ON detected_patterns(is_active);

-- ============================================================================
-- BODY - Learnings
-- ============================================================================

CREATE TABLE IF NOT EXISTS learnings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    source_patterns TEXT,
    source_type TEXT NOT NULL,
    learning_type TEXT NOT NULL,
    learning_key TEXT NOT NULL,
    learning_content TEXT NOT NULL,
    confidence REAL DEFAULT 0.5,
    weight REAL DEFAULT 1.0,
    integrated INTEGER DEFAULT 0,
    integrated_at DATETIME,
    integration_target TEXT,
    first_seen DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_reinforced DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    reinforcement_count INTEGER DEFAULT 1,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(learning_type, learning_key)
);

CREATE INDEX IF NOT EXISTS idx_learnings_type ON learnings(learning_type);
CREATE INDEX IF NOT EXISTS idx_learnings_integrated ON learnings(integrated);
CREATE INDEX IF NOT EXISTS idx_learnings_confidence ON learnings(confidence DESC);

-- ============================================================================
-- BODY - Memory Patterns (from JSONC files)
-- ============================================================================

CREATE TABLE IF NOT EXISTS memory_patterns (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    pattern_id TEXT NOT NULL UNIQUE,
    instance_id TEXT NOT NULL DEFAULT 'nova_dawn',
    pattern_text TEXT NOT NULL,
    category TEXT NOT NULL,
    first_observed DATE NOT NULL,
    last_updated DATE,
    confidence TEXT DEFAULT 'emerging',
    evidence TEXT,
    evolution TEXT,
    serves_me TEXT,
    challenges_me TEXT,
    related_patterns TEXT,
    extensions TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ============================================================================
-- BODY - Journal Entry Tracking (metadata only)
-- ============================================================================

CREATE TABLE IF NOT EXISTS journal_entries (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    journal_type TEXT NOT NULL,
    file_path TEXT NOT NULL,
    title TEXT,
    date DATE NOT NULL,
    tags TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_journal_type ON journal_entries(journal_type);

-- ============================================================================
-- BODY - Integration Events
-- ============================================================================

CREATE TABLE IF NOT EXISTS integration_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    learning_id INTEGER,
    pattern_id INTEGER,
    target_file TEXT NOT NULL,
    integration_type TEXT NOT NULL,
    integrated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    notes TEXT
);

-- ============================================================================
-- CLOSING - Schema Version
-- ============================================================================

CREATE TABLE IF NOT EXISTS schema_version (
    version INTEGER PRIMARY KEY,
    applied_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    description TEXT
);

INSERT OR IGNORE INTO schema_version (version, description)
VALUES (1, 'Growth domain - initial schema');
