// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-lib-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_lib.c — OmniCode Library Loader Implementation
// Resolves imports and links library functions for OmniCode programs.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-lib-impl
// Title: OmniCode Library Loader Implementation
// Type: Source
// Role: Resolve module paths and link imported functions

// # M.8 Grounding [GROUNDING]
//
// Scripture: Ecclesiastes 4:9 - "Two are better than one"
// Principle: Libraries extend capability through cooperation
// Anchor: Genesis 2:18 - "It is not good for man to be alone"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "omni_lib.h"
#include "omni_parser.h"
#include "omni_chunks.h"

// # S.1 Helpers [HELPERS]

static bool file_exists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static void safe_strcpy(char* dest, size_t dest_size, const char* src) {
    if (!dest || dest_size == 0) return;
    if (!src) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Loader Lifecycle [LIFECYCLE]

OmniLibLoader* omni_lib_create(const char* stdlib_path) {
    OmniLibLoader* loader = calloc(1, sizeof(OmniLibLoader));
    if (!loader) return NULL;

    if (stdlib_path) {
        safe_strcpy(loader->stdlib_path, sizeof(loader->stdlib_path), stdlib_path);
    }

    return loader;
}

void omni_lib_destroy(OmniLibLoader* loader) {
    if (!loader) return;

    // Destroy loaded module programs
    for (int i = 0; i < loader->module_count; i++) {
        if (loader->modules[i].program) {
            omni_ir_destroy(loader->modules[i].program);
        }
    }

    free(loader);
}

// # B.2 Search Paths [PATHS]

bool omni_lib_add_search_path(OmniLibLoader* loader, const char* path) {
    if (!loader || !path) return false;
    if (loader->search_path_count >= 8) return false;

    safe_strcpy(loader->search_paths[loader->search_path_count],
                sizeof(loader->search_paths[0]), path);
    loader->search_path_count++;
    return true;
}

void omni_lib_set_stdlib(OmniLibLoader* loader, const char* path) {
    if (!loader) return;
    if (path) {
        safe_strcpy(loader->stdlib_path, sizeof(loader->stdlib_path), path);
    } else {
        loader->stdlib_path[0] = '\0';
    }
}

// # B.3 Module Resolution [RESOLVE]

static char resolved_path[OMNI_LIB_MAX_PATH];

// Disable format-truncation warning - we do runtime bounds checks
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"

const char* omni_lib_resolve(OmniLibLoader* loader, const char* import_path) {
    if (!loader || !import_path) return NULL;

    // Maximum component length to prevent truncation
    // Buffer is 256, so limit components to prevent overflow
    size_t import_len = strlen(import_path);
    if (import_len > 100) return NULL;  // Path too long

    // Check for stdlib prefix
    if (strncmp(import_path, "stdlib/", 7) == 0) {
        const char* module_name = import_path + 7;
        size_t stdlib_len = strlen(loader->stdlib_path);
        size_t name_len = strlen(module_name);

        // Validate lengths before construction
        if (stdlib_len + name_len * 2 + 8 < sizeof(resolved_path)) {
            // Try stdlib_path/module/module.omni
            snprintf(resolved_path, sizeof(resolved_path),
                     "%s%s/%s.omni",
                     loader->stdlib_path,
                     module_name, module_name);

            if (file_exists(resolved_path)) {
                return resolved_path;
            }
        }

        if (stdlib_len + name_len + 6 < sizeof(resolved_path)) {
            // Try stdlib_path/module.omni
            snprintf(resolved_path, sizeof(resolved_path),
                     "%s%s.omni",
                     loader->stdlib_path, module_name);

            if (file_exists(resolved_path)) {
                return resolved_path;
            }
        }
    }

    // Search in search paths
    for (int i = 0; i < loader->search_path_count; i++) {
        // Try path/import/import.omni
        const char* basename = strrchr(import_path, '/');
        basename = basename ? basename + 1 : import_path;

        size_t search_len = strlen(loader->search_paths[i]);
        size_t base_len = strlen(basename);

        if (search_len + import_len + base_len + 8 < sizeof(resolved_path)) {
            snprintf(resolved_path, sizeof(resolved_path),
                     "%s%s/%s.omni",
                     loader->search_paths[i],
                     import_path, basename);

            if (file_exists(resolved_path)) {
                return resolved_path;
            }
        }

        if (search_len + import_len + 6 < sizeof(resolved_path)) {
            // Try path/import.omni
            snprintf(resolved_path, sizeof(resolved_path),
                     "%s%s.omni",
                     loader->search_paths[i], import_path);

            if (file_exists(resolved_path)) {
                return resolved_path;
            }
        }
    }

    // Try current directory
    if (import_len + 6 < sizeof(resolved_path)) {
        snprintf(resolved_path, sizeof(resolved_path), "%s.omni", import_path);
        if (file_exists(resolved_path)) {
            return resolved_path;
        }
    }

    return NULL;
}

#pragma GCC diagnostic pop

// # B.4 Module Loading [LOAD]

// Forward declarations for parsing
static bool parse_module_provides(OmniLibModule* module, const char* source);

int omni_lib_load(OmniLibLoader* loader, const char* import_path) {
    if (!loader || !import_path) return -1;
    if (loader->module_count >= OMNI_LIB_MAX_MODULES) return -1;

    // Extract module name from path
    const char* name = strrchr(import_path, '/');
    name = name ? name + 1 : import_path;

    // Check if already loaded
    for (int i = 0; i < loader->module_count; i++) {
        if (strcmp(loader->modules[i].name, name) == 0) {
            return i;  // Already loaded
        }
    }

    // Resolve path
    const char* resolved = omni_lib_resolve(loader, import_path);
    if (!resolved) {
        fprintf(stderr, "omni_lib: Cannot resolve module '%s'\n", import_path);
        return -1;
    }

    // Read source file
    FILE* f = fopen(resolved, "r");
    if (!f) {
        fprintf(stderr, "omni_lib: Cannot open '%s'\n", resolved);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* source = malloc(size + 1);
    if (!source) {
        fclose(f);
        return -1;
    }

    size_t read = fread(source, 1, size, f);
    source[read] = '\0';
    fclose(f);

    // Create module entry
    int idx = loader->module_count;
    OmniLibModule* module = &loader->modules[idx];

    safe_strcpy(module->name, sizeof(module->name), name);
    safe_strcpy(module->path, sizeof(module->path), resolved);
    module->export_count = 0;
    module->loaded = false;

    // Parse exports from source
    parse_module_provides(module, source);

    // Parse and compile module
    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, (size_t)read);

    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    OmniDocument* doc = omni_parser_parse(&parser);

    if (doc) {
        module->program = omni_ir_from_ast(doc);
        omni_document_destroy(doc);
    }

    free(source);

    if (!module->program) {
        fprintf(stderr, "omni_lib: Failed to compile '%s'\n", resolved);
        return -1;
    }

    module->loaded = true;
    loader->module_count++;

    return idx;
}

OmniLibModule* omni_lib_get_module(OmniLibLoader* loader, uint8_t index) {
    if (!loader || index >= loader->module_count) return NULL;
    return &loader->modules[index];
}

OmniLibModule* omni_lib_find_module(OmniLibLoader* loader, const char* name) {
    if (!loader || !name) return NULL;

    for (int i = 0; i < loader->module_count; i++) {
        if (strcmp(loader->modules[i].name, name) == 0) {
            return &loader->modules[i];
        }
    }
    return NULL;
}

// # B.5 Export Resolution [EXPORTS]

int omni_lib_find_export(OmniLibModule* module, const char* name) {
    if (!module || !name) return -1;

    for (int i = 0; i < module->export_count; i++) {
        if (strcmp(module->exports[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

uint16_t omni_lib_get_export_func(OmniLibModule* module, int export_index) {
    if (!module || export_index < 0 || export_index >= module->export_count) {
        return 0xFFFF;
    }
    return module->exports[export_index].func_index;
}

OmniValue omni_lib_get_export_value(OmniLibModule* module, int export_index) {
    OmniValue none = {.type = VAL_NONE};
    if (!module || export_index < 0 || export_index >= module->export_count) {
        return none;
    }
    return module->exports[export_index].value;
}

// # B.6 Module Linking [LINK]

bool omni_lib_link(OmniLibLoader* loader, OmniIRProgram* prog) {
    if (!loader || !prog) return false;

    // Linking happens at emit time by resolving import paths
    // and patching OP_CALL_EXT instructions with module indices

    // For now, just return success - actual linking done during emission
    return true;
}

// # B.7 Module Parsing [PARSE]

static bool parse_module_provides(OmniLibModule* module, const char* source) {
    if (!module || !source) return false;

    // Simple line-by-line parser for "provides" declarations
    // Format: "    name    \"description\""
    // Found after "this library provides:"

    const char* provides = strstr(source, "this library provides:");
    if (!provides) return false;

    // Skip to next line
    provides = strchr(provides, '\n');
    if (!provides) return false;
    provides++;

    // Parse export lines until we hit END METADATA or next section
    while (*provides && module->export_count < OMNI_LIB_MAX_EXPORTS) {
        // Skip whitespace
        while (*provides == ' ' || *provides == '\t') provides++;

        // Check for end of provides section
        if (*provides == '\n' || *provides == '\0') {
            provides++;
            continue;
        }

        // Check for section markers
        if (strncmp(provides, "//", 2) == 0 ||
            strncmp(provides, "END", 3) == 0) {
            break;
        }

        // Extract name (until whitespace or quote)
        char name[OMNI_LIB_MAX_NAME];
        int name_len = 0;
        while (*provides && *provides != ' ' && *provides != '\t' &&
               *provides != '"' && *provides != '\n' &&
               name_len < OMNI_LIB_MAX_NAME - 1) {
            name[name_len++] = *provides++;
        }
        name[name_len] = '\0';

        if (name_len > 0) {
            OmniLibExport* exp = &module->exports[module->export_count];
            safe_strcpy(exp->name, sizeof(exp->name), name);
            exp->is_function = true;
            exp->type = VAL_FUNC;
            exp->func_index = module->export_count;  // Will be updated during compile
            module->export_count++;
        }

        // Skip to end of line
        while (*provides && *provides != '\n') provides++;
        if (*provides == '\n') provides++;
    }

    return true;
}

bool omni_lib_parse_exports(OmniLibModule* module, const char* source) {
    return parse_module_provides(module, source);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Two are better than one; because they have a good reward for their labour."
//   — Ecclesiastes 4:9
//
// Libraries multiply our capability through shared wisdom.

// =============================================================================
// END CLOSING
// =============================================================================
