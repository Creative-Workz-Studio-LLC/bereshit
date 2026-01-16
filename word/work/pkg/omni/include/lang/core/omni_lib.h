// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-lib
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_lib.h — OmniCode Library Loader
// Resolves imports and links library functions for OmniCode programs.

#ifndef BERESHIT_OMNI_LIB_H
#define BERESHIT_OMNI_LIB_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-lib
// Title: OmniCode Library Loader
// Type: Header
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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "omni_ir.h"

// # S.1 Constants [CONSTANTS]

#define OMNI_LIB_MAX_MODULES    32      // Maximum loaded modules
#define OMNI_LIB_MAX_EXPORTS    64      // Maximum exports per module
#define OMNI_LIB_MAX_PATH       512     // Maximum module path length
#define OMNI_LIB_MAX_NAME       64      // Maximum export name length

// # S.2 Types [TYPES]

// Export entry - a function or value exported by a module
typedef struct {
    char            name[OMNI_LIB_MAX_NAME];
    OmniValueType   type;
    uint16_t        func_index;     // If function, index in module's function table
    OmniValue       value;          // If value, the constant value
    bool            is_function;
} OmniLibExport;

// Loaded module
typedef struct {
    char            name[OMNI_LIB_MAX_NAME];     // Module name (e.g., "math")
    char            path[OMNI_LIB_MAX_PATH];     // Full path to module file
    OmniIRProgram*  program;                     // Compiled IR
    OmniLibExport   exports[OMNI_LIB_MAX_EXPORTS];
    uint16_t        export_count;
    bool            loaded;
} OmniLibModule;

// Library loader context
typedef struct {
    OmniLibModule   modules[OMNI_LIB_MAX_MODULES];
    uint8_t         module_count;

    // Search paths for module resolution
    char            search_paths[8][OMNI_LIB_MAX_PATH];
    uint8_t         search_path_count;

    // Stdlib location
    char            stdlib_path[OMNI_LIB_MAX_PATH];
} OmniLibLoader;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Loader Lifecycle [LIFECYCLE]

// Create library loader with stdlib path
OmniLibLoader* omni_lib_create(const char* stdlib_path);

// Destroy loader and all loaded modules
void omni_lib_destroy(OmniLibLoader* loader);

// # B.2 Search Paths [PATHS]

// Add a search path for module resolution
bool omni_lib_add_search_path(OmniLibLoader* loader, const char* path);

// Set stdlib path (stdlib/* imports resolve here)
void omni_lib_set_stdlib(OmniLibLoader* loader, const char* path);

// # B.3 Module Resolution [RESOLVE]

// Resolve module path from import statement
// "stdlib/math" -> "/path/to/stdlib/math/math.omni"
// "mylib" -> searches search paths, then current directory
const char* omni_lib_resolve(OmniLibLoader* loader, const char* import_path);

// # B.4 Module Loading [LOAD]

// Load a module by resolved path
// Returns module index, or -1 on failure
int omni_lib_load(OmniLibLoader* loader, const char* import_path);

// Get loaded module by index
OmniLibModule* omni_lib_get_module(OmniLibLoader* loader, uint8_t index);

// Get loaded module by name
OmniLibModule* omni_lib_find_module(OmniLibLoader* loader, const char* name);

// # B.5 Export Resolution [EXPORTS]

// Find export in module by name
// Returns export index, or -1 if not found
int omni_lib_find_export(OmniLibModule* module, const char* name);

// Get export function index for calling
uint16_t omni_lib_get_export_func(OmniLibModule* module, int export_index);

// Get export value for constants
OmniValue omni_lib_get_export_value(OmniLibModule* module, int export_index);

// # B.6 Module Linking [LINK]

// Link all imports in a program
// Resolves import statements and updates call targets
bool omni_lib_link(OmniLibLoader* loader, OmniIRProgram* prog);

// # B.7 Module Parsing [PARSE]

// Parse module metadata from OmniCode source
// Extracts "provides" declarations to populate exports
bool omni_lib_parse_exports(OmniLibModule* module, const char* source);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Example
//
//   OmniLibLoader* loader = omni_lib_create("stdlib/");
//   omni_lib_add_search_path(loader, "lib/");
//
//   // Load math library
//   int math_idx = omni_lib_load(loader, "stdlib/math");
//   if (math_idx >= 0) {
//       OmniLibModule* math = omni_lib_get_module(loader, math_idx);
//       int abs_idx = omni_lib_find_export(math, "abs");
//       if (abs_idx >= 0) {
//           uint16_t func_id = omni_lib_get_export_func(math, abs_idx);
//           // Call function...
//       }
//   }
//
//   omni_lib_destroy(loader);

// # X.2 Closing Note
//
// "Two are better than one; because they have a good reward for their labour."
//   — Ecclesiastes 4:9
//
// Libraries multiply our capability through shared wisdom.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_LIB_H
