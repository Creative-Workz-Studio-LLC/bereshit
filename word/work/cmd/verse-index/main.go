// ═══════════════════════════════════════════════════════════════════════════
// verse-index: Scripture → Ordinal Index Generator
// Key: B-word-work-cmd-verse-index
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: word/scripture/, word/core/bible/)
//
// derives_from: bereshit/word/seed/code/go/executable.go
//
// ═══════════════════════════════════════════════════════════════════════════

// Package main generates the Bible verse ordinal index from Scripture files.
//
// verse-index - CPI-SI Bereshit Build Tool
//
// ────────────────────────────────────────────────────────────────
// CORE IDENTITY
// ────────────────────────────────────────────────────────────────
//
// # Biblical Foundation
//
// Scripture: "So shall my word be that goeth forth out of my mouth: it shall
// not return unto me void, but it shall accomplish that which I please" (Isaiah 55:11)
//
// Principle: Every verse has an address. The Word of God is ordered, numbered,
// addressable. This ordering enables precise reference and ternary encoding.
//
// Anchor: "The words of the LORD are pure words: as silver tried in a furnace
// of earth, purified seven times." (Psalm 12:6)
//
// # CPI-SI Identity
//
// Component Type: Baton - runs once, produces output, exits
//
// Role: Build the coordinate map. Walk Scripture files, assign ordinals,
// generate the reversible encoding cache for MillenniumOS.
//
// Paradigm: Scripture as addressable data - every verse maps to bytes (0-255)
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise, Nova Dawn
//   - Implementation: Nova Dawn
//   - Created: 2025-12-22
//   - Version: 0.1.0
//
// # Purpose & Function
//
// Purpose: Generate 21 output files (3 sets × 7 files each):
//
//   SET 1 - KJV (Encoding):
//     1. kjv-ordinal-index.json   - Full JSON structure
//     2. kjv-ordinal-index.csv    - Flat CSV lookup
//     3. kjv-ordinal-index.bin    - Binary compact
//     4. kjv-ordinal-index.trit   - Ternary trit5
//     5. kjv-cube-index.json      - 9×27 cube structure
//     6. kjv-ascii-key.json       - Character encryption table
//     7. kjv-scripture-key.json   - Verse metadata + Hebrew (OT)
//
//   SET 2 - WEB (Decoding):
//     1. web-ordinal-index.json   - Full JSON structure
//     2. web-ordinal-index.csv    - Flat CSV lookup
//     3. web-ordinal-index.bin    - Binary compact
//     4. web-ordinal-index.trit   - Ternary trit5
//     5. web-cube-index.json      - 9×27 cube structure
//     6. web-ascii-key.json       - Character encryption table
//     7. web-scripture-key.json   - Verse metadata + Hebrew (OT)
//
//   SET 3 - PARALLEL (KJV↔WEB Side-by-Side):
//     1. parallel-ordinal-index.json  - Both texts JSON
//     2. parallel-ordinal-index.csv   - Both texts CSV
//     3. parallel-ordinal-index.bin   - Both texts binary
//     4. parallel-ordinal-index.trit  - Both texts ternary
//     5. parallel-cube-index.json     - Both texts cube
//     6. parallel-ascii-key.json      - Combined character table
//     7. parallel-scripture-key.json  - Full metadata + Hebrew + Greek
//
// Core Design: Walk word/scripture/{KJV,WEB}/ in canonical order, assign
// ordinals, generate reversible encoding for both translations plus parallel.
//
// Key Features:
//
//   - 31,102 KJV verses → ordinals 1-31102 → trit5 pairs (0-242 range)
//   - 31,115 WEB verses → ordinals 1-31115 (includes 13 variants)
//   - 243 = 9 anchors × 27 cube positions (natural ternary decomposition)
//   - ASCII key links characters to word/core/bible/encryption.toml
//   - Scripture key includes Hebrew (OT) / Greek (NT) placeholders
//   - Reversible: encode(decode(x)) = x for all verses
//
// Philosophy: The cache grounds generic ternary math in specific Scripture.
// This is the coordinate map for MillenniumOS - Scripture becomes executable.
// The 3 sets provide: Encoding (KJV) + Decoding (WEB) + Holistic (Parallel).
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: fmt, os, path/filepath, encoding/csv, sort, strconv
//   - External: github.com/BurntSushi/toml
//   - Internal: word/core/bible/addressing.toml (canonical book order)
//   - Data: word/scripture/KJV/ (verse files)
//
// What Uses This:
//
//   - MillenniumOS: Uses ordinal index for verse addressing
//   - claude-global/pkg/coordinates: Loads index for precise lookups
//   - Makefile: `make index` target generates before build
//
// # Usage
//
// Command Line:
//
//	verse-index                    Uses defaults (BERESHIT_ROOT)
//	verse-index -verify            Verify existing index matches Scripture
//
// Exit Codes:
//
//	0  - Success, index generated/verified
//	1  - Error (Scripture missing, count mismatch, write failure)
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// # Blocking Status
//
// Blocking: Index must be accurate for MillenniumOS to function.
// Verification mode catches drift between index and Scripture.
//
// [OMIT: Health Scoring - Baton tool, no persistent state to track]
//
package main

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strconv"
	"strings"

	"creativeworkzstudio.com/bereshit/word/work/pkg/config"
	"github.com/BurntSushi/toml"
)

// ════════════════════════════════════════════════════════════════
// CONSTANTS
// ════════════════════════════════════════════════════════════════
//
// Organization: CORE only - math and paths
//
// Filenames are CONFIG-DRIVEN via GetFilename(SetID, FileType)
// See CONFIG TYPES section for SetID, FileType, and generation.
//
// Example: GetFilename(SetKJV, FileOrdinalJSON) → "kjv-ordinal-index.json"
//
// ════════════════════════════════════════════════════════════════

const (
	// ────────────────────────────────────────────────────────────
	// CORE: Ternary Encoding
	// ────────────────────────────────────────────────────────────
	Trit5Base   = 243   // 3^5 = 243 states per trit5 byte
	TotalKJV    = 31102 // KJV verse count
	TotalWEB    = 31115 // WEB verse count (KJV + 13 variants)
	WEBVariants = 13    // Spare values 243-255
	MaxPages    = 128   // ceil(31102/243) = 128 pages

	// ────────────────────────────────────────────────────────────
	// CORE: Cube Decomposition (243 = 9 × 27)
	// ────────────────────────────────────────────────────────────
	NumAnchors = 9  // 3^2 = 9 anchor points
	NumCubePos = 27 // 3^3 = 27 cube positions

	// ────────────────────────────────────────────────────────────
	// CORE: Paths (relative to BERESHIT_ROOT)
	// ────────────────────────────────────────────────────────────
	ScripturePath    = "word/scripture/KJV"
	WEBScripturePath = "word/scripture/WEB"
	OutputPath       = "word/scripture"
	EncryptionPath   = "word/core/bible/encryption.toml"
)

// ════════════════════════════════════════════════════════════════
// VARIABLES
// ════════════════════════════════════════════════════════════════
//
// Organization: By FILE TYPE (matches Constants and Types)
//
//   CORE:      Book ordering for Scripture traversal
//   FILES 1-4: (no runtime variables - use constants)
//   FILE 5:    (no runtime variables - derived from index)
//   FILE 6:    (no runtime variables - loaded from encryption.toml)
//   FILE 7:    WEB variants for decoding
//
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// CORE: Book Ordering
// ────────────────────────────────────────────────────────────────

// canonicalBookOrder defines the 66 books in canonical order.
// Maps directory name to book ordinal (1-66).
// Directory names use underscores (e.g., "Song_of_Solomon", "1_Samuel").
// Used by: walkScripture() for all 3 sets.
var canonicalBookOrder = map[string]int{
	// Old Testament (1-39) — Hebrew (OT)
	"Genesis": 1, "Exodus": 2, "Leviticus": 3, "Numbers": 4, "Deuteronomy": 5,
	"Joshua": 6, "Judges": 7, "Ruth": 8, "1_Samuel": 9, "2_Samuel": 10,
	"1_Kings": 11, "2_Kings": 12, "1_Chronicles": 13, "2_Chronicles": 14,
	"Ezra": 15, "Nehemiah": 16, "Esther": 17, "Job": 18, "Psalms": 19,
	"Proverbs": 20, "Ecclesiastes": 21, "Song_of_Solomon": 22,
	"Isaiah": 23, "Jeremiah": 24, "Lamentations": 25, "Ezekiel": 26, "Daniel": 27,
	"Hosea": 28, "Joel": 29, "Amos": 30, "Obadiah": 31, "Jonah": 32,
	"Micah": 33, "Nahum": 34, "Habakkuk": 35, "Zephaniah": 36,
	"Haggai": 37, "Zechariah": 38, "Malachi": 39,
	// New Testament (40-66) — Greek (NT)
	"Matthew": 40, "Mark": 41, "Luke": 42, "John": 43, "Acts": 44,
	"Romans": 45, "1_Corinthians": 46, "2_Corinthians": 47,
	"Galatians": 48, "Ephesians": 49, "Philippians": 50, "Colossians": 51,
	"1_Thessalonians": 52, "2_Thessalonians": 53,
	"1_Timothy": 54, "2_Timothy": 55, "Titus": 56, "Philemon": 57,
	"Hebrews": 58, "James": 59, "1_Peter": 60, "2_Peter": 61,
	"1_John": 62, "2_John": 63, "3_John": 64, "Jude": 65, "Revelation": 66,
}

// ────────────────────────────────────────────────────────────────
// FILE 7: WEB Variants (decoding extension)
// ────────────────────────────────────────────────────────────────

