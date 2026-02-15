//omni:code --rust -executable
//omni:key B-tool-seed-platform-rs
//omni:version a-01.00

// ════════════════════════════════════════════════════════════════════════════════
// METADATA
// ════════════════════════════════════════════════════════════════════════════════

//! # seed-platform (Rust)
//!
//! Populate L2 platform database from TOML specs.
//!
//! Reads all TOML specifications under `L2-platform/ladder/`
//! and seeds a SQLite database at `data/platform/database/platform.db`.
//!
//! The triangle: TOML (Configuration) -> SQLite (Data) -> Go/Rust (Code)
//!
//! "And God called the firmament Heaven." -- Genesis 1:8

// ════════════════════════════════════════════════════════════════════════════════
// SETUP
// ════════════════════════════════════════════════════════════════════════════════

use rusqlite::Connection;
use std::collections::HashMap;
use std::path::{Path, PathBuf};

// ────────────────────────────────────────────────────────────────────────────────
// S.1 Schema
// ────────────────────────────────────────────────────────────────────────────────

const SCHEMA: &str = r#"
-- L2 Platform Database Schema
-- "And God called the firmament Heaven." -- Genesis 1:8
--
-- Tables from 9 TOML specs across 3 domains:
--   Health (score, log, dar, diagnostics, restore, provider)
--   Permission (access)
--   Filesystem (contract, types)

PRAGMA journal_mode = WAL;
PRAGMA foreign_keys = ON;

-- specs: Metadata from every TOML spec file
CREATE TABLE IF NOT EXISTS specs (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    key         TEXT    NOT NULL UNIQUE,
    file        TEXT    NOT NULL,
    title       TEXT    NOT NULL,
    type        TEXT    NOT NULL DEFAULT '',
    component   TEXT    NOT NULL DEFAULT '',
    role        TEXT    NOT NULL DEFAULT '',
    domain      TEXT    NOT NULL DEFAULT 'platform',
    layer       TEXT    NOT NULL DEFAULT 'L2',
    version     TEXT    NOT NULL DEFAULT '',
    status      TEXT    NOT NULL DEFAULT 'Active',
    scripture   TEXT    NOT NULL DEFAULT '',
    principle   TEXT    NOT NULL DEFAULT '',
    tags        TEXT    NOT NULL DEFAULT '',
    provides    TEXT    NOT NULL DEFAULT '',
    created     TEXT    NOT NULL DEFAULT '',
    updated     TEXT    NOT NULL DEFAULT ''
);

-- dependencies: Cross-references between specs
CREATE TABLE IF NOT EXISTS dependencies (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_key    TEXT    NOT NULL,
    depends_on  TEXT    NOT NULL,
    relation    TEXT    NOT NULL DEFAULT 'requires',
    UNIQUE(spec_key, depends_on, relation)
);

-- health_levels: 7 health levels from score.toml
CREATE TABLE IF NOT EXISTS health_levels (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    description   TEXT    NOT NULL DEFAULT '',
    hebrew        TEXT    NOT NULL DEFAULT '',
    min_stored    INTEGER NOT NULL,
    max_stored    INTEGER NOT NULL,
    ternary_min   INTEGER NOT NULL,
    ternary_max   INTEGER NOT NULL,
    direction     TEXT    NOT NULL DEFAULT '',
    priority      TEXT    NOT NULL DEFAULT '',
    scripture     TEXT    NOT NULL DEFAULT '',
    emoji         TEXT    NOT NULL DEFAULT '',
    color         TEXT    NOT NULL DEFAULT '',
    ansi_code     TEXT    NOT NULL DEFAULT ''
);

-- health_normalization: Normalization bases from score.toml
CREATE TABLE IF NOT EXISTS health_normalization (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    description   TEXT    NOT NULL DEFAULT '',
    step          INTEGER NOT NULL,
    points        INTEGER NOT NULL,
    use_case      TEXT    NOT NULL DEFAULT '',
    key_points    TEXT    NOT NULL DEFAULT '[]'
);

-- health_scoring: Calculation rules from score.toml
CREATE TABLE IF NOT EXISTS health_scoring (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    rule_name     TEXT    NOT NULL UNIQUE,
    rule_value    TEXT    NOT NULL DEFAULT '',
    description   TEXT    NOT NULL DEFAULT ''
);

