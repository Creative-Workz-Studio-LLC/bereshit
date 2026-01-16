// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide-tui
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_ide_tui.h — OmniCode IDE Terminal UI
// ncurses-based interactive editor

#ifndef BERESHIT_OMNI_IDE_TUI_H
#define BERESHIT_OMNI_IDE_TUI_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-tui
// Title: OmniCode IDE TUI
// Type: Header
// Role: Terminal-based interactive editor using ncurses

// Grounding: Proverbs 22:29 - "Seest thou a man diligent in his work?"
// Principle: Skilled work through accessible tooling
// Anchor: Kingdom Technology excludes no one

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_ide_panel.h"  // Includes omni_ide.h
#include "display.h"         // Cornerstone display abstraction
#include <stdbool.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 TUI Configuration [CONFIG]

typedef struct {
    bool show_line_numbers;     // Display line numbers
    bool show_block_colors;     // Color by block type
    bool show_status_bar;       // Show status line
    bool syntax_highlight;      // Enable syntax highlighting
    int tab_width;              // Tab display width
    bool soft_wrap;             // Wrap long lines
} IDETUIConfig;

// # B.2 Menu System [MENU]

#ifndef IDE_TUI_MENU_MAX_ITEMS
#define IDE_TUI_MENU_MAX_ITEMS 12
#endif

typedef enum {
    MENU_NONE = -1,
    MENU_FILE = 0,
    MENU_EDIT,
    MENU_VIEW,
    MENU_HELP,
    MENU_COUNT
} IDEMenuType;

typedef struct {
    const char* label;          // Display label
    const char* shortcut;       // Shortcut hint (e.g., "Ctrl+S")
    int action;                 // Action code
    bool separator;             // Draw separator after this item
} IDEMenuItem;

typedef struct {
    const char* label;          // Menu title (e.g., "File")
    char hotkey;                // Alt+key to open (e.g., 'F')
    IDEMenuItem items[IDE_TUI_MENU_MAX_ITEMS];
    int item_count;
} IDEMenu;

typedef struct {
    IDEMenu menus[MENU_COUNT];
    IDEMenuType active_menu;    // Currently open menu (-1 = none)
    int active_item;            // Selected item in active menu
    bool visible;               // Menu bar visible
} IDEMenuBar;

// # B.3 TUI State [STATE]

typedef enum {
    FOCUS_EDITOR,
    FOCUS_FILE_BROWSER,
    FOCUS_BLOCK_OUTLINE,
    FOCUS_MENU
} IDETUIFocus;

typedef struct {
    IDEBuffer* buffer;          // The buffer being edited
    IDETUIConfig config;        // Display configuration
    IDEDiagnostics* diags;      // Current diagnostics

    // Dimensions
    int screen_rows;
    int screen_cols;
    int edit_rows;              // Rows available for editing
    int edit_cols;              // Cols available for editing

    // Menu bar
    IDEMenuBar menubar;

    // Panel system
    IDELayout* layout;          // Panel layout
    IDEFileBrowser file_browser; // File browser state
    IDEBlockOutline block_outline; // Block outline state
    IDETemplateInfo template_info; // Template info state
    IDEValidationPanel validation; // Validation panel state
    IDETUIFocus focus;          // Which panel has focus
    bool panels_visible;        // Toggle panels on/off

    // Status
    char status_message[256];
    bool running;
    bool needs_redraw;
} IDETUI;

// # B.3 TUI Lifecycle [LIFECYCLE]

// Initialize TUI (starts ncurses)
IDETUI* ide_tui_create(IDEBuffer* buffer);

// Clean up TUI (ends ncurses)
void ide_tui_destroy(IDETUI* tui);

// # B.4 Display [DISPLAY]

// Full redraw
void ide_tui_draw(IDETUI* tui);

// Draw just the current line
void ide_tui_draw_line(IDETUI* tui, uint32_t line);

// Draw status bar
void ide_tui_draw_status(IDETUI* tui);

// Set status message
void ide_tui_set_status(IDETUI* tui, const char* format, ...);

// # B.5 Input [INPUT]

// Handle single keypress (abstracted via display layer)
void ide_tui_handle_key(IDETUI* tui, DisplayKey key);

// Main event loop
void ide_tui_run(IDETUI* tui);

// # B.6 Commands [COMMANDS]

// Open file
bool ide_tui_open(IDETUI* tui, const char* filepath);

// Save file
bool ide_tui_save(IDETUI* tui);

// Save as
bool ide_tui_save_as(IDETUI* tui, const char* filepath);

// Prompt for input
char* ide_tui_prompt(IDETUI* tui, const char* message);

// # B.7 Navigation [NAVIGATION]

// Scroll viewport to show cursor
void ide_tui_ensure_visible(IDETUI* tui);

// Go to line
void ide_tui_goto_line(IDETUI* tui, uint32_t line);

// Find text
void ide_tui_find(IDETUI* tui, const char* pattern);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Kingdom Technology excludes no one."
// The TUI provides full editing in any terminal.

#endif // BERESHIT_OMNI_IDE_TUI_H

// =============================================================================
// END CLOSING
// =============================================================================
