// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide-gui-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_ide_gui.c — OmniCode IDE GUI Implementation
// Cornerstone display backend for graphical editing

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-gui-impl
// Grounding: Acts 10:34 - "God is no respecter of persons"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_ide_gui.h"
#include "framework/util/graphics/util/screenshot.util.h" // Shared screenshot module
#include "platform.hal.h"  // Platform layer for GUI
#include "renderer.mgr.h" // Renderer for GUI
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

// Shared signal handling (full suite - shared with cornerstone engine)
#include "signals.util.h"  // signals_is_running

// CPI-SI state-aware logging
#include "kernel/dar/phase/detect.phase.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.0 Forward Declarations
// -----------------------------------------------------------------------------

// Template types for block insertion
typedef enum {
    TEMPLATE_3BLOCK,
    TEMPLATE_4BLOCK,
    TEMPLATE_5BLOCK
} IDETemplateType;

// Forward declarations for template insertion (defined in B.7)
static void ide_gui_insert_block(IDEGUI* gui, const char* block_name);
static void ide_gui_insert_template(IDEGUI* gui, IDETemplateType type);

// -----------------------------------------------------------------------------
// B.1 Color Mappings
// -----------------------------------------------------------------------------

static DisplayStyle style_normal(void) {
    return (DisplayStyle){ DISPLAY_COLOR_DEFAULT, DISPLAY_COLOR_DEFAULT, false };
}

static DisplayStyle style_pragma(void) {
    return (DisplayStyle){ DISPLAY_COLOR_MAGENTA, DISPLAY_COLOR_DEFAULT, true };
}

static DisplayStyle style_keyword(void) {
    return (DisplayStyle){ DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_DEFAULT, true };
}

static DisplayStyle style_comment(void) {
    return (DisplayStyle){ DISPLAY_COLOR_GREEN, DISPLAY_COLOR_DEFAULT, false };
}

static DisplayStyle style_string(void) {
    return (DisplayStyle){ DISPLAY_COLOR_RED, DISPLAY_COLOR_DEFAULT, false };
}

static DisplayStyle style_number(void) {
    return (DisplayStyle){ DISPLAY_COLOR_CYAN, DISPLAY_COLOR_DEFAULT, false };
}

static DisplayStyle style_block(void) {
    return (DisplayStyle){ DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_DEFAULT, true };
}

static DisplayStyle style_section(void) {
    return (DisplayStyle){ DISPLAY_COLOR_BLUE, DISPLAY_COLOR_DEFAULT, true };
}

static DisplayStyle style_error(void) {
    return (DisplayStyle){ DISPLAY_COLOR_WHITE, DISPLAY_COLOR_RED, true };
}

static DisplayStyle style_status(void) {
    return (DisplayStyle){ DISPLAY_COLOR_BLACK, DISPLAY_COLOR_WHITE, false };
}

static DisplayStyle style_lineno(void) {
    return (DisplayStyle){ DISPLAY_COLOR_CYAN, DISPLAY_COLOR_DEFAULT, false };
}

// Block type to style mapping
static DisplayStyle block_style(IDEBlockType b) {
    switch (b) {
        case BLOCK_3_METADATA:
        case BLOCK_4_METADATA:
        case BLOCK_5_METADATA:
            return (DisplayStyle){ DISPLAY_COLOR_CYAN, DISPLAY_COLOR_DEFAULT, false };
        case BLOCK_4_SETUP:
            return (DisplayStyle){ DISPLAY_COLOR_BLUE, DISPLAY_COLOR_DEFAULT, false };
        case BLOCK_5_HEADER:
            return (DisplayStyle){ DISPLAY_COLOR_MAGENTA, DISPLAY_COLOR_DEFAULT, false };
        case BLOCK_5_CONTEXT:
            return (DisplayStyle){ DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_DEFAULT, false };
        case BLOCK_4_BODY:
        case BLOCK_3_CONTENT:
        case BLOCK_5_CONTENT:
            return (DisplayStyle){ DISPLAY_COLOR_GREEN, DISPLAY_COLOR_DEFAULT, false };
        case BLOCK_3_CLOSING:
        case BLOCK_4_CLOSING:
        case BLOCK_5_FOOTER:
            return (DisplayStyle){ DISPLAY_COLOR_RED, DISPLAY_COLOR_DEFAULT, false };
        default:
            return style_normal();
    }
}

// Token type to style mapping
static DisplayStyle token_style(IDETokenType t) {
    switch (t) {
        case TOKEN_PRAGMA: return style_pragma();
        case TOKEN_KEYWORD: return style_keyword();
        case TOKEN_COMMENT: return style_comment();
        case TOKEN_STRING: return style_string();
        case TOKEN_NUMBER: return style_number();
        case TOKEN_BLOCK_MARKER: return style_block();
        case TOKEN_SECTION: return style_section();
        case TOKEN_ERROR: return style_error();
        default: return style_normal();
    }
}

// -----------------------------------------------------------------------------
// B.2 Menu System
// -----------------------------------------------------------------------------

// Menu action codes
typedef enum {
    GUI_ACTION_NONE = 0,
    // File menu
    GUI_ACTION_NEW,
    GUI_ACTION_OPEN,
    GUI_ACTION_SAVE,
    GUI_ACTION_SAVE_AS,
    GUI_ACTION_QUIT,
    // Edit menu
    GUI_ACTION_UNDO,
    GUI_ACTION_REDO,
    GUI_ACTION_CUT,
    GUI_ACTION_COPY,
    GUI_ACTION_PASTE,
    GUI_ACTION_FIND,
    GUI_ACTION_GOTO,
    // Insert menu - Templates
    GUI_ACTION_INSERT_3BLOCK,      // 3-block data template
    GUI_ACTION_INSERT_4BLOCK,      // 4-block code template
    GUI_ACTION_INSERT_5BLOCK,      // 5-block documentation template
    GUI_ACTION_INSERT_METADATA,    // Insert METADATA block
    GUI_ACTION_INSERT_SETUP,       // Insert SETUP block (4-block)
    GUI_ACTION_INSERT_HEADER,      // Insert HEADER block (5-block)
    GUI_ACTION_INSERT_CONTEXT,     // Insert CONTEXT block (5-block)
    GUI_ACTION_INSERT_BODY,        // Insert BODY/CONTENT block
    GUI_ACTION_INSERT_CLOSING,     // Insert CLOSING/FOOTER block
    // Build menu
    GUI_ACTION_BUILD,              // Compile current file
    GUI_ACTION_RUN,                // Run compiled output
    GUI_ACTION_BUILD_RUN,          // Build and run
    GUI_ACTION_RUN_CORNERSTONE,    // Run in Cornerstone engine
    GUI_ACTION_CLEAN,              // Clean build artifacts
    // View menu
    GUI_ACTION_TOGGLE_LINENO,
    GUI_ACTION_TOGGLE_COLORS,
    GUI_ACTION_TOGGLE_CLI,         // Toggle CLI panel
    // Help menu
    GUI_ACTION_ABOUT,
    GUI_ACTION_KEYS
} IDEGUIMenuAction;

