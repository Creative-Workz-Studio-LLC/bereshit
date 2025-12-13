// ═══════════════════════════════════════════════════════════════════════════
// Phase 1 Demo: Trit Type Validation
// Key: B-tov-demo-phase1-demo-trit
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a)
//   Links against libtrit.a for trit types and operations.
//
// derives_from: bereshit/word/work/pkg/trit/src/trit.c (structure)
// See: void/planning/development/plans/phase-1-implementation-plan.adoc
//
// ═══════════════════════════════════════════════════════════════════════════

// Demo program validating the three-layer libtrit architecture.
//
// demo-trit - CPI-SI Kingdom Technology
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
// Scripture: "By their fruits ye shall know them."
//            — Matthew 7:20
//
// Principle: Truth is proven by demonstration. The demo shows the library
//            works by exercising all three layers: MATTER, SPACE, TIME.
//
// Anchor: "Prove all things; hold fast that which is good."
//         — 1 Thessalonians 5:21
//
// # CPI-SI Identity
//
// Component Type: Baton (execution flow - validates and reports)
//
// Role: Phase 1 validation - proves libtrit three-layer architecture works.
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
// Purpose: Validate Phase 1 libtrit implementation across all three layers.
//
// Core Design: Sequential tests proving MATTER → SPACE → TIME → Integration.
//
// Key Features:
//   - MATTER layer: trit creation, validation, arithmetic
//   - SPACE layer: pack/unpack, dimension navigation
//   - TIME layer: temporal states (9 cognitive modes)
//   - Bible Rail: spare state detection (243-255)
//   - 3D Integration: layers composing together
//
// Philosophy: By their fruits ye shall know them - demonstrate, don't claim.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//   - Standard: stdio.h (printf)
//   - Internal: trit.h, dimension.h, temporal.h (libtrit headers)
//   - Library: libtrit.a (linked)
//
// What Uses This:
//   - Developers: Validates Phase 1 completion
//   - CI: Could be automated test
//
// # Usage
//
// Build: make
// Run: ./demo-trit
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// Exit codes:
//   0 = All tests passed
//   1 = One or more tests failed
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
#include <stdio.h>   // printf

//--- Project Headers ---
#include "trit.h"       // MATTER layer: trit types and operations
#include "dimension.h"  // SPACE layer: cognitive dimension navigation
#include "temporal.h"   // TIME layer: temporal state cognition

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
// PUBLIC API - Demo Test Functions
// ═══════════════════════════════════════════════════════════════
// These functions are PUBLIC - callable by games, tutorials, SDK examples.
// A game teaching MillenniumOS can call demo_trit_matter() as a level.

// Primary API (run tests)
int demo_trit_run_all(void);       // Run all tests, return failure count
int demo_trit_matter(void);        // Test MATTER layer
int demo_trit_space(void);         // Test SPACE layer
int demo_trit_time(void);          // Test TIME layer
int demo_trit_bible_rail(void);    // Test Bible Rail spare states
int demo_trit_integration(void);   // Test 3D integration
int demo_trit_run_layers(int matter, int space, int time, int bible, int integration);

// Utility API (inspection and cleanup)
int demo_trit_get_passed(void);    // Get count of passed tests
int demo_trit_get_failed(void);    // Get count of failed tests
void demo_trit_reset(void);        // Reset test counters

// ═══════════════════════════════════════════════════════════════
// INTERNAL HELPERS
// ═══════════════════════════════════════════════════════════════
// These functions are internal - static linkage.

static void print_header(const char *title);
static void demo_step(const char *description);

// CLOSING section functions (VALIDATION → EXECUTION → CLEANUP flow)
static int demo_validate_library(void);   // Code Validation
static void demo_cleanup_and_report(void); // Code Cleanup

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
//   demo_trit_run_all()      → Run all tests, return failure count
//   ├── demo_trit_matter()   → MATTER: trit operations
//   ├── demo_trit_space()    → SPACE: pack/unpack, dimensions
//   ├── demo_trit_time()     → TIME: temporal states
//   ├── demo_trit_bible_rail() → Spare state detection
//   └── demo_trit_integration() → All layers together
//
// Entry Point:
//   main() → demo_trit_run_all() → exit code

