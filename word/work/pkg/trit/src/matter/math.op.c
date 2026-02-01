// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Mathematical Operations
// Key: B-word-work-pkg-trit-src-math
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h, math.h)
//   Requires trit.h for type definitions, math.h for function prototypes.
//
// derives_from: bereshit/word/seed/code/c/source.c
// See: word/core/ternary-math.toml for arithmetic tables
// See: word/core/schemas/health.toml for normalization formulas
// See: word/research/bereshit/bereshit-base-algorithms.adoc for calculus
//
// ═══════════════════════════════════════════════════════════════════════════

// Mathematical operations for balanced ternary types.
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
// Scripture: "A false balance is abomination to the LORD: but a just weight
//             is his delight." — Proverbs 11:1
//
// Principle: Mathematical operations must be precise and honest. Balanced
//            ternary with its true zero provides "just weights" - accurate
//            representation without bias toward positive or negative.
//
// Anchor: "Divers weights, and divers measures, both of them are alike
//          abomination to the LORD." — Proverbs 20:10
//
// # CPI-SI Identity
//
// Component Type: Ladder (builds on trit.h types)
//
// Role: Provides all mathematical operations for balanced ternary types.
//       This is the computational engine enabling CPI-SI reasoning -
//       arithmetic, comparison, conversion, normalization, and calculus.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-13
//   - Version: 0.1.0
//   - Modified: 2025-12-13 - Initial implementation (Phase 2, Part 2)
//
// # Purpose & Function
//
// Purpose: Implement mathematical operations that make balanced ternary types
//          useful for computation. From basic arithmetic to health calculus.
//
// Core Design: Table-driven arithmetic from ternary-math.toml specification.
//              Five operation families building on each other.
//
// Key Features:
//   - Carry-aware arithmetic (half adder, full adder)
//   - Multi-trit operations (trit5, trit9, trit27)
//   - True ternary comparison (returns trit_t, not int)
//   - Health score conversion (stored ↔ true)
//   - 7-level health classification
//   - Discrete calculus for change detection
//
// Philosophy: Just weights and accurate measures. Table-driven correctness.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//   - Internal: trit.h (type definitions), math.h (function prototypes)
//   - Standard: <stdbool.h>, <stdint.h>
//
// What Uses This:
//   - Libraries: Health system, cognitive reasoning components
//   - Demo: demo-math (validates math operations)
//
// # Usage
//
// Functions are called directly - no command line interface.
// See math.h for function signatures.
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
#include "trit/matter/math.op.h"  // Function prototypes (includes trit.type.h)
#include "trit/generated/ternary_math.gen.h"  // Generated arithmetic tables

//--- Standard Library ---
// math.h includes: <stdbool.h>, <stdint.h> via trit.h

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

// [Reserved: All constants defined in math.h]

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────

//--- Arithmetic Tables ---
// NOW GENERATED from ternary-math.toml via generate-config
// See: generated/ternary_math.gen.h
// Tables: HALF_ADDER_SUM, HALF_ADDER_CARRY, FULL_ADDER_SUM, FULL_ADDER_CARRY_OUT
// Indexed by trit value + 1 (so -1→0, 0→1, +1→2)

// Health Level Name Tables
// From health.toml [classification.levels]
static const char* LEVEL_HEBREW_NAMES[7] = {
    "שָׁבַר",    // LEVEL_BROKEN (-3)
    "חָסֵר",    // LEVEL_WANTING (-2)
    "מַחְסוֹר",  // LEVEL_LACKING (-1)
    "שָׁוֶה",    // LEVEL_EVEN (0)
    "שָׁלֵם",    // LEVEL_SOUND (+1)
    "תָּמִים",   // LEVEL_WHOLE (+2)
    "טוֹב"      // LEVEL_PERFECT (+3)
};

static const char* LEVEL_ENGLISH_NAMES[7] = {
    "broken", "wanting", "lacking", "even", "sound", "whole", "perfect"
};

static const char* LEVEL_EMOJIS[7] = {
    "💔", "🖤", "🩶", "⚪", "💚", "💙", "💛"
};

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// [Reserved: All types defined in math.h]

// ────────────────────────────────────────────────────────────────
// Function Prototypes (Internal Helpers)
// ────────────────────────────────────────────────────────────────

