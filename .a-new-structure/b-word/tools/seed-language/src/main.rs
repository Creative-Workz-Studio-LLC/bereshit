// #!omni code --rust
//
// seed-language — Populate L1 language database from TOML specs (Rust version).
//
// Reads TOML specifications under L1-omnicode/ladder/language/
// and seeds a SQLite database at data/language/database/language.db.
//
// The triangle: TOML (Configuration) → SQLite (Data) → Go/Rust (Code)
//
// "In the beginning was the Word, and the Word was with God,
//  and the Word was God." — John 1:1

use rusqlite::Connection;
use std::fs;
use std::path::{Path, PathBuf};
use toml::Table as TomlMap;

// ═══════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════

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
    domain      TEXT    NOT NULL DEFAULT 'language',
    layer       TEXT    NOT NULL DEFAULT 'L1',
    version     TEXT    NOT NULL DEFAULT '',
    status      TEXT    NOT NULL DEFAULT 'Active',
    scripture   TEXT    NOT NULL DEFAULT '',
    principle   TEXT    NOT NULL DEFAULT '',
    tags        TEXT    NOT NULL DEFAULT '',
    provides    TEXT    NOT NULL DEFAULT '',
    created     TEXT    NOT NULL DEFAULT '',
    updated     TEXT    NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS dependencies (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_key    TEXT    NOT NULL,
    depends_on  TEXT    NOT NULL,
    relation    TEXT    NOT NULL DEFAULT 'requires',
    UNIQUE(spec_key, depends_on, relation)
);

CREATE TABLE IF NOT EXISTS keywords (
    id             INTEGER PRIMARY KEY AUTOINCREMENT,
    keyword        TEXT    NOT NULL,
    category       TEXT    NOT NULL,
    role           TEXT    NOT NULL DEFAULT '',
    scripture      TEXT    NOT NULL DEFAULT '',
    scripture_text TEXT    NOT NULL DEFAULT '',
    example        TEXT    NOT NULL DEFAULT '',
    reserved       INTEGER NOT NULL DEFAULT 1,
    value          TEXT    NOT NULL DEFAULT '',
    UNIQUE(keyword, category)
);

CREATE TABLE IF NOT EXISTS syntax_patterns (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL,
    category    TEXT    NOT NULL,
    pattern     TEXT    NOT NULL DEFAULT '',
    maps_to     TEXT    NOT NULL DEFAULT '',
    example     TEXT    NOT NULL DEFAULT '',
    scripture   TEXT    NOT NULL DEFAULT '',
    notes       TEXT    NOT NULL DEFAULT '',
    UNIQUE(name, category)
);