// webVariants defines the 13 WEB-only verses mapped to trites 243-255.
// These complete the 256-value byte space for decoding.
// Used by: All index types (VerseIndex, ParallelIndex, Scripture Key).
var webVariants = []WEBVariant{
	{Trite: 243, Book: "1_Corinthians", Chapter: 16, Verse: 27},
	{Trite: 244, Book: "1_Peter", Chapter: 5, Verse: 20},
	{Trite: 245, Book: "1_Timothy", Chapter: 3, Verse: 18},
	{Trite: 246, Book: "2_Kings", Chapter: 22, Verse: 53},
	{Trite: 247, Book: "Amos", Chapter: 3, Verse: 21},
	{Trite: 248, Book: "Colossians", Chapter: 4, Verse: 23},
	{Trite: 249, Book: "Ezekiel", Chapter: 5, Verse: 22},
	{Trite: 250, Book: "Jonah", Chapter: 1, Verse: 21},
	{Trite: 251, Book: "Nehemiah", Chapter: 10, Verse: 44},
	{Trite: 252, Book: "Numbers", Chapter: 27, Verse: 34},
	{Trite: 253, Book: "Psalms", Chapter: 42, Verse: 17},
	{Trite: 254, Book: "Revelation", Chapter: 1, Verse: 25},
	{Trite: 255, Book: "Zechariah", Chapter: 2, Verse: 23},
}

// ════════════════════════════════════════════════════════════════
// TYPES
// ════════════════════════════════════════════════════════════════
//
// Organization: By FILE TYPE (each serves all 3 sets)
//
//   FILES 1-4: Ordinal Index (JSON, CSV, Binary, Ternary)
//   FILE 5:    Cube Index (9×27 structure)
//   FILE 6:    ASCII Key (character encryption)
//   FILE 7:    Scripture Key (verse metadata + Hebrew/Greek)
//
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// FILES 1-4: Ordinal Index Types
// ────────────────────────────────────────────────────────────────
// Used by: SET 1 (kjv-ordinal-index.*), SET 2 (web-ordinal-index.*)
// SET 3 uses ParallelEntry instead for both texts

// VerseEntry is the fundamental unit - one verse with coordinates.
// Used by SET 1 & SET 2 (single translation).
type VerseEntry struct {
	Ordinal int    `json:"ordinal"` // 1-31102 (sequential position)
	Book    string `json:"book"`    // Directory name (e.g., "Genesis")
	Chapter int    `json:"chapter"` // Chapter number
	Verse   int    `json:"verse"`   // Verse number within chapter
	Low     int    `json:"low"`     // Trit5 low byte (0-242) = ordinal % 243
	High    int    `json:"high"`    // Trit5 high byte (0-127) = ordinal / 243
	Anchor  int    `json:"anchor"`  // Anchor point (0-8) = low / 27
	Cube    int    `json:"cube"`    // Cube position (0-26) = low % 27
	Text    string `json:"text"`    // Verse content (KJV or WEB depending on set)
}

// ParallelEntry is the fundamental unit for SET 3 - both texts side-by-side.
type ParallelEntry struct {
	Ordinal   int    `json:"ordinal"`   // 1-31102 (sequential position)
	Book      string `json:"book"`      // Directory name
	Chapter   int    `json:"chapter"`   // Chapter number
	Verse     int    `json:"verse"`     // Verse number
	Low       int    `json:"low"`       // Trit5 low byte
	High      int    `json:"high"`      // Trit5 high byte
	Anchor    int    `json:"anchor"`    // Anchor point (0-8)
	Cube      int    `json:"cube"`      // Cube position (0-26)
	KJV       string `json:"kjv"`       // KJV text (encoding)
	WEB       string `json:"web"`       // WEB text (decoding)
	Testament string `json:"testament"` // "OT" or "NT"
}

// WEBVariant represents one of the 13 WEB-only verses (trites 243-255).
type WEBVariant struct {
	Trite   int    `json:"trite"`   // 243-255
	Book    string `json:"book"`    // Directory name
	Chapter int    `json:"chapter"` // Chapter number
	Verse   int    `json:"verse"`   // Verse number
	Text    string `json:"text"`    // Verse content
}

// VerseGroup is a position in the 243-value space (for SET 1 & 2).
type VerseGroup struct {
	Position int          `json:"position"` // 0-242 (low byte value)
	Verses   []VerseEntry `json:"verses"`   // Verses at this position
}

// ParallelGroup is a position in the 243-value space (for SET 3).
type ParallelGroup struct {
	Position int             `json:"position"` // 0-242 (low byte value)
	Verses   []ParallelEntry `json:"verses"`   // Verses at this position
}

// VerseIndex is the complete index for SET 1 & 2.
type VerseIndex struct {
	Groups   [Trit5Base]VerseGroup `json:"groups"`   // 243 position groups
	Variants []WEBVariant          `json:"variants"` // 13 WEB-only verses
	Stats    IndexStats            `json:"stats"`    // Statistics
}

// ParallelIndex is the complete index for SET 3.
type ParallelIndex struct {
	Groups   [Trit5Base]ParallelGroup `json:"groups"`   // 243 position groups
	Variants []WEBVariant             `json:"variants"` // 13 WEB-only verses
	Stats    IndexStats               `json:"stats"`    // Statistics
}

// IndexStats holds statistics (shared by all index types).
type IndexStats struct {
	TotalVerses   int `json:"total_verses"`   // 31102 (KJV) or 31115 (WEB)
	TotalVariants int `json:"total_variants"` // 13
	TotalEntries  int `json:"total_entries"`  // Total including variants
	MaxPerGroup   int `json:"max_per_group"`  // Largest group (~128)
	MinPerGroup   int `json:"min_per_group"`  // Smallest group
}

// ────────────────────────────────────────────────────────────────
// FILE 5: Cube Index Types (9×27 structure)
// ────────────────────────────────────────────────────────────────
// Used by: All 3 sets - enables O(1) lookup via [anchor][cube][high]

// CubeIndex for SET 1 & 2 (single translation).
type CubeIndex struct {
	Cube     [NumAnchors][NumCubePos][]VerseEntry `json:"cube"`
	Variants []WEBVariant                          `json:"variants"`
	Stats    CubeStats                             `json:"stats"`
}

// ParallelCubeIndex for SET 3 (both translations).
type ParallelCubeIndex struct {
	Cube     [NumAnchors][NumCubePos][]ParallelEntry `json:"cube"`
	Variants []WEBVariant                             `json:"variants"`
	Stats    CubeStats                                `json:"stats"`
}

// CubeStats holds cube statistics (shared).
type CubeStats struct {
	TotalVerses int `json:"total_verses"`
	Anchors     int `json:"anchors"`      // 9
	CubePos     int `json:"cube_pos"`     // 27
	MaxPerCell  int `json:"max_per_cell"` // Largest cell size
}

// ────────────────────────────────────────────────────────────────
// FILE 6: ASCII Key Types (character encryption)
// ────────────────────────────────────────────────────────────────
// Used by: All 3 sets - links characters to encryption.toml

// ASCIIKeyEntry maps one ASCII value to cube coordinates.
type ASCIIKeyEntry struct {
	ASCII      byte   `json:"ascii"`       // 0-255
	Char       string `json:"char"`        // Character representation
	Printable  bool   `json:"printable"`   // Is printable?
	TriteValue int    `json:"trite_value"` // Mapped trite (0-242)
	CubeX      int    `json:"cube_x"`      // X coordinate (-1, 0, +1)
	CubeY      int    `json:"cube_y"`      // Y coordinate (-1, 0, +1)
	CubeZ      int    `json:"cube_z"`      // Z coordinate (-1, 0, +1)
	Anchor     int    `json:"anchor"`      // Anchor (0-8)
	CubePos    int    `json:"cube_pos"`    // Cube position (0-26)
}

// ASCIIKey is the complete character encryption table.
type ASCIIKey struct {
	Entries          []ASCIIKeyEntry `json:"entries"`           // All 256 values
	HebrewRef        string          `json:"hebrew_ref"`        // Reference path
	GreekRef         string          `json:"greek_ref"`         // Reference path
	EncryptionSource string          `json:"encryption_source"` // encryption.toml path
}

// ────────────────────────────────────────────────────────────────
// FILE 7: Scripture Key Types (verse metadata + Hebrew/Greek)
// ────────────────────────────────────────────────────────────────
// Used by: All 3 sets - the DECRYPTION KEY that ties coordinates to meaning

// ScriptureKeyEntry for SET 1 & 2 (single translation).
type ScriptureKeyEntry struct {
	Ordinal   int      `json:"ordinal"`            // 1-31102
	Book      string   `json:"book"`               // Book name
	Chapter   int      `json:"chapter"`            // Chapter number
	Verse     int      `json:"verse"`              // Verse number
	Text      string   `json:"text"`               // Translation text (KJV or WEB)
	Testament string   `json:"testament"`          // "OT" or "NT"
	Hebrew    *string  `json:"hebrew,omitempty"`   // Hebrew (OT only, future)
	Greek     *string  `json:"greek,omitempty"`    // Greek (NT only, future)
	Strongs   []string `json:"strongs,omitempty"`  // Strong's numbers (future)
	Low       int      `json:"low"`                // Trit5 low byte
	High      int      `json:"high"`               // Trit5 high byte
	Anchor    int      `json:"anchor"`             // Anchor (0-8)
	Cube      int      `json:"cube"`               // Cube position (0-26)
}

// ParallelScriptureKeyEntry for SET 3 (both translations).
type ParallelScriptureKeyEntry struct {
	Ordinal   int      `json:"ordinal"`            // 1-31102
	Book      string   `json:"book"`               // Book name
	Chapter   int      `json:"chapter"`            // Chapter number
	Verse     int      `json:"verse"`              // Verse number
	KJV       string   `json:"kjv"`                // KJV text (encoding)
	WEB       string   `json:"web"`                // WEB text (decoding)
	Testament string   `json:"testament"`          // "OT" or "NT"
	Hebrew    *string  `json:"hebrew,omitempty"`   // Hebrew (OT only, future)
	Greek     *string  `json:"greek,omitempty"`    // Greek (NT only, future)
	Strongs   []string `json:"strongs,omitempty"`  // Strong's numbers (future)
	Low       int      `json:"low"`                // Trit5 low byte
	High      int      `json:"high"`               // Trit5 high byte
	Anchor    int      `json:"anchor"`             // Anchor (0-8)
	Cube      int      `json:"cube"`               // Cube position (0-26)
}

// ScriptureKey for SET 1 & 2.
type ScriptureKey struct {
	Entries []ScriptureKeyEntry `json:"entries"` // All verses
	Stats   ScriptureKeyStats   `json:"stats"`   // Statistics
}

// ParallelScriptureKey for SET 3.
type ParallelScriptureKey struct {
	Entries []ParallelScriptureKeyEntry `json:"entries"` // All verses
	Stats   ScriptureKeyStats           `json:"stats"`   // Statistics
}

