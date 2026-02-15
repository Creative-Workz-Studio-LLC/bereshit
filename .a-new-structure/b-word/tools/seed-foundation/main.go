// #!omni code --go
//
// seed-foundation — Populate L0 foundation database from TOML specs.
//
// Reads all TOML specifications under L0-universal/ladder/foundation/
// and seeds a SQLite database at data/foundation/database/foundation.db.
//
// The triangle: TOML (Configuration) → SQLite (Data) → Go/Rust (Code)
//
// "In the beginning God created the heaven and the earth." — Genesis 1:1

package main

import (
	"database/sql"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"

	"github.com/BurntSushi/toml"
	_ "modernc.org/sqlite"
)

// ═══════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════

const schema = `
-- L0 Foundation Database Schema
-- "In the beginning God created the heaven and the earth." — Genesis 1:1
--
-- Tables mirror the three TOML domains: types, math, bible
-- Plus a specs table that catalogs all TOML files and a dependencies table.

PRAGMA journal_mode = WAL;
PRAGMA foreign_keys = ON;

-- ─────────────────────────────────────────────────────────────
-- specs: Metadata from every TOML spec file
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS specs (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    key         TEXT    NOT NULL UNIQUE,
    file        TEXT    NOT NULL,
    title       TEXT    NOT NULL,
    type        TEXT    NOT NULL DEFAULT '',
    component   TEXT    NOT NULL DEFAULT '',
    role        TEXT    NOT NULL DEFAULT '',
    domain      TEXT    NOT NULL DEFAULT '',  -- 'types', 'math', 'bible'
    layer       TEXT    NOT NULL DEFAULT 'L0',
    version     TEXT    NOT NULL DEFAULT '',
    status      TEXT    NOT NULL DEFAULT 'Active',
    scripture   TEXT    NOT NULL DEFAULT '',
    principle   TEXT    NOT NULL DEFAULT '',
    tags        TEXT    NOT NULL DEFAULT '',
    provides    TEXT    NOT NULL DEFAULT '',  -- JSON array
    created     TEXT    NOT NULL DEFAULT '',
    updated     TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- type_families: The 10 primitive type families
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS type_families (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL UNIQUE,
    description TEXT    NOT NULL DEFAULT '',
    scripture   TEXT    NOT NULL DEFAULT '',
    purpose     TEXT    NOT NULL DEFAULT '',
    count       INTEGER NOT NULL DEFAULT 0
);

-- ─────────────────────────────────────────────────────────────
-- types: The 23 primitive types
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS types (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL UNIQUE,
    family      TEXT    NOT NULL REFERENCES type_families(name),
    size_trits  INTEGER NOT NULL DEFAULT 0,
    size_bits   INTEGER NOT NULL DEFAULT 0,
    min_value   TEXT    NOT NULL DEFAULT '',  -- text because some are huge
    max_value   TEXT    NOT NULL DEFAULT '',
    default_val TEXT    NOT NULL DEFAULT '0',
    nullable    INTEGER NOT NULL DEFAULT 0,  -- boolean
    signed      INTEGER NOT NULL DEFAULT 1,
    states      TEXT    NOT NULL DEFAULT '',
    description TEXT    NOT NULL DEFAULT '',
    layer_role  TEXT    NOT NULL DEFAULT ''   -- composition category: atomic/packed/derived/container/boundary/bridge
);

-- ─────────────────────────────────────────────────────────────
-- operations: Ternary arithmetic lookup tables
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS operations (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL,
    category    TEXT    NOT NULL,  -- 'unary', 'binary', 'logic', 'adder', 'algorithm'
    description TEXT    NOT NULL DEFAULT '',
    input_size  INTEGER NOT NULL DEFAULT 1,  -- 1=unary, 2=binary, 3=ternary
    table_data  TEXT    NOT NULL DEFAULT '',  -- JSON array of the lookup table
    properties  TEXT    NOT NULL DEFAULT '',  -- JSON object of algebraic properties
    UNIQUE(name, category)
);

-- ─────────────────────────────────────────────────────────────
-- books: The 66 Bible books (from addressing.toml)
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS books (
    id           INTEGER PRIMARY KEY,  -- ordinal 1-66
    name         TEXT    NOT NULL UNIQUE,
    abbreviation TEXT    NOT NULL,
    chapters     INTEGER NOT NULL,
    verses       INTEGER NOT NULL,
    range_start  INTEGER NOT NULL,
    range_end    INTEGER NOT NULL
);

-- ─────────────────────────────────────────────────────────────
-- ternary_scales: Named ternary scales (trit → block_243)
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS ternary_scales (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL UNIQUE,
    trit_count  INTEGER NOT NULL,
    states      TEXT    NOT NULL,  -- text because values get huge
    bytes       INTEGER NOT NULL,
    layer       TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- dependencies: Cross-references between specs
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dependencies (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_key    TEXT    NOT NULL,
    depends_on  TEXT    NOT NULL,
    relation    TEXT    NOT NULL DEFAULT 'requires',  -- 'requires', 'consumed_by', 'derives_from'
    UNIQUE(spec_key, depends_on, relation)
);

-- ─────────────────────────────────────────────────────────────
-- web_variants: WEB-only Bible verses using trite spare values
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS web_variants (
    trite       INTEGER PRIMARY KEY,  -- 243-255
    book        TEXT    NOT NULL,
    chapter     INTEGER NOT NULL,
    verse       INTEGER NOT NULL,
    summary     TEXT    NOT NULL DEFAULT ''
);

-- Indexes for common queries
CREATE INDEX IF NOT EXISTS idx_specs_domain ON specs(domain);
CREATE INDEX IF NOT EXISTS idx_specs_tags ON specs(tags);
CREATE INDEX IF NOT EXISTS idx_types_family ON types(family);
CREATE INDEX IF NOT EXISTS idx_operations_category ON operations(category);
CREATE INDEX IF NOT EXISTS idx_books_abbreviation ON books(abbreviation);
CREATE INDEX IF NOT EXISTS idx_dependencies_spec ON dependencies(spec_key);
`

