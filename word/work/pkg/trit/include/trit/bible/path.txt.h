// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Scripture Path Resolution (Variant-Aware File Lookup)
// Key: B-word-work-pkg-trit-include-scripture-path
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: bible_rail.h, verse_addr.h, scripture_text.h)
//   Depends on bible_rail.h for variant_location_t
//   Depends on verse_addr.h for verse_addr_t
//   Depends on scripture_text.h for encoding/decoding content
//
// derives_from: bereshit/word/research/bereshit/bereshit-trit-byte-encoding.adoc
// See: word/scripture/ for KJV and WEB directory structure
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_SCRIPTURE_PATH_H
#define BERESHIT_SCRIPTURE_PATH_H

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
// Role: Translates packed addresses to filesystem paths. Detects whether
//       an address is a KJV verse or WEB variant, generating the appropriate
//       path or marker. Bridges encoding to actual Scripture files.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-14
//   - Version: 0.1.0
//   - Modified: 2025-12-14 - Initial scripture path resolution
//
// # Purpose & Function
//
// Purpose: Generate filesystem paths for Scripture lookup from packed addresses.
//
// Core Design: Variant-aware dispatch—checks if address is KJV or WEB,
//              generates appropriate path. Book names convert to filesystem
//              format (spaces → underscores).
//
// Key Features:
//
//   - scripture_path_normalize_book: "1 Corinthians" → "1_Corinthians"
//   - scripture_path_kjv: book/ch/vs → KJV/{Book}/Chapter_{N}/Verse_{M}.txt
//   - scripture_path_resolve: verse_addr_t → path result with type detection
//   - scripture_path_type_name: enum → string for display
//   - scripture_path_read_verse: book/ch/vs → trit5 encoded content (CHAIN COMPLETE)
//   - scripture_path_read_file: path → trit5 encoded content
//   - scripture_path_decode_content: trit5 → human-readable string
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
//   - Standard Library: stdint.h, stddef.h, stdbool.h
//   - External: None
//   - Internal: bible_rail.h, verse_addr.h
//
// What Uses This:
//
//   - Libraries: Scripture lookup tools
//   - Commands: demo-math, verse resolution tools
//
// # Usage & Integration
//
// Import:
//
//    #include "trit/scripture_path.h"
//
// Integration Pattern:
//
//  1. Include scripture_path.h (includes bible_rail.h, verse_addr.h)
//  2. Use scripture_path_resolve to dispatch on address type
//  3. Use scripture_path_kjv/web for direct path generation
//  4. Check result.type to determine KJV_VERSE vs WEB_VARIANT
//
// Public API:
//
//    Path Generation:
//      scripture_path_normalize_book(book, out, len) → bool
//      scripture_path_kjv(book, ch, vs, out, len) → bool
//      scripture_path_web(book, ch, vs, out, len) → bool
//
//    Address Resolution:
//      scripture_path_resolve(addr) → scripture_path_result_t
//      scripture_path_resolve_with_location(addr, book, ch, vs) → result
//      scripture_path_type_name(type) → const char*
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
#include "trit/bible/rail.enc.h"     // variant_location_t, bible_rail_variant_location()
#include "trit/bible/verse.enc.h"     // verse_addr_t, verse_addr_is_variant()
#include "trit/bible/text.txt.h" // scripture_text_encode/decode, completes the chain

//--- Standard Library ---
#include <stdint.h>       // uint8_t
#include <stddef.h>       // size_t
#include <stdbool.h>      // bool

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Scripture Path Constants ---
// Maximum path buffer size (generous for long book names + path)
#define SCRIPTURE_PATH_MAX_LEN 256

// Path prefix for KJV Scripture (relative to bereshit root)
#define SCRIPTURE_PATH_KJV_PREFIX "word/scripture/KJV/"

// Path prefix for WEB Scripture (relative to bereshit root)
#define SCRIPTURE_PATH_WEB_PREFIX "word/scripture/WEB/"

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---