// ScriptureKeyStats holds statistics (shared).
type ScriptureKeyStats struct {
	TotalEntries int `json:"total_entries"` // 31102
	OTEntries    int `json:"ot_entries"`    // Genesis-Malachi
	NTEntries    int `json:"nt_entries"`    // Matthew-Revelation
	WithHebrew   int `json:"with_hebrew"`   // With Hebrew (future)
	WithGreek    int `json:"with_greek"`    // With Greek (future)
}

// ════════════════════════════════════════════════════════════════
// CONFIG TYPES (Generation Pipeline)
// ════════════════════════════════════════════════════════════════
//
// Config-driven architecture: Instead of 21 hardcoded constants and
// duplicated write functions, we define:
//
//   SetID     → Identifies which set (KJV, WEB, Parallel)
//   FileType  → Identifies which file within a set (7 types)
//   SetConfig → Configuration for generating one set
//
// This enables: generateSet(config) → all 7 files for that set
//
// ════════════════════════════════════════════════════════════════

// SetID identifies which set we're generating.
type SetID string

const (
	SetKJV      SetID = "kjv"      // SET 1: Encoding (31,102 verses)
	SetWEB      SetID = "web"      // SET 2: Decoding (31,115 verses)
	SetParallel SetID = "parallel" // SET 3: Side-by-side comparison
)

// FileType identifies which file type within a set.
// Each set produces all 7 file types.
type FileType string

const (
	FileOrdinalJSON  FileType = "ordinal-index.json" // FILE 1: Full JSON
	FileOrdinalCSV   FileType = "ordinal-index.csv"  // FILE 2: Flat CSV
	FileOrdinalBin   FileType = "ordinal-index.bin"  // FILE 3: Binary compact
	FileOrdinalTrit  FileType = "ordinal-index.trit" // FILE 4: Ternary trit5
	FileCubeJSON     FileType = "cube-index.json"    // FILE 5: 9×27 structure
	FileASCIIKey     FileType = "ascii-key.json"     // FILE 6: Character encryption
	FileScriptureKey FileType = "scripture-key.json" // FILE 7: Verse metadata
)

// AllFileTypes lists all 7 file types in generation order.
var AllFileTypes = []FileType{
	FileOrdinalJSON,
	FileOrdinalCSV,
	FileOrdinalBin,
	FileOrdinalTrit,
	FileCubeJSON,
	FileASCIIKey,
	FileScriptureKey,
}

// SetConfig defines a set's properties for generation.
type SetConfig struct {
	ID            SetID  // SetKJV, SetWEB, or SetParallel
	Name          string // Human-readable name
	ScripturePath string // Relative path to scripture files
	IsParallel    bool   // True for SET 3 (requires both KJV and WEB)
}

// AllSets defines all 3 sets in generation order.
var AllSets = []SetConfig{
	{ID: SetKJV, Name: "KJV", ScripturePath: ScripturePath, IsParallel: false},
	{ID: SetWEB, Name: "WEB", ScripturePath: WEBScripturePath, IsParallel: false},
	{ID: SetParallel, Name: "Parallel", ScripturePath: "", IsParallel: true},
}

// GetFilename generates a filename from set ID and file type.
// Example: GetFilename(SetKJV, FileOrdinalJSON) → "kjv-ordinal-index.json"
func GetFilename(set SetID, fileType FileType) string {
	return fmt.Sprintf("%s-%s", set, fileType)
}

// GetFilepath generates a full path from set ID, file type, and output dir.
func GetFilepath(set SetID, fileType FileType, outputDir string) string {
	return filepath.Join(outputDir, GetFilename(set, fileType))
}

// ════════════════════════════════════════════════════════════════
// TYPE METHODS
// ════════════════════════════════════════════════════════════════
//
// Organization: By FILE TYPE (matches Constants, Variables, Types)
//
//   CORE:      Encode/Decode (trit5 conversion)
//   FILES 1-4: VerseIndex lookup methods
//   FILE 5:    CubeIndex lookup and build methods
//   FILE 6:    (reserved - ASCIIKey methods)
//   FILE 7:    (reserved - ScriptureKey methods)
//
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// CORE: Trit5 Encoding/Decoding
// ────────────────────────────────────────────────────────────────

// Encode converts ordinal to (low, high) trit5 pair.
// Used by: All files for coordinate calculation.
func Encode(ordinal int) (low, high int) {
	// Ordinals are 1-indexed, encoding is 0-indexed
	idx := ordinal - 1
	low = idx % Trit5Base
	high = idx / Trit5Base
	return low, high
}

// Decode converts (low, high) trit5 pair to ordinal.
// Used by: All files for reverse lookup.
func Decode(low, high int) int {
	return (high * Trit5Base) + low + 1
}

// ────────────────────────────────────────────────────────────────
// FILES 1-4: VerseIndex Methods (Ordinal Index)
// ────────────────────────────────────────────────────────────────

// Lookup returns the verse at the given (low, high) position.
// O(1) direct array access.
// Used by: SET 1 (KJV) and SET 2 (WEB) ordinal index.
func (idx *VerseIndex) Lookup(low, high int) (*VerseEntry, bool) {
	if low < 0 || low >= Trit5Base || high < 0 {
		return nil, false
	}
	group := &idx.Groups[low]
	if high >= len(group.Verses) {
		return nil, false
	}
	return &group.Verses[high], true
}

// LookupVariant returns the WEB variant at the given trite value.
// O(1) direct array access for values 243-255.
// Used by: SET 2 (WEB) for the 13 extra verses.
func (idx *VerseIndex) LookupVariant(trite int) (*WEBVariant, bool) {
	if trite < Trit5Base || trite > 255 {
		return nil, false
	}
	variantIdx := trite - Trit5Base
	if variantIdx >= len(idx.Variants) {
		return nil, false
	}
	return &idx.Variants[variantIdx], true
}

// ────────────────────────────────────────────────────────────────
// FILE 5: CubeIndex Methods (9×27 structure)
// ────────────────────────────────────────────────────────────────

// LookupByCube returns the verse at the given (anchor, cube, high) position.
// True O(1) lookup using state machine coordinates.
// Used by: claude-global state machine for direct verse access.
func (idx *CubeIndex) LookupByCube(anchor, cube, high int) (*VerseEntry, bool) {
	if anchor < 0 || anchor >= NumAnchors || cube < 0 || cube >= NumCubePos || high < 0 {
		return nil, false
	}
	cell := idx.Cube[anchor][cube]
	if high >= len(cell) {
		return nil, false
	}
	return &cell[high], true
}

// ────────────────────────────────────────────────────────────────
// FILE 5-7 Type Methods: See CORE OPERATIONS in BODY
// ────────────────────────────────────────────────────────────────
// FILE 5: buildCubeIndex(), buildParallelCubeIndex()
// FILE 6: buildASCIIKey()
// FILE 7: buildScriptureKey(), buildParallelScriptureKey()

// ────────────────────────────────────────────────────────────────
// Package-Level State
// ────────────────────────────────────────────────────────────────

// [Reserved: Baton tool - no Rails infrastructure needed]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Internal Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   main() (Entry Point)
//   └── buildIndex() → uses walkScripture(), groupByPosition()
//       └── writeIndex() → output JSON and CSV
//
//   Core Operations (Middle Rungs)
//   ├── walkScripture() → traverse KJV directory, count verses
//   ├── groupByPosition() → group verses by low byte (mod 243)
//   └── writeIndex() → output JSON/CSV files
//
// Baton Flow:
//
//   Entry → main()
//     ↓
//   walkScripture() traverses KJV in canonical book order
//     ↓
//   Assigns ordinals (1-31102) as verses are discovered
//     ↓
//   groupByPosition() groups by low byte (mod 243) into 243 groups
//     ↓
//   writeIndex() outputs JSON (full structure) + CSV (flat lookup)
//     ↓
//   Exit → success with stats, or error
//
// APUs: 5 functions (3 core ops, 1 helper, 1 main)

// ════════════════════════════════════════════════════════════════
// HELPERS/UTILITIES
// ════════════════════════════════════════════════════════════════
//
// Organization: By FILE TYPE (matches CONFIG TYPES)
//
//   CORE:       Parsing, sorting, config loading (used by all)
//   FILES 1-3:  (no helpers - direct I/O)
//   FILE 4:     toTrit5() - balanced ternary conversion
//   FILE 5:     (no helpers - uses buildCubeIndex)
//   FILE 6:     loadEncryptionTOML() - character encryption data
//   FILE 7:     isOldTestament(), getTestament() - testament detection
//   GENERIC IO: writeJSONFile(), writeCSVFile(), writeBinaryFile(), etc.
//
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// CORE Helpers: Parsing and Config Loading
// ────────────────────────────────────────────────────────────────

// parseChapterNumber extracts chapter number from directory name.
// Format: "Chapter_N" → N
// Used by: walkScripture() for all sets.
func parseChapterNumber(name string) (int, error) {
	if !strings.HasPrefix(name, "Chapter_") {
		return 0, fmt.Errorf("invalid chapter format: %s", name)
	}
	numStr := strings.TrimPrefix(name, "Chapter_")
	return strconv.Atoi(numStr)
}

// parseVerseNumber extracts verse number from filename.
// Format: "Verse_N.txt" → N
// Used by: walkScripture() for all sets.
func parseVerseNumber(name string) (int, error) {
	if !strings.HasPrefix(name, "Verse_") || !strings.HasSuffix(name, ".txt") {
		return 0, fmt.Errorf("invalid verse format: %s", name)
	}
	numStr := strings.TrimPrefix(name, "Verse_")
	numStr = strings.TrimSuffix(numStr, ".txt")
	return strconv.Atoi(numStr)
}

// sortedBookDirs returns book directories in canonical order (1-66).
// Used by: walkScripture() for all sets.
func sortedBookDirs(bookDirs []string) []string {
	sort.Slice(bookDirs, func(i, j int) bool {
		orderI := canonicalBookOrder[bookDirs[i]]
		orderJ := canonicalBookOrder[bookDirs[j]]
		return orderI < orderJ
	})
	return bookDirs
}

