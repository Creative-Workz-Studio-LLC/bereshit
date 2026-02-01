// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-omnicode
// Title:       MillenniumOS OmniCode Runtime
// Type:        Header
// Component:   Stage 1 (Kernel) / OmniCode Subsystem
// Role:        Native OmniCode compilation and execution in MillenniumOS
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   John 1:1 — "In the beginning was the Word"
//              OmniCode IS the Word made executable.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_OMNICODE_H
#define MILLENNIUMOS_OMNICODE_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 OmniCode Constants
// -----------------------------------------------------------------------------

#define MOS_OMNI_VERSION_MAJOR      0
#define MOS_OMNI_VERSION_MINOR      1
#define MOS_OMNI_VERSION_STRING     "a-01.00"

// Max sizes (ternary aligned)
#define MOS_OMNI_MAX_SOURCE         19683       // 3^9 bytes per source file
#define MOS_OMNI_MAX_TOKENS         2187        // 3^7 tokens per file
#define MOS_OMNI_MAX_AST_NODES      729         // 3^6 AST nodes
#define MOS_OMNI_MAX_IR_OPS         2187        // 3^7 IR operations
#define MOS_OMNI_MAX_LOCALS         81          // 3^4 local variables
#define MOS_OMNI_MAX_FUNCTIONS      243         // 3^5 functions per module
#define MOS_OMNI_MAX_MODULES        27          // 3^3 loaded modules

// Output modes
#define MOS_OMNI_OUTPUT_C           TRIT_NEG    // -1: Transpile to C
#define MOS_OMNI_OUTPUT_IR          TRIT_ZERO   // 0: Generate IR bytecode
#define MOS_OMNI_OUTPUT_NATIVE      TRIT_POS    // +1: Native x86

// Execution modes
#define MOS_OMNI_EXEC_INTERPRET     TRIT_NEG    // -1: Interpret IR
#define MOS_OMNI_EXEC_JIT           TRIT_ZERO   // 0: JIT compile
#define MOS_OMNI_EXEC_NATIVE        TRIT_POS    // +1: Native execution

// -----------------------------------------------------------------------------
// B.2 Token Types
// -----------------------------------------------------------------------------

typedef enum {
    // Literals (0-26)
    MOS_TOK_INT             = 0,
    MOS_TOK_FLOAT           = 1,
    MOS_TOK_STRING          = 2,
    MOS_TOK_TRIT            = 3,
    MOS_TOK_BOOL            = 4,

    // Identifiers (27-53)
    MOS_TOK_IDENT           = 27,
    MOS_TOK_KEYWORD         = 28,
    MOS_TOK_TYPE            = 29,

    // Operators (54-80)
    MOS_TOK_PLUS            = 54,
    MOS_TOK_MINUS           = 55,
    MOS_TOK_STAR            = 56,
    MOS_TOK_SLASH           = 57,
    MOS_TOK_PERCENT         = 58,
    MOS_TOK_EQ              = 59,
    MOS_TOK_NE              = 60,
    MOS_TOK_LT              = 61,
    MOS_TOK_LE              = 62,
    MOS_TOK_GT              = 63,
    MOS_TOK_GE              = 64,
    MOS_TOK_AND             = 65,
    MOS_TOK_OR              = 66,
    MOS_TOK_NOT             = 67,
    MOS_TOK_ASSIGN          = 68,

    // Delimiters (81-107)
    MOS_TOK_LPAREN          = 81,
    MOS_TOK_RPAREN          = 82,
    MOS_TOK_LBRACE          = 83,
    MOS_TOK_RBRACE          = 84,
    MOS_TOK_LBRACKET        = 85,
    MOS_TOK_RBRACKET        = 86,
    MOS_TOK_COMMA           = 87,
    MOS_TOK_COLON           = 88,
    MOS_TOK_SEMICOLON       = 89,
    MOS_TOK_DOT             = 90,
    MOS_TOK_ARROW           = 91,

    // Special (108-134)
    MOS_TOK_OMNI_MARKER     = 108,  // @omni:
    MOS_TOK_BLOCK_START     = 109,  // [BLOCK]
    MOS_TOK_BLOCK_END       = 110,  // END BLOCK
    MOS_TOK_COMMENT         = 111,
    MOS_TOK_NEWLINE         = 112,
    MOS_TOK_EOF             = 113,
    MOS_TOK_ERROR           = 114,

    // OmniCode-specific (135-161)
    MOS_TOK_PRAGMA          = 135,  // #!omni
    MOS_TOK_REQ             = 136,  // @omni:req
    MOS_TOK_INH             = 137,  // @omni:inh
    MOS_TOK_INS             = 138,  // @omni:ins
    MOS_TOK_OPT             = 139,  // @omni:opt

    // Ternary operators (162-188)
    MOS_TOK_TRIT_NEG        = 162,  // -1 literal
    MOS_TOK_TRIT_ZERO       = 163,  // 0 literal
    MOS_TOK_TRIT_POS        = 164,  // +1 literal
    MOS_TOK_TRIT_ADD        = 165,  // Balanced ternary add
    MOS_TOK_TRIT_MUL        = 166,  // Balanced ternary multiply
    MOS_TOK_TRIT_NOT        = 167,  // Ternary negation
    MOS_TOK_TRIT_MIN        = 168,  // Ternary min
    MOS_TOK_TRIT_MAX        = 169,  // Ternary max
    MOS_TOK_TRIT_SUM        = 170,  // Sum consensus
    MOS_TOK_TRIT_ANY        = 171,  // Any non-zero

    MOS_TOK_COUNT           = 243   // Total token types (3^5)
} MosOmniTokenType;

