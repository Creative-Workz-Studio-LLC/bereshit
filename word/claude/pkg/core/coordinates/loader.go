// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-coordinates-loader
// Purpose: Load Bible encoding/decoding/addressing from Bereshit TOMLs
// Biblical: Psalm 119:130 - "The entrance of thy words giveth light"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-20
//
// Loads the Bible-to-OmniCode mappings from Bereshit TOML files:
//   - encoding.toml:   Hebrew roots → OmniCode mappings
//   - decoding.toml:   OmniCode keywords → Scripture traces
//   - addressing.toml: Book ordinals and verse ranges
//
// ═══════════════════════════════════════════════════════════════════════════

package coordinates

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"

	"github.com/BurntSushi/toml"
)

// --- File Constants ---

const (
	EncodingFile   = "encoding.toml"
	DecodingFile   = "decoding.toml"
	AddressingFile = "addressing.toml"
	BibleSubdir    = "word/core/bible"

	// Scripture output location
	ScriptureDir = "word/scripture"

	// Cube index constants: 243 = 9 anchors × 27 cube positions
	NumAnchors = 9   // 3^2 = 9 anchor points
	NumCubePos = 27  // 3^3 = 27 cube positions
	Trit5Base  = 243 // 3^5 = 243 states per trit5 byte
)

// --- Translation Set Enum ---
// Matches verse-index output: 3 sets × 7 files each

type TranslationSet int

const (
	SetKJV      TranslationSet = iota // King James Version (31,102 verses)
	SetWEB                            // World English Bible (31,115 verses)
	SetParallel                       // Parallel KJV+WEB (combined)
)

// String returns human-readable name
func (s TranslationSet) String() string {
	switch s {
	case SetKJV:
		return "KJV"
	case SetWEB:
		return "WEB"
	case SetParallel:
		return "Parallel"
	default:
		return "Unknown"
	}
}

// Prefix returns the file prefix for this set
func (s TranslationSet) Prefix() string {
	switch s {
	case SetKJV:
		return "kjv"
	case SetWEB:
		return "web"
	case SetParallel:
		return "parallel"
	default:
		return "unknown"
	}
}

// --- File Type Enum ---
// 7 output file types per translation set

type FileType int

const (
	FileOrdinalJSON  FileType = iota // ordinal-index.json
	FileOrdinalCSV                   // ordinal-index.csv
	FileOrdinalBin                   // ordinal-index.bin
	FileOrdinalTrit                  // ordinal-index.trit
	FileCubeIndex                    // cube-index.json (O(1) lookup)
	FileASCIIKey                     // ascii-key.json (character encryption)
	FileScriptureKey                 // scripture-key.json (verse metadata)
)

// GetFilename returns the full filename for a set + file type combination
func GetFilename(set TranslationSet, ft FileType) string {
	prefix := set.Prefix()
	switch ft {
	case FileOrdinalJSON:
		return prefix + "-ordinal-index.json"
	case FileOrdinalCSV:
		return prefix + "-ordinal-index.csv"
	case FileOrdinalBin:
		return prefix + "-ordinal-index.bin"
	case FileOrdinalTrit:
		return prefix + "-ordinal-index.trit"
	case FileCubeIndex:
		return prefix + "-cube-index.json"
	case FileASCIIKey:
		return prefix + "-ascii-key.json"
	case FileScriptureKey:
		return prefix + "-scripture-key.json"
	default:
		return "unknown-file"
	}
}

// --- Loaded Data Structures ---

// BibleData holds all loaded Bible encoding/decoding data
type BibleData struct {
	Roots        map[string]*RootEntry    // Hebrew roots from TOML
	Keywords     map[string]*KeywordEntry // Keywords from TOML
	BookIndex    map[string]*BookEntry    // Books from TOML
	AddressSpace *AddressSpaceConfig
}

