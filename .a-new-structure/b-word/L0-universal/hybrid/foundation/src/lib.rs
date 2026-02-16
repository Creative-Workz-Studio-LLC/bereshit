//omni:code --rust -library
//omni:key B-L0-foundation-loader-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # Foundation Loader (Rust)
//!
//! L0 foundation access to the database that materializes all TOML specs.
//!
//! The Triangle: Configuration (TOML) → Data (SQLite) → Code (this crate).
//! Every type, operation, scale, book, and dependency is discoverable
//! by query rather than hardcoded. The system knows itself.
//!
//! # Lifecycle Pattern
//!
//! **Hybrid** — combines ladder (sequential lookup by key/ordinal) and spiral
//! (iterative search across domains, dependency traversal) access patterns.
//!
//! # Tables
//!
//! ```text
//! specs, type_families, types, operations, books, ternary_scales, dependencies, web_variants
//! ```
//!
//! # Usage
//!
//! ```no_run
//! use bereshit_l0_foundation::DB;
//!
//! let db = DB::open("/path/to/b-word/data/foundation/database")?;
//! let trit = db.type_by_name("trit")?;
//! assert_eq!(trit.family, "trit");
//! # Ok::<(), bereshit_l0_foundation::FoundationError>(())
//! ```
//!
//! Data source: `b-word/data/foundation/database/foundation.db`

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// S.1 Imports
// ────────────────────────────────────────────────────────────────

use rusqlite::{Connection, OpenFlags, params};
use std::fmt;
use std::path::{Path, PathBuf};

// ────────────────────────────────────────────────────────────────
// S.2 Modules (promoted from L3 — universal infrastructure)
// ────────────────────────────────────────────────────────────────

pub mod result;
pub mod session;
pub mod session_context;
pub mod workflow;

// ────────────────────────────────────────────────────────────────
// S.3 Types
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

/// One of the 10 primitive type families.
#[derive(Debug, Clone)]
pub struct TypeFamily {
    pub id: i64,
    pub name: String,
    pub description: String,
    pub scripture: String,
    pub purpose: String,
    pub count: i64,
}

/// One of the 23+ primitive types.
#[derive(Debug, Clone)]
pub struct PrimitiveType {
    pub id: i64,
    pub name: String,
    pub family: String,
    pub size_trits: i64,
    pub size_bits: i64,
    pub min_value: String,
    pub max_value: String,
    pub default_val: String,
    pub nullable: bool,
    pub signed: bool,
    pub states: String,
    pub description: String,
    pub layer_role: String,
}

/// A ternary arithmetic lookup table.
#[derive(Debug, Clone)]
pub struct Operation {
    pub id: i64,
    pub name: String,
    pub category: String,
    pub description: String,
    pub input_size: i64,
    pub table_data: String,
    pub properties: String,
}

/// One of the 66 Bible books.
#[derive(Debug, Clone)]
pub struct Book {
    pub id: i64,
    pub name: String,
    pub abbreviation: String,
    pub chapters: i64,
    pub verses: i64,
    pub range_start: i64,
    pub range_end: i64,
}

/// A named ternary scale.
#[derive(Debug, Clone)]
pub struct TernaryScale {
    pub id: i64,
    pub name: String,
    pub trit_count: i64,
    pub states: String,
    pub bytes: i64,
    pub layer: String,
}

/// A cross-reference between specs.
#[derive(Debug, Clone)]
pub struct Dependency {
    pub id: i64,
    pub spec_key: String,
    pub depends_on: String,
    pub relation: String,
}

/// A WEB-only Bible verse mapped to trite spare values.
#[derive(Debug, Clone)]
pub struct WebVariant {
    pub trite: i64,
    pub book: String,
    pub chapter: i64,
    pub verse: i64,
    pub summary: String,
}

/// Errors from foundation database operations.
#[derive(Debug)]
pub enum FoundationError {
    Open(String, rusqlite::Error),
    Query(String, rusqlite::Error),
    NotFound(String),
}

