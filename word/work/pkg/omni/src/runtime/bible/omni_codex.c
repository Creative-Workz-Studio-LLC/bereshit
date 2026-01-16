// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-codex-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_codex.c — Bible Codex Layer Implementation
// Scripture as encoding scheme for data and identity.

// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key: B-word-work-pkg-omni-codex-impl
// Title: Bible Codex Layer Implementation
// Type: Source
// Role: Bridge Scripture references to OmniCode values and identifiers
//
// Scripture: John 1:1 — In the beginning was the Word
// Principle: The Word is the ultimate encoding scheme
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  // For strcasecmp
#include <ctype.h>
#include "omni_codex.h"

// # S.1 Book Data Table [BOOK_TABLE]
//
// Canonical book data: number, name, abbreviation, chapters, first/last ordinal
// Generated from word/scripture/kjv-ordinal-index.csv

static const CodexBook g_books[66] = {
    // Old Testament - Law (1-5)
    { 1, "Genesis", "GEN", 50, 1, 1533, true },
    { 2, "Exodus", "EXO", 40, 1534, 2746, true },
    { 3, "Leviticus", "LEV", 27, 2747, 3605, true },
    { 4, "Numbers", "NUM", 36, 3606, 4893, true },
    { 5, "Deuteronomy", "DEU", 34, 4894, 5852, true },

    // Old Testament - History (6-17)
    { 6, "Joshua", "JOS", 24, 5853, 6510, true },
    { 7, "Judges", "JDG", 21, 6511, 7128, true },
    { 8, "Ruth", "RUT", 4, 7129, 7213, true },
    { 9, "1 Samuel", "1SA", 31, 7214, 8023, true },
    {10, "2 Samuel", "2SA", 24, 8024, 8718, true },
    {11, "1 Kings", "1KI", 22, 8719, 9534, true },
    {12, "2 Kings", "2KI", 25, 9535, 10253, true },
    {13, "1 Chronicles", "1CH", 29, 10254, 11195, true },
    {14, "2 Chronicles", "2CH", 36, 11196, 12017, true },
    {15, "Ezra", "EZR", 10, 12018, 12297, true },
    {16, "Nehemiah", "NEH", 13, 12298, 12703, true },
    {17, "Esther", "EST", 10, 12704, 12870, true },

    // Old Testament - Poetry (18-22)
    {18, "Job", "JOB", 42, 12871, 13940, true },
    {19, "Psalms", "PSA", 150, 13941, 16401, true },
    {20, "Proverbs", "PRO", 31, 16402, 17316, true },
    {21, "Ecclesiastes", "ECC", 12, 17317, 17538, true },
    {22, "Song of Solomon", "SNG", 8, 17539, 17655, true },

    // Old Testament - Major Prophets (23-27)
    {23, "Isaiah", "ISA", 66, 17656, 18947, true },
    {24, "Jeremiah", "JER", 52, 18948, 20311, true },
    {25, "Lamentations", "LAM", 5, 20312, 20465, true },
    {26, "Ezekiel", "EZK", 48, 20466, 21738, true },
    {27, "Daniel", "DAN", 12, 21739, 22095, true },

    // Old Testament - Minor Prophets (28-39)
    {28, "Hosea", "HOS", 14, 22096, 22292, true },
    {29, "Joel", "JOL", 3, 22293, 22365, true },
    {30, "Amos", "AMO", 9, 22366, 22511, true },
    {31, "Obadiah", "OBA", 1, 22512, 22532, true },
    {32, "Jonah", "JON", 4, 22533, 22580, true },
    {33, "Micah", "MIC", 7, 22581, 22685, true },
    {34, "Nahum", "NAM", 3, 22686, 22732, true },
    {35, "Habakkuk", "HAB", 3, 22733, 22788, true },
    {36, "Zephaniah", "ZEP", 3, 22789, 22841, true },
    {37, "Haggai", "HAG", 2, 22842, 22879, true },
    {38, "Zechariah", "ZEC", 14, 22880, 23090, true },
    {39, "Malachi", "MAL", 4, 23091, 23145, true },

    // New Testament - Gospels (40-43)
    {40, "Matthew", "MAT", 28, 23146, 24216, false },
    {41, "Mark", "MRK", 16, 24217, 24894, false },
    {42, "Luke", "LUK", 24, 24895, 26045, false },
    {43, "John", "JHN", 21, 26046, 26924, false },

    // New Testament - Acts (44)
    {44, "Acts", "ACT", 28, 26925, 27931, false },

    // New Testament - Pauline Epistles (45-57)
    {45, "Romans", "ROM", 16, 27932, 28364, false },
    {46, "1 Corinthians", "1CO", 16, 28365, 28801, false },
    {47, "2 Corinthians", "2CO", 13, 28802, 29058, false },
    {48, "Galatians", "GAL", 6, 29059, 29207, false },
    {49, "Ephesians", "EPH", 6, 29208, 29362, false },
    {50, "Philippians", "PHP", 4, 29363, 29466, false },
    {51, "Colossians", "COL", 4, 29467, 29561, false },
    {52, "1 Thessalonians", "1TH", 5, 29562, 29650, false },
    {53, "2 Thessalonians", "2TH", 3, 29651, 29697, false },
    {54, "1 Timothy", "1TI", 6, 29698, 29810, false },
    {55, "2 Timothy", "2TI", 4, 29811, 29893, false },
    {56, "Titus", "TIT", 3, 29894, 29939, false },
    {57, "Philemon", "PHM", 1, 29940, 29964, false },

    // New Testament - General Epistles (58-65)
    {58, "Hebrews", "HEB", 13, 29965, 30267, false },
    {59, "James", "JAS", 5, 30268, 30375, false },
    {60, "1 Peter", "1PE", 5, 30376, 30480, false },
    {61, "2 Peter", "2PE", 3, 30481, 30541, false },
    {62, "1 John", "1JN", 5, 30542, 30646, false },
    {63, "2 John", "2JN", 1, 30647, 30659, false },
    {64, "3 John", "3JN", 1, 30660, 30673, false },
    {65, "Jude", "JUD", 1, 30674, 30698, false },

    // New Testament - Apocalyptic (66)
    {66, "Revelation", "REV", 22, 30699, 31102, false },
};

