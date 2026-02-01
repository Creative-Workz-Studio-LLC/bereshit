/// @file timing.h
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
/// @omni:req  key        = CPISI-TYPES-TIMING
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
/// @omni:ins  layer      = types/timing
/// @omni:ins  includes   = types.h
/// @omni:ins  provides   = [S.2_TIMING]
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
/// @omni:req  title      = CPI-SI Timing Domain
/// @omni:req  brief      = Interval derivation and temporal operations
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
 * @defgroup cpisi_timing_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup cpisi_timing_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_timing_special_config
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

#ifndef CPISI_TYPES_TIMING_H
#define CPISI_TYPES_TIMING_H

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
 * @file      timing.h
 * @brief     CPI-SI Timing Domain — Intervals, rotation, retention.
 *
 * @defgroup cpisi_timing_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-TIMING
 * Title:     CPI-SI Timing Domain Types
 * Type:      Header (Domain)
 * Component: Rails — provides timing types for scheduling and temporal ops
 * Role:      Intervals (5), rotation (9 tiers), retention policies.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_state M.2 STATE — Lifecycle State
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
 * @defgroup cpisi_timing_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_timing_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/timing/timing.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: types/types.h (foundation), types/config/config.h
 * Template:     bereshit/word/seed/code/c/type-header.h
 * Extracted:    2026-01-21 from types.h timing sections
 *
 * Builds FROM:  types.h, config/config.h
 * Builds TO:    Health monitoring, log rotation, data retention
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, timing, domain, intervals, rotation, retention
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide timing intervals, rotation schedules, retention policies.
 * Core Design: 5 intervals, 9 rotation tiers matching layer architecture.
 * Philosophy:  Proper timing enables sustainable operation and data lifecycle.
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Ecclesiastes 3:1 — "To every thing there is a season, and a time"
 * Principle:  Proper timing enables sustainable operation and data lifecycle.
 * Anchor:     Psalm 31:15 — "My times are in thy hand"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h, config/config.h
 *
 * What Uses This:
 *   - Health monitoring
 *   - Log rotation
 *   - Data retention
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
 * @defgroup cpisi_timing_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-21) — Extracted from types.h timing sections
 *   a-01.25 (2026-01-23) — Expanded METADATA to full M.1-M.10 structure
 *   b-03.00 (2026-01-25) — Template alignment: type-header.h, CLOSING X.1-X.9
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ Timing types defined
 *   ✓ X.1-X.9 CLOSING guidance
 *
 * Planned:
 *   ⏳ Scheduling utilities
 *   ⏳ Timezone-aware operations
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
///   - S.2 DEFINES   — Timing domain (Bounds*, Macros, Verify, Lookup, Types)
///
/// Note: (*) = reserved (inherited from types.h)
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
 * @defgroup cpisi_timing_includes S.1 INCLUDES — Header Dependencies
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
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** S.1a STANDARD — [Inherited from types.h S.1a] @defgroup cpisi_timing_standard @ingroup cpisi_timing_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_timing_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_timing_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * PROVIDES: Foundation types, macros, and timing infrastructure.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/config/cfg/domain.cfg.h"  /**< TIER 1: config anchors (brings types.h transitively) */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [None required] @defgroup cpisi_timing_external @ingroup cpisi_timing_includes @{ @} */

/** S.1d CPP — [Inherited from types.h S.1d] @defgroup cpisi_timing_cpp @ingroup cpisi_timing_includes @{ @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Timing Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_domain S.2 DEFINES — Timing Domain
 * @brief    Timing types — intervals, rotations, and retention policies.
 *
 * PURPOSE: Define timing-specific constants, operations, verification, and types.
 *
 * Builds FROM: S.1 INCLUDES (types.h foundation via config.h)
 * Builds TO:   Health monitoring, log rotation, data retention
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: no domain-specific preludes]
 *   - S.2a  BOUNDS  — [Reserved: inherited from types.h]
 *   - S.2b  MACROS  — Timing operations (interval derivation)
 *   - S.2c VERIFY  — Timing compile-time assertions
 *   - S.2d LOOKUP  — Timing runtime tables
 *   - S.2e TYPES   — Timing enumerations
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  TIMING DOMAIN — Health Check Scheduling                                   │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │                                                                             │
 * │  INTERVALS (9):  Health check frequencies (100ms → 1 day)                   │
 * │  ROTATIONS:      Log rotation schedules                                     │
 * │  RETENTION:      Data retention policies                                    │
 * │                                                                             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 * @{
 */

/** S.2.0 PRELUDE — [Reserved: inherits types.h] @defgroup cpisi_timing_prelude @ingroup cpisi_timing_domain @{ @} */

