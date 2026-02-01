// ═══════════════════════════════════════════════════════════════════════════
// libtrit Unit Tests - Verse Address Encoding (31,102 KJV Verses)
// Key: B-word-work-pkg-trit-test-verse-addr
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a)
//   Links against libtrit.a for verse_addr functions.
//
// derives_from: bereshit/word/work/pkg/trit/test/bible_rail_test.c (structure)
// See: word/core/bible/addressing.toml for encoding spec
//
// ═══════════════════════════════════════════════════════════════════════════

// Unit tests for verse_addr.c - designed to FAIL MEANINGFULLY.
// When verse address encoding breaks, run these tests to narrow down WHERE.
//
// verse_addr_test - CPI-SI Kingdom Technology
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
//             not return unto me void." — Isaiah 55:11
//
// Principle: Every verse has an address. 31,102 verses in canonical order,
//            each with a unique ordinal that encodes to 2 bytes. The encoding
//            makes Scripture computable without losing meaning.
//
// Anchor: "Prove all things; hold fast that which is good."
//         — 1 Thessalonians 5:21
//
// # CPI-SI Identity
//
// Component Type: Baton (execution flow - diagnoses and reports)
//
// Role: Diagnostic infrastructure - finds failure points in verse address encoding.
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
// Purpose: Diagnose WHERE failures occur in verse_addr.c (encode/decode).
//
// Core Design: Tests designed to fail meaningfully, covering:
//   - test_verse_addr_roundtrip()     → encode/decode roundtrip for key verses
//   - test_verse_addr_encode()        → encode boundaries and invalid inputs
//   - test_verse_addr_decode()        → decode boundaries and invalid inputs
//   - test_verse_addr_valid()         → validity detection (semantic vs spare)
//   - test_verse_addr_boundaries()    → first/last verse, edge cases
//   - test_verse_addr_full_range()    → all 31,102 verses roundtrip
//
// Key Features:
//   - Roundtrip: ordinal 1-31102 → packed (high, low) → ordinal 1-31102
//   - Boundaries: valid (1, 31102) vs invalid (0, 31103)
//   - Validity: semantic low (0-242) vs spare low (243-255)
//   - Key verses: Genesis 1:1, Psalm 119:1, Revelation 22:21
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
//   - Standard: stdio.h (printf), stdbool.h (bool)
//   - Internal: verse_addr.h (includes trit.h)
//   - Library: libtrit.a (linked)
//   - Reference: word/core/bible/addressing.toml (encoding spec)
//
// What Uses This:
//   - Developers: Run when verse address encoding fails to find issue
//   - CI: Automated regression testing
//
// # Usage
//
// Build: make test-verse-addr
// Run: ./build/verse_addr_test
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

//--- Project Headers ---
#include "trit/trit.h"       // Master aggregator: includes bible layer

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
// PUBLIC API - Verse Address Test Functions
// ═══════════════════════════════════════════════════════════════
// These functions are PUBLIC - callable for diagnostics.
// When verse address breaks, these narrow down WHERE.

// Primary API (run tests)
int test_verse_addr_run_all(void);       // Run all tests, return failure count
int test_verse_addr_roundtrip(void);     // Test encode/decode roundtrip
int test_verse_addr_encode(void);        // Test encode boundaries
int test_verse_addr_decode(void);        // Test decode boundaries
int test_verse_addr_valid(void);         // Test validity detection
int test_verse_addr_boundaries(void);    // Test first/last verse, edge cases
int test_verse_addr_full_range(void);    // Test all 31,102 verses roundtrip
int test_verse_addr_variant(void);       // Test WEB variant encoding