// Initialize menu bar with default menus
static void ide_gui_menubar_init(IDEGUIMenuBar* menubar) {
    memset(menubar, 0, sizeof(IDEGUIMenuBar));
    menubar->active_menu = GUI_MENU_NONE;
    menubar->active_item = 0;
    menubar->visible = true;

    // File menu
    IDEGUIMenu* file = &menubar->menus[GUI_MENU_FILE];
    file->label = "File";
    file->hotkey = 'F';
    file->item_count = 5;
    file->items[0] = (IDEGUIMenuItem){"New",      "Ctrl+N", GUI_ACTION_NEW,     false};
    file->items[1] = (IDEGUIMenuItem){"Open...",  "Ctrl+O", GUI_ACTION_OPEN,    false};
    file->items[2] = (IDEGUIMenuItem){"Save",     "Ctrl+S", GUI_ACTION_SAVE,    false};
    file->items[3] = (IDEGUIMenuItem){"Save As...", NULL,   GUI_ACTION_SAVE_AS, true};
    file->items[4] = (IDEGUIMenuItem){"Quit",     "Ctrl+Q", GUI_ACTION_QUIT,    false};

    // Edit menu
    IDEGUIMenu* edit = &menubar->menus[GUI_MENU_EDIT];
    edit->label = "Edit";
    edit->hotkey = 'E';
    edit->item_count = 6;
    edit->items[0] = (IDEGUIMenuItem){"Undo",     "Ctrl+Z", GUI_ACTION_UNDO,  false};
    edit->items[1] = (IDEGUIMenuItem){"Redo",     "Ctrl+Y", GUI_ACTION_REDO,  true};
    edit->items[2] = (IDEGUIMenuItem){"Cut",      "Ctrl+X", GUI_ACTION_CUT,   false};
    edit->items[3] = (IDEGUIMenuItem){"Copy",     "Ctrl+C", GUI_ACTION_COPY,  false};
    edit->items[4] = (IDEGUIMenuItem){"Paste",    "Ctrl+V", GUI_ACTION_PASTE, true};
    edit->items[5] = (IDEGUIMenuItem){"Find...",  "Ctrl+F", GUI_ACTION_FIND,  false};

    // Insert menu - Block templates
    IDEGUIMenu* insert = &menubar->menus[GUI_MENU_INSERT];
    insert->label = "Insert";
    insert->hotkey = 'I';
    insert->item_count = 9;
    insert->items[0] = (IDEGUIMenuItem){"3-Block (Data)",    "Ctrl+3", GUI_ACTION_INSERT_3BLOCK,   false};
    insert->items[1] = (IDEGUIMenuItem){"4-Block (Code)",    "Ctrl+4", GUI_ACTION_INSERT_4BLOCK,   false};
    insert->items[2] = (IDEGUIMenuItem){"5-Block (Docs)",    "Ctrl+5", GUI_ACTION_INSERT_5BLOCK,   true};
    insert->items[3] = (IDEGUIMenuItem){"METADATA Block",    NULL,     GUI_ACTION_INSERT_METADATA, false};
    insert->items[4] = (IDEGUIMenuItem){"SETUP Block",       NULL,     GUI_ACTION_INSERT_SETUP,    false};
    insert->items[5] = (IDEGUIMenuItem){"HEADER Block",      NULL,     GUI_ACTION_INSERT_HEADER,   false};
    insert->items[6] = (IDEGUIMenuItem){"CONTEXT Block",     NULL,     GUI_ACTION_INSERT_CONTEXT,  false};
    insert->items[7] = (IDEGUIMenuItem){"BODY Block",        NULL,     GUI_ACTION_INSERT_BODY,     false};
    insert->items[8] = (IDEGUIMenuItem){"CLOSING Block",     NULL,     GUI_ACTION_INSERT_CLOSING,  false};

    // Build menu
    IDEGUIMenu* build = &menubar->menus[GUI_MENU_BUILD];
    build->label = "Build";
    build->hotkey = 'B';
    build->item_count = 5;
    build->items[0] = (IDEGUIMenuItem){"Build",               "F5",  GUI_ACTION_BUILD,           false};
    build->items[1] = (IDEGUIMenuItem){"Run",                 "F6",  GUI_ACTION_RUN,             false};
    build->items[2] = (IDEGUIMenuItem){"Build & Run",         "F7",  GUI_ACTION_BUILD_RUN,       true};
    build->items[3] = (IDEGUIMenuItem){"Run in Cornerstone",  "F8",  GUI_ACTION_RUN_CORNERSTONE, false};
    build->items[4] = (IDEGUIMenuItem){"Clean",               NULL,  GUI_ACTION_CLEAN,           false};

    // View menu
    IDEGUIMenu* view = &menubar->menus[GUI_MENU_VIEW];
    view->label = "View";
    view->hotkey = 'V';
    view->item_count = 3;
    view->items[0] = (IDEGUIMenuItem){"Line Numbers",  NULL,  GUI_ACTION_TOGGLE_LINENO,  false};
    view->items[1] = (IDEGUIMenuItem){"Block Colors",  NULL,  GUI_ACTION_TOGGLE_COLORS,  false};
    view->items[2] = (IDEGUIMenuItem){"CPI-SI CLI",    "`",   GUI_ACTION_TOGGLE_CLI,     false};

    // Help menu
    IDEGUIMenu* help = &menubar->menus[GUI_MENU_HELP];
    help->label = "Help";
    help->hotkey = 'H';
    help->item_count = 2;
    help->items[0] = (IDEGUIMenuItem){"Key Bindings", "F1",  GUI_ACTION_KEYS,  false};
    help->items[1] = (IDEGUIMenuItem){"About",        NULL,  GUI_ACTION_ABOUT, false};
}

// Draw menu bar at top of screen
static void ide_gui_draw_menubar(IDEGUI* gui) {
    if (!gui || !gui->menubar.visible) return;

    // Menu bar background
    DisplayStyle bar_style = {DISPLAY_COLOR_BLACK, DISPLAY_COLOR_WHITE, false};
    DisplayStyle select_style = {DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLUE, false};

    // Draw menu bar background
    display_set_cursor(0, 0);
    char spaces[256];
    int fill_len = gui->screen_cols < 255 ? gui->screen_cols : 255;
    memset(spaces, ' ', fill_len);
    spaces[fill_len] = '\0';
    display_print_styled(spaces, bar_style);

    // Draw menu titles
    int x = 1;
    for (int m = 0; m < GUI_MENU_COUNT; m++) {
        IDEGUIMenu* menu = &gui->menubar.menus[m];
        display_set_cursor(x, 0);

        char label[32];
        snprintf(label, sizeof(label), " %s ", menu->label);

        if (m == gui->menubar.active_menu) {
            display_print_styled(label, select_style);
        } else {
            display_print_styled(label, bar_style);
        }

        x += strlen(menu->label) + 2;
    }

    // Draw F1 hint on right
    display_set_cursor(gui->screen_cols - 12, 0);
    display_print_styled(" F1: Menu ", bar_style);
}

// Draw dropdown menu
static void ide_gui_draw_menu_dropdown(IDEGUI* gui) {
    if (!gui || gui->menubar.active_menu == GUI_MENU_NONE) return;

    IDEGUIMenu* menu = &gui->menubar.menus[gui->menubar.active_menu];

    DisplayStyle item_style = {DISPLAY_COLOR_BLACK, DISPLAY_COLOR_WHITE, false};
    DisplayStyle select_style = {DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLUE, false};

    // Calculate menu position
    int x = 1;
    for (int m = 0; m < gui->menubar.active_menu; m++) {
        x += strlen(gui->menubar.menus[m].label) + 2;
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
        display_set_cursor(x, i + 1);

        DisplayStyle style = (i == gui->menubar.active_item) ? select_style : item_style;

        // Item label with padding
        char item_text[64];
        snprintf(item_text, sizeof(item_text), " %-*s ", max_width - 2, menu->items[i].label);
        display_print_styled(item_text, style);

        // Shortcut
        if (menu->items[i].shortcut) {
            int shortcut_x = x + max_width - strlen(menu->items[i].shortcut) - 1;
            display_set_cursor(shortcut_x, i + 1);
            display_print_styled(menu->items[i].shortcut, style);
        }
    }
}

