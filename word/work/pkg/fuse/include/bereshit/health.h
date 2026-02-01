// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-health
// Title: Bereshit Health Tracking
// Type: Header
// Component: Core
// Role: Track and manage health state per zone and directory

// Status: Active
// Version: a-01.00
// Created: 2026-01-30
// Updated: 2026-01-30

// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC

// Grounding: Proverbs 4:22 — "For they are life unto those that find them,
//            and health to all their flesh."
//
// Health tracking mirrors the Hebrew concept of shalom (wholeness):
//   - Positive health: System is whole, integrated, functioning
//   - Negative health: System is fragmented, broken, struggling
//   - Neutral health: System is in balance, neither growing nor declining

// =============================================================================
// END METADATA
// =============================================================================

#ifndef BERESHIT_HEALTH_H
#define BERESHIT_HEALTH_H

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
// B.1 Health Constants
// -----------------------------------------------------------------------------

// Health adjustment values
#define BERESHIT_HEALTH_SUCCESS_DELTA   1   // Small positive for success
#define BERESHIT_HEALTH_FAILURE_DELTA  -1   // Small negative for failure
#define BERESHIT_HEALTH_STRUCTURE_BONUS 5   // Bonus for adding root.omni
#define BERESHIT_HEALTH_ORPHAN_PENALTY -5   // Penalty for unstructured in tov

// Health file name (hidden file in each zone/directory)
#define BERESHIT_HEALTH_FILENAME ".health"

// -----------------------------------------------------------------------------
// B.2 Zone-Level Health
// -----------------------------------------------------------------------------

// Read current health value for a zone
//
// Health is stored in .health file in zone root directory.
// Range: -127 to +127 (signed byte)
// Returns 0 if file doesn't exist (neutral health)
//
int8_t bereshit_health_zone_read(BereshitZone zone);

// Write health value for a zone
//
// Creates or updates .health file in zone root.
// Value is clamped to valid range.
//
// Returns:
//   BERESHIT_OK on success
//   BERESHIT_ERR_ZONE if zone is invalid
//   BERESHIT_ERR_IO on write error
//
BereshitResult bereshit_health_zone_write(BereshitZone zone, int8_t health);

// Adjust zone health by delta
//
// Reads current health, applies delta, clamps to range, writes back.
// Returns the new health value.
//
int8_t bereshit_health_zone_adjust(BereshitZone zone, int8_t delta);

// -----------------------------------------------------------------------------
// B.3 Directory-Level Health (More Granular)
// -----------------------------------------------------------------------------

// Read health for a specific directory
//
// Each directory can have its own .health file.
// Falls back to zone health if directory .health doesn't exist.
//
int8_t bereshit_health_dir_read(const char* dir_path);

// Write health for a specific directory
BereshitResult bereshit_health_dir_write(const char* dir_path, int8_t health);

// Adjust directory health by delta
int8_t bereshit_health_dir_adjust(const char* dir_path, int8_t delta);

// -----------------------------------------------------------------------------
// B.4 Event-Based Health Updates
// -----------------------------------------------------------------------------

// Record a successful filesystem operation
//
// Called when operations complete successfully.
// Small positive adjustment to encourage healthy patterns.
// Affects the zone containing the path.
//
void bereshit_health_record_success(const char* path);

// Record a failed filesystem operation
//
// Called when operations fail (permission denied, etc.)
// Small negative adjustment to track problems.
// Affects the zone containing the path.
//
void bereshit_health_record_failure(const char* path);

// Record structure establishment
//
// Called when root.omni is created in a directory.
// Larger positive adjustment - structure is valuable.
//
void bereshit_health_record_structure(const char* path);

// Record orphaned content
//
// Called when unstructured content is created in tov.
// Negative adjustment - tov should be structured.
//
void bereshit_health_record_orphan(const char* path);

// -----------------------------------------------------------------------------
// B.5 Health Interpretation
// -----------------------------------------------------------------------------

// Health state categories (maps to Hebrew states)
typedef enum {
    BERESHIT_HEALTH_SHAVAR  = -1,   // Broken (-127 to -64)
    BERESHIT_HEALTH_CHASER  = -1,   // Lacking (-63 to -32)
    BERESHIT_HEALTH_RATSAH  = -1,   // Wanting (-31 to -1)
    BERESHIT_HEALTH_YASHAR  =  0,   // Even (0)
    BERESHIT_HEALTH_TAMIM   = +1,   // Sound (1 to 31)
    BERESHIT_HEALTH_SHALEM  = +1,   // Whole (32 to 63)
    BERESHIT_HEALTH_TOV     = +1    // Good/Perfect (64 to 127)
} BereshitHealthState;

// Get health state category from value
BereshitHealthState bereshit_health_get_state(int8_t health);

// Get human-readable state name
const char* bereshit_health_state_name(BereshitHealthState state);

// Get health state description
const char* bereshit_health_state_description(BereshitHealthState state);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Validation: Included by bereshit/bereshit.h
//
// Integration with cornerstone:
//   - Health feeds into CPI-SI DAR (Detect-Assess-Restore)
//   - Low health triggers assessment and potential restoration
//   - High health indicates system is functioning as designed
//
// Integration with Hebrew states:
//   - shavar (BROKEN): -1.0 edge, crisis state
//   - yashar (EVEN): 0.0 center, balanced
//   - tov (PERFECT): +1.0 edge, completion
//
// "Beloved, I wish above all things that thou mayest prosper and be in health,
//  even as thy soul prospereth." — 3 John 1:2

#endif // BERESHIT_HEALTH_H

// =============================================================================
// END CLOSING
// =============================================================================
