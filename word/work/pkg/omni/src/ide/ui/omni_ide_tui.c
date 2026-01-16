// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide-tui-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_ide_tui.c — OmniCode IDE Terminal UI Implementation
// ncurses-based interactive editor

#define _GNU_SOURCE

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-tui-impl
// Grounding: Proverbs 22:29 - "Seest thou a man diligent in his work?"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_ide_tui.h"
#include "ide/core/omni_screenshot.h"  // Session screenshots
#include <ncurses.h>
#include "display.h"  // Cornerstone display abstraction
#include "config.h"  // IDE config with themes (from libtrit)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/stat.h>
#include <signal.h>

// Signal flag for clean shutdown (defined in omni_ide_main.c)
extern volatile sig_atomic_t g_shutdown_requested;

// Control key macro (converts 'q' to Ctrl+Q code)
#define CTRL(c) ((c) & 037)

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Color Definitions
// -----------------------------------------------------------------------------

#define COLOR_NORMAL      1
#define COLOR_PRAGMA      2
#define COLOR_KEYWORD     3
#define COLOR_COMMENT     4
#define COLOR_STRING      5
#define COLOR_NUMBER      6
#define COLOR_BLOCK       7
#define COLOR_SECTION     8
#define COLOR_ERROR       9
#define COLOR_STATUS      10
#define COLOR_LINENO      11

// Block colors (use same as token colors where appropriate)
#define COLOR_METADATA    12
#define COLOR_SETUP       13
#define COLOR_BODY        14
#define COLOR_CLOSING     15
#define COLOR_HEADER      16
#define COLOR_CONTEXT     17
#define COLOR_CONTENT     18
#define COLOR_FOOTER      19

// Menu colors
#define COLOR_MENU_BAR     20
#define COLOR_MENU_ITEM    21
#define COLOR_MENU_SELECT  22
#define COLOR_MENU_HOTKEY  23

// Convert DisplayColor enum to ncurses color constant.
// DisplayColor values map to ncurses: DEFAULT=-1, BLACK=0, RED=1, etc.
static short display_to_ncurses_color(DisplayColor c) {
    switch (c) {
        case DISPLAY_COLOR_DEFAULT: return -1;
        case DISPLAY_COLOR_BLACK: return COLOR_BLACK;
        case DISPLAY_COLOR_RED: return COLOR_RED;
        case DISPLAY_COLOR_GREEN: return COLOR_GREEN;
        case DISPLAY_COLOR_YELLOW: return COLOR_YELLOW;
        case DISPLAY_COLOR_BLUE: return COLOR_BLUE;
        case DISPLAY_COLOR_MAGENTA: return COLOR_MAGENTA;
        case DISPLAY_COLOR_CYAN: return COLOR_CYAN;
        case DISPLAY_COLOR_WHITE: return COLOR_WHITE;
        case DISPLAY_COLOR_BRIGHT_BLACK: return 8;   // Gray
        case DISPLAY_COLOR_BRIGHT_RED: return 9;
        case DISPLAY_COLOR_BRIGHT_GREEN: return 10;
        case DISPLAY_COLOR_BRIGHT_YELLOW: return 11;
        case DISPLAY_COLOR_BRIGHT_BLUE: return 12;
        case DISPLAY_COLOR_BRIGHT_MAGENTA: return 13;
        case DISPLAY_COLOR_BRIGHT_CYAN: return 14;
        case DISPLAY_COLOR_BRIGHT_WHITE: return 15;
        default: return -1;
    }
}

// Convert string color name to DisplayColor enum.
// Used to bridge config.h string colors with display.h enum colors.
static DisplayColor string_to_display_color(const char* name) {
    if (!name || !name[0]) return DISPLAY_COLOR_DEFAULT;

    // Case-insensitive comparison for color names
    if (strcasecmp(name, "default") == 0) return DISPLAY_COLOR_DEFAULT;
    if (strcasecmp(name, "black") == 0) return DISPLAY_COLOR_BLACK;
    if (strcasecmp(name, "red") == 0) return DISPLAY_COLOR_RED;
    if (strcasecmp(name, "green") == 0) return DISPLAY_COLOR_GREEN;
    if (strcasecmp(name, "yellow") == 0) return DISPLAY_COLOR_YELLOW;
    if (strcasecmp(name, "blue") == 0) return DISPLAY_COLOR_BLUE;
    if (strcasecmp(name, "magenta") == 0) return DISPLAY_COLOR_MAGENTA;
    if (strcasecmp(name, "cyan") == 0) return DISPLAY_COLOR_CYAN;
    if (strcasecmp(name, "white") == 0) return DISPLAY_COLOR_WHITE;
    if (strcasecmp(name, "bright_black") == 0 || strcasecmp(name, "gray") == 0) return DISPLAY_COLOR_BRIGHT_BLACK;
    if (strcasecmp(name, "bright_red") == 0) return DISPLAY_COLOR_BRIGHT_RED;
    if (strcasecmp(name, "bright_green") == 0) return DISPLAY_COLOR_BRIGHT_GREEN;
    if (strcasecmp(name, "bright_yellow") == 0) return DISPLAY_COLOR_BRIGHT_YELLOW;
    if (strcasecmp(name, "bright_blue") == 0) return DISPLAY_COLOR_BRIGHT_BLUE;
    if (strcasecmp(name, "bright_magenta") == 0) return DISPLAY_COLOR_BRIGHT_MAGENTA;
    if (strcasecmp(name, "bright_cyan") == 0) return DISPLAY_COLOR_BRIGHT_CYAN;
    if (strcasecmp(name, "bright_white") == 0) return DISPLAY_COLOR_BRIGHT_WHITE;

    return DISPLAY_COLOR_DEFAULT;  // Fallback
}

// Convert string color name to ncurses color constant.
// Combines string_to_display_color with display_to_ncurses_color.
static short string_to_ncurses_color(const char* name) {
    return display_to_ncurses_color(string_to_display_color(name));
}

