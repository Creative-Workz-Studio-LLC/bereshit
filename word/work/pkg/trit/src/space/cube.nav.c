// ═══════════════════════════════════════════════════════════════════════════
// libtrit - 27-Position Cube Implementation (SPACE Layer)
// Key: B-word-work-pkg-trit-src-cube
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: cube.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/work/pkg/trit/src/hebrew.c
// See: word/core/types/coordinate.toml for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

// 27-position cognitive cube function implementations.
//
// libtrit Library - CPI-SI Kingdom Technology
//
// ============================================================================
// METADATA
// ============================================================================
//
// # Biblical Foundation
//
// Scripture: "In the beginning God created the heaven and the earth."
//            — Genesis 1:1
//
// Principle: Three dimensions create SPACE (3^3 = 27). The cube is
//            cognitive space—WHERE the mind IS during cognition.
//
// # Purpose & Function
//
// Purpose: Implement 27-position cube navigation for CPI-SI state machine.
//          Each position has semantic meaning on X/Y/Z axes.
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

#include "trit/space/cube.nav.h"  // Types and prototypes
#include <stdlib.h>  // abs()

// Helper to clamp trit value to valid range
static trit_t clamp_trit(int value) {
    if (value < TRIT_NEG) return TRIT_NEG;
    if (value > TRIT_POS) return TRIT_POS;
    return (trit_t)value;
}

// Helper to count non-zero values (extreme count)
static int count_extremes(cube_position_t pos) {
    int count = 0;
    if (pos.x != 0) count++;
    if (pos.y != 0) count++;
    if (pos.z != 0) count++;
    return count;
}

