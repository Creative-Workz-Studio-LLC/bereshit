// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-c-transpiler-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_c_transpiler.c — OmniCode to C Code Transpiler Implementation
// Generates static C structures from OmniCode definitions.

// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key: B-word-work-pkg-omni-c-transpiler-impl
// Title: OmniCode to C Code Transpiler Implementation
// Type: Source
// Role: Generate static C code from OmniCode for compile-time initialization
//
// Scripture: Exodus 25:40 — Make them after their pattern
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_c_transpiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Helpers
// -----------------------------------------------------------------------------

// Convert string to C identifier (replace non-alphanum with underscore)
static void to_c_identifier(const char* src, char* dst, size_t dst_size) {
    size_t i = 0;
    while (*src && i < dst_size - 1) {
        char c = *src++;
        if (isalnum((unsigned char)c)) {
            dst[i++] = c;
        } else {
            dst[i++] = '_';
        }
    }
    dst[i] = '\0';
}

// Get current timestamp as string
static void get_timestamp(char* buf, size_t size) {
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm);
}

// Map action type to C enum name
static const char* action_type_to_c(MenuActionType type) {
    switch (type) {
        case MENU_ACT_NAVIGATE: return "MENU_ACTION_NAVIGATE";
        case MENU_ACT_EXECUTE:  return "MENU_ACTION_EXECUTE";
        case MENU_ACT_TOGGLE:   return "MENU_ACTION_TOGGLE";
        case MENU_ACT_ADJUST:   return "MENU_ACTION_VALUE";
        case MENU_ACT_BACK:     return "MENU_ACTION_BACK";
        case MENU_ACT_QUIT:     return "MENU_ACTION_QUIT";
        default:                return "MENU_ACTION_NONE";
    }
}

// -----------------------------------------------------------------------------
// B.2 Config
// -----------------------------------------------------------------------------

void c_transpile_config_init(CTranspileConfig* cfg, const char* prefix) {
    if (!cfg) return;

    cfg->header_guard = NULL;
    cfg->prefix = prefix ? prefix : "omni_";
    cfg->include_header = NULL;
    cfg->generate_static = true;
    cfg->generate_getters = true;
    cfg->generate_register = true;
    cfg->mode = C_TRANSPILE_BOTH;
}

// -----------------------------------------------------------------------------
// B.3 Header Generation
// -----------------------------------------------------------------------------

CTranspileError c_transpile_menus_to_header(
    const ParsedMenuFile* pmf,
    const CTranspileConfig* cfg,
    const char* output_path
) {
    if (!pmf || !cfg || !output_path) return C_TRANSPILE_ERR_NULL;

    FILE* f = fopen(output_path, "w");
    if (!f) return C_TRANSPILE_ERR_FILE;

    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    // Generate header guard
    char guard[128];
    if (cfg->header_guard) {
        strncpy(guard, cfg->header_guard, sizeof(guard) - 1);
    } else {
        to_c_identifier(output_path, guard, sizeof(guard));
        for (char* p = guard; *p; p++) *p = toupper((unsigned char)*p);
    }

    // Header
    fprintf(f, "// #!omni code --c -library\n");
    fprintf(f, "// Generated from OmniCode menu DSL\n");
    fprintf(f, "// Generated at: %s\n", timestamp);
    fprintf(f, "// DO NOT EDIT — regenerate from .menu source\n\n");

    fprintf(f, "#ifndef %s\n", guard);
    fprintf(f, "#define %s\n\n", guard);

    fprintf(f, "#include \"menu.h\"\n\n");

    // Forward declarations for each menu
    for (int m = 0; m < pmf->menu_count; m++) {
        const ParsedMenu* menu = &pmf->menus[m];
        char menu_var[128];
        snprintf(menu_var, sizeof(menu_var), "%s%s", cfg->prefix, menu->menu_id);

        fprintf(f, "// Menu: %s\n", menu->title);
        fprintf(f, "extern const MenuItem %s_items[];\n", menu_var);
        fprintf(f, "extern const int %s_item_count;\n", menu_var);
        fprintf(f, "extern const Menu %s;\n", menu_var);

        if (cfg->generate_getters) {
            fprintf(f, "const Menu* %s_get(void);\n", menu_var);
        }
        fprintf(f, "\n");
    }

    // Registration function
    if (cfg->generate_register) {
        fprintf(f, "// Register all menus with menu system\n");
        fprintf(f, "void %sregister_all_menus(MenuSystem* sys);\n\n", cfg->prefix);
    }

    fprintf(f, "#endif // %s\n", guard);
    fclose(f);

    return C_TRANSPILE_OK;
}

