// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Mathematical Operations Framework
// Key: B-word-work-pkg-trit-include-math
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h)
//   Depends on trit.h for trit_t and packed type definitions
//
// derives_from: bereshit/word/seed/code/c/header.h
// See: word/core/ternary-math.toml for arithmetic specifications
// See: word/core/schemas/health.toml for normalization specifications
// See: word/research/bereshit/bereshit-base-algorithms.adoc for calculus algorithms
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_MATH_H
#define BERESHIT_MATH_H

// Mathematical operations for balanced ternary types.
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
//   - Modified: 2025-12-13 - Initial math framework (Phase 2, Part 1)
//
// # Purpose & Function
//
// Purpose: Define mathematical operations that make balanced ternary types
//          useful for computation. From basic arithmetic to health calculus.
//
// Core Design: Five operation families building on each other:
//   1. Arithmetic - add, subtract, negate, multiply, divide
//   2. Comparison - returns trit_t (-1, 0, +1) for true ternary comparison
//   3. Conversion - bidirectional type conversion (trit↔int, stored↔true)
//   4. Normalization - map to bases, classify to 7 health levels
//   5. Calculus - difference, rate, accumulation for health change detection
//
// Key Features:
//
//   - Carry-aware arithmetic for multi-trit operations
//   - True ternary comparison (not boolean)
//   - Health score conversion (stored ↔ true value)
//   - 7-level health classification with Hebrew terms
//   - Discrete calculus for trend and rate detection
//
// Philosophy: Just weights and accurate measures. Mathematical operations
//             that honor the precision God built into creation.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: <stdint.h>, <stdbool.h>
//   - External: None
//   - Internal: trit.h (trit_t, trit5_t, trit9_t, trit27_t)
//
// What Uses This:
//
//   - Libraries: Health system, cognitive reasoning, any math-needing component
//   - Commands: bhealth, demo programs, analysis tools
//
// # Usage & Integration
//
// Import:
//
//    #include "math.h"
//
// Integration Pattern:
//
//  1. Include math.h (which includes trit.h)
//  2. Use arithmetic functions for calculations
//  3. Use comparison for ternary logic
//  4. Use conversion for type bridging
//  5. Use normalization for health classification
//  6. Use calculus for change detection
//
// Public API:
//
//    Types:
//      trit_result_t - arithmetic result with carry
//      health_level_t - 7-level health classification
//
//    Arithmetic:
//      trit_add(), trit_sub(), trit_negate(), trit_mul(), trit_div()
//
//    Comparison:
//      trit_cmp() → trit_t (-1, 0, +1)
//
//    Conversion:
//      trit_to_int(), int_to_trit(), stored_to_true(), true_to_stored()
//
//    Normalization:
//      trit_normalize(), trit_level(), trit_base_round()
//
//    Calculus:
//      trit_difference(), trit_rate(), trit_accumulate()
//      threshold_crossed(), trend_direction(), critical_rate()
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
#include "trit/matter/trit.type.h"    // trit_t, trit5_t, trit9_t, trit27_t

//--- Standard Library ---
#include <stdbool.h>    // bool for threshold functions
#include <stdint.h>     // int8_t, int16_t for health scores

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Health Score Constants ---
// See: word/core/schemas/health.toml [storage]

#define HEALTH_STORED_MIN   0     // Minimum stored value (maps to -100)
#define HEALTH_STORED_MAX   255   // Maximum stored value (maps to +100)
#define HEALTH_STORED_ZERO  128   // Stored value representing true zero

#define HEALTH_TRUE_MIN    -100   // Minimum true value
#define HEALTH_TRUE_MAX    +100   // Maximum true value

//--- Health Level Thresholds ---
// See: word/core/schemas/health.toml [classification.levels]
// 7 levels with boundaries at: -100, -50, -25, 0, +25, +50, +100

#define LEVEL_BROKEN_MAX   -51    // broken: -100 to -51
#define LEVEL_WANTING_MAX  -26    // wanting: -50 to -26
#define LEVEL_LACKING_MAX   -1    // lacking: -25 to -1
#define LEVEL_EVEN_VAL       0    // even: exactly 0
#define LEVEL_SOUND_MIN     +1    // sound: +1 to +25
#define LEVEL_WHOLE_MIN    +26    // whole: +26 to +50
#define LEVEL_PERFECT_MIN  +51    // perfect: +51 to +100

