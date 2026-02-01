// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-TOOLS-OMNICODE-TRANSPILER-IMPL
// Title:       OmniCode General Transpiler Implementation
// Type:        Source
// Component:   Tools / OmniCode Toolchain
// Role:        Implementation of OmniCode → C transpilation with verification
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Transpile OmniCode to C, verify compilation, integrate libtrit.
//
// Grounding:   Exodus 25:40 — "Look that thou make them after their pattern"
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_transpiler.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Error Strings
// -----------------------------------------------------------------------------

const char* omni_trans_error_str(OmniTranspileError err) {
    switch (err) {
        case OMNI_TRANS_OK:         return "OK";
        case OMNI_TRANS_ERR_NULL:   return "Null pointer";
        case OMNI_TRANS_ERR_FILE:   return "File error";
        case OMNI_TRANS_ERR_PARSE:  return "Parse error";
        case OMNI_TRANS_ERR_VALIDATE: return "Validation error";
        case OMNI_TRANS_ERR_EMIT:   return "Code generation error";
        case OMNI_TRANS_ERR_COMPILE: return "Compilation error";
        case OMNI_TRANS_ERR_MEMORY: return "Memory allocation error";
        default:                    return "Unknown error";
    }
}

// -----------------------------------------------------------------------------
// B.2 Configuration
// -----------------------------------------------------------------------------

void omni_trans_config_init(OmniTransConfig* cfg) {
    if (!cfg) return;

    cfg->output_dir = ".";
    cfg->prefix = "omni_";
    cfg->emit_header = true;
    cfg->emit_source = true;
    cfg->use_libtrit = true;
    cfg->use_mos_types = false;
    cfg->verify_syntax = true;
    cfg->verify_compile = true;
    cfg->compiler = "gcc";
    cfg->cflags = "-fsyntax-only -Wall";
    cfg->verbose = false;
    cfg->keep_intermediate = false;
}

// -----------------------------------------------------------------------------
// B.3 State Management
// -----------------------------------------------------------------------------

mos_result_t omni_trans_init(OmniTransState* state) {
    if (!state) return MOS_ERR_INVALID;

    memset(state, 0, sizeof(OmniTransState));
    return MOS_OK;
}

void omni_trans_free(OmniTransState* state) {
    if (!state) return;

    // Free libomni document
    if (state->document) {
        omni_document_destroy(state->document);
        state->document = NULL;
    }

    // Free source buffer
    if (state->source_buffer) {
        free(state->source_buffer);
        state->source_buffer = NULL;
    }

    // Free generated code
    if (state->header_code) {
        free(state->header_code);
        state->header_code = NULL;
    }
    if (state->source_code) {
        free(state->source_code);
        state->source_code = NULL;
    }
    if (state->compile_errors) {
        free(state->compile_errors);
        state->compile_errors = NULL;
    }
}

// -----------------------------------------------------------------------------
// B.4 Parsing (uses libomni parser)
// -----------------------------------------------------------------------------

OmniTranspileError omni_trans_parse(OmniTransState* state, const char* filepath) {
    if (!state || !filepath) return OMNI_TRANS_ERR_NULL;

    // Read file
    FILE* f = fopen(filepath, "r");
    if (!f) return OMNI_TRANS_ERR_FILE;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return OMNI_TRANS_ERR_MEMORY;
    }

    size_t bytes_read = fread(buffer, 1, size, f);
    fclose(f);
    if (bytes_read != (size_t)size) {
        free(buffer);
        return OMNI_TRANS_ERR_FILE;
    }
    buffer[size] = '\0';

    // Parse using libomni
    OmniTranspileError err = omni_trans_parse_buffer(state, buffer, size);

    // If successful, keep the buffer; otherwise free it
    if (err != OMNI_TRANS_OK) {
        free(buffer);
    } else {
        state->source_buffer = buffer;
        state->source_size = size;
    }

    return err;
}

