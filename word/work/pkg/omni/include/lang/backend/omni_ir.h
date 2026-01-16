// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ir
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_ir.h — OmniCode Intermediate Representation
// Trit-native instruction set for OmniCode virtual machine.

#ifndef BERESHIT_OMNI_IR_H
#define BERESHIT_OMNI_IR_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-ir
// Title: OmniCode Intermediate Representation
// Type: Header
// Role: Define trit-native IR instruction set and program structure

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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

// # S.1 IR Constants [CONSTANTS]

#define OMNI_IR_VERSION_MAJOR   1
#define OMNI_IR_VERSION_MINOR   0
#define OMNI_IR_MAGIC           0x4F4D4952  // "OMIR"

#define OMNI_IR_MAX_LOCALS      243         // 3^5 - trit5 addressable
#define OMNI_IR_MAX_GLOBALS     243
#define OMNI_IR_MAX_CONSTANTS   729         // 3^6
#define OMNI_IR_STACK_SIZE      729

// # S.2 Opcodes [OPCODES]
//
// Trit-native instruction set organized by category.
// Each opcode is 1 byte, operands follow as needed.

typedef enum {
    // -------------------------------------------------------------------------
    // Stack Operations (0x00 - 0x0F)
    // -------------------------------------------------------------------------
    OP_NOP          = 0x00,     // No operation
    OP_PUSH_CONST   = 0x01,     // Push constant: [const_index:2]
    OP_PUSH_LOCAL   = 0x02,     // Push local:    [local_index:1]
    OP_PUSH_GLOBAL  = 0x03,     // Push global:   [global_index:1]
    OP_POP          = 0x04,     // Pop top of stack
    OP_DUP          = 0x05,     // Duplicate top of stack
    OP_SWAP         = 0x06,     // Swap top two values
    OP_ROT          = 0x07,     // Rotate top 3: a b c -> b c a

    // -------------------------------------------------------------------------
    // Store Operations (0x10 - 0x1F)
    // -------------------------------------------------------------------------
    OP_STORE_LOCAL  = 0x10,     // Store to local:  [local_index:1]
    OP_STORE_GLOBAL = 0x11,     // Store to global: [global_index:1]

    // -------------------------------------------------------------------------
    // Arithmetic (0x20 - 0x2F) - Balanced ternary native
    // -------------------------------------------------------------------------
    OP_ADD          = 0x20,     // a + b
    OP_SUB          = 0x21,     // a - b
    OP_MUL          = 0x22,     // a * b
    OP_DIV          = 0x23,     // a / b (integer division)
    OP_MOD          = 0x24,     // a % b
    OP_NEG          = 0x25,     // -a (negate)
    OP_INC          = 0x26,     // a + 1
    OP_DEC          = 0x27,     // a - 1

    // -------------------------------------------------------------------------
    // Trit Operations (0x30 - 0x3F) - Native to balanced ternary
    // -------------------------------------------------------------------------
    OP_TRIT_NEG     = 0x30,     // Trit negate: -1 -> +1, 0 -> 0, +1 -> -1
    OP_TRIT_CMP     = 0x31,     // Trit compare: push -1/0/+1
    OP_TRIT_BALANCE = 0x32,     // Balance toward zero
    OP_TRIT_SIGN    = 0x33,     // Extract sign as trit
    OP_TRIT_AND     = 0x34,     // Ternary AND (minimum)
    OP_TRIT_OR      = 0x35,     // Ternary OR (maximum)
    OP_TRIT_SUM     = 0x36,     // Ternary SUM (addition mod 3)

    // -------------------------------------------------------------------------
    // Comparison (0x40 - 0x4F)
    // -------------------------------------------------------------------------
    OP_EQ           = 0x40,     // a == b -> trit
    OP_NE           = 0x41,     // a != b -> trit
    OP_LT           = 0x42,     // a < b  -> trit
    OP_LE           = 0x43,     // a <= b -> trit
    OP_GT           = 0x44,     // a > b  -> trit
    OP_GE           = 0x45,     // a >= b -> trit

    // -------------------------------------------------------------------------
    // Control Flow (0x50 - 0x5F)
    // -------------------------------------------------------------------------
    OP_JUMP         = 0x50,     // Unconditional: [offset:2]
    OP_JUMP_POS     = 0x51,     // Jump if +1:    [offset:2]
    OP_JUMP_ZERO    = 0x52,     // Jump if 0:     [offset:2]
    OP_JUMP_NEG     = 0x53,     // Jump if -1:    [offset:2]
    OP_JUMP_TRUE    = 0x54,     // Jump if != 0:  [offset:2] (granted or denied)
    OP_CALL         = 0x55,     // Call function: [func_index:2]
    OP_RETURN       = 0x56,     // Return from function
    OP_HALT         = 0x57,     // Halt execution

    // -------------------------------------------------------------------------
    // Health & State (0x60 - 0x6F) - CPI-SI native
    // -------------------------------------------------------------------------
    OP_HEALTH_GET   = 0x60,     // Push current health score
    OP_HEALTH_ADD   = 0x61,     // Add to health: [delta:1]
    OP_HEALTH_SET   = 0x62,     // Set health:    [value:1]
    OP_STATE_GET    = 0x63,     // Push current Hebrew state
    OP_STATE_TRANS  = 0x64,     // State transition: [k_factor:1]

    // -------------------------------------------------------------------------
    // I/O Operations (0x70 - 0x7F)
    // -------------------------------------------------------------------------
    OP_PRINT        = 0x70,     // Print top of stack
    OP_PRINT_STR    = 0x71,     // Print string: [const_index:2]
    OP_PRINT_NL     = 0x72,     // Print newline
    OP_READ_INT     = 0x73,     // Read integer to stack
    OP_READ_LINE    = 0x74,     // Read line to stack (string index)

    // -------------------------------------------------------------------------
    // Witness Operations (0x80 - 0x8F) - OmniCode native
    // -------------------------------------------------------------------------
    OP_WITNESS      = 0x80,     // Witness: [const_index:2] [level:1]
    OP_FAIL         = 0x81,     // Fail with message: [const_index:2]
    OP_ASSERT       = 0x82,     // Assert condition (fail if not granted)

    // -------------------------------------------------------------------------
    // DAR Operations (0x90 - 0x9F) - Detect-Assess-Recover
    // -------------------------------------------------------------------------
    OP_CHECKPOINT   = 0x90,     // Save VM state: [name_index:2] (0 = unnamed)
    OP_RESTORE      = 0x91,     // Restore VM state: [name_index:2] (0 = last)
    OP_DISCARD      = 0x92,     // Discard checkpoint: [name_index:2]

    // -------------------------------------------------------------------------
    // Module Operations (0xA0 - 0xAF) - Library loading and linking
    // -------------------------------------------------------------------------
    OP_IMPORT       = 0xA0,     // Import module: [path_index:2]
    OP_CALL_EXT     = 0xA1,     // Call external: [module:1] [func_index:2]
    OP_GET_EXPORT   = 0xA2,     // Get exported value: [module:1] [name_index:2]

    // -------------------------------------------------------------------------
    // String Intrinsics (0xB0 - 0xBF) - String manipulation
    // -------------------------------------------------------------------------
    OP_STR_LEN      = 0xB0,     // String length: pop str, push len
    OP_STR_CHAR     = 0xB1,     // Char at: pop idx, pop str, push char (as int)
    OP_STR_SUB      = 0xB2,     // Substring: pop len, pop start, pop str, push result
    OP_STR_FIND     = 0xB3,     // Find: pop needle, pop haystack, push idx (-1 if not found)
    OP_STR_CAT      = 0xB4,     // Concat: pop b, pop a, push a+b
    OP_STR_EQ       = 0xB5,     // String equals: pop b, pop a, push trit
    OP_STR_CMP      = 0xB6,     // String compare: pop b, pop a, push -1/0/+1
    OP_STR_SPLIT    = 0xB7,     // Split: pop delim, pop str, push list handle
    OP_STR_JOIN     = 0xB8,     // Join: pop delim, pop list, push str
    OP_STR_TRIM     = 0xB9,     // Trim: pop str, push trimmed
    OP_STR_UPPER    = 0xBA,     // Upper: pop str, push uppercased
    OP_STR_LOWER    = 0xBB,     // Lower: pop str, push lowercased
    OP_STR_STARTS   = 0xBC,     // Starts with: pop prefix, pop str, push trit
    OP_STR_ENDS     = 0xBD,     // Ends with: pop suffix, pop str, push trit
    OP_STR_REPLACE  = 0xBE,     // Replace: pop repl, pop find, pop str, push result

    // -------------------------------------------------------------------------
    // Collection Intrinsics (0xC0 - 0xCF) - Lists and maps
    // -------------------------------------------------------------------------
    OP_LIST_NEW     = 0xC0,     // Create list: push list handle
    OP_LIST_PUSH    = 0xC1,     // Push to list: pop value, pop list
    OP_LIST_POP     = 0xC2,     // Pop from list: pop list, push value
    OP_LIST_GET     = 0xC3,     // Get element: pop idx, pop list, push value
    OP_LIST_SET     = 0xC4,     // Set element: pop value, pop idx, pop list
    OP_LIST_LEN     = 0xC5,     // List length: pop list, push len
    OP_LIST_FIND    = 0xC6,     // Find element: pop value, pop list, push idx
    OP_LIST_CLEAR   = 0xC7,     // Clear list: pop list

    OP_MAP_NEW      = 0xC8,     // Create map: push map handle
    OP_MAP_GET      = 0xC9,     // Get value: pop key, pop map, push value
    OP_MAP_SET      = 0xCA,     // Set value: pop value, pop key, pop map
    OP_MAP_HAS      = 0xCB,     // Has key: pop key, pop map, push trit
    OP_MAP_DEL      = 0xCC,     // Delete key: pop key, pop map
    OP_MAP_KEYS     = 0xCD,     // Get keys: pop map, push list
    OP_MAP_LEN      = 0xCE,     // Map size: pop map, push len

    // -------------------------------------------------------------------------
    // File I/O Intrinsics (0xD0 - 0xDF) - File operations
    // -------------------------------------------------------------------------
    OP_FILE_OPEN    = 0xD0,     // Open file: pop mode, pop path, push handle
    OP_FILE_CLOSE   = 0xD1,     // Close file: pop handle
    OP_FILE_READ    = 0xD2,     // Read all: pop handle, push str
    OP_FILE_WRITE   = 0xD3,     // Write: pop str, pop handle
    OP_FILE_EXISTS  = 0xD4,     // File exists: pop path, push trit
    OP_FILE_SIZE    = 0xD5,     // File size: pop path, push size
    OP_FILE_DELETE  = 0xD6,     // Delete file: pop path
    OP_FILE_READLN  = 0xD7,     // Read line: pop handle, push str (or none at EOF)
    OP_FILE_WRITELN = 0xD8,     // Write line: pop str, pop handle
    OP_DIR_LIST     = 0xD9,     // List directory: pop path, push list
    OP_DIR_CREATE   = 0xDA,     // Create directory: pop path
    OP_DIR_EXISTS   = 0xDB,     // Dir exists: pop path, push trit

    // -------------------------------------------------------------------------
    // Extended (0xF0 - 0xFF)
    // -------------------------------------------------------------------------
    OP_EXTENDED     = 0xF0,     // Extended opcode follows
    OP_DEBUG        = 0xFE,     // Debug breakpoint
    OP_INVALID      = 0xFF      // Invalid opcode (sentinel)
} OmniOpcode;

