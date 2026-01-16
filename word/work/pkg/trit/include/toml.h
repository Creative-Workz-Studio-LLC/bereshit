// #!omni document --header
// #!omni meta.key = BERESHIT-TRIT-TOML
// #!omni meta.from = cornerstone/engine/core/include/jsonc.h
// #!omni meta.at = document
//
// toml.h — TOML file parsing utility
// TOML config loading for Kingdom Technology specification files

#ifndef BERESHIT_TRIT_TOML_H
#define BERESHIT_TRIT_TOML_H

// ═══════════════════════════════════════════════════════════════════════════════
// METADATA [METADATA]
// ═══════════════════════════════════════════════════════════════════════════════
//
// 4-Block Code Structure: Identity and context for this component
//
// Section order: Identity → State → Attribution → Location → Derivation → Classification → Intent → Grounding → Dependencies → Roadmap
// Flow: who → when → by whom → where → from what → tagged as → why → grounded in → needs → going where
//
// ═══════════════════════════════════════════════════════════════════════════════

// # M.1 Core Identity [IDENTITY]
//
//   TOML parsing — config utility for Kingdom Technology specifications.
//
// Key: BERESHIT-TRIT-TOML
// Title: TOML Parser
// Type: Header
// Component: libtrit (Phase 3 — Config Reader)
// Role: Provides TOML file loading and parsing for type specs, math tables, etc.

// # M.2 Lifecycle State [STATE]
//
//   Current lifecycle position — alpha phase, initial implementation.
//
// Status: Active
// Version: a-03.00
// Created: 2026-01-13
// Updated: 2026-01-13

// # M.3 Attribution [ATTRIBUTION]
//
//   Authorship and ownership for this component.
//
// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
// Copyright: © 2026 CreativeWorkzStudio LLC. All rights reserved.

// # M.4 Location [LOCATION]
//
//   Filesystem path within the project.
//
// Path: word/work/pkg/trit/include/toml.h

// # M.5 Derivation [DERIVATION]
//
//   Template lineage for this file.
//
// Derives_from: cornerstone/engine/core/include/jsonc.h

// # M.6 Classification [CLASSIFICATION]
//
//   Discovery and categorization tags.
//
// Tags: header, c, libtrit, toml, parser, config, phase-3

// # M.7 Intent [INTENT]
//
//   Why this header exists and its design philosophy.
//
// Purpose: TOML parsing for Kingdom Technology specification files
// Core Design: Simple API — load file, find sections/keys, parse values
// Philosophy: Config-driven architecture — specs define types, C implements
//
// Key Features:
//   - File loading with comment stripping (# line comments)
//   - Section navigation [section] and [section.subsection]
//   - Key lookup by name (key = value syntax)
//   - Integer and string parsing from TOML values
//   - Memory management (load allocates, free releases)

// # M.8 Grounding [GROUNDING]
//
//   Biblical foundation for this component.
//
// Scripture: Proverbs 24:3-4 — By wisdom a house is built
// Principle: Good foundations enable good structures — shared utilities enable systems
// Anchor: Colossians 3:23 — Do it heartily, as to the Lord

// # M.9 Dependencies [DEPENDENCIES]
//
//   Relationship context — what this needs, what uses it.
//
// What This Needs:
//   Standard Library: stdint.h, stdbool.h, stddef.h
//   External: None
//   Internal: None — standalone utility
//
// What Uses This:
//   Commands: demo-config-reader (Phase 3 demo)
//   Libraries: libtrit config loading layer
//
// Integration Points (systems using TOML parser):
//   - config.c    → config_load_types()
//   - config.c    → config_load_math()
//   - config.c    → config_load_bible()
//   - Implementation in word/work/pkg/trit/src/toml.c

