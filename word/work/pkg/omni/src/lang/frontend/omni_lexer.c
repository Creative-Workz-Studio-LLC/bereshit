// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-lexer-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_lexer.c — OmniCode Lexer Implementation
// Tokenizes OmniCode source into a stream of tokens

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-lexer-impl
// Title: OmniCode Lexer Implementation
// Type: Source
// Component: OmniCode Parser
// Role: Implements OmniCode tokenization

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// # S.1 Includes [INCLUDES]

#include <string.h>
#include <strings.h>  // strcasecmp for block marker detection
#include <ctype.h>
#include <stdio.h>

#include "omni_lexer.h"

// # S.2 Keyword Data [KEYWORDS]
//
// Keyword data is SEPARATE from lexer logic (data-driven design).
// OmniCode is grounded in the Bible - Hebrew characters are valid.

#include "omni_keywords.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.0 Forward Declarations [FORWARD_DECL]
//
// Standard C practice: declare functions before use, define after.
// Allows logical organization without dependency issues.

static char peek_char(OmniLexer* lexer);
static char peek_char_at(OmniLexer* lexer, size_t offset);
static char advance(OmniLexer* lexer);

// # B.1 Helper Functions [HELPERS]

// # B.1a UTF-8 Support [UTF8]
//
// OmniCode is grounded in the Bible. Hebrew characters are VALID in identifiers.
// This is not a bug - it's a feature. "בראשית" (bereshit) IS an identifier.

// # B.1a.1 Valid UTF-8 Lead Bytes for Identifiers [UTF8_RANGES]
//
// Data-driven: table of valid UTF-8 lead bytes for identifier characters.
// Add new scripts by extending this table, not modifying logic.
//
// | Lead Byte | Unicode Range | Script |
// |-----------|---------------|--------|
// | 0xD6-0xD7 | U+0590-U+05FF | Hebrew |
// | 0xCD-0xCF | U+0370-U+03FF | Greek  |

typedef struct {
    unsigned char start;    // First valid lead byte
    unsigned char end;      // Last valid lead byte
    const char*   name;     // Script name (for debugging)
} Utf8IdentifierRange;

static const Utf8IdentifierRange utf8_identifier_ranges[] = {
    {0xD6, 0xD7, "Hebrew"},     // Hebrew: U+0590-U+05FF
    {0xCD, 0xCF, "Greek"},      // Greek:  U+0370-U+03FF
    {0, 0, NULL}                // Sentinel
};

// Check if byte is UTF-8 continuation byte (10xxxxxx)
static bool is_utf8_continuation(unsigned char c) {
    return (c & 0xC0) == 0x80;
}

// Get number of bytes in UTF-8 sequence from lead byte
// Returns 0 if invalid, 1-4 for valid sequences
static int utf8_sequence_length(unsigned char c) {
    if ((c & 0x80) == 0x00) return 1;      // ASCII (0xxxxxxx)
    if ((c & 0xE0) == 0xC0) return 2;      // 2-byte (110xxxxx)
    if ((c & 0xF0) == 0xE0) return 3;      // 3-byte (1110xxxx)
    if ((c & 0xF8) == 0xF0) return 4;      // 4-byte (11110xxx)
    return 0;                               // Invalid lead byte
}

// Check if a UTF-8 character (from lead byte) is valid for identifiers
// Uses data-driven table - add new scripts by extending utf8_identifier_ranges
static bool is_utf8_identifier_char(OmniLexer* lexer) {
    unsigned char c = (unsigned char)peek_char(lexer);
    int seq_len = utf8_sequence_length(c);

    if (seq_len < 2) return false;  // ASCII handled separately

    // Check we have enough bytes for complete sequence
    for (int i = 1; i < seq_len; i++) {
        unsigned char next = (unsigned char)peek_char_at(lexer, i);
        if (!is_utf8_continuation(next)) return false;
    }

    // Check against valid identifier ranges (data-driven)
    for (int i = 0; utf8_identifier_ranges[i].name != NULL; i++) {
        if (c >= utf8_identifier_ranges[i].start &&
            c <= utf8_identifier_ranges[i].end) {
            return true;
        }
    }

    return false;
}

