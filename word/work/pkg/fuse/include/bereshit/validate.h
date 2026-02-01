// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-validate
// Title: Bereshit Structure Validation
// Type: Header
// Component: Core
// Role: Validate file structure, naming conventions, and block patterns

// Status: Active
// Version: a-01.00
// Created: 2026-01-30
// Updated: 2026-01-30

// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC

// Grounding: Ezekiel 43:10-11 — "...that they may be ashamed of their iniquities:
//            and let them measure the pattern."
//
// Validation measures files against the pattern:
//   - Block structure (3/4/5 blocks)
//   - Naming conventions (snake_case, kebab-case)
//   - OmniCode pragmas (#!omni)
//   - Directory organization (zones)

// =============================================================================
// END METADATA
// =============================================================================

#ifndef BERESHIT_VALIDATE_H
#define BERESHIT_VALIDATE_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "bereshit/types.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Naming Convention Patterns
// -----------------------------------------------------------------------------

// Bereshit naming conventions (consistent across bereshit/cornerstone/millenniumos):
//
// DIRECTORIES:
//   - Always lowercase
//   - No spaces, no uppercase
//   - Examples: void/, word/, tov/, include/, src/, build/
//
// C FILES (source and header):
//   - snake_case for compound names: display_config.c, bereshit_fs.c
//   - Lowercase single words: zone.h, trit.h, types.h
//   - Layer notation with dots: display.hal.h, audio.drv.c
//
// DOCUMENTATION:
//   - kebab-case for markdown: millenniumos-boot-sequence.md
//   - kebab-case for asciidoc: building-block-method.adoc
//
// OMNICODE KEYS:
//   - B- prefix for Bereshit
//   - Hyphenated path-like: B-word-work-pkg-fuse-zone
//
// SPECIAL FILES:
//   - root.omni — directory metadata (standard name)
//   - .health — health tracking (hidden file)
//   - Makefile — build configuration (standard name)
//   - README.md — documentation (standard name)

// Check naming style of a filename (not full path)
//
// Returns:
//   BERESHIT_NAME_SNAKE: snake_case (my_file.c)
//   BERESHIT_NAME_KEBAB: kebab-case (my-file.md)
//   BERESHIT_NAME_PASCAL: PascalCase (MyFile.h)
//   BERESHIT_NAME_INVALID: Contains spaces or invalid characters
//
BereshitNamingStyle bereshit_validate_naming_style(const char* filename);

// Check if filename follows conventions for its extension
//
// Conventions:
//   .c, .h: snake_case or single lowercase word, may have .hal., .drv., .svc.
//   .md, .adoc: kebab-case
//   .omni: lowercase, typically root.omni
//   .toml, .jsonc: kebab-case or snake_case
//
// Returns true if filename matches expected convention
//
bool bereshit_validate_filename(const char* filename);

// Check if directory name follows conventions
//
// Requirements:
//   - All lowercase
//   - No spaces
//   - Alphanumeric, hyphens, underscores only
//
bool bereshit_validate_dirname(const char* dirname);

// -----------------------------------------------------------------------------
// B.2 Block Structure Validation
// -----------------------------------------------------------------------------

// Block structure patterns:
//
// 3-BLOCK (data files: .toml, .jsonc, configs):
//   METADATA → CONTENT → CLOSING
//
// 4-BLOCK (code files: .c, .h, .go, .rs):
//   METADATA → SETUP → BODY → CLOSING
//
// 5-BLOCK (documentation: .md, .adoc):
//   METADATA → HEADER → CONTEXT → CONTENT → FOOTER

// Detect block type from file content
//
// Scans file for block markers: [METADATA], [SETUP], [BODY], etc.
// Returns detected block type or BERESHIT_BLOCK_UNKNOWN
//
BereshitBlockType bereshit_validate_detect_block_type(const char* content,
                                                       size_t length);

// Detect block type from file path
//
// Opens file, reads content, detects block type
// Returns BERESHIT_BLOCK_UNKNOWN on error
//
BereshitBlockType bereshit_validate_detect_block_type_path(const char* path);

