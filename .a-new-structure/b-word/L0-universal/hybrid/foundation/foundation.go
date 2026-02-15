//omni:code --go -library
//omni:key B-L0-foundation-loader
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

// Package foundation loads and queries the L0 foundation database.
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       B-L0-foundation-loader
//
//	Scripture: "In the beginning God created the heaven and the earth."
//	           — Genesis 1:1
//	Principle: "The triangle: Configuration (TOML) → Data (SQLite) → Code (Go/Rust)"
//	Anchor:    Genesis 1:1
//
//	Type:      Hybrid (ladder + spiral access patterns)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-02-15
//	Version:         a-01.00
//
// # M.2 Version History [HISTORY]
//
//   - a-01.00 (2026-02-15) — Initial creation: SQLite-backed foundation queries
//
// # M.3 Interface [INTERFACE]
//
//	Requires: stdlib (1): database/sql, fmt, sync | external (-1): modernc.org/sqlite
//	Used by:  L3-cpisi/core, tools, future OmniCode code generation
//
// Data source: b-word/data/foundation/database/foundation.db
// Schema: specs, type_families, types, operations, books, ternary_scales, dependencies, web_variants
package foundation

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"path/filepath"
	"sync"

	_ "modernc.org/sqlite" // SQLite driver (pure Go, no CGO)
)

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────

// Spec represents a TOML specification file's metadata.
type Spec struct {
	ID        int
	Key       string
	File      string
	Title     string
	Type      string
	Component string
	Role      string
	Domain    string // "types", "math", "bible"
	Layer     string
	Version   string
	Status    string
	Scripture string
	Principle string
	Tags      string
	Provides  string
	Created   string
	Updated   string
}

// TypeFamily is one of the 10 primitive type families.
type TypeFamily struct {
	ID          int
	Name        string
	Description string
	Scripture   string
	Purpose     string
	Count       int
}

// Type is one of the 23 (or more) primitive types.
type Type struct {
	ID         int
	Name       string
	Family     string
	SizeTrits  int
	SizeBits   int
	MinValue   string
	MaxValue   string
	DefaultVal string
	Nullable   bool
	Signed     bool
	States     string
	Desc       string
	LayerRole  string // atomic, packed, derived, container, boundary, bridge
}

// Operation is a ternary arithmetic lookup table.
type Operation struct {
	ID         int
	Name       string
	Category   string // unary, binary, logic, adder, algorithm
	Desc       string
	InputSize  int
	TableData  string // JSON
	Properties string // JSON
}

// Book is one of the 66 Bible books (from addressing.toml).
type Book struct {
	ID           int // ordinal 1-66
	Name         string
	Abbreviation string
	Chapters     int
	Verses       int
	RangeStart   int
	RangeEnd     int
}

// TernaryScale is a named ternary scale (trit → block_243).
type TernaryScale struct {
	ID        int
	Name      string
	TritCount int
	States    string
	Bytes     int
	Layer     string
}

// Dependency is a cross-reference between specs.
type Dependency struct {
	ID        int
	SpecKey   string
	DependsOn string
	Relation  string // "requires", "consumed_by", "derives_from"
}

// WebVariant is a WEB-only Bible verse mapped to trite spare values.
type WebVariant struct {
	Trite   int // 243-255
	Book    string
	Chapter int
	Verse   int
	Summary string
}

// DB holds an open connection to the foundation database.
// Thread-safe — uses sync.RWMutex for concurrent reads.
type DB struct {
	db *sql.DB
	mu sync.RWMutex
}

// ────────────────────────────────────────────────────────────────
// S.2 Constants
// ────────────────────────────────────────────────────────────────

const (
	// DatabaseDir is the path to the foundation database relative to module root (b-word/).
	DatabaseDir = "data/foundation/database"

	// DatabaseFile is the database filename.
	DatabaseFile = "foundation.db"
)

// ────────────────────────────────────────────────────────────────
// S.3 Prepared Queries
// ────────────────────────────────────────────────────────────────

