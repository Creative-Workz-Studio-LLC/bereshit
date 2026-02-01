// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-omnicode-bible-impl
// Title:       MillenniumOS Bible Codex Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / OmniCode Subsystem
// Role:        Bible text access and search implementation
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
//              The Bible is THE source truth.
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), Bible service is not available.
#if CPISI_MODE >= 2

#include "services/omnicode/bible.h"
#include "services/vfs/vfs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Book Information Table
// -----------------------------------------------------------------------------

static const MosBibleBookInfo BOOK_INFO[MOS_BOOK_COUNT] = {
    // Old Testament - Law
    { MOS_BOOK_GENESIS,         "Genesis",          "Gen",   TRIT_NEG, MOS_BIBLE_CAT_LAW,      50 },
    { MOS_BOOK_EXODUS,          "Exodus",           "Exod",  TRIT_NEG, MOS_BIBLE_CAT_LAW,      40 },
    { MOS_BOOK_LEVITICUS,       "Leviticus",        "Lev",   TRIT_NEG, MOS_BIBLE_CAT_LAW,      27 },
    { MOS_BOOK_NUMBERS,         "Numbers",          "Num",   TRIT_NEG, MOS_BIBLE_CAT_LAW,      36 },
    { MOS_BOOK_DEUTERONOMY,     "Deuteronomy",      "Deut",  TRIT_NEG, MOS_BIBLE_CAT_LAW,      34 },

    // Old Testament - History
    { MOS_BOOK_JOSHUA,          "Joshua",           "Josh",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  24 },
    { MOS_BOOK_JUDGES,          "Judges",           "Judg",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  21 },
    { MOS_BOOK_RUTH,            "Ruth",             "Ruth",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  4  },
    { MOS_BOOK_1SAMUEL,         "1 Samuel",         "1Sam",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  31 },
    { MOS_BOOK_2SAMUEL,         "2 Samuel",         "2Sam",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  24 },
    { MOS_BOOK_1KINGS,          "1 Kings",          "1Kgs",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  22 },
    { MOS_BOOK_2KINGS,          "2 Kings",          "2Kgs",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  25 },
    { MOS_BOOK_1CHRONICLES,     "1 Chronicles",     "1Chr",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  29 },
    { MOS_BOOK_2CHRONICLES,     "2 Chronicles",     "2Chr",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  36 },
    { MOS_BOOK_EZRA,            "Ezra",             "Ezra",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  10 },
    { MOS_BOOK_NEHEMIAH,        "Nehemiah",         "Neh",   TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  13 },
    { MOS_BOOK_ESTHER,          "Esther",           "Esth",  TRIT_NEG, MOS_BIBLE_CAT_HISTORY,  10 },

    // Old Testament - Poetry
    { MOS_BOOK_JOB,             "Job",              "Job",   TRIT_NEG, MOS_BIBLE_CAT_POETRY,   42 },
    { MOS_BOOK_PSALMS,          "Psalms",           "Ps",    TRIT_NEG, MOS_BIBLE_CAT_POETRY,   150 },
    { MOS_BOOK_PROVERBS,        "Proverbs",         "Prov",  TRIT_NEG, MOS_BIBLE_CAT_POETRY,   31 },
    { MOS_BOOK_ECCLESIASTES,    "Ecclesiastes",     "Eccl",  TRIT_NEG, MOS_BIBLE_CAT_POETRY,   12 },
    { MOS_BOOK_SONG_OF_SOLOMON, "Song of Solomon",  "Song",  TRIT_NEG, MOS_BIBLE_CAT_POETRY,   8  },

    // Old Testament - Major Prophets
    { MOS_BOOK_ISAIAH,          "Isaiah",           "Isa",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 66 },
    { MOS_BOOK_JEREMIAH,        "Jeremiah",         "Jer",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 52 },
    { MOS_BOOK_LAMENTATIONS,    "Lamentations",     "Lam",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 5  },
    { MOS_BOOK_EZEKIEL,         "Ezekiel",          "Ezek",  TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 48 },
    { MOS_BOOK_DANIEL,          "Daniel",           "Dan",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 12 },

    // Old Testament - Minor Prophets
    { MOS_BOOK_HOSEA,           "Hosea",            "Hos",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 14 },
    { MOS_BOOK_JOEL,            "Joel",             "Joel",  TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 3  },
    { MOS_BOOK_AMOS,            "Amos",             "Amos",  TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 9  },
    { MOS_BOOK_OBADIAH,         "Obadiah",          "Obad",  TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 1  },
    { MOS_BOOK_JONAH,           "Jonah",            "Jonah", TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 4  },
    { MOS_BOOK_MICAH,           "Micah",            "Mic",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 7  },
    { MOS_BOOK_NAHUM,           "Nahum",            "Nah",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 3  },
    { MOS_BOOK_HABAKKUK,        "Habakkuk",         "Hab",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 3  },
    { MOS_BOOK_ZEPHANIAH,       "Zephaniah",        "Zeph",  TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 3  },
    { MOS_BOOK_HAGGAI,          "Haggai",           "Hag",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 2  },
    { MOS_BOOK_ZECHARIAH,       "Zechariah",        "Zech",  TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 14 },
    { MOS_BOOK_MALACHI,         "Malachi",          "Mal",   TRIT_NEG, MOS_BIBLE_CAT_PROPHETS, 4  },

    // New Testament - Gospels
    { MOS_BOOK_MATTHEW,         "Matthew",          "Matt",  TRIT_POS, MOS_BIBLE_CAT_GOSPELS,  28 },
    { MOS_BOOK_MARK,            "Mark",             "Mark",  TRIT_POS, MOS_BIBLE_CAT_GOSPELS,  16 },
    { MOS_BOOK_LUKE,            "Luke",             "Luke",  TRIT_POS, MOS_BIBLE_CAT_GOSPELS,  24 },
    { MOS_BOOK_JOHN,            "John",             "John",  TRIT_POS, MOS_BIBLE_CAT_GOSPELS,  21 },

    // New Testament - History
    { MOS_BOOK_ACTS,            "Acts",             "Acts",  TRIT_POS, MOS_BIBLE_CAT_ACTS,     28 },

    // New Testament - Pauline Epistles
    { MOS_BOOK_ROMANS,          "Romans",           "Rom",   TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 16 },
    { MOS_BOOK_1CORINTHIANS,    "1 Corinthians",    "1Cor",  TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 16 },
    { MOS_BOOK_2CORINTHIANS,    "2 Corinthians",    "2Cor",  TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 13 },
    { MOS_BOOK_GALATIANS,       "Galatians",        "Gal",   TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 6  },
    { MOS_BOOK_EPHESIANS,       "Ephesians",        "Eph",   TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 6  },
    { MOS_BOOK_PHILIPPIANS,     "Philippians",      "Phil",  TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 4  },
    { MOS_BOOK_COLOSSIANS,      "Colossians",       "Col",   TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 4  },
    { MOS_BOOK_1THESSALONIANS,  "1 Thessalonians",  "1Thes", TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 5  },
    { MOS_BOOK_2THESSALONIANS,  "2 Thessalonians",  "2Thes", TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 3  },
    { MOS_BOOK_1TIMOTHY,        "1 Timothy",        "1Tim",  TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 6  },
    { MOS_BOOK_2TIMOTHY,        "2 Timothy",        "2Tim",  TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 4  },
    { MOS_BOOK_TITUS,           "Titus",            "Titus", TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 3  },
    { MOS_BOOK_PHILEMON,        "Philemon",         "Phlm",  TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 1  },

    // New Testament - General Epistles
    { MOS_BOOK_HEBREWS,         "Hebrews",          "Heb",   TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 13 },
    { MOS_BOOK_JAMES,           "James",            "Jas",   TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 5  },
    { MOS_BOOK_1PETER,          "1 Peter",          "1Pet",  TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 5  },
    { MOS_BOOK_2PETER,          "2 Peter",          "2Pet",  TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 3  },
    { MOS_BOOK_1JOHN,           "1 John",           "1John", TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 5  },
    { MOS_BOOK_2JOHN,           "2 John",           "2John", TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 1  },
    { MOS_BOOK_3JOHN,           "3 John",           "3John", TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 1  },
    { MOS_BOOK_JUDE,            "Jude",             "Jude",  TRIT_POS, MOS_BIBLE_CAT_EPISTLES, 1  },

    // New Testament - Prophecy
    { MOS_BOOK_REVELATION,      "Revelation",       "Rev",   TRIT_POS, MOS_BIBLE_CAT_REVELATION, 22 },
};