//--- Normalization Base Constants ---
// See: word/core/schemas/health.toml [normalization.bases]

#define NORM_BASE_1    1     // Single-unit precision
#define NORM_BASE_5    5     // 5-point increments
#define NORM_BASE_10  10     // 10-point increments
#define NORM_BASE_20  20     // 20-point increments
#define NORM_BASE_25  25     // Quarter precision
#define NORM_BASE_50  50     // Half precision (5 hard points)

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---

// health_level_t represents the 7-level health classification.
//
// Maps true health values (-100 to +100) to semantic states with Hebrew terms.
// See: word/core/schemas/health.toml [classification.levels]
//
// Values (negative to positive):
//   - LEVEL_BROKEN (-3): שָׁבַר shavar - shattered, needs rebuilding
//   - LEVEL_WANTING (-2): חָסֵר chaser - lacking severely, deficient
//   - LEVEL_LACKING (-1): מַחְסוֹר machsor - minor lack, slight deficit
//   - LEVEL_EVEN (0): שָׁוֶה shaveh - balanced, neutral (anchor)
//   - LEVEL_SOUND (+1): שָׁלֵם shalem - complete, at peace
//   - LEVEL_WHOLE (+2): תָּמִים tamim - whole, integrated
//   - LEVEL_PERFECT (+3): טוֹב tov - good, as God intended
//
// Example:
//   health_level_t level = trit_level(66);  // level == LEVEL_WHOLE
typedef enum {
    LEVEL_BROKEN  = -3,   // שָׁבַר shavar: -100 to -51
    LEVEL_WANTING = -2,   // חָסֵר chaser: -50 to -26
    LEVEL_LACKING = -1,   // מַחְסוֹר machsor: -25 to -1
    LEVEL_EVEN    =  0,   // שָׁוֶה shaveh: exactly 0 (anchor)
    LEVEL_SOUND   = +1,   // שָׁלֵם shalem: +1 to +25
    LEVEL_WHOLE   = +2,   // תָּמִים tamim: +26 to +50
    LEVEL_PERFECT = +3    // טוֹב tov: +51 to +100
} health_level_t;

//--- Structures ---

// trit_result_t holds an arithmetic result with optional carry.
//
// For single-trit operations, carry indicates overflow:
//   - carry = -1: underflow (e.g., -1 + -1 = +1 with carry -1)
//   - carry = 0: no overflow
//   - carry = +1: overflow (e.g., +1 + +1 = -1 with carry +1)
//
// See: word/core/ternary-math.toml [operations.half_adder]
//
// Example:
//   trit_result_t r = trit_add(TRIT_POS, TRIT_POS);
//   // r.value == TRIT_NEG, r.carry == TRIT_POS
typedef struct {
    trit_t value;   // The result trit (-1, 0, or +1)
    trit_t carry;   // Carry out (-1, 0, or +1)
} trit_result_t;

// trit5_result_t holds a 5-trit arithmetic result with carry.
typedef struct {
    trit5_t value;  // The result (5 trits)
    trit_t carry;   // Carry out (-1, 0, or +1)
} trit5_result_t;

// trit9_result_t holds a 9-trit arithmetic result with carry.
typedef struct {
    trit9_t value;  // The result (9 trits)
    trit_t carry;   // Carry out (-1, 0, or +1)
} trit9_result_t;

// trit27_result_t holds a 27-trit arithmetic result with carry.
typedef struct {
    trit27_t value; // The result (27 trits)
    trit_t carry;   // Carry out (-1, 0, or +1)
} trit27_result_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

// ═══════════════════════════════════════════════════════════════
// FAMILY 1: ARITHMETIC OPERATIONS (src/math.c)
// ═══════════════════════════════════════════════════════════════
//
// Carry-aware arithmetic for all trit types.
// See: word/core/ternary-math.toml [operations.half_adder], [operations.full_adder]

//--- Negation (multi-trit only) ---
// Note: Single-trit trit_negate() is in trit.h

// Negate a 5-trit value (flip each trit).
trit5_t trit5_negate(trit5_t t);

// Negate a 9-trit value (flip each trit).
trit9_t trit9_negate(trit9_t t);