// -----------------------------------------------------------------------------
// B.4 Source Generation
// -----------------------------------------------------------------------------

static void emit_c_string(FILE* f, const char* s) {
    if (!s || !*s) {
        fprintf(f, "\"\"");
        return;
    }
    fprintf(f, "\"");
    while (*s) {
        switch (*s) {
            case '"':  fprintf(f, "\\\""); break;
            case '\\': fprintf(f, "\\\\"); break;
            case '\n': fprintf(f, "\\n"); break;
            case '\t': fprintf(f, "\\t"); break;
            default:   fputc(*s, f); break;
        }
        s++;
    }
    fprintf(f, "\"");
}

static void emit_binding_array(FILE* f, const char arr[][16], int count, const char* name) {
    if (count <= 0) {
        fprintf(f, "        .%s = {0},\n", name);
        fprintf(f, "        .%s_count = 0,\n", name);
        return;
    }

    fprintf(f, "        .%s = {", name);
    for (int i = 0; i < count; i++) {
        emit_c_string(f, arr[i]);
        if (i < count - 1) fprintf(f, ", ");
    }
    fprintf(f, "},\n");
    fprintf(f, "        .%s_count = %d,\n", name, count);
}

CTranspileError c_transpile_menus_to_source(
    const ParsedMenuFile* pmf,
    const CTranspileConfig* cfg,
    const char* output_path
) {
    if (!pmf || !cfg || !output_path) return C_TRANSPILE_ERR_NULL;

    FILE* f = fopen(output_path, "w");
    if (!f) return C_TRANSPILE_ERR_FILE;

    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    // Header
    fprintf(f, "// #!omni code --c -library\n");
    fprintf(f, "// Generated from OmniCode menu DSL\n");
    fprintf(f, "// Generated at: %s\n", timestamp);
    fprintf(f, "// DO NOT EDIT — regenerate from .menu source\n\n");

    // Includes
    if (cfg->include_header) {
        fprintf(f, "#include \"%s\"\n", cfg->include_header);
    }
    fprintf(f, "#include \"menu.h\"\n");
    fprintf(f, "#include <string.h>\n\n");

    // Generate each menu
    for (int m = 0; m < pmf->menu_count; m++) {
        const ParsedMenu* menu = &pmf->menus[m];
        char menu_var[128];
        snprintf(menu_var, sizeof(menu_var), "%s%s", cfg->prefix, menu->menu_id);

        fprintf(f, "// ==========================================================================\n");
        fprintf(f, "// Menu: %s (%s)\n", menu->title, menu->menu_id);
        fprintf(f, "// ==========================================================================\n\n");

        // Items array
        if (cfg->generate_static) {
            fprintf(f, "static ");
        }
        fprintf(f, "const MenuItem %s_items[] = {\n", menu_var);

        for (int i = 0; i < menu->item_count; i++) {
            const ParsedMenuItem* item = &menu->items[i];

            fprintf(f, "    {\n");
            fprintf(f, "        .item_id = ");
            emit_c_string(f, item->item_id);
            fprintf(f, ",\n");

            fprintf(f, "        .label = ");
            emit_c_string(f, item->label);
            fprintf(f, ",\n");

            fprintf(f, "        .description = ");
            emit_c_string(f, item->description);
            fprintf(f, ",\n");

            fprintf(f, "        .action = %s,\n", action_type_to_c(item->action.type));

            fprintf(f, "        .action_target = ");
            emit_c_string(f, item->action.target);
            fprintf(f, ",\n");

            fprintf(f, "        .hotkey = ");
            emit_c_string(f, item->hotkey);
            fprintf(f, ",\n");

            fprintf(f, "        .enabled_condition = ");
            emit_c_string(f, item->enabled_when);
            fprintf(f, ",\n");

            fprintf(f, "        .visible_condition = ");
            emit_c_string(f, item->visible_when);
            fprintf(f, ",\n");

            fprintf(f, "        .visible = true,\n");
            fprintf(f, "        .enabled = true,\n");
            fprintf(f, "        .selected = false,\n");

            fprintf(f, "    },\n");
        }
        fprintf(f, "};\n\n");

        // Item count
        fprintf(f, "const int %s_item_count = %d;\n\n", menu_var, menu->item_count);

        // Menu struct
        fprintf(f, "const Menu %s = {\n", menu_var);
        fprintf(f, "    .menu_id = ");
        emit_c_string(f, menu->menu_id);
        fprintf(f, ",\n");

        fprintf(f, "    .title = ");
        emit_c_string(f, menu->title);
        fprintf(f, ",\n");

        fprintf(f, "    .items = %s_items,\n", menu_var);
        fprintf(f, "    .item_count = %d,\n", menu->item_count);

        // Navigation
        fprintf(f, "    .nav = {\n");
        fprintf(f, "        .default_item = ");
        emit_c_string(f, menu->nav.default_item);
        fprintf(f, ",\n");
        fprintf(f, "        .wrap = %s,\n", menu->nav.wrap ? "true" : "false");
        fprintf(f, "        .remember_position = %s,\n", menu->nav.remember_position ? "true" : "false");
        fprintf(f, "        .parent_menu = ");
        emit_c_string(f, menu->nav.parent_menu);
        fprintf(f, ",\n");
        fprintf(f, "        .current_index = 0,\n");
        fprintf(f, "    },\n");

        // Bindings
        fprintf(f, "    .bindings = {\n");
        emit_binding_array(f, menu->bindings.up, menu->bindings.up_count, "up");
        emit_binding_array(f, menu->bindings.down, menu->bindings.down_count, "down");
        emit_binding_array(f, menu->bindings.left, menu->bindings.left_count, "left");
        emit_binding_array(f, menu->bindings.right, menu->bindings.right_count, "right");
        emit_binding_array(f, menu->bindings.select, menu->bindings.select_count, "select");
        emit_binding_array(f, menu->bindings.back, menu->bindings.back_count, "back");
        emit_binding_array(f, menu->bindings.cancel, menu->bindings.cancel_count, "cancel");
        fprintf(f, "    },\n");

        // Display hints
        fprintf(f, "    .display = {\n");
        fprintf(f, "        .cli_width = %d,\n", menu->display.cli.width > 0 ? menu->display.cli.width : 40);
        fprintf(f, "        .cli_border = %s,\n", menu->display.cli.show_border ? "true" : "false");
        fprintf(f, "        .cli_cursor = ");
        emit_c_string(f, menu->display.cli.cursor[0] ? menu->display.cli.cursor : "> ");
        fprintf(f, ",\n");
        fprintf(f, "    },\n");

        fprintf(f, "};\n\n");

        // Getter function
        if (cfg->generate_getters) {
            fprintf(f, "const Menu* %s_get(void) {\n", menu_var);
            fprintf(f, "    return &%s;\n", menu_var);
            fprintf(f, "}\n\n");
        }
    }

    // Registration function
    if (cfg->generate_register) {
        fprintf(f, "// ==========================================================================\n");
        fprintf(f, "// Register all menus\n");
        fprintf(f, "// ==========================================================================\n\n");

        fprintf(f, "void %sregister_all_menus(MenuSystem* sys) {\n", cfg->prefix);
        fprintf(f, "    if (!sys) return;\n\n");

        for (int m = 0; m < pmf->menu_count; m++) {
            const ParsedMenu* menu = &pmf->menus[m];
            char menu_var[128];
            snprintf(menu_var, sizeof(menu_var), "%s%s", cfg->prefix, menu->menu_id);

            fprintf(f, "    // Register %s\n", menu->title);
            fprintf(f, "    if (sys->menu_count < MENU_MENUS_MAX) {\n");
            fprintf(f, "        memcpy(&sys->menus[sys->menu_count], &%s, sizeof(Menu));\n", menu_var);
            fprintf(f, "        sys->menu_count++;\n");
            fprintf(f, "    }\n\n");
        }

        fprintf(f, "}\n");
    }

    fclose(f);
    return C_TRANSPILE_OK;
}

