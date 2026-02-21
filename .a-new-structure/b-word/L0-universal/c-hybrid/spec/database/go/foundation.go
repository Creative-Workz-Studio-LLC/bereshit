//omni:code --go -library
//omni:key B-L0-foundation-loader
//omni:version a-02.00

// ============================================================================
// METADATA
// ============================================================================

// Purpose: Foundation database queries — typed access to SQLite-materialized TOML specs.
// The Triangle: Configuration (TOML) → Data (SQLite) → Code (this package).

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
//   - a-02.00 (2026-02-19) — Remove RWMutex, type-safe table parsing, consistent error wrapping
//   - a-01.00 (2026-02-15) — Initial creation: SQLite-backed foundation queries
//
// # M.3 Interface [INTERFACE]
//
//	Requires: stdlib (1): database/sql, fmt, strings | external (-1): modernc.org/sqlite
//	Used by:  L3-cpisi/core, tools, future OmniCode code generation
//
// Data source: b-word/data/foundation/database/foundation.db
// Schema: specs, type_families, types, operations, books, ternary_scales, dependencies, web_variants
package foundation

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// SETUP makes things EXIST. BODY makes things HAPPEN.
//
// Section order (dependency chain — each layer uses only what's above):
//
//   §01 Imports      — External dependencies (database/sql, json, etc.)
//   §02 Constants    — Database paths and prepared SQL queries
//   §03 Variables    — OmniCode identity (Pragma I1-I4, Metadata C1-C7)
//   §06 Core Types   — Data model structs for all 8 foundation tables
//
// If anything must be hardcoded, it lives here — never scattered through BODY.

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

import (
	"database/sql"
	_ "embed"
	"encoding/json"
	"fmt"
	"path/filepath"
	"strings"

	_ "modernc.org/sqlite" // SQLite driver (pure Go, no CGO)
)

// ──────────────────────────────────────────────────────────────────────────
// 2. Constants
// ──────────────────────────────────────────────────────────────────────────

const (
	// DatabaseDir is the path to the foundation database relative to module root (b-word/).
	DatabaseDir = "data/foundation/database"

	// DatabaseFile is the database filename.
	DatabaseFile = "foundation.db"

	// SchemaVersion is the expected PRAGMA user_version value.
	// Increment when schema changes — Open() rejects mismatches.
	SchemaVersion = 1
)

// requiredTables lists the 8 core tables that must exist in foundation.db.
var requiredTables = [8]string{
	"books", "dependencies", "operations", "specs",
	"ternary_scales", "type_families", "types", "web_variants",
}

// SQL queries — single source of truth in go/sql/*.sql files.
// Shared with Rust via include_str!("../go/sql/...").
// Both drivers handle positional ? parameters.
var (
	//go:embed sql/spec_by_key.sql
	qSpecByKey string
	//go:embed sql/specs_by_domain.sql
	qSpecsByDomain string
	//go:embed sql/all_specs.sql
	qAllSpecs string
	//go:embed sql/spec_count.sql
	qSpecCount string

	//go:embed sql/all_families.sql
	qAllFamilies string
	//go:embed sql/family_by_name.sql
	qFamilyByName string

	//go:embed sql/all_types.sql
	qAllTypes string
	//go:embed sql/type_by_name.sql
	qTypeByName string
	//go:embed sql/types_by_family.sql
	qTypesByFamily string

	//go:embed sql/all_operations.sql
	qAllOps string
	//go:embed sql/operations_by_category.sql
	qOpsByCategory string
	//go:embed sql/operation_by_name.sql
	qOpByName string

	//go:embed sql/all_books.sql
	qAllBooks string
	//go:embed sql/book_by_ordinal.sql
	qBookByOrd string
	//go:embed sql/book_by_name.sql
	qBookByName string
	//go:embed sql/book_by_abbreviation.sql
	qBookByAbbr string

	//go:embed sql/all_scales.sql
	qAllScales string

	//go:embed sql/dependencies_of.sql
	qDepsOf string
	//go:embed sql/dependents_on.sql
	qDepsOn string

	//go:embed sql/all_web_variants.sql
	qAllWebVariants string
)

// ──────────────────────────────────────────────────────────────────────────
// 3. Variables
// ──────────────────────────────────────────────────────────────────────────