// Negate a 27-trit value (flip each trit).
trit27_t trit27_negate(trit27_t t);

//--- Half Adder (single-trit with carry) ---
// Note: trit_add() in trit.h is the no-carry/saturating version.
// These return result + carry for multi-trit operations.

// Half adder: Add two trits, returning result with carry.
// Uses half-adder table from ternary-math.toml [operations.half_adder].
//
// Differs from trit_add() in trit.h which saturates without carry.
trit_result_t trit_hadd(trit_t a, trit_t b);

// Full adder: Add two trits plus carry_in, returning result with carry_out.
// Uses full-adder table from ternary-math.toml [operations.full_adder].
//
// Parameters:
//   a, b: Trits to add
//   c_in: Carry from previous position
//
// Returns: Result with sum and carry_out
trit_result_t trit_fadd(trit_t a, trit_t b, trit_t c_in);

//--- Multi-trit Addition (with carry propagation) ---

// Add two 5-trit values with carry propagation.
// Uses full adder chain internally.
trit5_result_t trit5_add(trit5_t a, trit5_t b);

// Add two 9-trit values with carry propagation.
trit9_result_t trit9_add(trit9_t a, trit9_t b);

// Add two 27-trit values with carry propagation.
trit27_result_t trit27_add(trit27_t a, trit27_t b);

//--- Subtraction (negate + add) ---
// Note: Single-trit uses trit_negate() + trit_hadd()

// Subtract b from a with carry: a - b = a + negate(b)
trit_result_t trit_hsub(trit_t a, trit_t b);

// Subtract two 5-trit values.
trit5_result_t trit5_sub(trit5_t a, trit5_t b);

// Subtract two 9-trit values.
trit9_result_t trit9_sub(trit9_t a, trit9_t b);

// Subtract two 27-trit values.
trit27_result_t trit27_sub(trit27_t a, trit27_t b);

//--- Multiplication ---
// Note: Single-trit trit_multiply() is in trit.h

// Multiply two 5-trit values.
// Returns 10-trit result packed into trit9 (with overflow indicator).
trit9_result_t trit5_mul(trit5_t a, trit5_t b);

//--- Division (centered remainder) ---

// Divide a by b, returning quotient.
// Uses centered remainder: |remainder| ≤ |divisor|/2
// See: word/core/ternary-math.toml [operations.division]
//
// Parameters:
//   a: Dividend
//   b: Divisor (must not be zero)
//
// Returns: Quotient (remainder discarded). Returns 0 if b is zero.
trit_t trit_div(trit_t a, trit_t b);

// Divide two 5-trit values.
trit5_t trit5_div(trit5_t a, trit5_t b);

// ═══════════════════════════════════════════════════════════════
// FAMILY 2: COMPARISON OPERATIONS (src/math.c)
// ═══════════════════════════════════════════════════════════════
//
// True ternary comparison returning trit_t, not boolean.

// Compare two trits.
//
// Returns:
//   - TRIT_NEG (-1): a < b
//   - TRIT_ZERO (0): a == b
//   - TRIT_POS (+1): a > b
trit_t trit_cmp(trit_t a, trit_t b);

// Compare two 5-trit values.
trit_t trit5_cmp(trit5_t a, trit5_t b);

// Compare two 9-trit values.
trit_t trit9_cmp(trit9_t a, trit9_t b);

// Compare two 27-trit values.
trit_t trit27_cmp(trit27_t a, trit27_t b);

// ═══════════════════════════════════════════════════════════════
// FAMILY 3: CONVERSION OPERATIONS (src/math.c)
// ═══════════════════════════════════════════════════════════════
//
// Bidirectional type conversion preserving meaning.

//--- Trit ↔ Integer ---

// Convert trit to integer (-1, 0, or +1).
int trit_to_int(trit_t t);

// Convert integer to trit (clamps to valid range).
trit_t int_to_trit(int i);

// Convert 5-trit to integer (-121 to +121).
int trit5_to_int(trit5_t t);

// Convert integer to 5-trit (clamps to valid range).
trit5_t int_to_trit5(int i);

//--- Health Score Conversion ---
// See: word/core/schemas/health.toml [normalization.formulas]

// Convert stored health (0-255) to true value (-100 to +100).
// Formula: (stored - 128) * 100 / 127
int8_t stored_to_true(uint8_t stored);

