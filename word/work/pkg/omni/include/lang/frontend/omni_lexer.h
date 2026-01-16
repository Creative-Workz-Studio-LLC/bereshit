// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-lexer
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_lexer.h — OmniCode Lexer/Tokenizer
// Tokenizes OmniCode source into a stream of tokens

#ifndef BERESHIT_OMNI_LEXER_H
#define BERESHIT_OMNI_LEXER_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-lexer
// Title: OmniCode Lexer
// Type: Header
// Component: OmniCode Parser
// Role: Tokenizes OmniCode source into typed tokens

// # M.2 Lifecycle State [STATE]
//
// Status: Active
// Version: a-01.00
// Created: 2026-01-13
// Updated: 2026-01-13

// # M.3 Attribution [ATTRIBUTION]
//
// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
// Copyright: (c) 2026 CreativeWorkzStudio LLC. All rights reserved.

// # M.7 Intent [INTENT]
//
// Purpose: Tokenize OmniCode source files into a stream of typed tokens
// Core Design: Single-pass lexer with lookahead, returns tokens on demand
// Philosophy: Faithful to OmniCode syntax specification

// # M.8 Grounding [GROUNDING]
//
// Scripture: Isaiah 28:10 - Precept upon precept, line upon line
// Principle: Break source into fundamental units before understanding structure
// Anchor: Genesis 1:4 - God divided the light from the darkness (separation)

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// # S.1 Includes [INCLUDES]

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// # S.2 Defines [DEFINES]

#define OMNI_TOKEN_MAX_LEN 1024    // Max token text length
#define OMNI_ERROR_MAX_LEN 256    // Max error message length

// # S.3 Types [TYPES]

// # S.3a Token Types [TOKEN_TYPES]

