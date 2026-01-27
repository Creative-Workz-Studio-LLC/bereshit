// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-codegen-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_codegen.c — OmniCode to C Code Generator Implementation
// Translates OmniCode AST to C code using libtrit types.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-codegen-impl
// Title: OmniCode Code Generator Implementation
// Type: Source
// Role: Implement OmniCode → C code translation with type/value mappings

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:3 - "Let there be light: and there was light"
// Principle: Words that CREATE - sentences become executable code
// Anchor: John 1:1-3 - "All things were made by him"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "omni_codegen.h"

// CPI-SI state-aware logging
#include "kernel/cpisi/dar/detect.h"

// # S.1 Internal Helpers [HELPERS]

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

// # S.2 Type Mapping Table [TYPE_TABLE]
//
// OmniCode types → C types (with libtrit where applicable)

static const OmniTypeMapping type_mappings[] = {
    // -------------------------------------------------------------------------
    // Integer types (count)
    // -------------------------------------------------------------------------
    {"count",    "int64_t",  "0",     "<stdint.h>"},
    {"count8",   "int8_t",   "0",     "<stdint.h>"},
    {"count16",  "int16_t",  "0",     "<stdint.h>"},
    {"count32",  "int32_t",  "0",     "<stdint.h>"},
    {"count64",  "int64_t",  "0",     "<stdint.h>"},

    // -------------------------------------------------------------------------
    // Unsigned types (whole)
    // -------------------------------------------------------------------------
    {"whole",    "uint64_t", "0",     "<stdint.h>"},
    {"whole8",   "uint8_t",  "0",     "<stdint.h>"},
    {"whole16",  "uint16_t", "0",     "<stdint.h>"},
    {"whole32",  "uint32_t", "0",     "<stdint.h>"},
    {"whole64",  "uint64_t", "0",     "<stdint.h>"},

    // -------------------------------------------------------------------------
    // Floating point types (measure)
    // -------------------------------------------------------------------------
    {"measure",   "double",  "0.0",   NULL},
    {"measure32", "float",   "0.0f",  NULL},
    {"measure64", "double",  "0.0",   NULL},

    // -------------------------------------------------------------------------
    // Text types (word, char, scroll)
    // -------------------------------------------------------------------------
    {"word",   "const char*", "\"\"",    NULL},
    {"char",   "uint32_t",    "0",       "<stdint.h>"},  // Unicode codepoint
    {"scroll", "const char*", "\"\"",    NULL},

    // -------------------------------------------------------------------------
    // Logical types (truth, covenant)
    // -------------------------------------------------------------------------
    {"truth",    "bool",  "false",       "<stdbool.h>"},
    {"covenant", "trit_t",  "TRIT_ZERO",   "\"trit.h\""},   // libtrit!

    // -------------------------------------------------------------------------
    // Sentinel
    // -------------------------------------------------------------------------
    {NULL, NULL, NULL, NULL}
};

// # S.3 Value Mapping Table [VALUE_TABLE]
//
// OmniCode literal values → C values

static const OmniValueMapping value_mappings[] = {
    // -------------------------------------------------------------------------
    // Boolean values (truth type)
    // -------------------------------------------------------------------------
    {"yes",  "true"},
    {"no",   "false"},

    // -------------------------------------------------------------------------
    // Covenant values (ternary - maps to libtrit!)
    // -------------------------------------------------------------------------
    {"granted",  "TRIT_POS"},     // +1
    {"deferred", "TRIT_ZERO"},    //  0
    {"denied",   "TRIT_NEG"},     // -1

    // -------------------------------------------------------------------------
    // Special values
    // -------------------------------------------------------------------------
    {"none", "NULL"},

    // -------------------------------------------------------------------------
    // Sentinel
    // -------------------------------------------------------------------------
    {NULL, NULL}
};

// # S.4 Operator Mapping Table [OP_TABLE]
//
// OmniCode operators → C operators

static const OmniOpMapping op_mappings[] = {
    // -------------------------------------------------------------------------
    // Arithmetic operators
    // -------------------------------------------------------------------------
    {"plus",            "+",    6,  true},
    {"minus",           "-",    6,  true},
    {"multiplied by",   "*",    5,  true},
    {"divided by",      "/",    5,  true},
    {"modulo",          "%",    5,  true},
    {"to the power of", "pow",  4,  true},  // Function call

    // -------------------------------------------------------------------------
    // Comparison operators
    // -------------------------------------------------------------------------
    {"is equal to",         "==", 8,  true},
    {"is not equal to",     "!=", 8,  true},
    {"is greater than",     ">",  7,  true},
    {"is less than",        "<",  7,  true},
    {"is at least",         ">=", 7,  true},
    {"is at most",          "<=", 7,  true},

    // -------------------------------------------------------------------------
    // Logical operators
    // -------------------------------------------------------------------------
    {"and",  "&&", 9,  true},
    {"or",   "||", 10, true},
    {"not",  "!",  3,  false},

    // -------------------------------------------------------------------------
    // String operators
    // -------------------------------------------------------------------------
    {"joined with", "strcat", 6, true},  // Function call

    // -------------------------------------------------------------------------
    // Access operators
    // -------------------------------------------------------------------------
    {"at",   "[",  1, true},   // Array index

    // -------------------------------------------------------------------------
    // Sentinel
    // -------------------------------------------------------------------------
    {NULL, NULL, 0, false}
};