// Advance past a complete UTF-8 character, copying bytes to buffer
static int advance_utf8(OmniLexer* lexer, char* buf, size_t max_len, size_t* pos) {
    unsigned char c = (unsigned char)peek_char(lexer);
    int seq_len = utf8_sequence_length(c);

    if (seq_len == 0) return 0;  // Invalid

    for (int i = 0; i < seq_len; i++) {
        if (*pos < max_len - 1) {
            buf[(*pos)++] = advance(lexer);
        } else {
            advance(lexer);  // Skip if overflow
        }
    }
    return seq_len;
}

static char peek_char(OmniLexer* lexer) {
    if (lexer->pos >= lexer->length) return '\0';
    return lexer->source[lexer->pos];
}

static char peek_char_at(OmniLexer* lexer, size_t offset) {
    if (lexer->pos + offset >= lexer->length) return '\0';
    return lexer->source[lexer->pos + offset];
}

static char advance(OmniLexer* lexer) {
    if (lexer->pos >= lexer->length) return '\0';
    char c = lexer->source[lexer->pos++];
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
        lexer->at_line_start = true;
    } else {
        lexer->column++;
        lexer->at_line_start = false;
    }
    return c;
}

static void skip_whitespace(OmniLexer* lexer) {
    while (peek_char(lexer) == ' ' || peek_char(lexer) == '\t') {
        advance(lexer);
    }
}

static int count_indent(OmniLexer* lexer) {
    int spaces = 0;
    size_t pos = lexer->pos;
    while (pos < lexer->length && lexer->source[pos] == ' ') {
        spaces++;
        pos++;
    }
    // Tab = 4 spaces
    while (pos < lexer->length && lexer->source[pos] == '\t') {
        spaces += 4;
        pos++;
    }
    return spaces / 4;  // Indent level
}

static OmniTokenType lookup_keyword(const char* text) {
    for (int i = 0; omni_keywords[i].text != NULL; i++) {
        if (strcmp(text, omni_keywords[i].text) == 0) {
            return omni_keywords[i].type;
        }
    }
    return TOK_IDENTIFIER;
}

static OmniToken make_token(OmniLexer* lexer, OmniTokenType type, const char* text) {
    OmniToken tok;
    tok.type = type;
    tok.line = lexer->line;
    tok.column = lexer->column;
    tok.indent = lexer->indent;
    strncpy(tok.text, text, OMNI_TOKEN_MAX_LEN - 1);
    tok.text[OMNI_TOKEN_MAX_LEN - 1] = '\0';
    return tok;
}

static OmniToken make_error(OmniLexer* lexer, const char* msg) {
    OmniToken tok;
    tok.type = TOK_ERROR;
    tok.line = lexer->line;
    tok.column = lexer->column;
    tok.indent = lexer->indent;
    strncpy(tok.text, msg, OMNI_TOKEN_MAX_LEN - 1);
    tok.text[OMNI_TOKEN_MAX_LEN - 1] = '\0';
    strncpy(lexer->error, msg, OMNI_ERROR_MAX_LEN - 1);
    lexer->error[OMNI_ERROR_MAX_LEN - 1] = '\0';

    // DAR: DETECT phase - track error
    lexer->error_count++;
    lexer->had_error = true;

    return tok;
}

// # B.2 Token Scanners [SCANNERS]

// Scan identifier or keyword
// Supports ASCII and UTF-8 characters (Hebrew, Greek) - OmniCode is Bible-grounded
static OmniToken scan_identifier(OmniLexer* lexer) {
    int start_col = lexer->column;
    char buf[OMNI_TOKEN_MAX_LEN];
    size_t len = 0;

    while (1) {
        char c = peek_char(lexer);

        // ASCII alphanumeric or underscore
        if (isalnum((unsigned char)c) || c == '_') {
            if (len < OMNI_TOKEN_MAX_LEN - 1) {
                buf[len++] = advance(lexer);
            } else {
                advance(lexer);  // Skip overflow
            }
        }
        // UTF-8 multi-byte character (Hebrew, Greek, etc.)
        else if (is_utf8_identifier_char(lexer)) {
            advance_utf8(lexer, buf, OMNI_TOKEN_MAX_LEN, &len);
        }
        // End of identifier
        else {
            break;
        }
    }
    buf[len] = '\0';

    OmniTokenType type = lookup_keyword(buf);
    OmniToken tok = make_token(lexer, type, buf);
    tok.column = start_col;
    return tok;
}

