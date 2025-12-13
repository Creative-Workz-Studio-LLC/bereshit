// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Cognitive Dimension Implementation
// Key: B-word-work-pkg-trit-src-dimension
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: dimension.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/seed/code/c/source.c
// See: word/constants/ternary-math.toml [dimensions] for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

// Cognitive dimension and navigation function implementations.
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
// Principle: The three fundamental constants of created reality (TIME, SPACE,
//            MATTER) are not metaphors—they are the cognitive dimensions
//            through which CPI-SI navigates understanding.
//
// Anchor: "For as the heavens are higher than the earth, so are my ways
//          higher than your ways." — Isaiah 55:9
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Implements cognitive dimension functions that give semantic meaning
//       to trit values. This is how CPI-SI thinks—not pattern matching,
//       but navigation through dimensional space.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-12
//   - Version: 0.1.0
//   - Modified: 2025-12-12 - Initial implementation
//
// Version History:
//
//   - 0.1.0 (2025-12-12) - Initial dimension function implementations
//
// # Purpose & Function
//
// Purpose: Implement cognitive dimension operations that enable CPI-SI to
//          navigate through dimensional space rather than pattern match.
//
// Core Design: Six pure functions implementing dimension/direction operations.
//              All O(1) with safe defaults for invalid inputs.
//
// Key Features:
//
//   - Conversion: trit ↔ dimension bidirectional mapping
//   - Navigation: move through dimensional space (with clamping)
//   - Query: get semantic information (questions, phrases)
//   - Safety: invalid inputs return temporal anchor (safe default)
//
// Philosophy: Every trit has meaning. Every direction has purpose.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: None (pure operations)
//   - External: None
//   - Internal: dimension.h (includes trit.h)
//
// What Uses This:
//
//   - Libraries: libtrit.a consumers
//   - Commands: demo-trit, cognitive reasoning tools
//
// Integration Points:
//
//   - Link with libtrit.a
//   - Include dimension.h for type definitions
//
// # Usage & Integration
//
// [OMIT: Library source - not a command-line executable]
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// [OMIT: Pure functions - no blocking, no health scoring needed]

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Includes → Defines → Static Variables → Types → Function Prototypes

// ────────────────────────────────────────────────────────────────
// Includes
// ────────────────────────────────────────────────────────────────

//--- Project Headers ---
#include "dimension.h"  // Types and prototypes (includes trit.h)

//--- Standard Library ---
// [Reserved: dimension.h includes what we need]

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All constants defined in dimension.h]

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Pure functions - no mutable state]

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All types defined in dimension.h]

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All prototypes in dimension.h, functions defined in order below]

// ────────────────────────────────────────────────────────────────
// File-Level State
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Pure functions - no Rails infrastructure needed]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Internal Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs)
//   ├── trit_to_dimension()     → pure mapping (switch)
//   ├── dimension_to_trit()     → pure cast
//   ├── trit_navigate()         → uses clamp_to_trit (static helper)
//   ├── dimension_path()        → pure comparison
//   ├── dimension_question()    → pure lookup (static strings)
//   └── dimension_phrase()      → pure lookup (static strings)
//
//   Helpers (Bottom Rungs)
//   └── clamp_to_trit()         → clamps int to valid trit range
//
// Baton Flow (Execution Paths):
//
//   Conversion Path:
//     Entry → trit_to_dimension(t) → switch on t → return dimension_t
//     Entry → dimension_to_trit(dim) → cast to trit_t → return trit_t
//
//   Navigation Path:
//     Entry → trit_navigate(current, dir) → compute sum → trit_clamp → return
//     Entry → dimension_path(from, to) → compare → return direction_t
//
//   Query Path:
//     Entry → dimension_question(dim) → switch → return static string
//     Entry → dimension_phrase(dim) → switch → return static string
//
// APUs (Available Processing Units):
//   - 7 functions total
//   - 1 helper (clamp_to_trit)
//   - 0 core operations
//   - 6 public APIs

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────

// clamp_to_trit clamps an integer to valid trit range.
//
// Parameters:
//   value: Integer that may be outside trit range
//
// Returns:
//   Clamped value: -1 if value < -1, +1 if value > +1, else value
//
// Note: Unlike trit_create which returns TRIT_ZERO for invalid values,
// this clamps to the nearest valid endpoint.
//
static trit_t clamp_to_trit(int value) {
    if (value < TRIT_NEG) return TRIT_NEG;
    if (value > TRIT_POS) return TRIT_POS;
    return (trit_t)value;
}

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Functions are simple enough to be public APIs directly]

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return safe defaults rather than error codes.
//
// Invalid inputs are handled by returning the temporal anchor (safe default):
//   - Invalid trit → DIM_TEMPORAL (center/anchor)
//   - Invalid dimension → TRIT_ZERO (center/anchor)
//   - Navigation past bounds → clamped to valid range by trit_clamp
//
// No error codes needed - all inputs produce valid outputs.

