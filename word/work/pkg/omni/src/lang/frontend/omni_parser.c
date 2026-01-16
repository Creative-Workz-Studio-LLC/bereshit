// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-parser-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_parser.c — OmniCode AST Builder Implementation
// Transforms semantic chunks into structured syntax tree.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-parser-impl
// Title: OmniCode Parser Implementation
// Type: Source
// Role: Build AST from chunk stream using arena allocation

// # M.8 Grounding [GROUNDING]
//
// Scripture: 1 Corinthians 14:33 - God is not author of confusion, but of peace
// Principle: Order from chaos - transform stream into structure
// Anchor: Genesis 1:2-3 - From formless void to organized creation

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omni_parser.h"

// # S.1 Constants [CONSTANTS]

#define DEFAULT_NODE_POOL 1024
#define MAX_PRINT_DEPTH 32

// # S.2 Internal Helpers [HELPERS]

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

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Document Lifecycle [LIFECYCLE]

OmniDocument* omni_document_create(int max_nodes) {
    if (max_nodes <= 0) max_nodes = DEFAULT_NODE_POOL;

    OmniDocument* doc = calloc(1, sizeof(OmniDocument));
    if (!doc) return NULL;

    doc->node_pool = calloc(max_nodes, sizeof(OmniAstNode));
    if (!doc->node_pool) {
        free(doc);
        return NULL;
    }

    doc->pool_capacity = max_nodes;
    doc->pool_used = 0;
    doc->expected_pattern = OMNI_PATTERN_UNKNOWN;

    return doc;
}

void omni_document_destroy(OmniDocument* doc) {
    if (!doc) return;
    free(doc->node_pool);
    free(doc);
}

// # B.2 Node Operations [NODE_OPS]

OmniAstNode* omni_node_alloc(OmniDocument* doc) {
    if (!doc || doc->pool_used >= doc->pool_capacity) {
        return NULL;
    }

    OmniAstNode* node = &doc->node_pool[doc->pool_used++];
    memset(node, 0, sizeof(OmniAstNode));
    doc->node_count++;
    return node;
}

void omni_node_init(OmniAstNode* node, OmniAstNodeType type, int line, int col) {
    if (!node) return;
    node->type = type;
    node->line = line;
    node->column = col;
}

void omni_node_add_child(OmniAstNode* parent, OmniAstNode* child) {
    if (!parent || !child) return;

    child->parent = parent;
    child->next_sibling = NULL;

    if (!parent->first_child) {
        parent->first_child = child;
        parent->last_child = child;
    } else {
        parent->last_child->next_sibling = child;
        parent->last_child = child;
    }
}

// # B.3 Type Names [TYPE_NAMES]

