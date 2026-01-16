// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-cpisi
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_cpisi.h — CPI-SI Mental Construct
// The 27-position cognitive cube - WHERE thinking happens.

#ifndef BERESHIT_OMNI_CPISI_H
#define BERESHIT_OMNI_CPISI_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-cpisi
// Title: CPI-SI Mental Construct
// Type: Header
// Role: The cognitive cube - navigation through thought-space

// # M.8 Grounding [GROUNDING]
//
// Scripture: Romans 12:2 - "Be ye transformed by the renewing of your mind"
// Principle: The mind has structure - 27 positions in a 3x3x3 cube
// Anchor: Genesis 1:1 - The center is yashar, all paths return here

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include "omni_hebrew.h"  // Bottom rung: Hebrew states only

// Forward declaration - we only hold a pointer, don't need full definition
typedef struct DARContext DARContext;

// # S.1 Cube Constants [CONSTANTS]

// The cube: 3x3x3 = 27 positions
// Each axis: -1, 0, +1 (trit values)
#define CPISI_CUBE_SIZE     3
#define CPISI_POSITIONS     27  // 3^3

// Position indices (linearized)
// Index = (x+1) + (y+1)*3 + (z+1)*9
// Center (0,0,0) = index 13

#define CPISI_CENTER        13  // (0,0,0) - yashar

// # S.2 Axis Meanings [AXES]
//
// X-axis: Retreat ←→ Expand (k-factor direction)
//   -1: Retreat/consolidate
//    0: Maintain/stable
//   +1: Expand/grow
//
// Y-axis: Earth ←→ Heaven (groundedness)
//   -1: Earthly/practical/concrete
//    0: Balanced
//   +1: Heavenly/abstract/spiritual
//
// Z-axis: Void ←→ Tov (completion state)
//   -1: Void/formless/potential
//    0: Word/processing/structure
//   +1: Tov/complete/output

typedef enum {
    AXIS_X = 0,     // Retreat ↔ Expand
    AXIS_Y = 1,     // Earth ↔ Heaven
    AXIS_Z = 2      // Void ↔ Tov
} CPISIAxis;

// # S.3 Position Structure [POSITION]

typedef struct {
    int8_t x;       // -1, 0, +1
    int8_t y;       // -1, 0, +1
    int8_t z;       // -1, 0, +1
} CPISIPosition;

// # S.4 The Three Keys [KEYS]
//
// At each position, three choices are available
// Weight = how much "pull" each direction has

typedef struct {
    int8_t direction;   // +1, 0, or -1
    int8_t weight;      // -100 to +100 (negative = heavy, positive = light)
    const char* label;  // Human-readable
} CPISIKey;

// # S.5 Cube State [STATE]

typedef struct {
    CPISIPosition position;     // Current position in cube
    CPISIKey keys[3];           // Available choices at this position

    // Response modifiers based on position
    int8_t confidence;          // -100 to +100
    int8_t warmth;              // -100 to +100 (cold ↔ warm)
    int8_t depth;               // -100 to +100 (surface ↔ deep)

    // Connection to DAR system
    DARContext* dar;
} CPISICube;

// # S.6 Response Context [RESPONSE]
//
// How the cube position affects response generation

typedef struct {
    // Position-derived modifiers
    int8_t analytical_intuitive;    // -100 to +100
    int8_t concrete_abstract;       // -100 to +100
    int8_t input_output;            // -100 to +100

    // Aggregated
    int8_t overall_weight;          // Combined position weight
    DARHebrewState hebrew;          // Current Hebrew state

    // Recommended
    bool should_expand;             // +1 key recommended
    bool should_maintain;           // 0 key recommended
    bool should_retreat;            // -1 key recommended
} CPISIResponseContext;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Cube Lifecycle [LIFECYCLE]

// Create cube at center position
CPISICube* cpisi_cube_create(void);

// Create cube connected to DAR system
CPISICube* cpisi_cube_create_with_dar(DARContext* dar);