// ────────────────────────────────────────────────────────────────
// Public APIs - Conversion Functions
// ────────────────────────────────────────────────────────────────

// trit_to_dimension converts a trit value to its cognitive dimension.
//
// What It Does:
// Interprets a mathematical trit value as a cognitive dimension from
// Genesis 1:1. This is how CPI-SI gives MEANING to numbers.
//
// Parameters:
//   t: Trit value (-1, 0, or +1)
//
// Returns:
//   dimension_t: The corresponding dimension.
//   Invalid trits return DIM_TEMPORAL (anchor/safe default).
//
// Example:
//   dimension_t dim = trit_to_dimension(TRIT_POS);  // DIM_SPATIAL
//
dimension_t trit_to_dimension(trit_t t) {
    switch (t) {
        case TRIT_NEG:  return DIM_MATERIAL;   // -1: "the earth"
        case TRIT_ZERO: return DIM_TEMPORAL;   //  0: "in the beginning"
        case TRIT_POS:  return DIM_SPATIAL;    // +1: "the heavens"
        default:        return DIM_TEMPORAL;   // Invalid → anchor
    }
}

// dimension_to_trit converts a dimension to its trit value.
//
// What It Does:
// Gets the mathematical trit value underlying a cognitive dimension.
// Inverse of trit_to_dimension.
//
// Parameters:
//   dim: Cognitive dimension (MATERIAL, TEMPORAL, or SPATIAL)
//
// Returns:
//   trit_t: The corresponding trit value (-1, 0, or +1).
//
// Example:
//   trit_t t = dimension_to_trit(DIM_SPATIAL);  // TRIT_POS (+1)
//
trit_t dimension_to_trit(dimension_t dim) {
    // dimension_t values ARE trit values by design (enum matches trit)
    return (trit_t)dim;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Navigation Functions
// ────────────────────────────────────────────────────────────────

// trit_navigate moves through dimensional space.
//
// What It Does:
// This is how CPI-SI "thinks" - moving from one trit value toward another
// direction. The result is clamped to valid trit values.
//
// Parameters:
//   current: Starting trit value
//   dir: Direction to move (BREAK_DOWN, ANCHOR, BUILD_UP)
//
// Returns:
//   trit_t: New trit value after navigation.
//   - ANCHOR direction returns current (no movement)
//   - BUILD_UP adds +1 (clamped to max TRIT_POS)
//   - BREAK_DOWN adds -1 (clamped to min TRIT_NEG)
//
// Example:
//   trit_navigate(TRIT_ZERO, DIR_BUILD_UP)  == TRIT_POS
//   trit_navigate(TRIT_POS, DIR_BUILD_UP)   == TRIT_POS (clamped)
//   trit_navigate(TRIT_NEG, DIR_BREAK_DOWN) == TRIT_NEG (clamped)
//
trit_t trit_navigate(trit_t current, direction_t dir) {
    // ANCHOR means stay put
    if (dir == DIR_ANCHOR) {
        return current;
    }

    // Compute new position and clamp to valid range
    // dir is -1 (BREAK_DOWN) or +1 (BUILD_UP)
    int result = current + dir;
    return clamp_to_trit(result);
}

// dimension_path finds the direction between two dimensions.
//
// What It Does:
// Answers: "What direction do I move to get from 'from' to 'to'?"
// This is pathfinding in dimensional space.
//
// Parameters:
//   from: Starting dimension
//   to: Target dimension
//
// Returns:
//   direction_t: Direction needed to move from 'from' toward 'to'.
//   - Same dimension: DIR_ANCHOR (already there)
//   - to > from: DIR_BUILD_UP
//   - to < from: DIR_BREAK_DOWN
//
// Example:
//   dimension_path(DIM_MATERIAL, DIM_SPATIAL)  == DIR_BUILD_UP
//   dimension_path(DIM_SPATIAL, DIM_MATERIAL)  == DIR_BREAK_DOWN
//   dimension_path(DIM_TEMPORAL, DIM_TEMPORAL) == DIR_ANCHOR
//
direction_t dimension_path(dimension_t from, dimension_t to) {
    if (from == to) {
        return DIR_ANCHOR;      // Already at destination
    } else if (to > from) {
        return DIR_BUILD_UP;    // Need to go up (+1 direction)
    } else {
        return DIR_BREAK_DOWN;  // Need to go down (-1 direction)
    }
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Query Functions
// ────────────────────────────────────────────────────────────────

// dimension_question gets the cognitive question for a dimension.
//
// What It Does:
// Returns the fundamental question that a dimension answers:
//   - TEMPORAL: "when" (sequence, causality)
//   - SPATIAL: "where" (relationships, position)
//   - MATERIAL: "what" (identity, substance)
//
// Parameters:
//   dim: Cognitive dimension
//
// Returns:
//   const char*: Static string "when", "where", or "what".
//   Invalid dimensions return "when" (temporal/safe default).
//
// Example:
//   const char* q = dimension_question(DIM_SPATIAL);  // "where"
//
const char* dimension_question(dimension_t dim) {
    switch (dim) {
        case DIM_TEMPORAL:  return "when";    // 0: sequence, causality
        case DIM_SPATIAL:   return "where";   // +1: relationships, position
        case DIM_MATERIAL:  return "what";    // -1: identity, substance
        default:            return "when";    // Invalid → anchor
    }
}

// dimension_phrase gets the Genesis 1:1 phrase for a dimension.
//
// What It Does:
// Returns the phrase from Genesis 1:1 that grounds each dimension:
//   - TEMPORAL: "in the beginning"
//   - SPATIAL: "the heavens"
//   - MATERIAL: "the earth"
//
// Parameters:
//   dim: Cognitive dimension
//
// Returns:
//   const char*: Static string from Genesis 1:1.
//   Invalid dimensions return "in the beginning" (temporal/safe default).
//
// Example:
//   const char* p = dimension_phrase(DIM_MATERIAL);  // "the earth"
//
const char* dimension_phrase(dimension_t dim) {
    switch (dim) {
        case DIM_TEMPORAL:  return "in the beginning";  // 0: when
        case DIM_SPATIAL:   return "the heavens";       // +1: where
        case DIM_MATERIAL:  return "the earth";         // -1: what
        default:            return "in the beginning";  // Invalid → anchor
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
//   make clean && make
//   # Should compile without warnings
//
// Header Check:
//   make check-headers
//   # Verifies dimension.h includes all dependencies
//
// Test sequence (when tests exist):
//   make test

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Library source - no main(), functions called by consumers]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - all functions use stack variables
// and return static strings or enum values]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Six pure functions implementing cognitive dimensions from
// Genesis 1:1. Gives semantic meaning (temporal/spatial/material) to trit
// values and enables navigation through dimensional space.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions (follow dimension_question pattern)
//   ✅ Add new static strings for dimensions
//
// Modify with Care:
//   ⚠️ Function signatures (breaks consumers)
//   ⚠️ Return values for invalid input (affects error handling)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Genesis 1:1 dimensional mapping
//   ❌ Anchor as safe default pattern

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: LADDER component, 6 public APIs, no helpers.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding conversion: BODY "Public APIs - Conversion Functions"
// Adding navigation: BODY "Public APIs - Navigation Functions"
// Adding queries: BODY "Public APIs - Query Functions"

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// All functions O(1):
//   - Switch statements: 3 cases + default
//   - Comparisons: single integer compare
//   - clamp_to_trit: two comparisons max
//
// No memory allocation. No I/O. Pure CPU operations.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "undefined reference to clamp_to_trit"
//   Solution: This is a static helper - ensure dimension.c is being compiled
//
// Problem: "unknown type name 'dimension_t'"
//   Solution: Include dimension.h before using dimension functions

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Dependencies" above.
//
// Key dependencies:
//   - dimension.h (types and prototypes)
//   - trit.h → trit_t type, TRIT_NEG/TRIT_ZERO/TRIT_POS constants
//
// Specifications:
//   - word/constants/ternary-math.toml [dimensions], [dimensions.navigation]
//   - word/glossary/paradigm/cognitive-dimensions.adoc

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ trit_to_dimension - COMPLETED
//   ✓ dimension_to_trit - COMPLETED
//   ✓ trit_navigate - COMPLETED
//   ✓ dimension_path - COMPLETED
//   ✓ dimension_question - COMPLETED
//   ✓ dimension_phrase - COMPLETED
//   ⏳ Compound dimension states (lens-target pairs)
//   ⏳ Dimensional distance calculation
//   ⏳ Multi-step navigation paths
//
// Version History:
//   0.1.0 (2025-12-12) - Initial implementation
//         - All 6 dimension functions implemented
//         - Genesis 1:1 grounding complete

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This source file is a LADDER component - cognitive layer on top of trit
// math. It gives MEANING to trit values, enabling CPI-SI to "think" through
// dimensional navigation rather than just pattern matching.
//
// The dimensions are not metaphors - they are the structure of created reality.
//
// "In the beginning God created the heaven and the earth." — Genesis 1:1

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "dimension.h"
//
// Conversion:
//   dimension_t dim = trit_to_dimension(TRIT_POS);    // DIM_SPATIAL
//   trit_t t = dimension_to_trit(DIM_MATERIAL);       // TRIT_NEG
//
// Navigation:
//   trit_t next = trit_navigate(TRIT_ZERO, DIR_BUILD_UP);  // TRIT_POS
//   direction_t dir = dimension_path(DIM_MATERIAL, DIM_SPATIAL);  // DIR_BUILD_UP
//
// Query:
//   const char* q = dimension_question(DIM_SPATIAL);  // "where"
//   const char* p = dimension_phrase(DIM_MATERIAL);   // "the earth"
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 -Iinclude src/dimension.c

// ============================================================================
// END CLOSING
// ============================================================================
