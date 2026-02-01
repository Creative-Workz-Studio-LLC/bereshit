// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-chunks-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_chunks.c — OmniCode Semantic Chunk Lexer Implementation
// The lexer that understands, not just tokenizes

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-chunks-impl
// Title: OmniCode Chunk Lexer Implementation
// Type: Source
// Role: Produce meaningful chunks from OmniCode source

// # M.8 Grounding [GROUNDING]
//
// Scripture: John 1:1 - In the beginning was the Word
// Principle: Words carry meaning - chunking is understanding

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdlib.h>
#include <string.h>
#include <strings.h>  // strcasecmp
#include <stdio.h>
#include <ctype.h>

#include "omni_chunks.h"

// # S.1 Diagnostic Emission [DIAGNOSTICS]
//
// DAR Pattern: Detect → Assess → Recover
// These helpers implement the DETECT phase - collecting issues as we parse.

// Emit a diagnostic to the lexer's diagnostic buffer.
// Returns true if added, false if buffer full.
static bool emit_diagnostic(
    OmniChunkLexer* lexer,
    const char*     code,
    int             severity,
    const char*     message,
    const char*     detail,
    const char*     recovery,
    int             line,
    int             column)
{
    if (!lexer || lexer->diagnostic_count >= OMNI_MAX_DIAGNOSTICS) {
        return false;
    }

    OmniDiagnostic* diag = &lexer->diagnostics[lexer->diagnostic_count++];

    // Safe copy all fields
    if (code) {
        size_t len = strlen(code);
        if (len >= sizeof(diag->code)) len = sizeof(diag->code) - 1;
        memcpy(diag->code, code, len);
        diag->code[len] = '\0';
    } else {
        diag->code[0] = '\0';
    }

    diag->severity = severity;

    if (message) {
        size_t len = strlen(message);
        if (len >= sizeof(diag->message)) len = sizeof(diag->message) - 1;
        memcpy(diag->message, message, len);
        diag->message[len] = '\0';
    } else {
        diag->message[0] = '\0';
    }

    if (detail) {
        size_t len = strlen(detail);
        if (len >= sizeof(diag->detail)) len = sizeof(diag->detail) - 1;
        memcpy(diag->detail, detail, len);
        diag->detail[len] = '\0';
    } else {
        diag->detail[0] = '\0';
    }

    if (recovery) {
        size_t len = strlen(recovery);
        if (len >= sizeof(diag->recovery)) len = sizeof(diag->recovery) - 1;
        memcpy(diag->recovery, recovery, len);
        diag->recovery[len] = '\0';
    } else {
        diag->recovery[0] = '\0';
    }

    diag->line = line;
    diag->column = column;
    diag->addressed = false;

    // Track severity flags
    if (severity < 0) lexer->had_error = true;
    if (severity == 0) lexer->had_warning = true;

    return true;
}

// Convenience: emit error (severity -1)
static bool emit_error(OmniChunkLexer* lexer, const char* code,
                       const char* message, const char* detail,
                       const char* recovery, int line, int column) {
    return emit_diagnostic(lexer, code, -1, message, detail, recovery, line, column);
}

// Convenience: emit warning (severity 0)
static bool emit_warning(OmniChunkLexer* lexer, const char* code,
                         const char* message, const char* detail,
                         const char* recovery, int line, int column) {
    return emit_diagnostic(lexer, code, 0, message, detail, recovery, line, column);
}

// Convenience: emit info (severity +1)
// Note: Will be used in later phases (suggestions)
__attribute__((unused))
static bool emit_info(OmniChunkLexer* lexer, const char* code,
                      const char* message, const char* detail,
                      const char* recovery, int line, int column) {
    return emit_diagnostic(lexer, code, 1, message, detail, recovery, line, column);
}

// # S.2 Scripture Validation [VALIDATION]
//
// Validates Scripture references against BibleAddressingConfig.
// "In the beginning was the Word" — John 1:1

// Validate a Scripture reference (book, chapter, verse) against config.
// Returns true if valid, false if invalid (and emits diagnostic).
static bool validate_scripture_ref(
    OmniChunkLexer* lexer,
    const char*     book,
    int             chapter,
    int             verse,
    int             line,
    int             column)
{
    // If no config loaded, skip validation (permissive mode)
    if (!lexer->config || !lexer->config->bible.loaded) {
        return true;
    }

    const BibleAddressingConfig* addr = &lexer->config->bible.addressing;

    // Find the book
    int book_index = -1;
    for (int i = 0; i < addr->book_count; i++) {
        if (strcasecmp(addr->books[i].name, book) == 0 ||
            strcasecmp(addr->books[i].abbreviation, book) == 0) {
            book_index = i;
            break;
        }
    }

    if (book_index < 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Unknown Bible book: %s", book);
        emit_warning(lexer, "B001", msg,
                     "Scripture reference uses unrecognized book name",
                     "none", line, column);
        return false;
    }

    const BibleBookConfig* bk = &addr->books[book_index];

    // Check chapter bounds
    if (chapter < 1 || chapter > bk->chapters) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Invalid chapter %d for %s (has %d chapters)",
                 chapter, book, bk->chapters);
        emit_warning(lexer, "B002", msg,
                     "Chapter number is out of range for this book",
                     "none", line, column);
        return false;
    }

    // Note: We could validate verse if we had per-chapter verse counts
    // For now, just check it's positive
    if (verse < 1) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Invalid verse %d (must be positive)", verse);
        emit_warning(lexer, "B003", msg,
                     "Verse number must be at least 1",
                     "none", line, column);
        return false;
    }

    return true;
}

// Look up Hebrew root for an OmniCode keyword.
// Returns pointer to HebrewRootConfig or NULL if not found.
static const HebrewRootConfig* lookup_hebrew_root(
    const OmniChunkLexer* lexer,
    const char*           keyword)
{
    if (!lexer->config || !lexer->config->bible.loaded) {
        return NULL;
    }

    return bible_find_hebrew_by_keyword(&lexer->config->bible.encoding, keyword);
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Helper Functions [HELPERS]

// Safe string copy
static void safe_strcpy(char* dest, size_t dest_size, const char* src) {
    if (dest_size == 0) return;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
    memcpy(dest, src, copy_len);
    dest[copy_len] = '\0';
}

// Safe string append
static void safe_strcat(char* dest, size_t dest_size, const char* src) {
    if (dest_size == 0) return;
    size_t dest_len = strlen(dest);
    if (dest_len >= dest_size - 1) return;
    size_t remaining = dest_size - dest_len - 1;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < remaining) ? src_len : remaining;
    memcpy(dest + dest_len, src, copy_len);
    dest[dest_len + copy_len] = '\0';
}

// Get next raw token, using lookahead buffer if available
static OmniToken get_token(OmniChunkLexer* lexer) {
    if (lexer->lookahead_count > 0) {
        OmniToken tok = lexer->lookahead[0];
        // Shift lookahead buffer
        for (int i = 0; i < lexer->lookahead_count - 1; i++) {
            lexer->lookahead[i] = lexer->lookahead[i + 1];
        }
        lexer->lookahead_count--;
        return tok;
    }
    return omni_lexer_next(&lexer->lexer);
}

// Peek at token N positions ahead (0 = next token)
static OmniToken peek_token(OmniChunkLexer* lexer, int ahead) {
    // Fill lookahead buffer if needed
    while (lexer->lookahead_count <= ahead) {
        lexer->lookahead[lexer->lookahead_count++] = omni_lexer_next(&lexer->lexer);
    }
    return lexer->lookahead[ahead];
}

// Check if next token matches type
static bool check_token(OmniChunkLexer* lexer, OmniTokenType type) {
    return peek_token(lexer, 0).type == type;
}

// Match and consume token if type matches
static bool match_token(OmniChunkLexer* lexer, OmniTokenType type, OmniChunk* chunk) {
    if (!check_token(lexer, type)) return false;
    OmniToken tok = get_token(lexer);
    if (chunk && chunk->token_count < CHUNK_MAX_TOKENS) {
        chunk->tokens[chunk->token_count++] = tok;
    }
    return true;
}

// Initialize chunk with first token info
static void init_chunk(OmniChunk* chunk, OmniChunkType type, OmniToken* first) {
    memset(chunk, 0, sizeof(OmniChunk));
    chunk->type = type;
    chunk->line = first->line;
    chunk->column = first->column;
    if (chunk->token_count < CHUNK_MAX_TOKENS) {
        chunk->tokens[chunk->token_count++] = *first;
    }
}

// # B.2 Pattern Recognition [PATTERNS]
//
// Each function recognizes a semantic pattern and produces a chunk.

// Pattern: "#!omni ..."
static bool try_pragma(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_PRAGMA)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_PRAGMA, &first);

    // Parse pragma: "#!omni TYPE FLAGS..."
    // Extract type (document/code/data) and flags
    const char* text = first.text;

    // Skip "#!omni "
    if (strncmp(text, "#!omni ", 7) == 0) {
        text += 7;
    }

    // Extract type word (first word after #!omni)
    char type[64] = {0};
    int i = 0;
    while (*text && *text != ' ' && *text != '\t' && i < 63) {
        type[i++] = *text++;
    }
    type[i] = '\0';
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), type);

    // Skip whitespace
    while (*text == ' ' || *text == '\t') text++;

    // Rest is flags
    safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), text);

    return true;
}

// Pattern: "#!omni meta.key = X"
// Note: The lexer returns the entire pragma as one token, so we extract value from text
static bool try_meta_key(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_META_KEY)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_META_KEY, &first);

    // Extract value from token text: "#!omni meta.key = VALUE"
    const char* eq = strchr(first.text, '=');
    if (eq) {
        eq++;  // Skip '='
        while (*eq == ' ' || *eq == '\t') eq++;  // Skip whitespace
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), eq);
    }

    return true;
}