// Section names
static const char* g_section_names[] = {
    "Law",
    "History",
    "Poetry",
    "Major Prophets",
    "Minor Prophets",
    "Gospels",
    "Acts",
    "Pauline Epistles",
    "General Epistles",
    "Apocalyptic",
};

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Book Lookup [BOOK_LOOKUP]

const CodexBook* codex_book_by_number(uint8_t number) {
    if (number < 1 || number > 66) return NULL;
    return &g_books[number - 1];
}

const CodexBook* codex_book_by_name(const char* name) {
    if (!name) return NULL;

    for (int i = 0; i < 66; i++) {
        // Case-insensitive compare
        if (strcasecmp(g_books[i].name, name) == 0) {
            return &g_books[i];
        }
    }
    return NULL;
}

const CodexBook* codex_book_by_abbrev(const char* abbrev) {
    if (!abbrev) return NULL;

    for (int i = 0; i < 66; i++) {
        if (strcasecmp(g_books[i].abbrev, abbrev) == 0) {
            return &g_books[i];
        }
    }
    return NULL;
}

uint16_t codex_book_verse_count(uint8_t book_number) {
    const CodexBook* book = codex_book_by_number(book_number);
    if (!book) return 0;
    return book->last_verse - book->first_verse + 1;
}

// # B.2 Ordinal Operations [ORDINAL]

CodexError codex_ref_to_ordinal(const CodexRef* ref, uint16_t* ordinal) {
    if (!ref || !ordinal) return CODEX_ERR_NULL;

    const CodexBook* book = codex_book_by_number(ref->book);
    if (!book) return CODEX_ERR_INVALID_BOOK;

    if (ref->chapter < 1 || ref->chapter > book->chapters) {
        return CODEX_ERR_INVALID_CHAPTER;
    }

    // Simple linear search through verses
    // In production, would use chapter offset table
    uint16_t ord = book->first_verse;

    // Approximate: assume average verses per chapter
    // This is a simplification - full implementation would use verse count tables
    uint16_t verses_per_chapter = codex_book_verse_count(ref->book) / book->chapters;
    ord += (ref->chapter - 1) * verses_per_chapter;
    ord += ref->verse - 1;

    if (ord > book->last_verse) {
        ord = book->last_verse;  // Clamp
    }

    *ordinal = ord;
    return CODEX_OK;
}

CodexError codex_ordinal_to_ref(uint16_t ordinal, CodexRef* ref) {
    if (!ref) return CODEX_ERR_NULL;
    if (!codex_is_valid_ordinal(ordinal)) return CODEX_ERR_INVALID_ORDINAL;

    // Find book
    for (int i = 0; i < 66; i++) {
        if (ordinal >= g_books[i].first_verse && ordinal <= g_books[i].last_verse) {
            ref->book = g_books[i].number;

            // Approximate chapter/verse
            // In production, would use verse count tables
            uint16_t offset = ordinal - g_books[i].first_verse;
            uint16_t verses_per_chapter = codex_book_verse_count(g_books[i].number) / g_books[i].chapters;

            ref->chapter = (offset / verses_per_chapter) + 1;
            if (ref->chapter > g_books[i].chapters) {
                ref->chapter = g_books[i].chapters;
            }

            ref->verse = (offset % verses_per_chapter) + 1;
            return CODEX_OK;
        }
    }

    return CODEX_ERR_NOT_FOUND;
}

