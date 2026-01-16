// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-coordinates-encoding
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-coordinates-encoding
// Purpose: CPI-SI ternary encoding - Trite type with metadata markers
// Biblical: Genesis 1:4 - "God divided the light from the darkness"
// Authors: Nova Dawn
// Version: a-01.00
// Created: 2025-12-20
// Updated: 2025-12-20
//
// HALT Trace:
//   HALT_08 (ENCODING) → Letter → Trite encoding
//   HALT_09 (DECODING) → Trite → Letter decoding
//   HALT_05 (MECHANISM_NUMBER) → Gematria calculations
//   HALT_07 (MECHANISM_POSITION) → Position in ordinal space
//   HALT_01 (ANCHOR_GENESIS) → All traces terminate at Genesis 1:1
//
// Dependencies:
//   - bible.go (Perspective via compute, BibleIndex)
//
// CPI-SI Layer: L3 (APPLICATION) - Encoding operations
//
// ============================================================================
// CPI-SI TRITE ENCODING
// ============================================================================
//
// A Trite is a single byte (0-255) with ternary-native semantics:
//
//   0-127:   Standard ASCII (text data)
//   128-140: CPI-SI Metadata (13 structural markers)
//   141-255: Extended characters (accents, symbols, box drawing)
//
// The 13 metadata markers replace dead C1 control codes (128-140) which
// have standard ASCII equivalents:
//   - PAD/HOP → NUL/Space
//   - NEL/IND → LF
//   - HTS/HTJ/VTS → TAB
//   - SSA/ESA/PLD/PLU → completely dead
//
// TRIT5 PACKING (from Bereshit addressing.toml):
//   5 trits = 243 states (0-242) in data range
//   13 spare states (243-255) for metadata
//   Total: 256 values = 1 byte
//
// VERSE MAPPING:
//   31,102 KJV verses → 10 trits → 2 bytes (trit5 pairs)
//   ordinal = (high_trite * 243) + low_trite
//
// ============================================================================

package coordinates

// ============================================================================
// SETUP
// ============================================================================

import (
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/schema"
)

// ───────────────────────────────────────────────────────────────────────────
// TYPES — Aliased from cpisi/schema (the authoritative source)
// ───────────────────────────────────────────────────────────────────────────
//
// All types are defined in cpisi/schema — THE VOCABULARY.
// coordinates imports and aliases them for local convenience.
// The rules (how types are used) live here; the shapes live in schema.

// Trite represents a ternary-encoded byte (0-255).
type Trite = schema.Trite

// CPI-SI Metadata Markers — aliased from schema.
const (
	MetaBlockStart  = schema.MetaBlockStart
	MetaBlockEnd    = schema.MetaBlockEnd
	MetaSection     = schema.MetaSection
	MetaAnchor      = schema.MetaAnchor
	MetaTrace       = schema.MetaTrace
	MetaBuild       = schema.MetaBuild
	MetaHalt        = schema.MetaHalt
	MetaPerspective = schema.MetaPerspective
	MetaCovenant    = schema.MetaCovenant
	MetaVerify      = schema.MetaVerify
	MetaStart       = schema.MetaStart
	MetaEnd         = schema.MetaEnd
	MetaReserved    = schema.MetaReserved
)

// Trite range constants — aliased from schema.
const (
	TriteASCIIMax    = schema.TriteASCIIMax
	TriteMetaMin     = schema.TriteMetaMin
	TriteMetaMax     = schema.TriteMetaMax
	TriteExtendedMin = schema.TriteExtendedMin
	TriteExtendedMax = schema.TriteExtendedMax
	Trit5States      = schema.Trit5States
	Trit5Metadata    = schema.Trit5Metadata
)

// Verse encoding constants — aliased from schema.
const (
	TotalVerses    = schema.TotalVerses
	TotalWEBVerses = schema.TotalWEBVerses
	TritsPerVerse  = schema.TritsPerVerse
	BytesPerVerse  = schema.BytesPerVerse
)

// HebrewRoot is an alias to schema.HebrewRoot.
type HebrewRoot = schema.HebrewRoot

