// ═══════════════════════════════════════════════════════════════════════════
// libtrit Unit Tests - Bible Rail Encoding (13 WEB Variants)
// Key: B-word-work-pkg-trit-test-bible-rail
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a)
//   Links against libtrit.a for bible_rail functions.
//
// derives_from: bereshit/word/work/pkg/trit/test/math_test.c (structure)
// See: word/scripture/web-variant-index.csv for 13 variant mappings
//
// ═══════════════════════════════════════════════════════════════════════════

// Unit tests for bible_rail.c - designed to FAIL MEANINGFULLY.
// When Bible Rail encoding breaks, run these tests to narrow down WHERE.
//
// bible_rail_test - CPI-SI Kingdom Technology
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
// Scripture: "Add thou not unto his words, lest he reprove thee,
//             and thou be found a liar." — Proverbs 30:6
//
// Principle: The 13 spare values in trit5 packing (243-255) encode
//            the 13 textual variants between KJV and WEB. Every bit
//            serves the Word — nothing added, nothing removed.
//
// Anchor: "Prove all things; hold fast that which is good."
//         — 1 Thessalonians 5:21
//
// # CPI-SI Identity
//
// Component Type: Baton (execution flow - diagnoses and reports)
//
// Role: Diagnostic infrastructure - finds failure points in Bible Rail encoding.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-14
//   - Version: 0.1.0
//
// # Purpose & Function
//
// Purpose: Diagnose WHERE failures occur in bible_rail.c (encode/decode).
//
// Core Design: Tests designed to fail meaningfully, covering:
//   - test_bible_rail_roundtrip()    → encode/decode roundtrip for all 13
//   - test_bible_rail_encode()       → encode boundaries and invalid inputs
//   - test_bible_rail_decode()       → decode boundaries and invalid inputs
//   - test_bible_rail_spare()        → spare value detection (242 vs 243)
//   - test_bible_rail_queries()      → variant_ref, variant_desc, variant_count
//   - test_bible_rail_all_spares()   → verify exactly 13 spare values
//
// Key Features:
//   - Roundtrip: variant 0-12 → spare 243-255 → variant 0-12
//   - Boundaries: valid (0, 12) vs invalid (-1, 13)
//   - Spare detection: semantic (0-242) vs spare (243-255)
//   - Query functions: reference, description, count
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
//   - Standard: stdio.h (printf), stdbool.h (bool), string.h (strcmp)
//   - Internal: bible_rail.h (includes trit.h)
//   - Library: libtrit.a (linked)
//   - Reference: word/scripture/web-variant-index.csv (variant data)
//
// What Uses This:
//   - Developers: Run when Bible Rail encoding fails to find issue
//   - CI: Automated regression testing
//
// # Usage
//
// Build: make test-bible-rail
// Run: ./build/bible_rail_test
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// Exit codes:
//   0 = All tests passed
//   1 = One or more tests failed (investigate)
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
#include <stdbool.h>  // bool
#include <string.h>   // strcmp

//--- Project Headers ---
#include "bible_rail.h"  // Bible Rail encoding (includes trit.h)

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
// PUBLIC API - Bible Rail Test Functions
// ═══════════════════════════════════════════════════════════════
// These functions are PUBLIC - callable for diagnostics.
// When Bible Rail breaks, these narrow down WHERE.

// Primary API (run tests)
int test_bible_rail_run_all(void);       // Run all tests, return failure count
int test_bible_rail_roundtrip(void);     // Test encode/decode roundtrip
int test_bible_rail_encode(void);        // Test encode boundaries
int test_bible_rail_decode(void);        // Test decode boundaries
int test_bible_rail_spare(void);         // Test spare detection
int test_bible_rail_queries(void);       // Test query functions
int test_bible_rail_all_spares(void);    // Test all 13 spare values
int test_bible_rail_location(void);      // Test location lookup functions