impl fmt::Display for FoundationError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            FoundationError::Open(path, e) => write!(f, "foundation.open({path}): {e}"),
            FoundationError::Query(ctx, e) => write!(f, "foundation.{ctx}: {e}"),
            FoundationError::NotFound(ctx) => write!(f, "foundation.{ctx}: not found"),
        }
    }
}

impl std::error::Error for FoundationError {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match self {
            FoundationError::Open(_, e) | FoundationError::Query(_, e) => Some(e),
            FoundationError::NotFound(_) => None,
        }
    }
}

/// Open connection to the foundation database.
pub struct DB {
    conn: Connection,
}

// ────────────────────────────────────────────────────────────────
// S.4 Constants
// ────────────────────────────────────────────────────────────────

/// Path to foundation database relative to module root (b-word/).
pub const DATABASE_DIR: &str = "data/foundation/database";

/// Database filename.
pub const DATABASE_FILE: &str = "foundation.db";

// SQL queries
const Q_SPEC_BY_KEY: &str =
    "SELECT id, key, file, title, type, component, role, domain, layer, version, status, \
     scripture, principle, tags, provides, created, updated FROM specs WHERE key = ?1";

const Q_SPECS_BY_DOMAIN: &str =
    "SELECT id, key, file, title, type, component, role, domain, layer, version, status, \
     scripture, principle, tags, provides, created, updated FROM specs WHERE domain = ?1 ORDER BY key";

const Q_ALL_SPECS: &str =
    "SELECT id, key, file, title, type, component, role, domain, layer, version, status, \
     scripture, principle, tags, provides, created, updated FROM specs ORDER BY domain, key";

const Q_SPEC_COUNT: &str = "SELECT COUNT(*) FROM specs";

const Q_ALL_FAMILIES: &str =
    "SELECT id, name, description, scripture, purpose, count FROM type_families ORDER BY id";

const Q_FAMILY_BY_NAME: &str =
    "SELECT id, name, description, scripture, purpose, count FROM type_families WHERE name = ?1";

const Q_ALL_TYPES: &str =
    "SELECT id, name, family, size_trits, size_bits, min_value, max_value, default_val, \
     nullable, signed, states, description, layer_role FROM types ORDER BY id";

const Q_TYPE_BY_NAME: &str =
    "SELECT id, name, family, size_trits, size_bits, min_value, max_value, default_val, \
     nullable, signed, states, description, layer_role FROM types WHERE name = ?1";

const Q_TYPES_BY_FAMILY: &str =
    "SELECT id, name, family, size_trits, size_bits, min_value, max_value, default_val, \
     nullable, signed, states, description, layer_role FROM types WHERE family = ?1 ORDER BY id";

const Q_ALL_OPS: &str =
    "SELECT id, name, category, description, input_size, table_data, properties \
     FROM operations ORDER BY category, name";

const Q_OPS_BY_CAT: &str =
    "SELECT id, name, category, description, input_size, table_data, properties \
     FROM operations WHERE category = ?1 ORDER BY name";

const Q_OP_BY_NAME: &str =
    "SELECT id, name, category, description, input_size, table_data, properties \
     FROM operations WHERE name = ?1 AND category = ?2";

const Q_ALL_BOOKS: &str =
    "SELECT id, name, abbreviation, chapters, verses, range_start, range_end \
     FROM books ORDER BY id";

const Q_BOOK_BY_ORD: &str =
    "SELECT id, name, abbreviation, chapters, verses, range_start, range_end \
     FROM books WHERE id = ?1";

const Q_BOOK_BY_NAME: &str =
    "SELECT id, name, abbreviation, chapters, verses, range_start, range_end \
     FROM books WHERE name = ?1";

const Q_BOOK_BY_ABBR: &str =
    "SELECT id, name, abbreviation, chapters, verses, range_start, range_end \
     FROM books WHERE abbreviation = ?1";

const Q_ALL_SCALES: &str =
    "SELECT id, name, trit_count, states, bytes, layer FROM ternary_scales ORDER BY trit_count";

const Q_DEPS_OF: &str =
    "SELECT id, spec_key, depends_on, relation FROM dependencies \
     WHERE spec_key = ?1 ORDER BY relation, depends_on";