bool codex_is_valid_ordinal(uint16_t ordinal) {
    return ordinal >= 1 && ordinal <= CODEX_VERSE_COUNT;
}

// # B.3 Address Encoding [ADDRESS]

CodexAddr codex_addr_encode(uint16_t ordinal) {
    CodexAddr addr = {0, 0};
    if (!codex_is_valid_ordinal(ordinal)) return addr;

    addr.high = ordinal / CODEX_TRIT5_BASE;
    addr.low = ordinal % CODEX_TRIT5_BASE;
    return addr;
}

uint16_t codex_addr_decode(CodexAddr addr) {
    if (addr.low >= CODEX_SPARE_MIN) return 0;  // Variant, not verse
    return (addr.high * CODEX_TRIT5_BASE) + addr.low;
}

bool codex_addr_is_valid(CodexAddr addr) {
    return addr.low < CODEX_SPARE_MIN;
}

bool codex_addr_is_variant(CodexAddr addr) {
    // addr.low is uint8_t (0-255), so if >= 243 it's in the variant range
    // No need to check <= 255 as uint8_t cannot exceed that
    return addr.low >= CODEX_SPARE_MIN;
}

// # B.4 Key Operations [KEY_OPS]

CodexError codex_key_parse(const char* ref_str, CodexKey* key) {
    if (!ref_str || !key) return CODEX_ERR_NULL;

    memset(key, 0, sizeof(CodexKey));

    // Try to parse "GEN.1.1" format
    char book_abbrev[8] = {0};
    int chapter = 0, verse = 0;

    if (sscanf(ref_str, "%7[^.].%d.%d", book_abbrev, &chapter, &verse) == 3) {
        const CodexBook* book = codex_book_by_abbrev(book_abbrev);
        if (book) {
            CodexRef ref = { book->number, chapter, verse };
            return codex_key_from_ref(&ref, key);
        }
    }

    // Try to parse "Genesis 1:1" format
    char book_name[32] = {0};
    if (sscanf(ref_str, "%31[^0-9] %d:%d", book_name, &chapter, &verse) == 3) {
        // Trim trailing space from book name
        size_t len = strlen(book_name);
        while (len > 0 && book_name[len-1] == ' ') {
            book_name[--len] = '\0';
        }

        const CodexBook* book = codex_book_by_name(book_name);
        if (book) {
            CodexRef ref = { book->number, chapter, verse };
            return codex_key_from_ref(&ref, key);
        }
    }

    return CODEX_ERR_PARSE;
}

CodexError codex_key_format(const CodexKey* key, char* out, size_t size) {
    if (!key || !out || size == 0) return CODEX_ERR_NULL;

    snprintf(out, size, "%s", key->full);
    return CODEX_OK;
}

CodexError codex_key_from_ref(const CodexRef* ref, CodexKey* key) {
    if (!ref || !key) return CODEX_ERR_NULL;

    const CodexBook* book = codex_book_by_number(ref->book);
    if (!book) return CODEX_ERR_INVALID_BOOK;

    memset(key, 0, sizeof(CodexKey));

    // Build format string "GEN.1.1"
    snprintf(key->format, sizeof(key->format), "%s.%d.%d",
             book->abbrev, ref->chapter, ref->verse);

    // Build full string "Genesis 1:1"
    snprintf(key->full, sizeof(key->full), "%s %d:%d",
             book->name, ref->chapter, ref->verse);

    // Calculate ordinal
    CodexError err = codex_ref_to_ordinal(ref, &key->ordinal);
    if (err != CODEX_OK) return err;

    // Encode address
    key->addr = codex_addr_encode(key->ordinal);

    return CODEX_OK;
}

CodexError codex_key_from_ordinal(uint16_t ordinal, CodexKey* key) {
    if (!key) return CODEX_ERR_NULL;
    if (!codex_is_valid_ordinal(ordinal)) return CODEX_ERR_INVALID_ORDINAL;

    CodexRef ref;
    CodexError err = codex_ordinal_to_ref(ordinal, &ref);
    if (err != CODEX_OK) return err;

    return codex_key_from_ref(&ref, key);
}

// # B.5 Value Operations [VALUE_OPS]

CodexError codex_value_create(const CodexKey* key, CodexValue* value) {
    if (!key || !value) return CODEX_ERR_NULL;

    memset(value, 0, sizeof(CodexValue));
    value->key = *key;
    value->numeric = codex_value_numeric(value);
    value->text = NULL;
    value->is_variant = false;

    return CODEX_OK;
}