typedef enum {
    // Special
    TOK_EOF = 0,
    TOK_ERROR,
    TOK_NEWLINE,
    TOK_INDENT,

    // Pragma
    TOK_PRAGMA,           // #!omni
    TOK_META_KEY,         // meta.key
    TOK_META_FROM,        // meta.from
    TOK_META_AT,          // meta.at

    // Declaration Keywords
    TOK_LET,              // let
    TOK_ESTABLISH,        // establish
    TOK_CREATE,           // create

    // Function Keywords
    TOK_WORK,             // work
    TOK_TAKES,            // takes
    TOK_RETURNS,          // returns
    TOK_THAT,             // that
    TOK_PROVIDE,          // provide

    // Control Flow Keywords
    TOK_WHEN,             // when
    TOK_OTHERWISE,        // otherwise
    TOK_REPEAT,           // repeat
    TOK_UNTIL,            // until
    TOK_THROUGH,          // through
    TOK_FROM,             // from
    TOK_TO,               // to
    TOK_BY,               // by

    // Memory Keywords
    TOK_OWN,              // own
    TOK_SHARE,            // share
    TOK_REF,              // ref
    TOK_STEWARD,          // steward

    // Error Handling Keywords
    TOK_ATTEMPT,          // attempt
    TOK_RECOVER,          // recover
    TOK_ENSURE,           // ensure
    TOK_FAIL,             // fail

    // Module Keywords
    TOK_MODULE,           // module
    TOK_IMPORT,           // import
    TOK_EXPORT,           // export

    // Concurrency Keywords
    TOK_ASYNC,            // async
    TOK_AWAIT,            // await
    TOK_SPAWN,            // spawn

    // Logical Keywords
    TOK_AND,              // and
    TOK_OR,               // or
    TOK_NOT,              // not
    TOK_IS,               // is
    TOK_AS,               // as

    // Truth Values
    TOK_YES,              // yes
    TOK_NO,               // no
    TOK_GRANTED,          // granted
    TOK_DEFERRED,         // deferred
    TOK_DENIED,           // denied
    TOK_NONE,             // none

    // Structural Keywords (OmniCode metadata)
    TOK_THIS,             // this
    TOK_FOR,              // for
    TOK_KEYED,            // keyed
    TOK_FOLLOWING,        // following
    TOK_CLASSIFIES,       // classifies
    TOK_DERIVES,          // derives
    TOK_GROUNDED,         // grounded
    TOK_IN,               // in
    TOK_SERVES,           // serves
    TOK_AUTHORED,         // authored
    TOK_EXISTS,           // exists
    TOK_INTERFACE,        // interface
    TOK_DEPENDS,          // depends
    TOK_INTEGRATED,       // integrated
    TOK_OPERATIONAL,      // operational
    TOK_BLOCKING,         // blocking
    TOK_HEALTH,           // health
    TOK_SCRIPTURE,        // scripture
    TOK_PRINCIPLE,        // principle
    TOK_ANCHOR,           // anchor
    TOK_MEANING,          // meaning
    TOK_TEACHING,         // teaching
    TOK_CAUTIONING,       // cautioning
    TOK_BECAUSE,          // because
    TOK_BE,               // be
    TOK_OF,               // of
    TOK_WITH,             // with
    TOK_AT,               // at
    TOK_ON,               // on
    TOK_ABOVE,            // above
    TOK_BELOW,            // below
    TOK_WITHIN,           // within
    TOK_REQUIRES,         // requires
    TOK_INCLUDES,         // includes
    TOK_EXCLUDES,         // excludes
    TOK_USED,             // used
    TOK_INTEGRATES,       // integrates
    TOK_NAVIGATION,       // navigation
    TOK_EXITS,            // exits
    TOK_SYSTEM,           // system
    TOK_SCALE,            // scale
    TOK_OPERATIONS,       // operations
    TOK_SCORES,           // scores
    TOK_CASCADE,          // cascade
    TOK_MULTIPLIES,       // multiplies

    // Additional Structural Keywords (discovered from existing .omni files)
    TOK_FOLDER,           // folder
    TOK_FILE,             // file
    TOK_NAMED,            // named
    TOK_CORE,             // core
    TOK_IDENTITY,         // identity
    TOK_ROLE,             // role
    TOK_ARCHITECT,        // architect
    TOK_IMPLEMENTATION,   // implementation
    TOK_CREATED,          // created
    TOK_VERSION,          // version
    TOK_PURPOSE,          // purpose
    TOK_DEFINITION,       // definition
    TOK_VALUES,           // values
    TOK_PRIMARY,          // primary
    TOK_SYNTHESIS,        // synthesis
    TOK_QUICK,            // quick
    TOK_REFERENCE,        // reference
    TOK_NAVIGATE,         // navigate
    TOK_BRANCH,           // branch
    TOK_PARADIGM,         // paradigm
    TOK_DESIGN,           // design
    TOK_PHILOSOPHY,       // philosophy
    TOK_FEATURES,         // features
    TOK_MODIFIED,         // modified
    TOK_SCHEMA,           // schema
    TOK_FIELDS,           // fields
    TOK_CONSTRAINTS,      // constraints
    TOK_ENSURING,         // ensuring
    TOK_RESERVED,         // reserved
    TOK_DEFAULTING,       // defaulting
    TOK_STRUCTURE,        // structure
    TOK_CONTENTS,         // contents
    TOK_CHILDREN,         // children
    TOK_TABLES,           // tables
    TOK_VERIFIED,         // verified
    TOK_STATIC,           // static
    TOK_PRODUCING,        // producing
    TOK_RUNTIME,          // runtime
    TOK_SUCCEEDING,       // succeeding
    TOK_RETURNING,        // returning
    TOK_DETECTED,         // detected
    TOK_ACCESSED,         // accessed
    TOK_ACCESS,           // access
    // TOK_RETURNS already in Function Keywords
    TOK_POLICY,           // policy
    TOK_MODIFICATION,     // modification
    TOK_SAFE,             // safe
    TOK_CAREFUL,          // careful
    TOK_FUTURE,           // future
    TOK_PLANNED,          // planned
    TOK_CONSIDERED,       // considered
    TOK_REJECTED,         // rejected
    TOK_OVERVIEW,         // overview
    TOK_SEE,              // see
    TOK_CLOSING,          // closing
    TOK_NOTE,             // note
    TOK_IMPACT,           // impact
    TOK_VIEW,             // view
    TOK_QUERY,            // query
    TOK_CHECK,            // check
    TOK_A,                // a (article in "is a folder")
    TOK_CONTEXT,          // context
    TOK_MITIGATION,       // mitigation
    TOK_MULTIPLIERS,      // multipliers
    TOK_TERNARY,          // ternary
    TOK_INTEGRITY,        // integrity
    TOK_NEVER,            // never
    TOK_DEPENDENCIES,     // dependencies

    // Zone/Role Keywords (Capitalized - proper nouns in sentence syntax)
    // OmniCode is sentence-based: "serves as Word" vs "be word" (type)
    TOK_ZONE_VOID,        // Void (zone name, proper noun)
    TOK_ZONE_WORD,        // Word (zone name, proper noun)
    TOK_ZONE_TOV,         // Tov (zone name, proper noun)
    TOK_ROLE_MEMORY,      // Memory (role name)

    // Type Keywords
    TOK_COUNT,            // count (int)
    TOK_WHOLE,            // whole (uint)
    TOK_MEASURE,          // measure (float)
    TOK_WORD,             // word (string)
    TOK_CHAR,             // char
    TOK_SCROLL,           // scroll (long text)
    TOK_TRUTH,            // truth (bool)
    TOK_COVENANT,         // covenant (ternary)
    TOK_GATHERING,        // gathering (array/list)
    TOK_ASSEMBLY,         // assembly (struct)
    TOK_OPTIONAL,         // optional
    TOK_OWNED,            // owned
    TOK_SHARED,           // shared

    // Block Markers
    TOK_BLOCK_METADATA,   // METADATA
    TOK_BLOCK_SETUP,      // SETUP
    TOK_BLOCK_BODY,       // BODY
    TOK_BLOCK_CLOSING,    // CLOSING
    TOK_BLOCK_HEADER,     // HEADER
    TOK_BLOCK_CONTEXT,    // CONTEXT
    TOK_BLOCK_CONTENT,    // CONTENT
    TOK_BLOCK_FOOTER,     // FOOTER

    // Menu DSL Keywords (OmniCode → JSONC transpilation)
    TOK_MENU,             // menu
    TOK_ITEM,             // item
    TOK_ACTION,           // action
    TOK_HOTKEY,           // hotkey
    TOK_VISIBLE,          // visible
    TOK_ENABLED,          // enabled
    TOK_SELECTED,         // selected
    TOK_WRAP,             // wrap
    TOK_DEFAULT,          // default
    TOK_REMEMBER,         // remember
    TOK_POSITION,         // position
    TOK_PARENT,           // parent
    TOK_UP,               // up
    TOK_DOWN,             // down
    TOK_LEFT,             // left
    TOK_RIGHT,            // right
    TOK_SELECT,           // select
    TOK_BACK,             // back
    TOK_CANCEL,           // cancel
    TOK_DISPLAY,          // display
    TOK_HINTS,            // hints
    TOK_WIDTH,            // width
    TOK_SHOW,             // show
    TOK_BORDER,           // border
    TOK_CURSOR,           // cursor
    TOK_COLORS,           // colors
    TOK_BOX,              // box
    TOK_STYLE,            // style
    TOK_LAYOUT,           // layout
    TOK_ANIMATION,        // animation
    TOK_TOGGLE,           // toggle
    TOK_ADJUST,           // adjust
    TOK_EXECUTE,          // execute
    TOK_HIDE,             // hide
    TOK_SIMPLIFY,         // simplify
    TOK_REDUCE,           // reduce
    TOK_ENABLE,           // enable
    TOK_TRIPWIRE,         // tripwire
    TOK_BEHAVIOR,         // behavior
    TOK_FAILURE,          // failure
    TOK_DEGRADE,          // degrade
    TOK_RESTORE,          // restore
    TOK_VALID,            // valid
    TOK_TYPED,            // typed
    TOK_QUIT,             // quit
    TOK_STATE,            // state
    TOK_RULES,            // rules
    TOK_ITEMS,            // items
    TOK_OPTIONS,          // options
    TOK_ALL,              // all
    TOK_RECOVERY,         // recovery
    TOK_LAST,             // last

    // Operators
    TOK_COLON,            // :
    TOK_EQUAL,            // =
    TOK_LPAREN,           // (
    TOK_RPAREN,           // )
    TOK_LBRACKET,         // [
    TOK_RBRACKET,         // ]
    TOK_LBRACE,           // {
    TOK_RBRACE,           // }
    TOK_COMMA,            // ,
    TOK_DOT,              // .
    TOK_PLUS,             // +
    TOK_MINUS,            // -
    TOK_STAR,             // *
    TOK_SLASH,            // /
    TOK_ARROW,            // ->
    TOK_PLUS_MINUS,       // ± (U+00B1)
    TOK_INFINITY,         // ∞ (U+221E)

    // Literals
    TOK_STRING,           // "..."
    TOK_NUMBER,           // 123, -123, 3.14
    TOK_IDENTIFIER,       // user-defined names

    // Comments
    TOK_COMMENT,          // // ...
    TOK_BLOCK_COMMENT,    // /* ... */

    TOK_COUNT_TYPES       // Total token types
} OmniTokenType;

