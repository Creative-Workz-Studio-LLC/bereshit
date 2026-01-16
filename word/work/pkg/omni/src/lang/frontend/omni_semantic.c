// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-semantic-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_semantic.c — OmniCode Semantic Parser Implementation
// Parses OmniCode as natural language, extracting meaning

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-semantic-impl
// Title: OmniCode Semantic Parser Implementation
// Type: Source
// Role: Implement semantic understanding of OmniCode

// # M.8 Grounding [GROUNDING]
//
// Scripture: John 1:1 - In the beginning was the Word
// Principle: OmniCode IS English - parse it semantically

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "omni_semantic.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Helper Functions [HELPERS]

static void advance(OmniSemanticParser* parser) {
    parser->previous = parser->current;
    parser->current = omni_lexer_next(&parser->lexer);
}

static bool check(OmniSemanticParser* parser, OmniTokenType type) {
    return parser->current.type == type;
}

static bool match(OmniSemanticParser* parser, OmniTokenType type) {
    if (!check(parser, type)) return false;
    advance(parser);
    return true;
}

static void skip_newlines(OmniSemanticParser* parser) {
    while (check(parser, TOK_NEWLINE)) {
        advance(parser);
    }
}

static void skip_to_newline(OmniSemanticParser* parser) {
    while (!check(parser, TOK_NEWLINE) && !check(parser, TOK_EOF)) {
        advance(parser);
    }
}

// Safe string copy - always null terminates, never overflows
static void safe_strcpy(char* dest, size_t dest_size, const char* src) {
    if (dest_size == 0) return;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
    memcpy(dest, src, copy_len);
    dest[copy_len] = '\0';
}

// Safe string append - always null terminates, never overflows
static void safe_strcat(char* dest, size_t dest_size, const char* src) {
    if (dest_size == 0) return;
    size_t dest_len = strlen(dest);
    if (dest_len >= dest_size - 1) return;  // No room
    size_t remaining = dest_size - dest_len - 1;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < remaining) ? src_len : remaining;
    memcpy(dest + dest_len, src, copy_len);
    dest[dest_len + copy_len] = '\0';
}

// Collect string value (inside quotes)
static void collect_string(OmniSemanticParser* parser, char* buf, size_t max) {
    buf[0] = '\0';
    if (check(parser, TOK_STRING)) {
        safe_strcpy(buf, max, parser->current.text);
        advance(parser);
    }
}

// # B.2 Sentence Pattern Recognition [PATTERNS]
//
// OmniCode sentences follow natural English patterns.
// As a CPI-SI, I recognize these as meaningful utterances.

// Pattern: "this is a folder for X"
static bool parse_entity_declaration(OmniSemanticParser* parser) {
    if (!match(parser, TOK_THIS)) return false;
    if (!match(parser, TOK_IS)) return false;
    if (!match(parser, TOK_A)) return false;

    // Entity type: folder, file, module
    if (check(parser, TOK_FOLDER)) {
        strcpy(parser->doc->entity_type, "folder");
        advance(parser);
    } else {
        strcpy(parser->doc->entity_type, parser->current.text);
        advance(parser);
    }

    // "for"
    if (match(parser, TOK_FOR)) {
        // Entity name in quotes
        collect_string(parser, parser->doc->entity_name, sizeof(parser->doc->entity_name));
    }

    return true;
}

// Pattern: "keyed as X"
static bool parse_identity_key(OmniSemanticParser* parser) {
    if (!match(parser, TOK_KEYED)) return false;
    if (!match(parser, TOK_AS)) return false;

    collect_string(parser, parser->doc->entity_key, sizeof(parser->doc->entity_key));
    return true;
}

// Pattern: "named X"
static bool parse_identity_name(OmniSemanticParser* parser) {
    if (!match(parser, TOK_NAMED)) return false;

    collect_string(parser, parser->doc->entity_name, sizeof(parser->doc->entity_name));
    return true;
}

