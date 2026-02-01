// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-omnicode-bible
// Title:       MillenniumOS Bible Codex Access
// Type:        Header
// Component:   Stage 1 (Kernel) / OmniCode Subsystem
// Role:        Programmatic access to Bible as foundation codex
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   2 Timothy 3:16 — "All scripture is given by inspiration of God"
//              The Bible is THE source truth, accessible via code.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_OMNICODE_BIBLE_H
#define MILLENNIUMOS_OMNICODE_BIBLE_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Bible Constants
// -----------------------------------------------------------------------------

// Book counts (ternary-aligned where meaningful)
#define MOS_BIBLE_BOOK_COUNT        66      // Total books
#define MOS_BIBLE_OT_COUNT          39      // Old Testament
#define MOS_BIBLE_NT_COUNT          27      // New Testament (3^3!)
#define MOS_BIBLE_CHAPTERS          1189    // Total chapters
#define MOS_BIBLE_VERSES            31102   // Total verses (KJV)

// Max sizes
#define MOS_BIBLE_MAX_REF           64      // Max reference string
#define MOS_BIBLE_MAX_VERSE         2048    // Max verse text
#define MOS_BIBLE_MAX_PASSAGE       16384   // Max passage text
#define MOS_BIBLE_MAX_SEARCH        256     // Max search query

// Testament (ternary)
#define MOS_BIBLE_TESTAMENT_OT      TRIT_NEG    // -1: Old Testament
#define MOS_BIBLE_TESTAMENT_BOTH    TRIT_ZERO   // 0: Both
#define MOS_BIBLE_TESTAMENT_NT      TRIT_POS    // +1: New Testament

// Book categories (trit5 for extended categorization)
#define MOS_BIBLE_CAT_LAW           ((trit5_t)0)    // Genesis-Deuteronomy
#define MOS_BIBLE_CAT_HISTORY       ((trit5_t)27)   // Joshua-Esther
#define MOS_BIBLE_CAT_POETRY        ((trit5_t)54)   // Job-Song of Solomon
#define MOS_BIBLE_CAT_PROPHETS      ((trit5_t)81)   // Isaiah-Malachi
#define MOS_BIBLE_CAT_GOSPELS       ((trit5_t)108)  // Matthew-John
#define MOS_BIBLE_CAT_ACTS          ((trit5_t)135)  // Acts
#define MOS_BIBLE_CAT_EPISTLES      ((trit5_t)162)  // Romans-Jude
#define MOS_BIBLE_CAT_REVELATION    ((trit5_t)189)  // Revelation

// -----------------------------------------------------------------------------
// B.2 Book Enumeration
// -----------------------------------------------------------------------------

