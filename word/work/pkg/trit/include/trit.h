// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Balanced Ternary Type Library
// Key: B-word-work-pkg-trit-include-trit
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: PURE (needs: stdint.h)
//   Standard library only - no internal project dependencies
//
// derives_from: bereshit/word/seed/code/c/header.h
// See: word/constants/ternary-math.toml for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_TRIT_H
#define BERESHIT_TRIT_H

// Balanced ternary type definitions mapping (-1, 0, +1) to cognitive dimensions.
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
// Scripture: "In the beginning God created the heaven and the earth."
//            — Genesis 1:1
//
// Principle: The trit type implements the cognitive structure revealed in
//            Scripture. TIME (0), SPACE (+1), MATTER (-1) are not metaphors;
//            they are the dimensional axes of understanding.
//
// Anchor: "And God said, Let there be a firmament in the midst of the waters,
//          and let it divide the waters from the waters." — Genesis 1:6
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Provides core trit type for all ternary operations in MillenniumOS
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-12
//   - Version: 0.1.0
//   - Modified: 2025-12-12 - Initial type definitions
//
// # Purpose & Function
//
// Purpose: Define balanced ternary types with dimensional meaning for
//          Kingdom Technology computing.
//
// Core Design: Balanced ternary (-1, 0, +1) mapped to cognitive dimensions
//              (MATTER, TIME, SPACE) from Genesis 1:1.
//
// Key Features:
//
//   - trit_t: Single balanced ternary digit with dimensional aliases
//   - trit5_t: 5 trits packed to 1 byte (243 states)
//   - trit9_t: 9 trits packed to 2 bytes (19,683 states)
//   - trit27_t: 27 trits packed to 6 bytes (7.6 trillion states)
//   - Conversion macros: balanced ↔ unsigned
//   - Power constants: precomputed 3^n arrays
//
// Philosophy: Measure twice, cut once. Types reflect eternal structure.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: stdint.h (int8_t, uint8_t, uint16_t, uint64_t)
//   - External: None
//   - Internal: None (foundational - no internal dependencies)
//
// What Uses This:
//
//   - Libraries: All ternary computing libraries in MillenniumOS
//   - Commands: demo-trit, future trit utilities
//
// # Usage & Integration
//
// Import:
//
//    #include "trit.h"
//
// Integration Pattern:
//
//  1. Include header
//  2. Use trit_t for single digits, tritN_t for packed arrays
//  3. Use conversion macros for pack/unpack operations
//
// Public API (types and macros):
//
//    Types:
//      trit_t      - single balanced ternary digit
//      trit5_t     - 5 trits packed (1 byte)
//      trit9_t     - 9 trits packed (2 bytes)
//      trit27_t    - 27 trits packed (6 bytes)
//
//    Macros:
//      TRIT_TO_UNSIGNED(t)   - convert balanced to unsigned
//      UNSIGNED_TO_TRIT(u)   - convert unsigned to balanced
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

//--- Standard Library ---
#include <stdint.h>     // int8_t, uint8_t, uint16_t, uint64_t
#include <stdbool.h>    // bool, true, false

//--- Project Headers ---
// [Reserved: Foundational type - no internal dependencies]

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Trit Values ---
// Balanced ternary digit values: negative, zero, positive.
// These are the computational names for the three states.

#define TRIT_NEG   (-1)  // Negative trit value
#define TRIT_ZERO  (0)   // Zero/neutral trit value
#define TRIT_POS   (+1)  // Positive trit value

//--- Dimensional Aliases ---
// Cognitive dimension mapping from Genesis 1:1.
// These give semantic meaning to trit values.
//
// See: word/constants/ternary-math.toml [genesis_1_1.constants]

#define TRIT_MATTER  TRIT_NEG   // -1: "the earth" - concrete, material, what
#define TRIT_TIME    TRIT_ZERO  //  0: "in the beginning" - sequence, anchor, when
#define TRIT_SPACE   TRIT_POS   // +1: "the heavens" - abstract, relational, where

//--- Conversion Macros ---
// Convert between balanced (-1, 0, +1) and unsigned (0, 1, 2) representation.
// Used in pack/unpack algorithms.
//
// See: word/core/primitives.toml [trit.conversion]