// # S.5 Log Prefix Constants [LOG_PREFIXES]

static const char* LOG_PREFIX_INFO    = "[INFO]";
static const char* LOG_PREFIX_WARNING = "[WARN]";
static const char* LOG_PREFIX_ERROR   = "[ERROR]";
static const char* LOG_PREFIX_DEBUG   = "[DEBUG]";

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Lifecycle [LIFECYCLE]

void omni_codegen_init(OmniCodeGen* gen, FILE* output) {
    if (!gen) return;

    memset(gen, 0, sizeof(*gen));
    gen->output = output ? output : stdout;
    gen->own_output = false;
    gen->indent_level = 0;
    gen->indent_width = 4;
    gen->at_line_start = true;
    gen->health = 0;  // Start at deferred
    gen->emit_comments = true;
    gen->emit_debug = false;
    gen->emit_health = false;
}

bool omni_codegen_init_file(OmniCodeGen* gen, const char* path) {
    if (!gen || !path) return false;

    FILE* f = fopen(path, "w");
    if (!f) return false;

    omni_codegen_init(gen, f);
    gen->own_output = true;
    return true;
}

void omni_codegen_destroy(OmniCodeGen* gen) {
    if (!gen) return;

    if (gen->output) {
        fflush(gen->output);
        if (gen->own_output && gen->output != stdout && gen->output != stderr) {
            fclose(gen->output);
        }
    }
    gen->output = NULL;
}

void omni_codegen_reset(OmniCodeGen* gen) {
    if (!gen) return;

    gen->indent_level = 0;
    gen->at_line_start = true;
    gen->in_function = false;
    gen->in_block = false;
    gen->current_fn = NULL;
    gen->scope_depth = 0;
    gen->doc = NULL;
    gen->functions_emitted = 0;
    gen->statements_emitted = 0;
    gen->lines_emitted = 0;
    gen->error_count = 0;
    gen->warning_count = 0;
    gen->health = 0;
}

// # B.2 Configuration [CONFIG]

void omni_codegen_set_indent(OmniCodeGen* gen, int width) {
    if (gen && width > 0 && width <= 8) {
        gen->indent_width = width;
    }
}

void omni_codegen_set_comments(OmniCodeGen* gen, bool enable) {
    if (gen) gen->emit_comments = enable;
}

void omni_codegen_set_debug(OmniCodeGen* gen, bool enable) {
    if (gen) gen->emit_debug = enable;
}

void omni_codegen_set_health(OmniCodeGen* gen, bool enable) {
    if (gen) gen->emit_health = enable;
}

// # B.3 Type Mapping [TYPE_MAPPING]

const OmniTypeMapping* omni_type_map_lookup(const char* omni_type) {
    if (!omni_type) return NULL;

    for (int i = 0; type_mappings[i].omni_type != NULL; i++) {
        if (strcmp(omni_type, type_mappings[i].omni_type) == 0) {
            return &type_mappings[i];
        }
    }
    return NULL;
}

const char* omni_type_to_c(const char* omni_type) {
    const OmniTypeMapping* mapping = omni_type_map_lookup(omni_type);
    return mapping ? mapping->c_type : "/* unknown type */";
}

const char* omni_type_default_c(const char* omni_type) {
    const OmniTypeMapping* mapping = omni_type_map_lookup(omni_type);
    return mapping ? mapping->c_default : "0";
}

const char* omni_type_include_c(const char* omni_type) {
    const OmniTypeMapping* mapping = omni_type_map_lookup(omni_type);
    return mapping ? mapping->include : NULL;
}

// # B.4 Value Mapping [VALUE_MAPPING]

const char* omni_value_to_c(const char* omni_value) {
    if (!omni_value) return "NULL";

    for (int i = 0; value_mappings[i].omni_value != NULL; i++) {
        if (strcmp(omni_value, value_mappings[i].omni_value) == 0) {
            return value_mappings[i].c_value;
        }
    }
    return omni_value;  // Return as-is if unknown
}

const char* omni_covenant_to_c(const char* covenant_state) {
    if (!covenant_state) return "TRIT_ZERO";

    if (strcmp(covenant_state, "granted") == 0) return "TRIT_POS";
    if (strcmp(covenant_state, "deferred") == 0) return "TRIT_ZERO";
    if (strcmp(covenant_state, "denied") == 0) return "TRIT_NEG";

    return "TRIT_ZERO";  // Default to deferred
}

// # B.5 Operator Mapping [OP_MAPPING]

const OmniOpMapping* omni_op_map_lookup(const char* omni_op) {
    if (!omni_op) return NULL;

    for (int i = 0; op_mappings[i].omni_op != NULL; i++) {
        if (strcmp(omni_op, op_mappings[i].omni_op) == 0) {
            return &op_mappings[i];
        }
    }
    return NULL;
}

const char* omni_op_to_c(const char* omni_op) {
    const OmniOpMapping* mapping = omni_op_map_lookup(omni_op);
    return mapping ? mapping->c_op : "/* unknown op */";
}

