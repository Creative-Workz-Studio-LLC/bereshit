// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Single Trit Operations
// Key: B-word-work-pkg-trit-src-trit
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h)
//   Requires trit.h for type definitions and function prototypes.
//
// derives_from: bereshit/word/seed/code/c/source.c
// See: word/constants/ternary-math.toml for arithmetic tables
//
// ═══════════════════════════════════════════════════════════════════════════

// Single trit operations: create, validate, negate, add, multiply.
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
// Scripture: "To every thing there is a season, and a time to every purpose
//             under the heaven." — Ecclesiastes 3:1
//
// Principle: Operations establish sequence and time. Single-trit operations
//            are the atomic TIME dimension of ternary computing.
//
// Anchor: "In the beginning God created the heaven and the earth."
//         — Genesis 1:1 (TIME is the first dimension)
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Provides atomic trit operations that all higher-level operations
//       build upon. This is the TIME dimension of the trit library.
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
// Purpose: Implement atomic operations on single balanced ternary trits.
//
// Core Design: Table-driven arithmetic from ternary-math.toml specification.
//
// Key Features:
//   - trit_create: Safe trit creation with validation
//   - trit_valid: Check if value is valid trit (-1, 0, +1)
//   - trit_value: Extract raw integer value
//   - trit_negate: Flip sign using negation table
//   - trit_add: Single-trit addition (no carry)
//   - trit_multiply: Single-trit multiplication
//
// Philosophy: Table-driven correctness over clever computation.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//   - Internal: trit.h (type definitions, function prototypes)
//
// What Uses This:
//   - Libraries: pack.c (uses trit operations for pack/unpack)
//   - Demo: demo-trit (validates trit operations)
//
// # Usage
//
// Functions are called directly - no command line interface.
// See trit.h for function signatures.
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// [OMIT: Library functions - no blocking, no health scoring]
//
// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Includes → Defines → Static Variables → Types → Function Prototypes → File-Level State

// ────────────────────────────────────────────────────────────────
// Includes
// ────────────────────────────────────────────────────────────────

//--- Project Headers ---
#include "trit.h"  // Type definitions, function prototypes, constants

//--- Standard Library ---
// [Reserved: All needed headers included via trit.h]

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

// [Reserved: All constants defined in trit.h]

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────

//--- Arithmetic Tables ---
// From ternary-math.toml [arithmetic] section.
// Tables indexed by trit value + 1 (so -1→0, 0→1, +1→2).

// Negation table: negate(t) = NEGATION_TABLE[t + 1]
// negate(-1) = +1, negate(0) = 0, negate(+1) = -1
static const trit_t NEGATION_TABLE[3] = { TRIT_POS, TRIT_ZERO, TRIT_NEG };

// Addition table (no carry): add(a, b) = ADDITION_TABLE[a + 1][b + 1]
// Row -1: -1 + {-1, 0, +1} = {-1, -1, 0} (simplified, overflow clamps)
// Row  0:  0 + {-1, 0, +1} = {-1, 0, +1}
// Row +1: +1 + {-1, 0, +1} = {0, +1, +1} (simplified, overflow clamps)
static const trit_t ADDITION_TABLE[3][3] = {
    { TRIT_NEG, TRIT_NEG, TRIT_ZERO },  // -1 + {-1, 0, +1}
    { TRIT_NEG, TRIT_ZERO, TRIT_POS },  //  0 + {-1, 0, +1}
    { TRIT_ZERO, TRIT_POS, TRIT_POS }   // +1 + {-1, 0, +1}
};