const (
	// Specs
	qSpecByKey    = `SELECT id, key, file, title, type, component, role, domain, layer, version, status, scripture, principle, tags, provides, created, updated FROM specs WHERE key = ?`
	qSpecsByDomain = `SELECT id, key, file, title, type, component, role, domain, layer, version, status, scripture, principle, tags, provides, created, updated FROM specs WHERE domain = ? ORDER BY key`
	qAllSpecs     = `SELECT id, key, file, title, type, component, role, domain, layer, version, status, scripture, principle, tags, provides, created, updated FROM specs ORDER BY domain, key`
	qSpecCount    = `SELECT COUNT(*) FROM specs`

	// Type families
	qAllFamilies = `SELECT id, name, description, scripture, purpose, count FROM type_families ORDER BY id`
	qFamilyByName = `SELECT id, name, description, scripture, purpose, count FROM type_families WHERE name = ?`

	// Types
	qAllTypes     = `SELECT id, name, family, size_trits, size_bits, min_value, max_value, default_val, nullable, signed, states, description, layer_role FROM types ORDER BY id`
	qTypeByName   = `SELECT id, name, family, size_trits, size_bits, min_value, max_value, default_val, nullable, signed, states, description, layer_role FROM types WHERE name = ?`
	qTypesByFamily = `SELECT id, name, family, size_trits, size_bits, min_value, max_value, default_val, nullable, signed, states, description, layer_role FROM types WHERE family = ? ORDER BY id`

	// Operations
	qAllOps       = `SELECT id, name, category, description, input_size, table_data, properties FROM operations ORDER BY category, name`
	qOpsByCategory = `SELECT id, name, category, description, input_size, table_data, properties FROM operations WHERE category = ? ORDER BY name`
	qOpByName     = `SELECT id, name, category, description, input_size, table_data, properties FROM operations WHERE name = ? AND category = ?`

	// Books
	qAllBooks     = `SELECT id, name, abbreviation, chapters, verses, range_start, range_end FROM books ORDER BY id`
	qBookByOrd    = `SELECT id, name, abbreviation, chapters, verses, range_start, range_end FROM books WHERE id = ?`
	qBookByName   = `SELECT id, name, abbreviation, chapters, verses, range_start, range_end FROM books WHERE name = ?`
	qBookByAbbr   = `SELECT id, name, abbreviation, chapters, verses, range_start, range_end FROM books WHERE abbreviation = ?`

	// Scales
	qAllScales = `SELECT id, name, trit_count, states, bytes, layer FROM ternary_scales ORDER BY trit_count`

	// Dependencies
	qDepsOf   = `SELECT id, spec_key, depends_on, relation FROM dependencies WHERE spec_key = ? ORDER BY relation, depends_on`
	qDepsOn   = `SELECT id, spec_key, depends_on, relation FROM dependencies WHERE depends_on = ? ORDER BY relation, spec_key`

	// Web variants
	qAllWebVariants = `SELECT trite, book, chapter, verse, summary FROM web_variants ORDER BY trite`
)

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Helpers
// ────────────────────────────────────────────────────────────────

func scanSpec(scanner interface{ Scan(...any) error }) (*Spec, error) {
	s := &Spec{}
	err := scanner.Scan(
		&s.ID, &s.Key, &s.File, &s.Title, &s.Type, &s.Component,
		&s.Role, &s.Domain, &s.Layer, &s.Version, &s.Status,
		&s.Scripture, &s.Principle, &s.Tags, &s.Provides,
		&s.Created, &s.Updated,
	)
	if err != nil {
		return nil, err
	}
	return s, nil
}

func scanSpecs(rows *sql.Rows) ([]Spec, error) {
	var specs []Spec
	for rows.Next() {
		s, err := scanSpec(rows)
		if err != nil {
			return specs, err
		}
		specs = append(specs, *s)
	}
	return specs, rows.Err()
}

func scanTypeFamily(scanner interface{ Scan(...any) error }) (*TypeFamily, error) {
	tf := &TypeFamily{}
	err := scanner.Scan(&tf.ID, &tf.Name, &tf.Description, &tf.Scripture, &tf.Purpose, &tf.Count)
	if err != nil {
		return nil, err
	}
	return tf, nil
}

