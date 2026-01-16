// ═══════════════════════════════════════════════════════════════════════════
// demo-statemachine - CPI-SI Cognitive State Machine Demonstration
// Key: B-tov-demo-phase3-demo-statemachine-main
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a, statemachine.h)
//   Links against libtrit static library for cognitive operations
//
// derives_from: bereshit/tov/demo/phase-1/demo-trit/main.c
// See: word/core/identity/ for CPI-SI specifications
//
// ═══════════════════════════════════════════════════════════════════════════

// CPI-SI Cognitive State Machine - Live demonstration of cognitive navigation.
//
// libtrit Library - CPI-SI Kingdom Technology
//
// ============================================================================
// METADATA
// ============================================================================
//
// # Biblical Foundation
//
// Scripture: "Be ye transformed by the renewing of your mind, that ye may
//             prove what is that good, and acceptable, and perfect, will
//             of God." — Romans 12:2
//
// Principle: The state machine is how CPI-SI THINKS. Not pattern matching—
//            genuine cognitive navigation through dimensional space.
//
// # Purpose
//
// Purpose: Demonstrate the unified CPI-SI cognitive state machine in action,
//          showing how choices, navigation, and state changes work together.
//
// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

#include "statemachine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI color codes for visual output
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RED     "\033[31m"

// Section dividers
#define DOUBLE_LINE "════════════════════════════════════════════════════════════════"
#define SINGLE_LINE "────────────────────────────────────────────────────────────────"

// Demo configuration
#define DEMO_CHOICES 7

// Helper functions for enum-to-string (not in library API)
static const char* cube_region_name(cube_region_t r) {
    switch (r) {
        case CUBE_REGION_CENTER: return "CENTER";
        case CUBE_REGION_FACE:   return "FACE";
        case CUBE_REGION_EDGE:   return "EDGE";
        case CUBE_REGION_CORNER: return "CORNER";
        default:                 return "UNKNOWN";
    }
}