// # M.10 Roadmap [ROADMAP]
//
//   Vision and trajectory — where this TOML parser is going.
//
// Completed:
//   ✅ File loading with # comment stripping
//   ✅ Section parsing [section] and [section.subsection]
//   ✅ Key-value parsing (key = value)
//   ✅ String parsing (quoted and bare keys)
//   ✅ Integer parsing
//   ✅ Boolean parsing
//
// Planned:
//   ⏳ toml_parse_float for decimal values
//   ⏳ toml_parse_array for inline arrays
//   ⏳ Typed config struct generation
//
// Limitations:
//   - No multiline strings yet
//   - No datetime parsing yet
//   - No table arrays [[array]] yet
//
// Research:
//   - Full TOML 1.0 compliance
//   - Schema validation layer

// ═══════════════════════════════════════════════════════════════════════════════
// END METADATA
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// SETUP [SETUP]
// ═══════════════════════════════════════════════════════════════════════════════
//
// 4-Block Code Structure: Configuration and declarations before body
//
// Section order: Includes → Defines → Types → Prototypes → Extern → Omission
// Flow: dependencies → constants → data model → interface → shared state → guidance
//
// ═══════════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────────
// CODE SECTIONS
// ───────────────────────────────────────────────────────────────────────────────

// # S.1 Includes [INCLUDES]
//
//   Headers this component needs — standard types.

// # S.1a Standard Library [STDLIB]

#include <stdint.h>   // int32_t
#include <stdbool.h>  // bool, true, false
#include <stddef.h>   // size_t

// # S.1b Project Headers [PROJECT]

// [Reserved: No internal dependencies — standalone utility]

// # S.1c External Libraries [EXTERNAL]

// [Reserved: No external dependencies]

// # S.2 Defines [DEFINES]
//
//   Preprocessor constants — file limits.

// # S.2a Sizes [SIZES]

#define TOML_FILE_MAX    (128 * 1024)  // 128KB max file size
#define TOML_STRING_MAX  256           // Max string value length
#define TOML_KEY_MAX     128           // Max key name length
#define TOML_SECTION_MAX 256           // Max section path length

// # S.2b Limits [LIMITS]

// [Reserved: No additional limits needed]

// # S.2c Flags [FLAGS]

// [Reserved: No flags needed yet]

// # S.2d Macros [MACROS]

// [Reserved: No macros needed yet]

// # S.3 Types [TYPES]
//
//   Data structures — error codes for TOML operations.

// # S.3a Enumerations [ENUMS]

// TomlError codes for load/parse operations
typedef enum TomlError {
    TOML_OK = 0,               // Success
    TOML_ERR_NULL,             // Null pointer passed
    TOML_ERR_FILE,             // File open/read failed
    TOML_ERR_SIZE,             // File too large (> TOML_FILE_MAX)
    TOML_ERR_MEMORY,           // Memory allocation failed
    TOML_ERR_PARSE,            // Parse error (malformed TOML)
    TOML_ERR_NOT_FOUND,        // Key or section not found
} TomlError;

// # S.3b Structs [STRUCTS]

// [Reserved: No structs needed — uses char* for TOML content]

// # S.3c Opaque Types [OPAQUE]

// [Reserved: No opaque types needed]

// # S.3d Error Codes [ERRORS]

// [Defined in S.3a as TomlError enum]

// # S.4 Function Prototypes [PROTOTYPES]
//
//   Function declarations — TOML loading and parsing.

// # S.4a Lifecycle [LIFECYCLE]

char* toml_load_file(const char* path);     // Load file, strip comments, return allocated string
void toml_free(char* toml);                 // Free loaded TOML content

// # S.4b Accessors [ACCESSORS]

// Section navigation
const char* toml_find_section(const char* toml, const char* section);  // Find [section], return pointer after ]
const char* toml_find_subsection(const char* toml, const char* parent, const char* child);  // Find [parent.child]

// Key lookup
const char* toml_find_key(const char* toml, const char* key);          // Find key = value, return pointer to value
const char* toml_find_key_in_section(const char* toml, const char* section, const char* key);  // Find key in section

// # S.4c Operations [OPERATIONS]

// Value parsing (pointer advances past parsed content)
const char* toml_parse_int(const char* p, int32_t* out);               // Parse integer value
const char* toml_parse_int64(const char* p, int64_t* out);             // Parse 64-bit integer
const char* toml_parse_string(const char* p, char* out, size_t max);   // Parse quoted string
const char* toml_parse_bool(const char* p, bool* out);                 // Parse true/false value
const char* toml_parse_bare_key(const char* p, char* out, size_t max); // Parse bare (unquoted) key

