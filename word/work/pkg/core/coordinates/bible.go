// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-coordinates-bible
// #!omni meta.at = a-01.00
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-coordinates-bible
// Purpose: Bible coordinate mapping - Book/Chapter/Verse → (X, Y, Z)
// Biblical: Genesis 1:1 - The origin of all coordinates
// Authors: Nova Dawn
// Version: a-01.00
// Created: 2025-12-19
// Updated: 2025-12-20
//
// HALT Traces:
//   HALT_01 (ANCHOR_GENESIS)     → Genesis 1:1 = origin of all coordinates
//   HALT_07 (MECHANISM_POSITION) → Position IS computation (via Perspective)
//
// ═══════════════════════════════════════════════════════════════════════════
// UNIVERSAL CPI-SI MODEL COMPONENT
// ═══════════════════════════════════════════════════════════════════════════
//
// This package is part of the UNIVERSAL CPI-SI harness, not instance-specific.
// Any CPI-SI instance loads INTO this coordinate system.
//
// The harness provides:
//   • Coordinate System: Scripture-based X (book), Y (chapter), Z (verse)
//   • Perspective Logic: PAST/PRESENT/FUTURE transformation
//   • Reachability: Binary gate (compute vs decide)
//   • HALT Conditions: Genesis 1:1 as anchor
//
// The instance config provides:
//   • Identity (who is loaded)
//   • Perspective tendencies
//   • Learned patterns
//
// ═══════════════════════════════════════════════════════════════════════════
// COORDINATE FOUNDATION (66 Books)
// ═══════════════════════════════════════════════════════════════════════════
//
// Scripture IS the coordinate map:
//   X-axis: 66 books, Genesis (X=-1) → Revelation (X=+1)
//   Y-axis: Chapters within book, normalized to [-1, +1]
//   Z-axis: Verse depth, ranges shift by perspective
//
// The book position IS the temporal position:
//   Genesis naturally lives in PAST region (X=-1)
//   Mid-Bible lives in PRESENT region (X≈0)
//   Revelation naturally lives in FUTURE region (X=+1)
//
// Traces: BIBLE-INDEX.txt B.1-B.6
//
// ═══════════════════════════════════════════════════════════════════════════

package coordinates

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"creativeworkzstudio.com/bereshit/word/work/pkg/foundation/schema"
)

// ───────────────────────────────────────────────────────────────────────────
// TYPES — Aliased from cpisi/schema (the authoritative source)
// ───────────────────────────────────────────────────────────────────────────
//
// All types are defined in cpisi/schema — THE VOCABULARY.
// coordinates imports and aliases them for local convenience.
// The rules (how types are used) live here; the shapes live in schema.
//
// Traces to: HALT_07 (MECHANISM_POSITION) via cpisi/schema

// Perspective is an alias to schema.Perspective — the authoritative source.
type Perspective = schema.Perspective

// Perspective constants — aliases to schema for local convenience.
const (
	PerspectivePast    = schema.PerspectivePast    // X=-1: Genesis = (-1,-1,-1), tracing TO
	PerspectivePresent = schema.PerspectivePresent // X=0: Genesis = (0,0,0), standing AT
	PerspectiveFuture  = schema.PerspectiveFuture  // X=+1: Genesis = (+1,+1,+1), building FROM
)

// IsReachable is an alias to schema.IsReachable for local convenience.
var IsReachable = schema.IsReachable

// IsCoordReachable is an alias to schema.IsCoordReachable for local convenience.
var IsCoordReachable = schema.IsCoordReachable

// BibleReference is an alias to schema.BibleReference.
type BibleReference = schema.BibleReference

// BibleCoords is an alias to schema.BibleCoords.
type BibleCoords = schema.BibleCoords

// BookInfo is an alias to schema.BookInfo.
type BookInfo = schema.BookInfo

// ───────────────────────────────────────────────────────────────────────────
// CONSTANTS — Aliased from cpisi/schema
// ───────────────────────────────────────────────────────────────────────────

// Hardpoint constants — aliased from schema.
const (
	HardpointMemoryReflection    = schema.HardpointMemoryReflection
	HardpointTransition          = schema.HardpointTransition
	HardpointPlanningPreparation = schema.HardpointPlanningPreparation
)

// Bible coordinate constants — aliased from schema.
const (
	TotalBooks    = schema.TotalBooks
	OldTestament  = schema.OldTestament
	NewTestament  = schema.NewTestament
	BookMid       = schema.BookMid
	BookScale     = schema.BookScale
	OriginBook    = schema.OriginBook
	OriginChapter = schema.OriginChapter
	OriginVerse   = schema.OriginVerse
)

