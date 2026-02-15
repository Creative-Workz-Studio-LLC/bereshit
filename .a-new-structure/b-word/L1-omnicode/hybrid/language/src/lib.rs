//omni:code --rust -library
//omni:key B-L1-language-loader-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # Language Loader (Rust)
//!
//! L1 language access to the database that materializes all TOML specs.
//!
//! The Triangle: Configuration (TOML) -> Data (SQLite) -> Code (this crate).
//! Every keyword, operator, syntax pattern, and extension is discoverable
//! by query rather than hardcoded. The system knows its own language.
//!
//! # Lifecycle Pattern
//!
//! **Hybrid** — combines ladder (sequential lookup by key/name) and spiral
//! (iterative search across categories, dependency traversal) access patterns.
//!
//! # Tables
//!
//! ```text
//! specs, dependencies, keywords, syntax_patterns, token_types,
//! precedence_groups, operators, keyword_groups, phrasal_operators,
//! disambiguation_rules, extensions
//! ```
//!
//! # Usage
//!
//! ```no_run
//! use bereshit_l1_language::DB;
//!
//! let db = DB::open("/path/to/b-word/data/language/database")?;
//! let kw = db.keyword_by_name("let")?;
//! assert_eq!(kw.category, "declaration");
//! # Ok::<(), bereshit_l1_language::LanguageError>(())
//! ```
//!
//! Data source: `b-word/data/language/database/language.db`

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// S.1 Imports
// ────────────────────────────────────────────────────────────────

use rusqlite::{Connection, OpenFlags};
use std::fmt;
use std::path::{Path, PathBuf};

// ────────────────────────────────────────────────────────────────
// S.2 Types
// ────────────────────────────────────────────────────────────────

/// A TOML specification file's metadata.
#[derive(Debug, Clone)]
pub struct Spec {
    pub id: i64,
    pub key: String,
    pub file: String,
    pub title: String,
    pub spec_type: String,
    pub component: String,
    pub role: String,
    pub domain: String,
    pub layer: String,
    pub version: String,
    pub status: String,
    pub scripture: String,
    pub principle: String,
    pub tags: String,
    pub provides: String,
    pub created: String,
    pub updated: String,
}

/// A cross-reference between specs.
#[derive(Debug, Clone)]
pub struct Dependency {
    pub id: i64,
    pub spec_key: String,
    pub depends_on: String,
    pub relation: String,
}

/// An OmniCode language keyword.
#[derive(Debug, Clone)]
pub struct Keyword {
    pub id: i64,
    pub keyword: String,
    pub category: String,
    pub role: String,
    pub scripture: String,
    pub scripture_text: String,
    pub example: String,
    pub reserved: bool,
    pub value: String,
}

/// A language syntax rule.
#[derive(Debug, Clone)]
pub struct SyntaxPattern {
    pub id: i64,
    pub name: String,
    pub category: String,
    pub pattern: String,
    pub maps_to: String,
    pub example: String,
    pub scripture: String,
    pub notes: String,
}

/// Classifies lexer output.
#[derive(Debug, Clone)]
pub struct TokenType {
    pub id: i64,
    pub name: String,
    pub token_id: String,
    pub meaning: String,
    pub members: String,
    pub emitted_by: String,
    pub subtypes: String,
}

/// Operator precedence level.
#[derive(Debug, Clone)]
pub struct PrecedenceGroup {
    pub id: i64,
    pub level: i64,
    pub name: String,
    pub operators: String,
    pub note: String,
}

/// A symbol operator.
#[derive(Debug, Clone)]
pub struct Operator {
    pub id: i64,
    pub name: String,
    pub category: String,
    pub symbol: String,
    pub token_type: String,
    pub fixity: String,
    pub meaning: String,
    pub ternary: String,
    pub discovered: String,
    pub examples: String,
}

/// A classified set of keywords.
#[derive(Debug, Clone)]
pub struct KeywordGroup {
    pub id: i64,
    pub group_path: String,
    pub token_type: String,
    pub words: String,
    pub meaning: String,
    pub ternary: String,
    pub syntax_ref: String,
    pub scripture: String,
    pub patterns: String,
}