// Convert true health (-100 to +100) to stored value (0-255).
// Formula: (true_val * 127 / 100) + 128
uint8_t true_to_stored(int8_t true_val);

// ═══════════════════════════════════════════════════════════════
// FAMILY 4: NORMALIZATION OPERATIONS (src/math.c)
// ═══════════════════════════════════════════════════════════════
//
// Map raw scores to bases and classify to 7 health levels.
// See: word/core/schemas/health.toml [normalization], [classification]

// Round true value to nearest base increment.
// Formula: round(true_val / base) * base
//
// Parameters:
//   true_val: True health value (-100 to +100)
//   base: Normalization base (1, 5, 10, 20, 25, or 50)
//
// Returns: Rounded value (e.g., 66 with base 50 → 50)
int8_t trit_base_round(int8_t true_val, uint8_t base);

// Normalize to 5 hard anchor points (-100, -50, 0, +50, +100).
// Shorthand for trit_base_round(true_val, 50).
int8_t trit_normalize(int8_t true_val);

// Classify true value to 7-level health state.
//
// Parameters:
//   true_val: True health value (-100 to +100)
//
// Returns: Health level (LEVEL_BROKEN to LEVEL_PERFECT)
health_level_t trit_level(int8_t true_val);

// Get the Hebrew name for a health level.
//
// Returns: Static string (e.g., "שָׁלֵם" for LEVEL_SOUND)
const char* level_hebrew(health_level_t level);

// Get the English name for a health level.
//
// Returns: Static string (e.g., "sound" for LEVEL_SOUND)
const char* level_name(health_level_t level);

// Get the emoji for a health level.
//
// Returns: Static string (e.g., "💚" for LEVEL_SOUND)
const char* level_emoji(health_level_t level);

// ═══════════════════════════════════════════════════════════════
// FAMILY 5: CALCULUS OPERATIONS (src/math.c)
// ═══════════════════════════════════════════════════════════════
//
// Discrete calculus for health change detection.
// See: word/research/bereshit/bereshit-base-algorithms.adoc [calculus-operations]
//
// Biblical grounding:
//   - Ecclesiastes 3:1 - "To every thing there is a season" (time awareness)
//   - Genesis 1:14 - "for signs, and for seasons" (discrete intervals)
//   - Proverbs 22:28 - "Remove not the ancient landmark" (thresholds)

//--- Change Detection ---

// Calculate difference between two health values (Δ).
// Returns: current - previous (clamped to trit range for direction)
trit_t trit_difference(int8_t previous, int8_t current);

// Calculate rate of change (discrete derivative).
// Returns: Δvalue / Δtime approximation as trit (-1 declining, 0 stable, +1 improving)
//
// Parameters:
//   delta: Change in value (from trit_difference)
//   time_delta: Change in time (must be > 0)
trit_t trit_rate(int8_t delta, uint8_t time_delta);

// Accumulate values over time (discrete integral).
// Returns: Running sum clamped to int16_t range.
//
// Parameters:
//   accumulator: Current accumulated value
//   value: Value to add
int16_t trit_accumulate(int16_t accumulator, int8_t value);

//--- Threshold Detection ---

// Check if a threshold was crossed between two values.
//
// Parameters:
//   previous: Previous health value
//   current: Current health value
//   threshold: Boundary to check (e.g., 0, 50, -50)
//
// Returns: true if threshold was crossed in either direction
bool threshold_crossed(int8_t previous, int8_t current, int8_t threshold);

// Determine overall trend direction from history.
//
// Parameters:
//   history: Array of health values (oldest first)
//   count: Number of values in history
//
// Returns: TRIT_NEG (declining), TRIT_ZERO (stable), TRIT_POS (improving)
trit_t trend_direction(const int8_t* history, uint8_t count);

