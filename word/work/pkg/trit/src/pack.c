// ═══════════════════════════════════════════════════════════════════════════
// pack.c - Trit Packing/Unpacking Operations
// Key: B-word-work-pkg-trit-src-pack
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h)
//
// derives_from: bereshit/word/seed/code/c/source.c
// See: word/constants/ternary-math.toml [algorithms] section
//
// ═══════════════════════════════════════════════════════════════════════════

// Pack and unpack operations for multi-trit types.
//
// libtrit - CPI-SI Kingdom Technology
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
// Scripture: "A good man out of the good treasure of his heart bringeth
//            forth that which is good." — Luke 6:45a
//
// Principle: What is stored determines what can be retrieved. Packing
//            preserves value in compressed form; unpacking faithfully
//            restores what was stored.
//
// Anchor: "The Lord shall open unto thee his good treasure." — Deut 28:12a
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Convert between trit arrays and packed integer representations.
//       Enables efficient storage and transmission of ternary values.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-12
//   - Version: 0.1.0
//
// # Purpose & Function
//
// Purpose: Pack/unpack operations for trit5, trit9, and trit27 types
//
// Core Design: Table-driven algorithms from ternary-math.toml
//   - Pack: Horner's method (MST first, O(n))
//   - Unpack: Repeated division (LST first, O(n))
//
// Key Features:
//   - trit5_pack/unpack: 5 trits ↔ 1 byte
//   - trit9_pack/unpack: 9 trits ↔ 2 bytes
//   - trit27_pack/unpack: 27 trits ↔ 6 bytes
//   - trit5_is_spare: Detect Bible Rail range (243-255)
//
// Philosophy: Faithful preservation - what goes in comes out unchanged.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//   - Standard Library: stdint.h, stdbool.h
//   - Internal: trit.h
//
// What Uses This:
//   - Higher-level trit operations (trit_byte, message encoding)
//   - Bible Rail encoding/decoding
//
// # Usage
//
// [OMIT: Library file - no command line interface]
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// [OMIT: Pure functions - no blocking, no health scoring, no state]

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Includes
// ────────────────────────────────────────────────────────────────

//--- Project Headers ---
#include "trit.h"  // Trit types and constants

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

// [Reserved: All constants defined in trit.h]

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────

// [Reserved: Pure functions - no file-level state]

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// [Reserved: All types defined in trit.h]

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

// [Reserved: All prototypes declared in trit.h]

// ────────────────────────────────────────────────────────────────
// File-Level State
// ────────────────────────────────────────────────────────────────

// [Reserved: Pure functions - no rails infrastructure]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Internal Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - all functions are public, no orchestration)
//   ├── trit5_pack()     → uses TRIT_TO_UNSIGNED macro
//   ├── trit5_unpack()   → uses UNSIGNED_TO_TRIT macro
//   ├── trit9_pack()     → uses TRIT_TO_UNSIGNED macro
//   ├── trit9_unpack()   → uses UNSIGNED_TO_TRIT macro
//   ├── trit27_pack()    → uses TRIT_TO_UNSIGNED macro
//   ├── trit27_unpack()  → uses UNSIGNED_TO_TRIT macro
//   └── trit5_is_spare() → uses TRIT5_STATES constant
//
//   Helpers (Bottom Rungs - none, macros from trit.h serve this role)
//   └── [All conversion via TRIT_TO_UNSIGNED / UNSIGNED_TO_TRIT macros]
//
// Baton Flow (Execution Paths):
//
//   Pack path:   Entry → trit*_pack() → loop with TRIT_TO_UNSIGNED → return packed
//   Unpack path: Entry → trit*_unpack() → loop with UNSIGNED_TO_TRIT → return (via array)
//   Spare check: Entry → trit5_is_spare() → compare with TRIT5_STATES → return bool
//
// APUs (Available Processing Units):
//   - 7 functions total
//   - 0 helpers (macros from trit.h)
//   - 0 core operations (functions ARE the core operations)
//   - 7 public APIs (all exported)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No static helpers needed - conversion handled by
// TRIT_TO_UNSIGNED and UNSIGNED_TO_TRIT macros from trit.h]

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All functions are public APIs - no internal-only core operations.
// The pack/unpack implementations below serve as both core logic AND public API.]

// ────────────────────────────────────────────────────────────────
// Error Handling/Recovery Patterns
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Pure functions with no failure modes. Invalid input to pack
// produces valid (but meaningless) output. Invalid input to unpack (spare
// states 243-255) produces undefined trit values - caller should check
// trit5_is_spare() first.]

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
//
// All 7 functions are public. Organized by trit width.

