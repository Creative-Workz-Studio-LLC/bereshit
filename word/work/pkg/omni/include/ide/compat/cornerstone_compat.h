// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         OMNI-IDE-CORNERSTONE-COMPAT
// Title:       Cornerstone Compatibility Header
// Type:        Header
// Component:   OmniCode IDE
// Role:        Map old Cornerstone API names to new Phase A structure
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Bridge between OmniCode IDE and Cornerstone Phase A reorganization.
//              Maps old DISPLAY_KEY_* names to new KEY_* names.
//              This is TEMPORARY — IDE should be refactored to use new API directly.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef OMNI_IDE_CORNERSTONE_COMPAT_H
#define OMNI_IDE_CORNERSTONE_COMPAT_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "keyboard.dev.h"    // New keyboard types from Cornerstone HAL

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Type Compatibility
// -----------------------------------------------------------------------------

// DisplayKey was renamed to KeyCode in Phase A
typedef KeyCode DisplayKey;

// -----------------------------------------------------------------------------
// B.2 Key Constant Compatibility
// -----------------------------------------------------------------------------

// Navigation keys
#define DISPLAY_KEY_UP          KEY_UP
#define DISPLAY_KEY_DOWN        KEY_DOWN
#define DISPLAY_KEY_LEFT        KEY_LEFT
#define DISPLAY_KEY_RIGHT       KEY_RIGHT
#define DISPLAY_KEY_HOME        KEY_HOME
#define DISPLAY_KEY_END         KEY_END
#define DISPLAY_KEY_PAGEUP      KEY_PAGEUP
#define DISPLAY_KEY_PAGEDOWN    KEY_PAGEDOWN

// Action keys
#define DISPLAY_KEY_ENTER       KEY_ENTER
#define DISPLAY_KEY_ESCAPE      KEY_ESCAPE
#define DISPLAY_KEY_TAB         KEY_TAB
#define DISPLAY_KEY_BACKSPACE   KEY_BACKSPACE
#define DISPLAY_KEY_DELETE      KEY_DELETE
#define DISPLAY_KEY_INSERT      KEY_INSERT
#define DISPLAY_KEY_SPACE       KEY_SPACE

// Function keys
#define DISPLAY_KEY_F1          KEY_F1
#define DISPLAY_KEY_F2          KEY_F2
#define DISPLAY_KEY_F3          KEY_F3
#define DISPLAY_KEY_F4          KEY_F4
#define DISPLAY_KEY_F5          KEY_F5
#define DISPLAY_KEY_F6          KEY_F6
#define DISPLAY_KEY_F7          KEY_F7
#define DISPLAY_KEY_F8          KEY_F8
#define DISPLAY_KEY_F9          KEY_F9
#define DISPLAY_KEY_F10         KEY_F10
#define DISPLAY_KEY_F11         KEY_F11
#define DISPLAY_KEY_F12         KEY_F12

// Control key combinations (mapped to base key + modifier check)
// NOTE: In new API, modifiers are handled separately via KeyState.modifiers
// These macros map to base keys — code using them needs to also check modifiers
#define DISPLAY_KEY_CTRL_A      KEY_A
#define DISPLAY_KEY_CTRL_B      KEY_B
#define DISPLAY_KEY_CTRL_C      KEY_C
#define DISPLAY_KEY_CTRL_D      KEY_D
#define DISPLAY_KEY_CTRL_E      KEY_E
#define DISPLAY_KEY_CTRL_F      KEY_F
#define DISPLAY_KEY_CTRL_G      KEY_G
#define DISPLAY_KEY_CTRL_H      KEY_H
#define DISPLAY_KEY_CTRL_I      KEY_I
#define DISPLAY_KEY_CTRL_J      KEY_J
#define DISPLAY_KEY_CTRL_K      KEY_K
#define DISPLAY_KEY_CTRL_L      KEY_L
#define DISPLAY_KEY_CTRL_M      KEY_M
#define DISPLAY_KEY_CTRL_N      KEY_N
#define DISPLAY_KEY_CTRL_O      KEY_O
#define DISPLAY_KEY_CTRL_P      KEY_P
#define DISPLAY_KEY_CTRL_Q      KEY_Q
#define DISPLAY_KEY_CTRL_R      KEY_R
#define DISPLAY_KEY_CTRL_S      KEY_S
#define DISPLAY_KEY_CTRL_T      KEY_T
#define DISPLAY_KEY_CTRL_U      KEY_U
#define DISPLAY_KEY_CTRL_V      KEY_V
#define DISPLAY_KEY_CTRL_W      KEY_W
#define DISPLAY_KEY_CTRL_X      KEY_X
#define DISPLAY_KEY_CTRL_Y      KEY_Y
#define DISPLAY_KEY_CTRL_Z      KEY_Z