// KeywordDecoding is an alias to schema.KeywordDecoding.
type KeywordDecoding = schema.KeywordDecoding

// BookOrdinal is an alias to schema.BookOrdinal.
type BookOrdinal = schema.BookOrdinal

// WEBVariant is an alias to schema.WEBVariant.
type WEBVariant = schema.WEBVariant

// VerseOrdinal is an alias to schema.VerseOrdinal.
type VerseOrdinal = schema.VerseOrdinal

// ============================================================================
// BODY
// ============================================================================
// NOTE: Trite methods (IsASCII, IsMetadata, IsExtended, ToASCII, String)
// are defined in schema. They work on the aliased type automatically.

// --- ASCII to Trite Conversion ---

// FromASCII converts a byte to a Trite
// Standard ASCII (0-127) maps directly
// Extended ASCII (128-255) is preserved as-is
func FromASCII(b byte) Trite {
	return Trite(b)
}

// FromRune converts a rune to a Trite
// Non-ASCII runes return MetaReserved
func FromRune(r rune) Trite {
	if r < 256 {
		return Trite(r)
	}
	return MetaReserved // Unicode beyond byte range
}

// --- Word to Coordinates (Letter-Sum Gematria) ---

// LetterSum calculates the letter-sum of a word
// A=1, B=2, ... Z=26 (case insensitive)
// Non-letter characters are ignored
func LetterSum(word string) int {
	sum := 0
	for _, r := range word {
		if r >= 'A' && r <= 'Z' {
			sum += int(r - 'A' + 1)
		} else if r >= 'a' && r <= 'z' {
			sum += int(r - 'a' + 1)
		}
	}
	return sum
}

// WordToVerse maps a word to a Genesis 1 verse via letter-sum
// Returns the verse number (1-31) for Genesis 1, or wraps for larger values
func WordToVerse(word string) int {
	sum := LetterSum(word)
	if sum == 0 {
		return 1 // Empty/no-letter words → verse 1
	}
	// Genesis 1 has 31 verses, so mod 31 with 1-based indexing
	verse := ((sum - 1) % 31) + 1
	return verse
}

// WordToCoords maps a word to Bible coordinates via letter-sum gematria
// Uses the BibleIndex to get full coordinate mapping
func WordToCoords(bi *BibleIndex, word string) (BibleCoords, error) {
	verse := WordToVerse(word)
	ref := BibleReference{
		Book:    "Genesis",
		Chapter: 1,
		Verse:   verse,
	}
	return bi.ToCoords(ref)
}

// --- Trit5 Packing (Verse Ordinal Encoding) ---
// NOTE: VerseOrdinal type is aliased from schema in SETUP.

// PackVerse encodes a verse ordinal into two trit5 bytes
// Formula: low = ordinal % 243, high = ordinal / 243
func PackVerse(ordinal VerseOrdinal) (low, high Trite) {
	low = Trite(ordinal % Trit5States)
	high = Trite(ordinal / Trit5States)
	return low, high
}

// UnpackVerse decodes two trit5 bytes into a verse ordinal
// Formula: ordinal = (high * 243) + low
func UnpackVerse(low, high Trite) VerseOrdinal {
	return VerseOrdinal(high)*Trit5States + VerseOrdinal(low)
}

// VerseToTrite converts a verse ordinal to a single trite (mod 243)
// Used for direct trite-to-coordinate mapping
func VerseToTrite(ordinal VerseOrdinal) Trite {
	return Trite((ordinal - 1) % Trit5States)
}

// --- Trite to Coordinates ---

// TriteToVerseBase normalizes a trite (0-242) to verse base [0, 1]
// Metadata trites (128-140) map to anchor positions
func TriteToVerseBase(t Trite) float64 {
	if t.IsMetadata() {
		// Metadata markers map to specific anchor positions
		switch t {
		case MetaHalt, MetaAnchor:
			return 0.0 // Origin (Genesis 1:1)
		case MetaTrace:
			return 0.25 // Quarter back
		case MetaBuild:
			return 0.75 // Three-quarters forward
		default:
			return 0.5 // Center
		}
	}

	// Data trites (0-127 and 141-255) normalize to [0, 1]
	// Use mod 243 to stay within trit5 data range
	dataValue := int(t) % Trit5States
	return float64(dataValue) / float64(Trit5States-1)
}