// Forward declaration
static void ide_gui_recalc_layout(IDEGUI* gui);

// Execute menu action
static void ide_gui_menu_execute(IDEGUI* gui, IDEGUIMenuAction action) {
    switch (action) {
        case GUI_ACTION_NEW:
            ide_gui_set_status(gui, "New file (not implemented yet)");
            break;
        case GUI_ACTION_OPEN:
            {
                char* path = ide_gui_prompt(gui, "Open file: ");
                if (path && strlen(path) > 0) {
                    IDEBuffer* new_buf = ide_buffer_load(path);
                    if (new_buf) {
                        ide_buffer_free(gui->buffer);
                        gui->buffer = new_buf;
                        ide_validate_all(new_buf, gui->diags);
                        ide_gui_set_status(gui, "Opened: %s", path);
                    } else {
                        ide_gui_set_status(gui, "Failed to open: %s", path);
                    }
                    free(path);
                }
            }
            break;
        case GUI_ACTION_SAVE:
            if (ide_gui_save(gui)) {
                ide_gui_set_status(gui, "Saved: %s", gui->buffer->filepath);
            } else {
                ide_gui_set_status(gui, "Save failed!");
            }
            break;
        case GUI_ACTION_SAVE_AS:
            {
                char* path = ide_gui_prompt(gui, "Save as: ");
                if (path && strlen(path) > 0) {
                    if (ide_buffer_save_as(gui->buffer, path)) {
                        ide_gui_set_status(gui, "Saved: %s", path);
                    } else {
                        ide_gui_set_status(gui, "Save failed!");
                    }
                    free(path);
                }
            }
            break;
        case GUI_ACTION_QUIT:
            if (gui->buffer->modified) {
                ide_gui_set_status(gui, "Unsaved changes! Press Ctrl+Q to quit.");
            } else {
                gui->running = false;
            }
            break;
        case GUI_ACTION_TOGGLE_LINENO:
            gui->config.show_line_numbers = !gui->config.show_line_numbers;
            ide_gui_recalc_layout(gui);
            ide_gui_set_status(gui, "Line numbers %s", gui->config.show_line_numbers ? "on" : "off");
            break;
        case GUI_ACTION_TOGGLE_COLORS:
            gui->config.show_block_colors = !gui->config.show_block_colors;
            ide_gui_set_status(gui, "Block colors %s", gui->config.show_block_colors ? "on" : "off");
            break;
        case GUI_ACTION_GOTO:
            {
                char* input = ide_gui_prompt(gui, "Go to line: ");
                if (input) {
                    int line = atoi(input);
                    if (line > 0) {
                        ide_gui_goto_line(gui, line - 1);
                    }
                    free(input);
                }
            }
            break;
        case GUI_ACTION_KEYS:
            ide_gui_set_status(gui,
                "F1:Menu Ctrl+S:Save Ctrl+Q:Quit Ctrl+G:Goto");
            break;
        case GUI_ACTION_ABOUT:
            ide_gui_set_status(gui,
                "OmniCode IDE v0.1 - Kingdom Technology | \"In the beginning God created...\"");
            break;

        // Template insertion actions
        case GUI_ACTION_INSERT_3BLOCK:
            ide_gui_insert_template(gui, TEMPLATE_3BLOCK);
            break;
        case GUI_ACTION_INSERT_4BLOCK:
            ide_gui_insert_template(gui, TEMPLATE_4BLOCK);
            break;
        case GUI_ACTION_INSERT_5BLOCK:
            ide_gui_insert_template(gui, TEMPLATE_5BLOCK);
            break;
        case GUI_ACTION_INSERT_METADATA:
            ide_gui_insert_block(gui, "METADATA");
            break;
        case GUI_ACTION_INSERT_SETUP:
            ide_gui_insert_block(gui, "SETUP");
            break;
        case GUI_ACTION_INSERT_HEADER:
            ide_gui_insert_block(gui, "HEADER");
            break;
        case GUI_ACTION_INSERT_CONTEXT:
            ide_gui_insert_block(gui, "CONTEXT");
            break;
        case GUI_ACTION_INSERT_BODY:
            ide_gui_insert_block(gui, "BODY");
            break;
        case GUI_ACTION_INSERT_CLOSING:
            ide_gui_insert_block(gui, "CLOSING");
            break;
        case GUI_ACTION_TOGGLE_CLI:
            ide_gui_cli_toggle(gui);
            break;

        default:
            ide_gui_set_status(gui, "Action not implemented");
            break;
    }

    // Close menu after action
    gui->menubar.active_menu = GUI_MENU_NONE;
    gui->focus = GUI_FOCUS_EDITOR;
}

// Handle menu-specific keys
static bool ide_gui_menu_handle_key(IDEGUI* gui, DisplayKey key) {
    if (gui->focus != GUI_FOCUS_MENU) return false;

    IDEGUIMenuBar* mb = &gui->menubar;

    switch (key) {
        case DISPLAY_KEY_LEFT:
            if (mb->active_menu == GUI_MENU_NONE) {
                mb->active_menu = GUI_MENU_FILE;
            } else if (mb->active_menu > 0) {
                mb->active_menu--;
            } else {
                mb->active_menu = GUI_MENU_COUNT - 1;
            }
            mb->active_item = 0;
            return true;

        case DISPLAY_KEY_RIGHT:
            if (mb->active_menu == GUI_MENU_NONE) {
                mb->active_menu = GUI_MENU_FILE;
            } else if (mb->active_menu < GUI_MENU_COUNT - 1) {
                mb->active_menu++;
            } else {
                mb->active_menu = GUI_MENU_FILE;
            }
            mb->active_item = 0;
            return true;

        case DISPLAY_KEY_UP:
            if (mb->active_menu != GUI_MENU_NONE) {
                IDEGUIMenu* menu = &mb->menus[mb->active_menu];
                if (mb->active_item > 0) {
                    mb->active_item--;
                } else {
                    mb->active_item = menu->item_count - 1;
                }
            }
            return true;

        case DISPLAY_KEY_DOWN:
            if (mb->active_menu != GUI_MENU_NONE) {
                IDEGUIMenu* menu = &mb->menus[mb->active_menu];
                if (mb->active_item < menu->item_count - 1) {
                    mb->active_item++;
                } else {
                    mb->active_item = 0;
                }
            }
            return true;

        case DISPLAY_KEY_ENTER:
            if (mb->active_menu != GUI_MENU_NONE) {
                IDEGUIMenu* menu = &mb->menus[mb->active_menu];
                IDEGUIMenuAction action = menu->items[mb->active_item].action;
                ide_gui_menu_execute(gui, action);
            }
            return true;

        case DISPLAY_KEY_ESCAPE:
            mb->active_menu = GUI_MENU_NONE;
            gui->focus = GUI_FOCUS_EDITOR;
            ide_gui_set_status(gui, "Editor");
            return true;

        default:
            break;
    }

    return false;
}