/// A multi-word operator.
#[derive(Debug, Clone)]
pub struct PhrasalOperator {
    pub id: i64,
    pub name: String,
    pub words: String,
    pub meaning: String,
    pub maps_to: String,
    pub example: String,
    pub scripture: String,
    pub note: String,
}

/// Resolves keyword overloading.
#[derive(Debug, Clone)]
pub struct DisambiguationRule {
    pub id: i64,
    pub keyword: String,
    pub meanings: String,
    pub resolution: String,
}

/// A file extension definition.
#[derive(Debug, Clone)]
pub struct Extension {
    pub id: i64,
    pub ext: String,
    pub name: String,
    pub category: String,
    pub phase: String,
    pub biblical_root: String,
    pub operation: String,
    pub description: String,
    pub internal_format: String,
    pub block_pattern: String,
    pub default_type: String,
    pub mime_type: String,
    pub state_mapping: String,
    pub keyword_link: String,
    pub l0_parallel: String,
}

/// Errors from language database operations.
#[derive(Debug)]
pub enum LanguageError {
    Open(String, rusqlite::Error),
    Query(String, rusqlite::Error),
    NotFound(String),
}

impl fmt::Display for LanguageError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            LanguageError::Open(path, e) => write!(f, "language.open({path}): {e}"),
            LanguageError::Query(ctx, e) => write!(f, "language.{ctx}: {e}"),
            LanguageError::NotFound(ctx) => write!(f, "language.{ctx}: not found"),
        }
    }
}

impl std::error::Error for LanguageError {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match self {
            LanguageError::Open(_, e) | LanguageError::Query(_, e) => Some(e),
            LanguageError::NotFound(_) => None,
        }
    }
}

/// Open connection to the language database.
pub struct DB {
    conn: Connection,
}

// ────────────────────────────────────────────────────────────────
// S.3 Constants
// ────────────────────────────────────────────────────────────────

/// Path to language database relative to module root (b-word/).
pub const DATABASE_DIR: &str = "data/language/database";

/// Database filename.
pub const DATABASE_FILE: &str = "language.db";

// ────────────────────────────────────────────────────────────────
// S.4 SQL Queries
// ────────────────────────────────────────────────────────────────

// Specs
const Q_SPEC_BY_KEY: &str =
    "SELECT id, key, file, title, type, component, role, domain, layer, version, status, \
     scripture, principle, tags, provides, created, updated FROM specs WHERE key = ?1";
const Q_ALL_SPECS: &str =
    "SELECT id, key, file, title, type, component, role, domain, layer, version, status, \
     scripture, principle, tags, provides, created, updated FROM specs ORDER BY key";
const Q_SPEC_COUNT: &str = "SELECT COUNT(*) FROM specs";

// Dependencies
const Q_DEPS_OF: &str =
    "SELECT id, spec_key, depends_on, relation FROM dependencies \
     WHERE spec_key = ?1 ORDER BY relation, depends_on";
const Q_DEPS_ON: &str =
    "SELECT id, spec_key, depends_on, relation FROM dependencies \
     WHERE depends_on = ?1 ORDER BY relation, spec_key";

// Keywords
const Q_ALL_KEYWORDS: &str =
    "SELECT id, keyword, category, role, scripture, scripture_text, example, reserved, value \
     FROM keywords ORDER BY category, keyword";
const Q_KEYWORD_BY_NAME: &str =
    "SELECT id, keyword, category, role, scripture, scripture_text, example, reserved, value \
     FROM keywords WHERE keyword = ?1";
const Q_KEYWORDS_BY_CAT: &str =
    "SELECT id, keyword, category, role, scripture, scripture_text, example, reserved, value \
     FROM keywords WHERE category = ?1 ORDER BY keyword";

// Syntax patterns
const Q_ALL_SYNTAX: &str =
    "SELECT id, name, category, pattern, maps_to, example, scripture, notes \
     FROM syntax_patterns ORDER BY category, name";
