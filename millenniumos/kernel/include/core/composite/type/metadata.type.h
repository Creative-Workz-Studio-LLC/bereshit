/// @file metadata.h
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
/// @omni:req  key        = CPISI-TYPES-METADATA
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
/// @omni:ins  layer      = types/metadata
/// @omni:ins  includes   = types.h
/// @omni:ins  provides   = [S.2_METADATA]
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
/// @omni:req  title      = CPI-SI Metadata Domain
/// @omni:req  brief      = Metadata slots and byte encoding
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
 * @defgroup cpisi_metadata_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup cpisi_metadata_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_metadata_special_config
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

#ifndef CPISI_TYPES_METADATA_H
#define CPISI_TYPES_METADATA_H

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
 * @file      metadata.h
 * @brief     CPI-SI Metadata Domain — Metadata slots, encoding types.
 *
 * @defgroup cpisi_metadata_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-METADATA
 * Title:     CPI-SI Metadata Domain Types
 * Type:      Header (Domain)
 * Component: Rails — provides metadata types for BRIDGE equation slots
 * Role:      Metadata slot definitions (13 slots in BRIDGE equation).
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_state M.2 STATE — Lifecycle State
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
 * @defgroup cpisi_metadata_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_metadata_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/metadata/metadata.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: types/types.h (foundation), types/config/config.h
 * Template:     bereshit/word/seed/code/c/type-header.h
 * Extracted:    2026-01-21 from types.h metadata sections
 *
 * Builds FROM:  types.h, config/config.h
 * Builds TO:    DAR (Data Access Records), encoding systems
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, metadata, domain, slots, encoding, bridge
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide metadata slot types for the 13-slot BRIDGE encoding.
 * Core Design: 13 slots completing the BRIDGE equation (243 + 13 = 256).
 * Philosophy:  Metadata illuminates data, making it meaningful.
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Psalm 119:105 — "Thy word is a lamp unto my feet"
 * Principle:  Metadata illuminates data, making it meaningful.
 * Anchor:     Proverbs 2:6 — "the LORD giveth wisdom... knowledge and understanding"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h (foundation types)
 *
 * What Uses This:
 *   - DAR (Data Access Records)
 *   - Encoding systems
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
 * @defgroup cpisi_metadata_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-21) — Extracted from types.h metadata sections
 *   a-01.25 (2026-01-23) — Expanded METADATA to full M.1-M.10 structure
 *   b-03.00 (2026-01-25) — Template alignment: type-header.h, BODY/CLOSING structure
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ 13 metadata slots defined
 *   ✓ BODY B.1-B.6 structure
 *
 * Planned:
 *   ⏳ Metadata encoding/decoding functions
 *   ⏳ Slot validation utilities
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
///   - S.2 DEFINES   — Metadata domain (Bounds, Macros, Verify, Lookup, Types)
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
 * @defgroup cpisi_metadata_includes S.1 INCLUDES — Header Dependencies
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

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_metadata_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_metadata_includes
 * @brief    C standard library headers required by this module.
 *
 * INHERITED: All system headers come through types.h S.1a.
 *   types.h provides: stdint.h, stddef.h, stdbool.h, limits.h, string.h, stdio.h
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
 * @defgroup cpisi_metadata_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_metadata_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * PROVIDES: Foundation types, macros, and error code infrastructure.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/config/type/error.type.h"  /**< TIER 1: error codes (brings types.h transitively) */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_metadata_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_metadata_includes
 * @brief    External library headers (none for metadata domain).
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
 * @defgroup cpisi_metadata_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_metadata_includes
 * @brief    C++ compatibility wrapper.
 *
 * INHERITED: extern "C" wrapper comes through types.h S.1d.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Inherited from types.h S.1d — extern "C" already active] */

