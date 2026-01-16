// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-parser
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_parser.h — OmniCode AST Builder
// Parse semantic chunks into a structured tree.

#ifndef BERESHIT_OMNI_PARSER_H
#define BERESHIT_OMNI_PARSER_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-parser
// Title: OmniCode AST Parser
// Type: Header
// Role: Build Abstract Syntax Tree from semantic chunks

// # M.8 Grounding [GROUNDING]
//
// Scripture: 1 Corinthians 14:33 - God is not author of confusion, but of peace
// Principle: Structure reveals meaning - organized syntax enables understanding
// Anchor: Proverbs 24:27 - Prepare thy work without, make it fit

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stddef.h>
#include <stdbool.h>
#include "omni_chunks.h"

// # S.1 AST Node Types [NODE_TYPES]
//
// Hierarchical node types representing OmniCode structure:
//   Document → Pragma → Blocks → Sections → Statements → Values

typedef enum {
    // ─────────────────────────────────────────────────────────────────────────
    // Structural nodes
    // ─────────────────────────────────────────────────────────────────────────
    AST_DOCUMENT,               // Root node - entire file
    AST_PRAGMA,                 // #!omni directive line
    AST_BLOCK,                  // METADATA, SETUP, BODY, CONTENT, etc.
    AST_SECTION,                // Named section within a block
    AST_CODE_BLOCK,             // Indented code block (function body, etc.)

    // ─────────────────────────────────────────────────────────────────────────
    // Declaration nodes (metadata)
    // ─────────────────────────────────────────────────────────────────────────
    AST_ENTITY_DECL,            // "this is a folder/file/module for X"
    AST_IDENTITY,               // keyed as, named, following
    AST_CLASSIFICATION,         // zone classification

    // ─────────────────────────────────────────────────────────────────────────
    // Grounding nodes (Biblical foundation)
    // ─────────────────────────────────────────────────────────────────────────
    AST_GROUNDING_BLOCK,        // "grounded in:" section
    AST_SCRIPTURE,              // scripture "text" from Book C:V
    AST_PRINCIPLE,              // principle "text"
    AST_ANCHOR,                 // anchor "text"
    AST_SCRIPTURE_REF,          // Book Chapter:Verse reference

    // ─────────────────────────────────────────────────────────────────────────
    // Attribution nodes
    // ─────────────────────────────────────────────────────────────────────────
    AST_ATTRIBUTION_BLOCK,      // "authored by:" section
    AST_ARCHITECT,              // architect X
    AST_IMPLEMENTATION,         // implementation X
    AST_CREATED,                // created YYYY-MM-DD
    AST_MODIFIED,               // modified YYYY-MM-DD
    AST_VERSION,                // version X

    // ─────────────────────────────────────────────────────────────────────────
    // Lineage nodes
    // ─────────────────────────────────────────────────────────────────────────
    AST_LINEAGE_BLOCK,          // "derives from:" section
    AST_DERIVES_FROM,           // path to parent
    AST_BRANCH,                 // branch number

    // ─────────────────────────────────────────────────────────────────────────
    // Role nodes
    // ─────────────────────────────────────────────────────────────────────────
    AST_ROLE_BLOCK,             // "serves as:" section
    AST_ROLE,                   // role "X"
    AST_PURPOSE,                // purpose "X"
    AST_DESIGN,                 // design "X"
    AST_PHILOSOPHY,             // philosophy "X"

    // =========================================================================
    // EXECUTABLE CODE NODES (Layer 2 - Code Syntax)
    // =========================================================================

    // ─────────────────────────────────────────────────────────────────────────
    // Statement nodes - executable statements
    // ─────────────────────────────────────────────────────────────────────────
    AST_LET_STMT,               // "let X be Y" / "let X be TYPE of VALUE"
    AST_ASSIGN_STMT,            // "X be Y" (reassignment without let)
    AST_FUNC_DEF,               // "establish/provide work X"
    AST_ASSEMBLY_DEF,           // "establish assembly X:"
    AST_RETURN_STMT,            // "return X"
    AST_WITNESS_STMT,           // "witness X with level Y"
    AST_DISPLAY_STMT,           // "display X"
    AST_FAIL_STMT,              // "fail with X"
    AST_IMPORT_STMT,            // "import X"
    AST_MODULE_STMT,            // "module X"
    AST_SPAWN_STMT,             // "spawn X with Y"

    // ─────────────────────────────────────────────────────────────────────────
    // CPI-SI State nodes
    // ─────────────────────────────────────────────────────────────────────────
    AST_HEALTH_STMT,            // "health get/add/set"
    AST_STATE_STMT,             // "state get/transition"
    AST_CHECKPOINT_STMT,        // "checkpoint" (DAR snapshot)
    AST_RESTORE_STMT,           // "restore from X" (DAR recovery)
    AST_INTRINSIC_STMT,         // "intrinsic X with A and B" (VM intrinsic call)

    // ─────────────────────────────────────────────────────────────────────────
    // Control flow nodes
    // ─────────────────────────────────────────────────────────────────────────
    AST_WHEN_STMT,              // "when X is Y:" (conditional)
    AST_OTHERWISE_STMT,         // "otherwise:" / "otherwise when X:"
    AST_REPEAT_STMT,            // "repeat until X:" / "repeat X times:"
    AST_THROUGH_STMT,           // "through gathering X:"
    AST_FROM_STMT,              // "from X to Y:" / "from X to Y by Z:"
    AST_ATTEMPT_STMT,           // "attempt:" (try)
    AST_RECOVER_STMT,           // "recover from X:" (catch)
    AST_ENSURE_STMT,            // "ensure:" (finally)

    // ─────────────────────────────────────────────────────────────────────────
    // Expression nodes
    // ─────────────────────────────────────────────────────────────────────────
    AST_BINARY_EXPR,            // "X plus Y", "X is equal to Y", etc.
    AST_UNARY_EXPR,             // "not X"
    AST_CALL_EXPR,              // "process with X" / function call
    AST_MEMBER_EXPR,            // "X.Y" (field access)
    AST_INDEX_EXPR,             // "X at Y" (array index)
    AST_AWAIT_EXPR,             // "await X"
    AST_VAR_REF,                // Variable reference (identifier in expression)
    AST_GROUPED_EXPR,           // Parenthesized expression for precedence

    // ─────────────────────────────────────────────────────────────────────────
    // Literal nodes (leaves - specific values)
    // ─────────────────────────────────────────────────────────────────────────
    AST_COVENANT_LITERAL,       // granted / deferred / denied
    AST_TRUTH_LITERAL,          // yes / no
    AST_NONE_LITERAL,           // none

    // ─────────────────────────────────────────────────────────────────────────
    // Declaration helper nodes
    // ─────────────────────────────────────────────────────────────────────────
    AST_PARAM_DEF,              // Parameter: "word name", "count a"
    AST_PARAM_LIST,             // List of parameters
    AST_TYPE_REF,               // Type reference: covenant, word, gathering of X
    AST_MEANING,                // "meaning X" (documentation)
    AST_FUNC_SIGNATURE,         // "takes X and Y and returns Z"
    AST_FIELD_DEF,              // Assembly field: "name be word"

    // ─────────────────────────────────────────────────────────────────────────
    // Value nodes (leaves - general values)
    // ─────────────────────────────────────────────────────────────────────────
    AST_STRING_VALUE,           // "quoted string"
    AST_NUMBER_VALUE,           // 42, 3.14
    AST_DATE_VALUE,             // YYYY-MM-DD
    AST_VERSION_VALUE,          // a-01.00
    AST_IDENTIFIER,             // bare word

    // ─────────────────────────────────────────────────────────────────────────
    // Special nodes
    // ─────────────────────────────────────────────────────────────────────────
    AST_COMMENT,                // // comment text
    AST_RAW,                    // Unrecognized - preserved verbatim
    AST_ERROR,                  // Error node with diagnostic

    AST_NODE_TYPE_COUNT
} OmniAstNodeType;

