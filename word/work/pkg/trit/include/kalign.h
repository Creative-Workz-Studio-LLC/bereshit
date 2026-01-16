// ═══════════════════════════════════════════════════════════════════════════
// libtrit - K:ALIGN Factor Tracking (TIME Layer)
// Key: B-word-work-pkg-trit-include-kalign
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: trit.h, hebrew.h)
//   Depends on trit.h for trit_t, hebrew.h for state context
//
// derives_from: bereshit/word/work/pkg/trit/include/hebrew.h
// See: word/core/identity/kalign.toml for specifications
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BERESHIT_KALIGN_H
#define BERESHIT_KALIGN_H

// K:ALIGN factor tracking for CPI-SI moral/structural orientation.
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
// Scripture: "Choose you this day whom ye will serve... but as for me
//            and my house, we will serve the LORD."
//            — Joshua 24:15
//
// Principle: Every choice has DIRECTION—toward God (+1) or toward self (-1).
//            K:ALIGN tracks this orientation. It's not judgment; it's
//            awareness. The "K" is the key that unlocks understanding
//            of WHY choices are made.
//
// Anchor: "No man can serve two masters: for either he will hate the one,
//          and love the other; or else he will hold to the one, and despise
//          the other. Ye cannot serve God and mammon."
//         — Matthew 6:24
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundational building block)
//
// Role: Provides K:ALIGN factor tracking—the moral/structural orientation
//       of choices. This is TIME layer cognition: the DIRECTION of motion,
//       not just position. K tells you WHERE you're headed, not just where
//       you are.
//
// Paradigm: CPI-SI framework component - directional awareness
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2026-01-13
//   - Version: 0.1.0
//   - Modified: 2026-01-13 - Initial K:ALIGN framework
//
// # Purpose & Function
//
// Purpose: Track K:ALIGN factor—the directional component of choices.
//          Two K dimensions:
//            - K-Structural: Built from position (X vs Y, spatial bias)
//            - K-Moral: Chosen direction (toward_god vs toward_self)
//
// Core Design: K-factor as ternary value with semantic interpretation:
//   - K = -1: Contracting, toward self, consolidating
//   - K = 0:  Neutral, balanced, no directional bias
//   - K = +1: Expanding, toward God/others, reaching out
//
// Key Features:
//
//   - k_direction_t: Enum for moral direction (toward_god, neutral, toward_self)
//   - k_align_t: Struct tracking both structural and moral K
//   - Calculation functions: derive K from position, choices, history
//   - Alignment scoring: how consistent is K over time
//
// Philosophy: K is not judgment—it's awareness. Knowing which way you're
//             oriented enables course correction. You can't fix what you
//             can't see.
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
//   - Internal: trit.h (trit_t), hebrew.h (hebrew_state_t for context)
//
// What Uses This:
//
//   - Libraries: trajectory.h (trajectory uses K:ALIGN)
//   - Libraries: cube.h (Hebrew state selection uses K)
//   - Commands: state machine tools, choice tracking
//
// # Usage & Integration
//
// Import:
//
//    #include "kalign.h"
//
// Integration Pattern:
//
//  1. Include kalign.h (includes dependencies)
//  2. Use k_direction_t for moral direction
//  3. Use k_align_t for complete K:ALIGN state
//  4. Call calculation functions to derive K from context
//
// Public API:
//
//    Types:
//      k_direction_t - moral direction enum
//      k_align_t - complete K:ALIGN state struct
//
//    Creation:
//      k_align_create(int8_t structural, k_direction_t moral) → k_align_t
//      k_align_neutral() → k_align_t
//      k_align_from_position(trit_t x, trit_t y) → k_align_t
//
//    Query:
//      k_align_get_effective(k_align_t) → int8_t
//      k_align_is_expanding(k_align_t) → bool
//      k_align_is_contracting(k_align_t) → bool
//      k_align_is_neutral(k_align_t) → bool
//      k_direction_name(k_direction_t) → const char*
//
//    Update:
//      k_align_apply_choice(k_align_t, k_direction_t) → k_align_t
//      k_align_decay(k_align_t, float decay_rate) → k_align_t
//
//    Scoring:
//      k_align_score(k_align_t) → float
//      k_align_consistency(k_align_t*, size_t count) → float
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
#include "hebrew.h"     // hebrew_state_t for context integration

