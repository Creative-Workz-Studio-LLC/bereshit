// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Scripture Path Resolution Implementation
// Key: B-word-work-pkg-trit-src-scripture-path
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: scripture_path.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/work/pkg/trit/include/scripture_path.h
// See: word/scripture/ for KJV and WEB directory structure
//
// ═══════════════════════════════════════════════════════════════════════════

// Scripture path resolution: generates file paths for KJV verses and WEB variants.
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
// Scripture: "Thy word is a lamp unto my feet, and a light unto my path."
//            — Psalm 119:105
//
// Principle: Scripture paths are not arbitrary—they follow the Word's order.
//            KJV verses have canonical paths. WEB variants mark textual
//            differences. The path generation reveals Scripture's structure.
//
// Anchor: "All scripture is given by inspiration of God, and is profitable
//          for doctrine..." — 2 Timothy 3:16
//
// # CPI-SI Identity
//
// Component Type: Bridge (connects encoding to filesystem)
//
// Role: Implements scripture path resolution functions. Translates packed
//       addresses to filesystem paths. Variant-aware dispatch.
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
//   - 0.1.0 (2025-12-14) - Initial scripture path resolution
//
// # Purpose & Function
//
// Purpose: Implement scripture path resolution for variant-aware file lookup.
//
// Core Design: Book normalization (spaces → underscores), path generation
//              via snprintf, variant-aware dispatch using is_variant().
//
// Key Features:
//
//   - Path generation: book/ch/vs → filesystem path
//   - Variant-aware: detects KJV vs WEB addresses
//   - Type enumeration: INVALID, KJV_VERSE, WEB_VARIANT
//   - Safety: buffer overflow protection, NULL checks
//
// Philosophy: The path to Scripture is never arbitrary—it follows divine order.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: stdio.h (snprintf), string.h (strlen, strcpy)
//   - External: None
//   - Internal: scripture_path.h (includes bible_rail.h, verse_addr.h)
//
// What Uses This:
//
//   - Libraries: libtrit.a consumers
//   - Commands: demo-math, Scripture lookup tools
//
// Integration Points:
//
//   - Link with libtrit.a
//   - Include scripture_path.h for type definitions
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
#include "trit/bible/path.txt.h"  // Types and prototypes (includes rail.enc.h, verse.enc.h)

//--- Standard Library ---
#include <stdio.h>       // snprintf
#include <string.h>      // strlen, memset

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All constants defined in scripture_path.h]

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────

// Type name strings for scripture_path_type_name().
// Balanced ternary: -1, 0, +1 → index by adding 1 (0, 1, 2)
static const char* type_names[] = {
    "INVALID",     // SCRIPTURE_PATH_INVALID = -1  → index 0
    "KJV_VERSE",   // SCRIPTURE_PATH_KJV_VERSE = 0 → index 1
    "WEB_VARIANT"  // SCRIPTURE_PATH_WEB_VARIANT = 1 → index 2
};

// Version name strings for scripture_path_version_name().
// Balanced ternary: 0, +1 → direct index (0, 1)
static const char* version_names[] = {
    "KJV",  // SCRIPTURE_VERSION_KJV = 0 → index 0
    "WEB"   // SCRIPTURE_VERSION_WEB = 1 → index 1
};

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All types defined in scripture_path.h]

// ────────────────────────────────────────────────────────────────
// Function Prototypes (Private)
// ────────────────────────────────────────────────────────────────

// Internal helper: generate path with given prefix.
static bool path_generate_internal(const char* prefix, const char* book,
                                   uint8_t chapter, uint8_t verse,
                                   char* out, size_t outlen);

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Function implementations organized by purpose:
// 1. Helpers: path_generate_internal
// 2. Path Generation: normalize_book, kjv, web
// 3. Address Resolution: resolve, resolve_with_location, type_name

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Implementation Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   path_generate_internal (helper)
//   ├── scripture_path_kjv
//   └── scripture_path_web
//
//   scripture_path_normalize_book (standalone)
//   scripture_path_type_name (standalone)
//
//   scripture_path_resolve
//   └── Uses bible_rail_variant_location, verse_addr_is_variant
//
//   scripture_path_resolve_with_location
//   └── Uses path_generate_internal
//
// Baton Flow (Execution Paths):
//   Resolve: addr → is_variant? → WEB path or KJV type → return result
//   Path Gen: book → normalize → snprintf with prefix → return bool
//
// APUs (Available Processing Units):
//   - 6 public functions
//   - 1 private helper
//   - 1 static table (type_names)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────