// Pattern: "#!omni meta.from = X"
// Note: The lexer returns the entire pragma as one token, so we extract value from text
static bool try_meta_from(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_META_FROM)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_META_FROM, &first);

    // Extract value from token text: "#!omni meta.from = VALUE"
    const char* eq = strchr(first.text, '=');
    if (eq) {
        eq++;  // Skip '='
        while (*eq == ' ' || *eq == '\t') eq++;  // Skip whitespace
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), eq);
    }

    return true;
}

// Pattern: "#!omni meta.at = X"
// Note: The lexer returns the entire pragma as one token, so we extract value from text
static bool try_meta_at(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_META_AT)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_META_AT, &first);

    // Extract value from token text: "#!omni meta.at = VALUE"
    const char* eq = strchr(first.text, '=');
    if (eq) {
        eq++;  // Skip '='
        while (*eq == ' ' || *eq == '\t') eq++;  // Skip whitespace
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), eq);
    }

    return true;
}

// Pattern: "this is a folder for X"
static bool try_entity_declaration(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_THIS)) return false;

    // Look ahead: "this is a <entity>"
    if (peek_token(lexer, 1).type != TOK_IS) return false;
    if (peek_token(lexer, 2).type != TOK_A) return false;

    OmniToken first = get_token(lexer);  // this
    init_chunk(chunk, CHUNK_ENTITY_FOLDER, &first);  // Default, will update

    match_token(lexer, TOK_IS, chunk);
    match_token(lexer, TOK_A, chunk);

    // Entity type
    OmniToken entity = peek_token(lexer, 0);
    if (entity.type == TOK_FOLDER) {
        chunk->type = CHUNK_ENTITY_FOLDER;
    } else if (entity.type == TOK_FILE) {
        chunk->type = CHUNK_ENTITY_FILE;
    } else if (entity.type == TOK_MODULE) {
        chunk->type = CHUNK_ENTITY_MODULE;
    } else {
        // Unknown entity type, store as-is
        safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), entity.text);
    }
    match_token(lexer, entity.type, chunk);

    // "for" clause with name
    if (match_token(lexer, TOK_FOR, chunk)) {
        if (check_token(lexer, TOK_STRING)) {
            OmniToken name = get_token(lexer);
            chunk->tokens[chunk->token_count++] = name;
            safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), name.text);
        }
    }

    return true;
}

// Pattern: "keyed as X"
static bool try_identity_key(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_KEYED)) return false;
    if (peek_token(lexer, 1).type != TOK_AS) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_IDENTITY_KEY, &first);

    match_token(lexer, TOK_AS, chunk);

    if (check_token(lexer, TOK_STRING)) {
        OmniToken val = get_token(lexer);
        chunk->tokens[chunk->token_count++] = val;
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), val.text);
    }

    return true;
}

// Pattern: "named X"
static bool try_identity_name(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_NAMED)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_IDENTITY_NAME, &first);

    if (check_token(lexer, TOK_STRING)) {
        OmniToken val = get_token(lexer);
        chunk->tokens[chunk->token_count++] = val;
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), val.text);
    }

    return true;
}

// Pattern: "grounded in:"
static bool try_grounding_header(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_GROUNDED)) return false;
    if (peek_token(lexer, 1).type != TOK_IN) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_GROUND_HEADER, &first);

    match_token(lexer, TOK_IN, chunk);
    match_token(lexer, TOK_COLON, chunk);

    return true;
}

// Pattern: "scripture X from Book C:V"
static bool try_grounding_scripture(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_SCRIPTURE)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_GROUND_SCRIPTURE, &first);

    // Scripture description in quotes
    if (check_token(lexer, TOK_STRING)) {
        OmniToken desc = get_token(lexer);
        chunk->tokens[chunk->token_count++] = desc;
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), desc.text);
    }

    // "from Book Chapter:Verse"
    if (match_token(lexer, TOK_FROM, chunk)) {
        // Book name
        if (check_token(lexer, TOK_IDENTIFIER)) {
            OmniToken book = get_token(lexer);
            chunk->tokens[chunk->token_count++] = book;
            safe_strcpy(chunk->book, sizeof(chunk->book), book.text);
        }

        // Chapter:Verse (may be multiple tokens)
        chunk->chapter[0] = '\0';
        chunk->verse[0] = '\0';

        // Collect until "in" or newline
        char ref[64] = "";
        while (!check_token(lexer, TOK_IN) &&
               !check_token(lexer, TOK_NEWLINE) &&
               !check_token(lexer, TOK_EOF)) {
            OmniToken tok = get_token(lexer);
            if (chunk->token_count < CHUNK_MAX_TOKENS) {
                chunk->tokens[chunk->token_count++] = tok;
            }
            safe_strcat(ref, sizeof(ref), tok.text);
        }

        // Parse chapter:verse from ref
        char* colon = strchr(ref, ':');
        if (colon) {
            *colon = '\0';
            safe_strcpy(chunk->chapter, sizeof(chunk->chapter), ref);
            safe_strcpy(chunk->verse, sizeof(chunk->verse), colon + 1);
        } else {
            safe_strcpy(chunk->chapter, sizeof(chunk->chapter), ref);
        }

        // "in KJV" or similar
        if (match_token(lexer, TOK_IN, chunk)) {
            if (check_token(lexer, TOK_IDENTIFIER)) {
                OmniToken ver = get_token(lexer);
                chunk->tokens[chunk->token_count++] = ver;
                safe_strcpy(chunk->tertiary_value, sizeof(chunk->tertiary_value), ver.text);
            }
        }

        // Validate Scripture reference against Bible config (DAR: Detect)
        if (chunk->book[0] && chunk->chapter[0]) {
            int chap = atoi(chunk->chapter);
            int vers = chunk->verse[0] ? atoi(chunk->verse) : 1;
            validate_scripture_ref(lexer, chunk->book, chap, vers,
                                   chunk->line, chunk->column);
        }
    }

    return true;
}

// Pattern: "principle X"
static bool try_grounding_principle(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_PRINCIPLE)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_GROUND_PRINCIPLE, &first);

    if (check_token(lexer, TOK_STRING)) {
        OmniToken val = get_token(lexer);
        chunk->tokens[chunk->token_count++] = val;
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), val.text);
    }

    return true;
}

// Pattern: "anchor X"
static bool try_grounding_anchor(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_ANCHOR)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_GROUND_ANCHOR, &first);

    if (check_token(lexer, TOK_STRING)) {
        OmniToken val = get_token(lexer);
        chunk->tokens[chunk->token_count++] = val;
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), val.text);
    }

    return true;
}

// Pattern: "serves as Zone:"
static bool try_role_header(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_SERVES)) return false;
    if (peek_token(lexer, 1).type != TOK_AS) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ROLE_HEADER, &first);

    match_token(lexer, TOK_AS, chunk);

    // Zone name
    OmniToken zone = peek_token(lexer, 0);
    if (zone.type == TOK_ZONE_WORD || zone.type == TOK_ZONE_VOID ||
        zone.type == TOK_ZONE_TOV || zone.type == TOK_ROLE_MEMORY ||
        zone.type == TOK_IDENTIFIER) {
        get_token(lexer);
        chunk->tokens[chunk->token_count++] = zone;
        safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), zone.text);
    }

    match_token(lexer, TOK_COLON, chunk);

    return true;
}

// Pattern: "role X"
static bool try_role(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_ROLE)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ROLE_ROLE, &first);

    if (check_token(lexer, TOK_STRING)) {
        OmniToken val = get_token(lexer);
        chunk->tokens[chunk->token_count++] = val;
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), val.text);
    }

    return true;
}

// Pattern: "purpose X"
static bool try_purpose(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_PURPOSE)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ROLE_PURPOSE, &first);

    if (check_token(lexer, TOK_STRING)) {
        OmniToken val = get_token(lexer);
        chunk->tokens[chunk->token_count++] = val;
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), val.text);
    }

    return true;
}

// Pattern: "authored by:"
static bool try_attribution_header(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_AUTHORED)) return false;
    if (peek_token(lexer, 1).type != TOK_BY) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ATTR_HEADER, &first);

    match_token(lexer, TOK_BY, chunk);
    match_token(lexer, TOK_COLON, chunk);

    return true;
}

// Pattern: "architect X"
static bool try_architect(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_ARCHITECT)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ATTR_ARCHITECT, &first);

    if (check_token(lexer, TOK_STRING)) {
        OmniToken val = get_token(lexer);
        chunk->tokens[chunk->token_count++] = val;
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), val.text);
    }

    return true;
}

// Pattern: "implementation X"
static bool try_implementation(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_IMPLEMENTATION)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ATTR_IMPLEMENTATION, &first);

    if (check_token(lexer, TOK_STRING)) {
        OmniToken val = get_token(lexer);
        chunk->tokens[chunk->token_count++] = val;
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), val.text);
    }

    return true;
}

// Pattern: "created YYYY-MM-DD"
static bool try_created(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_CREATED)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ATTR_CREATED, &first);

    // Collect date tokens
    chunk->primary_value[0] = '\0';
    while (check_token(lexer, TOK_NUMBER) || check_token(lexer, TOK_MINUS)) {
        OmniToken tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = tok;
        }
        safe_strcat(chunk->primary_value, sizeof(chunk->primary_value), tok.text);
    }

    return true;
}

