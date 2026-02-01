// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Scripture Text Encoding Implementation
// Key: B-word-work-pkg-trit-src-scripture-text
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: scripture_text.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/work/pkg/trit/include/scripture_text.h
// See: word/core/bible/scripture-text.toml for encoding specification
//
// ═══════════════════════════════════════════════════════════════════════════

// Scripture text encoding: character ↔ trit5 mapping for Bible text storage.
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
// Principle: Every letter of Scripture matters. Character encoding preserves
//            the Word exactly—no loss, no corruption. ASCII maps directly
//            to trit5 for efficiency, extended characters for special cases.
//
// Anchor: "The words of the LORD are pure words: as silver tried in a furnace
//          of earth, purified seven times." — Psalm 12:6
//
// # CPI-SI Identity
//
// Component Type: Encoder (transforms text to binary representation)
//
// Role: Implements character-to-trit5 encoding functions. Direct ASCII mapping
//       for efficiency, extended table for typography and original languages.
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
//   - 0.1.0 (2025-12-14) - Initial character encoding
//
// # Purpose & Function
//
// Purpose: Implement character encoding for Scripture text storage.
//
// Core Design: Direct ASCII mapping (0-127), extended lookup table (128-242).
//              NUL-terminated verse strings. Bidirectional encode/decode.
//
// Key Features:
//
//   - Character encoding: char → trit5 value (0-242)
//   - Character decoding: trit5 value → char
//   - String encoding: text → trit5 byte array
//   - Safety: buffer overflow protection, NULL checks
//
// Philosophy: Every letter matters. Encoding preserves them all.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: string.h (memset)
//   - External: None
//   - Internal: scripture_text.h (types and prototypes)
//
// What Uses This:
//
//   - Libraries: libtrit.a consumers
//   - Commands: demo-math, Scripture encoding tools
//
// Integration Points:
//
//   - Link with libtrit.a
//   - Include scripture_text.h for type definitions
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
#include "trit/bible/text.txt.h"  // Types and prototypes

//--- Standard Library ---
#include <string.h>      // memset

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All constants defined in scripture_text.h]

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────

// Status name strings for scripture_text_status_name().
// Balanced ternary: -1, 0, +1 → index by adding 1 (0, 1, 2)
static const char* status_names[] = {
    "ERROR",      // SCRIPTURE_TEXT_ERROR = -1     → index 0
    "SUCCESS",    // SCRIPTURE_TEXT_SUCCESS = 0    → index 1
    "TRUNCATED"   // SCRIPTURE_TEXT_TRUNCATED = 1  → index 2
};

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All types defined in scripture_text.h]

// ────────────────────────────────────────────────────────────────
// Function Prototypes (Private)
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No private helpers needed - direct mapping]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Function implementations organized by purpose:
// 1. Character Encoding: char_to_trit5, trit5_to_char
// 2. String Encoding: encode, decode
// 3. Validation: is_valid_trit5, status_name

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Implementation Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   scripture_text_char_to_trit5 (standalone)
//   scripture_text_trit5_to_char (standalone)
//
//   scripture_text_encode
//   └── Uses scripture_text_char_to_trit5
//
//   scripture_text_decode
//   └── Uses scripture_text_trit5_to_char
//
//   scripture_text_is_valid_trit5 (standalone)
//   scripture_text_status_name (standalone)
//
// Baton Flow (Execution Paths):
//   Encode: text → char_to_trit5 each char → write bytes → NUL terminate
//   Decode: bytes → trit5_to_char each byte → write chars → NUL terminate
//
// APUs (Available Processing Units):
//   - 6 public functions (all pure)
//   - 1 static table (status_names)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Direct mapping - no helpers needed]

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Using Public APIs section for all implementations]

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────

//--- Character Encoding Functions ---

// Convert a single character to its trit5 value.
//
// ASCII characters (0-127) map directly. Characters > 127 that are
// within trit5 range (128-242) pass through. Others get replacement.
uint8_t scripture_text_char_to_trit5(char c) {
    uint8_t code = (uint8_t)c;

    // ASCII range: direct mapping
    if (code <= SCRIPTURE_TEXT_ASCII_MAX) {
        return code;
    }

    // Extended range: pass through if valid trit5
    if (code <= SCRIPTURE_TEXT_TRIT5_MAX) {
        return code;
    }

    // Invalid: return replacement character ('?')
    return SCRIPTURE_TEXT_REPLACEMENT;
}

// Convert a trit5 value to its character.
//
// Values 0-127 map directly to ASCII. Values 128-242 pass through
// as extended characters. Values > 242 return '?'.
char scripture_text_trit5_to_char(uint8_t value) {
    // Valid trit5 range: direct mapping
    if (value <= SCRIPTURE_TEXT_TRIT5_MAX) {
        return (char)value;
    }

    // Invalid: return '?'
    return '?';
}

