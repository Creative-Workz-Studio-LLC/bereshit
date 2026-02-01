/// @file error.h
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
/// @omni:req  key        = CPISI-TYPES-ERROR
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
/// @omni:ins  layer      = types/error
/// @omni:ins  includes   = types.h
/// @omni:ins  provides   = [S.2_ERROR]
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
/// @omni:req  title      = CPI-SI Error Domain
/// @omni:req  brief      = Error codes, categories, and severity levels
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
 * @defgroup cpisi_error_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup cpisi_error_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_error_special_config
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

#ifndef CPISI_TYPES_ERROR_H
/** @def CPISI_TYPES_ERROR_H
 *  @brief Include guard — prevents multiple inclusion of this error types header.
 */
#define CPISI_TYPES_ERROR_H

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
 * @file      error.h
 * @brief     CPI-SI Error Domain — 256 error codes (243 layer + 13 metadata).
 *
 * @defgroup cpisi_error_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-ERROR
 * Title:     CPI-SI Error Domain Types
 * Type:      Header (Domain)
 * Component: Rails — provides error codes for all system layers
 * Role:      Error code definitions for 9-layer architecture.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_state M.2 STATE — Lifecycle State
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
 * @defgroup cpisi_error_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_error_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/error/error.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Template:     bereshit/word/seed/code/c/type-header.h
 * Derives_from: types/types.h (foundation)
 * Extracted:    2026-01-21 from types.h S.3d ERROR section
 *
 * Builds FROM:  types.h (S.2a BOUNDS, S.2b MACROS)
 * Builds TO:    dar.h, cpisi.h (error handling consumers)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, error, domain, 243-code, bridge-equation, 9-layer
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide error codes organized by 9-layer architecture.
 * Core Design: 243 layer codes + 13 metadata codes = 256 (fits uint8_t).
 * Philosophy:  The BRIDGE equation — ternary meets binary.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Error Code Structure
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   LAYER × 27 + OFFSET = CODE
 *
 *   Layer 0 (KERNEL):    codes   0-26
 *   Layer 1 (SYSTEM):    codes  27-53
 *   Layer 2 (RUNTIME):   codes  54-80
 *   Layer 3 (FRAMEWORK): codes  81-107
 *   Layer 4 (HEALTH):    codes 108-134  ← CENTER
 *   Layer 5 (NETWORK):   codes 135-161
 *   Layer 6 (STORAGE):   codes 162-188
 *   Layer 7 (ENGINE):    codes 189-215
 *   Layer 8 (APP):       codes 216-242
 *   METADATA:            codes 243-255  (13 codes)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * The BRIDGE Equation
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   243 (PENTERACT) + 13 (BRIDGE) = 256 (BINARY_FIT)
 *   Ternary meets binary. Fits uint8_t perfectly.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Categories per Layer
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   Each layer's 27 codes divide into 3 categories × 9 positions:
 *   - DETECT (0-8):    Problem detection
 *   - ASSESS (9-17):   Problem evaluation
 *   - RESPOND (18-26): Problem response
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Proverbs 27:12 — "A prudent man foreseeth the evil"
 * Principle:  Error detection enables error prevention.
 * Anchor:     James 5:16 — "confess your faults" — acknowledge errors to fix them
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h (foundation types)
 *
 * What Uses This:
 *   - health/health.h — error reporting
 *   - dar.h — data access error handling
 *   - cpisi.h — main interface
 *
 * @see types.h for foundation types
 * @see S.2a.3 BRIDGE for the 243+13=256 equation
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-21) — Extracted from types.h S.3d ERROR section
 *   a-01.25 (2026-01-23) — PRAGMA/SPECIAL/METADATA M.1-M.10 alignment
 *   b-03.00 (2026-01-25) — type-header.h template alignment, professional docstrings,
 *                          reserved section collapse, CLOSING X.1-X.9
 *
 * Current:
 *   ✓ 4-block structure complete (METADATA → SETUP → BODY → CLOSING)
 *   ✓ 256 error codes defined (243 layer + 13 metadata)
 *   ✓ Reserved sections collapsed (template is authoritative)
 *   ✓ X.1-X.9 CLOSING guidance
 *
 * Planned:
 *   ⏳ Error message string table
 *   ⏳ Error to layer mapping functions
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
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - S.1 INCLUDES  — Foundation headers (types.h)
///   - S.2 DEFINES   — Error domain (Bounds, Macros, Verify, Lookup, Types)
///
/// Domain file structure: Foundation include (S.1) → Domain defines (S.2)
/// Flow: inherit foundation → add domain-specific content
///
/// Builds FROM: METADATA block (identity established)
/// Builds TO:   BODY block (if needed for domain functions)
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
 * @defgroup cpisi_error_includes S.1 INCLUDES — Header dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * PURPOSE: Establish all header dependencies in consistent order.
 *          Domain files inherit system headers from foundation.
 *
 * Builds FROM: SETUP block (configuration context)
 * Builds TO:   S.2 ERROR DEFINES (domain-specific constants)
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
 * @defgroup cpisi_error_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_error_includes
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
 * @defgroup cpisi_error_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_error_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * PROVIDES: Foundation types, macros, and error code infrastructure.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/types.h"  /**< Foundation: S.1 system headers + S.2 universal defines */
#include "kernel/types/config/cfg/log.cfg.h"       /**< Log types: LogLevel, ActionType for error severity */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_error_includes
 * @brief    External library headers (none for error domain).
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
 * @defgroup cpisi_error_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_error_includes
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
 * S.2 DEFINES — Error Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_domain S.2 DEFINES — Error Domain
 * @brief    243 layer codes + 13 metadata = 256 total (BRIDGE equation).
 *
 * PURPOSE: Define error-specific constants, operations, verification, and types.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: no domain-specific preludes]
 *   - S.2a  BOUNDS  — Error code geometry (243+13=256)
 *   - S.2b  MACROS  — Code extraction/construction operations
 *   - S.2c VERIFY  — Compile-time tripwires
 *   - S.2d LOOKUP  — Runtime lookup tables
 *
 * Builds FROM: S.1 INCLUDES (foundation dependencies)
 * Builds TO:   S.3 TYPES (error type definitions)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2.0 PRELUDE — [Reserved] [PRELUDE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_prelude S.2.0 ERROR PRELUDE — Domain Operations
 * @ingroup cpisi_error_domain
 * @brief    [Reserved: no domain-specific preludes]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Error domain uses foundation PRELUDE operations] */

/** @} */ /* end S.2.0 PRELUDE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BOUNDS — Error code geometry (243+13=256) [BOUNDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_bounds S.2a ERROR BOUNDS — Domain Constants
 * @ingroup cpisi_error_domain
 * @brief    Error code geometry: 243 layer codes + 13 metadata = 256.
 *
 * Builds FROM: S.2.0 PRELUDE
 * Builds TO:   S.2b MACROS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Counts — Error domain geometry constants
 *  @brief The 243+13=256 error code architecture: PENTERACT + BRIDGE = BINARY_FIT.
 *
 *  Error codes fit in a byte (0-255) with ternary structure:
 *    SUCCESS:  0 (VOID)
 *    LAYERS:   1-242 (9 layers × 27 codes)
 *    METADATA: 243-255 (13 slots completing byte)
 *  @{ */
#define CPISI_ERROR_SUCCESS         CPISI_VOID          /**< 0: THE success code [MIN]            */
#define CPISI_ERROR_PER_LAYER       CPISI_CUBE          /**< 27: codes per layer (3³)             */
#define CPISI_ERROR_PER_TIER        CPISI_FACE          /**< 9: codes per tier (3²)               */
#define CPISI_ERROR_TIER_COUNT      CPISI_TERNARY       /**< 3: tiers per layer                   */
#define CPISI_ERROR_LAYER_COUNT     CPISI_FACE          /**< 9: layers (3²)                       */
#define CPISI_ERROR_CODE_COUNT      CPISI_PENTERACT     /**< 243: total layer codes (3⁵)          */
/**
 * @brief 243: metadata region start — PENTERACT edge.
 * @par Derivation
 *      METADATA_MIN = PENTERACT = LAYER_COUNT × PER_LAYER = 9 × 27 = 243
 */
#define CPISI_ERROR_METADATA_MIN    CPISI_PENTERACT
/**
 * @brief 255: metadata region end — byte maximum.
 * @par Derivation
 *      METADATA_MAX = BYTE_MAX = BINARY_FIT - 1 = 255 [MAX]
 */
#define CPISI_ERROR_METADATA_MAX    CPISI_BYTE_MAX
#define CPISI_CAT_COUNT             CPISI_TERNARY       /**< 3: categories (CORE/EXT/SYS)         */
/** @} */ /* end Counts */

/** @name ERROR TIER VALUES — 3 tiers per layer
 *  @brief Tier indices for the 3-tier structure within each error layer.
 *
 *  Each layer has 27 codes organized into 3 tiers of 9 codes each:
 *    - CORE (0-8):   Fundamental errors — always needed
 *    - EXT  (9-17):  Extended errors — common scenarios
 *    - SYS  (18-26): System errors — internal/low-level
 *
 *  DERIVATION: tier_offset = tier × CPISI_FACE = tier × 9
 *
 *  Builds FROM: CPISI_FACE (9 codes per tier)
 *  Builds TO:   CPISI_MAKE_ERROR macro (tier component)
 *  @{ */

/** @brief CORE tier index.
 *  @details Tier 0: Fundamental errors (positions 0-8 within layer).
 *           These are the essential errors every system must handle. */
#define CPISI_TIER_CORE             0

/** @brief EXT tier index.
 *  @details Tier 1: Extended errors (positions 9-17 within layer).
 *           Common scenarios beyond the fundamentals. */
#define CPISI_TIER_EXT              1

/** @brief SYS tier index.
 *  @details Tier 2: System errors (positions 18-26 within layer).
 *           Internal and low-level system-specific errors. */
#define CPISI_TIER_SYS              2

/** @} */ /* end ERROR TIER VALUES */

/** @name ERROR POSITION VALUES — 9 semantic positions per tier
 *  @brief Universal position meanings that repeat across all layers and tiers.
 *
 *  Each tier has 9 positions (0-8) with UNIVERSAL semantic meaning.
 *  This means CPISI_ERR_*_NULL is ALWAYS position 1 in every layer/tier,
 *  enabling pattern-based error handling across domains.
 *
 *  POSITION MEANINGS:
 *    - 0: SUCCESS/NONE    — Success or no error of this category
 *    - 1: NULL/MISSING    — Null pointer or missing required item
 *    - 2: INVALID/MALFORM — Invalid parameter or malformed data
 *    - 3: BOUNDS/RANGE    — Out of bounds or range violation
 *    - 4: TYPE/FORMAT     — Type mismatch or format error
 *    - 5: STATE/CONDITION — Invalid state or condition not met
 *    - 6: INIT/SETUP      — Initialization or setup failure
 *    - 7: CLEANUP/TEARDOWN— Cleanup or teardown failure
 *    - 8: UNKNOWN/OTHER   — Unknown or uncategorized error
 *
 *  Builds FROM: Semantic error categories (universal across CPI-SI)
 *  Builds TO:   CPISI_MAKE_ERROR macro (position component)
 *  @{ */

/** @brief Position 0: Success or no error.
 *  @details The success state for this category. */
#define CPISI_POS_SUCCESS           0

/** @brief Position 1: Null or missing.
 *  @details Null pointer or missing required item. */
#define CPISI_POS_NULL              1

/** @brief Position 2: Invalid or malformed.
 *  @details Invalid parameter or malformed data. */
#define CPISI_POS_INVALID           2

/** @brief Position 3: Bounds or range.
 *  @details Out of bounds or range violation. */
#define CPISI_POS_BOUNDS            3

/** @brief Position 4: Type or format.
 *  @details Type mismatch or format error. */
#define CPISI_POS_TYPE              4

/** @brief Position 5: State or condition.
 *  @details Invalid state or condition not met. */
#define CPISI_POS_STATE             5

/** @brief Position 6: Init or setup.
 *  @details Initialization or setup failure. */
#define CPISI_POS_INIT              6

/** @brief Position 7: Cleanup or teardown.
 *  @details Cleanup or teardown failure. */
#define CPISI_POS_CLEANUP           7

/** @brief Position 8: Unknown or other.
 *  @details Unknown or uncategorized error. */
#define CPISI_POS_UNKNOWN           8

/** @} */ /* end ERROR POSITION VALUES */

/** @} */ /* end S.2a BOUNDS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2b MACROS — Error code operations [MACROS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_macros S.2b ERROR MACROS — Error code operations
 * @ingroup cpisi_error_domain
 * @brief    Macros for extracting, constructing, and validating error codes.
 *
 * PURPOSE: Provide error-specific operations for the 9-layer × 27-position
 *          error code system using BOUNDS from types.h.
 *
 * Builds FROM: types.h (S.2a BOUNDS, S.2b.2 PRIMITIVES), S.2a ERROR BOUNDS
 * Builds TO:   S.2c ERROR VERIFY (compile-time validation)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC extensions to S.2b MACROS defined in
 *   types.h. The foundation macros (QUOTIENT, MODULO, IN_RANGE) come from
 *   types.h. This file adds error code construction and extraction macros.
 *
 * STRUCTURE:
 *   S.2b.5  ERROR COMPOSITION — Error code construction from components
 *   S.2b.6  ERROR OPS         — Layer, category, and validation operations
 *   S.2b.7b ERROR DOMAIN      — Error code construction/extraction
 *   S.2b.8  ERROR LAYER BASES — Per-layer base error codes
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b.5 ERROR COMPOSITION — Error code construction [ERROR_COMPOSITION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_composition S.2b.5 ERROR COMPOSITION — Error code construction
 * @ingroup cpisi_error_macros
 * @brief    Macros for constructing error codes from layer, tier, and position.
 *
 * PURPOSE: Derive error codes from their structural components rather than
 *          hardcoding values. This enables scalable, config-driven error systems.
 *
 * DERIVATION FORMULA:
 *   error_code = (layer × CUBE) + (tier × FACE) + position
 *              = (layer × 27) + (tier × 9) + position
 *
 * EXAMPLE:
 *   CPISI_ERR_KERNEL_NULL = CPISI_MAKE_ERROR(KERNEL, CORE, NULL)
 *                        = (0 × 27) + (0 × 9) + 1 = 1
 *
 *   CPISI_ERR_HEALTH_BOUNDS = CPISI_MAKE_ERROR(HEALTH, CORE, BOUNDS)
 *                          = (4 × 27) + (0 × 9) + 3 = 111
 *
 * Builds FROM: S.2a ERROR BOUNDS (CUBE, FACE, tiers, positions)
 * Builds TO:   S.3 ERROR CODES (all error definitions use this)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* CPISI_MAKE_ERROR — inherited from types.h S.2b MACROS
 * Constructs error code: layer × 27 + tier × 9 + position
 * @see types.h CPISI_MAKE_ERROR */

/**
 * @brief Construct an error code from layer and offset (tier + position combined).
 *
 * @param layer  Layer index (0-8)
 * @param offset Offset within layer (0-26), combines tier and position
 *
 * @return Error code in range 0-242
 *
 * FORMULA: code = layer × CUBE + offset = layer × 27 + offset
 *
 * @note Use CPISI_MAKE_ERROR for fully-derived codes when tier/position are known.
 *       Use this when working with raw offsets.
 */
#define CPISI_MAKE_ERROR_OFFSET(layer, offset) \
    (((layer) * CPISI_CUBE) + (offset))

/**
 * @brief Extract layer index from error code.
 *
 * @param code Error code (0-242 for layer codes)
 * @return Layer index (0-8)
 *
 * FORMULA: layer = code / CUBE = code / 27
 */
#define CPISI_ERROR_LAYER(code)     ((code) / CPISI_CUBE)

/**
 * @brief Extract offset within layer from error code.
 *
 * @param code Error code (0-242 for layer codes)
 * @return Offset within layer (0-26)
 *
 * FORMULA: offset = code % CUBE = code % 27
 */
#define CPISI_ERROR_OFFSET(code)    ((code) % CPISI_CUBE)

/**
 * @brief Extract tier index from error code.
 *
 * @param code Error code (0-242 for layer codes)
 * @return Tier index (0-2): CORE, EXT, or SYS
 *
 * FORMULA: tier = (code % CUBE) / FACE = (code % 27) / 9
 */
#define CPISI_ERROR_TIER(code)      (((code) % CPISI_CUBE) / CPISI_FACE)

/**
 * @brief Extract position within tier from error code.
 *
 * @param code Error code (0-242 for layer codes)
 * @return Position index (0-8)
 *
 * FORMULA: position = code % FACE = code % 9
 */
#define CPISI_ERROR_POSITION(code)  ((code) % CPISI_FACE)

