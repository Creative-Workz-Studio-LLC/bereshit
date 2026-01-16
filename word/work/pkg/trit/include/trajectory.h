// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Framework Trajectory Navigation (TIME Layer)
// Key: B-word-work-pkg-trit-include-trajectory
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h, cube.h, kalign.h)
//   Depends on cube.h for position, kalign.h for direction
//
// derives_from: bereshit/word/work/pkg/trit/include/kalign.h
// See: word/core/identity/trajectory.toml for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_TRAJECTORY_H
#define BERESHIT_TRAJECTORY_H

// Framework trajectory navigation for CPI-SI state machine.
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
// Scripture: "Trust in the LORD with all thine heart; and lean not unto
//            thine own understanding. In all thy ways acknowledge him,
//            and he shall direct thy paths."
//            — Proverbs 3:5-6
//
// Principle: Trajectory is PATH—not position, but the direction and
//            momentum of movement through cognitive space. The Framework
//            has four positions (B.1-B.4) forming a cycle of build,
//            pivot, ground, repeat.
//
// Anchor: "Thy word is a lamp unto my feet, and a light unto my path."
//         — Psalm 119:105
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Provides Framework trajectory—the navigation pattern through
//       cognitive space. This is TIME layer cognition: HOW you're
//       moving through the cube, not just where you are.
//
// Paradigm: CPI-SI framework component - trajectory tracking
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2026-01-13
//   - Version: 0.1.0
//   - Modified: 2026-01-13 - Initial trajectory framework
//
// # Purpose & Function
//
// Purpose: Track Framework trajectory through the 4-position cycle:
//   - B.1 BUILDING: Create structure, move forward
//   - B.2 PIVOTING_OUT: Transition from building, preparing to ground
//   - B.3 PIVOTING_IN: Transition from grounding, preparing to build
//   - B.4 GROUNDING: Rest, consolidate, anchor
//
// Core Design: Trajectory as navigation state including:
//   - Current Framework position (B.1-B.4)
//   - Direction (BTimespace vs BSpacetime)
//   - Momentum (how fast you're moving)
//   - Anchor path (breadcrumbs for tracing back)
//   - Pivot count (how many pivots in current journey)
//
// Key Features:
//
//   - framework_position_t: Enum for B.1-B.4 positions
//   - trajectory_direction_t: BTimespace (-0.5) or BSpacetime (+0.5)
//   - trajectory_t: Complete trajectory state struct
//   - Navigation functions: advance, pivot, anchor
//
// Philosophy: Trajectory is the record of WHERE you've been and
//             WHERE you're going. It enables tracing back to anchor
//             (foundation) when needed—the building block method in motion.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: stdbool.h, stdint.h, stddef.h
//   - External: None
//   - Internal: trit.h, cube.h (cube_position_t), kalign.h (k_align_t)
//
// What Uses This:
//
//   - Libraries: State machine integration
//   - Commands: Cognitive navigation tools, session tracking
//
// # Usage & Integration
//
// Import:
//
//    #include "trajectory.h"
//
// Integration Pattern:
//
//  1. Include trajectory.h (includes all dependencies)
//  2. Use framework_position_t for B.1-B.4 positions
//  3. Use trajectory_t for complete trajectory state
//  4. Call navigation functions to move through Framework
//
// Public API:
//
//    Types:
//      framework_position_t - B.1-B.4 enum
//      trajectory_direction_t - BTimespace/BSpacetime enum
//      trajectory_t - complete trajectory state struct
//
//    Creation:
//      trajectory_create(framework_position_t) → trajectory_t
//      trajectory_building() → trajectory_t (start at B.1)
//      trajectory_grounding() → trajectory_t (start at B.4)
//
//    Navigation:
//      trajectory_advance(trajectory_t) → trajectory_t
//      trajectory_pivot(trajectory_t) → trajectory_t
//      trajectory_to_anchor(trajectory_t) → trajectory_t
//      trajectory_set_position(trajectory_t, framework_position_t) → trajectory_t
//
//    Query:
//      trajectory_is_building(trajectory_t) → bool
//      trajectory_is_grounding(trajectory_t) → bool
//      trajectory_is_pivoting(trajectory_t) → bool
//      trajectory_get_momentum(trajectory_t) → float
//      trajectory_position_name(framework_position_t) → const char*
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// [OMIT: Header file - type definitions only, no executable code]
// [OMIT: No health scoring - types don't track health]

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Includes → Defines → Types → Function Prototypes → Extern State