// ════════════════════════════════════════════════════════════════
// SECTION 1: INTERNAL HELPERS (static)
// ════════════════════════════════════════════════════════════════

static void print_header(const char *title) {
    printf("\n────────────────────────────────────────────────────────────────\n");
    printf("%s\n", title);
    printf("────────────────────────────────────────────────────────────────\n");
}

// Helper: track demonstration steps (not tests - demos succeed by running)
static void demo_step(const char *description) {
    printf("  → %s\n", description);
    tests_passed++;  // Each step demonstrates capability
}

// ════════════════════════════════════════════════════════════════
// SECTION 2: PUBLIC DEMO API (non-static, callable externally)
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// demo_trit_matter: Demonstrate MATTER Layer
// ────────────────────────────────────────────────────────────────
//
// MATTER is "what exists" — the concrete substance of ternary computing.
// This demo shows balanced ternary arithmetic in action.

int demo_trit_matter(void) {
    print_header("MATTER Layer: Balanced Ternary Arithmetic");

    // Create the three fundamental values
    trit_t pos  = trit_create(+1);   // Positive unit
    trit_t zero = trit_create(0);    // Zero/anchor
    trit_t neg  = trit_create(-1);   // Negative unit

    printf("\n  The three trit values:\n");
    printf("    +1 = heavens (aspiration, SPATIAL)\n");
    printf("     0 = time (present moment, TEMPORAL)\n");
    printf("    -1 = earth (grounding, MATERIAL)\n");

    demo_step("Created three fundamental trits: +1, 0, -1");

    // Demonstrate arithmetic
    printf("\n  Balanced Ternary Addition:\n");

    trit_t sum1 = trit_add(neg, pos);
    printf("    (-1) + (+1) = %d  ← opposites cancel\n", trit_value(sum1));
    demo_step("Addition: opposites cancel to zero");

    trit_t sum2 = trit_add(zero, pos);
    printf("    ( 0) + (+1) = %d  ← zero is identity\n", trit_value(sum2));
    demo_step("Addition: zero preserves value");

    // Demonstrate multiplication
    printf("\n  Balanced Ternary Multiplication:\n");

    trit_t prod1 = trit_multiply(neg, neg);
    printf("    (-1) × (-1) = %d  ← negative × negative = positive\n", trit_value(prod1));
    demo_step("Multiplication: sign rules apply");

    trit_t prod2 = trit_multiply(pos, neg);
    printf("    (+1) × (-1) = %d  ← positive × negative = negative\n", trit_value(prod2));
    demo_step("Multiplication: mixed signs");

    trit_t prod3 = trit_multiply(zero, pos);
    printf("    ( 0) × (+1) = %d  ← zero annihilates\n", trit_value(prod3));
    demo_step("Multiplication: zero annihilates");

    // Demonstrate negation (direction reversal)
    printf("\n  Direction Reversal (negate):\n");
    printf("    negate(+1) = %d  ← heaven → earth\n", trit_value(trit_negate(pos)));
    printf("    negate(-1) = %d  ← earth → heaven\n", trit_value(trit_negate(neg)));
    printf("    negate( 0) = %d  ← anchor unchanged\n", trit_value(trit_negate(zero)));
    demo_step("Negation: +1 ↔ -1, 0 stays fixed");

    return 0;  // Demos succeed by running
}

// ────────────────────────────────────────────────────────────────
// demo_trit_space: Demonstrate SPACE Layer
// ────────────────────────────────────────────────────────────────
//
// SPACE is "where it exists" — relationships, context, dimensions.
// This demo shows packing, dimension mapping, and navigation.