/** @} */ /* end S.1d CPP */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Metadata Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_domain S.2 DEFINES — Metadata Domain
 * @brief    Error metadata slots (13 slots across 4 categories).
 *
 * PURPOSE: Define metadata-specific constants, operations, verification, and types.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: no domain-specific preludes]
 *   - S.2a  BOUNDS  — Metadata counts, ranges, codes, categories
 *   - S.2b  MACROS  — Metadata extract, construct, convert, validate
 *   - S.2c VERIFY  — Metadata compile-time assertions
 *   - S.2d LOOKUP  — Metadata tables
 *   - S.2e TYPES   — Metadata enumerations
 *
 * THE BRIDGE EQUATION: 243 + 13 = 256
 *   - PENTERACT (3⁵ = 243) layer error codes
 *   - BRIDGE (13) metadata slots
 *   - BYTE (2⁸ = 256) total codes
 *
 * SLOT ORGANIZATION (1 + 4 + 4 + 4 = 13):
 *   Category 0 (RESERVED):  slot 0       (code 243)
 *   Category 1 (CONFIG):    slots 1-4    (codes 244-247)
 *   Category 2 (RESOURCE):  slots 5-8    (codes 248-251)
 *   Category 3 (SYSTEM):    slots 9-12   (codes 252-255)
 *
 * Builds FROM: S.1 INCLUDES (foundation dependencies)
 * Builds TO:   S.3 CLOSING (file end)
 *
 * @{
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2.0 PRELUDE — [Reserved] [PRELUDE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_metadata_prelude S.2.0 METADATA PRELUDE — Domain Operations
 * @ingroup cpisi_metadata_domain
 * @brief    [Reserved: no domain-specific preludes]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Metadata domain uses foundation PRELUDE operations] */

/** @} */ /* end S.2.0 PRELUDE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BOUNDS — Metadata bounds (counts, ranges, codes, categories) [BOUNDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_metadata_bounds S.2a METADATA BOUNDS — Domain Constants
 * @ingroup cpisi_metadata_domain
 * @brief    Metadata counts, ranges, codes, and category indices.
 *
 * CONTAINS:
 *   - Counts — META_COUNT (13), RESERVED (1), WORKING (12)
 *   - Category slots — SLOT_*_MIN/MAX per category
 *   - Offset ranges — OFFSET_MIN (0) to OFFSET_MAX (12)
 *   - Code ranges — CODE_RESERVED (243) to CODE_MAX (255)
 *
 * Builds FROM: S.2.0 PRELUDE
 * Builds TO:   S.2b MACROS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Counts — Metadata domain array sizes
 *  @brief META_COUNT (13 BRIDGE slots), RESERVED (1), WORKING (12).
 *  @{ */
#define CPISI_META_COUNT            CPISI_BRIDGE    /**< 13: metadata slots (BRIDGE)         */
#define CPISI_METADATA_COUNT        CPISI_META_COUNT /**< 13: alias for compatibility         */
#define CPISI_META_RESERVED_COUNT   CPISI_UNITY     /**< 1: reserved slot count              */
#define CPISI_META_WORKING_COUNT    CPISI_DIFF(CPISI_META_COUNT, CPISI_UNITY) /**< 12: working   */
/** @} */ /* end Counts */

/** @name Offset Bounds — MIN/CENTER/MAX for metadata offsets
 *  @brief 13 slots: 0 reserved, 1-12 working. Config-driven from META_COUNT.
 *  @{ */
/**
 * @brief 0: minimum offset — reserved anchor slot.
 * @par Derivation
 *      OFFSET_MIN = VOID = MIN_OF(META_COUNT) = 0
 */
#define CPISI_META_OFFSET_MIN       CPISI_VOID
/**
 * @brief 6: center offset — middle slot.
 * @par Derivation
 *      OFFSET_CENTER = HALF(META_COUNT) = HALF(13) = 6
 */
#define CPISI_META_OFFSET_CENTER    CPISI_HALF(CPISI_META_COUNT)
/**
 * @brief 12: maximum offset — last slot.
 * @par Derivation
 *      OFFSET_MAX = DIFF(META_COUNT, UNITY) = MAX_OF(META_COUNT) = 12
 */
#define CPISI_META_OFFSET_MAX       CPISI_DIFF(CPISI_META_COUNT, CPISI_UNITY)
/** @} */ /* end Offset Bounds */

/* CATEGORY SLOT RANGES */
#define CPISI_META_SLOT_RESERVED    CPISI_VOID      /**< 0: anchor slot                    */

#define CPISI_META_SLOT_CONFIG_MIN  CPISI_UNITY     /**< 1: CONFIG start                   */
#define CPISI_META_SLOT_CONFIG_MAX  CPISI_SQUARE    /**< 4: CONFIG end                     */
#define CPISI_META_SLOT_CONFIG_CENTER CPISI_SUM(CPISI_META_SLOT_CONFIG_MIN, CPISI_HALF(CPISI_SQUARE)) /**< 2 */

#define CPISI_META_SLOT_RESOURCE_MIN CPISI_SUM(CPISI_SQUARE, CPISI_UNITY) /**< 5: RESOURCE start */
#define CPISI_META_SLOT_RESOURCE_MAX CPISI_PRODUCT(CPISI_BINARY, CPISI_SQUARE) /**< 8: RESOURCE end */
#define CPISI_META_SLOT_RESOURCE_CENTER CPISI_SUM(CPISI_META_SLOT_RESOURCE_MIN, CPISI_HALF(CPISI_SQUARE)) /**< 6 */

#define CPISI_META_SLOT_SYSTEM_MIN  CPISI_SUM(CPISI_PRODUCT(CPISI_BINARY, CPISI_SQUARE), CPISI_UNITY) /**< 9 */
#define CPISI_META_SLOT_SYSTEM_MAX  CPISI_META_OFFSET_MAX       /**< 12: SYSTEM end    */
#define CPISI_META_SLOT_SYSTEM_CENTER CPISI_SUM(CPISI_META_SLOT_SYSTEM_MIN, CPISI_HALF(CPISI_SQUARE)) /**< 10 */

/* SLOT CODES (243-255) — actual error code values */
#define CPISI_META_RESERVED         CPISI_SUM(CPISI_PENTERACT, CPISI_META_SLOT_RESERVED) /**< 243: fallback */

#define CPISI_META_VERSION          CPISI_SUM(CPISI_PENTERACT, CPISI_META_SLOT_CONFIG_MIN) /**< 244: C:1      */
#define CPISI_META_CONFIG           CPISI_SUM(CPISI_PENTERACT, CPISI_SUM(CPISI_META_SLOT_CONFIG_MIN, CPISI_UNITY)) /**< 245 */
#define CPISI_META_STATE            CPISI_SUM(CPISI_PENTERACT, CPISI_SUM(CPISI_META_SLOT_CONFIG_MIN, CPISI_BINARY)) /**< 246 */
#define CPISI_META_INIT             CPISI_SUM(CPISI_PENTERACT, CPISI_META_SLOT_CONFIG_MAX) /**< 247: C:4      */

#define CPISI_META_MEMORY           CPISI_SUM(CPISI_PENTERACT, CPISI_META_SLOT_RESOURCE_MIN) /**< 248: R:1    */
#define CPISI_META_TIMEOUT          CPISI_SUM(CPISI_PENTERACT, CPISI_SUM(CPISI_META_SLOT_RESOURCE_MIN, CPISI_UNITY)) /**< 249 */
#define CPISI_META_PERMISSION       CPISI_SUM(CPISI_PENTERACT, CPISI_SUM(CPISI_META_SLOT_RESOURCE_MIN, CPISI_BINARY)) /**< 250 */
#define CPISI_META_RESOURCE         CPISI_SUM(CPISI_PENTERACT, CPISI_META_SLOT_RESOURCE_MAX) /**< 251: R:4    */

#define CPISI_META_ASSERT           CPISI_SUM(CPISI_PENTERACT, CPISI_META_SLOT_SYSTEM_MIN) /**< 252: S:1      */
#define CPISI_META_PANIC            CPISI_SUM(CPISI_PENTERACT, CPISI_SUM(CPISI_META_SLOT_SYSTEM_MIN, CPISI_UNITY)) /**< 253 */
#define CPISI_META_UNIMPLEMENTED    CPISI_SUM(CPISI_PENTERACT, CPISI_SUM(CPISI_META_SLOT_SYSTEM_MIN, CPISI_BINARY)) /**< 254 */
#define CPISI_META_FATAL            CPISI_SUM(CPISI_PENTERACT, CPISI_META_SLOT_SYSTEM_MAX) /**< 255: S:4      */

/* SLOT INDICES (0-12) — for array indexing */
#define CPISI_META_SLOT_VERSION     CPISI_META_SLOT_CONFIG_MIN  /**< 1: CONFIG start   */
#define CPISI_META_SLOT_CONFIG      CPISI_SUM(CPISI_META_SLOT_CONFIG_MIN, CPISI_UNITY)   /**< 2           */
#define CPISI_META_SLOT_STATE       CPISI_SUM(CPISI_META_SLOT_CONFIG_MIN, CPISI_BINARY)  /**< 3           */
#define CPISI_META_SLOT_INIT        CPISI_META_SLOT_CONFIG_MAX  /**< 4: CONFIG end     */

#define CPISI_META_SLOT_MEMORY      CPISI_META_SLOT_RESOURCE_MIN /**< 5: RESOURCE start */
#define CPISI_META_SLOT_TIMEOUT     CPISI_SUM(CPISI_META_SLOT_RESOURCE_MIN, CPISI_UNITY) /**< 6           */
#define CPISI_META_SLOT_PERMISSION  CPISI_SUM(CPISI_META_SLOT_RESOURCE_MIN, CPISI_BINARY) /**< 7          */
#define CPISI_META_SLOT_RESOURCE    CPISI_META_SLOT_RESOURCE_MAX /**< 8: RESOURCE end   */

#define CPISI_META_SLOT_ASSERT      CPISI_META_SLOT_SYSTEM_MIN  /**< 9: SYSTEM start   */
#define CPISI_META_SLOT_PANIC       CPISI_SUM(CPISI_META_SLOT_SYSTEM_MIN, CPISI_UNITY)   /**< 10          */
#define CPISI_META_SLOT_UNIMPLEMENTED CPISI_SUM(CPISI_META_SLOT_SYSTEM_MIN, CPISI_BINARY) /**< 11         */
#define CPISI_META_SLOT_FATAL       CPISI_META_SLOT_SYSTEM_MAX  /**< 12: SYSTEM end    */

/** @name Categories — CONFIG, RESOURCE, SYSTEM
 *  @brief Working categories for metadata slots (excluding RESERVED).
 *  @{ */
#define CPISI_META_CAT_COUNT        CPISI_SQUARE    /**< 4: total categories                 */
#define CPISI_META_CAT_WORKING      CPISI_TERNARY   /**< 3: working categories               */
#define CPISI_META_CAT_RESERVED     CPISI_VOID      /**< 0: reserved index [MIN]             */
#define CPISI_META_CAT_CONFIG       CPISI_SUM(CPISI_META_CAT_RESERVED, CPISI_UNITY) /**< 1: config idx   */
#define CPISI_META_CAT_RESOURCE     CPISI_SUM(CPISI_META_CAT_CONFIG, CPISI_UNITY)   /**< 2: resource idx */
#define CPISI_META_CAT_SYSTEM       CPISI_SUM(CPISI_META_CAT_RESOURCE, CPISI_UNITY) /**< 3: system [MAX] */
/** @} */ /* end Categories */

/** @} */ /* end S.2a BOUNDS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b MACROS — Metadata operations [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_metadata_macros S.2b METADATA MACROS — Operations
 * @ingroup cpisi_metadata_domain
 * @brief    Metadata slot extraction, construction, conversion, and validation.
 *
 * PURPOSE: Provide metadata-specific operations for the 13-slot BRIDGE system
 *          (codes 243-255) using BOUNDS from types.h.
 *
 * Builds FROM: types.h (S.2a.2 DIMENSIONS, S.2a.7 METADATA, S.2b.2 PRIMITIVES)
 * Builds TO:   Error reporting (system-level errors), config validation
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC extensions to S.2b MACROS defined in
 *   types.h. The foundation macros (DIFF, SUM, QUOTIENT, MODULO) come from
 *   types.h. This file adds metadata slot navigation macros.
 *
 * THE BRIDGE EQUATION:
 *   PENTERACT(243) + BRIDGE(13) = BYTE(256)
 *   13 metadata slots bridge ternary architecture (3⁵) to binary byte (2⁸)
 *
 * STRUCTURE:
 *   S.2b.7d METADATA DOMAIN — Slot operations
 *     S.2b.7d.1 EXTRACT     — code → offset
 *     S.2b.7d.2 CONSTRUCT   — offset → code
 *     S.2b.7d.3 CONVERT     — bidirectional aliases
 *     S.2b.7d.4 SLOT OPS    — slot ↔ category/position
 *     S.2b.7d.5 CATEGORY    — category operations
 *     S.2b.7d.6 VALIDATION  — range checks
 *     S.2b.7d.7 DERIVATION  — derive category/position from offset
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2b_7d_METADATA_DOMAIN S.2b.7d METADATA DOMAIN — Slot operations
 * @ingroup cpisi_metadata_macros
 * @brief Metadata slot extraction, construction, conversion, and validation macros.
 *
 * MATHEMATICAL STRUCTURE:
 *   The 243+13=256 BRIDGE equation: PENTERACT(243) + BRIDGE(13) = BYTE(256)
 *
 *   13 metadata slots (codes 243-255) provide system-level error information
 *   separate from the 243 layer errors. This bridges ternary architecture (3⁵)
 *   to binary byte (2⁸).
 *
 *   ┌───────────────────────────────────────────────────────────────────────────┐
 *   │  METADATA SLOT STRUCTURE (13 slots = 1 + 4 + 4 + 4)                       │
 *   │  ───────────────────────────────────────────────────────────────────────  │
 *   │  CATEGORY      │ SLOTS    │ CODES      │ PURPOSE                          │
 *   │  ────────────  │ ───────  │ ──────────  │ ────────────────────────────     │
 *   │  0 RESERVED    │ 0        │ 243         │ Future expansion                 │
 *   │  1 CONFIG      │ 1-4      │ 244-247     │ Configuration errors             │
 *   │  2 RESOURCE    │ 5-8      │ 248-251     │ Resource errors                  │
 *   │  3 SYSTEM      │ 9-12     │ 252-255     │ System errors                    │
 *   └───────────────────────────────────────────────────────────────────────────┘
 *
 * STRUCTURE:
 *   S.2b.7d.1 EXTRACT     — code → offset
 *   S.2b.7d.2 CONSTRUCT   — offset → code
 *   S.2b.7d.3 CONVERT     — bidirectional aliases
 *   S.2b.7d.4 SLOT OPS    — slot ↔ category/position
 *   S.2b.7d.5 CATEGORY    — category ↔ balanced, cross-domain
 *   S.2b.7d.6 VALIDATION  — range checks
 *   S.2b.7d.7 DERIVATION  — derive category/position from offset
 *
 * BUILDS FROM: S.2a.2 DIMENSIONS (PENTERACT=243), S.2a.7 METADATA
 * DEPENDS ON:  S.2b.2 PRIMITIVES (DIFF, SUM, QUOTIENT, MODULO)
 * @{ */

/* ── S.2b.7d.1 EXTRACT — code → offset ──────────────────────────────────────── */

/**
 * @brief Extract slot offset from metadata code.
 *
 * BOUNDS USED:
 *   - CPISI_PENTERACT = 243 (3⁵, start of metadata range)
 *
 * FORMULA: offset = code - PENTERACT = code - 243
 *
 * DERIVATION:
 *   - Metadata codes span [243, 255]
 *   - Subtracting 243 gives slot offset [0, 12]
 *   - This is the inverse of META_CODE
 *
 * EXAMPLES:
 *   META_OFFSET(243) = 243 - 243 = 0  (RESERVED slot)
 *   META_OFFSET(244) = 244 - 243 = 1  (CONFIG slot 1)
 *   META_OFFSET(255) = 255 - 243 = 12 (SYSTEM slot 4)
 *
 * @param code Metadata code (243-255)
 * @return Slot offset (0-12)
 */
#define CPISI_META_OFFSET(code)     CPISI_DIFF((code), CPISI_PENTERACT)

/* ── S.2b.7d.2 CONSTRUCT — offset → code ────────────────────────────────────── */

/**
 * @brief Construct metadata code from slot offset.
 *
 * BOUNDS USED:
 *   - CPISI_PENTERACT = 243 (base of metadata range)
 *
 * FORMULA: code = PENTERACT + offset = 243 + offset
 *
 * DERIVATION:
 *   - Slot offsets span [0, 12]
 *   - Adding 243 gives metadata codes [243, 255]
 *   - This is the inverse of META_OFFSET
 *
 * EXAMPLES:
 *   META_CODE(0)  = 243 + 0  = 243 (RESERVED)
 *   META_CODE(1)  = 243 + 1  = 244 (CONFIG slot 1)
 *   META_CODE(12) = 243 + 12 = 255 (SYSTEM slot 4)
 *
 * @param offset Slot offset (0-12)
 * @return Metadata code (243-255)
 */
#define CPISI_META_CODE(offset)     CPISI_SUM(CPISI_PENTERACT, (offset))

/* ── S.2b.7d.3 CONVERT — bidirectional aliases ──────────────────────────────── */

/** @brief Alias: slot → code (slot offset and code offset are equivalent) */
#define CPISI_META_SLOT_TO_CODE(slot)   CPISI_META_CODE(slot)

/** @brief Alias: code → slot */
#define CPISI_META_CODE_TO_SLOT(code)   CPISI_META_OFFSET(code)

/* ── S.2b.7d.4 SLOT OPS — extract/build slot components ─────────────────────── */

/**
 * SLOT LAYOUT:
 *   13 slots organized as 1 reserved + 3 categories × 4 slots each:
 *
 *   ┌────────────────────────────────────────────────────────────────────────┐
 *   │  SLOT    CAT   POS    FORMULA                                          │
 *   │  ─────   ───   ───    ────────────────────────────────────────────     │
 *   │  0       0     0      RESERVED (special case)                          │
 *   │  1-4     1     1-4    (slot-1) ÷ 4 + 1 = 1  (CONFIG)                   │
 *   │  5-8     2     1-4    (slot-1) ÷ 4 + 1 = 2  (RESOURCE)                 │
 *   │  9-12    3     1-4    (slot-1) ÷ 4 + 1 = 3  (SYSTEM)                   │
 *   └────────────────────────────────────────────────────────────────────────┘
 */

/**
 * @brief Extract category from slot index.
 *
 * FORMULA:
 *   slot == 0 → 0 (RESERVED)
 *   slot > 0  → (slot - 1) ÷ 4 + 1
 *
 * DERIVATION:
 *   - Slot 0 is special RESERVED slot → category 0
 *   - Slots 1-12 are working slots organized in 4-slot groups
 *   - Subtracting 1 makes 0-indexed, divide by 4 gives group, add 1 for category
 *
 * EXAMPLES:
 *   SLOT_CAT(0)  = 0 (RESERVED)
 *   SLOT_CAT(1)  = (1-1)÷4 + 1 = 1 (CONFIG)
 *   SLOT_CAT(5)  = (5-1)÷4 + 1 = 2 (RESOURCE)
 *   SLOT_CAT(12) = (12-1)÷4 + 1 = 3 (SYSTEM)
 *
 * @param slot Slot index (0-12)
 * @return Category index (0-3): 0=RESERVED, 1=CONFIG, 2=RESOURCE, 3=SYSTEM
 */
#define CPISI_META_SLOT_CAT(slot) \
    ((slot) == CPISI_VOID ? CPISI_VOID : \
     CPISI_SUM(CPISI_QUOTIENT(CPISI_DIFF((slot), CPISI_UNITY), CPISI_SQUARE), CPISI_UNITY))

/**
 * @brief Extract position within category from slot index.
 *
 * FORMULA:
 *   slot == 0 → 0 (RESERVED has no position)
 *   slot > 0  → (slot - 1) mod 4 + 1
 *
 * DERIVATION:
 *   - Slot 0 is special → position 0
 *   - Slots 1-12 repeat positions 1-4 within each category
 *   - Modulo 4 gives 0-3, adding 1 gives 1-4
 *
 * EXAMPLES:
 *   SLOT_POS(0)  = 0 (RESERVED)
 *   SLOT_POS(1)  = (1-1) mod 4 + 1 = 1
 *   SLOT_POS(4)  = (4-1) mod 4 + 1 = 4
 *   SLOT_POS(5)  = (5-1) mod 4 + 1 = 1 (RESOURCE, pos 1)
 *
 * @param slot Slot index (0-12)
 * @return Position within category (0 for reserved, 1-4 for working)
 */
#define CPISI_META_SLOT_POS(slot) \
    ((slot) == CPISI_VOID ? CPISI_VOID : \
     CPISI_SUM(CPISI_MODULO(CPISI_DIFF((slot), CPISI_UNITY), CPISI_SQUARE), CPISI_UNITY))

/**
 * @brief Build slot index from category and position.
 *
 * FORMULA:
 *   cat == 0 → 0 (RESERVED)
 *   cat > 0  → (cat - 1) × 4 + pos
 *
 * DERIVATION:
 *   - Category 0 always gives slot 0 (RESERVED)
 *   - Categories 1-3 each have 4 slots
 *   - (cat-1) gives 0-indexed category, multiply by 4 for base, add position
 *
 * EXAMPLES:
 *   SLOT_BUILD(0, 0) = 0 (RESERVED)
 *   SLOT_BUILD(1, 1) = (1-1)×4 + 1 = 1 (CONFIG slot 1)
 *   SLOT_BUILD(2, 3) = (2-1)×4 + 3 = 7 (RESOURCE slot 3)
 *   SLOT_BUILD(3, 4) = (3-1)×4 + 4 = 12 (SYSTEM slot 4)
 *
 * @param cat Category index (0-3)
 * @param pos Position within category (1-4)
 * @return Slot index (0-12)
 */
#define CPISI_META_SLOT_BUILD(cat, pos) \
    ((cat) == CPISI_VOID ? CPISI_VOID : \
     CPISI_SUM(CPISI_PRODUCT(CPISI_DIFF((cat), CPISI_UNITY), CPISI_SQUARE), (pos)))

/* ── S.2b.7d.5 CATEGORY — category ↔ balanced, cross-domain ─────────────────── */

/**
 * CATEGORY CONVERSION:
 *   Working categories (1-3) map to balanced form (-1..+1):
 *
 *   ┌────────────────────────────────────────────────────────────────────────┐
 *   │  CATEGORY INDEX    BALANCED    NAME                                    │
 *   │  ──────────────    ────────    ─────────────────                       │
 *   │  0                 N/A         RESERVED (not working)                  │
 *   │  1                 -1          CONFIG                                  │
 *   │  2                  0          RESOURCE (center)                       │
 *   │  3                 +1          SYSTEM                                  │
 *   └────────────────────────────────────────────────────────────────────────┘
 *
 *   FORMULA: bal = idx - WORKING_CENTER = idx - 2
 */

/** @brief Metadata category → balanced. FORMULA: bal = idx - 2 */
#define CPISI_META_CAT_TO_BAL(idx)      CPISI_DIFF((idx), CPISI_META_CAT_WORKING_CENTER)

/** @brief Balanced → metadata category. FORMULA: idx = bal + 2 */
#define CPISI_BAL_TO_META_CAT(bal)      CPISI_SUM((bal), CPISI_META_CAT_WORKING_CENTER)

/** @brief Cross-domain: metadata category → error category (via balanced) */
#define CPISI_META_CAT_TO_ERR_CAT(meta_idx) CPISI_BAL_TO_CAT(CPISI_META_CAT_TO_BAL(meta_idx))

/** @brief Cross-domain: error category → metadata category (via balanced) */
#define CPISI_ERR_CAT_TO_META_CAT(err_idx)  CPISI_BAL_TO_META_CAT(CPISI_CAT_TO_BAL(err_idx))

/* ── S.2b.7d.6 VALIDATION — range checks ────────────────────────────────────── */

/**
 * VALIDATION RANGES:
 *   ┌────────────────────────────────────────────────────────────────────────┐
 *   │  Component            MIN     MAX     Range                            │
 *   │  ─────────────────    ────    ────    ─────────────────────────────    │
 *   │  META_CODE            243     255     243-255 (PENTERACT + 0..12)      │
 *   │  META_OFFSET          0       12      0-12 (13 slots)                  │
 *   │  META_CAT             0       3       0-3 (RES/CFG/RES/SYS)            │
 *   │  META_CAT_WORKING     1       3       1-3 (working categories only)    │
 *   │  CONFIG slots         1       4       slots 1-4                        │
 *   │  RESOURCE slots       5       8       slots 5-8                        │
 *   │  SYSTEM slots         9       12      slots 9-12                       │
 *   └────────────────────────────────────────────────────────────────────────┘
 */

/** @brief Check if code is valid metadata code (243-255). */
#define CPISI_IS_META_CODE(code)        CPISI_IN_RANGE((code), CPISI_ERROR_METADATA_MIN, CPISI_ERROR_METADATA_MAX)

/** @brief Check if offset is valid metadata offset (0-12). */
#define CPISI_IS_META_OFFSET(off)       CPISI_IN_RANGE((off), CPISI_META_OFFSET_MIN, CPISI_META_OFFSET_MAX)

/** @brief Check if category is valid (0-3). */
#define CPISI_IS_META_CAT(cat)          CPISI_IN_RANGE((cat), CPISI_META_CAT_MIN, CPISI_META_CAT_MAX)

/** @brief Check if category is working (1-3, excludes RESERVED). */
#define CPISI_IS_META_CAT_WORKING(cat)  CPISI_IN_RANGE((cat), CPISI_META_CAT_WORKING_MIN, CPISI_META_CAT_WORKING_MAX)

/** @brief Check if category is RESERVED (0). */
#define CPISI_IS_META_CAT_RESERVED(cat) ((cat) == CPISI_META_CAT_RESERVED)

/** @brief Check if slot is in CONFIG range (1-4). */
#define CPISI_IS_META_SLOT_CONFIG(slot)   CPISI_IN_RANGE((slot), CPISI_META_SLOT_CONFIG_MIN, CPISI_META_SLOT_CONFIG_MAX)

/** @brief Check if slot is in RESOURCE range (5-8). */
#define CPISI_IS_META_SLOT_RESOURCE(slot) CPISI_IN_RANGE((slot), CPISI_META_SLOT_RESOURCE_MIN, CPISI_META_SLOT_RESOURCE_MAX)

/** @brief Check if slot is in SYSTEM range (9-12). */
#define CPISI_IS_META_SLOT_SYSTEM(slot)   CPISI_IN_RANGE((slot), CPISI_META_SLOT_SYSTEM_MIN, CPISI_META_SLOT_SYSTEM_MAX)

/* ── S.2b.7d.7 DERIVATION — derive category/position from offset/code ───────── */

/**
 * DERIVATION PATTERN:
 *   These macros use cascading conditionals to determine category/position
 *   from offset, rather than arithmetic. This handles the special RESERVED
 *   slot (0) cleanly.
 *
 *   ┌────────────────────────────────────────────────────────────────────────┐
 *   │  OFFSET    CATEGORY    POSITION    DERIVATION                          │
 *   │  ───────   ────────    ────────    ──────────────────────────────      │
 *   │  0         RESERVED    0           special case                        │
 *   │  1-4       CONFIG      offset      direct (offset IS position)         │
 *   │  5-8       RESOURCE    offset-4    subtract CONFIG range               │
 *   │  9-12      SYSTEM      offset-8    subtract CONFIG+RESOURCE range      │
 *   └────────────────────────────────────────────────────────────────────────┘
 */

/**
 * @brief Derive category from offset using cascading conditionals.
 *
 * FORMULA: Cascading check against slot ranges
 *   offset == 0 → RESERVED
 *   1 ≤ offset ≤ 4 → CONFIG
 *   5 ≤ offset ≤ 8 → RESOURCE
 *   9 ≤ offset ≤ 12 → SYSTEM
 *
 * @param offset Slot offset (0-12)
 * @return Category index (0-3)
 */
#define CPISI_META_CATEGORY(offset) ( \
    (offset) == CPISI_VOID ? CPISI_META_CAT_RESERVED : \
    CPISI_IS_META_SLOT_CONFIG(offset) ? CPISI_META_CAT_CONFIG : \
    CPISI_IS_META_SLOT_RESOURCE(offset) ? CPISI_META_CAT_RESOURCE : \
                                          CPISI_META_CAT_SYSTEM \
)