// -----------------------------------------------------------------------------
// B.3 AST Node Types
// -----------------------------------------------------------------------------

typedef enum {
    // Program structure
    MOS_AST_PROGRAM         = 0,
    MOS_AST_MODULE          = 1,
    MOS_AST_BLOCK           = 2,

    // Declarations
    MOS_AST_FUNC_DECL       = 3,
    MOS_AST_VAR_DECL        = 4,
    MOS_AST_TYPE_DECL       = 5,
    MOS_AST_CONST_DECL      = 6,

    // Statements
    MOS_AST_IF              = 7,
    MOS_AST_WHILE           = 8,
    MOS_AST_FOR             = 9,
    MOS_AST_RETURN          = 10,
    MOS_AST_BREAK           = 11,
    MOS_AST_CONTINUE        = 12,
    MOS_AST_EXPR_STMT       = 13,

    // Expressions
    MOS_AST_BINARY          = 14,
    MOS_AST_UNARY           = 15,
    MOS_AST_CALL            = 16,
    MOS_AST_INDEX           = 17,
    MOS_AST_MEMBER          = 18,
    MOS_AST_IDENT           = 19,
    MOS_AST_LITERAL         = 20,
    MOS_AST_ARRAY           = 21,
    MOS_AST_STRUCT          = 22,

    // OmniCode-specific
    MOS_AST_PRAGMA          = 23,
    MOS_AST_OMNI_MARKER     = 24,
    MOS_AST_OMNI_BLOCK      = 25,

    MOS_AST_COUNT           = 27    // 3^3 node types
} MosOmniAstType;

// -----------------------------------------------------------------------------
// B.4 Structures
// -----------------------------------------------------------------------------

// Token
typedef struct {
    MosOmniTokenType    type;
    const char*         lexeme;
    mos_size_t          length;
    trit27_t            line;
    trit27_t            column;
    union {
        int64_t         int_val;
        double          float_val;
        trit_t          trit_val;
        const char*     string_val;
    } value;
} MosOmniToken;

// AST Node
typedef struct MosOmniAstNode {
    MosOmniAstType          type;
    MosOmniToken            token;
    struct MosOmniAstNode*  left;
    struct MosOmniAstNode*  right;
    struct MosOmniAstNode*  children;
    struct MosOmniAstNode*  next;
    mos_health_t            health;
    MosHebrewState          state;
} MosOmniAstNode;

// Lexer state
typedef struct {
    const char*         source;
    mos_size_t          length;
    mos_size_t          pos;
    trit27_t            line;
    trit27_t            column;
    MosOmniToken*       tokens;
    mos_size_t          token_count;
    mos_size_t          token_capacity;
    mos_health_t        health;
} MosOmniLexer;

// Parser state
typedef struct {
    MosOmniToken*       tokens;
    mos_size_t          token_count;
    mos_size_t          current;
    MosOmniAstNode*     root;
    mos_health_t        health;
    MosHebrewState      state;
    char                error[256];
} MosOmniParser;

// IR instruction (from tools/omnicode/pkg/omni IR spec)
typedef struct {
    uint8_t             opcode;
    trit_t              operands[3];
    trit27_t            immediate;
    mos_health_t        health;
} MosOmniIrOp;

// Compiled module
typedef struct {
    char                name[MOS_MAX_NAME];
    MosOmniIrOp*        code;
    mos_size_t          code_size;
    trit27_t*           constants;
    mos_size_t          const_count;
    mos_health_t        health;
    MosHebrewState      state;
} MosOmniModule;