const char* omni_ast_type_name(OmniAstNodeType type) {
    static const char* names[] = {
        // Structural nodes
        [AST_DOCUMENT]          = "DOCUMENT",
        [AST_PRAGMA]            = "PRAGMA",
        [AST_BLOCK]             = "BLOCK",
        [AST_SECTION]           = "SECTION",
        [AST_CODE_BLOCK]        = "CODE_BLOCK",

        // Metadata nodes
        [AST_ENTITY_DECL]       = "ENTITY_DECL",
        [AST_IDENTITY]          = "IDENTITY",
        [AST_CLASSIFICATION]    = "CLASSIFICATION",
        [AST_GROUNDING_BLOCK]   = "GROUNDING_BLOCK",
        [AST_SCRIPTURE]         = "SCRIPTURE",
        [AST_PRINCIPLE]         = "PRINCIPLE",
        [AST_ANCHOR]            = "ANCHOR",
        [AST_SCRIPTURE_REF]     = "SCRIPTURE_REF",
        [AST_ATTRIBUTION_BLOCK] = "ATTRIBUTION_BLOCK",
        [AST_ARCHITECT]         = "ARCHITECT",
        [AST_IMPLEMENTATION]    = "IMPLEMENTATION",
        [AST_CREATED]           = "CREATED",
        [AST_MODIFIED]          = "MODIFIED",
        [AST_VERSION]           = "VERSION",
        [AST_LINEAGE_BLOCK]     = "LINEAGE_BLOCK",
        [AST_DERIVES_FROM]      = "DERIVES_FROM",
        [AST_BRANCH]            = "BRANCH",
        [AST_ROLE_BLOCK]        = "ROLE_BLOCK",
        [AST_ROLE]              = "ROLE",
        [AST_PURPOSE]           = "PURPOSE",
        [AST_DESIGN]            = "DESIGN",
        [AST_PHILOSOPHY]        = "PHILOSOPHY",

        // Statement nodes (executable)
        [AST_LET_STMT]          = "LET_STMT",
        [AST_ASSIGN_STMT]       = "ASSIGN_STMT",
        [AST_FUNC_DEF]          = "FUNC_DEF",
        [AST_ASSEMBLY_DEF]      = "ASSEMBLY_DEF",
        [AST_RETURN_STMT]       = "RETURN_STMT",
        [AST_WITNESS_STMT]      = "WITNESS_STMT",
        [AST_DISPLAY_STMT]      = "DISPLAY_STMT",
        [AST_FAIL_STMT]         = "FAIL_STMT",
        [AST_IMPORT_STMT]       = "IMPORT_STMT",
        [AST_MODULE_STMT]       = "MODULE_STMT",
        [AST_SPAWN_STMT]        = "SPAWN_STMT",

        // Control flow nodes
        [AST_WHEN_STMT]         = "WHEN_STMT",
        [AST_OTHERWISE_STMT]    = "OTHERWISE_STMT",
        [AST_REPEAT_STMT]       = "REPEAT_STMT",
        [AST_THROUGH_STMT]      = "THROUGH_STMT",
        [AST_FROM_STMT]         = "FROM_STMT",
        [AST_ATTEMPT_STMT]      = "ATTEMPT_STMT",
        [AST_RECOVER_STMT]      = "RECOVER_STMT",
        [AST_ENSURE_STMT]       = "ENSURE_STMT",

        // Expression nodes
        [AST_BINARY_EXPR]       = "BINARY_EXPR",
        [AST_UNARY_EXPR]        = "UNARY_EXPR",
        [AST_CALL_EXPR]         = "CALL_EXPR",
        [AST_MEMBER_EXPR]       = "MEMBER_EXPR",
        [AST_INDEX_EXPR]        = "INDEX_EXPR",
        [AST_AWAIT_EXPR]        = "AWAIT_EXPR",
        [AST_VAR_REF]           = "VAR_REF",
        [AST_GROUPED_EXPR]      = "GROUPED_EXPR",

        // Literal nodes
        [AST_COVENANT_LITERAL]  = "COVENANT_LITERAL",
        [AST_TRUTH_LITERAL]     = "TRUTH_LITERAL",
        [AST_NONE_LITERAL]      = "NONE_LITERAL",

        // Declaration helper nodes
        [AST_PARAM_DEF]         = "PARAM_DEF",
        [AST_PARAM_LIST]        = "PARAM_LIST",
        [AST_TYPE_REF]          = "TYPE_REF",
        [AST_MEANING]           = "MEANING",
        [AST_FUNC_SIGNATURE]    = "FUNC_SIGNATURE",
        [AST_FIELD_DEF]         = "FIELD_DEF",

        // Value nodes
        [AST_STRING_VALUE]      = "STRING_VALUE",
        [AST_NUMBER_VALUE]      = "NUMBER_VALUE",
        [AST_DATE_VALUE]        = "DATE_VALUE",
        [AST_VERSION_VALUE]     = "VERSION_VALUE",
        [AST_IDENTIFIER]        = "IDENTIFIER",

        // Special nodes
        [AST_COMMENT]           = "COMMENT",
        [AST_RAW]               = "RAW",
        [AST_ERROR]             = "ERROR",
    };

    if (type >= 0 && type < AST_NODE_TYPE_COUNT) {
        return names[type] ? names[type] : "UNKNOWN";
    }
    return "INVALID";
}

const char* omni_block_pattern_name(OmniBlockPattern structure) {
    switch (structure) {
        case OMNI_PATTERN_3BLOCK: return "3-block (DATA)";
        case OMNI_PATTERN_4BLOCK: return "4-block (CODE)";
        case OMNI_PATTERN_5BLOCK: return "5-block (DOCS)";
        default: return "unknown";
    }
}

const char* omni_op_type_name(OmniOpType op) {
    switch (op) {
        // Arithmetic
        case OP_PLUS:       return "plus";
        case OP_MINUS:      return "minus";
        case OP_MULTIPLY:   return "multiplied by";
        case OP_DIVIDE:     return "divided by";
        case OP_MODULO:     return "modulo";
        case OP_POWER:      return "to the power of";
        // Comparison
        case OP_EQUAL:      return "is equal to";
        case OP_NOT_EQUAL:  return "is not equal to";
        case OP_GREATER:    return "is greater than";
        case OP_LESS:       return "is less than";
        case OP_GREATER_EQ: return "is at least";
        case OP_LESS_EQ:    return "is at most";
        // Logical
        case OP_AND:        return "and";
        case OP_OR:         return "or";
        case OP_NOT:        return "not";
        // String
        case OP_JOIN:       return "joined with";
        // Type
        case OP_IS:         return "is";
        case OP_AS:         return "as";
        default:            return "unknown";
    }
}

