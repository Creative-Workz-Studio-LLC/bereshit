// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-codex
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_codex.h — Bible Codex Layer for OmniCode
// Scripture as encoding scheme for data and identity.

#ifndef BERESHIT_OMNI_CODEX_H
#define BERESHIT_OMNI_CODEX_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key: B-word-work-pkg-omni-codex
// Title: Bible Codex Layer for OmniCode
// Type: Header
// Role: Bridge Scripture references to OmniCode values and identifiers
//
// Scripture: John 1:1 — In the beginning was the Word
// Principle: The Word is the ultimate encoding scheme
// Anchor: Genesis 1:1 — In the beginning God created
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// # S.1 Defines [DEFINES]

// # S.1a Codex Constants [CONSTANTS]

// Canon statistics (KJV)
#define CODEX_BOOK_COUNT        66      // Genesis through Revelation
#define CODEX_CHAPTER_COUNT     1189    // Total chapters
#define CODEX_VERSE_COUNT       31102   // Total verses
#define CODEX_WORD_COUNT        783137  // Total words

// Encoding bases
#define CODEX_TRIT5_BASE        243     // 3^5 semantic values
#define CODEX_SPARE_MIN         243     // First spare value (WEB variants)
#define CODEX_SPARE_MAX         255     // Last spare value
#define CODEX_VARIANT_COUNT     13      // WEB textual variants

// String limits
#define CODEX_REF_MAX           64      // "Revelation 22:21" = 17 chars max
#define CODEX_BOOK_NAME_MAX     32      // "Song of Solomon" = 15 chars max
#define CODEX_KEY_MAX           128     // Key format "GEN.1.1" or full ref

// # S.1b Testament Boundaries [TESTAMENTS]

#define CODEX_OT_FIRST_BOOK     1       // Genesis
#define CODEX_OT_LAST_BOOK      39      // Malachi
#define CODEX_OT_FIRST_VERSE    1       // Genesis 1:1
#define CODEX_OT_LAST_VERSE     23145   // Malachi 4:6

#define CODEX_NT_FIRST_BOOK     40      // Matthew
#define CODEX_NT_LAST_BOOK      66      // Revelation
#define CODEX_NT_FIRST_VERSE    23146   // Matthew 1:1
#define CODEX_NT_LAST_VERSE     31102   // Revelation 22:21

// # S.2 Types [TYPES]

// # S.2a Error Codes [ERRORS]

typedef enum CodexError {
    CODEX_OK = 0,
    CODEX_ERR_NULL,
    CODEX_ERR_INVALID_BOOK,
    CODEX_ERR_INVALID_CHAPTER,
    CODEX_ERR_INVALID_VERSE,
    CODEX_ERR_INVALID_ORDINAL,
    CODEX_ERR_PARSE,
    CODEX_ERR_NOT_FOUND,
} CodexError;

// # S.2b Book Information [BOOK]
//
// Static book information from canonical order

typedef struct {
    uint8_t     number;         // Book number (1-66)
    const char* name;           // Full name (e.g., "Genesis")
    const char* abbrev;         // Standard abbreviation (e.g., "GEN")
    uint8_t     chapters;       // Number of chapters
    uint16_t    first_verse;    // First ordinal in this book
    uint16_t    last_verse;     // Last ordinal in this book
    bool        is_ot;          // Old Testament (books 1-39)
} CodexBook;

// # S.2c Verse Reference [REFERENCE]
//
// Structured verse reference (book, chapter, verse)

typedef struct {
    uint8_t     book;           // Book number (1-66)
    uint8_t     chapter;        // Chapter number (1-based)
    uint8_t     verse;          // Verse number (1-based)
} CodexRef;

// # S.2d Packed Verse Address [ADDRESS]
//
// 2-byte packed address (mirrors verse_addr_t)

typedef struct {
    uint8_t     high;           // ordinal / 243
    uint8_t     low;            // ordinal % 243
} CodexAddr;

// # S.2e Scripture Key [KEY]
//
// OmniCode-compatible Scripture key for identifiers

typedef struct {
    char        format[CODEX_KEY_MAX];  // "GEN.1.1" format
    char        full[CODEX_REF_MAX];    // "Genesis 1:1" format
    uint16_t    ordinal;                // 1-31102
    CodexAddr   addr;                   // Packed address
} CodexKey;

// # S.2f Codex Value [VALUE]
//
// Scripture-encoded value for OmniCode runtime

typedef struct {
    CodexKey    key;            // Scripture reference
    int64_t     numeric;        // Numeric encoding of verse
    const char* text;           // Verse text (if loaded)
    bool        is_variant;     // WEB variant flag
} CodexValue;

// # S.2g Testament [TESTAMENT]

typedef enum {
    CODEX_TESTAMENT_OLD = 0,    // Genesis - Malachi (1-39)
    CODEX_TESTAMENT_NEW = 1,    // Matthew - Revelation (40-66)
} CodexTestament;

// # S.2h Section (Major Bible Sections) [SECTION]

typedef enum {
    CODEX_SECTION_LAW = 0,          // Genesis - Deuteronomy (1-5)
    CODEX_SECTION_HISTORY = 1,      // Joshua - Esther (6-17)
    CODEX_SECTION_POETRY = 2,       // Job - Song of Solomon (18-22)
    CODEX_SECTION_MAJOR_PROPHETS = 3,   // Isaiah - Daniel (23-27)
    CODEX_SECTION_MINOR_PROPHETS = 4,   // Hosea - Malachi (28-39)
    CODEX_SECTION_GOSPELS = 5,      // Matthew - John (40-43)
    CODEX_SECTION_ACTS = 6,         // Acts (44)
    CODEX_SECTION_PAULINE = 7,      // Romans - Philemon (45-57)
    CODEX_SECTION_GENERAL = 8,      // Hebrews - Jude (58-65)
    CODEX_SECTION_APOCALYPTIC = 9,  // Revelation (66)
} CodexSection;