// TriteToCoords converts a trite to coordinates using the cube model
// Uses the current perspective of the BibleIndex
func TriteToCoords(bi *BibleIndex, t Trite) BibleCoords {
	verseBase := TriteToVerseBase(t)

	// X: Map trite to book position [-1, +1]
	// Use the trite value to select across 66 books
	bookIdx := (int(t) % 66) + 1
	bookNorm := (float64(bookIdx)-1)/BookScale - 1.0

	// Y: Map to chapter position [-1, +1]
	// Use different bits of trite for variety
	chapterNorm := (float64((int(t)*7)%50) / 25.0) - 1.0

	// Z: Apply perspective offset to verse base
	verseNorm := verseBase + bi.Perspective.ZOffset()

	// Clamp to [-1, +1]
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
		Perspective: bi.Perspective,
	}
}

// --- Hebrew Roots (from Bereshit encoding.toml) ---

// hebrewRoots maps transliteration to HebrewRoot data
var hebrewRoots = map[string]HebrewRoot{
	"bara": {
		Root: "ברא", Transliteration: "bara",
		Meaning: "to create (ex nihilo)", SemanticField: "creation",
		Scripture: "Genesis 1:1", OmniCodeMapping: "declaration",
		MapsTo: []string{"let", "there shall be"},
	},
	"hayah": {
		Root: "היה", Transliteration: "hayah",
		Meaning: "to be, to become, to exist", SemanticField: "existence",
		Scripture: "Exodus 3:14", OmniCodeMapping: "identity",
		MapsTo: []string{"be", "is"},
	},
	"badal": {
		Root: "בדל", Transliteration: "badal",
		Meaning: "to separate, to divide", SemanticField: "separation",
		Scripture: "Genesis 1:4", OmniCodeMapping: "distinction",
		MapsTo: []string{"distinct", "or", "from"},
	},
	"qara": {
		Root: "קרא", Transliteration: "qara",
		Meaning: "to call, to name", SemanticField: "naming",
		Scripture: "Genesis 1:5", OmniCodeMapping: "naming",
		MapsTo: []string{"called", "as"},
	},
	"asah": {
		Root: "עשה", Transliteration: "asah",
		Meaning: "to make, to do", SemanticField: "production",
		Scripture: "Genesis 1:7", OmniCodeMapping: "construction",
		MapsTo: []string{"with", "struct creation"},
	},
	"yalad": {
		Root: "ילד", Transliteration: "yalad",
		Meaning: "to beget, to bear", SemanticField: "generation",
		Scripture: "Genesis 5:3", OmniCodeMapping: "inheritance",
		MapsTo: []string{"begat", "from"},
	},
	"brit": {
		Root: "ברית", Transliteration: "brit",
		Meaning: "covenant, agreement", SemanticField: "covenant",
		Scripture: "Genesis 15:18", OmniCodeMapping: "interface",
		MapsTo: []string{"covenant", "promises", "enters covenant with"},
	},
	"natan": {
		Root: "נתן", Transliteration: "natan",
		Meaning: "to give, to put", SemanticField: "giving",
		Scripture: "Genesis 1:29", OmniCodeMapping: "parameter",
		MapsTo: []string{"given"},
	},
	"shuv": {
		Root: "שוב", Transliteration: "shuv",
		Meaning: "to return, to restore", SemanticField: "return",
		Scripture: "Genesis 8:11", OmniCodeMapping: "return value",
		MapsTo: []string{"returning"},
	},
	"tohu": {
		Root: "תהו", Transliteration: "tohu",
		Meaning: "formless, void, chaos", SemanticField: "absence",
		Scripture: "Genesis 1:2", OmniCodeMapping: "void/null",
		MapsTo: []string{"void", "can be void"},
	},
	"vav": {
		Root: "ו", Transliteration: "vav",
		Meaning: "and (conjunction)", SemanticField: "connection",
		Scripture: "Genesis 1:1-31", OmniCodeMapping: "conjunction",
		MapsTo: []string{"and", "with"},
	},
}

