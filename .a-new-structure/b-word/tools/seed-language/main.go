// #!omni code --go
//
// seed-language — Populate L1 language database from TOML specs.
//
// Reads all TOML specifications under L1-omnicode/ladder/language/
// and seeds a SQLite database at data/language/database/language.db.
//
// The triangle: TOML (Configuration) → SQLite (Data) → Go/Rust (Code)
//
// "In the beginning was the Word, and the Word was with God,
//  and the Word was God." — John 1:1

package main

import (
	"database/sql"
	"encoding/json"
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
-- L1 Language Database Schema
-- "In the beginning was the Word" — John 1:1
--
-- Tables mirror the four TOML specs: keywords, syntax, operators, extensions
-- Plus specs/dependencies for metadata tracking.

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

-- ─────────────────────────────────────────────────────────────
-- dependencies: Cross-references between specs
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dependencies (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    spec_key    TEXT    NOT NULL,
    depends_on  TEXT    NOT NULL,
    relation    TEXT    NOT NULL DEFAULT 'requires',
    UNIQUE(spec_key, depends_on, relation)
);

-- ─────────────────────────────────────────────────────────────
-- keywords: Individual OmniCode keywords from keywords.toml
-- ─────────────────────────────────────────────────────────────
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

-- ─────────────────────────────────────────────────────────────
-- syntax_patterns: Sentence syntax patterns from syntax.toml
-- ─────────────────────────────────────────────────────────────
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

-- ─────────────────────────────────────────────────────────────
-- token_types: Lexer token types from omnicode-operators.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS token_types (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    NOT NULL UNIQUE,
    token_id    TEXT    NOT NULL,
    meaning     TEXT    NOT NULL DEFAULT '',
    members     TEXT    NOT NULL DEFAULT '',
    emitted_by  TEXT    NOT NULL DEFAULT '',
    subtypes    TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- precedence_groups: Operator precedence from omnicode-operators.toml
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS precedence_groups (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    level       INTEGER NOT NULL,
    name        TEXT    NOT NULL UNIQUE,
    operators   TEXT    NOT NULL DEFAULT '',
    note        TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- operators: Symbol operators from omnicode-operators.toml
-- ─────────────────────────────────────────────────────────────
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

-- ─────────────────────────────────────────────────────────────
-- keyword_groups: COMMAND + LANGUAGE keyword groups from operators spec
-- ─────────────────────────────────────────────────────────────
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

-- ─────────────────────────────────────────────────────────────
-- phrasal_operators: Multi-word operator sequences
-- ─────────────────────────────────────────────────────────────
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

-- ─────────────────────────────────────────────────────────────
-- disambiguation_rules: Context-dependent keyword resolution
-- ─────────────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS disambiguation_rules (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    keyword     TEXT    NOT NULL UNIQUE,
    meanings    TEXT    NOT NULL DEFAULT '',
    resolution  TEXT    NOT NULL DEFAULT ''
);

-- ─────────────────────────────────────────────────────────────
-- extensions: OmniCode file extension registry from extensions.toml
-- ─────────────────────────────────────────────────────────────
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

-- Indexes for common queries
CREATE INDEX IF NOT EXISTS idx_specs_domain ON specs(domain);
CREATE INDEX IF NOT EXISTS idx_keywords_category ON keywords(category);
CREATE INDEX IF NOT EXISTS idx_syntax_category ON syntax_patterns(category);
CREATE INDEX IF NOT EXISTS idx_operators_category ON operators(category);
CREATE INDEX IF NOT EXISTS idx_operators_token_type ON operators(token_type);
CREATE INDEX IF NOT EXISTS idx_extensions_category ON extensions(category);
CREATE INDEX IF NOT EXISTS idx_dependencies_spec ON dependencies(spec_key);
`

// ═══════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════

func main() {
	root := findRoot()
	if root == "" {
		log.Fatal("Cannot find b-word root. Run from within the b-word directory tree.")
	}

	dbPath := filepath.Join(root, "data", "language", "database", "language.db")
	fmt.Printf("Seeding language database: %s\n", dbPath)

	// Ensure directory exists
	os.MkdirAll(filepath.Dir(dbPath), 0755)

	// Remove existing database to rebuild fresh
	os.Remove(dbPath)

	db, err := sql.Open("sqlite", dbPath)
	if err != nil {
		log.Fatalf("open database: %v", err)
	}
	defer db.Close()

	if _, err := db.Exec(schema); err != nil {
		log.Fatalf("create schema: %v", err)
	}

	langDir := filepath.Join(root, "L1-omnicode", "ladder", "language")

	// Seed specs from all 4 TOML files
	specFiles := []string{"keywords.toml", "syntax.toml", "omnicode-operators.toml", "extensions.toml"}
	for _, f := range specFiles {
		path := filepath.Join(langDir, f)
		if _, err := os.Stat(path); err == nil {
			seedSpec(db, path)
		}
	}

	// Seed domain-specific tables
	seedKeywords(db, langDir)
	seedSyntaxPatterns(db, langDir)
	seedTokenTypes(db, langDir)
	seedPrecedenceGroups(db, langDir)
	seedOperators(db, langDir)
	seedKeywordGroups(db, langDir)
	seedPhrasalOperators(db, langDir)
	seedDisambiguationRules(db, langDir)
	seedExtensions(db, langDir)

	fmt.Println("\nDone.")
	printSummary(db)
}

func findRoot() string {
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

// ─────────────────────────────────────────────────────────────
// Seed specs + dependencies (metadata from all TOML files)
// ─────────────────────────────────────────────────────────────

func seedSpec(db *sql.DB, path string) {
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
	m4 := getMap(meta, "M4_grounding")
	m5 := getMap(meta, "M5_dependencies")
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
		VALUES (?, ?, ?, ?, ?, ?, 'language', 'L1', ?, ?, ?, ?, ?, ?, ?, ?)`,
		key, file, title,
		getString(m1, "type"),
		getString(m1, "component"),
		getString(m1, "role"),
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
	if m5 != nil {
		for _, dep := range getStringArray(m5, "requires") {
			db.Exec(`INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?, ?, 'requires')`,
				key, dep)
		}
		for _, dep := range getStringArray(m5, "consumers") {
			db.Exec(`INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?, ?, 'consumed_by')`,
				dep, key)
		}
		if d := getString(m5, "derives_from"); d != "" {
			db.Exec(`INSERT OR IGNORE INTO dependencies (spec_key, depends_on, relation) VALUES (?, ?, 'derives_from')`,
				key, d)
		}
	}

	fmt.Printf("  spec: %s\n", key)
}

// ─────────────────────────────────────────────────────────────
// Seed keywords from keywords.toml
// ─────────────────────────────────────────────────────────────

func seedKeywords(db *sql.DB, langDir string) {
	path := filepath.Join(langDir, "keywords.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read keywords.toml: %v", err)
		return
	}

	// Keywords are organized as [category.name] with fields:
	// keyword, category, role, scripture, scripture_text, example, reserved, value
	count := 0
	skipKeys := map[string]bool{
		"_pragma": true, "_metadata": true, "_content": true,
		"_validation": true, "_closing": true, "extensions": true,
	}

	for category, v := range raw {
		if skipKeys[category] {
			continue
		}
		catMap, ok := v.(map[string]any)
		if !ok {
			continue
		}

		for _, kv := range catMap {
			entry, ok := kv.(map[string]any)
			if !ok {
				continue
			}
			kw := getString(entry, "keyword")
			if kw == "" {
				continue
			}

			reserved := 1
			if r, ok := entry["reserved"]; ok {
				if b, ok := r.(bool); ok && !b {
					reserved = 0
				}
			}

			_, err := db.Exec(`INSERT OR IGNORE INTO keywords
				(keyword, category, role, scripture, scripture_text, example, reserved, value)
				VALUES (?, ?, ?, ?, ?, ?, ?, ?)`,
				kw,
				getString(entry, "category"),
				getString(entry, "role"),
				getString(entry, "scripture"),
				getString(entry, "scripture_text"),
				getString(entry, "example"),
				reserved,
				getString(entry, "value"),
			)
			if err != nil {
				log.Printf("Warning: inserting keyword %s: %v", kw, err)
			}
			count++
		}
	}

	fmt.Printf("  keywords: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed syntax patterns from syntax.toml
// ─────────────────────────────────────────────────────────────

func seedSyntaxPatterns(db *sql.DB, langDir string) {
	path := filepath.Join(langDir, "syntax.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read syntax.toml: %v", err)
		return
	}

	count := 0
	skipKeys := map[string]bool{
		"_pragma": true, "_metadata": true, "_content": true,
		"_validation": true, "_closing": true, "extensions": true,
	}

	for category, v := range raw {
		if skipKeys[category] {
			continue
		}
		catMap, ok := v.(map[string]any)
		if !ok {
			continue
		}

		for name, pv := range catMap {
			entry, ok := pv.(map[string]any)
			if !ok {
				continue
			}
			pattern := getString(entry, "pattern")
			if pattern == "" {
				continue
			}

			_, err := db.Exec(`INSERT OR IGNORE INTO syntax_patterns
				(name, category, pattern, maps_to, example, scripture, notes)
				VALUES (?, ?, ?, ?, ?, ?, ?)`,
				name,
				category,
				pattern,
				getString(entry, "maps_to"),
				getString(entry, "example"),
				getString(entry, "scripture"),
				getString(entry, "notes"),
			)
			if err != nil {
				log.Printf("Warning: inserting syntax pattern %s: %v", name, err)
			}
			count++
		}
	}

	fmt.Printf("  syntax_patterns: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed token types from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

func seedTokenTypes(db *sql.DB, langDir string) {
	raw := loadOperators(langDir)
	if raw == nil {
		return
	}

	tokenTypes := getMap(raw, "token_types")
	if tokenTypes == nil {
		return
	}

	count := 0
	for name, v := range tokenTypes {
		entry, ok := v.(map[string]any)
		if !ok {
			continue
		}

		members := toJSON(getStringArray(entry, "members"))
		subtypes := toJSON(getStringArray(entry, "subtypes"))

		_, err := db.Exec(`INSERT OR IGNORE INTO token_types
			(name, token_id, meaning, members, emitted_by, subtypes)
			VALUES (?, ?, ?, ?, ?, ?)`,
			name,
			getString(entry, "id"),
			getString(entry, "meaning"),
			members,
			getString(entry, "emitted_by"),
			subtypes,
		)
		if err != nil {
			log.Printf("Warning: inserting token type %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  token_types: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed precedence groups from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

func seedPrecedenceGroups(db *sql.DB, langDir string) {
	raw := loadOperators(langDir)
	if raw == nil {
		return
	}

	precedence := getMap(raw, "precedence")
	if precedence == nil {
		return
	}

	groups, ok := precedence["groups"]
	if !ok {
		return
	}

	// BurntSushi/toml decodes [[array.of.tables]] as []map[string]any
	count := 0
	switch arr := groups.(type) {
	case []map[string]any:
		for _, entry := range arr {
			insertPrecedenceGroup(db, entry)
			count++
		}
	case []any:
		for _, g := range arr {
			if entry, ok := g.(map[string]any); ok {
				insertPrecedenceGroup(db, entry)
				count++
			}
		}
	default:
		log.Printf("Warning: unexpected type for precedence.groups: %T", groups)
		return
	}

	fmt.Printf("  precedence_groups: %d entries\n", count)
}

func insertPrecedenceGroup(db *sql.DB, entry map[string]any) {
	operators := toJSON(getStringArray(entry, "operators"))
	_, err := db.Exec(`INSERT OR IGNORE INTO precedence_groups
		(level, name, operators, note)
		VALUES (?, ?, ?, ?)`,
		getInt(entry, "level"),
		getString(entry, "name"),
		operators,
		getString(entry, "note"),
	)
	if err != nil {
		log.Printf("Warning: inserting precedence group: %v", err)
	}
}

// ─────────────────────────────────────────────────────────────
// Seed symbol operators from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

func seedOperators(db *sql.DB, langDir string) {
	raw := loadOperators(langDir)
	if raw == nil {
		return
	}

	// 18 operator categories
	categories := []string{
		"pragma", "control", "direction", "binding", "structure", "scope",
		"nesting", "containment", "reference", "punctuation", "emphasis",
		"access", "comparison", "arithmetic", "type_op", "composition",
		"ternary", "comment",
	}

	count := 0
	for _, cat := range categories {
		catMap := getMap(raw, cat)
		if catMap == nil {
			continue
		}

		for name, v := range catMap {
			entry, ok := v.(map[string]any)
			if !ok {
				continue
			}

			examples := toJSON(getStringArray(entry, "examples"))
			if examples == "[]" {
				// Try singular "example"
				if ex := getString(entry, "example"); ex != "" {
					examples = toJSON([]string{ex})
				}
			}

			_, err := db.Exec(`INSERT OR IGNORE INTO operators
				(name, category, symbol, token_type, fixity, meaning, ternary, discovered, examples)
				VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)`,
				name, cat,
				getString(entry, "symbol"),
				getString(entry, "token_type"),
				getString(entry, "fixity"),
				getString(entry, "meaning"),
				getString(entry, "ternary"),
				getString(entry, "discovered"),
				examples,
			)
			if err != nil {
				log.Printf("Warning: inserting operator %s.%s: %v", cat, name, err)
			}
			count++
		}
	}

	fmt.Printf("  operators: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed keyword groups from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

func seedKeywordGroups(db *sql.DB, langDir string) {
	raw := loadOperators(langDir)
	if raw == nil {
		return
	}

	keywords := getMap(raw, "keywords")
	if keywords == nil {
		return
	}

	count := 0

	// Walk two levels: keywords.control.proceed, keywords.declaration, etc.
	for groupName, v := range keywords {
		groupMap, ok := v.(map[string]any)
		if !ok {
			continue
		}

		// Check if this is a leaf (has "token_type") or a nested group
		if _, hasToken := groupMap["token_type"]; hasToken {
			// Leaf node: keywords.declaration, keywords.naming, etc.
			path := "keywords." + groupName
			insertKeywordGroup(db, path, groupMap)
			count++
		} else {
			// Nested group: keywords.control.proceed, keywords.response.affirm, etc.
			for subName, sv := range groupMap {
				subMap, ok := sv.(map[string]any)
				if !ok {
					continue
				}
				path := "keywords." + groupName + "." + subName
				insertKeywordGroup(db, path, subMap)
				count++
			}
		}
	}

	fmt.Printf("  keyword_groups: %d entries\n", count)
}

func insertKeywordGroup(db *sql.DB, path string, entry map[string]any) {
	words := toJSON(getStringArray(entry, "words"))
	patterns := toJSON(getStringArray(entry, "patterns"))

	_, err := db.Exec(`INSERT OR IGNORE INTO keyword_groups
		(group_path, token_type, words, meaning, ternary, syntax_ref, scripture, patterns)
		VALUES (?, ?, ?, ?, ?, ?, ?, ?)`,
		path,
		getString(entry, "token_type"),
		words,
		getString(entry, "meaning"),
		getString(entry, "ternary"),
		getString(entry, "syntax_ref"),
		getString(entry, "scripture"),
		patterns,
	)
	if err != nil {
		log.Printf("Warning: inserting keyword group %s: %v", path, err)
	}
}

// ─────────────────────────────────────────────────────────────
// Seed phrasal operators from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

func seedPhrasalOperators(db *sql.DB, langDir string) {
	raw := loadOperators(langDir)
	if raw == nil {
		return
	}

	phrasal := getMap(raw, "phrasal")
	if phrasal == nil {
		return
	}

	count := 0
	for name, v := range phrasal {
		entry, ok := v.(map[string]any)
		if !ok {
			continue
		}

		words := toJSON(getStringArray(entry, "words"))

		_, err := db.Exec(`INSERT OR IGNORE INTO phrasal_operators
			(name, words, meaning, maps_to, example, scripture, note)
			VALUES (?, ?, ?, ?, ?, ?, ?)`,
			name,
			words,
			getString(entry, "meaning"),
			getString(entry, "maps_to"),
			getString(entry, "example"),
			getString(entry, "scripture"),
			getString(entry, "note"),
		)
		if err != nil {
			log.Printf("Warning: inserting phrasal operator %s: %v", name, err)
		}
		count++
	}

	fmt.Printf("  phrasal_operators: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed disambiguation rules from omnicode-operators.toml
// ─────────────────────────────────────────────────────────────

func seedDisambiguationRules(db *sql.DB, langDir string) {
	raw := loadOperators(langDir)
	if raw == nil {
		return
	}

	disambiguation := getMap(raw, "disambiguation")
	if disambiguation == nil {
		return
	}

	count := 0
	for _, v := range disambiguation {
		entry, ok := v.(map[string]any)
		if !ok {
			continue
		}

		kw := getString(entry, "keyword")
		if kw == "" {
			continue
		}

		meanings := toJSON(getStringArray(entry, "meanings"))

		_, err := db.Exec(`INSERT OR IGNORE INTO disambiguation_rules
			(keyword, meanings, resolution)
			VALUES (?, ?, ?)`,
			kw,
			meanings,
			getString(entry, "resolution"),
		)
		if err != nil {
			log.Printf("Warning: inserting disambiguation rule %s: %v", kw, err)
		}
		count++
	}

	fmt.Printf("  disambiguation_rules: %d entries\n", count)
}

// ─────────────────────────────────────────────────────────────
// Seed extensions from extensions.toml
// ─────────────────────────────────────────────────────────────

func seedExtensions(db *sql.DB, langDir string) {
	path := filepath.Join(langDir, "extensions.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read extensions.toml: %v", err)
		return
	}

	// Three categories: omni, biblical, cognitive
	categories := []string{"omni", "biblical", "cognitive"}
	count := 0

	for _, cat := range categories {
		catMap := getMap(raw, cat)
		if catMap == nil {
			continue
		}

		for _, v := range catMap {
			entry, ok := v.(map[string]any)
			if !ok {
				continue
			}

			ext := getString(entry, "ext")
			if ext == "" {
				continue
			}

			_, err := db.Exec(`INSERT OR IGNORE INTO extensions
				(ext, name, category, phase, biblical_root, operation, description,
				 internal_format, block_pattern, default_type, mime_type,
				 state_mapping, keyword_link, l0_parallel)
				VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`,
				ext,
				getString(entry, "name"),
				getString(entry, "category"),
				getString(entry, "phase"),
				getString(entry, "biblical_root"),
				getString(entry, "operation"),
				getString(entry, "description"),
				getString(entry, "internal_format"),
				getString(entry, "block_pattern"),
				getString(entry, "default_type"),
				getString(entry, "mime_type"),
				getString(entry, "state_mapping"),
				getString(entry, "keyword_link"),
				getString(entry, "l0_parallel"),
			)
			if err != nil {
				log.Printf("Warning: inserting extension %s: %v", ext, err)
			}
			count++
		}
	}

	fmt.Printf("  extensions: %d entries\n", count)
}

// ═══════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════

// loadOperators caches and returns the parsed omnicode-operators.toml.
var operatorsCache map[string]any

func loadOperators(langDir string) map[string]any {
	if operatorsCache != nil {
		return operatorsCache
	}
	path := filepath.Join(langDir, "omnicode-operators.toml")
	var raw map[string]any
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		log.Printf("Warning: cannot read omnicode-operators.toml: %v", err)
		return nil
	}
	operatorsCache = raw
	return raw
}

func printSummary(db *sql.DB) {
	fmt.Println("\n--- Language Database Summary ---")
	tables := []string{
		"specs", "dependencies", "keywords", "syntax_patterns",
		"token_types", "precedence_groups", "operators", "keyword_groups",
		"phrasal_operators", "disambiguation_rules", "extensions",
	}
	total := 0
	for _, t := range tables {
		var count int
		db.QueryRow("SELECT COUNT(*) FROM " + t).Scan(&count)
		fmt.Printf("  %-25s %d rows\n", t, count)
		total += count
	}
	fmt.Printf("  %-25s %d rows\n", "TOTAL", total)
}

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

func toJSON(arr []string) string {
	if arr == nil {
		return "[]"
	}
	b, err := json.Marshal(arr)
	if err != nil {
		return "[]"
	}
	return string(b)
}
