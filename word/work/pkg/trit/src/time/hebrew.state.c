// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Hebrew State Implementation (TIME Layer)
// Key: B-word-work-pkg-trit-src-hebrew
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: hebrew.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/work/pkg/trit/src/temporal.c
// See: word/core/health/thresholds.toml for value specifications
//
// ═══════════════════════════════════════════════════════════════════════════

// Hebrew cognitive state function implementations.
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
// Scripture: "And God saw every thing that he had made, and, behold,
//            it was very good." — Genesis 1:31 (tov - טוֹב)
//
// Principle: The 7 days of Creation establish 7 cognitive states.
//            From shavar (broken) through yashar (even) to tov (perfect).
//
// Anchor: "Thou wilt shew me the path of life." — Psalm 16:11
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Implements Hebrew state functions—the 7 cognitive positions
//       the mind can occupy. This is TIME layer cognition.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2026-01-13
//   - Version: 0.1.0
//   - Modified: 2026-01-13 - Initial implementation
//
// # Purpose & Function
//
// Purpose: Implement Hebrew state operations that map cognitive positions
//          to the 7 states from shavar to tov.
//
// Core Design: 11 pure functions implementing Hebrew state queries,
//              navigation, and K-factor integration.
//
// Philosophy: Where IS the mind right now? These functions answer that.

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Includes
// ────────────────────────────────────────────────────────────────

#include "trit/time/hebrew.state.h"  // Types and prototypes (includes trit.type.h, stdbool.h, stdint.h)

// ────────────────────────────────────────────────────────────────
// Static Data - Hebrew State Information Table
// ────────────────────────────────────────────────────────────────

// Static table of Hebrew state information.
// Indexed by hebrew_state_t enum value (0-6).
static const hebrew_info_t hebrew_state_table[HEBREW_STATE_COUNT] = {
    // HEBREW_SHAVAR (0) - BROKEN, Day 1
    {
        .state           = HEBREW_SHAVAR,
        .name            = "shavar",
        .meaning         = "BROKEN",
        .z_value         = -1.0f,
        .k_selector      = 0,       // Pure state
        .day             = 1,
        .operator_symbol = "\xE2\x8E\xAF"  // ⎯ (horizontal line)
    },
    // HEBREW_CHASER (1) - LACKING, Day 2
    {
        .state           = HEBREW_CHASER,
        .name            = "chaser",
        .meaning         = "LACKING",
        .z_value         = -0.5f,
        .k_selector      = -1,      // Contracting (k=-1)
        .day             = 2,
        .operator_symbol = "\xE2\x86\x90"  // ←
    },
    // HEBREW_RATSAH (2) - WANTING, Day 3
    {
        .state           = HEBREW_RATSAH,
        .name            = "ratsah",
        .meaning         = "WANTING",
        .z_value         = -0.5f,
        .k_selector      = 1,       // Expanding (k=+1)
        .day             = 3,
        .operator_symbol = "\xE2\x86\x94"  // ↔
    },
    // HEBREW_YASHAR (3) - EVEN, Day 4 (anchor)
    {
        .state           = HEBREW_YASHAR,
        .name            = "yashar",
        .meaning         = "EVEN",
        .z_value         = 0.0f,
        .k_selector      = 0,       // Pure state (anchor)
        .day             = 4,
        .operator_symbol = "\xE2\x97\x86"  // ◆
    },
    // HEBREW_TAMIM (4) - SOUND, Day 5
    {
        .state           = HEBREW_TAMIM,
        .name            = "tamim",
        .meaning         = "SOUND",
        .z_value         = 0.5f,
        .k_selector      = -1,      // Consolidating (k=-1)
        .day             = 5,
        .operator_symbol = "\xE2\x86\x97"  // ↗
    },
    // HEBREW_SHALEM (5) - WHOLE, Day 6
    {
        .state           = HEBREW_SHALEM,
        .name            = "shalem",
        .meaning         = "WHOLE",
        .z_value         = 0.5f,
        .k_selector      = 1,       // Expanding (k=+1)
        .day             = 6,
        .operator_symbol = "\xE2\x86\x92"  // →
    },
    // HEBREW_TOV (6) - PERFECT, Day 7
    {
        .state           = HEBREW_TOV,
        .name            = "tov",
        .meaning         = "PERFECT",
        .z_value         = 1.0f,
        .k_selector      = 0,       // Pure state
        .day             = 7,
        .operator_symbol = "\xE2\x9C\x93"  // ✓
    }
};

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Public APIs - Query Functions
// ────────────────────────────────────────────────────────────────

// hebrew_get_info returns complete metadata for a Hebrew state.
//
// What It Does:
// Returns pointer to static hebrew_info_t containing all state metadata.
// Invalid states return yashar (anchor/safe default).
//
const hebrew_info_t* hebrew_get_info(hebrew_state_t state) {
    if (state < 0 || state >= HEBREW_STATE_COUNT) {
        return &hebrew_state_table[HEBREW_YASHAR];  // Safe default
    }
    return &hebrew_state_table[state];
}

// hebrew_get_name returns the Hebrew name for a state.
//
const char* hebrew_get_name(hebrew_state_t state) {
    return hebrew_get_info(state)->name;
}

// hebrew_get_meaning returns the English meaning for a state.
//
const char* hebrew_get_meaning(hebrew_state_t state) {
    return hebrew_get_info(state)->meaning;
}

// hebrew_get_z_value returns the Z-axis value for a state.
//
float hebrew_get_z_value(hebrew_state_t state) {
    return hebrew_get_info(state)->z_value;
}