// Utility API (inspection and cleanup)
int test_bible_rail_get_passed(void);    // Get count of passed tests
int test_bible_rail_get_failed(void);    // Get count of failed tests
void test_bible_rail_reset(void);        // Reset test counters

// ═══════════════════════════════════════════════════════════════
// INTERNAL HELPERS
// ═══════════════════════════════════════════════════════════════
// These functions are internal - static linkage.

static void print_header(const char *title);
static void test_assert(int condition, const char *test_name);

// CLOSING section functions (VALIDATION → EXECUTION → CLEANUP flow)
static int test_validate_library(void);     // Code Validation
static void test_cleanup_and_report(void);  // Code Cleanup

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
//   test_bible_rail_run_all()       → Run all tests, return failure count
//   ├── test_bible_rail_roundtrip()    → Encode→decode for all 13 variants
//   ├── test_bible_rail_encode()       → Encode boundaries and invalids
//   ├── test_bible_rail_decode()       → Decode boundaries and invalids
//   ├── test_bible_rail_spare()        → Spare vs semantic detection
//   ├── test_bible_rail_queries()      → ref, desc, count functions
//   ├── test_bible_rail_all_spares()   → Verify exactly 13 spares
//   └── test_bible_rail_location()     → Location lookup (variant_at, variant_location)
//
// Entry Point:
//   main() → test_bible_rail_run_all() → exit code

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
// test_bible_rail_roundtrip: Encode/Decode Roundtrip
// ────────────────────────────────────────────────────────────────
//
// Tests that each variant index (0-12) encodes to a spare value
// and decodes back to the original index. Verifies bijection.