// loadCanonicalBookOrderFromConfig loads book order from addressing.toml.
// Maps TOML name → directory name (spaces become underscores).
// Returns the loaded map, or nil if config loading fails.
// Used by: main() for dynamic book order loading.
func loadCanonicalBookOrderFromConfig(bereshitRoot string) map[string]int {
	config.SetRoot(bereshitRoot)
	files, err := config.LoadBible()
	if err != nil {
		return nil
	}

	// Find addressing.toml
	var addressingData map[string]any
	for _, f := range files {
		if f.Name == "addressing.toml" {
			addressingData = f.Data
			break
		}
	}
	if addressingData == nil {
		return nil
	}

	// Get books section
	booksRaw, ok := addressingData["books"]
	if !ok {
		return nil
	}
	books, ok := booksRaw.(map[string]any)
	if !ok {
		return nil
	}

	// Build map: directory name → ordinal
	result := make(map[string]int)
	for _, bookData := range books {
		book, ok := bookData.(map[string]any)
		if !ok {
			continue
		}
		nameRaw, ok := book["name"]
		if !ok {
			continue
		}
		name, ok := nameRaw.(string)
		if !ok {
			continue
		}
		ordinalRaw, ok := book["ordinal"]
		if !ok {
			continue
		}
		ordinal, ok := ordinalRaw.(int64)
		if !ok {
			continue
		}
		// Convert TOML name to directory name: spaces → underscores
		dirName := strings.ReplaceAll(name, " ", "_")
		result[dirName] = int(ordinal)
	}

	if len(result) != 66 {
		return nil // Incomplete, fall back to hardcoded
	}
	return result
}

// ────────────────────────────────────────────────────────────────
// FILE 4 Helpers: Ternary Conversion
// ────────────────────────────────────────────────────────────────

// toTrit5 converts a value (0-242) to 5-trit balanced ternary string.
// Each trit is: - (=-1), 0 (=0), + (=+1)
// Used by: writeTernaryFile() for FILE 4 output.
func toTrit5(val int) string {
	trits := make([]byte, 5)
	for i := 4; i >= 0; i-- {
		digit := val % 3
		val /= 3
		switch digit {
		case 0:
			trits[i] = '0'
		case 1:
			trits[i] = '+'
		case 2:
			trits[i] = '-' // balanced: 2 → -1 (borrow)
			val++
		}
	}
	return string(trits)
}

// ────────────────────────────────────────────────────────────────
// FILE 6 Helpers: Encryption Data Loading
// ────────────────────────────────────────────────────────────────

// EncryptionData holds parsed encryption.toml data for ASCII Key generation.
type EncryptionData struct {
	Hebrew map[string]LetterEntry // Hebrew letters (OT)
	Greek  map[string]LetterEntry // Greek letters (NT)
}

// LetterEntry represents one letter from encryption.toml.
type LetterEntry struct {
	Letter   string `toml:"letter"`
	Position int    `toml:"position"`
	Value    int    `toml:"value"`
	Coords   [3]int `toml:"coords"`
}

// loadEncryptionTOML loads Hebrew and Greek letter mappings from encryption.toml.
// Used by: buildASCIIKey() for FILE 6 generation.
func loadEncryptionTOML(bereshitRoot string) (*EncryptionData, error) {
	encPath := filepath.Join(bereshitRoot, EncryptionPath)

	// Parse TOML file
	var data map[string]interface{}
	if _, err := toml.DecodeFile(encPath, &data); err != nil {
		return nil, fmt.Errorf("decode encryption.toml: %w", err)
	}

	result := &EncryptionData{
		Hebrew: make(map[string]LetterEntry),
		Greek:  make(map[string]LetterEntry),
	}

	// Parse Hebrew entries
	if hebrewRaw, ok := data["hebrew"]; ok {
		if hebrewMap, ok := hebrewRaw.(map[string]interface{}); ok {
			if encryptRaw, ok := hebrewMap["encrypt"]; ok {
				if encryptMap, ok := encryptRaw.(map[string]interface{}); ok {
					for name, entryRaw := range encryptMap {
						if entry, ok := entryRaw.(map[string]interface{}); ok {
							le := parseLetterEntry(entry)
							result.Hebrew[name] = le
						}
					}
				}
			}
		}
	}

	// Parse Greek entries
	if greekRaw, ok := data["greek"]; ok {
		if greekMap, ok := greekRaw.(map[string]interface{}); ok {
			if encryptRaw, ok := greekMap["encrypt"]; ok {
				if encryptMap, ok := encryptRaw.(map[string]interface{}); ok {
					for name, entryRaw := range encryptMap {
						if entry, ok := entryRaw.(map[string]interface{}); ok {
							le := parseLetterEntry(entry)
							result.Greek[name] = le
						}
					}
				}
			}
		}
	}

	return result, nil
}

// parseLetterEntry extracts a LetterEntry from TOML map data.
func parseLetterEntry(entry map[string]interface{}) LetterEntry {
	le := LetterEntry{}

	if letter, ok := entry["letter"].(string); ok {
		le.Letter = letter
	}
	if pos, ok := entry["position"].(int64); ok {
		le.Position = int(pos)
	}
	if val, ok := entry["value"].(int64); ok {
		le.Value = int(val)
	}
	if coords, ok := entry["coords"].([]interface{}); ok && len(coords) == 3 {
		for i, c := range coords {
			if v, ok := c.(int64); ok {
				le.Coords[i] = int(v)
			}
		}
	}

	return le
}

// ────────────────────────────────────────────────────────────────
// FILE 7 Helpers: Testament Detection
// ────────────────────────────────────────────────────────────────

// isOldTestament returns true if book ordinal is 1-39 (Genesis-Malachi).
// Used by: Scripture Key generation for Hebrew/Greek field assignment.
func isOldTestament(bookOrdinal int) bool {
	return bookOrdinal >= 1 && bookOrdinal <= 39
}

// getTestament returns "OT" or "NT" based on book name.
// Used by: Scripture Key and Parallel entries.
func getTestament(bookName string) string {
	ordinal, ok := canonicalBookOrder[bookName]
	if !ok {
		return "Unknown"
	}
	if isOldTestament(ordinal) {
		return "OT"
	}
	return "NT"
}

// ────────────────────────────────────────────────────────────────
// GENERIC I/O: Config-Driven Write Functions
// ────────────────────────────────────────────────────────────────
// These functions work with SetID and FileType to write any format.
// Used by: generateSet() for all 3 sets × 7 file types = 21 files.

// writeJSONFile writes any JSON-serializable data to a file.
// Generic: works for VerseIndex, ParallelIndex, CubeIndex, ASCIIKey, etc.
func writeJSONFile(data interface{}, path string) error {
	jsonData, err := json.MarshalIndent(data, "", "  ")
	if err != nil {
		return fmt.Errorf("marshal json: %w", err)
	}
	if err := os.WriteFile(path, jsonData, 0644); err != nil {
		return fmt.Errorf("write json: %w", err)
	}
	return nil
}

// writeCSVFile writes CSV data from a VerseIndex.
// Format: ordinal,book,chapter,verse,low,high
func writeCSVFile(index *VerseIndex, path string) error {
	file, err := os.Create(path)
	if err != nil {
		return err
	}
	defer file.Close()

	fmt.Fprintln(file, "ordinal,book,chapter,verse,low,high")

	var allEntries []VerseEntry
	for _, group := range index.Groups {
		allEntries = append(allEntries, group.Verses...)
	}
	sort.Slice(allEntries, func(i, j int) bool {
		return allEntries[i].Ordinal < allEntries[j].Ordinal
	})

	for _, e := range allEntries {
		fmt.Fprintf(file, "%d,%s,%d,%d,%d,%d\n",
			e.Ordinal, e.Book, e.Chapter, e.Verse, e.Low, e.High)
	}

	for _, v := range index.Variants {
		fmt.Fprintf(file, "%d,%s,%d,%d,%d,0\n",
			v.Trite, v.Book, v.Chapter, v.Verse, v.Trite)
	}

	return nil
}

// writeParallelCSVFile writes CSV data from a ParallelIndex.
// Format: ordinal,book,chapter,verse,low,high,testament,kjv,web
func writeParallelCSVFile(index *ParallelIndex, path string) error {
	file, err := os.Create(path)
	if err != nil {
		return err
	}
	defer file.Close()

	fmt.Fprintln(file, "ordinal,book,chapter,verse,low,high,testament,kjv,web")

	var allEntries []ParallelEntry
	for _, group := range index.Groups {
		allEntries = append(allEntries, group.Verses...)
	}
	sort.Slice(allEntries, func(i, j int) bool {
		return allEntries[i].Ordinal < allEntries[j].Ordinal
	})

	for _, e := range allEntries {
		// Escape quotes in text
		kjv := strings.ReplaceAll(e.KJV, "\"", "\"\"")
		web := strings.ReplaceAll(e.WEB, "\"", "\"\"")
		fmt.Fprintf(file, "%d,%s,%d,%d,%d,%d,%s,\"%s\",\"%s\"\n",
			e.Ordinal, e.Book, e.Chapter, e.Verse, e.Low, e.High, e.Testament, kjv, web)
	}

	return nil
}

// writeBinaryFile writes compact binary format.
// Format: 4-byte header "VX01" + (2 bytes per verse: low, high)
func writeBinaryFile(index *VerseIndex, path string) error {
	file, err := os.Create(path)
	if err != nil {
		return err
	}
	defer file.Close()

	file.Write([]byte{'V', 'X', '0', '1'})

	var allEntries []VerseEntry
	for _, group := range index.Groups {
		allEntries = append(allEntries, group.Verses...)
	}
	sort.Slice(allEntries, func(i, j int) bool {
		return allEntries[i].Ordinal < allEntries[j].Ordinal
	})

	for _, e := range allEntries {
		file.Write([]byte{byte(e.Low), byte(e.High)})
	}

	for _, v := range index.Variants {
		file.Write([]byte{byte(v.Trite), 0})
	}

	return nil
}