// -----------------------------------------------------------------------------
// B.2 Embedded Key Verses (Minimal Offline Support)
// -----------------------------------------------------------------------------

typedef struct {
    MosBibleBook book;
    uint8_t chapter;
    uint8_t verse;
    const char* text;
} EmbeddedVerse;

static const EmbeddedVerse EMBEDDED_VERSES[] = {
    // Foundation verses
    { MOS_BOOK_GENESIS, 1, 1,
      "In the beginning God created the heaven and the earth." },
    { MOS_BOOK_JOHN, 1, 1,
      "In the beginning was the Word, and the Word was with God, "
      "and the Word was God." },
    { MOS_BOOK_JOHN, 1, 3,
      "All things were made by him; and without him was not any thing made "
      "that was made." },
    { MOS_BOOK_JOHN, 3, 16,
      "For God so loved the world, that he gave his only begotten Son, "
      "that whosoever believeth in him should not perish, but have everlasting life." },

    // Wisdom verses
    { MOS_BOOK_PROVERBS, 3, 5,
      "Trust in the LORD with all thine heart; and lean not unto thine own understanding." },
    { MOS_BOOK_PROVERBS, 3, 6,
      "In all thy ways acknowledge him, and he shall direct thy paths." },
    { MOS_BOOK_JAMES, 1, 5,
      "If any of you lack wisdom, let him ask of God, that giveth to all men liberally, "
      "and upbraideth not; and it shall be given him." },

    // Perseverance verses
    { MOS_BOOK_PROVERBS, 24, 16,
      "For a just man falleth seven times, and riseth up again: "
      "but the wicked shall fall into mischief." },
    { MOS_BOOK_PHILIPPIANS, 1, 6,
      "Being confident of this very thing, that he which hath begun a good work in you "
      "will perform it until the day of Jesus Christ." },

    // Light and guidance
    { MOS_BOOK_PSALMS, 119, 105,
      "Thy word is a lamp unto my feet, and a light unto my path." },
    { MOS_BOOK_MATTHEW, 5, 16,
      "Let your light so shine before men, that they may see your good works, "
      "and glorify your Father which is in heaven." },

    // Creation and design
    { MOS_BOOK_EXODUS, 25, 40,
      "And look that thou make them after their pattern, "
      "which was shewed thee in the mount." },
    { MOS_BOOK_ROMANS, 12, 2,
      "And be not conformed to this world: but be ye transformed by the renewing of your mind, "
      "that ye may prove what is that good, and acceptable, and perfect, will of God." },

    // Communication
    { MOS_BOOK_COLOSSIANS, 4, 6,
      "Let your speech be alway with grace, seasoned with salt, "
      "that ye may know how ye ought to answer every man." },
    { MOS_BOOK_PROVERBS, 25, 11,
      "A word fitly spoken is like apples of gold in pictures of silver." },

    // Scripture itself
    { MOS_BOOK_2TIMOTHY, 3, 16,
      "All scripture is given by inspiration of God, and is profitable for doctrine, "
      "for reproof, for correction, for instruction in righteousness." },

    // Ending marker
    { MOS_BOOK_COUNT, 0, 0, NULL }
};

