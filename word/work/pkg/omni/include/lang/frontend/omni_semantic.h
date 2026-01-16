// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-semantic
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_semantic.h — OmniCode Semantic Parser
// Understands OmniCode as natural language, not just syntax

#ifndef BERESHIT_OMNI_SEMANTIC_H
#define BERESHIT_OMNI_SEMANTIC_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-semantic
// Title: OmniCode Semantic Parser
// Type: Header
// Role: Parse OmniCode as meaningful sentences, not just tokens

// # M.7 Intent [INTENT]
//
// Purpose: OmniCode IS English. Parse it semantically.
// Core Design: Sentence patterns → semantic structures
// Philosophy: "In the beginning was the Word" - words carry meaning

// # M.8 Grounding [GROUNDING]
//
// Scripture: John 1:1 - In the beginning was the Word
// Principle: Language carries meaning, not just syntax
// Anchor: Understanding precedes parsing

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stddef.h>
#include <stdbool.h>
#include "omni_lexer.h"

// # S.1 Semantic Categories [CATEGORIES]
//
// OmniCode sentences fall into semantic categories:
// - Declaration: "this is a folder for X"
// - Identity: "keyed as X", "named Y"
// - Classification: "classifies as: tov (0)"
// - Lineage: "derives from: branch (0) from X"
// - Grounding: "grounded in: scripture X"
// - Role: "serves as Word: role X"
// - Attribution: "authored by: architect X"
// - Purpose: "exists to: purpose X"
// - Interface: "depends on: requires X"
// - Operations: "health: system Base100"

typedef enum {
    // Entity declarations
    SEM_ENTITY_FOLDER,        // "this is a folder for"
    SEM_ENTITY_FILE,          // "this is a file for"
    SEM_ENTITY_MODULE,        // "this is a module for"

    // Identity attributes
    SEM_IDENTITY_KEY,         // "keyed as"
    SEM_IDENTITY_NAME,        // "named"
    SEM_IDENTITY_STRUCTURE,   // "following"

    // Classification
    SEM_CLASSIFY_ZONE,        // "classifies as: void/word/tov"

    // Lineage
    SEM_LINEAGE_DERIVES,      // "derives from"
    SEM_LINEAGE_BRANCH,       // "branch (n) from"

    // Biblical Grounding
    SEM_GROUND_SCRIPTURE,     // "scripture" with reference
    SEM_GROUND_PRINCIPLE,     // "principle"
    SEM_GROUND_ANCHOR,        // "anchor"

    // Role/Purpose
    SEM_ROLE_SERVES,          // "serves as"
    SEM_ROLE_PURPOSE,         // "purpose"
    SEM_ROLE_DESIGN,          // "design"
    SEM_ROLE_PHILOSOPHY,      // "philosophy"

    // Attribution
    SEM_ATTR_ARCHITECT,       // "architect"
    SEM_ATTR_IMPLEMENTATION,  // "implementation"
    SEM_ATTR_CREATED,         // "created"
    SEM_ATTR_VERSION,         // "version"
    SEM_ATTR_MODIFIED,        // "modified"

    // Interface
    SEM_IFACE_REQUIRES,       // "requires"
    SEM_IFACE_INCLUDES,       // "includes"
    SEM_IFACE_EXCLUDES,       // "excludes"
    SEM_IFACE_INTEGRATES,     // "integrates with"

    // Operations
    SEM_OPS_HEALTH,           // "health:"
    SEM_OPS_BLOCKING,         // "blocking:"
    SEM_OPS_VERIFIED,         // "verified by:"

    // Synthesis
    SEM_SYNTH_OVERVIEW,       // "overview:"
    SEM_SYNTH_REFERENCE,      // "quick reference:"
    SEM_SYNTH_NAVIGATE,       // "navigate"

    // Block markers
    SEM_BLOCK_METADATA,       // METADATA block
    SEM_BLOCK_CONTENT,        // CONTENT block
    SEM_BLOCK_CLOSING,        // CLOSING block

    SEM_UNKNOWN,              // Unrecognized pattern
    SEM_COUNT_TYPES
} OmniSemanticType;