const char* omni_op_type_to_c(OmniOpType op) {
    switch (op) {
        // Arithmetic
        case OP_PLUS:       return "+";
        case OP_MINUS:      return "-";
        case OP_MULTIPLY:   return "*";
        case OP_DIVIDE:     return "/";
        case OP_MODULO:     return "%";
        case OP_POWER:      return "pow";  // function call
        // Comparison
        case OP_EQUAL:      return "==";
        case OP_NOT_EQUAL:  return "!=";
        case OP_GREATER:    return ">";
        case OP_LESS:       return "<";
        case OP_GREATER_EQ: return ">=";
        case OP_LESS_EQ:    return "<=";
        // Logical
        case OP_AND:        return "&&";
        case OP_OR:         return "||";
        case OP_NOT:        return "!";
        // String (special handling needed)
        case OP_JOIN:       return "/* join */";
        // Type
        case OP_IS:         return "==";
        case OP_AS:         return "(cast)";
        default:            return "/* unknown */";
    }
}

const char* omni_visibility_name(OmniVisibility vis) {
    switch (vis) {
        case VIS_PRIVATE: return "private";
        case VIS_PUBLIC:  return "public";
        default:          return "unknown";
    }
}

const char* omni_covenant_name(OmniCovenantValue cov) {
    switch (cov) {
        case COV_GRANTED:  return "granted";
        case COV_DEFERRED: return "deferred";
        case COV_DENIED:   return "denied";
        default:           return "unknown";
    }
}

// # B.4 Node Queries [QUERIES]

int omni_node_child_count(const OmniAstNode* node) {
    if (!node) return 0;
    int count = 0;
    OmniAstNode* child = node->first_child;
    while (child) {
        count++;
        child = child->next_sibling;
    }
    return count;
}

OmniAstNode* omni_node_find_child(OmniAstNode* node, OmniAstNodeType type) {
    if (!node) return NULL;
    OmniAstNode* child = node->first_child;
    while (child) {
        if (child->type == type) return child;
        child = child->next_sibling;
    }
    return NULL;
}

// # B.5 Block Structure Inference [INFERENCE]

OmniBlockPattern omni_infer_block_pattern(const char* pragma_type,
                                               const char* pragma_flags) {
    if (!pragma_type) return OMNI_PATTERN_UNKNOWN;

    // Documentation always 5-block
    if (strcmp(pragma_type, "document") == 0 ||
        strcmp(pragma_type, "documentation") == 0) {
        return OMNI_PATTERN_5BLOCK;
    }

    // Code always 4-block
    if (strcmp(pragma_type, "code") == 0) {
        return OMNI_PATTERN_4BLOCK;
    }

    // Data is 3-block by default
    if (strcmp(pragma_type, "data") == 0) {
        // But could be 4 or 5 block with flags
        if (pragma_flags) {
            if (strstr(pragma_flags, "--schema") ||
                strstr(pragma_flags, "--complex")) {
                return OMNI_PATTERN_4BLOCK;
            }
            if (strstr(pragma_flags, "--documented")) {
                return OMNI_PATTERN_5BLOCK;
            }
        }
        return OMNI_PATTERN_3BLOCK;
    }

    // Interface is 3-block
    if (strcmp(pragma_type, "interface") == 0) {
        return OMNI_PATTERN_3BLOCK;
    }

    // Template inherits from what it templates
    if (strcmp(pragma_type, "template") == 0) {
        if (pragma_flags) {
            if (strstr(pragma_flags, "--header") ||
                strstr(pragma_flags, "--source") ||
                strstr(pragma_flags, "--code")) {
                return OMNI_PATTERN_4BLOCK;
            }
            if (strstr(pragma_flags, "--doc") ||
                strstr(pragma_flags, "--adoc")) {
                return OMNI_PATTERN_5BLOCK;
            }
        }
        return OMNI_PATTERN_3BLOCK;
    }

    return OMNI_PATTERN_UNKNOWN;
}

// # B.6 Parser Implementation [PARSER]

void omni_parser_init(OmniParser* parser, OmniChunkLexer* chunk_lexer) {
    if (!parser) return;
    memset(parser, 0, sizeof(OmniParser));
    parser->chunk_lexer = chunk_lexer;
}

// Get next chunk (with lookahead support)
static OmniChunk parser_next(OmniParser* parser) {
    if (parser->lookahead_count > 0) {
        OmniChunk chunk = parser->lookahead[0];
        // Shift lookahead buffer
        for (int i = 0; i < parser->lookahead_count - 1; i++) {
            parser->lookahead[i] = parser->lookahead[i + 1];
        }
        parser->lookahead_count--;
        return chunk;
    }

    return omni_chunk_next(parser->chunk_lexer);
}

// Peek at next chunk without consuming
static OmniChunk parser_peek(OmniParser* parser) {
    if (parser->lookahead_count == 0) {
        parser->lookahead[0] = omni_chunk_next(parser->chunk_lexer);
        parser->lookahead_count = 1;
    }
    return parser->lookahead[0];
}

// Check if at end
static bool parser_at_end(OmniParser* parser) {
    OmniChunk peek = parser_peek(parser);
    return peek.type == CHUNK_EOF;
}