// # B.6 Emission Helpers [EMISSION]

void omni_emit_raw(OmniCodeGen* gen, const char* text) {
    if (!gen || !gen->output || !text) return;
    fputs(text, gen->output);
    gen->at_line_start = false;
}

void omni_emit_fmt(OmniCodeGen* gen, const char* fmt, ...) {
    if (!gen || !gen->output || !fmt) return;

    va_list args;
    va_start(args, fmt);
    vfprintf(gen->output, fmt, args);
    va_end(args);
    gen->at_line_start = false;
}

void omni_emit_indent(OmniCodeGen* gen) {
    if (!gen || !gen->output) return;

    int spaces = gen->indent_level * gen->indent_width;
    for (int i = 0; i < spaces; i++) {
        fputc(' ', gen->output);
    }
    gen->at_line_start = false;
}

void omni_emit_line(OmniCodeGen* gen, const char* line) {
    if (!gen || !gen->output) return;

    omni_emit_indent(gen);
    if (line) fputs(line, gen->output);
    fputc('\n', gen->output);
    gen->at_line_start = true;
    gen->lines_emitted++;
}

void omni_emit_linef(OmniCodeGen* gen, const char* fmt, ...) {
    if (!gen || !gen->output || !fmt) return;

    omni_emit_indent(gen);

    va_list args;
    va_start(args, fmt);
    vfprintf(gen->output, fmt, args);
    va_end(args);

    fputc('\n', gen->output);
    gen->at_line_start = true;
    gen->lines_emitted++;
}

void omni_emit_blank(OmniCodeGen* gen) {
    if (!gen || !gen->output) return;
    fputc('\n', gen->output);
    gen->at_line_start = true;
    gen->lines_emitted++;
}

void omni_emit_comment(OmniCodeGen* gen, const char* text) {
    if (!gen || !gen->emit_comments) return;
    omni_emit_linef(gen, "// %s", text ? text : "");
}

void omni_emit_block_comment(OmniCodeGen* gen, const char* text) {
    if (!gen || !gen->emit_comments) return;
    omni_emit_linef(gen, "/* %s */", text ? text : "");
}

void omni_emit_block_start(OmniCodeGen* gen) {
    if (!gen) return;
    omni_emit_line(gen, "{");
    gen->indent_level++;
    gen->in_block = true;
}

void omni_emit_block_end(OmniCodeGen* gen) {
    if (!gen) return;
    if (gen->indent_level > 0) gen->indent_level--;
    omni_emit_line(gen, "}");
    gen->in_block = (gen->indent_level > 0);
}

// # B.7 Statement Emission [STATEMENTS]

void omni_emit_var_decl(OmniCodeGen* gen, const char* c_type,
                        const char* name, const char* init_value) {
    if (!gen || !c_type || !name) return;

    if (init_value && *init_value) {
        omni_emit_linef(gen, "%s %s = %s;", c_type, name, init_value);
    } else {
        omni_emit_linef(gen, "%s %s;", c_type, name);
    }
    gen->statements_emitted++;
}

void omni_emit_func_start(OmniCodeGen* gen, const char* c_return_type,
                          const char* name, const char* params) {
    if (!gen || !name) return;

    const char* ret = c_return_type ? c_return_type : "void";
    const char* par = params ? params : "void";

    omni_emit_blank(gen);
    omni_emit_linef(gen, "%s %s(%s)", ret, name, par);
    omni_emit_block_start(gen);

    gen->in_function = true;
    gen->current_fn = name;
    gen->functions_emitted++;
}

void omni_emit_func_end(OmniCodeGen* gen) {
    if (!gen) return;

    omni_emit_block_end(gen);
    gen->in_function = false;
    gen->current_fn = NULL;
}

void omni_emit_if_start(OmniCodeGen* gen, const char* condition) {
    if (!gen || !condition) return;
    omni_emit_linef(gen, "if (%s)", condition);
    omni_emit_block_start(gen);
}

void omni_emit_else_if(OmniCodeGen* gen, const char* condition) {
    if (!gen || !condition) return;
    if (gen->indent_level > 0) gen->indent_level--;
    omni_emit_linef(gen, "} else if (%s) {", condition);
    gen->indent_level++;
}

void omni_emit_else(OmniCodeGen* gen) {
    if (!gen) return;
    if (gen->indent_level > 0) gen->indent_level--;
    omni_emit_line(gen, "} else {");
    gen->indent_level++;
}

void omni_emit_return(OmniCodeGen* gen, const char* value) {
    if (!gen) return;

    if (value && *value) {
        omni_emit_linef(gen, "return %s;", value);
    } else {
        omni_emit_line(gen, "return;");
    }
    gen->statements_emitted++;
}

void omni_emit_witness(OmniCodeGen* gen, const char* message, const char* level) {
    if (!gen || !message) return;

    const char* prefix = LOG_PREFIX_INFO;
    if (level) {
        if (strcmp(level, "warning") == 0) prefix = LOG_PREFIX_WARNING;
        else if (strcmp(level, "error") == 0) prefix = LOG_PREFIX_ERROR;
        else if (strcmp(level, "debug") == 0) prefix = LOG_PREFIX_DEBUG;
    }

    omni_emit_linef(gen, "printf(\"%s %s\\n\");", prefix, message);
    gen->statements_emitted++;
}

