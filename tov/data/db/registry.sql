-- registry.db Schema
-- Type registry database for Kingdom Technology specifications
-- Makes Phase 0 TOML specs queryable
--
-- Biblical anchor: "In the beginning was the Word" (John 1:1)
-- The Word defines all things - specs define implementations

-- Specification files table
CREATE TABLE IF NOT EXISTS specs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    key TEXT NOT NULL UNIQUE,              -- e.g., 'ternary' or 'health'
    path TEXT NOT NULL,                     -- relative path from word/core/
    name TEXT NOT NULL,                     -- human-readable name
    version TEXT,                           -- spec version
    created_at TEXT NOT NULL,              -- ISO timestamp
    updated_at TEXT NOT NULL,              -- ISO timestamp
    description TEXT,
    status TEXT DEFAULT 'active' CHECK(status IN ('active', 'deprecated', 'draft'))
);

-- Type definitions table (types defined in specs)
CREATE TABLE IF NOT EXISTS types (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_id INTEGER NOT NULL,               -- FK to specs
    key TEXT NOT NULL,                      -- type key e.g., 'trit_t'
    name TEXT NOT NULL,                     -- human-readable name
    category TEXT NOT NULL,                 -- 'primitive', 'packed', 'compound', 'enum'
    size_bits INTEGER,                      -- size in bits if applicable
    description TEXT,
    created_at TEXT NOT NULL,
    FOREIGN KEY (spec_id) REFERENCES specs(id),
    UNIQUE(spec_id, key)
);

-- Dependency relationships between specs
CREATE TABLE IF NOT EXISTS dependencies (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_id INTEGER NOT NULL,               -- the spec that depends
    depends_on_id INTEGER NOT NULL,         -- the spec it depends on
    dependency_type TEXT DEFAULT 'requires' CHECK(dependency_type IN ('requires', 'includes', 'uses')),
    created_at TEXT NOT NULL,
    FOREIGN KEY (spec_id) REFERENCES specs(id),
    FOREIGN KEY (depends_on_id) REFERENCES specs(id),
    UNIQUE(spec_id, depends_on_id)
);

-- Operations defined in specs (tables, formulas, algorithms)
CREATE TABLE IF NOT EXISTS operations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_id INTEGER NOT NULL,
    key TEXT NOT NULL,                      -- operation key e.g., 'half_adder'
    name TEXT NOT NULL,
    operation_type TEXT NOT NULL,           -- 'table', 'formula', 'algorithm'
    description TEXT,
    created_at TEXT NOT NULL,
    FOREIGN KEY (spec_id) REFERENCES specs(id),
    UNIQUE(spec_id, key)
);

-- Constants defined in specs
CREATE TABLE IF NOT EXISTS constants (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_id INTEGER NOT NULL,
    key TEXT NOT NULL,
    value TEXT NOT NULL,                    -- stored as text, parsed by consumer
    value_type TEXT NOT NULL,               -- 'integer', 'float', 'string', 'array'
    description TEXT,
    created_at TEXT NOT NULL,
    FOREIGN KEY (spec_id) REFERENCES specs(id),
    UNIQUE(spec_id, key)
);

-- Views for common queries

-- View: Dependency graph (which specs depend on which)
CREATE VIEW IF NOT EXISTS v_dependency_graph AS
SELECT
    s1.key AS spec_key,
    s1.name AS spec_name,
    s2.key AS depends_on_key,
    s2.name AS depends_on_name,
    d.dependency_type
FROM dependencies d
JOIN specs s1 ON d.spec_id = s1.id
JOIN specs s2 ON d.depends_on_id = s2.id;

-- View: Root specs (specs with no dependencies)
CREATE VIEW IF NOT EXISTS v_root_specs AS
SELECT s.* FROM specs s
WHERE s.id NOT IN (SELECT DISTINCT spec_id FROM dependencies);

-- View: Types with their specs
CREATE VIEW IF NOT EXISTS v_types_with_specs AS
SELECT
    t.*,
    s.key AS spec_key,
    s.name AS spec_name,
    s.path AS spec_path
FROM types t
JOIN specs s ON t.spec_id = s.id;

-- Indexes for performance
CREATE INDEX IF NOT EXISTS idx_types_spec_id ON types(spec_id);
CREATE INDEX IF NOT EXISTS idx_dependencies_spec_id ON dependencies(spec_id);
CREATE INDEX IF NOT EXISTS idx_dependencies_depends_on ON dependencies(depends_on_id);
CREATE INDEX IF NOT EXISTS idx_operations_spec_id ON operations(spec_id);
CREATE INDEX IF NOT EXISTS idx_constants_spec_id ON constants(spec_id);