// Skip newlines and comments
static void parser_skip_whitespace(OmniParser* parser) {
    while (!parser_at_end(parser)) {
        OmniChunk peek = parser_peek(parser);
        if (peek.type == CHUNK_NEWLINE || peek.type == CHUNK_COMMENT) {
            parser_next(parser);
        } else {
            break;
        }
    }
}

// Map chunk block type to AST block context
static OmniBlockContext chunk_to_block_context(OmniChunkType type) {
    switch (type) {
        case CHUNK_BLOCK_METADATA: return BLOCK_METADATA;
        case CHUNK_BLOCK_HEADER:   return BLOCK_HEADER;
        case CHUNK_BLOCK_CONTEXT:  return BLOCK_CONTEXT;
        case CHUNK_BLOCK_SETUP:    return BLOCK_SETUP;
        case CHUNK_BLOCK_BODY:     return BLOCK_BODY;
        case CHUNK_BLOCK_CONTENT:  return BLOCK_CONTENT;
        case CHUNK_BLOCK_CLOSING:  return BLOCK_CLOSING;
        case CHUNK_BLOCK_FOOTER:   return BLOCK_FOOTER;
        default: return BLOCK_NONE;
    }
}

// Parse a single chunk into an AST node
static OmniAstNode* parse_chunk_to_node(OmniParser* parser, OmniChunk* chunk) {
    OmniDocument* doc = parser->document;
    OmniAstNode* node = omni_node_alloc(doc);
    if (!node) return NULL;

    node->line = chunk->line;
    node->column = chunk->column;

    switch (chunk->type) {
        // Pragma chunks
        case CHUNK_PRAGMA:
            node->type = AST_PRAGMA;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->secondary_value);
            break;

        case CHUNK_META_KEY:
            node->type = AST_PRAGMA;
            safe_strcpy(node->value, sizeof(node->value), "meta.key");
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);
            safe_strcpy(doc->meta_key, sizeof(doc->meta_key), chunk->primary_value);
            break;

        case CHUNK_META_FROM:
            node->type = AST_PRAGMA;
            safe_strcpy(node->value, sizeof(node->value), "meta.from");
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);
            safe_strcpy(doc->meta_from, sizeof(doc->meta_from), chunk->primary_value);
            break;

        case CHUNK_META_AT:
            node->type = AST_PRAGMA;
            safe_strcpy(node->value, sizeof(node->value), "meta.at");
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);
            safe_strcpy(doc->meta_at, sizeof(doc->meta_at), chunk->primary_value);
            break;

        // Block markers
        case CHUNK_BLOCK_METADATA:
        case CHUNK_BLOCK_HEADER:
        case CHUNK_BLOCK_CONTEXT:
        case CHUNK_BLOCK_SETUP:
        case CHUNK_BLOCK_BODY:
        case CHUNK_BLOCK_CONTENT:
        case CHUNK_BLOCK_CLOSING:
        case CHUNK_BLOCK_FOOTER:
            node->type = AST_BLOCK;
            node->block_type = chunk_to_block_context(chunk->type);
            safe_strcpy(node->value, sizeof(node->value),
                        omni_block_context_name(node->block_type));
            // Store START/END in secondary
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);

            // Track which blocks we've seen
            switch (chunk->type) {
                case CHUNK_BLOCK_METADATA: doc->has_metadata = true; break;
                case CHUNK_BLOCK_HEADER:   doc->has_header = true; break;
                case CHUNK_BLOCK_CONTEXT:  doc->has_context = true; break;
                case CHUNK_BLOCK_SETUP:    doc->has_setup = true; break;
                case CHUNK_BLOCK_BODY:     doc->has_body = true; break;
                case CHUNK_BLOCK_CONTENT:  doc->has_content = true; break;
                case CHUNK_BLOCK_CLOSING:  doc->has_closing = true; break;
                case CHUNK_BLOCK_FOOTER:   doc->has_footer = true; break;
                default: break;
            }
            break;

        // Entity declarations
        case CHUNK_ENTITY_FOLDER:
            node->type = AST_ENTITY_DECL;
            safe_strcpy(node->value, sizeof(node->value), "folder");
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);
            break;

        case CHUNK_ENTITY_FILE:
            node->type = AST_ENTITY_DECL;
            safe_strcpy(node->value, sizeof(node->value), "file");
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);
            break;

        case CHUNK_ENTITY_MODULE:
            node->type = AST_ENTITY_DECL;
            safe_strcpy(node->value, sizeof(node->value), "module");
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);
            break;

        // Identity chunks
        case CHUNK_IDENTITY_KEY:
            node->type = AST_IDENTITY;
            safe_strcpy(node->value, sizeof(node->value), "keyed as");
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);
            break;

        case CHUNK_IDENTITY_NAME:
            node->type = AST_IDENTITY;
            safe_strcpy(node->value, sizeof(node->value), "named");
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);
            break;

        case CHUNK_IDENTITY_STRUCTURE:
            node->type = AST_IDENTITY;
            safe_strcpy(node->value, sizeof(node->value), "following");
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->primary_value);
            break;

        // Classification
        case CHUNK_CLASSIFY_ZONE:
            node->type = AST_CLASSIFICATION;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            if (chunk->has_ternary) {
                char buf[32];
                snprintf(buf, sizeof(buf), "%d", chunk->ternary_value);
                safe_strcpy(node->secondary, sizeof(node->secondary), buf);
            }
            break;

        // Grounding chunks
        case CHUNK_GROUND_HEADER:
            node->type = AST_GROUNDING_BLOCK;
            safe_strcpy(node->value, sizeof(node->value), "grounded in");
            break;

        case CHUNK_GROUND_SCRIPTURE:
            node->type = AST_SCRIPTURE;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            safe_strcpy(node->book, sizeof(node->book), chunk->book);
            safe_strcpy(node->chapter, sizeof(node->chapter), chunk->chapter);
            safe_strcpy(node->verse, sizeof(node->verse), chunk->verse);
            break;

        case CHUNK_GROUND_PRINCIPLE:
            node->type = AST_PRINCIPLE;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_GROUND_ANCHOR:
            node->type = AST_ANCHOR;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_SCRIPTURE_REF:
            node->type = AST_SCRIPTURE_REF;
            safe_strcpy(node->book, sizeof(node->book), chunk->book);
            safe_strcpy(node->chapter, sizeof(node->chapter), chunk->chapter);
            safe_strcpy(node->verse, sizeof(node->verse), chunk->verse);
            snprintf(node->value, sizeof(node->value), "%s %s:%s",
                     chunk->book, chunk->chapter, chunk->verse);
            break;

        // Role chunks
        case CHUNK_ROLE_HEADER:
            node->type = AST_ROLE_BLOCK;
            safe_strcpy(node->value, sizeof(node->value), "serves as");
            break;

        case CHUNK_ROLE_ROLE:
            node->type = AST_ROLE;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_ROLE_PURPOSE:
            node->type = AST_PURPOSE;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_ROLE_DESIGN:
            node->type = AST_DESIGN;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_ROLE_PHILOSOPHY:
            node->type = AST_PHILOSOPHY;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // Attribution chunks
        case CHUNK_ATTR_HEADER:
            node->type = AST_ATTRIBUTION_BLOCK;
            safe_strcpy(node->value, sizeof(node->value), "authored by");
            break;

        case CHUNK_ATTR_ARCHITECT:
            node->type = AST_ARCHITECT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_ATTR_IMPLEMENTATION:
            node->type = AST_IMPLEMENTATION;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_ATTR_CREATED:
            node->type = AST_CREATED;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_ATTR_MODIFIED:
            node->type = AST_MODIFIED;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_ATTR_VERSION:
            node->type = AST_VERSION;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // Lineage chunks
        case CHUNK_LINEAGE_DERIVES:
            node->type = AST_DERIVES_FROM;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_LINEAGE_BRANCH:
            node->type = AST_BRANCH;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            if (chunk->has_ternary) {
                char buf[32];
                snprintf(buf, sizeof(buf), "%d", chunk->ternary_value);
                safe_strcpy(node->secondary, sizeof(node->secondary), buf);
            }
            break;

        // Value chunks
        case CHUNK_STRING_VALUE:
            node->type = AST_STRING_VALUE;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // Note: CHUNK_NUMBER_VALUE handled in executable code section

        case CHUNK_DATE_VALUE:
            node->type = AST_DATE_VALUE;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_VERSION_VALUE:
            node->type = AST_VERSION_VALUE;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // Comments
        case CHUNK_COMMENT:
            node->type = AST_COMMENT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // ═══════════════════════════════════════════════════════════════════
        // Executable Code Chunks
        // "Let there be light: and there was light" — Genesis 1:3
        // ═══════════════════════════════════════════════════════════════════

        // Variable declarations: "let name be type" / "let name be type with value"
        case CHUNK_LET_DECL:
        case CHUNK_LET_INIT:
            node->type = AST_LET_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // name
            safe_strcpy(node->type_name, sizeof(node->type_name), chunk->secondary_value);  // type
            if (chunk->type == CHUNK_LET_INIT) {
                safe_strcpy(node->secondary, sizeof(node->secondary), chunk->tertiary_value); // init value
            }
            break;

        // Function definitions: "establish work name that takes params returns type:"
        case CHUNK_FUNC_DEF:
        case CHUNK_FUNC_PROVIDE:
            node->type = AST_FUNC_DEF;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // name
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->secondary_value); // params
            safe_strcpy(node->type_name, sizeof(node->type_name), chunk->tertiary_value);  // return type
            node->visibility = (chunk->type == CHUNK_FUNC_PROVIDE) ? VIS_PUBLIC : VIS_PRIVATE;
            break;

        // Return statement: "return value"
        case CHUNK_FUNC_RETURN:
            node->type = AST_RETURN_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // When conditional: "when condition:"
        case CHUNK_WHEN_COND:
            node->type = AST_WHEN_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // Otherwise clause: "otherwise:"
        case CHUNK_OTHERWISE:
            node->type = AST_OTHERWISE_STMT;
            break;

        // Repeat until: "repeat until condition:"
        case CHUNK_REPEAT_UNTIL:
            node->type = AST_REPEAT_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // Through each: "through each item in collection:"
        case CHUNK_THROUGH_EACH:
            node->type = AST_THROUGH_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // iterator var
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->secondary_value); // collection
            break;

        // From-to loop: "from start to end:"
        case CHUNK_FROM_TO:
            node->type = AST_FROM_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // start
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->secondary_value); // end
            if (chunk->tertiary_value[0]) {
                // Store step in type_name (reusing field)
                safe_strcpy(node->type_name, sizeof(node->type_name), chunk->tertiary_value);
            }
            break;

        // Error handling: attempt/recover/ensure
        case CHUNK_ATTEMPT:
            node->type = AST_ATTEMPT_STMT;
            break;

        case CHUNK_RECOVER:
            node->type = AST_RECOVER_STMT;
            break;

        case CHUNK_ENSURE:
            node->type = AST_ENSURE_STMT;
            break;

        case CHUNK_FAIL_WITH:
            node->type = AST_FAIL_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // Module system
        case CHUNK_IMPORT:
            node->type = AST_IMPORT_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // import name
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->secondary_value); // from path
            break;

        // Concurrency
        case CHUNK_SPAWN:
            node->type = AST_SPAWN_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        case CHUNK_AWAIT:
            node->type = AST_AWAIT_EXPR;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // Type definition: "create assembly name:"
        case CHUNK_ASSEMBLY_DEF:
            node->type = AST_ASSEMBLY_DEF;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // Witness (logging): "witness message with level"
        case CHUNK_WITNESS:
            node->type = AST_WITNESS_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // message
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->secondary_value); // level
            break;

        // Display: "display value"
        case CHUNK_DISPLAY:
            node->type = AST_DISPLAY_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        // CPI-SI State: "health get/add/set"
        case CHUNK_HEALTH:
            node->type = AST_HEALTH_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // operation
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->secondary_value); // value
            break;

        // CPI-SI State: "state get/transition"
        case CHUNK_STATE:
            node->type = AST_STATE_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // operation
            safe_strcpy(node->secondary, sizeof(node->secondary), chunk->secondary_value); // k-factor
            break;

        // DAR: "checkpoint [as name]"
        case CHUNK_CHECKPOINT:
            node->type = AST_CHECKPOINT_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // name (optional)
            break;

        // DAR: "restore [from name]"
        case CHUNK_RESTORE:
            node->type = AST_RESTORE_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // name (optional)
            break;

        // VM Intrinsic: "intrinsic name [args]"
        case CHUNK_INTRINSIC:
            node->type = AST_INTRINSIC_STMT;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);      // intrinsic name
            // Store args in secondary (space-separated)
            {
                char args[256] = "";
                for (int i = 0; i < chunk->token_count && i < 4; i++) {
                    if (i > 0) strcat(args, " ");
                    strncat(args, chunk->tokens[i].text, 60);
                }
                safe_strcpy(node->secondary, sizeof(node->secondary), args);
            }
            break;

        // Covenant literal: granted/deferred/denied
        case CHUNK_COVENANT_VALUE:
            node->type = AST_COVENANT_LITERAL;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            if (chunk->has_ternary) {
                if (chunk->ternary_value > 0) {
                    node->covenant_value = COV_GRANTED;
                } else if (chunk->ternary_value < 0) {
                    node->covenant_value = COV_DENIED;
                } else {
                    node->covenant_value = COV_DEFERRED;
                }
            }
            break;

        // Truth literal: yes/no
        case CHUNK_TRUTH_VALUE:
            node->type = AST_TRUTH_LITERAL;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            node->truth_value = (chunk->ternary_value > 0);
            break;

        // None literal
        case CHUNK_NONE_VALUE:
            node->type = AST_NONE_LITERAL;
            safe_strcpy(node->value, sizeof(node->value), "none");
            break;

        // Number literal
        case CHUNK_NUMBER_VALUE:
            node->type = AST_NUMBER_VALUE;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            // Parse the number
            node->number_value = atof(chunk->primary_value);
            node->is_integer = (strchr(chunk->primary_value, '.') == NULL);
            break;

        // Type reference
        case CHUNK_TYPE_REF:
            node->type = AST_TYPE_REF;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            safe_strcpy(node->type_name, sizeof(node->type_name), chunk->primary_value);
            break;

        // ═══════════════════════════════════════════════════════════════════
        // End Executable Code Chunks
        // ═══════════════════════════════════════════════════════════════════

        // Raw/unrecognized
        case CHUNK_RAW_TOKEN:
            node->type = AST_RAW;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;

        default:
            node->type = AST_RAW;
            safe_strcpy(node->value, sizeof(node->value), chunk->primary_value);
            break;
    }

    return node;
}

