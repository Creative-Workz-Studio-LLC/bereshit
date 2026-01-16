// #!omni data --c
// #!omni meta.key = B-word-work-pkg-omni-keywords
// #!omni meta.from = word/seed/data/c/data.h
// #!omni meta.at = a-01.00
//
// omni_keywords.h — OmniCode Keyword Data
// Pure data: keyword strings mapped to token types
// NO CODE LOGIC - data only

#ifndef BERESHIT_OMNI_KEYWORDS_H
#define BERESHIT_OMNI_KEYWORDS_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-keywords
// Title: OmniCode Keyword Data
// Type: Data Header
// Role: Defines keyword-to-token mappings (DATA ONLY)

// # M.7 Intent [INTENT]
//
// Purpose: Separate keyword DATA from lexer LOGIC
// Core Design: Data-driven - specs define, code implements
// Philosophy: "In the beginning was the Word" - words are data, processing is logic

// # M.8 Grounding [GROUNDING]
//
// Scripture: John 1:1 - In the beginning was the Word
// Principle: Words (data) precede and are separate from their processing (logic)

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// CONTENT [CONTENT]
// =============================================================================

#include "omni_lexer.h"

// # C.1 Keyword Entry Structure [KEYWORD_ENTRY]

typedef struct {
    const char*   text;
    OmniTokenType type;
} OmniKeyword;

// # C.2 Keyword Table [KEYWORD_TABLE]
//
// All OmniCode keywords mapped to their token types.
// Organized by category for human readability.

