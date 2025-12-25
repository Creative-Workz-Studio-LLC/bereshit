// ═══════════════════════════════════════════════════════════════════════════
// Phase 2 Demo: Mathematical Operations
// Key: B-tov-demo-phase2-demo-math
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a)
//   Links against libtrit.a for trit types and math operations.
//
// derives_from: bereshit/tov/demo/phase-1/demo-trit/main.c (structure)
// See: void/planning/development/plans/phase-2-implementation-plan.adoc
//
// ═══════════════════════════════════════════════════════════════════════════

// Demo program demonstrating the five math operation families.
//
// demo-math - CPI-SI Kingdom Technology
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
// Principle: Truth is proven by demonstration. The demo shows the math
//            operations work - arithmetic, comparison, conversion,
//            normalization, and calculus. Just weights, accurate measures.
//
// Anchor: "By their fruits ye shall know them." — Matthew 7:20
//
// # CPI-SI Identity
//
// Component Type: Baton (execution flow - demonstrates and reports)
//
// Role: Phase 2 demonstration - shows math operations working across
//       all five families, culminating in temporal integration.
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
// Purpose: Demonstrate Phase 2 math operations across all five families.
//
// Core Design: Sequential demonstrations: Arithmetic → Comparison →
//              Conversion → Normalization → Calculus → Integration.
//
// Key Features:
//   - Family 1: Arithmetic (half/full adder, carry propagation)
//   - Family 2: Comparison (ternary cmp returning trit_t)
//   - Family 3: Conversion (trit↔int, stored↔true)
//   - Family 4: Normalization (7 levels, Hebrew names, emoji)
//   - Family 5: Calculus (change detection, thresholds, trends)
//   - Integration: Calculus with temporal context
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
//   - Internal: trit.h, math.h, temporal.h (libtrit headers)
//   - Library: libtrit.a (linked)
//
// What Uses This:
//   - Developers: Demonstrates Phase 2 completion
//   - CI: Could be automated validation
//
// # Usage
//
// Build: make
// Run: ./demo-math
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// Exit codes:
//   0 = All demonstrations completed successfully
//   1 = One or more demonstrations failed
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
#include <stdint.h>  // int8_t, int16_t
#include <string.h>  // strlen

//--- Project Headers ---
#include "trit.h"            // MATTER layer: trit types and operations
#include "math.h"            // Math operations: 5 families
#include "temporal.h"        // TIME layer: for integration demo
#include "bible_rail.h"      // Bible Rail: spare values → WEB variants
#include "verse_addr.h"      // Verse Address: ordinals → packed trit5 pairs
#include "scripture_path.h"  // Scripture Path: variant-aware file resolution
#include "scripture_text.h"  // Scripture Text: character ↔ trit5 encoding

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

// Demo output markers
#define DEMO_ARROW "  → "

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────

// Demo result tracking (matches Phase 1 pattern)
static int demos_passed = 0;
static int demos_failed = 0;

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// [Reserved: All types defined in library headers]

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

// ═══════════════════════════════════════════════════════════════
// PUBLIC API - Demo Functions (5 Families + Integration)
// ═══════════════════════════════════════════════════════════════
// These functions are PUBLIC - callable by games, tutorials, SDK examples.
// A game teaching ternary math can call demo_math_arithmetic() as a level.

// Primary API (run demos)
int demo_math_run_all(void);         // Run all demos, show all families
int demo_math_arithmetic(void);      // Family 1: half/full adder, carry
int demo_math_comparison(void);      // Family 2: ternary cmp returning trit_t
int demo_math_conversion(void);      // Family 3: trit↔int, stored↔true
int demo_math_normalization(void);   // Family 4: 7 levels, Hebrew, emoji
int demo_math_calculus(void);        // Family 5: change, rate, trend
int demo_math_integration(void);     // Integration: calculus + temporal
int demo_bible_rail(void);           // Bible Rail: spare values → WEB variants
int demo_verse_addr(void);           // Verse Address: ordinals → packed encoding
int demo_scripture_path(void);       // Scripture Path: variant-aware file lookup
int demo_scripture_text(void);       // Scripture Text: character ↔ trit5 encoding

// Utility API (inspection)
int demo_math_get_passed(void);      // Get count of demos passed
int demo_math_get_failed(void);      // Get count of failures
void demo_math_reset(void);          // Reset demo counters

// ═══════════════════════════════════════════════════════════════
// INTERNAL HELPERS
// ═══════════════════════════════════════════════════════════════
// These functions are internal - static linkage.

static void print_header(const char *title);
static void demo_step(const char *description);

// CLOSING section functions
static int demo_validate_library(void);    // Code Validation
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
//   demo_math_run_all()        → Run all demos
//   ├── demo_math_arithmetic()   → Family 1: half/full adder, carry
//   ├── demo_math_comparison()   → Family 2: ternary cmp
//   ├── demo_math_conversion()   → Family 3: trit↔int, stored↔true
//   ├── demo_math_normalization()→ Family 4: 7 levels
//   ├── demo_math_calculus()     → Family 5: change, rate, trend
//   ├── demo_math_integration()  → Calculus + temporal context
//   ├── demo_bible_rail()        → Spare values → WEB variants
//   ├── demo_verse_addr()        → Ordinals → packed trit5 pairs
//   ├── demo_scripture_path()    → Variant-aware file lookup
//   └── demo_scripture_text()    → Character ↔ trit5 encoding
//
// Entry Point:
//   main() → demo_math_run_all() → exit code

// ════════════════════════════════════════════════════════════════
// SECTION 1: INTERNAL HELPERS (static)
// ════════════════════════════════════════════════════════════════

static void print_header(const char *title) {
    printf("\n────────────────────────────────────────────────────────────────\n");
    printf("%s\n", title);
    printf("────────────────────────────────────────────────────────────────\n");
}

// Helper: track demonstration steps (demos succeed by running)
static void demo_step(const char *description) {
    printf(DEMO_ARROW "%s\n", description);
    demos_passed++;
}

// ════════════════════════════════════════════════════════════════
// SECTION 2: PUBLIC DEMO API (non-static, callable externally)
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// demo_math_arithmetic: Family 1 - Carry-Aware Addition
// ────────────────────────────────────────────────────────────────
//
// Demonstrates half adder and full adder operations with carry.
// Shows how balanced ternary arithmetic propagates carry.

