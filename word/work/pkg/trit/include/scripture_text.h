// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Scripture Text Encoding (Character ↔ Trit5 Mapping)
// Key: B-word-work-pkg-trit-include-scripture-text
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: STANDALONE
//   No internal dependencies - pure encoding/decoding operations
//
// derives_from: bereshit/word/core/bible/scripture-text.toml
// See: word/core/bible/scripture-text.toml for encoding specification
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_SCRIPTURE_TEXT_H
#define BERESHIT_SCRIPTURE_TEXT_H

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
// Role: Character-to-trit5 encoding for Scripture text storage. Maps ASCII
//       characters (0-127) directly to trit5 values. Extended characters
//       (128-242) for typography and original languages.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-14
//   - Version: 0.1.0
//   - Modified: 2025-12-14 - Initial character encoding
//
// # Purpose & Function
//
// Purpose: Encode and decode Scripture text for binary storage.
//
// Core Design: Direct ASCII mapping (0-127), extended table (128-242).
//              NUL-terminated verse strings. Bidirectional: encode + decode.
//
// Key Features:
//
//   - scripture_text_char_to_trit5: char → trit5 value
//   - scripture_text_trit5_to_char: trit5 value → char
//   - scripture_text_encode: text string → trit5 byte array
//   - scripture_text_decode: trit5 byte array → text string
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
//   - Standard Library: stdint.h, stddef.h, stdbool.h
//   - External: None
//   - Internal: None (standalone)
//
// What Uses This:
//
//   - Libraries: Scripture storage encoder/decoder
//   - Commands: demo-math, Scripture processing tools
//
// # Usage & Integration
//
// Import:
//
//    #include "scripture_text.h"
//
// Integration Pattern:
//
//  1. Include scripture_text.h (standalone, no other deps)
//  2. Use scripture_text_encode to convert text → trit5 bytes
//  3. Use scripture_text_decode to convert trit5 bytes → text
//  4. Direct char mapping via scripture_text_char_to_trit5
//
// Public API:
//
//    Character Encoding:
//      scripture_text_char_to_trit5(c) → uint8_t
//      scripture_text_trit5_to_char(v) → char
//
//    String Encoding:
//      scripture_text_encode(text, out, outlen) → size_t
//      scripture_text_decode(data, len, out, outlen) → size_t
//
//    Validation:
//      scripture_text_is_valid_trit5(v) → bool
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// [OMIT: Header file - type definitions only, no executable code]
// [OMIT: No health scoring - types don't track health]

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Includes → Defines → Types → Function Prototypes → Extern State

// ────────────────────────────────────────────────────────────────
// Includes
// ────────────────────────────────────────────────────────────────

//--- Project Headers ---
// [None - standalone module]

//--- Standard Library ---
#include <stdint.h>       // uint8_t
#include <stddef.h>       // size_t
#include <stdbool.h>      // bool

//--- Generated Headers ---
#include "generated/scripture_text.gen.h"  // TOML-derived constants

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Scripture Text Constants ---

// Maximum trit5 value (3^5 - 1 = 242)
#define SCRIPTURE_TEXT_TRIT5_MAX 242

// ASCII range boundary (direct mapping 0-127)
#define SCRIPTURE_TEXT_ASCII_MAX 127

// Extended character range start
#define SCRIPTURE_TEXT_EXTENDED_START 128

// Replacement character for unknown input (ASCII '?')
#define SCRIPTURE_TEXT_REPLACEMENT 63

// NUL terminator value
#define SCRIPTURE_TEXT_NUL 0

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---

// Encoding result status - what happened during encode/decode?
// Uses balanced ternary: -1 (error), 0 (success), +1 (truncated)
typedef enum {
    SCRIPTURE_TEXT_ERROR     = -1,  // Encoding failed (NULL input, etc.)
    SCRIPTURE_TEXT_SUCCESS   =  0,  // Encoding completed fully
    SCRIPTURE_TEXT_TRUNCATED =  1   // Output buffer too small, truncated
} scripture_text_status_t;

//--- Building Blocks ---
// Uses uint8_t for trit5 values (0-242)
// Uses char for text characters

//--- Composed Types ---

// Encoding result - contains status and bytes written.
//
// Returned by scripture_text_encode() with:
//   - status: SUCCESS, TRUNCATED, or ERROR
//   - bytes_written: Number of bytes written to output buffer
typedef struct {
    scripture_text_status_t status;  // What happened
    size_t bytes_written;            // How many bytes were written
} scripture_text_result_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Character Encoding Functions (src/scripture_text.c) ---

// Convert a single character to its trit5 value.
//
// ASCII characters (0-127) map directly. Extended characters (128-242)
// use lookup table. Unknown characters return SCRIPTURE_TEXT_REPLACEMENT.
//
// Parameters:
//   c - Character to encode
//
// Returns: trit5 value (0-242) or SCRIPTURE_TEXT_REPLACEMENT for unknown
uint8_t scripture_text_char_to_trit5(char c);

// Convert a trit5 value to its character.
//
// ASCII values (0-127) map directly. Extended values (128-242) use
// reverse lookup table. Invalid values return '?'.
//
// Parameters:
//   value - trit5 value to decode
//
// Returns: Decoded character or '?' for invalid values
char scripture_text_trit5_to_char(uint8_t value);

//--- String Encoding Functions (src/scripture_text.c) ---

// Encode a text string to trit5 byte array.
//
// Converts each character to its trit5 value and writes to output buffer.
// Adds NUL terminator at end. Returns result with status and bytes written.
//
// Parameters:
//   text   - NUL-terminated text string to encode
//   out    - Output buffer for trit5 bytes
//   outlen - Size of output buffer
//
// Returns: scripture_text_result_t with status and bytes_written
scripture_text_result_t scripture_text_encode(const char* text,
                                               uint8_t* out, size_t outlen);

