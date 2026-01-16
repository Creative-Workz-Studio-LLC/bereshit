// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-chunks
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_chunks.h — OmniCode Semantic Chunks
// Words carry meaning. The lexer should understand, not just tokenize.

#ifndef BERESHIT_OMNI_CHUNKS_H
#define BERESHIT_OMNI_CHUNKS_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-chunks
// Title: OmniCode Semantic Chunks
// Type: Header
// Role: Define meaningful language units, not just tokens

// # M.8 Grounding [GROUNDING]
//
// Scripture: John 1:1 - In the beginning was the Word
// Principle: Words carry inherent meaning - lexer should understand
// Anchor: OmniCode IS English - chunk as humans chunk language

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stddef.h>
#include <stdbool.h>
#include "omni_lexer.h"
#include "config.h"  // For Bible validation and error config

// # S.1 Chunk Categories [CATEGORIES]
//
// Semantic chunks represent meaningful units of OmniCode.
// These map to how humans naturally parse English sentences.

typedef enum {
    // ─────────────────────────────────────────────────────────────────────────
    // Declaration chunks - "this is a X for Y"
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_ENTITY_FOLDER,          // "this is a folder for X"
    CHUNK_ENTITY_FILE,            // "this is a file for X"
    CHUNK_ENTITY_MODULE,          // "this is a module for X"

    // ─────────────────────────────────────────────────────────────────────────
    // Identity chunks - who/what this is
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_IDENTITY_KEY,           // "keyed as X"
    CHUNK_IDENTITY_NAME,          // "named X"
    CHUNK_IDENTITY_STRUCTURE,     // "following X-block"

    // ─────────────────────────────────────────────────────────────────────────
    // Classification chunks - where this belongs
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_CLASSIFY_ZONE,          // "classifies as: zone (n)"
    CHUNK_CLASSIFY_REASON,        // "because X"

    // ─────────────────────────────────────────────────────────────────────────
    // Lineage chunks - where this comes from
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_LINEAGE_DERIVES,        // "derives from: X"
    CHUNK_LINEAGE_BRANCH,         // "branch (n) from X"
    CHUNK_LINEAGE_AS,             // "as X"

    // ─────────────────────────────────────────────────────────────────────────
    // Grounding chunks - biblical foundation
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_GROUND_HEADER,          // "grounded in:"
    CHUNK_GROUND_SCRIPTURE,       // "scripture X from Book C:V"
    CHUNK_GROUND_PRINCIPLE,       // "principle X"
    CHUNK_GROUND_ANCHOR,          // "anchor X"

    // ─────────────────────────────────────────────────────────────────────────
    // Scripture reference - parsed biblical citation
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_SCRIPTURE_REF,          // "Genesis 1:1" or "John 1:1"

    // ─────────────────────────────────────────────────────────────────────────
    // Role chunks - what this serves as
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_ROLE_HEADER,            // "serves as Zone:"
    CHUNK_ROLE_ROLE,              // "role X"
    CHUNK_ROLE_PURPOSE,           // "purpose X"
    CHUNK_ROLE_DESIGN,            // "design X"
    CHUNK_ROLE_PHILOSOPHY,        // "philosophy X"

    // ─────────────────────────────────────────────────────────────────────────
    // Attribution chunks - who created this
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_ATTR_HEADER,            // "authored by:"
    CHUNK_ATTR_ARCHITECT,         // "architect X"
    CHUNK_ATTR_IMPLEMENTATION,    // "implementation X"
    CHUNK_ATTR_CREATED,           // "created YYYY-MM-DD"
    CHUNK_ATTR_VERSION,           // "version X"
    CHUNK_ATTR_MODIFIED,          // "modified YYYY-MM-DD"

    // ─────────────────────────────────────────────────────────────────────────
    // Purpose chunks - why this exists
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_PURPOSE_HEADER,         // "exists to:"

    // ─────────────────────────────────────────────────────────────────────────
    // Block markers - structural boundaries
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_BLOCK_METADATA,         // [METADATA] or METADATA block
    CHUNK_BLOCK_HEADER,           // [HEADER] (documentation)
    CHUNK_BLOCK_CONTEXT,          // [CONTEXT] (documentation)
    CHUNK_BLOCK_SETUP,            // [SETUP] (code)
    CHUNK_BLOCK_BODY,             // [BODY] (code)
    CHUNK_BLOCK_CONTENT,          // [CONTENT] (data/documentation)
    CHUNK_BLOCK_CLOSING,          // [CLOSING] or CLOSING block
    CHUNK_BLOCK_FOOTER,           // [FOOTER] (documentation)

    // ─────────────────────────────────────────────────────────────────────────
    // Pragma - file header directives
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_PRAGMA,                 // "#!omni document --folder"
    CHUNK_META_KEY,               // "#!omni meta.key = X"
    CHUNK_META_FROM,              // "#!omni meta.from = X"
    CHUNK_META_AT,                // "#!omni meta.at = X"

    // ─────────────────────────────────────────────────────────────────────────
    // Content chunks - values and literals
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_STRING_VALUE,           // "quoted string"
    CHUNK_NUMBER_VALUE,           // numeric literal
    CHUNK_DATE_VALUE,             // YYYY-MM-DD
    CHUNK_VERSION_VALUE,          // a-01.00

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Variable Declarations
    // "let X be Y" — OmniCode variable declaration
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_LET_DECL,               // "let name be type"
    CHUNK_LET_INIT,               // "let name be type with value"
    CHUNK_ASSIGN,                 // "set name to value"

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Function Definitions
    // "establish work X that takes Y returns Z" — OmniCode function
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_FUNC_DEF,               // "establish work X ..."
    CHUNK_FUNC_PROVIDE,           // "provide work X ..." (public)
    CHUNK_FUNC_PARAM,             // Parameter in function signature
    CHUNK_FUNC_RETURN,            // "return X"
    CHUNK_FUNC_CALL,              // "process with X" or "call X with Y"

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Control Flow
    // "when X is Y:" — OmniCode conditional
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_WHEN_COND,              // "when condition:"
    CHUNK_OTHERWISE,              // "otherwise:"
    CHUNK_REPEAT_UNTIL,           // "repeat until condition:"
    CHUNK_THROUGH_EACH,           // "through each X in Y:"
    CHUNK_FROM_TO,                // "from X to Y:"

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Error Handling
    // "attempt:" / "recover:" / "ensure:" — OmniCode error handling
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_ATTEMPT,                // "attempt:"
    CHUNK_RECOVER,                // "recover:"
    CHUNK_ENSURE,                 // "ensure:"
    CHUNK_FAIL_WITH,              // "fail with X"

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Module System
    // "import X from Y" — OmniCode module imports
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_IMPORT,                 // "import X from Y"
    CHUNK_EXPORT,                 // "export X"

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Concurrency
    // "spawn work X" — OmniCode concurrency
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_SPAWN,                  // "spawn work X"
    CHUNK_AWAIT,                  // "await X"

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Expressions
    // OmniCode expression chunks
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_BINARY_OP,              // "X plus Y", "X is equal to Y"
    CHUNK_UNARY_OP,               // "not X"
    CHUNK_MEMBER_ACCESS,          // "X of Y" (field access)
    CHUNK_INDEX_ACCESS,           // "X at N" (array index)

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Type Declarations
    // "create assembly X:" — OmniCode struct/type definition
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_ASSEMBLY_DEF,           // "create assembly X:"
    CHUNK_FIELD_DEF,              // Field in assembly

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Witness (Logging)
    // "witness X with level Y" — OmniCode logging
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_WITNESS,                // "witness message with level"
    CHUNK_DISPLAY,                // "display X" (output)

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - CPI-SI State
    // Health, state, and DAR operations native to CPI-SI
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_HEALTH,                 // "health get/add/set"
    CHUNK_STATE,                  // "state get/transition"
    CHUNK_CHECKPOINT,             // "checkpoint" (DAR snapshot)
    CHUNK_RESTORE,                // "restore from X" (DAR recovery)
    CHUNK_INTRINSIC,              // "intrinsic name with args" (VM intrinsic call)

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Literals
    // OmniCode literal values
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_COVENANT_VALUE,         // granted / deferred / denied
    CHUNK_TRUTH_VALUE,            // yes / no
    CHUNK_NONE_VALUE,             // none

    // ─────────────────────────────────────────────────────────────────────────
    // Executable Code - Type References
    // OmniCode type names
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_TYPE_REF,               // Type reference (count, word, covenant, etc.)

    // ─────────────────────────────────────────────────────────────────────────
    // Structural
    // ─────────────────────────────────────────────────────────────────────────
    CHUNK_NEWLINE,                // Sentence boundary
    CHUNK_COMMENT,                // // comment text
    CHUNK_RAW_TOKEN,              // Unrecognized - pass through raw token
    CHUNK_EOF,                    // End of file

    CHUNK_COUNT_TYPES
} OmniChunkType;

