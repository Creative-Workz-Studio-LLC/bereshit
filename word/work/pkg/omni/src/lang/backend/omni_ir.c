// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ir-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_ir.c — OmniCode Intermediate Representation Implementation
// Trit-native instruction set for OmniCode virtual machine.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-ir-impl
// Title: OmniCode IR Implementation
// Type: Source
// Role: Implement IR program construction, emission, and serialization

// # M.8 Grounding [GROUNDING]
//
// Scripture: Exodus 25:40 - "Look that thou make them after their pattern"
// Principle: The pattern defines the execution - IR is the blueprint
// Anchor: Genesis 1:2 - Spirit moved upon the face of the waters

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#define _GNU_SOURCE  // For strdup

#include <stdlib.h>
#include <string.h>
#include "omni_ir.h"

// # S.1 Internal Constants [CONSTANTS]

#define DEFAULT_CODE_CAPACITY       1024
#define DEFAULT_CONST_CAPACITY      256
#define DEFAULT_STRING_CAPACITY     128
#define DEFAULT_FUNC_CAPACITY       64

// # S.2 Helpers [HELPERS]

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

static bool ensure_code_capacity(OmniIRProgram* prog, uint32_t needed) {
    if (prog->code_length + needed <= prog->code_capacity) return true;

    uint32_t new_cap = prog->code_capacity * 2;
    while (new_cap < prog->code_length + needed) new_cap *= 2;

    OmniInstruction* new_code = realloc(prog->code, new_cap * sizeof(OmniInstruction));
    if (!new_code) return false;

    uint32_t* new_lines = realloc(prog->line_numbers, new_cap * sizeof(uint32_t));
    if (!new_lines) {
        prog->code = new_code;  // Keep the expanded code array
        return false;
    }

    prog->code = new_code;
    prog->line_numbers = new_lines;
    prog->code_capacity = new_cap;
    return true;
}

static bool ensure_const_capacity(OmniIRProgram* prog) {
    if (prog->constant_count < prog->constant_capacity) return true;

    uint16_t new_cap = prog->constant_capacity * 2;
    OmniValue* new_consts = realloc(prog->constants, new_cap * sizeof(OmniValue));
    if (!new_consts) return false;

    prog->constants = new_consts;
    prog->constant_capacity = new_cap;
    return true;
}

static bool ensure_string_capacity(OmniIRProgram* prog) {
    if (prog->string_count < prog->string_capacity) return true;

    uint16_t new_cap = prog->string_capacity * 2;
    char** new_strings = realloc(prog->strings, new_cap * sizeof(char*));
    if (!new_strings) return false;

    prog->strings = new_strings;
    prog->string_capacity = new_cap;
    return true;
}

