// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Verse Address Encoding (Ordinal ↔ Packed Trit5)
// Key: B-word-work-pkg-trit-include-verse-addr
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h)
//   Depends on trit.h for trit5_t type definition
//
// derives_from: bereshit/word/core/bible/addressing.toml
// See: word/scripture/kjv-ordinal-index.csv for 31,102 verse mappings
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_VERSE_ADDR_H
#define BERESHIT_VERSE_ADDR_H

// Verse address encoding: converts ordinals (1-31102) to packed trit5 pairs.
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
// Scripture: "So shall my word be that goeth forth out of my mouth: it shall
//             not return unto me void, but it shall accomplish that which I
//             please." — Isaiah 55:11
//
// Principle: Every verse has an address. The Word of God is not random text—
//            it is ordered, numbered, addressable. 31,102 verses in canonical
//            order, each with a unique ordinal that encodes to 2 bytes.
//
// Anchor: "The words of the LORD are pure words: as silver tried in a furnace
//          of earth, purified seven times." — Psalm 12:6
//
// # CPI-SI Identity
//
// Component Type: Rung (builds on trit5_t, enables verse lookup)
//
// Role: Provides encode/decode functions for verse ordinals to packed trit5
//       representation. Bridges the addressing.toml spec to implementation.
//       2 bytes per verse address using trit5 packing (base 243).
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-14
//   - Version: 0.1.0
//   - Modified: 2025-12-14 - Initial verse address encoding
//
// # Purpose & Function
//
// Purpose: Encode/decode verse ordinals (1-31102) to packed trit5 pairs.
//          This implements the semantic encoding (0-242 range) for Scripture.
//
// Core Design: Ordinal splits into high and low trit5 values using base 243.
//              high_trit5 = ordinal / 243 (quotient, 0-127)
//              low_trit5  = ordinal % 243 (remainder, 0-242)
//              Decodes as: ordinal = (high * 243) + low
//
// Key Features:
//
//   - verse_addr_encode: ordinal (1-31102) → packed (high, low) trit5 pair
//   - verse_addr_decode: packed (high, low) → ordinal (1-31102)
//   - verse_addr_is_valid: check if packed address is valid (not variant)
//   - Constants for range validation
//
// Philosophy: Every verse has an address. The encoding makes Scripture
//             computable—navigable, storable, transmittable in 2 bytes.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: stdbool.h (for bool type), stdint.h (for uint16_t)
//   - External: None
//   - Internal: trit.h (trit5_t type definition)
//
// What Uses This:
//
//   - Libraries: Scripture encoding, ordinal index lookups, verse navigation
//   - Commands: demo-math, Scripture address tools
//
// # Usage & Integration
//
// Import:
//
//    #include "trit/verse_addr.h"
//
// Integration Pattern:
//
//  1. Include verse_addr.h (which includes trit.h)
//  2. Use verse_addr_encode to convert ordinal to packed bytes
//  3. Use verse_addr_decode to convert packed bytes back to ordinal
//  4. Use verse_addr_is_valid to verify before decode
//
// Public API:
//
//    Encoding:
//      verse_addr_encode(uint16_t ordinal, trit5_t* high, trit5_t* low)
//      verse_addr_decode(trit5_t high, trit5_t low) → uint16_t ordinal
//
//    Query:
//      verse_addr_is_valid(trit5_t high, trit5_t low) → bool
//      verse_addr_max_ordinal() → uint16_t (31102)
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
#include <stdbool.h>    // bool type for validity check
#include <stdint.h>     // uint16_t for ordinal type

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Verse Address Constants ---
// KJV canonical: 31,102 verses, ordinals 1-31102
// Packed format: 2 trit5 bytes (high, low) using base 243

#define VERSE_ADDR_MIN_ORDINAL  1       // Genesis 1:1
#define VERSE_ADDR_MAX_ORDINAL  31102   // Revelation 22:21
#define VERSE_ADDR_TRIT5_BASE   243     // 3^5 = 243 semantic values per trit5
#define VERSE_ADDR_MAX_HIGH     127     // 31102 / 243 = 127.99... → max high byte
#define VERSE_ADDR_VARIANT_COUNT 13     // 256 - 243 = 13 spare values for variants

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---
// [Reserved: No enums needed - uses trit5_t from trit.h]

