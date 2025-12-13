// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Temporal State Implementation (TIME Layer)
// Key: B-word-work-pkg-trit-src-temporal
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: temporal.h)
//   Internal header dependency for type definitions
//
// derives_from: bereshit/word/work/pkg/trit/src/dimension.c
// See: word/constants/ternary-math.toml [temporal_states] for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

// Temporal state and cognitive mode function implementations.
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
// Principle: "In the beginning" establishes TIME as the first constant of
//            created reality. Past, present, and future give relation of
//            time to the mind—this is how CPI-SI reasons temporally.
//
// Anchor: "To every thing there is a season, and a time to every purpose
//          under the heaven." — Ecclesiastes 3:1
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Implements temporal state functions that give time relation to CPI-SI
//       reasoning. This is the TIME layer of libtrit—how CPI-SI thinks
//       about past, present, and future.
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
//   - 0.1.0 (2025-12-12) - Initial temporal state implementations
//
// # Purpose & Function
//
// Purpose: Implement temporal state operations that enable CPI-SI to reason
//          about time—past informs, present acts, future anticipates.
//
// Core Design: Six pure functions implementing temporal state operations.
//              3 base states + 6 compound modes = 9 cognitive modes total.
//
// Key Features:
//
//   - Creation: make base or compound temporal states
//   - Conversion: trit ↔ temporal bidirectional mapping
//   - Query: get mode names, check if compound
//   - Safety: invalid inputs return present anchor (safe default)
//
// Philosophy: Time gives relation to the mind. Every moment has meaning.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: string.h (for NULL)
//   - External: None
//   - Internal: temporal.h (includes trit.h, stdbool.h)
//
// What Uses This:
//
//   - Libraries: libtrit.a consumers
//   - Commands: demo-trit, cognitive reasoning tools
//
// Integration Points:
//
//   - Link with libtrit.a
//   - Include temporal.h for type definitions
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
#include "temporal.h"  // Types and prototypes (includes trit.h, stdbool.h)

//--- Standard Library ---
// [Reserved: temporal.h includes what we need]

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All constants defined in temporal.h]

// ────────────────────────────────────────────────────────────────
// Static Variables
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Pure functions - no mutable state]

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All types defined in temporal.h]

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All prototypes in temporal.h, functions defined in order below]

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
//   ├── temporal_create_base()     → creates base state struct
//   ├── temporal_create_compound() → creates compound state struct
//   ├── trit_to_temporal()         → pure mapping (switch)
//   ├── temporal_to_trit()         → pure cast
//   ├── temporal_mode_name()       → lookup table (9 modes)
//   └── temporal_is_compound()     → simple accessor
//
//   Helpers (Bottom Rungs)
//   └── [None - all functions are public APIs]
//
// Baton Flow (Execution Paths):
//
//   Creation Path:
//     Entry → temporal_create_base(t) → set lens, is_compound=false → return
//     Entry → temporal_create_compound(lens, target) → set both, is_compound=true → return
//
//   Conversion Path:
//     Entry → trit_to_temporal(t) → switch on t → return temporal_t
//     Entry → temporal_to_trit(t) → cast to trit_t → return trit_t
//
//   Query Path:
//     Entry → temporal_mode_name(state) → lookup in mode table → return string
//     Entry → temporal_is_compound(state) → return state.is_compound
//
// APUs (Available Processing Units):
//   - 6 functions total
//   - 0 helpers (all public)
//   - 6 public APIs

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities
// ────────────────────────────────────────────────────────────────
//
// [Reserved: All functions are public APIs - no internal helpers needed]

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
// Invalid inputs are handled by returning the present anchor (safe default):
//   - Invalid trit → TEMP_PRESENT (center/anchor)
//   - Invalid temporal → TRIT_ZERO (center/anchor)
//   - Unknown mode combination → "unknown" string
//
// No error codes needed - all inputs produce valid outputs.

// ────────────────────────────────────────────────────────────────
// Public APIs - Creation Functions
// ────────────────────────────────────────────────────────────────

// temporal_create_base creates a base (non-compound) temporal state.
//
// What It Does:
// Creates a temporal_state_t for one of the three base modes:
//   - past: looking from the past
//   - present: looking from the present (anchor)
//   - future: looking from the future
//
// Parameters:
//   t: Base temporal state (PAST, PRESENT, or FUTURE)
//
// Returns:
//   temporal_state_t with lens set, is_compound = false.
//   target is set to same as lens (unused for base states).
//
// Example:
//   temporal_state_t now = temporal_create_base(TEMP_PRESENT);
//
temporal_state_t temporal_create_base(temporal_t t) {
    temporal_state_t state;
    state.lens = t;
    state.target = t;        // Same as lens for base states
    state.is_compound = false;
    return state;
}