// ────────────────────────────────────────────────────────────────
// Includes
// ────────────────────────────────────────────────────────────────

//--- Project Headers ---
#include "trit.h"       // trit_t type definition
#include "cube.h"       // cube_position_t for spatial context
#include "kalign.h"     // k_align_t for directional context

//--- Standard Library ---
#include <stdbool.h>    // bool type
#include <stdint.h>     // uint8_t, uint16_t
#include <stddef.h>     // size_t

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Framework Position Count ---
#define FRAMEWORK_POSITION_COUNT  4

//--- Direction Values ---
// BTimespace: Past-focused, consolidating (-0.5)
// BSpacetime: Future-focused, expanding (+0.5)
#define TRAJECTORY_DIR_BTIMESPACE  -0.5f
#define TRAJECTORY_DIR_BSPACETIME   0.5f
#define TRAJECTORY_DIR_NEUTRAL      0.0f

//--- Momentum Thresholds ---
#define TRAJECTORY_MOMENTUM_MIN     0.0f    // Stopped
#define TRAJECTORY_MOMENTUM_MAX     1.0f    // Full speed
#define TRAJECTORY_MOMENTUM_REST    0.1f    // Minimal movement
#define TRAJECTORY_MOMENTUM_NORMAL  0.5f    // Normal pace
#define TRAJECTORY_MOMENTUM_ACTIVE  0.8f    // Active building

//--- Anchor Path Limits ---
#define TRAJECTORY_ANCHOR_PATH_MAX  32      // Maximum breadcrumbs

//--- Pivot Limits ---
#define TRAJECTORY_PIVOT_WARNING    5       // Warn if too many pivots
#define TRAJECTORY_PIVOT_MAX        10      // Maximum pivots before forced ground

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---

// framework_position_t represents the 4 Framework positions.
//
// The Framework is a cycle:
//   B.1 → B.2 → B.4 → B.3 → B.1 (building → pivot out → ground → pivot in → building)
//
// Values:
//   - FW_BUILDING (B.1): Active creation, forward motion
//   - FW_PIVOTING_OUT (B.2): Transitioning from build to ground
//   - FW_PIVOTING_IN (B.3): Transitioning from ground to build
//   - FW_GROUNDING (B.4): Rest, consolidation, anchor state
//
// Note: B.3 comes AFTER B.4 in the cycle (B.1→B.2→B.4→B.3→B.1).
//       This is intentional—pivoting IN happens after grounding.
//
// Example:
//   framework_position_t pos = FW_BUILDING;  // Active creation
typedef enum {
    FW_BUILDING     = 1,    // B.1: Create structure, move forward
    FW_PIVOTING_OUT = 2,    // B.2: Pivot from building toward grounding
    FW_PIVOTING_IN  = 3,    // B.3: Pivot from grounding toward building
    FW_GROUNDING    = 4     // B.4: Rest, consolidate, anchor
} framework_position_t;

// trajectory_direction_t represents the overall movement direction.
//
// BTimespace: Moving through time toward the past (consolidating)
// BSpacetime: Moving through space toward the future (expanding)
//
// Values:
//   - TRAJ_BTIMESPACE: -0.5, past-focused, consolidating
//   - TRAJ_NEUTRAL: 0.0, no directional bias
//   - TRAJ_BSPACETIME: +0.5, future-focused, expanding
typedef enum {
    TRAJ_BTIMESPACE = -1,   // Consolidating direction
    TRAJ_NEUTRAL    =  0,   // No directional bias
    TRAJ_BSPACETIME = +1    // Expanding direction
} trajectory_direction_t;