// Main parse loop
bool omni_parser_parse_into(OmniParser* parser, OmniDocument* doc) {
    if (!parser || !doc || !parser->chunk_lexer) return false;

    parser->document = doc;

    // Create root document node
    doc->root = omni_node_alloc(doc);
    if (!doc->root) return false;

    omni_node_init(doc->root, AST_DOCUMENT, 1, 1);

    // Parse all chunks
    while (!parser_at_end(parser)) {
        // Skip leading whitespace and comments
        parser_skip_whitespace(parser);
        if (parser_at_end(parser)) break;

        OmniChunk chunk = parser_next(parser);

        // Skip pure whitespace that got through
        if (chunk.type == CHUNK_NEWLINE) continue;

        // Convert chunk to AST node
        OmniAstNode* node = parse_chunk_to_node(parser, &chunk);
        if (!node) {
            parser->had_error = true;
            doc->error_count++;
            continue;
        }

        // Handle pragma - extract type and flags for structure inference
        if (chunk.type == CHUNK_PRAGMA) {
            safe_strcpy(doc->pragma_type, sizeof(doc->pragma_type),
                        chunk.primary_value);
            safe_strcpy(doc->pragma_flags, sizeof(doc->pragma_flags),
                        chunk.secondary_value);

            // Infer expected structure from pragma
            doc->expected_pattern = omni_infer_block_pattern(
                doc->pragma_type, doc->pragma_flags);
        }

        // Handle block markers - update current block
        if (node->type == AST_BLOCK) {
            parser->current_block = node;
            parser->current_section = NULL;  // Reset section
        }

        // Add node to tree
        if (parser->current_block && node->type != AST_BLOCK) {
            // Add to current block
            omni_node_add_child(parser->current_block, node);
        } else {
            // Add to document root
            omni_node_add_child(doc->root, node);
        }
    }

    return !parser->had_error;
}