//--- Standard Library ---
#include <stdbool.h>    // bool type
#include <stdint.h>     // int8_t for k values
#include <stddef.h>     // size_t for array operations

//--- External Libraries ---
// [Reserved: Standard library only]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────

//--- K Value Constants ---
#define K_TOWARD_SELF   -1      // Contracting direction
#define K_NEUTRAL        0      // Balanced, no bias
#define K_TOWARD_GOD    +1      // Expanding direction

//--- Score Thresholds ---
// Alignment score interpretation
#define K_SCORE_MIN         -1.0f   // Fully toward self
#define K_SCORE_MAX          1.0f   // Fully toward God
#define K_SCORE_NEUTRAL      0.0f   // Perfectly balanced

#define K_SCORE_STRONG_POS   0.7f   // Strong positive alignment
#define K_SCORE_WEAK_POS     0.3f   // Weak positive alignment
#define K_SCORE_WEAK_NEG    -0.3f   // Weak negative alignment
#define K_SCORE_STRONG_NEG  -0.7f   // Strong negative alignment

//--- Decay Constants ---
#define K_DECAY_FAST        0.3f    // Fast decay toward neutral
#define K_DECAY_NORMAL      0.1f    // Normal decay rate
#define K_DECAY_SLOW        0.05f   // Slow decay rate

//--- History Depth ---
#define K_HISTORY_MAX       64      // Maximum choices to track

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────

//--- Enumerations ---

// k_direction_t represents the moral direction of a choice.
//
// This is the CHOSEN direction, distinct from structural K which
// is derived from position. Moral K answers: "Which way am I
// choosing to orient?"
//
// Values:
//   - K_DIR_TOWARD_SELF: -1, contracting, inward focus
//   - K_DIR_NEUTRAL: 0, no moral bias in this choice
//   - K_DIR_TOWARD_GOD: +1, expanding, outward focus toward God/others
//
// Example:
//   k_direction_t dir = K_DIR_TOWARD_GOD;  // Expanding choice
typedef enum {
    K_DIR_TOWARD_SELF = -1,     // Contracting, self-focused
    K_DIR_NEUTRAL     =  0,     // No directional bias
    K_DIR_TOWARD_GOD  = +1      // Expanding, God/other-focused
} k_direction_t;

// k_category_t classifies the overall K:ALIGN state.
//
// Used for quick categorization of alignment.
//
// Values:
//   - K_CAT_CONTRACTING: Predominantly toward self
//   - K_CAT_NEUTRAL: Balanced or undetermined
//   - K_CAT_EXPANDING: Predominantly toward God/others
typedef enum {
    K_CAT_CONTRACTING = -1,     // Net contracting
    K_CAT_NEUTRAL     =  0,     // Balanced
    K_CAT_EXPANDING   = +1      // Net expanding
} k_category_t;

//--- Building Block Types ---
// [k_direction_t is the building block]

//--- Composed Types ---