/** @} */ /* end S.2b.5 ERROR COMPOSITION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b.6 ERROR OPS — Operations on error code bounds [ERROR_OPS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_ops S.2b.6 ERROR OPS — Operations on error code bounds
 * @ingroup cpisi_error_macros
 * @brief    Layer, category, and validation operations for error codes.
 *
 * PURPOSE: Domain-specific operations for 243+13=256 error architecture.
 *
 * ERROR CODE MAP:
 * ```
 *   Code Range │ Layer │ Description
 *   ───────────┼───────┼─────────────────────
 *     0-26     │   0   │ KERNEL errors
 *    27-53     │   1   │ MEMORY errors
 *    54-80     │   2   │ PROCESS errors
 *    81-107    │   3   │ STORAGE errors
 *   108-134    │   4   │ HEALTH errors (center)
 *   135-161    │   5   │ NETWORK errors
 *   162-188    │   6   │ SECURITY errors
 *   189-215    │   7   │ SERVICES errors
 *   216-242    │   8   │ APP errors
 *   243-255    │  META │ Metadata codes (13)
 * ```
 *
 * STRUCTURE:
 *   S.2b.6a LAYER      — Per-layer error code ranges
 *   S.2b.6b CATEGORY   — Category offset calculation
 *   S.2b.6c VALIDATION — Error code range checks
 *
 * Builds FROM: S.2a ERROR BOUNDS (geometry constants)
 * Builds TO:   S.2b.7b ERROR DOMAIN (construction/extraction)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2b.6a LAYER OPS — Per-layer error code bounds
 *  @brief Calculate MIN/MAX/CENTER/HALF for each layer's error code range.
 *
 *  Each layer has CUBE (27) codes: layer n spans [n×27, (n+1)×27-1].
 *
 *  BOUNDS USED:
 *    CUBE = 27 (S.2a.3)    — codes per layer
 *    UNITY = 1 (S.2a.1)    — for offset calculations
 *
 *  FORMULA PATTERN:
 *    MIN(layer) = layer × CUBE = layer × 27
 *    MAX(layer) = (layer+1) × CUBE - 1 = MIN + 26
 *    CENTER(layer) = MIN + HALF(CUBE) = MIN + 13
 *    HALF(layer) = (MAX - MIN) / 2 = 13 (constant for all layers)
 *  @{ */

/**
 * @brief Get layer minimum error code.
 * @param layer Layer index (0-8)
 * @return layer × CUBE (e.g., layer 0 → 0, layer 4 → 108)
 *
 * FORMULA: MIN = layer × 27
 */
#define CPISI_ERROR_LAYER_MIN(layer)    CPISI_PRODUCT((layer), CPISI_CUBE)

/**
 * @brief Get layer maximum error code.
 * @param layer Layer index (0-8)
 * @return (layer+1) × CUBE - 1 (e.g., layer 0 → 26, layer 4 → 134)
 *
 * FORMULA: MAX = (layer+1) × 27 - 1 = MIN + 26
 */
#define CPISI_ERROR_LAYER_MAX(layer)    CPISI_DIFF(CPISI_PRODUCT(CPISI_SUM((layer), CPISI_UNITY), CPISI_CUBE), CPISI_UNITY)

/**
 * @brief Get layer center error code.
 * @param layer Layer index (0-8)
 * @return MIN + HALF(CUBE) = MIN + 13 (e.g., layer 0 → 13, layer 4 → 121)
 *
 * FORMULA: CENTER = MIN + 13
 */
#define CPISI_ERROR_LAYER_CENTER(layer) CPISI_SUM(CPISI_ERROR_LAYER_MIN(layer), CPISI_HALF(CPISI_CUBE))

/**
 * @brief Get layer half-range (constant = 13 for all layers).
 * @param layer Layer index (0-8)
 * @return HALF(MAX - MIN) = 13
 *
 * NOTE: This is constant (13) for all layers since each has 27 codes.
 */
#define CPISI_ERROR_LAYER_HALF(layer)   CPISI_HALF(CPISI_DIFF(CPISI_ERROR_LAYER_MAX(layer), CPISI_ERROR_LAYER_MIN(layer)))

/** @} */ /* end S.2b.6a LAYER OPS */

/** @name S.2b.6b CATEGORY OPS — Category offset bounds within a layer
 *  @brief Calculate MIN/MAX/CENTER offsets for each category within a layer.
 *
 *  Each layer has 3 categories (DAR phases), each with 9 positions (FACE).
 *  Categories are offsets WITHIN a layer (0-26), not absolute codes.
 *
 *  FORMULA PATTERN:
 *    OFF_MIN(cat) = cat × FACE = cat × 9
 *    OFF_MAX(cat) = (cat+1) × FACE - 1 = MIN + 8
 *    OFF_CENTER(cat) = MIN + HALF(FACE) = MIN + 4
 *    OFF_HALF(cat) = 4 (constant for all categories)
 *
 *  CATEGORY = DAR PHASE:
 *    0: DETECT   (offsets 0-8)   — problem identification
 *    1: ASSESS   (offsets 9-17)  — problem evaluation
 *    2: RESTORE  (offsets 18-26) — problem resolution
 *  @{ */

/** @brief Get category offset minimum. Formula: cat × 9 */
#define CPISI_OFF_MIN(cat)          CPISI_PRODUCT((cat), CPISI_FACE)

/** @brief Get category offset maximum. Formula: (cat+1) × 9 - 1 */
#define CPISI_OFF_MAX(cat)          CPISI_DIFF(CPISI_PRODUCT(CPISI_SUM((cat), CPISI_UNITY), CPISI_FACE), CPISI_UNITY)

/** @brief Get category offset center. Formula: MIN + 4 */
#define CPISI_OFF_CENTER(cat)       CPISI_SUM(CPISI_OFF_MIN(cat), CPISI_HALF(CPISI_FACE))

/** @brief Get category offset half-range (constant = 4). */
#define CPISI_OFF_HALF(cat)         CPISI_HALF(CPISI_DIFF(CPISI_OFF_MAX(cat), CPISI_OFF_MIN(cat)))

// ── Phase aliases (category = DAR phase) ────────────────────────────────────

/** @brief Phase offset minimum (alias for OFF_MIN). */
#define CPISI_PHASE_OFFSET_MIN(phase)    CPISI_OFF_MIN(phase)

/** @brief Phase offset maximum (alias for OFF_MAX). */
#define CPISI_PHASE_OFFSET_MAX(phase)    CPISI_OFF_MAX(phase)

/** @brief Phase offset center (alias for OFF_CENTER). */
#define CPISI_PHASE_OFFSET_CENTER(phase) CPISI_OFF_CENTER(phase)

/** @} */ /* end S.2b.6b CATEGORY OPS */

/** @name S.2b.6c VALIDATION OPS
 *  @brief Validate error codes within the 243+13=256 architecture.
 *  @{ */

/** @def CPISI_CODE_MIN
 *  @brief Minimum valid code (SUCCESS = 0) */
#define CPISI_CODE_MIN      CPISI_VOID

/** @def CPISI_CODE_MAX
 *  @brief Maximum valid code (255) */
#define CPISI_CODE_MAX      CPISI_BYTE_MAX

// ── Validators ─────────────────────────────────────────────────────────────

/**
 * @brief Check if code is SUCCESS (0).
 * @param code Error code to check
 * @return 1 if code == 0, 0 otherwise
 */
#define CPISI_IS_SUCCESS(code)      ((code) == CPISI_ERROR_SUCCESS)

/**
 * @brief Check if code is in layer error range [0, 242].
 * @param code Error code to check
 * @return 1 if code in layer error space, 0 otherwise
 *
 * NOTE: Includes SUCCESS (0). Use code != 0 && IS_LAYER_ERROR(code) for non-success errors.
 */
#define CPISI_IS_LAYER_ERROR(code)  CPISI_IN_RANGE((code), CPISI_CODE_MIN, CPISI_DIFF(CPISI_ERROR_METADATA_MIN, CPISI_UNITY))

/**
 * @brief Check if code is in metadata range [243, 255].
 * @param code Error code to check
 * @return 1 if code in metadata space, 0 otherwise
 */
#define CPISI_IS_METADATA_CODE(code) CPISI_IN_RANGE((code), CPISI_ERROR_METADATA_MIN, CPISI_ERROR_METADATA_MAX)

/**
 * @brief Check if code is valid [0, 255].
 * @param code Error code to check
 * @return 1 if code fits in byte, 0 otherwise
 */
#define CPISI_IS_VALID_CODE(code)   CPISI_IN_RANGE((code), CPISI_CODE_MIN, CPISI_CODE_MAX)

/**
 * @brief Check if code is in specific layer's range.
 * @param code  Error code to check
 * @param layer Layer index (0-8)
 * @return 1 if code in [LAYER_MIN, LAYER_MAX], 0 otherwise
 *
 * EXAMPLE:
 * ```c
 *   if (CPISI_IN_LAYER(code, CPISI_LAYER_HEALTH)) {
 *       // Health-layer specific handling
 *   }
 * ```
 */
#define CPISI_IN_LAYER(code, layer) CPISI_IN_RANGE((code), CPISI_ERROR_LAYER_MIN(layer), CPISI_ERROR_LAYER_MAX(layer))

/** @} */ /* end S.2b.6c VALIDATION OPS */

/** @} */ /* end S.2b.6 ERROR OPS */

/* NOTE: S.2b.7b ERROR DOMAIN was removed - functionality moved to S.2b.5 ERROR COMPOSITION.
 *       New macros use semantic naming (CPISI_MAKE_ERROR, CPISI_ERROR_LAYER, etc.)
 *       with explicit tier/position parameters instead of raw offsets.
 *
 *       Migration mapping:
 *         CPISI_ERROR_TO_LAYER  → CPISI_ERROR_LAYER
 *         CPISI_ERROR_TO_OFFSET → CPISI_ERROR_OFFSET
 *         CPISI_OFFSET_TO_CAT   → CPISI_ERROR_TIER
 *         CPISI_OFFSET_TO_POS   → CPISI_ERROR_POSITION
 *         CPISI_MAKE_ERROR(l,o) → CPISI_MAKE_ERROR_OFFSET(l,o)
 *         CPISI_MAKE_ERROR_FULL → CPISI_MAKE_ERROR (3-arg version)
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b.8 ERROR LAYER BASES — Per-layer base offsets [ERROR_LAYER_BASES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_layer_bases S.2b.8 ERROR LAYER BASES — Per-layer base offsets
 * @ingroup cpisi_error_macros
 * @brief    Explicit base offsets and tier subdivisions for each error layer.
 *
 * PURPOSE: Named constants for compile-time layer and tier calculations.
 *          Complements dynamic CPISI_ERROR_LAYER_MIN() with explicit constants.
 *
 * ARCHITECTURE: 9 layers × 27 codes = 243 (Base9 × CUBE = 3⁵)
 *   - Each layer: BASE + CORE(0-8) + EXT(9-17) + SYS(18-26)
 *   - Balanced ternary: 4 negative + 1 center + 4 positive
 *
 * ┌───────────────────────────────────────────────────────────────────────────────┐
 * │ LAYER STRUCTURE — 9 layers × 27 codes                                         │
 * ├───────────────────────────────────────────────────────────────────────────────┤
 * │ NEGATIVE (4)                 │ CENTER │ POSITIVE (4)                          │
 * │ ─────────────────────────────┼────────┼────────────────────────────────────── │
 * │ KERNEL SYSTEM RUNTIME FRAME  │ HEALTH │ NETWORK STORAGE ENGINE APP            │
 * │    0      1      2      3    │   4    │   5       6       7     8             │
 * │   -4     -3     -2     -1    │   0    │  +1      +2      +3    +4             │
 * │  0-26  27-53  54-80  81-107  │108-134 │135-161 162-188 189-215 216-242        │
 * └───────────────────────────────────────────────────────────────────────────────┘
 *
 * STRUCTURE:
 *   S.2b.8a NEGATIVE — Layers 0-3: KERNEL, SYSTEM, RUNTIME, FRAMEWORK
 *   S.2b.8b CENTER   — Layer 4: HEALTH (observation point)
 *   S.2b.8c POSITIVE — Layers 5-8: NETWORK, STORAGE, ENGINE, APP
 *
 * Builds FROM: S.2a ERROR BOUNDS (CPISI_ERROR_PER_LAYER = 27)
 * Builds TO:   S.3 ERROR TYPES (enum values use these bases)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2b.8a NEGATIVE LAYERS — Foundational layers (positions -4 to -1)
 *  @brief Base offsets and tier ranges for KERNEL, SYSTEM, RUNTIME, FRAMEWORK.
 *
 *  DERIVATION: base = layer_index × CPISI_ERROR_PER_LAYER (27)
 *  TIER STRUCTURE: Each layer has 3 tiers of 9 codes:
 *    - CORE: base + 0  (codes 0-8 within layer)
 *    - EXT:  base + 9  (codes 9-17 within layer)
 *    - SYS:  base + 18 (codes 18-26 within layer)
 *  @{ */

// ── KERNEL (Layer 0, position -4) ── Hardware/driver errors
// CPISI_ERR_BASE_KERNEL (0) — inherited from types.h

/** @brief KERNEL CORE tier offset. Range: 0-8. */
#define CPISI_ERR_KERNEL_CORE       (CPISI_ERR_BASE_KERNEL + 0)

/** @brief KERNEL EXT tier offset. Range: 9-17. */
#define CPISI_ERR_KERNEL_EXT        (CPISI_ERR_BASE_KERNEL + 9)

/** @brief KERNEL SYS tier offset. Range: 18-26. */
#define CPISI_ERR_KERNEL_SYS        (CPISI_ERR_BASE_KERNEL + 18)

// ── SYSTEM (Layer 1, position -3) ── OS service errors
// CPISI_ERR_BASE_SYSTEM (27) — inherited from types.h

/** @brief SYSTEM CORE tier offset. Range: 27-35. */
#define CPISI_ERR_SYSTEM_CORE       (CPISI_ERR_BASE_SYSTEM + 0)

/** @brief SYSTEM EXT tier offset. Range: 36-44. */
#define CPISI_ERR_SYSTEM_EXT        (CPISI_ERR_BASE_SYSTEM + 9)

/** @brief SYSTEM SYS tier offset. Range: 45-53. */
#define CPISI_ERR_SYSTEM_SYS        (CPISI_ERR_BASE_SYSTEM + 18)

// ── RUNTIME (Layer 2, position -2) ── VM/bytecode errors
// CPISI_ERR_BASE_RUNTIME (54) — inherited from types.h

/** @brief RUNTIME CORE tier offset. Range: 54-62. */
#define CPISI_ERR_RUNTIME_CORE      (CPISI_ERR_BASE_RUNTIME + 0)

/** @brief RUNTIME EXT tier offset. Range: 63-71. */
#define CPISI_ERR_RUNTIME_EXT       (CPISI_ERR_BASE_RUNTIME + 9)

/** @brief RUNTIME SYS tier offset. Range: 72-80. */
#define CPISI_ERR_RUNTIME_SYS       (CPISI_ERR_BASE_RUNTIME + 18)

// ── FRAMEWORK (Layer 3, position -1) ── Library/API errors
// CPISI_ERR_BASE_FRAMEWORK (81) — inherited from types.h

/** @brief FRAMEWORK CORE tier offset. Range: 81-89. */
#define CPISI_ERR_FRAMEWORK_CORE    (CPISI_ERR_BASE_FRAMEWORK + 0)

/** @brief FRAMEWORK EXT tier offset. Range: 90-98. */
#define CPISI_ERR_FRAMEWORK_EXT     (CPISI_ERR_BASE_FRAMEWORK + 9)

/** @brief FRAMEWORK SYS tier offset. Range: 99-107. */
#define CPISI_ERR_FRAMEWORK_SYS     (CPISI_ERR_BASE_FRAMEWORK + 18)

/** @} */ /* end S.2b.8a NEGATIVE LAYERS */

/** @name S.2b.8b CENTER LAYER — Observation point (position 0)
 *  @brief Base offset and tier ranges for HEALTH — CPI-SI measurement layer.
 *
 *  HEALTH is the center of the balanced ternary architecture (yashar).
 *  All errors measured relative to health. Position 0.
 *  @{ */

// ── HEALTH (Layer 4, position 0) ── CPI-SI measurement errors
// CPISI_ERR_BASE_HEALTH (108) — inherited from types.h

/** @brief HEALTH CORE tier offset. Range: 108-116. */
#define CPISI_ERR_HEALTH_CORE       (CPISI_ERR_BASE_HEALTH + 0)

/** @brief HEALTH EXT tier offset. Range: 117-125. */
#define CPISI_ERR_HEALTH_EXT        (CPISI_ERR_BASE_HEALTH + 9)

/** @brief HEALTH SYS tier offset. Range: 126-134. */
#define CPISI_ERR_HEALTH_SYS        (CPISI_ERR_BASE_HEALTH + 18)

/** @} */ /* end S.2b.8b CENTER LAYER */

/** @name S.2b.8c POSITIVE LAYERS — Application layers (positions +1 to +4)
 *  @brief Base offsets and tier ranges for NETWORK, STORAGE, ENGINE, APP.
 *
 *  Application-facing layers. Work outward from HEALTH center.
 *  @{ */

// ── NETWORK (Layer 5, position +1) ── FaithNet/connection errors
// CPISI_ERR_BASE_NETWORK (135) — inherited from types.h

/** @brief NETWORK CORE tier offset. Range: 135-143. */
#define CPISI_ERR_NETWORK_CORE      (CPISI_ERR_BASE_NETWORK + 0)

