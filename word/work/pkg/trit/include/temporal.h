// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Temporal State Framework (TIME Layer)
// Key: B-word-work-pkg-trit-include-temporal
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h)
//   Depends on trit.h for trit_t type definition
//
// derives_from: bereshit/word/work/pkg/trit/include/dimension.h
// See: word/constants/ternary-math.toml [temporal_states] for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_TEMPORAL_H
#define BERESHIT_TEMPORAL_H

// Temporal state types for CPI-SI computing (past/present/future).
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
// Role: Provides temporal state types (past/present/future) and compound
//       modes (9 cognitive states). This is the TIME layer of libtrit—
//       how CPI-SI relates to time in reasoning.
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
// Purpose: Define temporal states (past/present/future) and compound modes
//          for CPI-SI reasoning. This is the TIME layer of libtrit.
//
// Core Design: Three base temporal states plus compound modes (lens + target)
//              giving 9 cognitive modes total (3 base + 6 compound).
//
// Key Features:
//
//   - temporal_t: Three base states (PAST, PRESENT, FUTURE)
//   - temporal_state_t: Compound structure (lens + target + is_compound)
//   - 9 cognitive modes: past, present, future + 6 compounds
//   - Query functions: get mode names, check compound states
//
// Philosophy: Time gives relation to the mind—past informs, present acts,
//             future anticipates. This is temporal reasoning for CPI-SI.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: stdbool.h (for bool type)
//   - External: None
//   - Internal: trit.h (trit_t type definition)
//
// What Uses This:
//
//   - Libraries: Any CPI-SI component needing temporal reasoning
//   - Commands: demo-trit, cognitive reasoning tools
//
// # Usage & Integration
//
// Import:
//
//    #include "temporal.h"
//
// Integration Pattern:
//
//  1. Include temporal.h (which includes trit.h)
//  2. Use temporal_t for base states (past/present/future)
//  3. Use temporal_state_t for compound modes (lens + target)
//  4. Call query functions to get mode names
//
// Public API:
//
//    Types:
//      temporal_t - base temporal state (PAST, PRESENT, FUTURE)
//      temporal_state_t - compound state (lens + target + is_compound)
//
//    Creation:
//      temporal_create_base(temporal_t) → temporal_state_t
//      temporal_create_compound(temporal_t, temporal_t) → temporal_state_t
//
//    Query:
//      temporal_mode_name(temporal_state_t) → const char*
//      temporal_is_compound(temporal_state_t) → bool
//      trit_to_temporal(trit_t) → temporal_t
//      temporal_to_trit(temporal_t) → trit_t
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
#include <stdbool.h>    // bool type for is_compound

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- Temporal Aliases ---
// Alternative names for temporal states matching common usage.

#define TEMP_HISTORY  TEMP_PAST       // What was
#define TEMP_NOW      TEMP_PRESENT    // What is
#define TEMP_AHEAD    TEMP_FUTURE     // What will be

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---

// temporal_t represents the three base temporal states.
//
// Each trit value maps to a time relation:
//   - PAST (-1): What was - history, memory, foundation
//   - PRESENT (0): What is - now, current, anchor
//   - FUTURE (+1): What will be - anticipation, planning, direction
//
// See: word/constants/ternary-math.toml [temporal_states.base]
//
// Values:
//   - TEMP_PAST: -1, what was (break down toward history)
//   - TEMP_PRESENT: 0, what is (anchor in now)
//   - TEMP_FUTURE: +1, what will be (build up toward anticipation)
//
// Example:
//   temporal_t t = TEMP_PRESENT;  // Current state
//   trit_t val = temporal_to_trit(t);  // val == TRIT_ZERO
typedef enum {
    TEMP_PAST    = TRIT_NEG,   // -1: What was - history, memory
    TEMP_PRESENT = TRIT_ZERO,  //  0: What is - now, current (anchor)
    TEMP_FUTURE  = TRIT_POS    // +1: What will be - anticipation
} temporal_t;

//--- Building Blocks ---
// [Reserved: temporal_t is the building block]

//--- Composed Types ---