// # B.8 Error Handling [ERRORS]

void omni_codegen_error(OmniCodeGen* gen, int line, int col,
                        const char* fmt, ...) {
    if (!gen || gen->error_count >= CODEGEN_MAX_ERRORS) return;

    va_list args;
    va_start(args, fmt);
    vsnprintf(gen->errors[gen->error_count].message,
              CODEGEN_ERROR_SIZE, fmt, args);
    va_end(args);

    gen->errors[gen->error_count].line = line;
    gen->errors[gen->error_count].column = col;
    gen->error_count++;

    // Errors hurt health
    omni_codegen_health_adjust(gen, -20, "error");
}

void omni_codegen_warning(OmniCodeGen* gen, int line, int col,
                          const char* fmt, ...) {
    if (!gen) return;

    // Build full message: "line:col: <msg>"
    char full_msg[512];
    int prefix_len = snprintf(full_msg, sizeof(full_msg), "%d:%d: ", line, col);

    va_list args;
    va_start(args, fmt);
    vsnprintf(full_msg + prefix_len, sizeof(full_msg) - prefix_len, fmt, args);
    va_end(args);

    LOG_WARN("codegen", "%s", full_msg);

    gen->warning_count++;
    omni_codegen_health_adjust(gen, -5, "warning");
}

int omni_codegen_error_count(const OmniCodeGen* gen) {
    return gen ? gen->error_count : 0;
}

int omni_codegen_warning_count(const OmniCodeGen* gen) {
    return gen ? gen->warning_count : 0;
}

void omni_codegen_print_errors(const OmniCodeGen* gen) {
    if (!gen) return;

    for (int i = 0; i < gen->error_count; i++) {
        LOG_ERROR("codegen", "error:%d:%d: %s",
                gen->errors[i].line,
                gen->errors[i].column,
                gen->errors[i].message);
    }
}

// # B.9 Health Tracking [HEALTH]

void omni_codegen_health_adjust(OmniCodeGen* gen, int delta, const char* reason) {
    if (!gen) return;

    gen->health += delta;

    // Clamp to [-100, +100]
    if (gen->health > 100) gen->health = 100;
    if (gen->health < -100) gen->health = -100;

    // Debug logging if enabled
    if (gen->emit_debug && reason) {
        LOG_DEBUG("codegen", "[HEALTH] %+d (%s) → %d", delta, reason, gen->health);
    }
}

int omni_codegen_health_get(const OmniCodeGen* gen) {
    return gen ? gen->health : 0;
}

const char* omni_codegen_health_state(const OmniCodeGen* gen) {
    if (!gen) return "unknown";

    if (gen->health > 50) return "granted";
    if (gen->health < -50) return "denied";
    return "deferred";
}

// # B.10 Queries [QUERIES]

const char* omni_codegen_result_name(OmniCodeGenResult result) {
    switch (result) {
        case CODEGEN_GRANTED:  return "granted";
        case CODEGEN_DEFERRED: return "deferred";
        case CODEGEN_DENIED:   return "denied";
        default:               return "unknown";
    }
}

int omni_codegen_lines(const OmniCodeGen* gen) {
    return gen ? gen->lines_emitted : 0;
}

int omni_codegen_functions(const OmniCodeGen* gen) {
    return gen ? gen->functions_emitted : 0;
}

// # B.11 Code Generation [GENERATION]

OmniCodeGenResult omni_codegen_emit_header(OmniCodeGen* gen) {
    if (!gen || !gen->doc) return CODEGEN_DENIED;

    // File header comment
    omni_emit_comment(gen, "Generated from OmniCode");
    omni_emit_comment(gen, "\"Let there be light\" — Genesis 1:3");
    omni_emit_blank(gen);

    // Standard includes
    omni_emit_line(gen, "#include <stdio.h>");
    omni_emit_line(gen, "#include <stdlib.h>");
    omni_emit_line(gen, "#include <stdint.h>");
    omni_emit_line(gen, "#include <stdbool.h>");
    omni_emit_line(gen, "#include <string.h>");

    // libtrit include for covenant type
    omni_emit_blank(gen);
    omni_emit_comment(gen, "libtrit for covenant (ternary) type");
    omni_emit_line(gen, "#include \"trit.h\"");

    omni_emit_blank(gen);

    omni_codegen_health_adjust(gen, +10, "header emitted");
    return CODEGEN_GRANTED;
}