const Q_SYNTAX_BY_CAT: &str =
    "SELECT id, name, category, pattern, maps_to, example, scripture, notes \
     FROM syntax_patterns WHERE category = ?1 ORDER BY name";
const Q_SYNTAX_BY_NAME: &str =
    "SELECT id, name, category, pattern, maps_to, example, scripture, notes \
     FROM syntax_patterns WHERE name = ?1 AND category = ?2";

// Token types
const Q_ALL_TOKEN_TYPES: &str =
    "SELECT id, name, token_id, meaning, members, emitted_by, subtypes \
     FROM token_types ORDER BY name";
const Q_TOKEN_TYPE_BY_NAME: &str =
    "SELECT id, name, token_id, meaning, members, emitted_by, subtypes \
     FROM token_types WHERE name = ?1";

// Precedence groups
const Q_ALL_PRECEDENCE: &str =
    "SELECT id, level, name, operators, note FROM precedence_groups ORDER BY level";
const Q_PRECEDENCE_BY_NAME: &str =
    "SELECT id, level, name, operators, note FROM precedence_groups WHERE name = ?1";

// Operators
const Q_ALL_OPERATORS: &str =
    "SELECT id, name, category, symbol, token_type, fixity, meaning, ternary, discovered, examples \
     FROM operators ORDER BY category, name";
const Q_OPERATORS_BY_CAT: &str =
    "SELECT id, name, category, symbol, token_type, fixity, meaning, ternary, discovered, examples \
     FROM operators WHERE category = ?1 ORDER BY name";
const Q_OPERATOR_BY_NAME: &str =
    "SELECT id, name, category, symbol, token_type, fixity, meaning, ternary, discovered, examples \
     FROM operators WHERE name = ?1 AND category = ?2";

// Keyword groups
const Q_ALL_KEYWORD_GROUPS: &str =
    "SELECT id, group_path, token_type, words, meaning, ternary, syntax_ref, scripture, patterns \
     FROM keyword_groups ORDER BY group_path";
const Q_KEYWORD_GROUP_BY_PATH: &str =
    "SELECT id, group_path, token_type, words, meaning, ternary, syntax_ref, scripture, patterns \
     FROM keyword_groups WHERE group_path = ?1";

// Phrasal operators
const Q_ALL_PHRASAL: &str =
    "SELECT id, name, words, meaning, maps_to, example, scripture, note \
     FROM phrasal_operators ORDER BY name";
const Q_PHRASAL_BY_NAME: &str =
    "SELECT id, name, words, meaning, maps_to, example, scripture, note \
     FROM phrasal_operators WHERE name = ?1";

// Disambiguation rules
const Q_ALL_DISAMBIG: &str =
    "SELECT id, keyword, meanings, resolution FROM disambiguation_rules ORDER BY keyword";
const Q_DISAMBIG_BY_KEYWORD: &str =
    "SELECT id, keyword, meanings, resolution FROM disambiguation_rules WHERE keyword = ?1";

// Extensions
const Q_ALL_EXTENSIONS: &str =
    "SELECT id, ext, name, category, phase, biblical_root, operation, description, \
     internal_format, block_pattern, default_type, mime_type, state_mapping, keyword_link, \
     l0_parallel FROM extensions ORDER BY category, ext";
const Q_EXTENSION_BY_EXT: &str =
    "SELECT id, ext, name, category, phase, biblical_root, operation, description, \
     internal_format, block_pattern, default_type, mime_type, state_mapping, keyword_link, \
     l0_parallel FROM extensions WHERE ext = ?1";
const Q_EXTENSIONS_BY_CAT: &str =
    "SELECT id, ext, name, category, phase, biblical_root, operation, description, \
     internal_format, block_pattern, default_type, mime_type, state_mapping, keyword_link, \
     l0_parallel FROM extensions WHERE category = ?1 ORDER BY ext";

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Helpers (scan functions)
// ────────────────────────────────────────────────────────────────