// Multiplication table: mul(a, b) = MULTIPLICATION_TABLE[a + 1][b + 1]
// Row -1: -1 × {-1, 0, +1} = {+1, 0, -1}
// Row  0:  0 × {-1, 0, +1} = {0, 0, 0}
// Row +1: +1 × {-1, 0, +1} = {-1, 0, +1}
static const trit_t MULTIPLICATION_TABLE[3][3] = {
    { TRIT_POS, TRIT_ZERO, TRIT_NEG },  // -1 × {-1, 0, +1}
    { TRIT_ZERO, TRIT_ZERO, TRIT_ZERO }, //  0 × {-1, 0, +1}
    { TRIT_NEG, TRIT_ZERO, TRIT_POS }   // +1 × {-1, 0, +1}
};

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// [Reserved: All types defined in trit.h]

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

// [Reserved: All prototypes declared in trit.h]

// [Reserved: Pure library functions - no Rails infrastructure needed]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Organizational Chart
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//   Public APIs (top):
//   ├── trit_create → trit_valid (validates input)
//   ├── trit_valid → (pure, no deps)
//   ├── trit_value → (pure, no deps)
//   ├── trit_negate → NEGATION_TABLE (static const)
//   ├── trit_add → ADDITION_TABLE (static const)
//   └── trit_multiply → MULTIPLICATION_TABLE (static const)
//
// All functions are simple table lookups or validations.
// No helper functions needed - tables do the work.

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────

// [Reserved: No helpers needed - arithmetic tables handle all complexity]

// ────────────────────────────────────────────────────────────────
// Core Operations - Validation
// ────────────────────────────────────────────────────────────────

// trit_valid checks if a trit value is valid (-1, 0, or +1).
//
// Parameters:
//   t: The trit value to check
//
// Returns:
//   true if t is -1, 0, or +1; false otherwise
//
bool trit_valid(trit_t t) {
    return (t >= TRIT_NEG && t <= TRIT_POS);
}

// trit_create creates a trit from an integer value.
//
// Parameters:
//   value: Integer to convert (-1, 0, or +1 expected)
//
// Returns:
//   The trit value if valid, TRIT_ZERO if invalid
//
trit_t trit_create(int value) {
    if (value < TRIT_NEG || value > TRIT_POS) {
        return TRIT_ZERO;  // Safe default for invalid input
    }
    return (trit_t)value;
}

