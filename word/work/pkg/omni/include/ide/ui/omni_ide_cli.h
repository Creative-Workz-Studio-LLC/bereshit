// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide-cli
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_ide_cli.h — OmniCode IDE Base Implementation
// Uses only display_* abstraction — works across CLI/TUI/GUI modes

#ifndef BERESHIT_OMNI_IDE_CLI_H
#define BERESHIT_OMNI_IDE_CLI_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-cli
// Title: OmniCode IDE Base Implementation
// Type: Header
// Role: Display-abstracted editor — single implementation for all modes

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
#include "display.hal.h"     // Cornerstone display abstraction (HAL layer)
#include "../compat/cornerstone_compat.h"  // Key compatibility for Phase A
#include <stdbool.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 CLI Configuration [CONFIG]
//
// Same structure as TUI — base implementation uses identical config.

typedef struct {
    bool show_line_numbers;     // Display line numbers
    bool show_block_colors;     // Color by block type
    bool show_status_bar;       // Show status line
    bool syntax_highlight;      // Enable syntax highlighting
    int tab_width;              // Tab display width
    bool soft_wrap;             // Wrap long lines
} IDECLIConfig;

// # B.2 Style Definitions [STYLES]
//
// DisplayStyle constants for consistent theming across modes.

// Token styles
extern const DisplayStyle IDE_STYLE_NORMAL;
extern const DisplayStyle IDE_STYLE_PRAGMA;
extern const DisplayStyle IDE_STYLE_KEYWORD;
extern const DisplayStyle IDE_STYLE_COMMENT;
extern const DisplayStyle IDE_STYLE_STRING;
extern const DisplayStyle IDE_STYLE_NUMBER;
extern const DisplayStyle IDE_STYLE_BLOCK;
extern const DisplayStyle IDE_STYLE_SECTION;
extern const DisplayStyle IDE_STYLE_ERROR;
extern const DisplayStyle IDE_STYLE_STATUS;
extern const DisplayStyle IDE_STYLE_LINENO;

// Block styles
extern const DisplayStyle IDE_STYLE_METADATA;
extern const DisplayStyle IDE_STYLE_SETUP;
extern const DisplayStyle IDE_STYLE_BODY;
extern const DisplayStyle IDE_STYLE_CLOSING;
extern const DisplayStyle IDE_STYLE_HEADER;
extern const DisplayStyle IDE_STYLE_CONTEXT;
extern const DisplayStyle IDE_STYLE_CONTENT;
extern const DisplayStyle IDE_STYLE_FOOTER;

// Menu styles
extern const DisplayStyle IDE_STYLE_MENU_BAR;
extern const DisplayStyle IDE_STYLE_MENU_ITEM;
extern const DisplayStyle IDE_STYLE_MENU_SELECT;
extern const DisplayStyle IDE_STYLE_MENU_HOTKEY;

// # B.3 Menu System [MENU]
//
// Same structure as TUI — shared menu definitions.

#define MENU_MAX_ITEMS 12

typedef enum {
    MENU_NONE = -1,
    MENU_FILE = 0,
    MENU_EDIT,
    MENU_BUILD,         // Build/Run actions
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
    IDEMenuItem items[MENU_MAX_ITEMS];
    int item_count;
} IDEMenu;

typedef struct {
    IDEMenu menus[MENU_COUNT];
    IDEMenuType active_menu;    // Currently open menu (-1 = none)
    int active_item;            // Selected item in active menu
    bool visible;               // Menu bar visible
} IDEMenuBar;

// # B.4 CLI State [STATE]

typedef enum {
    FOCUS_EDITOR,
    FOCUS_FILE_BROWSER,
    FOCUS_BLOCK_OUTLINE,
    FOCUS_MENU
} IDECLIFocus;

typedef struct {
    IDEBuffer* buffer;          // The buffer being edited
    IDECLIConfig config;        // Display configuration
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
    IDECLIFocus focus;          // Which panel has focus
    bool panels_visible;        // Toggle panels on/off

    // Status
    char status_message[256];
    bool running;
    bool needs_redraw;
} IDECLI;

// # B.5 CLI Lifecycle [LIFECYCLE]

// Initialize CLI with specified display mode
// mode: DISPLAY_TUI, DISPLAY_GUI, or DISPLAY_CLI
IDECLI* ide_cli_create(IDEBuffer* buffer, DisplayMode mode);

// Clean up CLI (ends display layer)
void ide_cli_destroy(IDECLI* cli);

// # B.6 Display [DISPLAY]

// Full redraw
void ide_cli_draw(IDECLI* cli);

// Draw just the current line
void ide_cli_draw_line(IDECLI* cli, uint32_t line);

// Draw status bar
void ide_cli_draw_status(IDECLI* cli);

// Set status message
void ide_cli_set_status(IDECLI* cli, const char* format, ...);

// # B.7 Input [INPUT]

// Handle single keypress (DisplayKey)
void ide_cli_handle_key(IDECLI* cli, DisplayKey key);

// Main event loop
void ide_cli_run(IDECLI* cli);

// # B.8 Commands [COMMANDS]

// Open file
bool ide_cli_open(IDECLI* cli, const char* filepath);

// Save file
bool ide_cli_save(IDECLI* cli);

// Save as
bool ide_cli_save_as(IDECLI* cli, const char* filepath);

// Prompt for input
char* ide_cli_prompt(IDECLI* cli, const char* message);

// # B.9 Navigation [NAVIGATION]

// Scroll viewport to show cursor
void ide_cli_ensure_visible(IDECLI* cli);

// Go to line
void ide_cli_goto_line(IDECLI* cli, uint32_t line);

// Find text
void ide_cli_find(IDECLI* cli, const char* pattern);

// # B.10 Style Helpers [STYLE_HELPERS]

// Get DisplayStyle for block type
DisplayStyle ide_block_style(IDEBlockType block);

// Get DisplayStyle for token type
DisplayStyle ide_token_style(IDETokenType token);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Kingdom Technology excludes no one."
// The CLI base provides full editing across all display modes.
//
// This is the canonical implementation — TUI and GUI extend or replace
// specific functions as needed, but the core logic lives here.

#endif // BERESHIT_OMNI_IDE_CLI_H

// =============================================================================
// END CLOSING
// =============================================================================
