// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ir-emit
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_ir_emit.c — AST to IR Emitter
// Transforms OmniCode AST into IR for VM execution.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-ir-emit
// Title: AST to IR Emitter
// Type: Source
// Role: Bridge AST representation to executable IR

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:3 - "And God said... and there was"
// Principle: Words (AST) become light (IR) - the bridge from structure to execution
// Anchor: John 1:14 - "The Word became flesh" - abstract becomes concrete

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#define _GNU_SOURCE  // For strcasecmp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "omni_ir.h"
#include "omni_parser.h"
#include "omni_lib.h"

// Safe copy with explicit null termination (truncation is intentional)
static void safe_copy(char* dest, size_t dest_size, const char* src) {
    if (!dest || !src || dest_size == 0) return;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
    memcpy(dest, src, copy_len);
    dest[copy_len] = '\0';
}

// # S.1 Emitter State [STATE]

typedef struct {
    OmniIRProgram*  prog;
    OmniDocument*   doc;

    // Variable tracking (simple name → global index mapping)
    char            var_names[64][64];
    uint8_t         var_count;

    // Current function being emitted
    uint16_t        current_func;
    bool            in_function;

    // For tracking function column (scope detection)
    int             func_column;

    // Import tracking (module name → module index)
    char            import_names[32][64];
    uint8_t         import_count;

    // Library loader (optional, for resolving imports)
    OmniLibLoader*  lib_loader;
} OmniIREmitter;

// # S.2 Helpers [HELPERS]

// Forward declaration for intrinsic opcode lookup (used in emit_let_stmt)
static uint8_t intrinsic_opcode(const char* name);