// Pattern: "version X"
static bool try_version(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_VERSION)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ATTR_VERSION, &first);

    // Collect version tokens (a-01.00)
    chunk->primary_value[0] = '\0';
    while (check_token(lexer, TOK_IDENTIFIER) || check_token(lexer, TOK_NUMBER) ||
           check_token(lexer, TOK_MINUS) || check_token(lexer, TOK_DOT)) {
        OmniToken tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = tok;
        }
        safe_strcat(chunk->primary_value, sizeof(chunk->primary_value), tok.text);
    }

    return true;
}

// Pattern: "exists to:"
static bool try_purpose_header(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_EXISTS)) return false;
    if (peek_token(lexer, 1).type != TOK_TO) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_PURPOSE_HEADER, &first);

    match_token(lexer, TOK_TO, chunk);
    match_token(lexer, TOK_COLON, chunk);

    return true;
}

// Pattern: Sentence-style block markers
// "this begins METADATA" / "this ends METADATA"
// OmniCode native syntax - the structure IS the code
static bool try_sentence_block(OmniChunkLexer* lexer, OmniChunk* chunk) {
    // Pattern: THIS + "begins"/"ends" + BLOCKNAME
    if (!check_token(lexer, TOK_THIS)) return false;

    // Peek ahead for "begins" or "ends"
    OmniToken second = peek_token(lexer, 1);
    if (second.type != TOK_IDENTIFIER) return false;

    bool is_end = false;
    if (strcasecmp(second.text, "begins") == 0) {
        is_end = false;
    } else if (strcasecmp(second.text, "ends") == 0) {
        is_end = true;
    } else {
        return false;  // Not a block sentence
    }

    // Third token should be block name
    OmniToken third = peek_token(lexer, 2);
    if (third.type != TOK_IDENTIFIER) return false;

    // Map block name to chunk type
    OmniChunkType type;
    OmniBlockContext ctx;
    if (strcasecmp(third.text, "METADATA") == 0) {
        type = CHUNK_BLOCK_METADATA;
        ctx = BLOCK_METADATA;
    } else if (strcasecmp(third.text, "HEADER") == 0) {
        type = CHUNK_BLOCK_HEADER;
        ctx = BLOCK_HEADER;
    } else if (strcasecmp(third.text, "CONTEXT") == 0) {
        type = CHUNK_BLOCK_CONTEXT;
        ctx = BLOCK_CONTEXT;
    } else if (strcasecmp(third.text, "SETUP") == 0) {
        type = CHUNK_BLOCK_SETUP;
        ctx = BLOCK_SETUP;
    } else if (strcasecmp(third.text, "BODY") == 0) {
        type = CHUNK_BLOCK_BODY;
        ctx = BLOCK_BODY;
    } else if (strcasecmp(third.text, "CONTENT") == 0) {
        type = CHUNK_BLOCK_CONTENT;
        ctx = BLOCK_CONTENT;
    } else if (strcasecmp(third.text, "CLOSING") == 0) {
        type = CHUNK_BLOCK_CLOSING;
        ctx = BLOCK_CLOSING;
    } else if (strcasecmp(third.text, "FOOTER") == 0) {
        type = CHUNK_BLOCK_FOOTER;
        ctx = BLOCK_FOOTER;
    } else {
        return false;  // Unknown block name
    }

    // Consume all three tokens
    OmniToken first = get_token(lexer);  // this
    OmniToken second_tok = get_token(lexer);  // begins/ends
    OmniToken third_tok = get_token(lexer);   // BLOCKNAME

    (void)second_tok; (void)third_tok;  // Suppress unused warnings

    init_chunk(chunk, type, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value),
                is_end ? "END" : "START");

    // Update block context
    lexer->block = ctx;

    return true;
}

// Pattern: Block markers [METADATA], [CONTENT], etc.
static bool try_block_marker(OmniChunkLexer* lexer, OmniChunk* chunk) {
    OmniToken tok = peek_token(lexer, 0);

    OmniChunkType type;
    OmniBlockContext ctx;

    switch (tok.type) {
        case TOK_BLOCK_METADATA:
            type = CHUNK_BLOCK_METADATA;
            ctx = BLOCK_METADATA;
            break;
        case TOK_BLOCK_HEADER:
            type = CHUNK_BLOCK_HEADER;
            ctx = BLOCK_HEADER;
            break;
        case TOK_BLOCK_CONTEXT:
            type = CHUNK_BLOCK_CONTEXT;
            ctx = BLOCK_CONTEXT;
            break;
        case TOK_BLOCK_SETUP:
            type = CHUNK_BLOCK_SETUP;
            ctx = BLOCK_SETUP;
            break;
        case TOK_BLOCK_BODY:
            type = CHUNK_BLOCK_BODY;
            ctx = BLOCK_BODY;
            break;
        case TOK_BLOCK_CONTENT:
            type = CHUNK_BLOCK_CONTENT;
            ctx = BLOCK_CONTENT;
            break;
        case TOK_BLOCK_CLOSING:
            type = CHUNK_BLOCK_CLOSING;
            ctx = BLOCK_CLOSING;
            break;
        case TOK_BLOCK_FOOTER:
            type = CHUNK_BLOCK_FOOTER;
            ctx = BLOCK_FOOTER;
            break;
        default:
            return false;
    }

    OmniToken first = get_token(lexer);
    init_chunk(chunk, type, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), first.text);

    // Update block context
    lexer->block = ctx;

    return true;
}

// Pattern: Comment (with block marker and pragma detection)
// Block markers appear as comments like "// METADATA" or "// [METADATA]"
// Pragmas appear as comments like "// #!omni code" or "// #!omni meta.key = X"
static bool try_comment(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_COMMENT)) return false;

    OmniToken first = get_token(lexer);
    const char* text = first.text;

    // Skip leading whitespace in comment text
    while (*text == ' ' || *text == '\t') text++;

    // =========================================================================
    // Check for #!omni pragmas inside comments (MUST be first)
    // This is how OmniCode embeds in host languages like C, Go, etc.
    // =========================================================================

    // Check for "#!omni meta.key = VALUE"
    if (strncmp(text, "#!omni meta.key", 15) == 0) {
        init_chunk(chunk, CHUNK_META_KEY, &first);
        const char* eq = strchr(text, '=');
        if (eq) {
            eq++;
            while (*eq == ' ' || *eq == '\t') eq++;
            safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), eq);
        }
        return true;
    }

    // Check for "#!omni meta.from = VALUE"
    if (strncmp(text, "#!omni meta.from", 16) == 0) {
        init_chunk(chunk, CHUNK_META_FROM, &first);
        const char* eq = strchr(text, '=');
        if (eq) {
            eq++;
            while (*eq == ' ' || *eq == '\t') eq++;
            safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), eq);
        }
        return true;
    }

    // Check for "#!omni meta.at = VALUE"
    if (strncmp(text, "#!omni meta.at", 14) == 0) {
        init_chunk(chunk, CHUNK_META_AT, &first);
        const char* eq = strchr(text, '=');
        if (eq) {
            eq++;
            while (*eq == ' ' || *eq == '\t') eq++;
            safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), eq);
        }
        return true;
    }

    // Check for "#!omni TYPE FLAGS..." (main pragma line)
    if (strncmp(text, "#!omni ", 7) == 0) {
        init_chunk(chunk, CHUNK_PRAGMA, &first);
        text += 7;  // Skip "#!omni "

        // Extract type word (first word after #!omni)
        char type[64] = {0};
        int i = 0;
        while (*text && *text != ' ' && *text != '\t' && i < 63) {
            type[i++] = *text++;
        }
        type[i] = '\0';
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), type);

        // Skip whitespace
        while (*text == ' ' || *text == '\t') text++;

        // Rest is flags
        safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), text);

        return true;
    }

    // =========================================================================
    // Check for block markers (with or without brackets)
    // Format: "METADATA", "[METADATA]", "END METADATA", etc.
    // =========================================================================

    // Check for END markers first (they're longer)
    if (strncasecmp(text, "END METADATA", 12) == 0 ||
        strncasecmp(text, "END [METADATA]", 14) == 0) {
        // END markers - still emit block type for tracking
        init_chunk(chunk, CHUNK_BLOCK_METADATA, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "END");
        return true;
    }
    if (strncasecmp(text, "END HEADER", 10) == 0 ||
        strncasecmp(text, "END [HEADER]", 12) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_HEADER, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "END");
        return true;
    }
    if (strncasecmp(text, "END CONTEXT", 11) == 0 ||
        strncasecmp(text, "END [CONTEXT]", 13) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_CONTEXT, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "END");
        return true;
    }
    if (strncasecmp(text, "END SETUP", 9) == 0 ||
        strncasecmp(text, "END [SETUP]", 11) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_SETUP, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "END");
        return true;
    }
    if (strncasecmp(text, "END BODY", 8) == 0 ||
        strncasecmp(text, "END [BODY]", 10) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_BODY, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "END");
        return true;
    }
    if (strncasecmp(text, "END CONTENT", 11) == 0 ||
        strncasecmp(text, "END [CONTENT]", 13) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_CONTENT, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "END");
        return true;
    }
    if (strncasecmp(text, "END CLOSING", 11) == 0 ||
        strncasecmp(text, "END [CLOSING]", 13) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_CLOSING, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "END");
        return true;
    }
    if (strncasecmp(text, "END FOOTER", 10) == 0 ||
        strncasecmp(text, "END [FOOTER]", 12) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_FOOTER, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "END");
        return true;
    }

    // Check for START markers (just the block name)
    // Must be exact match or followed by space/bracket
    if (strncasecmp(text, "METADATA", 8) == 0 &&
        (text[8] == '\0' || text[8] == ' ' || text[8] == '[')) {
        init_chunk(chunk, CHUNK_BLOCK_METADATA, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "[METADATA]", 10) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_METADATA, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "HEADER", 6) == 0 &&
        (text[6] == '\0' || text[6] == ' ' || text[6] == '[')) {
        init_chunk(chunk, CHUNK_BLOCK_HEADER, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "[HEADER]", 8) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_HEADER, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "CONTEXT", 7) == 0 &&
        (text[7] == '\0' || text[7] == ' ' || text[7] == '[')) {
        init_chunk(chunk, CHUNK_BLOCK_CONTEXT, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "[CONTEXT]", 9) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_CONTEXT, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "SETUP", 5) == 0 &&
        (text[5] == '\0' || text[5] == ' ' || text[5] == '[')) {
        init_chunk(chunk, CHUNK_BLOCK_SETUP, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "[SETUP]", 7) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_SETUP, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "BODY", 4) == 0 &&
        (text[4] == '\0' || text[4] == ' ' || text[4] == '[')) {
        init_chunk(chunk, CHUNK_BLOCK_BODY, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "[BODY]", 6) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_BODY, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "CONTENT", 7) == 0 &&
        (text[7] == '\0' || text[7] == ' ' || text[7] == '[')) {
        init_chunk(chunk, CHUNK_BLOCK_CONTENT, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "[CONTENT]", 9) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_CONTENT, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "CLOSING", 7) == 0 &&
        (text[7] == '\0' || text[7] == ' ' || text[7] == '[')) {
        init_chunk(chunk, CHUNK_BLOCK_CLOSING, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "[CLOSING]", 9) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_CLOSING, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "FOOTER", 6) == 0 &&
        (text[6] == '\0' || text[6] == ' ' || text[6] == '[')) {
        init_chunk(chunk, CHUNK_BLOCK_FOOTER, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }
    if (strncasecmp(text, "[FOOTER]", 8) == 0) {
        init_chunk(chunk, CHUNK_BLOCK_FOOTER, &first);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "START");
        return true;
    }

    // Not a block marker - regular comment
    init_chunk(chunk, CHUNK_COMMENT, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), first.text);

    return true;
}

