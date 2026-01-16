// #!omni document --source
// #!omni meta.key = BERESHIT-TRIT-TOML
// #!omni meta.from = cornerstone/engine/core/src/jsonc.c
// #!omni meta.at = document
//
// toml.c — TOML file parsing implementation
// TOML config loading for Kingdom Technology specification files

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
//   TOML parser implementation — config file loading.
//
// Key: BERESHIT-TRIT-TOML
// Title: TOML Parser Implementation
// Type: Source
// Component: libtrit (Phase 3 — Config Reader)
// Role: Implements toml.h interface — file loading, section/key lookup, value parsing

// # M.2 Lifecycle State [STATE]
//
//   Current state of this implementation.
//
// Status: Active
// Version: a-03.00
// Created: 2026-01-13
// Updated: 2026-01-13

// # M.3 Attribution [ATTRIBUTION]
//
//   Who created and maintains this implementation.
//
// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
// Copyright: © 2026 CreativeWorkzStudio LLC. All rights reserved.

// # M.4 Location [LOCATION]
//
//   Where this file lives in the project.
//
// Path: word/work/pkg/trit/src/toml.c

// # M.5 Derivation [DERIVATION]
//
//   Template lineage for this file.
//
// Derives_from: cornerstone/engine/core/src/jsonc.c

// # M.6 Classification [CLASSIFICATION]
//
//   Discovery tags for this implementation.
//
// Tags: source, c, implementation, libtrit, toml, parser, config, phase-3

// # M.7 Intent [INTENT]
//
//   Why this implementation exists.
//
// Purpose: Provides TOML file loading and parsing for Kingdom Technology specs
// Core Design: Load file → strip # comments → navigate sections → parse values
// Philosophy: Config-driven architecture — specs define types, C implements
//
// Key Features:
//   - File loading with # comment stripping
//   - Section navigation [section] and [section.subsection]
//   - Key lookup with key = value syntax
//   - Integer and string value parsing
//   - Error handling with codes

// # M.8 Grounding [GROUNDING]
//
//   Biblical foundation for this work.
//
// Scripture: Proverbs 25:11 — A word fitly spoken is like apples of gold
// Principle: Configuration externalizes values; code implements wisdom
// Anchor: Colossians 3:23 — Do it heartily, as to the Lord

// # M.9 Dependencies [DEPENDENCIES]
//
//   What this implementation needs and what uses it.
//
// What This Needs:
//   Standard Library: stdio.h (FILE), stdlib.h (malloc/free), string.h (strlen)
//   External: None
//   Internal: toml.h
//
// What Uses This:
//   Commands: demo-config-reader (Phase 3 demo)
//   Libraries: libtrit config loading layer
//   Tools: None
//
// Integration Points:
//   - Implements toml.h interface
//   - Used by all systems loading TOML config files
//   - No external dependencies

// # M.10 Roadmap [ROADMAP]
//
//   Future development for this implementation.
//
// Planned:
//   ⏳ toml_parse_float for decimal values
//   ⏳ toml_parse_datetime for date/time
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
//
// Operational:
//   Blocking: No — parse failures return NULL, caller handles
//   Mitigation: Return error codes, callers use defaults

// ═══════════════════════════════════════════════════════════════════════════════
// END METADATA
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// SETUP [SETUP]
// ═══════════════════════════════════════════════════════════════════════════════
//
// 4-Block Code Structure: Configuration and declarations before body
//
// Section order: Includes → Defines → Types → Prototypes → Static Variables → Omission
// Flow: dependencies → constants → data model → declarations → file state → guidance
//
// ═══════════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────────
// CODE SECTIONS
// ───────────────────────────────────────────────────────────────────────────────

// # S.1 Includes [INCLUDES]
//
//   Headers needed for JSONC implementation.

// # S.1a Standard Library [STDLIB]