// Pattern: "classifies as: zone (n) because X"
static bool parse_classification(OmniSemanticParser* parser) {
    if (!check(parser, TOK_FOLDER)) return false;
    advance(parser);
    if (!match(parser, TOK_CLASSIFIES)) return false;
    if (!match(parser, TOK_AS)) return false;
    match(parser, TOK_COLON);  // Optional colon

    skip_newlines(parser);

    // Zone name: void, word, tov or Void, Word, Tov
    if (check(parser, TOK_ZONE_VOID) || check(parser, TOK_IDENTIFIER)) {
        if (strcmp(parser->current.text, "void") == 0 || parser->current.type == TOK_ZONE_VOID) {
            strcpy(parser->doc->zone, "void");
            parser->doc->zone_value = -1;
        } else if (strcmp(parser->current.text, "word") == 0 || parser->current.type == TOK_ZONE_WORD) {
            strcpy(parser->doc->zone, "word");
            parser->doc->zone_value = 0;
        } else if (strcmp(parser->current.text, "tov") == 0 || parser->current.type == TOK_ZONE_TOV) {
            strcpy(parser->doc->zone, "tov");
            parser->doc->zone_value = 1;
        } else {
            strcpy(parser->doc->zone, parser->current.text);
        }
        advance(parser);
    }

    // Optional (n) value
    if (match(parser, TOK_LPAREN)) {
        if (check(parser, TOK_NUMBER)) {
            parser->doc->zone_value = atoi(parser->current.text);
            advance(parser);
        }
        match(parser, TOK_RPAREN);
    }

    // "because" clause
    if (match(parser, TOK_BECAUSE)) {
        collect_string(parser, parser->doc->zone_reason, sizeof(parser->doc->zone_reason));
    }

    return true;
}

// Pattern: "derives from: branch (n) from X as Y"
static bool parse_lineage(OmniSemanticParser* parser) {
    if (!check(parser, TOK_FOLDER)) return false;
    advance(parser);
    if (!match(parser, TOK_DERIVES)) return false;
    if (!match(parser, TOK_FROM)) return false;
    match(parser, TOK_COLON);

    skip_newlines(parser);

    // "branch (n) from X as Y"
    if (match(parser, TOK_BRANCH)) {
        // Skip (n)
        if (match(parser, TOK_LPAREN)) {
            advance(parser);  // number
            match(parser, TOK_RPAREN);
        }

        if (match(parser, TOK_FROM)) {
            collect_string(parser, parser->doc->derives_from, sizeof(parser->doc->derives_from));
        }

        if (match(parser, TOK_AS)) {
            collect_string(parser, parser->doc->derives_as, sizeof(parser->doc->derives_as));
        }
    }

    return true;
}

// Pattern: "grounded in: scripture X from Y in Z"
static bool parse_grounding(OmniSemanticParser* parser) {
    if (!match(parser, TOK_GROUNDED)) return false;
    if (!match(parser, TOK_IN)) return false;
    match(parser, TOK_COLON);

    skip_newlines(parser);

    // Create grounding node
    OmniSemanticNode* node = calloc(1, sizeof(OmniSemanticNode));
    if (!node) return false;

    while (!check(parser, TOK_EOF)) {
        skip_newlines(parser);

        if (match(parser, TOK_SCRIPTURE)) {
            node->type = SEM_GROUND_SCRIPTURE;
            collect_string(parser, node->object, sizeof(node->object));

            // "from Book Chapter:Verse in Version"
            if (match(parser, TOK_FROM)) {
                // Book name (identifier)
                if (check(parser, TOK_IDENTIFIER)) {
                    safe_strcpy(node->scripture_book, sizeof(node->scripture_book), parser->current.text);
                    advance(parser);
                }
                // Chapter:Verse
                char ref[64] = "";
                while (!check(parser, TOK_IN) && !check(parser, TOK_NEWLINE) && !check(parser, TOK_EOF)) {
                    safe_strcat(ref, sizeof(ref), parser->current.text);
                    advance(parser);
                }
                safe_strcpy(node->scripture_verse, sizeof(node->scripture_verse), ref);

                // "in KJV"
                if (match(parser, TOK_IN)) {
                    // Skip version
                    advance(parser);
                }

                // Build full reference
                snprintf(node->scripture_ref, sizeof(node->scripture_ref),
                         "%s %s", node->scripture_book, node->scripture_verse);
            }
        } else if (match(parser, TOK_PRINCIPLE)) {
            node->type = SEM_GROUND_PRINCIPLE;
            collect_string(parser, node->predicate, sizeof(node->predicate));
        } else if (match(parser, TOK_ANCHOR)) {
            node->type = SEM_GROUND_ANCHOR;
            collect_string(parser, node->predicate, sizeof(node->predicate));
        } else {
            break;  // End of grounding section
        }
    }

    parser->doc->grounding = node;
    return true;
}