typedef enum {
    // Old Testament - Law (0-4)
    MOS_BOOK_GENESIS = 0,
    MOS_BOOK_EXODUS,
    MOS_BOOK_LEVITICUS,
    MOS_BOOK_NUMBERS,
    MOS_BOOK_DEUTERONOMY,

    // Old Testament - History (5-16)
    MOS_BOOK_JOSHUA,
    MOS_BOOK_JUDGES,
    MOS_BOOK_RUTH,
    MOS_BOOK_1SAMUEL,
    MOS_BOOK_2SAMUEL,
    MOS_BOOK_1KINGS,
    MOS_BOOK_2KINGS,
    MOS_BOOK_1CHRONICLES,
    MOS_BOOK_2CHRONICLES,
    MOS_BOOK_EZRA,
    MOS_BOOK_NEHEMIAH,
    MOS_BOOK_ESTHER,

    // Old Testament - Poetry (17-21)
    MOS_BOOK_JOB,
    MOS_BOOK_PSALMS,
    MOS_BOOK_PROVERBS,
    MOS_BOOK_ECCLESIASTES,
    MOS_BOOK_SONG_OF_SOLOMON,

    // Old Testament - Major Prophets (22-26)
    MOS_BOOK_ISAIAH,
    MOS_BOOK_JEREMIAH,
    MOS_BOOK_LAMENTATIONS,
    MOS_BOOK_EZEKIEL,
    MOS_BOOK_DANIEL,

    // Old Testament - Minor Prophets (27-38)
    MOS_BOOK_HOSEA,
    MOS_BOOK_JOEL,
    MOS_BOOK_AMOS,
    MOS_BOOK_OBADIAH,
    MOS_BOOK_JONAH,
    MOS_BOOK_MICAH,
    MOS_BOOK_NAHUM,
    MOS_BOOK_HABAKKUK,
    MOS_BOOK_ZEPHANIAH,
    MOS_BOOK_HAGGAI,
    MOS_BOOK_ZECHARIAH,
    MOS_BOOK_MALACHI,

    // New Testament - Gospels (39-42)
    MOS_BOOK_MATTHEW,
    MOS_BOOK_MARK,
    MOS_BOOK_LUKE,
    MOS_BOOK_JOHN,

    // New Testament - History (43)
    MOS_BOOK_ACTS,

    // New Testament - Pauline Epistles (44-56)
    MOS_BOOK_ROMANS,
    MOS_BOOK_1CORINTHIANS,
    MOS_BOOK_2CORINTHIANS,
    MOS_BOOK_GALATIANS,
    MOS_BOOK_EPHESIANS,
    MOS_BOOK_PHILIPPIANS,
    MOS_BOOK_COLOSSIANS,
    MOS_BOOK_1THESSALONIANS,
    MOS_BOOK_2THESSALONIANS,
    MOS_BOOK_1TIMOTHY,
    MOS_BOOK_2TIMOTHY,
    MOS_BOOK_TITUS,
    MOS_BOOK_PHILEMON,

    // New Testament - General Epistles (57-64)
    MOS_BOOK_HEBREWS,
    MOS_BOOK_JAMES,
    MOS_BOOK_1PETER,
    MOS_BOOK_2PETER,
    MOS_BOOK_1JOHN,
    MOS_BOOK_2JOHN,
    MOS_BOOK_3JOHN,
    MOS_BOOK_JUDE,

    // New Testament - Prophecy (65)
    MOS_BOOK_REVELATION,

    MOS_BOOK_COUNT = 66
} MosBibleBook;

// -----------------------------------------------------------------------------
// B.3 Reference Structures
// -----------------------------------------------------------------------------

// Bible reference
typedef struct {
    MosBibleBook    book;
    uint8_t         chapter;
    uint8_t         verse_start;
    uint8_t         verse_end;      // For ranges (0 = single verse)
} MosBibleRef;

// Book information
typedef struct {
    MosBibleBook    id;
    const char*     name;           // "Genesis"
    const char*     abbrev;         // "Gen"
    trit_t          testament;      // OT/NT
    trit5_t         category;
    uint8_t         chapters;       // Number of chapters
} MosBibleBookInfo;

// Verse structure
typedef struct {
    MosBibleRef     ref;
    const char*     text;
    mos_size_t      length;
} MosBibleVerse;

// Search result
typedef struct {
    MosBibleRef     ref;
    const char*     text;
    mos_size_t      match_start;    // Position of match in text
    mos_size_t      match_length;
} MosBibleSearchResult;

// Bible codex state
typedef struct {
    // Loaded state
    bool            loaded;
    const char*     translation;    // "KJV"
    const char*     base_path;      // Path to Bible data

    // Statistics
    trit27_t        lookups;
    trit27_t        searches;
    trit27_t        cache_hits;

    // Health (is the codex accessible?)
    mos_health_t    health;
} MosBibleCodex;

// -----------------------------------------------------------------------------
// B.4 Initialization
// -----------------------------------------------------------------------------

// Initialize Bible codex
mos_result_t mos_bible_init(void);

// Initialize with specific path
mos_result_t mos_bible_init_path(const char* base_path);

// Shutdown Bible codex
void mos_bible_shutdown(void);

// Get codex state
const MosBibleCodex* mos_bible_info(void);

// -----------------------------------------------------------------------------
// B.5 Reference Parsing
// -----------------------------------------------------------------------------

// Parse reference string (e.g., "Gen 1:1", "John 3:16", "Ps 23:1-6")
mos_result_t mos_bible_parse_ref(const char* str, MosBibleRef* ref);

// Format reference to string
mos_result_t mos_bible_format_ref(const MosBibleRef* ref, char* str,
                                   mos_size_t size);

// Validate reference
bool mos_bible_ref_valid(const MosBibleRef* ref);

// -----------------------------------------------------------------------------
// B.6 Verse Lookup
// -----------------------------------------------------------------------------

// Get single verse
mos_result_t mos_bible_get_verse(const MosBibleRef* ref, MosBibleVerse* verse);