#include <stdio.h>    // FILE, fopen, fread, fclose
#include <stdlib.h>   // malloc, free
#include <string.h>   // strlen, strncmp, memcpy
#include <stdbool.h>  // bool for internal helpers
#include <ctype.h>    // isspace, isdigit

// # S.1b Project Headers [PROJECT]

#include "toml.h"  // Interface we implement

// # S.1c External Libraries [EXTERNAL]

// [Reserved: No external dependencies]

// # S.2 Defines [DEFINES]
//
//   Constants and macros for JSONC implementation.

// # S.2a Sizes [SIZES]

// [Reserved: Sizes defined in jsonc.h]

// # S.2b Limits [LIMITS]

// [Reserved: Limits defined in jsonc.h]

// # S.2c Flags [FLAGS]

// [Reserved: No flags needed yet]

// # S.2d Macros [MACROS]

// [Reserved: No macros needed]

// # S.3 Types [TYPES]
//
//   Internal types for JSONC implementation.

// # S.3a Enumerations [ENUMS]

// [Reserved: All enums defined in jsonc.h]

// # S.3b Structs [STRUCTS]

// [Reserved: No structs needed]

// # S.3c Opaque Types [OPAQUE]

// [Reserved: No opaque types needed]

// # S.3d Error Codes [ERRORS]

// [Reserved: Error codes defined in jsonc.h]

// # S.4 Function Prototypes [PROTOTYPES]
//
//   Static helper declarations for JSONC implementation.

// # S.4a Lifecycle [LIFECYCLE]

// [Reserved: No static lifecycle helpers needed]

// # S.4b Accessors [ACCESSORS]

// [Reserved: No static accessors needed]

// # S.4c Operations [OPERATIONS]

static char* strip_comments(const char* src, size_t len);  // Strip # line comments

// # S.5 File-Level Declarations [FILE_LEVEL]
//
//   Static state for TOML parser.

// # S.5a Variables [VARIABLES]

// [Reserved: TOML parser is stateless — no file-level state]

// # S.5b Function Types [FUNC_TYPES]

// [Reserved: No callback types needed yet]

// # S.5c Infrastructure [INFRASTRUCTURE]

// [Reserved: Using fprintf(stderr) until logger infrastructure exists]

// # S.6 Omission Guide [OMISSION]
//
//   Guidance for SETUP sections in this implementation.
//
//   S.2a-d: Reserved — sizes/limits in header, no macros needed
//   S.3: Reserved — all types in toml.h
//   S.4c: strip_comments helper for # comment removal
//   S.5: Reserved — parser is stateless

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
//   Implementation structure for TOML parser.

// # B.1a Structure [STRUCTURE]

// Public APIs (Top Rungs - Orchestration)
// Lifecycle (2):
// ├── toml_load_file()          → load file, strip comments
// └── toml_free()               → free loaded content
//
// Accessors (4):
// ├── toml_find_section()       → find [section], return pointer after ]
// ├── toml_find_subsection()    → find [parent.child]
// ├── toml_find_key()           → find key = value, return value pointer
// └── toml_find_key_in_section()→ find key within section
//
// Operations (10):
// ├── toml_parse_int()          → parse integer value
// ├── toml_parse_string()       → parse quoted string
// ├── toml_parse_bool()         → parse true/false value
// ├── toml_parse_bare_key()     → parse unquoted key
// ├── toml_skip_whitespace()    → skip space/tab
// ├── toml_skip_line()          → skip to next line
// ├── toml_skip_to_newline()    → skip to end of line
// ├── toml_array_first()        → get first inline array element
// ├── toml_array_next()         → get next inline array element
// └── toml_count_array()        → count inline array elements
//
// Errors (1):
// └── toml_get_error_string()   → error messages
//
// Helpers (Bottom Rungs - Foundations)
// └── strip_comments()          → remove # line comments

// # B.1b Flow [FLOW]

// Entry → toml_load_file(path)
//   ↓
// Navigate: toml_find_section(), toml_find_subsection()
//   ↓
// Lookup: toml_find_key(), toml_find_key_in_section()
//   ↓
// Parse: toml_parse_int(), toml_parse_string(), toml_parse_bool()
//   ↓
// Cleanup → toml_free(toml)