static bool ensure_func_capacity(OmniIRProgram* prog) {
    if (prog->function_count < prog->function_capacity) return true;

    uint16_t new_cap = prog->function_capacity * 2;
    OmniIRFunction* new_funcs = realloc(prog->functions, new_cap * sizeof(OmniIRFunction));
    if (!new_funcs) return false;

    prog->functions = new_funcs;
    prog->function_capacity = new_cap;
    return true;
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Program Lifecycle [LIFECYCLE]

OmniIRProgram* omni_ir_create(void) {
    OmniIRProgram* prog = calloc(1, sizeof(OmniIRProgram));
    if (!prog) return NULL;

    prog->magic = OMNI_IR_MAGIC;
    prog->version_major = OMNI_IR_VERSION_MAJOR;
    prog->version_minor = OMNI_IR_VERSION_MINOR;

    // Allocate arrays
    prog->code = calloc(DEFAULT_CODE_CAPACITY, sizeof(OmniInstruction));
    prog->line_numbers = calloc(DEFAULT_CODE_CAPACITY, sizeof(uint32_t));
    prog->constants = calloc(DEFAULT_CONST_CAPACITY, sizeof(OmniValue));
    prog->strings = calloc(DEFAULT_STRING_CAPACITY, sizeof(char*));
    prog->functions = calloc(DEFAULT_FUNC_CAPACITY, sizeof(OmniIRFunction));

    if (!prog->code || !prog->line_numbers || !prog->constants ||
        !prog->strings || !prog->functions) {
        omni_ir_destroy(prog);
        return NULL;
    }

    prog->code_capacity = DEFAULT_CODE_CAPACITY;
    prog->constant_capacity = DEFAULT_CONST_CAPACITY;
    prog->string_capacity = DEFAULT_STRING_CAPACITY;
    prog->function_capacity = DEFAULT_FUNC_CAPACITY;

    return prog;
}

void omni_ir_destroy(OmniIRProgram* prog) {
    if (!prog) return;

    // Free strings
    for (uint16_t i = 0; i < prog->string_count; i++) {
        free(prog->strings[i]);
    }
    free(prog->strings);

    free(prog->code);
    free(prog->line_numbers);
    free(prog->constants);
    free(prog->functions);
    free(prog);
}

// # B.2 Constant Pool [CONSTANTS]

uint16_t omni_ir_add_int(OmniIRProgram* prog, int64_t value) {
    if (!prog || !ensure_const_capacity(prog)) return 0xFFFF;

    uint16_t index = prog->constant_count++;
    prog->constants[index].type = VAL_INT;
    prog->constants[index].as_int = value;
    return index;
}

uint16_t omni_ir_add_trit(OmniIRProgram* prog, int8_t value) {
    if (!prog || !ensure_const_capacity(prog)) return 0xFFFF;

    // Clamp to valid trit range
    if (value < -1) value = -1;
    if (value > 1) value = 1;

    uint16_t index = prog->constant_count++;
    prog->constants[index].type = VAL_TRIT;
    prog->constants[index].as_trit = value;
    return index;
}

uint16_t omni_ir_add_float(OmniIRProgram* prog, double value) {
    if (!prog || !ensure_const_capacity(prog)) return 0xFFFF;

    uint16_t index = prog->constant_count++;
    prog->constants[index].type = VAL_FLOAT;
    prog->constants[index].as_float = value;
    return index;
}

uint16_t omni_ir_add_string(OmniIRProgram* prog, const char* value) {
    if (!prog || !value || !ensure_string_capacity(prog)) return 0xFFFF;

    // Add to string pool
    uint16_t str_index = prog->string_count++;
    prog->strings[str_index] = strdup(value);
    if (!prog->strings[str_index]) {
        prog->string_count--;
        return 0xFFFF;
    }

    // Add constant referencing string
    if (!ensure_const_capacity(prog)) return 0xFFFF;

    uint16_t const_index = prog->constant_count++;
    prog->constants[const_index].type = VAL_STRING;
    prog->constants[const_index].as_string = str_index;
    return const_index;
}

uint16_t omni_ir_add_bool(OmniIRProgram* prog, bool value) {
    if (!prog || !ensure_const_capacity(prog)) return 0xFFFF;

    uint16_t index = prog->constant_count++;
    prog->constants[index].type = VAL_BOOL;
    prog->constants[index].as_bool = value;
    return index;
}

// # B.3 Code Emission [EMIT]

static uint32_t current_line = 0;

void omni_ir_emit(OmniIRProgram* prog, OmniOpcode op) {
    if (!prog || !ensure_code_capacity(prog, 1)) return;

    uint32_t ip = prog->code_length++;
    prog->code[ip].opcode = op;
    prog->code[ip].operand1 = 0;
    prog->code[ip].operand2 = 0;
    prog->code[ip].operand3 = 0;
    prog->line_numbers[ip] = current_line;
}

void omni_ir_emit_1(OmniIRProgram* prog, OmniOpcode op, uint8_t arg1) {
    if (!prog || !ensure_code_capacity(prog, 1)) return;

    uint32_t ip = prog->code_length++;
    prog->code[ip].opcode = op;
    prog->code[ip].operand1 = arg1;
    prog->code[ip].operand2 = 0;
    prog->code[ip].operand3 = 0;
    prog->line_numbers[ip] = current_line;
}

void omni_ir_emit_2(OmniIRProgram* prog, OmniOpcode op, uint8_t arg1, uint8_t arg2) {
    if (!prog || !ensure_code_capacity(prog, 1)) return;

    uint32_t ip = prog->code_length++;
    prog->code[ip].opcode = op;
    prog->code[ip].operand1 = arg1;
    prog->code[ip].operand2 = arg2;
    prog->code[ip].operand3 = 0;
    prog->line_numbers[ip] = current_line;
}

void omni_ir_emit_3(OmniIRProgram* prog, OmniOpcode op, uint8_t a1, uint8_t a2, uint8_t a3) {
    if (!prog || !ensure_code_capacity(prog, 1)) return;

    uint32_t ip = prog->code_length++;
    prog->code[ip].opcode = op;
    prog->code[ip].operand1 = a1;
    prog->code[ip].operand2 = a2;
    prog->code[ip].operand3 = a3;
    prog->line_numbers[ip] = current_line;
}

void omni_ir_emit_wide(OmniIRProgram* prog, OmniOpcode op, uint16_t arg) {
    // Split 16-bit into two 8-bit operands (big-endian)
    omni_ir_emit_2(prog, op, (arg >> 8) & 0xFF, arg & 0xFF);
}

uint32_t omni_ir_ip(OmniIRProgram* prog) {
    return prog ? prog->code_length : 0;
}

void omni_ir_patch_jump(OmniIRProgram* prog, uint32_t offset, uint32_t target) {
    if (!prog || offset >= prog->code_length) return;

    // Store target as 16-bit in operand1 and operand2
    prog->code[offset].operand1 = (target >> 8) & 0xFF;
    prog->code[offset].operand2 = target & 0xFF;
}

// # B.4 Functions [FUNCTIONS]

uint16_t omni_ir_begin_function(OmniIRProgram* prog, const char* name,
                                 uint8_t arity, OmniValueType return_type) {
    if (!prog || !ensure_func_capacity(prog)) return 0xFFFF;

    uint16_t index = prog->function_count++;
    OmniIRFunction* func = &prog->functions[index];

    safe_strcpy(func->name, sizeof(func->name), name ? name : "");
    func->start_ip = (uint16_t)prog->code_length;
    func->end_ip = 0;  // Set in end_function
    func->arity = arity;
    func->locals = 0;
    func->return_type = return_type;

    return index;
}

void omni_ir_end_function(OmniIRProgram* prog, uint16_t func_index) {
    if (!prog || func_index >= prog->function_count) return;

    prog->functions[func_index].end_ip = (uint16_t)prog->code_length;
}

void omni_ir_set_entry(OmniIRProgram* prog, uint16_t func_index) {
    if (!prog) return;
    prog->entry_function = func_index;
}

// # B.5 Debug Info [DEBUG]

void omni_ir_set_line(OmniIRProgram* prog, uint32_t line) {
    (void)prog;
    current_line = line;
}

// # B.6 Serialization [SERIALIZE]

bool omni_ir_write(OmniIRProgram* prog, const char* path) {
    if (!prog || !path) return false;

    FILE* f = fopen(path, "wb");
    if (!f) return false;

    // Write header
    fwrite(&prog->magic, sizeof(prog->magic), 1, f);
    fwrite(&prog->version_major, sizeof(prog->version_major), 1, f);
    fwrite(&prog->version_minor, sizeof(prog->version_minor), 1, f);

    // Write constants
    fwrite(&prog->constant_count, sizeof(prog->constant_count), 1, f);
    fwrite(prog->constants, sizeof(OmniValue), prog->constant_count, f);

    // Write strings
    fwrite(&prog->string_count, sizeof(prog->string_count), 1, f);
    for (uint16_t i = 0; i < prog->string_count; i++) {
        uint16_t len = (uint16_t)strlen(prog->strings[i]);
        fwrite(&len, sizeof(len), 1, f);
        fwrite(prog->strings[i], 1, len, f);
    }

    // Write code
    fwrite(&prog->code_length, sizeof(prog->code_length), 1, f);
    fwrite(prog->code, sizeof(OmniInstruction), prog->code_length, f);

    // Write functions
    fwrite(&prog->function_count, sizeof(prog->function_count), 1, f);
    fwrite(prog->functions, sizeof(OmniIRFunction), prog->function_count, f);

    // Write entry point
    fwrite(&prog->entry_function, sizeof(prog->entry_function), 1, f);

    fclose(f);
    return true;
}

OmniIRProgram* omni_ir_read(const char* path) {
    if (!path) return NULL;

    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    OmniIRProgram* prog = omni_ir_create();
    if (!prog) {
        fclose(f);
        return NULL;
    }

    // Helper macro for checked fread
    #define READ_OR_FAIL(ptr, size, count, stream) \
        if (fread(ptr, size, count, stream) != (size_t)(count)) { \
            omni_ir_destroy(prog); \
            fclose(f); \
            return NULL; \
        }

    // Read header
    READ_OR_FAIL(&prog->magic, sizeof(prog->magic), 1, f);
    if (prog->magic != OMNI_IR_MAGIC) {
        omni_ir_destroy(prog);
        fclose(f);
        return NULL;
    }
    READ_OR_FAIL(&prog->version_major, sizeof(prog->version_major), 1, f);
    READ_OR_FAIL(&prog->version_minor, sizeof(prog->version_minor), 1, f);

    // Read constants
    READ_OR_FAIL(&prog->constant_count, sizeof(prog->constant_count), 1, f);
    if (prog->constant_count > 0) {
        prog->constants = realloc(prog->constants,
                                   prog->constant_count * sizeof(OmniValue));
        READ_OR_FAIL(prog->constants, sizeof(OmniValue), prog->constant_count, f);
    }

    // Read strings
    READ_OR_FAIL(&prog->string_count, sizeof(prog->string_count), 1, f);
    if (prog->string_count > 0) {
        prog->strings = realloc(prog->strings,
                                 prog->string_count * sizeof(char*));
        for (uint16_t i = 0; i < prog->string_count; i++) {
            uint16_t len;
            READ_OR_FAIL(&len, sizeof(len), 1, f);
            prog->strings[i] = malloc(len + 1);
            READ_OR_FAIL(prog->strings[i], 1, len, f);
            prog->strings[i][len] = '\0';
        }
    }

    // Read code
    READ_OR_FAIL(&prog->code_length, sizeof(prog->code_length), 1, f);
    if (prog->code_length > 0) {
        prog->code = realloc(prog->code,
                              prog->code_length * sizeof(OmniInstruction));
        READ_OR_FAIL(prog->code, sizeof(OmniInstruction), prog->code_length, f);
    }

    // Read functions
    READ_OR_FAIL(&prog->function_count, sizeof(prog->function_count), 1, f);
    if (prog->function_count > 0) {
        prog->functions = realloc(prog->functions,
                                   prog->function_count * sizeof(OmniIRFunction));
        READ_OR_FAIL(prog->functions, sizeof(OmniIRFunction), prog->function_count, f);
    }

    // Read entry point
    READ_OR_FAIL(&prog->entry_function, sizeof(prog->entry_function), 1, f);

    #undef READ_OR_FAIL

    fclose(f);
    return prog;
}

// # B.7 Disassembly [DISASM]

const char* omni_opcode_name(OmniOpcode op) {
    switch (op) {
        // Stack
        case OP_NOP:            return "NOP";
        case OP_PUSH_CONST:     return "PUSH_CONST";
        case OP_PUSH_LOCAL:     return "PUSH_LOCAL";
        case OP_PUSH_GLOBAL:    return "PUSH_GLOBAL";
        case OP_POP:            return "POP";
        case OP_DUP:            return "DUP";
        case OP_SWAP:           return "SWAP";
        case OP_ROT:            return "ROT";

        // Store
        case OP_STORE_LOCAL:    return "STORE_LOCAL";
        case OP_STORE_GLOBAL:   return "STORE_GLOBAL";

        // Arithmetic
        case OP_ADD:            return "ADD";
        case OP_SUB:            return "SUB";
        case OP_MUL:            return "MUL";
        case OP_DIV:            return "DIV";
        case OP_MOD:            return "MOD";
        case OP_NEG:            return "NEG";
        case OP_INC:            return "INC";
        case OP_DEC:            return "DEC";

        // Trit
        case OP_TRIT_NEG:       return "TRIT_NEG";
        case OP_TRIT_CMP:       return "TRIT_CMP";
        case OP_TRIT_BALANCE:   return "TRIT_BALANCE";
        case OP_TRIT_SIGN:      return "TRIT_SIGN";
        case OP_TRIT_AND:       return "TRIT_AND";
        case OP_TRIT_OR:        return "TRIT_OR";
        case OP_TRIT_SUM:       return "TRIT_SUM";

        // Comparison
        case OP_EQ:             return "EQ";
        case OP_NE:             return "NE";
        case OP_LT:             return "LT";
        case OP_LE:             return "LE";
        case OP_GT:             return "GT";
        case OP_GE:             return "GE";

        // Control flow
        case OP_JUMP:           return "JUMP";
        case OP_JUMP_POS:       return "JUMP_POS";
        case OP_JUMP_ZERO:      return "JUMP_ZERO";
        case OP_JUMP_NEG:       return "JUMP_NEG";
        case OP_JUMP_TRUE:      return "JUMP_TRUE";
        case OP_CALL:           return "CALL";
        case OP_RETURN:         return "RETURN";
        case OP_HALT:           return "HALT";

        // Health/State
        case OP_HEALTH_GET:     return "HEALTH_GET";
        case OP_HEALTH_ADD:     return "HEALTH_ADD";
        case OP_HEALTH_SET:     return "HEALTH_SET";
        case OP_STATE_GET:      return "STATE_GET";
        case OP_STATE_TRANS:    return "STATE_TRANS";

        // I/O
        case OP_PRINT:          return "PRINT";
        case OP_PRINT_STR:      return "PRINT_STR";
        case OP_PRINT_NL:       return "PRINT_NL";
        case OP_READ_INT:       return "READ_INT";
        case OP_READ_LINE:      return "READ_LINE";

        // Witness
        case OP_WITNESS:        return "WITNESS";
        case OP_FAIL:           return "FAIL";
        case OP_ASSERT:         return "ASSERT";

        // DAR (Detect-Assess-Recover)
        case OP_CHECKPOINT:     return "CHECKPOINT";
        case OP_RESTORE:        return "RESTORE";
        case OP_DISCARD:        return "DISCARD";

        // Module Operations
        case OP_IMPORT:         return "IMPORT";
        case OP_CALL_EXT:       return "CALL_EXT";
        case OP_GET_EXPORT:     return "GET_EXPORT";

        // String Intrinsics
        case OP_STR_LEN:        return "STR_LEN";
        case OP_STR_CHAR:       return "STR_CHAR";
        case OP_STR_SUB:        return "STR_SUB";
        case OP_STR_FIND:       return "STR_FIND";
        case OP_STR_CAT:        return "STR_CAT";
        case OP_STR_EQ:         return "STR_EQ";
        case OP_STR_CMP:        return "STR_CMP";
        case OP_STR_SPLIT:      return "STR_SPLIT";
        case OP_STR_JOIN:       return "STR_JOIN";
        case OP_STR_TRIM:       return "STR_TRIM";
        case OP_STR_UPPER:      return "STR_UPPER";
        case OP_STR_LOWER:      return "STR_LOWER";
        case OP_STR_STARTS:     return "STR_STARTS";
        case OP_STR_ENDS:       return "STR_ENDS";
        case OP_STR_REPLACE:    return "STR_REPLACE";

        // Collection Intrinsics
        case OP_LIST_NEW:       return "LIST_NEW";
        case OP_LIST_PUSH:      return "LIST_PUSH";
        case OP_LIST_POP:       return "LIST_POP";
        case OP_LIST_GET:       return "LIST_GET";
        case OP_LIST_SET:       return "LIST_SET";
        case OP_LIST_LEN:       return "LIST_LEN";
        case OP_LIST_FIND:      return "LIST_FIND";
        case OP_LIST_CLEAR:     return "LIST_CLEAR";
        case OP_MAP_NEW:        return "MAP_NEW";
        case OP_MAP_GET:        return "MAP_GET";
        case OP_MAP_SET:        return "MAP_SET";
        case OP_MAP_HAS:        return "MAP_HAS";
        case OP_MAP_DEL:        return "MAP_DEL";
        case OP_MAP_KEYS:       return "MAP_KEYS";
        case OP_MAP_LEN:        return "MAP_LEN";

        // File I/O Intrinsics
        case OP_FILE_OPEN:      return "FILE_OPEN";
        case OP_FILE_CLOSE:     return "FILE_CLOSE";
        case OP_FILE_READ:      return "FILE_READ";
        case OP_FILE_WRITE:     return "FILE_WRITE";
        case OP_FILE_EXISTS:    return "FILE_EXISTS";
        case OP_FILE_SIZE:      return "FILE_SIZE";
        case OP_FILE_DELETE:    return "FILE_DELETE";
        case OP_FILE_READLN:    return "FILE_READLN";
        case OP_FILE_WRITELN:   return "FILE_WRITELN";
        case OP_DIR_LIST:       return "DIR_LIST";
        case OP_DIR_CREATE:     return "DIR_CREATE";
        case OP_DIR_EXISTS:     return "DIR_EXISTS";

        // Extended
        case OP_EXTENDED:       return "EXTENDED";
        case OP_DEBUG:          return "DEBUG";
        case OP_INVALID:        return "INVALID";

        default:                return "UNKNOWN";
    }
}

void omni_ir_disassemble(OmniIRProgram* prog, FILE* out) {
    if (!prog || !out) return;

    fprintf(out, "═══════════════════════════════════════════════════════════════════\n");
    fprintf(out, "  OmniCode IR Disassembly\n");
    fprintf(out, "  Version: %d.%d\n", prog->version_major, prog->version_minor);
    fprintf(out, "═══════════════════════════════════════════════════════════════════\n\n");

    // Print constants
    fprintf(out, "Constants: %d\n", prog->constant_count);
    for (uint16_t i = 0; i < prog->constant_count; i++) {
        OmniValue* v = &prog->constants[i];
        fprintf(out, "  [%3d] ", i);
        switch (v->type) {
            case VAL_INT:    fprintf(out, "INT    %ld\n", v->as_int); break;
            case VAL_TRIT:   fprintf(out, "TRIT   %+d\n", v->as_trit); break;
            case VAL_FLOAT:  fprintf(out, "FLOAT  %g\n", v->as_float); break;
            case VAL_STRING: fprintf(out, "STRING \"%s\"\n",
                                     prog->strings[v->as_string]); break;
            case VAL_BOOL:   fprintf(out, "BOOL   %s\n",
                                     v->as_bool ? "true" : "false"); break;
            case VAL_FUNC:   fprintf(out, "FUNC   #%d\n", v->as_func); break;
            case VAL_LIST:   fprintf(out, "LIST   @%p\n", v->as_list); break;
            case VAL_MAP:    fprintf(out, "MAP    @%p\n", v->as_map); break;
            case VAL_FILE:   fprintf(out, "FILE   @%p\n", v->as_file); break;
            default:         fprintf(out, "NONE\n"); break;
        }
    }
    fprintf(out, "\n");

    // Print functions
    fprintf(out, "Functions: %d (entry: %d)\n", prog->function_count, prog->entry_function);
    for (uint16_t i = 0; i < prog->function_count; i++) {
        OmniIRFunction* f = &prog->functions[i];
        fprintf(out, "  [%3d] %s (arity=%d, locals=%d) @ %d-%d\n",
                i, f->name, f->arity, f->locals, f->start_ip, f->end_ip);
    }
    fprintf(out, "\n");

    // Print code
    fprintf(out, "Code: %d instructions\n", prog->code_length);
    fprintf(out, "───────────────────────────────────────────────────────────────────\n");

    for (uint32_t ip = 0; ip < prog->code_length; ip++) {
        OmniInstruction* inst = &prog->code[ip];
        uint16_t wide_arg = ((uint16_t)inst->operand1 << 8) | inst->operand2;

        fprintf(out, "  %04d: %-14s", ip, omni_opcode_name(inst->opcode));

        // Print operands based on opcode
        switch (inst->opcode) {
            case OP_PUSH_CONST:
            case OP_PRINT_STR:
            case OP_WITNESS:
            case OP_FAIL:
            case OP_CALL:
                fprintf(out, " %d", wide_arg);
                break;

            case OP_PUSH_LOCAL:
            case OP_PUSH_GLOBAL:
            case OP_STORE_LOCAL:
            case OP_STORE_GLOBAL:
            case OP_HEALTH_ADD:
            case OP_HEALTH_SET:
            case OP_STATE_TRANS:
                fprintf(out, " %d", inst->operand1);
                break;

            case OP_JUMP:
            case OP_JUMP_POS:
            case OP_JUMP_ZERO:
            case OP_JUMP_NEG:
            case OP_JUMP_TRUE:
                fprintf(out, " -> %d", wide_arg);
                break;

            default:
                break;
        }

        fprintf(out, "\n");
    }

    fprintf(out, "───────────────────────────────────────────────────────────────────\n");
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Closing Note
//
// "Look that thou make them after their pattern, which was shewed thee."
//   — Exodus 25:40
//
// The IR is the pattern - faithfully executed by the VM.

// =============================================================================
// END CLOSING
// =============================================================================
