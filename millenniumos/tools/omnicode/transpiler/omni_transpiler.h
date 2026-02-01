// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-TOOLS-OMNICODE-TRANSPILER
// Title:       OmniCode General Transpiler
// Type:        Header
// Component:   Tools / OmniCode Toolchain
// Role:        Parse general OmniCode and generate C with libtrit integration
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     General OmniCode transpiler supporting:
//              - 4-block C code generation
//              - libtrit type integration
//              - Verification of generated output
//
// Grounding:   Exodus 25:40 — "Look that thou make them after their pattern"
//              OmniCode is the pattern; C is the manifestation.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_OMNI_TRANSPILER_H
#define MOS_OMNI_TRANSPILER_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// Use the canonical OmniCode parser from pkg/omni
// This provides OmniDocument, OmniParser, and full AST infrastructure
#include "omni_parser.h"
#include "omni_chunks.h"

// MillenniumOS types (for mos_result_t, mos_size_t, etc.)
#include "../../kernel/types.h"

#include <stdio.h>
#include <stdbool.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Transpiler Error Types
// -----------------------------------------------------------------------------

typedef enum {
    OMNI_TRANS_OK = 0,
    OMNI_TRANS_ERR_NULL,
    OMNI_TRANS_ERR_FILE,
    OMNI_TRANS_ERR_PARSE,
    OMNI_TRANS_ERR_VALIDATE,
    OMNI_TRANS_ERR_EMIT,
    OMNI_TRANS_ERR_COMPILE,
    OMNI_TRANS_ERR_MEMORY
} OmniTranspileError;

// Error to string
const char* omni_trans_error_str(OmniTranspileError err);

// -----------------------------------------------------------------------------
// B.2 Transpiler Configuration
// -----------------------------------------------------------------------------

typedef struct {
    // Output control
    const char*     output_dir;         // Directory for generated files
    const char*     prefix;             // Prefix for generated identifiers

    // Generation flags
    bool            emit_header;        // Generate .h file
    bool            emit_source;        // Generate .c file
    bool            use_libtrit;        // Use libtrit types (trit_t, trit5_t, etc.)
    bool            use_mos_types;      // Use MillenniumOS types

    // Verification
    bool            verify_syntax;      // Validate OmniCode structure
    bool            verify_compile;     // Try to compile generated C
    const char*     compiler;           // Compiler to use (default: gcc)
    const char*     cflags;             // Extra compiler flags

    // Debug
    bool            verbose;            // Print progress
    bool            keep_intermediate;  // Keep intermediate files
} OmniTransConfig;

// Initialize default config
void omni_trans_config_init(OmniTransConfig* cfg);

// -----------------------------------------------------------------------------
// B.3 Transpiler State
// -----------------------------------------------------------------------------

typedef struct {
    // Parsed input (uses libomni parser)
    OmniDocument*       document;       // Full AST from libomni parser
    OmniParser          parser;         // Parser state
    OmniChunkLexer      chunk_lexer;    // Chunk lexer state
    char*               source_buffer;  // Owned copy of source
    mos_size_t          source_size;

    // Generated output
    char*               header_code;
    mos_size_t          header_size;
    char*               source_code;
    mos_size_t          output_size;

    // Verification results
    bool                syntax_valid;
    bool                compile_success;
    char*               compile_errors;
    mos_size_t          error_size;

    // Statistics
    int32_t             functions_generated;
    int32_t             types_generated;
    int32_t             lines_generated;
} OmniTransState;

// Initialize state
mos_result_t omni_trans_init(OmniTransState* state);

// Free state
void omni_trans_free(OmniTransState* state);

// -----------------------------------------------------------------------------
// B.4 Core Transpiler Functions
// -----------------------------------------------------------------------------

// Parse OmniCode file
OmniTranspileError omni_trans_parse(OmniTransState* state,
                                     const char* filepath);

// Parse from buffer
OmniTranspileError omni_trans_parse_buffer(OmniTransState* state,
                                            const char* buffer, mos_size_t size);

// Generate C code from parsed OmniCode
OmniTranspileError omni_trans_generate(OmniTransState* state,
                                        const OmniTransConfig* cfg);

// Write generated code to files
OmniTranspileError omni_trans_write(OmniTransState* state,
                                     const OmniTransConfig* cfg,
                                     const char* base_name);

// Verify generated code compiles
OmniTranspileError omni_trans_verify(OmniTransState* state,
                                      const OmniTransConfig* cfg);

// -----------------------------------------------------------------------------
// B.5 One-Shot Functions
// -----------------------------------------------------------------------------

// Transpile file to C (parse + generate + write)
OmniTranspileError omni_transpile_file(const char* input_path,
                                        const char* output_base,
                                        const OmniTransConfig* cfg);

// Transpile and verify (full pipeline)
OmniTranspileError omni_transpile_and_verify(const char* input_path,
                                              const char* output_base,
                                              const OmniTransConfig* cfg,
                                              OmniTransState* out_state);

// -----------------------------------------------------------------------------
// B.6 libtrit Integration
// -----------------------------------------------------------------------------

// Map OmniCode type hints to libtrit types
const char* omni_to_libtrit_type(const char* omni_type);

// Generate libtrit includes based on parsed document
void omni_emit_libtrit_includes(FILE* f, const OmniDocument* doc);

// Map OmniCode values to libtrit initializers
void omni_emit_libtrit_value(FILE* f, const char* type, const char* value);

// -----------------------------------------------------------------------------
// B.7 Verification Helpers
// -----------------------------------------------------------------------------
//
// Note: omni_validate_structure() is provided by libomni (omni_parser.h).
// These wrappers provide transpiler-specific error handling.
//

// Validate document structure (wraps libomni validation)
OmniTranspileError omni_trans_validate_structure(const OmniDocument* doc);

// Check for required pragma fields
OmniTranspileError omni_trans_validate_pragma(const OmniDocument* doc);

// Check block completeness
OmniTranspileError omni_trans_validate_blocks(const OmniDocument* doc);

// Compile generated C with gcc
OmniTranspileError omni_compile_check(const char* source_path,
                                       const OmniTransConfig* cfg,
                                       char** errors, mos_size_t* error_size);

// =============================================================================
// END BODY
// =============================================================================

#endif // MOS_OMNI_TRANSPILER_H

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// OmniCode Transpiler provides:
//   - General OmniCode parsing (4-block structure)
//   - C code generation with libtrit type integration
//   - Verification that generated code compiles
//
// Usage:
//   OmniTransConfig cfg;
//   OmniTransState state;
//
//   omni_trans_config_init(&cfg);
//   cfg.use_libtrit = true;
//   cfg.verify_compile = true;
//
//   OmniTranspileError err = omni_transpile_and_verify(
//       "example.omni", "example", &cfg, &state);
//
//   if (err == OMNI_TRANS_OK && state.compile_success) {
//       // Generated code is valid!
//   }
//
// "Look that thou make them after their pattern"
//   — Exodus 25:40
//
// =============================================================================
// END CLOSING
// =============================================================================