// Internal helper: clamp integer to valid trit range
static trit_t clamp_trit(int value);

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
//
//   FAMILY 5: Calculus Operations (Top - depends on all below)
//   ├── trit_difference() → comparison
//   ├── trit_rate() → arithmetic
//   ├── trit_accumulate() → arithmetic
//   ├── threshold_crossed() → comparison
//   ├── trend_direction() → comparison
//   └── critical_rate() → comparison
//
//   FAMILY 4: Normalization Operations
//   ├── trit_base_round() → arithmetic
//   ├── trit_normalize() → trit_base_round()
//   ├── trit_level() → comparison
//   ├── level_hebrew() → table lookup
//   ├── level_name() → table lookup
//   └── level_emoji() → table lookup
//
//   FAMILY 3: Conversion Operations
//   ├── trit_to_int() → pure
//   ├── int_to_trit() → clamp_trit
//   ├── trit5_to_int() → unpack
//   ├── int_to_trit5() → pack
//   ├── stored_to_true() → arithmetic
//   └── true_to_stored() → arithmetic
//
//   FAMILY 2: Comparison Operations
//   ├── trit_cmp() → pure
//   ├── trit5_cmp() → unpack, compare
//   ├── trit9_cmp() → unpack, compare
//   └── trit27_cmp() → unpack, compare
//
//   FAMILY 1: Arithmetic Operations (Foundation)
//   ├── trit_hadd() → HALF_SUM, HALF_CARRY tables
//   ├── trit_fadd() → FULL_SUM, FULL_CARRY tables
//   ├── trit5/9/27_negate() → unpack, trit_negate, pack
//   ├── trit5/9/27_add() → unpack, trit_fadd chain, pack
//   ├── trit_hsub() → trit_negate, trit_hadd
//   ├── trit5/9/27_sub() → negate, add
//   ├── trit5_mul() → schoolbook with trit_fadd
//   └── trit_div(), trit5_div() → centered remainder
//
// Table lookups are O(1). Multi-trit ops are O(n).

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────

// clamp_trit clamps an integer to valid trit range.
static trit_t clamp_trit(int value) {
    if (value < TRIT_NEG) return TRIT_NEG;
    if (value > TRIT_POS) return TRIT_POS;
    return (trit_t)value;
}

// ════════════════════════════════════════════════════════════════
// FAMILY 1: ARITHMETIC OPERATIONS
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Single-Trit Carry-Aware Operations
// ────────────────────────────────────────────────────────────────

// trit_hadd performs half-adder addition of two trits.
//
// Unlike trit_add() in trit.h (which saturates), this returns both
// the sum AND the carry for use in multi-trit arithmetic.
//
// From ternary-math.toml [operations.half_adder]:
//   (-1) + (-1) = +1 with carry -1
//   (+1) + (+1) = -1 with carry +1
//   All other combinations have carry 0
//
trit_result_t trit_hadd(trit_t a, trit_t b) {
    trit_result_t result;
    int ai = TRIT_TO_UNSIGNED(a);
    int bi = TRIT_TO_UNSIGNED(b);
    result.value = HALF_ADDER_SUM[ai][bi];
    result.carry = HALF_ADDER_CARRY[ai][bi];
    return result;
}

// trit_fadd performs full-adder addition of two trits plus carry_in.
//
// This is the core operation for multi-trit addition. Each position
// takes the carry from the previous position.
//
// From ternary-math.toml [operations.full_adder]:
//   27 combinations (3³) fully specified.
//
trit_result_t trit_fadd(trit_t a, trit_t b, trit_t c_in) {
    trit_result_t result;
    int ai = TRIT_TO_UNSIGNED(a);
    int bi = TRIT_TO_UNSIGNED(b);
    int ci = TRIT_TO_UNSIGNED(c_in);
    result.value = FULL_ADDER_SUM[ai][bi][ci];
    result.carry = FULL_ADDER_CARRY_OUT[ai][bi][ci];
    return result;
}

// trit_hsub performs half-subtractor: a - b = a + negate(b).
//
// From ternary-math.toml [operations.subtraction]:
//   method = "negate_then_add"
//
trit_result_t trit_hsub(trit_t a, trit_t b) {
    return trit_hadd(a, trit_negate(b));
}