// # S.2 Block Context [CONTEXT]
//
// The lexer tracks which block we're in.
// This enables context-aware chunking.

typedef enum {
    BLOCK_NONE,                   // Before any block
    BLOCK_METADATA,               // Inside METADATA
    BLOCK_HEADER,                 // Inside HEADER (5-block)
    BLOCK_CONTEXT,                // Inside CONTEXT (5-block)
    BLOCK_SETUP,                  // Inside SETUP (4-block)
    BLOCK_BODY,                   // Inside BODY (4-block)
    BLOCK_CONTENT,                // Inside CONTENT (3/5-block)
    BLOCK_CLOSING,                // Inside CLOSING
    BLOCK_FOOTER                  // Inside FOOTER (5-block)
} OmniBlockContext;

// # S.3 Chunk Structure [STRUCTURE]
//
// A chunk contains:
// - Its semantic type (what it means)
// - Its constituent tokens (what it's made of)
// - Extracted values (parsed data)

#define CHUNK_MAX_TOKENS 16
#define CHUNK_MAX_VALUE 512

typedef struct {
    OmniChunkType type;

    // Constituent tokens
    OmniToken tokens[CHUNK_MAX_TOKENS];
    int token_count;

    // Extracted semantic values
    char primary_value[CHUNK_MAX_VALUE];    // Main value (name, key, etc.)
    char secondary_value[256];              // Secondary (book name, zone, etc.)
    char tertiary_value[128];               // Tertiary (verse, branch num, etc.)

    // For scripture refs
    char book[64];
    char chapter[16];
    char verse[16];

    // Ternary value (for zones, etc.)
    int ternary_value;
    bool has_ternary;

    // Source location (first token)
    int line;
    int column;
} OmniChunk;

