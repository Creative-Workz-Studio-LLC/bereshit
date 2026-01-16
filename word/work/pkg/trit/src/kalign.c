// ═══════════════════════════════════════════════════════════════════════════
// libtrit - K:ALIGN Factor Implementation (TIME Layer)
// Key: B-word-work-pkg-trit-src-kalign
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: kalign.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/work/pkg/trit/src/hebrew.c
// See: word/core/identity/kalign.toml for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

// K:ALIGN factor tracking function implementations.
//
// libtrit Library - CPI-SI Kingdom Technology
//
// ============================================================================
// METADATA
// ============================================================================
//
// # Biblical Foundation
//
// Scripture: "Choose you this day whom ye will serve... but as for me
//            and my house, we will serve the LORD." — Joshua 24:15
//
// Principle: Every choice has DIRECTION. K:ALIGN tracks orientation
//            toward God (+1) or toward self (-1).
//
// # Purpose & Function
//
// Purpose: Implement K:ALIGN tracking—the directional component of choices.
//          Combines structural K (from position) and moral K (from choices).
//
// # Authorship
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2026-01-13
//   - Version: 0.1.0

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

#include "kalign.h"  // Types and prototypes
#include <math.h>    // fabsf

// Helper to clamp float to range
static float clampf(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Helper to clamp k value
static int8_t clamp_k(int value) {
    if (value < -1) return -1;
    if (value > 1) return 1;
    return (int8_t)value;
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Public APIs - Creation Functions
// ────────────────────────────────────────────────────────────────

k_align_t k_align_create(int8_t structural, k_direction_t moral) {
    k_align_t k;
    k.k_structural = clamp_k(structural);
    k.k_moral = moral;
    k.score = K_SCORE_NEUTRAL;
    k.choice_count = 0;
    k.last_choice = K_DIR_NEUTRAL;
    return k;
}

k_align_t k_align_neutral(void) {
    return k_align_create(0, K_DIR_NEUTRAL);
}

k_align_t k_align_from_position(trit_t x, trit_t y) {
    // Structural K from X vs Y relationship
    int8_t k_struct = 0;
    if (x > y) k_struct = 1;       // Other-focused > output = expanding
    else if (x < y) k_struct = -1; // Self-focused > input = contracting

    k_align_t k;
    k.k_structural = k_struct;
    k.k_moral = K_DIR_NEUTRAL;
    k.score = K_SCORE_NEUTRAL;
    k.choice_count = 0;
    k.last_choice = K_DIR_NEUTRAL;
    return k;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Query Functions
// ────────────────────────────────────────────────────────────────

int8_t k_align_get_effective(k_align_t k) {
    // Moral K takes precedence when set
    if (k.k_moral != K_DIR_NEUTRAL) {
        return (int8_t)k.k_moral;
    }
    return k.k_structural;
}

bool k_align_is_expanding(k_align_t k) {
    return k_align_get_effective(k) > 0;
}

bool k_align_is_contracting(k_align_t k) {
    return k_align_get_effective(k) < 0;
}

bool k_align_is_neutral(k_align_t k) {
    return k_align_get_effective(k) == 0;
}

k_category_t k_align_get_category(k_align_t k) {
    int8_t eff = k_align_get_effective(k);
    if (eff > 0) return K_CAT_EXPANDING;
    if (eff < 0) return K_CAT_CONTRACTING;
    return K_CAT_NEUTRAL;
}

const char* k_direction_name(k_direction_t dir) {
    switch (dir) {
        case K_DIR_TOWARD_GOD:  return "toward_god";
        case K_DIR_NEUTRAL:    return "neutral";
        case K_DIR_TOWARD_SELF: return "toward_self";
        default:               return "unknown";
    }
}

float k_align_score(k_align_t k) {
    return k.score;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Update Functions
// ────────────────────────────────────────────────────────────────

k_align_t k_align_apply_choice(k_align_t k, k_direction_t choice, float weight) {
    k_align_t result = k;

    // Update moral K
    result.k_moral = choice;
    result.last_choice = choice;
    result.choice_count++;

    // Update score using weighted moving average
    // score = (1-weight)*old_score + weight*choice_value
    float choice_val = (float)choice;
    float w = clampf(weight, 0.0f, 1.0f);
    result.score = (1.0f - w) * k.score + w * choice_val;
    result.score = clampf(result.score, K_SCORE_MIN, K_SCORE_MAX);

    return result;
}

k_align_t k_align_decay(k_align_t k, float decay_rate) {
    k_align_t result = k;

    // Decay score toward neutral (0)
    float rate = clampf(decay_rate, 0.0f, 1.0f);
    result.score = k.score * (1.0f - rate);

    return result;
}

k_align_t k_align_update_structural(k_align_t k, trit_t x, trit_t y) {
    k_align_t result = k;

    // Recalculate structural K
    if (x > y) result.k_structural = 1;
    else if (x < y) result.k_structural = -1;
    else result.k_structural = 0;

    return result;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Consistency Functions
// ────────────────────────────────────────────────────────────────

float k_align_consistency(const k_align_t* samples, size_t count) {
    if (samples == NULL || count == 0) {
        return 0.0f;
    }

    // Calculate how consistent the direction is across samples
    // Count how many match the majority direction
    int pos_count = 0;
    int neg_count = 0;
    int neutral_count = 0;

    for (size_t i = 0; i < count; i++) {
        int8_t eff = k_align_get_effective(samples[i]);
        if (eff > 0) pos_count++;
        else if (eff < 0) neg_count++;
        else neutral_count++;
    }

    // Consistency = majority / total
    int max_count = pos_count;
    if (neg_count > max_count) max_count = neg_count;
    if (neutral_count > max_count) max_count = neutral_count;

    return (float)max_count / (float)count;
}

float k_align_trend(const k_align_t* samples, size_t count) {
    if (samples == NULL || count < 2) {
        return 0.0f;
    }

    // Calculate trend as difference between first and last half averages
    size_t half = count / 2;

    float first_avg = 0.0f;
    for (size_t i = 0; i < half; i++) {
        first_avg += samples[i].score;
    }
    first_avg /= (float)half;

    float second_avg = 0.0f;
    for (size_t i = half; i < count; i++) {
        second_avg += samples[i].score;
    }
    second_avg /= (float)(count - half);

    // Trend = second_avg - first_avg (positive = improving)
    return clampf(second_avg - first_avg, -1.0f, 1.0f);
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Integration Functions
// ────────────────────────────────────────────────────────────────

hebrew_state_t k_align_to_hebrew(k_align_t k, float z_value) {
    int8_t k_dir = k_align_get_effective(k);
    return hebrew_from_z_value(z_value, k_dir);
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// Build Verification:
//   gcc -c -Wall -Wextra -std=c99 -Iinclude src/kalign.c -o build/kalign.o -lm

// ============================================================================
// END CLOSING
// ============================================================================