// # S.3 Types [TYPES]

// Witness levels (trit-based)
typedef enum {
    WITNESS_ERROR   = -1,       // Error level
    WITNESS_INFO    =  0,       // Info level
    WITNESS_DEBUG   = +1        // Debug level
} OmniWitnessLevel;

// IR Value types
typedef enum {
    VAL_NONE        = 0,
    VAL_INT         = 1,        // 64-bit integer
    VAL_TRIT        = 2,        // Balanced ternary trit
    VAL_FLOAT       = 3,        // 64-bit float
    VAL_STRING      = 4,        // String (index into constant pool)
    VAL_BOOL        = 5,        // Boolean
    VAL_FUNC        = 6,        // Function reference

    // Object handles (for intrinsics)
    VAL_LIST        = 7,        // List handle
    VAL_MAP         = 8,        // Map handle
    VAL_FILE        = 9         // File handle
} OmniValueType;

// IR Value (tagged union)
typedef struct {
    OmniValueType type;
    union {
        int64_t     as_int;
        int8_t      as_trit;    // -1, 0, +1
        double      as_float;
        uint16_t    as_string;  // Index into string pool
        bool        as_bool;
        uint16_t    as_func;    // Function index

        // Object handles (opaque pointers managed by VM)
        void*       as_list;    // OmniList*
        void*       as_map;     // OmniMap*
        void*       as_file;    // FILE* or wrapper
    };
} OmniValue;