// -----------------------------------------------------------------------------
// B.5 Combined Generation
// -----------------------------------------------------------------------------

CTranspileError c_transpile_menus(
    const ParsedMenuFile* pmf,
    const CTranspileConfig* cfg,
    const char* output_base
) {
    if (!pmf || !cfg || !output_base) return C_TRANSPILE_ERR_NULL;

    CTranspileError err = C_TRANSPILE_OK;
    char path[512];

    if (cfg->mode & C_TRANSPILE_HEADER) {
        snprintf(path, sizeof(path), "%s.h", output_base);
        err = c_transpile_menus_to_header(pmf, cfg, path);
        if (err != C_TRANSPILE_OK) return err;
    }

    if (cfg->mode & C_TRANSPILE_SOURCE) {
        snprintf(path, sizeof(path), "%s.c", output_base);

        // Update config to include the header we just generated
        CTranspileConfig src_cfg = *cfg;
        char header_name[256];
        snprintf(header_name, sizeof(header_name), "%s.h", output_base);
        // Extract just filename
        const char* slash = strrchr(header_name, '/');
        src_cfg.include_header = slash ? slash + 1 : header_name;

        err = c_transpile_menus_to_source(pmf, &src_cfg, path);
    }

    return err;
}

// -----------------------------------------------------------------------------
// B.6 One-Shot
// -----------------------------------------------------------------------------