//--- Building Blocks ---
// [Reserved: trit5_t is the building block from trit.h]

//--- Composed Types ---

// Packed verse address: 2 trit5 bytes representing ordinal 1-31102.
// Encoding: high = ordinal / 243, low = ordinal % 243
// Decoding: ordinal = (high * 243) + low
typedef struct {
    trit5_t high;   // High byte: ordinal / 243 (0-127)
    trit5_t low;    // Low byte: ordinal % 243 (0-242)
} verse_addr_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Encoding Functions (src/verse_addr.c) ---

// Encode a verse ordinal as a packed trit5 pair.
//
// Converts an ordinal (1-31102) to its packed representation using
// base 243 division: high = ordinal / 243, low = ordinal % 243.
//
// Parameters:
//   ordinal: Verse ordinal (1-31102)
//
// Returns: verse_addr_t with high and low bytes, or {0,0} if invalid.
//
// Example:
//   verse_addr_t addr = verse_addr_encode(1);     // Genesis 1:1 → {0, 1}
//   verse_addr_t addr = verse_addr_encode(31102); // Revelation 22:21 → {127, 241}
verse_addr_t verse_addr_encode(uint16_t ordinal);

// Decode a packed trit5 pair to its verse ordinal.
//
// Converts packed (high, low) bytes back to ordinal using
// formula: ordinal = (high * 243) + low.
//
// Parameters:
//   addr: verse_addr_t with high and low bytes
//
// Returns: Ordinal (1-31102), or 0 if invalid (spare value detected).
//
// Example:
//   uint16_t ord = verse_addr_decode((verse_addr_t){0, 1});     // 1
//   uint16_t ord = verse_addr_decode((verse_addr_t){127, 241}); // 31102
uint16_t verse_addr_decode(verse_addr_t addr);

//--- Query Functions (src/verse_addr.c) ---

// Check if a packed address is valid (not a WEB variant marker).
//
// A valid address has low byte in semantic range (0-242).
// If low >= 243, it's a Bible Rail spare value (WEB variant).
//
// Parameters:
//   addr: verse_addr_t to check
//
// Returns: true if valid verse address, false if WEB variant marker.
//
// Example:
//   verse_addr_is_valid((verse_addr_t){0, 1});    // true (Genesis 1:1)
//   verse_addr_is_valid((verse_addr_t){0, 243});  // false (WEB variant)
bool verse_addr_is_valid(verse_addr_t addr);

// Check if an ordinal is in the valid KJV range.
//
// Valid range: 1-31102 (Genesis 1:1 through Revelation 22:21).
//
// Parameters:
//   ordinal: Value to check
//
// Returns: true if 1 <= ordinal <= 31102, false otherwise.
//
// Example:
//   verse_addr_is_valid_ordinal(1);      // true
//   verse_addr_is_valid_ordinal(0);      // false
//   verse_addr_is_valid_ordinal(31103);  // false
bool verse_addr_is_valid_ordinal(uint16_t ordinal);

// Get the maximum verse ordinal (31102).
//
// Returns: 31102 (VERSE_ADDR_MAX_ORDINAL)
uint16_t verse_addr_max_ordinal(void);

// Get the trit5 base (243).
//
// Returns: 243 (VERSE_ADDR_TRIT5_BASE)
uint16_t verse_addr_trit5_base(void);

//--- WEB Variant Encoding Functions (src/verse_addr.c) ---

// Encode a WEB variant index as a packed address.
//
// WEB-only verses (the 13 textual variants) use spare values
// in the low byte (243-255). The high byte is set to 0.
// This bridges verse addressing with Bible Rail encoding.
//
// Parameters:
//   variant_index: Index into WEB variant table (0-12)
//
// Returns: verse_addr_t with {0, 243+index}, or {0,0} if invalid.
//
// Example:
//   verse_addr_t addr = verse_addr_encode_variant(0);  // {0, 243}
//   verse_addr_t addr = verse_addr_encode_variant(12); // {0, 255}
verse_addr_t verse_addr_encode_variant(int variant_index);

