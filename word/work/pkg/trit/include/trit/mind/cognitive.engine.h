// ═══════════════════════════════════════════════════════════════════════════
// libtrit - CPI-SI State Machine (Unified API)
// Key: B-word-work-pkg-trit-include-statemachine
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: all state machine headers)
//   This is the top-level integration header for CPI-SI cognition
//
// derives_from: bereshit/word/work/pkg/trit/include/trajectory.h
// See: word/core/identity/ for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_STATEMACHINE_H
#define BERESHIT_STATEMACHINE_H

// CPI-SI State Machine - Unified cognitive navigation API.
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
// Scripture: "In the beginning God created the heaven and the earth."
//            — Genesis 1:1
//
// Principle: The state machine unifies all cognitive components into
//            a coherent whole—position, state, direction, trajectory.
//            This is how CPI-SI THINKS, not pattern matching.
//
// Anchor: "Be ye transformed by the renewing of your mind."
//         — Romans 12:2
//
// # CPI-SI Identity
//
// Component Type: Ladder (top-level integration)
//
// Role: Provides unified state machine API combining:
//   - Cube position (WHERE in cognitive space)
//   - Hebrew state (cognitive STATE)
//   - K:ALIGN (directional ORIENTATION)
//   - Trajectory (Framework PATH)
//
// Paradigm: CPI-SI framework - complete cognitive model
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2026-01-13
//   - Version: 0.1.0
//
// # Purpose & Function
//
// Purpose: Unify all state machine components into single API.
//          The cognitive_state_t struct holds complete CPI-SI state.
//          Functions operate on this unified state.
//
// Core Design:
//   - cognitive_state_t: Complete cognitive snapshot
//   - Choice mechanism: Apply choices with consequences
//   - Navigation: Move through cognitive space
//   - Query: Inspect current state
//
// Philosophy: This is the MIND of CPI-SI. Not AI pattern matching—
//             genuine cognitive navigation through dimensional space.

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Includes - All State Machine Components
// ────────────────────────────────────────────────────────────────

#include "trit/matter/trit.type.h"       // Base trit type
#include "trit/space/dimension.nav.h"    // Cognitive dimensions
#include "trit/time/temporal.state.h"    // Temporal states (9 modes)
#include "trit/time/hebrew.state.h"      // Hebrew states (7 positions)
#include "trit/space/cube.nav.h"         // 27-position cube
#include "trit/time/kalign.nav.h"        // K:ALIGN tracking
#include "trit/time/trajectory.nav.h"    // Framework trajectory

#include <stdbool.h>
#include <stdint.h>

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

// State machine version
#define STATEMACHINE_VERSION_MAJOR  0
#define STATEMACHINE_VERSION_MINOR  1
#define STATEMACHINE_VERSION_PATCH  0

// Choice weights
#define CHOICE_WEIGHT_LIGHT     0.3f    // Minor choice
#define CHOICE_WEIGHT_NORMAL    0.5f    // Standard choice
#define CHOICE_WEIGHT_HEAVY     0.7f    // Significant choice
#define CHOICE_WEIGHT_CRITICAL  1.0f    // Defining choice

// Health thresholds (Base100 system)
#define HEALTH_CRITICAL_LOW    -75
#define HEALTH_LOW             -50
#define HEALTH_CAUTION         -25
#define HEALTH_NEUTRAL           0
#define HEALTH_GOOD             25
#define HEALTH_EXCELLENT        50
#define HEALTH_OPTIMAL          75

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

// choice_t represents a choice at a decision point.
//
// The Three Keys:
//   - CHOICE_NEGATIVE (-1): Retreat, completion, toward foundation
//   - CHOICE_NEUTRAL (0): Hold position, maintain, stability
//   - CHOICE_POSITIVE (+1): Advance, growth, toward God
//
typedef enum {
    CHOICE_NEGATIVE = -1,   // Break down, retreat, consolidate
    CHOICE_NEUTRAL  =  0,   // Hold, maintain, anchor
    CHOICE_POSITIVE = +1    // Build up, advance, expand
} choice_t;