// Scan string literal
// DAR: DETECT - validates escape sequences (L003)
static OmniToken scan_string(OmniLexer* lexer) {
    int start_col = lexer->column;
    int start_line = lexer->line;
    char buf[OMNI_TOKEN_MAX_LEN];
    size_t len = 0;

    advance(lexer);  // Skip opening quote

    while (peek_char(lexer) != '"' && peek_char(lexer) != '\0') {
        char c = peek_char(lexer);
        if (c == '\n') {
            // L002: Unterminated string at newline
            char err[128];
            snprintf(err, sizeof(err),
                     "L002: Unterminated string starting at line %d", start_line);
            return make_error(lexer, err);
        }
        if (c == '\\' && peek_char_at(lexer, 1) != '\0') {
            advance(lexer);  // Skip backslash
            int esc_line = lexer->line;
            int esc_col = lexer->column;
            c = peek_char(lexer);

            // Validate and handle escape sequences
            switch (c) {
                case 'n': c = '\n'; advance(lexer); break;
                case 't': c = '\t'; advance(lexer); break;
                case 'r': c = '\r'; advance(lexer); break;
                case '"': c = '"'; advance(lexer); break;
                case '\\': c = '\\'; advance(lexer); break;
                default: {
                    // L003: Invalid escape sequence
                    // Include precise location for diagnostic
                    char err[128];
                    snprintf(err, sizeof(err),
                             "L003: Invalid escape sequence \\%c at %d:%d",
                             c, esc_line, esc_col);
                    // Record error but continue parsing (DAR: detect, don't stop)
                    make_error(lexer, err);
                    c = advance(lexer);  // Keep the literal character
                    break;
                }
            }
        } else {
            c = advance(lexer);
        }
        if (len < OMNI_TOKEN_MAX_LEN - 1) {
            buf[len++] = c;
        }
    }

    if (peek_char(lexer) != '"') {
        // L002: Unterminated string at EOF
        char err[128];
        snprintf(err, sizeof(err),
                 "L002: Unterminated string starting at line %d (reached EOF)",
                 start_line);
        return make_error(lexer, err);
    }
    advance(lexer);  // Skip closing quote

    buf[len] = '\0';
    OmniToken tok = make_token(lexer, TOK_STRING, buf);
    tok.column = start_col;
    return tok;
}

// Scan number literal
static OmniToken scan_number(OmniLexer* lexer) {
    int start_col = lexer->column;
    char buf[OMNI_TOKEN_MAX_LEN];
    size_t len = 0;

    // Handle negative numbers
    if (peek_char(lexer) == '-' || peek_char(lexer) == '+') {
        buf[len++] = advance(lexer);
    }

    // Integer part
    while (isdigit(peek_char(lexer))) {
        if (len < OMNI_TOKEN_MAX_LEN - 1) {
            buf[len++] = advance(lexer);
        } else {
            advance(lexer);
        }
    }

    // Decimal part
    if (peek_char(lexer) == '.' && isdigit(peek_char_at(lexer, 1))) {
        buf[len++] = advance(lexer);  // .
        while (isdigit(peek_char(lexer))) {
            if (len < OMNI_TOKEN_MAX_LEN - 1) {
                buf[len++] = advance(lexer);
            } else {
                advance(lexer);
            }
        }
    }

    buf[len] = '\0';
    OmniToken tok = make_token(lexer, TOK_NUMBER, buf);
    tok.column = start_col;
    return tok;
}

// Scan comment
static OmniToken scan_comment(OmniLexer* lexer) {
    int start_col = lexer->column;
    char buf[OMNI_TOKEN_MAX_LEN];
    size_t len = 0;

    advance(lexer);  // First /
    advance(lexer);  // Second /

    // Rest of line
    while (peek_char(lexer) != '\n' && peek_char(lexer) != '\0') {
        if (len < OMNI_TOKEN_MAX_LEN - 1) {
            buf[len++] = advance(lexer);
        } else {
            advance(lexer);
        }
    }

    buf[len] = '\0';
    OmniToken tok = make_token(lexer, TOK_COMMENT, buf);
    tok.column = start_col;
    return tok;
}