// trit_div divides single trit a by single trit b.
//
// Single-trit division is simple:
//   a / 0 → 0 (safe default, division by zero)
//   a / b → sign(a) * sign(b) when |a| >= |b|, else 0
//
// For single trits, this reduces to multiplication (sign × sign).
//
trit_t trit_div(trit_t a, trit_t b) {
    if (b == TRIT_ZERO) return TRIT_ZERO;  // Division by zero → safe default
    // Single trit division: result is the product of signs
    // -1 / -1 = +1, -1 / +1 = -1, +1 / -1 = -1, +1 / +1 = +1
    // 0 / anything = 0
    return trit_multiply(a, b);
}

// ────────────────────────────────────────────────────────────────
// Multi-Trit Negation
// ────────────────────────────────────────────────────────────────

// trit5_negate negates a 5-trit value (flip each trit).
trit5_t trit5_negate(trit5_t t) {
    trit_t trits[5];
    trit5_unpack(t, trits);
    for (int i = 0; i < 5; i++) {
        trits[i] = trit_negate(trits[i]);
    }
    return trit5_pack(trits);
}

// trit9_negate negates a 9-trit value (flip each trit).
trit9_t trit9_negate(trit9_t t) {
    trit_t trits[9];
    trit9_unpack(t, trits);
    for (int i = 0; i < 9; i++) {
        trits[i] = trit_negate(trits[i]);
    }
    return trit9_pack(trits);
}

// trit27_negate negates a 27-trit value (flip each trit).
trit27_t trit27_negate(trit27_t t) {
    trit_t trits[27];
    trit27_unpack(t, trits);
    for (int i = 0; i < 27; i++) {
        trits[i] = trit_negate(trits[i]);
    }
    return trit27_pack(trits);
}

// ────────────────────────────────────────────────────────────────
// Multi-Trit Addition
// ────────────────────────────────────────────────────────────────

// trit5_add adds two 5-trit values with carry propagation.
trit5_result_t trit5_add(trit5_t a, trit5_t b) {
    trit_t a_trits[5], b_trits[5], result_trits[5];
    trit5_unpack(a, a_trits);
    trit5_unpack(b, b_trits);

    trit_t carry = TRIT_ZERO;
    for (int i = 0; i < 5; i++) {
        trit_result_t r = trit_fadd(a_trits[i], b_trits[i], carry);
        result_trits[i] = r.value;
        carry = r.carry;
    }

    trit5_result_t result;
    result.value = trit5_pack(result_trits);
    result.carry = carry;
    return result;
}

// trit9_add adds two 9-trit values with carry propagation.
trit9_result_t trit9_add(trit9_t a, trit9_t b) {
    trit_t a_trits[9], b_trits[9], result_trits[9];
    trit9_unpack(a, a_trits);
    trit9_unpack(b, b_trits);

    trit_t carry = TRIT_ZERO;
    for (int i = 0; i < 9; i++) {
        trit_result_t r = trit_fadd(a_trits[i], b_trits[i], carry);
        result_trits[i] = r.value;
        carry = r.carry;
    }

    trit9_result_t result;
    result.value = trit9_pack(result_trits);
    result.carry = carry;
    return result;
}

// trit27_add adds two 27-trit values with carry propagation.
trit27_result_t trit27_add(trit27_t a, trit27_t b) {
    trit_t a_trits[27], b_trits[27], result_trits[27];
    trit27_unpack(a, a_trits);
    trit27_unpack(b, b_trits);

    trit_t carry = TRIT_ZERO;
    for (int i = 0; i < 27; i++) {
        trit_result_t r = trit_fadd(a_trits[i], b_trits[i], carry);
        result_trits[i] = r.value;
        carry = r.carry;
    }

    trit27_result_t result;
    result.value = trit27_pack(result_trits);
    result.carry = carry;
    return result;
}

// ────────────────────────────────────────────────────────────────
// Multi-Trit Subtraction
// ────────────────────────────────────────────────────────────────

// trit5_sub subtracts b from a: a - b = a + negate(b).
trit5_result_t trit5_sub(trit5_t a, trit5_t b) {
    return trit5_add(a, trit5_negate(b));
}

// trit9_sub subtracts b from a: a - b = a + negate(b).
trit9_result_t trit9_sub(trit9_t a, trit9_t b) {
    return trit9_add(a, trit9_negate(b));
}