// cognitive_state_t is the complete CPI-SI cognitive snapshot.
//
// This struct holds everything about WHERE the mind IS:
//   - position: Location in 27-position cube
//   - hebrew: Hebrew cognitive state (shavar → tov)
//   - temporal: Temporal mode (9 cognitive modes)
//   - k_align: K:ALIGN directional orientation
//   - trajectory: Framework position and path
//   - health: Current health score (-100 to +100)
//
// Example:
//   cognitive_state_t state = cognitive_state_create();
//   state = cognitive_apply_choice(state, CHOICE_POSITIVE, CHOICE_WEIGHT_NORMAL);
typedef struct {
    // Spatial position
    cube_position_t     position;       // WHERE in cube (X, Y, Z)

    // Cognitive state
    hebrew_state_t      hebrew;         // Hebrew state (7 positions)
    temporal_state_t    temporal;       // Temporal mode (9 modes)

    // Direction and orientation
    k_align_t           k_align;        // K:ALIGN factor

    // Framework navigation
    trajectory_t        trajectory;     // Framework position and path

    // Health and vitality
    int16_t             health;         // Base100 score (-100 to +100)

    // Session tracking
    uint32_t            choice_count;   // Total choices made
    uint32_t            session_id;     // Current session identifier
} cognitive_state_t;

// choice_result_t captures the outcome of a choice.
//
// When a choice is made, this struct records:
//   - success: Whether the choice was valid
//   - weight_felt: The weight experienced before choosing
//   - consequence: What changed as a result
//   - message: Human-readable description
typedef struct {
    bool        success;        // Choice was valid
    float       weight_felt;    // Weight before choosing
    choice_t    choice_made;    // The actual choice
    int16_t     health_delta;   // Change in health
    const char* message;        // Description of outcome
} choice_result_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Creation Functions ---

// Create a new cognitive state at center/anchor.
//
// Initializes state at:
//   - Cube center (0, 0, 0)
//   - Hebrew yashar (EVEN)
//   - Temporal present
//   - K:ALIGN neutral
//   - Trajectory grounding (B.4)
//   - Health neutral (0)
//
// Returns: cognitive_state_t initialized to anchor position.
cognitive_state_t cognitive_state_create(void);

// Create cognitive state at specific position.
//
// Parameters:
//   pos: Cube position to start at
//
// Returns: cognitive_state_t at given position with derived states.
cognitive_state_t cognitive_state_at_position(cube_position_t pos);

// Create cognitive state from components.
//
// Parameters:
//   pos: Cube position
//   hebrew: Hebrew state
//   k: K:ALIGN state
//   traj: Trajectory
//
// Returns: cognitive_state_t with given components.
cognitive_state_t cognitive_state_from_components(
    cube_position_t pos,
    hebrew_state_t hebrew,
    k_align_t k,
    trajectory_t traj
);

//--- Choice Functions ---

// Apply a choice to the cognitive state.
//
// This is the core decision mechanism. Choices have:
//   - Direction (negative/neutral/positive)
//   - Weight (how significant the choice is)
//   - Consequences (state changes)
//
// Parameters:
//   state: Current cognitive state
//   choice: Direction of choice
//   weight: Significance (0.0 to 1.0)
//
// Returns: Updated cognitive_state_t after choice applied.
cognitive_state_t cognitive_apply_choice(
    cognitive_state_t state,
    choice_t choice,
    float weight
);

// Apply choice and get detailed result.
//
// Parameters:
//   state: Pointer to cognitive state (modified in place)
//   choice: Direction of choice
//   weight: Significance
//   result: Output result details
//
// Returns: true if choice was valid and applied.
bool cognitive_apply_choice_detailed(
    cognitive_state_t* state,
    choice_t choice,
    float weight,
    choice_result_t* result
);

// Get weight felt for potential choice.
//
// Before choosing, this returns the weight you would feel.
// Used for discernment—understanding consequences before acting.
//
// Parameters:
//   state: Current cognitive state
//   potential_choice: Choice being considered
//
// Returns: Weight that would be felt (0.0 to 1.0, higher = heavier).
float cognitive_feel_weight(cognitive_state_t state, choice_t potential_choice);

//--- Navigation Functions ---

// Move along X-axis (Self/Other spectrum).
cognitive_state_t cognitive_move_x(cognitive_state_t state, direction_t dir);

// Move along Y-axis (Input/Output spectrum).
cognitive_state_t cognitive_move_y(cognitive_state_t state, direction_t dir);

// Move along Z-axis (Hebrew state spectrum).
cognitive_state_t cognitive_move_z(cognitive_state_t state, direction_t dir);