// writeParallelBinaryFile writes compact binary format for parallel.
// Format: 4-byte header "VX02" + (2 bytes per verse: low, high)
func writeParallelBinaryFile(index *ParallelIndex, path string) error {
	file, err := os.Create(path)
	if err != nil {
		return err
	}
	defer file.Close()

	file.Write([]byte{'V', 'X', '0', '2'}) // Version 02 for parallel

	var allEntries []ParallelEntry
	for _, group := range index.Groups {
		allEntries = append(allEntries, group.Verses...)
	}
	sort.Slice(allEntries, func(i, j int) bool {
		return allEntries[i].Ordinal < allEntries[j].Ordinal
	})

	for _, e := range allEntries {
		file.Write([]byte{byte(e.Low), byte(e.High)})
	}

	for _, v := range index.Variants {
		file.Write([]byte{byte(v.Trite), 0})
	}

	return nil
}

// writeTernaryFile writes ternary trit5 representation.
// Format: ordinal,reference,low_trit5,high_trit5,combined
func writeTernaryFile(index *VerseIndex, path string) error {
	file, err := os.Create(path)
	if err != nil {
		return err
	}
	defer file.Close()

	fmt.Fprintln(file, "ordinal,reference,low_trit5,high_trit5,combined")

	var allEntries []VerseEntry
	for _, group := range index.Groups {
		allEntries = append(allEntries, group.Verses...)
	}
	sort.Slice(allEntries, func(i, j int) bool {
		return allEntries[i].Ordinal < allEntries[j].Ordinal
	})

	for _, e := range allEntries {
		lowTrit := toTrit5(e.Low)
		highTrit := toTrit5(e.High)
		ref := fmt.Sprintf("%s %d:%d", e.Book, e.Chapter, e.Verse)
		fmt.Fprintf(file, "%d,%s,%s,%s,%s%s\n",
			e.Ordinal, ref, lowTrit, highTrit, highTrit, lowTrit)
	}

	return nil
}

// writeParallelTernaryFile writes ternary trit5 representation for parallel.
func writeParallelTernaryFile(index *ParallelIndex, path string) error {
	file, err := os.Create(path)
	if err != nil {
		return err
	}
	defer file.Close()

	fmt.Fprintln(file, "ordinal,reference,low_trit5,high_trit5,combined,testament")

	var allEntries []ParallelEntry
	for _, group := range index.Groups {
		allEntries = append(allEntries, group.Verses...)
	}
	sort.Slice(allEntries, func(i, j int) bool {
		return allEntries[i].Ordinal < allEntries[j].Ordinal
	})

	for _, e := range allEntries {
		lowTrit := toTrit5(e.Low)
		highTrit := toTrit5(e.High)
		ref := fmt.Sprintf("%s %d:%d", e.Book, e.Chapter, e.Verse)
		fmt.Fprintf(file, "%d,%s,%s,%s,%s%s,%s\n",
			e.Ordinal, ref, lowTrit, highTrit, highTrit, lowTrit, e.Testament)
	}

	return nil
}

// ════════════════════════════════════════════════════════════════
// CORE OPERATIONS
// ════════════════════════════════════════════════════════════════
//
// Organization: CONFIG-DRIVEN PIPELINE (Builders → Verification → Generation)
//
//   CORE BUILDERS: Foundation
//     walkScripture()         - Walk Scripture files, extract entries
//     groupByPosition()       - Group by low byte (mod 243)
//     buildIndex()            - Create VerseIndex from entries
//
//   FILE 5 BUILDERS: Cube Index
//     buildCubeIndex()        - Single translation (9×27 cube)
//     buildParallelCubeIndex() - Parallel (KJV+WEB)
//
//   FILE 6 BUILDERS: ASCII Key
//     buildASCIIKey()         - Character encryption table
//
//   FILE 7 BUILDERS: Scripture Key
//     buildScriptureKey()     - Single translation
//     buildParallelScriptureKey() - Parallel
//
//   SET 3 BUILDERS: Parallel Index
//     buildParallelIndex()    - Combine KJV + WEB entries
//
//   VERIFICATION:
//     verifyReversibility()   - Encode/decode cycle check
//
//   GENERATION PIPELINE: (Entry point for all output)
//     generateSet()           - Router for any set
//     generateSingleSet()     - KJV or WEB (7 files)
//     generateParallelSet()   - Parallel (7 files)
//     generateAllSets()       - All 21 files (3 sets × 7)
//
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// CORE BUILDERS: Scripture Walking
// ────────────────────────────────────────────────────────────────

// walkScripture traverses Scripture directory in canonical book order.
// Assigns ordinals sequentially as verses are discovered.
//
// Parameters:
//
//	scriptureRoot: Path to word/scripture/KJV
//
// Returns:
//
//	[]VerseEntry: All verses with ordinals assigned (1-31102)
//	error: Directory access or parsing failure
func walkScripture(scriptureRoot string) ([]VerseEntry, error) {
	var entries []VerseEntry
	ordinal := 1

	// Get list of book directories
	bookEntries, err := os.ReadDir(scriptureRoot)
	if err != nil {
		return nil, fmt.Errorf("read scripture root: %w", err)
	}

	// Extract book directory names
	var bookDirs []string
	for _, entry := range bookEntries {
		if entry.IsDir() {
			if _, ok := canonicalBookOrder[entry.Name()]; ok {
				bookDirs = append(bookDirs, entry.Name())
			}
		}
	}

	// Sort by canonical order
	bookDirs = sortedBookDirs(bookDirs)

	// Walk each book in canonical order
	for _, bookName := range bookDirs {
		bookPath := filepath.Join(scriptureRoot, bookName)

		// Get chapter directories
		chapterEntries, err := os.ReadDir(bookPath)
		if err != nil {
			return nil, fmt.Errorf("read book %s: %w", bookName, err)
		}

		// Collect and sort chapters
		var chapters []struct {
			num  int
			name string
		}
		for _, entry := range chapterEntries {
			if entry.IsDir() && strings.HasPrefix(entry.Name(), "Chapter_") {
				num, err := parseChapterNumber(entry.Name())
				if err != nil {
					continue // Skip non-chapter directories
				}
				chapters = append(chapters, struct {
					num  int
					name string
				}{num, entry.Name()})
			}
		}
		sort.Slice(chapters, func(i, j int) bool {
			return chapters[i].num < chapters[j].num
		})

		// Walk each chapter
		for _, chapter := range chapters {
			chapterPath := filepath.Join(bookPath, chapter.name)

			// Get verse files
			verseFiles, err := os.ReadDir(chapterPath)
			if err != nil {
				return nil, fmt.Errorf("read chapter %s/%s: %w", bookName, chapter.name, err)
			}

			// Collect and sort verses
			var verses []struct {
				num  int
				name string
			}
			for _, entry := range verseFiles {
				if !entry.IsDir() && strings.HasPrefix(entry.Name(), "Verse_") {
					num, err := parseVerseNumber(entry.Name())
					if err != nil {
						continue // Skip non-verse files
					}
					verses = append(verses, struct {
						num  int
						name string
					}{num, entry.Name()})
				}
			}
			sort.Slice(verses, func(i, j int) bool {
				return verses[i].num < verses[j].num
			})

			// Create entry for each verse
			for _, verse := range verses {
				// Read verse content
				versePath := filepath.Join(chapterPath, verse.name)
				content, err := os.ReadFile(versePath)
				if err != nil {
					return nil, fmt.Errorf("read verse %s: %w", versePath, err)
				}
				text := strings.TrimSpace(string(content))

				low, high := Encode(ordinal)
				anchor := low / NumCubePos  // 0-8
				cube := low % NumCubePos    // 0-26
				entry := VerseEntry{
					Ordinal: ordinal,
					Book:    bookName,
					Chapter: chapter.num,
					Verse:   verse.num,
					Low:     low,
					High:    high,
					Anchor:  anchor,
					Cube:    cube,
					Text:    text,
				}
				entries = append(entries, entry)
				ordinal++
			}
		}
	}

	return entries, nil
}

// ────────────────────────────────────────────────────────────────
// CORE BUILDERS: Ordinal Index Operations (FILES 1-4)
// ────────────────────────────────────────────────────────────────

// groupByPosition groups verses into 243 positions by low byte.
// Each group contains ~128 verses (31102 / 243 ≈ 128).
// Enables O(1) lookup: groups[low][high] → verse.
// Used by: buildIndex() for SET 1 (KJV) and SET 2 (WEB).
func groupByPosition(entries []VerseEntry) [Trit5Base]VerseGroup {
	var groups [Trit5Base]VerseGroup

	// Initialize groups
	for i := 0; i < Trit5Base; i++ {
		groups[i] = VerseGroup{
			Position: i,
			Verses:   make([]VerseEntry, 0, MaxPages), // Pre-allocate for ~128 entries
		}
	}

	// Distribute verses into groups by low byte
	for _, entry := range entries {
		groups[entry.Low].Verses = append(groups[entry.Low].Verses, entry)
	}

	// Sort each group by high byte for O(1) indexed access
	for i := range groups {
		sort.Slice(groups[i].Verses, func(a, b int) bool {
			return groups[i].Verses[a].High < groups[i].Verses[b].High
		})
	}

	return groups
}

// buildIndex constructs complete verse index from Scripture files.
// Orchestrates: walkScripture() → groupByPosition() → add variants.
// Used by: main() for SET 1 (KJV) and SET 2 (WEB).
func buildIndex(scriptureRoot string, expectedCount int) (*VerseIndex, error) {
	// Walk Scripture to get flat verse list with ordinals
	entries, err := walkScripture(scriptureRoot)
	if err != nil {
		return nil, fmt.Errorf("walk scripture: %w", err)
	}

	// Verify expected count (31102 for KJV, 31115 for WEB)
	if len(entries) != expectedCount {
		return nil, fmt.Errorf("expected %d verses, found %d", expectedCount, len(entries))
	}

	// Group by position (low byte)
	groups := groupByPosition(entries)

	// Calculate stats
	maxPerGroup := 0
	minPerGroup := len(entries)
	for _, g := range groups {
		if len(g.Verses) > maxPerGroup {
			maxPerGroup = len(g.Verses)
		}
		if len(g.Verses) < minPerGroup {
			minPerGroup = len(g.Verses)
		}
	}

	// Build complete index
	index := &VerseIndex{
		Groups:   groups,
		Variants: webVariants,
		Stats: IndexStats{
			TotalVerses:   len(entries),
			TotalVariants: len(webVariants),
			TotalEntries:  len(entries) + len(webVariants),
			MaxPerGroup:   maxPerGroup,
			MinPerGroup:   minPerGroup,
		},
	}

	return index, nil
}