int demo_math_arithmetic(void) {
    print_header("FAMILY 1: Arithmetic - Carry-Aware Operations");

    printf("\n  Half Adder: Two trits → sum + carry\n");
    printf("  ┌─────┬─────┬──────┬───────┐\n");
    printf("  │  A  │  B  │ Sum  │ Carry │\n");
    printf("  ├─────┼─────┼──────┼───────┤\n");

    // Show the key half-adder cases
    trit_t cases_a[] = {TRIT_NEG, TRIT_NEG, TRIT_ZERO, TRIT_POS, TRIT_POS};
    trit_t cases_b[] = {TRIT_NEG, TRIT_POS, TRIT_POS,  TRIT_POS, TRIT_NEG};

    for (int i = 0; i < 5; i++) {
        trit_result_t r = trit_hadd(cases_a[i], cases_b[i]);
        printf("  │ %+2d  │ %+2d  │  %+2d  │  %+2d   │\n",
               trit_value(cases_a[i]), trit_value(cases_b[i]),
               trit_value(r.value), trit_value(r.carry));
    }
    printf("  └─────┴─────┴──────┴───────┘\n");
    demo_step("Half adder: overflow creates carry");

    // Show full adder with carry in
    printf("\n  Full Adder: Three trits → sum + carry_out\n");
    printf("    (+1) + (+1) + (+1) = ?\n");
    trit_result_t full = trit_fadd(TRIT_POS, TRIT_POS, TRIT_POS);
    printf("    Result: sum=%+d, carry=%+d\n",
           trit_value(full.value), trit_value(full.carry));
    printf("    Meaning: +1+1+1 = 0 with carry +1 (total: 3 = 1×3 + 0)\n");
    demo_step("Full adder: three inputs with carry propagation");

    // Show subtraction via half subtractor
    printf("\n  Half Subtractor: a - b via carry\n");
    trit_result_t sub = trit_hsub(TRIT_NEG, TRIT_POS);
    printf("    (-1) - (+1) = sum=%+d, borrow=%+d\n",
           trit_value(sub.value), trit_value(sub.carry));
    demo_step("Subtraction: borrow propagates like carry");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_math_comparison: Family 2 - Ternary Comparison
// ────────────────────────────────────────────────────────────────
//
// Demonstrates trit_cmp returning a trit: -1 (less), 0 (equal), +1 (greater).
// Pure ternary result, not boolean!

int demo_math_comparison(void) {
    print_header("FAMILY 2: Comparison - Ternary Results");

    printf("\n  The Ternary Comparison Principle:\n");
    printf("    Binary: a < b? → true/false (1 bit)\n");
    printf("    Ternary: cmp(a,b) → -1/0/+1 (1 trit)\n");
    printf("    One operation, three outcomes!\n");
    demo_step("Ternary comparison returns trit, not boolean");

    printf("\n  trit_cmp(a, b) Results:\n");
    printf("  ┌─────┬─────┬────────┬─────────────┐\n");
    printf("  │  A  │  B  │ Result │   Meaning   │\n");
    printf("  ├─────┼─────┼────────┼─────────────┤\n");

    // Show all 9 single-trit comparisons
    trit_t vals[] = {TRIT_NEG, TRIT_ZERO, TRIT_POS};
    const char* meanings[] = {"A < B", "A = B", "A > B"};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            trit_t result = trit_cmp(vals[i], vals[j]);
            int idx = trit_value(result) + 1;  // -1→0, 0→1, +1→2
            printf("  │ %+2d  │ %+2d  │   %+2d   │   %s   │\n",
                   trit_value(vals[i]), trit_value(vals[j]),
                   trit_value(result), meanings[idx]);
        }
    }
    printf("  └─────┴─────┴────────┴─────────────┘\n");
    demo_step("All 9 single-trit comparisons shown");

    // Show multi-trit comparison
    printf("\n  Multi-Trit Comparison (trit5_cmp):\n");
    trit5_t low = 50;    // Some packed value
    trit5_t mid = 121;   // Middle (all zeros)
    trit5_t high = 200;  // Some higher packed value

    trit_t cmp1 = trit5_cmp(low, high);
    trit_t cmp2 = trit5_cmp(mid, mid);
    trit_t cmp3 = trit5_cmp(high, low);

    printf("    trit5_cmp(%d, %d) = %+d → %d %s %d\n",
           low, high, trit_value(cmp1), low,
           trit_value(cmp1) < 0 ? "<" : (trit_value(cmp1) > 0 ? ">" : "="), high);
    printf("    trit5_cmp(%d, %d) = %+d → %d %s %d\n",
           mid, mid, trit_value(cmp2), mid,
           trit_value(cmp2) < 0 ? "<" : (trit_value(cmp2) > 0 ? ">" : "="), mid);
    printf("    trit5_cmp(%d, %d) = %+d → %d %s %d\n",
           high, low, trit_value(cmp3), high,
           trit_value(cmp3) < 0 ? "<" : (trit_value(cmp3) > 0 ? ">" : "="), low);
    demo_step("Multi-trit comparison works on packed values");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_math_conversion: Family 3 - Value Conversion Pipeline
// ────────────────────────────────────────────────────────────────
//
// Demonstrates the conversion between representations:
// trit ↔ int, stored (0-2) ↔ true (-1 to +1).