// # B.1c Counts [COUNTS]

// - 18 functions total (1 helper + 17 public)
// - 1 helper: strip_comments
// - 17 public APIs (toml_*): 2 lifecycle, 4 accessors, 10 operations, 1 error

// # B.2 Helpers [HELPERS]
//
//   Foundation functions for TOML implementation.

// # B.2a Pure Functions [PURE]

// [Reserved: No pure functions needed]

// # B.2b Utilities [UTIL]

// strip_comments removes # line comments from TOML content.
// Preserves content inside quoted strings.
//
// Parameters:
//   src: Source TOML content
//   len: Length of source
//
// Returns:
//   Allocated string with comments stripped (caller must free)
//   NULL on memory allocation failure
static char* strip_comments(const char* src, size_t len) {
    char* out = malloc(len + 1);
    if (!out) return NULL;

    size_t j = 0;
    bool in_string = false;

    for (size_t i = 0; i < len; i++) {
        // Track string state (handle escape sequences)
        if (src[i] == '"' && (i == 0 || src[i-1] != '\\')) {
            in_string = !in_string;
            out[j++] = src[i];
            continue;
        }

        // Inside string - copy verbatim
        if (in_string) {
            out[j++] = src[i];
            continue;
        }

        // TOML line comment: # to end of line
        if (src[i] == '#') {
            while (i < len && src[i] != '\n') i++;
            if (i < len) out[j++] = '\n';  // Keep newline
            continue;
        }

        // Regular character
        out[j++] = src[i];
    }

    out[j] = '\0';
    return out;
}

// # B.3 Core Operations [OPERATIONS]
//
//   [Reserved: All logic in B.5 Public API for this simple parser]

// # B.4 Error Handling [ERRORS]
//
//   Error handling for TOML operations.
//
//   Design Principle: Non-blocking — parse failures return NULL
//
//   Recovery Strategy:
//     - NULL pointer: Return NULL
//     - File not found: Return NULL
//     - Parse error: Return NULL, caller uses defaults

// # B.4a Error Logging [LOGGING]

// [Reserved: Using fprintf(stderr) directly — no logger infrastructure yet]

// # B.4b Cleanup Patterns [CLEANUP]

// TOML uses NULL return pattern:
//   - toml_load_file returns NULL on any error
//   - Caller must check return value
//   - toml_free is NULL-safe

// # B.4c Recovery [RECOVERY]

// [Reserved: TOML errors are recoverable — callers use defaults]

// # B.5 Public API [API]
//
//   TOML interface implementations.

// # B.5a Lifecycle [LIFECYCLE]
//
//   File loading and cleanup.

// toml_load_file loads a TOML file and strips comments.
//
// Parameters:
//   path: Path to TOML file
//
// Returns:
//   Allocated string with comments stripped (caller must free)
//   NULL on error (file not found, too large, memory error)
char* toml_load_file(const char* path) {
    if (path == NULL) {
        fprintf(stderr, "[TOML] toml_load_file: NULL path\n");
        return NULL;
    }

    // Open file
    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[TOML] Cannot open: %s\n", path);
        return NULL;
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Check size limit
    if (size > TOML_FILE_MAX) {
        fprintf(stderr, "[TOML] File too large: %s (%ld bytes)\n", path, size);
        fclose(f);
        return NULL;
    }

    // Allocate and read
    char* buf = malloc((size_t)size + 1);
    if (!buf) {
        fprintf(stderr, "[TOML] Memory allocation failed\n");
        fclose(f);
        return NULL;
    }

    size_t read = fread(buf, 1, (size_t)size, f);
    fclose(f);
    buf[read] = '\0';

    // Strip comments
    char* toml = strip_comments(buf, read);
    free(buf);

    return toml;
}