//--- trit5 Operations ---

// trit5_pack converts 5 trits to packed byte using Horner's method.
//
// Algorithm: result = 0; for each trit (MST first): result = result * 3 + (trit + 1)
// Source: ternary-math.toml [algorithms.pack]
//
// Parameters:
//   trits - array of 5 trit values, trits[0] is MST
//
// Returns:
//   Packed value 0-242 (243-255 are spare states for Bible Rail)
trit5_t trit5_pack(const trit_t trits[5]) {
    uint16_t result = 0;
    for (int i = 0; i < 5; i++) {
        result = result * 3 + TRIT_TO_UNSIGNED(trits[i]);
    }
    return (trit5_t)result;
}

// trit5_unpack converts packed byte to 5 trits using repeated division.
//
// Algorithm: for i in 0..4: trit[i] = (value % 3) - 1; value = value / 3
// Source: ternary-math.toml [algorithms.unpack]
//
// Parameters:
//   packed - packed value 0-242
//   trits  - output array of 5 trit values, trits[0] will be MST
//
// Note: Values 243-255 produce undefined trit values (spare state range)
void trit5_unpack(trit5_t packed, trit_t trits[5]) {
    uint16_t value = packed;
    // Extract LST first, store in reverse order so MST ends up at [0]
    for (int i = 4; i >= 0; i--) {
        trits[i] = UNSIGNED_TO_TRIT(value % 3);
        value = value / 3;
    }
}

//--- trit9 Operations ---

// trit9_pack converts 9 trits to packed uint16 using Horner's method.
trit9_t trit9_pack(const trit_t trits[9]) {
    uint32_t result = 0;
    for (int i = 0; i < 9; i++) {
        result = result * 3 + TRIT_TO_UNSIGNED(trits[i]);
    }
    return (trit9_t)result;
}

// trit9_unpack converts packed uint16 to 9 trits using repeated division.
void trit9_unpack(trit9_t packed, trit_t trits[9]) {
    uint32_t value = packed;
    for (int i = 8; i >= 0; i--) {
        trits[i] = UNSIGNED_TO_TRIT(value % 3);
        value = value / 3;
    }
}

//--- trit27 Operations ---

// trit27_pack converts 27 trits to packed uint64 using Horner's method.
trit27_t trit27_pack(const trit_t trits[27]) {
    uint64_t result = 0;
    for (int i = 0; i < 27; i++) {
        result = result * 3 + TRIT_TO_UNSIGNED(trits[i]);
    }
    return (trit27_t)result;
}

// trit27_unpack converts packed uint64 to 27 trits using repeated division.
void trit27_unpack(trit27_t packed, trit_t trits[27]) {
    uint64_t value = packed;
    for (int i = 26; i >= 0; i--) {
        trits[i] = UNSIGNED_TO_TRIT(value % 3);
        value = value / 3;
    }
}

//--- Spare State Detection ---