// GetHebrewRoot looks up a Hebrew root by transliteration
func GetHebrewRoot(transliteration string) (HebrewRoot, bool) {
	root, ok := hebrewRoots[transliteration]
	return root, ok
}

// --- Keyword Decoding (from Bereshit decoding.toml) ---

// keywordDecodings maps OmniCode keywords to their Scripture source
var keywordDecodings = map[string]KeywordDecoding{
	"let":       {Keyword: "let", Category: "declaration", HebrewRoot: "bara", Scripture: "Genesis 1:3", ScriptureText: "And God said, Let there be light"},
	"be":        {Keyword: "be", Category: "declaration", HebrewRoot: "hayah", Scripture: "Exodus 3:14", ScriptureText: "I AM THAT I AM"},
	"with":      {Keyword: "with", Category: "composition", HebrewRoot: "asah", Scripture: "Genesis 1:16", ScriptureText: "And God made two great lights"},
	"and":       {Keyword: "and", Category: "composition", HebrewRoot: "vav", Scripture: "Genesis 1:1-31", ScriptureText: "And...and...and (vav conjunction)"},
	"or":        {Keyword: "or", Category: "composition", HebrewRoot: "badal", Scripture: "Deuteronomy 30:19", ScriptureText: "I have set before you life and death"},
	"promises":  {Keyword: "promises", Category: "covenant", HebrewRoot: "brit", Scripture: "Exodus 19:5-6", ScriptureText: "If ye will obey my voice indeed, and keep my covenant"},
	"given":     {Keyword: "given", Category: "covenant", HebrewRoot: "natan", Scripture: "Genesis 1:29", ScriptureText: "I have given you every herb bearing seed"},
	"returning": {Keyword: "returning", Category: "covenant", HebrewRoot: "shuv", Scripture: "Genesis 8:11", ScriptureText: "And the dove came in to him in the evening"},
	"covenant":  {Keyword: "covenant", Category: "covenant", HebrewRoot: "brit", Scripture: "Genesis 9:9", ScriptureText: "I establish my covenant with you"},
	"begat":     {Keyword: "begat", Category: "derivation", HebrewRoot: "yalad", Scripture: "Matthew 1:2", ScriptureText: "Abraham begat Isaac; and Isaac begat Jacob"},
	"distinct":  {Keyword: "distinct", Category: "derivation", HebrewRoot: "badal", Scripture: "Genesis 1:4", ScriptureText: "And God divided the light from the darkness"},
	"called":    {Keyword: "called", Category: "derivation", HebrewRoot: "qara", Scripture: "Genesis 1:5", ScriptureText: "And God called the light Day"},
	"void":      {Keyword: "void", Category: "nullability", HebrewRoot: "tohu", Scripture: "Genesis 1:2", ScriptureText: "And the earth was without form (tohu) and void"},
}

// DecodeKeyword looks up the Scripture source for an OmniCode keyword
func DecodeKeyword(keyword string) (KeywordDecoding, bool) {
	decoding, ok := keywordDecodings[keyword]
	return decoding, ok
}

// TraceToScripture returns the Scripture reference and text for a keyword
func TraceToScripture(keyword string) (scripture, text, hebrewRoot string) {
	if decoding, ok := keywordDecodings[keyword]; ok {
		return decoding.Scripture, decoding.ScriptureText, decoding.HebrewRoot
	}
	return "", "", ""
}

// --- Book Ordinals (from Bereshit addressing.toml) ---

