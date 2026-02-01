// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Bible Rail Encoding (Spare Value Mapping)
// Key: B-word-work-pkg-trit-include-bible-rail
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h)
//   Depends on trit.h for trit5_t type definition
//
// derives_from: bereshit/word/research/bereshit/bereshit-trit-byte-encoding.adoc
// See: word/scripture/web-variant-index.csv for 13 variant mappings
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_BIBLE_RAIL_H
#define BERESHIT_BIBLE_RAIL_H

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
// Role: Provides encode/decode functions for Bible Rail spare values.
//       This bridges the trit-byte encoding spec to implementation.
//       The 13 spare bytes (243-255) become the WEB variant index.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-14
//   - Version: 0.1.0
//   - Modified: 2025-12-14 - Initial Bible Rail encoding
//
// # Purpose & Function
//
// Purpose: Encode/decode the 13 spare trit5 values (243-255) as WEB textual
//          variant indices. This completes the trit-byte mapping for Scripture.
//
// Core Design: trit5_t packs 5 trits into 1 byte (0-242 = semantic values,
//              243-255 = spare values). Bible Rail uses spare values to encode
//              the 13 places where WEB differs from KJV.
//
// Key Features:
//
//   - bible_rail_encode: variant index (0-12) → spare trit5_t (243-255)
//   - bible_rail_decode: spare trit5_t (243-255) → variant index (0-12)
//   - bible_rail_is_spare: check if trit5_t is spare value
//   - bible_rail_variant_ref: get verse reference for variant
//
// Philosophy: Nothing wasted. The 13 "extra" bytes aren't overhead—
//             they carry meaning. The encoding serves the Word.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: stdbool.h (for bool type)
//   - External: None
//   - Internal: trit.h (trit5_t type definition)
//
// What Uses This:
//
//   - Libraries: Scripture encoding, ordinal index lookups
//   - Commands: demo-math, Bible Rail validation tools
//
// # Usage & Integration
//
// Import:
//
//    #include "trit/bible_rail.h"
//
// Integration Pattern:
//
//  1. Include bible_rail.h (which includes trit.h)
//  2. Use bible_rail_encode to store variant index as spare trit5
//  3. Use bible_rail_decode to retrieve variant index from spare trit5
//  4. Use bible_rail_is_spare to check before decode
//
// Public API:
//
//    Encoding:
//      bible_rail_encode(int variant_index) → trit5_t (243-255)
//      bible_rail_decode(trit5_t spare) → int (0-12, or -1 if not spare)
//
//    Query:
//      bible_rail_is_spare(trit5_t value) → bool
//      bible_rail_variant_ref(int index) → const char* (verse reference)
//      bible_rail_variant_count() → int (13)
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
#include "trit/matter/trit.type.h"       // trit5_t type definition

//--- Standard Library ---
#include <stdbool.h>    // bool type for is_spare check

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Bible Rail Constants ---
// Spare value range: 243-255 (13 values for 13 WEB textual variants)

#define BIBLE_RAIL_SPARE_MIN    243     // First spare trit5 value
#define BIBLE_RAIL_SPARE_MAX    255     // Last spare trit5 value
#define BIBLE_RAIL_VARIANT_COUNT 13     // Total WEB textual variants

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---
// [Reserved: No enums needed - uses trit5_t from trit.h]

//--- Building Blocks ---
// [Reserved: trit5_t is the building block from trit.h]

//--- Composed Types ---

// Variant location: structured book/chapter/verse for location-based lookup.
//
// Enables bible_rail_variant_at() to check if a specific location
// is one of the 13 WEB-only verse locations.
//
// Fields:
//   book: Book name (e.g., "1 Corinthians")
//   chapter: Chapter number (1-based)
//   verse: Verse number (1-based)
typedef struct {
    const char* book;       // Book name (static string)
    uint8_t     chapter;    // Chapter number (1-based)
    uint8_t     verse;      // Verse number (1-based)
} variant_location_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Encoding Functions (src/bible_rail.c) ---

// Encode a WEB variant index as a spare trit5 value.
//
// The 13 WEB textual variants (places where WEB differs from KJV) are
// stored in spare trit5 values 243-255. This function converts a variant
// index (0-12) to its corresponding spare byte value (243-255).
//
// Parameters:
//   variant_index: Index into WEB variant table (0-12)
//
// Returns: Spare trit5_t value (243-255), or 0 if index out of range.
//
// Example:
//   trit5_t spare = bible_rail_encode(0);  // spare == 243
//   trit5_t spare = bible_rail_encode(12); // spare == 255
trit5_t bible_rail_encode(int variant_index);