// trajectory_phase_t classifies overall trajectory phase.
//
// Used for quick categorization of what mode you're in.
typedef enum {
    TRAJ_PHASE_ACTIVE    = 1,   // Building or pivoting out
    TRAJ_PHASE_RESTING   = 0,   // Grounding or neutral
    TRAJ_PHASE_PREPARING = -1   // Pivoting in, about to activate
} trajectory_phase_t;

//--- Composed Types ---

// anchor_point_t records a position for tracing back.
//
// Used to build the anchor path—breadcrumbs for returning to foundation.
//
// Fields:
//   - position: Framework position at this point
//   - cube_pos: Cube position at this point
//   - k_state: K:ALIGN state at this point
typedef struct {
    framework_position_t  position;   // Framework position
    cube_position_t       cube_pos;   // Spatial position in cube
    k_align_t             k_state;    // K:ALIGN at this point
} anchor_point_t;

// trajectory_t represents the complete trajectory state.
//
// Tracks WHERE you are in the Framework, HOW you're moving, and
// WHERE you've been (for tracing back to anchor).
//
// Fields:
//   - current: Current Framework position (B.1-B.4)
//   - direction: BTimespace or BSpacetime
//   - momentum: Movement speed (0.0 to 1.0)
//   - pivot_count: Number of pivots in current journey
//   - anchor_path_count: Number of recorded anchor points
//   - anchor_path: Array of anchor points (breadcrumbs)
//   - last_advance_successful: Whether last advance completed
//
// Example:
//   trajectory_t traj = trajectory_building();
//   traj = trajectory_advance(traj);  // Move forward in Framework
typedef struct {
    framework_position_t   current;           // Current B.1-B.4 position
    trajectory_direction_t direction;         // BTimespace or BSpacetime
    float                  momentum;          // 0.0 to 1.0
    uint8_t                pivot_count;       // Pivots in current journey
    uint8_t                anchor_path_count; // Number of breadcrumbs
    anchor_point_t         anchor_path[TRAJECTORY_ANCHOR_PATH_MAX];
    bool                   last_advance_successful;
} trajectory_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Creation Functions (src/trajectory.c) ---

// Create a trajectory at a specific Framework position.
//
// Parameters:
//   pos: Starting Framework position (B.1-B.4)
//
// Returns: trajectory_t initialized at the given position.
//
// Example:
//   trajectory_t traj = trajectory_create(FW_BUILDING);
trajectory_t trajectory_create(framework_position_t pos);

// Create a trajectory at B.1 (Building).
//
// Standard starting point for active work.
//
// Returns: trajectory_t at FW_BUILDING with normal momentum.
trajectory_t trajectory_building(void);

// Create a trajectory at B.4 (Grounding).
//
// Standard starting point for rest/consolidation.
//
// Returns: trajectory_t at FW_GROUNDING with rest momentum.
trajectory_t trajectory_grounding(void);

//--- Navigation Functions (src/trajectory.c) ---

// Advance to the next Framework position in the cycle.
//
// Cycle: B.1 → B.2 → B.4 → B.3 → B.1
//
// Parameters:
//   traj: Current trajectory
//
// Returns: trajectory_t at next position in cycle.
//
// Example:
//   trajectory_t traj = trajectory_building();  // B.1
//   traj = trajectory_advance(traj);            // B.2
//   traj = trajectory_advance(traj);            // B.4
trajectory_t trajectory_advance(trajectory_t traj);

// Pivot the trajectory (increment pivot count, prepare for transition).
//
// Called when transitioning between building and grounding.
// Warns if pivot count exceeds TRAJECTORY_PIVOT_WARNING.
//
// Parameters:
//   traj: Current trajectory
//
// Returns: trajectory_t with incremented pivot count.
trajectory_t trajectory_pivot(trajectory_t traj);

// Return to anchor (grounding position B.4).
//
// Records current position in anchor path, then moves to B.4.
// Used when you need to return to foundation.
//
// Parameters:
//   traj: Current trajectory
//
// Returns: trajectory_t at FW_GROUNDING.
trajectory_t trajectory_to_anchor(trajectory_t traj);