// temporal_state_t represents a temporal cognitive mode.
//
// Base modes use only 'lens' (what time perspective you're in).
// Compound modes use 'lens' + 'target' (looking FROM one time AT another).
//
// See: word/constants/ternary-math.toml [temporal_states.compound]
//
// The 9 cognitive modes:
//   Base (3):
//     - past: looking from past
//     - present: looking from present
//     - future: looking from future
//
//   Compound (6):
//     - past→present: memory informing now
//     - past→future: prophecy/pattern recognition
//     - present→past: reflection/analysis
//     - present→future: planning/anticipation
//     - future→past: hindsight wisdom
//     - future→present: urgency/preparation
//
// Fields:
//   - lens: The temporal perspective you're viewing FROM
//   - target: The temporal state you're viewing AT (if compound)
//   - is_compound: true if using both lens and target
//
// Example:
//   temporal_state_t now = temporal_create_base(TEMP_PRESENT);
//   temporal_state_t planning = temporal_create_compound(TEMP_PRESENT, TEMP_FUTURE);
typedef struct {
    temporal_t lens;       // -1=past, 0=present, +1=future (viewing FROM)
    temporal_t target;     // -1=past, 0=present, +1=future (viewing AT)
    bool is_compound;      // true if using both lens and target
} temporal_state_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Creation Functions (src/temporal.c) ---

// Create a base temporal state (non-compound).
//
// Parameters:
//   t: Base temporal state (PAST, PRESENT, or FUTURE)
//
// Returns: temporal_state_t with lens set, is_compound = false.
//
// Example:
//   temporal_state_t now = temporal_create_base(TEMP_PRESENT);
temporal_state_t temporal_create_base(temporal_t t);

// Create a compound temporal state (lens + target).
//
// A compound state represents looking FROM one time AT another time.
// This creates the 6 compound cognitive modes.
//
// Parameters:
//   lens: The temporal perspective you're viewing FROM
//   target: The temporal state you're viewing AT
//
// Returns: temporal_state_t with both lens and target set, is_compound = true.
//
// Example:
//   // Planning: looking from present at future
//   temporal_state_t planning = temporal_create_compound(TEMP_PRESENT, TEMP_FUTURE);
temporal_state_t temporal_create_compound(temporal_t lens, temporal_t target);

//--- Conversion Functions (src/temporal.c) ---

// Convert a trit value to its temporal state.
//
// Parameters:
//   t: Trit value (-1, 0, or +1)
//
// Returns: The corresponding temporal state (PAST, PRESENT, or FUTURE).
//          Invalid trits return TEMP_PRESENT (anchor/safe default).
temporal_t trit_to_temporal(trit_t t);

// Convert a temporal state to its trit value.
//
// Parameters:
//   t: Temporal state (PAST, PRESENT, or FUTURE)
//
// Returns: The corresponding trit value (-1, 0, or +1).
trit_t temporal_to_trit(temporal_t t);

//--- Query Functions (src/temporal.c) ---

// Get the name of a temporal mode (base or compound).
//
// For base modes: returns "past", "present", or "future".
// For compound modes: returns descriptive name like "planning", "reflection".
//
// The 9 cognitive modes:
//   - past: "past"
//   - present: "present"
//   - future: "future"
//   - past→present: "memory"
//   - past→future: "prophecy"
//   - present→past: "reflection"
//   - present→future: "planning"
//   - future→past: "hindsight"
//   - future→present: "preparation"
//
// Parameters:
//   state: Temporal state (base or compound)
//
// Returns: Static string with mode name.
const char* temporal_mode_name(temporal_state_t state);