static void init_colors(void) {
    start_color();
    use_default_colors();

    // Check if we have config loaded
    const IDEDisplayConfig* cfg = display_get_ide_config();
    const DisplayTheme* theme = NULL;
    if (cfg && cfg->loaded) {
        theme = display_get_current_theme(cfg);
    }

    // Token colors (from config or defaults)
    init_pair(COLOR_NORMAL, -1, -1);
    if (theme) {
        init_pair(COLOR_PRAGMA, string_to_ncurses_color(theme->syntax.pragma), -1);
        init_pair(COLOR_KEYWORD, string_to_ncurses_color(theme->syntax.keyword), -1);
        init_pair(COLOR_COMMENT, string_to_ncurses_color(theme->syntax.comment), -1);
        init_pair(COLOR_STRING, string_to_ncurses_color(theme->syntax.string), -1);
        init_pair(COLOR_NUMBER, string_to_ncurses_color(theme->syntax.number), -1);
        init_pair(COLOR_BLOCK, string_to_ncurses_color(theme->syntax.block_marker), -1);
    } else {
        // Fallback to defaults
        init_pair(COLOR_PRAGMA, COLOR_MAGENTA, -1);
        init_pair(COLOR_KEYWORD, COLOR_YELLOW, -1);
        init_pair(COLOR_COMMENT, COLOR_GREEN, -1);
        init_pair(COLOR_STRING, COLOR_RED, -1);
        init_pair(COLOR_NUMBER, COLOR_CYAN, -1);
        init_pair(COLOR_BLOCK, COLOR_YELLOW, -1);
    }
    // Section, error, status, lineno colors
    if (theme) {
        init_pair(COLOR_SECTION, string_to_ncurses_color(theme->syntax.section_marker), -1);
        init_pair(COLOR_ERROR, string_to_ncurses_color(theme->syntax.error),
                  string_to_ncurses_color(theme->syntax.error_bg));
        init_pair(COLOR_STATUS, string_to_ncurses_color(theme->status.bar_fg),
                  string_to_ncurses_color(theme->status.bar_bg));
        init_pair(COLOR_LINENO, string_to_ncurses_color(theme->editor.line_number_fg), -1);
    } else {
        init_pair(COLOR_SECTION, COLOR_BLUE, -1);
        init_pair(COLOR_ERROR, COLOR_WHITE, COLOR_RED);
        init_pair(COLOR_STATUS, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOR_LINENO, COLOR_CYAN, -1);
    }

    // Block background colors
    if (theme) {
        init_pair(COLOR_METADATA, string_to_ncurses_color(theme->blocks.metadata), -1);
        init_pair(COLOR_SETUP, string_to_ncurses_color(theme->blocks.setup), -1);
        init_pair(COLOR_BODY, string_to_ncurses_color(theme->blocks.body), -1);
        init_pair(COLOR_CLOSING, string_to_ncurses_color(theme->blocks.closing), -1);
        init_pair(COLOR_HEADER, string_to_ncurses_color(theme->blocks.header), -1);
        init_pair(COLOR_CONTEXT, string_to_ncurses_color(theme->blocks.context), -1);
        init_pair(COLOR_CONTENT, string_to_ncurses_color(theme->blocks.content), -1);
        init_pair(COLOR_FOOTER, string_to_ncurses_color(theme->blocks.footer), -1);
    } else {
        init_pair(COLOR_METADATA, COLOR_CYAN, -1);
        init_pair(COLOR_SETUP, COLOR_BLUE, -1);
        init_pair(COLOR_BODY, COLOR_GREEN, -1);
        init_pair(COLOR_CLOSING, COLOR_RED, -1);
        init_pair(COLOR_HEADER, COLOR_MAGENTA, -1);
        init_pair(COLOR_CONTEXT, COLOR_YELLOW, -1);
        init_pair(COLOR_CONTENT, COLOR_GREEN, -1);
        init_pair(COLOR_FOOTER, COLOR_RED, -1);
    }

    // Menu colors
    if (theme) {
        init_pair(COLOR_MENU_BAR, string_to_ncurses_color(theme->menu.bar_fg),
                  string_to_ncurses_color(theme->menu.bar_bg));
        init_pair(COLOR_MENU_ITEM, string_to_ncurses_color(theme->menu.item_fg),
                  string_to_ncurses_color(theme->menu.bar_bg));
        init_pair(COLOR_MENU_SELECT, string_to_ncurses_color(theme->menu.selected_fg),
                  string_to_ncurses_color(theme->menu.selected_bg));
        init_pair(COLOR_MENU_HOTKEY, string_to_ncurses_color(theme->menu.hotkey_fg),
                  string_to_ncurses_color(theme->menu.bar_bg));
    } else {
        init_pair(COLOR_MENU_BAR, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOR_MENU_ITEM, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOR_MENU_SELECT, COLOR_WHITE, COLOR_BLUE);
        init_pair(COLOR_MENU_HOTKEY, COLOR_RED, COLOR_WHITE);
    }
}

// Legacy color functions - kept for reference, theme system now used
static int __attribute__((unused)) block_color(IDEBlockType b) {
    switch (b) {
        case BLOCK_3_METADATA:
        case BLOCK_4_METADATA:
        case BLOCK_5_METADATA: return COLOR_METADATA;
        case BLOCK_4_SETUP: return COLOR_SETUP;
        case BLOCK_5_HEADER: return COLOR_HEADER;
        case BLOCK_5_CONTEXT: return COLOR_CONTEXT;
        case BLOCK_4_BODY:
        case BLOCK_3_CONTENT:
        case BLOCK_5_CONTENT: return COLOR_CONTENT;
        case BLOCK_3_CLOSING:
        case BLOCK_4_CLOSING:
        case BLOCK_5_FOOTER: return COLOR_CLOSING;
        default: return COLOR_NORMAL;
    }
}

static int __attribute__((unused)) token_color(IDETokenType t) {
    switch (t) {
        case TOKEN_PRAGMA: return COLOR_PRAGMA;
        case TOKEN_KEYWORD: return COLOR_KEYWORD;
        case TOKEN_COMMENT: return COLOR_COMMENT;
        case TOKEN_STRING: return COLOR_STRING;
        case TOKEN_NUMBER: return COLOR_NUMBER;
        case TOKEN_BLOCK_MARKER: return COLOR_BLOCK;
        case TOKEN_SECTION: return COLOR_SECTION;
        case TOKEN_ERROR: return COLOR_ERROR;
        default: return COLOR_NORMAL;
    }
}