// Pattern: String literal
static bool try_string(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_STRING)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_STRING_VALUE, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), first.text);

    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// B.2.5 Executable Code Patterns [EXECUTABLE]
// OmniCode executable constructs - the language that DOES things
// "Let there be light: and there was light" — Genesis 1:3
// ═══════════════════════════════════════════════════════════════════════════

// Helper: Check if token is a type keyword
static bool is_type_token(OmniTokenType type) {
    return type == TOK_COUNT ||     // int
           type == TOK_WHOLE ||     // uint
           type == TOK_MEASURE ||   // float
           type == TOK_WORD ||      // string
           type == TOK_CHAR ||      // char
           type == TOK_SCROLL ||    // long text
           type == TOK_TRUTH ||     // bool
           type == TOK_COVENANT ||  // ternary
           type == TOK_GATHERING || // array
           type == TOK_ASSEMBLY ||  // struct
           type == TOK_OPTIONAL;    // nullable
}

// Helper: Check if a token can be used as a variable name
// OmniCode allows keywords to be used as names (like "health", "status")
// Only control flow keywords should be reserved
static bool is_name_token(OmniTokenType type) {
    // Reserved: control flow and structural keywords
    if (type == TOK_LET || type == TOK_WHEN || type == TOK_OTHERWISE ||
        type == TOK_ESTABLISH || type == TOK_PROVIDE || type == TOK_WORK ||
        type == TOK_ATTEMPT || type == TOK_RECOVER || type == TOK_ENSURE ||
        type == TOK_FAIL || type == TOK_IMPORT || type == TOK_EXPORT ||
        type == TOK_SPAWN || type == TOK_AWAIT || type == TOK_REPEAT ||
        type == TOK_THROUGH || type == TOK_FROM || type == TOK_ASSEMBLY) {
        return false;
    }
    // Accept identifiers and any other keyword (used as soft keywords)
    return type == TOK_IDENTIFIER ||
           (type >= TOK_LET && type <= TOK_REJECTED);  // All keywords in the enum range
}

// Pattern: "let name be type" or "let name be type with value"
// Examples:
//   let health be count
//   let message be word with "Hello"
//   let status be covenant with granted
static bool try_let_declaration(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_LET)) return false;

    OmniToken first = get_token(lexer);  // consume 'let'

    // Get variable name - allow keywords as names (soft keywords)
    OmniToken name_tok = peek_token(lexer, 0);
    if (!is_name_token(name_tok.type)) {
        // Not a valid let statement
        // Note: We've consumed 'let' but can't put it back
        // This is a parser limitation - 'let' will be lost
        return false;
    }
    get_token(lexer);  // consume name

    // Expect 'be'
    if (!check_token(lexer, TOK_BE)) {
        return false;
    }
    get_token(lexer);  // consume 'be'

    // Get type
    OmniToken type_tok = peek_token(lexer, 0);
    bool has_type = is_type_token(type_tok.type) || type_tok.type == TOK_IDENTIFIER;
    if (has_type) {
        get_token(lexer);  // consume type
    }

    // Check for 'with value' initialization
    bool has_init = check_token(lexer, TOK_WITH);
    if (has_init) {
        init_chunk(chunk, CHUNK_LET_INIT, &first);
        get_token(lexer);  // consume 'with'

        // Collect value tokens until newline or EOF
        char value_buf[256] = "";
        while (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
            OmniToken val_tok = get_token(lexer);
            if (chunk->token_count < CHUNK_MAX_TOKENS) {
                chunk->tokens[chunk->token_count++] = val_tok;
            }
            if (value_buf[0]) safe_strcat(value_buf, sizeof(value_buf), " ");
            safe_strcat(value_buf, sizeof(value_buf), val_tok.text);
        }
        safe_strcpy(chunk->tertiary_value, sizeof(chunk->tertiary_value), value_buf);
    } else {
        init_chunk(chunk, CHUNK_LET_DECL, &first);
    }

    // Store name and type
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), name_tok.text);
    if (has_type) {
        safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), type_tok.text);
    }

    return true;
}

// Pattern: "return value" or "return"
// Examples:
//   return granted
//   return health plus 10
//   return
static bool try_return_statement(OmniChunkLexer* lexer, OmniChunk* chunk) {
    // "return" is not a keyword (TOK_RETURNS is "returns"), so check by text
    OmniToken tok = peek_token(lexer, 0);
    if (tok.type != TOK_IDENTIFIER || strcasecmp(tok.text, "return") != 0) {
        return false;
    }

    OmniToken first = get_token(lexer);  // consume 'return'
    init_chunk(chunk, CHUNK_FUNC_RETURN, &first);

    // Collect return value tokens until newline or EOF
    char value_buf[256] = "";
    while (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
        OmniToken val_tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = val_tok;
        }
        if (value_buf[0]) safe_strcat(value_buf, sizeof(value_buf), " ");
        safe_strcat(value_buf, sizeof(value_buf), val_tok.text);
    }
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), value_buf);

    return true;
}

// Pattern: "when condition:"
// Examples:
//   when health is greater than 50:
//   when status is granted:
static bool try_when_condition(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_WHEN)) return false;

    OmniToken first = get_token(lexer);  // consume 'when'
    init_chunk(chunk, CHUNK_WHEN_COND, &first);

    // Collect condition tokens until colon or newline
    char cond_buf[256] = "";
    while (!check_token(lexer, TOK_COLON) &&
           !check_token(lexer, TOK_NEWLINE) &&
           !check_token(lexer, TOK_EOF)) {
        OmniToken cond_tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = cond_tok;
        }
        if (cond_buf[0]) safe_strcat(cond_buf, sizeof(cond_buf), " ");
        safe_strcat(cond_buf, sizeof(cond_buf), cond_tok.text);
    }
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), cond_buf);

    // Consume trailing colon if present
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: "otherwise:"
static bool try_otherwise(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_OTHERWISE)) return false;

    OmniToken first = get_token(lexer);  // consume 'otherwise'
    init_chunk(chunk, CHUNK_OTHERWISE, &first);

    // Consume trailing colon if present
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: "establish work name that takes params returns type:"
// Examples:
//   establish work calculate_health that takes count damage returns count:
//   establish work greet that returns word:
static bool try_function_definition(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_ESTABLISH) && !check_token(lexer, TOK_PROVIDE)) {
        return false;
    }

    bool is_public = check_token(lexer, TOK_PROVIDE);
    OmniToken first = get_token(lexer);  // consume 'establish' or 'provide'

    // Expect 'work'
    if (!check_token(lexer, TOK_WORK)) {
        return false;
    }
    get_token(lexer);  // consume 'work'

    // Get function name
    OmniToken name_tok = peek_token(lexer, 0);
    if (name_tok.type != TOK_IDENTIFIER) {
        return false;
    }
    get_token(lexer);  // consume name

    init_chunk(chunk, is_public ? CHUNK_FUNC_PROVIDE : CHUNK_FUNC_DEF, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), name_tok.text);

    // Parse 'that takes' parameters if present
    if (check_token(lexer, TOK_THAT)) {
        get_token(lexer);  // consume 'that'
        if (check_token(lexer, TOK_TAKES)) {
            get_token(lexer);  // consume 'takes'

            // Collect parameter tokens until 'returns' or colon
            char params_buf[256] = "";
            while (!check_token(lexer, TOK_RETURNS) &&
                   !check_token(lexer, TOK_COLON) &&
                   !check_token(lexer, TOK_NEWLINE) &&
                   !check_token(lexer, TOK_EOF)) {
                OmniToken param_tok = get_token(lexer);
                if (chunk->token_count < CHUNK_MAX_TOKENS) {
                    chunk->tokens[chunk->token_count++] = param_tok;
                }
                if (params_buf[0]) safe_strcat(params_buf, sizeof(params_buf), " ");
                safe_strcat(params_buf, sizeof(params_buf), param_tok.text);
            }
            safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), params_buf);
        }
    }

    // Parse 'returns type' if present
    if (check_token(lexer, TOK_RETURNS)) {
        get_token(lexer);  // consume 'returns'

        // Collect return type tokens until colon or newline
        char return_buf[64] = "";
        while (!check_token(lexer, TOK_COLON) &&
               !check_token(lexer, TOK_NEWLINE) &&
               !check_token(lexer, TOK_EOF)) {
            OmniToken ret_tok = get_token(lexer);
            if (chunk->token_count < CHUNK_MAX_TOKENS) {
                chunk->tokens[chunk->token_count++] = ret_tok;
            }
            if (return_buf[0]) safe_strcat(return_buf, sizeof(return_buf), " ");
            safe_strcat(return_buf, sizeof(return_buf), ret_tok.text);
        }
        safe_strcpy(chunk->tertiary_value, sizeof(chunk->tertiary_value), return_buf);
    }

    // Consume trailing colon if present
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: "witness message with level"
// Examples:
//   witness "Access granted" with info
//   witness message with error
static bool try_witness(OmniChunkLexer* lexer, OmniChunk* chunk) {
    // Check for 'witness' - but not as part of metadata (.witness files)
    // In executable context, 'witness' is a logging statement
    if (peek_token(lexer, 0).type != TOK_IDENTIFIER) return false;
    if (strcasecmp(peek_token(lexer, 0).text, "witness") != 0) return false;

    OmniToken first = get_token(lexer);  // consume 'witness'
    init_chunk(chunk, CHUNK_WITNESS, &first);

    // Get message (usually a string or expression)
    char msg_buf[256] = "";
    while (!check_token(lexer, TOK_WITH) &&
           !check_token(lexer, TOK_NEWLINE) &&
           !check_token(lexer, TOK_EOF)) {
        OmniToken msg_tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = msg_tok;
        }
        if (msg_buf[0]) safe_strcat(msg_buf, sizeof(msg_buf), " ");
        safe_strcat(msg_buf, sizeof(msg_buf), msg_tok.text);
    }
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), msg_buf);

    // Parse 'with level' if present
    if (check_token(lexer, TOK_WITH)) {
        get_token(lexer);  // consume 'with'

        // Get level
        OmniToken level_tok = peek_token(lexer, 0);
        if (level_tok.type == TOK_IDENTIFIER) {
            get_token(lexer);  // consume level
            safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), level_tok.text);
        }
    }

    return true;
}