// Path type enumeration - what kind of Scripture reference is this?
// Uses balanced ternary: -1, 0, +1
typedef enum {
    SCRIPTURE_PATH_INVALID   = -1,  // Invalid address (decode failed)
    SCRIPTURE_PATH_KJV_VERSE =  0,  // Standard KJV verse (canonical anchor)
    SCRIPTURE_PATH_WEB_VARIANT = 1  // WEB textual variant (extension)
} scripture_path_type_t;

// Scripture version enumeration - which Bible translation?
// Balanced ternary: 0 (anchor), +1 (extension)
typedef enum {
    SCRIPTURE_VERSION_KJV = 0,  // King James Version (canonical anchor)
    SCRIPTURE_VERSION_WEB = 1   // World English Bible (accessibility extension)
} scripture_version_t;

//--- Building Blocks ---
// Uses verse_addr_t from verse_addr.h
// Uses variant_location_t from bible_rail.h

//--- Composed Types ---

// Scripture path result - contains resolved path and type.
//
// Returned by scripture_path_resolve() with:
//   - type: KJV_VERSE, WEB_VARIANT, or INVALID
//   - path: Generated filesystem path (or empty if unknown location)
//   - variant_index: If WEB_VARIANT, which one (0-12); -1 otherwise
typedef struct {
    scripture_path_type_t type;          // What kind of path this is
    char path[SCRIPTURE_PATH_MAX_LEN];   // Generated path (or empty)
    int variant_index;                   // If WEB_VARIANT, which one (0-12)
} scripture_path_result_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Path Generation Functions (src/scripture_path.c) ---

// Normalize a book name to filesystem format.
//
// Converts "1 Corinthians" → "1_Corinthians" (spaces to underscores).
// Copies result to `out` buffer. Returns true on success.
//
// Parameters:
//   book   - Book name (e.g., "Genesis", "1 Corinthians")
//   out    - Output buffer for normalized name
//   outlen - Size of output buffer
//
// Returns: true if successful, false if buffer too small or book is NULL
bool scripture_path_normalize_book(const char* book, char* out, size_t outlen);

// Generate a KJV Scripture file path from book/chapter/verse.
//
// Produces: "word/scripture/KJV/{Book}/Chapter_{ch}/Verse_{vs}.txt"
//
// Parameters:
//   book    - Book name (e.g., "Genesis", "1 Corinthians")
//   chapter - Chapter number (1-based)
//   verse   - Verse number (1-based)
//   out     - Output buffer for path
//   outlen  - Size of output buffer
//
// Returns: true if successful, false if buffer too small or invalid input
bool scripture_path_kjv(const char* book, uint8_t chapter, uint8_t verse,
                        char* out, size_t outlen);

// Generate a WEB Scripture file path from book/chapter/verse.
//
// Produces: "word/scripture/WEB/{Book}/Chapter_{ch}/Verse_{vs}.txt"
//
// Parameters:
//   book    - Book name (e.g., "Genesis", "1 Corinthians")
//   chapter - Chapter number (1-based)
//   verse   - Verse number (1-based)
//   out     - Output buffer for path
//   outlen  - Size of output buffer
//
// Returns: true if successful, false if buffer too small or invalid input
bool scripture_path_web(const char* book, uint8_t chapter, uint8_t verse,
                        char* out, size_t outlen);

//--- Address Resolution Functions (src/scripture_path.c) ---

// Resolve a packed verse address to its Scripture path.
//
// This is the variant-aware dispatch function:
//   - If addr is a KJV verse (low < 243): returns KJV_VERSE type
//   - If addr is a WEB variant (low >= 243): generates WEB path using variant_location
//   - If addr is invalid: returns INVALID type
//
// For KJV verses, the path requires external ordinal→location lookup,
// so this returns KJV_VERSE type but empty path. Use resolve_with_location
// when you already have the book/chapter/verse.
//
// For WEB variants, uses bible_rail_variant_location() to get location
// and generates the full path.
//
// Parameters:
//   addr - Packed verse address to resolve
//
// Returns: scripture_path_result_t with type, path, and variant_index
scripture_path_result_t scripture_path_resolve(verse_addr_t addr);