/**
 * @brief Derive position within category from offset.
 *
 * FORMULA: Cascading subtraction based on category
 *   offset == 0 → 0 (RESERVED has no position)
 *   CONFIG:   position = offset (1-4)
 *   RESOURCE: position = offset - 4 (5-8 → 1-4)
 *   SYSTEM:   position = offset - 8 (9-12 → 1-4)
 *
 * @param offset Slot offset (0-12)
 * @return Position (0 for reserved, 1-4 for working)
 */
#define CPISI_META_SLOT_POSITION(offset) ( \
    (offset) == CPISI_VOID ? CPISI_VOID : \
    CPISI_IS_META_SLOT_CONFIG(offset) ? (offset) : \
    CPISI_IS_META_SLOT_RESOURCE(offset) ? CPISI_DIFF((offset), CPISI_SQUARE) : \
                                          CPISI_DIFF((offset), CPISI_PRODUCT(CPISI_BINARY, CPISI_SQUARE)) \
)

/** @brief Alias: offset → category */
#define CPISI_META_GET_CAT(offset)          CPISI_META_CATEGORY(offset)

/** @brief Alias: offset → position */
#define CPISI_META_GET_POSITION(offset)     CPISI_META_SLOT_POSITION(offset)

/**
 * @brief Derive balanced category from offset (working categories only).
 *
 * FORMULA: If working category, convert to balanced; else VOID
 *
 * @param offset Slot offset (0-12)
 * @return Balanced category (-1/0/+1) or VOID if reserved
 */