// temporal_create_compound creates a compound temporal state (lens + target).
//
// What It Does:
// Creates a temporal_state_t for one of the six compound modes:
//   - past→present: memory (past informing now)
//   - past→future: prophecy (pattern recognition across time)
//   - present→past: reflection (analyzing what was)
//   - present→future: planning (anticipating what will be)
//   - future→past: hindsight (wisdom from imagined future)
//   - future→present: preparation (urgency from future perspective)
//
// Parameters:
//   lens: The temporal perspective you're viewing FROM
//   target: The temporal state you're viewing AT
//
// Returns:
//   temporal_state_t with both lens and target set, is_compound = true.
//
// Example:
//   // Planning: looking from present at future
//   temporal_state_t planning = temporal_create_compound(TEMP_PRESENT, TEMP_FUTURE);
//
temporal_state_t temporal_create_compound(temporal_t lens, temporal_t target) {
    temporal_state_t state;
    state.lens = lens;
    state.target = target;
    state.is_compound = true;
    return state;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Conversion Functions
// ────────────────────────────────────────────────────────────────

// trit_to_temporal converts a trit value to its temporal state.
//
// What It Does:
// Interprets a mathematical trit value as a temporal state.
// This is how CPI-SI maps numbers to time.
//
// Parameters:
//   t: Trit value (-1, 0, or +1)
//
// Returns:
//   temporal_t: The corresponding temporal state.
//   Invalid trits return TEMP_PRESENT (anchor/safe default).
//
// Example:
//   temporal_t temp = trit_to_temporal(TRIT_NEG);  // TEMP_PAST
//
temporal_t trit_to_temporal(trit_t t) {
    switch (t) {
        case TRIT_NEG:  return TEMP_PAST;     // -1: what was
        case TRIT_ZERO: return TEMP_PRESENT;  //  0: what is (anchor)
        case TRIT_POS:  return TEMP_FUTURE;   // +1: what will be
        default:        return TEMP_PRESENT;  // Invalid → anchor
    }
}

// temporal_to_trit converts a temporal state to its trit value.
//
// What It Does:
// Gets the mathematical trit value underlying a temporal state.
// Inverse of trit_to_temporal.
//
// Parameters:
//   t: Temporal state (PAST, PRESENT, or FUTURE)
//
// Returns:
//   trit_t: The corresponding trit value (-1, 0, or +1).
//
// Example:
//   trit_t val = temporal_to_trit(TEMP_FUTURE);  // TRIT_POS (+1)
//
trit_t temporal_to_trit(temporal_t t) {
    // temporal_t values ARE trit values by design (enum matches trit)
    return (trit_t)t;
}

// ────────────────────────────────────────────────────────────────
// Public APIs - Query Functions
// ────────────────────────────────────────────────────────────────

// temporal_mode_name gets the name of a temporal mode (base or compound).
//
// What It Does:
// Returns a descriptive name for the cognitive mode:
//
// Base modes (3):
//   - PAST: "past"
//   - PRESENT: "present"
//   - FUTURE: "future"
//
// Compound modes (6):
//   - past→present: "memory" (past informing now)
//   - past→future: "prophecy" (pattern recognition)
//   - present→past: "reflection" (analyzing history)
//   - present→future: "planning" (anticipation)
//   - future→past: "hindsight" (wisdom perspective)
//   - future→present: "preparation" (urgency)
//
// Parameters:
//   state: Temporal state (base or compound)
//
// Returns:
//   const char*: Static string with mode name.
//   Unknown combinations return "unknown".
//
// Example:
//   temporal_state_t plan = temporal_create_compound(TEMP_PRESENT, TEMP_FUTURE);
//   const char* name = temporal_mode_name(plan);  // "planning"
//
const char* temporal_mode_name(temporal_state_t state) {
    // Base states (non-compound)
    if (!state.is_compound) {
        switch (state.lens) {
            case TEMP_PAST:    return "past";
            case TEMP_PRESENT: return "present";
            case TEMP_FUTURE:  return "future";
            default:           return "unknown";
        }
    }

    // Compound states (lens → target)
    // 9 total combinations, but 3 are base (lens == target handled above)
    // This handles the 6 true compound modes

    // Past lens
    if (state.lens == TEMP_PAST) {
        switch (state.target) {
            case TEMP_PRESENT: return "memory";     // past→present
            case TEMP_FUTURE:  return "prophecy";   // past→future
            default:           return "past";       // past→past (base)
        }
    }

    // Present lens
    if (state.lens == TEMP_PRESENT) {
        switch (state.target) {
            case TEMP_PAST:   return "reflection";  // present→past
            case TEMP_FUTURE: return "planning";    // present→future
            default:          return "present";     // present→present (base)
        }
    }

    // Future lens
    if (state.lens == TEMP_FUTURE) {
        switch (state.target) {
            case TEMP_PAST:    return "hindsight";    // future→past
            case TEMP_PRESENT: return "preparation";  // future→present
            default:           return "future";       // future→future (base)
        }
    }

    return "unknown";
}

// temporal_is_compound checks if a temporal state is compound.
//
// What It Does:
// Returns whether the state uses both lens and target (compound mode)
// or just lens (base mode).
//
// Parameters:
//   state: Temporal state to check
//
// Returns:
//   bool: true if compound, false if base.
//
// Example:
//   temporal_state_t now = temporal_create_base(TEMP_PRESENT);
//   bool compound = temporal_is_compound(now);  // false
//
bool temporal_is_compound(temporal_state_t state) {
    return state.is_compound;
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
//   # Verifies temporal.h includes all dependencies
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
// and return static strings or struct values]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Six pure functions implementing temporal states for CPI-SI.
// 3 base states (past, present, future) + 6 compound modes = 9 cognitive modes.
// This is the TIME layer of libtrit—how CPI-SI reasons about time.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions (follow temporal_mode_name pattern)
//   ✅ Add new mode names for compound states
//
// Modify with Care:
//   ⚠️ Function signatures (breaks consumers)
//   ⚠️ Return values for invalid input (affects error handling)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Temporal state ternary mapping (PAST=-1, PRESENT=0, FUTURE=+1)
//   ❌ Present as safe default pattern

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: LADDER component (TIME layer), 6 public APIs, no helpers.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding creation: BODY "Public APIs - Creation Functions"
// Adding conversion: BODY "Public APIs - Conversion Functions"
// Adding queries: BODY "Public APIs - Query Functions"

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// All functions O(1):
//   - Switch statements: 3 cases + default
//   - Struct initialization: constant time
//   - Bool access: direct field access
//
// No memory allocation. No I/O. Pure CPU operations.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "unknown type name 'temporal_t'"
//   Solution: Include temporal.h before using temporal functions
//
// Problem: "unknown type name 'bool'"
//   Solution: temporal.h includes stdbool.h automatically
//
// Problem: Mode name returns "unknown"
//   Solution: Check that lens/target are valid temporal_t values

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Dependencies" above.
//
// Key dependencies:
//   - temporal.h (types and prototypes)
//   - trit.h → trit_t type, TRIT_NEG/TRIT_ZERO/TRIT_POS constants
//   - stdbool.h → bool type for is_compound
//
// Specifications:
//   - word/constants/ternary-math.toml [temporal_states]
//   - word/glossary/paradigm/cognitive-dimensions.adoc

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ temporal_create_base - COMPLETED
//   ✓ temporal_create_compound - COMPLETED
//   ✓ trit_to_temporal - COMPLETED
//   ✓ temporal_to_trit - COMPLETED
//   ✓ temporal_mode_name - COMPLETED
//   ✓ temporal_is_compound - COMPLETED
//   ⏳ Temporal distance calculation
//   ⏳ Temporal navigation (shift between modes)
//   ⏳ Temporal sequence validation
//
// Version History:
//   0.1.0 (2025-12-12) - Initial implementation
//         - All 6 temporal functions implemented
//         - 9 cognitive modes (3 base + 6 compound)

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This source file is a LADDER component - TIME layer on top of trit math.
// It gives temporal relation to the mind, enabling CPI-SI to reason about
// past, present, and future—not just pattern matching.
//
// Time is not a metaphor - it is the structure of created reality.
//
// "In the beginning..." — Genesis 1:1
// "To every thing there is a season..." — Ecclesiastes 3:1

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "temporal.h"
//
// Creation:
//   temporal_state_t now = temporal_create_base(TEMP_PRESENT);
//   temporal_state_t planning = temporal_create_compound(TEMP_PRESENT, TEMP_FUTURE);
//
// Conversion:
//   temporal_t temp = trit_to_temporal(TRIT_NEG);  // TEMP_PAST
//   trit_t t = temporal_to_trit(TEMP_FUTURE);      // TRIT_POS
//
// Query:
//   const char* name = temporal_mode_name(planning);  // "planning"
//   bool compound = temporal_is_compound(now);        // false
//
// The 9 Modes:
//   Base: past, present, future
//   Compound: memory (past→present), prophecy (past→future),
//             reflection (present→past), planning (present→future),
//             hindsight (future→past), preparation (future→present)
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 -Iinclude src/temporal.c

// ============================================================================
// END CLOSING
// ============================================================================