// Recalculate layout dimensions
static void ide_gui_recalc_layout(IDEGUI* gui) {
    if (!gui) return;

    display_get_size(&gui->screen_cols, &gui->screen_rows);

    // Menu bar takes 1 row, status takes 2 rows
    int menu_rows = gui->menubar.visible ? 1 : 0;
    int status_rows = gui->config.show_status_bar ? 2 : 0;

    gui->edit_rows = gui->screen_rows - status_rows - menu_rows;
    gui->edit_cols = gui->screen_cols - (gui->config.show_line_numbers ? 6 : 0);

    if (gui->edit_rows < 1) gui->edit_rows = 1;
    if (gui->edit_cols < 10) gui->edit_cols = 10;

    // Update buffer viewport
    if (gui->buffer) {
        gui->buffer->visible_lines = gui->edit_rows;
        gui->buffer->visible_cols = gui->edit_cols;
    }
}

// -----------------------------------------------------------------------------
// B.3 GUI Lifecycle
// -----------------------------------------------------------------------------

IDEGUI* ide_gui_create(IDEBuffer* buffer) {
    LOG_DEBUG("ide-gui", "Creating GUI instance");

    IDEGUI* gui = malloc(sizeof(IDEGUI));
    if (!gui) {
        LOG_ERROR("ide-gui", "Failed to allocate GUI structure");
        return NULL;
    }

    memset(gui, 0, sizeof(IDEGUI));
    gui->buffer = buffer;
    gui->running = true;
    gui->needs_redraw = true;

    // Default config
    gui->config.show_line_numbers = true;
    gui->config.show_block_colors = true;
    gui->config.show_status_bar = true;
    gui->config.syntax_highlight = true;
    gui->config.tab_width = 4;
    gui->config.soft_wrap = false;

    // Initialize platform (creates GLFW window for GUI)
    PlatformConfig pconfig = {
        .title = "OmniCode IDE - Kingdom Technology",
        .width = 1280,
        .height = 720,
        .vsync = true,
        .app_id = "omni-ide"
    };
    gui->platform_initialized = (platform_init(&pconfig) == PLATFORM_OK);

    // Initialize renderer (required before display layer can get viewport size)
    if (gui->platform_initialized) {
        RendererError rerr = renderer_init(pconfig.width, pconfig.height);
        if (rerr != RENDERER_OK) {
            LOG_WARN("ide", "Renderer init failed, falling back to TUI");
            platform_shutdown();
            gui->platform_initialized = false;
        }
    }

    // Initialize Cornerstone display
    DisplayConfig config = {
        .mode = gui->platform_initialized ? DISPLAY_GUI : DISPLAY_TUI,
        .width = gui->platform_initialized ? 1280 : 80,
        .height = gui->platform_initialized ? 720 : 24,
        .title = "OmniCode IDE - Kingdom Technology"
    };

    DisplayError err = display_init(&config);
    if (err != DISPLAY_OK) {
        // Fallback to TUI if GUI fails
        if (gui->platform_initialized) {
            platform_shutdown();
            gui->platform_initialized = false;
        }
        config.mode = DISPLAY_TUI;
        config.width = 80;
        config.height = 24;
        err = display_init(&config);
        if (err != DISPLAY_OK) {
            free(gui);
            return NULL;
        }
    }

    // Create diagnostics
    gui->diags = ide_diagnostics_create();

    // Initialize menu bar
    ide_gui_menubar_init(&gui->menubar);
    gui->focus = GUI_FOCUS_EDITOR;

    // Calculate layout (accounts for menu bar)
    ide_gui_recalc_layout(gui);

    ide_gui_set_status(gui, "OmniCode IDE | F1: Menu | Ctrl+S: Save | Ctrl+Q: Quit");

    LOG_INFO("ide-gui", "GUI created: %dx%d, platform=%s",
             gui->screen_cols, gui->screen_rows,
             gui->platform_initialized ? "initialized" : "fallback");

    return gui;
}

void ide_gui_destroy(IDEGUI* gui) {
    if (!gui) return;

    LOG_DEBUG("ide-gui", "Destroying GUI instance");

    // Capture screenshot of last known state before shutdown
    if (gui->platform_initialized) {
        screenshot_capture("shutdown");
    }

    display_shutdown();

    // Shutdown renderer and platform if we initialized them
    if (gui->platform_initialized) {
        renderer_shutdown();
        platform_shutdown();
    }

    ide_diagnostics_free(gui->diags);
    free(gui);

    LOG_INFO("ide-gui", "GUI destroyed");
}

// -----------------------------------------------------------------------------
// B.3 Display
// -----------------------------------------------------------------------------

void ide_gui_draw_line(IDEGUI* gui, uint32_t line_num) {
    if (!gui || !gui->buffer) return;

    IDEBuffer* buf = gui->buffer;
    int row = line_num - buf->scroll_line;
    if (row < 0 || row >= gui->edit_rows) return;

    int col = 0;

    // Line number
    if (gui->config.show_line_numbers) {
        char lineno[8];
        snprintf(lineno, sizeof(lineno), "%5u ", line_num + 1);
        display_set_cursor(0, row);
        display_print_styled(lineno, style_lineno());
        col = 6;
    }

    if (line_num >= buf->line_count) return;

    IDELine* line = &buf->lines[line_num];

    // Get block style for this line
    DisplayStyle base_style = style_normal();
    if (gui->config.show_block_colors && line->block != BLOCK_NONE) {
        base_style = block_style(line->block);
    }

    // Draw text with syntax highlighting
    if (gui->config.syntax_highlight && line->token_count > 0) {
        uint32_t pos = 0;
        for (uint32_t t = 0; t < line->token_count; t++) {
            IDEToken* tok = &line->tokens[t];

            // Print before token (with base style)
            while (pos < tok->start && pos < line->length) {
                char ch[2] = { line->text[pos++], '\0' };
                display_set_cursor(col++, row);
                display_print_styled(ch, base_style);
            }

            // Print token with color
            DisplayStyle ts = token_style(tok->type);
            while (pos < tok->end && pos < line->length) {
                char ch[2] = { line->text[pos++], '\0' };
                display_set_cursor(col++, row);
                display_print_styled(ch, ts);
            }
        }

        // Print remainder
        while (pos < line->length) {
            char ch[2] = { line->text[pos++], '\0' };
            display_set_cursor(col++, row);
            display_print_styled(ch, base_style);
        }
    } else {
        // No syntax highlighting - just print
        display_set_cursor(col, row);
        display_print_styled(line->text, base_style);
    }
}

void ide_gui_draw_status(IDEGUI* gui) {
    if (!gui || !gui->config.show_status_bar) return;

    IDEBuffer* buf = gui->buffer;
    int row = gui->screen_rows - 1;

    // Status bar background - fill both lines
    DisplayStyle status = style_status();

    // File info line
    char info[256];
    const char* structure = "";
    if (buf) {
        switch (ide_get_structure(buf)) {
            case STRUCTURE_3_BLOCK: structure = "[3-Block]"; break;
            case STRUCTURE_4_BLOCK: structure = "[4-Block]"; break;
            case STRUCTURE_5_BLOCK: structure = "[5-Block]"; break;
            default: structure = "[Unknown]"; break;
        }
    }
    snprintf(info, sizeof(info), " %s %s %s",
             buf && buf->filepath ? buf->filepath : "[New File]",
             buf && buf->modified ? "[Modified]" : "",
             structure);

    display_set_cursor(0, row - 1);
    display_print_styled(info, status);

    // Position info (right side)
    if (buf) {
        char pos[32];
        snprintf(pos, sizeof(pos), "Ln %u, Col %u ",
                 buf->cursor.line + 1, buf->cursor.col + 1);
        display_set_cursor(gui->screen_cols - (int32_t)strlen(pos), row - 1);
        display_print_styled(pos, status);
    }

    // Status message line
    char msg[258];  // Extra space for " " prefix
    snprintf(msg, sizeof(msg), " %s", gui->status_message);
    display_set_cursor(0, row);
    display_print_styled(msg, status);
}

