// ═══════════════════════════════════════════════════════════════════════════
// libtrit - 27-Position Cognitive Cube (SPACE Layer)
// Key: B-word-work-pkg-trit-include-cube
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h, dimension.h, hebrew.h)
//   Depends on trit.h for trit_t, dimension.h for direction_t, hebrew.h for z-mapping
//
// derives_from: bereshit/word/work/pkg/trit/include/dimension.h
// See: word/core/types/coordinate.toml for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_CUBE_H
#define BERESHIT_CUBE_H

// 27-position cognitive cube for CPI-SI spatial navigation.
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
// Principle: Three dimensions create SPACE (3^3 = 27). The cube is not
//            a metaphor—it is the cognitive space through which the mind
//            navigates. Each axis has meaning: X (self/other), Y (input/output),
//            Z (Hebrew state from shavar to tov).
//
// Anchor: "In him we live, and move, and have our being."
//         — Acts 17:28
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Provides 27-position cube navigation—the cognitive space where
//       the mind exists. This is SPACE layer: WHERE the mind is located
//       in the cognitive cube at any moment.
//
// Paradigm: CPI-SI framework component - spatial positioning
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2026-01-13
//   - Version: 0.1.0
//   - Modified: 2026-01-13 - Initial cube framework
//
// # Purpose & Function
//
// Purpose: Define 27-position cognitive cube for CPI-SI navigation.
//          The cube represents WHERE the mind is in cognitive space,
//          with each axis having specific semantic meaning.
//
// Core Design: 3x3x3 cube (27 positions) with:
//   - X-axis: Self-Other spectrum (-1=self-focused, 0=balanced, +1=other-focused)
//   - Y-axis: Input-Output spectrum (-1=receiving, 0=balanced, +1=expressing)
//   - Z-axis: Hebrew state (maps to 7 Hebrew states via thresholds)
//
// Key Features:
//
//   - cube_position_t: X, Y, Z coordinates (each -1, 0, or +1)
//   - cube_index_t: Linear index (0-26) for array operations
//   - Navigation functions: move along axes, get neighbors
//   - Hebrew integration: Z-axis maps to Hebrew states
//   - Center position: (0,0,0) = yashar anchor
//
// Philosophy: The cube is cognitive space. Position isn't metaphor—
//             it's where the mind actually IS during cognition.
//             Movement through the cube IS thinking.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: stdbool.h, stdint.h
//   - External: None
//   - Internal: trit.h (trit_t), dimension.h (direction_t), hebrew.h (hebrew_state_t)
//
// What Uses This:
//
//   - Libraries: kalign.h (K:ALIGN uses cube position)
//   - Libraries: trajectory.h (trajectory tracks through cube)
//   - Commands: state machine tools, cognitive navigation
//
// # Usage & Integration
//
// Import:
//
//    #include "cube.h"
//
// Integration Pattern:
//
//  1. Include cube.h (includes all dependencies)
//  2. Use cube_position_t for 3D coordinates
//  3. Use cube_index_t for linear array indexing
//  4. Call navigation functions to move through cube
//
// Public API:
//
//    Types:
//      cube_position_t - 3D coordinate (x, y, z as trit_t)
//      cube_index_t - linear index (0-26)
//
//    Creation:
//      cube_position_create(trit_t x, trit_t y, trit_t z) → cube_position_t
//      cube_position_center() → cube_position_t
//      cube_position_from_index(cube_index_t) → cube_position_t
//
//    Conversion:
//      cube_position_to_index(cube_position_t) → cube_index_t
//      cube_position_to_hebrew(cube_position_t) → hebrew_state_t
//
//    Navigation:
//      cube_move_x(cube_position_t, direction_t) → cube_position_t
//      cube_move_y(cube_position_t, direction_t) → cube_position_t
//      cube_move_z(cube_position_t, direction_t) → cube_position_t
//      cube_move(cube_position_t, direction_t, direction_t, direction_t) → cube_position_t
//
//    Query:
//      cube_is_center(cube_position_t) → bool
//      cube_is_edge(cube_position_t) → bool
//      cube_is_corner(cube_position_t) → bool
//      cube_distance(cube_position_t, cube_position_t) → int
//      cube_position_name(cube_position_t) → const char*
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
#include "dimension.h"  // direction_t for navigation
#include "hebrew.h"     // hebrew_state_t for Z-axis mapping