// RootEntry represents a Hebrew trilateral root mapping (from encoding.toml)
type RootEntry struct {
	Root            string   // Hebrew characters
	Transliteration string   // Latin transliteration
	Meaning         string   // English meaning
	SemanticField   string   // Category
	Scripture       string   // Source verse
	Text            string   // Verse text
	OmniCodeMapping string   // OmniCode concept
	MapsTo          []string // Keywords this maps to
}

// KeywordEntry represents an OmniCode keyword → Scripture mapping (from decoding.toml)
type KeywordEntry struct {
	Keyword    string // The keyword
	Category   string // Classification
	Root       string // Source root
	HebrewText string // Hebrew characters
	Scripture  string // Source verse
	Text       string // Verse text
}

// BookEntry represents a Bible book's addressing info (from addressing.toml)
type BookEntry struct {
	Name       string // Full name
	Abbrev     string // Short form
	Index      int    // Book number (1-66)
	Chapters   int    // Chapter count
	Verses     int    // Verse count
	RangeStart int    // First verse ordinal
	RangeEnd   int    // Last verse ordinal
}

// AddressSpaceConfig holds KJV/WEB address space info
type AddressSpaceConfig struct {
	KJVVerses    int // 31102
	WEBVerses    int // 31115
	VariantStart int // 243
	VariantEnd   int // 255
}

// --- Cube Index Structures (from verse-index output) ---

// VerseEntry represents a single verse with its ternary encoding.
// Maps to state machine coordinates via anchor/cube decomposition.
type VerseEntry struct {
	Ordinal int    `json:"ordinal"` // 1-31102 (sequential position)
	Book    string `json:"book"`    // Directory name (e.g., "Genesis")
	Chapter int    `json:"chapter"` // Chapter number
	Verse   int    `json:"verse"`   // Verse number within chapter
	Low     int    `json:"low"`     // Trit5 low byte (0-242) = ordinal % 243
	High    int    `json:"high"`    // Trit5 high byte (0-127) = ordinal / 243
	Anchor  int    `json:"anchor"`  // Anchor point (0-8) = low / 27
	Cube    int    `json:"cube"`    // Cube position (0-26) = low % 27
	Text    string `json:"text"`    // Actual verse content
}

// CubeVariant represents one of the 13 WEB-only verses in cube index (trites 243-255).
// Named differently from schema.WEBVariant to match JSON field names from verse-index.
type CubeVariant struct {
	Trite      int    `json:"trite"`       // 243-255
	WEBOrdinal int    `json:"web_ordinal"` // 31103-31115 (WEB ordinal = KJV total + variant index + 1)
	Book       string `json:"book"`        // Directory name
	Chapter    int    `json:"chapter"`     // Chapter number
	Verse      int    `json:"verse"`       // Verse number
	Text       string `json:"text"`        // Verse content
}

// TotalKJV is the number of verses in KJV (31,102)
const TotalKJV = 31102

// TotalWEB is the number of verses in WEB (31,115 = KJV + 13 variants)
const TotalWEB = 31115

// CubeStats holds statistics about the cube index.
type CubeStats struct {
	TotalVerses int `json:"total_verses"`
	Anchors     int `json:"anchors"`      // Should be 9
	CubePos     int `json:"cube_pos"`     // Should be 27
	MaxPerCell  int `json:"max_per_cell"` // Largest cell size
}