// Navigation helpers
const char* toml_skip_whitespace(const char* p);       // Skip whitespace (space, tab)
const char* toml_skip_line(const char* p);             // Skip to next line
const char* toml_skip_to_newline(const char* p);       // Skip to end of current line

// Inline array iteration (for [ a, b, c ] syntax)
const char* toml_array_first(const char* arr);         // Get pointer to first element (after '[')
const char* toml_array_next(const char* p);            // Move to next element (after ',' or NULL at ']')
int32_t     toml_count_array(const char* arr);         // Count elements in inline array

// # S.4d Errors [ERRORS]

const char* toml_get_error_string(TomlError err);      // Get error description

// # S.5 File-Level Declarations [FILE_LEVEL]
//
//   File-level state — TOML parser is stateless, no externs needed.

// # S.5a Variables [VARIABLES]

// [Reserved: TOML parser is stateless]

// # S.5b Function Types [FUNC_TYPES]

// [Reserved: No callbacks needed yet]

// # S.5c Infrastructure [INFRASTRUCTURE]

// [Reserved: Infrastructure lives in source files, not headers]

// # S.6 Omission Guide [OMISSION]
//
//   Guidance for what can be reserved in SETUP sections.
//
//   S.2b-d reserved — only size constants defined
//   S.3b-d reserved — only TomlError enum needed
//   S.5a-c reserved — parser is stateless, no externs

// ═══════════════════════════════════════════════════════════════════════════════
// END SETUP
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// BODY [BODY]
// ═══════════════════════════════════════════════════════════════════════════════
//
// 4-Block Code Structure: The actual work — declarations (headers), implementations (sources)
//
// Section order: Org Chart → Helpers → Core Ops → Errors → Public API → Omission
// Flow: map structure → foundations → business logic → error handling → exported interface → guidance
//
// ═══════════════════════════════════════════════════════════════════════════════

// # B.1 Organizational Chart [ORG_CHART]
//
//   Interface structure — what this header provides.

// # B.1a Structure [STRUCTURE]

// Types:      1 enum (TomlError)
// Functions:  2 lifecycle, 4 accessors, 10 operations, 1 error
// Externs:    0 globals, 0 callbacks

// # B.1b Flow [FLOW]

// Implementation: word/work/pkg/trit/src/toml.c

// # B.1c Counts [COUNTS]

// Types:      1 total (1 enum)
// Functions:  17 total (2 lifecycle, 4 accessors, 10 operations, 1 error)
// Externs:    0 total

// # B.2 Helpers [HELPERS]
//
//   Foundation functions — all helpers are static in toml.c.

// # B.2a Pure Functions [PURE]

// [Reserved: Comment stripping static in source file]

// # B.2b Utilities [UTIL]

// [Reserved: File I/O helpers static in source file]

// # B.3 Core Operations [OPERATIONS]
//
//   TOML operations — file loading and value parsing.

// # B.3a File Loading [LOAD]
//
//   Load TOML files with comment stripping.

// toml_load_file loads a TOML file and strips comments.
//
// Parameters:
//   path: Path to TOML file
//
// Returns:
//   Allocated string with comments stripped (caller must free)
//   NULL on error (file not found, too large, memory error)
//
// Notes:
//   Strips # line comments only
//   Preserves strings (comments inside quotes are kept)

// toml_free frees content returned by toml_load_file.
//
// Parameters:
//   toml: Pointer returned by toml_load_file (NULL-safe)

// # B.3b Section Navigation [SECTION]
//
//   Find sections in TOML content.

// toml_find_section finds a [section] header and returns pointer after ].
//
// Parameters:
//   toml:    TOML content string
//   section: Section name to find (without brackets)
//
// Returns:
//   Pointer to content after [section] header
//   NULL if section not found

// toml_find_subsection finds a [parent.child] section.
//
// Parameters:
//   toml:   TOML content string
//   parent: Parent section name
//   child:  Child section name
//
// Returns:
//   Pointer to content after [parent.child] header
//   NULL if not found

