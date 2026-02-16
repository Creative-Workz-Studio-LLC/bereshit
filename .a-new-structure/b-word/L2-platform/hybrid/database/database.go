//omni:code --go -library
//omni:key B-L2-database-database
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L2-database-database
// Purpose: Generic SQLite database infrastructure — open, configure, migrate
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-16
//
// Platform-level database infrastructure usable by any layer.
// Domain-specific repository interfaces and implementations belong
// in their respective layers (L3 for CPI-SI, etc.).
//
// ============================================================================

package database

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"database/sql"
	"fmt"
	"sync"
	"time"

	_ "modernc.org/sqlite" // Pure Go SQLite driver (no CGO)
)

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────

// Config holds database connection settings.
type Config struct {
	Path            string
	MaxOpenConns    int
	MaxIdleConns    int
	ConnMaxLifetime time.Duration
	WALMode         bool
	ForeignKeys     bool
}

// Repository is the minimal interface for any database backend.
// Domain-specific interfaces extend this in their own layers.
type Repository interface {
	// Migrate applies schema migrations.
	Migrate(ctx context.Context) error
	// Close releases database resources.
	Close() error
	// Exec executes a statement, returning rows affected.
	Exec(ctx context.Context, query string, args ...interface{}) (int64, error)
	// Query executes a query, returning generic rows.
	Query(ctx context.Context, query string, args ...interface{}) ([]map[string]interface{}, error)
}

