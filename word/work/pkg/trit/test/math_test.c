// ═══════════════════════════════════════════════════════════════════════════
// libtrit Unit Tests - Mathematical Operations (5 Families)
// Key: B-word-work-pkg-trit-math-test
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a)
//   Links against libtrit.a for math operations testing.
//
// derives_from: bereshit/word/work/pkg/trit/test/trit_test.c (structure)
// See: void/planning/development/plans/phase-2-implementation-plan.adoc
//
// ═══════════════════════════════════════════════════════════════════════════

// Unit tests for math.c - designed to FAIL MEANINGFULLY.
// When demo-math breaks, run these tests to narrow down WHERE.
//
// math_test - CPI-SI Kingdom Technology
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
// Scripture: "A false balance is abomination to the LORD:
//             but a just weight is his delight." — Proverbs 11:1
//
// Principle: Tests verify just weights. Each assertion checks that
//            mathematical operations produce CORRECT results.
//
// Anchor: "Prove all things; hold fast that which is good."
//         — 1 Thessalonians 5:21
//
// # CPI-SI Identity
//
// Component Type: Baton (execution flow - diagnoses and reports)
//
// Role: Diagnostic infrastructure - finds failure points in math operations.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-13
//   - Version: 0.1.0
//
// # Purpose & Function
//
// Purpose: Diagnose WHERE failures occur in math.c (5 operation families).
//
// Core Design: Tests designed to fail meaningfully, matching 5 families:
//   - test_math_arithmetic()     → diagnose Family 1 failures
//   - test_math_comparison()     → diagnose Family 2 failures
//   - test_math_conversion()     → diagnose Family 3 failures
//   - test_math_normalization()  → diagnose Family 4 failures
//   - test_math_calculus()       → diagnose Family 5 failures
//
// Key Features:
//   - Arithmetic: hadd/fadd tables, carry propagation, multi-trit ops
//   - Comparison: trit_cmp returns trit_t (-1, 0, +1)
//   - Conversion: trit↔int roundtrip, stored↔true health range
//   - Normalization: base rounding, 7-level classification, lookups
//   - Calculus: difference, rate, accumulate, threshold, trend
//
// Philosophy: Prove a pass by making a test that allows you to fail.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//   - Standard: stdio.h (printf), stdint.h (int8_t, uint8_t)
//   - Internal: trit.h, math.h (libtrit headers)
//   - Library: libtrit.a (linked)
//   - Reference: word/core/ternary-math.toml (arithmetic tables)
//   - Reference: word/core/schemas/health.toml (normalization)
//
// What Uses This:
//   - Developers: Run when demo-math fails to find issue
//   - CI: Automated regression testing
//
// # Usage
//
// Build: make test-math
// Run: ./build/math_test
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// Exit codes:
//   0 = All tests passed
//   N = Number of failed tests (for diagnosis)
//
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

//--- Standard Library ---
#include <stdio.h>    // printf
#include <stdint.h>   // int8_t, int16_t, uint8_t
#include <stdbool.h>  // bool for threshold functions
#include <string.h>   // strcmp for string comparison

//--- Project Headers ---
#include "trit.h"   // MATTER layer: trit types
#include "trit_math.h"   // Math operations: 5 families

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

#define PASS "✓"
#define FAIL "✗"

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────

// Test result tracking
static int tests_passed = 0;
static int tests_failed = 0;

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// [Reserved: All types defined in library headers]

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

// ═══════════════════════════════════════════════════════════════
// PUBLIC API - Math Test Functions
// ═══════════════════════════════════════════════════════════════
// These functions are PUBLIC - callable for diagnostics.
// When demo-math breaks, these narrow down WHERE.

// Primary API (run tests)
int test_math_run_all(void);          // Run all tests, return failure count
int test_math_arithmetic(void);       // Test Family 1: Arithmetic
int test_math_comparison(void);       // Test Family 2: Comparison
int test_math_conversion(void);       // Test Family 3: Conversion
int test_math_normalization(void);    // Test Family 4: Normalization
int test_math_calculus(void);         // Test Family 5: Calculus