// Get theme category string for token type (for display layer styling)
static const char* token_theme_category(IDETokenType t) {
    switch (t) {
        case TOKEN_PRAGMA: return "syntax.pragma";
        case TOKEN_KEYWORD: return "syntax.keyword";
        case TOKEN_COMMENT: return "syntax.comment";
        case TOKEN_STRING: return "syntax.string";
        case TOKEN_NUMBER: return "syntax.number";
        case TOKEN_BLOCK_MARKER: return "syntax.marker";
        case TOKEN_SECTION: return "syntax.section";
        case TOKEN_ERROR: return "syntax.error";
        default: return NULL;  // No style change needed
    }
}

// Get theme category string for block type (for display layer styling)
static const char* block_theme_category(IDEBlockType b) {
    switch (b) {
        case BLOCK_3_METADATA:
        case BLOCK_4_METADATA:
        case BLOCK_5_METADATA: return "blocks.metadata";
        case BLOCK_4_SETUP: return "blocks.setup";
        case BLOCK_5_HEADER: return "blocks.header";
        case BLOCK_5_CONTEXT: return "blocks.context";
        case BLOCK_4_BODY: return "blocks.body";
        case BLOCK_3_CONTENT:
        case BLOCK_5_CONTENT: return "blocks.content";
        case BLOCK_3_CLOSING:
        case BLOCK_4_CLOSING: return "blocks.closing";
        case BLOCK_5_FOOTER: return "blocks.footer";
        default: return NULL;
    }
}

// -----------------------------------------------------------------------------
// B.2 Layout Calculation
// -----------------------------------------------------------------------------

// Minimum sizes
#define MIN_PANEL_WIDTH   20
#define MAX_PANEL_WIDTH   40
#define MIN_EDITOR_WIDTH  40
#define PANEL_WIDTH_PCT   25   // 25% of screen for left panels
#define BOTTOM_PANEL_PCT  25   // 25% of height for bottom panel

// Recalculate all dimensions based on current terminal size
static void ide_tui_recalc_layout(IDETUI* tui) {
    if (!tui) return;

    // Get current terminal size via display abstraction
    int32_t cols, rows;
    display_get_size(&cols, &rows);
    tui->screen_cols = cols;
    tui->screen_rows = rows;

    // Calculate proportional panel width
    int panel_width = 0;
    if (tui->panels_visible && tui->layout) {
        panel_width = (tui->screen_cols * PANEL_WIDTH_PCT) / 100;

        // Clamp to min/max
        if (panel_width < MIN_PANEL_WIDTH) panel_width = MIN_PANEL_WIDTH;
        if (panel_width > MAX_PANEL_WIDTH) panel_width = MAX_PANEL_WIDTH;

        // But ensure editor has enough space
        if (tui->screen_cols - panel_width < MIN_EDITOR_WIDTH) {
            panel_width = tui->screen_cols - MIN_EDITOR_WIDTH;
            if (panel_width < 0) panel_width = 0;
        }

        tui->layout->left_width = panel_width;
    }

    // Calculate bottom panel height (for validation/diagnostics)
    int bottom_height = 0;
    if (tui->panels_visible && tui->diags && tui->diags->count > 0) {
        bottom_height = (tui->screen_rows * BOTTOM_PANEL_PCT) / 100;
        if (bottom_height < 4) bottom_height = 4;   // Minimum for useful display
        if (bottom_height > 10) bottom_height = 10; // Cap it
        tui->layout->bottom_height = bottom_height;
    } else {
        if (tui->layout) tui->layout->bottom_height = 0;
    }

    // Status bar takes 2 rows, menu bar takes 1 row
    int status_rows = tui->config.show_status_bar ? 2 : 0;
    int menu_rows = tui->menubar.visible ? 1 : 0;

    // Calculate editing area
    tui->edit_rows = tui->screen_rows - status_rows - bottom_height - menu_rows;
    tui->edit_cols = tui->screen_cols - panel_width - (tui->config.show_line_numbers ? 6 : 0);

    // Ensure minimums
    if (tui->edit_rows < 1) tui->edit_rows = 1;
    if (tui->edit_cols < 10) tui->edit_cols = 10;

    // Update buffer viewport
    if (tui->buffer) {
        tui->buffer->visible_lines = tui->edit_rows;
        tui->buffer->visible_cols = tui->edit_cols;
    }
}

// -----------------------------------------------------------------------------
// B.3 Menu System
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
static void ide_tui_draw_menubar(IDETUI* tui) {
    if (!tui || !tui->menubar.visible) return;

    // Draw menu bar background
    display_set_theme_style("menu.bar");
    display_set_cursor(0, 0);  // x, y (display layer order)
    for (int i = 0; i < tui->screen_cols; i++) display_putchar(' ');
    display_reset_style();

    // Draw menu titles
    int x = 1;
    for (int m = 0; m < MENU_COUNT; m++) {
        IDEMenu* menu = &tui->menubar.menus[m];
        display_set_cursor(x, 0);  // x, y

        if (m == tui->menubar.active_menu) {
            display_set_theme_style("menu.selected");
        } else {
            display_set_theme_style("menu.bar");
        }

        display_printf(" %s ", menu->label);
        display_reset_style();

        x += strlen(menu->label) + 2;
    }

    // Draw F10 hint on right
    display_set_theme_style("menu.bar");
    display_printf_at(tui->screen_cols - 12, 0, " F10: Menu ");
    display_reset_style();
}