-- health_log_actions: Action types from log.toml
CREATE TABLE IF NOT EXISTS health_log_actions (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    description   TEXT    NOT NULL DEFAULT '',
    delta         TEXT    NOT NULL DEFAULT '',
    examples      TEXT    NOT NULL DEFAULT '[]',
    scripture     TEXT    NOT NULL DEFAULT ''
);

-- dar_phases: DAR phases from dar.toml
CREATE TABLE IF NOT EXISTS dar_phases (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    idx           INTEGER NOT NULL,
    balanced      INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT ''
);

-- dar_aspects: DAR aspects from dar.toml
CREATE TABLE IF NOT EXISTS dar_aspects (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    idx           INTEGER NOT NULL,
    balanced      INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT ''
);

-- dar_severity: DAR severity levels from dar.toml
CREATE TABLE IF NOT EXISTS dar_severity (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    idx           INTEGER NOT NULL,
    balanced      INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT ''
);

-- dar_restore_results: Restore outcomes from dar.toml
CREATE TABLE IF NOT EXISTS dar_restore_results (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    value         INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT ''
);

-- dar_restore_strategies: Restore strategies from dar.toml
CREATE TABLE IF NOT EXISTS dar_restore_strategies (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    value         TEXT    NOT NULL DEFAULT '',
    description   TEXT    NOT NULL DEFAULT ''
);

-- permission_states: Permission states from access.toml
CREATE TABLE IF NOT EXISTS permission_states (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    value         INTEGER NOT NULL,
    description   TEXT    NOT NULL DEFAULT '',
    meaning       TEXT    NOT NULL DEFAULT ''
);

-- permission_action_categories: Action categories from access.toml
CREATE TABLE IF NOT EXISTS permission_action_categories (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    name             TEXT    NOT NULL UNIQUE,
    description      TEXT    NOT NULL DEFAULT '',
    examples         TEXT    NOT NULL DEFAULT '[]',
    minimum_state    TEXT    NOT NULL DEFAULT '',
    health_threshold INTEGER NOT NULL DEFAULT 0
);

-- permission_gating: Health->permission mapping from access.toml
CREATE TABLE IF NOT EXISTS permission_gating (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    health_level     TEXT    NOT NULL UNIQUE,
    permission_state TEXT    NOT NULL DEFAULT ''
);

-- filesystem_biblical_extensions: Biblical file types from types.toml
CREATE TABLE IF NOT EXISTS filesystem_biblical_extensions (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    NOT NULL UNIQUE,
    hebrew        TEXT    NOT NULL DEFAULT '',
    meaning       TEXT    NOT NULL DEFAULT '',
    extensions    TEXT    NOT NULL DEFAULT '[]'
);

-- Indexes for common queries
CREATE INDEX IF NOT EXISTS idx_specs_domain ON specs(domain);
CREATE INDEX IF NOT EXISTS idx_health_levels_direction ON health_levels(direction);
CREATE INDEX IF NOT EXISTS idx_dar_phases_balanced ON dar_phases(balanced);
CREATE INDEX IF NOT EXISTS idx_permission_gating_state ON permission_gating(permission_state);
CREATE INDEX IF NOT EXISTS idx_dependencies_spec ON dependencies(spec_key);
"#;

// ────────────────────────────────────────────────────────────────────────────────
// S.2 Types
// ────────────────────────────────────────────────────────────────────────────────

type TomlMap = toml::map::Map<String, toml::Value>;

// ════════════════════════════════════════════════════════════════════════════════
// BODY
// ════════════════════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────────────────────
// B.1 Helpers
// ────────────────────────────────────────────────────────────────────────────────

fn get_table<'a>(map: &'a TomlMap, key: &str) -> Option<&'a TomlMap> {
    map.get(key)?.as_table()
}

fn get_str<'a>(map: &'a TomlMap, key: &str) -> &'a str {
    map.get(key).and_then(|v| v.as_str()).unwrap_or("")
}

fn get_i64(map: &TomlMap, key: &str) -> i64 {
    map.get(key).and_then(|v| v.as_integer()).unwrap_or(0)
}

fn get_str_array(map: &TomlMap, key: &str) -> Vec<String> {
    map.get(key)
        .and_then(|v| v.as_array())
        .map(|arr| {
            arr.iter()
                .filter_map(|v| v.as_str().map(String::from))
                .collect()
        })
        .unwrap_or_default()
}