OmniDocument* omni_parser_parse(OmniParser* parser) {
    if (!parser) return NULL;

    OmniDocument* doc = omni_document_create(DEFAULT_NODE_POOL);
    if (!doc) return NULL;

    if (!omni_parser_parse_into(parser, doc)) {
        // Parse failed but we still return the document with errors
        // so caller can inspect what went wrong
    }

    return doc;
}

// # B.7 Block Structure Validation [VALIDATION]

bool omni_validate_structure(OmniDocument* doc) {
    if (!doc) return false;

    bool valid = true;

    switch (doc->expected_pattern) {
        case OMNI_PATTERN_3BLOCK:
            // METADATA → CONTENT → CLOSING
            if (!doc->has_metadata) {
                valid = false;
            }
            if (!doc->has_content) {
                valid = false;
            }
            if (!doc->has_closing) {
                valid = false;
            }
            break;

        case OMNI_PATTERN_4BLOCK:
            // METADATA → SETUP → BODY → CLOSING
            if (!doc->has_metadata) {
                valid = false;
            }
            if (!doc->has_setup) {
                valid = false;
            }
            if (!doc->has_body) {
                valid = false;
            }
            if (!doc->has_closing) {
                valid = false;
            }
            break;

        case OMNI_PATTERN_5BLOCK:
            // METADATA → HEADER → CONTEXT → CONTENT → FOOTER
            if (!doc->has_metadata) {
                valid = false;
            }
            if (!doc->has_header) {
                valid = false;
            }
            if (!doc->has_context) {
                valid = false;
            }
            if (!doc->has_content) {
                valid = false;
            }
            if (!doc->has_footer) {
                valid = false;
            }
            break;

        default:
            // Unknown structure - can't validate
            break;
    }

    return valid;
}