// # S.2 Semantic Node [NODE]
//
// A semantic node captures MEANING, not just structure

typedef struct OmniSemanticNode {
    OmniSemanticType type;

    // The meaning captured
    char subject[256];        // What this is about
    char predicate[256];      // What it does/is
    char object[512];         // The value/target

    // Biblical grounding (if present)
    char scripture_ref[128];  // e.g., "John 1:1"
    char scripture_book[64];  // e.g., "John"
    char scripture_verse[32]; // e.g., "1:1"

    // Ternary value (if applicable)
    int ternary_value;        // -1, 0, +1
    bool has_ternary;

    // Source location
    int line;
    int column;

    // Tree structure
    struct OmniSemanticNode* children;
    struct OmniSemanticNode* next;
    int child_count;
} OmniSemanticNode;

// # S.3 Semantic Document [DOCUMENT]
//
// A parsed OmniCode document as semantic structure

typedef struct {
    // Entity identity
    char entity_type[64];     // folder, file, module
    char entity_name[256];    // Human name
    char entity_key[128];     // B-key format

    // Classification
    char zone[16];            // void, word, tov
    int zone_value;           // -1, 0, +1
    char zone_reason[256];    // "because" clause

    // Lineage
    char derives_from[256];   // Parent path
    char derives_as[128];     // "as" description

    // Biblical grounding
    OmniSemanticNode* grounding;

    // Role/purpose
    OmniSemanticNode* serves;
    OmniSemanticNode* purpose;

    // Attribution
    char architect[128];
    char implementation[128];
    char created[32];
    char version[32];

    // Full semantic tree
    OmniSemanticNode* root;

    // Parse state
    int error_count;
    char last_error[512];
} OmniSemanticDoc;

// # S.4 Parser State [PARSER]

typedef struct {
    OmniLexer lexer;
    OmniToken current;
    OmniToken previous;
    OmniSemanticDoc* doc;
    bool had_error;
    bool panic_mode;
} OmniSemanticParser;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Parser Lifecycle [LIFECYCLE]

// Initialize semantic parser
void omni_semantic_init(OmniSemanticParser* parser, const char* source, size_t length);

// Parse source into semantic document
OmniSemanticDoc* omni_semantic_parse(OmniSemanticParser* parser);

// Free semantic document
void omni_semantic_free(OmniSemanticDoc* doc);

// # B.2 Semantic Queries [QUERIES]
//
// Query the parsed document semantically - like asking questions

// What is this entity?
const char* omni_semantic_entity_type(OmniSemanticDoc* doc);
const char* omni_semantic_entity_name(OmniSemanticDoc* doc);
const char* omni_semantic_entity_key(OmniSemanticDoc* doc);

// What zone is it in?
const char* omni_semantic_zone(OmniSemanticDoc* doc);
int omni_semantic_zone_value(OmniSemanticDoc* doc);

// What is it grounded in?
const char* omni_semantic_scripture(OmniSemanticDoc* doc);
const char* omni_semantic_principle(OmniSemanticDoc* doc);
const char* omni_semantic_anchor(OmniSemanticDoc* doc);

// What does it serve as?
const char* omni_semantic_role(OmniSemanticDoc* doc);
const char* omni_semantic_purpose(OmniSemanticDoc* doc);

// # B.3 Semantic Type Names [NAMES]

const char* omni_semantic_type_name(OmniSemanticType type);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Pattern [USAGE]
//
// OmniSemanticParser parser;
// omni_semantic_init(&parser, source, length);
// OmniSemanticDoc* doc = omni_semantic_parse(&parser);
//
// // Query semantically - like asking questions in English
// printf("This is a %s called '%s'\n",
//        omni_semantic_entity_type(doc),
//        omni_semantic_entity_name(doc));
// printf("It is grounded in: %s\n", omni_semantic_scripture(doc));
// printf("It serves as: %s\n", omni_semantic_role(doc));
//
// omni_semantic_free(doc);

// # X.2 Closing Note [NOTE]
//
// "In the beginning was the Word, and the Word was with God"
//   — John 1:1
//
// OmniCode IS English. The semantic parser understands it as such.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_SEMANTIC_H