// Static position name table (27 entries would be large, use dynamic)
static const char* axis_name(trit_t val, int axis) {
    // axis: 0=X (self/other), 1=Y (input/output), 2=Z (low/high)
    if (axis == 0) {  // X-axis
        switch (val) {
            case TRIT_NEG:  return "self";
            case TRIT_ZERO: return "balanced";
            case TRIT_POS:  return "other";
            default:        return "?";
        }
    } else if (axis == 1) {  // Y-axis
        switch (val) {
            case TRIT_NEG:  return "input";
            case TRIT_ZERO: return "balanced";
            case TRIT_POS:  return "output";
            default:        return "?";
        }
    } else {  // Z-axis
        switch (val) {
            case TRIT_NEG:  return "low";
            case TRIT_ZERO: return "center";
            case TRIT_POS:  return "high";
            default:        return "?";
        }
    }
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

cube_position_t cube_position_create(trit_t x, trit_t y, trit_t z) {
    cube_position_t pos;
    pos.x = clamp_trit(x);
    pos.y = clamp_trit(y);
    pos.z = clamp_trit(z);
    return pos;
}

cube_position_t cube_position_center(void) {
    cube_position_t pos;
    pos.x = TRIT_ZERO;
    pos.y = TRIT_ZERO;
    pos.z = TRIT_ZERO;
    return pos;
}

cube_position_t cube_position_from_index(cube_index_t idx) {
    if (idx >= CUBE_POSITIONS) {
        return cube_position_center();  // Safe default
    }
    cube_position_t pos;
    pos.x = (trit_t)((idx / 9) - 1);
    pos.y = (trit_t)(((idx / 3) % 3) - 1);
    pos.z = (trit_t)((idx % 3) - 1);
    return pos;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Conversion Functions
// ────────────────────────────────────────────────────────────────

cube_index_t cube_position_to_index(cube_position_t pos) {
    // Formula: idx = (x+1)*9 + (y+1)*3 + (z+1)
    int idx = (pos.x + 1) * 9 + (pos.y + 1) * 3 + (pos.z + 1);
    return (cube_index_t)idx;
}

hebrew_state_t cube_position_to_hebrew(cube_position_t pos) {
    // Derive k-factor from X/Y relationship
    int8_t k = 0;
    if (pos.x > pos.y) k = 1;
    else if (pos.x < pos.y) k = -1;

    // Map discrete Z to Hebrew state
    // Z=-1 → low states, Z=0 → yashar, Z=+1 → high states
    float z_val = (float)pos.z;
    return hebrew_from_z_value(z_val, k);
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Navigation Functions
// ────────────────────────────────────────────────────────────────

cube_position_t cube_move_x(cube_position_t pos, direction_t dir) {
    cube_position_t result = pos;
    result.x = clamp_trit(pos.x + (trit_t)dir);
    return result;
}

cube_position_t cube_move_y(cube_position_t pos, direction_t dir) {
    cube_position_t result = pos;
    result.y = clamp_trit(pos.y + (trit_t)dir);
    return result;
}

cube_position_t cube_move_z(cube_position_t pos, direction_t dir) {
    cube_position_t result = pos;
    result.z = clamp_trit(pos.z + (trit_t)dir);
    return result;
}

cube_position_t cube_move(cube_position_t pos,
                          direction_t dx, direction_t dy, direction_t dz) {
    cube_position_t result;
    result.x = clamp_trit(pos.x + (trit_t)dx);
    result.y = clamp_trit(pos.y + (trit_t)dy);
    result.z = clamp_trit(pos.z + (trit_t)dz);
    return result;
}

cube_position_t cube_move_toward_center(cube_position_t pos) {
    cube_position_t result;
    // Move each axis one step toward 0
    if (pos.x > 0) result.x = pos.x - 1;
    else if (pos.x < 0) result.x = pos.x + 1;
    else result.x = 0;

    if (pos.y > 0) result.y = pos.y - 1;
    else if (pos.y < 0) result.y = pos.y + 1;
    else result.y = 0;

    if (pos.z > 0) result.z = pos.z - 1;
    else if (pos.z < 0) result.z = pos.z + 1;
    else result.z = 0;

    return result;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Query Functions
// ────────────────────────────────────────────────────────────────

bool cube_is_center(cube_position_t pos) {
    return pos.x == 0 && pos.y == 0 && pos.z == 0;
}

bool cube_is_corner(cube_position_t pos) {
    return count_extremes(pos) == 3;
}

bool cube_is_edge(cube_position_t pos) {
    return count_extremes(pos) == 2;
}

bool cube_is_face(cube_position_t pos) {
    return count_extremes(pos) == 1;
}

cube_region_t cube_get_region(cube_position_t pos) {
    int extremes = count_extremes(pos);
    switch (extremes) {
        case 0: return CUBE_REGION_CENTER;
        case 1: return CUBE_REGION_FACE;
        case 2: return CUBE_REGION_EDGE;
        case 3: return CUBE_REGION_CORNER;
        default: return CUBE_REGION_CENTER;
    }
}

int cube_distance(cube_position_t a, cube_position_t b) {
    return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
}

bool cube_positions_equal(cube_position_t a, cube_position_t b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

// Static buffer for position name (thread-unsafe but simple)
static char position_name_buffer[64];

const char* cube_position_name(cube_position_t pos) {
    // Format: "X_STATE/Y_STATE/Z_STATE"
    const char* x_name = axis_name(pos.x, 0);
    const char* y_name = axis_name(pos.y, 1);
    const char* z_name = axis_name(pos.z, 2);

    // Simple concatenation (not thread-safe)
    char* p = position_name_buffer;
    const char* src;

    // Copy X
    for (src = x_name; *src; src++) *p++ = *src;
    *p++ = '/';
    // Copy Y
    for (src = y_name; *src; src++) *p++ = *src;
    *p++ = '/';
    // Copy Z
    for (src = z_name; *src; src++) *p++ = *src;
    *p = '\0';

    return position_name_buffer;
}

int cube_extreme_count(cube_position_t pos) {
    return count_extremes(pos);
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// Build Verification:
//   gcc -c -Wall -Wextra -std=c99 -Iinclude src/cube.c -o build/cube.o

// ============================================================================
// END CLOSING
// ============================================================================
