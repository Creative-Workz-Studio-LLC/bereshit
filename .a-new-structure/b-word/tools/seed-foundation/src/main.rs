//omni:code --rust -executable
//omni:key B-tool-seed-foundation-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # seed-foundation (Rust)
//!
//! Populate L0 foundation database from TOML specs.
//!
//! Reads all TOML specifications under `L0-universal/ladder/foundation/`
//! and seeds a SQLite database at `data/foundation/database/foundation.db`.
//!
//! The triangle: TOML (Configuration) → SQLite (Data) → Go/Rust (Code)
//!
//! "In the beginning God created the heaven and the earth." — Genesis 1:1

// ============================================================================
// SETUP
// ============================================================================

use rusqlite::Connection;
use std::collections::HashMap;
use std::path::{Path, PathBuf};
use walkdir::WalkDir;

// ────────────────────────────────────────────────────────────────
// S.1 Schema
// ────────────────────────────────────────────────────────────────

const SCHEMA: &str = r#"
PRAGMA journal_mode = WAL;
PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS specs (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    key         TEXT    NOT NULL UNIQUE,
    file        TEXT    NOT NULL,
    title       TEXT    NOT NULL,
    type        TEXT    NOT NULL DEFAULT '',
    component   TEXT    NOT NULL DEFAULT '',
    role        TEXT    NOT NULL DEFAULT '',
    domain      TEXT    NOT NULL DEFAULT '',
    layer       TEXT    NOT NULL DEFAULT 'L0',
    version     TEXT    NOT NULL DEFAULT '',
    status      TEXT    NOT NULL DEFAULT 'Active',
    scripture   TEXT    NOT NULL DEFAULT '',
    principle   TEXT    NOT NULL DEFAULT '',
    tags        TEXT    NOT NULL DEFAULT '',
    provides    TEXT    NOT NULL DEFAULT '',
    created     TEXT    NOT NULL DEFAULT '',
    updated     TEXT    NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS type_families (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL UNIQUE,
    description TEXT    NOT NULL DEFAULT '',
    scripture   TEXT    NOT NULL DEFAULT '',
    purpose     TEXT    NOT NULL DEFAULT '',
    count       INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS types (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL UNIQUE,
    family      TEXT    NOT NULL REFERENCES type_families(name),
    size_trits  INTEGER NOT NULL DEFAULT 0,
    size_bits   INTEGER NOT NULL DEFAULT 0,
    min_value   TEXT    NOT NULL DEFAULT '',
    max_value   TEXT    NOT NULL DEFAULT '',
    default_val TEXT    NOT NULL DEFAULT '0',
    nullable    INTEGER NOT NULL DEFAULT 0,
    signed      INTEGER NOT NULL DEFAULT 1,
    states      TEXT    NOT NULL DEFAULT '',
    description TEXT    NOT NULL DEFAULT '',
    layer_role  TEXT    NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS operations (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL,
    category    TEXT    NOT NULL,
    description TEXT    NOT NULL DEFAULT '',
    input_size  INTEGER NOT NULL DEFAULT 1,
    table_data  TEXT    NOT NULL DEFAULT '',
    properties  TEXT    NOT NULL DEFAULT '',
    UNIQUE(name, category)
);

CREATE TABLE IF NOT EXISTS books (
    id           INTEGER PRIMARY KEY,
    name         TEXT    NOT NULL UNIQUE,
    abbreviation TEXT    NOT NULL,
    chapters     INTEGER NOT NULL,
    verses       INTEGER NOT NULL,
    range_start  INTEGER NOT NULL,
    range_end    INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS ternary_scales (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL UNIQUE,
    trit_count  INTEGER NOT NULL,
    states      TEXT    NOT NULL,
    bytes       INTEGER NOT NULL,
    layer       TEXT    NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS dependencies (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_key    TEXT    NOT NULL,
    depends_on  TEXT    NOT NULL,
    relation    TEXT    NOT NULL DEFAULT 'requires',
    UNIQUE(spec_key, depends_on, relation)
);

CREATE TABLE IF NOT EXISTS web_variants (
    trite       INTEGER PRIMARY KEY,
    book        TEXT    NOT NULL,
    chapter     INTEGER NOT NULL,
    verse       INTEGER NOT NULL,
    summary     TEXT    NOT NULL DEFAULT ''
);

CREATE INDEX IF NOT EXISTS idx_specs_domain ON specs(domain);
CREATE INDEX IF NOT EXISTS idx_specs_tags ON specs(tags);
CREATE INDEX IF NOT EXISTS idx_types_family ON types(family);
CREATE INDEX IF NOT EXISTS idx_operations_category ON operations(category);
CREATE INDEX IF NOT EXISTS idx_books_abbreviation ON books(abbreviation);
CREATE INDEX IF NOT EXISTS idx_dependencies_spec ON dependencies(spec_key);
"#;

// ────────────────────────────────────────────────────────────────
// S.2 Types
// ────────────────────────────────────────────────────────────────

type TomlMap = toml::map::Map<String, toml::Value>;

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Helpers
// ────────────────────────────────────────────────────────────────

fn get_table<'a>(map: &'a TomlMap, key: &str) -> Option<&'a TomlMap> {
    map.get(key)?.as_table()
}

fn get_str<'a>(map: &'a TomlMap, key: &str) -> &'a str {
    map.get(key).and_then(|v| v.as_str()).unwrap_or("")
}

fn get_i64(map: &TomlMap, key: &str) -> i64 {
    map.get(key).and_then(|v| v.as_integer()).unwrap_or(0)
}

fn get_bool(map: &TomlMap, key: &str) -> bool {
    map.get(key).and_then(|v| v.as_bool()).unwrap_or(false)
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
        .map(|arr| {
            arr.iter()
                .filter_map(|v| v.as_integer())
                .collect()
        })
        .unwrap_or_default()
}

fn value_to_string(v: &toml::Value) -> String {
    match v {
        toml::Value::String(s) => s.clone(),
        toml::Value::Integer(i) => i.to_string(),
        toml::Value::Float(f) => f.to_string(),
        toml::Value::Boolean(b) => b.to_string(),
        toml::Value::Array(arr) => {
            let items: Vec<String> = arr.iter().map(value_to_string).collect();
            format!("[{}]", items.join(", "))
        }
        _ => format!("{v}"),
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Root finder
// ────────────────────────────────────────────────────────────────

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

// ────────────────────────────────────────────────────────────────
// B.3 Spec seeding
// ────────────────────────────────────────────────────────────────

fn seed_spec(conn: &Connection, path: &Path, domain: &str) {
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
            eprintln!("Warning: parsing {} ({}b): {e}", path.display(), content.len());
            return;
        }
    };

    let pragma = get_table(&table, "_pragma").cloned().unwrap_or_default();
    let meta = get_table(&table, "_metadata").cloned().unwrap_or_default();

    let p1 = get_table(&pragma, "P1_core").cloned().unwrap_or_default();
    let key = {
        let k = get_str(&p1, "key");
        if k.is_empty() {
            path.file_name().unwrap_or_default().to_string_lossy().to_string()
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
        if t.is_empty() { get_str(&p5, "title") } else { t }
    };

    let file = {
        let f = get_str(&m1, "file");
        if f.is_empty() {
            path.file_name().unwrap_or_default().to_string_lossy().to_string()
        } else {
            f.to_string()
        }
    };

    if let Err(e) = conn.execute(
        "INSERT OR REPLACE INTO specs \
         (key, file, title, type, component, role, domain, layer, version, status, \
          scripture, principle, tags, provides, created, updated) \
         VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13,?14,?15,?16)",
        rusqlite::params![
            key,
            file,
            title,
            get_str(&m1, "type"),
            get_str(&m1, "component"),
            get_str(&m1, "role"),
            domain,
            get_str(&p3, "layer"),
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
    seed_dependencies(conn, &key, &m5);
}

fn seed_dependencies(conn: &Connection, spec_key: &str, m5: &TomlMap) {
    for dep in get_str_array(m5, "requires") {
        let _ = conn.execute(
            "INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?1, ?2, 'requires')",
            rusqlite::params![spec_key, dep],
        );
    }
    for dep in get_str_array(m5, "consumers") {
        let _ = conn.execute(
            "INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?1, ?2, 'consumed_by')",
            rusqlite::params![dep, spec_key],
        );
    }
    let derives = get_str(m5, "derives_from");
    if !derives.is_empty() {
        let _ = conn.execute(
            "INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?1, ?2, 'derives_from')",
            rusqlite::params![spec_key, derives],
        );
    }
}

fn seed_domain(conn: &Connection, foundation_dir: &Path, domain: &str) -> usize {
    let domain_dir = foundation_dir.join(domain);
    let mut count = 0;

    for entry in WalkDir::new(&domain_dir)
        .max_depth(3) // domain → subdir → sub-subdir (current max is 2)
        .into_iter()
        .filter_map(|e| e.ok())
    {
        let path = entry.path();
        if path.is_file() && path.extension().is_some_and(|e| e == "toml") {
            seed_spec(conn, path, domain);
            count += 1;
        }
    }

    println!("  {domain}: {count} specs");
    count
}

// ────────────────────────────────────────────────────────────────
// B.4 Type families & types
// ────────────────────────────────────────────────────────────────

fn seed_type_families(conn: &Connection, foundation_dir: &Path) {
    let path = foundation_dir.join("types/primitives/primitives-catalog.toml");
    let content = match std::fs::read_to_string(&path) {
        Ok(c) => c,
        Err(e) => {
            eprintln!("Warning: cannot read primitives-catalog.toml: {e}");
            return;
        }
    };
    let table: TomlMap = match toml::from_str::<toml::Table>(&content) {
        Ok(t) => t,
        Err(_) => return,
    };

    let cc = get_table(&table, "cc").cloned().unwrap_or_default();
    let identity = get_table(&cc, "identity").cloned().unwrap_or_default();
    let catalog = get_table(&identity, "catalog").cloned().unwrap_or_default();

    let families = [
        "storage", "trit", "integer", "float", "text",
        "boolean", "collections", "special", "decimal", "temporal",
    ];

    for name in &families {
        if let Some(fam) = get_table(&catalog, name) {
            let _ = conn.execute(
                "INSERT OR REPLACE INTO type_families (name, description, scripture, purpose, count) \
                 VALUES (?1, ?2, ?3, ?4, ?5)",
                rusqlite::params![
                    name,
                    get_str(fam, "description"),
                    get_str(fam, "scripture"),
                    get_str(fam, "purpose"),
                    get_i64(fam, "count"),
                ],
            );
        }
    }

    println!("  type_families: {} families", families.len());
}

fn seed_types(conn: &Connection, foundation_dir: &Path) {
    let settings_path = foundation_dir.join("types/primitives/primitives-settings.toml");
    let settings_content = match std::fs::read_to_string(&settings_path) {
        Ok(c) => c,
        Err(e) => {
            eprintln!("Warning: cannot read primitives-settings.toml: {e}");
            return;
        }
    };
    let settings_table: TomlMap = match toml::from_str::<toml::Table>(&settings_content) {
        Ok(t) => t,
        Err(e) => {
            eprintln!("Warning: parsing primitives-settings.toml: {e}");
            return;
        }
    };

    let cc = get_table(&settings_table, "cc").cloned().unwrap_or_default();
    let settings = get_table(&cc, "settings").cloned().unwrap_or_default();

    // Build composition map from catalog
    let cat_path = foundation_dir.join("types/primitives/primitives-catalog.toml");
    let mut comp_map: HashMap<String, String> = HashMap::new();
    if let Ok(cat_content) = std::fs::read_to_string(&cat_path) {
        if let Ok(cat) = toml::from_str::<toml::Table>(&cat_content) {
            let cat_cc = get_table(&cat, "cc").cloned().unwrap_or_default();
            let cat_ident = get_table(&cat_cc, "identity").cloned().unwrap_or_default();
            let arch = get_table(&cat_ident, "architecture").cloned().unwrap_or_default();
            let comp = get_table(&arch, "composition").cloned().unwrap_or_default();

            for (layer, v) in &comp {
                if let Some(arr) = v.as_array() {
                    for item in arr {
                        if let Some(s) = item.as_str() {
                            comp_map.insert(s.to_string(), layer.clone());
                        }
                    }
                }
            }
        }
    }

    let mut count = 0;

    // Helper closure to insert a type
    let mut insert_type = |name: &str, family: &str, trits: i64, bits: i64,
                           min_val: &str, max_val: &str, def_val: &str,
                           nullable: bool, signed: bool, states: &str, desc: &str| {
        let layer_role = comp_map.get(name).cloned().unwrap_or_default();
        let _ = conn.execute(
            "INSERT OR REPLACE INTO types \
             (name, family, size_trits, size_bits, min_value, max_value, default_val, \
              nullable, signed, states, description, layer_role) \
             VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12)",
            rusqlite::params![
                name, family, trits, bits, min_val, max_val, def_val,
                nullable as i64, signed as i64, states, desc, layer_role,
            ],
        );
        count += 1;
    };

    // Storage
    if let Some(storage) = get_table(&settings, "storage") {
        for name in &["uint8", "int8"] {
            if let Some(t) = get_table(storage, name) {
                let min_v = t.get("min_value").map(value_to_string).unwrap_or_default();
                let max_v = t.get("max_value").map(value_to_string).unwrap_or_default();
                let def_v = t.get("default").map(value_to_string).unwrap_or_default();
                insert_type(name, "storage", 0, get_i64(t, "size_bits"),
                    &min_v, &max_v, &def_v, false, get_bool(t, "signed"), "", "");
            }
        }
    }

    // Trit
    if let Some(trit_sec) = get_table(&settings, "trit") {
        for name in &["trit", "trit5"] {
            if let Some(t) = get_table(trit_sec, name) {
                let min_v = t.get("min_value").map(value_to_string).unwrap_or_default();
                let max_v = t.get("max_value").map(value_to_string).unwrap_or_default();
                let def_v = t.get("default").map(value_to_string).unwrap_or_default();
                let states = t.get("states").map(value_to_string).unwrap_or_default();
                insert_type(name, "trit", get_i64(t, "size_trits"), 0,
                    &min_v, &max_v, &def_v, false, false, &states, "");
            }
        }
    }

    // Integer
    if let Some(int_sec) = get_table(&settings, "integer") {
        for name in &["int9", "uint9", "int27", "uint27", "int81", "uint81", "int243", "uint243"] {
            if let Some(t) = get_table(int_sec, name) {
                let min_v = t.get("min_value").map(value_to_string).unwrap_or_default();
                let max_v = t.get("max_value").map(value_to_string).unwrap_or_default();
                let def_v = t.get("default").map(value_to_string).unwrap_or_default();
                let states = t.get("states").map(value_to_string).unwrap_or_default();
                insert_type(name, "integer", get_i64(t, "size_trits"), 0,
                    &min_v, &max_v, &def_v, false, get_bool(t, "signed"), &states, "");
            }
        }
    }

    // Float
    if let Some(float_sec) = get_table(&settings, "float") {
        if let Some(t) = get_table(float_sec, "float27") {
            let def_v = t.get("default").map(value_to_string).unwrap_or_default();
            insert_type("float27", "float", get_i64(t, "size_trits"), 0,
                "", "", &def_v, false, false, "continuous", "");
        }
    }

    // Text
    if let Some(text_sec) = get_table(&settings, "text") {
        for name in &["text", "char"] {
            if let Some(t) = get_table(text_sec, name) {
                let def_v = t.get("default").map(value_to_string).unwrap_or_default();
                insert_type(name, "text", 0, 0,
                    "", "", &def_v, get_bool(t, "nullable"), false, "", get_str(t, "encoding"));
            }
        }
    }

    // Boolean
    if let Some(bool_sec) = get_table(&settings, "boolean") {
        if let Some(_) = get_table(bool_sec, "bool3") {
            insert_type("bool3", "boolean", 1, 0, "-1", "1", "0", false, false, "3", "");
        }
    }

    // Collections
    if let Some(coll_sec) = get_table(&settings, "collections") {
        for name in &["list", "map", "set"] {
            if let Some(t) = get_table(coll_sec, name) {
                insert_type(name, "collections", 0, 0,
                    "", "", "", true, false, "", get_str(t, "description"));
            }
        }
    }

    // Special
    if let Some(spec_sec) = get_table(&settings, "special") {
        for name in &["void", "any"] {
            if let Some(t) = get_table(spec_sec, name) {
                insert_type(name, "special", 0, 0,
                    "", "", "", false, false, "", get_str(t, "description"));
            }
        }
    }

    // Decimal
    if let Some(dec_sec) = get_table(&settings, "decimal") {
        if let Some(t) = get_table(dec_sec, "decimal27") {
            let def_v = t.get("default").map(value_to_string).unwrap_or_default();
            insert_type("decimal27", "decimal", get_i64(t, "size_trits"), 0,
                "", "", &def_v, false, false, "", "");
        }
    }

    // Temporal
    if let Some(temp_sec) = get_table(&settings, "temporal") {
        for name in &["timestamp", "duration"] {
            if let Some(t) = get_table(temp_sec, name) {
                insert_type(name, "temporal", get_i64(t, "size_trits"), 0,
                    "", "", "", false, false, "", get_str(t, "description"));
            }
        }
    }

    println!("  types: {count} primitive types");
}

// ────────────────────────────────────────────────────────────────
// B.5 Operations
// ────────────────────────────────────────────────────────────────

fn seed_operations(conn: &Connection, foundation_dir: &Path) {
    let path = foundation_dir.join("math/ternary-operations.toml");
    let content = match std::fs::read_to_string(&path) {
        Ok(c) => c,
        Err(e) => {
            eprintln!("Warning: cannot read ternary-operations.toml: {e}");
            return;
        }
    };
    let table: TomlMap = match toml::from_str::<toml::Table>(&content) {
        Ok(t) => t,
        Err(_) => return,
    };

    let arith = get_table(&table, "arithmetic").cloned().unwrap_or_default();
    let mut count = 0;

    // Unary operations
    for name in &["negation", "absolute", "sign", "increment_sat", "decrement_sat"] {
        if let Some(v) = arith.get(*name) {
            let data = value_to_string(v);
            let _ = conn.execute(
                "INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data) \
                 VALUES (?1, 'unary', ?2, 1, ?3)",
                rusqlite::params![name, "Unary trit operation", data],
            );
            count += 1;
        }
    }

    // Binary operations
    for name in &["addition", "multiplication", "compare", "min", "max",
                   "logic_and", "logic_or", "logic_xor", "consensus"] {
        if let Some(sub) = get_table(&arith, name) {
            let desc = get_str(sub, "description").to_string();
            let data = sub.get("table").map(value_to_string).unwrap_or_default();
            let _ = conn.execute(
                "INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data) \
                 VALUES (?1, 'binary', ?2, 2, ?3)",
                rusqlite::params![name, desc, data],
            );
            count += 1;
        }
    }

    // Adders
    let ops = get_table(&table, "operations").cloned().unwrap_or_default();

    if let Some(ha) = get_table(&ops, "half_adder") {
        let desc = get_str(ha, "description").to_string();
        let sum_data = ha.get("sum").map(value_to_string).unwrap_or_default();
        let carry_data = ha.get("carry").map(value_to_string).unwrap_or_default();
        let _ = conn.execute(
            "INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data) \
             VALUES ('half_adder_sum', 'adder', ?1, 2, ?2)",
            rusqlite::params![desc, sum_data],
        );
        let _ = conn.execute(
            "INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data) \
             VALUES ('half_adder_carry', 'adder', ?1, 2, ?2)",
            rusqlite::params![desc, carry_data],
        );
        count += 2;
    }

    if let Some(fa) = get_table(&ops, "full_adder") {
        let desc = get_str(fa, "description").to_string();
        let sum_data = fa.get("sum").map(value_to_string).unwrap_or_default();
        let carry_data = fa.get("carry_out").map(value_to_string).unwrap_or_default();
        let _ = conn.execute(
            "INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data) \
             VALUES ('full_adder_sum', 'adder', ?1, 3, ?2)",
            rusqlite::params![desc, sum_data],
        );
        let _ = conn.execute(
            "INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data) \
             VALUES ('full_adder_carry', 'adder', ?1, 3, ?2)",
            rusqlite::params![desc, carry_data],
        );
        count += 2;
    }

    println!("  operations: {count} lookup tables");
}

// ────────────────────────────────────────────────────────────────
// B.6 Books, Scales, WebVariants
// ────────────────────────────────────────────────────────────────

fn seed_books(conn: &Connection, foundation_dir: &Path) {
    let path = foundation_dir.join("bible/addressing.toml");
    let content = match std::fs::read_to_string(&path) {
        Ok(c) => c,
        Err(e) => {
            eprintln!("Warning: cannot read addressing.toml: {e}");
            return;
        }
    };
    let table: TomlMap = match toml::from_str::<toml::Table>(&content) {
        Ok(t) => t,
        Err(_) => return,
    };

    let books = match get_table(&table, "books") {
        Some(b) => b.clone(),
        None => {
            eprintln!("Warning: no [books] section in addressing.toml");
            return;
        }
    };

    let mut count = 0;
    for (_key, v) in &books {
        if let Some(book) = v.as_table() {
            let name = get_str(book, "name");
            if name.is_empty() {
                continue;
            }

            let rng = get_i64_array(book, "range");
            let (range_start, range_end) = if rng.len() == 2 {
                (rng[0], rng[1])
            } else {
                (0, 0)
            };

            let _ = conn.execute(
                "INSERT OR REPLACE INTO books (id, name, abbreviation, chapters, verses, range_start, range_end) \
                 VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)",
                rusqlite::params![
                    get_i64(book, "ordinal"),
                    name,
                    get_str(book, "abbreviation"),
                    get_i64(book, "chapters"),
                    get_i64(book, "verses"),
                    range_start,
                    range_end,
                ],
            );
            count += 1;
        }
    }

    println!("  books: {count} Bible books");
}

fn seed_scales(conn: &Connection, foundation_dir: &Path) {
    let path = foundation_dir.join("math/ternary-definitions.toml");
    let content = match std::fs::read_to_string(&path) {
        Ok(c) => c,
        Err(e) => {
            eprintln!("Warning: cannot read ternary-definitions.toml: {e}");
            return;
        }
    };
    let table: TomlMap = match toml::from_str::<toml::Table>(&content) {
        Ok(t) => t,
        Err(_) => return,
    };

    let settings = get_table(&table, "settings").cloned().unwrap_or_default();
    let scales = match get_table(&settings, "scales") {
        Some(s) => s.clone(),
        None => {
            eprintln!("Warning: no [settings.scales] in ternary-definitions.toml");
            return;
        }
    };

    let mut count = 0;
    for (name, v) in &scales {
        if let Some(s) = v.as_table() {
            let states = s.get("states").map(value_to_string).unwrap_or_default();
            let _ = conn.execute(
                "INSERT OR REPLACE INTO ternary_scales (name, trit_count, states, bytes, layer) \
                 VALUES (?1, ?2, ?3, ?4, ?5)",
                rusqlite::params![
                    name,
                    get_i64(s, "count"),
                    states,
                    get_i64(s, "bytes"),
                    get_str(s, "layer"),
                ],
            );
            count += 1;
        }
    }

    println!("  ternary_scales: {count} scales");
}

fn seed_web_variants(conn: &Connection, foundation_dir: &Path) {
    let path = foundation_dir.join("bible/addressing.toml");
    let content = match std::fs::read_to_string(&path) {
        Ok(c) => c,
        Err(_) => return,
    };
    let table: TomlMap = match toml::from_str::<toml::Table>(&content) {
        Ok(t) => t,
        Err(_) => return,
    };

    let variants = match get_table(&table, "variants") {
        Some(v) => v.clone(),
        None => return,
    };

    let mut count = 0;
    for (_key, v) in &variants {
        if let Some(vv) = v.as_table() {
            let trite = get_i64(vv, "trite");
            if trite == 0 {
                continue;
            }
            let _ = conn.execute(
                "INSERT OR REPLACE INTO web_variants (trite, book, chapter, verse, summary) \
                 VALUES (?1, ?2, ?3, ?4, ?5)",
                rusqlite::params![
                    trite,
                    get_str(vv, "book"),
                    get_i64(vv, "chapter"),
                    get_i64(vv, "verse"),
                    get_str(vv, "summary"),
                ],
            );
            count += 1;
        }
    }

    println!("  web_variants: {count} variant verses");
}

// ────────────────────────────────────────────────────────────────
// B.7 Summary
// ────────────────────────────────────────────────────────────────

fn print_summary(conn: &Connection) {
    println!("\n--- Foundation Database Summary ---");
    let tables = [
        "specs", "type_families", "types", "operations",
        "books", "ternary_scales", "dependencies", "web_variants",
    ];
    for t in &tables {
        let count: i64 = conn
            .query_row(&format!("SELECT COUNT(*) FROM {t}"), [], |row| row.get(0))
            .unwrap_or(0);
        println!("  {t:<20} {count} rows");
    }
}

// ============================================================================
// CLOSING — Main entry point
// ============================================================================

fn main() {
    let root = find_root().expect("Cannot find b-word root. Run from within the b-word directory tree.");
    let db_path = root.join("data/foundation/database/foundation.db");
    println!("Seeding foundation database: {}", db_path.display());

    // Remove existing to rebuild fresh
    let _ = std::fs::remove_file(&db_path);

    let conn = Connection::open(&db_path).expect("open database");
    conn.execute_batch(SCHEMA).expect("create schema");

    let foundation_dir = root.join("L0-universal/ladder/foundation");

    let mut specs = 0;
    specs += seed_domain(&conn, &foundation_dir, "types");
    specs += seed_domain(&conn, &foundation_dir, "math");
    specs += seed_domain(&conn, &foundation_dir, "bible");

    // Index file at foundation root
    let index_path = foundation_dir.join("index.toml");
    if index_path.exists() {
        seed_spec(&conn, &index_path, "foundation");
        specs += 1;
    }

    seed_type_families(&conn, &foundation_dir);
    seed_types(&conn, &foundation_dir);
    seed_operations(&conn, &foundation_dir);
    seed_books(&conn, &foundation_dir);
    seed_scales(&conn, &foundation_dir);
    seed_web_variants(&conn, &foundation_dir);

    println!("Done. {specs} specs seeded.");
    print_summary(&conn);
}
