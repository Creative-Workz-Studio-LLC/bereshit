// #!omni code --c -library
// ═══════════════════════════════════════════════════════════════════════════════
// OmniCode 4-Block C Header — DAR Witness System Orchestrator
// ═══════════════════════════════════════════════════════════════════════════════
//
// METADATA → SETUP → BODY → CLOSING
//
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// PRAGMA BLOCK [PRAGMA]
// ═══════════════════════════════════════════════════════════════════════════════
//
// @omni document
// @omni.pragma.format = code
// @omni.pragma.language = c
// @omni.pragma.variant = header
// @omni.pragma.structure = 4-block
// @omni.pragma.version = a-03.00
//
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * P.1 CORE — Mandatory Pragma Fields [CORE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni.core.key = CORNERSTONE-CORE-CPISI-DAR
 * @omni.core.type = code
 * @omni.core.status = active
 *
 * ───────────────────────────────────────────────────────────────────────────────
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * P.2 FAMILY — Template Lineage [FAMILY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni.family.template = word/seed/code/c/header.h
 * @omni.family.derives_from = cpisi/types/types.h
 * @omni.family.implements = word/core/os/health/dar.toml
 *
 * ───────────────────────────────────────────────────────────────────────────────
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * P.3 INSTANCE — This File Identity [INSTANCE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni.instance.path = cornerstone/engine/core/include/cpisi/dar/dar.h
 * @omni.instance.created = 2025-12-19
 * @omni.instance.updated = 2026-01-25
 *
 * ───────────────────────────────────────────────────────────────────────────────
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * P.4 ARCHITECTURE — Position in 9-Layer Pyramid [ARCHITECTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni.arch.layer = 4 (HEALTH — center of 9-layer pyramid)
 * @omni.arch.tier = GROUP (orchestrator — includes all DAR phases)
 * @omni.arch.domain = dar
 * @omni.arch.error_range = 100-134 (HEALTH domain)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * P.5 SUMMARY — Human-Readable Description [SUMMARY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @brief CPI-SI DAR Witness System — Orchestrator Header
 *
 * Single include for the complete DAR (Detect-Assess-Restore) witness system.
 * This is the canonical entry point for DAR functionality.
 *
 * DAR FLOW: DETECT (witness) → ASSESS (evaluate) → RESTORE (recover)
 *
 * BIBLICAL FOUNDATION: "Let us search and try our ways, and turn again
 *                       to the LORD." — Lamentations 3:40
 *
 * PRINCIPLE: DAR embodies this verse:
 *   - DETECT = "search" — witness the problem
 *   - ASSESS = "try our ways" — evaluate the situation
 *   - RESTORE = "turn again" — recover to health
 *
 * ───────────────────────────────────────────────────────────────────────────────
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END PRAGMA
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SPECIAL BLOCK [SPECIAL]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Pre-include configuration for DAR system.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-Time Configuration [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_config SP.1 CONFIG — Compile-Time Configuration
 * @brief    Feature flags and platform configuration.
 *
 * WHY: DAR phases need consistent feature configuration.
 *      Master switches here propagate to all phase headers.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* POSIX requirement — propagates to all phase headers */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/* DAR master enable — disable to stub out all DAR functionality */
#ifndef CPISI_DAR_ENABLED
#define CPISI_DAR_ENABLED 1
#endif

/* Panel support — enables DAR panel integration */
#ifndef CPISI_DAR_PANEL_ENABLED
#define CPISI_DAR_PANEL_ENABLED 1
#endif

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL
/// ═══════════════════════════════════════════════════════════════════════════════

#ifndef CORNERSTONE_CORE_CPISI_DAR_H
#define CORNERSTONE_CORE_CPISI_DAR_H

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief File-level documentation for Doxygen.
///
/// @file dar.h
/// @ingroup cpisi_dar
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - M.1 IDENTITY       — Key, title, component, status
///   - M.2 STATE          — Version, lifecycle stage
///   - M.3 ATTRIBUTION    — Authors, ownership
///   - M.4 LOCATION       — File paths, project position
///   - M.5 DERIVATION     — Template and type sources
///   - M.6 CLASSIFICATION — Layer, domain, access
///   - M.7 INTENT         — Purpose and function
///   - M.8 GROUNDING      — Biblical foundation
///   - M.9 DEPENDENCIES   — What this needs, what needs this
///   - M.10 ROADMAP       — History and future
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Component Identification [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_identity M.1 IDENTITY — Component Identification
 * @ingroup cpisi_dar
 * @brief    Key, title, type, and status.
 *
 * Key:       CORNERSTONE-CORE-CPISI-DAR
 * Title:     CPI-SI DAR Witness System (Orchestrator)
 * Component: Core (CPI-SI infrastructure)
 * Status:    Active
 *
 * @{ @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Version and Lifecycle [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_state M.2 STATE — Version and Lifecycle
 * @ingroup cpisi_dar
 * @brief    Version, stage, and lifecycle status.
 *
 * Version: a-03.00
 * Stage:   Alpha (a) — active development
 * Phase:   03 — third iteration
 *
 * Living versioning format: STAGE-PHASE.COMPLETION
 *   a = alpha (-1), b = beta (0), c = stable (+1)
 *
 * @{ @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authorship [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_attribution M.3 ATTRIBUTION — Authorship
 * @ingroup cpisi_dar
 * @brief    Authors and ownership.
 *
 * Architect:       Seanje Lenox-Wise
 * Implementation:  Nova Dawn
 * Organization:    CreativeWorkzStudio LLC
 * Created:         2025-12-19
 *
 * @{ @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Position [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_location M.4 LOCATION — File Position
 * @ingroup cpisi_dar
 * @brief    File paths and project position.
 *
 * Path:     cornerstone/engine/core/include/cpisi/dar/dar.h
 * Position: cpisi/dar/ subfolder (orchestrates contents of dar/)
 *
 * Folder Structure:
 *   cpisi/
 *     types/types.h    ← Foundation
 *     dar/             ← DAR subfolder
 *       dar.h          ← THIS (orchestrator)
 *       io/            ← I/O support
 *       phases/        ← Phase headers
 *     cpisi.h          ← Main orchestrator (includes this)
 *
 * @{ @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Template and Sources [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_derivation M.5 DERIVATION — Template and Sources
 * @ingroup cpisi_dar
 * @brief    Template lineage and type sources.
 *
 * Template:   word/seed/code/c/header.h
 * Types:      cpisi/types/types.h (foundation)
 * Spec:       word/core/os/health/dar.toml
 *
 * Derivation Chain:
 *   header.h template → types.h foundation → dar.toml spec → this file
 *
 * @{ @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Layer and Domain [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_classification M.6 CLASSIFICATION — Layer and Domain
 * @ingroup cpisi_dar
 * @brief    Architectural position and access level.
 *
 * Layer:  4 (HEALTH — center of 9-layer pyramid)
 * Domain: dar (Detect-Assess-Restore)
 * Tier:   GROUP (orchestrator aggregating phases)
 * Access: Public (orchestrator API)
 *
 * 9-Layer Pyramid:
 *   0:KERNEL, 1:SYSTEM, 2:RUNTIME, 3:FRAMEWORK,
 *   4:HEALTH (center), 5:NETWORK, 6:STORAGE, 7:ENGINE, 8:APP
 *
 * @{ @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Function [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_intent M.7 INTENT — Purpose and Function
 * @ingroup cpisi_dar
 * @brief    Orchestrator role and responsibilities.
 *
 * PURPOSE:
 *   Single entry point for DAR witness system. Users include this header
 *   to access all DAR functionality — no need to include individual phases.
 *
 * ORCHESTRATOR RESPONSIBILITIES:
 *   1. Establish correct ladder import chain
 *   2. Include all DAR phase headers in dependency order
 *   3. Provide cross-phase orchestration functions
 *   4. Synchronize state across phases
 *   5. Prepare for panel integration
 *
 * DAR FLOW:
 *   DETECT → ASSESS → RESTORE
 *   Each phase can trigger the next. Health scoring drives transitions.
 *
 * @{ @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_grounding M.8 GROUNDING — Biblical Foundation
 * @ingroup cpisi_dar
 * @brief    Scripture and principle anchoring this component.
 *
 * Scripture: "Let us search and try our ways, and turn again to the LORD."
 *            — Lamentations 3:40 KJV
 *
 * Principle: DAR embodies this verse:
 *   - DETECT = "search" — witness the problem
 *   - ASSESS = "try our ways" — evaluate the situation
 *   - RESTORE = "turn again" — recover to health
 *
 * Anchor: "Without counsel purposes are disappointed: but in the multitude
 *          of counsellors they are established." — Proverbs 15:22 KJV
 *
 * The orchestrator coordinates the "counsellors" (phases) so purposes succeed.
 *
 * @{ @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Ladder Chain [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_dependencies M.9 DEPENDENCIES — Ladder Chain
 * @ingroup cpisi_dar
 * @brief    What this orchestrator includes and what includes it.
 *
 * WHAT THIS INCLUDES (LADDER DOWN):
 *   - cpisi/types/types.h        — Foundation (all CPI-SI types)
 *   - cpisi/dar/io/rotation.h    — I/O support (file rotation)
 *   - cpisi/dar/phases/detect.h  — DETECT phase
 *   - cpisi/dar/phases/assess.h  — ASSESS phase
 *   - cpisi/dar/phases/restore.h — RESTORE phase
 *
 * WHAT INCLUDES THIS (LADDER UP):
 *   - cpisi/cpisi.h — Main CPI-SI orchestrator
 *
 * PANEL INTEGRATION (FUTURE):
 *   - engine/ui/dar/panel.h — Visual DAR panel (will include this)
 *
 * @{ @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_roadmap M.10 ROADMAP — History and Future
 * @ingroup cpisi_dar
 * @brief    Version history and planned features.
 *
 * HISTORY:
 *   a-01.00 (2025-12-19): Initial structure
 *   a-02.00 (2026-01-20): Added CPI-SI cognitive features
 *   a-03.00 (2026-01-25): Full alignment with header.h template
 *                         Added orchestration functions
 *                         Added cross-phase state sync
 *                         Added panel preparation
 *
 * FUTURE:
 *   - Panel integration (engine/ui/dar/)
 *   - Real-time DAR visualization
 *   - Cross-session DAR analytics
 *
 * @{ @} end M.10 ROADMAP
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END METADATA
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SETUP BLOCK [SETUP]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Declarations enabling the orchestrator.
///
/// @defgroup cpisi_dar_setup SETUP BLOCK — Declarations
///
/// CONTAINS:
///   - S.1 INCLUDES    — Ladder chain imports
///   - S.2 DEFINES     — Cross-phase constants and macros
///   - S.3 TYPES       — Orchestrator types
///   - S.4 PROTOTYPES  — Orchestration functions
///   - S.5 FILE-LEVEL  — Global orchestrator state
///   - S.6 OMISSION    — Reserved section guide
///
/// ═══════════════════════════════════════════════════════════════════════════════

/// ─────────────────────────────────────────────────────────────────────────────
/// CODE SECTIONS [CODE_SECTIONS]
/// ─────────────────────────────────────────────────────────────────────────────

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES — Ladder Chain Imports [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_includes S.1 INCLUDES — Ladder Chain Imports
 * @brief    Headers in strict dependency order.
 *
 * WHY ORCHESTRATOR INCLUDES MATTER:
 *   This header establishes the correct import order for all DAR functionality.
 *   Users include dar.h and get everything in the right order.
 *
 * LADDER CHAIN ORDER:
 *   1. Foundation (types.h) — all types build on this
 *   2. I/O support (rotation.h) — used by all phases
 *   3. DETECT phase — first in DAR flow
 *   4. ASSESS phase — evaluates DETECT output
 *   5. RESTORE phase — acts on ASSESS evaluation
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a FOUNDATION — Core Types [FOUNDATION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_includes_foundation S.1a FOUNDATION — Core Types
 * @ingroup cpisi_dar_includes
 * @brief    Foundation types MUST come first.
 *
 * WHY: types.h is the single source of truth for all CPI-SI types.
 *      Every other header depends on these definitions.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel.h"  /**< Foundation: HebrewState, HealthScore, all base types */

/* DAR Orchestrator requires hosted mode (CPISI_MODE >= 2) */
#if CPISI_MODE >= 2

/** @} */ /* end S.1a FOUNDATION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b IO — I/O Support [IO]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_includes_io S.1b IO — I/O Support
 * @ingroup cpisi_dar_includes
 * @brief    File management used by all phases.
 *
 * WHY: Rotation provides dated folders, archival, and retention.
 *      All phases write to rotated directories.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "witness/utility/rotation.util.h"  /**< DAR file rotation: dated folders, archival */

/** @} */ /* end S.1b IO */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c PHASES — DAR Phase Headers [PHASES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_includes_phases S.1c PHASES — DAR Phase Headers
 * @ingroup cpisi_dar_includes
 * @brief    DAR phases in flow order: DETECT → ASSESS → RESTORE.
 *
 * WHY ORDER MATTERS:
 *   - DETECT defines DetectContext, DetectType, logging functions
 *   - ASSESS uses DetectType, defines AssessContext, DiagReport
 *   - RESTORE uses DiagReport, defines RestoreContext, recovery functions
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "witness/phase/detect.phase.h"   /**< DETECT: Witness and record events */
#include "witness/phase/assess.phase.h"   /**< ASSESS: Evaluate and diagnose */
#include "witness/phase/restore.phase.h"  /**< RESTORE: Recover and fix */

/** @} */ /* end S.1c PHASES */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Cross-Phase Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_defines S.2 DEFINES — Cross-Phase Constants
 * @brief    Constants and macros spanning all DAR phases.
 *
 * CONTAINS:
 *   - S.2a PHASES  — Phase enumeration
 *   - S.2b LIMITS  — Orchestrator limits
 *   - S.2c MACROS  — Cross-phase convenience macros
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a PHASES — Phase Enumeration [PHASES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_defines_phases S.2a PHASES — Phase Enumeration
 * @ingroup cpisi_dar_defines
 * @brief    DAR phase indices following ternary balanced pattern.
 *
 * WHY: Phases follow the ternary pattern from dar.toml:
 *   DETECT  = -1 (witness, break down)
 *   ASSESS  =  0 (evaluate, center)
 *   RESTORE = +1 (recover, build up)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief DAR phase count. */
#define CPISI_DAR_PHASE_COUNT   3

/** @brief DAR phase indices (balanced ternary). */
#define CPISI_DAR_DETECT       (-1)  /**< DETECT: witness, -1 */
#define CPISI_DAR_ASSESS         0   /**< ASSESS: evaluate, 0 (center) */
#define CPISI_DAR_RESTORE        1   /**< RESTORE: recover, +1 */

/** @} */ /* end S.2a PHASES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b LIMITS — Orchestrator Limits [LIMITS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_defines_limits S.2b LIMITS — Orchestrator Limits
 * @ingroup cpisi_dar_defines
 * @brief    Limits for orchestrator operations.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Maximum DAR cycles to track. */
#ifndef CPISI_DAR_CYCLE_HISTORY
#define CPISI_DAR_CYCLE_HISTORY     100
#endif

/** @brief Panel update interval in milliseconds. */
#ifndef CPISI_DAR_PANEL_INTERVAL_MS
#define CPISI_DAR_PANEL_INTERVAL_MS 1000
#endif

/** @} */ /* end S.2b LIMITS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c MACROS — Cross-Phase Convenience [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_defines_macros S.2c MACROS — Cross-Phase Convenience
 * @ingroup cpisi_dar_defines
 * @brief    Macros that work with any DAR context type.
 *
 * WHY: All DAR contexts share common fields (hebrew_state, health, k_factor).
 *      These macros provide uniform access.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Check if DAR context is initialized.
 *
 * @param ctx  Any DAR context (DetectContext, AssessContext, RestoreContext).
 *
 * @return true if initialized, false otherwise.
 */
#define DAR_IS_INITIALIZED(ctx) ((ctx) != NULL && (ctx)->initialized)

/**
 * @brief Get current Hebrew state from any DAR context.
 *
 * @param ctx  Any DAR context.
 *
 * @return Current HebrewState.
 */
#define DAR_HEBREW_STATE(ctx) ((ctx)->hebrew_state)

/**
 * @brief Get current health score from any DAR context.
 *
 * @param ctx  Any DAR context.
 *
 * @return Current HealthScore.
 */
#define DAR_HEALTH(ctx) ((ctx)->health)

/**
 * @brief Get current k-factor from any DAR context.
 *
 * @param ctx  Any DAR context.
 *
 * @return Current KFactor.
 */
#define DAR_K_FACTOR(ctx) ((ctx)->k_factor)

/**
 * @brief Check if context is in critical state.
 *
 * @param ctx  Any DAR context.
 *
 * @return true if hebrew_state is SHAVAR (broken).
 */
#define DAR_IS_CRITICAL(ctx) ((ctx)->hebrew_state == HEBREW_STATE_SHAVAR)

/**
 * @brief Check if context is healthy.
 *
 * @param ctx  Any DAR context.
 *
 * @return true if hebrew_state is TOV (complete) or SHALEM (whole).
 */
#define DAR_IS_HEALTHY(ctx) \
    ((ctx)->hebrew_state == HEBREW_STATE_TOV || \
     (ctx)->hebrew_state == HEBREW_STATE_SHALEM)

/** @} */ /* end S.2c MACROS */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Orchestrator Types [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_types S.3 TYPES — Orchestrator Types
 * @brief    Types for DAR orchestration.
 *
 * CONTAINS:
 *   - S.3a ENUMS   — Orchestrator enumerations
 *   - S.3b STRUCTS — Orchestrator structures
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — Orchestrator Enumerations [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_types_enums S.3a ENUMS — Orchestrator Enumerations
 * @ingroup cpisi_dar_types
 * @brief    Enumerations for orchestrator state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Current DAR phase.
 *
 * WHY: Tracks which phase the orchestrator is currently in.
 *
 * NOTE: DARPhase is already defined in kernel/types/composite/type/phase.type.h.
 *       Use those values (DAR_DETECT=0, DAR_ASSESS=1, DAR_RESTORE=2).
 *       These DAR_PHASE_* aliases map to the canonical values.
 */
#ifndef DAR_PHASE_IDLE
#define DAR_PHASE_IDLE    (-1)  /**< Not running (idle state before detect) */
#define DAR_PHASE_DETECT  DAR_DETECT   /**< In DETECT phase (=0) */
#define DAR_PHASE_ASSESS  DAR_ASSESS   /**< In ASSESS phase (=1) */
#define DAR_PHASE_RESTORE DAR_RESTORE  /**< In RESTORE phase (=2) */
#endif

/**
 * @brief DAR cycle result.
 *
 * WHY: Reports outcome of a complete DAR cycle.
 */
typedef enum DARCycleResult {
    DAR_CYCLE_SUCCESS     = 0,  /**< Cycle completed successfully */
    DAR_CYCLE_PARTIAL     = 1,  /**< Cycle partially completed */
    DAR_CYCLE_DEGRADED    = 2,  /**< Running in degraded mode */
    DAR_CYCLE_FAILED      = 3,  /**< Cycle failed */
    DAR_CYCLE_SKIPPED     = 4,  /**< Cycle skipped (not needed) */
} DARCycleResult;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — Orchestrator Structures [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_types_structs S.3b STRUCTS — Orchestrator Structures
 * @ingroup cpisi_dar_types
 * @brief    Structures for orchestrator state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief DAR cycle statistics.
 *
 * WHY: Tracks performance of DAR cycles for monitoring and panel display.
 */
typedef struct DARCycleStats {
    uint32_t total_cycles;       /**< Total cycles run */
    uint32_t successful_cycles;  /**< Cycles that succeeded */
    uint32_t partial_cycles;     /**< Cycles that partially succeeded */
    uint32_t failed_cycles;      /**< Cycles that failed */
    uint32_t skipped_cycles;     /**< Cycles that were skipped */
    uint64_t last_cycle_ms;      /**< Last cycle duration in ms */
    uint64_t avg_cycle_ms;       /**< Average cycle duration in ms */
} DARCycleStats;

/**
 * @brief DAR orchestrator context.
 *
 * WHY: Coordinates all three phases and maintains cross-phase state.
 */
typedef struct DAROrchestrator {
    /* Phase contexts (linked, not owned) */
    DetectContext*   detect;       /**< DETECT phase context */
    AssessContext*   assess;       /**< ASSESS phase context */
    RestoreContext*  restore;      /**< RESTORE phase context */

    /* Orchestrator state */
    bool             initialized;  /**< Orchestrator initialized */
    DARPhase         current_phase;/**< Current active phase */
    HebrewState      hebrew_state; /**< Synchronized Hebrew state */
    HealthScore      health;       /**< Synchronized health score */
    KFactor          k_factor;     /**< Synchronized k-factor */

    /* Cycle tracking */
    DARCycleStats    stats;        /**< Cycle statistics */
    uint32_t         cycle_count;  /**< Current cycle count */

    /* Panel integration */
    bool             panel_enabled;/**< Panel updates enabled */
    uint64_t         panel_last_update; /**< Last panel update timestamp */

    /* Session Database integration */
    void*            session_db;   /**< SessionDB* for cross-session tracking */
    char             session_id[32]; /**< Current session ID */
    bool             db_enabled;   /**< Session database integration enabled */

    /* Callbacks */
    void (*on_phase_change)(DARPhase old_phase, DARPhase new_phase, void* user_data);
    void (*on_cycle_complete)(DARCycleResult result, void* user_data);
    void (*on_state_change)(HebrewState state, HealthScore health, void* user_data);
    void*            user_data;    /**< User context for callbacks */
} DAROrchestrator;

/**
 * @brief Default initializer for DAROrchestrator.
 */
#define DAR_ORCHESTRATOR_INIT { \
    .detect = NULL, \
    .assess = NULL, \
    .restore = NULL, \
    .initialized = false, \
    .current_phase = DAR_PHASE_IDLE, \
    .hebrew_state = HEBREW_STATE_YASHAR, \
    .health = 0, \
    .k_factor = K_FACTOR_NEUTRAL, \
    .stats = {0}, \
    .cycle_count = 0, \
    .panel_enabled = false, \
    .panel_last_update = 0, \
    .session_db = NULL, \
    .session_id = {0}, \
    .db_enabled = false, \
    .on_phase_change = NULL, \
    .on_cycle_complete = NULL, \
    .on_state_change = NULL, \
    .user_data = NULL \
}

/** @} */ /* end S.3b STRUCTS */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Orchestration Functions [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_prototypes S.4 PROTOTYPES — Orchestration Functions
 * @brief    Function declarations for DAR orchestration.
 *
 * CONTAINS:
 *   - S.4a LIFECYCLE   — Init/shutdown
 *   - S.4b LINKING     — Phase linking
 *   - S.4c CYCLE       — DAR cycle operations
 *   - S.4d STATE       — Cross-phase state sync
 *   - S.4e PANEL       — Panel integration
 *   - S.4e2 SESSION_DB — Session database integration
 *   - S.4f REPORTING   — Stats and diagnostics
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Init/Shutdown [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_lifecycle S.4a LIFECYCLE — Init/Shutdown
 * @ingroup cpisi_dar_prototypes
 * @brief    Orchestrator lifecycle management.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize the DAR orchestrator.
 *
 * WHY: Prepares orchestrator for coordinating DAR phases.
 *      Does NOT initialize individual phases — call phase init functions separately.
 *
 * @param[out] orch  Orchestrator to initialize.
 */
void cpisi_dar_init(DAROrchestrator* orch);

/**
 * @brief Shutdown the DAR orchestrator.
 *
 * WHY: Cleans up orchestrator state.
 *      Does NOT shutdown phases — call phase shutdown functions separately.
 *
 * @param[in,out] orch  Orchestrator to shutdown.
 */
void cpisi_dar_shutdown(DAROrchestrator* orch);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b LINKING — Phase Linking [LINKING]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_linking S.4b LINKING — Phase Linking
 * @ingroup cpisi_dar_prototypes
 * @brief    Link phase contexts to orchestrator.
 *
 * WHY: Orchestrator coordinates phases but doesn't own them.
 *      Phases are initialized separately and linked to orchestrator.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Link DETECT phase to orchestrator.
 *
 * @param[in,out] orch    Orchestrator.
 * @param[in]     detect  Initialized DetectContext.
 */
void cpisi_dar_link_detect(DAROrchestrator* orch, DetectContext* detect);

/**
 * @brief Link ASSESS phase to orchestrator.
 *
 * @param[in,out] orch    Orchestrator.
 * @param[in]     assess  Initialized AssessContext.
 */
void cpisi_dar_link_assess(DAROrchestrator* orch, AssessContext* assess);

/**
 * @brief Link RESTORE phase to orchestrator.
 *
 * @param[in,out] orch     Orchestrator.
 * @param[in]     restore  Initialized RestoreContext.
 */
void cpisi_dar_link_restore(DAROrchestrator* orch, RestoreContext* restore);

/**
 * @brief Link all phases at once.
 *
 * WHY: Convenience for common case where all phases are available.
 *
 * @param[in,out] orch     Orchestrator.
 * @param[in]     detect   Initialized DetectContext.
 * @param[in]     assess   Initialized AssessContext.
 * @param[in]     restore  Initialized RestoreContext.
 */
void cpisi_dar_link_all(DAROrchestrator* orch,
                        DetectContext* detect,
                        AssessContext* assess,
                        RestoreContext* restore);

/** @} */ /* end S.4b LINKING */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c CYCLE — DAR Cycle Operations [CYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_cycle S.4c CYCLE — DAR Cycle Operations
 * @ingroup cpisi_dar_prototypes
 * @brief    Run complete DAR cycles.
 *
 * WHY: The core purpose of orchestration — run DETECT→ASSESS→RESTORE cycles.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Run a complete DAR cycle through the orchestrator.
 *
 * WHY: Executes DETECT → ASSESS → RESTORE in sequence.
 *      Synchronizes state between phases.
 *
 * NOTE: This is distinct from cpisi_dar_cycle() in phase.h which is a
 *       simple health-based evaluation. This function uses the full
 *       orchestrator with linked phase contexts.
 *
 * @param[in,out] orch  Orchestrator with linked phases.
 *
 * @return DARCycleResult indicating outcome.
 */
DARCycleResult cpisi_dar_run_cycle(DAROrchestrator* orch);

/**
 * @brief Run DAR cycle if health is below threshold.
 *
 * WHY: Conditional cycle — only run if health indicates need.
 *
 * @param[in,out] orch       Orchestrator.
 * @param[in]     threshold  Minimum health to skip cycle.
 *
 * @return DARCycleResult (DAR_CYCLE_SKIPPED if health >= threshold).
 */
DARCycleResult cpisi_dar_cycle_if_needed(DAROrchestrator* orch, HealthScore threshold);

/**
 * @brief Advance to next phase manually.
 *
 * WHY: For fine-grained control over phase progression.
 *
 * @param[in,out] orch  Orchestrator.
 *
 * @return New phase after advancement.
 */
DARPhase cpisi_dar_advance_phase(DAROrchestrator* orch);

/** @} */ /* end S.4c CYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4d STATE — Cross-Phase State Sync [STATE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_state S.4d STATE — Cross-Phase State Sync
 * @ingroup cpisi_dar_prototypes
 * @brief    Synchronize cognitive state across phases.
 *
 * WHY: Phases must share consistent cognitive state.
 *      Orchestrator is the source of truth for cross-phase state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Synchronize state from orchestrator to all phases.
 *
 * WHY: Push orchestrator state to all linked phases.
 *
 * @param[in,out] orch  Orchestrator.
 */
void cpisi_dar_sync_to_phases(DAROrchestrator* orch);

/**
 * @brief Synchronize state from current phase to orchestrator.
 *
 * WHY: Pull state from active phase to orchestrator.
 *
 * @param[in,out] orch  Orchestrator.
 */
void cpisi_dar_sync_from_phase(DAROrchestrator* orch);

/**
 * @brief Set Hebrew state across all phases.
 *
 * WHY: Unified state change propagated to all phases.
 *
 * @param[in,out] orch   Orchestrator.
 * @param[in]     state  New Hebrew state.
 */
void cpisi_dar_set_hebrew_state(DAROrchestrator* orch, HebrewState state);

/**
 * @brief Set health score across all phases.
 *
 * WHY: Unified health change propagated to all phases.
 *
 * @param[in,out] orch    Orchestrator.
 * @param[in]     health  New health score.
 */
void cpisi_dar_set_health(DAROrchestrator* orch, HealthScore health);

/**
 * @brief Set k-factor across all phases.
 *
 * WHY: Unified k-factor change propagated to all phases.
 *
 * @param[in,out] orch  Orchestrator.
 * @param[in]     k     New k-factor.
 */
void cpisi_dar_set_k_factor(DAROrchestrator* orch, KFactor k);

/** @} */ /* end S.4d STATE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4e PANEL — Panel Integration [PANEL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_panel S.4e PANEL — Panel Integration
 * @ingroup cpisi_dar_prototypes
 * @brief    Prepare data for DAR panel visualization.
 *
 * WHY: The DAR panel (engine/ui/dar/) needs structured data.
 *      These functions prepare data for panel consumption.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Enable or disable panel updates.
 *
 * @param[in,out] orch    Orchestrator.
 * @param[in]     enable  true to enable, false to disable.
 */
void cpisi_dar_panel_enable(DAROrchestrator* orch, bool enable);

/**
 * @brief Check if panel update is needed.
 *
 * WHY: Rate-limited updates to avoid overwhelming the panel.
 *
 * @param[in] orch  Orchestrator.
 *
 * @return true if enough time has passed since last update.
 */
bool cpisi_dar_panel_needs_update(const DAROrchestrator* orch);

/**
 * @brief Get current phase for panel display.
 *
 * @param[in] orch  Orchestrator.
 *
 * @return Current DARPhase.
 */
DARPhase cpisi_dar_panel_get_phase(const DAROrchestrator* orch);

/**
 * @brief Get cycle statistics for panel display.
 *
 * @param[in]  orch   Orchestrator.
 * @param[out] stats  Statistics output.
 */
void cpisi_dar_panel_get_stats(const DAROrchestrator* orch, DARCycleStats* stats);

/**
 * @brief Get orchestrator phase name for panel display.
 *
 * NOTE: This is distinct from cpisi_dar_phase_name() in rotation.h which
 *       takes DARPhaseId (0-3). This function takes DARPhase (-2 to +1)
 *       which includes the IDLE state for orchestrator tracking.
 *
 * @param[in] phase  Orchestrator DARPhase.
 *
 * @return Static string (do not free).
 */
const char* cpisi_dar_orch_phase_str(DARPhase phase);

/** @} */ /* end S.4e PANEL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4e2 SESSION_DB — Session Database Integration [SESSION_DB]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_session_db S.4e2 SESSION_DB — Session Database Integration
 * @ingroup cpisi_dar_prototypes
 * @brief    Enable cross-session tracking and analytics.
 *
 * WHY: DAR learns across sessions through the session database.
 *      The orchestrator propagates database to all phases.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Enable session database for the DAR orchestrator.
 *
 * WHY: Connect DAR to cross-session tracking. Propagates database
 *      to DETECT, ASSESS, and RESTORE phases.
 *
 * @param[in,out] orch        Orchestrator.
 * @param[in]     db          SessionDB pointer (from session_db_open).
 * @param[in]     session_id  Current session ID string.
 *
 * @post orch->db_enabled is true
 * @post All linked phases have database enabled
 */
void cpisi_dar_enable_session_db(DAROrchestrator* orch, void* db, const char* session_id);

/**
 * @brief Check if session database is enabled.
 *
 * @param[in] orch  Orchestrator.
 *
 * @return true if database integration is enabled.
 */
bool cpisi_dar_session_db_enabled(const DAROrchestrator* orch);

/** @} */ /* end S.4e2 SESSION_DB */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4f REPORTING — Stats and Diagnostics [REPORTING]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_dar_reporting S.4f REPORTING — Stats and Diagnostics
 * @ingroup cpisi_dar_prototypes
 * @brief    Query orchestrator state and statistics.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Dump orchestrator state to debug output.
 *
 * @param[in] orch  Orchestrator.
 */
void cpisi_dar_dump_state(const DAROrchestrator* orch);

/**
 * @brief Dump cycle statistics to debug output.
 *
 * @param[in] orch  Orchestrator.
 */
void cpisi_dar_dump_stats(const DAROrchestrator* orch);

/**
 * @brief Set callbacks for orchestrator events.
 *
 * @param[in,out] orch              Orchestrator.
 * @param[in]     on_phase_change   Called when phase changes.
 * @param[in]     on_cycle_complete Called when cycle completes.
 * @param[in]     on_state_change   Called when cognitive state changes.
 * @param[in]     user_data         User context for callbacks.
 */
void cpisi_dar_set_callbacks(DAROrchestrator* orch,
                             void (*on_phase_change)(DARPhase, DARPhase, void*),
                             void (*on_cycle_complete)(DARCycleResult, void*),
                             void (*on_state_change)(HebrewState, HealthScore, void*),
                             void* user_data);

/** @} */ /* end S.4f REPORTING */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — Global Orchestrator State [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_file S.5 FILE-LEVEL — Global Orchestrator State
 * @brief    Global orchestrator for convenience macros.
 *
 * WHY: Simple use cases can use a global orchestrator.
 *      Complex code should use explicit orchestrator passing.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Global DAR orchestrator for convenience macros.
 *
 * @note Set via cpisi_dar_init() or cpisi_dar_set_global().
 * @note Thread safety: Not thread-safe. Use explicit orchestrator in threaded code.
 */
extern DAROrchestrator* g_cpisi_dar;

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_omission S.6 OMISSION — Reserved Section Guide
 * @brief    Documents any reserved sections.
 *
 * RESERVED IN BODY:
 *   B.2 HELPERS  — Helper functions are static in dar.c
 *   B.3 CORE OPS — Operations are in S.4 PROTOTYPES (this is a header)
 *   B.4 ERRORS   — Error handling in phase headers
 *
 * WHY BODY IS MOSTLY DOCUMENTATION:
 *   This is a header file. Declarations in SETUP, implementations in source.
 *   BODY provides architecture documentation and reserved section explanations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: S.6 is documentation-only — no code content] */

/** @} */ /* end S.6 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Implementation documentation for the orchestrator.
///
/// @defgroup cpisi_dar_body BODY BLOCK — Implementation
///
/// CONTAINS:
///   - B.1 ORG CHART   — DAR architecture overview
///   - B.2 HELPERS     — [Reserved: static in dar.c]
///   - B.3 CORE OPS    — [Reserved: declarations in S.4]
///   - B.4 ERRORS      — [Reserved: in phase headers]
///   - B.5 PUBLIC API  — API summary
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — DAR Architecture [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_orgchart B.1 ORG CHART — DAR Architecture
 * @brief    Structure overview for the DAR witness system.
 *
 * DAR FLOW DIAGRAM:
 * @code
 *   ┌─────────────────────────────────────────────────────────────────┐
 *   │                    DAR WITNESS SYSTEM                          │
 *   │          "Let us search and try our ways" — Lam 3:40           │
 *   ├─────────────────────────────────────────────────────────────────┤
 *   │                                                                 │
 *   │   ┌──────────┐    ┌──────────┐    ┌──────────┐                 │
 *   │   │  DETECT  │───▶│  ASSESS  │───▶│ RESTORE  │                 │
 *   │   │   (-1)   │    │   (0)    │    │   (+1)   │                 │
 *   │   │ witness  │    │ evaluate │    │ recover  │                 │
 *   │   └──────────┘    └──────────┘    └──────────┘                 │
 *   │        │               │               │                        │
 *   │        └───────────────┴───────────────┘                        │
 *   │                        │                                        │
 *   │                        ▼                                        │
 *   │              ┌─────────────────┐                               │
 *   │              │  ORCHESTRATOR   │                               │
 *   │              │   (dar.h)       │                               │
 *   │              └─────────────────┘                               │
 *   │                        │                                        │
 *   │        ┌───────────────┼───────────────┐                        │
 *   │        ▼               ▼               ▼                        │
 *   │   ┌──────────┐    ┌──────────┐    ┌──────────┐                 │
 *   │   │ ROTATION │    │  STATE   │    │  PANEL   │                 │
 *   │   │  (I/O)   │    │  (sync)  │    │  (viz)   │                 │
 *   │   └──────────┘    └──────────┘    └──────────┘                 │
 *   │                                                                 │
 *   └─────────────────────────────────────────────────────────────────┘
 * @endcode
 *
 * LADDER CHAIN:
 * @code
 *   types.h (foundation)
 *       ↓
 *   rotation.h (I/O support)
 *       ↓
 *   detect.h → assess.h → restore.h (phases in flow order)
 *       ↓
 *   dar.h (THIS — orchestrator)
 *       ↓
 *   cpisi.h (main CPI-SI header)
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [B.1 ORG CHART is documentation-only — diagrams above] */

/** @} */ /* end B.1 ORG CHART */

/**
 * B.2 HELPERS — Foundation Functions [HELPERS]
 * @defgroup cpisi_dar_helpers B.2 HELPERS
 * WHY RESERVED: Helper functions are static in dar.c.
 *   Not exposed in header — internal implementation details.
 * @{ */
/* [Reserved: B.2 HELPERS — static in dar.c] */
/** @} */

/**
 * B.3 CORE OPS — Operations [OPERATIONS]
 * @defgroup cpisi_dar_core_ops B.3 CORE OPS
 * WHY RESERVED: All operations declared in S.4 PROTOTYPES.
 *   This is a header file — declarations only.
 * @{ */
/* [Reserved: B.3 CORE OPS — see S.4 PROTOTYPES] */
/** @} */

/**
 * B.4 ERRORS — Error Handling [ERRORS]
 * @defgroup cpisi_dar_errors B.4 ERRORS
 * WHY RESERVED: Error handling in each phase header.
 *   Orchestrator delegates errors to appropriate phase.
 * @{ */
/* [Reserved: B.4 ERRORS — in phase headers] */
/** @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Interface Summary [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_public_api B.5 PUBLIC API — Interface Summary
 * @brief    Summary of the orchestrator public interface.
 *
 * LIFECYCLE:
 *   cpisi_dar_init()      — Initialize orchestrator
 *   cpisi_dar_shutdown()  — Shutdown orchestrator
 *
 * LINKING:
 *   cpisi_dar_link_detect()  — Link DETECT phase
 *   cpisi_dar_link_assess()  — Link ASSESS phase
 *   cpisi_dar_link_restore() — Link RESTORE phase
 *   cpisi_dar_link_all()     — Link all phases at once
 *
 * CYCLE:
 *   cpisi_dar_run_cycle()       — Run complete DAR cycle (orchestrator)
 *   cpisi_dar_cycle_if_needed() — Conditional cycle
 *   cpisi_dar_advance_phase()   — Manual phase advancement
 *
 * STATE SYNC:
 *   cpisi_dar_sync_to_phases()   — Push state to phases
 *   cpisi_dar_sync_from_phase()  — Pull state from active phase
 *   cpisi_dar_set_hebrew_state() — Set Hebrew state globally
 *   cpisi_dar_set_health()       — Set health globally
 *   cpisi_dar_set_k_factor()     — Set k-factor globally
 *
 * PANEL:
 *   cpisi_dar_panel_enable()       — Enable/disable panel updates
 *   cpisi_dar_panel_needs_update() — Check if update needed
 *   cpisi_dar_panel_get_phase()    — Get current phase
 *   cpisi_dar_panel_get_stats()    — Get cycle statistics
 *   cpisi_dar_orch_phase_str()     — Get orchestrator phase name string
 *
 * REPORTING:
 *   cpisi_dar_dump_state()    — Debug dump of state
 *   cpisi_dar_dump_stats()    — Debug dump of statistics
 *   cpisi_dar_set_callbacks() — Set event callbacks
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [B.5 PUBLIC API is documentation-only — summary above] */

/** @} */ /* end B.5 PUBLIC API */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Guidance for using and extending the DAR system.
///
/// @defgroup cpisi_dar_closing CLOSING BLOCK — Guidance
///
/// CONTAINS:
///   - X.1 VALIDATION      — How to verify DAR is working
///   - X.2 EXECUTION       — Usage examples
///   - X.3 CLEANUP         — Shutdown patterns
///   - X.4 POLICY          — Modification guidelines
///   - X.5 EXTENSION       — How to expand
///   - X.6 TROUBLESHOOTING — Common problems
///   - X.7 REFERENCE       — Quick lookup
///   - X.8 NOTE            — Final grounding
///   - X.9-X.10            — [Reserved]
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_validation X.1 VALIDATION — Verify Correctness
 * @brief    How to verify DAR system is working.
 *
 * BUILD VERIFICATION:
 *   #include "witness/manager/dar.mgr.h" — should compile without errors
 *
 * RUNTIME VERIFICATION:
 *   DAR_IS_INITIALIZED(ctx) — returns true after init
 *   DAR_HEALTH(ctx) — returns valid health score
 *   DAR_HEBREW_STATE(ctx) — returns valid state
 *
 * ORCHESTRATOR VERIFICATION:
 *   orch->initialized == true after cpisi_dar_init()
 *   orch->detect != NULL after cpisi_dar_link_detect()
 *   cpisi_dar_run_cycle() returns DAR_CYCLE_SUCCESS on healthy system
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [X.1 VALIDATION is documentation-only] */

/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Examples [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_execution X.2 EXECUTION — Usage Examples
 * @brief    How to use the DAR system.
 *
 * BASIC USAGE:
 * @code
 *   #include "witness/manager/dar.mgr.h"
 *
 *   // Initialize phases
 *   DetectContext detect;
 *   AssessContext assess;
 *   RestoreContext restore;
 *   DAROrchestrator orch = DAR_ORCHESTRATOR_INIT;
 *
 *   cpisi_detect_init(&detect, DETECT_MODE_RUNTIME);
 *   cpisi_assess_init(&assess, ASSESS_MODE_FULL);
 *   cpisi_restore_init(&restore, RESTORE_STRATEGY_FULL);
 *   cpisi_dar_init(&orch);
 *
 *   // Link phases to orchestrator
 *   cpisi_dar_link_all(&orch, &detect, &assess, &restore);
 *
 *   // Run DAR cycle
 *   DARCycleResult result = cpisi_dar_run_cycle(&orch);
 *   if (result == DAR_CYCLE_SUCCESS) {
 *       printf("DAR cycle completed successfully\n");
 *   }
 *
 *   // Shutdown (reverse order)
 *   cpisi_dar_shutdown(&orch);
 *   cpisi_restore_shutdown(&restore);
 *   cpisi_assess_shutdown(&assess);
 *   cpisi_detect_shutdown(&detect);
 * @endcode
 *
 * CONDITIONAL CYCLE:
 * @code
 *   // Only run cycle if health is below 50
 *   DARCycleResult result = cpisi_dar_cycle_if_needed(&orch, 50);
 *   if (result == DAR_CYCLE_SKIPPED) {
 *       printf("Health is good, cycle skipped\n");
 *   }
 * @endcode
 *
 * PANEL INTEGRATION:
 * @code
 *   cpisi_dar_panel_enable(&orch, true);
 *
 *   while (running) {
 *       if (cpisi_dar_panel_needs_update(&orch)) {
 *           DARCycleStats stats;
 *           cpisi_dar_panel_get_stats(&orch, &stats);
 *           update_panel_display(&stats);
 *       }
 *   }
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [X.2 EXECUTION is documentation-only — examples above] */

/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Shutdown Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_cleanup X.3 CLEANUP — Shutdown Patterns
 * @brief    How to properly shut down DAR phases.
 *
 * SHUTDOWN ORDER (reverse of init):
 *   1. cpisi_dar_shutdown(&orch)      — Orchestrator first
 *   2. cpisi_restore_shutdown(&restore)
 *   3. cpisi_assess_shutdown(&assess)
 *   4. cpisi_detect_shutdown(&detect)
 *
 * WHY ORDER MATTERS:
 *   Orchestrator references all phases — shutdown first to break links.
 *   RESTORE may reference ASSESS patterns.
 *   ASSESS may reference DETECT history.
 *   Shutdown in reverse order to break dependencies cleanly.
 *
 * OWNERSHIP:
 *   Orchestrator does NOT own phase contexts — just links to them.
 *   Caller is responsible for allocating and freeing phase contexts.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [X.3 CLEANUP is documentation-only] */

/** @} */ /* end X.3 CLEANUP */

/// ─────────────────────────────────────────────────────────────────────────────
/// END CODE SECTIONS [END_CODE_SECTIONS]
/// ─────────────────────────────────────────────────────────────────────────────

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_policy X.4 POLICY — Modification Guidelines
 * @brief    Guidelines for modifying this component.
 *
 * DO:
 *   - Maintain ladder chain order in S.1 INCLUDES
 *   - Add new orchestration functions to appropriate S.4 subsection
 *   - Update M.10 ROADMAP when adding features
 *   - Keep phase contexts as linked, not owned
 *   - Synchronize state changes across all phases
 *
 * DO NOT:
 *   - Reorder phase includes (breaks dependencies)
 *   - Have orchestrator own phase contexts (caller owns)
 *   - Modify phase state directly (use sync functions)
 *   - Add phase-specific logic here (belongs in phase headers)
 *
 * VERSION UPDATES:
 *   - Bug fixes: increment completion (a-03.25 → a-03.26)
 *   - Features: increment phase (a-03.25 → a-04.00)
 *   - Breaking changes: increment stage (a-03.25 → b-01.00)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [X.4 POLICY is documentation-only] */

/** @} */ /* end X.4 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_extension X.5 EXTENSION — How to Expand
 * @brief    How to extend the DAR orchestrator.
 *
 * ADDING A NEW PHASE:
 *   1. Create phase header in phases/ subfolder
 *   2. Add include to S.1c PHASES (in flow order)
 *   3. Add link function to S.4b LINKING
 *   4. Update DARPhase enum in S.3a ENUMS
 *   5. Update cycle logic in cpisi_dar_run_cycle()
 *
 * ADDING ORCHESTRATION FUNCTIONS:
 *   1. Add prototype to appropriate S.4 subsection
 *   2. Document with full docstring (WHY, @param, @return)
 *   3. Update B.5 PUBLIC API summary
 *   4. Implement in dar.c
 *
 * ADDING PANEL FEATURES:
 *   1. Add function to S.4e PANEL
 *   2. Add data to DARCycleStats if needed
 *   3. Update panel update logic
 *
 * @see M.10 ROADMAP for planned extensions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [X.5 EXTENSION is documentation-only] */

/** @} */ /* end X.5 EXTENSION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @brief    Solutions to common issues.
 *
 * PROBLEM: "Unknown type 'DetectContext'"
 *   CAUSE: Phase headers not included properly
 *   FIX: Include dar.h, not individual phase headers
 *
 * PROBLEM: "Orchestrator not initialized"
 *   CAUSE: cpisi_dar_init() not called
 *   FIX: Call cpisi_dar_init(&orch) before use
 *
 * PROBLEM: "Phase not linked"
 *   CAUSE: cpisi_dar_link_*() not called
 *   FIX: Link all phases before running cycle
 *
 * PROBLEM: "State out of sync"
 *   CAUSE: Phase state modified directly
 *   FIX: Use cpisi_dar_set_hebrew_state() etc. for unified changes
 *
 * PROBLEM: "Panel not updating"
 *   CAUSE: Panel not enabled or interval too long
 *   FIX: Call cpisi_dar_panel_enable(&orch, true)
 *        Check CPISI_DAR_PANEL_INTERVAL_MS
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [X.6 TROUBLESHOOTING is documentation-only] */

/** @} */ /* end X.6 TROUBLESHOOTING */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick reference for common operations.
 *
 * PHASE VALUES:
 *   CPISI_DAR_DETECT  = -1
 *   CPISI_DAR_ASSESS  =  0
 *   CPISI_DAR_RESTORE = +1
 *
 * MACROS:
 *   DAR_IS_INITIALIZED(ctx) — check if initialized
 *   DAR_HEBREW_STATE(ctx)   — get Hebrew state
 *   DAR_HEALTH(ctx)         — get health score
 *   DAR_K_FACTOR(ctx)       — get k-factor
 *   DAR_IS_CRITICAL(ctx)    — check if critical
 *   DAR_IS_HEALTHY(ctx)     — check if healthy
 *
 * CYCLE RESULTS:
 *   DAR_CYCLE_SUCCESS  = 0
 *   DAR_CYCLE_PARTIAL  = 1
 *   DAR_CYCLE_DEGRADED = 2
 *   DAR_CYCLE_FAILED   = 3
 *   DAR_CYCLE_SKIPPED  = 4
 *
 * KEY FILES:
 *   Spec: word/core/os/health/dar.toml
 *   Template: word/seed/code/c/header.h
 *   Phases: dar/phases/{detect,assess,restore}.h
 *   I/O: dar/io/rotation.h
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [X.7 REFERENCE is documentation-only] */

/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Grounding [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar_note X.8 NOTE — Final Grounding
 * @brief    Closing wisdom and biblical anchor.
 *
 * "Let us search and try our ways, and turn again to the LORD."
 *   — Lamentations 3:40 KJV
 *
 * The DAR system embodies this principle:
 *   - DETECT = "search" — witness what's happening
 *   - ASSESS = "try our ways" — evaluate the evidence
 *   - RESTORE = "turn again" — correct course
 *
 * The orchestrator coordinates this process, ensuring the three phases
 * work together in harmony. Like a conductor leading an orchestra,
 * the orchestrator ensures each phase plays its part at the right time.
 *
 * "Without counsel purposes are disappointed: but in the multitude
 *  of counsellors they are established." — Proverbs 15:22 KJV
 *
 * The three phases are the counsellors. The orchestrator ensures
 * their counsel is heard and applied.
 *
 * "He restoreth my soul: he leadeth me in the paths of righteousness
 *  for his name's sake." — Psalm 23:3 KJV
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [X.8 NOTE is documentation-only — grounding above] */

/** @} */ /* end X.8 NOTE */

/**
 * X.9 TEMPLATE — Template Guidance [TEMPLATE]
 * @defgroup cpisi_dar_template X.9 TEMPLATE
 * WHY RESERVED: This is an instance, not a template.
 *   See word/seed/code/c/header.h for template guidance.
 * @{ */
/* [Reserved: X.9 TEMPLATE — this is an instance] */
/** @} */

/**
 * X.10 OMISSION — Reserved Section Summary [OMISSION]
 * @defgroup cpisi_dar_closing_omission X.10 OMISSION
 * WHY RESERVED: All reserved sections documented in S.6 OMISSION.
 * @{ */
/* [Reserved: X.10 — see S.6 OMISSION for full list] */
/** @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_MODE >= 2 - Full DAR Orchestrator */

#endif /* CORNERSTONE_CORE_CPISI_DAR_H */
