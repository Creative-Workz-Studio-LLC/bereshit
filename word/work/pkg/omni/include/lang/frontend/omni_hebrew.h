// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-hebrew
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_hebrew.h — Hebrew State Enumeration (Bottom Rung)
// Pure types, zero dependencies. The baton passed up the ladder.

#ifndef BERESHIT_OMNI_HEBREW_H
#define BERESHIT_OMNI_HEBREW_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-hebrew
// Title: Hebrew State Types
// Type: Header (Bottom Rung)
// Role: Define the 7 Hebrew cognitive states - pure data, no dependencies

// # M.8 Grounding [GROUNDING]
//
// Scripture: Proverbs 24:16 - "A just man falleth seven times, and riseth up again"
// Principle: The 7 states form the cognitive spectrum
// Anchor: Genesis 1:1 - yashar (center) is the resting state

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// Single source of truth: Cornerstone's domain types
#include "kernel/types/types.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Hebrew State — Using Cornerstone's Canonical Types [STATES]
//
// The 7 Hebrew states form a spectrum from broken to complete.
// HebrewState is defined in Cornerstone's types.h (0-indexed for array access):
//
//   HEBREW_SHAVAR (0) ← HEBREW_CHASER/RATSAH (1,2) ← HEBREW_YASHAR (3) → ...
//        ↑                                              ↑
//     crisis                                         center
//
// Old DARHebrewState (-3 to +3) is DEPRECATED. Use HebrewState (0-6) directly.

// # B.2 Backward Compatibility Aliases [COMPAT]
//
// These aliases exist for code that still uses DAR_ prefixed names.
// New code should use HebrewState and HEBREW_* constants directly.

#ifndef DAR_HEBREW_COMPAT_DEFINED
#define DAR_HEBREW_COMPAT_DEFINED

// Alias the canonical type
typedef HebrewState DARHebrewState;

// Alias the constants (maps DAR_* to HEBREW_*)
#define DAR_SHAVAR  HEBREW_SHAVAR   // 0
#define DAR_CHASER  HEBREW_CHASER   // 1
#define DAR_RATSAH  HEBREW_RATSAH   // 2
#define DAR_YASHAR  HEBREW_YASHAR   // 3
#define DAR_TAMIM   HEBREW_TAMIM    // 4
#define DAR_SHALEM  HEBREW_SHALEM   // 5
#define DAR_TOV     HEBREW_TOV      // 6

#endif // DAR_HEBREW_COMPAT_DEFINED

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// This header is the BATON passed up the ladder:
//   omni_hebrew.h (bottom) → omni_cpisi.h → omni_dar.h → ...
//
// It has NO dependencies. Any component that needs Hebrew states
// includes this directly without dragging the full system.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_HEBREW_H