/** S.2a BOUNDS — [Reserved: inherits types.h] @defgroup cpisi_timing_bounds @ingroup cpisi_timing_domain @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b MACROS — Timing Operations [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2b_TIMING S.2b TIMING MACROS — Operations
 * @ingroup cpisi_timing_domain
 * @brief    Interval, rotation, and retention operations for timing domain.
 *
 * PURPOSE: Provide timing-specific operations that transform BOUNDS values
 *          from types.h into usable timing calculations.
 *
 * Builds FROM: types.h (S.2a BOUNDS, S.2b.2 PRIMITIVES)
 * Builds TO:   Health monitoring (interval scheduling), log systems (rotation)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC extensions to S.2b MACROS defined in
 *   types.h. The foundation macros (SIGN_VALID, LAYER_VALID, etc.) come from
 *   types.h. This file adds timing-specific operations using those primitives.
 *
 * STRUCTURE:
 *   S.2b.5 TIMING MACROS — Operations on timing bounds
 *     S.2b.5a INTERVAL  — Interval derivation and validation
 *     S.2b.5b ROTATION  — Log rotation size scaling
 *     S.2b.5c RETENTION — Data retention period validation
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @defgroup S2b_5_TIMING S.2b.5 TIMING MACROS — Operations on timing bounds
 * @ingroup S2b_MACROS
 * @brief DOMAIN-SPECIFIC operations for timing configuration. Target: types-config.h.
 *
 * Timing macros derive all intervals from the POLL anchor (1000ms).
 * This ensures consistent timing relationships throughout the system.
 *
 * INTERVAL DERIVATION (from POLL = 1000ms):
 * ```
 *   Interval   │ Formula      │ Value  │ Use Case
 *   ───────────┼──────────────┼────────┼─────────────────────
 *   DEBOUNCE   │ POLL ÷ 10    │  100ms │ Rapid events
 *   HALF_POLL  │ POLL ÷ 2     │  500ms │ Sub-poll checks
 *   POLL       │ (anchor)     │ 1000ms │ Standard polling
 *   FLUSH      │ POLL × 3     │ 3000ms │ Buffer flushing
 *   TIMEOUT    │ POLL × 5     │ 5000ms │ Operation timeout
 * ```
 *
 * STRUCTURE:
 *   S.2b.5a INTERVAL  — Interval derivation and validation
 *   S.2b.5b ROTATION  — Log rotation size scaling
 *   S.2b.5c RETENTION — Data retention period validation
 * @{ */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.5a INTERVAL OPS — Interval derivation and validation
 *
 * All intervals derive from POLL_INTERVAL_MS (1000ms).
 *
 * BOUNDS USED:
 *   POLL_INTERVAL_MS = 1000 (S.2a.8)  — the timing anchor
 *   HARDPOINTS = 5 (S.2a.2)           — timeout multiplier
 *   TRINITY = 3 (S.2a.1)              — flush multiplier
 *   INTERVAL_COUNT = 5 (S.2a.8)       — number of interval types
 *   RETRY_MIN = 1, RETRY_MAX = 5 (S.2a.8)
 *
 * DERIVATION CHAIN:
 *   POLL (anchor) → DEBOUNCE (÷10) → HALF_POLL (÷2) → FLUSH (×3) → TIMEOUT (×5)
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_5a_INTERVAL S.2b.5a INTERVAL OPS — Interval operations
 * @ingroup S2b_5_TIMING
 * @brief Derive and validate interval values from POLL anchor.
 *
 * INTERVAL TABLE:
 * ```
 *   Index │ Name      │ Formula    │ Value  │ Meaning
 *   ──────┼───────────┼────────────┼────────┼────────────────
 *     0   │ DEBOUNCE  │ POLL÷10    │  100ms │ Fastest (input)
 *     1   │ HALF_POLL │ POLL÷2     │  500ms │ Sub-cycle
 *     2   │ POLL      │ anchor     │ 1000ms │ Standard (center)
 *     3   │ FLUSH     │ POLL×3     │ 3000ms │ Buffering
 *     4   │ TIMEOUT   │ POLL×5     │ 5000ms │ Slowest (failure)
 * ```
 * @{ */

/* ── Derived intervals from POLL anchor ────────────────────────────────────── */

/**
 * @brief Debounce interval: POLL ÷ 10 = 100ms (fastest input response).
 *
 * BOUNDS USED:
 *   - CPISI_POLL_INTERVAL_MS = 1000 (S.2a.8 TIMING) — the timing anchor
 *
 * @par Derivation
 *      POLL ÷ 10 = 1000 ÷ 10 = 100ms
 *
 * USE CASE: Rapid event filtering, input debounce, anti-flutter protection.
 */
#define CPISI_DEBOUNCE_MS       CPISI_QUOTIENT(CPISI_POLL_INTERVAL_MS, 10)

/**
 * @brief Timeout interval: POLL × HARDPOINTS = 5000ms (operation failure).
 *
 * BOUNDS USED:
 *   - CPISI_POLL_INTERVAL_MS = 1000 (S.2a.8 TIMING) — the timing anchor
 *   - CPISI_HARDPOINTS = 5 (S.2a.2 STRUCTURE) — multiplier
 *
 * @par Derivation
 *      POLL × HARDPOINTS = 1000 × 5 = 5000ms
 *
 * USE CASE: Operation timeout, failure detection, health check threshold.
 */
#define CPISI_TIMEOUT_MS        CPISI_PRODUCT(CPISI_POLL_INTERVAL_MS, CPISI_HARDPOINTS)

/**
 * @brief Flush interval: POLL × TERNARY = 3000ms (buffer commits).
 *
 * BOUNDS USED:
 *   - CPISI_POLL_INTERVAL_MS = 1000 (S.2a.8 TIMING) — the timing anchor
 *   - CPISI_TERNARY = 3 (S.2a.1 PRIMITIVE) — multiplier
 *
 * @par Derivation
 *      POLL × TERNARY = 1000 × 3 = 3000ms
 *
 * USE CASE: Buffer flushing, batch commits, periodic sync operations.
 */
#define CPISI_FLUSH_MS          CPISI_PRODUCT(CPISI_POLL_INTERVAL_MS, CPISI_TERNARY)

/**
 * @brief Half-poll interval: POLL ÷ 2 = 499ms (sub-cycle checks).
 *
 * BOUNDS USED:
 *   - CPISI_POLL_INTERVAL_MS = 1000 (S.2a.8 TIMING) — the timing anchor
 *   - CPISI_HALF macro (S.2b.2 PRIMITIVES) — floor division
 *
 * @par Derivation
 *      HALF(POLL) = (1000 - 1) ÷ 2 = 499ms (floor)
 *
 * USE CASE: Offset timing, sub-poll checks, avoiding sync issues.
 */
#define CPISI_HALF_POLL_MS      CPISI_HALF(CPISI_POLL_INTERVAL_MS)

/* ── Interval MS bounds ────────────────────────────────────────────────────── */

/**
 * @brief Minimum interval (fastest): 100ms.
 *
 * @par Derivation
 *      Equals CPISI_DEBOUNCE_MS — the fastest standard interval.
 *
 * USE CASE: Lower bound for interval validation.
 */
#define CPISI_INTERVAL_MS_MIN   CPISI_DEBOUNCE_MS

/**
 * @brief Maximum interval (slowest): 5000ms.
 *
 * @par Derivation
 *      Equals CPISI_TIMEOUT_MS — the slowest standard interval.
 *
 * USE CASE: Upper bound for interval validation.
 */
#define CPISI_INTERVAL_MS_MAX   CPISI_TIMEOUT_MS

/**
 * @brief Center interval index: 2 (POLL).
 *
 * @par Derivation
 *      Equals CPISI_HARDPOINTS_CENTER — the center of 5 intervals.
 *
 * USE CASE: Default interval selection, standard polling.
 */
#define CPISI_INTERVAL_CENTER   CPISI_HARDPOINTS_CENTER

/* ── Named interval indices (0-4) ──────────────────────────────────────────── */

/**
 * @brief Interval index 0: DEBOUNCE (100ms, fastest).
 *
 * @par Derivation
 *      Equals CPISI_VOID (0) — first interval position.
 */
#define CPISI_INTERVAL_DEBOUNCE     CPISI_VOID

/**
 * @brief Interval index 1: HALF_POLL (499ms).
 *
 * @par Derivation
 *      Equals CPISI_UNITY (1) — second interval position.
 */
#define CPISI_INTERVAL_HALF_POLL    CPISI_UNITY

/**
 * @brief Interval index 2: POLL (1000ms, center).
 *
 * @par Derivation
 *      Equals CPISI_PAIR (2) — center interval position.
 */
#define CPISI_INTERVAL_POLL         CPISI_PAIR

/**
 * @brief Interval index 3: FLUSH (3000ms).
 *
 * @par Derivation
 *      Equals CPISI_TERNARY (3) — fourth interval position.
 */
#define CPISI_INTERVAL_FLUSH        CPISI_TERNARY

/**
 * @brief Interval index 4: TIMEOUT (5000ms, slowest).
 *
 * @par Derivation
 *      Equals CPISI_SQUARE (4) — last interval position.
 */
#define CPISI_INTERVAL_TIMEOUT      CPISI_SQUARE

/* ── Validation ────────────────────────────────────────────────────────────── */

/**
 * @brief Validate interval index [0, INTERVAL_COUNT-1].
 *
 * @param i Interval index to validate
 * @return 1 if i in [0, 4], 0 otherwise
 *
 * BOUNDS: INTERVAL_COUNT = 5 (S.2a.8 TIMING)
 * RANGE:  [MIN_OF(5), MAX_OF(5)] = [0, 4]
 *
 * INTERVAL INDICES:
 *   0 = DEBOUNCE, 1 = HALF_POLL, 2 = POLL (center), 3 = FLUSH, 4 = TIMEOUT
 *
 * EXAMPLE:
 * ```c
 *   uint8_t interval_idx = get_interval_selection();
 *   if (!CPISI_INTERVAL_VALID(interval_idx)) {
 *       return CPISI_ERROR_INVALID_INTERVAL;
 *   }
 *   uint32_t ms = interval_to_ms(interval_idx);
 * ```
 */
#define CPISI_INTERVAL_VALID(i)     CPISI_IN_RANGE((i), CPISI_MIN_OF(CPISI_INTERVAL_COUNT), CPISI_MAX_OF(CPISI_INTERVAL_COUNT))

/**
 * @brief Validate interval milliseconds [DEBOUNCE, TIMEOUT].
 *
 * @param ms Milliseconds value to validate
 * @return 1 if ms in [100, 5000], 0 otherwise
 *
 * BOUNDS: INTERVAL_MS_MIN = 100 (DEBOUNCE), INTERVAL_MS_MAX = 5000 (TIMEOUT)
 * DERIVATION: Both derived from POLL_INTERVAL_MS (1000ms)
 *
 * NOTE: This validates absolute ms values. For index validation, use INTERVAL_VALID.
 *
 * EXAMPLE:
 * ```c
 *   uint32_t poll_ms = config_get_poll_interval();
 *   if (!CPISI_INTERVAL_MS_VALID(poll_ms)) {
 *       poll_ms = CPISI_POLL_INTERVAL_MS;  // Fall back to default
 *   }
 * ```
 */
#define CPISI_INTERVAL_MS_VALID(ms) CPISI_IN_RANGE((ms), CPISI_INTERVAL_MS_MIN, CPISI_INTERVAL_MS_MAX)

/**
 * @brief Validate retry count [RETRY_MIN, RETRY_MAX].
 *
 * @param r Retry count to validate
 * @return 1 if r in [1, 5], 0 otherwise
 *
 * BOUNDS: RETRY_MIN = 1 (S.2a.8), RETRY_MAX = 5 (S.2a.8)
 * RANGE:  [1, 5] — always at least 1 attempt, at most 5 (HARDPOINTS)
 *
 * WHY MIN=1: Zero retries means no attempt at all — invalid for retry logic.
 * WHY MAX=5: HARDPOINTS (5) represents the maximum reasonable retry count.
 *
 * EXAMPLE:
 * ```c
 *   uint8_t retries = config_get_max_retries();
 *   if (!CPISI_RETRY_VALID(retries)) {
 *       retries = CPISI_RETRY_DEFAULT;  // Fall back to default (3)
 *   }
 * ```
 */
#define CPISI_RETRY_VALID(r)        CPISI_IN_RANGE((r), CPISI_RETRY_MIN, CPISI_RETRY_MAX)

/** @} */ /* end S.2b.5a INTERVAL OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.5b ROTATION OPS — Log rotation size scaling
 *
 * Log rotation sizes scale by layer: higher layers = larger rotation files.
 *
 * BOUNDS USED:
 *   LAYERS = 9 (S.2a.3)           — rotation tiers = pyramid layers
 *   ROTATION_BASE_KB (S.2a.8)     — base size multiplied by 3^layer
 *   LAYER_POWERS[] (S.2d)         — runtime scaling factors
 *
 * ROTATION SIZE TABLE (assuming BASE = 1KB):
 *   Tier 0: 1 × 1 = 1 KB
 *   Tier 4: 1 × 81 = 81 KB
 *   Tier 8: 1 × 6561 = 6.4 MB
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_5b_ROTATION S.2b.5b ROTATION OPS — Rotation operations
 * @ingroup S2b_5_TIMING
 * @brief Log rotation size calculation and tier validation.
 *
 * Rotation tiers parallel pyramid layers — inner layers have smaller
 * rotation files (fast rotation), outer layers have larger files (less churn).
 * @{ */

/**
 * @brief Number of rotation tiers (= LAYERS = 9).
 *
 * @par Derivation
 *      Equals CPISI_LAYERS — rotation tiers parallel pyramid layers.
 *
 * USE CASE: Iteration bound for rotation tier loops.
 */
#define CPISI_ROTATION_COUNT    CPISI_LAYERS

/**
 * @brief Calculate rotation size for layer: base × 3^layer.
 *
 * @param layer Layer index (0-8)
 * @return Rotation size in KB
 *
 * BOUNDS USED:
 *   - ROTATION_BASE_KB (S.2a.8) — base rotation size
 *   - LAYER_POWERS[] (S.2d)     — runtime 3^layer factors
 *
 * FORMULA: size = ROTATION_BASE_KB × 3^layer
 *
 * DERIVATION:
 *   - Inner layers (KERNEL) rotate small files frequently
 *   - Outer layers (APP) rotate large files less frequently
 *   - Exponential scaling matches layer power sequence
 *
 * SIZE TABLE (assuming BASE = 1KB):
 * | Layer | Name     | 3^layer | Size    |
 * |:-----:|----------|:-------:|---------|
 * |   0   | KERNEL   |    1    | 1 KB    |
 * |   4   | HEALTH   |   81    | 81 KB   |
 * |   8   | APP      | 6561    | 6.4 MB  |
 *
 * EXAMPLE:
 * ```c
 *   uint8_t layer = get_current_layer();
 *   size_t max_size = CPISI_ROTATION_SIZE(layer) * 1024;  // Convert to bytes
 *   if (log_file_size > max_size) {
 *       rotate_log();
 *   }
 * ```
 */
#define CPISI_ROTATION_SIZE(layer)  CPISI_LAYER_SCALE(CPISI_ROTATION_BASE_KB, (layer))

/**
 * @brief Validate rotation tier index [0, ROTATION_COUNT-1].
 *
 * @param r Rotation tier to validate
 * @return 1 if r in [0, 8], 0 otherwise
 *
 * BOUNDS: ROTATION_COUNT = 9 (= LAYERS, S.2a.3)
 * RANGE:  [MIN_OF(9), MAX_OF(9)] = [0, 8]
 *
 * NOTE: Rotation tiers parallel pyramid layers — same validation logic.
 *
 * EXAMPLE:
 * ```c
 *   uint8_t tier = config_get_rotation_tier();
 *   if (!CPISI_ROTATION_VALID(tier)) {
 *       tier = CPISI_LAYER_CENTER;  // Default to center (HEALTH)
 *   }
 * ```
 */
#define CPISI_ROTATION_VALID(r)     CPISI_IN_RANGE((r), CPISI_MIN_OF(CPISI_ROTATION_COUNT), CPISI_MAX_OF(CPISI_ROTATION_COUNT))

/** @} */ /* end S.2b.5b ROTATION OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.5c RETENTION OPS — Data retention period validation
 *
 * Retention periods define how long data is kept before archiving/deletion.
 *
 * BOUNDS USED:
 *   RETENTION_RECENT (S.2a.8)  — minimum retention (recent data threshold)
 *
 * RETENTION POLICY:
 *   - Days < RECENT: Too short, invalid
 *   - Days >= RECENT: Valid retention period
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_5c_RETENTION S.2b.5c RETENTION OPS — Retention operations
 * @ingroup S2b_5_TIMING
 * @brief Data retention period validation.
 *
 * Ensures retention periods meet minimum requirements.
 * Short retention = data loss risk; valid periods ensure data integrity.
 * @{ */

/**
 * @brief Validate retention period (days >= RETENTION_RECENT).
 *
 * @param days Days to validate
 * @return 1 if days >= minimum, 0 otherwise
 *
 * BOUNDS USED:
 *   - RETENTION_RECENT (S.2a.8) — minimum valid retention period
 *
 * FORMULA: days >= RETENTION_RECENT
 *
 * WHY MINIMUM-ONLY:
 *   This is a floor check, not a range check. There is no maximum retention
 *   because longer data retention is always valid. The concern is too-short
 *   retention risking data loss before archival/analysis completes.
 *
 * RETENTION POLICY:
 *   - Days < RECENT: Too short → invalid (data loss risk)
 *   - Days >= RECENT: Valid retention period
 *
 * EXAMPLE:
 * ```c
 *   uint32_t days = config_get_retention_days();
 *   if (!CPISI_RETENTION_VALID(days)) {
 *       log_warn("Retention too short: %u days", days);
 *       days = CPISI_RETENTION_RECENT;  // Use minimum
 *   }
 * ```
 */
#define CPISI_RETENTION_VALID(days) ((days) >= CPISI_RETENTION_RECENT)

/** @} */ /* end S.2b.5c RETENTION OPS */
/** @} */ /* end S.2b.5 TIMING MACROS */

/** @} */ /* end S.2b TIMING MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c VERIFY — Compile-time Assertions [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2c_TIMING S.2c TIMING VERIFY — Compile-time assertions
 * @ingroup cpisi_timing_domain
 * @brief    Verify timing interval, rotation, and retention architecture.
 *
 * PURPOSE: Catch timing configuration errors BEFORE runtime. Zero runtime cost.
 *          If any assertion fails, compilation stops with a descriptive message.
 *
 * Builds FROM: S.2a TIMING BOUNDS (interval values), S.2b TIMING MACROS (derivations)
 * Builds TO:   Runtime timing systems (verified values ready for use)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC tripwires extending S.2c VERIFY in types.h.
 *   Core tripwires (VOID==0, UNITY==1, etc.) are in types.h. This file verifies
 *   timing-specific values derived from those primitives.
 *
 * TRIPWIRE PHILOSOPHY:
 *   Each value is verified TWO ways:
 *   1. VALUE TRIPWIRE:        Assert the literal (e.g., POLL_INTERVAL_MS == 1000)
 *   2. RELATIONSHIP TRIPWIRE: Assert the derivation (e.g., TIMEOUT_MS == POLL × 5)
 *   BOTH together catches drift in either direction.
 *
 * STRUCTURE:
 *   S.2c.4 TIMING VERIFY — Verify timing values
 *     S.2c.4a VALUE       — Timing literals (POLL, DEBOUNCE, TIMEOUT)
 *     S.2c.4b RELATIONSHIP — Derivations from POLL
 *     S.2c.4c ORDER       — DEBOUNCE < POLL < TIMEOUT ordering
 *     S.2c.4d RETENTION   — 90/180 day retention periods
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2c_4_TIMING S.2c.4 TIMING VERIFY — Verify timing values
 * @ingroup S2c_VERIFY
 * @brief Verify timing intervals, ordering, and retention periods.
 *
 * TIMING HIERARCHY:
 *   DEBOUNCE(100ms) < POLL(1000ms) < TIMEOUT(5000ms)
 *
 * ANCHOR VALUE: POLL_INTERVAL_MS = 1000 (1 second)
 *   All other intervals derive from POLL:
 *   - DEBOUNCE = POLL ÷ 10 = 100ms (anti-flutter)
 *   - TIMEOUT = POLL × HARDPOINTS = 1000 × 5 = 5000ms
 *
 * STRUCTURE:
 *   S.2c.4a VALUE       — Timing literals
 *   S.2c.4b RELATIONSHIP — Derivations from POLL
 *   S.2c.4c ORDER       — DEBOUNCE < POLL < TIMEOUT ordering
 *   S.2c.4d RETENTION   — 90/180 day retention periods
 * @{ */

/**
 * @defgroup S2c_4a_VALUE S.2c.4a VALUE TRIPWIRES — Verify timing literals
 * @ingroup S2c_4_TIMING
 * @brief Verify interval and retention literal values.
 * @{ */

/* VALUE tripwires — timing intervals */
CPISI_STATIC_ASSERT(CPISI_POLL_INTERVAL_MS == 1000,
                    "POLL_INTERVAL_MS must be 1000 — THE anchor");
CPISI_STATIC_ASSERT(CPISI_DEBOUNCE_MS == 100,
                    "DEBOUNCE_MS must be 100 — POLL÷10");
CPISI_STATIC_ASSERT(CPISI_FLUSH_MS == 3000,
                    "FLUSH_MS must be 3000 — POLL×3");
CPISI_STATIC_ASSERT(CPISI_TIMEOUT_MS == 5000,
                    "TIMEOUT_MS must be 5000 — POLL×5");
CPISI_STATIC_ASSERT(CPISI_HALF_POLL_MS == 499,
                    "HALF_POLL_MS must be 499 — (POLL-1)/2");

/* VALUE tripwires — interval indices */
CPISI_STATIC_ASSERT(CPISI_INTERVAL_DEBOUNCE == 0,
                    "INTERVAL_DEBOUNCE must be 0 (fastest)");
CPISI_STATIC_ASSERT(CPISI_INTERVAL_HALF_POLL == 1,
                    "INTERVAL_HALF_POLL must be 1");
CPISI_STATIC_ASSERT(CPISI_INTERVAL_POLL == 2,
                    "INTERVAL_POLL must be 2 (center)");
CPISI_STATIC_ASSERT(CPISI_INTERVAL_FLUSH == 3,
                    "INTERVAL_FLUSH must be 3");
CPISI_STATIC_ASSERT(CPISI_INTERVAL_TIMEOUT == 4,
                    "INTERVAL_TIMEOUT must be 4 (slowest)");

/* EQUIVALENCE tripwires — count matches HARDPOINTS */
CPISI_STATIC_ASSERT(CPISI_INTERVAL_COUNT == CPISI_HARDPOINTS,
                    "INTERVAL_COUNT must equal HARDPOINTS (5)");

/** @} */ /* end S.2c.4a VALUE */

/**
 * @defgroup S2c_4b_RELATIONSHIP S.2c.4b RELATIONSHIP TRIPWIRES — Verify timing derivations
 * @ingroup S2c_4_TIMING
 * @brief Verify DEBOUNCE and TIMEOUT derive from POLL.
 *
 * DERIVATION CHAIN:
 *   POLL = 1000ms (anchor)
 *   DEBOUNCE = POLL ÷ 10 = 100ms
 *   TIMEOUT = POLL × HARDPOINTS = 1000 × 5 = 5000ms
 * @{ */

/* RELATIONSHIP tripwires — derive from POLL */
CPISI_STATIC_ASSERT(CPISI_DEBOUNCE_MS == CPISI_QUOTIENT(CPISI_POLL_INTERVAL_MS, 10),
                    "DEBOUNCE_MS must equal POLL÷10 — 100 = 1000/10");
CPISI_STATIC_ASSERT(CPISI_FLUSH_MS == CPISI_PRODUCT(CPISI_POLL_INTERVAL_MS, CPISI_TERNARY),
                    "FLUSH_MS must equal POLL×TERNARY — 3000 = 1000×3");
CPISI_STATIC_ASSERT(CPISI_TIMEOUT_MS == CPISI_PRODUCT(CPISI_POLL_INTERVAL_MS, CPISI_HARDPOINTS),
                    "TIMEOUT_MS must equal POLL×HARDPOINTS — 5000 = 1000×5");
CPISI_STATIC_ASSERT(CPISI_HALF_POLL_MS == CPISI_HALF(CPISI_POLL_INTERVAL_MS),
                    "HALF_POLL_MS must equal HALF(POLL) — 499 = (1000-1)/2");

/* EQUIVALENCE tripwires — interval indices match primitives */
CPISI_STATIC_ASSERT(CPISI_INTERVAL_DEBOUNCE == CPISI_VOID,
                    "INTERVAL_DEBOUNCE must equal VOID (0)");
CPISI_STATIC_ASSERT(CPISI_INTERVAL_HALF_POLL == CPISI_UNITY,
                    "INTERVAL_HALF_POLL must equal UNITY (1)");
CPISI_STATIC_ASSERT(CPISI_INTERVAL_POLL == CPISI_BINARY,
                    "INTERVAL_POLL must equal BINARY (2)");
CPISI_STATIC_ASSERT(CPISI_INTERVAL_FLUSH == CPISI_TERNARY,
                    "INTERVAL_FLUSH must equal TERNARY (3)");
CPISI_STATIC_ASSERT(CPISI_INTERVAL_TIMEOUT == CPISI_SQUARE,
                    "INTERVAL_TIMEOUT must equal SQUARE (4)");

/** @} */ /* end S.2c.4b RELATIONSHIP */

/**
 * @defgroup S2c_4c_ORDER S.2c.4c ORDER TRIPWIRES — Verify timing sequence
 * @ingroup S2c_4_TIMING
 * @brief Verify timing intervals are properly ordered.
 *
 * REQUIRED ORDERING: DEBOUNCE < POLL < TIMEOUT
 *
 * WHY THIS MATTERS:
 *   - DEBOUNCE must be shorter than POLL (can't debounce longer than poll cycle)
 *   - TIMEOUT must be longer than POLL (must allow multiple polls before timeout)
 * @{ */

/* ORDER tripwires — verify timing sequence */
CPISI_STATIC_ASSERT(CPISI_DEBOUNCE_MS < CPISI_POLL_INTERVAL_MS,
                    "DEBOUNCE < POLL — 100 < 1000");
CPISI_STATIC_ASSERT(CPISI_POLL_INTERVAL_MS < CPISI_TIMEOUT_MS,
                    "POLL < TIMEOUT — 1000 < 5000");
CPISI_STATIC_ASSERT(CPISI_FLUSH_MS < CPISI_TIMEOUT_MS,
                    "FLUSH < TIMEOUT — 3000 < 5000");

/** @} */ /* end S.2c.4c ORDER */

/**
 * @defgroup S2c_4d_RETENTION S.2c.4d RETENTION TRIPWIRES — Verify retention values
 * @ingroup S2c_4_TIMING
 * @brief Verify 90-day recent and 180-day archive retention.
 *
 * RETENTION POLICY:
 *   - RECENT = 90 days (3 months of immediate access)
 *   - ARCHIVE = 180 days = RECENT × 2 (6 months total)
 * @{ */

/* VALUE tripwires — retention days */
CPISI_STATIC_ASSERT(CPISI_RETENTION_RECENT == 90,
                    "RETENTION_RECENT must be 90 days");
CPISI_STATIC_ASSERT(CPISI_RETENTION_ARCHIVE == 180,
                    "RETENTION_ARCHIVE must be 180 days");

/* RELATIONSHIP tripwires — archive derives from recent */
CPISI_STATIC_ASSERT(CPISI_RETENTION_ARCHIVE == CPISI_PRODUCT(CPISI_RETENTION_RECENT, CPISI_BINARY),
                    "RETENTION_ARCHIVE must equal RECENT×BINARY — 180 = 90×2");

/** @} */ /* end S.2c.4d RETENTION */
/** @} */ /* end S.2c.4 TIMING */

/** @} */ /* end S.2c TIMING VERIFY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d LOOKUP — Runtime lookup tables [LOOKUP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_timing_lookup S.2d LOOKUP — Runtime lookup tables
 * @ingroup cpisi_timing_domain
 * @brief Pre-computed tables for O(1) timing interval operations.
 *
 * PURPOSE: Container for timing domain lookup tables.
 *
 * STRUCTURE (mirrors types.h S.2d pattern: Domain → a/b/c):
 *   S.2d.6 TIMING — Timing domain tables
 *     S.2d.6a TABLES     — 2 arrays (INTERVAL_MS[5], INTERVAL_NAMES[5])
 *     S.2d.6b ACCESSORS  — 2 macros (cpisi_interval_ms, cpisi_interval_name)
 *     S.2d.6c CONVERSION — [Reserved: intervals not converted, only looked up]
 *
 * Builds FROM: S.2c TIMING VERIFY (structure verified)
 * Builds TO:   S.2e TIMING TYPES (enum definitions)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup cpisi_timing_tables S.2d.6 TIMING — Timing domain lookup
 * @ingroup cpisi_timing_lookup
 * @brief Runtime tables for timing intervals.
 *
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  THE 5 TIMING INTERVALS — Standard intervals for health monitoring        ║
 * ╠═══════════════════════════════════════════════════════════════════════════╣
 * ║  INDEX   NAME         MS      PURPOSE                                     ║
 * ║  ──────  ───────────  ──────  ──────────────────────────────────────────  ║
 * ║  [0]     debounce     100     Settle time before acting on change         ║
 * ║  [1]     half-poll    499     Half of standard poll (offset timing)       ║
 * ║  [2]     poll         1000    Standard health check interval              ║
 * ║  [3]     flush        3000    Buffer flush interval                       ║
 * ║  [4]     timeout      5000    Operation timeout threshold                 ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 *
 * STRUCTURE (template pattern: a=TABLES, b=ACCESSORS, c=CONVERSION):
 *   S.2d.6a TABLES     — 2 arrays: INTERVAL_MS[5], INTERVAL_NAMES[5]
 *   S.2d.6b ACCESSORS  — 2 macros: cpisi_interval_ms(), cpisi_interval_name()
 *   S.2d.6c CONVERSION — [Reserved: intervals not converted, only looked up]
 *
 * @ingroup S2d_LOOKUP
 * @{ */

/** @name S.2d.6a TABLES — Timing lookup arrays
 *  @brief Two arrays for timing interval lookup.
 *
 *  INTERVAL ARRAYS (5 entries each, indexed 0-4):
 *    CPISI_INTERVAL_MS[5]    — Millisecond values (100, 499, 1000, 3000, 5000)
 *    CPISI_INTERVAL_NAMES[5] — Name strings ("debounce"..."timeout")
 *
 *  @{ */

/**
 * @brief Interval milliseconds lookup — maps index (0-4) to milliseconds.
 *
 * BOUNDS USED:
 *   - CPISI_INTERVAL_COUNT = 5 — array size
 *   - CPISI_DEBOUNCE_MS, CPISI_HALF_POLL_MS, etc. — source values
 *
 * 5-INTERVAL TIMING (standard health monitoring intervals):
 *   [0] debounce  = 100ms  — settle time before acting on state change
 *   [1] half-poll = 499ms  — offset timing to avoid sync issues
 *   [2] poll      = 1000ms — standard health check interval (1 second)
 *   [3] flush     = 3000ms — buffer flush / batch commit interval
 *   [4] timeout   = 5000ms — operation timeout threshold
 *
 * @par Semantic Meaning
 *      Millisecond values for timer configuration and health monitoring.
 *      Poll (2) at center is the standard interval; debounce (0) for stability,
 *      timeout (4) for failure detection.
 *
 * @see cpisi_interval_ms — accessor macro for this table
 */
static const int CPISI_INTERVAL_MS[CPISI_INTERVAL_COUNT] = {
    CPISI_DEBOUNCE_MS,      /* [0] debounce   = 100ms  */
    CPISI_HALF_POLL_MS,     /* [1] half-poll  = 499ms  */
    CPISI_POLL_INTERVAL_MS, /* [2] poll       = 1000ms */
    CPISI_FLUSH_MS,         /* [3] flush      = 3000ms */
    CPISI_TIMEOUT_MS        /* [4] timeout    = 5000ms */
};

/**
 * @brief Interval names lookup — maps index (0-4) to name string.
 *
 * BOUNDS USED:
 *   - CPISI_INTERVAL_COUNT = 5 — array size
 *
 * 5-INTERVAL NAMES (parallel to INTERVAL_MS):
 *   [0] "debounce"  — 100ms settle
 *   [1] "half-poll" — 499ms offset
 *   [2] "poll"      — 1000ms standard
 *   [3] "flush"     — 3000ms batch
 *   [4] "timeout"   — 5000ms limit
 *
 * @par Semantic Meaning
 *      Human-readable interval names for logging, debugging, and UI display.
 *      Same index as INTERVAL_MS — use together for complete interval info.
 *
 * @see cpisi_interval_name — accessor macro for this table
 */
static const char* const CPISI_INTERVAL_NAMES[CPISI_INTERVAL_COUNT] = {
    "debounce",  /* [0] 100ms settle    */
    "half-poll", /* [1] 499ms offset    */
    "poll",      /* [2] 1000ms standard */
    "flush",     /* [3] 3000ms batch    */
    "timeout"    /* [4] 5000ms limit    */
};

/** @} */ /* end S.2d.6a TABLES */

/** @name S.2d.6b ACCESSORS — Timing table access macros
 *  @brief Type-safe access macros for timing lookup tables.
 *
 *  PATTERN: Each accessor provides direct array access without bounds check.
 *           Caller must ensure valid index range.
 *
 *  ACCESSORS (2 total):
 *    cpisi_interval_ms(idx)   — Interval milliseconds (index 0-4)
 *    cpisi_interval_name(idx) — Interval name string (index 0-4)
 *
 *  @{ */

/**
 * @brief Get interval milliseconds — INDEX (0-4) → MS value.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_INTERVAL_COUNT-1] = [0, 4]
 *   - OUTPUT: int ∈ {100, 499, 1000, 3000, 5000}
 *
 * @par Derivation
 *      Direct index into CPISI_INTERVAL_MS[idx].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 4.
 *
 * @par Semantic Meaning
 *      Millisecond value for timer configuration. Use poll (2) for standard
 *      health checks, timeout (4) for failure detection thresholds.
 *
 * EXAMPLE:
 *   cpisi_interval_ms(0) → 100  (debounce)
 *   cpisi_interval_ms(2) → 1000 (poll)
 *   cpisi_interval_ms(4) → 5000 (timeout)
 *
 * @param idx Interval index 0-4 (unchecked)
 *
 * @see CPISI_INTERVAL_MS — source table
 */
#define cpisi_interval_ms(idx)   (CPISI_INTERVAL_MS[(idx)])

/**
 * @brief Get interval name — INDEX (0-4) → NAME string.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_INTERVAL_COUNT-1] = [0, 4]
 *   - OUTPUT: const char* ∈ {"debounce"..."timeout"}
 *
 * @par Derivation
 *      Direct index into CPISI_INTERVAL_NAMES[idx].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 4.
 *
 * @par Semantic Meaning
 *      Human-readable interval name for logging, debugging, and display.
 *      Parallel to INTERVAL_MS — same index gives matching name and value.
 *
 * EXAMPLE:
 *   cpisi_interval_name(0) → "debounce"
 *   cpisi_interval_name(2) → "poll"
 *   cpisi_interval_name(4) → "timeout"
 *
 * @param idx Interval index 0-4 (unchecked)
 *
 * @see CPISI_INTERVAL_NAMES — source table
 */
#define cpisi_interval_name(idx) (CPISI_INTERVAL_NAMES[(idx)])

/** @} */ /* end S.2d.6b ACCESSORS */

/** @name S.2d.6c CONVERSION — Timing conversion utilities
 *  @brief [Reserved: Timing intervals are looked up, not converted between formats]
 *
 *  Timing intervals use direct table lookup (S.2d.6a) via accessor macros (S.2d.6b).
 *  No conversion utilities needed — intervals map directly to indices.
 *
 *  FUTURE: If string→interval parsing needed, add here.
 *  @{ */

/* No conversion utilities — intervals use direct lookup */

/** @} */ /* end S.2d.6c CONVERSION */

/** @} */ /* end S.2d.6 TIMING */

/** @} */ /* end S.2d LOOKUP */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e TYPES — Timing domain type definitions [TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_timing_types S.2e TIMING TYPES — Enumerations
 * @ingroup cpisi_timing_domain
 * @brief CpisiInterval enumeration.
 *
 * PURPOSE: Type definitions for timing domain.
 *
 * CONTAINS:
 *   S.2e.4 TIMING TYPES — CpisiInterval enumeration (5 values)
 *
 * Builds FROM: S.2d LOOKUP (runtime tables)
 * Builds TO:   S.3 CLOSING (file end)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2e_4_TIMING S.2e.4 TIMING TYPES — CpisiInterval
 * @brief Type-safe timing interval enumeration — 5 standard durations.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  S.2e.4 TIMING — Interval Enumeration (1 type, 5 values)                   │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  SEMANTIC FOUNDATION — 5 Timing Intervals:                                 │
 * │                                                                            │
 * │    DEBOUNCE  (0) —  100ms — input settle time                              │
 * │    HALF_POLL (1) —  499ms — half-second check                              │
 * │    POLL      (2) — 1000ms — standard poll interval ← CENTER                │
 * │    FLUSH     (3) — 3000ms — buffer flush interval                          │
 * │    TIMEOUT   (4) — 5000ms — operation timeout                              │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.6):                                          │
 * │    - CPISI_INTERVAL_MS[5] — millisecond values                             │
 * │    - CPISI_INTERVAL_NAMES[5] — human-readable names                        │
 * │    - cpisi_interval_ms(), cpisi_interval_name() accessors                  │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup cpisi_timing_types
 * @{ */

/**
 * @brief Interval enumeration — timing indices for standard durations.
 *
 * BOUNDS USED:
 *   - CPISI_INTERVAL_DEBOUNCE (0) through CPISI_INTERVAL_TIMEOUT (4) — S.2a.1
 *   - CPISI_INTERVAL_COUNT (5) — iteration bound
 *   - CPISI_INTERVAL_POLL (2) — CENTER, standard polling interval
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for standard timing intervals. POLL (2) at center provides
 *   balanced default (1000ms). Values scale roughly logarithmically from 100ms
 *   to 5000ms, covering common operational timing needs.
 *
 * VALUE MAPPING:
 *   CPISI_INTV_DEBOUNCE (0) — 100ms, input settle/debounce time
 *   CPISI_INTV_HALF_POLL (1) — 499ms, half-second quick check
 *   CPISI_INTV_POLL (2) — 1000ms, standard poll interval (CENTER)
 *   CPISI_INTV_FLUSH (3) — 3000ms, buffer flush interval
 *   CPISI_INTV_TIMEOUT (4) — 5000ms, operation timeout limit
 *
 * USE CASE:
 *   - Timer configuration: cpisi_set_timer(CpisiInterval intv);
 *   - Duration lookup: ms = cpisi_interval_ms(interval);
 *   - Timeout selection: timeout = CPISI_INTV_TIMEOUT for operations
 *
 * RELATIONSHIP TO LOOKUP (S.2d.6):
 *   - cpisi_interval_ms(idx) → 100, 499, 1000, 3000, 5000
 *   - cpisi_interval_name(idx) → "DEBOUNCE", "HALF_POLL", ..., "TIMEOUT"
 */
typedef enum CpisiInterval {
    CPISI_INTV_DEBOUNCE  = CPISI_INTERVAL_DEBOUNCE,  /**< 0: 100ms — input settle/debounce time */
    CPISI_INTV_HALF_POLL = CPISI_INTERVAL_HALF_POLL, /**< 1: 499ms — half-second quick check */
    CPISI_INTV_POLL      = CPISI_INTERVAL_POLL,      /**< 2: 1000ms — standard poll (CENTER) */
    CPISI_INTV_FLUSH     = CPISI_INTERVAL_FLUSH,     /**< 3: 3000ms — buffer flush interval */
    CPISI_INTV_TIMEOUT   = CPISI_INTERVAL_TIMEOUT    /**< 4: 5000ms — operation timeout limit */
} CpisiInterval;

/** @} */ /* end S.2e.4 TIMING */

/** @} */ /* end S.2e TYPES */

/** @} */ /* End cpisi_timing_domain */

/** S.3 TYPES — [Reserved: timing types in S.2e] @defgroup cpisi_timing_s3_types @ingroup cpisi_timing_domain @{ @} */

/** S.4 PROTOTYPES — [Reserved: timing accessors inline in S.2d] @defgroup cpisi_timing_prototypes @ingroup cpisi_timing_domain @{ @} */

/** S.5 FILE-LEVEL — [Reserved: no file-level state] @defgroup cpisi_timing_file @ingroup cpisi_timing_domain @{ @} */

/** S.6 OMISSION — [Reserved: S.3=S.2e, S.4=S.2d, S.5=none] @defgroup cpisi_timing_omission @ingroup cpisi_timing_domain @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief [Reserved: all timing logic in SETUP S.2]
///
/// ═══════════════════════════════════════════════════════════════════════════════

/** B.1 ORG CHART — [Reserved: types/functions in SETUP] @defgroup cpisi_timing_orgchart @{ @} */
/** B.2 HELPERS — [Reserved: macros in S.2b, inline in S.2d] @defgroup cpisi_timing_helpers @{ @} */
/** B.3 RESERVED — [Reserved: inherits types.h B.3] @defgroup cpisi_timing_b3_reserved @{ @} */
/** B.4 RESERVED — [Reserved: inherits types.h B.4] @defgroup cpisi_timing_b4_reserved @{ @} */
/** B.5 RESERVED — [Reserved: inherits types.h B.5] @defgroup cpisi_timing_b5_reserved @{ @} */
/** B.6 RESERVED — [Reserved: inherits types.h B.6] @defgroup cpisi_timing_b6_reserved @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Guidance and documentation for maintaining this file.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Compile-time Verification [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_validation X.1 VALIDATION — Compile-time Verification
 * @{
 *
 * VALIDATION NOTES:
 *   - Static assertions in S.2c VERIFY catch timing errors at compile time
 *   - All timing intervals verified against POLL anchor (1000ms)
 *   - Retention periods verified (90/180 day relationship)
 *   - If validation fails, check timing derivation chain
 *
 * @} end X.1 VALIDATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Post-include Checks [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_execution X.2 EXECUTION — Post-include Checks
 * @{
 *
 * POST-INCLUDE CHECKS:
 *   - Verify config.h included (provides types.h transitively)
 *   - All timing macros resolve to expected values
 *   - Lookup tables accessible via accessor macros
 *
 * @} end X.2 EXECUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Resource Management [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_cleanup X.3 CLEANUP — Resource Management
 * @{
 *
 * CLEANUP NOTES:
 *   - No dynamic resources — timing is compile-time/static only
 *   - Lookup tables are static const — no cleanup needed
 *   - Future: If dynamic timing added, document cleanup here
 *
 * @} end X.3 CLEANUP
 */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_policy X.4 POLICY — Modification Guidance
 * @{
 *
 * MODIFICATION POLICY:
 *   - All timing derives from POLL_INTERVAL_MS (1000ms) anchor
 *   - Adding intervals: derive from POLL, add tripwires in S.2c
 *   - Changing POLL: requires updating all derived values
 *   - Template: bereshit/word/seed/code/c/type-header.h
 *
 * @} end X.4 POLICY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — Future Expansion [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_extension X.5 EXTENSION — Future Expansion
 * @{
 *
 * EXTENSION POINTS:
 *   - New intervals: Add to S.2b.5a, S.2c.4a, S.2d.6a lookup tables
 *   - Timezone support: Add S.2e.5 TIMEZONE types
 *   - Scheduling utilities: Add to BODY when needed
 *   - Always add tripwires for new timing values
 *
 * @} end X.5 EXTENSION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Issues [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_troubleshoot X.6 TROUBLESHOOT — Common Issues
 * @{
 *
 * COMMON ISSUES:
 *   Q: "Static assert failed on timing interval"
 *   A: Check POLL_INTERVAL_MS = 1000, verify derivation math
 *
 *   Q: "Interval index out of range"
 *   A: Use CPISI_INTERVAL_VALID() before table access
 *
 *   Q: "Retention validation failing"
 *   A: Ensure days >= CPISI_RETENTION_RECENT (90)
 *
 * @} end X.6 TROUBLESHOOT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Related Documentation [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_reference X.7 REFERENCE — Related Documentation
 * @{
 *
 * RELATED FILES:
 *   - types.h: Foundation types (BOUNDS, PRIMITIVES)
 *   - config/config.h: Tier 1 config anchor (includes types.h)
 *   - flags.h: Domain flags (similar structure)
 *   - layer.h: Domain layers (similar structure)
 *
 * TEMPLATE: bereshit/word/seed/code/c/type-header.h
 *
 * @} end X.7 REFERENCE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Implementation Notes [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_note X.8 NOTE — Implementation Notes
 * @{
 *
 * IMPLEMENTATION NOTES:
 *   - POLL (1000ms) is THE timing anchor — everything derives from it
 *   - Lookup tables are static const for O(1) access
 *   - Tripwires verify both VALUE and RELATIONSHIP
 *   - Domain inherits from types.h via config/config.h
 *
 * @} end X.8 NOTE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Intentional Exclusions [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_timing_x9_omission X.9 OMISSION — Intentional Exclusions
 * @{
 *
 * INTENTIONAL EXCLUSIONS:
 *   - No dynamic timing: All timing is compile-time constants
 *   - No timezone handling: Future extension (see X.5)
 *   - No scheduling utilities: BODY reserved until needed
 *   - S.3-S.6 reserved: Timing logic complete in S.2
 *
 * @} end X.9 OMISSION
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_TYPES_TIMING_H */