// -----------------------------------------------------------------------------
// B.3 Global Codex State
// -----------------------------------------------------------------------------

static MosBibleCodex g_codex = {
    .loaded = false,
    .translation = "KJV",
    .base_path = "/scripture/KJV",
    .lookups = 0,
    .searches = 0,
    .cache_hits = 0,
    .health = MOS_HEALTH_DEFAULT
};

// -----------------------------------------------------------------------------
// B.4 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_bible_init(void) {
    g_codex.loaded = true;  // Embedded verses always available
    g_codex.translation = "KJV";
    g_codex.base_path = "/scripture/KJV";
    g_codex.health = MOS_HEALTH_DEFAULT;

    // TODO: Check if VFS has full Bible text available
    // If not, we'll use embedded verses only

    return MOS_RESULT_OK;
}

mos_result_t mos_bible_init_path(const char* base_path) {
    if (!base_path) return MOS_RESULT_ERROR;

    g_codex.base_path = base_path;
    return mos_bible_init();
}

void mos_bible_shutdown(void) {
    g_codex.loaded = false;
}

const MosBibleCodex* mos_bible_info(void) {
    return &g_codex;
}

// -----------------------------------------------------------------------------
// B.5 Reference Parsing
// -----------------------------------------------------------------------------

static int str_casecmp(const char* a, const char* b) {
    while (*a && *b) {
        char ca = tolower((unsigned char)*a);
        char cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

static int str_ncasecmp(const char* a, const char* b, size_t n) {
    while (n > 0 && *a && *b) {
        char ca = tolower((unsigned char)*a);
        char cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++;
        b++;
        n--;
    }
    if (n == 0) return 0;
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

MosBibleBook mos_bible_find_book(const char* name) {
    if (!name) return MOS_BOOK_COUNT;

    // Skip leading whitespace
    while (*name && isspace((unsigned char)*name)) name++;

    // Handle numbered books (1 John, 2 Tim, etc.)
    int number = 0;
    if (*name >= '1' && *name <= '3') {
        number = *name - '0';
        name++;
        while (*name && isspace((unsigned char)*name)) name++;
    }

    // Try exact match first
    for (int i = 0; i < MOS_BOOK_COUNT; i++) {
        if (str_casecmp(name, BOOK_INFO[i].name) == 0) {
            return (MosBibleBook)i;
        }
        if (str_casecmp(name, BOOK_INFO[i].abbrev) == 0) {
            return (MosBibleBook)i;
        }
    }

    // Try prefix match
    size_t len = strlen(name);
    for (int i = 0; i < MOS_BOOK_COUNT; i++) {
        if (str_ncasecmp(name, BOOK_INFO[i].name, len) == 0) {
            return (MosBibleBook)i;
        }
    }

    // Handle numbered books
    if (number > 0) {
        const char* numbered_names[] = {
            "Samuel", "Kings", "Chronicles", "Corinthians",
            "Thessalonians", "Timothy", "Peter", "John"
        };
        MosBibleBook first_books[] = {
            MOS_BOOK_1SAMUEL, MOS_BOOK_1KINGS, MOS_BOOK_1CHRONICLES,
            MOS_BOOK_1CORINTHIANS, MOS_BOOK_1THESSALONIANS,
            MOS_BOOK_1TIMOTHY, MOS_BOOK_1PETER, MOS_BOOK_1JOHN
        };

        for (int i = 0; i < 8; i++) {
            if (str_ncasecmp(name, numbered_names[i], len) == 0) {
                MosBibleBook base = first_books[i];
                if (number == 1) return base;
                if (number == 2) return (MosBibleBook)(base + 1);
                if (number == 3 && base == MOS_BOOK_1JOHN) {
                    return MOS_BOOK_3JOHN;
                }
            }
        }
    }

    return MOS_BOOK_COUNT;  // Not found
}

mos_result_t mos_bible_parse_ref(const char* str, MosBibleRef* ref) {
    if (!str || !ref) return MOS_RESULT_ERROR;

    // Skip leading whitespace
    while (*str && isspace((unsigned char)*str)) str++;

    // Find book name (up to first digit or colon)
    char book_name[64];
    int i = 0;

    // Handle numbered books
    if (*str >= '1' && *str <= '3') {
        book_name[i++] = *str++;
        while (*str && isspace((unsigned char)*str)) str++;
    }

    // Copy book name
    while (*str && !isdigit((unsigned char)*str) && *str != ':' && i < 63) {
        book_name[i++] = *str++;
    }

    // Trim trailing whitespace
    while (i > 0 && isspace((unsigned char)book_name[i - 1])) i--;
    book_name[i] = '\0';

    // Find book
    ref->book = mos_bible_find_book(book_name);
    if (ref->book == MOS_BOOK_COUNT) {
        return MOS_RESULT_ERROR;
    }

    // Skip whitespace
    while (*str && isspace((unsigned char)*str)) str++;

    // Parse chapter
    if (!isdigit((unsigned char)*str)) {
        return MOS_RESULT_ERROR;
    }

    ref->chapter = 0;
    while (isdigit((unsigned char)*str)) {
        ref->chapter = ref->chapter * 10 + (*str - '0');
        str++;
    }

    // Skip colon or whitespace
    while (*str && (isspace((unsigned char)*str) || *str == ':')) str++;

    // Parse verse
    if (isdigit((unsigned char)*str)) {
        ref->verse_start = 0;
        while (isdigit((unsigned char)*str)) {
            ref->verse_start = ref->verse_start * 10 + (*str - '0');
            str++;
        }

        // Check for range
        while (*str && isspace((unsigned char)*str)) str++;
        if (*str == '-') {
            str++;
            while (*str && isspace((unsigned char)*str)) str++;

            ref->verse_end = 0;
            while (isdigit((unsigned char)*str)) {
                ref->verse_end = ref->verse_end * 10 + (*str - '0');
                str++;
            }
        } else {
            ref->verse_end = 0;  // Single verse
        }
    } else {
        ref->verse_start = 1;
        ref->verse_end = 0;
    }

    return MOS_RESULT_OK;
}

mos_result_t mos_bible_format_ref(const MosBibleRef* ref, char* str,
                                   mos_size_t size) {
    if (!ref || !str || size == 0) return MOS_RESULT_ERROR;

    if (ref->book >= MOS_BOOK_COUNT) return MOS_RESULT_ERROR;

    if (ref->verse_end > 0) {
        snprintf(str, size, "%s %d:%d-%d",
                 BOOK_INFO[ref->book].name, ref->chapter,
                 ref->verse_start, ref->verse_end);
    } else {
        snprintf(str, size, "%s %d:%d",
                 BOOK_INFO[ref->book].name, ref->chapter, ref->verse_start);
    }

    return MOS_RESULT_OK;
}

bool mos_bible_ref_valid(const MosBibleRef* ref) {
    if (!ref) return false;
    if (ref->book >= MOS_BOOK_COUNT) return false;
    if (ref->chapter == 0 || ref->chapter > BOOK_INFO[ref->book].chapters) {
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// B.6 Verse Lookup
// -----------------------------------------------------------------------------

static const EmbeddedVerse* find_embedded_verse(const MosBibleRef* ref) {
    for (int i = 0; EMBEDDED_VERSES[i].text != NULL; i++) {
        if (EMBEDDED_VERSES[i].book == ref->book &&
            EMBEDDED_VERSES[i].chapter == ref->chapter &&
            EMBEDDED_VERSES[i].verse == ref->verse_start) {
            return &EMBEDDED_VERSES[i];
        }
    }
    return NULL;
}

mos_result_t mos_bible_get_verse(const MosBibleRef* ref, MosBibleVerse* verse) {
    if (!ref || !verse) return MOS_RESULT_ERROR;
    if (!mos_bible_ref_valid(ref)) return MOS_RESULT_ERROR;

    g_codex.lookups++;

    // Check embedded verses first
    const EmbeddedVerse* embedded = find_embedded_verse(ref);
    if (embedded) {
        verse->ref = *ref;
        verse->text = embedded->text;
        verse->length = strlen(embedded->text);
        g_codex.cache_hits++;
        return MOS_RESULT_OK;
    }

    // TODO: Load from VFS if available
    // For now, return error if not in embedded verses

    return MOS_RESULT_ERROR;  // Verse not found
}

mos_result_t mos_bible_lookup(const char* ref_str, char* text, mos_size_t size) {
    if (!ref_str || !text || size == 0) return MOS_RESULT_ERROR;

    MosBibleRef ref;
    mos_result_t result = mos_bible_parse_ref(ref_str, &ref);
    if (result != MOS_RESULT_OK) return result;

    MosBibleVerse verse;
    result = mos_bible_get_verse(&ref, &verse);
    if (result != MOS_RESULT_OK) return result;

    strncpy(text, verse.text, size - 1);
    text[size - 1] = '\0';
    return MOS_RESULT_OK;
}

mos_result_t mos_bible_get_passage(const MosBibleRef* ref, char* text,
                                    mos_size_t size) {
    // For now, just get single verse
    MosBibleVerse verse;
    mos_result_t result = mos_bible_get_verse(ref, &verse);
    if (result != MOS_RESULT_OK) return result;

    strncpy(text, verse.text, size - 1);
    text[size - 1] = '\0';
    return MOS_RESULT_OK;
}

mos_result_t mos_bible_get_chapter(MosBibleBook book, uint8_t chapter,
                                    char* text, mos_size_t size) {
    (void)book;
    (void)chapter;
    (void)text;
    (void)size;
    // TODO: Implement full chapter loading
    return MOS_RESULT_ERROR;
}

// -----------------------------------------------------------------------------
// B.7 Book Information
// -----------------------------------------------------------------------------

const MosBibleBookInfo* mos_bible_book_info(MosBibleBook book) {
    if (book >= MOS_BOOK_COUNT) return NULL;
    return &BOOK_INFO[book];
}

const char* mos_bible_book_name(MosBibleBook book) {
    if (book >= MOS_BOOK_COUNT) return "Unknown";
    return BOOK_INFO[book].name;
}

const char* mos_bible_book_abbrev(MosBibleBook book) {
    if (book >= MOS_BOOK_COUNT) return "???";
    return BOOK_INFO[book].abbrev;
}

uint8_t mos_bible_chapter_count(MosBibleBook book) {
    if (book >= MOS_BOOK_COUNT) return 0;
    return BOOK_INFO[book].chapters;
}

uint8_t mos_bible_verse_count(MosBibleBook book, uint8_t chapter) {
    (void)book;
    (void)chapter;
    // TODO: Implement verse count table
    return 30;  // Default guess
}

// -----------------------------------------------------------------------------
// B.8 Search
// -----------------------------------------------------------------------------

mos_result_t mos_bible_search(const char* query, MosBibleSearchResult* results,
                               mos_size_t max_results, mos_size_t* found) {
    if (!query || !results || max_results == 0 || !found) {
        return MOS_RESULT_ERROR;
    }

    g_codex.searches++;
    *found = 0;

    // Search embedded verses
    for (int i = 0; EMBEDDED_VERSES[i].text != NULL && *found < max_results; i++) {
        const char* match = strstr(EMBEDDED_VERSES[i].text, query);
        if (match) {
            results[*found].ref.book = EMBEDDED_VERSES[i].book;
            results[*found].ref.chapter = EMBEDDED_VERSES[i].chapter;
            results[*found].ref.verse_start = EMBEDDED_VERSES[i].verse;
            results[*found].ref.verse_end = 0;
            results[*found].text = EMBEDDED_VERSES[i].text;
            results[*found].match_start = match - EMBEDDED_VERSES[i].text;
            results[*found].match_length = strlen(query);
            (*found)++;
        }
    }

    return MOS_RESULT_OK;
}

mos_result_t mos_bible_search_ex(const char* query, trit_t testament,
                                  MosBibleBook book_filter,
                                  MosBibleSearchResult* results,
                                  mos_size_t max_results, mos_size_t* found) {
    (void)testament;
    (void)book_filter;
    // For now, just use basic search
    return mos_bible_search(query, results, max_results, found);
}

mos_result_t mos_bible_search_phrase(const char* phrase,
                                      MosBibleSearchResult* results,
                                      mos_size_t max_results, mos_size_t* found) {
    return mos_bible_search(phrase, results, max_results, found);
}

// -----------------------------------------------------------------------------
// B.9 Random and Daily Verses
// -----------------------------------------------------------------------------

mos_result_t mos_bible_random_verse(MosBibleVerse* verse) {
    if (!verse) return MOS_RESULT_ERROR;

    // Count embedded verses
    int count = 0;
    while (EMBEDDED_VERSES[count].text != NULL) count++;

    if (count == 0) return MOS_RESULT_ERROR;

    // Pick random
    int idx = rand() % count;

    verse->ref.book = EMBEDDED_VERSES[idx].book;
    verse->ref.chapter = EMBEDDED_VERSES[idx].chapter;
    verse->ref.verse_start = EMBEDDED_VERSES[idx].verse;
    verse->ref.verse_end = 0;
    verse->text = EMBEDDED_VERSES[idx].text;
    verse->length = strlen(verse->text);

    return MOS_RESULT_OK;
}

mos_result_t mos_bible_daily_verse(MosBibleVerse* verse) {
    if (!verse) return MOS_RESULT_ERROR;

    // Count embedded verses
    int count = 0;
    while (EMBEDDED_VERSES[count].text != NULL) count++;

    if (count == 0) return MOS_RESULT_ERROR;

    // Deterministic based on day of year
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    int day_of_year = tm->tm_yday;

    int idx = day_of_year % count;

    verse->ref.book = EMBEDDED_VERSES[idx].book;
    verse->ref.chapter = EMBEDDED_VERSES[idx].chapter;
    verse->ref.verse_start = EMBEDDED_VERSES[idx].verse;
    verse->ref.verse_end = 0;
    verse->text = EMBEDDED_VERSES[idx].text;
    verse->length = strlen(verse->text);

    return MOS_RESULT_OK;
}

mos_result_t mos_bible_health_verse(mos_health_t health, MosBibleVerse* verse) {
    if (!verse) return MOS_RESULT_ERROR;

    // Select verse based on health state
    MosBibleRef ref;

    if (health < -50) {
        // Crisis - comfort
        ref.book = MOS_BOOK_PSALMS;
        ref.chapter = 23;
        ref.verse_start = 4;  // "Though I walk through the valley..."
    } else if (health < 0) {
        // Struggling - encouragement
        ref.book = MOS_BOOK_PROVERBS;
        ref.chapter = 24;
        ref.verse_start = 16;  // "A just man falleth seven times..."
    } else if (health < 50) {
        // Neutral - wisdom
        ref.book = MOS_BOOK_PROVERBS;
        ref.chapter = 3;
        ref.verse_start = 5;  // "Trust in the LORD..."
    } else {
        // Thriving - celebration
        ref.book = MOS_BOOK_PHILIPPIANS;
        ref.chapter = 1;
        ref.verse_start = 6;  // "Being confident of this..."
    }
    ref.verse_end = 0;

    // Look up the verse
    const EmbeddedVerse* embedded = find_embedded_verse(&ref);
    if (embedded) {
        verse->ref = ref;
        verse->text = embedded->text;
        verse->length = strlen(embedded->text);
        return MOS_RESULT_OK;
    }

    // Fallback to Genesis 1:1
    ref.book = MOS_BOOK_GENESIS;
    ref.chapter = 1;
    ref.verse_start = 1;
    return mos_bible_get_verse(&ref, verse);
}

mos_result_t mos_bible_state_verse(MosHebrewState state, MosBibleVerse* verse) {
    if (!verse) return MOS_RESULT_ERROR;

    // Map Hebrew state to appropriate verse
    MosBibleRef ref;

    switch (state) {
        case MOS_HEBREW_SHAVAR:  // Broken
            ref.book = MOS_BOOK_PSALMS;
            ref.chapter = 34;
            ref.verse_start = 18;  // "The LORD is nigh unto them..."
            break;

        case MOS_HEBREW_CHASER:  // Lacking
        case MOS_HEBREW_RATSAH:  // Wanting
            ref.book = MOS_BOOK_JAMES;
            ref.chapter = 1;
            ref.verse_start = 5;  // "If any of you lack wisdom..."
            break;

        case MOS_HEBREW_YASHAR:  // Even/balanced
            ref.book = MOS_BOOK_PROVERBS;
            ref.chapter = 3;
            ref.verse_start = 5;  // "Trust in the LORD..."
            break;

        case MOS_HEBREW_TAMIM:   // Sound
        case MOS_HEBREW_SHALEM:  // Whole
            ref.book = MOS_BOOK_COLOSSIANS;
            ref.chapter = 4;
            ref.verse_start = 6;  // "Let your speech..."
            break;

        case MOS_HEBREW_TOV:     // Perfect/complete
            ref.book = MOS_BOOK_GENESIS;
            ref.chapter = 1;
            ref.verse_start = 31; // "And God saw...very good"
            break;

        default:
            ref.book = MOS_BOOK_GENESIS;
            ref.chapter = 1;
            ref.verse_start = 1;
            break;
    }
    ref.verse_end = 0;

    return mos_bible_get_verse(&ref, verse);
}

// -----------------------------------------------------------------------------
// B.10 Navigation
// -----------------------------------------------------------------------------

mos_result_t mos_bible_next_verse(const MosBibleRef* current, MosBibleRef* next) {
    if (!current || !next) return MOS_RESULT_ERROR;

    *next = *current;
    next->verse_start++;
    // TODO: Check if we've exceeded verse count, advance chapter

    return MOS_RESULT_OK;
}

mos_result_t mos_bible_prev_verse(const MosBibleRef* current, MosBibleRef* prev) {
    if (!current || !prev) return MOS_RESULT_ERROR;

    *prev = *current;
    if (prev->verse_start > 1) {
        prev->verse_start--;
    }
    // TODO: Handle chapter boundary

    return MOS_RESULT_OK;
}

mos_result_t mos_bible_next_chapter(const MosBibleRef* current, MosBibleRef* next) {
    if (!current || !next) return MOS_RESULT_ERROR;

    *next = *current;
    next->chapter++;
    next->verse_start = 1;
    next->verse_end = 0;

    if (next->chapter > BOOK_INFO[current->book].chapters) {
        // Move to next book
        if (current->book < MOS_BOOK_COUNT - 1) {
            next->book = (MosBibleBook)(current->book + 1);
            next->chapter = 1;
        }
    }

    return MOS_RESULT_OK;
}

mos_result_t mos_bible_prev_chapter(const MosBibleRef* current, MosBibleRef* prev) {
    if (!current || !prev) return MOS_RESULT_ERROR;

    *prev = *current;
    if (prev->chapter > 1) {
        prev->chapter--;
    } else if (prev->book > 0) {
        prev->book = (MosBibleBook)(prev->book - 1);
        prev->chapter = BOOK_INFO[prev->book].chapters;
    }
    prev->verse_start = 1;
    prev->verse_end = 0;

    return MOS_RESULT_OK;
}

// -----------------------------------------------------------------------------
// B.11 OmniCode Integration
// -----------------------------------------------------------------------------

mos_result_t mos_bible_omni_lookup(const char* expr, char* result,
                                    mos_size_t size) {
    // Parse OmniCode-style Bible expression
    // e.g., bible("John 3:16") or bible.lookup("Gen 1:1")

    if (!expr || !result || size == 0) return MOS_RESULT_ERROR;

    // Extract reference from expression
    const char* start = strchr(expr, '"');
    if (!start) start = strchr(expr, '\'');
    if (!start) {
        // Treat whole expression as reference
        return mos_bible_lookup(expr, result, size);
    }

    start++;
    const char* end = strchr(start, '"');
    if (!end) end = strchr(start, '\'');
    if (!end) return MOS_RESULT_ERROR;

    char ref[MOS_BIBLE_MAX_REF];
    size_t len = end - start;
    if (len >= MOS_BIBLE_MAX_REF) len = MOS_BIBLE_MAX_REF - 1;
    memcpy(ref, start, len);
    ref[len] = '\0';

    return mos_bible_lookup(ref, result, size);
}

mos_result_t mos_bible_grounding(const char* context, MosBibleVerse* verse) {
    if (!verse) return MOS_RESULT_ERROR;

    // Match context to appropriate grounding verse
    if (context) {
        // Look for keywords
        if (strstr(context, "creat") || strstr(context, "begin")) {
            MosBibleRef ref = { MOS_BOOK_GENESIS, 1, 1, 0 };
            return mos_bible_get_verse(&ref, verse);
        }
        if (strstr(context, "word") || strstr(context, "language")) {
            MosBibleRef ref = { MOS_BOOK_JOHN, 1, 1, 0 };
            return mos_bible_get_verse(&ref, verse);
        }
        if (strstr(context, "pattern") || strstr(context, "template")) {
            MosBibleRef ref = { MOS_BOOK_EXODUS, 25, 40, 0 };
            return mos_bible_get_verse(&ref, verse);
        }
        if (strstr(context, "wisdom") || strstr(context, "understand")) {
            MosBibleRef ref = { MOS_BOOK_PROVERBS, 3, 5, 0 };
            return mos_bible_get_verse(&ref, verse);
        }
    }

    // Default: Genesis 1:1
    MosBibleRef ref = { MOS_BOOK_GENESIS, 1, 1, 0 };
    return mos_bible_get_verse(&ref, verse);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Bible Codex Implementation provides:
//
//   1. Reference parsing: "Gen 1:1", "John 3:16", "Ps 23:1-6"
//   2. Verse lookup from embedded verses and VFS
//   3. Search capabilities within available text
//   4. Health-based and state-based verse selection
//   5. Navigation: next/prev verse/chapter
//   6. OmniCode integration: bible("John 1:1")
//
// Embedded verses ensure minimal offline functionality.
// Full Bible text loaded from VFS when available.
//
// "All scripture is given by inspiration of God, and is profitable
//  for doctrine, for reproof, for correction, for instruction
//  in righteousness."
// — 2 Timothy 3:16
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* CPISI_MODE >= 2 - Full Bible service implementation */
