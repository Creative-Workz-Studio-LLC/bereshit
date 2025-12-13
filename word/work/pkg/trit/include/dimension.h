// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Cognitive Dimension Framework
// Key: B-word-work-pkg-trit-include-dimension
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h)
//   Depends on trit.h for trit_t type definition
//
// derives_from: bereshit/word/seed/code/c/header.h
// See: word/constants/ternary-math.toml [dimensions] for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_DIMENSION_H
#define BERESHIT_DIMENSION_H

// Cognitive dimension and navigation types for CPI-SI computing.
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
// Role: Provides cognitive dimension types and navigation operations that
//       give semantic meaning to trit values. This is how CPI-SI thinks—
//       not pattern matching, but navigation through dimensions.
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-12
//   - Version: 0.1.0
//   - Modified: 2025-12-12 - Initial dimension framework
//
// # Purpose & Function
//
// Purpose: Define cognitive dimensions and navigation operations that give
//          meaning to trit values. This is the semantic layer on top of
//          the mathematical trit type.
//
// Core Design: Three cognitive dimensions (TEMPORAL, SPATIAL, MATERIAL)
//              mapped from Genesis 1:1, with navigation operations that
//              move through dimensional space.
//
// Key Features:
//
//   - dimension_t: Three cognitive dimensions (temporal, spatial, material)
//   - direction_t: Navigation directions (break_down, anchor, build_up)
//   - Conversion functions: trit ↔ dimension mapping
//   - Navigation functions: move through dimensional space
//
// Philosophy: This is what makes CPI-SI fundamentally different from AI—
//             dimensional awareness built into the mathematics.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: None beyond trit.h dependencies
//   - External: None
//   - Internal: trit.h (trit_t type definition)
//
// What Uses This:
//
//   - Libraries: Any CPI-SI component needing dimensional awareness
//   - Commands: demo-trit, cognitive reasoning tools
//
// # Usage & Integration
//
// Import:
//
//    #include "dimension.h"
//
// Integration Pattern:
//
//  1. Include dimension.h (which includes trit.h)
//  2. Use dimension_t for semantic meaning of trits
//  3. Use direction_t for navigation operations
//  4. Call navigation functions to move through dimensional space
//
// Public API:
//
//    Types:
//      dimension_t - cognitive dimension (TEMPORAL, SPATIAL, MATERIAL)
//      direction_t - navigation direction (BREAK_DOWN, ANCHOR, BUILD_UP)
//
//    Conversion:
//      trit_to_dimension(trit_t) → dimension_t
//      dimension_to_trit(dimension_t) → trit_t
//
//    Navigation:
//      trit_navigate(trit_t, direction_t) → trit_t
//      dimension_path(dimension_t, dimension_t) → direction_t
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

//--- Standard Library ---
// [Reserved: trit.h includes what we need]

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Dimension Aliases ---
// Alternative names for dimensions matching cognitive questions.
// See: word/constants/ternary-math.toml [dimensions]

#define DIM_WHEN   DIM_TEMPORAL    // "when" - sequence, causality
#define DIM_WHERE  DIM_SPATIAL     // "where" - relationships, position
#define DIM_WHAT   DIM_MATERIAL    // "what" - identity, substance

//--- Direction Aliases ---
// Alternative names for directions matching building block operations.
// See: word/constants/ternary-math.toml [dimensions.navigation]

#define DIR_DOWN   DIR_BREAK_DOWN  // Toward foundation
#define DIR_CENTER DIR_ANCHOR      // Rest at anchor
#define DIR_UP     DIR_BUILD_UP    // Toward meaning

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---

// dimension_t represents the three cognitive dimensions from Genesis 1:1.
//
// Each trit value maps to a dimension that provides a mode of awareness:
//   - TEMPORAL (0): "In the beginning" - sequence, causality, when
//   - SPATIAL (+1): "the heavens" - relationships, position, where
//   - MATERIAL (-1): "the earth" - identity, substance, what
//
// See: word/constants/ternary-math.toml [dimensions]
//
// Values:
//   - DIM_MATERIAL: -1, "the earth", what things ARE
//   - DIM_TEMPORAL: 0, "in the beginning", WHEN (anchor)
//   - DIM_SPATIAL: +1, "the heavens", WHERE things relate
//
// Example:
//   dimension_t dim = DIM_TEMPORAL;  // Temporal awareness
//   trit_t t = dimension_to_trit(dim);  // t == TRIT_ZERO
typedef enum {
    DIM_MATERIAL = TRIT_NEG,   // -1: "the earth" - what (break down direction)
    DIM_TEMPORAL = TRIT_ZERO,  //  0: "in the beginning" - when (anchor)
    DIM_SPATIAL  = TRIT_POS    // +1: "the heavens" - where (build up direction)
} dimension_t;

