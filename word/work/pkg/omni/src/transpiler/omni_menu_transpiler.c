// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-menu-transpiler-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_menu_transpiler.c — OmniCode Menu DSL to JSONC Transpiler
// Implementation of menu parsing and JSONC output.

// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key: B-word-work-pkg-omni-menu-transpiler-impl
// Title: OmniCode Menu DSL to JSONC Transpiler Implementation
// Type: Source
// Role: Parse OmniCode menu files, output JSONC for cornerstone menu system
//
// Scripture: Proverbs 4:26 — Ponder the path of thy feet
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_menu_transpiler.h"
#include "omni_lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Internal Helpers
// -----------------------------------------------------------------------------

static void safe_strcpy(char* dst, size_t dst_size, const char* src) {
    if (!dst || dst_size == 0) return;
    if (!src) {
        dst[0] = '\0';
        return;
    }
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dst_size - 1) ? src_len : dst_size - 1;
    memcpy(dst, src, copy_len);
    dst[copy_len] = '\0';
}

// Skip whitespace in string
static const char* skip_ws(const char* s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

// Check if line starts with pattern (case-insensitive prefix match)
static bool starts_with(const char* line, const char* prefix) {
    while (*prefix) {
        if (tolower((unsigned char)*line) != tolower((unsigned char)*prefix)) {
            return false;
        }
        line++;
        prefix++;
    }
    return true;
}

// Extract quoted string from line
static bool extract_quoted(const char* line, char* out, size_t out_size) {
    const char* start = strchr(line, '"');
    if (!start) return false;
    start++;

    const char* end = strchr(start, '"');
    if (!end) return false;

    size_t len = end - start;
    if (len >= out_size) len = out_size - 1;
    memcpy(out, start, len);
    out[len] = '\0';
    return true;
}

// Extract array of strings ["a", "b", "c"] - for KEY_MAX (16) sized strings
static int extract_keys(const char* line, char arr[][MENU_KEY_MAX], int max_count) {
    const char* p = strchr(line, '[');
    if (!p) return 0;
    p++;

    int count = 0;
    while (*p && *p != ']' && count < max_count) {
        while (*p && *p != '"' && *p != ']') p++;
        if (*p != '"') break;
        p++;

        char* dst = arr[count];
        int i = 0;
        while (*p && *p != '"' && i < MENU_KEY_MAX - 1) {
            dst[i++] = *p++;
        }
        dst[i] = '\0';
        count++;

        if (*p == '"') p++;
        while (*p && (*p == ',' || isspace((unsigned char)*p))) p++;
    }
    return count;
}

// Extract array of strings - for ID_MAX (64) sized strings
static int extract_ids(const char* line, char arr[][MENU_ID_MAX], int max_count) {
    const char* p = strchr(line, '[');
    if (!p) return 0;
    p++;

    int count = 0;
    while (*p && *p != ']' && count < max_count) {
        while (*p && *p != '"' && *p != ']') p++;
        if (*p != '"') break;
        p++;

        char* dst = arr[count];
        int i = 0;
        while (*p && *p != '"' && i < MENU_ID_MAX - 1) {
            dst[i++] = *p++;
        }
        dst[i] = '\0';
        count++;

        if (*p == '"') p++;
        while (*p && (*p == ',' || isspace((unsigned char)*p))) p++;
    }
    return count;
}

// Extract array of strings - for LABEL_MAX (128) sized strings
static int extract_labels(const char* line, char arr[][MENU_LABEL_MAX], int max_count) {
    const char* p = strchr(line, '[');
    if (!p) return 0;
    p++;

    int count = 0;
    while (*p && *p != ']' && count < max_count) {
        while (*p && *p != '"' && *p != ']') p++;
        if (*p != '"') break;
        p++;

        char* dst = arr[count];
        int i = 0;
        while (*p && *p != '"' && i < MENU_LABEL_MAX - 1) {
            dst[i++] = *p++;
        }
        dst[i] = '\0';
        count++;

        if (*p == '"') p++;
        while (*p && (*p == ',' || isspace((unsigned char)*p))) p++;
    }
    return count;
}

// Get indentation level (number of leading spaces / 4)
static int get_indent(const char* line) {
    int spaces = 0;
    while (*line == ' ') {
        spaces++;
        line++;
    }
    return spaces / 4;
}

// -----------------------------------------------------------------------------
// B.2 Lifecycle
// -----------------------------------------------------------------------------

ParsedMenuFile* menu_parsed_create(void) {
    ParsedMenuFile* pmf = calloc(1, sizeof(ParsedMenuFile));
    if (!pmf) return NULL;

    // Set defaults
    safe_strcpy(pmf->on_menu_not_found, sizeof(pmf->on_menu_not_found), "use_engine_default");
    safe_strcpy(pmf->on_render_failure, sizeof(pmf->on_render_failure), "degrade_to_cli");
    pmf->health_penalty = 10;

    return pmf;
}

void menu_parsed_destroy(ParsedMenuFile* pmf) {
    if (pmf) {
        free(pmf);
    }
}

// -----------------------------------------------------------------------------
// B.3 Parser
// -----------------------------------------------------------------------------

// Parse state machine
typedef enum {
    PARSE_STATE_TOP,
    PARSE_STATE_MENU,
    PARSE_STATE_ITEM,
    PARSE_STATE_NAVIGATION,
    PARSE_STATE_BINDINGS,
    PARSE_STATE_DISPLAY,
    PARSE_STATE_DISPLAY_CLI,
    PARSE_STATE_DISPLAY_TUI,
    PARSE_STATE_DISPLAY_GUI,
    PARSE_STATE_HEBREW_RULES,
    PARSE_STATE_HEBREW_RULE,
    PARSE_STATE_TRIPWIRE,
} ParseState;

MenuTranspileError menu_parse_string(const char* source, ParsedMenuFile* out) {
    if (!source || !out) return MENU_TRANSPILE_ERR_NULL;

    ParseState state = PARSE_STATE_TOP;
    ParsedMenu* current_menu = NULL;
    ParsedMenuItem* current_item = NULL;
    ParsedHebrewRule* current_rule = NULL;
    int current_indent = 0;

    // Process line by line
    const char* line_start = source;
    while (*line_start) {
        // Find end of line
        const char* line_end = line_start;
        while (*line_end && *line_end != '\n') line_end++;

        // Copy line to buffer
        size_t line_len = line_end - line_start;
        char line[1024];
        if (line_len >= sizeof(line)) line_len = sizeof(line) - 1;
        memcpy(line, line_start, line_len);
        line[line_len] = '\0';

        // Skip empty lines and comments
        const char* trimmed = skip_ws(line);
        if (*trimmed == '\0' || (*trimmed == '/' && *(trimmed+1) == '/')) {
            line_start = (*line_end) ? line_end + 1 : line_end;
            continue;
        }

        // Get indentation
        int indent = get_indent(line);

        // Check for state transitions based on keywords
        if (starts_with(trimmed, "this is a menu for")) {
            // New menu definition
            if (out->menu_count >= 32) {
                return MENU_TRANSPILE_ERR_MEMORY;
            }
            current_menu = &out->menus[out->menu_count++];
            memset(current_menu, 0, sizeof(ParsedMenu));
            extract_quoted(trimmed, current_menu->title, sizeof(current_menu->title));
            state = PARSE_STATE_MENU;
            current_indent = indent;
        }
        else if (state == PARSE_STATE_MENU || state == PARSE_STATE_ITEM) {
            if (starts_with(trimmed, "keyed as")) {
                char id[MENU_ID_MAX];
                if (extract_quoted(trimmed, id, sizeof(id))) {
                    if (current_item) {
                        safe_strcpy(current_item->item_id, sizeof(current_item->item_id), id);
                    } else if (current_menu) {
                        safe_strcpy(current_menu->menu_id, sizeof(current_menu->menu_id), id);
                    }
                }
            }
            else if (starts_with(trimmed, "typed as")) {
                if (current_menu) {
                    extract_quoted(trimmed, current_menu->type, sizeof(current_menu->type));
                }
            }
            else if (starts_with(trimmed, "description")) {
                char desc[MENU_DESC_MAX];
                if (extract_quoted(trimmed, desc, sizeof(desc))) {
                    if (current_item) {
                        safe_strcpy(current_item->description, sizeof(current_item->description), desc);
                    } else if (current_menu) {
                        safe_strcpy(current_menu->description, sizeof(current_menu->description), desc);
                    }
                }
            }
            else if (starts_with(trimmed, "item")) {
                // New item
                if (current_menu && current_menu->item_count < MENU_ITEMS_MAX) {
                    current_item = &current_menu->items[current_menu->item_count++];
                    memset(current_item, 0, sizeof(ParsedMenuItem));
                    extract_quoted(trimmed, current_item->label, sizeof(current_item->label));
                    state = PARSE_STATE_ITEM;
                }
            }
            else if (starts_with(trimmed, "action navigate to")) {
                if (current_item) {
                    current_item->action.type = MENU_ACT_NAVIGATE;
                    extract_quoted(trimmed, current_item->action.target, sizeof(current_item->action.target));
                }
            }
            else if (starts_with(trimmed, "action execute")) {
                if (current_item) {
                    current_item->action.type = MENU_ACT_EXECUTE;
                    extract_quoted(trimmed, current_item->action.target, sizeof(current_item->action.target));
                }
            }
            else if (starts_with(trimmed, "action toggle")) {
                if (current_item) {
                    current_item->action.type = MENU_ACT_TOGGLE;
                    extract_quoted(trimmed, current_item->action.target, sizeof(current_item->action.target));
                }
            }
            else if (starts_with(trimmed, "action adjust")) {
                if (current_item) {
                    current_item->action.type = MENU_ACT_ADJUST;
                    // Extract setting key and options
                    const char* with_pos = strstr(trimmed, "with");
                    if (with_pos) {
                        // Get key before "with"
                        const char* key_start = strchr(trimmed, '"');
                        if (key_start && key_start < with_pos) {
                            key_start++;
                            const char* key_end = strchr(key_start, '"');
                            if (key_end && key_end < with_pos) {
                                size_t len = key_end - key_start;
                                if (len >= sizeof(current_item->action.target)) {
                                    len = sizeof(current_item->action.target) - 1;
                                }
                                memcpy(current_item->action.target, key_start, len);
                                current_item->action.target[len] = '\0';
                            }
                        }
                        // Extract options array
                        current_item->action.option_count = extract_labels(
                            with_pos, current_item->action.options, MENU_OPTIONS_MAX);
                    }
                }
            }
            else if (starts_with(trimmed, "action back")) {
                if (current_item) {
                    current_item->action.type = MENU_ACT_BACK;
                }
            }
            else if (starts_with(trimmed, "action quit")) {
                if (current_item) {
                    current_item->action.type = MENU_ACT_QUIT;
                }
            }
            else if (starts_with(trimmed, "hotkey")) {
                if (current_item) {
                    extract_quoted(trimmed, current_item->hotkey, sizeof(current_item->hotkey));
                }
            }
            else if (starts_with(trimmed, "visible when")) {
                if (current_item) {
                    extract_quoted(trimmed, current_item->visible_when, sizeof(current_item->visible_when));
                }
            }
            else if (starts_with(trimmed, "enabled when")) {
                if (current_item) {
                    extract_quoted(trimmed, current_item->enabled_when, sizeof(current_item->enabled_when));
                }
            }
            else if (starts_with(trimmed, "selected when")) {
                if (current_item) {
                    extract_quoted(trimmed, current_item->selected_when, sizeof(current_item->selected_when));
                }
            }
            else if (starts_with(trimmed, "navigation:")) {
                state = PARSE_STATE_NAVIGATION;
                current_item = NULL;
            }
            else if (starts_with(trimmed, "bindings:")) {
                state = PARSE_STATE_BINDINGS;
                current_item = NULL;
            }
            else if (starts_with(trimmed, "display hints:")) {
                state = PARSE_STATE_DISPLAY;
                current_item = NULL;
            }
            else if (starts_with(trimmed, "hebrew state rules:")) {
                state = PARSE_STATE_HEBREW_RULES;
                current_item = NULL;
            }
            else if (starts_with(trimmed, "tripwire behavior:")) {
                state = PARSE_STATE_TRIPWIRE;
                current_item = NULL;
            }
        }
        else if (state == PARSE_STATE_NAVIGATION && current_menu) {
            if (starts_with(trimmed, "default item")) {
                extract_quoted(trimmed, current_menu->nav.default_item, sizeof(current_menu->nav.default_item));
            }
            else if (starts_with(trimmed, "wrap yes")) {
                current_menu->nav.wrap = true;
            }
            else if (starts_with(trimmed, "wrap no")) {
                current_menu->nav.wrap = false;
            }
            else if (starts_with(trimmed, "remember position yes")) {
                current_menu->nav.remember_position = true;
            }
            else if (starts_with(trimmed, "parent menu")) {
                extract_quoted(trimmed, current_menu->nav.parent_menu, sizeof(current_menu->nav.parent_menu));
            }
            else if (indent <= current_indent) {
                state = PARSE_STATE_MENU;
            }
        }
        else if (state == PARSE_STATE_BINDINGS && current_menu) {
            if (starts_with(trimmed, "up with")) {
                current_menu->bindings.up_count = extract_keys(
                    trimmed, current_menu->bindings.up, MENU_BINDINGS_MAX);
            }
            else if (starts_with(trimmed, "down with")) {
                current_menu->bindings.down_count = extract_keys(
                    trimmed, current_menu->bindings.down, MENU_BINDINGS_MAX);
            }
            else if (starts_with(trimmed, "left with")) {
                current_menu->bindings.left_count = extract_keys(
                    trimmed, current_menu->bindings.left, MENU_BINDINGS_MAX);
            }
            else if (starts_with(trimmed, "right with")) {
                current_menu->bindings.right_count = extract_keys(
                    trimmed, current_menu->bindings.right, MENU_BINDINGS_MAX);
            }
            else if (starts_with(trimmed, "select with")) {
                current_menu->bindings.select_count = extract_keys(
                    trimmed, current_menu->bindings.select, MENU_BINDINGS_MAX);
            }
            else if (starts_with(trimmed, "back with")) {
                current_menu->bindings.back_count = extract_keys(
                    trimmed, current_menu->bindings.back, MENU_BINDINGS_MAX);
            }
            else if (starts_with(trimmed, "cancel with")) {
                current_menu->bindings.cancel_count = extract_keys(
                    trimmed, current_menu->bindings.cancel, MENU_BINDINGS_MAX);
            }
            else if (indent <= current_indent) {
                state = PARSE_STATE_MENU;
            }
        }
        else if (state == PARSE_STATE_DISPLAY && current_menu) {
            if (starts_with(trimmed, "cli:")) {
                state = PARSE_STATE_DISPLAY_CLI;
            }
            else if (starts_with(trimmed, "tui:")) {
                state = PARSE_STATE_DISPLAY_TUI;
            }
            else if (starts_with(trimmed, "gui:")) {
                state = PARSE_STATE_DISPLAY_GUI;
            }
            else if (indent <= current_indent) {
                state = PARSE_STATE_MENU;
            }
        }
        else if (state == PARSE_STATE_DISPLAY_CLI && current_menu) {
            if (starts_with(trimmed, "width")) {
                const char* num = trimmed + 5;
                while (*num && !isdigit((unsigned char)*num)) num++;
                current_menu->display.cli.width = atoi(num);
            }
            else if (starts_with(trimmed, "show border yes")) {
                current_menu->display.cli.show_border = true;
            }
            else if (starts_with(trimmed, "show hotkeys yes")) {
                current_menu->display.cli.show_hotkeys = true;
            }
            else if (starts_with(trimmed, "cursor")) {
                extract_quoted(trimmed, current_menu->display.cli.cursor, sizeof(current_menu->display.cli.cursor));
            }
            else if (indent <= current_indent + 1) {
                state = PARSE_STATE_DISPLAY;
            }
        }
        else if (state == PARSE_STATE_DISPLAY_TUI && current_menu) {
            if (starts_with(trimmed, "colors")) {
                extract_quoted(trimmed, current_menu->display.tui.colorscheme, sizeof(current_menu->display.tui.colorscheme));
            }
            else if (starts_with(trimmed, "box style")) {
                extract_quoted(trimmed, current_menu->display.tui.box_style, sizeof(current_menu->display.tui.box_style));
            }
            else if (indent <= current_indent + 1) {
                state = PARSE_STATE_DISPLAY;
            }
        }
        else if (state == PARSE_STATE_DISPLAY_GUI && current_menu) {
            if (starts_with(trimmed, "layout")) {
                extract_quoted(trimmed, current_menu->display.gui.layout, sizeof(current_menu->display.gui.layout));
            }
            else if (starts_with(trimmed, "animation")) {
                extract_quoted(trimmed, current_menu->display.gui.animation, sizeof(current_menu->display.gui.animation));
            }
            else if (indent <= current_indent + 1) {
                state = PARSE_STATE_DISPLAY;
            }
        }
        else if (state == PARSE_STATE_HEBREW_RULES && current_menu) {
            // Hebrew state rule: "when shavar:" or "when yashar:"
            if (starts_with(trimmed, "when ")) {
                if (current_menu->rule_count < MENU_RULES_MAX) {
                    current_rule = &current_menu->hebrew_rules[current_menu->rule_count++];
                    memset(current_rule, 0, sizeof(ParsedHebrewRule));
                    // Extract state name (between "when " and ":")
                    const char* state_start = trimmed + 5;
                    const char* state_end = strchr(state_start, ':');
                    if (state_end) {
                        size_t len = state_end - state_start;
                        if (len >= sizeof(current_rule->state)) len = sizeof(current_rule->state) - 1;
                        memcpy(current_rule->state, state_start, len);
                        current_rule->state[len] = '\0';
                    }
                    state = PARSE_STATE_HEBREW_RULE;
                }
            }
            else if (indent <= current_indent) {
                state = PARSE_STATE_MENU;
                current_rule = NULL;
            }
        }
        else if (state == PARSE_STATE_HEBREW_RULE && current_rule) {
            if (starts_with(trimmed, "hide items")) {
                current_rule->hide_count = extract_ids(
                    trimmed, current_rule->hide_items, MENU_ITEMS_MAX);
            }
            else if (starts_with(trimmed, "enable items")) {
                current_rule->enable_count = extract_ids(
                    trimmed, current_rule->enable_items, MENU_ITEMS_MAX);
            }
            else if (starts_with(trimmed, "simplify navigation yes")) {
                current_rule->simplify_navigation = true;
            }
            else if (starts_with(trimmed, "reduce options yes")) {
                current_rule->reduce_options = true;
            }
            else if (starts_with(trimmed, "show all options yes")) {
                current_rule->show_all_options = true;
            }
            else if (starts_with(trimmed, "when ")) {
                // Another rule - stay in HEBREW_RULES state
                state = PARSE_STATE_HEBREW_RULES;
                current_rule = NULL;
                // Re-process this line
                continue;
            }
            else if (indent <= current_indent) {
                state = PARSE_STATE_MENU;
                current_rule = NULL;
            }
        }
        else if (state == PARSE_STATE_TRIPWIRE) {
            if (starts_with(trimmed, "on menu not found:")) {
                const char* colon = strchr(trimmed, ':');
                if (colon) {
                    safe_strcpy(out->on_menu_not_found, sizeof(out->on_menu_not_found), skip_ws(colon + 1));
                }
            }
            else if (starts_with(trimmed, "on render failure:")) {
                const char* colon = strchr(trimmed, ':');
                if (colon) {
                    const char* val = skip_ws(colon + 1);
                    if (starts_with(val, "degrade health by")) {
                        const char* num = val + 17;
                        while (*num && !isdigit((unsigned char)*num)) num++;
                        out->health_penalty = atoi(num);
                    }
                }
            }
            else if (indent <= current_indent) {
                state = PARSE_STATE_MENU;
            }
        }

        // Move to next line
        line_start = (*line_end) ? line_end + 1 : line_end;
    }

    return MENU_TRANSPILE_OK;
}

MenuTranspileError menu_parse_file(const char* input_path, ParsedMenuFile* out) {
    if (!input_path || !out) return MENU_TRANSPILE_ERR_NULL;

    FILE* f = fopen(input_path, "r");
    if (!f) return MENU_TRANSPILE_ERR_FILE;

    // Get file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* source = malloc(size + 1);
    if (!source) {
        fclose(f);
        return MENU_TRANSPILE_ERR_MEMORY;
    }

    size_t read = fread(source, 1, size, f);
    source[read] = '\0';
    fclose(f);

    MenuTranspileError err = menu_parse_string(source, out);
    free(source);
    return err;
}

// -----------------------------------------------------------------------------
// B.4 JSONC Output
// -----------------------------------------------------------------------------

static void emit_keys(FILE* f, const char arr[][MENU_KEY_MAX], int count) {
    fprintf(f, "[");
    for (int i = 0; i < count; i++) {
        fprintf(f, "\"%s\"", arr[i]);
        if (i < count - 1) fprintf(f, ", ");
    }
    fprintf(f, "]");
}

static void emit_labels(FILE* f, const char arr[][MENU_LABEL_MAX], int count) {
    fprintf(f, "[");
    for (int i = 0; i < count; i++) {
        fprintf(f, "\"%s\"", arr[i]);
        if (i < count - 1) fprintf(f, ", ");
    }
    fprintf(f, "]");
}

static void emit_action(FILE* f, const ParsedMenuAction* action) {
    fprintf(f, "\"action\": {\n");
    fprintf(f, "        \"type\": ");
    switch (action->type) {
        case MENU_ACT_NAVIGATE: fprintf(f, "\"navigate\",\n        \"target\": \"%s\"", action->target); break;
        case MENU_ACT_EXECUTE:  fprintf(f, "\"execute\",\n        \"command\": \"%s\"", action->target); break;
        case MENU_ACT_TOGGLE:   fprintf(f, "\"toggle\",\n        \"setting\": \"%s\"", action->target); break;
        case MENU_ACT_ADJUST:
            fprintf(f, "\"adjust\",\n        \"setting\": \"%s\",\n        \"options\": ", action->target);
            emit_labels(f, action->options, action->option_count);
            break;
        case MENU_ACT_BACK:     fprintf(f, "\"back\""); break;
        case MENU_ACT_QUIT:     fprintf(f, "\"quit\""); break;
        default:                fprintf(f, "\"none\""); break;
    }
    fprintf(f, "\n      }");
}

MenuTranspileError menu_transpile_to_file(const ParsedMenuFile* pmf, const char* output_path) {
    if (!pmf || !output_path) return MENU_TRANSPILE_ERR_NULL;

    FILE* f = fopen(output_path, "w");
    if (!f) return MENU_TRANSPILE_ERR_WRITE;

    // Header
    fprintf(f, "// #!omni document --jsonc\n");
    fprintf(f, "// Generated from OmniCode menu DSL\n");
    fprintf(f, "// DO NOT EDIT — regenerate from .menu source\n\n");

    fprintf(f, "{\n");
    fprintf(f, "  \"name\": \"Generated Menu Configuration\",\n");
    fprintf(f, "  \"version\": \"a-01.00\",\n\n");

    // Menus array
    fprintf(f, "  \"menus\": [\n");
    for (int m = 0; m < pmf->menu_count; m++) {
        const ParsedMenu* menu = &pmf->menus[m];
        fprintf(f, "    {\n");
        fprintf(f, "      \"identity\": {\n");
        fprintf(f, "        \"menu_id\": \"%s\",\n", menu->menu_id);
        fprintf(f, "        \"title\": \"%s\",\n", menu->title);
        fprintf(f, "        \"type\": \"%s\"\n", menu->type[0] ? menu->type : "submenu");
        if (menu->description[0]) {
            fprintf(f, ",\n        \"description\": \"%s\"\n", menu->description);
        }
        fprintf(f, "      },\n");

        // Items
        fprintf(f, "      \"items\": [\n");
        for (int i = 0; i < menu->item_count; i++) {
            const ParsedMenuItem* item = &menu->items[i];
            fprintf(f, "        {\n");
            fprintf(f, "          \"item_id\": \"%s\",\n", item->item_id);
            fprintf(f, "          \"label\": \"%s\",\n", item->label);
            fprintf(f, "          ");
            emit_action(f, &item->action);

            if (item->hotkey[0]) {
                fprintf(f, ",\n          \"hotkey\": \"%s\"", item->hotkey);
            }
            if (item->enabled_when[0]) {
                fprintf(f, ",\n          \"condition\": { \"enabled_when\": \"%s\" }", item->enabled_when);
            }
            if (item->visible_when[0]) {
                fprintf(f, ",\n          \"visible_when\": \"%s\"", item->visible_when);
            }
            fprintf(f, "\n        }");
            if (i < menu->item_count - 1) fprintf(f, ",");
            fprintf(f, "\n");
        }
        fprintf(f, "      ],\n");

        // Navigation
        fprintf(f, "      \"navigation\": {\n");
        fprintf(f, "        \"default_item\": \"%s\",\n", menu->nav.default_item);
        fprintf(f, "        \"wrap\": %s\n", menu->nav.wrap ? "true" : "false");
        fprintf(f, "      },\n");

        // Bindings
        fprintf(f, "      \"bindings\": {\n");
        if (menu->bindings.up_count > 0) {
            fprintf(f, "        \"up\": ");
            emit_keys(f, menu->bindings.up, menu->bindings.up_count);
            fprintf(f, ",\n");
        }
        if (menu->bindings.down_count > 0) {
            fprintf(f, "        \"down\": ");
            emit_keys(f, menu->bindings.down, menu->bindings.down_count);
            fprintf(f, ",\n");
        }
        if (menu->bindings.select_count > 0) {
            fprintf(f, "        \"select\": ");
            emit_keys(f, menu->bindings.select, menu->bindings.select_count);
            fprintf(f, ",\n");
        }
        if (menu->bindings.back_count > 0) {
            fprintf(f, "        \"back\": ");
            emit_keys(f, menu->bindings.back, menu->bindings.back_count);
        }
        fprintf(f, "\n      },\n");

        // Display hints
        fprintf(f, "      \"display_hints\": {\n");
        fprintf(f, "        \"cli\": {\n");
        fprintf(f, "          \"width\": %d,\n", menu->display.cli.width > 0 ? menu->display.cli.width : 40);
        fprintf(f, "          \"show_border\": %s,\n", menu->display.cli.show_border ? "true" : "false");
        fprintf(f, "          \"cursor\": \"%s\"\n", menu->display.cli.cursor[0] ? menu->display.cli.cursor : "> ");
        fprintf(f, "        }\n");
        fprintf(f, "      }\n");

        fprintf(f, "    }");
        if (m < pmf->menu_count - 1) fprintf(f, ",");
        fprintf(f, "\n");
    }
    fprintf(f, "  ],\n\n");

    // Tripwire
    fprintf(f, "  \"tripwire\": {\n");
    fprintf(f, "    \"on_menu_not_found\": \"%s\",\n", pmf->on_menu_not_found);
    fprintf(f, "    \"health_penalty\": %d\n", pmf->health_penalty);
    fprintf(f, "  }\n");

    fprintf(f, "}\n");
    fclose(f);

    return MENU_TRANSPILE_OK;
}

MenuTranspileError menu_transpile_to_string(const ParsedMenuFile* pmf, char** out_jsonc) {
    if (!pmf || !out_jsonc) return MENU_TRANSPILE_ERR_NULL;

    // Write to temp file, read back (simplest approach)
    const char* tmp = "/tmp/omni_menu_transpile.jsonc";
    MenuTranspileError err = menu_transpile_to_file(pmf, tmp);
    if (err != MENU_TRANSPILE_OK) return err;

    FILE* f = fopen(tmp, "r");
    if (!f) return MENU_TRANSPILE_ERR_FILE;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    *out_jsonc = malloc(size + 1);
    if (!*out_jsonc) {
        fclose(f);
        return MENU_TRANSPILE_ERR_MEMORY;
    }

    size_t read = fread(*out_jsonc, 1, size, f);
    (*out_jsonc)[read] = '\0';
    fclose(f);

    return MENU_TRANSPILE_OK;
}

// -----------------------------------------------------------------------------
// B.5 Convenience
// -----------------------------------------------------------------------------

MenuTranspileError menu_omni_to_jsonc(const char* input_path, const char* output_path) {
    ParsedMenuFile* pmf = menu_parsed_create();
    if (!pmf) return MENU_TRANSPILE_ERR_MEMORY;

    MenuTranspileError err = menu_parse_file(input_path, pmf);
    if (err != MENU_TRANSPILE_OK) {
        menu_parsed_destroy(pmf);
        return err;
    }

    err = menu_transpile_to_file(pmf, output_path);
    menu_parsed_destroy(pmf);
    return err;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Ponder the path of thy feet, and let all thy ways be established."
//   — Proverbs 4:26
//
// The transpiler transforms sentence-based OmniCode menus into
// structured JSONC that the Cornerstone engine consumes.

// =============================================================================
// END CLOSING
// =============================================================================