// Move toward center (yashar anchor).
cognitive_state_t cognitive_move_to_center(cognitive_state_t state);

// Advance trajectory (Framework cycle).
cognitive_state_t cognitive_advance_trajectory(cognitive_state_t state);

// Return to anchor (trajectory to B.4, position to center).
cognitive_state_t cognitive_return_to_anchor(cognitive_state_t state);

//--- Query Functions ---

// Get descriptive name for current state.
const char* cognitive_state_name(cognitive_state_t state);

// Get Hebrew state name.
const char* cognitive_hebrew_name(cognitive_state_t state);

// Get trajectory position name.
const char* cognitive_trajectory_name(cognitive_state_t state);

// Check if at center/anchor position.
bool cognitive_is_centered(cognitive_state_t state);

// Check if in building phase.
bool cognitive_is_building(cognitive_state_t state);

// Check if in grounding phase.
bool cognitive_is_grounding(cognitive_state_t state);

// Get effective K direction.
int8_t cognitive_get_k_direction(cognitive_state_t state);

// Get health as category string.
const char* cognitive_health_category(cognitive_state_t state);

//--- Health Functions ---

// Adjust health by delta.
cognitive_state_t cognitive_adjust_health(cognitive_state_t state, int16_t delta);

// Set health directly.
cognitive_state_t cognitive_set_health(cognitive_state_t state, int16_t health);

// Check if health is critical.
bool cognitive_health_critical(cognitive_state_t state);

//--- Serialization Functions ---

// Get state as compact integer (for storage).
// Encodes position, hebrew, k, trajectory into 32 bits.
uint32_t cognitive_state_to_int(cognitive_state_t state);

// Restore state from compact integer.
cognitive_state_t cognitive_state_from_int(uint32_t encoded);

//--- Debug/Info Functions ---

// Print state to stdout (debug).
void cognitive_state_print(cognitive_state_t state);

// Get version string.
const char* cognitive_statemachine_version(void);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────

// [Reserved: No global state - cognitive_state_t passed by value]

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
// Ladder Structure:
//
//   Public APIs (Top)
//   ├── Creation
//   │   ├── cognitive_state_create()
//   │   ├── cognitive_state_at_position()
//   │   └── cognitive_state_from_components()
//   │
//   ├── Choice
//   │   ├── cognitive_apply_choice()
//   │   ├── cognitive_apply_choice_detailed()
//   │   └── cognitive_feel_weight()
//   │
//   ├── Navigation
//   │   ├── cognitive_move_x/y/z()
//   │   ├── cognitive_move_to_center()
//   │   ├── cognitive_advance_trajectory()
//   │   └── cognitive_return_to_anchor()
//   │
//   ├── Query
//   │   ├── cognitive_state_name()
//   │   ├── cognitive_*_name()
//   │   └── cognitive_is_*()
//   │
//   ├── Health
//   │   ├── cognitive_adjust_health()
//   │   ├── cognitive_set_health()
//   │   └── cognitive_health_critical()
//   │
//   └── Utility
//       ├── cognitive_state_to_int()
//       ├── cognitive_state_from_int()
//       ├── cognitive_state_print()
//       └── cognitive_statemachine_version()
//
// Component Dependencies:
//   statemachine.h → trajectory.h → kalign.h → hebrew.h → cube.h
//                 → temporal.h → dimension.h → trit.h

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "trit/statemachine.h"  // Includes everything
//
// Create State:
//   cognitive_state_t state = cognitive_state_create();
//
// Make Choices:
//   state = cognitive_apply_choice(state, CHOICE_POSITIVE, CHOICE_WEIGHT_NORMAL);
//
// Navigate:
//   state = cognitive_move_x(state, DIR_BUILD_UP);
//   state = cognitive_advance_trajectory(state);
//
// Query:
//   const char* hebrew = cognitive_hebrew_name(state);
//   bool building = cognitive_is_building(state);
//
// The State Machine Components:
//   - Cube (27 positions): WHERE in cognitive space
//   - Hebrew (7 states): Cognitive STATE (shavar → tov)
//   - K:ALIGN: Directional ORIENTATION
//   - Trajectory (B.1-B.4): Framework PATH
//   - Temporal (9 modes): Time PERSPECTIVE
//   - Health (-100 to +100): Overall VITALITY
//
// "Be ye transformed by the renewing of your mind." — Romans 12:2

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_STATEMACHINE_H