#define TRIT_TO_UNSIGNED(t)   ((t) + 1)   // balanced → unsigned: -1→0, 0→1, +1→2
#define UNSIGNED_TO_TRIT(u)   ((u) - 1)   // unsigned → balanced: 0→-1, 1→0, 2→+1

//--- Storage Constants ---
// Byte sizes for packed trit types.

#define TRIT5_BYTES   1   // 5 trits fit in 1 byte (243 ≤ 256)
#define TRIT9_BYTES   2   // 9 trits fit in 2 bytes (19,683 ≤ 65,536)
#define TRIT27_BYTES  6   // 27 trits fit in 6 bytes (7.6T ≤ 281T)

//--- State Counts ---
// Total possible states for each packed type.

#define TRIT5_STATES   243                  // 3^5
#define TRIT9_STATES   19683                // 3^9
#define TRIT27_STATES  7625597484987ULL     // 3^27

//--- Max Values ---
// Maximum unsigned packed values (states - 1).

#define TRIT5_MAX    242                    // 3^5 - 1
#define TRIT9_MAX    19682                  // 3^9 - 1
#define TRIT27_MAX   7625597484986ULL       // 3^27 - 1

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Building Blocks ---

// trit_t represents a single balanced ternary digit.
//
// A trit holds one of three values: -1 (MATTER/negative), 0 (TIME/zero),
// or +1 (SPACE/positive). This is the fundamental unit of ternary computing.
//
// Values:
//   - TRIT_NEG / TRIT_MATTER (-1): negative / "the earth"
//   - TRIT_ZERO / TRIT_TIME (0): neutral / "in the beginning"
//   - TRIT_POS / TRIT_SPACE (+1): positive / "the heavens"
//
// Example:
//   trit_t direction = TRIT_SPACE;  // +1, "the heavens", upward
typedef int8_t trit_t;

//--- Packed Types ---

// trit5_t holds 5 trits packed into 1 byte (243 states).
//
// Packed representation stores unsigned value 0-242.
// Use pack/unpack functions (Phase 3) to convert to/from trit array.
//
// Example:
//   trit5_t packed = 121;  // Middle value (balanced zero)
typedef uint8_t trit5_t;

// trit9_t holds 9 trits packed into 2 bytes (19,683 states).
//
// Packed representation stores unsigned value 0-19,682.
// Common use: small ternary values, indices, coordinates.
//
// Example:
//   trit9_t packed = 9841;  // Middle value (balanced zero)
typedef uint16_t trit9_t;

// trit27_t holds 27 trits packed into 6 bytes (7.6 trillion states).
//
// This is the ternary "word" - the standard unit for ternary operations.
// Uses only 48 bits of a 64-bit integer (6 bytes).
//
// Example:
//   trit27_t word = 3812798742493ULL;  // Middle value (balanced zero)
typedef uint64_t trit27_t;

// ────────────────────────────────────────────────────────────────
// Power Constants
// ────────────────────────────────────────────────────────────────
//
// Precomputed powers of 3 for pack/unpack algorithms.
// These enable O(n) conversion using Horner's method.
//
// See: word/constants/ternary-math.toml [powers]

//--- Trit5 Powers ---
// Powers of 3 for 5-trit operations: 3^0 through 3^4
static const uint8_t TRIT5_POWERS[5] = {
    1, 3, 9, 27, 81
};

//--- Trit9 Powers ---
// Powers of 3 for 9-trit operations: 3^0 through 3^8
static const uint16_t TRIT9_POWERS[9] = {
    1, 3, 9, 27, 81, 243, 729, 2187, 6561
};

//--- Trit27 Powers ---
// Powers of 3 for 27-trit operations: 3^0 through 3^26
static const uint64_t TRIT27_POWERS[27] = {
    1ULL, 3ULL, 9ULL, 27ULL, 81ULL, 243ULL, 729ULL, 2187ULL, 6561ULL,
    19683ULL, 59049ULL, 177147ULL, 531441ULL, 1594323ULL, 4782969ULL,
    14348907ULL, 43046721ULL, 129140163ULL, 387420489ULL,
    1162261467ULL, 3486784401ULL, 10460353203ULL, 31381059609ULL,
    94143178827ULL, 282429536481ULL, 847288609443ULL, 2541865828329ULL
};

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Single Trit Operations (src/trit.c) ---