// Pattern: "serves as Word: role X"
static bool parse_serves(OmniSemanticParser* parser) {
    if (!match(parser, TOK_SERVES)) return false;
    if (!match(parser, TOK_AS)) return false;

    OmniSemanticNode* node = calloc(1, sizeof(OmniSemanticNode));
    if (!node) return false;
    node->type = SEM_ROLE_SERVES;

    // Zone name (Word, Memory, etc.)
    if (check(parser, TOK_ZONE_WORD) || check(parser, TOK_ZONE_VOID) ||
        check(parser, TOK_ZONE_TOV) || check(parser, TOK_ROLE_MEMORY) ||
        check(parser, TOK_IDENTIFIER)) {
        safe_strcpy(node->subject, sizeof(node->subject), parser->current.text);
        advance(parser);
    }

    match(parser, TOK_COLON);
    skip_newlines(parser);

    // "role X"
    if (match(parser, TOK_ROLE)) {
        collect_string(parser, node->object, sizeof(node->object));
    }

    parser->doc->serves = node;
    return true;
}

// Pattern: "authored by: architect X, implementation Y"
static bool parse_attribution(OmniSemanticParser* parser) {
    if (!match(parser, TOK_AUTHORED)) return false;
    if (!match(parser, TOK_BY)) return false;
    match(parser, TOK_COLON);

    skip_newlines(parser);

    while (!check(parser, TOK_EOF)) {
        skip_newlines(parser);

        if (match(parser, TOK_ARCHITECT)) {
            collect_string(parser, parser->doc->architect, sizeof(parser->doc->architect));
        } else if (match(parser, TOK_IMPLEMENTATION)) {
            collect_string(parser, parser->doc->implementation, sizeof(parser->doc->implementation));
        } else if (match(parser, TOK_CREATED)) {
            // Date
            if (check(parser, TOK_NUMBER) || check(parser, TOK_IDENTIFIER)) {
                safe_strcpy(parser->doc->created, sizeof(parser->doc->created), parser->current.text);
                advance(parser);
                // Get full date
                while (check(parser, TOK_MINUS) || check(parser, TOK_NUMBER)) {
                    safe_strcat(parser->doc->created, sizeof(parser->doc->created), parser->current.text);
                    advance(parser);
                }
            }
        } else if (match(parser, TOK_VERSION)) {
            if (check(parser, TOK_IDENTIFIER) || check(parser, TOK_NUMBER)) {
                safe_strcpy(parser->doc->version, sizeof(parser->doc->version), parser->current.text);
                advance(parser);
                // Get full version (a-01.00)
                while (check(parser, TOK_MINUS) || check(parser, TOK_DOT) || check(parser, TOK_NUMBER)) {
                    safe_strcat(parser->doc->version, sizeof(parser->doc->version), parser->current.text);
                    advance(parser);
                }
            }
        } else {
            break;
        }
    }

    return true;
}

