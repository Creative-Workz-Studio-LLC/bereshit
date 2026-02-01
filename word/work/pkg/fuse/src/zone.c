// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-zone
// Title: Zone Detection and Management
// Type: Source
// Component: Core
// Role: Detect zones and manage Bereshit root path

// Status: Active
// Version: a-02.00
// Created: 2026-01-15
// Updated: 2026-01-30

// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC

// Grounding: Genesis 1:1-2 — void (formless) → word (naming) → tov (good)
//
// This file handles ONLY zone detection and root management.
// Policy enforcement is in policy.c
// Health tracking is in health.c

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "zone.h"
#include <string.h>
#include <stdio.h>

// Bereshit root path (set during mount)
static char bereshit_root[BERESHIT_PATH_MAX] = "";

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Root Path Management
// -----------------------------------------------------------------------------

const char* zone_get_bereshit_root(void) {
    return bereshit_root;
}

void zone_set_bereshit_root(const char* path) {
    if (path) {
        strncpy(bereshit_root, path, sizeof(bereshit_root) - 1);
        bereshit_root[sizeof(bereshit_root) - 1] = '\0';
    }
}

// -----------------------------------------------------------------------------
// B.2 Zone Detection
// -----------------------------------------------------------------------------

// zone_from_path determines which zone a path belongs to.
//
// Path structure: /[void|word|tov]/...
// Returns BERESHIT_ZONE_UNKNOWN if not in a recognized zone.
//
BereshitZone zone_from_path(const char* path) {
    if (!path || !*path) {
        return BERESHIT_ZONE_UNKNOWN;
    }

    // Skip leading slashes to get to zone name
    while (*path == '/') path++;

    // Check for zone names
    // void (-1): "without form, and void" — Genesis 1:2
    if (strncmp(path, "void", 4) == 0 && (path[4] == '/' || path[4] == '\0')) {
        return BERESHIT_ZONE_VOID;
    }

    // word (0): "And God said" — Genesis 1:3
    if (strncmp(path, "word", 4) == 0 && (path[4] == '/' || path[4] == '\0')) {
        return BERESHIT_ZONE_WORD;
    }

    // tov (+1): "And God saw that it was good" — Genesis 1:4
    if (strncmp(path, "tov", 3) == 0 && (path[3] == '/' || path[3] == '\0')) {
        return BERESHIT_ZONE_TOV;
    }

    return BERESHIT_ZONE_UNKNOWN;
}

// -----------------------------------------------------------------------------
// B.3 Zone Properties
// -----------------------------------------------------------------------------

// zone_name returns the string name for a zone.
const char* zone_name(BereshitZone zone) {
    switch (zone) {
        case BERESHIT_ZONE_VOID: return "void";
        case BERESHIT_ZONE_WORD: return "word";
        case BERESHIT_ZONE_TOV:  return "tov";
        default:                  return "unknown";
    }
}

// zone_value returns the integer value for a zone.
int zone_value(BereshitZone zone) {
    switch (zone) {
        case BERESHIT_ZONE_VOID: return -1;
        case BERESHIT_ZONE_WORD: return  0;
        case BERESHIT_ZONE_TOV:  return +1;
        default:                  return -99;
    }
}

// zone_is_bereshit_path checks if a path is within Bereshit structure.
bool zone_is_bereshit_path(const char* path) {
    return zone_from_path(path) != BERESHIT_ZONE_UNKNOWN;
}

// zone_directory returns the actual filesystem path for a zone.
const char* zone_directory(BereshitZone zone) {
    // Buffer: root (4096) + "/" (1) + zone name (7) + null
    static char path_buf[BERESHIT_PATH_MAX + 8];

    const char* name = zone_name(zone);
    if (strcmp(name, "unknown") == 0) {
        return NULL;
    }

    snprintf(path_buf, sizeof(path_buf), "%s/%s", bereshit_root, name);
    return path_buf;
}

// -----------------------------------------------------------------------------
// B.4 Zone Information Loading
// -----------------------------------------------------------------------------

// zone_load_info loads zone metadata from root.omni file.
bool zone_load_info(const char* zone_path, ZoneInfo* info) {
    if (!zone_path || !info) {
        return false;
    }

    // Initialize info
    memset(info, 0, sizeof(*info));
    info->zone = BERESHIT_ZONE_UNKNOWN;
    info->health = 0;

    // Determine zone from path
    if (strstr(zone_path, "/void") || strcmp(zone_path, "void") == 0) {
        info->zone = BERESHIT_ZONE_VOID;
    } else if (strstr(zone_path, "/word") || strcmp(zone_path, "word") == 0) {
        info->zone = BERESHIT_ZONE_WORD;
    } else if (strstr(zone_path, "/tov") || strcmp(zone_path, "tov") == 0) {
        info->zone = BERESHIT_ZONE_TOV;
    }

    if (info->zone == BERESHIT_ZONE_UNKNOWN) {
        return false;
    }

    // Set name
    strncpy(info->name, zone_name(info->zone), sizeof(info->name) - 1);

    // Build path to root.omni
    char root_omni_path[BERESHIT_PATH_MAX];
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

    // Health is loaded separately by health.c functions
    // But for backward compatibility, try to read it here too
    char health_path[BERESHIT_PATH_MAX];
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

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// This file handles ONLY zone detection and root management.
//
// The separation follows Genesis 1 pattern:
//   - zone.c: WHAT zones exist (detection)
//   - policy.c: HOW zones behave (rules)
//   - health.c: WHAT STATE zones are in (tracking)
//
// "And God called the light Day, and the darkness he called Night."
//   — Genesis 1:5
//
// Naming and identification is the foundation of order.

// =============================================================================
// END CLOSING
// =============================================================================
