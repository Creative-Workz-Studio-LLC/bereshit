// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-c-transpiler
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_c_transpiler.h — OmniCode to C Code Transpiler
// Generates static C structures from OmniCode definitions.

#ifndef BERESHIT_OMNI_C_TRANSPILER_H
#define BERESHIT_OMNI_C_TRANSPILER_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key: B-word-work-pkg-omni-c-transpiler
// Title: OmniCode to C Code Transpiler
// Type: Header
// Role: Generate static C code from OmniCode for compile-time initialization
//
// Scripture: Exodus 25:40 — Make them after their pattern
// Principle: OmniCode patterns become C structures
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_menu_transpiler.h"
#include <stdbool.h>
#include <stdio.h>

// # S.1 Defines [DEFINES]

// Output modes
#define C_TRANSPILE_HEADER   1    // Generate .h file
#define C_TRANSPILE_SOURCE   2    // Generate .c file
#define C_TRANSPILE_BOTH     3    // Generate both

// # S.2 Types [TYPES]

typedef enum CTranspileError {
    C_TRANSPILE_OK = 0,
    C_TRANSPILE_ERR_NULL,
    C_TRANSPILE_ERR_FILE,
    C_TRANSPILE_ERR_PARSE,
    C_TRANSPILE_ERR_WRITE,
    C_TRANSPILE_ERR_MEMORY,
} CTranspileError;

// Configuration for C code generation
typedef struct {
    const char* header_guard;       // Header guard name (e.g., "GAME_MENUS_H")
    const char* prefix;             // Prefix for generated names (e.g., "game_")
    const char* include_header;     // Header to include in .c file
    bool        generate_static;    // Generate static arrays
    bool        generate_getters;   // Generate accessor functions
    bool        generate_register;  // Generate registration function
    int         mode;               // C_TRANSPILE_HEADER, SOURCE, or BOTH
} CTranspileConfig;

// # S.3 Function Prototypes [PROTOTYPES]

// Initialize default config
void c_transpile_config_init(CTranspileConfig* cfg, const char* prefix);

// Transpile parsed menus to C header file
CTranspileError c_transpile_menus_to_header(
    const ParsedMenuFile* pmf,
    const CTranspileConfig* cfg,
    const char* output_path
);

// Transpile parsed menus to C source file
CTranspileError c_transpile_menus_to_source(
    const ParsedMenuFile* pmf,
    const CTranspileConfig* cfg,
    const char* output_path
);

// Transpile parsed menus to C (header and/or source based on mode)
CTranspileError c_transpile_menus(
    const ParsedMenuFile* pmf,
    const CTranspileConfig* cfg,
    const char* output_base  // e.g., "menus" → menus.h, menus.c
);

// One-shot: parse OmniCode menu file and generate C code
CTranspileError omni_menu_to_c(
    const char* input_path,
    const CTranspileConfig* cfg,
    const char* output_base
);

// Write C code to file stream (for custom output)
CTranspileError c_transpile_menus_to_stream(
    const ParsedMenuFile* pmf,
    const CTranspileConfig* cfg,
    FILE* header_out,
    FILE* source_out
);

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================
//
// Organization:
//   Types: 2 (enum, config struct)
//   Functions: 6 (config init, header, source, both, one-shot, stream)
//
// Generated Code Pattern:
//
//   Header (.h):
//     extern const MenuItem menu_main_items[];
//     extern const Menu menu_main;
//     const Menu* get_menu_main(void);
//
//   Source (.c):
//     static const MenuItem menu_main_items[] = { ... };
//     const Menu menu_main = { ... };
//     const Menu* get_menu_main(void) { return &menu_main; }
//
// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// "Look that thou make them after their pattern, which was shewed thee."
//   — Exodus 25:40
//
// OmniCode is the pattern; C is the manifestation.
// The transpiler bridges specification to implementation.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_C_TRANSPILER_H