bool omni_document_is_complete(const OmniDocument* doc) {
    if (!doc) return false;
    return omni_validate_structure((OmniDocument*)doc);
}

// # B.8 Document Queries [DOC_QUERIES]

const char* omni_document_get_key(const OmniDocument* doc) {
    if (!doc) return NULL;
    return doc->meta_key[0] ? doc->meta_key : NULL;
}

const char* omni_document_get_entity_type(const OmniDocument* doc) {
    if (!doc || !doc->root) return NULL;

    // Find first entity declaration
    OmniAstNode* child = doc->root->first_child;
    while (child) {
        if (child->type == AST_BLOCK) {
            OmniAstNode* block_child = child->first_child;
            while (block_child) {
                if (block_child->type == AST_ENTITY_DECL) {
                    return block_child->value;
                }
                block_child = block_child->next_sibling;
            }
        } else if (child->type == AST_ENTITY_DECL) {
            return child->value;
        }
        child = child->next_sibling;
    }

    return NULL;
}

const char* omni_document_get_description(const OmniDocument* doc) {
    if (!doc || !doc->root) return NULL;

    // Find first entity declaration's secondary value
    OmniAstNode* child = doc->root->first_child;
    while (child) {
        if (child->type == AST_BLOCK) {
            OmniAstNode* block_child = child->first_child;
            while (block_child) {
                if (block_child->type == AST_ENTITY_DECL) {
                    return block_child->secondary;
                }
                block_child = block_child->next_sibling;
            }
        } else if (child->type == AST_ENTITY_DECL) {
            return child->secondary;
        }
        child = child->next_sibling;
    }

    return NULL;
}