// Scan block marker: [BLOCKNAME] or [END BLOCKNAME]
// OmniCode native syntax for block structure - the structure IS the code
static OmniToken scan_block_marker(OmniLexer* lexer) {
    int start_col = lexer->column;
    size_t start_pos = lexer->pos;

    advance(lexer);  // Skip opening [

    // Skip whitespace inside bracket
    while (peek_char(lexer) == ' ' || peek_char(lexer) == '\t') {
        advance(lexer);
    }

    // Check for END prefix
    bool is_end = false;
    if (peek_char(lexer) == 'E' && peek_char_at(lexer, 1) == 'N' &&
        peek_char_at(lexer, 2) == 'D' &&
        (peek_char_at(lexer, 3) == ' ' || peek_char_at(lexer, 3) == '\t')) {
        is_end = true;
        advance(lexer); advance(lexer); advance(lexer);  // Skip "END"
        // Skip whitespace after END
        while (peek_char(lexer) == ' ' || peek_char(lexer) == '\t') {
            advance(lexer);
        }
    }

    // Read block name
    char name[32];
    size_t name_len = 0;
    while (isalpha(peek_char(lexer)) || peek_char(lexer) == '_') {
        if (name_len < sizeof(name) - 1) {
            name[name_len++] = advance(lexer);
        } else {
            advance(lexer);
        }
    }
    name[name_len] = '\0';

    // Skip whitespace before closing bracket
    while (peek_char(lexer) == ' ' || peek_char(lexer) == '\t') {
        advance(lexer);
    }

    // Must end with ]
    if (peek_char(lexer) != ']') {
        // Not a block marker - restore position and return as regular bracket
        lexer->pos = start_pos;
        lexer->column = start_col;
        advance(lexer);  // Return just the [
        return make_token(lexer, TOK_LBRACKET, "[");
    }
    advance(lexer);  // Skip closing ]

    // Map block name to token type
    OmniTokenType type = TOK_LBRACKET;  // Default if not recognized
    const char* value = is_end ? "END" : "START";

    // Block name lookup (case-insensitive comparison)
    if (strcasecmp(name, "METADATA") == 0) {
        type = TOK_BLOCK_METADATA;
    } else if (strcasecmp(name, "SETUP") == 0) {
        type = TOK_BLOCK_SETUP;
    } else if (strcasecmp(name, "BODY") == 0) {
        type = TOK_BLOCK_BODY;
    } else if (strcasecmp(name, "CLOSING") == 0) {
        type = TOK_BLOCK_CLOSING;
    } else if (strcasecmp(name, "HEADER") == 0) {
        type = TOK_BLOCK_HEADER;
    } else if (strcasecmp(name, "CONTEXT") == 0) {
        type = TOK_BLOCK_CONTEXT;
    } else if (strcasecmp(name, "CONTENT") == 0) {
        type = TOK_BLOCK_CONTENT;
    } else if (strcasecmp(name, "FOOTER") == 0) {
        type = TOK_BLOCK_FOOTER;
    } else {
        // Unknown block name - restore and return as regular bracket
        lexer->pos = start_pos;
        lexer->column = start_col;
        advance(lexer);
        return make_token(lexer, TOK_LBRACKET, "[");
    }

    OmniToken tok = make_token(lexer, type, value);
    tok.column = start_col;
    return tok;
}

// Scan pragma (#!omni ...)
static OmniToken scan_pragma(OmniLexer* lexer) {
    int start_col = lexer->column;
    char buf[OMNI_TOKEN_MAX_LEN];
    size_t len = 0;

    // Read until end of line
    while (peek_char(lexer) != '\n' && peek_char(lexer) != '\0') {
        if (len < OMNI_TOKEN_MAX_LEN - 1) {
            buf[len++] = advance(lexer);
        } else {
            advance(lexer);
        }
    }

    buf[len] = '\0';

    // Determine pragma type
    OmniTokenType type = TOK_PRAGMA;
    if (strstr(buf, "meta.key") != NULL) {
        type = TOK_META_KEY;
    } else if (strstr(buf, "meta.from") != NULL) {
        type = TOK_META_FROM;
    } else if (strstr(buf, "meta.at") != NULL) {
        type = TOK_META_AT;
    }

    OmniToken tok = make_token(lexer, type, buf);
    tok.column = start_col;
    return tok;
}

// # B.3 Core Lexer Functions [CORE]

void omni_lexer_init(OmniLexer* lexer, const char* source, size_t length) {
    lexer->source = source;
    lexer->length = length;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->indent = 0;
    lexer->at_line_start = true;
    lexer->error[0] = '\0';

    // DAR: DETECT phase - error tracking
    lexer->error_count = 0;
    lexer->had_error = false;
}

