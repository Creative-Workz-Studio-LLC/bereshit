// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-policy
// Title: Zone Policy Enforcement
// Type: Source
// Component: Core
// Role: Enforce zone-specific rules for filesystem operations

// Status: Active
// Version: a-01.00
// Created: 2026-01-30
// Updated: 2026-01-30

// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC

// Grounding: Exodus 26:30 — "Set up the tabernacle according to the plan"
//
// Policy enforcement ensures the filesystem operates according to the pattern:
//   - void: Permissive (formless accepts all)
//   - word: Standard (work happens here)
//   - tov: Structured (only completed work exits)

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "zone.h"
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Policy Helper Functions
// -----------------------------------------------------------------------------

// Check if a directory is "structured" (has root.omni)
// Returns true if root.omni exists in the directory
static bool directory_is_structured(const char* dir_path) {
    // Buffer must hold max path + "/root.omni" + null
    char root_omni_path[BERESHIT_PATH_MAX * 2 + 16];
    snprintf(root_omni_path, sizeof(root_omni_path), "%s/root.omni", dir_path);

    FILE* f = fopen(root_omni_path, "r");
    if (f) {
        fclose(f);
        return true;
    }
    return false;
}

// Get parent directory from a path
// Copies parent path to buf, returns pointer to buf
static char* get_parent_directory(const char* path, char* buf, size_t size) {
    strncpy(buf, path, size - 1);
    buf[size - 1] = '\0';

    // Find last slash
    char* last_slash = strrchr(buf, '/');
    if (last_slash && last_slash != buf) {
        *last_slash = '\0';
    } else if (last_slash == buf) {
        // Root directory
        buf[1] = '\0';
    }
    return buf;
}

// Check if filename is "root.omni"
static bool is_root_omni_file(const char* path) {
    const char* filename = strrchr(path, '/');
    if (filename) {
        filename++;  // Skip the slash
    } else {
        filename = path;
    }
    return strcmp(filename, "root.omni") == 0;
}

// Check if directory has any non-hidden children besides root.omni
static bool directory_has_children(const char* dir_path) {
    DIR* d = opendir(dir_path);
    if (!d) return false;

    struct dirent* entry;
    int count = 0;
    while ((entry = readdir(d)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // Skip hidden files like .health
        if (entry->d_name[0] == '.') {
            continue;
        }
        // Skip root.omni itself
        if (strcmp(entry->d_name, "root.omni") == 0) {
            continue;
        }
        count++;
        if (count > 0) {
            closedir(d);
            return true;
        }
    }
    closedir(d);
    return false;
}

// -----------------------------------------------------------------------------
// B.2 Zone Policy Enforcement
// -----------------------------------------------------------------------------

// zone_check_policy enforces zone semantics on filesystem operations.
//
// Zone policies (ENFORCED, not optional):
//
//   void (-1): Entry gate - PERMISSIVE
//     "without form, and void" — Genesis 1:2
//     - All operations permitted
//     - This is where unstructured work happens
//     - Planning, imports, rough drafts
//
//   word (0): Processing - STANDARD
//     "And God said" — Genesis 1:3
//     - All operations permitted
//     - This is where definitions and implementations live
//     - Active work happens here
//
//   tov (+1): Exit gate - STRUCTURED
//     "And God saw that it was good" — Genesis 1:4
//     - Read: Always permitted
//     - Create file: Only if creating root.omni OR parent has root.omni
//     - Write: Permitted (file already in structured context)
//     - Delete root.omni: Blocked if directory has children
//     - Mkdir: Permitted (user should add root.omni)
//     - Rmdir: Only empty directories
//
// Returns 0 if allowed, -EACCES if denied by policy
//
int zone_check_policy(const char* path, ZoneOperation op) {
    BereshitZone zone = zone_from_path(path);

    // Unknown zone - pass through (could be root level)
    if (zone == BERESHIT_ZONE_UNKNOWN) {
        return 0;
    }

    // void (-1): Entry gate - all operations permitted
    // "without form, and void" - formless accepts all
    if (zone == BERESHIT_ZONE_VOID) {
        return 0;
    }

    // word (0): Processing - all operations permitted
    // Standard operations for definitions and work
    if (zone == BERESHIT_ZONE_WORD) {
        return 0;
    }

    // tov (+1): Exit gate - structured operations
    // "And God saw that it was good" - only completed work exits
    if (zone == BERESHIT_ZONE_TOV) {
        // Build full source path for checks
        char source_path[BERESHIT_PATH_MAX * 2];
        const char* root = zone_get_bereshit_root();
        snprintf(source_path, sizeof(source_path), "%s%s", root, path);

        switch (op) {
            case BERESHIT_OP_READ:
                // Always allow reading from tov
                return 0;

            case BERESHIT_OP_WRITE:
                // Writing to existing files is allowed
                // The file already exists in a (presumably) structured context
                return 0;

            case BERESHIT_OP_CREATE: {
                // Creating root.omni is always allowed - it ESTABLISHES structure
                if (is_root_omni_file(path)) {
                    return 0;
                }

                // For other files, parent directory must be structured
                char parent[BERESHIT_PATH_MAX * 2];
                get_parent_directory(source_path, parent, sizeof(parent));

                if (!directory_is_structured(parent)) {
                    // Parent lacks root.omni - creation denied
                    // "Only completed work exits" - establish structure first
                    return -EACCES;
                }
                return 0;
            }

            case BERESHIT_OP_DELETE: {
                // Deleting root.omni is blocked if directory has other content
                // This prevents orphaning structured content
                if (is_root_omni_file(path)) {
                    char parent[BERESHIT_PATH_MAX * 2];
                    get_parent_directory(source_path, parent, sizeof(parent));

                    if (directory_has_children(parent)) {
                        // Cannot remove structure while content exists
                        // "God saw that it was good" - don't break what's complete
                        return -EACCES;
                    }
                }
                // Other deletes allowed - caller maintains integrity
                return 0;
            }

            case BERESHIT_OP_MKDIR:
                // Creating directories is allowed
                // Health will be tracked - unstructured dirs hurt health
                // User should add root.omni to establish structure
                return 0;

            case BERESHIT_OP_RMDIR:
                // Only empty directories can be removed
                // (Standard filesystem semantics handle this)
                return 0;
        }
    }

    return 0;  // Default: allow
}

// -----------------------------------------------------------------------------
// B.3 Policy Descriptions
// -----------------------------------------------------------------------------

// zone_policy_description returns human-readable policy for zone.
const char* zone_policy_description(BereshitZone zone) {
    switch (zone) {
        case BERESHIT_ZONE_VOID:
            return "Entry gate - permissive (formless accepts all)";
        case BERESHIT_ZONE_WORD:
            return "Processing - standard (definitions and work)";
        case BERESHIT_ZONE_TOV:
            return "Exit gate - structured (requires root.omni for creates)";
        default:
            return "Unknown zone - passthrough";
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Policy enforcement is the LAW of the filesystem.
//
// Like the tabernacle had specific rules for what could go where,
// Bereshit FS has rules for what can exist in each zone:
//
//   void: Anything can enter (the outer court)
//   word: Work happens here (the holy place)
//   tov: Only completed work exits (the holy of holies)
//
// "Set up the tabernacle according to the plan shown you on the mountain."
//   — Exodus 26:30

// =============================================================================
// END CLOSING
// =============================================================================
