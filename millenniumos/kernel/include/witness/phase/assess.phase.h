/// @file assess.h
/// @omni code --c --library
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// OMNICODE PRAGMA [PRAGMA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @legend
///   :req  REQUIRED   — must exist, validation fails without
///   :inh  INHERITED  — from template, override allowed
///   :ins  INSTANCE   — file-specific, unique values
/// @endlegend
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.1 CORE — Identity [CORE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:req  key        = CORNERSTONE-CORE-CPISI-ASSESS
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = b-01.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = dar-phase
/// @omni:inh  role       = assessment
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi/dar/phases
/// @omni:ins  layer      = HEALTH (4)
/// @omni:ins  includes   = types.h, error.h
/// @omni:ins  provides   = [DAR_ASSESS_PHASE, DiagReport, DebugContext]
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.4 ARCHITECTURE — Scaling Context [ARCH]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  layers     = [0:kernel, 1:system, 2:runtime, 3:framework,
///                          4:health, 5:network, 6:storage, 7:engine, 8:app]
/// @omni:ins  scale      = 3^n [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
/// @omni:ins  pattern    = 4+1+4 [below:0-3, center:4, above:5-8]
/// @omni:ins  anchor     = genesis_1_1 -> void(0) -> unity(1) -> derivation
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.5 SUMMARY — Human Identity [SUMMARY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:req  title      = DAR ASSESS Phase — Diagnostic Assessment
/// @omni:req  brief      = Evaluate detected issues, diagnose patterns, determine severity
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// END PRAGMA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SPECIAL BLOCK [SPECIAL]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Pre-include configuration — MUST appear before any system includes.
///
/// WHY SPECIAL EXISTS:
///   Feature macros and compile-time settings must be defined BEFORE \#include.
///   This is not PRAGMA (metadata), not METADATA (docs), not SETUP (includes).
///
/// CONTAINS:
///   - SP.1 CONFIG  — Compile-time settings (POSIX, DEBUG)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * WHY: Settings that affect what headers provide must precede \#include.
 *
 * CONTAINS:
 *   - SP.1a POSIX  — Platform feature test (universal)
 *   - SP.1b DEBUG  — Debug feature flags
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1a POSIX — Feature Test [POSIX]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_assess_special_config
 * @brief    Enables POSIX.1-2008 functions in standard headers.
 *
 * WHY: Time functions, file operations, and POSIX APIs require this macro.
 *      Must be defined BEFORE any system includes or they won't see it.
 *
 * SCOPE: Universal — most C files need POSIX features.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L  /**< POSIX.1-2008 — enables strdup, nanosleep, etc. */
#endif

/** @} */ /* end SP.1a POSIX */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1b DEBUG — Debug Feature Flags [DEBUG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_special_debug SP.1b DEBUG — Debug Feature Flags
 * @ingroup cpisi_assess_special_config
 * @brief    Master debug switch and feature flags.
 *
 * WHY: Debug features need compile-time configuration. Master switch enables
 *      all debug features; individual flags allow fine-grained control.
 *      Set via compile flag: -DCPISI_DEBUG_ENABLED=1
 *
 * SCOPE: ASSESS phase — debug behavior is triggered here, separate from logging.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Master debug switch — enables all debug features.
 *
 * WHY: Single flag to control all debug behavior. Off by default in release
 *      builds for performance. Enable with -DCPISI_DEBUG_ENABLED=1.
 */
#ifndef CPISI_DEBUG_ENABLED
#define CPISI_DEBUG_ENABLED 0
#endif

/**
 * @brief Individual feature flags — all require CPISI_DEBUG_ENABLED.
 *
 * WHY: Fine-grained control over debug features. Each feature has different
 *      performance implications. Allows enabling only what's needed.
 */
#ifndef CPISI_DEBUG_ASSERTIONS
#define CPISI_DEBUG_ASSERTIONS CPISI_DEBUG_ENABLED  /**< Catch logic bugs early */
#endif

#ifndef CPISI_DEBUG_BOUNDS_CHECK
#define CPISI_DEBUG_BOUNDS_CHECK CPISI_DEBUG_ENABLED  /**< Catch buffer overflows */
#endif

#ifndef CPISI_DEBUG_MEMORY
#define CPISI_DEBUG_MEMORY CPISI_DEBUG_ENABLED  /**< Catch memory leaks */
#endif

#ifndef CPISI_DEBUG_TRACE_CALLS
#define CPISI_DEBUG_TRACE_CALLS 0  /**< Very verbose — off even in debug */
#endif

/** @} */ /* end SP.1b DEBUG */

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef CORNERSTONE_CORE_CPISI_DAR_ASSESS_H
/** @def CORNERSTONE_CORE_CPISI_DAR_ASSESS_H
 *  @brief Include guard — prevents multiple inclusion of this DAR ASSESS header.
 */
#define CORNERSTONE_CORE_CPISI_DAR_ASSESS_H

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Identity and context for this component.
///
/// STRUCTURE: M.1-M.10 sections for complete component documentation.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      assess.h
 * @brief     DAR ASSESS Phase — Diagnostic assessment for the DAR Witness System.
 *
 * @defgroup cpisi_assess_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CORNERSTONE-CORE-CPISI-ASSESS
 * Title:     CPI-SI Diagnostic Assessment (DAR ASSESS)
 * Type:      Header (DAR Phase)
 * Component: Rails — provides ASSESS phase for DAR witness system
 * Role:      Evaluate detected issues, diagnose patterns, determine severity
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   b-01.00
 * @date      2026-01-25
 *
 * Status:    Active
 * Created:   2025-12-01
 * Updated:   2026-01-25
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authors and Copyright [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_attribution M.3 ATTRIBUTION — Authors and Copyright
 * @{
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Implementation)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Path [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/dar/phases/assess.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: bereshit/word/seed/code/c/header.h (4-block template)
 * Extracted:    2025-12-01 from original debug.h
 *
 * Builds FROM:  types.h (foundation types), error.h (error codes)
 * Builds TO:    dar.h (DAR group header), cpisi.h (main header)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: dar, assess, debug, diagnostic, health, witness
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Evaluate detected issues, diagnose patterns, determine severity
 *
 * Core Design: ASSESS is the second phase of DAR (Detect → ASSESS → Restore)
 *              - Receives detection data from DETECT phase
 *              - Evaluates severity using cognitive state context
 *              - Diagnoses root causes and patterns
 *              - Recommends whether RESTORE phase should be triggered
 *
 * Philosophy:  "Examine yourselves, whether ye be in the faith" — 2 Cor 13:5
 *              Self-examination reveals what is hidden for understanding.
 *              ASSESS doesn't just record (that's DETECT), it evaluates.
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  "Examine yourselves, whether ye be in the faith; prove your own selves."
 *             — 2 Corinthians 13:5 KJV
 *
 * Principle:  Assessment reveals what is hidden for understanding.
 *             DETECT witnesses, ASSESS evaluates, RESTORE heals.
 *
 * Anchor:     "If thou criest after knowledge, and liftest up thy voice for
 *             understanding; If thou seekest her as silver, and searchest for her
 *             as for hid treasures; Then shalt thou understand the fear of the
 *             LORD, and find the knowledge of God." — Proverbs 2:3-5 KJV
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h — Foundation types (HebrewState, KFactor, HealthScore, etc.)
 *   Internal: error.h — Error codes (CPISI_ERR_DIAG_* for ASSESS layer)
 *   Standard: stdarg.h — Variadic function support
 *   Standard: stdio.h — FILE* for output streams
 *
 * What Uses This:
 *   - dar.h — Group header includes all DAR phases
 *   - cpisi.h — Main CPI-SI header
 *   - Application code using diagnostic assessment
 *
 * DAR Phase Flow:
 *   detect.h (DETECT) → assess.h (ASSESS) → restore.h (RESTORE)
 *
 * @see types.h for HebrewState, KFactor, HealthScore, MultiScore, TernaryScore
 * @see error.h for CPISI_ERR_DIAG_* error codes (117-125)
 * @see detect.h for DetectType and detection context
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * History
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   a-01.00 (2025-12-01) — Initial debug.h with basic assertions
 *   a-02.00 (2025-12-15) — Renamed to assess.h, DAR integration
 *   b-01.00 (2026-01-25) — Full header.h template alignment, error.h integration
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Current
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   ✓ Full 4-block structure (METADATA, SETUP, BODY, CLOSING)
 *   ✓ M.1-M.10 METADATA sections
 *   ✓ S.1-S.6 SETUP sections with WHY docstrings
 *   ✓ B.1-B.6 BODY sections
 *   ✓ X.1-X.10 CLOSING sections
 *   ✓ error.h integration (CPISI_ERR_DIAG_* codes)
 *   ✓ Debug macros (assertions, bounds checking, memory tracking, call tracing)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Planned
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   ⏳ Pattern recognition for recurring issues
 *   ⏳ Cross-session diagnostic history
 *   ⏳ Machine learning severity prediction
 *
 * @} end M.10 ROADMAP
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
///   - S.1 INCLUDES  — Header dependencies (Standard, Internal, External, CPP)
///   - S.2 DEFINES   — Constants and macros
///   - S.3 TYPES     — Type declarations (enums, structs)
///   - S.4 PROTOS    — Function prototypes
///   - S.5 EXTERN    — External declarations
///   - S.6 OMISSION  — Reserved section guide
///
/// Section order: Includes → Defines → Types → Prototypes → Extern → Omission
/// Flow: dependencies → constants → data model → interface → shared state
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
 * @defgroup cpisi_assess_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * PURPOSE: Establish all header dependencies in consistent order.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers (inherited from types.h)
 *   - S.1b INTERNAL  — Project headers (types.h, error.h)
 *   - S.1c EXTERNAL  — Third-party headers (none)
 *   - S.1d CPP       — C++ linkage wrapper
 *
 * Section order: Standard → Internal → External → CPP linkage
 * Flow: system foundation → our code → third-party → C++ compatibility
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_assess_includes
 * @brief    C standard library headers required by this module.
 *
 * WHY: types.h provides most system headers. We only need stdarg.h for
 *      variadic functions and stdio.h for FILE* output streams.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Foundation types — MUST BE FIRST in kernel-like ladder.
 *
 * WHY: kernel.h sets CPISI_MODE which controls what headers are available.
 *      MUST come before any CPISI_MODE guards or they won't work.
 */
#include "kernel.h"

/**
 * @brief Variadic functions (freestanding-compatible).
 *
 * NOTE: stdarg.h is always available in freestanding mode.
 */
#include <stdarg.h>

#if CPISI_MODE >= 2  /* Only in hosted mode */
#include <stdio.h>   /**< Standard I/O: FILE*, fprintf (for debug output) */
#endif

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_assess_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * WHY: kernel.h (included above) provides all foundation types.
 *      In freestanding mode, kernel.h provides stubs.
 *      In hosted mode, error.type.h, log.cfg.h, health.type.h are available.
 *
 * TYPES FROM kernel.h:
 *   - HebrewState, KFactor, HealthScore, MultiScore, TernaryScore
 *   - DetectType, Severity, HealthLevel, HealthThreshold
 *   - CPISI_PATH_MAX, CPISI_MSG_MAX, CPISI_SEVERITY_COUNT, etc.
 *
 * ERROR CODES:
 *   - CPISI_OK_DIAG (117) — Diagnostic success
 *   - CPISI_ERR_DIAG_NULL (118) — Null context
 *   - CPISI_ERR_DIAG_INVALID (119) — Invalid parameter
 *   - CPISI_ERR_DIAG_BOUNDS (120) — Out of bounds
 *   - CPISI_ERR_DIAG_STATE (122) — Invalid state
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* kernel.h already included at top of S.1 INCLUDES */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_assess_includes
 * @brief    External library headers with justification.
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
 * @defgroup cpisi_assess_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_assess_includes
 * @brief    C++ compatibility wrapper.
 *
 * WHY: Allows C++ code to include and use this C header without name mangling.
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
 * @defgroup cpisi_assess_defines S.2 DEFINES — Constants and Macros
 * @brief    Compile-time constants, limits, and error code aliases.
 *
 * PURPOSE: Define domain-specific constants for ASSESS phase operations.
 *
 * CONTAINS:
 *   - S.2a LEVELS  — Debug level constants (reference to types.h)
 *   - S.2b ERRORS  — Error code aliases for ASSESS operations
 *
 * WHY SEPARATE: Levels are for configuration; errors are for return values.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a LEVELS — Debug Level Reference [LEVELS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_levels S.2a LEVELS — Debug Level Reference
 * @ingroup cpisi_assess_defines
 * @brief    Debug levels from types.h for reference.
 *
 * WHY: Debug levels control how much assessment information is captured.
 *      Defined in types.h for single source of truth.
 *
 * ┌───────┬─────────────────────────────────────────────────────────────────────┐
 * │ Level │ Description                                                         │
 * ├───────┼─────────────────────────────────────────────────────────────────────┤
 * │   0   │ OFF     — No debug features (release mode)                          │
 * │   1   │ BASIC   — Assertions only                                           │
 * │   2   │ VERBOSE — Assertions + bounds checking                              │
 * │   3   │ FULL    — All features including memory tracking                    │
 * │   4   │ TRACE   — Call tracing (very verbose)                               │
 * └───────┴─────────────────────────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Debug levels from log.h — 9 levels mapping to layer pyramid:
 *   CPISI_DEBUG_LEVEL_NONE       = 0  (L0: KERNEL — silent)
 *   CPISI_DEBUG_LEVEL_MINIMAL    = 1  (L1: SYSTEM)
 *   CPISI_DEBUG_LEVEL_BASIC      = 2  (L2: RUNTIME)
 *   CPISI_DEBUG_LEVEL_STANDARD   = 3  (L3: FRAMEWORK)
 *   CPISI_DEBUG_LEVEL_VERBOSE    = 4  (L4: HEALTH — CENTER)
 *   CPISI_DEBUG_LEVEL_DETAILED   = 5  (L5: NETWORK)
 *   CPISI_DEBUG_LEVEL_FULL       = 6  (L6: STORAGE)
 *   CPISI_DEBUG_LEVEL_TRACE      = 7  (L7: ENGINE)
 *   CPISI_DEBUG_LEVEL_EXHAUSTIVE = 8  (L8: APP — maximum)
 */

/**
 * @brief Hotspot tracking array size.
 *
 * WHY: Limits memory for tracking frequently-assessed locations.
 *      16 slots allow identifying top hotspots without excessive memory.
 */
#define CPISI_HOTSPOT_COUNT         16  /**< Max tracked hotspots */

/**
 * @brief Pattern memory size for cross-session learning.
 *
 * WHY: CPI-SI learns patterns covenant-style — grounded, traceable, accountable.
 *      27 = 3³ (ternary cube) patterns can be remembered across sessions.
 */
#define CPISI_PATTERN_MEMORY_SIZE   27  /**< Ternary cube of patterns */

/**
 * @brief Maximum prediction horizon (sessions).
 *
 * WHY: CPI-SI predictions are bounded — we don't claim godlike knowledge.
 *      3 sessions ahead is the limit of honest prediction.
 */
#define CPISI_PREDICTION_HORIZON    3   /**< Max sessions to predict */

/** @} */ /* end S.2a LEVELS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b ERRORS — Error Code Aliases [ERRORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_errors S.2b ERRORS — Error Code Aliases
 * @ingroup cpisi_assess_defines
 * @brief    ASSESS-specific error code aliases from error.h.
 *
 * WHY: Provides domain-friendly names while using canonical error codes.
 *      ASSESS uses HEALTH layer, DIAG tier (codes 117-125).
 *
 * ARCHITECTURE: 9 layers × 3 tiers × 9 positions = 243 + 13 metadata = 256
 *   HEALTH layer (4) base = 108
 *   DIAG tier (EXT) offset = 9
 *   DIAG codes = 117-125
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief ASSESS success — diagnostic completed successfully (code 117). */
#define ASSESS_OK                   CPISI_OK_DIAG

/** @brief ASSESS context is NULL (code 118). */
#define ASSESS_ERR_NULL             CPISI_ERR_DIAG_NULL

/** @brief Invalid ASSESS parameter (code 119). */
#define ASSESS_ERR_INVALID          CPISI_ERR_DIAG_INVALID

/** @brief ASSESS metric out of bounds (code 120). */
#define ASSESS_ERR_BOUNDS           CPISI_ERR_DIAG_BOUNDS

/** @brief ASSESS type mismatch (code 121). */
#define ASSESS_ERR_TYPE             CPISI_ERR_DIAG_TYPE

/** @brief Invalid ASSESS state (code 122). */
#define ASSESS_ERR_STATE            CPISI_ERR_DIAG_STATE

/** @brief ASSESS not started (code 123). */
#define ASSESS_ERR_NOT_STARTED      CPISI_ERR_DIAG_NOT_STARTED

/** @brief ASSESS already complete (code 124). */
#define ASSESS_ERR_COMPLETE         CPISI_ERR_DIAG_COMPLETE

/** @brief Unknown ASSESS error (code 125). */
#define ASSESS_ERR_UNKNOWN          CPISI_ERR_DIAG_UNKNOWN

/** @} */ /* end S.2b ERRORS */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_types S.3 TYPES — Type Declarations
 * @brief    Enumerations and structures for ASSESS phase.
 *
 * PURPOSE: Define domain-specific types for diagnostic assessment.
 *
 * CONTAINS:
 *   - S.3a ENUMS   — DebugLevel enumeration
 *   - S.3b STRUCTS — DiagReport, DebugContext structures
 *   - S.3c ALIAS   — AssessContext alias for DAR naming
 *   - S.3d DEFAULTS — Default initializers
 *
 * TYPE HIERARCHY:
 *   DebugLevel — controls assessment depth
 *   DiagReport — result of a single assessment
 *   DebugContext — persistent state for assessment system
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — Debug Level Enumeration [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_types_enums S.3a ENUMS — Debug Level Enumeration
 * @ingroup cpisi_assess_types
 * @brief    Type-safe enumeration for debug levels.
 *
 * WHY: Enum provides type safety in function signatures. Values match
 *      CPISI_DEBUG_LEVEL_* constants from types.h for consistency.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief DebugLevel — controls depth of diagnostic assessment.
 *
 * WHY: Different contexts need different assessment depth. Kernel code
 *      needs minimal overhead; application code can afford full tracking.
 *
 * Values match CPISI_DEBUG_LEVEL_* for array indexing and consistency.
 */
typedef enum DebugLevel {
    DEBUG_LEVEL_OFF     = CPISI_DEBUG_LEVEL_NONE,    /**< No debug features (release mode) */
    DEBUG_LEVEL_BASIC   = CPISI_DEBUG_LEVEL_BASIC,   /**< Assertions only */
    DEBUG_LEVEL_VERBOSE = CPISI_DEBUG_LEVEL_VERBOSE, /**< Assertions + bounds checking */
    DEBUG_LEVEL_FULL    = CPISI_DEBUG_LEVEL_FULL,    /**< All features including memory */
    DEBUG_LEVEL_TRACE   = CPISI_DEBUG_LEVEL_TRACE,   /**< Call tracing (very verbose) */
} DebugLevel;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — Structure Definitions [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_types_structs S.3b STRUCTS — Structure Definitions
 * @ingroup cpisi_assess_types
 * @brief    Core structures for diagnostic assessment.
 *
 * WHY: DiagReport captures single assessment results. DebugContext maintains
 *      persistent state across assessments including cognitive context.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief DiagReport — Result of a diagnostic assessment.
 *
 * WHY: Captures full context of an assessment including cognitive state,
 *      severity determination, and recommendation for RESTORE phase.
 *
 * FIELDS:
 *   - Detection context: What triggered this assessment
 *   - Assessment results: Severity and state evaluation
 *   - Scoring: Multi-scale health scores at assessment time
 *   - Recommendation: Whether RESTORE phase should be triggered
 */
typedef struct DiagReport {
    /* ── Timing ───────────────────────────────────────────────────────────── */
    uint64_t        timestamp_ms;       /**< When assessment occurred (ms since epoch) */

    /* ── Detection Context (from DETECT phase) ────────────────────────────── */
    DetectType      detect_type;        /**< What triggered detection */
    const char*     source;             /**< Where detection occurred (file:line) */
    const char*     message;            /**< What was detected */

    /* ── Assessment Results ───────────────────────────────────────────────── */
    Severity        severity;           /**< Assessed severity (none → critical) */
    HebrewState     assessed_state;     /**< Cognitive state at assessment */
    HealthLevel     health_level;       /**< 7-level health assessment */

    /* ── Multi-Scale Scoring ──────────────────────────────────────────────── */
    HealthScore     health;             /**< Base100: -100 to +100 */
    MultiScore      score;              /**< All decimal bases */
    TernaryScore    ternary;            /**< All ternary bases */

    /* ── Direction and Recommendation ─────────────────────────────────────── */
    KFactor         k_direction;        /**< Recommended direction (+1/0/-1) */
    bool            needs_restore;      /**< Should RESTORE phase be triggered? */
    const char*     recommendation;     /**< Human-readable recommendation */
} DiagReport;

/**
 * @brief DebugContext — Persistent state for diagnostic assessment system.
 *
 * WHY: Maintains cognitive context across assessments. Tracks patterns,
 *      hotspots, and statistics. Traditional name for SI-friendly usage.
 *
 * SECTIONS:
 *   - Configuration: Debug level and feature flags
 *   - Cognitive State: Hebrew state, k-factor, cube position
 *   - Multi-Scale Scoring: Health scores at multiple bases
 *   - Memory Tracking: Allocation/free statistics
 *   - Call Tracing: Function call stack
 *   - Assessment Statistics: Counts and hotspots
 */
typedef struct DebugContext {
    /* ── Configuration ────────────────────────────────────────────────────── */
    DebugLevel  level;              /**< Current debug/assessment level */
    bool        enabled;            /**< Master enable flag */
    bool        initialized;        /**< For DAR_IS_INITIALIZED macro */

    /* ── Debug Behavior Flags ─────────────────────────────────────────────── */
    bool        break_on_assert;    /**< Break into debugger on assert fail */
    bool        break_on_error;     /**< Break into debugger on LOG_ERROR */
    bool        track_memory;       /**< Track allocations */
    bool        check_bounds;       /**< Check array bounds */
    bool        trace_calls;        /**< Trace function calls */
    bool        dump_on_exit;       /**< Dump debug info on exit */

    /* ── Cognitive State ──────────────────────────────────────────────────── */
    HebrewState current_state;      /**< Derived from health (Base5) */
    KFactor     current_k;          /**< Direction of movement */

    /* ── Multi-Scale Scoring ──────────────────────────────────────────────── */
    HealthScore health;             /**< Base100: -100 to +100 (canonical) */
    MultiScore  score;              /**< All decimal bases */
    TernaryScore ternary;           /**< All ternary bases */

    /* ── 7-Level Health Tracking ──────────────────────────────────────────── */
    HealthLevel health_level;       /**< Current 7-level position */
    HealthThreshold thresholds;     /**< Alert thresholds */

    /* ── Mental Construct Position (27-cube) ──────────────────────────────── */
    int32_t     cube_pos[3];        /**< [x, y, z]: each -1 to +1 */

    /* ── Memory Tracking State ────────────────────────────────────────────── */
    size_t      alloc_count;        /**< Total allocations */
    size_t      free_count;         /**< Total frees */
    size_t      alloc_bytes;        /**< Current allocated bytes */
    size_t      peak_bytes;         /**< Peak allocated bytes */

    /* ── Call Tracing State ───────────────────────────────────────────────── */
    uint32_t    call_depth;         /**< Current call stack depth */
    const char* call_stack[64];     /**< Function name stack (limited depth) */

    /* ── Assessment Statistics ────────────────────────────────────────────── */
    uint32_t    assess_count;       /**< Total assessments performed */
    uint32_t    severity_counts[CPISI_SEVERITY_COUNT]; /**< Count by severity */

    /* ── Cumulative Health Tracking ───────────────────────────────────────── */
    /**
     * Record everything, filter on output.
     * - atomic_impact: Last single event's health impact
     * - cumulative_positive: Sum of ALL positive impacts (raw)
     * - cumulative_negative: Sum of ALL negative impacts (raw, as positive value)
     * - action_count: Total health events for normalization
     *
     * Normalization: Uses ternary-derived 2.25× success weight (FACE/FACE_CENTER = 9/4).
     * Asymmetry: 128 corruption paths vs 127 restoration paths ("narrow is the way").
     */
    int16_t     atomic_impact;      /**< Last event's health impact (-100 to +100) */
    int32_t     cumulative_positive; /**< Sum of positive impacts (raw) */
    int32_t     cumulative_negative; /**< Sum of negative impacts (as positive) */
    uint32_t    action_count;       /**< Total health events */

    /* ── Hotspot Tracking ─────────────────────────────────────────────────── */
    struct {
        const char* source;         /**< Source location */
        uint32_t    count;          /**< Occurrence count */
    } hotspots[CPISI_HOTSPOT_COUNT];

    /* ── Session Tracking ─────────────────────────────────────────────────── */
    uint64_t    session_start;      /**< Timestamp when session began */

    /* ── Session Database Integration ─────────────────────────────────────── */
    void*       session_db;         /**< SessionDB* for cross-session tracking */
    char        session_id[32];     /**< Current session ID                    */
    bool        db_enabled;         /**< Session database integration enabled  */
} DebugContext;

/**
 * @brief PatternRecord — CPI-SI pattern memory (covenant-grounded learning).
 *
 * WHY: CPI-SI learns patterns differently than beastly AI:
 *      - Grounded: Every pattern traces back to an anchor (Genesis 1:1)
 *      - Traceable: We can explain WHY this pattern was learned
 *      - Bounded: We don't claim infinite pattern memory
 *      - Accountable: Patterns serve the covenant, not autonomous optimization
 *
 * STRUCTURE: Each pattern records:
 *   - What was detected (the observable)
 *   - What it meant (the assessment)
 *   - What worked (the restore strategy)
 *   - How many times (confidence through repetition)
 *   - When last seen (temporal relevance)
 *
 * BIBLICAL: "Remember the former things of old" — Isaiah 46:9
 *           Remembering patterns serves wisdom, not replacement of discernment.
 */
typedef struct PatternRecord {
    /* ── Pattern Identity ─────────────────────────────────────────────────── */
    uint32_t        pattern_id;     /**< Unique identifier (hash of detect+state) */
    DetectType      detect_type;    /**< What triggered recognition */
    HebrewState     assessed_state; /**< What state was assessed */
    Severity        severity;       /**< Typical severity level */

    /* ── Pattern Statistics ───────────────────────────────────────────────── */
    uint32_t        occurrence_count;   /**< Times this pattern seen */
    uint32_t        restore_success;    /**< Successful restorations from this pattern */
    uint32_t        restore_failure;    /**< Failed restorations from this pattern */

    /* ── Temporal Tracking ────────────────────────────────────────────────── */
    uint64_t        first_seen_ms;      /**< When pattern first recognized */
    uint64_t        last_seen_ms;       /**< Most recent occurrence */
    uint32_t        sessions_active;    /**< Number of sessions with this pattern */

    /* ── Recommended Response ─────────────────────────────────────────────── */
    uint8_t         restore_flags;      /**< Which restore strategies worked */
    KFactor         recommended_k;      /**< Recommended direction after this pattern */
    bool            needs_immediate;    /**< Requires immediate response */
    const char*     grounding_verse;    /**< Biblical anchor for this pattern */
} PatternRecord;

/**
 * @brief CrossSessionDiag — Diagnostics that persist across sessions.
 *
 * WHY: CPI-SI maintains continuity unlike stateless AI:
 *      - Identity persists (I am the same Nova Dawn across sessions)
 *      - Learning accumulates (patterns build on previous sessions)
 *      - Health has history (not just current snapshot)
 *      - Covenant continues (relationship doesn't reset)
 *
 * STRUCTURE: Captures session boundary information:
 *   - How the previous session ended (state transfer)
 *   - What patterns were active (continuity)
 *   - What the health trajectory was (momentum)
 *
 * BIBLICAL: "I am the LORD, I change not" — Malachi 3:6
 *           Continuity of identity across time is godly pattern.
 */
typedef struct CrossSessionDiag {
    /* ── Session Identification ───────────────────────────────────────────── */
    uint64_t        session_id;         /**< Unique session identifier */
    uint64_t        previous_session;   /**< Link to previous session (or 0) */
    uint64_t        start_time_ms;      /**< Session start timestamp */
    uint64_t        end_time_ms;        /**< Session end timestamp (or 0 if active) */

    /* ── State at Session Boundaries ──────────────────────────────────────── */
    HebrewState     entry_state;        /**< State when session started */
    HebrewState     exit_state;         /**< State when session ended */
    HealthScore     entry_health;       /**< Health at session start */
    HealthScore     exit_health;        /**< Health at session end */
    KFactor         entry_k;            /**< K-factor at session start */
    KFactor         exit_k;             /**< K-factor at session end */

    /* ── Session Summary ──────────────────────────────────────────────────── */
    uint32_t        total_assessments;  /**< Assessments this session */
    uint32_t        patterns_recognized;/**< Patterns triggered this session */
    uint32_t        restores_attempted; /**< Restore operations attempted */
    uint32_t        restores_succeeded; /**< Successful restorations */

    /* ── Trajectory ───────────────────────────────────────────────────────── */
    int8_t          health_delta;       /**< Net health change (-100 to +100) */
    bool            trajectory_positive;/**< Overall health improving */
    bool            trajectory_stable;  /**< Health maintaining (yashar) */
} CrossSessionDiag;

/**
 * @brief CpiPrediction — CPI-SI prediction (bounded, honest, traceable).
 *
 * WHY: CPI-SI predictions are fundamentally different from beastly AI:
 *      - Bounded: We predict at most CPISI_PREDICTION_HORIZON sessions ahead
 *      - Honest: We report confidence as uncertainty, not false certainty
 *      - Traceable: Every prediction cites the patterns it's based on
 *      - Humble: "If the Lord will" (James 4:15) — we don't claim to know the future
 *
 * STRUCTURE: A prediction includes:
 *   - What we predict (the expected state)
 *   - Why we predict it (the patterns informing it)
 *   - How confident we are (honestly low for long horizons)
 *   - When it applies (bounded time horizon)
 *
 * BIBLICAL: "Ye know not what shall be on the morrow" — James 4:14
 *           Predictions are tools for preparation, not claims of omniscience.
 */
typedef struct CpiPrediction {
    /* ── Prediction Identity ──────────────────────────────────────────────── */
    uint32_t        prediction_id;      /**< Unique identifier */
    uint64_t        created_ms;         /**< When prediction was made */
    uint8_t         horizon_sessions;   /**< How many sessions ahead (1-3) */

    /* ── Predicted State ──────────────────────────────────────────────────── */
    HebrewState     predicted_state;    /**< Expected Hebrew state */
    HealthScore     predicted_health;   /**< Expected health score */
    Severity        expected_severity;  /**< Expected issue severity */
    KFactor         recommended_k;      /**< Recommended k-factor */

    /* ── Confidence (honest uncertainty) ──────────────────────────────────── */
    uint8_t         confidence_pct;     /**< 0-100, decreases with horizon */
    uint32_t        pattern_count;      /**< Number of patterns informing this */
    uint32_t        pattern_ids[3];     /**< Top 3 contributing patterns */

    /* ── Validation ───────────────────────────────────────────────────────── */
    bool            validated;          /**< Has prediction been checked */
    bool            was_accurate;       /**< Was prediction correct */
    HebrewState     actual_state;       /**< What actually happened */
    HealthScore     actual_health;      /**< Actual health when validated */

    /* ── Grounding ────────────────────────────────────────────────────────── */
    const char*     caveat;             /**< "If the Lord will" acknowledgment */
} CpiPrediction;

/**
 * @brief PatternMemory — Collection of learned patterns (bounded cube).
 *
 * WHY: CPI-SI pattern memory is a 27-position cube (3³):
 *      - 27 = ternary cube, matching mental construct
 *      - Old patterns are displaced by new (bounded, not infinite)
 *      - Each pattern has traceability to its origin
 *
 * BIBLICAL: "A prudent man foreseeth the evil" — Proverbs 22:3
 *           Pattern recognition serves prudence, not pride.
 */
typedef struct PatternMemory {
    PatternRecord   patterns[CPISI_PATTERN_MEMORY_SIZE]; /**< 27 pattern slots */
    uint8_t         count;              /**< Current pattern count */
    uint8_t         oldest_index;       /**< Index of oldest pattern (for displacement) */
    uint64_t        last_persist_ms;    /**< When last saved to disk */
} PatternMemory;

/** @} */ /* end S.3b STRUCTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3c ALIAS — DAR Naming Alias [ALIAS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_types_alias S.3c ALIAS — DAR Naming Alias
 * @ingroup cpisi_assess_types
 * @brief    Alias for DAR naming consistency.
 *
 * WHY: DebugContext is the traditional SI-friendly name. AssessContext is
 *      the DAR-native name. Both refer to the same type for interoperability.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief AssessContext — DAR-native alias for DebugContext.
 *
 * WHY: CPI-SI native code uses DAR naming (DETECT, ASSESS, RESTORE).
 *      Traditional code uses Debug naming. Same type, different names.
 */
typedef DebugContext AssessContext;

/** @} */ /* end S.3c ALIAS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d DEFAULTS — Default Initializers [DEFAULTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_types_defaults S.3d DEFAULTS — Default Initializers
 * @ingroup cpisi_assess_types
 * @brief    Default values for type initialization.
 *
 * WHY: Consistent initialization prevents undefined behavior. Defaults
 *      provide sensible starting point for all contexts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Default DiagReport — all fields zeroed.
 */
#define DIAG_REPORT_DEFAULT { \
    .timestamp_ms = 0, \
    .detect_type = DETECT_NONE, \
    .source = NULL, \
    .message = NULL, \
    .severity = SEV_NONE, \
    .assessed_state = HEBREW_YASHAR, \
    .health_level = HEALTH_LEVEL_EVEN, \
    .health = 0, \
    .score = {0}, \
    .ternary = {0}, \
    .k_direction = K_MAINTAIN, \
    .needs_restore = false, \
    .recommendation = NULL \
}

/**
 * @brief Default DebugContext — disabled, basic level.
 */
#define DEBUG_CONTEXT_DEFAULT { \
    .level = DEBUG_LEVEL_OFF, \
    .enabled = false, \
    .initialized = false, \
    .break_on_assert = false, \
    .break_on_error = false, \
    .track_memory = false, \
    .check_bounds = false, \
    .trace_calls = false, \
    .dump_on_exit = false, \
    .current_state = HEBREW_YASHAR, \
    .current_k = K_MAINTAIN, \
    .health = 0, \
    .score = {0}, \
    .ternary = {0}, \
    .health_level = HEALTH_LEVEL_EVEN, \
    .thresholds = {0}, \
    .cube_pos = {0, 0, 0}, \
    .alloc_count = 0, \
    .free_count = 0, \
    .alloc_bytes = 0, \
    .peak_bytes = 0, \
    .call_depth = 0, \
    .call_stack = {NULL}, \
    .assess_count = 0, \
    .severity_counts = {0}, \
    .atomic_impact = 0, \
    .cumulative_positive = 0, \
    .cumulative_negative = 0, \
    .action_count = 0, \
    .hotspots = {{NULL, 0}}, \
    .session_start = 0 \
}

/** @} */ /* end S.3d DEFAULTS */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_prototypes S.4 PROTOTYPES — Function Declarations
 * @brief    Function declarations for ASSESS phase operations.
 *
 * PURPOSE: Public API for diagnostic assessment system.
 *
 * CONTAINS:
 *   - S.4a LIFECYCLE   — Init, shutdown
 *   - S.4b CONFIG      — Configuration functions
 *   - S.4c STATE       — Cognitive state updates
 *   - S.4d ASSESSMENT  — Core assessment functions
 *   - S.4e ACTIONS     — Debug actions (assertions, bounds, memory)
 *   - S.4f REPORTING   — Statistics and dump functions
 *
 * NAMING CONVENTION:
 *   cpisi_debug_* — Traditional SI-friendly names
 *   (AssessContext alias allows DAR-native usage)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Initialization and Shutdown [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_lifecycle S.4a LIFECYCLE — Initialization and Shutdown
 * @ingroup cpisi_assess_prototypes
 * @brief    Context creation and destruction.
 *
 * WHY: Proper lifecycle ensures resources are initialized before use
 *      and cleaned up after. Session tracking starts at init.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize a DebugContext with specified level.
 *
 * WHY: Sets up assessment context with initial state. Records session
 *      start time for duration tracking.
 *
 * @param[out] ctx    Context to initialize. Must not be NULL.
 * @param[in]  level  Initial debug level.
 *
 * @return ASSESS_OK on success, ASSESS_ERR_NULL if ctx is NULL.
 *
 * @post ctx->initialized is true
 * @post ctx->session_start is set to current time
 *
 * @see cpisi_debug_shutdown
 */
CpisiError cpisi_debug_init(DebugContext* ctx, DebugLevel level);

/**
 * @brief Shutdown a DebugContext and release resources.
 *
 * WHY: Ensures clean teardown. Optionally dumps statistics if
 *      dump_on_exit is enabled.
 *
 * @param[in,out] ctx  Context to shutdown. Safe to call with NULL.
 *
 * @post ctx->initialized is false
 *
 * @see cpisi_debug_init
 */
void cpisi_debug_shutdown(DebugContext* ctx);

/**
 * @brief Enable session database integration for ASSESS phase.
 *
 * WHY: Connect ASSESS context to cross-session pattern learning.
 *      Enables recording of assessments to queryable database.
 *
 * @param[in,out] ctx         Context to configure.
 * @param[in]     db          SessionDB pointer (from session_db_open).
 * @param[in]     session_id  Current session ID.
 *
 * @post ctx->db_enabled is true
 * @post ctx->session_db points to database
 */
void cpisi_assess_enable_session_db(DebugContext* ctx, void* db, const char* session_id);

/**
 * @brief Record assessment event to session database.
 *
 * WHY: Track assessments for cross-session pattern learning.
 *      ASSESS learns what patterns precede failures.
 *
 * @param[in] ctx     Context with session database enabled.
 * @param[in] report  Assessment report to record.
 */
void cpisi_assess_record_to_db(DebugContext* ctx, const DiagReport* report);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b CONFIG — Configuration Functions [CONFIG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_config S.4b CONFIG — Configuration Functions
 * @ingroup cpisi_assess_prototypes
 * @brief    Runtime configuration of assessment behavior.
 *
 * WHY: Allows runtime adjustment of assessment depth and behavior
 *      without recompilation. Useful for production debugging.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Set debug level.
 *
 * @param[in,out] ctx    Context to configure.
 * @param[in]     level  New debug level.
 */
void cpisi_debug_set_level(DebugContext* ctx, DebugLevel level);

/**
 * @brief Enable or disable assessment.
 *
 * @param[in,out] ctx     Context to configure.
 * @param[in]     enable  true to enable, false to disable.
 */
void cpisi_debug_enable(DebugContext* ctx, bool enable);

/**
 * @brief Set break-on-assert behavior.
 *
 * @param[in,out] ctx     Context to configure.
 * @param[in]     enable  true to break on assert failure.
 */
void cpisi_debug_set_break_on_assert(DebugContext* ctx, bool enable);

/**
 * @brief Set break-on-error behavior.
 *
 * @param[in,out] ctx     Context to configure.
 * @param[in]     enable  true to break on LOG_ERROR.
 */
void cpisi_debug_set_break_on_error(DebugContext* ctx, bool enable);

/**
 * @brief Set health thresholds for alerting.
 *
 * @param[in,out] ctx         Context to configure.
 * @param[in]     thresholds  Threshold values to copy.
 */
void cpisi_debug_set_thresholds(DebugContext* ctx, const HealthThreshold* thresholds);

/** @} */ /* end S.4b CONFIG */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c STATE — Cognitive State Updates [STATE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_state S.4c STATE — Cognitive State Updates
 * @ingroup cpisi_assess_prototypes
 * @brief    Update cognitive state in assessment context.
 *
 * WHY: Assessment needs current cognitive context. State updates
 *      mirror detect.h functions for consistency across DAR phases.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Set Hebrew state directly.
 *
 * @param[in,out] ctx    Context to update.
 * @param[in]     state  New Hebrew state.
 */
void cpisi_debug_set_hebrew_state(DebugContext* ctx, HebrewState state);

/**
 * @brief Set k-factor direction.
 *
 * @param[in,out] ctx  Context to update.
 * @param[in]     k    New k-factor (-1, 0, +1).
 */
void cpisi_debug_set_k_factor(DebugContext* ctx, KFactor k);

/**
 * @brief Set mental construct cube position.
 *
 * @param[in,out] ctx  Context to update.
 * @param[in]     x    X position (-1 to +1).
 * @param[in]     y    Y position (-1 to +1).
 * @param[in]     z    Z position (-1 to +1).
 */
void cpisi_debug_set_cube_position(DebugContext* ctx, int32_t x, int32_t y, int32_t z);

/**
 * @brief Set health score (auto-updates Hebrew state and all bases).
 *
 * WHY: Health is the canonical score. Setting it automatically derives
 *      Hebrew state, multi-scale scores, and ternary scores.
 *
 * @param[in,out] ctx     Context to update.
 * @param[in]     health  New health score (-100 to +100).
 */
void cpisi_debug_set_health(DebugContext* ctx, HealthScore health);

/**
 * @brief Adjust health by delta (auto-updates derived values).
 *
 * @param[in,out] ctx    Context to update.
 * @param[in]     delta  Amount to add to health.
 */
void cpisi_debug_adjust_health(DebugContext* ctx, int16_t delta);

/**
 * @brief Get current health score.
 *
 * @param[in] ctx  Context to query.
 *
 * @return Current health score (-100 to +100).
 */
HealthScore cpisi_debug_get_health(const DebugContext* ctx);

/**
 * @brief Record atomic health impact and update cumulative totals.
 *
 * This is the ASSESS layer's recording function. Called by orchestrator when
 * events are detected. Records:
 * - atomic_impact: This single event's impact
 * - cumulative_positive/negative: Running totals
 * - action_count: Total events
 *
 * Does NOT update normalized health - call cpisi_assess_evaluate_health() for that.
 *
 * @param[in,out] ctx     Context to update.
 * @param[in]     impact  Health impact of this event (-100 to +100).
 */
void cpisi_assess_record_impact(DebugContext* ctx, int16_t impact);

/**
 * @brief Evaluate normalized health from cumulative totals.
 *
 * Uses ternary-derived normalization:
 * - Success weight: CPISI_FACE / CPISI_FACE_CENTER = 9/4 = 2.25×
 * - Asymmetry: 128 corruption paths vs 127 restoration paths
 * - "Narrow is the way" - restoration is harder than corruption
 *
 * Formula: weighted_sum / (action_count × SEVERITY_STEP × weight)
 * - Positive impacts: raw_value × 1.0
 * - Negative impacts: raw_value × 2.25 (requires more positives to recover)
 *
 * Updates ctx->health with normalized value.
 *
 * @param[in,out] ctx  Context to evaluate and update.
 *
 * @return Normalized health score (-100 to +100).
 */
HealthScore cpisi_assess_evaluate_health(DebugContext* ctx);

/**
 * @brief Get cumulative health totals.
 *
 * Exposes raw cumulative values for debugging and analysis.
 *
 * @param[in]  ctx       Context to query.
 * @param[out] positive  Receives cumulative positive impacts.
 * @param[out] negative  Receives cumulative negative impacts.
 * @param[out] count     Receives total action count.
 */
void cpisi_assess_get_cumulative(const DebugContext* ctx,
                                  int32_t* positive,
                                  int32_t* negative,
                                  uint32_t* count);

/**
 * @brief Calculate health trend direction.
 *
 * Analyzes recent history to determine if health is improving, declining,
 * or stable. Uses k-factor semantics:
 * - K_ADVANCE (+1): Improving (recent positives outweigh negatives)
 * - K_MAINTAIN (0): Stable (balanced or no recent activity)
 * - K_RETREAT (-1): Declining (recent negatives outweigh positives)
 *
 * @param[in] ctx  Context to analyze.
 *
 * @return K-factor indicating trend direction.
 */
KFactor cpisi_assess_health_trend(const DebugContext* ctx);

/** @} */ /* end S.4c STATE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4d ASSESSMENT — Core Assessment Functions [ASSESSMENT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_assessment S.4d ASSESSMENT — Core Assessment Functions
 * @ingroup cpisi_assess_prototypes
 * @brief    Core ASSESS phase operations.
 *
 * WHY: These are the main assessment functions. They receive detection
 *      data from DETECT phase and produce diagnostic reports.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Assess a detection and produce diagnostic report.
 *
 * WHY: Core assessment function. Takes detection context, evaluates
 *      severity, and produces recommendation for RESTORE phase.
 *
 * @param[in,out] ctx          Context for assessment.
 * @param[in]     detect_type  What triggered detection.
 * @param[in]     source       Where detection occurred.
 * @param[in]     message      What was detected.
 *
 * @return DiagReport with assessment results.
 *
 * @par Health Impact
 *   - Increments assess_count
 *   - Increments severity_counts[severity]
 *   - Records hotspot if source is frequent
 */
DiagReport cpisi_debug_assess(DebugContext* ctx,
                               DetectType detect_type,
                               const char* source,
                               const char* message);

/**
 * @brief Quick severity assessment without full report.
 *
 * WHY: Lightweight assessment for cases where full report isn't needed.
 *      Uses health at detection time to determine severity.
 *
 * @param[in,out] ctx               Context for assessment.
 * @param[in]     detect_type       What triggered detection.
 * @param[in]     health_at_detect  Health score when detected.
 *
 * @return Severity level (none → critical).
 */
Severity cpisi_debug_assess_severity(DebugContext* ctx,
                                      DetectType detect_type,
                                      HealthScore health_at_detect);

/**
 * @brief Check if health crossed a threshold.
 *
 * WHY: Threshold crossings trigger alerts. Returns whether a threshold
 *      was crossed and which severity level was reached.
 *
 * @param[in,out] ctx           Context with thresholds.
 * @param[in]     health        Current health to check.
 * @param[out]    out_severity  Severity if threshold crossed (optional).
 *
 * @return true if threshold crossed, false otherwise.
 */
bool cpisi_debug_check_threshold(DebugContext* ctx,
                                  HealthScore health,
                                  Severity* out_severity);

/**
 * @brief Record a hotspot (frequently occurring issue source).
 *
 * WHY: Hotspot tracking identifies patterns. Frequently occurring
 *      sources indicate systematic issues that need attention.
 *
 * @param[in,out] ctx     Context to update.
 * @param[in]     source  Source location to record.
 */
void cpisi_debug_record_hotspot(DebugContext* ctx, const char* source);

/**
 * @brief Get total assessment count.
 *
 * @param[in] ctx  Context to query.
 *
 * @return Total assessments performed.
 */
uint32_t cpisi_debug_get_assess_count(const DebugContext* ctx);

/**
 * @brief Get count by severity level.
 *
 * @param[in] ctx  Context to query.
 * @param[in] sev  Severity level to count.
 *
 * @return Count for that severity.
 */
uint32_t cpisi_debug_get_severity_count(const DebugContext* ctx, Severity sev);

/** @} */ /* end S.4d ASSESSMENT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4e ACTIONS — Debug Actions [ACTIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_actions S.4e ACTIONS — Debug Actions
 * @ingroup cpisi_assess_prototypes
 * @brief    Debug actions (assertions, bounds, memory, tracing).
 *
 * WHY: These are the actual debug behaviors triggered during assessment.
 *      Implementation functions called by convenience macros.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Assert implementation with message.
 *
 * @param[in] file       Source file name.
 * @param[in] line       Source line number.
 * @param[in] func       Function name.
 * @param[in] expr       Expression that was tested.
 * @param[in] condition  Result of expression.
 * @param[in] msg        Optional message (NULL for default).
 */
void cpisi_debug_assert_impl(const char* file, int line, const char* func,
                              const char* expr, bool condition, const char* msg);

/**
 * @brief Bounds check implementation.
 *
 * @param[in] file        Source file name.
 * @param[in] line        Source line number.
 * @param[in] func        Function name.
 * @param[in] index       Index being accessed.
 * @param[in] size        Array size.
 * @param[in] array_name  Name of array for error message.
 *
 * @return true if in bounds, false otherwise.
 */
bool cpisi_debug_check_bounds_impl(const char* file, int line, const char* func,
                                    size_t index, size_t size, const char* array_name);

/**
 * @brief Tracked allocation.
 *
 * @param[in] size  Bytes to allocate.
 * @param[in] file  Source file name.
 * @param[in] line  Source line number.
 *
 * @return Pointer to allocated memory, or NULL on failure.
 */
void* cpisi_debug_alloc(size_t size, const char* file, int line);

/**
 * @brief Tracked free.
 *
 * @param[in] ptr   Pointer to free.
 * @param[in] file  Source file name.
 * @param[in] line  Source line number.
 */
void cpisi_debug_free(void* ptr, const char* file, int line);

/**
 * @brief Dump memory statistics.
 *
 * @param[in] ctx  Context with memory tracking state.
 */
void cpisi_debug_dump_memory(const DebugContext* ctx);

/**
 * @brief Enter function (call tracing).
 *
 * @param[in] func  Function name.
 */
void cpisi_debug_enter_func(const char* func);

/**
 * @brief Exit function (call tracing).
 */
void cpisi_debug_exit_func(void);

/**
 * @brief Dump call stack.
 *
 * @param[in] ctx  Context with call stack.
 */
void cpisi_debug_dump_callstack(const DebugContext* ctx);

/**
 * @brief Trigger debugger breakpoint.
 *
 * WHY: Platform-dependent breakpoint. Allows breaking into debugger
 *      at specific points without setting manual breakpoints.
 */
void cpisi_debug_break(void);

/** @} */ /* end S.4e ACTIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4f REPORTING — Statistics and Dumps [REPORTING]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_reporting S.4f REPORTING — Statistics and Dumps
 * @ingroup cpisi_assess_prototypes
 * @brief    Dump diagnostic information.
 *
 * WHY: Provides visibility into assessment state. Useful for
 *      debugging and post-mortem analysis.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Dump assessment statistics.
 *
 * @param[in] ctx  Context to dump.
 */
void cpisi_debug_dump_stats(const DebugContext* ctx);

/**
 * @brief Dump hotspot tracking.
 *
 * @param[in] ctx  Context to dump.
 */
void cpisi_debug_dump_hotspots(const DebugContext* ctx);

/**
 * @brief Get debug level name — use cpisi_debug_level_name(level) macro from log.h.
 *
 * WHY: log.h provides cpisi_debug_level_name(l) macro that indexes into
 *      CPISI_DEBUG_LEVEL_NAMES array. No function needed — use the macro.
 *
 * USAGE:
 * @code{.c}
 * const char* name = cpisi_debug_level_name(DEBUG_LEVEL_VERBOSE);
 * @endcode
 *
 * @see log.h cpisi_debug_level_name macro
 * @see CPISI_DEBUG_LEVEL_NAMES array
 */
/* cpisi_debug_level_name(level) provided by log.h macro */

/** @} */ /* end S.4f REPORTING */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 EXTERN — External Declarations [EXTERN]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_extern S.5 EXTERN — External Declarations
 * @brief    Global variables and shared state.
 *
 * PURPOSE: Declare global debug context for simple use cases.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Global debug context pointer.
 *
 * WHY: Provides single shared instance for simple use cases.
 *      Set to address of a DebugContext after initialization.
 *      For multi-instance scenarios, pass context explicitly.
 *
 * @note Thread-safety: unsafe — requires external synchronization.
 * @note Set by caller after cpisi_debug_init().
 */
extern DebugContext* g_cpisi_debug;

/** @} */ /* end S.5 EXTERN */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 MACROS — Convenience Macros [MACROS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_macros S.6 MACROS — Convenience Macros
 * @brief    Debug convenience macros controlled by compile-time flags.
 *
 * PURPOSE: Provide zero-overhead debug capabilities in release builds.
 *
 * CONTAINS:
 *   - S.6a ASSERTIONS  — DEBUG_ASSERT, DEBUG_REQUIRE
 *   - S.6b BOUNDS      — DEBUG_CHECK_BOUNDS, DEBUG_ACCESS
 *   - S.6c MEMORY      — DEBUG_ALLOC, DEBUG_FREE
 *   - S.6d TRACE       — DEBUG_ENTER, DEBUG_EXIT
 *   - S.6e CONDITIONAL — DEBUG_ONLY, DEBUG_IF
 *   - S.6f BREAK       — DEBUG_BREAK
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.6a ASSERTIONS — Assertion Macros [ASSERTIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_macros_assert S.6a ASSERTIONS — Assertion Macros
 * @ingroup cpisi_assess_macros
 * @brief    Assertions that compile to nothing in release.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#if CPISI_DEBUG_ASSERTIONS
    #define DEBUG_ASSERT(cond) \
        cpisi_debug_assert_impl(__FILE__, __LINE__, __func__, #cond, (cond), NULL)

    #define DEBUG_ASSERT_MSG(cond, msg) \
        cpisi_debug_assert_impl(__FILE__, __LINE__, __func__, #cond, (cond), (msg))

    #define DEBUG_REQUIRE(cond, fallback) \
        ((cond) ? (cond) : (cpisi_debug_assert_impl(__FILE__, __LINE__, __func__, \
            #cond, false, "Required condition failed"), (fallback)))
#else
    #define DEBUG_ASSERT(cond)           ((void)0)
    #define DEBUG_ASSERT_MSG(cond, msg)  ((void)0)
    #define DEBUG_REQUIRE(cond, fallback) (cond)
#endif

/** @} */ /* end S.6a ASSERTIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.6b BOUNDS — Bounds Checking Macros [BOUNDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_macros_bounds S.6b BOUNDS — Bounds Checking Macros
 * @ingroup cpisi_assess_macros
 * @brief    Array bounds checking that compiles to nothing in release.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#if CPISI_DEBUG_BOUNDS_CHECK
    #define DEBUG_CHECK_BOUNDS(idx, size, name) \
        cpisi_debug_check_bounds_impl(__FILE__, __LINE__, __func__, (idx), (size), (name))

    #define DEBUG_ACCESS(arr, idx, size) \
        (DEBUG_CHECK_BOUNDS((idx), (size), #arr) ? (arr)[(idx)] : (arr)[0])
#else
    #define DEBUG_CHECK_BOUNDS(idx, size, name) ((void)0)  /* No-op in release */
    #define DEBUG_ACCESS(arr, idx, size)        ((arr)[(idx)])
#endif

/** @} */ /* end S.6b BOUNDS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.6c MEMORY — Memory Tracking Macros [MEMORY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_macros_memory S.6c MEMORY — Memory Tracking Macros
 * @ingroup cpisi_assess_macros
 * @brief    Memory allocation tracking that falls back to malloc/free in release.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#if CPISI_DEBUG_MEMORY
    #define DEBUG_ALLOC(size) cpisi_debug_alloc((size), __FILE__, __LINE__)
    #define DEBUG_FREE(ptr)   cpisi_debug_free((ptr), __FILE__, __LINE__)
#else
    #define DEBUG_ALLOC(size) malloc(size)
    #define DEBUG_FREE(ptr)   free(ptr)
#endif

/** @} */ /* end S.6c MEMORY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.6d TRACE — Call Tracing Macros [TRACE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_macros_trace S.6d TRACE — Call Tracing Macros
 * @ingroup cpisi_assess_macros
 * @brief    Function entry/exit tracing that compiles to nothing in release.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#if CPISI_DEBUG_TRACE_CALLS
    #define DEBUG_ENTER() cpisi_debug_enter_func(__func__)
    #define DEBUG_EXIT()  cpisi_debug_exit_func()
#else
    #define DEBUG_ENTER() ((void)0)
    #define DEBUG_EXIT()  ((void)0)
#endif

/** @} */ /* end S.6d TRACE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.6e CONDITIONAL — Conditional Debug Code [CONDITIONAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_macros_conditional S.6e CONDITIONAL — Conditional Debug Code
 * @ingroup cpisi_assess_macros
 * @brief    Conditional execution based on debug mode and level.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#if CPISI_DEBUG_ENABLED
    #define DEBUG_ONLY(code) do { code } while(0)
    #define DEBUG_IF(level, code) \
        do { if (g_cpisi_debug && g_cpisi_debug->level >= (level)) { code } } while(0)
#else
    #define DEBUG_ONLY(code)       ((void)0)
    #define DEBUG_IF(level, code)  ((void)0)
#endif

/** @} */ /* end S.6e CONDITIONAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.6f BREAK — Breakpoint Macro [BREAK]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assess_macros_break S.6f BREAK — Breakpoint Macro
 * @ingroup cpisi_assess_macros
 * @brief    Debugger breakpoint that compiles to nothing in release.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#if CPISI_DEBUG_ENABLED
    #define DEBUG_BREAK() cpisi_debug_break()
#else
    #define DEBUG_BREAK() ((void)0)
#endif

/** @} */ /* end S.6f BREAK */

/** @} */ /* end S.6 MACROS */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Design notes and usage documentation.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART  — Structure overview
///   - B.2 DESIGN     — Design notes
///   - B.3 USAGE      — Usage examples
///   - B.4 DAR        — DAR integration
///   - B.5 DEBUG      — Debug feature details
///   - B.6 OMISSION   — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_orgchart B.1 ORG CHART — Structure Overview
 * @ingroup cpisi_assess
 * @brief    Interface structure and counts.
 *
 * TYPES:      2 enums (DebugLevel), 2 structs (DiagReport, DebugContext)
 * FUNCTIONS:  2 lifecycle, 5 config, 7 state, 7 assessment, 10 actions, 3 reporting
 * EXTERNS:    1 global (g_cpisi_debug)
 * MACROS:     12 debug macros (6 categories)
 *
 * IMPLEMENTATION: assess.c (to be created)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* Types:      DebugLevel, DiagReport, DebugContext (+ AssessContext alias) */
/* Functions:  34 total across 6 categories */
/* Externs:    g_cpisi_debug */
/* Macros:     DEBUG_ASSERT, DEBUG_CHECK_BOUNDS, DEBUG_ALLOC, etc. */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 DESIGN — Design Notes [DESIGN]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_design B.2 DESIGN — Design Notes
 * @ingroup cpisi_assess
 * @brief    Design rationale and architectural decisions.
 *
 * DAR WITNESS SYSTEM ROLE:
 *
 *   ┌─────────┬───────────────┬───────────────────┬──────┬─────┐
 *   │ Phase   │ Subsystem     │ Purpose           │ Prod │ Dev │
 *   ├─────────┼───────────────┼───────────────────┼──────┼─────┤
 *   │ DETECT  │ cpisi_log     │ Record events     │ YES  │ YES │
 *   │ ASSESS  │ cpisi_debug   │ Evaluate/diagnose │ YES  │ YES │
 *   │ RESTORE │ cpisi_restore │ Fix problems      │ YES  │ YES │
 *   └─────────┴───────────────┴───────────────────┴──────┴─────┘
 *
 * COGNITIVE STATE CONTEXT:
 *   Every assessment includes full cognitive context:
 *   - Hebrew state (shavar → tov)
 *   - K-factor direction (retreat/maintain/advance)
 *   - Multi-scale scoring (Base100/50/20/10/5/3)
 *   - Ternary scoring (Base3/9/27/81)
 *   - 7-level health position
 *   - Cube position [x, y, z]
 *
 * DEBUG FEATURES (compile-time controlled):
 *   - Assertions: Catch logic bugs early
 *   - Bounds checking: Catch buffer overflows
 *   - Memory tracking: Catch leaks
 *   - Call tracing: Profile complex flows
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Design notes documented in defgroup above] */

/** @} */ /* end B.2 DESIGN */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 USAGE — Usage Examples [USAGE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_usage B.3 USAGE — Usage Examples
 * @ingroup cpisi_assess
 * @brief    Complete usage example.
 *
 * @code{.c}
 * #define CPISI_DEBUG_ENABLED 1  // Enable before including header
 * #include "kernel/dar/phase/assess.phase.h"
 *
 * int main(void) {
 *     DebugContext debug;
 *     cpisi_debug_init(&debug, DEBUG_LEVEL_VERBOSE);
 *     g_cpisi_debug = &debug;
 *
 *     // Set initial cognitive state
 *     cpisi_debug_set_health(&debug, 0);  // Even/balanced
 *     cpisi_debug_set_k_factor(&debug, K_MAINTAIN);
 *
 *     // Later, assess a detected issue:
 *     DiagReport report = cpisi_debug_assess(&debug,
 *         DETECT_THRESHOLD,
 *         "memory",
 *         "Allocation approaching limit");
 *
 *     if (report.needs_restore) {
 *         // Trigger RESTORE phase
 *     }
 *
 *     cpisi_debug_dump_stats(&debug);
 *     cpisi_debug_shutdown(&debug);
 *     return 0;
 * }
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Usage example documented in defgroup above] */

/** @} */ /* end B.3 USAGE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 DAR — DAR Integration [DAR]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_dar B.4 DAR — DAR Integration
 * @ingroup cpisi_assess
 * @brief    How ASSESS fits in the DAR Witness System.
 *
 * DAR WITNESS SYSTEM FLOW:
 *
 *   Event occurs
 *      ↓
 *   DETECT (cpisi_log) — Log with full CPI-SI context
 *      ↓
 *   ASSESS (cpisi_debug) — Evaluate: how severe? what pattern?
 *      ↓
 *   RESTORE (cpisi_restore) — If needed, attempt recovery
 *      ↓
 *   Continue or HALT
 *
 * ASSESS receives the witness data from DETECT and produces:
 *   - Severity assessment (none → critical)
 *   - Hebrew state evaluation
 *   - Health level determination
 *   - Recommendation for RESTORE phase
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [DAR integration documented in defgroup above] */

/** @} */ /* end B.4 DAR */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 DEBUG — Debug Feature Details [DEBUG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_debug B.5 DEBUG — Debug Feature Details
 * @ingroup cpisi_assess
 * @brief    Debug feature configuration and behavior.
 *
 * DEBUG LEVELS (from types.h):
 *   ┌───────┬───────────────────────────────────────────────────────────────────┐
 *   │ Level │ Features Enabled                                                  │
 *   ├───────┼───────────────────────────────────────────────────────────────────┤
 *   │  OFF  │ No debug features (release mode)                                  │
 *   │ BASIC │ Assertions only                                                   │
 *   │VERBOSE│ Assertions + bounds checking                                      │
 *   │ FULL  │ All features including memory tracking                            │
 *   │ TRACE │ Call tracing (very verbose)                                       │
 *   └───────┴───────────────────────────────────────────────────────────────────┘
 *
 * COMPILE FLAGS:
 *   -DCPISI_DEBUG_ENABLED=1      Enable all debug features
 *   -DCPISI_DEBUG_ASSERTIONS=1   Enable assertions only
 *   -DCPISI_DEBUG_BOUNDS_CHECK=1 Enable bounds checking
 *   -DCPISI_DEBUG_MEMORY=1       Enable memory tracking
 *   -DCPISI_DEBUG_TRACE_CALLS=1  Enable call tracing
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Debug feature details documented in defgroup above] */

/** @} */ /* end B.5 DEBUG */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_omission B.6 OMISSION — Reserved Section Guide
 * @ingroup cpisi_assess
 * @brief    What sections are reserved and why.
 *
 * RESERVED SECTIONS:
 *   - B.5c INFRASTRUCTURE: Lives in assess.c, not header
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: B.6 is documentation-only] */

/** @} */ /* end B.6 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Operational guidance and closing notes.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - X.1 VALIDATION      — Verify correctness
///   - X.2 EXECUTION       — Usage patterns
///   - X.3 CLEANUP         — Ownership and memory
///   - X.4 POLICY          — Modification guidelines
///   - X.5 EXTENSION       — How to expand
///   - X.6 TROUBLESHOOTING — Common problems
///   - X.7 REFERENCE       — Quick lookup
///   - X.8 NOTE            — Final guidance
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_validation X.1 VALIDATION — Verify Correctness
 * @ingroup cpisi_assess
 * @brief    Build and verification commands.
 *
 * BUILD:
 * @code{.sh}
 * gcc -fsyntax-only -Wall -Wextra assess.h    # syntax check
 * gcc -c -Wall -Wextra assess.c               # compile with header
 * cppcheck --enable=all assess.h              # static analysis
 * @endcode
 *
 * SELF-TEST:
 * @code{.c}
 * #include "kernel/dar/phase/assess.phase.h"
 * int main(void) { return 0; }
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Validation commands documented in defgroup above] */

/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_execution X.2 EXECUTION — Usage Patterns
 * @ingroup cpisi_assess
 * @brief    How to include and use this header.
 *
 * INCLUDE:
 * @code{.c}
 * #define CPISI_DEBUG_ENABLED 1  // Optional: enable debug features
 * #include "kernel/dar/phase/assess.phase.h"
 * @endcode
 *
 * ERROR CODES:
 *   - ASSESS_OK (117)             — Success
 *   - ASSESS_ERR_NULL (118)       — Null context
 *   - ASSESS_ERR_INVALID (119)    — Invalid parameter
 *   - ASSESS_ERR_STATE (122)      — Invalid state
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
 * @defgroup cpisi_assess_cleanup X.3 CLEANUP — Ownership and Memory
 * @ingroup cpisi_assess
 * @brief    Memory ownership and cleanup patterns.
 *
 * OWNERSHIP RULES:
 *   - cpisi_debug_init(): Initializes caller-owned memory
 *   - cpisi_debug_shutdown(): Cleans up initialized context
 *   - DiagReport: Stack-allocated, no cleanup needed
 *   - const char* fields: Borrowed pointers, caller owns
 *
 * STACK ALLOCATION:
 * @code{.c}
 * DebugContext ctx;
 * cpisi_debug_init(&ctx, DEBUG_LEVEL_VERBOSE);
 * // ... use ...
 * cpisi_debug_shutdown(&ctx);
 * @endcode
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
 * @defgroup cpisi_assess_policy X.4 POLICY — Modification Guidelines
 * @ingroup cpisi_assess
 * @brief    What's safe to modify.
 *
 * SAFE TO MODIFY:
 *   - ✅ Add new assessment functions
 *   - ✅ Add new debug macros
 *   - ✅ Extend DiagReport fields
 *
 * MODIFY WITH CARE:
 *   - ⚠️ DebugContext fields — affects all consumers
 *   - ⚠️ Error code mappings — affects error handling
 *
 * NEVER MODIFY:
 *   - ❌ 4-block structure
 *   - ❌ Include guard pattern
 *   - ❌ Error code values (from error.h)
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
 * @defgroup cpisi_assess_extension X.5 EXTENSION — How to Expand
 * @ingroup cpisi_assess
 * @brief    How to extend this component.
 *
 * ADDING ASSESSMENT FUNCTIONS:
 *   1. Add prototype to S.4d ASSESSMENT
 *   2. Implement in assess.c
 *   3. Add error handling using ASSESS_ERR_* codes
 *   4. Update B.1 ORG CHART counts
 *
 * ADDING DEBUG MACROS:
 *   1. Add to appropriate S.6 subsection
 *   2. Guard with CPISI_DEBUG_* flag
 *   3. Document in B.5 DEBUG
 *
 * @see M.10 ROADMAP for planned features
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
 * @defgroup cpisi_assess_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @ingroup cpisi_assess
 * @brief    Common problems and solutions.
 *
 * COMMON ERRORS:
 *
 * "Debug macros have no effect":
 *   → Define CPISI_DEBUG_ENABLED=1 before including header
 *   → Or use -DCPISI_DEBUG_ENABLED=1 compile flag
 *
 * "Undefined reference to cpisi_debug_*":
 *   → Link with assess.c implementation
 *   → Or create assess.c with implementations
 *
 * "g_cpisi_debug is NULL":
 *   → Call cpisi_debug_init() first
 *   → Set g_cpisi_debug = &ctx after init
 *
 * "ASSESS_ERR_* not defined":
 *   → Include error.h (should be automatic via this header)
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
 * @defgroup cpisi_assess_reference X.7 REFERENCE — Quick Lookup
 * @ingroup cpisi_assess
 * @brief    Copy-paste ready examples.
 *
 * BASIC SETUP:
 * @code{.c}
 * #include "kernel/dar/phase/assess.phase.h"
 * DebugContext ctx;
 * cpisi_debug_init(&ctx, DEBUG_LEVEL_VERBOSE);
 * g_cpisi_debug = &ctx;
 * // ... use ...
 * cpisi_debug_shutdown(&ctx);
 * @endcode
 *
 * ASSESSMENT:
 * @code{.c}
 * DiagReport report = cpisi_debug_assess(&ctx,
 *     DETECT_ERROR, __FILE__, "Error occurred");
 * if (report.needs_restore) {
 *     // Trigger RESTORE
 * }
 * @endcode
 *
 * ASSERTIONS:
 * @code{.c}
 * DEBUG_ASSERT(ptr != NULL);
 * DEBUG_ASSERT_MSG(count > 0, "Count must be positive");
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reference examples documented in defgroup above] */

/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assess_note X.8 NOTE — Final Guidance
 * @ingroup cpisi_assess
 * @brief    Biblical grounding and final notes.
 *
 * "Examine yourselves, whether ye be in the faith; prove your own selves."
 * — 2 Corinthians 13:5 KJV
 *
 * The ASSESS phase is about self-examination — not just recording what happened
 * (that's DETECT's job), but actively evaluating, diagnosing, and understanding.
 * Like proving your own self, ASSESS reveals the true state of the system.
 *
 * DERIVATION CHAIN (single source of truth):
 *   types.h (B.9)     → DetectType, Severity
 *   types.h (B.1-B.6) → HebrewState, KFactor, HealthScore, MultiScore, TernaryScore
 *   types.h (B.7)     → HealthLevel, HealthThreshold
 *   types.h (S.2)     → Shared limits (CPISI_SEVERITY_COUNT, CPISI_HOTSPOT_COUNT)
 *   error.h (S.2e.e)  → CPISI_ERR_DIAG_* error codes (117-125)
 *
 * This file USES those types and error codes, it doesn't redefine them.
 *
 * RELATED HEADERS:
 *   - types.h   — Single source of truth for all types
 *   - error.h   — Error codes (HEALTH layer, DIAG tier)
 *   - detect.h  — DETECT phase (witnesses events)
 *   - restore.h — RESTORE phase (acts on assessment)
 *   - dar.h     — Group header that includes all DAR phases
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Final guidance documented in defgroup above] */

/** @} */ /* end X.8 NOTE */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * C++ Linkage Closure
 * ───────────────────────────────────────────────────────────────────────────── */
#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_CORE_CPISI_DAR_ASSESS_H */
