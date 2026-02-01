// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-xattr
// Title: Bereshit Extended Attributes
// Type: Header
// Component: Core
// Role: Expose zone metadata through filesystem extended attributes

// Status: Active
// Version: a-01.00
// Created: 2026-01-30
// Updated: 2026-01-30

// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC

// Grounding: Exodus 28:36 — "And thou shalt make a plate of pure gold,
//            and grave upon it, like the engravings of a signet, HOLINESS TO THE LORD."
//
// Extended attributes are the "engravings" on each file — marking what it is,
// where it belongs, and its state. Every file carries its identity.

// =============================================================================
// END METADATA
// =============================================================================

#ifndef BERESHIT_XATTR_H
#define BERESHIT_XATTR_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "bereshit/types.h"
#include <sys/types.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Extended Attribute Names
// -----------------------------------------------------------------------------

// All Bereshit extended attributes use the "user.bereshit." namespace
// This follows Linux xattr conventions for user-defined attributes

#define BERESHIT_XATTR_PREFIX "user.bereshit."

// Zone information
#define BERESHIT_XATTR_ZONE        "user.bereshit.zone"        // "void", "word", "tov"
#define BERESHIT_XATTR_ZONE_VALUE  "user.bereshit.zone.value"  // "-1", "0", "+1"

// Health information
#define BERESHIT_XATTR_HEALTH      "user.bereshit.health"      // "-127" to "+127"
#define BERESHIT_XATTR_HEALTH_STATE "user.bereshit.health.state" // "yashar", "tov", etc.

// Structure information
#define BERESHIT_XATTR_STRUCTURED  "user.bereshit.structured"  // "true" or "false"
#define BERESHIT_XATTR_KEY         "user.bereshit.key"         // OmniCode key

// Block structure (for files)
#define BERESHIT_XATTR_BLOCK_TYPE  "user.bereshit.block.type"  // "3", "4", "5"
#define BERESHIT_XATTR_HAS_PRAGMA  "user.bereshit.pragma"      // "true" or "false"

// Naming convention
#define BERESHIT_XATTR_NAME_STYLE  "user.bereshit.name.style"  // "snake", "kebab", etc.

// -----------------------------------------------------------------------------
// B.2 Get Extended Attributes
// -----------------------------------------------------------------------------

// Get the zone for a path as a string
//
// Returns zone name in buf: "void", "word", "tov", or "unknown"
// Returns length of string (not including null terminator)
// Returns -1 on error
//
ssize_t bereshit_xattr_get_zone(const char* path, char* buf, size_t size);

// Get the zone value for a path as a string
//
// Returns zone value in buf: "-1", "0", "+1"
//
ssize_t bereshit_xattr_get_zone_value(const char* path, char* buf, size_t size);

// Get health for a path as a string
//
// Returns health value in buf: "-127" to "+127"
//
ssize_t bereshit_xattr_get_health(const char* path, char* buf, size_t size);

// Get health state name for a path
//
// Returns state name in buf: "shavar", "yashar", "tov", etc.
//
ssize_t bereshit_xattr_get_health_state(const char* path, char* buf, size_t size);

// Get structured status for a path
//
// Returns "true" if directory has root.omni, "false" otherwise
//
ssize_t bereshit_xattr_get_structured(const char* path, char* buf, size_t size);

// Get OmniCode key for a path
//
// Returns key from nearest root.omni, or empty string if none
//
ssize_t bereshit_xattr_get_key(const char* path, char* buf, size_t size);

// Get block type for a file
//
// Returns "3", "4", "5", or "unknown"
//
ssize_t bereshit_xattr_get_block_type(const char* path, char* buf, size_t size);

// Get naming style for a file
//
// Returns "snake", "kebab", "pascal", or "invalid"
//
ssize_t bereshit_xattr_get_name_style(const char* path, char* buf, size_t size);

// -----------------------------------------------------------------------------
// B.3 Generic Extended Attribute Operations
// -----------------------------------------------------------------------------

// Get any Bereshit extended attribute
//
// name should include the full "user.bereshit." prefix
// Returns length of value on success, -1 on error
//
ssize_t bereshit_xattr_get(const char* path, const char* name,
                           char* buf, size_t size);

// List all Bereshit extended attributes for a path
//
// Fills buf with null-separated list of attribute names
// Returns total length of list on success, -1 on error
//
ssize_t bereshit_xattr_list(const char* path, char* buf, size_t size);

// -----------------------------------------------------------------------------
// B.4 FUSE Integration
// -----------------------------------------------------------------------------

// These functions are called by the FUSE layer to handle xattr operations

// FUSE getxattr handler for Bereshit attributes
// Returns attribute value length, or negative errno
int bereshit_xattr_fuse_get(const char* path, const char* name,
                            char* value, size_t size);

// FUSE listxattr handler for Bereshit attributes
// Returns list length, or negative errno
int bereshit_xattr_fuse_list(const char* path, char* list, size_t size);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Validation: Included by bereshit/bereshit.h
//
// Usage with standard tools:
//   getfattr -n user.bereshit.zone /mnt/bereshit/word/work/pkg/trit/trit.h
//   getfattr -d /mnt/bereshit/word/  # List all attributes
//
// Integration with cornerstone:
//   - IDE can query file metadata via xattrs
//   - Build system can verify structure
//   - Health monitoring reads xattrs for status
//
// "And thou shalt put in the breastplate of judgment the Urim and the Thummim;
//  and they shall be upon Aaron's heart." — Exodus 28:30
//
// Extended attributes are the Urim and Thummim of the filesystem —
// they reveal the truth about each file's identity and state.

#endif // BERESHIT_XATTR_H

// =============================================================================
// END CLOSING
// =============================================================================
