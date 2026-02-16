//omni:code --rust -library
//omni:key B-L2-database-lib-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # Database Infrastructure (Rust)
//!
//! Platform-level SQLite database management — connection configuration,
//! migration runner, MultiDB coordinator.
//!
//! Domain-specific repository traits and implementations belong in their
//! respective layers (L3 for CPI-SI, etc.).
//!
//! # Types
//!
//! - `Config` — Database connection settings (pool size, WAL, foreign keys)
//! - `MultiDB` — Manages multiple named databases
//! - `Repository` — Minimal trait for any database backend
//!
//! # Usage
//!
//! ```no_run
//! use bereshit_l2_database::{Config, open_sqlite};
//!
//! let cfg = Config::default_for("/path/to/db.sqlite");
//! let conn = open_sqlite(&cfg)?;
//! # Ok::<(), bereshit_l2_database::DatabaseError>(())
//! ```

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// S.1 Imports
// ────────────────────────────────────────────────────────────────

use rusqlite::{Connection, OpenFlags};
use std::collections::HashMap;
use std::fmt;
use std::path::Path;
use std::sync::{Mutex, RwLock};

// ────────────────────────────────────────────────────────────────
// S.2 Types
// ────────────────────────────────────────────────────────────────

/// Database connection configuration.
#[derive(Debug, Clone)]
pub struct Config {
    pub path: String,
    pub wal_mode: bool,
    pub foreign_keys: bool,
}

/// Errors from database operations.
#[derive(Debug)]
pub enum DatabaseError {
    Open(String, rusqlite::Error),
    Migration(String, rusqlite::Error),
    Query(String, rusqlite::Error),
    NotRegistered(String),
    Lock(String),
}

/// Minimal repository trait for any database backend.
/// Domain-specific interfaces extend this in their own layers.
pub trait Repository: Send + Sync {
    /// Apply schema migrations.
    fn migrate(&self) -> Result<(), DatabaseError>;
    /// Close the repository (release resources).
    fn close(&mut self);
}

/// Manages multiple named database connections.
pub struct MultiDB {
    dbs: RwLock<HashMap<String, Mutex<Connection>>>,
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Config
// ────────────────────────────────────────────────────────────────

impl Config {
    /// Default configuration for a SQLite database.
    pub fn default_for(path: impl Into<String>) -> Self {
        Self {
            path: path.into(),
            wal_mode: true,
            foreign_keys: true,
        }
    }