// # S.3b Token Structure [TOKEN]

typedef struct {
    OmniTokenType type;
    char          text[OMNI_TOKEN_MAX_LEN];
    int           line;
    int           column;
    int           indent;     // Indentation level (spaces/4)
} OmniToken;

// # S.3c Lexer State [LEXER]
//
// Forward declaration needed for error struct

struct OmniLexerError;

typedef struct {
    const char*   source;     // Source text
    size_t        length;     // Source length
    size_t        pos;        // Current position
    int           line;       // Current line (1-based)
    int           column;     // Current column (1-based)
    int           indent;     // Current indentation level
    bool          at_line_start; // Are we at line start?
    char          error[OMNI_ERROR_MAX_LEN];

    // DAR: DETECT phase - error collection
    // Errors are collected without stopping parsing.
    // See OmniLexerError struct below for format.
    int           error_count;   // Number of errors detected
    bool          had_error;     // Any error detected?
} OmniLexer;

// # S.3d Error Codes [ERRORS]
//
// Error codes aligned with errors.toml specifications.
// Format: L### for lexical errors (DETECT phase).

typedef enum {
    OMNI_LEX_OK = 0,
    OMNI_LEX_ERR_NULL,
    OMNI_LEX_ERR_UNTERMINATED_STRING,
    OMNI_LEX_ERR_INVALID_CHAR,
    OMNI_LEX_ERR_OVERFLOW,
    OMNI_LEX_ERR_INVALID_ESCAPE,
    OMNI_LEX_ERR_INVALID_NUMBER,
} OmniLexError;