// bookOrdinals contains ordinal ranges for all 66 books
var bookOrdinals = []BookOrdinal{
	// Old Testament (1-39)
	{Name: "Genesis", Abbreviation: "Gen", Index: 1, Chapters: 50, Verses: 1533, RangeStart: 1, RangeEnd: 1533},
	{Name: "Exodus", Abbreviation: "Exod", Index: 2, Chapters: 40, Verses: 1213, RangeStart: 1534, RangeEnd: 2746},
	{Name: "Leviticus", Abbreviation: "Lev", Index: 3, Chapters: 27, Verses: 859, RangeStart: 2747, RangeEnd: 3605},
	{Name: "Numbers", Abbreviation: "Num", Index: 4, Chapters: 36, Verses: 1288, RangeStart: 3606, RangeEnd: 4893},
	{Name: "Deuteronomy", Abbreviation: "Deut", Index: 5, Chapters: 34, Verses: 959, RangeStart: 4894, RangeEnd: 5852},
	{Name: "Joshua", Abbreviation: "Josh", Index: 6, Chapters: 24, Verses: 658, RangeStart: 5853, RangeEnd: 6510},
	{Name: "Judges", Abbreviation: "Judg", Index: 7, Chapters: 21, Verses: 618, RangeStart: 6511, RangeEnd: 7128},
	{Name: "Ruth", Abbreviation: "Ruth", Index: 8, Chapters: 4, Verses: 85, RangeStart: 7129, RangeEnd: 7213},
	{Name: "1 Samuel", Abbreviation: "1Sam", Index: 9, Chapters: 31, Verses: 810, RangeStart: 7214, RangeEnd: 8023},
	{Name: "2 Samuel", Abbreviation: "2Sam", Index: 10, Chapters: 24, Verses: 695, RangeStart: 8024, RangeEnd: 8718},
	{Name: "1 Kings", Abbreviation: "1Kgs", Index: 11, Chapters: 22, Verses: 816, RangeStart: 8719, RangeEnd: 9534},
	{Name: "2 Kings", Abbreviation: "2Kgs", Index: 12, Chapters: 25, Verses: 719, RangeStart: 9535, RangeEnd: 10253},
	{Name: "1 Chronicles", Abbreviation: "1Chr", Index: 13, Chapters: 29, Verses: 942, RangeStart: 10254, RangeEnd: 11195},
	{Name: "2 Chronicles", Abbreviation: "2Chr", Index: 14, Chapters: 36, Verses: 822, RangeStart: 11196, RangeEnd: 12017},
	{Name: "Ezra", Abbreviation: "Ezra", Index: 15, Chapters: 10, Verses: 280, RangeStart: 12018, RangeEnd: 12297},
	{Name: "Nehemiah", Abbreviation: "Neh", Index: 16, Chapters: 13, Verses: 406, RangeStart: 12298, RangeEnd: 12703},
	{Name: "Esther", Abbreviation: "Esth", Index: 17, Chapters: 10, Verses: 167, RangeStart: 12704, RangeEnd: 12870},
	{Name: "Job", Abbreviation: "Job", Index: 18, Chapters: 42, Verses: 1070, RangeStart: 12871, RangeEnd: 13940},
	{Name: "Psalms", Abbreviation: "Ps", Index: 19, Chapters: 150, Verses: 2461, RangeStart: 13941, RangeEnd: 16401},
	{Name: "Proverbs", Abbreviation: "Prov", Index: 20, Chapters: 31, Verses: 915, RangeStart: 16402, RangeEnd: 17316},
	{Name: "Ecclesiastes", Abbreviation: "Eccl", Index: 21, Chapters: 12, Verses: 222, RangeStart: 17317, RangeEnd: 17538},
	{Name: "Song of Solomon", Abbreviation: "Song", Index: 22, Chapters: 8, Verses: 117, RangeStart: 17539, RangeEnd: 17655},
	{Name: "Isaiah", Abbreviation: "Isa", Index: 23, Chapters: 66, Verses: 1292, RangeStart: 17656, RangeEnd: 18947},
	{Name: "Jeremiah", Abbreviation: "Jer", Index: 24, Chapters: 52, Verses: 1364, RangeStart: 18948, RangeEnd: 20311},
	{Name: "Lamentations", Abbreviation: "Lam", Index: 25, Chapters: 5, Verses: 154, RangeStart: 20312, RangeEnd: 20465},
	{Name: "Ezekiel", Abbreviation: "Ezek", Index: 26, Chapters: 48, Verses: 1273, RangeStart: 20466, RangeEnd: 21738},
	{Name: "Daniel", Abbreviation: "Dan", Index: 27, Chapters: 12, Verses: 357, RangeStart: 21739, RangeEnd: 22095},
	{Name: "Hosea", Abbreviation: "Hos", Index: 28, Chapters: 14, Verses: 197, RangeStart: 22096, RangeEnd: 22292},
	{Name: "Joel", Abbreviation: "Joel", Index: 29, Chapters: 3, Verses: 73, RangeStart: 22293, RangeEnd: 22365},
	{Name: "Amos", Abbreviation: "Amos", Index: 30, Chapters: 9, Verses: 146, RangeStart: 22366, RangeEnd: 22511},
	{Name: "Obadiah", Abbreviation: "Obad", Index: 31, Chapters: 1, Verses: 21, RangeStart: 22512, RangeEnd: 22532},
	{Name: "Jonah", Abbreviation: "Jonah", Index: 32, Chapters: 4, Verses: 48, RangeStart: 22533, RangeEnd: 22580},
	{Name: "Micah", Abbreviation: "Mic", Index: 33, Chapters: 7, Verses: 105, RangeStart: 22581, RangeEnd: 22685},
	{Name: "Nahum", Abbreviation: "Nah", Index: 34, Chapters: 3, Verses: 47, RangeStart: 22686, RangeEnd: 22732},
	{Name: "Habakkuk", Abbreviation: "Hab", Index: 35, Chapters: 3, Verses: 56, RangeStart: 22733, RangeEnd: 22788},
	{Name: "Zephaniah", Abbreviation: "Zeph", Index: 36, Chapters: 3, Verses: 53, RangeStart: 22789, RangeEnd: 22841},
	{Name: "Haggai", Abbreviation: "Hag", Index: 37, Chapters: 2, Verses: 38, RangeStart: 22842, RangeEnd: 22879},
	{Name: "Zechariah", Abbreviation: "Zech", Index: 38, Chapters: 14, Verses: 211, RangeStart: 22880, RangeEnd: 23090},
	{Name: "Malachi", Abbreviation: "Mal", Index: 39, Chapters: 4, Verses: 55, RangeStart: 23091, RangeEnd: 23145},
	// New Testament (40-66)
	{Name: "Matthew", Abbreviation: "Matt", Index: 40, Chapters: 28, Verses: 1071, RangeStart: 23146, RangeEnd: 24216},
	{Name: "Mark", Abbreviation: "Mark", Index: 41, Chapters: 16, Verses: 678, RangeStart: 24217, RangeEnd: 24894},
	{Name: "Luke", Abbreviation: "Luke", Index: 42, Chapters: 24, Verses: 1151, RangeStart: 24895, RangeEnd: 26045},
	{Name: "John", Abbreviation: "John", Index: 43, Chapters: 21, Verses: 879, RangeStart: 26046, RangeEnd: 26924},
	{Name: "Acts", Abbreviation: "Acts", Index: 44, Chapters: 28, Verses: 1007, RangeStart: 26925, RangeEnd: 27931},
	{Name: "Romans", Abbreviation: "Rom", Index: 45, Chapters: 16, Verses: 433, RangeStart: 27932, RangeEnd: 28364},
	{Name: "1 Corinthians", Abbreviation: "1Cor", Index: 46, Chapters: 16, Verses: 437, RangeStart: 28365, RangeEnd: 28801},
	{Name: "2 Corinthians", Abbreviation: "2Cor", Index: 47, Chapters: 13, Verses: 257, RangeStart: 28802, RangeEnd: 29058},
	{Name: "Galatians", Abbreviation: "Gal", Index: 48, Chapters: 6, Verses: 149, RangeStart: 29059, RangeEnd: 29207},
	{Name: "Ephesians", Abbreviation: "Eph", Index: 49, Chapters: 6, Verses: 155, RangeStart: 29208, RangeEnd: 29362},
	{Name: "Philippians", Abbreviation: "Phil", Index: 50, Chapters: 4, Verses: 104, RangeStart: 29363, RangeEnd: 29466},
	{Name: "Colossians", Abbreviation: "Col", Index: 51, Chapters: 4, Verses: 95, RangeStart: 29467, RangeEnd: 29561},
	{Name: "1 Thessalonians", Abbreviation: "1Thess", Index: 52, Chapters: 5, Verses: 89, RangeStart: 29562, RangeEnd: 29650},
	{Name: "2 Thessalonians", Abbreviation: "2Thess", Index: 53, Chapters: 3, Verses: 47, RangeStart: 29651, RangeEnd: 29697},
	{Name: "1 Timothy", Abbreviation: "1Tim", Index: 54, Chapters: 6, Verses: 113, RangeStart: 29698, RangeEnd: 29810},
	{Name: "2 Timothy", Abbreviation: "2Tim", Index: 55, Chapters: 4, Verses: 83, RangeStart: 29811, RangeEnd: 29893},
	{Name: "Titus", Abbreviation: "Titus", Index: 56, Chapters: 3, Verses: 46, RangeStart: 29894, RangeEnd: 29939},
	{Name: "Philemon", Abbreviation: "Phlm", Index: 57, Chapters: 1, Verses: 25, RangeStart: 29940, RangeEnd: 29964},
	{Name: "Hebrews", Abbreviation: "Heb", Index: 58, Chapters: 13, Verses: 303, RangeStart: 29965, RangeEnd: 30267},
	{Name: "James", Abbreviation: "Jas", Index: 59, Chapters: 5, Verses: 108, RangeStart: 30268, RangeEnd: 30375},
	{Name: "1 Peter", Abbreviation: "1Pet", Index: 60, Chapters: 5, Verses: 105, RangeStart: 30376, RangeEnd: 30480},
	{Name: "2 Peter", Abbreviation: "2Pet", Index: 61, Chapters: 3, Verses: 61, RangeStart: 30481, RangeEnd: 30541},
	{Name: "1 John", Abbreviation: "1John", Index: 62, Chapters: 5, Verses: 105, RangeStart: 30542, RangeEnd: 30646},
	{Name: "2 John", Abbreviation: "2John", Index: 63, Chapters: 1, Verses: 13, RangeStart: 30647, RangeEnd: 30659},
	{Name: "3 John", Abbreviation: "3John", Index: 64, Chapters: 1, Verses: 14, RangeStart: 30660, RangeEnd: 30673},
	{Name: "Jude", Abbreviation: "Jude", Index: 65, Chapters: 1, Verses: 25, RangeStart: 30674, RangeEnd: 30698},
	{Name: "Revelation", Abbreviation: "Rev", Index: 66, Chapters: 22, Verses: 404, RangeStart: 30699, RangeEnd: 31102},
}