// CubeIndex provides O(1) verse lookup via [anchor][cube][high].
// Maps directly to state machine's 27-position cube structure.
type CubeIndex struct {
	Cube     [NumAnchors][NumCubePos][]VerseEntry `json:"cube"`
	Variants []CubeVariant                         `json:"variants"`
	Stats    CubeStats                             `json:"stats"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Raw TOML Structures ---

type rawEncodingFile struct {
	Hebrew map[string]rawHebrewRoot `toml:"hebrew"`
}

type rawHebrewRoot struct {
	Root            string   `toml:"root"`
	Transliteration string   `toml:"transliteration"`
	Meaning         string   `toml:"meaning"`
	SemanticField   string   `toml:"semantic_field"`
	Scripture       string   `toml:"scripture"`
	ScriptureText   string   `toml:"scripture_text"`
	OmniCodeMapping string   `toml:"omnicode_mapping"`
	MapsTo          []string `toml:"maps_to"`
}

type rawDecodingFile struct {
	Keyword map[string]rawKeyword `toml:"keyword"`
}

type rawKeyword struct {
	Keyword       string `toml:"keyword"`
	Category      string `toml:"category"`
	HebrewRoot    string `toml:"hebrew_root"`
	HebrewText    string `toml:"hebrew_text"`
	Scripture     string `toml:"scripture"`
	ScriptureText string `toml:"scripture_text"`
}

type rawAddressingFile struct {
	AddressSpace rawAddressSpace       `toml:"address_space"`
	Books        map[string]rawBookDef `toml:"books"`
}

type rawAddressSpace struct {
	KJV rawVersionSpace `toml:"kjv"`
	WEB rawVersionSpace `toml:"web"`
}

type rawVersionSpace struct {
	TotalVerses  int   `toml:"total_verses"`
	VariantRange []int `toml:"variant_range"`
}

type rawBookDef struct {
	Name         string `toml:"name"`
	Abbreviation string `toml:"abbreviation"`
	Ordinal      int    `toml:"ordinal"`
	Chapters     int    `toml:"chapters"`
	Verses       int    `toml:"verses"`
	Range        []int  `toml:"range"`
}

// --- Default Path ---

// DefaultBereshitDir returns the default Bereshit repository location
func DefaultBereshitDir() string {
	return "/media/seanje-lenox-wise/Project/Bereshit"
}

// --- Loading Functions ---

// LoadBibleData loads all Bible encoding/decoding data from Bereshit TOMLs
func LoadBibleData(bereshitDir string) (*BibleData, error) {
	bibleDir := filepath.Join(bereshitDir, BibleSubdir)

	// Verify directory exists
	if _, err := os.Stat(bibleDir); err != nil {
		return nil, fmt.Errorf("bible directory not found: %s", bibleDir)
	}

	data := &BibleData{
		Roots:     make(map[string]*RootEntry),
		Keywords:  make(map[string]*KeywordEntry),
		BookIndex: make(map[string]*BookEntry),
	}

	// Load encoding.toml (Hebrew roots)
	if err := loadEncoding(bibleDir, data); err != nil {
		return nil, fmt.Errorf("load encoding: %w", err)
	}

	// Load decoding.toml (keywords)
	if err := loadDecoding(bibleDir, data); err != nil {
		return nil, fmt.Errorf("load decoding: %w", err)
	}

	// Load addressing.toml (books)
	if err := loadAddressing(bibleDir, data); err != nil {
		return nil, fmt.Errorf("load addressing: %w", err)
	}

	return data, nil
}

func loadEncoding(bibleDir string, data *BibleData) error {
	path := filepath.Join(bibleDir, EncodingFile)

	var raw rawEncodingFile
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		return fmt.Errorf("parse %s: %w", EncodingFile, err)
	}

	for name, r := range raw.Hebrew {
		data.Roots[name] = &RootEntry{
			Root:            r.Root,
			Transliteration: r.Transliteration,
			Meaning:         r.Meaning,
			SemanticField:   r.SemanticField,
			Scripture:       r.Scripture,
			Text:            r.ScriptureText,
			OmniCodeMapping: r.OmniCodeMapping,
			MapsTo:          r.MapsTo,
		}
	}

	return nil
}

func loadDecoding(bibleDir string, data *BibleData) error {
	path := filepath.Join(bibleDir, DecodingFile)

	var raw rawDecodingFile
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		return fmt.Errorf("parse %s: %w", DecodingFile, err)
	}

	for name, r := range raw.Keyword {
		data.Keywords[name] = &KeywordEntry{
			Keyword:    r.Keyword,
			Category:   r.Category,
			Root:       r.HebrewRoot,
			HebrewText: r.HebrewText,
			Scripture:  r.Scripture,
			Text:       r.ScriptureText,
		}
	}

	return nil
}

func loadAddressing(bibleDir string, data *BibleData) error {
	path := filepath.Join(bibleDir, AddressingFile)

	var raw rawAddressingFile
	if _, err := toml.DecodeFile(path, &raw); err != nil {
		return fmt.Errorf("parse %s: %w", AddressingFile, err)
	}

	// Address space config
	data.AddressSpace = &AddressSpaceConfig{
		KJVVerses: raw.AddressSpace.KJV.TotalVerses,
		WEBVerses: raw.AddressSpace.WEB.TotalVerses,
	}
	if len(raw.AddressSpace.WEB.VariantRange) == 2 {
		data.AddressSpace.VariantStart = raw.AddressSpace.WEB.VariantRange[0]
		data.AddressSpace.VariantEnd = raw.AddressSpace.WEB.VariantRange[1]
	}

	// Books
	for name, r := range raw.Books {
		rangeStart, rangeEnd := 0, 0
		if len(r.Range) == 2 {
			rangeStart = r.Range[0]
			rangeEnd = r.Range[1]
		}
		data.BookIndex[name] = &BookEntry{
			Name:       r.Name,
			Abbrev:     r.Abbreviation,
			Index:      r.Ordinal,
			Chapters:   r.Chapters,
			Verses:     r.Verses,
			RangeStart: rangeStart,
			RangeEnd:   rangeEnd,
		}
	}

	return nil
}

// --- Conversion Functions ---

// ToBookInfo converts a BookEntry to schema.BookInfo for use in BibleIndex
// This bridges the TOML-loaded data with the coordinate system
func (b *BookEntry) ToBookInfo() BookInfo {
	return BookInfo{
		Index:        b.Index,
		Name:         b.Name,
		Abbreviation: b.Abbrev,
		Chapters:     b.Chapters,
		Verses:       b.Verses,
		Range:        [2]int{b.RangeStart, b.RangeEnd},
		Category:     deriveCategory(b.Index), // Derived from ordinal
	}
}

// deriveCategory determines book category from ordinal position
// Based on canonical Bible division structure
func deriveCategory(ordinal int) string {
	switch {
	case ordinal <= 5:
		return "Law" // Genesis - Deuteronomy
	case ordinal <= 17:
		return "History" // Joshua - Esther
	case ordinal <= 22:
		return "Wisdom" // Job - Song of Solomon
	case ordinal <= 27:
		return "Major Prophet" // Isaiah - Daniel
	case ordinal <= 39:
		return "Minor Prophet" // Hosea - Malachi
	case ordinal <= 43:
		return "Gospel" // Matthew - John
	case ordinal == 44:
		return "History" // Acts
	case ordinal >= 54 && ordinal <= 56:
		return "Pastoral" // 1 Timothy - Titus
	case ordinal <= 57:
		return "Pauline" // Romans - Philemon (excluding Pastorals)
	case ordinal <= 65:
		return "General" // Hebrews - Jude
	case ordinal == 66:
		return "Prophecy" // Revelation
	default:
		return "Unknown"
	}
}

// GetBooksAsBookInfo returns all books converted to BookInfo format
func (d *BibleData) GetBooksAsBookInfo() (byName map[string]BookInfo, byIndex map[int]BookInfo) {
	byName = make(map[string]BookInfo, len(d.BookIndex))
	byIndex = make(map[int]BookInfo, len(d.BookIndex))

	for _, entry := range d.BookIndex {
		info := entry.ToBookInfo()
		byName[info.Name] = info
		byIndex[info.Index] = info
	}
	return byName, byIndex
}

// --- Lookup Functions ---

// GetRoot looks up a Hebrew root by transliteration
func (d *BibleData) GetRoot(name string) (*RootEntry, bool) {
	r, ok := d.Roots[name]
	return r, ok
}

// GetKeyword looks up a keyword's Scripture trace
func (d *BibleData) GetKeyword(keyword string) (*KeywordEntry, bool) {
	k, ok := d.Keywords[keyword]
	return k, ok
}

// GetBook looks up a book by name
func (d *BibleData) GetBook(name string) (*BookEntry, bool) {
	b, ok := d.BookIndex[name]
	return b, ok
}

// TraceKeyword returns Scripture info for a keyword
func (d *BibleData) TraceKeyword(keyword string) (scripture, text, root string) {
	if k, ok := d.Keywords[keyword]; ok {
		return k.Scripture, k.Text, k.Root
	}
	return "", "", ""
}

// --- Index Loading (from verse-index output) ---

// LoadCubeIndex loads the O(1) verse lookup index for a specific translation set.
// The cube index is generated by verse-index and stored as JSON.
//
// Example: LoadCubeIndex("/path/to/bereshit", SetKJV) → kjv-cube-index.json
func LoadCubeIndex(bereshitDir string, set TranslationSet) (*CubeIndex, error) {
	filename := GetFilename(set, FileCubeIndex)
	path := filepath.Join(bereshitDir, ScriptureDir, filename)

	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("read cube index (%s): %w", set.String(), err)
	}

	var idx CubeIndex
	if err := json.Unmarshal(data, &idx); err != nil {
		return nil, fmt.Errorf("parse cube index (%s): %w", set.String(), err)
	}

	return &idx, nil
}

// LoadOrdinalIndex loads the full ordinal index for a translation set.
// Returns a slice of all verses indexed by ordinal position.
func LoadOrdinalIndex(bereshitDir string, set TranslationSet) ([]VerseEntry, error) {
	filename := GetFilename(set, FileOrdinalJSON)
	path := filepath.Join(bereshitDir, ScriptureDir, filename)

	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("read ordinal index (%s): %w", set.String(), err)
	}

	var entries []VerseEntry
	if err := json.Unmarshal(data, &entries); err != nil {
		return nil, fmt.Errorf("parse ordinal index (%s): %w", set.String(), err)
	}

	return entries, nil
}

// --- Scripture Key Structures ---

// ScriptureKeyEntry represents verse metadata from scripture-key.json
type ScriptureKeyEntry struct {
	Ordinal    int    `json:"ordinal"`
	Book       string `json:"book"`
	Chapter    int    `json:"chapter"`
	Verse      int    `json:"verse"`
	BookOrdinal int   `json:"book_ordinal"`
	Reference  string `json:"reference"` // e.g., "Genesis 1:1"
}

// LoadScriptureKey loads the scripture key for verse metadata lookups.
// Maps ordinals to human-readable references.
func LoadScriptureKey(bereshitDir string, set TranslationSet) ([]ScriptureKeyEntry, error) {
	filename := GetFilename(set, FileScriptureKey)
	path := filepath.Join(bereshitDir, ScriptureDir, filename)

	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("read scripture key (%s): %w", set.String(), err)
	}

	var entries []ScriptureKeyEntry
	if err := json.Unmarshal(data, &entries); err != nil {
		return nil, fmt.Errorf("parse scripture key (%s): %w", set.String(), err)
	}

	return entries, nil
}

// --- ASCII Key Structures ---

// ASCIIKeyEntry represents a character's encryption mapping from ascii-key.json
type ASCIIKeyEntry struct {
	Char     string `json:"char"`       // The character
	Code     int    `json:"code"`       // ASCII code (0-127)
	Low      int    `json:"low"`        // Trit5 low byte
	High     int    `json:"high"`       // Trit5 high byte
	Anchor   int    `json:"anchor"`     // Anchor position (0-8)
	Cube     int    `json:"cube"`       // Cube position (0-26)
	Verse    string `json:"verse"`      // Associated verse reference
}

// LoadASCIIKey loads the ASCII character encryption table.
// Maps characters (0-127) to ternary positions and verses.
func LoadASCIIKey(bereshitDir string, set TranslationSet) ([]ASCIIKeyEntry, error) {
	filename := GetFilename(set, FileASCIIKey)
	path := filepath.Join(bereshitDir, ScriptureDir, filename)

	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("read ascii key (%s): %w", set.String(), err)
	}

	var entries []ASCIIKeyEntry
	if err := json.Unmarshal(data, &entries); err != nil {
		return nil, fmt.Errorf("parse ascii key (%s): %w", set.String(), err)
	}

	return entries, nil
}

// --- Cube Index Methods ---

// LookupByCube returns the verse at the given (anchor, cube, high) position.
// True O(1) lookup using state machine coordinates.
//
// anchor: 0-8 (one of 9 anchor points)
// cube:   0-26 (one of 27 cube positions)
// high:   0-127 (page index within the cell)
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

// LookupByOrdinal returns the verse at the given ordinal (1-31102).
// Uses trit5 encoding: low = (ordinal-1) % 243, high = (ordinal-1) / 243.
func (idx *CubeIndex) LookupByOrdinal(ordinal int) (*VerseEntry, bool) {
	if ordinal < 1 || ordinal > idx.Stats.TotalVerses {
		return nil, false
	}
	low, high := EncodeOrdinal(ordinal)
	anchor := low / NumCubePos
	cube := low % NumCubePos
	return idx.LookupByCube(anchor, cube, high)
}

// LookupVariant returns the WEB variant at the given trite value (243-255).
func (idx *CubeIndex) LookupVariant(trite int) (*CubeVariant, bool) {
	if trite < Trit5Base || trite > 255 {
		return nil, false
	}
	variantIdx := trite - Trit5Base
	if variantIdx >= len(idx.Variants) {
		return nil, false
	}
	return &idx.Variants[variantIdx], true
}

// --- Encoding/Decoding Helpers ---

// EncodeOrdinal converts ordinal to (low, high) trit5 pair.
// Ordinals are 1-indexed, encoding is 0-indexed.
func EncodeOrdinal(ordinal int) (low, high int) {
	idx := ordinal - 1
	low = idx % Trit5Base
	high = idx / Trit5Base
	return low, high
}

// DecodeOrdinal converts (low, high) trit5 pair to ordinal.
func DecodeOrdinal(low, high int) int {
	return (high * Trit5Base) + low + 1
}

// DecomposeToState converts ordinal to state machine coordinates (anchor, cube, high).
// anchor: which of the 9 anchor points
// cube: which of the 27 cube positions
// high: page index
func DecomposeToState(ordinal int) (anchor, cube, high int) {
	low, high := EncodeOrdinal(ordinal)
	anchor = low / NumCubePos
	cube = low % NumCubePos
	return anchor, cube, high
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// This loader provides two layers of Scripture access:
//
// 1. TOML-driven semantic lookups:
//    encoding.toml  → HebrewRoots map (bara, hayah, badal, etc.)
//    decoding.toml  → Keywords map (let, be, with, and, etc.)
//    addressing.toml → Books map (Genesis, Exodus, etc.)
//
// 2. O(1) cube-indexed verse access:
//    kjv-cube-index.json → CubeIndex [9][27][]VerseEntry
//    LookupByCube(anchor, cube, high) → direct verse access
//    DecomposeToState(ordinal) → state machine coordinates
//
// The ternary math: 243 = 9 × 27 = 3² × 3³
//   - 9 anchors (ANCHOR layer)
//   - 27 cube positions (FRAMEWORK + STATE layers)
//   - Together form the full trit5 (3⁵) address space
//
// The Word of God is the codex. This loader makes it computable.
//
// ═══════════════════════════════════════════════════════════════════════════