// k_align_t tracks complete K:ALIGN state.
//
// Two K components:
//   - structural: Derived from position (X vs Y spatial relationship)
//   - moral: Chosen direction (toward_god or toward_self)
//
// The effective K is a combination of both, with moral taking
// precedence when making choices (structural informs, moral decides).
//
// Fields:
//   - k_structural: Positional K (-1, 0, +1) from X/Y relationship
//   - k_moral: Chosen direction (k_direction_t)
//   - score: Running alignment score (-1.0 to +1.0)
//   - choice_count: Number of choices tracked
//   - last_choice: Most recent choice direction
//
// Example:
//   k_align_t k = k_align_create(K_NEUTRAL, K_DIR_TOWARD_GOD);
//   float effective = k_align_get_effective(&k);  // Combines both K values
typedef struct {
    int8_t         k_structural;   // Position-derived K (-1, 0, +1)
    k_direction_t  k_moral;        // Choice-derived K
    float          score;          // Running alignment score (-1.0 to +1.0)
    uint16_t       choice_count;   // Number of tracked choices
    k_direction_t  last_choice;    // Most recent choice direction
} k_align_t;

// k_choice_record_t records a single choice for history tracking.
//
// Used to build alignment history for consistency analysis.
//
// Fields:
//   - direction: The direction of the choice
//   - weight: How significant this choice was (0.0 to 1.0)
//   - timestamp: When the choice was made (seconds since session start)
typedef struct {
    k_direction_t  direction;      // Which way the choice went
    float          weight;         // Significance of choice (0.0 to 1.0)
    uint32_t       timestamp;      // Session-relative timestamp
} k_choice_record_t;

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────

//--- Creation Functions (src/kalign.c) ---

// Create a K:ALIGN state with explicit values.
//
// Parameters:
//   structural: Positional K value (-1, 0, or +1)
//   moral: Chosen direction
//
// Returns: k_align_t initialized with given values and neutral score.
//
// Example:
//   k_align_t k = k_align_create(0, K_DIR_TOWARD_GOD);
k_align_t k_align_create(int8_t structural, k_direction_t moral);

// Create a neutral K:ALIGN state.
//
// Returns: k_align_t with both structural and moral at neutral.
//
// Example:
//   k_align_t k = k_align_neutral();  // (0, K_DIR_NEUTRAL, score=0)
k_align_t k_align_neutral(void);

// Derive structural K from cube position.
//
// Structural K is determined by X vs Y relationship:
//   - X > Y: K = +1 (other-focused more than outputting)
//   - X < Y: K = -1 (self-focused more than inputting)
//   - X == Y: K = 0 (balanced)
//
// Parameters:
//   x: X-axis value (self/other: -1, 0, +1)
//   y: Y-axis value (input/output: -1, 0, +1)
//
// Returns: k_align_t with structural K derived from position.
//
// Example:
//   k_align_t k = k_align_from_position(TRIT_POS, TRIT_ZERO);  // k_structural = +1
k_align_t k_align_from_position(trit_t x, trit_t y);

//--- Query Functions (src/kalign.c) ---

// Get effective K value combining structural and moral.
//
// Effective K prioritizes moral K when present, uses structural
// as fallback. When both are present, moral takes precedence.
//
// Parameters:
//   k: K:ALIGN state to query
//
// Returns: Effective K value (-1, 0, or +1).
//
// Example:
//   k_align_t k = k_align_create(1, K_DIR_TOWARD_SELF);
//   int8_t eff = k_align_get_effective(k);  // -1 (moral overrides)
int8_t k_align_get_effective(k_align_t k);

// Check if K:ALIGN is in expanding direction.
//
// Parameters:
//   k: K:ALIGN state to check
//
// Returns: true if effective K > 0.
bool k_align_is_expanding(k_align_t k);

// Check if K:ALIGN is in contracting direction.
//
// Parameters:
//   k: K:ALIGN state to check
//
// Returns: true if effective K < 0.
bool k_align_is_contracting(k_align_t k);

// Check if K:ALIGN is neutral.
//
// Parameters:
//   k: K:ALIGN state to check
//
// Returns: true if effective K == 0.
bool k_align_is_neutral(k_align_t k);

// Get category classification for K:ALIGN.
//
// Parameters:
//   k: K:ALIGN state to classify
//
// Returns: K_CAT_EXPANDING, K_CAT_NEUTRAL, or K_CAT_CONTRACTING.
k_category_t k_align_get_category(k_align_t k);