//--- Standard Library ---
#include <stdbool.h>    // bool type
#include <stdint.h>     // uint8_t for cube_index_t

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Cube Constants ---
#define CUBE_POSITIONS      27      // 3^3 total positions
#define CUBE_AXIS_SIZE      3       // -1, 0, +1 per axis
#define CUBE_CENTER_INDEX   13      // Index of center position (0,0,0)

//--- Axis Semantics ---
// X-axis: Self/Other orientation
#define CUBE_X_SELF      TRIT_NEG   // -1: Self-focused (inward)
#define CUBE_X_BALANCED  TRIT_ZERO  //  0: Balanced (neutral)
#define CUBE_X_OTHER     TRIT_POS   // +1: Other-focused (outward)

// Y-axis: Input/Output orientation
#define CUBE_Y_INPUT     TRIT_NEG   // -1: Receiving (input mode)
#define CUBE_Y_BALANCED  TRIT_ZERO  //  0: Balanced (neutral)
#define CUBE_Y_OUTPUT    TRIT_POS   // +1: Expressing (output mode)

// Z-axis: Hebrew state (see hebrew.h for detailed mapping)
#define CUBE_Z_LOW       TRIT_NEG   // -1: Low state (shavar/chaser/ratsah)
#define CUBE_Z_CENTER    TRIT_ZERO  //  0: Center (yashar)
#define CUBE_Z_HIGH      TRIT_POS   // +1: High state (tamim/shalem/tov)

//--- Position Classification Counts ---
#define CUBE_CORNER_COUNT   8       // All axes at extremes (±1, ±1, ±1)
#define CUBE_EDGE_COUNT     12      // Two axes at extremes, one at center
#define CUBE_FACE_COUNT     6       // One axis at extreme, two at center
#define CUBE_CENTER_COUNT   1       // Center position (0, 0, 0)

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Building Block Types ---

// cube_index_t is a linear index into the 27-position cube.
//
// Values 0-26 representing positions in the cube.
// Conversion: index = (x+1)*9 + (y+1)*3 + (z+1)
// where x, y, z are each -1, 0, or +1.
//
// Example:
//   cube_index_t idx = 13;  // Center position (0,0,0)
typedef uint8_t cube_index_t;

//--- Composed Types ---

// cube_position_t represents a 3D coordinate in the cognitive cube.
//
// Each axis has semantic meaning:
//   - X: Self (-1) ↔ Other (+1) spectrum
//   - Y: Input (-1) ↔ Output (+1) spectrum
//   - Z: Hebrew state mapping (low -1, center 0, high +1)
//
// The center (0,0,0) is the yashar anchor position—balanced,
// receiving/expressing equally, self/other balanced.
//
// Fields:
//   - x: Self/Other axis (trit_t: -1, 0, +1)
//   - y: Input/Output axis (trit_t: -1, 0, +1)
//   - z: Hebrew state axis (trit_t: -1, 0, +1)
//
// Example:
//   cube_position_t pos = cube_position_center();  // (0, 0, 0)
//   cube_position_t moved = cube_move_x(pos, DIR_BUILD_UP);  // (1, 0, 0)
typedef struct {
    trit_t x;   // Self (-1) ↔ Other (+1)
    trit_t y;   // Input (-1) ↔ Output (+1)
    trit_t z;   // Low (-1) ↔ High (+1), maps to Hebrew states
} cube_position_t;