void ide_gui_draw(IDEGUI* gui) {
    if (!gui) return;

    display_clear();

    // Draw menu bar first (takes row 0)
    ide_gui_draw_menubar(gui);

    int menu_offset = gui->menubar.visible ? 1 : 0;

    // Draw lines (offset by menu bar)
    for (int i = 0; i < gui->edit_rows; i++) {
        uint32_t line = gui->buffer->scroll_line + i;
        // Offset drawing by menu bar row
        int orig_row = line - gui->buffer->scroll_line;
        (void)orig_row;  // Draw at i + menu_offset

        if (line < gui->buffer->line_count) {
            IDEBuffer* buf = gui->buffer;
            IDELine* ln = &buf->lines[line];
            int col = 0;

            // Line number
            if (gui->config.show_line_numbers) {
                char lineno[16];
                snprintf(lineno, sizeof(lineno), "%5u ", line + 1);
                display_set_cursor(0, i + menu_offset);
                display_print_styled(lineno, style_lineno());
                col = 6;
            }

            // Get block style for this line
            DisplayStyle base_style = style_normal();
            if (gui->config.show_block_colors && ln->block != BLOCK_NONE) {
                base_style = block_style(ln->block);
            }

            // Draw text
            display_set_cursor(col, i + menu_offset);
            if (gui->config.syntax_highlight && ln->token_count > 0) {
                uint32_t pos = 0;
                for (uint32_t t = 0; t < ln->token_count; t++) {
                    IDEToken* tok = &ln->tokens[t];
                    while (pos < tok->start && pos < ln->length) {
                        char ch[2] = { ln->text[pos++], '\0' };
                        display_print_styled(ch, base_style);
                    }
                    DisplayStyle ts = token_style(tok->type);
                    while (pos < tok->end && pos < ln->length) {
                        char ch[2] = { ln->text[pos++], '\0' };
                        display_print_styled(ch, ts);
                    }
                }
                while (pos < ln->length) {
                    char ch[2] = { ln->text[pos++], '\0' };
                    display_print_styled(ch, base_style);
                }
            } else {
                display_print_styled(ln->text, base_style);
            }
        }
    }

    // Draw CLI panel if visible
    if (gui->cli.visible) {
        int cli_start = gui->edit_rows + (gui->menubar.visible ? 1 : 0);

        // Draw CLI border
        display_set_cursor(0, cli_start);
        DisplayStyle border_style = { DISPLAY_COLOR_CYAN, DISPLAY_COLOR_DEFAULT, false };
        char border[256];
        memset(border, '-', gui->screen_cols - 1);
        border[gui->screen_cols - 1] = '\0';
        border[0] = '[';
        const char* title = " CPI-SI ";
        memcpy(border + 1, title, strlen(title));
        border[1 + strlen(title)] = ']';
        display_print_styled(border, border_style);

        // Draw output lines
        DisplayStyle output_style = { DISPLAY_COLOR_WHITE, DISPLAY_COLOR_DEFAULT, false };
        for (int i = 0; i < CLI_OUTPUT_LINES; i++) {
            display_set_cursor(0, cli_start + 1 + i);
            if (gui->cli.output[i][0] != '\0') {
                display_print_styled(gui->cli.output[i], output_style);
            }
        }

        // Draw input line
        int input_row = cli_start + 1 + CLI_OUTPUT_LINES;
        display_set_cursor(0, input_row);
        DisplayStyle prompt_style = { DISPLAY_COLOR_GREEN, DISPLAY_COLOR_DEFAULT, true };
        display_print_styled("> ", prompt_style);
        display_print_styled(gui->cli.input, output_style);
    }

    // Draw status
    ide_gui_draw_status(gui);

    // Draw menu dropdown (on top of everything if menu active)
    if (gui->focus == GUI_FOCUS_MENU && gui->menubar.active_menu != GUI_MENU_NONE) {
        ide_gui_draw_menu_dropdown(gui);
    }

    // Position cursor
    if (gui->focus == GUI_FOCUS_CLI) {
        // Cursor in CLI input
        int cli_start = gui->edit_rows + (gui->menubar.visible ? 1 : 0);
        int input_row = cli_start + 1 + CLI_OUTPUT_LINES;
        display_set_cursor(2 + gui->cli.input_pos, input_row);
    } else if (gui->buffer && gui->focus == GUI_FOCUS_EDITOR) {
        IDEBuffer* buf = gui->buffer;
        int row = buf->cursor.line - buf->scroll_line + menu_offset;
        int col = buf->cursor.col + (gui->config.show_line_numbers ? 6 : 0);
        display_set_cursor(col, row);
    }

    display_refresh();
    gui->needs_redraw = false;
}

void ide_gui_set_status(IDEGUI* gui, const char* format, ...) {
    if (!gui) return;

    va_list args;
    va_start(args, format);
    vsnprintf(gui->status_message, sizeof(gui->status_message), format, args);
    va_end(args);
}

// -----------------------------------------------------------------------------
// B.4 Navigation
// -----------------------------------------------------------------------------

void ide_gui_ensure_visible(IDEGUI* gui) {
    if (!gui || !gui->buffer) return;

    IDEBuffer* buf = gui->buffer;

    // Scroll up if cursor above viewport
    if (buf->cursor.line < buf->scroll_line) {
        buf->scroll_line = buf->cursor.line;
        gui->needs_redraw = true;
    }

    // Scroll down if cursor below viewport
    if (buf->cursor.line >= buf->scroll_line + (uint32_t)gui->edit_rows) {
        buf->scroll_line = buf->cursor.line - gui->edit_rows + 1;
        gui->needs_redraw = true;
    }
}

void ide_gui_goto_line(IDEGUI* gui, uint32_t line) {
    if (!gui || !gui->buffer) return;
    ide_cursor_goto(gui->buffer, line, 0);
    ide_gui_ensure_visible(gui);
    gui->needs_redraw = true;
}

// -----------------------------------------------------------------------------
// B.5 Input Handling
// -----------------------------------------------------------------------------