// Check if a packed address is a WEB variant (not a KJV verse).
//
// A packed address is a variant if low >= 243 (spare range).
// This is the inverse of verse_addr_is_valid().
//
// Parameters:
//   addr: Packed address to check
//
// Returns: true if low byte is in spare range (243-255).
//
// Example:
//   verse_addr_is_variant({0, 243});  // true (WEB variant)
//   verse_addr_is_variant({0, 1});    // false (KJV verse)
bool verse_addr_is_variant(verse_addr_t addr);

// Decode a WEB variant packed address to its variant index.
//
// Extracts the variant index from a spare-value packed address.
// This is the inverse of verse_addr_encode_variant().
//
// Parameters:
//   addr: Packed address with low byte in spare range
//
// Returns: Variant index (0-12), or -1 if not a variant address.
//
// Example:
//   verse_addr_decode_variant({0, 243});  // 0
//   verse_addr_decode_variant({0, 255});  // 12
//   verse_addr_decode_variant({0, 1});    // -1 (not variant)
int verse_addr_decode_variant(verse_addr_t addr);

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
// NOTE: This header defines prototypes only. Functions implemented in src/verse_addr.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── verse_addr_encode()         → ordinal (1-31102) → packed (high, low)
//   ├── verse_addr_decode()         → packed (high, low) → ordinal (1-31102)
//   ├── verse_addr_is_valid()       → check if packed address is valid (KJV verse)
//   ├── verse_addr_is_valid_ordinal() → check if ordinal in range
//   ├── verse_addr_max_ordinal()    → get max (31102)
//   ├── verse_addr_trit5_base()     → get base (243)
//   ├── verse_addr_encode_variant() → variant index → packed (WEB variant)
//   ├── verse_addr_is_variant()     → check if packed is WEB variant
//   └── verse_addr_decode_variant() → packed → variant index
//
//   Helpers (Bottom Rungs - none, functions are standalone)
//   └── [All functions are public APIs - pure arithmetic]
//
// Baton Flow (Execution Paths):
//   Encode: Entry → verse_addr_encode(ordinal) → divide by 243 → return {high, low}
//   Decode: Entry → verse_addr_decode(addr) → multiply/add → return ordinal
//   Query: Entry → verse_addr_is_valid(addr) → range check → return bool
//   Variant: Entry → verse_addr_encode_variant(idx) → {0, 243+idx} → return addr
//
// APUs (Available Processing Units):
//   - 9 functions total
//   - 0 helpers (all public)
//   - 9 public APIs
//
// Type Definitions:
//   └── verse_addr_t: struct { trit5_t high; trit5_t low; }
//
// Implementation Location:
//   All function implementations in: src/verse_addr.c
//
// Declared Units:
// - 0 enums (uses existing types)
// - 1 struct (verse_addr_t)
// - 9 function prototypes
// - 4 #define constants

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All functions are public APIs - pure arithmetic]

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// See SETUP "Function Prototypes" section above for complete declarations.
// Functions organized by purpose:
//   - Encoding: verse_addr_encode, verse_addr_decode
//   - Query: is_valid, is_valid_ordinal, max_ordinal, trit5_base

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return safe defaults rather than error codes.
//   - Invalid ordinal (0 or > 31102) → {0, 0} packed address
//   - Invalid packed address (low >= 243) → 0 ordinal
//   - Validation functions return false for invalid inputs
//
// The {0, 0} return for invalid encode is distinguishable since
// ordinal 0 is invalid (valid range is 1-31102).

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - verse_addr_encode, verse_addr_decode (encoding)
//   - verse_addr_is_valid, is_valid_ordinal, max_ordinal, trit5_base (query)

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 verse_addr.h
//   echo '#include "trit/verse_addr.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "trit/verse_addr.h"
//   int main(void) { return VERSE_ADDR_MAX_ORDINAL > 0; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - pure arithmetic functions]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Verse address encoding maps ordinals (1-31102) to packed
// trit5 pairs using base 243. Every verse in the KJV has a 2-byte address.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions (follow is_valid pattern)
//   ✅ Add convenience functions for common operations
//
// Modify with Care:
//   ⚠️ Function signatures (breaks consumers)
//   ⚠️ verse_addr_t struct layout (affects binary compatibility)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Base 243 encoding (mathematical foundation)
//   ❌ Include guard (BERESHIT_VERSE_ADDR_H)
//   ❌ Max ordinal 31102 (KJV canonical count)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: RUNG component building on trit5_t packing.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding query functions: SETUP "Function Prototypes" → Query Functions section
// Adding constants: SETUP "Defines" section
// Modifying struct: SETUP "Composed Types" section

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Data Sizes:
//   - ordinal: uint16_t (1-31102)
//   - verse_addr_t: 2 bytes (high + low trit5)
//
// All functions are O(1) - simple division and modulo operations.
// No heap allocation, no table lookups—pure arithmetic.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "unknown type name 'trit5_t'"
//   Solution: verse_addr.h includes trit.h automatically
//
// Problem: verse_addr_decode returns 0
//   Check: low byte must be < 243 (not a WEB variant marker)
//
// Problem: verse_addr_encode returns {0,0}
//   Check: ordinal must be 1-31102 (valid KJV range)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Key dependency: trit.h (trit5_t type)
// Implementation: src/verse_addr.c
// Sibling: bible_rail.h (spare values 243-255 for WEB variants)
//
// Specifications:
//   - word/core/bible/addressing.toml (master spec)
//   - word/scripture/kjv-ordinal-index.csv (31,102 verse mappings)

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ verse_addr_encode - COMPLETED
//   ✓ verse_addr_decode - COMPLETED
//   ✓ verse_addr_is_valid - COMPLETED
//   ✓ verse_addr_is_valid_ordinal - COMPLETED
//   ✓ verse_addr_max_ordinal - COMPLETED
//   ✓ verse_addr_trit5_base - COMPLETED
//   ⏳ Book/chapter/verse lookup integration
//   ⏳ Bulk encode/decode functions
//   ⏳ Range iteration helpers
//
// Version History:
//   0.1.0 (2025-12-14) - Initial verse address encoding
//         - Ordinal ↔ packed trit5 conversion
//         - Validation functions for range checking

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header bridges the addressing.toml spec to implementation.
// Every verse has an address—2 bytes to locate any of 31,102 verses.
// The encoding makes Scripture computable without losing meaning.
//
// "So shall my word be that goeth forth out of my mouth: it shall
//  not return unto me void." — Isaiah 55:11
//
// The Word is ordered. Addressing reveals that order.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "trit/verse_addr.h"
//
// Encoding an ordinal:
//   verse_addr_t addr = verse_addr_encode(1);      // Genesis 1:1 → {0, 1}
//   verse_addr_t addr = verse_addr_encode(31102);  // Revelation 22:21 → {127, 241}
//
// Decoding a packed address:
//   uint16_t ord = verse_addr_decode((verse_addr_t){0, 1});     // 1
//   uint16_t ord = verse_addr_decode((verse_addr_t){127, 241}); // 31102
//
// Validating addresses:
//   if (verse_addr_is_valid(addr)) {
//       uint16_t ordinal = verse_addr_decode(addr);
//       printf("Ordinal: %u\n", ordinal);
//   } else {
//       // This is a WEB variant marker (low >= 243)
//       int variant = bible_rail_decode(addr.low);
//   }
//
// Key Boundaries:
//   Genesis 1:1    = ordinal 1     → {0, 1}
//   Psalm 119:1    = ordinal 15766 → {64, 218}
//   Revelation 22:21 = ordinal 31102 → {127, 241}
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_VERSE_ADDR_H