// ═══════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════

func main() {
	// Resolve b-word root from executable location or CWD
	root := findRoot()
	if root == "" {
		log.Fatal("Cannot find b-word root. Run from within the b-word directory tree.")
	}

	dbPath := filepath.Join(root, "data", "foundation", "database", "foundation.db")
	fmt.Printf("Seeding foundation database: %s\n", dbPath)

	// Remove existing database to rebuild fresh
	os.Remove(dbPath)

	db, err := sql.Open("sqlite", dbPath)
	if err != nil {
		log.Fatalf("open database: %v", err)
	}
	defer db.Close()

	// Create schema
	if _, err := db.Exec(schema); err != nil {
		log.Fatalf("create schema: %v", err)
	}

	foundationDir := filepath.Join(root, "L0-universal", "ladder", "foundation")

	// Seed each domain
	specs := 0
	specs += seedDomain(db, foundationDir, "types")
	specs += seedDomain(db, foundationDir, "math")
	specs += seedDomain(db, foundationDir, "bible")

	// Seed the index.toml at foundation root
	indexPath := filepath.Join(foundationDir, "index.toml")
	if _, err := os.Stat(indexPath); err == nil {
		seedSpec(db, indexPath, "foundation")
		specs++
	}

	// Seed type families and types from primitives-catalog.toml
	seedTypeFamilies(db, foundationDir)
	seedTypes(db, foundationDir)

	// Seed operations from ternary-operations.toml
	seedOperations(db, foundationDir)

	// Seed books from addressing.toml
	seedBooks(db, foundationDir)

	// Seed ternary scales from ternary-definitions.toml
	seedScales(db, foundationDir)

	// Seed web variants from addressing.toml
	seedWebVariants(db, foundationDir)

	fmt.Printf("Done. %d specs seeded.\n", specs)

	// Print summary
	printSummary(db)
}

