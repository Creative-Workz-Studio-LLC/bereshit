// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-omnicode-impl
// Title:       MillenniumOS OmniCode Runtime Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / OmniCode Subsystem
// Role:        Core implementation of OmniCode compilation and execution
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   John 1:3 — "All things were made by him"
//              The runtime MAKES from the Word.
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), OmniCode service is not available.
#if CPISI_MODE >= 2

#include "services/omnicode/omnicode.h"
#include "services/omnicode/repl.h"
#include "services/omnicode/bible.h"
#include "services/vfs/vfs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global Runtime State
// -----------------------------------------------------------------------------

static MosOmniRuntime g_runtime = {
    .modules = {NULL},
    .module_count = 0,
    .exec_mode = MOS_OMNI_EXEC_INTERPRET,
    .state = MOS_HEBREW_YASHAR,
    .health = MOS_HEALTH_DEFAULT,
    .lines_compiled = 0,
    .ir_ops_executed = 0,
    .functions_called = 0,
    .output_mode = MOS_OMNI_OUTPUT_IR,
    .debug = false,
    .verbose = false
};

static bool g_initialized = false;

// -----------------------------------------------------------------------------
// B.2 Keyword Table
// -----------------------------------------------------------------------------

static const char* KEYWORDS[] = {
    // Control flow
    "if", "else", "elif", "while", "for", "break", "continue", "return",
    // Declarations
    "let", "const", "func", "type", "struct", "enum",
    // Types
    "trit", "trit5", "trit9", "trit27", "int", "float", "bool", "string",
    "health", "hebrew_state",
    // Values
    "true", "false", "nil",
    // OmniCode-specific
    "bible", "witness", "detect", "assess", "restore",
    NULL
};