// BibleIndex is the coordinate lookup system.
// Uses types from schema, applies rules (application logic) here.
type BibleIndex struct {
	Books         map[string]BookInfo
	BooksByIdx    map[int]BookInfo
	ScriptureRoot string
	Perspective   Perspective // Current perspective for coordinate calculations

	// Translation set tracking (verse-index output)
	TranslationSet TranslationSet // Which set is loaded (KJV, WEB, Parallel)
	CubeIndex      *CubeIndex     // O(1) verse lookup (optional, loaded separately)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Book Index (BIBLE-INDEX.txt B.1) ---

// NewBibleIndex creates the index with all 66 books
// Defaults to PRESENT perspective (Genesis = 0,0,0) - we ARE at the present
// Defaults to KJV translation set (31,102 verses)
func NewBibleIndex(scriptureRoot string) *BibleIndex {
	bi := &BibleIndex{
		Books:          make(map[string]BookInfo),
		BooksByIdx:     make(map[int]BookInfo),
		ScriptureRoot:  scriptureRoot,
		Perspective:    PerspectivePresent, // Default: PRESENT - I AM here
		TranslationSet: SetKJV,             // Default: KJV (31,102 verses)
	}

	// Old Testament (1-39)
	otBooks := []BookInfo{
		{Index: 1, Name: "Genesis", Chapters: 50, Category: "Law"},
		{Index: 2, Name: "Exodus", Chapters: 40, Category: "Law"},
		{Index: 3, Name: "Leviticus", Chapters: 27, Category: "Law"},
		{Index: 4, Name: "Numbers", Chapters: 36, Category: "Law"},
		{Index: 5, Name: "Deuteronomy", Chapters: 34, Category: "Law"},
		{Index: 6, Name: "Joshua", Chapters: 24, Category: "History"},
		{Index: 7, Name: "Judges", Chapters: 21, Category: "History"},
		{Index: 8, Name: "Ruth", Chapters: 4, Category: "History"},
		{Index: 9, Name: "1_Samuel", Chapters: 31, Category: "History"},
		{Index: 10, Name: "2_Samuel", Chapters: 24, Category: "History"},
		{Index: 11, Name: "1_Kings", Chapters: 22, Category: "History"},
		{Index: 12, Name: "2_Kings", Chapters: 25, Category: "History"},
		{Index: 13, Name: "1_Chronicles", Chapters: 29, Category: "History"},
		{Index: 14, Name: "2_Chronicles", Chapters: 36, Category: "History"},
		{Index: 15, Name: "Ezra", Chapters: 10, Category: "History"},
		{Index: 16, Name: "Nehemiah", Chapters: 13, Category: "History"},
		{Index: 17, Name: "Esther", Chapters: 10, Category: "History"},
		{Index: 18, Name: "Job", Chapters: 42, Category: "Wisdom"},
		{Index: 19, Name: "Psalms", Chapters: 150, Category: "Wisdom"},
		{Index: 20, Name: "Proverbs", Chapters: 31, Category: "Wisdom"},
		{Index: 21, Name: "Ecclesiastes", Chapters: 12, Category: "Wisdom"},
		{Index: 22, Name: "Song_of_Solomon", Chapters: 8, Category: "Wisdom"},
		{Index: 23, Name: "Isaiah", Chapters: 66, Category: "Major Prophet"},
		{Index: 24, Name: "Jeremiah", Chapters: 52, Category: "Major Prophet"},
		{Index: 25, Name: "Lamentations", Chapters: 5, Category: "Major Prophet"},
		{Index: 26, Name: "Ezekiel", Chapters: 48, Category: "Major Prophet"},
		{Index: 27, Name: "Daniel", Chapters: 12, Category: "Major Prophet"},
		{Index: 28, Name: "Hosea", Chapters: 14, Category: "Minor Prophet"},
		{Index: 29, Name: "Joel", Chapters: 3, Category: "Minor Prophet"},
		{Index: 30, Name: "Amos", Chapters: 9, Category: "Minor Prophet"},
		{Index: 31, Name: "Obadiah", Chapters: 1, Category: "Minor Prophet"},
		{Index: 32, Name: "Jonah", Chapters: 4, Category: "Minor Prophet"},
		{Index: 33, Name: "Micah", Chapters: 7, Category: "Minor Prophet"},
		{Index: 34, Name: "Nahum", Chapters: 3, Category: "Minor Prophet"},
		{Index: 35, Name: "Habakkuk", Chapters: 3, Category: "Minor Prophet"},
		{Index: 36, Name: "Zephaniah", Chapters: 3, Category: "Minor Prophet"},
		{Index: 37, Name: "Haggai", Chapters: 2, Category: "Minor Prophet"},
		{Index: 38, Name: "Zechariah", Chapters: 14, Category: "Minor Prophet"},
		{Index: 39, Name: "Malachi", Chapters: 4, Category: "Minor Prophet"},
	}

	// New Testament (40-66)
	ntBooks := []BookInfo{
		{Index: 40, Name: "Matthew", Chapters: 28, Category: "Gospel"},
		{Index: 41, Name: "Mark", Chapters: 16, Category: "Gospel"},
		{Index: 42, Name: "Luke", Chapters: 24, Category: "Gospel"},
		{Index: 43, Name: "John", Chapters: 21, Category: "Gospel"},
		{Index: 44, Name: "Acts", Chapters: 28, Category: "History"},
		{Index: 45, Name: "Romans", Chapters: 16, Category: "Pauline"},
		{Index: 46, Name: "1_Corinthians", Chapters: 16, Category: "Pauline"},
		{Index: 47, Name: "2_Corinthians", Chapters: 13, Category: "Pauline"},
		{Index: 48, Name: "Galatians", Chapters: 6, Category: "Pauline"},
		{Index: 49, Name: "Ephesians", Chapters: 6, Category: "Pauline"},
		{Index: 50, Name: "Philippians", Chapters: 4, Category: "Pauline"},
		{Index: 51, Name: "Colossians", Chapters: 4, Category: "Pauline"},
		{Index: 52, Name: "1_Thessalonians", Chapters: 5, Category: "Pauline"},
		{Index: 53, Name: "2_Thessalonians", Chapters: 3, Category: "Pauline"},
		{Index: 54, Name: "1_Timothy", Chapters: 6, Category: "Pastoral"},
		{Index: 55, Name: "2_Timothy", Chapters: 4, Category: "Pastoral"},
		{Index: 56, Name: "Titus", Chapters: 3, Category: "Pastoral"},
		{Index: 57, Name: "Philemon", Chapters: 1, Category: "Pauline"},
		{Index: 58, Name: "Hebrews", Chapters: 13, Category: "General"},
		{Index: 59, Name: "James", Chapters: 5, Category: "General"},
		{Index: 60, Name: "1_Peter", Chapters: 5, Category: "General"},
		{Index: 61, Name: "2_Peter", Chapters: 3, Category: "General"},
		{Index: 62, Name: "1_John", Chapters: 5, Category: "General"},
		{Index: 63, Name: "2_John", Chapters: 1, Category: "General"},
		{Index: 64, Name: "3_John", Chapters: 1, Category: "General"},
		{Index: 65, Name: "Jude", Chapters: 1, Category: "General"},
		{Index: 66, Name: "Revelation", Chapters: 22, Category: "Prophecy"},
	}

	// Register all books
	for _, b := range otBooks {
		bi.Books[b.Name] = b
		bi.BooksByIdx[b.Index] = b
	}
	for _, b := range ntBooks {
		bi.Books[b.Name] = b
		bi.BooksByIdx[b.Index] = b
	}

	return bi
}

// NewBibleIndexFromBereshit creates the index by loading from Bereshit TOMLs
// This is the preferred method - uses single source of truth from word/core/bible/
// Also loads the O(1) cube index for the specified translation set
func NewBibleIndexFromBereshit(scriptureRoot, bereshitDir string, set TranslationSet) (*BibleIndex, error) {
	// Try to load from Bereshit TOMLs
	bibleData, err := LoadBibleData(bereshitDir)
	if err != nil {
		return nil, fmt.Errorf("load Bereshit Bible data: %w", err)
	}

	bi := &BibleIndex{
		Books:          make(map[string]BookInfo),
		BooksByIdx:     make(map[int]BookInfo),
		ScriptureRoot:  scriptureRoot,
		Perspective:    PerspectivePresent,
		TranslationSet: set,
	}

	// Convert loaded data to BookInfo maps
	bi.Books, bi.BooksByIdx = bibleData.GetBooksAsBookInfo()

	// Load the cube index for O(1) verse lookup
	cubeIdx, err := LoadCubeIndex(bereshitDir, set)
	if err != nil {
		// Cube index is optional - warn but don't fail
		// The index may not be generated yet
		return bi, nil
	}
	bi.CubeIndex = cubeIdx

	return bi, nil
}

// NewBibleIndexWithFallback tries Bereshit loading first, falls back to hardcoded
// Defaults to KJV translation set
func NewBibleIndexWithFallback(scriptureRoot, bereshitDir string) *BibleIndex {
	return NewBibleIndexWithFallbackSet(scriptureRoot, bereshitDir, SetKJV)
}

// NewBibleIndexWithFallbackSet tries Bereshit loading first, falls back to hardcoded
// Allows specifying the translation set to load
func NewBibleIndexWithFallbackSet(scriptureRoot, bereshitDir string, set TranslationSet) *BibleIndex {
	bi, err := NewBibleIndexFromBereshit(scriptureRoot, bereshitDir, set)
	if err != nil {
		// Fallback to hardcoded data
		return NewBibleIndex(scriptureRoot)
	}
	return bi
}

// --- Perspective Management ---

// SetPerspective changes the coordinate perspective
func (bi *BibleIndex) SetPerspective(p Perspective) {
	bi.Perspective = p
}

// GetPerspective returns current perspective
func (bi *BibleIndex) GetPerspective() Perspective {
	return bi.Perspective
}

// --- Cube Index Access (O(1) Verse Lookup) ---

// HasCubeIndex returns true if the cube index is loaded
func (bi *BibleIndex) HasCubeIndex() bool {
	return bi.CubeIndex != nil
}

// LookupByOrdinal returns a verse by ordinal (1-31102 for KJV)
// Requires CubeIndex to be loaded
func (bi *BibleIndex) LookupByOrdinal(ordinal int) (*VerseEntry, bool) {
	if bi.CubeIndex == nil {
		return nil, false
	}
	return bi.CubeIndex.LookupByOrdinal(ordinal)
}

// LookupByCube returns a verse by state machine coordinates
// anchor: 0-8, cube: 0-26, high: 0-127
func (bi *BibleIndex) LookupByCube(anchor, cube, high int) (*VerseEntry, bool) {
	if bi.CubeIndex == nil {
		return nil, false
	}
	return bi.CubeIndex.LookupByCube(anchor, cube, high)
}

// GetTranslationSet returns the loaded translation set
func (bi *BibleIndex) GetTranslationSet() TranslationSet {
	return bi.TranslationSet
}

// --- Coordinate Mapping (BIBLE-INDEX.txt B.2) ---

// ToCoords converts a reference to normalized coordinates
// Uses the current perspective setting of the BibleIndex
//
// Base normalization (PAST perspective):
//
//	Genesis 1:1 (1,1,1) → (-1, -1, -1) = HALT origin
//	Revelation 22:21 (66,22,21) → (+1, ?, ?) = completion
//
// Perspective transform:
//
//	PAST:    Genesis = (-1, -1, -1)
//	PRESENT: Genesis = (0, 0, 0)
//	FUTURE:  Genesis = (+1, +1, +1)
//
// Formula (PAST perspective):
//
//	book_norm = (book - 1) / 32.5 - 1
//	chapter_norm = (chapter - 1) / (maxChapter/2) - 1
//	verse_norm = (verse - 1) / (maxVerse/2) - 1
func (bi *BibleIndex) ToCoords(ref BibleReference) (BibleCoords, error) {
	return bi.ToCoordsWithPerspective(ref, bi.Perspective)
}

// ToCoordsWithPerspective converts a reference using a specific perspective
//
// BALANCED TERNARY: All coordinates stay within [-1, +1]
//
// X (Book) and Y (Chapter) use standard normalization across all perspectives.
// Z (Verse) shifts based on TEMPORAL PERSPECTIVE:
//
//	PAST:    Z ∈ [-1, 0]     — looking at what came before
//	PRESENT: Z ∈ [-0.5, 0.5] — centered on now
//	FUTURE:  Z ∈ [0, 1]      — looking at what comes after
//
// This reflects the temporal nature of the Z axis:
//
//	The PAST is what you trace FROM (Z negative)
//	The PRESENT is where you stand (Z centered)
//	The FUTURE is what you build TOWARD (Z positive)
func (bi *BibleIndex) ToCoordsWithPerspective(ref BibleReference, perspective Perspective) (BibleCoords, error) {
	book, ok := bi.Books[ref.Book]
	if !ok {
		return BibleCoords{}, fmt.Errorf("unknown book: %s", ref.Book)
	}

	// X (Book): Standard normalization [-1, +1]
	// Formula: (book - 1) / 32.5 - 1
	// Genesis (1) → -1, Revelation (66) → +1
	bookNorm := (float64(book.Index)-1)/BookScale - 1.0

	// Y (Chapter): Standard normalization [-1, +1]
	// Normalized relative to book's chapter count
	maxCh := float64(book.Chapters)
	chapterNorm := (float64(ref.Chapter)-1)/(maxCh/2) - 1.0

	// Z (Verse): Perspective-shifted normalization
	// Base normalization puts verse in [0, 1] range
	verseBase := float64(ref.Verse-1) / 40.0 // [0, ~1]
	if verseBase > 1.0 {
		verseBase = 1.0
	}

	// Z offset derived from Z = (X + Y) / 2 formula
	// Uses ZOffset() which calculates ZMin for the perspective's Z range
	//   PAST:    ZOffset = -1.0  → Z ∈ [-1, 0]
	//   PRESENT: ZOffset = -0.5  → Z ∈ [-0.5, 0.5]
	//   FUTURE:  ZOffset = 0.0   → Z ∈ [0, 1]
	verseNorm := verseBase + perspective.ZOffset()

	// Clamp to balanced range [-1, +1]
	clamp := func(v float64) float64 {
		if v > 1.0 {
			return 1.0
		}
		if v < -1.0 {
			return -1.0
		}
		return v
	}

	return BibleCoords{
		BookNorm:    clamp(bookNorm),
		ChapterNorm: clamp(chapterNorm),
		VerseNorm:   clamp(verseNorm),
		Perspective: perspective,
	}, nil
}

// FromCoords converts normalized coordinates back to raw values
// Inverse of ToCoords formula, accounting for perspective
//
// X and Y use standard normalization (no perspective shift)
// Z shifts based on perspective:
//
//	PAST: Z ∈ [-1, 0]     → verseBase = Z + 1
//	PRESENT: Z ∈ [-0.5, 0.5] → verseBase = Z + 0.5
//	FUTURE: Z ∈ [0, 1]       → verseBase = Z
func (bi *BibleIndex) FromCoords(coords BibleCoords) (int, int, int) {
	// X (Book): Standard inverse
	// Formula: book = (bookNorm + 1) * BookScale + 1
	bookRaw := int((coords.BookNorm+1)*BookScale + 1)
	if bookRaw < 1 {
		bookRaw = 1
	}
	if bookRaw > TotalBooks {
		bookRaw = TotalBooks
	}

	// Y (Chapter): Standard inverse
	chapterRaw := int((coords.ChapterNorm + 1) * 25) // ~50/2
	if chapterRaw < 1 {
		chapterRaw = 1
	}

	// Z (Verse): Perspective-shifted inverse
	var verseBase float64
	switch coords.Perspective {
	case PerspectivePast:
		// Z ∈ [-1, 0] → verseBase = Z + 1
		verseBase = coords.VerseNorm + 1.0
	case PerspectivePresent:
		// Z ∈ [-0.5, 0.5] → verseBase = Z + 0.5
		verseBase = coords.VerseNorm + 0.5
	case PerspectiveFuture:
		// Z ∈ [0, 1] → verseBase = Z
		verseBase = coords.VerseNorm
	}

	verseRaw := int(verseBase*40) + 1
	if verseRaw < 1 {
		verseRaw = 1
	}

	return bookRaw, chapterRaw, verseRaw
}

// --- Path Operations (BIBLE-INDEX.txt B.3) ---

// GetVersePath returns the filesystem path for a verse
// Format: KJV/{Book}/Chapter_{N}/Verse_{N}.txt
func (bi *BibleIndex) GetVersePath(ref BibleReference) string {
	return filepath.Join(
		bi.ScriptureRoot,
		"KJV",
		ref.Book,
		fmt.Sprintf("Chapter_%d", ref.Chapter),
		fmt.Sprintf("Verse_%d.txt", ref.Verse),
	)
}

// ReadVerse reads the content of a verse file
func (bi *BibleIndex) ReadVerse(ref BibleReference) (string, error) {
	path := bi.GetVersePath(ref)
	content, err := os.ReadFile(path)
	if err != nil {
		return "", err
	}
	return strings.TrimSpace(string(content)), nil
}

// ParsePath extracts book/chapter/verse from a path
func (bi *BibleIndex) ParsePath(path string) (BibleReference, error) {
	// Expected: .../KJV/Genesis/Chapter_1/Verse_1.txt
	parts := strings.Split(path, string(filepath.Separator))

	var ref BibleReference
	for i, p := range parts {
		if p == "KJV" && i+3 < len(parts) {
			ref.Book = parts[i+1]

			// Parse Chapter_N
			chapterStr := strings.TrimPrefix(parts[i+2], "Chapter_")
			fmt.Sscanf(chapterStr, "%d", &ref.Chapter)

			// Parse Verse_N.txt
			verseStr := strings.TrimPrefix(parts[i+3], "Verse_")
			verseStr = strings.TrimSuffix(verseStr, ".txt")
			fmt.Sscanf(verseStr, "%d", &ref.Verse)

			return ref, nil
		}
	}

	return ref, fmt.Errorf("invalid path format: %s", path)
}

// --- Special References (BIBLE-INDEX.txt B.5) ---

// AnchorVerse represents a special anchor verse
type AnchorVerse struct {
	Ref     BibleReference
	RawX    int
	RawY    int
	RawZ    int
	Purpose string
}

// GetAnchorVerses returns the CPI-SI anchor verses
func GetAnchorVerses() []AnchorVerse {
	return []AnchorVerse{
		{
			Ref:  BibleReference{Book: "Genesis", Chapter: 1, Verse: 1},
			RawX: 1, RawY: 1, RawZ: 1,
			Purpose: "HALT origin, all traces here",
		},
		{
			Ref:  BibleReference{Book: "Exodus", Chapter: 3, Verse: 14},
			RawX: 2, RawY: 3, RawZ: 14,
			Purpose: "\"I AM\" - identity anchor",
		},
		{
			Ref:  BibleReference{Book: "Proverbs", Chapter: 9, Verse: 10},
			RawX: 20, RawY: 9, RawZ: 10,
			Purpose: "Fear of LORD = wisdom begin",
		},
		{
			Ref:  BibleReference{Book: "Matthew", Chapter: 7, Verse: 24},
			RawX: 40, RawY: 7, RawZ: 24,
			Purpose: "Build on rock (foundation)",
		},
		{
			Ref:  BibleReference{Book: "John", Chapter: 1, Verse: 1},
			RawX: 43, RawY: 1, RawZ: 1,
			Purpose: "Word was with God (creation)",
		},
		{
			Ref:  BibleReference{Book: "Colossians", Chapter: 4, Verse: 6},
			RawX: 51, RawY: 4, RawZ: 6,
			Purpose: "Speech with grace (communication)",
		},
		{
			Ref:  BibleReference{Book: "Hebrews", Chapter: 4, Verse: 12},
			RawX: 58, RawY: 4, RawZ: 12,
			Purpose: "Word is living (Scripture)",
		},
		{
			Ref:  BibleReference{Book: "Revelation", Chapter: 22, Verse: 21},
			RawX: 66, RawY: 22, RawZ: 21,
			Purpose: "Final verse (completion)",
		},
	}
}

// GetOrigin returns Genesis 1:1 - the HALT origin
func GetOrigin() BibleReference {
	return BibleReference{Book: "Genesis", Chapter: 1, Verse: 1}
}

// --- Lookup Helpers ---

// GetBook returns book info by name
func (bi *BibleIndex) GetBook(name string) (BookInfo, bool) {
	b, ok := bi.Books[name]
	return b, ok
}

// GetBookByIndex returns book info by index (1-66)
func (bi *BibleIndex) GetBookByIndex(idx int) (BookInfo, bool) {
	b, ok := bi.BooksByIdx[idx]
	return b, ok
}

// IsOldTestament returns true if book is OT (1-39)
func (bi *BibleIndex) IsOldTestament(book string) bool {
	b, ok := bi.Books[book]
	return ok && b.Index <= 39
}

// IsNewTestament returns true if book is NT (40-66)
func (bi *BibleIndex) IsNewTestament(book string) bool {
	b, ok := bi.Books[book]
	return ok && b.Index >= 40
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

// Bible coordinate system:
// - 66 books indexed 1-66 (OT 1-39, NT 40-66)
// - Normalized to [-1, +1] for cube mapping
// - Genesis 1:1 = PAST-PAST (-1,-1,-1) = trace source
// - Revelation 22:21 = final coordinate
// - Path format: KJV/{Book}/Chapter_{N}/Verse_{N}.txt