static int find_var(OmniIREmitter* emit, const char* name) {
    for (int i = 0; i < emit->var_count; i++) {
        if (strcmp(emit->var_names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

static int add_var(OmniIREmitter* emit, const char* name) {
    int idx = find_var(emit, name);
    if (idx >= 0) return idx;

    if (emit->var_count >= 64) return -1;

    safe_copy(emit->var_names[emit->var_count], 64, name);
    return emit->var_count++;
}

// Translate OmniCode value to IR constant
static uint16_t emit_value(OmniIREmitter* emit, const char* value, const char* type_name) {
    (void)type_name;  // May use for type-specific handling later

    if (!value || !value[0]) {
        return omni_ir_add_int(emit->prog, 0);
    }

    // Check for covenant values
    if (strcmp(value, "granted") == 0) {
        return omni_ir_add_trit(emit->prog, +1);
    }
    if (strcmp(value, "deferred") == 0) {
        return omni_ir_add_trit(emit->prog, 0);
    }
    if (strcmp(value, "denied") == 0) {
        return omni_ir_add_trit(emit->prog, -1);
    }

    // Check for boolean values
    if (strcmp(value, "yes") == 0 || strcmp(value, "true") == 0) {
        return omni_ir_add_bool(emit->prog, true);
    }
    if (strcmp(value, "no") == 0 || strcmp(value, "false") == 0) {
        return omni_ir_add_bool(emit->prog, false);
    }

    // Check if it's a number
    char* end;
    long num = strtol(value, &end, 10);
    if (*end == '\0') {
        return omni_ir_add_int(emit->prog, num);
    }

    // Check if it's a float
    double fnum = strtod(value, &end);
    if (*end == '\0') {
        return omni_ir_add_float(emit->prog, fnum);
    }

    // It's a string
    // Remove quotes if present
    if (value[0] == '"') {
        size_t len = strlen(value);
        if (len >= 2 && value[len-1] == '"') {
            char* unquoted = malloc(len - 1);
            strncpy(unquoted, value + 1, len - 2);
            unquoted[len - 2] = '\0';
            uint16_t idx = omni_ir_add_string(emit->prog, unquoted);
            free(unquoted);
            return idx;
        }
    }

    return omni_ir_add_string(emit->prog, value);
}

// Parse and emit an expression
static void emit_expression(OmniIREmitter* emit, const char* expr);

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Expression Emission [EXPR]

static void emit_expression(OmniIREmitter* emit, const char* expr) {
    if (!emit || !expr || !expr[0]) return;

    // Skip leading whitespace
    while (*expr && isspace(*expr)) expr++;

    // Check for binary operators (work right to left for correct precedence on stack)
    const char* op;

    // Check for "is greater than"
    op = strstr(expr, " is greater than ");
    if (op) {
        char left[128] = "";
        strncpy(left, expr, op - expr);
        left[op - expr] = '\0';
        const char* right = op + 18;

        emit_expression(emit, left);
        emit_expression(emit, right);
        omni_ir_emit(emit->prog, OP_GT);
        return;
    }

    // Check for "is less than"
    op = strstr(expr, " is less than ");
    if (op) {
        char left[128] = "";
        strncpy(left, expr, op - expr);
        left[op - expr] = '\0';
        const char* right = op + 14;

        emit_expression(emit, left);
        emit_expression(emit, right);
        omni_ir_emit(emit->prog, OP_LT);
        return;
    }

    // Check for "is"
    op = strstr(expr, " is ");
    if (op) {
        char left[128] = "";
        strncpy(left, expr, op - expr);
        left[op - expr] = '\0';
        const char* right = op + 4;

        emit_expression(emit, left);
        emit_expression(emit, right);
        omni_ir_emit(emit->prog, OP_EQ);
        return;
    }

    // Check for "plus"
    op = strstr(expr, " plus ");
    if (op) {
        char left[128] = "";
        strncpy(left, expr, op - expr);
        left[op - expr] = '\0';
        const char* right = op + 6;

        emit_expression(emit, left);
        emit_expression(emit, right);
        omni_ir_emit(emit->prog, OP_ADD);
        return;
    }

    // Check for "minus"
    op = strstr(expr, " minus ");
    if (op) {
        char left[128] = "";
        strncpy(left, expr, op - expr);
        left[op - expr] = '\0';
        const char* right = op + 7;

        emit_expression(emit, left);
        emit_expression(emit, right);
        omni_ir_emit(emit->prog, OP_SUB);
        return;
    }

    // Not a binary expression - check if it's a variable reference
    int var_idx = find_var(emit, expr);
    if (var_idx >= 0) {
        omni_ir_emit_1(emit->prog, OP_PUSH_GLOBAL, var_idx);
        return;
    }

    // It's a literal value
    uint16_t const_idx = emit_value(emit, expr, NULL);
    omni_ir_emit_wide(emit->prog, OP_PUSH_CONST, const_idx);
}

// # B.2 Statement Emission [STMT]

static void emit_let_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = variable name
    // node->secondary = initial value (may be literal, variable, or intrinsic call)
    // node->type_name = type

    int var_idx = add_var(emit, node->value);
    if (var_idx < 0) return;

    // Check if the value is an intrinsic call
    // Format: "intrinsic_name arg1 arg2 ..." or just "value"
    char value_copy[256];
    strncpy(value_copy, node->secondary, sizeof(value_copy) - 1);
    value_copy[sizeof(value_copy) - 1] = '\0';

    // Extract first word to check if it's an intrinsic
    char* first_word = strtok(value_copy, " ");
    if (first_word) {
        uint8_t opcode = intrinsic_opcode(first_word);
        if (opcode != 0) {
            // This is an intrinsic call: "let x be count with str_len source"
            // Emit intrinsic arguments first
            char* arg = strtok(NULL, " ");
            while (arg != NULL) {
                int arg_var_idx = find_var(emit, arg);
                if (arg_var_idx >= 0) {
                    omni_ir_emit_1(emit->prog, OP_PUSH_GLOBAL, arg_var_idx);
                } else {
                    uint16_t const_idx = emit_value(emit, arg, NULL);
                    omni_ir_emit_wide(emit->prog, OP_PUSH_CONST, const_idx);
                }
                arg = strtok(NULL, " ");
            }

            // Emit the intrinsic opcode (pushes result to stack)
            omni_ir_emit(emit->prog, opcode);

            // Store result to variable
            omni_ir_emit_1(emit->prog, OP_STORE_GLOBAL, var_idx);
            return;
        }
    }

    // Not an intrinsic - check if it's a variable reference
    int src_var_idx = find_var(emit, node->secondary);
    if (src_var_idx >= 0) {
        // Copy from another variable
        omni_ir_emit_1(emit->prog, OP_PUSH_GLOBAL, src_var_idx);
    } else {
        // Push literal value as constant
        uint16_t const_idx = emit_value(emit, node->secondary, node->type_name);
        omni_ir_emit_wide(emit->prog, OP_PUSH_CONST, const_idx);
    }

    // Store to global
    omni_ir_emit_1(emit->prog, OP_STORE_GLOBAL, var_idx);
}

static void emit_return_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = return expression
    emit_expression(emit, node->value);
    omni_ir_emit(emit->prog, OP_RETURN);
}

static void emit_witness_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = message
    // node->secondary = level (info, error, debug)

    uint16_t msg_idx = emit_value(emit, node->value, "word");

    int8_t level = 0;  // info
    if (node->secondary[0]) {
        if (strcmp(node->secondary, "error") == 0) level = -1;
        else if (strcmp(node->secondary, "debug") == 0) level = +1;
    }

    omni_ir_emit_3(emit->prog, OP_WITNESS,
                   (msg_idx >> 8) & 0xFF, msg_idx & 0xFF, level);
}

static void emit_display_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = what to display
    int var_idx = find_var(emit, node->value);
    if (var_idx >= 0) {
        omni_ir_emit_1(emit->prog, OP_PUSH_GLOBAL, var_idx);
        omni_ir_emit(emit->prog, OP_PRINT);
    } else {
        uint16_t const_idx = emit_value(emit, node->value, NULL);
        omni_ir_emit_wide(emit->prog, OP_PRINT_STR, const_idx);
    }
    omni_ir_emit(emit->prog, OP_PRINT_NL);
}