// Helper: emit a function and its body statements
// Returns pointer to next node after function body ends
// Uses column position to detect scope (indentation-based)
static OmniAstNode* emit_function_with_body(OmniCodeGen* gen, OmniAstNode* func_node) {
    if (!gen || !func_node || func_node->type != AST_FUNC_DEF) return func_node;

    // Remember function's column for scope detection
    int func_column = func_node->column;

    // Emit function signature
    const char* func_name = func_node->value;
    const char* params = func_node->secondary;
    const char* ret_type = func_node->type_name;

    const char* c_ret = (ret_type && ret_type[0]) ? omni_type_to_c(ret_type) : "void";

    char c_params[256] = "void";
    if (params && params[0]) {
        char type_buf[64] = "";
        char name_buf[64] = "";
        if (sscanf(params, "%63s %63s", type_buf, name_buf) == 2) {
            const char* c_param_type = omni_type_to_c(type_buf);
            snprintf(c_params, sizeof(c_params), "%s %s", c_param_type, name_buf);
        }
    }

    omni_emit_func_start(gen, c_ret, func_name, c_params);

    // Emit body statements until we exit function scope
    // Scope detection: statements at column <= func_column are outside function
    OmniAstNode* node = func_node->next_sibling;
    while (node != NULL) {
        // Stop at next function definition
        if (node->type == AST_FUNC_DEF) {
            break;
        }

        // Skip block markers
        if (node->type == AST_BLOCK) {
            node = node->next_sibling;
            continue;
        }

        // Scope detection: if statement is at same or lesser column, it's outside function
        // (unless it's an OTHERWISE which continues the current block)
        if (node->column > 0 && node->column <= func_column &&
            node->type != AST_OTHERWISE_STMT) {
            break;  // Exit function scope
        }

        // Emit the statement
        omni_emit_node(gen, node);
        node = node->next_sibling;
    }

    // Close any open blocks inside the function
    while (gen->indent_level > 1) {
        omni_emit_block_end(gen);
    }

    omni_emit_func_end(gen);

    return node;  // Return next node after function body
}

// Helper: find BODY block and return its content
// The AST structure has content as CHILDREN of the BODY START block, not siblings.
static void find_body_range(OmniAstNode* root, OmniAstNode** start, OmniAstNode** end) {
    *start = NULL;
    *end = NULL;

    if (!root) return;

    // Find BODY block - content is its first_child
    for (OmniAstNode* n = root->first_child; n != NULL; n = n->next_sibling) {
        if (n->type == AST_BLOCK && n->block_type == BLOCK_BODY) {
            // Check if this is START (has children) vs END (no children)
            if (n->first_child != NULL) {
                *start = n->first_child;
                // end stays NULL - we'll iterate until next_sibling is NULL
                return;
            }
        }
    }

    // Fallback: search by node value containing "BODY"
    for (OmniAstNode* n = root->first_child; n != NULL; n = n->next_sibling) {
        if (n->type == AST_BLOCK &&
            (strstr(n->value, "BODY") != NULL || strstr(n->value, "Body") != NULL)) {
            if (n->first_child != NULL) {
                *start = n->first_child;
                return;
            }
        }
    }
}

OmniCodeGenResult omni_codegen_emit_body(OmniCodeGen* gen) {
    if (!gen || !gen->doc || !gen->doc->root) return CODEGEN_DENIED;

    omni_emit_comment(gen, "OmniCode program body");
    omni_emit_blank(gen);

    // Find BODY section - content is children of BODY block node
    OmniAstNode* body_start = NULL;
    OmniAstNode* body_end = NULL;  // Unused - iterate until next_sibling is NULL
    find_body_range(gen->doc->root, &body_start, &body_end);

    if (!body_start) {
        omni_codegen_warning(gen, 0, 0, "No BODY block found in document");
        omni_emit_func_start(gen, "int", "main", "void");
        omni_emit_return(gen, "0");
        omni_emit_func_end(gen);
        return CODEGEN_DEFERRED;
    }

    // Start at first node in BODY
    OmniAstNode* node = body_start;

    // Find first function (within BODY range)
    OmniAstNode* first_func = NULL;
    for (OmniAstNode* n = node; n != NULL && n != body_end; n = n->next_sibling) {
        if (n->type == AST_FUNC_DEF) {
            first_func = n;
            break;
        }
    }

    // =========================================================================
    // PASS 0: Emit global variables at file scope (before functions)
    // =========================================================================
    omni_emit_comment(gen, "Global variables");
    for (OmniAstNode* n = node; n != NULL && n != first_func && n != body_end; n = n->next_sibling) {
        if (n->type == AST_LET_STMT) {
            omni_emit_node(gen, n);
        }
    }
    omni_emit_blank(gen);

    // =========================================================================
    // PASS 1: Emit all function definitions at file scope
    // =========================================================================
    // emit_function_with_body returns the next node after function body
    // Stop when we hit a non-function node (that's where top-level logic begins)
    OmniAstNode* func_node = first_func;
    while (func_node != NULL && func_node != body_end && func_node->type == AST_FUNC_DEF) {
        func_node = emit_function_with_body(gen, func_node);
    }
    // func_node now points to first non-function after all functions (top-level logic)

    // =========================================================================
    // PASS 2: Emit main() with top-level logic
    // =========================================================================
    omni_emit_func_start(gen, "int", "main", "void");

    // Emit top-level logic (all statements after functions)
    // func_node points to first non-function after all functions
    for (OmniAstNode* n = func_node; n != NULL && n != body_end; n = n->next_sibling) {
        // Skip block markers
        if (n->type == AST_BLOCK) continue;

        // Emit all remaining statements (these are top-level logic in main)
        omni_emit_node(gen, n);
    }

    // Close any open blocks
    while (gen->indent_level > 1) {
        omni_emit_block_end(gen);
    }

    omni_emit_return(gen, "0");
    omni_emit_func_end(gen);

    omni_codegen_health_adjust(gen, +20, "body emitted");
    return CODEGEN_GRANTED;
}