void ide_gui_handle_key(IDEGUI* gui, DisplayKey key) {
    if (!gui || !gui->buffer) return;

    IDEBuffer* buf = gui->buffer;

    LOG_TRACE("ide-gui", "Key event: key=%d, focus=%d", (int)key, (int)gui->focus);

    // Handle CLI keys first if CLI is focused
    if (gui->focus == GUI_FOCUS_CLI) {
        ide_gui_cli_handle_key(gui, key);
        return;
    }

    // Handle menu keys first if menu is focused
    if (gui->focus == GUI_FOCUS_MENU) {
        if (ide_gui_menu_handle_key(gui, key)) {
            gui->needs_redraw = true;
            return;
        }
    }

    // Global: backtick (`) toggles CLI panel
    if (key == DISPLAY_KEY_CHAR) {
        char ch = display_get_last_char();
        if (ch == '`') {
            ide_gui_cli_toggle(gui);
            return;
        }
    }

    switch (key) {
        // Mouse click handling (disabled in Phase A API)
        // NOTE: Mouse events need full refactor for new input API
        // TODO: Refactor to use new input system for mouse handling
        //
        // case DISPLAY_KEY_MOUSE: {
        //     DisplayMouseEvent mouse = display_get_mouse_event();
        //     if (mouse.button == DISPLAY_MOUSE_BUTTON_LEFT) {
        //         // Check if click is on menu bar (row 0)
        //         if (mouse.y == 0 && gui->menubar.visible) {
        //             // Find which menu was clicked
        //             int x = 2;  // Start after initial padding
        //             for (int m = 0; m < GUI_MENU_COUNT; m++) {
        //                 int menu_width = strlen(gui->menubar.menus[m].label) + 2;
        //                 if (mouse.x >= x && mouse.x < x + menu_width) {
        //                     // Clicked this menu
        //                     gui->focus = GUI_FOCUS_MENU;
        //                     gui->menubar.active_menu = m;
        //                     gui->menubar.active_item = 0;
        //                     ide_gui_set_status(gui, "Menu | Arrows: Navigate | Enter: Select | Esc: Close");
        //                     gui->needs_redraw = true;
        //                     return;
        //                 }
        //                 x += menu_width;
        //             }
        //         }
        //         // Click elsewhere could set cursor position in editor
        //         // (future enhancement)
        //     }
        //     return;
        // }

        // F1 - Open menu
        case DISPLAY_KEY_F1:
            gui->focus = GUI_FOCUS_MENU;
            gui->menubar.active_menu = GUI_MENU_FILE;
            gui->menubar.active_item = 0;
            ide_gui_set_status(gui, "Menu | Arrows: Navigate | Enter: Select | Esc: Close");
            gui->needs_redraw = true;
            return;

        // ---------------------------------------------------------------------
        // Ctrl+Key Commands
        // ---------------------------------------------------------------------

        // Ctrl+Q - Quit
        case DISPLAY_KEY_CTRL_Q:
            if (buf->modified) {
                ide_gui_set_status(gui, "Unsaved changes! Press Ctrl+Q again to quit.");
                static int quit_count = 0;
                if (++quit_count > 1) {
                    gui->running = false;
                }
            } else {
                gui->running = false;
            }
            break;

        // Ctrl+S - Save
        case DISPLAY_KEY_CTRL_S:
            if (ide_gui_save(gui)) {
                ide_gui_set_status(gui, "Saved: %s", buf->filepath);
            } else {
                ide_gui_set_status(gui, "Save failed!");
            }
            break;

        // Ctrl+O - Open file
        case DISPLAY_KEY_CTRL_O:
            {
                char* path = ide_gui_prompt(gui, "Open file: ");
                if (path) {
                    IDEBuffer* new_buf = ide_buffer_load(path);
                    if (new_buf) {
                        ide_buffer_free(gui->buffer);
                        gui->buffer = new_buf;
                        buf = new_buf;  // Update local pointer
                        ide_gui_set_status(gui, "Opened: %s", path);
                    } else {
                        ide_gui_set_status(gui, "Failed to open: %s", path);
                    }
                    free(path);
                }
            }
            break;

        // Ctrl+G - Go to line
        case DISPLAY_KEY_CTRL_G:
            {
                char* line_str = ide_gui_prompt(gui, "Go to line: ");
                if (line_str) {
                    int line = atoi(line_str);
                    if (line > 0) {
                        uint32_t target_line = (uint32_t)(line - 1);
                        uint32_t max_line = ide_line_count(buf);
                        if (target_line >= max_line) {
                            target_line = max_line - 1;
                        }
                        ide_cursor_goto(buf, target_line, 0);
                        ide_gui_ensure_visible(gui);
                        ide_gui_set_status(gui, "Line %d", line);
                    }
                    free(line_str);
                }
            }
            break;

        // Ctrl+F - Find
        case DISPLAY_KEY_CTRL_F:
            {
                char* search = ide_gui_prompt(gui, "Find: ");
                if (search && strlen(search) > 0) {
                    // Simple forward search from current position
                    bool found = false;
                    uint32_t start_line = buf->cursor.line;
                    uint32_t total_lines = ide_line_count(buf);
                    for (uint32_t y = start_line; y < total_lines && !found; y++) {
                        const char* line = ide_get_line(buf, y);
                        if (!line) continue;
                        int start_x = (y == start_line) ? (int)buf->cursor.col + 1 : 0;
                        const char* match = strstr(line + start_x, search);
                        if (match) {
                            ide_cursor_goto(buf, y, (uint32_t)(match - line));
                            ide_gui_ensure_visible(gui);
                            ide_gui_set_status(gui, "Found at line %d", y + 1);
                            found = true;
                        }
                    }
                    if (!found) {
                        ide_gui_set_status(gui, "Not found: %s", search);
                    }
                    free(search);
                }
            }
            break;

        // Ctrl+N - New file (reload empty)
        case DISPLAY_KEY_CTRL_N:
            // Clear by reinitializing buffer content
            if (buf->modified) {
                ide_gui_set_status(gui, "Unsaved changes! Save first (Ctrl+S)");
            } else {
                // Free existing lines, start fresh
                for (uint32_t i = 0; i < buf->line_count; i++) {
                    free(buf->lines[i].text);
                }
                buf->line_count = 1;
                buf->lines[0].text = strdup("");
                buf->lines[0].length = 0;
                buf->lines[0].capacity = 1;
                buf->cursor.line = 0;
                buf->cursor.col = 0;
                buf->modified = false;
                free(buf->filepath);
                buf->filepath = NULL;
                ide_gui_set_status(gui, "New file");
            }
            break;

        // Legacy single-key fallbacks (for compatibility)
        // NOTE: Disabled - conflicts with CTRL_* in new API (both map to same KEY_*)
        // TODO: Refactor to use KeyState with modifier checking
        // In new API: Check KeyState.modifiers to distinguish Q from Ctrl+Q
        //
        // case DISPLAY_KEY_Q:
        //     if (buf->modified) {
        //         ide_gui_set_status(gui, "Unsaved changes! Use Ctrl+Q to quit.");
        //     } else {
        //         gui->running = false;
        //     }
        //     break;
        //
        // case DISPLAY_KEY_S:
        //     // Hint to use Ctrl+S
        //     ide_gui_set_status(gui, "Use Ctrl+S to save");
        //     break;

        // Cursor movement
        case DISPLAY_KEY_UP:
            ide_cursor_up(buf);
            ide_gui_ensure_visible(gui);
            break;

        case DISPLAY_KEY_DOWN:
            ide_cursor_down(buf);
            ide_gui_ensure_visible(gui);
            break;

        case DISPLAY_KEY_LEFT:
            ide_cursor_left(buf);
            break;

        case DISPLAY_KEY_RIGHT:
            ide_cursor_right(buf);
            break;

        // Character input
        case DISPLAY_KEY_CHAR:
            {
                char ch = display_get_last_char();
                if (ch >= 32 && ch < 127) {
                    ide_insert_char(buf, ch);
                }
            }
            break;

        case DISPLAY_KEY_ENTER:
            ide_newline(buf);
            ide_gui_ensure_visible(gui);
            break;

        case DISPLAY_KEY_SPACE:
            ide_insert_char(buf, ' ');
            break;

        case DISPLAY_KEY_ESCAPE:
            // Close menu if open, otherwise quit
            if (gui->focus == GUI_FOCUS_MENU) {
                gui->menubar.active_menu = GUI_MENU_NONE;
                gui->focus = GUI_FOCUS_EDITOR;
                ide_gui_set_status(gui, "Editor");
            } else {
                gui->running = false;
            }
            break;

        default:
            break;
    }

    gui->needs_redraw = true;
}