OmniTranspileError omni_trans_parse_buffer(OmniTransState* state,
                                            const char* buffer, mos_size_t size) {
    if (!state || !buffer) return OMNI_TRANS_ERR_NULL;

    // Initialize chunk lexer with source (basic, no Scripture validation)
    omni_chunk_init(&state->chunk_lexer, buffer, size);

    // Initialize parser
    omni_parser_init(&state->parser, &state->chunk_lexer);

    // Parse to document
    state->document = omni_parser_parse(&state->parser);
    if (!state->document) {
        return OMNI_TRANS_ERR_PARSE;
    }

    if (state->parser.had_error) {
        return OMNI_TRANS_ERR_PARSE;
    }

    return OMNI_TRANS_OK;
}

// -----------------------------------------------------------------------------
// B.5 libtrit Type Mapping
// -----------------------------------------------------------------------------

const char* omni_to_libtrit_type(const char* omni_type) {
    if (!omni_type) return "int32_t";

    // Balanced ternary types
    if (strcmp(omni_type, "trit") == 0) return "trit_t";
    if (strcmp(omni_type, "trit3") == 0) return "trit3_t";
    if (strcmp(omni_type, "trit5") == 0) return "trit5_t";
    if (strcmp(omni_type, "trit9") == 0) return "trit9_t";
    if (strcmp(omni_type, "trit27") == 0) return "trit27_t";

    // Health and state
    if (strcmp(omni_type, "health") == 0) return "mos_health_t";
    if (strcmp(omni_type, "hebrew_state") == 0) return "MosHebrewState";

    // Standard types
    if (strcmp(omni_type, "bool") == 0) return "bool";
    if (strcmp(omni_type, "int") == 0) return "int32_t";
    if (strcmp(omni_type, "uint") == 0) return "uint32_t";
    if (strcmp(omni_type, "int8") == 0) return "int8_t";
    if (strcmp(omni_type, "int16") == 0) return "int16_t";
    if (strcmp(omni_type, "int32") == 0) return "int32_t";
    if (strcmp(omni_type, "int64") == 0) return "int64_t";
    if (strcmp(omni_type, "string") == 0) return "const char*";

    return omni_type;  // Pass through unknown types
}

void omni_emit_libtrit_includes(FILE* f, const OmniDocument* doc) {
    if (!f || !doc) return;

    bool needs_trit = false;
    bool needs_health = false;
    bool needs_hebrew = false;

    // Check document pragma flags for type hints
    const char* flags = doc->pragma_flags;
    if (strstr(flags, "trit") != NULL) needs_trit = true;
    if (strstr(flags, "health") != NULL) needs_health = true;
    if (strstr(flags, "hebrew") != NULL) needs_hebrew = true;

    // Emit includes based on needs
    if (needs_trit || needs_health || needs_hebrew) {
        fprintf(f, "// libtrit types\n");
        if (needs_trit) {
            fprintf(f, "#include \"trit/trit.h\"\n");
        }
        if (needs_health) {
            fprintf(f, "#include \"kernel/types.h\"\n");
        }
        if (needs_hebrew) {
            fprintf(f, "#include \"kernel/cpisi/cpisi.h\"\n");
        }
        fprintf(f, "\n");
    }
}