// Internal helper: generate path with given prefix.
//
// Shared logic for KJV and WEB path generation.
// Normalizes book name (spaces → underscores) and formats path.
static bool path_generate_internal(const char* prefix, const char* book,
                                   uint8_t chapter, uint8_t verse,
                                   char* out, size_t outlen) {
    if (book == NULL || out == NULL || outlen == 0) {
        return false;
    }

    // Normalize book name (spaces → underscores)
    char norm_book[64];
    if (!scripture_path_normalize_book(book, norm_book, sizeof(norm_book))) {
        return false;
    }

    // Format: {prefix}{Book}/Chapter_{ch}/Verse_{vs}.txt
    int written = snprintf(out, outlen, "%s%s/Chapter_%u/Verse_%u.txt",
                           prefix, norm_book, (unsigned)chapter, (unsigned)verse);

    // Check for truncation
    if (written < 0 || (size_t)written >= outlen) {
        out[0] = '\0';  // Clear on failure
        return false;
    }

    return true;
}

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Using Public APIs section for all implementations]

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────

//--- Path Generation Functions ---

// Normalize a book name to filesystem format.
//
// Converts "1 Corinthians" → "1_Corinthians" (spaces to underscores).
bool scripture_path_normalize_book(const char* book, char* out, size_t outlen) {
    if (book == NULL || out == NULL || outlen == 0) {
        return false;
    }

    size_t len = strlen(book);
    if (len >= outlen) {
        return false;  // Won't fit
    }

    // Copy with space → underscore transformation
    for (size_t i = 0; i <= len; i++) {  // Include null terminator
        out[i] = (book[i] == ' ') ? '_' : book[i];
    }

    return true;
}

// Generate a KJV Scripture file path from book/chapter/verse.
//
// Produces: "word/scripture/KJV/{Book}/Chapter_{ch}/Verse_{vs}.txt"
bool scripture_path_kjv(const char* book, uint8_t chapter, uint8_t verse,
                        char* out, size_t outlen) {
    return path_generate_internal(SCRIPTURE_PATH_KJV_PREFIX, book,
                                  chapter, verse, out, outlen);
}

// Generate a WEB Scripture file path from book/chapter/verse.
//
// Produces: "word/scripture/WEB/{Book}/Chapter_{ch}/Verse_{vs}.txt"
bool scripture_path_web(const char* book, uint8_t chapter, uint8_t verse,
                        char* out, size_t outlen) {
    return path_generate_internal(SCRIPTURE_PATH_WEB_PREFIX, book,
                                  chapter, verse, out, outlen);
}

//--- Address Resolution Functions ---

// Resolve a packed verse address to its Scripture path.
//
// Variant-aware dispatch:
//   - KJV verse (low < 243): returns KJV_VERSE type, empty path
//   - WEB variant (low >= 243): generates WEB path using variant_location
//   - Invalid: returns INVALID type
scripture_path_result_t scripture_path_resolve(verse_addr_t addr) {
    scripture_path_result_t result;
    memset(&result, 0, sizeof(result));
    result.variant_index = -1;  // Default: not a variant

    // Check if this is a variant address
    if (verse_addr_is_variant(addr)) {
        // WEB variant - decode and generate path
        int idx = verse_addr_decode_variant(addr);
        if (idx < 0) {
            result.type = SCRIPTURE_PATH_INVALID;
            return result;
        }

        result.type = SCRIPTURE_PATH_WEB_VARIANT;
        result.variant_index = idx;

        // Get location from bible_rail
        variant_location_t loc = bible_rail_variant_location(idx);
        if (loc.book != NULL) {
            scripture_path_web(loc.book, loc.chapter, loc.verse,
                               result.path, sizeof(result.path));
        }
    } else {
        // KJV verse - we have the address but not the location
        // Caller needs ordinal→location lookup externally
        result.type = SCRIPTURE_PATH_KJV_VERSE;
        // Path remains empty - caller must use resolve_with_location
    }

    return result;
}

// Resolve a packed address with known location to its Scripture path.
//
// Full resolution when book/chapter/verse is already known.
scripture_path_result_t scripture_path_resolve_with_location(
    verse_addr_t addr, const char* book, uint8_t chapter, uint8_t verse) {
    scripture_path_result_t result;
    memset(&result, 0, sizeof(result));
    result.variant_index = -1;

    if (book == NULL) {
        result.type = SCRIPTURE_PATH_INVALID;
        return result;
    }

    if (verse_addr_is_variant(addr)) {
        // WEB variant
        result.type = SCRIPTURE_PATH_WEB_VARIANT;
        result.variant_index = verse_addr_decode_variant(addr);
        scripture_path_web(book, chapter, verse, result.path, sizeof(result.path));
    } else {
        // KJV verse
        result.type = SCRIPTURE_PATH_KJV_VERSE;
        scripture_path_kjv(book, chapter, verse, result.path, sizeof(result.path));
    }

    return result;
}

// Get the string name for a path type.
//
// Uses balanced ternary indexing: type + 1 → array index
const char* scripture_path_type_name(scripture_path_type_t type) {
    // Balanced ternary: -1, 0, +1 → index 0, 1, 2
    int index = (int)type + 1;
    if (index < 0 || index > 2) {
        return "UNKNOWN";
    }
    return type_names[index];
}

