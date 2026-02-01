/// @file health.h
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
/// @omni:req  key        = CPISI-TYPES-HEALTH
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
/// @omni:ins  layer      = types/health
/// @omni:ins  includes   = types.h
/// @omni:ins  provides   = [S.2_HEALTH]
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
/// @omni:req  title      = CPI-SI Health Domain
/// @omni:req  brief      = Health states, trends, and severity metrics
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
 * @defgroup cpisi_health_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup cpisi_health_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_health_special_config
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

#ifndef CPISI_TYPES_HEALTH_H
#define CPISI_TYPES_HEALTH_H

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
 * @file      health.h
 * @brief     CPI-SI Health Domain — Health states, trends, severity levels.
 *
 * @defgroup cpisi_health_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-HEALTH
 * Title:     CPI-SI Health Domain Types
 * Type:      Header (Domain)
 * Component: Rails — provides health types for system monitoring
 * Role:      Health state, trend, and severity type definitions.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_state M.2 STATE — Lifecycle State
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
 * @defgroup cpisi_health_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_health_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/health/health.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: types/types.h (foundation), types/config/config.h
 * Template:     bereshit/word/seed/code/c/type-header.h
 * Extracted:    2026-01-21 from types.h health sections
 *
 * Builds FROM:  types.h, config/config.h, error/error.h
 * Builds TO:    cpisi.h, dar.h, health monitoring systems
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, health, domain, state, trend, severity, layer-4
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide health monitoring types for Layer 4 (CENTER) operations.
 * Core Design: Hebrew state model for health assessment.
 * Philosophy:  Health observation enables timely intervention.
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  3 John 1:2 — "I wish above all that thou mayest prosper and be in health"
 * Principle:  Health observation enables timely intervention.
 * Anchor:     Proverbs 4:20-22 — "attend to my words... they are life... and health"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h, config/config.h, error/error.h
 *
 * What Uses This:
 *   - cpisi.h — main interface
 *   - dar.h — data access records
 *   - Health monitoring systems
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
 * @defgroup cpisi_health_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-21) — Extracted from types.h health sections
 *   a-01.25 (2026-01-23) — Expanded METADATA to full M.1-M.10 structure
 *   b-03.00 (2026-01-25) — Template alignment: type-header.h, CLOSING X.1-X.9
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ Health state types defined
 *
 * Planned:
 *   ⏳ Health trend analysis functions
 *   ⏳ Health report generation
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
///   - S.2 DEFINES   — Health domain (Bounds, Macros, Verify, Lookup, Types)
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
 * @defgroup cpisi_health_includes S.1 INCLUDES — Header Dependencies
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

/** S.1a STANDARD — [Inherited from types.h S.1a] @defgroup cpisi_health_includes_standard @ingroup cpisi_health_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_includes_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_health_includes
 * @brief    Internal CPI-SI headers this file depends on.
 *
 * PURPOSE: Include foundation header providing S.1 system headers and S.2 defines.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/config/cfg/domain.cfg.h"  /**< TIER 1: config anchors (brings types.h transitively) */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [None required] @defgroup cpisi_health_includes_external @ingroup cpisi_health_includes @{ @} */

/** S.1d CPP — [Inherited from types.h S.1d] @defgroup cpisi_health_includes_cpp @ingroup cpisi_health_includes @{ @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Health Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_domain S.2 DEFINES — Health Domain
 * @brief    Health pipeline types — state, severity, trend, and derived values.
 *
 * PURPOSE: Define health-specific constants, operations, verification, and types.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: no domain-specific preludes]
 *   - S.2a  BOUNDS  — Health counts and capacities
 *   - S.2b  MACROS  — Health operations
 *   - S.2c VERIFY  — Health compile-time assertions
 *   - S.2d LOOKUP  — Health runtime tables
 *   - S.2e TYPES   — Health enumerations
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  HEALTH DOMAIN — DAR Pipeline Architecture                                 │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │                                                                             │
 * │  HEBREW STATES (7):  shavar → chaser/ratsah → yashar → tamim/shalem → tov  │
 * │  SEVERITY (5):       NONE → LOW → MEDIUM → HIGH → CRITICAL                 │
 * │  TREND (3):          DECLINING (-1) → STABLE (0) → IMPROVING (+1)          │
 * │                                                                             │
 * │  PIPELINE LAYERS (4): KERNEL → LAYER → ZONE → HEALTH                       │
 * │    Each with 9 positions (FACE = 3²)                                        │
 * │                                                                             │
 * │  DERIVED VALUES:                                                            │
 * │    - Milestones (6 types × 9 layers)                                        │
 * │    - Weights    (4 layers × 9 positions)                                    │
 * │    - Halves     (4 layers × 9 positions)                                    │
 * │    - Zones      (5 zones × 9 positions)                                     │
 * │    - Impact     (severity ↔ impact conversions)                             │
 * │    - Normalization (k-factor, positions, divisors)                          │
 * │                                                                             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * Scripture: "Beloved, I wish above all things that thou mayest prosper and be
 *            in health, even as thy soul prospereth." — 3 John 1:2
 *
 * Builds FROM: S.1 INCLUDES (foundation dependencies)
 * Builds TO:   S.3 CLOSING (file end)
 *
 * @{
 */

/** S.2.0 PRELUDE — [Reserved: inherits types.h] @defgroup cpisi_health_prelude @ingroup cpisi_health_domain @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BOUNDS — Health domain constants [BOUNDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_bounds S.2a HEALTH BOUNDS — Domain Constants
 * @ingroup cpisi_health_domain
 * @brief    Health counts, capacities, and pipeline bounds.
 *
 * CONTAINS:
 *   - Counts — HEALTH_COUNT (7), TREND_COUNT (3), SEVERITY_COUNT (5)
 *   - Pipeline — Layer transformation bounds
 *   - Milestone — 9 milestone bounds
 *   - Ternary — Signed values for Hebrew states
 *
 * Builds FROM: S.2.0 PRELUDE
 * Builds TO:   S.2b MACROS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Counts — Health domain array sizes
 *  @brief HEALTH_COUNT (7 states), TREND_COUNT (3), SEVERITY_COUNT (5).
 *  @{ */
#define CPISI_HEALTH_COUNT          CPISI_STATES        /**< 7: health states (shavar→tov)        */
#define CPISI_TREND_COUNT           CPISI_SIGN_COUNT    /**< 3: trend directions (-1, 0, +1)      */
#define CPISI_SEVERITY_ROW_COUNT    CPISI_TERNARY       /**< 3: severity matrix rows              */
#define CPISI_SEVERITY_COUNT        CPISI_HARDPOINTS    /**< 5: severity array size               */
/** @} */ /* end Counts */

/** @name Pipeline — 4-layer health value transformation
 *  @brief RAW(0-255) → BALANCED(-128..+127) → TERNARY(-1..+1) → DISPLAY(-100..+100).
 *
 *  | Layer | Range | Purpose |
 *  |-------|-------|---------|
 *  | RAW | 0-255 | Storage (byte) |
 *  | BALANCED | -128..+127 | Signed arithmetic |
 *  | TERNARY | -1.0..+1.0 | Normalized computation |
 *  | DISPLAY | -100..+100 | Human-readable output |
 *  @{ */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2a.5b.2a PIPELINE — 4-layer health value transformation
 *
 * RAW(0-255) → BALANCED(-128..+127) → TERNARY(-1..+1) → DISPLAY(-100..+100)
 *
 * Asymmetric edges: Corruption path (128 steps) > Restoration path (127 steps)
 * This reflects that breaking is easier than building.
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @brief -1: minimum trend — declining (shavar).
 * @par Derivation
 *      TREND_MIN = NEGATIVE = BAL_MIN_OF(TREND_COUNT) = -1
 */
#define CPISI_TREND_MIN         CPISI_NEGATIVE
/**
 * @brief 0: stable trend — center (yashar).
 * @par Derivation
 *      TREND_STABLE = NEUTRAL = BAL_CENTER_OF(TREND_COUNT) = 0
 */
#define CPISI_TREND_STABLE      CPISI_NEUTRAL
/**
 * @brief +1: maximum trend — improving (tov).
 * @par Derivation
 *      TREND_MAX = POSITIVE = BAL_MAX_OF(TREND_COUNT) = +1
 */
#define CPISI_TREND_MAX         CPISI_POSITIVE

/* RAW LAYER — byte storage (0-255) */
#define CPISI_HEALTH_POSITIONS      CPISI_BINARY_FIT                              /**< 256: total positions           */
#define CPISI_HEALTH_RAW_HALF       CPISI_HALF(CPISI_HEALTH_RAW_MAX)              /**< 127: half of max               */
#define CPISI_HEALTH_RAW_LAYER_SIZE CPISI_QUOTIENT(CPISI_HEALTH_POSITIONS, CPISI_FACE) /**< 28: ~positions/layer      */
/**
 * @brief 0: minimum RAW health — shavar edge.
 * @par Derivation
 *      RAW_MIN = VOID = MIN_OF(HEALTH_POSITIONS) = 0
 */
#define CPISI_HEALTH_RAW_MIN        CPISI_VOID
/**
 * @brief 128: center RAW health — yashar anchor.
 * @par Derivation
 *      RAW_CENTER = QUOTIENT(256, 2) = CENTER_OF(HEALTH_POSITIONS) = 128
 */
#define CPISI_HEALTH_RAW_CENTER     CPISI_QUOTIENT(CPISI_BINARY_FIT, CPISI_BINARY)
/**
 * @brief 255: maximum RAW health — tov edge.
 * @par Derivation
 *      RAW_MAX = DIFF(256, 1) = MAX_OF(HEALTH_POSITIONS) = 255
 */
#define CPISI_HEALTH_RAW_MAX        CPISI_DIFF(CPISI_BINARY_FIT, CPISI_UNITY)

/* EDGE DISTANCES — asymmetric: 128 steps down, 127 steps up */
#define CPISI_HEALTH_EDGE_NEG       CPISI_HEALTH_RAW_CENTER                       /**< 128: center→shavar           */
#define CPISI_HEALTH_EDGE_POS       CPISI_DIFF(CPISI_HEALTH_RAW_MAX, CPISI_HEALTH_RAW_CENTER) /**< 127: center→tov */

/* BALANCED LAYER — signed arithmetic (-128..+127) */
#define CPISI_HEALTH_BAL_HALF       CPISI_HALF(CPISI_HEALTH_BAL_MAX)              /**< 63: half of max                */
/**
 * @brief -128: minimum BALANCED health — shavar edge.
 * @par Derivation
 *      BAL_MIN = NEGATE(EDGE_NEG) = -128
 */
#define CPISI_HEALTH_BAL_MIN        CPISI_NEGATE(CPISI_HEALTH_EDGE_NEG)
/**
 * @brief 0: center BALANCED health — yashar anchor.
 * @par Derivation
 *      BAL_CENTER = VOID = 0
 */
#define CPISI_HEALTH_BAL_CENTER     CPISI_VOID
/**
 * @brief +127: maximum BALANCED health — tov edge.
 * @par Derivation
 *      BAL_MAX = EDGE_POS = 127
 */
#define CPISI_HEALTH_BAL_MAX        CPISI_HEALTH_EDGE_POS

/* TERNARY LAYER — normalized computation (-1.0..+1.0) */
#define CPISI_HEALTH_DIVISOR_NEG    ((float)CPISI_HEALTH_EDGE_NEG)                /**< 128.0f for normalization       */
#define CPISI_HEALTH_DIVISOR_POS    ((float)CPISI_HEALTH_EDGE_POS)                /**< 127.0f for normalization       */
/**
 * @brief -1.0f: minimum TERNARY health — shavar edge.
 * @par Derivation
 *      TERN_MIN = -1.0f = BAL_MIN / DIVISOR_NEG = -128/128
 */
#define CPISI_HEALTH_TERN_MIN       (-1.0f)
/**
 * @brief 0.0f: center TERNARY health — yashar anchor.
 * @par Derivation
 *      TERN_CENTER = 0.0f = BAL_CENTER / DIVISOR = 0/128
 */
#define CPISI_HEALTH_TERN_CENTER    (0.0f)
/**
 * @brief +1.0f: maximum TERNARY health — tov edge.
 * @par Derivation
 *      TERN_MAX = +1.0f = BAL_MAX / DIVISOR_POS = 127/127
 */
#define CPISI_HEALTH_TERN_MAX       (1.0f)

/* DISPLAY LAYER — human-readable output (-100..+100) */
#define CPISI_HEALTH_DISPLAY_RANGE  CPISI_SUM(CPISI_PRODUCT(CPISI_HEALTH_DISPLAY_MAX, CPISI_BINARY), CPISI_UNITY) /**< 201 positions   */
#define CPISI_HEALTH_DISPLAY_HALF   CPISI_HALF(CPISI_HEALTH_DISPLAY_MAX)          /**< 49: half of max                */
#define CPISI_HEALTH_DISPLAY_DIVISOR ((float)CPISI_HEALTH_DISPLAY_MAX)            /**< 100.0f for conversion          */
/**
 * @brief -100: minimum DISPLAY health — shavar edge.
 * @par Derivation
 *      DISPLAY_MIN = NEGATE(100) = TERN_MIN × 100 = -100
 */
#define CPISI_HEALTH_DISPLAY_MIN    CPISI_NEGATE(100)
/**
 * @brief 0: center DISPLAY health — yashar anchor.
 * @par Derivation
 *      DISPLAY_CENTER = VOID = TERN_CENTER × 100 = 0
 */
#define CPISI_HEALTH_DISPLAY_CENTER CPISI_VOID
/**
 * @brief +100: maximum DISPLAY health — tov edge.
 * @par Derivation
 *      DISPLAY_MAX = 100 = TERN_MAX × 100 = +100
 */
#define CPISI_HEALTH_DISPLAY_MAX    100
/** @} */ /* end Pipeline */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2a.5b.2b MILESTONE — 9 milestone bounds and step
 *
 * Milestones divide DISPLAY range into 9 positions: -4, -3, -2, -1, 0, +1, +2, +3, +4
 * STEP = DISPLAY_MAX / FACE_CENTER = 100 / 4 = 25
 * ───────────────────────────────────────────────────────────────────────────── */
#define CPISI_MILESTONE_COUNT       CPISI_FACE                                    /**< 9: milestones = modes        */
#define CPISI_MILESTONE_MIN         CPISI_NEGATE(CPISI_FACE_CENTER)               /**< -4: min milestone            */
#define CPISI_MILESTONE_CENTER      CPISI_VOID                                    /**< 0: center milestone          */
#define CPISI_MILESTONE_MAX         CPISI_FACE_CENTER                             /**< +4: max milestone            */
#define CPISI_MILESTONE_HALF        CPISI_HALF(CPISI_MILESTONE_MAX)               /**< 2: half of max               */
#define CPISI_MILESTONE_STEP        CPISI_QUOTIENT(CPISI_HEALTH_DISPLAY_MAX, CPISI_FACE_CENTER) /**< 25: 100/4     */
#define CPISI_MILESTONE_DOUBLE      CPISI_PRODUCT(CPISI_MILESTONE_STEP, CPISI_BINARY) /**< 50: 2×STEP              */

/* MILESTONE DISPLAY — mapped display values at each milestone */
#define CPISI_MILESTONE_DISPLAY_MIN     CPISI_HEALTH_DISPLAY_MIN                  /**< -100: milestone -4           */
#define CPISI_MILESTONE_DISPLAY_CENTER  CPISI_HEALTH_DISPLAY_CENTER               /**< 0: milestone 0               */
#define CPISI_MILESTONE_DISPLAY_MAX     CPISI_HEALTH_DISPLAY_MAX                  /**< +100: milestone +4           */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2a.5b.2c TERNARY — Signed ternary values for Hebrew states
 *
 * Integer ternary: -1, 0, +1
 * Float ternary step: 1/FACE_CENTER = 0.25
 * Quarter steps map to Hebrew states at 0.25, 0.50, 0.75 thresholds
 * ───────────────────────────────────────────────────────────────────────────── */

/* INTEGER TERNARY — guarded to prevent redefinition if types.h loaded first */
#ifndef CPISI_TERNARY_MIN
#define CPISI_TERNARY_MIN           CPISI_NEGATE(CPISI_UNITY)                     /**< -1: shavar                   */
#endif
#ifndef CPISI_TERNARY_CENTER
#define CPISI_TERNARY_CENTER        CPISI_VOID                                    /**< 0: yashar                    */
#endif
#ifndef CPISI_TERNARY_MAX
#define CPISI_TERNARY_MAX           CPISI_UNITY                                   /**< +1: tov                      */
#endif

/* FLOAT TERNARY */
#define CPISI_TERNARY_MIN_F         CPISI_HEALTH_TERN_MIN                         /**< -1.0: shavar                 */
#define CPISI_TERNARY_CENTER_F      CPISI_HEALTH_TERN_CENTER                      /**< 0.0: yashar                  */
#define CPISI_TERNARY_MAX_F         CPISI_HEALTH_TERN_MAX                         /**< +1.0: tov                    */

/* TERNARY STEPS — quarter increments for Hebrew state boundaries */
#define CPISI_TERNARY_STEP_F        (1.0f / (float)CPISI_FACE_CENTER)             /**< 0.25: 1/FACE_CENTER          */
#define CPISI_TERNARY_QUARTER_F     CPISI_TERNARY_STEP_F                          /**< 0.25: 1×STEP                 */
#define CPISI_TERNARY_HALF_F        (CPISI_TERNARY_STEP_F * (float)CPISI_BINARY)  /**< 0.50: 2×STEP                 */
#define CPISI_TERNARY_THREE_Q_F     (CPISI_TERNARY_STEP_F * (float)CPISI_TERNARY) /**< 0.75: 3×STEP                 */
#define CPISI_TERNARY_BRIDGE_F      ((float)CPISI_BRIDGE / (float)CPISI_HEALTH_DISPLAY_MAX) /**< 0.13              */

/* SIGNED TERNARY — Hebrew state thresholds (guarded) */
#ifndef CPISI_TERNARY_NEG_THREE_Q
#define CPISI_TERNARY_NEG_THREE_Q   (-CPISI_TERNARY_THREE_Q_F)                    /**< -0.75: machsor               */
#endif
#ifndef CPISI_TERNARY_NEG_HALF
#define CPISI_TERNARY_NEG_HALF      (-CPISI_TERNARY_HALF_F)                       /**< -0.50: chaser                */
#endif
#ifndef CPISI_TERNARY_NEG_QUARTER
#define CPISI_TERNARY_NEG_QUARTER   (-CPISI_TERNARY_QUARTER_F)                    /**< -0.25: ratsah                */
#endif
#ifndef CPISI_TERNARY_POS_QUARTER
#define CPISI_TERNARY_POS_QUARTER   CPISI_TERNARY_QUARTER_F                       /**< +0.25: ratson                */
#endif
#ifndef CPISI_TERNARY_POS_HALF
#define CPISI_TERNARY_POS_HALF      CPISI_TERNARY_HALF_F                          /**< +0.50: tamim                 */
#endif
#ifndef CPISI_TERNARY_POS_THREE_Q
#define CPISI_TERNARY_POS_THREE_Q   CPISI_TERNARY_THREE_Q_F                       /**< +0.75: shalem                */
#endif

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2a.5b.2d WEIGHT — Layer weight bounds
 *
 * Weight(layer) = FACE - layer_index (KERNEL=9, APP=1)
 * Foundation layers have more weight; user-facing layers have less.
 * Total weight: sum(1..9) = 45
 * ───────────────────────────────────────────────────────────────────────────── */

/* WEIGHT BOUNDS */
#define CPISI_HEALTH_WEIGHT_MIN     CPISI_UNITY     /**< 1: APP (least)                    */
#define CPISI_HEALTH_WEIGHT_CENTER  CPISI_QUOTIENT(CPISI_SUM(CPISI_FACE, CPISI_UNITY), CPISI_BINARY) /**< 5: HEALTH center */
#define CPISI_HEALTH_WEIGHT_MAX     CPISI_FACE      /**< 9: KERNEL (most)                  */
#define CPISI_HEALTH_WEIGHT_HALF    CPISI_HALF(CPISI_HEALTH_WEIGHT_MAX)           /**< 4: half of max          */
#define CPISI_HEALTH_WEIGHT_TOTAL   CPISI_QUOTIENT(CPISI_PRODUCT(CPISI_FACE, CPISI_SUM(CPISI_FACE, CPISI_UNITY)), CPISI_BINARY) /**< 45: sum(1..9) */

/* WEIGHT PER LAYER — KERNEL=9 descending to APP=1 */
#define CPISI_HEALTH_WEIGHT_KERNEL    CPISI_DIFF(CPISI_FACE, CPISI_LAYER_KERNEL)    /**< 9: foundational       */
#define CPISI_HEALTH_WEIGHT_SYSTEM    CPISI_DIFF(CPISI_FACE, CPISI_LAYER_SYSTEM)    /**< 8: OS services        */
#define CPISI_HEALTH_WEIGHT_RUNTIME   CPISI_DIFF(CPISI_FACE, CPISI_LAYER_RUNTIME)   /**< 7: VM, execution      */
#define CPISI_HEALTH_WEIGHT_FRAMEWORK CPISI_DIFF(CPISI_FACE, CPISI_LAYER_FRAMEWORK) /**< 6: libraries          */
#define CPISI_HEALTH_WEIGHT_HEALTH    CPISI_DIFF(CPISI_FACE, CPISI_LAYER_HEALTH)    /**< 5: CPI-SI center      */
#define CPISI_HEALTH_WEIGHT_NETWORK   CPISI_DIFF(CPISI_FACE, CPISI_LAYER_NETWORK)   /**< 4: FaithNet           */
#define CPISI_HEALTH_WEIGHT_STORAGE   CPISI_DIFF(CPISI_FACE, CPISI_LAYER_STORAGE)   /**< 3: Bereshit           */
#define CPISI_HEALTH_WEIGHT_ENGINE    CPISI_DIFF(CPISI_FACE, CPISI_LAYER_ENGINE)    /**< 2: Cornerstone        */
#define CPISI_HEALTH_WEIGHT_APP       CPISI_DIFF(CPISI_FACE, CPISI_LAYER_APP)       /**< 1: user-facing        */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2a.5b.2e HALF — Layer half bounds
 *
 * Half(layer) = (3^layer - 1) / 2
 * Used for centering operations within each layer's ternary space.
 * ───────────────────────────────────────────────────────────────────────────── */

/* HALF BOUNDS */
#define CPISI_HALF_MIN              CPISI_VOID                                    /**< 0: KERNEL             */
#define CPISI_HALF_CENTER           CPISI_HALF(CPISI_TESSERACT)                   /**< 40: HEALTH            */
#define CPISI_HALF_MAX              CPISI_HALF(CPISI_POWER_L8)                     /**< 3280: APP             */

/* HALF PER LAYER — (3^n - 1) / 2 */
#define CPISI_HALF_KERNEL           CPISI_HALF(CPISI_POW_0)                        /**< 0: (3⁰-1)/2 = 0       */
#define CPISI_HALF_SYSTEM           CPISI_HALF(CPISI_POW_1)                        /**< 1: (3¹-1)/2 = 1       */
#define CPISI_HALF_RUNTIME          CPISI_HALF(CPISI_POW_2)                        /**< 4: (3²-1)/2 = 4       */
#define CPISI_HALF_FRAMEWORK        CPISI_HALF(CPISI_POW_3)                        /**< 13: (3³-1)/2 = 13     */
#define CPISI_HALF_HEALTH           CPISI_HALF(CPISI_POW_4)                        /**< 40: (3⁴-1)/2 = 40     */
#define CPISI_HALF_NETWORK          CPISI_HALF(CPISI_POW_5)                        /**< 121: (3⁵-1)/2 = 121   */
#define CPISI_HALF_STORAGE          CPISI_HALF(CPISI_POW_6)                        /**< 364: (3⁶-1)/2 = 364   */
#define CPISI_HALF_ENGINE           CPISI_HALF(CPISI_POW_7)                        /**< 1093: (3⁷-1)/2 = 1093 */
#define CPISI_HALF_APP              CPISI_HALF(CPISI_POWER_L8)                     /**< 3280: (3⁸-1)/2 = 3280 */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2a.5b.2f ZONE — 5 zones with display and ternary boundaries
 *
 * 5 zones map to 7 Hebrew states via 4 boundaries:
 *   INNER (±25) and OUTER (±50) boundaries divide the DISPLAY range.
 *
 * | Zone | Display Range | Hebrew State |
 * |------|---------------|--------------|
 * | CRITICAL | -100..-51 | shavar (broken) |
 * | LOW | -50..-26 | chaser/ratsah |
 * | BALANCED | -25..+25 | yashar (center) |
 * | HIGH | +26..+50 | tamim/shalem |
 * | OPTIMAL | +51..+100 | tov (perfect) |
 * ───────────────────────────────────────────────────────────────────────────── */

/* ZONE INDEX BOUNDS */
#define CPISI_ZONE_MIN              CPISI_VOID                                        /**< 0: CRITICAL           */
#define CPISI_ZONE_CENTER           CPISI_HARDPOINTS_CENTER                           /**< 2: BALANCED           */
#define CPISI_ZONE_MAX              CPISI_DIFF(CPISI_HARDPOINTS, CPISI_UNITY)         /**< 4: OPTIMAL            */
#define CPISI_ZONE_BOUNDARY_COUNT   CPISI_DIFF(CPISI_HARDPOINTS, CPISI_UNITY)         /**< 4: boundaries         */

/* ZONE COUNTS — 5→7→9 progression */
#define CPISI_ZONE_COUNT            CPISI_HARDPOINTS                                  /**< 5: zones              */
#define CPISI_STATE_COUNT           CPISI_STATES                                      /**< 7: Hebrew states      */
#define CPISI_LAYER_COUNT_HEALTH    CPISI_FACE                                        /**< 9: layers             */

/* ZONE DISPLAY DISTANCES — multiples of MILESTONE_STEP */
#define CPISI_ZONE_INNER            CPISI_MILESTONE_STEP                              /**< 25: 1×STEP            */
#define CPISI_ZONE_OUTER            CPISI_PRODUCT(CPISI_MILESTONE_STEP, CPISI_HARDPOINTS_CENTER) /**< 50: 2×STEP */
#define CPISI_ZONE_THREE_Q          CPISI_PRODUCT(CPISI_MILESTONE_STEP, CPISI_DIFF(CPISI_FACE_CENTER, CPISI_UNITY)) /**< 75: 3×STEP */
#define CPISI_ZONE_EDGE             CPISI_PRODUCT(CPISI_MILESTONE_STEP, CPISI_FACE_CENTER) /**< 100: 4×STEP       */

/* ZONE DISPLAY BOUNDARIES — signed thresholds */
#define CPISI_ZONE_BROKEN           CPISI_NEGATE(CPISI_ZONE_OUTER)                    /**< -50: shavar           */
#define CPISI_ZONE_LOWER            CPISI_NEGATE(CPISI_ZONE_INNER)                    /**< -25: chaser/ratsah    */
#define CPISI_ZONE_UPPER            CPISI_ZONE_INNER                                  /**< +25: tamim/shalem     */
#define CPISI_ZONE_PERFECT          CPISI_ZONE_OUTER                                  /**< +50: tov              */

/* ZONE TERNARY DISTANCES — float equivalents */
#define CPISI_TERNARY_ZONE_INNER_F    CPISI_TERNARY_STEP_F                            /**< 0.25: 1×STEP          */
#define CPISI_TERNARY_ZONE_OUTER_F    (CPISI_TERNARY_STEP_F * (float)CPISI_HARDPOINTS_CENTER) /**< 0.50: 2×STEP */
#define CPISI_TERNARY_ZONE_THREE_Q_F  (CPISI_TERNARY_STEP_F * (float)CPISI_DIFF(CPISI_FACE_CENTER, CPISI_UNITY)) /**< 0.75 */
#define CPISI_TERNARY_ZONE_EDGE_F     (CPISI_TERNARY_STEP_F * (float)CPISI_FACE_CENTER) /**< 1.00: 4×STEP        */

/* ZONE TERNARY BOUNDARIES — signed float thresholds */
#define CPISI_TERNARY_ZONE_BROKEN_F   (-CPISI_TERNARY_ZONE_OUTER_F)                   /**< -0.50                 */
#define CPISI_TERNARY_ZONE_LOWER_F    (-CPISI_TERNARY_ZONE_INNER_F)                   /**< -0.25                 */
#define CPISI_TERNARY_ZONE_UPPER_F    CPISI_TERNARY_ZONE_INNER_F                      /**< +0.25                 */
#define CPISI_TERNARY_ZONE_PERFECT_F  CPISI_TERNARY_ZONE_OUTER_F                      /**< +0.50                 */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2a.5b.2g SEVERITY — 9 severity levels
 *
 * Severity maps to milestones: -4..0..+4 with STEP units (25).
 * ───────────────────────────────────────────────────────────────────────────── */
#define CPISI_HEALTH_SEVERITY_MIN       CPISI_NEGATE(CPISI_FACE_CENTER)               /**< -4: FATAL edge        */
#define CPISI_HEALTH_SEVERITY_CENTER    CPISI_VOID                                    /**< 0: NONE (yashar)      */
#define CPISI_HEALTH_SEVERITY_MAX       CPISI_FACE_CENTER                             /**< +4: PERFECT edge      */
#define CPISI_HEALTH_SEVERITY_COUNT     CPISI_FACE                                    /**< 9: severity levels    */
#define CPISI_HEALTH_SEVERITY_STEP      CPISI_MILESTONE_STEP                          /**< 25: units per level   */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2a.5b.2h IMPACT — Display values at severity edges
 *
 * Impact values for extreme severities.
 * ───────────────────────────────────────────────────────────────────────────── */
#define CPISI_IMPACT_FATAL              CPISI_HEALTH_DISPLAY_MIN                      /**< -100: FATAL           */
#define CPISI_IMPACT_NONE               CPISI_HEALTH_DISPLAY_CENTER                   /**< 0: NONE               */
#define CPISI_IMPACT_PERFECT            CPISI_HEALTH_DISPLAY_MAX                      /**< +100: PERFECT         */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2a.5b.2i NORMALIZATION — True score, k-factor, positions, asymmetry
 *
 * Asymmetric normalization system converting true score to normalized range.
 *
 *   TRUE SCORE: -128..+127 (balanced layer values)
 *   K-FACTOR:   -1, 0, +1 (direction from yashar anchor)
 *   NORMALIZED: -1.0..+1.0 (scaled using asymmetric divisors)
 *   POSITIONS:  128 (neg), 1 (center), 127 (pos) — 256 total
 *
 * ASYMMETRY PRINCIPLE: 128 paths to corruption vs 127 to wholeness.
 * One more step to destruction than restoration — "many ways to fall."
 * ───────────────────────────────────────────────────────────────────────────── */

/* TRUE SCORE — aliases for BALANCED layer bounds */
#define CPISI_TRUE_MIN                  CPISI_HEALTH_BAL_MIN                          /**< -128: shavar edge      */
#define CPISI_TRUE_CENTER               CPISI_HEALTH_BAL_CENTER                       /**< 0: yashar anchor       */
#define CPISI_TRUE_MAX                  CPISI_HEALTH_BAL_MAX                          /**< +127: tov edge         */
#define CPISI_TRUE_RANGE_NEG            CPISI_HEALTH_EDGE_NEG                         /**< 128: corruption steps  */
#define CPISI_TRUE_RANGE_POS            CPISI_HEALTH_EDGE_POS                         /**< 127: restoration steps */
#define CPISI_TRUE_HALF_NEG             CPISI_HALF(CPISI_TRUE_RANGE_NEG)              /**< 63: midpoint to shavar */
#define CPISI_TRUE_HALF_POS             CPISI_HALF(CPISI_TRUE_RANGE_POS)              /**< 63: midpoint to tov    */

/* K-FACTOR — direction from yashar anchor */
#define CPISI_K_MIN                     CPISI_NEGATIVE                                /**< -1: toward shavar      */
#define CPISI_K_CENTER                  CPISI_NEUTRAL                                 /**< 0: at yashar anchor    */
#define CPISI_K_MAX                     CPISI_POSITIVE                                /**< +1: toward tov         */
#define CPISI_K_COUNT                   CPISI_TERNARY                                 /**< 3: ternary directions  */

/* NORMALIZED — scaled range bounds */
#define CPISI_NORM_MIN                  CPISI_HEALTH_TERN_MIN                         /**< -1.0: shavar edge      */
#define CPISI_NORM_CENTER               CPISI_HEALTH_TERN_CENTER                      /**< 0.0: yashar anchor     */
#define CPISI_NORM_MAX                  CPISI_HEALTH_TERN_MAX                         /**< +1.0: tov edge         */
#define CPISI_NORM_DIVISOR_NEG          CPISI_HEALTH_DIVISOR_NEG                      /**< 128.0f: neg divisor    */
#define CPISI_NORM_DIVISOR_CENTER       (1.0f)                                        /**< 1.0f: identity         */
#define CPISI_NORM_DIVISOR_POS          CPISI_HEALTH_DIVISOR_POS                      /**< 127.0f: pos divisor    */

/* POSITIONS — discrete step counts per direction */
#define CPISI_POSITIONS_NEG             CPISI_HEALTH_EDGE_NEG                         /**< 128: corruption steps  */
#define CPISI_POSITIONS_CENTER          CPISI_UNITY                                   /**< 1: yashar anchor only  */
#define CPISI_POSITIONS_POS             CPISI_HEALTH_EDGE_POS                         /**< 127: restoration steps */
#define CPISI_POSITIONS_TOTAL           CPISI_HEALTH_POSITIONS                        /**< 256: all positions     */

/* ASYMMETRY — position difference */
#define CPISI_ASYMMETRY_DIFF            CPISI_UNITY                                   /**< 1: 128-127=1 (one more step to corruption) */

/** @name Health Zones — Threshold boundaries for Hebrew state mapping (DISPLAY layer)
 *  @brief Divides the health DISPLAY range (-100 to +100) into 5 zones for state assessment.
 *
 *  ZONE LAYOUT (7 Hebrew states map to 5 thresholds on DISPLAY scale):
 *    -100 ──[BROKEN]── -75 ──[LOWER]── -25 ──[UPPER]── +25 ──[PERFECT]── +75 ── +100
 *          shavar           ratsah/chaser     yashar      shalem/tamim        tov
 *
 *  WHY DISPLAY SCALE: These thresholds operate on human-readable health scores.
 *  The cpisi_health_to_hebrew() function uses these to map scores to Hebrew states.
 *  @{ */

/**
 * @brief -75: broken zone threshold (DISPLAY scale).
 *
 * @return -75 (integer constant)
 *
 * @par Semantic Meaning
 *      Health scores at or below -75 indicate shavar (BROKEN) state.
 *      Severe degradation requiring immediate intervention.
 *
 * @par Derivation
 *      ZONE_BROKEN = TERNARY_ZONE_BROKEN * HEALTH_DISP_MAX = -0.75 * 100 = -75
 *
 * @see CPISI_TERNARY_ZONE_BROKEN — the normalized threshold (-0.75)
 */
#ifndef CPISI_ZONE_BROKEN
#define CPISI_ZONE_BROKEN               (-75)
#endif

/**
 * @brief -25: lower zone threshold (DISPLAY scale).
 *
 * @return -25 (integer constant)
 *
 * @par Semantic Meaning
 *      Health scores at or below -25 (but above -75) indicate:
 *      - ratsah (WANTING) if k-factor is advancing
 *      - chaser (LACKING) if k-factor is retreating
 *
 * @par Derivation
 *      ZONE_LOWER = TERNARY_ZONE_LOWER * HEALTH_DISP_MAX = -0.25 * 100 = -25
 *
 * @see CPISI_TERNARY_ZONE_LOWER — the normalized threshold (-0.25)
 */
#ifndef CPISI_ZONE_LOWER
#define CPISI_ZONE_LOWER                (-25)
#endif

/**
 * @brief +25: upper zone threshold (DISPLAY scale).
 *
 * @return +25 (integer constant)
 *
 * @par Semantic Meaning
 *      Health scores between -25 and +25 (exclusive) indicate yashar (EVEN) state.
 *      Scores at or above +25 (but below +75) indicate:
 *      - shalem (WHOLE) if k-factor is advancing
 *      - tamim (SOUND) if k-factor is retreating
 *
 * @par Derivation
 *      ZONE_UPPER = TERNARY_ZONE_UPPER * HEALTH_DISP_MAX = +0.25 * 100 = +25
 *
 * @see CPISI_TERNARY_ZONE_UPPER — the normalized threshold (+0.25)
 */
#ifndef CPISI_ZONE_UPPER
#define CPISI_ZONE_UPPER                (+25)
#endif

/**
 * @brief +75: perfect zone threshold (DISPLAY scale).
 *
 * @return +75 (integer constant)
 *
 * @par Semantic Meaning
 *      Health scores at or above +75 indicate tov (PERFECT) state.
 *      Optimal functioning — the goal state.
 *
 * @par Derivation
 *      ZONE_PERFECT = TERNARY_ZONE_PERFECT * HEALTH_DISP_MAX = +0.75 * 100 = +75
 *
 * @see CPISI_TERNARY_ZONE_PERFECT — the normalized threshold (+0.75)
 */
#ifndef CPISI_ZONE_PERFECT
#define CPISI_ZONE_PERFECT              (+75)
#endif

/** @} */ /* end Health Zones */

/** @} */ /* end S.2a BOUNDS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b MACROS — Health operations [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_macros S.2b HEALTH MACROS — Operations
 * @ingroup cpisi_health_domain
 * @brief    Health state, severity, trend, and pipeline operations.
 *
 * PURPOSE: Provide health-specific operations for the 4-layer pipeline
 *          (RAW → BALANCED → TERNARY → DISPLAY) using BOUNDS from types.h.
 *
 * Builds FROM: types.h (S.2a BOUNDS, S.2b.2 PRIMITIVES), error/error.h
 * Builds TO:   cpisi.h, dar.h, health monitoring systems
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC extensions to S.2b MACROS defined in
 *   types.h. The foundation macros (IN_RANGE, SIGN_VALID, etc.) come from
 *   types.h. This file adds health-specific pipeline operations.
 *
 * STRUCTURE:
 *   S.2b.4 HEALTH MACROS — Operations on health bounds
 *     S.2b.4a STATE        — Health state validation
 *     S.2b.4b SEVERITY     — Severity level validation
 *     S.2b.4c TREND        — Trend direction validation
 *     S.2b.4d LAYER        — Per-layer raw value ranges
 *     S.2b.4e CONVERSION   — Pipeline transforms (RAW↔BAL↔TERN↔DISP)
 *     S.2b.4f VALIDATION   — Pipeline layer range checks
 *     S.2b.4g MILESTONE    — Milestone ↔ display ↔ ternary
 *     S.2b.4h WEIGHT       — Layer weight operations
 *     S.2b.4i HALF         — Layer half validation
 *     S.2b.4j ZONE         — Zone index/boundary validation
 *     S.2b.4k IMPACT       — Severity ↔ impact conversion
 *     S.2b.4l NORMALIZATION — True score, k-factor, positions, asymmetry ops
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2b_4_HEALTH S.2b.4 HEALTH MACROS — Operations on health bounds
 * @ingroup cpisi_health_macros
 * @brief DOMAIN-SPECIFIC operations for health pipeline. Target: types-health.h.
 *
 * The health system uses a 4-layer pipeline for processing health values.
 * These macros provide validation, conversion, and calculation operations.
 *
 * THE 4-LAYER PIPELINE:
 * ```
 *   RAW (0-255)           — Storage layer (uint8_t, file/network)
 *        ↓ subtract 128
 *   BALANCED (-128..+127) — Math layer (int8_t, signed arithmetic)
 *        ↓ divide by 128/127 (asymmetric)
 *   TERNARY (-1.0..+1.0)  — Normalized layer (float, unit interval)
 *        ↓ multiply by 100
 *   DISPLAY (-100..+100)  — User layer (int, percentage display)
 * ```
 *
 * WHY 4 LAYERS:
 *   - RAW: Compact storage, no sign bit issues
 *   - BALANCED: Proper signed math (can go negative)
 *   - TERNARY: Normalized for mixing/interpolation
 *   - DISPLAY: Human-readable percentages
 *
 * ASYMMETRIC NORMALIZATION:
 *   BAL range is [-128, +127] (not symmetric). Negative values divide
 *   by 128, positive by 127. This ensures:
 *   - -128 maps to exactly -1.0
 *   - +127 maps to exactly +1.0
 *   - 0 maps to exactly 0.0
 *
 * STRUCTURE:
 *   S.2b.4a STATE      — Health state validation
 *   S.2b.4b SEVERITY   — Severity level validation
 *   S.2b.4c TREND      — Trend direction validation
 *   S.2b.4d LAYER      — Per-layer raw value ranges
 *   S.2b.4e CONVERSION — Pipeline transforms (RAW↔BAL↔TERN↔DISP)
 *   S.2b.4f VALIDATION — Pipeline layer range checks
 *   S.2b.4g MILESTONE  — Milestone ↔ display ↔ ternary
 *   S.2b.4h WEIGHT     — Layer weight operations
 *   S.2b.4i HALF       — Layer half validation
 *   S.2b.4j ZONE       — Zone index/boundary validation
 *   S.2b.4k IMPACT     — Severity ↔ impact conversion
 *   S.2b.4l NORMALIZATION — True score, k-factor, positions, asymmetry ops
 * @{ */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4a STATE OPS — Health state validation
 *
 * BOUNDS USED:
 *   HEALTH_COUNT = 5 (S.2a.6)  — 5 discrete health states
 *
 * HEALTH STATES (mapped to display percentages):
 *   0: CRITICAL  (-100 to -51)
 *   1: POOR      (-50 to -1)
 *   2: FAIR      (0)
 *   3: GOOD      (+1 to +50)
 *   4: EXCELLENT (+51 to +100)
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4a_STATE S.2b.4a STATE OPS — Health state operations
 * @ingroup S2b_4_HEALTH
 * @brief Validate health state indices.
 *
 * Health states are discrete categories derived from continuous health values.
 * The 5 states map to the display percentage range [-100, +100].
 *
 * STATE TABLE:
 * ```
 *   Index │ Name      │ Display Range │ Meaning
 *   ──────┼───────────┼───────────────┼─────────────────
 *     0   │ CRITICAL  │ -100 to -51   │ Severe degradation
 *     1   │ POOR      │  -50 to -1    │ Below normal
 *     2   │ FAIR      │   0           │ Neutral/baseline
 *     3   │ GOOD      │  +1 to +50    │ Above normal
 *     4   │ EXCELLENT │ +51 to +100   │ Optimal condition
 * ```
 * @{ */

/**
 * @brief Validate health state index [0, HEALTH_COUNT-1].
 *
 * @param h State index to validate
 * @return 1 if h in [0, 4], 0 otherwise
 *
 * BOUNDS: HEALTH_COUNT = 5 (S.2a.6 HEALTH)
 * RANGE:  [MIN_OF(5), MAX_OF(5)] = [0, 4]
 *
 * NOTE: This validates the discrete STATE index, not continuous health values.
 *       For continuous validation, use IS_HEALTH_RAW/BAL/TERNARY/DISPLAY.
 *
 * EXAMPLE:
 * ```c
 *   uint8_t state = display_to_state(health_display);
 *   if (!CPISI_HEALTH_VALID(state)) {
 *       return CPISI_ERROR_INVALID_HEALTH_STATE;
 *   }
 *   show_health_icon(state);  // CRITICAL..EXCELLENT
 * ```
 */
#define CPISI_HEALTH_VALID(h)   CPISI_IN_RANGE((h), CPISI_MIN_OF(CPISI_HEALTH_COUNT), CPISI_MAX_OF(CPISI_HEALTH_COUNT))

/** @} */ /* end S.2b.4a STATE OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4b SEVERITY OPS — Severity level validation
 *
 * BOUNDS USED:
 *   SEVERITY_COUNT = 3 (S.2a.6)  — TERNARY severity levels
 *
 * SEVERITY LEVELS (ternary: mild/medium/critical):
 *   0: MINOR    — informational, low impact
 *   1: ERROR    — significant, needs attention
 *   2: CRITICAL — severe, immediate action required
 *
 * WHY TERNARY:
 *   3 levels = balanced ternary (-1, 0, +1) mapped to urgency.
 *   Matches DAR phase severity (Detect/Assess/Restore each has 3 severities).
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4b_SEVERITY S.2b.4b SEVERITY OPS — Severity operations
 * @ingroup S2b_4_HEALTH
 * @brief Validate severity levels (MINOR, ERROR, CRITICAL).
 *
 * SEVERITY TABLE:
 * ```
 *   Index │ Name     │ Balanced │ Response
 *   ──────┼──────────┼──────────┼────────────────────
 *     0   │ MINOR    │    -1    │ Log, continue
 *     1   │ ERROR    │     0    │ Handle, may degrade
 *     2   │ CRITICAL │    +1    │ Immediate action
 * ```
 * @{ */

/**
 * @brief Validate severity level [0, SEVERITY_COUNT-1].
 *
 * @param s Severity to validate
 * @return 1 if s in [0, 2], 0 otherwise
 *
 * BOUNDS: SEVERITY_COUNT = 3 (S.2a.6 = TERNARY)
 * RANGE:  [MIN_OF(3), MAX_OF(3)] = [0, 2]
 *
 * EXAMPLE:
 * ```c
 *   uint8_t severity = get_error_severity(error_code);
 *   if (!CPISI_SEVERITY_VALID(severity)) {
 *       severity = CPISI_SEVERITY_ERROR;  // Default to middle
 *   }
 *   log_with_severity(message, severity);
 * ```
 */
#define CPISI_SEVERITY_VALID(s) CPISI_IN_RANGE((s), CPISI_MIN_OF(CPISI_SEVERITY_COUNT), CPISI_MAX_OF(CPISI_SEVERITY_COUNT))

/** @} */ /* end S.2b.4b SEVERITY OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4c TREND OPS — Trend direction validation
 *
 * Trends use BALANCED bounds (-1 to +1), not index bounds.
 *
 * BOUNDS USED:
 *   TREND_MIN = -1 (S.2a.6)  — declining
 *   TREND_MAX = +1 (S.2a.6)  — improving
 *
 * WHY BALANCED (not index):
 *   Trend is SEMANTIC direction, not array index.
 *   -1 = getting worse, 0 = stable, +1 = getting better.
 *   Use SIGN_INDEX(trend) to convert to array index if needed.
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4c_TREND S.2b.4c TREND OPS — Trend operations
 * @ingroup S2b_4_HEALTH
 * @brief Validate trend direction (declining/stable/improving).
 *
 * Trend indicates the DIRECTION of health change over time.
 * This is distinct from current health value — you can have:
 *   - EXCELLENT health with DECLINING trend (was better, getting worse)
 *   - POOR health with IMPROVING trend (was worse, getting better)
 *
 * TREND TABLE:
 * ```
 *   Value │ Name      │ Meaning
 *   ──────┼───────────┼────────────────────────
 *    -1   │ DECLINING │ Health getting worse
 *     0   │ STABLE    │ Health unchanged
 *    +1   │ IMPROVING │ Health getting better
 * ```
 * @{ */

/**
 * @brief Validate trend direction [-1, +1].
 *
 * @param t Trend value to validate
 * @return 1 if t in [-1, +1], 0 otherwise
 *
 * BOUNDS: TREND_MIN=-1, TREND_MAX=+1 (S.2a.6 HEALTH)
 * RANGE:  BALANCED [-1, +1], NOT index [0, 2]
 *
 * WHY EXPLICIT BOUNDS (not _OF macros):
 *   Trend is BALANCED (centered on 0), not COUNT-based.
 *   Same pattern as SIGN_VALID — these are semantic values, not indices.
 *
 * EXAMPLE:
 * ```c
 *   int8_t trend = calculate_health_trend(current, previous);
 *   if (!CPISI_TREND_VALID(trend)) {
 *       trend = CPISI_TREND_STABLE;  // Default to stable
 *   }
 *   update_trend_indicator(trend);
 * ```
 */
#define CPISI_TREND_VALID(t)    CPISI_IN_RANGE((t), CPISI_TREND_MIN, CPISI_TREND_MAX)

/** @} */ /* end S.2b.4c TREND OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4d LAYER OPS — Health layer range operations
 *
 * Each layer has a RAW value range: layer × 28 to (layer+1) × 28 - 1.
 *
 * BOUNDS USED:
 *   HEALTH_RAW_LAYER_SIZE = 28 (S.2a.6)  — raw values per layer
 *   FACE = 9 (S.2a.3)                    — number of layers
 *
 * WHY 28:
 *   256 ÷ 9 = 28.44... → 28 raw values per layer.
 *   8 layers × 28 = 224, last layer gets 224-255 = 32 values.
 *   (Slight asymmetry at APP layer, acceptable for health mapping.)
 *
 * LAYER RANGES:
 *   Layer 0 (KERNEL):    0-27
 *   Layer 1 (MEMORY):   28-55
 *   Layer 2 (PROCESS):  56-83
 *   Layer 3 (STORAGE):  84-111
 *   Layer 4 (HEALTH):  112-139  ← CENTER
 *   Layer 5 (NETWORK): 140-167
 *   Layer 6 (SECURITY):168-195
 *   Layer 7 (SERVICES):196-223
 *   Layer 8 (APP):     224-255  (32 values)
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4d_LAYER S.2b.4d LAYER OPS — Health layer range operations
 * @ingroup S2b_4_HEALTH
 * @brief Per-layer raw value ranges for 9-layer pyramid.
 *
 * Health values can be partitioned by layer — each of the 9 pyramid layers
 * owns a slice of the 0-255 raw range. This enables layer-specific health
 * thresholds and per-layer health aggregation.
 *
 * RAW RANGE PARTITION:
 * ```
 *   Layer │ Min │ Max │ Size │ Name
 *   ──────┼─────┼─────┼──────┼──────────
 *     0   │   0 │  27 │   28 │ KERNEL
 *     1   │  28 │  55 │   28 │ MEMORY
 *     2   │  56 │  83 │   28 │ PROCESS
 *     3   │  84 │ 111 │   28 │ STORAGE
 *     4   │ 112 │ 139 │   28 │ HEALTH
 *     5   │ 140 │ 167 │   28 │ NETWORK
 *     6   │ 168 │ 195 │   28 │ SECURITY
 *     7   │ 196 │ 223 │   28 │ SERVICES
 *     8   │ 224 │ 255 │   32 │ APP (extra)
 * ```
 * @{ */

/**
 * @brief Get minimum raw value for layer.
 *
 * @param layer Layer index (0-8)
 * @return Minimum raw value (layer × 28)
 *
 * FORMULA: MIN = layer × HEALTH_RAW_LAYER_SIZE = layer × 28
 *
 * USES:
 *   - HEALTH_RAW_LAYER_SIZE (28) from S.2a.6 HEALTH
 *   - PRODUCT from S.2.0c ARITHMETIC
 *
 * EXAMPLES:
 * ```c
 *   CPISI_HEALTH_RAW_LAYER_MIN(0) = 0 × 28 = 0    // KERNEL
 *   CPISI_HEALTH_RAW_LAYER_MIN(4) = 4 × 28 = 112  // HEALTH (center)
 *   CPISI_HEALTH_RAW_LAYER_MIN(8) = 8 × 28 = 224  // APP
 * ```
 */
#define CPISI_HEALTH_RAW_LAYER_MIN(layer)  CPISI_PRODUCT((layer), CPISI_HEALTH_RAW_LAYER_SIZE)

/**
 * @brief Get maximum raw value for layer.
 *
 * @param layer Layer index (0-8)
 * @return Maximum raw value (min + 28 - 1)
 *
 * FORMULA: MAX = MIN + LAYER_SIZE - 1 = (layer+1) × 28 - 1
 *
 * USES:
 *   - HEALTH_RAW_LAYER_MIN(layer) defined above
 *   - HEALTH_RAW_LAYER_SIZE (28) from S.2a.6 HEALTH
 *   - SUM, DIFF from S.2.0c ARITHMETIC
 *   - UNITY (1) from S.2a.1 CORE
 *
 * NOTE: Layer 8 (APP) actually goes to 255, not 251.
 *       This formula gives 251, but real max is capped at 255.
 *       Use explicit cap in production: MIN(MAX(layer), 255).
 *
 * EXAMPLES:
 * ```c
 *   CPISI_HEALTH_RAW_LAYER_MAX(0) = 0 + 28 - 1 = 27   // KERNEL
 *   CPISI_HEALTH_RAW_LAYER_MAX(4) = 112 + 28 - 1 = 139 // HEALTH
 *   CPISI_HEALTH_RAW_LAYER_MAX(8) = 224 + 28 - 1 = 251 // APP (formula)
 *   // Real APP max is 255 (capped)
 * ```
 */
#define CPISI_HEALTH_RAW_LAYER_MAX(layer)  CPISI_DIFF(CPISI_SUM(CPISI_HEALTH_RAW_LAYER_MIN(layer), CPISI_HEALTH_RAW_LAYER_SIZE), CPISI_UNITY)

/** @} */ /* end S.2b.4d LAYER OPS */

/**
 * @defgroup S2b_4e_CONVERSION S.2b.4e CONVERSION OPS — 4-layer pipeline transforms
 * @ingroup S2b_4_HEALTH
 * @brief RAW(0-255) ↔ BALANCED(-128..+127) ↔ TERNARY(-1..+1) ↔ DISPLAY(-100..+100).
 *
 * The 4-layer pipeline converts health values between representations:
 *   - RAW: Compact storage (uint8_t, files, network)
 *   - BALANCED: Signed math (int8_t, arithmetic operations)
 *   - TERNARY: Normalized (float, interpolation, mixing)
 *   - DISPLAY: Human-readable (int, percentage UI)
 *
 * CONVERSION FLOW:
 * ```
 *   RAW ──subtract 128──► BALANCED ──divide 128/127──► TERNARY ──×100──► DISPLAY
 *    ▲                        ▲                           ▲                  │
 *    │                        │                           │                  │
 *    └──────add 128───────────┴───────multiply────────────┴────÷100──────────┘
 * ```
 *
 * BOUNDS USED:
 *   HEALTH_RAW_CENTER = 128 (S.2a.6)    — RAW↔BAL offset
 *   HEALTH_DIVISOR_NEG = 128 (S.2a.6)   — negative normalization
 *   HEALTH_DIVISOR_POS = 127 (S.2a.6)   — positive normalization
 *   HEALTH_DISPLAY_MAX = 100 (S.2a.6)   — TERNARY↔DISPLAY scale
 *
 * ASYMMETRIC NORMALIZATION:
 *   int8_t range is [-128, +127], not symmetric.
 *   - Negative values: divide by 128 → -128 becomes exactly -1.0
 *   - Positive values: divide by 127 → +127 becomes exactly +1.0
 *   - Zero: remains 0.0 (no division needed)
 * @{ */

/* ── S.2b.4e.1 RAW↔BALANCED — storage ↔ math layer ────────────────────────── */

/**
 * @brief Convert RAW (0-255) to BALANCED (-128..+127).
 *
 * @param raw Unsigned raw value (0-255)
 * @return Signed balanced value (-128 to +127)
 *
 * FORMULA: bal = raw - CENTER = raw - 128
 *
 * MAPPING:
 *   raw=0   → bal=-128 (worst)
 *   raw=128 → bal=0    (neutral)
 *   raw=255 → bal=+127 (best)
 */
#define CPISI_HEALTH_RAW_TO_BAL(raw)   ((int8_t)CPISI_DIFF((raw), CPISI_HEALTH_RAW_CENTER))

/**
 * @brief Convert BALANCED (-128..+127) to RAW (0-255).
 *
 * @param bal Signed balanced value (-128 to +127)
 * @return Unsigned raw value (0-255)
 *
 * FORMULA: raw = bal + CENTER = bal + 128
 *
 * MAPPING:
 *   bal=-128 → raw=0   (worst)
 *   bal=0    → raw=128 (neutral)
 *   bal=+127 → raw=255 (best)
 */
#define CPISI_HEALTH_BAL_TO_RAW(bal)   ((uint8_t)CPISI_SUM((bal), CPISI_HEALTH_RAW_CENTER))

/* ── S.2b.4e.2 BALANCED→TERNARY — asymmetric normalization ────────────────── */

/**
 * @brief Convert BALANCED (-128..+127) to TERNARY (-1.0..+1.0).
 *
 * @param bal Signed balanced value (-128 to +127)
 * @return Normalized float (-1.0 to +1.0)
 *
 * FORMULA (asymmetric):
 *   if bal < 0:  ternary = bal / 128  (negative range)
 *   if bal >= 0: ternary = bal / 127  (positive range)
 *
 * WHY ASYMMETRIC:
 *   int8_t has 128 negative values (-128..-1) but only 127 positive (1..127).
 *   Symmetric division would make +127 map to 0.992, not 1.0.
 *   Asymmetric ensures both extremes reach exactly ±1.0.
 *
 * MAPPING:
 *   bal=-128 → -128/128 = -1.0  (exact)
 *   bal=0    → 0/127    =  0.0  (exact)
 *   bal=+127 → +127/127 = +1.0  (exact)
 */
#define CPISI_HEALTH_BAL_TO_TERNARY(bal) \
    ((bal) < 0 ? (float)(bal) / CPISI_HEALTH_DIVISOR_NEG \
               : (float)(bal) / CPISI_HEALTH_DIVISOR_POS)

/* ── S.2b.4e.3 RAW→TERNARY — compound conversion ──────────────────────────── */

/**
 * @brief Convert RAW (0-255) to TERNARY (-1.0..+1.0) via BALANCED intermediate.
 *
 * @param raw Unsigned raw value (0-255)
 * @return Normalized float (-1.0 to +1.0)
 *
 * FORMULA: RAW → BAL → TERNARY
 *          raw → (raw-128) → asymmetric_normalize
 *
 * SHORTHAND: Chains RAW_TO_BAL then BAL_TO_TERNARY.
 */
#define CPISI_HEALTH_RAW_TO_TERNARY(raw) \
    CPISI_HEALTH_BAL_TO_TERNARY(CPISI_HEALTH_RAW_TO_BAL(raw))

/* ── S.2b.4e.4 TERNARY→DISPLAY — scale to ±100 ────────────────────────────── */

/**
 * @brief Convert TERNARY (-1.0..+1.0) to DISPLAY (-100..+100).
 *
 * @param t Normalized float (-1.0 to +1.0)
 * @return Integer percentage (-100 to +100)
 *
 * FORMULA: display = ternary × DISPLAY_MAX = t × 100
 *
 * MAPPING:
 *   t=-1.0 → -100 (worst)
 *   t=0.0  →    0 (neutral)
 *   t=+1.0 → +100 (best)
 */
#define CPISI_HEALTH_TERNARY_TO_DISPLAY(t) \
    ((int)((t) * (float)CPISI_HEALTH_DISPLAY_MAX))

/* ── S.2b.4e.5 BAL→DISPLAY — compound conversion ──────────────────────────── */

/**
 * @brief Convert BALANCED (-128..+127) to DISPLAY (-100..+100) via TERNARY.
 *
 * @param bal Signed balanced value (-128 to +127)
 * @return Integer percentage (-100 to +100)
 *
 * FORMULA: BAL → TERNARY → DISPLAY
 *
 * SHORTHAND: Chains BAL_TO_TERNARY then TERNARY_TO_DISPLAY.
 */
#define CPISI_HEALTH_BAL_TO_DISPLAY(bal) \
    CPISI_HEALTH_TERNARY_TO_DISPLAY(CPISI_HEALTH_BAL_TO_TERNARY(bal))

/* ── S.2b.4e.6 RAW→DISPLAY — full pipeline ────────────────────────────────── */

/**
 * @brief Convert RAW (0-255) to DISPLAY (-100..+100) — full pipeline.
 *
 * @param raw Unsigned raw value (0-255)
 * @return Integer percentage (-100 to +100)
 *
 * FORMULA: RAW → BAL → TERNARY → DISPLAY
 *
 * MOST COMMON USE: Read raw from storage, show percentage to user.
 *
 * EXAMPLE:
 * ```c
 *   uint8_t raw = read_health_from_file();
 *   int display = CPISI_HEALTH_RAW_TO_DISPLAY(raw);
 *   printf("Health: %d%%\n", display);  // "Health: 42%"
 * ```
 */
#define CPISI_HEALTH_RAW_TO_DISPLAY(raw) \
    CPISI_HEALTH_TERNARY_TO_DISPLAY(CPISI_HEALTH_RAW_TO_TERNARY(raw))

/** @} */ /* end S.2b.4e CONVERSION OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4f VALIDATION OPS — Pipeline layer range checks
 *
 * Validate values at each layer of the 4-layer pipeline.
 * Use these BEFORE conversion to catch invalid inputs.
 *
 * BOUNDS USED:
 *   RAW:     0-255 (S.2a.6)
 *   BAL:     -128..+127 (S.2a.6)
 *   TERNARY: -1.0..+1.0 (S.2a.6)
 *   DISPLAY: -100..+100 (S.2a.6)
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4f_VALIDATION S.2b.4f VALIDATION OPS — Pipeline layer range checks
 * @ingroup S2b_4_HEALTH
 * @brief Validate values at each layer of the 4-layer pipeline.
 *
 * VALIDATION TABLE:
 * ```
 *   Layer   │ Validator         │ Range         │ Type
 *   ────────┼───────────────────┼───────────────┼────────
 *   RAW     │ IS_HEALTH_RAW     │ [0, 255]      │ uint8_t
 *   BAL     │ IS_HEALTH_BAL     │ [-128, +127]  │ int8_t
 *   TERNARY │ IS_HEALTH_TERNARY │ [-1.0, +1.0]  │ float
 *   DISPLAY │ IS_HEALTH_DISPLAY │ [-100, +100]  │ int
 * ```
 *
 * USE CASE: Validate before conversion to catch corrupt/invalid data.
 * ```c
 *   if (!CPISI_IS_HEALTH_RAW(raw)) {
 *       return CPISI_ERROR_INVALID_HEALTH_RAW;
 *   }
 *   int display = CPISI_HEALTH_RAW_TO_DISPLAY(raw);
 * ```
 * @{ */

/** @brief Validate RAW health value [0, 255]. */
#define CPISI_IS_HEALTH_RAW(raw)     CPISI_IN_RANGE((raw), CPISI_HEALTH_RAW_MIN, CPISI_HEALTH_RAW_MAX)

/** @brief Validate BALANCED health value [-128, +127]. */
#define CPISI_IS_HEALTH_BAL(bal)     CPISI_IN_RANGE((bal), CPISI_HEALTH_BAL_MIN, CPISI_HEALTH_BAL_MAX)

/** @brief Validate TERNARY health value [-1.0, +1.0]. Uses float comparison. */
#define CPISI_IS_HEALTH_TERNARY(t)   ((t) >= CPISI_HEALTH_TERN_MIN && (t) <= CPISI_HEALTH_TERN_MAX)

/** @brief Validate DISPLAY health value [-100, +100]. */
#define CPISI_IS_HEALTH_DISPLAY(d)   CPISI_IN_RANGE((d), CPISI_HEALTH_DISPLAY_MIN, CPISI_HEALTH_DISPLAY_MAX)

/** @} */ /* end S.2b.4f VALIDATION OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4g MILESTONE OPS — Milestone conversion operations
 *
 * Milestones are discrete checkpoints on the health scale.
 * 9 milestones map to the display range: -100, -75, -50, -25, 0, +25, +50, +75, +100.
 *
 * BOUNDS USED:
 *   MILESTONE_MIN = -4 (S.2a.6)   MILESTONE_MAX = +4 (S.2a.6)
 *   MILESTONE_STEP = 25 (S.2a.6)  — display units per milestone
 *   HEALTH_DISPLAY_DIVISOR = 100.0 (S.2a.6)
 *
 * MILESTONE TABLE:
 *   m=-4 → -100 (CRITICAL)
 *   m=-2 → -50  (POOR)
 *   m=0  →   0  (FAIR)
 *   m=+2 → +50  (GOOD)
 *   m=+4 → +100 (EXCELLENT)
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4g_MILESTONE S.2b.4g MILESTONE OPS — Milestone conversion operations
 * @ingroup S2b_4_HEALTH
 * @brief Milestone ↔ display ↔ ternary conversion and validation.
 *
 * Milestones provide DISCRETE checkpoints while display is CONTINUOUS.
 * Use milestones for thresholds, achievements, state boundaries.
 *
 * MILESTONE MAP:
 * ```
 *   Milestone │ Display │ State
 *   ──────────┼─────────┼───────────
 *      -4     │  -100   │ CRITICAL
 *      -3     │   -75   │ CRITICAL
 *      -2     │   -50   │ POOR
 *      -1     │   -25   │ POOR
 *       0     │     0   │ FAIR
 *      +1     │   +25   │ GOOD
 *      +2     │   +50   │ GOOD
 *      +3     │   +75   │ EXCELLENT
 *      +4     │  +100   │ EXCELLENT
 * ```
 * @{ */

/** @brief Convert milestone [-4,+4] to display [-100,+100]. Formula: m × 25 */
#define CPISI_MILESTONE_TO_DISPLAY(m)  CPISI_PRODUCT((m), CPISI_MILESTONE_STEP)

/** @brief Convert display [-100,+100] to milestone [-4,+4]. Formula: d ÷ 25 */
#define CPISI_DISPLAY_TO_MILESTONE(d)  CPISI_QUOTIENT((d), CPISI_MILESTONE_STEP)

/** @brief Convert display [-100,+100] to ternary [-1.0,+1.0]. Formula: d ÷ 100.0 */
#define CPISI_DISPLAY_TO_TERNARY(d)    ((float)(d) / CPISI_HEALTH_DISPLAY_DIVISOR)

/** @brief Convert ternary [-1.0,+1.0] to display [-100,+100]. Formula: t × 100.0 */
#define CPISI_TERNARY_TO_DISPLAY_ALT(t) ((int)((t) * CPISI_HEALTH_DISPLAY_DIVISOR))

/** @brief Validate milestone [-4, +4]. */
#define CPISI_IS_MILESTONE(m)          CPISI_IN_RANGE((m), CPISI_MILESTONE_MIN, CPISI_MILESTONE_MAX)

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4g.2 MILESTONE DISPLAY VALUES — 9 fixed display values from ZONE constants
 *
 * These macros define the 9 milestone DISPLAY values (-100 to +100) derived
 * from ZONE_* constants. Unlike conversion macros, these are VALUE definitions.
 *
 * DERIVATION:
 *   NEG_* = -ZONE_* (toward shavar/broken)
 *   POS_* = +ZONE_* (toward tov/perfect)
 *   NEUTRAL = HEALTH_DISPLAY_CENTER (yashar/even)
 *
 * The 9 display values map to Hebrew states:
 *   -100 (shavar), -75 (machsor), -50 (chaser), -25 (ratsah), 0 (yashar),
 *   +25 (ratson), +50 (shalem), +75 (tamim), +100 (tov)
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Milestone NEG_4 display value — shavar (broken) edge.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_EDGE = 100 (S.2a.5b.2f) — edge distance from center
 *
 * FORMULA: NEGATE(ZONE_EDGE) = -100
 *
 * SEMANTIC MEANING:
 *   Represents the crisis edge of health — shavar (שָׁבַר, broken).
 *   The lowest milestone marking complete system failure state.
 *   Maps to milestone index 0, Hebrew state shavar.
 *
 * USE CASE:
 *   - Boundary check for minimum health threshold
 *   - Initialization of critical failure state
 *   - UI display of crisis indicator
 *
 * RELATIONSHIP TO:
 *   - CPISI_MILESTONE_POS_4: Symmetric opposite (+100)
 *   - CPISI_HEALTH_DISPLAY_MIN: Equal value (-100)
 *   - CPISI_IMPACT_FATAL: Equal value for impact system
 */
#define CPISI_MILESTONE_NEG_4       CPISI_NEGATE(CPISI_ZONE_EDGE)

/**
 * @brief Milestone NEG_3 display value — machsor (deprived) threshold.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_THREE_Q = 75 (S.2a.5b.2f) — three-quarter distance
 *
 * FORMULA: NEGATE(ZONE_THREE_Q) = -75
 *
 * SEMANTIC MEANING:
 *   Represents severe degradation — machsor (מַחְסוֹר, deprived).
 *   Second-worst milestone marking serious resource deprivation.
 *   Maps to milestone index 1, between shavar and chaser.
 *
 * USE CASE:
 *   - Warning threshold before critical failure
 *   - Trigger for aggressive recovery actions
 *   - UI display of severe degradation
 *
 * RELATIONSHIP TO:
 *   - CPISI_MILESTONE_POS_3: Symmetric opposite (+75)
 *   - CPISI_MILESTONE_NEG_4: One step more severe (-100)
 *   - CPISI_MILESTONE_NEG_2: One step less severe (-50)
 */
#define CPISI_MILESTONE_NEG_3       CPISI_NEGATE(CPISI_ZONE_THREE_Q)

/**
 * @brief Milestone NEG_2 display value — chaser (lacking) threshold.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_OUTER = 50 (S.2a.5b.2f) — outer zone boundary
 *
 * FORMULA: NEGATE(ZONE_OUTER) = -50
 *
 * SEMANTIC MEANING:
 *   Represents moderate degradation — chaser (חָסֵר, lacking).
 *   Marks entry into the LOW zone from BALANCED.
 *   Maps to milestone index 2, standard decline state.
 *
 * USE CASE:
 *   - Zone boundary: LOW/BALANCED transition
 *   - Moderate alert threshold
 *   - Scheduled maintenance trigger
 *
 * RELATIONSHIP TO:
 *   - CPISI_MILESTONE_POS_2: Symmetric opposite (+50)
 *   - CPISI_ZONE_OUTER: Absolute value equals this threshold
 *   - Zone boundary at -50 separates LOW from CRITICAL
 */
#define CPISI_MILESTONE_NEG_2       CPISI_NEGATE(CPISI_ZONE_OUTER)

/**
 * @brief Milestone NEG_1 display value — ratsah (wanting) threshold.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_INNER = 25 (S.2a.5b.2f) — inner zone boundary
 *
 * FORMULA: NEGATE(ZONE_INNER) = -25
 *
 * SEMANTIC MEANING:
 *   Represents minor degradation — ratsah (רָצָה, wanting).
 *   Marks entry into negative territory from center.
 *   Maps to milestone index 3, hungry for restoration.
 *
 * USE CASE:
 *   - Zone boundary: BALANCED/LOW transition
 *   - Early warning indicator
 *   - Soft alert for preventive action
 *
 * RELATIONSHIP TO:
 *   - CPISI_MILESTONE_POS_1: Symmetric opposite (+25)
 *   - CPISI_ZONE_INNER: Absolute value equals this threshold
 *   - Zone boundary at -25 separates BALANCED from LOW
 */
#define CPISI_MILESTONE_NEG_1       CPISI_NEGATE(CPISI_ZONE_INNER)

/**
 * @brief Milestone NEUTRAL display value — yashar (even) center.
 *
 * BOUNDS USED:
 *   - CPISI_HEALTH_DISPLAY_CENTER = 0 (S.2a.5b.2c) — display center
 *
 * FORMULA: HEALTH_DISPLAY_CENTER = 0
 *
 * SEMANTIC MEANING:
 *   Represents the balanced center — yashar (יָשָׁר, even/upright).
 *   The equilibrium point of the health scale.
 *   Maps to milestone index 4, the default/target state.
 *
 * USE CASE:
 *   - Default initialization value
 *   - Restoration target for recovery algorithms
 *   - UI display of balanced state
 *
 * RELATIONSHIP TO:
 *   - CPISI_HEALTH_DISPLAY_CENTER: Equal value (0)
 *   - CPISI_VOID: Equal value as mathematical zero
 *   - Center of 4+1+4 milestone pattern
 */
#define CPISI_MILESTONE_NEUTRAL     CPISI_HEALTH_DISPLAY_CENTER

/**
 * @brief Milestone POS_1 display value — ratson (willing) threshold.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_INNER = 25 (S.2a.5b.2f) — inner zone boundary
 *
 * FORMULA: ZONE_INNER = +25
 *
 * SEMANTIC MEANING:
 *   Represents minor improvement — ratson (רָצוֹן, willing/favor).
 *   Marks entry into positive territory from center.
 *   Maps to milestone index 5, receiving goodness.
 *
 * USE CASE:
 *   - Zone boundary: BALANCED/HIGH transition
 *   - Positive trend indicator
 *   - Achievement milestone (first positive)
 *
 * RELATIONSHIP TO:
 *   - CPISI_MILESTONE_NEG_1: Symmetric opposite (-25)
 *   - CPISI_ZONE_INNER: Direct value reference
 *   - Zone boundary at +25 separates BALANCED from HIGH
 */
#define CPISI_MILESTONE_POS_1       CPISI_ZONE_INNER

/**
 * @brief Milestone POS_2 display value — shalem (whole) threshold.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_OUTER = 50 (S.2a.5b.2f) — outer zone boundary
 *
 * FORMULA: ZONE_OUTER = +50
 *
 * SEMANTIC MEANING:
 *   Represents moderate improvement — shalem (שָׁלֵם, whole/complete).
 *   Marks entry into the HIGH zone from BALANCED.
 *   Maps to milestone index 6, expanding wholeness.
 *
 * USE CASE:
 *   - Zone boundary: HIGH/OPTIMAL transition
 *   - Achievement milestone (halfway to peak)
 *   - Stability indicator
 *
 * RELATIONSHIP TO:
 *   - CPISI_MILESTONE_NEG_2: Symmetric opposite (-50)
 *   - CPISI_ZONE_OUTER: Direct value reference
 *   - Zone boundary at +50 separates HIGH from OPTIMAL
 */
#define CPISI_MILESTONE_POS_2       CPISI_ZONE_OUTER

/**
 * @brief Milestone POS_3 display value — tamim (sound) threshold.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_THREE_Q = 75 (S.2a.5b.2f) — three-quarter distance
 *
 * FORMULA: ZONE_THREE_Q = +75
 *
 * SEMANTIC MEANING:
 *   Represents strong improvement — tamim (תָּמִים, sound/blameless).
 *   Second-best milestone marking excellent health.
 *   Maps to milestone index 7, consolidated soundness.
 *
 * USE CASE:
 *   - Excellence threshold
 *   - Achievement milestone (near peak)
 *   - UI display of exceptional state
 *
 * RELATIONSHIP TO:
 *   - CPISI_MILESTONE_NEG_3: Symmetric opposite (-75)
 *   - CPISI_MILESTONE_POS_4: One step toward peak (+100)
 *   - CPISI_MILESTONE_POS_2: One step from wholeness (+50)
 */
#define CPISI_MILESTONE_POS_3       CPISI_ZONE_THREE_Q

/**
 * @brief Milestone POS_4 display value — tov (perfect) edge.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_EDGE = 100 (S.2a.5b.2f) — edge distance from center
 *
 * FORMULA: ZONE_EDGE = +100
 *
 * SEMANTIC MEANING:
 *   Represents the completion edge — tov (טוֹב, good/perfect).
 *   The highest milestone marking peak health state.
 *   Maps to milestone index 8, Hebrew state tov.
 *
 * USE CASE:
 *   - Boundary check for maximum health threshold
 *   - Completion/achievement marker
 *   - UI display of peak state
 *
 * RELATIONSHIP TO:
 *   - CPISI_MILESTONE_NEG_4: Symmetric opposite (-100)
 *   - CPISI_HEALTH_DISPLAY_MAX: Equal value (+100)
 *   - CPISI_IMPACT_PERFECT: Equal value for impact system
 */
#define CPISI_MILESTONE_POS_4       CPISI_ZONE_EDGE

/**
 * @brief Validate milestone display value — true if -100 ≤ value ≤ +100.
 *
 * BOUNDS USED:
 *   - CPISI_MILESTONE_DISPLAY_MIN = -100 (S.2a.5b.2c) — minimum display
 *   - CPISI_MILESTONE_DISPLAY_MAX = +100 (S.2a.5b.2c) — maximum display
 *
 * FORMULA: CPISI_IN_RANGE(t, -100, +100)
 *   Returns true if -100 ≤ t ≤ +100, false otherwise.
 *
 * USE CASE:
 *   - Input validation before display value operations
 *   - Guard condition for threshold comparisons
 *   - Assertion in debug builds
 *
 * EXAMPLE:
 *   CPISI_IS_MILESTONE_DISPLAY(-100) → true  (shavar edge)
 *   CPISI_IS_MILESTONE_DISPLAY(0)    → true  (yashar center)
 *   CPISI_IS_MILESTONE_DISPLAY(100)  → true  (tov edge)
 *   CPISI_IS_MILESTONE_DISPLAY(101)  → false (out of range)
 *
 * @param t Display value to validate (any integer)
 * @return true if t ∈ [-100, +100], false otherwise
 */
#define CPISI_IS_MILESTONE_DISPLAY(t)  CPISI_IN_RANGE((t), CPISI_MILESTONE_DISPLAY_MIN, CPISI_MILESTONE_DISPLAY_MAX)

/** @} */ /* end S.2b.4g MILESTONE OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4h WEIGHT OPS — Layer weight operations
 *
 * Layer weights determine priority/importance. Inner layers (KERNEL) have
 * higher weight; outer layers (APP) have lower weight.
 *
 * BOUNDS USED:
 *   FACE = 9 (S.2a.3)              — total layers
 *   HEALTH_WEIGHT_MIN = 1 (S.2a.6) — minimum weight (APP layer)
 *   HEALTH_WEIGHT_MAX = 9 (S.2a.6) — maximum weight (KERNEL layer)
 *   HEALTH_WEIGHT_CENTER = 5 (S.2a.6) — center weight (HEALTH layer)
 *
 * WEIGHT TABLE:
 *   Layer 0 (KERNEL):  weight = 9 - 0 = 9 (highest priority)
 *   Layer 4 (HEALTH):  weight = 9 - 4 = 5 (center)
 *   Layer 8 (APP):     weight = 9 - 8 = 1 (lowest priority)
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4h_WEIGHT S.2b.4h WEIGHT OPS — Layer weight operations
 * @ingroup S2b_4_HEALTH
 * @brief Weight calculation, balanced offset, and validation.
 *
 * Weights are INVERSE to layer index: lower layers (kernel) = higher weight.
 * This reflects priority: kernel issues are more critical than app issues.
 *
 * WEIGHT TABLE:
 * ```
 *   Layer │ Index │ Weight │ Balanced │ Priority
 *   ──────┼───────┼────────┼──────────┼──────────
 *   KERNEL│   0   │    9   │    +4    │ Highest
 *   MEMORY│   1   │    8   │    +3    │
 *   PROC  │   2   │    7   │    +2    │
 *   STORE │   3   │    6   │    +1    │
 *   HEALTH│   4   │    5   │     0    │ Center
 *   NET   │   5   │    4   │    -1    │
 *   SEC   │   6   │    3   │    -2    │
 *   SERV  │   7   │    2   │    -3    │
 *   APP   │   8   │    1   │    -4    │ Lowest
 * ```
 * @{ */

/** @brief Calculate layer weight. Formula: FACE - layer = 9 - layer */
#define CPISI_HEALTH_WEIGHT(layer)      CPISI_DIFF(CPISI_FACE, (layer))

/** @brief Calculate balanced weight (offset from center). Formula: weight - 5 */
#define CPISI_HEALTH_WEIGHT_BAL(layer)  CPISI_DIFF(CPISI_HEALTH_WEIGHT(layer), CPISI_HEALTH_WEIGHT_CENTER)

/** @brief Validate weight [1, 9]. */
#define CPISI_IS_WEIGHT(w)              CPISI_IN_RANGE((w), CPISI_HEALTH_WEIGHT_MIN, CPISI_HEALTH_WEIGHT_MAX)

/** @} */ /* end S.2b.4h WEIGHT OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4i HALF OPS — Layer half validation
 *
 * Half values are CENTER_OF(size) for each layer's buffer size.
 * Used for balanced indexing: indices run from -half to +half.
 *
 * BOUNDS USED:
 *   HALF_MIN = 0 (S.2a.7)      — minimum half (degenerate case)
 *   HALF_MAX = 3280 (S.2a.7)   — maximum half = CENTER_OF(6561) = (6561-1)/2
 *
 * HALF TABLE (for layer powers):
 *   HALF(1) = 0, HALF(3) = 1, HALF(9) = 4, HALF(27) = 13,
 *   HALF(81) = 40, HALF(243) = 121, HALF(729) = 364,
 *   HALF(2187) = 1093, HALF(6561) = 3280
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4i_HALF S.2b.4i HALF OPS — Layer half validation
 * @ingroup S2b_4_HEALTH
 * @brief Validate half values for balanced indexing.
 *
 * Half = (size - 1) / 2 = center index for balanced iteration.
 * Valid halves range from 0 (for size=1) to 3280 (for size=6561).
 * @{ */

/** @brief Validate half value [0, 3280]. */
#define CPISI_IS_LAYER_HALF(h)      CPISI_IN_RANGE((h), CPISI_HALF_MIN, CPISI_HALF_MAX)

/** @} */ /* end S.2b.4i HALF OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4j ZONE OPS — Zone index and boundary validation
 *
 * 5 zones map health values to Hebrew state categories via 4 boundaries.
 * Zone indices [0..4] correspond to: CRITICAL, LOW, BALANCED, HIGH, OPTIMAL.
 *
 * BOUNDS USED:
 *   ZONE_MIN = 0 (S.2a.5b.2f)         — CRITICAL zone
 *   ZONE_CENTER = 2 (S.2a.5b.2f)      — BALANCED zone (CENTER)
 *   ZONE_MAX = 4 (S.2a.5b.2f)         — OPTIMAL zone
 *   ZONE_COUNT = 5 (S.2a.5b.2f)       — total zones
 *   ZONE_BOUNDARY_COUNT = 4 (S.2a.5b.2f) — boundaries between zones
 *
 * ZONE TABLE:
 *   Zone Index │ Name     │ Display Range  │ Hebrew State
 *   ───────────┼──────────┼────────────────┼──────────────
 *   0          │ CRITICAL │ -100..-51      │ shavar (broken)
 *   1          │ LOW      │  -50..-26      │ chaser/ratsah
 *   2          │ BALANCED │  -25..+25      │ yashar (center)
 *   3          │ HIGH     │  +26..+50      │ tamim/shalem
 *   4          │ OPTIMAL  │  +51..+100     │ tov (perfect)
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4j_ZONE S.2b.4j ZONE OPS — Zone index operations
 * @ingroup S2b_4_HEALTH
 * @brief Validate zone indices and boundaries for health categorization.
 *
 * Zones provide categorical interpretation of continuous health values.
 * The 5 zones map to 7 Hebrew states via 4 boundaries at ±25 and ±50 display units.
 * @{ */

/**
 * @brief Validate zone index — VALUE → BOOL (true if valid zone).
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_MIN = 0 (S.2a.5b.2f) — CRITICAL zone
 *   - CPISI_ZONE_MAX = 4 (S.2a.5b.2f) — OPTIMAL zone
 *
 * FORMULA: CPISI_IN_RANGE(z, 0, 4)
 *   Returns true if 0 ≤ z ≤ 4, false otherwise.
 *
 * USE CASE:
 *   - Input validation before zone lookup
 *   - Guard condition for zone-based operations
 *   - Assertion in debug builds
 *
 * EXAMPLE:
 *   CPISI_IS_ZONE(0) → true  (CRITICAL)
 *   CPISI_IS_ZONE(2) → true  (BALANCED)
 *   CPISI_IS_ZONE(4) → true  (OPTIMAL)
 *   CPISI_IS_ZONE(5) → false (out of range)
 *
 * @param z Zone index to validate (any integer)
 * @return true if z ∈ [0, 4], false otherwise
 */
#define CPISI_IS_ZONE(z)            CPISI_IN_RANGE((z), CPISI_ZONE_MIN, CPISI_ZONE_MAX)

/**
 * @brief Validate zone boundary index — VALUE → BOOL (true if valid boundary).
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_MIN = 0 (S.2a.5b.2f) — first boundary index
 *   - CPISI_ZONE_BOUNDARY_COUNT = 4 (S.2a.5b.2f) — total boundaries
 *
 * FORMULA: CPISI_IN_RANGE(b, 0, 3)
 *   4 boundaries divide 5 zones. Valid indices are [0, 3].
 *
 * BOUNDARY MAPPING:
 *   [0] = -50 (BROKEN threshold)
 *   [1] = -25 (LOWER threshold)
 *   [2] = +25 (UPPER threshold)
 *   [3] = +50 (PERFECT threshold)
 *
 * USE CASE:
 *   - Input validation before boundary lookup
 *   - Guard condition for threshold comparisons
 *
 * EXAMPLE:
 *   CPISI_IS_ZONE_BOUNDARY(0) → true  (BROKEN)
 *   CPISI_IS_ZONE_BOUNDARY(3) → true  (PERFECT)
 *   CPISI_IS_ZONE_BOUNDARY(4) → false (out of range)
 *
 * @param b Boundary index to validate (any integer)
 * @return true if b ∈ [0, 3], false otherwise
 */
#define CPISI_IS_ZONE_BOUNDARY(b)   CPISI_IN_RANGE((b), CPISI_ZONE_MIN, CPISI_DIFF(CPISI_ZONE_BOUNDARY_COUNT, CPISI_UNITY))

/**
 * @brief Check if zone is at center — INDEX → BOOL (true if BALANCED).
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_CENTER = 2 (S.2a.5b.2f) — BALANCED zone index
 *
 * FORMULA: (z == CPISI_ZONE_CENTER)
 *   Returns true only for zone index 2 (BALANCED/yashar).
 *
 * SEMANTIC MEANING:
 *   BALANCED (center) represents yashar — the even/default state.
 *   This is the target equilibrium for health restoration.
 *
 * USE CASE:
 *   - Check if health is at equilibrium
 *   - Skip correction when already balanced
 *   - Identify center position in zone-based UI
 *
 * EXAMPLE:
 *   CPISI_IS_ZONE_CENTER(0) → false (CRITICAL)
 *   CPISI_IS_ZONE_CENTER(2) → true  (BALANCED)
 *   CPISI_IS_ZONE_CENTER(4) → false (OPTIMAL)
 *
 * @param z Zone index to check (should be 0-4)
 * @return true if z == 2, false otherwise
 */
#define CPISI_IS_ZONE_CENTER(z)     ((z) == CPISI_ZONE_CENTER)

/**
 * @brief Check if zone is at edge — INDEX → BOOL (true if CRITICAL or OPTIMAL).
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_MIN = 0 (S.2a.5b.2f) — CRITICAL zone (negative edge)
 *   - CPISI_ZONE_MAX = 4 (S.2a.5b.2f) — OPTIMAL zone (positive edge)
 *
 * FORMULA: (z == CPISI_ZONE_MIN) || (z == CPISI_ZONE_MAX)
 *   Returns true for zone 0 (CRITICAL/shavar) or zone 4 (OPTIMAL/tov).
 *
 * SEMANTIC MEANING:
 *   Edge zones represent extremes: crisis (shavar) or completion (tov).
 *   Both require special handling — one needs recovery, other is peak state.
 *
 * USE CASE:
 *   - Trigger alerts for edge conditions
 *   - Special UI treatment for extreme states
 *   - Determine if stabilization is needed
 *
 * EXAMPLE:
 *   CPISI_IS_ZONE_EDGE(0) → true  (CRITICAL - crisis)
 *   CPISI_IS_ZONE_EDGE(2) → false (BALANCED - center)
 *   CPISI_IS_ZONE_EDGE(4) → true  (OPTIMAL - peak)
 *
 * @param z Zone index to check (should be 0-4)
 * @return true if z == 0 or z == 4, false otherwise
 */
#define CPISI_IS_ZONE_EDGE(z)       (((z) == CPISI_ZONE_MIN) || ((z) == CPISI_ZONE_MAX))

/** @} */ /* end S.2b.4j ZONE OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4k IMPACT OPS — Severity↔impact conversion and validation
 *
 * Impact/severity system provides 9-level granularity (-4..+4 severity,
 * -100..+100 display impact). Conversions use SEVERITY_STEP (25).
 *
 * BOUNDS USED:
 *   HEALTH_SEVERITY_MIN = -4 (S.2a.6)    HEALTH_SEVERITY_MAX = +4 (S.2a.6)
 *   HEALTH_SEVERITY_STEP = 25 (S.2a.6)   — display units per severity
 *   IMPACT_FATAL = -100 (S.2a.6)         IMPACT_PERFECT = +100 (S.2a.6)
 *
 * CONVERSION TABLE:
 *   Severity │ Impact  │ Meaning
 *   ─────────┼─────────┼──────────────
 *     -4     │  -100   │ FATAL
 *     -2     │   -50   │ MEDIUM
 *      0     │     0   │ NONE
 *     +2     │   +50   │ GOOD
 *     +4     │  +100   │ PERFECT
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4k_IMPACT S.2b.4k IMPACT OPS — Severity↔impact operations
 * @ingroup S2b_4_HEALTH
 * @brief Bidirectional severity↔impact conversion and validation.
 *
 * The impact system maps 9 severity levels (-4..+4) to display values (-100..+100).
 * Formula: impact = severity × 25, severity = impact / 25.
 *
 * IMPACT MAP:
 * ```
 *   Severity │ Impact │ State
 *   ─────────┼────────┼───────────
 *      -4    │  -100  │ FATAL
 *      -3    │   -75  │ HIGH
 *      -2    │   -50  │ MEDIUM
 *      -1    │   -25  │ LOW
 *       0    │     0  │ NONE
 *      +1    │   +25  │ FAIR
 *      +2    │   +50  │ GOOD
 *      +3    │   +75  │ EXCELLENT
 *      +4    │  +100  │ PERFECT
 * ```
 * @{ */

/**
 * @brief Convert severity level to display impact — SEVERITY → DISPLAY.
 *
 * BOUNDS USED:
 *   - CPISI_HEALTH_SEVERITY_STEP = 25 (S.2a.6) — display units per severity
 *
 * FORMULA: CPISI_PRODUCT(sev, CPISI_HEALTH_SEVERITY_STEP)
 *   severity × 25 = impact
 *   Range: severity (-4..+4) → impact (-100..+100)
 *
 * USE CASE:
 *   - Convert internal severity level to user-facing display value
 *   - Populate impact display arrays from severity values
 *   - Health bar percentage calculation
 *
 * EXAMPLE:
 *   CPISI_HEALTH_SEVERITY_TO_IMPACT(-4) → -100 (FATAL)
 *   CPISI_HEALTH_SEVERITY_TO_IMPACT(0)  →    0 (NONE)
 *   CPISI_HEALTH_SEVERITY_TO_IMPACT(+4) → +100 (PERFECT)
 *
 * @param sev Severity level (-4 to +4)
 * @return Display impact value (-100 to +100)
 */
#define CPISI_HEALTH_SEVERITY_TO_IMPACT(sev)  CPISI_PRODUCT((sev), CPISI_HEALTH_SEVERITY_STEP)

/**
 * @brief Convert display impact to severity level — DISPLAY → SEVERITY.
 *
 * BOUNDS USED:
 *   - CPISI_HEALTH_SEVERITY_STEP = 25 (S.2a.6) — display units per severity
 *
 * FORMULA: CPISI_QUOTIENT(imp, CPISI_HEALTH_SEVERITY_STEP)
 *   impact / 25 = severity
 *   Range: impact (-100..+100) → severity (-4..+4)
 *
 * USE CASE:
 *   - Parse user input or external data to internal severity
 *   - Snap display value to nearest severity level
 *   - Threshold comparison in severity units
 *
 * EXAMPLE:
 *   CPISI_IMPACT_TO_HEALTH_SEVERITY(-100) → -4 (FATAL)
 *   CPISI_IMPACT_TO_HEALTH_SEVERITY(0)    →  0 (NONE)
 *   CPISI_IMPACT_TO_HEALTH_SEVERITY(+100) → +4 (PERFECT)
 *
 * @param imp Display impact value (-100 to +100)
 * @return Severity level (-4 to +4)
 */
#define CPISI_IMPACT_TO_HEALTH_SEVERITY(imp)  CPISI_QUOTIENT((imp), CPISI_HEALTH_SEVERITY_STEP)

/**
 * @brief Validate severity level — VALUE → BOOL (true if valid severity).
 *
 * BOUNDS USED:
 *   - CPISI_HEALTH_SEVERITY_MIN = -4 (S.2a.6) — FATAL edge
 *   - CPISI_HEALTH_SEVERITY_MAX = +4 (S.2a.6) — PERFECT edge
 *
 * FORMULA: CPISI_IN_RANGE(s, -4, +4)
 *   Returns true if -4 ≤ s ≤ +4, false otherwise.
 *
 * USE CASE:
 *   - Input validation before severity operations
 *   - Guard condition for severity-indexed arrays
 *   - Assertion in debug builds
 *
 * EXAMPLE:
 *   CPISI_IS_HEALTH_SEVERITY(-4) → true  (FATAL)
 *   CPISI_IS_HEALTH_SEVERITY(0)  → true  (NONE)
 *   CPISI_IS_HEALTH_SEVERITY(+4) → true  (PERFECT)
 *   CPISI_IS_HEALTH_SEVERITY(5)  → false (out of range)
 *
 * @param s Severity value to validate (any integer)
 * @return true if s ∈ [-4, +4], false otherwise
 */
#define CPISI_IS_HEALTH_SEVERITY(s) CPISI_IN_RANGE((s), CPISI_HEALTH_SEVERITY_MIN, CPISI_HEALTH_SEVERITY_MAX)

/**
 * @brief Validate impact value — VALUE → BOOL (true if valid impact).
 *
 * BOUNDS USED:
 *   - CPISI_IMPACT_FATAL = -100 (S.2a.6) — minimum impact
 *   - CPISI_IMPACT_PERFECT = +100 (S.2a.6) — maximum impact
 *
 * FORMULA: CPISI_IN_RANGE(i, -100, +100)
 *   Returns true if -100 ≤ i ≤ +100, false otherwise.
 *
 * USE CASE:
 *   - Input validation before impact operations
 *   - Guard condition for impact-based display
 *   - Assertion in debug builds
 *
 * EXAMPLE:
 *   CPISI_IS_IMPACT(-100) → true  (FATAL)
 *   CPISI_IS_IMPACT(0)    → true  (NONE)
 *   CPISI_IS_IMPACT(+100) → true  (PERFECT)
 *   CPISI_IS_IMPACT(101)  → false (out of range)
 *
 * @param i Impact value to validate (any integer)
 * @return true if i ∈ [-100, +100], false otherwise
 */
#define CPISI_IS_IMPACT(i)          CPISI_IN_RANGE((i), CPISI_IMPACT_FATAL, CPISI_IMPACT_PERFECT)

/** @} */ /* end S.2b.4k IMPACT OPS */

/* ─────────────────────────────────────────────────────────────────────────────
 * S.2b.4l NORMALIZATION OPS — True score, k-factor, positions, asymmetry
 *
 * BOUNDS USED:
 *   TRUE_MIN = -128, TRUE_CENTER = 0, TRUE_MAX = +127 (S.2a.5b.2i)
 *   K_MIN = -1, K_CENTER = 0, K_MAX = +1, K_COUNT = 3 (S.2a.5b.2i)
 *   NORM_DIVISOR_NEG = 128.0f, NORM_DIVISOR_POS = 127.0f (S.2a.5b.2i)
 *   POSITIONS_NEG = 128, POSITIONS_POS = 127 (S.2a.5b.2i)
 *
 * ASYMMETRY PRINCIPLE:
 *   128 paths to corruption (shavar) vs 127 paths to wholeness (tov).
 *   One more step to destruction than to restoration.
 *   "Many ways to fall, narrow path to life."
 *
 * MACRO CATEGORIES:
 *   CONVERSION:  TRUE_TO_NORMALIZED, TRUE_TO_K
 *   SELECTION:   K_TO_DIVISOR, K_TO_POSITIONS
 *   ASYMMETRY:   ASYMMETRY_DIFF (computed from bounds)
 *   VALIDATION:  TRUE_IN_RANGE, NORM_IN_RANGE, K_IN_RANGE
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @defgroup S2b_4l_NORMALIZATION S.2b.4l NORMALIZATION OPS — Normalization operations
 * @ingroup S2b_4_HEALTH
 * @brief Asymmetric normalization from true score to k-factor and normalized range.
 *
 * TRUE SCORE is the balanced layer value (-128..+127).
 * K-FACTOR indicates direction from yashar anchor:
 *   k = -1: toward shavar (many paths to corruption)
 *   k =  0: at yashar (anchor, stability)
 *   k = +1: toward tov (one path to wholeness)
 *
 * NORMALIZED SCORE maps to [-1.0, +1.0] using asymmetric divisors:
 *   - negative: balanced / 128.0 → -1.0 at -128
 *   - positive: balanced / 127.0 → +1.0 at +127
 *
 * Structure:
 *   S.2b.4l.1 CONVERSION  — True score to normalized and k-factor (2 macros)
 *   S.2b.4l.2 SELECTION   — Select divisor/positions by k-factor (2 macros)
 *   S.2b.4l.3 ASYMMETRY   — Position difference constant (1 macro)
 *   S.2b.4l.4 VALIDATION  — Range checks for true, norm, k (3 macros)
 * @{ */

/* ── S.2b.4l.1 CONVERSION — True score to normalized and k-factor ─────────── */

/**
 * @brief Convert true score to normalized value.
 *
 * BOUNDS USED:
 *   - CPISI_HEALTH_BAL_TO_TERNARY (S.2b.4e.2) — asymmetric conversion macro
 *   - TRUE_MIN (-128), TRUE_MAX (+127) — input range
 *   - NORM_MIN (-1.0), NORM_MAX (+1.0) — output range
 *
 * FORMULA:
 *   Delegates to HEALTH_BAL_TO_TERNARY which applies asymmetric divisors:
 *   t < 0: t / 128.0 (divide by NORM_DIVISOR_NEG)
 *   t = 0: 0.0 (identity at center)
 *   t > 0: t / 127.0 (divide by NORM_DIVISOR_POS)
 *
 * USE CASE:
 *   Convert integer health scores to floating-point normalized range
 *   for interpolation, mixing, and threshold comparisons.
 *
 * EXAMPLE:
 *   CPISI_TRUE_TO_NORMALIZED(-128) → -1.0 (shavar edge)
 *   CPISI_TRUE_TO_NORMALIZED(0)    →  0.0 (yashar anchor)
 *   CPISI_TRUE_TO_NORMALIZED(+127) → +1.0 (tov edge)
 *   CPISI_TRUE_TO_NORMALIZED(-64)  → -0.5 (half to shavar)
 *
 * @param t  True score value (-128 to +127)
 * @return   Normalized value (-1.0 to +1.0)
 */
#define CPISI_TRUE_TO_NORMALIZED(t) CPISI_HEALTH_BAL_TO_TERNARY(t)

/**
 * @brief Derive k-factor from true score sign.
 *
 * BOUNDS USED:
 *   - K_MIN (-1), K_CENTER (0), K_MAX (+1) — output values
 *   - TRUE comparison against 0 — determines direction
 *
 * FORMULA:
 *   t < 0 ? K_MIN : (t > 0 ? K_MAX : K_CENTER)
 *
 * USE CASE:
 *   Determine direction of health change from yashar anchor.
 *   K-factor is used for selecting asymmetric divisors and position counts.
 *
 * EXAMPLE:
 *   CPISI_TRUE_TO_K(-50) → -1 (toward shavar)
 *   CPISI_TRUE_TO_K(0)   →  0 (at yashar anchor)
 *   CPISI_TRUE_TO_K(+50) → +1 (toward tov)
 *
 * RELATIONSHIP TO:
 *   - K_TO_DIVISOR: k → divisor for normalization
 *   - K_TO_POSITIONS: k → step count for that direction
 *
 * @param t  True score value (-128 to +127)
 * @return   K-factor (-1, 0, or +1)
 */
#define CPISI_TRUE_TO_K(t)          ((t) < 0 ? CPISI_K_MIN : ((t) > 0 ? CPISI_K_MAX : CPISI_K_CENTER))

/* ── S.2b.4l.2 SELECTION — Select divisor/positions by k-factor ───────────── */

/**
 * @brief Select asymmetric divisor based on k-factor.
 *
 * BOUNDS USED:
 *   - NORM_DIVISOR_NEG (128.0f) — for k < 0
 *   - NORM_DIVISOR_CENTER (1.0f) — for k = 0 (identity)
 *   - NORM_DIVISOR_POS (127.0f) — for k > 0
 *
 * FORMULA:
 *   k < 0 ? DIVISOR_NEG : (k > 0 ? DIVISOR_POS : DIVISOR_CENTER)
 *
 * USE CASE:
 *   Get the appropriate divisor for normalizing a true score.
 *   Asymmetric divisors ensure full [-1.0, +1.0] range coverage.
 *
 * EXAMPLE:
 *   CPISI_K_TO_DIVISOR(-1) → 128.0f (corruption divisor)
 *   CPISI_K_TO_DIVISOR(0)  →   1.0f (identity)
 *   CPISI_K_TO_DIVISOR(+1) → 127.0f (restoration divisor)
 *
 * RELATIONSHIP TO:
 *   - TRUE_TO_NORMALIZED: uses this divisor internally
 *   - g_cpisi_divisor[]: table equivalent for O(1) lookup
 *
 * @param k  K-factor value (-1, 0, or +1)
 * @return   Normalization divisor (128.0f, 1.0f, or 127.0f)
 */
#define CPISI_K_TO_DIVISOR(k)       ((k) < 0 ? CPISI_NORM_DIVISOR_NEG : \
                                     ((k) > 0 ? CPISI_NORM_DIVISOR_POS : CPISI_NORM_DIVISOR_CENTER))

/**
 * @brief Select position count based on k-factor.
 *
 * BOUNDS USED:
 *   - POSITIONS_NEG (128) — corruption steps
 *   - POSITIONS_CENTER (1) — yashar anchor only
 *   - POSITIONS_POS (127) — restoration steps
 *
 * FORMULA:
 *   k < 0 ? POSITIONS_NEG : (k > 0 ? POSITIONS_POS : POSITIONS_CENTER)
 *
 * USE CASE:
 *   Get the number of discrete positions in a given direction.
 *   Used for progress calculation and step counting.
 *
 * EXAMPLE:
 *   CPISI_K_TO_POSITIONS(-1) → 128 (corruption steps: -128 to -1)
 *   CPISI_K_TO_POSITIONS(0)  →   1 (yashar anchor only)
 *   CPISI_K_TO_POSITIONS(+1) → 127 (restoration steps: +1 to +127)
 *
 * RELATIONSHIP TO:
 *   - POSITIONS_TOTAL: 128+1+127 = 256
 *   - g_cpisi_positions[]: table equivalent for O(1) lookup
 *
 * @param k  K-factor value (-1, 0, or +1)
 * @return   Position count (128, 1, or 127)
 */
#define CPISI_K_TO_POSITIONS(k)     ((k) < 0 ? CPISI_POSITIONS_NEG : \
                                     ((k) > 0 ? CPISI_POSITIONS_POS : CPISI_POSITIONS_CENTER))

/* ── S.2b.4l.3 ASYMMETRY — Position difference constant ───────────────────── */

/**
 * @brief Compute asymmetry difference (128 - 127 = 1).
 *
 * BOUNDS USED:
 *   - POSITIONS_NEG (128), POSITIONS_POS (127)
 *   - CPISI_DIFF macro (S.2a.1) — difference operation
 *
 * FORMULA:
 *   DIFF(POSITIONS_NEG, POSITIONS_POS) = 128 - 127 = 1
 *
 * SEMANTIC MEANING:
 *   One more step to corruption than to restoration.
 *   "Many ways to fall, narrow path to life."
 *
 * USE CASE:
 *   Document and verify the asymmetry principle.
 *   Used in tripwires to ensure asymmetry is maintained.
 *
 * VALUE MAPPING:
 *   Always evaluates to 1 (UNITY)
 *
 * RELATIONSHIP TO:
 *   - ASYMMETRY_DIFF bound (S.2a.5b.2i) — same value, derived form
 *
 * @return 1 (the asymmetry difference)
 */
#define CPISI_ASYMMETRY_DIFF_COMPUTED CPISI_DIFF(CPISI_POSITIONS_NEG, CPISI_POSITIONS_POS)

/* ── S.2b.4l.4 VALIDATION — Range checks for true, norm, k ────────────────── */

/**
 * @brief Check if value is valid true score.
 *
 * BOUNDS USED:
 *   - TRUE_MIN (-128), TRUE_MAX (+127) — valid range
 *   - CPISI_IN_RANGE macro (S.2a.1) — range check
 *
 * FORMULA:
 *   IN_RANGE(t, TRUE_MIN, TRUE_MAX)
 *   Expands to: t >= -128 && t <= +127
 *
 * USE CASE:
 *   Validate true score before conversion operations.
 *   Guard against out-of-range inputs.
 *
 * EXAMPLE:
 *   CPISI_TRUE_IN_RANGE(-128) → true (min edge)
 *   CPISI_TRUE_IN_RANGE(0)    → true (center)
 *   CPISI_TRUE_IN_RANGE(+127) → true (max edge)
 *   CPISI_TRUE_IN_RANGE(-129) → false (below min)
 *   CPISI_TRUE_IN_RANGE(+128) → false (above max)
 *
 * @param t  True score value to validate
 * @return   true if t ∈ [-128, +127], false otherwise
 */
#define CPISI_TRUE_IN_RANGE(t)      CPISI_IN_RANGE((t), CPISI_TRUE_MIN, CPISI_TRUE_MAX)

/**
 * @brief Check if value is valid normalized.
 *
 * BOUNDS USED:
 *   - NORM_MIN (-1.0f), NORM_MAX (+1.0f) — valid range
 *
 * FORMULA:
 *   (n >= NORM_MIN) && (n <= NORM_MAX)
 *
 * USE CASE:
 *   Validate normalized values from external sources.
 *   Ensure interpolated/computed values stay in range.
 *
 * EXAMPLE:
 *   CPISI_NORM_IN_RANGE(-1.0f) → true (min edge)
 *   CPISI_NORM_IN_RANGE(0.0f)  → true (center)
 *   CPISI_NORM_IN_RANGE(+1.0f) → true (max edge)
 *   CPISI_NORM_IN_RANGE(-1.1f) → false (below min)
 *   CPISI_NORM_IN_RANGE(+1.1f) → false (above max)
 *
 * @param n  Normalized value to validate
 * @return   true if n ∈ [-1.0, +1.0], false otherwise
 */
#define CPISI_NORM_IN_RANGE(n)      ((n) >= CPISI_NORM_MIN && (n) <= CPISI_NORM_MAX)

/**
 * @brief Check if value is valid k-factor.
 *
 * BOUNDS USED:
 *   - K_MIN (-1), K_MAX (+1) — valid range
 *   - CPISI_IN_RANGE macro (S.2a.1) — range check
 *
 * FORMULA:
 *   IN_RANGE(k, K_MIN, K_MAX)
 *   Expands to: k >= -1 && k <= +1
 *
 * USE CASE:
 *   Validate k-factor before using it to select divisors/positions.
 *   Guard against invalid direction values.
 *
 * EXAMPLE:
 *   CPISI_K_IN_RANGE(-1) → true (retreat)
 *   CPISI_K_IN_RANGE(0)  → true (maintain)
 *   CPISI_K_IN_RANGE(+1) → true (advance)
 *   CPISI_K_IN_RANGE(-2) → false (invalid)
 *   CPISI_K_IN_RANGE(+2) → false (invalid)
 *
 * @param k  K-factor value to validate
 * @return   true if k ∈ [-1, +1], false otherwise
 */
#define CPISI_K_IN_RANGE(k)         CPISI_IN_RANGE((k), CPISI_K_MIN, CPISI_K_MAX)

/** @} */ /* end S.2b.4l NORMALIZATION OPS */
/** @} */ /* end S.2b.4 HEALTH MACROS */

/** @} */ /* end S.2b HEALTH MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c VERIFY — Health compile-time assertions [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_verify S.2c HEALTH VERIFY — Compile-time assertions
 * @ingroup cpisi_health_domain
 * @brief    Verify health pipeline, milestone, zone, and normalization architecture.
 *
 * PURPOSE: Catch health configuration errors BEFORE runtime. Zero runtime cost.
 *          If any assertion fails, compilation stops with a descriptive message.
 *
 * Builds FROM: S.2a HEALTH BOUNDS (pipeline values), S.2b HEALTH MACROS (derivations)
 * Builds TO:   Health monitoring systems (verified values ready for use)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC tripwires extending S.2c VERIFY in types.h.
 *   Core tripwires are in types.h. This file verifies the 4-layer health pipeline
 *   and its derived values (milestones, weights, zones, impacts).
 *
 * TRIPWIRE PHILOSOPHY:
 *   Each value is verified TWO ways:
 *   1. VALUE TRIPWIRE:        Assert the literal (e.g., RAW_MAX == 255)
 *   2. RELATIONSHIP TRIPWIRE: Assert the derivation (e.g., DISPLAY_CENTER == 0)
 *   BOTH together catches drift in either direction.
 *
 * STRUCTURE:
 *   S.2c.3 HEALTH VERIFY — Verify health bounds
 *     S.2c.3a VALUE         — HEALTH_COUNT, SEVERITY_COUNT, TREND_COUNT
 *     S.2c.3b PIPELINE VALUE — RAW, BALANCED, DISPLAY range values
 *     S.2c.3c PIPELINE REL  — Pipeline derivation relationships
 *     S.2c.3d MILESTONE     — 9 milestones at 25-unit intervals
 *     S.2c.3e WEIGHT        — Layer weights (1-9, sum=45)
 *     S.2c.3f HALF          — Layer HALF values (0 to 3280)
 *     S.2c.3g ZONE          — 5→7→9 zone progression
 *     S.2c.3h IMPACT        — Impact/severity values (-4..+4, -100..+100)
 *     S.2c.3i NORMALIZATION — True score, k-factor, positions, asymmetry
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2c_3_HEALTH S.2c.3 HEALTH VERIFY — Verify health bounds
 * @ingroup cpisi_health_verify
 * @brief Verify health pipeline bounds, milestones, weights, and halves.
 *
 * WHAT THIS VERIFIES:
 *   The complete 4-layer health pipeline from S.2a.4 HEALTH bounds.
 *   These values drive the diagnostic and monitoring system.
 *
 * 4-LAYER PIPELINE VERIFIED:
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  LAYER       RANGE         POSITIONS    PURPOSE                     │
 *   │  ──────────  ────────────  ──────────   ──────────────────────      │
 *   │  RAW         0..255        256          uint8_t storage             │
 *   │  BALANCED    -128..+127    256          int8_t for arithmetic       │
 *   │  TERNARY     -1..+1        3            Sign direction only         │
 *   │  DISPLAY     -100..+100    201          Percentage display          │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * STRUCTURE:
 *   S.2c.3a VALUE         — HEALTH_COUNT, SEVERITY_COUNT, TREND_COUNT
 *   S.2c.3b PIPELINE VALUE — RAW, BALANCED, DISPLAY range values
 *   S.2c.3c PIPELINE REL  — Pipeline derivation relationships
 *   S.2c.3d MILESTONE     — 9 milestones at 25-unit intervals
 *   S.2c.3e WEIGHT        — Layer weights (1-9, sum=45)
 *   S.2c.3f HALF          — Layer HALF values (0 to 3280)
 *   S.2c.3g ZONE          — 5→7→9 zone progression
 *   S.2c.3h IMPACT        — Impact/severity values (-4..+4, -100..+100)
 *   S.2c.3i NORMALIZATION — True score, k-factor, positions, asymmetry
 *
 * VERIFIED FROM: S.2a.4 HEALTH bounds, S.2a.5b.2f ZONE bounds, S.2a.5b.2i NORMALIZATION
 * @{ */

/**
 * @defgroup S2c_3a_VALUE S.2c.3a VALUE TRIPWIRES — Verify health counts
 * @ingroup S2c_3_HEALTH
 * @brief Verify health counts connect to STATES, HARDPOINTS, SIGN_COUNT.
 *
 * COUNT CONNECTIONS:
 *   - HEALTH_COUNT = STATES = 7 (seven Hebrew states)
 *   - SEVERITY_COUNT = HARDPOINTS = 5 (five severity levels)
 *   - TREND_COUNT = SIGN_COUNT = 3 (ternary trend: down/stable/up)
 * @{ */

CPISI_STATIC_ASSERT(CPISI_HEALTH_COUNT == CPISI_STATES,
                    "HEALTH_COUNT must equal STATES (7)");
CPISI_STATIC_ASSERT(CPISI_CENTER_OF(CPISI_HEALTH_COUNT) == 3,
                    "CENTER_OF(HEALTH_COUNT) must be 3 (yashar)");
CPISI_STATIC_ASSERT(CPISI_SEVERITY_COUNT == CPISI_HARDPOINTS,
                    "SEVERITY_COUNT must equal HARDPOINTS (5)");
CPISI_STATIC_ASSERT(CPISI_TREND_COUNT == CPISI_SIGN_COUNT,
                    "TREND_COUNT must equal SIGN_COUNT (3)");


/** @} */ /* end S.2c.3a VALUE */

/**
 * @defgroup S2c_3b_PIPELINE_VALUE S.2c.3b PIPELINE VALUE TRIPWIRES — Verify pipeline literals
 * @ingroup S2c_3_HEALTH
 * @brief Verify RAW, BALANCED, and DISPLAY layer literal values.
 *
 * KEY VALUES:
 *   - RAW: 256 positions (0-255), center at 128
 *   - BALANCED: Asymmetric edges (128 negative, 127 positive)
 *   - DISPLAY: 201 positions (-100 to +100)
 *
 * ASYMMETRY SIGNIFICANCE:
 *   int8_t has 128 negative values (-128..-1) but only 127 positive (1..127).
 *   EDGE_NEG = 128 > EDGE_POS = 127 reflects theological truth:
 *   "Corruption is easier than restoration."
 * @{ */

/* RAW layer — 256 positions */
CPISI_STATIC_ASSERT(CPISI_HEALTH_POSITIONS == 256,
                    "HEALTH_POSITIONS must be 256 — total raw positions");
CPISI_STATIC_ASSERT(CPISI_HEALTH_RAW_CENTER == 128,
                    "HEALTH_RAW_CENTER must be 128 — yashar midpoint");
CPISI_STATIC_ASSERT(CPISI_HEALTH_RAW_MAX == 255,
                    "HEALTH_RAW_MAX must be 255 — tov maximum");

/* BALANCED layer — asymmetric edges */
CPISI_STATIC_ASSERT(CPISI_HEALTH_EDGE_NEG == 128,
                    "HEALTH_EDGE_NEG must be 128 — corruption path steps");
CPISI_STATIC_ASSERT(CPISI_HEALTH_EDGE_POS == 127,
                    "HEALTH_EDGE_POS must be 127 — restoration path steps");

/* DISPLAY layer — 201 positions */
CPISI_STATIC_ASSERT(CPISI_HEALTH_DISPLAY_RANGE == 201,
                    "HEALTH_DISPLAY_RANGE must be 201 — total display positions");


/** @} */ /* end S.2c.3b PIPELINE VALUE */

/**
 * @defgroup S2c_3c_PIPELINE_REL S.2c.3c PIPELINE RELATIONSHIP TRIPWIRES — Verify pipeline derivations
 * @ingroup S2c_3_HEALTH
 * @brief Verify pipeline values derive from BINARY_FIT and display constants.
 *
 * DERIVATION CHAINS:
 *   RAW:      BINARY_FIT(256) → POSITIONS, CENTER=256/2, MAX=256-1
 *   BALANCED: EDGE_NEG=RAW_CENTER=128, EDGE_POS=RAW_MAX-RAW_CENTER=127
 *   DISPLAY:  RANGE = 2×DISPLAY_MAX + 1 = 2×100 + 1 = 201
 *
 * THEOLOGICAL VERIFICATION:
 *   EDGE_NEG > EDGE_POS asserts "corruption is easier than restoration"
 *   — a foundational truth embedded in the type system itself.
 * @{ */

/* RAW derivations */
CPISI_STATIC_ASSERT(CPISI_HEALTH_POSITIONS == CPISI_BINARY_FIT,
                    "HEALTH_POSITIONS must equal BINARY_FIT — verifies 256 = 256");
CPISI_STATIC_ASSERT(CPISI_HEALTH_RAW_CENTER == CPISI_QUOTIENT(CPISI_BINARY_FIT, CPISI_BINARY),
                    "HEALTH_RAW_CENTER must equal BINARY_FIT/2 — verifies 128 = 256/2");
CPISI_STATIC_ASSERT(CPISI_HEALTH_RAW_MAX == CPISI_DIFF(CPISI_BINARY_FIT, CPISI_UNITY),
                    "HEALTH_RAW_MAX must equal BINARY_FIT-1 — verifies 255 = 256-1");

/* BALANCED derivations — asymmetry verification */
CPISI_STATIC_ASSERT(CPISI_HEALTH_EDGE_NEG == CPISI_HEALTH_RAW_CENTER,
                    "HEALTH_EDGE_NEG must equal RAW_CENTER — verifies 128 = 128");
CPISI_STATIC_ASSERT(CPISI_HEALTH_EDGE_POS == CPISI_DIFF(CPISI_HEALTH_RAW_MAX, CPISI_HEALTH_RAW_CENTER),
                    "HEALTH_EDGE_POS must equal RAW_MAX-RAW_CENTER — verifies 127 = 255-128");
CPISI_STATIC_ASSERT(CPISI_HEALTH_EDGE_NEG > CPISI_HEALTH_EDGE_POS,
                    "EDGE_NEG must exceed EDGE_POS — corruption > restoration (theological)");

/* DISPLAY derivations */
CPISI_STATIC_ASSERT(CPISI_HEALTH_DISPLAY_RANGE == CPISI_SUM(CPISI_PRODUCT(CPISI_HEALTH_DISPLAY_MAX, CPISI_BINARY), CPISI_UNITY),
                    "HEALTH_DISPLAY_RANGE must equal 2×MAX+1 — verifies 201 = 2×100+1");


/** @} */ /* end S.2c.3c PIPELINE REL */

/**
 * @defgroup S2c_3d_MILESTONE S.2c.3d MILESTONE TRIPWIRES — Verify milestone values
 * @ingroup S2c_3_HEALTH
 * @brief Verify 9 milestones at 25-unit intervals (-100 to +100).
 *
 * MILESTONE SYSTEM:
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  MILESTONE   DISPLAY    MEANING                                     │
 *   │  ──────────  ─────────  ───────────────────────────────────────     │
 *   │  -4          -100       Critical failure (shavar)                   │
 *   │  -3          -75        Severe degradation                          │
 *   │  -2          -50        Moderate degradation                        │
 *   │  -1          -25        Minor degradation                           │
 *   │   0           0         Stable (yashar)                             │
 *   │  +1          +25        Minor improvement                           │
 *   │  +2          +50        Moderate improvement                        │
 *   │  +3          +75        Strong improvement                          │
 *   │  +4          +100       Peak health (tov)                           │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * DERIVATIONS:
 *   - COUNT = FACE = 9 (nine milestones match nine layers)
 *   - STEP = DISPLAY_MAX / FACE_CENTER = 100 / 4 = 25
 *   - MAX × STEP = DISPLAY_MAX: 4 × 25 = 100 ✓
 * @{ */

/* VALUE tripwires — verify literals */
CPISI_STATIC_ASSERT(CPISI_MILESTONE_COUNT == 9,
                    "MILESTONE_COUNT must be 9 — total milestones");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_STEP == 25,
                    "MILESTONE_STEP must be 25 — display units per milestone");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_MAX == 4,
                    "MILESTONE_MAX must be +4 — maximum milestone index");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_MIN == -4,
                    "MILESTONE_MIN must be -4 — minimum milestone index");

/* RELATIONSHIP tripwires — verify derivations */
CPISI_STATIC_ASSERT(CPISI_MILESTONE_COUNT == CPISI_FACE,
                    "MILESTONE_COUNT must equal FACE — verifies 9 = 9");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_STEP == CPISI_QUOTIENT(CPISI_HEALTH_DISPLAY_MAX, CPISI_FACE_CENTER),
                    "MILESTONE_STEP must equal DISPLAY_MAX/FACE_CENTER — verifies 25 = 100/4");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_MAX == CPISI_FACE_CENTER,
                    "MILESTONE_MAX must equal FACE_CENTER — verifies 4 = 4");
CPISI_STATIC_ASSERT(CPISI_PRODUCT(CPISI_MILESTONE_MAX, CPISI_MILESTONE_STEP) == CPISI_HEALTH_DISPLAY_MAX,
                    "MILESTONE_MAX × MILESTONE_STEP must equal DISPLAY_MAX — verifies 4×25=100");

/* DISPLAY bounds tripwires — verify display range */
CPISI_STATIC_ASSERT(CPISI_MILESTONE_DISPLAY_MIN == -100,
                    "MILESTONE_DISPLAY_MIN must be -100 — shavar edge");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_DISPLAY_CENTER == 0,
                    "MILESTONE_DISPLAY_CENTER must be 0 — yashar center");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_DISPLAY_MAX == 100,
                    "MILESTONE_DISPLAY_MAX must be +100 — tov edge");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_DISPLAY_MIN == CPISI_HEALTH_DISPLAY_MIN,
                    "MILESTONE_DISPLAY_MIN must equal HEALTH_DISPLAY_MIN — verifies -100 = -100");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_DISPLAY_CENTER == CPISI_HEALTH_DISPLAY_CENTER,
                    "MILESTONE_DISPLAY_CENTER must equal HEALTH_DISPLAY_CENTER — verifies 0 = 0");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_DISPLAY_MAX == CPISI_HEALTH_DISPLAY_MAX,
                    "MILESTONE_DISPLAY_MAX must equal HEALTH_DISPLAY_MAX — verifies +100 = +100");

/* EDGE value tripwires — verify milestone display values */
CPISI_STATIC_ASSERT(CPISI_MILESTONE_NEG_4 == -100,
                    "MILESTONE_NEG_4 must be -100 — shavar edge");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_NEUTRAL == 0,
                    "MILESTONE_NEUTRAL must be 0 — yashar center");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_POS_4 == 100,
                    "MILESTONE_POS_4 must be +100 — tov edge");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_NEG_4 == CPISI_NEGATE(CPISI_ZONE_EDGE),
                    "MILESTONE_NEG_4 must equal -ZONE_EDGE — verifies -100 = -100");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_POS_4 == CPISI_ZONE_EDGE,
                    "MILESTONE_POS_4 must equal ZONE_EDGE — verifies +100 = +100");

/* SYMMETRY tripwires — verify NEG_n == -POS_n */
CPISI_STATIC_ASSERT(CPISI_MILESTONE_NEG_4 == CPISI_NEGATE(CPISI_MILESTONE_POS_4),
                    "NEG_4 must equal -POS_4 — symmetric (±100)");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_NEG_3 == CPISI_NEGATE(CPISI_MILESTONE_POS_3),
                    "NEG_3 must equal -POS_3 — symmetric (±75)");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_NEG_2 == CPISI_NEGATE(CPISI_MILESTONE_POS_2),
                    "NEG_2 must equal -POS_2 — symmetric (±50)");
CPISI_STATIC_ASSERT(CPISI_MILESTONE_NEG_1 == CPISI_NEGATE(CPISI_MILESTONE_POS_1),
                    "NEG_1 must equal -POS_1 — symmetric (±25)");


/** @} */ /* end S.2c.3d MILESTONE */

/**
 * @defgroup S2c_3e_WEIGHT S.2c.3e WEIGHT TRIPWIRES — Verify weight values
 * @ingroup S2c_3_HEALTH
 * @brief Verify layer weights (1-9) with inverted priority (KERNEL=9, APP=1).
 *
 * WEIGHT SYSTEM:
 *   Lower layers (closer to KERNEL) have HIGHER weights.
 *   This reflects foundational importance: if KERNEL fails, everything fails.
 *
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  LAYER       INDEX    WEIGHT    RATIONALE                           │
 *   │  ──────────  ─────    ───────   ──────────────────────────────      │
 *   │  KERNEL      0        9         Foundation — most critical          │
 *   │  ...         ...      ...       ...                                 │
 *   │  HEALTH      4        5         Center — diagnostic hub             │
 *   │  ...         ...      ...       ...                                 │
 *   │  APP         8        1         Application — least critical        │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * TOTAL: 1+2+3+4+5+6+7+8+9 = 45 = FACE×(FACE+1)/2 (triangular number)
 * @{ */

/* VALUE tripwires — verify literals */
CPISI_STATIC_ASSERT(CPISI_HEALTH_WEIGHT_MIN == 1,
                    "WEIGHT_MIN must be 1 — APP layer (least)");
CPISI_STATIC_ASSERT(CPISI_HEALTH_WEIGHT_CENTER == 5,
                    "WEIGHT_CENTER must be 5 — HEALTH layer");
CPISI_STATIC_ASSERT(CPISI_HEALTH_WEIGHT_MAX == 9,
                    "WEIGHT_MAX must be 9 — KERNEL layer (most)");
CPISI_STATIC_ASSERT(CPISI_HEALTH_WEIGHT_TOTAL == 45,
                    "WEIGHT_TOTAL must be 45 — sum(1..9)");

/* RELATIONSHIP tripwires — verify derivations */
CPISI_STATIC_ASSERT(CPISI_HEALTH_WEIGHT_MIN == CPISI_UNITY,
                    "WEIGHT_MIN must equal UNITY — verifies 1 = 1");
CPISI_STATIC_ASSERT(CPISI_HEALTH_WEIGHT_MAX == CPISI_FACE,
                    "WEIGHT_MAX must equal FACE — verifies 9 = 9");
CPISI_STATIC_ASSERT(CPISI_HEALTH_WEIGHT_CENTER == CPISI_QUOTIENT(CPISI_SUM(CPISI_FACE, CPISI_UNITY), CPISI_BINARY),
                    "WEIGHT_CENTER must equal (FACE+1)/2 — verifies 5 = 10/2");
CPISI_STATIC_ASSERT(CPISI_HEALTH_WEIGHT_TOTAL == CPISI_QUOTIENT(CPISI_PRODUCT(CPISI_FACE, CPISI_SUM(CPISI_FACE, CPISI_UNITY)), CPISI_BINARY),
                    "WEIGHT_TOTAL must equal FACE×(FACE+1)/2 — verifies 45 = 9×10/2");


/** @} */ /* end S.2c.3e WEIGHT */

/**
 * @defgroup S2c_3f_HALF S.2c.3f HALF TRIPWIRES — Verify half values
 * @ingroup S2c_3_HEALTH
 * @brief Verify HALF values at layer power boundaries.
 *
 * HALF VALUES BY LAYER:
 *   HALF(power) = (power-1)/2 for index-to-balanced conversion.
 *
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  LAYER     POWER     HALF       DERIVATION                          │
 *   │  ───────   ───────   ────────   ──────────────────────────────      │
 *   │  L0        1 (3⁰)    0          (1-1)/2 = 0                         │
 *   │  L4        81 (3⁴)   40         (81-1)/2 = 40 ← HALF_CENTER         │
 *   │  L8        6561(3⁸)  3280       (6561-1)/2 = 3280 ← HALF_MAX        │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * WHY THIS MATTERS:
 *   HALF is used for index↔balanced conversion.
 *   If HALF(TESSERACT) ≠ 40, layer conversions at HEALTH would fail.
 * @{ */

/* VALUE tripwires — verify literals */
CPISI_STATIC_ASSERT(CPISI_HALF_MIN == 0,
                    "HALF_MIN must be 0 — KERNEL layer");
CPISI_STATIC_ASSERT(CPISI_HALF_CENTER == 40,
                    "HALF_CENTER must be 40 — HEALTH layer");
CPISI_STATIC_ASSERT(CPISI_HALF_MAX == 3280,
                    "HALF_MAX must be 3280 — APP layer");

/* RELATIONSHIP tripwires — verify derivations */
CPISI_STATIC_ASSERT(CPISI_HALF_MIN == CPISI_VOID,
                    "HALF_MIN must equal VOID — verifies 0 = 0");
CPISI_STATIC_ASSERT(CPISI_HALF_CENTER == CPISI_HALF(CPISI_TESSERACT),
                    "HALF_CENTER must equal HALF(81) — verifies 40 = (81-1)/2");
CPISI_STATIC_ASSERT(CPISI_HALF_MAX == CPISI_HALF(CPISI_POWER_L8),
                    "HALF_MAX must equal HALF(6561) — verifies 3280 = (6561-1)/2");


/** @} */ /* end S.2c.3f HALF */

/**
 * @defgroup S2c_3g_ZONE S.2c.3g ZONE TRIPWIRES — Verify zone relationships
 * @ingroup S2c_3_HEALTH
 * @brief Verify 5→7→9 progression and zone boundary derivations.
 *
 * ZONE PROGRESSION:
 *   5 zones → 7 Hebrew states → 9 measurement layers
 *   (HARDPOINTS → STATES → FACE)
 *
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  CONSTANT              VALUE    DERIVATION                          │
 *   │  ────────────────────  ─────    ──────────────────────────────      │
 *   │  ZONE_COUNT            5        HARDPOINTS                          │
 *   │  STATE_COUNT           7        ZONES + 2 (k-splits)                │
 *   │  LAYER_COUNT_HEALTH    9        STATES + 2 (edge-splits)            │
 *   │  ZONE_BOUNDARY_COUNT   4        HARDPOINTS - 1                      │
 *   │  ZONE_INNER            25       MILESTONE_STEP × 1                  │
 *   │  ZONE_OUTER            50       MILESTONE_STEP × 2                  │
 *   │  ZONE_EDGE             100      MILESTONE_STEP × 4                  │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * WHY THIS MATTERS:
 *   If ZONE_COUNT ≠ HARDPOINTS, zone-to-state mapping breaks.
 *   If zone distances don't scale from MILESTONE_STEP, boundaries drift.
 * @{ */

/* VALUE tripwires — verify literals */
CPISI_STATIC_ASSERT(CPISI_ZONE_COUNT == 5,
                    "ZONE_COUNT must be 5 — zones");
CPISI_STATIC_ASSERT(CPISI_STATE_COUNT == 7,
                    "STATE_COUNT must be 7 — Hebrew states");
CPISI_STATIC_ASSERT(CPISI_LAYER_COUNT_HEALTH == 9,
                    "LAYER_COUNT_HEALTH must be 9 — measurement layers");
CPISI_STATIC_ASSERT(CPISI_ZONE_BOUNDARY_COUNT == 4,
                    "ZONE_BOUNDARY_COUNT must be 4 — boundaries");
CPISI_STATIC_ASSERT(CPISI_ZONE_INNER == 25,
                    "ZONE_INNER must be 25 — inner threshold");
CPISI_STATIC_ASSERT(CPISI_ZONE_OUTER == 50,
                    "ZONE_OUTER must be 50 — outer threshold");
CPISI_STATIC_ASSERT(CPISI_ZONE_EDGE == 100,
                    "ZONE_EDGE must be 100 — edge distance");

/* RELATIONSHIP tripwires — verify derivations */
CPISI_STATIC_ASSERT(CPISI_ZONE_COUNT == CPISI_HARDPOINTS,
                    "ZONE_COUNT must equal HARDPOINTS — verifies 5 = 5");
CPISI_STATIC_ASSERT(CPISI_STATE_COUNT == CPISI_STATES,
                    "STATE_COUNT must equal STATES — verifies 7 = 7");
CPISI_STATIC_ASSERT(CPISI_LAYER_COUNT_HEALTH == CPISI_FACE,
                    "LAYER_COUNT_HEALTH must equal FACE — verifies 9 = 9");
CPISI_STATIC_ASSERT(CPISI_ZONE_BOUNDARY_COUNT == CPISI_DIFF(CPISI_HARDPOINTS, CPISI_UNITY),
                    "ZONE_BOUNDARY_COUNT must equal HARDPOINTS-1 — verifies 4 = 5-1");
CPISI_STATIC_ASSERT(CPISI_ZONE_INNER == CPISI_MILESTONE_STEP,
                    "ZONE_INNER must equal MILESTONE_STEP — verifies 25 = 25");
CPISI_STATIC_ASSERT(CPISI_ZONE_OUTER == CPISI_PRODUCT(CPISI_MILESTONE_STEP, CPISI_HARDPOINTS_CENTER),
                    "ZONE_OUTER must equal STEP×CENTER — verifies 50 = 25×2");
CPISI_STATIC_ASSERT(CPISI_ZONE_EDGE == CPISI_PRODUCT(CPISI_MILESTONE_STEP, CPISI_FACE_CENTER),
                    "ZONE_EDGE must equal STEP×FACE_CENTER — verifies 100 = 25×4");

/* PROGRESSION tripwires — verify 5→7→9 expansion */
CPISI_STATIC_ASSERT(CPISI_STATE_COUNT == CPISI_SUM(CPISI_ZONE_COUNT, CPISI_BINARY),
                    "STATE_COUNT must equal ZONE_COUNT+2 — verifies 7 = 5+2");
CPISI_STATIC_ASSERT(CPISI_LAYER_COUNT_HEALTH == CPISI_SUM(CPISI_STATE_COUNT, CPISI_BINARY),
                    "LAYER_COUNT_HEALTH must equal STATE_COUNT+2 — verifies 9 = 7+2");

/** @} */ /* end S.2c.3g ZONE */

/**
 * @defgroup S2c_3h_IMPACT S.2c.3h IMPACT TRIPWIRES — Verify impact/severity values
 * @ingroup S2c_3_HEALTH
 * @brief Verify 9 severity levels, impact conversion, and edge alignment.
 *
 * IMPACT/SEVERITY SYSTEM:
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  SEVERITY  IMPACT    MEANING                                        │
 *   │  ────────  ────────  ────────────────────────────────────           │
 *   │  -4        -100      FATAL (crisis edge)                            │
 *   │  -3        -75       HIGH (severe)                                  │
 *   │  -2        -50       MEDIUM (moderate)                              │
 *   │  -1        -25       LOW (minor)                                    │
 *   │   0          0       NONE (center/yashar)                           │
 *   │  +1        +25       FAIR (slight positive)                         │
 *   │  +2        +50       GOOD (moderate positive)                       │
 *   │  +3        +75       EXCELLENT (strong positive)                    │
 *   │  +4       +100       PERFECT (peak/tov)                             │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * DERIVATIONS:
 *   - SEVERITY_COUNT = FACE = 9 (nine severity levels)
 *   - SEVERITY_STEP = MILESTONE_STEP = 25 (shared step)
 *   - IMPACT_* = MILESTONE_* (shared edge values)
 * @{ */

/* VALUE tripwires — verify literals */
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_MIN == -4,
                    "SEVERITY_MIN must be -4 — FATAL edge");
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_CENTER == 0,
                    "SEVERITY_CENTER must be 0 — NONE (yashar)");
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_MAX == 4,
                    "SEVERITY_MAX must be +4 — PERFECT edge");
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_COUNT == 9,
                    "SEVERITY_COUNT must be 9 — 4+1+4 pattern");
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_STEP == 25,
                    "SEVERITY_STEP must be 25 — display units per severity");
CPISI_STATIC_ASSERT(CPISI_IMPACT_FATAL == -100,
                    "IMPACT_FATAL must be -100 — shavar edge");
CPISI_STATIC_ASSERT(CPISI_IMPACT_NONE == 0,
                    "IMPACT_NONE must be 0 — yashar center");
CPISI_STATIC_ASSERT(CPISI_IMPACT_PERFECT == 100,
                    "IMPACT_PERFECT must be +100 — tov edge");

/* RELATIONSHIP tripwires — verify derivations */
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_MIN == CPISI_NEGATE(CPISI_FACE_CENTER),
                    "SEVERITY_MIN must equal -FACE_CENTER — verifies -4 = -4");
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_CENTER == CPISI_VOID,
                    "SEVERITY_CENTER must equal VOID — verifies 0 = 0");
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_MAX == CPISI_FACE_CENTER,
                    "SEVERITY_MAX must equal FACE_CENTER — verifies +4 = +4");
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_COUNT == CPISI_FACE,
                    "SEVERITY_COUNT must equal FACE — verifies 9 = 9");
CPISI_STATIC_ASSERT(CPISI_HEALTH_SEVERITY_STEP == CPISI_MILESTONE_STEP,
                    "SEVERITY_STEP must equal MILESTONE_STEP — verifies 25 = 25");
CPISI_STATIC_ASSERT(CPISI_IMPACT_FATAL == CPISI_MILESTONE_NEG_4,
                    "IMPACT_FATAL must equal MILESTONE_NEG_4 — verifies -100 = -100");
CPISI_STATIC_ASSERT(CPISI_IMPACT_NONE == CPISI_MILESTONE_NEUTRAL,
                    "IMPACT_NONE must equal MILESTONE_NEUTRAL — verifies 0 = 0");
CPISI_STATIC_ASSERT(CPISI_IMPACT_PERFECT == CPISI_MILESTONE_POS_4,
                    "IMPACT_PERFECT must equal MILESTONE_POS_4 — verifies +100 = +100");

/* CONVERSION tripwires — verify severity × step = impact */
CPISI_STATIC_ASSERT(CPISI_PRODUCT(CPISI_HEALTH_SEVERITY_MAX, CPISI_HEALTH_SEVERITY_STEP) == 100,
                    "SEVERITY_MAX × SEVERITY_STEP must be 100 — verifies 4×25=100");
CPISI_STATIC_ASSERT(CPISI_PRODUCT(CPISI_HEALTH_SEVERITY_MIN, CPISI_HEALTH_SEVERITY_STEP) == -100,
                    "SEVERITY_MIN × SEVERITY_STEP must be -100 — verifies -4×25=-100");
CPISI_STATIC_ASSERT(CPISI_PRODUCT(CPISI_HEALTH_SEVERITY_MAX, CPISI_HEALTH_SEVERITY_STEP) == CPISI_IMPACT_PERFECT,
                    "SEVERITY_MAX × SEVERITY_STEP must equal IMPACT_PERFECT");
CPISI_STATIC_ASSERT(CPISI_PRODUCT(CPISI_HEALTH_SEVERITY_MIN, CPISI_HEALTH_SEVERITY_STEP) == CPISI_IMPACT_FATAL,
                    "SEVERITY_MIN × SEVERITY_STEP must equal IMPACT_FATAL");


/** @} */ /* end S.2c.3h IMPACT */

/**
 * @defgroup S2c_3i_NORMALIZATION S.2c.3i NORMALIZATION TRIPWIRES — Verify normalization values
 * @ingroup S2c_3_HEALTH
 * @brief Verify true score, k-factor, positions, and asymmetry.
 *
 * NORMALIZATION SYSTEM VERIFIED:
 *   TRUE_SCORE: -128..+127 (balanced layer range)
 *   K_FACTOR:   -1, 0, +1 (direction values)
 *   POSITIONS:  128, 1, 127 (step counts per direction)
 *   ASYMMETRY:  128-127=1 (one more step to corruption)
 *
 * Structure:
 *   S.2c.3i.1 TRUE_SCORE — Boundary verification
 *   S.2c.3i.2 K_FACTOR   — Direction value verification
 *   S.2c.3i.3 POSITIONS  — Position count verification
 *   S.2c.3i.4 ASYMMETRY  — Asymmetry verification
 *   S.2c.3i.5 HALVES     — Midpoint verification
 * @{ */

/* ── S.2c.3i.1 TRUE_SCORE — Boundary verification ───────────────────────────── */

/* VALUE TRIPWIRES — Verify true score bounds match expected literals */
CPISI_STATIC_ASSERT(CPISI_TRUE_MIN == -128,
                    "TRUE_MIN must be -128 — shavar edge");
CPISI_STATIC_ASSERT(CPISI_TRUE_CENTER == 0,
                    "TRUE_CENTER must be 0 — yashar anchor");
CPISI_STATIC_ASSERT(CPISI_TRUE_MAX == 127,
                    "TRUE_MAX must be +127 — tov edge");

/* RELATIONSHIP TRIPWIRES — Verify true score bounds derive from S.2a.5b.2a PIPELINE */
CPISI_STATIC_ASSERT(CPISI_TRUE_MIN == CPISI_NEGATE(CPISI_HEALTH_EDGE_NEG),
                    "TRUE_MIN must equal -HEALTH_EDGE_NEG — verifies -128 = -128");
CPISI_STATIC_ASSERT(CPISI_TRUE_CENTER == CPISI_VOID,
                    "TRUE_CENTER must equal VOID — verifies 0 = 0");
CPISI_STATIC_ASSERT(CPISI_TRUE_MAX == CPISI_HEALTH_EDGE_POS,
                    "TRUE_MAX must equal HEALTH_EDGE_POS — verifies 127 = 127");

/* ── S.2c.3i.2 K_FACTOR — Direction value verification ──────────────────────── */

/* VALUE TRIPWIRES — Verify k-factor match expected literals */
CPISI_STATIC_ASSERT(CPISI_K_MIN == -1,
                    "K_MIN must be -1 — toward shavar");
CPISI_STATIC_ASSERT(CPISI_K_CENTER == 0,
                    "K_CENTER must be 0 — maintain/neutral");
CPISI_STATIC_ASSERT(CPISI_K_MAX == 1,
                    "K_MAX must be +1 — toward tov");
CPISI_STATIC_ASSERT(CPISI_K_COUNT == 3,
                    "K_COUNT must be 3 — ternary values");

/* RELATIONSHIP TRIPWIRES — Verify k-factor derive from S.2a.1b SIGNS */
CPISI_STATIC_ASSERT(CPISI_K_MIN == CPISI_NEGATE(CPISI_UNITY),
                    "K_MIN must equal -UNITY — verifies -1 = -1");
CPISI_STATIC_ASSERT(CPISI_K_CENTER == CPISI_VOID,
                    "K_CENTER must equal VOID — verifies 0 = 0");
CPISI_STATIC_ASSERT(CPISI_K_MAX == CPISI_UNITY,
                    "K_MAX must equal UNITY — verifies +1 = +1");
CPISI_STATIC_ASSERT(CPISI_K_COUNT == CPISI_TERNARY,
                    "K_COUNT must equal TERNARY — verifies 3 = 3");

/* ── S.2c.3i.3 POSITIONS — Position count verification ──────────────────────── */

/* VALUE TRIPWIRES — Verify position counts match expected literals */
CPISI_STATIC_ASSERT(CPISI_POSITIONS_NEG == 128,
                    "POSITIONS_NEG must be 128 — corruption steps");
CPISI_STATIC_ASSERT(CPISI_POSITIONS_CENTER == 1,
                    "POSITIONS_CENTER must be 1 — yashar anchor");
CPISI_STATIC_ASSERT(CPISI_POSITIONS_POS == 127,
                    "POSITIONS_POS must be 127 — restoration steps");
CPISI_STATIC_ASSERT(CPISI_POSITIONS_TOTAL == 256,
                    "POSITIONS_TOTAL must be 256 — all positions");

/* RELATIONSHIP TRIPWIRES — Verify position relationships */
CPISI_STATIC_ASSERT(CPISI_SUM(CPISI_POSITIONS_NEG, CPISI_SUM(CPISI_POSITIONS_CENTER, CPISI_POSITIONS_POS))
                    == CPISI_POSITIONS_TOTAL,
                    "NEG + CENTER + POS must equal TOTAL — verifies 128+1+127=256");
CPISI_STATIC_ASSERT(CPISI_POSITIONS_NEG == CPISI_SUM(CPISI_POSITIONS_POS, CPISI_UNITY),
                    "POSITIONS_NEG must equal POS + 1 — verifies 128 = 127+1");

/* ── S.2c.3i.4 ASYMMETRY — Asymmetry verification ───────────────────────────── */

/* VALUE TRIPWIRES — Verify asymmetry matches expected literal */
CPISI_STATIC_ASSERT(CPISI_ASYMMETRY_DIFF == 1,
                    "ASYMMETRY_DIFF must be 1 — one more step to corruption");

/* RELATIONSHIP TRIPWIRES — Verify asymmetry derives from UNITY */
CPISI_STATIC_ASSERT(CPISI_ASYMMETRY_DIFF == CPISI_UNITY,
                    "ASYMMETRY_DIFF must equal UNITY — verifies 1 = 1");
CPISI_STATIC_ASSERT(CPISI_ASYMMETRY_DIFF_COMPUTED == CPISI_ASYMMETRY_DIFF,
                    "Computed asymmetry must equal bound — verifies 128-127 = 1");

/* ── S.2c.3i.5 HALVES — Midpoint verification ───────────────────────────────── */

/* VALUE TRIPWIRES — Verify halves match expected literals */
CPISI_STATIC_ASSERT(CPISI_TRUE_HALF_NEG == 63,
                    "TRUE_HALF_NEG must be 63 — midpoint to shavar");
CPISI_STATIC_ASSERT(CPISI_TRUE_HALF_POS == 63,
                    "TRUE_HALF_POS must be 63 — midpoint to tov");

/* RELATIONSHIP TRIPWIRES — Verify halves derive from HALF formula */
CPISI_STATIC_ASSERT(CPISI_TRUE_HALF_NEG == CPISI_HALF(CPISI_TRUE_RANGE_NEG),
                    "TRUE_HALF_NEG must equal HALF(RANGE_NEG) — verifies 63 = (128-1)/2");
CPISI_STATIC_ASSERT(CPISI_TRUE_HALF_POS == CPISI_HALF(CPISI_TRUE_RANGE_POS),
                    "TRUE_HALF_POS must equal HALF(RANGE_POS) — verifies 63 = (127-1)/2");

/** @} */ /* end S.2c.3i NORMALIZATION */
/** @} */ /* end S.2c.3 HEALTH */

/** @} */ /* end S.2c VERIFY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d LOOKUP — Health domain runtime tables [LOOKUP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_lookup S.2d HEALTH LOOKUP — Runtime Tables
 * @ingroup cpisi_health_domain
 * @brief Health domain runtime lookup tables — state, severity, trend, milestone, weight, half, zone.
 *
 * PURPOSE: Container for health domain lookup tables.
 *
 * Builds FROM: S.2c VERIFY (structure verified)
 * Builds TO:   S.2e TYPES (enum definitions)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2d_9_HEALTH S.2d.9 HEALTH — Health domain runtime tables
 * @ingroup cpisi_health_lookup
 * @brief Runtime tables for health monitoring system.
 *
 * THE 7 HEBREW HEALTH STATES:
 *
 *   Health states use Hebrew terms reflecting spiritual-cognitive parallels:
 *
 *   ┌─────────────────────────────────────────────────────────────────────────┐
 *   │  INDEX   HEBREW    ENGLISH    VALUE     MEANING                         │
 *   │  ──────  ────────  ─────────  ────────  ──────────────────────────────  │
 *   │  [0]     shavar    BROKEN     -1.0      Crisis state (edge)             │
 *   │  [1]     chaser    LACKING    -0.5      Declining (k=-1)                │
 *   │  [2]     ratsah    WANTING    -0.5      Hungry for growth (k=+1)        │
 *   │  [3]     yashar    EVEN       0.0       CENTER — default state          │
 *   │  [4]     tamim     SOUND      +0.5      Consolidating (k=-1)            │
 *   │  [5]     shalem    WHOLE      +0.5      Expanding (k=+1)                │
 *   │  [6]     tov       PERFECT    +1.0      Completion state (edge)         │
 *   └─────────────────────────────────────────────────────────────────────────┘
 *
 * THE 9-MILESTONE EXTENSION:
 *
 *   For finer granularity, milestones extend to 9 positions (-4..+4):
 *   shavar(-4) → machsor(-3) → chaser(-2) → ratsah(-1) → yashar(0)
 *   → ratson(+1) → shalem(+2) → tamim(+3) → tov(+4)
 *
 * STRUCTURE:
 *   S.2d.9a TABLES       — All runtime lookup arrays (9 table groups)
 *   S.2d.9b ACCESSORS    — Macro wrappers for array access (13 accessors)
 *   S.2d.9c CONVERSION   — [Reserved] Format conversion utilities
 *
 * TABLE GROUPS (internal to S.2d.9a):
 *   STATE        — 7 Hebrew state names
 *   SEVERITY     — 5 severity level names
 *   TREND        — 3 trend direction names
 *   MILESTONE    — 6 tables × 9 entries
 *   WEIGHT       — 4 tables × 9 entries
 *   HALF         — 4 tables × 9 entries
 *   ZONE         — 5 tables
 *   IMPACT       — 6 tables × 9 entries
 *   NORMALIZATION— K-factor, positions, divisor, true_bounds, true_half
 *
 * @{
 */

/**
 * @defgroup S2d_9a_TABLES S.2d.9a TABLES — Health domain lookup arrays
 * @ingroup S2d_9_HEALTH
 * @brief All runtime lookup arrays organized by functional group.
 *
 * TABLE GROUPS:
 *   STATE        — CPISI_HEALTH_NAMES[7]
 *   SEVERITY     — CPISI_SEVERITY_NAMES[5]
 *   TREND        — CPISI_TREND_NAMES[3]
 *   MILESTONE    — 6 arrays (display, balanced, ternary, center, name, abbrev)
 *   WEIGHT       — 4 arrays (layer weights)
 *   HALF         — 4 arrays (layer halves)
 *   ZONE         — 5 arrays (boundary, boundary_ternary, name, abbrev, center)
 *   IMPACT       — 6 arrays (display, ternary, severity, name, abbrev, center)
 *   NORMALIZATION— k-factor, positions, divisor, true_bounds, true_half arrays
 *
 * @{
 */

/** ── STATE — 7 Hebrew health state names ──────────────────────────────────────
 * @name STATE
 * @{ */


/**
 * @brief Health state names — maps state index (0-6) to Hebrew name string.
 *
 * BOUNDS USED:
 *   - CPISI_HEALTH_COUNT = 7 (from S.2a.3 TIER 3) — array size
 *
 * 7-STATE HEALTH SYSTEM (Hebrew terms):
 *   [0] "shavar" — BROKEN, crisis state (-1.0)
 *   [1] "chaser" — LACKING, declining (k=-1)
 *   [2] "ratsah" — WANTING, hungry for growth (k=+1)
 *   [3] "yashar" — EVEN, CENTER/default state (0.0)
 *   [4] "tamim"  — SOUND, consolidating (k=-1)
 *   [5] "shalem" — WHOLE, expanding (k=+1)
 *   [6] "tov"    — PERFECT, completion state (+1.0)
 *
 * CENTER: yashar (index 3) = HALF(7) = (7-1)/2 = 3
 *
 * USE CASE: Display health state in logs, UI, diagnostics.
 */
static const char* const CPISI_HEALTH_NAMES[CPISI_HEALTH_COUNT] = {
    "shavar",  /* [0] BROKEN  -1.0  */
    "chaser",  /* [1] LACKING -0.5  */
    "ratsah",  /* [2] WANTING -0.5  */
    "yashar",  /* [3] EVEN     0.0  */
    "tamim",   /* [4] SOUND   +0.5  */
    "shalem",  /* [5] WHOLE   +0.5  */
    "tov"      /* [6] PERFECT +1.0  */
};

/** @} */ /* End STATE */

/** ── SEVERITY — 5 severity level names ────────────────────────────────────────
 * @name SEVERITY
 * @{ */

/**
 * @brief Severity names — maps severity index (0-4) to classification string.
 *
 * BOUNDS USED:
 *   - CPISI_SEVERITY_COUNT = 5 (from S.2a.3 TIER 3) — array size
 *
 * 5-LEVEL SEVERITY (escalating urgency):
 *   [0] "nominal"  — Normal operation, no issues
 *   [1] "advisory" — Informational notice, awareness only
 *   [2] "caution"  — Minor concern, monitor (CENTER of 5)
 *   [3] "warning"  — Significant issue, attention needed
 *   [4] "critical" — Immediate action required
 *
 * USE CASE: Classify health issues by urgency for prioritization.
 */
static const char* const CPISI_SEVERITY_NAMES[CPISI_SEVERITY_COUNT] = {
    "nominal",   /* [0] normal    */
    "advisory",  /* [1] notice    */
    "caution",   /* [2] monitor   */
    "warning",   /* [3] attention */
    "critical"   /* [4] immediate */
};

/** @} */ /* End SEVERITY */

/** ── TREND — 3 trend direction names ──────────────────────────────────────────
 * @name TREND
 * @{ */

/**
 * @brief Trend names — maps trend index (0-2) to direction string.
 *
 * BOUNDS USED:
 *   - CPISI_TREND_COUNT = 3 (from S.2a.3 TIER 3) — array size
 *
 * 3-WAY TREND (ternary pattern, matches sign):
 *   [0] "declining"  — Getting worse (sign = -1)
 *   [1] "stable"     — Holding steady (sign = 0, CENTER)
 *   [2] "improving"  — Getting better (sign = +1)
 *
 * PARALLEL TO: CPISI_SIGN_VALUES — same structure, health domain.
 *
 * USE CASE: Indicate health trajectory over time.
 */
static const char* const CPISI_TREND_NAMES[CPISI_TREND_COUNT] = {
    "declining",  /* [0] worse  = -1 */
    "stable",     /* [1] steady =  0 */
    "improving"   /* [2] better = +1 */
};


/** @} */ /* End TREND */

/** ── MILESTONE — 6 tables × 9 entries ────────────────────────────────────────
 * @name MILESTONE
 *
 * THE 9 MILESTONES (-4 to +4):
 *   [0] shavar  (-4)  BROKEN    — crisis edge
 *   [1] machsor (-3)  DEPRIVED  — severe lack
 *   [2] chaser  (-2)  LACKING   — declining
 *   [3] ratsah  (-1)  WANTING   — hungry for growth
 *   [4] yashar  (0)   EVEN      — CENTER/default
 *   [5] ratson  (+1)  WILLING   — receiving
 *   [6] shalem  (+2)  WHOLE     — expanding
 *   [7] tamim   (+3)  SOUND     — consolidating
 *   [8] tov     (+4)  PERFECT   — completion edge
 *
 * TABLES PROVIDED:
 *   - g_cpisi_milestone_display[9]        — int16_t: -100..+100 (display percentage)
 *   - g_cpisi_milestone_balanced[9]       — int8_t: -4..+4 (balanced form)
 *   - g_cpisi_milestone_ternary[9]        — float: -1.0..+1.0 (ternary form)
 *   - g_cpisi_milestone_display_center[9] — int16_t: zone centers
 *   - g_cpisi_milestone_name[9]           — char*: Hebrew names
 *   - g_cpisi_milestone_abbrev[9]         — char: single-char abbreviations
 *
 * ACCESSORS: cpisi_milestone_display(), _balanced(), _ternary(), _display_center(), _name(), _abbrev()
 * @{ */

/**
 * @brief Display value lookup — maps milestone index (0-8) to display percentage (-100..+100).
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size (9 milestones)
 *   - Display range: -100 to +100 (percentile scale)
 *
 * DERIVATION:
 *   Index 0 (shavar)  → -100 (minimum, crisis)
 *   Index 4 (yashar)  →    0 (CENTER, balanced)
 *   Index 8 (tov)     → +100 (maximum, completion)
 *   Formula: value = (index - 4) × 25
 *
 * USE CASE: Convert internal milestone index to human-readable percentage for UI/logging.
 */
static const int16_t g_cpisi_milestone_display[CPISI_FACE] = {
    -100, /* [0] shavar  = (0-4)×25 = -100  */
    -75,  /* [1] machsor = (1-4)×25 = -75   */
    -50,  /* [2] chaser  = (2-4)×25 = -50   */
    -25,  /* [3] ratsah  = (3-4)×25 = -25   */
    0,    /* [4] yashar  = (4-4)×25 = 0     */
    +25,  /* [5] ratson  = (5-4)×25 = +25   */
    +50,  /* [6] shalem  = (6-4)×25 = +50   */
    +75,  /* [7] tamim   = (7-4)×25 = +75   */
    +100  /* [8] tov     = (8-4)×25 = +100  */
};

/**
 * @brief Balanced value lookup — maps milestone index (0-8) to balanced form (-4..+4).
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - CPISI_MILESTONE_HALF = 4 (from S.2a.2 TIER 2) — offset from center
 *
 * DERIVATION:
 *   Formula: balanced = index - CPISI_MILESTONE_HALF = index - 4
 *   Range: [0-4, 8-4] = [-4, +4]
 *   Center: yashar at index 4 → balanced 0
 *
 * RELATIONSHIP: Parallel to g_cpisi_milestone_display (scaled ×25).
 *
 * USE CASE: Compact signed representation for state machine arithmetic.
 */
static const int8_t g_cpisi_milestone_balanced[CPISI_FACE] = {
    -4, /* [0] shavar  = 0-4  */
    -3, /* [1] machsor = 1-4  */
    -2, /* [2] chaser  = 2-4  */
    -1, /* [3] ratsah  = 3-4  */
    0,  /* [4] yashar  = 4-4  */
    +1, /* [5] ratson  = 5-4  */
    +2, /* [6] shalem  = 6-4  */
    +3, /* [7] tamim   = 7-4  */
    +4  /* [8] tov     = 8-4  */
};

/**
 * @brief Ternary value lookup — maps milestone index (0-8) to normalized float (-1.0..+1.0).
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - CPISI_MILESTONE_HALF = 4 (from S.2a.2 TIER 2) — divisor
 *
 * DERIVATION:
 *   Formula: ternary = (index - 4) / 4.0 = balanced / MILESTONE_HALF
 *   Range: [-4/4, +4/4] = [-1.0, +1.0]
 *   Step: 0.25 per milestone (1/4)
 *
 * RELATIONSHIP: g_cpisi_milestone_balanced / 4.0 (normalized to [-1,+1]).
 *
 * USE CASE: Floating-point operations, interpolation, continuous state representation.
 */
static const float g_cpisi_milestone_ternary[CPISI_FACE] = {
    -1.0f,  /* [0] shavar  = -4/4  */
    -0.75f, /* [1] machsor = -3/4  */
    -0.5f,  /* [2] chaser  = -2/4  */
    -0.25f, /* [3] ratsah  = -1/4  */
    0.0f,   /* [4] yashar  =  0/4  */
    +0.25f, /* [5] ratson  = +1/4  */
    +0.5f,  /* [6] shalem  = +2/4  */
    +0.75f, /* [7] tamim   = +3/4  */
    +1.0f   /* [8] tov     = +4/4  */
};

/**
 * @brief Zone center lookup — maps milestone index (0-8) to zone midpoint display value.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - Display range: -100 to +100
 *   - Zone width: 25 units per milestone zone
 *
 * DERIVATION:
 *   Center of each zone = boundary - half_zone_width = (index-4)×25 - 12.5
 *   Except yashar at exact 0, edges at ±88 (center of 12-unit edge zones)
 *   Pattern: -88, -63, -38, -13, 0, +13, +38, +63, +88
 *
 * USE CASE: Snap-to-center behavior, zone visualization, midpoint calculations.
 */
static const int16_t g_cpisi_milestone_display_center[CPISI_FACE] = {
    -88, /* [0] shavar  zone center  */
    -63, /* [1] machsor zone center  */
    -38, /* [2] chaser  zone center  */
    -13, /* [3] ratsah  zone center  */
    0,   /* [4] yashar  at exact 0   */
    +13, /* [5] ratson  zone center  */
    +38, /* [6] shalem  zone center  */
    +63, /* [7] tamim   zone center  */
    +88  /* [8] tov     zone center  */
};

/**
 * @brief Hebrew state name lookup — maps milestone index (0-8) to Hebrew name string.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *
 * DERIVATION: 9 Hebrew cognitive states from biblical vocabulary:
 *   Negative: shavar (שָׁבַר broken), machsor (מַחְסוֹר deprived), chaser (חָסֵר lacking), ratsah (רָצָה wanting)
 *   Center:   yashar (יָשָׁר even/upright)
 *   Positive: ratson (רָצוֹן willing), shalem (שָׁלֵם whole), tamim (תָּמִים sound), tov (טוֹב good/perfect)
 *
 * RELATIONSHIP: Parallel to CPISI_HEALTH_NAMES[7] (7-state canonical vs 9-state milestone).
 *
 * USE CASE: Logging, debugging, UI display of cognitive/health state names.
 */
static const char* const g_cpisi_milestone_name[CPISI_FACE] = {
    "shavar",  /* [0] שָׁבַר BROKEN   */
    "machsor", /* [1] מַחְסוֹר DEPRIVED */
    "chaser",  /* [2] חָסֵר LACKING  */
    "ratsah",  /* [3] רָצָה WANTING  */
    "yashar",  /* [4] יָשָׁר EVEN     */
    "ratson",  /* [5] רָצוֹן WILLING  */
    "shalem",  /* [6] שָׁלֵם WHOLE    */
    "tamim",   /* [7] תָּמִים SOUND    */
    "tov"      /* [8] טוֹב PERFECT  */
};

/**
 * @brief Abbreviation lookup — maps milestone index (0-8) to single-character code.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *
 * DERIVATION: Single-char mnemonics, case distinguishes direction:
 *   UPPERCASE = negative/center: B(roken), M(issing), C(ut), R(eaching), Y(ashar)
 *   lowercase = positive: r(eceiving), s(ound), t(amim), v(ery good)
 *   Exception: S,T,V uppercase to avoid confusion with common single-char meanings
 *
 * USE CASE: Compact logging, status bars, abbreviated state display.
 */
static const char g_cpisi_milestone_abbrev[CPISI_FACE] = {
    'B', /* [0] shavar  — Broken    */
    'M', /* [1] machsor — Missing   */
    'C', /* [2] chaser  — Cut       */
    'R', /* [3] ratsah  — Reaching  */
    'Y', /* [4] yashar  — Yashar    */
    'r', /* [5] ratson  — receiving */
    'S', /* [6] shalem  — Sound     */
    'T', /* [7] tamim   — Tamim     */
    'V'  /* [8] tov     — Very good */
};

/**
 * @brief Get milestone display value — INDEX (0-8) → DISPLAY (-100..+100).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: int16_t ∈ [-100, +100]
 *
 * FORMULA: Direct table lookup — g_cpisi_milestone_display[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_milestone_display(0) → -100 (shavar)
 *   cpisi_milestone_display(4) →    0 (yashar)
 *   cpisi_milestone_display(8) → +100 (tov)
 *
 * @param idx Milestone index 0-8 (unchecked)
 * @return Display value: -100 to +100
 */
#define cpisi_milestone_display(idx)        (g_cpisi_milestone_display[(idx)])

/**
 * @brief Get milestone balanced value — INDEX (0-8) → BALANCED (-4..+4).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: int8_t ∈ [-4, +4]
 *
 * FORMULA: Direct table lookup — g_cpisi_milestone_balanced[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_milestone_balanced(0) → -4 (shavar)
 *   cpisi_milestone_balanced(4) →  0 (yashar)
 *   cpisi_milestone_balanced(8) → +4 (tov)
 *
 * @param idx Milestone index 0-8 (unchecked)
 * @return Balanced value: -4 to +4
 */
#define cpisi_milestone_balanced(idx)       (g_cpisi_milestone_balanced[(idx)])

/**
 * @brief Get milestone ternary value — INDEX (0-8) → TERNARY (-1.0..+1.0).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: float ∈ [-1.0, +1.0]
 *
 * FORMULA: Direct table lookup — g_cpisi_milestone_ternary[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_milestone_ternary(0) → -1.0f  (shavar)
 *   cpisi_milestone_ternary(4) →  0.0f  (yashar)
 *   cpisi_milestone_ternary(8) → +1.0f  (tov)
 *
 * @param idx Milestone index 0-8 (unchecked)
 * @return Ternary value: -1.0 to +1.0
 */
#define cpisi_milestone_ternary(idx)        (g_cpisi_milestone_ternary[(idx)])

/**
 * @brief Get milestone zone center — INDEX (0-8) → CENTER display value.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: int16_t ∈ {-88, -63, -38, -13, 0, +13, +38, +63, +88}
 *
 * FORMULA: Direct table lookup — g_cpisi_milestone_display_center[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_milestone_display_center(0) → -88 (shavar zone)
 *   cpisi_milestone_display_center(4) →   0 (yashar exact)
 *   cpisi_milestone_display_center(8) → +88 (tov zone)
 *
 * @param idx Milestone index 0-8 (unchecked)
 * @return Zone center value
 */
#define cpisi_milestone_display_center(idx) (g_cpisi_milestone_display_center[(idx)])

/**
 * @brief Get milestone Hebrew name — INDEX (0-8) → NAME string.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: const char* (null-terminated)
 *
 * FORMULA: Direct table lookup — g_cpisi_milestone_name[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_milestone_name(0) → "shavar"
 *   cpisi_milestone_name(4) → "yashar"
 *   cpisi_milestone_name(8) → "tov"
 *
 * @param idx Milestone index 0-8 (unchecked)
 * @return Hebrew state name string
 */
#define cpisi_milestone_name(idx)           (g_cpisi_milestone_name[(idx)])

/**
 * @brief Get milestone abbreviation — INDEX (0-8) → single char.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: char ∈ {B, M, C, R, Y, r, S, T, V}
 *
 * FORMULA: Direct table lookup — g_cpisi_milestone_abbrev[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_milestone_abbrev(0) → 'B' (Broken)
 *   cpisi_milestone_abbrev(4) → 'Y' (Yashar)
 *   cpisi_milestone_abbrev(8) → 'V' (Very good)
 *
 * @param idx Milestone index 0-8 (unchecked)
 * @return Single-character abbreviation
 */
#define cpisi_milestone_abbrev(idx)         (g_cpisi_milestone_abbrev[(idx)])

/** @} */ /* End MILESTONE */

/** ── WEIGHT — 4 tables × 9 entries ───────────────────────────────────────────
 * @name WEIGHT
 *
 * WEIGHT PATTERN (inverted from layer index):
 *   KERNEL(9) → SYSTEM(8) → RUNTIME(7) → FRAMEWORK(6) → HEALTH(5)
 *   → NETWORK(4) → STORAGE(3) → ENGINE(2) → APP(1)
 *
 * TABLES PROVIDED:
 *   - g_cpisi_layer_weight[9]          — uint8_t: 9..1 (criticality)
 *   - g_cpisi_layer_weight_balanced[9] — int8_t: +4..-4 (balanced form)
 *   - g_cpisi_layer_weight_ternary[9]  — float: +1.0..-1.0 (ternary form)
 *   - g_cpisi_layer_weight_center[9]   — uint8_t: constant 5 (CENTER reference)
 *
 * @{ */

/**
 * @brief Weight lookup — maps layer index (0-8) to criticality weight (9..1).
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size (9 layers)
 *   - Weight range: 9 (most critical) to 1 (least critical)
 *
 * DERIVATION:
 *   Formula: weight = CPISI_FACE - layer_index = 9 - index
 *   KERNEL(0) → 9 (most critical, failure = system death)
 *   HEALTH(4) → 5 (CENTER, monitoring layer)
 *   APP(8)    → 1 (least critical, user-space)
 *
 * RELATIONSHIP: Inverted layer index — lower layers = higher weight.
 *
 * USE CASE: Priority scheduling, error severity weighting, resource allocation.
 */
static const uint8_t g_cpisi_layer_weight[CPISI_FACE] = {
    9, /* [0] KERNEL    = 9-0 = 9  */
    8, /* [1] SYSTEM    = 9-1 = 8  */
    7, /* [2] RUNTIME   = 9-2 = 7  */
    6, /* [3] FRAMEWORK = 9-3 = 6  */
    5, /* [4] HEALTH    = 9-4 = 5  */
    4, /* [5] NETWORK   = 9-5 = 4  */
    3, /* [6] STORAGE   = 9-6 = 3  */
    2, /* [7] ENGINE    = 9-7 = 2  */
    1  /* [8] APP       = 9-8 = 1  */
};

/**
 * @brief Balanced weight lookup — maps layer index (0-8) to balanced form (+4..-4).
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - CENTER_WEIGHT = 5 (HEALTH layer weight)
 *
 * DERIVATION:
 *   Formula: balanced = weight - CENTER_WEIGHT = (9-index) - 5 = 4-index
 *   Range: [+4, -4] centered on HEALTH layer
 *   KERNEL(0) → +4, HEALTH(4) → 0, APP(8) → -4
 *
 * RELATIONSHIP: g_cpisi_layer_weight - 5 (offset to center at zero).
 *
 * USE CASE: Signed arithmetic for layer comparisons, deviation from center.
 */
static const int8_t g_cpisi_layer_weight_balanced[CPISI_FACE] = {
    +4, /* [0] KERNEL    = 9-5 = +4  */
    +3, /* [1] SYSTEM    = 8-5 = +3  */
    +2, /* [2] RUNTIME   = 7-5 = +2  */
    +1, /* [3] FRAMEWORK = 6-5 = +1  */
    0,  /* [4] HEALTH    = 5-5 = 0   */
    -1, /* [5] NETWORK   = 4-5 = -1  */
    -2, /* [6] STORAGE   = 3-5 = -2  */
    -3, /* [7] ENGINE    = 2-5 = -3  */
    -4  /* [8] APP       = 1-5 = -4  */
};

/**
 * @brief Ternary weight lookup — maps layer index (0-8) to normalized float (+1.0..-1.0).
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - CPISI_MILESTONE_HALF = 4 (from S.2a.2 TIER 2) — divisor
 *
 * DERIVATION:
 *   Formula: ternary = balanced / 4.0 = (4-index) / 4.0
 *   Range: [+1.0, -1.0], step = 0.25
 *   KERNEL(0) → +1.0, HEALTH(4) → 0.0, APP(8) → -1.0
 *
 * RELATIONSHIP: g_cpisi_layer_weight_balanced / 4.0 (normalized to [-1,+1]).
 *
 * USE CASE: Floating-point weighting, interpolation, continuous severity scaling.
 */
static const float g_cpisi_layer_weight_ternary[CPISI_FACE] = {
    +1.0f,  /* [0] KERNEL    = +4/4  */
    +0.75f, /* [1] SYSTEM    = +3/4  */
    +0.5f,  /* [2] RUNTIME   = +2/4  */
    +0.25f, /* [3] FRAMEWORK = +1/4  */
    0.0f,   /* [4] HEALTH    =  0/4  */
    -0.25f, /* [5] NETWORK   = -1/4  */
    -0.5f,  /* [6] STORAGE   = -2/4  */
    -0.75f, /* [7] ENGINE    = -3/4  */
    -1.0f   /* [8] APP       = -4/4  */
};

/**
 * @brief Center weight reference — constant 5 for all layers.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - CENTER_WEIGHT = 5 (HEALTH layer weight = 9-4)
 *
 * DERIVATION:
 *   All entries = 5 (HEALTH layer's weight, the pyramid CENTER)
 *   Purpose: Reference point for balanced calculations
 *
 * RELATIONSHIP: Constant used to compute g_cpisi_layer_weight_balanced.
 *
 * USE CASE: Baseline reference, deviation calculations, normalization base.
 */
static const uint8_t g_cpisi_layer_weight_center[CPISI_FACE] = {
    5, /* [0] KERNEL    ref = 5  */
    5, /* [1] SYSTEM    ref = 5  */
    5, /* [2] RUNTIME   ref = 5  */
    5, /* [3] FRAMEWORK ref = 5  */
    5, /* [4] HEALTH    ref = 5  */
    5, /* [5] NETWORK   ref = 5  */
    5, /* [6] STORAGE   ref = 5  */
    5, /* [7] ENGINE    ref = 5  */
    5  /* [8] APP       ref = 5  */
};

/**
 * @brief Get layer weight — INDEX (0-8) → WEIGHT (9..1).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: uint8_t ∈ [1, 9]
 *
 * FORMULA: Direct table lookup — g_cpisi_layer_weight[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_layer_weight(0) → 9 (KERNEL, most critical)
 *   cpisi_layer_weight(4) → 5 (HEALTH, center)
 *   cpisi_layer_weight(8) → 1 (APP, least critical)
 *
 * @param idx Layer index 0-8 (unchecked)
 * @return Weight value: 9 to 1
 */
#define cpisi_layer_weight(idx)          (g_cpisi_layer_weight[(idx)])

/**
 * @brief Get layer balanced weight — INDEX (0-8) → BALANCED (+4..-4).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: int8_t ∈ [-4, +4]
 *
 * FORMULA: Direct table lookup — g_cpisi_layer_weight_balanced[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_layer_weight_balanced(0) → +4 (KERNEL)
 *   cpisi_layer_weight_balanced(4) →  0 (HEALTH)
 *   cpisi_layer_weight_balanced(8) → -4 (APP)
 *
 * @param idx Layer index 0-8 (unchecked)
 * @return Balanced weight: +4 to -4
 */
#define cpisi_layer_weight_balanced(idx) (g_cpisi_layer_weight_balanced[(idx)])

/**
 * @brief Get layer ternary weight — INDEX (0-8) → TERNARY (+1.0..-1.0).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: float ∈ [-1.0, +1.0]
 *
 * FORMULA: Direct table lookup — g_cpisi_layer_weight_ternary[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_layer_weight_ternary(0) → +1.0f  (KERNEL)
 *   cpisi_layer_weight_ternary(4) →  0.0f  (HEALTH)
 *   cpisi_layer_weight_ternary(8) → -1.0f  (APP)
 *
 * @param idx Layer index 0-8 (unchecked)
 * @return Ternary weight: +1.0 to -1.0
 */
#define cpisi_layer_weight_ternary(idx)  (g_cpisi_layer_weight_ternary[(idx)])

/**
 * @brief Get layer center reference — INDEX (0-8) → CENTER (constant 5).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: uint8_t = 5 (constant)
 *
 * FORMULA: Direct table lookup — g_cpisi_layer_weight_center[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_layer_weight_center(0) → 5
 *   cpisi_layer_weight_center(4) → 5
 *   cpisi_layer_weight_center(8) → 5
 *
 * @param idx Layer index 0-8 (unchecked)
 * @return Center reference: always 5
 */
#define cpisi_layer_weight_center(idx)   (g_cpisi_layer_weight_center[(idx)])

/** @} */ /* End WEIGHT */

/** ── HALF — 4 tables × 9 entries ─────────────────────────────────────────────
 * @name HALF
 *
 * HALF PATTERN (centers of power ranges):
 *   KERNEL(0) → SYSTEM(1) → RUNTIME(4) → FRAMEWORK(13=BRIDGE)
 *   → HEALTH(40) → NETWORK(121) → STORAGE(364) → ENGINE(1093) → APP(3280)
 *
 * NOTE: FRAMEWORK half = 13 = BRIDGE. This is not coincidence.
 *
 * TABLES PROVIDED:
 *   - g_cpisi_layer_half[9]          — uint16_t: 0..3280 (half values)
 *   - g_cpisi_layer_half_balanced[9] — int16_t: relative to HEALTH(40)
 *   - g_cpisi_layer_half_ternary[9]  — float: 0.0..1.0 (normalized)
 *   - g_cpisi_layer_half_center[9]   — uint16_t: constant 40 (CENTER reference)
 *
 * @{ */

/**
 * @brief Half value lookup — maps layer index (0-8) to HALF(3^n) = (3^n - 1) / 2.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - CPISI_LAYER_POWERS[n] = 3^n (from S.2d.2) — base for half formula
 *
 * DERIVATION:
 *   Formula: half[n] = (3^n - 1) / 2 = (LAYER_POWER[n] - 1) / 2
 *   [0] KERNEL = (1-1)/2 = 0
 *   [1] SYSTEM = (3-1)/2 = 1
 *   [2] RUNTIME = (9-1)/2 = 4
 *   [3] FRAMEWORK = (27-1)/2 = 13 = BRIDGE (significant!)
 *   [4] HEALTH = (81-1)/2 = 40 (CENTER)
 *   [5] NETWORK = (243-1)/2 = 121
 *   [6] STORAGE = (729-1)/2 = 364
 *   [7] ENGINE = (2187-1)/2 = 1093
 *   [8] APP = (6561-1)/2 = 3280
 *
 * NOTE: FRAMEWORK half = 13 = CPISI_BRIDGE. This alignment is foundational.
 *
 * USE CASE: Range midpoints for ternary address space, centering calculations.
 */
static const uint16_t g_cpisi_layer_half[CPISI_FACE] = {
    0,    /* [0] KERNEL    = (1-1)/2 = 0          */
    1,    /* [1] SYSTEM    = (3-1)/2 = 1          */
    4,    /* [2] RUNTIME   = (9-1)/2 = 4          */
    13,   /* [3] FRAMEWORK = (27-1)/2 = BRIDGE!   */
    40,   /* [4] HEALTH    = (81-1)/2 = CENTER    */
    121,  /* [5] NETWORK   = (243-1)/2 = 121      */
    364,  /* [6] STORAGE   = (729-1)/2 = 364      */
    1093, /* [7] ENGINE    = (2187-1)/2 = 1093    */
    3280  /* [8] APP       = (6561-1)/2 = 3280    */
};

/**
 * @brief Balanced half lookup — maps layer index (0-8) to half relative to HEALTH(40).
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - CENTER_HALF = 40 (HEALTH layer half value)
 *
 * DERIVATION:
 *   Formula: balanced = g_cpisi_layer_half[n] - 40
 *   Range: [-40, +3240] — asymmetric due to exponential growth
 *   KERNEL(0) → 0-40 = -40, HEALTH(4) → 40-40 = 0, APP(8) → 3280-40 = +3240
 *
 * NOTE: Asymmetric range reflects ternary exponential — upper layers grow faster.
 *
 * USE CASE: Signed deviation from center, layer distance calculations.
 */
static const int16_t g_cpisi_layer_half_balanced[CPISI_FACE] = {
    -40,   /* [0] KERNEL    = 0-40 = -40     */
    -39,   /* [1] SYSTEM    = 1-40 = -39     */
    -36,   /* [2] RUNTIME   = 4-40 = -36     */
    -27,   /* [3] FRAMEWORK = 13-40 = -27    */
    0,     /* [4] HEALTH    = 40-40 = 0      */
    +81,   /* [5] NETWORK   = 121-40 = +81   */
    +324,  /* [6] STORAGE   = 364-40 = +324  */
    +1053, /* [7] ENGINE    = 1093-40 = +1053*/
    +3240  /* [8] APP       = 3280-40 = +3240*/
};

/**
 * @brief Ternary half lookup — maps layer index (0-8) to normalized float (0.0..1.0).
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - MAX_HALF = 3280 (APP layer half value)
 *
 * DERIVATION:
 *   Formula: ternary = g_cpisi_layer_half[n] / 3280.0
 *   Range: [0.0, 1.0] normalized to APP layer
 *   Exponential curve — lower layers cluster near 0
 *
 * NOTE: Values follow power-of-3 pattern: each layer ≈ 3× previous (roughly).
 *
 * USE CASE: Interpolation, visualization scaling, progress bars.
 */
static const float g_cpisi_layer_half_ternary[CPISI_FACE] = {
    0.0f,    /* [0] KERNEL    = 0/3280 = 0.0      */
    0.0003f, /* [1] SYSTEM    = 1/3280 ≈ 0.0003   */
    0.0012f, /* [2] RUNTIME   = 4/3280 ≈ 0.0012   */
    0.004f,  /* [3] FRAMEWORK = 13/3280 ≈ 0.004   */
    0.012f,  /* [4] HEALTH    = 40/3280 ≈ 0.012   */
    0.037f,  /* [5] NETWORK   = 121/3280 ≈ 0.037  */
    0.111f,  /* [6] STORAGE   = 364/3280 ≈ 0.111  */
    0.333f,  /* [7] ENGINE    = 1093/3280 ≈ 0.333 */
    1.0f     /* [8] APP       = 3280/3280 = 1.0   */
};

/**
 * @brief Center half reference — constant 40 for all layers.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — array size
 *   - CENTER_HALF = 40 (HEALTH layer half = (81-1)/2)
 *
 * DERIVATION:
 *   All entries = 40 (HEALTH layer's half value, the pyramid CENTER)
 *   Purpose: Reference point for balanced calculations
 *
 * RELATIONSHIP: Constant used to compute g_cpisi_layer_half_balanced.
 *
 * USE CASE: Baseline reference, deviation calculations, normalization base.
 */
static const uint16_t g_cpisi_layer_half_center[CPISI_FACE] = {
    40, /* [0] KERNEL    ref = 40  */
    40, /* [1] SYSTEM    ref = 40  */
    40, /* [2] RUNTIME   ref = 40  */
    40, /* [3] FRAMEWORK ref = 40  */
    40, /* [4] HEALTH    ref = 40  */
    40, /* [5] NETWORK   ref = 40  */
    40, /* [6] STORAGE   ref = 40  */
    40, /* [7] ENGINE    ref = 40  */
    40  /* [8] APP       ref = 40  */
};

/**
 * @brief Get layer half value — INDEX (0-8) → HALF (0..3280).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: uint16_t ∈ [0, 3280]
 *
 * FORMULA: Direct table lookup — g_cpisi_layer_half[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_layer_half(0) → 0    (KERNEL)
 *   cpisi_layer_half(3) → 13   (FRAMEWORK = BRIDGE)
 *   cpisi_layer_half(4) → 40   (HEALTH = CENTER)
 *   cpisi_layer_half(8) → 3280 (APP)
 *
 * @param idx Layer index 0-8 (unchecked)
 * @return Half value: 0 to 3280
 */
#define cpisi_layer_half(idx)           (g_cpisi_layer_half[(idx)])

/**
 * @brief Get layer balanced half — INDEX (0-8) → BALANCED (-40..+3240).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: int16_t ∈ [-40, +3240]
 *
 * FORMULA: Direct table lookup — g_cpisi_layer_half_balanced[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_layer_half_balanced(0) → -40   (KERNEL)
 *   cpisi_layer_half_balanced(4) →   0   (HEALTH)
 *   cpisi_layer_half_balanced(8) → +3240 (APP)
 *
 * @param idx Layer index 0-8 (unchecked)
 * @return Balanced half: -40 to +3240
 */
#define cpisi_layer_half_balanced(idx)  (g_cpisi_layer_half_balanced[(idx)])

/**
 * @brief Get layer ternary half — INDEX (0-8) → TERNARY (0.0..1.0).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: float ∈ [0.0, 1.0]
 *
 * FORMULA: Direct table lookup — g_cpisi_layer_half_ternary[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_layer_half_ternary(0) → 0.0f  (KERNEL)
 *   cpisi_layer_half_ternary(4) → 0.012f (HEALTH)
 *   cpisi_layer_half_ternary(8) → 1.0f  (APP)
 *
 * @param idx Layer index 0-8 (unchecked)
 * @return Ternary half: 0.0 to 1.0
 */
#define cpisi_layer_half_ternary(idx)   (g_cpisi_layer_half_ternary[(idx)])

/**
 * @brief Get layer center half reference — INDEX (0-8) → CENTER (constant 40).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_FACE-1] = [0, 8]
 *   - OUTPUT: uint16_t = 40 (constant)
 *
 * FORMULA: Direct table lookup — g_cpisi_layer_half_center[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 8.
 *
 * EXAMPLE:
 *   cpisi_layer_half_center(0) → 40
 *   cpisi_layer_half_center(4) → 40
 *   cpisi_layer_half_center(8) → 40
 *
 * @param idx Layer index 0-8 (unchecked)
 * @return Center reference: always 40
 */
#define cpisi_layer_half_center(idx)    (g_cpisi_layer_half_center[(idx)])

/** @} */ /* End HALF */

/** ── ZONE — 5 zone lookup tables ──────────────────────────────────────────────
 * @name ZONE
 *
 * TABLES PROVIDED:
 *   - g_cpisi_zone_boundary[4]         — int16_t: display thresholds
 *   - g_cpisi_zone_boundary_ternary[4] — float: ternary thresholds
 *   - g_cpisi_zone_name[5]             — char*: zone names
 *   - g_cpisi_zone_abbrev[5]           — char: single-char abbreviations
 *   - g_cpisi_zone_center[5]           — uint8_t: center index (all 2)
 *
 * ZONE MAPPING:
 *
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  INDEX   NAME       ABBREV   DISPLAY RANGE    HEBREW STATE          │
 *   │  ─────   ─────────  ──────   ─────────────    ───────────────────   │
 *   │  [0]     CRITICAL   C        -100..-51        shavar (broken)       │
 *   │  [1]     DEGRADED   D         -50..-26        chaser/ratsah         │
 *   │  [2]     BALANCED   B         -25..+25        yashar (center)       │
 *   │  [3]     HEALTHY    H         +26..+50        tamim/shalem          │
 *   │  [4]     OPTIMAL    O         +51..+100       tov (perfect)         │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * BOUNDARY THRESHOLDS (4 boundaries divide 5 zones):
 *
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  INDEX   DISPLAY    TERNARY    MEANING                              │
 *   │  ─────   ───────    ───────    ────────────────────────────────     │
 *   │  [0]     -50        -0.50      BROKEN threshold (shavar edge)       │
 *   │  [1]     -25        -0.25      LOWER threshold (k=-1 split)         │
 *   │  [2]     +25        +0.25      UPPER threshold (k=+1 split)         │
 *   │  [3]     +50        +0.50      PERFECT threshold (tov edge)         │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * @{ */

/**
 * @brief Zone boundary display values — maps boundary index (0-3) to display threshold.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_BOUNDARY_COUNT = 4 (S.2a.5b.2f) — array size
 *   - CPISI_ZONE_BROKEN = -50 (S.2a.5b.2f) — first threshold
 *   - CPISI_ZONE_PERFECT = +50 (S.2a.5b.2f) — last threshold
 *
 * VALUE MAPPING:
 *   [0] = -50 — BROKEN threshold (shavar edge)
 *   [1] = -25 — LOWER threshold (k=-1 split)
 *   [2] = +25 — UPPER threshold (k=+1 split)
 *   [3] = +50 — PERFECT threshold (tov edge)
 *
 * DERIVATION: Boundaries at ±ZONE_INNER (±25) and ±ZONE_OUTER (±50).
 *
 * USE CASE: Map display value to zone, threshold comparisons, UI visualization.
 *
 * RELATIONSHIP TO MACROS (S.2b.4j):
 *   - CPISI_IS_ZONE_BOUNDARY(b) validates index before access
 */
static const int16_t g_cpisi_zone_boundary[CPISI_ZONE_BOUNDARY_COUNT] = {
    -50,    /* [0] BROKEN threshold  */
    -25,    /* [1] LOWER threshold   */
    +25,    /* [2] UPPER threshold   */
    +50     /* [3] PERFECT threshold */
};

/**
 * @brief Zone boundary ternary values — maps boundary index (0-3) to ternary threshold.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_BOUNDARY_COUNT = 4 (S.2a.5b.2f) — array size
 *   - CPISI_TERNARY_ZONE_BROKEN_F = -0.50f (S.2a.5b.2f) — first threshold
 *   - CPISI_TERNARY_ZONE_PERFECT_F = +0.50f (S.2a.5b.2f) — last threshold
 *
 * VALUE MAPPING:
 *   [0] = -0.50f — BROKEN threshold (shavar edge)
 *   [1] = -0.25f — LOWER threshold (k=-1 split)
 *   [2] = +0.25f — UPPER threshold (k=+1 split)
 *   [3] = +0.50f — PERFECT threshold (tov edge)
 *
 * DERIVATION: ternary = display / 100.0f (display range ±100 → ternary ±1.0).
 *
 * USE CASE: Zone classification with floating-point health values.
 *
 * RELATIONSHIP TO DISPLAY:
 *   - g_cpisi_zone_boundary_ternary[i] = g_cpisi_zone_boundary[i] / 100.0f
 */
static const float g_cpisi_zone_boundary_ternary[CPISI_ZONE_BOUNDARY_COUNT] = {
    -0.50f,  /* [0] BROKEN threshold  */
    -0.25f,  /* [1] LOWER threshold   */
    +0.25f,  /* [2] UPPER threshold   */
    +0.50f   /* [3] PERFECT threshold */
};

/**
 * @brief Zone names — maps zone index (0-4) to human-readable name string.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_COUNT = 5 (S.2a.5b.2f) — array size
 *   - CPISI_HARDPOINTS = 5 (S.2a.1d) — also valid size reference
 *
 * VALUE MAPPING:
 *   [0] = "CRITICAL" — severe health issues (shavar zone)
 *   [1] = "DEGRADED" — noticeable problems (chaser/ratsah zone)
 *   [2] = "BALANCED" — CENTER/yashar (default equilibrium)
 *   [3] = "HEALTHY"  — good condition (tamim/shalem zone)
 *   [4] = "OPTIMAL"  — peak health (tov zone)
 *
 * CENTER: BALANCED (index 2) = HALF(5) = (5-1)/2 = 2
 *
 * USE CASE: Display zone status in logs, UI, diagnostics.
 *
 * RELATIONSHIP TO HEBREW STATES:
 *   - CRITICAL ↔ shavar (broken)
 *   - BALANCED ↔ yashar (even)
 *   - OPTIMAL ↔ tov (perfect)
 */
static const char* const g_cpisi_zone_name[CPISI_HARDPOINTS] = {
    "CRITICAL",  /* [0] shavar zone  */
    "DEGRADED",  /* [1] chaser zone  */
    "BALANCED",  /* [2] yashar zone  */
    "HEALTHY",   /* [3] shalem zone  */
    "OPTIMAL"    /* [4] tov zone     */
};

/**
 * @brief Zone abbreviations — maps zone index (0-4) to single-character code.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_COUNT = 5 (S.2a.5b.2f) — array size
 *   - CPISI_HARDPOINTS = 5 (S.2a.1d) — also valid size reference
 *
 * VALUE MAPPING:
 *   [0] = 'C' — CRITICAL
 *   [1] = 'D' — DEGRADED
 *   [2] = 'B' — BALANCED (CENTER)
 *   [3] = 'H' — HEALTHY
 *   [4] = 'O' — OPTIMAL
 *
 * USE CASE: Compact status display, log prefixes, single-char indicators.
 *
 * DESIGN: First letter of zone name for intuitive mapping.
 */
static const char g_cpisi_zone_abbrev[CPISI_HARDPOINTS] = {
    'C',  /* [0] CRITICAL */
    'D',  /* [1] DEGRADED */
    'B',  /* [2] BALANCED */
    'H',  /* [3] HEALTHY  */
    'O'   /* [4] OPTIMAL  */
};

/**
 * @brief Zone center reference — maps zone index (0-4) to center index (constant 2).
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_COUNT = 5 (S.2a.5b.2f) — array size
 *   - CPISI_ZONE_CENTER = 2 (S.2a.5b.2f) — universal center
 *
 * VALUE MAPPING:
 *   [0] = 2 — CRITICAL center reference
 *   [1] = 2 — DEGRADED center reference
 *   [2] = 2 — BALANCED center reference
 *   [3] = 2 — HEALTHY center reference
 *   [4] = 2 — OPTIMAL center reference
 *
 * WHY ALL 2: All zones share the same center (BALANCED = 2).
 *   This provides a uniform reference for "where should health target?"
 *
 * USE CASE: Target calculation, restoration goal, equilibrium reference.
 *
 * NOTE: Table exists for API consistency even though value is constant.
 */
static const uint8_t g_cpisi_zone_center[CPISI_HARDPOINTS] = {
    2, 2, 2, 2, 2  /* All zones share center = BALANCED (2) */
};

/* ─── ZONE ACCESSORS ─── */

/**
 * @brief Get zone boundary display value — INDEX (0-3) → DISPLAY threshold.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_ZONE_BOUNDARY_COUNT-1] = [0, 3]
 *   - OUTPUT: int16_t ∈ {-50, -25, +25, +50}
 *
 * FORMULA: Direct table lookup — g_cpisi_zone_boundary[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 3.
 *
 * EXAMPLE:
 *   cpisi_zone_boundary(0) → -50 (BROKEN)
 *   cpisi_zone_boundary(1) → -25 (LOWER)
 *   cpisi_zone_boundary(2) → +25 (UPPER)
 *   cpisi_zone_boundary(3) → +50 (PERFECT)
 *
 * @param idx Boundary index 0-3 (unchecked)
 * @return Display threshold: -50, -25, +25, or +50
 */
#define cpisi_zone_boundary(idx)         (g_cpisi_zone_boundary[(idx)])

/**
 * @brief Get zone boundary ternary value — INDEX (0-3) → TERNARY threshold.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_ZONE_BOUNDARY_COUNT-1] = [0, 3]
 *   - OUTPUT: float ∈ {-0.50f, -0.25f, +0.25f, +0.50f}
 *
 * FORMULA: Direct table lookup — g_cpisi_zone_boundary_ternary[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 3.
 *
 * EXAMPLE:
 *   cpisi_zone_boundary_ternary(0) → -0.50f (BROKEN)
 *   cpisi_zone_boundary_ternary(1) → -0.25f (LOWER)
 *   cpisi_zone_boundary_ternary(2) → +0.25f (UPPER)
 *   cpisi_zone_boundary_ternary(3) → +0.50f (PERFECT)
 *
 * @param idx Boundary index 0-3 (unchecked)
 * @return Ternary threshold: -0.50, -0.25, +0.25, or +0.50
 */
#define cpisi_zone_boundary_ternary(idx) (g_cpisi_zone_boundary_ternary[(idx)])

/**
 * @brief Get zone name — INDEX (0-4) → NAME string.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_ZONE_COUNT-1] = [0, 4]
 *   - OUTPUT: const char* ∈ {"CRITICAL", "DEGRADED", "BALANCED", "HEALTHY", "OPTIMAL"}
 *
 * FORMULA: Direct table lookup — g_cpisi_zone_name[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 4.
 *
 * EXAMPLE:
 *   cpisi_zone_name(0) → "CRITICAL"
 *   cpisi_zone_name(2) → "BALANCED"
 *   cpisi_zone_name(4) → "OPTIMAL"
 *
 * @param idx Zone index 0-4 (unchecked)
 * @return Zone name string (const char*)
 */
#define cpisi_zone_name(idx)             (g_cpisi_zone_name[(idx)])

/**
 * @brief Get zone abbreviation — INDEX (0-4) → ABBREV character.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_ZONE_COUNT-1] = [0, 4]
 *   - OUTPUT: char ∈ {'C', 'D', 'B', 'H', 'O'}
 *
 * FORMULA: Direct table lookup — g_cpisi_zone_abbrev[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 4.
 *
 * EXAMPLE:
 *   cpisi_zone_abbrev(0) → 'C' (CRITICAL)
 *   cpisi_zone_abbrev(2) → 'B' (BALANCED)
 *   cpisi_zone_abbrev(4) → 'O' (OPTIMAL)
 *
 * @param idx Zone index 0-4 (unchecked)
 * @return Single-character abbreviation
 */
#define cpisi_zone_abbrev(idx)           (g_cpisi_zone_abbrev[(idx)])

/**
 * @brief Get zone center reference — INDEX (0-4) → CENTER (constant 2).
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_ZONE_COUNT-1] = [0, 4]
 *   - OUTPUT: uint8_t = 2 (constant)
 *
 * FORMULA: Direct table lookup — g_cpisi_zone_center[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 4.
 *
 * EXAMPLE:
 *   cpisi_zone_center(0) → 2 (CRITICAL → target BALANCED)
 *   cpisi_zone_center(2) → 2 (BALANCED → already at center)
 *   cpisi_zone_center(4) → 2 (OPTIMAL → reference BALANCED)
 *
 * @param idx Zone index 0-4 (unchecked)
 * @return Center index: always 2 (BALANCED)
 */
#define cpisi_zone_center(idx)           (g_cpisi_zone_center[(idx)])

/** @} */ /* End ZONE */

/** ── IMPACT — 6 impact/severity lookup tables ─────────────────────────────────
 * @name IMPACT
 *
 * TABLES PROVIDED:
 *   - g_cpisi_impact_display[9]          — int16_t: display values (-100..+100)
 *   - g_cpisi_impact_ternary[9]          — float: ternary values (-1.0..+1.0)
 *   - g_cpisi_health_severity[9]         — int8_t: severity levels (-4..+4)
 *   - g_cpisi_health_severity_name[9]    — char*: severity names ("FATAL".."PERFECT")
 *   - g_cpisi_health_severity_abbrev[9]  — char: abbreviations ('F'..'P')
 *   - g_cpisi_impact_center[9]           — int16_t: all zeros (baseline)
 *
 * THE 9 SEVERITY LEVELS (4+1+4 pattern):
 *
 *   ┌─────────────────────────────────────────────────────────────────────────┐
 *   │  IDX  SEV  IMPACT  NAME       ABBR  MEANING                             │
 *   │  ───  ───  ──────  ─────────  ────  ────────────────────────────────    │
 *   │  [0]  -4   -100    FATAL      F     Crisis edge (shavar)                │
 *   │  [1]  -3   -75     HIGH       H     Severe negative                     │
 *   │  [2]  -2   -50     MEDIUM     M     Moderate negative                   │
 *   │  [3]  -1   -25     LOW        L     Minor negative                      │
 *   │  [4]   0     0     NONE       N     Center (yashar)                     │
 *   │  [5]  +1   +25     FAIR       f     Slight positive                     │
 *   │  [6]  +2   +50     GOOD       G     Moderate positive                   │
 *   │  [7]  +3   +75     EXCELLENT  E     Strong positive                     │
 *   │  [8]  +4  +100     PERFECT    P     Completion edge (tov)               │
 *   └─────────────────────────────────────────────────────────────────────────┘
 *
 * CONVERSION: severity × SEVERITY_STEP (25) = impact display value
 *
 * CENTER: Index 4 = NONE (yashar, severity 0, impact 0)
 *
 * @{ */

/**
 * @brief Impact display values by severity index — maps index (0-8) to display impact.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — array size (9 severity positions)
 *   - CPISI_HEALTH_SEVERITY_STEP = 25 (S.2a.6) — display units per severity level
 *   - CPISI_IMPACT_FATAL = -100 (S.2a.6) — minimum display value
 *   - CPISI_IMPACT_PERFECT = 100 (S.2a.6) — maximum display value
 *
 * DERIVATION:
 *   Each value = severity_level × SEVERITY_STEP:
 *   - [0] = -4 × 25 = -100 (FATAL)
 *   - [4] =  0 × 25 =    0 (NONE/center)
 *   - [8] = +4 × 25 = +100 (PERFECT)
 *
 * USE CASE:
 *   Convert severity position index to human-readable display percentage.
 *   Used in health status displays and logging.
 *
 * Index mapping:
 *   [0] → -100 (FATAL, severity -4)
 *   [1] → -75  (HIGH, severity -3)
 *   [2] → -50  (MEDIUM, severity -2)
 *   [3] → -25  (LOW, severity -1)
 *   [4] →   0  (NONE, severity 0) ← CENTER
 *   [5] → +25  (FAIR, severity +1)
 *   [6] → +50  (GOOD, severity +2)
 *   [7] → +75  (EXCELLENT, severity +3)
 *   [8] → +100 (PERFECT, severity +4)
 *
 * @see cpisi_impact_display
 * @see CPISI_HEALTH_SEVERITY_STEP
 */
static const int16_t g_cpisi_impact_display[CPISI_FACE] = {
    -100,  /**< 0: FATAL (severity -4, -4 × 25) */
    -75,   /**< 1: HIGH (severity -3, -3 × 25) */
    -50,   /**< 2: MEDIUM (severity -2, -2 × 25) */
    -25,   /**< 3: LOW (severity -1, -1 × 25) */
    0,     /**< 4: NONE (severity 0, 0 × 25) — CENTER */
    +25,   /**< 5: FAIR (severity +1, +1 × 25) */
    +50,   /**< 6: GOOD (severity +2, +2 × 25) */
    +75,   /**< 7: EXCELLENT (severity +3, +3 × 25) */
    +100   /**< 8: PERFECT (severity +4, +4 × 25) */
};

/**
 * @brief Impact ternary values by severity index — maps index (0-8) to normalized ternary.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — array size (9 severity positions)
 *   - CPISI_TERNARY_NEG = -1.0 (S.2a.6) — minimum ternary value
 *   - CPISI_TERNARY_POS = +1.0 (S.2a.6) — maximum ternary value
 *
 * DERIVATION:
 *   Each value = severity_level / FACE_CENTER:
 *   - [0] = -4 / 4 = -1.0 (shavar edge)
 *   - [4] =  0 / 4 =  0.0 (yashar center)
 *   - [8] = +4 / 4 = +1.0 (tov edge)
 *
 * USE CASE:
 *   Normalized representation for mathematical operations and comparisons.
 *   Maps 9 discrete levels to continuous -1.0..+1.0 range.
 *
 * Index mapping:
 *   [0] → -1.0  (shavar, FATAL)
 *   [1] → -0.75 (chaser, HIGH)
 *   [2] → -0.5  (chaser, MEDIUM)
 *   [3] → -0.25 (ratsah, LOW)
 *   [4] →  0.0  (yashar, NONE) ← CENTER
 *   [5] → +0.25 (tamim, FAIR)
 *   [6] → +0.5  (shalem, GOOD)
 *   [7] → +0.75 (shalem, EXCELLENT)
 *   [8] → +1.0  (tov, PERFECT)
 *
 * @see cpisi_impact_ternary
 * @see g_cpisi_impact_display
 */
static const float g_cpisi_impact_ternary[CPISI_FACE] = {
    -1.0f,   /**< 0: shavar (FATAL, normalized -1.0) */
    -0.75f,  /**< 1: chaser (HIGH, normalized -0.75) */
    -0.5f,   /**< 2: chaser (MEDIUM, normalized -0.5) */
    -0.25f,  /**< 3: ratsah (LOW, normalized -0.25) */
    0.0f,    /**< 4: yashar (NONE, normalized 0.0) — CENTER */
    +0.25f,  /**< 5: tamim (FAIR, normalized +0.25) */
    +0.5f,   /**< 6: shalem (GOOD, normalized +0.5) */
    +0.75f,  /**< 7: shalem (EXCELLENT, normalized +0.75) */
    +1.0f    /**< 8: tov (PERFECT, normalized +1.0) */
};

/**
 * @brief Severity level values by index — maps index (0-8) to severity level.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — array size (9 severity positions)
 *   - CPISI_HEALTH_SEVERITY_MIN = -4 (S.2a.6) — minimum severity level
 *   - CPISI_HEALTH_SEVERITY_MAX = +4 (S.2a.6) — maximum severity level
 *   - CPISI_HEALTH_SEVERITY_CENTER = 0 (S.2a.6) — center severity level
 *
 * DERIVATION:
 *   Direct severity value in the 4+1+4 pattern:
 *   - index - FACE_CENTER = severity
 *   - [0] - 4 = -4 (FATAL)
 *   - [4] - 4 =  0 (NONE)
 *   - [8] - 4 = +4 (PERFECT)
 *
 * USE CASE:
 *   Convert position index to raw severity value for calculations.
 *   Inverse of (severity + FACE_CENTER) → index conversion.
 *
 * Index mapping:
 *   [0] → -4 (FATAL)
 *   [1] → -3 (HIGH)
 *   [2] → -2 (MEDIUM)
 *   [3] → -1 (LOW)
 *   [4] →  0 (NONE) ← CENTER
 *   [5] → +1 (FAIR)
 *   [6] → +2 (GOOD)
 *   [7] → +3 (EXCELLENT)
 *   [8] → +4 (PERFECT)
 *
 * @see cpisi_health_severity
 * @see CPISI_HEALTH_SEVERITY_MIN
 * @see CPISI_HEALTH_SEVERITY_MAX
 */
static const int8_t g_cpisi_health_severity[CPISI_FACE] = {
    -4,  /**< 0: FATAL */
    -3,  /**< 1: HIGH */
    -2,  /**< 2: MEDIUM */
    -1,  /**< 3: LOW */
    0,   /**< 4: NONE — CENTER */
    +1,  /**< 5: FAIR */
    +2,  /**< 6: GOOD */
    +3,  /**< 7: EXCELLENT */
    +4   /**< 8: PERFECT */
};

/**
 * @brief Severity name strings by index — maps index (0-8) to human-readable name.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — array size (9 severity positions)
 *
 * DERIVATION:
 *   Human-readable names for each severity level in the 4+1+4 pattern.
 *   Negative severities use intense names (FATAL, HIGH, MEDIUM, LOW).
 *   Positive severities use quality names (FAIR, GOOD, EXCELLENT, PERFECT).
 *
 * USE CASE:
 *   Display severity status in logs, UI, and diagnostic output.
 *   Provides clear human-readable labels for each severity position.
 *
 * Index mapping:
 *   [0] → "FATAL"     (severity -4)
 *   [1] → "HIGH"      (severity -3)
 *   [2] → "MEDIUM"    (severity -2)
 *   [3] → "LOW"       (severity -1)
 *   [4] → "NONE"      (severity 0) ← CENTER
 *   [5] → "FAIR"      (severity +1)
 *   [6] → "GOOD"      (severity +2)
 *   [7] → "EXCELLENT" (severity +3)
 *   [8] → "PERFECT"   (severity +4)
 *
 * @see cpisi_health_severity_name
 * @see g_cpisi_health_severity_abbrev
 */
static const char* const g_cpisi_health_severity_name[CPISI_FACE] = {
    "FATAL",      /**< 0: severity -4 */
    "HIGH",       /**< 1: severity -3 */
    "MEDIUM",     /**< 2: severity -2 */
    "LOW",        /**< 3: severity -1 */
    "NONE",       /**< 4: severity 0 — CENTER */
    "FAIR",       /**< 5: severity +1 */
    "GOOD",       /**< 6: severity +2 */
    "EXCELLENT",  /**< 7: severity +3 */
    "PERFECT"     /**< 8: severity +4 */
};

/**
 * @brief Severity abbreviation characters by index — maps index (0-8) to single char.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — array size (9 severity positions)
 *
 * DERIVATION:
 *   Single-character abbreviations for compact display.
 *   Convention: Most chars uppercase, FAIR uses lowercase 'f' to distinguish
 *   from FATAL ('F') in compact output.
 *
 * USE CASE:
 *   Compact status display in constrained UI or log formats.
 *   Single character per severity level for dense output.
 *
 * Index mapping:
 *   [0] → 'F' (FATAL)
 *   [1] → 'H' (HIGH)
 *   [2] → 'M' (MEDIUM)
 *   [3] → 'L' (LOW)
 *   [4] → 'N' (NONE) ← CENTER
 *   [5] → 'f' (FAIR, lowercase to distinguish from FATAL)
 *   [6] → 'G' (GOOD)
 *   [7] → 'E' (EXCELLENT)
 *   [8] → 'P' (PERFECT)
 *
 * @see cpisi_health_severity_abbrev
 * @see g_cpisi_health_severity_name
 */
static const char g_cpisi_health_severity_abbrev[CPISI_FACE] = {
    'F',  /**< 0: FATAL */
    'H',  /**< 1: HIGH */
    'M',  /**< 2: MEDIUM */
    'L',  /**< 3: LOW */
    'N',  /**< 4: NONE — CENTER */
    'f',  /**< 5: FAIR (lowercase to distinguish from FATAL) */
    'G',  /**< 6: GOOD */
    'E',  /**< 7: EXCELLENT */
    'P'   /**< 8: PERFECT */
};

/**
 * @brief Center impact values by index — all zeros baseline reference.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — array size (9 severity positions)
 *   - CPISI_IMPACT_NONE = 0 (S.2a.6) — center/baseline value
 *
 * DERIVATION:
 *   All-zeros array provides baseline reference for impact comparison.
 *   Every position returns 0 (the center/neutral impact value).
 *
 * USE CASE:
 *   Reset or comparison operations where baseline reference is needed.
 *   Provides uniform "no impact" array for initialization.
 *
 * Index mapping:
 *   [0..8] → 0 (baseline center)
 *
 * @see cpisi_impact_center
 * @see g_cpisi_impact_display
 */
static const int16_t g_cpisi_impact_center[CPISI_FACE] = {
    0,  /**< 0: center baseline */
    0,  /**< 1: center baseline */
    0,  /**< 2: center baseline */
    0,  /**< 3: center baseline */
    0,  /**< 4: center baseline — NONE */
    0,  /**< 5: center baseline */
    0,  /**< 6: center baseline */
    0,  /**< 7: center baseline */
    0   /**< 8: center baseline */
};

/**
 * @brief Get display impact value by severity index.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — valid index range (0-8)
 *
 * USE CASE:
 *   Access display impact value for given severity position.
 *
 * EXAMPLE:
 *   cpisi_impact_display(0)  → -100 (FATAL)
 *   cpisi_impact_display(4)  → 0    (NONE)
 *   cpisi_impact_display(8)  → +100 (PERFECT)
 *
 * @param idx  Position index (0-8, unchecked)
 * @return     Display impact value (-100 to +100)
 *
 * @see g_cpisi_impact_display
 */
#define cpisi_impact_display(idx)         (g_cpisi_impact_display[(idx)])

/**
 * @brief Get ternary impact value by severity index.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — valid index range (0-8)
 *
 * USE CASE:
 *   Access normalized ternary value for mathematical operations.
 *
 * EXAMPLE:
 *   cpisi_impact_ternary(0)  → -1.0  (shavar)
 *   cpisi_impact_ternary(4)  → 0.0   (yashar)
 *   cpisi_impact_ternary(8)  → +1.0  (tov)
 *
 * @param idx  Position index (0-8, unchecked)
 * @return     Ternary impact value (-1.0 to +1.0)
 *
 * @see g_cpisi_impact_ternary
 */
#define cpisi_impact_ternary(idx)         (g_cpisi_impact_ternary[(idx)])

/**
 * @brief Get severity level by index.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — valid index range (0-8)
 *
 * USE CASE:
 *   Access raw severity value for calculations.
 *
 * EXAMPLE:
 *   cpisi_health_severity(0)  → -4 (FATAL)
 *   cpisi_health_severity(4)  → 0  (NONE)
 *   cpisi_health_severity(8)  → +4 (PERFECT)
 *
 * @param idx  Position index (0-8, unchecked)
 * @return     Severity level (-4 to +4)
 *
 * @see g_cpisi_health_severity
 */
#define cpisi_health_severity(idx)        (g_cpisi_health_severity[(idx)])

/**
 * @brief Get severity name string by index.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — valid index range (0-8)
 *
 * USE CASE:
 *   Access human-readable severity name for display/logging.
 *
 * EXAMPLE:
 *   cpisi_health_severity_name(0)  → "FATAL"
 *   cpisi_health_severity_name(4)  → "NONE"
 *   cpisi_health_severity_name(8)  → "PERFECT"
 *
 * @param idx  Position index (0-8, unchecked)
 * @return     Severity name string (e.g., "FATAL", "NONE", "PERFECT")
 *
 * @see g_cpisi_health_severity_name
 */
#define cpisi_health_severity_name(idx)   (g_cpisi_health_severity_name[(idx)])

/**
 * @brief Get severity abbreviation character by index.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — valid index range (0-8)
 *
 * USE CASE:
 *   Access single-character abbreviation for compact display.
 *
 * EXAMPLE:
 *   cpisi_health_severity_abbrev(0)  → 'F' (FATAL)
 *   cpisi_health_severity_abbrev(4)  → 'N' (NONE)
 *   cpisi_health_severity_abbrev(8)  → 'P' (PERFECT)
 *
 * @param idx  Position index (0-8, unchecked)
 * @return     Single character abbreviation ('F', 'N', 'P', etc.)
 *
 * @see g_cpisi_health_severity_abbrev
 */
#define cpisi_health_severity_abbrev(idx) (g_cpisi_health_severity_abbrev[(idx)])

/**
 * @brief Get center impact value by index (always returns 0).
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — valid index range (0-8)
 *
 * USE CASE:
 *   Access baseline reference value for reset or comparison.
 *
 * EXAMPLE:
 *   cpisi_impact_center(0)  → 0 (baseline)
 *   cpisi_impact_center(4)  → 0 (baseline)
 *   cpisi_impact_center(8)  → 0 (baseline)
 *
 * @param idx  Position index (0-8, unchecked)
 * @return     Center value (always 0)
 *
 * @see g_cpisi_impact_center
 */
#define cpisi_impact_center(idx)          (g_cpisi_impact_center[(idx)])

/** @} */ /* End IMPACT */

/** ── NORMALIZATION — K-factor, positions, divisor tables ──────────────────────
 * @name NORMALIZATION
 *
 * TABLES PROVIDED (13 total):
 *   K-Factor:    g_cpisi_k_factor[3], g_cpisi_k_center[1],
 *                g_cpisi_k_name[3], g_cpisi_k_name_center[1],
 *                g_cpisi_k_sign[3], g_cpisi_k_sign_center[1]
 *   Positions:   g_cpisi_positions[3], g_cpisi_positions_center[1]
 *   Divisors:    g_cpisi_divisor[3], g_cpisi_divisor_center[1]
 *   True Score:  g_cpisi_true_bounds[3], g_cpisi_true_center[1], g_cpisi_true_half[3]
 *
 * TABLE PATTERN: Main table[TERNARY] + center anchor table[UNITY]
 *
 * Index mapping for all tables:
 *   [0] = negative direction (retreat/shavar/k=-1)
 *   [1] = center/anchor (maintain/yashar/k=0)
 *   [2] = positive direction (advance/tov/k=+1)
 *
 * @{ */

/* ── K_FACTOR — K-factor values, names, and signs ──────────────────────────── */

/**
 * @brief K-factor numeric values for directional operations.
 *
 * BOUNDS USED:
 *   - K_MIN (-1), K_CENTER (0), K_MAX (+1) from S.2a.5b.2i
 *   - CPISI_TERNARY (3) — array size
 *
 * DERIVATION:
 *   Each entry is a bound from S.2a.5b.2i, stored for O(1) lookup.
 *
 * USE CASE:
 *   Get k-factor value by position index for arithmetic operations.
 *
 * INDEX MAPPING:
 *   [0] = K_MIN    = -1 (retreat/shavar direction)
 *   [1] = K_CENTER =  0 (maintain/yashar anchor)
 *   [2] = K_MAX    = +1 (advance/tov direction)
 *
 * @see g_cpisi_k_center for anchor-only access
 * @see g_cpisi_k_name for string representations
 */
static const int8_t g_cpisi_k_factor[CPISI_TERNARY] = {
    CPISI_K_MIN,                                          /**< [0] = -1 (retreat/shavar) */
    CPISI_K_CENTER,                                       /**< [1] =  0 (maintain/yashar) */
    CPISI_K_MAX                                           /**< [2] = +1 (advance/tov) */
};

/**
 * @brief K-factor center anchor array.
 *
 * BOUNDS USED:
 *   - K_CENTER (0) from S.2a.5b.2i
 *   - CPISI_UNITY (1) — array size
 *
 * DERIVATION:
 *   Single-element array containing yashar anchor value.
 *
 * USE CASE:
 *   Access the neutral k-factor directly without index.
 *
 * INDEX MAPPING:
 *   [0] = K_CENTER = 0 (yashar anchor)
 *
 * @see g_cpisi_k_factor for full ternary array
 */
static const int8_t g_cpisi_k_center[CPISI_UNITY] = {
    CPISI_K_CENTER                                        /**< [0] = 0 (yashar anchor) */
};

/**
 * @brief K-factor direction names for display.
 *
 * BOUNDS USED:
 *   - CPISI_TERNARY (3) — array size
 *
 * DERIVATION:
 *   Semantic names aligned with Hebrew cognates and action meaning.
 *
 * USE CASE:
 *   Display direction in logs, UI, and debugging.
 *
 * INDEX MAPPING:
 *   [0] = "retreat"  (shavar direction — toward corruption)
 *   [1] = "maintain" (yashar anchor — hold steady)
 *   [2] = "advance"  (tov direction — toward perfection)
 *
 * @see g_cpisi_k_name_center for anchor-only access
 */
static const char* const g_cpisi_k_name[CPISI_TERNARY] = {
    "retreat",                                            /**< [0] = shavar direction */
    "maintain",                                           /**< [1] = yashar direction */
    "advance"                                             /**< [2] = tov direction */
};

/**
 * @brief K-factor center name anchor array.
 *
 * BOUNDS USED:
 *   - CPISI_UNITY (1) — array size
 *
 * DERIVATION:
 *   Single-element array containing yashar anchor name.
 *
 * USE CASE:
 *   Access the neutral direction name directly.
 *
 * INDEX MAPPING:
 *   [0] = "maintain" (yashar anchor)
 *
 * @see g_cpisi_k_name for full ternary array
 */
static const char* const g_cpisi_k_name_center[CPISI_UNITY] = {
    "maintain"                                            /**< [0] = yashar (anchor) */
};

/**
 * @brief K-factor direction signs for compact display.
 *
 * BOUNDS USED:
 *   - CPISI_TERNARY (3) — array size
 *
 * DERIVATION:
 *   Single-character representations of direction.
 *
 * USE CASE:
 *   Compact status displays and logging.
 *
 * INDEX MAPPING:
 *   [0] = '-' (negative/retreat direction)
 *   [1] = '0' (neutral/maintain anchor)
 *   [2] = '+' (positive/advance direction)
 *
 * @see g_cpisi_k_sign_center for anchor-only access
 */
static const char g_cpisi_k_sign[CPISI_TERNARY] = {
    '-',                                                  /**< [0] = negative direction */
    '0',                                                  /**< [1] = neutral (anchor) */
    '+'                                                   /**< [2] = positive direction */
};

/**
 * @brief K-factor center sign anchor array.
 *
 * BOUNDS USED:
 *   - CPISI_UNITY (1) — array size
 *
 * DERIVATION:
 *   Single-element array containing neutral sign character.
 *
 * USE CASE:
 *   Access the neutral sign directly.
 *
 * INDEX MAPPING:
 *   [0] = '0' (neutral anchor)
 *
 * @see g_cpisi_k_sign for full ternary array
 */
static const char g_cpisi_k_sign_center[CPISI_UNITY] = {
    '0'                                                   /**< [0] = neutral (anchor) */
};

/* ── POSITIONS — Position counts per direction ─────────────────────────────── */

/**
 * @brief Position counts per k-factor direction.
 *
 * BOUNDS USED:
 *   - POSITIONS_NEG (128), POSITIONS_CENTER (1), POSITIONS_POS (127) from S.2a.5b.2i
 *   - CPISI_TERNARY (3) — array size
 *
 * DERIVATION:
 *   Each entry is a bound from S.2a.5b.2i.
 *   Asymmetry: NEG(128) > POS(127) — one more step to corruption.
 *
 * USE CASE:
 *   Get the number of discrete positions in a given direction.
 *
 * INDEX MAPPING:
 *   [0] = POSITIONS_NEG    = 128 (corruption steps: -128 to -1)
 *   [1] = POSITIONS_CENTER =   1 (yashar anchor: 0 only)
 *   [2] = POSITIONS_POS    = 127 (restoration steps: +1 to +127)
 *
 * @see g_cpisi_positions_center for anchor-only access
 * @see g_cpisi_divisor for float normalization divisors
 */
static const uint8_t g_cpisi_positions[CPISI_TERNARY] = {
    CPISI_POSITIONS_NEG,                                  /**< [0] = 128 (corruption steps) */
    CPISI_POSITIONS_CENTER,                               /**< [1] =   1 (yashar anchor) */
    CPISI_POSITIONS_POS                                   /**< [2] = 127 (restoration steps) */
};

/**
 * @brief Position center anchor array.
 *
 * BOUNDS USED:
 *   - POSITIONS_CENTER (1) from S.2a.5b.2i
 *   - CPISI_UNITY (1) — array size
 *
 * DERIVATION:
 *   Single-element array for yashar position count (always 1).
 *
 * USE CASE:
 *   Access the anchor position count directly.
 *
 * INDEX MAPPING:
 *   [0] = POSITIONS_CENTER = 1 (anchor — only one neutral state)
 *
 * @see g_cpisi_positions for full ternary array
 */
static const uint8_t g_cpisi_positions_center[CPISI_UNITY] = {
    CPISI_POSITIONS_CENTER                                /**< [0] = 1 (anchor) */
};

/* ── DIVISORS — Normalization divisors ─────────────────────────────────────── */

/**
 * @brief Normalization divisors for true_score → normalized conversion.
 *
 * BOUNDS USED:
 *   - NORM_DIVISOR_NEG (128.0f), NORM_DIVISOR_CENTER (1.0f), NORM_DIVISOR_POS (127.0f)
 *   - CPISI_TERNARY (3) — array size
 *
 * DERIVATION:
 *   Float divisors enabling proper normalization to [-1.0, +1.0] range.
 *   Asymmetric: NEG(128.0f) ≠ POS(127.0f) for accurate scaling.
 *
 * USE CASE:
 *   Get the appropriate divisor for normalizing a true score.
 *   Formula: normalized = true_score / divisor[k]
 *
 * INDEX MAPPING:
 *   [0] = NORM_DIVISOR_NEG    = 128.0f (divide negative scores by 128)
 *   [1] = NORM_DIVISOR_CENTER =   1.0f (identity — no scaling at center)
 *   [2] = NORM_DIVISOR_POS    = 127.0f (divide positive scores by 127)
 *
 * @see g_cpisi_divisor_center for anchor-only access
 * @see CPISI_NORM_DIVISOR_* macros for raw values
 */
static const float g_cpisi_divisor[CPISI_TERNARY] = {
    CPISI_NORM_DIVISOR_NEG,                               /**< [0] = 128.0 (corruption range) */
    CPISI_NORM_DIVISOR_CENTER,                            /**< [1] =   1.0 (identity) */
    CPISI_NORM_DIVISOR_POS                                /**< [2] = 127.0 (restoration range) */
};

/**
 * @brief Divisor center anchor array.
 *
 * BOUNDS USED:
 *   - NORM_DIVISOR_CENTER (1.0f) from S.2a.5b.2i
 *   - CPISI_UNITY (1) — array size
 *
 * DERIVATION:
 *   Single-element array for identity divisor (1.0f — no scaling).
 *
 * USE CASE:
 *   Access the identity divisor directly.
 *
 * INDEX MAPPING:
 *   [0] = NORM_DIVISOR_CENTER = 1.0f (identity divisor)
 *
 * @see g_cpisi_divisor for full ternary array
 */
static const float g_cpisi_divisor_center[CPISI_UNITY] = {
    CPISI_NORM_DIVISOR_CENTER                             /**< [0] = 1.0 (identity) */
};

/* ── TRUE_SCORE — True score bounds and halves ─────────────────────────────── */

/**
 * @brief True score boundary values for each direction.
 *
 * BOUNDS USED:
 *   - TRUE_MIN (-128), TRUE_CENTER (0), TRUE_MAX (+127) from S.2a.5b.2i
 *   - CPISI_TERNARY (3) — array size
 *
 * DERIVATION:
 *   Edge/anchor values in each direction. Defines the full true_score range.
 *
 * USE CASE:
 *   Get the boundary value for a given direction.
 *
 * INDEX MAPPING:
 *   [0] = TRUE_MIN    = -128 (shavar edge — maximum corruption)
 *   [1] = TRUE_CENTER =    0 (yashar anchor — neutral)
 *   [2] = TRUE_MAX    = +127 (tov edge — maximum restoration)
 *
 * @see g_cpisi_true_center for anchor-only access
 * @see g_cpisi_true_half for midpoint values
 */
static const int16_t g_cpisi_true_bounds[CPISI_TERNARY] = {
    CPISI_TRUE_MIN,                                       /**< [0] = -128 (shavar edge) */
    CPISI_TRUE_CENTER,                                    /**< [1] =    0 (yashar anchor) */
    CPISI_TRUE_MAX                                        /**< [2] = +127 (tov edge) */
};

/**
 * @brief True score center anchor array.
 *
 * BOUNDS USED:
 *   - TRUE_CENTER (0) from S.2a.5b.2i
 *   - CPISI_UNITY (1) — array size
 *
 * DERIVATION:
 *   Single-element array for yashar true_score value (always 0).
 *
 * USE CASE:
 *   Access the neutral true score directly.
 *
 * INDEX MAPPING:
 *   [0] = TRUE_CENTER = 0 (yashar anchor)
 *
 * @see g_cpisi_true_bounds for full ternary array
 */
static const int16_t g_cpisi_true_center[CPISI_UNITY] = {
    CPISI_TRUE_CENTER                                     /**< [0] = 0 (yashar anchor) */
};

/**
 * @brief True score half (midpoint) values for each direction.
 *
 * BOUNDS USED:
 *   - TRUE_HALF_NEG (63), TRUE_HALF_POS (63) from S.2a.5b.2i
 *   - CPISI_TERNARY (3) — array size
 *
 * DERIVATION:
 *   Midpoint values for each direction. Used for thresholds.
 *   Formula: HALF(n) = (n-1)/2 → HALF(128)=63, HALF(127)=63
 *
 * USE CASE:
 *   Get midpoint for threshold comparisons and state classification.
 *
 * INDEX MAPPING:
 *   [0] = -TRUE_HALF_NEG = -63 (half corruption — midpoint to shavar)
 *   [1] =              0 =   0 (yashar anchor — center is center)
 *   [2] =  TRUE_HALF_POS = +63 (half restoration — midpoint to tov)
 *
 * @see CPISI_TRUE_HALF_NEG, CPISI_TRUE_HALF_POS macros
 */
static const int16_t g_cpisi_true_half[CPISI_TERNARY] = {
    CPISI_NEGATE(CPISI_TRUE_HALF_NEG),                    /**< [0] = -63 (half corruption) */
    0,                                                    /**< [1] =   0 (yashar anchor) */
    CPISI_TRUE_HALF_POS                                   /**< [2] = +63 (half restoration) */
};

/** @} */ /* End NORMALIZATION */
/** @} */ /* end S.2d.9a TABLES */

/**
 * @defgroup S2d_9b_ACCESSORS S.2d.9b ACCESSORS — Macro wrappers for array access
 * @ingroup S2d_9_HEALTH
 * @brief Accessor macros for k-factor, positions, divisor, true score lookups.
 *
 * ACCESSORS PROVIDED (13 total):
 *   K-Factor:    cpisi_k_factor(), cpisi_k_center(), cpisi_k_name(),
 *                cpisi_k_name_center(), cpisi_k_sign(), cpisi_k_sign_center()
 *   Positions:   cpisi_positions(), cpisi_positions_center()
 *   Divisors:    cpisi_divisor(), cpisi_divisor_center()
 *   True Score:  cpisi_true_bounds(), cpisi_true_center(), cpisi_true_half()
 *
 * NO BOUNDS CHECKING: All accessors perform direct array access.
 * Caller must ensure valid index range.
 *
 * @{
 */

/**
 * @brief Access k-factor value by index.
 *
 * BOUNDS USED:
 *   - g_cpisi_k_factor[TERNARY] — source array
 *
 * USE CASE:
 *   Get k-factor value by position index.
 *
 * EXAMPLE:
 *   cpisi_k_factor(0) → -1 (retreat)
 *   cpisi_k_factor(1) →  0 (maintain)
 *   cpisi_k_factor(2) → +1 (advance)
 *
 * @param idx  K-factor index (0=retreat, 1=maintain, 2=advance)
 * @return     K-factor value (-1, 0, or +1)
 */
#define cpisi_k_factor(idx)         g_cpisi_k_factor[idx]

/**
 * @brief Access k-factor center anchor directly.
 *
 * BOUNDS USED:
 *   - g_cpisi_k_center[UNITY] — source array
 *
 * USE CASE:
 *   Get the neutral k-factor without index.
 *
 * EXAMPLE:
 *   cpisi_k_center() → 0 (yashar anchor)
 *
 * @return  K_CENTER value (0 — yashar anchor)
 */
#define cpisi_k_center()            g_cpisi_k_center[0]

/**
 * @brief Access k-factor name by index.
 *
 * BOUNDS USED:
 *   - g_cpisi_k_name[TERNARY] — source array
 *
 * USE CASE:
 *   Get direction name for display.
 *
 * EXAMPLE:
 *   cpisi_k_name(0) → "retreat"
 *   cpisi_k_name(1) → "maintain"
 *   cpisi_k_name(2) → "advance"
 *
 * @param idx  K-factor index (0=retreat, 1=maintain, 2=advance)
 * @return     Direction name string
 */
#define cpisi_k_name(idx)           g_cpisi_k_name[idx]

/**
 * @brief Access k-factor center name anchor directly.
 *
 * @return  "maintain" string (yashar anchor name)
 */
#define cpisi_k_name_center()       g_cpisi_k_name_center[0]

/**
 * @brief Access k-factor sign by index.
 *
 * BOUNDS USED:
 *   - g_cpisi_k_sign[TERNARY] — source array
 *
 * USE CASE:
 *   Get sign character for compact display.
 *
 * EXAMPLE:
 *   cpisi_k_sign(0) → '-'
 *   cpisi_k_sign(1) → '0'
 *   cpisi_k_sign(2) → '+'
 *
 * @param idx  K-factor index (0=retreat, 1=maintain, 2=advance)
 * @return     Sign character ('-', '0', or '+')
 */
#define cpisi_k_sign(idx)           g_cpisi_k_sign[idx]

/**
 * @brief Access k-factor center sign anchor directly.
 *
 * @return  '0' character (neutral sign)
 */
#define cpisi_k_sign_center()       g_cpisi_k_sign_center[0]

/**
 * @brief Access position count by index.
 *
 * BOUNDS USED:
 *   - g_cpisi_positions[TERNARY] — source array
 *
 * USE CASE:
 *   Get step count for a direction.
 *
 * EXAMPLE:
 *   cpisi_positions(0) → 128 (corruption steps)
 *   cpisi_positions(1) →   1 (yashar anchor)
 *   cpisi_positions(2) → 127 (restoration steps)
 *
 * @param idx  K-factor index (0=neg, 1=center, 2=pos)
 * @return     Position count (128, 1, or 127)
 */
#define cpisi_positions(idx)        g_cpisi_positions[idx]

/**
 * @brief Access position center anchor directly.
 *
 * @return  POSITIONS_CENTER value (1 — only one neutral state)
 */
#define cpisi_positions_center()    g_cpisi_positions_center[0]

/**
 * @brief Access normalization divisor by index.
 *
 * BOUNDS USED:
 *   - g_cpisi_divisor[TERNARY] — source array
 *
 * USE CASE:
 *   Get divisor for true_score normalization.
 *
 * EXAMPLE:
 *   cpisi_divisor(0) → 128.0f
 *   cpisi_divisor(1) →   1.0f
 *   cpisi_divisor(2) → 127.0f
 *
 * @param idx  K-factor index (0=neg, 1=center, 2=pos)
 * @return     Divisor float (128.0f, 1.0f, or 127.0f)
 */
#define cpisi_divisor(idx)          g_cpisi_divisor[idx]

/**
 * @brief Access divisor center anchor directly.
 *
 * @return  NORM_DIVISOR_CENTER value (1.0f — identity)
 */
#define cpisi_divisor_center()      g_cpisi_divisor_center[0]

/**
 * @brief Access true score bound by index.
 *
 * BOUNDS USED:
 *   - g_cpisi_true_bounds[TERNARY] — source array
 *
 * USE CASE:
 *   Get boundary value for a direction.
 *
 * EXAMPLE:
 *   cpisi_true_bounds(0) → -128 (shavar edge)
 *   cpisi_true_bounds(1) →    0 (yashar anchor)
 *   cpisi_true_bounds(2) → +127 (tov edge)
 *
 * @param idx  K-factor index (0=min, 1=center, 2=max)
 * @return     Boundary value (-128, 0, or +127)
 */
#define cpisi_true_bounds(idx)      g_cpisi_true_bounds[idx]

/**
 * @brief Access true score center anchor directly.
 *
 * @return  TRUE_CENTER value (0 — yashar anchor)
 */
#define cpisi_true_center()         g_cpisi_true_center[0]

/**
 * @brief Access true score half by index.
 *
 * BOUNDS USED:
 *   - g_cpisi_true_half[TERNARY] — source array
 *
 * USE CASE:
 *   Get midpoint value for threshold comparisons.
 *
 * EXAMPLE:
 *   cpisi_true_half(0) → -63 (half to shavar)
 *   cpisi_true_half(1) →   0 (yashar anchor)
 *   cpisi_true_half(2) → +63 (half to tov)
 *
 * @param idx  K-factor index (0=neg, 1=center, 2=pos)
 * @return     Half value (-63, 0, or +63)
 */
#define cpisi_true_half(idx)        g_cpisi_true_half[idx]

/** @} */ /* end S.2d.9b ACCESSORS */

/**
 * @defgroup S2d_9c_CONVERSION S.2d.9c CONVERSION [Reserved]
 * @ingroup S2d_9_HEALTH
 * @brief [Reserved] Format conversion utilities for health domain.
 *
 * RESERVED FOR:
 *   - Display format conversion functions
 *   - Ternary to display scaling
 *   - Zone/milestone string formatting
 *
 * @{
 */
/** @} */ /* end S.2d.9c CONVERSION */

/** @} */ /* end S.2d.9 HEALTH */
/** @} */ /* end S.2d LOOKUP */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e TYPES — Health domain type definitions [TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_types S.2e HEALTH TYPES — Enumerations
 * @ingroup cpisi_health_domain
 * @brief CpisiHealthState, CpisiSeverity, CpisiTrend, and related types.
 *
 * PURPOSE: Type definitions for health domain.
 *
 * Builds FROM: S.2d LOOKUP (runtime tables)
 * Builds TO:   S.3 CLOSING (file end)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2e_3_HEALTH S.2e.3 HEALTH TYPES — State, Severity, Trend, and more
 * @brief Health-related enumerations — Hebrew states, severity levels, trends.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  S.2e.3 HEALTH TYPES — Health Subsystem Enumerations (10 subsections)      │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  STRUCTURE:                                                                │
 * │    S.2e.3a STATE       — CpisiHealthState (7 Hebrew states)                │
 * │    S.2e.3b SEVERITY    — CpisiSeverity (5 alert levels)                    │
 * │    S.2e.3c TREND       — CpisiTrend (3 directions)                         │
 * │    S.2e.3d LAYER       — CpisiHealthLayer (9 layers, health context)       │
 * │    S.2e.3e MILESTONE   — CpisiMilestone (9 milestone indices)              │
 * │    S.2e.3f WEIGHT      — CpisiWeightValue (9 priority weights)             │
 * │    S.2e.3g HALF        — CpisiHalfLayer (9 half-cumulative indices)        │
 * │    S.2e.3h ZONE        — CpisiZone (5 health zone categories)              │
 * │    S.2e.3i THRESHOLD   — CpisiThresholdPosition (9 display positions)      │
 * │    S.2e.3j SEV_POSITION — CpisiSeverityPosition (9 severity indices)       │
 * │    S.2e.3k K_FACTOR    — CpisiKFactorPosition (3 direction indices)        │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  SEMANTIC FOUNDATION — Hebrew State Model:                                 │
 * │                                                                            │
 * │    SHAVAR (0)  — broken, crisis state                                      │
 * │    CHASER (1)  — lacking, deficient                                        │
 * │    RATSAH (2)  — wanting, hungry for growth                                │
 * │    YASHAR (3)  — even, balanced ← CENTER                                   │
 * │    TAMIM (4)   — sound, consolidated                                       │
 * │    SHALEM (5)  — whole, integrated                                         │
 * │    TOV (6)     — perfect, completion                                       │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.9):                                          │
 * │    - STATE tables: CPISI_HEALTH_*[7] (name, display, balanced, etc.)       │
 * │    - SEVERITY tables: CPISI_SEVERITY_*[5]                                  │
 * │    - TREND tables: CPISI_TREND_*[3]                                        │
 * │    - MILESTONE tables: 6 arrays × 9 entries each                           │
 * │    - WEIGHT/HALF tables: indexed by layer                                  │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup S2e_TYPES
 * @{ */

/**
 * @defgroup S2e_3a_STATE S.2e.3a CpisiHealthState — 7 Hebrew states
 * @brief Core health enumeration — 7 Hebrew cognitive/wellness states.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — 7 Hebrew State Words:                               │
 * │                                                                            │
 * │    שָׁבַר (shavar)  — to break (crisis)                                     │
 * │    חָסֵר (chaser)   — to lack (deficient)                                   │
 * │    רָצָה (ratsah)   — to want/desire (hungry for growth)                    │
 * │    יָשָׁר (yashar)   — to be straight/even (balanced) ← CENTER              │
 * │    תָּמִים (tamim)   — to be complete/sound (consolidated)                   │
 * │    שָׁלֵם (shalem)   — to be whole (integrated)                              │
 * │    טוֹב (tov)      — good/perfect (completion)                              │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.9a-c):                                       │
 * │    - CPISI_HEALTH_NAME[7] — English names                                  │
 * │    - CPISI_HEALTH_DISPLAY[7] — "-1.0", "-0.5", ..., "+1.0"                 │
 * │    - CPISI_HEALTH_BALANCED[7] — balanced ternary values                    │
 * │    - cpisi_health_*() accessors for runtime lookup                         │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup S2e_3_HEALTH
 * @{ */

/**
 * @brief Health state enumeration — 7 Hebrew cognitive states.
 *
 * BOUNDS USED:
 *   - CPISI_HEALTH_STATE_COUNT (7) — S.2a.1 TIER 1 health state cardinality
 *   - CPISI_HEALTH_STATE_CENTER (3) — S.2a.1 TIER 1 center = YASHAR
 *   - Direct integer values 0-6 (maps to Hebrew state words)
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for the 7 Hebrew cognitive/wellness states. This is the
 *   core health model — mapping system wellness to biblical concepts of
 *   brokenness through completion. YASHAR (3) at center = balanced/even.
 *
 * VALUE MAPPING:
 *   CPISI_HEALTH_SHAVAR (0) — שָׁבַר — broken, crisis state
 *   CPISI_HEALTH_CHASER (1) — חָסֵר — lacking, deficient
 *   CPISI_HEALTH_RATSAH (2) — רָצָה — wanting, hungry for growth
 *   CPISI_HEALTH_YASHAR (3) — יָשָׁר — even, balanced (CENTER)
 *   CPISI_HEALTH_TAMIM (4) — תָּמִים — sound, consolidated
 *   CPISI_HEALTH_SHALEM (5) — שָׁלֵם — whole, integrated
 *   CPISI_HEALTH_TOV (6) — טוֹב — perfect, completion
 *
 * USE CASE:
 *   - System health assessment: CpisiHealthState state = cpisi_assess_health();
 *   - State transitions: cpisi_transition(current, target);
 *   - Health scoring: score = cpisi_health_score(state);
 *   - Threshold checks: if (state < CPISI_HEALTH_YASHAR) alert();
 *
 * RELATIONSHIP TO LOOKUP (S.2d.9a-c):
 *   - cpisi_health_name(idx) → "SHAVAR", "CHASER", ..., "TOV"
 *   - cpisi_health_display(idx) → "-1.0", "-0.5", ..., "+1.0"
 *   - cpisi_health_balanced(idx) → -3, -2, -1, 0, +1, +2, +3 (centered)
 */
typedef enum CpisiHealthState {
    CPISI_HEALTH_SHAVAR = 0,  /**< 0: שָׁבַר — broken, crisis state */
    CPISI_HEALTH_CHASER = 1,  /**< 1: חָסֵר — lacking, deficient */
    CPISI_HEALTH_RATSAH = 2,  /**< 2: רָצָה — wanting, desiring growth */
    CPISI_HEALTH_YASHAR = 3,  /**< 3: יָשָׁר — even, balanced (CENTER) */
    CPISI_HEALTH_TAMIM  = 4,  /**< 4: תָּמִים — sound, consolidated */
    CPISI_HEALTH_SHALEM = 5,  /**< 5: שָׁלֵם — whole, integrated */
    CPISI_HEALTH_TOV    = 6   /**< 6: טוֹב — perfect, complete */
} CpisiHealthState;

/** @} */ /* End of S2e_3a_STATE */

/**
 * @defgroup S2e_3b_SEVERITY S.2e.3b CpisiSeverity — 5 alert levels
 * @brief Alert severity — NOMINAL(0) → CRITICAL(4), CAUTION(2) at CENTER.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — 5 Alert Levels (aerospace-inspired):                │
 * │                                                                            │
 * │    NOMINAL  (0) — all systems normal, no issues                            │
 * │    ADVISORY (1) — informational notice, monitor                            │
 * │    CAUTION  (2) — minor concern, attention needed ← CENTER                 │
 * │    WARNING  (3) — significant issue, action recommended                    │
 * │    CRITICAL (4) — immediate action required                                │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.9):                                          │
 * │    - CPISI_SEVERITY_NAME[5] — full names                                   │
 * │    - cpisi_severity_name() accessor                                        │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup S2e_3_HEALTH
 * @{ */

/**
 * @brief Severity enumeration — 5 alert levels (aerospace-inspired).
 *
 * BOUNDS USED:
 *   - CPISI_SEVERITY_COUNT (5) — S.2a.1 TIER 1 severity level cardinality
 *   - CPISI_SEVERITY_CENTER (2) — S.2a.1 TIER 1 center = CAUTION
 *   - Direct integer values 0-4 (standard aerospace alert pattern)
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for 5-level alert severity, inspired by aerospace cockpit
 *   alert conventions. CAUTION (2) at center provides balanced threshold.
 *   Distinct from SeverityRow (3-level for offset calculation).
 *
 * VALUE MAPPING:
 *   CPISI_SEV_NOMINAL (0) — all systems normal, no issues
 *   CPISI_SEV_ADVISORY (1) — informational notice, monitor
 *   CPISI_SEV_CAUTION (2) — minor concern, attention needed (CENTER)
 *   CPISI_SEV_WARNING (3) — significant issue, action recommended
 *   CPISI_SEV_CRITICAL (4) — immediate action required
 *
 * USE CASE:
 *   - Alert classification: CpisiSeverity sev = cpisi_classify_alert(error);
 *   - Threshold comparison: if (severity >= CPISI_SEV_WARNING) escalate();
 *   - Priority routing: route = cpisi_route_by_severity(sev);
 *
 * RELATIONSHIP TO LOOKUP (S.2d.9):
 *   - cpisi_severity_name(idx) → "NOMINAL", "ADVISORY", ..., "CRITICAL"
 */
typedef enum CpisiSeverity {
    CPISI_SEV_NOMINAL  = 0,  /**< 0: All systems normal — no issues */
    CPISI_SEV_ADVISORY = 1,  /**< 1: Informational notice — monitor */
    CPISI_SEV_CAUTION  = 2,  /**< 2: Minor concern — attention needed (CENTER) */
    CPISI_SEV_WARNING  = 3,  /**< 3: Significant issue — action recommended */
    CPISI_SEV_CRITICAL = 4   /**< 4: Immediate action required — emergency */
} CpisiSeverity;

/** @} */ /* End of S2e_3b_SEVERITY */

/**
 * @defgroup S2e_3c_TREND S.2e.3c CpisiTrend — Direction indicators
 * @brief Trend direction — DECLINING(-1), STABLE(0), IMPROVING(+1).
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — Balanced Ternary Direction:                         │
 * │                                                                            │
 * │    DECLINING (-1) — getting worse, moving toward crisis                    │
 * │    STABLE    ( 0) — holding steady, no change ← CENTER                     │
 * │    IMPROVING (+1) — getting better, moving toward completion               │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO SIGN:                                                     │
 * │    Trend is semantically identical to CpisiSign but applied to health.     │
 * │    Sign = mathematical direction; Trend = health trajectory direction.     │
 * │                                                                            │
 * │    TREND enum values use CPISI_NEGATIVE, CPISI_NEUTRAL, CPISI_POSITIVE     │
 * │    for type-safe assignment while maintaining semantic clarity.            │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup S2e_3_HEALTH
 * @{ */

/**
 * @brief Trend enumeration — health trajectory direction indicators.
 *
 * BOUNDS USED:
 *   - CPISI_NEGATIVE (-1), CPISI_NEUTRAL (0), CPISI_POSITIVE (+1) — S.2a.0 TIER 0
 *   - Semantically equivalent to CpisiSign but applied to health trajectories
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for health trajectory direction. Answers "which way is
 *   health moving?" — toward crisis (-1), stable (0), or toward completion (+1).
 *   Uses same underlying values as CpisiSign but carries health-specific meaning.
 *
 * VALUE MAPPING:
 *   CPISI_TRND_DECLINING = CPISI_NEGATIVE (-1) — getting worse, toward crisis
 *   CPISI_TRND_STABLE = CPISI_NEUTRAL (0) — holding steady, no change (CENTER)
 *   CPISI_TRND_IMPROVING = CPISI_POSITIVE (+1) — getting better, toward completion
 *
 * USE CASE:
 *   - Trajectory assessment: CpisiTrend trend = cpisi_calculate_trend(samples);
 *   - Decision logic: if (trend == CPISI_TRND_DECLINING) increase_monitoring();
 *   - Health forecasting: predicted_state = cpisi_forecast(state, trend, time);
 *
 * RELATIONSHIP TO SIGN:
 *   CpisiSign = mathematical direction (generic)
 *   CpisiTrend = health trajectory direction (domain-specific)
 *   Same values, different semantic context for type safety.
 */
typedef enum CpisiTrend {
    CPISI_TRND_DECLINING = CPISI_NEGATIVE,  /**< -1: Getting worse — toward crisis */
    CPISI_TRND_STABLE    = CPISI_NEUTRAL,   /**<  0: Holding steady — no change (CENTER) */
    CPISI_TRND_IMPROVING = CPISI_POSITIVE   /**< +1: Getting better — toward completion */
} CpisiTrend;

/** @} */ /* End of S2e_3c_TREND */

/**
 * @defgroup S2e_3d_LAYER S.2e.3d CpisiHealthLayer — 9 layers with health context
 * @brief Maps 9-layer pyramid to health scoring context.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — Layer-to-Health State Mapping:                      │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  Each layer position conceptually maps to a Hebrew state word:             │
 * │                                                                            │
 * │    Layer 0 (KERNEL)    → shavar (broken, foundational crisis)              │
 * │    Layer 1 (SYSTEM)    → chaser (lacking, system deficient)                │
 * │    Layer 2 (RUNTIME)   → ratsah (wanting, runtime hungry)                  │
 * │    Layer 3 (FRAMEWORK) → pre-yashar                                        │
 * │    Layer 4 (HEALTH)    → yashar (balanced) ← CENTER                        │
 * │    Layer 5 (NETWORK)   → post-yashar                                       │
 * │    Layer 6 (STORAGE)   → tamim (sound, storage consolidated)               │
 * │    Layer 7 (ENGINE)    → shalem (whole, engine integrated)                 │
 * │    Layer 8 (APP)       → pre-tov (approaching completion)                  │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  USE CASE:                                                                 │
 * │    CpisiHealthLayer provides type-safe layer parameter when calling        │
 * │    health-specific APIs that need layer context for scoring or routing.    │
 * │    Functionally equivalent to CpisiLayer but semantically scoped.          │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup S2e_3_HEALTH
 * @{ */

/**
 * @brief Health layer enumeration — 9 layers with Hebrew state context.
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_KERNEL (0) through CPISI_LAYER_APP (8) — S.2a.1 TIER 1
 *   - CPISI_LAYER_COUNT (9) — iteration bound
 *   - CPISI_LAYER_HEALTH (4) — CENTER of the 9-layer pyramid
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for the 9-layer architecture in health-specific contexts.
 *   Functionally equivalent to CpisiLayer but carries health-domain semantics.
 *   Each layer position maps to Hebrew state concepts for scoring context.
 *
 * VALUE MAPPING:
 *   CPISI_HEALTH_LAYER_ENUM_KERNEL (0) — KERNEL → shavar (foundational crisis)
 *   CPISI_HEALTH_LAYER_ENUM_SYSTEM (1) — SYSTEM → chaser (lacking)
 *   CPISI_HEALTH_LAYER_ENUM_RUNTIME (2) — RUNTIME → ratsah (wanting)
 *   CPISI_HEALTH_LAYER_ENUM_FRAMEWORK (3) — FRAMEWORK → pre-yashar
 *   CPISI_HEALTH_LAYER_ENUM_HEALTH (4) — HEALTH → yashar (CENTER)
 *   CPISI_HEALTH_LAYER_ENUM_NETWORK (5) — NETWORK → post-yashar
 *   CPISI_HEALTH_LAYER_ENUM_STORAGE (6) — STORAGE → tamim (sound)
 *   CPISI_HEALTH_LAYER_ENUM_ENGINE (7) — ENGINE → shalem (whole)
 *   CPISI_HEALTH_LAYER_ENUM_APP (8) — APP → pre-tov
 *   CPISI_HEALTH_LAYER_ENUM_COUNT (9) — iteration bound
 *
 * USE CASE:
 *   - Health-specific APIs: cpisi_health_score(CpisiHealthLayer layer);
 *   - Domain separation: health APIs use CpisiHealthLayer, generic use CpisiLayer
 *   - Type safety: prevents mixing health and generic layer parameters
 *
 * RELATIONSHIP TO CpisiLayer:
 *   Same underlying values (0-8), different type for domain separation.
 *   CpisiLayer = generic architecture layer
 *   CpisiHealthLayer = health-domain layer with Hebrew state semantics
 */
typedef enum CpisiHealthLayer {
    CPISI_HEALTH_LAYER_ENUM_KERNEL    = CPISI_LAYER_KERNEL,    /**< 0: KERNEL → shavar (foundational crisis) */
    CPISI_HEALTH_LAYER_ENUM_SYSTEM    = CPISI_LAYER_SYSTEM,    /**< 1: SYSTEM → chaser (lacking) */
    CPISI_HEALTH_LAYER_ENUM_RUNTIME   = CPISI_LAYER_RUNTIME,   /**< 2: RUNTIME → ratsah (wanting) */
    CPISI_HEALTH_LAYER_ENUM_FRAMEWORK = CPISI_LAYER_FRAMEWORK, /**< 3: FRAMEWORK → pre-yashar */
    CPISI_HEALTH_LAYER_ENUM_HEALTH    = CPISI_LAYER_HEALTH,    /**< 4: HEALTH → yashar (CENTER) */
    CPISI_HEALTH_LAYER_ENUM_NETWORK   = CPISI_LAYER_NETWORK,   /**< 5: NETWORK → post-yashar */
    CPISI_HEALTH_LAYER_ENUM_STORAGE   = CPISI_LAYER_STORAGE,   /**< 6: STORAGE → tamim (sound) */
    CPISI_HEALTH_LAYER_ENUM_ENGINE    = CPISI_LAYER_ENGINE,    /**< 7: ENGINE → shalem (whole) */
    CPISI_HEALTH_LAYER_ENUM_APP       = CPISI_LAYER_APP,       /**< 8: APP → pre-tov */
    CPISI_HEALTH_LAYER_ENUM_COUNT     = CPISI_LAYER_COUNT      /**< 9: Iteration bound */
} CpisiHealthLayer;

/** @} */ /* End of S2e_3d_LAYER */

/**
 * @defgroup S2e_3e_MILESTONE S.2e.3e CpisiMilestone — 9 milestone indices
 * @brief Type-safe milestone index — 9 Hebrew state progression markers.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — 9 Milestone Progression:                            │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  Milestones mark progression through the Hebrew state journey:             │
 * │                                                                            │
 * │    0: SHAVAR  — starting point (broken, crisis)                            │
 * │    1: MACHSOR — deprivation milestone                                      │
 * │    2: CHASER  — lacking milestone                                          │
 * │    3: RATSAH  — wanting milestone                                          │
 * │    4: YASHAR  — balanced milestone ← CENTER                                │
 * │    5: RATSON  — willing milestone                                          │
 * │    6: SHALEM  — wholeness milestone                                        │
 * │    7: TAMIM   — soundness milestone                                        │
 * │    8: TOV     — completion milestone (goal)                                │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.9d):                                         │
 * │    - 6 MILESTONE tables × 9 entries: display, balanced, ternary,           │
 * │      center, name, abbrev                                                  │
 * │    - cpisi_milestone_*() accessors for runtime lookup                      │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup S2e_3_HEALTH
 * @{ */

/**
 * @brief Milestone index enumeration — 9 Hebrew state milestones.
 *
 * BOUNDS USED:
 *   - CPISI_FACE (9) — S.2a.1 TIER 1, cube face = 3^2
 *   - Direct integer values 0-8 for milestone indices
 *   - YASHAR (4) at center of 9 milestones
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for the 9 Hebrew state progression markers. Unlike the
 *   7-state CpisiHealthState which maps health levels, milestones mark the
 *   9-step journey from broken (0) to perfect (8). Maps 1:1 with layer count.
 *
 * VALUE MAPPING:
 *   CPISI_MILESTONE_ENUM_SHAVAR (0) — שָׁבַר — broken, crisis start
 *   CPISI_MILESTONE_ENUM_MACHSOR (1) — מַחְסוֹר — deprived, lacking resources
 *   CPISI_MILESTONE_ENUM_CHASER (2) — חָסֵר — lacking, deficient
 *   CPISI_MILESTONE_ENUM_RATSAH (3) — רָצָה — wanting, desiring growth
 *   CPISI_MILESTONE_ENUM_YASHAR (4) — יָשָׁר — even, balanced (CENTER)
 *   CPISI_MILESTONE_ENUM_RATSON (5) — רָצוֹן — willing, favor
 *   CPISI_MILESTONE_ENUM_SHALEM (6) — שָׁלֵם — whole, integrated
 *   CPISI_MILESTONE_ENUM_TAMIM (7) — תָּמִים — sound, consolidated
 *   CPISI_MILESTONE_ENUM_TOV (8) — טוֹב — perfect, completion goal
 *   CPISI_MILESTONE_ENUM_COUNT (9) — iteration bound = CPISI_FACE
 *
 * USE CASE:
 *   - Progress tracking: cpisi_mark_milestone(CpisiMilestone ms);
 *   - Journey visualization: render milestone progress bar
 *   - State lookup: name = cpisi_milestone_name(milestone);
 *
 * RELATIONSHIP TO LOOKUP (S.2d.9d):
 *   - cpisi_milestone_display(idx) → "-4", "-3", ..., "+4"
 *   - cpisi_milestone_name(idx) → "SHAVAR", "MACHSOR", ..., "TOV"
 *   - cpisi_milestone_balanced(idx) → -4, -3, ..., +4 (centered on YASHAR)
 */
typedef enum CpisiMilestone {
    CPISI_MILESTONE_ENUM_SHAVAR  = 0,  /**< 0: שָׁבַר — broken, crisis start */
    CPISI_MILESTONE_ENUM_MACHSOR = 1,  /**< 1: מַחְסוֹר — deprived, lacking resources */
    CPISI_MILESTONE_ENUM_CHASER  = 2,  /**< 2: חָסֵר — lacking, deficient */
    CPISI_MILESTONE_ENUM_RATSAH  = 3,  /**< 3: רָצָה — wanting, desiring growth */
    CPISI_MILESTONE_ENUM_YASHAR  = 4,  /**< 4: יָשָׁר — even, balanced (CENTER) */
    CPISI_MILESTONE_ENUM_RATSON  = 5,  /**< 5: רָצוֹן — willing, favor */
    CPISI_MILESTONE_ENUM_SHALEM  = 6,  /**< 6: שָׁלֵם — whole, integrated */
    CPISI_MILESTONE_ENUM_TAMIM   = 7,  /**< 7: תָּמִים — sound, consolidated */
    CPISI_MILESTONE_ENUM_TOV     = 8,  /**< 8: טוֹב — perfect, completion goal */
    CPISI_MILESTONE_ENUM_COUNT   = CPISI_FACE  /**< 9: Iteration bound */
} CpisiMilestone;

/** @} */ /* End of S2e_3e_MILESTONE */

/**
 * @defgroup S2e_3f_WEIGHT S.2e.3f CpisiWeightValue — 9 weight values
 * @brief Type-safe weight enumeration — layer priority scoring (1-9).
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — Inverted Layer Weights:                             │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  Weight values run OPPOSITE to layer indices:                              │
 * │    KERNEL (layer 0) has highest weight (9) — most critical                 │
 * │    APP (layer 8) has lowest weight (1) — least critical                    │
 * │                                                                            │
 * │  Formula: weight = 9 - layer                                               │
 * │                                                                            │
 * │    Layer → Weight                                                          │
 * │    APP (8)       → 1  (least critical)                                     │
 * │    ENGINE (7)    → 2                                                       │
 * │    STORAGE (6)   → 3                                                       │
 * │    NETWORK (5)   → 4                                                       │
 * │    HEALTH (4)    → 5  ← CENTER                                             │
 * │    FRAMEWORK (3) → 6                                                       │
 * │    RUNTIME (2)   → 7                                                       │
 * │    SYSTEM (1)    → 8                                                       │
 * │    KERNEL (0)    → 9  (most critical)                                      │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.9e):                                         │
 * │    - CPISI_WEIGHT_*[9] tables indexed by layer                             │
 * │    - cpisi_weight_*() accessors for layer → weight conversion              │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup S2e_3_HEALTH
 * @{ */

/**
 * @brief Weight value enumeration — layer priority scoring (1-9).
 *
 * BOUNDS USED:
 *   - CPISI_FACE (9) — S.2a.1 TIER 1, iteration bound
 *   - Direct integer values 1-9 (weight values, NOT indices)
 *   - HEALTH weight (5) at center
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for layer priority weights. Values run OPPOSITE to layer
 *   indices: KERNEL (layer 0) has highest weight (9), APP (layer 8) has lowest
 *   weight (1). Formula: weight = 9 - layer. Used for health scoring priority.
 *
 * VALUE MAPPING:
 *   CPISI_WEIGHT_ENUM_APP (1) — APP layer weight (least critical)
 *   CPISI_WEIGHT_ENUM_ENGINE (2) — ENGINE layer weight
 *   CPISI_WEIGHT_ENUM_STORAGE (3) — STORAGE layer weight
 *   CPISI_WEIGHT_ENUM_NETWORK (4) — NETWORK layer weight
 *   CPISI_WEIGHT_ENUM_HEALTH (5) — HEALTH layer weight (CENTER)
 *   CPISI_WEIGHT_ENUM_FRAMEWORK (6) — FRAMEWORK layer weight
 *   CPISI_WEIGHT_ENUM_RUNTIME (7) — RUNTIME layer weight
 *   CPISI_WEIGHT_ENUM_SYSTEM (8) — SYSTEM layer weight
 *   CPISI_WEIGHT_ENUM_KERNEL (9) — KERNEL layer weight (most critical)
 *   CPISI_WEIGHT_ENUM_COUNT (9) — iteration bound = CPISI_FACE
 *
 * USE CASE:
 *   - Priority scoring: total_score += error_count * weight_value;
 *   - Criticality ordering: KERNEL errors weighted 9× APP errors
 *   - Health calculations: weighted aggregation across layers
 *
 * RELATIONSHIP TO LOOKUP (S.2d.9e):
 *   - cpisi_weight_value(layer) → weight for that layer
 *   - cpisi_weight_name(layer) → layer name at that weight position
 *   - CPISI_WEIGHT_VALUE[9] — precomputed weight table indexed by layer
 */
typedef enum CpisiWeightValue {
    CPISI_WEIGHT_ENUM_APP       = 1,  /**< Weight 1: APP layer (least critical) */
    CPISI_WEIGHT_ENUM_ENGINE    = 2,  /**< Weight 2: ENGINE layer */
    CPISI_WEIGHT_ENUM_STORAGE   = 3,  /**< Weight 3: STORAGE layer */
    CPISI_WEIGHT_ENUM_NETWORK   = 4,  /**< Weight 4: NETWORK layer */
    CPISI_WEIGHT_ENUM_HEALTH    = 5,  /**< Weight 5: HEALTH layer (CENTER) */
    CPISI_WEIGHT_ENUM_FRAMEWORK = 6,  /**< Weight 6: FRAMEWORK layer */
    CPISI_WEIGHT_ENUM_RUNTIME   = 7,  /**< Weight 7: RUNTIME layer */
    CPISI_WEIGHT_ENUM_SYSTEM    = 8,  /**< Weight 8: SYSTEM layer */
    CPISI_WEIGHT_ENUM_KERNEL    = 9,  /**< Weight 9: KERNEL layer (most critical) */
    CPISI_WEIGHT_ENUM_COUNT     = CPISI_FACE  /**< 9: Iteration bound */
} CpisiWeightValue;

/** @} */ /* End of S2e_3f_WEIGHT */

/**
 * @defgroup S2e_3g_HALF S.2e.3g CpisiHalfLayer — 9 half-cumulative indices
 * @brief Type-safe half-cumulative index — (3^layer - 1) / 2 for each layer.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — Half-Cumulative Formula:                            │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  Formula: Half(layer) = (3^layer - 1) / 2                                  │
 * │                                                                            │
 * │  This represents the count of all smaller ternary positions:               │
 * │    Layer 0: (3^0 - 1)/2 = (1-1)/2   = 0                                    │
 * │    Layer 1: (3^1 - 1)/2 = (3-1)/2   = 1                                    │
 * │    Layer 2: (3^2 - 1)/2 = (9-1)/2   = 4                                    │
 * │    Layer 3: (3^3 - 1)/2 = (27-1)/2  = 13                                   │
 * │    Layer 4: (3^4 - 1)/2 = (81-1)/2  = 40   ← CENTER                        │
 * │    Layer 5: (3^5 - 1)/2 = (243-1)/2 = 121                                  │
 * │    Layer 6: (3^6 - 1)/2 = (729-1)/2 = 364                                  │
 * │    Layer 7: (3^7 - 1)/2 = (2187-1)/2 = 1093                                │
 * │    Layer 8: (3^8 - 1)/2 = (6561-1)/2 = 3280                                │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  USE CASE:                                                                 │
 * │    Used for balanced ternary address calculations where you need the       │
 * │    midpoint of a layer's range for centering operations.                   │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.9f):                                         │
 * │    - CPISI_HALF_*[9] tables precompute these values                        │
 * │    - cpisi_half_*() accessors for layer → half lookup                      │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * Will split to types-health.h
 *
 * @ingroup S2e_3_HEALTH
 * @{ */

/**
 * @brief Half-cumulative layer enumeration — indices into half table.
 *
 * BOUNDS USED:
 *   - CPISI_FACE (9) — S.2a.1 TIER 1, iteration bound
 *   - Direct integer values 0-8 as layer indices
 *   - HEALTH (4) at center of 9 layers
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for indices into half-cumulative lookup tables. The half
 *   formula (3^layer - 1) / 2 gives the count of all smaller ternary positions,
 *   used for balanced address calculations. Enum provides type-safe layer index.
 *
 * VALUE MAPPING (enum value → half-cumulative result):
 *   CPISI_HALF_ENUM_KERNEL (0) → (3^0-1)/2 = 0
 *   CPISI_HALF_ENUM_SYSTEM (1) → (3^1-1)/2 = 1
 *   CPISI_HALF_ENUM_RUNTIME (2) → (3^2-1)/2 = 4
 *   CPISI_HALF_ENUM_FRAMEWORK (3) → (3^3-1)/2 = 13
 *   CPISI_HALF_ENUM_HEALTH (4) → (3^4-1)/2 = 40 (CENTER)
 *   CPISI_HALF_ENUM_NETWORK (5) → (3^5-1)/2 = 121
 *   CPISI_HALF_ENUM_STORAGE (6) → (3^6-1)/2 = 364
 *   CPISI_HALF_ENUM_ENGINE (7) → (3^7-1)/2 = 1093
 *   CPISI_HALF_ENUM_APP (8) → (3^8-1)/2 = 3280
 *   CPISI_HALF_ENUM_COUNT (9) — iteration bound = CPISI_FACE
 *
 * USE CASE:
 *   - Address centering: center_addr = cpisi_half_value(layer);
 *   - Range calculations: midpoint of layer's ternary address space
 *   - Balanced ternary arithmetic: offset calculations
 *
 * RELATIONSHIP TO LOOKUP (S.2d.9f):
 *   - cpisi_half_value(layer) → precomputed (3^layer - 1) / 2
 *   - CPISI_HALF_VALUE[9] — half-cumulative table indexed by layer
 */
typedef enum CpisiHalfLayer {
    CPISI_HALF_ENUM_KERNEL    = 0,  /**< Layer 0: (3^0-1)/2 = 0 — KERNEL half */
    CPISI_HALF_ENUM_SYSTEM    = 1,  /**< Layer 1: (3^1-1)/2 = 1 — SYSTEM half */
    CPISI_HALF_ENUM_RUNTIME   = 2,  /**< Layer 2: (3^2-1)/2 = 4 — RUNTIME half */
    CPISI_HALF_ENUM_FRAMEWORK = 3,  /**< Layer 3: (3^3-1)/2 = 13 — FRAMEWORK half */
    CPISI_HALF_ENUM_HEALTH    = 4,  /**< Layer 4: (3^4-1)/2 = 40 — HEALTH half (CENTER) */
    CPISI_HALF_ENUM_NETWORK   = 5,  /**< Layer 5: (3^5-1)/2 = 121 — NETWORK half */
    CPISI_HALF_ENUM_STORAGE   = 6,  /**< Layer 6: (3^6-1)/2 = 364 — STORAGE half */
    CPISI_HALF_ENUM_ENGINE    = 7,  /**< Layer 7: (3^7-1)/2 = 1093 — ENGINE half */
    CPISI_HALF_ENUM_APP       = 8,  /**< Layer 8: (3^8-1)/2 = 3280 — APP half */
    CPISI_HALF_ENUM_COUNT     = CPISI_FACE  /**< 9: Iteration bound */
} CpisiHalfLayer;

/** @} */ /* End of S2e_3g_HALF */

/**
 * @defgroup S2e_3h_ZONE S.2e.3h CpisiZone — 5 health zone indices
 * @ingroup S2e_3_HEALTH
 * @brief Type-safe enumeration for 5 health zone classifications.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  S.2e.3h ZONE — Zone Index Enumeration (5 values + COUNT)                  │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  ZONE MAPPING (5 zones → 7 Hebrew states → 9 layers):                      │
 * │                                                                            │
 * │    CRITICAL (0) — severe health issues (shavar zone)                       │
 * │    DEGRADED (1) — noticeable problems (chaser/ratsah zone)                 │
 * │    BALANCED (2) — CENTER/yashar (default equilibrium)                      │
 * │    HEALTHY  (3) — good condition (tamim/shalem zone)                       │
 * │    OPTIMAL  (4) — peak health (tov zone)                                   │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.9g):                                         │
 * │    - cpisi_zone_name(idx) → "CRITICAL"..."OPTIMAL"                         │
 * │    - cpisi_zone_abbrev(idx) → 'C', 'D', 'B', 'H', 'O'                       │
 * │    - cpisi_zone_center(idx) → 2 (all zones reference BALANCED)             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup S2e_TYPES
 * @{ */

/**
 * @brief Zone index enumeration — type-safe zone classifications.
 *
 * BOUNDS USED:
 *   - CPISI_ZONE_MIN = 0 (S.2a.5b.2f) — CRITICAL zone
 *   - CPISI_ZONE_CENTER = 2 (S.2a.5b.2f) — BALANCED zone
 *   - CPISI_ZONE_MAX = 4 (S.2a.5b.2f) — OPTIMAL zone
 *   - CPISI_ZONE_COUNT = 5 (S.2a.5b.2f) — total zones
 *   - CPISI_HARDPOINTS = 5 (S.2a.1d) — equivalent count
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for zone indices [0-4]. Provides compiler-checked
 *   zone parameters instead of raw integers. Maps health values to
 *   categorical classifications via 4 boundaries at ±25 and ±50.
 *
 * VALUE MAPPING:
 *   CPISI_ZONE_ENUM_CRITICAL (0) — CRITICAL zone, severe health issues
 *   CPISI_ZONE_ENUM_DEGRADED (1) — DEGRADED zone, noticeable problems
 *   CPISI_ZONE_ENUM_BALANCED (2) — BALANCED zone (CENTER), equilibrium
 *   CPISI_ZONE_ENUM_HEALTHY  (3) — HEALTHY zone, good condition
 *   CPISI_ZONE_ENUM_OPTIMAL  (4) — OPTIMAL zone, peak health
 *   CPISI_ZONE_ENUM_COUNT    (5) — iteration bound
 *
 * USE CASE:
 *   - Function parameters: void cpisi_set_zone(CpisiZone zone);
 *   - Switch dispatch: switch(zone) { case CPISI_ZONE_ENUM_CRITICAL: ... }
 *   - Loop iteration: for (CpisiZone z = 0; z < CPISI_ZONE_ENUM_COUNT; z++)
 *   - Type-safe assignment: CpisiZone current = CPISI_ZONE_ENUM_BALANCED;
 *
 * RELATIONSHIP TO LOOKUP (S.2d.9g):
 *   - cpisi_zone_name(idx) → "CRITICAL", "DEGRADED", "BALANCED", "HEALTHY", "OPTIMAL"
 *   - cpisi_zone_abbrev(idx) → 'C', 'D', 'B', 'H', 'O'
 *   - cpisi_zone_center(idx) → 2 (all zones)
 *   - cpisi_zone_boundary(idx) → -50, -25, +25, +50 (boundary thresholds)
 *
 * RELATIONSHIP TO MACROS (S.2b.4j):
 *   - CPISI_IS_ZONE(z) validates zone index
 *   - CPISI_IS_ZONE_CENTER(z) checks if BALANCED
 *   - CPISI_IS_ZONE_EDGE(z) checks if CRITICAL or OPTIMAL
 */
typedef enum CpisiZone {
    /* S.2e.3h.1 VALUES — Zone indices (5) */
    CPISI_ZONE_ENUM_CRITICAL  = 0,  /**< Zone 0: CRITICAL (shavar) */
    CPISI_ZONE_ENUM_DEGRADED  = 1,  /**< Zone 1: DEGRADED (chaser) */
    CPISI_ZONE_ENUM_BALANCED  = 2,  /**< Zone 2: BALANCED (yashar) — CENTER */
    CPISI_ZONE_ENUM_HEALTHY   = 3,  /**< Zone 3: HEALTHY (shalem) */
    CPISI_ZONE_ENUM_OPTIMAL   = 4,  /**< Zone 4: OPTIMAL (tov) */

    /* S.2e.3h.2 COUNT — Iteration bound */
    CPISI_ZONE_ENUM_COUNT     = CPISI_HARDPOINTS  /**< Total zones (5) */
} CpisiZone;

/** @} */ /* End of S2e_3h_ZONE */

/**
 * @defgroup S2e_3i_THRESHOLD S.2e.3i CpisiThresholdPosition — 9 threshold positions
 * @brief Type-safe threshold position enumeration for milestone display contexts.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — 9 Threshold Display Positions:                      │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  Threshold positions provide type-safe indexing for display-context APIs.  │
 * │  Same 9 positions as CpisiMilestone but with THRESHOLD_ENUM_ prefix to     │
 * │  avoid collision in APIs that need distinct types for milestone index      │
 * │  vs threshold display context.                                             │
 * │                                                                            │
 * │    0: SHAVAR  — (-100) broken, crisis edge                                 │
 * │    1: MACHSOR — (-75)  severe lack                                         │
 * │    2: CHASER  — (-50)  lacking                                             │
 * │    3: RATSAH  — (-25)  wanting                                             │
 * │    4: YASHAR  — (0)    even, center                                        │
 * │    5: RATSON  — (+25)  willing                                             │
 * │    6: SHALEM  — (+50)  whole                                               │
 * │    7: TAMIM   — (+75)  sound                                               │
 * │    8: TOV     — (+100) perfect, completion edge                            │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  WHY SEPARATE FROM CpisiMilestone:                                         │
 * │    - CpisiMilestone indices (-4..+4 semantic values)                       │
 * │    - CpisiThresholdPosition for display API contexts (0..8 indices)        │
 * │    - Type safety prevents mixing milestone calculations with display lookup│
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup S2e_3_HEALTH
 * @{ */

/**
 * @brief Threshold position enumeration — 9 positions for display-context APIs.
 *
 * BOUNDS USED:
 *   - CPISI_FACE (9) — S.2a.1 TIER 1, array size for threshold tables
 *   - Direct integer values 0-8 for threshold position indices
 *   - YASHAR (4) at center matching CpisiMilestone center
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for threshold POSITIONS used in display-context APIs.
 *   Uses THRESHOLD_ENUM_ prefix to distinguish from CpisiMilestone which uses
 *   MILESTONE_ENUM_ prefix. Both reference the same 9 Hebrew states but in
 *   different API contexts:
 *   - CpisiMilestone: milestone index arithmetic (-4..+4 balanced)
 *   - CpisiThresholdPosition: display lookup (0..8 array index)
 *
 * VALUE MAPPING:
 *   CPISI_THRESHOLD_ENUM_SHAVAR (0) — position 0: shavar (-100, broken)
 *   CPISI_THRESHOLD_ENUM_MACHSOR (1) — position 1: machsor (-75, severe lack)
 *   CPISI_THRESHOLD_ENUM_CHASER (2) — position 2: chaser (-50, lacking)
 *   CPISI_THRESHOLD_ENUM_RATSAH (3) — position 3: ratsah (-25, wanting)
 *   CPISI_THRESHOLD_ENUM_YASHAR (4) — position 4: yashar (0, even, CENTER)
 *   CPISI_THRESHOLD_ENUM_RATSON (5) — position 5: ratson (+25, willing)
 *   CPISI_THRESHOLD_ENUM_SHALEM (6) — position 6: shalem (+50, whole)
 *   CPISI_THRESHOLD_ENUM_TAMIM (7) — position 7: tamim (+75, sound)
 *   CPISI_THRESHOLD_ENUM_TOV (8) — position 8: tov (+100, perfect)
 *   CPISI_THRESHOLD_ENUM_COUNT (9) — iteration bound = CPISI_FACE
 *
 * USE CASE:
 *   - Threshold display lookup: display = cpisi_milestone_threshold(pos);
 *   - Position-based iteration: for (pos = 0; pos < THRESHOLD_ENUM_COUNT; pos++)
 *   - Type-safe API signatures: void render_threshold(CpisiThresholdPosition pos);
 *
 * RELATIONSHIP TO:
 *   - CpisiMilestone (S.2e.3e): Same positions, different prefix for type safety
 *   - S.2d.9d MILESTONE TABLES: Indexed by this type
 *   - MILESTONE_DISPLAY_* bounds: Display values at each position
 */
typedef enum CpisiThresholdPosition {
    /* S.2e.3i.1 VALUES — Threshold positions (9) */
    CPISI_THRESHOLD_ENUM_SHAVAR   = 0,  /**< Position 0: shavar (-100, broken) */
    CPISI_THRESHOLD_ENUM_MACHSOR  = 1,  /**< Position 1: machsor (-75, severe lack) */
    CPISI_THRESHOLD_ENUM_CHASER   = 2,  /**< Position 2: chaser (-50, lacking) */
    CPISI_THRESHOLD_ENUM_RATSAH   = 3,  /**< Position 3: ratsah (-25, wanting) */
    CPISI_THRESHOLD_ENUM_YASHAR   = 4,  /**< Position 4: yashar (0, even, CENTER) */
    CPISI_THRESHOLD_ENUM_RATSON   = 5,  /**< Position 5: ratson (+25, willing) */
    CPISI_THRESHOLD_ENUM_SHALEM   = 6,  /**< Position 6: shalem (+50, whole) */
    CPISI_THRESHOLD_ENUM_TAMIM    = 7,  /**< Position 7: tamim (+75, sound) */
    CPISI_THRESHOLD_ENUM_TOV      = 8,  /**< Position 8: tov (+100, perfect) */

    /* S.2e.3i.2 COUNT — Iteration bound */
    CPISI_THRESHOLD_ENUM_COUNT    = CPISI_FACE  /**< Total threshold positions (9) */
} CpisiThresholdPosition;

/** @} */ /* End of S2e_3i_THRESHOLD */

/**
 * @defgroup S2e_3j_SEV_POS S.2e.3j SEVERITY POSITION — CpisiSeverityPosition
 * @ingroup S2e_3_HEALTH
 * @brief Type-safe severity position enum for 9-level indexing.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  S.2e.3j SEVERITY POSITION — 9-Level Impact/Severity Index (9+1 values)    │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  THE 9 SEVERITY POSITIONS (4+1+4 pattern):                                 │
 * │                                                                            │
 * │    FATAL     (0) — severity -4, impact -100, shavar edge                   │
 * │    HIGH      (1) — severity -3, impact -75                                 │
 * │    MEDIUM    (2) — severity -2, impact -50                                 │
 * │    LOW       (3) — severity -1, impact -25                                 │
 * │    NONE      (4) — severity  0, impact   0 ← CENTER (yashar)               │
 * │    FAIR      (5) — severity +1, impact +25                                 │
 * │    GOOD      (6) — severity +2, impact +50                                 │
 * │    EXCELLENT (7) — severity +3, impact +75                                 │
 * │    PERFECT   (8) — severity +4, impact +100, tov edge                      │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  NOTE: Uses SEV_POS_ prefix to distinguish from CpisiSeverity (5-level).   │
 * │  CpisiSeverity (5 alert levels) vs CpisiSeverityPosition (9 indices).      │
 * └─────────────────────────────────────────────────────────────────────────────┘
 * @{ */

/**
 * @brief Severity position enum for type-safe 9-level indexing.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (S.2a.3 TIER 1) — total positions (array size)
 *   - CPISI_HEALTH_SEVERITY_MIN = -4 (S.2a.6) — maps to position 0
 *   - CPISI_HEALTH_SEVERITY_MAX = +4 (S.2a.6) — maps to position 8
 *   - CPISI_HEALTH_SEVERITY_CENTER = 0 (S.2a.6) — maps to position 4
 *
 * SEMANTIC MEANING:
 *   Type-safe enumeration for indexing 9-level severity arrays.
 *   Values 0-8 correspond to the 9 severity levels in the 4+1+4 pattern.
 *   Center position (NONE) represents yashar — even, balanced state.
 *
 * VALUE MAPPING:
 *   CPISI_SEV_POS_FATAL     (0) — severity -4, FATAL
 *   CPISI_SEV_POS_HIGH      (1) — severity -3, HIGH
 *   CPISI_SEV_POS_MEDIUM    (2) — severity -2, MEDIUM
 *   CPISI_SEV_POS_LOW       (3) — severity -1, LOW
 *   CPISI_SEV_POS_NONE      (4) — severity  0, NONE (center/yashar)
 *   CPISI_SEV_POS_FAIR      (5) — severity +1, FAIR
 *   CPISI_SEV_POS_GOOD      (6) — severity +2, GOOD
 *   CPISI_SEV_POS_EXCELLENT (7) — severity +3, EXCELLENT
 *   CPISI_SEV_POS_PERFECT   (8) — severity +4, PERFECT
 *   CPISI_SEV_POS_COUNT     (9) — total positions (FACE)
 *
 * USE CASE:
 *   Type-safe array indexing for severity/impact lookup tables.
 *   Provides compile-time safety for 9-position arrays.
 *
 * RELATIONSHIP TO:
 *   - S.2d.9h IMPACT tables — indices match table positions
 *   - CpisiSeverity (5-level) — this is 9-level, that is 5-level
 *   - CpisiThresholdPosition — similar structure, different semantic domain
 *
 * @see cpisi_health_severity
 * @see g_cpisi_health_severity_name
 * @see CPISI_HEALTH_SEVERITY_COUNT
 */
typedef enum CpisiSeverityPosition {
    /* S.2e.3j.1 POSITION VALUES — FATAL(0)..PERFECT(8) */
    CPISI_SEV_POS_FATAL     = 0,  /**< Position 0: FATAL (severity -4, impact -100) */
    CPISI_SEV_POS_HIGH      = 1,  /**< Position 1: HIGH (severity -3, impact -75) */
    CPISI_SEV_POS_MEDIUM    = 2,  /**< Position 2: MEDIUM (severity -2, impact -50) */
    CPISI_SEV_POS_LOW       = 3,  /**< Position 3: LOW (severity -1, impact -25) */
    CPISI_SEV_POS_NONE      = 4,  /**< Position 4: NONE (severity 0, impact 0) — CENTER */
    CPISI_SEV_POS_FAIR      = 5,  /**< Position 5: FAIR (severity +1, impact +25) */
    CPISI_SEV_POS_GOOD      = 6,  /**< Position 6: GOOD (severity +2, impact +50) */
    CPISI_SEV_POS_EXCELLENT = 7,  /**< Position 7: EXCELLENT (severity +3, impact +75) */
    CPISI_SEV_POS_PERFECT   = 8,  /**< Position 8: PERFECT (severity +4, impact +100) */

    /* S.2e.3j.2 COUNT — Iteration bound */
    CPISI_SEV_POS_COUNT     = CPISI_FACE  /**< Total severity positions (9) */
} CpisiSeverityPosition;

/** @} */ /* End of S2e_3j_SEV_POS */

/**
 * @defgroup S2e_3k_K_FACTOR S.2e.3k K_FACTOR POSITION — CpisiKFactorPosition
 * @brief K-factor direction enumeration — 3 directional indices for normalization.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  S.2e.3k K_FACTOR — Direction Index Enumeration (3+1 values)               │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  SEMANTIC FOUNDATION — Directional Choice from Yashar:                     │
 * │                                                                            │
 * │    RETREAT  (0) — k=-1, toward shavar (128 corruption steps)               │
 * │    MAINTAIN (1) — k= 0, at yashar anchor (1 position)                      │
 * │    ADVANCE  (2) — k=+1, toward tov (127 restoration steps)                 │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  ASYMMETRY PRINCIPLE:                                                      │
 * │    More paths to corruption than restoration.                              │
 * │    "Many ways to fall, narrow path to life."                               │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.9i):                                         │
 * │    - g_cpisi_k_factor[3]: k-factor values indexed by position              │
 * │    - g_cpisi_k_name[3]: direction names indexed by position                │
 * │    - g_cpisi_k_sign[3]: sign characters indexed by position                │
 * │    - g_cpisi_positions[3]: step counts indexed by position                 │
 * │    - g_cpisi_divisor[3]: normalization divisors indexed by position        │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @{
 */

/**
 * @brief K-factor position enum for type-safe array indexing.
 *
 * BOUNDS USED:
 *   - CPISI_TERNARY (3) — total k-factor positions
 *
 * SEMANTIC MEANING:
 *   Maps semantic direction names to array indices for k-factor lookups.
 *   Enables compile-time verification and switch exhaustiveness checking.
 *
 *   RETREAT  — Moving away from yashar toward shavar (corruption)
 *   MAINTAIN — Holding steady at yashar (stability)
 *   ADVANCE  — Moving toward tov (restoration)
 *
 * VALUE MAPPING:
 *   CPISI_K_ENUM_RETREAT  = 0 — Index for retreat/shavar direction (k=-1)
 *   CPISI_K_ENUM_MAINTAIN = 1 — Index for maintain/yashar anchor (k=0)
 *   CPISI_K_ENUM_ADVANCE  = 2 — Index for advance/tov direction (k=+1)
 *   CPISI_K_ENUM_COUNT    = 3 — Total positions (equals TERNARY)
 *
 * USE CASE:
 *   Type-safe indexing into normalization arrays.
 *   Switch statements with exhaustiveness checking.
 *
 * RELATIONSHIP TO:
 *   - g_cpisi_k_factor[]: value array indexed by this enum
 *   - g_cpisi_k_name[]: name array indexed by this enum
 *   - g_cpisi_positions[]: step count array indexed by this enum
 *   - g_cpisi_divisor[]: divisor array indexed by this enum
 */
typedef enum CpisiKFactorPosition {
    /* S.2e.3k.1 POSITION VALUES — RETREAT(0), MAINTAIN(1), ADVANCE(2) */
    CPISI_K_ENUM_RETREAT  = 0,  /**< Position 0: retreat (k=-1, shavar direction) */
    CPISI_K_ENUM_MAINTAIN = 1,  /**< Position 1: maintain (k=0, yashar anchor) */
    CPISI_K_ENUM_ADVANCE  = 2,  /**< Position 2: advance (k=+1, tov direction) */

    /* S.2e.3k.2 COUNT — Iteration bound */
    CPISI_K_ENUM_COUNT    = CPISI_TERNARY  /**< Total k-factor positions (3) */
} CpisiKFactorPosition;

/** @} */ /* end S.2e.3k K_FACTOR */
/** @} */ /* end S.2e.3 HEALTH */

/** @} */ /* end S.2e TYPES */

/** @} */ /* end S.2 DEFINES */

/** S.3 TYPES — [Reserved: health types in S.2e] @defgroup cpisi_health_s3_types @ingroup cpisi_health_domain @{ @} */

/** S.4 PROTOTYPES — [Reserved: health accessors inline in S.2d/BODY] @defgroup cpisi_health_prototypes @ingroup cpisi_health_domain @{ @} */

/** S.5 FILE-LEVEL — [Reserved: no file-level state] @defgroup cpisi_health_file @ingroup cpisi_health_domain @{ @} */

/** S.6 OMISSION — [Reserved: S.3=S.2e, S.4=S.2d/BODY, S.5=none] @defgroup cpisi_health_omission @ingroup cpisi_health_domain @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * BODY BLOCK [BODY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_body BODY — Health Domain Implementation
 * @brief    Health types and functions owned by health.h
 *
 * DOMAIN OWNERSHIP: health.h OWNS all health types and functions.
 * types.h provides foundation (KFactor, HebrewState); health.h provides health.
 *
 * ┌─────────────────┬─────────────────────────────────────────────────────────────┐
 * │ Section         │ Contents                                                    │
 * ├─────────────────┼─────────────────────────────────────────────────────────────┤
 * │ B.1 ORG CHART   │ Structure overview, counts, flow                            │
 * ├─────────────────┼─────────────────────────────────────────────────────────────┤
 * │ B.2 TYPES       │ HealthScore, HealthDirection (3), HealthLevel (7),          │
 * │                 │ HealthThreshold — core type definitions                     │
 * ├─────────────────┼─────────────────────────────────────────────────────────────┤
 * │ B.3 HELPERS     │ Validation, Conversion, Utilities                           │
 * ├─────────────────┼─────────────────────────────────────────────────────────────┤
 * │ B.4 ERRORS      │ Error handling patterns                                     │
 * ├─────────────────┼─────────────────────────────────────────────────────────────┤
 * │ B.5 PUBLIC API  │ Exported interface (accessors)                              │
 * ├─────────────────┼─────────────────────────────────────────────────────────────┤
 * │ B.6 OMISSION    │ What's intentionally excluded                               │
 * └─────────────────┴─────────────────────────────────────────────────────────────┘
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_orgchart B.1 ORG CHART — Structure Overview
 * @ingroup cpisi_health_body
 * @brief    Map structure — interface overview, implementation location.
 *
 * CONTAINS:
 *   - B.1a STRUCTURE — Types, functions count
 *   - B.1b FLOW      — Implementation location
 *   - B.1c COUNTS    — Summary statistics
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Interface Overview [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_orgchart_structure B.1a STRUCTURE — Interface Overview
 * @ingroup cpisi_health_orgchart
 * @brief    Types, functions declared in this header.
 *
 * TYPES (4):
 *   - HealthScore (typedef int16_t)
 *   - HealthDirection (enum, 3 values)
 *   - HealthLevel (enum, 7 values)
 *   - HealthThreshold (struct)
 *
 * INFO STRUCTS (2):
 *   - HealthDirectionInfo (lookup metadata)
 *   - HealthLevelInfo (lookup metadata)
 *
 * FUNCTIONS (18 inline):
 *   - Validation: 4 (clamp, valid for score and level)
 *   - Conversion: 10 (float, level, hebrew, base3, severity)
 *   - Utilities: 4 (adjust, recover, degrade, fallback)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types: 4 (HealthScore, HealthDirection, HealthLevel, HealthThreshold) */
/* Structs: 2 (HealthDirectionInfo, HealthLevelInfo) */
/* Functions: 18 inline */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_orgchart_flow B.1b FLOW — Implementation Location
 * @ingroup cpisi_health_orgchart
 * @brief    Where implementations live.
 *
 * All functions are static inline — no .c implementation file needed.
 * Config source: bereshit/word/core/os/health/health.toml
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: All inline in this header */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_orgchart_counts B.1c COUNTS — Summary Statistics
 * @ingroup cpisi_health_orgchart
 * @brief    Interface element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types: 4 | Structs: 2 | Functions: 18 | Defines: 12 */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 TYPES — Health Type Definitions [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_types B.2 TYPES — Health Type Definitions
 * @ingroup cpisi_health_body
 * @brief    Health measurement — scores, levels, direction, thresholds.
 *
 * WHY: Health measurement types owned by health.h (domain ownership).
 *      Builds on foundation (KFactor, HebrewState from types.h).
 *
 * SCOPE: HealthScore (base100), HealthDirection (3), HealthLevel (7), HealthThreshold.
 *
 * CONTAINS:
 *   - B.2a HealthScore      — Base100 continuous value (-100 to +100)
 *   - B.2b HealthDirection  — Trajectory (wanting/center/integrity) (3 values)
 *   - B.2c HealthLevel      — MASTER, embeds direction + hebrew_state (7 values)
 *   - B.2d HealthThreshold  — Runtime monitoring alerts
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ HEALTH ARCHITECTURE = SCORE (base100) × DIRECTION (3) × LEVEL (7)          │
 * ├─────────┬───────┬───────────────────────────────────────────────────────────┤
 * │ Score   │ base  │ HealthScore — continuous value (-100 to +100)             │
 * │ Direction│  3   │ HealthDirection — trajectory (wanting/center/integrity)   │
 * │ Level   │   7   │ HealthLevel — MASTER, embeds direction + hebrew_state     │
 * ├─────────┼───────┼───────────────────────────────────────────────────────────┤
 * │ Threshold│config│ HealthThreshold — runtime monitoring alerts               │
 * └─────────┴───────┴───────────────────────────────────────────────────────────┘
 *
 * Dependency Order: Score → Direction (3) → Level (7) → Threshold
 *
 * Config source: bereshit/word/core/os/health/health.toml
 *
 * > "Just balances, just weights" — Leviticus 19:36
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a HealthScore — Base100 Value [HEALTH_SCORE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_score B.2a HealthScore — Base100 Value
 * @ingroup cpisi_health_types
 * @brief    Base100 canonical health score (-100 to +100).
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ SCALE RELATIONSHIP (derives from S.2 ternary scale)                         │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ Base100 / 100.0 = Ternary Value                                             │
 * │   -100 → -1.0 (shavar)    -50 → -0.5 (chaser)    0 → 0.0 (yashar)          │
 * │   +50 → +0.5 (shalem)    +100 → +1.0 (tov)                                  │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{
 */

/**
 * @name HealthScore [HEALTH_SCORE]
 * @brief Base100 canonical health score (-100 to +100).
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ SCALE RELATIONSHIP (derives from S.2 ternary scale)                         │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ Base100 / 100.0 = Ternary Value                                             │
 * │   -100 → -1.0 (shavar)    -50 → -0.5 (chaser)    0 → 0.0 (yashar)          │
 * │   +50 → +0.5 (shalem)    +100 → +1.0 (tov)                                  │
 * └─────────────────────────────────────────────────────────────────────────────┘
 * @{
 */

/** @brief Base100 health score type: -100 (broken) to +100 (perfect) */
typedef int16_t HealthScore;

/** @brief Health score scale factor (derives from S.2 ternary scale) */
#define HEALTH_SCALE        (100)

/** @brief Minimum health score (anchor, derives: CPISI_TERNARY_MIN * HEALTH_SCALE) */
#ifndef HEALTH_MIN
#define HEALTH_MIN          ((int16_t)(CPISI_TERNARY_MIN * HEALTH_SCALE))
#endif
/** @brief Maximum health score (anchor, derives: CPISI_TERNARY_MAX * HEALTH_SCALE) */
#ifndef HEALTH_MAX
#define HEALTH_MAX          ((int16_t)(CPISI_TERNARY_MAX * HEALTH_SCALE))
#endif
/** @brief Balanced health score (anchor, derives: CPISI_TERNARY_CENTER * HEALTH_SCALE) */
#ifndef HEALTH_BALANCED
#define HEALTH_BALANCED     ((int16_t)(CPISI_TERNARY_CENTER * HEALTH_SCALE))
#endif
/** @brief Health range (derived: MAX - MIN) */
#define HEALTH_RANGE        (HEALTH_MAX - HEALTH_MIN)

/** @brief Health scale divisors for multi-scale conversion (derive from HEALTH_SCALE) */
#define HEALTH_SCALE_HALF    (HEALTH_SCALE / 2)      /**< 50: hardpoint at ±50 */
#define HEALTH_SCALE_QUARTER (HEALTH_SCALE / 4)      /**< 25: severity step (Base9) */
#define HEALTH_SCALE_FIFTH   (HEALTH_SCALE / 5)      /**< 20: base20 divisor */
#define HEALTH_SCALE_TENTH   (HEALTH_SCALE / 10)     /**< 10: base10 divisor */

/** @brief Dead zone threshold for base3 sign detection (±10% of scale) */
#define HEALTH_DEADZONE      (HEALTH_SCALE_TENTH)    /**< 10: values within ±10 → 0 */

/** @} */ /* end HealthScore */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b HealthDirection (3) — Trajectory [HEALTH_DIRECTION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_direction B.2b HealthDirection — Trajectory
 * @ingroup cpisi_health_types
 * @brief    3-value macro-level health trajectory (maps to k-factor).
 *
 * 3-value health trajectory, maps to KFactor semantics.
 * Enum values ARE ternary values (-1, 0, +1).
 *
 * ┌───────────┬───────┬───────────────────────────────────────────────────────┐
 * │ Direction │ Value │ Meaning                                               │
 * ├───────────┼───────┼───────────────────────────────────────────────────────┤
 * │ wanting   │  -1   │ Moving toward deficit (retreat)                       │
 * │ center    │   0   │ At balance point [DEFAULT] (maintain)                 │
 * │ integrity │  +1   │ Moving toward wholeness (advance)                     │
 * └───────────┴───────┴───────────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{
 */

/**
 * @brief   3-value health trajectory
 *
 * Indicates macro-level direction of health movement.
 * Maps directly to ternary values and KFactor semantics.
 *
 * @note    Default: HEALTH_DIR_CENTER (stable)
 * @note    Enum values ARE ternary values (-1, 0, +1)
 * @see     KFactor for cognitive direction
 */
typedef enum HealthDirection {
    HEALTH_DIR_WANTING   = -1,   /**< Moving toward deficit (ternary: -1) */
    HEALTH_DIR_CENTER    =  0,   /**< At balance point [DEFAULT] (ternary: 0) */
    HEALTH_DIR_INTEGRITY = +1,   /**< Moving toward wholeness (ternary: +1) */
} HealthDirection;

/** @brief Total number of health directions */
#define HEALTH_DIR_COUNT   (3)

/**
 * @brief   Unified lookup table for HealthDirection
 *
 * Contains all direction metadata in ONE struct.
 * Index: (direction + 1) maps -1→0, 0→1, +1→2
 */
typedef struct HealthDirectionInfo {
    int8_t      value;      /**< Ternary value (-1, 0, +1) */
    KFactor     kfactor;    /**< Corresponding k-factor */
    const char* name;       /**< "wanting", "center", "integrity" */
    const char* symbol;     /**< ←, •, → */
    const char* emoji;      /**< Visual indicator */
} HealthDirectionInfo;

/**
 * @brief   MASTER lookup for HealthDirection
 *
 * Access: HEALTH_DIR_INFO[dir + 1] (offset for negative index)
 * Or use: health_dir_info(dir) inline accessor
 */
static const HealthDirectionInfo HEALTH_DIR_INFO[HEALTH_DIR_COUNT] = {
    /*[0]*/ { -1, K_RETREAT,  "wanting",   "←", "⬇️" },  /* HEALTH_DIR_WANTING */
    /*[1]*/ {  0, K_MAINTAIN, "center",    "•", "⚖️" },  /* HEALTH_DIR_CENTER */
    /*[2]*/ { +1, K_ADVANCE,  "integrity", "→", "⬆️" },  /* HEALTH_DIR_INTEGRITY */
};

/**
 * @brief   Get HealthDirectionInfo for a direction value
 * @param   dir  HealthDirection (-1, 0, +1)
 * @return  Pointer to info struct (defaults to center if invalid)
 */
static inline const HealthDirectionInfo* health_dir_info(HealthDirection dir) {
    int idx = dir + 1;
    if (idx < 0 || idx >= HEALTH_DIR_COUNT) idx = 1; /* default: center */
    return &HEALTH_DIR_INFO[idx];
}

/** @} */ // end HealthDirection

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2c HealthLevel (7) — MASTER [HEALTH_LEVEL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_level B.2c HealthLevel — MASTER
 * @ingroup cpisi_health_types
 * @brief    7-level discrete health system (embeds direction + hebrew_state).
 *
 * 7-level discrete health system. MASTER table embeds:
 *   - HealthDirection (which macro-level trajectory)
 *   - HebrewState (cognitive state mapping)
 *   - Score ranges (min/max bounds)
 *
 * ┌─────────┬──────────────┬───────────┬────────────────────────────────────────┐
 * │ Level   │ Range        │ Direction │ Hebrew Anchor                          │
 * ├─────────┼──────────────┼───────────┼────────────────────────────────────────┤
 * │ broken  │ [-100, -50]  │ wanting   │ shavar (severe negative)               │
 * │ wanting │ [-49, -25]   │ wanting   │ chaser (transition)                    │
 * │ lacking │ [-24, -1]    │ wanting   │ chaser (mild negative)                 │
 * │ even    │ [0, 0]       │ center    │ yashar                                 │
 * │ sound   │ [1, 25]      │ integrity │ shalem (transition)                    │
 * │ whole   │ [26, 50]     │ integrity │ shalem (mild positive)                 │
 * │ perfect │ [51, 100]    │ integrity │ tov (severe positive)                  │
 * └─────────┴──────────────┴───────────┴────────────────────────────────────────┘
 *
 * Hardpoints: -100, -50, 0, +50, +100 (derived from HEALTH_SCALE_HALF)
 * Config source: bereshit/word/core/os/health/health.toml
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{
 */

/**
 * @brief   7-level discrete health classification (MASTER table).
 *
 * Hardpoints: -100, -50, 0, +50, +100 (derived from HEALTH_SCALE_HALF)
 * Config source: bereshit/word/core/os/health/health.toml
 * @{
 */

/**
 * @brief   7-level discrete health classification
 *
 * Maps continuous HealthScore to discrete levels for threshold-based logic.
 * Each level has embedded direction and Hebrew state.
 *
 * @note    Default: HEALTH_LEVEL_EVEN (balanced)
 */
typedef enum HealthLevel {
    HEALTH_LEVEL_BROKEN  = 0,   /**< [-100, -50] — crisis (shavar) */
    HEALTH_LEVEL_WANTING = 1,   /**< [-49, -25]  — significant deficit */
    HEALTH_LEVEL_LACKING = 2,   /**< [-24, -1]   — needs attention (chaser) */
    HEALTH_LEVEL_EVEN    = 3,   /**< [0, 0]      — balanced [DEFAULT] (yashar) */
    HEALTH_LEVEL_SOUND   = 4,   /**< [1, 25]     — building integrity */
    HEALTH_LEVEL_WHOLE   = 5,   /**< [26, 50]    — significant wholeness (shalem) */
    HEALTH_LEVEL_PERFECT = 6,   /**< [51, 100]   — full integrity (tov) */
} HealthLevel;

/** @brief Total number of health levels */
#define HEALTH_LEVEL_COUNT   (7)

/**
 * @brief   Unified lookup table for HealthLevel (MASTER)
 *
 * Contains ALL level metadata in ONE struct:
 *   - Score bounds (min/max)
 *   - HealthDirection (trajectory)
 *   - HebrewState (cognitive anchor)
 *   - Display info (name, emoji)
 */
typedef struct HealthLevelInfo {
    int16_t         score_min;  /**< Lower bound (inclusive) */
    int16_t         score_max;  /**< Upper bound (inclusive) */
    HealthDirection direction;  /**< Trajectory: wanting/center/integrity */
    HebrewState     hebrew;     /**< Cognitive state anchor */
    const char*     name;       /**< "broken", "wanting", etc. */
    const char*     emoji;      /**< Visual symbol */
} HealthLevelInfo;

/**
 * @brief   MASTER lookup for HealthLevel
 *
 * Access: HEALTH_LEVEL_INFO[level]
 * Or use: health_level_info(level) inline accessor
 *
 * Score ranges derive from HEALTH_SCALE (100):
 *   - Hardpoints: -100, -50, -25, 0, +25, +50, +100
 *   - HEALTH_SCALE_HALF = 50, HEALTH_SCALE_QUARTER = 25
 */
static const HealthLevelInfo HEALTH_LEVEL_INFO[HEALTH_LEVEL_COUNT] = {
    [HEALTH_LEVEL_BROKEN]  = { -100,  -50, HEALTH_DIR_WANTING,   HEBREW_SHAVAR, "broken",  "💔" },
    [HEALTH_LEVEL_WANTING] = {  -49,  -25, HEALTH_DIR_WANTING,   HEBREW_CHASER, "wanting", "🩹" },
    [HEALTH_LEVEL_LACKING] = {  -24,   -1, HEALTH_DIR_WANTING,   HEBREW_CHASER, "lacking", "💛" },
    [HEALTH_LEVEL_EVEN]    = {    0,    0, HEALTH_DIR_CENTER,    HEBREW_YASHAR, "even",    "⚖️" },
    [HEALTH_LEVEL_SOUND]   = {    1,   25, HEALTH_DIR_INTEGRITY, HEBREW_SHALEM, "sound",   "💚" },
    [HEALTH_LEVEL_WHOLE]   = {   26,   50, HEALTH_DIR_INTEGRITY, HEBREW_SHALEM, "whole",   "💙" },
    [HEALTH_LEVEL_PERFECT] = {   51,  100, HEALTH_DIR_INTEGRITY, HEBREW_TOV,    "perfect", "👑" },
};

/**
 * @brief   Get HealthLevelInfo for a level
 * @param   level  HealthLevel enum value
 * @return  Pointer to info struct (defaults to EVEN if invalid)
 */
static inline const HealthLevelInfo* health_level_info(HealthLevel level) {
    if (level < 0 || level >= HEALTH_LEVEL_COUNT) level = HEALTH_LEVEL_EVEN;
    return &HEALTH_LEVEL_INFO[level];
}

/**
 * @brief   Get HealthDirection for a level
 * @param   level  HealthLevel enum value
 * @return  HealthDirection (wanting/center/integrity)
 */
static inline HealthDirection health_level_direction(HealthLevel level) {
    return health_level_info(level)->direction;
}

/**
 * @brief   Get HebrewState for a level
 * @param   level  HealthLevel enum value
 * @return  HebrewState (cognitive anchor)
 */
static inline HebrewState health_level_hebrew(HealthLevel level) {
    return health_level_info(level)->hebrew;
}

/**
 * @brief   Convert HealthScore to HealthLevel
 * @param   score  Base100 health score (-100 to +100)
 * @return  Corresponding HealthLevel
 */
static inline HealthLevel health_score_to_level(HealthScore score) {
    if (score <= -50) return HEALTH_LEVEL_BROKEN;
    if (score <= -25) return HEALTH_LEVEL_WANTING;
    if (score <    0) return HEALTH_LEVEL_LACKING;
    if (score ==   0) return HEALTH_LEVEL_EVEN;
    if (score <=  25) return HEALTH_LEVEL_SOUND;
    if (score <=  50) return HEALTH_LEVEL_WHOLE;
    return HEALTH_LEVEL_PERFECT;
}

/** @} */ // end HealthLevel

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2d HealthThreshold — Runtime Monitoring [HEALTH_THRESHOLD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_threshold B.2d HealthThreshold — Runtime Monitoring
 * @ingroup cpisi_health_types
 * @brief    Configurable alert thresholds for runtime health monitoring.
 *
 * Uses HealthScore (base100) values for comparison.
 * Thresholds reference HEALTH_LEVEL_INFO score bounds.
 * @{
 */

/**
 * @brief   Runtime health monitoring thresholds
 *
 * Configurable alert thresholds for triggering warnings, critical alerts,
 * and recovery confirmation. Debounce prevents alert spam.
 *
 * @note    All threshold values are HealthScore (base100)
 */
typedef struct HealthThreshold {
    HealthScore warn_below;     /**< Warn when health drops below this */
    HealthScore critical_below; /**< Critical when health drops below this */
    HealthScore recover_above;  /**< Recovery confirmed when health rises above */
    uint32_t    debounce_ms;    /**< Minimum time between alerts (prevent spam) */
} HealthThreshold;

/**
 * @brief Default health threshold configuration
 *
 * Values derive from HEALTH_LEVEL_INFO score bounds:
 *   warn_below:     -25 (entering WANTING level)
 *   critical_below: -50 (entering BROKEN level)
 *   recover_above:   25 (back to SOUND level)
 */
#define HEALTH_THRESHOLD_DEFAULT { \
    .warn_below     = -25,   /* HEALTH_LEVEL_INFO[WANTING].score_max */ \
    .critical_below = -50,   /* HEALTH_LEVEL_INFO[BROKEN].score_max */ \
    .recover_above  =  25,   /* HEALTH_LEVEL_INFO[SOUND].score_max */ \
    .debounce_ms    = 1000,  \
}

/** @} */ /* end B.2d HealthThreshold */

/** @} */ /* end B.2 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 HELPERS — Health Support Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_helpers B.3 HELPERS — Health Support Functions
 * @ingroup cpisi_health_body
 * @brief    Validation, conversion, and utility functions for health types.
 *
 * WHY: Support functions that operate on health types.
 *      Validation ensures correctness, conversion bridges types, utilities provide operations.
 *
 * SCOPE: All inline helper functions owned by health.h.
 *
 * CONTAINS:
 *   - B.3a VALIDATION  — Clamp, valid checks for HealthScore, HealthLevel, HealthDirection
 *   - B.3b CONVERSION  — Score↔Float, Score↔Level, Score↔Hebrew, Direction conversions
 *   - B.3c UTILITIES   — adjust, recover, degrade, fallback operations
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a VALIDATION — Type Validation [VALIDATION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_validation B.3a VALIDATION — Type Validation
 * @ingroup cpisi_health_helpers
 * @brief    Clamp and validity checks for health types.
 *
 * CONTAINS:
 *   - HealthScore: cpisi_health_clamp(), cpisi_health_valid()
 *   - HealthLevel: cpisi_health_level_clamp(), cpisi_health_level_valid()
 *   - HealthDirection: cpisi_sign_to_health_dir()
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* ─────────────────────────────────────────────────────────────────────────────
 * HealthScore — signed int16: HEALTH_MIN(-100) to HEALTH_MAX(+100)
 * ───────────────────────────────────────────────────────────────────────────── */

/** @brief Clamp int to valid HealthScore range [-100, +100] */
static inline HealthScore cpisi_health_clamp(int value) {
    return (HealthScore)CPISI_CLAMP(value, HEALTH_MIN, HEALTH_MAX);
}

/** @brief Check if HealthScore is valid */
static inline bool cpisi_health_valid(HealthScore health) {
    return CPISI_IN_RANGE_SIGNED(health, HEALTH_MIN, HEALTH_MAX);
}

/** @brief Convert sign to HealthDirection */
static inline HealthDirection cpisi_sign_to_health_dir(int value) {
    return CPISI_SIGN_TO(value, HEALTH_DIR_WANTING, HEALTH_DIR_CENTER, HEALTH_DIR_INTEGRITY);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * HealthLevel — unsigned enum: 0 to HEALTH_LEVEL_COUNT-1 (7 levels)
 * ───────────────────────────────────────────────────────────────────────────── */

/** @brief Clamp int to valid HealthLevel range [0, 6] */
static inline HealthLevel cpisi_health_level_clamp(int value) {
    return (HealthLevel)CPISI_CLAMP(value, 0, HEALTH_LEVEL_COUNT - 1);
}

/** @brief Check if HealthLevel is valid */
static inline bool cpisi_health_level_valid(HealthLevel level) {
    return CPISI_IN_RANGE_UNSIGNED(level, HEALTH_LEVEL_COUNT - 1);
}

/** @} */ /* end B.3a VALIDATION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b CONVERSION — Type Conversions [CONVERSION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_conversion B.3b CONVERSION — Type Conversions
 * @ingroup cpisi_health_helpers
 * @brief    Conversion functions between health types and related types.
 *
 * CONTAINS:
 *   - Score↔Float: cpisi_health_to_float(), cpisi_float_to_health()
 *   - Score↔Hebrew: cpisi_health_to_hebrew() [KEY BRIDGE]
 *   - Score→Level: cpisi_health_to_level()
 *   - Score→Base3: cpisi_health_to_base3()
 *   - Score→Severity: cpisi_health_to_severity()
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{
 */

/**
 * @brief THE KEY BRIDGE: Health + KFactor → HebrewState (uses S.2 CPISI_ZONE_*)
 *
 * Hardpoints: -50, -25, +25, +50 (derived from HEALTH_SCALE_HALF/QUARTER)
 * K-factor selects directional variant in zones 2 and 4.
 *
 * | Health Range              | K=-1   | K=0    | K=+1   |
 * |---------------------------|--------|--------|--------|
 * | h <= -50 (ZONE_BROKEN)    | shavar | shavar | shavar |
 * | -50 < h <= -25 (zone 2)   | chaser | chaser | ratsah |
 * | -25 < h < +25 (center)    | yashar | yashar | yashar |
 * | +25 <= h < +50 (zone 4)   | tamim  | shalem | shalem |
 * | h >= +50 (ZONE_PERFECT)   | tov    | tov    | tov    |
 */
static inline HebrewState cpisi_health_to_hebrew(HealthScore health, KFactor k) {
    // Edge states (do not depend on k-factor)
    if (health <= CPISI_ZONE_BROKEN)  return HEBREW_SHAVAR;
    if (health >= CPISI_ZONE_PERFECT) return HEBREW_TOV;

    // Center state
    if (health > CPISI_ZONE_LOWER && health < CPISI_ZONE_UPPER) return HEBREW_YASHAR;

    // Negative zone: chaser vs ratsah
    if (health <= CPISI_ZONE_LOWER) {
        return (k == K_ADVANCE) ? HEBREW_RATSAH : HEBREW_CHASER;
    }

    // Positive zone: tamim vs shalem
    return (k == K_RETREAT) ? HEBREW_TAMIM : HEBREW_SHALEM;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Float Conversions (HealthScore ↔ Float)
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * @brief   Convert HealthScore to ternary float [-1.0, +1.0]
 * @param   health  Health score in range [-100, +100]
 * @return  Ternary float in range [-1.0, +1.0]
 */
static inline float cpisi_health_to_float(HealthScore health) {
    return (float)health / (float)HEALTH_SCALE;
}

/**
 * @brief   Convert ternary float to HealthScore
 * @param   ternary  Ternary float in range [-1.0, +1.0]
 * @return  Health score clamped to [-100, +100]
 */
static inline HealthScore cpisi_float_to_health(float ternary) {
    return cpisi_health_clamp((int)(ternary * HEALTH_SCALE));
}

// --- Level Conversions (HealthScore → Level → Direction) ---

/**
 * @brief   Convert HealthScore to HealthLevel (0-6)
 * @param   health  Health score in range [-100, +100]
 * @return  HealthLevel corresponding to score range
 */
static inline HealthLevel cpisi_health_to_level(HealthScore health) {
    return health_score_to_level(health);
}

/**
 * @brief   Get direction from HealthLevel
 * @param   level   Health level (0-6)
 * @return  HealthDirection (wanting/center/integrity)
 */
static inline HealthDirection cpisi_health_level_direction(HealthLevel level) {
    return health_level_direction(level);
}

// --- Hebrew Bridge (Health → Hebrew via Level) ---

/**
 * @brief   Convert HealthLevel to HebrewState
 * @param   level   Health level (0-6)
 * @return  HebrewState anchored to that level
 */
static inline HebrewState cpisi_health_level_to_hebrew(HealthLevel level) {
    return health_level_hebrew(level);
}

// --- Base3 Sign ---

/** @brief Convert HealthScore to base3 sign with dead zone */
static inline int8_t cpisi_health_to_base3(HealthScore health) {
    if (health < -HEALTH_DEADZONE) return -1;
    if (health > +HEALTH_DEADZONE) return +1;
    return 0;
}

/** @brief Convert HealthScore to HebrewState with neutral k-factor */
static inline HebrewState cpisi_health_to_hebrew_neutral(HealthScore health) {
    return cpisi_health_to_hebrew(health, K_MAINTAIN);
}

// --- Severity → Health (from types.h Severity metadata) ---

/**
 * @brief   Get health impact from Severity (config-driven)
 * @param   sev  Severity enum value (-4 to +4)
 * @return  Health impact from severity_info()->health_impact
 * @note    Uses types.h severity_info() lookup
 */
static inline int cpisi_severity_to_health(Severity sev) {
    return severity_info(sev)->health_impact;
}

/** @} */ /* end B.3b CONVERSION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3c UTILITIES — Health Operations [UTILITIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_utilities B.3c UTILITIES — Health Operations
 * @ingroup cpisi_health_helpers
 * @brief    Utility functions for health operations.
 *
 * CONTAINS:
 *   - cpisi_health_adjust()        — Adjust health by delta
 *   - cpisi_health_recover()       — Recover to center
 *   - cpisi_health_degrade()       — Graceful degradation toward center
 *   - cpisi_health_level_fallback() — Step toward safer level
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Adjust health by delta, clamping to valid range */
static inline HealthScore cpisi_health_adjust(HealthScore health, int delta) {
    return cpisi_health_clamp((int)health + delta);
}

/**
 * @brief   Recover HealthScore to center (0)
 * @return  HealthScore at YASHAR center (0)
 */
static inline HealthScore cpisi_health_recover(void) {
    return (HealthScore)0;  /* YASHAR = 0 */
}

/**
 * @brief   Recover HealthLevel to center (EVEN)
 * @return  HEALTH_LEVEL_EVEN (center level)
 */
static inline HealthLevel cpisi_health_level_recover(void) {
    return HEALTH_LEVEL_EVEN;
}

/**
 * @brief   Degrade health toward center (graceful degradation)
 * @param   health  Current health score
 * @param   step    Step size toward center (positive)
 * @return  Health score moved toward 0
 */
static inline HealthScore cpisi_health_degrade(HealthScore health, int step) {
    if (step <= 0) step = 1;
    int delta = (health > 0) ? -step : (health < 0) ? +step : 0;
    return cpisi_health_adjust(health, delta);
}

/**
 * @brief   Get fallback health level (safer than current)
 * @param   level   Current health level
 * @return  HealthLevel one step closer to EVEN
 */
static inline HealthLevel cpisi_health_level_fallback(HealthLevel level) {
    if (level < HEALTH_LEVEL_EVEN) {
        return cpisi_health_level_clamp((int)level + 1);
    } else if (level > HEALTH_LEVEL_EVEN) {
        return cpisi_health_level_clamp((int)level - 1);
    }
    return HEALTH_LEVEL_EVEN;
}

/** @} */ /* end B.3c UTILITIES */

/** @} */ /* end B.3 HELPERS */

/** B.4 ERRORS — [Reserved: error handling in error.h] @defgroup cpisi_health_errors B.4 ERRORS @ingroup cpisi_health_body @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Exported Interface [PUBLIC_API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_api B.5 PUBLIC API — Exported Interface
 * @ingroup cpisi_health_body
 * @brief    Accessor functions for health metadata (public interface).
 *
 * CONTAINS:
 *   - cpisi_health_level_name()  — Get name for HealthLevel
 *   - cpisi_health_level_emoji() — Get emoji for HealthLevel
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief   Get name for HealthLevel
 * @param   level   HealthLevel enum (0-6)
 * @return  Name string from HEALTH_LEVEL_INFO
 */
static inline const char* cpisi_health_level_name(HealthLevel level) {
    return health_level_info(level)->name;
}

/**
 * @brief   Get emoji symbol for HealthLevel
 * @param   level   HealthLevel enum (0-6)
 * @return  Emoji string from HEALTH_LEVEL_INFO
 */
static inline const char* cpisi_health_level_emoji(HealthLevel level) {
    return health_level_info(level)->emoji;
}

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Intentionally Excluded [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_omission B.6 OMISSION — Intentionally Excluded
 * @ingroup cpisi_health_body
 * @brief    What is NOT in this file and why.
 *
 * NOT INCLUDED:
 *   - Health persistence: Belongs in health storage layer
 *   - Health aggregation: Belongs in health analysis layer
 *   - Health display formatting: Belongs in UI layer
 *   - Error handling: Defined in error.h, used here
 *
 * WHY: Domain ownership — health.h owns types and operations, not persistence/UI.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* See: health storage, health analysis, UI formatters for excluded functionality */

/** @} */ /* end B.6 OMISSION */

/** @} */ /* end BODY */

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
 * @defgroup cpisi_health_x1_validation X.1 VALIDATION — Compile-time Verification
 * @{
 *
 * VALIDATION NOTES:
 *   - Static assertions in S.2c VERIFY catch health errors at compile time
 *   - Hebrew state bounds verified (7 states, -1.0 to +1.0)
 *   - Severity and trend ranges verified
 *   - If validation fails, check health derivation chain
 *
 * @} end X.1 VALIDATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Post-include Checks [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_execution X.2 EXECUTION — Post-include Checks
 * @{
 *
 * POST-INCLUDE CHECKS:
 *   - Verify config.h included (provides types.h transitively)
 *   - All health macros resolve to expected values
 *   - Lookup tables accessible via accessor macros
 *
 * @} end X.2 EXECUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Resource Management [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_cleanup X.3 CLEANUP — Resource Management
 * @{
 *
 * CLEANUP NOTES:
 *   - No dynamic resources in header — health is compile-time/static only
 *   - Lookup tables are static const — no cleanup needed
 *   - Inline functions use no heap allocation
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
 * @defgroup cpisi_health_policy X.4 POLICY — Modification Guidance
 * @{
 *
 * MODIFICATION POLICY:
 *   - Health types derive from Hebrew state model (7 states)
 *   - Adding states: update S.2a bounds, S.2c tripwires, S.2d/S.2e tables
 *   - Severity/trend changes require corresponding tripwire updates
 *   - Template: bereshit/word/seed/code/c/type-header.h
 *
 * @} end X.4 POLICY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — Future Expansion [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_extension X.5 EXTENSION — Future Expansion
 * @{
 *
 * EXTENSION POINTS:
 *   - Health trend analysis: Add to BODY B.3c UTILITIES
 *   - Health report generation: Add to BODY B.5 PUBLIC API
 *   - New severity levels: Update S.2a, S.2c, S.2e
 *   - Always add tripwires for new health values
 *
 * @} end X.5 EXTENSION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Issues [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_troubleshoot X.6 TROUBLESHOOT — Common Issues
 * @{
 *
 * COMMON ISSUES:
 *   Q: "Static assert failed on health state"
 *   A: Check HEALTH_STATE_COUNT = 7, verify Hebrew state mapping
 *
 *   Q: "Severity index out of range"
 *   A: Use CPISI_SEVERITY_VALID() before table access
 *
 *   Q: "Health score clamping unexpected"
 *   A: Verify score in [-100, +100] range, check clamp functions
 *
 * @} end X.6 TROUBLESHOOT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Related Documentation [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_reference X.7 REFERENCE — Related Documentation
 * @{
 *
 * RELATED FILES:
 *   - types.h: Foundation types (BOUNDS, PRIMITIVES, Hebrew states)
 *   - config/config.h: Tier 1 config anchor (includes types.h)
 *   - error/error.h: Error handling integration
 *   - dar.h: Data access records using health types
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
 * @defgroup cpisi_health_note X.8 NOTE — Implementation Notes
 * @{
 *
 * IMPLEMENTATION NOTES:
 *   - Hebrew state model: 7 states from shavar(-1.0) to tov(+1.0)
 *   - YASHAR (0) is the health center — balance point
 *   - Severity levels (5): NONE → LOW → MEDIUM → HIGH → CRITICAL
 *   - Trend values (3): DECLINING (-1) → STABLE (0) → IMPROVING (+1)
 *   - DAR pipeline: KERNEL → LAYER → ZONE → HEALTH aggregation
 *
 * @} end X.8 NOTE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Intentional Exclusions [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_health_x9_omission X.9 OMISSION — Intentional Exclusions
 * @{
 *
 * INTENTIONAL EXCLUSIONS:
 *   - No dynamic allocation: All health ops are compile-time or stack-based
 *   - No threading: Health state is read-only or caller-managed
 *   - S.3-S.6 reserved: Health logic complete in S.2 and BODY
 *
 * @} end X.9 OMISSION
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_TYPES_HEALTH_H */