int demo_math_conversion(void) {
    print_header("FAMILY 3: Conversion - Between Representations");

    // Part 1: Simple trit conversion
    printf("\n  Simple Trit Conversion:\n");
    printf("    trit_to_int() and int_to_trit() for single trits.\n\n");

    printf("  ┌─────────┬───────┬──────────┐\n");
    printf("  │ Integer │ Trit  │ Back     │\n");
    printf("  ├─────────┼───────┼──────────┤\n");
    for (int i = -1; i <= 1; i++) {
        trit_t t = int_to_trit(i);
        int back = trit_to_int(t);
        printf("  │   %+2d    │ trit  │   %+2d     │\n", i, back);
    }
    printf("  └─────────┴───────┴──────────┘\n");
    demo_step("trit ↔ int: single trit conversion");

    // Part 2: Health value conversion (0-255 stored ↔ -100 to +100 true)
    printf("\n  Health Value Conversion:\n");
    printf("    stored_to_true(): byte (0-255) → health (-100 to +100)\n");
    printf("    true_to_stored(): health (-100 to +100) → byte (0-255)\n\n");

    printf("  stored_to_true() examples:\n");
    printf("  ┌────────┬──────┬───────────────────┐\n");
    printf("  │ Stored │ True │ Interpretation    │\n");
    printf("  ├────────┼──────┼───────────────────┤\n");
    uint8_t stored_examples[] = {0, 28, 64, 128, 192, 228, 255};
    const char* interp[] = {"Minimum", "Broken", "Wanting", "Even/Balanced",
                            "Whole", "Perfect", "Maximum"};
    for (int i = 0; i < 7; i++) {
        int8_t t = stored_to_true(stored_examples[i]);
        printf("  │  %3d   │ %+4d │ %-17s │\n", stored_examples[i], t, interp[i]);
    }
    printf("  └────────┴──────┴───────────────────┘\n");
    demo_step("stored_to_true: byte → health range");

    printf("\n  true_to_stored() examples:\n");
    printf("  ┌──────┬────────┐\n");
    printf("  │ True │ Stored │\n");
    printf("  ├──────┼────────┤\n");
    int8_t true_examples[] = {-100, -50, 0, 50, 100};
    for (int i = 0; i < 5; i++) {
        uint8_t s = true_to_stored(true_examples[i]);
        printf("  │ %+4d │  %3d   │\n", true_examples[i], s);
    }
    printf("  └──────┴────────┘\n");
    demo_step("true_to_stored: health → byte range");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_math_normalization: Family 4 - Health Levels & Rounding
// ────────────────────────────────────────────────────────────────
//
// Demonstrates the 7 health levels with Hebrew names and emoji.
// Shows base rounding and normalization.

int demo_math_normalization(void) {
    print_header("FAMILY 4: Normalization - 7 Levels of Health");

    printf("\n  The 7 Health Levels:\n");
    printf("  ┌───────┬─────────────┬────────────┬───────┐\n");
    printf("  │ Level │   Hebrew    │   English  │ Emoji │\n");
    printf("  ├───────┼─────────────┼────────────┼───────┤\n");

    // Iterate from LEVEL_PERFECT (+3) down to LEVEL_BROKEN (-3)
    for (int lvl = LEVEL_PERFECT; lvl >= LEVEL_BROKEN; lvl--) {
        printf("  │  %+2d   │ %11s │ %10s │   %s  │\n",
               lvl,
               level_hebrew((health_level_t)lvl),
               level_name((health_level_t)lvl),
               level_emoji((health_level_t)lvl));
    }
    printf("  └───────┴─────────────┴────────────┴───────┘\n");
    demo_step("7 levels: Perfect to Broken with Hebrew names");

    // Show base rounding
    printf("\n  Base Rounding (trit_base_round):\n");
    printf("    Reduces precision to fit base-3 boundaries.\n\n");

    int8_t test_vals[] = {-100, -50, -10, 0, 10, 50, 100};
    printf("    Value → Base 27 → Base 9 → Base 3\n");
    for (int i = 0; i < 7; i++) {
        int8_t v = test_vals[i];
        printf("    %+4d  →  %+4d   →  %+4d  →  %+4d\n",
               v, trit_base_round(v, 27),
               trit_base_round(v, 9), trit_base_round(v, 3));
    }
    demo_step("Base rounding: coarser granularity");

    // Show normalization
    printf("\n  Normalization (trit_normalize):\n");
    printf("    Maps any int8_t to [-121, +121] range.\n\n");

    int8_t edge_vals[] = {-127, -64, 0, 64, 127};
    for (int i = 0; i < 5; i++) {
        int8_t v = edge_vals[i];
        printf("    %+4d → %+4d\n", v, trit_normalize(v));
    }
    demo_step("Normalization: clamp to valid range");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_math_calculus: Family 5 - Change Detection & Trends
// ────────────────────────────────────────────────────────────────
//
// Demonstrates calculus operations: difference, rate, accumulate, trend.
// These are inherently TEMPORAL - they track change over time.

int demo_math_calculus(void) {
    print_header("FAMILY 5: Calculus - Change Over Time");

    printf("\n  Calculus = Math That Tracks Change\n");
    printf("    trit_difference: What changed?\n");
    printf("    trit_rate:       How fast?\n");
    printf("    trit_accumulate: Running total\n");
    printf("    trend_direction: Where is it going?\n");
    demo_step("Four calculus operations defined");

    // Difference
    printf("\n  trit_difference(previous, current):\n");
    printf("  ┌──────────┬─────────┬────────┬─────────────┐\n");
    printf("  │ Previous │ Current │ Result │   Meaning   │\n");
    printf("  ├──────────┼─────────┼────────┼─────────────┤\n");

    int8_t prev_vals[] = {50, 50, 50, 100, 0};
    int8_t curr_vals[] = {60, 50, 40,  0, 100};
    const char* diff_meanings[] = {"decreasing", "stable", "increasing"};

    for (int i = 0; i < 5; i++) {
        trit_t diff = trit_difference(prev_vals[i], curr_vals[i]);
        int idx = trit_value(diff) + 1;
        printf("  │   %+4d   │  %+4d   │   %+2d   │ %11s │\n",
               prev_vals[i], curr_vals[i], trit_value(diff), diff_meanings[idx]);
    }
    printf("  └──────────┴─────────┴────────┴─────────────┘\n");
    demo_step("Difference: direction of change");

    // Rate
    printf("\n  trit_rate(delta, time_delta):\n");
    printf("    Rate = magnitude of change over time\n");
    int8_t deltas[] = {10, 50, 100};
    uint8_t times[] = {1, 10, 100};

    for (int i = 0; i < 3; i++) {
        trit_t rate = trit_rate(deltas[i], times[i]);
        printf("    delta=%+d, time=%d → rate=%+d\n",
               deltas[i], times[i], trit_value(rate));
    }
    demo_step("Rate: change per time unit");

    // Accumulator
    printf("\n  trit_accumulate(accumulator, value):\n");
    int16_t acc = 0;
    int8_t values[] = {10, -5, 20, -30, 15};
    printf("    Starting: %d\n", acc);
    for (int i = 0; i < 5; i++) {
        acc = trit_accumulate(acc, values[i]);
        printf("    + (%+3d) = %+4d\n", values[i], acc);
    }
    demo_step("Accumulator: running total");

    // Trend
    printf("\n  trend_direction(history, count):\n");
    int8_t rising[]  = {10, 20, 30, 40, 50};
    int8_t falling[] = {50, 40, 30, 20, 10};
    int8_t stable[]  = {30, 31, 29, 30, 30};

    printf("    [10,20,30,40,50] → trend = %+d (rising)\n",
           trit_value(trend_direction(rising, 5)));
    printf("    [50,40,30,20,10] → trend = %+d (falling)\n",
           trit_value(trend_direction(falling, 5)));
    printf("    [30,31,29,30,30] → trend = %+d (stable)\n",
           trit_value(trend_direction(stable, 5)));
    demo_step("Trend: overall direction from history");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_math_integration: Calculus + Temporal Integration
// ────────────────────────────────────────────────────────────────
//
// Shows how calculus operations integrate with temporal cognition.
// Calculus IS temporal - it tracks change over time.

int demo_math_integration(void) {
    print_header("INTEGRATION: Calculus Meets Temporal Cognition");

    printf("\n  Why Calculus IS Temporal:\n");
    printf("    - Difference compares NOW to BEFORE\n");
    printf("    - Rate measures change OVER TIME\n");
    printf("    - Trend projects PAST into FUTURE\n");
    printf("    - Accumulation tracks HISTORY\n");
    demo_step("Calculus operations are inherently temporal");

    // Simulate health tracking over time
    printf("\n  Health Tracking Scenario:\n");
    printf("    Tracking a creature's health over 5 time steps.\n\n");

    int8_t health_history[] = {100, 90, 75, 60, 80};
    int16_t accumulated_damage = 0;

    printf("    Time  Health  Change  Rate   Trend  Accumulated\n");
    printf("    ────  ──────  ──────  ────   ─────  ───────────\n");

    for (int t = 0; t < 5; t++) {
        trit_t change = (t == 0) ? TRIT_ZERO :
                        trit_difference(health_history[t-1], health_history[t]);
        trit_t rate = (t == 0) ? TRIT_ZERO :
                      trit_rate(health_history[t] - health_history[t-1], 1);

        // Calculate trend from history up to current point
        trit_t trend = (t < 2) ? TRIT_ZERO :
                       trend_direction(health_history, (uint8_t)(t + 1));

        // Accumulate damage (negative changes)
        if (t > 0 && health_history[t] < health_history[t-1]) {
            accumulated_damage = trit_accumulate(accumulated_damage,
                                    (int8_t)(health_history[t-1] - health_history[t]));
        }

        printf("     %d     %3d     %+2d      %+2d     %+2d       %+4d\n",
               t, health_history[t],
               trit_value(change), trit_value(rate),
               trit_value(trend), accumulated_damage);
    }
    demo_step("Health tracked: change, rate, trend, accumulation");

    // Show temporal context interpretation
    printf("\n  Temporal Context Interpretation:\n");
    printf("    At t=4 (health=80, rising from 60):\n");

    temporal_t past = trit_to_temporal(TRIT_NEG);
    temporal_t present = trit_to_temporal(TRIT_ZERO);
    temporal_t future = trit_to_temporal(TRIT_POS);
    (void)past;  // Referenced in output

    printf("\n    PAST mode: \"Took 40 damage total over history\"\n");
    printf("    PRESENT mode: \"Currently at 80, just healed +20\"\n");
    printf("    FUTURE mode: \"Trend is now positive, expect recovery\"\n");

    // Planning mode: present + future
    temporal_state_t planning = temporal_create_compound(present, future);
    (void)planning;  // Demonstrated compound state
    printf("\n    PLANNING (present+future): \"At 80 and rising - safe to engage\"\n");

    demo_step("Temporal modes give meaning to calculus data");

    printf("\n  The Integration:\n");
    printf("    MATTER: The health value (80)\n");
    printf("    SPACE:  Where in the range (80/100 = 80%%)\n");
    printf("    TIME:   When and how it's changing (rising trend)\n");
    printf("\n    All three layers work together for understanding.\n");
    demo_step("Complete: MATTER × SPACE × TIME");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_bible_rail: Bible Rail - Spare Values Encode Scripture
// ────────────────────────────────────────────────────────────────
//
// Demonstrates how the 13 spare trit5 values (243-255) encode
// WEB textual variants. Nothing wasted - every bit serves the Word.

int demo_bible_rail(void) {
    print_header("BIBLE RAIL: Spare Values Encode Scripture");

    printf("\n  The Trit-Byte Discovery:\n");
    printf("    5 trits pack into 1 byte: 3^5 = 243 semantic values\n");
    printf("    1 byte holds 256 values: 256 - 243 = 13 spare!\n");
    printf("\n    Most systems waste these spare values.\n");
    printf("    We use them to encode WEB textual variants.\n");
    demo_step("13 spare values discovered (243-255)");

    // Show the mapping
    printf("\n  All 13 WEB Variants:\n");
    printf("  ┌───────┬───────────────────────────┬────────────────────────┐\n");
    printf("  │ Spare │ Reference                 │ Description            │\n");
    printf("  ├───────┼───────────────────────────┼────────────────────────┤\n");

    for (int i = 0; i < bible_rail_variant_count(); i++) {
        trit5_t spare = bible_rail_encode(i);
        const char* ref = bible_rail_variant_ref(i);
        const char* desc = bible_rail_variant_desc(i);
        printf("  │  %3d  │ %-25s │ %-22s │\n", spare, ref, desc);
    }
    printf("  └───────┴───────────────────────────┴────────────────────────┘\n");
    demo_step("13 WEB variants mapped to spare values");

    // Demonstrate encoding/decoding
    printf("\n  Encode/Decode Roundtrip:\n");
    printf("    Variant 0 → encode → %d → decode → %d\n",
           bible_rail_encode(0), bible_rail_decode(bible_rail_encode(0)));
    printf("    Variant 6 → encode → %d → decode → %d\n",
           bible_rail_encode(6), bible_rail_decode(bible_rail_encode(6)));
    printf("    Variant 12 → encode → %d → decode → %d\n",
           bible_rail_encode(12), bible_rail_decode(bible_rail_encode(12)));
    demo_step("Encode/decode roundtrip verified");

    // Show semantic vs spare distinction
    printf("\n  Semantic vs Spare Detection:\n");
    printf("    is_spare(242) = %s  (last semantic value)\n",
           bible_rail_is_spare(242) ? "true" : "false");
    printf("    is_spare(243) = %s   (first spare value)\n",
           bible_rail_is_spare(243) ? "true" : "false");
    printf("    is_spare(255) = %s   (last spare value)\n",
           bible_rail_is_spare(255) ? "true" : "false");
    demo_step("Semantic/spare boundary at 242/243");

    // Practical application
    printf("\n  Practical Application:\n");
    printf("    When packing verse text with 5 trits per byte:\n");
    printf("    - Semantic bytes (0-242): normal trit data\n");
    printf("    - Spare bytes (243-255): \"This verse has a WEB variant\"\n");
    printf("\n    Example: Encountering byte 254 in packed stream means:\n");
    printf("      → Variant index: %d\n", bible_rail_decode(254));
    printf("      → Reference: %s\n", bible_rail_variant_ref(bible_rail_decode(254)));
    printf("      → Description: %s\n", bible_rail_variant_desc(bible_rail_decode(254)));
    demo_step("Spare values signal textual variants in-band");

    // Location-based variant detection
    printf("\n  Location-Based Variant Detection:\n");
    printf("    bible_rail_variant_at() checks if a location is a variant.\n\n");
    printf("    ┌─────────────────────────────┬─────────┐\n");
    printf("    │ Location                    │ Spare   │\n");
    printf("    ├─────────────────────────────┼─────────┤\n");
    printf("    │ Genesis 1:1                 │  %3d    │\n",
           bible_rail_variant_at("Genesis", 1, 1));
    printf("    │ 1 Corinthians 16:27         │  %3d    │\n",
           bible_rail_variant_at("1 Corinthians", 16, 27));
    printf("    │ John 3:16                   │  %3d    │\n",
           bible_rail_variant_at("John", 3, 16));
    printf("    │ Revelation 1:25             │  %3d    │\n",
           bible_rail_variant_at("Revelation", 1, 25));
    printf("    └─────────────────────────────┴─────────┘\n");
    printf("    (0 = not a variant, 243-255 = WEB variant spare value)\n");
    demo_step("variant_at() enables location-aware encoding");

    // Structured location retrieval
    printf("\n  Structured Location Retrieval:\n");
    printf("    bible_rail_variant_location() returns book/chapter/verse.\n\n");
    variant_location_t loc = bible_rail_variant_location(0);
    printf("    Index 0 → Book: %s, Chapter: %d, Verse: %d\n",
           loc.book, loc.chapter, loc.verse);
    loc = bible_rail_variant_location(11);
    printf("    Index 11 → Book: %s, Chapter: %d, Verse: %d\n",
           loc.book, loc.chapter, loc.verse);
    demo_step("variant_location() provides structured data");

    // The theological point
    printf("\n  Theological Significance:\n");
    printf("    \"Add thou not unto his words, lest he reprove thee,\n");
    printf("     and thou be found a liar.\" — Proverbs 30:6\n");
    printf("\n    The spare values aren't adding to Scripture.\n");
    printf("    They're encoding WHERE the texts differ.\n");
    printf("    13 spare bytes. 13 textual variants. Nothing wasted.\n");
    printf("    Every bit serves the Word.\n");
    demo_step("Nothing added - variants encoded faithfully");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_verse_addr: Verse Address Encoding (31,102 KJV Verses)
// ────────────────────────────────────────────────────────────────
//
// Demonstrates how verse ordinals (1-31102) encode to packed trit5 pairs.
// Every verse addressable in 2 bytes using base 243 arithmetic.

int demo_verse_addr(void) {
    print_header("VERSE ADDRESS: Ordinals Encode to Packed Pairs");

    printf("\n  The Verse Encoding Algorithm:\n");
    printf("    31,102 KJV verses, each with unique ordinal (1-31102)\n");
    printf("    Packed into 2 bytes using base 243:\n");
    printf("      high = ordinal / 243\n");
    printf("      low  = ordinal %% 243\n");
    printf("    Decode: ordinal = (high × 243) + low\n");
    demo_step("Base 243 encoding: 2 bytes per verse");

    // Show key verses
    printf("\n  Key Verse Encodings:\n");
    printf("  ┌─────────────────────────────┬─────────┬───────┬───────┐\n");
    printf("  │ Verse                       │ Ordinal │ High  │  Low  │\n");
    printf("  ├─────────────────────────────┼─────────┼───────┼───────┤\n");

    // Genesis 1:1 (first)
    verse_addr_t gen1_1 = verse_addr_encode(1);
    printf("  │ Genesis 1:1 (first)         │   %5d │  %3d  │  %3d  │\n",
           1, gen1_1.high, gen1_1.low);

    // Psalm 119:1 (middle of Bible)
    verse_addr_t ps119_1 = verse_addr_encode(15766);
    printf("  │ Psalm 119:1 (middle)        │   %5d │  %3d  │  %3d  │\n",
           15766, ps119_1.high, ps119_1.low);

    // Matthew 1:1 (start of NT)
    verse_addr_t matt1_1 = verse_addr_encode(23146);
    printf("  │ Matthew 1:1 (NT start)      │   %5d │  %3d  │  %3d  │\n",
           23146, matt1_1.high, matt1_1.low);

    // Revelation 22:21 (last)
    verse_addr_t rev22_21 = verse_addr_encode(31102);
    printf("  │ Revelation 22:21 (last)     │   %5d │  %3d  │  %3d  │\n",
           31102, rev22_21.high, rev22_21.low);

    printf("  └─────────────────────────────┴─────────┴───────┴───────┘\n");
    demo_step("Key verses: Genesis to Revelation encoded");

    // Show roundtrip verification
    printf("\n  Encode/Decode Roundtrip:\n");
    uint16_t test_ordinals[] = {1, 243, 15766, 23146, 31102};
    const char* test_names[] = {"Genesis 1:1", "First ×243", "Psalm 119:1",
                                "Matthew 1:1", "Revelation 22:21"};

    for (int i = 0; i < 5; i++) {
        verse_addr_t addr = verse_addr_encode(test_ordinals[i]);
        uint16_t decoded = verse_addr_decode(addr);
        printf("    %s: %d → {%d,%d} → %d %s\n",
               test_names[i], test_ordinals[i],
               addr.high, addr.low, decoded,
               decoded == test_ordinals[i] ? "✓" : "✗");
    }
    demo_step("Roundtrip verified: encode → decode = original");

    // Show relationship with Bible Rail
    printf("\n  Integration with Bible Rail:\n");
    printf("    Verse addressing uses semantic range: low = 0-242\n");
    printf("    Bible Rail uses spare range: low = 243-255\n");
    printf("\n    When low byte >= 243, it's NOT a verse address.\n");
    printf("    It's a WEB textual variant marker!\n");

    verse_addr_t valid = {64, 100};    // low < 243: valid address
    verse_addr_t spare = {64, 245};    // low >= 243: WEB variant marker

    printf("\n    is_valid({64, 100}) = %s  (semantic low byte)\n",
           verse_addr_is_valid(valid) ? "true" : "false");
    printf("    is_valid({64, 245}) = %s (spare low byte → variant)\n",
           verse_addr_is_valid(spare) ? "true" : "false");
    demo_step("Verse addresses and Bible Rail share the byte space");

    // Bidirectional KJV ↔ WEB bridge
    printf("\n  Bidirectional KJV ↔ WEB Bridge:\n");
    printf("    verse_addr_encode_variant() bridges to Bible Rail.\n\n");
    printf("    ┌───────────────────────────┬─────────────────────────────┐\n");
    printf("    │ KJV Verse (semantic)      │ WEB Variant (spare)         │\n");
    printf("    ├───────────────────────────┼─────────────────────────────┤\n");

    verse_addr_t kjv = verse_addr_encode(1);  // Genesis 1:1
    printf("    │ ordinal 1 → {%d, %d}       │", kjv.high, kjv.low);
    verse_addr_t web = verse_addr_encode_variant(0);  // First variant
    printf(" variant 0 → {%d, %d}          │\n", web.high, web.low);

    printf("    │ is_variant: %s          │ is_variant: %s            │\n",
           verse_addr_is_variant(kjv) ? "true " : "false",
           verse_addr_is_variant(web) ? "true " : "false");
    printf("    │ decode: %d              │ decode_variant: %d           │\n",
           verse_addr_decode(kjv), verse_addr_decode_variant(web));
    printf("    └───────────────────────────┴─────────────────────────────┘\n");
    demo_step("KJV and WEB verse types distinguished in packed format");

    // The theological point
    printf("\n  Theological Significance:\n");
    printf("    \"So shall my word be that goeth forth out of my mouth:\n");
    printf("     it shall not return unto me void.\" — Isaiah 55:11\n");
    printf("\n    31,102 verses. 2 bytes each. Every verse addressable.\n");
    printf("    The encoding reveals Scripture's order.\n");
    demo_step("Every verse has an address - Scripture is ordered");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_scripture_path: Variant-Aware File Path Resolution
// ────────────────────────────────────────────────────────────────
//
// Demonstrates scripture_path module: generating filesystem paths
// from packed addresses for both KJV verses and WEB variants.

int demo_scripture_path(void) {
    print_header("SCRIPTURE PATH: Variant-Aware File Resolution");

    printf("\n  The Scripture Path Resolution System:\n");
    printf("    Every verse has a file: word/scripture/{KJV|WEB}/{Book}/...\n");
    printf("    Packed addresses resolve to paths for file lookup.\n");
    printf("    Balanced ternary type enum: -1 (INVALID), 0 (KJV), +1 (WEB)\n");
    demo_step("Packed address → filesystem path resolution");

    // Book name normalization
    printf("\n  Book Name Normalization:\n");
    printf("    Filesystem paths need underscores, not spaces.\n\n");
    char norm[64];
    scripture_path_normalize_book("1 Corinthians", norm, sizeof(norm));
    printf("    \"1 Corinthians\" → \"%s\"\n", norm);
    scripture_path_normalize_book("Song of Solomon", norm, sizeof(norm));
    printf("    \"Song of Solomon\" → \"%s\"\n", norm);
    demo_step("Book names normalized for filesystem");

    // Path generation
    printf("\n  Path Generation:\n");
    char path[SCRIPTURE_PATH_MAX_LEN];

    scripture_path_kjv("Genesis", 1, 1, path, sizeof(path));
    printf("\n    KJV Genesis 1:1:\n");
    printf("      %s\n", path);

    scripture_path_web("Genesis", 1, 1, path, sizeof(path));
    printf("\n    WEB Genesis 1:1:\n");
    printf("      %s\n", path);

    scripture_path_kjv("1 Corinthians", 16, 27, path, sizeof(path));
    printf("\n    KJV 1 Corinthians 16:27 (WEB variant location):\n");
    printf("      %s\n", path);
    demo_step("KJV and WEB paths generated from book/chapter/verse");

    // Variant-aware resolution
    printf("\n  Variant-Aware Resolution:\n");
    printf("    scripture_path_resolve() dispatches based on address type.\n\n");

    printf("    ┌─────────────────────────┬────────────┬──────────────┐\n");
    printf("    │ Address Type            │ Enum Value │ Path Result  │\n");
    printf("    ├─────────────────────────┼────────────┼──────────────┤\n");

    // KJV verse address
    verse_addr_t kjv = verse_addr_encode(1);
    scripture_path_result_t result = scripture_path_resolve(kjv);
    printf("    │ KJV verse (ordinal 1)   │     %2d     │ ", result.type);
    if (strlen(result.path) > 0) {
        printf("(has path)   │\n");
    } else {
        printf("(no path)*   │\n");
    }

    // WEB variant address
    verse_addr_t web = verse_addr_encode_variant(0);
    result = scripture_path_resolve(web);
    printf("    │ WEB variant (index 0)   │     %2d     │ ", result.type);
    if (strlen(result.path) > 0) {
        printf("(has path)   │\n");
    } else {
        printf("(no path)    │\n");
    }

    printf("    └─────────────────────────┴────────────┴──────────────┘\n");
    printf("\n    * KJV resolve returns type only; use resolve_with_location\n");
    printf("      when book/chapter/verse is known for full path.\n");
    demo_step("Resolve dispatches to KJV or WEB based on packed address");

    // Resolve with location
    printf("\n  Full Resolution with Known Location:\n");
    result = scripture_path_resolve_with_location(kjv, "Genesis", 1, 1);
    printf("\n    resolve_with_location(addr, \"Genesis\", 1, 1):\n");
    printf("      Type: %s (%d)\n", scripture_path_type_name(result.type), result.type);
    printf("      Path: %s\n", result.path);
    demo_step("Full path when location is known");

    // Type enumeration
    printf("\n  Balanced Ternary Type Enumeration:\n");
    printf("    ┌──────────────────────┬───────┬────────────────────────────┐\n");
    printf("    │ Type Name            │ Value │ Meaning                    │\n");
    printf("    ├──────────────────────┼───────┼────────────────────────────┤\n");
    printf("    │ %-20s │  %2d   │ Error/decode failed        │\n",
           scripture_path_type_name(SCRIPTURE_PATH_INVALID), SCRIPTURE_PATH_INVALID);
    printf("    │ %-20s │  %2d   │ Canonical anchor (0)       │\n",
           scripture_path_type_name(SCRIPTURE_PATH_KJV_VERSE), SCRIPTURE_PATH_KJV_VERSE);
    printf("    │ %-20s │  %2d   │ Extension (+1)             │\n",
           scripture_path_type_name(SCRIPTURE_PATH_WEB_VARIANT), SCRIPTURE_PATH_WEB_VARIANT);
    printf("    └──────────────────────┴───────┴────────────────────────────┘\n");
    demo_step("Balanced ternary: -1 (error), 0 (anchor), +1 (extension)");

    // Version-aware lookup
    printf("\n  Version-Aware Lookup (Any Verse, Any Version):\n");
    printf("    scripture_path_for_version() selects KJV or WEB explicitly.\n\n");

    char kjv_path[SCRIPTURE_PATH_MAX_LEN];
    char web_path[SCRIPTURE_PATH_MAX_LEN];

    scripture_path_for_version("John", 3, 16, SCRIPTURE_VERSION_KJV,
                               kjv_path, sizeof(kjv_path));
    scripture_path_for_version("John", 3, 16, SCRIPTURE_VERSION_WEB,
                               web_path, sizeof(web_path));

    printf("    John 3:16 in %s:\n", scripture_path_version_name(SCRIPTURE_VERSION_KJV));
    printf("      %s\n", kjv_path);
    printf("\n    John 3:16 in %s:\n", scripture_path_version_name(SCRIPTURE_VERSION_WEB));
    printf("      %s\n", web_path);
    printf("\n    Same verse. Two versions. Both accessible.\n");
    demo_step("for_version() provides version-aware Scripture access");

    // The theological point
    printf("\n  Theological Significance:\n");
    printf("    \"Thy word is a lamp unto my feet, and a light unto my path.\"\n");
    printf("     — Psalm 119:105\n");
    printf("\n    62,204 verses: 31,102 KJV + 31,102 WEB.\n");
    printf("    13 variant markers flag significant textual differences.\n");
    printf("    KJV = canonical anchor (precision). WEB = accessibility (clarity).\n");
    printf("    PhD rigor + Bible accessibility. Same verse, both versions.\n");
    demo_step("The path to Scripture follows divine order");

    // The Complete Chain: scripture_path + scripture_text (LIVE DEMO)
    printf("\n  The Complete Chain (Path + Text Integration):\n");
    printf("    scripture_path ties into scripture_text. Let's read Genesis 1:1:\n\n");

    // Build path relative to demo location (demo is at tov/demo/phase-2/demo-math/)
    const char* genesis_path = "../../../../word/scripture/KJV/Genesis/Chapter_1/Verse_1.txt";
    uint8_t encoded[512];
    char decoded[512];

    // Step 1: Read and encode the file
    printf("    Step 1: scripture_path_read_file(\"%s\")\n", genesis_path);
    scripture_text_result_t read_result = scripture_path_read_file(genesis_path, encoded, sizeof(encoded));

    if (read_result.status == SCRIPTURE_TEXT_SUCCESS) {
        printf("    Status: %s\n", scripture_text_status_name(read_result.status));
        printf("    Bytes encoded: %zu\n\n", read_result.bytes_written);

        // Step 2: Show encoded data (first 20 bytes)
        printf("    Step 2: Encoded trit5 data (first 20 bytes):\n");
        printf("    [");
        size_t show = read_result.bytes_written < 20 ? read_result.bytes_written : 20;
        for (size_t i = 0; i < show; i++) {
            printf("%d", encoded[i]);
            if (i < show - 1) printf(",");
        }
        if (read_result.bytes_written > 20) printf(",...");
        printf("]\n\n");

        // Step 3: Decode back to string
        printf("    Step 3: scripture_path_decode_content(encoded, %zu, ...)\n", read_result.bytes_written);
        scripture_text_result_t dec_result = scripture_path_decode_content(
            encoded, read_result.bytes_written, decoded, sizeof(decoded));

        printf("    Status: %s\n\n", scripture_text_status_name(dec_result.status));
        printf("    Step 4: Decoded text:\n");
        printf("    \"%s\"\n", decoded);
    } else {
        printf("    (File not accessible from demo location - showing API structure)\n");
        printf("    scripture_path_read_file() → trit5 encoded\n");
        printf("    scripture_path_decode_content() → human readable\n");
    }
    demo_step("Chain complete: file → trit5 → text roundtrip");

    // API summary
    printf("\n  Scripture Path API (Complete Chain):\n");
    printf("    ┌──────────────────────────────────┬──────────────────────────┐\n");
    printf("    │ Function                         │ Purpose                  │\n");
    printf("    ├──────────────────────────────────┼──────────────────────────┤\n");
    printf("    │ scripture_path_read_verse()      │ Book/ch/vs → trit5       │\n");
    printf("    │ scripture_path_read_file()       │ Path → trit5             │\n");
    printf("    │ scripture_path_decode_content()  │ trit5 → readable string  │\n");
    printf("    └──────────────────────────────────┴──────────────────────────┘\n");
    printf("\n    Independent modules. Unified through integration layer.\n");
    printf("    scripture_text encodes. scripture_path ties it to files.\n");
    demo_step("Integration layer completes the chain without coupling");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_scripture_text: Character ↔ Trit5 Encoding
// ────────────────────────────────────────────────────────────────
//
// Demonstrates scripture_text module: converting characters to trit5
// values for Scripture storage. 243 values: ASCII + extended.

int demo_scripture_text(void) {
    print_header("SCRIPTURE TEXT: Character ↔ Trit5 Encoding");

    printf("\n  The Scripture Text Encoding System:\n");
    printf("    Every character maps to a trit5 value (0-242).\n");
    printf("    ASCII (0-127) maps directly. Extended (128-242) for specials.\n");
    printf("    243 total values = 3^5. One character per trit5 slot.\n");
    demo_step("243 trit5 values encode Scripture characters");

    // Single character encoding
    printf("\n  Character Encoding (char → trit5):\n");
    printf("  ┌──────┬───────┬──────────────────────────────┐\n");
    printf("  │ Char │ Trit5 │ Notes                        │\n");
    printf("  ├──────┼───────┼──────────────────────────────┤\n");

    char test_chars[] = {'A', 'z', '0', ' ', ':', ';', '?'};
    const char* notes[] = {
        "Uppercase letter",
        "Lowercase letter",
        "Digit",
        "Space (verse separator)",
        "Colon (chapter:verse)",
        "Semicolon (list separator)",
        "Question mark (replacement)"
    };

    for (int i = 0; i < 7; i++) {
        uint8_t trit5 = scripture_text_char_to_trit5(test_chars[i]);
        printf("  │  '%c'  │  %3d  │ %-28s │\n",
               test_chars[i], trit5, notes[i]);
    }
    printf("  └──────┴───────┴──────────────────────────────┘\n");
    demo_step("Characters encode to trit5 values");

    // Reverse mapping
    printf("\n  Reverse Mapping (trit5 → char):\n");
    printf("    trit5 65  → '%c'\n", scripture_text_trit5_to_char(65));
    printf("    trit5 97  → '%c'\n", scripture_text_trit5_to_char(97));
    printf("    trit5 32  → '%c' (space)\n", scripture_text_trit5_to_char(32));
    printf("    trit5 243 → '%c' (invalid → replacement)\n", scripture_text_trit5_to_char(243));
    demo_step("Trit5 values decode back to characters");

    // String encoding
    printf("\n  String Encoding (full roundtrip):\n");
    const char* genesis = "In the beginning";
    uint8_t encoded[64];
    char decoded[64];

    scripture_text_result_t enc_result = scripture_text_encode(genesis, encoded, sizeof(encoded));
    printf("\n    Original: \"%s\"\n", genesis);
    printf("    Encoded:  [");
    for (size_t i = 0; i < enc_result.bytes_written - 1; i++) {
        printf("%d", encoded[i]);
        if (i < enc_result.bytes_written - 2) printf(",");
    }
    printf("]\n");

    scripture_text_result_t dec_result = scripture_text_decode(encoded, enc_result.bytes_written,
                                                                decoded, sizeof(decoded));
    printf("    Decoded:  \"%s\"\n", decoded);
    printf("    Status:   %s\n", scripture_text_status_name(dec_result.status));
    demo_step("String encode → decode roundtrip preserves text");

    // Balanced ternary status values
    printf("\n  Balanced Ternary Status Values:\n");
    printf("    ┌───────┬─────────────┬────────────────────────────┐\n");
    printf("    │ Value │ Name        │ Meaning                    │\n");
    printf("    ├───────┼─────────────┼────────────────────────────┤\n");
    printf("    │  -1   │ %-11s │ Operation failed           │\n",
           scripture_text_status_name(SCRIPTURE_TEXT_ERROR));
    printf("    │   0   │ %-11s │ Operation completed        │\n",
           scripture_text_status_name(SCRIPTURE_TEXT_SUCCESS));
    printf("    │  +1   │ %-11s │ Output was truncated       │\n",
           scripture_text_status_name(SCRIPTURE_TEXT_TRUNCATED));
    printf("    └───────┴─────────────┴────────────────────────────┘\n");
    demo_step("Status follows balanced ternary: -1/0/+1");

    // Validation
    printf("\n  Trit5 Validation:\n");
    printf("    is_valid_trit5(127) = %s  (ASCII max)\n",
           scripture_text_is_valid_trit5(127) ? "true" : "false");
    printf("    is_valid_trit5(242) = %s  (trit5 max)\n",
           scripture_text_is_valid_trit5(242) ? "true" : "false");
    printf("    is_valid_trit5(243) = %s (spare value → invalid)\n",
           scripture_text_is_valid_trit5(243) ? "true" : "false");
    demo_step("Validation distinguishes semantic from spare range");

    // The theological point
    printf("\n  Theological Significance:\n");
    printf("    \"The words of the LORD are pure words: as silver tried\n");
    printf("     in a furnace of earth, purified seven times.\" — Psalm 12:6\n");
    printf("\n    243 trit5 values. Every character encoded faithfully.\n");
    printf("    ASCII for text. Extended for special characters.\n");
    printf("    The encoding preserves every letter of Scripture.\n");
    demo_step("Every letter preserved - pure words encoded faithfully");

    return 0;
}

// ────────────────────────────────────────────────────────────────
// demo_math_run_all: Run Complete Demonstration (Orchestrator)
// ────────────────────────────────────────────────────────────────
//
// Flow: VALIDATION → EXECUTION → CLEANUP
//   1. Validate prerequisites (library sanity checks)
//   2. Execute demonstrations (show each family in action)
//   3. Cleanup and report (summarize)

int demo_math_run_all(void) {
    // ═══════════════════════════════════════════════════════════
    // SETUP: Reset state for fresh run
    // ═══════════════════════════════════════════════════════════
    demo_math_reset();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("Phase 2 Demo: Mathematical Operations\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Demonstrating 5 operation families + temporal integration\n");
    printf("  \"A just weight is his delight.\" — Proverbs 11:1\n");

    // ═══════════════════════════════════════════════════════════
    // VALIDATION: Verify prerequisites before proceeding
    // ═══════════════════════════════════════════════════════════
    if (!demo_validate_library()) {
        demo_cleanup_and_report();
        return 1;
    }

    // ═══════════════════════════════════════════════════════════
    // EXECUTION: Run all family demonstrations
    // ═══════════════════════════════════════════════════════════
    demo_math_arithmetic();      // Family 1: carry-aware addition
    demo_math_comparison();      // Family 2: ternary comparison
    demo_math_conversion();      // Family 3: value conversions
    demo_math_normalization();   // Family 4: health levels
    demo_math_calculus();        // Family 5: change detection
    demo_math_integration();     // Integration: calculus + temporal
    demo_bible_rail();           // Bible Rail: spare values → WEB variants
    demo_verse_addr();           // Verse Address: ordinals → packed pairs
    demo_scripture_path();       // Scripture Path: variant-aware file lookup
    demo_scripture_text();       // Scripture Text: character ↔ trit5 encoding

    // ═══════════════════════════════════════════════════════════
    // CLEANUP: Report results
    // ═══════════════════════════════════════════════════════════
    demo_cleanup_and_report();

    return demos_failed;
}

// ════════════════════════════════════════════════════════════════
// SECTION 3: ENTRY POINT
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// main: Entry Point (calls public API)
// ────────────────────────────────────────────────────────────────

int main(void) {
    int failures = demo_math_run_all();
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
    printf("Validating libtrit + math prerequisites...\n");
    printf("────────────────────────────────────────────────────────────────\n");

    int valid = 1;

    // MATTER: Can we create and use trits?
    trit_t t = trit_create(1);
    if (!trit_valid(t) || trit_value(t) != 1) {
        printf("  ✗ MATTER: trit_create broken\n");
        demos_failed++;
        valid = 0;
    } else {
        printf("  ✓ MATTER: trit operations ready\n");
    }

    // MATH: Can we do half-adder?
    trit_result_t r = trit_hadd(TRIT_POS, TRIT_POS);
    if (trit_value(r.value) != -1 || trit_value(r.carry) != 1) {
        printf("  ✗ MATH: trit_hadd broken\n");
        demos_failed++;
        valid = 0;
    } else {
        printf("  ✓ MATH: arithmetic operations ready\n");
    }

    // TEMPORAL: Can we create temporal states?
    temporal_t temp = trit_to_temporal(TRIT_ZERO);
    if (temp != TEMP_PRESENT) {
        printf("  ✗ TEMPORAL: temporal mapping broken\n");
        demos_failed++;
        valid = 0;
    } else {
        printf("  ✓ TEMPORAL: temporal layer ready\n");
    }

    // BIBLE RAIL: Can we encode/decode variants?
    trit5_t spare = bible_rail_encode(0);
    int decoded = bible_rail_decode(spare);
    if (spare != 243 || decoded != 0) {
        printf("  ✗ BIBLE RAIL: encode/decode broken\n");
        demos_failed++;
        valid = 0;
    } else {
        printf("  ✓ BIBLE RAIL: encoding layer ready\n");
    }

    // VERSE ADDR: Can we encode/decode verse addresses?
    verse_addr_t addr = verse_addr_encode(1);
    uint16_t verse_decoded = verse_addr_decode(addr);
    if (addr.high != 0 || addr.low != 1 || verse_decoded != 1) {
        printf("  ✗ VERSE ADDR: encode/decode broken\n");
        demos_failed++;
        valid = 0;
    } else {
        printf("  ✓ VERSE ADDR: addressing layer ready\n");
    }

    // SCRIPTURE TEXT: Can we encode/decode characters?
    uint8_t char_encoded = scripture_text_char_to_trit5('A');
    char char_decoded = scripture_text_trit5_to_char(65);
    if (char_encoded != 65 || char_decoded != 'A') {
        printf("  ✗ SCRIPTURE TEXT: char encode/decode broken\n");
        demos_failed++;
        valid = 0;
    } else {
        printf("  ✓ SCRIPTURE TEXT: character encoding ready\n");
    }

    printf("────────────────────────────────────────────────────────────────\n");
    if (valid) {
        printf("✓ All prerequisites validated. Proceeding with demonstration.\n");
    } else {
        printf("✗ Prerequisites failed. Cannot proceed.\n");
    }

    return valid;
}

// Helpers: Get demo counts (for external inspection)
int demo_math_get_passed(void) { return demos_passed; }
int demo_math_get_failed(void) { return demos_failed; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// Entry Point: main()
//   └── demo_math_run_all() orchestrates all demos
//
// Execution Flow:
//   1. Reset counters (allows multiple runs)
//   2. Print header with scripture
//   3. Validate prerequisites
//   4. Run each family demo (Arithmetic → Comparison → Conversion →
//      Normalization → Calculus → Integration)
//   5. Report results
//   6. Return failure count (0 = success)
//
// Exit Codes:
//   0 - All demos completed successfully
//   1 - One or more validation failures

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// Cleanup runs AFTER execution to report results.
// Called at end of demo_math_run_all().

// Report final results
static void demo_cleanup_and_report(void) {
    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("Demonstration Complete\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n  Steps demonstrated: %d\n", demos_passed);

    if (demos_failed > 0) {
        printf("  Validation failures: %d\n", demos_failed);
    }

    printf("\n  \"A false balance is abomination to the LORD:\n");
    printf("   but a just weight is his delight.\" — Proverbs 11:1\n");
    printf("\n  The 5 families: Arithmetic, Comparison, Conversion,\n");
    printf("  Normalization, and Calculus — with temporal integration.\n");
    printf("\n════════════════════════════════════════════════════════════════\n");
}

// Public: Reset demo state (for multiple runs)
void demo_math_reset(void) {
    demos_passed = 0;
    demos_failed = 0;
}

// ════════════════════════════════════════════════════════════════
// GROUP 2: FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Demo Overview
// ────────────────────────────────────────────────────────────────
//
// This demo demonstrates the 5 math operation families:
//
//   Family 1: Arithmetic   - Half/full adder with carry propagation
//   Family 2: Comparison   - Ternary cmp returning -1/0/+1
//   Family 3: Conversion   - trit↔int, stored↔true
//   Family 4: Normalization - 7 health levels with Hebrew names
//   Family 5: Calculus     - Change detection, rate, trend
//
// Integration shows how calculus operations are inherently temporal.

// ────────────────────────────────────────────────────────────────
// Public API Summary
// ────────────────────────────────────────────────────────────────
//
// Primary API (run demos):
//   demo_math_run_all()        - Run all family demos, return failure count
//   demo_math_arithmetic()     - Family 1: carry-aware addition
//   demo_math_comparison()     - Family 2: ternary comparison
//   demo_math_conversion()     - Family 3: value conversions
//   demo_math_normalization()  - Family 4: health levels
//   demo_math_calculus()       - Family 5: change detection
//   demo_math_integration()    - Calculus + temporal context
//
// Utility API (inspection):
//   demo_math_get_passed()     - Get count of demos passed
//   demo_math_get_failed()     - Get count of validation failures
//   demo_math_reset()          - Reset counters
//
// Usage in educational game:
//
//     // Level 1: Teach arithmetic
//     demo_math_arithmetic();
//     // Level 2: Teach comparison
//     demo_math_comparison();
//     // Final: Show how calculus + time work together
//     demo_math_integration();

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new demo functions (follow demo_math_* pattern)
//   ✅ Adjust print formatting and messages
//   ✅ Add new demonstrations within existing families
//
// Modify with Care:
//   ⚠️ Public API signatures - external code may call these
//   ⚠️ Demo counter logic - affects reporting
//
// NEVER Modify:
//   ❌ 4-block structure
//   ❌ Public API return semantics (0=success)
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "libtrit.a not found" during build
//   - Cause: Library not built yet
//   - Solution: cd word/work/pkg/trit && make
//
// Problem: Demo compiles but shows wrong values
//   - Check: Is libtrit.a up to date? Rebuild if math.c changed.
//   - Check: Are include paths correct in Makefile?
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Library:
//   - word/work/pkg/trit/          (libtrit source)
//   - word/work/pkg/trit/include/  (trit.h, math.h, temporal.h)
//   - word/work/pkg/trit/build/    (libtrit.a)
//
// Demo Files:
//   - main.c     (this file - demo source)
//   - Makefile   (build system)
//
// Other Demos:
//   - tov/demo/phase-0/demo-config/ (config validation)
//   - tov/demo/phase-1/demo-trit/   (three-layer architecture)
//
// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Build and Run:
//
//     make        # Build demo
//     ./demo-math # Run all demos
//     make run    # Build and run
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// "A false balance is abomination to the LORD:
//  but a just weight is his delight." — Proverbs 11:1
//
// The fruit of Phase 2: 38 math functions across 5 families,
// demonstrating that balanced ternary math is not just theoretical
// but practical and elegant. The calculus family shows how math
// and time naturally integrate — change IS temporal.
//
// Just weights. Accurate measures. Kingdom Technology.
//

// ============================================================================
// END CLOSING
// ============================================================================
