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
// BODY [BODY]
// =============================================================================

// # B.1 Hebrew State Enum [STATES]
//
// The 7 Hebrew states form a spectrum from broken to complete.
// Encoded as trit-like values: -3 to +3
//
// shavar(-1) ←── chaser/ratsah(-0.5) ←── yashar(0) ──→ tamim/shalem(+0.5) ──→ tov(+1)
//      ↑                                     ↑                                    ↑
//   crisis                                center                              completion

// DAR (Detect-Assess-Recover) Hebrew States
// Extended 7-state system for fine-grained cognitive positioning
// These use DAR_ prefix to avoid conflict with simpler 5-state HebrewState in cpisi_log.h
typedef enum {
    DAR_SHAVAR  = -3,    // Broken (crisis)
    DAR_CHASER  = -2,    // Lacking (regressing, k=-1)
    DAR_RATSAH  = -1,    // Wanting (hungry for growth, k=+1)
    DAR_YASHAR  =  0,    // Even (center/default)
    DAR_TAMIM   = +1,    // Sound (consolidating, k=-1)
    DAR_SHALEM  = +2,    // Whole (expanding, k=+1)
    DAR_TOV     = +3     // Perfect (completion)
} DARHebrewState;  // 7-state system for DAR compatibility

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