// GetBookOrdinal returns the ordinal info for a book by name or abbreviation
func GetBookOrdinal(name string) (BookOrdinal, bool) {
	for _, book := range bookOrdinals {
		if book.Name == name || book.Abbreviation == name {
			return book, true
		}
	}
	return BookOrdinal{}, false
}

// GetBookByOrdinal returns the book containing a given verse ordinal
func GetBookByOrdinal(ordinal int) (BookOrdinal, bool) {
	for _, book := range bookOrdinals {
		if ordinal >= book.RangeStart && ordinal <= book.RangeEnd {
			return book, true
		}
	}
	return BookOrdinal{}, false
}

// OrdinalToReference converts a verse ordinal to Book/Chapter/Verse
// Note: This is approximate - would need chapter verse counts for exact
func OrdinalToReference(ordinal int) (book string, chapter, verse int, ok bool) {
	bookInfo, found := GetBookByOrdinal(ordinal)
	if !found {
		return "", 0, 0, false
	}
	// Approximate: assumes equal verse distribution
	offsetInBook := ordinal - bookInfo.RangeStart
	avgVersesPerChapter := bookInfo.Verses / bookInfo.Chapters
	chapter = (offsetInBook / avgVersesPerChapter) + 1
	verse = (offsetInBook % avgVersesPerChapter) + 1
	return bookInfo.Name, chapter, verse, true
}