func findRoot() string {
	// Walk up from CWD looking for Cargo.toml + go.mod (b-word root markers)
	dir, _ := os.Getwd()
	for {
		if _, err := os.Stat(filepath.Join(dir, "Cargo.toml")); err == nil {
			if _, err := os.Stat(filepath.Join(dir, "go.mod")); err == nil {
				return dir
			}
		}
		parent := filepath.Dir(dir)
		if parent == dir {
			return ""
		}
		dir = parent
	}
}

// seedDomain walks a domain directory and seeds all TOML files into the specs table.
func seedDomain(db *sql.DB, foundationDir, domain string) int {
	domainDir := filepath.Join(foundationDir, domain)
	count := 0

	const maxDepth = 3 // domain → subdir → sub-subdir (current max is 2)
	err := filepath.Walk(domainDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return nil // skip errors
		}
		// Depth guard — count path separators relative to domainDir
		rel, _ := filepath.Rel(domainDir, path)
		depth := strings.Count(rel, string(filepath.Separator))
		if info.IsDir() && depth >= maxDepth {
			return filepath.SkipDir
		}
		if info.IsDir() || !strings.HasSuffix(info.Name(), ".toml") {
			return nil
		}
		seedSpec(db, path, domain)
		count++
		return nil
	})
	if err != nil {
		log.Printf("Warning: walking %s: %v", domain, err)
	}

	fmt.Printf("  %s: %d specs\n", domain, count)
	return count
}