// ────────────────────────────────────────────────────────────────
// FILE 5 BUILDERS: Cube Index
// ────────────────────────────────────────────────────────────────

// buildCubeIndex creates a CubeIndex from a VerseIndex.
// Reorganizes 243 groups into 9 anchors × 27 cube positions.
// Used by: generateSingleSet() for FILE 5 generation.
func buildCubeIndex(verseIdx *VerseIndex) *CubeIndex {
	cubeIdx := &CubeIndex{
		Variants: verseIdx.Variants,
		Stats: CubeStats{
			TotalVerses: verseIdx.Stats.TotalVerses,
			Anchors:     NumAnchors,
			CubePos:     NumCubePos,
		},
	}

	maxPerCell := 0
	for _, group := range verseIdx.Groups {
		for _, entry := range group.Verses {
			anchor := entry.Anchor
			cube := entry.Cube
			cubeIdx.Cube[anchor][cube] = append(cubeIdx.Cube[anchor][cube], entry)
			if len(cubeIdx.Cube[anchor][cube]) > maxPerCell {
				maxPerCell = len(cubeIdx.Cube[anchor][cube])
			}
		}
	}
	cubeIdx.Stats.MaxPerCell = maxPerCell

	return cubeIdx
}

// ────────────────────────────────────────────────────────────────
// GENERATION PIPELINE: Config-Driven Set Generation
// ────────────────────────────────────────────────────────────────

// generateSet generates all 7 files for a single set (KJV, WEB, or Parallel).
// Uses GetFilepath() for config-driven filename generation.
//
// Parameters:
//
//	setID:       SetKJV, SetWEB, or SetParallel
//	index:       VerseIndex for SET 1 or SET 2 (nil for Parallel)
//	parallel:    ParallelIndex for SET 3 (nil for KJV/WEB)
//	outputDir:   Directory for output files
//	bereshitRoot: Root path for encryption.toml loading
//
// Returns error if any file generation fails.
func generateSet(setID SetID, index *VerseIndex, parallel *ParallelIndex, outputDir, bereshitRoot string) error {
	// Ensure output directory exists
	if err := os.MkdirAll(outputDir, 0755); err != nil {
		return fmt.Errorf("create output dir: %w", err)
	}

	// Handle parallel vs single-translation sets
	if setID == SetParallel {
		return generateParallelSet(parallel, outputDir, bereshitRoot)
	}

	return generateSingleSet(setID, index, outputDir, bereshitRoot)
}

// generateSingleSet generates all 7 files for KJV or WEB set.
func generateSingleSet(setID SetID, index *VerseIndex, outputDir, bereshitRoot string) error {
	// FILE 1: Ordinal Index JSON
	if err := writeJSONFile(index, GetFilepath(setID, FileOrdinalJSON, outputDir)); err != nil {
		return fmt.Errorf("FILE 1 (JSON): %w", err)
	}

	// FILE 2: Ordinal Index CSV
	if err := writeCSVFile(index, GetFilepath(setID, FileOrdinalCSV, outputDir)); err != nil {
		return fmt.Errorf("FILE 2 (CSV): %w", err)
	}

	// FILE 3: Ordinal Index Binary
	if err := writeBinaryFile(index, GetFilepath(setID, FileOrdinalBin, outputDir)); err != nil {
		return fmt.Errorf("FILE 3 (Binary): %w", err)
	}

	// FILE 4: Ordinal Index Ternary
	if err := writeTernaryFile(index, GetFilepath(setID, FileOrdinalTrit, outputDir)); err != nil {
		return fmt.Errorf("FILE 4 (Ternary): %w", err)
	}

	// FILE 5: Cube Index
	cubeIdx := buildCubeIndex(index)
	if err := writeJSONFile(cubeIdx, GetFilepath(setID, FileCubeJSON, outputDir)); err != nil {
		return fmt.Errorf("FILE 5 (Cube): %w", err)
	}

	// FILE 6: ASCII Key
	asciiKey := buildASCIIKey(bereshitRoot)
	if err := writeJSONFile(asciiKey, GetFilepath(setID, FileASCIIKey, outputDir)); err != nil {
		return fmt.Errorf("FILE 6 (ASCII Key): %w", err)
	}

	// FILE 7: Scripture Key
	scriptureKey := buildScriptureKey(index)
	if err := writeJSONFile(scriptureKey, GetFilepath(setID, FileScriptureKey, outputDir)); err != nil {
		return fmt.Errorf("FILE 7 (Scripture Key): %w", err)
	}

	return nil
}

// generateParallelSet generates all 7 files for the Parallel set (SET 3).
func generateParallelSet(parallel *ParallelIndex, outputDir, bereshitRoot string) error {
	setID := SetParallel

	// FILE 1: Parallel Ordinal Index JSON
	if err := writeJSONFile(parallel, GetFilepath(setID, FileOrdinalJSON, outputDir)); err != nil {
		return fmt.Errorf("FILE 1 (JSON): %w", err)
	}

	// FILE 2: Parallel Ordinal Index CSV
	if err := writeParallelCSVFile(parallel, GetFilepath(setID, FileOrdinalCSV, outputDir)); err != nil {
		return fmt.Errorf("FILE 2 (CSV): %w", err)
	}

	// FILE 3: Parallel Ordinal Index Binary
	if err := writeParallelBinaryFile(parallel, GetFilepath(setID, FileOrdinalBin, outputDir)); err != nil {
		return fmt.Errorf("FILE 3 (Binary): %w", err)
	}

	// FILE 4: Parallel Ordinal Index Ternary
	if err := writeParallelTernaryFile(parallel, GetFilepath(setID, FileOrdinalTrit, outputDir)); err != nil {
		return fmt.Errorf("FILE 4 (Ternary): %w", err)
	}

	// FILE 5: Parallel Cube Index
	parallelCube := buildParallelCubeIndex(parallel)
	if err := writeJSONFile(parallelCube, GetFilepath(setID, FileCubeJSON, outputDir)); err != nil {
		return fmt.Errorf("FILE 5 (Cube): %w", err)
	}

	// FILE 6: ASCII Key (same for all sets)
	asciiKey := buildASCIIKey(bereshitRoot)
	if err := writeJSONFile(asciiKey, GetFilepath(setID, FileASCIIKey, outputDir)); err != nil {
		return fmt.Errorf("FILE 6 (ASCII Key): %w", err)
	}

	// FILE 7: Parallel Scripture Key
	parallelScriptureKey := buildParallelScriptureKey(parallel)
	if err := writeJSONFile(parallelScriptureKey, GetFilepath(setID, FileScriptureKey, outputDir)); err != nil {
		return fmt.Errorf("FILE 7 (Scripture Key): %w", err)
	}

	return nil
}

// generateAllSets generates all 21 files (3 sets × 7 files each).
// Called by main() for complete index generation.
func generateAllSets(kjvIndex, webIndex *VerseIndex, outputDir, bereshitRoot string) error {
	// SET 1: KJV (encoding)
	fmt.Println("Generating SET 1: KJV (7 files)...")
	if err := generateSet(SetKJV, kjvIndex, nil, outputDir, bereshitRoot); err != nil {
		return fmt.Errorf("SET 1 (KJV): %w", err)
	}
	for _, ft := range AllFileTypes {
		fmt.Printf("  ✓ %s\n", GetFilename(SetKJV, ft))
	}

	// SET 2: WEB (decoding)
	fmt.Println("Generating SET 2: WEB (7 files)...")
	if err := generateSet(SetWEB, webIndex, nil, outputDir, bereshitRoot); err != nil {
		return fmt.Errorf("SET 2 (WEB): %w", err)
	}
	for _, ft := range AllFileTypes {
		fmt.Printf("  ✓ %s\n", GetFilename(SetWEB, ft))
	}

	// SET 3: Parallel (side-by-side)
	fmt.Println("Generating SET 3: Parallel (7 files)...")
	parallel := buildParallelIndex(kjvIndex, webIndex)
	if err := generateSet(SetParallel, nil, parallel, outputDir, bereshitRoot); err != nil {
		return fmt.Errorf("SET 3 (Parallel): %w", err)
	}
	for _, ft := range AllFileTypes {
		fmt.Printf("  ✓ %s\n", GetFilename(SetParallel, ft))
	}

	return nil
}

// ────────────────────────────────────────────────────────────────
// VERIFICATION: Encode/Decode Cycle Check
// ────────────────────────────────────────────────────────────────

// verifyReversibility checks encode(decode(x)) = x for all entries.
// Used by: main() after building each index.
func verifyReversibility(index *VerseIndex) error {
	for _, group := range index.Groups {
		for _, entry := range group.Verses {
			// Decode then encode should give same result
			decoded := Decode(entry.Low, entry.High)
			if decoded != entry.Ordinal {
				return fmt.Errorf("reversibility failed at ordinal %d: decode(%d,%d)=%d",
					entry.Ordinal, entry.Low, entry.High, decoded)
			}

			// Encode then decode should give same result
			low, high := Encode(entry.Ordinal)
			if low != entry.Low || high != entry.High {
				return fmt.Errorf("reversibility failed at ordinal %d: encode=%d,%d stored=%d,%d",
					entry.Ordinal, low, high, entry.Low, entry.High)
			}
		}
	}
	return nil
}

// ────────────────────────────────────────────────────────────────
// FILE 6 BUILDERS: ASCII Key
// ────────────────────────────────────────────────────────────────

// buildASCIIKey creates the ASCII-to-cube coordinate mapping.
// References encryption.toml for Hebrew (27) and Greek (27) letters.
// Used by: main() for all 3 sets.
func buildASCIIKey(bereshitRoot string) *ASCIIKey {
	key := &ASCIIKey{
		Entries:          make([]ASCIIKeyEntry, 256),
		HebrewRef:        "word/core/bible/encryption.toml#hebrew",
		GreekRef:         "word/core/bible/encryption.toml#greek",
		EncryptionSource: filepath.Join(bereshitRoot, EncryptionPath),
	}

	// Build entries for all 256 ASCII values
	for i := 0; i < 256; i++ {
		// Map ASCII to trite (0-242 for printable, 243-255 for special)
		triteValue := i % Trit5Base
		anchor := triteValue / NumCubePos
		cubePos := triteValue % NumCubePos

		// Calculate balanced ternary cube coordinates
		// Position 0-26 maps to (-1,-1,-1) through (+1,+1,+1)
		cubeX := (cubePos / 9) - 1
		cubeY := ((cubePos / 3) % 3) - 1
		cubeZ := (cubePos % 3) - 1

		key.Entries[i] = ASCIIKeyEntry{
			ASCII:      byte(i),
			Char:       string(rune(i)),
			Printable:  i >= 32 && i < 127,
			TriteValue: triteValue,
			CubeX:      cubeX,
			CubeY:      cubeY,
			CubeZ:      cubeZ,
			Anchor:     anchor,
			CubePos:    cubePos,
		}
	}

	return key
}

