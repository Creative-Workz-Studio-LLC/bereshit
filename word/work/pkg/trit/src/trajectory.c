// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Framework Trajectory Implementation (TIME Layer)
// Key: B-word-work-pkg-trit-src-trajectory
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trajectory.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/work/pkg/trit/src/kalign.c
// See: word/core/identity/trajectory.toml for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

// Framework trajectory navigation function implementations.
//
// libtrit Library - CPI-SI Kingdom Technology
//
// ============================================================================
// METADATA
// ============================================================================
//
// # Biblical Foundation
//
// Scripture: "Trust in the LORD with all thine heart; and lean not unto
//            thine own understanding. In all thy ways acknowledge him,
//            and he shall direct thy paths." — Proverbs 3:5-6
//
// Principle: Trajectory is PATH—direction and momentum through cognitive space.
//            The Framework has 4 positions forming a build-pivot-ground cycle.
//
// # Purpose & Function
//
// Purpose: Implement Framework trajectory navigation through B.1-B.4 positions.
//          Track position, direction, momentum, and anchor path for tracing back.
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

#include "trajectory.h"  // Types and prototypes

// Helper to clamp float to range
static float clampf(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
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

trajectory_t trajectory_create(framework_position_t pos) {
    trajectory_t traj;

    // Validate position
    if (pos < FW_BUILDING || pos > FW_GROUNDING) {
        pos = FW_GROUNDING;  // Safe default
    }

    traj.current = pos;
    traj.direction = TRAJ_NEUTRAL;
    traj.momentum = (pos == FW_GROUNDING) ? TRAJECTORY_MOMENTUM_REST : TRAJECTORY_MOMENTUM_NORMAL;
    traj.pivot_count = 0;
    traj.anchor_path_count = 0;
    traj.last_advance_successful = true;

    return traj;
}

trajectory_t trajectory_building(void) {
    trajectory_t traj = trajectory_create(FW_BUILDING);
    traj.direction = TRAJ_BSPACETIME;  // Expanding
    traj.momentum = TRAJECTORY_MOMENTUM_ACTIVE;
    return traj;
}

trajectory_t trajectory_grounding(void) {
    trajectory_t traj = trajectory_create(FW_GROUNDING);
    traj.direction = TRAJ_BTIMESPACE;  // Consolidating
    traj.momentum = TRAJECTORY_MOMENTUM_REST;
    return traj;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Navigation Functions
// ────────────────────────────────────────────────────────────────

// Framework cycle: B.1 → B.2 → B.4 → B.3 → B.1
// Note: B.3 comes AFTER B.4 (pivot IN happens after grounding)
trajectory_t trajectory_advance(trajectory_t traj) {
    trajectory_t result = traj;
    result.last_advance_successful = true;

    switch (traj.current) {
        case FW_BUILDING:     // B.1 → B.2
            result.current = FW_PIVOTING_OUT;
            result.direction = TRAJ_BTIMESPACE;
            break;

        case FW_PIVOTING_OUT: // B.2 → B.4
            result.current = FW_GROUNDING;
            result.pivot_count++;
            result.momentum = TRAJECTORY_MOMENTUM_REST;
            break;

        case FW_GROUNDING:    // B.4 → B.3
            result.current = FW_PIVOTING_IN;
            result.direction = TRAJ_BSPACETIME;
            break;

        case FW_PIVOTING_IN:  // B.3 → B.1
            result.current = FW_BUILDING;
            result.pivot_count++;
            result.momentum = TRAJECTORY_MOMENTUM_ACTIVE;
            break;

        default:
            result.current = FW_GROUNDING;
            result.last_advance_successful = false;
    }

    return result;
}

trajectory_t trajectory_pivot(trajectory_t traj) {
    trajectory_t result = traj;
    result.pivot_count++;
    return result;
}

trajectory_t trajectory_to_anchor(trajectory_t traj) {
    trajectory_t result = traj;

    // Move to grounding (anchor position)
    result.current = FW_GROUNDING;
    result.direction = TRAJ_BTIMESPACE;
    result.momentum = TRAJECTORY_MOMENTUM_REST;
    // Reset pivot count when grounding
    result.pivot_count = 0;

    return result;
}

trajectory_t trajectory_set_position(trajectory_t traj, framework_position_t pos) {
    trajectory_t result = traj;

    // Validate position
    if (pos < FW_BUILDING || pos > FW_GROUNDING) {
        pos = FW_GROUNDING;
    }

    result.current = pos;

    // Reset pivot count if moving to grounding
    if (pos == FW_GROUNDING) {
        result.pivot_count = 0;
        result.momentum = TRAJECTORY_MOMENTUM_REST;
    }

    return result;
}

trajectory_t trajectory_set_direction(trajectory_t traj, trajectory_direction_t dir) {
    trajectory_t result = traj;
    result.direction = dir;
    return result;
}

trajectory_t trajectory_adjust_momentum(trajectory_t traj, float delta) {
    trajectory_t result = traj;
    result.momentum = clampf(traj.momentum + delta,
                             TRAJECTORY_MOMENTUM_MIN,
                             TRAJECTORY_MOMENTUM_MAX);
    return result;
}

trajectory_t trajectory_record_anchor(trajectory_t traj,
                                      cube_position_t cube_pos,
                                      k_align_t k_state) {
    trajectory_t result = traj;

    // Check if we have room
    if (result.anchor_path_count < TRAJECTORY_ANCHOR_PATH_MAX) {
        anchor_point_t point;
        point.position = traj.current;
        point.cube_pos = cube_pos;
        point.k_state = k_state;

        result.anchor_path[result.anchor_path_count] = point;
        result.anchor_path_count++;
    } else {
        // Path full - shift array (drop oldest)
        for (int i = 0; i < TRAJECTORY_ANCHOR_PATH_MAX - 1; i++) {
            result.anchor_path[i] = result.anchor_path[i + 1];
        }
        anchor_point_t point;
        point.position = traj.current;
        point.cube_pos = cube_pos;
        point.k_state = k_state;
        result.anchor_path[TRAJECTORY_ANCHOR_PATH_MAX - 1] = point;
    }

    return result;
}

trajectory_t trajectory_trace_back(trajectory_t traj,
                                   cube_position_t* out_cube_pos,
                                   k_align_t* out_k_state) {
    trajectory_t result = traj;

    if (traj.anchor_path_count == 0) {
        // No history to trace back
        return result;
    }

    // Get most recent anchor point
    uint8_t idx = result.anchor_path_count - 1;
    anchor_point_t* point = &result.anchor_path[idx];

    // Output values if requested
    if (out_cube_pos != NULL) {
        *out_cube_pos = point->cube_pos;
    }
    if (out_k_state != NULL) {
        *out_k_state = point->k_state;
    }

    // Move to that position
    result.current = point->position;
    result.anchor_path_count--;

    return result;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Query Functions
// ────────────────────────────────────────────────────────────────

bool trajectory_is_building(trajectory_t traj) {
    return traj.current == FW_BUILDING;
}

bool trajectory_is_grounding(trajectory_t traj) {
    return traj.current == FW_GROUNDING;
}

bool trajectory_is_pivoting(trajectory_t traj) {
    return traj.current == FW_PIVOTING_OUT || traj.current == FW_PIVOTING_IN;
}

trajectory_phase_t trajectory_get_phase(trajectory_t traj) {
    switch (traj.current) {
        case FW_BUILDING:
        case FW_PIVOTING_OUT:
            return TRAJ_PHASE_ACTIVE;

        case FW_GROUNDING:
            return TRAJ_PHASE_RESTING;

        case FW_PIVOTING_IN:
            return TRAJ_PHASE_PREPARING;

        default:
            return TRAJ_PHASE_RESTING;
    }
}

float trajectory_get_momentum(trajectory_t traj) {
    return traj.momentum;
}

float trajectory_get_direction_value(trajectory_t traj) {
    switch (traj.direction) {
        case TRAJ_BTIMESPACE: return TRAJECTORY_DIR_BTIMESPACE;  // -0.5
        case TRAJ_BSPACETIME: return TRAJECTORY_DIR_BSPACETIME;  // +0.5
        default:              return TRAJECTORY_DIR_NEUTRAL;     // 0.0
    }
}

const char* trajectory_position_name(framework_position_t pos) {
    switch (pos) {
        case FW_BUILDING:     return "BUILDING";
        case FW_PIVOTING_OUT: return "PIVOTING_OUT";
        case FW_PIVOTING_IN:  return "PIVOTING_IN";
        case FW_GROUNDING:    return "GROUNDING";
        default:              return "UNKNOWN";
    }
}

const char* trajectory_position_description(framework_position_t pos) {
    switch (pos) {
        case FW_BUILDING:
            return "B.1 - Create structure, move forward";
        case FW_PIVOTING_OUT:
            return "B.2 - Transition from building toward grounding";
        case FW_PIVOTING_IN:
            return "B.3 - Transition from grounding toward building";
        case FW_GROUNDING:
            return "B.4 - Rest, consolidate, anchor";
        default:
            return "Unknown position";
    }
}

const char* trajectory_direction_name(trajectory_direction_t dir) {
    switch (dir) {
        case TRAJ_BTIMESPACE: return "BTimespace";
        case TRAJ_NEUTRAL:    return "Neutral";
        case TRAJ_BSPACETIME: return "BSpacetime";
        default:              return "Unknown";
    }
}

bool trajectory_pivot_warning(trajectory_t traj) {
    return traj.pivot_count >= TRAJECTORY_PIVOT_WARNING;
}

int trajectory_pivots_remaining(trajectory_t traj) {
    if (traj.pivot_count >= TRAJECTORY_PIVOT_MAX) {
        return 0;
    }
    return TRAJECTORY_PIVOT_MAX - traj.pivot_count;
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// Build Verification:
//   gcc -c -Wall -Wextra -std=c99 -Iinclude src/trajectory.c -o build/trajectory.o

// ============================================================================
// END CLOSING
// ============================================================================