// seedSpec reads a single TOML file and inserts its metadata into the specs table.
func seedSpec(db *sql.DB, path, domain string) {
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: parsing %s: %v", path, err)
		return
	}

	pragma := getMap(raw, "_pragma")
	meta := getMap(raw, "_metadata")

	p1 := getMap(pragma, "P1_core")
	key := getString(p1, "key")
	if key == "" {
		key = filepath.Base(path)
	}

	m1 := getMap(meta, "M1_identity")
	m2 := getMap(meta, "M2_state")
	m3 := getMap(meta, "M3_attribution")
	_ = m3
	m4 := getMap(meta, "M4_grounding")
	m5 := getMap(meta, "M5_dependencies")
	_ = m5
	m7 := getMap(meta, "M7_classification")

	p3 := getMap(pragma, "P3_instance")
	provides := getStringArray(p3, "provides")

	title := getString(m1, "title")
	if title == "" {
		title = getString(getMap(pragma, "P5_summary"), "title")
	}

	file := getString(m1, "file")
	if file == "" {
		file = filepath.Base(path)
	}

	_, err := db.Exec(`INSERT OR REPLACE INTO specs
		(key, file, title, type, component, role, domain, layer, version, status,
		 scripture, principle, tags, provides, created, updated)
		VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
		key, file, title,
		getString(m1, "type"),
		getString(m1, "component"),
		getString(m1, "role"),
		domain,
		getString(p3, "layer"),
		getString(m2, "version"),
		getString(m2, "status"),
		getString(m4, "scripture"),
		getString(m4, "principle"),
		getString(m7, "tags"),
		strings.Join(provides, ", "),
		getString(m2, "created"),
		getString(m2, "updated"),
	)
	if err != nil {
		log.Printf("Warning: inserting spec %s: %v", key, err)
	}

	// Seed dependencies
	seedDependencies(db, key, m5)
}

func seedDependencies(db *sql.DB, specKey string, m5 map[string]any) {
	if m5 == nil {
		return
	}

	// requires
	for _, dep := range getStringArray(m5, "requires") {
		db.Exec(`INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?, ?, 'requires')`,
			specKey, dep)
	}

	// consumers
	for _, dep := range getStringArray(m5, "consumers") {
		db.Exec(`INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?, ?, 'consumed_by')`,
			dep, specKey)
	}

	// derives_from
	if d := getString(m5, "derives_from"); d != "" {
		db.Exec(`INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?, ?, 'derives_from')`,
			specKey, d)
	}
}

func seedTypeFamilies(db *sql.DB, foundationDir string) {
	path := filepath.Join(foundationDir, "types", "primitives", "primitives-catalog.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read primitives-catalog.toml: %v", err)
		return
	}

	cc := getMap(raw, "cc")
	identity := getMap(cc, "identity")
	catalog := getMap(identity, "catalog")

	families := []string{
		"storage", "trit", "integer", "float", "text",
		"boolean", "collections", "special", "decimal", "temporal",
	}

	for _, name := range families {
		fam := getMap(catalog, name)
		if fam == nil {
			continue
		}
		_, err := db.Exec(`INSERT OR REPLACE INTO type_families (name, description, scripture, purpose, count)
			VALUES (?, ?, ?, ?, ?)`,
			name,
			getString(fam, "description"),
			getString(fam, "scripture"),
			getString(fam, "purpose"),
			getInt(fam, "count"),
		)
		if err != nil {
			log.Printf("Warning: inserting type_family %s: %v", name, err)
		}
	}

	fmt.Printf("  type_families: %d families\n", len(families))
}

func seedTypes(db *sql.DB, foundationDir string) {
	// Read settings for type details
	path := filepath.Join(foundationDir, "types", "primitives", "primitives-settings.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read primitives-settings.toml: %v", err)
		return
	}

	cc := getMap(raw, "cc")
	settings := getMap(cc, "settings")

	// Read catalog for architecture (composition layers)
	catPath := filepath.Join(foundationDir, "types", "primitives", "primitives-catalog.toml")
	var catRaw map[string]any
	toml.DecodeFile(catPath, &catRaw)
	catCC := getMap(catRaw, "cc")
	catIdent := getMap(catCC, "identity")
	arch := getMap(catIdent, "architecture")
	comp := getMap(arch, "composition")

	// Build a reverse map: type_name → composition layer
	compMap := map[string]string{}
	for layer, v := range comp {
		if arr, ok := v.([]any); ok {
			for _, t := range arr {
				if s, ok := t.(string); ok {
					compMap[s] = layer
				}
			}
		}
	}

	count := 0

	// Process each family's settings
	type typeEntry struct {
		name      string
		family    string
		trits     int
		bits      int
		minVal    string
		maxVal    string
		defVal    string
		nullable  bool
		signed    bool
		states    string
		desc      string
		layerRole string
	}

	var entries []typeEntry

	// Storage types
	storage := getMap(settings, "storage")
	for _, name := range []string{"uint8", "int8"} {
		t := getMap(storage, name)
		if t == nil {
			continue
		}
		entries = append(entries, typeEntry{
			name: name, family: "storage", bits: getInt(t, "size_bits"),
			minVal: fmt.Sprintf("%v", t["min_value"]), maxVal: fmt.Sprintf("%v", t["max_value"]),
			defVal: fmt.Sprintf("%v", t["default"]), signed: getBool(t, "signed"),
			layerRole: compMap[name],
		})
	}

	// Trit types
	tritSec := getMap(settings, "trit")
	for _, name := range []string{"trit", "trit5"} {
		t := getMap(tritSec, name)
		if t == nil {
			continue
		}
		entries = append(entries, typeEntry{
			name: name, family: "trit", trits: getInt(t, "size_trits"),
			minVal: fmt.Sprintf("%v", t["min_value"]), maxVal: fmt.Sprintf("%v", t["max_value"]),
			defVal: fmt.Sprintf("%v", t["default"]), states: fmt.Sprintf("%v", t["states"]),
			layerRole: compMap[name],
		})
	}

	// Integer types
	intSec := getMap(settings, "integer")
	for _, name := range []string{"int9", "uint9", "int27", "uint27", "int81", "uint81", "int243", "uint243"} {
		t := getMap(intSec, name)
		if t == nil {
			continue
		}
		entries = append(entries, typeEntry{
			name: name, family: "integer", trits: getInt(t, "size_trits"),
			minVal: fmt.Sprintf("%v", t["min_value"]), maxVal: fmt.Sprintf("%v", t["max_value"]),
			defVal: fmt.Sprintf("%v", t["default"]), signed: getBool(t, "signed"),
			states: fmt.Sprintf("%v", t["states"]),
			layerRole: compMap[name],
		})
	}

	// Float types
	floatSec := getMap(settings, "float")
	if ft := getMap(floatSec, "float27"); ft != nil {
		entries = append(entries, typeEntry{
			name: "float27", family: "float", trits: getInt(ft, "size_trits"),
			defVal: fmt.Sprintf("%v", ft["default"]), states: "continuous",
			layerRole: compMap["float27"],
		})
	}

	// Text types
	textSec := getMap(settings, "text")
	for _, name := range []string{"text", "char"} {
		t := getMap(textSec, name)
		if t == nil {
			continue
		}
		entries = append(entries, typeEntry{
			name: name, family: "text", nullable: getBool(t, "nullable"),
			defVal: fmt.Sprintf("%v", t["default"]),
			desc:   getString(t, "encoding"),
			layerRole: compMap[name],
		})
	}

	// Boolean
	boolSec := getMap(settings, "boolean")
	if b3 := getMap(boolSec, "bool3"); b3 != nil {
		entries = append(entries, typeEntry{
			name: "bool3", family: "boolean", trits: 1,
			minVal: "-1", maxVal: "1", defVal: "0", states: "3",
			layerRole: compMap["bool3"],
		})
	}

	// Collections
	collSec := getMap(settings, "collections")
	for _, name := range []string{"list", "map", "set"} {
		t := getMap(collSec, name)
		if t == nil {
			continue
		}
		entries = append(entries, typeEntry{
			name: name, family: "collections", nullable: true,
			desc:      getString(t, "description"),
			layerRole: compMap[name],
		})
	}

	// Special
	specSec := getMap(settings, "special")
	for _, name := range []string{"void", "any"} {
		t := getMap(specSec, name)
		if t == nil {
			continue
		}
		entries = append(entries, typeEntry{
			name: name, family: "special",
			desc:      getString(t, "description"),
			layerRole: compMap[name],
		})
	}

	// Decimal
	decSec := getMap(settings, "decimal")
	if d := getMap(decSec, "decimal27"); d != nil {
		entries = append(entries, typeEntry{
			name: "decimal27", family: "decimal", trits: getInt(d, "size_trits"),
			defVal:    fmt.Sprintf("%v", d["default"]),
			layerRole: compMap["decimal27"],
		})
	}

	// Temporal
	tempSec := getMap(settings, "temporal")
	for _, name := range []string{"timestamp", "duration"} {
		t := getMap(tempSec, name)
		if t == nil {
			continue
		}
		entries = append(entries, typeEntry{
			name: name, family: "temporal", trits: getInt(t, "size_trits"),
			desc:      getString(t, "description"),
			layerRole: compMap[name],
		})
	}

	for _, e := range entries {
		nullable := 0
		if e.nullable {
			nullable = 1
		}
		signed := 0
		if e.signed {
			signed = 1
		}
		_, err := db.Exec(`INSERT OR REPLACE INTO types
			(name, family, size_trits, size_bits, min_value, max_value, default_val,
			 nullable, signed, states, description, layer_role)
			VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
			e.name, e.family, e.trits, e.bits, e.minVal, e.maxVal, e.defVal,
			nullable, signed, e.states, e.desc, e.layerRole,
		)
		if err != nil {
			log.Printf("Warning: inserting type %s: %v", e.name, err)
		}
		count++
	}

	fmt.Printf("  types: %d primitive types\n", count)
}

