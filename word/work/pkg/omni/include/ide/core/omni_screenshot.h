// #!omni code --c -header
// #!omni meta.key = B-word-work-pkg-omni-screenshot-h
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_screenshot.h — Session screenshot capture API
// Visual feedback loop for UI development

#ifndef OMNI_SCREENSHOT_H
#define OMNI_SCREENSHOT_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-screenshot-h
// Purpose: Enable visual debugging of UI across all display modes

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// Screenshot functions are conditionally compiled based on display mode

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// Capture GUI screenshot (OpenGL framebuffer -> PNG)
// Label is optional suffix for the filename
// Returns 0 on success, -1 on failure
#ifdef IDE_GUI_ENABLED
int screenshot_gui(const char* label);
#endif

// Capture TUI screenshot (ncurses buffer -> text file)
// Returns 0 on success, -1 on failure
#ifdef IDE_TUI_ENABLED
int screenshot_tui(const char* label);
#endif

// Log CLI session (no visual capture possible)
// Returns 0 on success, -1 on failure
int screenshot_cli(const char* label);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Usage:
//   Before shutdown, call the appropriate function:
//     screenshot_gui("shutdown");   // Saves: gui/YYYY-MM-DD/HH-MM-SS_shutdown.png
//     screenshot_tui("shutdown");   // Saves: tui/YYYY-MM-DD/HH-MM-SS_shutdown.txt
//     screenshot_cli("shutdown");   // Saves: cli/YYYY-MM-DD/HH-MM-SS_shutdown.log

// =============================================================================
// END CLOSING
// =============================================================================

#endif // OMNI_SCREENSHOT_H