// # B.3c Key Lookup [LOOKUP]
//
//   Find keys in TOML content.

// toml_find_key finds key = value and returns pointer to value.
//
// Parameters:
//   toml: TOML content string
//   key:  Key name to find (bare or quoted)
//
// Returns:
//   Pointer to value after = (skip whitespace)
//   NULL if key not found

// toml_find_key_in_section finds key within a specific section.
//
// Parameters:
//   toml:    TOML content string
//   section: Section name (without brackets)
//   key:     Key name to find
//
// Returns:
//   Pointer to value within that section
//   NULL if section or key not found

// # B.3d Value Parsing [PARSE]
//
//   Parse values from TOML content.

// toml_parse_int parses an integer value.
//
// Parameters:
//   p:   Pointer to start of value
//   out: Output integer
//
// Returns:
//   Pointer past parsed value, NULL on error

// toml_parse_string parses a quoted string value.
//
// Parameters:
//   p:   Pointer to opening quote
//   out: Output buffer
//   max: Buffer size
//
// Returns:
//   Pointer past closing quote, NULL on error

// toml_parse_bool parses a TOML boolean value (true/false).
//
// Parameters:
//   p:   Pointer to 't' or 'f' of true/false
//   out: Output boolean
//
// Returns:
//   Pointer past parsed value, NULL on error

// toml_parse_bare_key parses an unquoted key name.
//
// Parameters:
//   p:   Pointer to start of key
//   out: Output buffer
//   max: Buffer size
//
// Returns:
//   Pointer past key (at = or whitespace), NULL on error

// toml_skip_whitespace advances past whitespace (space/tab only).
//
// Parameters:
//   p: Current position
//
// Returns:
//   Pointer to first non-whitespace character

// toml_skip_line advances to start of next line.
//
// Parameters:
//   p: Current position
//
// Returns:
//   Pointer to first character of next line, or end of string

// toml_skip_to_newline advances to end of current line.
//
// Parameters:
//   p: Current position
//
// Returns:
//   Pointer to newline character, or end of string

// # B.3e Array Iteration [ARRAY]
//
//   Iterate over TOML inline arrays.

// toml_array_first returns pointer to first element in inline array.
//
// Parameters:
//   arr: Pointer to opening '[' of inline array
//
// Returns:
//   Pointer to first element (past '[' and whitespace)
//   NULL if empty array or error

// toml_array_next moves to next element in inline array.
//
// Parameters:
//   p: Pointer to current element
//
// Returns:
//   Pointer to next element (past ',' and whitespace)
//   NULL if at end of array (']' reached) or error

// toml_count_array counts elements in inline array.
//
// Parameters:
//   arr: Pointer to opening '[' of inline array
//
// Returns:
//   Number of elements, 0 for empty array, -1 on error

// # B.4 Error Handling [ERRORS]
//
//   TOML errors — recoverable, caller checks return values.
//
// Recovery Strategy:
//   - TOML_ERR_NULL: Check pointer before calling
//   - TOML_ERR_FILE: Check path exists, permissions
//   - TOML_ERR_SIZE: File too large, increase TOML_FILE_MAX or split file
//   - TOML_ERR_MEMORY: System out of memory
//   - TOML_ERR_PARSE: Malformed TOML, check syntax
//   - TOML_ERR_NOT_FOUND: Section or key doesn't exist

// # B.4a Error Queries [QUERIES]
//
//   Functions to get human-readable error messages.

// toml_get_error_string returns human-readable error description.
//
// Parameters:
//   err: Error code from TomlError enum
//
// Returns:
//   Static string (do not free)

// # B.4b Logging [LOGGING]
//
//   Logging convention — matches libtrit pattern.
//
// Format: fprintf(stderr, "[TOML] message\n", ...);
// Prefix: [TOML] for all parser output
// Level:  stderr for errors (no logger infrastructure yet)

// # B.4c Recovery [RECOVERY]
//
//   [Reserved: Caller frees on error via toml_free]

// # B.5 Public API [API]
//
//   [Reserved: API declared in S.4 Prototypes, documented in B.3 Operations]