// hebrew_get_operator returns the visual operator symbol.
//
const char* hebrew_get_operator(hebrew_state_t state) {
    return hebrew_get_info(state)->operator_symbol;
}

// hebrew_get_day returns the Day of Creation for a state.
//
int hebrew_get_day(hebrew_state_t state) {
    return hebrew_get_info(state)->day;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - K-Factor Functions
// ────────────────────────────────────────────────────────────────

// hebrew_get_k_selector returns the K-selector for a state.
//
// K-selector indicates structural direction:
//   -1: Contracting (chaser, tamim)
//    0: Pure/neutral (shavar, yashar, tov)
//   +1: Expanding (ratsah, shalem)
//
int8_t hebrew_get_k_selector(hebrew_state_t state) {
    return hebrew_get_info(state)->k_selector;
}

// hebrew_is_pure_state checks if state has k_selector == 0.
//
// Pure states: shavar, yashar, tov
// Mixed states: chaser, ratsah, tamim, shalem
//
bool hebrew_is_pure_state(hebrew_state_t state) {
    return hebrew_get_k_selector(state) == 0;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Conversion Functions
// ────────────────────────────────────────────────────────────────

// hebrew_from_z_value converts a Z-value to Hebrew state.
//
// Uses thresholds and K-direction to disambiguate:
//   z <= -0.75: SHAVAR
//   -0.75 < z <= -0.25: CHASER (k=-1) or RATSAH (k=+1)
//   -0.25 < z < 0.25: YASHAR
//   0.25 <= z < 0.75: TAMIM (k=-1) or SHALEM (k=+1)
//   z >= 0.75: TOV
//
hebrew_state_t hebrew_from_z_value(float z_value, int8_t k_direction) {
    // Clamp to valid range
    if (z_value <= HEBREW_Z_THRESHOLD_1) {
        return HEBREW_SHAVAR;
    }
    if (z_value < HEBREW_Z_THRESHOLD_2) {
        // Ambiguous zone: chaser or ratsah
        return (k_direction <= 0) ? HEBREW_CHASER : HEBREW_RATSAH;
    }
    if (z_value < HEBREW_Z_THRESHOLD_3) {
        return HEBREW_YASHAR;
    }
    if (z_value < HEBREW_Z_THRESHOLD_4) {
        // Ambiguous zone: tamim or shalem
        return (k_direction <= 0) ? HEBREW_TAMIM : HEBREW_SHALEM;
    }
    return HEBREW_TOV;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Navigation Functions
// ────────────────────────────────────────────────────────────────

// hebrew_next_toward_tov returns next state moving toward tov.
//
// Progression: shavar → chaser/ratsah → yashar → tamim/shalem → tov
// At tov, returns tov (already at maximum).
//
hebrew_state_t hebrew_next_toward_tov(hebrew_state_t current) {
    switch (current) {
        case HEBREW_SHAVAR:
            return HEBREW_RATSAH;  // Move to wanting (growth)
        case HEBREW_CHASER:
            return HEBREW_YASHAR;  // Move to center
        case HEBREW_RATSAH:
            return HEBREW_YASHAR;  // Move to center
        case HEBREW_YASHAR:
            return HEBREW_SHALEM;  // Move to expanding
        case HEBREW_TAMIM:
            return HEBREW_TOV;     // Move to completion
        case HEBREW_SHALEM:
            return HEBREW_TOV;     // Move to completion
        case HEBREW_TOV:
            return HEBREW_TOV;     // Already at max
        default:
            return HEBREW_YASHAR;  // Safe default
    }
}

// hebrew_next_toward_shavar returns next state moving toward shavar.
//
// Progression: tov → tamim/shalem → yashar → chaser/ratsah → shavar
// At shavar, returns shavar (already at minimum).
//
hebrew_state_t hebrew_next_toward_shavar(hebrew_state_t current) {
    switch (current) {
        case HEBREW_TOV:
            return HEBREW_TAMIM;   // Move to consolidating
        case HEBREW_SHALEM:
            return HEBREW_YASHAR;  // Move to center
        case HEBREW_TAMIM:
            return HEBREW_YASHAR;  // Move to center
        case HEBREW_YASHAR:
            return HEBREW_CHASER;  // Move to lacking
        case HEBREW_RATSAH:
            return HEBREW_SHAVAR;  // Move to broken
        case HEBREW_CHASER:
            return HEBREW_SHAVAR;  // Move to broken
        case HEBREW_SHAVAR:
            return HEBREW_SHAVAR;  // Already at min
        default:
            return HEBREW_YASHAR;  // Safe default
    }
}

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
//   cd /media/seanje-lenox-wise/Project/Bereshit/word/work/pkg/trit
//   gcc -c -Wall -Wextra -std=c99 -Iinclude src/hebrew.c -o build/hebrew.o
//
// Test sequence:
//   make test  (when tests exist)

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// 11 pure functions implementing Hebrew cognitive states for CPI-SI.
// 7 states from Genesis Creation week, with K-factor integration
// and Z-value mapping for cube navigation.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Get state info:
//   const hebrew_info_t* info = hebrew_get_info(HEBREW_YASHAR);
//
// Query properties:
//   const char* name = hebrew_get_name(HEBREW_TOV);  // "tov"
//   float z = hebrew_get_z_value(HEBREW_SHAVAR);     // -1.0
//
// Convert from Z:
//   hebrew_state_t state = hebrew_from_z_value(-0.5f, +1);  // RATSAH
//
// Navigate:
//   hebrew_state_t next = hebrew_next_toward_tov(HEBREW_YASHAR);  // SHALEM

// ============================================================================
// END CLOSING
// ============================================================================