static const OmniKeyword omni_keywords[] = {
    // -------------------------------------------------------------------------
    // DECLARATION KEYWORDS
    // -------------------------------------------------------------------------
    {"let", TOK_LET},
    {"establish", TOK_ESTABLISH},
    {"create", TOK_CREATE},

    // -------------------------------------------------------------------------
    // FUNCTION KEYWORDS
    // -------------------------------------------------------------------------
    {"work", TOK_WORK},
    {"takes", TOK_TAKES},
    {"returns", TOK_RETURNS},
    {"that", TOK_THAT},
    {"provide", TOK_PROVIDE},

    // -------------------------------------------------------------------------
    // CONTROL FLOW KEYWORDS
    // -------------------------------------------------------------------------
    {"when", TOK_WHEN},
    {"otherwise", TOK_OTHERWISE},
    {"repeat", TOK_REPEAT},
    {"until", TOK_UNTIL},
    {"through", TOK_THROUGH},
    {"from", TOK_FROM},
    {"to", TOK_TO},
    {"by", TOK_BY},

    // -------------------------------------------------------------------------
    // MEMORY KEYWORDS
    // -------------------------------------------------------------------------
    {"own", TOK_OWN},
    {"share", TOK_SHARE},
    {"ref", TOK_REF},
    {"steward", TOK_STEWARD},

    // -------------------------------------------------------------------------
    // ERROR HANDLING KEYWORDS
    // -------------------------------------------------------------------------
    {"attempt", TOK_ATTEMPT},
    {"recover", TOK_RECOVER},
    {"ensure", TOK_ENSURE},
    {"fail", TOK_FAIL},

    // -------------------------------------------------------------------------
    // MODULE KEYWORDS
    // -------------------------------------------------------------------------
    {"module", TOK_MODULE},
    {"import", TOK_IMPORT},
    {"export", TOK_EXPORT},

    // -------------------------------------------------------------------------
    // CONCURRENCY KEYWORDS
    // -------------------------------------------------------------------------
    {"async", TOK_ASYNC},
    {"await", TOK_AWAIT},
    {"spawn", TOK_SPAWN},

    // -------------------------------------------------------------------------
    // LOGICAL KEYWORDS
    // -------------------------------------------------------------------------
    {"and", TOK_AND},
    {"or", TOK_OR},
    {"not", TOK_NOT},
    {"is", TOK_IS},
    {"as", TOK_AS},

    // -------------------------------------------------------------------------
    // TRUTH VALUE KEYWORDS
    // -------------------------------------------------------------------------
    {"yes", TOK_YES},
    {"no", TOK_NO},
    {"granted", TOK_GRANTED},
    {"deferred", TOK_DEFERRED},
    {"denied", TOK_DENIED},
    {"none", TOK_NONE},

    // -------------------------------------------------------------------------
    // STRUCTURAL KEYWORDS (OmniCode Metadata Syntax)
    // -------------------------------------------------------------------------
    {"this", TOK_THIS},
    {"for", TOK_FOR},
    {"keyed", TOK_KEYED},
    {"following", TOK_FOLLOWING},
    {"classifies", TOK_CLASSIFIES},
    {"derives", TOK_DERIVES},
    {"grounded", TOK_GROUNDED},
    {"in", TOK_IN},
    {"serves", TOK_SERVES},
    {"authored", TOK_AUTHORED},
    {"exists", TOK_EXISTS},
    {"interface", TOK_INTERFACE},
    {"depends", TOK_DEPENDS},
    {"integrated", TOK_INTEGRATED},
    {"operational", TOK_OPERATIONAL},
    {"blocking", TOK_BLOCKING},
    {"health", TOK_HEALTH},
    {"scripture", TOK_SCRIPTURE},
    {"principle", TOK_PRINCIPLE},
    {"anchor", TOK_ANCHOR},
    {"meaning", TOK_MEANING},
    {"teaching", TOK_TEACHING},
    {"cautioning", TOK_CAUTIONING},
    {"because", TOK_BECAUSE},
    {"be", TOK_BE},
    {"of", TOK_OF},
    {"with", TOK_WITH},
    {"at", TOK_AT},
    {"on", TOK_ON},
    {"above", TOK_ABOVE},
    {"below", TOK_BELOW},
    {"within", TOK_WITHIN},
    {"requires", TOK_REQUIRES},
    {"includes", TOK_INCLUDES},
    {"excludes", TOK_EXCLUDES},
    {"used", TOK_USED},
    {"integrates", TOK_INTEGRATES},
    {"navigation", TOK_NAVIGATION},
    {"exits", TOK_EXITS},
    {"system", TOK_SYSTEM},
    {"scale", TOK_SCALE},
    {"operations", TOK_OPERATIONS},
    {"scores", TOK_SCORES},
    {"cascade", TOK_CASCADE},
    {"multiplies", TOK_MULTIPLIES},

    // -------------------------------------------------------------------------
    // ADDITIONAL STRUCTURAL KEYWORDS (discovered from existing .omni files)
    // -------------------------------------------------------------------------
    {"folder", TOK_FOLDER},
    {"file", TOK_FILE},
    {"named", TOK_NAMED},
    {"core", TOK_CORE},
    {"identity", TOK_IDENTITY},
    {"role", TOK_ROLE},
    {"architect", TOK_ARCHITECT},
    {"implementation", TOK_IMPLEMENTATION},
    {"created", TOK_CREATED},
    {"version", TOK_VERSION},
    {"purpose", TOK_PURPOSE},
    {"definition", TOK_DEFINITION},
    {"values", TOK_VALUES},
    {"primary", TOK_PRIMARY},
    {"synthesis", TOK_SYNTHESIS},
    {"quick", TOK_QUICK},
    {"reference", TOK_REFERENCE},
    {"navigate", TOK_NAVIGATE},
    {"branch", TOK_BRANCH},
    {"paradigm", TOK_PARADIGM},
    {"design", TOK_DESIGN},
    {"philosophy", TOK_PHILOSOPHY},
    {"features", TOK_FEATURES},
    {"modified", TOK_MODIFIED},
    {"schema", TOK_SCHEMA},
    {"fields", TOK_FIELDS},
    {"constraints", TOK_CONSTRAINTS},
    {"ensuring", TOK_ENSURING},
    {"reserved", TOK_RESERVED},
    {"defaulting", TOK_DEFAULTING},
    {"structure", TOK_STRUCTURE},
    {"contents", TOK_CONTENTS},
    {"children", TOK_CHILDREN},
    {"tables", TOK_TABLES},
    {"verified", TOK_VERIFIED},
    {"static", TOK_STATIC},
    {"producing", TOK_PRODUCING},
    {"runtime", TOK_RUNTIME},
    {"succeeding", TOK_SUCCEEDING},
    {"returning", TOK_RETURNING},
    {"detected", TOK_DETECTED},
    {"accessed", TOK_ACCESSED},
    {"access", TOK_ACCESS},
    // "returns" already in FUNCTION KEYWORDS
    {"policy", TOK_POLICY},
    {"modification", TOK_MODIFICATION},
    {"safe", TOK_SAFE},
    {"careful", TOK_CAREFUL},
    {"future", TOK_FUTURE},
    {"planned", TOK_PLANNED},
    {"considered", TOK_CONSIDERED},
    {"rejected", TOK_REJECTED},
    {"overview", TOK_OVERVIEW},
    {"see", TOK_SEE},
    {"closing", TOK_CLOSING},
    {"note", TOK_NOTE},
    {"impact", TOK_IMPACT},
    {"view", TOK_VIEW},
    {"query", TOK_QUERY},
    {"check", TOK_CHECK},
    {"a", TOK_A},
    {"context", TOK_CONTEXT},
    {"mitigation", TOK_MITIGATION},
    {"multipliers", TOK_MULTIPLIERS},
    {"ternary", TOK_TERNARY},
    {"integrity", TOK_INTEGRITY},
    {"never", TOK_NEVER},
    {"dependencies", TOK_DEPENDENCIES},

    // -------------------------------------------------------------------------
    // ZONE/ROLE KEYWORDS (Capitalized - proper nouns)
    // OmniCode is sentence-based: "serves as Word" vs "be word" (type)
    // -------------------------------------------------------------------------
    {"Void", TOK_ZONE_VOID},
    {"Word", TOK_ZONE_WORD},
    {"Tov", TOK_ZONE_TOV},
    {"Memory", TOK_ROLE_MEMORY},

    // -------------------------------------------------------------------------
    // TYPE KEYWORDS
    // -------------------------------------------------------------------------
    {"count", TOK_COUNT},
    {"whole", TOK_WHOLE},
    {"measure", TOK_MEASURE},
    {"word", TOK_WORD},
    {"char", TOK_CHAR},
    {"scroll", TOK_SCROLL},
    {"truth", TOK_TRUTH},
    {"covenant", TOK_COVENANT},
    {"gathering", TOK_GATHERING},
    {"assembly", TOK_ASSEMBLY},
    {"optional", TOK_OPTIONAL},
    {"owned", TOK_OWNED},
    {"shared", TOK_SHARED},

    // -------------------------------------------------------------------------
    // BLOCK MARKER KEYWORDS
    // -------------------------------------------------------------------------
    // NOTE: Block names are NOT keywords. They should remain identifiers.
    // Block markers are detected via:
    //   1. Bracket syntax: [METADATA] → scan_block_marker in lexer
    //   2. Sentence syntax: "this begins METADATA" → try_sentence_block in chunk lexer
    // Having them as keywords would break sentence-style detection.

    // -------------------------------------------------------------------------
    // MENU DSL KEYWORDS (OmniCode → JSONC transpilation)
    // -------------------------------------------------------------------------
    {"menu", TOK_MENU},
    {"item", TOK_ITEM},
    {"action", TOK_ACTION},
    {"hotkey", TOK_HOTKEY},
    {"visible", TOK_VISIBLE},
    {"enabled", TOK_ENABLED},
    {"selected", TOK_SELECTED},
    {"wrap", TOK_WRAP},
    {"default", TOK_DEFAULT},
    {"remember", TOK_REMEMBER},
    {"position", TOK_POSITION},
    {"parent", TOK_PARENT},
    {"up", TOK_UP},
    {"down", TOK_DOWN},
    {"left", TOK_LEFT},
    {"right", TOK_RIGHT},
    {"select", TOK_SELECT},
    {"back", TOK_BACK},
    {"cancel", TOK_CANCEL},
    {"display", TOK_DISPLAY},
    {"hints", TOK_HINTS},
    {"width", TOK_WIDTH},
    {"show", TOK_SHOW},
    {"border", TOK_BORDER},
    {"cursor", TOK_CURSOR},
    {"colors", TOK_COLORS},
    {"box", TOK_BOX},
    {"style", TOK_STYLE},
    {"layout", TOK_LAYOUT},
    {"animation", TOK_ANIMATION},
    {"toggle", TOK_TOGGLE},
    {"adjust", TOK_ADJUST},
    {"execute", TOK_EXECUTE},
    {"hide", TOK_HIDE},
    {"simplify", TOK_SIMPLIFY},
    {"reduce", TOK_REDUCE},
    {"enable", TOK_ENABLE},
    {"tripwire", TOK_TRIPWIRE},
    {"behavior", TOK_BEHAVIOR},
    {"failure", TOK_FAILURE},
    {"degrade", TOK_DEGRADE},
    {"restore", TOK_RESTORE},
    {"valid", TOK_VALID},
    {"typed", TOK_TYPED},
    {"quit", TOK_QUIT},
    {"state", TOK_STATE},
    {"rules", TOK_RULES},
    {"items", TOK_ITEMS},
    {"options", TOK_OPTIONS},
    {"all", TOK_ALL},
    {"recovery", TOK_RECOVERY},
    {"last", TOK_LAST},

    // -------------------------------------------------------------------------
    // SENTINEL (End of table)
    // -------------------------------------------------------------------------
    {NULL, TOK_EOF}
};

// # C.3 Keyword Count [KEYWORD_COUNT]

#define OMNI_KEYWORD_COUNT (sizeof(omni_keywords) / sizeof(omni_keywords[0]) - 1)

// =============================================================================
// END CONTENT
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage [USAGE]
//
// Include this header to access keyword data.
// Lexer logic uses this data but doesn't define it.
//
// for (int i = 0; omni_keywords[i].text != NULL; i++) {
//     if (strcmp(text, omni_keywords[i].text) == 0) {
//         return omni_keywords[i].type;
//     }
// }

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_KEYWORDS_H