OmniCodeGenResult omni_codegen_emit_footer(OmniCodeGen* gen) {
    if (!gen) return CODEGEN_DENIED;

    omni_emit_blank(gen);
    omni_emit_comment(gen, "\"And there was light.\" — Genesis 1:3");

    omni_codegen_health_adjust(gen, +10, "footer emitted");
    return CODEGEN_GRANTED;
}

OmniCodeGenResult omni_codegen_generate(OmniCodeGen* gen, OmniDocument* doc) {
    if (!gen || !doc) {
        return CODEGEN_DENIED;
    }

    omni_codegen_reset(gen);
    gen->doc = doc;

    OmniCodeGenResult result = CODEGEN_GRANTED;

    // Emit header
    if (omni_codegen_emit_header(gen) == CODEGEN_DENIED) {
        omni_codegen_error(gen, 0, 0, "Failed to emit header");
        result = CODEGEN_DENIED;
    }

    // Emit body
    if (result != CODEGEN_DENIED) {
        if (omni_codegen_emit_body(gen) == CODEGEN_DENIED) {
            omni_codegen_error(gen, 0, 0, "Failed to emit body");
            result = CODEGEN_DENIED;
        }
    }

    // Emit footer
    if (result != CODEGEN_DENIED) {
        if (omni_codegen_emit_footer(gen) == CODEGEN_DENIED) {
            omni_codegen_error(gen, 0, 0, "Failed to emit footer");
            result = CODEGEN_DENIED;
        }
    }

    // Determine final result based on health
    if (result != CODEGEN_DENIED) {
        if (gen->health > 50) {
            result = CODEGEN_GRANTED;
        } else if (gen->health < -50) {
            result = CODEGEN_DENIED;
        } else {
            result = CODEGEN_DEFERRED;
        }
    }

    return result;
}

// # B.12 AST Node Emission [AST_EMISSION]
//
// Full AST walking for executable code generation.
// Dispatches to specialized emitters based on node type.

// Forward declarations for statement emitters
static OmniCodeGenResult emit_let_stmt(OmniCodeGen* gen, OmniAstNode* node);
static OmniCodeGenResult emit_func_def(OmniCodeGen* gen, OmniAstNode* node);
static OmniCodeGenResult emit_when_stmt(OmniCodeGen* gen, OmniAstNode* node);
static OmniCodeGenResult emit_otherwise_stmt(OmniCodeGen* gen, OmniAstNode* node);
static OmniCodeGenResult emit_return_stmt(OmniCodeGen* gen, OmniAstNode* node);
static OmniCodeGenResult emit_witness_stmt(OmniCodeGen* gen, OmniAstNode* node);
static OmniCodeGenResult emit_display_stmt(OmniCodeGen* gen, OmniAstNode* node);
static OmniCodeGenResult emit_fail_stmt(OmniCodeGen* gen, OmniAstNode* node);

// Helper: translate OmniCode expression to C
// For now, handles simple translations; full expression parsing later
static void translate_expr_to_c(char* buf, size_t buf_size, const char* omni_expr) {
    if (!buf || buf_size == 0 || !omni_expr) return;

    // Start with the original
    safe_strcpy(buf, buf_size, omni_expr);

    // Translate value literals
    const char* c_val = omni_value_to_c(omni_expr);
    if (c_val != omni_expr) {  // If translation found
        safe_strcpy(buf, buf_size, c_val);
        return;
    }

    // For complex expressions, do simple word replacements
    // TODO: Full expression parser for proper AST-based translation
    // Use fixed size that fits within output buffer (with room for operators)
    // 120 + 120 + 16 (formatting) = 256 max
    char lhs[120];
    char rhs_buf[120];
    safe_strcpy(lhs, sizeof(lhs), buf);

    // Replace operators - find first match and split
    char* pos;
    if ((pos = strstr(lhs, " plus ")) != NULL) {
        *pos = '\0';
        safe_strcpy(rhs_buf, sizeof(rhs_buf), pos + 6);
        snprintf(buf, buf_size, "(%s + %s)", lhs, rhs_buf);
    } else if ((pos = strstr(lhs, " minus ")) != NULL) {
        *pos = '\0';
        safe_strcpy(rhs_buf, sizeof(rhs_buf), pos + 7);
        snprintf(buf, buf_size, "(%s - %s)", lhs, rhs_buf);
    } else if ((pos = strstr(lhs, " multiplied by ")) != NULL) {
        *pos = '\0';
        safe_strcpy(rhs_buf, sizeof(rhs_buf), pos + 15);
        snprintf(buf, buf_size, "(%s * %s)", lhs, rhs_buf);
    } else if ((pos = strstr(lhs, " divided by ")) != NULL) {
        *pos = '\0';
        safe_strcpy(rhs_buf, sizeof(rhs_buf), pos + 12);
        snprintf(buf, buf_size, "(%s / %s)", lhs, rhs_buf);
    } else if ((pos = strstr(lhs, " is greater than ")) != NULL) {
        *pos = '\0';
        safe_strcpy(rhs_buf, sizeof(rhs_buf), pos + 17);
        snprintf(buf, buf_size, "(%s > %s)", lhs, rhs_buf);
    } else if ((pos = strstr(lhs, " is less than ")) != NULL) {
        *pos = '\0';
        safe_strcpy(rhs_buf, sizeof(rhs_buf), pos + 14);
        snprintf(buf, buf_size, "(%s < %s)", lhs, rhs_buf);
    } else if ((pos = strstr(lhs, " is equal to ")) != NULL) {
        *pos = '\0';
        safe_strcpy(rhs_buf, sizeof(rhs_buf), pos + 13);
        snprintf(buf, buf_size, "(%s == %s)", lhs, rhs_buf);
    } else if ((pos = strstr(lhs, " is ")) != NULL) {
        *pos = '\0';
        // "status is granted" → check covenant value
        const char* rhs = pos + 4;
        if (strcmp(rhs, "granted") == 0) {
            snprintf(buf, buf_size, "(trit_value(%s) == 1)", lhs);
        } else if (strcmp(rhs, "deferred") == 0) {
            snprintf(buf, buf_size, "(trit_value(%s) == 0)", lhs);
        } else if (strcmp(rhs, "denied") == 0) {
            snprintf(buf, buf_size, "(trit_value(%s) == -1)", lhs);
        } else {
            snprintf(buf, buf_size, "(%s == %s)", lhs, omni_value_to_c(rhs));
        }
    }
}

