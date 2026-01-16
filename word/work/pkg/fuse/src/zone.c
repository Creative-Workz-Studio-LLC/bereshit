// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-fuse-zone-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// zone.c — Bereshit Zone Abstraction Implementation
// Ternary zones: void(-1) / word(0) / tov(+1)

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-zone-impl
// Title: Zone Abstraction Implementation
// Type: Source
// Role: Implement ternary zone semantics for filesystem

// Grounding: Genesis 1:1-2 - void (formless) → word (naming) → tov (good)

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "zone.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Bereshit root path (set during mount)
static char bereshit_root[4096] = "";

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// Get/set Bereshit root path

const char* zone_get_bereshit_root(void) {
    return bereshit_root;
}

void zone_set_bereshit_root(const char* path) {
    if (path) {
        strncpy(bereshit_root, path, sizeof(bereshit_root) - 1);
        bereshit_root[sizeof(bereshit_root) - 1] = '\0';
    }
}

// zone_from_path determines which zone a path belongs to.
//
// Path structure: /mount/point/[void|word|tov]/...
// Returns ZONE_UNKNOWN if not in a recognized zone.
BereshitZone zone_from_path(const char* path) {
    if (!path || !*path) {
        return ZONE_UNKNOWN;
    }

    // Skip leading slashes to get to zone name
    while (*path == '/') path++;

    // Check for zone names
    if (strncmp(path, "void", 4) == 0 && (path[4] == '/' || path[4] == '\0')) {
        return ZONE_VOID;
    }
    if (strncmp(path, "word", 4) == 0 && (path[4] == '/' || path[4] == '\0')) {
        return ZONE_WORD;
    }
    if (strncmp(path, "tov", 3) == 0 && (path[3] == '/' || path[3] == '\0')) {
        return ZONE_TOV;
    }

    return ZONE_UNKNOWN;
}

// zone_name returns the string name for a zone.
const char* zone_name(BereshitZone zone) {
    switch (zone) {
        case ZONE_VOID: return "void";
        case ZONE_WORD: return "word";
        case ZONE_TOV:  return "tov";
        default:        return "unknown";
    }
}

// zone_value returns the integer value for a zone.
int zone_value(BereshitZone zone) {
    switch (zone) {
        case ZONE_VOID: return -1;
        case ZONE_WORD: return  0;
        case ZONE_TOV:  return +1;
        default:        return -99;
    }
}

// zone_is_bereshit_path checks if a path is within Bereshit structure.
bool zone_is_bereshit_path(const char* path) {
    return zone_from_path(path) != ZONE_UNKNOWN;
}

// zone_directory returns the actual filesystem path for a zone.
const char* zone_directory(BereshitZone zone) {
    // Buffer large enough for bereshit_root (4096) + "/" (1) + zone name (7) + null
    static char path_buf[4104];

    const char* name = zone_name(zone);
    if (strcmp(name, "unknown") == 0) {
        return NULL;
    }

    snprintf(path_buf, sizeof(path_buf), "%s/%s", bereshit_root, name);
    return path_buf;
}

// zone_load_info loads zone metadata from root.omni file.
//
// Parses root.omni in the zone directory to extract:
//   - key, description from OmniCode pragmas
//   - health from .health file
bool zone_load_info(const char* zone_path, ZoneInfo* info) {
    if (!zone_path || !info) {
        return false;
    }

    // Initialize info
    memset(info, 0, sizeof(*info));
    info->zone = ZONE_UNKNOWN;
    info->health = 0;

    // Determine zone from path - look for zone name anywhere in path
    // This handles both FUSE paths (/void) and full paths (.../Bereshit/void)
    if (strstr(zone_path, "/void") || strcmp(zone_path, "void") == 0) {
        info->zone = ZONE_VOID;
    } else if (strstr(zone_path, "/word") || strcmp(zone_path, "word") == 0) {
        info->zone = ZONE_WORD;
    } else if (strstr(zone_path, "/tov") || strcmp(zone_path, "tov") == 0) {
        info->zone = ZONE_TOV;
    }

    if (info->zone == ZONE_UNKNOWN) {
        return false;
    }

    // Set name
    strncpy(info->name, zone_name(info->zone), sizeof(info->name) - 1);

    // Build path to root.omni
    char root_omni_path[4096];
    snprintf(root_omni_path, sizeof(root_omni_path), "%s/root.omni", zone_path);

    // Try to open root.omni
    FILE* f = fopen(root_omni_path, "r");
    if (!f) {
        info->has_root_omni = false;
        return true;  // Zone exists but no root.omni
    }

    info->has_root_omni = true;

    // Parse root.omni for key and description
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        // Look for meta.key = value
        if (strstr(line, "meta.key")) {
            char* eq = strchr(line, '=');
            if (eq) {
                eq++;
                while (*eq == ' ' || *eq == '\t') eq++;
                // Remove trailing newline
                char* nl = strchr(eq, '\n');
                if (nl) *nl = '\0';
                strncpy(info->key, eq, sizeof(info->key) - 1);
            }
        }
        // Look for description
        if (strstr(line, "description")) {
            char* eq = strchr(line, '=');
            if (eq) {
                eq++;
                while (*eq == ' ' || *eq == '\t') eq++;
                char* nl = strchr(eq, '\n');
                if (nl) *nl = '\0';
                strncpy(info->description, eq, sizeof(info->description) - 1);
            }
        }
    }
    fclose(f);

    // Try to read health from .health file
    char health_path[4096];
    snprintf(health_path, sizeof(health_path), "%s/.health", zone_path);

    FILE* hf = fopen(health_path, "rb");
    if (hf) {
        int8_t health_byte;
        if (fread(&health_byte, 1, 1, hf) == 1) {
            info->health = health_byte;
        }
        fclose(hf);
    }

    return true;
}