// toml_free frees content returned by toml_load_file.
//
// Parameters:
//   toml: Pointer returned by toml_load_file (NULL-safe)
void toml_free(char* toml) {
    free(toml);  // free(NULL) is safe
}

// # B.5b Accessors [ACCESSORS]
//
//   Section and key lookup functions.

// toml_find_section finds a [section] header and returns pointer after ].
//
// Parameters:
//   toml:    TOML content string
//   section: Section name to find (without brackets)
//
// Returns:
//   Pointer to content after [section] header
//   NULL if section not found
const char* toml_find_section(const char* toml, const char* section) {
    if (!toml || !section) return NULL;

    size_t section_len = strlen(section);
    const char* p = toml;

    while (*p) {
        // Skip whitespace at start of line
        while (*p && (*p == ' ' || *p == '\t')) p++;

        // Look for section header [
        if (*p == '[' && p[1] != '[') {  // Not table array [[
            p++;  // Skip [

            // Skip whitespace inside brackets
            while (*p && (*p == ' ' || *p == '\t')) p++;

            // Check if section name matches
            if (strncmp(p, section, section_len) == 0) {
                const char* after = p + section_len;
                // Skip whitespace
                while (*after && (*after == ' ' || *after == '\t')) after++;
                // Must end with ]
                if (*after == ']') {
                    after++;  // Skip ]
                    // Skip to end of line
                    while (*after && *after != '\n') after++;
                    if (*after == '\n') after++;
                    return after;  // Return pointer to content after section header
                }
            }
        }

        // Skip to next line
        while (*p && *p != '\n') p++;
        if (*p == '\n') p++;
    }

    return NULL;
}

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
const char* toml_find_subsection(const char* toml, const char* parent, const char* child) {
    if (!toml || !parent || !child) return NULL;

    // Build the full section name "parent.child"
    char full_section[TOML_SECTION_MAX];
    size_t parent_len = strlen(parent);
    size_t child_len = strlen(child);

    if (parent_len + 1 + child_len >= TOML_SECTION_MAX) return NULL;

    memcpy(full_section, parent, parent_len);
    full_section[parent_len] = '.';
    memcpy(full_section + parent_len + 1, child, child_len);
    full_section[parent_len + 1 + child_len] = '\0';

    return toml_find_section(toml, full_section);
}

// toml_find_key finds key = value and returns pointer to value.
//
// Parameters:
//   toml: TOML content string
//   key:  Key name to find (bare or quoted)
//
// Returns:
//   Pointer to value after = (skip whitespace)
//   NULL if key not found
const char* toml_find_key(const char* toml, const char* key) {
    if (!toml || !key) return NULL;

    size_t key_len = strlen(key);
    const char* p = toml;

    while (*p) {
        // Check for section header BEFORE skipping whitespace
        // In TOML, section headers like [section] have no leading whitespace
        // Arrays like "    [ 1, 2, 3 ]" have leading whitespace - don't confuse them
        if (*p == '[') return NULL;  // True section header at column 0

        // Skip whitespace at start of line
        while (*p && (*p == ' ' || *p == '\t')) p++;

        // Check if this line starts with the key
        // Handle both bare keys and quoted keys
        bool key_match = false;
        const char* line_start = p;

        if (*p == '"') {
            // Quoted key
            p++;
            if (strncmp(p, key, key_len) == 0 && p[key_len] == '"') {
                p += key_len + 1;  // Skip key and closing quote
                key_match = true;
            }
        } else if (isalnum((unsigned char)*p) || *p == '_' || *p == '-') {
            // Bare key
            if (strncmp(p, key, key_len) == 0) {
                char next = p[key_len];
                // Key must be followed by whitespace or =
                if (next == ' ' || next == '\t' || next == '=') {
                    p += key_len;
                    key_match = true;
                }
            }
        }

        if (key_match) {
            // Skip whitespace before =
            while (*p && (*p == ' ' || *p == '\t')) p++;
            if (*p == '=') {
                p++;  // Skip =
                // Skip whitespace after =
                while (*p && (*p == ' ' || *p == '\t')) p++;
                return p;  // Return pointer to value
            }
        }

        // Skip to next line
        p = line_start;
        while (*p && *p != '\n') p++;
        if (*p == '\n') p++;
    }

    return NULL;
}

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
const char* toml_find_key_in_section(const char* toml, const char* section, const char* key) {
    if (!toml || !section || !key) return NULL;

    // Find the section first
    const char* section_start = toml_find_section(toml, section);
    if (!section_start) return NULL;

    // Find the key within this section
    return toml_find_key(section_start, key);
}