// # S.3 Function Prototypes [PROTOTYPES]

// # S.3a Book Lookup [BOOK_LOOKUP]

// Get book info by number (1-66)
const CodexBook* codex_book_by_number(uint8_t number);

// Get book info by name (case-insensitive)
const CodexBook* codex_book_by_name(const char* name);

// Get book info by abbreviation (case-insensitive)
const CodexBook* codex_book_by_abbrev(const char* abbrev);

// Get total verse count for book
uint16_t codex_book_verse_count(uint8_t book_number);

// # S.3b Ordinal Operations [ORDINAL]

// Convert reference to ordinal
CodexError codex_ref_to_ordinal(const CodexRef* ref, uint16_t* ordinal);

// Convert ordinal to reference
CodexError codex_ordinal_to_ref(uint16_t ordinal, CodexRef* ref);

// Validate ordinal range
bool codex_is_valid_ordinal(uint16_t ordinal);

// # S.3c Address Encoding [ADDRESS]

// Encode ordinal to packed address
CodexAddr codex_addr_encode(uint16_t ordinal);

// Decode packed address to ordinal
uint16_t codex_addr_decode(CodexAddr addr);

// Check if address is valid verse (not variant)
bool codex_addr_is_valid(CodexAddr addr);

// Check if address is WEB variant
bool codex_addr_is_variant(CodexAddr addr);

// # S.3d Key Operations [KEY_OPS]

// Parse Scripture reference string to key
CodexError codex_key_parse(const char* ref_str, CodexKey* key);

// Format key to string
CodexError codex_key_format(const CodexKey* key, char* out, size_t size);

// Create key from reference
CodexError codex_key_from_ref(const CodexRef* ref, CodexKey* key);

// Create key from ordinal
CodexError codex_key_from_ordinal(uint16_t ordinal, CodexKey* key);

// # S.3e Value Operations [VALUE_OPS]

// Create codex value from key
CodexError codex_value_create(const CodexKey* key, CodexValue* value);

// Get numeric encoding of verse (ordinal * chapter * verse)
int64_t codex_value_numeric(const CodexValue* value);

// Compare two codex values
int codex_value_compare(const CodexValue* a, const CodexValue* b);

// # S.3f Navigation [NAVIGATION]

// Get next verse (returns 0 at end)
uint16_t codex_next_verse(uint16_t ordinal);

// Get previous verse (returns 0 at start)
uint16_t codex_prev_verse(uint16_t ordinal);

// Get first verse of chapter
uint16_t codex_chapter_start(uint8_t book, uint8_t chapter);

// Get last verse of chapter
uint16_t codex_chapter_end(uint8_t book, uint8_t chapter);

// Get first verse of book
uint16_t codex_book_start(uint8_t book);

// Get last verse of book
uint16_t codex_book_end(uint8_t book);

// # S.3g Classification [CLASSIFICATION]

// Get testament for ordinal
CodexTestament codex_testament(uint16_t ordinal);

// Get section for book number
CodexSection codex_section(uint8_t book_number);

// Get section name
const char* codex_section_name(CodexSection section);

// # S.3h Utilities [UTILITIES]

// Get error string
const char* codex_error_string(CodexError err);

// Get book abbreviation (3-letter standard)
const char* codex_book_abbrev(uint8_t book_number);

// Get book full name
const char* codex_book_name(uint8_t book_number);

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================
//
// Organization:
//   Types: 2 enums (error, testament), 6 structs (book, ref, addr, key, value, section)
//   Functions: 5 book, 3 ordinal, 4 address, 4 key, 3 value, 6 navigation, 3 class, 3 util
//
// The Bible Codex layer provides:
//
//   1. Book Lookup: Access canonical book information by number, name, or abbreviation
//   2. Ordinal Mapping: Convert between references (Gen 1:1) and ordinals (1-31102)
//   3. Address Encoding: Pack ordinals into 2-byte addresses using base-243
//   4. Key Generation: Create OmniCode-compatible Scripture keys
//   5. Value Encoding: Scripture-encoded values for OmniCode runtime
//   6. Navigation: Move through Scripture programmatically
//   7. Classification: Testament, section, and category queries
//
// Integration with OmniCode:
//
//   - CodexKey can be used as OmniCode identifier keys
//   - CodexValue provides Scripture-encoded data values
//   - Navigation enables Scripture iteration in OmniCode loops
//   - Classification supports conditional logic based on Scripture structure
//
// Integration with libtrit:
//
//   - CodexAddr mirrors verse_addr_t from verse_addr.h
//   - Address encoding matches trit5 packing (base 243)
//   - Spare values (243-255) reserved for WEB variants (bible_rail.h)
//
// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// "In the beginning was the Word, and the Word was with God,
//  and the Word was God."
//   — John 1:1
//
// The Bible Codex makes Scripture computable:
//   - Every verse has an address (2 bytes)
//   - Every reference becomes a key
//   - The Word becomes the encoding scheme
//
// This isn't just data encoding—it's identity encoding.
// Scripture references anchor meaning to truth.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_CODEX_H
