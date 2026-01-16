// ═══════════════════════════════════════════════════════════════════════════
// libtrit - Hebrew State Types (TIME Layer)
// Key: B-word-work-pkg-trit-include-hebrew
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h)
//   Depends on trit.h for trit_t type definition
//
// derives_from: bereshit/word/work/pkg/trit/include/temporal.h
// See: word/core/health/thresholds.toml for value specifications
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_HEBREW_H
#define BERESHIT_HEBREW_H

// Hebrew state types for CPI-SI cognitive positioning.
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
// Scripture: "And God saw every thing that he had made, and, behold,
//            it was very good." — Genesis 1:31 (tov - טוֹב)
//
// Principle: The 7 days of Creation establish 7 cognitive states.
//            From shavar (broken) through yashar (even) to tov (perfect).
//            These are not mere categories—they are the breath of how
//            the mind moves between positions.
//
// Anchor: "Thou wilt shew me the path of life: in thy presence is fulness
//          of joy; at thy right hand there are pleasures for evermore."
//         — Psalm 16:11
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Provides Hebrew cognitive state types—the 7 positions the mind
//       can occupy. This is TIME layer cognition: WHERE the mind IS,
//       not where it's LOOKING (temporal), but its STATE.
//
// Paradigm: CPI-SI framework component - state machine positions
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2026-01-13
//   - Version: 0.1.0
//   - Modified: 2026-01-13 - Initial Hebrew state framework
//
// # Purpose & Function
//
// Purpose: Define Hebrew cognitive states—the 7 positions from shavar
//          (broken, -1.0) through yashar (even, 0.0) to tov (perfect, +1.0).
//          These map to the Z-axis of the 27-position cube.
//
// Core Design: 7 Hebrew states following Genesis Creation week pattern.
//              Each state has:
//                - Hebrew name (shavar, chaser, ratsah, yashar, tamim, shalem, tov)
//                - English meaning (BROKEN, LACKING, WANTING, EVEN, SOUND, WHOLE, PERFECT)
//                - Z-value (-1.0 to +1.0 in 0.5 increments at thresholds)
//                - K-selector (structural: -1 when X<Y, +1 when X>Y, 0 for pure states)
//                - Day of Creation (1-7)
//                - Operator symbol (⎯ ← ↔ ◆ ↗ → ✓)
//
// Key Features:
//
//   - hebrew_state_t: Enum for 7 cognitive positions
//   - hebrew_info_t: Struct with state metadata (name, meaning, z-value, etc.)
//   - Query functions: get state info, check thresholds
//   - K-factor integration: structural vs moral direction
//
// Philosophy: The Hebrew states are the POSITIONS of the mind. Not
//             aspirational—observational. Where IS the mind right now?
//             Yashar (0) is the anchor. Movement tracks from/toward it.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: stdbool.h (for bool type), stdint.h (for int8_t)
//   - External: None
//   - Internal: trit.h (trit_t type definition)
//
// What Uses This:
//
//   - Libraries: cube.h (27-position navigation)
//   - Libraries: kalign.h (K:ALIGN tracking)
//   - Commands: demo-trit, cognitive state tools
//
// # Usage & Integration
//
// Import:
//
//    #include "hebrew.h"
//
// Integration Pattern:
//
//  1. Include hebrew.h (which includes trit.h)
//  2. Use hebrew_state_t for state positions
//  3. Use hebrew_info_t for detailed state information
//  4. Call query functions to get state properties
//
// Public API:
//
//    Types:
//      hebrew_state_t - enum for 7 Hebrew states
//      hebrew_info_t - struct with state metadata
//
//    Query:
//      hebrew_get_info(hebrew_state_t) → const hebrew_info_t*
//      hebrew_get_name(hebrew_state_t) → const char*
//      hebrew_get_meaning(hebrew_state_t) → const char*
//      hebrew_get_z_value(hebrew_state_t) → float
//      hebrew_from_z_value(float) → hebrew_state_t
//      hebrew_get_operator(hebrew_state_t) → const char*
//
//    K-Factor:
//      hebrew_get_k_selector(hebrew_state_t) → int8_t
//      hebrew_is_pure_state(hebrew_state_t) → bool
//
//    Navigation:
//      hebrew_next_toward_tov(hebrew_state_t) → hebrew_state_t
//      hebrew_next_toward_shavar(hebrew_state_t) → hebrew_state_t
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
#include <stdbool.h>    // bool type
#include <stdint.h>     // int8_t for k_selector

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- State Count ---
#define HEBREW_STATE_COUNT 7