// Pattern: "display value"
// Examples:
//   display "Hello, World!"
//   display result
static bool try_display(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (peek_token(lexer, 0).type != TOK_IDENTIFIER) return false;
    if (strcasecmp(peek_token(lexer, 0).text, "display") != 0) return false;

    OmniToken first = get_token(lexer);  // consume 'display'
    init_chunk(chunk, CHUNK_DISPLAY, &first);

    // Collect value tokens until newline or EOF
    char value_buf[256] = "";
    while (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
        OmniToken val_tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = val_tok;
        }
        if (value_buf[0]) safe_strcat(value_buf, sizeof(value_buf), " ");
        safe_strcat(value_buf, sizeof(value_buf), val_tok.text);
    }
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), value_buf);

    return true;
}

// =============================================================================
// CPI-SI State Operations
// =============================================================================

// Pattern: "health get/add/set"
// Examples:
//   health get           → push health onto stack
//   health add 10        → add to health
//   health set 50        → set health value
static bool try_health(OmniChunkLexer* lexer, OmniChunk* chunk) {
    // Check for TOK_HEALTH keyword (not TOK_IDENTIFIER - lexer recognizes this)
    if (peek_token(lexer, 0).type != TOK_HEALTH) return false;

    OmniToken first = get_token(lexer);  // consume 'health'
    init_chunk(chunk, CHUNK_HEALTH, &first);

    // Get operation: get, add, set
    if (check_token(lexer, TOK_NEWLINE) || check_token(lexer, TOK_EOF)) {
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "get");
        return true;
    }

    OmniToken op_tok = get_token(lexer);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), op_tok.text);

    // Get value for add/set
    if (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
        OmniToken val_tok = get_token(lexer);
        safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), val_tok.text);
    }

    return true;
}

// Pattern: "state get/transition"
// Examples:
//   state get            → push state onto stack
//   state transition 1   → transition with k-factor
static bool try_state(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (peek_token(lexer, 0).type != TOK_IDENTIFIER) return false;
    if (strcasecmp(peek_token(lexer, 0).text, "state") != 0) return false;

    OmniToken first = get_token(lexer);  // consume 'state'
    init_chunk(chunk, CHUNK_STATE, &first);

    // Get operation: get, transition
    if (check_token(lexer, TOK_NEWLINE) || check_token(lexer, TOK_EOF)) {
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "get");
        return true;
    }

    OmniToken op_tok = get_token(lexer);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), op_tok.text);

    // Get k-factor for transition
    if (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
        OmniToken val_tok = get_token(lexer);
        safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), val_tok.text);
    }

    return true;
}

// Pattern: "checkpoint [as name]"
// Examples:
//   checkpoint           → save VM state
//   checkpoint as "pre-change"  → save with name
static bool try_checkpoint(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (peek_token(lexer, 0).type != TOK_IDENTIFIER) return false;
    if (strcasecmp(peek_token(lexer, 0).text, "checkpoint") != 0) return false;

    OmniToken first = get_token(lexer);  // consume 'checkpoint'
    init_chunk(chunk, CHUNK_CHECKPOINT, &first);

    // Check for 'as name'
    if (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
        OmniToken as_tok = peek_token(lexer, 0);
        if (strcasecmp(as_tok.text, "as") == 0) {
            get_token(lexer);  // consume 'as'
            if (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
                OmniToken name_tok = get_token(lexer);
                safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), name_tok.text);
            }
        }
    }

    return true;
}

// Pattern: "restore [from name]"
// Examples:
//   restore              → restore last checkpoint
//   restore from "pre-change"  → restore specific checkpoint
static bool try_restore(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (peek_token(lexer, 0).type != TOK_IDENTIFIER) return false;
    if (strcasecmp(peek_token(lexer, 0).text, "restore") != 0) return false;

    OmniToken first = get_token(lexer);  // consume 'restore'
    init_chunk(chunk, CHUNK_RESTORE, &first);

    // Check for 'from name'
    if (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
        OmniToken from_tok = peek_token(lexer, 0);
        if (strcasecmp(from_tok.text, "from") == 0) {
            get_token(lexer);  // consume 'from'
            if (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
                OmniToken name_tok = get_token(lexer);
                safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), name_tok.text);
            }
        }
    }

    return true;
}

// Pattern: "intrinsic name [with arg1 [and arg2 [and arg3]]]"
// Examples:
//   intrinsic str_len s              → string length intrinsic
//   intrinsic str_concat a b         → string concat intrinsic
//   intrinsic file_open path mode    → file open intrinsic
static bool try_intrinsic(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (peek_token(lexer, 0).type != TOK_IDENTIFIER) return false;
    if (strcasecmp(peek_token(lexer, 0).text, "intrinsic") != 0) return false;

    OmniToken first = get_token(lexer);  // consume 'intrinsic'
    init_chunk(chunk, CHUNK_INTRINSIC, &first);

    // Get intrinsic name (required)
    if (check_token(lexer, TOK_NEWLINE) || check_token(lexer, TOK_EOF)) {
        return false;  // intrinsic name is required
    }

    OmniToken name_tok = get_token(lexer);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), name_tok.text);

    // Collect arguments (up to 4)
    int arg_index = 0;
    while (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF) && arg_index < 4) {
        OmniToken tok = peek_token(lexer, 0);

        // Skip 'with' and 'and' keywords
        if (strcasecmp(tok.text, "with") == 0 || strcasecmp(tok.text, "and") == 0) {
            get_token(lexer);
            continue;
        }

        // Store argument
        OmniToken arg_tok = get_token(lexer);
        if (arg_index < (int)CHUNK_MAX_TOKENS) {
            chunk->tokens[arg_index] = arg_tok;
            chunk->token_count++;
        }
        arg_index++;
    }

    return true;
}

// Pattern: Covenant values (granted/deferred/denied)
static bool try_covenant_value(OmniChunkLexer* lexer, OmniChunk* chunk) {
    OmniToken tok = peek_token(lexer, 0);
    if (tok.type != TOK_GRANTED && tok.type != TOK_DEFERRED && tok.type != TOK_DENIED) {
        return false;
    }

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_COVENANT_VALUE, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), first.text);

    // Store ternary value
    if (tok.type == TOK_GRANTED) {
        chunk->ternary_value = 1;
    } else if (tok.type == TOK_DENIED) {
        chunk->ternary_value = -1;
    } else {
        chunk->ternary_value = 0;
    }
    chunk->has_ternary = true;

    return true;
}

// Pattern: Truth values (yes/no)
static bool try_truth_value(OmniChunkLexer* lexer, OmniChunk* chunk) {
    OmniToken tok = peek_token(lexer, 0);
    if (tok.type != TOK_YES && tok.type != TOK_NO) {
        return false;
    }

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_TRUTH_VALUE, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), first.text);

    // Store ternary value (yes=+1, no=-1)
    chunk->ternary_value = (tok.type == TOK_YES) ? 1 : -1;
    chunk->has_ternary = true;

    return true;
}

// Pattern: None value
static bool try_none_value(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_NONE)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_NONE_VALUE, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), "none");

    return true;
}

