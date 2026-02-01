// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-types
// Title: Bereshit Filesystem Core Types
// Type: Header
// Component: Foundation
// Role: Define fundamental types for Bereshit FS

// Status: Active
// Version: a-01.00
// Created: 2026-01-30
// Updated: 2026-01-30

// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC

// Grounding: Genesis 1:1 - "In the beginning God created"

// =============================================================================
// END METADATA
// =============================================================================

#ifndef BERESHIT_TYPES_H
#define BERESHIT_TYPES_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Constants
// -----------------------------------------------------------------------------

// Maximum path length (consistent with Linux PATH_MAX)
#define BERESHIT_PATH_MAX 4096

// Maximum key length for OmniCode keys
#define BERESHIT_KEY_MAX 128

// Maximum description length
#define BERESHIT_DESC_MAX 256

// Maximum zone name length
#define BERESHIT_ZONE_NAME_MAX 64

// Health value range
#define BERESHIT_HEALTH_MIN -127
#define BERESHIT_HEALTH_MAX +127

// -----------------------------------------------------------------------------
// B.2 Zone Types (Ternary Semantics)
// -----------------------------------------------------------------------------

// Zone values follow ternary semantics from Genesis 1:1-2
//
//   void (-1): "without form, and void" — Entry gate
//              Formless, accepts all, imports, planning
//
//   word (0):  "And God said" — Processing
//              Naming, definitions, implementations
//
//   tov (+1):  "And God saw that it was good" — Exit gate
//              Completion, structured, verified
//
typedef enum {
    BERESHIT_ZONE_VOID    = -1,     // Entry gate - formless
    BERESHIT_ZONE_WORD    =  0,     // Processing - definitions
    BERESHIT_ZONE_TOV     = +1,     // Exit gate - completion
    BERESHIT_ZONE_UNKNOWN = -99     // Not in a recognized zone
} BereshitZone;

// -----------------------------------------------------------------------------
// B.3 Operation Types
// -----------------------------------------------------------------------------

// Filesystem operations for policy checking
typedef enum {
    BERESHIT_OP_READ,       // Reading files/directories
    BERESHIT_OP_WRITE,      // Writing to files
    BERESHIT_OP_CREATE,     // Creating files
    BERESHIT_OP_DELETE,     // Deleting files
    BERESHIT_OP_MKDIR,      // Creating directories
    BERESHIT_OP_RMDIR       // Removing directories
} BereshitOperation;

// -----------------------------------------------------------------------------
// B.4 Block Types (Structure Validation)
// -----------------------------------------------------------------------------

// Block structure types for file validation
//
//   3-Block: METADATA → CONTENT → CLOSING
//            Used for data files (configs, settings)
//
//   4-Block: METADATA → SETUP → BODY → CLOSING
//            Used for code files (executables, libraries)
//
//   5-Block: METADATA → HEADER → CONTEXT → CONTENT → FOOTER
//            Used for documentation (for humans)
//
typedef enum {
    BERESHIT_BLOCK_3 = 3,   // Data files
    BERESHIT_BLOCK_4 = 4,   // Code files
    BERESHIT_BLOCK_5 = 5,   // Documentation
    BERESHIT_BLOCK_UNKNOWN = 0
} BereshitBlockType;

// -----------------------------------------------------------------------------
// B.5 Result Codes
// -----------------------------------------------------------------------------

// Function result codes
typedef enum {
    BERESHIT_OK            =  0,    // Success
    BERESHIT_ERR_NULL      = -1,    // NULL pointer
    BERESHIT_ERR_PATH      = -2,    // Invalid path
    BERESHIT_ERR_ZONE      = -3,    // Invalid zone
    BERESHIT_ERR_POLICY    = -4,    // Policy violation
    BERESHIT_ERR_STRUCTURE = -5,    // Structure violation
    BERESHIT_ERR_IO        = -6,    // I/O error
    BERESHIT_ERR_HEALTH    = -7,    // Health tracking error
    BERESHIT_ERR_XATTR     = -8     // Extended attribute error
} BereshitResult;

// -----------------------------------------------------------------------------
// B.6 Info Structures
// -----------------------------------------------------------------------------

// Zone information structure
typedef struct {
    BereshitZone zone;              // Which zone
    char name[BERESHIT_ZONE_NAME_MAX];      // "void", "word", "tov"
    char description[BERESHIT_DESC_MAX];    // From root.omni
    char key[BERESHIT_KEY_MAX];             // OmniCode key
    int8_t health;                  // Current health (-127 to +127)
    bool has_root_omni;             // Whether root.omni exists
    bool is_structured;             // Has root.omni in this dir
} BereshitZoneInfo;

// File validation result
typedef struct {
    bool valid;                     // Passes validation
    BereshitBlockType block_type;   // Detected block type
    bool has_pragma;                // Has #!omni pragma
    bool has_metadata;              // Has METADATA block
    bool has_closing;               // Has CLOSING block
    char key[BERESHIT_KEY_MAX];     // OmniCode key if found
    char message[BERESHIT_DESC_MAX]; // Validation message
} BereshitValidation;

// -----------------------------------------------------------------------------
// B.7 Naming Convention Types
// -----------------------------------------------------------------------------

// File naming style
typedef enum {
    BERESHIT_NAME_KEBAB,    // kebab-case (preferred for docs)
    BERESHIT_NAME_SNAKE,    // snake_case (preferred for code)
    BERESHIT_NAME_PASCAL,   // PascalCase (types, structs)
    BERESHIT_NAME_INVALID   // Contains spaces or invalid chars
} BereshitNamingStyle;

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Validation: Included by bereshit/bereshit.h
// Closing: "In the beginning God created the heaven and the earth." — Genesis 1:1

#endif // BERESHIT_TYPES_H

// =============================================================================
// END CLOSING
// =============================================================================