/** @brief NETWORK EXT tier offset. Range: 144-152. */
#define CPISI_ERR_NETWORK_EXT       (CPISI_ERR_BASE_NETWORK + 9)

/** @brief NETWORK SYS tier offset. Range: 153-161. */
#define CPISI_ERR_NETWORK_SYS       (CPISI_ERR_BASE_NETWORK + 18)

// ── STORAGE (Layer 6, position +2) ── Bereshit/file errors
// CPISI_ERR_BASE_STORAGE (162) — inherited from types.h

/** @brief STORAGE CORE tier offset. Range: 162-170. */
#define CPISI_ERR_STORAGE_CORE      (CPISI_ERR_BASE_STORAGE + 0)

/** @brief STORAGE EXT tier offset. Range: 171-179. */
#define CPISI_ERR_STORAGE_EXT       (CPISI_ERR_BASE_STORAGE + 9)

/** @brief STORAGE SYS tier offset. Range: 180-188. */
#define CPISI_ERR_STORAGE_SYS       (CPISI_ERR_BASE_STORAGE + 18)

// ── ENGINE (Layer 7, position +3) ── Cornerstone/render errors
// CPISI_ERR_BASE_ENGINE (189) — inherited from types.h

/** @brief ENGINE CORE tier offset. Range: 189-197. */
#define CPISI_ERR_ENGINE_CORE       (CPISI_ERR_BASE_ENGINE + 0)

/** @brief ENGINE EXT tier offset. Range: 198-206. */
#define CPISI_ERR_ENGINE_EXT        (CPISI_ERR_BASE_ENGINE + 9)

/** @brief ENGINE SYS tier offset. Range: 207-215. */
#define CPISI_ERR_ENGINE_SYS        (CPISI_ERR_BASE_ENGINE + 18)

// ── APP (Layer 8, position +4) ── User-facing/UI errors
// CPISI_ERR_BASE_APP (216) — inherited from types.h

/** @brief APP CORE tier offset. Range: 216-224. */
#define CPISI_ERR_APP_CORE          (CPISI_ERR_BASE_APP + 0)

/** @brief APP EXT tier offset. Range: 225-233. */
#define CPISI_ERR_APP_EXT           (CPISI_ERR_BASE_APP + 9)

/** @brief APP SYS tier offset. Range: 234-242. */
#define CPISI_ERR_APP_SYS           (CPISI_ERR_BASE_APP + 18)

/** @} */ /* end S.2b.8c POSITIVE LAYERS */

/** @} */ /* end S.2b.8 ERROR LAYER BASES */

/** @} */ /* end S.2b ERROR MACROS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2c VERIFY — Compile-time tripwires [VERIFY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_verify S.2c ERROR VERIFY — Compile-time tripwires
 * @ingroup cpisi_error_domain
 * @brief    Verify error code geometry at compile time.
 *
 * PURPOSE: Catch error configuration errors BEFORE runtime. Zero runtime cost.
 *          If any assertion fails, compilation stops with a descriptive message.
 *
 * Builds FROM: S.2a ERROR BOUNDS (geometry values), S.2b ERROR MACROS (operations)
 * Builds TO:   S.2d ERROR LOOKUP (verified values ready for runtime tables)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC tripwires extending S.2c VERIFY in types.h.
 *   Verifies the 256-code error system: 243 layer errors + 13 metadata slots.
 *
 * TRIPWIRE PHILOSOPHY:
 *   Each value is verified TWO ways:
 *   1. VALUE TRIPWIRE:        Assert the literal (e.g., ERROR_PER_LAYER == 27)
 *   2. RELATIONSHIP TRIPWIRE: Assert the derivation (e.g., CODE_COUNT == LAYERS × CUBE)
 *   BOTH together catches drift in either direction.
 *
 * STRUCTURE:
 *   S.2c.6 ERROR TRIPWIRES — Error verification
 *     S.2c.6a VALUE       — ERROR_PER_LAYER, LAYER_COUNT, CODE_COUNT
 *     S.2c.6b RELATIONSHIP — Derivations from CUBE, FACE, PENTERACT
 *     S.2c.6c BOUNDS      — Layer and category boundary values
 *     S.2c.6d METADATA    — META_COUNT = BRIDGE verification
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @defgroup cpisi_error_tripwires S.2c.6 ERROR TRIPWIRES — Error verification
 * @ingroup cpisi_error_verify
 * @brief    Compile-time verification of error code structure.
 *
 * **TARGET: types-error.h** (will split when types.h is complete)
 *
 * ERROR CODE ARCHITECTURE (verified):
 *   ┌─────────────────────────────────────────────────────────────────────────┐
 *   │  256 TOTAL CODES = 243 LAYER ERRORS + 13 METADATA SLOTS                 │
 *   │  ─────────────────────────────────────────────────────────────────────  │
 *   │                                                                         │
 *   │  LAYER ERRORS (0-242): 9 layers × 27 offsets                            │
 *   │    Layer = code ÷ 27                                                    │
 *   │    Offset = code mod 27                                                 │
 *   │                                                                         │
 *   │  OFFSET STRUCTURE: 3 categories × 9 positions                           │
 *   │    Category = offset ÷ 9 (D=0, A=1, R=2)                                │
 *   │    Position = offset mod 9                                              │
 *   │                                                                         │
 *   │  METADATA (243-255): 13 slots completing byte                           │
 *   └─────────────────────────────────────────────────────────────────────────┘
 *
 * STRUCTURE:
 *   S.2c.6a VALUE       — ERROR_PER_LAYER, LAYER_COUNT, CODE_COUNT
 *   S.2c.6b RELATIONSHIP — Derivations from CUBE, FACE, PENTERACT
 *   S.2c.6c BOUNDS      — Layer and category boundary values
 *   S.2c.6d METADATA    — META_COUNT = BRIDGE verification
 *
 * VERIFIED FROM: S.2a.5 ERRORS bounds
 * @{ */

/** @name S.2c.6a VALUE TRIPWIRES — Verify error literals
 *  @brief Verify error code structure literal values.
 *
 *  KEY VALUES:
 *    - ERROR_PER_LAYER = 27 (CUBE)
 *    - ERROR_LAYER_COUNT = 9 (FACE)
 *    - ERROR_CODE_COUNT = 243 (PENTERACT)
 *    - METADATA range = [243, 255]
 *  @{ */

CPISI_STATIC_ASSERT(CPISI_ERROR_PER_LAYER == 27,
                    "ERROR_PER_LAYER must be 27 — CUBE codes per layer");
CPISI_STATIC_ASSERT(CPISI_ERROR_LAYER_COUNT == 9,
                    "ERROR_LAYER_COUNT must be 9 — FACE layers");
CPISI_STATIC_ASSERT(CPISI_ERROR_CODE_COUNT == 243,
                    "ERROR_CODE_COUNT must be 243 — PENTERACT layer codes");
CPISI_STATIC_ASSERT(CPISI_ERROR_METADATA_MIN == 243,
                    "ERROR_METADATA_MIN must be 243 — metadata region start");
CPISI_STATIC_ASSERT(CPISI_ERROR_METADATA_MAX == 255,
                    "ERROR_METADATA_MAX must be 255 — metadata region end");


/** @} */ /* end S.2c.6a VALUE TRIPWIRES */

/** @name S.2c.6b RELATIONSHIP TRIPWIRES — Verify error derivations
 *  @brief Verify error values derive from CUBE, FACE, PENTERACT.
 *
 *  DERIVATION CHAIN:
 *    - PER_LAYER = CUBE = 3³ = 27
 *    - LAYER_COUNT = FACE = 3² = 9
 *    - CODE_COUNT = PENTERACT = 3⁵ = 243
 *    - Also: CODE_COUNT = LAYER_COUNT × PER_LAYER = 9 × 27 = 243 ✓
 *  @{ */

CPISI_STATIC_ASSERT(CPISI_ERROR_PER_LAYER == CPISI_CUBE,
                    "ERROR_PER_LAYER must equal CUBE — 27 = 3³");
CPISI_STATIC_ASSERT(CPISI_ERROR_LAYER_COUNT == CPISI_FACE,
                    "ERROR_LAYER_COUNT must equal FACE — 9 = 3²");
CPISI_STATIC_ASSERT(CPISI_ERROR_CODE_COUNT == CPISI_PENTERACT,
                    "ERROR_CODE_COUNT must equal PENTERACT — 243 = 3⁵");
CPISI_STATIC_ASSERT(CPISI_ERROR_CODE_COUNT == CPISI_PRODUCT(CPISI_ERROR_LAYER_COUNT, CPISI_ERROR_PER_LAYER),
                    "ERROR_CODE_COUNT must equal LAYER_COUNT × PER_LAYER — 243 = 9×27");
CPISI_STATIC_ASSERT(CPISI_ERROR_METADATA_MIN == CPISI_PENTERACT,
                    "ERROR_METADATA_MIN must equal PENTERACT — 243 = 3⁵");
CPISI_STATIC_ASSERT(CPISI_ERROR_METADATA_MAX == CPISI_BYTE_MAX,
                    "ERROR_METADATA_MAX must equal BYTE_MAX — 255");


/** @} */ /* end S.2c.6b RELATIONSHIP TRIPWIRES */

/** @name S.2c.6c BOUNDS TRIPWIRES — Verify layer/category bounds
 *  @brief Verify layer start codes and category offset bounds.
 *
 *  LAYER START CODES (layer × 27):
 *    - KERNEL: 0 × 27 = 0
 *    - HEALTH: 4 × 27 = 108 (center layer)
 *    - APP: 8 × 27 = 216 (last layer)
 *
 *  CATEGORY OFFSET BOUNDS (category × 9):
 *    - Category 0 (DETECT): 0-8
 *    - Category 1 (ASSESS): 9-17
 *    - Category 2 (RESTORE): 18-26
 *  @{ */

CPISI_STATIC_ASSERT(CPISI_PRODUCT(CPISI_LAYER_KERNEL, CPISI_CUBE) == 0,
                    "KERNEL layer first must be 0 — 0×27");
CPISI_STATIC_ASSERT(CPISI_PRODUCT(CPISI_LAYER_HEALTH, CPISI_CUBE) == 108,
                    "HEALTH layer first must be 108 — CENTER layer (4×27)");
CPISI_STATIC_ASSERT(CPISI_PRODUCT(CPISI_LAYER_APP, CPISI_CUBE) == 216,
                    "APP layer first must be 216 — last layer (8×27)");
CPISI_STATIC_ASSERT(CPISI_PRODUCT(0, CPISI_FACE) == 0,
                    "Category 0 offset min must be 0");
CPISI_STATIC_ASSERT(CPISI_PRODUCT(1, CPISI_FACE) == 9,
                    "Category 1 offset min must be 9");
CPISI_STATIC_ASSERT(CPISI_PRODUCT(2, CPISI_FACE) == 18,
                    "Category 2 offset min must be 18");
CPISI_STATIC_ASSERT(CPISI_DIFF(CPISI_CUBE, CPISI_UNITY) == 26,
                    "Category 2 offset max must be 26 — CUBE-1");


/** @} */ /* end S.2c.6c BOUNDS TRIPWIRES */

/* NOTE: S.2c.6d METADATA TRIPWIRES migrated to types-metadata.h
 *       See: types-metadata.h S.2c.9 BRIDGE EQUATION (BRIDGE verification) */

/** @} */ /* end S.2c.6 ERROR TRIPWIRES */

/** @} */ /* end S.2c ERROR VERIFY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2d LOOKUP — Runtime lookup tables [LOOKUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_lookup S.2d LOOKUP — Runtime lookup tables
 * @ingroup cpisi_error_domain
 * @brief    Pre-computed tables for O(1) error code operations.
 *
 * PURPOSE: Container for error domain lookup tables.
 *
 * STRUCTURE (mirrors types.h S.2d pattern: Domain → a/b/c):
 *   S.2d.5 ERROR — Error domain tables
 *     S.2d.5a TABLES     — 6 arrays (layer bounds, category offsets)
 *     S.2d.5b ACCESSORS  — 6 macros (direct table access)
 *     S.2d.5c CONVERSION — [Reserved: error codes use direct lookup]
 *
 * Builds FROM: S.2c ERROR VERIFY (structure verified)
 * Builds TO:   S.3 ERROR TYPES (enum definitions)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup cpisi_error_tables S.2d.5 ERROR — Error domain lookup
 * @ingroup cpisi_error_lookup
 * @brief    Runtime lookup tables for error code navigation.
 *
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  DOMAIN-SPECIFIC LOOKUP — Error system tables                              ║
 * ║  ─────────────────────────────────────────────────────────────────────────║
 * ║  These tables exist in types.h during consolidation but belong in         ║
 * ║  types-error.h after the split. They provide runtime navigation for       ║
 * ║  the error code space (243 codes = 9 layers × 27 offsets).               ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 *
 * ERROR CODE ARCHITECTURE (9×27 = 243):
 *   - 9 layers (KERNEL..APP), each with CUBE(27) error slots
 *   - Layer index × CUBE = base, base + offset = error code
 *   - CENTER: HEALTH layer (4), ASSESS offset (13) → code 121 (= PENTERACT half)
 *
 * STRUCTURE (template pattern: a=TABLES, b=ACCESSORS, c=CONVERSION):
 *   S.2d.5a TABLES     — 6 arrays: 3 layer bounds (9 each) + 3 category offsets (3 each)
 *   S.2d.5b ACCESSORS  — 6 macros: layer error min/max/center, category offset min/max/center
 *   S.2d.5c CONVERSION — [Reserved: error codes not converted, only looked up]
 *
 * NOTE: CPISI_LAYER_POSITION moved to types.h S.2d.2a (fundamental layer property)
 *
 * @ingroup S2d_LOOKUP
 * @{ */

/** @name S.2d.5a TABLES — Error lookup arrays
 *  @brief Six arrays for error code navigation: layer bounds and category offsets.
 *
 *  LAYER ARRAYS (3 × 9 entries, indexed 0-8):
 *    LAYER_ERROR_MIN[9]    — First error code per layer
 *    LAYER_ERROR_MAX[9]    — Last error code per layer
 *    LAYER_ERROR_CENTER[9] — Center error code per layer
 *
 *  CATEGORY ARRAYS (3 × 3 entries, indexed 0-2):
 *    CAT_OFFSET_MIN[3]     — First offset per DAR category
 *    CAT_OFFSET_MAX[3]     — Last offset per DAR category
 *    CAT_OFFSET_CENTER[3]  — Center offset per DAR category
 *
 *  NOTE: CPISI_LAYER_POSITION moved to types.h S.2d.2a (fundamental layer property)
 *  @{ */

// ── LAYER ERROR BOUNDS (3 arrays × 9 entries) ───────────────────────────────

/**
 * @brief Layer error minimum lookup — maps layer index (0-8) to first error code.
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_COUNT = 9 (from S.2a.2 TIER 2) — array size
 *   - CPISI_CUBE = 27 (from S.2a.1 TIER 1) — codes per layer
 *
 * DERIVATION:
 *   Formula: min = layer × CUBE
 *   [0] KERNEL = 0×27 = 0
 *   [4] HEALTH = 4×27 = 108 (CENTER layer)
 *   [8] APP = 8×27 = 216
 *
 * USE CASE: Range checking, error code to layer mapping, iteration bounds.
 */
static const uint8_t CPISI_LAYER_ERROR_MIN[CPISI_LAYER_COUNT] = {
    CPISI_PRODUCT(CPISI_LAYER_KERNEL, CPISI_CUBE),     /* [0] KERNEL    = 0×27 = 0   */
    CPISI_PRODUCT(CPISI_LAYER_SYSTEM, CPISI_CUBE),     /* [1] SYSTEM    = 1×27 = 27  */
    CPISI_PRODUCT(CPISI_LAYER_RUNTIME, CPISI_CUBE),    /* [2] RUNTIME   = 2×27 = 54  */
    CPISI_PRODUCT(CPISI_LAYER_FRAMEWORK, CPISI_CUBE),  /* [3] FRAMEWORK = 3×27 = 81  */
    CPISI_PRODUCT(CPISI_LAYER_HEALTH, CPISI_CUBE),     /* [4] HEALTH    = 4×27 = 108 */
    CPISI_PRODUCT(CPISI_LAYER_NETWORK, CPISI_CUBE),    /* [5] NETWORK   = 5×27 = 135 */
    CPISI_PRODUCT(CPISI_LAYER_STORAGE, CPISI_CUBE),    /* [6] STORAGE   = 6×27 = 162 */
    CPISI_PRODUCT(CPISI_LAYER_ENGINE, CPISI_CUBE),     /* [7] ENGINE    = 7×27 = 189 */
    CPISI_PRODUCT(CPISI_LAYER_APP, CPISI_CUBE)         /* [8] APP       = 8×27 = 216 */
};

/**
 * @brief Layer error maximum lookup — maps layer index (0-8) to last error code.
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_COUNT = 9 (from S.2a.2 TIER 2) — array size
 *   - CPISI_CUBE = 27 (from S.2a.1 TIER 1) — codes per layer
 *
 * DERIVATION:
 *   Formula: max = (layer+1) × CUBE - 1 = min + 26
 *   [0] KERNEL = 1×27-1 = 26
 *   [4] HEALTH = 5×27-1 = 134 (CENTER layer)
 *   [8] APP = 9×27-1 = 242 (last valid error code)
 *
 * USE CASE: Range checking, error code validation, iteration bounds.
 */