// Pragma carries OmniCode identity (I1-I4) for runtime self-awareness.
var Pragma = [][2]string{
	// I1: Core
	{"I1.key", "B-L0-foundation-loader"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.go"},
	{"I1.at", "a-02.00"},
	// I2: Family
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "foundation"},
	// I3: Instance
	{"I3.file", "foundation.go"},
	{"I3.title", "Foundation Database"},
	{"I3.component", "c-hybrid/spec/database"},
	{"I3.path", "L0-universal/c-hybrid/spec/database/go/foundation.go"},
	{"I3.provides", "FOUNDATION_QUERIES"},
	{"I3.brief", "Typed SQLite access to all TOML-materialized foundation data"},
	// I4: Architecture
	{"I4.layer", "L0"},
	{"I4.position", "hybrid/spec/database"},
	{"I4.pattern", "The Triangle — configuration (TOML) → data (SQLite) → code (queries)"},
}

// Metadata carries OmniCode context (C1-C7).
var Metadata = [][2]string{
	// C1: State
	{"C1.version", "a-02.00"},
	{"C1.status", "Active"},
	{"C1.created", "2026-02-15"},
	{"C1.updated", "2026-02-19"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "CreativeWorkzStudio LLC"},
	// C3: Grounding
	{"C3.scripture", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	{"C3.principle", "The Triangle complete — configuration → data → code. The system knows itself."},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "database/sql, encoding/json, fmt, path/filepath, strings"},
	{"C4.requires.external", "modernc.org/sqlite (pure Go, no CGO)"},
	{"C4.consumers", "L3-cpisi/core, tools, future OmniCode code generation"},
	{"C4.integration", "import foundation; db, _ := foundation.Open(moduleRoot); defer db.Close()"},
	{"C4.if_missing", "no typed access to foundation data — consumers parse SQLite directly"},
	// C5: Intent
	{"C5.purpose", "Typed query layer for the foundation database — specs, types, operations, books, scales"},
	{"C5.philosophy", "L0 stays universal. Read-only access. The database is truth materialized from TOML specs."},
	// C6: Roadmap
	{"C6.current", "a-02.00 — 8 table readers, schema validation, type-safe table parsing, unified SQL (go/sql/*.sql), batch queries"},
	{"C6.planned", "Connection pooling, context.Context support"},
	{"C6.limitations", "Read-only — no write operations. Single database path."},
	// C7: Classification
	{"C7.tags", "database, sqlite, foundation, queries, specs, types, operations, books"},
	{"C7.category", "Hybrid"},
	{"C7.domain", "foundation-queries"},
	{"C7.paradigm", "CPI-SI"},
}

// ──────────────────────────────────────────────────────────────────────────
// 6. Core Types
// ──────────────────────────────────────────────────────────────────────────

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
// The database is opened read-only (?mode=ro). SQLite handles
// concurrent read access natively — no application-level mutex needed.
type DB struct {
	db *sql.DB
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// B.1 Helpers
// ──────────────────────────────────────────────────────────────────────────

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

// ──────────────────────────────────────────────────────────────────────────
// B.2 Core Operations (Open/Close)
// ──────────────────────────────────────────────────────────────────────────

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

	if err := validateSchema(conn); err != nil {
		conn.Close()
		return nil, err
	}

	return &DB{db: conn}, nil
}

// validateSchema checks PRAGMA user_version and required tables.
// Fails fast if database is wrong version or missing expected tables.
func validateSchema(conn *sql.DB) error {
	// Check version
	var version int
	if err := conn.QueryRow("PRAGMA user_version").Scan(&version); err != nil {
		return fmt.Errorf("foundation.validateSchema: cannot read user_version: %w", err)
	}
	if version != SchemaVersion {
		return fmt.Errorf("foundation.validateSchema: schema version %d, expected %d", version, SchemaVersion)
	}

	// Check required tables exist
	rows, err := conn.Query("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name")
	if err != nil {
		return fmt.Errorf("foundation.validateSchema: cannot list tables: %w", err)
	}
	defer rows.Close()

	tables := make(map[string]bool)
	for rows.Next() {
		var name string
		if err := rows.Scan(&name); err != nil {
			return fmt.Errorf("foundation.validateSchema: scan error: %w", err)
		}
		tables[name] = true
	}

	for _, t := range requiredTables {
		if !tables[t] {
			return fmt.Errorf("foundation.validateSchema: missing required table %q", t)
		}
	}

	return nil
}

// OpenFromRoot opens the foundation database using module root path.
// Convenience: OpenFromRoot("/path/to/b-word") resolves to
// "/path/to/b-word/data/foundation/database/foundation.db".
func OpenFromRoot(moduleRoot string) (*DB, error) {
	return Open(filepath.Join(moduleRoot, DatabaseDir))
}

// Close releases the database connection.
func (db *DB) Close() error {
	return db.db.Close()
}