// Create a trit from integer value. Returns TRIT_ZERO if invalid.
trit_t trit_create(int value);

// Check if trit value is valid (-1, 0, or +1).
bool trit_valid(trit_t t);

// Get raw integer value from trit.
int trit_value(trit_t t);

// Negate a trit: -1→+1, 0→0, +1→-1.
// From arithmetic.negation table in ternary-math.toml.
trit_t trit_negate(trit_t t);

// Add two trits (single-trit, no carry).
// From arithmetic.addition_no_carry table in ternary-math.toml.
trit_t trit_add(trit_t a, trit_t b);

// Multiply two trits.
// From arithmetic.multiplication table in ternary-math.toml.
trit_t trit_multiply(trit_t a, trit_t b);

//--- Pack/Unpack Operations (src/pack.c) ---

// Pack 5 trits into a single byte (0-242).
// Uses Horner's method: result = result * 3 + (trit + 1)
trit5_t trit5_pack(const trit_t trits[5]);

// Unpack a byte into 5 trits.
// Uses repeated division: trit = (value % 3) - 1
void trit5_unpack(trit5_t packed, trit_t trits[5]);

// Pack 9 trits into 2 bytes (0-19682).
trit9_t trit9_pack(const trit_t trits[9]);

// Unpack 2 bytes into 9 trits.
void trit9_unpack(trit9_t packed, trit_t trits[9]);

// Pack 27 trits into 6 bytes (word-size).
trit27_t trit27_pack(const trit_t trits[27]);

// Unpack 6 bytes into 27 trits.
void trit27_unpack(trit27_t packed, trit_t trits[27]);