#define CPISI_META_GET_CAT_BAL(offset) ( \
    CPISI_IS_META_CAT_WORKING(CPISI_META_CATEGORY(offset)) ? \
    CPISI_META_CAT_TO_BAL(CPISI_META_CATEGORY(offset)) : CPISI_VOID \
)

/** @brief Compound: code → category (via offset) */
#define CPISI_META_CODE_CATEGORY(code)      CPISI_META_CATEGORY(CPISI_META_OFFSET(code))

/** @brief Compound: code → balanced category (via offset) */
#define CPISI_META_CODE_CAT_BAL(code)       CPISI_META_GET_CAT_BAL(CPISI_META_OFFSET(code))


/** @} */ /* end S.2b.7d METADATA DOMAIN */
/** @} */ /* end S.2b METADATA MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c VERIFY — Metadata compile-time assertions [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_metadata_verify S.2c METADATA VERIFY — Compile-time assertions
 * @ingroup cpisi_metadata_domain
 * @brief    Verify metadata slot architecture at compile time.
 *
 * PURPOSE: Catch metadata configuration errors BEFORE runtime. Zero runtime cost.
 *          If any assertion fails, compilation stops with a descriptive message.
 *
 * Builds FROM: S.2a METADATA BOUNDS (slot values), S.2b METADATA MACROS (derivations)
 * Builds TO:   System-level error reporting (verified values ready for use)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC tripwires extending S.2c VERIFY in types.h.
 *   Verifies the 13-slot BRIDGE system: category counts, slot ranges, code boundaries.
 *
 * THE BRIDGE EQUATION:
 *   PENTERACT(243) + BRIDGE(13) = BYTE(256)
 *   This is verified in S.2c.9 BRIDGE EQUATION.
 *
 * TRIPWIRE PHILOSOPHY:
 *   Each value is verified TWO ways:
 *   1. VALUE TRIPWIRE:        Assert the literal (e.g., META_COUNT == 13)
 *   2. RELATIONSHIP TRIPWIRE: Assert the derivation (e.g., PENTERACT + BRIDGE == BYTE)
 *   BOTH together catches drift in either direction.
 *
 * STRUCTURE:
 *   S.2c.8 METADATA DOMAIN — Verify metadata slots
 *     S.2c.8a COUNT  — META_COUNT, CAT_COUNT, WORKING_COUNT
 *     S.2c.8b BOUNDS — OFFSET_MIN/MAX
 *     S.2c.8c SLOTS  — CONFIG/RESOURCE/SYSTEM slot spans
 *     S.2c.8d CODES  — RESERVED/FATAL code boundaries
 *     S.2c.8e CATS   — Category indices
 *   S.2c.9 BRIDGE EQUATION — Verify 243 + 13 = 256
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2c_8_METADATA_DOMAIN S.2c.8 METADATA DOMAIN — Slot verification
 * @ingroup cpisi_metadata_verify
 * @brief Compile-time verification of metadata slot system.
 *
 * **TARGET: types-metadata.h** (will split when types.h is complete)
 *
 * THE BRIDGE SLOTS:
 *   The 13 metadata slots are THE BRIDGE that completes 243 + 13 = 256.
 *   They occupy codes 243-255, providing system-level status beyond layer errors.
 *
 * METADATA SLOT ARCHITECTURE:
 *   ┌─────────────────────────────────────────────────────────────────────────┐
 *   │  CATEGORY      SLOTS    CODES        PURPOSE                            │
 *   │  ───────────   ──────   ──────────   ───────────────────────────────    │
 *   │  RESERVED      0        243          Placeholder/unused                 │
 *   │  CONFIG        1-4      244-247      Configuration state                │
 *   │  RESOURCE      5-8      248-251      Resource allocation                │
 *   │  SYSTEM        9-12     252-255      System status (255=FATAL)          │
 *   └─────────────────────────────────────────────────────────────────────────┘
 *
 * KEY NUMBERS:
 *   - META_CAT_COUNT = 4 (SQUARE) — 4 categories
 *   - META_WORKING_COUNT = 12 = 3×4 (TERNARY × SQUARE) — 12 working slots
 *   - META_OFFSET_MAX = 12 (BRIDGE - 1) — 13 total slots (0-12)
 *   - META_RESERVED = 243 (PENTERACT) — first metadata code
 *   - META_FATAL = 255 (BYTE_MAX) — last code, fatal system error
 *
 * STRUCTURE:
 *   S.2c.8a COUNT  — META_COUNT, CAT_COUNT, WORKING_COUNT
 *   S.2c.8b BOUNDS — OFFSET_MIN/MAX
 *   S.2c.8c SLOTS  — CONFIG/RESOURCE/SYSTEM slot spans
 *   S.2c.8d CODES  — RESERVED/FATAL code boundaries
 *   S.2c.8e CATS   — CAT_RESERVED/SYSTEM indices
 *
 * @ingroup S2c_VERIFY
 * @{ */

/**
 * @defgroup S2c_8a_COUNT S.2c.8a COUNT TRIPWIRES — Verify metadata counts
 * @ingroup S2c_8_METADATA_DOMAIN
 * @brief Verify category count (4) and working slot count (12).
 *
 * DERIVATIONS:
 *   - META_CAT_COUNT = SQUARE (4)
 *   - META_WORKING_COUNT = TERNARY × SQUARE (3×4 = 12)
 * @{ */

CPISI_STATIC_ASSERT(CPISI_META_CAT_COUNT == 4,
                    "META_CAT_COUNT must be 4 — categories (RESERVED, CONFIG, RESOURCE, SYSTEM)");
CPISI_STATIC_ASSERT(CPISI_META_WORKING_COUNT == 12,
                    "META_WORKING_COUNT must be 12 — working slots (excluding reserved)");
CPISI_STATIC_ASSERT(CPISI_META_CAT_COUNT == CPISI_SQUARE,
                    "META_CAT_COUNT must equal SQUARE — verifies 4 = 4");
CPISI_STATIC_ASSERT(CPISI_META_WORKING_COUNT == CPISI_PRODUCT(CPISI_TERNARY, CPISI_SQUARE),
                    "META_WORKING_COUNT must equal TERNARY × SQUARE — verifies 12 = 3×4");


/** @} */ /* end S.2c.8a COUNT */

/**
 * @defgroup S2c_8b_BOUNDS S.2c.8b BOUNDS TRIPWIRES — Verify offset bounds
 * @ingroup S2c_8_METADATA_DOMAIN
 * @brief Verify metadata offset range (0..12).
 *
 * DERIVATIONS:
 *   - META_OFFSET_MIN = VOID (0)
 *   - META_OFFSET_MAX = BRIDGE - 1 (13-1 = 12)
 * @{ */

CPISI_STATIC_ASSERT(CPISI_META_OFFSET_MIN == 0,
                    "META_OFFSET_MIN must be 0 — first offset (reserved slot)");
CPISI_STATIC_ASSERT(CPISI_META_OFFSET_MAX == 12,
                    "META_OFFSET_MAX must be 12 — last offset (BRIDGE-1)");
CPISI_STATIC_ASSERT(CPISI_META_OFFSET_MIN == CPISI_VOID,
                    "META_OFFSET_MIN must equal VOID — verifies 0 = 0");
CPISI_STATIC_ASSERT(CPISI_META_OFFSET_MAX == CPISI_DIFF(CPISI_BRIDGE, CPISI_UNITY),
                    "META_OFFSET_MAX must equal BRIDGE-1 — verifies 12 = 13-1");


/** @} */ /* end S.2c.8b BOUNDS */

/**
 * @defgroup S2c_8c_SLOTS S.2c.8c SLOTS TRIPWIRES — Verify slot ranges span SQUARE
 * @ingroup S2c_8_METADATA_DOMAIN
 * @brief Verify each category spans exactly SQUARE (4) slots.
 *
 * SLOT RANGES:
 *   - CONFIG: 1-4 (4 slots), RESOURCE: 5-8 (4 slots), SYSTEM: 9-12 (4 slots)
 *   - Each span = SQUARE (4)
 * @{ */

CPISI_STATIC_ASSERT(CPISI_DIFF(CPISI_META_SLOT_CONFIG_MAX, CPISI_META_SLOT_CONFIG_MIN) + 1 == 4,
                    "CONFIG slot count must be 4 — span of slots 1-4");
CPISI_STATIC_ASSERT(CPISI_DIFF(CPISI_META_SLOT_RESOURCE_MAX, CPISI_META_SLOT_RESOURCE_MIN) + 1 == 4,
                    "RESOURCE slot count must be 4 — span of slots 5-8");
CPISI_STATIC_ASSERT(CPISI_DIFF(CPISI_META_SLOT_SYSTEM_MAX, CPISI_META_SLOT_SYSTEM_MIN) + 1 == 4,
                    "SYSTEM slot count must be 4 — span of slots 9-12");
CPISI_STATIC_ASSERT(CPISI_DIFF(CPISI_META_SLOT_CONFIG_MAX, CPISI_META_SLOT_CONFIG_MIN) + CPISI_UNITY == CPISI_SQUARE,
                    "CONFIG slot count must equal SQUARE — verifies 4 = 4");
CPISI_STATIC_ASSERT(CPISI_DIFF(CPISI_META_SLOT_RESOURCE_MAX, CPISI_META_SLOT_RESOURCE_MIN) + CPISI_UNITY == CPISI_SQUARE,
                    "RESOURCE slot count must equal SQUARE — verifies 4 = 4");
CPISI_STATIC_ASSERT(CPISI_DIFF(CPISI_META_SLOT_SYSTEM_MAX, CPISI_META_SLOT_SYSTEM_MIN) + CPISI_UNITY == CPISI_SQUARE,
                    "SYSTEM slot count must equal SQUARE — verifies 4 = 4");


/** @} */ /* end S.2c.8c SLOTS */

/**
 * @defgroup S2c_8d_CODES S.2c.8d CODES TRIPWIRES — Verify code boundaries
 * @ingroup S2c_8_METADATA_DOMAIN
 * @brief Verify reserved (243) and fatal (255) code boundaries.
 *
 * DERIVATIONS:
 *   - META_RESERVED = PENTERACT (3⁵ = 243)
 *   - META_FATAL = BYTE_MAX (255)
 * @{ */

CPISI_STATIC_ASSERT(CPISI_META_RESERVED == 243,
                    "META_RESERVED must be 243 — first metadata code (reserved slot)");
CPISI_STATIC_ASSERT(CPISI_META_FATAL == 255,
                    "META_FATAL must be 255 — last metadata code (fatal system error)");
CPISI_STATIC_ASSERT(CPISI_META_RESERVED == CPISI_PENTERACT,
                    "META_RESERVED must equal PENTERACT — verifies 243 = 3⁵");
CPISI_STATIC_ASSERT(CPISI_META_FATAL == CPISI_BYTE_MAX,
                    "META_FATAL must equal BYTE_MAX — verifies 255 = 255");


/** @} */ /* end S.2c.8d CODES */