// # B.5a Lifecycle [LIFECYCLE]

// [Reserved: See S.4a — toml_load_file, toml_free]

// # B.5b Accessors [ACCESSORS]

// [Reserved: See S.4b — toml_find_section, toml_find_key, toml_find_key_in_section]

// # B.5c Operations [OPS]

// [Reserved: See S.4c — toml_parse_int, toml_parse_string, toml_skip_whitespace]

// # B.5d Utilities [UTIL]

// [Reserved: See S.4d — toml_get_error_string]

// # B.6 Omission Guide [OMISSION]
//
//   Guidance for BODY sections in this header.
//
//   B.2 Helpers: Reserved — comment stripping static in source file
//   B.5 Public API: Reserved — API declared in S.4, documented in B.3

// ═══════════════════════════════════════════════════════════════════════════════
// END BODY
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// CLOSING [CLOSING]
// ═══════════════════════════════════════════════════════════════════════════════
//
// 4-Block Code Structure: Operational guidance and closing notes
//
// Section order: Validation → Execution → Cleanup | Policy → Extension → Troubleshooting → Reference → Note → Template
// Flow: verify → run → release | modify → extend → debug → lookup → ground → instantiate
//
// ═══════════════════════════════════════════════════════════════════════════════

// # X.1 Validation [VALIDATION]
//
//   Verify header correctness — syntax and self-containment.

// # X.1a Build Commands [BUILD]
//
//   gcc -fsyntax-only -Wall -Wextra word/work/pkg/trit/include/toml.h
//   gcc -c word/work/pkg/trit/src/toml.c -I word/work/pkg/trit/include

// # X.1b Checks [CHECKS]
//
//   - No external dependencies (standalone header)
//   - Prototypes match toml.c implementations

// # X.1c Self-Containment Test [SELF_TEST]
//
//   // test.c — verify header compiles standalone
//   #include "toml.h"
//   int main(void) { return 0; }

// # X.1d Memory Checks [MEMORY]
//
//   valgrind --leak-check=full ./demo-config-reader  // Check for TOML memory leaks
//
// # X.2 Execution [EXECUTION]
//
//   Usage patterns — include and call sequence.

// # X.2a Include [INCLUDE]
//
//   #include "toml.h"

// # X.2b Exit Codes [EXIT_CODES]
//
//   TOML_OK = 0 - Success
//   TOML_ERR_* - Various failure modes

// # X.2c Usage Example [USAGE]
//
//   #include "toml.h"
//
//   char* toml = toml_load_file("word/core/types/trit.toml");
//   if (toml == NULL) {
//       fprintf(stderr, "Failed to load config\n");
//       return;
//   }
//
//   // Find section and key
//   const char* val = toml_find_key_in_section(toml, "trit", "bits");
//   int32_t bits;
//   if (val && toml_parse_int(val, &bits)) {
//       printf("Trit bits: %d\n", bits);
//   }
//
//   toml_free(toml);  // Cleanup required

// # X.3 Cleanup [CLEANUP]
//
//   Ownership and cleanup — toml_load_file allocates, caller frees.

// # X.3a Ownership [OWNERSHIP]
//
//   TOML parser allocates, caller frees:
//   - toml_load_file returns heap-allocated string
//   - Caller MUST call toml_free when done
//   - NULL return means no allocation (nothing to free)

// # X.3b Memory Patterns [MEMORY]
//
//   char* toml = toml_load_file("config.toml");
//   if (toml) {
//       // ... parse values from toml ...
//       toml_free(toml);  // Required cleanup
//   }

// # X.3c Graceful Shutdown [SHUTDOWN]
//
//   - Call toml_free for each toml_load_file
//   - Parser is stateless — no global cleanup
//   - NULL-safe: toml_free(NULL) is no-op

// ───────────────────────────────────────────────────────────────────────────────
// END CODE SECTIONS
// ───────────────────────────────────────────────────────────────────────────────