fn get_i64_array(map: &TomlMap, key: &str) -> Vec<i64> {
    map.get(key)
        .and_then(|v| v.as_array())
        .map(|arr| arr.iter().filter_map(|v| v.as_integer()).collect())
        .unwrap_or_default()
}

fn to_json_strings(arr: &[String]) -> String {
    serde_json::to_string(arr).unwrap_or_else(|_| "[]".to_string())
}

fn to_json_ints(arr: &[i64]) -> String {
    serde_json::to_string(arr).unwrap_or_else(|_| "[]".to_string())
}

// ────────────────────────────────────────────────────────────────────────────────
// B.2 File cache & root finder
// ────────────────────────────────────────────────────────────────────────────────

fn find_root() -> Option<PathBuf> {
    let mut dir = std::env::current_dir().ok()?;
    loop {
        if dir.join("Cargo.toml").exists() && dir.join("go.mod").exists() {
            return Some(dir);
        }
        if !dir.pop() {
            return None;
        }
    }
}

fn load_file<'a>(
    cache: &'a mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
    rel_path: &str,
) -> Option<&'a TomlMap> {
    let path = ladder_dir.join(rel_path);
    if !cache.contains_key(&path) {
        let content = match std::fs::read_to_string(&path) {
            Ok(c) => c,
            Err(e) => {
                eprintln!("Warning: cannot read {rel_path}: {e}");
                return None;
            }
        };
        let table: TomlMap = match toml::from_str::<toml::Table>(&content) {
            Ok(t) => t,
            Err(e) => {
                eprintln!("Warning: parsing {rel_path}: {e}");
                return None;
            }
        };
        cache.insert(path.clone(), table);
    }
    cache.get(&path)
}

// ────────────────────────────────────────────────────────────────────────────────
// B.3 Spec seeding (metadata from all TOML files)
// ────────────────────────────────────────────────────────────────────────────────