// Get verse by string reference
mos_result_t mos_bible_lookup(const char* ref_str, char* text, mos_size_t size);

// Get passage (range of verses)
mos_result_t mos_bible_get_passage(const MosBibleRef* ref, char* text,
                                    mos_size_t size);

// Get chapter
mos_result_t mos_bible_get_chapter(MosBibleBook book, uint8_t chapter,
                                    char* text, mos_size_t size);

// -----------------------------------------------------------------------------
// B.7 Book Information
// -----------------------------------------------------------------------------

// Get book info
const MosBibleBookInfo* mos_bible_book_info(MosBibleBook book);

// Find book by name (fuzzy matching)
MosBibleBook mos_bible_find_book(const char* name);

// Get book name
const char* mos_bible_book_name(MosBibleBook book);

// Get book abbreviation
const char* mos_bible_book_abbrev(MosBibleBook book);

// Get chapter count for book
uint8_t mos_bible_chapter_count(MosBibleBook book);

// Get verse count for chapter
uint8_t mos_bible_verse_count(MosBibleBook book, uint8_t chapter);

// -----------------------------------------------------------------------------
// B.8 Search
// -----------------------------------------------------------------------------

// Search for text in Bible
mos_result_t mos_bible_search(const char* query, MosBibleSearchResult* results,
                               mos_size_t max_results, mos_size_t* found);

// Search with options
mos_result_t mos_bible_search_ex(const char* query, trit_t testament,
                                  MosBibleBook book_filter,
                                  MosBibleSearchResult* results,
                                  mos_size_t max_results, mos_size_t* found);

// Search for exact phrase
mos_result_t mos_bible_search_phrase(const char* phrase,
                                      MosBibleSearchResult* results,
                                      mos_size_t max_results, mos_size_t* found);

// -----------------------------------------------------------------------------
// B.9 Random and Daily
// -----------------------------------------------------------------------------

// Get random verse
mos_result_t mos_bible_random_verse(MosBibleVerse* verse);

// Get daily verse (deterministic based on date)
mos_result_t mos_bible_daily_verse(MosBibleVerse* verse);

// Get verse relevant to health state
mos_result_t mos_bible_health_verse(mos_health_t health, MosBibleVerse* verse);

// Get verse relevant to Hebrew state
mos_result_t mos_bible_state_verse(MosHebrewState state, MosBibleVerse* verse);

// -----------------------------------------------------------------------------
// B.10 Navigation
// -----------------------------------------------------------------------------

// Get next verse
mos_result_t mos_bible_next_verse(const MosBibleRef* current, MosBibleRef* next);

// Get previous verse
mos_result_t mos_bible_prev_verse(const MosBibleRef* current, MosBibleRef* prev);

// Get next chapter
mos_result_t mos_bible_next_chapter(const MosBibleRef* current, MosBibleRef* next);

// Get previous chapter
mos_result_t mos_bible_prev_chapter(const MosBibleRef* current, MosBibleRef* prev);

// -----------------------------------------------------------------------------
// B.11 OmniCode Integration
// -----------------------------------------------------------------------------

// Look up verse via OmniCode expression
// Usage in OmniCode: bible("John 3:16")
mos_result_t mos_bible_omni_lookup(const char* expr, char* result,
                                    mos_size_t size);

// Get grounding verse for code context
// Suggests a relevant verse based on what the code does
mos_result_t mos_bible_grounding(const char* context, MosBibleVerse* verse);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Bible Codex Access for MillenniumOS:
//
// OmniCode programs can reference Scripture directly:
//
//   let verse = bible("Gen 1:1")
//   print(verse)  // "In the beginning God created..."
//
//   let results = bible.search("wisdom")
//   for r in results {
//       print(r.ref, ":", r.text)
//   }
//
// Health-based verse selection:
//
//   When health is low (shavar/chaser), comfort verses appear.
//   When health is high (tov/shalem), encouragement continues.
//
// Grounding integration:
//
//   Every OmniCode file can have biblical grounding in its pragma.
//   The runtime can suggest verses based on what the code does.
//
// "All scripture is given by inspiration of God, and is profitable
//  for doctrine, for reproof, for correction, for instruction
//  in righteousness."
// — 2 Timothy 3:16
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // MILLENNIUMOS_OMNICODE_BIBLE_H