OmniCodeGenResult omni_emit_node(OmniCodeGen* gen, OmniAstNode* node) {
    if (!gen || !node) return CODEGEN_DENIED;

    // Dispatch based on node type
    switch (node->type) {
        // Block structure
        case AST_BLOCK:
            return omni_emit_block(gen, node);

        // Statements
        case AST_LET_STMT:
            return emit_let_stmt(gen, node);

        case AST_FUNC_DEF:
            return emit_func_def(gen, node);

        case AST_WHEN_STMT:
            return emit_when_stmt(gen, node);

        case AST_OTHERWISE_STMT:
            return emit_otherwise_stmt(gen, node);

        case AST_RETURN_STMT:
            return emit_return_stmt(gen, node);

        case AST_WITNESS_STMT:
            return emit_witness_stmt(gen, node);

        case AST_DISPLAY_STMT:
            return emit_display_stmt(gen, node);

        case AST_FAIL_STMT:
            return emit_fail_stmt(gen, node);

        // Skip certain node types (metadata, etc.)
        case AST_DOCUMENT:
        case AST_PRAGMA:
        case AST_ENTITY_DECL:
        case AST_IDENTITY:
        case AST_IMPORT_STMT:
        case AST_RAW:
            return CODEGEN_GRANTED;  // Silently skip

        default:
            // Unhandled node type - skip with warning
            omni_codegen_warning(gen, node->line, node->column,
                                 "Unhandled node type: %s",
                                 omni_ast_type_name(node->type));
            return CODEGEN_DEFERRED;
    }
}

OmniCodeGenResult omni_emit_block(OmniCodeGen* gen, OmniAstNode* block) {
    if (!gen || !block) return CODEGEN_DENIED;

    // Only emit code from BODY block
    if (block->block_type == BLOCK_BODY) {
        // Walk children
        for (OmniAstNode* child = block->first_child; child; child = child->next_sibling) {
            omni_emit_node(gen, child);
        }
    }

    return CODEGEN_GRANTED;
}

// # B.12.1 Statement Emitters [STMT_EMITTERS]

// Helper: check if type is a string type
static bool is_string_type(const char* omni_type) {
    return omni_type && (strcmp(omni_type, "word") == 0 ||
                         strcmp(omni_type, "scroll") == 0);
}

// Helper: wrap value in quotes if it's a string literal (no existing quotes)
static void wrap_string_literal(char* buf, size_t buf_size, const char* value) {
    if (!buf || buf_size < 3 || !value) return;

    // Check if already quoted
    size_t len = strlen(value);
    if (len >= 2 && value[0] == '"' && value[len-1] == '"') {
        safe_strcpy(buf, buf_size, value);
        return;
    }

    // Add quotes
    snprintf(buf, buf_size, "\"%s\"", value);
}

static OmniCodeGenResult emit_let_stmt(OmniCodeGen* gen, OmniAstNode* node) {
    if (!gen || !node) return CODEGEN_DENIED;

    // node->value = variable name
    // node->type_name = OmniCode type
    // node->secondary = initial value (if any)

    const char* var_name = node->value;
    const char* omni_type = node->type_name;
    const char* init_val = node->secondary;

    // Get C type
    const char* c_type = omni_type_to_c(omni_type);

    // Translate initial value to C
    char c_init[256] = "";
    if (init_val && init_val[0]) {
        // For string types, wrap in quotes
        if (is_string_type(omni_type)) {
            wrap_string_literal(c_init, sizeof(c_init), init_val);
        } else {
            translate_expr_to_c(c_init, sizeof(c_init), init_val);
        }
    } else {
        // Use default for type
        safe_strcpy(c_init, sizeof(c_init), omni_type_default_c(omni_type));
    }

    omni_emit_var_decl(gen, c_type, var_name, c_init);
    return CODEGEN_GRANTED;
}