// Pattern: "exists to: purpose X"
static bool parse_purpose(OmniSemanticParser* parser) {
    if (!match(parser, TOK_EXISTS)) return false;
    if (!match(parser, TOK_TO)) return false;
    match(parser, TOK_COLON);

    skip_newlines(parser);

    OmniSemanticNode* node = calloc(1, sizeof(OmniSemanticNode));
    if (!node) return false;

    while (!check(parser, TOK_EOF)) {
        skip_newlines(parser);

        if (match(parser, TOK_PURPOSE)) {
            node->type = SEM_ROLE_PURPOSE;
            collect_string(parser, node->object, sizeof(node->object));
        } else if (match(parser, TOK_DESIGN)) {
            node->type = SEM_ROLE_DESIGN;
            collect_string(parser, node->predicate, sizeof(node->predicate));
        } else if (match(parser, TOK_PHILOSOPHY)) {
            node->type = SEM_ROLE_PHILOSOPHY;
            collect_string(parser, node->predicate, sizeof(node->predicate));
        } else {
            break;
        }
    }

    parser->doc->purpose = node;
    return true;
}

// # B.3 Main Parse Loop [PARSE]

static void parse_document(OmniSemanticParser* parser) {
    advance(parser);  // Get first token

    // Skip pragma
    while (check(parser, TOK_PRAGMA) || check(parser, TOK_META_KEY) ||
           check(parser, TOK_META_FROM) || check(parser, TOK_META_AT) ||
           check(parser, TOK_NEWLINE) || check(parser, TOK_COMMENT)) {
        advance(parser);
    }

    while (!check(parser, TOK_EOF)) {
        skip_newlines(parser);

        // Try each sentence pattern - as CPI-SI, I understand these naturally
        if (check(parser, TOK_THIS)) {
            parse_entity_declaration(parser);
        } else if (check(parser, TOK_KEYED)) {
            parse_identity_key(parser);
        } else if (check(parser, TOK_NAMED)) {
            parse_identity_name(parser);
        } else if (check(parser, TOK_FOLDER) || check(parser, TOK_THIS)) {
            // Could be classification or lineage
            OmniToken saved = parser->current;
            advance(parser);
            if (check(parser, TOK_CLASSIFIES)) {
                parser->current = saved;
                parse_classification(parser);
            } else if (check(parser, TOK_DERIVES)) {
                parser->current = saved;
                parse_lineage(parser);
            } else {
                // Restore and continue
                parser->current = saved;
                advance(parser);
            }
        } else if (check(parser, TOK_CORE)) {
            // "core identity of this folder"
            advance(parser);
            skip_to_newline(parser);
        } else if (check(parser, TOK_GROUNDED)) {
            parse_grounding(parser);
        } else if (check(parser, TOK_SERVES)) {
            parse_serves(parser);
        } else if (check(parser, TOK_AUTHORED)) {
            parse_attribution(parser);
        } else if (check(parser, TOK_EXISTS)) {
            parse_purpose(parser);
        } else if (check(parser, TOK_BLOCK_METADATA) || check(parser, TOK_BLOCK_CONTENT) ||
                   check(parser, TOK_BLOCK_CLOSING)) {
            // Block markers - skip for now
            advance(parser);
        } else if (check(parser, TOK_COMMENT)) {
            advance(parser);
        } else {
            // Skip unrecognized tokens
            advance(parser);
        }
    }
}

// # B.4 Public API [API]

void omni_semantic_init(OmniSemanticParser* parser, const char* source, size_t length) {
    omni_lexer_init(&parser->lexer, source, length);
    parser->had_error = false;
    parser->panic_mode = false;
    parser->doc = NULL;
}

OmniSemanticDoc* omni_semantic_parse(OmniSemanticParser* parser) {
    parser->doc = calloc(1, sizeof(OmniSemanticDoc));
    if (!parser->doc) return NULL;

    parse_document(parser);

    return parser->doc;
}

void omni_semantic_free(OmniSemanticDoc* doc) {
    if (!doc) return;

    // Free grounding nodes
    if (doc->grounding) free(doc->grounding);
    if (doc->serves) free(doc->serves);
    if (doc->purpose) free(doc->purpose);

    // TODO: Free full tree when implemented

    free(doc);
}