static void emit_fail_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    uint16_t msg_idx = emit_value(emit, node->value, "word");
    omni_ir_emit_wide(emit->prog, OP_FAIL, msg_idx);
}

// # B.2.5 CPI-SI State Emission [CPISI]

static void emit_health_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = operation (get, add, set)
    // node->secondary = value (for add/set)

    if (strcmp(node->value, "get") == 0) {
        omni_ir_emit(emit->prog, OP_HEALTH_GET);
    } else if (strcmp(node->value, "add") == 0) {
        int8_t delta = (int8_t)atoi(node->secondary);
        omni_ir_emit_1(emit->prog, OP_HEALTH_ADD, (uint8_t)delta);
    } else if (strcmp(node->value, "set") == 0) {
        int8_t value = (int8_t)atoi(node->secondary);
        omni_ir_emit_1(emit->prog, OP_HEALTH_SET, (uint8_t)value);
    }
}

static void emit_state_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = operation (get, transition)
    // node->secondary = direction (expand/retreat/maintain or +1/-1/0)

    if (strcmp(node->value, "get") == 0) {
        omni_ir_emit(emit->prog, OP_STATE_GET);
    } else if (strcmp(node->value, "transition") == 0) {
        int8_t k_factor = 0;  // Default: maintain

        // Parse direction - can be text or number
        if (strcasecmp(node->secondary, "expand") == 0 ||
            strcmp(node->secondary, "+1") == 0 ||
            strcmp(node->secondary, "1") == 0) {
            k_factor = +1;
        } else if (strcasecmp(node->secondary, "retreat") == 0 ||
                   strcmp(node->secondary, "-1") == 0) {
            k_factor = -1;
        } else if (strcasecmp(node->secondary, "maintain") == 0 ||
                   strcmp(node->secondary, "0") == 0) {
            k_factor = 0;
        }

        omni_ir_emit_1(emit->prog, OP_STATE_TRANS, (uint8_t)k_factor);
    }
}

static void emit_checkpoint_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = checkpoint name (optional)
    uint16_t name_idx = 0;
    if (node->value[0]) {
        name_idx = omni_ir_add_string(emit->prog, node->value);
    }
    omni_ir_emit_wide(emit->prog, OP_CHECKPOINT, name_idx);
}

static void emit_restore_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = checkpoint name to restore (optional)
    uint16_t name_idx = 0;
    if (node->value[0]) {
        name_idx = omni_ir_add_string(emit->prog, node->value);
    }
    omni_ir_emit_wide(emit->prog, OP_RESTORE, name_idx);
}

// # B.2.8 VM Intrinsic Statement [INTRINSIC]

