//omni:code --go -library
//omni:key B-L1-language-loader
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

// Package language loads and queries the L1 language database.
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       B-L1-language-loader
//
//	Scripture: "In the beginning was the Word, and the Word was with God,
//	           and the Word was God." — John 1:1
//	Principle: "The triangle: Configuration (TOML) -> Data (SQLite) -> Code (Go/Rust)"
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
//   - a-01.00 (2026-02-15) — Initial creation: SQLite-backed language queries
//
// # M.3 Interface [INTERFACE]
//
//	Requires: stdlib (1): database/sql, fmt, sync | external (-1): modernc.org/sqlite
//	Used by:  L3-cpisi/core, tools, future OmniCode compiler pipeline
//
// Data source: b-word/data/language/database/language.db
// Schema: specs, dependencies, keywords, syntax_patterns, token_types,
//
//	precedence_groups, operators, keyword_groups, phrasal_operators,
//	disambiguation_rules, extensions
package language

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

import (
	"database/sql"
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
	Domain    string
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

// Dependency is a cross-reference between specs.
type Dependency struct {
	ID        int
	SpecKey   string
	DependsOn string
	Relation  string // "requires", "consumed_by", "derives_from"
}

// Keyword is an OmniCode language keyword.
type Keyword struct {
	ID            int
	Keyword       string
	Category      string
	Role          string
	Scripture     string
	ScriptureText string
	Example       string
	Reserved      bool
	Value         string
}

// SyntaxPattern is a language syntax rule.
type SyntaxPattern struct {
	ID        int
	Name      string
	Category  string
	Pattern   string
	MapsTo    string
	Example   string
	Scripture string
	Notes     string
}

// TokenType classifies lexer output.
type TokenType struct {
	ID       int
	Name     string
	TokenID  string
	Meaning  string
	Members  string // JSON array
	EmittedBy string
	Subtypes string // JSON array
}

// PrecedenceGroup defines operator precedence.
type PrecedenceGroup struct {
	ID        int
	Level     int
	Name      string
	Operators string // JSON array
	Note      string
}

// Operator is a symbol operator.
type Operator struct {
	ID         int
	Name       string
	Category   string
	Symbol     string
	TokenType  string
	Fixity     string
	Meaning    string
	Ternary    string
	Discovered string
	Examples   string // JSON array
}

// KeywordGroup is a classified set of keywords.
type KeywordGroup struct {
	ID        int
	GroupPath string // e.g., "keywords.control.proceed"
	TokenType string
	Words     string // JSON array
	Meaning   string
	Ternary   string
	SyntaxRef string
	Scripture string
	Patterns  string // JSON array
}

// PhrasalOperator is a multi-word operator.
type PhrasalOperator struct {
	ID       int
	Name     string
	Words    string // JSON array
	Meaning  string
	MapsTo   string
	Example  string
	Scripture string
	Note     string
}

// DisambiguationRule resolves keyword overloading.
type DisambiguationRule struct {
	ID         int
	Keyword    string
	Meanings   string // JSON array
	Resolution string
}

// Extension is a file extension definition.
type Extension struct {
	ID             int
	Ext            string
	Name           string
	Category       string
	Phase          string
	BiblicalRoot   string
	Operation      string
	Description    string
	InternalFormat string
	BlockPattern   string
	DefaultType    string
	MimeType       string
	StateMapping   string
	KeywordLink    string
	L0Parallel     string
}

// DB holds an open connection to the language database.
// Thread-safe — uses sync.RWMutex for concurrent reads.
type DB struct {
	db *sql.DB
	mu sync.RWMutex
}

// ────────────────────────────────────────────────────────────────
// S.2 Constants
// ────────────────────────────────────────────────────────────────

const (
	// DatabaseDir is the path to the language database relative to module root (b-word/).
	DatabaseDir = "data/language/database"

	// DatabaseFile is the database filename.
	DatabaseFile = "language.db"
)

// ────────────────────────────────────────────────────────────────
// S.3 Prepared Queries
// ────────────────────────────────────────────────────────────────

const (
	// Specs
	qSpecByKey = `SELECT id, key, file, title, type, component, role, domain, layer, version, status, scripture, principle, tags, provides, created, updated FROM specs WHERE key = ?`
	qAllSpecs  = `SELECT id, key, file, title, type, component, role, domain, layer, version, status, scripture, principle, tags, provides, created, updated FROM specs ORDER BY key`
	qSpecCount = `SELECT COUNT(*) FROM specs`

	// Dependencies
	qDepsOf = `SELECT id, spec_key, depends_on, relation FROM dependencies WHERE spec_key = ? ORDER BY relation, depends_on`
	qDepsOn = `SELECT id, spec_key, depends_on, relation FROM dependencies WHERE depends_on = ? ORDER BY relation, spec_key`

	// Keywords
	qAllKeywords       = `SELECT id, keyword, category, role, scripture, scripture_text, example, reserved, value FROM keywords ORDER BY category, keyword`
	qKeywordByName     = `SELECT id, keyword, category, role, scripture, scripture_text, example, reserved, value FROM keywords WHERE keyword = ?`
	qKeywordsByCategory = `SELECT id, keyword, category, role, scripture, scripture_text, example, reserved, value FROM keywords WHERE category = ? ORDER BY keyword`

	// Syntax patterns
	qAllSyntax       = `SELECT id, name, category, pattern, maps_to, example, scripture, notes FROM syntax_patterns ORDER BY category, name`
	qSyntaxByCategory = `SELECT id, name, category, pattern, maps_to, example, scripture, notes FROM syntax_patterns WHERE category = ? ORDER BY name`
	qSyntaxByName    = `SELECT id, name, category, pattern, maps_to, example, scripture, notes FROM syntax_patterns WHERE name = ? AND category = ?`

	// Token types
	qAllTokenTypes  = `SELECT id, name, token_id, meaning, members, emitted_by, subtypes FROM token_types ORDER BY name`
	qTokenTypeByName = `SELECT id, name, token_id, meaning, members, emitted_by, subtypes FROM token_types WHERE name = ?`

	// Precedence groups
	qAllPrecedence    = `SELECT id, level, name, operators, note FROM precedence_groups ORDER BY level`
	qPrecedenceByName = `SELECT id, level, name, operators, note FROM precedence_groups WHERE name = ?`

	// Operators
	qAllOperators       = `SELECT id, name, category, symbol, token_type, fixity, meaning, ternary, discovered, examples FROM operators ORDER BY category, name`
	qOperatorsByCategory = `SELECT id, name, category, symbol, token_type, fixity, meaning, ternary, discovered, examples FROM operators WHERE category = ? ORDER BY name`
	qOperatorByName     = `SELECT id, name, category, symbol, token_type, fixity, meaning, ternary, discovered, examples FROM operators WHERE name = ? AND category = ?`

	// Keyword groups
	qAllKeywordGroups = `SELECT id, group_path, token_type, words, meaning, ternary, syntax_ref, scripture, patterns FROM keyword_groups ORDER BY group_path`
	qKeywordGroupByPath = `SELECT id, group_path, token_type, words, meaning, ternary, syntax_ref, scripture, patterns FROM keyword_groups WHERE group_path = ?`

	// Phrasal operators
	qAllPhrasal     = `SELECT id, name, words, meaning, maps_to, example, scripture, note FROM phrasal_operators ORDER BY name`
	qPhrasalByName  = `SELECT id, name, words, meaning, maps_to, example, scripture, note FROM phrasal_operators WHERE name = ?`

	// Disambiguation rules
	qAllDisambig      = `SELECT id, keyword, meanings, resolution FROM disambiguation_rules ORDER BY keyword`
	qDisambigByKeyword = `SELECT id, keyword, meanings, resolution FROM disambiguation_rules WHERE keyword = ?`

	// Extensions
	qAllExtensions       = `SELECT id, ext, name, category, phase, biblical_root, operation, description, internal_format, block_pattern, default_type, mime_type, state_mapping, keyword_link, l0_parallel FROM extensions ORDER BY category, ext`
	qExtensionByExt      = `SELECT id, ext, name, category, phase, biblical_root, operation, description, internal_format, block_pattern, default_type, mime_type, state_mapping, keyword_link, l0_parallel FROM extensions WHERE ext = ?`
	qExtensionsByCategory = `SELECT id, ext, name, category, phase, biblical_root, operation, description, internal_format, block_pattern, default_type, mime_type, state_mapping, keyword_link, l0_parallel FROM extensions WHERE category = ? ORDER BY ext`
)

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Helpers (scan functions)
// ────────────────────────────────────────────────────────────────

type scanner interface{ Scan(...any) error }

func scanSpec(s scanner) (*Spec, error) {
	sp := &Spec{}
	err := s.Scan(
		&sp.ID, &sp.Key, &sp.File, &sp.Title, &sp.Type, &sp.Component,
		&sp.Role, &sp.Domain, &sp.Layer, &sp.Version, &sp.Status,
		&sp.Scripture, &sp.Principle, &sp.Tags, &sp.Provides,
		&sp.Created, &sp.Updated,
	)
	if err != nil {
		return nil, err
	}
	return sp, nil
}

func scanDep(s scanner) (*Dependency, error) {
	d := &Dependency{}
	err := s.Scan(&d.ID, &d.SpecKey, &d.DependsOn, &d.Relation)
	if err != nil {
		return nil, err
	}
	return d, nil
}

func scanKeyword(s scanner) (*Keyword, error) {
	k := &Keyword{}
	var reserved int
	err := s.Scan(&k.ID, &k.Keyword, &k.Category, &k.Role, &k.Scripture, &k.ScriptureText, &k.Example, &reserved, &k.Value)
	if err != nil {
		return nil, err
	}
	k.Reserved = reserved != 0
	return k, nil
}

func scanSyntax(s scanner) (*SyntaxPattern, error) {
	sp := &SyntaxPattern{}
	err := s.Scan(&sp.ID, &sp.Name, &sp.Category, &sp.Pattern, &sp.MapsTo, &sp.Example, &sp.Scripture, &sp.Notes)
	if err != nil {
		return nil, err
	}
	return sp, nil
}

func scanTokenType(s scanner) (*TokenType, error) {
	t := &TokenType{}
	err := s.Scan(&t.ID, &t.Name, &t.TokenID, &t.Meaning, &t.Members, &t.EmittedBy, &t.Subtypes)
	if err != nil {
		return nil, err
	}
	return t, nil
}

func scanPrecedence(s scanner) (*PrecedenceGroup, error) {
	p := &PrecedenceGroup{}
	err := s.Scan(&p.ID, &p.Level, &p.Name, &p.Operators, &p.Note)
	if err != nil {
		return nil, err
	}
	return p, nil
}

func scanOperator(s scanner) (*Operator, error) {
	o := &Operator{}
	err := s.Scan(&o.ID, &o.Name, &o.Category, &o.Symbol, &o.TokenType, &o.Fixity, &o.Meaning, &o.Ternary, &o.Discovered, &o.Examples)
	if err != nil {
		return nil, err
	}
	return o, nil
}

func scanKeywordGroup(s scanner) (*KeywordGroup, error) {
	g := &KeywordGroup{}
	err := s.Scan(&g.ID, &g.GroupPath, &g.TokenType, &g.Words, &g.Meaning, &g.Ternary, &g.SyntaxRef, &g.Scripture, &g.Patterns)
	if err != nil {
		return nil, err
	}
	return g, nil
}

func scanPhrasal(s scanner) (*PhrasalOperator, error) {
	p := &PhrasalOperator{}
	err := s.Scan(&p.ID, &p.Name, &p.Words, &p.Meaning, &p.MapsTo, &p.Example, &p.Scripture, &p.Note)
	if err != nil {
		return nil, err
	}
	return p, nil
}

func scanDisambig(s scanner) (*DisambiguationRule, error) {
	d := &DisambiguationRule{}
	err := s.Scan(&d.ID, &d.Keyword, &d.Meanings, &d.Resolution)
	if err != nil {
		return nil, err
	}
	return d, nil
}

func scanExtension(s scanner) (*Extension, error) {
	e := &Extension{}
	err := s.Scan(
		&e.ID, &e.Ext, &e.Name, &e.Category, &e.Phase,
		&e.BiblicalRoot, &e.Operation, &e.Description,
		&e.InternalFormat, &e.BlockPattern, &e.DefaultType,
		&e.MimeType, &e.StateMapping, &e.KeywordLink, &e.L0Parallel,
	)
	if err != nil {
		return nil, err
	}
	return e, nil
}

// Generic multi-row collector.
func collectRows[T any](rows *sql.Rows, scan func(scanner) (*T, error)) ([]T, error) {
	var results []T
	for rows.Next() {
		item, err := scan(rows)
		if err != nil {
			return results, err
		}
		results = append(results, *item)
	}
	return results, rows.Err()
}

// ────────────────────────────────────────────────────────────────
// B.2 Core Operations (Open/Close)
// ────────────────────────────────────────────────────────────────

// Open opens the language database from a directory path.
// Opens read-only — language database is reference data.
func Open(dbDir string) (*DB, error) {
	dbPath := filepath.Join(dbDir, DatabaseFile)

	conn, err := sql.Open("sqlite", dbPath+"?mode=ro")
	if err != nil {
		return nil, fmt.Errorf("language.Open: %w", err)
	}

	if err := conn.Ping(); err != nil {
		conn.Close()
		return nil, fmt.Errorf("language.Open: ping failed: %w", err)
	}

	return &DB{db: conn}, nil
}

// OpenFromRoot opens the language database using module root path.
// Convenience: OpenFromRoot("/path/to/b-word") resolves to
// "/path/to/b-word/data/language/database/language.db".
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

// AllSpecs returns every spec in the database.
func (db *DB) AllSpecs() ([]Spec, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllSpecs)
	if err != nil {
		return nil, fmt.Errorf("AllSpecs: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanSpec)
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
// B.4 Dependency Queries
// ────────────────────────────────────────────────────────────────

// DependenciesOf returns what a spec depends on.
func (db *DB) DependenciesOf(specKey string) ([]Dependency, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qDepsOf, specKey)
	if err != nil {
		return nil, fmt.Errorf("DependenciesOf(%s): %w", specKey, err)
	}
	defer rows.Close()
	return collectRows(rows, scanDep)
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
	return collectRows(rows, scanDep)
}

// ────────────────────────────────────────────────────────────────
// B.5 Keyword Queries
// ────────────────────────────────────────────────────────────────

// AllKeywords returns all keywords.
func (db *DB) AllKeywords() ([]Keyword, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllKeywords)
	if err != nil {
		return nil, fmt.Errorf("AllKeywords: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanKeyword)
}

// KeywordByName returns a keyword by its name.
func (db *DB) KeywordByName(keyword string) (*Keyword, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanKeyword(db.db.QueryRow(qKeywordByName, keyword))
}

// KeywordsByCategory returns keywords in a category.
func (db *DB) KeywordsByCategory(category string) ([]Keyword, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qKeywordsByCategory, category)
	if err != nil {
		return nil, fmt.Errorf("KeywordsByCategory(%s): %w", category, err)
	}
	defer rows.Close()
	return collectRows(rows, scanKeyword)
}

// ────────────────────────────────────────────────────────────────
// B.6 Syntax Pattern Queries
// ────────────────────────────────────────────────────────────────

// AllSyntaxPatterns returns all syntax patterns.
func (db *DB) AllSyntaxPatterns() ([]SyntaxPattern, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllSyntax)
	if err != nil {
		return nil, fmt.Errorf("AllSyntaxPatterns: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanSyntax)
}

// SyntaxPatternsByCategory returns patterns in a category.
func (db *DB) SyntaxPatternsByCategory(category string) ([]SyntaxPattern, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qSyntaxByCategory, category)
	if err != nil {
		return nil, fmt.Errorf("SyntaxPatternsByCategory(%s): %w", category, err)
	}
	defer rows.Close()
	return collectRows(rows, scanSyntax)
}

// SyntaxPatternByName returns a single pattern by name and category.
func (db *DB) SyntaxPatternByName(name, category string) (*SyntaxPattern, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanSyntax(db.db.QueryRow(qSyntaxByName, name, category))
}

// ────────────────────────────────────────────────────────────────
// B.7 Token Type & Precedence Queries
// ────────────────────────────────────────────────────────────────

// AllTokenTypes returns all token types.
func (db *DB) AllTokenTypes() ([]TokenType, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllTokenTypes)
	if err != nil {
		return nil, fmt.Errorf("AllTokenTypes: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanTokenType)
}

// TokenTypeByName returns a single token type.
func (db *DB) TokenTypeByName(name string) (*TokenType, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanTokenType(db.db.QueryRow(qTokenTypeByName, name))
}

// AllPrecedenceGroups returns all precedence groups ordered by level.
func (db *DB) AllPrecedenceGroups() ([]PrecedenceGroup, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllPrecedence)
	if err != nil {
		return nil, fmt.Errorf("AllPrecedenceGroups: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanPrecedence)
}

// PrecedenceGroupByName returns a single precedence group.
func (db *DB) PrecedenceGroupByName(name string) (*PrecedenceGroup, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanPrecedence(db.db.QueryRow(qPrecedenceByName, name))
}

// ────────────────────────────────────────────────────────────────
// B.8 Operator Queries
// ────────────────────────────────────────────────────────────────

// AllOperators returns all symbol operators.
func (db *DB) AllOperators() ([]Operator, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllOperators)
	if err != nil {
		return nil, fmt.Errorf("AllOperators: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanOperator)
}

// OperatorsByCategory returns operators in a category.
func (db *DB) OperatorsByCategory(category string) ([]Operator, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qOperatorsByCategory, category)
	if err != nil {
		return nil, fmt.Errorf("OperatorsByCategory(%s): %w", category, err)
	}
	defer rows.Close()
	return collectRows(rows, scanOperator)
}

// OperatorByName returns a single operator by name and category.
func (db *DB) OperatorByName(name, category string) (*Operator, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanOperator(db.db.QueryRow(qOperatorByName, name, category))
}

// ────────────────────────────────────────────────────────────────
// B.9 Keyword Group Queries
// ────────────────────────────────────────────────────────────────

// AllKeywordGroups returns all keyword groups.
func (db *DB) AllKeywordGroups() ([]KeywordGroup, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllKeywordGroups)
	if err != nil {
		return nil, fmt.Errorf("AllKeywordGroups: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanKeywordGroup)
}

// KeywordGroupByPath returns a keyword group by its path (e.g., "keywords.control.proceed").
func (db *DB) KeywordGroupByPath(path string) (*KeywordGroup, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanKeywordGroup(db.db.QueryRow(qKeywordGroupByPath, path))
}

// ────────────────────────────────────────────────────────────────
// B.10 Phrasal Operator & Disambiguation Queries
// ────────────────────────────────────────────────────────────────

// AllPhrasalOperators returns all phrasal (multi-word) operators.
func (db *DB) AllPhrasalOperators() ([]PhrasalOperator, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllPhrasal)
	if err != nil {
		return nil, fmt.Errorf("AllPhrasalOperators: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanPhrasal)
}

// PhrasalOperatorByName returns a single phrasal operator.
func (db *DB) PhrasalOperatorByName(name string) (*PhrasalOperator, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanPhrasal(db.db.QueryRow(qPhrasalByName, name))
}

// AllDisambiguationRules returns all disambiguation rules.
func (db *DB) AllDisambiguationRules() ([]DisambiguationRule, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllDisambig)
	if err != nil {
		return nil, fmt.Errorf("AllDisambiguationRules: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanDisambig)
}

// DisambiguationRuleByKeyword returns a disambiguation rule.
func (db *DB) DisambiguationRuleByKeyword(keyword string) (*DisambiguationRule, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanDisambig(db.db.QueryRow(qDisambigByKeyword, keyword))
}

// ────────────────────────────────────────────────────────────────
// B.11 Extension Queries
// ────────────────────────────────────────────────────────────────

// AllExtensions returns all file extension definitions.
func (db *DB) AllExtensions() ([]Extension, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qAllExtensions)
	if err != nil {
		return nil, fmt.Errorf("AllExtensions: %w", err)
	}
	defer rows.Close()
	return collectRows(rows, scanExtension)
}

// ExtensionByExt returns a single extension by its extension string (e.g., ".omni").
func (db *DB) ExtensionByExt(ext string) (*Extension, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	return scanExtension(db.db.QueryRow(qExtensionByExt, ext))
}

// ExtensionsByCategory returns extensions in a category ("omni", "biblical", "cognitive").
func (db *DB) ExtensionsByCategory(category string) ([]Extension, error) {
	db.mu.RLock()
	defer db.mu.RUnlock()
	rows, err := db.db.Query(qExtensionsByCategory, category)
	if err != nil {
		return nil, fmt.Errorf("ExtensionsByCategory(%s): %w", category, err)
	}
	defer rows.Close()
	return collectRows(rows, scanExtension)
}

// ============================================================================
// CLOSING
// ============================================================================
//
// This package provides L1 language access to the database that materializes
// all TOML specifications into queryable records. The Triangle is complete:
//
//   Configuration (TOML specs) -> Data (SQLite) -> Code (this package)
//
// Every keyword, operator, syntax pattern, and extension is discoverable by
// query rather than hardcoded. The system knows its own language.
//
// "In the beginning was the Word, and the Word was with God,
//  and the Word was God." — John 1:1
//
// Dependencies: modernc.org/sqlite (pure Go, no CGO)
// Data source: b-word/data/language/database/language.db
//
// Safe to add: New query methods, search functions, aggregate queries
// Modify with care: Type struct fields, database schema assumptions
// Never modify: Read-only access pattern, table names