// # B.9 AST Traversal [TRAVERSAL]

static void walk_node(OmniAstNode* node, int depth, OmniAstVisitor visitor, void* ctx) {
    if (!node || depth > MAX_PRINT_DEPTH) return;

    visitor(node, depth, ctx);

    OmniAstNode* child = node->first_child;
    while (child) {
        walk_node(child, depth + 1, visitor, ctx);
        child = child->next_sibling;
    }
}

void omni_ast_walk(OmniAstNode* root, OmniAstVisitor visitor, void* ctx) {
    if (!root || !visitor) return;
    walk_node(root, 0, visitor, ctx);
}

// # B.10 AST Printing [PRINTING]

void omni_ast_print_node(const OmniAstNode* node, int indent) {
    if (!node || indent > MAX_PRINT_DEPTH) return;

    // Print indent
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    // Print node type and value
    printf("[%s]", omni_ast_type_name(node->type));

    if (node->value[0]) {
        printf(" \"%s\"", node->value);
    }
    if (node->secondary[0]) {
        printf(" (%s)", node->secondary);
    }
    if (node->type == AST_SCRIPTURE_REF || node->type == AST_SCRIPTURE) {
        if (node->book[0]) {
            printf(" [%s %s:%s]", node->book, node->chapter, node->verse);
        }
    }

    printf(" @%d:%d\n", node->line, node->column);

    // Print children
    OmniAstNode* child = node->first_child;
    while (child) {
        omni_ast_print_node(child, indent + 1);
        child = child->next_sibling;
    }
}

void omni_ast_print(const OmniDocument* doc) {
    if (!doc) {
        printf("(null document)\n");
        return;
    }

    printf("════════════════════════════════════════════════════════════════\n");
    printf("OmniCode Document AST\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    printf("Pragma Type: %s\n", doc->pragma_type[0] ? doc->pragma_type : "(none)");
    printf("Pragma Flags: %s\n", doc->pragma_flags[0] ? doc->pragma_flags : "(none)");
    printf("Meta Key: %s\n", doc->meta_key[0] ? doc->meta_key : "(none)");
    printf("Meta From: %s\n", doc->meta_from[0] ? doc->meta_from : "(none)");
    printf("Meta At: %s\n", doc->meta_at[0] ? doc->meta_at : "(none)");
    printf("Expected Structure: %s\n", omni_block_pattern_name(doc->expected_pattern));
    printf("Node Count: %d\n", doc->node_count);
    printf("Error Count: %d\n", doc->error_count);

    printf("\n────────────────────────────────────────────────────────────────\n");
    printf("Blocks Present:\n");
    printf("────────────────────────────────────────────────────────────────\n");

    printf("  METADATA: %s\n", doc->has_metadata ? "yes" : "no");
    printf("  HEADER:   %s\n", doc->has_header ? "yes" : "no");
    printf("  CONTEXT:  %s\n", doc->has_context ? "yes" : "no");
    printf("  SETUP:    %s\n", doc->has_setup ? "yes" : "no");
    printf("  BODY:     %s\n", doc->has_body ? "yes" : "no");
    printf("  CONTENT:  %s\n", doc->has_content ? "yes" : "no");
    printf("  CLOSING:  %s\n", doc->has_closing ? "yes" : "no");
    printf("  FOOTER:   %s\n", doc->has_footer ? "yes" : "no");

    printf("\n────────────────────────────────────────────────────────────────\n");
    printf("AST Tree:\n");
    printf("────────────────────────────────────────────────────────────────\n\n");

    if (doc->root) {
        omni_ast_print_node(doc->root, 0);
    } else {
        printf("(no root node)\n");
    }

    printf("\n════════════════════════════════════════════════════════════════\n");
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Closing Note [NOTE]
//
// "For God is not the author of confusion, but of peace."
//   — 1 Corinthians 14:33
//
// The parser transforms a stream of semantic chunks into a structured tree.
// Order emerges from the sequence, revealing the document's true form.

// =============================================================================
// END CLOSING
// =============================================================================
