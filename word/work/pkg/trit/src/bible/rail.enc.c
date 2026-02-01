// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Bible Rail Encoding Implementation
// Key: B-word-work-pkg-trit-src-bible-rail
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: bible_rail.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/work/pkg/trit/src/temporal.c
// See: word/scripture/web-variant-index.csv for 13 variant mappings
//
// ═══════════════════════════════════════════════════════════════════════════

// Bible Rail encoding: maps spare trit5 values (243-255) to WEB textual variants.
//
// libtrit Library - CPI-SI Kingdom Technology
//
// ============================================================================
// METADATA
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// CORE IDENTITY
// ────────────────────────────────────────────────────────────────
//
// # Biblical Foundation
//
// Scripture: "Add thou not unto his words, lest he reprove thee, and
//             thou be found a liar." — Proverbs 30:6
//
// Principle: The 13 spare values in trit5 packing (243-255) are not waste—
//            they encode the 13 textual variants between KJV and WEB.
//            Every bit serves the Word. Nothing is added or removed.
//
// Anchor: "For I testify unto every man that heareth the words of the
//          prophecy of this book, If any man shall add unto these things..."
//          — Revelation 22:18
//
// # CPI-SI Identity
//
// Component Type: Ladder (builds on trit5_t packing)
//
// Role: Implements Bible Rail encoding functions. Maps spare trit5 values
//       to WEB textual variant indices. Bridges spec to implementation.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-14
//   - Version: 0.1.0
//   - Modified: 2025-12-14 - Initial implementation
//
// Version History:
//
//   - 0.1.0 (2025-12-14) - Initial Bible Rail encoding
//
// # Purpose & Function
//
// Purpose: Implement Bible Rail encode/decode functions for spare trit5 values.
//
// Core Design: Static variant table with 13 entries. O(1) encode/decode via
//              simple arithmetic (index + 243 = spare, spare - 243 = index).
//
// Key Features:
//
//   - Encoding: variant index → spare trit5 value
//   - Decoding: spare trit5 value → variant index
//   - Query: check spare, get reference, get description
//   - Safety: invalid inputs return -1 or NULL
//
// Philosophy: Nothing wasted. Every spare byte serves the Word.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: stddef.h (for NULL)
//   - External: None
//   - Internal: bible_rail.h (includes trit.h, stdbool.h)
//
// What Uses This:
//
//   - Libraries: libtrit.a consumers
//   - Commands: demo-math, Bible Rail validation tools
//
// Integration Points:
//
//   - Link with libtrit.a
//   - Include bible_rail.h for type definitions
//
// # Usage & Integration
//
// [OMIT: Library source - not a command-line executable]
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// [OMIT: Pure functions - no blocking, no health scoring needed]

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Includes → Defines → Static Variables → Types → Function Prototypes

// ────────────────────────────────────────────────────────────────
// Includes
// ────────────────────────────────────────────────────────────────

//--- Project Headers ---
#include "trit/bible/rail.enc.h"  // Types and prototypes (includes trit.type.h, stdbool.h)

//--- Standard Library ---
#include <stddef.h>      // NULL
#include <string.h>      // strcmp for variant_at

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All constants defined in bible_rail.h]

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────

// WEB textual variant table.
//
// The 13 entries correspond to spare values 243-255.
// Index 0 = spare 243, index 12 = spare 255.
//
// See: word/scripture/web-variant-index.csv
//
typedef struct {
    const char* reference;  // Book chapter:verse
    const char* description;  // Brief description
} variant_entry_t;

static const variant_entry_t variant_table[BIBLE_RAIL_VARIANT_COUNT] = {
    { "1 Corinthians 16:27", "TR manuscript note" },
    { "1 Peter 5:20", "turn sinner from error" },
    { "1 Timothy 3:18", "Grace benediction" },
    { "2 Kings 22:53", "served Baal worshiped him" },
    { "Amos 3:21", "cleanse their blood" },
    { "Colossians 4:23", "Grace benediction" },
    { "Ezekiel 5:22", "utterly rejected us" },
    { "Jonah 1:21", "Saviors on Mount Zion" },
    { "Nehemiah 10:44", "taken foreign wives" },
    { "Numbers 27:34", "commandments on Sinai" },
    { "Psalms 42:17", "Job died old and full" },
    { "Revelation 1:25", "Doxology" },
    { "Zechariah 2:23", "Zerubbabel my servant" }
};

// Structured location table for location-based lookup.
//
// Parallel to variant_table - same index, structured data.
// Enables bible_rail_variant_at() to check by book/chapter/verse.
//
// See: word/scripture/web-variant-index.csv
//
static const variant_location_t location_table[BIBLE_RAIL_VARIANT_COUNT] = {
    { "1 Corinthians", 16, 27 },  // index 0, spare 243
    { "1 Peter",        5, 20 },  // index 1, spare 244
    { "1 Timothy",      3, 18 },  // index 2, spare 245
    { "2 Kings",       22, 53 },  // index 3, spare 246
    { "Amos",           3, 21 },  // index 4, spare 247
    { "Colossians",     4, 23 },  // index 5, spare 248
    { "Ezekiel",        5, 22 },  // index 6, spare 249
    { "Jonah",          1, 21 },  // index 7, spare 250
    { "Nehemiah",      10, 44 },  // index 8, spare 251
    { "Numbers",       27, 34 },  // index 9, spare 252
    { "Psalms",        42, 17 },  // index 10, spare 253
    { "Revelation",     1, 25 },  // index 11, spare 254
    { "Zechariah",      2, 23 }   // index 12, spare 255
};

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// [Reserved: variant_entry_t defined above, all other types in bible_rail.h]