// # S.2 Block Pattern Types [PATTERN_TYPES]
//
// OmniCode has three valid block patterns:

typedef enum {
    OMNI_PATTERN_UNKNOWN,       // Not yet determined
    OMNI_PATTERN_3BLOCK,        // DATA: METADATA → CONTENT → CLOSING
    OMNI_PATTERN_4BLOCK,        // CODE: METADATA → SETUP → BODY → CLOSING
    OMNI_PATTERN_5BLOCK         // DOCS: METADATA → HEADER → CONTEXT → CONTENT → FOOTER
} OmniBlockPattern;

// # S.2b Operator Types [OP_TYPES]
//
// Operators for AST_BINARY_EXPR and AST_UNARY_EXPR nodes.

typedef enum {
    // ─────────────────────────────────────────────────────────────────────────
    // Arithmetic operators
    // ─────────────────────────────────────────────────────────────────────────
    OP_PLUS,                    // "plus" → +
    OP_MINUS,                   // "minus" → -
    OP_MULTIPLY,                // "multiplied by" → *
    OP_DIVIDE,                  // "divided by" → /
    OP_MODULO,                  // "modulo" → %
    OP_POWER,                   // "to the power of" → pow()

    // ─────────────────────────────────────────────────────────────────────────
    // Comparison operators
    // ─────────────────────────────────────────────────────────────────────────
    OP_EQUAL,                   // "is equal to" → ==
    OP_NOT_EQUAL,               // "is not equal to" → !=
    OP_GREATER,                 // "is greater than" → >
    OP_LESS,                    // "is less than" → <
    OP_GREATER_EQ,              // "is at least" → >=
    OP_LESS_EQ,                 // "is at most" → <=

    // ─────────────────────────────────────────────────────────────────────────
    // Logical operators
    // ─────────────────────────────────────────────────────────────────────────
    OP_AND,                     // "and" → &&
    OP_OR,                      // "or" → ||
    OP_NOT,                     // "not" → ! (unary)

    // ─────────────────────────────────────────────────────────────────────────
    // String operators
    // ─────────────────────────────────────────────────────────────────────────
    OP_JOIN,                    // "joined with" → concatenation

    // ─────────────────────────────────────────────────────────────────────────
    // Type operators
    // ─────────────────────────────────────────────────────────────────────────
    OP_IS,                      // "is" → type check / equality
    OP_AS,                      // "as" → type cast

    OP_UNKNOWN                  // Unknown operator
} OmniOpType;