/**
 * @defgroup S2c_8e_CATS S.2c.8e CATS TRIPWIRES — Verify category indices
 * @ingroup S2c_8_METADATA_DOMAIN
 * @brief Verify category index range (0=RESERVED to 3=SYSTEM).
 *
 * DERIVATIONS:
 *   - META_CAT_RESERVED = VOID (0)
 *   - META_CAT_SYSTEM = CAT_COUNT - 1 (4-1 = 3)
 * @{ */

CPISI_STATIC_ASSERT(CPISI_META_CAT_RESERVED == 0,
                    "META_CAT_RESERVED must be 0 — first category index");
CPISI_STATIC_ASSERT(CPISI_META_CAT_SYSTEM == 3,
                    "META_CAT_SYSTEM must be 3 — last category index");
CPISI_STATIC_ASSERT(CPISI_META_CAT_RESERVED == CPISI_VOID,
                    "META_CAT_RESERVED must equal VOID — verifies 0 = 0");
CPISI_STATIC_ASSERT(CPISI_META_CAT_SYSTEM == CPISI_DIFF(CPISI_META_CAT_COUNT, CPISI_UNITY),
                    "META_CAT_SYSTEM must equal CAT_COUNT-1 — verifies 3 = 4-1");


/** @} */ /* end S.2c.8e CATS */
/** @} */ /* end S.2c.8 METADATA DOMAIN */

/**
 * @defgroup S2c_9_BRIDGE_EQUATION S.2c.9 BRIDGE EQUATION — Verify 243 + 13 = 256
 * @ingroup S2c_VERIFY
 * @brief Verify metadata count = BRIDGE, completing the byte.
 *
 * THE BRIDGE EQUATION VERIFICATION:
 *   ERROR_CODE_COUNT + META_COUNT = 243 + 13 = 256 = BINARY_FIT
 *
 * This is the keystone assertion — if this fails, the entire
 * ternary↔binary bridge is broken.
 * @{ */

CPISI_STATIC_ASSERT(CPISI_META_COUNT == 13,
                    "META_COUNT must be 13 — THE BRIDGE");
CPISI_STATIC_ASSERT(CPISI_META_COUNT == CPISI_BRIDGE,
                    "META_COUNT must equal BRIDGE — 13 metadata slots");
CPISI_STATIC_ASSERT(CPISI_SUM(CPISI_ERROR_CODE_COUNT, CPISI_META_COUNT) == 256,
                    "ERROR_CODE_COUNT + META_COUNT must be 256 — 243+13 = byte");


/** @} */ /* end S.2c.9 BRIDGE EQUATION */

/** @} */ /* end S.2c METADATA VERIFY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d LOOKUP — Runtime lookup tables [LOOKUP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_metadata_lookup S.2d LOOKUP — Runtime lookup tables
 * @ingroup cpisi_metadata_domain
 * @brief Pre-computed tables for O(1) metadata slot operations.
 *
 * PURPOSE: Container for metadata domain lookup tables.
 *
 * STRUCTURE (mirrors types.h S.2d pattern: Domain → a/b/c):
 *   S.2d.7 METADATA — Metadata domain tables
 *     S.2d.7a TABLES     — 9 arrays (category, slot, bounds)
 *     S.2d.7b ACCESSORS  — 11 macros (category, slot, bounds access)
 *     S.2d.7c CONVERSION — [Reserved: metadata uses direct lookup]
 *
 * Builds FROM: S.2c METADATA VERIFY (structure verified)
 * Builds TO:   S.2e METADATA TYPES (enum definitions)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup cpisi_metadata_tables S.2d.7 METADATA — Metadata domain lookup
 * @ingroup cpisi_metadata_lookup
 * @brief Runtime lookup tables for metadata slots — 13 slots, 4 categories.
 *
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  METADATA SLOT TABLES — Bridge between PENTERACT and BINARY_FIT           ║
 * ║  ─────────────────────────────────────────────────────────────────────────║
 * ║  13 slots (BRIDGE) organized into 4 categories. Code range: 243-255.      ║
 * ║  Categories: RESERVED(0), CONFIG(1-4), RESOURCE(5-8), SYSTEM(9-12)        ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 *
 * METADATA SLOT STRUCTURE (13 = BRIDGE slots):
 *   - 4 categories: RESERVED(0), CONFIG(1-4), RESOURCE(5-8), SYSTEM(9-12)
 *   - 13 slots total = CPISI_BRIDGE (significant!)
 *   - Code range: 243-255 = PENTERACT + offset
 *
 * STRUCTURE (template pattern: a=TABLES, b=ACCESSORS, c=CONVERSION):
 *   S.2d.7a TABLES     — 9 arrays organized in 3 groups:
 *                        • CATEGORY (4×4): name, abbrev, version, balanced
 *                        • SLOT (2×13): name, desc
 *                        • BOUNDS (3×4): min, max, center per category
 *   S.2d.7b ACCESSORS  — 11 macros: 4 category + 2 slot + 3 bounds + 2 code
 *   S.2d.7c CONVERSION — [Reserved: metadata uses direct lookup]
 *
 * @ingroup S2d_LOOKUP
 * @{ */

/** @name S.2d.7a TABLES — Metadata lookup arrays
 *  @brief 9 arrays organized in 3 groups for metadata slot navigation.
 *
 *  CATEGORY ARRAYS (4 × 4 entries, indexed 0-3):
 *    g_cpisi_meta_cat_name[4]     — Full category names
 *    g_cpisi_meta_cat_abbrev[4]   — Single-char abbreviations
 *    g_cpisi_meta_cat_version[4]  — Living version chars
 *    g_cpisi_meta_cat_balanced[4] — Ternary balance values
 *
 *  SLOT ARRAYS (2 × 13 entries, indexed 0-12):
 *    g_cpisi_meta_name[13]        — Slot names
 *    g_cpisi_meta_desc[13]        — Slot descriptions
 *
 *  BOUNDS ARRAYS (3 × 4 entries, indexed 0-3):
 *    g_cpisi_meta_slot_min[4]     — First slot per category
 *    g_cpisi_meta_slot_max[4]     — Last slot per category
 *    g_cpisi_meta_slot_center[4]  — Center slot per category
 *
 *  @{ */

/* ═══════════════════════════════════════════════════════════════════════════
 * CATEGORY TABLES (4 × 4 arrays)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Metadata category tables (RESERVED/CONFIG/RESOURCE/SYSTEM).
 *
 * 4 arrays × 4 entries each, providing category-level lookup.
 */

/**
 * @brief Category name lookup — maps category index (0-3) to full name.
 *
 * BOUNDS USED:
 *   - CPISI_META_CAT_COUNT = 4 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Metadata category classification
 *   [0] RESERVED = slot 0 only, unclassified/unknown errors
 *   [1] CONFIG = slots 1-4, configuration-related errors
 *   [2] RESOURCE = slots 5-8, resource-related errors
 *   [3] SYSTEM = slots 9-12, system-level errors
 *
 * USE CASE: Error categorization, logging, filtering.
 */
static const char* const g_cpisi_meta_cat_name[CPISI_META_CAT_COUNT] = {
    "RESERVED",  /* [0] slot 0 only    */
    "CONFIG",    /* [1] slots 1-4      */
    "RESOURCE",  /* [2] slots 5-8      */
    "SYSTEM"     /* [3] slots 9-12     */
};

/**
 * @brief Category abbreviation lookup — maps category index (0-3) to single char.
 *
 * BOUNDS USED:
 *   - CPISI_META_CAT_COUNT = 4 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: XCRS abbreviations (X=unknown, C=Config, R=Resource, S=System)
 *
 * USE CASE: Compact error codes, status indicators.
 */
static const char g_cpisi_meta_cat_abbrev[CPISI_META_CAT_COUNT] = {
    'X',  /* [0] RESERVED (unknown) */
    'C',  /* [1] CONFIG             */
    'R',  /* [2] RESOURCE           */
    'S'   /* [3] SYSTEM             */
};

/**
 * @brief Category version char lookup — maps category index (0-3) to version char.
 *
 * BOUNDS USED:
 *   - CPISI_META_CAT_COUNT = 4 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Living versioning alignment
 *   RESERVED → 'x' (outside system)
 *   CONFIG → 'a' (alpha, early detection)
 *   RESOURCE → 'b' (beta, runtime)
 *   SYSTEM → 'c' (stable, core)
 *
 * USE CASE: Version string generation.
 */
static const char g_cpisi_meta_cat_version[CPISI_META_CAT_COUNT] = {
    'x',  /* [0] RESERVED (unknown)  */
    'a',  /* [1] CONFIG (alpha)      */
    'b',  /* [2] RESOURCE (beta)     */
    'c'   /* [3] SYSTEM (stable)     */
};

/**
 * @brief Category balanced value lookup — maps category index (0-3) to ternary value.
 *
 * BOUNDS USED:
 *   - CPISI_META_CAT_COUNT = 4 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Ternary assignment (note: 4 values, not strict ternary)
 *   RESERVED = 0 (outside ternary system)
 *   CONFIG = -1 (input-side, early)
 *   RESOURCE = 0 (process, middle)
 *   SYSTEM = +1 (output-side, late)
 *
 * USE CASE: Priority weighting, ternary calculations.
 */
static const int8_t g_cpisi_meta_cat_balanced[CPISI_META_CAT_COUNT] = {
     0,  /* [0] RESERVED (outside)   */
    -1,  /* [1] CONFIG (input-side)  */
     0,  /* [2] RESOURCE (process)   */
    +1   /* [3] SYSTEM (output-side) */
};

/* ═══════════════════════════════════════════════════════════════════════════
 * SLOT TABLES (2 × 13 arrays)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief The 13 metadata slot names and human-readable descriptions.
 *
 * 2 arrays × 13 entries each, providing slot-level lookup.
 */

/**
 * @brief Slot name lookup — maps slot offset (0-12) to short name.
 *
 * BOUNDS USED:
 *   - CPISI_META_COUNT = 13 = CPISI_BRIDGE (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: 13 metadata slots organized by category
 *   [0] RESERVED — unclassified (category 0)
 *   [1-4] CONFIG — VERSION, CONFIG, STATE, INIT (category 1)
 *   [5-8] RESOURCE — MEMORY, TIMEOUT, PERMISSION, RESOURCE (category 2)
 *   [9-12] SYSTEM — ASSERT, PANIC, UNIMPLEMENTED, FATAL (category 3)
 *
 * USE CASE: Error slot identification, logging, UI display.
 */
static const char* const g_cpisi_meta_name[CPISI_META_COUNT] = {
    "RESERVED",       /*  [0] X:0 — unclassified      */
    "VERSION",        /*  [1] C:1 — version mismatch  */
    "CONFIG",         /*  [2] C:2 — config error      */
    "STATE",          /*  [3] C:3 — state error       */
    "INIT",           /*  [4] C:4 — init failure      */
    "MEMORY",         /*  [5] R:1 — memory exhausted  */
    "TIMEOUT",        /*  [6] R:2 — timeout           */
    "PERMISSION",     /*  [7] R:3 — permission denied */
    "RESOURCE",       /*  [8] R:4 — resource busy     */
    "ASSERT",         /*  [9] S:1 — assertion failed  */
    "PANIC",          /* [10] S:2 — panic/abort       */
    "UNIMPLEMENTED",  /* [11] S:3 — not implemented   */
    "FATAL"           /* [12] S:4 — fatal failure     */
};

