/// @file config.h
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
/// @omni:req  key        = CPISI-TYPES-CONFIG
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
/// @omni:ins  layer      = types/config
/// @omni:ins  includes   = types.h
/// @omni:ins  provides   = [S.2_CONFIG]
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
/// @omni:req  title      = CPI-SI Config Domain
/// @omni:req  brief      = Timing, rotation, retention, buffer configuration
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
 * @defgroup cpisi_config_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup cpisi_config_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_config_special_config
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

#ifndef CPISI_TYPES_CONFIG_H
/** @def CPISI_TYPES_CONFIG_H
 *  @brief Include guard — prevents multiple inclusion of this config types header.
 */
#define CPISI_TYPES_CONFIG_H

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
 * @file      config.h
 * @brief     CPI-SI Config Domain — Timing, rotation, retention, buffer types.
 *
 * @defgroup cpisi_config_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-CONFIG
 * Title:     CPI-SI Config Domain Types
 * Type:      Header (Domain)
 * Component: Rails — provides config types for health and logging systems
 * Role:      Configuration structure types for timing, rotation, retention.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_state M.2 STATE — Lifecycle State
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
 * @defgroup cpisi_config_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_config_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/config/config.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Template:     bereshit/word/seed/code/c/type-header.h
 * Derives_from: types/types.h (foundation)
 * Extracted:    2026-01-21 from types.h config sections
 *
 * Builds FROM:  types.h (foundation types)
 * Builds TO:    error.h, log.h (use config types)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, config, domain, timing, rotation, retention, buffer
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide configuration structure types for CPI-SI subsystems.
 * Core Design: Nested config structs with sensible defaults.
 * Philosophy:  Configuration as data, not code.
 *
 * Key Features:
 *   - TimingConfig — intervals, timeouts, scheduling
 *   - RotationConfig — file rotation policies
 *   - RetentionConfig — data retention policies
 *   - BufferConfig — buffer sizing and thresholds
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Proverbs 24:27 — "Prepare thy work without"
 * Principle:  Configuration prepares systems before they run.
 * Anchor:     Luke 14:28 — "count the cost" — plan before building
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h (foundation types)
 *
 * What Uses This:
 *   - health/health.h — HealthConfig embeds these
 *   - log/log.h — LogConfig embeds these
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
 * @defgroup cpisi_config_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-21) — Extracted from types.h
 *   a-01.25 (2026-01-23) — PRAGMA/SPECIAL/METADATA M.1-M.10 alignment
 *   b-03.00 (2026-01-25) — type-header.h template alignment, full docstrings,
 *                          reserved section collapse, CLOSING X.1-X.9
 *
 * Current:
 *   ✓ 4-block structure complete (METADATA → SETUP → BODY → CLOSING)
 *   ✓ Full docstrings on all active code
 *   ✓ Reserved sections collapsed (template is authoritative)
 *   ✓ X.1-X.9 CLOSING guidance
 *
 * Planned:
 *   ⏳ Add validation functions for config values
 *   ⏳ Add default config initializers
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
///   - S.2 DEFINES   — Config domain (Bounds, Macros*, Verify*, Lookup*, Types*)
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
 * @defgroup cpisi_config_includes S.1 INCLUDES — Header Dependencies
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
 * @defgroup cpisi_config_includes_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_config_includes
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
 * @defgroup cpisi_config_includes_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_config_includes
 * @brief    Internal CPI-SI headers this file depends on.
 *
 * PURPOSE: Include foundation header providing S.1 system headers and S.2 defines.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/types.h"  /**< Foundation: S.1 system headers + S.2 universal defines */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_includes_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_config_includes
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
 * @defgroup cpisi_config_includes_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_config_includes
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
 * S.2 DEFINES — Config Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_domain S.2 DEFINES — Config Domain
 * @brief    Configuration constants for runtime behavior.
 *
 * PURPOSE: Define config-specific constants, operations, verification, and types.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: no domain-specific preludes]
 *   - S.2a  BOUNDS  — Configuration constants (timing, rotation, retention, buffers)
 *   - S.2b  MACROS  — [Reserved: inherited from types.h]
 *   - S.2c VERIFY  — [Reserved: inherited from types.h]
 *   - S.2d LOOKUP  — [Reserved: inherited from types.h]
 *   - S.2e TYPES   — [Reserved: inherited from types.h]
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  CONFIG DOMAIN — Runtime Configuration Values                              │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │                                                                             │
 * │  TIMING:     Poll intervals, retry counts                                   │
 * │  ROTATION:   Log file rotation thresholds                                   │
 * │  RETENTION:  Data retention periods                                         │
 * │  BUFFERS:    Semantic buffer size mappings                                  │
 * │  HISTORY:    DAR and Engram buffer sizes                                    │
 * │                                                                             │
 * │  Sources: log.toml, restore.toml, diagnostics.toml                          │
 * │                                                                             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * Builds FROM: S.1 INCLUDES (foundation dependencies)
 * Builds TO:   S.3 CLOSING (file end)
 *
 * @{
 */

/** S.2.0 PRELUDE — [Reserved] @defgroup cpisi_config_prelude @ingroup cpisi_config_domain @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BOUNDS — Configuration Constants [BOUNDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_bounds S.2a CONFIG BOUNDS — Domain Constants
 * @ingroup cpisi_config_domain
 * @brief    Configuration constants for runtime behavior.
 *
 * CONTAINS:
 *   - Counts — INTERVAL_COUNT (5)
 *   - Timing — Poll interval, retry bounds
 *   - Rotation — Log file thresholds
 *   - Retention — Data retention periods
 *   - Buffers — Semantic buffer sizes
 *
 * Builds FROM: S.2.0 PRELUDE
 * Builds TO:   S.2b MACROS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @name Counts
 * @brief Config domain array sizes.
 *
 * INTERVAL_COUNT (5) — timing interval array dimension.
 * @{
 */

/**
 * @brief 5: timing interval array size — 5 hardpoints for time-based operations.
 *
 * @return 5 (integer constant)
 *
 * @par Derivation
 *      INTERVAL_COUNT = HARDPOINTS = 5
 *      Five intervals: immediate, short, medium, long, extended
 *
 * @par Usage
 *      Size for timing interval arrays throughout CPI-SI.
 */
#define CPISI_INTERVAL_COUNT        CPISI_HARDPOINTS

/** @} */ /* end Counts */

/**
 * @name Timing
 * @brief Interval anchors and retry bounds.
 *
 * - POLL_INTERVAL_MS (1000) — THE timing anchor
 * - RETRY_MIN/MAX/DEFAULT (1/5/3) — retry bounds
 * @{
 */

/**
 * @brief 1000: poll interval in milliseconds — THE timing anchor.
 *
 * @return 1000 (integer constant, milliseconds)
 *
 * @par Derivation
 *      1000ms = 1 second — human-perceptible response time
 *
 * @par Usage
 *      Base polling interval for health checks, status updates.
 *      Other intervals derive from this anchor.
 */
#define CPISI_POLL_INTERVAL_MS      1000
/**
 * @brief Minimum retry count — one attempt minimum.
 *
 * @return 1 (integer constant)
 *
 * @par Derivation
 *      RETRY_MIN = UNITY = 1
 *
 * @par Usage
 *      Floor for retry configuration. At least one attempt always made.
 */
#define CPISI_RETRY_MIN             CPISI_UNITY

/**
 * @brief Maximum retry count — five attempts maximum.
 *
 * @return 5 (integer constant)
 *
 * @par Derivation
 *      RETRY_MAX = HARDPOINTS = 5
 *
 * @par Usage
 *      Ceiling for retry configuration. Prevents infinite retry loops.
 */
#define CPISI_RETRY_MAX             CPISI_HARDPOINTS

/**
 * @brief Default retry count — Ecclesiastes 4:12 cord of three.
 *
 * @return 3 (integer constant)
 *
 * @par Derivation
 *      RETRY_DEFAULT = TERNARY = 3
 *
 * @par Usage
 *      Balanced retry count. "A threefold cord is not quickly broken."
 */
#define CPISI_RETRY_DEFAULT         CPISI_TERNARY
/** @} */ /* end Timing */


/**
 * @name Rotation
 * @brief File rotation thresholds.
 *
 * ROTATION_BASE_KB (1024) — 1MB base, tier multipliers scale up.
 * @{
 */

/**
 * @brief 1024: rotation base size in kilobytes — 1MB rotation anchor.
 *
 * @return 1024 (integer constant, kilobytes)
 *
 * @par Derivation
 *      1024KB = 1MB — standard file size unit
 *
 * @par Usage
 *      Base size for log file rotation. Tier multipliers scale:
 *      TIER_0 = 1MB, TIER_1 = 3MB, TIER_2 = 9MB, etc.
 */
#define CPISI_ROTATION_BASE_KB      1024

/** @} */ /* end Rotation */

/**
 * @name Retention
 * @brief Data retention periods.
 *
 * - RETENTION_RECENT (90) — 3 months active window
 * - RETENTION_ARCHIVE (180) — 6 months cold storage
 * @{
 */

/**
 * @brief 90: recent retention period in days — three months.
 *
 * @return 90 (integer constant, days)
 *
 * @par Derivation
 *      90 days ≈ 3 months — standard short-term retention
 *
 * @par Usage
 *      Active data retention window. Data older than 90 days
 *      moves to archive tier.
 */
#define CPISI_RETENTION_RECENT      90

/**
 * @brief 180: archive retention period in days — six months.
 *
 * @return 180 (integer constant, days)
 *
 * @par Derivation
 *      ARCHIVE = RECENT × BINARY = 90 × 2 = 180 days
 *
 * @par Usage
 *      Long-term archive retention. Data older than 180 days
 *      may be purged or moved to cold storage.
 */
#define CPISI_RETENTION_ARCHIVE     CPISI_PRODUCT(CPISI_RETENTION_RECENT, CPISI_BINARY)

/** @} */ /* end Retention */


/**
 * @name Buffers
 * @brief Semantic buffer size mappings to universal SIZE tiers.
 *
 * | Constant | Tier | Size | Use Case |
 * |----------|------|------|----------|
 * | FLAG_CAPACITY | KERNEL | 27 | Bitfield capacity |
 * | LABEL_MAX | RUNTIME | 243 | Short labels |
 * | NAME_MAX | FRAMEWORK | 729 | Names, symbols |
 * | FILENAME_MAX | HEALTH | 2187 | File paths |
 * | MSG_MAX | NETWORK | 6561 | Messages, logs |
 * | RENDER_BUF_MAX | ENGINE | 59049 | Render buffers |
 * | USER_BUF_MAX | APP | 177147 | User data |
 * @{
 */

/**
 * @brief 27: flag capacity — bitfield and small array size.
 *
 * @return 27 (3³, integer constant)
 *
 * @par Derivation
 *      FLAG_CAPACITY = SIZE_KERNEL = 3³ = 27
 *
 * @par Usage
 *      Maximum flags in a bitfield, small lookup tables.
 */
#define CPISI_FLAG_CAPACITY         CPISI_SIZE_KERNEL

/**
 * @brief 243: label maximum — short label string length.
 *
 * @return 243 (3⁵, integer constant)
 *
 * @par Derivation
 *      LABEL_MAX = SIZE_RUNTIME = 3⁵ = 243
 *
 * @par Usage
 *      Maximum length for short labels, tags, identifiers.
 */
#define CPISI_LABEL_MAX             CPISI_SIZE_RUNTIME

/**
 * @brief 243: history maximum — history entry count (equals LABEL_MAX).
 *
 * @return 243 (integer constant)
 *
 * @par Derivation
 *      HISTORY_MAX = LABEL_MAX = 243
 *
 * @par Usage
 *      Maximum entries in history buffers (DAR, diagnostic).
 */
#define CPISI_HISTORY_MAX           CPISI_LABEL_MAX

/**
 * @brief 729: name maximum — name and symbol string length.
 *
 * @return 729 (3⁶, integer constant)
 *
 * @par Derivation
 *      NAME_MAX = SIZE_FRAMEWORK = 3⁶ = 729
 *
 * @par Usage
 *      Maximum length for names, symbols, function identifiers.
 */
#define CPISI_NAME_MAX              CPISI_SIZE_FRAMEWORK

/**
 * @brief 2187: filename maximum — file path string length.
 *
 * @return 2187 (3⁷, integer constant)
 *
 * @par Derivation
 *      FILENAME_MAX = SIZE_HEALTH = 3⁷ = 2187
 *
 * @par Usage
 *      Maximum length for file paths, URLs, resource locators.
 */
#define CPISI_FILENAME_MAX          CPISI_SIZE_HEALTH

/**
 * @brief 2187: error buffer maximum — error message length (equals FILENAME_MAX).
 *
 * @return 2187 (integer constant)
 *
 * @par Derivation
 *      ERROR_BUF_MAX = FILENAME_MAX = 2187
 *
 * @par Usage
 *      Maximum length for error messages, stack traces.
 */
#define CPISI_ERROR_BUF_MAX         CPISI_FILENAME_MAX

/**
 * @brief 6561: message maximum — log and message buffer size.
 *
 * @return 6561 (3⁸, integer constant)
 *
 * @par Derivation
 *      MSG_MAX = SIZE_NETWORK = 3⁸ = 6561
 *
 * @par Usage
 *      Maximum length for log messages, network payloads.
 */
#define CPISI_MSG_MAX               CPISI_SIZE_NETWORK

/**
 * @brief 59049: render buffer maximum — graphics rendering buffer.
 *
 * @return 59049 (3¹⁰, integer constant)
 *
 * @par Derivation
 *      RENDER_BUF_MAX = SIZE_ENGINE = 3¹⁰ = 59049
 *
 * @par Usage
 *      Maximum size for render buffers, vertex arrays.
 */
#define CPISI_RENDER_BUF_MAX        CPISI_SIZE_ENGINE

/**
 * @brief 177147: user buffer maximum — user data storage.
 *
 * @return 177147 (3¹¹, integer constant)
 *
 * @par Derivation
 *      USER_BUF_MAX = SIZE_APP = 3¹¹ = 177147
 *
 * @par Usage
 *      Maximum size for user-facing data buffers.
 */
#define CPISI_USER_BUF_MAX          CPISI_SIZE_APP

/**
 * @brief Path maximum — system-defined or fallback (19683).
 *
 * @return PATH_MAX from system, or 19683 (3⁹) as fallback
 *
 * @par Derivation
 *      Uses system PATH_MAX if available, else SIZE_STORAGE = 3⁹ = 19683
 *
 * @par Usage
 *      Maximum file path length for the current system.
 */
#ifndef CPISI_PATH_MAX
#ifdef PATH_MAX
#define CPISI_PATH_MAX              PATH_MAX
#else
#define CPISI_PATH_MAX              CPISI_SIZE_STORAGE
#endif
#endif

/** @} */ /* end Buffers */

/**
 * @name History
 * @brief Subsystem history buffer sizes.
 *
 * - DAR_HISTORY (243) — Detect-Assess-Restore events
 * - ENGRAM_HISTORY (243) — Cognitive memory snapshots
 * - HISTORY_SIZE (729) — General diagnostic trail
 * @{
 */

/**
 * @brief 243: DAR history buffer size — Detect-Assess-Restore events.
 *
 * @return 243 (3⁵, integer constant)
 *
 * @par Derivation
 *      DAR_HISTORY = SIZE_RUNTIME = 3⁵ = 243
 *
 * @par Usage
 *      Maximum DAR cycle events retained in memory.
 */
#define CPISI_DAR_HISTORY           CPISI_SIZE_RUNTIME

/**
 * @brief 243: engram history buffer size — cognitive memory snapshots.
 *
 * @return 243 (3⁵, integer constant)
 *
 * @par Derivation
 *      ENGRAM_HISTORY = SIZE_RUNTIME = 3⁵ = 243
 *
 * @par Usage
 *      Maximum engram entries for cognitive state recall.
 */
#define CPISI_ENGRAM_HISTORY        CPISI_SIZE_RUNTIME

/**
 * @brief 729: general history size — diagnostic and audit trail.
 *
 * @return 729 (3⁶, integer constant)
 *
 * @par Derivation
 *      HISTORY_SIZE = SIZE_FRAMEWORK = 3⁶ = 729
 *
 * @par Usage
 *      Maximum entries in general history/audit buffers.
 */
#define CPISI_HISTORY_SIZE          CPISI_SIZE_FRAMEWORK

/** @} */ /* end History */

/**
 * @name Ternary Zones
 * @brief Threshold boundaries for Hebrew state mapping.
 *
 * Divides the ternary range (-1.0 to +1.0) into 5 zones for state assessment.
 *
 * @par Zone Layout
 * 7 Hebrew states map to 5 thresholds:
 * @code
 * -1.0 ──[BROKEN]── -0.75 ──[LOWER]── -0.25 ──[UPPER]── +0.25 ──[PERFECT]── +0.75 ── +1.0
 *      shavar              ratsah/chaser      yashar       shalem/tamim         tov
 * @endcode
 *
 * @note Floating point because ternary values are continuous (-1.0 to +1.0).
 * @{
 */

/**
 * @brief -0.75: broken zone threshold — below this is shavar (BROKEN) state.
 *
 * @return -0.75 (floating point constant)
 *
 * @par Semantic Meaning
 *      The boundary below which a system is considered broken (shavar).
 *      Severe degradation requiring intervention.
 */
#define CPISI_TERNARY_ZONE_BROKEN   (-0.75)

/**
 * @brief -0.25: lower zone threshold — below this is ratsah/chaser (WANTING/LACKING).
 *
 * @return -0.25 (floating point constant)
 *
 * @par Semantic Meaning
 *      The boundary between negative states and center. Below: wanting growth (ratsah)
 *      or regressing (chaser) based on k-factor direction.
 */
#define CPISI_TERNARY_ZONE_LOWER    (-0.25)

/**
 * @brief +0.25: upper zone threshold — below this is yashar (EVEN) state.
 *
 * @return +0.25 (floating point constant)
 *
 * @par Semantic Meaning
 *      The boundary of the center zone. Between LOWER and UPPER is yashar —
 *      the balanced, even state. Neither deficient nor abundant.
 */
#define CPISI_TERNARY_ZONE_UPPER    (+0.25)

/**
 * @brief +0.75: perfect zone threshold — above this is tov (PERFECT) state.
 *
 * @return +0.75 (floating point constant)
 *
 * @par Semantic Meaning
 *      The boundary above which a system achieves tov (perfect/complete).
 *      Between UPPER and PERFECT is shalem/tamim based on k-factor direction.
 */
#define CPISI_TERNARY_ZONE_PERFECT  (+0.75)

/**
 * @brief -0.5: negative half — midpoint of negative ternary range.
 *
 * @return -0.5 (floating point constant)
 *
 * @par Derivation
 *      NEG_HALF = (TERNARY_MIN + TERNARY_CENTER) / 2 = (-1 + 0) / 2 = -0.5
 *
 * @par Semantic Meaning
 *      K-factor weighted threshold. Below this, k-factor is K_RETREAT.
 */
#define CPISI_TERNARY_NEG_HALF      (-0.5)

/**
 * @brief +0.5: positive half — midpoint of positive ternary range.
 *
 * @return +0.5 (floating point constant)
 *
 * @par Derivation
 *      POS_HALF = (TERNARY_CENTER + TERNARY_MAX) / 2 = (0 + 1) / 2 = +0.5
 *
 * @par Semantic Meaning
 *      K-factor weighted threshold. Above this, k-factor is K_ADVANCE.
 */
#define CPISI_TERNARY_POS_HALF      (+0.5)

/** @} */ /* end Ternary Zones */

/** @} */ /* end S.2a BOUNDS */

/** S.2b MACROS — [Reserved: inherits types.h] @defgroup cpisi_config_macros @ingroup cpisi_config_domain @{ @} */
/** S.2c VERIFY — [Reserved: inherits types.h] @defgroup cpisi_config_verify @ingroup cpisi_config_domain @{ @} */
/** S.2d LOOKUP — [Reserved: constants only] @defgroup cpisi_config_lookup @ingroup cpisi_config_domain @{ @} */
/** S.2e TYPES — [Reserved: constants only] @defgroup cpisi_config_types @ingroup cpisi_config_domain @{ @} */

/** @} */ /* end S.2 DEFINES */

/** S.3 TYPES — [Reserved: inherits types.h S.3] @defgroup cpisi_config_s3_types @{ @} */

/** S.4 PROTOTYPES — [Reserved: constants only] @defgroup cpisi_config_s4_prototypes @{ @} */

/** S.5 FILE-LEVEL — [Reserved: no state] @defgroup cpisi_config_s5_file_level @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — What This File Does Not Provide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_s6_omission S.6 OMISSION — Excluded Content
 * @brief    Documents what config.h intentionally does NOT provide.
 *
 * OMITTED (by design):
 *   - S.3 TYPES      — Uses types.h foundation types
 *   - S.4 PROTOTYPES — Pure constants, no functions
 *   - S.5 FILE-LEVEL — No static state needed
 *
 * WHY: Config domain is pure compile-time constants. No runtime types,
 *      functions, or state. All configuration values derive from types.h
 *      BOUNDS and are used by other domain files.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* Intentionally minimal — config provides constants only */

/** @} */ /* end S.6 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Runtime functions using SETUP constants.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART  — Structure overview
///   - B.2 HELPERS    — Zone threshold functions
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_orgchart B.1 ORG CHART — Structure Overview
 * @ingroup cpisi_types_config
 * @brief    Map structure — interface overview, implementation location.
 *
 * WHY: Understanding the file structure before diving into details.
 *      Org chart provides the mental model for navigation.
 *
 * SCOPE: File-level organization, implementation location, interface counts.
 *
 * CONTAINS:
 *   - B.1a Structure — Types, functions, externs count
 *   - B.1b Flow      — Implementation location
 *   - B.1c Counts    — Summary statistics
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Interface Overview [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_orgchart_structure B.1a STRUCTURE — Interface Overview
 * @ingroup cpisi_config_orgchart
 * @brief    Types, functions, and externs declared in this header.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      0 (bounds constants in SETUP) */
/* Functions:  2 inline (zone threshold functions) */
/* Externs:    0 */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_orgchart_flow B.1b FLOW — Implementation Location
 * @ingroup cpisi_config_orgchart
 * @brief    Where implementations live.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: Inline in header (simple threshold functions) */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_orgchart_counts B.1c COUNTS — Summary Statistics
 * @ingroup cpisi_config_orgchart
 * @brief    Interface element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      0 total */
/* Functions:  2 total (cpisi_kfactor_weighted, cpisi_ternary_to_hebrew) */
/* Externs:    0 total */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Zone Threshold Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_helpers B.2 HELPERS — Zone Threshold Functions
 * @ingroup cpisi_types_config
 * @brief    Foundation functions using S.2 BOUNDS constants.
 *
 * WHY: These functions apply the zone thresholds defined in SETUP.
 *      They convert between numeric values and discrete states.
 *
 * SCOPE: KFactor weighting, ternary-to-Hebrew conversion.
 *
 * CONTAINS:
 *   - B.2a CONVERSION — Value-to-state conversions
 *   - B.2b UTILITIES  — Common threshold operations
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a CONVERSION — Value-to-State Conversions [CONVERSION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_helpers_conversion B.2a CONVERSION — Value-to-State
 * @ingroup cpisi_config_helpers
 * @brief    Convert numeric values to discrete states using zone thresholds.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Apply weight to KFactor and return threshold-based result.
 *
 * Multiplies KFactor by weight and maps the result to a discrete KFactor
 * based on S.2 half-thresholds (±0.5).
 *
 * @param k      KFactor to weight (-1, 0, or +1)
 * @param weight Multiplier (0.0 to 1.0 typical)
 * @return KFactor based on weighted value vs half-thresholds:
 *         - K_RETREAT if weighted < -0.5
 *         - K_ADVANCE if weighted > +0.5
 *         - K_MAINTAIN otherwise
 *
 * @par Algorithm
 *      weighted = k × weight
 *      if weighted < NEG_HALF (-0.5): retreat
 *      if weighted > POS_HALF (+0.5): advance
 *      else: maintain
 *
 * @par Usage
 *      Applies confidence/certainty weighting to directional intent.
 *      Low confidence pulls toward center (maintain).
 *
 * @see CPISI_TERNARY_NEG_HALF — lower threshold (-0.5)
 * @see CPISI_TERNARY_POS_HALF — upper threshold (+0.5)
 */
static inline KFactor cpisi_kfactor_weighted(KFactor k, float weight) {
    float weighted = (float)k * weight;
    if (weighted < CPISI_TERNARY_NEG_HALF) return K_RETREAT;
    if (weighted > CPISI_TERNARY_POS_HALF) return K_ADVANCE;
    return K_MAINTAIN;
}

/**
 * @brief Convert ternary float to HebrewState using zone thresholds.
 *
 * Maps a ternary value [-1.0, +1.0] to one of 7 Hebrew cognitive states
 * using the 5-zone threshold system. Returns POSITIVE-direction variant
 * for middle zones (ratsah for wanting, shalem for expanding).
 *
 * @param value Ternary float in range [-1.0, +1.0]
 * @return HebrewState based on zone thresholds:
 *         - SHAVAR if value ≤ -0.75 (broken)
 *         - RATSAH if value ≤ -0.25 (wanting)
 *         - YASHAR if value ≤ +0.25 (even/center)
 *         - SHALEM if value ≤ +0.75 (whole)
 *         - TOV    if value > +0.75 (perfect)
 *
 * @par Algorithm
 *      Cascading threshold check against S.2 zone constants:
 *      BROKEN (-0.75) → LOWER (-0.25) → UPPER (+0.25) → PERFECT (+0.75)
 *
 * @par Usage
 *      Converts continuous health scores to discrete cognitive states.
 *      Used by health monitoring to determine current system state.
 *
 * @see CPISI_TERNARY_ZONE_BROKEN — shavar threshold (-0.75)
 * @see CPISI_TERNARY_ZONE_LOWER  — warning threshold (-0.25)
 * @see CPISI_TERNARY_ZONE_UPPER  — healthy threshold (+0.25)
 * @see CPISI_TERNARY_ZONE_PERFECT — tov threshold (+0.75)
 */
static inline HebrewState cpisi_ternary_to_hebrew(float value) {
    if (value <= CPISI_TERNARY_ZONE_BROKEN) return HEBREW_SHAVAR;
    if (value <= CPISI_TERNARY_ZONE_LOWER)  return HEBREW_RATSAH;
    if (value <= CPISI_TERNARY_ZONE_UPPER)  return HEBREW_YASHAR;
    if (value <= CPISI_TERNARY_ZONE_PERFECT) return HEBREW_SHALEM;
    return HEBREW_TOV;
}

/** @} */ /* end B.2a CONVERSION */

/** B.2b UTILITIES — [Reserved] @defgroup cpisi_config_helpers_utilities @ingroup cpisi_config_helpers @{ @} */

/** @} */ /* end B.2 HELPERS */

/** B.3 ERRORS — [Reserved: uses types.h B.4] @defgroup cpisi_config_errors @ingroup cpisi_types_config @{ @} */

/** B.4 PUBLIC API — [Reserved: #define constants] @defgroup cpisi_config_api @ingroup cpisi_types_config @{ @} */

/** B.5 EXTENSIONS — [Reserved: leaf domain] @defgroup cpisi_config_extensions @ingroup cpisi_types_config @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — What BODY Reserves [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_body_omission B.6 OMISSION — Reserved BODY Sections
 * @brief    Documents what BODY sections are intentionally reserved.
 *
 * | Section | Status | Reason |
 * |---------|--------|--------|
 * | B.1 ORG CHART | Active | Structure overview |
 * | B.2 HELPERS | Active | Zone threshold functions (2) |
 * | B.3 ERRORS | Reserved | Uses foundation error recovery |
 * | B.4 PUBLIC API | Reserved | Constants accessed directly |
 * | B.5 EXTENSIONS | Reserved | Config is leaf domain |
 *
 * WHY: Config domain is minimal by design — compile-time constants only.
 * Runtime functions limited to threshold conversions in B.2.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* Intentionally minimal — see table above */

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
///   - X.1 VALIDATION    — Build verification
///   - X.2 EXECUTION     — Usage patterns
///   - X.3 CLEANUP       — Ownership notes
///   - X.4 POLICY        — Modification guidance
///   - X.5 EXTENSION     — How to add config constants
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
 * @defgroup cpisi_config_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build and syntax verification.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -fsyntax-only -Wall -Wextra config.h       # syntax check
 * cppcheck --enable=all config.h                 # static analysis
 * @endcode
 *
 * @par X.1b SELFTEST — Minimal Isolation Test
 * @code{.c}
 * #include "kernel/cpisi/types/config/config.h"
 * int main(void) { return 0; }
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_execution X.2 EXECUTION — Usage Patterns
 * @brief    Include pattern and usage examples.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.2a INCLUDE — Include Pattern
 * @code{.c}
 * #include "kernel/cpisi/types/config/config.h"
 * @endcode
 *
 * @par X.2b USAGE — Config Constants
 * @code{.c}
 * char filename[CPISI_FILENAME_MAX];
 * int retries = CPISI_RETRY_DEFAULT;  // 3
 *
 * HebrewState state = cpisi_ternary_to_hebrew(0.5);  // HEBREW_SHALEM
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Ownership notes — compile-time constants, no allocation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * All values are compile-time constants (#define).
 * No heap allocation, no cleanup required.
 * Inline functions operate on stack values only.
 *
 * @} */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_policy X.4 POLICY — Modification Guidance
 * @brief    Guide maintainers — modification safety levels.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Safe to Modify
 *   - Add new buffer size constants (following tier pattern)
 *   - Adjust retention/rotation values
 *
 * @par Modify with Care
 *   - Zone thresholds — affects all state classification
 *   - Retry bounds — affects error handling behavior
 *
 * @par NEVER Modify
 *   - Derivation from types.h SIZE_* values
 *   - CPISI_UNITY, CPISI_TERNARY, CPISI_HARDPOINTS references
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Add Config [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_extension X.5 EXTENSION — How to Add Config
 * @brief    How to add new configuration constants.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Adding Constants
 *   - Add to S.2a BOUNDS in appropriate @name group
 *   - Derive from types.h SIZE_* or BOUNDS where possible
 *   - Include full docstring (@brief, @return, @par Derivation, @par Usage)
 *   - Update B.1c COUNTS if adding functions
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Problems [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par "Undefined CPISI_SIZE_*"
 *   Ensure types.h is included before config.h.
 *   config.h includes types.h automatically.
 *
 * @par "Zone threshold mismatch"
 *   Verify threshold order: BROKEN < LOWER < UPPER < PERFECT
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick lookup — key config values.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Buffer Sizes (Layer → Size)
 *   L0 KERNEL: 27, L2 RUNTIME: 243, L3 FRAMEWORK: 729,
 *   L4 HEALTH: 2187, L5 NETWORK: 6561, L6 STORAGE: 19683,
 *   L7 ENGINE: 59049, L8 APP: 177147
 *
 * @par Zone Thresholds
 *   BROKEN: -0.75, LOWER: -0.25, UPPER: +0.25, PERFECT: +0.75
 *
 * @par Retry Bounds
 *   MIN: 1, DEFAULT: 3, MAX: 5
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * Scripture: Proverbs 16:11
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * Config domain provides THE measurement standards for CPI-SI.
 * Zone thresholds, buffer sizes, and timing values — all derive from
 * ternary mathematics anchored in types.h.
 *
 * "A just weight and balance are the LORD's: all the weights of the
 *  bag are his work." — Proverbs 16:11
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Reserved Sections [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_closing_omission X.9 OMISSION — Reserved Sections
 * @brief    CLOSING sections guidance.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * All CLOSING sections present. Config domain is minimal — constants only.
 *
 * @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_TYPES_CONFIG_H */