// # S.4 Diagnostic Entry [DIAGNOSTIC]
//
// DAR pattern: Detect → Assess → Recover
// Each diagnostic captures what was detected and suggests recovery.

#define OMNI_MAX_DIAGNOSTICS 32

typedef struct {
    char code[16];                // Error code (E001, S001, etc.)
    int  severity;                // -1=error, 0=warning, +1=info
    char message[256];            // Short message
    char detail[256];             // Detailed explanation
    char recovery[64];            // Recovery action name
    int  line;                    // Source line
    int  column;                  // Source column
    bool addressed;               // Has this been recovered?
} OmniDiagnostic;

// # S.5 Chunk Lexer State [STATE]
//
// Extended lexer state with block context awareness and config integration

typedef struct {
    OmniLexer lexer;              // Base lexer
    OmniBlockContext block;       // Current block context
    OmniToken lookahead[8];       // Token lookahead buffer
    int lookahead_count;

    // Config integration (optional - NULL if not provided)
    const Config* config;         // Loaded configuration for validation

    // DAR: Detect phase - collect diagnostics
    OmniDiagnostic diagnostics[OMNI_MAX_DIAGNOSTICS];
    int diagnostic_count;
    bool had_error;               // Any severity=-1 diagnostic?
    bool had_warning;             // Any severity=0 diagnostic?
} OmniChunkLexer;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Chunk Lexer Lifecycle [LIFECYCLE]