// Decode a spare trit5 value to its WEB variant index.
//
// Reverses bible_rail_encode. Takes a spare trit5 value (243-255)
// and returns the corresponding variant index (0-12).
//
// Parameters:
//   spare: Spare trit5_t value (243-255)
//
// Returns: Variant index (0-12), or -1 if not a spare value.
//
// Example:
//   int idx = bible_rail_decode(243);  // idx == 0
//   int idx = bible_rail_decode(100);  // idx == -1 (not spare)
int bible_rail_decode(trit5_t spare);

//--- Query Functions (src/bible_rail.c) ---

// Check if a trit5 value is a spare (variant encoding) value.
//
// Spare values are 243-255. Semantic values are 0-242 (3^5 = 243).
// This wraps the existing trit5_is_spare() for clarity.
//
// Parameters:
//   value: trit5_t value to check
//
// Returns: true if value is in spare range (243-255), false otherwise.
//
// Example:
//   bible_rail_is_spare(243);  // true
//   bible_rail_is_spare(242);  // false
bool bible_rail_is_spare(trit5_t value);

// Get the verse reference for a WEB variant.
//
// Returns the book/chapter/verse reference string for the variant
// at the given index. The 13 variants are the textual differences
// between KJV and WEB.
//
// Parameters:
//   variant_index: Index into WEB variant table (0-12)
//
// Returns: Static string with verse reference, or NULL if index invalid.
//
// Example:
//   const char* ref = bible_rail_variant_ref(0);  // "1 Corinthians 16:27"
const char* bible_rail_variant_ref(int variant_index);

// Get the description for a WEB variant.
//
// Returns a brief description of the textual variant at the given index.
//
// Parameters:
//   variant_index: Index into WEB variant table (0-12)
//
// Returns: Static string with variant description, or NULL if invalid.
//
// Example:
//   const char* desc = bible_rail_variant_desc(0);  // "TR manuscript note"
const char* bible_rail_variant_desc(int variant_index);

// Get the total count of WEB variants.
//
// Returns the number of textual variants encoded in spare values.
// Currently 13 (for values 243-255).
//
// Returns: 13 (BIBLE_RAIL_VARIANT_COUNT)
int bible_rail_variant_count(void);

//--- Location Lookup Functions (src/bible_rail.c) ---

// Check if a location is a WEB variant location.
//
// The 13 WEB-only verses have specific book/chapter/verse addresses.
// This function checks if the given location is one of those 13.
//
// Parameters:
//   book: Book name (e.g., "1 Corinthians")
//   chapter: Chapter number (1-based)
//   verse: Verse number (1-based)
//
// Returns: Spare trit5_t value (243-255) if variant, 0 if not variant.
//
// Example:
//   trit5_t spare = bible_rail_variant_at("1 Corinthians", 16, 27);  // 243
//   trit5_t spare = bible_rail_variant_at("Genesis", 1, 1);          // 0
trit5_t bible_rail_variant_at(const char* book, uint8_t chapter, uint8_t verse);