// -----------------------------------------------------------------------------
// B.6 Commands
// -----------------------------------------------------------------------------

bool ide_gui_save(IDEGUI* gui) {
    if (!gui || !gui->buffer) return false;

    if (!gui->buffer->filepath) {
        char* path = ide_gui_prompt(gui, "Save as: ");
        if (!path) return false;
        bool result = ide_buffer_save_as(gui->buffer, path);
        free(path);
        return result;
    }

    return ide_buffer_save(gui->buffer);
}

char* ide_gui_prompt(IDEGUI* gui, const char* message) {
    if (!gui) return NULL;

    char input[256] = {0};
    int pos = 0;

    // Draw initial prompt with full UI redraw (required for double-buffering)
    ide_gui_draw(gui);
    display_set_cursor(0, gui->screen_rows - 1);
    DisplayStyle prompt_style = { DISPLAY_COLOR_BLACK, DISPLAY_COLOR_WHITE, true };
    display_print_styled(message, prompt_style);
    display_refresh();

    while (pos < 255) {
        DisplayKey key = display_wait_key();

        if (key == DISPLAY_KEY_ENTER) {
            break;
        } else if (key == DISPLAY_KEY_ESCAPE) {
            gui->needs_redraw = true;
            return NULL;
        } else if (key == DISPLAY_KEY_BACKSPACE) {
            if (pos > 0) {
                pos--;
                input[pos] = '\0';
                // Redraw full UI with updated input
                ide_gui_draw(gui);
                display_set_cursor(0, gui->screen_rows - 1);
                display_print_styled(message, prompt_style);
                display_print(input);
                display_refresh();
            }
        } else if (key == DISPLAY_KEY_CHAR) {
            char ch = display_get_last_char();
            if (ch >= 32 && ch < 127 && pos < 255) {
                input[pos++] = ch;
                // Redraw full UI with updated input
                ide_gui_draw(gui);
                display_set_cursor(0, gui->screen_rows - 1);
                display_print_styled(message, prompt_style);
                display_print(input);
                display_refresh();
            }
        }
    }

    gui->needs_redraw = true;
    return strdup(input);
}

// -----------------------------------------------------------------------------
// B.7 Template Insertion
// -----------------------------------------------------------------------------

// IDETemplateType enum defined in B.0 Forward Declarations

// Block content templates
static const char* BLOCK_METADATA =
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "// METADATA [METADATA]\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "//\n"
    "// Key: [PROJECT-COMPONENT-TYPE]\n"
    "// Title: [Title]\n"
    "// Type: [Type]\n"
    "// Version: a-01.00\n"
    "//\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n";

static const char* BLOCK_SETUP =
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "// SETUP [SETUP]\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "\n"
    "#include <stdio.h>\n"
    "\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n";

static const char* BLOCK_HEADER =
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "// HEADER [HEADER]\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "//\n"
    "// # [Title]\n"
    "//\n"
    "// [One-paragraph description]\n"
    "//\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n";

static const char* BLOCK_CONTEXT =
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "// CONTEXT [CONTEXT]\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "//\n"
    "// ## Prerequisites\n"
    "// ## Scope\n"
    "// ## Key Terms\n"
    "//\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n";

static const char* BLOCK_BODY =
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "// BODY [BODY]\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "\n"
    "// Implementation goes here\n"
    "\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n";

static const char* BLOCK_CLOSING =
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "// CLOSING [CLOSING]\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n"
    "//\n"
    "// \"In the beginning God created the heavens and the earth.\" — Genesis 1:1\n"
    "//\n"
    "// ═══════════════════════════════════════════════════════════════════════════════\n";

// Insert a single block at cursor position
static void ide_gui_insert_block(IDEGUI* gui, const char* block_name) {
    if (!gui || !gui->buffer) return;

    const char* content = NULL;
    if (strcmp(block_name, "METADATA") == 0) content = BLOCK_METADATA;
    else if (strcmp(block_name, "SETUP") == 0) content = BLOCK_SETUP;
    else if (strcmp(block_name, "HEADER") == 0) content = BLOCK_HEADER;
    else if (strcmp(block_name, "CONTEXT") == 0) content = BLOCK_CONTEXT;
    else if (strcmp(block_name, "BODY") == 0) content = BLOCK_BODY;
    else if (strcmp(block_name, "CLOSING") == 0) content = BLOCK_CLOSING;

    if (content) {
        // Insert each line of the block
        const char* line_start = content;
        while (*line_start) {
            const char* line_end = strchr(line_start, '\n');
            if (!line_end) line_end = line_start + strlen(line_start);

            // Insert line
            size_t len = line_end - line_start;
            if (len > 0) {
                char* line = malloc(len + 1);
                strncpy(line, line_start, len);
                line[len] = '\0';
                ide_insert(gui->buffer, line);
                free(line);
            }
            ide_insert_char(gui->buffer, '\n');

            if (*line_end) line_start = line_end + 1;
            else break;
        }

        gui->buffer->modified = true;
        ide_gui_set_status(gui, "Inserted %s block", block_name);
    }
    gui->needs_redraw = true;
}

// Insert a full template structure
static void ide_gui_insert_template(IDEGUI* gui, IDETemplateType type) {
    if (!gui || !gui->buffer) return;

    switch (type) {
        case TEMPLATE_3BLOCK:
            ide_gui_insert_block(gui, "METADATA");
            ide_gui_insert_block(gui, "BODY");  // CONTENT for 3-block
            ide_gui_insert_block(gui, "CLOSING");
            ide_gui_set_status(gui, "Inserted 3-block template (Data)");
            break;

        case TEMPLATE_4BLOCK:
            ide_gui_insert_block(gui, "METADATA");
            ide_gui_insert_block(gui, "SETUP");
            ide_gui_insert_block(gui, "BODY");
            ide_gui_insert_block(gui, "CLOSING");
            ide_gui_set_status(gui, "Inserted 4-block template (Code)");
            break;

        case TEMPLATE_5BLOCK:
            ide_gui_insert_block(gui, "METADATA");
            ide_gui_insert_block(gui, "HEADER");
            ide_gui_insert_block(gui, "CONTEXT");
            ide_gui_insert_block(gui, "BODY");  // CONTENT for 5-block
            ide_gui_insert_block(gui, "CLOSING");  // FOOTER for 5-block
            ide_gui_set_status(gui, "Inserted 5-block template (Documentation)");
            break;
    }
}

// -----------------------------------------------------------------------------
// B.8 Main Loop
// -----------------------------------------------------------------------------

void ide_gui_run(IDEGUI* gui) {
    if (!gui) return;

    // Initial validation
    if (gui->buffer) {
        ide_validate_all(gui->buffer, gui->diags);
        if (gui->diags->count > 0) {
            ide_gui_set_status(gui, "%u issue(s) found.",
                              gui->diags->count);
        }
    }

    while (gui->running && display_is_running() && signals_is_running()) {
        if (gui->needs_redraw) {
            ide_gui_draw(gui);
        }

        DisplayKey key = display_wait_key();
        ide_gui_handle_key(gui, key);
    }
}