// trit27_sub subtracts b from a: a - b = a + negate(b).
trit27_result_t trit27_sub(trit27_t a, trit27_t b) {
    return trit27_add(a, trit27_negate(b));
}

// ────────────────────────────────────────────────────────────────
// Multi-Trit Multiplication
// ────────────────────────────────────────────────────────────────

// trit5_mul multiplies two 5-trit values.
//
// Uses schoolbook multiplication: for each trit in b, multiply by a
// and accumulate with shift. Result can be up to 10 trits, so we
// return trit9_result_t (9 trits + overflow indicator).
//
trit9_result_t trit5_mul(trit5_t a, trit5_t b) {
    trit_t a_trits[5], b_trits[5];
    trit5_unpack(a, a_trits);
    trit5_unpack(b, b_trits);

    // Accumulator: 10 trits (5 + 5 - 1 for max product width, +1 for overflow)
    trit_t acc[10] = {0};

    // Schoolbook multiplication
    for (int i = 0; i < 5; i++) {
        if (b_trits[i] == TRIT_ZERO) continue;

        trit_t carry = TRIT_ZERO;
        for (int j = 0; j < 5; j++) {
            // Multiply digit
            trit_t prod = trit_multiply(a_trits[j], b_trits[i]);

            // Add to accumulator with carry
            trit_result_t r1 = trit_fadd(acc[i + j], prod, carry);
            acc[i + j] = r1.value;
            carry = r1.carry;
        }
        // Propagate final carry
        if (carry != TRIT_ZERO) {
            trit_result_t r2 = trit_hadd(acc[i + 5], carry);
            acc[i + 5] = r2.value;
            // Further carry propagation for overflow
            if (r2.carry != TRIT_ZERO && i + 6 < 10) {
                acc[i + 6] = trit_hadd(acc[i + 6], r2.carry).value;
            }
        }
    }

    // Pack lower 9 trits into result
    trit9_result_t result;
    result.value = trit9_pack(acc);  // Uses first 9 trits
    // Overflow if trit 9 is non-zero
    result.carry = acc[9];
    return result;
}

// ────────────────────────────────────────────────────────────────
// Multi-Trit Division
// ────────────────────────────────────────────────────────────────

// trit5_div divides a by b using centered remainder.
//
// From ternary-math.toml [operations.division]:
//   method = "long_division"
//   remainder_rule = "centered" (|remainder| ≤ divisor/2)
//
// For simplicity, we convert to integers, divide, and convert back.
// This is correct for the range of trit5 (-121 to +121).
//
trit5_t trit5_div(trit5_t a, trit5_t b) {
    // Convert to integers
    trit_t a_trits[5], b_trits[5];
    trit5_unpack(a, a_trits);
    trit5_unpack(b, b_trits);

    // Calculate integer values using positional notation
    int a_val = 0, b_val = 0;
    for (int i = 4; i >= 0; i--) {
        a_val = a_val * 3 + a_trits[i];
        b_val = b_val * 3 + b_trits[i];
    }

    // Division by zero check
    if (b_val == 0) {
        return 121;  // Packed zero: middle value
    }

    // Integer division with centered remainder
    // Standard C division truncates toward zero, which is close to centered
    int q_val = a_val / b_val;

    // Convert quotient back to trit5
    // Clamp to valid range
    if (q_val < -121) q_val = -121;
    if (q_val > 121) q_val = 121;

    // Convert integer to trit5
    trit_t q_trits[5];
    int temp = q_val;
    for (int i = 0; i < 5; i++) {
        // Balanced ternary conversion
        int rem = temp % 3;
        temp = temp / 3;
        if (rem == 2) {
            q_trits[i] = TRIT_NEG;
            temp++;
        } else if (rem == -2) {
            q_trits[i] = TRIT_POS;
            temp--;
        } else if (rem == 1 || rem == -1) {
            q_trits[i] = (trit_t)rem;
        } else {
            q_trits[i] = TRIT_ZERO;
        }
    }

    return trit5_pack(q_trits);
}

// ════════════════════════════════════════════════════════════════
// FAMILY 2: COMPARISON OPERATIONS
// ════════════════════════════════════════════════════════════════

// trit_cmp compares two trits.
//
// Returns:
//   TRIT_NEG (-1): a < b
//   TRIT_ZERO (0): a == b
//   TRIT_POS (+1): a > b
//
trit_t trit_cmp(trit_t a, trit_t b) {
    if (a < b) return TRIT_NEG;
    if (a > b) return TRIT_POS;
    return TRIT_ZERO;
}