// trit_value extracts the raw integer value from a trit.
//
// Parameters:
//   t: The trit to extract from
//
// Returns:
//   The integer value (-1, 0, or +1)
//
int trit_value(trit_t t) {
    return (int)t;
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Arithmetic
// ────────────────────────────────────────────────────────────────

// trit_negate negates a trit: -1→+1, 0→0, +1→-1.
//
// Uses NEGATION_TABLE from ternary-math.toml [arithmetic.negation].
//
// Parameters:
//   t: The trit to negate
//
// Returns:
//   The negated trit value
//
trit_t trit_negate(trit_t t) {
    // Table indexed by t + 1: -1→index 0, 0→index 1, +1→index 2
    return NEGATION_TABLE[TRIT_TO_UNSIGNED(t)];
}

// trit_add adds two trits (single-trit, no carry).
//
// Uses ADDITION_TABLE from ternary-math.toml [arithmetic.addition_no_carry].
// Note: This is simplified single-trit addition. Full results:
//   -1 + -1 = -2 → clamps to -1 (would need carry in multi-trit)
//   +1 + +1 = +2 → clamps to +1 (would need carry in multi-trit)
//
// Parameters:
//   a: First trit
//   b: Second trit
//
// Returns:
//   The sum (clamped to single trit range)
//
trit_t trit_add(trit_t a, trit_t b) {
    // Table indexed by [a+1][b+1]
    return ADDITION_TABLE[TRIT_TO_UNSIGNED(a)][TRIT_TO_UNSIGNED(b)];
}

// trit_multiply multiplies two trits.
//
// Uses MULTIPLICATION_TABLE from ternary-math.toml [arithmetic.multiplication].
// Single-trit multiplication never overflows.
//
// Parameters:
//   a: First trit
//   b: Second trit
//
// Returns:
//   The product
//
trit_t trit_multiply(trit_t a, trit_t b) {
    // Table indexed by [a+1][b+1]
    return MULTIPLICATION_TABLE[TRIT_TO_UNSIGNED(a)][TRIT_TO_UNSIGNED(b)];
}

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────

// [Reserved: Simple functions return safe defaults - no error handling needed]
// - trit_create(invalid) → TRIT_ZERO
// - Arithmetic functions assume valid input (caller's responsibility)

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────

// All functions in this file ARE the public API.
// See trit.h for the complete interface.

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
//   make              # Build library
//   make check-headers # Verify trit.h compiles
//
// Testing (after Part 5 unit tests):
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
//
// ════════════════════════════════════════════════════════════════
// FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// Purpose: See METADATA "Purpose & Function" section above
//
// Provides:
//   - trit_valid: Check if value is valid trit
//   - trit_create: Create trit from integer
//   - trit_value: Extract integer from trit
//   - trit_negate: Flip sign (-1↔+1, 0 unchanged)
//   - trit_add: Add two trits (no carry)
//   - trit_multiply: Multiply two trits
//
// Architecture: LADDER component - foundational building block
//   Other components build on these primitives.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new single-trit operations (follow existing patterns)
//   ✅ Add new arithmetic tables (with mathematical verification)
//
// Modify with Extreme Care:
//   ⚠️ Function signatures - breaks all calling code
//   ⚠️ Arithmetic tables - mathematical correctness critical
//   ⚠️ TRIT_TO_UNSIGNED macro - all table indexing depends on it
//
// NEVER Modify:
//   ❌ 4-block structure
//   ❌ Trit value range (-1, 0, +1)
//   ❌ Table-driven arithmetic approach
//
// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Dependency Diagram" for ladder structure.
//
// Baton: All functions are leaf operations - no orchestration.
//   Caller → trit_* → return
//
// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding new trit operation:
//   1. Add prototype to trit.h (Function Prototypes section)
//   2. If table-driven, add table in SETUP (Arithmetic Tables)
//   3. Implement in BODY following existing pattern
//   4. Add test in test/test_trit.c (Part 5)
//
// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// All operations are O(1) constant-time table lookups.
// No branches in hot paths. Predictable cache behavior.
//
// Tables are static const - compiler can inline values.
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: trit_create returns TRIT_ZERO unexpectedly
//   - Cause: Input value outside range [-1, +1]
//   - Solution: Validate input before calling, or use result of trit_valid
//
// Problem: Arithmetic results seem wrong
//   - Check: Are you using ternary (balanced) logic, not binary?
//   - Note: -1 + +1 = 0 is correct in balanced ternary (no carry)
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Dependencies" section for full details.
//
// Key relationships:
//   - trit.h: Type definitions and prototypes
//   - pack.c: Uses trit.c for multi-trit operations
//   - ternary-math.toml: Source of truth for arithmetic tables
//
// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned:
//   ⏳ trit_compare: Ordering operation
//   ⏳ trit_min/trit_max: Ternary min/max
//
// Version History:
//   0.1.0 (2025-12-12) - Initial implementation
//     - Six core operations (valid, create, value, negate, add, multiply)
//     - Table-driven arithmetic
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This is the LADDER foundation for all ternary operations.
// Everything in the trit library builds on these six primitives.
//
// Changes here affect all downstream code. Test thoroughly.
//
// "To every thing there is a season." — Ecclesiastes 3:1
//
// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Usage:
//
//     #include "trit.h"
//
//     trit_t a = trit_create(-1);   // TRIT_NEG
//     trit_t b = trit_create(+1);   // TRIT_POS
//
//     trit_t sum = trit_add(a, b);        // → TRIT_ZERO
//     trit_t product = trit_multiply(a, b); // → TRIT_NEG
//     trit_t neg_a = trit_negate(a);      // → TRIT_POS
//
//     if (trit_valid(a)) {
//         int val = trit_value(a);  // → -1
//     }
//

// ============================================================================
// END CLOSING
// ============================================================================
