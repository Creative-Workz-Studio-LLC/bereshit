-- build.db Schema
-- Build state database for Kingdom Technology implementation
-- Makes Phase 1+ build artifacts queryable
--
-- Biblical anchor: "Except the LORD build the house, they labour in vain" (Psalm 127:1)
-- Build state tracks the work of implementation

-- Source files table
CREATE TABLE IF NOT EXISTS sources (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    path TEXT NOT NULL UNIQUE,              -- relative path from word/work/
    name TEXT NOT NULL,                     -- filename
    extension TEXT NOT NULL,                -- .c, .h, .go, etc
    source_type TEXT NOT NULL,              -- 'header', 'implementation', 'test', 'demo'
    layer TEXT,                             -- 'MATTER', 'SPACE', 'TIME' for libtrit
    size_bytes INTEGER,
    last_modified TEXT NOT NULL,            -- ISO timestamp
    checksum TEXT,                          -- SHA256 for change detection
    created_at TEXT NOT NULL
);

-- Object files table (compiled artifacts)
CREATE TABLE IF NOT EXISTS objects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    source_id INTEGER NOT NULL,             -- FK to sources
    path TEXT NOT NULL UNIQUE,              -- path to .o file
    size_bytes INTEGER,
    last_built TEXT,                        -- ISO timestamp
    build_status TEXT DEFAULT 'pending' CHECK(build_status IN ('pending', 'building', 'success', 'failed')),
    build_duration_ms INTEGER,
    compiler_version TEXT,
    created_at TEXT NOT NULL,
    FOREIGN KEY (source_id) REFERENCES sources(id)
);

-- Libraries table (archives like libtrit.a)
CREATE TABLE IF NOT EXISTS libraries (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,              -- e.g., 'libtrit'
    path TEXT NOT NULL,                     -- path to .a file
    version TEXT,
    size_bytes INTEGER,
    last_built TEXT,
    object_count INTEGER,                   -- number of .o files included
    created_at TEXT NOT NULL
);

-- Library to object mapping
CREATE TABLE IF NOT EXISTS library_objects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    library_id INTEGER NOT NULL,
    object_id INTEGER NOT NULL,
    created_at TEXT NOT NULL,
    FOREIGN KEY (library_id) REFERENCES libraries(id),
    FOREIGN KEY (object_id) REFERENCES objects(id),
    UNIQUE(library_id, object_id)
);

-- Test results table
CREATE TABLE IF NOT EXISTS tests (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,                     -- test name/suite
    source_id INTEGER,                      -- FK to test source file
    last_run TEXT,                          -- ISO timestamp
    status TEXT DEFAULT 'pending' CHECK(status IN ('pending', 'running', 'passed', 'failed', 'skipped')),
    assertions_total INTEGER DEFAULT 0,
    assertions_passed INTEGER DEFAULT 0,
    assertions_failed INTEGER DEFAULT 0,
    duration_ms INTEGER,
    output TEXT,                            -- captured test output
    created_at TEXT NOT NULL,
    FOREIGN KEY (source_id) REFERENCES sources(id)
);

-- Demo programs table
CREATE TABLE IF NOT EXISTS demos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,              -- e.g., 'demo-trit', 'demo-math'
    phase INTEGER NOT NULL,                 -- 0, 1, 2, etc
    path TEXT NOT NULL,                     -- path to demo folder
    last_run TEXT,
    status TEXT DEFAULT 'pending' CHECK(status IN ('pending', 'building', 'passed', 'failed')),
    steps_total INTEGER DEFAULT 0,
    steps_passed INTEGER DEFAULT 0,
    duration_ms INTEGER,
    created_at TEXT NOT NULL
);

-- Build events log
CREATE TABLE IF NOT EXISTS build_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_type TEXT NOT NULL,               -- 'compile', 'link', 'test', 'demo'
    target TEXT NOT NULL,                   -- what was built
    status TEXT NOT NULL,                   -- 'start', 'success', 'failed'
    message TEXT,
    duration_ms INTEGER,
    timestamp TEXT NOT NULL
);

-- Views for common queries

-- View: Source files by layer
CREATE VIEW IF NOT EXISTS v_sources_by_layer AS
SELECT
    layer,
    COUNT(*) AS file_count,
    SUM(size_bytes) AS total_bytes,
    GROUP_CONCAT(name) AS files
FROM sources
WHERE layer IS NOT NULL
GROUP BY layer
ORDER BY
    CASE layer
        WHEN 'MATTER' THEN 1
        WHEN 'SPACE' THEN 2
        WHEN 'TIME' THEN 3
        ELSE 4
    END;

-- View: Build status summary
CREATE VIEW IF NOT EXISTS v_build_status AS
SELECT
    'sources' AS category,
    COUNT(*) AS total,
    SUM(CASE WHEN o.build_status = 'success' THEN 1 ELSE 0 END) AS built
FROM sources s
LEFT JOIN objects o ON s.id = o.source_id
UNION ALL
SELECT
    'tests' AS category,
    COUNT(*) AS total,
    SUM(CASE WHEN status = 'passed' THEN 1 ELSE 0 END) AS built
FROM tests
UNION ALL
SELECT
    'demos' AS category,
    COUNT(*) AS total,
    SUM(CASE WHEN status = 'passed' THEN 1 ELSE 0 END) AS built
FROM demos;

-- View: Test summary
CREATE VIEW IF NOT EXISTS v_test_summary AS
SELECT
    name,
    status,
    assertions_passed || '/' || assertions_total AS assertions,
    duration_ms,
    last_run
FROM tests
ORDER BY last_run DESC;

-- View: Recent build events
CREATE VIEW IF NOT EXISTS v_recent_builds AS
SELECT * FROM build_events
ORDER BY timestamp DESC
LIMIT 50;

-- Indexes for performance
CREATE INDEX IF NOT EXISTS idx_sources_layer ON sources(layer);
CREATE INDEX IF NOT EXISTS idx_sources_type ON sources(source_type);
CREATE INDEX IF NOT EXISTS idx_objects_source_id ON objects(source_id);
CREATE INDEX IF NOT EXISTS idx_objects_status ON objects(build_status);
CREATE INDEX IF NOT EXISTS idx_tests_status ON tests(status);
CREATE INDEX IF NOT EXISTS idx_demos_phase ON demos(phase);
CREATE INDEX IF NOT EXISTS idx_build_events_type ON build_events(event_type);
CREATE INDEX IF NOT EXISTS idx_build_events_timestamp ON build_events(timestamp);
