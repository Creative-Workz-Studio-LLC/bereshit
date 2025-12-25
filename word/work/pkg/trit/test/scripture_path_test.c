// ═══════════════════════════════════════════════════════════════════════════
// libtrit Unit Tests - Scripture Path Resolution (Variant-Aware Lookup)
// Key: B-word-work-pkg-trit-test-scripture-path
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a)
//   Links against libtrit.a for scripture_path functions.
//
// derives_from: bereshit/word/work/pkg/trit/test/verse_addr_test.c (structure)
// See: word/scripture/ for filesystem layout
//
// ═══════════════════════════════════════════════════════════════════════════

// Unit tests for scripture_path.c - designed to FAIL MEANINGFULLY.
// When scripture path resolution breaks, run these tests to narrow down WHERE.
//
// scripture_path_test - CPI-SI Kingdom Technology
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
// Scripture: "Thy word is a lamp unto my feet, and a light unto my path."
//            — Psalm 119:105
//
// Principle: Scripture paths are not arbitrary—they follow the Word's order.
//            Testing verifies the path generation matches filesystem reality.
//
// Anchor: "Prove all things; hold fast that which is good."
//         — 1 Thessalonians 5:21
//
// # CPI-SI Identity
//
// Component Type: Baton (execution flow - diagnoses and reports)
//
// Role: Diagnostic infrastructure - finds failure points in scripture path resolution.
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
// Purpose: Diagnose WHERE failures occur in scripture_path.c (path generation).
//
// Core Design: Tests designed to fail meaningfully, covering:
//   - test_scripture_path_normalize() → book name normalization
//   - test_scripture_path_kjv()       → KJV path generation
//   - test_scripture_path_web()       → WEB path generation
//   - test_scripture_path_resolve()   → variant-aware address resolution
//   - test_scripture_path_type()      → balanced ternary type names
//
// Key Features:
//   - Normalization: "1 Corinthians" → "1_Corinthians"
//   - Path format: word/scripture/{KJV|WEB}/{Book}/Chapter_{N}/Verse_{M}.txt
//   - Balanced ternary: -1 (INVALID), 0 (KJV_VERSE), +1 (WEB_VARIANT)
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
//   - Internal: scripture_path.h (includes bible_rail.h, verse_addr.h)
//   - Library: libtrit.a (linked)
//   - Filesystem: word/scripture/KJV/, word/scripture/WEB/
//
// What Uses This:
//   - Developers: Run when scripture path resolution fails
//   - CI: Automated regression testing
//
// # Usage
//
// Build: make test-scripture-path
// Run: ./build/scripture_path_test
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
#include "scripture_path.h"  // Scripture path resolution (includes bible_rail.h, verse_addr.h)

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
// PUBLIC API - Scripture Path Test Functions
// ═══════════════════════════════════════════════════════════════
// These functions are PUBLIC - callable for diagnostics.
// When scripture path breaks, these narrow down WHERE.

// Primary API (run tests)
int test_scripture_path_run_all(void);     // Run all tests, return failure count
int test_scripture_path_normalize(void);   // Test book name normalization
int test_scripture_path_kjv(void);         // Test KJV path generation
int test_scripture_path_resolve(void);     // Test variant-aware resolution
int test_scripture_path_type(void);        // Test balanced ternary type names
int test_scripture_path_version(void);     // Test version-aware functions

// Utility API (inspection and cleanup)
int test_scripture_path_get_passed(void);  // Get count of passed tests
int test_scripture_path_get_failed(void);  // Get count of failed tests
void test_scripture_path_reset(void);      // Reset test counters

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
//   test_scripture_path_run_all()    → Run all tests, return failure count
//   ├── test_scripture_path_normalize() → Book name normalization
//   ├── test_scripture_path_kjv()       → KJV path generation
//   ├── test_scripture_path_resolve()   → Variant-aware address resolution
//   └── test_scripture_path_type()      → Balanced ternary type names
//
// Entry Point:
//   main() → test_scripture_path_run_all() → exit code

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
// test_scripture_path_normalize: Book Name Normalization
// ────────────────────────────────────────────────────────────────
//
// Tests that book names convert to filesystem format.
// "1 Corinthians" → "1_Corinthians" (spaces to underscores).