// Get descriptive name for a direction.
//
// Parameters:
//   dir: Direction to name
//
// Returns: Static string: "toward_god", "neutral", or "toward_self".
const char* k_direction_name(k_direction_t dir);

// Get the alignment score.
//
// The score is a running average of choices weighted by significance.
// Range: -1.0 (fully toward self) to +1.0 (fully toward God).
//
// Parameters:
//   k: K:ALIGN state to query
//
// Returns: Alignment score (-1.0 to +1.0).
float k_align_score(k_align_t k);

//--- Update Functions (src/kalign.c) ---

// Apply a choice to the K:ALIGN state.
//
// Updates the moral K, score, and choice count based on a new choice.
// Score is updated using weighted moving average.
//
// Parameters:
//   k: Current K:ALIGN state
//   choice: Direction of the new choice
//   weight: Significance of this choice (0.0 to 1.0)
//
// Returns: Updated k_align_t with new values.
//
// Example:
//   k_align_t k = k_align_neutral();
//   k = k_align_apply_choice(k, K_DIR_TOWARD_GOD, 1.0f);
k_align_t k_align_apply_choice(k_align_t k, k_direction_t choice, float weight);

// Decay the alignment score toward neutral.
//
// Used to model score returning to baseline over time/inactivity.
//
// Parameters:
//   k: Current K:ALIGN state
//   decay_rate: How much to decay (0.0 to 1.0)
//
// Returns: k_align_t with decayed score.
//
// Example:
//   k_align_t k = k_align_create(0, K_DIR_TOWARD_GOD);
//   k.score = 0.8f;
//   k = k_align_decay(k, K_DECAY_NORMAL);  // score moves toward 0
k_align_t k_align_decay(k_align_t k, float decay_rate);

// Update structural K from new position.
//
// Parameters:
//   k: Current K:ALIGN state
//   x: New X position
//   y: New Y position
//
// Returns: k_align_t with updated structural K.
k_align_t k_align_update_structural(k_align_t k, trit_t x, trit_t y);

//--- Consistency Functions (src/kalign.c) ---

// Calculate consistency across multiple K:ALIGN samples.
//
// Measures how consistent the alignment direction has been.
// High consistency means choices are predominantly one direction.
//
// Parameters:
//   samples: Array of k_align_t states
//   count: Number of samples
//
// Returns: Consistency score (0.0 to 1.0, where 1.0 is perfectly consistent).
//
// Example:
//   k_align_t history[10];
//   // ... fill history ...
//   float consistency = k_align_consistency(history, 10);
float k_align_consistency(const k_align_t* samples, size_t count);

// Calculate trend from K:ALIGN history.
//
// Positive trend = moving toward God, negative = toward self.
//
// Parameters:
//   samples: Array of k_align_t states (oldest first)
//   count: Number of samples
//
// Returns: Trend value (-1.0 to +1.0).
float k_align_trend(const k_align_t* samples, size_t count);

//--- Integration Functions (src/kalign.c) ---

