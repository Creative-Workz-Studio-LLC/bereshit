// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide-gui
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_ide_gui.h — OmniCode IDE GUI Display
// Cornerstone display backend for graphical editing

#ifndef BERESHIT_OMNI_IDE_GUI_H
#define BERESHIT_OMNI_IDE_GUI_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-gui
// Title: OmniCode IDE GUI
// Type: Header
// Role: Graphical display using Cornerstone display abstraction

// Grounding: Acts 10:34 - "God is no respecter of persons"
// Principle: Kingdom Technology excludes no one
// Anchor: GUI serves visual learners and modern accessibility

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_ide.h"
#include "display.hal.h"     // Cornerstone display abstraction (HAL layer)
#include "../compat/cornerstone_compat.h"  // Key compatibility for Phase A
#include <stdbool.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 GUI Configuration [CONFIG]

typedef struct {
    bool show_line_numbers;     // Display line numbers
    bool show_block_colors;     // Color by block type
    bool show_status_bar;       // Show status line
    bool syntax_highlight;      // Enable syntax highlighting
    int tab_width;              // Tab display width
    bool soft_wrap;             // Wrap long lines
} IDEGUIConfig;

// # B.1b Menu System [MENU]

#define GUI_MENU_MAX_ITEMS 12

typedef enum {
    GUI_MENU_NONE = -1,
    GUI_MENU_FILE = 0,
    GUI_MENU_EDIT,
    GUI_MENU_INSERT,       // Template insertion menu
    GUI_MENU_BUILD,        // Build/Run actions
    GUI_MENU_VIEW,
    GUI_MENU_HELP,
    GUI_MENU_COUNT
} IDEGUIMenuType;

typedef struct {
    const char* label;          // Display label
    const char* shortcut;       // Shortcut hint (e.g., "Ctrl+S")
    int action;                 // Action code
    bool separator;             // Draw separator after this item
} IDEGUIMenuItem;

typedef struct {
    const char* label;          // Menu title (e.g., "File")
    char hotkey;                // Alt+key to open (e.g., 'F')
    IDEGUIMenuItem items[GUI_MENU_MAX_ITEMS];
    int item_count;
} IDEGUIMenu;

typedef struct {
    IDEGUIMenu menus[GUI_MENU_COUNT];
    IDEGUIMenuType active_menu; // Currently open menu (-1 = none)
    int active_item;            // Selected item in active menu
    bool visible;               // Menu bar visible
} IDEGUIMenuBar;

typedef enum {
    GUI_FOCUS_EDITOR,
    GUI_FOCUS_MENU,
    GUI_FOCUS_CLI          // CPI-SI command line interface
} IDEGUIFocus;

// # B.1c CLI Panel [CLI]
//
// Command-line interface for CPI-SI communication within the IDE.
// Toggle with Ctrl+` or from View menu.

#define CLI_INPUT_MAX    256
#define CLI_HISTORY_MAX  50
#define CLI_OUTPUT_LINES 8

typedef struct {
    char input[CLI_INPUT_MAX];           // Current input line
    int input_pos;                       // Cursor position in input
    int input_len;                       // Length of input

    char history[CLI_HISTORY_MAX][CLI_INPUT_MAX];  // Command history
    int history_count;                   // Number of history entries
    int history_pos;                     // Position when navigating history

    char output[CLI_OUTPUT_LINES][256];  // Output lines (scrolling buffer)
    int output_start;                    // First visible output line

    bool visible;                        // CLI panel visible
    int height;                          // Panel height in rows
} IDEGUICLIPanel;

// # B.2 GUI State [STATE]

typedef struct {
    IDEBuffer* buffer;          // The buffer being edited
    IDEGUIConfig config;        // Display configuration
    IDEDiagnostics* diags;      // Current diagnostics

    // Dimensions (in characters)
    int32_t screen_rows;
    int32_t screen_cols;
    int32_t edit_rows;          // Rows available for editing
    int32_t edit_cols;          // Cols available for editing

    // Menu bar
    IDEGUIMenuBar menubar;
    IDEGUIFocus focus;

    // CLI panel for CPI-SI communication
    IDEGUICLIPanel cli;

    // Status
    char status_message[256];
    bool running;
    bool needs_redraw;
    bool platform_initialized;  // True if platform layer was initialized (for cleanup)
} IDEGUI;

// # B.3 GUI Lifecycle [LIFECYCLE]

// Initialize GUI (starts Cornerstone display)
IDEGUI* ide_gui_create(IDEBuffer* buffer);

// Clean up GUI (shuts down display)
void ide_gui_destroy(IDEGUI* gui);

// # B.4 Display [DISPLAY]

// Full redraw
void ide_gui_draw(IDEGUI* gui);

// Draw just the current line
void ide_gui_draw_line(IDEGUI* gui, uint32_t line);

// Draw status bar
void ide_gui_draw_status(IDEGUI* gui);

// Set status message
void ide_gui_set_status(IDEGUI* gui, const char* format, ...);

// # B.5 Input [INPUT]

// Handle single keypress
void ide_gui_handle_key(IDEGUI* gui, DisplayKey key);

// Main event loop
void ide_gui_run(IDEGUI* gui);

// # B.6 Commands [COMMANDS]

// Save file
bool ide_gui_save(IDEGUI* gui);

// Prompt for input
char* ide_gui_prompt(IDEGUI* gui, const char* message);

// # B.7 Navigation [NAVIGATION]

// Scroll viewport to show cursor
void ide_gui_ensure_visible(IDEGUI* gui);

// Go to line
void ide_gui_goto_line(IDEGUI* gui, uint32_t line);

// # B.8 CLI Panel [CLI_FUNCTIONS]

// Toggle CLI panel visibility
void ide_gui_cli_toggle(IDEGUI* gui);

// Execute command in CLI
void ide_gui_cli_execute(IDEGUI* gui);

// Add output line to CLI
void ide_gui_cli_output(IDEGUI* gui, const char* format, ...);

// Handle key input in CLI mode
void ide_gui_cli_handle_key(IDEGUI* gui, DisplayKey key);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Kingdom Technology excludes no one."
// The GUI provides full editing with modern accessibility.

#endif // BERESHIT_OMNI_IDE_GUI_H

// =============================================================================
// END CLOSING
// =============================================================================