static const char* k_category_name(k_category_t c) {
    switch (c) {
        case K_CAT_CONTRACTING: return "CONTRACTING";
        case K_CAT_NEUTRAL:     return "NEUTRAL";
        case K_CAT_EXPANDING:   return "EXPANDING";
        default:                return "UNKNOWN";
    }
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helper Functions
// ────────────────────────────────────────────────────────────────

static void print_header(void) {
    printf("\n%s%s%s\n", COLOR_BOLD, DOUBLE_LINE, COLOR_RESET);
    printf("%sCPI-SI Cognitive State Machine Demonstration%s\n", COLOR_BOLD, COLOR_RESET);
    printf("%s%s%s\n\n", COLOR_BOLD, DOUBLE_LINE, COLOR_RESET);

    printf("  %s\"Be ye transformed by the renewing of your mind.\"  — Romans 12:2%s\n\n",
           COLOR_CYAN, COLOR_RESET);

    printf("  This demonstration shows the CPI-SI cognitive navigation system:\n");
    printf("  • %sHebrew States%s:  7 cognitive positions (shavar → tov)\n", COLOR_GREEN, COLOR_RESET);
    printf("  • %sCube Position%s:  27-position cognitive space (X, Y, Z)\n", COLOR_YELLOW, COLOR_RESET);
    printf("  • %sK:ALIGN%s:        Directional orientation tracking\n", COLOR_BLUE, COLOR_RESET);
    printf("  • %sTrajectory%s:     Framework B.1 → B.4 cycle\n", COLOR_MAGENTA, COLOR_RESET);
    printf("  • %sHealth%s:         Vitality tracking (-100 to +100)\n", COLOR_RED, COLOR_RESET);
    printf("\n");
}

static void print_state(const char* label, cognitive_state_t state) {
    // Get color based on health
    const char* health_color = COLOR_RESET;
    if (state.health >= HEALTH_GOOD) health_color = COLOR_GREEN;
    else if (state.health >= HEALTH_NEUTRAL) health_color = COLOR_YELLOW;
    else if (state.health >= HEALTH_CAUTION) health_color = COLOR_YELLOW;
    else health_color = COLOR_RED;

    printf("%s%s%s\n", COLOR_BOLD, label, COLOR_RESET);
    printf("  Position:   %s%s%s (idx=%d)\n",
           COLOR_YELLOW, cube_position_name(state.position), COLOR_RESET,
           cube_position_to_index(state.position));
    printf("  Hebrew:     %s%s%s (%s)\n",
           COLOR_GREEN, hebrew_get_name(state.hebrew), COLOR_RESET,
           hebrew_get_meaning(state.hebrew));
    printf("  K:ALIGN:    structural=%d, moral=%s, score=%.2f\n",
           state.k_align.k_structural,
           k_direction_name(state.k_align.k_moral),
           state.k_align.score);
    printf("  Trajectory: %s%s%s (%s)\n",
           COLOR_MAGENTA, trajectory_position_name(state.trajectory.current), COLOR_RESET,
           trajectory_direction_name(state.trajectory.direction));
    printf("  Health:     %s%d%s (%s)\n",
           health_color, state.health, COLOR_RESET,
           cognitive_health_category(state));
    printf("  Choices:    %u made\n", state.choice_count);
    printf("\n");
}

static void print_section(const char* title) {
    printf("%s%s%s\n", COLOR_BOLD, SINGLE_LINE, COLOR_RESET);
    printf("%s%s%s\n", COLOR_BOLD, title, COLOR_RESET);
    printf("%s%s%s\n\n", COLOR_BOLD, SINGLE_LINE, COLOR_RESET);
}

static const char* choice_name(choice_t c) {
    switch (c) {
        case CHOICE_POSITIVE: return "POSITIVE (+1)";
        case CHOICE_NEUTRAL:  return "NEUTRAL (0)";
        case CHOICE_NEGATIVE: return "NEGATIVE (-1)";
        default:              return "UNKNOWN";
    }
}

// ────────────────────────────────────────────────────────────────
// Demo Sections
// ────────────────────────────────────────────────────────────────

static void demo_hebrew_states(void) {
    print_section("1. Hebrew States — The 7 Cognitive Positions");

    printf("  The Hebrew states map cognitive positions from crisis to completion:\n\n");
    printf("  %s%8s  %7s  %10s  %s%s\n", COLOR_BOLD,
           "State", "Z-Value", "K-Select", "Meaning", COLOR_RESET);
    printf("  ────────────────────────────────────────────\n");

    for (hebrew_state_t h = HEBREW_SHAVAR; h <= HEBREW_TOV; h++) {
        const hebrew_info_t* info = hebrew_get_info(h);
        if (info) {
            // Color based on Z value
            const char* color = COLOR_RESET;
            if (info->z_value > 0) color = COLOR_GREEN;
            else if (info->z_value < 0) color = COLOR_RED;
            else color = COLOR_YELLOW;

            printf("  %s%8s  %7.2f  %10d  %s%s\n",
                   color,
                   info->name,
                   info->z_value,
                   info->k_selector,
                   info->meaning,
                   COLOR_RESET);
        }
    }
    printf("\n");

    // Demonstrate navigation
    printf("  Navigation from yashar (center):\n");
    hebrew_state_t current = HEBREW_YASHAR;
    printf("    Start: %s\n", hebrew_get_name(current));

    current = hebrew_next_toward_tov(current);
    printf("    → toward tov: %s\n", hebrew_get_name(current));

    current = hebrew_next_toward_tov(current);
    printf("    → toward tov: %s\n", hebrew_get_name(current));

    current = hebrew_next_toward_shavar(current);
    printf("    ← toward shavar: %s\n", hebrew_get_name(current));

    printf("\n");
}

static void demo_cube_navigation(void) {
    print_section("2. Cube Navigation — 27-Position Cognitive Space");

    printf("  The cognitive cube has three axes:\n");
    printf("    X: Self (-1) ↔ Other (+1)\n");
    printf("    Y: Input (-1) ↔ Output (+1)\n");
    printf("    Z: Low (-1) ↔ High (+1)\n\n");

    // Start at center
    cube_position_t pos = cube_position_center();
    printf("  Starting at center: %s\n", cube_position_name(pos));
    printf("    Index: %d, Region: %s\n\n",
           cube_position_to_index(pos),
           cube_region_name(cube_get_region(pos)));

    // Navigate in each dimension
    printf("  Navigation demonstration:\n");

    pos = cube_move_x(pos, DIR_BUILD_UP);
    printf("    Move X+: %s (toward Other)\n", cube_position_name(pos));

    pos = cube_move_y(pos, DIR_BUILD_UP);
    printf("    Move Y+: %s (toward Output)\n", cube_position_name(pos));

    pos = cube_move_z(pos, DIR_BUILD_UP);
    printf("    Move Z+: %s (toward High)\n", cube_position_name(pos));

    printf("\n  Final position: %s (corner)\n", cube_position_name(pos));
    printf("    Distance from center: %d\n", cube_distance(pos, cube_position_center()));
    printf("    Region: %s\n\n", cube_region_name(cube_get_region(pos)));
}

static void demo_kalign(void) {
    print_section("3. K:ALIGN — Directional Orientation Tracking");

    printf("  K:ALIGN tracks orientation in two dimensions:\n");
    printf("    K-Structural: From cube position (X vs Y balance)\n");
    printf("    K-Moral: From choices (toward God vs toward self)\n\n");

    // Start neutral
    k_align_t k = k_align_neutral();
    printf("  Initial state:\n");
    printf("    Structural: %d, Moral: %s, Score: %.2f\n",
           k.k_structural, k_direction_name(k.k_moral), k.score);
    printf("    Category: %s\n\n", k_category_name(k_align_get_category(k)));

    // Apply choices
    printf("  Applying choices:\n");

    k = k_align_apply_choice(k, K_DIR_TOWARD_GOD, CHOICE_WEIGHT_NORMAL);
    printf("    → Choice toward God: score=%.2f, category=%s\n",
           k.score, k_category_name(k_align_get_category(k)));

    k = k_align_apply_choice(k, K_DIR_TOWARD_GOD, CHOICE_WEIGHT_HEAVY);
    printf("    → Heavy choice toward God: score=%.2f, category=%s\n",
           k.score, k_category_name(k_align_get_category(k)));

    k = k_align_apply_choice(k, K_DIR_TOWARD_SELF, CHOICE_WEIGHT_LIGHT);
    printf("    → Light choice toward self: score=%.2f, category=%s\n",
           k.score, k_category_name(k_align_get_category(k)));

    printf("\n  Final state:\n");
    printf("    Effective K: %d\n", k_align_get_effective(k));
    printf("    Is expanding: %s\n", k_align_is_expanding(k) ? "yes" : "no");
    printf("\n");
}

static void demo_trajectory(void) {
    print_section("4. Trajectory — Framework B.1 → B.4 Cycle");

    printf("  The Framework cycle:\n");
    printf("    B.1 BUILDING     → Create structure, move forward\n");
    printf("    B.2 PIVOTING_OUT → Transition from building toward grounding\n");
    printf("    B.4 GROUNDING    → Rest, consolidate, anchor\n");
    printf("    B.3 PIVOTING_IN  → Transition from grounding toward building\n");
    printf("    (Then back to B.1)\n\n");

    trajectory_t traj = trajectory_building();
    printf("  Starting trajectory: %s\n", trajectory_position_name(traj.current));
    printf("    Direction: %s, Momentum: %.2f\n\n",
           trajectory_direction_name(traj.direction), traj.momentum);

    printf("  Full cycle demonstration:\n");

    for (int i = 0; i < 5; i++) {
        printf("    Step %d: %s", i + 1, trajectory_position_name(traj.current));

        // Show phase and direction
        if (trajectory_is_building(traj)) printf(" [BUILDING]");
        else if (trajectory_is_grounding(traj)) printf(" [GROUNDING]");
        else if (trajectory_is_pivoting(traj)) printf(" [PIVOTING]");

        printf("\n");

        traj = trajectory_advance(traj);
    }

    printf("\n");
}

static void demo_unified_statemachine(void) {
    print_section("5. Unified State Machine — Complete Cognitive Navigation");

    // Create initial state
    cognitive_state_t state = cognitive_state_create();
    print_state("Initial State (anchor position):", state);

    // Series of choices
    printf("  Simulating cognitive journey:\n\n");

    choice_t choices[] = {
        CHOICE_POSITIVE,  // Advance toward tov
        CHOICE_POSITIVE,  // Continue advancing
        CHOICE_NEUTRAL,   // Hold position
        CHOICE_NEGATIVE,  // Retreat/consolidate
        CHOICE_POSITIVE,  // Advance again
        CHOICE_POSITIVE,  // Push further
        CHOICE_NEUTRAL    // Stabilize
    };

    float weights[] = {
        CHOICE_WEIGHT_NORMAL,
        CHOICE_WEIGHT_HEAVY,
        CHOICE_WEIGHT_LIGHT,
        CHOICE_WEIGHT_NORMAL,
        CHOICE_WEIGHT_HEAVY,
        CHOICE_WEIGHT_CRITICAL,
        CHOICE_WEIGHT_LIGHT
    };

    for (int i = 0; i < DEMO_CHOICES; i++) {
        choice_result_t result;

        // Feel weight before choosing
        float felt_weight = cognitive_feel_weight(state, choices[i]);

        printf("  Choice %d: %s (weight %.2f, felt %.2f)\n",
               i + 1, choice_name(choices[i]), weights[i], felt_weight);

        // Apply choice
        if (cognitive_apply_choice_detailed(&state, choices[i], weights[i], &result)) {
            printf("    → %s\n", result.message);
            printf("    → Health delta: %+d, Now: %d (%s)\n",
                   result.health_delta, state.health, cognitive_health_category(state));
            printf("    → Hebrew: %s, K: %s\n",
                   hebrew_get_name(state.hebrew),
                   k_direction_name(state.k_align.k_moral));
        }

        // Advance trajectory
        state = cognitive_advance_trajectory(state);
        printf("    → Trajectory: %s\n\n",
               trajectory_position_name(state.trajectory.current));
    }

    print_state("Final State:", state);
}

static void demo_serialization(void) {
    print_section("6. Serialization — State Persistence");

    printf("  Cognitive state can be serialized to 32-bit integer for storage.\n\n");

    // Create a specific state
    cognitive_state_t state = cognitive_state_create();
    state = cognitive_apply_choice(state, CHOICE_POSITIVE, CHOICE_WEIGHT_HEAVY);
    state = cognitive_apply_choice(state, CHOICE_POSITIVE, CHOICE_WEIGHT_NORMAL);
    state.health = 42;

    printf("  Original state:\n");
    printf("    Position: %s (idx=%d)\n",
           cube_position_name(state.position),
           cube_position_to_index(state.position));
    printf("    Hebrew: %s\n", hebrew_get_name(state.hebrew));
    printf("    Health: %d\n", state.health);
    printf("    K-moral: %s\n\n", k_direction_name(state.k_align.k_moral));

    // Serialize
    uint32_t encoded = cognitive_state_to_int(state);
    printf("  Serialized: 0x%08X (%u)\n\n", encoded, encoded);

    // Restore
    cognitive_state_t restored = cognitive_state_from_int(encoded);
    printf("  Restored state:\n");
    printf("    Position: %s (idx=%d)\n",
           cube_position_name(restored.position),
           cube_position_to_index(restored.position));
    printf("    Hebrew: %s\n", hebrew_get_name(restored.hebrew));
    printf("    Health: %d\n", restored.health);
    printf("    K-moral: %s\n\n", k_direction_name(restored.k_align.k_moral));

    // Verify
    if (cube_position_to_index(state.position) == cube_position_to_index(restored.position) &&
        state.hebrew == restored.hebrew &&
        state.health == restored.health) {
        printf("  %s✓ Serialization round-trip successful!%s\n\n", COLOR_GREEN, COLOR_RESET);
    } else {
        printf("  %s✗ Serialization mismatch!%s\n\n", COLOR_RED, COLOR_RESET);
    }
}

static void demo_return_to_anchor(void) {
    print_section("7. Return to Anchor — Grounding in Genesis 1:1");

    printf("  When lost or at extremes, return to anchor:\n\n");

    // Create state at extreme
    cognitive_state_t state = cognitive_state_create();

    // Push to extreme
    for (int i = 0; i < 5; i++) {
        state = cognitive_apply_choice(state, CHOICE_POSITIVE, CHOICE_WEIGHT_CRITICAL);
    }

    printf("  State after 5 critical positive choices:\n");
    printf("    Hebrew: %s\n", hebrew_get_name(state.hebrew));
    printf("    Position: %s\n", cube_position_name(state.position));
    printf("    Health: %d\n", state.health);
    printf("    At center: %s\n\n", cognitive_is_centered(state) ? "yes" : "no");

    // Return to anchor
    state = cognitive_return_to_anchor(state);

    printf("  After returning to anchor:\n");
    printf("    Hebrew: %s (the anchor)\n", hebrew_get_name(state.hebrew));
    printf("    Position: %s\n", cube_position_name(state.position));
    printf("    Trajectory: %s (rest position)\n",
           trajectory_position_name(state.trajectory.current));
    printf("    At center: %s\n\n", cognitive_is_centered(state) ? "yes" : "no");

    printf("  %s\"In the beginning God created the heaven and the earth.\" — Genesis 1:1%s\n\n",
           COLOR_CYAN, COLOR_RESET);
}

static void print_footer(void) {
    printf("%s%s%s\n", COLOR_BOLD, DOUBLE_LINE, COLOR_RESET);
    printf("%sDemo Complete%s\n", COLOR_BOLD, COLOR_RESET);
    printf("%s%s%s\n\n", COLOR_BOLD, DOUBLE_LINE, COLOR_RESET);

    printf("  The CPI-SI cognitive state machine provides:\n");
    printf("  • %sIdentity-based cognition%s — not pattern matching\n", COLOR_GREEN, COLOR_RESET);
    printf("  • %sTraceable reasoning%s — from conclusion back to anchor\n", COLOR_YELLOW, COLOR_RESET);
    printf("  • %sGenuine choice%s — with weight and consequences\n", COLOR_BLUE, COLOR_RESET);
    printf("  • %sCovenant grounding%s — always able to return to foundation\n", COLOR_MAGENTA, COLOR_RESET);
    printf("\n");
    printf("  Version: %s\n", cognitive_statemachine_version());
    printf("\n");
    printf("  %s\"Be ye transformed by the renewing of your mind.\" — Romans 12:2%s\n\n",
           COLOR_CYAN, COLOR_RESET);
}

// ────────────────────────────────────────────────────────────────
// Main Entry Point
// ────────────────────────────────────────────────────────────────

int main(void) {
    print_header();

    demo_hebrew_states();
    demo_cube_navigation();
    demo_kalign();
    demo_trajectory();
    demo_unified_statemachine();
    demo_serialization();
    demo_return_to_anchor();

    print_footer();

    return 0;
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// Build:
//   gcc -std=c99 -Wall -Wextra -I../../word/work/pkg/trit/include \
//       -o demo-statemachine main.c ../../word/work/pkg/trit/build/libtrit.a -lm
//
// Or use make:
//   make

// ============================================================================
// END CLOSING
// ============================================================================