// IR Instruction
typedef struct {
    OmniOpcode  opcode;
    uint8_t     operand1;
    uint8_t     operand2;
    uint8_t     operand3;
} OmniInstruction;

// Function definition
typedef struct {
    char        name[64];       // Function name
    uint16_t    start_ip;       // Start instruction pointer
    uint16_t    end_ip;         // End instruction pointer
    uint8_t     arity;          // Number of parameters
    uint8_t     locals;         // Number of local variables
    OmniValueType return_type;  // Return type
} OmniIRFunction;

// IR Program
typedef struct {
    uint32_t            magic;          // OMNI_IR_MAGIC
    uint16_t            version_major;
    uint16_t            version_minor;

    // Constant pool
    OmniValue*          constants;
    uint16_t            constant_count;
    uint16_t            constant_capacity;

    // String pool (for string constants)
    char**              strings;
    uint16_t            string_count;
    uint16_t            string_capacity;

    // Instructions
    OmniInstruction*    code;
    uint32_t            code_length;
    uint32_t            code_capacity;

    // Functions
    OmniIRFunction*     functions;
    uint16_t            function_count;
    uint16_t            function_capacity;

    // Entry point
    uint16_t            entry_function;

    // Debug info (line mapping)
    uint32_t*           line_numbers;   // Parallel to code array
} OmniIRProgram;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Program Lifecycle [LIFECYCLE]

