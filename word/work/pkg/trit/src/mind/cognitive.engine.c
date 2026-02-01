// ═══════════════════════════════════════════════════════════════════════════
// libtrit - CPI-SI State Machine Implementation
// Key: B-word-work-pkg-trit-src-statemachine
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: statemachine.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/work/pkg/trit/src/trajectory.c
// See: word/core/identity/ for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

// CPI-SI State Machine - Unified cognitive navigation implementation.
//
// libtrit Library - CPI-SI Kingdom Technology
//
// ============================================================================
// METADATA
// ============================================================================
//
// # Biblical Foundation
//
// Scripture: "Be ye transformed by the renewing of your mind."
//            — Romans 12:2
//
// Principle: The state machine is how CPI-SI THINKS. Not pattern matching—
//            genuine cognitive navigation through dimensional space.
//
// # Purpose
//
// Purpose: Implement unified state machine API combining all cognitive
//          components into coherent whole.

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

#include "trit/mind/cognitive.engine.h"
#include <stdio.h>   // printf for debug
#include <string.h>  // memset

// Helper to clamp int16_t to health range
static int16_t clamp_health(int value) {
    if (value < -100) return -100;
    if (value > 100) return 100;
    return (int16_t)value;
}

// Version string
static const char* VERSION_STRING = "0.1.0";

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Creation Functions
// ────────────────────────────────────────────────────────────────

cognitive_state_t cognitive_state_create(void) {
    cognitive_state_t state;
    memset(&state, 0, sizeof(state));

    // Position at center
    state.position = cube_position_center();

    // Hebrew at yashar (anchor)
    state.hebrew = HEBREW_YASHAR;

    // Temporal at present
    state.temporal = temporal_create_base(TEMP_PRESENT);

    // K:ALIGN neutral
    state.k_align = k_align_neutral();

    // Trajectory at grounding (rest)
    state.trajectory = trajectory_grounding();

    // Health neutral
    state.health = HEALTH_NEUTRAL;

    // Session init
    state.choice_count = 0;
    state.session_id = 0;

    return state;
}

cognitive_state_t cognitive_state_at_position(cube_position_t pos) {
    cognitive_state_t state = cognitive_state_create();

    state.position = pos;

    // Derive Hebrew state from Z
    state.k_align = k_align_from_position(pos.x, pos.y);
    state.hebrew = cube_position_to_hebrew(pos);

    return state;
}

cognitive_state_t cognitive_state_from_components(
    cube_position_t pos,
    hebrew_state_t hebrew,
    k_align_t k,
    trajectory_t traj
) {
    cognitive_state_t state = cognitive_state_create();

    state.position = pos;
    state.hebrew = hebrew;
    state.k_align = k;
    state.trajectory = traj;

    return state;
}

// ────────────────────────────────────────────────────────────────
// Choice Functions
// ────────────────────────────────────────────────────────────────

float cognitive_feel_weight(cognitive_state_t state, choice_t potential_choice) {
    // Weight is influenced by:
    // 1. Distance from center (further = heavier)
    // 2. Current K alignment (opposing direction = heavier)
    // 3. Hebrew state (extremes feel heavier)

    float weight = 0.5f;  // Base weight

    // Distance from center
    int dist = cube_distance(state.position, cube_position_center());
    weight += dist * 0.1f;

    // K alignment
    int8_t k_eff = k_align_get_effective(state.k_align);
    if ((k_eff > 0 && potential_choice < 0) ||
        (k_eff < 0 && potential_choice > 0)) {
        weight += 0.2f;  // Opposing direction feels heavier
    }

    // Hebrew state extremes
    if (state.hebrew == HEBREW_SHAVAR || state.hebrew == HEBREW_TOV) {
        weight += 0.15f;  // At extremes, any choice feels heavier
    }

    // Clamp to 0.0-1.0
    if (weight < 0.0f) weight = 0.0f;
    if (weight > 1.0f) weight = 1.0f;

    return weight;
}

cognitive_state_t cognitive_apply_choice(
    cognitive_state_t state,
    choice_t choice,
    float weight
) {
    cognitive_state_t result = state;

    // Update choice count
    result.choice_count++;

    // Apply choice to K:ALIGN
    k_direction_t k_dir;
    switch (choice) {
        case CHOICE_POSITIVE: k_dir = K_DIR_TOWARD_GOD; break;
        case CHOICE_NEGATIVE: k_dir = K_DIR_TOWARD_SELF; break;
        default:              k_dir = K_DIR_NEUTRAL; break;
    }
    result.k_align = k_align_apply_choice(result.k_align, k_dir, weight);

    // Move in Z direction based on choice
    if (choice == CHOICE_POSITIVE) {
        result.position = cube_move_z(result.position, DIR_BUILD_UP);
        result.hebrew = hebrew_next_toward_tov(result.hebrew);
    } else if (choice == CHOICE_NEGATIVE) {
        result.position = cube_move_z(result.position, DIR_BREAK_DOWN);
        result.hebrew = hebrew_next_toward_shavar(result.hebrew);
    }
    // NEUTRAL doesn't change position

    // Update health based on alignment
    float k_score = k_align_score(result.k_align);
    int16_t health_delta = (int16_t)(k_score * weight * 10.0f);
    result.health = clamp_health(result.health + health_delta);

    return result;
}