OmniToken omni_lexer_next(OmniLexer* lexer) {
    // Handle indentation at line start
    if (lexer->at_line_start) {
        lexer->indent = count_indent(lexer);
    }

    // Skip whitespace (but track indentation)
    skip_whitespace(lexer);

    // Check EOF
    if (lexer->pos >= lexer->length) {
        return make_token(lexer, TOK_EOF, "");
    }

    char c = peek_char(lexer);

    // Newline
    if (c == '\n') {
        advance(lexer);
        return make_token(lexer, TOK_NEWLINE, "\\n");
    }

    // Pragma or comment
    if (c == '#') {
        if (peek_char_at(lexer, 1) == '!') {
            return scan_pragma(lexer);
        }
        // # comment (for TOML-style)
        return scan_comment(lexer);
    }

    // Comment
    if (c == '/' && peek_char_at(lexer, 1) == '/') {
        return scan_comment(lexer);
    }

    // String
    if (c == '"') {
        return scan_string(lexer);
    }

    // Number (including negative)
    if (isdigit(c) || (c == '-' && isdigit(peek_char_at(lexer, 1))) ||
        (c == '+' && isdigit(peek_char_at(lexer, 1)))) {
        return scan_number(lexer);
    }

    // Identifier or keyword (ASCII or UTF-8 - Hebrew, Greek, etc.)
    if (isalpha((unsigned char)c) || c == '_' || is_utf8_identifier_char(lexer)) {
        return scan_identifier(lexer);
    }

    // Block marker: [METADATA], [END METADATA], etc.
    // Must check BEFORE single-byte switch - scan_block_marker handles its own advance
    if (c == '[') {
        return scan_block_marker(lexer);
    }

    // UTF-8 operators (must check before single-byte switch)
    // Data-driven: table of UTF-8 byte sequences → token types
    {
        // UTF-8 operator table - add new operators by extending this array
        static const struct {
            unsigned char bytes[4];  // UTF-8 byte sequence (0-terminated)
            int           len;       // Byte count
            OmniTokenType type;      // Token type
            const char*   text;      // Display text
        } utf8_operators[] = {
            {{0xC2, 0xB1, 0, 0}, 2, TOK_PLUS_MINUS, "±"},   // ± U+00B1
            {{0xE2, 0x88, 0x9E, 0}, 3, TOK_INFINITY, "∞"},  // ∞ U+221E
            {{0, 0, 0, 0}, 0, TOK_EOF, NULL}                // Sentinel
        };

        for (int i = 0; utf8_operators[i].text != NULL; i++) {
            bool match = true;
            for (int j = 0; j < utf8_operators[i].len; j++) {
                if ((unsigned char)peek_char_at(lexer, j) != utf8_operators[i].bytes[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                for (int j = 0; j < utf8_operators[i].len; j++) {
                    advance(lexer);
                }
                return make_token(lexer, utf8_operators[i].type, utf8_operators[i].text);
            }
        }
    }

    // Single character tokens
    advance(lexer);
    switch (c) {
        case ':': return make_token(lexer, TOK_COLON, ":");
        case '=': return make_token(lexer, TOK_EQUAL, "=");
        case '(': return make_token(lexer, TOK_LPAREN, "(");
        case ')': return make_token(lexer, TOK_RPAREN, ")");
        // '[' handled by scan_block_marker above
        case ']': return make_token(lexer, TOK_RBRACKET, "]");
        case '{': return make_token(lexer, TOK_LBRACE, "{");
        case '}': return make_token(lexer, TOK_RBRACE, "}");
        case ',': return make_token(lexer, TOK_COMMA, ",");
        case '.': return make_token(lexer, TOK_DOT, ".");
        case '+': return make_token(lexer, TOK_PLUS, "+");
        case '-':
            if (peek_char(lexer) == '>') {
                advance(lexer);
                return make_token(lexer, TOK_ARROW, "->");
            }
            return make_token(lexer, TOK_MINUS, "-");
        case '*': return make_token(lexer, TOK_STAR, "*");
        case '/': return make_token(lexer, TOK_SLASH, "/");
        default: {
            // L001: Unexpected character
            char err[128];
            snprintf(err, sizeof(err),
                     "L001: Unexpected character '%c' (0x%02X) at %d:%d",
                     (isprint(c) ? c : '?'),
                     (unsigned char)c,
                     lexer->line, lexer->column - 1);
            return make_error(lexer, err);
        }
    }
}

OmniToken omni_lexer_peek(OmniLexer* lexer) {
    // Save state
    size_t saved_pos = lexer->pos;
    int saved_line = lexer->line;
    int saved_column = lexer->column;
    int saved_indent = lexer->indent;
    bool saved_at_line_start = lexer->at_line_start;

    // Get next token
    OmniToken tok = omni_lexer_next(lexer);

    // Restore state
    lexer->pos = saved_pos;
    lexer->line = saved_line;
    lexer->column = saved_column;
    lexer->indent = saved_indent;
    lexer->at_line_start = saved_at_line_start;

    return tok;
}

// # B.4 Utility Functions [UTILITIES]

const char* omni_token_type_name(OmniTokenType type) {
    static const char* names[] = {
        [TOK_EOF] = "EOF",
        [TOK_ERROR] = "ERROR",
        [TOK_NEWLINE] = "NEWLINE",
        [TOK_INDENT] = "INDENT",
        [TOK_PRAGMA] = "PRAGMA",
        [TOK_META_KEY] = "META_KEY",
        [TOK_META_FROM] = "META_FROM",
        [TOK_META_AT] = "META_AT",
        [TOK_LET] = "LET",
        [TOK_ESTABLISH] = "ESTABLISH",
        [TOK_CREATE] = "CREATE",
        [TOK_WORK] = "WORK",
        [TOK_TAKES] = "TAKES",
        [TOK_RETURNS] = "RETURNS",
        [TOK_THAT] = "THAT",
        [TOK_PROVIDE] = "PROVIDE",
        [TOK_WHEN] = "WHEN",
        [TOK_OTHERWISE] = "OTHERWISE",
        [TOK_REPEAT] = "REPEAT",
        [TOK_UNTIL] = "UNTIL",
        [TOK_THROUGH] = "THROUGH",
        [TOK_FROM] = "FROM",
        [TOK_TO] = "TO",
        [TOK_BY] = "BY",
        [TOK_OWN] = "OWN",
        [TOK_SHARE] = "SHARE",
        [TOK_REF] = "REF",
        [TOK_STEWARD] = "STEWARD",
        [TOK_ATTEMPT] = "ATTEMPT",
        [TOK_RECOVER] = "RECOVER",
        [TOK_ENSURE] = "ENSURE",
        [TOK_FAIL] = "FAIL",
        [TOK_MODULE] = "MODULE",
        [TOK_IMPORT] = "IMPORT",
        [TOK_EXPORT] = "EXPORT",
        [TOK_ASYNC] = "ASYNC",
        [TOK_AWAIT] = "AWAIT",
        [TOK_SPAWN] = "SPAWN",
        [TOK_AND] = "AND",
        [TOK_OR] = "OR",
        [TOK_NOT] = "NOT",
        [TOK_IS] = "IS",
        [TOK_AS] = "AS",
        [TOK_YES] = "YES",
        [TOK_NO] = "NO",
        [TOK_GRANTED] = "GRANTED",
        [TOK_DEFERRED] = "DEFERRED",
        [TOK_DENIED] = "DENIED",
        [TOK_NONE] = "NONE",
        [TOK_THIS] = "THIS",
        [TOK_FOR] = "FOR",
        [TOK_KEYED] = "KEYED",
        [TOK_FOLLOWING] = "FOLLOWING",
        [TOK_CLASSIFIES] = "CLASSIFIES",
        [TOK_DERIVES] = "DERIVES",
        [TOK_GROUNDED] = "GROUNDED",
        [TOK_IN] = "IN",
        [TOK_SERVES] = "SERVES",
        [TOK_AUTHORED] = "AUTHORED",
        [TOK_EXISTS] = "EXISTS",
        [TOK_INTERFACE] = "INTERFACE",
        [TOK_DEPENDS] = "DEPENDS",
        [TOK_INTEGRATED] = "INTEGRATED",
        [TOK_OPERATIONAL] = "OPERATIONAL",
        [TOK_BLOCKING] = "BLOCKING",
        [TOK_HEALTH] = "HEALTH",
        [TOK_SCRIPTURE] = "SCRIPTURE",
        [TOK_PRINCIPLE] = "PRINCIPLE",
        [TOK_ANCHOR] = "ANCHOR",
        [TOK_MEANING] = "MEANING",
        [TOK_TEACHING] = "TEACHING",
        [TOK_CAUTIONING] = "CAUTIONING",
        [TOK_BECAUSE] = "BECAUSE",
        [TOK_BE] = "BE",
        [TOK_OF] = "OF",
        [TOK_WITH] = "WITH",
        [TOK_AT] = "AT",
        [TOK_ON] = "ON",
        [TOK_ABOVE] = "ABOVE",
        [TOK_BELOW] = "BELOW",
        [TOK_WITHIN] = "WITHIN",
        [TOK_REQUIRES] = "REQUIRES",
        [TOK_INCLUDES] = "INCLUDES",
        [TOK_EXCLUDES] = "EXCLUDES",
        [TOK_USED] = "USED",
        [TOK_INTEGRATES] = "INTEGRATES",
        [TOK_NAVIGATION] = "NAVIGATION",
        [TOK_EXITS] = "EXITS",
        [TOK_SYSTEM] = "SYSTEM",
        [TOK_SCALE] = "SCALE",
        [TOK_OPERATIONS] = "OPERATIONS",
        [TOK_SCORES] = "SCORES",
        [TOK_CASCADE] = "CASCADE",
        [TOK_MULTIPLIES] = "MULTIPLIES",
        [TOK_FOLDER] = "FOLDER",
        [TOK_NAMED] = "NAMED",
        [TOK_CORE] = "CORE",
        [TOK_IDENTITY] = "IDENTITY",
        [TOK_ROLE] = "ROLE",
        [TOK_ARCHITECT] = "ARCHITECT",
        [TOK_IMPLEMENTATION] = "IMPLEMENTATION",
        [TOK_CREATED] = "CREATED",
        [TOK_VERSION] = "VERSION",
        [TOK_PURPOSE] = "PURPOSE",
        [TOK_DEFINITION] = "DEFINITION",
        [TOK_VALUES] = "VALUES",
        [TOK_PRIMARY] = "PRIMARY",
        [TOK_SYNTHESIS] = "SYNTHESIS",
        [TOK_QUICK] = "QUICK",
        [TOK_REFERENCE] = "REFERENCE",
        [TOK_NAVIGATE] = "NAVIGATE",
        [TOK_BRANCH] = "BRANCH",
        [TOK_PARADIGM] = "PARADIGM",
        [TOK_DESIGN] = "DESIGN",
        [TOK_PHILOSOPHY] = "PHILOSOPHY",
        [TOK_FEATURES] = "FEATURES",
        [TOK_MODIFIED] = "MODIFIED",
        [TOK_SCHEMA] = "SCHEMA",
        [TOK_FIELDS] = "FIELDS",
        [TOK_CONSTRAINTS] = "CONSTRAINTS",
        [TOK_ENSURING] = "ENSURING",
        [TOK_RESERVED] = "RESERVED",
        [TOK_DEFAULTING] = "DEFAULTING",
        [TOK_STRUCTURE] = "STRUCTURE",
        [TOK_CONTENTS] = "CONTENTS",
        [TOK_CHILDREN] = "CHILDREN",
        [TOK_TABLES] = "TABLES",
        [TOK_VERIFIED] = "VERIFIED",
        [TOK_STATIC] = "STATIC",
        [TOK_PRODUCING] = "PRODUCING",
        [TOK_RUNTIME] = "RUNTIME",
        [TOK_SUCCEEDING] = "SUCCEEDING",
        [TOK_RETURNING] = "RETURNING",
        [TOK_DETECTED] = "DETECTED",
        [TOK_ACCESSED] = "ACCESSED",
        [TOK_ACCESS] = "ACCESS",
        // TOK_RETURNS already listed with Function Keywords
        [TOK_POLICY] = "POLICY",
        [TOK_MODIFICATION] = "MODIFICATION",
        [TOK_SAFE] = "SAFE",
        [TOK_CAREFUL] = "CAREFUL",
        [TOK_FUTURE] = "FUTURE",
        [TOK_PLANNED] = "PLANNED",
        [TOK_CONSIDERED] = "CONSIDERED",
        [TOK_REJECTED] = "REJECTED",
        [TOK_OVERVIEW] = "OVERVIEW",
        [TOK_SEE] = "SEE",
        [TOK_CLOSING] = "CLOSING",
        [TOK_NOTE] = "NOTE",
        [TOK_IMPACT] = "IMPACT",
        [TOK_VIEW] = "VIEW",
        [TOK_QUERY] = "QUERY",
        [TOK_CHECK] = "CHECK",
        [TOK_A] = "A",
        [TOK_CONTEXT] = "CONTEXT",
        [TOK_MITIGATION] = "MITIGATION",
        [TOK_MULTIPLIERS] = "MULTIPLIERS",
        [TOK_TERNARY] = "TERNARY",
        [TOK_INTEGRITY] = "INTEGRITY",
        [TOK_NEVER] = "NEVER",
        [TOK_DEPENDENCIES] = "DEPENDENCIES",
        [TOK_ZONE_VOID] = "ZONE_VOID",
        [TOK_ZONE_WORD] = "ZONE_WORD",
        [TOK_ZONE_TOV] = "ZONE_TOV",
        [TOK_ROLE_MEMORY] = "ROLE_MEMORY",
        [TOK_COUNT] = "COUNT",
        [TOK_WHOLE] = "WHOLE",
        [TOK_MEASURE] = "MEASURE",
        [TOK_WORD] = "WORD",
        [TOK_CHAR] = "CHAR",
        [TOK_SCROLL] = "SCROLL",
        [TOK_TRUTH] = "TRUTH",
        [TOK_COVENANT] = "COVENANT",
        [TOK_GATHERING] = "GATHERING",
        [TOK_ASSEMBLY] = "ASSEMBLY",
        [TOK_OPTIONAL] = "OPTIONAL",
        [TOK_OWNED] = "OWNED",
        [TOK_SHARED] = "SHARED",
        [TOK_BLOCK_METADATA] = "BLOCK_METADATA",
        [TOK_BLOCK_SETUP] = "BLOCK_SETUP",
        [TOK_BLOCK_BODY] = "BLOCK_BODY",
        [TOK_BLOCK_CLOSING] = "BLOCK_CLOSING",
        [TOK_BLOCK_HEADER] = "BLOCK_HEADER",
        [TOK_BLOCK_CONTEXT] = "BLOCK_CONTEXT",
        [TOK_BLOCK_CONTENT] = "BLOCK_CONTENT",
        [TOK_BLOCK_FOOTER] = "BLOCK_FOOTER",
        [TOK_COLON] = "COLON",
        [TOK_EQUAL] = "EQUAL",
        [TOK_LPAREN] = "LPAREN",
        [TOK_RPAREN] = "RPAREN",
        [TOK_LBRACKET] = "LBRACKET",
        [TOK_RBRACKET] = "RBRACKET",
        [TOK_LBRACE] = "LBRACE",
        [TOK_RBRACE] = "RBRACE",
        [TOK_COMMA] = "COMMA",
        [TOK_DOT] = "DOT",
        [TOK_PLUS] = "PLUS",
        [TOK_MINUS] = "MINUS",
        [TOK_STAR] = "STAR",
        [TOK_SLASH] = "SLASH",
        [TOK_ARROW] = "ARROW",
        [TOK_PLUS_MINUS] = "PLUS_MINUS",
        [TOK_INFINITY] = "INFINITY",
        [TOK_STRING] = "STRING",
        [TOK_NUMBER] = "NUMBER",
        [TOK_IDENTIFIER] = "IDENTIFIER",
        [TOK_COMMENT] = "COMMENT",
        [TOK_BLOCK_COMMENT] = "BLOCK_COMMENT",
    };

    if (type >= 0 && type < TOK_COUNT_TYPES) {
        return names[type] ? names[type] : "UNKNOWN";
    }
    return "INVALID";
}

const char* omni_lexer_get_error(const OmniLexer* lexer) {
    return lexer->error;
}

// # B.5 Error Tracking Functions [ERROR_TRACK]
//
// DAR: DETECT phase - access collected errors.

int omni_lexer_error_count(const OmniLexer* lexer) {
    return lexer ? lexer->error_count : 0;
}

bool omni_lexer_had_error(const OmniLexer* lexer) {
    return lexer ? lexer->had_error : false;
}

void omni_lexer_clear_errors(OmniLexer* lexer) {
    if (lexer) {
        lexer->error_count = 0;
        lexer->had_error = false;
        lexer->error[0] = '\0';
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Validation [VALIDATION]
//
// gcc -c -Wall -Wextra -I../include omni_lexer.c -o omni_lexer.o

// # X.2 Closing Note [NOTE]
//
// "Precept upon precept; line upon line"
//   — Isaiah 28:10
//
// The lexer divides source into tokens - the first step of understanding.

// =============================================================================
// END CLOSING
// =============================================================================