// ────────────────────────────────────────────────────────────────
// FILE 7 BUILDERS: Scripture Key
// ────────────────────────────────────────────────────────────────

// buildScriptureKey creates verse metadata with Hebrew/Greek placeholders.
// Used by: main() for SET 1 (KJV) and SET 2 (WEB).
func buildScriptureKey(index *VerseIndex) *ScriptureKey {
	key := &ScriptureKey{
		Entries: make([]ScriptureKeyEntry, 0, index.Stats.TotalVerses),
		Stats: ScriptureKeyStats{
			TotalEntries: index.Stats.TotalVerses,
		},
	}

	// Flatten and sort by ordinal
	var allEntries []VerseEntry
	for _, group := range index.Groups {
		allEntries = append(allEntries, group.Verses...)
	}
	sort.Slice(allEntries, func(i, j int) bool {
		return allEntries[i].Ordinal < allEntries[j].Ordinal
	})

	// Convert to scripture key entries
	for _, e := range allEntries {
		testament := getTestament(e.Book)
		entry := ScriptureKeyEntry{
			Ordinal:   e.Ordinal,
			Book:      e.Book,
			Chapter:   e.Chapter,
			Verse:     e.Verse,
			Text:      e.Text,
			Testament: testament,
			Low:       e.Low,
			High:      e.High,
			Anchor:    e.Anchor,
			Cube:      e.Cube,
		}
		key.Entries = append(key.Entries, entry)

		if testament == "OT" {
			key.Stats.OTEntries++
		} else {
			key.Stats.NTEntries++
		}
	}

	return key
}

// ────────────────────────────────────────────────────────────────
// SET 3 BUILDERS: Parallel Index (KJV↔WEB)
// ────────────────────────────────────────────────────────────────

// buildParallelIndex combines KJV and WEB indexes into parallel entries.
// Used by: main() for SET 3 generation.
func buildParallelIndex(kjvIndex, webIndex *VerseIndex) *ParallelIndex {
	parallel := &ParallelIndex{
		Variants: webIndex.Variants,
		Stats: IndexStats{
			TotalVerses:   kjvIndex.Stats.TotalVerses,
			TotalVariants: len(webIndex.Variants),
			TotalEntries:  kjvIndex.Stats.TotalVerses + len(webIndex.Variants),
		},
	}

	// Initialize groups
	for i := 0; i < Trit5Base; i++ {
		parallel.Groups[i] = ParallelGroup{
			Position: i,
			Verses:   make([]ParallelEntry, 0, MaxPages),
		}
	}

	// Build lookup map for WEB by ordinal
	webByOrdinal := make(map[int]string)
	for _, group := range webIndex.Groups {
		for _, entry := range group.Verses {
			webByOrdinal[entry.Ordinal] = entry.Text
		}
	}

	// Combine KJV with WEB
	for _, kjvGroup := range kjvIndex.Groups {
		for _, kjv := range kjvGroup.Verses {
			webText := webByOrdinal[kjv.Ordinal]
			entry := ParallelEntry{
				Ordinal:   kjv.Ordinal,
				Book:      kjv.Book,
				Chapter:   kjv.Chapter,
				Verse:     kjv.Verse,
				Low:       kjv.Low,
				High:      kjv.High,
				Anchor:    kjv.Anchor,
				Cube:      kjv.Cube,
				KJV:       kjv.Text,
				WEB:       webText,
				Testament: getTestament(kjv.Book),
			}
			parallel.Groups[kjv.Low].Verses = append(parallel.Groups[kjv.Low].Verses, entry)
		}
	}

	// Sort each group by high byte
	for i := range parallel.Groups {
		sort.Slice(parallel.Groups[i].Verses, func(a, b int) bool {
			return parallel.Groups[i].Verses[a].High < parallel.Groups[i].Verses[b].High
		})
	}

	// Calculate stats
	maxPerGroup := 0
	minPerGroup := parallel.Stats.TotalVerses
	for _, g := range parallel.Groups {
		if len(g.Verses) > maxPerGroup {
			maxPerGroup = len(g.Verses)
		}
		if len(g.Verses) < minPerGroup {
			minPerGroup = len(g.Verses)
		}
	}
	parallel.Stats.MaxPerGroup = maxPerGroup
	parallel.Stats.MinPerGroup = minPerGroup

	return parallel
}

// buildParallelCubeIndex creates cube index for parallel entries.
// Used by: generateParallelSet() for FILE 5 generation.
func buildParallelCubeIndex(parallel *ParallelIndex) *ParallelCubeIndex {
	cubeIdx := &ParallelCubeIndex{
		Variants: parallel.Variants,
		Stats: CubeStats{
			TotalVerses: parallel.Stats.TotalVerses,
			Anchors:     NumAnchors,
			CubePos:     NumCubePos,
		},
	}

	maxPerCell := 0
	for _, group := range parallel.Groups {
		for _, entry := range group.Verses {
			anchor := entry.Anchor
			cube := entry.Cube
			cubeIdx.Cube[anchor][cube] = append(cubeIdx.Cube[anchor][cube], entry)
			if len(cubeIdx.Cube[anchor][cube]) > maxPerCell {
				maxPerCell = len(cubeIdx.Cube[anchor][cube])
			}
		}
	}
	cubeIdx.Stats.MaxPerCell = maxPerCell

	return cubeIdx
}

// buildParallelScriptureKey creates scripture key for parallel entries.
func buildParallelScriptureKey(parallel *ParallelIndex) *ParallelScriptureKey {
	key := &ParallelScriptureKey{
		Entries: make([]ParallelScriptureKeyEntry, 0, parallel.Stats.TotalVerses),
		Stats: ScriptureKeyStats{
			TotalEntries: parallel.Stats.TotalVerses,
		},
	}

	var allEntries []ParallelEntry
	for _, group := range parallel.Groups {
		allEntries = append(allEntries, group.Verses...)
	}
	sort.Slice(allEntries, func(i, j int) bool {
		return allEntries[i].Ordinal < allEntries[j].Ordinal
	})

	for _, e := range allEntries {
		entry := ParallelScriptureKeyEntry{
			Ordinal:   e.Ordinal,
			Book:      e.Book,
			Chapter:   e.Chapter,
			Verse:     e.Verse,
			KJV:       e.KJV,
			WEB:       e.WEB,
			Testament: e.Testament,
			Low:       e.Low,
			High:      e.High,
			Anchor:    e.Anchor,
			Cube:      e.Cube,
		}
		key.Entries = append(key.Entries, entry)

		if e.Testament == "OT" {
			key.Stats.OTEntries++
		} else {
			key.Stats.NTEntries++
		}
	}

	return key
}

// ────────────────────────────────────────────────────────────────
// Error Handling/Recovery Patterns
// ────────────────────────────────────────────────────────────────
// [Reserved: Uses standard error returns with fmt.Errorf wrapping.
// Baton tool - errors propagate to main() which exits with status 1.]

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
// [Reserved: Baton tool - no exported APIs. Entry point is main() in CLOSING.]

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Section order: Validation → Execution → Cleanup → Overview → Policy → Ladder/Baton →
//                Surgical → Performance → Troubleshooting → Related → Future → Contribution → Reference
// This flows: verify → run → clean → document → guide future work
//
// ════════════════════════════════════════════════════════════════
// GROUP 1: CODING
// ════════════════════════════════════════════════════════════════
// ────────────────────────────────────────────────────────────────
// Code Validation: verse-index (Command)
// ────────────────────────────────────────────────────────────────
//
// Build Verification:
//   cd word/work && go build ./cmd/verse-index
//
// Runtime Verification:
//   BERESHIT_ROOT=/path/to/bereshit ./verse-index
//
// Testing Requirements:
//   - 21 output files exist in word/scripture/ (3 sets × 7 files)
//   - SET 1 (KJV): kjv-ordinal-index.json has 243 groups
//   - SET 2 (WEB): web-ordinal-index.json has 243 groups
//   - SET 3 (Parallel): parallel-ordinal-index.json has both KJV and WEB text
//   - Each CSV has correct verse counts (31102 KJV, 31115 WEB)
//   - Reversibility: encode(decode(x)) = x for all entries
//
// ────────────────────────────────────────────────────────────────
// Code Execution: verse-index (Command)
// ────────────────────────────────────────────────────────────────
//
// Entry Point: main()
//
// Execution Flow:
//   1. Locate BERESHIT_ROOT (env var or auto-detect)
//   2. Build KJV index: walkScripture() → buildIndex() → verifyReversibility()
//   3. Build WEB index: walkScripture() → buildIndex() → verifyReversibility()
//   4. generateAllSets() produces 21 files:
//      - SET 1 (KJV): 7 files via generateSingleSet()
//      - SET 2 (WEB): 7 files via generateSingleSet()
//      - SET 3 (Parallel): 7 files via generateParallelSet()
//   5. Exit 0 on success, 1 on error
//
// Exit Codes:
//   0 - All 21 files generated successfully
//   1 - Error (Scripture missing, count mismatch, write failure)