//--- Version-Aware Functions ---

// Generate a Scripture file path for a specific version.
//
// Version-aware dispatch: generates KJV or WEB path based on version parameter.
bool scripture_path_for_version(const char* book, uint8_t chapter, uint8_t verse,
                                scripture_version_t version, char* out, size_t outlen) {
    switch (version) {
        case SCRIPTURE_VERSION_KJV:
            return scripture_path_kjv(book, chapter, verse, out, outlen);
        case SCRIPTURE_VERSION_WEB:
            return scripture_path_web(book, chapter, verse, out, outlen);
        default:
            if (out != NULL && outlen > 0) {
                out[0] = '\0';
            }
            return false;
    }
}

// Get the string name for a Scripture version.
//
// Uses direct indexing: version 0, 1 → array index 0, 1
const char* scripture_path_version_name(scripture_version_t version) {
    if (version < 0 || version > 1) {
        return "UNKNOWN";
    }
    return version_names[version];
}

//--- Content Read/Decode Functions ---
// These functions complete the chain: path → file → encoded trit5

// Read a file at a given path and return its content encoded as trit5.
//
// Lower-level function: path → file read → trit5 encode
// Strips UTF-8 BOM if present (Scripture files have BOM).
scripture_text_result_t scripture_path_read_file(
    const char* path, uint8_t* out, size_t outlen) {
    scripture_text_result_t result = {SCRIPTURE_TEXT_ERROR, 0};

    if (path == NULL || out == NULL || outlen == 0) {
        return result;
    }

    // Open the file
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        return result;  // File not found or inaccessible
    }

    // Read file content into temporary buffer
    char buffer[4096];  // Max verse size we'll support
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, f);
    fclose(f);

    if (bytes_read == 0) {
        return result;  // Empty file or read error
    }
    buffer[bytes_read] = '\0';  // Null-terminate

    // Strip UTF-8 BOM if present (0xEF 0xBB 0xBF)
    // Scripture files consistently have BOM - handle it at the file layer
    char* content = buffer;
    if (bytes_read >= 3 &&
        (unsigned char)buffer[0] == 0xEF &&
        (unsigned char)buffer[1] == 0xBB &&
        (unsigned char)buffer[2] == 0xBF) {
        content = buffer + 3;  // Skip BOM
    }

    // Encode the content to trit5
    result = scripture_text_encode(content, out, outlen);

    return result;
}

// Read a verse and return its content encoded as trit5.
//
// Complete chain: book/ch/vs → path → file read → trit5 encode
scripture_text_result_t scripture_path_read_verse(
    const char* book, uint8_t chapter, uint8_t verse,
    scripture_version_t version, uint8_t* out, size_t outlen) {
    scripture_text_result_t result = {SCRIPTURE_TEXT_ERROR, 0};

    if (book == NULL || out == NULL || outlen == 0) {
        return result;
    }

    // Generate the path for this verse
    char path[SCRIPTURE_PATH_MAX_LEN];
    bool path_ok = scripture_path_for_version(book, chapter, verse, version,
                                               path, sizeof(path));
    if (!path_ok) {
        return result;
    }

    // Read and encode the file
    return scripture_path_read_file(path, out, outlen);
}

// Decode trit5 encoded content back to human-readable string.
//
// Wraps scripture_text_decode for API consistency.
scripture_text_result_t scripture_path_decode_content(
    const uint8_t* data, size_t len, char* out, size_t outlen) {
    // Direct pass-through to scripture_text_decode
    return scripture_text_decode(data, len, out, outlen);
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
//   gcc -c -Wall -Wextra -std=c99 -I../include scripture_path.c
//
// Test Verification:
//   See test/scripture_path_test.c for path generation tests

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Library source - linked into libtrit.a]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - paths written to caller buffers]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Scripture path resolution generates filesystem paths
// for Scripture lookup. Variant-aware—routes KJV vs WEB addresses.
// Uses balanced ternary enum: -1 (INVALID), 0 (KJV), +1 (WEB).

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new path generation functions
//   ✅ Add convenience helpers
//
// Modify with Care:
//   ⚠️ Path format strings (must match filesystem layout)
//   ⚠️ Function implementations (match header prototypes)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Balanced ternary enum values (-1, 0, +1)
//   ❌ Path prefix constants

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Header: include/scripture_path.h
// Test: test/scripture_path_test.c
// Dependencies: bible_rail.h, verse_addr.h
// Filesystem: word/scripture/KJV/, word/scripture/WEB/

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// The path to Scripture is never arbitrary—it follows divine order.
//
// "Thy word is a lamp unto my feet, and a light unto my path."
// — Psalm 119:105

// ============================================================================
// END CLOSING
// ============================================================================