/**
 * @brief Slot description lookup — maps slot offset (0-12) to human-readable description.
 *
 * BOUNDS USED:
 *   - CPISI_META_COUNT = 13 = CPISI_BRIDGE (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Human-readable error descriptions for each slot.
 *
 * RELATIONSHIP: Parallel to g_cpisi_meta_name (same index = same slot).
 *
 * USE CASE: Error messages, user-facing diagnostics, logging detail.
 */
static const char* const g_cpisi_meta_desc[CPISI_META_COUNT] = {
    "Unknown or unclassified error",         /*  [0] RESERVED       */
    "Version mismatch detected",             /*  [1] VERSION        */
    "Configuration invalid or missing",      /*  [2] CONFIG         */
    "State corrupt or inconsistent",         /*  [3] STATE          */
    "Initialization failed",                 /*  [4] INIT           */
    "Memory exhausted or allocation failed", /*  [5] MEMORY         */
    "Operation timed out",                   /*  [6] TIMEOUT        */
    "Permission denied",                     /*  [7] PERMISSION     */
    "Resource unavailable or busy",          /*  [8] RESOURCE       */
    "Assertion failed",                      /*  [9] ASSERT         */
    "Panic or abort triggered",              /* [10] PANIC          */
    "Feature not implemented",               /* [11] UNIMPLEMENTED  */
    "Fatal system failure"                   /* [12] FATAL          */
};

/* ═══════════════════════════════════════════════════════════════════════════
 * BOUNDS TABLES (3 × 4 arrays)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Minimum, maximum, and center slot offsets per metadata category.
 *
 * 3 arrays × 4 entries each, providing category bounds lookup.
 */

/**
 * @brief Slot minimum per category — maps category (0-3) to first slot offset.
 *
 * BOUNDS USED:
 *   - CPISI_META_CAT_COUNT = 4 (from S.2a.2 TIER 2) — array size
 *   - CPISI_META_SLOT_* constants (from S.2a.2 TIER 2) — slot boundaries
 *
 * DERIVATION:
 *   [0] RESERVED = 0 (single slot)
 *   [1] CONFIG = 1 (slots 1-4)
 *   [2] RESOURCE = 5 (slots 5-8)
 *   [3] SYSTEM = 9 (slots 9-12)
 *
 * USE CASE: Category iteration, range checking, slot-to-category mapping.
 */
static const uint8_t g_cpisi_meta_slot_min[CPISI_META_CAT_COUNT] = {
    CPISI_META_SLOT_RESERVED,      /* [0] RESERVED = 0  */
    CPISI_META_SLOT_CONFIG_MIN,    /* [1] CONFIG = 1    */
    CPISI_META_SLOT_RESOURCE_MIN,  /* [2] RESOURCE = 5  */
    CPISI_META_SLOT_SYSTEM_MIN     /* [3] SYSTEM = 9    */
};

/**
 * @brief Slot maximum per category — maps category (0-3) to last slot offset.
 *
 * BOUNDS USED:
 *   - CPISI_META_CAT_COUNT = 4 (from S.2a.2 TIER 2) — array size
 *   - CPISI_META_SLOT_* constants (from S.2a.2 TIER 2) — slot boundaries
 *
 * DERIVATION:
 *   [0] RESERVED = 0 (single slot, min=max)
 *   [1] CONFIG = 4 (4 slots: 1,2,3,4)
 *   [2] RESOURCE = 8 (4 slots: 5,6,7,8)
 *   [3] SYSTEM = 12 (4 slots: 9,10,11,12)
 *
 * USE CASE: Category iteration, range checking.
 */
static const uint8_t g_cpisi_meta_slot_max[CPISI_META_CAT_COUNT] = {
    CPISI_META_SLOT_RESERVED,      /* [0] RESERVED = 0  */
    CPISI_META_SLOT_CONFIG_MAX,    /* [1] CONFIG = 4    */
    CPISI_META_SLOT_RESOURCE_MAX,  /* [2] RESOURCE = 8  */
    CPISI_META_SLOT_SYSTEM_MAX     /* [3] SYSTEM = 12   */
};

/**
 * @brief Slot center per category — maps category (0-3) to center slot offset.
 *
 * BOUNDS USED:
 *   - CPISI_META_CAT_COUNT = 4 (from S.2a.2 TIER 2) — array size
 *   - CPISI_META_SLOT_*_CENTER constants (from S.2a.2 TIER 2) — centers
 *
 * DERIVATION:
 *   [0] RESERVED = 0 (single slot, is its own center)
 *   [1] CONFIG = 2 (center of 1-4 → STATE slot)
 *   [2] RESOURCE = 6 (center of 5-8 → TIMEOUT slot)
 *   [3] SYSTEM = 10 (center of 9-12 → PANIC slot)
 *
 * USE CASE: Default slot selection, center-based navigation.
 */
static const uint8_t g_cpisi_meta_slot_center[CPISI_META_CAT_COUNT] = {
    CPISI_META_SLOT_RESERVED,        /* [0] RESERVED = 0  */
    CPISI_META_SLOT_CONFIG_CENTER,   /* [1] CONFIG = 2    */
    CPISI_META_SLOT_RESOURCE_CENTER, /* [2] RESOURCE = 6  */
    CPISI_META_SLOT_SYSTEM_CENTER    /* [3] SYSTEM = 10   */
};

/** @} */ /* end S.2d.7a TABLES */

/** @name S.2d.7b ACCESSORS — Metadata table access macros
 *  @brief Type-safe access macros for metadata domain lookup tables (11 macros).
 *
 *  ACCESSOR GROUPS (11 total):
 *    CATEGORY (4): cpisi_meta_cat_name/abbrev/version/balanced()
 *    SLOT (2):     cpisi_meta_name/desc()
 *    BOUNDS (3):   cpisi_meta_slot_min/max/center()
 *    CODE (2):     cpisi_meta_code_name/desc() — lookup by full code (243-255)
 *
 *  @{ */

/* ═══════════════════════════════════════════════════════════════════════════
 * CATEGORY ACCESSORS (4 macros)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Get category name — INDEX (0-3) → full category name string.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, CPISI_META_CAT_COUNT-1] = [0, 3]
 *   - OUTPUT: const char* ∈ {"RESERVED", "CONFIG", "RESOURCE", "SYSTEM"}
 *
 * FORMULA: Direct table lookup — g_cpisi_meta_cat_name[cat]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 3.
 *
 * EXAMPLE:
 *   cpisi_meta_cat_name(0) → "RESERVED"
 *   cpisi_meta_cat_name(1) → "CONFIG"
 *   cpisi_meta_cat_name(2) → "RESOURCE"
 *   cpisi_meta_cat_name(3) → "SYSTEM"
 *
 * @param cat Category index 0-3 (unchecked)
 * @return Category name string
 */
#define cpisi_meta_cat_name(cat)        (g_cpisi_meta_cat_name[(cat)])

/**
 * @brief Get category abbreviation — INDEX (0-3) → single char.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, CPISI_META_CAT_COUNT-1] = [0, 3]
 *   - OUTPUT: char ∈ {'X', 'C', 'R', 'S'}
 *
 * FORMULA: Direct table lookup — g_cpisi_meta_cat_abbrev[cat]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 3.
 *
 * EXAMPLE:
 *   cpisi_meta_cat_abbrev(0) → 'X'
 *   cpisi_meta_cat_abbrev(1) → 'C'
 *   cpisi_meta_cat_abbrev(2) → 'R'
 *   cpisi_meta_cat_abbrev(3) → 'S'
 *
 * @param cat Category index 0-3 (unchecked)
 * @return Category abbreviation char
 */
#define cpisi_meta_cat_abbrev(cat)      (g_cpisi_meta_cat_abbrev[(cat)])

/**
 * @brief Get category version char — INDEX (0-3) → living version char.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, CPISI_META_CAT_COUNT-1] = [0, 3]
 *   - OUTPUT: char ∈ {'x', 'a', 'b', 'c'}
 *
 * FORMULA: Direct table lookup — g_cpisi_meta_cat_version[cat]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 3.
 *
 * EXAMPLE:
 *   cpisi_meta_cat_version(0) → 'x' (unknown)
 *   cpisi_meta_cat_version(1) → 'a' (alpha)
 *   cpisi_meta_cat_version(2) → 'b' (beta)
 *   cpisi_meta_cat_version(3) → 'c' (stable)
 *
 * @param cat Category index 0-3 (unchecked)
 * @return Version char
 */
#define cpisi_meta_cat_version(cat)     (g_cpisi_meta_cat_version[(cat)])

/**
 * @brief Get category balanced value — INDEX (0-3) → ternary value.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, CPISI_META_CAT_COUNT-1] = [0, 3]
 *   - OUTPUT: int8_t ∈ {0, -1, 0, +1}
 *
 * FORMULA: Direct table lookup — g_cpisi_meta_cat_balanced[cat]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 3.
 *
 * EXAMPLE:
 *   cpisi_meta_cat_balanced(0) →  0 (RESERVED)
 *   cpisi_meta_cat_balanced(1) → -1 (CONFIG)
 *   cpisi_meta_cat_balanced(2) →  0 (RESOURCE)
 *   cpisi_meta_cat_balanced(3) → +1 (SYSTEM)
 *
 * @param cat Category index 0-3 (unchecked)
 * @return Balanced ternary value
 */
#define cpisi_meta_cat_balanced(cat)    (g_cpisi_meta_cat_balanced[(cat)])

/* ═══════════════════════════════════════════════════════════════════════════
 * SLOT ACCESSORS (2 macros)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Get slot name by offset — OFFSET (0-12) → slot name string.
 *
 * BOUNDS:
 *   - INPUT:  offset ∈ [0, CPISI_META_COUNT-1] = [0, 12]
 *   - OUTPUT: const char* (slot name)
 *
 * FORMULA: Direct table lookup — g_cpisi_meta_name[offset]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ offset ≤ 12.
 *
 * EXAMPLE:
 *   cpisi_meta_name(0)  → "RESERVED"
 *   cpisi_meta_name(5)  → "MEMORY"
 *   cpisi_meta_name(12) → "FATAL"
 *
 * @param offset Slot offset 0-12 (unchecked)
 * @return Slot name string
 */
#define cpisi_meta_name(offset)         (g_cpisi_meta_name[(offset)])

/**
 * @brief Get slot description by offset — OFFSET (0-12) → description string.
 *
 * BOUNDS:
 *   - INPUT:  offset ∈ [0, CPISI_META_COUNT-1] = [0, 12]
 *   - OUTPUT: const char* (human-readable description)
 *
 * FORMULA: Direct table lookup — g_cpisi_meta_desc[offset]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ offset ≤ 12.
 *
 * EXAMPLE:
 *   cpisi_meta_desc(0)  → "Unknown or unclassified error"
 *   cpisi_meta_desc(5)  → "Memory exhausted or allocation failed"
 *   cpisi_meta_desc(12) → "Fatal system failure"
 *
 * @param offset Slot offset 0-12 (unchecked)
 * @return Slot description string
 */
#define cpisi_meta_desc(offset)         (g_cpisi_meta_desc[(offset)])