// Map intrinsic names to opcodes
static uint8_t intrinsic_opcode(const char* name) {
    // String intrinsics
    if (strcasecmp(name, "str_len") == 0) return OP_STR_LEN;
    if (strcasecmp(name, "str_char") == 0) return OP_STR_CHAR;
    if (strcasecmp(name, "str_sub") == 0) return OP_STR_SUB;
    if (strcasecmp(name, "str_find") == 0) return OP_STR_FIND;
    if (strcasecmp(name, "str_concat") == 0 || strcasecmp(name, "str_cat") == 0) return OP_STR_CAT;
    if (strcasecmp(name, "str_eq") == 0) return OP_STR_EQ;
    if (strcasecmp(name, "str_cmp") == 0) return OP_STR_CMP;
    if (strcasecmp(name, "str_split") == 0) return OP_STR_SPLIT;
    if (strcasecmp(name, "str_join") == 0) return OP_STR_JOIN;
    if (strcasecmp(name, "str_trim") == 0) return OP_STR_TRIM;
    if (strcasecmp(name, "str_upper") == 0) return OP_STR_UPPER;
    if (strcasecmp(name, "str_lower") == 0) return OP_STR_LOWER;
    if (strcasecmp(name, "str_starts") == 0) return OP_STR_STARTS;
    if (strcasecmp(name, "str_ends") == 0) return OP_STR_ENDS;
    if (strcasecmp(name, "str_replace") == 0) return OP_STR_REPLACE;

    // List intrinsics
    if (strcasecmp(name, "list_new") == 0) return OP_LIST_NEW;
    if (strcasecmp(name, "list_push") == 0) return OP_LIST_PUSH;
    if (strcasecmp(name, "list_pop") == 0) return OP_LIST_POP;
    if (strcasecmp(name, "list_get") == 0) return OP_LIST_GET;
    if (strcasecmp(name, "list_set") == 0) return OP_LIST_SET;
    if (strcasecmp(name, "list_len") == 0) return OP_LIST_LEN;
    if (strcasecmp(name, "list_find") == 0) return OP_LIST_FIND;
    if (strcasecmp(name, "list_clear") == 0) return OP_LIST_CLEAR;

    // Map intrinsics
    if (strcasecmp(name, "map_new") == 0) return OP_MAP_NEW;
    if (strcasecmp(name, "map_get") == 0) return OP_MAP_GET;
    if (strcasecmp(name, "map_set") == 0) return OP_MAP_SET;
    if (strcasecmp(name, "map_has") == 0) return OP_MAP_HAS;
    if (strcasecmp(name, "map_del") == 0) return OP_MAP_DEL;
    if (strcasecmp(name, "map_keys") == 0) return OP_MAP_KEYS;
    if (strcasecmp(name, "map_len") == 0) return OP_MAP_LEN;

    // File intrinsics
    if (strcasecmp(name, "file_open") == 0) return OP_FILE_OPEN;
    if (strcasecmp(name, "file_close") == 0) return OP_FILE_CLOSE;
    if (strcasecmp(name, "file_read") == 0) return OP_FILE_READ;
    if (strcasecmp(name, "file_write") == 0) return OP_FILE_WRITE;
    if (strcasecmp(name, "file_exists") == 0) return OP_FILE_EXISTS;
    if (strcasecmp(name, "file_size") == 0) return OP_FILE_SIZE;
    if (strcasecmp(name, "file_delete") == 0) return OP_FILE_DELETE;
    if (strcasecmp(name, "file_readline") == 0) return OP_FILE_READLN;
    if (strcasecmp(name, "file_writeline") == 0) return OP_FILE_WRITELN;
    if (strcasecmp(name, "dir_list") == 0) return OP_DIR_LIST;
    if (strcasecmp(name, "dir_create") == 0) return OP_DIR_CREATE;
    if (strcasecmp(name, "dir_exists") == 0) return OP_DIR_EXISTS;

    return 0;  // Unknown intrinsic
}

static void emit_intrinsic_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = intrinsic name (e.g., "str_len")
    // node->secondary = arguments (space-separated, e.g., "s" or "a b")

    uint8_t opcode = intrinsic_opcode(node->value);
    if (opcode == 0) {
        fprintf(stderr, "IR emit: unknown intrinsic '%s'\n", node->value);
        return;
    }

    // Push arguments onto stack in order
    // Parse space-separated args from node->secondary
    char args[256];
    strncpy(args, node->secondary, sizeof(args) - 1);
    args[sizeof(args) - 1] = '\0';

    char* arg = strtok(args, " ");
    while (arg != NULL) {
        // Try to find as variable first
        int var_idx = find_var(emit, arg);
        if (var_idx >= 0) {
            omni_ir_emit_1(emit->prog, OP_PUSH_GLOBAL, var_idx);
        } else {
            // Treat as literal value
            uint16_t const_idx = emit_value(emit, arg, NULL);
            omni_ir_emit_wide(emit->prog, OP_PUSH_CONST, const_idx);
        }
        arg = strtok(NULL, " ");
    }

    // Emit the intrinsic opcode
    omni_ir_emit(emit->prog, opcode);
}

