// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// 4-Block Code Structure: Identity and context for this component
//
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-fuse-bereshit
// Title: Bereshit Filesystem Master Aggregator
// Type: Header
// Component: Core
// Role: Single entry point for all Bereshit FS subsystems

// # M.2 Lifecycle State [STATE]
//
// Status: Active
// Version: a-01.00
// Created: 2026-01-30
// Updated: 2026-01-30

// # M.3 Attribution [ATTRIBUTION]
//
// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
// Copyright: (c) 2026 CreativeWorkzStudio LLC. All rights reserved.

// # M.4 Location [LOCATION]
//
// Path: word/work/pkg/fuse/include/bereshit/bereshit.h

// # M.5 Derivation [DERIVATION]
//
// Derives_from: word/seed/code/c/header.h
// Pattern_from: word/work/pkg/trit/include/trit/trit.h

// # M.6 Classification [CLASSIFICATION]
//
// Tags: bereshit, filesystem, fuse, aggregator

// # M.7 Intent [INTENT]
//
// Purpose: Single #include for all Bereshit FS functionality
// Core Design: Layer-ordered includes following creation order
// Philosophy: "In the beginning God created" — Genesis 1:1
//
// Key Features:
//   - Single entry point: #include "bereshit/bereshit.h"
//   - Ternary zone semantics (void/word/tov)
//   - Health tracking per zone
//   - Extended attributes for zone info
//   - Block structure validation

// # M.8 Grounding [GROUNDING]
//
// Scripture: "In the beginning God created the heaven and the earth."
//            — Genesis 1:1
// Principle: Creation has structure (void → word → tov)
// Anchor: Genesis 1:4 — "God saw the light, that it was good"

// =============================================================================
// END METADATA
// =============================================================================

#ifndef BERESHIT_FS_H
#define BERESHIT_FS_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// # S.1 Include Order [INCLUDE_ORDER]
//
// Layers are included in dependency order:
//
//   LAYER 1: FOUNDATION (types, constants)
//     - bereshit/types.h — Core types and constants
//
//   LAYER 2: ZONE (ternary semantics)
//     - bereshit/zone.h — Zone abstraction (void/word/tov)
//
//   LAYER 3: HEALTH (state tracking)
//     - bereshit/health.h — Health tracking per zone/directory
//
//   LAYER 4: XATTR (extended attributes)
//     - bereshit/xattr.h — Extended attribute operations
//
//   LAYER 5: VALIDATE (structure checking)
//     - bereshit/validate.h — Block structure validation

// ---------------------------------------------------------------------------
// LAYER 1: FOUNDATION — Core Types
// ---------------------------------------------------------------------------

#include "bereshit/types.h"

// ---------------------------------------------------------------------------
// LAYER 2: ZONE — Ternary Semantics
// ---------------------------------------------------------------------------

#include "bereshit/zone.h"

// ---------------------------------------------------------------------------
// LAYER 3: HEALTH — State Tracking
// ---------------------------------------------------------------------------

#include "bereshit/health.h"

// ---------------------------------------------------------------------------
// LAYER 4: XATTR — Extended Attributes
// ---------------------------------------------------------------------------

#include "bereshit/xattr.h"

// ---------------------------------------------------------------------------
// LAYER 5: VALIDATE — Structure Checking
// ---------------------------------------------------------------------------

#include "bereshit/validate.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Version Information [VERSION]

#define BERESHIT_FS_VERSION_MAJOR 0
#define BERESHIT_FS_VERSION_MINOR 2
#define BERESHIT_FS_VERSION_PATCH 0
#define BERESHIT_FS_VERSION_STRING "0.2.0"

// # B.2 Feature Flags [FEATURES]

// Policy enforcement is ALWAYS on — no optional stubs
#define BERESHIT_FS_POLICY_ENFORCED 1

// Extended attributes support
#define BERESHIT_FS_XATTR_ENABLED 1

// Health tracking support
#define BERESHIT_FS_HEALTH_ENABLED 1

// Block validation support
#define BERESHIT_FS_VALIDATE_ENABLED 1

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Validation [VALIDATION]
//
// Build:
//   gcc -fsyntax-only -I../include bereshit.h
//
// Test:
//   Include in any source: #include "bereshit/bereshit.h"

// # X.2 Usage [USAGE]
//
// Simple:
//   #include "bereshit/bereshit.h"
//   // All Bereshit FS functionality available
//
// Selective:
//   #include "bereshit/zone.h"   // Just zones
//   #include "bereshit/health.h" // Just health

// # X.8 Closing Note [NOTE]
//
// "In the beginning God created the heaven and the earth.
//  And the earth was without form, and void..."
//   — Genesis 1:1-2
//
// void (-1): Formless, accepts all
// word (0):  Processing, definitions
// tov (+1):  Completion, structured
//
// The filesystem reflects the order of creation.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_FS_H