// Draw dropdown menu
static void ide_tui_draw_menu_dropdown(IDETUI* tui) {
    if (!tui || tui->menubar.active_menu == MENU_NONE) return;

    IDEMenu* menu = &tui->menubar.menus[tui->menubar.active_menu];

    // Calculate menu position
    int x = 1;
    for (int m = 0; m < tui->menubar.active_menu; m++) {
        x += strlen(tui->menubar.menus[m].label) + 2;
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

    // Draw menu items
    for (int i = 0; i < menu->item_count; i++) {
        display_set_cursor(x, i + 1);  // x, y

        if (i == tui->menubar.active_item) {
            display_set_theme_style("menu.selected");
        } else {
            display_set_theme_style("menu.item");
        }

        // Item label
        display_printf(" %-*s", max_width - 2, menu->items[i].label);

        // Shortcut
        if (menu->items[i].shortcut) {
            int label_len = strlen(menu->items[i].label);
            int shortcut_len = strlen(menu->items[i].shortcut);
            int shortcut_x = x + max_width - shortcut_len - 1;
            if (shortcut_x > x + label_len + 2) {
                display_set_cursor(shortcut_x, i + 1);  // x, y
                display_printf("%s", menu->items[i].shortcut);
            }
        }

        display_print(" ");
        display_reset_style();

        // Draw separator if needed
        if (menu->items[i].separator && i + 1 < menu->item_count) {
            display_set_cursor(x + max_width, i + 1);  // x, y
            display_set_theme_style("menu.item");
            for (int j = 0; j < max_width; j++) {
                display_putchar_at(x + j, i + 2, '-');
            }
            display_reset_style();
        }
    }
}

// Execute menu action
static void ide_menu_execute(IDETUI* tui, IDEMenuAction action) {
    switch (action) {
        case ACTION_NEW:
            // TODO: New file
            ide_tui_set_status(tui, "New file (not implemented yet)");
            break;
        case ACTION_OPEN:
            {
                char* path = ide_tui_prompt(tui, "Open file: ");
                if (path && strlen(path) > 0) {
                    IDEBuffer* new_buf = ide_buffer_load(path);
                    if (new_buf) {
                        ide_buffer_free(tui->buffer);
                        tui->buffer = new_buf;
                        ide_block_outline_update(&tui->block_outline, new_buf);
                        ide_template_info_update(&tui->template_info, new_buf);
                        ide_validate_all(new_buf, tui->diags);
                        ide_tui_set_status(tui, "Opened: %s", path);
                    } else {
                        ide_tui_set_status(tui, "Failed to open: %s", path);
                    }
                    free(path);
                }
            }
            break;
        case ACTION_SAVE:
            if (ide_tui_save(tui)) {
                ide_tui_set_status(tui, "Saved: %s", tui->buffer->filepath);
            } else {
                ide_tui_set_status(tui, "Save failed!");
            }
            break;
        case ACTION_SAVE_AS:
            {
                char* path = ide_tui_prompt(tui, "Save as: ");
                if (path && strlen(path) > 0) {
                    if (ide_buffer_save_as(tui->buffer, path)) {
                        ide_tui_set_status(tui, "Saved: %s", path);
                    } else {
                        ide_tui_set_status(tui, "Save failed!");
                    }
                    free(path);
                }
            }
            break;
        case ACTION_QUIT:
            if (tui->buffer->modified) {
                ide_tui_set_status(tui, "Unsaved changes! Press Ctrl+Q to quit.");
            } else {
                tui->running = false;
            }
            break;
        case ACTION_TOGGLE_PANELS:
            tui->panels_visible = !tui->panels_visible;
            if (!tui->panels_visible) tui->focus = FOCUS_EDITOR;
            ide_tui_recalc_layout(tui);
            ide_tui_set_status(tui, "Panels %s", tui->panels_visible ? "visible" : "hidden");
            break;
        case ACTION_TOGGLE_LINENO:
            tui->config.show_line_numbers = !tui->config.show_line_numbers;
            ide_tui_recalc_layout(tui);
            ide_tui_set_status(tui, "Line numbers %s", tui->config.show_line_numbers ? "on" : "off");
            break;
        case ACTION_TOGGLE_COLORS:
            tui->config.show_block_colors = !tui->config.show_block_colors;
            ide_tui_set_status(tui, "Block colors %s", tui->config.show_block_colors ? "on" : "off");
            break;
        case ACTION_GOTO:
            {
                char* input = ide_tui_prompt(tui, "Go to line: ");
                if (input) {
                    int line = atoi(input);
                    if (line > 0) {
                        ide_tui_goto_line(tui, line - 1);
                    }
                    free(input);
                }
            }
            break;
        case ACTION_KEYS:
            ide_tui_set_status(tui,
                "F10:Menu Tab:Panels Ctrl+S:Save Ctrl+Q:Quit Ctrl+G:Goto Ctrl+B:Toggle");
            break;
        case ACTION_ABOUT:
            ide_tui_set_status(tui,
                "OmniCode IDE v0.1 - Kingdom Technology | \"In the beginning God created...\"");
            break;
        default:
            ide_tui_set_status(tui, "Action not implemented");
            break;
    }

    // Close menu after action
    tui->menubar.active_menu = MENU_NONE;
    tui->focus = FOCUS_EDITOR;
}

// Handle menu-specific keys
static bool ide_menu_handle_key(IDETUI* tui, DisplayKey key) {
    if (tui->focus != FOCUS_MENU) return false;

    IDEMenuBar* mb = &tui->menubar;

    switch (key) {
        case DISPLAY_KEY_LEFT:
            if (mb->active_menu == MENU_NONE) {
                mb->active_menu = MENU_FILE;
            } else if (mb->active_menu > 0) {
                mb->active_menu--;
            } else {
                mb->active_menu = MENU_COUNT - 1;
            }
            mb->active_item = 0;
            return true;

        case DISPLAY_KEY_RIGHT:
            if (mb->active_menu == MENU_NONE) {
                mb->active_menu = MENU_FILE;
            } else if (mb->active_menu < MENU_COUNT - 1) {
                mb->active_menu++;
            } else {
                mb->active_menu = MENU_FILE;
            }
            mb->active_item = 0;
            return true;

        case DISPLAY_KEY_UP:
            if (mb->active_menu != MENU_NONE) {
                IDEMenu* menu = &mb->menus[mb->active_menu];
                if (mb->active_item > 0) {
                    mb->active_item--;
                } else {
                    mb->active_item = menu->item_count - 1;
                }
            }
            return true;

        case DISPLAY_KEY_DOWN:
            if (mb->active_menu != MENU_NONE) {
                IDEMenu* menu = &mb->menus[mb->active_menu];
                if (mb->active_item < menu->item_count - 1) {
                    mb->active_item++;
                } else {
                    mb->active_item = 0;
                }
            }
            return true;

        case DISPLAY_KEY_ENTER:
            if (mb->active_menu != MENU_NONE) {
                IDEMenu* menu = &mb->menus[mb->active_menu];
                IDEMenuAction action = menu->items[mb->active_item].action;
                ide_menu_execute(tui, action);
            }
            return true;

        case DISPLAY_KEY_ESCAPE:
            mb->active_menu = MENU_NONE;
            tui->focus = FOCUS_EDITOR;
            ide_tui_set_status(tui, "Editor");
            return true;

        case DISPLAY_KEY_CHAR:
            // Check for hotkeys (letter typed)
            {
                char c = display_get_last_char();
                for (int m = 0; m < MENU_COUNT; m++) {
                    if (toupper(c) == tui->menubar.menus[m].hotkey) {
                        mb->active_menu = m;
                        mb->active_item = 0;
                        return true;
                    }
                }
            }
            break;

        default:
            break;
    }

    return false;
}

// -----------------------------------------------------------------------------
// B.4 TUI Lifecycle
// -----------------------------------------------------------------------------

IDETUI* ide_tui_create(IDEBuffer* buffer) {
    IDETUI* tui = malloc(sizeof(IDETUI));
    if (!tui) return NULL;

    memset(tui, 0, sizeof(IDETUI));
    tui->buffer = buffer;
    tui->running = true;
    tui->needs_redraw = true;

    // Default config
    tui->config.show_line_numbers = true;
    tui->config.show_block_colors = true;
    tui->config.show_status_bar = true;
    tui->config.syntax_highlight = true;
    tui->config.tab_width = 4;
    tui->config.soft_wrap = false;

    // Initialize display layer (handles ncurses init, terminal modes, etc.)
    DisplayConfig display_cfg = {
        .mode = DISPLAY_TUI,
        .width = 0,   // Auto-detect from terminal
        .height = 0,  // Auto-detect from terminal
        .title = "OmniCode IDE"
    };
    display_init(&display_cfg);
    init_colors();  // IDE-specific color pairs (builds on display layer's colors)

    // Create diagnostics first (needed for layout calculation)
    tui->diags = ide_diagnostics_create();
    tui->validation.diags = tui->diags;

    // Menu bar
    ide_menubar_init(&tui->menubar);

    // Panel system
    tui->layout = ide_layout_create_default();
    tui->panels_visible = true;
    tui->focus = FOCUS_EDITOR;

    // Calculate layout (proportional, auto-sizing)
    ide_tui_recalc_layout(tui);

    // Initialize file browser with Bereshit root (derived from buffer path)
    if (buffer && buffer->filepath) {
        // Try to find Bereshit root by looking for void/word/tov dirs
        char root[512];
        strncpy(root, buffer->filepath, sizeof(root) - 1);
        char* last_slash = strrchr(root, '/');
        if (last_slash) *last_slash = '\0';  // Get directory

        // Walk up looking for void/word/tov
        while (strlen(root) > 1) {
            char check[600];
            snprintf(check, sizeof(check), "%s/void", root);
            struct stat st;
            if (stat(check, &st) == 0 && S_ISDIR(st.st_mode)) {
                ide_file_browser_init(&tui->file_browser, root);
                break;
            }
            last_slash = strrchr(root, '/');
            if (last_slash) *last_slash = '\0';
            else break;
        }
    }

    // Initialize block outline from buffer
    if (buffer) {
        ide_block_outline_update(&tui->block_outline, buffer);
        ide_template_info_update(&tui->template_info, buffer);
    }

    ide_tui_set_status(tui, "OmniCode IDE | Tab: Panels | Ctrl+B: Toggle | Ctrl+S: Save | Ctrl+Q: Quit");

    return tui;
}

void ide_tui_destroy(IDETUI* tui) {
    if (!tui) return;

    // Capture screenshot of last known state before shutdown
    screenshot_tui("shutdown");

    display_shutdown();
    ide_layout_free(tui->layout);
    ide_diagnostics_free(tui->diags);
    free(tui);
}

// -----------------------------------------------------------------------------
// B.3 Display
// -----------------------------------------------------------------------------

void ide_tui_draw_line(IDETUI* tui, uint32_t line_num) {
    if (!tui || !tui->buffer) return;

    IDEBuffer* buf = tui->buffer;
    int row = line_num - buf->scroll_line;
    if (row < 0 || row >= tui->edit_rows) return;

    display_set_cursor(0, row);  // x, y
    display_clear_to_eol();

    // Line number
    if (tui->config.show_line_numbers) {
        display_set_theme_style("editor.lineno");
        display_printf("%5u ", line_num + 1);
        display_reset_style();
    }

    if (line_num >= buf->line_count) return;

    IDELine* line = &buf->lines[line_num];
    int col_offset = tui->config.show_line_numbers ? 6 : 0;
    (void)col_offset;  // Reserved for horizontal scroll

    // Block color indicator
    const char* block_cat = NULL;
    if (tui->config.show_block_colors && line->block != BLOCK_NONE) {
        block_cat = block_theme_category(line->block);
        if (block_cat) display_set_theme_style(block_cat);
    }

    // Draw text with syntax highlighting
    if (tui->config.syntax_highlight && line->token_count > 0) {
        uint32_t pos = 0;
        for (uint32_t t = 0; t < line->token_count; t++) {
            IDEToken* tok = &line->tokens[t];

            // Print before token
            while (pos < tok->start && pos < line->length) {
                display_putchar(line->text[pos++]);
            }

            // Print token with color
            const char* tok_cat = token_theme_category(tok->type);
            if (tok_cat) display_set_theme_style(tok_cat);
            while (pos < tok->end && pos < line->length) {
                display_putchar(line->text[pos++]);
            }
            if (tok_cat) display_reset_style();

            // Restore block color if active
            if (block_cat) display_set_theme_style(block_cat);
        }

        // Print remainder
        while (pos < line->length) {
            display_putchar(line->text[pos++]);
        }
    } else {
        // No syntax highlighting - just print
        display_printf("%s", line->text);
    }

    if (block_cat) {
        display_reset_style();
    }
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

void ide_tui_draw_status(IDETUI* tui) {
    if (!tui || !tui->config.show_status_bar) return;

    IDEBuffer* buf = tui->buffer;
    int row = tui->screen_rows - 1;

    // Status bar background
    display_set_theme_style("status.line");
    display_set_cursor(0, row - 1);  // x, y
    for (int i = 0; i < tui->screen_cols; i++) display_putchar(' ');
    display_set_cursor(0, row);  // x, y
    for (int i = 0; i < tui->screen_cols; i++) display_putchar(' ');

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
    int diag_count = tui->diags ? tui->diags->count : 0;

    // Left side: file info (use modified style if file is modified)
    if (buf && buf->modified) {
        display_set_theme_style("status.modified");
    }
    display_printf(" %s%s",
           buf && buf->filepath ? buf->filepath : "[New File]",
           buf && buf->modified ? " [+]" : "");
    if (buf && buf->modified) {
        display_set_theme_style("status.line");  // Restore status style
    }

    // Right side: position, block, structure, diagnostics
    if (buf) {
        char info[128];
        if (diag_count > 0) {
            snprintf(info, sizeof(info), "%s | %s | Ln %u Col %u | ⚠ %d",
                     structure, block_name[0] ? block_name : "-",
                     buf->cursor.line + 1, buf->cursor.col + 1, diag_count);
        } else {
            snprintf(info, sizeof(info), "%s | %s | Ln %u Col %u | ✓",
                     structure, block_name[0] ? block_name : "-",
                     buf->cursor.line + 1, buf->cursor.col + 1);
        }
        int info_col = tui->screen_cols - strlen(info) - 1;
        if (info_col > 0) {
            display_printf_at(info_col, row - 1, "%s", info);
        }
    }

    // Status message line (bottom status bar)
    display_set_cursor(0, row);  // x, y
    display_printf(" %s", tui->status_message);

    display_reset_style();
}

void ide_tui_draw(IDETUI* tui) {
    if (!tui) return;

    display_clear();

    // Draw menu bar first (takes row 0)
    ide_tui_draw_menubar(tui);

    int panel_width = tui->panels_visible ? tui->layout->left_width : 0;
    int bottom_height = tui->layout ? tui->layout->bottom_height : 0;
    int editor_x = panel_width;
    int menu_offset = tui->menubar.visible ? 1 : 0;  // Offset for menu bar

    // Draw left panels if visible (offset by menu bar)
    if (tui->panels_visible && tui->layout && panel_width > 0) {
        // Split left panels: 60% file browser, 40% block outline
        int file_browser_h = (tui->edit_rows * 60) / 100;
        int block_outline_h = tui->edit_rows - file_browser_h;

        // File browser (top-left)
        ide_panel_draw_file_browser(&tui->file_browser, 0, menu_offset,
                                    panel_width, file_browser_h,
                                    tui->focus == FOCUS_FILE_BROWSER);

        // Block outline (bottom-left)
        ide_panel_draw_block_outline(&tui->block_outline, 0, menu_offset + file_browser_h,
                                     panel_width, block_outline_h,
                                     tui->focus == FOCUS_BLOCK_OUTLINE);
    }

    // Draw editor lines (offset by panel width and menu bar)
    for (int i = 0; i < tui->edit_rows; i++) {
        uint32_t line_num = tui->buffer->scroll_line + i;
        int row = i + menu_offset;

        display_set_cursor(editor_x, row);  // x, y
        display_clear_to_eol();

        // Line number
        if (tui->config.show_line_numbers) {
            display_set_theme_style("editor.line_number");
            display_printf("%5u ", line_num + 1);
            display_reset_style();
        }

        if (line_num < tui->buffer->line_count) {
            IDELine* line = &tui->buffer->lines[line_num];

            // Block color indicator
            if (tui->config.show_block_colors && line->block != BLOCK_NONE) {
                display_set_theme_style(block_theme_category(line->block));
            }

            // Draw text with syntax highlighting
            if (tui->config.syntax_highlight && line->token_count > 0) {
                uint32_t pos = 0;
                for (uint32_t t = 0; t < line->token_count; t++) {
                    IDEToken* tok = &line->tokens[t];

                    // Print before token
                    while (pos < tok->start && pos < line->length) {
                        display_putchar(line->text[pos++]);
                    }

                    // Print token with color
                    display_set_theme_style(token_theme_category(tok->type));
                    while (pos < tok->end && pos < line->length) {
                        display_putchar(line->text[pos++]);
                    }
                    display_reset_style();
                }

                // Print remainder
                while (pos < line->length) {
                    display_putchar(line->text[pos++]);
                }
            } else {
                // No syntax highlighting
                display_printf("%s", line->text);
            }

            if (tui->config.show_block_colors && line->block != BLOCK_NONE) {
                display_reset_style();
            }
        }
    }

    // Draw bottom panel (validation/diagnostics) if there are issues
    if (tui->panels_visible && bottom_height > 0 && tui->diags && tui->diags->count > 0) {
        int bottom_y = tui->edit_rows + menu_offset;
        int bottom_w = tui->screen_cols;
        ide_panel_draw_validation(&tui->validation, 0, bottom_y, bottom_w, bottom_height);
    }

    // Draw status
    ide_tui_draw_status(tui);

    // Draw menu dropdown (on top of everything if menu active)
    if (tui->focus == FOCUS_MENU && tui->menubar.active_menu != MENU_NONE) {
        ide_tui_draw_menu_dropdown(tui);
    }

    // Position cursor (only if editor has focus)
    if (tui->buffer && tui->focus == FOCUS_EDITOR) {
        IDEBuffer* buf = tui->buffer;
        int row = buf->cursor.line - buf->scroll_line + menu_offset;
        int col = buf->cursor.col + editor_x + (tui->config.show_line_numbers ? 6 : 0);
        display_set_cursor(col, row);  // x, y
        display_show_cursor();
    } else {
        display_hide_cursor();  // Hide cursor when panels/menu focused
    }

    display_refresh();
    tui->needs_redraw = false;
}

void ide_tui_set_status(IDETUI* tui, const char* format, ...) {
    if (!tui) return;

    va_list args;
    va_start(args, format);
    vsnprintf(tui->status_message, sizeof(tui->status_message), format, args);
    va_end(args);
}

// -----------------------------------------------------------------------------
// B.4 Navigation
// -----------------------------------------------------------------------------

void ide_tui_ensure_visible(IDETUI* tui) {
    if (!tui || !tui->buffer) return;

    IDEBuffer* buf = tui->buffer;

    // Scroll up if cursor above viewport
    if (buf->cursor.line < buf->scroll_line) {
        buf->scroll_line = buf->cursor.line;
        tui->needs_redraw = true;
    }

    // Scroll down if cursor below viewport
    if (buf->cursor.line >= buf->scroll_line + tui->edit_rows) {
        buf->scroll_line = buf->cursor.line - tui->edit_rows + 1;
        tui->needs_redraw = true;
    }
}

void ide_tui_goto_line(IDETUI* tui, uint32_t line) {
    if (!tui || !tui->buffer) return;
    ide_cursor_goto(tui->buffer, line, 0);
    ide_tui_ensure_visible(tui);
    tui->needs_redraw = true;
}

// -----------------------------------------------------------------------------
// B.5 Input Handling
// -----------------------------------------------------------------------------

// Handle panel-specific keys
static void handle_panel_key(IDETUI* tui, DisplayKey key) {
    switch (tui->focus) {
        case FOCUS_FILE_BROWSER:
            switch (key) {
                case DISPLAY_KEY_UP:
                    ide_file_browser_up(&tui->file_browser);
                    break;
                case DISPLAY_KEY_DOWN:
                    ide_file_browser_down(&tui->file_browser);
                    break;
                case DISPLAY_KEY_RIGHT:
                case DISPLAY_KEY_ENTER:
                    // Open file or expand directory
                    {
                        const char* path = ide_file_browser_get_selected_path(&tui->file_browser);
                        if (path) {
                            // Open file in editor
                            IDEBuffer* new_buf = ide_buffer_load(path);
                            if (new_buf) {
                                ide_buffer_free(tui->buffer);
                                tui->buffer = new_buf;
                                ide_block_outline_update(&tui->block_outline, new_buf);
                                ide_template_info_update(&tui->template_info, new_buf);
                                ide_validate_all(new_buf, tui->diags);
                                tui->focus = FOCUS_EDITOR;
                                ide_tui_set_status(tui, "Opened: %s", path);
                            } else {
                                ide_tui_set_status(tui, "Failed to open: %s", path);
                            }
                        } else {
                            // Toggle directory expansion
                            ide_file_browser_toggle(&tui->file_browser);
                        }
                    }
                    break;
                case DISPLAY_KEY_LEFT:
                    // Collapse or go up
                    break;
                default:
                    break;
            }
            break;

        case FOCUS_BLOCK_OUTLINE:
            switch (key) {
                case DISPLAY_KEY_UP:
                    ide_block_outline_up(&tui->block_outline);
                    break;
                case DISPLAY_KEY_DOWN:
                    ide_block_outline_down(&tui->block_outline);
                    break;
                case DISPLAY_KEY_ENTER:
                    // Jump to block in editor
                    {
                        uint32_t line = ide_block_outline_get_line(&tui->block_outline);
                        ide_tui_goto_line(tui, line);
                        tui->focus = FOCUS_EDITOR;
                        ide_tui_set_status(tui, "Jumped to line %u", line + 1);
                    }
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void ide_tui_handle_key(IDETUI* tui, DisplayKey key) {
    if (!tui || !tui->buffer) return;

    IDEBuffer* buf = tui->buffer;

    // Handle menu keys first if menu is focused
    if (tui->focus == FOCUS_MENU) {
        if (ide_menu_handle_key(tui, key)) {
            tui->needs_redraw = true;
            return;
        }
    }

    // Global keys (work in any focus mode)
    switch (key) {
        // F10 - Open menu
        case DISPLAY_KEY_F10:
            tui->focus = FOCUS_MENU;
            tui->menubar.active_menu = MENU_FILE;
            tui->menubar.active_item = 0;
            ide_tui_set_status(tui, "Menu | Arrows: Navigate | Enter: Select | Esc: Close");
            tui->needs_redraw = true;
            return;

        // F1 - Quick help
        case DISPLAY_KEY_F1:
            ide_tui_set_status(tui,
                "F10:Menu Tab:Panels Ctrl+S:Save Ctrl+Q:Quit Ctrl+G:Goto Ctrl+B:Toggle");
            tui->needs_redraw = true;
            return;

        // Terminal/window resize
        case DISPLAY_KEY_RESIZE:
            ide_tui_recalc_layout(tui);
            ide_tui_set_status(tui, "Resized: %dx%d", tui->screen_cols, tui->screen_rows);
            tui->needs_redraw = true;
            return;

        // Quit
        case DISPLAY_KEY_CTRL_Q:
            if (buf->modified) {
                ide_tui_set_status(tui, "Unsaved changes! Press Ctrl+Q again to quit.");
                static int quit_count = 0;
                if (++quit_count > 1) {
                    tui->running = false;
                }
            } else {
                tui->running = false;
            }
            tui->needs_redraw = true;
            return;

        // Save
        case DISPLAY_KEY_CTRL_S:
            if (ide_tui_save(tui)) {
                ide_tui_set_status(tui, "Saved: %s", buf->filepath);
            } else {
                ide_tui_set_status(tui, "Save failed!");
            }
            tui->needs_redraw = true;
            return;

        // Toggle panels
        case DISPLAY_KEY_CTRL_B:
            tui->panels_visible = !tui->panels_visible;
            if (!tui->panels_visible) {
                tui->focus = FOCUS_EDITOR;
            }
            // Recalculate entire layout
            ide_tui_recalc_layout(tui);
            ide_tui_set_status(tui, "Panels %s", tui->panels_visible ? "visible" : "hidden");
            tui->needs_redraw = true;
            return;

        // Cycle focus with Tab
        case DISPLAY_KEY_TAB:
            if (tui->panels_visible) {
                switch (tui->focus) {
                    case FOCUS_EDITOR:
                        tui->focus = FOCUS_FILE_BROWSER;
                        ide_tui_set_status(tui, "File Browser | Enter: Open | Tab: Next");
                        break;
                    case FOCUS_FILE_BROWSER:
                        tui->focus = FOCUS_BLOCK_OUTLINE;
                        ide_tui_set_status(tui, "Block Outline | Enter: Jump | Tab: Next");
                        break;
                    case FOCUS_BLOCK_OUTLINE:
                        tui->focus = FOCUS_EDITOR;
                        ide_tui_set_status(tui, "Editor | Ctrl+B: Toggle Panels");
                        break;
                    case FOCUS_MENU:
                        // Close menu and go to editor
                        tui->menubar.active_menu = MENU_NONE;
                        tui->focus = FOCUS_EDITOR;
                        ide_tui_set_status(tui, "Editor");
                        break;
                }
                tui->needs_redraw = true;
                return;
            }
            break;

        // Escape returns to editor (closes menu if open)
        case DISPLAY_KEY_ESCAPE:
            tui->menubar.active_menu = MENU_NONE;
            tui->focus = FOCUS_EDITOR;
            ide_tui_set_status(tui, "Editor");
            tui->needs_redraw = true;
            return;

        // Go to line
        case DISPLAY_KEY_CTRL_G:
            {
                char* input = ide_tui_prompt(tui, "Go to line: ");
                if (input) {
                    int line = atoi(input);
                    if (line > 0) {
                        ide_tui_goto_line(tui, line - 1);
                    }
                    free(input);
                }
            }
            tui->needs_redraw = true;
            return;

        default:
            break;
    }

    // Panel-specific keys (menu handled at top of function)
    if (tui->focus != FOCUS_EDITOR && tui->focus != FOCUS_MENU) {
        handle_panel_key(tui, key);
        tui->needs_redraw = true;
        return;
    }

    // Editor keys (only when editor focused)
    switch (key) {
        // Cursor movement
        case DISPLAY_KEY_UP:
            ide_cursor_up(buf);
            ide_tui_ensure_visible(tui);
            break;

        case DISPLAY_KEY_DOWN:
            ide_cursor_down(buf);
            ide_tui_ensure_visible(tui);
            break;

        case DISPLAY_KEY_LEFT:
            ide_cursor_left(buf);
            break;

        case DISPLAY_KEY_RIGHT:
            ide_cursor_right(buf);
            break;

        case DISPLAY_KEY_HOME:
            ide_cursor_home(buf);
            break;

        case DISPLAY_KEY_END:
            ide_cursor_end(buf);
            break;

        case DISPLAY_KEY_PAGEUP:
            ide_cursor_page_up(buf);
            ide_tui_ensure_visible(tui);
            break;

        case DISPLAY_KEY_PAGEDOWN:
            ide_cursor_page_down(buf);
            ide_tui_ensure_visible(tui);
            break;

        // Editing
        case DISPLAY_KEY_BACKSPACE:
            ide_backspace(buf);
            ide_tui_ensure_visible(tui);
            break;

        case DISPLAY_KEY_DELETE:
            ide_delete(buf);
            break;

        case DISPLAY_KEY_ENTER:
            ide_newline(buf);
            ide_tui_ensure_visible(tui);
            break;

        // Regular character
        case DISPLAY_KEY_CHAR:
            {
                char c = display_get_last_char();
                if (c >= 32 && c < 127) {
                    ide_insert_char(buf, c);
                }
            }
            break;

        default:
            break;
    }

    tui->needs_redraw = true;
}

// -----------------------------------------------------------------------------
// B.6 Commands
// -----------------------------------------------------------------------------

bool ide_tui_save(IDETUI* tui) {
    if (!tui || !tui->buffer) return false;

    if (!tui->buffer->filepath) {
        char* path = ide_tui_prompt(tui, "Save as: ");
        if (!path) return false;
        bool result = ide_buffer_save_as(tui->buffer, path);
        free(path);
        return result;
    }

    return ide_buffer_save(tui->buffer);
}

char* ide_tui_prompt(IDETUI* tui, const char* message) {
    if (!tui) return NULL;

    // Draw prompt on status line
    display_set_theme_style("status.bar");
    display_set_cursor(0, tui->screen_rows - 1);  // x, y
    display_clear_to_eol();
    display_printf(" %s", message);
    display_reset_style();

    echo();
    display_show_cursor();

    char input[256];
    if (getnstr(input, sizeof(input) - 1) == ERR) {
        noecho();
        tui->needs_redraw = true;
        return NULL;
    }

    noecho();
    tui->needs_redraw = true;

    return strdup(input);
}

// -----------------------------------------------------------------------------
// B.7 Main Loop
// -----------------------------------------------------------------------------

void ide_tui_run(IDETUI* tui) {
    if (!tui) return;

    // Initial validation
    if (tui->buffer) {
        ide_validate_all(tui->buffer, tui->diags);
        if (tui->diags->count > 0) {
            ide_tui_set_status(tui, "%u issue(s) found. Ctrl+G to go to line.",
                              tui->diags->count);
        }
    }

    while (tui->running && !g_shutdown_requested) {
        if (tui->needs_redraw) {
            ide_tui_draw(tui);
        }

        DisplayKey key = display_wait_key();
        ide_tui_handle_key(tui, key);
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Kingdom Technology excludes no one."
// The TUI provides full editing in any terminal.

// =============================================================================
// END CLOSING
// =============================================================================