// # B.2.9 Import Statement [IMPORT]

static void emit_import_stmt(OmniIREmitter* emit, OmniAstNode* node) {
    // node->value = import name (e.g., "math")
    // node->secondary = import path (e.g., "stdlib/math")

    if (emit->import_count >= 32) {
        fprintf(stderr, "IR emit: too many imports\n");
        return;
    }

    // Store import name for later resolution
    safe_copy(emit->import_names[emit->import_count], 64, node->value);

    // Emit OP_IMPORT with path string index
    uint16_t path_idx = omni_ir_add_string(emit->prog, node->secondary);
    omni_ir_emit_wide(emit->prog, OP_IMPORT, path_idx);

    emit->import_count++;
}

// Forward declaration
static void emit_node(OmniIREmitter* emit, OmniAstNode* node);

static uint32_t emit_when_stmt(OmniIREmitter* emit, OmniAstNode* node,
                                OmniAstNode** next_node) {
    // node->value = condition expression
    // Following nodes until OTHERWISE or scope exit are the then-branch

    // Emit condition
    emit_expression(emit, node->value);

    // Jump if condition is not positive (trit semantics)
    uint32_t jump_else = omni_ir_ip(emit->prog);
    omni_ir_emit_wide(emit->prog, OP_JUMP_NEG, 0);  // Will patch

    // Emit then-branch statements
    OmniAstNode* n = node->next_sibling;
    int when_column = node->column;

    while (n != NULL) {
        // Stop at OTHERWISE
        if (n->type == AST_OTHERWISE_STMT) {
            break;
        }
        // Stop if we exit scope (back to when's column or less, except blocks)
        if (n->type != AST_BLOCK && n->column > 0 && n->column <= when_column) {
            break;
        }
        // Skip blocks
        if (n->type == AST_BLOCK) {
            n = n->next_sibling;
            continue;
        }

        emit_node(emit, n);
        n = n->next_sibling;
    }

    // Jump over else branch
    uint32_t jump_end = omni_ir_ip(emit->prog);
    omni_ir_emit_wide(emit->prog, OP_JUMP, 0);  // Will patch

    // Patch else jump
    uint32_t else_addr = omni_ir_ip(emit->prog);
    omni_ir_patch_jump(emit->prog, jump_else, else_addr);

    // Emit else-branch if OTHERWISE exists
    if (n != NULL && n->type == AST_OTHERWISE_STMT) {
        n = n->next_sibling;  // Skip OTHERWISE marker

        while (n != NULL) {
            // Stop if we exit scope
            if (n->type != AST_BLOCK && n->column > 0 && n->column <= when_column) {
                break;
            }
            if (n->type == AST_BLOCK) {
                n = n->next_sibling;
                continue;
            }

            emit_node(emit, n);
            n = n->next_sibling;
        }
    }

    // Patch end jump
    uint32_t end_addr = omni_ir_ip(emit->prog);
    omni_ir_patch_jump(emit->prog, jump_end, end_addr);

    *next_node = n;
    return end_addr;
}

// # B.3 Node Emission [NODE]

static void emit_node(OmniIREmitter* emit, OmniAstNode* node) {
    if (!emit || !node) return;

    switch (node->type) {
        case AST_LET_STMT:
            emit_let_stmt(emit, node);
            break;

        case AST_RETURN_STMT:
            emit_return_stmt(emit, node);
            break;

        case AST_WITNESS_STMT:
            emit_witness_stmt(emit, node);
            break;

        case AST_DISPLAY_STMT:
            emit_display_stmt(emit, node);
            break;

        case AST_FAIL_STMT:
            emit_fail_stmt(emit, node);
            break;

        // CPI-SI State operations
        case AST_HEALTH_STMT:
            emit_health_stmt(emit, node);
            break;

        case AST_STATE_STMT:
            emit_state_stmt(emit, node);
            break;

        // DAR operations
        case AST_CHECKPOINT_STMT:
            emit_checkpoint_stmt(emit, node);
            break;

        case AST_RESTORE_STMT:
            emit_restore_stmt(emit, node);
            break;

        // VM Intrinsics
        case AST_INTRINSIC_STMT:
            emit_intrinsic_stmt(emit, node);
            break;

        // Module operations
        case AST_IMPORT_STMT:
            emit_import_stmt(emit, node);
            break;

        // WHEN and OTHERWISE handled specially (grouped together)
        case AST_WHEN_STMT:
        case AST_OTHERWISE_STMT:
        case AST_FUNC_DEF:
        case AST_BLOCK:
            // These are handled by the traversal logic
            break;

        default:
            break;
    }
}