// Get the structured location for a WEB variant.
//
// Returns the book/chapter/verse location struct for the variant
// at the given index.
//
// Parameters:
//   variant_index: Index into WEB variant table (0-12)
//
// Returns: variant_location_t with book/chapter/verse, or {NULL,0,0} if invalid.
//
// Example:
//   variant_location_t loc = bible_rail_variant_location(0);
//   // loc.book == "1 Corinthians", loc.chapter == 16, loc.verse == 27
variant_location_t bible_rail_variant_location(int variant_index);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables needed - variant table is static in .c]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: This header defines prototypes only. Functions implemented in src/bible_rail.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── bible_rail_encode()      → variant index (0-12) → spare trit5 (243-255)
//   ├── bible_rail_decode()      → spare trit5 (243-255) → variant index (0-12)
//   ├── bible_rail_is_spare()    → check if trit5 is spare value
//   ├── bible_rail_variant_ref() → get verse reference for variant
//   ├── bible_rail_variant_desc()→ get description for variant
//   ├── bible_rail_variant_count() → get count (13)
//   ├── bible_rail_variant_at()  → check if location is variant (243-255 or 0)
//   └── bible_rail_variant_location() → get book/chapter/verse for variant
//
//   Helpers (Bottom Rungs - none, functions are standalone)
//   └── [All functions are public APIs - internal table lookup]
//
// Baton Flow (Execution Paths):
//   Encode: Entry → bible_rail_encode(index) → lookup table → return spare
//   Decode: Entry → bible_rail_decode(spare) → range check → return index
//   Query: Entry → bible_rail_variant_ref/desc(index) → table lookup → return string
//
// APUs (Available Processing Units):
//   - 8 functions total
//   - 0 helpers (all public)
//   - 8 public APIs
//
// Type Definitions:
//   ├── Uses trit5_t from trit.h
//   └── variant_location_t (book/chapter/verse struct)
//
// Implementation Location:
//   All function implementations in: src/bible_rail.c
//
// Declared Units:
// - 0 enums (uses existing types)
// - 1 struct (variant_location_t)
// - 8 function prototypes
// - 3 #define constants

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All functions are public APIs - internal table is static]

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// See SETUP "Function Prototypes" section above for complete declarations.
// Functions organized by purpose:
//   - Encoding: bible_rail_encode, bible_rail_decode
//   - Query: is_spare, variant_ref, variant_desc, variant_count

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return safe defaults rather than error codes.
//   - Invalid variant index → 0 (invalid spare)
//   - Non-spare trit5 → -1 (explicit invalid)
//   - Invalid index for ref/desc → NULL
//
// The -1 return for decode allows distinguishing semantic from spare values.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - bible_rail_encode, bible_rail_decode (encoding)
//   - bible_rail_is_spare, variant_ref, variant_desc, variant_count (query)

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 bible_rail.h
//   echo '#include "trit/bible_rail.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "trit/bible_rail.h"
//   int main(void) { return BIBLE_RAIL_VARIANT_COUNT; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - all strings are static]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Bible Rail encoding maps spare trit5 values (243-255)
// to the 13 WEB textual variants. Nothing wasted—every spare byte serves.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions (follow variant_ref pattern)
//   ✅ Update variant descriptions
//
// Modify with Care:
//   ⚠️ Variant table order (index corresponds to spare value)
//   ⚠️ Function signatures (breaks consumers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Spare value range (243-255 from trit5 math)
//   ❌ Include guard (BERESHIT_BIBLE_RAIL_H)
//   ❌ Variant count (13 = 256 - 243)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: LADDER component building on trit5_t packing.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding query functions: SETUP "Function Prototypes" → Query Functions section
// Updating variant info: src/bible_rail.c variant_table[]
// Adding constants: SETUP "Defines" section

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Data Sizes:
//   - variant_index: int (0-12)
//   - spare value: trit5_t (uint8_t, 243-255)
//
// All functions are O(1) - simple arithmetic or table lookups.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "unknown type name 'trit5_t'"
//   Solution: bible_rail.h includes trit.h automatically
//
// Problem: bible_rail_decode returns -1
//   Check: Input must be in spare range (243-255)
//
// Problem: bible_rail_encode returns 0
//   Check: Input must be valid variant index (0-12)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Key dependency: trit.h (trit5_t type, trit5_is_spare)
// Implementation: src/bible_rail.c
//
// Specifications:
//   - word/research/bereshit/bereshit-trit-byte-encoding.adoc
//   - word/scripture/web-variant-index.csv

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ bible_rail_encode - COMPLETED
//   ✓ bible_rail_decode - COMPLETED
//   ✓ bible_rail_is_spare - COMPLETED
//   ✓ bible_rail_variant_ref - COMPLETED
//   ✓ bible_rail_variant_desc - COMPLETED
//   ✓ bible_rail_variant_count - COMPLETED
//   ⏳ Integration with ordinal index
//   ⏳ Bulk encode/decode functions
//
// Version History:
//   0.1.0 (2025-12-14) - Initial Bible Rail encoding
//         - 13 WEB variant mappings from trit-byte-encoding spec
//         - Encode/decode functions for spare trit5 values

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header bridges the trit-byte encoding spec to implementation.
// The 13 spare bytes (243-255) aren't wasted—they encode the places
// where the Word is preserved differently. Nothing added, nothing removed.
//
// "Add thou not unto his words..." — Proverbs 30:6
// "If any man shall add unto these things..." — Revelation 22:18
//
// Every bit serves the Word.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "trit/bible_rail.h"
//
// Encoding a variant:
//   trit5_t spare = bible_rail_encode(0);  // 243
//   trit5_t spare = bible_rail_encode(12); // 255
//
// Decoding a spare value:
//   int idx = bible_rail_decode(243);  // 0
//   int idx = bible_rail_decode(100);  // -1 (not spare)
//
// Checking and querying:
//   if (bible_rail_is_spare(value)) {
//       int idx = bible_rail_decode(value);
//       printf("%s: %s\n",
//              bible_rail_variant_ref(idx),
//              bible_rail_variant_desc(idx));
//   }
//
// The 13 Variants (243-255):
//   0: 1 Corinthians 16:27 - TR manuscript note
//   1: 1 Peter 5:20 - turn sinner from error
//   ... (see web-variant-index.csv)
//   12: Zechariah 2:23 - Zerubbabel my servant
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_BIBLE_RAIL_H