// Check if rate of change exceeds critical threshold.
//
// Parameters:
//   rate: Current rate (from trit_rate)
//   threshold: Critical rate threshold (positive value)
//
// Returns: true if |rate| >= threshold
bool critical_rate(trit_t rate, trit_t threshold);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables - all operations are pure functions]

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables needed - all constants are #define or enum]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: This header defines types only. Functions implemented in src/math.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   FAMILY 5: Calculus Operations (Top - depends on all below)
//   ├── trit_difference()    → change detection (needs comparison)
//   ├── trit_rate()          → rate of change (needs arithmetic)
//   ├── trit_accumulate()    → running sum (needs arithmetic)
//   ├── threshold_crossed()  → boundary detection (needs comparison)
//   ├── trend_direction()    → overall trend (needs comparison)
//   └── critical_rate()      → alert detection (needs comparison)
//
//   FAMILY 4: Normalization Operations
//   ├── trit_base_round()    → round to base increment
//   ├── trit_normalize()     → 5 hard points
//   ├── trit_level()         → 7-level classification
//   ├── level_hebrew()       → Hebrew name lookup
//   ├── level_name()         → English name lookup
//   └── level_emoji()        → emoji lookup
//
//   FAMILY 3: Conversion Operations
//   ├── trit_to_int()        → trit → integer
//   ├── int_to_trit()        → integer → trit
//   ├── stored_to_true()     → stored (0-255) → true (-100 to +100)
//   └── true_to_stored()     → true → stored
//
//   FAMILY 2: Comparison Operations
//   ├── trit_cmp()           → single trit comparison
//   ├── trit5_cmp()          → 5-trit comparison
//   ├── trit9_cmp()          → 9-trit comparison
//   └── trit27_cmp()         → 27-trit comparison
//
//   FAMILY 1: Arithmetic Operations (Foundation - depends on trit.h types)
//   ├── trit_negate() family → flip sign
//   ├── trit_add() family    → carry-aware addition
//   ├── trit_sub() family    → negate + add
//   ├── trit_mul() family    → multiplication
//   └── trit_div() family    → centered remainder division
//
// Baton Flow (Execution Paths):
//   Health Pipeline:
//     stored → stored_to_true() → trit_normalize() → trit_level() → level_emoji()
//   Change Detection:
//     values → trit_difference() → trit_rate() → critical_rate()
//   Arithmetic Chain:
//     trits → trit_add() → propagate carry → result with carry
//
// APUs (Available Processing Units):
//   - 38 functions total across 5 families
//   - 0 helpers (all public)
//   - 38 public APIs
//
// Type Definitions:
//   ├── health_level_t → 7-level health classification enum
//   ├── trit_result_t  → arithmetic result with carry (single trit)
//   ├── trit5_result_t → arithmetic result with carry (5 trits)
//   ├── trit9_result_t → arithmetic result with carry (9 trits)
//   └── trit27_result_t → arithmetic result with carry (27 trits)
//
// Implementation Location:
//   All function implementations in: src/math.c
//
// Declared Units:
// - 1 enum (health_level_t)
// - 4 structs (trit_result_t, trit5_result_t, trit9_result_t, trit27_result_t)
// - 38 function prototypes
// - 0 extern variables

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All functions are public APIs - no internal helpers in header]
// Internal helpers (carry propagation, table lookups) defined in src/math.c

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// See SETUP "Function Prototypes" section above for complete declarations.
// Functions organized by family (arithmetic → comparison → conversion →
// normalization → calculus), each building on the previous.

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return safe defaults rather than error codes.
//   - Invalid trit values → clamped to valid range
//   - Division by zero → returns 0 (safe default)
//   - Out of range conversions → clamped to valid range
//   - Invalid health level → LEVEL_EVEN (anchor/safe default)
//
// All functions are total - they return valid values for all inputs.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - Family 1: Arithmetic (negate, add, sub, mul, div)
//   - Family 2: Comparison (cmp for all types)
//   - Family 3: Conversion (trit↔int, stored↔true)
//   - Family 4: Normalization (base_round, normalize, level, lookups)
//   - Family 5: Calculus (difference, rate, accumulate, threshold, trend)

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 -I. math.h
//   echo '#include "math.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "math.h"
//   int main(void) { health_level_t l = LEVEL_SOUND; return l; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - all operations are pure functions]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Mathematical operations for balanced ternary types.
// Five families: Arithmetic (add/sub/mul/div), Comparison (ternary cmp),
// Conversion (trit↔int, stored↔true), Normalization (bases, levels),
// Calculus (difference, rate, accumulation, thresholds).

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new level lookup functions (follow level_name pattern)
//   ✅ Add new conversion helpers
//
// Modify with Care:
//   ⚠️ health_level_t values (must match health.toml classification)
//   ⚠️ Function signatures (breaks consumers)
//   ⚠️ Result struct layouts (breaks ABI)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Carry semantics (defined by ternary-math.toml)
//   ❌ Include guard (BERESHIT_MATH_H)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: LADDER component building on trit.h types.
// Each family builds on the previous (arithmetic → comparison →
// conversion → normalization → calculus).

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding arithmetic: SETUP "Function Prototypes" → FAMILY 1 section
// Adding comparisons: SETUP "Function Prototypes" → FAMILY 2 section
// Adding conversions: SETUP "Function Prototypes" → FAMILY 3 section
// Adding normalization: SETUP "Function Prototypes" → FAMILY 4 section
// Adding calculus: SETUP "Function Prototypes" → FAMILY 5 section
// Adding types: SETUP "Type Definitions" section
// Adding constants: SETUP "Defines" section

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Type Sizes:
//   - health_level_t: sizeof(int) (enum)
//   - trit_result_t: 2 bytes (2 × trit_t)
//   - trit5_result_t: 2 bytes (trit5_t + trit_t)
//   - trit9_result_t: 3 bytes (trit9_t + trit_t)
//   - trit27_result_t: 5 bytes (trit27_t + trit_t)
//
// Complexity:
//   - Single-trit ops: O(1) - table lookup
//   - Multi-trit ops: O(n) where n = number of trits
//   - Normalization: O(1) - arithmetic
//   - Calculus: O(1) to O(n) depending on function

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "unknown type name 'trit_t'"
//   Solution: math.h includes trit.h automatically
//
// Problem: "unknown type name 'health_level_t'"
//   Solution: Ensure math.h is included, not just trit.h
//
// Problem: Carry value unexpected
//   Check: Carry follows ternary-math.toml [operations.half_adder] table
//   Note: -1 + -1 = +1 with carry -1 (wraps, doesn't saturate)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Key dependencies:
//   - trit.h (trit_t, trit5_t, trit9_t, trit27_t)
//   - <stdbool.h>, <stdint.h>
//
// Implementation: src/math.c
//
// Specifications:
//   - word/core/ternary-math.toml (arithmetic tables)
//   - word/core/schemas/health.toml (normalization, classification)
//   - word/research/bereshit/bereshit-base-algorithms.adoc (calculus)

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ health_level_t enum - COMPLETED
//   ✓ trit_result_t structs - COMPLETED
//   ✓ Arithmetic signatures - COMPLETED (Part 1.2)
//   ✓ Comparison signatures - COMPLETED (Part 1.3)
//   ✓ Conversion signatures - COMPLETED (Part 1.4)
//   ✓ Normalization signatures - COMPLETED (Part 1.5)
//   ✓ Calculus signatures - COMPLETED (Part 1.6)
//   ⏳ Implementation in src/math.c (Part 2+)
//   ⏳ Test coverage (Part 7)
//
// Version History:
//   0.1.0 (2025-12-13) - Initial math framework (Phase 2, Part 1)
//         - Five operation families defined
//         - Carry-aware arithmetic approach (return structs)
//         - 7-level health classification from health.toml
//         - Discrete calculus from bereshit-base-algorithms.adoc

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header is a LADDER component - mathematical operations on top of
// trit types. It provides the computational engine for CPI-SI reasoning,
// from basic arithmetic to health-aware calculus.
//
// "A false balance is abomination to the LORD: but a just weight is his
//  delight." — Proverbs 11:1
//
// Just weights and accurate measures. Mathematical operations that honor
// the precision God built into creation.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "math.h"
//
// Arithmetic with Carry:
//   trit_result_t r = trit_add(TRIT_POS, TRIT_POS);
//   // r.value == TRIT_NEG, r.carry == TRIT_POS
//
// Health Conversion:
//   int8_t true_val = stored_to_true(212);  // true_val == +66
//   health_level_t level = trit_level(true_val);  // level == LEVEL_WHOLE
//   const char* emoji = level_emoji(level);  // emoji == "💙"
//
// Change Detection:
//   trit_t delta = trit_difference(50, 66);  // delta == TRIT_POS (improving)
//   bool crossed = threshold_crossed(50, 66, 50);  // true (crossed +50)
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 -I. your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_MATH_H