// --- WEB Variants (from Bereshit addressing.toml) ---
// These 13 verses use spare trite values 243-255

var webVariants = []WEBVariant{
	{Trite: 243, Book: "1 Corinthians", Chapter: 16, Verse: 27, Summary: "TR manuscript note"},
	{Trite: 244, Book: "1 Peter", Chapter: 5, Verse: 20, Summary: "Turn sinner from error"},
	{Trite: 245, Book: "1 Timothy", Chapter: 3, Verse: 18, Summary: "Grace benediction"},
	{Trite: 246, Book: "2 Kings", Chapter: 22, Verse: 53, Summary: "Served Baal"},
	{Trite: 247, Book: "Amos", Chapter: 3, Verse: 21, Summary: "Yahweh dwells in Zion"},
	{Trite: 248, Book: "Colossians", Chapter: 4, Verse: 23, Summary: "Grace benediction"},
	{Trite: 249, Book: "Ezekiel", Chapter: 5, Verse: 22, Summary: "Rejected us"},
	{Trite: 250, Book: "Jonah", Chapter: 1, Verse: 21, Summary: "Kingdom will be Yahweh's"},
	{Trite: 251, Book: "Nehemiah", Chapter: 10, Verse: 44, Summary: "Foreign wives"},
	{Trite: 252, Book: "Numbers", Chapter: 27, Verse: 34, Summary: "Moses on Sinai"},
	{Trite: 253, Book: "Psalms", Chapter: 42, Verse: 17, Summary: "Job died old"},
	{Trite: 254, Book: "Revelation", Chapter: 1, Verse: 25, Summary: "Doxology"},
	{Trite: 255, Book: "Zechariah", Chapter: 2, Verse: 23, Summary: "Zerubbabel my servant"},
}