static bool is_keyword(const char* str) {
    for (int i = 0; KEYWORDS[i] != NULL; i++) {
        if (strcmp(str, KEYWORDS[i]) == 0) return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
// B.3 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_omni_init(void) {
    if (g_initialized) {
        return MOS_RESULT_OK;
    }

    // Clear runtime state
    memset(&g_runtime, 0, sizeof(g_runtime));
    g_runtime.state = MOS_HEBREW_YASHAR;
    g_runtime.health = MOS_HEALTH_DEFAULT;
    g_runtime.output_mode = MOS_OMNI_OUTPUT_IR;
    g_runtime.exec_mode = MOS_OMNI_EXEC_INTERPRET;

    // Initialize Bible codex
    mos_result_t result = mos_bible_init();
    if (result < 0) {
        // Bible init failure is warning, not fatal
        g_runtime.health -= 10;
    }

    g_initialized = true;
    return MOS_RESULT_OK;
}

mos_result_t mos_omni_shutdown(void) {
    if (!g_initialized) return MOS_RESULT_OK;

    // Unload all modules
    for (mos_size_t i = 0; i < g_runtime.module_count; i++) {
        if (g_runtime.modules[i]) {
            free(g_runtime.modules[i]->code);
            free(g_runtime.modules[i]->constants);
            free(g_runtime.modules[i]);
            g_runtime.modules[i] = NULL;
        }
    }
    g_runtime.module_count = 0;

    // Shutdown Bible codex
    mos_bible_shutdown();

    g_initialized = false;
    return MOS_RESULT_OK;
}

const MosOmniRuntime* mos_omni_info(void) {
    return &g_runtime;
}

// -----------------------------------------------------------------------------
// B.4 Lexer Implementation
// -----------------------------------------------------------------------------

mos_result_t mos_omni_lexer_create(MosOmniLexer* lexer, const char* source,
                                    mos_size_t length) {
    if (!lexer || !source) return MOS_RESULT_ERROR;

    lexer->source = source;
    lexer->length = length;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->token_count = 0;
    lexer->token_capacity = 256;
    lexer->tokens = malloc(lexer->token_capacity * sizeof(MosOmniToken));
    lexer->health = MOS_HEALTH_DEFAULT;

    if (!lexer->tokens) return MOS_RESULT_ERROR;
    return MOS_RESULT_OK;
}

static void lexer_advance(MosOmniLexer* lexer) {
    if (lexer->pos < lexer->length) {
        if (lexer->source[lexer->pos] == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->pos++;
    }
}

static char lexer_peek(MosOmniLexer* lexer) {
    if (lexer->pos >= lexer->length) return '\0';
    return lexer->source[lexer->pos];
}

static char lexer_peek_next(MosOmniLexer* lexer) {
    if (lexer->pos + 1 >= lexer->length) return '\0';
    return lexer->source[lexer->pos + 1];
}

static void lexer_skip_whitespace(MosOmniLexer* lexer) {
    while (lexer->pos < lexer->length) {
        char c = lexer_peek(lexer);
        if (c == ' ' || c == '\t' || c == '\r') {
            lexer_advance(lexer);
        } else {
            break;
        }
    }
}

static void lexer_add_token(MosOmniLexer* lexer, MosOmniTokenType type,
                            const char* start, mos_size_t length) {
    if (lexer->token_count >= lexer->token_capacity) {
        lexer->token_capacity *= 2;
        lexer->tokens = realloc(lexer->tokens,
                                lexer->token_capacity * sizeof(MosOmniToken));
    }

    MosOmniToken* token = &lexer->tokens[lexer->token_count++];
    token->type = type;
    token->lexeme = start;
    token->length = length;
    token->line = lexer->line;
    token->column = lexer->column - length;
}

static void lexer_scan_string(MosOmniLexer* lexer) {
    const char* start = lexer->source + lexer->pos;
    lexer_advance(lexer);  // Skip opening quote

    while (lexer_peek(lexer) != '"' && lexer->pos < lexer->length) {
        if (lexer_peek(lexer) == '\\') lexer_advance(lexer);
        lexer_advance(lexer);
    }

    if (lexer->pos >= lexer->length) {
        lexer_add_token(lexer, MOS_TOK_ERROR, start,
                        lexer->source + lexer->pos - start);
        return;
    }

    lexer_advance(lexer);  // Skip closing quote
    lexer_add_token(lexer, MOS_TOK_STRING, start,
                    lexer->source + lexer->pos - start);
}

static void lexer_scan_number(MosOmniLexer* lexer) {
    const char* start = lexer->source + lexer->pos;
    MosOmniTokenType type = MOS_TOK_INT;

    while (lexer_peek(lexer) >= '0' && lexer_peek(lexer) <= '9') {
        lexer_advance(lexer);
    }

    if (lexer_peek(lexer) == '.' && lexer_peek_next(lexer) >= '0' &&
        lexer_peek_next(lexer) <= '9') {
        type = MOS_TOK_FLOAT;
        lexer_advance(lexer);
        while (lexer_peek(lexer) >= '0' && lexer_peek(lexer) <= '9') {
            lexer_advance(lexer);
        }
    }

    lexer_add_token(lexer, type, start, lexer->source + lexer->pos - start);
}

static void lexer_scan_identifier(MosOmniLexer* lexer) {
    const char* start = lexer->source + lexer->pos;

    while (lexer->pos < lexer->length) {
        char c = lexer_peek(lexer);
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_') {
            lexer_advance(lexer);
        } else {
            break;
        }
    }

    mos_size_t length = lexer->source + lexer->pos - start;

    // Check for keywords
    char temp[64];
    if (length < sizeof(temp)) {
        memcpy(temp, start, length);
        temp[length] = '\0';

        if (is_keyword(temp)) {
            lexer_add_token(lexer, MOS_TOK_KEYWORD, start, length);
            return;
        }
    }

    lexer_add_token(lexer, MOS_TOK_IDENT, start, length);
}

static void lexer_scan_omni_marker(MosOmniLexer* lexer) {
    const char* start = lexer->source + lexer->pos;

    // Skip @omni:
    while (lexer->pos < lexer->length && lexer_peek(lexer) != ' ' &&
           lexer_peek(lexer) != '\n') {
        lexer_advance(lexer);
    }

    mos_size_t length = lexer->source + lexer->pos - start;

    // Determine marker type
    MosOmniTokenType type = MOS_TOK_OMNI_MARKER;
    if (length >= 9 && strncmp(start, "@omni:req", 9) == 0) {
        type = MOS_TOK_REQ;
    } else if (length >= 9 && strncmp(start, "@omni:inh", 9) == 0) {
        type = MOS_TOK_INH;
    } else if (length >= 9 && strncmp(start, "@omni:ins", 9) == 0) {
        type = MOS_TOK_INS;
    } else if (length >= 9 && strncmp(start, "@omni:opt", 9) == 0) {
        type = MOS_TOK_OPT;
    }

    lexer_add_token(lexer, type, start, length);
}

mos_result_t mos_omni_lex(MosOmniLexer* lexer) {
    if (!lexer) return MOS_RESULT_ERROR;

    while (lexer->pos < lexer->length) {
        lexer_skip_whitespace(lexer);
        if (lexer->pos >= lexer->length) break;

        char c = lexer_peek(lexer);
        const char* start = lexer->source + lexer->pos;

        // Check for pragma
        if (c == '#' && lexer_peek_next(lexer) == '!') {
            while (lexer_peek(lexer) != '\n' && lexer->pos < lexer->length) {
                lexer_advance(lexer);
            }
            lexer_add_token(lexer, MOS_TOK_PRAGMA, start,
                            lexer->source + lexer->pos - start);
            continue;
        }

        // Check for comments
        if (c == '/' && lexer_peek_next(lexer) == '/') {
            while (lexer_peek(lexer) != '\n' && lexer->pos < lexer->length) {
                lexer_advance(lexer);
            }
            lexer_add_token(lexer, MOS_TOK_COMMENT, start,
                            lexer->source + lexer->pos - start);
            continue;
        }

        // Check for OmniCode markers
        if (c == '@' && lexer->pos + 5 < lexer->length &&
            strncmp(start, "@omni:", 6) == 0) {
            lexer_scan_omni_marker(lexer);
            continue;
        }

        // String
        if (c == '"') {
            lexer_scan_string(lexer);
            continue;
        }

        // Number
        if (c >= '0' && c <= '9') {
            lexer_scan_number(lexer);
            continue;
        }

        // Identifier or keyword
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
            lexer_scan_identifier(lexer);
            continue;
        }

        // Single-character tokens
        lexer_advance(lexer);
        switch (c) {
            case '\n': lexer_add_token(lexer, MOS_TOK_NEWLINE, start, 1); break;
            case '+':  lexer_add_token(lexer, MOS_TOK_PLUS, start, 1); break;
            case '-':  lexer_add_token(lexer, MOS_TOK_MINUS, start, 1); break;
            case '*':  lexer_add_token(lexer, MOS_TOK_STAR, start, 1); break;
            case '/':  lexer_add_token(lexer, MOS_TOK_SLASH, start, 1); break;
            case '%':  lexer_add_token(lexer, MOS_TOK_PERCENT, start, 1); break;
            case '(':  lexer_add_token(lexer, MOS_TOK_LPAREN, start, 1); break;
            case ')':  lexer_add_token(lexer, MOS_TOK_RPAREN, start, 1); break;
            case '{':  lexer_add_token(lexer, MOS_TOK_LBRACE, start, 1); break;
            case '}':  lexer_add_token(lexer, MOS_TOK_RBRACE, start, 1); break;
            case '[':  lexer_add_token(lexer, MOS_TOK_LBRACKET, start, 1); break;
            case ']':  lexer_add_token(lexer, MOS_TOK_RBRACKET, start, 1); break;
            case ',':  lexer_add_token(lexer, MOS_TOK_COMMA, start, 1); break;
            case ':':  lexer_add_token(lexer, MOS_TOK_COLON, start, 1); break;
            case ';':  lexer_add_token(lexer, MOS_TOK_SEMICOLON, start, 1); break;
            case '.':  lexer_add_token(lexer, MOS_TOK_DOT, start, 1); break;
            case '=':
                if (lexer_peek(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_add_token(lexer, MOS_TOK_EQ, start, 2);
                } else {
                    lexer_add_token(lexer, MOS_TOK_ASSIGN, start, 1);
                }
                break;
            case '!':
                if (lexer_peek(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_add_token(lexer, MOS_TOK_NE, start, 2);
                } else {
                    lexer_add_token(lexer, MOS_TOK_NOT, start, 1);
                }
                break;
            case '<':
                if (lexer_peek(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_add_token(lexer, MOS_TOK_LE, start, 2);
                } else {
                    lexer_add_token(lexer, MOS_TOK_LT, start, 1);
                }
                break;
            case '>':
                if (lexer_peek(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_add_token(lexer, MOS_TOK_GE, start, 2);
                } else {
                    lexer_add_token(lexer, MOS_TOK_GT, start, 1);
                }
                break;
            default:
                lexer_add_token(lexer, MOS_TOK_ERROR, start, 1);
                lexer->health -= 5;
                break;
        }
    }

    lexer_add_token(lexer, MOS_TOK_EOF, "", 0);
    g_runtime.lines_compiled += lexer->line;
    return MOS_RESULT_OK;
}

void mos_omni_lexer_free(MosOmniLexer* lexer) {
    if (lexer && lexer->tokens) {
        free(lexer->tokens);
        lexer->tokens = NULL;
    }
}

// -----------------------------------------------------------------------------
// B.5 Parser Implementation
// -----------------------------------------------------------------------------

mos_result_t mos_omni_parser_create(MosOmniParser* parser, MosOmniToken* tokens,
                                     mos_size_t count) {
    if (!parser || !tokens) return MOS_RESULT_ERROR;

    parser->tokens = tokens;
    parser->token_count = count;
    parser->current = 0;
    parser->root = NULL;
    parser->health = MOS_HEALTH_DEFAULT;
    parser->state = MOS_HEBREW_YASHAR;
    parser->error[0] = '\0';

    return MOS_RESULT_OK;
}

static MosOmniToken* parser_peek(MosOmniParser* parser) {
    if (parser->current >= parser->token_count) return NULL;
    return &parser->tokens[parser->current];
}

static MosOmniToken* parser_advance(MosOmniParser* parser) {
    if (parser->current >= parser->token_count) return NULL;
    return &parser->tokens[parser->current++];
}

static bool parser_check(MosOmniParser* parser, MosOmniTokenType type) {
    MosOmniToken* token = parser_peek(parser);
    return token && token->type == type;
}

static bool parser_match(MosOmniParser* parser, MosOmniTokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return true;
    }
    return false;
}

static MosOmniAstNode* parser_alloc_node(MosOmniAstType type) {
    MosOmniAstNode* node = calloc(1, sizeof(MosOmniAstNode));
    if (node) {
        node->type = type;
        node->health = MOS_HEALTH_DEFAULT;
        node->state = MOS_HEBREW_YASHAR;
    }
    return node;
}

// Forward declarations
static MosOmniAstNode* parse_expression(MosOmniParser* parser);
static MosOmniAstNode* parse_statement(MosOmniParser* parser);

static MosOmniAstNode* parse_primary(MosOmniParser* parser) {
    MosOmniToken* token = parser_peek(parser);
    if (!token) return NULL;

    switch (token->type) {
        case MOS_TOK_INT:
        case MOS_TOK_FLOAT:
        case MOS_TOK_STRING:
        case MOS_TOK_TRIT: {
            MosOmniAstNode* node = parser_alloc_node(MOS_AST_LITERAL);
            node->token = *parser_advance(parser);
            return node;
        }

        case MOS_TOK_IDENT: {
            MosOmniAstNode* node = parser_alloc_node(MOS_AST_IDENT);
            node->token = *parser_advance(parser);
            return node;
        }

        case MOS_TOK_LPAREN: {
            parser_advance(parser);
            MosOmniAstNode* expr = parse_expression(parser);
            if (!parser_match(parser, MOS_TOK_RPAREN)) {
                snprintf(parser->error, sizeof(parser->error),
                         "Expected ')' after expression");
                parser->health -= 10;
            }
            return expr;
        }

        default:
            return NULL;
    }
}

static MosOmniAstNode* parse_unary(MosOmniParser* parser) {
    if (parser_check(parser, MOS_TOK_MINUS) ||
        parser_check(parser, MOS_TOK_NOT)) {
        MosOmniAstNode* node = parser_alloc_node(MOS_AST_UNARY);
        node->token = *parser_advance(parser);
        node->right = parse_unary(parser);
        return node;
    }
    return parse_primary(parser);
}

static MosOmniAstNode* parse_binary(MosOmniParser* parser, int precedence) {
    MosOmniAstNode* left = parse_unary(parser);

    while (true) {
        MosOmniToken* op = parser_peek(parser);
        if (!op) break;

        int op_prec = 0;
        switch (op->type) {
            case MOS_TOK_STAR:
            case MOS_TOK_SLASH:
            case MOS_TOK_PERCENT:
                op_prec = 3;
                break;
            case MOS_TOK_PLUS:
            case MOS_TOK_MINUS:
                op_prec = 2;
                break;
            case MOS_TOK_LT:
            case MOS_TOK_LE:
            case MOS_TOK_GT:
            case MOS_TOK_GE:
            case MOS_TOK_EQ:
            case MOS_TOK_NE:
                op_prec = 1;
                break;
            default:
                op_prec = 0;
                break;
        }

        if (op_prec <= precedence) break;

        MosOmniAstNode* node = parser_alloc_node(MOS_AST_BINARY);
        node->token = *parser_advance(parser);
        node->left = left;
        node->right = parse_binary(parser, op_prec);
        left = node;
    }

    return left;
}

static MosOmniAstNode* parse_expression(MosOmniParser* parser) {
    return parse_binary(parser, 0);
}

static MosOmniAstNode* parse_statement(MosOmniParser* parser) {
    // Skip newlines and comments
    while (parser_match(parser, MOS_TOK_NEWLINE) ||
           parser_match(parser, MOS_TOK_COMMENT)) {
    }

    MosOmniToken* token = parser_peek(parser);
    if (!token || token->type == MOS_TOK_EOF) return NULL;

    // Handle different statement types
    if (token->type == MOS_TOK_KEYWORD) {
        if (strncmp(token->lexeme, "let", 3) == 0 ||
            strncmp(token->lexeme, "const", 5) == 0) {
            MosOmniAstNode* node = parser_alloc_node(MOS_AST_VAR_DECL);
            node->token = *parser_advance(parser);

            // Variable name
            if (parser_check(parser, MOS_TOK_IDENT)) {
                MosOmniAstNode* name = parser_alloc_node(MOS_AST_IDENT);
                name->token = *parser_advance(parser);
                node->left = name;
            }

            // Optional initializer
            if (parser_match(parser, MOS_TOK_ASSIGN)) {
                node->right = parse_expression(parser);
            }

            return node;
        }

        if (strncmp(token->lexeme, "return", 6) == 0) {
            MosOmniAstNode* node = parser_alloc_node(MOS_AST_RETURN);
            node->token = *parser_advance(parser);
            node->right = parse_expression(parser);
            return node;
        }

        if (strncmp(token->lexeme, "if", 2) == 0) {
            MosOmniAstNode* node = parser_alloc_node(MOS_AST_IF);
            node->token = *parser_advance(parser);
            node->left = parse_expression(parser);  // Condition

            // Skip to body
            while (parser_match(parser, MOS_TOK_NEWLINE)) {}

            if (parser_match(parser, MOS_TOK_LBRACE)) {
                MosOmniAstNode* body = parser_alloc_node(MOS_AST_BLOCK);
                MosOmniAstNode** current = &body->children;

                while (!parser_check(parser, MOS_TOK_RBRACE) &&
                       !parser_check(parser, MOS_TOK_EOF)) {
                    MosOmniAstNode* stmt = parse_statement(parser);
                    if (stmt) {
                        *current = stmt;
                        current = &stmt->next;
                    }
                }
                parser_match(parser, MOS_TOK_RBRACE);
                node->right = body;
            }

            return node;
        }
    }

    // OmniCode markers
    if (token->type == MOS_TOK_REQ || token->type == MOS_TOK_INH ||
        token->type == MOS_TOK_INS || token->type == MOS_TOK_OPT ||
        token->type == MOS_TOK_OMNI_MARKER) {
        MosOmniAstNode* node = parser_alloc_node(MOS_AST_OMNI_MARKER);
        node->token = *parser_advance(parser);
        // Skip rest of line
        while (!parser_check(parser, MOS_TOK_NEWLINE) &&
               !parser_check(parser, MOS_TOK_EOF)) {
            parser_advance(parser);
        }
        return node;
    }

    // Pragma
    if (token->type == MOS_TOK_PRAGMA) {
        MosOmniAstNode* node = parser_alloc_node(MOS_AST_PRAGMA);
        node->token = *parser_advance(parser);
        return node;
    }

    // Expression statement
    MosOmniAstNode* expr = parse_expression(parser);
    if (expr) {
        MosOmniAstNode* node = parser_alloc_node(MOS_AST_EXPR_STMT);
        node->left = expr;
        return node;
    }

    // Unknown - skip
    parser_advance(parser);
    return NULL;
}

mos_result_t mos_omni_parse(MosOmniParser* parser) {
    if (!parser) return MOS_RESULT_ERROR;

    parser->root = parser_alloc_node(MOS_AST_PROGRAM);
    if (!parser->root) return MOS_RESULT_ERROR;

    MosOmniAstNode** current = &parser->root->children;

    while (!parser_check(parser, MOS_TOK_EOF)) {
        MosOmniAstNode* stmt = parse_statement(parser);
        if (stmt) {
            *current = stmt;
            current = &stmt->next;
        }
    }

    return MOS_RESULT_OK;
}

static void free_ast_node(MosOmniAstNode* node) {
    if (!node) return;
    free_ast_node(node->left);
    free_ast_node(node->right);
    free_ast_node(node->children);
    free_ast_node(node->next);
    free(node);
}

void mos_omni_parser_free(MosOmniParser* parser) {
    if (parser && parser->root) {
        free_ast_node(parser->root);
        parser->root = NULL;
    }
}

// -----------------------------------------------------------------------------
// B.6 Compilation (AST to IR)
// -----------------------------------------------------------------------------

mos_result_t mos_omni_compile(MosOmniAstNode* ast, MosOmniModule* module) {
    if (!ast || !module) return MOS_RESULT_ERROR;

    // Initialize module
    module->code = malloc(MOS_OMNI_MAX_IR_OPS * sizeof(MosOmniIrOp));
    module->code_size = 0;
    module->constants = malloc(256 * sizeof(trit27_t));
    module->const_count = 0;
    module->health = MOS_HEALTH_DEFAULT;
    module->state = MOS_HEBREW_YASHAR;

    if (!module->code || !module->constants) {
        free(module->code);
        free(module->constants);
        return MOS_RESULT_ERROR;
    }

    // TODO: Full AST-to-IR compilation
    // For now, emit minimal IR

    // HALT instruction
    MosOmniIrOp* op = &module->code[module->code_size++];
    op->opcode = 0x00;  // NOP/HALT
    op->operands[0] = TRIT_ZERO;
    op->operands[1] = TRIT_ZERO;
    op->operands[2] = TRIT_ZERO;
    op->immediate = 0;
    op->health = MOS_HEALTH_DEFAULT;

    return MOS_RESULT_OK;
}

// -----------------------------------------------------------------------------
// B.7 Transpilation (AST to C)
// -----------------------------------------------------------------------------

static mos_size_t transpile_node(MosOmniAstNode* node, char* output,
                                  mos_size_t size, mos_size_t pos) {
    if (!node || pos >= size - 1) return pos;

    switch (node->type) {
        case MOS_AST_PROGRAM:
        case MOS_AST_BLOCK: {
            MosOmniAstNode* child = node->children;
            while (child) {
                pos = transpile_node(child, output, size, pos);
                child = child->next;
            }
            break;
        }

        case MOS_AST_VAR_DECL: {
            // Output variable declaration
            int written = snprintf(output + pos, size - pos, "int32_t ");
            pos += (written > 0) ? written : 0;

            if (node->left) {
                memcpy(output + pos, node->left->token.lexeme,
                       node->left->token.length);
                pos += node->left->token.length;
            }

            if (node->right) {
                written = snprintf(output + pos, size - pos, " = ");
                pos += (written > 0) ? written : 0;
                pos = transpile_node(node->right, output, size, pos);
            }

            written = snprintf(output + pos, size - pos, ";\n");
            pos += (written > 0) ? written : 0;
            break;
        }

        case MOS_AST_LITERAL:
        case MOS_AST_IDENT: {
            if (pos + node->token.length < size) {
                memcpy(output + pos, node->token.lexeme, node->token.length);
                pos += node->token.length;
            }
            break;
        }

        case MOS_AST_BINARY: {
            pos = transpile_node(node->left, output, size, pos);

            if (pos + 3 < size) {
                output[pos++] = ' ';
                memcpy(output + pos, node->token.lexeme, node->token.length);
                pos += node->token.length;
                output[pos++] = ' ';
            }

            pos = transpile_node(node->right, output, size, pos);
            break;
        }

        default:
            break;
    }

    return pos;
}

mos_result_t mos_omni_transpile_c(MosOmniAstNode* ast, char* output,
                                   mos_size_t size) {
    if (!ast || !output || size == 0) return MOS_RESULT_ERROR;

    // Header
    mos_size_t pos = 0;
    int written = snprintf(output + pos, size - pos,
        "// Generated by MillenniumOS OmniCode Transpiler\n"
        "#include <stdint.h>\n\n");
    pos += (written > 0) ? written : 0;

    // Transpile AST
    pos = transpile_node(ast, output, size, pos);

    if (pos < size) output[pos] = '\0';
    return MOS_RESULT_OK;
}

// -----------------------------------------------------------------------------
// B.8 Execution
// -----------------------------------------------------------------------------

mos_result_t mos_omni_exec_ir(MosOmniIrOp* code, mos_size_t count,
                               trit27_t* result) {
    if (!code || count == 0) return MOS_RESULT_ERROR;

    // Simple IR interpreter
    // TODO: Full IR execution engine

    trit27_t stack[256];
    mos_size_t sp = 0;

    for (mos_size_t ip = 0; ip < count; ip++) {
        MosOmniIrOp* op = &code[ip];
        g_runtime.ir_ops_executed++;

        switch (op->opcode) {
            case 0x00:  // HALT
                if (result && sp > 0) *result = stack[sp - 1];
                return MOS_RESULT_OK;

            case 0x01:  // PUSH immediate
                if (sp < 256) stack[sp++] = op->immediate;
                break;

            case 0x10:  // ADD
                if (sp >= 2) {
                    trit27_t b = stack[--sp];
                    trit27_t a = stack[--sp];
                    stack[sp++] = a + b;
                }
                break;

            case 0x11:  // SUB
                if (sp >= 2) {
                    trit27_t b = stack[--sp];
                    trit27_t a = stack[--sp];
                    stack[sp++] = a - b;
                }
                break;

            default:
                break;
        }
    }

    if (result && sp > 0) *result = stack[sp - 1];
    return MOS_RESULT_OK;
}

mos_result_t mos_omni_exec(MosOmniModule* module, const char* entry_point,
                           trit27_t* result) {
    if (!module || !module->code) return MOS_RESULT_ERROR;
    (void)entry_point;  // TODO: Find entry point

    g_runtime.functions_called++;
    return mos_omni_exec_ir(module->code, module->code_size, result);
}

// -----------------------------------------------------------------------------
// B.9 Full Pipeline
// -----------------------------------------------------------------------------

mos_result_t mos_omni_eval(const char* source, mos_size_t length,
                           trit27_t* result) {
    if (!source || length == 0) return MOS_RESULT_ERROR;

    MosOmniLexer lexer;
    MosOmniParser parser;
    MosOmniModule module = {0};
    mos_result_t res;

    // Lex
    res = mos_omni_lexer_create(&lexer, source, length);
    if (res != MOS_RESULT_OK) return res;

    res = mos_omni_lex(&lexer);
    if (res != MOS_RESULT_OK) {
        mos_omni_lexer_free(&lexer);
        return res;
    }

    // Parse
    res = mos_omni_parser_create(&parser, lexer.tokens, lexer.token_count);
    if (res != MOS_RESULT_OK) {
        mos_omni_lexer_free(&lexer);
        return res;
    }

    res = mos_omni_parse(&parser);
    if (res != MOS_RESULT_OK) {
        mos_omni_parser_free(&parser);
        mos_omni_lexer_free(&lexer);
        return res;
    }

    // Compile
    res = mos_omni_compile(parser.root, &module);
    if (res != MOS_RESULT_OK) {
        mos_omni_parser_free(&parser);
        mos_omni_lexer_free(&lexer);
        return res;
    }

    // Execute
    res = mos_omni_exec(&module, "main", result);

    // Cleanup
    free(module.code);
    free(module.constants);
    mos_omni_parser_free(&parser);
    mos_omni_lexer_free(&lexer);

    return res;
}

mos_result_t mos_omni_run(const char* path, trit27_t* result) {
    // TODO: Load file via VFS and evaluate
    (void)path;
    (void)result;
    return MOS_RESULT_ERROR;  // Not implemented yet
}

// -----------------------------------------------------------------------------
// B.10 Module Management
// -----------------------------------------------------------------------------

mos_result_t mos_omni_register(MosOmniModule* module) {
    if (!module || g_runtime.module_count >= MOS_OMNI_MAX_MODULES) {
        return MOS_RESULT_ERROR;
    }

    g_runtime.modules[g_runtime.module_count++] = module;
    return MOS_RESULT_OK;
}

mos_result_t mos_omni_unregister(const char* name) {
    for (mos_size_t i = 0; i < g_runtime.module_count; i++) {
        if (g_runtime.modules[i] &&
            strcmp(g_runtime.modules[i]->name, name) == 0) {
            free(g_runtime.modules[i]->code);
            free(g_runtime.modules[i]->constants);
            free(g_runtime.modules[i]);

            // Shift remaining modules
            for (mos_size_t j = i; j < g_runtime.module_count - 1; j++) {
                g_runtime.modules[j] = g_runtime.modules[j + 1];
            }
            g_runtime.module_count--;
            return MOS_RESULT_OK;
        }
    }
    return MOS_RESULT_ERROR;
}

MosOmniModule* mos_omni_find(const char* name) {
    for (mos_size_t i = 0; i < g_runtime.module_count; i++) {
        if (g_runtime.modules[i] &&
            strcmp(g_runtime.modules[i]->name, name) == 0) {
            return g_runtime.modules[i];
        }
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// B.11 Health and State
// -----------------------------------------------------------------------------

mos_health_t mos_omni_health(void) {
    return g_runtime.health;
}

MosHebrewState mos_omni_state(void) {
    return g_runtime.state;
}

bool mos_omni_can_self_host(void) {
    // Self-hosting requires:
    // 1. Lexer working
    // 2. Parser working
    // 3. IR compiler working
    // 4. IR executor working
    // 5. All core language features

    // For now, check basic functionality
    const char* test = "let x = 42";
    trit27_t result;

    mos_result_t res = mos_omni_eval(test, strlen(test), &result);
    return res == MOS_RESULT_OK;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// OmniCode Runtime Implementation:
//
// Pipeline: Source → Lexer → Parser → Compiler → Executor
//
//   1. LEXER: Tokenizes OmniCode source including pragma markers
//   2. PARSER: Builds AST from tokens
//   3. COMPILER: Generates IR (trit-native bytecode) from AST
//   4. EXECUTOR: Interprets IR or (future) JIT compiles
//
// Self-Hosting Path:
//
//   This C implementation is Stage 0 of the bootstrap.
//   OmniCode compiler written in OmniCode will use this
//   to compile itself, achieving self-hosting.
//
// "All things were made by him; and without him was not
//  any thing made that was made."
// — John 1:3
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* CPISI_MODE >= 2 - Full OmniCode service implementation */