fn scan_spec(row: &rusqlite::Row<'_>) -> rusqlite::Result<Spec> {
    Ok(Spec {
        id:        row.get(0)?,
        key:       row.get(1)?,
        file:      row.get(2)?,
        title:     row.get(3)?,
        spec_type: row.get(4)?,
        component: row.get(5)?,
        role:      row.get(6)?,
        domain:    row.get(7)?,
        layer:     row.get(8)?,
        version:   row.get(9)?,
        status:    row.get(10)?,
        scripture: row.get(11)?,
        principle: row.get(12)?,
        tags:      row.get(13)?,
        provides:  row.get(14)?,
        created:   row.get(15)?,
        updated:   row.get(16)?,
    })
}

fn scan_dep(row: &rusqlite::Row<'_>) -> rusqlite::Result<Dependency> {
    Ok(Dependency {
        id:         row.get(0)?,
        spec_key:   row.get(1)?,
        depends_on: row.get(2)?,
        relation:   row.get(3)?,
    })
}

fn scan_keyword(row: &rusqlite::Row<'_>) -> rusqlite::Result<Keyword> {
    let reserved: i64 = row.get(7)?;
    Ok(Keyword {
        id:             row.get(0)?,
        keyword:        row.get(1)?,
        category:       row.get(2)?,
        role:           row.get(3)?,
        scripture:      row.get(4)?,
        scripture_text: row.get(5)?,
        example:        row.get(6)?,
        reserved:       reserved != 0,
        value:          row.get(8)?,
    })
}

fn scan_syntax(row: &rusqlite::Row<'_>) -> rusqlite::Result<SyntaxPattern> {
    Ok(SyntaxPattern {
        id:        row.get(0)?,
        name:      row.get(1)?,
        category:  row.get(2)?,
        pattern:   row.get(3)?,
        maps_to:   row.get(4)?,
        example:   row.get(5)?,
        scripture: row.get(6)?,
        notes:     row.get(7)?,
    })
}

fn scan_token_type(row: &rusqlite::Row<'_>) -> rusqlite::Result<TokenType> {
    Ok(TokenType {
        id:         row.get(0)?,
        name:       row.get(1)?,
        token_id:   row.get(2)?,
        meaning:    row.get(3)?,
        members:    row.get(4)?,
        emitted_by: row.get(5)?,
        subtypes:   row.get(6)?,
    })
}

fn scan_precedence(row: &rusqlite::Row<'_>) -> rusqlite::Result<PrecedenceGroup> {
    Ok(PrecedenceGroup {
        id:        row.get(0)?,
        level:     row.get(1)?,
        name:      row.get(2)?,
        operators: row.get(3)?,
        note:      row.get(4)?,
    })
}

fn scan_operator(row: &rusqlite::Row<'_>) -> rusqlite::Result<Operator> {
    Ok(Operator {
        id:         row.get(0)?,
        name:       row.get(1)?,
        category:   row.get(2)?,
        symbol:     row.get(3)?,
        token_type: row.get(4)?,
        fixity:     row.get(5)?,
        meaning:    row.get(6)?,
        ternary:    row.get(7)?,
        discovered: row.get(8)?,
        examples:   row.get(9)?,
    })
}

fn scan_keyword_group(row: &rusqlite::Row<'_>) -> rusqlite::Result<KeywordGroup> {
    Ok(KeywordGroup {
        id:         row.get(0)?,
        group_path: row.get(1)?,
        token_type: row.get(2)?,
        words:      row.get(3)?,
        meaning:    row.get(4)?,
        ternary:    row.get(5)?,
        syntax_ref: row.get(6)?,
        scripture:  row.get(7)?,
        patterns:   row.get(8)?,
    })
}

fn scan_phrasal(row: &rusqlite::Row<'_>) -> rusqlite::Result<PhrasalOperator> {
    Ok(PhrasalOperator {
        id:        row.get(0)?,
        name:      row.get(1)?,
        words:     row.get(2)?,
        meaning:   row.get(3)?,
        maps_to:   row.get(4)?,
        example:   row.get(5)?,
        scripture: row.get(6)?,
        note:      row.get(7)?,
    })
}