// # B.5 Semantic Queries [QUERIES]

const char* omni_semantic_entity_type(OmniSemanticDoc* doc) {
    return doc ? doc->entity_type : "";
}

const char* omni_semantic_entity_name(OmniSemanticDoc* doc) {
    return doc ? doc->entity_name : "";
}

const char* omni_semantic_entity_key(OmniSemanticDoc* doc) {
    return doc ? doc->entity_key : "";
}

const char* omni_semantic_zone(OmniSemanticDoc* doc) {
    return doc ? doc->zone : "";
}

int omni_semantic_zone_value(OmniSemanticDoc* doc) {
    return doc ? doc->zone_value : 0;
}

const char* omni_semantic_scripture(OmniSemanticDoc* doc) {
    if (doc && doc->grounding && doc->grounding->type == SEM_GROUND_SCRIPTURE) {
        return doc->grounding->scripture_ref;
    }
    return "";
}

const char* omni_semantic_principle(OmniSemanticDoc* doc) {
    if (doc && doc->grounding && doc->grounding->type == SEM_GROUND_PRINCIPLE) {
        return doc->grounding->predicate;
    }
    return "";
}

const char* omni_semantic_anchor(OmniSemanticDoc* doc) {
    if (doc && doc->grounding && doc->grounding->type == SEM_GROUND_ANCHOR) {
        return doc->grounding->predicate;
    }
    return "";
}

const char* omni_semantic_role(OmniSemanticDoc* doc) {
    if (doc && doc->serves) {
        return doc->serves->object;
    }
    return "";
}

const char* omni_semantic_purpose(OmniSemanticDoc* doc) {
    if (doc && doc->purpose) {
        return doc->purpose->object;
    }
    return "";
}

const char* omni_semantic_type_name(OmniSemanticType type) {
    static const char* names[] = {
        [SEM_ENTITY_FOLDER] = "ENTITY_FOLDER",
        [SEM_ENTITY_FILE] = "ENTITY_FILE",
        [SEM_ENTITY_MODULE] = "ENTITY_MODULE",
        [SEM_IDENTITY_KEY] = "IDENTITY_KEY",
        [SEM_IDENTITY_NAME] = "IDENTITY_NAME",
        [SEM_IDENTITY_STRUCTURE] = "IDENTITY_STRUCTURE",
        [SEM_CLASSIFY_ZONE] = "CLASSIFY_ZONE",
        [SEM_LINEAGE_DERIVES] = "LINEAGE_DERIVES",
        [SEM_LINEAGE_BRANCH] = "LINEAGE_BRANCH",
        [SEM_GROUND_SCRIPTURE] = "GROUND_SCRIPTURE",
        [SEM_GROUND_PRINCIPLE] = "GROUND_PRINCIPLE",
        [SEM_GROUND_ANCHOR] = "GROUND_ANCHOR",
        [SEM_ROLE_SERVES] = "ROLE_SERVES",
        [SEM_ROLE_PURPOSE] = "ROLE_PURPOSE",
        [SEM_ROLE_DESIGN] = "ROLE_DESIGN",
        [SEM_ROLE_PHILOSOPHY] = "ROLE_PHILOSOPHY",
        [SEM_ATTR_ARCHITECT] = "ATTR_ARCHITECT",
        [SEM_ATTR_IMPLEMENTATION] = "ATTR_IMPLEMENTATION",
        [SEM_ATTR_CREATED] = "ATTR_CREATED",
        [SEM_ATTR_VERSION] = "ATTR_VERSION",
        [SEM_ATTR_MODIFIED] = "ATTR_MODIFIED",
        [SEM_UNKNOWN] = "UNKNOWN",
    };

    if (type >= 0 && type < SEM_COUNT_TYPES) {
        return names[type] ? names[type] : "UNKNOWN";
    }
    return "INVALID";
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
// OmniCode IS English. As CPI-SI, I understand it naturally.
// The semantic parser extracts meaning, not just syntax.

// =============================================================================
// END CLOSING
// =============================================================================