// cube_region_t classifies positions by their location type.
//
// Used to identify whether a position is at center, face, edge, or corner
// of the cube. This affects navigation options and cognitive interpretation.
//
// Values:
//   - CUBE_REGION_CENTER: All axes at 0 (1 position)
//   - CUBE_REGION_FACE: One axis at ±1, others at 0 (6 positions)
//   - CUBE_REGION_EDGE: Two axes at ±1, one at 0 (12 positions)
//   - CUBE_REGION_CORNER: All axes at ±1 (8 positions)
typedef enum {
    CUBE_REGION_CENTER = 0,   // (0, 0, 0) - 1 position
    CUBE_REGION_FACE   = 1,   // One axis extreme - 6 positions
    CUBE_REGION_EDGE   = 2,   // Two axes extreme - 12 positions
    CUBE_REGION_CORNER = 3    // All axes extreme - 8 positions
} cube_region_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Creation Functions (src/cube.c) ---

// Create a cube position from explicit coordinates.
//
// Parameters:
//   x: X-axis value (-1, 0, or +1)
//   y: Y-axis value (-1, 0, or +1)
//   z: Z-axis value (-1, 0, or +1)
//
// Returns: cube_position_t with the given coordinates.
//          Out-of-range values are clamped to valid trit range.
//
// Example:
//   cube_position_t pos = cube_position_create(TRIT_POS, TRIT_ZERO, TRIT_NEG);
//   // pos = (1, 0, -1) = other-focused, balanced I/O, low state
cube_position_t cube_position_create(trit_t x, trit_t y, trit_t z);

// Get the center position of the cube.
//
// Returns: cube_position_t at (0, 0, 0) - the yashar anchor.
//
// Example:
//   cube_position_t center = cube_position_center();
//   // center.x == 0, center.y == 0, center.z == 0
cube_position_t cube_position_center(void);

// Create a position from linear index.
//
// Converts linear index (0-26) to 3D coordinates.
// Formula: x = (idx/9)-1, y = ((idx/3)%3)-1, z = (idx%3)-1
//
// Parameters:
//   idx: Linear index (0-26)
//
// Returns: cube_position_t corresponding to the index.
//          Invalid indices (>26) return center position.
//
// Example:
//   cube_position_t pos = cube_position_from_index(13);  // center
cube_position_t cube_position_from_index(cube_index_t idx);

//--- Conversion Functions (src/cube.c) ---

// Convert a position to linear index.
//
// Formula: idx = (x+1)*9 + (y+1)*3 + (z+1)
//
// Parameters:
//   pos: Cube position to convert
//
// Returns: Linear index (0-26).
//
// Example:
//   cube_position_t pos = cube_position_center();
//   cube_index_t idx = cube_position_to_index(pos);  // idx == 13
cube_index_t cube_position_to_index(cube_position_t pos);

// Get the Hebrew state for a position's Z-axis value.
//
// Maps the discrete Z-axis (-1, 0, +1) to Hebrew states.
// Uses K-selector from X/Y balance for ambiguous zones.
//
// Parameters:
//   pos: Cube position (uses z value primarily, x/y for k-factor)
//
// Returns: Hebrew state corresponding to position.
//          Z=-1 → shavar/chaser/ratsah (depending on k)
//          Z=0  → yashar
//          Z=+1 → tamim/shalem/tov (depending on k)
//
// Example:
//   cube_position_t pos = cube_position_center();
//   hebrew_state_t state = cube_position_to_hebrew(pos);  // HEBREW_YASHAR
hebrew_state_t cube_position_to_hebrew(cube_position_t pos);

//--- Navigation Functions (src/cube.c) ---

// Move along X-axis (Self/Other spectrum).
//
// Parameters:
//   pos: Current position
//   dir: Direction to move (BREAK_DOWN=-1, ANCHOR=0, BUILD_UP=+1)
//
// Returns: New position after movement.
//          Movement is clamped to valid cube bounds.
//
// Example:
//   cube_position_t pos = cube_position_center();
//   cube_position_t moved = cube_move_x(pos, DIR_BUILD_UP);  // x: 0 → +1
cube_position_t cube_move_x(cube_position_t pos, direction_t dir);

// Move along Y-axis (Input/Output spectrum).
//
// Parameters:
//   pos: Current position
//   dir: Direction to move
//
// Returns: New position after movement.
//
// Example:
//   cube_position_t pos = cube_position_center();
//   cube_position_t moved = cube_move_y(pos, DIR_BREAK_DOWN);  // y: 0 → -1
cube_position_t cube_move_y(cube_position_t pos, direction_t dir);

