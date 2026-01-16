// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-codegen
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_codegen.h — OmniCode to C Code Generator
// Translates OmniCode AST to C code using libtrit types.

#ifndef BERESHIT_OMNI_CODEGEN_H
#define BERESHIT_OMNI_CODEGEN_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-codegen
// Title: OmniCode Code Generator
// Type: Header
// Role: Define interface for OmniCode → C code translation

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:3 - "Let there be light: and there was light"
// Principle: Words that CREATE - OmniCode sentences become executable code
// Anchor: John 1:1 - "In the beginning was the Word"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdbool.h>
#include "omni_parser.h"

// # S.1 Code Generation Result [RESULT]
//
// Generation result codes (ternary semantics)

typedef enum {
    CODEGEN_GRANTED  = +1,      // Success - code generated
    CODEGEN_DEFERRED =  0,      // Partial - some code generated with warnings
    CODEGEN_DENIED   = -1       // Failure - code generation failed
} OmniCodeGenResult;

// # S.2 Type Mapping Entry [TYPE_ENTRY]
//
// Maps OmniCode type to C type representation

typedef struct {
    const char* omni_type;      // OmniCode type name
    const char* c_type;         // Corresponding C type
    const char* c_default;      // Default value in C
    const char* include;        // Required #include (or NULL)
} OmniTypeMapping;

// # S.3 Value Mapping Entry [VALUE_ENTRY]
//
// Maps OmniCode values to C values

typedef struct {
    const char* omni_value;     // OmniCode value
    const char* c_value;        // Corresponding C value
} OmniValueMapping;

// # S.4 Operator Mapping Entry [OP_ENTRY]
//
// Maps OmniCode operators to C operators

typedef struct {
    const char* omni_op;        // OmniCode operator phrase
    const char* c_op;           // C operator or function
    int precedence;             // For proper parenthesization
    bool binary;                // true = binary, false = unary
} OmniOpMapping;

// # S.5 Code Generator Context [CONTEXT]
//
// Maintains state during code generation.

#define CODEGEN_ERROR_SIZE 1024
#define CODEGEN_MAX_ERRORS 32
#define CODEGEN_MAX_INDENT 32

typedef struct {
    // Output
    FILE* output;               // Output file/stream
    bool own_output;            // If true, close output on destroy

    // Indentation
    int indent_level;           // Current indent (0-based)
    int indent_width;           // Spaces per indent level (default: 4)
    bool at_line_start;         // True if at start of new line

    // Scope tracking
    bool in_function;           // Inside a function body
    bool in_block;              // Inside a code block
    const char* current_fn;     // Current function name (or NULL)
    int scope_depth;            // Nested scope depth

    // Source document
    OmniDocument* doc;          // Document being generated

    // Statistics
    int functions_emitted;      // Count of functions emitted
    int statements_emitted;     // Count of statements emitted
    int lines_emitted;          // Count of lines emitted

    // Error tracking
    struct {
        char message[CODEGEN_ERROR_SIZE];
        int line;
        int column;
    } errors[CODEGEN_MAX_ERRORS];
    int error_count;
    int warning_count;

    // Health tracking (ternary)
    int health;                 // Current health score (-100 to +100)

    // Feature flags
    bool emit_comments;         // Include source line comments
    bool emit_debug;            // Include debug macros
    bool emit_health;           // Include health tracking
} OmniCodeGen;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Lifecycle [LIFECYCLE]
//
// Create, configure, and destroy code generator.

// Initialize code generator with output stream.
// If output is NULL, uses stdout.
void omni_codegen_init(OmniCodeGen* gen, FILE* output);

// Initialize code generator with file path.
// Opens file for writing. Returns false if file cannot be opened.
bool omni_codegen_init_file(OmniCodeGen* gen, const char* path);

// Destroy code generator, flush output.
void omni_codegen_destroy(OmniCodeGen* gen);

// Reset generator for reuse with new document.
void omni_codegen_reset(OmniCodeGen* gen);

// # B.2 Configuration [CONFIG]
//
// Configure generator behavior.

// Set indentation width (default: 4)
void omni_codegen_set_indent(OmniCodeGen* gen, int width);

// Enable/disable source line comments
void omni_codegen_set_comments(OmniCodeGen* gen, bool enable);

// Enable/disable debug macro emission
void omni_codegen_set_debug(OmniCodeGen* gen, bool enable);

// Enable/disable health tracking code
void omni_codegen_set_health(OmniCodeGen* gen, bool enable);

// # B.3 Generation [GENERATION]
//
// Main code generation interface.

// Generate C code from OmniCode document.
// Returns result code indicating success/failure.
OmniCodeGenResult omni_codegen_generate(OmniCodeGen* gen, OmniDocument* doc);

// Generate C header file (includes, forward declarations)
OmniCodeGenResult omni_codegen_emit_header(OmniCodeGen* gen);

// Generate C source body (functions, main)
OmniCodeGenResult omni_codegen_emit_body(OmniCodeGen* gen);

// Generate C footer (cleanup, closing)
OmniCodeGenResult omni_codegen_emit_footer(OmniCodeGen* gen);

// # B.4 Type Mapping [TYPE_MAPPING]
//
// OmniCode type → C type translation.

// Get C type for OmniCode type name.
// Returns NULL if type is unknown.
const OmniTypeMapping* omni_type_map_lookup(const char* omni_type);

// Get C type string for OmniCode type.
const char* omni_type_to_c(const char* omni_type);

// Get C default value for OmniCode type.
const char* omni_type_default_c(const char* omni_type);

// Get required #include for OmniCode type.
const char* omni_type_include_c(const char* omni_type);