// -----------------------------------------------------------------------------
// B.8 CLI Panel - CPI-SI Communication
// -----------------------------------------------------------------------------

#include "omni_cpisi.h"
#include "omni_patterns.h"
#include "omni_dar.h"  // For dar_hebrew_state_name

// Global CPI-SI cube for IDE (initialized on first CLI use)
static CPISICube* g_ide_cube = NULL;

void ide_gui_cli_toggle(IDEGUI* gui) {
    if (!gui) return;

    gui->cli.visible = !gui->cli.visible;

    if (gui->cli.visible) {
        // Initialize CLI panel
        gui->cli.height = CLI_OUTPUT_LINES + 2;  // Output + input + border
        gui->cli.input_pos = 0;
        gui->cli.input_len = 0;
        gui->cli.input[0] = '\0';
        gui->focus = GUI_FOCUS_CLI;

        // Initialize CPI-SI cube if needed
        if (!g_ide_cube) {
            g_ide_cube = cpisi_cube_create();
            ide_gui_cli_output(gui, "CPI-SI initialized. Type 'help' for commands.");
        }

        // Adjust edit area
        gui->edit_rows -= gui->cli.height;
    } else {
        gui->focus = GUI_FOCUS_EDITOR;
        gui->edit_rows += gui->cli.height;
    }

    gui->needs_redraw = true;
}

void ide_gui_cli_output(IDEGUI* gui, const char* format, ...) {
    if (!gui) return;

    // Shift existing output up
    for (int i = 0; i < CLI_OUTPUT_LINES - 1; i++) {
        strncpy(gui->cli.output[i], gui->cli.output[i + 1], 255);
        gui->cli.output[i][255] = '\0';
    }

    // Add new line at bottom
    va_list args;
    va_start(args, format);
    vsnprintf(gui->cli.output[CLI_OUTPUT_LINES - 1], 256, format, args);
    va_end(args);

    gui->needs_redraw = true;
}

void ide_gui_cli_execute(IDEGUI* gui) {
    if (!gui || gui->cli.input_len == 0) return;

    char* cmd = gui->cli.input;

    // Add to history
    if (gui->cli.history_count < CLI_HISTORY_MAX) {
        int idx = gui->cli.history_count++;
        snprintf(gui->cli.history[idx], CLI_INPUT_MAX, "%s", cmd);
    }
    gui->cli.history_pos = gui->cli.history_count;

    // Echo command
    ide_gui_cli_output(gui, "> %s", cmd);

    // Built-in commands
    if (strcmp(cmd, "help") == 0) {
        ide_gui_cli_output(gui, "Commands: help, status, center, respond <text>, clear, exit");
    } else if (strcmp(cmd, "status") == 0) {
        if (g_ide_cube) {
            DARHebrewState state = cpisi_hebrew_state(g_ide_cube);
            const char* name = dar_hebrew_state_name(state);
            ide_gui_cli_output(gui, "State: %s", name);
        }
    } else if (strcmp(cmd, "center") == 0) {
        if (g_ide_cube) {
            cpisi_center(g_ide_cube);
            ide_gui_cli_output(gui, "Centered. State: yashar");
        }
    } else if (strcmp(cmd, "clear") == 0) {
        for (int i = 0; i < CLI_OUTPUT_LINES; i++) {
            gui->cli.output[i][0] = '\0';
        }
    } else if (strcmp(cmd, "exit") == 0) {
        ide_gui_cli_toggle(gui);
    } else if (strncmp(cmd, "respond ", 8) == 0) {
        if (g_ide_cube) {
            const char* input = cmd + 8;
            FilledPattern fp = pattern_respond(g_ide_cube, input, NULL);
            ide_gui_cli_output(gui, "%s", fp.output);
        }
    } else {
        // Try as pattern response
        if (g_ide_cube) {
            FilledPattern fp = pattern_respond(g_ide_cube, cmd, NULL);
            ide_gui_cli_output(gui, "%s", fp.output);
        } else {
            ide_gui_cli_output(gui, "Unknown command: %s", cmd);
        }
    }

    // Clear input
    gui->cli.input[0] = '\0';
    gui->cli.input_len = 0;
    gui->cli.input_pos = 0;
    gui->needs_redraw = true;
}

void ide_gui_cli_handle_key(IDEGUI* gui, DisplayKey key) {
    if (!gui) return;

    switch (key) {
        case DISPLAY_KEY_ENTER:
            ide_gui_cli_execute(gui);
            break;

        case DISPLAY_KEY_ESCAPE:
            ide_gui_cli_toggle(gui);
            break;

        case DISPLAY_KEY_LEFT:
            if (gui->cli.input_pos > 0) {
                gui->cli.input_pos--;
                gui->needs_redraw = true;
            }
            break;

        case DISPLAY_KEY_RIGHT:
            if (gui->cli.input_pos < gui->cli.input_len) {
                gui->cli.input_pos++;
                gui->needs_redraw = true;
            }
            break;

        case DISPLAY_KEY_UP:
            // Navigate history up
            if (gui->cli.history_pos > 0) {
                gui->cli.history_pos--;
                strncpy(gui->cli.input, gui->cli.history[gui->cli.history_pos], CLI_INPUT_MAX - 1);
                gui->cli.input_len = strlen(gui->cli.input);
                gui->cli.input_pos = gui->cli.input_len;
                gui->needs_redraw = true;
            }
            break;

        case DISPLAY_KEY_DOWN:
            // Navigate history down
            if (gui->cli.history_pos < gui->cli.history_count - 1) {
                gui->cli.history_pos++;
                strncpy(gui->cli.input, gui->cli.history[gui->cli.history_pos], CLI_INPUT_MAX - 1);
                gui->cli.input_len = strlen(gui->cli.input);
                gui->cli.input_pos = gui->cli.input_len;
                gui->needs_redraw = true;
            } else if (gui->cli.history_pos == gui->cli.history_count - 1) {
                gui->cli.history_pos++;
                gui->cli.input[0] = '\0';
                gui->cli.input_len = 0;
                gui->cli.input_pos = 0;
                gui->needs_redraw = true;
            }
            break;

        case DISPLAY_KEY_CHAR: {
            char ch = display_get_last_char();

            // Handle backspace (ASCII 127 or 8)
            if (ch == 127 || ch == 8) {
                if (gui->cli.input_pos > 0) {
                    memmove(&gui->cli.input[gui->cli.input_pos - 1],
                            &gui->cli.input[gui->cli.input_pos],
                            gui->cli.input_len - gui->cli.input_pos + 1);
                    gui->cli.input_pos--;
                    gui->cli.input_len--;
                    gui->needs_redraw = true;
                }
            } else if (ch >= 32 && ch < 127 && gui->cli.input_len < CLI_INPUT_MAX - 1) {
                // Insert printable character at cursor
                memmove(&gui->cli.input[gui->cli.input_pos + 1],
                        &gui->cli.input[gui->cli.input_pos],
                        gui->cli.input_len - gui->cli.input_pos + 1);
                gui->cli.input[gui->cli.input_pos] = ch;
                gui->cli.input_pos++;
                gui->cli.input_len++;
                gui->needs_redraw = true;
            }
            break;
        }

        default:
            break;
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Kingdom Technology excludes no one."
// The GUI provides full editing with modern accessibility.

// =============================================================================
// END CLOSING
// =============================================================================