/* ═══════════════════════════════════════════════════════════════════════════
 * BOUNDS ACCESSORS (3 macros)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Get slot minimum for category — CATEGORY (0-3) → first slot offset.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, CPISI_META_CAT_COUNT-1] = [0, 3]
 *   - OUTPUT: uint8_t ∈ {0, 1, 5, 9}
 *
 * FORMULA: Direct table lookup — g_cpisi_meta_slot_min[cat]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 3.
 *
 * EXAMPLE:
 *   cpisi_meta_slot_min(0) → 0 (RESERVED)
 *   cpisi_meta_slot_min(1) → 1 (CONFIG)
 *   cpisi_meta_slot_min(2) → 5 (RESOURCE)
 *   cpisi_meta_slot_min(3) → 9 (SYSTEM)
 *
 * @param cat Category index 0-3 (unchecked)
 * @return First slot offset for category
 */
#define cpisi_meta_slot_min(cat)        (g_cpisi_meta_slot_min[(cat)])

/**
 * @brief Get slot maximum for category — CATEGORY (0-3) → last slot offset.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, CPISI_META_CAT_COUNT-1] = [0, 3]
 *   - OUTPUT: uint8_t ∈ {0, 4, 8, 12}
 *
 * FORMULA: Direct table lookup — g_cpisi_meta_slot_max[cat]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 3.
 *
 * EXAMPLE:
 *   cpisi_meta_slot_max(0) → 0  (RESERVED)
 *   cpisi_meta_slot_max(1) → 4  (CONFIG)
 *   cpisi_meta_slot_max(2) → 8  (RESOURCE)
 *   cpisi_meta_slot_max(3) → 12 (SYSTEM)
 *
 * @param cat Category index 0-3 (unchecked)
 * @return Last slot offset for category
 */
#define cpisi_meta_slot_max(cat)        (g_cpisi_meta_slot_max[(cat)])

/**
 * @brief Get slot center for category — CATEGORY (0-3) → center slot offset.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, CPISI_META_CAT_COUNT-1] = [0, 3]
 *   - OUTPUT: uint8_t ∈ {0, 2, 6, 10}
 *
 * FORMULA: Direct table lookup — g_cpisi_meta_slot_center[cat]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 3.
 *
 * EXAMPLE:
 *   cpisi_meta_slot_center(0) → 0  (RESERVED)
 *   cpisi_meta_slot_center(1) → 2  (CONFIG → STATE)
 *   cpisi_meta_slot_center(2) → 6  (RESOURCE → TIMEOUT)
 *   cpisi_meta_slot_center(3) → 10 (SYSTEM → PANIC)
 *
 * @param cat Category index 0-3 (unchecked)
 * @return Center slot offset for category
 */
#define cpisi_meta_slot_center(cat)     (g_cpisi_meta_slot_center[(cat)])

/* ═══════════════════════════════════════════════════════════════════════════
 * CODE ACCESSORS (2 macros)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Get slot name by metadata code — CODE (243-255) → slot name string.
 *
 * BOUNDS:
 *   - INPUT:  code ∈ [CPISI_PENTERACT, CPISI_PENTERACT+12] = [243, 255]
 *   - OUTPUT: const char* (slot name)
 *
 * FORMULA: g_cpisi_meta_name[CPISI_META_OFFSET(code)] where offset = code - 243
 *
 * NO BOUNDS CHECK: Caller must ensure 243 ≤ code ≤ 255.
 *
 * EXAMPLE:
 *   cpisi_meta_code_name(243) → "RESERVED"
 *   cpisi_meta_code_name(248) → "MEMORY"
 *   cpisi_meta_code_name(255) → "FATAL"
 *
 * @param code Metadata code 243-255 (unchecked)
 * @return Slot name string
 */
#define cpisi_meta_code_name(code)      (g_cpisi_meta_name[CPISI_META_OFFSET(code)])

/**
 * @brief Get slot description by metadata code — CODE (243-255) → description.
 *
 * BOUNDS:
 *   - INPUT:  code ∈ [CPISI_PENTERACT, CPISI_PENTERACT+12] = [243, 255]
 *   - OUTPUT: const char* (human-readable description)
 *
 * FORMULA: g_cpisi_meta_desc[CPISI_META_OFFSET(code)] where offset = code - 243
 *
 * NO BOUNDS CHECK: Caller must ensure 243 ≤ code ≤ 255.
 *
 * EXAMPLE:
 *   cpisi_meta_code_desc(243) → "Unknown or unclassified error"
 *   cpisi_meta_code_desc(248) → "Memory exhausted or allocation failed"
 *   cpisi_meta_code_desc(255) → "Fatal system failure"
 *
 * @param code Metadata code 243-255 (unchecked)
 * @return Slot description string
 */
#define cpisi_meta_code_desc(code)      (g_cpisi_meta_desc[CPISI_META_OFFSET(code)])

/** @} */ /* end S.2d.7b ACCESSORS */

/** @name S.2d.7c CONVERSION — Metadata conversion utilities
 *  @brief [Reserved: Metadata uses direct lookup, not format conversion]
 *
 *  Metadata slots use direct table lookup (S.2d.7a) via accessor macros (S.2d.7b).
 *  No conversion utilities needed — slots map directly to indices.
 *
 *  FUTURE: If string→slot parsing needed, add here.
 *  @{ */

/* No conversion utilities — metadata uses direct lookup */

/** @} */ /* end S.2d.7c CONVERSION */

/** @} */ /* end S.2d.7 METADATA */