// # B.5 Value Mapping [VALUE_MAPPING]
//
// OmniCode value → C value translation.

// Get C value for OmniCode value.
// Returns NULL if value is unknown.
const char* omni_value_to_c(const char* omni_value);

// Covenant values (ternary)
const char* omni_covenant_to_c(const char* covenant_state);

// # B.6 Operator Mapping [OP_MAPPING]
//
// OmniCode operator → C operator translation.

// Get C operator for OmniCode operator phrase.
const OmniOpMapping* omni_op_map_lookup(const char* omni_op);

// Get C operator string.
const char* omni_op_to_c(const char* omni_op);

// # B.7 Emission Helpers [EMISSION]
//
// Low-level code emission functions.

// Emit raw string (no indent, no newline)
void omni_emit_raw(OmniCodeGen* gen, const char* text);

// Emit formatted string (no indent, no newline)
void omni_emit_fmt(OmniCodeGen* gen, const char* fmt, ...);

// Emit with current indent level
void omni_emit_indent(OmniCodeGen* gen);

// Emit line with indent
void omni_emit_line(OmniCodeGen* gen, const char* line);

// Emit formatted line with indent
void omni_emit_linef(OmniCodeGen* gen, const char* fmt, ...);

// Emit blank line
void omni_emit_blank(OmniCodeGen* gen);

// Emit C comment
void omni_emit_comment(OmniCodeGen* gen, const char* text);

// Emit C block comment (/* ... */)
void omni_emit_block_comment(OmniCodeGen* gen, const char* text);

// Start indented block (emit { and increase indent)
void omni_emit_block_start(OmniCodeGen* gen);

// End indented block (decrease indent and emit })
void omni_emit_block_end(OmniCodeGen* gen);

// # B.8 Statement Emission [STATEMENTS]
//
// Emit specific C constructs.

// Emit variable declaration
void omni_emit_var_decl(OmniCodeGen* gen, const char* c_type,
                        const char* name, const char* init_value);

// Emit function signature start
void omni_emit_func_start(OmniCodeGen* gen, const char* c_return_type,
                          const char* name, const char* params);

// Emit function end
void omni_emit_func_end(OmniCodeGen* gen);

// Emit if statement start
void omni_emit_if_start(OmniCodeGen* gen, const char* condition);

// Emit else if
void omni_emit_else_if(OmniCodeGen* gen, const char* condition);

// Emit else
void omni_emit_else(OmniCodeGen* gen);

// Emit return statement
void omni_emit_return(OmniCodeGen* gen, const char* value);

// Emit printf (for witness statements)
void omni_emit_witness(OmniCodeGen* gen, const char* message, const char* level);

// # B.9 AST Node Emission [AST_EMISSION]
//
// Generate C code from specific AST node types.

// Emit code for any node (dispatcher)
OmniCodeGenResult omni_emit_node(OmniCodeGen* gen, OmniAstNode* node);

// Emit code for a block
OmniCodeGenResult omni_emit_block(OmniCodeGen* gen, OmniAstNode* block);

// Emit code for a statement
OmniCodeGenResult omni_emit_statement(OmniCodeGen* gen, OmniAstNode* stmt);

// Emit code for an expression
OmniCodeGenResult omni_emit_expression(OmniCodeGen* gen, OmniAstNode* expr);

// # B.10 Error Handling [ERRORS]
//
// Error reporting and tracking.

// Report error at location
void omni_codegen_error(OmniCodeGen* gen, int line, int col,
                        const char* fmt, ...);

// Report warning at location
void omni_codegen_warning(OmniCodeGen* gen, int line, int col,
                          const char* fmt, ...);

// Get error count
int omni_codegen_error_count(const OmniCodeGen* gen);

// Get warning count
int omni_codegen_warning_count(const OmniCodeGen* gen);

// Print all errors to stderr
void omni_codegen_print_errors(const OmniCodeGen* gen);

// # B.11 Health Tracking [HEALTH]
//
// Track generation health (ternary system).

// Adjust health score
void omni_codegen_health_adjust(OmniCodeGen* gen, int delta, const char* reason);

// Get current health score
int omni_codegen_health_get(const OmniCodeGen* gen);

// Get health state name (granted/deferred/denied)
const char* omni_codegen_health_state(const OmniCodeGen* gen);

// # B.12 Queries [QUERIES]
//
// Query generator state.

// Get result name for display
const char* omni_codegen_result_name(OmniCodeGenResult result);

// Get total lines emitted
int omni_codegen_lines(const OmniCodeGen* gen);

// Get total functions emitted
int omni_codegen_functions(const OmniCodeGen* gen);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Pattern [USAGE]
//
// // Initialize generator
// OmniCodeGen gen;
// omni_codegen_init(&gen, stdout);
// omni_codegen_set_comments(&gen, true);
//
// // Parse source
// OmniChunkLexer chunk_lexer;
// omni_chunk_init(&chunk_lexer, source, len);
// OmniParser parser;
// omni_parser_init(&parser, &chunk_lexer);
// OmniDocument* doc = omni_parser_parse(&parser);
//
// // Generate C code
// OmniCodeGenResult result = omni_codegen_generate(&gen, doc);
// if (result == CODEGEN_DENIED) {
//     omni_codegen_print_errors(&gen);
// }
//
// // Cleanup
// omni_codegen_destroy(&gen);
// omni_document_destroy(doc);

// # X.2 Closing Note [NOTE]
//
// "Let there be light: and there was light."
//   — Genesis 1:3
//
// OmniCode sentences become executable code. Words that DO things.
// The code generator bridges divine pattern (sentence) and machine
// execution (compiled C with libtrit).

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_CODEGEN_H