// Single letter keys (for legacy direct-key handling)
// NOTE: These are duplicates of CTRL_* mappings — will cause conflicts in switch
// Use #if 0 to disable the direct key cases in source, or refactor to use modifiers
#define DISPLAY_KEY_Q           KEY_Q
#define DISPLAY_KEY_S           KEY_S

// Special key for generic character input (printable chars)
// In new API, check KeyState for printable keys
#define DISPLAY_KEY_CHAR        KEY_NONE

// Mouse event key (signals a mouse event occurred)
#define DISPLAY_KEY_MOUSE       KEY_NONE  // Placeholder — mouse events need different handling

// Terminal resize event (ncurses KEY_RESIZE = 410)
#define DISPLAY_KEY_RESIZE      KEY_NONE  // Placeholder — resize handled via signal/callback

// Alt key combinations (same approach as Ctrl)
// WARNING: ALT_* maps to same KeyCode as CTRL_* — check modifiers separately!
#define DISPLAY_KEY_ALT_A       KEY_A
#define DISPLAY_KEY_ALT_B       KEY_B
#define DISPLAY_KEY_ALT_C       KEY_C
#define DISPLAY_KEY_ALT_D       KEY_D
#define DISPLAY_KEY_ALT_E       KEY_E
#define DISPLAY_KEY_ALT_F       KEY_F
#define DISPLAY_KEY_ALT_G       KEY_G
#define DISPLAY_KEY_ALT_H       KEY_H
#define DISPLAY_KEY_ALT_I       KEY_I
#define DISPLAY_KEY_ALT_J       KEY_J
#define DISPLAY_KEY_ALT_K       KEY_K
#define DISPLAY_KEY_ALT_L       KEY_L
#define DISPLAY_KEY_ALT_M       KEY_M
#define DISPLAY_KEY_ALT_N       KEY_N
#define DISPLAY_KEY_ALT_O       KEY_O
#define DISPLAY_KEY_ALT_P       KEY_P
#define DISPLAY_KEY_ALT_Q       KEY_Q
#define DISPLAY_KEY_ALT_R       KEY_R
#define DISPLAY_KEY_ALT_S       KEY_S
#define DISPLAY_KEY_ALT_T       KEY_T
#define DISPLAY_KEY_ALT_U       KEY_U
#define DISPLAY_KEY_ALT_V       KEY_V
#define DISPLAY_KEY_ALT_W       KEY_W
#define DISPLAY_KEY_ALT_X       KEY_X
#define DISPLAY_KEY_ALT_Y       KEY_Y
#define DISPLAY_KEY_ALT_Z       KEY_Z

// -----------------------------------------------------------------------------
// B.3 Function Compatibility Stubs
// -----------------------------------------------------------------------------
// These functions were removed in Phase A. The new API uses keyboard_* functions.
// These stubs allow compilation but need proper implementation.

// Stub: Wait for key (old API) — use keyboard_poll_key() in new API
static inline DisplayKey display_wait_key(void) {
    // TODO: Implement using new keyboard API
    // For now, return ESCAPE to exit immediately
    return KEY_ESCAPE;
}

// Stub: Get last character pressed (old API)
static inline char display_get_last_char(void) {
    // TODO: Implement using new keyboard API
    return '\0';
}

// -----------------------------------------------------------------------------
// B.4 Mouse Event Compatibility Stubs
// -----------------------------------------------------------------------------
// Mouse handling was removed or reorganized in Phase A.
// These stubs allow compilation — mouse functionality disabled until refactored.

// Mouse button constants
#define DISPLAY_MOUSE_BUTTON_LEFT   0
#define DISPLAY_MOUSE_BUTTON_RIGHT  1
#define DISPLAY_MOUSE_BUTTON_MIDDLE 2

// Mouse event structure (legacy)
typedef struct {
    int x;
    int y;
    int button;
    int action;  // 0=release, 1=press
} DisplayMouseEvent;

// Stub: Get mouse event (old API)
static inline DisplayMouseEvent display_get_mouse_event(void) {
    // TODO: Implement using new input API
    DisplayMouseEvent empty = {0, 0, -1, 0};
    return empty;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// TODO: This compatibility layer should be removed once the OmniCode IDE
//       is properly refactored to use the new Cornerstone API directly.
//
//       The new API separates:
//       - KeyCode: Raw key identity (which key)
//       - KeyModifier: Modifier state (Shift/Ctrl/Alt)
//       - KeyState: Full event (code + modifiers + pressed/released)
//
//       Code should be updated to check KeyState instead of having separate
//       CTRL_* and ALT_* key codes.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // OMNI_IDE_CORNERSTONE_COMPAT_H
