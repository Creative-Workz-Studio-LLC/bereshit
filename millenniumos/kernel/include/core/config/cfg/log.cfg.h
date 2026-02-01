/// @file log.h
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
/// @omni:req  key        = CPISI-TYPES-LOG
/// @omni:req  from       = templates/code/c/header.h
/// @omni:req  at         = a-01.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = domain
/// @omni:inh  role       = types
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = types/config/log
/// @omni:ins  includes   = config.h
/// @omni:ins  provides   = [S.2_LOG, LogLevel, ActionType]
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
/// @omni:req  title      = CPI-SI Log Domain
/// @omni:req  brief      = Debug levels and log severity names
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
///   - SP.1 CONFIG  — Compile-time settings (POSIX)
///
/// INHERITS (from types.h):
///   - SP.1b MODE   — Layer selection (defined in foundation)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * WHY: Settings that affect what headers provide must precede \#include.
 *
 * CONTAINS:
 *   - SP.1a POSIX  — Platform feature test (universal)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1a POSIX — Feature Test [POSIX]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_log_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_log_special_config
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

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef CPISI_TYPES_LOG_H
#define CPISI_TYPES_LOG_H

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Identity and context for this component.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      log.h
 * @brief     CPI-SI Log Domain — Debug levels, log names, logging types.
 *
 * @defgroup cpisi_log_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-LOG
 * Title:     CPI-SI Log Domain Types
 * Type:      Header (Domain)
 * Component: Rails — provides logging types for system diagnostics
 * Role:      Debug level, log name, and logging configuration types.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   b-03.00
 * @date      2026-01-25
 *
 * Status:    Active
 * Created:   2026-01-21
 * Updated:   2026-01-25
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authors and Copyright [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_log_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/log/log.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: types/types.h (foundation)
 * Template:     bereshit/word/seed/code/c/type-header.h
 * Extracted:    2026-01-21 from types.h log sections
 *
 * Builds FROM:  types.h, config/config.h
 * Builds TO:    Health monitoring, diagnostics, debugging systems
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, log, domain, debug, severity, diagnostics
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide logging types for system diagnostics and debugging.
 * Core Design: Severity-based log levels with configurable output.
 * Philosophy:  Logging makes system behavior visible and debuggable.
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Habakkuk 2:2 — "Write the vision, make it plain"
 * Principle:  Logging makes system behavior visible and debuggable.
 * Anchor:     Deuteronomy 31:19 — "write ye this song... that it may be a witness"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h, config/config.h
 *
 * What Uses This:
 *   - Health monitoring
 *   - Diagnostics
 *   - Debugging systems
 *
 * @see types.h for foundation types
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-21) — Extracted from types.h log sections
 *   a-01.25 (2026-01-23) — Expanded METADATA to full M.1-M.10 structure
 *   b-03.00 (2026-01-25) — Template alignment: type-header.h, CLOSING X.1-X.9
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ Log level types defined
 *
 * Planned:
 *   ⏳ Structured log output format
 *   ⏳ Log rotation integration
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
/// @brief Configuration and dependencies before body.
///
/// CONTAINS:
///   - S.1 INCLUDES  — Foundation headers (types.h)
///   - S.2 DEFINES   — Log domain (Bounds, Macros*, Verify*, Lookup, Types)
///
/// Note: (*) = reserved (inherited from types.h or not applicable)
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
 * @defgroup cpisi_log_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * PURPOSE: Establish all header dependencies in consistent order.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers (inherited from types.h)
 *   - S.1b INTERNAL  — Project headers (types.h foundation)
 *   - S.1c EXTERNAL  — Third-party headers (none)
 *   - S.1d CPP       — C++ linkage (inherited from types.h)
 *
 * Section order: Standard → Internal → External → CPP linkage
 * Flow: system foundation → our code → third-party → C++ compatibility
 *
 * Builds FROM: SETUP block (configuration context)
 * Builds TO:   S.2 DEFINES (domain-specific constants)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_log_includes_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_log_includes
 * @brief    Standard library includes (inherited from types.h).
 *
 * INHERITED: All system headers provided by types.h S.1a STANDARD.
 *            See types.h for MODE-conditional header table.
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
 * @defgroup cpisi_log_includes_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_log_includes
 * @brief    Internal CPI-SI headers this file depends on.
 *
 * PURPOSE: Include foundation header providing S.1 system headers and S.2 defines.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/config/cfg/domain.cfg.h"  /**< TIER 1: config anchors (brings types.h transitively) */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_log_includes_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_log_includes
 * @brief    External library headers (none required).
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
 * @defgroup cpisi_log_includes_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_log_includes
 * @brief    C++ compatibility (inherited from types.h).
 *
 * INHERITED: extern "C" wrapper opened in types.h S.1d.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Inherited from types.h S.1d — extern "C" already active] */