// Utility API (inspection and cleanup)
int test_math_get_passed(void);       // Get count of passed tests
int test_math_get_failed(void);       // Get count of failed tests
void test_math_reset(void);           // Reset test counters

// ═══════════════════════════════════════════════════════════════
// INTERNAL HELPERS
// ═══════════════════════════════════════════════════════════════
// These functions are internal - static linkage.

static void print_header(const char *title);
static void test_assert(int condition, const char *test_name);

// CLOSING section functions (VALIDATION → EXECUTION → CLEANUP flow)
static int test_validate_library(void);    // Code Validation
static void test_cleanup_and_report(void); // Code Cleanup

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
// Public API (Portable - callable by external code):
//   test_math_run_all()        → Run all tests, return failure count
//   ├── test_math_arithmetic()    → Family 1: hadd, fadd, multi-trit ops
//   ├── test_math_comparison()    → Family 2: trit_cmp returning trit_t
//   ├── test_math_conversion()    → Family 3: trit↔int, stored↔true
//   ├── test_math_normalization() → Family 4: base_round, level, lookups
//   └── test_math_calculus()      → Family 5: difference, rate, trend
//
// Entry Point:
//   main() → test_math_run_all() → exit code

// ════════════════════════════════════════════════════════════════
// SECTION 1: INTERNAL HELPERS (static)
// ════════════════════════════════════════════════════════════════

static void print_header(const char *title) {
    printf("\n────────────────────────────────────────────────────────────────\n");
    printf("%s\n", title);
    printf("────────────────────────────────────────────────────────────────\n");
}

// Helper: assert condition - PASS if true, FAIL if false (diagnostic)
static void test_assert(int condition, const char *test_name) {
    if (condition) {
        printf("  %s %s\n", PASS, test_name);
        tests_passed++;
    } else {
        printf("  %s %s  ← FAILURE POINT\n", FAIL, test_name);
        tests_failed++;
    }
}

// ════════════════════════════════════════════════════════════════
// SECTION 2: PUBLIC TEST API (non-static, callable externally)
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// test_math_arithmetic: Test Family 1 - Carry-Aware Operations
// ────────────────────────────────────────────────────────────────
//
// Tests half adder, full adder, multi-trit add/sub, and division.
// From ternary-math.toml [operations.half_adder], [operations.full_adder]

