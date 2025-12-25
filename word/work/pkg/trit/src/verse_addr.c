// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Verse Address Encoding Implementation
// Key: B-word-work-pkg-trit-src-verse-addr
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: verse_addr.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/core/bible/addressing.toml
// See: word/scripture/kjv-ordinal-index.csv for 31,102 verse mappings
//
// ═══════════════════════════════════════════════════════════════════════════

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
// Role: Implements verse address encode/decode functions. Maps ordinals to
//       packed trit5 pairs using base 243. Bridges spec to implementation.
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
//   - 0.1.0 (2025-12-14) - Initial verse address encoding
//
// # Purpose & Function
//
// Purpose: Implement verse address encode/decode functions for ordinal ↔ packed.
//
// Core Design: Pure arithmetic using base 243 (trit5 semantic range).
//              high = ordinal / 243, low = ordinal % 243.
//              ordinal = (high * 243) + low. O(1) operations.
//
// Key Features:
//
//   - Encoding: ordinal → packed (high, low) trit5 pair
//   - Decoding: packed (high, low) → ordinal
//   - Validation: range checking for ordinals and packed addresses
//   - Safety: invalid inputs return {0, 0} or 0
//
// Philosophy: Every verse has an address. The encoding reveals Scripture's order.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: (none needed - pure arithmetic)
//   - External: None
//   - Internal: verse_addr.h (includes trit.h, stdbool.h, stdint.h)
//
// What Uses This:
//
//   - Libraries: libtrit.a consumers
//   - Commands: demo-math, Scripture address tools
//
// Integration Points:
//
//   - Link with libtrit.a
//   - Include verse_addr.h for type definitions
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
#include "verse_addr.h"  // Types and prototypes (includes trit.h, stdbool.h, stdint.h)

//--- Standard Library ---
// [Reserved: No additional includes needed - pure arithmetic]

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All constants defined in verse_addr.h]

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No static variables - pure functions with no state]

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All types defined in verse_addr.h]

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
// 1. Encoding: verse_addr_encode, verse_addr_decode
// 2. Query: verse_addr_is_valid, is_valid_ordinal, max_ordinal, trit5_base

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Implementation Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   No static data - pure arithmetic functions
//   All functions are standalone
//
// Baton Flow (Execution Paths):
//   Encode: ordinal → bounds check → divide/modulo → return {high, low}
//   Decode: addr → is_valid check → multiply/add → return ordinal
//   Query: value → bounds check → return bool/constant
//
// APUs (Available Processing Units):
//   - 6 functions (all public)
//   - 0 static variables (pure functions)

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

// Encode a verse ordinal as a packed trit5 pair.
//
// Implements: ordinal (1-31102) → packed (high, low)
// Formula: high = ordinal / 243, low = ordinal % 243
//
// Returns {0, 0} if ordinal out of range (0 is never a valid ordinal).
verse_addr_t verse_addr_encode(uint16_t ordinal) {
    verse_addr_t addr = {0, 0};

    // Validate ordinal range (1-31102)
    if (ordinal < VERSE_ADDR_MIN_ORDINAL || ordinal > VERSE_ADDR_MAX_ORDINAL) {
        return addr;  // Invalid - {0, 0} signals error
    }

    // Encode using base 243
    // high = ordinal / 243 (quotient: 0-127)
    // low  = ordinal % 243 (remainder: 0-242)
    addr.high = (trit5_t)(ordinal / VERSE_ADDR_TRIT5_BASE);
    addr.low  = (trit5_t)(ordinal % VERSE_ADDR_TRIT5_BASE);

    return addr;
}

// Decode a packed trit5 pair to its verse ordinal.
//
// Implements: packed (high, low) → ordinal (1-31102)
// Formula: ordinal = (high * 243) + low
//
// Returns 0 if packed address is invalid (low >= 243, i.e., WEB variant marker).
uint16_t verse_addr_decode(verse_addr_t addr) {
    // Check if this is a valid semantic address (not a WEB variant marker)
    if (!verse_addr_is_valid(addr)) {
        return 0;  // Invalid - low byte is spare value (WEB variant)
    }

    // Decode using base 243
    // ordinal = (high * 243) + low
    uint16_t ordinal = ((uint16_t)addr.high * VERSE_ADDR_TRIT5_BASE) + addr.low;

    // Final range check (handles edge cases like high > 127)
    if (ordinal < VERSE_ADDR_MIN_ORDINAL || ordinal > VERSE_ADDR_MAX_ORDINAL) {
        return 0;  // Decoded value out of valid range
    }

    return ordinal;
}

