// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-zone-compat
// Title: Zone Compatibility Header
// Type: Header
// Component: Compatibility
// Role: Backward compatibility - forwards to bereshit/zone.h

// Status: Active (Compatibility Layer)
// Version: a-02.00
// Created: 2026-01-15
// Updated: 2026-01-30

// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC

// NOTE: This header provides backward compatibility.
// New code should use: #include "bereshit/bereshit.h"

// =============================================================================
// END METADATA
// =============================================================================

#ifndef BERESHIT_ZONE_COMPAT_H
#define BERESHIT_ZONE_COMPAT_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// Include the canonical types
#include "bereshit/types.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Type Aliases (Old → New)
// -----------------------------------------------------------------------------

// Zone values - old names map to new
#define ZONE_VOID     BERESHIT_ZONE_VOID
#define ZONE_WORD     BERESHIT_ZONE_WORD
#define ZONE_TOV      BERESHIT_ZONE_TOV
#define ZONE_UNKNOWN  BERESHIT_ZONE_UNKNOWN

// Operation types - old names map to new
#define ZONE_OP_READ    BERESHIT_OP_READ
#define ZONE_OP_WRITE   BERESHIT_OP_WRITE
#define ZONE_OP_CREATE  BERESHIT_OP_CREATE
#define ZONE_OP_DELETE  BERESHIT_OP_DELETE
#define ZONE_OP_MKDIR   BERESHIT_OP_MKDIR
#define ZONE_OP_RMDIR   BERESHIT_OP_RMDIR

// Old type aliases
typedef BereshitOperation ZoneOperation;

// Old ZoneInfo structure (compatible with BereshitZoneInfo)
typedef struct {
    BereshitZone zone;
    char name[64];          // "void", "word", "tov"
    char description[256];  // From root.omni
    char key[128];          // OmniCode key
    int health;             // Current health value (int for backward compat)
    bool has_root_omni;     // Whether root.omni exists
} ZoneInfo;

// -----------------------------------------------------------------------------
// B.2 Function Declarations (Old API)
// -----------------------------------------------------------------------------

// Zone detection
BereshitZone zone_from_path(const char* path);
const char* zone_name(BereshitZone zone);
int zone_value(BereshitZone zone);
bool zone_is_bereshit_path(const char* path);
const char* zone_directory(BereshitZone zone);

// Zone root management
const char* zone_get_bereshit_root(void);
void zone_set_bereshit_root(const char* path);

// Zone info
bool zone_load_info(const char* zone_path, ZoneInfo* info);

// Zone policy
int zone_check_policy(const char* path, ZoneOperation op);
const char* zone_policy_description(BereshitZone zone);

// Health tracking
int8_t zone_health_read(BereshitZone zone);
int zone_health_write(BereshitZone zone, int8_t health);
int8_t zone_health_adjust(BereshitZone zone, int8_t delta);
void zone_health_success(const char* path);
void zone_health_failure(const char* path);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// NOTE: This is a COMPATIBILITY header.
//
// For new code, use the canonical API:
//   #include "bereshit/bereshit.h"
//
// The old API (zone_*) continues to work but the new API (bereshit_zone_*)
// is preferred for consistency with libtrit (trit_*) and libomni (omni_*).
//
// "And God saw that it was good" — Genesis 1:10

#endif // BERESHIT_ZONE_COMPAT_H

// =============================================================================
// END CLOSING
// =============================================================================