// Resolve a packed address with known location to its Scripture path.
//
// Full resolution when book/chapter/verse is already known.
// Uses verse_addr_is_variant() to determine KJV vs WEB path generation.
//
// Parameters:
//   addr    - Packed verse address
//   book    - Book name
//   chapter - Chapter number
//   verse   - Verse number
//
// Returns: scripture_path_result_t with type, path, and variant_index
scripture_path_result_t scripture_path_resolve_with_location(
    verse_addr_t addr, const char* book, uint8_t chapter, uint8_t verse);

// Get the string name for a path type.
//
// Returns: "INVALID", "KJV_VERSE", or "WEB_VARIANT"
const char* scripture_path_type_name(scripture_path_type_t type);

//--- Version-Aware Functions (src/scripture_path.c) ---

// Generate a Scripture file path for a specific version.
//
// Version-aware dispatch: generates KJV or WEB path based on version parameter.
// This is the primary function for version-aware Scripture lookup.
//
// Parameters:
//   book    - Book name (e.g., "Genesis", "1 Corinthians")
//   chapter - Chapter number (1-based)
//   verse   - Verse number (1-based)
//   version - Which translation (SCRIPTURE_VERSION_KJV or SCRIPTURE_VERSION_WEB)
//   out     - Output buffer for path
//   outlen  - Size of output buffer
//
// Returns: true if successful, false if buffer too small or invalid input
bool scripture_path_for_version(const char* book, uint8_t chapter, uint8_t verse,
                                scripture_version_t version, char* out, size_t outlen);

// Get the string name for a Scripture version.
//
// Returns: "KJV" or "WEB"
const char* scripture_path_version_name(scripture_version_t version);

//--- Content Read/Decode Functions (src/scripture_path.c) ---
// These functions complete the chain: path → file → encoded trit5

// Read a verse and return its content encoded as trit5.
//
// Complete chain: book/ch/vs → path → file read → trit5 encode
// This is the primary function for getting Scripture as ternary data.
//
// Parameters:
//   book    - Book name (e.g., "Genesis", "1 Corinthians")
//   chapter - Chapter number (1-based)
//   verse   - Verse number (1-based)
//   version - Which translation (SCRIPTURE_VERSION_KJV or SCRIPTURE_VERSION_WEB)
//   out     - Output buffer for encoded trit5 data
//   outlen  - Size of output buffer
//
// Returns: scripture_text_result_t with status and bytes_written
scripture_text_result_t scripture_path_read_verse(
    const char* book, uint8_t chapter, uint8_t verse,
    scripture_version_t version, uint8_t* out, size_t outlen);

// Read a file at a given path and return its content encoded as trit5.
//
// Lower-level function: path → file read → trit5 encode
// Use when you already have the path (e.g., from scripture_path_resolve).
//
// Parameters:
//   path   - Path to Scripture file (from scripture_path_kjv/web)
//   out    - Output buffer for encoded trit5 data
//   outlen - Size of output buffer
//
// Returns: scripture_text_result_t with status and bytes_written
scripture_text_result_t scripture_path_read_file(
    const char* path, uint8_t* out, size_t outlen);

