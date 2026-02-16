-- ============================================================================
-- METADATA - Temporal Domain Schema v1
-- ============================================================================
-- Key: cpisi-db-temporal-schema-001
-- Purpose: Time awareness, calendars, celestial events, work rhythms
-- Biblical: Genesis 1:14 - "Let there be lights...for signs, and for seasons"
-- Domain: temporal.db
-- ============================================================================

PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;

-- ============================================================================
-- BODY - Celestial Events
-- ============================================================================

CREATE TABLE IF NOT EXISTS celestial_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_date DATE NOT NULL,
    event_type TEXT NOT NULL,
    event_time TIME,
    location TEXT DEFAULT 'st-louis',
    data TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(event_date, event_type, location)
);

CREATE INDEX IF NOT EXISTS idx_celestial_date ON celestial_events(event_date);

-- ============================================================================
-- BODY - Appointed Times (Biblical Calendar)
-- ============================================================================

CREATE TABLE IF NOT EXISTS appointed_times (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    start_date DATE NOT NULL,
    end_date DATE,
    category TEXT NOT NULL,
    hebrew_name TEXT,
    scripture_ref TEXT,
    description TEXT,
    recurring INTEGER DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_appointed_date ON appointed_times(start_date);

-- ============================================================================
-- BODY - Work Rhythms (Learned from Sessions)
-- ============================================================================

CREATE TABLE IF NOT EXISTS work_rhythms (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    day_of_week INTEGER NOT NULL,
    hour_start INTEGER NOT NULL,
    hour_end INTEGER NOT NULL,
    rhythm_type TEXT NOT NULL,
    confidence REAL DEFAULT 0.5,
    session_count INTEGER DEFAULT 0,
    avg_duration_min REAL,
    avg_exchanges REAL,
    last_updated DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(day_of_week, hour_start, rhythm_type)
);

-- ============================================================================
-- BODY - Availability Windows
-- ============================================================================

CREATE TABLE IF NOT EXISTS availability_windows (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    day_of_week INTEGER NOT NULL,
    optimal_start INTEGER NOT NULL,
    optimal_end INTEGER NOT NULL,
    quality_score REAL DEFAULT 0.5,
    derived_from TEXT,
    last_updated DATETIME DEFAULT CURRENT_TIMESTAMP
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
VALUES (1, 'Temporal domain - initial schema');