// MultiDB manages multiple named databases.
// Each database is independently opened, migrated, and closed.
// Consumers register databases by name; the coordinator handles lifecycle.
type MultiDB struct {
	mu  sync.RWMutex
	dbs map[string]*sql.DB
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Config
// ────────────────────────────────────────────────────────────────

// DefaultConfig returns sensible defaults for a SQLite database.
func DefaultConfig(path string) Config {
	return Config{
		Path:            path,
		MaxOpenConns:    25,
		MaxIdleConns:    5,
		ConnMaxLifetime: 5 * time.Minute,
		WALMode:         true,
		ForeignKeys:     true,
	}
}

// ────────────────────────────────────────────────────────────────
// B.2 OpenSQLite
// ────────────────────────────────────────────────────────────────

// OpenSQLite opens a SQLite database with the given configuration.
// Applies WAL mode and foreign key enforcement by default.
func OpenSQLite(cfg Config) (*sql.DB, error) {
	dsn := cfg.Path
	pragmas := ""

	if cfg.ForeignKeys {
		pragmas += "&_pragma=foreign_keys(1)"
	}
	if cfg.WALMode {
		pragmas += "&_pragma=journal_mode(WAL)"
	}

	if pragmas != "" {
		dsn += "?" + pragmas[1:] // Strip leading &
	}

	db, err := sql.Open("sqlite", dsn)
	if err != nil {
		return nil, fmt.Errorf("open database %s: %w", cfg.Path, err)
	}

	// Test connection
	if err := db.Ping(); err != nil {
		db.Close()
		return nil, fmt.Errorf("ping database %s: %w", cfg.Path, err)
	}

	// Apply pool configuration
	if cfg.MaxOpenConns > 0 {
		db.SetMaxOpenConns(cfg.MaxOpenConns)
	}
	if cfg.MaxIdleConns > 0 {
		db.SetMaxIdleConns(cfg.MaxIdleConns)
	}
	if cfg.ConnMaxLifetime > 0 {
		db.SetConnMaxLifetime(cfg.ConnMaxLifetime)
	}

	return db, nil
}

// OpenDefault opens a SQLite database with default configuration.
func OpenDefault(path string) (*sql.DB, error) {
	return OpenSQLite(DefaultConfig(path))
}

// ────────────────────────────────────────────────────────────────
// B.3 Migration
// ────────────────────────────────────────────────────────────────

// RunMigration executes a SQL schema against a database.
// Uses CREATE TABLE IF NOT EXISTS pattern — safe to run repeatedly.
func RunMigration(ctx context.Context, db *sql.DB, schema string) error {
	_, err := db.ExecContext(ctx, schema)
	if err != nil {
		return fmt.Errorf("execute migration: %w", err)
	}
	return nil
}

// EnsureColumn adds a column if it doesn't exist (SQLite lacks IF NOT EXISTS for ADD COLUMN).
// Safe "check then add" pattern for schema upgrades.
func EnsureColumn(ctx context.Context, db *sql.DB, table, column, colType string) error {
	// Check if column exists
	query := fmt.Sprintf("PRAGMA table_info(%s)", table)
	rows, err := db.QueryContext(ctx, query)
	if err != nil {
		return fmt.Errorf("check column %s.%s: %w", table, column, err)
	}
	defer rows.Close()

	for rows.Next() {
		var cid int
		var name, coltype string
		var notnull int
		var dfltValue interface{}
		var pk int
		if err := rows.Scan(&cid, &name, &coltype, &notnull, &dfltValue, &pk); err != nil {
			continue
		}
		if name == column {
			return nil // Column already exists
		}
	}

	// Add column
	alter := fmt.Sprintf("ALTER TABLE %s ADD COLUMN %s %s", table, column, colType)
	_, err = db.ExecContext(ctx, alter)
	if err != nil {
		return fmt.Errorf("add column %s.%s: %w", table, column, err)
	}
	return nil
}

// ────────────────────────────────────────────────────────────────
// B.4 MultiDB
// ────────────────────────────────────────────────────────────────

// NewMultiDB creates an empty MultiDB coordinator.
func NewMultiDB() *MultiDB {
	return &MultiDB{
		dbs: make(map[string]*sql.DB),
	}
}

// Register adds a named database to the coordinator.
func (m *MultiDB) Register(name string, db *sql.DB) {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.dbs[name] = db
}

// Get retrieves a named database. Returns nil if not found.
func (m *MultiDB) Get(name string) *sql.DB {
	m.mu.RLock()
	defer m.mu.RUnlock()
	return m.dbs[name]
}

// Names returns all registered database names.
func (m *MultiDB) Names() []string {
	m.mu.RLock()
	defer m.mu.RUnlock()
	names := make([]string, 0, len(m.dbs))
	for name := range m.dbs {
		names = append(names, name)
	}
	return names
}

// CloseAll closes all managed databases, collecting errors.
func (m *MultiDB) CloseAll() error {
	m.mu.Lock()
	defer m.mu.Unlock()

	var firstErr error
	for name, db := range m.dbs {
		if err := db.Close(); err != nil && firstErr == nil {
			firstErr = fmt.Errorf("close %s: %w", name, err)
		}
	}
	m.dbs = make(map[string]*sql.DB)
	return firstErr
}

// MigrateAll runs a migration function against each named database.
func (m *MultiDB) MigrateAll(ctx context.Context, migrator func(name string, db *sql.DB) error) error {
	m.mu.RLock()
	defer m.mu.RUnlock()

	for name, db := range m.dbs {
		if err := migrator(name, db); err != nil {
			return fmt.Errorf("migrate %s: %w", name, err)
		}
	}
	return nil
}

// ────────────────────────────────────────────────────────────────
// B.5 Generic Query Helpers
// ────────────────────────────────────────────────────────────────

// QueryRows executes a query and returns results as []map[string]interface{}.
// Flexible schema — useful for raw SQL access and extensions.
func QueryRows(ctx context.Context, db *sql.DB, query string, args ...interface{}) ([]map[string]interface{}, error) {
	rows, err := db.QueryContext(ctx, query, args...)
	if err != nil {
		return nil, fmt.Errorf("query: %w", err)
	}
	defer rows.Close()

	columns, err := rows.Columns()
	if err != nil {
		return nil, fmt.Errorf("columns: %w", err)
	}

	var results []map[string]interface{}
	for rows.Next() {
		values := make([]interface{}, len(columns))
		pointers := make([]interface{}, len(columns))
		for i := range values {
			pointers[i] = &values[i]
		}

		if err := rows.Scan(pointers...); err != nil {
			return nil, fmt.Errorf("scan: %w", err)
		}

		row := make(map[string]interface{})
		for i, col := range columns {
			row[col] = values[i]
		}
		results = append(results, row)
	}
	return results, rows.Err()
}

// ExecStatement executes a SQL statement and returns rows affected.
func ExecStatement(ctx context.Context, db *sql.DB, query string, args ...interface{}) (int64, error) {
	result, err := db.ExecContext(ctx, query, args...)
	if err != nil {
		return 0, fmt.Errorf("exec: %w", err)
	}
	return result.RowsAffected()
}

// ============================================================================
// CLOSING
// ============================================================================
// Platform database infrastructure — generic SQLite management.
// Domain-specific repository interfaces belong in their own layers.
// "Except the LORD build the house, they labour in vain" — Psalm 127:1