/** @} */ /* end S.1d CPP */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Log Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_domain S.2 DEFINES — Log Domain
 * @brief    Debug level constants for logging verbosity control.
 *
 * PURPOSE: Define log-specific constants, operations, verification, and types.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: no domain-specific preludes]
 *   - S.2a  BOUNDS  — Log domain constants
 *   - S.2b  MACROS  — [Reserved: inherited from types.h]
 *   - S.2c VERIFY  — [Reserved: inherited from types.h]
 *   - S.2d LOOKUP  — Log and debug level tables
 *   - S.2e TYPES   — Debug level enumeration
 *
 * Builds FROM: S.1 INCLUDES (foundation dependencies)
 * Builds TO:   S.3 CLOSING (file end)
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  LOG DOMAIN — 9 Debug Levels Mapped to Genesis Pyramid                     │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │                                                                             │
 * │  Debug levels follow the 4+1+4 pattern around VERBOSE center:               │
 * │                                                                             │
 * │  BELOW CENTER (0-3):  Kernel-side, less verbose                             │
 * │  CENTER (4):          VERBOSE — the balanced default                        │
 * │  ABOVE CENTER (5-8):  App-side, more verbose                                │
 * │                                                                             │
 * │  ┌───────────────────────────────────────────────────────────────────────┐  │
 * │  │ Level      │ Layer     │ Position │ Verbosity                        │  │
 * │  │────────────│───────────│──────────│────────────────────────────────  │  │
 * │  │ NONE       │ KERNEL    │ 0        │ No output                        │  │
 * │  │ MINIMAL    │ SYSTEM    │ 1        │ Errors only                      │  │
 * │  │ BASIC      │ RUNTIME   │ 2        │ + Warnings                       │  │
 * │  │ STANDARD   │ FRAMEWORK │ 3        │ + Info                           │  │
 * │  │ **VERBOSE**│ **HEALTH**│ **4**    │ **CENTER — Balanced Default**    │  │
 * │  │ DETAILED   │ NETWORK   │ 5        │ + Debug                          │  │
 * │  │ FULL       │ STORAGE   │ 6        │ + Trace                          │  │
 * │  │ TRACE      │ ENGINE    │ 7        │ + Fine                           │  │
 * │  │ EXHAUSTIVE │ APP       │ 8        │ Everything                       │  │
 * │  └───────────────────────────────────────────────────────────────────────┘  │
 * │                                                                             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 * @{
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2.0 PRELUDE — [Reserved] [PRELUDE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_log_prelude S.2.0 LOG PRELUDE — Domain Operations
 * @ingroup cpisi_log_domain
 * @brief    [Reserved: no domain-specific preludes]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Log domain uses foundation PRELUDE operations] */

/** @} */ /* end S.2.0 PRELUDE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BOUNDS — Log Domain Constants [BOUNDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_log_bounds S.2a LOG BOUNDS — Domain Constants
 * @ingroup cpisi_log_domain
 * @brief    Log level indices and debug level constants.
 *
 * CONTAINS:
 *   - Counts — LOG_COUNT (5), DEBUG_LEVEL_COUNT (9)
 *   - Debug Levels — 9 levels mapped to layers (0-8)
 *   - Debug Bounds — MIN/MAX/CENTER/DEFAULT from layer bounds
 *   - Log Levels — 5 severity levels (0-4)
 *   - Log Bounds — MIN/MAX/CENTER/DEFAULT
 *
 * Builds FROM: S.2.0 PRELUDE
 * Builds TO:   S.2b MACROS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Counts — Log domain array sizes
 *  @brief LOG_COUNT (5 severities), DEBUG_LEVEL_COUNT (9 verbosity levels).
 *  @{ */

#define CPISI_LOG_COUNT             CPISI_HARDPOINTS    /**< 5: log severity count       */
#define CPISI_DEBUG_LEVEL_COUNT     CPISI_LAYER_COUNT   /**< 9: debug verbosity count    */

/** @} */ /* end Counts */

/** @name Buffer Sizes — Health log entry field sizes
 *  @brief Field sizes for HealthLogEntry source and detail strings.
 *  @{ */

#define CPISI_SOURCE_MAX            64      /**< Max source string length (e.g., "test_pass")   */
#define CPISI_DETAIL_MAX            256     /**< Max detail string length (optional context)    */

/** @} */ /* end Buffer Sizes */

/** @name Debug Levels — 9 levels mapped to layers (0-8)
 *  @brief MIN=NONE(0), CENTER=VERBOSE(4), MAX=EXHAUSTIVE(8). Layer-driven.
 *  @{ */

#define CPISI_DEBUG_LEVEL_NONE       CPISI_LAYER_KERNEL      /**< L0: no output [MIN]         */
#define CPISI_DEBUG_LEVEL_MINIMAL    CPISI_LAYER_SYSTEM      /**< L1: minimal output          */
#define CPISI_DEBUG_LEVEL_BASIC      CPISI_LAYER_RUNTIME     /**< L2: basic output            */
#define CPISI_DEBUG_LEVEL_STANDARD   CPISI_LAYER_FRAMEWORK   /**< L3: standard output         */
#define CPISI_DEBUG_LEVEL_VERBOSE    CPISI_LAYER_HEALTH      /**< L4: verbose [CENTER]        */
#define CPISI_DEBUG_LEVEL_DETAILED   CPISI_LAYER_NETWORK     /**< L5: detailed output         */
#define CPISI_DEBUG_LEVEL_FULL       CPISI_LAYER_STORAGE     /**< L6: full output             */
#define CPISI_DEBUG_LEVEL_TRACE      CPISI_LAYER_ENGINE      /**< L7: trace output            */
#define CPISI_DEBUG_LEVEL_EXHAUSTIVE CPISI_LAYER_APP         /**< L8: exhaustive [MAX]        */

/** @} */ /* end Debug Levels */

/** @name Debug Bounds — MIN/MAX/CENTER/DEFAULT from layer bounds
 *  @brief Config-driven: *_OF(DEBUG_LEVEL_COUNT) pattern.
 *  @{ */

/**
 * @brief 0: minimum debug level — level at MIN_OF(DEBUG_LEVEL_COUNT).
 * @par Layer Mapping
 *      MIN_OF(9) = 0 → LAYER_KERNEL → DEBUG_LEVEL_NONE → DEBUG_LEVEL_MIN
 */
#define CPISI_DEBUG_LEVEL_MIN        CPISI_DEBUG_LEVEL_NONE

/**
 * @brief 8: maximum debug level — level at MAX_OF(DEBUG_LEVEL_COUNT).
 * @par Layer Mapping
 *      MAX_OF(9) = 8 → LAYER_APP → DEBUG_LEVEL_EXHAUSTIVE → DEBUG_LEVEL_MAX
 */
#define CPISI_DEBUG_LEVEL_MAX        CPISI_DEBUG_LEVEL_EXHAUSTIVE

/**
 * @brief 4: center debug level — level at CENTER_OF(DEBUG_LEVEL_COUNT).
 * @par Layer Mapping
 *      CENTER_OF(9) = 4 → LAYER_HEALTH → DEBUG_LEVEL_VERBOSE → DEBUG_LEVEL_CENTER
 */
#define CPISI_DEBUG_LEVEL_CENTER     CPISI_DEBUG_LEVEL_VERBOSE

/**
 * @brief 4: default debug level — aliases CENTER (yashar state).
 */
#define CPISI_DEBUG_LEVEL_DEFAULT    CPISI_DEBUG_LEVEL_CENTER

/** @} */ /* end Debug Bounds */

/** @} */ /* end S.2a BOUNDS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b MACROS — [Reserved] [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_log_macros S.2b LOG MACROS — Domain Operations
 * @ingroup cpisi_log_domain
 * @brief    [Reserved: Log domain inherits macros from types.h]
 *
 * Log level operations (clamping, validation) use generic CPISI_CLAMP and
 * CPISI_IN_RANGE from types.h S.2b MACROS. No domain-specific operations needed.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Log domain uses foundation MACROS operations] */

/** @} */ /* end S.2b MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c VERIFY — [Reserved] [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_log_verify S.2c LOG VERIFY — Domain Tripwires
 * @ingroup cpisi_log_domain
 * @brief    [Reserved: Log domain inherits verification from types.h]
 *
 * Log level constants (CPISI_LOG_LEVEL_COUNT) are simple derivations verified
 * in types.h S.2c VERIFY. Debug level bounds mirror log levels with no additional
 * tripwires required.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Log domain uses foundation VERIFY operations] */

/** @} */ /* end S.2c VERIFY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d LOOKUP — Runtime lookup tables [LOOKUP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_log_lookup S.2d LOOKUP — Runtime lookup tables
 * @ingroup cpisi_log_domain
 * @brief Pre-computed tables for O(1) log/debug level operations.
 *
 * PURPOSE: Container for log domain lookup tables.
 *
 * STRUCTURE (mirrors types.h S.2d pattern: Domain → a/b/c):
 *   S.2d.3 LOG — Log domain tables
 *     S.2d.3a TABLES     — 2 arrays (LOG_NAMES[5], DEBUG_LEVEL_NAMES[9])
 *     S.2d.3b ACCESSORS  — 2 macros (cpisi_log_name, cpisi_debug_level_name)
 *     S.2d.3c CONVERSION — [Reserved: log levels use direct lookup]
 *
 * Builds FROM: S.2c LOG VERIFY (structure verified)
 * Builds TO:   S.2e LOG TYPES (enum definitions)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup cpisi_log_tables S.2d.3 LOG — Log domain lookup
 * @ingroup cpisi_log_lookup
 * @brief Runtime lookup tables for log severity and debug verbosity.
 *
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  LOG DOMAIN LOOKUP — Two naming systems                                   ║
 * ║  ─────────────────────────────────────────────────────────────────────────║
 * ║  5-LEVEL LOG SEVERITY: none/error/warn/info/debug                         ║
 * ║  9-LEVEL DEBUG VERBOSITY: none...exhaustive (mirrors layer architecture)  ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 *
 * TWO PARALLEL SYSTEMS:
 *   - Log severity (5 levels): Filters which messages appear
 *   - Debug verbosity (9 levels): Controls detail level (mirrors 9-layer pyramid)
 *
 * STRUCTURE (template pattern: a=TABLES, b=ACCESSORS, c=CONVERSION):
 *   S.2d.3a TABLES     — 2 arrays: LOG_NAMES[5], DEBUG_LEVEL_NAMES[9]
 *   S.2d.3b ACCESSORS  — 2 macros: cpisi_log_name(), cpisi_debug_level_name()
 *   S.2d.3c CONVERSION — [Reserved: log levels not converted, only looked up]
 *
 * @ingroup S2d_LOOKUP
 * @{ */

/** @name S.2d.3a TABLES — Log lookup arrays
 *  @brief Two arrays for log/debug level name lookup.
 *
 *  LOG SEVERITY ARRAY (5 entries, indexed 0-4):
 *    CPISI_LOG_NAMES[5] — "none", "error", "warn", "info", "debug"
 *
 *  DEBUG VERBOSITY ARRAY (9 entries, indexed 0-8):
 *    CPISI_DEBUG_LEVEL_NAMES[9] — "none"..."exhaustive" (mirrors layer pyramid)
 *
 *  @{ */

/**
 * @brief Log level names — maps severity (0-4) to name string.
 *
 * BOUNDS USED:
 *   - CPISI_LOG_COUNT = 5 — array size
 *
 * 5-LEVEL LOG SEVERITY (each level includes more severe levels):
 *   [0] "none"  — logging disabled
 *   [1] "error" — errors only
 *   [2] "warn"  — warnings and above
 *   [3] "info"  — informational and above
 *   [4] "debug" — all messages
 *
 * @par Semantic Meaning
 *      Log severity controls which messages appear in output. Higher levels
 *      include all lower (more severe) levels. Most applications use info (3)
 *      for normal operation, debug (4) for troubleshooting.
 *
 * @see cpisi_log_name — accessor macro for this table
 */
static const char* const CPISI_LOG_NAMES[CPISI_LOG_COUNT] = {
    "none",   /* [0] disabled  */
    "error",  /* [1] errors    */
    "warn",   /* [2] warnings  */
    "info",   /* [3] info      */
    "debug"   /* [4] all       */
};

/** @brief Backward compatibility alias for CPISI_LOG_NAMES.
 *  @deprecated Use CPISI_LOG_NAMES directly. */
#define CPISI_DEBUG_NAMES CPISI_LOG_NAMES

/**
 * @brief Debug level names — maps verbosity (0-8) to name string.
 *
 * BOUNDS USED:
 *   - CPISI_DEBUG_LEVEL_COUNT = 9 — array size (mirrors LAYER_COUNT)
 *
 * 9-LEVEL DEBUG VERBOSITY (mirrors 9-layer pyramid):
 *   [0] "none"       — Silent, no output
 *   [1] "minimal"    — Critical only
 *   [2] "basic"      — Basic messages
 *   [3] "standard"   — Standard output
 *   [4] "verbose"    — CENTER (default for detailed work)
 *   [5] "detailed"   — Detailed output
 *   [6] "full"       — Full output
 *   [7] "trace"      — Trace output
 *   [8] "exhaustive" — Maximum verbosity
 *
 * @par Semantic Meaning
 *      Debug verbosity controls how much detail appears. Mirrors the 9-layer
 *      pyramid: verbose (4) at CENTER provides balanced default. Foundation
 *      levels (0-3) show less; upper levels (5-8) show more.
 *
 * @see cpisi_debug_level_name — accessor macro for this table
 */
static const char* const CPISI_DEBUG_LEVEL_NAMES[CPISI_DEBUG_LEVEL_COUNT] = {
    "none",       /* [0] silent     */
    "minimal",    /* [1] critical   */
    "basic",      /* [2] basic      */
    "standard",   /* [3] standard   */
    "verbose",    /* [4] CENTER     */
    "detailed",   /* [5] detailed   */
    "full",       /* [6] full       */
    "trace",      /* [7] trace      */
    "exhaustive"  /* [8] maximum    */
};

/** @} */ /* end S.2d.3a TABLES */

/** @name S.2d.3b ACCESSORS — Log table access macros
 *  @brief Type-safe access macros for log lookup tables.
 *
 *  PATTERN: Each accessor provides direct array access without bounds check.
 *           Caller must ensure valid index range.
 *
 *  ACCESSORS (2 total):
 *    cpisi_log_name(l)         — Log severity name (index 0-4)
 *    cpisi_debug_level_name(l) — Debug verbosity name (index 0-8)
 *
 *  @{ */

/**
 * @brief Get log level name — LEVEL (0-4) → NAME string.
 *
 * BOUNDS:
 *   - INPUT:  l ∈ [0, CPISI_LOG_COUNT-1] = [0, 4]
 *   - OUTPUT: CPISI_LOG_NAMES[l] ∈ {"none", "error", "warn", "info", "debug"}
 *
 * @par Derivation
 *      Direct index into CPISI_LOG_NAMES[l].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ l ≤ 4.
 *
 * @par Semantic Meaning
 *      Human-readable log severity name for configuration, status display,
 *      and log output headers.
 *
 * EXAMPLE:
 *   cpisi_log_name(0) → "none"  (logging disabled)
 *   cpisi_log_name(2) → "warn"  (warnings and above)
 *   cpisi_log_name(4) → "debug" (all messages)
 *
 * @param l Level index 0-4 (unchecked)
 *
 * @see CPISI_LOG_NAMES — source table
 */
#define cpisi_log_name(l)   (CPISI_LOG_NAMES[(l)])

/**
 * @brief Get debug level name — LEVEL (0-8) → NAME string.
 *
 * BOUNDS:
 *   - INPUT:  l ∈ [0, CPISI_DEBUG_LEVEL_COUNT-1] = [0, 8]
 *   - OUTPUT: CPISI_DEBUG_LEVEL_NAMES[l] ∈ {"none"..."exhaustive"}
 *
 * @par Derivation
 *      Direct index into CPISI_DEBUG_LEVEL_NAMES[l].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ l ≤ 8.
 *
 * @par Semantic Meaning
 *      Human-readable debug verbosity name. Mirrors 9-layer pyramid structure.
 *      verbose (4) at CENTER is the balanced default for detailed work.
 *
 * EXAMPLE:
 *   cpisi_debug_level_name(0) → "none"      (silent)
 *   cpisi_debug_level_name(4) → "verbose"   (CENTER, default)
 *   cpisi_debug_level_name(8) → "exhaustive" (maximum)
 *
 * @param l Level index 0-8 (unchecked)
 *
 * @see CPISI_DEBUG_LEVEL_NAMES — source table
 */
#define cpisi_debug_level_name(l)   (CPISI_DEBUG_LEVEL_NAMES[(l)])

/** @} */ /* end S.2d.3b ACCESSORS */

/** @name S.2d.3c CONVERSION — Log level conversion utilities
 *  @brief [Reserved: Log levels are looked up, not converted between formats]
 *
 *  Log levels use direct table lookup (S.2d.3a) via accessor macros (S.2d.3b).
 *  No conversion utilities needed — levels map directly to indices.
 *
 *  FUTURE: If string→level parsing needed, add here.
 *  @{ */

/* No conversion utilities — log levels use direct lookup */

/** @} */ /* end S.2d.3c CONVERSION */

/** @} */ /* end S.2d.3 LOG */

/** @} */ /* end S.2d LOOKUP */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e TYPES — Log domain type definitions [TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_log_types S.2e LOG TYPES — Type Definitions
 * @ingroup cpisi_log_domain
 * @brief Log domain type-safe enumerations.
 *
 * PURPOSE: Type definitions for log domain.
 *
 * CONTAINS:
 *   S.2e.1 LOG_LEVEL   — CpisiLogLevel (5 values) — severity filtering
 *   S.2e.2 DEBUG_LEVEL — CpisiDebugLevel (9 values) — verbosity depth
 *
 * Builds FROM: S.2d LOOKUP (runtime tables)
 * Builds TO:   S.3 CLOSING (file end)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2e_1_LOG_LEVEL S.2e.1 LOG_LEVEL — Message severity filtering
 * @ingroup cpisi_log_types
 * @brief 5-level log severity: NONE(0) → DEBUG(4), WARN(2) at CENTER.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  CpisiLogLevel — 5-Level Severity (WHAT to show)                            │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  Enum Value     │ Numeric │ Description                                     │
 * │─────────────────│─────────│─────────────────────────────────────────────────│
 * │  LOG_NONE       │ 0       │ Logging disabled (silent)                       │
 * │  LOG_ERROR      │ 1       │ Errors only (failures)                          │
 * │  LOG_WARN       │ 2       │ Warnings and above (CENTER)                     │
 * │  LOG_INFO       │ 3       │ Informational (normal status)                   │
 * │  LOG_DEBUG      │ 4       │ All messages (development)                      │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * RELATIONSHIP TO DEBUG LEVEL:
 *   LogLevel = WHAT to show (severity filtering)
 *   DebugLevel = HOW MUCH detail (verbosity depth)
 *
 * @{
 */

/**
 * @brief Log level enumeration — message severity filtering.
 *
 * BOUNDS USED:
 *   - CPISI_LOG_LEVEL_COUNT (5) — array size
 *   - CPISI_LOG_LEVEL_CENTER (2) — center = WARN
 */
typedef enum CpisiLogLevel {
    CPISI_LOG_NONE  = 0,  /**< 0: Logging disabled — silent operation */
    CPISI_LOG_ERROR = 1,  /**< 1: Errors only — failures requiring attention */
    CPISI_LOG_WARN  = 2,  /**< 2: Warnings and above (CENTER) — potential issues */
    CPISI_LOG_INFO  = 3,  /**< 3: Informational — normal operational status */
    CPISI_LOG_DEBUG = 4   /**< 4: All messages — detailed debugging output */
} CpisiLogLevel;

/** @} */ /* end S.2e.1 LOG_LEVEL */

/**
 * @defgroup S2e_2_DEBUG_LEVEL S.2e.2 DEBUG_LEVEL — Debug verbosity enumeration
 * @ingroup cpisi_log_types
 * @brief Type-safe enumeration for 9-level debug verbosity control.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  CpisiDebugLevel — 9-Level Verbosity (mirrors 9-layer pyramid)             │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  Enum Value         │ Numeric │ Description                                │
 * │─────────────────────│─────────│────────────────────────────────────────────│
 * │  DEBUG_ENUM_NONE    │ 0       │ Silent, no output                          │
 * │  DEBUG_ENUM_MINIMAL │ 1       │ Critical only                              │
 * │  DEBUG_ENUM_BASIC   │ 2       │ Basic messages                             │
 * │  DEBUG_ENUM_STANDARD│ 3       │ Standard output                            │
 * │  DEBUG_ENUM_VERBOSE │ 4       │ Verbose (CENTER, default)                  │
 * │  DEBUG_ENUM_DETAILED│ 5       │ Detailed output                            │
 * │  DEBUG_ENUM_FULL    │ 6       │ Full output                                │
 * │  DEBUG_ENUM_TRACE   │ 7       │ Trace output                               │
 * │  DEBUG_ENUM_EXHAUSTIVE│ 8     │ All layers, maximum verbosity              │
 * │  DEBUG_ENUM_COUNT   │ 9       │ Total count (iteration bound)              │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * RELATIONSHIP TO LOOKUP (S.2d):
 *   - cpisi_debug_level_name(idx) → "none", "minimal", ..., "exhaustive"
 * @{
 */

/**
 * @brief Debug level enumeration — 9-level verbosity control.
 *
 * BOUNDS USED:
 *   - CPISI_DEBUG_LEVEL_NONE (0) through CPISI_DEBUG_LEVEL_EXHAUSTIVE (8)
 *   - CPISI_LAYER_COUNT (9) for iteration bound
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for debug verbosity levels. VERBOSE (4) at center provides
 *   balanced default. Lower values = less output, higher values = more output.
 */
typedef enum CpisiDebugLevel {
    CPISI_DEBUG_ENUM_NONE       = CPISI_DEBUG_LEVEL_NONE,       /**< 0: Silent — no debug output */
    CPISI_DEBUG_ENUM_MINIMAL    = CPISI_DEBUG_LEVEL_MINIMAL,    /**< 1: Minimal — critical only */
    CPISI_DEBUG_ENUM_BASIC      = CPISI_DEBUG_LEVEL_BASIC,      /**< 2: Basic — essential messages */
    CPISI_DEBUG_ENUM_STANDARD   = CPISI_DEBUG_LEVEL_STANDARD,   /**< 3: Standard — normal verbosity */
    CPISI_DEBUG_ENUM_VERBOSE    = CPISI_DEBUG_LEVEL_VERBOSE,    /**< 4: Verbose (CENTER) — balanced default */
    CPISI_DEBUG_ENUM_DETAILED   = CPISI_DEBUG_LEVEL_DETAILED,   /**< 5: Detailed — expanded output */
    CPISI_DEBUG_ENUM_FULL       = CPISI_DEBUG_LEVEL_FULL,       /**< 6: Full — comprehensive logging */
    CPISI_DEBUG_ENUM_TRACE      = CPISI_DEBUG_LEVEL_TRACE,      /**< 7: Trace — execution path tracking */
    CPISI_DEBUG_ENUM_EXHAUSTIVE = CPISI_DEBUG_LEVEL_EXHAUSTIVE, /**< 8: Exhaustive — maximum verbosity */
    CPISI_DEBUG_ENUM_COUNT      = CPISI_LAYER_COUNT             /**< 9: Count — iteration bound (not a level) */
} CpisiDebugLevel;

/** @} */ /* end S.2e.2 DEBUG_LEVEL */

/** @} */ /* end S.2e TYPES */

/** @} */ /* End cpisi_log_domain */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_types S.3 LOG TYPES — Type Declarations
 * @ingroup cpisi_log_domain
 * @brief    Reserved for additional type declarations beyond S.2e.
 *
 * PURPOSE: Type declarations (enums, structs, opaque types).
 *          Currently reserved — log types defined in S.2e.
 *
 * Builds FROM: S.2e TYPES (enum definitions)
 * Builds TO:   S.4 PROTOTYPES
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: Log types defined in S.2e] */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_prototypes S.4 LOG PROTOTYPES — Function Declarations
 * @ingroup cpisi_log_domain
 * @brief    Reserved for function prototypes.
 *
 * PURPOSE: Function declarations (lifecycle, accessors, operations).
 *          Currently reserved — log accessors defined inline in S.2d.
 *
 * Builds FROM: S.3 TYPES
 * Builds TO:   S.5 FILE-LEVEL
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: Log accessors defined inline in S.2d] */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File Scope Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_file S.5 LOG FILE-LEVEL — File Scope Declarations
 * @ingroup cpisi_log_domain
 * @brief    Reserved for file-level declarations.
 *
 * PURPOSE: Extern/static variables, callback types, infrastructure.
 *          Currently reserved — log domain has no file-level state.
 *
 * Builds FROM: S.4 PROTOTYPES
 * Builds TO:   S.6 OMISSION
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: Log domain has no file-level state] */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_omission S.6 LOG OMISSION — Reserved Section Guide
 * @ingroup cpisi_log_domain
 * @brief    Documents what sections are reserved and why.
 *
 * PURPOSE: Explicit documentation of intentionally empty sections.
 *
 * RESERVED SECTIONS:
 *   S.3 TYPES          — Log types defined in S.2e
 *   S.4 PROTOTYPES     — Log accessors defined inline in S.2d
 *   S.5 FILE-LEVEL     — Log domain has no mutable state
 *
 * Builds FROM: S.5 FILE-LEVEL
 * Builds TO:   END SETUP
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
/// @brief The actual work — LogLevel and ActionType type definitions.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 LogLevel   — 9-level Base9 cognitive state witness (9 values)
///   - B.2 ActionType — Event action classification (5 values)
///   - B.3 Validation — Log type validation functions
///   - B.4 Accessors  — Type accessor functions
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 LogLevel (9) — Base9 Cognitive State Witness [LOGLEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_level B.1 LogLevel — Cognitive State Witness
 * @ingroup cpisi_types_config_log
 * @brief    9-level Base9 (3²) cognitive state witness with unified lookup.
 *
 * WHY: 9-level logging system mapped to the Hebrew state spectrum.
 *      This is NOT a traditional severity system — it's a cognitive state witness.
 *      LogLevel index = CpisiLayer index (both 0-8).
 *
 * SCOPE: Logging operations throughout the CPI-SI system.
 *
 * ┌──────────┬───────┬─────────┬──────────┬────────────────────────────────────┐
 * │ Level    │ Index │ Ternary │ Hebrew   │ Description                        │
 * ├──────────┼───────┼─────────┼──────────┼────────────────────────────────────┤
 * │ CRISIS   │   0   │  -1.00  │ shavar   │ System-wide failure, unrecoverable │
 * │ FATAL    │   1   │  -0.75  │ trans    │ Critical, must stop soon           │
 * │ ERROR    │   2   │  -0.50  │ chaser   │ Broken, needs attention            │
 * │ WARN     │   3   │  -0.25  │ trans    │ Concern, potentially degraded      │
 * │ INFO     │   4   │   0.00  │ yashar   │ Normal operation [DEFAULT]         │
 * │ NOTICE   │   5   │  +0.25  │ trans    │ Notable, worth recording           │
 * │ DEBUG    │   6   │  +0.50  │ shalem   │ Building understanding             │
 * │ TRACE    │   7   │  +0.75  │ trans    │ Detailed flow                      │
 * │ INSIGHT  │   8   │  +1.00  │ tov      │ Deep insight, CPI-SI breakthrough  │
 * └──────────┴───────┴─────────┴──────────┴────────────────────────────────────┘
 *
 * Config source: bereshit/word/core/os/health/log.toml
 *
 * > "And the books were opened" — Revelation 20:12
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/**
 * @brief   9-level logging system mapped to Hebrew state spectrum
 * @note    Default: LOG_INFO (yashar, center)
 */
typedef enum LogLevel {
    LOG_CRISIS  = 0,    /**< shavar — ternary: -1.00 (system failure) */
    LOG_FATAL   = 1,    /**< shavar→chaser — ternary: -0.75 (critical) */
    LOG_ERROR   = 2,    /**< chaser — ternary: -0.50 (broken) */
    LOG_WARN    = 3,    /**< chaser→yashar — ternary: -0.25 (degraded) */
    LOG_INFO    = 4,    /**< yashar — ternary: 0.00 (normal) [DEFAULT] */
    LOG_NOTICE  = 5,    /**< yashar→shalem — ternary: +0.25 (notable) */
    LOG_DEBUG   = 6,    /**< shalem — ternary: +0.50 (understanding) */
    LOG_TRACE   = 7,    /**< shalem→tov — ternary: +0.75 (detailed) */
    LOG_INSIGHT = 8,    /**< tov — ternary: +1.00 (full insight) */
} LogLevel;

/** @brief Total log levels (9 = 3², Base9) */
#define LOG_LEVEL_COUNT     9
#define LOG_LEVEL_MIN       LOG_CRISIS
#define LOG_LEVEL_MAX       LOG_INSIGHT
#define LOG_LEVEL_DEFAULT   LOG_INFO
#define LOG_LEVEL_CENTER    LOG_INFO

/**
 * @brief   Unified lookup table for LogLevel
 * @note    Contains all level metadata in ONE struct
 *          9 LogLevels map to 9 CpisiLayers (KERNEL→APP)
 */
typedef struct LogLevelInfo {
    float       ternary;    /**< Ternary value (-1.0 to +1.0) */
    CpisiLayer  layer;      /**< Corresponding 9-layer mode */
    HebrewState hebrew;     /**< Corresponding Hebrew state */
    const char* name;       /**< "CRISIS", "INFO", etc. */
    const char* padded;     /**< Padded name for alignment (7 chars) */
    const char* emoji;      /**< Visual symbol */
} LogLevelInfo;

/**
 * @brief   MASTER lookup for LogLevel
 * @note    Access: LOG_LEVEL_INFO[level] or log_level_info(level)
 *          LogLevel index = CpisiLayer index (both 0-8)
 */
static const LogLevelInfo LOG_LEVEL_INFO[LOG_LEVEL_COUNT] = {
    [LOG_CRISIS]  = { -1.00f, (CpisiLayer)CPISI_LAYER_KERNEL,    (HebrewState)HEBREW_SHAVAR, "CRISIS",  "CRISIS ", "🔴" },
    [LOG_FATAL]   = { -0.75f, (CpisiLayer)CPISI_LAYER_SYSTEM,    (HebrewState)HEBREW_SHAVAR, "FATAL",   "FATAL  ", "💀" },
    [LOG_ERROR]   = { -0.50f, (CpisiLayer)CPISI_LAYER_RUNTIME,   (HebrewState)HEBREW_CHASER, "ERROR",   "ERROR  ", "❌" },
    [LOG_WARN]    = { -0.25f, (CpisiLayer)CPISI_LAYER_FRAMEWORK, (HebrewState)HEBREW_CHASER, "WARN",    "WARN   ", "⚠️" },
    [LOG_INFO]    = {  0.00f, (CpisiLayer)CPISI_LAYER_HEALTH,    (HebrewState)HEBREW_YASHAR, "INFO",    "INFO   ", "ℹ️" },
    [LOG_NOTICE]  = { +0.25f, (CpisiLayer)CPISI_LAYER_NETWORK,   (HebrewState)HEBREW_YASHAR, "NOTICE",  "NOTICE ", "📝" },
    [LOG_DEBUG]   = { +0.50f, (CpisiLayer)CPISI_LAYER_STORAGE,   (HebrewState)HEBREW_SHALEM, "DEBUG",   "DEBUG  ", "🔧" },
    [LOG_TRACE]   = { +0.75f, (CpisiLayer)CPISI_LAYER_ENGINE,    (HebrewState)HEBREW_SHALEM, "TRACE",   "TRACE  ", "🔍" },
    [LOG_INSIGHT] = { +1.00f, (CpisiLayer)CPISI_LAYER_APP,       (HebrewState)HEBREW_TOV,    "INSIGHT", "INSIGHT", "💡" },
};

/**
 * @brief   Get LogLevelInfo for a level
 * @param   level  LogLevel enum value
 * @return  Pointer to info struct (defaults to INFO if invalid)
 */
static inline const LogLevelInfo* log_level_info(LogLevel level) {
    if (level < 0 || level >= LOG_LEVEL_COUNT) level = LOG_LEVEL_DEFAULT;
    return &LOG_LEVEL_INFO[level];
}

/** @} */ /* end B.1 LogLevel */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 ActionType (5) — Event Action Classification [ACTIONTYPE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_action_type B.2 ActionType — Event Classification
 * @ingroup cpisi_types_config_log
 * @brief    5-type event action classification with unified lookup.
 *
 * WHY: What KIND of event occurred — success, failure, recovery, reset, neutral.
 *      Maps to atomic values (+1, 0, -1) for health impact calculation.
 *
 * SCOPE: Event classification throughout logging and health systems.
 *
 * ┌─────────────┬───────┬────────┬────────────────────────────────────────────┐
 * │ Action      │ Value │ Atomic │ Description                                │
 * ├─────────────┼───────┼────────┼────────────────────────────────────────────┤
 * │ NEUTRAL     │   0   │    0   │ Informational, no impact                   │
 * │ SUCCESS     │  +1   │   +1   │ Success operation                          │
 * │ RECOVERY    │  +2   │   +1   │ Recovery from error (success + marker)     │
 * │ FAILURE     │  -1   │   -1   │ Failure operation                          │
 * │ RESET       │   3   │    0   │ Reset operation (special, no health impact)│
 * └─────────────┴───────┴────────┴────────────────────────────────────────────┘
 *
 * > "A just man falleth seven times, and riseth up again" — Proverbs 24:16
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/**
 * @brief   Event action classification
 * @note    Maps to atomic values (+1, 0, -1) for health impact
 */
typedef enum ActionType {
    ACTION_NEUTRAL  =  0,   /**< Informational, no impact */
    ACTION_SUCCESS  = +1,   /**< Success (+1 atomic) */
    ACTION_RECOVERY = +2,   /**< Recovery from error (success + marker) */
    ACTION_FAILURE  = -1,   /**< Failure (-1 atomic) */
    ACTION_RESET    =  3,   /**< Reset operation (special, not health impact) */
} ActionType;

#define ACTION_TYPE_COUNT 5

/**
 * @brief   Unified lookup table for ActionType
 */
typedef struct ActionTypeInfo {
    int         atomic;     /**< Atomic value (ATOMIC_SUCCESS/NEUTRAL/FAILURE) */
    KFactor     kfactor;    /**< Direction (advance/maintain/retreat) */
    const char* name;       /**< "success", "failure", etc. */
    const char* emoji;      /**< Visual symbol */
} ActionTypeInfo;

/**
 * @brief   MASTER lookup for ActionType
 * @note    Index by: (action >= 0) ? action : (ACTION_TYPE_COUNT + action)
 */
static const ActionTypeInfo ACTION_TYPE_INFO[] = {
    [0] = { ATOMIC_NEUTRAL,  K_MAINTAIN, "neutral",  "•" },   // ACTION_NEUTRAL
    [1] = { ATOMIC_SUCCESS,  K_ADVANCE,  "success",  "✓" },   // ACTION_SUCCESS
    [2] = { ATOMIC_SUCCESS,  K_ADVANCE,  "recovery", "↑" },   // ACTION_RECOVERY
    [3] = { ATOMIC_NEUTRAL,  K_MAINTAIN, "reset",    "⟲" },   // ACTION_RESET
};

/**
 * @brief   Get ActionTypeInfo for an action
 * @param   action  ActionType enum value
 * @return  Pointer to info struct (defaults to NEUTRAL if invalid)
 * @note    ACTION_FAILURE (-1) handled specially
 */
static inline const ActionTypeInfo* action_type_info(ActionType action) {
    // Handle negative value (FAILURE)
    if (action == ACTION_FAILURE) {
        static const ActionTypeInfo failure_info = {
            ATOMIC_FAILURE, K_RETREAT, "failure", "✗"
        };
        return &failure_info;
    }
    if (action < 0 || action > ACTION_RESET) action = ACTION_NEUTRAL;
    return &ACTION_TYPE_INFO[action];
}

/** @} */ /* end B.2 ActionType */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 Validation — Log Type Validation [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_validation B.3 Validation — Log Type Validation
 * @ingroup cpisi_types_config_log
 * @brief    Validation functions for LogLevel types.
 *
 * Builds FROM: B.2 ActionType
 * Builds TO:   B.4 Accessors
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/** @brief Check if log level is valid */
static inline bool cpisi_log_level_valid(LogLevel level) {
    return (level >= LOG_LEVEL_MIN && level <= LOG_LEVEL_MAX);
}

/** @brief Clamp log level to valid range */
static inline LogLevel cpisi_log_level_clamp(int value) {
    return (LogLevel)CPISI_CLAMP(value, LOG_LEVEL_MIN, LOG_LEVEL_MAX);
}

/** @} */ // end cpisi_log_validation (B.3)

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 Accessors — Log Type Accessors [ACCESSORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_accessors B.4 Accessors — Log Type Accessors
 * @ingroup cpisi_types_config_log
 * @brief    Accessor functions for LogLevel and ActionType.
 *
 * Builds FROM: B.3 Validation
 * Builds TO:   B.5 Conversions
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

// ─────────────────────────────────────────────────────────────────────────────
// B.4a LogLevel Accessors
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Get log level name (config-driven via log_level_info()) */
static inline const char* cpisi_log_level_name(LogLevel level) {
    return log_level_info(level)->name;
}

/** @brief Get log level padded name for alignment (config-driven) */
static inline const char* cpisi_log_level_name_padded(LogLevel level) {
    return log_level_info(level)->padded;
}

/** @brief Get log level emoji (config-driven) */
static inline const char* cpisi_log_level_emoji(LogLevel level) {
    return log_level_info(level)->emoji;
}

/** @brief Parse string to LogLevel */
static inline LogLevel cpisi_log_level_parse(const char* name) {
    if (!name) return LOG_LEVEL_DEFAULT;
    for (int i = 0; i < LOG_LEVEL_COUNT; i++) {
        if (strncmp(name, LOG_LEVEL_INFO[i].name, 7) == 0) return (LogLevel)i;
    }
    // Try lowercase
    if (strncmp(name, "crisis", 6) == 0)  return LOG_CRISIS;
    if (strncmp(name, "fatal", 5) == 0)   return LOG_FATAL;
    if (strncmp(name, "error", 5) == 0)   return LOG_ERROR;
    if (strncmp(name, "warn", 4) == 0)    return LOG_WARN;
    if (strncmp(name, "info", 4) == 0)    return LOG_INFO;
    if (strncmp(name, "notice", 6) == 0)  return LOG_NOTICE;
    if (strncmp(name, "debug", 5) == 0)   return LOG_DEBUG;
    if (strncmp(name, "trace", 5) == 0)   return LOG_TRACE;
    if (strncmp(name, "insight", 7) == 0) return LOG_INSIGHT;
    return LOG_LEVEL_DEFAULT;
}

// ─────────────────────────────────────────────────────────────────────────────
// B.4b ActionType Accessors
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Get action type name (config-driven via action_type_info()) */
static inline const char* cpisi_action_type_name(ActionType action) {
    return action_type_info(action)->name;
}

/** @brief Get action type emoji (config-driven) */
static inline const char* cpisi_action_type_emoji(ActionType action) {
    return action_type_info(action)->emoji;
}

/** @} */ // end cpisi_log_accessors (B.4)

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 Conversions — Log Type Conversions [CONVERSIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_conversions B.5 Conversions — Log Type Conversions
 * @ingroup cpisi_types_config_log
 * @brief    Conversion functions for LogLevel and ActionType.
 *
 * Builds FROM: B.4 Accessors
 * Builds TO:   B.6 Reserved (END BODY)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

// ─────────────────────────────────────────────────────────────────────────────
// B.5a LogLevel Conversions
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief   Get ternary value for log level (config-driven)
 * @param   level  LogLevel enum value
 * @return  Ternary value from log_level_info()
 */
static inline float cpisi_log_level_ternary(LogLevel level) {
    return log_level_info(level)->ternary;
}

/**
 * @brief   Convert LogLevel to CpisiLayer (config-driven)
 * @param   level  LogLevel enum value
 * @return  CpisiLayer from log_level_info()
 * @note    LogLevel index = CpisiLayer index (both 0-8)
 */
static inline CpisiLayer cpisi_level_to_layer(LogLevel level) {
    return log_level_info(level)->layer;
}

/**
 * @brief   Convert CpisiLayer to LogLevel
 * @param   layer  CpisiLayer enum value
 * @return  Corresponding LogLevel (direct mapping)
 * @note    Both are 0-8, direct cast is valid
 */
static inline LogLevel cpisi_layer_to_level(CpisiLayer layer) {
    if (layer >= CPISI_LAYER_COUNT) layer = (CpisiLayer)CPISI_LAYER_HEALTH;
    return (LogLevel)layer;
}

/**
 * @brief   Convert LogLevel to HebrewState (config-driven)
 * @param   level  LogLevel enum value
 * @return  HebrewState from log_level_info()
 */
static inline HebrewState cpisi_level_to_hebrew(LogLevel level) {
    return log_level_info(level)->hebrew;
}

/**
 * @brief   Convert HebrewState to suggested LogLevel
 * @param   state  HebrewState enum value
 * @return  Center log level for that state's range
 */
static inline LogLevel cpisi_hebrew_to_level(HebrewState state) {
    switch (state) {
        case HEBREW_SHAVAR: return LOG_CRISIS;
        case HEBREW_CHASER: return LOG_ERROR;
        case HEBREW_RATSAH: return LOG_WARN;
        case HEBREW_YASHAR: return LOG_INFO;
        case HEBREW_TAMIM:  return LOG_NOTICE;
        case HEBREW_SHALEM: return LOG_DEBUG;
        case HEBREW_TOV:    return LOG_INSIGHT;
        default:            return LOG_INFO;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// B.5b ActionType Conversions
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief   Get atomic value for action type (config-driven)
 * @param   action  ActionType enum value
 * @return  Atomic value from action_type_info()
 */
static inline int cpisi_action_to_atomic(ActionType action) {
    return action_type_info(action)->atomic;
}

/**
 * @brief   Get k-factor direction for action type (config-driven)
 * @param   action  ActionType enum value
 * @return  KFactor from action_type_info()
 */
static inline KFactor cpisi_action_to_kfactor(ActionType action) {
    return action_type_info(action)->kfactor;
}

/** @} */ /* end B.5 Conversions */

/** B.6 RESERVED — [Reserved: inherits types.h B.6] @defgroup cpisi_log_b6_reserved @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS
 * ───────────────────────────────────────────────────────────────────────────── */

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Final assembly — X.1-X.9 sections (template-aligned).
///
/// CONTAINS:
///   - X.1 VALIDATION    — Build verification
///   - X.2 EXECUTION     — Usage patterns
///   - X.3 CLEANUP       — Ownership notes
///   - X.4 POLICY        — Modification guidance
///   - X.5 EXTENSION     — How to extend log domain
///   - X.6 TROUBLESHOOT  — Common problems
///   - X.7 REFERENCE     — Quick lookup
///   - X.8 NOTE          — Closing guidance
///   - X.9 OMISSION      — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build and self-containment verification.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 *   @code
 *   # Syntax check (from include/ directory):
 *   gcc -fsyntax-only -Wall -Wextra cpisi/types/config/log.h
 *   @endcode
 *
 * @par X.1b CHECKS — Verification Checklist
 *   - ✓ Compiles without warnings (-Wall -Wextra)
 *   - ✓ Self-contained (includes config.h which brings types.h)
 *   - ✓ LogLevel↔CpisiLayer 1:1 mapping verified
 *   - ✓ All lookup tables initialized with designated initializers
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_execution X.2 EXECUTION — Usage Patterns
 * @brief    How to use log types in code.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.2a INCLUDE — Include Directive
 *   @code
 *   #include "kernel/cpisi/types/config/log.h"
 *   @endcode
 *
 * @par X.2b USAGE — LogLevel Example
 *   @code
 *   LogLevel level = LOG_INFO;
 *   const LogLevelInfo* info = log_level_info(level);
 *   printf("Level: %s (ternary: %.2f)\n", info->name, info->ternary);
 *   @endcode
 *
 * @par X.2c USAGE — ActionType Example
 *   @code
 *   ActionType action = ACTION_SUCCESS;
 *   int atomic = cpisi_action_to_atomic(action);  // Returns ATOMIC_SUCCESS (+1)
 *   @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Memory and resource ownership.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.3a OWNERSHIP — No Dynamic Allocation
 *   Log types are enums and static lookup tables.
 *   No dynamic memory allocation. No cleanup required.
 *
 * @par X.3b STRINGS — Static String Pointers
 *   All name strings (from LogLevelInfo, ActionTypeInfo) are static.
 *   Do NOT free returned string pointers.
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_policy X.4 POLICY — Modification Guidance
 * @brief    Rules for modifying this file.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par DO NOT CHANGE
 *   - LogLevel count (9) — mirrors CpisiLayer
 *   - LOG_LEVEL_INFO table order — indexed by LogLevel enum
 *   - LogLevel↔CpisiLayer 1:1 relationship
 *
 * @par MAY ADD
 *   - New accessor functions (add to B.4)
 *   - New conversion functions (add to B.5)
 *   - Additional ActionType values (update table)
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Extend [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_extension X.5 EXTENSION — How to Extend
 * @brief    How to add to this log domain.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Adding Accessors
 *   - Add to B.4 ACCESSORS section
 *   - Follow pattern: static inline, return from info struct
 *
 * @par Adding Conversions
 *   - Add to B.5 CONVERSIONS section
 *   - Use log_level_info() or action_type_info() for values
 *
 * @par Log Levels Fixed
 *   Log levels are fixed at 9 (mirrors CpisiLayer 0-8).
 *   Adding levels would break Layer↔Level mapping.
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Problems [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par "Undefined HebrewState / CpisiLayer"
 *   Ensure config.h is included (brings types.h transitively).
 *
 * @par "Invalid level returned"
 *   LogLevel functions clamp/default to LOG_INFO for invalid input.
 *   Check input range: 0-8 for LogLevel, -1 to 3 for ActionType.
 *
 * @par "Wrong layer for level"
 *   LogLevel and CpisiLayer are 1:1 mapped (both 0-8).
 *   LOG_INFO(4) = CPISI_LAYER_HEALTH(4).
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick lookup — log level to ternary/layer/hebrew.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Level → Ternary → Hebrew
 *   0 CRISIS  → -1.00 → shavar
 *   1 FATAL   → -0.75 → shavar
 *   2 ERROR   → -0.50 → chaser
 *   3 WARN    → -0.25 → chaser
 *   4 INFO    →  0.00 → yashar (CENTER)
 *   5 NOTICE  → +0.25 → yashar
 *   6 DEBUG   → +0.50 → shalem
 *   7 TRACE   → +0.75 → shalem
 *   8 INSIGHT → +1.00 → tov
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * Scripture: Habakkuk 2:2, Revelation 20:12
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * Log domain provides cognitive state witness for CPI-SI.
 * 9 levels map to the Hebrew state spectrum, from shavar (broken)
 * through yashar (balanced) to tov (complete).
 *
 * "Write the vision, and make it plain upon tables, that he may
 *  run that readeth it." — Habakkuk 2:2
 *
 * "And the books were opened... and the dead were judged out of
 *  those things which were written in the books." — Revelation 20:12
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Reserved Sections [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_log_omission X.9 OMISSION — Reserved Sections
 * @brief    Reserved section documentation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par SETUP Reserved Sections
 *   - S.2.0 PRELUDE — Log uses foundation prelude operations
 *   - S.2b MACROS — Log uses foundation macros (CPISI_CLAMP, etc.)
 *   - S.2c VERIFY — Log bounds verified in types.h
 *   - S.3-S.5 — Types/Prototypes/File-Level defined inline
 *
 * @par BODY Reserved Sections
 *   - B.6 — Inherits types.h B.6
 *
 * @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_TYPES_LOG_H */