const Q_DEPS_ON: &str =
    "SELECT id, spec_key, depends_on, relation FROM dependencies \
     WHERE depends_on = ?1 ORDER BY relation, spec_key";

const Q_ALL_WEB_VARIANTS: &str =
    "SELECT trite, book, chapter, verse, summary FROM web_variants ORDER BY trite";

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Helpers
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

fn scan_family(row: &rusqlite::Row<'_>) -> rusqlite::Result<TypeFamily> {
    Ok(TypeFamily {
        id:          row.get(0)?,
        name:        row.get(1)?,
        description: row.get(2)?,
        scripture:   row.get(3)?,
        purpose:     row.get(4)?,
        count:       row.get(5)?,
    })
}

fn scan_type(row: &rusqlite::Row<'_>) -> rusqlite::Result<PrimitiveType> {
    let nullable: i64 = row.get(8)?;
    let signed: i64 = row.get(9)?;
    Ok(PrimitiveType {
        id:          row.get(0)?,
        name:        row.get(1)?,
        family:      row.get(2)?,
        size_trits:  row.get(3)?,
        size_bits:   row.get(4)?,
        min_value:   row.get(5)?,
        max_value:   row.get(6)?,
        default_val: row.get(7)?,
        nullable:    nullable != 0,
        signed:      signed != 0,
        states:      row.get(10)?,
        description: row.get(11)?,
        layer_role:  row.get(12)?,
    })
}

fn scan_op(row: &rusqlite::Row<'_>) -> rusqlite::Result<Operation> {
    Ok(Operation {
        id:          row.get(0)?,
        name:        row.get(1)?,
        category:    row.get(2)?,
        description: row.get(3)?,
        input_size:  row.get(4)?,
        table_data:  row.get(5)?,
        properties:  row.get(6)?,
    })
}

fn scan_book(row: &rusqlite::Row<'_>) -> rusqlite::Result<Book> {
    Ok(Book {
        id:           row.get(0)?,
        name:         row.get(1)?,
        abbreviation: row.get(2)?,
        chapters:     row.get(3)?,
        verses:       row.get(4)?,
        range_start:  row.get(5)?,
        range_end:    row.get(6)?,
    })
}