// Decode trit5 encoded content back to human-readable string.
//
// Inverse of read functions: trit5 → human-readable text
// Wraps scripture_text_decode for API consistency.
//
// Parameters:
//   data   - Trit5 encoded data (from read_verse or read_file)
//   len    - Length of encoded data
//   out    - Output buffer for decoded string
//   outlen - Size of output buffer
//
// Returns: scripture_text_result_t with status and bytes_written
scripture_text_result_t scripture_path_decode_content(
    const uint8_t* data, size_t len, char* out, size_t outlen);

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
// NOTE: This header defines prototypes only. Functions implemented in src/scripture_path.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── scripture_path_normalize_book() → book name → filesystem format
//   ├── scripture_path_kjv()            → book/ch/vs → KJV file path
//   ├── scripture_path_web()            → book/ch/vs → WEB file path
//   ├── scripture_path_resolve()        → verse_addr_t → path result
//   ├── scripture_path_resolve_with_location() → addr + location → full path
//   └── scripture_path_type_name()      → type enum → string name
//
//   Helpers (Bottom Rungs - internal to scripture_path.c)
//   └── path_generate_internal() - shared path generation logic
//
// Baton Flow (Execution Paths):
//   Resolve: Entry → is_variant? → WEB path or KJV type → return result
//   Path Gen: Entry → normalize_book → snprintf path → return bool
//
// APUs (Available Processing Units):
//   - 6 functions total
//   - 1 helper (internal path generation)
//   - 6 public APIs
//
// Type Definitions:
//   ├── scripture_path_type_t (enum)
//   └── scripture_path_result_t (struct)
//
// Implementation Location:
//   All function implementations in: src/scripture_path.c
//
// Declared Units:
// - 1 enum (scripture_path_type_t)
// - 1 struct (scripture_path_result_t)
// - 6 function prototypes
// - 3 #define constants

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Internal helpers in .c file]

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// See SETUP "Function Prototypes" section above for complete declarations.
// Functions organized by purpose:
//   - Path Generation: normalize_book, kjv, web
//   - Address Resolution: resolve, resolve_with_location, type_name

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return safe defaults rather than error codes.
//   - NULL book → false for path functions
//   - Buffer too small → false
//   - Invalid address → INVALID type with empty path
//   - variant_index set to -1 for non-variants
//
// The type enum allows callers to distinguish KJV, WEB, and invalid addresses.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - Path Generation: normalize_book, kjv, web
//   - Address Resolution: resolve, resolve_with_location, type_name

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 -I. scripture_path.h
//   echo '#include "trit/scripture_path.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "trit/scripture_path.h"
//   int main(void) { return SCRIPTURE_PATH_MAX_LEN; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

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
// for Scripture lookup. Variant-aware—detects KJV vs WEB addresses
// and routes appropriately.

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
//   ⚠️ Function signatures (breaks consumers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Include guard (BERESHIT_SCRIPTURE_PATH_H)
//   ❌ Path type enum values (breaks compatibility)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Key dependencies:
//   - bible_rail.h (variant_location_t, variant_location())
//   - verse_addr.h (verse_addr_t, is_variant())
//
// Implementation: src/scripture_path.c
// Test: test/scripture_path_test.c
//
// Filesystem layout:
//   - word/scripture/KJV/{Book}/Chapter_{N}/Verse_{M}.txt
//   - word/scripture/WEB/{Book}/Chapter_{N}/Verse_{M}.txt

// ────────────────────────────────────────────────────────────────
// Version History
// ────────────────────────────────────────────────────────────────
//
// 0.1.0 (2025-12-14) - Initial scripture path resolution
//       - Path generation for KJV and WEB
//       - Variant-aware address resolution
//       - Book name normalization (spaces → underscores)

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// "Thy word is a lamp unto my feet, and a light unto my path."
// — Psalm 119:105
//
// The path to Scripture is never arbitrary—it follows divine order.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "trit/scripture_path.h"
//
// Generate KJV path:
//   char path[SCRIPTURE_PATH_MAX_LEN];
//   scripture_path_kjv("Genesis", 1, 1, path, sizeof(path));
//   // path = "word/scripture/KJV/Genesis/Chapter_1/Verse_1.txt"
//
// Resolve variant address:
//   verse_addr_t addr = verse_addr_encode_variant(0);
//   scripture_path_result_t result = scripture_path_resolve(addr);
//   if (result.type == SCRIPTURE_PATH_WEB_VARIANT) {
//       printf("Variant %d: %s\n", result.variant_index, result.path);
//   }
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 -I../include your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_SCRIPTURE_PATH_H