// # B.5c Operations [OPERATIONS]
//
//   Value parsing functions.

// toml_parse_int parses an integer value.
//
// Parameters:
//   p:   Pointer to start of value
//   out: Output integer
//
// Returns:
//   Pointer past parsed value, NULL on error
const char* toml_parse_int(const char* p, int32_t* out) {
    if (!p || !out) return NULL;

    // Skip whitespace (space/tab only for TOML)
    while (*p && (*p == ' ' || *p == '\t')) p++;

    // Handle negative
    int sign = 1;
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    // Must start with digit
    if (!isdigit((unsigned char)*p)) return NULL;

    // Parse digits (TOML allows underscores in numbers)
    int32_t val = 0;
    while (isdigit((unsigned char)*p) || *p == '_') {
        if (*p != '_') {
            val = val * 10 + (*p - '0');
        }
        p++;
    }

    *out = val * sign;
    return p;
}

// toml_parse_int64 parses a 64-bit integer value.
//
// Parameters:
//   p:   Pointer to start of value
//   out: Output 64-bit integer
//
// Returns:
//   Pointer past parsed value, NULL on error
const char* toml_parse_int64(const char* p, int64_t* out) {
    if (!p || !out) return NULL;

    // Skip whitespace (space/tab only for TOML)
    while (*p && (*p == ' ' || *p == '\t')) p++;

    // Handle negative
    int sign = 1;
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    // Must start with digit
    if (!isdigit((unsigned char)*p)) return NULL;

    // Parse digits (TOML allows underscores in numbers)
    int64_t val = 0;
    while (isdigit((unsigned char)*p) || *p == '_') {
        if (*p != '_') {
            val = val * 10 + (*p - '0');
        }
        p++;
    }

    *out = val * sign;
    return p;
}

// toml_parse_string parses a quoted string value.
//
// Parameters:
//   p:   Pointer to opening quote
//   out: Output buffer
//   max: Buffer size
//
// Returns:
//   Pointer past closing quote, NULL on error
const char* toml_parse_string(const char* p, char* out, size_t max) {
    if (!p || !out || max == 0) return NULL;

    // Skip whitespace
    while (*p && (*p == ' ' || *p == '\t')) p++;

    // Must start with quote
    if (*p != '"') return NULL;
    p++;

    size_t i = 0;
    while (*p && *p != '"' && *p != '\n' && i < max - 1) {
        // Handle escape sequences
        if (*p == '\\' && p[1]) {
            p++;
            switch (*p) {
                case 'n':  out[i++] = '\n'; break;
                case 't':  out[i++] = '\t'; break;
                case 'r':  out[i++] = '\r'; break;
                case '\\': out[i++] = '\\'; break;
                case '"':  out[i++] = '"';  break;
                default:   out[i++] = *p;   break;
            }
        } else {
            out[i++] = *p;
        }
        p++;
    }

    out[i] = '\0';

    // Must end with quote
    if (*p != '"') return NULL;
    p++;

    return p;
}