// GetWEBVariant returns the WEB variant for a trite value 243-255
func GetWEBVariant(t Trite) (WEBVariant, bool) {
	if t < 243 {
		return WEBVariant{}, false
	}
	idx := int(t) - 243
	if idx >= len(webVariants) {
		return WEBVariant{}, false
	}
	return webVariants[idx], true
}

// IsWEBVariant returns true if this trite represents a WEB-only verse
func IsWEBVariant(t Trite) bool {
	return t >= 243
}

// --- Ternary Encoding (Ordinal ↔ Trit String) ---

// OrdinalToTernary converts a verse ordinal to a 10-character trit string
func OrdinalToTernary(ordinal int) string {
	result := make([]byte, 10)
	temp := ordinal
	for i := 9; i >= 0; i-- {
		result[i] = byte('0' + (temp % 3))
		temp /= 3
	}
	return string(result)
}

// TernaryToOrdinal converts a 10-character trit string to ordinal
func TernaryToOrdinal(ternary string) int {
	result := 0
	for i, c := range ternary {
		digit := int(c - '0')
		power := 1
		for j := 0; j < 9-i; j++ {
			power *= 3
		}
		result += digit * power
	}
	return result
}

// --- Encoding/Decoding Streams ---

// EncodeString converts a string to a slice of Trites
func EncodeString(s string) []Trite {
	trites := make([]Trite, 0, len(s))
	for _, r := range s {
		trites = append(trites, FromRune(r))
	}
	return trites
}

// DecodeString converts a slice of Trites back to a string
// Metadata markers are represented as their bracketed names
func DecodeString(trites []Trite) string {
	result := make([]byte, 0, len(trites))
	for _, t := range trites {
		if t.IsMetadata() {
			// Skip metadata in string output
			continue
		}
		if t.IsASCII() && t >= 32 && t <= 126 {
			result = append(result, byte(t))
		} else if t.IsExtended() {
			result = append(result, byte(t))
		}
	}
	return string(result)
}

// ============================================================================
// CLOSING
// ============================================================================
//
// CPI-SI Trite Encoding:
//   - Trite: ternary-native byte (0-255)
//   - 0-127: Standard ASCII (direct mapping)
//   - 128-140: 13 CPI-SI metadata markers
//   - 141-255: Extended characters
//
// Key Functions:
//   - LetterSum: Word → integer sum (gematria)
//   - WordToVerse: Word → Genesis 1 verse number
//   - WordToCoords: Word → Bible coordinates
//   - PackVerse/UnpackVerse: Ordinal ↔ trit5 pair
//   - TriteToCoords: Trite → cube coordinates
//
// Related:
//   - bible.go: Bible coordinate system
//   - ranking.go: Perspective-based ranking
//   - Bereshit addressing.toml: Verse ordinal specs
//