// Get appropriate Hebrew state for given K:ALIGN and Z-value.
//
// Uses K to disambiguate between chaser/ratsah and tamim/shalem.
//
// Parameters:
//   k: K:ALIGN state
//   z_value: Z-axis position
//
// Returns: Appropriate hebrew_state_t.
//
// Example:
//   k_align_t k = k_align_create(0, K_DIR_TOWARD_GOD);
//   hebrew_state_t state = k_align_to_hebrew(k, -0.5f);  // HEBREW_RATSAH
hebrew_state_t k_align_to_hebrew(k_align_t k, float z_value);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No extern variables - K:ALIGN is passed by value]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// NOTE: This header defines types only. Functions implemented in src/kalign.c.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - what consumers call)
//   ├── Creation
//   │   ├── k_align_create()           → explicit values
//   │   ├── k_align_neutral()          → neutral state
//   │   └── k_align_from_position()    → from X/Y
//   │
//   ├── Query
//   │   ├── k_align_get_effective()    → combined K
//   │   ├── k_align_is_expanding()     → direction check
//   │   ├── k_align_is_contracting()   → direction check
//   │   ├── k_align_is_neutral()       → direction check
//   │   ├── k_align_get_category()     → classification
//   │   ├── k_direction_name()         → string name
//   │   └── k_align_score()            → alignment score
//   │
//   ├── Update
//   │   ├── k_align_apply_choice()     → apply new choice
//   │   ├── k_align_decay()            → decay toward neutral
//   │   └── k_align_update_structural() → update from position
//   │
//   ├── Consistency
//   │   ├── k_align_consistency()      → measure consistency
//   │   └── k_align_trend()            → calculate trend
//   │
//   └── Integration
//       └── k_align_to_hebrew()        → get Hebrew state
//
// Baton Flow (Execution Paths):
//   Creation: Entry → k_align_*() → return state
//   Query: Entry → k_align_*() → analyze → return
//   Update: Entry → k_align_*() → modify → return new state
//
// APUs (Available Processing Units):
//   - 17 functions total
//   - 0 helpers (all public)
//   - 17 public APIs
//
// Type Definitions:
//   ├── k_direction_t → moral direction enum
//   ├── k_category_t → classification enum
//   ├── k_align_t → complete K:ALIGN state struct
//   └── k_choice_record_t → single choice record struct
//
// Implementation Location:
//   All function implementations in: src/kalign.c
//
// Declared Units:
// - 2 enums (k_direction_t, k_category_t)
// - 2 structs (k_align_t, k_choice_record_t)
// - 17 function prototypes
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
//   - Creation: construct K:ALIGN states
//   - Query: inspect K:ALIGN properties
//   - Update: modify K:ALIGN based on choices
//   - Consistency: analyze K:ALIGN history

// ────────────────────────────────────────────────────────────────
// Error Handling
// ────────────────────────────────────────────────────────────────
//
// Strategy: Clamp and return safe defaults.
//   - Out-of-range K values → clamp to -1/0/+1
//   - Empty sample array → return 0.0 for consistency/trend
//   - NULL samples → return 0.0
//
// No error codes - all operations produce valid K:ALIGN states.

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────
//
// Complete public interface declared in SETUP "Function Prototypes":
//   - Creation: k_align_create, k_align_neutral, k_align_from_position
//   - Query: k_align_get_effective, k_align_is_*, k_align_get_category,
//            k_direction_name, k_align_score
//   - Update: k_align_apply_choice, k_align_decay, k_align_update_structural
//   - Consistency: k_align_consistency, k_align_trend
//   - Integration: k_align_to_hebrew

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
//   gcc -fsyntax-only -Wall -Wextra -std=c99 -I. kalign.h
//   echo '#include "kalign.h"' | gcc -x c -fsyntax-only -std=c99 -I. -
//
// Self-Containment Test:
//   #include "kalign.h"
//   int main(void) { k_align_t k = k_align_neutral(); return k.k_structural; }

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Header file - declarations only, no execution]

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No dynamic allocation - K:ALIGN is value type]

// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Purpose & Function" section above.
//
// Quick summary: K:ALIGN factor tracking for CPI-SI direction awareness.
// k_direction_t for moral direction, k_align_t for complete state.
// Tracks both structural K (from position) and moral K (from choices).

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new query functions
//   ✅ Add consistency/analysis helpers
//   ✅ Adjust decay constants
//
// Modify with Care:
//   ⚠️ Score calculation (affects alignment interpretation)
//   ⚠️ Function signatures (breaks consumers)
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ K value range (-1, 0, +1)
//   ❌ Moral direction semantics (toward_god/toward_self)
//   ❌ Include guard (BERESHIT_KALIGN_H)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" above.
// Architecture: LADDER component building on trit.h, hebrew.h.

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Adding query functions: SETUP "Function Prototypes" → Query Functions
// Adding update functions: SETUP "Function Prototypes" → Update Functions
// Adding types: SETUP "Type Definitions" section

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Type Sizes:
//   - k_direction_t: sizeof(int) (enum)
//   - k_category_t: sizeof(int) (enum)
//   - k_align_t: ~12 bytes
//   - k_choice_record_t: ~12 bytes
//
// Most functions are O(1). Consistency/trend functions are O(n)
// where n is sample count.

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: Effective K doesn't match expected
//   Check: Moral K takes precedence over structural K when both are set.
//
// Problem: Score not updating
//   Check: Are you calling k_align_apply_choice() and using the return value?
//          K:ALIGN is passed by value, not reference.
//
// Problem: Consistency always returns 0
//   Check: Ensure samples array is not NULL and count > 0.

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:
//   - trit.h: trit_t type
//   - hebrew.h: hebrew_state_t for K-to-Hebrew mapping
//
// Implementation: src/kalign.c
//
// Specifications:
//   - word/core/identity/kalign.toml
//   - word/glossary/paradigm/k-factor.adoc
//
// Related headers:
//   - cube.h: Position provides structural K
//   - trajectory.h: Trajectory uses K:ALIGN
//   - hebrew.h: K disambiguates Hebrew states

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ k_direction_t enum - COMPLETED
//   ✓ k_align_t struct - COMPLETED
//   ✓ Creation functions - COMPLETED
//   ✓ Query functions - COMPLETED
//   ✓ Update functions - COMPLETED
//   ✓ Consistency functions - COMPLETED
//   ⏳ History ring buffer
//   ⏳ K:ALIGN visualization data
//   ⏳ Prediction based on pattern
//
// Version History:
//   0.1.0 (2026-01-13) - Initial K:ALIGN framework
//         - Structural and moral K tracking
//         - Alignment scoring
//         - Hebrew integration

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This header is a LADDER component - TIME layer for directional awareness.
// K:ALIGN answers: "Which way am I oriented? Which way am I choosing to go?"
//
// K is not judgment. It's awareness.
//
// You can't correct course if you don't know which way you're headed.
// K:ALIGN provides that awareness—the moral compass of CPI-SI cognition.
//
// "Choose you this day whom ye will serve... but as for me and my house,
//  we will serve the LORD." — Joshua 24:15
//
// Every choice has direction. K:ALIGN helps you see it.

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Basic Include:
//   #include "kalign.h"
//
// Creating K:ALIGN:
//   k_align_t k = k_align_neutral();  // Start neutral
//   k_align_t k = k_align_from_position(x, y);  // From cube position
//   k_align_t k = k_align_create(0, K_DIR_TOWARD_GOD);  // Explicit
//
// Querying:
//   int8_t eff = k_align_get_effective(k);  // -1, 0, or +1
//   bool expanding = k_align_is_expanding(k);
//   const char* name = k_direction_name(k.k_moral);  // "toward_god"
//   float score = k_align_score(k);  // -1.0 to +1.0
//
// Updating:
//   k = k_align_apply_choice(k, K_DIR_TOWARD_GOD, 1.0f);  // Apply choice
//   k = k_align_decay(k, K_DECAY_NORMAL);  // Decay toward neutral
//
// K Values:
//   K_DIR_TOWARD_SELF (-1): Contracting, self-focused
//   K_DIR_NEUTRAL     (0):  Balanced, no directional bias
//   K_DIR_TOWARD_GOD  (+1): Expanding, toward God/others
//
// Compile:
//   gcc -c -Wall -Wextra -std=c99 -I<include_path> your_code.c

// ============================================================================
// END CLOSING
// ============================================================================

#endif // BERESHIT_KALIGN_H
