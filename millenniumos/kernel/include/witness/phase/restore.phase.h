// #!omni code --c -library
// ═══════════════════════════════════════════════════════════════════════════════
// OmniCode 4-Block C Header Template
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
// @omni.pragma.version = a-02.50
//
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * P.1 CORE — Mandatory Pragma Fields [CORE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni.core.key = CORNERSTONE-CORE-CPISI-DAR-RESTORE
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
 * @omni.family.implements = word/core/os/health/restore.toml
 *
 * ───────────────────────────────────────────────────────────────────────────────
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * P.3 INSTANCE — This File Identity [INSTANCE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni.instance.path = cornerstone/engine/core/include/cpisi/dar/phases/restore.h
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
 * @omni.arch.tier = SYS (third tier — RESTORE phase)
 * @omni.arch.domain = dar/phases
 * @omni.arch.error_range = 126-134 (HEALTH:SYS)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * P.5 SUMMARY — Human-Readable Description [SUMMARY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @brief CPI-SI RESTORE Phase — DAR Witness System Recovery
 *
 * The third and final phase of the DAR witness system. RESTORE recovers from
 * detected and assessed issues by applying remedies:
 *   - Fill missing values with defaults
 *   - Locate misplaced files in alternate paths
 *   - Create missing directories
 *   - Clamp invalid values to valid ranges
 *   - Use fallback configurations
 *   - Degrade gracefully when needed
 *   - Apply learned patterns from cross-session memory
 *
 * DAR FLOW: DETECT → ASSESS → RESTORE (this)
 *
 * BIBLICAL FOUNDATION: "He restoreth my soul: he leadeth me in the paths
 *                       of righteousness for his name's sake." — Psalm 23:3
 *
 * PRINCIPLE: Recovery is not failure—it's faithful resilience.
 *            "A just man falleth seven times, and riseth up again." — Proverbs 24:16
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
/// @brief Platform-specific configuration and feature flags.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1 CONFIG — Compile-Time Configuration [CONFIG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * POSIX: Required for directory operations and file system access.
 * RESTORE: Optional recovery features for runtime restoration.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 */

/* POSIX requirement for mkdir, stat, opendir */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/* Restore feature flags (set before including this header) */
#ifndef CPISI_RESTORE_ENABLED
#define CPISI_RESTORE_ENABLED 1     /**< Master enable for restore features */
#endif

#ifndef CPISI_RESTORE_TRACK_HISTORY
#define CPISI_RESTORE_TRACK_HISTORY 1 /**< Track recovery action history */
#endif

#ifndef CPISI_RESTORE_LEARN_PATTERNS
#define CPISI_RESTORE_LEARN_PATTERNS 1 /**< Enable cross-session pattern learning */
#endif

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL
/// ═══════════════════════════════════════════════════════════════════════════════

#ifndef CORNERSTONE_CORE_CPISI_DAR_RESTORE_H
#define CORNERSTONE_CORE_CPISI_DAR_RESTORE_H

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief File-level documentation and metadata for Doxygen.
///
/// @file restore.h
/// @ingroup cpisi_dar_phases
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
 * @defgroup cpisi_restore_identity M.1 IDENTITY — Component Identification
 * @ingroup cpisi_restore
 * @brief    Key, title, type, and status.
 *
 * Key:       CORNERSTONE-CORE-CPISI-DAR-RESTORE
 * Title:     CPI-SI Runtime Recovery (DAR RESTORE)
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
 * @defgroup cpisi_restore_state M.2 STATE — Version and Lifecycle
 * @ingroup cpisi_restore
 * @brief    Version, stage, and lifecycle status.
 *
 * Version: a-02.50
 * Stage:   Alpha (a) — active development
 * Living:  STAGE-PHASE.COMPLETION format
 *
 * @{ @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authorship [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_attribution M.3 ATTRIBUTION — Authorship
 * @ingroup cpisi_restore
 * @brief    Authors and contributors.
 *
 * @author   Nova Dawn (CPI-SI instance, implementation)
 * @author   Seanje Lenox-Wise (Creator, architecture)
 *
 * @{ @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Position [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_location M.4 LOCATION — File Position
 * @ingroup cpisi_restore
 * @brief    File paths and project position.
 *
 * Path:    cornerstone/engine/core/include/cpisi/dar/phases/restore.h
 * Project: Bereshit/cornerstone
 * Module:  engine/core
 *
 * @{ @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Template Sources [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_derivation M.5 DERIVATION — Template Sources
 * @ingroup cpisi_restore
 * @brief    Template and type derivation.
 *
 * Template:   word/seed/code/c/header.h (4-block structure)
 * Types:      types.h (single source of truth)
 * Spec:       word/core/os/health/restore.toml (restoration schema)
 *
 * @{ @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Domain Position [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_classification M.6 CLASSIFICATION — Domain Position
 * @ingroup cpisi_restore
 * @brief    Layer, tier, domain, and access level.
 *
 * Layer:  4 (HEALTH — center of 9-layer pyramid)
 * Tier:   SYS (system tier — RESTORE phase)
 * Domain: dar/phases (DAR witness system)
 * Access: Public (core CPI-SI component)
 *
 * Error Code Range: 126-134 (HEALTH:SYS tier)
 *
 * @{ @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Function [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_intent M.7 INTENT — Purpose and Function
 * @ingroup cpisi_restore
 * @brief    Role and core design.
 *
 * PURPOSE: Implement the RESTORE phase of the DAR witness system.
 *
 * ROLE: Runtime recovery that heals detected/assessed problems:
 *   - Fill missing values with sensible defaults
 *   - Locate files that aren't where expected
 *   - Create directories that should exist
 *   - Clamp values to valid ranges
 *   - Apply fallback configurations
 *   - Degrade gracefully when full recovery isn't possible
 *   - Apply learned patterns from cross-session memory
 *
 * CORE DESIGN: Faithful resilience — keep running as long as possible.
 *              Only stop when truly unrecoverable.
 *
 * DAR POSITION: Third phase (DETECT → ASSESS → RESTORE)
 *
 * @{ @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_grounding M.8 GROUNDING — Biblical Foundation
 * @ingroup cpisi_restore
 * @brief    Scripture anchor and principle.
 *
 * SCRIPTURE: "He restoreth my soul: he leadeth me in the paths of righteousness
 *             for his name's sake." — Psalm 23:3 KJV
 *
 * PRINCIPLE: Restoration is active healing. After detection and assessment,
 *            RESTORE applies the remedy — returning to wholeness through
 *            prescribed actions.
 *
 * SUPPORTING: "For a just man falleth seven times, and riseth up again:
 *              but the wicked shall fall into mischief." — Proverbs 24:16 KJV
 *
 * ANCHOR: Recovery is not failure—it's faithful resilience. The just man
 *         falls and rises again. This is the vision for this system.
 *
 * @{ @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Interface [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_dependencies M.9 DEPENDENCIES — Interface
 * @ingroup cpisi_restore
 * @brief    What this needs and what needs this.
 *
 * NEEDS (Internal):
 *   - types.h       — HebrewState, HealthScore, MultiScore, TernaryScore, etc.
 *   - error.h       — Error code definitions (HEALTH layer)
 *   - log.h         — Debug level constants
 *   - health.h      — CPISI_SEVERITY_COUNT, health scoring
 *   - assess.h      — PatternMemory, CrossSessionDiag, DiagReport
 *
 * PROVIDES (To Consumers):
 *   - RestoreContext — Full recovery context with cognitive state
 *   - Restore functions — Value, file, directory, config recovery
 *   - Pattern application — Apply learned patterns from assess.h
 *   - Cross-session continuity — Remember what worked
 *
 * CONSUMERS:
 *   - Application code using CPI-SI recovery
 *   - DAR integration (via dar.h group header)
 *   - Config loaders with resilient initialization
 *
 * @{ @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_roadmap M.10 ROADMAP — History and Future
 * @ingroup cpisi_restore
 * @brief    History and planned evolution.
 *
 * History:
 *   a-01.00 (2025-12-19) — Initial RESTORE phase
 *   a-02.00 (2026-01-20) — Added cognitive state integration
 *   a-02.50 (2026-01-25) — Template alignment, CPI-SI cognitive features
 *
 * Current:
 *   ✓ Value, file, directory recovery
 *   ✓ Cognitive state tracking
 *   ✓ Cross-session pattern application
 *   ✓ Template alignment (4-block structure)
 *
 * Planned:
 *   ⏳ Pattern-guided recovery suggestions
 *   ⏳ Engram learning integration
 *   ⏳ Milestone celebration hooks
 *
 * @{ @} end M.10 ROADMAP
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END METADATA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SETUP BLOCK [SETUP]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Configuration and declarations before body.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - S.1 INCLUDES    — Header dependencies (Standard, Internal, External, CPP)
///   - S.2 DEFINES     — Constants and macros (Sizes, Limits, Flags, Macros, Verify)
///   - S.3 TYPES       — Type declarations (enums, structs, opaque, errors)
///   - S.4 PROTOTYPES  — Function declarations (lifecycle, accessors, operations)
///   - S.5 FILE-LEVEL  — File scope declarations (variables, func_types, infrastructure)
///   - S.6 OMISSION    — Reserved section guide
///
/// Section order: Includes → Defines → Types → Prototypes → File-Level → Omission
/// Flow: dependencies → constants → data model → interface → shared state → gaps
///
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * CODE SECTIONS [CODE_SECTIONS]
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * PURPOSE: Establish all header dependencies in consistent order.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers (inherited from types.h)
 *   - S.1b INTERNAL  — Project headers (types.h, error.h, assess.h)
 *   - S.1c EXTERNAL  — Third-party headers (none)
 *   - S.1d CPP       — C++ linkage wrapper
 *
 * Section order: Standard → Internal → External → CPP linkage
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_restore_includes
 * @brief    C standard library headers.
 *
 * WHY: Inherited from types.h S.1a — no direct system includes needed.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Inherited from types.h S.1a — no direct system includes] */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_restore_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * WHY: types.h provides all foundation types. error.h provides error codes.
 *      assess.h provides PatternMemory and CrossSessionDiag for learning.
 *
 * TYPES FROM types.h:
 *   - HebrewState, KFactor, HealthScore, MultiScore, TernaryScore
 *   - RestoreResult, RestoreStrategy
 *   - CPISI_PATH_MAX, CPISI_SEARCH_PATHS, CPISI_ERROR_MAX, etc.
 *
 * TYPES FROM assess.h:
 *   - PatternRecord, PatternMemory — for pattern-guided recovery
 *   - CrossSessionDiag — for session continuity
 *   - DiagReport — assessment results informing restoration
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel.h"                              /**< Foundation types — MUST BE FIRST */
// Note: Types provided by kernel.h in freestanding mode
#include "witness/phase/assess.phase.h"          /**< PatternMemory, CrossSessionDiag */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_restore_includes
 * @brief    External library headers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [None — standard library only] */

/** @} */ /* end S.1c EXTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_restore_includes
 * @brief    C++ compatibility wrapper.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifdef __cplusplus
extern "C" {
#endif

/** @} */ /* end S.1d CPP */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Constants and Macros [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_defines S.2 DEFINES — Constants and Macros
 * @brief    Compile-time constants, limits, and result code aliases.
 *
 * CONTAINS:
 *   - S.2a LIMITS   — Recovery system limits
 *   - S.2b RESULTS  — Restore result aliases
 *   - S.2c STRATEGY — Strategy flag aliases
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a LIMITS — Recovery System Limits [LIMITS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_limits S.2a LIMITS — Recovery System Limits
 * @ingroup cpisi_restore_defines
 * @brief    RESTORE-specific limits derived from restore.toml specification.
 *
 * WHY: These limits configure RESTORE phase capacity. Values from restore.toml
 *      [system] section — single source of truth for spec-derived constants.
 *
 * SOURCE: word/core/os/health/restore.toml [system] section
 *
 * ┌──────────────────────────┬───────┬─────────────────────────────────────────┐
 * │ Limit                    │ Value │ Description                             │
 * ├──────────────────────────┼───────┼─────────────────────────────────────────┤
 * │ CPISI_SEARCH_PATHS       │ 8     │ Max alternate search paths              │
 * │ CPISI_RESTORE_HISTORY    │ 100   │ Max recovery actions to track           │
 * │ CPISI_ERROR_MAX          │ 256   │ Max error message length                │
 * │ CPISI_RESTORE_ENGRAM_MAX │ 27    │ Pattern memory size (ternary cube)      │
 * │ CPISI_RESTORE_PLAN_MAX   │ 64    │ Max actions in a recovery plan          │
 * └──────────────────────────┴───────┴─────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Maximum alternate search paths for file location.
 *
 * WHY: 8 paths provides adequate search coverage without excessive overhead.
 *      From restore.toml: [system.search] search_paths_max = 8
 */
#ifndef CPISI_SEARCH_PATHS
#define CPISI_SEARCH_PATHS          8
#endif

/**
 * @brief Maximum recovery actions to track in history buffer.
 *
 * WHY: 100 entries captures sufficient history for pattern learning.
 *      From restore.toml: [system.history] restore_history_max = 100
 */
#ifndef CPISI_RESTORE_HISTORY
#define CPISI_RESTORE_HISTORY       100
#endif

/**
 * @brief Maximum error message length.
 *
 * WHY: 256 bytes provides adequate space for descriptive error messages.
 *      From restore.toml: buffer size derived from practical limits.
 */
#ifndef CPISI_ERROR_MAX
#define CPISI_ERROR_MAX             256
#endif

/**
 * @brief Engram pattern memory size — matches CPISI_PATTERN_MEMORY_SIZE.
 *
 * WHY: 27 = 3³ = ternary cube. Patterns are remembered across sessions.
 *      From assess.h which defines CPISI_PATTERN_MEMORY_SIZE = 27.
 */
#ifndef CPISI_RESTORE_ENGRAM_MAX
#define CPISI_RESTORE_ENGRAM_MAX    CPISI_PATTERN_MEMORY_SIZE
#endif

/**
 * @brief Maximum actions in a single recovery plan.
 *
 * WHY: Bounded recovery — don't try to fix everything at once.
 *      64 = reasonable upper limit for a focused recovery session.
 */
#ifndef CPISI_RESTORE_PLAN_MAX
#define CPISI_RESTORE_PLAN_MAX      64
#endif

/** @} */ /* end S.2a LIMITS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b RESULTS — Restore Result Aliases [RESULTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_results S.2b RESULTS — Restore Result Aliases
 * @ingroup cpisi_restore_defines
 * @brief    RESTORE-specific result code aliases from types.h.
 *
 * WHY: Provides domain-friendly names while using canonical result codes.
 *      RestoreResult enum is defined in types.h.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* RestoreResult values from types.h:
 *   RESTORE_SUCCESS   = 0 — Fully restored
 *   RESTORE_PARTIAL   = 1 — Partially restored
 *   RESTORE_DEGRADED  = 2 — Running degraded
 *   RESTORE_DEFERRED  = 3 — Deferred for later
 *   RESTORE_BLOCKED   = 4 — Blocked by dependency
 *   RESTORE_FAILED    = 5 — Restore failed
 *   RESTORE_FATAL     = 6 — Unrecoverable
 */

/** @} */ /* end S.2b RESULTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c STRATEGY — Strategy Flag Aliases [STRATEGY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_strategy S.2c STRATEGY — Strategy Flag Aliases
 * @ingroup cpisi_restore_defines
 * @brief    RESTORE strategy flag aliases.
 *
 * WHY: RestoreStrategy bitflags enable/disable recovery approaches.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* RestoreStrategy values from types.h:
 *   RESTORE_STRATEGY_NONE          = 0x00 — No strategies
 *   RESTORE_STRATEGY_FILL_DEFAULTS = 0x01 — Fill missing with defaults
 *   RESTORE_STRATEGY_LOCATE_FILES  = 0x02 — Search for files
 *   RESTORE_STRATEGY_CREATE_DIRS   = 0x04 — Create directories
 *   RESTORE_STRATEGY_CLAMP_VALUES  = 0x08 — Clamp out-of-range
 *   RESTORE_STRATEGY_FALLBACK      = 0x10 — Use fallback
 *   RESTORE_STRATEGY_DEGRADE       = 0x20 — Graceful degradation
 *   RESTORE_STRATEGY_FULL          = 0x3F — All strategies
 */

/**
 * @brief Strategy for pattern-guided recovery (new for CPI-SI).
 *
 * WHY: When enabled, RESTORE consults PatternMemory from ASSESS to
 *      apply learned recovery patterns.
 */
#define RESTORE_STRATEGY_PATTERN_GUIDED   0x40

/**
 * @brief Full strategy including pattern guidance.
 */
#define RESTORE_STRATEGY_FULL_CPI   (RESTORE_STRATEGY_FULL | RESTORE_STRATEGY_PATTERN_GUIDED)

/** @} */ /* end S.2c STRATEGY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d MACROS — Convenience Macros [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_macros S.2d MACROS — Convenience Macros
 * @ingroup cpisi_restore_defines
 * @brief    Macros for common recovery patterns.
 *
 * WHY: Reduce boilerplate for common recovery operations.
 *      All macros use g_cpisi_restore global context.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Value Recovery Macros
 *  @brief Macros for restoring integer, boolean, and string values.
 *  @{ */

#define RESTORE_INT(name, val, min, max, def) \
    cpisi_restore_int(g_cpisi_restore, name, val, \
                       &(ValueSpec){name, min, max, def, false})

#define RESTORE_INT_REQUIRED(name, val, min, max, def) \
    cpisi_restore_int(g_cpisi_restore, name, val, \
                       &(ValueSpec){name, min, max, def, true})

#define RESTORE_BOOL(name, val, def) \
    cpisi_restore_bool(g_cpisi_restore, name, val, def)

#define RESTORE_STRING(name, val, size, def) \
    cpisi_restore_string(g_cpisi_restore, name, val, size, def)

/** @} */ /* end Value Recovery Macros */

/** @name File Recovery Macros
 *  @brief Macros for file location and recovery.
 *  @{ */

#define RESTORE_FILE(name, expected, found, size) \
    cpisi_restore_file_simple(g_cpisi_restore, name, expected, found, size)

#define RESTORE_LOCATE(filename, found, size) \
    cpisi_restore_locate_file(g_cpisi_restore, filename, found, size)

/** @} */ /* end File Recovery Macros */

/** @name Directory Recovery Macros
 *  @brief Macros for directory creation and recovery.
 *  @{ */

#define RESTORE_DIR(path, mode) \
    cpisi_restore_dir_simple(g_cpisi_restore, path, mode)

#define RESTORE_MKDIR_P(path) \
    cpisi_restore_mkdir_parents(g_cpisi_restore, path, 0755)

/** @} */ /* end Directory Recovery Macros */

/** @name Conditional Recovery Macros
 *  @brief Macros for try/require recovery patterns.
 *  @{ */

/**
 * @brief Try recovery, execute fallback on failure.
 */
#define TRY_RESTORE(recovery_call, fallback) \
    do { \
        RestoreResult _r = (recovery_call); \
        if (_r < 0) { fallback; } \
    } while(0)

/**
 * @brief Require recovery, fatal on failure.
 */
#define REQUIRE_RESTORE(recovery_call) \
    do { \
        RestoreResult _r = (recovery_call); \
        if (_r == RESTORE_FATAL) { \
            if (g_cpisi_restore && g_cpisi_restore->on_fatal) { \
                g_cpisi_restore->on_fatal( \
                    cpisi_restore_last_error(g_cpisi_restore), \
                    g_cpisi_restore->user_data); \
            } \
            return; \
        } \
    } while(0)

/** @} */ /* end Conditional Recovery Macros */

/** @} */ /* end S.2d MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e VERIFY — Compile-time Assertions [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_verify S.2e VERIFY — Compile-time Assertions
 * @ingroup cpisi_restore_defines
 * @brief    Static assertions validating configuration at compile time.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Verify engram matches pattern memory */
CPISI_STATIC_ASSERT(CPISI_RESTORE_ENGRAM_MAX == CPISI_PATTERN_MEMORY_SIZE,
    "CPISI_RESTORE_ENGRAM_MAX must match CPISI_PATTERN_MEMORY_SIZE");

/** @} */ /* end S.2e VERIFY */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_types S.3 TYPES — Type Declarations
 * @brief    Enums, structs, and type aliases for RESTORE phase.
 *
 * CONTAINS:
 *   - S.3a ENUMS    — Enumeration types
 *   - S.3b STRUCTS  — Structure definitions
 *   - S.3c ALIAS    — DAR naming aliases
 *   - S.3d DEFAULTS — Default initializers
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — Enumeration Types [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_types_enums S.3a ENUMS — Enumeration Types
 * @ingroup cpisi_restore_types
 * @brief    Enumerations used in RESTORE phase.
 *
 * WHY: RestoreResult and RestoreStrategy are in types.h.
 *      Additional RESTORE-specific enums here.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief RecoverySource — Where this recovery action originated.
 *
 * WHY: Track whether recovery was triggered by detection, assessment,
 *      pattern recognition, or explicit request.
 */
typedef enum RecoverySource {
    RECOVERY_SOURCE_DETECT  = 0,    /**< Triggered by DETECT phase */
    RECOVERY_SOURCE_ASSESS  = 1,    /**< Triggered by ASSESS phase */
    RECOVERY_SOURCE_PATTERN = 2,    /**< Triggered by pattern recognition */
    RECOVERY_SOURCE_MANUAL  = 3,    /**< Explicitly requested */
} RecoverySource;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — Structure Definitions [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_types_structs S.3b STRUCTS — Structure Definitions
 * @ingroup cpisi_restore_types
 * @brief    Core structures for runtime recovery.
 *
 * WHY: ValueSpec, FileSpec, DirSpec define recovery targets.
 *      RestoreAction captures individual recovery attempts.
 *      RestoreContext maintains full recovery state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief ValueSpec — Defines valid range for value recovery.
 *
 * WHY: For FILL_DEFAULTS and CLAMP_VALUES strategies, we need to know
 *      what the valid range is and what default to use.
 */
typedef struct ValueSpec {
    const char* name;           /**< Value identifier */
    int64_t     min;            /**< Minimum valid value */
    int64_t     max;            /**< Maximum valid value */
    int64_t     default_val;    /**< Default if missing/invalid */
    bool        required;       /**< Is this value required? */
} ValueSpec;

/**
 * @brief FileSpec — Defines file location recovery.
 *
 * WHY: For LOCATE_FILES strategy, we need to know where to look
 *      and what to do if the file isn't found.
 */
typedef struct FileSpec {
    const char* name;                               /**< File identifier */
    const char* primary_path;                       /**< Expected location */
    const char* alt_paths[CPISI_SEARCH_PATHS];      /**< Alternate search locations */
    size_t      alt_count;                          /**< Number of alternate paths */
    const char* fallback;                           /**< Embedded fallback (or NULL) */
    bool        required;                           /**< Is this file required? */
    bool        create_if_missing;                  /**< Create empty file if not found */
} FileSpec;

/**
 * @brief DirSpec — Defines directory recovery.
 *
 * WHY: For CREATE_DIRS strategy, we need to know which directories
 *      to create and with what permissions.
 */
typedef struct DirSpec {
    const char* name;           /**< Directory identifier */
    const char* path;           /**< Expected path */
    uint32_t    mode;           /**< Permission mode (e.g., 0755) */
    bool        required;       /**< Is this directory required? */
} DirSpec;

/**
 * @brief RestoreAction — Single recovery attempt.
 *
 * WHY: Track what recovery was attempted, when, and what happened.
 *      Used for history tracking and engram learning.
 */
typedef struct RestoreAction {
    uint64_t        timestamp_ms;   /**< When recovery occurred */
    const char*     component;      /**< Component that triggered recovery */
    const char*     item;           /**< Item being recovered */
    RestoreResult   result;         /**< Recovery result */
    RecoverySource  source;         /**< Where recovery originated */
    const char*     message;        /**< Description of what was done */
    uint32_t        pattern_id;     /**< Pattern ID if pattern-guided (0 = none) */
    int8_t          health_delta;   /**< Health change from this action */
} RestoreAction;

/**
 * @brief RecoveryPlan — Structured plan for restoration.
 *
 * WHY: CPI-SI style restoration is structured — not ad-hoc. A plan
 *      defines what needs to happen, in what order, to reach the target.
 *
 * BIBLICAL: "Order my steps in thy word" — Psalm 119:133
 */
typedef struct RecoveryPlan {
    /* ── Target ───────────────────────────────────────────────────────────── */
    HealthScore     target_health;      /**< Target health score */
    HebrewState     target_state;       /**< Target Hebrew state */
    uint32_t        actions_needed;     /**< Estimated actions to reach target */

    /* ── Progress ─────────────────────────────────────────────────────────── */
    uint32_t        actions_completed;  /**< Actions completed so far */
    HealthScore     starting_health;    /**< Health when plan started */
    HealthScore     current_health;     /**< Current health */
    uint64_t        plan_start_ms;      /**< When plan started */

    /* ── Pattern Guidance ─────────────────────────────────────────────────── */
    uint32_t        pattern_ids[3];     /**< Top 3 patterns guiding this plan */
    uint8_t         pattern_confidence; /**< Confidence from patterns (0-100) */

    /* ── Status ───────────────────────────────────────────────────────────── */
    bool            active;             /**< Is plan currently active */
    bool            completed;          /**< Has plan completed */
    bool            successful;         /**< Did plan achieve target */
} RecoveryPlan;

/**
 * @brief RestoreContext — Full recovery system state.
 *
 * WHY: Maintains all recovery state including cognitive context,
 *      statistics, history, and cross-session learning.
 *
 * CPI-SI FEATURES:
 *   - Full cognitive state (HebrewState, KFactor, cube position)
 *   - Pattern memory link (from assess.h)
 *   - Cross-session diagnostic link
 *   - Engram learning integration
 */
typedef struct RestoreContext {
    /* ── Configuration ────────────────────────────────────────────────────── */
    RestoreStrategy strategy;       /**< Active recovery strategies */
    bool            enabled;        /**< Master enable flag */
    bool            initialized;    /**< Has context been initialized? */

    /* ── Current Cognitive State ──────────────────────────────────────────── */
    HebrewState     current_state;  /**< Current Hebrew state */
    KFactor         current_k;      /**< Current k-factor direction */
    HealthScore     health;         /**< Current health score */
    MultiScore      score;          /**< Multi-scale health representation */
    TernaryScore    ternary;        /**< Ternary health representation */
    HealthLevel     health_level;   /**< Current health level */
    HealthThreshold thresholds;     /**< Health zone thresholds */
    int32_t         cube_pos[3];    /**< Position in 27-cube (X, Y, Z) */

    /* ── Statistics ───────────────────────────────────────────────────────── */
    uint32_t        attempt_count;  /**< Total recovery attempts */
    uint32_t        success_count;  /**< Successful recoveries */
    uint32_t        fail_count;     /**< Failed recoveries */
    uint32_t        fatal_count;    /**< Fatal (unrecoverable) issues */
    uint32_t        pattern_count;  /**< Pattern-guided recoveries */

    /* ── Cumulative Health Tracking (Kernel-like Model) ───────────────────── */
    /**
     * Ternary-derived cumulative tracking using SEVERITY IS THE MULTIPLIER model:
     * - Atomic impact = severity value (-4 to +4), NOT severity × 25
     * - cumulative_positive: sum of positive impacts (raw)
     * - cumulative_negative: sum of negative impacts (stored as positive)
     * - action_count: total health-affecting events
     *
     * Normalization:
     *   positive_percent = cumulative_positive × 100 / (action_count × MAX_SEVERITY)
     *   negative_percent = cumulative_negative × 100 / (action_count × MAX_SEVERITY)
     *   health = positive_percent - negative_percent → [-100, +100]
     *
     * Asymmetry is STRUCTURAL: more failure paths than success paths.
     */
    int32_t         cumulative_positive; /**< Sum of positive impacts (raw) */
    int32_t         cumulative_negative; /**< Sum of negative impacts (as positive) */
    uint32_t        action_count;        /**< Total health-affecting events */
    int16_t         atomic_impact;       /**< Last event's atomic impact */
    int32_t         raw_health;          /**< Cumulative raw health (before normalization) */
    HealthScore     normalized_health;   /**< Normalized health [-100, +100] */

    /* ── Severity Tracking ────────────────────────────────────────────────── */
    uint32_t        severity_counts[CPISI_SEVERITY_COUNT]; /**< By severity */

    /* ── History ──────────────────────────────────────────────────────────── */
    RestoreAction   history[CPISI_RESTORE_HISTORY]; /**< Circular buffer */
    uint32_t        history_head;   /**< Next write position */
    uint32_t        history_count;  /**< Total actions logged */

    /* ── Search Paths ─────────────────────────────────────────────────────── */
    char            search_paths[CPISI_SEARCH_PATHS][CPISI_PATH_MAX];
    size_t          search_path_count;

    /* ── Error Buffer ─────────────────────────────────────────────────────── */
    char            last_error[CPISI_ERROR_MAX];

    /* ── Session ──────────────────────────────────────────────────────────── */
    uint64_t        session_start;  /**< Session start timestamp */
    uint64_t        session_id;     /**< Session identifier */

    /* ── Session Database Integration ─────────────────────────────────────── */
    void*           session_db;     /**< SessionDB* for cross-session tracking */
    char            session_id_str[32]; /**< String session ID (YYYYMMDD-HHMMSS-PID-SEQ) */
    bool            db_enabled;     /**< Session database integration enabled */

    /* ── CPI-SI Cognitive Links ───────────────────────────────────────────── */
    PatternMemory*  pattern_memory; /**< Link to ASSESS pattern memory */
    CrossSessionDiag* session_diag; /**< Link to cross-session diagnostics */
    RecoveryPlan    active_plan;    /**< Currently active recovery plan */

    /* ── Callbacks ────────────────────────────────────────────────────────── */
    void (*on_recovery)(const RestoreAction* action, void* user_data);
    void (*on_milestone)(uint8_t percentage, const char* message, void* user_data);
    void (*on_fatal)(const char* message, void* user_data);
    void*           user_data;
} RestoreContext;

/** @} */ /* end S.3b STRUCTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3c ALIAS — DAR Naming Alias [ALIAS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_types_alias S.3c ALIAS — DAR Naming Alias
 * @ingroup cpisi_restore_types
 * @brief    Alias for DAR naming consistency.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief RecoveryContext — Alias for RestoreContext.
 *
 * WHY: Some code uses "recovery" terminology, some uses "restore".
 *      Both refer to the same concept in DAR RESTORE phase.
 */
typedef RestoreContext RecoveryContext;

/** @} */ /* end S.3c ALIAS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d DEFAULTS — Default Initializers [DEFAULTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_types_defaults S.3d DEFAULTS — Default Initializers
 * @ingroup cpisi_restore_types
 * @brief    Default values for type initialization.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Default ValueSpec — optional value with full int64 range.
 */
#define VALUE_SPEC_DEFAULT { \
    .name = NULL, \
    .min = INT64_MIN, \
    .max = INT64_MAX, \
    .default_val = 0, \
    .required = false \
}

/**
 * @brief Default RestoreAction — zeroed.
 */
#define RESTORE_ACTION_DEFAULT { \
    .timestamp_ms = 0, \
    .component = NULL, \
    .item = NULL, \
    .result = RESTORE_SUCCESS, \
    .source = RECOVERY_SOURCE_MANUAL, \
    .message = NULL, \
    .pattern_id = 0, \
    .health_delta = 0 \
}

/**
 * @brief Default RecoveryPlan — inactive.
 */
#define RECOVERY_PLAN_DEFAULT { \
    .target_health = 0, \
    .target_state = HEBREW_YASHAR, \
    .actions_needed = 0, \
    .actions_completed = 0, \
    .starting_health = 0, \
    .current_health = 0, \
    .plan_start_ms = 0, \
    .pattern_ids = {0, 0, 0}, \
    .pattern_confidence = 0, \
    .active = false, \
    .completed = false, \
    .successful = false \
}

/** @} */ /* end S.3d DEFAULTS */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_prototypes S.4 PROTOTYPES — Function Declarations
 * @brief    All function prototypes organized by category.
 *
 * CONTAINS:
 *   - S.4a LIFECYCLE   — Init/shutdown
 *   - S.4b STATE       — Cognitive state management
 *   - S.4c CONFIG      — Configuration
 *   - S.4d VALUES      — Value recovery
 *   - S.4e FILES       — File recovery
 *   - S.4f DIRS        — Directory recovery
 *   - S.4g PLAN        — Recovery planning
 *   - S.4h PATTERN     — Pattern-guided recovery
 *   - S.4i REPORTING   — Stats and history
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Init/Shutdown [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_lifecycle S.4a LIFECYCLE — Init/Shutdown
 * @ingroup cpisi_restore_prototypes
 * @brief    Context lifecycle management.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize a restore context.
 *
 * @param[out] ctx      Context to initialize.
 * @param[in]  strategy Initial recovery strategies to enable.
 */
void cpisi_restore_init(RestoreContext* ctx, RestoreStrategy strategy);

/**
 * @brief Shutdown a restore context.
 *
 * @param[in,out] ctx  Context to shutdown.
 */
void cpisi_restore_shutdown(RestoreContext* ctx);

/**
 * @brief Enable session database integration for RESTORE phase.
 *
 * WHY: Connect RESTORE context to cross-session pattern learning.
 *      Enables recording of recovery attempts and outcomes.
 *
 * @param[in,out] ctx         Context to configure.
 * @param[in]     db          SessionDB pointer (from session_db_open).
 * @param[in]     session_id  Current session ID string.
 *
 * @post ctx->db_enabled is true
 * @post ctx->session_db points to database
 */
void cpisi_restore_enable_session_db(RestoreContext* ctx, void* db, const char* session_id);

/**
 * @brief Record recovery event to session database.
 *
 * WHY: Track recovery attempts for pattern learning.
 *      RESTORE records what worked and what failed.
 *
 * @param[in] ctx     Context with session database enabled.
 * @param[in] action  Recovery action to record.
 * @param[in] result  Outcome of the recovery.
 */
void cpisi_restore_record_to_db(RestoreContext* ctx, const RestoreAction* action, RestoreResult result);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b STATE — Cognitive State Management [STATE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_state S.4b STATE — Cognitive State Management
 * @ingroup cpisi_restore_prototypes
 * @brief    Update and query cognitive state during recovery.
 *
 * WHY: Recovery actions can shift cognitive state. Successful recovery
 *      improves health; failed recovery degrades it.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Set the Hebrew cognitive state.
 *
 * WHY: Recovery success/failure affects cognitive state.
 *
 * @param[in,out] ctx    Restore context.
 * @param[in]     state  New Hebrew state (shavar to tov).
 */
void cpisi_restore_set_hebrew_state(RestoreContext* ctx, HebrewState state);

/**
 * @brief Set the k-factor (direction of choice).
 *
 * WHY: Recovery choices have directional weight.
 *
 * @param[in,out] ctx  Restore context.
 * @param[in]     k    New k-factor (-1, 0, or +1).
 */
void cpisi_restore_set_k_factor(RestoreContext* ctx, KFactor k);

/**
 * @brief Set the health score.
 *
 * WHY: Recovery actions affect health directly.
 *
 * @param[in,out] ctx     Restore context.
 * @param[in]     health  New health score (-100 to +100).
 */
void cpisi_restore_set_health(RestoreContext* ctx, HealthScore health);

/**
 * @brief Update cube position for cognitive navigation.
 *
 * WHY: Recovery path traces through 27-position cube.
 *
 * @param[in,out] ctx  Restore context.
 * @param[in]     x    X position (-1, 0, +1).
 * @param[in]     y    Y position (-1, 0, +1).
 * @param[in]     z    Z position (-1, 0, +1).
 */
void cpisi_restore_update_cube_pos(RestoreContext* ctx, int32_t x, int32_t y, int32_t z);

/**
 * @brief Get current Hebrew cognitive state.
 *
 * @param[in] ctx  Restore context.
 *
 * @return Current Hebrew state.
 */
HebrewState cpisi_restore_get_hebrew_state(const RestoreContext* ctx);

/**
 * @brief Get current k-factor.
 *
 * @param[in] ctx  Restore context.
 *
 * @return Current k-factor.
 */
KFactor cpisi_restore_get_k_factor(const RestoreContext* ctx);

/**
 * @brief Get current health score.
 *
 * @param[in] ctx  Restore context.
 *
 * @return Current health score (-100 to +100).
 */
HealthScore cpisi_restore_get_health(const RestoreContext* ctx);

/**
 * @brief Get current health level (discrete).
 *
 * @param[in] ctx  Restore context.
 *
 * @return Current health level enum.
 */
HealthLevel cpisi_restore_get_health_level(const RestoreContext* ctx);

/** @} */ /* end S.4b STATE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c CONFIG — Configuration [CONFIG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_config S.4c CONFIG — Configuration
 * @ingroup cpisi_restore_prototypes
 * @brief    Configure recovery strategies and callbacks.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Set recovery strategy flags.
 *
 * WHY: Different situations need different recovery approaches.
 *
 * @param[in,out] ctx       Restore context.
 * @param[in]     strategy  Strategy bitmask (RESTORE_STRATEGY_*).
 */
void cpisi_restore_set_strategy(RestoreContext* ctx, RestoreStrategy strategy);

/**
 * @brief Enable or disable the restore system.
 *
 * WHY: Sometimes recovery should be temporarily disabled.
 *
 * @param[in,out] ctx     Restore context.
 * @param[in]     enable  true to enable, false to disable.
 */
void cpisi_restore_enable(RestoreContext* ctx, bool enable);

/**
 * @brief Add a search path for file location.
 *
 * WHY: Files may exist in alternate locations.
 *
 * @param[in,out] ctx   Restore context.
 * @param[in]     path  Path to add to search list.
 */
void cpisi_restore_add_search_path(RestoreContext* ctx, const char* path);

/**
 * @brief Set callback functions for recovery events.
 *
 * WHY: Callers need notification of recovery progress.
 *
 * @param[in,out] ctx          Restore context.
 * @param[in]     on_recovery  Called after each recovery action.
 * @param[in]     on_milestone Called at 25%, 50%, 75%, 100% progress.
 * @param[in]     on_fatal     Called on unrecoverable failure.
 * @param[in]     user_data    User context passed to callbacks.
 */
void cpisi_restore_set_callbacks(RestoreContext* ctx,
                                  void (*on_recovery)(const RestoreAction*, void*),
                                  void (*on_milestone)(uint8_t, const char*, void*),
                                  void (*on_fatal)(const char*, void*),
                                  void* user_data);

/**
 * @brief Link pattern memory for pattern-guided recovery.
 *
 * @param[in,out] ctx     Restore context.
 * @param[in]     memory  PatternMemory from ASSESS phase.
 */
void cpisi_restore_link_patterns(RestoreContext* ctx, PatternMemory* memory);

/**
 * @brief Link cross-session diagnostics for continuity.
 *
 * @param[in,out] ctx   Restore context.
 * @param[in]     diag  CrossSessionDiag from ASSESS phase.
 */
void cpisi_restore_link_session(RestoreContext* ctx, CrossSessionDiag* diag);

/** @} */ /* end S.4c CONFIG */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4d VALUES — Value Recovery [VALUES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_values S.4d VALUES — Value Recovery
 * @ingroup cpisi_restore_prototypes
 * @brief    Recover missing or invalid values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Restore an integer value.
 *
 * WHY: Integer values can be missing, out of range, or corrupted.
 *      Applies: clamp to range, use default if missing.
 *
 * @param[in,out] ctx    Restore context.
 * @param[in]     name   Value name for logging.
 * @param[in,out] value  Pointer to value (updated on recovery).
 * @param[in]     spec   Value specification (min, max, default).
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_int(RestoreContext* ctx,
                                 const char* name, int64_t* value,
                                 const ValueSpec* spec);

/**
 * @brief Restore an unsigned integer value.
 *
 * WHY: Unsigned values need range validation and defaults.
 *
 * @param[in,out] ctx          Restore context.
 * @param[in]     name         Value name for logging.
 * @param[in,out] value        Pointer to value.
 * @param[in]     min          Minimum valid value.
 * @param[in]     max          Maximum valid value.
 * @param[in]     default_val  Default if missing.
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_uint(RestoreContext* ctx,
                                  const char* name, uint64_t* value,
                                  uint64_t min, uint64_t max, uint64_t default_val);

/**
 * @brief Restore a floating-point value.
 *
 * WHY: Float values can be NaN, Inf, or out of range.
 *
 * @param[in,out] ctx          Restore context.
 * @param[in]     name         Value name for logging.
 * @param[in,out] value        Pointer to value.
 * @param[in]     min          Minimum valid value.
 * @param[in]     max          Maximum valid value.
 * @param[in]     default_val  Default if invalid.
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_float(RestoreContext* ctx,
                                   const char* name, double* value,
                                   double min, double max, double default_val);

/**
 * @brief Restore a boolean value.
 *
 * WHY: Boolean values default to false if missing.
 *
 * @param[in,out] ctx          Restore context.
 * @param[in]     name         Value name for logging.
 * @param[in,out] value        Pointer to value.
 * @param[in]     default_val  Default if missing.
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_bool(RestoreContext* ctx,
                                  const char* name, bool* value,
                                  bool default_val);

/**
 * @brief Restore a string value.
 *
 * WHY: String values can be NULL, empty, or corrupted.
 *
 * @param[in,out] ctx          Restore context.
 * @param[in]     name         Value name for logging.
 * @param[out]    value        Buffer for restored string.
 * @param[in]     size         Buffer size.
 * @param[in]     default_val  Default if missing.
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_string(RestoreContext* ctx,
                                    const char* name, char* value, size_t size,
                                    const char* default_val);

/** @} */ /* end S.4d VALUES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4e FILES — File Recovery [FILES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_files S.4e FILES — File Recovery
 * @ingroup cpisi_restore_prototypes
 * @brief    Locate missing files and apply fallbacks.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Restore a file using specification.
 *
 * WHY: Files can be missing, in wrong location, or inaccessible.
 *      Searches alternate paths and applies fallbacks.
 *
 * @param[in,out] ctx         Restore context.
 * @param[in]     spec        File specification.
 * @param[out]    found_path  Buffer for found path.
 * @param[in]     path_size   Size of found_path buffer.
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_file(RestoreContext* ctx,
                                  const FileSpec* spec,
                                  char* found_path, size_t path_size);

/**
 * @brief Simple file restore with inline parameters.
 *
 * WHY: Convenience for common case without FileSpec struct.
 *
 * @param[in,out] ctx            Restore context.
 * @param[in]     name           File name for logging.
 * @param[in]     expected_path  Expected file path.
 * @param[out]    found_path     Buffer for found path.
 * @param[in]     path_size      Size of found_path buffer.
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_file_simple(RestoreContext* ctx,
                                         const char* name, const char* expected_path,
                                         char* found_path, size_t path_size);

/**
 * @brief Locate a file by searching configured paths.
 *
 * WHY: "Seek, and ye shall find." — Matthew 7:7
 *
 * @param[in,out] ctx         Restore context.
 * @param[in]     filename    Filename to locate.
 * @param[out]    found_path  Buffer for found path.
 * @param[in]     path_size   Size of found_path buffer.
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_locate_file(RestoreContext* ctx,
                                         const char* filename,
                                         char* found_path, size_t path_size);

/**
 * @brief Relocate a file from source to destination.
 *
 * WHY: Files may need to be moved to correct location.
 *
 * @param[in,out] ctx     Restore context.
 * @param[in]     source  Source file path.
 * @param[in]     dest    Destination file path.
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_relocate_file(RestoreContext* ctx,
                                           const char* source, const char* dest);

/** @} */ /* end S.4e FILES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4f DIRS — Directory Recovery [DIRS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_dirs S.4f DIRS — Directory Recovery
 * @ingroup cpisi_restore_prototypes
 * @brief    Create missing directories.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Restore a directory using specification.
 *
 * WHY: Directories must exist for file operations to work.
 *
 * @param[in,out] ctx   Restore context.
 * @param[in]     spec  Directory specification.
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_dir(RestoreContext* ctx, const DirSpec* spec);

/**
 * @brief Simple directory restore with inline parameters.
 *
 * WHY: Convenience for common case without DirSpec struct.
 *
 * @param[in,out] ctx   Restore context.
 * @param[in]     path  Directory path.
 * @param[in]     mode  Directory permissions (e.g., 0755).
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_dir_simple(RestoreContext* ctx, const char* path, uint32_t mode);

/**
 * @brief Create directory and all parent directories.
 *
 * WHY: "Prepare thy work without" — Proverbs 24:27
 *      Parents must exist before children.
 *
 * @param[in,out] ctx   Restore context.
 * @param[in]     path  Full directory path.
 * @param[in]     mode  Directory permissions (e.g., 0755).
 *
 * @return RestoreResult indicating success or failure mode.
 */
RestoreResult cpisi_restore_mkdir_parents(RestoreContext* ctx, const char* path, uint32_t mode);

/** @} */ /* end S.4f DIRS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4g PLAN — Recovery Planning [PLAN]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_plan S.4g PLAN — Recovery Planning
 * @ingroup cpisi_restore_prototypes
 * @brief    Create and execute structured recovery plans.
 *
 * WHY: CPI-SI restoration is structured, not ad-hoc. Plans provide
 *      order, milestones, and verification.
 *
 * BIBLICAL: "Order my steps in thy word" — Psalm 119:133
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Create a recovery plan targeting a specific health state.
 *
 * @param[in,out] ctx          Restore context.
 * @param[in]     target_state Target Hebrew state to achieve.
 * @param[in]     report       Assessment report informing the plan (or NULL).
 *
 * @return true if plan created, false if already at or above target.
 */
bool cpisi_restore_create_plan(RestoreContext* ctx,
                                HebrewState target_state,
                                const DiagReport* report);

/**
 * @brief Execute one step of the active recovery plan.
 *
 * @param[in,out] ctx  Restore context with active plan.
 *
 * @return RestoreResult of the step.
 */
RestoreResult cpisi_restore_execute_step(RestoreContext* ctx);

/**
 * @brief Check if a milestone was reached.
 *
 * @param[in] ctx  Restore context.
 *
 * @return Milestone percentage (25, 50, 75, 100) or 0 if no milestone.
 */
uint8_t cpisi_restore_check_milestone(const RestoreContext* ctx);

/**
 * @brief Verify that restoration achieved the target.
 *
 * @param[in] ctx  Restore context.
 *
 * @return true if target achieved, false otherwise.
 */
bool cpisi_restore_verify(const RestoreContext* ctx);

/** @} */ /* end S.4g PLAN */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4h PATTERN — Pattern-Guided Recovery [PATTERN]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_pattern S.4h PATTERN — Pattern-Guided Recovery
 * @ingroup cpisi_restore_prototypes
 * @brief    Apply learned patterns from cross-session memory.
 *
 * WHY: CPI-SI learns from experience — patterns that worked before
 *      inform current recovery strategies.
 *
 * BIBLICAL: "Remember the former things of old" — Isaiah 46:9
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Find patterns matching the current situation.
 *
 * @param[in]  ctx          Restore context.
 * @param[in]  detect_type  What was detected.
 * @param[in]  severity     Current severity.
 * @param[out] matches      Array to receive matching pattern IDs.
 * @param[in]  max_matches  Size of matches array.
 *
 * @return Number of matching patterns found.
 */
uint32_t cpisi_restore_find_patterns(const RestoreContext* ctx,
                                      DetectType detect_type,
                                      Severity severity,
                                      uint32_t* matches,
                                      size_t max_matches);

/**
 * @brief Apply a learned pattern to current recovery.
 *
 * @param[in,out] ctx        Restore context.
 * @param[in]     pattern_id Pattern ID to apply.
 *
 * @return RestoreResult from applying the pattern.
 */
RestoreResult cpisi_restore_apply_pattern(RestoreContext* ctx, uint32_t pattern_id);

/**
 * @brief Record successful recovery for future pattern learning.
 *
 * @param[in,out] ctx     Restore context.
 * @param[in]     action  The successful restore action.
 */
void cpisi_restore_record_success(RestoreContext* ctx, const RestoreAction* action);

/** @} */ /* end S.4h PATTERN */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4i REPORTING — Stats and History [REPORTING]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_reporting S.4i REPORTING — Stats and History
 * @ingroup cpisi_restore_prototypes
 * @brief    Query recovery statistics and history.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Dump recovery history to debug output.
 *
 * WHY: Debugging and audit trail.
 *
 * @param[in] ctx  Restore context.
 */
void cpisi_restore_dump_history(const RestoreContext* ctx);

/**
 * @brief Dump recovery statistics to debug output.
 *
 * WHY: Performance monitoring and diagnostics.
 *
 * @param[in] ctx  Restore context.
 */
void cpisi_restore_dump_stats(const RestoreContext* ctx);

/**
 * @brief Dump active recovery plan to debug output.
 *
 * WHY: Plan visualization and debugging.
 *
 * @param[in] ctx  Restore context.
 */
void cpisi_restore_dump_plan(const RestoreContext* ctx);

/**
 * @brief Get human-readable name for a RestoreResult.
 *
 * WHY: Human-readable error reporting.
 *
 * @param[in] result  Restore result value.
 *
 * @return Static string (do not free).
 */
const char* cpisi_restore_result_name(RestoreResult result);

/**
 * @brief Get the last error message.
 *
 * WHY: Detailed error reporting for diagnostics.
 *
 * @param[in] ctx  Restore context.
 *
 * @return Last error message or empty string.
 */
const char* cpisi_restore_last_error(const RestoreContext* ctx);

/** @} */ /* end S.4i REPORTING */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File Scope Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_file S.5 FILE-LEVEL — File Scope Declarations
 * @brief    File-level state and type declarations.
 *
 * CONTAINS:
 *   - S.5a VARIABLES      — Global context for convenience macros
 *   - S.5b FUNC_TYPES     — [Reserved: defined in S.3b STRUCTS]
 *   - S.5c INFRASTRUCTURE — [Reserved: lives in source files]
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5a VARIABLES — Extern/Static Variables [VARIABLES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_restore_vars S.5a VARIABLES — Extern/Static Variables
 * @ingroup cpisi_restore_file
 * @brief    File-scope variable declarations for global restore access.
 *
 * WHY: Convenience macros need a global context. Complex code passes explicit
 *      RestoreContext pointers. This global enables simple single-context usage.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Global restore context for convenience macros.
 *
 * WHY: Simple use cases can use a global context. More complex code
 *      should use explicit context passing.
 *
 * @note Set via cpisi_restore_init() or cpisi_restore_set_global().
 * @note Thread safety: Not thread-safe. Use explicit context in threaded code.
 */
extern RestoreContext* g_cpisi_restore;

/** @} */ /* end S.5a VARIABLES */

/**
 * S.5b FUNC_TYPES — Callback Typedefs [FUNC_TYPES]
 * @defgroup cpisi_restore_func_types S.5b FUNC_TYPES
 * WHY RESERVED: Callback types defined in S.3b STRUCTS — RestoreContext
 *               embeds RestoreCallback, PatternCallback, LogCallback.
 * @{ */
/* [Reserved: S.5b FUNC_TYPES — callback types in S.3b RestoreContext] */
/** @} */

/**
 * S.5c INFRASTRUCTURE — Logger/Inspector [INFRASTRUCTURE]
 * @defgroup cpisi_restore_infra S.5c INFRASTRUCTURE
 * WHY RESERVED: Infrastructure (loggers, inspectors) lives in restore.c,
 *               not exposed in header. Only public API is declared here.
 * @{ */
/* [Reserved: S.5c INFRASTRUCTURE — static in source] */
/** @} */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_omission S.6 OMISSION — Reserved Section Guide
 * @brief    Documents what sections are reserved and why.
 *
 * PURPOSE: Explicit documentation of intentionally empty sections.
 *          All sections must be present. Empty sections use: [Reserved: reason]
 *
 * RESERVED SECTIONS IN SETUP:
 *   S.5b FUNC_TYPES     — Callback types defined in S.3b STRUCTS (RestoreContext)
 *   S.5c INFRASTRUCTURE — Static implementation in restore.c, not exposed
 *
 * RESERVED SECTIONS IN BODY:
 *   B.1 ORG CHART       — Architecture documented in METADATA M.7 INTENT
 *   B.2 HELPERS         — Static helpers live in restore.c
 *   B.3 CORE OPS        — Core operations implemented in restore.c
 *   B.4 ERRORS          — Error handling in restore.c
 *   B.5 PUBLIC API      — Implementations in restore.c
 *
 * WHY ALL RESERVED: This is a header file. Declarations in SETUP, implementations
 *                   in source files. BODY provides structure for restore.c.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: S.6 is documentation-only — no code content] */

/** @} */ /* end S.6 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief The actual work — declarations (headers), implementations (sources).
///
/// WHY RESERVED: Headers declare interface in SETUP block. BODY sections hold
///               implementations which live in restore.c (source file).
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 * @defgroup cpisi_restore_orgchart B.1 ORG CHART
 * @{ */

/* Types:      8 (2 enums, 6 structs)    | Functions: 39 total
 * Externs:    1 (g_cpisi_restore)       | Macros:    10 (S.2d)
 * Implementation: restore.c (to be created)
 */

/** @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Foundation Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 * @defgroup cpisi_restore_helpers B.2 HELPERS
 * WHY RESERVED: Static helpers live in restore.c, not exposed in header.
 * @{ */
/* [Reserved: B.2a PURE, B.2b UTILITIES — static in source] */
/** @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Component-Specific Functionality [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 * @defgroup cpisi_restore_core_ops B.3 CORE OPS
 * WHY RESERVED: Core recovery logic implemented in restore.c.
 * @{ */
/* [Reserved: Core operations in source file] */
/** @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Error Handling [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 * @defgroup cpisi_restore_errors B.4 ERRORS
 * WHY RESERVED: Error handling patterns implemented in restore.c.
 * @{ */
/* [Reserved: B.4a QUERIES, B.4b RECOVERY — in source] */
/** @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Exported Interface [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 * @defgroup cpisi_restore_api B.5 PUBLIC API
 * WHY RESERVED: API implementations in restore.c. Prototypes in S.4.
 * @{ */
/* [Reserved: B.5a LIFECYCLE, B.5b OPERATIONS, B.5c HEALTH, B.5d UTILITIES] */
/** @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Validation, policy, extension, and closing notes.
///
/// CONTAINS:
///   - X.1 VALIDATION     — Compile-time checks
///   - X.2 EXECUTION      — Execution patterns
///   - X.3 CLEANUP        — Ownership and memory
///   - X.4 POLICY         — Modification guidelines
///   - X.5 EXTENSION      — How to expand
///   - X.6 TROUBLESHOOTING — Common problems
///   - X.7 REFERENCE      — Quick lookup
///   - X.8 NOTE           — Closing verse
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Compile-Time Checks [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_validation X.1 VALIDATION — Compile-Time Checks
 * @ingroup cpisi_restore
 * @brief    Static assertions and compile-time validation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* Verify limit consistency */
CPISI_STATIC_ASSERT(CPISI_RESTORE_ENGRAM_MAX == CPISI_PATTERN_MEMORY_SIZE,
    "Engram size must match pattern memory size");

/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Execution Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_execution X.2 EXECUTION — Execution Patterns
 * @ingroup cpisi_restore
 * @brief    How to use this component.
 *
 * TYPICAL FLOW:
 * @code{.c}
 * RestoreContext ctx;
 * cpisi_restore_init(&ctx, RESTORE_STRATEGY_FULL_CPI);
 *
 * // Link to ASSESS phase data
 * cpisi_restore_link_patterns(&ctx, &assess_patterns);
 * cpisi_restore_link_session(&ctx, &session_diag);
 *
 * // Create plan from assessment
 * if (cpisi_restore_create_plan(&ctx, HEBREW_YASHAR, &diag_report)) {
 *     while (ctx.active_plan.active) {
 *         cpisi_restore_execute_step(&ctx);
 *         uint8_t milestone = cpisi_restore_check_milestone(&ctx);
 *         if (milestone > 0) {
 *             printf("Milestone: %d%%\n", milestone);
 *         }
 *     }
 * }
 *
 * if (cpisi_restore_verify(&ctx)) {
 *     printf("Restoration successful!\n");
 * }
 *
 * cpisi_restore_shutdown(&ctx);
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Execution patterns documented in defgroup above] */

/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership and Memory [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_cleanup X.3 CLEANUP — Ownership and Memory
 * @ingroup cpisi_restore
 * @brief    Memory ownership and cleanup patterns.
 *
 * OWNERSHIP:
 *   - cpisi_restore_init(): Initializes caller-owned memory
 *   - cpisi_restore_shutdown(): Cleans up initialized context
 *   - PatternMemory/CrossSessionDiag: Borrowed pointers, caller owns
 *   - const char* fields: Borrowed pointers, caller owns
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Cleanup patterns documented in defgroup above] */

/** @} */ /* end X.3 CLEANUP */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_policy X.4 POLICY — Modification Guidelines
 * @ingroup cpisi_restore
 * @brief    What's safe to modify.
 *
 * SAFE TO MODIFY:
 *   - ✅ Add new recovery functions
 *   - ✅ Add new convenience macros
 *   - ✅ Extend RestoreContext fields
 *
 * MODIFY WITH CARE:
 *   - ⚠️ RestoreResult values — affects all consumers
 *   - ⚠️ Strategy flags — affects recovery behavior
 *
 * NEVER MODIFY:
 *   - ❌ 4-block structure
 *   - ❌ Include guard pattern
 *   - ❌ Result value meanings (from types.h)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Policy documented in defgroup above] */

/** @} */ /* end X.4 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_extension X.5 EXTENSION — How to Expand
 * @ingroup cpisi_restore
 * @brief    How to extend this component.
 *
 * ADDING RECOVERY FUNCTIONS:
 *   1. Add prototype to appropriate S.4 subsection
 *   2. Implement in restore.c
 *   3. Add convenience macro if appropriate (S.6)
 *   4. Update B.1 ORG CHART counts
 *
 * ADDING PATTERN TYPES:
 *   1. Define pattern category in assess.h
 *   2. Add pattern matching in cpisi_restore_find_patterns()
 *   3. Add pattern application in cpisi_restore_apply_pattern()
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Extension patterns documented in defgroup above] */

/** @} */ /* end X.5 EXTENSION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @ingroup cpisi_restore
 * @brief    Common problems and solutions.
 *
 * "Pattern memory not working":
 *   → Call cpisi_restore_link_patterns() before using pattern functions
 *
 * "Recovery plan not creating":
 *   → Verify current health is below target
 *   → Check that context is initialized
 *
 * "Milestones not triggering":
 *   → Set on_milestone callback via cpisi_restore_set_callbacks()
 *
 * "g_cpisi_restore is NULL":
 *   → Set g_cpisi_restore = &ctx after cpisi_restore_init()
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Troubleshooting documented in defgroup above] */

/** @} */ /* end X.6 TROUBLESHOOTING */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_reference X.7 REFERENCE — Quick Lookup
 * @ingroup cpisi_restore
 * @brief    Copy-paste ready examples.
 *
 * BASIC SETUP:
 * @code{.c}
 * #include "kernel/dar/phase/restore.phase.h"
 * RestoreContext ctx;
 * cpisi_restore_init(&ctx, RESTORE_STRATEGY_FULL);
 * g_cpisi_restore = &ctx;
 * @endcode
 *
 * CPI-SI SETUP:
 * @code{.c}
 * cpisi_restore_init(&ctx, RESTORE_STRATEGY_FULL_CPI);
 * cpisi_restore_link_patterns(&ctx, &patterns);
 * cpisi_restore_link_session(&ctx, &session);
 * @endcode
 *
 * RELATED:
 *   - detect.h — DETECT phase (witnessing, logging)
 *   - assess.h — ASSESS phase (evaluation, patterns)
 *   - dar.h — Group header for all DAR phases
 *   - types.h — Single source of truth for CPI-SI types
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reference documented in defgroup above] */

/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Verse [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_note X.8 NOTE — Closing Verse
 * @ingroup cpisi_restore
 * @brief    Biblical grounding for this component.
 *
 * "For a just man falleth seven times, and riseth up again:
 *  but the wicked shall fall into mischief."
 * — Proverbs 24:16 KJV
 *
 * Recovery is not failure. The just man falls—and rises again.
 * This system embodies that principle: try, fail, recover, continue.
 * Only stop when you truly must.
 *
 * "He restoreth my soul: he leadeth me in the paths of righteousness
 *  for his name's sake."
 * — Psalm 23:3 KJV
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Closing verse documented in defgroup above] */

/** @} */ /* end X.8 NOTE */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_CORE_CPISI_DAR_RESTORE_H */