func scanType(scanner interface{ Scan(...any) error }) (*Type, error) {
	t := &Type{}
	var nullable, signed int
	err := scanner.Scan(
		&t.ID, &t.Name, &t.Family, &t.SizeTrits, &t.SizeBits,
		&t.MinValue, &t.MaxValue, &t.DefaultVal,
		&nullable, &signed,
		&t.States, &t.Desc, &t.LayerRole,
	)
	if err != nil {
		return nil, err
	}
	t.Nullable = nullable != 0
	t.Signed = signed != 0
	return t, nil
}

func scanTypes(rows *sql.Rows) ([]Type, error) {
	var types []Type
	for rows.Next() {
		t, err := scanType(rows)
		if err != nil {
			return types, err
		}
		types = append(types, *t)
	}
	return types, rows.Err()
}

func scanOp(scanner interface{ Scan(...any) error }) (*Operation, error) {
	o := &Operation{}
	err := scanner.Scan(&o.ID, &o.Name, &o.Category, &o.Desc, &o.InputSize, &o.TableData, &o.Properties)
	if err != nil {
		return nil, err
	}
	return o, nil
}

func scanBook(scanner interface{ Scan(...any) error }) (*Book, error) {
	b := &Book{}
	err := scanner.Scan(&b.ID, &b.Name, &b.Abbreviation, &b.Chapters, &b.Verses, &b.RangeStart, &b.RangeEnd)
	if err != nil {
		return nil, err
	}
	return b, nil
}

func scanBooks(rows *sql.Rows) ([]Book, error) {
	var books []Book
	for rows.Next() {
		b, err := scanBook(rows)
		if err != nil {
			return books, err
		}
		books = append(books, *b)
	}
	return books, rows.Err()
}

func scanScale(scanner interface{ Scan(...any) error }) (*TernaryScale, error) {
	s := &TernaryScale{}
	err := scanner.Scan(&s.ID, &s.Name, &s.TritCount, &s.States, &s.Bytes, &s.Layer)
	if err != nil {
		return nil, err
	}
	return s, nil
}

func scanDep(scanner interface{ Scan(...any) error }) (*Dependency, error) {
	d := &Dependency{}
	err := scanner.Scan(&d.ID, &d.SpecKey, &d.DependsOn, &d.Relation)
	if err != nil {
		return nil, err
	}
	return d, nil
}

func scanDeps(rows *sql.Rows) ([]Dependency, error) {
	var deps []Dependency
	for rows.Next() {
		d, err := scanDep(rows)
		if err != nil {
			return deps, err
		}
		deps = append(deps, *d)
	}
	return deps, rows.Err()
}

// ────────────────────────────────────────────────────────────────
// B.2 Core Operations (Open/Close)
// ────────────────────────────────────────────────────────────────

// Open opens the foundation database from a directory path.
// The dbDir parameter is the absolute path to the database directory
// (typically: moduleRoot + "/data/foundation/database").
//
// Opens read-only — foundation database is reference data.
func Open(dbDir string) (*DB, error) {
	dbPath := filepath.Join(dbDir, DatabaseFile)

	conn, err := sql.Open("sqlite", dbPath+"?mode=ro")
	if err != nil {
		return nil, fmt.Errorf("foundation.Open: %w", err)
	}

	if err := conn.Ping(); err != nil {
		conn.Close()
		return nil, fmt.Errorf("foundation.Open: ping failed: %w", err)
	}

	return &DB{db: conn}, nil
}

// OpenFromRoot opens the foundation database using module root path.
// Convenience: OpenFromRoot("/path/to/b-word") resolves to
// "/path/to/b-word/data/foundation/database/foundation.db".
func OpenFromRoot(moduleRoot string) (*DB, error) {
	return Open(filepath.Join(moduleRoot, DatabaseDir))
}

// Close releases the database connection.
func (db *DB) Close() error {
	db.mu.Lock()
	defer db.mu.Unlock()
	return db.db.Close()
}