//--- String Encoding Functions ---

// Encode a text string to trit5 byte array.
//
// Converts each character to its trit5 value and writes to output buffer.
// Adds NUL terminator at end. Returns result with status and bytes written.
scripture_text_result_t scripture_text_encode(const char* text,
                                               uint8_t* out, size_t outlen) {
    scripture_text_result_t result;
    memset(&result, 0, sizeof(result));

    // NULL check
    if (text == NULL || out == NULL) {
        result.status = SCRIPTURE_TEXT_ERROR;
        return result;
    }

    // Empty buffer
    if (outlen == 0) {
        result.status = SCRIPTURE_TEXT_ERROR;
        return result;
    }

    size_t i = 0;
    while (text[i] != '\0') {
        // Check if we have room (need space for NUL terminator)
        if (i >= outlen - 1) {
            out[i] = SCRIPTURE_TEXT_NUL;
            result.bytes_written = i + 1;
            result.status = SCRIPTURE_TEXT_TRUNCATED;
            return result;
        }

        out[i] = scripture_text_char_to_trit5(text[i]);
        i++;
    }

    // Add NUL terminator
    out[i] = SCRIPTURE_TEXT_NUL;
    result.bytes_written = i + 1;
    result.status = SCRIPTURE_TEXT_SUCCESS;

    return result;
}

// Decode a trit5 byte array to text string.
//
// Converts each trit5 value to its character and writes to output buffer.
// Stops at NUL (0) or when len bytes processed. Adds NUL terminator.
scripture_text_result_t scripture_text_decode(const uint8_t* data, size_t len,
                                               char* out, size_t outlen) {
    scripture_text_result_t result;
    memset(&result, 0, sizeof(result));

    // NULL check
    if (data == NULL || out == NULL) {
        result.status = SCRIPTURE_TEXT_ERROR;
        return result;
    }

    // Empty buffer
    if (outlen == 0) {
        result.status = SCRIPTURE_TEXT_ERROR;
        return result;
    }

    size_t i = 0;
    while (i < len || len == 0) {
        // Stop at NUL terminator
        if (data[i] == SCRIPTURE_TEXT_NUL) {
            break;
        }

        // Check if we have room (need space for NUL terminator)
        if (i >= outlen - 1) {
            out[i] = '\0';
            result.bytes_written = i + 1;
            result.status = SCRIPTURE_TEXT_TRUNCATED;
            return result;
        }

        out[i] = scripture_text_trit5_to_char(data[i]);
        i++;

        // If len == 0, we're reading until NUL, don't check bounds
        if (len == 0) {
            continue;
        }
    }

    // Add NUL terminator
    out[i] = '\0';
    result.bytes_written = i + 1;
    result.status = SCRIPTURE_TEXT_SUCCESS;

    return result;
}

//--- Validation Functions ---

// Check if a value is a valid trit5 value.
//
// Valid range is 0-242 (3^5 - 1).
bool scripture_text_is_valid_trit5(uint8_t value) {
    return value <= SCRIPTURE_TEXT_TRIT5_MAX;
}

// Get the string name for an encoding status.
//
// Uses balanced ternary indexing: status + 1 → array index
const char* scripture_text_status_name(scripture_text_status_t status) {
    // Balanced ternary: -1, 0, +1 → index 0, 1, 2
    int index = (int)status + 1;
    if (index < 0 || index > 2) {
        return "UNKNOWN";
    }
    return status_names[index];
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
//   gcc -c -Wall -Wextra -std=c99 -I../include scripture_text.c
//
// Test Verification:
//   See test/scripture_text_test.c for encoding tests

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
// Quick summary: Scripture text encoding implements character ↔ trit5
// mapping for Bible text storage. Direct ASCII mapping (0-127), extended
// table (128-242). Uses balanced ternary status: -1 (ERROR), 0 (SUCCESS), +1 (TRUNCATED).

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new character mapping functions
//   ✅ Add convenience helpers
//
// Modify with Care:
//   ⚠️ ASCII mapping boundaries (must maintain direct mapping)
//   ⚠️ Function implementations (match header prototypes)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Balanced ternary enum values (-1, 0, +1)
//   ❌ TRIT5_MAX constant (242 is 3^5 - 1)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Header: include/scripture_text.h
// Test: test/scripture_text_test.c
// Specification: word/core/bible/scripture-text.toml
// Dependencies: None (standalone module)

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// Every letter of Scripture matters. Character encoding preserves them all.
//
// "The words of the LORD are pure words: as silver tried in a furnace
//  of earth, purified seven times." — Psalm 12:6

// ============================================================================
// END CLOSING
// ============================================================================