    /// Configuration without WAL mode (for read-only or temp databases).
    pub fn readonly(path: impl Into<String>) -> Self {
        Self {
            path: path.into(),
            wal_mode: false,
            foreign_keys: true,
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Open & Migrate
// ────────────────────────────────────────────────────────────────

/// Open a SQLite database with the given configuration.
pub fn open_sqlite(cfg: &Config) -> Result<Connection, DatabaseError> {
    let conn = Connection::open_with_flags(
        &cfg.path,
        OpenFlags::SQLITE_OPEN_READ_WRITE | OpenFlags::SQLITE_OPEN_CREATE,
    )
    .map_err(|e| DatabaseError::Open(cfg.path.clone(), e))?;

    if cfg.foreign_keys {
        conn.execute_batch("PRAGMA foreign_keys = ON;")
            .map_err(|e| DatabaseError::Open(cfg.path.clone(), e))?;
    }

    if cfg.wal_mode {
        conn.execute_batch("PRAGMA journal_mode = WAL;")
            .map_err(|e| DatabaseError::Open(cfg.path.clone(), e))?;
    }

    Ok(conn)
}

/// Open a SQLite database with default settings.
pub fn open_default(path: impl AsRef<Path>) -> Result<Connection, DatabaseError> {
    let cfg = Config::default_for(path.as_ref().to_string_lossy().to_string());
    open_sqlite(&cfg)
}

/// Run a SQL migration against a connection.
/// Uses CREATE TABLE IF NOT EXISTS pattern — safe to run repeatedly.
pub fn run_migration(conn: &Connection, schema: &str) -> Result<(), DatabaseError> {
    conn.execute_batch(schema)
        .map_err(|e| DatabaseError::Migration("execute schema".into(), e))
}

/// Check if a column exists in a table.
pub fn column_exists(conn: &Connection, table: &str, column: &str) -> Result<bool, DatabaseError> {
    let query = format!("PRAGMA table_info({table})");
    let mut stmt = conn
        .prepare(&query)
        .map_err(|e| DatabaseError::Query(format!("table_info({table})"), e))?;

    let rows = stmt
        .query_map([], |row| row.get::<_, String>(1))
        .map_err(|e| DatabaseError::Query(format!("table_info({table})"), e))?;

    for name in rows.flatten() {
        if name == column {
            return Ok(true);
        }
    }
    Ok(false)
}

/// Add a column if it doesn't exist (SQLite lacks ADD COLUMN IF NOT EXISTS).
pub fn ensure_column(
    conn: &Connection,
    table: &str,
    column: &str,
    col_type: &str,
) -> Result<(), DatabaseError> {
    if column_exists(conn, table, column)? {
        return Ok(());
    }

    let alter = format!("ALTER TABLE {table} ADD COLUMN {column} {col_type}");
    conn.execute_batch(&alter)
        .map_err(|e| DatabaseError::Migration(format!("add {table}.{column}"), e))
}

// ────────────────────────────────────────────────────────────────
// B.3 Query Helpers
// ────────────────────────────────────────────────────────────────

/// Execute a SQL statement and return rows affected.
pub fn exec_statement(
    conn: &Connection,
    query: &str,
    params: &[&dyn rusqlite::types::ToSql],
) -> Result<usize, DatabaseError> {
    conn.execute(query, params)
        .map_err(|e| DatabaseError::Query("exec".into(), e))
}

// ────────────────────────────────────────────────────────────────
// B.4 MultiDB
// ────────────────────────────────────────────────────────────────

impl MultiDB {
    /// Create an empty MultiDB coordinator.
    pub fn new() -> Self {
        Self {
            dbs: RwLock::new(HashMap::new()),
        }
    }

    /// Register a named database connection.
    pub fn register(&self, name: impl Into<String>, conn: Connection) {
        let mut dbs = self.dbs.write().unwrap();
        dbs.insert(name.into(), Mutex::new(conn));
    }

    /// Open and register a database with default config.
    pub fn open_and_register(
        &self,
        name: impl Into<String>,
        path: impl AsRef<Path>,
    ) -> Result<(), DatabaseError> {
        let conn = open_default(path)?;
        self.register(name, conn);
        Ok(())
    }

    /// Execute a function against a named database.
    pub fn with_db<F, T>(&self, name: &str, f: F) -> Result<T, DatabaseError>
    where
        F: FnOnce(&Connection) -> Result<T, DatabaseError>,
    {
        let dbs = self.dbs.read().map_err(|_| {
            DatabaseError::Lock(format!("read lock for {name}"))
        })?;

        let mutex = dbs
            .get(name)
            .ok_or_else(|| DatabaseError::NotRegistered(name.into()))?;

        let conn = mutex.lock().map_err(|_| {
            DatabaseError::Lock(format!("db lock for {name}"))
        })?;

        f(&conn)
    }

    /// Run a migration function against all registered databases.
    pub fn migrate_all<F>(&self, migrator: F) -> Result<(), DatabaseError>
    where
        F: Fn(&str, &Connection) -> Result<(), DatabaseError>,
    {
        let dbs = self.dbs.read().map_err(|_| {
            DatabaseError::Lock("read lock for migrate_all".into())
        })?;

        for (name, mutex) in dbs.iter() {
            let conn = mutex.lock().map_err(|_| {
                DatabaseError::Lock(format!("db lock for {name}"))
            })?;
            migrator(name, &conn)?;
        }
        Ok(())
    }

    /// Return all registered database names.
    pub fn names(&self) -> Vec<String> {
        let dbs = self.dbs.read().unwrap();
        dbs.keys().cloned().collect()
    }

    /// Close all databases (drops connections).
    pub fn close_all(&self) {
        let mut dbs = self.dbs.write().unwrap();
        dbs.clear();
    }
}

impl Default for MultiDB {
    fn default() -> Self {
        Self::new()
    }
}

// ────────────────────────────────────────────────────────────────
// B.5 Error Display
// ────────────────────────────────────────────────────────────────

impl fmt::Display for DatabaseError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Open(path, e) => write!(f, "database.open({path}): {e}"),
            Self::Migration(ctx, e) => write!(f, "database.migrate({ctx}): {e}"),
            Self::Query(ctx, e) => write!(f, "database.query({ctx}): {e}"),
            Self::NotRegistered(name) => write!(f, "database: {name} not registered"),
            Self::Lock(ctx) => write!(f, "database.lock({ctx}): poisoned"),
        }
    }
}

impl std::error::Error for DatabaseError {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match self {
            Self::Open(_, e) | Self::Migration(_, e) | Self::Query(_, e) => Some(e),
            Self::NotRegistered(_) | Self::Lock(_) => None,
        }
    }
}