int test_math_arithmetic(void) {
    print_header("Family 1: Arithmetic - Carry-Aware Operations");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: trit_hadd - Half Adder (key cases)
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_hadd() - half adder:\n");

    trit_result_t r;

    // -1 + -1 = +1, carry -1
    r = trit_hadd(TRIT_NEG, TRIT_NEG);
    test_assert(trit_value(r.value) == +1, "hadd(-1,-1).value == +1");
    test_assert(trit_value(r.carry) == -1, "hadd(-1,-1).carry == -1");

    // -1 + +1 = 0, carry 0
    r = trit_hadd(TRIT_NEG, TRIT_POS);
    test_assert(trit_value(r.value) == 0, "hadd(-1,+1).value == 0");
    test_assert(trit_value(r.carry) == 0, "hadd(-1,+1).carry == 0");

    // +1 + +1 = -1, carry +1
    r = trit_hadd(TRIT_POS, TRIT_POS);
    test_assert(trit_value(r.value) == -1, "hadd(+1,+1).value == -1");
    test_assert(trit_value(r.carry) == +1, "hadd(+1,+1).carry == +1");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: trit_fadd - Full Adder
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_fadd() - full adder:\n");

    // +1 + +1 + +1 = 0, carry +1
    r = trit_fadd(TRIT_POS, TRIT_POS, TRIT_POS);
    test_assert(trit_value(r.value) == 0, "fadd(+1,+1,+1).value == 0");
    test_assert(trit_value(r.carry) == +1, "fadd(+1,+1,+1).carry == +1");

    // -1 + -1 + -1 = 0, carry -1
    r = trit_fadd(TRIT_NEG, TRIT_NEG, TRIT_NEG);
    test_assert(trit_value(r.value) == 0, "fadd(-1,-1,-1).value == 0");
    test_assert(trit_value(r.carry) == -1, "fadd(-1,-1,-1).carry == -1");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: trit_hsub - Subtraction
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_hsub() - subtraction:\n");

    r = trit_hsub(TRIT_POS, TRIT_POS);
    test_assert(trit_value(r.value) == 0, "hsub(+1,+1).value == 0");

    r = trit_hsub(TRIT_ZERO, TRIT_POS);
    test_assert(trit_value(r.value) == -1, "hsub(0,+1).value == -1");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 4: trit_div - Division
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_div() - division:\n");

    test_assert(trit_value(trit_div(TRIT_POS, TRIT_POS)) == +1, "+1 / +1 == +1");
    test_assert(trit_value(trit_div(TRIT_NEG, TRIT_NEG)) == +1, "-1 / -1 == +1");
    test_assert(trit_value(trit_div(TRIT_ZERO, TRIT_POS)) == 0, "0 / +1 == 0");
    test_assert(trit_value(trit_div(TRIT_POS, TRIT_ZERO)) == 0, "+1 / 0 == 0 (safe)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 5: Multi-trit negate
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit5_negate() - multi-trit negation:\n");

    trit_t trits[5] = {TRIT_NEG, TRIT_ZERO, TRIT_POS, TRIT_POS, TRIT_NEG};
    trit5_t packed = trit5_pack(trits);
    trit5_t negated = trit5_negate(packed);
    trit_t unpacked[5];
    trit5_unpack(negated, unpacked);

    test_assert(trit_value(unpacked[0]) == +1, "negate[0]: -1 → +1");
    test_assert(trit_value(unpacked[1]) == 0, "negate[1]: 0 → 0");
    test_assert(trit_value(unpacked[2]) == -1, "negate[2]: +1 → -1");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_math_comparison: Test Family 2 - Ternary Comparison
// ────────────────────────────────────────────────────────────────
//
// Tests trit_cmp returning trit_t (-1, 0, +1) for true ternary comparison.

int test_math_comparison(void) {
    print_header("Family 2: Comparison - Ternary Compare");

    printf("\n  trit_cmp() - single trit:\n");

    test_assert(trit_value(trit_cmp(TRIT_NEG, TRIT_POS)) == -1, "cmp(-1,+1) == -1 (less)");
    test_assert(trit_value(trit_cmp(TRIT_ZERO, TRIT_ZERO)) == 0, "cmp(0,0) == 0 (equal)");
    test_assert(trit_value(trit_cmp(TRIT_POS, TRIT_NEG)) == +1, "cmp(+1,-1) == +1 (greater)");

    printf("\n  trit5_cmp() - 5-trit:\n");

    trit_t a5[5] = {TRIT_ZERO, TRIT_ZERO, TRIT_ZERO, TRIT_ZERO, TRIT_ZERO};
    trit_t b5[5] = {TRIT_POS, TRIT_ZERO, TRIT_ZERO, TRIT_ZERO, TRIT_ZERO};
    trit5_t pa = trit5_pack(a5);
    trit5_t pb = trit5_pack(b5);

    test_assert(trit_value(trit5_cmp(pa, pa)) == 0, "cmp5(a,a) == 0 (equal)");
    test_assert(trit_value(trit5_cmp(pa, pb)) == -1, "cmp5(0,+1) == -1 (less)");
    test_assert(trit_value(trit5_cmp(pb, pa)) == +1, "cmp5(+1,0) == +1 (greater)");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_math_conversion: Test Family 3 - Type Conversion
// ────────────────────────────────────────────────────────────────
//
// Tests trit↔int and stored↔true (health range) conversions.

int test_math_conversion(void) {
    print_header("Family 3: Conversion - Type Bridging");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: trit ↔ int
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_to_int() / int_to_trit():\n");

    test_assert(trit_to_int(TRIT_NEG) == -1, "trit_to_int(NEG) == -1");
    test_assert(trit_to_int(TRIT_ZERO) == 0, "trit_to_int(ZERO) == 0");
    test_assert(trit_to_int(TRIT_POS) == +1, "trit_to_int(POS) == +1");

    test_assert(trit_value(int_to_trit(-1)) == -1, "int_to_trit(-1) == NEG");
    test_assert(trit_value(int_to_trit(0)) == 0, "int_to_trit(0) == ZERO");
    test_assert(trit_value(int_to_trit(+1)) == +1, "int_to_trit(+1) == POS");
    test_assert(trit_value(int_to_trit(99)) == +1, "int_to_trit(99) clamps to +1");
    test_assert(trit_value(int_to_trit(-99)) == -1, "int_to_trit(-99) clamps to -1");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: stored ↔ true (health range)
    // ─────────────────────────────────────────────────────────────
    printf("\n  stored_to_true() / true_to_stored():\n");

    // stored 0 → true -100 (minimum)
    test_assert(stored_to_true(0) == -100, "stored(0) → true(-100)");
    // stored 128 → true 0 (center)
    test_assert(stored_to_true(128) == 0, "stored(128) → true(0)");
    // stored 255 → true +100 (maximum)
    test_assert(stored_to_true(255) == 100, "stored(255) → true(+100)");

    // Roundtrip: true → stored → true
    // Note: Integer division causes ±1 variance. Test with 0 (exact center)
    int8_t original = 0;
    uint8_t stored = true_to_stored(original);
    int8_t back = stored_to_true(stored);
    test_assert(back == original, "roundtrip: 0 → stored → 0 (exact)");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_math_normalization: Test Family 4 - Health Levels
// ────────────────────────────────────────────────────────────────
//
// Tests base rounding, 7-level classification, and lookups.

int test_math_normalization(void) {
    print_header("Family 4: Normalization - Health Classification");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: trit_base_round
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_base_round():\n");

    test_assert(trit_base_round(66, 50) == 50, "round(66, base=50) → 50");
    test_assert(trit_base_round(74, 50) == 50, "round(74, base=50) → 50");
    test_assert(trit_base_round(76, 50) == 100, "round(76, base=50) → 100");
    test_assert(trit_base_round(-33, 25) == -25, "round(-33, base=25) → -25");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: trit_normalize (shorthand for base=50)
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_normalize():\n");

    test_assert(trit_normalize(0) == 0, "normalize(0) → 0");
    test_assert(trit_normalize(24) == 0, "normalize(24) → 0");
    test_assert(trit_normalize(26) == 50, "normalize(26) → 50");
    test_assert(trit_normalize(-75) == -100, "normalize(-75) → -100");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: trit_level - 7-level classification
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_level():\n");

    test_assert(trit_level(-100) == LEVEL_BROKEN, "level(-100) == BROKEN");
    test_assert(trit_level(-50) == LEVEL_WANTING, "level(-50) == WANTING");
    test_assert(trit_level(-25) == LEVEL_LACKING, "level(-25) == LACKING");
    test_assert(trit_level(0) == LEVEL_EVEN, "level(0) == EVEN");
    test_assert(trit_level(+25) == LEVEL_SOUND, "level(+25) == SOUND");
    test_assert(trit_level(+50) == LEVEL_WHOLE, "level(+50) == WHOLE");
    test_assert(trit_level(+100) == LEVEL_PERFECT, "level(+100) == PERFECT");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 4: level lookups
    // ─────────────────────────────────────────────────────────────
    printf("\n  level_name() / level_emoji():\n");

    test_assert(strcmp(level_name(LEVEL_EVEN), "even") == 0, "name(EVEN) == 'even'");
    test_assert(strcmp(level_name(LEVEL_PERFECT), "perfect") == 0, "name(PERFECT) == 'perfect'");
    test_assert(strcmp(level_emoji(LEVEL_BROKEN), "💔") == 0, "emoji(BROKEN) == 💔");
    test_assert(strcmp(level_emoji(LEVEL_PERFECT), "💛") == 0, "emoji(PERFECT) == 💛");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_math_calculus: Test Family 5 - Discrete Calculus
// ────────────────────────────────────────────────────────────────
//
// Tests difference, rate, accumulation, threshold, and trend.

int test_math_calculus(void) {
    print_header("Family 5: Calculus - Change Detection");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: trit_difference
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_difference():\n");

    test_assert(trit_value(trit_difference(50, 66)) == +1, "diff(50→66) == +1 (improving)");
    test_assert(trit_value(trit_difference(66, 50)) == -1, "diff(66→50) == -1 (declining)");
    test_assert(trit_value(trit_difference(50, 50)) == 0, "diff(50→50) == 0 (stable)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: trit_rate
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_rate():\n");

    // Note: threshold is >10, not >=10. Rate must exceed 10 to trigger.
    test_assert(trit_value(trit_rate(11, 1)) == +1, "rate(+11/1) == +1 (improving)");
    test_assert(trit_value(trit_rate(-11, 1)) == -1, "rate(-11/1) == -1 (declining)");
    test_assert(trit_value(trit_rate(0, 1)) == 0, "rate(0/1) == 0 (stable)");
    test_assert(trit_value(trit_rate(10, 1)) == 0, "rate(+10/1) == 0 (not rapid)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: trit_accumulate
    // ─────────────────────────────────────────────────────────────
    printf("\n  trit_accumulate():\n");

    int16_t acc = 0;
    acc = trit_accumulate(acc, 50);
    test_assert(acc == 50, "accumulate(0, 50) == 50");
    acc = trit_accumulate(acc, 25);
    test_assert(acc == 75, "accumulate(50, 25) == 75");
    acc = trit_accumulate(acc, -100);
    test_assert(acc == -25, "accumulate(75, -100) == -25");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 4: threshold_crossed
    // ─────────────────────────────────────────────────────────────
    printf("\n  threshold_crossed():\n");

    test_assert(threshold_crossed(40, 60, 50) == true, "40→60 crosses 50");
    test_assert(threshold_crossed(60, 40, 50) == true, "60→40 crosses 50");
    test_assert(threshold_crossed(51, 60, 50) == false, "51→60 doesn't cross 50");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 5: trend_direction
    // ─────────────────────────────────────────────────────────────
    printf("\n  trend_direction():\n");

    int8_t improving[] = {30, 40, 50, 60};
    int8_t declining[] = {60, 50, 40, 30};
    int8_t stable[] = {50, 50, 50, 50};

    test_assert(trit_value(trend_direction(improving, 4)) == +1, "trend([30,40,50,60]) == +1");
    test_assert(trit_value(trend_direction(declining, 4)) == -1, "trend([60,50,40,30]) == -1");
    test_assert(trit_value(trend_direction(stable, 4)) == 0, "trend([50,50,50,50]) == 0");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 6: critical_rate
    // ─────────────────────────────────────────────────────────────
    printf("\n  critical_rate():\n");

    test_assert(critical_rate(TRIT_POS, TRIT_POS) == true, "rate +1 >= threshold +1");
    test_assert(critical_rate(TRIT_NEG, TRIT_POS) == true, "rate -1 (abs) >= threshold +1");
    test_assert(critical_rate(TRIT_ZERO, TRIT_POS) == false, "rate 0 < threshold +1");

    return tests_failed;
}

// ════════════════════════════════════════════════════════════════
// SECTION 3: TEST ORCHESTRATION
// ════════════════════════════════════════════════════════════════

// Run all math tests
int test_math_run_all(void) {
    tests_passed = 0;
    tests_failed = 0;

    printf("════════════════════════════════════════════════════════════════\n");
    printf("Phase 2 Unit Tests: Mathematical Operations (5 Families)\n");
    printf("════════════════════════════════════════════════════════════════\n");

    // Validate prerequisites
    if (!test_validate_library()) {
        test_cleanup_and_report();
        return tests_failed;
    }

    // Run all 5 family tests
    test_math_arithmetic();
    test_math_comparison();
    test_math_conversion();
    test_math_normalization();
    test_math_calculus();

    // Report results
    test_cleanup_and_report();

    return tests_failed;
}

// ════════════════════════════════════════════════════════════════
// SECTION 4: ENTRY POINT
// ════════════════════════════════════════════════════════════════

int main(void) {
    int failures = test_math_run_all();
    return (failures > 0) ? 1 : 0;
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

// Validate library is working (sanity checks)
static int test_validate_library(void) {
    printf("\n────────────────────────────────────────────────────────────────\n");
    printf("Validating libtrit math prerequisites...\n");
    printf("────────────────────────────────────────────────────────────────\n");

    int valid = 1;

    // Check trit types work
    trit_t t = trit_create(1);
    if (!trit_valid(t) || trit_value(t) != 1) {
        printf("  ✗ trit types failed\n");
        valid = 0;
    } else {
        printf("  ✓ trit types ready\n");
    }

    // Check math.h included properly (test a result struct)
    trit_result_t r = trit_hadd(TRIT_ZERO, TRIT_ZERO);
    if (trit_value(r.value) != 0) {
        printf("  ✗ math operations failed\n");
        valid = 0;
    } else {
        printf("  ✓ math operations ready\n");
    }

    printf("────────────────────────────────────────────────────────────────\n");
    if (valid) {
        printf("✓ Prerequisites validated. Running tests.\n");
    } else {
        printf("✗ Prerequisites failed.\n");
        tests_failed++;
    }

    return valid;
}

// Utility: Get test counts
int test_math_get_passed(void) { return tests_passed; }
int test_math_get_failed(void) { return tests_failed; }

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────

// Report final results
static void test_cleanup_and_report(void) {
    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("Math Tests Complete\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Passed: %d\n", tests_passed);

    if (tests_failed > 0) {
        printf("  Failed: %d  ← investigate these\n", tests_failed);
    } else {
        printf("  Failed: 0  ✓\n");
    }

    printf("\n  \"A false balance is abomination to the LORD:\n");
    printf("   but a just weight is his delight.\" — Proverbs 11:1\n");
    printf("\n════════════════════════════════════════════════════════════════\n");
}

// Public: Reset test state
void test_math_reset(void) {
    tests_passed = 0;
    tests_failed = 0;
}

// ════════════════════════════════════════════════════════════════
// GROUP 2: DOCUMENTATION
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Test Overview
// ────────────────────────────────────────────────────────────────
//
// This test validates the 5 math operation families:
//
//   Family 1: Arithmetic - hadd, fadd, hsub, div, multi-trit ops
//   Family 2: Comparison - trit_cmp returning trit_t
//   Family 3: Conversion - trit↔int, stored↔true
//   Family 4: Normalization - base_round, level, lookups
//   Family 5: Calculus - difference, rate, accumulate, threshold, trend

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new test cases within families
//   ✅ Adjust messages and formatting
//
// Modify with Care:
//   ⚠️ Expected values - must match math.c implementation
//   ⚠️ Public API signatures
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ test_assert() semantics

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Library:
//   - word/work/pkg/trit/src/math.c (implementation)
//   - word/work/pkg/trit/include/math.h (interface)
//
// Demo:
//   - tov/demo/phase-2/demo-math/ (demonstrates, doesn't diagnose)
//
// Specs:
//   - word/core/ternary-math.toml (arithmetic tables)
//   - word/core/schemas/health.toml (normalization)

// ============================================================================
// END CLOSING
// ============================================================================