func seedOperations(db *sql.DB, foundationDir string) {
	path := filepath.Join(foundationDir, "math", "ternary-operations.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read ternary-operations.toml: %v", err)
		return
	}

	arith := getMap(raw, "arithmetic")
	count := 0

	// Unary operations (flat arrays)
	for _, name := range []string{"negation", "absolute", "sign", "increment_sat", "decrement_sat"} {
		if v, ok := arith[name]; ok {
			data := fmt.Sprintf("%v", v)
			db.Exec(`INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data)
				VALUES (?, 'unary', ?, 1, ?)`, name, "Unary trit operation", data)
			count++
		}
	}

	// Binary operations (nested tables with .table)
	for _, name := range []string{"addition", "multiplication", "compare", "min", "max",
		"logic_and", "logic_or", "logic_xor", "consensus"} {
		sub := getMap(arith, name)
		if sub == nil {
			continue
		}
		desc := getString(sub, "description")
		data := fmt.Sprintf("%v", sub["table"])
		db.Exec(`INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data)
			VALUES (?, 'binary', ?, 2, ?)`, name, desc, data)
		count++
	}

	// Adders
	ops := getMap(raw, "operations")

	ha := getMap(ops, "half_adder")
	if ha != nil {
		sumData := fmt.Sprintf("%v", ha["sum"])
		carryData := fmt.Sprintf("%v", ha["carry"])
		db.Exec(`INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data, properties)
			VALUES (?, 'adder', ?, 2, ?, ?)`, "half_adder_sum", getString(ha, "description"), sumData, "")
		db.Exec(`INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data, properties)
			VALUES (?, 'adder', ?, 2, ?, ?)`, "half_adder_carry", getString(ha, "description"), carryData, "")
		count += 2
	}

	fa := getMap(ops, "full_adder")
	if fa != nil {
		sumData := fmt.Sprintf("%v", fa["sum"])
		carryData := fmt.Sprintf("%v", fa["carry_out"])
		db.Exec(`INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data)
			VALUES (?, 'adder', ?, 3, ?)`, "full_adder_sum", getString(fa, "description"), sumData)
		db.Exec(`INSERT OR REPLACE INTO operations (name, category, description, input_size, table_data)
			VALUES (?, 'adder', ?, 3, ?)`, "full_adder_carry", getString(fa, "description"), carryData)
		count += 2
	}

	fmt.Printf("  operations: %d lookup tables\n", count)
}