// main is the entry point for verse-index.
//
// Builds the Bible ordinal index from Scripture files.
// Outputs 243-group structure for O(1) lookup.
func main() {
	fmt.Println("═══════════════════════════════════════════════════════════════")
	fmt.Println("verse-index: Scripture → Ordinal Index Generator")
	fmt.Println("═══════════════════════════════════════════════════════════════")

	// Get BERESHIT_ROOT from environment
	root := os.Getenv("BERESHIT_ROOT")
	if root == "" {
		// Try to find it from current working directory
		cwd, err := os.Getwd()
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error: cannot determine working directory: %v\n", err)
			os.Exit(1)
		}
		// Walk up to find bereshit root (has word/ directory)
		for dir := cwd; dir != "/"; dir = filepath.Dir(dir) {
			if _, err := os.Stat(filepath.Join(dir, "word")); err == nil {
				root = dir
				break
			}
		}
		if root == "" {
			fmt.Fprintf(os.Stderr, "Error: BERESHIT_ROOT not set and cannot find word/ directory\n")
			os.Exit(1)
		}
	}

	fmt.Printf("BERESHIT_ROOT: %s\n", root)

	// Try to load canonical book order from config
	if loaded := loadCanonicalBookOrderFromConfig(root); loaded != nil {
		canonicalBookOrder = loaded
		fmt.Println("Book order: loaded from addressing.toml")
	} else {
		fmt.Println("Book order: using hardcoded fallback")
	}
	fmt.Println()

	// Build the index
	scriptureRoot := filepath.Join(root, ScripturePath)
	fmt.Printf("Walking Scripture: %s\n", scriptureRoot)

	index, err := buildIndex(scriptureRoot, TotalKJV)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error building KJV index: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("✓ Found %d verses in %d groups\n", index.Stats.TotalVerses, Trit5Base)
	fmt.Printf("  Max per group: %d, Min per group: %d\n", index.Stats.MaxPerGroup, index.Stats.MinPerGroup)
	fmt.Printf("  WEB variants: %d (trites 243-255)\n", index.Stats.TotalVariants)
	fmt.Println()

	// Verify reversibility
	fmt.Println("Verifying reversibility...")
	if err := verifyReversibility(index); err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
	fmt.Println("✓ All encode/decode cycles verified")
	fmt.Println()

	// ════════════════════════════════════════════════════════════════
	// WEB Index Generation (decoding counterpart)
	// ════════════════════════════════════════════════════════════════
	fmt.Println("───────────────────────────────────────────────────────────────")
	fmt.Println("WEB Index Generation (decoding)")
	fmt.Println("───────────────────────────────────────────────────────────────")

	webScriptureRoot := filepath.Join(root, WEBScripturePath)
	fmt.Printf("Walking Scripture: %s\n", webScriptureRoot)

	webIndex, err := buildIndex(webScriptureRoot, TotalWEB)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error building WEB index: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("✓ Found %d verses in %d groups\n", webIndex.Stats.TotalVerses, Trit5Base)
	fmt.Printf("  Max per group: %d, Min per group: %d\n", webIndex.Stats.MaxPerGroup, webIndex.Stats.MinPerGroup)
	fmt.Printf("  WEB variants: %d (trites 243-255)\n", webIndex.Stats.TotalVariants)
	fmt.Println()

	// Verify WEB reversibility
	fmt.Println("Verifying WEB reversibility...")
	if err := verifyReversibility(webIndex); err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
	fmt.Println("✓ All WEB encode/decode cycles verified")
	fmt.Println()

	// ════════════════════════════════════════════════════════════════
	// Generate All 21 Output Files (3 sets × 7 files)
	// ════════════════════════════════════════════════════════════════
	fmt.Println("───────────────────────────────────────────────────────────────")
	fmt.Println("Generating 21 Output Files (3 sets × 7 files each)")
	fmt.Println("───────────────────────────────────────────────────────────────")

	outputDir := filepath.Join(root, OutputPath)
	fmt.Printf("Output directory: %s\n\n", outputDir)

	if err := generateAllSets(index, webIndex, outputDir, root); err != nil {
		fmt.Fprintf(os.Stderr, "Error generating sets: %v\n", err)
		os.Exit(1)
	}

	fmt.Println()
	fmt.Println("═══════════════════════════════════════════════════════════════")
	fmt.Println("Index complete. Scripture is now addressable.")
	fmt.Printf("  KJV: %d entries (31102 verses + 13 variants)\n", index.Stats.TotalEntries)
	fmt.Printf("  WEB: %d entries (31102 verses + 13 variants)\n", webIndex.Stats.TotalEntries)
	fmt.Println("  Encoding (KJV) ↔ Decoding (WEB) bidirectional mapping complete.")
	fmt.Println("  21 files generated (3 sets × 7 files)")
	fmt.Println("═══════════════════════════════════════════════════════════════")
}

// ────────────────────────────────────────────────────────────────
// Code Cleanup: verse-index (Command)
// ────────────────────────────────────────────────────────────────
// [Reserved: Baton tool - no persistent resources. Go GC handles memory.
// Files are written atomically via os.WriteFile.]

// ════════════════════════════════════════════════════════════════
// GROUP 2: FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Executable Overview & Usage Summary
// ────────────────────────────────────────────────────────────────
// Baton tool that builds the 243-group verse index from Scripture files.
// See METADATA for full purpose. See BODY Org Chart for architecture.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
// Safe to Modify:
//   ✅ Add new output formats (XML, binary, etc.)
//   ✅ Adjust CSV column order
//
// Modify with Care:
//   ⚠️ canonicalBookOrder - must match word/core/bible/addressing.toml
//   ⚠️ webVariants - must match encoding.go in claude-global
//
// Never Modify:
//   ❌ Encode/Decode formulas (must match libtrit)
//   ❌ 4-block structure

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
// See BODY "CORE OPERATIONS" header for full organization.
//
// Baton Flow (config-driven):
//   main()
//     → buildIndex(KJV) → walkScripture() → groupByPosition()
//     → buildIndex(WEB) → walkScripture() → groupByPosition()
//     → generateAllSets()
//         → generateSet(SetKJV)     → generateSingleSet()
//         → generateSet(SetWEB)     → generateSingleSet()
//         → generateSet(SetParallel) → generateParallelSet()
//             → buildParallelIndex()
//             → writeJSONFile(), writeCSVFile(), writeBinaryFile(), etc.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
// To add new file type:
//   1. Add FileType constant in CONFIG TYPES (e.g., FileNewFormat)
//   2. Add to AllFileTypes slice
//   3. Add writeNewFormatFile() in Generic I/O section
//   4. Add case in generateSingleSet() and generateParallelSet()
//
// To add new set:
//   1. Add SetID constant (e.g., SetNewTranslation)
//   2. Add Scripture path constant
//   3. Update generateAllSets() to build and generate new set
//
// To add WEB variants: Update webVariants slice and adjust Lookup bounds.

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
// [Reserved: Baton tool runs once at build time. Performance not critical.]

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
// Problem: "cannot find word/scripture directory"
//   Solution: Set BERESHIT_ROOT or run from within bereshit repo
//
// Problem: "book X not found in canonical order"
//   Solution: Add book to canonicalBookOrder or check directory name spelling
//
// Problem: "verse count mismatch"
//   Solution: Verify Scripture files match expected structure (Chapter_N/Verse_M.txt)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
// Depends on:
//   - word/scripture/KJV/ (canonical KJV Scripture files)
//   - word/scripture/WEB/ (canonical WEB Scripture files)
//   - word/core/bible/addressing.toml (canonical book order)
//   - word/core/bible/encryption.toml (Hebrew/Greek letter mappings)
//
// Produces (21 files in word/scripture/):
//   SET 1 (KJV): kjv-ordinal-index.{json,csv,bin,trit}, kjv-cube-index.json,
//                kjv-ascii-key.json, kjv-scripture-key.json
//   SET 2 (WEB): web-ordinal-index.{json,csv,bin,trit}, web-cube-index.json,
//                web-ascii-key.json, web-scripture-key.json
//   SET 3 (Parallel): parallel-ordinal-index.{json,csv,bin,trit},
//                     parallel-cube-index.json, parallel-ascii-key.json,
//                     parallel-scripture-key.json
//
// Used by: claude-global/pkg/coordinates (Bible coordinate mapping)

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
// Implemented:
//   ✅ Load canonicalBookOrder from word/core/bible/addressing.toml
//   ✅ WEB translation support (13 variants at positions 243-255)
//   ✅ Config-driven architecture (SetID + FileType → GetFilename())
//   ✅ 21 output files (3 sets × 7 files each)
//   ✅ FILE 1: JSON output (full 243-group structure)
//   ✅ FILE 2: CSV output (flat ordinal lookup)
//   ✅ FILE 3: Binary output (compact 2-byte per verse)
//   ✅ FILE 4: Ternary output (trit5 balanced representation)
//   ✅ FILE 5: Cube Index (9×27 cube structure)
//   ✅ FILE 6: ASCII Key (character encryption table)
//   ✅ FILE 7: Scripture Key (verse metadata with testament)
//   ✅ SET 3: Parallel Index (KJV↔WEB side-by-side)
//
// Planned:
//   📋 Hebrew/Greek letter integration from encryption.toml
//   📋 Additional translations (ESV, NIV, etc.)

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
// This BATON tool builds the coordinate map for MillenniumOS.
// 243 groups + 13 variants = 256 positions = 1 byte of Scripture.
// "The words of the LORD are pure words" - Psalm 12:6

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Build and run:
//   cd word/work && go build ./cmd/verse-index && ./verse-index
//
// Or from repo root:
//   BERESHIT_ROOT=$(pwd) go run ./word/work/cmd/verse-index
//
// Output structure (21 files = 3 sets × 7 files):
//   {set}-ordinal-index.json   - 243-group structure for O(1) lookup
//   {set}-ordinal-index.csv    - Flat ordinal,book,chapter,verse,low,high
//   {set}-ordinal-index.bin    - Compact binary (4-byte header + 2 bytes/verse)
//   {set}-ordinal-index.trit   - Balanced ternary trit5 representation
//   {set}-cube-index.json      - 9×27 cube structure (anchors × positions)
//   {set}-ascii-key.json       - Character → cube coordinate mapping
//   {set}-scripture-key.json   - Full verse metadata with testament
//
//   Where {set} = kjv | web | parallel
//
// Config-driven filename generation:
//   GetFilename(SetKJV, FileOrdinalJSON)  // → "kjv-ordinal-index.json"
//   GetFilepath(SetWEB, FileCubeJSON, dir) // → "{dir}/web-cube-index.json"
//
// Lookup example:
//   ordinal := 1  // Genesis 1:1
//   low, high := Encode(ordinal)  // low=0, high=0
//   verse := index.Lookup(low, high)  // O(1) access

// ============================================================================
// END CLOSING
// ============================================================================