fn seed_spec(conn: &Connection, path: &Path) {
    let content = match std::fs::read_to_string(path) {
        Ok(c) => c,
        Err(e) => {
            eprintln!("Warning: reading {}: {e}", path.display());
            return;
        }
    };
    let table: TomlMap = match toml::from_str::<toml::Table>(&content) {
        Ok(t) => t,
        Err(e) => {
            eprintln!("Warning: parsing {}: {e}", path.display());
            return;
        }
    };

    let pragma = get_table(&table, "_pragma").cloned().unwrap_or_default();
    let meta = get_table(&table, "_metadata").cloned().unwrap_or_default();

    let p1 = get_table(&pragma, "P1_core").cloned().unwrap_or_default();
    let key = {
        let k = get_str(&p1, "key");
        if k.is_empty() {
            path.file_name()
                .unwrap_or_default()
                .to_string_lossy()
                .to_string()
        } else {
            k.to_string()
        }
    };

    let m1 = get_table(&meta, "M1_identity").cloned().unwrap_or_default();
    let m2 = get_table(&meta, "M2_state").cloned().unwrap_or_default();
    let m4 = get_table(&meta, "M4_grounding").cloned().unwrap_or_default();
    let m5 = get_table(&meta, "M5_dependencies").cloned().unwrap_or_default();
    let m7 = get_table(&meta, "M7_classification").cloned().unwrap_or_default();

    let p3 = get_table(&pragma, "P3_instance").cloned().unwrap_or_default();
    let p5 = get_table(&pragma, "P5_summary").cloned().unwrap_or_default();

    let provides = get_str_array(&p3, "provides").join(", ");

    let title = {
        let t = get_str(&m1, "title");
        if t.is_empty() {
            get_str(&p5, "title")
        } else {
            t
        }
    };

    let file = {
        let f = get_str(&m1, "file");
        if f.is_empty() {
            path.file_name()
                .unwrap_or_default()
                .to_string_lossy()
                .to_string()
        } else {
            f.to_string()
        }
    };

    if let Err(e) = conn.execute(
        "INSERT OR REPLACE INTO specs \
         (key, file, title, type, component, role, domain, layer, version, status, \
          scripture, principle, tags, provides, created, updated) \
         VALUES (?1,?2,?3,?4,?5,?6,'platform','L2',?7,?8,?9,?10,?11,?12,?13,?14)",
        rusqlite::params![
            key,
            file,
            title,
            get_str(&m1, "type"),
            get_str(&m1, "component"),
            get_str(&m1, "role"),
            get_str(&m2, "version"),
            get_str(&m2, "status"),
            get_str(&m4, "scripture"),
            get_str(&m4, "principle"),
            get_str(&m7, "tags"),
            provides,
            get_str(&m2, "created"),
            get_str(&m2, "updated"),
        ],
    ) {
        eprintln!("Warning: inserting spec {key}: {e}");
    }

    // Dependencies
    for dep in get_str_array(&m5, "requires") {
        let _ = conn.execute(
            "INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) \
             VALUES (?1, ?2, 'requires')",
            rusqlite::params![key, dep],
        );
    }
    for dep in get_str_array(&m5, "consumers") {
        let _ = conn.execute(
            "INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) \
             VALUES (?1, ?2, 'consumed_by')",
            rusqlite::params![dep, key],
        );
    }
    let derives = get_str(&m5, "derives_from");
    if !derives.is_empty() {
        let _ = conn.execute(
            "INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) \
             VALUES (?1, ?2, 'derives_from')",
            rusqlite::params![key, derives],
        );
    }

    println!("  spec: {key}");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.4 Health levels from score.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_health_levels(conn: &Connection, cache: &mut HashMap<PathBuf, TomlMap>, ladder_dir: &Path) {
    let raw = match load_file(cache, ladder_dir, "os/health/score.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let levels = get_table(&raw, "levels").cloned().unwrap_or_default();
    let display = get_table(&raw, "display").cloned().unwrap_or_default();

    let level_names = [
        "broken", "wanting", "lacking", "even", "sound", "whole", "perfect",
    ];
    let mut count = 0;

    for name in &level_names {
        let lvl = match get_table(&levels, name) {
            Some(l) => l.clone(),
            None => continue,
        };

        let disp = get_table(&display, name).cloned().unwrap_or_default();
        let emoji = get_str(&disp, "emoji").to_string();
        let color = get_str(&disp, "color").to_string();
        let ansi_code = get_str(&disp, "ansi_code").to_string();

        let ternary_range = get_i64_array(&lvl, "ternary_range");
        let (ternary_min, ternary_max) = if ternary_range.len() >= 2 {
            (ternary_range[0], ternary_range[1])
        } else {
            (0, 0)
        };

        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO health_levels \
             (name, description, hebrew, min_stored, max_stored, \
              ternary_min, ternary_max, direction, priority, scripture, \
              emoji, color, ansi_code) \
             VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13)",
            rusqlite::params![
                name,
                get_str(&lvl, "description"),
                get_str(&lvl, "hebrew"),
                get_i64(&lvl, "min_stored"),
                get_i64(&lvl, "max_stored"),
                ternary_min,
                ternary_max,
                get_str(&lvl, "direction"),
                get_str(&lvl, "priority"),
                get_str(&lvl, "scripture"),
                emoji,
                color,
                ansi_code,
            ],
        ) {
            eprintln!("Warning: inserting health level {name}: {e}");
        }
        count += 1;
    }

    println!("  health_levels: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.5 Health normalization bases from score.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_health_normalization(
    conn: &Connection,
    cache: &mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
) {
    let raw = match load_file(cache, ladder_dir, "os/health/score.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let health = get_table(&raw, "health").cloned().unwrap_or_default();
    let normalized = get_table(&health, "normalized").cloned().unwrap_or_default();
    let bases = match get_table(&normalized, "bases") {
        Some(b) => b.clone(),
        None => return,
    };

    let base_names = ["base1", "base5", "base10", "base20", "base25", "base50"];
    let mut count = 0;

    for name in &base_names {
        let base = match get_table(&bases, name) {
            Some(b) => b.clone(),
            None => continue,
        };

        let key_points = to_json_ints(&get_i64_array(&base, "key_points"));

        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO health_normalization \
             (name, description, step, points, use_case, key_points) \
             VALUES (?1,?2,?3,?4,?5,?6)",
            rusqlite::params![
                name,
                get_str(&base, "description"),
                get_i64(&base, "step"),
                get_i64(&base, "points"),
                get_str(&base, "use_case"),
                key_points,
            ],
        ) {
            eprintln!("Warning: inserting normalization {name}: {e}");
        }
        count += 1;
    }

    println!("  health_normalization: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.6 Health scoring rules from score.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_health_scoring(
    conn: &Connection,
    cache: &mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
) {
    let raw = match load_file(cache, ladder_dir, "os/health/score.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let mut count = 0;

    let mut insert_rule = |name: &str, value: &str, desc: &str| {
        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO health_scoring (rule_name, rule_value, description) \
             VALUES (?1, ?2, ?3)",
            rusqlite::params![name, value, desc],
        ) {
            eprintln!("Warning: inserting scoring rule {name}: {e}");
        }
        count += 1;
    };

    // Storage fundamentals
    let health = get_table(&raw, "health").cloned().unwrap_or_default();
    insert_rule(
        "storage_min",
        &get_i64(&health, "storage_min").to_string(),
        "Minimum stored value",
    );
    insert_rule(
        "storage_max",
        &get_i64(&health, "storage_max").to_string(),
        "Maximum stored value",
    );
    insert_rule(
        "storage_default",
        &get_i64(&health, "storage_default").to_string(),
        "Default stored value (center)",
    );

    // Ternary interpretation
    let ternary = get_table(&health, "ternary").cloned().unwrap_or_default();
    insert_rule(
        "ternary_min",
        &get_i64(&ternary, "interpretation_min").to_string(),
        "Minimum ternary value",
    );
    insert_rule(
        "ternary_max",
        &get_i64(&ternary, "interpretation_max").to_string(),
        "Maximum ternary value",
    );
    insert_rule(
        "ternary_center",
        &get_i64(&ternary, "interpretation_center").to_string(),
        "Ternary center (even)",
    );

    // Default normalization base
    let normalized = get_table(&health, "normalized").cloned().unwrap_or_default();
    insert_rule(
        "default_base",
        &get_i64(&normalized, "default_base").to_string(),
        "Default normalization base",
    );

    // Calculation rules
    let calc = get_table(&raw, "calculation").cloned().unwrap_or_default();
    let init = get_table(&calc, "initialization").cloned().unwrap_or_default();
    insert_rule(
        "init_default",
        get_str(&init, "default_value"),
        "Default health value for new files",
    );

    let increase = get_table(&calc, "increase").cloned().unwrap_or_default();
    insert_rule(
        "increase_max_per_event",
        &get_i64(&increase, "max_increase_per_event").to_string(),
        "Max increase per event",
    );
    insert_rule(
        "increase_ceiling",
        &get_i64(&increase, "ceiling").to_string(),
        "Ceiling (cannot exceed)",
    );

    let decrease = get_table(&calc, "decrease").cloned().unwrap_or_default();
    insert_rule(
        "decrease_max_per_event",
        &get_i64(&decrease, "max_decrease_per_event").to_string(),
        "Max decrease per event",
    );
    insert_rule(
        "decrease_floor",
        &get_i64(&decrease, "floor").to_string(),
        "Floor (cannot go below)",
    );

    let staleness = get_table(&calc, "staleness").cloned().unwrap_or_default();
    insert_rule(
        "decay_rate",
        &get_i64(&staleness, "decay_rate").to_string(),
        "Decay per period",
    );
    insert_rule(
        "decay_period",
        get_str(&staleness, "decay_period"),
        "Decay period",
    );
    insert_rule(
        "decay_minimum",
        &get_i64(&staleness, "minimum_after_decay").to_string(),
        "Min after decay (never below even)",
    );

    // Hierarchy
    let hier = get_table(&raw, "hierarchy").cloned().unwrap_or_default();
    let agg = get_table(&hier, "aggregation").cloned().unwrap_or_default();
    insert_rule("hierarchy_method", get_str(&agg, "method"), "Aggregation method");

    println!("  health_scoring: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.7 Health log actions from log.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_health_log_actions(
    conn: &Connection,
    cache: &mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
) {
    let raw = match load_file(cache, ladder_dir, "os/health/log.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let actions = match get_table(&raw, "actions") {
        Some(a) => a.clone(),
        None => return,
    };

    let action_names = ["success", "failure", "neutral", "recovery", "reset"];
    let mut count = 0;

    for name in &action_names {
        let act = match get_table(&actions, name) {
            Some(a) => a.clone(),
            None => continue,
        };

        // Delta can be int or string ("special" for reset)
        let delta = if let Some(d) = act.get("delta") {
            match d {
                toml::Value::Integer(i) => i.to_string(),
                toml::Value::String(s) => s.clone(),
                other => format!("{other}"),
            }
        } else if act.get("delta_range").is_some() {
            "range".to_string()
        } else {
            String::new()
        };

        let examples = to_json_strings(&get_str_array(&act, "examples"));

        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO health_log_actions \
             (name, description, delta, examples, scripture) \
             VALUES (?1,?2,?3,?4,?5)",
            rusqlite::params![
                name,
                get_str(&act, "description"),
                delta,
                examples,
                get_str(&act, "scripture"),
            ],
        ) {
            eprintln!("Warning: inserting log action {name}: {e}");
        }
        count += 1;
    }

    println!("  health_log_actions: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.8 DAR phases, aspects, severity from dar.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_dar_phases(conn: &Connection, cache: &mut HashMap<PathBuf, TomlMap>, ladder_dir: &Path) {
    let raw = match load_file(cache, ladder_dir, "os/health/dar.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let dar = get_table(&raw, "dar").cloned().unwrap_or_default();

    // Phases
    let phases = get_table(&dar, "phases").cloned().unwrap_or_default();
    let mut phase_count = 0;
    for name in &["DETECT", "ASSESS", "RESTORE"] {
        let entry = match get_table(&phases, name) {
            Some(e) => e.clone(),
            None => continue,
        };
        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO dar_phases (name, idx, balanced, description) \
             VALUES (?1,?2,?3,?4)",
            rusqlite::params![
                name,
                get_i64(&entry, "index"),
                get_i64(&entry, "balanced"),
                get_str(&entry, "description"),
            ],
        ) {
            eprintln!("Warning: inserting DAR phase {name}: {e}");
        }
        phase_count += 1;
    }
    println!("  dar_phases: {phase_count} entries");

    // Aspects
    let aspects = get_table(&dar, "aspects").cloned().unwrap_or_default();
    let mut aspect_count = 0;
    for name in &["ENTRY", "PROCESS", "EXIT"] {
        let entry = match get_table(&aspects, name) {
            Some(e) => e.clone(),
            None => continue,
        };
        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO dar_aspects (name, idx, balanced, description) \
             VALUES (?1,?2,?3,?4)",
            rusqlite::params![
                name,
                get_i64(&entry, "index"),
                get_i64(&entry, "balanced"),
                get_str(&entry, "description"),
            ],
        ) {
            eprintln!("Warning: inserting DAR aspect {name}: {e}");
        }
        aspect_count += 1;
    }
    println!("  dar_aspects: {aspect_count} entries");

    // Severity
    let severity = get_table(&dar, "severity").cloned().unwrap_or_default();
    let mut sev_count = 0;
    for name in &["MILD", "MEDIUM", "CRITICAL"] {
        let entry = match get_table(&severity, name) {
            Some(e) => e.clone(),
            None => continue,
        };
        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO dar_severity (name, idx, balanced, description) \
             VALUES (?1,?2,?3,?4)",
            rusqlite::params![
                name,
                get_i64(&entry, "index"),
                get_i64(&entry, "balanced"),
                get_str(&entry, "description"),
            ],
        ) {
            eprintln!("Warning: inserting DAR severity {name}: {e}");
        }
        sev_count += 1;
    }
    println!("  dar_severity: {sev_count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.9 DAR restore results from dar.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_dar_restore_results(
    conn: &Connection,
    cache: &mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
) {
    let raw = match load_file(cache, ladder_dir, "os/health/dar.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let dar = get_table(&raw, "dar").cloned().unwrap_or_default();
    let results = match get_table(&dar, "restore_result") {
        Some(r) => r.clone(),
        None => return,
    };

    let names = [
        "SUCCESS", "PARTIAL", "DEGRADED", "DEFERRED", "BLOCKED", "FAILED", "FATAL",
    ];
    let mut count = 0;

    for name in &names {
        let entry = match get_table(&results, name) {
            Some(e) => e.clone(),
            None => continue,
        };
        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO dar_restore_results (name, value, description) \
             VALUES (?1,?2,?3)",
            rusqlite::params![name, get_i64(&entry, "value"), get_str(&entry, "description"),],
        ) {
            eprintln!("Warning: inserting restore result {name}: {e}");
        }
        count += 1;
    }

    println!("  dar_restore_results: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.10 DAR restore strategies from dar.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_dar_restore_strategies(
    conn: &Connection,
    cache: &mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
) {
    let raw = match load_file(cache, ladder_dir, "os/health/dar.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let dar = get_table(&raw, "dar").cloned().unwrap_or_default();
    let strategies = match get_table(&dar, "restore_strategy") {
        Some(s) => s.clone(),
        None => return,
    };

    let names = [
        "NONE",
        "FILL_DEFAULTS",
        "LOCATE_FILES",
        "CREATE_DIRS",
        "CLAMP_VALUES",
        "FALLBACK",
        "DEGRADE",
    ];
    let mut count = 0;

    for name in &names {
        let entry = match get_table(&strategies, name) {
            Some(e) => e.clone(),
            None => continue,
        };
        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO dar_restore_strategies (name, value, description) \
             VALUES (?1,?2,?3)",
            rusqlite::params![name, get_str(&entry, "value"), get_str(&entry, "description"),],
        ) {
            eprintln!("Warning: inserting restore strategy {name}: {e}");
        }
        count += 1;
    }

    println!("  dar_restore_strategies: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.11 Permission states from access.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_permission_states(
    conn: &Connection,
    cache: &mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
) {
    let raw = match load_file(cache, ladder_dir, "os/permission/access.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let states = match get_table(&raw, "states") {
        Some(s) => s.clone(),
        None => return,
    };

    let state_names = ["denied", "deferred", "granted"];
    let mut count = 0;

    for name in &state_names {
        let entry = match get_table(&states, name) {
            Some(e) => e.clone(),
            None => continue,
        };
        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO permission_states (name, value, description, meaning) \
             VALUES (?1,?2,?3,?4)",
            rusqlite::params![
                name,
                get_i64(&entry, "value"),
                get_str(&entry, "description"),
                get_str(&entry, "meaning"),
            ],
        ) {
            eprintln!("Warning: inserting permission state {name}: {e}");
        }
        count += 1;
    }

    println!("  permission_states: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.12 Permission action categories from access.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_permission_action_categories(
    conn: &Connection,
    cache: &mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
) {
    let raw = match load_file(cache, ladder_dir, "os/permission/access.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let actions = get_table(&raw, "actions").cloned().unwrap_or_default();
    let categories = match get_table(&actions, "categories") {
        Some(c) => c.clone(),
        None => return,
    };

    let cat_names = ["safe", "standard", "sensitive", "critical"];
    let mut count = 0;

    for name in &cat_names {
        let cat = match get_table(&categories, name) {
            Some(c) => c.clone(),
            None => continue,
        };

        let examples = to_json_strings(&get_str_array(&cat, "examples"));

        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO permission_action_categories \
             (name, description, examples, minimum_state, health_threshold) \
             VALUES (?1,?2,?3,?4,?5)",
            rusqlite::params![
                name,
                get_str(&cat, "description"),
                examples,
                get_str(&cat, "minimum_state"),
                get_i64(&cat, "health_threshold"),
            ],
        ) {
            eprintln!("Warning: inserting action category {name}: {e}");
        }
        count += 1;
    }

    println!("  permission_action_categories: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.13 Permission gating from access.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_permission_gating(
    conn: &Connection,
    cache: &mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
) {
    let raw = match load_file(cache, ladder_dir, "os/permission/access.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let gating = get_table(&raw, "gating").cloned().unwrap_or_default();
    let mapping = match get_table(&gating, "mapping") {
        Some(m) => m.clone(),
        None => return,
    };

    let health_levels = [
        "broken", "wanting", "lacking", "even", "sound", "whole", "perfect",
    ];
    let mut count = 0;

    for level in &health_levels {
        let state = get_str(&mapping, level);
        if state.is_empty() {
            continue;
        }

        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO permission_gating (health_level, permission_state) \
             VALUES (?1,?2)",
            rusqlite::params![level, state],
        ) {
            eprintln!("Warning: inserting gating {level}: {e}");
        }
        count += 1;
    }

    println!("  permission_gating: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.14 Filesystem biblical extensions from types.toml
// ────────────────────────────────────────────────────────────────────────────────

fn seed_filesystem_biblical_extensions(
    conn: &Connection,
    cache: &mut HashMap<PathBuf, TomlMap>,
    ladder_dir: &Path,
) {
    let raw = match load_file(cache, ladder_dir, "filesystem/types.toml") {
        Some(r) => r.clone(),
        None => return,
    };

    let biblical = match get_table(&raw, "biblical") {
        Some(b) => b.clone(),
        None => return,
    };

    let names = ["dabar", "chok", "sefer", "tavnit", "zera", "tselem"];
    let mut count = 0;

    for name in &names {
        let entry = match get_table(&biblical, name) {
            Some(e) => e.clone(),
            None => continue,
        };

        let exts = to_json_strings(&get_str_array(&entry, "extensions"));

        if let Err(e) = conn.execute(
            "INSERT OR IGNORE INTO filesystem_biblical_extensions \
             (name, hebrew, meaning, extensions) \
             VALUES (?1,?2,?3,?4)",
            rusqlite::params![
                name,
                get_str(&entry, "hebrew"),
                get_str(&entry, "meaning"),
                exts,
            ],
        ) {
            eprintln!("Warning: inserting biblical extension {name}: {e}");
        }
        count += 1;
    }

    println!("  filesystem_biblical_extensions: {count} entries");
}

// ────────────────────────────────────────────────────────────────────────────────
// B.15 Summary
// ────────────────────────────────────────────────────────────────────────────────

fn print_summary(conn: &Connection) {
    println!("\n--- Platform Database Summary ---");
    let tables = [
        "specs",
        "dependencies",
        "health_levels",
        "health_normalization",
        "health_scoring",
        "health_log_actions",
        "dar_phases",
        "dar_aspects",
        "dar_severity",
        "dar_restore_results",
        "dar_restore_strategies",
        "permission_states",
        "permission_action_categories",
        "permission_gating",
        "filesystem_biblical_extensions",
    ];
    let mut total = 0;
    for t in &tables {
        let count: i64 = conn
            .query_row(&format!("SELECT COUNT(*) FROM {t}"), [], |row| row.get(0))
            .unwrap_or(0);
        println!("  {t:<35} {count} rows");
        total += count;
    }
    println!("  {:<35} {total} rows", "TOTAL");
}

// ════════════════════════════════════════════════════════════════════════════════
// CLOSING -- Main entry point
// ════════════════════════════════════════════════════════════════════════════════

fn main() {
    let root =
        find_root().expect("Cannot find b-word root. Run from within the b-word directory tree.");
    let db_path = root.join("data/platform/database/platform.db");
    println!("Seeding platform database: {}", db_path.display());

    // Ensure directory exists
    if let Some(parent) = db_path.parent() {
        let _ = std::fs::create_dir_all(parent);
    }

    // Remove existing database to rebuild fresh
    let _ = std::fs::remove_file(&db_path);

    let conn = Connection::open(&db_path).expect("open database");
    conn.execute_batch(SCHEMA).expect("create schema");

    let ladder_dir = root.join("L2-platform/ladder");

    // Seed specs from all 9 TOML files
    let spec_files = [
        "os/health/score.toml",
        "os/health/log.toml",
        "os/health/dar.toml",
        "os/health/diagnostics.toml",
        "os/health/restore.toml",
        "os/health/provider.toml",
        "os/permission/access.toml",
        "filesystem/contract.toml",
        "filesystem/types.toml",
    ];
    for f in &spec_files {
        let path = ladder_dir.join(f);
        if path.exists() {
            seed_spec(&conn, &path);
        }
    }

    // File cache for domain-specific seeding functions
    let mut cache: HashMap<PathBuf, TomlMap> = HashMap::new();

    // Seed domain-specific tables
    seed_health_levels(&conn, &mut cache, &ladder_dir);
    seed_health_normalization(&conn, &mut cache, &ladder_dir);
    seed_health_scoring(&conn, &mut cache, &ladder_dir);
    seed_health_log_actions(&conn, &mut cache, &ladder_dir);
    seed_dar_phases(&conn, &mut cache, &ladder_dir);
    seed_dar_restore_results(&conn, &mut cache, &ladder_dir);
    seed_dar_restore_strategies(&conn, &mut cache, &ladder_dir);
    seed_permission_states(&conn, &mut cache, &ladder_dir);
    seed_permission_action_categories(&conn, &mut cache, &ladder_dir);
    seed_permission_gating(&conn, &mut cache, &ladder_dir);
    seed_filesystem_biblical_extensions(&conn, &mut cache, &ladder_dir);

    println!("\nDone.");
    print_summary(&conn);
}