// -----------------------------------------------------------------------------
// Zone Policy Functions
// -----------------------------------------------------------------------------

// zone_check_policy enforces zone semantics on filesystem operations.
//
// Zone policies:
//   void (-1): Entry gate - PERMISSIVE
//     - Accepts all operations (formless, awaiting classification)
//     - This is where unstructured work happens
//
//   word (0): Processing - STANDARD
//     - All operations permitted
//     - This is where definitions and implementations live
//
//   tov (+1): Exit gate - STRUCTURED
//     - Read always permitted
//     - Write/Create/Delete require existing structure
//     - This ensures completeness before exit
//
// Returns 0 if allowed, -EACCES or -EPERM if denied
int zone_check_policy(const char* path, ZoneOperation op) {
    BereshitZone zone = zone_from_path(path);

    // Unknown zone - pass through (could be root level)
    if (zone == ZONE_UNKNOWN) {
        return 0;
    }

    // void (-1): Entry gate - all operations permitted
    // "without form, and void" - formless accepts all
    if (zone == ZONE_VOID) {
        return 0;
    }

    // word (0): Processing - all operations permitted
    // Standard operations for definitions and work
    if (zone == ZONE_WORD) {
        return 0;
    }

    // tov (+1): Exit gate - structured operations
    // "And God saw that it was good" - only completed work exits
    if (zone == ZONE_TOV) {
        switch (op) {
            case ZONE_OP_READ:
                // Always allow reading from tov
                return 0;

            case ZONE_OP_WRITE:
            case ZONE_OP_CREATE:
            case ZONE_OP_DELETE:
            case ZONE_OP_MKDIR:
            case ZONE_OP_RMDIR:
                // For now, allow all operations in tov
                // Future: Check if path has root.omni (structured)
                // Future: Validate block structure requirements
                return 0;
        }
    }

    return 0;  // Default: allow
}

// zone_policy_description returns human-readable policy for zone.
const char* zone_policy_description(BereshitZone zone) {
    switch (zone) {
        case ZONE_VOID:
            return "Entry gate - permissive (formless accepts all)";
        case ZONE_WORD:
            return "Processing - standard (definitions and work)";
        case ZONE_TOV:
            return "Exit gate - structured (completion zone)";
        default:
            return "Unknown zone - passthrough";
    }
}

// -----------------------------------------------------------------------------
// Health Tracking Functions
// -----------------------------------------------------------------------------

// Build path to .health file for zone
static void build_health_path(BereshitZone zone, char* buf, size_t size) {
    const char* zone_dir = zone_directory(zone);
    if (zone_dir) {
        snprintf(buf, size, "%s/.health", zone_dir);
    } else {
        buf[0] = '\0';
    }
}

// zone_health_read reads the current health value from .health file.
int8_t zone_health_read(BereshitZone zone) {
    char health_path[4096];
    build_health_path(zone, health_path, sizeof(health_path));

    if (health_path[0] == '\0') {
        return 0;
    }

    FILE* f = fopen(health_path, "rb");
    if (!f) {
        return 0;  // No health file = neutral health
    }

    int8_t health = 0;
    if (fread(&health, 1, 1, f) != 1) {
        health = 0;
    }
    fclose(f);

    return health;
}

// zone_health_write writes the health value to .health file.
int zone_health_write(BereshitZone zone, int8_t health) {
    char health_path[4096];
    build_health_path(zone, health_path, sizeof(health_path));

    if (health_path[0] == '\0') {
        return -1;
    }

    FILE* f = fopen(health_path, "wb");
    if (!f) {
        return -1;
    }

    size_t written = fwrite(&health, 1, 1, f);
    fclose(f);

    return (written == 1) ? 0 : -1;
}

// zone_health_adjust adjusts health by delta, clamping to valid range.
int8_t zone_health_adjust(BereshitZone zone, int8_t delta) {
    int8_t current = zone_health_read(zone);
    int new_health = current + delta;

    // Clamp to valid range
    if (new_health > 127) new_health = 127;
    if (new_health < -127) new_health = -127;

    zone_health_write(zone, (int8_t)new_health);
    return (int8_t)new_health;
}

// zone_health_success records a successful operation.
// Small positive adjustment to encourage good behavior.
void zone_health_success(const char* path) {
    BereshitZone zone = zone_from_path(path);
    if (zone != ZONE_UNKNOWN) {
        // Increment by 1, capped at 127
        zone_health_adjust(zone, 1);
    }
}

// zone_health_failure records a failed operation.
// Small negative adjustment to track problems.
void zone_health_failure(const char* path) {
    BereshitZone zone = zone_from_path(path);
    if (zone != ZONE_UNKNOWN) {
        // Decrement by 1, floored at -127
        zone_health_adjust(zone, -1);
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Validation: Part of libfuse-bereshit
// Closing: "And God saw that it was good" — Genesis 1:10

// =============================================================================
// END CLOSING
// =============================================================================