// # X.4 Policy [POLICY]
//
//   Modification guidance — TOML parser is stable interface.
//
// Safe to Modify:
//   ✅ Add new parse functions (toml_parse_float, toml_parse_datetime)
//   ✅ Add new error codes to TomlError
//   ✅ Increase size limits (TOML_FILE_MAX, TOML_STRING_MAX)
//
// Modify with Care:
//   ⚠️ Function signatures — breaks all calling code
//   ⚠️ Return conventions (NULL on error) — breaks error handling
//
// NEVER Modify:
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Include guard (BERESHIT_TRIT_TOML_H)
//   ❌ toml_load_file/toml_free ownership contract
//
// Validation: See X.1 for build and memory checks.
//
// # X.5 Extension [EXTENSION]
//
//   How to extend TOML parser — add value types while keeping simple interface.
//
// Adding New Parse Functions:
//   - toml_parse_float for decimal values
//   - toml_parse_datetime for dates/times
//   - toml_parse_array for inline arrays
//
// Adding Section Support:
//   - toml_find_section navigates to [section]
//   - toml_find_subsection for [parent.child]
//   - toml_find_key_in_section combines both
//
// Adding Validation:
//   - Schema validation layer on top of parser
//   - Type checking with TomlError codes
//
// Roadmap: See M.10 for planned features.

// # X.6 Troubleshooting [TROUBLESHOOTING]
//
//   Common problems and solutions — TOML parser issues.
//
// "toml_load_file returns NULL":
//   → Check file path is correct
//   → Verify file exists and is readable
//   → Check file size < TOML_FILE_MAX
//
// "toml_find_key returns NULL":
//   → Key name must match exactly (case-sensitive)
//   → Use toml_find_section first for keys in sections
//   → Use toml_find_key_in_section for easier access
//
// "toml_parse_int returns NULL":
//   → Value must be a number, not quoted string
//   → Check for leading/trailing whitespace
//   → Verify TOML syntax (no trailing commas)
//
// Function-specific issues: See B.3 docstrings for each function.

// # X.7 Reference [REFERENCE]
//
//   Quick lookup — copy-paste ready TOML patterns.
//
// Load and Parse:
//   char* toml = toml_load_file("word/core/types/trit.toml");
//   if (!toml) { /* handle error */ }
//
// Find Key in Section:
//   const char* val = toml_find_key_in_section(toml, "trit", "bits");
//   int32_t bits;
//   toml_parse_int(val, &bits);
//
// Find Section, then Key:
//   const char* section = toml_find_section(toml, "trit");
//   const char* val = toml_find_key(section, "bits");
//
// Parse String:
//   char name[TOML_STRING_MAX];
//   toml_parse_string(val, name, sizeof(name));
//
// Error Handling:
//   if (!toml) {
//       fprintf(stderr, "[CONFIG] %s\n",
//               toml_get_error_string(TOML_ERR_FILE));
//   }
//
// Cleanup:
//   toml_free(toml);  // Always free when done
//
// Compile:
//   gcc -Wall -Wextra -std=c11 main.c toml.c -I include

// # X.8 Closing Note [NOTE]
//
//   Final guidance — configuration enables adaptability.
//
// This header declares the TOML parser foundation.
// Kingdom Technology specs drive libtrit implementation.
//
// "A word fitly spoken is like apples of gold in pictures of silver."
//   — Proverbs 25:11
//
// Configuration externalizes values; code implements wisdom.

// # X.9 Omission Guide [OMISSION]
//
//   Guidance for CLOSING sections in TOML header.
//
//   All sections present and filled:
//   - X.1 Validation: Build and memory check commands
//   - X.2 Execution: Include path and usage example
//   - X.3 Cleanup: Memory ownership patterns
//   - X.4 Policy: Modification safety levels
//   - X.5 Extension: How to add parse functions
//   - X.6 Troubleshooting: Parser-specific issues
//   - X.7 Reference: Copy-paste TOML patterns
//   - X.8 Closing Note: Configuration grounding
//
//   Key principle: CLOSING synthesizes, earlier blocks contain details.

// ═══════════════════════════════════════════════════════════════════════════════
// END CLOSING
// ═══════════════════════════════════════════════════════════════════════════════

#endif // BERESHIT_TRIT_TOML_H