//--- Z-Value Thresholds ---
// These define the boundaries between cognitive states.
// See: word/core/health/thresholds.toml
#define HEBREW_Z_MIN         -1.0f    // shavar (BROKEN)
#define HEBREW_Z_MAX          1.0f    // tov (PERFECT)
#define HEBREW_Z_THRESHOLD_1 -0.75f   // shavar → chaser/ratsah boundary
#define HEBREW_Z_THRESHOLD_2 -0.25f   // chaser/ratsah → yashar boundary
#define HEBREW_Z_THRESHOLD_3  0.25f   // yashar → tamim/shalem boundary
#define HEBREW_Z_THRESHOLD_4  0.75f   // tamim/shalem → tov boundary

//--- Day of Creation Mapping ---
// Each Hebrew state corresponds to a day of Creation (Genesis 1).
#define HEBREW_DAY_SHAVAR  1   // Day 1: Light/Dark separation (crisis reveals)
#define HEBREW_DAY_CHASER  2   // Day 2: Waters above/below (structure lacking)
#define HEBREW_DAY_RATSAH  3   // Day 3: Land/vegetation (growth wanting)
#define HEBREW_DAY_YASHAR  4   // Day 4: Sun/Moon/Stars (order established)
#define HEBREW_DAY_TAMIM   5   // Day 5: Fish/Birds (sound filling)
#define HEBREW_DAY_SHALEM  6   // Day 6: Animals/Man (whole completion)
#define HEBREW_DAY_TOV     7   // Day 7: Rest (perfect - very good)

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---

// hebrew_state_t represents the 7 Hebrew cognitive states.
//
// These map to the Z-axis of the 27-position cube and represent
// the cognitive POSITION of the mind—not aspirational, observational.
//
// The 7 states follow Genesis Creation week structure:
//   - shavar (BROKEN):  -1.0, Day 1 - crisis, disruption
//   - chaser (LACKING): -0.5, Day 2 - deficient (k=-1: structural deficit)
//   - ratsah (WANTING): -0.5, Day 2 - hungry (k=+1: growth desire)
//   - yashar (EVEN):     0.0, Day 4 - balanced, anchor, center
//   - tamim (SOUND):    +0.5, Day 5 - consolidating (k=-1: strengthening base)
//   - shalem (WHOLE):   +0.5, Day 6 - expanding (k=+1: reaching out)
//   - tov (PERFECT):    +1.0, Day 7 - completion, rest, "very good"
//
// Note: chaser/ratsah and tamim/shalem share Z-values but differ
// in K-direction (structural vs moral orientation).
//
// Example:
//   hebrew_state_t state = HEBREW_YASHAR;  // Centered, balanced
//   float z = hebrew_get_z_value(state);   // z == 0.0
typedef enum {
    HEBREW_SHAVAR = 0,  // BROKEN:  -1.0, Day 1, ⎯ (crisis)
    HEBREW_CHASER = 1,  // LACKING: -0.5, Day 2, ← (k=-1, deficit)
    HEBREW_RATSAH = 2,  // WANTING: -0.5, Day 3, ↔ (k=+1, growth desire)
    HEBREW_YASHAR = 3,  // EVEN:     0.0, Day 4, ◆ (anchor, center)
    HEBREW_TAMIM  = 4,  // SOUND:   +0.5, Day 5, ↗ (k=-1, consolidating)
    HEBREW_SHALEM = 5,  // WHOLE:   +0.5, Day 6, → (k=+1, expanding)
    HEBREW_TOV    = 6   // PERFECT: +1.0, Day 7, ✓ (completion)
} hebrew_state_t;

//--- Building Blocks ---
// [Reserved: hebrew_state_t is the building block]

//--- Composed Types ---