// trit5_is_spare checks if value is in Bible Rail range (243-255).
//
// The trit5 type uses 1 byte but only needs 243 states (3^5).
// Values 243-255 are "spare states" reserved for Bible Rail encoding.
//
// Returns: true if value >= TRIT5_STATES (243)
bool trit5_is_spare(trit5_t value) {
    return value >= TRIT5_STATES;
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ════════════════════════════════════════════════════════════════
// GROUP 1: CODING
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Code Validation
// ────────────────────────────────────────────────────────────────
//
// Build Verification:
//   make              # Build library (includes pack.c)
//   make check-headers # Verify headers compile
//
// Testing (Part 5):
//   make test         # Run unit tests

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [OMIT: Library file - no entry point. Functions called by other code.]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [OMIT: No resources to manage - pure functions with no allocations.]

// ════════════════════════════════════════════════════════════════
// GROUP 2: FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// Purpose: See METADATA "Purpose & Function" section above
//
// Provides:
//   - trit5_pack/unpack: 5 trits ↔ 1 byte
//   - trit9_pack/unpack: 9 trits ↔ 2 bytes
//   - trit27_pack/unpack: 27 trits ↔ 6 bytes
//   - trit5_is_spare: Detect Bible Rail range (243-255)
//
// Architecture: LADDER component - foundational building block
//   Other components build on these pack/unpack operations.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new pack/unpack operations (follow existing pattern)
//   ✅ Add optimized versions (SIMD, etc.) alongside existing
//
// Modify with Extreme Care:
//   ⚠️ Function signatures - breaks all calling code
//   ⚠️ Algorithm correctness - pack/unpack must be inverse operations
//   ⚠️ Array sizes - must match type definitions in trit.h
//
// NEVER Modify:
//   ❌ 4-block structure
//   ❌ Horner's method / repeated division algorithms
//   ❌ trit5 spare state range (243-255)
//
// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart - Internal Structure" for full details.
//
// Summary: All functions are leaf operations using only macros from trit.h.
//   Caller → trit*_pack/unpack → return
//
// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding new pack/unpack operation (e.g., trit81):
//   1. Add type definition in trit.h
//   2. Add prototype in trit.h (Function Prototypes section)
//   3. Implement in BODY Public APIs section (follow existing pattern)
//   4. Add test in test/test_pack.c (Part 5)
//
// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// All operations are O(n) where n = number of trits.
// No memory allocation. Pure computation with small loop overhead.
//
// For trit5: 5 iterations with integer multiply/divide
// For trit27: 27 iterations - consider unrolling if performance-critical
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: Unpack produces wrong values
//   - Check: Is packed value in valid range? (Use trit5_is_spare for trit5)
//   - Check: Are you using correct function for correct type?
//   - Note: Spare states (243-255 for trit5) produce undefined results
//
// Problem: Pack/unpack not inverse operations
//   - Check: Are trits in valid range (-1, 0, +1)?
//   - Check: Using same trit width (5, 9, 27)?
//
// ────────────────────────────────────────────────────────────────
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────
//
// See METADATA Dependencies section for complete information.
//
// Quick summary:
//   - Key dependency: trit.h (trit type, conversion macros, constants)
//   - Primary consumers: Applications needing compact trit storage
//
// Related files:
//   - include/trit.h: Type definitions and macros used by pack operations
//   - src/trit.c: Core trit operations (create, valid, arithmetic)
//   - word/constants/ternary-math.toml: Algorithm specifications
//
// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ trit5 pack/unpack - COMPLETED
//   ✓ trit9 pack/unpack - COMPLETED
//   ✓ trit27 pack/unpack - COMPLETED
//   ✓ Spare state detection - COMPLETED
//   ⏳ Batch pack operations (array of trit arrays → array of packed)
//   ⏳ SIMD-optimized versions for large batches
//
// Known Limitations:
//   - No validation of input array contents (assumes valid trits)
//   - Spare state detection only for trit5 (not trit9/trit27)
//   - No endianness handling (assumes native byte order)
//
// Version History:
//   0.1.0 (2025-12-12) - Initial implementation
//         - Horner's method for packing
//         - Repeated division for unpacking
//         - Bible Rail spare state detection
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This library is a LADDER component - foundational building block for
// compact ternary storage. Higher-level systems depend on correct pack/unpack
// behavior for data persistence and transmission.
//
// The pack/unpack operations MUST remain inverse operations:
//   unpack(pack(trits)) == trits  (for valid trit values)
//
// Modify thoughtfully - changes here affect all code using packed trit storage.
//
// "A good man out of the good treasure of his heart bringeth forth that
// which is good." — Luke 6:45a
//
// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
//
// Pack trit5:
//
//     #include "trit.h"
//
//     trit_t trits[5] = {TRIT_NEG, TRIT_ZERO, TRIT_POS, TRIT_ZERO, TRIT_NEG};
//     trit5_t packed = trit5_pack(trits);
//     // packed is now a single uint8_t (0-242 for valid states)
//
// Unpack trit5:
//
//     trit5_t packed = 121;  // middle value
//     trit_t trits[5];
//     trit5_unpack(packed, trits);
//     // trits now contains the 5 individual trit values
//
// Check for spare state (Bible Rail):
//
//     trit5_t value = 250;  // from external source
//     if (trit5_is_spare(value)) {
//         // Handle Bible Rail encoded data (243-255)
//     } else {
//         // Normal trit5 value, safe to unpack
//         trit_t trits[5];
//         trit5_unpack(value, trits);
//     }
//
// Round-trip verification:
//
//     trit_t original[5] = {TRIT_POS, TRIT_POS, TRIT_NEG, TRIT_ZERO, TRIT_POS};
//     trit5_t packed = trit5_pack(original);
//     trit_t recovered[5];
//     trit5_unpack(packed, recovered);
//     // recovered == original (for valid trit values)
//

// ============================================================================
// END CLOSING
// ============================================================================