// Utility API (inspection and cleanup)
int test_verse_addr_get_passed(void);    // Get count of passed tests
int test_verse_addr_get_failed(void);    // Get count of failed tests
void test_verse_addr_reset(void);        // Reset test counters

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
//   test_verse_addr_run_all()       → Run all tests, return failure count
//   ├── test_verse_addr_roundtrip()    → Encode→decode for key verses
//   ├── test_verse_addr_encode()       → Encode boundaries and invalids
//   ├── test_verse_addr_decode()       → Decode boundaries and invalids
//   ├── test_verse_addr_valid()        → Semantic vs spare detection
//   ├── test_verse_addr_boundaries()   → First/last verse edge cases
//   ├── test_verse_addr_full_range()   → All 31,102 verses roundtrip
//   └── test_verse_addr_variant()      → WEB variant encoding (13 variants)
//
// Entry Point:
//   main() → test_verse_addr_run_all() → exit code

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
// test_verse_addr_roundtrip: Encode/Decode Roundtrip
// ────────────────────────────────────────────────────────────────
//
// Tests that key ordinals encode to packed form and decode back.
// Verifies bijection for representative verses.

int test_verse_addr_roundtrip(void) {
    print_header("Roundtrip: Encode → Decode for Key Verses");

    printf("\n  Testing ordinal → packed → ordinal:\n");

    // Key verses to test
    uint16_t key_ordinals[] = {
        1,      // Genesis 1:1
        243,    // First multiple of 243
        244,    // One past first multiple
        1533,   // End of Genesis
        15766,  // Psalm 119:1 (middle of Bible)
        23145,  // End of OT (Malachi 4:6)
        23146,  // Start of NT (Matthew 1:1)
        31102   // Revelation 22:21 (last verse)
    };
    int num_keys = sizeof(key_ordinals) / sizeof(key_ordinals[0]);

    for (int i = 0; i < num_keys; i++) {
        uint16_t ordinal = key_ordinals[i];
        verse_addr_t addr = verse_addr_encode(ordinal);
        uint16_t decoded = verse_addr_decode(addr);

        char name[100];
        snprintf(name, sizeof(name),
                 "ordinal %5u: encode→{%3u,%3u}, decode→%5u",
                 ordinal, addr.high, addr.low, decoded);
        test_assert(decoded == ordinal, name);
    }

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_verse_addr_encode: Encode Boundaries
// ────────────────────────────────────────────────────────────────
//
// Tests encode function with valid and invalid inputs.
// Valid: 1-31102. Invalid: 0, 31103 → {0,0}.

int test_verse_addr_encode(void) {
    print_header("Encode: Boundaries and Invalid Inputs");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Valid boundaries
    // ─────────────────────────────────────────────────────────────
    printf("\n  Valid encode boundaries:\n");

    verse_addr_t first = verse_addr_encode(1);
    test_assert(first.high == 0 && first.low == 1, "encode(1) == {0,1} (Genesis 1:1)");

    verse_addr_t last = verse_addr_encode(31102);
    test_assert(last.high == 127 && last.low == 241, "encode(31102) == {127,241} (Revelation 22:21)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Invalid inputs (return {0,0})
    // ─────────────────────────────────────────────────────────────
    printf("\n  Invalid encode inputs (return {0,0}):\n");

    verse_addr_t zero = verse_addr_encode(0);
    test_assert(zero.high == 0 && zero.low == 0, "encode(0) == {0,0} (invalid: below range)");

    verse_addr_t over = verse_addr_encode(31103);
    test_assert(over.high == 0 && over.low == 0, "encode(31103) == {0,0} (invalid: above range)");

    verse_addr_t far_over = verse_addr_encode(50000);
    test_assert(far_over.high == 0 && far_over.low == 0, "encode(50000) == {0,0} (invalid: far above)");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_verse_addr_decode: Decode Boundaries
// ────────────────────────────────────────────────────────────────
//
// Tests decode function with valid and invalid inputs.
// Valid packed addresses (low < 243) decode to ordinals.
// Invalid: low >= 243 (WEB variant marker) → 0.

int test_verse_addr_decode(void) {
    print_header("Decode: Boundaries and Invalid Inputs");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Valid packed addresses
    // ─────────────────────────────────────────────────────────────
    printf("\n  Valid decode (semantic addresses):\n");

    verse_addr_t first = {0, 1};
    test_assert(verse_addr_decode(first) == 1, "decode({0,1}) == 1 (Genesis 1:1)");

    verse_addr_t last = {127, 241};
    test_assert(verse_addr_decode(last) == 31102, "decode({127,241}) == 31102 (Revelation 22:21)");

    verse_addr_t mid = {64, 220};
    uint16_t mid_ordinal = (64 * 243) + 220;  // 15772
    test_assert(verse_addr_decode(mid) == mid_ordinal, "decode({64,220}) == 15772 (middle)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Invalid inputs (return 0)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Invalid decode inputs (return 0):\n");

    verse_addr_t zero_addr = {0, 0};
    test_assert(verse_addr_decode(zero_addr) == 0, "decode({0,0}) == 0 (invalid: ordinal 0)");

    verse_addr_t spare_low = {0, 243};
    test_assert(verse_addr_decode(spare_low) == 0, "decode({0,243}) == 0 (invalid: WEB variant)");

    verse_addr_t spare_max = {0, 255};
    test_assert(verse_addr_decode(spare_max) == 0, "decode({0,255}) == 0 (invalid: WEB variant)");

    verse_addr_t over_range = {200, 100};
    test_assert(verse_addr_decode(over_range) == 0, "decode({200,100}) == 0 (invalid: > 31102)");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_verse_addr_valid: Validity Detection
// ────────────────────────────────────────────────────────────────
//
// Tests is_valid functions to distinguish valid from invalid addresses/ordinals.

int test_verse_addr_valid(void) {
    print_header("Validity Detection: Address and Ordinal Validation");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: verse_addr_is_valid (packed address validation)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Address validity (low < 243 = valid):\n");

    verse_addr_t valid1 = {0, 1};
    test_assert(verse_addr_is_valid(valid1), "is_valid({0,1}) == true (Genesis 1:1)");

    verse_addr_t valid2 = {127, 241};
    test_assert(verse_addr_is_valid(valid2), "is_valid({127,241}) == true (Revelation 22:21)");

    verse_addr_t valid_zero_low = {0, 0};
    test_assert(verse_addr_is_valid(valid_zero_low), "is_valid({0,0}) == true (low=0 is semantic)");

    verse_addr_t valid_max_low = {0, 242};
    test_assert(verse_addr_is_valid(valid_max_low), "is_valid({0,242}) == true (last semantic)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Invalid addresses (low >= 243 = WEB variant)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Invalid addresses (low >= 243 = WEB variant marker):\n");

    verse_addr_t spare1 = {0, 243};
    test_assert(!verse_addr_is_valid(spare1), "is_valid({0,243}) == false (first spare)");

    verse_addr_t spare2 = {0, 255};
    test_assert(!verse_addr_is_valid(spare2), "is_valid({0,255}) == false (last spare)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: verse_addr_is_valid_ordinal
    // ─────────────────────────────────────────────────────────────
    printf("\n  Ordinal validity (1-31102):\n");

    test_assert(verse_addr_is_valid_ordinal(1), "is_valid_ordinal(1) == true (Genesis 1:1)");
    test_assert(verse_addr_is_valid_ordinal(31102), "is_valid_ordinal(31102) == true (Revelation 22:21)");
    test_assert(verse_addr_is_valid_ordinal(15551), "is_valid_ordinal(15551) == true (middle)");
    test_assert(!verse_addr_is_valid_ordinal(0), "is_valid_ordinal(0) == false (below range)");
    test_assert(!verse_addr_is_valid_ordinal(31103), "is_valid_ordinal(31103) == false (above range)");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_verse_addr_boundaries: First/Last Verse and Edge Cases
// ────────────────────────────────────────────────────────────────
//
// Tests query functions and boundary conditions for verse addressing.

int test_verse_addr_boundaries(void) {
    print_header("Query Functions and Boundary Conditions");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Query functions
    // ─────────────────────────────────────────────────────────────
    printf("\n  Query functions:\n");

    test_assert(verse_addr_max_ordinal() == 31102, "max_ordinal() == 31102");
    test_assert(verse_addr_trit5_base() == 243, "trit5_base() == 243");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: First verse (Genesis 1:1)
    // ─────────────────────────────────────────────────────────────
    printf("\n  First verse - Genesis 1:1 (ordinal 1):\n");

    verse_addr_t gen1_1 = verse_addr_encode(1);
    test_assert(gen1_1.high == 0, "Genesis 1:1 high == 0");
    test_assert(gen1_1.low == 1, "Genesis 1:1 low == 1");
    test_assert(verse_addr_decode(gen1_1) == 1, "Genesis 1:1 roundtrip");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: Last verse (Revelation 22:21)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Last verse - Revelation 22:21 (ordinal 31102):\n");

    verse_addr_t rev22_21 = verse_addr_encode(31102);
    test_assert(rev22_21.high == 127, "Revelation 22:21 high == 127");
    test_assert(rev22_21.low == 241, "Revelation 22:21 low == 241");
    test_assert(verse_addr_decode(rev22_21) == 31102, "Revelation 22:21 roundtrip");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 4: Arithmetic verification (31102 = 127*243 + 241)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Arithmetic verification:\n");

    uint16_t computed = (127 * 243) + 241;
    test_assert(computed == 31102, "127*243 + 241 == 31102");

    // Verify first multiple boundary
    verse_addr_t at_243 = verse_addr_encode(243);
    test_assert(at_243.high == 1 && at_243.low == 0, "encode(243) == {1,0}");

    verse_addr_t at_244 = verse_addr_encode(244);
    test_assert(at_244.high == 1 && at_244.low == 1, "encode(244) == {1,1}");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_verse_addr_full_range: All 31,102 Verses Roundtrip
// ────────────────────────────────────────────────────────────────
//
// Exhaustively tests encode/decode roundtrip for all valid ordinals.
// Reports first failure point if any.

int test_verse_addr_full_range(void) {
    print_header("Full Range: All 31,102 Verses Roundtrip");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Exhaustive roundtrip test
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing ordinals 1-31102 (all KJV verses):\n");

    int failures = 0;
    uint16_t first_failure = 0;

    for (uint16_t ordinal = 1; ordinal <= 31102; ordinal++) {
        verse_addr_t addr = verse_addr_encode(ordinal);
        uint16_t decoded = verse_addr_decode(addr);

        if (decoded != ordinal) {
            if (first_failure == 0) {
                first_failure = ordinal;
            }
            failures++;
        }
    }

    if (failures == 0) {
        test_assert(1, "all 31,102 ordinals encode/decode correctly");
    } else {
        char msg[100];
        snprintf(msg, sizeof(msg),
                 "FAILED: %d ordinals failed, first at %u",
                 failures, first_failure);
        test_assert(0, msg);
    }

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Verify valid range count
    // ─────────────────────────────────────────────────────────────
    printf("\n  Counting valid ordinals:\n");

    int valid_count = 0;
    for (uint32_t i = 0; i <= 65535; i++) {
        if (verse_addr_is_valid_ordinal((uint16_t)i)) {
            valid_count++;
        }
    }
    test_assert(valid_count == 31102, "exactly 31,102 valid ordinals in uint16 range");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: Verify high byte distribution
    // ─────────────────────────────────────────────────────────────
    printf("\n  High byte range verification:\n");

    // Max ordinal 31102 / 243 = 127.99 → high byte range is 0-127
    verse_addr_t max_addr = verse_addr_encode(31102);
    test_assert(max_addr.high == 127, "maximum high byte is 127");

    verse_addr_t min_addr = verse_addr_encode(1);
    test_assert(min_addr.high == 0, "minimum high byte is 0");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_verse_addr_variant: WEB Variant Encoding
// ────────────────────────────────────────────────────────────────
//
// Tests verse_addr_encode_variant(), verse_addr_is_variant(),
// and verse_addr_decode_variant() - the WEB variant encoding bridge.
//
int test_verse_addr_variant(void) {
    print_header("WEB Variant Encoding: 13 Variant Addresses");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Encode variant indices
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing encode_variant for all 13 indices:\n");

    for (int i = 0; i < 13; i++) {
        verse_addr_t addr = verse_addr_encode_variant(i);
        int expected_low = 243 + i;
        char desc[64];
        snprintf(desc, sizeof(desc), "encode_variant(%d) → {0, %d}", i, expected_low);
        test_assert(addr.high == 0 && addr.low == expected_low, desc);
    }

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Invalid variant indices
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing encode_variant for invalid indices:\n");

    verse_addr_t invalid = verse_addr_encode_variant(-1);
    test_assert(invalid.high == 0 && invalid.low == 0, "encode_variant(-1) → {0, 0}");

    invalid = verse_addr_encode_variant(13);
    test_assert(invalid.high == 0 && invalid.low == 0, "encode_variant(13) → {0, 0}");

    invalid = verse_addr_encode_variant(100);
    test_assert(invalid.high == 0 && invalid.low == 0, "encode_variant(100) → {0, 0}");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: is_variant detection
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing is_variant detection:\n");

    // KJV verse addresses (semantic) should NOT be variants
    verse_addr_t kjv_addr = verse_addr_encode(1);  // Genesis 1:1
    test_assert(!verse_addr_is_variant(kjv_addr), "Genesis 1:1 is NOT variant");

    kjv_addr = verse_addr_encode(31102);  // Revelation 22:21
    test_assert(!verse_addr_is_variant(kjv_addr), "Revelation 22:21 is NOT variant");

    // WEB variant addresses (spare) SHOULD be variants
    verse_addr_t web_addr = verse_addr_encode_variant(0);  // First variant
    test_assert(verse_addr_is_variant(web_addr), "variant index 0 IS variant");

    web_addr = verse_addr_encode_variant(12);  // Last variant
    test_assert(verse_addr_is_variant(web_addr), "variant index 12 IS variant");

    // Boundary test
    verse_addr_t boundary_semantic = {0, 242};  // Last semantic
    test_assert(!verse_addr_is_variant(boundary_semantic), "low=242 is NOT variant");

    verse_addr_t boundary_spare = {0, 243};  // First spare
    test_assert(verse_addr_is_variant(boundary_spare), "low=243 IS variant");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 4: Decode variant addresses
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing decode_variant:\n");

    for (int i = 0; i < 13; i++) {
        verse_addr_t addr = verse_addr_encode_variant(i);
        int decoded = verse_addr_decode_variant(addr);
        char desc[64];
        snprintf(desc, sizeof(desc), "decode_variant({0, %d}) → %d", 243 + i, i);
        test_assert(decoded == i, desc);
    }

    // Non-variant addresses should return -1
    int non_variant = verse_addr_decode_variant(verse_addr_encode(1));
    test_assert(non_variant == -1, "decode_variant(Genesis 1:1) → -1");

    non_variant = verse_addr_decode_variant(verse_addr_encode(15000));
    test_assert(non_variant == -1, "decode_variant(middle KJV) → -1");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 5: Roundtrip variant encode/decode
    // ─────────────────────────────────────────────────────────────
    printf("\n  Testing variant encode/decode roundtrip:\n");

    int roundtrip_ok = 1;
    for (int i = 0; i < 13; i++) {
        verse_addr_t addr = verse_addr_encode_variant(i);
        int decoded = verse_addr_decode_variant(addr);
        if (decoded != i) {
            roundtrip_ok = 0;
            break;
        }
    }
    test_assert(roundtrip_ok, "all 13 variants roundtrip correctly");

    return tests_failed;
}

// ════════════════════════════════════════════════════════════════
// SECTION 3: TEST ORCHESTRATION
// ════════════════════════════════════════════════════════════════

// Run all verse address tests
int test_verse_addr_run_all(void) {
    tests_passed = 0;
    tests_failed = 0;

    printf("════════════════════════════════════════════════════════════════\n");
    printf("Phase 2 Unit Tests: Verse Address Encoding (31,102 KJV Verses)\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Testing ordinal (1-31102) ↔ packed (high, low) encoding\n");

    // Validate prerequisites
    if (!test_validate_library()) {
        test_cleanup_and_report();
        return tests_failed;
    }

    // Run all test functions
    test_verse_addr_roundtrip();
    test_verse_addr_encode();
    test_verse_addr_decode();
    test_verse_addr_valid();
    test_verse_addr_boundaries();
    test_verse_addr_full_range();
    test_verse_addr_variant();

    // Report results
    test_cleanup_and_report();

    return tests_failed;
}

// ════════════════════════════════════════════════════════════════
// SECTION 4: ENTRY POINT
// ════════════════════════════════════════════════════════════════

int main(void) {
    int failures = test_verse_addr_run_all();
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
    printf("Validating libtrit verse address prerequisites...\n");
    printf("────────────────────────────────────────────────────────────────\n");

    int valid = 1;

    // Check verse_addr.h included properly
    uint16_t max = verse_addr_max_ordinal();
    if (max != 31102) {
        printf("  %s max_ordinal() != 31102 (got %u)\n", FAIL, max);
        valid = 0;
    } else {
        printf("  %s verse_addr functions accessible\n", PASS);
    }

    // Check constants defined
    uint16_t base = verse_addr_trit5_base();
    if (base != 243) {
        printf("  %s trit5_base() != 243 (got %u)\n", FAIL, base);
        valid = 0;
    } else {
        printf("  %s trit5 base correct (243)\n", PASS);
    }

    // Check basic encode/decode
    verse_addr_t addr = verse_addr_encode(1);
    uint16_t decoded = verse_addr_decode(addr);
    if (decoded != 1) {
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
int test_verse_addr_get_passed(void) { return tests_passed; }
int test_verse_addr_get_failed(void) { return tests_failed; }

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────

// Report final results
static void test_cleanup_and_report(void) {
    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("Verse Address Tests Complete\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Passed: %d\n", tests_passed);

    if (tests_failed > 0) {
        printf("  Failed: %d  ← investigate these\n", tests_failed);
    } else {
        printf("  Failed: 0  %s\n", PASS);
    }

    printf("\n  \"So shall my word be that goeth forth out of my mouth:\n");
    printf("   it shall not return unto me void.\" — Isaiah 55:11\n");
    printf("\n  31,102 verses. 2 bytes each. Every verse addressable.\n");
    printf("  The encoding reveals Scripture's order.\n");
    printf("\n════════════════════════════════════════════════════════════════\n");
}

// Public: Reset test state
void test_verse_addr_reset(void) {
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
// This test validates verse address encoding - the mapping of
// ordinals (1-31102) to packed trit5 pairs (high, low).
//
// Test Functions:
//   - test_verse_addr_roundtrip()    → ordinal → packed → ordinal
//   - test_verse_addr_encode()       → valid/invalid input handling
//   - test_verse_addr_decode()       → valid/invalid input handling
//   - test_verse_addr_valid()        → address and ordinal validation
//   - test_verse_addr_boundaries()   → first/last verse, query functions
//   - test_verse_addr_full_range()   → all 31,102 verses roundtrip
//
// Background:
//   Verse encoding uses base 243: high = ordinal / 243, low = ordinal % 243.
//   2 bytes per verse, addressing all 31,102 KJV canonical verses.
//   See: word/core/bible/addressing.toml

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
//   ⚠️ Expected values - must match verse_addr.c and addressing.toml
//   ⚠️ Public API signatures (affects external callers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ test_assert() semantics
//   ❌ Max ordinal (31102 from KJV canonical count)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Library:
//   - word/work/pkg/trit/src/verse_addr.c (implementation)
//   - word/work/pkg/trit/include/verse_addr.h (interface)
//
// Data:
//   - word/scripture/kjv-ordinal-index.csv (31,102 verse ordinals)
//
// Demo:
//   - tov/demo/phase-2/demo-math/ (demonstrates verse addressing)
//
// Specs:
//   - word/core/bible/addressing.toml (encoding specification)

// ============================================================================
// END CLOSING
// ============================================================================