// ────────────────────────────────────────────────────────────────
// B.3 Spec Queries
// ────────────────────────────────────────────────────────────────

// SpecByKey returns a single spec by its unique key.
func (db *DB) SpecByKey(key string) (*Spec, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanSpec(db.db.QueryRow(qSpecByKey, key))
}

// SpecsByDomain returns all specs in a domain ("types", "math", "bible").
func (db *DB) SpecsByDomain(domain string) ([]Spec, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qSpecsByDomain, domain)
	if err != nil {
		return nil, fmt.Errorf("SpecsByDomain(%s): %w", domain, err)
	}
	defer rows.Close()
	return scanSpecs(rows)
}

// AllSpecs returns every spec in the database.
func (db *DB) AllSpecs() ([]Spec, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllSpecs)
	if err != nil {
		return nil, fmt.Errorf("AllSpecs: %w", err)
	}
	defer rows.Close()
	return scanSpecs(rows)
}

// SpecCount returns the number of specs.
func (db *DB) SpecCount() (int, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	var count int
	err := db.db.QueryRow(qSpecCount).Scan(&count)
	return count, err
}

// ────────────────────────────────────────────────────────────────
// B.4 Type Queries
// ────────────────────────────────────────────────────────────────

// AllFamilies returns all 10 type families.
func (db *DB) AllFamilies() ([]TypeFamily, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllFamilies)
	if err != nil {
		return nil, fmt.Errorf("AllFamilies: %w", err)
	}
	defer rows.Close()
	var families []TypeFamily
	for rows.Next() {
		tf, err := scanTypeFamily(rows)
		if err != nil {
			return families, err
		}
		families = append(families, *tf)
	}
	return families, rows.Err()
}

// FamilyByName returns a single type family.
func (db *DB) FamilyByName(name string) (*TypeFamily, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanTypeFamily(db.db.QueryRow(qFamilyByName, name))
}

// AllTypes returns all primitive types.
func (db *DB) AllTypes() ([]Type, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllTypes)
	if err != nil {
		return nil, fmt.Errorf("AllTypes: %w", err)
	}
	defer rows.Close()
	return scanTypes(rows)
}

// TypeByName returns a single type by name.
func (db *DB) TypeByName(name string) (*Type, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanType(db.db.QueryRow(qTypeByName, name))
}

// TypesByFamily returns all types in a family.
func (db *DB) TypesByFamily(family string) ([]Type, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qTypesByFamily, family)
	if err != nil {
		return nil, fmt.Errorf("TypesByFamily(%s): %w", family, err)
	}
	defer rows.Close()
	return scanTypes(rows)
}

// ────────────────────────────────────────────────────────────────
// B.5 Operation Queries
// ────────────────────────────────────────────────────────────────

// AllOperations returns all operations.
func (db *DB) AllOperations() ([]Operation, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllOps)
	if err != nil {
		return nil, fmt.Errorf("AllOperations: %w", err)
	}
	defer rows.Close()
	var ops []Operation
	for rows.Next() {
		o, err := scanOp(rows)
		if err != nil {
			return ops, err
		}
		ops = append(ops, *o)
	}
	return ops, rows.Err()
}

// OperationsByCategory returns operations in a category ("unary", "binary", "logic", "adder").
func (db *DB) OperationsByCategory(category string) ([]Operation, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qOpsByCategory, category)
	if err != nil {
		return nil, fmt.Errorf("OperationsByCategory(%s): %w", category, err)
	}
	defer rows.Close()
	var ops []Operation
	for rows.Next() {
		o, err := scanOp(rows)
		if err != nil {
			return ops, err
		}
		ops = append(ops, *o)
	}
	return ops, rows.Err()
}

// OperationByName returns a single operation by name and category.
func (db *DB) OperationByName(name, category string) (*Operation, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanOp(db.db.QueryRow(qOpByName, name, category))
}