// toml_parse_bool parses a TOML boolean value (true/false).
//
// Parameters:
//   p:   Pointer to 't' or 'f' of true/false
//   out: Output boolean
//
// Returns:
//   Pointer past parsed value, NULL on error
const char* toml_parse_bool(const char* p, bool* out) {
    if (!p || !out) return NULL;

    // Skip whitespace
    while (*p && (*p == ' ' || *p == '\t')) p++;

    // Check for true
    if (strncmp(p, "true", 4) == 0) {
        *out = true;
        return p + 4;
    }

    // Check for false
    if (strncmp(p, "false", 5) == 0) {
        *out = false;
        return p + 5;
    }

    return NULL;  // Not a boolean
}

// toml_parse_bare_key parses an unquoted key name.
//
// Parameters:
//   p:   Pointer to start of key
//   out: Output buffer
//   max: Buffer size
//
// Returns:
//   Pointer past key (at = or whitespace), NULL on error
const char* toml_parse_bare_key(const char* p, char* out, size_t max) {
    if (!p || !out || max == 0) return NULL;

    // Skip whitespace
    while (*p && (*p == ' ' || *p == '\t')) p++;

    // Bare keys can contain A-Za-z0-9_-
    size_t i = 0;
    while (*p && i < max - 1) {
        if (isalnum((unsigned char)*p) || *p == '_' || *p == '-') {
            out[i++] = *p;
            p++;
        } else {
            break;
        }
    }

    if (i == 0) return NULL;  // No key found

    out[i] = '\0';
    return p;
}

// toml_skip_whitespace advances past whitespace (space/tab only).
//
// Parameters:
//   p: Current position
//
// Returns:
//   Pointer to first non-whitespace character
const char* toml_skip_whitespace(const char* p) {
    if (!p) return NULL;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    return p;
}

// toml_skip_line advances to start of next line.
//
// Parameters:
//   p: Current position
//
// Returns:
//   Pointer to first character of next line, or end of string
const char* toml_skip_line(const char* p) {
    if (!p) return NULL;
    while (*p && *p != '\n') p++;
    if (*p == '\n') p++;
    return p;
}

// toml_skip_to_newline advances to end of current line.
//
// Parameters:
//   p: Current position
//
// Returns:
//   Pointer to newline character, or end of string
const char* toml_skip_to_newline(const char* p) {
    if (!p) return NULL;
    while (*p && *p != '\n') p++;
    return p;
}

// # B.5e Array Iteration [ARRAY]
//
//   Inline array iteration functions for [ a, b, c ] syntax.

// toml_array_first returns pointer to first element in inline array.
//
// Parameters:
//   arr: Pointer to opening '[' of inline array
//
// Returns:
//   Pointer to first element, NULL if empty or error
const char* toml_array_first(const char* arr) {
    if (!arr || *arr != '[') return NULL;

    arr++;  // Skip '['
    // Skip whitespace including newlines (for multi-line arrays)
    while (*arr && (*arr == ' ' || *arr == '\t' || *arr == '\n' || *arr == '\r')) arr++;

    if (*arr == ']') return NULL;  // Empty array

    return arr;
}

// toml_array_next moves to next element in inline array.
//
// Parameters:
//   p: Pointer to current element
//
// Returns:
//   Pointer to next element, NULL at end or error
const char* toml_array_next(const char* p) {
    if (!p) return NULL;

    // Skip current value (simple approach - skip to comma or ])
    bool in_string = false;
    int bracket_depth = 0;

    while (*p) {
        if (*p == '"' && (p[-1] != '\\')) {
            in_string = !in_string;
        } else if (!in_string) {
            if (*p == '[') bracket_depth++;
            else if (*p == ']') {
                if (bracket_depth == 0) return NULL;  // End of array
                bracket_depth--;
            }
            else if (*p == ',' && bracket_depth == 0) {
                p++;  // Skip comma
                // Skip whitespace including newlines (for multi-line arrays)
                while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) p++;
                if (*p == ']') return NULL;  // Trailing comma
                return p;
            }
        }
        p++;
    }

    return NULL;  // End of string
}