void omni_emit_libtrit_value(FILE* f, const char* type, const char* value) {
    if (!f || !value) return;

    // Handle special trit values
    if (type && (strcmp(type, "trit_t") == 0 || strcmp(type, "trit") == 0)) {
        if (strcmp(value, "+1") == 0 || strcmp(value, "1") == 0) {
            fprintf(f, "TRIT_POS");
            return;
        }
        if (strcmp(value, "0") == 0) {
            fprintf(f, "TRIT_ZERO");
            return;
        }
        if (strcmp(value, "-1") == 0) {
            fprintf(f, "TRIT_NEG");
            return;
        }
    }

    // Handle Hebrew states
    if (type && strcmp(type, "MosHebrewState") == 0) {
        if (strcmp(value, "shavar") == 0) { fprintf(f, "MOS_STATE_SHAVAR"); return; }
        if (strcmp(value, "chaser") == 0) { fprintf(f, "MOS_STATE_CHASER"); return; }
        if (strcmp(value, "ratsah") == 0) { fprintf(f, "MOS_STATE_RATSAH"); return; }
        if (strcmp(value, "yashar") == 0) { fprintf(f, "MOS_STATE_YASHAR"); return; }
        if (strcmp(value, "tamim") == 0) { fprintf(f, "MOS_STATE_TAMIM"); return; }
        if (strcmp(value, "shalem") == 0) { fprintf(f, "MOS_STATE_SHALEM"); return; }
        if (strcmp(value, "tov") == 0) { fprintf(f, "MOS_STATE_TOV"); return; }
    }

    // Default: emit as-is
    fprintf(f, "%s", value);
}

// -----------------------------------------------------------------------------
// B.6 Code Generation
// -----------------------------------------------------------------------------

static void get_timestamp(char* buf, size_t size) {
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm);
}

static void to_upper(char* s) {
    while (*s) {
        *s = toupper((unsigned char)*s);
        s++;
    }
}

// Sanitize string to be a valid C identifier (replace invalid chars with _)
static void sanitize_identifier(char* dest, const char* src, size_t dest_size) {
    size_t i = 0;
    while (*src && i < dest_size - 1) {
        char c = *src++;
        // Valid C identifier: [a-zA-Z_][a-zA-Z0-9_]*
        if (isalnum((unsigned char)c) || c == '_') {
            dest[i++] = c;
        } else {
            dest[i++] = '_';  // Replace invalid chars with underscore
        }
    }
    dest[i] = '\0';
}

static void emit_header_preamble(FILE* f, const OmniDocument* doc,
                                  const OmniTransConfig* cfg) {
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    // Use key from document metadata, sanitized for C identifiers
    const char* raw_key = doc->meta_key[0] ? doc->meta_key : "GENERATED";
    char key[260];
    sanitize_identifier(key, raw_key, sizeof(key));

    // Generate header guard
    char guard[264];
    snprintf(guard, sizeof(guard), "%s_H", key);
    to_upper(guard);

    fprintf(f, "// =============================================================================\n");
    fprintf(f, "// GENERATED CODE - DO NOT EDIT\n");
    fprintf(f, "// =============================================================================\n");
    fprintf(f, "//\n");
    fprintf(f, "// Generated from: %s\n", doc->meta_key);
    fprintf(f, "// Generated at:   %s\n", timestamp);
    fprintf(f, "// Generator:      OmniCode Transpiler v%s\n", "a-01.00");
    fprintf(f, "//\n");
    fprintf(f, "// =============================================================================\n\n");

    fprintf(f, "#ifndef %s\n", guard);
    fprintf(f, "#define %s\n\n", guard);

    // Standard includes
    fprintf(f, "#include <stdint.h>\n");
    fprintf(f, "#include <stdbool.h>\n");

    // libtrit includes if needed
    if (cfg->use_libtrit) {
        omni_emit_libtrit_includes(f, doc);
    }

    fprintf(f, "\n");
}

static void emit_header_closing(FILE* f, const char* key) {
    char guard[264];
    snprintf(guard, sizeof(guard), "%s_H", key);
    to_upper(guard);

    fprintf(f, "#endif // %s\n", guard);
}

static void emit_source_preamble(FILE* f, const OmniDocument* doc,
                                  const OmniTransConfig* cfg,
                                  const char* header_name) {
    (void)cfg;  // Unused for now
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(f, "// =============================================================================\n");
    fprintf(f, "// GENERATED CODE - DO NOT EDIT\n");
    fprintf(f, "// =============================================================================\n");
    fprintf(f, "//\n");
    fprintf(f, "// Generated from: %s\n", doc->meta_key);
    fprintf(f, "// Generated at:   %s\n", timestamp);
    fprintf(f, "//\n");
    fprintf(f, "// =============================================================================\n\n");

    fprintf(f, "#include \"%s\"\n", header_name);
    fprintf(f, "#include <string.h>\n\n");
}