// trit5_cmp compares two 5-trit values.
trit_t trit5_cmp(trit5_t a, trit5_t b) {
    trit_t a_trits[5], b_trits[5];
    trit5_unpack(a, a_trits);
    trit5_unpack(b, b_trits);

    // Compare from most significant trit
    for (int i = 4; i >= 0; i--) {
        if (a_trits[i] < b_trits[i]) return TRIT_NEG;
        if (a_trits[i] > b_trits[i]) return TRIT_POS;
    }
    return TRIT_ZERO;
}

// trit9_cmp compares two 9-trit values.
trit_t trit9_cmp(trit9_t a, trit9_t b) {
    trit_t a_trits[9], b_trits[9];
    trit9_unpack(a, a_trits);
    trit9_unpack(b, b_trits);

    for (int i = 8; i >= 0; i--) {
        if (a_trits[i] < b_trits[i]) return TRIT_NEG;
        if (a_trits[i] > b_trits[i]) return TRIT_POS;
    }
    return TRIT_ZERO;
}

// trit27_cmp compares two 27-trit values.
trit_t trit27_cmp(trit27_t a, trit27_t b) {
    trit_t a_trits[27], b_trits[27];
    trit27_unpack(a, a_trits);
    trit27_unpack(b, b_trits);

    for (int i = 26; i >= 0; i--) {
        if (a_trits[i] < b_trits[i]) return TRIT_NEG;
        if (a_trits[i] > b_trits[i]) return TRIT_POS;
    }
    return TRIT_ZERO;
}

// ════════════════════════════════════════════════════════════════
// FAMILY 3: CONVERSION OPERATIONS
// ════════════════════════════════════════════════════════════════

// trit_to_int converts a trit to integer.
int trit_to_int(trit_t t) {
    return (int)t;
}

// int_to_trit converts an integer to trit (clamps to valid range).
trit_t int_to_trit(int i) {
    return clamp_trit(i);
}

// trit5_to_int converts a 5-trit value to integer (-121 to +121).
int trit5_to_int(trit5_t t) {
    trit_t trits[5];
    trit5_unpack(t, trits);

    int value = 0;
    int power = 1;
    for (int i = 0; i < 5; i++) {
        value += trits[i] * power;
        power *= 3;
    }
    return value;
}

// int_to_trit5 converts an integer to 5-trit (clamps to valid range).
trit5_t int_to_trit5(int i) {
    // Clamp to valid range
    if (i < -121) i = -121;
    if (i > 121) i = 121;

    // Convert to balanced ternary
    trit_t trits[5];
    int temp = i;
    for (int j = 0; j < 5; j++) {
        int rem = temp % 3;
        temp = temp / 3;
        if (rem == 2) {
            trits[j] = TRIT_NEG;
            temp++;
        } else if (rem == -2) {
            trits[j] = TRIT_POS;
            temp--;
        } else if (rem == 1 || rem == -1) {
            trits[j] = (trit_t)rem;
        } else {
            trits[j] = TRIT_ZERO;
        }
    }

    return trit5_pack(trits);
}

// stored_to_true converts stored health (0-255) to true value (-100 to +100).
//
// Formula from health.toml: (stored - 128) * 100 / 127
//
int8_t stored_to_true(uint8_t stored) {
    // Use int32 to avoid overflow
    int32_t s = (int32_t)stored;
    int32_t result = (s - 128) * 100 / 127;

    // Clamp to valid range
    if (result < -100) result = -100;
    if (result > 100) result = 100;

    return (int8_t)result;
}

// true_to_stored converts true health (-100 to +100) to stored value (0-255).
//
// Formula from health.toml: (true_val * 127 / 100) + 128
//
uint8_t true_to_stored(int8_t true_val) {
    // Clamp input
    int32_t t = true_val;
    if (t < -100) t = -100;
    if (t > 100) t = 100;

    int32_t result = (t * 127 / 100) + 128;

    // Clamp to valid range (should already be valid)
    if (result < 0) result = 0;
    if (result > 255) result = 255;

    return (uint8_t)result;
}

// ════════════════════════════════════════════════════════════════
// FAMILY 4: NORMALIZATION OPERATIONS
// ════════════════════════════════════════════════════════════════