// Validate file has expected block structure
//
// Checks:
//   - Has all required blocks for its type
//   - Blocks are in correct order
//   - Block markers are properly formatted
//
// Fills result struct with validation details
//
BereshitResult bereshit_validate_block_structure(const char* path,
                                                  BereshitValidation* result);

// -----------------------------------------------------------------------------
// B.3 OmniCode Validation
// -----------------------------------------------------------------------------

// Check if file has OmniCode pragma
//
// Looks for #!omni at start of file
// Returns true if pragma found
//
bool bereshit_validate_has_pragma(const char* path);

// Extract OmniCode key from file
//
// Parses #!omni meta.key = ... or Key: ... in METADATA block
// Copies key to buf, returns length or -1 on error
//
ssize_t bereshit_validate_extract_key(const char* path, char* buf, size_t size);

// Validate OmniCode key format
//
// Valid format: B-word-work-pkg-name-file
//   - Starts with B- (for Bereshit)
//   - Hyphenated segments
//   - Matches path structure
//
bool bereshit_validate_key_format(const char* key);

// Check if key matches file path
//
// Compares key path segments against actual file path
// Returns true if key accurately reflects location
//
bool bereshit_validate_key_matches_path(const char* key, const char* path);

// -----------------------------------------------------------------------------
// B.4 Full File Validation
// -----------------------------------------------------------------------------

// Validate a file completely
//
// Checks:
//   1. Filename follows naming conventions
//   2. Has OmniCode pragma (if code/doc file)
//   3. Block structure is correct
//   4. Key matches path (if key present)
//
// Fills result struct with all validation details
//
BereshitResult bereshit_validate_file(const char* path,
                                       BereshitValidation* result);

// Validate a directory
//
// Checks:
//   1. Directory name follows conventions
//   2. Has root.omni if in tov zone
//   3. root.omni has valid structure
//
BereshitResult bereshit_validate_directory(const char* path,
                                            BereshitValidation* result);

// -----------------------------------------------------------------------------
// B.5 Cornerstone Integration
// -----------------------------------------------------------------------------

// File type categories for cornerstone integration
typedef enum {
    BERESHIT_FILE_HAL,      // Hardware abstraction: .hal.h, .hal.c
    BERESHIT_FILE_DRV,      // Driver: .drv.c
    BERESHIT_FILE_SVC,      // Service: _service.c, .svc.c
    BERESHIT_FILE_UI,       // UI component: menu.c, dialog.c
    BERESHIT_FILE_CORE,     // Core implementation: trit.c, zone.c
    BERESHIT_FILE_TEST,     // Test file: _test.c, test_*.c
    BERESHIT_FILE_CONFIG,   // Configuration: .toml, .jsonc
    BERESHIT_FILE_DOC,      // Documentation: .md, .adoc
    BERESHIT_FILE_DATA,     // Data file: .omni, .json
    BERESHIT_FILE_OTHER     // Unknown type
} BereshitFileCategory;

// Categorize file by cornerstone layer/type
BereshitFileCategory bereshit_validate_categorize_file(const char* filename);

// Get expected block type for file category
BereshitBlockType bereshit_validate_expected_block_type(BereshitFileCategory cat);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Validation: Included by bereshit/bereshit.h
//
// Integration with build system:
//   - Makefile can call validation before compile
//   - CI/CD can enforce structure across project
//   - Editor plugins can show validation warnings
//
// Integration with cornerstone:
//   - HAL files: display.hal.h → BERESHIT_FILE_HAL
//   - Services: dialogue_service.c → BERESHIT_FILE_SVC
//   - Drivers: ahci.drv.c → BERESHIT_FILE_DRV
//
// "Son of man, shew the house to the house of Israel, that they may be
//  ashamed of their iniquities: and let them measure the pattern."
//   — Ezekiel 43:10
//
// Files measured against the pattern reflect the order of creation.

#endif // BERESHIT_VALIDATE_H

// =============================================================================
// END CLOSING
// =============================================================================