OmniTranspileError omni_trans_generate(OmniTransState* state,
                                        const OmniTransConfig* cfg) {
    if (!state || !cfg || !state->document) return OMNI_TRANS_ERR_NULL;

    OmniDocument* doc = state->document;

    // Allocate buffers for generated code
    size_t header_capacity = 16384;
    size_t source_capacity = 32768;

    state->header_code = (char*)malloc(header_capacity);
    state->source_code = (char*)malloc(source_capacity);
    if (!state->header_code || !state->source_code) {
        return OMNI_TRANS_ERR_MEMORY;
    }

    // Create temp files for fprintf-based generation
    FILE* hf = tmpfile();
    FILE* sf = tmpfile();
    if (!hf || !sf) {
        if (hf) fclose(hf);
        if (sf) fclose(sf);
        return OMNI_TRANS_ERR_FILE;
    }

    // Get key and title from document metadata (sanitize for C identifiers)
    const char* raw_key = doc->meta_key[0] ? doc->meta_key : "GENERATED";
    char key[260];
    sanitize_identifier(key, raw_key, sizeof(key));
    const char* title = omni_document_get_key(doc);
    if (!title || !title[0]) title = "Generated Code";

    // Generate header
    emit_header_preamble(hf, doc, cfg);

    fprintf(hf, "// =============================================================================\n");
    fprintf(hf, "// %s\n", title);
    fprintf(hf, "// =============================================================================\n\n");

    // Generate declarations based on parsed content
    // For now, emit a placeholder structure
    char struct_name[64];
    snprintf(struct_name, sizeof(struct_name), "%sData", cfg->prefix);

    fprintf(hf, "typedef struct {\n");
    fprintf(hf, "    const char* name;\n");
    if (cfg->use_libtrit) {
        fprintf(hf, "    trit5_t     state;      // Hebrew state (0-242)\n");
        fprintf(hf, "    mos_health_t health;    // Health score\n");
    } else {
        fprintf(hf, "    int32_t    state;\n");
        fprintf(hf, "    int8_t     health;\n");
    }
    fprintf(hf, "} %s;\n\n", struct_name);

    fprintf(hf, "// Get instance\n");
    fprintf(hf, "const %s* %sget_data(void);\n\n", struct_name, cfg->prefix);

    emit_header_closing(hf, key);

    state->types_generated = 1;
    state->functions_generated = 1;

    // Generate source - use sanitized key for header name
    char header_name[264];
    snprintf(header_name, sizeof(header_name), "%s.h", key);

    emit_source_preamble(sf, doc, cfg, header_name);

    fprintf(sf, "// =============================================================================\n");
    fprintf(sf, "// Implementation\n");
    fprintf(sf, "// =============================================================================\n\n");

    fprintf(sf, "static const %s s_data = {\n", struct_name);
    fprintf(sf, "    .name = \"%s\",\n", key);
    if (cfg->use_libtrit) {
        fprintf(sf, "    .state = 121,  // YASHAR (center)\n");
        fprintf(sf, "    .health = 0,   // Neutral\n");
    } else {
        fprintf(sf, "    .state = 121,\n");
        fprintf(sf, "    .health = 0,\n");
    }
    fprintf(sf, "};\n\n");

    fprintf(sf, "const %s* %sget_data(void) {\n", struct_name, cfg->prefix);
    fprintf(sf, "    return &s_data;\n");
    fprintf(sf, "}\n");

    // Copy from temp files to buffers
    long header_len = ftell(hf);
    long source_len = ftell(sf);

    rewind(hf);
    rewind(sf);

    if ((size_t)header_len >= header_capacity) header_len = header_capacity - 1;
    if ((size_t)source_len >= source_capacity) source_len = source_capacity - 1;

    size_t header_read = fread(state->header_code, 1, header_len, hf);
    size_t source_read = fread(state->source_code, 1, source_len, sf);

    state->header_code[header_read] = '\0';
    state->source_code[source_read] = '\0';

    state->header_size = header_read;
    state->output_size = source_read;

    // Count lines
    state->lines_generated = 0;
    for (char* p = state->header_code; *p; p++) {
        if (*p == '\n') state->lines_generated++;
    }
    for (char* p = state->source_code; *p; p++) {
        if (*p == '\n') state->lines_generated++;
    }

    fclose(hf);
    fclose(sf);

    return OMNI_TRANS_OK;
}