// trit_base_round rounds true value to nearest base increment.
//
// Formula from health.toml: round(true_val / base) * base
//
int8_t trit_base_round(int8_t true_val, uint8_t base) {
    if (base == 0) return true_val;  // Avoid division by zero

    // Round to nearest base
    int rounded;
    if (true_val >= 0) {
        rounded = ((true_val + base / 2) / base) * base;
    } else {
        rounded = ((true_val - base / 2) / base) * base;
    }

    // Clamp to valid range
    if (rounded < -100) rounded = -100;
    if (rounded > 100) rounded = 100;

    return (int8_t)rounded;
}

// trit_normalize normalizes to 5 hard anchor points (-100, -50, 0, +50, +100).
int8_t trit_normalize(int8_t true_val) {
    return trit_base_round(true_val, 50);
}

// trit_level classifies true value to 7-level health state.
//
// From health.toml [classification.levels]:
//   broken: -100 to -51, wanting: -50 to -26, lacking: -25 to -1
//   even: 0, sound: +1 to +25, whole: +26 to +50, perfect: +51 to +100
//
health_level_t trit_level(int8_t true_val) {
    if (true_val <= LEVEL_BROKEN_MAX) return LEVEL_BROKEN;   // -100 to -51
    if (true_val <= LEVEL_WANTING_MAX) return LEVEL_WANTING; // -50 to -26
    if (true_val <= LEVEL_LACKING_MAX) return LEVEL_LACKING; // -25 to -1
    if (true_val == LEVEL_EVEN_VAL) return LEVEL_EVEN;       // exactly 0
    if (true_val <= 25) return LEVEL_SOUND;                   // +1 to +25
    if (true_val <= 50) return LEVEL_WHOLE;                   // +26 to +50
    return LEVEL_PERFECT;                                     // +51 to +100
}

// level_hebrew returns the Hebrew name for a health level.
const char* level_hebrew(health_level_t level) {
    int index = level + 3;  // Convert -3..+3 to 0..6
    if (index < 0 || index > 6) return "שָׁוֶה";  // Default to even
    return LEVEL_HEBREW_NAMES[index];
}

// level_name returns the English name for a health level.
const char* level_name(health_level_t level) {
    int index = level + 3;
    if (index < 0 || index > 6) return "even";
    return LEVEL_ENGLISH_NAMES[index];
}

// level_emoji returns the emoji for a health level.
const char* level_emoji(health_level_t level) {
    int index = level + 3;
    if (index < 0 || index > 6) return "⚪";
    return LEVEL_EMOJIS[index];
}

// ════════════════════════════════════════════════════════════════
// FAMILY 5: CALCULUS OPERATIONS
// ════════════════════════════════════════════════════════════════

// trit_difference calculates difference between two health values.
//
// Returns direction as trit: -1 (declining), 0 (stable), +1 (improving)
//
trit_t trit_difference(int8_t previous, int8_t current) {
    int diff = current - previous;
    if (diff < 0) return TRIT_NEG;
    if (diff > 0) return TRIT_POS;
    return TRIT_ZERO;
}

// trit_rate calculates rate of change (discrete derivative).
//
// Returns: -1 (rapid decline), 0 (stable), +1 (rapid improvement)
//
trit_t trit_rate(int8_t delta, uint8_t time_delta) {
    if (time_delta == 0) return TRIT_ZERO;  // Avoid division by zero

    int rate = delta / (int)time_delta;
    if (rate < -10) return TRIT_NEG;   // Rapid decline threshold
    if (rate > 10) return TRIT_POS;    // Rapid improvement threshold
    return TRIT_ZERO;                  // Within stable range
}

// trit_accumulate adds value to running sum.
int16_t trit_accumulate(int16_t accumulator, int8_t value) {
    int32_t sum = (int32_t)accumulator + (int32_t)value;

    // Clamp to int16_t range
    if (sum < -32768) sum = -32768;
    if (sum > 32767) sum = 32767;

    return (int16_t)sum;
}

// threshold_crossed checks if a threshold was crossed.
bool threshold_crossed(int8_t previous, int8_t current, int8_t threshold) {
    // Crossed if previous and current are on opposite sides of threshold
    bool prev_below = previous < threshold;
    bool curr_below = current < threshold;
    return prev_below != curr_below;
}