int test_scripture_path_normalize(void) {
    print_header("Normalize: Book Name to Filesystem Format");

    char out[64];

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Simple book names (no spaces)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Simple book names:\n");

    test_assert(scripture_path_normalize_book("Genesis", out, sizeof(out)) &&
                strcmp(out, "Genesis") == 0, "Genesis → Genesis");

    test_assert(scripture_path_normalize_book("Matthew", out, sizeof(out)) &&
                strcmp(out, "Matthew") == 0, "Matthew → Matthew");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Book names with spaces
    // ─────────────────────────────────────────────────────────────
    printf("\n  Book names with spaces:\n");

    test_assert(scripture_path_normalize_book("1 Corinthians", out, sizeof(out)) &&
                strcmp(out, "1_Corinthians") == 0, "1 Corinthians → 1_Corinthians");

    test_assert(scripture_path_normalize_book("Song of Solomon", out, sizeof(out)) &&
                strcmp(out, "Song_of_Solomon") == 0, "Song of Solomon → Song_of_Solomon");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: Edge cases
    // ─────────────────────────────────────────────────────────────
    printf("\n  Edge cases:\n");

    test_assert(!scripture_path_normalize_book(NULL, out, sizeof(out)),
                "NULL book → false");

    test_assert(!scripture_path_normalize_book("Genesis", NULL, sizeof(out)),
                "NULL output → false");

    test_assert(!scripture_path_normalize_book("Genesis", out, 0),
                "zero length → false");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_scripture_path_kjv: KJV Path Generation
// ────────────────────────────────────────────────────────────────
//
// Tests KJV path generation from book/chapter/verse.

int test_scripture_path_kjv(void) {
    print_header("KJV Path: Generate Filesystem Paths");

    char path[SCRIPTURE_PATH_MAX_LEN];

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Genesis 1:1 (first verse)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Key verses:\n");

    test_assert(scripture_path_kjv("Genesis", 1, 1, path, sizeof(path)) &&
                strcmp(path, "word/scripture/KJV/Genesis/Chapter_1/Verse_1.txt") == 0,
                "Genesis 1:1 path");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Books with spaces
    // ─────────────────────────────────────────────────────────────
    printf("\n  Books with spaces:\n");

    test_assert(scripture_path_kjv("1 Corinthians", 16, 27, path, sizeof(path)) &&
                strcmp(path, "word/scripture/KJV/1_Corinthians/Chapter_16/Verse_27.txt") == 0,
                "1 Corinthians 16:27 path");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: WEB path (for comparison)
    // ─────────────────────────────────────────────────────────────
    printf("\n  WEB paths:\n");

    test_assert(scripture_path_web("Genesis", 1, 1, path, sizeof(path)) &&
                strcmp(path, "word/scripture/WEB/Genesis/Chapter_1/Verse_1.txt") == 0,
                "Genesis 1:1 WEB path");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_scripture_path_resolve: Variant-Aware Resolution
// ────────────────────────────────────────────────────────────────
//
// Tests that packed addresses resolve correctly to KJV or WEB paths.

int test_scripture_path_resolve(void) {
    print_header("Resolve: Variant-Aware Address Resolution");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: KJV verse addresses
    // ─────────────────────────────────────────────────────────────
    printf("\n  KJV verse addresses (semantic):\n");

    verse_addr_t kjv = verse_addr_encode(1);  // Genesis 1:1
    scripture_path_result_t result = scripture_path_resolve(kjv);

    test_assert(result.type == SCRIPTURE_PATH_KJV_VERSE,
                "Genesis 1:1 type == KJV_VERSE (0)");
    test_assert(result.variant_index == -1,
                "Genesis 1:1 variant_index == -1");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: WEB variant addresses
    // ─────────────────────────────────────────────────────────────
    printf("\n  WEB variant addresses (spare):\n");

    verse_addr_t web = verse_addr_encode_variant(0);  // First variant
    result = scripture_path_resolve(web);

    test_assert(result.type == SCRIPTURE_PATH_WEB_VARIANT,
                "variant 0 type == WEB_VARIANT (+1)");
    test_assert(result.variant_index == 0,
                "variant 0 variant_index == 0");
    test_assert(strlen(result.path) > 0,
                "variant 0 has generated path");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: Resolve with location (full path)
    // ─────────────────────────────────────────────────────────────
    printf("\n  Resolve with known location:\n");

    result = scripture_path_resolve_with_location(kjv, "Genesis", 1, 1);
    test_assert(result.type == SCRIPTURE_PATH_KJV_VERSE,
                "Genesis 1:1 with location type == KJV_VERSE");
    test_assert(strcmp(result.path, "word/scripture/KJV/Genesis/Chapter_1/Verse_1.txt") == 0,
                "Genesis 1:1 with location has full path");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_scripture_path_type: Balanced Ternary Type Names
// ────────────────────────────────────────────────────────────────
//
// Tests type_name returns correct strings for balanced ternary enum.

int test_scripture_path_type(void) {
    print_header("Type Names: Balanced Ternary Enum");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Type names for each value
    // ─────────────────────────────────────────────────────────────
    printf("\n  Balanced ternary type names:\n");

    test_assert(strcmp(scripture_path_type_name(SCRIPTURE_PATH_INVALID), "INVALID") == 0,
                "-1 → INVALID");
    test_assert(strcmp(scripture_path_type_name(SCRIPTURE_PATH_KJV_VERSE), "KJV_VERSE") == 0,
                " 0 → KJV_VERSE");
    test_assert(strcmp(scripture_path_type_name(SCRIPTURE_PATH_WEB_VARIANT), "WEB_VARIANT") == 0,
                "+1 → WEB_VARIANT");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Verify enum values
    // ─────────────────────────────────────────────────────────────
    printf("\n  Enum value verification:\n");

    test_assert(SCRIPTURE_PATH_INVALID == -1, "INVALID == -1");
    test_assert(SCRIPTURE_PATH_KJV_VERSE == 0, "KJV_VERSE == 0");
    test_assert(SCRIPTURE_PATH_WEB_VARIANT == 1, "WEB_VARIANT == +1");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// test_scripture_path_version: Version-Aware Functions
// ────────────────────────────────────────────────────────────────
//
// Tests version enum, for_version(), and version_name().

int test_scripture_path_version(void) {
    print_header("Version: KJV/WEB Version-Aware Functions");

    char path[SCRIPTURE_PATH_MAX_LEN];

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 1: Version enum values
    // ─────────────────────────────────────────────────────────────
    printf("\n  Version enum values:\n");

    test_assert(SCRIPTURE_VERSION_KJV == 0, "KJV == 0 (anchor)");
    test_assert(SCRIPTURE_VERSION_WEB == 1, "WEB == 1 (extension)");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 2: Version name strings
    // ─────────────────────────────────────────────────────────────
    printf("\n  Version name strings:\n");

    test_assert(strcmp(scripture_path_version_name(SCRIPTURE_VERSION_KJV), "KJV") == 0,
                "version_name(KJV) == \"KJV\"");
    test_assert(strcmp(scripture_path_version_name(SCRIPTURE_VERSION_WEB), "WEB") == 0,
                "version_name(WEB) == \"WEB\"");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 3: for_version() generates correct paths
    // ─────────────────────────────────────────────────────────────
    printf("\n  for_version() path generation:\n");

    // KJV Genesis 1:1
    test_assert(scripture_path_for_version("Genesis", 1, 1, SCRIPTURE_VERSION_KJV,
                                           path, sizeof(path)) &&
                strcmp(path, "word/scripture/KJV/Genesis/Chapter_1/Verse_1.txt") == 0,
                "for_version(Genesis 1:1, KJV) → KJV path");

    // WEB Genesis 1:1
    test_assert(scripture_path_for_version("Genesis", 1, 1, SCRIPTURE_VERSION_WEB,
                                           path, sizeof(path)) &&
                strcmp(path, "word/scripture/WEB/Genesis/Chapter_1/Verse_1.txt") == 0,
                "for_version(Genesis 1:1, WEB) → WEB path");

    // ─────────────────────────────────────────────────────────────
    // TEST GROUP 4: Same verse, both versions
    // ─────────────────────────────────────────────────────────────
    printf("\n  Same verse, both versions:\n");

    char kjv_path[SCRIPTURE_PATH_MAX_LEN];
    char web_path[SCRIPTURE_PATH_MAX_LEN];

    scripture_path_for_version("John", 3, 16, SCRIPTURE_VERSION_KJV,
                               kjv_path, sizeof(kjv_path));
    scripture_path_for_version("John", 3, 16, SCRIPTURE_VERSION_WEB,
                               web_path, sizeof(web_path));

    test_assert(strstr(kjv_path, "/KJV/") != NULL, "John 3:16 KJV has /KJV/ in path");
    test_assert(strstr(web_path, "/WEB/") != NULL, "John 3:16 WEB has /WEB/ in path");
    test_assert(strcmp(kjv_path, web_path) != 0, "KJV and WEB paths differ");

    return tests_failed;
}

// ════════════════════════════════════════════════════════════════
// SECTION 3: TEST ORCHESTRATION
// ════════════════════════════════════════════════════════════════

// Run all scripture path tests
int test_scripture_path_run_all(void) {
    tests_passed = 0;
    tests_failed = 0;

    printf("════════════════════════════════════════════════════════════════\n");
    printf("Phase 2 Unit Tests: Scripture Path Resolution\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Testing variant-aware file path generation\n");

    // Validate prerequisites
    if (!test_validate_library()) {
        test_cleanup_and_report();
        return tests_failed;
    }

    // Run all test functions
    test_scripture_path_normalize();
    test_scripture_path_kjv();
    test_scripture_path_resolve();
    test_scripture_path_type();
    test_scripture_path_version();

    // Report results
    test_cleanup_and_report();

    return tests_failed;
}

// Utility functions
int test_scripture_path_get_passed(void) { return tests_passed; }
int test_scripture_path_get_failed(void) { return tests_failed; }
void test_scripture_path_reset(void) { tests_passed = 0; tests_failed = 0; }

// ════════════════════════════════════════════════════════════════
// SECTION 4: ENTRY POINT
// ════════════════════════════════════════════════════════════════

int main(void) {
    int failures = test_scripture_path_run_all();
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

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────

// Report final results
static void test_cleanup_and_report(void) {
    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("Scripture Path Tests Complete\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Passed: %d\n", tests_passed);

    if (tests_failed > 0) {
        printf("  Failed: %d  ← investigate these\n", tests_failed);
    } else {
        printf("  Failed: 0  %s\n", PASS);
    }

    printf("\n  \"Thy word is a lamp unto my feet, and a light unto my path.\"\n");
    printf("   — Psalm 119:105\n");
    printf("\n  31,115 entries: 31,102 KJV + 13 WEB variants.\n");
    printf("  The path to Scripture follows divine order.\n");
    printf("\n════════════════════════════════════════════════════════════════\n");
}

// ════════════════════════════════════════════════════════════════
// GROUP 2: DOCUMENTATION
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Test Overview
// ────────────────────────────────────────────────────────────────
//
// This test validates scripture path resolution - generating filesystem
// paths from packed addresses for both KJV verses and WEB variants.
//
// Test Functions:
//   - test_scripture_path_normalize() → book name normalization
//   - test_scripture_path_kjv()       → KJV path generation
//   - test_scripture_path_resolve()   → variant-aware resolution
//   - test_scripture_path_type()      → balanced ternary enum values
//
// Background:
//   Scripture paths map packed addresses to filesystem locations.
//   31,115 entries: 31,102 KJV verses + 13 WEB textual variants.
//   Balanced ternary enum: -1 (INVALID), 0 (KJV_VERSE), +1 (WEB_VARIANT).
//   See: word/scripture/KJV/, word/scripture/WEB/

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
//   ⚠️ Expected path formats - must match scripture_path.c
//   ⚠️ Public API signatures (affects external callers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ test_assert() semantics
//   ❌ Balanced ternary enum values (-1, 0, +1)

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Library:
//   - word/work/pkg/trit/src/scripture_path.c (implementation)
//   - word/work/pkg/trit/include/scripture_path.h (interface)
//
// Dependencies:
//   - verse_addr.h (packed address types)
//   - bible_rail.h (variant location lookup)
//
// Filesystem:
//   - word/scripture/KJV/{Book}/Chapter_{N}/Verse_{M}.txt
//   - word/scripture/WEB/{Book}/Chapter_{N}/Verse_{M}.txt
//
// Demo:
//   - tov/demo/phase-2/demo-math/ (demonstrates scripture paths)

// ============================================================================
// END CLOSING
// ============================================================================