// ──────────────────────────────────────────────────────────────────────────
// B.3 Spec Queries
//
// Table: specs
//   id INTEGER PK, key TEXT UNIQUE, file TEXT, title TEXT, type TEXT,
//   component TEXT, role TEXT, domain TEXT, layer TEXT, version TEXT,
//   status TEXT, scripture TEXT, principle TEXT, tags TEXT, provides TEXT,
//   created TEXT, updated TEXT
// ──────────────────────────────────────────────────────────────────────────

// SpecByKey returns a single spec by its unique key.
func (db *DB) SpecByKey(key string) (*Spec, error) {

	return scanSpec(db.db.QueryRow(qSpecByKey, key))
}

// SpecsByDomain returns all specs in a domain ("types", "math", "bible").
func (db *DB) SpecsByDomain(domain string) ([]Spec, error) {

	rows, err := db.db.Query(qSpecsByDomain, domain)
	if err != nil {
		return nil, fmt.Errorf("foundation.SpecsByDomain(%s): %w", domain, err)
	}
	defer rows.Close()
	return scanSpecs(rows)
}

// AllSpecs returns every spec in the database.
func (db *DB) AllSpecs() ([]Spec, error) {

	rows, err := db.db.Query(qAllSpecs)
	if err != nil {
		return nil, fmt.Errorf("foundation.AllSpecs: %w", err)
	}
	defer rows.Close()
	return scanSpecs(rows)
}

// SpecCount returns the number of specs.
func (db *DB) SpecCount() (int, error) {

	var count int
	err := db.db.QueryRow(qSpecCount).Scan(&count)
	return count, err
}

// SpecsByKeys returns multiple specs by their keys in a single query.
// Builds a dynamic IN clause — avoids N+1 lookups when consumers need
// several specs at once. Returns specs in key order.
func (db *DB) SpecsByKeys(keys []string) ([]Spec, error) {
	if len(keys) == 0 {
		return nil, nil
	}
	placeholders := strings.Repeat("?,", len(keys))
	placeholders = placeholders[:len(placeholders)-1] // trim trailing comma
	query := "SELECT id, key, file, title, type, component, role, domain, layer, " +
		"version, status, scripture, principle, tags, provides, created, updated " +
		"FROM specs WHERE key IN (" + placeholders + ") ORDER BY key"

	args := make([]any, len(keys))
	for i, k := range keys {
		args[i] = k
	}

	rows, err := db.db.Query(query, args...)
	if err != nil {
		return nil, fmt.Errorf("foundation.SpecsByKeys: %w", err)
	}
	defer rows.Close()
	return scanSpecs(rows)
}

// ──────────────────────────────────────────────────────────────────────────
// B.4 Type Queries
//
// Table: type_families
//   id INTEGER PK, name TEXT UNIQUE, description TEXT, scripture TEXT,
//   purpose TEXT, count INTEGER
//
// Table: types
//   id INTEGER PK, name TEXT UNIQUE, family TEXT, size_trits INTEGER,
//   size_bits INTEGER, min_value TEXT, max_value TEXT, default_val TEXT,
//   nullable INTEGER, signed INTEGER, states TEXT, description TEXT,
//   layer_role TEXT
// ──────────────────────────────────────────────────────────────────────────