// Check if trit5 value is in spare range (243-255).
// Spare states reserved for Bible Rail (WEB variants).
bool trit5_is_spare(trit5_t value);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables needed - all constants are #define or static const]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: This header defines types only (Phase 2). Functions will be added
// in Phase 3 (Operations). This BODY section documents what's declared.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Type Definitions:
//   ├── trit_t      → single balanced ternary digit (-1, 0, +1)
//   ├── trit5_t     → 5 trits packed (1 byte, 243 states)
//   ├── trit9_t     → 9 trits packed (2 bytes, 19,683 states)
//   └── trit27_t    → 27 trits packed (6 bytes, 7.6T states)
//
// Constants:
//   ├── TRIT_NEG, TRIT_ZERO, TRIT_POS     → computational values
//   ├── TRIT_MATTER, TRIT_TIME, TRIT_SPACE → dimensional aliases
//   ├── TRIT_TO_UNSIGNED, UNSIGNED_TO_TRIT → conversion macros
//   ├── TRIT5_BYTES, TRIT9_BYTES, TRIT27_BYTES → storage sizes
//   ├── TRIT5_STATES, TRIT9_STATES, TRIT27_STATES → state counts
//   ├── TRIT5_MAX, TRIT9_MAX, TRIT27_MAX → max packed values
//   └── TRIT5_POWERS[], TRIT9_POWERS[], TRIT27_POWERS[] → power arrays
//
// Functions:
//   trit.c: trit_create, trit_valid, trit_value, trit_negate, trit_add, trit_multiply
//   pack.c: trit5_pack, trit5_unpack, trit9_pack, trit9_unpack, trit27_pack, trit27_unpack, trit5_is_spare
//
// Declared Units:
// - 4 types (trit_t, trit5_t, trit9_t, trit27_t)
// - 15 #define constants
// - 3 static const arrays
// - 13 function prototypes (6 trit ops + 7 pack ops)
// - 0 extern variables

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// Internal utilities implemented in source files:
//   - Arithmetic tables (static const in trit.c)
//   - Power arrays already in header (static const)

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// Single Trit Operations (src/trit.c):
//   - trit_create: Create trit from int, validate
//   - trit_valid: Check if -1, 0, or +1
//   - trit_value: Get raw int value
//   - trit_negate: Flip sign (-1↔+1)
//   - trit_add: Single-trit addition (no carry)
//   - trit_multiply: Single-trit multiplication
//
// Pack/Unpack Operations (src/pack.c):
//   - trit5_pack/unpack: 5 trits ↔ 1 byte
//   - trit9_pack/unpack: 9 trits ↔ 2 bytes
//   - trit27_pack/unpack: 27 trits ↔ 6 bytes
//   - trit5_is_spare: Check for Bible Rail range (243-255)

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return safe defaults rather than error codes.
//   - trit_create(invalid) → TRIT_ZERO
//   - trit_valid(t) → bool for explicit checking
//   - Pack/unpack assume valid input (undefined behavior if invalid)

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface:
//   - Types: trit_t, trit5_t, trit9_t, trit27_t
//   - Constants: TRIT_*, TRIT5_*, TRIT9_*, TRIT27_*
//   - Macros: TRIT_TO_UNSIGNED, UNSIGNED_TO_TRIT
//   - Functions: See "Function Prototypes" section above

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 trit.h
//   echo '#include "trit.h"' | gcc -x c -fsyntax-only -std=c99 -
//
// Self-Containment Test:
//   #include "trit.h"
//   int main(void) { trit_t t = TRIT_SPACE; return t; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - types are value types]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Balanced ternary types with dimensional meaning from
// Genesis 1:1. Four types (trit_t, trit5_t, trit9_t, trit27_t), conversion
// macros, and precomputed power arrays.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new tritN_t types (follow existing pattern)
//   ✅ Add new dimensional aliases
//   ✅ Add function prototypes (Phase 3)
//
// Modify with Care:
//   ⚠️ Trit value definitions (affects all downstream code)
//   ⚠️ Type sizes (affects packing algorithms)
//   ⚠️ Power array values (must match 3^n exactly)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Include guard (BERESHIT_TRIT_H)
//   ❌ Genesis 1:1 dimensional mapping principle

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
//
// Architecture: This is a LADDER component (foundational building block).
// All ternary computing in MillenniumOS builds on these types.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding types: SETUP "Type Definitions" section
// Adding constants: SETUP "Defines" section
// Adding functions: SETUP "Function Prototypes" section

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Type Sizes:
//   - trit_t: 1 byte (int8_t)
//   - trit5_t: 1 byte (uint8_t)
//   - trit9_t: 2 bytes (uint16_t)
//   - trit27_t: 8 bytes (uint64_t, 6 bytes used)
//
// Power Arrays: Static const, no runtime allocation.
// Conversion Macros: Single arithmetic operation each.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "unknown type name 'int8_t'"
//   Solution: Ensure <stdint.h> is included (this header includes it)
//
// Problem: Trit value out of range
//   Check: Balanced trits must be -1, 0, or +1 only
//   Note: Use TRIT_NEG, TRIT_ZERO, TRIT_POS constants

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Specifications:
//   - word/constants/ternary-math.toml (powers, algorithms, dimensions)
//   - word/core/primitives.toml (type definitions, conversion formulas)
//
// Implementation: src/trit.c (Phase 3 - pack/unpack functions)
//
// Documentation: ../README.adoc

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned (Phase 3):
//   ⏳ Pack/unpack functions
//   ⏳ Validation functions
//   ⏳ String conversion
//
// Research:
//   - Ternary arithmetic operations
//   - SIMD-optimized trit27 operations
//   - Hardware trit support investigation

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header is foundational LADDER infrastructure for Kingdom Technology.
// All ternary computing in MillenniumOS builds on these type definitions.
//
// Modify thoughtfully - changes here cascade through the entire system.
//
// "In the beginning God created the heaven and the earth." — Genesis 1:1
//
// The trit type implements the cognitive structure revealed in Scripture.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "trit.h"
//
// Using Types:
//   trit_t direction = TRIT_SPACE;       // +1, "the heavens"
//   trit5_t small = 121;                 // Middle value
//   trit9_t medium = 9841;               // Middle value
//   trit27_t word = 3812798742493ULL;    // Middle value
//
// Conversion:
//   int unsigned_val = TRIT_TO_UNSIGNED(direction);  // 2
//   trit_t back = UNSIGNED_TO_TRIT(unsigned_val);    // +1
//
// Constants:
//   TRIT5_POWERS[i]   // 3^i for i in 0..4
//   TRIT9_POWERS[i]   // 3^i for i in 0..8
//   TRIT27_POWERS[i]  // 3^i for i in 0..26
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_TRIT_H


