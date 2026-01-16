// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide-panel-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_ide_panel.c — OmniCode IDE Panel Implementation
// File browser, block outline, validation panels

#define _GNU_SOURCE

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-panel-impl
// Grounding: Genesis 1:1 - Structure enables navigation

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_ide_panel.h"
#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// Panel styles using display abstraction
//
// These replace ncurses COLOR_PAIR macros with DisplayStyle structs.
// The display layer handles mode-specific styling (ANSI, ncurses, OpenGL).

static const DisplayStyle STYLE_PANEL_ITEM = {
    .fg = DISPLAY_COLOR_WHITE,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

static const DisplayStyle STYLE_PANEL_SELECT = {
    .fg = DISPLAY_COLOR_BLACK,
    .bg = DISPLAY_COLOR_WHITE,
    .bold = false
};

static const DisplayStyle STYLE_ZONE_VOID = {
    .fg = DISPLAY_COLOR_RED,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

static const DisplayStyle STYLE_ZONE_WORD = {
    .fg = DISPLAY_COLOR_GREEN,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

static const DisplayStyle STYLE_ZONE_TOV = {
    .fg = DISPLAY_COLOR_BLUE,
    .bg = DISPLAY_COLOR_DEFAULT,
    .bold = false
};

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Panel Lifecycle
// -----------------------------------------------------------------------------

IDEPanel* ide_panel_create(IDEPanelType type, IDEPanelPosition pos, int32_t size) {
    IDEPanel* panel = calloc(1, sizeof(IDEPanel));
    if (!panel) return NULL;

    panel->type = type;
    panel->position = pos;
    panel->visible = true;
    panel->focused = false;

    if (pos == PANEL_POS_BOTTOM) {
        panel->height = size;
        panel->width = 0;  // Full width
    } else {
        panel->width = size;
        panel->height = 0;  // Full height
    }

    return panel;
}

void ide_panel_free(IDEPanel* panel) {
    if (!panel) return;
    free(panel);
}

IDELayout* ide_layout_create_default(void) {
    IDELayout* layout = calloc(1, sizeof(IDELayout));
    if (!layout) return NULL;

    // Left panel: File browser (top) + Block outline (bottom)
    layout->panels[0].type = PANEL_FILE_BROWSER;
    layout->panels[0].position = PANEL_POS_LEFT;
    layout->panels[0].width = 24;
    layout->panels[0].visible = true;

    layout->panels[1].type = PANEL_BLOCK_OUTLINE;
    layout->panels[1].position = PANEL_POS_LEFT;
    layout->panels[1].width = 24;
    layout->panels[1].visible = true;

    layout->panel_count = 2;
    layout->left_width = 24;

    return layout;
}

void ide_layout_free(IDELayout* layout) {
    if (!layout) return;
    free(layout);
}

// -----------------------------------------------------------------------------
// B.2 File Browser
// -----------------------------------------------------------------------------

static IDEFileItemType get_zone_type(const char* name) {
    if (strcmp(name, "void") == 0) return FILE_ITEM_ZONE_VOID;
    if (strcmp(name, "word") == 0) return FILE_ITEM_ZONE_WORD;
    if (strcmp(name, "tov") == 0) return FILE_ITEM_ZONE_TOV;
    return FILE_ITEM_DIR;
}

static bool is_omni_file(const char* name) {
    size_t len = strlen(name);
    if (len < 5) return false;
    return strcmp(name + len - 5, ".omni") == 0;
}

static int compare_items(const void* a, const void* b) {
    const IDEFileItem* ia = (const IDEFileItem*)a;
    const IDEFileItem* ib = (const IDEFileItem*)b;

    // Directories first
    bool a_dir = (ia->type != FILE_ITEM_FILE);
    bool b_dir = (ib->type != FILE_ITEM_FILE);
    if (a_dir != b_dir) return b_dir - a_dir;

    // Then alphabetically
    return strcmp(ia->name, ib->name);
}

static void add_directory_contents(IDEFileBrowser* browser, const char* path, int depth) {
    if (browser->item_count >= FILE_BROWSER_MAX_ITEMS - 1) return;

    // Copy path to local buffer (fixes restrict warning - path may be browser->root_path)
    char local_path[FILE_BROWSER_MAX_PATH];
    size_t path_len = strlen(path);
    if (path_len >= sizeof(local_path)) {
        path_len = sizeof(local_path) - 1;
    }
    memcpy(local_path, path, path_len);
    local_path[path_len] = '\0';

    DIR* dir = opendir(local_path);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (browser->item_count >= FILE_BROWSER_MAX_ITEMS - 1) break;

        // Skip hidden files and . / ..
        if (entry->d_name[0] == '.') continue;

        // Check if combined path would fit (path + "/" + name + null)
        size_t name_len = strlen(entry->d_name);
        if (path_len + 1 + name_len >= FILE_BROWSER_MAX_PATH) {
            // Path too long, skip this entry
            continue;
        }

        IDEFileItem* item = &browser->items[browser->item_count];

        // Copy name (truncate if needed, though unlikely)
        if (name_len >= sizeof(item->name)) {
            name_len = sizeof(item->name) - 1;
        }
        memcpy(item->name, entry->d_name, name_len);
        item->name[name_len] = '\0';

        // Build full path - we already checked it fits
        memcpy(item->path, local_path, path_len);
        item->path[path_len] = '/';
        memcpy(item->path + path_len + 1, entry->d_name, strlen(entry->d_name) + 1);

        item->depth = depth;
        item->expanded = false;

        struct stat st;
        if (stat(item->path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                item->type = get_zone_type(entry->d_name);
                item->is_omni = false;
            } else {
                item->type = FILE_ITEM_FILE;
                item->is_omni = is_omni_file(entry->d_name);
            }
            browser->item_count++;
        }
    }

    closedir(dir);
}

void ide_file_browser_init(IDEFileBrowser* browser, const char* bereshit_root) {
    if (!browser || !bereshit_root) return;

    memset(browser, 0, sizeof(IDEFileBrowser));
    strncpy(browser->root_path, bereshit_root, sizeof(browser->root_path) - 1);

    ide_file_browser_refresh(browser);
}

void ide_file_browser_refresh(IDEFileBrowser* browser) {
    if (!browser) return;

    browser->item_count = 0;
    add_directory_contents(browser, browser->root_path, 0);

    // Sort items
    if (browser->item_count > 0) {
        qsort(browser->items, browser->item_count, sizeof(IDEFileItem), compare_items);
    }
}

void ide_file_browser_up(IDEFileBrowser* browser) {
    if (!browser || browser->item_count == 0) return;
    if (browser->selected > 0) {
        browser->selected--;
        if (browser->selected < browser->scroll) {
            browser->scroll = browser->selected;
        }
    }
}

void ide_file_browser_down(IDEFileBrowser* browser) {
    if (!browser || browser->item_count == 0) return;
    if (browser->selected < browser->item_count - 1) {
        browser->selected++;
    }
}

void ide_file_browser_toggle(IDEFileBrowser* browser) {
    if (!browser || browser->item_count == 0) return;

    IDEFileItem* item = &browser->items[browser->selected];
    if (item->type == FILE_ITEM_FILE) return;  // Can't toggle files

    item->expanded = !item->expanded;

    // TODO: Rebuild item list with expanded directories
    // For now, just toggle the flag
}

const char* ide_file_browser_get_selected_path(IDEFileBrowser* browser) {
    if (!browser || browser->item_count == 0) return NULL;

    IDEFileItem* item = &browser->items[browser->selected];
    if (item->type == FILE_ITEM_FILE) {
        return item->path;
    }
    return NULL;  // Directory selected
}

// -----------------------------------------------------------------------------
// B.3 Block Outline
// -----------------------------------------------------------------------------

static const char* block_type_name(IDEBlockType type) {
    switch (type) {
        case BLOCK_3_METADATA:
        case BLOCK_4_METADATA:
        case BLOCK_5_METADATA: return "METADATA";
        case BLOCK_4_SETUP: return "SETUP";
        case BLOCK_5_HEADER: return "HEADER";
        case BLOCK_5_CONTEXT: return "CONTEXT";
        case BLOCK_4_BODY: return "BODY";
        case BLOCK_3_CONTENT:
        case BLOCK_5_CONTENT: return "CONTENT";
        case BLOCK_3_CLOSING:
        case BLOCK_4_CLOSING: return "CLOSING";
        case BLOCK_5_FOOTER: return "FOOTER";
        default: return "UNKNOWN";
    }
}

void ide_block_outline_update(IDEBlockOutline* outline, IDEBuffer* buffer) {
    if (!outline || !buffer) return;

    memset(outline, 0, sizeof(IDEBlockOutline));
    outline->structure = ide_get_structure(buffer);

    IDEBlockType current_block = BLOCK_NONE;

    for (uint32_t i = 0; i < buffer->line_count && outline->item_count < BLOCK_OUTLINE_MAX_ITEMS; i++) {
        IDEBlockType line_block = buffer->lines[i].block;

        if (line_block != current_block && line_block != BLOCK_NONE) {
            // End previous block
            if (current_block != BLOCK_NONE && outline->item_count > 0) {
                outline->items[outline->item_count - 1].end_line = i - 1;
                outline->items[outline->item_count - 1].line_count =
                    i - outline->items[outline->item_count - 1].start_line;
            }

            // Start new block
            IDEBlockItem* item = &outline->items[outline->item_count];
            strncpy(item->label, block_type_name(line_block), sizeof(item->label) - 1);
            item->type = line_block;
            item->start_line = i;
            outline->item_count++;

            current_block = line_block;
        }
    }

    // Close last block
    if (outline->item_count > 0) {
        outline->items[outline->item_count - 1].end_line = buffer->line_count - 1;
        outline->items[outline->item_count - 1].line_count =
            buffer->line_count - outline->items[outline->item_count - 1].start_line;
    }
}

void ide_block_outline_up(IDEBlockOutline* outline) {
    if (!outline || outline->item_count == 0) return;
    if (outline->selected > 0) {
        outline->selected--;
    }
}

void ide_block_outline_down(IDEBlockOutline* outline) {
    if (!outline || outline->item_count == 0) return;
    if (outline->selected < outline->item_count - 1) {
        outline->selected++;
    }
}

uint32_t ide_block_outline_get_line(IDEBlockOutline* outline) {
    if (!outline || outline->item_count == 0) return 0;
    return outline->items[outline->selected].start_line;
}

// -----------------------------------------------------------------------------
// B.4 Template Info
// -----------------------------------------------------------------------------

void ide_template_info_update(IDETemplateInfo* info, IDEBuffer* buffer) {
    if (!info || !buffer) return;

    memset(info, 0, sizeof(IDETemplateInfo));
    info->structure = ide_get_structure(buffer);
    info->valid = (info->structure != STRUCTURE_UNKNOWN);

    // Parse pragma lines for meta info
    for (uint32_t i = 0; i < buffer->line_count && i < 10; i++) {
        const char* line = buffer->lines[i].text;

        // Store first pragma line
        if (i == 0 && strstr(line, "#!omni")) {
            strncpy(info->pragma_line, line, sizeof(info->pragma_line) - 1);
        }

        // Find meta.key
        const char* key = strstr(line, "meta.key");
        if (key) {
            const char* eq = strchr(key, '=');
            if (eq) {
                eq++;
                while (*eq == ' ') eq++;
                strncpy(info->key, eq, sizeof(info->key) - 1);
                // Trim trailing whitespace
                char* end = info->key + strlen(info->key) - 1;
                while (end > info->key && (*end == ' ' || *end == '\n')) *end-- = '\0';
            }
        }

        // Find meta.from (derives_from)
        const char* from = strstr(line, "meta.from");
        if (from) {
            const char* eq = strchr(from, '=');
            if (eq) {
                eq++;
                while (*eq == ' ') eq++;
                strncpy(info->derives_from, eq, sizeof(info->derives_from) - 1);
                char* end = info->derives_from + strlen(info->derives_from) - 1;
                while (end > info->derives_from && (*end == ' ' || *end == '\n')) *end-- = '\0';
            }
        }
    }
}

// -----------------------------------------------------------------------------
// B.5 Panel Drawing
// -----------------------------------------------------------------------------

static void draw_box(int32_t x, int32_t y, int32_t w, int32_t h, const char* title) {
    // Use display abstraction for box with title
    display_draw_box_titled(x, y, w, h, title);
}

void ide_panel_draw_file_browser(IDEFileBrowser* browser, int32_t x, int32_t y, int32_t w, int32_t h, bool focused) {
    if (!browser) return;

    draw_box(x, y, w, h, "Files");

    int32_t content_h = h - 2;
    int32_t content_w = w - 2;

    for (int32_t i = 0; i < content_h && browser->scroll + i < browser->item_count; i++) {
        uint32_t idx = browser->scroll + i;
        IDEFileItem* item = &browser->items[idx];

        int32_t row = y + 1 + i;
        int32_t col = x + 1;

        // Selection highlight
        if (idx == browser->selected && focused) {
            display_set_style(STYLE_PANEL_SELECT);
            for (int32_t j = 0; j < content_w; j++) display_putchar_at(col + j, row, ' ');
        }

        // Indentation
        for (int depth = 0; depth < item->depth && col < x + w - 10; depth++) {
            display_print_at(col, row, "  ");
            col += 2;
        }

        // Icon and name
        const char* icon;
        DisplayStyle item_style = STYLE_PANEL_ITEM;

        switch (item->type) {
            case FILE_ITEM_ZONE_VOID:
                icon = "○ ";  // Empty circle - entry gate
                item_style = STYLE_ZONE_VOID;
                break;
            case FILE_ITEM_ZONE_WORD:
                icon = "◆ ";  // Diamond - processing
                item_style = STYLE_ZONE_WORD;
                break;
            case FILE_ITEM_ZONE_TOV:
                icon = "● ";  // Filled circle - exit gate
                item_style = STYLE_ZONE_TOV;
                break;
            case FILE_ITEM_DIR:
                icon = item->expanded ? "▼ " : "▶ ";
                break;
            case FILE_ITEM_FILE:
                icon = item->is_omni ? "◇ " : "  ";
                break;
            default:
                icon = "  ";
        }

        if (idx != browser->selected || !focused) {
            display_set_style(item_style);
        }

        display_printf_at(col, row, "%s", icon);
        col += 2;

        // Truncate name if needed
        char truncated[32];
        int max_name = content_w - (col - x - 1);
        if ((int)strlen(item->name) > max_name - 1) {
            strncpy(truncated, item->name, max_name - 4);
            truncated[max_name - 4] = '\0';
            strcat(truncated, "...");
            display_printf_at(col, row, "%s", truncated);
        } else {
            display_printf_at(col, row, "%s", item->name);
        }

        display_reset_style();
    }
}

void ide_panel_draw_block_outline(IDEBlockOutline* outline, int32_t x, int32_t y, int32_t w, int32_t h, bool focused) {
    if (!outline) return;

    // Title includes structure type
    char title[32];
    switch (outline->structure) {
        case STRUCTURE_3_BLOCK: snprintf(title, sizeof(title), "3-Block"); break;
        case STRUCTURE_4_BLOCK: snprintf(title, sizeof(title), "4-Block"); break;
        case STRUCTURE_5_BLOCK: snprintf(title, sizeof(title), "5-Block"); break;
        default: snprintf(title, sizeof(title), "Blocks"); break;
    }

    draw_box(x, y, w, h, title);

    int32_t content_h = h - 2;
    int32_t content_w = w - 2;

    for (uint32_t i = 0; i < outline->item_count && (int32_t)i < content_h; i++) {
        IDEBlockItem* item = &outline->items[i];
        int32_t row = y + 1 + i;

        // Selection highlight
        if (i == outline->selected && focused) {
            display_set_style(STYLE_PANEL_SELECT);
            for (int32_t j = 0; j < content_w; j++) display_putchar_at(x + 1 + j, row, ' ');
        }

        // Tree connector
        const char* connector = (i == outline->item_count - 1) ? "└─" : "├─";
        display_printf_at(x + 1, row, "%s", connector);

        // Block name and line count
        display_printf_at(x + 4, row, "%s", item->label);

        // Line count on right
        char lines[16];
        snprintf(lines, sizeof(lines), "%u", item->line_count);
        int32_t lines_col = x + w - 2 - strlen(lines);
        if (lines_col > x + 4 + (int32_t)strlen(item->label)) {
            display_printf_at(lines_col, row, "%s", lines);
        }

        if (i == outline->selected && focused) {
            display_reset_style();
        }
    }
}

void ide_panel_draw_validation(IDEValidationPanel* validation, int32_t x, int32_t y, int32_t w, int32_t h) {
    if (!validation || !validation->diags) return;

    char title[32];
    if (validation->diags->count == 0) {
        snprintf(title, sizeof(title), "✓ Valid");
    } else {
        snprintf(title, sizeof(title), "Issues (%u)", validation->diags->count);
    }

    draw_box(x, y, w, h, title);

    int32_t content_h = h - 2;

    if (validation->diags->count == 0) {
        display_set_style(STYLE_ZONE_TOV);
        display_printf_at(x + 2, y + 1, "No issues found");
        display_reset_style();
        return;
    }

    for (uint32_t i = 0; i < validation->diags->count && (int32_t)i < content_h; i++) {
        IDEDiagnostic* d = &validation->diags->items[i];
        int32_t row = y + 1 + i;

        // Error/warning indicator
        if (d->severity == DIAG_ERROR) {
            display_set_style(STYLE_ZONE_VOID);
            display_printf_at(x + 1, row, "✗");
            display_reset_style();
        } else {
            display_set_style(STYLE_ZONE_WORD);
            display_printf_at(x + 1, row, "!");
            display_reset_style();
        }

        // Line number and message
        display_printf_at(x + 3, row, "L%u: %.*s", d->line + 1, w - 10, d->message);
    }
}

void ide_panel_draw_template_info(IDETemplateInfo* info, int32_t x, int32_t y, int32_t w, int32_t h) {
    if (!info) return;

    draw_box(x, y, w, h, "Template");

    int32_t row = y + 1;
    int32_t content_w = w - 4;

    // Key
    if (strlen(info->key) > 0) {
        display_printf_at(x + 2, row++, "Key:");
        display_printf_at(x + 2, row++, " %.*s", content_w, info->key);
    }

    // Derives from
    if (strlen(info->derives_from) > 0) {
        display_printf_at(x + 2, row++, "From:");
        display_printf_at(x + 2, row++, " %.*s", content_w, info->derives_from);
    }

    // Structure
    const char* structure = "Unknown";
    switch (info->structure) {
        case STRUCTURE_3_BLOCK: structure = "3-Block (Data)"; break;
        case STRUCTURE_4_BLOCK: structure = "4-Block (Code)"; break;
        case STRUCTURE_5_BLOCK: structure = "5-Block (Doc)"; break;
        default: break;
    }
    display_printf_at(x + 2, row++, "Type: %s", structure);

    // Valid indicator
    if (info->valid) {
        display_set_style(STYLE_ZONE_TOV);
        display_printf_at(x + 2, row, "✓ Valid");
        display_reset_style();
    } else {
        display_set_style(STYLE_ZONE_VOID);
        display_printf_at(x + 2, row, "✗ Invalid");
        display_reset_style();
    }
}

void ide_panel_draw(IDEPanel* panel, int32_t x, int32_t y, int32_t w, int32_t h) {
    if (!panel || !panel->visible) return;

    switch (panel->type) {
        case PANEL_FILE_BROWSER:
            ide_panel_draw_file_browser(&panel->data.file_browser, x, y, w, h, panel->focused);
            break;
        case PANEL_BLOCK_OUTLINE:
            ide_panel_draw_block_outline(&panel->data.block_outline, x, y, w, h, panel->focused);
            break;
        case PANEL_VALIDATION:
            ide_panel_draw_validation(&panel->data.validation, x, y, w, h);
            break;
        case PANEL_TEMPLATE_INFO:
            ide_panel_draw_template_info(&panel->data.template_info, x, y, w, h);
            break;
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

// "In the beginning God created the heaven and the earth." — Genesis 1:1
// Panels reveal structure - enabling navigation through ordered creation.

// =============================================================================
// END CLOSING
// =============================================================================