fn scan_disambig(row: &rusqlite::Row<'_>) -> rusqlite::Result<DisambiguationRule> {
    Ok(DisambiguationRule {
        id:         row.get(0)?,
        keyword:    row.get(1)?,
        meanings:   row.get(2)?,
        resolution: row.get(3)?,
    })
}

fn scan_extension(row: &rusqlite::Row<'_>) -> rusqlite::Result<Extension> {
    Ok(Extension {
        id:              row.get(0)?,
        ext:             row.get(1)?,
        name:            row.get(2)?,
        category:        row.get(3)?,
        phase:           row.get(4)?,
        biblical_root:   row.get(5)?,
        operation:       row.get(6)?,
        description:     row.get(7)?,
        internal_format: row.get(8)?,
        block_pattern:   row.get(9)?,
        default_type:    row.get(10)?,
        mime_type:       row.get(11)?,
        state_mapping:   row.get(12)?,
        keyword_link:    row.get(13)?,
        l0_parallel:     row.get(14)?,
    })
}

/// Collect rows from a prepared statement into a Vec using a scan function.
fn collect<T>(
    conn: &Connection,
    sql: &str,
    params: &[&dyn rusqlite::types::ToSql],
    scan: fn(&rusqlite::Row<'_>) -> rusqlite::Result<T>,
    ctx: &str,
) -> Result<Vec<T>, LanguageError> {
    let mut stmt = conn.prepare(sql)
        .map_err(|e| LanguageError::Query(ctx.into(), e))?;
    let rows = stmt.query_map(params, scan)
        .map_err(|e| LanguageError::Query(ctx.into(), e))?;
    let mut results = Vec::new();
    for row in rows {
        results.push(row.map_err(|e| LanguageError::Query(ctx.into(), e))?);
    }
    Ok(results)
}

/// Single-row query returning NotFound if missing.
fn query_one<T>(
    conn: &Connection,
    sql: &str,
    params: &[&dyn rusqlite::types::ToSql],
    scan: fn(&rusqlite::Row<'_>) -> rusqlite::Result<T>,
    ctx: String,
) -> Result<T, LanguageError> {
    conn.query_row(sql, params, scan)
        .map_err(|e| match e {
            rusqlite::Error::QueryReturnedNoRows => LanguageError::NotFound(ctx),
            other => LanguageError::Query(ctx, other),
        })
}

// ────────────────────────────────────────────────────────────────
// B.2 Core Operations (Open/Close)
// ────────────────────────────────────────────────────────────────

impl DB {
    /// Open the language database from a directory path.
    pub fn open(db_dir: impl AsRef<Path>) -> Result<Self, LanguageError> {
        let db_path: PathBuf = db_dir.as_ref().join(DATABASE_FILE);
        let path_str = db_path.display().to_string();

        let conn = Connection::open_with_flags(
            &db_path,
            OpenFlags::SQLITE_OPEN_READ_ONLY | OpenFlags::SQLITE_OPEN_NO_MUTEX,
        )
        .map_err(|e| LanguageError::Open(path_str, e))?;

        Ok(DB { conn })
    }

    /// Open the language database using module root path.
    pub fn open_from_root(module_root: impl AsRef<Path>) -> Result<Self, LanguageError> {
        Self::open(module_root.as_ref().join(DATABASE_DIR))
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 Spec Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// Lookup a single spec by its unique key.
    pub fn spec_by_key(&self, key: &str) -> Result<Spec, LanguageError> {
        query_one(&self.conn, Q_SPEC_BY_KEY, &[&key], scan_spec, format!("spec_by_key({key})"))
    }

    /// Every spec in the database.
    pub fn all_specs(&self) -> Result<Vec<Spec>, LanguageError> {
        collect(&self.conn, Q_ALL_SPECS, &[], scan_spec, "all_specs")
    }

    /// Number of specs.
    pub fn spec_count(&self) -> Result<i64, LanguageError> {
        self.conn
            .query_row(Q_SPEC_COUNT, [], |row| row.get(0))
            .map_err(|e| LanguageError::Query("spec_count".into(), e))
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 Dependency Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// What a spec depends on.
    pub fn dependencies_of(&self, spec_key: &str) -> Result<Vec<Dependency>, LanguageError> {
        collect(&self.conn, Q_DEPS_OF, &[&spec_key], scan_dep, &format!("dependencies_of({spec_key})"))
    }

    /// What depends on a spec.
    pub fn dependents_on(&self, spec_key: &str) -> Result<Vec<Dependency>, LanguageError> {
        collect(&self.conn, Q_DEPS_ON, &[&spec_key], scan_dep, &format!("dependents_on({spec_key})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.5 Keyword Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All keywords.
    pub fn all_keywords(&self) -> Result<Vec<Keyword>, LanguageError> {
        collect(&self.conn, Q_ALL_KEYWORDS, &[], scan_keyword, "all_keywords")
    }

    /// Lookup a keyword by name.
    pub fn keyword_by_name(&self, keyword: &str) -> Result<Keyword, LanguageError> {
        query_one(&self.conn, Q_KEYWORD_BY_NAME, &[&keyword], scan_keyword, format!("keyword_by_name({keyword})"))
    }

    /// Keywords in a category.
    pub fn keywords_by_category(&self, category: &str) -> Result<Vec<Keyword>, LanguageError> {
        collect(&self.conn, Q_KEYWORDS_BY_CAT, &[&category], scan_keyword, &format!("keywords_by_category({category})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.6 Syntax Pattern Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All syntax patterns.
    pub fn all_syntax_patterns(&self) -> Result<Vec<SyntaxPattern>, LanguageError> {
        collect(&self.conn, Q_ALL_SYNTAX, &[], scan_syntax, "all_syntax_patterns")
    }

    /// Patterns in a category.
    pub fn syntax_patterns_by_category(&self, category: &str) -> Result<Vec<SyntaxPattern>, LanguageError> {
        collect(&self.conn, Q_SYNTAX_BY_CAT, &[&category], scan_syntax, &format!("syntax_patterns_by_category({category})"))
    }

    /// Single pattern by name and category.
    pub fn syntax_pattern_by_name(&self, name: &str, category: &str) -> Result<SyntaxPattern, LanguageError> {
        query_one(&self.conn, Q_SYNTAX_BY_NAME, &[&name, &category], scan_syntax, format!("syntax_pattern_by_name({name}, {category})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.7 Token Type & Precedence Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All token types.
    pub fn all_token_types(&self) -> Result<Vec<TokenType>, LanguageError> {
        collect(&self.conn, Q_ALL_TOKEN_TYPES, &[], scan_token_type, "all_token_types")
    }

    /// Single token type by name.
    pub fn token_type_by_name(&self, name: &str) -> Result<TokenType, LanguageError> {
        query_one(&self.conn, Q_TOKEN_TYPE_BY_NAME, &[&name], scan_token_type, format!("token_type_by_name({name})"))
    }

    /// All precedence groups ordered by level.
    pub fn all_precedence_groups(&self) -> Result<Vec<PrecedenceGroup>, LanguageError> {
        collect(&self.conn, Q_ALL_PRECEDENCE, &[], scan_precedence, "all_precedence_groups")
    }

    /// Single precedence group by name.
    pub fn precedence_group_by_name(&self, name: &str) -> Result<PrecedenceGroup, LanguageError> {
        query_one(&self.conn, Q_PRECEDENCE_BY_NAME, &[&name], scan_precedence, format!("precedence_group_by_name({name})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.8 Operator Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All symbol operators.
    pub fn all_operators(&self) -> Result<Vec<Operator>, LanguageError> {
        collect(&self.conn, Q_ALL_OPERATORS, &[], scan_operator, "all_operators")
    }

    /// Operators in a category.
    pub fn operators_by_category(&self, category: &str) -> Result<Vec<Operator>, LanguageError> {
        collect(&self.conn, Q_OPERATORS_BY_CAT, &[&category], scan_operator, &format!("operators_by_category({category})"))
    }

    /// Single operator by name and category.
    pub fn operator_by_name(&self, name: &str, category: &str) -> Result<Operator, LanguageError> {
        query_one(&self.conn, Q_OPERATOR_BY_NAME, &[&name, &category], scan_operator, format!("operator_by_name({name}, {category})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.9 Keyword Group Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All keyword groups.
    pub fn all_keyword_groups(&self) -> Result<Vec<KeywordGroup>, LanguageError> {
        collect(&self.conn, Q_ALL_KEYWORD_GROUPS, &[], scan_keyword_group, "all_keyword_groups")
    }

    /// Keyword group by path (e.g., "keywords.control.proceed").
    pub fn keyword_group_by_path(&self, path: &str) -> Result<KeywordGroup, LanguageError> {
        query_one(&self.conn, Q_KEYWORD_GROUP_BY_PATH, &[&path], scan_keyword_group, format!("keyword_group_by_path({path})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.10 Phrasal Operator & Disambiguation Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All phrasal (multi-word) operators.
    pub fn all_phrasal_operators(&self) -> Result<Vec<PhrasalOperator>, LanguageError> {
        collect(&self.conn, Q_ALL_PHRASAL, &[], scan_phrasal, "all_phrasal_operators")
    }

    /// Single phrasal operator by name.
    pub fn phrasal_operator_by_name(&self, name: &str) -> Result<PhrasalOperator, LanguageError> {
        query_one(&self.conn, Q_PHRASAL_BY_NAME, &[&name], scan_phrasal, format!("phrasal_operator_by_name({name})"))
    }

    /// All disambiguation rules.
    pub fn all_disambiguation_rules(&self) -> Result<Vec<DisambiguationRule>, LanguageError> {
        collect(&self.conn, Q_ALL_DISAMBIG, &[], scan_disambig, "all_disambiguation_rules")
    }

    /// Disambiguation rule by keyword.
    pub fn disambiguation_rule_by_keyword(&self, keyword: &str) -> Result<DisambiguationRule, LanguageError> {
        query_one(&self.conn, Q_DISAMBIG_BY_KEYWORD, &[&keyword], scan_disambig, format!("disambiguation_rule_by_keyword({keyword})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.11 Extension Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All file extension definitions.
    pub fn all_extensions(&self) -> Result<Vec<Extension>, LanguageError> {
        collect(&self.conn, Q_ALL_EXTENSIONS, &[], scan_extension, "all_extensions")
    }

    /// Single extension by its extension string (e.g., ".omni").
    pub fn extension_by_ext(&self, ext: &str) -> Result<Extension, LanguageError> {
        query_one(&self.conn, Q_EXTENSION_BY_EXT, &[&ext], scan_extension, format!("extension_by_ext({ext})"))
    }

    /// Extensions in a category ("omni", "biblical", "cognitive").
    pub fn extensions_by_category(&self, category: &str) -> Result<Vec<Extension>, LanguageError> {
        collect(&self.conn, Q_EXTENSIONS_BY_CAT, &[&category], scan_extension, &format!("extensions_by_category({category})"))
    }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// This crate provides L1 language access from Rust. The Triangle complete:
//   Configuration (TOML specs) -> Data (SQLite) -> Code (this crate)
//
// "In the beginning was the Word, and the Word was with God,
//  and the Word was God." — John 1:1
//
// Dependencies: rusqlite (bundled SQLite — no system dep)
// Data source: b-word/data/language/database/language.db

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;

    fn db_dir() -> PathBuf {
        let manifest = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
        manifest
            .parent().unwrap()  // hybrid/
            .parent().unwrap()  // L1-omnicode/
            .parent().unwrap()  // b-word/
            .join(DATABASE_DIR)
    }

    #[test]
    fn open_db() {
        let db = DB::open(db_dir());
        assert!(db.is_ok(), "Failed to open language DB: {:?}", db.err());
    }

    #[test]
    fn spec_count() {
        let db = DB::open(db_dir()).unwrap();
        let count = db.spec_count().unwrap();
        assert_eq!(count, 4, "Expected 4 specs, got {count}");
    }

    #[test]
    fn all_keywords() {
        let db = DB::open(db_dir()).unwrap();
        let keywords = db.all_keywords().unwrap();
        assert_eq!(keywords.len(), 73, "Expected 73 keywords, got {}", keywords.len());
    }

    #[test]
    fn keyword_let() {
        let db = DB::open(db_dir()).unwrap();
        let kw = db.keyword_by_name("let").unwrap();
        assert_eq!(kw.category, "declaration");
        assert!(kw.reserved);
    }

    #[test]
    fn keywords_by_category() {
        let db = DB::open(db_dir()).unwrap();
        let decl = db.keywords_by_category("declaration").unwrap();
        assert!(!decl.is_empty(), "Expected declaration keywords");
    }

    #[test]
    fn all_syntax_patterns() {
        let db = DB::open(db_dir()).unwrap();
        let patterns = db.all_syntax_patterns().unwrap();
        assert_eq!(patterns.len(), 100, "Expected 100 syntax patterns, got {}", patterns.len());
    }

    #[test]
    fn all_token_types() {
        let db = DB::open(db_dir()).unwrap();
        let types = db.all_token_types().unwrap();
        assert_eq!(types.len(), 18, "Expected 18 token types, got {}", types.len());
    }

    #[test]
    fn all_precedence_groups() {
        let db = DB::open(db_dir()).unwrap();
        let groups = db.all_precedence_groups().unwrap();
        assert_eq!(groups.len(), 19, "Expected 19 precedence groups, got {}", groups.len());
    }

    #[test]
    fn all_operators() {
        let db = DB::open(db_dir()).unwrap();
        let ops = db.all_operators().unwrap();
        assert_eq!(ops.len(), 80, "Expected 80 operators, got {}", ops.len());
    }

    #[test]
    fn operators_by_category() {
        let db = DB::open(db_dir()).unwrap();
        let arith = db.operators_by_category("arithmetic").unwrap();
        assert!(!arith.is_empty(), "Expected arithmetic operators");
    }

    #[test]
    fn all_keyword_groups() {
        let db = DB::open(db_dir()).unwrap();
        let groups = db.all_keyword_groups().unwrap();
        assert_eq!(groups.len(), 32, "Expected 32 keyword groups, got {}", groups.len());
    }

    #[test]
    fn all_phrasal_operators() {
        let db = DB::open(db_dir()).unwrap();
        let phrasal = db.all_phrasal_operators().unwrap();
        assert_eq!(phrasal.len(), 34, "Expected 34 phrasal operators, got {}", phrasal.len());
    }

    #[test]
    fn all_disambiguation_rules() {
        let db = DB::open(db_dir()).unwrap();
        let rules = db.all_disambiguation_rules().unwrap();
        assert_eq!(rules.len(), 7, "Expected 7 disambiguation rules, got {}", rules.len());
    }

    #[test]
    fn all_extensions() {
        let db = DB::open(db_dir()).unwrap();
        let exts = db.all_extensions().unwrap();
        assert_eq!(exts.len(), 15, "Expected 15 extensions, got {}", exts.len());
    }

    #[test]
    fn extension_omni() {
        let db = DB::open(db_dir()).unwrap();
        let ext = db.extension_by_ext(".omni").unwrap();
        assert_eq!(ext.category, "omni-suite");
    }

    #[test]
    fn extensions_by_category() {
        let db = DB::open(db_dir()).unwrap();
        let biblical = db.extensions_by_category("biblical").unwrap();
        assert!(!biblical.is_empty(), "Expected biblical extensions");
    }

    #[test]
    fn not_found() {
        let db = DB::open(db_dir()).unwrap();
        let result = db.keyword_by_name("nonexistent_keyword");
        assert!(matches!(result, Err(LanguageError::NotFound(_))));
    }

    #[test]
    fn dependencies() {
        let db = DB::open(db_dir()).unwrap();
        let specs = db.all_specs().unwrap();
        if let Some(spec) = specs.first() {
            let _ = db.dependencies_of(&spec.key); // shouldn't error
        }
    }
}