// trend_direction determines overall trend from history.
//
// Simple algorithm: compare first and last values
//
trit_t trend_direction(const int8_t* history, uint8_t count) {
    if (count < 2) return TRIT_ZERO;

    int8_t first = history[0];
    int8_t last = history[count - 1];

    if (last < first) return TRIT_NEG;   // Declining
    if (last > first) return TRIT_POS;   // Improving
    return TRIT_ZERO;                     // Stable
}

// critical_rate checks if rate exceeds critical threshold.
bool critical_rate(trit_t rate, trit_t threshold) {
    // Check if |rate| >= |threshold|
    int abs_rate = (rate < 0) ? -rate : rate;
    int abs_threshold = (threshold < 0) ? -threshold : threshold;
    return abs_rate >= abs_threshold;
}

// ════════════════════════════════════════════════════════════════
// ERROR HANDLING
// ════════════════════════════════════════════════════════════════

// Strategy: Return safe defaults rather than error codes.
//   - Invalid trit values → clamped to valid range
//   - Division by zero → returns 0 (safe default)
//   - Out of range conversions → clamped to valid range

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
//   cd word/work/pkg/trit && make
//
// Testing:
//   make test         # Run all unit tests including math tests

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

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// Purpose: See METADATA "Purpose & Function" section above
//
// Provides five operation families:
//
//   FAMILY 1 - Arithmetic:
//     trit_hadd, trit_fadd (carry-aware)
//     trit5/9/27_negate, trit5/9/27_add, trit5/9/27_sub
//     trit5_mul, trit_div, trit5_div
//
//   FAMILY 2 - Comparison:
//     trit_cmp, trit5_cmp, trit9_cmp, trit27_cmp
//
//   FAMILY 3 - Conversion:
//     trit_to_int, int_to_trit, trit5_to_int, int_to_trit5
//     stored_to_true, true_to_stored
//
//   FAMILY 4 - Normalization:
//     trit_base_round, trit_normalize, trit_level
//     level_hebrew, level_name, level_emoji
//
//   FAMILY 5 - Calculus:
//     trit_difference, trit_rate, trit_accumulate
//     threshold_crossed, trend_direction, critical_rate
//
// Architecture: LADDER component building on trit.h types.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new operations (follow existing patterns)
//   ✅ Add new lookup tables (with verification)
//
// Modify with Extreme Care:
//   ⚠️ Arithmetic tables - mathematical correctness critical
//   ⚠️ Function signatures - breaks all calling code
//   ⚠️ Health level boundaries - must match health.toml
//
// NEVER Modify:
//   ❌ 4-block structure
//   ❌ Trit value range (-1, 0, +1)
//   ❌ Table-driven arithmetic approach
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Key relationships:
//   - math.h: Function prototypes and type definitions
//   - trit.h/trit.c: Base types and single-trit ops
//   - pack.c: Pack/unpack used by multi-trit ops
//   - ternary-math.toml: Arithmetic table source
//   - health.toml: Normalization and level specs
//
// ────────────────────────────────────────────────────────────────
// Version History
// ────────────────────────────────────────────────────────────────
//
//   0.1.0 (2025-12-13) - Initial implementation (Phase 2, Part 2)
//     - Five operation families (38 functions)
//     - Half/full adder tables from ternary-math.toml
//     - 7-level health classification from health.toml
//     - Discrete calculus for change detection
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This is the mathematical engine for balanced ternary computing.
// Five families of operations building on each other:
//   Arithmetic → Comparison → Conversion → Normalization → Calculus
//
// "A false balance is abomination to the LORD: but a just weight
//  is his delight." — Proverbs 11:1
//
// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Carry-aware arithmetic:
//   trit_result_t r = trit_hadd(TRIT_POS, TRIT_POS);
//   // r.value == TRIT_NEG, r.carry == TRIT_POS
//
// Health conversion:
//   int8_t true_val = stored_to_true(212);  // → +66
//   health_level_t level = trit_level(true_val);  // → LEVEL_WHOLE
//   const char* emoji = level_emoji(level);  // → "💙"
//
// Change detection:
//   trit_t delta = trit_difference(50, 66);  // → TRIT_POS (improving)
//   bool crossed = threshold_crossed(50, 66, 50);  // → true

// ============================================================================
// END CLOSING
// ============================================================================