bool cognitive_apply_choice_detailed(
    cognitive_state_t* state,
    choice_t choice,
    float weight,
    choice_result_t* result
) {
    if (state == NULL || result == NULL) {
        return false;
    }

    // Feel the weight first
    result->weight_felt = cognitive_feel_weight(*state, choice);
    result->choice_made = choice;

    // Record old health
    int16_t old_health = state->health;

    // Apply the choice
    *state = cognitive_apply_choice(*state, choice, weight);

    // Fill result
    result->success = true;
    result->health_delta = state->health - old_health;

    // Generate message
    switch (choice) {
        case CHOICE_POSITIVE:
            result->message = "Advanced toward tov";
            break;
        case CHOICE_NEGATIVE:
            result->message = "Retreated toward shavar";
            break;
        default:
            result->message = "Maintained position";
            break;
    }

    return true;
}

// ────────────────────────────────────────────────────────────────
// Navigation Functions
// ────────────────────────────────────────────────────────────────

cognitive_state_t cognitive_move_x(cognitive_state_t state, direction_t dir) {
    cognitive_state_t result = state;
    result.position = cube_move_x(result.position, dir);
    result.k_align = k_align_update_structural(result.k_align,
                                                result.position.x,
                                                result.position.y);
    return result;
}

cognitive_state_t cognitive_move_y(cognitive_state_t state, direction_t dir) {
    cognitive_state_t result = state;
    result.position = cube_move_y(result.position, dir);
    result.k_align = k_align_update_structural(result.k_align,
                                                result.position.x,
                                                result.position.y);
    return result;
}

cognitive_state_t cognitive_move_z(cognitive_state_t state, direction_t dir) {
    cognitive_state_t result = state;
    result.position = cube_move_z(result.position, dir);
    result.hebrew = cube_position_to_hebrew(result.position);
    return result;
}

cognitive_state_t cognitive_move_to_center(cognitive_state_t state) {
    cognitive_state_t result = state;
    result.position = cube_move_toward_center(result.position);
    result.hebrew = cube_position_to_hebrew(result.position);
    result.k_align = k_align_update_structural(result.k_align,
                                                result.position.x,
                                                result.position.y);
    return result;
}

cognitive_state_t cognitive_advance_trajectory(cognitive_state_t state) {
    cognitive_state_t result = state;
    result.trajectory = trajectory_advance(result.trajectory);
    return result;
}

cognitive_state_t cognitive_return_to_anchor(cognitive_state_t state) {
    cognitive_state_t result = state;

    // Record current position before returning
    result.trajectory = trajectory_record_anchor(result.trajectory,
                                                  result.position,
                                                  result.k_align);

    // Return trajectory to grounding
    result.trajectory = trajectory_to_anchor(result.trajectory);

    // Return position to center
    result.position = cube_position_center();
    result.hebrew = HEBREW_YASHAR;
    result.k_align = k_align_neutral();

    return result;
}

// ────────────────────────────────────────────────────────────────
// Query Functions
// ────────────────────────────────────────────────────────────────

// Static buffer for state name
static char state_name_buffer[128];

const char* cognitive_state_name(cognitive_state_t state) {
    snprintf(state_name_buffer, sizeof(state_name_buffer),
             "%s @ %s [%s]",
             hebrew_get_name(state.hebrew),
             trajectory_position_name(state.trajectory.current),
             cube_position_name(state.position));
    return state_name_buffer;
}

const char* cognitive_hebrew_name(cognitive_state_t state) {
    return hebrew_get_name(state.hebrew);
}

const char* cognitive_trajectory_name(cognitive_state_t state) {
    return trajectory_position_name(state.trajectory.current);
}

bool cognitive_is_centered(cognitive_state_t state) {
    return cube_is_center(state.position);
}

bool cognitive_is_building(cognitive_state_t state) {
    return trajectory_is_building(state.trajectory);
}

bool cognitive_is_grounding(cognitive_state_t state) {
    return trajectory_is_grounding(state.trajectory);
}

int8_t cognitive_get_k_direction(cognitive_state_t state) {
    return k_align_get_effective(state.k_align);
}

