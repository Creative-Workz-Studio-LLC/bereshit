// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-fuse-zone
// Title: Bereshit Zone Abstraction
// Type: Header
// Component: Core
// Role: Define ternary zone semantics for filesystem

// Status: Active
// Version: a-02.00
// Created: 2026-01-15
// Updated: 2026-01-30

// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC

// Grounding: Genesis 1:1-2 — void (formless) → word (naming) → tov (good)
//
// The three zones mirror creation:
//   "And the earth was without form, and void" — void (-1)
//   "And God said, Let there be..." — word (0)
//   "And God saw that it was good" — tov (+1)

// =============================================================================
// END METADATA
// =============================================================================

#ifndef BERESHIT_ZONE_H
#define BERESHIT_ZONE_H

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
// B.1 Zone Root Management
// -----------------------------------------------------------------------------

// Get the Bereshit root path (set during mount)
const char* bereshit_zone_get_root(void);

// Set the Bereshit root path (during mount)
void bereshit_zone_set_root(const char* path);

// -----------------------------------------------------------------------------
// B.2 Zone Detection
// -----------------------------------------------------------------------------

// Determine which zone a path belongs to
//
// Path structure: /[void|word|tov]/...
// Returns BERESHIT_ZONE_UNKNOWN if not in a recognized zone
//
// Examples:
//   "/void/planning/task.md" → BERESHIT_ZONE_VOID
//   "/word/work/pkg/trit/trit.h" → BERESHIT_ZONE_WORD
//   "/tov/demo/phase-1/demo-trit" → BERESHIT_ZONE_TOV
//
BereshitZone bereshit_zone_from_path(const char* path);

// Get zone name string
//
// Returns:
//   "void" for BERESHIT_ZONE_VOID
//   "word" for BERESHIT_ZONE_WORD
//   "tov" for BERESHIT_ZONE_TOV
//   "unknown" otherwise
//
const char* bereshit_zone_name(BereshitZone zone);

// Get zone integer value (-1, 0, +1)
int bereshit_zone_value(BereshitZone zone);

// Check if path is within Bereshit structure
bool bereshit_zone_is_valid_path(const char* path);

// Get absolute filesystem path for a zone
// Returns NULL if zone is invalid
const char* bereshit_zone_directory(BereshitZone zone);

// -----------------------------------------------------------------------------
// B.3 Zone Information
// -----------------------------------------------------------------------------

// Load zone info from root.omni and .health files
//
// Populates BereshitZoneInfo with:
//   - zone: Which zone this is
//   - name: Zone name string
//   - description: From root.omni description field
//   - key: OmniCode key from root.omni
//   - health: Current health value
//   - has_root_omni: Whether root.omni exists
//   - is_structured: Has root.omni in directory
//
// Returns true if zone info was loaded (zone may exist without root.omni)
//
bool bereshit_zone_load_info(const char* zone_path, BereshitZoneInfo* info);

// Check if a directory is "structured" (has root.omni)
//
// A structured directory:
//   - Has a root.omni file defining its identity
//   - Can contain other structured content
//   - Is required in tov zone for file creation
//
bool bereshit_zone_is_structured(const char* dir_path);

// -----------------------------------------------------------------------------
// B.4 Zone Policy Enforcement
// -----------------------------------------------------------------------------

// Check if an operation is permitted by zone policy
//
// Zone policies (ENFORCED, not optional):
//
//   void (-1): Entry gate - PERMISSIVE
//     "without form, and void" — formless accepts all
//     - All operations permitted
//     - This is where unstructured work happens
//
//   word (0): Processing - STANDARD
//     "And God said" — naming and work
//     - All operations permitted
//     - This is where definitions and implementations live
//
//   tov (+1): Exit gate - STRUCTURED
//     "And God saw that it was good" — only completed work
//     - Read: Always permitted
//     - Create file: Only if creating root.omni OR parent has root.omni
//     - Write: Permitted (file already in structured context)
//     - Delete root.omni: Blocked if directory has children
//     - Mkdir: Permitted (health tracked until root.omni added)
//     - Rmdir: Only empty directories
//
// Returns:
//   0 if allowed
//   -EACCES if denied by policy
//
int bereshit_zone_check_policy(const char* path, BereshitOperation op);

// Get human-readable policy description for a zone
const char* bereshit_zone_policy_description(BereshitZone zone);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Validation: Included by bereshit/bereshit.h
//
// Integration with cornerstone:
//   - Zone semantics inform cornerstone file organization
//   - void/ maps to imports, planning (pre-work)
//   - word/ maps to implementation (active work)
//   - tov/ maps to completion (demos, releases)
//
// "And God saw the light, that it was good: and God divided
//  the light from the darkness." — Genesis 1:4

#endif // BERESHIT_ZONE_H

// =============================================================================
// END CLOSING
// =============================================================================