// Pattern: Type reference (standalone type name)
static bool try_type_reference(OmniChunkLexer* lexer, OmniChunk* chunk) {
    OmniToken tok = peek_token(lexer, 0);
    if (!is_type_token(tok.type)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_TYPE_REF, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), first.text);

    return true;
}

// Pattern: "attempt:"
static bool try_attempt(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_ATTEMPT)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ATTEMPT, &first);

    // Consume trailing colon if present
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: "recover:"
static bool try_recover(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_RECOVER)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_RECOVER, &first);

    // Consume trailing colon if present
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: "ensure:"
static bool try_ensure(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_ENSURE)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_ENSURE, &first);

    // Consume trailing colon if present
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: "fail with message"
static bool try_fail(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_FAIL)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_FAIL_WITH, &first);

    // Optional 'with'
    if (check_token(lexer, TOK_WITH)) {
        get_token(lexer);
    }

    // Collect message tokens
    char msg_buf[256] = "";
    while (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
        OmniToken msg_tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = msg_tok;
        }
        if (msg_buf[0]) safe_strcat(msg_buf, sizeof(msg_buf), " ");
        safe_strcat(msg_buf, sizeof(msg_buf), msg_tok.text);
    }
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), msg_buf);

    return true;
}

// Pattern: "import name from path"
static bool try_import(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_IMPORT)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_IMPORT, &first);

    // Get import name
    OmniToken name_tok = peek_token(lexer, 0);
    if (name_tok.type == TOK_IDENTIFIER || name_tok.type == TOK_STRING) {
        get_token(lexer);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), name_tok.text);
    }

    // Optional 'from path'
    if (check_token(lexer, TOK_FROM)) {
        get_token(lexer);

        OmniToken path_tok = peek_token(lexer, 0);
        if (path_tok.type == TOK_IDENTIFIER || path_tok.type == TOK_STRING) {
            get_token(lexer);
            safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), path_tok.text);
        }
    }

    return true;
}

// Pattern: "spawn work name"
static bool try_spawn(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_SPAWN)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_SPAWN, &first);

    // Optional 'work'
    if (check_token(lexer, TOK_WORK)) {
        get_token(lexer);
    }

    // Get spawn target
    OmniToken target_tok = peek_token(lexer, 0);
    if (target_tok.type == TOK_IDENTIFIER) {
        get_token(lexer);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), target_tok.text);
    }

    return true;
}

// Pattern: "await expression"
static bool try_await(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_AWAIT)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_AWAIT, &first);

    // Collect await expression tokens
    char expr_buf[256] = "";
    while (!check_token(lexer, TOK_NEWLINE) && !check_token(lexer, TOK_EOF)) {
        OmniToken expr_tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = expr_tok;
        }
        if (expr_buf[0]) safe_strcat(expr_buf, sizeof(expr_buf), " ");
        safe_strcat(expr_buf, sizeof(expr_buf), expr_tok.text);
    }
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), expr_buf);

    return true;
}

// Pattern: "repeat until condition:"
static bool try_repeat_until(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_REPEAT)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_REPEAT_UNTIL, &first);

    // Expect 'until'
    if (check_token(lexer, TOK_UNTIL)) {
        get_token(lexer);
    }

    // Collect condition tokens until colon or newline
    char cond_buf[256] = "";
    while (!check_token(lexer, TOK_COLON) &&
           !check_token(lexer, TOK_NEWLINE) &&
           !check_token(lexer, TOK_EOF)) {
        OmniToken cond_tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = cond_tok;
        }
        if (cond_buf[0]) safe_strcat(cond_buf, sizeof(cond_buf), " ");
        safe_strcat(cond_buf, sizeof(cond_buf), cond_tok.text);
    }
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), cond_buf);

    // Consume trailing colon
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: "through each item in collection:"
static bool try_through_each(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_THROUGH)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_THROUGH_EACH, &first);

    // Optional 'each'
    if (peek_token(lexer, 0).type == TOK_IDENTIFIER &&
        strcasecmp(peek_token(lexer, 0).text, "each") == 0) {
        get_token(lexer);
    }

    // Get iterator variable name
    OmniToken var_tok = peek_token(lexer, 0);
    if (var_tok.type == TOK_IDENTIFIER) {
        get_token(lexer);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), var_tok.text);
    }

    // Expect 'in'
    if (check_token(lexer, TOK_IN)) {
        get_token(lexer);
    }

    // Collect collection expression until colon or newline
    char coll_buf[256] = "";
    while (!check_token(lexer, TOK_COLON) &&
           !check_token(lexer, TOK_NEWLINE) &&
           !check_token(lexer, TOK_EOF)) {
        OmniToken coll_tok = get_token(lexer);
        if (chunk->token_count < CHUNK_MAX_TOKENS) {
            chunk->tokens[chunk->token_count++] = coll_tok;
        }
        if (coll_buf[0]) safe_strcat(coll_buf, sizeof(coll_buf), " ");
        safe_strcat(coll_buf, sizeof(coll_buf), coll_tok.text);
    }
    safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), coll_buf);

    // Consume trailing colon
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: "from start to end:" or "from start to end by step:"
static bool try_from_to(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_FROM)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_FROM_TO, &first);

    // Get start value
    OmniToken start_tok = peek_token(lexer, 0);
    if (start_tok.type == TOK_NUMBER || start_tok.type == TOK_IDENTIFIER) {
        get_token(lexer);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), start_tok.text);
    }

    // Expect 'to'
    if (check_token(lexer, TOK_TO)) {
        get_token(lexer);
    }

    // Get end value
    OmniToken end_tok = peek_token(lexer, 0);
    if (end_tok.type == TOK_NUMBER || end_tok.type == TOK_IDENTIFIER) {
        get_token(lexer);
        safe_strcpy(chunk->secondary_value, sizeof(chunk->secondary_value), end_tok.text);
    }

    // Optional 'by step'
    if (check_token(lexer, TOK_BY)) {
        get_token(lexer);

        OmniToken step_tok = peek_token(lexer, 0);
        if (step_tok.type == TOK_NUMBER || step_tok.type == TOK_IDENTIFIER) {
            get_token(lexer);
            safe_strcpy(chunk->tertiary_value, sizeof(chunk->tertiary_value), step_tok.text);
        }
    }

    // Consume trailing colon
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: "create assembly name:"
static bool try_assembly_definition(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_CREATE)) return false;

    // Peek ahead for 'assembly'
    if (peek_token(lexer, 1).type != TOK_ASSEMBLY) return false;

    OmniToken first = get_token(lexer);  // consume 'create'
    get_token(lexer);                      // consume 'assembly'

    init_chunk(chunk, CHUNK_ASSEMBLY_DEF, &first);

    // Get assembly name
    OmniToken name_tok = peek_token(lexer, 0);
    if (name_tok.type == TOK_IDENTIFIER) {
        get_token(lexer);
        safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), name_tok.text);
    }

    // Consume trailing colon
    if (check_token(lexer, TOK_COLON)) {
        get_token(lexer);
    }

    return true;
}

// Pattern: Number literal
static bool try_number(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_NUMBER)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_NUMBER_VALUE, &first);
    safe_strcpy(chunk->primary_value, sizeof(chunk->primary_value), first.text);

    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// End Executable Code Patterns
// ═══════════════════════════════════════════════════════════════════════════

// Pattern: Newline
static bool try_newline(OmniChunkLexer* lexer, OmniChunk* chunk) {
    if (!check_token(lexer, TOK_NEWLINE)) return false;

    OmniToken first = get_token(lexer);
    init_chunk(chunk, CHUNK_NEWLINE, &first);

    return true;
}

// # B.3 Main Chunking Function [MAIN]