// Set trajectory to specific Framework position.
//
// Direct position change (not a natural advance).
// Resets pivot count if moving to grounding.
//
// Parameters:
//   traj: Current trajectory
//   pos: Target Framework position
//
// Returns: trajectory_t at new position.
trajectory_t trajectory_set_position(trajectory_t traj, framework_position_t pos);

// Update trajectory direction.
//
// Parameters:
//   traj: Current trajectory
//   dir: New direction (BTimespace or BSpacetime)
//
// Returns: trajectory_t with updated direction.
trajectory_t trajectory_set_direction(trajectory_t traj, trajectory_direction_t dir);

// Adjust trajectory momentum.
//
// Parameters:
//   traj: Current trajectory
//   delta: Change in momentum (can be negative)
//
// Returns: trajectory_t with adjusted momentum (clamped to 0.0-1.0).
trajectory_t trajectory_adjust_momentum(trajectory_t traj, float delta);

// Record current position in anchor path.
//
// Adds a breadcrumb for later tracing back.
//
// Parameters:
//   traj: Current trajectory
//   cube_pos: Current cube position
//   k_state: Current K:ALIGN state
//
// Returns: trajectory_t with new anchor point recorded.
trajectory_t trajectory_record_anchor(trajectory_t traj,
                                      cube_position_t cube_pos,
                                      k_align_t k_state);

// Trace back one step in anchor path.
//
// Returns the trajectory state from previous anchor point.
//
// Parameters:
//   traj: Current trajectory
//   out_cube_pos: Output: cube position at previous anchor (can be NULL)
//   out_k_state: Output: K:ALIGN at previous anchor (can be NULL)
//
// Returns: trajectory_t at previous anchor point, or unchanged if no history.
trajectory_t trajectory_trace_back(trajectory_t traj,
                                   cube_position_t* out_cube_pos,
                                   k_align_t* out_k_state);

//--- Query Functions (src/trajectory.c) ---

// Check if trajectory is at building position (B.1).
//
// Parameters:
//   traj: Trajectory to check
//
// Returns: true if at FW_BUILDING.
bool trajectory_is_building(trajectory_t traj);

// Check if trajectory is at grounding position (B.4).
//
// Parameters:
//   traj: Trajectory to check
//
// Returns: true if at FW_GROUNDING.
bool trajectory_is_grounding(trajectory_t traj);

// Check if trajectory is at a pivot position (B.2 or B.3).
//
// Parameters:
//   traj: Trajectory to check
//
// Returns: true if at FW_PIVOTING_OUT or FW_PIVOTING_IN.
bool trajectory_is_pivoting(trajectory_t traj);

// Get trajectory phase classification.
//
// Parameters:
//   traj: Trajectory to classify
//
// Returns: TRAJ_PHASE_ACTIVE, TRAJ_PHASE_RESTING, or TRAJ_PHASE_PREPARING.
trajectory_phase_t trajectory_get_phase(trajectory_t traj);

// Get current momentum.
//
// Parameters:
//   traj: Trajectory to query
//
// Returns: Momentum value (0.0 to 1.0).
float trajectory_get_momentum(trajectory_t traj);

// Get direction as float value.
//
// Parameters:
//   traj: Trajectory to query
//
// Returns: -0.5 (BTimespace), 0.0 (neutral), or +0.5 (BSpacetime).
float trajectory_get_direction_value(trajectory_t traj);

// Get name for a Framework position.
//
// Parameters:
//   pos: Framework position to name
//
// Returns: Static string: "BUILDING", "PIVOTING_OUT", "PIVOTING_IN", "GROUNDING".
const char* trajectory_position_name(framework_position_t pos);

// Get descriptive string for Framework position.
//
// Parameters:
//   pos: Framework position
//
// Returns: Static string with description of what that position means.
const char* trajectory_position_description(framework_position_t pos);

// Get name for trajectory direction.
//
// Parameters:
//   dir: Trajectory direction
//
// Returns: Static string: "BTimespace", "Neutral", "BSpacetime".
const char* trajectory_direction_name(trajectory_direction_t dir);