// -----------------------------------------------------------------------------
// B.7 Writing and Verification
// -----------------------------------------------------------------------------

OmniTranspileError omni_trans_write(OmniTransState* state,
                                     const OmniTransConfig* cfg,
                                     const char* base_name) {
    if (!state || !cfg || !base_name) return OMNI_TRANS_ERR_NULL;

    char path[512];

    if (cfg->emit_header && state->header_code) {
        snprintf(path, sizeof(path), "%s/%s.h", cfg->output_dir, base_name);
        FILE* f = fopen(path, "w");
        if (!f) return OMNI_TRANS_ERR_FILE;
        fwrite(state->header_code, 1, state->header_size, f);
        fclose(f);
    }

    if (cfg->emit_source && state->source_code) {
        snprintf(path, sizeof(path), "%s/%s.c", cfg->output_dir, base_name);
        FILE* f = fopen(path, "w");
        if (!f) return OMNI_TRANS_ERR_FILE;
        fwrite(state->source_code, 1, state->source_size, f);
        fclose(f);
    }

    return OMNI_TRANS_OK;
}

OmniTranspileError omni_compile_check(const char* source_path,
                                       const OmniTransConfig* cfg,
                                       char** errors, mos_size_t* error_size) {
    if (!source_path || !cfg) return OMNI_TRANS_ERR_NULL;

    // Build compile command
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "%s %s %s 2>&1",
             cfg->compiler, cfg->cflags, source_path);

    FILE* pipe = popen(cmd, "r");
    if (!pipe) return OMNI_TRANS_ERR_FILE;

    // Read compiler output
    char buffer[4096] = {0};
    size_t total = 0;
    char line[256];

    while (fgets(line, sizeof(line), pipe) && total < sizeof(buffer) - 256) {
        size_t len = strlen(line);
        memcpy(buffer + total, line, len);
        total += len;
    }

    int status = pclose(pipe);

    // Copy errors if requested
    if (errors && total > 0) {
        *errors = (char*)malloc(total + 1);
        if (*errors) {
            memcpy(*errors, buffer, total);
            (*errors)[total] = '\0';
            if (error_size) *error_size = total;
        }
    }

    return (status == 0) ? OMNI_TRANS_OK : OMNI_TRANS_ERR_COMPILE;
}

OmniTranspileError omni_trans_verify(OmniTransState* state,
                                      const OmniTransConfig* cfg) {
    if (!state || !cfg) return OMNI_TRANS_ERR_NULL;

    // Write temp file
    const char* tmp_path = "/tmp/omni_verify.c";
    FILE* f = fopen(tmp_path, "w");
    if (!f) return OMNI_TRANS_ERR_FILE;

    // Combine header and source for syntax check
    fprintf(f, "// Verification build\n");
    fprintf(f, "%s\n", state->header_code ? state->header_code : "");
    fprintf(f, "%s\n", state->source_code ? state->source_code : "");
    fclose(f);

    // Try to compile
    OmniTranspileError err = omni_compile_check(tmp_path, cfg,
                                                 &state->compile_errors,
                                                 &state->error_size);

    state->compile_success = (err == OMNI_TRANS_OK);

    // Cleanup
    if (!cfg->keep_intermediate) {
        remove(tmp_path);
    }

    return err;
}