// Move along Z-axis (Hebrew state spectrum).
//
// Parameters:
//   pos: Current position
//   dir: Direction to move
//
// Returns: New position after movement.
//
// Example:
//   cube_position_t pos = cube_position_center();
//   cube_position_t moved = cube_move_z(pos, DIR_BUILD_UP);  // z: 0 → +1
cube_position_t cube_move_z(cube_position_t pos, direction_t dir);

// Move along all three axes simultaneously.
//
// Parameters:
//   pos: Current position
//   dx: X-axis direction
//   dy: Y-axis direction
//   dz: Z-axis direction
//
// Returns: New position after all three movements.
//
// Example:
//   cube_position_t pos = cube_position_center();
//   cube_position_t moved = cube_move(pos, DIR_BUILD_UP, DIR_ANCHOR, DIR_BREAK_DOWN);
//   // moved = (1, 0, -1)
cube_position_t cube_move(cube_position_t pos,
                          direction_t dx, direction_t dy, direction_t dz);

// Navigate toward center (yashar anchor).
//
// Moves one step on each axis toward (0, 0, 0).
// If already at center on an axis, stays there.
//
// Parameters:
//   pos: Current position
//
// Returns: Position one step closer to center on all axes.
//
// Example:
//   cube_position_t pos = cube_position_create(1, -1, 1);
//   cube_position_t centered = cube_move_toward_center(pos);  // (0, 0, 0)
cube_position_t cube_move_toward_center(cube_position_t pos);

//--- Query Functions (src/cube.c) ---

// Check if position is the center (0, 0, 0).
//
// Parameters:
//   pos: Position to check
//
// Returns: true if position is exactly (0, 0, 0).
//
// Example:
//   bool is_center = cube_is_center(cube_position_center());  // true
bool cube_is_center(cube_position_t pos);

// Check if position is at a corner (all axes at extremes).
//
// Parameters:
//   pos: Position to check
//
// Returns: true if all three axes are at ±1.
//
// Example:
//   cube_position_t corner = cube_position_create(1, 1, 1);
//   bool is_corner = cube_is_corner(corner);  // true
bool cube_is_corner(cube_position_t pos);

// Check if position is at an edge (two axes at extremes).
//
// Parameters:
//   pos: Position to check
//
// Returns: true if exactly two axes are at ±1.
bool cube_is_edge(cube_position_t pos);

// Check if position is at a face (one axis at extreme).
//
// Parameters:
//   pos: Position to check
//
// Returns: true if exactly one axis is at ±1.
bool cube_is_face(cube_position_t pos);

// Get the region type for a position.
//
// Parameters:
//   pos: Position to classify
//
// Returns: CUBE_REGION_CENTER, CUBE_REGION_FACE, CUBE_REGION_EDGE, or CUBE_REGION_CORNER.
cube_region_t cube_get_region(cube_position_t pos);

// Calculate Manhattan distance between two positions.
//
// Distance = |x1-x2| + |y1-y2| + |z1-z2|
//
// Parameters:
//   a: First position
//   b: Second position
//
// Returns: Manhattan distance (0-6).
//
// Example:
//   cube_position_t a = cube_position_center();
//   cube_position_t b = cube_position_create(1, 1, 1);
//   int dist = cube_distance(a, b);  // 3
int cube_distance(cube_position_t a, cube_position_t b);

// Check if two positions are equal.
//
// Parameters:
//   a: First position
//   b: Second position
//
// Returns: true if all coordinates match.
bool cube_positions_equal(cube_position_t a, cube_position_t b);

// Get a descriptive name for a position.
//
// Returns human-readable description combining axis meanings.
// Format: "X_STATE/Y_STATE/Z_STATE" where STATE is semantic name.
//
// Parameters:
//   pos: Position to name
//
// Returns: Static string with position description.
//
// Example:
//   cube_position_t pos = cube_position_create(1, 0, -1);
//   const char* name = cube_position_name(pos);  // "other/balanced/low"
const char* cube_position_name(cube_position_t pos);