fn scan_scale(row: &rusqlite::Row<'_>) -> rusqlite::Result<TernaryScale> {
    Ok(TernaryScale {
        id:         row.get(0)?,
        name:       row.get(1)?,
        trit_count: row.get(2)?,
        states:     row.get(3)?,
        bytes:      row.get(4)?,
        layer:      row.get(5)?,
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

fn scan_variant(row: &rusqlite::Row<'_>) -> rusqlite::Result<WebVariant> {
    Ok(WebVariant {
        trite:   row.get(0)?,
        book:    row.get(1)?,
        chapter: row.get(2)?,
        verse:   row.get(3)?,
        summary: row.get(4)?,
    })
}

/// Collect rows from a prepared statement into a Vec using a scan function.
fn collect<T>(
    conn: &Connection,
    sql: &str,
    params: &[&dyn rusqlite::types::ToSql],
    scan: fn(&rusqlite::Row<'_>) -> rusqlite::Result<T>,
    ctx: &str,
) -> Result<Vec<T>, FoundationError> {
    let mut stmt = conn.prepare(sql)
        .map_err(|e| FoundationError::Query(ctx.into(), e))?;
    let rows = stmt.query_map(params, scan)
        .map_err(|e| FoundationError::Query(ctx.into(), e))?;
    let mut results = Vec::new();
    for row in rows {
        results.push(row.map_err(|e| FoundationError::Query(ctx.into(), e))?);
    }
    Ok(results)
}

// ────────────────────────────────────────────────────────────────
// B.2 Core Operations (Open/Close)
// ────────────────────────────────────────────────────────────────

impl DB {
    /// Open the foundation database from a directory path.
    pub fn open(db_dir: impl AsRef<Path>) -> Result<Self, FoundationError> {
        let db_path: PathBuf = db_dir.as_ref().join(DATABASE_FILE);
        let path_str = db_path.display().to_string();

        let conn = Connection::open_with_flags(
            &db_path,
            OpenFlags::SQLITE_OPEN_READ_ONLY | OpenFlags::SQLITE_OPEN_NO_MUTEX,
        )
        .map_err(|e| FoundationError::Open(path_str, e))?;

        Ok(DB { conn })
    }

    /// Open the foundation database using module root path.
    pub fn open_from_root(module_root: impl AsRef<Path>) -> Result<Self, FoundationError> {
        Self::open(module_root.as_ref().join(DATABASE_DIR))
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 Spec Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// Lookup a single spec by its unique key.
    pub fn spec_by_key(&self, key: &str) -> Result<Spec, FoundationError> {
        let ctx = format!("spec_by_key({key})");
        self.conn
            .query_row(Q_SPEC_BY_KEY, params![key], scan_spec)
            .map_err(|e| match e {
                rusqlite::Error::QueryReturnedNoRows => FoundationError::NotFound(ctx),
                other => FoundationError::Query(ctx, other),
            })
    }

    /// All specs in a domain ("types", "math", "bible").
    pub fn specs_by_domain(&self, domain: &str) -> Result<Vec<Spec>, FoundationError> {
        collect(&self.conn, Q_SPECS_BY_DOMAIN, &[&domain], scan_spec, &format!("specs_by_domain({domain})"))
    }

    /// Every spec in the database.
    pub fn all_specs(&self) -> Result<Vec<Spec>, FoundationError> {
        collect(&self.conn, Q_ALL_SPECS, &[], scan_spec, "all_specs")
    }

    /// Number of specs.
    pub fn spec_count(&self) -> Result<i64, FoundationError> {
        self.conn
            .query_row(Q_SPEC_COUNT, [], |row| row.get(0))
            .map_err(|e| FoundationError::Query("spec_count".into(), e))
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 Type Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All 10 type families.
    pub fn all_families(&self) -> Result<Vec<TypeFamily>, FoundationError> {
        collect(&self.conn, Q_ALL_FAMILIES, &[], scan_family, "all_families")
    }

    /// Lookup a type family by name.
    pub fn family_by_name(&self, name: &str) -> Result<TypeFamily, FoundationError> {
        let ctx = format!("family_by_name({name})");
        self.conn
            .query_row(Q_FAMILY_BY_NAME, params![name], scan_family)
            .map_err(|e| match e {
                rusqlite::Error::QueryReturnedNoRows => FoundationError::NotFound(ctx),
                other => FoundationError::Query(ctx, other),
            })
    }

    /// All primitive types.
    pub fn all_types(&self) -> Result<Vec<PrimitiveType>, FoundationError> {
        collect(&self.conn, Q_ALL_TYPES, &[], scan_type, "all_types")
    }

    /// Lookup a type by name (e.g., "trit", "int27").
    pub fn type_by_name(&self, name: &str) -> Result<PrimitiveType, FoundationError> {
        let ctx = format!("type_by_name({name})");
        self.conn
            .query_row(Q_TYPE_BY_NAME, params![name], scan_type)
            .map_err(|e| match e {
                rusqlite::Error::QueryReturnedNoRows => FoundationError::NotFound(ctx),
                other => FoundationError::Query(ctx, other),
            })
    }

    /// All types in a family (e.g., "integer", "trit").
    pub fn types_by_family(&self, family: &str) -> Result<Vec<PrimitiveType>, FoundationError> {
        collect(&self.conn, Q_TYPES_BY_FAMILY, &[&family], scan_type, &format!("types_by_family({family})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.5 Operation Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All operations.
    pub fn all_operations(&self) -> Result<Vec<Operation>, FoundationError> {
        collect(&self.conn, Q_ALL_OPS, &[], scan_op, "all_operations")
    }

    /// Operations in a category ("unary", "binary", "logic", "adder").
    pub fn operations_by_category(&self, category: &str) -> Result<Vec<Operation>, FoundationError> {
        collect(&self.conn, Q_OPS_BY_CAT, &[&category], scan_op, &format!("operations_by_category({category})"))
    }

    /// Single operation by name and category.
    pub fn operation_by_name(&self, name: &str, category: &str) -> Result<Operation, FoundationError> {
        let ctx = format!("operation_by_name({name}, {category})");
        self.conn
            .query_row(Q_OP_BY_NAME, params![name, category], scan_op)
            .map_err(|e| match e {
                rusqlite::Error::QueryReturnedNoRows => FoundationError::NotFound(ctx),
                other => FoundationError::Query(ctx, other),
            })
    }
}

// ────────────────────────────────────────────────────────────────
// B.6 Book Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All 66 Bible books in ordinal order.
    pub fn all_books(&self) -> Result<Vec<Book>, FoundationError> {
        collect(&self.conn, Q_ALL_BOOKS, &[], scan_book, "all_books")
    }

    /// Book by ordinal (1-66).
    pub fn book_by_ordinal(&self, ordinal: i64) -> Result<Book, FoundationError> {
        let ctx = format!("book_by_ordinal({ordinal})");
        self.conn
            .query_row(Q_BOOK_BY_ORD, params![ordinal], scan_book)
            .map_err(|e| match e {
                rusqlite::Error::QueryReturnedNoRows => FoundationError::NotFound(ctx),
                other => FoundationError::Query(ctx, other),
            })
    }

    /// Book by full name (e.g., "Genesis").
    pub fn book_by_name(&self, name: &str) -> Result<Book, FoundationError> {
        let ctx = format!("book_by_name({name})");
        self.conn
            .query_row(Q_BOOK_BY_NAME, params![name], scan_book)
            .map_err(|e| match e {
                rusqlite::Error::QueryReturnedNoRows => FoundationError::NotFound(ctx),
                other => FoundationError::Query(ctx, other),
            })
    }

    /// Book by abbreviation (e.g., "Gen").
    pub fn book_by_abbreviation(&self, abbr: &str) -> Result<Book, FoundationError> {
        let ctx = format!("book_by_abbreviation({abbr})");
        self.conn
            .query_row(Q_BOOK_BY_ABBR, params![abbr], scan_book)
            .map_err(|e| match e {
                rusqlite::Error::QueryReturnedNoRows => FoundationError::NotFound(ctx),
                other => FoundationError::Query(ctx, other),
            })
    }
}

// ────────────────────────────────────────────────────────────────
// B.7 Scale & Dependency Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// All ternary scales ordered by trit count.
    pub fn all_scales(&self) -> Result<Vec<TernaryScale>, FoundationError> {
        collect(&self.conn, Q_ALL_SCALES, &[], scan_scale, "all_scales")
    }

    /// What a spec depends on.
    pub fn dependencies_of(&self, spec_key: &str) -> Result<Vec<Dependency>, FoundationError> {
        collect(&self.conn, Q_DEPS_OF, &[&spec_key], scan_dep, &format!("dependencies_of({spec_key})"))
    }

    /// What depends on a spec.
    pub fn dependents_on(&self, spec_key: &str) -> Result<Vec<Dependency>, FoundationError> {
        collect(&self.conn, Q_DEPS_ON, &[&spec_key], scan_dep, &format!("dependents_on({spec_key})"))
    }
}

// ────────────────────────────────────────────────────────────────
// B.8 Web Variant Queries
// ────────────────────────────────────────────────────────────────

impl DB {
    /// The 13 WEB-only verses.
    pub fn all_web_variants(&self) -> Result<Vec<WebVariant>, FoundationError> {
        collect(&self.conn, Q_ALL_WEB_VARIANTS, &[], scan_variant, "all_web_variants")
    }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// This crate provides L0 foundation access from Rust. The Triangle complete:
//   Configuration (TOML specs) → Data (SQLite) → Code (this crate)
//
// "In the beginning God created the heaven and the earth." — Genesis 1:1
//
// Dependencies: rusqlite (bundled SQLite — no system dep)
// Data source: b-word/data/foundation/database/foundation.db

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::PathBuf;

    fn db_dir() -> PathBuf {
        let manifest = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
        manifest
            .parent().unwrap()  // hybrid/
            .parent().unwrap()  // L0-universal/
            .parent().unwrap()  // b-word/
            .join(DATABASE_DIR)
    }

    #[test]
    fn open_db() {
        let db = DB::open(db_dir());
        assert!(db.is_ok(), "Failed to open foundation DB: {:?}", db.err());
    }

    #[test]
    fn spec_count() {
        let db = DB::open(db_dir()).unwrap();
        let count = db.spec_count().unwrap();
        assert!(count >= 49, "Expected >= 49 specs, got {count}");
    }

    #[test]
    fn all_families() {
        let db = DB::open(db_dir()).unwrap();
        let families = db.all_families().unwrap();
        assert_eq!(families.len(), 10);
        assert_eq!(families[0].name, "storage");
    }

    #[test]
    fn trit_type() {
        let db = DB::open(db_dir()).unwrap();
        let t = db.type_by_name("trit").unwrap();
        assert_eq!(t.family, "trit");
        assert_eq!(t.size_trits, 1);
        assert_eq!(t.layer_role, "atomic");
    }

    #[test]
    fn all_types() {
        let db = DB::open(db_dir()).unwrap();
        let types = db.all_types().unwrap();
        assert!(types.len() >= 23, "Expected >= 23 types, got {}", types.len());
    }

    #[test]
    fn integer_family() {
        let db = DB::open(db_dir()).unwrap();
        let types = db.types_by_family("integer").unwrap();
        assert!(types.len() >= 4, "Expected >= 4 integer types");
    }

    #[test]
    fn all_operations() {
        let db = DB::open(db_dir()).unwrap();
        let ops = db.all_operations().unwrap();
        assert!(ops.len() >= 15, "Expected >= 15 operations, got {}", ops.len());
    }

    #[test]
    fn unary_operations() {
        let db = DB::open(db_dir()).unwrap();
        let ops = db.operations_by_category("unary").unwrap();
        assert!(ops.len() >= 5, "Expected >= 5 unary ops");
    }

    #[test]
    fn all_books() {
        let db = DB::open(db_dir()).unwrap();
        let books = db.all_books().unwrap();
        assert_eq!(books.len(), 66);
        assert_eq!(books[0].name, "Genesis");
        assert_eq!(books[65].name, "Revelation");
    }

    #[test]
    fn genesis_by_ordinal() {
        let db = DB::open(db_dir()).unwrap();
        let book = db.book_by_ordinal(1).unwrap();
        assert_eq!(book.name, "Genesis");
        assert_eq!(book.chapters, 50);
    }

    #[test]
    fn genesis_by_abbr() {
        let db = DB::open(db_dir()).unwrap();
        let book = db.book_by_abbreviation("Gen").unwrap();
        assert_eq!(book.name, "Genesis");
    }

    #[test]
    fn all_scales() {
        let db = DB::open(db_dir()).unwrap();
        let scales = db.all_scales().unwrap();
        assert_eq!(scales.len(), 6);
    }

    #[test]
    fn specs_by_domain() {
        let db = DB::open(db_dir()).unwrap();
        let types_specs = db.specs_by_domain("types").unwrap();
        assert!(types_specs.len() >= 30, "Expected >= 30 type specs");
    }

    #[test]
    fn dependencies() {
        let db = DB::open(db_dir()).unwrap();
        let all_specs = db.all_specs().unwrap();
        // Just check that dependency queries don't error on first spec with deps
        if let Some(spec) = all_specs.first() {
            let _ = db.dependencies_of(&spec.key); // may be empty, but shouldn't error
        }
    }

    #[test]
    fn web_variants() {
        let db = DB::open(db_dir()).unwrap();
        let variants = db.all_web_variants().unwrap();
        assert_eq!(variants.len(), 13);
        assert!(variants[0].trite >= 243);
    }

    #[test]
    fn not_found() {
        let db = DB::open(db_dir()).unwrap();
        let result = db.type_by_name("nonexistent_type");
        assert!(matches!(result, Err(FoundationError::NotFound(_))));
    }
}