// AllFamilies returns all 10 type families.
func (db *DB) AllFamilies() ([]TypeFamily, error) {

	rows, err := db.db.Query(qAllFamilies)
	if err != nil {
		return nil, fmt.Errorf("foundation.AllFamilies: %w", err)
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

	return scanTypeFamily(db.db.QueryRow(qFamilyByName, name))
}

// AllTypes returns all primitive types.
func (db *DB) AllTypes() ([]Type, error) {

	rows, err := db.db.Query(qAllTypes)
	if err != nil {
		return nil, fmt.Errorf("foundation.AllTypes: %w", err)
	}
	defer rows.Close()
	return scanTypes(rows)
}

// TypeByName returns a single type by name.
func (db *DB) TypeByName(name string) (*Type, error) {

	return scanType(db.db.QueryRow(qTypeByName, name))
}

// TypesByFamily returns all types in a family.
func (db *DB) TypesByFamily(family string) ([]Type, error) {

	rows, err := db.db.Query(qTypesByFamily, family)
	if err != nil {
		return nil, fmt.Errorf("foundation.TypesByFamily(%s): %w", family, err)
	}
	defer rows.Close()
	return scanTypes(rows)
}

// ──────────────────────────────────────────────────────────────────────────
// B.5 Operation Queries
//
// Table: operations
//   id INTEGER PK, name TEXT, category TEXT, description TEXT,
//   input_size INTEGER, table_data TEXT (JSON), properties TEXT (JSON)
// ──────────────────────────────────────────────────────────────────────────

// AllOperations returns all operations.
func (db *DB) AllOperations() ([]Operation, error) {

	rows, err := db.db.Query(qAllOps)
	if err != nil {
		return nil, fmt.Errorf("foundation.AllOperations: %w", err)
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

	rows, err := db.db.Query(qOpsByCategory, category)
	if err != nil {
		return nil, fmt.Errorf("foundation.OperationsByCategory(%s): %w", category, err)
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

	return scanOp(db.db.QueryRow(qOpByName, name, category))
}

// OperationTable parses an operation's table_data into a typed result.
// For unary operations (InputSize == 1), returns []int.
// For binary operations (InputSize == 2), returns [][]int.
//
// The table_data column uses space-separated format (e.g., "[1 0 -1]"),
// not JSON. This method handles both formats for forward compatibility.
//
// Deprecated: Use UnaryTable or BinaryTable for type-safe access.
func (db *DB) OperationTable(op *Operation) (any, error) {
	if op.InputSize == 1 {
		return UnaryTable(op)
	}
	return BinaryTable(op)
}

// UnaryTable parses a unary operation's table_data into []int.
// Handles both space-separated ("[1 0 -1]") and JSON ("[1,0,-1]") formats.
func UnaryTable(op *Operation) ([]int, error) {
	if op.InputSize != 1 {
		return nil, fmt.Errorf("foundation.UnaryTable: operation %q has InputSize %d, not 1",
			op.Name, op.InputSize)
	}

	// Try JSON first (forward compatibility).
	var table []int
	if err := json.Unmarshal([]byte(op.TableData), &table); err == nil {
		return table, nil
	}

	// Parse space-separated format: "[1 0 -1]"
	return parseIntList(op.TableData)
}

// BinaryTable parses a binary operation's table_data into [][]int.
// Handles both space-separated ("[[-1 0 1] [0 0 0] [1 0 -1]]") and JSON formats.
func BinaryTable(op *Operation) ([][]int, error) {
	if op.InputSize != 2 {
		return nil, fmt.Errorf("foundation.BinaryTable: operation %q has InputSize %d, not 2",
			op.Name, op.InputSize)
	}

	// Try JSON first (forward compatibility).
	var table [][]int
	if err := json.Unmarshal([]byte(op.TableData), &table); err == nil {
		return table, nil
	}

	// Parse space-separated format: "[[-1 0 1] [0 0 0] [1 0 -1]]"
	return parseIntMatrix(op.TableData)
}

// parseIntList parses a space-separated int list: "[1 0 -1]" → []int{1, 0, -1}.
func parseIntList(s string) ([]int, error) {
	s = strings.TrimSpace(s)
	if !strings.HasPrefix(s, "[") || !strings.HasSuffix(s, "]") {
		return nil, fmt.Errorf("foundation.parseIntList: expected [...], got %q", s)
	}
	inner := strings.TrimSpace(s[1 : len(s)-1])
	if inner == "" {
		return []int{}, nil
	}

	fields := strings.Fields(inner)
	result := make([]int, len(fields))
	for i, f := range fields {
		var n int
		if _, err := fmt.Sscanf(f, "%d", &n); err != nil {
			return nil, fmt.Errorf("foundation.parseIntList: bad value %q at index %d: %w", f, i, err)
		}
		result[i] = n
	}
	return result, nil
}

// parseIntMatrix parses a space-separated 2D int array:
// "[[-1 0 1] [0 0 0] [1 0 -1]]" → [][]int{{-1,0,1}, {0,0,0}, {1,0,-1}}.
func parseIntMatrix(s string) ([][]int, error) {
	s = strings.TrimSpace(s)
	if !strings.HasPrefix(s, "[[") || !strings.HasSuffix(s, "]]") {
		return nil, fmt.Errorf("foundation.parseIntMatrix: expected [[...]], got %q", s)
	}

	// Strip outer brackets, then split on "] [" boundaries.
	inner := s[1 : len(s)-1] // "[...]...[...]"
	// Split by "] [" — handles "[1 0 -1] [0 0 0] [1 0 -1]".
	parts := strings.Split(inner, "] [")

	var result [][]int
	for i, part := range parts {
		// Clean up remaining brackets from split edges.
		part = strings.TrimPrefix(part, "[")
		part = strings.TrimSuffix(part, "]")
		row, err := parseIntList("[" + strings.TrimSpace(part) + "]")
		if err != nil {
			return nil, fmt.Errorf("foundation.parseIntMatrix: row %d: %w", i, err)
		}
		result = append(result, row)
	}
	return result, nil
}

// ──────────────────────────────────────────────────────────────────────────
// B.6 Book Queries
//
// Table: books
//   id INTEGER PK (ordinal 1-66), name TEXT UNIQUE, abbreviation TEXT,
//   chapters INTEGER, verses INTEGER, range_start INTEGER, range_end INTEGER
// ──────────────────────────────────────────────────────────────────────────

// AllBooks returns all 66 Bible books in ordinal order.
func (db *DB) AllBooks() ([]Book, error) {

	rows, err := db.db.Query(qAllBooks)
	if err != nil {
		return nil, fmt.Errorf("foundation.AllBooks: %w", err)
	}
	defer rows.Close()
	return scanBooks(rows)
}

// BookByOrdinal returns a book by its ordinal (1-66).
func (db *DB) BookByOrdinal(ordinal int) (*Book, error) {

	return scanBook(db.db.QueryRow(qBookByOrd, ordinal))
}

// BookByName returns a book by its full name (e.g., "Genesis").
func (db *DB) BookByName(name string) (*Book, error) {

	return scanBook(db.db.QueryRow(qBookByName, name))
}

// BookByAbbreviation returns a book by abbreviation (e.g., "Gen").
func (db *DB) BookByAbbreviation(abbr string) (*Book, error) {

	return scanBook(db.db.QueryRow(qBookByAbbr, abbr))
}

// ──────────────────────────────────────────────────────────────────────────
// B.7 Scale & Dependency Queries
//
// Table: ternary_scales
//   id INTEGER PK, name TEXT UNIQUE, trit_count INTEGER, states TEXT,
//   bytes INTEGER, layer TEXT
//
// Table: dependencies
//   id INTEGER PK, spec_key TEXT, depends_on TEXT, relation TEXT
// ──────────────────────────────────────────────────────────────────────────

// AllScales returns all ternary scales ordered by trit count.
func (db *DB) AllScales() ([]TernaryScale, error) {

	rows, err := db.db.Query(qAllScales)
	if err != nil {
		return nil, fmt.Errorf("foundation.AllScales: %w", err)
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

	rows, err := db.db.Query(qDepsOf, specKey)
	if err != nil {
		return nil, fmt.Errorf("foundation.DependenciesOf(%s): %w", specKey, err)
	}
	defer rows.Close()
	return scanDeps(rows)
}

// DependentsOn returns what depends on a spec.
func (db *DB) DependentsOn(specKey string) ([]Dependency, error) {

	rows, err := db.db.Query(qDepsOn, specKey)
	if err != nil {
		return nil, fmt.Errorf("foundation.DependentsOn(%s): %w", specKey, err)
	}
	defer rows.Close()
	return scanDeps(rows)
}

// ──────────────────────────────────────────────────────────────────────────
// B.8 Web Variant Queries
//
// Table: web_variants
//   trite INTEGER PK (243-255), book TEXT, chapter INTEGER, verse INTEGER,
//   summary TEXT
// ──────────────────────────────────────────────────────────────────────────

// AllWebVariants returns the 13 WEB-only verses.
func (db *DB) AllWebVariants() ([]WebVariant, error) {

	rows, err := db.db.Query(qAllWebVariants)
	if err != nil {
		return nil, fmt.Errorf("foundation.AllWebVariants: %w", err)
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
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
//
//   Never:
//     - Add write operations (L0 = read-only, writes are L2+ concern)
//     - Change query result types (downstream consumers depend on field names)
//     - Remove pure-Go SQLite driver (ensures zero CGO dependency)
//
//   Careful:
//     - Changing SQL queries (affects result correctness)
//     - Adding new table readers (must match schema in foundation.db)
//     - Type struct field changes (downstream consumers depend on names)
//
//   Safe:
//     - Adding query variations (by_id, by_name, filtered)
//     - Improving doc comments
//     - Adding search/aggregate functions
//
// ──────────────────────────────────────────────────────────────────────────

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
//
//   note: The Triangle complete — Configuration (TOML specs) → Data (SQLite)
//         → Code (this package). Every type, operation, scale, book, and
//         dependency — discoverable by query, not hardcoded.
//
//   scripture: Genesis 1:1 (KJV) — In the beginning God created the heaven
//              and the earth.
//
//   anchor: Foundation is read-only truth. The database materializes what
//           the specs define. Code makes it queryable.
//
// ──────────────────────────────────────────────────────────────────────────

// ============================================================================
// END CLOSING
// ============================================================================