// ============================================================================
// CLOSING
// ============================================================================
// Platform database infrastructure — generic SQLite management.
// "Except the LORD build the house, they labour in vain" — Psalm 127:1

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn default_config() {
        let cfg = Config::default_for("/tmp/test.db");
        assert!(cfg.wal_mode);
        assert!(cfg.foreign_keys);
    }

    #[test]
    fn readonly_config() {
        let cfg = Config::readonly("/tmp/test.db");
        assert!(!cfg.wal_mode);
        assert!(cfg.foreign_keys);
    }

    #[test]
    fn multidb_empty() {
        let mdb = MultiDB::new();
        assert!(mdb.names().is_empty());
    }

    #[test]
    fn open_in_memory() {
        let cfg = Config {
            path: ":memory:".into(),
            wal_mode: false,
            foreign_keys: true,
        };
        let conn = open_sqlite(&cfg).expect("open in-memory");
        run_migration(
            &conn,
            "CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name TEXT);",
        )
        .expect("migrate");

        conn.execute("INSERT INTO test (name) VALUES (?1)", &["hello"])
            .expect("insert");

        let count: i64 = conn
            .query_row("SELECT COUNT(*) FROM test", [], |r| r.get(0))
            .expect("count");
        assert_eq!(count, 1);
    }

    #[test]
    fn multidb_register_and_access() {
        let mdb = MultiDB::new();
        let conn = Connection::open_in_memory().expect("open");
        mdb.register("test", conn);

        assert_eq!(mdb.names().len(), 1);

        let result = mdb.with_db("test", |c| {
            c.execute_batch("CREATE TABLE t (id INTEGER);")
                .map_err(|e| DatabaseError::Query("create".into(), e))
        });
        assert!(result.is_ok());
    }

    #[test]
    fn multidb_not_registered() {
        let mdb = MultiDB::new();
        let result = mdb.with_db("missing", |_| Ok(()));
        assert!(result.is_err());
    }

    #[test]
    fn ensure_column_test() {
        let conn = Connection::open_in_memory().expect("open");
        conn.execute_batch("CREATE TABLE t (id INTEGER PRIMARY KEY);")
            .expect("create");

        // Column doesn't exist yet
        assert!(!column_exists(&conn, "t", "name").expect("check"));

        // Add it
        ensure_column(&conn, "t", "name", "TEXT DEFAULT ''").expect("ensure");

        // Now it exists
        assert!(column_exists(&conn, "t", "name").expect("check2"));

        // Idempotent
        ensure_column(&conn, "t", "name", "TEXT DEFAULT ''").expect("ensure2");
    }
}