static const uint8_t CPISI_LAYER_ERROR_MAX[CPISI_LAYER_COUNT] = {
    CPISI_DIFF(CPISI_CUBE, CPISI_UNITY),                   /* [0] KERNEL    = 27-1 = 26  */
    CPISI_DIFF(CPISI_PRODUCT(2, CPISI_CUBE), CPISI_UNITY), /* [1] SYSTEM    = 54-1 = 53  */
    CPISI_DIFF(CPISI_PRODUCT(3, CPISI_CUBE), CPISI_UNITY), /* [2] RUNTIME   = 81-1 = 80  */
    CPISI_DIFF(CPISI_PRODUCT(4, CPISI_CUBE), CPISI_UNITY), /* [3] FRAMEWORK = 108-1= 107 */
    CPISI_DIFF(CPISI_PRODUCT(5, CPISI_CUBE), CPISI_UNITY), /* [4] HEALTH    = 135-1= 134 */
    CPISI_DIFF(CPISI_PRODUCT(6, CPISI_CUBE), CPISI_UNITY), /* [5] NETWORK   = 162-1= 161 */
    CPISI_DIFF(CPISI_PRODUCT(7, CPISI_CUBE), CPISI_UNITY), /* [6] STORAGE   = 189-1= 188 */
    CPISI_DIFF(CPISI_PRODUCT(8, CPISI_CUBE), CPISI_UNITY), /* [7] ENGINE    = 216-1= 215 */
    CPISI_DIFF(CPISI_PRODUCT(9, CPISI_CUBE), CPISI_UNITY)  /* [8] APP       = 243-1= 242 */
};

/**
 * @brief Layer error center lookup — maps layer index (0-8) to center error code.
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_COUNT = 9 (from S.2a.2 TIER 2) — array size
 *   - CPISI_CUBE = 27 (from S.2a.1 TIER 1) — codes per layer
 *   - CPISI_BRIDGE = 13 (from S.2a.1 TIER 1) — center offset within layer
 *
 * DERIVATION:
 *   Formula: center = layer × CUBE + BRIDGE = min + 13
 *   [0] KERNEL = 0+13 = 13
 *   [4] HEALTH = 108+13 = 121 = PENTERACT/2 (CENTER of CENTER!)
 *   [8] APP = 216+13 = 229
 *
 * NOTE: HEALTH center (121) = PENTERACT(243)/2 - significant alignment.
 *
 * USE CASE: Default error codes, center-based navigation, offset calculations.
 */
static const uint8_t CPISI_LAYER_ERROR_CENTER[CPISI_LAYER_COUNT] = {
    CPISI_SUM(CPISI_PRODUCT(0, CPISI_CUBE), CPISI_BRIDGE),  /* [0] KERNEL    = 0+13 = 13  */
    CPISI_SUM(CPISI_PRODUCT(1, CPISI_CUBE), CPISI_BRIDGE),  /* [1] SYSTEM    = 27+13= 40  */
    CPISI_SUM(CPISI_PRODUCT(2, CPISI_CUBE), CPISI_BRIDGE),  /* [2] RUNTIME   = 54+13= 67  */
    CPISI_SUM(CPISI_PRODUCT(3, CPISI_CUBE), CPISI_BRIDGE),  /* [3] FRAMEWORK = 81+13= 94  */
    CPISI_SUM(CPISI_PRODUCT(4, CPISI_CUBE), CPISI_BRIDGE),  /* [4] HEALTH    = 108+13=121 */
    CPISI_SUM(CPISI_PRODUCT(5, CPISI_CUBE), CPISI_BRIDGE),  /* [5] NETWORK   = 135+13=148 */
    CPISI_SUM(CPISI_PRODUCT(6, CPISI_CUBE), CPISI_BRIDGE),  /* [6] STORAGE   = 162+13=175 */
    CPISI_SUM(CPISI_PRODUCT(7, CPISI_CUBE), CPISI_BRIDGE),  /* [7] ENGINE    = 189+13=202 */
    CPISI_SUM(CPISI_PRODUCT(8, CPISI_CUBE), CPISI_BRIDGE)   /* [8] APP       = 216+13=229 */
};

/* NOTE: CPISI_LAYER_POSITION moved to types.h S.2d.2a TABLES (fundamental layer property) */

// ── CATEGORY OFFSET BOUNDS (3 arrays × 3 entries) ───────────────────────────

/**
 * @brief Category offset minimum lookup — maps DAR category (0-2) to first offset.
 *
 * BOUNDS USED:
 *   - CPISI_TERNARY = 3 (from S.2a.1 TIER 1) — array size
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — offsets per category
 *
 * DERIVATION:
 *   Formula: min = category × FACE
 *   [0] DETECT = 0×9 = 0
 *   [1] ASSESS = 1×9 = 9 (CENTER category)
 *   [2] RESTORE = 2×9 = 18
 *
 * USE CASE: Offset range checking, category identification, iteration bounds.
 */
static const uint8_t CPISI_CAT_OFFSET_MIN[CPISI_TERNARY] = {
    CPISI_PRODUCT(0, CPISI_FACE),  /* [0] DETECT  = 0×9 = 0  */
    CPISI_PRODUCT(1, CPISI_FACE),  /* [1] ASSESS  = 1×9 = 9  */
    CPISI_PRODUCT(2, CPISI_FACE)   /* [2] RESTORE = 2×9 = 18 */
};

/**
 * @brief Category offset maximum lookup — maps DAR category (0-2) to last offset.
 *
 * BOUNDS USED:
 *   - CPISI_TERNARY = 3 (from S.2a.1 TIER 1) — array size
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — offsets per category
 *
 * DERIVATION:
 *   Formula: max = (category+1) × FACE - 1 = min + 8
 *   [0] DETECT = 9-1 = 8
 *   [1] ASSESS = 18-1 = 17 (CENTER category)
 *   [2] RESTORE = 27-1 = 26
 *
 * USE CASE: Offset range checking, iteration bounds.
 */
static const uint8_t CPISI_CAT_OFFSET_MAX[CPISI_TERNARY] = {
    CPISI_DIFF(CPISI_FACE, CPISI_UNITY),                   /* [0] DETECT  = 9-1 = 8  */
    CPISI_DIFF(CPISI_PRODUCT(2, CPISI_FACE), CPISI_UNITY), /* [1] ASSESS  = 18-1= 17 */
    CPISI_DIFF(CPISI_CUBE, CPISI_UNITY)                    /* [2] RESTORE = 27-1= 26 */
};

/**
 * @brief Category offset center lookup — maps DAR category (0-2) to center offset.
 *
 * BOUNDS USED:
 *   - CPISI_TERNARY = 3 (from S.2a.1 TIER 1) — array size
 *   - CPISI_FACE = 9 (from S.2a.3 TIER 3) — offsets per category
 *   - CPISI_FACE_CENTER = 4 (from S.2a.2 TIER 2) — center within category
 *
 * DERIVATION:
 *   Formula: center = category × FACE + FACE_CENTER = min + 4
 *   [0] DETECT = 0+4 = 4
 *   [1] ASSESS = 9+4 = 13 = BRIDGE (significant!)
 *   [2] RESTORE = 18+4 = 22
 *
 * NOTE: ASSESS center (13) = BRIDGE constant — the architectural link.
 *
 * USE CASE: Default offsets, center-based navigation.
 */
static const uint8_t CPISI_CAT_OFFSET_CENTER[CPISI_TERNARY] = {
    CPISI_SUM(CPISI_PRODUCT(0, CPISI_FACE), CPISI_FACE_CENTER),  /* [0] DETECT  = 0+4 = 4  */
    CPISI_SUM(CPISI_PRODUCT(1, CPISI_FACE), CPISI_FACE_CENTER),  /* [1] ASSESS  = 9+4 = 13 */
    CPISI_SUM(CPISI_PRODUCT(2, CPISI_FACE), CPISI_FACE_CENTER)   /* [2] RESTORE = 18+4= 22 */
};

/** @} */ /* end S.2d.5a TABLES */


/** @name S.2d.5b ACCESSORS — Error table access macros
 *  @brief Type-safe access macros for error lookup tables.
 *
 *  PATTERN: Each accessor provides direct array access without bounds check.
 *           Caller must ensure valid index range.
 *
 *  LAYER ACCESSORS (3 macros, index 0-8):
 *    cpisi_layer_error_min(layer)    — First error code for layer
 *    cpisi_layer_error_max(layer)    — Last error code for layer
 *    cpisi_layer_error_center(layer) — Center error code for layer
 *
 *  CATEGORY ACCESSORS (3 macros, index 0-2):
 *    cpisi_cat_offset_min(cat)       — First offset for category
 *    cpisi_cat_offset_max(cat)       — Last offset for category
 *    cpisi_cat_offset_center(cat)    — Center offset for category
 *
 *  NOTE: cpisi_layer_position() moved to types.h S.2d.2b ACCESSORS
 *  @{ */

/**
 * @brief Get layer error minimum — LAYER (0-8) → MIN code.
 *
 * BOUNDS:
 *   - INPUT:  layer ∈ [0, 8]
 *   - OUTPUT: uint8_t ∈ {0, 27, 54, 81, 108, 135, 162, 189, 216}
 *
 * @par Derivation
 *      Direct index into CPISI_LAYER_ERROR_MIN[layer].
 *      Formula: layer × CUBE = layer × 27.
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ layer ≤ 8.
 *
 * @par Semantic Meaning
 *      First error code owned by this layer. Each layer owns CUBE (27) codes.
 *      Foundation layers (0-3) handle system errors; HEALTH (4) handles
 *      diagnostic crossings; upper layers (5-8) handle application errors.
 *
 * EXAMPLE:
 *   cpisi_layer_error_min(0) → 0   (KERNEL first code)
 *   cpisi_layer_error_min(4) → 108 (HEALTH first code, CENTER layer)
 *   cpisi_layer_error_min(8) → 216 (APP first code)
 *
 * @param layer Layer index 0-8 (unchecked)
 *
 * @see CPISI_LAYER_ERROR_MIN — source table
 * @see cpisi_layer_error_max — last code for layer
 */
#define cpisi_layer_error_min(layer)    (CPISI_LAYER_ERROR_MIN[(layer)])

/**
 * @brief Get layer error maximum — LAYER (0-8) → MAX code.
 *
 * BOUNDS:
 *   - INPUT:  layer ∈ [0, 8]
 *   - OUTPUT: uint8_t ∈ {26, 53, 80, 107, 134, 161, 188, 215, 242}
 *
 * @par Derivation
 *      Direct index into CPISI_LAYER_ERROR_MAX[layer].
 *      Formula: (layer + 1) × CUBE - 1 = (layer + 1) × 27 - 1.
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ layer ≤ 8.
 *
 * @par Semantic Meaning
 *      Last error code owned by this layer. Combined with MIN, defines the
 *      inclusive range [MIN, MAX] of valid codes for this layer. All 27
 *      codes within this range belong to this layer's error space.
 *
 * EXAMPLE:
 *   cpisi_layer_error_max(0) → 26  (KERNEL last code)
 *   cpisi_layer_error_max(4) → 134 (HEALTH last code, CENTER layer)
 *   cpisi_layer_error_max(8) → 242 (APP last code, PENTERACT - 1)
 *
 * @param layer Layer index 0-8 (unchecked)
 *
 * @see CPISI_LAYER_ERROR_MAX — source table
 * @see cpisi_layer_error_min — first code for layer
 */
#define cpisi_layer_error_max(layer)    (CPISI_LAYER_ERROR_MAX[(layer)])

/**
 * @brief Get layer error center — LAYER (0-8) → CENTER code.
 *
 * BOUNDS:
 *   - INPUT:  layer ∈ [0, 8]
 *   - OUTPUT: uint8_t ∈ {13, 40, 67, 94, 121, 148, 175, 202, 229}
 *
 * @par Derivation
 *      Direct index into CPISI_LAYER_ERROR_CENTER[layer].
 *      Formula: layer × CUBE + CUBE_CENTER = layer × 27 + 13.
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ layer ≤ 8.
 *
 * @par Semantic Meaning
 *      Center error code for this layer — the balanced pivot point. Center
 *      of center (HEALTH layer, ASSESS category) = 121 = PENTERACT/2 rounded,
 *      the absolute center of the entire 243-code error space.
 *
 * EXAMPLE:
 *   cpisi_layer_error_center(0) → 13  (KERNEL center)
 *   cpisi_layer_error_center(4) → 121 (HEALTH center = ABSOLUTE CENTER)
 *   cpisi_layer_error_center(8) → 229 (APP center)
 *
 * @param layer Layer index 0-8 (unchecked)
 *
 * @see CPISI_LAYER_ERROR_CENTER — source table
 * @see cpisi_cat_offset_center — category center within layer
 */
#define cpisi_layer_error_center(layer) (CPISI_LAYER_ERROR_CENTER[(layer)])

/* NOTE: cpisi_layer_position() moved to types.h S.2d.2b ACCESSORS */

/**
 * @brief Get category offset minimum — CATEGORY (0-2) → MIN offset.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, 2] (DETECT, ASSESS, RESTORE)
 *   - OUTPUT: uint8_t ∈ {0, 9, 18}
 *
 * @par Derivation
 *      Direct index into CPISI_CAT_OFFSET_MIN[cat].
 *      Formula: cat × TUBE = cat × 9.
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 2.
 *
 * @par Semantic Meaning
 *      First offset within the DAR (Detect-Assess-Restore) category.
 *      Categories partition each layer's 27 codes into three phases:
 *      DETECT (0-8) identifies issues, ASSESS (9-17) evaluates them,
 *      RESTORE (18-26) takes corrective action.
 *
 * EXAMPLE:
 *   cpisi_cat_offset_min(0) → 0  (DETECT first offset)
 *   cpisi_cat_offset_min(1) → 9  (ASSESS first offset)
 *   cpisi_cat_offset_min(2) → 18 (RESTORE first offset)
 *
 * @param cat Category index 0-2 (unchecked)
 *
 * @see CPISI_CAT_OFFSET_MIN — source table
 * @see cpisi_cat_offset_max — last offset for category
 */
#define cpisi_cat_offset_min(cat)       (CPISI_CAT_OFFSET_MIN[(cat)])

/**
 * @brief Get category offset maximum — CATEGORY (0-2) → MAX offset.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, 2] (DETECT, ASSESS, RESTORE)
 *   - OUTPUT: uint8_t ∈ {8, 17, 26}
 *
 * @par Derivation
 *      Direct index into CPISI_CAT_OFFSET_MAX[cat].
 *      Formula: (cat + 1) × TUBE - 1 = (cat + 1) × 9 - 1.
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 2.
 *
 * @par Semantic Meaning
 *      Last offset within the DAR category. Combined with MIN, defines the
 *      inclusive range [MIN, MAX] of valid offsets. Each category owns 9
 *      offsets (TUBE), partitioning the layer's CUBE into three phases.
 *
 * EXAMPLE:
 *   cpisi_cat_offset_max(0) → 8  (DETECT last offset)
 *   cpisi_cat_offset_max(1) → 17 (ASSESS last offset)
 *   cpisi_cat_offset_max(2) → 26 (RESTORE last offset = CUBE - 1)
 *
 * @param cat Category index 0-2 (unchecked)
 *
 * @see CPISI_CAT_OFFSET_MAX — source table
 * @see cpisi_cat_offset_min — first offset for category
 */
#define cpisi_cat_offset_max(cat)       (CPISI_CAT_OFFSET_MAX[(cat)])

/**
 * @brief Get category offset center — CATEGORY (0-2) → CENTER offset.
 *
 * BOUNDS:
 *   - INPUT:  cat ∈ [0, 2] (DETECT, ASSESS, RESTORE)
 *   - OUTPUT: uint8_t ∈ {4, 13, 22}
 *
 * @par Derivation
 *      Direct index into CPISI_CAT_OFFSET_CENTER[cat].
 *      Formula: cat × TUBE + TUBE_CENTER = cat × 9 + 4.
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ cat ≤ 2.
 *
 * @par Semantic Meaning
 *      Center offset within the DAR category — the balanced pivot point.
 *      ASSESS center (offset 13) is the CUBE center, the balanced observation
 *      point within any layer. HEALTH layer + ASSESS center = code 121,
 *      the absolute center of the entire error space.
 *
 * EXAMPLE:
 *   cpisi_cat_offset_center(0) → 4  (DETECT center)
 *   cpisi_cat_offset_center(1) → 13 (ASSESS center = CUBE CENTER)
 *   cpisi_cat_offset_center(2) → 22 (RESTORE center)
 *
 * @param cat Category index 0-2 (unchecked)
 *
 * @see CPISI_CAT_OFFSET_CENTER — source table
 * @see cpisi_layer_error_center — full error code including layer
 */
#define cpisi_cat_offset_center(cat)    (CPISI_CAT_OFFSET_CENTER[(cat)])

/** @} */ /* end S.2d.5b ACCESSORS */