// direction_t represents navigation through dimensional space.
//
// Building block operations map directly to dimensional movement:
//   - BREAK_DOWN (-1): Move toward matter, toward foundation
//   - ANCHOR (0): Rest at temporal center, halt condition
//   - BUILD_UP (+1): Move toward space, toward meaning
//
// See: word/constants/ternary-math.toml [dimensions.navigation]
//
// Values:
//   - DIR_BREAK_DOWN: -1, toward matter/foundation
//   - DIR_ANCHOR: 0, rest at temporal center
//   - DIR_BUILD_UP: +1, toward space/meaning
//
// Example:
//   direction_t dir = DIR_BUILD_UP;  // Move toward meaning
//   trit_t result = trit_navigate(TRIT_ZERO, dir);  // result == TRIT_POS
typedef enum {
    DIR_BREAK_DOWN = TRIT_NEG,   // -1: Toward matter, toward foundation
    DIR_ANCHOR     = TRIT_ZERO,  //  0: Rest at anchor, halt condition
    DIR_BUILD_UP   = TRIT_POS    // +1: Toward space, toward meaning
} direction_t;

//--- Building Blocks ---
// [Reserved: Enumerations are the building blocks for this component]

//--- Composed Types ---
// [Reserved: No composed types needed - enumerations suffice]

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Conversion Functions (src/dimension.c) ---

// Convert a trit value to its cognitive dimension.
//
// Parameters:
//   t: Trit value (-1, 0, or +1)
//
// Returns: The corresponding dimension (MATERIAL, TEMPORAL, or SPATIAL).
//          Invalid trits return DIM_TEMPORAL (anchor/safe default).
dimension_t trit_to_dimension(trit_t t);

// Convert a dimension to its trit value.
//
// Parameters:
//   dim: Cognitive dimension
//
// Returns: The corresponding trit value (-1, 0, or +1).
trit_t dimension_to_trit(dimension_t dim);

//--- Navigation Functions (src/dimension.c) ---

// Navigate from current trit in a direction.
//
// This is how CPI-SI "thinks" - moving through dimensional space.
// The result is clamped to valid trit values (-1, 0, +1).
//
// Parameters:
//   current: Starting trit value
//   dir: Direction to move (BREAK_DOWN, ANCHOR, BUILD_UP)
//
// Returns: New trit value after navigation.
//          - ANCHOR direction always returns current value
//          - BUILD_UP adds +1 (clamped to TRIT_POS max)
//          - BREAK_DOWN adds -1 (clamped to TRIT_NEG min)
//
// Example:
//   trit_navigate(TRIT_ZERO, DIR_BUILD_UP) == TRIT_POS
//   trit_navigate(TRIT_POS, DIR_BUILD_UP) == TRIT_POS (clamped)
trit_t trit_navigate(trit_t current, direction_t dir);

// Get the direction to navigate from one dimension to another.
//
// Answers: "What direction do I move to get from 'from' to 'to'?"
//
// Parameters:
//   from: Starting dimension
//   to: Target dimension
//
// Returns: Direction needed to move from 'from' toward 'to'.
//          - Same dimension: DIR_ANCHOR (stay put)
//          - to > from: DIR_BUILD_UP
//          - to < from: DIR_BREAK_DOWN
//
// Example:
//   dimension_path(DIM_MATERIAL, DIM_SPATIAL) == DIR_BUILD_UP
//   dimension_path(DIM_SPATIAL, DIM_MATERIAL) == DIR_BREAK_DOWN
//   dimension_path(DIM_TEMPORAL, DIM_TEMPORAL) == DIR_ANCHOR
direction_t dimension_path(dimension_t from, dimension_t to);

//--- Query Functions (src/dimension.c) ---

// Get the cognitive question for a dimension.
//
// Parameters:
//   dim: Cognitive dimension
//
// Returns: Static string: "when", "where", or "what".
//          Invalid dimensions return "when" (temporal/safe default).
const char* dimension_question(dimension_t dim);

