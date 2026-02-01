// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-health
// Title: Zone Health Tracking
// Type: Source
// Component: Core
// Role: Track and manage health state per zone

// Status: Active
// Version: a-01.00
// Created: 2026-01-30
// Updated: 2026-01-30

// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC

// Grounding: Proverbs 4:22 — "For they are life unto those that find them,
//            and health to all their flesh."
//
// Health tracking monitors the state of each zone:
//   - Positive health: Zone is thriving, structured, well-maintained
//   - Negative health: Zone is struggling, unstructured, problematic
//   - Neutral health: Zone is in balance

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "zone.h"
#include <string.h>
#include <stdio.h>

// Health file name
#define HEALTH_FILENAME ".health"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Health File Paths
// -----------------------------------------------------------------------------

// Build path to .health file for zone
static void build_health_path(BereshitZone zone, char* buf, size_t size) {
    const char* zone_dir = zone_directory(zone);
    if (zone_dir) {
        snprintf(buf, size, "%s/%s", zone_dir, HEALTH_FILENAME);
    } else {
        buf[0] = '\0';
    }
}

// Build path to .health file for specific directory
static void build_dir_health_path(const char* dir_path, char* buf, size_t size) {
    snprintf(buf, size, "%s/%s", dir_path, HEALTH_FILENAME);
}

// -----------------------------------------------------------------------------
// B.2 Zone-Level Health Operations
// -----------------------------------------------------------------------------

// zone_health_read reads the current health value from .health file.
//
// Health is stored as a single signed byte (-127 to +127).
// Returns 0 if file doesn't exist (neutral health).
//
int8_t zone_health_read(BereshitZone zone) {
    char health_path[BERESHIT_PATH_MAX + 16];
    build_health_path(zone, health_path, sizeof(health_path));

    if (health_path[0] == '\0') {
        return 0;  // Invalid zone
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
//
// Creates or updates the .health file.
// Returns 0 on success, -1 on error.
//
int zone_health_write(BereshitZone zone, int8_t health) {
    char health_path[BERESHIT_PATH_MAX + 16];
    build_health_path(zone, health_path, sizeof(health_path));

    if (health_path[0] == '\0') {
        return -1;  // Invalid zone
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
//
// Reads current health, applies delta, clamps to [-127, +127], writes back.
// Returns the new health value.
//
int8_t zone_health_adjust(BereshitZone zone, int8_t delta) {
    int8_t current = zone_health_read(zone);
    int new_health = current + delta;

    // Clamp to valid range
    if (new_health > BERESHIT_HEALTH_MAX) {
        new_health = BERESHIT_HEALTH_MAX;
    }
    if (new_health < BERESHIT_HEALTH_MIN) {
        new_health = BERESHIT_HEALTH_MIN;
    }

    zone_health_write(zone, (int8_t)new_health);
    return (int8_t)new_health;
}

// -----------------------------------------------------------------------------
// B.3 Directory-Level Health Operations (More Granular)
// -----------------------------------------------------------------------------

// Read health for a specific directory
//
// Each directory can have its own .health file for granular tracking.
// Falls back to zone health if directory .health doesn't exist.
//
int8_t dir_health_read(const char* dir_path) {
    if (!dir_path) return 0;

    char health_path[BERESHIT_PATH_MAX * 2];
    build_dir_health_path(dir_path, health_path, sizeof(health_path));

    FILE* f = fopen(health_path, "rb");
    if (!f) {
        // Fall back to zone health
        BereshitZone zone = zone_from_path(dir_path);
        return zone_health_read(zone);
    }

    int8_t health = 0;
    if (fread(&health, 1, 1, f) != 1) {
        health = 0;
    }
    fclose(f);

    return health;
}

// Write health for a specific directory
//
// Creates or updates the .health file in the directory.
// Returns 0 on success, -1 on error.
//
int dir_health_write(const char* dir_path, int8_t health) {
    if (!dir_path) return -1;

    char health_path[BERESHIT_PATH_MAX * 2];
    build_dir_health_path(dir_path, health_path, sizeof(health_path));

    FILE* f = fopen(health_path, "wb");
    if (!f) {
        return -1;
    }

    size_t written = fwrite(&health, 1, 1, f);
    fclose(f);

    return (written == 1) ? 0 : -1;
}

// Adjust directory health by delta
//
// Reads current, applies delta, clamps, writes back.
// Returns the new health value.
//
int8_t dir_health_adjust(const char* dir_path, int8_t delta) {
    int8_t current = dir_health_read(dir_path);
    int new_health = current + delta;

    // Clamp to valid range
    if (new_health > BERESHIT_HEALTH_MAX) {
        new_health = BERESHIT_HEALTH_MAX;
    }
    if (new_health < BERESHIT_HEALTH_MIN) {
        new_health = BERESHIT_HEALTH_MIN;
    }

    dir_health_write(dir_path, (int8_t)new_health);
    return (int8_t)new_health;
}

// -----------------------------------------------------------------------------
// B.4 Event-Based Health Updates
// -----------------------------------------------------------------------------

// zone_health_success records a successful filesystem operation.
//
// Called when operations complete successfully.
// Small positive adjustment (+1) to encourage healthy patterns.
//
void zone_health_success(const char* path) {
    BereshitZone zone = zone_from_path(path);
    if (zone != BERESHIT_ZONE_UNKNOWN) {
        zone_health_adjust(zone, +1);
    }
}

// zone_health_failure records a failed filesystem operation.
//
// Called when operations fail (permission denied, etc.)
// Small negative adjustment (-1) to track problems.
//
void zone_health_failure(const char* path) {
    BereshitZone zone = zone_from_path(path);
    if (zone != BERESHIT_ZONE_UNKNOWN) {
        zone_health_adjust(zone, -1);
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Health tracking provides insight into zone state.
//
// Like a person's health reflects their lifestyle:
//   - Positive: Well-structured, maintained, complete work
//   - Negative: Unstructured, neglected, problematic
//   - Neutral: In balance, neither growing nor declining
//
// Health feeds into CPI-SI's DAR (Detect-Assess-Restore) system.
// Low health triggers assessment; high health indicates healthy operation.
//
// "Beloved, I wish above all things that thou mayest prosper and be in health,
//  even as thy soul prospereth." — 3 John 1:2

// =============================================================================
// END CLOSING
// =============================================================================