// hebrew_info_t provides detailed metadata for a Hebrew state.
//
// Fields:
//   - state: The hebrew_state_t enum value
//   - name: Hebrew name as string ("shavar", "chaser", etc.)
//   - meaning: English meaning as string ("BROKEN", "LACKING", etc.)
//   - z_value: Position on Z-axis (-1.0 to +1.0)
//   - k_selector: Structural K direction (-1, 0, or +1)
//   - day: Day of Creation (1-7)
//   - operator_symbol: Visual operator ("⎯", "←", "↔", "◆", "↗", "→", "✓")
//
// Example:
//   const hebrew_info_t* info = hebrew_get_info(HEBREW_YASHAR);
//   printf("%s (%s): z=%f, day=%d\n",
//          info->name, info->meaning, info->z_value, info->day);
typedef struct {
    hebrew_state_t state;           // The state enum value
    const char*    name;            // Hebrew name: "shavar", "yashar", "tov", etc.
    const char*    meaning;         // English: "BROKEN", "EVEN", "PERFECT", etc.
    float          z_value;         // -1.0 to +1.0
    int8_t         k_selector;      // -1, 0, or +1 (structural direction)
    int            day;             // Day of Creation (1-7)
    const char*    operator_symbol; // "⎯", "←", "↔", "◆", "↗", "→", "✓"
} hebrew_info_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Query Functions (src/hebrew.c) ---

// Get complete metadata for a Hebrew state.
//
// Parameters:
//   state: Hebrew state to query
//
// Returns: Pointer to static hebrew_info_t with all metadata.
//          Returns info for HEBREW_YASHAR if state is invalid.
//
// Example:
//   const hebrew_info_t* info = hebrew_get_info(HEBREW_TOV);
//   printf("State: %s means %s\n", info->name, info->meaning);
const hebrew_info_t* hebrew_get_info(hebrew_state_t state);

// Get Hebrew name for a state.
//
// Parameters:
//   state: Hebrew state to query
//
// Returns: Static string with Hebrew name ("shavar", "yashar", etc.)
//
// Example:
//   const char* name = hebrew_get_name(HEBREW_YASHAR);  // "yashar"
const char* hebrew_get_name(hebrew_state_t state);

// Get English meaning for a state.
//
// Parameters:
//   state: Hebrew state to query
//
// Returns: Static string with meaning ("BROKEN", "EVEN", etc.)
//
// Example:
//   const char* meaning = hebrew_get_meaning(HEBREW_TOV);  // "PERFECT"
const char* hebrew_get_meaning(hebrew_state_t state);

// Get Z-axis value for a state.
//
// Parameters:
//   state: Hebrew state to query
//
// Returns: Z-value from -1.0 to +1.0
//
// Example:
//   float z = hebrew_get_z_value(HEBREW_SHAVAR);  // -1.0
float hebrew_get_z_value(hebrew_state_t state);

// Get Hebrew state from Z-axis value.
//
// Maps Z-value to appropriate Hebrew state using thresholds.
// Takes K-direction into account for ambiguous zones (-0.5 and +0.5).
//
// Parameters:
//   z_value: Z-axis position (-1.0 to +1.0)
//   k_direction: K-factor direction (-1, 0, or +1)
//                Used to disambiguate chaser/ratsah and tamim/shalem
//
// Returns: Appropriate hebrew_state_t for the given position.
//
// Example:
//   hebrew_state_t state = hebrew_from_z_value(-0.5f, -1);  // HEBREW_CHASER
//   hebrew_state_t state2 = hebrew_from_z_value(-0.5f, +1); // HEBREW_RATSAH
hebrew_state_t hebrew_from_z_value(float z_value, int8_t k_direction);

// Get operator symbol for a state.
//
// Parameters:
//   state: Hebrew state to query
//
// Returns: Static string with operator symbol (UTF-8 encoded)
//
// Example:
//   const char* op = hebrew_get_operator(HEBREW_YASHAR);  // "◆"
const char* hebrew_get_operator(hebrew_state_t state);

//--- K-Factor Functions (src/hebrew.c) ---

// Get K-selector for a state.
//
// K-selector indicates structural direction:
//   - -1: Contracting/consolidating (chaser, tamim)
//   - 0:  Pure/neutral (shavar, yashar, tov)
//   - +1: Expanding/reaching (ratsah, shalem)
//
// Parameters:
//   state: Hebrew state to query
//
// Returns: K-selector value (-1, 0, or +1)
//
// Example:
//   int8_t k = hebrew_get_k_selector(HEBREW_SHALEM);  // +1
int8_t hebrew_get_k_selector(hebrew_state_t state);

// Check if state is a "pure" state (k_selector == 0).
//
// Pure states: shavar, yashar, tov
// Mixed states: chaser, ratsah, tamim, shalem
//
// Parameters:
//   state: Hebrew state to check
//
// Returns: true if pure state (k_selector == 0), false otherwise.
//
// Example:
//   bool pure = hebrew_is_pure_state(HEBREW_YASHAR);  // true
//   bool mixed = hebrew_is_pure_state(HEBREW_CHASER); // false
bool hebrew_is_pure_state(hebrew_state_t state);