// # S.3e Lexer Error Entry [LEX_ERROR]
//
// DAR Pattern: DETECT phase - collect errors without stopping.
// Each entry captures what was detected for ASSESS phase.

#define OMNI_LEXER_MAX_ERRORS 16

typedef struct {
    char    code[8];           // Error code (L001, L002, etc.)
    char    message[128];      // Short message
    char    context[64];       // What was encountered (char, escape, etc.)
    int     line;              // Source line
    int     column;            // Source column
} OmniLexerError;

// # S.4 Function Prototypes [PROTOTYPES]

// # S.4a Lifecycle [LIFECYCLE]

// omni_lexer_init initializes lexer with source text.
void omni_lexer_init(OmniLexer* lexer, const char* source, size_t length);

// # S.4b Operations [OPERATIONS]

// omni_lexer_next returns the next token from source.
OmniToken omni_lexer_next(OmniLexer* lexer);

// omni_lexer_peek looks at next token without consuming.
OmniToken omni_lexer_peek(OmniLexer* lexer);

// # S.4c Utilities [UTILITIES]

// omni_token_type_name returns string name for token type.
const char* omni_token_type_name(OmniTokenType type);

// omni_lexer_get_error returns last error message.
const char* omni_lexer_get_error(const OmniLexer* lexer);

// # S.4d Error Tracking [ERROR_TRACK]
//
// DAR: DETECT phase functions.
// Get count and details of detected errors.

// omni_lexer_error_count returns number of errors detected.
int omni_lexer_error_count(const OmniLexer* lexer);

// omni_lexer_had_error returns true if any error was detected.
bool omni_lexer_had_error(const OmniLexer* lexer);

// omni_lexer_clear_errors resets error state.
void omni_lexer_clear_errors(OmniLexer* lexer);

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Usage Pattern [USAGE]
//
// OmniLexer lexer;
// omni_lexer_init(&lexer, source, strlen(source));
//
// OmniToken tok;
// while ((tok = omni_lexer_next(&lexer)).type != TOK_EOF) {
//     printf("%s: %s\n", omni_token_type_name(tok.type), tok.text);
// }

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Validation [VALIDATION]
//
// gcc -fsyntax-only -Wall -Wextra omni_lexer.h

// # X.2 Closing Note [NOTE]
//
// "Precept upon precept; line upon line"
//   — Isaiah 28:10
//
// The lexer breaks source into fundamental units - the first step of understanding.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_LEXER_H