// ────────────────────────────────────────────────────────────────
// Function Prototypes (Private)
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All functions are public APIs - no private helpers]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Function implementations organized by purpose:
// 1. Encoding: bible_rail_encode, bible_rail_decode
// 2. Query: bible_rail_is_spare, variant_ref, variant_desc, variant_count
// 3. Location Lookup: variant_at, variant_location

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Implementation Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   variant_table (static data)
//   └── All query functions read from this table
//
//   Helpers: None (all functions are standalone)
//
// Baton Flow (Execution Paths):
//   Encode: index → bounds check → index + 243 → return spare
//   Decode: spare → is_spare check → spare - 243 → return index
//   Query: index → bounds check → table lookup → return string
//
// APUs (Available Processing Units):
//   - 8 functions (all public)
//   - 2 static tables (13 entries each)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All functions are public APIs]

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Using Public APIs section for all implementations]

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────

//--- Encoding Functions ---

// Encode a WEB variant index as a spare trit5 value.
//
// Implements: variant_index (0-12) → spare trit5 (243-255)
// Formula: spare = index + BIBLE_RAIL_SPARE_MIN
//
// Returns 0 if index out of range (0 is never a valid spare).
trit5_t bible_rail_encode(int variant_index) {
    if (variant_index < 0 || variant_index >= BIBLE_RAIL_VARIANT_COUNT) {
        return 0;  // Invalid - 0 is semantic, never spare
    }
    return (trit5_t)(BIBLE_RAIL_SPARE_MIN + variant_index);
}

// Decode a spare trit5 value to its WEB variant index.
//
// Implements: spare trit5 (243-255) → variant_index (0-12)
// Formula: index = spare - BIBLE_RAIL_SPARE_MIN
//
// Returns -1 if not a spare value.
int bible_rail_decode(trit5_t spare) {
    if (!bible_rail_is_spare(spare)) {
        return -1;  // Not a spare value
    }
    return (int)(spare - BIBLE_RAIL_SPARE_MIN);
}

//--- Query Functions ---

// Check if a trit5 value is a spare (variant encoding) value.
//
// Spare values are 243-255 (13 values).
// Semantic values are 0-242 (3^5 = 243 values).
// Note: upper bound check omitted since trit5_t (uint8_t) max is 255.
bool bible_rail_is_spare(trit5_t value) {
    return value >= BIBLE_RAIL_SPARE_MIN;  // Max is 255, always valid spare if >= 243
}

// Get the verse reference for a WEB variant.
//
// Returns static string with book chapter:verse reference.
// Returns NULL if variant_index is out of range.
const char* bible_rail_variant_ref(int variant_index) {
    if (variant_index < 0 || variant_index >= BIBLE_RAIL_VARIANT_COUNT) {
        return NULL;
    }
    return variant_table[variant_index].reference;
}

// Get the description for a WEB variant.
//
// Returns static string with brief description.
// Returns NULL if variant_index is out of range.
const char* bible_rail_variant_desc(int variant_index) {
    if (variant_index < 0 || variant_index >= BIBLE_RAIL_VARIANT_COUNT) {
        return NULL;
    }
    return variant_table[variant_index].description;
}

// Get the total count of WEB variants.
//
// Returns 13 (BIBLE_RAIL_VARIANT_COUNT).
int bible_rail_variant_count(void) {
    return BIBLE_RAIL_VARIANT_COUNT;
}

//--- Location Lookup Functions ---

// Check if a location is a WEB variant location.
//
// Searches the location_table for matching book/chapter/verse.
// Returns spare trit5 value (243-255) if found, 0 if not variant.
trit5_t bible_rail_variant_at(const char* book, uint8_t chapter, uint8_t verse) {
    if (book == NULL) {
        return 0;  // Invalid input
    }

    // Linear search through 13 entries (small table, no optimization needed)
    for (int i = 0; i < BIBLE_RAIL_VARIANT_COUNT; i++) {
        if (strcmp(book, location_table[i].book) == 0 &&
            chapter == location_table[i].chapter &&
            verse == location_table[i].verse) {
            // Found match - return spare value (index + 243)
            return (trit5_t)(BIBLE_RAIL_SPARE_MIN + i);
        }
    }

    return 0;  // Not a variant location
}

// Get the structured location for a WEB variant.
//
// Returns variant_location_t with book/chapter/verse.
// Returns {NULL, 0, 0} if variant_index is out of range.
variant_location_t bible_rail_variant_location(int variant_index) {
    variant_location_t invalid = { NULL, 0, 0 };

    if (variant_index < 0 || variant_index >= BIBLE_RAIL_VARIANT_COUNT) {
        return invalid;
    }

    return location_table[variant_index];
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Code Validation
// ────────────────────────────────────────────────────────────────
//
// Build Verification:
//   gcc -c -Wall -Wextra -std=c99 -I../include bible_rail.c
//
// Test Verification:
//   See test/bible_rail_test.c for encode/decode roundtrip tests

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Library source - linked into libtrit.a]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - static table, static strings]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Bible Rail implementation maps spare trit5 values (243-255)
// to the 13 WEB textual variants. Static table, O(1) operations.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions (follow existing pattern)
//   ✅ Update variant descriptions
//
// Modify with Care:
//   ⚠️ variant_table order (must match CSV and spec)
//   ⚠️ Function implementations (match header prototypes)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Encode/decode arithmetic (index + 243 = spare)
//   ❌ Variant count (13 from trit5 math)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Header: include/bible_rail.h
// Test: test/bible_rail_test.c
// Data: word/scripture/web-variant-index.csv
// Spec: word/research/bereshit/bereshit-trit-byte-encoding.adoc

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// 13 spare bytes. 13 textual variants. Nothing wasted.
// Every bit serves the Word.
//
// "Add thou not unto his words..." — Proverbs 30:6

// ============================================================================
// END CLOSING
// ============================================================================