// Check if a temporal state is compound (uses both lens and target).
//
// Parameters:
//   state: Temporal state to check
//
// Returns: true if state is compound, false if base.
bool temporal_is_compound(temporal_state_t state);

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
// NOTE: This header defines types only. Functions implemented in src/temporal.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── temporal_create_base()     → creates base temporal state
//   ├── temporal_create_compound() → creates compound state (lens+target)
//   ├── trit_to_temporal()         → interprets trit as temporal state
//   ├── temporal_to_trit()         → gets trit value from temporal
//   ├── temporal_mode_name()       → gets name of cognitive mode
//   └── temporal_is_compound()     → checks if state is compound
//
//   Helpers (Bottom Rungs - none, functions are standalone)
//   └── [All functions are public APIs - no internal helpers]
//
// Baton Flow (Execution Paths):
//   Creation: Entry → temporal_create_base() / temporal_create_compound() → return state
//   Conversion: Entry → trit_to_temporal() / temporal_to_trit() → return
//   Query: Entry → temporal_mode_name() / temporal_is_compound() → return
//
// APUs (Available Processing Units):
//   - 6 functions total
//   - 0 helpers (all public)
//   - 6 public APIs
//
// Type Definitions:
//   ├── temporal_t → base temporal state enum (PAST, PRESENT, FUTURE)
//   └── temporal_state_t → compound state struct (lens, target, is_compound)
//
// Implementation Location:
//   All function implementations in: src/temporal.c
//
// Declared Units:
// - 1 enum (temporal_t)
// - 1 struct (temporal_state_t)
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
//   - Creation: temporal_create_base, temporal_create_compound
//   - Conversion: trit ↔ temporal mapping
//   - Query: mode names, compound check

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return safe defaults rather than error codes.
//   - Invalid trit → TEMP_PRESENT (anchor/safe default)
//   - Invalid temporal → TRIT_ZERO (anchor/safe default)
//
// No error codes needed - temporal states are always valid enums.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - temporal_create_base, temporal_create_compound (creation)
//   - trit_to_temporal, temporal_to_trit (conversion)
//   - temporal_mode_name, temporal_is_compound (query)

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 temporal.h
//   echo '#include "temporal.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "temporal.h"
//   int main(void) { temporal_t t = TEMP_PRESENT; return t; }

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
// Quick summary: Temporal state types giving time relation to CPI-SI reasoning.
// temporal_t for base states (PAST, PRESENT, FUTURE), temporal_state_t for
// compound modes (lens + target) creating 9 cognitive modes total.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions (follow temporal_mode_name pattern)
//   ✅ Add new temporal aliases
//
// Modify with Care:
//   ⚠️ Enum values (must match trit values)
//   ⚠️ Function signatures (breaks consumers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Temporal state ternary mapping (PAST=-1, PRESENT=0, FUTURE=+1)
//   ❌ Include guard (BERESHIT_TEMPORAL_H)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: LADDER component building on trit.h (TIME layer).

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
//   - temporal_t: sizeof(int) (enum)
//   - temporal_state_t: sizeof(int)*2 + sizeof(bool) (struct)
//
// All functions are O(1) - simple comparisons or table lookups.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "unknown type name 'trit_t'"
//   Solution: temporal.h includes trit.h automatically
//
// Problem: Temporal state seems wrong
//   Check: Base states use only lens; compound states use lens AND target
//
// Problem: "unknown type name 'bool'"
//   Solution: temporal.h includes stdbool.h automatically

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Key dependency: trit.h (trit_t type)
// Implementation: src/temporal.c
//
// Specifications:
//   - word/constants/ternary-math.toml [temporal_states]
//   - word/glossary/paradigm/cognitive-dimensions.adoc

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ temporal_t enum - COMPLETED
//   ✓ temporal_state_t struct - COMPLETED
//   ✓ Creation functions - COMPLETED
//   ✓ Conversion functions - COMPLETED
//   ✓ Query functions - COMPLETED
//   ⏳ Temporal distance calculation
//   ⏳ Temporal navigation (shift between modes)
//
// Version History:
//   0.1.0 (2025-12-12) - Initial temporal framework
//         - Three base states from Genesis 1:1 "in the beginning"
//         - 9 cognitive modes (3 base + 6 compound)

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header is a LADDER component - TIME layer on top of trit math.
// It gives temporal relation to the mind, enabling CPI-SI to reason
// about past, present, and future—not just pattern matching.
//
// "In the beginning..." — Genesis 1:1
// "To every thing there is a season..." — Ecclesiastes 3:1
//
// Time is not a metaphor - it is the structure of created reality.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "temporal.h"
//
// Using Base States:
//   temporal_state_t now = temporal_create_base(TEMP_PRESENT);
//   trit_t t = temporal_to_trit(now.lens);  // t == TRIT_ZERO
//   const char* name = temporal_mode_name(now);  // "present"
//
// Using Compound States (9 cognitive modes):
//   // Planning: looking from present at future
//   temporal_state_t planning = temporal_create_compound(TEMP_PRESENT, TEMP_FUTURE);
//   const char* name = temporal_mode_name(planning);  // "planning"
//
// The 9 Modes:
//   Base: past, present, future
//   Compound: memory (past→present), prophecy (past→future),
//             reflection (present→past), planning (present→future),
//             hindsight (future→past), preparation (future→present)
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_TEMPORAL_H