// # B.4 Body Emission [BODY]

// Find SETUP block content (for imports)
static OmniAstNode* find_setup_start(OmniDocument* doc) {
    if (!doc || !doc->root) return NULL;

    for (OmniAstNode* n = doc->root->first_child; n != NULL; n = n->next_sibling) {
        if (n->type == AST_BLOCK && n->block_type == BLOCK_SETUP) {
            if (n->first_child != NULL) {
                return n->first_child;
            }
        }
    }
    return NULL;
}

// Find BODY block content (same logic as codegen)
static OmniAstNode* find_body_start(OmniDocument* doc) {
    if (!doc || !doc->root) return NULL;

    for (OmniAstNode* n = doc->root->first_child; n != NULL; n = n->next_sibling) {
        if (n->type == AST_BLOCK && n->block_type == BLOCK_BODY) {
            if (n->first_child != NULL) {
                return n->first_child;
            }
        }
    }
    return NULL;
}

// # B.5 Public API [API]

OmniIRProgram* omni_ir_from_ast(struct OmniDocument* doc) {
    if (!doc) return NULL;

    OmniIRProgram* prog = omni_ir_create();
    if (!prog) return NULL;

    OmniIREmitter emit = {0};
    emit.prog = prog;
    emit.doc = doc;

    // Process SETUP block for imports first
    OmniAstNode* setup_start = find_setup_start(doc);
    if (setup_start) {
        OmniAstNode* setup_node = setup_start;
        while (setup_node != NULL) {
            // Only process import statements from SETUP
            if (setup_node->type == AST_IMPORT_STMT) {
                emit_import_stmt(&emit, setup_node);
            }
            setup_node = setup_node->next_sibling;
        }
    }

    // Find BODY content
    OmniAstNode* body_start = find_body_start(doc);
    if (!body_start) {
        // Empty program
        uint16_t main_fn = omni_ir_begin_function(prog, "main", 0, VAL_INT);
        omni_ir_emit_wide(prog, OP_PUSH_CONST, omni_ir_add_int(prog, 0));
        omni_ir_emit(prog, OP_RETURN);
        omni_ir_end_function(prog, main_fn);
        omni_ir_set_entry(prog, main_fn);
        return prog;
    }

    // Begin main function
    uint16_t main_fn = omni_ir_begin_function(prog, "main", 0, VAL_INT);
    emit.current_func = main_fn;
    emit.in_function = true;

    // Emit all nodes
    OmniAstNode* node = body_start;
    while (node != NULL) {
        // Skip block markers
        if (node->type == AST_BLOCK) {
            node = node->next_sibling;
            continue;
        }

        // Handle WHEN specially (includes its body and OTHERWISE)
        if (node->type == AST_WHEN_STMT) {
            OmniAstNode* next = NULL;
            emit_when_stmt(&emit, node, &next);
            node = next;
            continue;
        }

        // Skip OTHERWISE (handled by WHEN)
        if (node->type == AST_OTHERWISE_STMT) {
            node = node->next_sibling;
            continue;
        }

        // Skip function definitions for now (would need separate handling)
        if (node->type == AST_FUNC_DEF) {
            // Skip until next top-level node
            int func_col = node->column;
            node = node->next_sibling;
            while (node != NULL && node->type != AST_FUNC_DEF &&
                   (node->type == AST_BLOCK || node->column > func_col)) {
                node = node->next_sibling;
            }
            continue;
        }

        emit_node(&emit, node);
        node = node->next_sibling;
    }

    // Return 0 at end
    omni_ir_emit_wide(prog, OP_PUSH_CONST, omni_ir_add_int(prog, 0));
    omni_ir_emit(prog, OP_RETURN);

    omni_ir_end_function(prog, main_fn);
    omni_ir_set_entry(prog, main_fn);

    return prog;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Closing Note
//
// "And the Word was made flesh, and dwelt among us."
//   — John 1:14
//
// The abstract AST becomes concrete IR. Structure becomes execution.

// =============================================================================
// END CLOSING
// =============================================================================