int64_t codex_value_numeric(const CodexValue* value) {
    if (!value) return 0;

    // Numeric encoding: ordinal combined with structural information
    // Format: (book * 1000000) + (chapter * 1000) + verse
    CodexRef ref;
    if (codex_ordinal_to_ref(value->key.ordinal, &ref) != CODEX_OK) {
        return value->key.ordinal;  // Fallback to just ordinal
    }

    return (ref.book * 1000000) + (ref.chapter * 1000) + ref.verse;
}

int codex_value_compare(const CodexValue* a, const CodexValue* b) {
    if (!a || !b) return 0;
    if (a->key.ordinal < b->key.ordinal) return -1;
    if (a->key.ordinal > b->key.ordinal) return 1;
    return 0;
}

// # B.6 Navigation [NAVIGATION]

uint16_t codex_next_verse(uint16_t ordinal) {
    if (ordinal >= CODEX_VERSE_COUNT) return 0;
    return ordinal + 1;
}

uint16_t codex_prev_verse(uint16_t ordinal) {
    if (ordinal <= 1) return 0;
    return ordinal - 1;
}

uint16_t codex_chapter_start(uint8_t book, uint8_t chapter) {
    const CodexBook* bk = codex_book_by_number(book);
    if (!bk || chapter < 1 || chapter > bk->chapters) return 0;

    // Approximate
    uint16_t verses_per_chapter = codex_book_verse_count(book) / bk->chapters;
    return bk->first_verse + ((chapter - 1) * verses_per_chapter);
}

uint16_t codex_chapter_end(uint8_t book, uint8_t chapter) {
    const CodexBook* bk = codex_book_by_number(book);
    if (!bk || chapter < 1 || chapter > bk->chapters) return 0;

    // Approximate
    uint16_t verses_per_chapter = codex_book_verse_count(book) / bk->chapters;
    uint16_t end = bk->first_verse + (chapter * verses_per_chapter) - 1;

    if (end > bk->last_verse) end = bk->last_verse;
    return end;
}

uint16_t codex_book_start(uint8_t book) {
    const CodexBook* bk = codex_book_by_number(book);
    return bk ? bk->first_verse : 0;
}

uint16_t codex_book_end(uint8_t book) {
    const CodexBook* bk = codex_book_by_number(book);
    return bk ? bk->last_verse : 0;
}

// # B.7 Classification [CLASSIFICATION]

CodexTestament codex_testament(uint16_t ordinal) {
    if (ordinal <= CODEX_OT_LAST_VERSE) return CODEX_TESTAMENT_OLD;
    return CODEX_TESTAMENT_NEW;
}

CodexSection codex_section(uint8_t book_number) {
    if (book_number >= 1 && book_number <= 5) return CODEX_SECTION_LAW;
    if (book_number >= 6 && book_number <= 17) return CODEX_SECTION_HISTORY;
    if (book_number >= 18 && book_number <= 22) return CODEX_SECTION_POETRY;
    if (book_number >= 23 && book_number <= 27) return CODEX_SECTION_MAJOR_PROPHETS;
    if (book_number >= 28 && book_number <= 39) return CODEX_SECTION_MINOR_PROPHETS;
    if (book_number >= 40 && book_number <= 43) return CODEX_SECTION_GOSPELS;
    if (book_number == 44) return CODEX_SECTION_ACTS;
    if (book_number >= 45 && book_number <= 57) return CODEX_SECTION_PAULINE;
    if (book_number >= 58 && book_number <= 65) return CODEX_SECTION_GENERAL;
    if (book_number == 66) return CODEX_SECTION_APOCALYPTIC;
    return CODEX_SECTION_LAW;  // Default
}

const char* codex_section_name(CodexSection section) {
    if (section < 0 || section > 9) return "Unknown";
    return g_section_names[section];
}

// # B.8 Utilities [UTILITIES]

const char* codex_error_string(CodexError err) {
    switch (err) {
        case CODEX_OK:              return "success";
        case CODEX_ERR_NULL:        return "null argument";
        case CODEX_ERR_INVALID_BOOK: return "invalid book number";
        case CODEX_ERR_INVALID_CHAPTER: return "invalid chapter";
        case CODEX_ERR_INVALID_VERSE: return "invalid verse";
        case CODEX_ERR_INVALID_ORDINAL: return "invalid ordinal";
        case CODEX_ERR_PARSE:       return "parse error";
        case CODEX_ERR_NOT_FOUND:   return "not found";
        default:                    return "unknown error";
    }
}

const char* codex_book_abbrev(uint8_t book_number) {
    const CodexBook* book = codex_book_by_number(book_number);
    return book ? book->abbrev : NULL;
}

const char* codex_book_name(uint8_t book_number) {
    const CodexBook* book = codex_book_by_number(book_number);
    return book ? book->name : NULL;
}

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
// =============================================================================
// END CLOSING
// =============================================================================