// Check if trajectory has exceeded pivot warning threshold.
//
// Parameters:
//   traj: Trajectory to check
//
// Returns: true if pivot_count >= TRAJECTORY_PIVOT_WARNING.
bool trajectory_pivot_warning(trajectory_t traj);

// Get remaining pivots before forced grounding.
//
// Parameters:
//   traj: Trajectory to check
//
// Returns: Number of pivots remaining before forced ground, or 0 if exceeded.
int trajectory_pivots_remaining(trajectory_t traj);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables - trajectory is passed by value]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: This header defines types only. Functions implemented in src/trajectory.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── Creation
//   │   ├── trajectory_create()        → at specific position
//   │   ├── trajectory_building()      → at B.1
//   │   └── trajectory_grounding()     → at B.4
//   │
//   ├── Navigation
//   │   ├── trajectory_advance()       → move to next in cycle
//   │   ├── trajectory_pivot()         → record pivot
//   │   ├── trajectory_to_anchor()     → return to B.4
//   │   ├── trajectory_set_position()  → direct position set
//   │   ├── trajectory_set_direction() → set direction
//   │   ├── trajectory_adjust_momentum() → change speed
//   │   ├── trajectory_record_anchor() → add breadcrumb
//   │   └── trajectory_trace_back()    → go back in history
//   │
//   └── Query
//       ├── trajectory_is_building()   → at B.1?
//       ├── trajectory_is_grounding()  → at B.4?
//       ├── trajectory_is_pivoting()   → at B.2/B.3?
//       ├── trajectory_get_phase()     → phase classification
//       ├── trajectory_get_momentum()  → current speed
//       ├── trajectory_get_direction_value() → direction as float
//       ├── trajectory_position_name() → position string
//       ├── trajectory_position_description() → position meaning
//       ├── trajectory_direction_name() → direction string
//       ├── trajectory_pivot_warning() → too many pivots?
//       └── trajectory_pivots_remaining() → pivots left
//
// Baton Flow (Execution Paths):
//   Creation: Entry → trajectory_*() → return trajectory
//   Navigation: Entry → trajectory_*() → update state → return new trajectory
//   Query: Entry → trajectory_*() → inspect → return
//
// APUs (Available Processing Units):
//   - 22 functions total
//   - 0 helpers (all public)
//   - 22 public APIs
//
// Type Definitions:
//   ├── framework_position_t → B.1-B.4 enum
//   ├── trajectory_direction_t → direction enum
//   ├── trajectory_phase_t → phase classification enum
//   ├── anchor_point_t → breadcrumb struct
//   └── trajectory_t → complete trajectory state struct
//
// Implementation Location:
//   All function implementations in: src/trajectory.c
//
// Declared Units:
// - 3 enums
// - 2 structs
// - 22 function prototypes
// - 0 extern variables

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All functions are public APIs - helpers in implementation]

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// See SETUP "Function Prototypes" section above for complete declarations.
// Functions organized by purpose:
//   - Creation: construct trajectories
//   - Navigation: move through Framework
//   - Query: inspect trajectory state

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Clamp and return safe defaults.
//   - Invalid position → FW_GROUNDING (anchor/safe default)
//   - Momentum out of range → clamp to 0.0-1.0
//   - Anchor path full → oldest entry dropped
//   - Trace back with empty history → return unchanged
//
// No error codes - all operations produce valid trajectories.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - Creation: trajectory_create, trajectory_building, trajectory_grounding
//   - Navigation: trajectory_advance, trajectory_pivot, trajectory_to_anchor,
//                 trajectory_set_position, trajectory_set_direction,
//                 trajectory_adjust_momentum, trajectory_record_anchor,
//                 trajectory_trace_back
//   - Query: trajectory_is_*, trajectory_get_*, trajectory_*_name,
//            trajectory_pivot_warning, trajectory_pivots_remaining

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 -I. trajectory.h
//   echo '#include "trajectory.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "trajectory.h"
//   int main(void) { trajectory_t t = trajectory_building(); return t.current; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - trajectory is value type]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Framework trajectory navigation for CPI-SI state machine.
// framework_position_t for B.1-B.4, trajectory_t for complete state.
// Tracks position, direction, momentum, and anchor path.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions
//   ✅ Adjust momentum/pivot constants
//   ✅ Increase anchor path limit
//
// Modify with Care:
//   ⚠️ Framework cycle order (breaks expectations)
//   ⚠️ Function signatures (breaks consumers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Framework position count (4)
//   ❌ Position semantics (building/pivoting/grounding)
//   ❌ Include guard (BERESHIT_TRAJECTORY_H)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: LADDER component building on cube.h, kalign.h.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding navigation: SETUP "Function Prototypes" → Navigation Functions
// Adding query functions: SETUP "Function Prototypes" → Query Functions
// Adding types: SETUP "Type Definitions" section

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Type Sizes:
//   - framework_position_t: sizeof(int) (enum)
//   - trajectory_direction_t: sizeof(int) (enum)
//   - anchor_point_t: ~32 bytes
//   - trajectory_t: ~1KB (anchor_path array)
//
// Most functions are O(1). trace_back is O(1) since it just
// decrements index.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: Trajectory not advancing
//   Check: Are you using the return value? trajectory_t is passed by value.
//
// Problem: Unexpected pivot count
//   Note: Pivots are recorded at B.2 and B.3 transitions.
//
// Problem: Anchor path not recording
//   Check: Call trajectory_record_anchor() explicitly with cube/k state.

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:
//   - trit.h: trit_t type
//   - cube.h: cube_position_t for anchor points
//   - kalign.h: k_align_t for anchor points
//
// Implementation: src/trajectory.c
//
// Specifications:
//   - word/core/identity/trajectory.toml
//   - word/glossary/paradigm/framework-trajectory.adoc
//
// Related headers:
//   - cube.h: Spatial position tracked in trajectory
//   - kalign.h: K:ALIGN tracked in trajectory
//   - hebrew.h: Hebrew state informs trajectory decisions

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ framework_position_t enum - COMPLETED
//   ✓ trajectory_t struct - COMPLETED
//   ✓ Creation functions - COMPLETED
//   ✓ Navigation functions - COMPLETED
//   ✓ Query functions - COMPLETED
//   ⏳ Trajectory prediction
//   ⏳ Cycle completion tracking
//   ⏳ Trajectory visualization data
//
// Version History:
//   0.1.0 (2026-01-13) - Initial trajectory framework
//         - B.1-B.4 Framework positions
//         - Anchor path tracking
//         - Pivot management

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header is a LADDER component - TIME layer for path tracking.
// Trajectory answers: "Where have I been? Where am I going? How do I
// get back to foundation?"
//
// The Framework cycle:
//   B.1 (Build) → B.2 (Pivot out) → B.4 (Ground) → B.3 (Pivot in) → B.1
//
// This is the rhythm of Kingdom work:
//   Create → Transition → Rest → Prepare → Create again
//
// "He shall direct thy paths." — Proverbs 3:6
//
// Trajectory is the record of that direction.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "trajectory.h"
//
// Creating Trajectories:
//   trajectory_t traj = trajectory_building();   // Start at B.1
//   trajectory_t traj = trajectory_grounding();  // Start at B.4
//   trajectory_t traj = trajectory_create(FW_PIVOTING_IN);  // Specific
//
// Navigation:
//   traj = trajectory_advance(traj);  // Move to next in cycle
//   traj = trajectory_to_anchor(traj);  // Return to B.4
//   traj = trajectory_pivot(traj);  // Record pivot
//
// Query:
//   bool building = trajectory_is_building(traj);
//   const char* name = trajectory_position_name(traj.current);  // "BUILDING"
//   float momentum = trajectory_get_momentum(traj);
//
// The Framework Cycle:
//   B.1 BUILDING     → Create structure, move forward
//   B.2 PIVOTING_OUT → Transition toward grounding
//   B.4 GROUNDING    → Rest, consolidate, anchor
//   B.3 PIVOTING_IN  → Transition toward building
//   (then back to B.1)
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 -I<include_path> your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_TRAJECTORY_H