// Runtime state
typedef struct {
    // Loaded modules
    MosOmniModule*      modules[MOS_OMNI_MAX_MODULES];
    mos_size_t          module_count;

    // Execution state
    trit_t              exec_mode;
    MosHebrewState      state;
    mos_health_t        health;

    // Statistics
    trit27_t            lines_compiled;
    trit27_t            ir_ops_executed;
    trit27_t            functions_called;

    // Configuration
    trit_t              output_mode;
    bool                debug;
    bool                verbose;
} MosOmniRuntime;

// -----------------------------------------------------------------------------
// B.5 Initialization
// -----------------------------------------------------------------------------

// Initialize OmniCode runtime
mos_result_t mos_omni_init(void);

// Shutdown OmniCode runtime
mos_result_t mos_omni_shutdown(void);

// Get runtime info
const MosOmniRuntime* mos_omni_info(void);

// -----------------------------------------------------------------------------
// B.6 Lexer API
// -----------------------------------------------------------------------------

// Create lexer
mos_result_t mos_omni_lexer_create(MosOmniLexer* lexer, const char* source,
                                    mos_size_t length);

// Tokenize source
mos_result_t mos_omni_lex(MosOmniLexer* lexer);

// Free lexer
void mos_omni_lexer_free(MosOmniLexer* lexer);

// -----------------------------------------------------------------------------
// B.7 Parser API
// -----------------------------------------------------------------------------

// Create parser
mos_result_t mos_omni_parser_create(MosOmniParser* parser, MosOmniToken* tokens,
                                     mos_size_t count);

// Parse tokens to AST
mos_result_t mos_omni_parse(MosOmniParser* parser);

// Free parser
void mos_omni_parser_free(MosOmniParser* parser);

// -----------------------------------------------------------------------------
// B.8 Compilation API
// -----------------------------------------------------------------------------

// Compile AST to IR
mos_result_t mos_omni_compile(MosOmniAstNode* ast, MosOmniModule* module);

// Compile to C (transpile)
mos_result_t mos_omni_transpile_c(MosOmniAstNode* ast, char* output,
                                   mos_size_t size);

// Compile to native x86
mos_result_t mos_omni_compile_native(MosOmniAstNode* ast, void** code,
                                      mos_size_t* size);

// -----------------------------------------------------------------------------
// B.9 Execution API
// -----------------------------------------------------------------------------

// Load module
mos_result_t mos_omni_load(const char* path, MosOmniModule** module);

// Execute module
mos_result_t mos_omni_exec(MosOmniModule* module, const char* entry_point,
                           trit27_t* result);

// Execute IR directly
mos_result_t mos_omni_exec_ir(MosOmniIrOp* code, mos_size_t count,
                               trit27_t* result);

// -----------------------------------------------------------------------------
// B.10 Full Pipeline
// -----------------------------------------------------------------------------

// Compile and execute file
mos_result_t mos_omni_run(const char* path, trit27_t* result);

// Compile and execute source buffer
mos_result_t mos_omni_eval(const char* source, mos_size_t length,
                           trit27_t* result);

// -----------------------------------------------------------------------------
// B.11 Module Management
// -----------------------------------------------------------------------------

// Register module
mos_result_t mos_omni_register(MosOmniModule* module);

// Unregister module
mos_result_t mos_omni_unregister(const char* name);

// Find module by name
MosOmniModule* mos_omni_find(const char* name);

// -----------------------------------------------------------------------------
// B.12 Health and State
// -----------------------------------------------------------------------------

// Get runtime health
mos_health_t mos_omni_health(void);

// Get runtime Hebrew state
MosHebrewState mos_omni_state(void);

// Check self-hosting capability
bool mos_omni_can_self_host(void);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// OmniCode Runtime for MillenniumOS:
//
//   1. LEXER: Source → Tokens (tokenize OmniCode pragma + code)
//   2. PARSER: Tokens → AST (build syntax tree)
//   3. COMPILER: AST → IR (generate trit-native bytecode)
//   4. EXECUTOR: IR → Result (interpret or JIT)
//
// Self-Hosting Path:
//   - OmniCode compiler written in OmniCode
//   - Bootstrapped via this C runtime (Stage 0)
//   - Self-compiles through Stage 1, 2, 3
//   - Trust verified by binary identity
//
// "In the beginning was the Word, and the Word was with God,
//  and the Word was God."
// — John 1:1
//
// OmniCode IS the Word made executable. Users write in OmniCode,
// MillenniumOS compiles and runs it natively.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // MILLENNIUMOS_OMNICODE_H