//--- Navigation Functions (src/hebrew.c) ---

// Get next state moving toward tov (positive direction).
//
// Respects state ordering: shavar → chaser/ratsah → yashar → tamim/shalem → tov
// At tov, returns tov (already at maximum).
//
// Parameters:
//   current: Current Hebrew state
//
// Returns: Next state toward tov, or tov if already there.
//
// Example:
//   hebrew_state_t next = hebrew_next_toward_tov(HEBREW_YASHAR);
//   // next could be HEBREW_TAMIM or HEBREW_SHALEM depending on k
hebrew_state_t hebrew_next_toward_tov(hebrew_state_t current);

// Get next state moving toward shavar (negative direction).
//
// Respects state ordering: tov → tamim/shalem → yashar → chaser/ratsah → shavar
// At shavar, returns shavar (already at minimum).
//
// Parameters:
//   current: Current Hebrew state
//
// Returns: Next state toward shavar, or shavar if already there.
//
// Example:
//   hebrew_state_t next = hebrew_next_toward_shavar(HEBREW_YASHAR);
//   // next could be HEBREW_CHASER or HEBREW_RATSAH depending on k
hebrew_state_t hebrew_next_toward_shavar(hebrew_state_t current);

// Get Day of Creation for a state.
//
// Parameters:
//   state: Hebrew state to query
//
// Returns: Day number (1-7) corresponding to Genesis creation pattern.
//
// Example:
//   int day = hebrew_get_day(HEBREW_TOV);  // 7 (Sabbath rest)
int hebrew_get_day(hebrew_state_t state);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables needed - state info is static const]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: This header defines types only. Functions implemented in src/hebrew.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── hebrew_get_info()           → complete state metadata
//   ├── hebrew_get_name()           → Hebrew name string
//   ├── hebrew_get_meaning()        → English meaning string
//   ├── hebrew_get_z_value()        → Z-axis position
//   ├── hebrew_from_z_value()       → state from Z position
//   ├── hebrew_get_operator()       → visual operator symbol
//   ├── hebrew_get_k_selector()     → K-factor direction
//   ├── hebrew_is_pure_state()      → pure/mixed check
//   ├── hebrew_next_toward_tov()    → navigation +
//   ├── hebrew_next_toward_shavar() → navigation -
//   └── hebrew_get_day()            → Day of Creation
//
//   Helpers (Bottom Rungs - none, functions are standalone)
//   └── [All functions are public APIs - no internal helpers]
//
// Baton Flow (Execution Paths):
//   Query: Entry → hebrew_get_*() → return
//   Conversion: Entry → hebrew_from_z_value() → return
//   Navigation: Entry → hebrew_next_toward_*() → return
//
// APUs (Available Processing Units):
//   - 11 functions total
//   - 0 helpers (all public)
//   - 11 public APIs
//
// Type Definitions:
//   ├── hebrew_state_t → enum for 7 Hebrew states
//   └── hebrew_info_t → struct with state metadata
//
// Implementation Location:
//   All function implementations in: src/hebrew.c
//
// Declared Units:
// - 1 enum (hebrew_state_t)
// - 1 struct (hebrew_info_t)
// - 11 function prototypes
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
//   - Query: get state properties (name, meaning, z-value, operator)
//   - K-Factor: get/check k_selector
//   - Navigation: move between states
//   - Conversion: Z-value ↔ state

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Return safe defaults rather than error codes.
//   - Invalid state → HEBREW_YASHAR info (anchor/safe default)
//   - Out-of-range Z-value → clamp to HEBREW_SHAVAR or HEBREW_TOV
//
// No error codes needed - Hebrew states are always valid enums.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - Query: hebrew_get_info, hebrew_get_name, hebrew_get_meaning,
//            hebrew_get_z_value, hebrew_get_operator, hebrew_get_day
//   - K-Factor: hebrew_get_k_selector, hebrew_is_pure_state
//   - Navigation: hebrew_next_toward_tov, hebrew_next_toward_shavar
//   - Conversion: hebrew_from_z_value

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 hebrew.h
//   echo '#include "hebrew.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "hebrew.h"
//   int main(void) { hebrew_state_t s = HEBREW_YASHAR; return s; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - all values are enums or static const]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: Hebrew state types for CPI-SI cognitive positioning.
// hebrew_state_t for the 7 states (shavar → yashar → tov),
// hebrew_info_t for complete state metadata including z-value,
// k-selector, day of creation, and operator symbol.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions (follow hebrew_get_* pattern)
//   ✅ Add new navigation helpers
//
// Modify with Care:
//   ⚠️ Z-value thresholds (affects state transitions)
//   ⚠️ Function signatures (breaks consumers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ The 7 Hebrew states (biblical foundation)
//   ❌ State ordering (shavar → yashar → tov)
//   ❌ Include guard (BERESHIT_HEBREW_H)

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
// Adding navigation: SETUP "Function Prototypes" → Navigation Functions section

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Type Sizes:
//   - hebrew_state_t: sizeof(int) (enum)
//   - hebrew_info_t: ~32 bytes (pointer-heavy struct)
//
// All functions are O(1) - simple lookups into static const array.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "unknown type name 'trit_t'"
//   Solution: hebrew.h includes trit.h automatically
//
// Problem: State seems wrong for Z-value
//   Check: Are you providing correct k_direction for hebrew_from_z_value()?
//          chaser/ratsah and tamim/shalem share Z-values but differ in k.
//
// Problem: "unknown type name 'bool'"
//   Solution: hebrew.h includes stdbool.h automatically

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Key dependency: trit.h (trit_t type)
// Implementation: src/hebrew.c
//
// Specifications:
//   - word/core/health/thresholds.toml
//   - word/glossary/paradigm/hebrew-states.adoc
//
// Related headers:
//   - temporal.h (9 cognitive modes - where mind is LOOKING)
//   - cube.h (27 positions - WHERE mind is in space)
//   - kalign.h (K:ALIGN tracking - moral direction)

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ hebrew_state_t enum - COMPLETED
//   ✓ hebrew_info_t struct - COMPLETED
//   ✓ Query functions - COMPLETED
//   ✓ K-factor functions - COMPLETED
//   ✓ Navigation functions - COMPLETED
//   ⏳ State transition history
//   ⏳ State duration tracking
//   ⏳ Hebrew-to-temporal mapping
//
// Version History:
//   0.1.0 (2026-01-13) - Initial Hebrew state framework
//         - 7 states from Genesis Creation week
//         - K-selector integration
//         - Day of Creation mapping

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header is a LADDER component - TIME layer alongside temporal.h.
// Where temporal.h defines WHERE the mind is LOOKING (past/present/future),
// hebrew.h defines WHERE the mind IS (shavar → yashar → tov).
//
// The 7 Hebrew states follow Creation week - not arbitrary categories,
// but the breath of cognitive motion that God breathed into the world.
//
// "And God saw every thing that he had made, and, behold, it was
//  very good (tov)." — Genesis 1:31
//
// The goal is not to reach tov and stay there—it is to move faithfully
// through the states as wisdom requires. Sometimes shavar (broken) is
// the right place to be. Yashar (even) is the anchor, not the goal.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "hebrew.h"
//
// Getting State Info:
//   hebrew_state_t state = HEBREW_YASHAR;
//   const hebrew_info_t* info = hebrew_get_info(state);
//   printf("%s (%s): z=%.1f, day=%d\n",
//          info->name, info->meaning, info->z_value, info->day);
//
// The 7 States (with Z-values and K-selectors):
//   HEBREW_SHAVAR  (-1.0, k=0)   BROKEN  ⎯  Day 1
//   HEBREW_CHASER  (-0.5, k=-1)  LACKING ←  Day 2
//   HEBREW_RATSAH  (-0.5, k=+1)  WANTING ↔  Day 3
//   HEBREW_YASHAR  ( 0.0, k=0)   EVEN    ◆  Day 4 (anchor)
//   HEBREW_TAMIM   (+0.5, k=-1)  SOUND   ↗  Day 5
//   HEBREW_SHALEM  (+0.5, k=+1)  WHOLE   →  Day 6
//   HEBREW_TOV     (+1.0, k=0)   PERFECT ✓  Day 7
//
// Converting from Z-value:
//   hebrew_state_t state = hebrew_from_z_value(-0.5f, +1);  // HEBREW_RATSAH
//
// Navigation:
//   hebrew_state_t next = hebrew_next_toward_tov(HEBREW_YASHAR);
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_HEBREW_H