// # S.2c Visibility [VISIBILITY]
//
// Function/member visibility: establish = private, provide = public.

typedef enum {
    VIS_PRIVATE,                // establish work → internal
    VIS_PUBLIC                  // provide work → exported
} OmniVisibility;

// # S.2d Covenant Values [COVENANT]
//
// Ternary covenant literal values.

typedef enum {
    COV_GRANTED  = +1,          // Affirmative
    COV_DEFERRED =  0,          // Pending/uncertain
    COV_DENIED   = -1           // Negative
} OmniCovenantValue;

// # S.3 AST Node [AST_NODE]
//
// A single node in the AST tree.
// Uses child/sibling representation for flexible tree structure.

#define AST_VALUE_SIZE 512
#define AST_BOOK_SIZE 64
#define AST_REF_SIZE 32

typedef struct OmniAstNode {
    OmniAstNodeType type;

    // Source location
    int line;
    int column;

    // Primary value (name, key, text content)
    char value[AST_VALUE_SIZE];

    // Secondary value (for scripture: book name; for blocks: block type name)
    char secondary[128];

    // For AST_SCRIPTURE_REF nodes
    char book[AST_BOOK_SIZE];
    char chapter[AST_REF_SIZE];
    char verse[AST_REF_SIZE];

    // For AST_BLOCK nodes - which block type
    OmniBlockContext block_type;

    // ─────────────────────────────────────────────────────────────────────────
    // Fields for executable code nodes
    // ─────────────────────────────────────────────────────────────────────────

    // For AST_BINARY_EXPR / AST_UNARY_EXPR
    OmniOpType op_type;

    // For AST_FUNC_DEF - visibility (establish = private, provide = public)
    OmniVisibility visibility;

    // For AST_COVENANT_LITERAL
    OmniCovenantValue covenant_value;

    // For AST_TRUTH_LITERAL
    bool truth_value;

    // For AST_NUMBER_VALUE - numeric value
    double number_value;
    bool is_integer;

    // For AST_FUNC_DEF - async modifier
    bool is_async;

    // For AST_TYPE_REF - the type name (covenant, word, count, gathering, etc.)
    char type_name[64];

    // For AST_TYPE_REF with "gathering of X" - the element type
    char element_type[64];

    // For AST_TYPE_REF with "X or none" - optional flag
    bool is_optional;

    // Tree structure (left-child, right-sibling)
    struct OmniAstNode* parent;
    struct OmniAstNode* first_child;
    struct OmniAstNode* last_child;      // For O(1) append
    struct OmniAstNode* next_sibling;
} OmniAstNode;

// # S.4 Document Structure [DOCUMENT]
//
// Represents a complete parsed OmniCode document.

typedef struct OmniDocument {
    OmniAstNode* root;                    // AST_DOCUMENT node

    // Pragma metadata (quick access)
    char pragma_type[64];                 // "document", "code", "data"
    char pragma_flags[128];               // "--folder", "--c", etc.
    char meta_key[256];                   // #!omni meta.key value
    char meta_from[256];                  // #!omni meta.from value
    char meta_at[64];                     // #!omni meta.at value

    // Block pattern validation
    OmniBlockPattern expected_pattern;
    bool has_metadata;
    bool has_header;                      // 5-block only
    bool has_context;                     // 5-block only
    bool has_setup;                       // 4-block only
    bool has_body;                        // 4-block only
    bool has_content;                     // 3/5-block
    bool has_closing;                     // 3/4-block
    bool has_footer;                      // 5-block only

    // Statistics
    int node_count;
    int error_count;

    // Memory pool for nodes (arena allocator)
    OmniAstNode* node_pool;
    int pool_capacity;
    int pool_used;
} OmniDocument;

// # S.5 Parser State [PARSER]
//
// Parser state machine for building AST from chunks.