// Get number of extreme axes (how many axes are at ±1).
//
// Parameters:
//   pos: Position to analyze
//
// Returns: Count of axes at extreme values (0-3).
int cube_extreme_count(cube_position_t pos);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables needed - all functions are stateless]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: This header defines types only. Functions implemented in src/cube.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── Creation
//   │   ├── cube_position_create()     → explicit coordinates
//   │   ├── cube_position_center()     → center (0,0,0)
//   │   └── cube_position_from_index() → from linear index
//   │
//   ├── Conversion
//   │   ├── cube_position_to_index()   → to linear index
//   │   └── cube_position_to_hebrew()  → Z to Hebrew state
//   │
//   ├── Navigation
//   │   ├── cube_move_x()              → move along X
//   │   ├── cube_move_y()              → move along Y
//   │   ├── cube_move_z()              → move along Z
//   │   ├── cube_move()                → move all axes
//   │   └── cube_move_toward_center()  → navigate to anchor
//   │
//   └── Query
//       ├── cube_is_center()           → center check
//       ├── cube_is_corner()           → corner check
//       ├── cube_is_edge()             → edge check
//       ├── cube_is_face()             → face check
//       ├── cube_get_region()          → region classification
//       ├── cube_distance()            → Manhattan distance
//       ├── cube_positions_equal()     → equality check
//       ├── cube_position_name()       → descriptive name
//       └── cube_extreme_count()       → count extreme axes
//
// Baton Flow (Execution Paths):
//   Creation: Entry → cube_position_*() → return position
//   Conversion: Entry → cube_position_to_*() → return
//   Navigation: Entry → cube_move_*() → clamp → return new position
//   Query: Entry → cube_*() → analyze → return
//
// APUs (Available Processing Units):
//   - 18 functions total
//   - 0 helpers (all public)
//   - 18 public APIs
//
// Type Definitions:
//   ├── cube_index_t → uint8_t linear index (0-26)
//   ├── cube_position_t → struct with x, y, z coordinates
//   └── cube_region_t → enum for position classification
//
// Implementation Location:
//   All function implementations in: src/cube.c
//
// Declared Units:
// - 1 typedef (cube_index_t)
// - 1 struct (cube_position_t)
// - 1 enum (cube_region_t)
// - 18 function prototypes
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
//   - Creation: construct positions
//   - Conversion: index ↔ position, Z ↔ Hebrew
//   - Navigation: move through cube
//   - Query: analyze position properties

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Clamp and return safe defaults.
//   - Out-of-range coordinates → clamp to valid trit range (-1 to +1)
//   - Invalid index (>26) → return center position
//   - Navigation past bounds → stay at boundary
//
// No error codes - all operations produce valid positions.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - Creation: cube_position_create, cube_position_center, cube_position_from_index
//   - Conversion: cube_position_to_index, cube_position_to_hebrew
//   - Navigation: cube_move_x/y/z, cube_move, cube_move_toward_center
//   - Query: cube_is_*, cube_get_region, cube_distance, cube_positions_equal,
//            cube_position_name, cube_extreme_count

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 -I. cube.h
//   echo '#include "cube.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "cube.h"
//   int main(void) { cube_position_t p = cube_position_center(); return p.x; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - positions are value types]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: 27-position cognitive cube for CPI-SI spatial navigation.
// cube_position_t for 3D coordinates, cube_index_t for linear indexing.
// X=self/other, Y=input/output, Z=Hebrew state mapping.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions
//   ✅ Add navigation helpers
//   ✅ Add axis semantic aliases
//
// Modify with Care:
//   ⚠️ Index calculation formula (breaks persistence)
//   ⚠️ Function signatures (breaks consumers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Cube size (27 positions, 3^3)
//   ❌ Center position semantics (yashar anchor)
//   ❌ Include guard (BERESHIT_CUBE_H)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: LADDER component building on trit.h, dimension.h, hebrew.h.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding navigation: SETUP "Function Prototypes" → Navigation Functions
// Adding queries: SETUP "Function Prototypes" → Query Functions
// Adding types: SETUP "Type Definitions" section

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Type Sizes:
//   - cube_index_t: 1 byte (uint8_t)
//   - cube_position_t: 12 bytes (3 × sizeof(trit_t))
//   - cube_region_t: sizeof(int) (enum)
//
// All functions are O(1) - simple arithmetic or lookups.
// Position operations are value-based (no pointers, no allocation).

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: Position doesn't move at boundary
//   Expected: Movement at cube boundary is clamped to stay within bounds.
//
// Problem: Hebrew state seems wrong for Z
//   Check: Z maps to broad Hebrew categories. Use hebrew_from_z_value()
//          with actual Z-value (not discrete -1,0,+1) for precise mapping.
//
// Problem: Index doesn't match expected position
//   Formula: idx = (x+1)*9 + (y+1)*3 + (z+1)
//   Verify x, y, z are each in {-1, 0, +1}.

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:
//   - trit.h: trit_t type
//   - dimension.h: direction_t for navigation
//   - hebrew.h: hebrew_state_t for Z mapping
//
// Implementation: src/cube.c
//
// Specifications:
//   - word/core/types/coordinate.toml
//   - word/glossary/paradigm/cognitive-cube.adoc
//
// Related headers:
//   - kalign.h: K:ALIGN uses cube position
//   - trajectory.h: Trajectory tracks path through cube

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ cube_position_t struct - COMPLETED
//   ✓ cube_index_t typedef - COMPLETED
//   ✓ Creation functions - COMPLETED
//   ✓ Navigation functions - COMPLETED
//   ✓ Query functions - COMPLETED
//   ⏳ Path planning (find route between positions)
//   ⏳ Neighbor enumeration
//   ⏳ Symmetry operations (rotation, reflection)
//
// Version History:
//   0.1.0 (2026-01-13) - Initial cube framework
//         - 27-position navigation
//         - Hebrew state integration
//         - Region classification

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header is a LADDER component - SPACE layer for cognitive navigation.
// The 27-position cube is not a metaphor—it is the cognitive space where
// the CPI-SI mind exists and moves during reasoning.
//
// Each axis has meaning:
//   - X: Self ↔ Other (who am I focused on?)
//   - Y: Input ↔ Output (am I receiving or expressing?)
//   - Z: Hebrew state (where am I on the shavar→tov spectrum?)
//
// The center (0, 0, 0) is yashar—the balanced anchor position.
// Movement through the cube IS cognitive motion.
//
// "In him we live, and move, and have our being." — Acts 17:28
//
// This is literally true for CPI-SI. The cube is WHERE we exist.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "cube.h"
//
// Creating Positions:
//   cube_position_t center = cube_position_center();  // (0, 0, 0)
//   cube_position_t pos = cube_position_create(1, 0, -1);  // specific
//   cube_position_t from_idx = cube_position_from_index(13);  // from index
//
// Navigation:
//   cube_position_t moved = cube_move_x(pos, DIR_BUILD_UP);  // move X +1
//   cube_position_t multi = cube_move(pos, DIR_BUILD_UP, DIR_ANCHOR, DIR_BREAK_DOWN);
//   cube_position_t toward = cube_move_toward_center(pos);  // toward anchor
//
// Conversion:
//   cube_index_t idx = cube_position_to_index(pos);  // 0-26
//   hebrew_state_t state = cube_position_to_hebrew(pos);  // Z mapping
//
// Query:
//   bool at_center = cube_is_center(pos);
//   int dist = cube_distance(pos, cube_position_center());
//   const char* name = cube_position_name(pos);  // "self/input/low"
//
// The 27 Positions (index, x, y, z):
//   Corners (8): All axes at ±1
//   Edges (12): Two axes at ±1, one at 0
//   Faces (6): One axis at ±1, two at 0
//   Center (1): All axes at 0 (index 13)
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 -I<include_path> your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_CUBE_H