// toml_count_array counts elements in inline array.
//
// Parameters:
//   arr: Pointer to opening '[' of inline array
//
// Returns:
//   Number of elements, 0 for empty, -1 on error
int32_t toml_count_array(const char* arr) {
    if (!arr || *arr != '[') return -1;

    const char* p = toml_array_first(arr);
    if (!p) return 0;  // Empty array

    int32_t count = 1;
    while ((p = toml_array_next(p)) != NULL) {
        count++;
    }

    return count;
}

// # B.5d Utilities [UTIL]
//
//   Utility functions for error handling.

// toml_get_error_string returns human-readable error description.
//
// Parameters:
//   err: Error code from TomlError enum
//
// Returns:
//   Static string (do not free)
const char* toml_get_error_string(TomlError err) {
    switch (err) {
        case TOML_OK:            return "Success";
        case TOML_ERR_NULL:      return "NULL pointer";
        case TOML_ERR_FILE:      return "File open/read failed";
        case TOML_ERR_SIZE:      return "File too large";
        case TOML_ERR_MEMORY:    return "Memory allocation failed";
        case TOML_ERR_PARSE:     return "Parse error (malformed TOML)";
        case TOML_ERR_NOT_FOUND: return "Section or key not found";
        default:                 return "Unknown error";
    }
}

// # B.6 Omission Guide [OMISSION]
//
//   Guidance for BODY sections in this implementation.
//
//   B.2 Helpers: strip_comments for comment removal
//   B.3 Core Operations: Reserved — all logic in B.5 Public API
//   B.4a-c: Reserved — using fprintf, errors recoverable
//   B.5 Public API: Full implementations (load, find, parse, error)

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
//   Verify TOML implementation correctness.

// # X.1a Build Commands [BUILD]
//
//   gcc -c -Wall -Wextra -std=c11 word/work/pkg/trit/src/toml.c -I word/work/pkg/trit/include -o toml.o
//   make  // Uses project Makefile

// # X.1b Runtime Tests [RUNTIME]
//
//   ./demo-config-reader  // Run demo, configs should load correctly
//   Check stderr for [TOML] warnings  // Should be silent on success

// # X.1c Self-Test [SELF_TEST]
//
//   // test_toml.c — minimal TOML test
//   #include "toml.h"
//   char* toml = toml_load_file("word/core/types/trit.toml");
//   assert(toml != NULL);
//   const char* val = toml_find_key_in_section(toml, "trit", "bits");
//   int32_t bits;
//   assert(toml_parse_int(val, &bits) != NULL);
//   assert(bits == 2);
//   toml_free(toml);

// # X.1d Memory Checks [MEMORY]
//
//   valgrind --leak-check=full ./demo-config-reader  // Check for TOML memory leaks

// # X.2 Execution [EXECUTION]
//
//   How consumers use this implementation (library, not executable).

// # X.2a Include [INCLUDE]
//
//   #include "toml.h"

// # X.2b Exit Codes [EXIT_CODES]
//
//   [Reserved: Library — exit codes in calling executable]

// # X.2c Signal Handling [SIGNALS]
//
//   [Reserved: Library — signal handling in calling executable]

// # X.2d Usage Example [USAGE]
//
//   // Load and parse config file
//   char* toml = toml_load_file("word/core/types/trit.toml");
//   if (!toml) { /* use compile-time defaults */ }
//   // Navigate to section and key
//   const char* val = toml_find_key_in_section(toml, "trit", "bits");
//   int32_t bits;
//   if (val && toml_parse_int(val, &bits)) {
//       /* use bits from config */
//   }
//   toml_free(toml);

// # X.3 Cleanup [CLEANUP]
//
//   Ownership and cleanup for TOML resources.

// # X.3a Ownership [OWNERSHIP]
//
//   toml_load_file allocates, caller frees:
//   - toml_load_file returns heap-allocated string
//   - Caller MUST call toml_free when done
//   - NULL return means no allocation (nothing to free)