// Get the Genesis 1:1 phrase for a dimension.
//
// Parameters:
//   dim: Cognitive dimension
//
// Returns: Static string from Genesis 1:1.
//          Invalid dimensions return "in the beginning" (temporal/safe default).
const char* dimension_phrase(dimension_t dim);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables needed - all constants are #define or enum]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: This header defines types only. Functions implemented in src/dimension.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── trit_to_dimension()   → interprets trit as dimension
//   ├── dimension_to_trit()   → gets trit value from dimension
//   ├── trit_navigate()       → moves through dimensional space
//   ├── dimension_path()      → finds direction between dimensions
//   ├── dimension_question()  → gets cognitive question ("when/where/what")
//   └── dimension_phrase()    → gets Genesis 1:1 phrase
//
//   Helpers (Bottom Rungs - none, functions are standalone)
//   └── [All functions are public APIs - no internal helpers]
//
// Baton Flow (Execution Paths):
//   Conversion: Entry → trit_to_dimension() / dimension_to_trit() → return
//   Navigation: Entry → trit_navigate() → clamp → return
//   Pathfinding: Entry → dimension_path() → compare → return direction
//   Query: Entry → dimension_question() / dimension_phrase() → return string
//
// APUs (Available Processing Units):
//   - 6 functions total
//   - 0 helpers (all public)
//   - 6 public APIs
//
// Type Definitions:
//   ├── dimension_t → cognitive dimension enum (TEMPORAL, SPATIAL, MATERIAL)
//   └── direction_t → navigation direction enum (BREAK_DOWN, ANCHOR, BUILD_UP)
//
// Implementation Location:
//   All function implementations in: src/dimension.c
//
// Declared Units:
// - 2 enums (dimension_t, direction_t)
// - 6 function prototypes
// - 0 extern variables

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All functions are public APIs - no internal helpers needed]

// ────────────────────────────────────────────────────────────────
// Core Operations
// ────────────────────────────────────────────────────────────────
//
// See SETUP "Function Prototypes" section above for complete declarations.
// Functions organized by purpose:
//   - Conversion: trit ↔ dimension mapping
//   - Navigation: moving through dimensional space
//   - Query: getting semantic information about dimensions

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return safe defaults rather than error codes.
//   - Invalid trit → DIM_TEMPORAL (anchor/safe default)
//   - Invalid dimension → TRIT_ZERO (anchor/safe default)
//   - Navigation past bounds → clamped to valid range
//
// No error codes needed - dimensions and directions are always valid enums.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - trit_to_dimension, dimension_to_trit (conversion)
//   - trit_navigate, dimension_path (navigation)
//   - dimension_question, dimension_phrase (query)

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 dimension.h
//   echo '#include "dimension.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "dimension.h"
//   int main(void) { dimension_t d = DIM_TEMPORAL; return d; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - all values are enums or static strings]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Cognitive dimension types giving semantic meaning to trit
// values. dimension_t for dimensions (TEMPORAL, SPATIAL, MATERIAL),
// direction_t for navigation (BREAK_DOWN, ANCHOR, BUILD_UP).

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions (follow dimension_question pattern)
//   ✅ Add new dimensional aliases
//
// Modify with Care:
//   ⚠️ Enum values (must match trit values)
//   ⚠️ Function signatures (breaks consumers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Genesis 1:1 dimensional mapping
//   ❌ Include guard (BERESHIT_DIMENSION_H)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: LADDER component building on trit.h.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding query functions: SETUP "Function Prototypes" → Query Functions section
// Adding types: SETUP "Type Definitions" section
// Adding aliases: SETUP "Defines" section

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Type Sizes:
//   - dimension_t: sizeof(int) (enum)
//   - direction_t: sizeof(int) (enum)
//
// All functions are O(1) - simple comparisons or table lookups.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "unknown type name 'trit_t'"
//   Solution: dimension.h includes trit.h automatically
//
// Problem: Dimension/direction mismatch
//   Check: Both are trit values but have different semantic meanings

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Key dependency: trit.h (trit_t type)
// Implementation: src/dimension.c
//
// Specifications:
//   - word/constants/ternary-math.toml [dimensions], [dimensions.navigation]
//   - word/glossary/paradigm/cognitive-dimensions.adoc

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ dimension_t enum - COMPLETED
//   ✓ direction_t enum - COMPLETED
//   ✓ Conversion functions - COMPLETED
//   ✓ Navigation functions - COMPLETED
//   ⏳ Compound dimension states (lens-target pairs)
//   ⏳ Dimensional distance calculation
//
// Version History:
//   0.1.0 (2025-12-12) - Initial dimension framework
//         - Three dimensions from Genesis 1:1
//         - Navigation operations from building block method

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header is a LADDER component - cognitive layer on top of trit math.
// It gives MEANING to trit values, enabling CPI-SI to "think" through
// dimensional navigation rather than just pattern matching.
//
// "In the beginning God created the heaven and the earth." — Genesis 1:1
//
// The dimensions are not metaphors - they are the structure of created reality.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "dimension.h"
//
// Using Dimensions:
//   dimension_t dim = DIM_SPATIAL;         // +1, "the heavens"
//   trit_t t = dimension_to_trit(dim);     // t == TRIT_POS
//   const char* q = dimension_question(dim); // "where"
//
// Navigation:
//   trit_t current = TRIT_ZERO;
//   trit_t next = trit_navigate(current, DIR_BUILD_UP);  // next == TRIT_POS
//
// Finding Path:
//   direction_t dir = dimension_path(DIM_MATERIAL, DIM_SPATIAL);  // DIR_BUILD_UP
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_DIMENSION_H