// Initialize chunk lexer (basic, no config)
void omni_chunk_init(OmniChunkLexer* lexer, const char* source, size_t length);

// Initialize chunk lexer with config for validation
// Config enables Scripture reference validation and Hebrew root lookup.
void omni_chunk_init_with_config(OmniChunkLexer* lexer, const char* source,
                                  size_t length, const Config* config);

// Get next semantic chunk
OmniChunk omni_chunk_next(OmniChunkLexer* lexer);

// Peek at next chunk without consuming
OmniChunk omni_chunk_peek(OmniChunkLexer* lexer);

// # B.2 Chunk Queries [QUERIES]

// Get chunk type name
const char* omni_chunk_type_name(OmniChunkType type);

// Get current block context name
const char* omni_block_context_name(OmniBlockContext ctx);

// Is this a declaration chunk?
bool omni_chunk_is_declaration(OmniChunkType type);

// Is this a grounding chunk?
bool omni_chunk_is_grounding(OmniChunkType type);

// Is this a block marker?
bool omni_chunk_is_block(OmniChunkType type);

// # B.3 Diagnostic API [DIAGNOSTICS]
//
// DAR Pattern: Detect → Assess → Recover
// These functions access diagnostics collected during parsing.

// Get count of diagnostics collected
int omni_chunk_diagnostic_count(const OmniChunkLexer* lexer);

// Get diagnostic at index (returns NULL if out of bounds)
const OmniDiagnostic* omni_chunk_diagnostic_get(const OmniChunkLexer* lexer, int index);

// Check if any errors (severity -1) were detected
bool omni_chunk_had_error(const OmniChunkLexer* lexer);

// Check if any warnings (severity 0) were detected
bool omni_chunk_had_warning(const OmniChunkLexer* lexer);

// Clear all diagnostics
void omni_chunk_diagnostics_clear(OmniChunkLexer* lexer);

// Get severity name for display ("error", "warning", "info")
const char* omni_diagnostic_severity_name(int severity);

// # B.4 Hebrew Root Lookup [HEBREW]
//
// Look up Scripture grounding for OmniCode keywords.

// Look up Hebrew root for an OmniCode keyword (requires config)
const HebrewRootConfig* omni_chunk_lookup_hebrew(const OmniChunkLexer* lexer,
                                                  const char* keyword);

// Check if keyword has Scripture grounding via Hebrew root
bool omni_chunk_keyword_is_grounded(const OmniChunkLexer* lexer,
                                     const char* keyword);

// # B.5 Recovery Suggestions [RECOVERY]
//
// DAR: RECOVER phase - get suggestions for fixing diagnostics.
// "A just man falleth seven times, and riseth up again." — Proverbs 24:16

// Get recovery suggestion text for a diagnostic
// Returns human-readable suggestion or NULL if none available
const char* omni_chunk_recovery_suggestion(const OmniDiagnostic* diag);

// Check if a diagnostic has a recovery suggestion available
bool omni_chunk_has_recovery(const OmniDiagnostic* diag);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Pattern [USAGE]
//
// OmniChunkLexer lexer;
// omni_chunk_init(&lexer, source, length);
//
// OmniChunk chunk;
// while ((chunk = omni_chunk_next(&lexer)).type != CHUNK_EOF) {
//     switch (chunk.type) {
//         case CHUNK_ENTITY_FOLDER:
//             printf("Folder: %s\n", chunk.primary_value);
//             break;
//         case CHUNK_GROUND_SCRIPTURE:
//             printf("Scripture: %s %s:%s\n",
//                    chunk.book, chunk.chapter, chunk.verse);
//             break;
//         // ...
//     }
// }

// # X.2 Closing Note [NOTE]
//
// "In the beginning was the Word, and the Word was with God"
//   — John 1:1
//
// Words carry meaning. The lexer should understand, not just tokenize.
// Semantic chunks make OmniCode truly English.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_CHUNKS_H