// # X.3b Memory Management [MEMORY]
//
//   TOML parser allocates for file content:
//   - toml_load_file: malloc for file + stripped content
//   - toml_free: free the allocated content
//   - Parser itself is stateless

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
//   Modification guidance for TOML implementation.
//
// Safe to Modify:
//   ✅ Add new parse functions (toml_parse_float, toml_parse_datetime)
//   ✅ Add logging/debugging output
//   ✅ Improve error messages
//   ✅ Increase size limits
//
// Modify with Care:
//   ⚠️ Function implementations — must match toml.h declarations
//   ⚠️ Return conventions (NULL on error)
//   ⚠️ Memory ownership (load allocates, free releases)
//
// NEVER Modify:
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Function signatures — must match toml.h exactly
//   ❌ toml_load_file/toml_free ownership contract
//
// Validation: See X.1 for build and memory checks.

// # X.5 Extension [EXTENSION]
//
//   How to extend TOML implementation.
//
// Adding Parse Functions:
//   1. Add function to toml.h (declaration)
//   2. Implement here with NULL checks
//   3. Add to B.5c with docstring
//   4. Update B.1 counts
//
// Adding Value Types:
//   1. Create toml_parse_* function
//   2. Follow existing pattern (skip whitespace, parse, return pointer)
//   3. Return NULL on error
//
// Roadmap: See M.10 for planned extensions.

// # X.6 Troubleshooting [TROUBLESHOOTING]
//
//   Common problems and solutions for TOML implementation.
//
// "[TOML] Cannot open: path":
//   → File not found or no read permission
//   → Check path is correct, file exists
//
// "[TOML] File too large":
//   → File exceeds TOML_FILE_MAX (128KB)
//   → Increase limit or split file
//
// "toml_find_key returns NULL":
//   → Key name must match exactly (case-sensitive)
//   → Use toml_find_section first for keys in sections
//
// "toml_parse_int returns NULL":
//   → Value may be quoted string, not number
//   → Check for leading/trailing whitespace

// # X.7 Reference [REFERENCE]
//
//   Quick lookup — copy-paste ready patterns.
//
// TOML Lifecycle:
//   char* toml = toml_load_file("config.toml");
//   // ... parse values ...
//   toml_free(toml);  // Required cleanup
//
// Section + Key Lookup:
//   const char* val = toml_find_key_in_section(toml, "trit", "bits");
//
// Manual Section Navigation:
//   const char* section = toml_find_section(toml, "trit");
//   const char* val = toml_find_key(section, "bits");
//
// Value Parsing:
//   int32_t num;
//   toml_parse_int(val, &num);
//   char str[256];
//   toml_parse_string(val, str, sizeof(str));
//
// Error Handling:
//   if (!toml) {
//       fprintf(stderr, "Parse: %s\n", toml_get_error_string(TOML_ERR_FILE));
//   }
//
// Compile:
//   gcc -Wall -Wextra -std=c11 main.c toml.c -I include

// # X.8 Closing Note [NOTE]
//
//   Final guidance — configuration enables adaptability.
//
// This file implements TOML parsing — config loading for Kingdom Technology.
// Type specs drive libtrit — this parser is the bridge.
//
// "A word fitly spoken is like apples of gold in pictures of silver."
//   — Proverbs 25:11
//
// Configuration externalizes values; code implements wisdom.

// # X.9 Omission Guide [OMISSION]
//
//   Guidance for CLOSING sections in this implementation.
//
//   All sections present and filled:
//   - X.1 Validation: Build and test commands
//   - X.2 Execution: Usage example (library, no main)
//   - X.3 Cleanup: Memory ownership patterns
//   - X.4 Policy: Modification safety levels
//   - X.5 Extension: How to add parse functions
//   - X.6 Troubleshooting: Parser-specific issues
//   - X.7 Reference: Copy-paste TOML patterns
//   - X.8 Closing Note: Configuration grounding
//
//   X.2b-c: Reserved — library, not executable

// ═══════════════════════════════════════════════════════════════════════════════
// END CLOSING
// ═══════════════════════════════════════════════════════════════════════════════