// OperationTable parses an operation's table_data JSON into a slice.
// For unary operations, returns []int (flat). For binary, returns [][]int (2D).
func (db *DB) OperationTable(op *Operation) (any, error) {
	if op.InputSize == 1 {
		var table []int
		err := json.Unmarshal([]byte(op.TableData), &table)
		return table, err
	}
	var table [][]int
	err := json.Unmarshal([]byte(op.TableData), &table)
	return table, err
}

// ────────────────────────────────────────────────────────────────
// B.6 Book Queries
// ────────────────────────────────────────────────────────────────

// AllBooks returns all 66 Bible books in ordinal order.
func (db *DB) AllBooks() ([]Book, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllBooks)
	if err != nil {
		return nil, fmt.Errorf("AllBooks: %w", err)
	}
	defer rows.Close()
	return scanBooks(rows)
}

// BookByOrdinal returns a book by its ordinal (1-66).
func (db *DB) BookByOrdinal(ordinal int) (*Book, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanBook(db.db.QueryRow(qBookByOrd, ordinal))
}

// BookByName returns a book by its full name (e.g., "Genesis").
func (db *DB) BookByName(name string) (*Book, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanBook(db.db.QueryRow(qBookByName, name))
}

// BookByAbbreviation returns a book by abbreviation (e.g., "Gen").
func (db *DB) BookByAbbreviation(abbr string) (*Book, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanBook(db.db.QueryRow(qBookByAbbr, abbr))
}

// ────────────────────────────────────────────────────────────────
// B.7 Scale & Dependency Queries
// ────────────────────────────────────────────────────────────────

// AllScales returns all ternary scales ordered by trit count.
func (db *DB) AllScales() ([]TernaryScale, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllScales)
	if err != nil {
		return nil, fmt.Errorf("AllScales: %w", err)
	}
	defer rows.Close()
	var scales []TernaryScale
	for rows.Next() {
		s, err := scanScale(rows)
		if err != nil {
			return scales, err
		}
		scales = append(scales, *s)
	}
	return scales, rows.Err()
}

// DependenciesOf returns what a spec depends on.
func (db *DB) DependenciesOf(specKey string) ([]Dependency, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qDepsOf, specKey)
	if err != nil {
		return nil, fmt.Errorf("DependenciesOf(%s): %w", specKey, err)
	}
	defer rows.Close()
	return scanDeps(rows)
}

// DependentsOn returns what depends on a spec.
func (db *DB) DependentsOn(specKey string) ([]Dependency, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qDepsOn, specKey)
	if err != nil {
		return nil, fmt.Errorf("DependentsOn(%s): %w", specKey, err)
	}
	defer rows.Close()
	return scanDeps(rows)
}

// ────────────────────────────────────────────────────────────────
// B.8 Web Variant Queries
// ────────────────────────────────────────────────────────────────

// AllWebVariants returns the 13 WEB-only verses.
func (db *DB) AllWebVariants() ([]WebVariant, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllWebVariants)
	if err != nil {
		return nil, fmt.Errorf("AllWebVariants: %w", err)
	}
	defer rows.Close()
	var variants []WebVariant
	for rows.Next() {
		v := WebVariant{}
		if err := rows.Scan(&v.Trite, &v.Book, &v.Chapter, &v.Verse, &v.Summary); err != nil {
			return variants, err
		}
		variants = append(variants, v)
	}
	return variants, rows.Err()
}

// ============================================================================
// CLOSING
// ============================================================================
//
// This package provides L0 foundation access to the database that materializes
// all TOML specifications into queryable records. The Triangle is complete:
//
//   Configuration (TOML specs) → Data (SQLite) → Code (this package)
//
// Every type, operation, scale, book, and dependency is discoverable by query
// rather than hardcoded. The system knows itself.
//
// "In the beginning God created the heaven and the earth." — Genesis 1:1
//
// Dependencies: modernc.org/sqlite (pure Go, no CGO)
// Data source: b-word/data/foundation/database/foundation.db
//
// Safe to add: New query methods, search functions, aggregate queries
// Modify with care: Type struct fields, database schema assumptions
// Never modify: Read-only access pattern, table names