OmniChunk omni_chunk_next(OmniChunkLexer* lexer) {
    OmniChunk chunk;
    memset(&chunk, 0, sizeof(chunk));

    // Skip consecutive newlines (but keep one)
    while (peek_token(lexer, 0).type == TOK_NEWLINE &&
           peek_token(lexer, 1).type == TOK_NEWLINE) {
        get_token(lexer);
    }

    // DAR: ASSESS - Convert lexer errors to diagnostics
    // Check for TOK_ERROR from base lexer and convert to diagnostic
    if (check_token(lexer, TOK_ERROR)) {
        OmniToken err = get_token(lexer);

        // Extract error code from message (format: "L00X: message")
        const char* code = "L000";
        if (err.text[0] == 'L' && err.text[4] == ':') {
            // Error message starts with code
            static char code_buf[8];
            strncpy(code_buf, err.text, 4);
            code_buf[4] = '\0';
            code = code_buf;
        }

        // Emit diagnostic from lexer error
        emit_error(lexer, code, err.text,
                   "Lexical error detected during tokenization",
                   "none", err.line, err.column);

        // Return as raw token so parsing can continue
        init_chunk(&chunk, CHUNK_RAW_TOKEN, &err);
        safe_strcpy(chunk.primary_value, sizeof(chunk.primary_value), err.text);
        return chunk;
    }

    // EOF check
    if (check_token(lexer, TOK_EOF)) {
        OmniToken eof = get_token(lexer);
        init_chunk(&chunk, CHUNK_EOF, &eof);
        return chunk;
    }

    // Try patterns in priority order

    // Pragmas and meta (must be first - they're at file start)
    if (try_pragma(lexer, &chunk)) return chunk;
    if (try_meta_key(lexer, &chunk)) return chunk;
    if (try_meta_from(lexer, &chunk)) return chunk;
    if (try_meta_at(lexer, &chunk)) return chunk;

    // Block markers (bracket and sentence style)
    if (try_block_marker(lexer, &chunk)) return chunk;
    if (try_sentence_block(lexer, &chunk)) return chunk;

    // Entity declarations
    if (try_entity_declaration(lexer, &chunk)) return chunk;

    // Identity
    if (try_identity_key(lexer, &chunk)) return chunk;
    if (try_identity_name(lexer, &chunk)) return chunk;

    // Grounding
    if (try_grounding_header(lexer, &chunk)) return chunk;
    if (try_grounding_scripture(lexer, &chunk)) return chunk;
    if (try_grounding_principle(lexer, &chunk)) return chunk;
    if (try_grounding_anchor(lexer, &chunk)) return chunk;

    // Role
    if (try_role_header(lexer, &chunk)) return chunk;
    if (try_role(lexer, &chunk)) return chunk;
    if (try_purpose(lexer, &chunk)) return chunk;

    // Attribution
    if (try_attribution_header(lexer, &chunk)) return chunk;
    if (try_architect(lexer, &chunk)) return chunk;
    if (try_implementation(lexer, &chunk)) return chunk;
    if (try_created(lexer, &chunk)) return chunk;
    if (try_version(lexer, &chunk)) return chunk;

    // Purpose
    if (try_purpose_header(lexer, &chunk)) return chunk;

    // ═══════════════════════════════════════════════════════════════════
    // Executable Code Patterns
    // "Let there be light: and there was light" — Genesis 1:3
    // ═══════════════════════════════════════════════════════════════════

    // Variable declarations
    if (try_let_declaration(lexer, &chunk)) return chunk;

    // Function definitions (before control flow - 'establish' is a keyword)
    if (try_function_definition(lexer, &chunk)) return chunk;

    // Control flow
    if (try_when_condition(lexer, &chunk)) return chunk;
    if (try_otherwise(lexer, &chunk)) return chunk;
    if (try_repeat_until(lexer, &chunk)) return chunk;
    if (try_through_each(lexer, &chunk)) return chunk;
    if (try_from_to(lexer, &chunk)) return chunk;

    // Error handling
    if (try_attempt(lexer, &chunk)) return chunk;
    if (try_recover(lexer, &chunk)) return chunk;
    if (try_ensure(lexer, &chunk)) return chunk;
    if (try_fail(lexer, &chunk)) return chunk;

    // Return statement
    if (try_return_statement(lexer, &chunk)) return chunk;

    // Type definitions
    if (try_assembly_definition(lexer, &chunk)) return chunk;

    // Module system
    if (try_import(lexer, &chunk)) return chunk;

    // Concurrency
    if (try_spawn(lexer, &chunk)) return chunk;
    if (try_await(lexer, &chunk)) return chunk;

    // Witness/display statements
    if (try_witness(lexer, &chunk)) return chunk;
    if (try_display(lexer, &chunk)) return chunk;

    // CPI-SI state operations
    if (try_health(lexer, &chunk)) return chunk;
    if (try_state(lexer, &chunk)) return chunk;
    if (try_checkpoint(lexer, &chunk)) return chunk;
    if (try_restore(lexer, &chunk)) return chunk;
    if (try_intrinsic(lexer, &chunk)) return chunk;

    // Literals
    if (try_covenant_value(lexer, &chunk)) return chunk;
    if (try_truth_value(lexer, &chunk)) return chunk;
    if (try_none_value(lexer, &chunk)) return chunk;
    if (try_number(lexer, &chunk)) return chunk;

    // Type references
    if (try_type_reference(lexer, &chunk)) return chunk;

    // ═══════════════════════════════════════════════════════════════════
    // End Executable Code Patterns
    // ═══════════════════════════════════════════════════════════════════

    // Comments
    if (try_comment(lexer, &chunk)) return chunk;

    // String literals
    if (try_string(lexer, &chunk)) return chunk;

    // Newlines
    if (try_newline(lexer, &chunk)) return chunk;

    // Fallback: raw token
    OmniToken tok = get_token(lexer);
    init_chunk(&chunk, CHUNK_RAW_TOKEN, &tok);
    safe_strcpy(chunk.primary_value, sizeof(chunk.primary_value), tok.text);

    return chunk;
}

// # B.4 Public API [API]

void omni_chunk_init(OmniChunkLexer* lexer, const char* source, size_t length) {
    omni_lexer_init(&lexer->lexer, source, length);
    lexer->block = BLOCK_NONE;
    lexer->lookahead_count = 0;

    // Config integration (none for basic init)
    lexer->config = NULL;

    // DAR: Diagnostic tracking
    lexer->diagnostic_count = 0;
    lexer->had_error = false;
    lexer->had_warning = false;
}

// Initialize chunk lexer with config for validation.
// Config enables Scripture reference validation and Hebrew root lookup.
void omni_chunk_init_with_config(OmniChunkLexer* lexer, const char* source,
                                  size_t length, const Config* config) {
    omni_chunk_init(lexer, source, length);
    lexer->config = config;
}

// # B.5 Diagnostic API [DIAG_API]
//
// Access diagnostics collected during parsing (DAR: Assess phase).

// Get count of diagnostics collected.
int omni_chunk_diagnostic_count(const OmniChunkLexer* lexer) {
    return lexer ? lexer->diagnostic_count : 0;
}

// Get diagnostic at index. Returns NULL if out of bounds.
const OmniDiagnostic* omni_chunk_diagnostic_get(const OmniChunkLexer* lexer, int index) {
    if (!lexer || index < 0 || index >= lexer->diagnostic_count) {
        return NULL;
    }
    return &lexer->diagnostics[index];
}

// Check if any errors (severity -1) were detected.
bool omni_chunk_had_error(const OmniChunkLexer* lexer) {
    return lexer ? lexer->had_error : false;
}

// Check if any warnings (severity 0) were detected.
bool omni_chunk_had_warning(const OmniChunkLexer* lexer) {
    return lexer ? lexer->had_warning : false;
}

// Clear all diagnostics.
void omni_chunk_diagnostics_clear(OmniChunkLexer* lexer) {
    if (lexer) {
        lexer->diagnostic_count = 0;
        lexer->had_error = false;
        lexer->had_warning = false;
    }
}

// Get severity name for display.
const char* omni_diagnostic_severity_name(int severity) {
    if (severity < 0) return "error";
    if (severity == 0) return "warning";
    return "info";
}

// # B.6 Hebrew Root Lookup API [HEBREW_API]
//
// Look up Hebrew roots for OmniCode keywords.
// "In the beginning was the Word" — John 1:1

// Look up Hebrew root for an OmniCode keyword.
// Returns pointer to HebrewRootConfig or NULL if not found.
// Requires config to be loaded.
const HebrewRootConfig* omni_chunk_lookup_hebrew(
    const OmniChunkLexer* lexer,
    const char*           keyword)
{
    return lookup_hebrew_root(lexer, keyword);
}

// Check if keyword has Scripture grounding via Hebrew root.
bool omni_chunk_keyword_is_grounded(const OmniChunkLexer* lexer,
                                     const char* keyword) {
    return lookup_hebrew_root(lexer, keyword) != NULL;
}

// # B.7 Recovery Suggestions API [RECOVERY]
//
// DAR: RECOVER phase - get suggestions for fixing diagnostics.
// "A just man falleth seven times, and riseth up again." — Proverbs 24:16

// Get recovery suggestion text for a diagnostic.
// Returns human-readable suggestion or NULL if no suggestion available.
const char* omni_chunk_recovery_suggestion(const OmniDiagnostic* diag) {
    if (!diag || !diag->recovery[0]) return NULL;

    // Map recovery action names to suggestion text
    // These align with recovery.toml actions

    if (strcmp(diag->recovery, "pragma_template") == 0) {
        return "Add OmniCode pragma at file start:\n"
               "  #!omni <type> [options]\n"
               "  #!omni meta.key = <key>\n"
               "  #!omni meta.from = <template>\n"
               "  #!omni meta.at = a-01.00";
    }

    if (strcmp(diag->recovery, "block_template") == 0) {
        return "Add missing block with proper markers:\n"
               "  // =============================================================================\n"
               "  // BLOCK_NAME [BLOCK_NAME]\n"
               "  // =============================================================================\n"
               "  // Content here\n"
               "  // END BLOCK_NAME";
    }

    if (strcmp(diag->recovery, "identity_template") == 0) {
        return "Add identity section to METADATA:\n"
               "  // # M.1 Core Identity [IDENTITY]\n"
               "  //\n"
               "  // Key: <unique-key>\n"
               "  // Title: <title>\n"
               "  // Type: <type>\n"
               "  // Role: <role>";
    }

    if (strcmp(diag->recovery, "grounding_template") == 0) {
        return "Add grounding section to METADATA:\n"
               "  // # M.8 Grounding [GROUNDING]\n"
               "  //\n"
               "  // Scripture: <verse> from <Book> <Chapter>:<Verse>\n"
               "  // Principle: <principle>";
    }

    if (strcmp(diag->recovery, "key_template") == 0) {
        return "Add key declaration:\n"
               "  keyed as \"B-zone-path-name\"\n"
               "Format: B-<zone>-<path>-<name>";
    }

    if (strcmp(diag->recovery, "close_block") == 0) {
        return "Add block closing marker:\n"
               "  // END <BLOCK_NAME>";
    }

    if (strcmp(diag->recovery, "close_string") == 0) {
        return "Add closing quote to terminate string literal";
    }

    if (strcmp(diag->recovery, "reorder_suggestion") == 0) {
        return "Reorder blocks to follow required structure:\n"
               "  3-block: METADATA → CONTENT → CLOSING\n"
               "  4-block: METADATA → SETUP → BODY → CLOSING\n"
               "  5-block: METADATA → HEADER → CONTEXT → CONTENT → FOOTER";
    }

    if (strcmp(diag->recovery, "merge_suggestion") == 0) {
        return "Merge duplicate blocks, keeping content from both";
    }

    if (strcmp(diag->recovery, "none") == 0) {
        return NULL;  // No automatic recovery available
    }

    return NULL;  // Unknown recovery action
}