static OmniCodeGenResult emit_func_def(OmniCodeGen* gen, OmniAstNode* node) {
    // Functions are handled by emit_function_with_body in the two-pass approach
    // This is called during omni_emit_node dispatch but should be skipped
    // since functions are emitted at file scope in pass 1
    (void)gen;
    (void)node;
    return CODEGEN_GRANTED;
}

static OmniCodeGenResult emit_when_stmt(OmniCodeGen* gen, OmniAstNode* node) {
    if (!gen || !node) return CODEGEN_DENIED;

    // node->value = condition string ("status is granted", "amount is greater than health")

    char c_cond[256];
    translate_expr_to_c(c_cond, sizeof(c_cond), node->value);

    omni_emit_if_start(gen, c_cond);
    return CODEGEN_GRANTED;
}

static OmniCodeGenResult emit_otherwise_stmt(OmniCodeGen* gen, OmniAstNode* node) {
    if (!gen || !node) return CODEGEN_DENIED;

    omni_emit_else(gen);
    return CODEGEN_GRANTED;
}

// Helper: detect if value looks like a string literal
// Heuristic: contains commas/exclamation/spaces without operators = probably string
static bool looks_like_string_literal(const char* value) {
    if (!value || !value[0]) return false;

    // Already quoted
    size_t len = strlen(value);
    if (len >= 2 && value[0] == '"' && value[len-1] == '"') return false;

    // Check for common string patterns
    if (strchr(value, '!') != NULL) return true;
    if (strchr(value, ',') != NULL && strstr(value, " minus ") == NULL) return true;

    // Check if it's NOT an operator expression or variable name
    // If it has spaces but no operators, probably a string
    if (strchr(value, ' ') != NULL &&
        strstr(value, " plus ") == NULL &&
        strstr(value, " minus ") == NULL &&
        strstr(value, " is ") == NULL &&
        strstr(value, " multiplied ") == NULL &&
        strstr(value, " divided ") == NULL) {
        return true;
    }

    return false;
}

static OmniCodeGenResult emit_return_stmt(OmniCodeGen* gen, OmniAstNode* node) {
    if (!gen || !node) return CODEGEN_DENIED;

    // node->value = return expression

    if (node->value[0]) {
        char c_val[256];

        // Check if this looks like a string literal
        if (looks_like_string_literal(node->value)) {
            wrap_string_literal(c_val, sizeof(c_val), node->value);
        } else {
            translate_expr_to_c(c_val, sizeof(c_val), node->value);
        }

        omni_emit_return(gen, c_val);
    } else {
        omni_emit_return(gen, NULL);
    }
    return CODEGEN_GRANTED;
}

static OmniCodeGenResult emit_witness_stmt(OmniCodeGen* gen, OmniAstNode* node) {
    if (!gen || !node) return CODEGEN_DENIED;

    // node->value = message
    // node->secondary = level (info, warning, error, debug)

    omni_emit_witness(gen, node->value, node->secondary);
    return CODEGEN_GRANTED;
}

static OmniCodeGenResult emit_display_stmt(OmniCodeGen* gen, OmniAstNode* node) {
    if (!gen || !node) return CODEGEN_DENIED;

    // node->value = value to display

    // For now, emit printf with %s for strings, %lld for numbers
    // TODO: Type-aware formatting
    omni_emit_linef(gen, "printf(\"%%s\\n\", %s);", node->value);
    gen->statements_emitted++;
    return CODEGEN_GRANTED;
}

static OmniCodeGenResult emit_fail_stmt(OmniCodeGen* gen, OmniAstNode* node) {
    if (!gen || !node) return CODEGEN_DENIED;

    // node->value = error message

    omni_emit_linef(gen, "fprintf(stderr, \"Error: %s\\n\");", node->value);
    omni_emit_linef(gen, "exit(1);");
    gen->statements_emitted += 2;
    return CODEGEN_GRANTED;
}

OmniCodeGenResult omni_emit_statement(OmniCodeGen* gen, OmniAstNode* stmt) {
    // Dispatch to omni_emit_node
    return omni_emit_node(gen, stmt);
}

OmniCodeGenResult omni_emit_expression(OmniCodeGen* gen, OmniAstNode* expr) {
    (void)gen;
    (void)expr;
    // TODO: Implement full expression emission
    return CODEGEN_DEFERRED;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Validation [VALIDATION]
//
// Build:
//   gcc -Wall -Wextra -c omni_codegen.c -I../include
//
// Test:
//   Link with demo program to verify code generation

// # X.2 Closing Note [NOTE]
//
// "Let there be light: and there was light."
//   — Genesis 1:3
//
// Words that CREATE. OmniCode sentences become executable C code.
// The covenant type maps directly to libtrit's trit_t:
//   granted → TRIT_POS (+1)
//   deferred → TRIT_ZERO (0)
//   denied → TRIT_NEG (-1)
//
// This is not abstraction - this is implementation of divine pattern.

// =============================================================================
// END CLOSING
// =============================================================================