/** @} */ /* end S.2d LOOKUP */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e TYPES — Metadata enums (CpisiMetaCategory, CpisiMetaSlot) [TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_metadata_types S.2e METADATA TYPES — Type Definitions
 * @ingroup cpisi_metadata_domain
 * @brief Type-safe enums for metadata slot system (DOMAIN-SPECIFIC).
 *
 * PURPOSE: Type definitions for metadata domain.
 *
 * CONTAINS:
 *   S.2e.7a CATEGORY — CpisiMetaCategory (4 values: Reserved, Config, ...)
 *   S.2e.7b SLOT     — CpisiMetaSlot (13 values: specific error slots)
 *
 * Builds FROM: S.2d LOOKUP (runtime tables)
 * Builds TO:   S.3 CLOSING (file end)
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  S.2e.7 METADATA DOMAIN — Error Metadata Enumerations (2 subsections)      │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  MATHEMATICAL FOUNDATION — 13 Metadata Slots (BRIDGE):                     │
 * │                                                                            │
 * │    Error codes 243-255 = PENTERACT to BINARY_FIT-1                         │
 * │    13 slots = CPISI_BRIDGE = 1 + 4 + 4 + 4 organization:                   │
 * │                                                                            │
 * │    Category 0 (RESERVED):  slot 0  (1 slot)                                │
 * │    Category 1 (CONFIG):    slots 1-4  (4 slots: version, config, state, init)│
 * │    Category 2 (RESOURCE):  slots 5-8  (4 slots: memory, timeout, perm, res)│
 * │    Category 3 (SYSTEM):    slots 9-12 (4 slots: assert, panic, unimp, fatal)│
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.7):                                          │
 * │    - g_cpisi_meta_cat_*[4] tables (name, abbrev, version, balanced)        │
 * │    - g_cpisi_meta_name[13], g_cpisi_meta_desc[13] — slot info              │
 * │    - g_cpisi_meta_slot_min/max/center[4] — category bounds                 │
 * │    - cpisi_meta_*() accessors for runtime lookup                           │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2e_7a_META_CATEGORY S.2e.7a CpisiMetaCategory — Category enum
 * @brief Type-safe metadata category — RESERVED(0), CONFIG(1), RESOURCE(2), SYSTEM(3).
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — 4 Metadata Categories:                              │
 * │                                                                            │
 * │    RESERVED (0) — outside balanced ternary (1 slot: unknown/unclassified)  │
 * │    CONFIG   (1) — configuration errors (4 slots, bal=-1)                   │
 * │    RESOURCE (2) — resource errors (4 slots, bal=0) ← CENTER                │
 * │    SYSTEM   (3) — system errors (4 slots, bal=+1)                          │
 * │                                                                            │
 * │  Slot distribution: 1 + 4 + 4 + 4 = 13 = CPISI_BRIDGE                      │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup cpisi_metadata_types
 * @{ */

/**
 * @brief Category enumeration for metadata error classification.
 *
 * BOUNDS USED:
 *   - CPISI_META_CAT_RESERVED (0), CONFIG (1), RESOURCE (2), SYSTEM (3)
 *   - CPISI_META_CAT_COUNT (4) — iteration bound
 *   - CPISI_META_CAT_RESOURCE (2) — effective CENTER (RESERVED is outside ternary)
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for the 4 metadata error categories. Categories 1-3 form
 *   a balanced ternary group (config/resource/system), with category 0 reserved
 *   for unclassified errors. Slot distribution: 1 + 4 + 4 + 4 = 13 = BRIDGE.
 *
 * VALUE MAPPING:
 *   CPISI_META_CAT_ENUM_RESERVED (0) — reserved, unknown/unclassified (1 slot)
 *   CPISI_META_CAT_ENUM_CONFIG (1) — configuration errors (4 slots, bal=-1)
 *   CPISI_META_CAT_ENUM_RESOURCE (2) — resource errors (4 slots, bal=0, CENTER)
 *   CPISI_META_CAT_ENUM_SYSTEM (3) — system errors (4 slots, bal=+1)
 *   CPISI_META_CAT_ENUM_COUNT (4) — iteration bound
 *
 * USE CASE:
 *   - Category classification: cat = cpisi_meta_category(slot);
 *   - Category iteration: for (cat = 0; cat < CPISI_META_CAT_ENUM_COUNT; cat++)
 *   - Slot bounds: min = cpisi_meta_slot_min(cat); max = cpisi_meta_slot_max(cat);
 *
 * RELATIONSHIP TO LOOKUP (S.2d.7a):
 *   - cpisi_meta_cat_name(idx) → "RESERVED", "CONFIG", "RESOURCE", "SYSTEM"
 *   - cpisi_meta_cat_balanced(idx) → 0, -1, 0, +1 (RESERVED is neutral)
 *   - cpisi_meta_slot_min/max/center(cat) → slot bounds per category
 */
typedef enum CpisiMetaCategory {
    CPISI_META_CAT_ENUM_RESERVED = CPISI_META_CAT_RESERVED,  /**< 0: Reserved — unknown/unclassified */
    CPISI_META_CAT_ENUM_CONFIG   = CPISI_META_CAT_CONFIG,    /**< 1: Config — configuration errors */
    CPISI_META_CAT_ENUM_RESOURCE = CPISI_META_CAT_RESOURCE,  /**< 2: Resource — resource errors */
    CPISI_META_CAT_ENUM_SYSTEM   = CPISI_META_CAT_SYSTEM,    /**< 3: System — system-level errors */
    CPISI_META_CAT_ENUM_COUNT    = CPISI_META_CAT_COUNT      /**< 4: Iteration bound */
} CpisiMetaCategory;


/** @} */ /* end S.2e.7a META_CATEGORY */

/**
 * @defgroup S2e_7b_META_SLOT S.2e.7b CpisiMetaSlot — Slot enum
 * @brief Type-safe metadata slot — 13 specific error type indices.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — 13 Metadata Slots (codes 243-255):                  │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RESERVED (X):                                                             │
 * │     0: RESERVED — unknown or unclassified error (code 243)                 │
 * │                                                                            │
 * │  CONFIG (C):                                                               │
 * │     1: VERSION — version mismatch (code 244)                               │
 * │     2: CONFIG — configuration error (code 245)                             │
 * │     3: STATE — invalid state (code 246) ← category center                  │
 * │     4: INIT — initialization failure (code 247)                            │
 * │                                                                            │
 * │  RESOURCE (R):                                                             │
 * │     5: MEMORY — memory exhausted (code 248)                                │
 * │     6: TIMEOUT — operation timeout (code 249) ← category center            │
 * │     7: PERMISSION — permission denied (code 250)                           │
 * │     8: RESOURCE — resource unavailable (code 251)                          │
 * │                                                                            │
 * │  SYSTEM (S):                                                               │
 * │     9: ASSERT — assertion failed (code 252)                                │
 * │    10: PANIC — panic triggered (code 253) ← category center                │
 * │    11: UNIMPLEMENTED — not implemented (code 254)                          │
 * │    12: FATAL — fatal system failure (code 255)                             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup cpisi_metadata_types
 * @{ */

/**
 * @brief Slot enumeration for the 13 metadata error types.
 *
 * BOUNDS USED:
 *   - CPISI_META_SLOT_RESERVED (0) through CPISI_META_SLOT_FATAL (12)
 *   - CPISI_META_COUNT (13) — iteration bound = CPISI_BRIDGE
 *   - Error codes 243-255 = PENTERACT + slot index
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for the 13 metadata error slots (codes 243-255). The
 *   slots are organized as 1 + 4 + 4 + 4 = 13 across 4 categories. Provides
 *   type-safe indexing into metadata lookup tables.
 *
 * VALUE MAPPING:
 *   RESERVED category (1 slot):
 *     CPISI_META_SLOT_ENUM_RESERVED (0) — code 243, unknown/unclassified
 *   CONFIG category (4 slots):
 *     CPISI_META_SLOT_ENUM_VERSION (1) — code 244, version mismatch
 *     CPISI_META_SLOT_ENUM_CONFIG (2) — code 245, configuration error
 *     CPISI_META_SLOT_ENUM_STATE (3) — code 246, invalid state (center)
 *     CPISI_META_SLOT_ENUM_INIT (4) — code 247, initialization failure
 *   RESOURCE category (4 slots):
 *     CPISI_META_SLOT_ENUM_MEMORY (5) — code 248, memory exhausted
 *     CPISI_META_SLOT_ENUM_TIMEOUT (6) — code 249, operation timeout (center)
 *     CPISI_META_SLOT_ENUM_PERMISSION (7) — code 250, permission denied
 *     CPISI_META_SLOT_ENUM_RESOURCE (8) — code 251, resource unavailable
 *   SYSTEM category (4 slots):
 *     CPISI_META_SLOT_ENUM_ASSERT (9) — code 252, assertion failed
 *     CPISI_META_SLOT_ENUM_PANIC (10) — code 253, panic triggered (center)
 *     CPISI_META_SLOT_ENUM_UNIMPLEMENTED (11) — code 254, not implemented
 *     CPISI_META_SLOT_ENUM_FATAL (12) — code 255, fatal system failure
 *   CPISI_META_SLOT_ENUM_COUNT (13) — iteration bound
 *
 * USE CASE:
 *   - Slot lookup: name = cpisi_meta_slot_name(slot);
 *   - Code conversion: code = CPISI_PENTERACT + slot;
 *   - Reverse lookup: slot = CPISI_META_OFFSET(code);
 *
 * RELATIONSHIP TO LOOKUP (S.2d.7b):
 *   - cpisi_meta_slot_name(idx) → "RESERVED", "VERSION", ..., "FATAL"
 *   - cpisi_meta_code_name(code) → slot name by error code
 *   - cpisi_meta_code_desc(code) → human-readable description
 */
typedef enum CpisiMetaSlot {
    CPISI_META_SLOT_ENUM_RESERVED      = CPISI_META_SLOT_RESERVED,      /**<  0: Unknown/unclassified (code 243) */
    CPISI_META_SLOT_ENUM_VERSION       = CPISI_META_SLOT_VERSION,       /**<  1: Version mismatch (code 244) */
    CPISI_META_SLOT_ENUM_CONFIG        = CPISI_META_SLOT_CONFIG,        /**<  2: Configuration error (code 245) */
    CPISI_META_SLOT_ENUM_STATE         = CPISI_META_SLOT_STATE,         /**<  3: Invalid state (code 246) */
    CPISI_META_SLOT_ENUM_INIT          = CPISI_META_SLOT_INIT,          /**<  4: Initialization failure (code 247) */
    CPISI_META_SLOT_ENUM_MEMORY        = CPISI_META_SLOT_MEMORY,        /**<  5: Memory exhausted (code 248) */
    CPISI_META_SLOT_ENUM_TIMEOUT       = CPISI_META_SLOT_TIMEOUT,       /**<  6: Operation timeout (code 249) */
    CPISI_META_SLOT_ENUM_PERMISSION    = CPISI_META_SLOT_PERMISSION,    /**<  7: Permission denied (code 250) */
    CPISI_META_SLOT_ENUM_RESOURCE      = CPISI_META_SLOT_RESOURCE,      /**<  8: Resource unavailable (code 251) */
    CPISI_META_SLOT_ENUM_ASSERT        = CPISI_META_SLOT_ASSERT,        /**<  9: Assertion failed (code 252) */
    CPISI_META_SLOT_ENUM_PANIC         = CPISI_META_SLOT_PANIC,         /**< 10: Panic triggered (code 253) */
    CPISI_META_SLOT_ENUM_UNIMPLEMENTED = CPISI_META_SLOT_UNIMPLEMENTED, /**< 11: Not implemented (code 254) */
    CPISI_META_SLOT_ENUM_FATAL         = CPISI_META_SLOT_FATAL,         /**< 12: Fatal system failure (code 255) */
    CPISI_META_SLOT_ENUM_COUNT         = CPISI_META_COUNT               /**< 13: Iteration bound */
} CpisiMetaSlot;


/** @} */ /* end S.2e.7b META_SLOT */

/** @} */ /* end S.2e TYPES */

/** @} */ /* end S.2 DEFINES */

/** S.3 TYPES — [Reserved: metadata types in S.2e] @defgroup cpisi_metadata_types S.3 TYPES @{ @} */

/** S.4 PROTOTYPES — [Reserved: accessors inline in S.2d] @defgroup cpisi_metadata_prototypes S.4 PROTOTYPES @{ @} */

/** S.5 FILE-LEVEL — [Reserved: no file-level state] @defgroup cpisi_metadata_file S.5 FILE-LEVEL @{ @} */

/** S.6 OMISSION — [Reserved: S.3-S.5 empty, types in S.2e] @defgroup cpisi_metadata_omission S.6 OMISSION @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief [Reserved: metadata complete in S.2 — no runtime code needed]
///
/// All metadata types in S.2e, accessors in S.2d. BODY reserved for future runtime.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/** B.1 ORG CHART — [Reserved: types=S.2e, accessors=S.2d, functions=0] @defgroup cpisi_metadata_orgchart B.1 ORG CHART @{ @} */
/** B.2 TYPES — [Reserved: metadata types in S.2e] @defgroup cpisi_metadata_b2_types B.2 TYPES @{ @} */
/** B.3 HELPERS — [Reserved: metadata accessors in S.2d] @defgroup cpisi_metadata_helpers B.3 HELPERS @{ @} */
/** B.4 ERRORS — [Reserved: error handling in error.h] @defgroup cpisi_metadata_errors B.4 ERRORS @{ @} */
/** B.5 PUBLIC API — [Reserved: accessors inline in S.2d] @defgroup cpisi_metadata_api B.5 PUBLIC API @{ @} */
/** B.6 OMISSION — [Reserved: BODY empty, S.2 complete] @defgroup cpisi_metadata_b6_omission B.6 OMISSION @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Guidance for maintaining this file.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Compile-time Verification [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_x1_validation X.1 VALIDATION — Compile-time Verification
 * @{
 *
 * VALIDATION NOTES:
 *   - Static assertions in S.2c VERIFY catch metadata errors at compile time
 *   - BRIDGE equation verified: 243 + 13 = 256
 *   - Slot/category ranges verified
 *
 * @} end X.1 VALIDATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Post-include Checks [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_x2_execution X.2 EXECUTION — Post-include Checks
 * @{
 *
 * POST-INCLUDE CHECKS:
 *   - Verify error.h included (provides types.h transitively)
 *   - All metadata macros resolve to expected values
 *
 * @} end X.2 EXECUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Resource Management [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_x3_cleanup X.3 CLEANUP — Resource Management
 * @{
 *
 * CLEANUP NOTES:
 *   - No dynamic resources — metadata is compile-time/static only
 *   - Lookup tables are static const — no cleanup needed
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
 * @defgroup cpisi_metadata_x4_policy X.4 POLICY — Modification Guidance
 * @{
 *
 * MODIFICATION POLICY:
 *   - Metadata slots follow BRIDGE equation (13 slots)
 *   - Adding slots: update S.2a bounds, S.2c tripwires, S.2d/S.2e tables
 *   - Template: bereshit/word/seed/code/c/type-header.h
 *
 * @} end X.4 POLICY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — Future Expansion [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_x5_extension X.5 EXTENSION — Future Expansion
 * @{
 *
 * EXTENSION POINTS:
 *   - New slots: Update S.2a, S.2c, S.2d, S.2e
 *   - Runtime functions: Add to BODY B.3 HELPERS
 *   - Always add tripwires for new metadata values
 *
 * @} end X.5 EXTENSION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Issues [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_x6_troubleshoot X.6 TROUBLESHOOT — Common Issues
 * @{
 *
 * COMMON ISSUES:
 *   Q: "Static assert failed on BRIDGE equation"
 *   A: Check META_COUNT = 13, PENTERACT = 243, BYTE = 256
 *
 *   Q: "Slot index out of range"
 *   A: Use CPISI_META_VALID_OFFSET() before table access
 *
 * @} end X.6 TROUBLESHOOT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Related Documentation [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_x7_reference X.7 REFERENCE — Related Documentation
 * @{
 *
 * RELATED FILES:
 *   - types.h: Foundation types (BOUNDS, PRIMITIVES)
 *   - config/error.h: Error code anchor (includes types.h)
 *   - dar.h: Data access records using metadata
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
 * @defgroup cpisi_metadata_x8_note X.8 NOTE — Implementation Notes
 * @{
 *
 * IMPLEMENTATION NOTES:
 *   - BRIDGE equation: 243 + 13 = 256 (PENTERACT + BRIDGE = BYTE)
 *   - 13 slots across 4 categories: Reserved, Config, Resource, System
 *   - Codes 243-255 are metadata slots (error metadata range)
 *
 * @} end X.8 NOTE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Intentional Exclusions [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_metadata_x9_omission X.9 OMISSION — Intentional Exclusions
 * @{
 *
 * INTENTIONAL EXCLUSIONS:
 *   - No dynamic allocation: All metadata ops are compile-time or lookup-based
 *   - No runtime functions: Metadata is static const tables
 *   - BODY reserved: Metadata logic complete in S.2
 *
 * @} end X.9 OMISSION
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_TYPES_METADATA_H */