const char* cognitive_health_category(cognitive_state_t state) {
    if (state.health <= HEALTH_CRITICAL_LOW) return "CRITICAL";
    if (state.health <= HEALTH_LOW) return "LOW";
    if (state.health <= HEALTH_CAUTION) return "CAUTION";
    if (state.health < HEALTH_GOOD) return "NEUTRAL";
    if (state.health < HEALTH_EXCELLENT) return "GOOD";
    if (state.health < HEALTH_OPTIMAL) return "EXCELLENT";
    return "OPTIMAL";
}

// ────────────────────────────────────────────────────────────────
// Health Functions
// ────────────────────────────────────────────────────────────────

cognitive_state_t cognitive_adjust_health(cognitive_state_t state, int16_t delta) {
    cognitive_state_t result = state;
    result.health = clamp_health(result.health + delta);
    return result;
}

cognitive_state_t cognitive_set_health(cognitive_state_t state, int16_t health) {
    cognitive_state_t result = state;
    result.health = clamp_health(health);
    return result;
}

bool cognitive_health_critical(cognitive_state_t state) {
    return state.health <= HEALTH_CRITICAL_LOW;
}

// ────────────────────────────────────────────────────────────────
// Serialization Functions
// ────────────────────────────────────────────────────────────────

uint32_t cognitive_state_to_int(cognitive_state_t state) {
    // Pack into 32 bits:
    // Bits 0-4: cube index (0-26, needs 5 bits)
    // Bits 5-7: hebrew state (0-6, needs 3 bits)
    // Bits 8-9: k_structural (-1,0,1 → 0,1,2, needs 2 bits)
    // Bits 10-11: k_moral (-1,0,1 → 0,1,2, needs 2 bits)
    // Bits 12-13: trajectory position (1-4 → 0-3, needs 2 bits)
    // Bits 14-21: health (-100 to 100 → 0-200, needs 8 bits)
    // Bits 22-31: reserved

    uint32_t encoded = 0;

    // Cube index (5 bits)
    encoded |= (cube_position_to_index(state.position) & 0x1F);

    // Hebrew state (3 bits)
    encoded |= ((uint32_t)(state.hebrew & 0x07) << 5);

    // K structural (2 bits)
    encoded |= ((uint32_t)((state.k_align.k_structural + 1) & 0x03) << 8);

    // K moral (2 bits)
    encoded |= ((uint32_t)((state.k_align.k_moral + 1) & 0x03) << 10);

    // Trajectory position (2 bits)
    encoded |= ((uint32_t)((state.trajectory.current - 1) & 0x03) << 12);

    // Health (8 bits, shifted to 0-200)
    encoded |= ((uint32_t)((state.health + 100) & 0xFF) << 14);

    return encoded;
}

cognitive_state_t cognitive_state_from_int(uint32_t encoded) {
    cognitive_state_t state = cognitive_state_create();

    // Cube index
    cube_index_t idx = encoded & 0x1F;
    state.position = cube_position_from_index(idx);

    // Hebrew state
    state.hebrew = (hebrew_state_t)((encoded >> 5) & 0x07);

    // K structural
    state.k_align.k_structural = (int8_t)(((encoded >> 8) & 0x03) - 1);

    // K moral
    state.k_align.k_moral = (k_direction_t)(((encoded >> 10) & 0x03) - 1);

    // Trajectory position
    state.trajectory.current = (framework_position_t)(((encoded >> 12) & 0x03) + 1);

    // Health
    state.health = (int16_t)(((encoded >> 14) & 0xFF) - 100);

    return state;
}

// ────────────────────────────────────────────────────────────────
// Debug Functions
// ────────────────────────────────────────────────────────────────

void cognitive_state_print(cognitive_state_t state) {
    printf("CPI-SI Cognitive State:\n");
    printf("  Position: %s (idx=%d)\n",
           cube_position_name(state.position),
           cube_position_to_index(state.position));
    printf("  Hebrew: %s (%s)\n",
           hebrew_get_name(state.hebrew),
           hebrew_get_meaning(state.hebrew));
    printf("  K:ALIGN: structural=%d, moral=%s, score=%.2f\n",
           state.k_align.k_structural,
           k_direction_name(state.k_align.k_moral),
           state.k_align.score);
    printf("  Trajectory: %s (%s)\n",
           trajectory_position_name(state.trajectory.current),
           trajectory_direction_name(state.trajectory.direction));
    printf("  Health: %d (%s)\n",
           state.health,
           cognitive_health_category(state));
    printf("  Choices: %u\n", state.choice_count);
}

const char* cognitive_statemachine_version(void) {
    return VERSION_STRING;
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// Build Verification:
//   gcc -c -Wall -Wextra -std=c99 -Iinclude src/statemachine.c -o build/statemachine.o

// ============================================================================
// END CLOSING
// ============================================================================