int test_bible_rail_roundtrip(void) {
    print_header("Roundtrip: Encode → Decode for All 13 Variants");

    printf("\n  Testing variant index → spare → variant index:\n");

    // Test all 13 variants
    for (int i = 0; i < BIBLE_RAIL_VARIANT_COUNT; i++) {
        trit5_t spare = bible_rail_encode(i);
        int decoded = bible_rail_decode(spare);

        char name[80];
        snprintf(name, sizeof(name),
                 "variant %2d: encode(%d)=%d, decode(%d)=%d",
                 i, i, spare, spare, decoded);
        test_assert(decoded == i, name);
    }

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_bible_rail_encode: Encode Boundaries
// ────────────────────────────────────────────────────────────────
//
// Tests encode function with valid and invalid inputs.
// Valid: 0-12 → 243-255. Invalid: -1, 13, 100 → 0.

int test_bible_rail_encode(void) {
    print_header("Encode: Boundaries and Invalid Inputs");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Valid boundaries
    // ─────────────────────────────────────────────────────────────
    printf("\n  Valid encode boundaries:\n");

    test_assert(bible_rail_encode(0) == 243, "encode(0) == 243 (first variant)");
    test_assert(bible_rail_encode(12) == 255, "encode(12) == 255 (last variant)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Invalid inputs (return 0)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Invalid encode inputs (return 0):\n");

    test_assert(bible_rail_encode(-1) == 0, "encode(-1) == 0 (invalid: below range)");
    test_assert(bible_rail_encode(13) == 0, "encode(13) == 0 (invalid: above range)");
    test_assert(bible_rail_encode(100) == 0, "encode(100) == 0 (invalid: far above)");
    test_assert(bible_rail_encode(-100) == 0, "encode(-100) == 0 (invalid: far below)");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_bible_rail_decode: Decode Boundaries
// ────────────────────────────────────────────────────────────────
//
// Tests decode function with valid and invalid inputs.
// Valid: 243-255 → 0-12. Invalid: 0-242 → -1 (not a spare).

int test_bible_rail_decode(void) {
    print_header("Decode: Boundaries and Invalid Inputs");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Valid spare values
    // ─────────────────────────────────────────────────────────────
    printf("\n  Valid decode (spare values):\n");

    test_assert(bible_rail_decode(243) == 0, "decode(243) == 0 (first spare)");
    test_assert(bible_rail_decode(249) == 6, "decode(249) == 6 (middle spare)");
    test_assert(bible_rail_decode(255) == 12, "decode(255) == 12 (last spare)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Invalid inputs (return -1)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Invalid decode inputs (return -1):\n");

    test_assert(bible_rail_decode(242) == -1, "decode(242) == -1 (last semantic)");
    test_assert(bible_rail_decode(0) == -1, "decode(0) == -1 (not spare)");
    test_assert(bible_rail_decode(100) == -1, "decode(100) == -1 (not spare)");
    test_assert(bible_rail_decode(121) == -1, "decode(121) == -1 (center semantic)");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_bible_rail_spare: Spare Value Detection
// ────────────────────────────────────────────────────────────────
//
// Tests is_spare function to distinguish semantic (0-242) from spare (243-255).

int test_bible_rail_spare(void) {
    print_header("Spare Detection: Semantic vs Spare Values");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Semantic values (NOT spare)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Semantic values (is_spare == false):\n");

    test_assert(!bible_rail_is_spare(0), "is_spare(0) == false (minimum semantic)");
    test_assert(!bible_rail_is_spare(121), "is_spare(121) == false (center semantic)");
    test_assert(!bible_rail_is_spare(242), "is_spare(242) == false (last semantic)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Spare values (IS spare)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Spare values (is_spare == true):\n");

    test_assert(bible_rail_is_spare(243), "is_spare(243) == true (first spare)");
    test_assert(bible_rail_is_spare(249), "is_spare(249) == true (middle spare)");
    test_assert(bible_rail_is_spare(255), "is_spare(255) == true (last spare)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: Boundary (242 vs 243)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Critical boundary (242/243):\n");

    test_assert(!bible_rail_is_spare(242) && bible_rail_is_spare(243),
                "242 semantic, 243 spare (boundary correct)");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_bible_rail_queries: Query Functions
// ────────────────────────────────────────────────────────────────
//
// Tests variant_ref, variant_desc, and variant_count functions.

int test_bible_rail_queries(void) {
    print_header("Query Functions: Reference, Description, Count");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: variant_count
    // ─────────────────────────────────────────────────────────────
    printf("\n  variant_count():\n");

    test_assert(bible_rail_variant_count() == 13, "variant_count() == 13");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: variant_ref (valid indices)
    // ─────────────────────────────────────────────────────────────
    printf("\n  variant_ref() - valid indices:\n");

    const char* ref0 = bible_rail_variant_ref(0);
    test_assert(ref0 != NULL, "variant_ref(0) != NULL");
    test_assert(ref0 && strcmp(ref0, "1 Corinthians 16:27") == 0,
                "variant_ref(0) == '1 Corinthians 16:27'");

    const char* ref12 = bible_rail_variant_ref(12);
    test_assert(ref12 != NULL, "variant_ref(12) != NULL");
    test_assert(ref12 && strcmp(ref12, "Zechariah 2:23") == 0,
                "variant_ref(12) == 'Zechariah 2:23'");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: variant_ref (invalid indices)
    // ─────────────────────────────────────────────────────────────
    printf("\n  variant_ref() - invalid indices:\n");

    test_assert(bible_rail_variant_ref(-1) == NULL, "variant_ref(-1) == NULL");
    test_assert(bible_rail_variant_ref(13) == NULL, "variant_ref(13) == NULL");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 4: variant_desc (valid indices)
    // ─────────────────────────────────────────────────────────────
    printf("\n  variant_desc() - valid indices:\n");

    const char* desc0 = bible_rail_variant_desc(0);
    test_assert(desc0 != NULL, "variant_desc(0) != NULL");
    test_assert(desc0 && strcmp(desc0, "TR manuscript note") == 0,
                "variant_desc(0) == 'TR manuscript note'");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 5: variant_desc (invalid indices)
    // ─────────────────────────────────────────────────────────────
    printf("\n  variant_desc() - invalid indices:\n");

    test_assert(bible_rail_variant_desc(-1) == NULL, "variant_desc(-1) == NULL");
    test_assert(bible_rail_variant_desc(13) == NULL, "variant_desc(13) == NULL");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_bible_rail_all_spares: Verify Exactly 13 Spare Values
// ────────────────────────────────────────────────────────────────
//
// Scans all 256 possible byte values to verify:
// 1. Exactly 13 values are spare (243-255)
// 2. Spare values are contiguous

int test_bible_rail_all_spares(void) {
    print_header("All 13 Spare Values: Count and Contiguity");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Count spare values
    // ─────────────────────────────────────────────────────────────
    printf("\n  Counting spare values in 0-255:\n");

    int spare_count = 0;
    for (int v = 0; v <= 255; v++) {
        if (bible_rail_is_spare((trit5_t)v)) {
            spare_count++;
        }
    }
    test_assert(spare_count == 13, "exactly 13 spare values in 0-255 range");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Verify contiguous range
    // ─────────────────────────────────────────────────────────────
    printf("\n  Verifying spare values 243-255 are contiguous:\n");

    bool contiguous = true;
    for (int v = BIBLE_RAIL_SPARE_MIN; v <= BIBLE_RAIL_SPARE_MAX; v++) {
        if (!bible_rail_is_spare((trit5_t)v)) {
            contiguous = false;
            break;
        }
    }
    test_assert(contiguous, "spare values 243-255 are contiguous");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: Verify NO spares below 243
    // ─────────────────────────────────────────────────────────────
    printf("\n  Verifying no spare values below 243:\n");

    bool no_early_spares = true;
    for (int v = 0; v < BIBLE_RAIL_SPARE_MIN; v++) {
        if (bible_rail_is_spare((trit5_t)v)) {
            no_early_spares = false;
            break;
        }
    }
    test_assert(no_early_spares, "no spare values in 0-242 (all semantic)");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_bible_rail_location: Location Lookup Functions
// ────────────────────────────────────────────────────────────────
//
// Tests bible_rail_variant_at() and bible_rail_variant_location()
// These functions enable variant detection by book/chapter/verse.
//
int test_bible_rail_location(void) {
    print_header("Location Lookup: variant_at and variant_location");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: variant_at for known variant locations
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing variant_at for all 13 variant locations:\n");

    // Test each of the 13 variant locations
    test_assert(bible_rail_variant_at("1 Corinthians", 16, 27) == 243,
                "1 Corinthians 16:27 → 243");
    test_assert(bible_rail_variant_at("1 Peter", 5, 20) == 244,
                "1 Peter 5:20 → 244");
    test_assert(bible_rail_variant_at("1 Timothy", 3, 18) == 245,
                "1 Timothy 3:18 → 245");
    test_assert(bible_rail_variant_at("2 Kings", 22, 53) == 246,
                "2 Kings 22:53 → 246");
    test_assert(bible_rail_variant_at("Amos", 3, 21) == 247,
                "Amos 3:21 → 247");
    test_assert(bible_rail_variant_at("Colossians", 4, 23) == 248,
                "Colossians 4:23 → 248");
    test_assert(bible_rail_variant_at("Ezekiel", 5, 22) == 249,
                "Ezekiel 5:22 → 249");
    test_assert(bible_rail_variant_at("Jonah", 1, 21) == 250,
                "Jonah 1:21 → 250");
    test_assert(bible_rail_variant_at("Nehemiah", 10, 44) == 251,
                "Nehemiah 10:44 → 251");
    test_assert(bible_rail_variant_at("Numbers", 27, 34) == 252,
                "Numbers 27:34 → 252");
    test_assert(bible_rail_variant_at("Psalms", 42, 17) == 253,
                "Psalms 42:17 → 253");
    test_assert(bible_rail_variant_at("Revelation", 1, 25) == 254,
                "Revelation 1:25 → 254");
    test_assert(bible_rail_variant_at("Zechariah", 2, 23) == 255,
                "Zechariah 2:23 → 255");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: variant_at for non-variant locations
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing variant_at for non-variant locations:\n");

    test_assert(bible_rail_variant_at("Genesis", 1, 1) == 0,
                "Genesis 1:1 → 0 (not variant)");
    test_assert(bible_rail_variant_at("John", 3, 16) == 0,
                "John 3:16 → 0 (not variant)");
    test_assert(bible_rail_variant_at("Revelation", 22, 21) == 0,
                "Revelation 22:21 → 0 (not variant)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: variant_at edge cases
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing variant_at edge cases:\n");

    test_assert(bible_rail_variant_at(NULL, 1, 1) == 0,
                "NULL book → 0 (invalid)");
    test_assert(bible_rail_variant_at("1 Corinthians", 16, 26) == 0,
                "wrong verse in variant book → 0");
    test_assert(bible_rail_variant_at("1 Corinthians", 15, 27) == 0,
                "wrong chapter in variant book → 0");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 4: variant_location for valid indices
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing variant_location for valid indices:\n");

    variant_location_t loc = bible_rail_variant_location(0);
    test_assert(loc.book != NULL && loc.chapter == 16 && loc.verse == 27,
                "variant_location(0) → 1 Corinthians 16:27");

    loc = bible_rail_variant_location(12);
    test_assert(loc.book != NULL && loc.chapter == 2 && loc.verse == 23,
                "variant_location(12) → Zechariah 2:23");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 5: variant_location for invalid indices
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing variant_location for invalid indices:\n");

    loc = bible_rail_variant_location(-1);
    test_assert(loc.book == NULL && loc.chapter == 0 && loc.verse == 0,
                "variant_location(-1) → {NULL, 0, 0}");

    loc = bible_rail_variant_location(13);
    test_assert(loc.book == NULL && loc.chapter == 0 && loc.verse == 0,
                "variant_location(13) → {NULL, 0, 0}");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 6: Roundtrip - variant_at ↔ variant_location
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing roundtrip: variant_location → variant_at:\n");

    int roundtrip_ok = 1;
    for (int i = 0; i < BIBLE_RAIL_VARIANT_COUNT; i++) {
        loc = bible_rail_variant_location(i);
        trit5_t spare = bible_rail_variant_at(loc.book, loc.chapter, loc.verse);
        if (spare != (trit5_t)(BIBLE_RAIL_SPARE_MIN + i)) {
            roundtrip_ok = 0;
            break;
        }
    }
    test_assert(roundtrip_ok, "all 13 locations roundtrip correctly");

    return tests_failed;
}

// ════════════════════════════════════════════════════════════════
// SECTION 3: TEST ORCHESTRATION
// ════════════════════════════════════════════════════════════════

// Run all Bible Rail tests
int test_bible_rail_run_all(void) {
    tests_passed = 0;
    tests_failed = 0;

    printf("════════════════════════════════════════════════════════════════\n");
    printf("Phase 2 Unit Tests: Bible Rail Encoding (13 WEB Variants)\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Testing spare trit5 values (243-255) → WEB textual variants\n");

    // Validate prerequisites
    if (!test_validate_library()) {
        test_cleanup_and_report();
        return tests_failed;
    }

    // Run all test functions
    test_bible_rail_roundtrip();
    test_bible_rail_encode();
    test_bible_rail_decode();
    test_bible_rail_spare();
    test_bible_rail_queries();
    test_bible_rail_all_spares();
    test_bible_rail_location();

    // Report results
    test_cleanup_and_report();

    return tests_failed;
}

// ════════════════════════════════════════════════════════════════
// SECTION 4: ENTRY POINT
// ════════════════════════════════════════════════════════════════

int main(void) {
    int failures = test_bible_rail_run_all();
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
    printf("Validating libtrit Bible Rail prerequisites...\n");
    printf("────────────────────────────────────────────────────────────────\n");

    int valid = 1;

    // Check bible_rail.h included properly
    int count = bible_rail_variant_count();
    if (count != 13) {
        printf("  %s variant_count() != 13 (got %d)\n", FAIL, count);
        valid = 0;
    } else {
        printf("  %s Bible Rail functions accessible\n", PASS);
    }

    // Check constants defined
    if (BIBLE_RAIL_SPARE_MIN != 243 || BIBLE_RAIL_SPARE_MAX != 255) {
        printf("  %s spare constants incorrect\n", FAIL);
        valid = 0;
    } else {
        printf("  %s spare constants correct (243-255)\n", PASS);
    }

    // Check basic encode/decode
    trit5_t spare = bible_rail_encode(0);
    int decoded = bible_rail_decode(spare);
    if (decoded != 0) {
        printf("  %s basic encode/decode failed\n", FAIL);
        valid = 0;
    } else {
        printf("  %s basic encode/decode works\n", PASS);
    }

    printf("────────────────────────────────────────────────────────────────\n");
    if (valid) {
        printf("%s Prerequisites validated. Running tests.\n", PASS);
    } else {
        printf("%s Prerequisites failed.\n", FAIL);
        tests_failed++;
    }

    return valid;
}

// Utility: Get test counts
int test_bible_rail_get_passed(void) { return tests_passed; }
int test_bible_rail_get_failed(void) { return tests_failed; }

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────

// Report final results
static void test_cleanup_and_report(void) {
    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("Bible Rail Tests Complete\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Passed: %d\n", tests_passed);

    if (tests_failed > 0) {
        printf("  Failed: %d  ← investigate these\n", tests_failed);
    } else {
        printf("  Failed: 0  %s\n", PASS);
    }

    printf("\n  \"Add thou not unto his words, lest he reprove thee,\n");
    printf("   and thou be found a liar.\" — Proverbs 30:6\n");
    printf("\n  13 spare bytes. 13 textual variants. Nothing wasted.\n");
    printf("  Every bit serves the Word.\n");
    printf("\n════════════════════════════════════════════════════════════════\n");
}

// Public: Reset test state
void test_bible_rail_reset(void) {
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
// This test validates Bible Rail encoding - the mapping of spare
// trit5 values (243-255) to WEB textual variants.
//
// Test Functions:
//   - test_bible_rail_roundtrip()  → encode(i) → decode → i
//   - test_bible_rail_encode()     → valid/invalid input handling
//   - test_bible_rail_decode()     → valid/invalid input handling
//   - test_bible_rail_spare()      → semantic vs spare detection
//   - test_bible_rail_queries()    → ref, desc, count lookups
//   - test_bible_rail_all_spares() → exactly 13, contiguous
//
// Background:
//   Trit5 packing: 5 trits → 1 byte. 3^5 = 243 semantic values (0-242).
//   13 spare values (243-255) encode WEB textual variants.
//   See: word/research/bereshit/bereshit-trit-byte-encoding.adoc

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new test cases within test functions
//   ✅ Adjust messages and formatting
//   ✅ Add new test functions (update Org Chart and orchestration)
//
// Modify with Care:
//   ⚠️ Expected values - must match bible_rail.c and CSV
//   ⚠️ Public API signatures (affects external callers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ test_assert() semantics
//   ❌ Variant count (13 from trit5 math)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Library:
//   - word/work/pkg/trit/src/bible_rail.c (implementation)
//   - word/work/pkg/trit/include/bible_rail.h (interface)
//
// Data:
//   - word/scripture/web-variant-index.csv (13 WEB variants)
//
// Demo:
//   - tov/demo/phase-2/demo-math/ (demonstrates Bible Rail usage)
//
// Specs:
//   - word/research/bereshit/bereshit-trit-byte-encoding.adoc

// ============================================================================
// END CLOSING
// ============================================================================