// Decode a trit5 byte array to text string.
//
// Converts each trit5 value to its character and writes to output buffer.
// Stops at NUL (0) or when len bytes processed. Adds NUL terminator.
//
// Parameters:
//   data   - trit5 byte array to decode
//   len    - Number of bytes to decode (or 0 to read until NUL)
//   out    - Output buffer for text
//   outlen - Size of output buffer
//
// Returns: scripture_text_result_t with status and bytes_written
scripture_text_result_t scripture_text_decode(const uint8_t* data, size_t len,
                                               char* out, size_t outlen);

//--- Validation Functions (src/scripture_text.c) ---

// Check if a value is a valid trit5 value.
//
// Valid range is 0-242 (3^5 - 1).
//
// Parameters:
//   value - Value to check
//
// Returns: true if valid trit5 value, false otherwise
bool scripture_text_is_valid_trit5(uint8_t value);

// Get the string name for an encoding status.
//
// Returns: "ERROR", "SUCCESS", or "TRUNCATED"
const char* scripture_text_status_name(scripture_text_status_t status);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables needed - pure functions]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: This header defines prototypes only. Functions implemented in src/scripture_text.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── scripture_text_char_to_trit5() → char → trit5 value
//   ├── scripture_text_trit5_to_char() → trit5 value → char
//   ├── scripture_text_encode()        → text → trit5 bytes
//   ├── scripture_text_decode()        → trit5 bytes → text
//   ├── scripture_text_is_valid_trit5() → value → bool
//   └── scripture_text_status_name()   → status enum → string
//
//   Helpers (Bottom Rungs - internal to scripture_text.c)
//   └── [None needed - direct mapping]
//
// Baton Flow (Execution Paths):
//   Encode: text → char_to_trit5 → bytes → NUL terminate
//   Decode: bytes → trit5_to_char → text → NUL terminate
//
// APUs (Available Processing Units):
//   - 6 public APIs (all pure functions)
//   - No internal helpers needed
//
// Type Definitions:
//   ├── scripture_text_status_t (enum)
//   └── scripture_text_result_t (struct)
//
// Implementation Location:
//   All function implementations in: src/scripture_text.c
//
// Declared Units:
// - 1 enum (scripture_text_status_t)
// - 1 struct (scripture_text_result_t)
// - 6 function prototypes
// - 5 #define constants

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Direct mapping - no helpers needed]

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// See SETUP "Function Prototypes" section above for complete declarations.
// Functions organized by purpose:
//   - Character Encoding: char_to_trit5, trit5_to_char
//   - String Encoding: encode, decode
//   - Validation: is_valid_trit5, status_name

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return result structs with status codes.
//   - NULL input → ERROR status
//   - Buffer too small → TRUNCATED status
//   - Unknown character → REPLACEMENT value (not error)
//   - Invalid trit5 → '?' character (not error)
//
// The status enum in result allows callers to distinguish full, truncated, and error cases.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - Character Encoding: char_to_trit5, trit5_to_char
//   - String Encoding: encode, decode
//   - Validation: is_valid_trit5, status_name

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 -I. scripture_text.h
//   echo '#include "scripture_text.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "scripture_text.h"
//   int main(void) { return SCRIPTURE_TEXT_TRIT5_MAX; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - all output to caller buffers]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Scripture text encoding converts characters to trit5
// values (0-242) for binary storage. ASCII maps directly. Extended
// characters use lookup table. Bidirectional: encode and decode.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new encoding helper functions
//   ✅ Extend character table (128-242 range)
//
// Modify with Care:
//   ⚠️ ASCII mapping (must remain direct 0-127)
//   ⚠️ Function signatures (breaks consumers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Include guard (BERESHIT_SCRIPTURE_TEXT_H)
//   ❌ Status enum values (breaks compatibility)
//   ❌ TRIT5_MAX value (3^5 - 1 = 242)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Key dependencies:
//   - None (standalone module)
//
// Specification: word/core/bible/scripture-text.toml
// Implementation: src/scripture_text.c
// Test: test/scripture_text_test.c
//
// Encoding scheme:
//   - ASCII (0-127): Direct mapping
//   - Extended (128-242): Lookup table
//   - Spare (243-255): Used by verse_addr for variants

// ────────────────────────────────────────────────────────────────
// Version History
// ────────────────────────────────────────────────────────────────
//
// 0.1.0 (2025-12-14) - Initial character encoding
//       - Direct ASCII mapping (0-127)
//       - Extended character table (128-242)
//       - Encode/decode string functions
//       - Validation helpers

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// "The words of the LORD are pure words: as silver tried in a furnace
//  of earth, purified seven times." — Psalm 12:6
//
// Every letter matters. Encoding preserves them all.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "scripture_text.h"
//
// Encode text:
//   uint8_t encoded[256];
//   scripture_text_result_t result = scripture_text_encode("Hello", encoded, sizeof(encoded));
//   if (result.status == SCRIPTURE_TEXT_SUCCESS) {
//       // encoded = {72, 101, 108, 108, 111, 0}
//   }
//
// Decode text:
//   char decoded[256];
//   result = scripture_text_decode(encoded, result.bytes_written, decoded, sizeof(decoded));
//   // decoded = "Hello"
//
// Single character:
//   uint8_t trit5 = scripture_text_char_to_trit5('A');  // = 65
//   char c = scripture_text_trit5_to_char(65);           // = 'A'
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 -I../include your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_SCRIPTURE_TEXT_H


