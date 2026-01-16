// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide-cli-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_ide_cli.c — OmniCode IDE Base Implementation
// Uses only display_* abstraction — works across CLI/TUI/GUI modes

#define _GNU_SOURCE

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-cli-impl
// Grounding: Proverbs 22:29 - "Seest thou a man diligent in his work?"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_ide_cli.h"
#include "display.h"  // Cornerstone display abstraction
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/stat.h>

// Control key macro (converts 'q' to Ctrl+Q code)
#define CTRL(c) ((c) & 037)

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Style Definitions
// -----------------------------------------------------------------------------

// Token styles
const DisplayStyle IDE_STYLE_NORMAL = {
    .fg = DISPLAY_COLOR_DEFAULT,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_PRAGMA = {
    .fg = DISPLAY_COLOR_MAGENTA,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_KEYWORD = {
    .fg = DISPLAY_COLOR_YELLOW,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_COMMENT = {
    .fg = DISPLAY_COLOR_GREEN,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_STRING = {
    .fg = DISPLAY_COLOR_RED,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_NUMBER = {
    .fg = DISPLAY_COLOR_CYAN,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_BLOCK = {
    .fg = DISPLAY_COLOR_YELLOW,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_SECTION = {
    .fg = DISPLAY_COLOR_BLUE,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_ERROR = {
    .fg = DISPLAY_COLOR_WHITE,
    .bg = DISPLAY_COLOR_RED,
    .bold = true
};

const DisplayStyle IDE_STYLE_STATUS = {
    .fg = DISPLAY_COLOR_BLACK,
    .bg = DISPLAY_COLOR_WHITE,
    .bold = false
};

const DisplayStyle IDE_STYLE_LINENO = {
    .fg = DISPLAY_COLOR_CYAN,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

// Block styles
const DisplayStyle IDE_STYLE_METADATA = {
    .fg = DISPLAY_COLOR_CYAN,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_SETUP = {
    .fg = DISPLAY_COLOR_BLUE,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_BODY = {
    .fg = DISPLAY_COLOR_GREEN,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_CLOSING = {
    .fg = DISPLAY_COLOR_RED,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_HEADER = {
    .fg = DISPLAY_COLOR_MAGENTA,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_CONTEXT = {
    .fg = DISPLAY_COLOR_YELLOW,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_CONTENT = {
    .fg = DISPLAY_COLOR_GREEN,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

const DisplayStyle IDE_STYLE_FOOTER = {
    .fg = DISPLAY_COLOR_RED,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

// Menu styles
const DisplayStyle IDE_STYLE_MENU_BAR = {
    .fg = DISPLAY_COLOR_BLACK,
    .bg = DISPLAY_COLOR_WHITE,
    .bold = false
};

const DisplayStyle IDE_STYLE_MENU_ITEM = {
    .fg = DISPLAY_COLOR_BLACK,
    .bg = DISPLAY_COLOR_WHITE,
    .bold = false
};

const DisplayStyle IDE_STYLE_MENU_SELECT = {
    .fg = DISPLAY_COLOR_WHITE,
    .bg = DISPLAY_COLOR_BLUE,
    .bold = false
};

const DisplayStyle IDE_STYLE_MENU_HOTKEY = {
    .fg = DISPLAY_COLOR_RED,
    .bg = DISPLAY_COLOR_WHITE,
    .bold = false
};

// -----------------------------------------------------------------------------
// B.2 Style Helpers
// -----------------------------------------------------------------------------

DisplayStyle ide_block_style(IDEBlockType b) {
    switch (b) {
        case BLOCK_3_METADATA:
        case BLOCK_4_METADATA:
        case BLOCK_5_METADATA:
            return IDE_STYLE_METADATA;
        case BLOCK_4_SETUP:
            return IDE_STYLE_SETUP;
        case BLOCK_5_HEADER:
            return IDE_STYLE_HEADER;
        case BLOCK_5_CONTEXT:
            return IDE_STYLE_CONTEXT;
        case BLOCK_4_BODY:
        case BLOCK_3_CONTENT:
        case BLOCK_5_CONTENT:
            return IDE_STYLE_CONTENT;
        case BLOCK_3_CLOSING:
        case BLOCK_4_CLOSING:
            return IDE_STYLE_CLOSING;
        case BLOCK_5_FOOTER:
            return IDE_STYLE_FOOTER;
        default:
            return IDE_STYLE_NORMAL;
    }
}

DisplayStyle ide_token_style(IDETokenType t) {
    switch (t) {
        case TOKEN_PRAGMA:
            return IDE_STYLE_PRAGMA;
        case TOKEN_KEYWORD:
            return IDE_STYLE_KEYWORD;
        case TOKEN_COMMENT:
            return IDE_STYLE_COMMENT;
        case TOKEN_STRING:
            return IDE_STYLE_STRING;
        case TOKEN_NUMBER:
            return IDE_STYLE_NUMBER;
        case TOKEN_BLOCK_MARKER:
            return IDE_STYLE_BLOCK;
        case TOKEN_SECTION:
            return IDE_STYLE_SECTION;
        case TOKEN_ERROR:
            return IDE_STYLE_ERROR;
        default:
            return IDE_STYLE_NORMAL;
    }
}

// -----------------------------------------------------------------------------
// B.3 Layout Calculation
// -----------------------------------------------------------------------------

// Minimum sizes
#define MIN_PANEL_WIDTH   20
#define MAX_PANEL_WIDTH   40
#define MIN_EDITOR_WIDTH  40
#define PANEL_WIDTH_PCT   25   // 25% of screen for left panels
#define BOTTOM_PANEL_PCT  25   // 25% of height for bottom panel

// Recalculate all dimensions based on current terminal size
static void ide_cli_recalc_layout(IDECLI* cli) {
    if (!cli) return;

    // Get current terminal size via display abstraction
    int32_t cols, rows;
    display_get_size(&cols, &rows);
    cli->screen_cols = cols;
    cli->screen_rows = rows;

    // Calculate proportional panel width
    int panel_width = 0;
    if (cli->panels_visible && cli->layout) {
        panel_width = (cli->screen_cols * PANEL_WIDTH_PCT) / 100;

        // Clamp to min/max
        if (panel_width < MIN_PANEL_WIDTH) panel_width = MIN_PANEL_WIDTH;
        if (panel_width > MAX_PANEL_WIDTH) panel_width = MAX_PANEL_WIDTH;

        // But ensure editor has enough space
        if (cli->screen_cols - panel_width < MIN_EDITOR_WIDTH) {
            panel_width = cli->screen_cols - MIN_EDITOR_WIDTH;
            if (panel_width < 0) panel_width = 0;
        }

        cli->layout->left_width = panel_width;
    }

    // Calculate bottom panel height (for validation/diagnostics)
    int bottom_height = 0;
    if (cli->panels_visible && cli->diags && cli->diags->count > 0) {
        bottom_height = (cli->screen_rows * BOTTOM_PANEL_PCT) / 100;
        if (bottom_height < 4) bottom_height = 4;   // Minimum for useful display
        if (bottom_height > 10) bottom_height = 10; // Cap it
        cli->layout->bottom_height = bottom_height;
    } else {
        if (cli->layout) cli->layout->bottom_height = 0;
    }

    // Status bar takes 2 rows, menu bar takes 1 row
    int status_rows = cli->config.show_status_bar ? 2 : 0;
    int menu_rows = cli->menubar.visible ? 1 : 0;

    // Calculate editing area
    cli->edit_rows = cli->screen_rows - status_rows - bottom_height - menu_rows;
    cli->edit_cols = cli->screen_cols - panel_width - (cli->config.show_line_numbers ? 6 : 0);

    // Ensure minimums
    if (cli->edit_rows < 1) cli->edit_rows = 1;
    if (cli->edit_cols < 10) cli->edit_cols = 10;

    // Update buffer viewport
    if (cli->buffer) {
        cli->buffer->visible_lines = cli->edit_rows;
        cli->buffer->visible_cols = cli->edit_cols;
    }
}

// -----------------------------------------------------------------------------
// B.4 Menu System
// -----------------------------------------------------------------------------

// Menu action codes
typedef enum {
    ACTION_NONE = 0,
    // File menu
    ACTION_NEW,
    ACTION_OPEN,
    ACTION_SAVE,
    ACTION_SAVE_AS,
    ACTION_QUIT,
    // Edit menu
    ACTION_UNDO,
    ACTION_REDO,
    ACTION_CUT,
    ACTION_COPY,
    ACTION_PASTE,
    ACTION_FIND,
    ACTION_GOTO,
    // Build menu
    ACTION_BUILD,               // Compile current file
    ACTION_RUN,                 // Run compiled output
    ACTION_BUILD_RUN,           // Build and run
    ACTION_RUN_CORNERSTONE,     // Run in Cornerstone engine
    ACTION_CLEAN,               // Clean build artifacts
    // View menu
    ACTION_TOGGLE_PANELS,
    ACTION_TOGGLE_LINENO,
    ACTION_TOGGLE_COLORS,
    // Help menu
    ACTION_ABOUT,
    ACTION_KEYS
} IDEMenuAction;

// Initialize menu bar with default menus
static void ide_menubar_init(IDEMenuBar* menubar) {
    memset(menubar, 0, sizeof(IDEMenuBar));
    menubar->active_menu = MENU_NONE;
    menubar->active_item = 0;
    menubar->visible = true;

    // File menu
    IDEMenu* file = &menubar->menus[MENU_FILE];
    file->label = "File";
    file->hotkey = 'F';
    file->item_count = 5;
    file->items[0] = (IDEMenuItem){"New",      "Ctrl+N", ACTION_NEW,     false};
    file->items[1] = (IDEMenuItem){"Open...",  "Ctrl+O", ACTION_OPEN,    false};
    file->items[2] = (IDEMenuItem){"Save",     "Ctrl+S", ACTION_SAVE,    false};
    file->items[3] = (IDEMenuItem){"Save As...", NULL,   ACTION_SAVE_AS, true};
    file->items[4] = (IDEMenuItem){"Quit",     "Ctrl+Q", ACTION_QUIT,    false};

    // Edit menu
    IDEMenu* edit = &menubar->menus[MENU_EDIT];
    edit->label = "Edit";
    edit->hotkey = 'E';
    edit->item_count = 6;
    edit->items[0] = (IDEMenuItem){"Undo",     "Ctrl+Z", ACTION_UNDO,  false};
    edit->items[1] = (IDEMenuItem){"Redo",     "Ctrl+Y", ACTION_REDO,  true};
    edit->items[2] = (IDEMenuItem){"Cut",      "Ctrl+X", ACTION_CUT,   false};
    edit->items[3] = (IDEMenuItem){"Copy",     "Ctrl+C", ACTION_COPY,  false};
    edit->items[4] = (IDEMenuItem){"Paste",    "Ctrl+V", ACTION_PASTE, true};
    edit->items[5] = (IDEMenuItem){"Find...",  "Ctrl+F", ACTION_FIND,  false};

    // Build menu
    IDEMenu* build = &menubar->menus[MENU_BUILD];
    build->label = "Build";
    build->hotkey = 'B';
    build->item_count = 5;
    build->items[0] = (IDEMenuItem){"Build",           "F5",     ACTION_BUILD,           false};
    build->items[1] = (IDEMenuItem){"Run",             "F6",     ACTION_RUN,             false};
    build->items[2] = (IDEMenuItem){"Build & Run",     "F7",     ACTION_BUILD_RUN,       true};
    build->items[3] = (IDEMenuItem){"Run in Cornerstone", "F8",  ACTION_RUN_CORNERSTONE, false};
    build->items[4] = (IDEMenuItem){"Clean",           NULL,     ACTION_CLEAN,           false};

    // View menu
    IDEMenu* view = &menubar->menus[MENU_VIEW];
    view->label = "View";
    view->hotkey = 'V';
    view->item_count = 3;
    view->items[0] = (IDEMenuItem){"Toggle Panels",    "Ctrl+B", ACTION_TOGGLE_PANELS, false};
    view->items[1] = (IDEMenuItem){"Line Numbers",     NULL,     ACTION_TOGGLE_LINENO, false};
    view->items[2] = (IDEMenuItem){"Block Colors",     NULL,     ACTION_TOGGLE_COLORS, false};

    // Help menu
    IDEMenu* help = &menubar->menus[MENU_HELP];
    help->label = "Help";
    help->hotkey = 'H';
    help->item_count = 2;
    help->items[0] = (IDEMenuItem){"Key Bindings", "F1",  ACTION_KEYS,  false};
    help->items[1] = (IDEMenuItem){"About",        NULL,  ACTION_ABOUT, false};
}

// Draw menu bar at top of screen
static void ide_cli_draw_menubar(IDECLI* cli) {
    if (!cli || !cli->menubar.visible) return;

    // Draw menu bar background
    display_set_style(IDE_STYLE_MENU_BAR);
    display_set_cursor(0, 0);  // x, y (display layer order)
    for (int i = 0; i < cli->screen_cols; i++) display_putchar(' ');

    // Draw menu titles
    int x = 1;
    for (int m = 0; m < MENU_COUNT; m++) {
        IDEMenu* menu = &cli->menubar.menus[m];
        display_set_cursor(x, 0);  // x, y

        if (m == cli->menubar.active_menu) {
            display_set_style(IDE_STYLE_MENU_SELECT);
        }

        display_printf(" %s ", menu->label);

        if (m == cli->menubar.active_menu) {
            display_set_style(IDE_STYLE_MENU_BAR);
        }

        x += strlen(menu->label) + 2;
    }

    display_reset_style();

    // Draw F10 hint on right
    display_set_style(IDE_STYLE_MENU_BAR);
    display_printf_at(cli->screen_cols - 12, 0, " F10: Menu ");
    display_reset_style();
}

// Draw dropdown menu
static void ide_cli_draw_menu_dropdown(IDECLI* cli) {
    if (!cli || cli->menubar.active_menu == MENU_NONE) return;

    IDEMenu* menu = &cli->menubar.menus[cli->menubar.active_menu];

    // Calculate menu position
    int x = 1;
    for (int m = 0; m < cli->menubar.active_menu; m++) {
        x += strlen(cli->menubar.menus[m].label) + 2;
    }

    // Calculate menu width
    int max_width = 0;
    for (int i = 0; i < menu->item_count; i++) {
        int w = strlen(menu->items[i].label);
        if (menu->items[i].shortcut) {
            w += 2 + strlen(menu->items[i].shortcut);
        }
        if (w > max_width) max_width = w;
    }
    max_width += 4;  // Padding

    // Draw menu items with shortcuts inline like "Save (Ctrl+S)"
    for (int i = 0; i < menu->item_count; i++) {
        display_set_cursor(x, i + 1);  // x, y

        if (i == cli->menubar.active_item) {
            display_set_style(IDE_STYLE_MENU_SELECT);
        } else {
            display_set_style(IDE_STYLE_MENU_ITEM);
        }

        // Build label with shortcut: "Save (Ctrl+S)"
        if (menu->items[i].shortcut) {
            display_printf(" %s (%s) ", menu->items[i].label, menu->items[i].shortcut);
        } else {
            display_printf(" %s ", menu->items[i].label);
        }

        display_reset_style();
    }
}

// Execute menu action
static void ide_menu_execute(IDECLI* cli, IDEMenuAction action) {
    switch (action) {
        case ACTION_NEW:
            ide_cli_set_status(cli, "New file (not implemented yet)");
            break;
        case ACTION_OPEN:
            {
                char* path = ide_cli_prompt(cli, "Open file: ");
                if (path && strlen(path) > 0) {
                    IDEBuffer* new_buf = ide_buffer_load(path);
                    if (new_buf) {
                        ide_buffer_free(cli->buffer);
                        cli->buffer = new_buf;
                        ide_block_outline_update(&cli->block_outline, new_buf);
                        ide_template_info_update(&cli->template_info, new_buf);
                        ide_validate_all(new_buf, cli->diags);
                        ide_cli_set_status(cli, "Opened: %s", path);
                    } else {
                        ide_cli_set_status(cli, "Failed to open: %s", path);
                    }
                    free(path);
                }
            }
            break;
        case ACTION_SAVE:
            if (ide_cli_save(cli)) {
                ide_cli_set_status(cli, "Saved: %s", cli->buffer->filepath);
            } else {
                ide_cli_set_status(cli, "Save failed!");
            }
            break;
        case ACTION_SAVE_AS:
            {
                char* path = ide_cli_prompt(cli, "Save as: ");
                if (path && strlen(path) > 0) {
                    if (ide_buffer_save_as(cli->buffer, path)) {
                        ide_cli_set_status(cli, "Saved: %s", path);
                    } else {
                        ide_cli_set_status(cli, "Save failed!");
                    }
                    free(path);
                }
            }
            break;
        case ACTION_QUIT:
            if (cli->buffer->modified) {
                ide_cli_set_status(cli, "Unsaved changes! Press Ctrl+Q to quit.");
            } else {
                cli->running = false;
            }
            break;
        case ACTION_TOGGLE_PANELS:
            cli->panels_visible = !cli->panels_visible;
            if (!cli->panels_visible) cli->focus = FOCUS_EDITOR;
            ide_cli_recalc_layout(cli);
            ide_cli_set_status(cli, "Panels %s", cli->panels_visible ? "visible" : "hidden");
            break;
        case ACTION_TOGGLE_LINENO:
            cli->config.show_line_numbers = !cli->config.show_line_numbers;
            ide_cli_recalc_layout(cli);
            ide_cli_set_status(cli, "Line numbers %s", cli->config.show_line_numbers ? "on" : "off");
            break;
        case ACTION_TOGGLE_COLORS:
            cli->config.show_block_colors = !cli->config.show_block_colors;
            ide_cli_set_status(cli, "Block colors %s", cli->config.show_block_colors ? "on" : "off");
            break;
        case ACTION_GOTO:
            {
                char* input = ide_cli_prompt(cli, "Go to line: ");
                if (input) {
                    int line = atoi(input);
                    if (line > 0) {
                        ide_cli_goto_line(cli, line - 1);
                    }
                    free(input);
                }
            }
            break;
        case ACTION_KEYS:
            ide_cli_set_status(cli,
                "F10:Menu Tab:Panels Ctrl+S:Save Ctrl+Q:Quit Ctrl+G:Goto Ctrl+B:Toggle");
            break;
        case ACTION_ABOUT:
            ide_cli_set_status(cli,
                "OmniCode IDE v0.1 - Kingdom Technology | \"In the beginning God created...\"");
            break;
        default:
            ide_cli_set_status(cli, "Action not implemented");
            break;
    }

    // Close menu after action
    cli->menubar.active_menu = MENU_NONE;
    cli->focus = FOCUS_EDITOR;
}

// -----------------------------------------------------------------------------
// B.5 CLI Lifecycle
// -----------------------------------------------------------------------------

IDECLI* ide_cli_create(IDEBuffer* buffer, DisplayMode mode) {
    IDECLI* cli = malloc(sizeof(IDECLI));
    if (!cli) return NULL;

    memset(cli, 0, sizeof(IDECLI));
    cli->buffer = buffer;
    cli->running = true;
    cli->needs_redraw = true;

    // Default config
    cli->config.show_line_numbers = true;
    cli->config.show_block_colors = true;
    cli->config.show_status_bar = true;
    cli->config.syntax_highlight = true;
    cli->config.tab_width = 4;
    cli->config.soft_wrap = false;

    // Initialize display layer with requested mode
    // Display routes all calls to the appropriate backend
    DisplayConfig display_cfg = {
        .mode = mode,
        .width = 0,   // Auto-detect from terminal
        .height = 0,  // Auto-detect from terminal
        .title = "OmniCode IDE"
    };
    display_init(&display_cfg);

    // Create diagnostics first (needed for layout calculation)
    cli->diags = ide_diagnostics_create();
    cli->validation.diags = cli->diags;

    // Menu bar
    ide_menubar_init(&cli->menubar);

    // Panel system
    cli->layout = ide_layout_create_default();
    cli->panels_visible = true;
    cli->focus = FOCUS_EDITOR;

    // Calculate layout (proportional, auto-sizing)
    ide_cli_recalc_layout(cli);

    // Initialize file browser with Bereshit root (derived from buffer path)
    if (buffer && buffer->filepath) {
        char root[512];
        strncpy(root, buffer->filepath, sizeof(root) - 1);
        char* last_slash = strrchr(root, '/');
        if (last_slash) *last_slash = '\0';

        // Walk up looking for void/word/tov
        while (strlen(root) > 1) {
            char check[600];
            snprintf(check, sizeof(check), "%s/void", root);
            struct stat st;
            if (stat(check, &st) == 0 && S_ISDIR(st.st_mode)) {
                ide_file_browser_init(&cli->file_browser, root);
                break;
            }
            last_slash = strrchr(root, '/');
            if (last_slash) *last_slash = '\0';
            else break;
        }
    }

    // Initialize block outline from buffer
    if (buffer) {
        ide_block_outline_update(&cli->block_outline, buffer);
        ide_template_info_update(&cli->template_info, buffer);
    }

    ide_cli_set_status(cli, "OmniCode IDE | Tab: Panels | Ctrl+B: Toggle | Ctrl+S: Save | Ctrl+Q: Quit");

    return cli;
}

void ide_cli_destroy(IDECLI* cli) {
    if (!cli) return;

    display_shutdown();
    ide_layout_free(cli->layout);
    ide_diagnostics_free(cli->diags);
    free(cli);
}

// -----------------------------------------------------------------------------
// B.6 Display
// -----------------------------------------------------------------------------

void ide_cli_draw_line(IDECLI* cli, uint32_t line_num) {
    if (!cli || !cli->buffer) return;

    IDEBuffer* buf = cli->buffer;
    int row = line_num - buf->scroll_line;
    if (row < 0 || row >= cli->edit_rows) return;

    display_set_cursor(0, row);  // x, y
    display_clear_to_eol();

    // Line number
    if (cli->config.show_line_numbers) {
        display_set_style(IDE_STYLE_LINENO);
        display_printf("%5u ", line_num + 1);
        display_reset_style();
    }

    if (line_num >= buf->line_count) return;

    IDELine* line = &buf->lines[line_num];

    // Block color indicator
    if (cli->config.show_block_colors && line->block != BLOCK_NONE) {
        display_set_style(ide_block_style(line->block));
    }

    // Draw text with syntax highlighting
    if (cli->config.syntax_highlight && line->token_count > 0) {
        uint32_t pos = 0;
        for (uint32_t t = 0; t < line->token_count; t++) {
            IDEToken* tok = &line->tokens[t];

            // Print before token
            while (pos < tok->start && pos < line->length) {
                display_putchar(line->text[pos++]);
            }

            // Print token with color
            display_set_style(ide_token_style(tok->type));
            while (pos < tok->end && pos < line->length) {
                display_putchar(line->text[pos++]);
            }
            display_reset_style();

            // Restore block color if active
            if (cli->config.show_block_colors && line->block != BLOCK_NONE) {
                display_set_style(ide_block_style(line->block));
            }
        }

        // Print remainder
        while (pos < line->length) {
            display_putchar(line->text[pos++]);
        }
    } else {
        // No syntax highlighting - just print
        display_printf("%s", line->text);
    }

    display_reset_style();
}

// Get current block name from cursor position
static const char* get_current_block_name(IDEBuffer* buf) {
    if (!buf || buf->cursor.line >= buf->line_count) return "";
    IDEBlockType block = buf->lines[buf->cursor.line].block;
    switch (block) {
        case BLOCK_3_METADATA: case BLOCK_4_METADATA: case BLOCK_5_METADATA: return "META";
        case BLOCK_4_SETUP: return "SETUP";
        case BLOCK_5_HEADER: return "HEAD";
        case BLOCK_5_CONTEXT: return "CTX";
        case BLOCK_4_BODY: return "BODY";
        case BLOCK_3_CONTENT: case BLOCK_5_CONTENT: return "CONT";
        case BLOCK_3_CLOSING: case BLOCK_4_CLOSING: return "CLOSE";
        case BLOCK_5_FOOTER: return "FOOT";
        default: return "";
    }
}

void ide_cli_draw_status(IDECLI* cli) {
    if (!cli || !cli->config.show_status_bar) return;

    IDEBuffer* buf = cli->buffer;
    int row = cli->screen_rows - 1;

    // Status bar background
    display_set_style(IDE_STYLE_STATUS);
    display_set_cursor(0, row - 1);  // x, y
    for (int i = 0; i < cli->screen_cols; i++) display_putchar(' ');
    display_set_cursor(0, row);  // x, y
    for (int i = 0; i < cli->screen_cols; i++) display_putchar(' ');

    // File info line (top status bar)
    display_set_cursor(0, row - 1);  // x, y

    // Structure type
    const char* structure = "";
    if (buf) {
        switch (ide_get_structure(buf)) {
            case STRUCTURE_3_BLOCK: structure = "3-Block"; break;
            case STRUCTURE_4_BLOCK: structure = "4-Block"; break;
            case STRUCTURE_5_BLOCK: structure = "5-Block"; break;
            default: structure = "Unknown"; break;
        }
    }

    // Current block
    const char* block_name = get_current_block_name(buf);

    // Diagnostics count
    int diag_count = cli->diags ? cli->diags->count : 0;

    // Left side: file info
    display_printf(" %s%s",
           buf && buf->filepath ? buf->filepath : "[New File]",
           buf && buf->modified ? " [+]" : "");

    // Right side: position, block, structure, diagnostics
    if (buf) {
        char info[128];
        if (diag_count > 0) {
            snprintf(info, sizeof(info), "%s | %s | Ln %u Col %u | ! %d",
                     structure, block_name[0] ? block_name : "-",
                     buf->cursor.line + 1, buf->cursor.col + 1, diag_count);
        } else {
            snprintf(info, sizeof(info), "%s | %s | Ln %u Col %u | OK",
                     structure, block_name[0] ? block_name : "-",
                     buf->cursor.line + 1, buf->cursor.col + 1);
        }
        int info_col = cli->screen_cols - strlen(info) - 1;
        if (info_col > 0) {
            display_printf_at(info_col, row - 1, "%s", info);
        }
    }

    // Status message line (bottom status bar)
    display_set_cursor(0, row);  // x, y
    display_printf(" %s", cli->status_message);

    // Help hints on right side of bottom bar
    const char* help = "^S Save | ^Q Quit | ^G Goto | ^F Find | F10 Menu";
    int help_col = cli->screen_cols - strlen(help) - 1;
    if (help_col > (int)strlen(cli->status_message) + 2) {
        display_printf_at(help_col, row, "%s", help);
    }

    display_reset_style();
}

void ide_cli_draw(IDECLI* cli) {
    if (!cli) return;

    display_clear();

    // Draw menu bar first (takes row 0)
    ide_cli_draw_menubar(cli);

    int panel_width = cli->panels_visible ? cli->layout->left_width : 0;
    int bottom_height = cli->layout ? cli->layout->bottom_height : 0;
    int editor_x = panel_width;
    int menu_offset = cli->menubar.visible ? 1 : 0;  // Offset for menu bar

    // Draw left panels if visible (offset by menu bar)
    if (cli->panels_visible && cli->layout && panel_width > 0) {
        // Split left panels: 60% file browser, 40% block outline
        int file_browser_h = (cli->edit_rows * 60) / 100;
        int block_outline_h = cli->edit_rows - file_browser_h;

        // File browser (top-left)
        ide_panel_draw_file_browser(&cli->file_browser, 0, menu_offset,
                                    panel_width, file_browser_h,
                                    cli->focus == FOCUS_FILE_BROWSER);

        // Block outline (bottom-left)
        ide_panel_draw_block_outline(&cli->block_outline, 0, menu_offset + file_browser_h,
                                     panel_width, block_outline_h,
                                     cli->focus == FOCUS_BLOCK_OUTLINE);
    }

    // Draw editor lines (offset by panel width and menu bar)
    for (int i = 0; i < cli->edit_rows; i++) {
        uint32_t line_num = cli->buffer->scroll_line + i;
        int row = i + menu_offset;

        display_set_cursor(editor_x, row);  // x, y
        display_clear_to_eol();

        // Line number
        if (cli->config.show_line_numbers) {
            display_set_style(IDE_STYLE_LINENO);
            display_printf("%5u ", line_num + 1);
            display_reset_style();
        }

        if (line_num < cli->buffer->line_count) {
            IDELine* line = &cli->buffer->lines[line_num];

            // Block color indicator
            if (cli->config.show_block_colors && line->block != BLOCK_NONE) {
                display_set_style(ide_block_style(line->block));
            }

            // Draw text with syntax highlighting
            if (cli->config.syntax_highlight && line->token_count > 0) {
                uint32_t pos = 0;
                for (uint32_t t = 0; t < line->token_count; t++) {
                    IDEToken* tok = &line->tokens[t];

                    // Print before token
                    while (pos < tok->start && pos < line->length) {
                        display_putchar(line->text[pos++]);
                    }

                    // Print token with color
                    display_set_style(ide_token_style(tok->type));
                    while (pos < tok->end && pos < line->length) {
                        display_putchar(line->text[pos++]);
                    }
                    display_reset_style();

                    // Restore block color if active
                    if (cli->config.show_block_colors && line->block != BLOCK_NONE) {
                        display_set_style(ide_block_style(line->block));
                    }
                }

                // Print remainder
                while (pos < line->length) {
                    display_putchar(line->text[pos++]);
                }
            } else {
                // No syntax highlighting
                display_printf("%s", line->text);
            }

            display_reset_style();
        }
    }

    // Draw bottom panel (validation/diagnostics) if there are issues
    if (cli->panels_visible && bottom_height > 0 && cli->diags && cli->diags->count > 0) {
        int bottom_y = cli->edit_rows + menu_offset;
        int bottom_w = cli->screen_cols;
        ide_panel_draw_validation(&cli->validation, 0, bottom_y, bottom_w, bottom_height);
    }

    // Draw status
    ide_cli_draw_status(cli);

    // Draw menu dropdown (on top of everything if menu active)
    if (cli->focus == FOCUS_MENU && cli->menubar.active_menu != MENU_NONE) {
        ide_cli_draw_menu_dropdown(cli);
    }

    // Position cursor (only if editor has focus)
    if (cli->buffer && cli->focus == FOCUS_EDITOR) {
        IDEBuffer* buf = cli->buffer;
        int row = buf->cursor.line - buf->scroll_line + menu_offset;
        int col = buf->cursor.col + editor_x + (cli->config.show_line_numbers ? 6 : 0);
        display_set_cursor(col, row);  // x, y
        display_show_cursor();
    } else {
        display_hide_cursor();  // Hide cursor when panels/menu focused
    }

    display_refresh();
    cli->needs_redraw = false;
}

void ide_cli_set_status(IDECLI* cli, const char* format, ...) {
    if (!cli) return;

    va_list args;
    va_start(args, format);
    vsnprintf(cli->status_message, sizeof(cli->status_message), format, args);
    va_end(args);
}

// -----------------------------------------------------------------------------
// B.7 Navigation
// -----------------------------------------------------------------------------

void ide_cli_ensure_visible(IDECLI* cli) {
    if (!cli || !cli->buffer) return;

    IDEBuffer* buf = cli->buffer;

    // Scroll up if cursor above viewport
    if (buf->cursor.line < buf->scroll_line) {
        buf->scroll_line = buf->cursor.line;
        cli->needs_redraw = true;
    }

    // Scroll down if cursor below viewport
    if (buf->cursor.line >= buf->scroll_line + cli->edit_rows) {
        buf->scroll_line = buf->cursor.line - cli->edit_rows + 1;
        cli->needs_redraw = true;
    }
}

void ide_cli_goto_line(IDECLI* cli, uint32_t line) {
    if (!cli || !cli->buffer) return;
    ide_cursor_goto(cli->buffer, line, 0);
    ide_cli_ensure_visible(cli);
    cli->needs_redraw = true;
}

void ide_cli_find(IDECLI* cli, const char* pattern) {
    if (!cli || !cli->buffer || !pattern || strlen(pattern) == 0) {
        ide_cli_set_status(cli, "Find: no pattern");
        return;
    }

    IDEBuffer* buf = cli->buffer;
    size_t plen = strlen(pattern);

    // Search from current position forward
    for (uint32_t line = buf->cursor.line; line < buf->line_count; line++) {
        IDELine* ln = &buf->lines[line];
        if (!ln->text) continue;

        // Start column: on current line start after cursor, else from 0
        uint32_t start_col = (line == buf->cursor.line) ? buf->cursor.col + 1 : 0;

        for (uint32_t col = start_col; col + plen <= ln->length; col++) {
            if (strncmp(&ln->text[col], pattern, plen) == 0) {
                // Found! Move cursor here
                ide_cursor_goto(buf, line, col);
                ide_cli_ensure_visible(cli);
                ide_cli_set_status(cli, "Found: \"%s\" at line %u", pattern, line + 1);
                cli->needs_redraw = true;
                return;
            }
        }
    }

    // Wrap around: search from beginning
    for (uint32_t line = 0; line <= buf->cursor.line && line < buf->line_count; line++) {
        IDELine* ln = &buf->lines[line];
        if (!ln->text) continue;

        uint32_t end_col = (line == buf->cursor.line) ? buf->cursor.col : ln->length;

        for (uint32_t col = 0; col + plen <= end_col; col++) {
            if (strncmp(&ln->text[col], pattern, plen) == 0) {
                // Found! Move cursor here
                ide_cursor_goto(buf, line, col);
                ide_cli_ensure_visible(cli);
                ide_cli_set_status(cli, "Found: \"%s\" at line %u (wrapped)", pattern, line + 1);
                cli->needs_redraw = true;
                return;
            }
        }
    }

    ide_cli_set_status(cli, "Not found: \"%s\"", pattern);
    cli->needs_redraw = true;
}

// -----------------------------------------------------------------------------
// B.8 Input Handling
// -----------------------------------------------------------------------------

// NOTE: This section needs DisplayKey values for editor keys.
// Currently using placeholder until display layer is extended.

void ide_cli_handle_key(IDECLI* cli, DisplayKey key) {
    if (!cli || !cli->buffer) return;

    switch (key) {
        // --- Escape: Close menu or cancel ---
        case DISPLAY_KEY_ESCAPE:
            cli->menubar.active_menu = MENU_NONE;
            cli->focus = FOCUS_EDITOR;
            ide_cli_set_status(cli, "Editor");
            cli->needs_redraw = true;
            break;

        // --- Arrow keys: Navigation ---
        case DISPLAY_KEY_UP:
            if (cli->focus == FOCUS_MENU) {
                // Navigate menu items up
                IDEMenu* menu = &cli->menubar.menus[cli->menubar.active_menu];
                if (cli->menubar.active_item > 0) {
                    cli->menubar.active_item--;
                } else {
                    cli->menubar.active_item = menu->item_count - 1;
                }
            } else if (cli->focus == FOCUS_EDITOR) {
                ide_cursor_up(cli->buffer);
                ide_cli_ensure_visible(cli);
            }
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_DOWN:
            if (cli->focus == FOCUS_MENU) {
                // Navigate menu items down
                IDEMenu* menu = &cli->menubar.menus[cli->menubar.active_menu];
                if (cli->menubar.active_item < menu->item_count - 1) {
                    cli->menubar.active_item++;
                } else {
                    cli->menubar.active_item = 0;
                }
            } else if (cli->focus == FOCUS_EDITOR) {
                ide_cursor_down(cli->buffer);
                ide_cli_ensure_visible(cli);
            }
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_LEFT:
            if (cli->focus == FOCUS_MENU) {
                // Switch to previous menu
                if (cli->menubar.active_menu > 0) {
                    cli->menubar.active_menu--;
                } else {
                    cli->menubar.active_menu = MENU_COUNT - 1;
                }
                cli->menubar.active_item = 0;
            } else if (cli->focus == FOCUS_EDITOR) {
                ide_cursor_left(cli->buffer);
            }
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_RIGHT:
            if (cli->focus == FOCUS_MENU) {
                // Switch to next menu
                if (cli->menubar.active_menu < MENU_COUNT - 1) {
                    cli->menubar.active_menu++;
                } else {
                    cli->menubar.active_menu = 0;
                }
                cli->menubar.active_item = 0;
            } else if (cli->focus == FOCUS_EDITOR) {
                ide_cursor_right(cli->buffer);
            }
            cli->needs_redraw = true;
            break;

        // --- Home/End: Line navigation ---
        case DISPLAY_KEY_HOME:
            if (cli->focus == FOCUS_EDITOR) {
                ide_cursor_home(cli->buffer);
            }
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_END:
            if (cli->focus == FOCUS_EDITOR) {
                ide_cursor_end(cli->buffer);
            }
            cli->needs_redraw = true;
            break;

        // --- Page Up/Down: Scroll ---
        case DISPLAY_KEY_PAGEUP:
            if (cli->focus == FOCUS_EDITOR) {
                ide_cursor_page_up(cli->buffer);
                ide_cli_ensure_visible(cli);
            }
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_PAGEDOWN:
            if (cli->focus == FOCUS_EDITOR) {
                ide_cursor_page_down(cli->buffer);
                ide_cli_ensure_visible(cli);
            }
            cli->needs_redraw = true;
            break;

        // --- Editing keys ---
        case DISPLAY_KEY_ENTER:
            if (cli->focus == FOCUS_MENU) {
                // Execute selected menu item
                IDEMenu* menu = &cli->menubar.menus[cli->menubar.active_menu];
                if (cli->menubar.active_item >= 0 && cli->menubar.active_item < menu->item_count) {
                    IDEMenuAction action = (IDEMenuAction)menu->items[cli->menubar.active_item].action;
                    ide_menu_execute(cli, action);
                }
                // Close menu after action
                cli->menubar.active_menu = MENU_NONE;
                cli->focus = FOCUS_EDITOR;
            } else if (cli->focus == FOCUS_EDITOR) {
                ide_newline(cli->buffer);
                ide_cli_ensure_visible(cli);
            }
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_BACKSPACE:
            if (cli->focus == FOCUS_EDITOR) {
                ide_backspace(cli->buffer);
                ide_cli_ensure_visible(cli);
            }
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_DELETE:
            if (cli->focus == FOCUS_EDITOR) {
                ide_delete(cli->buffer);
            }
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_TAB:
            if (cli->focus == FOCUS_EDITOR) {
                // Insert tab (4 spaces or actual tab based on config)
                for (int i = 0; i < cli->config.tab_width; i++) {
                    ide_insert_char(cli->buffer, ' ');
                }
            }
            cli->needs_redraw = true;
            break;

        // --- Ctrl+key commands ---
        case DISPLAY_KEY_CTRL_S:
            ide_cli_save(cli);
            break;

        case DISPLAY_KEY_CTRL_Q:
            if (cli->buffer->modified) {
                ide_cli_set_status(cli, "Unsaved changes! Press Ctrl+Q again to quit.");
                // Simple: just set a flag and check on next Ctrl+Q
            } else {
                cli->running = false;
            }
            break;

        case DISPLAY_KEY_CTRL_G:
            {
                char* input = ide_cli_prompt(cli, "Go to line: ");
                if (input) {
                    int line = atoi(input);
                    if (line > 0) {
                        ide_cli_goto_line(cli, line - 1);
                        ide_cli_set_status(cli, "Line %d", line);
                    } else {
                        ide_cli_set_status(cli, "Invalid line number");
                    }
                    free(input);
                } else {
                    ide_cli_set_status(cli, "Cancelled");
                }
            }
            break;

        case DISPLAY_KEY_CTRL_O:
            {
                char* path = ide_cli_prompt(cli, "Open file: ");
                if (path && strlen(path) > 0) {
                    IDEBuffer* new_buf = ide_buffer_load(path);
                    if (new_buf) {
                        ide_buffer_free(cli->buffer);
                        cli->buffer = new_buf;
                        ide_tokenize_all(cli->buffer);
                        ide_update_blocks(cli->buffer);
                        ide_block_outline_update(&cli->block_outline, new_buf);
                        ide_template_info_update(&cli->template_info, new_buf);
                        ide_validate_all(new_buf, cli->diags);
                        ide_cli_set_status(cli, "Opened: %s", path);
                    } else {
                        ide_cli_set_status(cli, "Failed to open: %s", path);
                    }
                    free(path);
                } else {
                    ide_cli_set_status(cli, "Cancelled");
                }
            }
            break;

        case DISPLAY_KEY_CTRL_F:
            {
                char* pattern = ide_cli_prompt(cli, "Find: ");
                if (pattern && strlen(pattern) > 0) {
                    ide_cli_find(cli, pattern);
                    free(pattern);
                } else {
                    ide_cli_set_status(cli, "Cancelled");
                }
            }
            break;

        // --- F10: Toggle menu bar ---
        case DISPLAY_KEY_F10:
            if (cli->menubar.active_menu == MENU_NONE) {
                cli->menubar.active_menu = MENU_FILE;
                cli->menubar.active_item = 0;
                cli->focus = FOCUS_MENU;
                ide_cli_set_status(cli, "Menu (arrows to navigate, Enter to select, Esc to close)");
            } else {
                cli->menubar.active_menu = MENU_NONE;
                cli->focus = FOCUS_EDITOR;
                ide_cli_set_status(cli, "Editor");
            }
            cli->needs_redraw = true;
            break;

        // --- Alt+key: Open specific menu ---
        case DISPLAY_KEY_ALT_F:
            cli->menubar.active_menu = MENU_FILE;
            cli->menubar.active_item = 0;
            cli->focus = FOCUS_MENU;
            ide_cli_set_status(cli, "File menu");
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_ALT_E:
            cli->menubar.active_menu = MENU_EDIT;
            cli->menubar.active_item = 0;
            cli->focus = FOCUS_MENU;
            ide_cli_set_status(cli, "Edit menu");
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_ALT_V:
            cli->menubar.active_menu = MENU_VIEW;
            cli->menubar.active_item = 0;
            cli->focus = FOCUS_MENU;
            ide_cli_set_status(cli, "View menu");
            cli->needs_redraw = true;
            break;

        case DISPLAY_KEY_ALT_H:
            cli->menubar.active_menu = MENU_HELP;
            cli->menubar.active_item = 0;
            cli->focus = FOCUS_MENU;
            ide_cli_set_status(cli, "Help menu");
            cli->needs_redraw = true;
            break;

        // --- Legacy single-key quit (for compatibility) ---
        case DISPLAY_KEY_Q:
            // Only quit if not in editor focus (e.g., menu)
            if (cli->focus != FOCUS_EDITOR) {
                cli->running = false;
            }
            cli->needs_redraw = true;
            break;

        // --- Printable characters ---
        case DISPLAY_KEY_CHAR:
            if (cli->focus == FOCUS_EDITOR) {
                char c = display_get_last_char();
                if (c >= 32 && c < 127) {
                    ide_insert_char(cli->buffer, c);
                    cli->needs_redraw = true;
                }
            }
            break;

        default:
            // Unhandled key
            break;
    }
}

// Main event loop
void ide_cli_run(IDECLI* cli) {
    if (!cli) return;

    // Initial validation
    if (cli->buffer) {
        ide_validate_all(cli->buffer, cli->diags);
        if (cli->diags->count > 0) {
            ide_cli_set_status(cli, "%u issue(s) found. Ctrl+G to go to line.",
                              cli->diags->count);
        }
    }

    while (cli->running) {
        if (cli->needs_redraw) {
            ide_cli_draw(cli);
        }

        DisplayKey key = display_wait_key();
        ide_cli_handle_key(cli, key);
    }
}

// -----------------------------------------------------------------------------
// B.9 Commands
// -----------------------------------------------------------------------------

bool ide_cli_open(IDECLI* cli, const char* filepath) {
    if (!cli || !filepath) return false;

    IDEBuffer* new_buf = ide_buffer_load(filepath);
    if (!new_buf) return false;

    ide_buffer_free(cli->buffer);
    cli->buffer = new_buf;
    ide_block_outline_update(&cli->block_outline, new_buf);
    ide_template_info_update(&cli->template_info, new_buf);
    ide_validate_all(new_buf, cli->diags);

    return true;
}

bool ide_cli_save(IDECLI* cli) {
    if (!cli || !cli->buffer) return false;

    if (!cli->buffer->filepath) {
        char* path = ide_cli_prompt(cli, "Save as: ");
        if (!path) return false;
        bool result = ide_buffer_save_as(cli->buffer, path);
        free(path);
        return result;
    }

    return ide_buffer_save(cli->buffer);
}

bool ide_cli_save_as(IDECLI* cli, const char* filepath) {
    if (!cli || !cli->buffer || !filepath) return false;
    return ide_buffer_save_as(cli->buffer, filepath);
}

char* ide_cli_prompt(IDECLI* cli, const char* message) {
    if (!cli) return NULL;

    // Input buffer
    char input[256] = {0};
    int len = 0;
    int cursor = 0;

    // Calculate prompt position
    int prompt_row = cli->screen_rows - 1;
    int prompt_col = strlen(message) + 2;

    while (1) {
        // Draw prompt
        display_set_style(IDE_STYLE_STATUS);
        display_set_cursor(0, prompt_row);
        display_clear_to_eol();
        display_printf(" %s%s", message, input);
        display_reset_style();

        // Position cursor
        display_set_cursor(prompt_col + cursor, prompt_row);
        display_show_cursor();
        display_refresh();

        // Get key
        DisplayKey key = display_wait_key();

        switch (key) {
            case DISPLAY_KEY_ENTER:
                // Return copy of input
                cli->needs_redraw = true;
                if (len == 0) return NULL;
                return strdup(input);

            case DISPLAY_KEY_ESCAPE:
                // Cancel
                cli->needs_redraw = true;
                return NULL;

            case DISPLAY_KEY_BACKSPACE:
                if (cursor > 0) {
                    // Delete char before cursor
                    memmove(&input[cursor - 1], &input[cursor], len - cursor + 1);
                    cursor--;
                    len--;
                }
                break;

            case DISPLAY_KEY_DELETE:
                if (cursor < len) {
                    // Delete char at cursor
                    memmove(&input[cursor], &input[cursor + 1], len - cursor);
                    len--;
                }
                break;

            case DISPLAY_KEY_LEFT:
                if (cursor > 0) cursor--;
                break;

            case DISPLAY_KEY_RIGHT:
                if (cursor < len) cursor++;
                break;

            case DISPLAY_KEY_HOME:
                cursor = 0;
                break;

            case DISPLAY_KEY_END:
                cursor = len;
                break;

            case DISPLAY_KEY_CHAR:
                if (len < 255) {
                    char c = display_get_last_char();
                    if (c >= 32 && c < 127) {
                        // Insert char at cursor
                        memmove(&input[cursor + 1], &input[cursor], len - cursor + 1);
                        input[cursor] = c;
                        cursor++;
                        len++;
                    }
                }
                break;

            default:
                break;
        }
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Kingdom Technology excludes no one."
// The CLI base provides full editing across all display modes.
//
// TODO: Functions still needed in display layer:
//   - display_input_line()    - Read text input from user
//   - Extended DisplayKey values for editor keys (HOME, END, PGUP, PGDN, DEL, BACKSPACE, F1-F12)

// =============================================================================
// END CLOSING
// =============================================================================
