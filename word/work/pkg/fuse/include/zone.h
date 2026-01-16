// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-fuse-zone
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// zone.h — Bereshit Zone Abstraction
// Ternary zones: void(-1) / word(0) / tov(+1)

#ifndef BERESHIT_ZONE_H
#define BERESHIT_ZONE_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-zone
// Title: Zone Abstraction Header
// Type: Header
// Role: Define ternary zone semantics for filesystem

// Grounding: Genesis 1:1-2 - void (formless) → word (naming) → tov (good)

// =============================================================================
// END METADATA
// =============================================================================

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

// Zone values follow ternary semantics
typedef enum {
    ZONE_VOID = -1,     // Entry gate - formless, planning, imports
    ZONE_WORD =  0,     // Processing - definitions, implementations
    ZONE_TOV  = +1,     // Exit gate - completion, demos, licenses
    ZONE_UNKNOWN = -99  // Not in a recognized zone
} BereshitZone;

// Zone metadata from root.omni
typedef struct {
    BereshitZone zone;
    char name[64];          // "void", "word", "tov"
    char description[256];  // From root.omni
    char key[128];          // OmniCode key
    int health;             // Current health value
    bool has_root_omni;     // Whether root.omni exists
} ZoneInfo;

// Get zone from path (returns ZONE_UNKNOWN if not in Bereshit)
BereshitZone zone_from_path(const char* path);

// Get zone name
const char* zone_name(BereshitZone zone);

// Get zone value as integer
int zone_value(BereshitZone zone);

// Check if path is within Bereshit structure
bool zone_is_bereshit_path(const char* path);

// Load zone info from root.omni
bool zone_load_info(const char* zone_path, ZoneInfo* info);

// Get zone directory path
const char* zone_directory(BereshitZone zone);

// Get the Bereshit root path (from mount options)
const char* zone_get_bereshit_root(void);

// Set the Bereshit root path (during mount)
void zone_set_bereshit_root(const char* path);

// -----------------------------------------------------------------------------
// Zone Policy Functions
// -----------------------------------------------------------------------------

// Operation types for policy checks
typedef enum {
    ZONE_OP_READ,       // Reading files/directories
    ZONE_OP_WRITE,      // Writing to files
    ZONE_OP_CREATE,     // Creating files
    ZONE_OP_DELETE,     // Deleting files
    ZONE_OP_MKDIR,      // Creating directories
    ZONE_OP_RMDIR       // Removing directories
} ZoneOperation;

// Check if operation is permitted in zone
// Returns 0 if allowed, -errno if denied
int zone_check_policy(const char* path, ZoneOperation op);

// Get policy description for zone
const char* zone_policy_description(BereshitZone zone);

// -----------------------------------------------------------------------------
// Health Tracking Functions
// -----------------------------------------------------------------------------

// Read current health value for zone
// Returns health value (-127 to +127), or 0 if file doesn't exist
int8_t zone_health_read(BereshitZone zone);

// Write health value for zone
// Returns 0 on success, -1 on error
int zone_health_write(BereshitZone zone, int8_t health);

// Adjust health by delta (clamps to -127/+127)
// Returns new health value
int8_t zone_health_adjust(BereshitZone zone, int8_t delta);

// Record successful operation (small positive health adjustment)
void zone_health_success(const char* path);

// Record failed operation (small negative health adjustment)
void zone_health_failure(const char* path);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Validation: Included by bereshit_fs.c
// Closing: "And God saw that it was good" — Genesis 1:10

#endif // BERESHIT_ZONE_H

// =============================================================================
// END CLOSING
// =============================================================================