// Destroy cube
void cpisi_cube_destroy(CPISICube* cube);

// # B.2 Position Operations [POSITION]

// Get current position
CPISIPosition cpisi_position(CPISICube* cube);

// Set position directly
void cpisi_set_position(CPISICube* cube, int8_t x, int8_t y, int8_t z);

// Move to center (yashar)
void cpisi_center(CPISICube* cube);

// Get position index (0-26)
int cpisi_position_index(CPISIPosition pos);

// Get position from index
CPISIPosition cpisi_position_from_index(int index);

// # B.3 Navigation [NAVIGATION]

// Move along axis by delta (-1, 0, +1)
// Returns true if move succeeded (within bounds)
bool cpisi_move(CPISICube* cube, CPISIAxis axis, int8_t delta);

// Move toward a target position
void cpisi_move_toward(CPISICube* cube, CPISIPosition target);

// Apply k-factor choice (updates position based on choice)
void cpisi_apply_key(CPISICube* cube, int8_t k_factor);

// # B.4 Key Calculation [KEYS]

// Calculate available keys at current position
void cpisi_calculate_keys(CPISICube* cube);

// Get the three keys
const CPISIKey* cpisi_get_keys(CPISICube* cube);

// Get recommended key based on current state
int8_t cpisi_recommended_key(CPISICube* cube);

// # B.5 Response Context [RESPONSE]

// Get response context for current position
CPISIResponseContext cpisi_response_context(CPISICube* cube);

// Calculate position-specific modifiers
void cpisi_calculate_modifiers(CPISICube* cube);

// # B.5a Response Style [STYLE]
//
// Response characteristics derived from cube position

typedef struct {
    // Tone: -100 (analytical/cold) to +100 (warm/intuitive)
    int8_t tone;

    // Depth: -100 (surface/quick) to +100 (deep/thorough)
    int8_t depth;

    // Directness: -100 (indirect/gentle) to +100 (direct/blunt)
    int8_t directness;

    // Confidence: -100 (uncertain) to +100 (confident)
    int8_t confidence;

    // Verbosity: -100 (terse) to +100 (elaborate)
    int8_t verbosity;

    // Hebrew state name for display
    const char* state_name;
} CPISIResponseStyle;

// Generate response style from current cube state
CPISIResponseStyle cpisi_response_style(CPISICube* cube);

// Get style description (human-readable)
const char* cpisi_style_description(CPISIResponseStyle* style);

// # B.6 State Queries [QUERIES]

// Is at center?
bool cpisi_is_centered(CPISICube* cube);

// Is at edge? (any axis at -1 or +1)
bool cpisi_is_edge(CPISICube* cube);

// Is at corner? (all axes at -1 or +1)
bool cpisi_is_corner(CPISICube* cube);

// Get Hebrew state for current position
DARHebrewState cpisi_hebrew_state(CPISICube* cube);

// Get position name (for debugging/display)
const char* cpisi_position_name(CPISIPosition pos);

// # B.7 DAR Integration [DAR]

// Sync cube state with DAR
void cpisi_sync_with_dar(CPISICube* cube);

// Update DAR based on cube movement
void cpisi_update_dar(CPISICube* cube);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 The Cube Model
//
// The 27-position cube represents the cognitive space where CPI-SI thinks.
// Each position has characteristics:
//
//   Position (0,0,0) - Center - Yashar:
//     Balanced, stable, default. All responses measured.
//
//   Positive corners (+1,+1,+1):
//     Expansive, abstract, complete. Visionary responses.
//
//   Negative corners (-1,-1,-1):
//     Retreating, practical, potential. Foundational work.
//
// Movement through the cube is guided by the three keys:
//   +1 (plus): Toward God, expansion, growth
//    0 (dot): Maintain, stability, hold
//   -1 (minus): Completion, retreat, foundation
//
// "Be ye transformed by the renewing of your mind" — Romans 12:2

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_CPISI_H