typedef struct {
    OmniChunkLexer* chunk_lexer;          // Source of chunks
    OmniDocument* document;               // Document being built
    OmniAstNode* current_block;           // Current block node
    OmniAstNode* current_section;         // Current section node
    bool had_error;
    bool at_end;

    // Lookahead buffer
    OmniChunk lookahead[4];
    int lookahead_count;
} OmniParser;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Document Lifecycle [LIFECYCLE]

// Create new document with node pool
// Allocates pool for max_nodes AST nodes.
OmniDocument* omni_document_create(int max_nodes);

// Destroy document and free all memory
void omni_document_destroy(OmniDocument* doc);

// # B.2 Parser Lifecycle [PARSER_LIFECYCLE]

// Initialize parser with chunk lexer
// The parser does NOT own the chunk lexer - caller manages its lifetime.
void omni_parser_init(OmniParser* parser, OmniChunkLexer* chunk_lexer);

// Parse entire source into document
// Returns NULL on allocation failure.
OmniDocument* omni_parser_parse(OmniParser* parser);

// Parse with provided document (for reuse)
// Returns true on success, false on error.
bool omni_parser_parse_into(OmniParser* parser, OmniDocument* doc);

// # B.3 AST Node Operations [NODE_OPS]

// Allocate node from document pool
// Returns NULL if pool exhausted.
OmniAstNode* omni_node_alloc(OmniDocument* doc);

// Initialize node with type and location
void omni_node_init(OmniAstNode* node, OmniAstNodeType type, int line, int col);

// Add child to parent (appends to end)
void omni_node_add_child(OmniAstNode* parent, OmniAstNode* child);

// # B.4 AST Queries [QUERIES]

// Get node type name for display
const char* omni_ast_type_name(OmniAstNodeType type);

// Get block pattern name
const char* omni_block_pattern_name(OmniBlockPattern pattern);

// Get operator type name for display
const char* omni_op_type_name(OmniOpType op);

// Get C operator string for operator type
const char* omni_op_type_to_c(OmniOpType op);

// Get visibility name for display
const char* omni_visibility_name(OmniVisibility vis);

// Get covenant value name for display
const char* omni_covenant_name(OmniCovenantValue cov);

// Count children of node
int omni_node_child_count(const OmniAstNode* node);

// Find first child of specific type
OmniAstNode* omni_node_find_child(OmniAstNode* node, OmniAstNodeType type);

// # B.5 Block Structure Validation [VALIDATION]

// Infer expected block pattern from pragma
OmniBlockPattern omni_infer_block_pattern(const char* pragma_type,
                                           const char* pragma_flags);

// Validate document has correct block structure
// Returns true if valid, adds diagnostics if invalid.
bool omni_validate_structure(OmniDocument* doc);

// Check if document is complete (all required blocks present)
bool omni_document_is_complete(const OmniDocument* doc);

// # B.6 Document Queries [DOC_QUERIES]

// Get document title from metadata
const char* omni_document_get_key(const OmniDocument* doc);

// Get entity type (folder, file, module)
const char* omni_document_get_entity_type(const OmniDocument* doc);

// Get entity description
const char* omni_document_get_description(const OmniDocument* doc);

// # B.7 AST Traversal [TRAVERSAL]

// Callback for AST traversal
typedef void (*OmniAstVisitor)(OmniAstNode* node, int depth, void* ctx);

// Walk AST depth-first, calling visitor for each node
void omni_ast_walk(OmniAstNode* root, OmniAstVisitor visitor, void* ctx);

// # B.8 AST Printing [PRINTING]

// Print AST to stdout (for debugging)
void omni_ast_print(const OmniDocument* doc);

// Print AST node with indent
void omni_ast_print_node(const OmniAstNode* node, int indent);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Pattern [USAGE]
//
// // Initialize chunk lexer with config
// OmniChunkLexer chunk_lexer;
// omni_chunk_init_with_config(&chunk_lexer, source, len, &config);
//
// // Initialize parser
// OmniParser parser;
// omni_parser_init(&parser, &chunk_lexer);
//
// // Parse to document
// OmniDocument* doc = omni_parser_parse(&parser);
// if (!doc) {
//     fprintf(stderr, "Parse failed\n");
//     return 1;
// }
//
// // Use document
// printf("Key: %s\n", doc->meta_key);
// printf("Structure: %s\n", omni_block_structure_name(doc->expected_structure));
// omni_ast_print(doc);
//
// // Validate structure
// if (!omni_validate_structure(doc)) {
//     fprintf(stderr, "Invalid block structure\n");
// }
//
// // Cleanup
// omni_document_destroy(doc);

// # X.2 Closing Note [NOTE]
//
// "For God is not the author of confusion, but of peace."
//   — 1 Corinthians 14:33
//
// Structure reveals meaning. The parser transforms flat chunks
// into a tree that reflects OmniCode's intentional organization.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_PARSER_H