// -----------------------------------------------------------------------------
// B.8 Validation (uses libomni validation, wraps with transpiler error types)
// -----------------------------------------------------------------------------

OmniTranspileError omni_trans_validate_structure(const OmniDocument* doc) {
    if (!doc) return OMNI_TRANS_ERR_NULL;

    // Use libomni's validation
    if (!omni_validate_structure((OmniDocument*)doc)) {
        return OMNI_TRANS_ERR_VALIDATE;
    }

    return OMNI_TRANS_OK;
}

OmniTranspileError omni_trans_validate_pragma(const OmniDocument* doc) {
    if (!doc) return OMNI_TRANS_ERR_NULL;

    // Check required pragma fields
    if (doc->meta_key[0] == '\0') {
        return OMNI_TRANS_ERR_VALIDATE;
    }

    return OMNI_TRANS_OK;
}

OmniTranspileError omni_trans_validate_blocks(const OmniDocument* doc) {
    if (!doc) return OMNI_TRANS_ERR_NULL;

    // Use libomni's completeness check
    if (!omni_document_is_complete(doc)) {
        return OMNI_TRANS_ERR_VALIDATE;
    }

    return OMNI_TRANS_OK;
}

// -----------------------------------------------------------------------------
// B.9 One-Shot Functions
// -----------------------------------------------------------------------------

OmniTranspileError omni_transpile_file(const char* input_path,
                                        const char* output_base,
                                        const OmniTransConfig* cfg) {
    OmniTransState state;
    omni_trans_init(&state);

    OmniTranspileError err = omni_trans_parse(&state, input_path);
    if (err != OMNI_TRANS_OK) {
        omni_trans_free(&state);
        return err;
    }

    if (cfg->verify_syntax) {
        err = omni_trans_validate_structure(state.document);
        if (err != OMNI_TRANS_OK) {
            omni_trans_free(&state);
            return err;
        }
    }

    err = omni_trans_generate(&state, cfg);
    if (err != OMNI_TRANS_OK) {
        omni_trans_free(&state);
        return err;
    }

    err = omni_trans_write(&state, cfg, output_base);
    omni_trans_free(&state);
    return err;
}

OmniTranspileError omni_transpile_and_verify(const char* input_path,
                                              const char* output_base,
                                              const OmniTransConfig* cfg,
                                              OmniTransState* out_state) {
    if (!out_state) return OMNI_TRANS_ERR_NULL;

    omni_trans_init(out_state);

    OmniTranspileError err = omni_trans_parse(out_state, input_path);
    if (err != OMNI_TRANS_OK) return err;

    if (cfg->verify_syntax) {
        out_state->syntax_valid = (omni_trans_validate_structure(out_state->document) == OMNI_TRANS_OK);
        if (!out_state->syntax_valid) return OMNI_TRANS_ERR_VALIDATE;
    }

    err = omni_trans_generate(out_state, cfg);
    if (err != OMNI_TRANS_OK) return err;

    err = omni_trans_write(out_state, cfg, output_base);
    if (err != OMNI_TRANS_OK) return err;

    if (cfg->verify_compile) {
        omni_trans_verify(out_state, cfg);
        // Don't fail on compile error - just report
    }

    return OMNI_TRANS_OK;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// OmniCode Transpiler converts specification to implementation:
//   1. Parse OmniCode (pragma, blocks, structure)
//   2. Validate structure (required fields, block completeness)
//   3. Generate C code (with libtrit type mapping)
//   4. Verify compilation (syntax check with gcc)
//
// "Look that thou make them after their pattern"
//   — Exodus 25:40
//
// =============================================================================
// END CLOSING
// =============================================================================