// Check if a diagnostic has a recovery suggestion available.
bool omni_chunk_has_recovery(const OmniDiagnostic* diag) {
    return omni_chunk_recovery_suggestion(diag) != NULL;
}

OmniChunk omni_chunk_peek(OmniChunkLexer* lexer) {
    // Save state
    OmniLexer saved_lexer = lexer->lexer;
    OmniBlockContext saved_block = lexer->block;
    OmniToken saved_lookahead[8];
    int saved_count = lexer->lookahead_count;
    memcpy(saved_lookahead, lexer->lookahead, sizeof(saved_lookahead));

    // Get next chunk
    OmniChunk chunk = omni_chunk_next(lexer);

    // Restore state
    lexer->lexer = saved_lexer;
    lexer->block = saved_block;
    lexer->lookahead_count = saved_count;
    memcpy(lexer->lookahead, saved_lookahead, sizeof(saved_lookahead));

    return chunk;
}

const char* omni_chunk_type_name(OmniChunkType type) {
    static const char* names[] = {
        [CHUNK_ENTITY_FOLDER] = "ENTITY_FOLDER",
        [CHUNK_ENTITY_FILE] = "ENTITY_FILE",
        [CHUNK_ENTITY_MODULE] = "ENTITY_MODULE",
        [CHUNK_IDENTITY_KEY] = "IDENTITY_KEY",
        [CHUNK_IDENTITY_NAME] = "IDENTITY_NAME",
        [CHUNK_IDENTITY_STRUCTURE] = "IDENTITY_STRUCTURE",
        [CHUNK_CLASSIFY_ZONE] = "CLASSIFY_ZONE",
        [CHUNK_CLASSIFY_REASON] = "CLASSIFY_REASON",
        [CHUNK_LINEAGE_DERIVES] = "LINEAGE_DERIVES",
        [CHUNK_LINEAGE_BRANCH] = "LINEAGE_BRANCH",
        [CHUNK_LINEAGE_AS] = "LINEAGE_AS",
        [CHUNK_GROUND_HEADER] = "GROUND_HEADER",
        [CHUNK_GROUND_SCRIPTURE] = "GROUND_SCRIPTURE",
        [CHUNK_GROUND_PRINCIPLE] = "GROUND_PRINCIPLE",
        [CHUNK_GROUND_ANCHOR] = "GROUND_ANCHOR",
        [CHUNK_SCRIPTURE_REF] = "SCRIPTURE_REF",
        [CHUNK_ROLE_HEADER] = "ROLE_HEADER",
        [CHUNK_ROLE_ROLE] = "ROLE_ROLE",
        [CHUNK_ROLE_PURPOSE] = "ROLE_PURPOSE",
        [CHUNK_ROLE_DESIGN] = "ROLE_DESIGN",
        [CHUNK_ROLE_PHILOSOPHY] = "ROLE_PHILOSOPHY",
        [CHUNK_ATTR_HEADER] = "ATTR_HEADER",
        [CHUNK_ATTR_ARCHITECT] = "ATTR_ARCHITECT",
        [CHUNK_ATTR_IMPLEMENTATION] = "ATTR_IMPLEMENTATION",
        [CHUNK_ATTR_CREATED] = "ATTR_CREATED",
        [CHUNK_ATTR_VERSION] = "ATTR_VERSION",
        [CHUNK_ATTR_MODIFIED] = "ATTR_MODIFIED",
        [CHUNK_PURPOSE_HEADER] = "PURPOSE_HEADER",
        [CHUNK_BLOCK_METADATA] = "BLOCK_METADATA",
        [CHUNK_BLOCK_HEADER] = "BLOCK_HEADER",
        [CHUNK_BLOCK_CONTEXT] = "BLOCK_CONTEXT",
        [CHUNK_BLOCK_SETUP] = "BLOCK_SETUP",
        [CHUNK_BLOCK_BODY] = "BLOCK_BODY",
        [CHUNK_BLOCK_CONTENT] = "BLOCK_CONTENT",
        [CHUNK_BLOCK_CLOSING] = "BLOCK_CLOSING",
        [CHUNK_BLOCK_FOOTER] = "BLOCK_FOOTER",
        [CHUNK_PRAGMA] = "PRAGMA",
        [CHUNK_META_KEY] = "META_KEY",
        [CHUNK_META_FROM] = "META_FROM",
        [CHUNK_META_AT] = "META_AT",
        [CHUNK_STRING_VALUE] = "STRING_VALUE",
        [CHUNK_NUMBER_VALUE] = "NUMBER_VALUE",
        [CHUNK_DATE_VALUE] = "DATE_VALUE",
        [CHUNK_VERSION_VALUE] = "VERSION_VALUE",

        // Executable Code - Variable Declarations
        [CHUNK_LET_DECL] = "LET_DECL",
        [CHUNK_LET_INIT] = "LET_INIT",
        [CHUNK_ASSIGN] = "ASSIGN",

        // Executable Code - Function Definitions
        [CHUNK_FUNC_DEF] = "FUNC_DEF",
        [CHUNK_FUNC_PROVIDE] = "FUNC_PROVIDE",
        [CHUNK_FUNC_PARAM] = "FUNC_PARAM",
        [CHUNK_FUNC_RETURN] = "FUNC_RETURN",
        [CHUNK_FUNC_CALL] = "FUNC_CALL",

        // Executable Code - Control Flow
        [CHUNK_WHEN_COND] = "WHEN_COND",
        [CHUNK_OTHERWISE] = "OTHERWISE",
        [CHUNK_REPEAT_UNTIL] = "REPEAT_UNTIL",
        [CHUNK_THROUGH_EACH] = "THROUGH_EACH",
        [CHUNK_FROM_TO] = "FROM_TO",

        // Executable Code - Error Handling
        [CHUNK_ATTEMPT] = "ATTEMPT",
        [CHUNK_RECOVER] = "RECOVER",
        [CHUNK_ENSURE] = "ENSURE",
        [CHUNK_FAIL_WITH] = "FAIL_WITH",

        // Executable Code - Module System
        [CHUNK_IMPORT] = "IMPORT",
        [CHUNK_EXPORT] = "EXPORT",

        // Executable Code - Concurrency
        [CHUNK_SPAWN] = "SPAWN",
        [CHUNK_AWAIT] = "AWAIT",

        // Executable Code - Expressions
        [CHUNK_BINARY_OP] = "BINARY_OP",
        [CHUNK_UNARY_OP] = "UNARY_OP",
        [CHUNK_MEMBER_ACCESS] = "MEMBER_ACCESS",
        [CHUNK_INDEX_ACCESS] = "INDEX_ACCESS",

        // Executable Code - Type Declarations
        [CHUNK_ASSEMBLY_DEF] = "ASSEMBLY_DEF",
        [CHUNK_FIELD_DEF] = "FIELD_DEF",

        // Executable Code - Witness (Logging)
        [CHUNK_WITNESS] = "WITNESS",
        [CHUNK_DISPLAY] = "DISPLAY",

        // Executable Code - Literals
        [CHUNK_COVENANT_VALUE] = "COVENANT_VALUE",
        [CHUNK_TRUTH_VALUE] = "TRUTH_VALUE",
        [CHUNK_NONE_VALUE] = "NONE_VALUE",

        // Executable Code - Type References
        [CHUNK_TYPE_REF] = "TYPE_REF",

        // Structural
        [CHUNK_NEWLINE] = "NEWLINE",
        [CHUNK_COMMENT] = "COMMENT",
        [CHUNK_RAW_TOKEN] = "RAW_TOKEN",
        [CHUNK_EOF] = "EOF",
    };

    if (type >= 0 && type < CHUNK_COUNT_TYPES) {
        return names[type] ? names[type] : "UNKNOWN";
    }
    return "INVALID";
}

const char* omni_block_context_name(OmniBlockContext ctx) {
    static const char* names[] = {
        [BLOCK_NONE] = "NONE",
        [BLOCK_METADATA] = "METADATA",
        [BLOCK_HEADER] = "HEADER",
        [BLOCK_CONTEXT] = "CONTEXT",
        [BLOCK_SETUP] = "SETUP",
        [BLOCK_BODY] = "BODY",
        [BLOCK_CONTENT] = "CONTENT",
        [BLOCK_CLOSING] = "CLOSING",
        [BLOCK_FOOTER] = "FOOTER",
    };
    return names[ctx];
}

bool omni_chunk_is_declaration(OmniChunkType type) {
    return type == CHUNK_ENTITY_FOLDER ||
           type == CHUNK_ENTITY_FILE ||
           type == CHUNK_ENTITY_MODULE;
}

bool omni_chunk_is_grounding(OmniChunkType type) {
    return type == CHUNK_GROUND_HEADER ||
           type == CHUNK_GROUND_SCRIPTURE ||
           type == CHUNK_GROUND_PRINCIPLE ||
           type == CHUNK_GROUND_ANCHOR;
}

bool omni_chunk_is_block(OmniChunkType type) {
    return type >= CHUNK_BLOCK_METADATA && type <= CHUNK_BLOCK_FOOTER;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Note [NOTE]
//
// "In the beginning was the Word"
//   — John 1:1
//
// Words carry meaning. The chunk lexer understands OmniCode as language.
// Semantic chunks make parsing trivial - the hard work is already done.

// =============================================================================
// END CLOSING
// =============================================================================