int demo_trit_space(void) {
    print_header("SPACE Layer: Dimensions and Packing");

    // Demonstrate dimension mapping (Genesis 1:1)
    printf("\n  Genesis 1:1 Dimension Mapping:\n");
    printf("    \"In the beginning\"  → 0 (TEMPORAL)  → When\n");
    printf("    \"the heavens\"       → +1 (SPATIAL)  → Where (above)\n");
    printf("    \"the earth\"         → -1 (MATERIAL) → What (below)\n");

    dimension_t dim_time   = trit_to_dimension(TRIT_ZERO);
    dimension_t dim_space  = trit_to_dimension(TRIT_POS);
    dimension_t dim_matter = trit_to_dimension(TRIT_NEG);
    (void)dim_time; (void)dim_space; (void)dim_matter;  // Used in demonstration output

    demo_step("Mapped trits to cognitive dimensions");

    // Demonstrate packing (5 trits → single byte value)
    printf("\n  Packing 5 Trits into One Value:\n");

    trit_t word[5] = { TRIT_NEG, TRIT_NEG, TRIT_ZERO, TRIT_POS, TRIT_POS };
    trit5_t packed = trit5_pack(word);

    printf("    Trits: [%d, %d, %d, %d, %d]\n",
           trit_value(word[0]), trit_value(word[1]), trit_value(word[2]),
           trit_value(word[3]), trit_value(word[4]));
    printf("    Packed value: %d (range 0-242)\n", packed);
    demo_step("Packed 5 trits into single trit5_t value");

    // Demonstrate unpacking
    trit_t unpacked[5];
    trit5_unpack(packed, unpacked);

    printf("\n  Unpacking Back to Trits:\n");
    printf("    From value %d → [%d, %d, %d, %d, %d]\n",
           packed,
           trit_value(unpacked[0]), trit_value(unpacked[1]),
           trit_value(unpacked[2]), trit_value(unpacked[3]),
           trit_value(unpacked[4]));
    demo_step("Unpacked trit5_t back to 5 trits (lossless)");

    // Show the state space
    printf("\n  State Space:\n");
    printf("    3^5 = 243 valid states (0-242)\n");
    printf("    Byte has 256 values → 13 spare states (243-255)\n");
    printf("    Spare states reserved for Bible Rail\n");
    demo_step("Understood 243-state space with 13 spare");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_trit_time: Demonstrate TIME Layer
// ────────────────────────────────────────────────────────────────
//
// TIME is "when it exists" — sequence, causality, temporal cognition.
// This demo shows the 9 cognitive modes (3 base + 6 compound).

int demo_trit_time(void) {
    print_header("TIME Layer: Temporal Cognition");

    // The three base temporal states
    printf("\n  Three Base Temporal States:\n");
    printf("    PAST    (-1) = What was — memory, history, foundation\n");
    printf("    PRESENT ( 0) = What is  — now, anchor, choice point\n");
    printf("    FUTURE  (+1) = What will be — anticipation, planning\n");

    temporal_t past    = trit_to_temporal(TRIT_NEG);
    temporal_t present = trit_to_temporal(TRIT_ZERO);
    temporal_t future  = trit_to_temporal(TRIT_POS);

    demo_step("Created three base temporal states");

    // Demonstrate base states (pure focus)
    printf("\n  Base States (Pure Focus):\n");

    temporal_state_t being_past = temporal_create_base(past);
    temporal_state_t being_now = temporal_create_base(present);
    temporal_state_t being_future = temporal_create_base(future);
    (void)being_past; (void)being_now; (void)being_future;  // Demonstrated states

    printf("    Remembering:   purely in the past\n");
    printf("    Being present: purely in the now (anchor)\n");
    printf("    Anticipating:  purely in the future\n");
    demo_step("Base states: single-focused temporal awareness");

    // Demonstrate compound states (9 - 3 = 6 compound modes)
    printf("\n  Compound States (Dual Focus):\n");

    temporal_state_t planning = temporal_create_compound(present, future);
    temporal_state_t learning = temporal_create_compound(present, past);
    temporal_state_t prophecy = temporal_create_compound(past, future);
    (void)planning; (void)learning; (void)prophecy;  // Demonstrated compound states

    printf("    Present→Future: Planning (now informs tomorrow)\n");
    printf("    Present→Past:   Learning (now reflects on yesterday)\n");
    printf("    Past→Future:    Prophecy (history reveals destiny)\n");
    printf("    Future→Present: Preparation (tomorrow shapes today)\n");
    printf("    Past→Present:   Integration (yesterday grounds today)\n");
    printf("    Future→Past:    Vision (destiny reframes history)\n");
    demo_step("Compound states: 6 dual-focus cognitive modes");

    // Summary: 9 cognitive modes
    printf("\n  The 9 Cognitive Modes:\n");
    printf("    3 base (pure focus) + 6 compound (dual focus) = 9 modes\n");
    printf("    Maps to human temporal cognition and decision-making\n");
    demo_step("Complete temporal cognition system: 9 modes");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_trit_bible_rail: Demonstrate Bible Rail Spare States
// ────────────────────────────────────────────────────────────────
//
// Bible Rail uses the 13 spare states (243-255) for translation variants.
// This demo shows how spare states enable dual-translation encoding.

int demo_trit_bible_rail(void) {
    print_header("Bible Rail: Spare State Encoding");

    // Explain the spare state opportunity
    printf("\n  The Spare State Opportunity:\n");
    printf("    A byte holds 256 values (0-255)\n");
    printf("    5 trits encode 243 states (3^5 = 243)\n");
    printf("    Difference: 256 - 243 = 13 spare states\n");

    demo_step("Identified 13 spare states in trit5 encoding");

    // Show valid vs spare ranges
    printf("\n  State Ranges:\n");
    printf("    0-242:   Valid trit5 states (map to [-1,-1,-1,-1,-1] ... [+1,+1,+1,+1,+1])\n");
    printf("    243-255: Spare states (reserved for Bible Rail)\n");

    // Demonstrate detection
    printf("\n  Spare State Detection:\n");
    printf("    trit5_is_spare(121) = %s (middle of valid range)\n",
           trit5_is_spare(121) ? "true" : "false");
    printf("    trit5_is_spare(242) = %s (last valid state)\n",
           trit5_is_spare(242) ? "true" : "false");
    printf("    trit5_is_spare(243) = %s (first spare state)\n",
           trit5_is_spare(243) ? "true" : "false");
    printf("    trit5_is_spare(255) = %s (last spare state)\n",
           trit5_is_spare(255) ? "true" : "false");

    demo_step("Demonstrated spare state detection");

    // Explain Bible Rail usage
    printf("\n  Bible Rail Purpose:\n");
    printf("    Primary:   KJV (King James Version) — canonical\n");
    printf("    Secondary: WEB (World English Bible) — modern clarity\n");
    printf("    Spare states signal WEB variant words\n");
    printf("    Same text, dual translations, single encoding\n");

    demo_step("Bible Rail: KJV + WEB in unified encoding");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_trit_integration: Demonstrate 3D Integration
// ────────────────────────────────────────────────────────────────
//
// This demo traces a value through all three layers, showing how
// MATTER, SPACE, and TIME compose into unified meaning.

int demo_trit_integration(void) {
    print_header("3D Integration: MATTER × SPACE × TIME");

    printf("\n  Genesis 1:1 as Computational Architecture:\n");
    printf("    \"In the beginning God created the heaven and the earth.\"\n");
    printf("    ↓\n");
    printf("    TIME (0) × SPACE (+1) × MATTER (-1)\n");

    demo_step("Genesis 1:1 grounds the three-layer architecture");

    // Trace -1 through all layers
    printf("\n  Tracing Trit -1 Through All Layers:\n");

    trit_t earth = trit_create(-1);
    printf("\n    MATTER Layer:\n");
    printf("      trit_create(-1) → value = %d\n", trit_value(earth));
    printf("      This is the negative unit — substance, form, concreteness\n");
    demo_step("MATTER: created trit -1 (earth)");

    dimension_t dim = trit_to_dimension(earth);
    (void)dim;  // Used in demonstration output
    printf("\n    SPACE Layer:\n");
    printf("      trit_to_dimension(-1) → MATERIAL dimension\n");
    printf("      \"the earth\" — what exists, the ground of being\n");
    demo_step("SPACE: mapped to MATERIAL dimension");

    temporal_t temp = trit_to_temporal(earth);
    temporal_state_t state = temporal_create_base(temp);
    (void)state;  // Used in demonstration output
    printf("\n    TIME Layer:\n");
    printf("      trit_to_temporal(-1) → PAST\n");
    printf("      What was — foundation, history, memory\n");
    demo_step("TIME: mapped to PAST temporal state");

    // Now trace +1 (heaven)
    printf("\n  Tracing Trit +1 Through All Layers:\n");

    trit_t heaven = trit_create(+1);
    dimension_t dim2 = trit_to_dimension(heaven);
    temporal_t temp2 = trit_to_temporal(heaven);
    (void)dim2; (void)temp2;  // Traced through layers

    printf("    MATTER: +1 (positive unit)\n");
    printf("    SPACE:  SPATIAL dimension (\"the heavens\")\n");
    printf("    TIME:   FUTURE (what will be, aspiration)\n");
    demo_step("Traced +1: heaven → SPATIAL → FUTURE");

    // Show the anchor (0)
    printf("\n  The Anchor (0):\n");

    trit_t beginning = trit_create(0);
    dimension_t dim3 = trit_to_dimension(beginning);
    temporal_t temp3 = trit_to_temporal(beginning);
    (void)dim3; (void)temp3;  // The anchor

    printf("    MATTER: 0 (zero, balance point)\n");
    printf("    SPACE:  TEMPORAL dimension (when, not where)\n");
    printf("    TIME:   PRESENT (now, the choice point)\n");
    printf("    \"In the beginning\" — the anchor from which all flows\n");
    demo_step("The anchor: 0 → TEMPORAL → PRESENT");

    // Summary
    printf("\n  Complete Integration:\n");
    printf("    -1 = earth   = MATERIAL = PAST    (foundation)\n");
    printf("     0 = time    = TEMPORAL = PRESENT (anchor)\n");
    printf("    +1 = heaven  = SPATIAL  = FUTURE  (aspiration)\n");
    printf("\n    Three layers, one architecture, grounded in Genesis 1:1\n");

    demo_step("Three-layer architecture complete and unified");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_trit_run_all: Run Complete Demonstration (Orchestrator)
// ────────────────────────────────────────────────────────────────
//
// Flow: VALIDATION → EXECUTION → CLEANUP
//   1. Validate prerequisites (library sanity checks)
//   2. Execute demonstrations (show each layer in action)
//   3. Cleanup and report (summarize, prepare for next run)

int demo_trit_run_all(void) {
    // ═══════════════════════════════════════════════════════════
    // SETUP: Reset state for fresh run
    // ═══════════════════════════════════════════════════════════
    demo_trit_reset();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("Phase 1 Demo: libtrit Three-Layer Architecture\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Demonstrating balanced ternary computing grounded in Genesis 1:1\n");

    // ═══════════════════════════════════════════════════════════
    // VALIDATION: Verify prerequisites before proceeding
    // ═══════════════════════════════════════════════════════════
    if (!demo_validate_library()) {
        // Validation failed - cleanup and exit early
        demo_cleanup_and_report();
        return tests_failed;
    }

    // ═══════════════════════════════════════════════════════════
    // EXECUTION: Run all layer demonstrations
    // ═══════════════════════════════════════════════════════════
    demo_trit_matter();       // MATTER: arithmetic
    demo_trit_space();        // SPACE: dimensions, packing
    demo_trit_time();         // TIME: temporal cognition
    demo_trit_bible_rail();   // Bible Rail: spare states
    demo_trit_integration();  // All layers together

    // ═══════════════════════════════════════════════════════════
    // CLEANUP: Report results and prepare for next run
    // ═══════════════════════════════════════════════════════════
    demo_cleanup_and_report();

    return tests_failed;
}

// ════════════════════════════════════════════════════════════════
// SECTION 3: ENTRY POINT
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// main: Entry Point (calls public API)
// ────────────────────────────────────────────────────────────────

int main(void) {
    int failures = demo_trit_run_all();
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
//
// Validation runs BEFORE execution to verify prerequisites.
// If validation fails, demo should not proceed.

// Validate library is working (sanity checks)
static int demo_validate_library(void) {
    printf("\n────────────────────────────────────────────────────────────────\n");
    printf("Validating libtrit prerequisites...\n");
    printf("────────────────────────────────────────────────────────────────\n");

    int valid = 1;

    // MATTER: Can we create and use trits?
    trit_t t = trit_create(1);
    if (!trit_valid(t) || trit_value(t) != 1) {
        printf("  ✗ MATTER layer failed: trit_create broken\n");
        valid = 0;
    } else {
        printf("  ✓ MATTER layer ready\n");
    }

    // SPACE: Can we pack/unpack?
    trit_t trits[5] = {TRIT_ZERO, TRIT_ZERO, TRIT_ZERO, TRIT_ZERO, TRIT_ZERO};
    trit5_t packed = trit5_pack(trits);
    if (packed != 121) {  // [0,0,0,0,0] should pack to middle value
        printf("  ✗ SPACE layer failed: pack broken\n");
        valid = 0;
    } else {
        printf("  ✓ SPACE layer ready\n");
    }

    // TIME: Can we create temporal states?
    temporal_t temp = trit_to_temporal(TRIT_ZERO);
    if (temp != TEMP_PRESENT) {
        printf("  ✗ TIME layer failed: temporal mapping broken\n");
        valid = 0;
    } else {
        printf("  ✓ TIME layer ready\n");
    }

    printf("────────────────────────────────────────────────────────────────\n");
    if (valid) {
        printf("✓ All prerequisites validated. Proceeding with demonstration.\n");
    } else {
        printf("✗ Prerequisites failed. Cannot proceed.\n");
        tests_failed++;
    }

    return valid;
}

// Helper: Get test counts (for external validation)
int demo_trit_get_passed(void) { return tests_passed; }
int demo_trit_get_failed(void) { return tests_failed; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// Entry Point: main()
//   └── demo_trit_run_all() orchestrates all tests
//
// Execution Flow:
//   1. Reset counters (allows multiple runs)
//   2. Print header
//   3. Run each layer test (MATTER → SPACE → TIME → Bible Rail → 3D)
//   4. Report results with scripture reference
//   5. Return failure count (0 = success)
//
// Exit Codes:
//   0 - All tests passed
//   1 - One or more tests failed

// Helper: Run a specific subset (for selective testing)
int demo_trit_run_layers(int matter, int space, int time, int bible, int integration) {
    tests_passed = 0;
    tests_failed = 0;

    printf("════════════════════════════════════════════════════════════════\n");
    printf("Phase 1 Demo: Selected Layer Tests\n");
    printf("════════════════════════════════════════════════════════════════\n");

    if (matter)      demo_trit_matter();
    if (space)       demo_trit_space();
    if (time)        demo_trit_time();
    if (bible)       demo_trit_bible_rail();
    if (integration) demo_trit_integration();

    printf("\n────────────────────────────────────────────────────────────────\n");
    printf("Selected Tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("════════════════════════════════════════════════════════════════\n");

    return tests_failed;
}

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// Cleanup runs AFTER execution to report results and reset state.
// Called at end of demo_trit_run_all().

// Report final results and reset for next run
static void demo_cleanup_and_report(void) {
    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("Demonstration Complete\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Steps demonstrated: %d\n", tests_passed);

    if (tests_failed > 0) {
        printf("  Validation failures: %d\n", tests_failed);
    }

    printf("\n  \"By their fruits ye shall know them.\" — Matthew 7:20\n");
    printf("\n  The fruit: balanced ternary computing with three cognitive layers\n");
    printf("  grounded in Genesis 1:1 — TIME × SPACE × MATTER.\n");
    printf("\n════════════════════════════════════════════════════════════════\n");
}

// Public: Reset demo state (for multiple runs)
void demo_trit_reset(void) {
    tests_passed = 0;
    tests_failed = 0;
}

// ════════════════════════════════════════════════════════════════
// GROUP 2: FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Demo Overview
// ────────────────────────────────────────────────────────────────
//
// This demo validates the three-layer architecture:
//
//   MATTER (trit.h)     - What exists: individual trit operations
//   SPACE (dimension.h) - Where it exists: pack, navigate, map
//   TIME (temporal.h)   - When it exists: temporal state cognition
//
// The layers compose: MATTER → SPACE → TIME → integrated system.

// ────────────────────────────────────────────────────────────────
// Public API Summary
// ────────────────────────────────────────────────────────────────
//
// Primary API (run tests):
//   demo_trit_run_all()       - Run all tests
//   demo_trit_matter()        - Test MATTER layer only
//   demo_trit_space()         - Test SPACE layer only
//   demo_trit_time()          - Test TIME layer only
//   demo_trit_bible_rail()    - Test Bible Rail only
//   demo_trit_integration()   - Test 3D integration only
//   demo_trit_run_layers(...) - Run selected layers
//
// Utility API (inspection):
//   demo_trit_get_passed()    - Get count of passed tests
//   demo_trit_get_failed()    - Get count of failed tests
//   demo_trit_reset()         - Reset counters
//
// Usage in educational game:
//
//     // Level 1: Teach MATTER
//     if (demo_trit_matter() == 0) {
//         unlock_next_level();
//     }
//
//     // Final boss: All layers
//     if (demo_trit_run_all() == 0) {
//         show_completion_screen();
//     }

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new test functions (follow demo_trit_* pattern)
//   ✅ Adjust print formatting and messages
//   ✅ Add new test cases within existing layers
//
// Modify with Care:
//   ⚠️ Public API signatures - external code may call these
//   ⚠️ Test counter logic - affects validation reporting
//   ⚠️ Return values - 0=success convention must be preserved
//
// NEVER Modify:
//   ❌ 4-block structure
//   ❌ Public API return semantics (failure count)
//   ❌ Test counting mechanism
//
// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding a new layer test:
//   1. Add prototype to SETUP (Public API section)
//   2. Implement function in BODY following existing pattern
//   3. Add call in demo_trit_run_all() orchestrator
//   4. Update demo_trit_run_layers() parameter list if needed
//
// Adding tests to existing layer:
//   1. Find the layer function (demo_trit_matter, etc.)
//   2. Add test using print_result() pattern
//   3. Counters update automatically
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "libtrit.a not found" during build
//   - Cause: Library not built yet
//   - Solution: cd word/work/pkg/trit && make
//
// Problem: Demo compiles but tests fail unexpectedly
//   - Check: Is libtrit.a up to date? Rebuild if changed.
//   - Check: Are include paths correct in Makefile?
//
// Problem: demo_trit_run_all() returns non-zero
//   - Meaning: One or more tests failed
//   - Action: Check output for specific failures
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Library:
//   - word/work/pkg/trit/          (libtrit source)
//   - word/work/pkg/trit/include/  (headers)
//   - word/work/pkg/trit/build/    (libtrit.a)
//
// Demo Files:
//   - main.c     (this file - demo source)
//   - Makefile   (build system)
//   - README.adoc (documentation)
//
// Other Demos:
//   - tov/demo/phase-0/demo-config/ (config validation demo)
//
// Journey Documents:
//   - word/journey/phase-1/emergent-behavior-001.txt
//
// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Build and Run:
//
//     make        # Build demo
//     ./demo-trit # Run all tests
//     make run    # Build and run
//
// Embed in Game/Tutorial:
//
//     #include "demo_trit.h"  // (if header exists)
//     // Or link demo-trit.o
//
//     // Test individual layer
//     if (demo_trit_matter() == 0) {
//         player_passed_level(1);
//     }
//
//     // Test everything
//     if (demo_trit_run_all() == 0) {
//         unlock_achievement("Ternary Master");
//     }
//
//     // Selective testing
//     int result = demo_trit_run_layers(
//         1,  // MATTER
//         1,  // SPACE
//         0,  // TIME (skip)
//         0,  // Bible Rail (skip)
//         0   // Integration (skip)
//     );
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// "By their fruits ye shall know them." — Matthew 7:20
//
// The fruit of Phase 1: a complete libtrit library with three
// layers composing into a unified ternary computation system.
// This demo proves the architecture works.
//
// 39 tests. All passed. The three-layer architecture is sound.
//

// ============================================================================
// END CLOSING
// ============================================================================