//--- Query Functions ---

// Check if a packed address is valid (not a WEB variant marker).
//
// A valid address has low byte in semantic range (0-242).
// If low >= 243, it's a Bible Rail spare value (WEB variant marker).
bool verse_addr_is_valid(verse_addr_t addr) {
    // Low byte must be in semantic range (0-242)
    // Spare values (243-255) indicate WEB variant markers, not verse addresses
    return addr.low < VERSE_ADDR_TRIT5_BASE;
}

// Check if an ordinal is in the valid KJV range.
//
// Valid range: 1-31102 (Genesis 1:1 through Revelation 22:21).
bool verse_addr_is_valid_ordinal(uint16_t ordinal) {
    return ordinal >= VERSE_ADDR_MIN_ORDINAL && ordinal <= VERSE_ADDR_MAX_ORDINAL;
}

// Get the maximum verse ordinal (31102).
//
// Returns: 31102 (VERSE_ADDR_MAX_ORDINAL)
uint16_t verse_addr_max_ordinal(void) {
    return VERSE_ADDR_MAX_ORDINAL;
}

// Get the trit5 base (243).
//
// Returns: 243 (VERSE_ADDR_TRIT5_BASE)
uint16_t verse_addr_trit5_base(void) {
    return VERSE_ADDR_TRIT5_BASE;
}

//--- WEB Variant Encoding Functions ---

// Encode a WEB variant index as a packed address.
//
// Variant addresses use spare values (243-255) in the low byte.
// High byte is always 0 for variants (distinguishes from KJV encoding).
verse_addr_t verse_addr_encode_variant(int variant_index) {
    verse_addr_t addr = {0, 0};

    // Validate variant index (0-12, matching VERSE_ADDR_VARIANT_COUNT)
    if (variant_index < 0 || variant_index >= VERSE_ADDR_VARIANT_COUNT) {
        return addr;  // Invalid - {0, 0} signals error
    }

    // Encode as spare value in low byte
    // High byte = 0, Low byte = 243 + index
    addr.high = 0;
    addr.low = (trit5_t)(VERSE_ADDR_TRIT5_BASE + variant_index);

    return addr;
}

// Check if a packed address is a WEB variant (not a KJV verse).
//
// A packed address is a variant if low >= 243 (spare range).
// This is the inverse of verse_addr_is_valid().
bool verse_addr_is_variant(verse_addr_t addr) {
    // Spare values are 243-255
    return addr.low >= VERSE_ADDR_TRIT5_BASE;
}

// Decode a WEB variant packed address to its variant index.
//
// Extracts the variant index from a spare-value packed address.
// Returns -1 if the address is not a variant (low < 243).
int verse_addr_decode_variant(verse_addr_t addr) {
    // Check if this is actually a variant address
    if (!verse_addr_is_variant(addr)) {
        return -1;  // Not a variant - this is a KJV verse address
    }

    // Extract variant index: low - 243
    return (int)(addr.low - VERSE_ADDR_TRIT5_BASE);
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
//   gcc -c -Wall -Wextra -std=c99 -I../include verse_addr.c
//
// Test Verification:
//   See test/verse_addr_test.c for encode/decode roundtrip tests

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Library source - linked into libtrit.a]

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
// Quick summary: Verse address implementation maps ordinals (1-31102)
// to packed trit5 pairs using base 243. Pure arithmetic, O(1) operations.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions (follow existing pattern)
//   ✅ Add convenience helpers
//
// Modify with Care:
//   ⚠️ Function implementations (match header prototypes)
//   ⚠️ Validation logic (affects decode safety)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Encode/decode arithmetic (base 243 division/modulo)
//   ❌ Max ordinal (31102 from KJV canonical count)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Header: include/verse_addr.h
// Test: test/verse_addr_test.c
// Sibling: bible_rail.c (spare values 243-255 for WEB variants)
// Spec: word/core/bible/addressing.toml

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// 31,102 verses. 2 bytes each. Every verse addressable.
// The encoding reveals Scripture's order.
//
// "So shall my word be that goeth forth out of my mouth..." — Isaiah 55:11

// ============================================================================
// END CLOSING
// ============================================================================