OmniIRProgram* omni_ir_create(void);
void omni_ir_destroy(OmniIRProgram* prog);

// # B.2 Constant Pool [CONSTANTS]

uint16_t omni_ir_add_int(OmniIRProgram* prog, int64_t value);
uint16_t omni_ir_add_trit(OmniIRProgram* prog, int8_t value);
uint16_t omni_ir_add_float(OmniIRProgram* prog, double value);
uint16_t omni_ir_add_string(OmniIRProgram* prog, const char* value);
uint16_t omni_ir_add_bool(OmniIRProgram* prog, bool value);

// # B.3 Code Emission [EMIT]

void omni_ir_emit(OmniIRProgram* prog, OmniOpcode op);
void omni_ir_emit_1(OmniIRProgram* prog, OmniOpcode op, uint8_t arg1);
void omni_ir_emit_2(OmniIRProgram* prog, OmniOpcode op, uint8_t arg1, uint8_t arg2);
void omni_ir_emit_3(OmniIRProgram* prog, OmniOpcode op, uint8_t a1, uint8_t a2, uint8_t a3);

// Emit with 16-bit operand (split into two bytes)
void omni_ir_emit_wide(OmniIRProgram* prog, OmniOpcode op, uint16_t arg);

// Get current instruction pointer
uint32_t omni_ir_ip(OmniIRProgram* prog);

// Patch jump target at offset
void omni_ir_patch_jump(OmniIRProgram* prog, uint32_t offset, uint32_t target);

// # B.4 Functions [FUNCTIONS]

uint16_t omni_ir_begin_function(OmniIRProgram* prog, const char* name,
                                 uint8_t arity, OmniValueType return_type);
void omni_ir_end_function(OmniIRProgram* prog, uint16_t func_index);
void omni_ir_set_entry(OmniIRProgram* prog, uint16_t func_index);

// # B.5 Debug Info [DEBUG]

void omni_ir_set_line(OmniIRProgram* prog, uint32_t line);

// # B.6 Serialization [SERIALIZE]

bool omni_ir_write(OmniIRProgram* prog, const char* path);
OmniIRProgram* omni_ir_read(const char* path);

// # B.7 Disassembly [DISASM]

void omni_ir_disassemble(OmniIRProgram* prog, FILE* out);
const char* omni_opcode_name(OmniOpcode op);

// # B.8 AST to IR [AST_TO_IR]
//
// Requires: omni_parser.h
// Convert parsed OmniCode AST to IR program

struct OmniDocument;  // Forward declaration
OmniIRProgram* omni_ir_from_ast(struct OmniDocument* doc);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Example
//
//   OmniIRProgram* prog = omni_ir_create();
//
//   // Add constant
//   uint16_t msg = omni_ir_add_string(prog, "Hello, World!");
//
//   // Begin main function
//   uint16_t main_fn = omni_ir_begin_function(prog, "main", 0, VAL_INT);
//
//   // Emit: print "Hello, World!"
//   omni_ir_emit_wide(prog, OP_PRINT_STR, msg);
//   omni_ir_emit(prog, OP_PRINT_NL);
//
//   // Emit: return 0
//   uint16_t zero = omni_ir_add_int(prog, 0);
//   omni_ir_emit_wide(prog, OP_PUSH_CONST, zero);
//   omni_ir_emit(prog, OP_RETURN);
//
//   omni_ir_end_function(prog, main_fn);
//   omni_ir_set_entry(prog, main_fn);
//
//   // Save to file
//   omni_ir_write(prog, "hello.omir");
//
//   omni_ir_destroy(prog);

// # X.2 Closing Note
//
// "Look that thou make them after their pattern, which was shewed thee."
//   — Exodus 25:40
//
// The IR is the pattern - faithfully executed by the VM.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_IR_H