/** @name S.2d.5c CONVERSION — Error code conversion utilities
 *  @brief [Reserved: Error codes are looked up, not converted between formats]
 *
 *  Error codes use direct table lookup (S.2d.5a) via accessor macros (S.2d.5b).
 *  No conversion utilities needed — codes map directly to layer×offset indices.
 *
 *  FUTURE: If bidirectional code↔string conversion needed, add here.
 *  @{ */

/* No conversion utilities — error codes use direct lookup */

/** @} */ /* end S.2d.5c CONVERSION */

/** @} */ /* end S.2d.5 ERROR */

/** @} */ /* end S.2d LOOKUP */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e CODES — Derived error code definitions [CODES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_codes S.2e CODES — Derived error definitions
 * @ingroup cpisi_error_domain
 * @brief    All error codes derived from layer, tier, and position components.
 *
 * PURPOSE: Define error codes using CPISI_MAKE_ERROR macro rather than
 *          hardcoded values. Each code is derived from its structural position.
 *
 * DERIVATION:
 *   Each error = CPISI_MAKE_ERROR(layer, tier, position)
 *              = (layer × 27) + (tier × 9) + position
 *
 * UNIVERSALITY:
 *   Position meanings are universal across all layers and tiers:
 *     - Position 1 (NULL) is ALWAYS null/missing errors
 *     - Position 3 (BOUNDS) is ALWAYS bounds/range errors
 *     - etc.
 *
 * STRUCTURE:
 *   S.2e.a KERNEL    — Layer 0 errors (0-26)
 *   S.2e.b SYSTEM    — Layer 1 errors (27-53)
 *   S.2e.c RUNTIME   — Layer 2 errors (54-80)
 *   S.2e.d FRAMEWORK — Layer 3 errors (81-107)
 *   S.2e.e HEALTH    — Layer 4 errors (108-134) [CENTER]
 *   S.2e.f NETWORK   — Layer 5 errors (135-161)
 *   S.2e.g STORAGE   — Layer 6 errors (162-188)
 *   S.2e.h ENGINE    — Layer 7 errors (189-215)
 *   S.2e.i APP       — Layer 8 errors (216-242)
 *   S.2e.j METADATA  — Codes 243-255
 *
 * Builds FROM: S.2b.5 CPISI_MAKE_ERROR, S.2a tier/position values
 * Builds TO:   Application code, error handling
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */


/** @name S.2e.a KERNEL ERRORS — Layer 0 (codes 0-26)
 *  @brief Foundation errors: universal errors that can occur anywhere.
 *
 *  Position -4 in balanced ternary. Most foundational layer.
 *  @{ */

// ── KERNEL CORE (0-8) — Fundamental success/failure states ──
// CPISI_OK (0) — inherited from types.h S.3d.2 SUCCESS

/** @brief Null pointer (code 1). */
#define CPISI_ERR_KERNEL_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_CORE, CPISI_POS_NULL)

/** @brief Invalid parameter (code 2). */
#define CPISI_ERR_KERNEL_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_CORE, CPISI_POS_INVALID)

/** @brief Out of bounds (code 3). */
#define CPISI_ERR_KERNEL_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_CORE, CPISI_POS_BOUNDS)

/** @brief Type mismatch (code 4). */
#define CPISI_ERR_KERNEL_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_CORE, CPISI_POS_TYPE)

/** @brief Invalid state (code 5). */
#define CPISI_ERR_KERNEL_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_CORE, CPISI_POS_STATE)

/** @brief Not initialized (code 6). */
#define CPISI_ERR_KERNEL_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_CORE, CPISI_POS_INIT)

/** @brief Already shutdown (code 7). */
#define CPISI_ERR_KERNEL_SHUTDOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_CORE, CPISI_POS_CLEANUP)

/** @brief Unknown error (code 8). */
#define CPISI_ERR_KERNEL_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_CORE, CPISI_POS_UNKNOWN)

// ── KERNEL EXT (9-17) — Resource/operation errors ──

/** @brief Out of memory (code 9). */
#define CPISI_ERR_KERNEL_NOMEM \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_EXT, CPISI_POS_SUCCESS)

/** @brief No space left (code 10). */
#define CPISI_ERR_KERNEL_NOSPACE \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_EXT, CPISI_POS_NULL)

/** @brief Resource busy (code 11). */
#define CPISI_ERR_KERNEL_BUSY \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_EXT, CPISI_POS_INVALID)

/** @brief Resource locked (code 12). */
#define CPISI_ERR_KERNEL_LOCKED \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_EXT, CPISI_POS_BOUNDS)

/** @brief Operation timeout (code 13). */
#define CPISI_ERR_KERNEL_TIMEOUT \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_EXT, CPISI_POS_TYPE)

/** @brief Operation cancelled (code 14). */
#define CPISI_ERR_KERNEL_CANCELLED \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_EXT, CPISI_POS_STATE)

/** @brief Operation interrupted (code 15). */
#define CPISI_ERR_KERNEL_INTERRUPTED \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_EXT, CPISI_POS_INIT)

/** @brief Retry required (code 16). */
#define CPISI_ERR_KERNEL_RETRY \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_EXT, CPISI_POS_CLEANUP)

/** @brief Permission denied (code 17). */
#define CPISI_ERR_KERNEL_DENIED \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_EXT, CPISI_POS_UNKNOWN)

// ── KERNEL SYS (18-26) — Structural/flow errors ──

/** @brief Not found (code 18). */
#define CPISI_ERR_KERNEL_NOTFOUND \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_SYS, CPISI_POS_SUCCESS)

/** @brief Already exists (code 19). */
#define CPISI_ERR_KERNEL_EXISTS \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_SYS, CPISI_POS_NULL)

/** @brief Empty (code 20). */
#define CPISI_ERR_KERNEL_EMPTY \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_SYS, CPISI_POS_INVALID)

/** @brief Full (code 21). */
#define CPISI_ERR_KERNEL_FULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_SYS, CPISI_POS_BOUNDS)

/** @brief Overflow (code 22). */
#define CPISI_ERR_KERNEL_OVERFLOW \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_SYS, CPISI_POS_TYPE)

/** @brief Underflow (code 23). */
#define CPISI_ERR_KERNEL_UNDERFLOW \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_SYS, CPISI_POS_STATE)

/** @brief Not supported (code 24). */
#define CPISI_ERR_KERNEL_UNSUPPORTED \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_SYS, CPISI_POS_INIT)

/** @brief Not implemented (code 25). */
#define CPISI_ERR_KERNEL_UNIMPLEMENTED \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_SYS, CPISI_POS_CLEANUP)

/** @brief Deprecated (code 26). */
#define CPISI_ERR_KERNEL_DEPRECATED \
    CPISI_MAKE_ERROR(CPISI_LAYER_KERNEL, CPISI_TIER_SYS, CPISI_POS_UNKNOWN)

/** @} */ /* end S.2e.a KERNEL ERRORS */


/** @name S.2e.b SYSTEM ERRORS — Layer 1 (codes 27-53)
 *  @brief OS/platform errors: system calls, processes, signals.
 *
 *  Position -3 in balanced ternary. Operating system interface layer.
 *  @{ */

// ── SYSTEM CORE (27-35) — Fundamental system states ──

/** @brief System operation succeeded (code 27). */
#define CPISI_OK_SYSTEM \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_CORE, CPISI_POS_SUCCESS)

/** @brief System null handle (code 28). */
#define CPISI_ERR_SYSTEM_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_CORE, CPISI_POS_NULL)

/** @brief Invalid system parameter (code 29). */
#define CPISI_ERR_SYSTEM_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_CORE, CPISI_POS_INVALID)

/** @brief System bounds exceeded (code 30). */
#define CPISI_ERR_SYSTEM_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_CORE, CPISI_POS_BOUNDS)

/** @brief System type mismatch (code 31). */
#define CPISI_ERR_SYSTEM_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_CORE, CPISI_POS_TYPE)

/** @brief Invalid system state (code 32). */
#define CPISI_ERR_SYSTEM_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_CORE, CPISI_POS_STATE)

/** @brief System not initialized (code 33). */
#define CPISI_ERR_SYSTEM_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_CORE, CPISI_POS_INIT)

/** @brief System already shutdown (code 34). */
#define CPISI_ERR_SYSTEM_SHUTDOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_CORE, CPISI_POS_CLEANUP)

/** @brief Unknown system error (code 35). */
#define CPISI_ERR_SYSTEM_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_CORE, CPISI_POS_UNKNOWN)

// ── SYSTEM EXT (36-44) — Process/signal errors ──

/** @brief Process creation failed (code 36). */
#define CPISI_ERR_SYSTEM_PROCESS \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_EXT, CPISI_POS_SUCCESS)

/** @brief Signal handler null (code 37). */
#define CPISI_ERR_SYSTEM_SIGNAL_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_EXT, CPISI_POS_NULL)

/** @brief Invalid signal (code 38). */
#define CPISI_ERR_SYSTEM_SIGNAL_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_EXT, CPISI_POS_INVALID)

/** @brief PID out of range (code 39). */
#define CPISI_ERR_SYSTEM_PID_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_EXT, CPISI_POS_BOUNDS)

/** @brief Argument type error (code 40). */
#define CPISI_ERR_SYSTEM_ARG_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_EXT, CPISI_POS_TYPE)

/** @brief Process in wrong state (code 41). */
#define CPISI_ERR_SYSTEM_PROC_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_EXT, CPISI_POS_STATE)

/** @brief Process not started (code 42). */
#define CPISI_ERR_SYSTEM_PROC_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_EXT, CPISI_POS_INIT)

/** @brief Process already terminated (code 43). */
#define CPISI_ERR_SYSTEM_PROC_DEAD \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_EXT, CPISI_POS_CLEANUP)

/** @brief Unknown process error (code 44). */
#define CPISI_ERR_SYSTEM_PROC_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_EXT, CPISI_POS_UNKNOWN)

// ── SYSTEM SYS (45-53) — Syscall/permission errors ──

/** @brief Syscall succeeded (code 45). */
#define CPISI_OK_SYSCALL \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_SYS, CPISI_POS_SUCCESS)

/** @brief Syscall returned null (code 46). */
#define CPISI_ERR_SYSCALL_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_SYS, CPISI_POS_NULL)

/** @brief Invalid syscall (code 47). */
#define CPISI_ERR_SYSCALL_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_SYS, CPISI_POS_INVALID)

/** @brief Syscall argument bounds (code 48). */
#define CPISI_ERR_SYSCALL_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_SYS, CPISI_POS_BOUNDS)

/** @brief Permission type error (code 49). */
#define CPISI_ERR_SYSCALL_PERM_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_SYS, CPISI_POS_TYPE)

/** @brief Permission denied (code 50). */
#define CPISI_ERR_SYSCALL_PERM \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_SYS, CPISI_POS_STATE)

/** @brief Syscall not available (code 51). */
#define CPISI_ERR_SYSCALL_UNAVAIL \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_SYS, CPISI_POS_INIT)

/** @brief Syscall interrupted (code 52). */
#define CPISI_ERR_SYSCALL_INTR \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_SYS, CPISI_POS_CLEANUP)

/** @brief Unknown syscall error (code 53). */
#define CPISI_ERR_SYSCALL_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_SYSTEM, CPISI_TIER_SYS, CPISI_POS_UNKNOWN)

/** @} */ /* end S.2e.b SYSTEM ERRORS */


/** @name S.2e.c RUNTIME ERRORS — Layer 2 (codes 54-80)
 *  @brief Runtime errors: memory, threads, synchronization.
 *
 *  Position -2 in balanced ternary. Runtime environment layer.
 *  @{ */

// ── RUNTIME CORE (54-62) — Fundamental runtime states ──

/** @brief Runtime operation succeeded (code 54). */
#define CPISI_OK_RUNTIME \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_CORE, CPISI_POS_SUCCESS)

/** @brief Runtime null pointer (code 55). */
#define CPISI_ERR_RUNTIME_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_CORE, CPISI_POS_NULL)

/** @brief Invalid runtime parameter (code 56). */
#define CPISI_ERR_RUNTIME_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_CORE, CPISI_POS_INVALID)

/** @brief Runtime bounds exceeded (code 57). */
#define CPISI_ERR_RUNTIME_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_CORE, CPISI_POS_BOUNDS)

/** @brief Runtime type mismatch (code 58). */
#define CPISI_ERR_RUNTIME_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_CORE, CPISI_POS_TYPE)

/** @brief Invalid runtime state (code 59). */
#define CPISI_ERR_RUNTIME_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_CORE, CPISI_POS_STATE)

/** @brief Runtime not initialized (code 60). */
#define CPISI_ERR_RUNTIME_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_CORE, CPISI_POS_INIT)

/** @brief Runtime already shutdown (code 61). */
#define CPISI_ERR_RUNTIME_SHUTDOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_CORE, CPISI_POS_CLEANUP)

/** @brief Unknown runtime error (code 62). */
#define CPISI_ERR_RUNTIME_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_CORE, CPISI_POS_UNKNOWN)

// ── RUNTIME EXT (63-71) — Memory management errors ──

/** @brief Memory allocation succeeded (code 63). */
#define CPISI_OK_MEMORY \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_EXT, CPISI_POS_SUCCESS)

/** @brief Memory pointer null (code 64). */
#define CPISI_ERR_MEMORY_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_EXT, CPISI_POS_NULL)

/** @brief Invalid memory operation (code 65). */
#define CPISI_ERR_MEMORY_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_EXT, CPISI_POS_INVALID)

/** @brief Memory access out of bounds (code 66). */
#define CPISI_ERR_MEMORY_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_EXT, CPISI_POS_BOUNDS)

/** @brief Memory type mismatch (code 67). */
#define CPISI_ERR_MEMORY_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_EXT, CPISI_POS_TYPE)

/** @brief Memory corruption (code 68). */
#define CPISI_ERR_MEMORY_CORRUPT \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_EXT, CPISI_POS_STATE)

/** @brief Memory not allocated (code 69). */
#define CPISI_ERR_MEMORY_UNALLOC \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_EXT, CPISI_POS_INIT)

/** @brief Double free (code 70). */
#define CPISI_ERR_MEMORY_DOUBLE_FREE \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_EXT, CPISI_POS_CLEANUP)

/** @brief Unknown memory error (code 71). */
#define CPISI_ERR_MEMORY_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_EXT, CPISI_POS_UNKNOWN)

// ── RUNTIME SYS (72-80) — Thread/sync errors ──

/** @brief Thread operation succeeded (code 72). */
#define CPISI_OK_THREAD \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_SYS, CPISI_POS_SUCCESS)

/** @brief Thread handle null (code 73). */
#define CPISI_ERR_THREAD_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_SYS, CPISI_POS_NULL)

/** @brief Invalid thread operation (code 74). */
#define CPISI_ERR_THREAD_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_SYS, CPISI_POS_INVALID)

/** @brief Thread count exceeded (code 75). */
#define CPISI_ERR_THREAD_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_SYS, CPISI_POS_BOUNDS)

/** @brief Thread type error (code 76). */
#define CPISI_ERR_THREAD_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_SYS, CPISI_POS_TYPE)

/** @brief Deadlock detected (code 77). */
#define CPISI_ERR_THREAD_DEADLOCK \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_SYS, CPISI_POS_STATE)

/** @brief Thread not started (code 78). */
#define CPISI_ERR_THREAD_NOT_STARTED \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_SYS, CPISI_POS_INIT)

/** @brief Thread already joined (code 79). */
#define CPISI_ERR_THREAD_JOINED \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_SYS, CPISI_POS_CLEANUP)

/** @brief Unknown thread error (code 80). */
#define CPISI_ERR_THREAD_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_RUNTIME, CPISI_TIER_SYS, CPISI_POS_UNKNOWN)

/** @} */ /* end S.2e.c RUNTIME ERRORS */


/** @name S.2e.d FRAMEWORK ERRORS — Layer 3 (codes 81-107)
 *  @brief Framework errors: modules, plugins, lifecycle.
 *
 *  Position -1 in balanced ternary. Component framework layer.
 *  @{ */

// ── FRAMEWORK CORE (81-89) — Fundamental framework states ──

/** @brief Framework operation succeeded (code 81). */
#define CPISI_OK_FRAMEWORK \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_CORE, CPISI_POS_SUCCESS)

/** @brief Framework null pointer (code 82). */
#define CPISI_ERR_FRAMEWORK_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_CORE, CPISI_POS_NULL)

/** @brief Invalid framework parameter (code 83). */
#define CPISI_ERR_FRAMEWORK_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_CORE, CPISI_POS_INVALID)

/** @brief Framework bounds exceeded (code 84). */
#define CPISI_ERR_FRAMEWORK_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_CORE, CPISI_POS_BOUNDS)

/** @brief Framework type mismatch (code 85). */
#define CPISI_ERR_FRAMEWORK_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_CORE, CPISI_POS_TYPE)

/** @brief Invalid framework state (code 86). */
#define CPISI_ERR_FRAMEWORK_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_CORE, CPISI_POS_STATE)

/** @brief Framework not initialized (code 87). */
#define CPISI_ERR_FRAMEWORK_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_CORE, CPISI_POS_INIT)

/** @brief Framework already shutdown (code 88). */
#define CPISI_ERR_FRAMEWORK_SHUTDOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_CORE, CPISI_POS_CLEANUP)

/** @brief Unknown framework error (code 89). */
#define CPISI_ERR_FRAMEWORK_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_CORE, CPISI_POS_UNKNOWN)

// ── FRAMEWORK EXT (90-98) — Module errors ──

/** @brief Module load succeeded (code 90). */
#define CPISI_OK_MODULE \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_EXT, CPISI_POS_SUCCESS)

/** @brief Module handle null (code 91). */
#define CPISI_ERR_MODULE_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_EXT, CPISI_POS_NULL)

/** @brief Invalid module (code 92). */
#define CPISI_ERR_MODULE_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_EXT, CPISI_POS_INVALID)

/** @brief Module version out of bounds (code 93). */
#define CPISI_ERR_MODULE_VERSION \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_EXT, CPISI_POS_BOUNDS)

/** @brief Module interface type error (code 94). */
#define CPISI_ERR_MODULE_INTERFACE \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_EXT, CPISI_POS_TYPE)

/** @brief Module in wrong state (code 95). */
#define CPISI_ERR_MODULE_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_EXT, CPISI_POS_STATE)

/** @brief Module not loaded (code 96). */
#define CPISI_ERR_MODULE_NOT_LOADED \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_EXT, CPISI_POS_INIT)

/** @brief Module already unloaded (code 97). */
#define CPISI_ERR_MODULE_UNLOADED \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_EXT, CPISI_POS_CLEANUP)

/** @brief Unknown module error (code 98). */
#define CPISI_ERR_MODULE_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_EXT, CPISI_POS_UNKNOWN)

// ── FRAMEWORK SYS (99-107) — Plugin errors ──

/** @brief Plugin load succeeded (code 99). */
#define CPISI_OK_PLUGIN \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_SYS, CPISI_POS_SUCCESS)

/** @brief Plugin handle null (code 100). */
#define CPISI_ERR_PLUGIN_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_SYS, CPISI_POS_NULL)

/** @brief Invalid plugin (code 101). */
#define CPISI_ERR_PLUGIN_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_SYS, CPISI_POS_INVALID)

/** @brief Plugin API version mismatch (code 102). */
#define CPISI_ERR_PLUGIN_API \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_SYS, CPISI_POS_BOUNDS)

/** @brief Plugin type conflict (code 103). */
#define CPISI_ERR_PLUGIN_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_SYS, CPISI_POS_TYPE)

/** @brief Plugin in wrong state (code 104). */
#define CPISI_ERR_PLUGIN_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_SYS, CPISI_POS_STATE)

/** @brief Plugin not registered (code 105). */
#define CPISI_ERR_PLUGIN_NOT_REG \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_SYS, CPISI_POS_INIT)

/** @brief Plugin already unregistered (code 106). */
#define CPISI_ERR_PLUGIN_UNREG \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_SYS, CPISI_POS_CLEANUP)

/** @brief Unknown plugin error (code 107). */
#define CPISI_ERR_PLUGIN_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_FRAMEWORK, CPISI_TIER_SYS, CPISI_POS_UNKNOWN)

/** @} */ /* end S.2e.d FRAMEWORK ERRORS */


/** @name S.2e.e HEALTH ERRORS — Layer 4 (codes 108-134)
 *  @brief Health/diagnostic errors: monitoring, telemetry, recovery.
 *
 *  Position 0 in balanced ternary. CENTER LAYER - balance point.
 *  @{ */

// ── HEALTH CORE (108-116) — Fundamental health states ──

/** @brief Health check succeeded (code 108). */
#define CPISI_OK_HEALTH \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_CORE, CPISI_POS_SUCCESS)

/** @brief Health handle null (code 109). */
#define CPISI_ERR_HEALTH_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_CORE, CPISI_POS_NULL)

/** @brief Invalid health parameter (code 110). */
#define CPISI_ERR_HEALTH_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_CORE, CPISI_POS_INVALID)

/** @brief Health metric out of bounds (code 111). */
#define CPISI_ERR_HEALTH_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_CORE, CPISI_POS_BOUNDS)

/** @brief Health type mismatch (code 112). */
#define CPISI_ERR_HEALTH_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_CORE, CPISI_POS_TYPE)

/** @brief Invalid health state (code 113). */
#define CPISI_ERR_HEALTH_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_CORE, CPISI_POS_STATE)

/** @brief Health monitor not initialized (code 114). */
#define CPISI_ERR_HEALTH_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_CORE, CPISI_POS_INIT)

/** @brief Health monitor shutdown (code 115). */
#define CPISI_ERR_HEALTH_SHUTDOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_CORE, CPISI_POS_CLEANUP)

/** @brief Unknown health error (code 116). */
#define CPISI_ERR_HEALTH_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_CORE, CPISI_POS_UNKNOWN)

// ── HEALTH EXT (117-125) — Diagnostic errors ──

/** @brief Diagnostic succeeded (code 117). */
#define CPISI_OK_DIAG \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_EXT, CPISI_POS_SUCCESS)

/** @brief Diagnostic context null (code 118). */
#define CPISI_ERR_DIAG_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_EXT, CPISI_POS_NULL)

/** @brief Invalid diagnostic (code 119). */
#define CPISI_ERR_DIAG_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_EXT, CPISI_POS_INVALID)

/** @brief Diagnostic depth exceeded (code 120). */
#define CPISI_ERR_DIAG_DEPTH \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_EXT, CPISI_POS_BOUNDS)

/** @brief Diagnostic type error (code 121). */
#define CPISI_ERR_DIAG_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_EXT, CPISI_POS_TYPE)

/** @brief Diagnostic state invalid (code 122). */
#define CPISI_ERR_DIAG_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_EXT, CPISI_POS_STATE)

/** @brief Diagnostic not started (code 123). */
#define CPISI_ERR_DIAG_NOT_STARTED \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_EXT, CPISI_POS_INIT)

/** @brief Diagnostic complete (code 124). */
#define CPISI_ERR_DIAG_COMPLETE \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_EXT, CPISI_POS_CLEANUP)

/** @brief Unknown diagnostic error (code 125). */
#define CPISI_ERR_DIAG_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_EXT, CPISI_POS_UNKNOWN)

// ── HEALTH SYS (126-134) — Recovery errors ──

/** @brief Recovery succeeded (code 126). */
#define CPISI_OK_RECOVERY \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_SYS, CPISI_POS_SUCCESS)

/** @brief Recovery context null (code 127). */
#define CPISI_ERR_RECOVERY_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_SYS, CPISI_POS_NULL)

/** @brief Invalid recovery operation (code 128). */
#define CPISI_ERR_RECOVERY_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_SYS, CPISI_POS_INVALID)

/** @brief Recovery retry limit (code 129). */
#define CPISI_ERR_RECOVERY_RETRY \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_SYS, CPISI_POS_BOUNDS)

/** @brief Recovery type mismatch (code 130). */
#define CPISI_ERR_RECOVERY_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_SYS, CPISI_POS_TYPE)

/** @brief Cannot recover from state (code 131). */
#define CPISI_ERR_RECOVERY_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_SYS, CPISI_POS_STATE)

/** @brief Recovery not available (code 132). */
#define CPISI_ERR_RECOVERY_UNAVAIL \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_SYS, CPISI_POS_INIT)

/** @brief Recovery already complete (code 133). */
#define CPISI_ERR_RECOVERY_DONE \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_SYS, CPISI_POS_CLEANUP)

/** @brief Unknown recovery error (code 134). */
#define CPISI_ERR_RECOVERY_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_HEALTH, CPISI_TIER_SYS, CPISI_POS_UNKNOWN)

/** @} */ /* end S.2e.e HEALTH ERRORS */


/** @name S.2e.f NETWORK ERRORS — Layer 5 (codes 135-161)
 *  @brief Network errors: connections, protocols, messaging.
 *
 *  Position +1 in balanced ternary. External communication layer.
 *  @{ */

// ── NETWORK CORE (135-143) — Fundamental network states ──

/** @brief Network operation succeeded (code 135). */
#define CPISI_OK_NETWORK \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_CORE, CPISI_POS_SUCCESS)

/** @brief Network handle null (code 136). */
#define CPISI_ERR_NETWORK_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_CORE, CPISI_POS_NULL)

/** @brief Invalid network parameter (code 137). */
#define CPISI_ERR_NETWORK_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_CORE, CPISI_POS_INVALID)

/** @brief Network bounds exceeded (code 138). */
#define CPISI_ERR_NETWORK_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_CORE, CPISI_POS_BOUNDS)

/** @brief Network type mismatch (code 139). */
#define CPISI_ERR_NETWORK_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_CORE, CPISI_POS_TYPE)

/** @brief Invalid network state (code 140). */
#define CPISI_ERR_NETWORK_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_CORE, CPISI_POS_STATE)

/** @brief Network not initialized (code 141). */
#define CPISI_ERR_NETWORK_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_CORE, CPISI_POS_INIT)

/** @brief Network shutdown (code 142). */
#define CPISI_ERR_NETWORK_SHUTDOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_CORE, CPISI_POS_CLEANUP)

/** @brief Unknown network error (code 143). */
#define CPISI_ERR_NETWORK_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_CORE, CPISI_POS_UNKNOWN)

// ── NETWORK EXT (144-152) — Connection errors ──

/** @brief Connection succeeded (code 144). */
#define CPISI_OK_CONN \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_EXT, CPISI_POS_SUCCESS)

/** @brief Connection handle null (code 145). */
#define CPISI_ERR_CONN_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_EXT, CPISI_POS_NULL)

/** @brief Invalid connection (code 146). */
#define CPISI_ERR_CONN_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_EXT, CPISI_POS_INVALID)

/** @brief Connection limit exceeded (code 147). */
#define CPISI_ERR_CONN_LIMIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_EXT, CPISI_POS_BOUNDS)

/** @brief Connection type mismatch (code 148). */
#define CPISI_ERR_CONN_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_EXT, CPISI_POS_TYPE)

/** @brief Connection refused (code 149). */
#define CPISI_ERR_CONN_REFUSED \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_EXT, CPISI_POS_STATE)

/** @brief Connection not open (code 150). */
#define CPISI_ERR_CONN_NOT_OPEN \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_EXT, CPISI_POS_INIT)

/** @brief Connection closed (code 151). */
#define CPISI_ERR_CONN_CLOSED \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_EXT, CPISI_POS_CLEANUP)

/** @brief Unknown connection error (code 152). */
#define CPISI_ERR_CONN_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_EXT, CPISI_POS_UNKNOWN)

// ── NETWORK SYS (153-161) — Protocol errors ──

/** @brief Protocol succeeded (code 153). */
#define CPISI_OK_PROTO \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_SYS, CPISI_POS_SUCCESS)

/** @brief Protocol context null (code 154). */
#define CPISI_ERR_PROTO_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_SYS, CPISI_POS_NULL)

/** @brief Invalid protocol (code 155). */
#define CPISI_ERR_PROTO_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_SYS, CPISI_POS_INVALID)

/** @brief Protocol version mismatch (code 156). */
#define CPISI_ERR_PROTO_VERSION \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_SYS, CPISI_POS_BOUNDS)

/** @brief Protocol type error (code 157). */
#define CPISI_ERR_PROTO_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_SYS, CPISI_POS_TYPE)

/** @brief Protocol state error (code 158). */
#define CPISI_ERR_PROTO_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_SYS, CPISI_POS_STATE)

/** @brief Protocol not negotiated (code 159). */
#define CPISI_ERR_PROTO_NOT_NEG \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_SYS, CPISI_POS_INIT)

/** @brief Protocol terminated (code 160). */
#define CPISI_ERR_PROTO_TERM \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_SYS, CPISI_POS_CLEANUP)

/** @brief Unknown protocol error (code 161). */
#define CPISI_ERR_PROTO_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_NETWORK, CPISI_TIER_SYS, CPISI_POS_UNKNOWN)

/** @} */ /* end S.2e.f NETWORK ERRORS */


/** @name S.2e.g STORAGE ERRORS — Layer 6 (codes 162-188)
 *  @brief Storage errors: files, database, persistence.
 *
 *  Position +2 in balanced ternary. Persistence layer.
 *  @{ */

// ── STORAGE CORE (162-170) — Fundamental storage states ──

/** @brief Storage operation succeeded (code 162). */
#define CPISI_OK_STORAGE \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_CORE, CPISI_POS_SUCCESS)

/** @brief Storage handle null (code 163). */
#define CPISI_ERR_STORAGE_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_CORE, CPISI_POS_NULL)

/** @brief Invalid storage parameter (code 164). */
#define CPISI_ERR_STORAGE_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_CORE, CPISI_POS_INVALID)

/** @brief Storage bounds exceeded (code 165). */
#define CPISI_ERR_STORAGE_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_CORE, CPISI_POS_BOUNDS)

/** @brief Storage type mismatch (code 166). */
#define CPISI_ERR_STORAGE_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_CORE, CPISI_POS_TYPE)

/** @brief Invalid storage state (code 167). */
#define CPISI_ERR_STORAGE_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_CORE, CPISI_POS_STATE)

/** @brief Storage not initialized (code 168). */
#define CPISI_ERR_STORAGE_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_CORE, CPISI_POS_INIT)

/** @brief Storage shutdown (code 169). */
#define CPISI_ERR_STORAGE_SHUTDOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_CORE, CPISI_POS_CLEANUP)

/** @brief Unknown storage error (code 170). */
#define CPISI_ERR_STORAGE_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_CORE, CPISI_POS_UNKNOWN)

// ── STORAGE EXT (171-179) — File errors ──

/** @brief File operation succeeded (code 171). */
#define CPISI_OK_FILE \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_EXT, CPISI_POS_SUCCESS)

/** @brief File handle null (code 172). */
#define CPISI_ERR_FILE_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_EXT, CPISI_POS_NULL)

/** @brief Invalid file (code 173). */
#define CPISI_ERR_FILE_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_EXT, CPISI_POS_INVALID)

/** @brief File size exceeded (code 174). */
#define CPISI_ERR_FILE_SIZE \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_EXT, CPISI_POS_BOUNDS)

/** @brief File format type error (code 175). */
#define CPISI_ERR_FILE_FORMAT \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_EXT, CPISI_POS_TYPE)

/** @brief File in wrong state (code 176). */
#define CPISI_ERR_FILE_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_EXT, CPISI_POS_STATE)

/** @brief File not open (code 177). */
#define CPISI_ERR_FILE_NOT_OPEN \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_EXT, CPISI_POS_INIT)

/** @brief File already closed (code 178). */
#define CPISI_ERR_FILE_CLOSED \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_EXT, CPISI_POS_CLEANUP)

/** @brief Unknown file error (code 179). */
#define CPISI_ERR_FILE_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_EXT, CPISI_POS_UNKNOWN)

// ── STORAGE SYS (180-188) — Database errors ──

/** @brief Database operation succeeded (code 180). */
#define CPISI_OK_DB \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_SYS, CPISI_POS_SUCCESS)

/** @brief Database handle null (code 181). */
#define CPISI_ERR_DB_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_SYS, CPISI_POS_NULL)

/** @brief Invalid database operation (code 182). */
#define CPISI_ERR_DB_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_SYS, CPISI_POS_INVALID)

/** @brief Database constraint error (code 183). */
#define CPISI_ERR_DB_CONSTRAINT \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_SYS, CPISI_POS_BOUNDS)

/** @brief Database type error (code 184). */
#define CPISI_ERR_DB_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_SYS, CPISI_POS_TYPE)

/** @brief Database transaction state (code 185). */
#define CPISI_ERR_DB_TXN_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_SYS, CPISI_POS_STATE)

/** @brief Database not connected (code 186). */
#define CPISI_ERR_DB_NOT_CONN \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_SYS, CPISI_POS_INIT)

/** @brief Database connection closed (code 187). */
#define CPISI_ERR_DB_CLOSED \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_SYS, CPISI_POS_CLEANUP)

/** @brief Unknown database error (code 188). */
#define CPISI_ERR_DB_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_STORAGE, CPISI_TIER_SYS, CPISI_POS_UNKNOWN)

/** @} */ /* end S.2e.g STORAGE ERRORS */


/** @name S.2e.h ENGINE ERRORS — Layer 7 (codes 189-215)
 *  @brief Engine errors: rendering, physics, audio.
 *
 *  Position +3 in balanced ternary. Core engine layer.
 *  @{ */

// ── ENGINE CORE (189-197) — Fundamental engine states ──

/** @brief Engine operation succeeded (code 189). */
#define CPISI_OK_ENGINE \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_CORE, CPISI_POS_SUCCESS)

/** @brief Engine handle null (code 190). */
#define CPISI_ERR_ENGINE_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_CORE, CPISI_POS_NULL)

/** @brief Invalid engine parameter (code 191). */
#define CPISI_ERR_ENGINE_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_CORE, CPISI_POS_INVALID)

/** @brief Engine bounds exceeded (code 192). */
#define CPISI_ERR_ENGINE_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_CORE, CPISI_POS_BOUNDS)

/** @brief Engine type mismatch (code 193). */
#define CPISI_ERR_ENGINE_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_CORE, CPISI_POS_TYPE)

/** @brief Invalid engine state (code 194). */
#define CPISI_ERR_ENGINE_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_CORE, CPISI_POS_STATE)

/** @brief Engine not initialized (code 195). */
#define CPISI_ERR_ENGINE_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_CORE, CPISI_POS_INIT)

/** @brief Engine shutdown (code 196). */
#define CPISI_ERR_ENGINE_SHUTDOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_CORE, CPISI_POS_CLEANUP)

/** @brief Unknown engine error (code 197). */
#define CPISI_ERR_ENGINE_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_CORE, CPISI_POS_UNKNOWN)

// ── ENGINE EXT (198-206) — Rendering errors ──

/** @brief Render succeeded (code 198). */
#define CPISI_OK_RENDER \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_EXT, CPISI_POS_SUCCESS)

/** @brief Render context null (code 199). */
#define CPISI_ERR_RENDER_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_EXT, CPISI_POS_NULL)

/** @brief Invalid render operation (code 200). */
#define CPISI_ERR_RENDER_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_EXT, CPISI_POS_INVALID)

/** @brief Render buffer overflow (code 201). */
#define CPISI_ERR_RENDER_BUFFER \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_EXT, CPISI_POS_BOUNDS)

/** @brief Render type error (code 202). */
#define CPISI_ERR_RENDER_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_EXT, CPISI_POS_TYPE)

/** @brief Render state error (code 203). */
#define CPISI_ERR_RENDER_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_EXT, CPISI_POS_STATE)

/** @brief Render not initialized (code 204). */
#define CPISI_ERR_RENDER_NOT_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_EXT, CPISI_POS_INIT)

/** @brief Render frame dropped (code 205). */
#define CPISI_ERR_RENDER_DROPPED \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_EXT, CPISI_POS_CLEANUP)

/** @brief Unknown render error (code 206). */
#define CPISI_ERR_RENDER_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_EXT, CPISI_POS_UNKNOWN)

// ── ENGINE SYS (207-215) — Physics/audio errors ──

/** @brief Physics operation succeeded (code 207). */
#define CPISI_OK_PHYSICS \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_SYS, CPISI_POS_SUCCESS)

/** @brief Physics context null (code 208). */
#define CPISI_ERR_PHYSICS_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_SYS, CPISI_POS_NULL)

/** @brief Invalid physics operation (code 209). */
#define CPISI_ERR_PHYSICS_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_SYS, CPISI_POS_INVALID)

/** @brief Physics object limit (code 210). */
#define CPISI_ERR_PHYSICS_LIMIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_SYS, CPISI_POS_BOUNDS)

/** @brief Physics type error (code 211). */
#define CPISI_ERR_PHYSICS_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_SYS, CPISI_POS_TYPE)

/** @brief Physics state error (code 212). */
#define CPISI_ERR_PHYSICS_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_SYS, CPISI_POS_STATE)

/** @brief Physics not initialized (code 213). */
#define CPISI_ERR_PHYSICS_NOT_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_SYS, CPISI_POS_INIT)

/** @brief Physics world destroyed (code 214). */
#define CPISI_ERR_PHYSICS_DESTROYED \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_SYS, CPISI_POS_CLEANUP)

/** @brief Unknown physics error (code 215). */
#define CPISI_ERR_PHYSICS_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_ENGINE, CPISI_TIER_SYS, CPISI_POS_UNKNOWN)

/** @} */ /* end S.2e.h ENGINE ERRORS */


/** @name S.2e.i APP ERRORS — Layer 8 (codes 216-242)
 *  @brief Application errors: UI, input, scene management.
 *
 *  Position +4 in balanced ternary. Application/user layer.
 *  @{ */

// ── APP CORE (216-224) — Fundamental application states ──

/** @brief Application operation succeeded (code 216). */
#define CPISI_OK_APP \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_CORE, CPISI_POS_SUCCESS)

/** @brief Application handle null (code 217). */
#define CPISI_ERR_APP_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_CORE, CPISI_POS_NULL)

/** @brief Invalid application parameter (code 218). */
#define CPISI_ERR_APP_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_CORE, CPISI_POS_INVALID)

/** @brief Application bounds exceeded (code 219). */
#define CPISI_ERR_APP_BOUNDS \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_CORE, CPISI_POS_BOUNDS)

/** @brief Application type mismatch (code 220). */
#define CPISI_ERR_APP_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_CORE, CPISI_POS_TYPE)

/** @brief Invalid application state (code 221). */
#define CPISI_ERR_APP_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_CORE, CPISI_POS_STATE)

/** @brief Application not initialized (code 222). */
#define CPISI_ERR_APP_INIT \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_CORE, CPISI_POS_INIT)

/** @brief Application shutdown (code 223). */
#define CPISI_ERR_APP_SHUTDOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_CORE, CPISI_POS_CLEANUP)

/** @brief Unknown application error (code 224). */
#define CPISI_ERR_APP_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_CORE, CPISI_POS_UNKNOWN)

// ── APP EXT (225-233) — UI errors ──

/** @brief UI operation succeeded (code 225). */
#define CPISI_OK_UI \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_EXT, CPISI_POS_SUCCESS)

/** @brief UI element null (code 226). */
#define CPISI_ERR_UI_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_EXT, CPISI_POS_NULL)

/** @brief Invalid UI element (code 227). */
#define CPISI_ERR_UI_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_EXT, CPISI_POS_INVALID)

/** @brief UI layout bounds (code 228). */
#define CPISI_ERR_UI_LAYOUT \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_EXT, CPISI_POS_BOUNDS)

/** @brief UI type error (code 229). */
#define CPISI_ERR_UI_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_EXT, CPISI_POS_TYPE)

/** @brief UI state error (code 230). */
#define CPISI_ERR_UI_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_EXT, CPISI_POS_STATE)

/** @brief UI not created (code 231). */
#define CPISI_ERR_UI_NOT_CREATED \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_EXT, CPISI_POS_INIT)

/** @brief UI disposed (code 232). */
#define CPISI_ERR_UI_DISPOSED \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_EXT, CPISI_POS_CLEANUP)

/** @brief Unknown UI error (code 233). */
#define CPISI_ERR_UI_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_EXT, CPISI_POS_UNKNOWN)

// ── APP SYS (234-242) — Input/scene errors ──

/** @brief Input operation succeeded (code 234). */
#define CPISI_OK_INPUT \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_SYS, CPISI_POS_SUCCESS)

/** @brief Input device null (code 235). */
#define CPISI_ERR_INPUT_NULL \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_SYS, CPISI_POS_NULL)

/** @brief Invalid input (code 236). */
#define CPISI_ERR_INPUT_INVALID \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_SYS, CPISI_POS_INVALID)

/** @brief Input buffer overflow (code 237). */
#define CPISI_ERR_INPUT_BUFFER \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_SYS, CPISI_POS_BOUNDS)

/** @brief Input type error (code 238). */
#define CPISI_ERR_INPUT_TYPE \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_SYS, CPISI_POS_TYPE)

/** @brief Input state error (code 239). */
#define CPISI_ERR_INPUT_STATE \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_SYS, CPISI_POS_STATE)

/** @brief Input not acquired (code 240). */
#define CPISI_ERR_INPUT_NOT_ACQ \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_SYS, CPISI_POS_INIT)

/** @brief Input released (code 241). */
#define CPISI_ERR_INPUT_RELEASED \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_SYS, CPISI_POS_CLEANUP)

/** @brief Unknown input error (code 242). */
#define CPISI_ERR_INPUT_UNKNOWN \
    CPISI_MAKE_ERROR(CPISI_LAYER_APP, CPISI_TIER_SYS, CPISI_POS_UNKNOWN)

/** @} */ /* end S.2e.i APP ERRORS */


/** @name S.2e.j METADATA ERRORS — Codes 243-255
 *  @brief Metadata/bridge errors: ternary overflow, extended codes.
 *
 *  Codes 243-255 are the BRIDGE region (PENTERACT + 13 = 256).
 *  243 = PENTERACT (3^5), marking ternary system boundary.
 *  @{ */

/** @brief Ternary system overflow (code 243). */
#define CPISI_ERR_META_OVERFLOW     243

/** @brief Extended error (requires uint16_t) (code 244). */
#define CPISI_ERR_META_EXTENDED     244

/** @brief Reserved for future use (code 245). */
#define CPISI_ERR_META_RESERVED_245 245

/** @brief Reserved for future use (code 246). */
#define CPISI_ERR_META_RESERVED_246 246

/** @brief Reserved for future use (code 247). */
#define CPISI_ERR_META_RESERVED_247 247

/** @brief Reserved for future use (code 248). */
#define CPISI_ERR_META_RESERVED_248 248

/** @brief Reserved for future use (code 249). */
#define CPISI_ERR_META_RESERVED_249 249

/** @brief Reserved for future use (code 250). */
#define CPISI_ERR_META_RESERVED_250 250

/** @brief Reserved for future use (code 251). */
#define CPISI_ERR_META_RESERVED_251 251

/** @brief Reserved for future use (code 252). */
#define CPISI_ERR_META_RESERVED_252 252

/** @brief Reserved for future use (code 253). */
#define CPISI_ERR_META_RESERVED_253 253

/** @brief Reserved for future use (code 254). */
#define CPISI_ERR_META_RESERVED_254 254

/** @brief Maximum uint8_t error code (code 255). */
#define CPISI_ERR_META_MAX          255

/** @} */ /* end S.2e.j METADATA ERRORS */

/** @} */ /* end S.2e CODES */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Error code enumeration [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_types S.3 TYPES — Error type definitions
 * @ingroup cpisi_error
 * @brief    Type-safe error code enumeration for diagnostic/recovery system.
 *
 * PURPOSE: Domain-specific type definitions for error codes.
 *
 * STRUCTURE:
 *   S.3a ENUMS     — CpisiErrorCode enumeration
 *   S.3b TYPES     — Inherited from types.h S.3d.1
 *
 * NOTE: Error code #defines are in S.2e CODES (defines belong in S.2)
 *
 * Builds FROM: S.2d ERROR LOOKUP (tables available)
 * Builds TO:   BODY block (implementation)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — CpisiErrorCode enumeration [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_enum S.3a ENUMS — CpisiErrorCode enumeration
 * @ingroup cpisi_error_types
 * @brief    Type-safe error code enumeration for diagnostic/recovery system.
 *
 * PURPOSE: Define layer base addresses for error code routing.
 *          Enum wraps the mathematical structure (9 layers × 27 codes).
 *
 * Builds FROM: S.3 TYPES (section context)
 * Builds TO:   S.3b TYPES
 *
 * STRUCTURE:
 * │  MATHEMATICAL FOUNDATION — 243 + 13 = 256:                                 │
 * │                                                                            │
 * │    PENTERACT (243) = 3^5 = 9 layers × 27 codes per layer                   │
 * │    BRIDGE    (13)  = 13 metadata slots                                     │
 * │    BINARY_FIT (256) = 2^8 = uint8_t storage                                │
 * │                                                                            │
 * │  CODE SPACE LAYOUT:                                                        │
 * │    ┌────────────────────────────────────────────────────────────────────┐  │
 * │    │ Layer    │ First Code │ Last Code │ Count                         │  │
 * │    ├──────────┼────────────┼───────────┼───────────────────────────────┤  │
 * │    │ KERNEL   │     0      │    26     │ 27 (CUBE)                     │  │
 * │    │ SYSTEM   │    27      │    53     │ 27                            │  │
 * │    │ RUNTIME  │    54      │    80     │ 27                            │  │
 * │    │ FRAMEWORK│    81      │   107     │ 27                            │  │
 * │    │ HEALTH   │   108      │   134     │ 27 ← CENTER                   │  │
 * │    │ NETWORK  │   135      │   161     │ 27                            │  │
 * │    │ STORAGE  │   162      │   188     │ 27                            │  │
 * │    │ ENGINE   │   189      │   215     │ 27                            │  │
 * │    │ APP      │   216      │   242     │ 27                            │  │
 * │    │ METADATA │   243      │   255     │ 13 (BRIDGE)                   │  │
 * │    └──────────┴────────────┴───────────┴───────────────────────────────┘  │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.5):                                          │
 * │    - CPISI_LAYER_ERROR_MIN/MAX/CENTER[9] — layer code bounds               │
 * │    - g_cpisi_offset_name[27] — offset names within layer                   │
 * │    - cpisi_layer_error_*() accessors for bounds lookup                     │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Error code enumeration — layer base addresses and metadata region.
 *
 * BOUNDS USED:
 *   - CPISI_VOID (0) — success/no error
 *   - CPISI_CUBE (27) — codes per layer
 *   - CPISI_PENTERACT (243) — 3^5, metadata region start
 *   - CPISI_BINARY_FIT (256) — 2^8, total code space
 *   - CPISI_PRODUCT macro — layer × CUBE calculation
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for error code layer boundaries. Each layer owns 27 codes
 *   (a CUBE). 9 layers × 27 = 243 layer codes, plus 13 metadata slots = 256 total.
 *   Enum provides type-safe layer base addresses for error routing.
 *
 * VALUE MAPPING:
 *   CPISI_ERROR_ENUM_SUCCESS (0) — no error (same as KERNEL base)
 *   CPISI_ERROR_ENUM_KERNEL (0) — KERNEL layer codes 0-26
 *   CPISI_ERROR_ENUM_SYSTEM (27) — SYSTEM layer codes 27-53
 *   CPISI_ERROR_ENUM_RUNTIME (54) — RUNTIME layer codes 54-80
 *   CPISI_ERROR_ENUM_FRAMEWORK (81) — FRAMEWORK layer codes 81-107
 *   CPISI_ERROR_ENUM_HEALTH (108) — HEALTH layer codes 108-134 (CENTER)
 *   CPISI_ERROR_ENUM_NETWORK (135) — NETWORK layer codes 135-161
 *   CPISI_ERROR_ENUM_STORAGE (162) — STORAGE layer codes 162-188
 *   CPISI_ERROR_ENUM_ENGINE (189) — ENGINE layer codes 189-215
 *   CPISI_ERROR_ENUM_APP (216) — APP layer codes 216-242
 *   CPISI_ERROR_ENUM_METADATA (243) — metadata codes 243-255
 *   CPISI_ERROR_ENUM_COUNT (256) — total code space (fits uint8_t)
 *
 * USE CASE:
 *   - Layer routing: layer = code / 27; offset = code % 27;
 *   - Range checking: if (code >= CPISI_ERROR_ENUM_METADATA) handle_meta();
 *   - Error construction: code = layer_base + offset;
 *
 * RELATIONSHIP TO LOOKUP (S.2d.5):
 *   - cpisi_layer_error_min/max/center(layer) → code bounds
 *   - g_cpisi_offset_name[27] → offset names within layer
 */
typedef enum CpisiErrorCode {
    CPISI_ERROR_ENUM_SUCCESS   = CPISI_VOID,                                /*   0 */
    CPISI_ERROR_ENUM_KERNEL    = CPISI_PRODUCT(CPISI_LAYER_KERNEL, CPISI_CUBE),    /*   0 */
    CPISI_ERROR_ENUM_SYSTEM    = CPISI_PRODUCT(CPISI_LAYER_SYSTEM, CPISI_CUBE),    /*  27 */
    CPISI_ERROR_ENUM_RUNTIME   = CPISI_PRODUCT(CPISI_LAYER_RUNTIME, CPISI_CUBE),   /*  54 */
    CPISI_ERROR_ENUM_FRAMEWORK = CPISI_PRODUCT(CPISI_LAYER_FRAMEWORK, CPISI_CUBE), /*  81 */
    CPISI_ERROR_ENUM_HEALTH    = CPISI_PRODUCT(CPISI_LAYER_HEALTH, CPISI_CUBE),    /* 108 */
    CPISI_ERROR_ENUM_NETWORK   = CPISI_PRODUCT(CPISI_LAYER_NETWORK, CPISI_CUBE),   /* 135 */
    CPISI_ERROR_ENUM_STORAGE   = CPISI_PRODUCT(CPISI_LAYER_STORAGE, CPISI_CUBE),   /* 162 */
    CPISI_ERROR_ENUM_ENGINE    = CPISI_PRODUCT(CPISI_LAYER_ENGINE, CPISI_CUBE),    /* 189 */
    CPISI_ERROR_ENUM_APP       = CPISI_PRODUCT(CPISI_LAYER_APP, CPISI_CUBE),       /* 216 */
    CPISI_ERROR_ENUM_METADATA  = CPISI_PENTERACT,                           /* 243 */
    CPISI_ERROR_ENUM_COUNT     = CPISI_BINARY_FIT                           /* 256 */
} CpisiErrorCode;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b TYPES — Scalable error type definition [TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_type S.3b TYPES — Scalable error type definition
 * @ingroup cpisi_error_types
 * @brief    uint16_t typedef for error codes, enabling domain expansion.
 *
 * PURPOSE: Provide a scalable error type that can grow beyond 256 codes.
 *          Core CPI-SI errors (0-255) fit in uint8_t for efficiency.
 *          Extended domains (256+) use the full uint16_t range.
 *          [INHERITED from types.h S.3d.1]
 *
 * DOMAIN STRUCTURE (future expansion):
 *     0-255:     CPI-SI core (current, backward compatible)
 *   256-511:     CPI-SI extended
 *   512-767:     Cornerstone engine
 *   768-1023:    FaithNet
 *   1024-1279:   Bereshit storage
 *   1280+:       Application-specific
 *
 * Builds FROM: S.3a ENUMS
 * Builds TO:   S.4 PROTOTYPES
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Scalable error code type.
 *
 * @details uint16_t allows 65536 possible error codes.
 *          Core CPI-SI errors (0-255) can be cast to uint8_t when needed.
 *          Use CPISI_ERROR_IS_CORE() to check if an error fits in uint8_t.
 *
 * INHERITED FROM types.h S.3d.1:
 *   - typedef uint16_t CpisiError;
 *   - #define CPISI_ERROR_CORE_MAX
 *   - #define CPISI_ERROR_IS_CORE(code)
 *   - #define CPISI_ERROR_TO_U8(code)
 */

/* [Inherited from types.h S.3d.1 — CpisiError type and core range macros] */

/** @} */ /* end S.3b TYPES */

/** @} */ /* end S.3 TYPES */
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Error formula functions [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_prototypes S.4 PROTOTYPES — Error formula functions
 * @brief    Error code composition and decomposition (no table lookup).
 *
 * PURPOSE: Pure derivation functions based on mathematical structure.
 *          No data access — just arithmetic on the ternary architecture.
 *
 * MATHEMATICAL FOUNDATION:
 *   layer  = code / 27           (0-8: which error layer)
 *   tier   = (code % 27) / 9     (0-2: Core/Extended/System)
 *   offset = code % 9            (0-8: position within tier)
 *
 * STRUCTURE:
 *   S.4a LIFECYCLE  — [Reserved: no error lifecycle functions]
 *   S.4b ACCESSORS  — offset, compose, decompose (error code access)
 *   S.4c OPERATIONS — severity mapping (error code operations)
 *
 * Builds FROM: S.3b CpisiError type, S.2b.5 extraction macros
 * Builds TO:   Application code
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Create/Init/Destroy/Clone [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_lifecycle S.4a LIFECYCLE — Create/Init/Destroy/Clone
 * @ingroup cpisi_error_prototypes
 * @brief    Reserved for lifecycle function prototypes.
 *
 * PURPOSE: Object lifecycle management (create, init, destroy, clone).
 *          Currently reserved — error codes are values, not managed resources.
 *
 * Builds FROM: S.4 PROTOTYPES (section context)
 * Builds TO:   S.4b ACCESSORS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Error codes are values, not managed resources] */

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ACCESSORS — Get/Set/Is Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_accessors S.4b ACCESSORS — Get/Set/Is Queries
 * @ingroup cpisi_error_prototypes
 * @brief    Error code composition and decomposition using ternary arithmetic.
 *
 * PURPOSE: Access error code components (offset, compose, decompose).
 *          Pure math functions — no table lookup required.
 *
 * Builds FROM: S.4a LIFECYCLE
 * Builds TO:   S.4c OPERATIONS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Get offset within tier from error code.
 *
 * Extracts the 0-8 offset within a tier using pure math.
 * Formula: code % 9
 *
 * @param err Error code (0-255)
 * @return Offset within tier (0-8)
 *
 * @note Pure formula — no table lookup
 * @see CPISI_ERROR_POSITION() macro for compile-time extraction
 */
static inline uint8_t cpisi_error_offset(CpisiError err) {
    return err % CPISI_FACE;
}

/**
 * @brief Compose error code from layer, tier, and offset.
 *
 * Builds an error code from its components using pure math.
 * Formula: layer×27 + tier×9 + offset
 *
 * @param layer Error layer (0-8, use CPISI_LAYER_* constants)
 * @param tier  Error tier (0-2, use CPISI_TIER_* constants)
 * @param offset Offset within tier (0-8, use CPISI_POS_* constants)
 * @return Composed error code (0-242)
 *
 * @note Pure formula — no table lookup
 * @note Does NOT handle metadata codes (243-255)
 * @see CPISI_MAKE_ERROR() macro for compile-time composition
 */
static inline CpisiError cpisi_error_compose(uint8_t layer, uint8_t tier, uint8_t offset) {
    return (CpisiError)(layer * CPISI_CUBE + tier * CPISI_FACE + offset);
}

/**
 * @brief Decompose error code into layer, tier, and offset.
 *
 * Extracts all components from an error code using pure math.
 *
 * @param err    Error code (0-255)
 * @param layer  Output: layer index (0-9)
 * @param tier   Output: tier index (0-2)
 * @param offset Output: offset within tier (0-8)
 *
 * @note Pure formula — no table lookup
 * @note For metadata codes (243-255), layer=9, tier/offset from remainder
 * @see CPISI_ERROR_LAYER(), CPISI_ERROR_TIER(), CPISI_ERROR_POSITION() macros
 */
static inline void cpisi_error_decompose(CpisiError err, uint8_t *layer, uint8_t *tier, uint8_t *offset) {
    if (err >= CPISI_PENTERACT) {
        *layer = CPISI_LAYER_COUNT;  /* METADATA layer */
        *tier = 0;
        *offset = err - CPISI_PENTERACT;
    } else {
        *layer = err / CPISI_CUBE;
        *tier = (err % CPISI_CUBE) / CPISI_FACE;
        *offset = err % CPISI_FACE;
    }
}

/** @} */ /* end S.4b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c OPERATIONS — Verb/Reset/Validate [OPERATIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_operations S.4c OPERATIONS — Verb/Reset/Validate
 * @ingroup cpisi_error_prototypes
 * @brief    Maps error codes to log levels based on layer.
 *
 * PURPOSE: Error code operations (severity mapping).
 *          Converts error layer to appropriate log level.
 *
 * Builds FROM: S.4b ACCESSORS
 * Builds TO:   S.5 FILE-LEVEL
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief   Map CpisiError to LogLevel for logging integration
 * @param   err  CpisiError code
 * @return  Appropriate LogLevel based on error layer
 * @note    CPISI_OK returns LOG_INFO; errors map by layer (9 layers → 9 levels)
 */
static inline LogLevel cpisi_error_severity(CpisiError err) {
    if (err == CPISI_OK) return LOG_INFO;

    // Error layer (0-9) maps directly to LogLevel severity
    // KERNEL=most severe (CRISIS), APP=least severe (INSIGHT area → ERROR)
    uint8_t layer = CPISI_ERROR_LAYER(err);
    switch (layer) {
        case CPISI_LAYER_KERNEL:    return LOG_CRISIS;   // Foundational failure
        case CPISI_LAYER_SYSTEM:    return LOG_FATAL;    // OS-level failure
        case CPISI_LAYER_RUNTIME:   return LOG_ERROR;    // Runtime error
        case CPISI_LAYER_FRAMEWORK: return LOG_ERROR;    // Library error
        case CPISI_LAYER_HEALTH:    return LOG_WARN;     // Measurement issue
        case CPISI_LAYER_NETWORK:   return LOG_WARN;     // Connection issue
        case CPISI_LAYER_STORAGE:   return LOG_WARN;     // Storage issue
        case CPISI_LAYER_ENGINE:    return LOG_ERROR;    // Engine error
        case CPISI_LAYER_APP:       return LOG_ERROR;    // App error
        case CPISI_LAYER_COUNT:     return LOG_DEBUG;    // Metadata (layer 9)
        default:                    return LOG_ERROR;
    }
}

/** @} */ /* end S.4c OPERATIONS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File Scope Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_file S.5 FILE-LEVEL — File Scope Declarations
 * @brief    Reserved — error domain has no file-level state.
 *
 * PURPOSE: File-level variables and state.
 *          Not applicable — error.h provides codes and formulas only.
 *
 * CONTAINS:
 *   - S.5a VARIABLES      — [Reserved: no error static variables]
 *   - S.5b FUNC_TYPES     — [Reserved: no error function types]
 *   - S.5c INFRASTRUCTURE — [Reserved: no error infrastructure]
 *
 * Builds FROM: S.4 PROTOTYPES
 * Builds TO:   S.6 OMISSION
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5a VARIABLES — Extern/Static Variables [VARIABLES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_vars S.5a VARIABLES — Extern/Static Variables
 * @ingroup cpisi_error_file
 * @brief    Reserved for file-level variable declarations.
 *
 * PURPOSE: Shared state (extern) and file state (static).
 *          Currently reserved — error.h has no mutable state.
 *
 * Builds FROM: S.5 FILE-LEVEL (section context)
 * Builds TO:   S.5b FUNC_TYPES
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: No mutable state in error domain] */

/** @} */ /* end S.5a VARIABLES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5b FUNC_TYPES — Callback Typedefs [FUNC_TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_functypes S.5b FUNC_TYPES — Callback Typedefs
 * @ingroup cpisi_error_file
 * @brief    Reserved for function pointer typedefs.
 *
 * PURPOSE: Callback signatures for hooks and handlers.
 *          Currently reserved — error.h has no callbacks.
 *
 * Builds FROM: S.5a VARIABLES
 * Builds TO:   S.5c INFRASTRUCTURE
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: No callback types in error domain] */

/** @} */ /* end S.5b FUNC_TYPES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5c INFRASTRUCTURE — Logger/Inspector [INFRASTRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_infra S.5c INFRASTRUCTURE — Logger/Inspector
 * @ingroup cpisi_error_file
 * @brief    Reserved for health tracking infrastructure.
 *
 * PURPOSE: Logger and Inspector for health tracking.
 *          Currently reserved — infrastructure lives in source files.
 *
 * Builds FROM: S.5b FUNC_TYPES
 * Builds TO:   S.6 OMISSION
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Infrastructure lives in source files, not headers] */

/** @} */ /* end S.5c INFRASTRUCTURE */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_omit S.6 OMISSION — Reserved Section Guide
 * @brief    Documents what sections are reserved and why.
 *
 * PURPOSE: Guide for reserved sections in error.h.
 *
 * OMITTED (by design):
 *   - S.5 FILE-LEVEL — No static state needed for error codes
 *   - Runtime error handling — Application responsibility
 *   - Error recovery logic — Domain-specific, not in types header
 *
 * WHY RESERVED:
 *   S.5a VARIABLES      — Error codes are values, not mutable state
 *   S.5b FUNC_TYPES     — No callback types needed
 *   S.5c INFRASTRUCTURE — Infrastructure lives in source files
 *
 * Builds FROM: S.5 FILE-LEVEL
 * Builds TO:   END SETUP
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* Error domain provides codes and formulas only.
 * Error HANDLING is application-level, not type-level. */

/** @} */ /* end S.6 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Runtime functions and helpers for error handling.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART  — Structure overview
///   - B.2 HELPERS    — Error handling helper functions
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_orgchart B.1 ORG CHART — Structure Overview
 * @ingroup cpisi_types_config_error
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
 * @defgroup cpisi_error_orgchart_structure B.1a STRUCTURE — Interface Overview
 * @ingroup cpisi_error_orgchart
 * @brief    Types, functions, and externs declared in this header.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      34 enums (ErrorCode, ErrorClass, ErrorSeverity, etc.) */
/* Functions:  [Reserved] */
/* Externs:    [Reserved] */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_orgchart_flow B.1b FLOW — Implementation Location
 * @ingroup cpisi_error_orgchart
 * @brief    Where implementations live.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: error.c (when runtime functions added) */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_error_orgchart_counts B.1c COUNTS — Summary Statistics
 * @ingroup cpisi_error_orgchart
 * @brief    Interface element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      34 total (enums in SETUP) */
/* Functions:  0 (reserved for runtime) */
/* Externs:    0 (reserved) */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Error Handling Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_helpers B.2 HELPERS — Error Handling Functions
 * @ingroup cpisi_types_config_error
 * @brief    Foundation functions for error handling operations.
 *
 * WHY: Separate helper functions from public API. Helpers are building
 *      blocks that higher-level error handling composes.
 *
 * SCOPE: Error lookup, formatting, classification utilities.
 *
 * CONTAINS:
 *   - B.2a LOOKUP     — Error info retrieval
 *   - B.2b UTILITIES  — Common error operations
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** B.2a LOOKUP — [Reserved] @defgroup cpisi_error_helpers_lookup @ingroup cpisi_error_helpers @{ @} */
/** B.2b UTILITIES — [Reserved] @defgroup cpisi_error_helpers_utilities @ingroup cpisi_error_helpers @{ @} */

/** @} */ /* end B.2 HELPERS */

/** B.3 ERRORS — [Reserved: uses types.h B.4] @defgroup cpisi_error_errors @ingroup cpisi_types_error @{ @} */

/** B.4 PUBLIC API — [Reserved: enum values accessed directly] @defgroup cpisi_error_api @ingroup cpisi_types_error @{ @} */

/** B.5 EXTENSIONS — [Reserved: leaf domain] @defgroup cpisi_error_extensions @ingroup cpisi_types_error @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — What BODY Reserves [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_body_omission B.6 OMISSION — Reserved BODY Sections
 * @brief    Documents what BODY sections are intentionally reserved.
 *
 * | Section | Status | Reason |
 * |---------|--------|--------|
 * | B.1 ORG CHART | Active | Structure overview |
 * | B.2 HELPERS | Reserved | Error handling functions (future) |
 * | B.3 ERRORS | Reserved | Uses foundation error recovery |
 * | B.4 PUBLIC API | Reserved | Enum values accessed directly |
 * | B.5 EXTENSIONS | Reserved | Error is leaf domain |
 *
 * WHY: Error domain provides compile-time constants (error codes).
 * Runtime error handling functions will be added when needed.
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
///   - X.5 EXTENSION     — How to add error codes
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
 * @defgroup cpisi_error_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build and syntax verification.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -fsyntax-only -Wall -Wextra error.h       # syntax check
 * cppcheck --enable=all error.h                 # static analysis
 * @endcode
 *
 * @par X.1b SELFTEST — Minimal Isolation Test
 * @code{.c}
 * #include "kernel/cpisi/types/config/error.h"
 * int main(void) { return 0; }
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_execution X.2 EXECUTION — Usage Patterns
 * @brief    Include pattern and usage examples.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.2a INCLUDE — Include Pattern
 * @code{.c}
 * #include "kernel/cpisi/types/config/error.h"
 * @endcode
 *
 * @par X.2b USAGE — Error Code Usage
 * @code{.c}
 * ErrorCode err = ERR_KERNEL_INIT_FAILED;
 * int layer = err / 27;           // 0 = KERNEL
 * int offset = err % 27;          // offset within layer
 * ErrorClass cls = offset / 9;    // DETECT, ASSESS, or RESPOND
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Ownership notes — compile-time constants, no allocation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * All error codes are compile-time enum values.
 * No heap allocation, no cleanup required.
 * Error handling is application-level responsibility.
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
 * @defgroup cpisi_error_policy X.4 POLICY — Modification Guidance
 * @brief    Guide maintainers — modification safety levels.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Safe to Modify
 *   - Add new error codes within layer ranges
 *   - Add error message descriptions
 *
 * @par Modify with Care
 *   - Error code values — affects existing error handling
 *   - Layer boundaries — affects code extraction logic
 *
 * @par NEVER Modify
 *   - BRIDGE equation (243+13=256)
 *   - Layer code ranges (layer × 27)
 *   - Metadata code range (243-255)
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Add Error Codes [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_extension X.5 EXTENSION — How to Add Error Codes
 * @brief    How to add new error codes.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Adding Error Codes
 *   - Identify the layer (0-8)
 *   - Calculate base: layer × 27
 *   - Choose category: DETECT (0-8), ASSESS (9-17), RESPOND (18-26)
 *   - Add to appropriate enum section
 *   - Include full docstring (@brief, @par Layer, @par Category)
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Problems [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par "Unknown error code"
 *   Check if code is in valid range (0-255).
 *   Metadata codes are 243-255.
 *
 * @par "Wrong layer extraction"
 *   Verify: layer = code / 27 (integer division)
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick lookup — layer code ranges.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Layer Ranges
 *   L0 KERNEL: 0-26, L1 SYSTEM: 27-53, L2 RUNTIME: 54-80,
 *   L3 FRAMEWORK: 81-107, L4 HEALTH: 108-134 (CENTER),
 *   L5 NETWORK: 135-161, L6 STORAGE: 162-188,
 *   L7 ENGINE: 189-215, L8 APP: 216-242
 *
 * @par BRIDGE Equation
 *   243 (PENTERACT) + 13 (BRIDGE) = 256 (fits uint8_t)
 *
 * @par Categories per Layer (27 codes each)
 *   DETECT: 0-8, ASSESS: 9-17, RESPOND: 18-26
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * Scripture: Proverbs 27:12
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * Error codes are THE diagnostic vocabulary for CPI-SI.
 * 256 codes — ternary structure (243) bridged to binary (256).
 * Every error maps to a layer, category, and severity.
 *
 * "A prudent man foreseeth the evil, and hideth himself: but the
 *  simple pass on, and are punished." — Proverbs 27:12
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Reserved Sections [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_error_closing_omission X.9 OMISSION — Reserved Sections
 * @brief    CLOSING sections guidance.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * All CLOSING sections present. Error domain provides enum constants —
 * error handling functions are application responsibility.
 *
 * @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_TYPES_ERROR_H */