func seedBooks(db *sql.DB, foundationDir string) {
	path := filepath.Join(foundationDir, "bible", "addressing.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read addressing.toml: %v", err)
		return
	}

	books := getMap(raw, "books")
	if books == nil {
		log.Printf("Warning: no [books] section in addressing.toml")
		return
	}

	count := 0
	for _, v := range books {
		book, ok := v.(map[string]any)
		if !ok {
			continue
		}
		name := getString(book, "name")
		if name == "" {
			continue
		}

		rng := getIntArray(book, "range")
		rangeStart, rangeEnd := 0, 0
		if len(rng) == 2 {
			rangeStart = rng[0]
			rangeEnd = rng[1]
		}

		_, err := db.Exec(`INSERT OR REPLACE INTO books (id, name, abbreviation, chapters, verses, range_start, range_end)
			VALUES (?, ?, ?, ?, ?, ?, ?)`,
			getInt(book, "ordinal"),
			name,
			getString(book, "abbreviation"),
			getInt(book, "chapters"),
			getInt(book, "verses"),
			rangeStart,
			rangeEnd,
		)
		if err != nil {
			log.Printf("Warning: inserting book %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  books: %d Bible books\n", count)
}

func seedScales(db *sql.DB, foundationDir string) {
	path := filepath.Join(foundationDir, "math", "ternary-definitions.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read ternary-definitions.toml: %v", err)
		return
	}

	settings := getMap(raw, "settings")
	scales := getMap(settings, "scales")
	if scales == nil {
		log.Printf("Warning: no [settings.scales] in ternary-definitions.toml")
		return
	}

	count := 0
	for name, v := range scales {
		s, ok := v.(map[string]any)
		if !ok {
			continue
		}
		_, err := db.Exec(`INSERT OR REPLACE INTO ternary_scales (name, trit_count, states, bytes, layer)
			VALUES (?, ?, ?, ?, ?)`,
			name,
			getInt(s, "count"),
			fmt.Sprintf("%v", s["states"]),
			getInt(s, "bytes"),
			getString(s, "layer"),
		)
		if err != nil {
			log.Printf("Warning: inserting scale %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  ternary_scales: %d scales\n", count)
}

func seedWebVariants(db *sql.DB, foundationDir string) {
	path := filepath.Join(foundationDir, "bible", "addressing.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		return
	}

	variants := getMap(raw, "variants")
	if variants == nil {
		return
	}

	count := 0
	for _, v := range variants {
		vv, ok := v.(map[string]any)
		if !ok {
			continue
		}
		trite := getInt(vv, "trite")
		if trite == 0 {
			continue
		}
		db.Exec(`INSERT OR REPLACE INTO web_variants (trite, book, chapter, verse, summary)
			VALUES (?, ?, ?, ?, ?)`,
			trite,
			getString(vv, "book"),
			getInt(vv, "chapter"),
			getInt(vv, "verse"),
			getString(vv, "summary"),
		)
		count++
	}

	fmt.Printf("  web_variants: %d variant verses\n", count)
}

func printSummary(db *sql.DB) {
	fmt.Println("\n--- Foundation Database Summary ---")
	tables := []string{"specs", "type_families", "types", "operations", "books", "ternary_scales", "dependencies", "web_variants"}
	for _, t := range tables {
		var count int
		db.QueryRow("SELECT COUNT(*) FROM " + t).Scan(&count)
		fmt.Printf("  %-20s %d rows\n", t, count)
	}
}

// ═══════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════

func getMap(m map[string]any, key string) map[string]any {
	if m == nil {
		return nil
	}
	v, ok := m[key]
	if !ok {
		return nil
	}
	mm, ok := v.(map[string]any)
	if !ok {
		return nil
	}
	return mm
}

func getString(m map[string]any, key string) string {
	if m == nil {
		return ""
	}
	v, ok := m[key]
	if !ok {
		return ""
	}
	s, ok := v.(string)
	if !ok {
		return fmt.Sprintf("%v", v)
	}
	return s
}

func getInt(m map[string]any, key string) int {
	if m == nil {
		return 0
	}
	v, ok := m[key]
	if !ok {
		return 0
	}
	switch n := v.(type) {
	case int64:
		return int(n)
	case float64:
		return int(n)
	case int:
		return n
	default:
		return 0
	}
}

func getBool(m map[string]any, key string) bool {
	if m == nil {
		return false
	}
	v, ok := m[key]
	if !ok {
		return false
	}
	b, ok := v.(bool)
	if !ok {
		return false
	}
	return b
}

func getStringArray(m map[string]any, key string) []string {
	if m == nil {
		return nil
	}
	v, ok := m[key]
	if !ok {
		return nil
	}
	arr, ok := v.([]any)
	if !ok {
		return nil
	}
	result := make([]string, 0, len(arr))
	for _, item := range arr {
		if s, ok := item.(string); ok {
			result = append(result, s)
		}
	}
	return result
}

func getIntArray(m map[string]any, key string) []int {
	if m == nil {
		return nil
	}
	v, ok := m[key]
	if !ok {
		return nil
	}
	arr, ok := v.([]any)
	if !ok {
		return nil
	}
	result := make([]int, 0, len(arr))
	for _, item := range arr {
		switch n := item.(type) {
		case int64:
			result = append(result, int(n))
		case float64:
			result = append(result, int(n))
		}
	}
	return result
}