CTranspileError omni_menu_to_c(
    const char* input_path,
    const CTranspileConfig* cfg,
    const char* output_base
) {
    if (!input_path || !cfg || !output_base) return C_TRANSPILE_ERR_NULL;

    ParsedMenuFile* pmf = menu_parsed_create();
    if (!pmf) return C_TRANSPILE_ERR_MEMORY;

    MenuTranspileError perr = menu_parse_file(input_path, pmf);
    if (perr != MENU_TRANSPILE_OK) {
        menu_parsed_destroy(pmf);
        return C_TRANSPILE_ERR_PARSE;
    }

    CTranspileError err = c_transpile_menus(pmf, cfg, output_base);
    menu_parsed_destroy(pmf);
    return err;
}

// -----------------------------------------------------------------------------
// B.7 Stream Output
// -----------------------------------------------------------------------------

CTranspileError c_transpile_menus_to_stream(
    const ParsedMenuFile* pmf,
    const CTranspileConfig* cfg,
    FILE* header_out,
    FILE* source_out
) {
    // This would write to provided streams instead of files
    // Implementation similar to file versions but using provided FILEs
    // For now, redirect through temp files

    if (!pmf || !cfg) return C_TRANSPILE_ERR_NULL;

    CTranspileError err = C_TRANSPILE_OK;

    if (header_out) {
        err = c_transpile_menus_to_header(pmf, cfg, "/tmp/omni_c_transpile.h");
        if (err == C_TRANSPILE_OK) {
            FILE* tmp = fopen("/tmp/omni_c_transpile.h", "r");
            if (tmp) {
                char buf[4096];
                size_t n;
                while ((n = fread(buf, 1, sizeof(buf), tmp)) > 0) {
                    fwrite(buf, 1, n, header_out);
                }
                fclose(tmp);
            }
        }
    }

    if (source_out && err == C_TRANSPILE_OK) {
        err = c_transpile_menus_to_source(pmf, cfg, "/tmp/omni_c_transpile.c");
        if (err == C_TRANSPILE_OK) {
            FILE* tmp = fopen("/tmp/omni_c_transpile.c", "r");
            if (tmp) {
                char buf[4096];
                size_t n;
                while ((n = fread(buf, 1, sizeof(buf), tmp)) > 0) {
                    fwrite(buf, 1, n, source_out);
                }
                fclose(tmp);
            }
        }
    }

    return err;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Look that thou make them after their pattern, which was shewed thee."
//   — Exodus 25:40
//
// OmniCode is the pattern; C is the manifestation.
// The transpiler bridges specification to implementation.

// =============================================================================
// END CLOSING
// =============================================================================