CREATE TABLE IF NOT EXISTS token_types (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL UNIQUE,
    token_id    TEXT    NOT NULL,
    meaning     TEXT    NOT NULL DEFAULT '',
    members     TEXT    NOT NULL DEFAULT '',
    emitted_by  TEXT    NOT NULL DEFAULT '',
    subtypes    TEXT    NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS precedence_groups (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    level       INTEGER NOT NULL,
    name        TEXT    NOT NULL UNIQUE,
    operators   TEXT    NOT NULL DEFAULT '',
    note        TEXT    NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS operators (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL,
    category    TEXT    NOT NULL,
    symbol      TEXT    NOT NULL DEFAULT '',
    token_type  TEXT    NOT NULL DEFAULT '',
    fixity      TEXT    NOT NULL DEFAULT '',
    meaning     TEXT    NOT NULL DEFAULT '',
    ternary     TEXT    NOT NULL DEFAULT '',
    discovered  TEXT    NOT NULL DEFAULT '',
    examples    TEXT    NOT NULL DEFAULT '',
    UNIQUE(name, category)
);

CREATE TABLE IF NOT EXISTS keyword_groups (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    group_path  TEXT    NOT NULL UNIQUE,
    token_type  TEXT    NOT NULL DEFAULT '',
    words       TEXT    NOT NULL DEFAULT '',
    meaning     TEXT    NOT NULL DEFAULT '',
    ternary     TEXT    NOT NULL DEFAULT '',
    syntax_ref  TEXT    NOT NULL DEFAULT '',
    scripture   TEXT    NOT NULL DEFAULT '',
    patterns    TEXT    NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS phrasal_operators (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL UNIQUE,
    words       TEXT    NOT NULL DEFAULT '',
    meaning     TEXT    NOT NULL DEFAULT '',
    maps_to     TEXT    NOT NULL DEFAULT '',
    example     TEXT    NOT NULL DEFAULT '',
    scripture   TEXT    NOT NULL DEFAULT '',
    note        TEXT    NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS disambiguation_rules (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    keyword     TEXT    NOT NULL UNIQUE,
    meanings    TEXT    NOT NULL DEFAULT '',
    resolution  TEXT    NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS extensions (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    ext             TEXT    NOT NULL UNIQUE,
    name            TEXT    NOT NULL,
    category        TEXT    NOT NULL,
    phase           TEXT    NOT NULL DEFAULT 'a',
    biblical_root   TEXT    NOT NULL DEFAULT '',
    operation       TEXT    NOT NULL DEFAULT '',
    description     TEXT    NOT NULL DEFAULT '',
    internal_format TEXT    NOT NULL DEFAULT '',
    block_pattern   TEXT    NOT NULL DEFAULT '',
    default_type    TEXT    NOT NULL DEFAULT '',
    mime_type       TEXT    NOT NULL DEFAULT '',
    state_mapping   TEXT    NOT NULL DEFAULT '',
    keyword_link    TEXT    NOT NULL DEFAULT '',
    l0_parallel     TEXT    NOT NULL DEFAULT ''
);

CREATE INDEX IF NOT EXISTS idx_specs_domain ON specs(domain);
CREATE INDEX IF NOT EXISTS idx_keywords_category ON keywords(category);
CREATE INDEX IF NOT EXISTS idx_syntax_category ON syntax_patterns(category);
CREATE INDEX IF NOT EXISTS idx_operators_category ON operators(category);
CREATE INDEX IF NOT EXISTS idx_operators_token_type ON operators(token_type);
CREATE INDEX IF NOT EXISTS idx_extensions_category ON extensions(category);
CREATE INDEX IF NOT EXISTS idx_dependencies_spec ON dependencies(spec_key);
"#;

// ═══════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════

fn main() {
    let root = find_root().expect("Cannot find b-word root. Run from within the b-word directory tree.");
    let db_path = root.join("data/language/database/language.db");

    println!("Seeding language database: {}", db_path.display());

    // Ensure directory exists
    fs::create_dir_all(db_path.parent().unwrap()).ok();

    // Remove existing to rebuild fresh
    fs::remove_file(&db_path).ok();

    let conn = Connection::open(&db_path).expect("open database");
    conn.execute_batch(SCHEMA).expect("create schema");

    let lang_dir = root.join("L1-omnicode/ladder/language");

    // Seed specs from all 4 TOML files
    for f in &["keywords.toml", "syntax.toml", "omnicode-operators.toml", "extensions.toml"] {
        let path = lang_dir.join(f);
        if path.exists() {
            seed_spec(&conn, &path);
        }
    }

    // Load operators TOML once for reuse
    let ops_path = lang_dir.join("omnicode-operators.toml");
    let ops_raw = load_toml(&ops_path);

    // Seed domain-specific tables
    seed_keywords(&conn, &lang_dir);
    seed_syntax_patterns(&conn, &lang_dir);
    seed_token_types(&conn, &ops_raw);
    seed_precedence_groups(&conn, &ops_raw);
    seed_operators(&conn, &ops_raw);
    seed_keyword_groups(&conn, &ops_raw);
    seed_phrasal_operators(&conn, &ops_raw);
    seed_disambiguation_rules(&conn, &ops_raw);
    seed_extensions(&conn, &lang_dir);

    println!("\nDone.");
    print_summary(&conn);
}

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

fn load_toml(path: &Path) -> Option<TomlMap> {
    let content = fs::read_to_string(path).ok()?;
    match toml::from_str::<TomlMap>(&content) {
        Ok(t) => Some(t),
        Err(e) => {
            eprintln!("Warning: parsing {}: {e}", path.display());
            None
        }
    }
}

// ─────────────────────────────────────────────────────────────
// Seed specs + dependencies
// ─────────────────────────────────────────────────────────────

fn seed_spec(conn: &Connection, path: &Path) {
    let Some(raw) = load_toml(path) else { return };

    let pragma = get_table(&raw, "_pragma").unwrap_or_default();
    let meta = get_table(&raw, "_metadata").unwrap_or_default();

    let p1 = get_table(&pragma, "P1_core").unwrap_or_default();
    let key = get_str(&p1, "key")
        .unwrap_or_else(|| path.file_name().unwrap().to_string_lossy().to_string());

    let m1 = get_table(&meta, "M1_identity").unwrap_or_default();
    let m2 = get_table(&meta, "M2_state").unwrap_or_default();
    let m4 = get_table(&meta, "M4_grounding").unwrap_or_default();
    let m5 = get_table(&meta, "M5_dependencies").unwrap_or_default();
    let m7 = get_table(&meta, "M7_classification").unwrap_or_default();
    let p3 = get_table(&pragma, "P3_instance").unwrap_or_default();
    let p5 = get_table(&pragma, "P5_summary").unwrap_or_default();

    let title = get_str(&m1, "title").or_else(|| get_str(&p5, "title")).unwrap_or_default();
    let file = get_str(&m1, "file")
        .unwrap_or_else(|| path.file_name().unwrap().to_string_lossy().to_string());
    let provides = get_str_array(&p3, "provides").join(", ");

    conn.execute(
        "INSERT OR REPLACE INTO specs
         (key, file, title, type, component, role, domain, layer, version, status,
          scripture, principle, tags, provides, created, updated)
         VALUES (?1,?2,?3,?4,?5,?6,'language','L1',?7,?8,?9,?10,?11,?12,?13,?14)",
        rusqlite::params![
            key, file, title,
            s(&m1, "type"), s(&m1, "component"), s(&m1, "role"),
            s(&m2, "version"), s(&m2, "status"),
            s(&m4, "scripture"), s(&m4, "principle"),
            s(&m7, "tags"), provides,
            s(&m2, "created"), s(&m2, "updated"),
        ],
    ).ok();

    // Seed dependencies
    for dep in get_str_array(&m5, "requires") {
        conn.execute(
            "INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?1,?2,'requires')",
            rusqlite::params![key, dep],
        ).ok();
    }
    for dep in get_str_array(&m5, "consumers") {
        conn.execute(
            "INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?1,?2,'consumed_by')",
            rusqlite::params![dep, key],
        ).ok();
    }
    if let Some(d) = get_str(&m5, "derives_from") {
        conn.execute(
            "INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?1,?2,'derives_from')",
            rusqlite::params![key, d],
        ).ok();
    }

    println!("  spec: {key}");
}

// ─────────────────────────────────────────────────────────────
// Seed keywords from keywords.toml
// ─────────────────────────────────────────────────────────────

fn seed_keywords(conn: &Connection, lang_dir: &Path) {
    let Some(raw) = load_toml(&lang_dir.join("keywords.toml")) else { return };

    let skip = ["_pragma", "_metadata", "_content", "_validation", "_closing", "extensions"];
    let mut count = 0u32;

    for (category, val) in &raw {
        if skip.contains(&category.as_str()) {
            continue;
        }
        let Some(cat_table) = val.as_table() else { continue };

        for (_name, kv) in cat_table {
            let Some(entry) = kv.as_table() else { continue };
            let Some(kw) = get_str(entry, "keyword") else { continue };

            let reserved: i32 = entry.get("reserved")
                .and_then(|v| v.as_bool())
                .map(|b| if b { 1 } else { 0 })
                .unwrap_or(1);

            conn.execute(
                "INSERT OR IGNORE INTO keywords
                 (keyword, category, role, scripture, scripture_text, example, reserved, value)
                 VALUES (?1,?2,?3,?4,?5,?6,?7,?8)",
                rusqlite::params![
                    kw,
                    s(entry, "category"), s(entry, "role"),
                    s(entry, "scripture"), s(entry, "scripture_text"),
                    s(entry, "example"), reserved, s(entry, "value"),
                ],
            ).ok();
            count += 1;
        }
    }

    println!("  keywords: {count} entries");
}

// ─────────────────────────────────────────────────────────────
// Seed syntax patterns from syntax.toml
// ─────────────────────────────────────────────────────────────

fn seed_syntax_patterns(conn: &Connection, lang_dir: &Path) {
    let Some(raw) = load_toml(&lang_dir.join("syntax.toml")) else { return };

    let skip = ["_pragma", "_metadata", "_content", "_validation", "_closing", "extensions"];
    let mut count = 0u32;

    for (category, val) in &raw {
        if skip.contains(&category.as_str()) {
            continue;
        }
        let Some(cat_table) = val.as_table() else { continue };

        for (name, pv) in cat_table {
            let Some(entry) = pv.as_table() else { continue };
            if get_str(entry, "pattern").is_none() {
                continue;
            }

            conn.execute(
                "INSERT OR IGNORE INTO syntax_patterns
                 (name, category, pattern, maps_to, example, scripture, notes)
                 VALUES (?1,?2,?3,?4,?5,?6,?7)",
                rusqlite::params![
                    name, category,
                    s(entry, "pattern"), s(entry, "maps_to"),
                    s(entry, "example"), s(entry, "scripture"),
                    s(entry, "notes"),
                ],
            ).ok();
            count += 1;
        }
    }

    println!("  syntax_patterns: {count} entries");
}

// ─────────────────────────────────────────────────────────────
// Seed token types from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

fn seed_token_types(conn: &Connection, raw: &Option<TomlMap>) {
    let Some(raw) = raw else { return };
    let Some(tt) = raw.get("token_types").and_then(|v| v.as_table()) else { return };

    let mut count = 0u32;
    for (name, val) in tt {
        let Some(entry) = val.as_table() else { continue };

        let members = to_json(&get_str_array(entry, "members"));
        let subtypes = to_json(&get_str_array(entry, "subtypes"));

        conn.execute(
            "INSERT OR IGNORE INTO token_types
             (name, token_id, meaning, members, emitted_by, subtypes)
             VALUES (?1,?2,?3,?4,?5,?6)",
            rusqlite::params![
                name, s(entry, "id"), s(entry, "meaning"),
                members, s(entry, "emitted_by"), subtypes,
            ],
        ).ok();
        count += 1;
    }

    println!("  token_types: {count} entries");
}

// ─────────────────────────────────────────────────────────────
// Seed precedence groups from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

fn seed_precedence_groups(conn: &Connection, raw: &Option<TomlMap>) {
    let Some(raw) = raw else { return };
    let Some(prec) = raw.get("precedence").and_then(|v| v.as_table()) else { return };
    let Some(groups) = prec.get("groups").and_then(|v| v.as_array()) else { return };

    let mut count = 0u32;
    for g in groups {
        let Some(entry) = g.as_table() else { continue };

        let operators = to_json(&get_str_array(entry, "operators"));

        conn.execute(
            "INSERT OR IGNORE INTO precedence_groups
             (level, name, operators, note)
             VALUES (?1,?2,?3,?4)",
            rusqlite::params![
                entry.get("level").and_then(|v| v.as_integer()).unwrap_or(0),
                s(entry, "name"), operators, s(entry, "note"),
            ],
        ).ok();
        count += 1;
    }

    println!("  precedence_groups: {count} entries");
}

// ─────────────────────────────────────────────────────────────
// Seed symbol operators from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

fn seed_operators(conn: &Connection, raw: &Option<TomlMap>) {
    let Some(raw) = raw else { return };

    let categories = [
        "pragma", "control", "direction", "binding", "structure", "scope",
        "nesting", "containment", "reference", "punctuation", "emphasis",
        "access", "comparison", "arithmetic", "type_op", "composition",
        "ternary", "comment",
    ];

    let mut count = 0u32;
    for cat in &categories {
        let Some(cat_table) = raw.get(*cat).and_then(|v| v.as_table()) else { continue };

        for (name, val) in cat_table {
            let Some(entry) = val.as_table() else { continue };

            let mut examples = to_json(&get_str_array(entry, "examples"));
            if examples == "[]" {
                if let Some(ex) = get_str(entry, "example") {
                    examples = to_json(&[ex]);
                }
            }

            conn.execute(
                "INSERT OR IGNORE INTO operators
                 (name, category, symbol, token_type, fixity, meaning, ternary, discovered, examples)
                 VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9)",
                rusqlite::params![
                    name, cat,
                    s(entry, "symbol"), s(entry, "token_type"),
                    s(entry, "fixity"), s(entry, "meaning"),
                    s(entry, "ternary"), s(entry, "discovered"),
                    examples,
                ],
            ).ok();
            count += 1;
        }
    }

    println!("  operators: {count} entries");
}

// ─────────────────────────────────────────────────────────────
// Seed keyword groups from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

fn seed_keyword_groups(conn: &Connection, raw: &Option<TomlMap>) {
    let Some(raw) = raw else { return };
    let Some(keywords) = raw.get("keywords").and_then(|v| v.as_table()) else { return };

    let mut count = 0u32;
    for (group_name, val) in keywords {
        let Some(group_table) = val.as_table() else { continue };

        // Check if leaf (has "token_type") or nested
        if group_table.contains_key("token_type") {
            let path = format!("keywords.{group_name}");
            insert_keyword_group(conn, &path, group_table);
            count += 1;
        } else {
            for (sub_name, sv) in group_table {
                let Some(sub_table) = sv.as_table() else { continue };
                let path = format!("keywords.{group_name}.{sub_name}");
                insert_keyword_group(conn, &path, sub_table);
                count += 1;
            }
        }
    }

    println!("  keyword_groups: {count} entries");
}

fn insert_keyword_group(conn: &Connection, path: &str, entry: &TomlMap) {
    let words = to_json(&get_str_array(entry, "words"));
    let patterns = to_json(&get_str_array(entry, "patterns"));

    conn.execute(
        "INSERT OR IGNORE INTO keyword_groups
         (group_path, token_type, words, meaning, ternary, syntax_ref, scripture, patterns)
         VALUES (?1,?2,?3,?4,?5,?6,?7,?8)",
        rusqlite::params![
            path, s(entry, "token_type"), words,
            s(entry, "meaning"), s(entry, "ternary"),
            s(entry, "syntax_ref"), s(entry, "scripture"),
            patterns,
        ],
    ).ok();
}

// ─────────────────────────────────────────────────────────────
// Seed phrasal operators from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

fn seed_phrasal_operators(conn: &Connection, raw: &Option<TomlMap>) {
    let Some(raw) = raw else { return };
    let Some(phrasal) = raw.get("phrasal").and_then(|v| v.as_table()) else { return };

    let mut count = 0u32;
    for (name, val) in phrasal {
        let Some(entry) = val.as_table() else { continue };

        let words = to_json(&get_str_array(entry, "words"));

        conn.execute(
            "INSERT OR IGNORE INTO phrasal_operators
             (name, words, meaning, maps_to, example, scripture, note)
             VALUES (?1,?2,?3,?4,?5,?6,?7)",
            rusqlite::params![
                name, words, s(entry, "meaning"),
                s(entry, "maps_to"), s(entry, "example"),
                s(entry, "scripture"), s(entry, "note"),
            ],
        ).ok();
        count += 1;
    }

    println!("  phrasal_operators: {count} entries");
}

// ─────────────────────────────────────────────────────────────
// Seed disambiguation rules from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

fn seed_disambiguation_rules(conn: &Connection, raw: &Option<TomlMap>) {
    let Some(raw) = raw else { return };
    let Some(dis) = raw.get("disambiguation").and_then(|v| v.as_table()) else { return };

    let mut count = 0u32;
    for (_name, val) in dis {
        let Some(entry) = val.as_table() else { continue };
        let Some(kw) = get_str(entry, "keyword") else { continue };

        let meanings = to_json(&get_str_array(entry, "meanings"));

        conn.execute(
            "INSERT OR IGNORE INTO disambiguation_rules
             (keyword, meanings, resolution)
             VALUES (?1,?2,?3)",
            rusqlite::params![kw, meanings, s(entry, "resolution")],
        ).ok();
        count += 1;
    }

    println!("  disambiguation_rules: {count} entries");
}

// ─────────────────────────────────────────────────────────────
// Seed extensions from extensions.toml
// ─────────────────────────────────────────────────────────────

fn seed_extensions(conn: &Connection, lang_dir: &Path) {
    let Some(raw) = load_toml(&lang_dir.join("extensions.toml")) else { return };

    let categories = ["omni", "biblical", "cognitive"];
    let mut count = 0u32;

    for cat in &categories {
        let Some(cat_table) = raw.get(*cat).and_then(|v| v.as_table()) else { continue };

        for (_name, val) in cat_table {
            let Some(entry) = val.as_table() else { continue };
            let Some(ext) = get_str(entry, "ext") else { continue };

            conn.execute(
                "INSERT OR IGNORE INTO extensions
                 (ext, name, category, phase, biblical_root, operation, description,
                  internal_format, block_pattern, default_type, mime_type,
                  state_mapping, keyword_link, l0_parallel)
                 VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13,?14)",
                rusqlite::params![
                    ext, s(entry, "name"), s(entry, "category"),
                    s(entry, "phase"), s(entry, "biblical_root"),
                    s(entry, "operation"), s(entry, "description"),
                    s(entry, "internal_format"), s(entry, "block_pattern"),
                    s(entry, "default_type"), s(entry, "mime_type"),
                    s(entry, "state_mapping"), s(entry, "keyword_link"),
                    s(entry, "l0_parallel"),
                ],
            ).ok();
            count += 1;
        }
    }

    println!("  extensions: {count} entries");
}

// ═══════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════

fn print_summary(conn: &Connection) {
    println!("\n--- Language Database Summary ---");
    let tables = [
        "specs", "dependencies", "keywords", "syntax_patterns",
        "token_types", "precedence_groups", "operators", "keyword_groups",
        "phrasal_operators", "disambiguation_rules", "extensions",
    ];
    let mut total = 0u32;
    for t in &tables {
        let count: u32 = conn
            .query_row(&format!("SELECT COUNT(*) FROM {t}"), [], |r| r.get(0))
            .unwrap_or(0);
        println!("  {t:<25} {count} rows");
        total += count;
    }
    println!("  {:<25} {total} rows", "TOTAL");
}

fn get_table(table: &TomlMap, key: &str) -> Option<TomlMap> {
    table.get(key)?.as_table().cloned()
}

fn get_str(table: &TomlMap, key: &str) -> Option<String> {
    table.get(key)?.as_str().map(String::from)
}

/// Shorthand: get string or empty
fn s(table: &TomlMap, key: &str) -> String {
    get_str(table, key).unwrap_or_default()
}

fn get_str_array(table: &TomlMap, key: &str) -> Vec<String> {
    table.get(key)
        .and_then(|v| v.as_array())
        .map(|arr| {
            arr.iter()
                .filter_map(|v| v.as_str().map(String::from))
                .collect()
        })
        .unwrap_or_default()
}

fn to_json<S: AsRef<str>>(arr: &[S]) -> String {
    let v: Vec<&str> = arr.iter().map(|s| s.as_ref()).collect();
    serde_json::to_string(&v).unwrap_or_else(|_| "[]".to_string())
}
