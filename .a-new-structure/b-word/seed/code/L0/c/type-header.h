#if 0  // TEMPLATE: Remove this block when instantiating
/// @file type-header.h
/// @omni template --header --type
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
/// @omni:req  key        = B-word-seed-code-c-type-header
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = template
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = type-template
/// @omni:inh  role       = seed
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = seed
/// @omni:ins  layer      = code/c
/// @omni:ins  includes   = types.h
/// @omni:ins  provides   = [C_TYPE_HEADER_TEMPLATE]
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
/// @omni:req  title      = C Type Header File Template
/// @omni:req  brief      = 4-block structure template for CPI-SI type domain files
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// END PRAGMA [END]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// USAGE: cp type-header.h dest.h → update pragma → fill sections
#endif

/// ═══════════════════════════════════════════════════════════════════════════════
/// SPECIAL BLOCK [SPECIAL]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Pre-include configuration — MUST appear before any system includes.
///
/// WHY SPECIAL EXISTS:
///   Feature macros and compile-time settings must be defined BEFORE #include.
///   This is not PRAGMA (metadata), not METADATA (docs), not SETUP (includes).
///
/// CONTAINS:
///   - SP.1 CONFIG  — Compile-time settings (POSIX, domain-specific macros)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * WHY: Settings that affect what headers provide must precede #include.
 *
 * CONTAINS:
 *   - SP.1a POSIX  — Platform feature test (universal)
 *   - Add domain-specific config here if needed
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1a POSIX — Feature Test [POSIX]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_special_posix SP.1a POSIX — Feature Test
 * @ingroup seed_type_special_config
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

#ifndef SEED_TYPE_H
#define SEED_TYPE_H

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Identity and context for this component.
///
/// STRUCTURE: M.1-M.10 sections, grouped for readability:
///   - M.1 IDENTITY:    Core identity (file, brief, key, title, type, component, role)
///   - M.2-M.6:         State, Attribution, Location, Derivation, Classification
///   - M.7-M.10:        Intent, Grounding, Dependencies, Roadmap
///
/// DOMAIN FILES: Use condensed M.2-M.6 and M.7-M.10 groupings.
/// FOUNDATION FILES: May expand individual M.X sections as needed.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      type-header.h
 * @brief     Seed Type — Type definition template for CPI-SI headers.
 *
 * @defgroup seed_type_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       B-SEED-TYPE-HEADER
 * Title:     Seed Type Header Template
 * Type:      Header (Domain)
 * Component: Ladder — type definitions for domain
 * Role:      Define domain-specific types and enumerations
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   a-XX.XX
 * @date      YYYY-MM-DD
 *
 * Status:    Active
 * Created:   YYYY-MM-DD
 * Updated:   YYYY-MM-DD
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authors and Copyright [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_attribution M.3 ATTRIBUTION — Authors and Copyright
 * @{
 *
 * @author    Seed (Architect)
 * @author    Seed (Implementation)
 * @copyright © YYYY CreativeWorkzStudio LLC. All rights reserved.
 *
 * @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Path [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_location M.4 LOCATION — File Path
 * @{
 *
 * Path: bereshit/word/seed/code/c/type-header.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: bereshit/word/seed/code/c/header.h (foundation) | None — canonical source
 * Extracted:    2025-12-05 type header specialization
 *
 * Builds FROM:  types.h foundation
 * Builds TO:    Domain implementations
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, domain, template
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Type-safe domain definitions for CPI-SI components
 * Core Design: Ternary structure with lookup tables
 * Philosophy:  Boundaries before content (Genesis 1:1)
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Genesis 1:1 — "In the beginning God created"
 * Principle:  Boundaries before content (Genesis 1:1)
 * Anchor:     Proverbs 24:27
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h
 *
 * What Uses This:
 *   - Domain source implementations
 *
 * @see types.h for additional context
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-XX.XX (YYYY-MM-DD) — Initial type header template
 *   a-XX.XX (YYYY-MM-DD) — Docstring format standardization
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ Complete 4-block structure with Doxygen docstrings
 *
 * Planned:
 *   ⏳ Domain-specific extensions
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
///   - S.1 INCLUDES  — Header dependencies (types.h foundation)
///   - S.2 DEFINES   — Domain definitions (Bounds, Macros, Verify, Lookup, Types)
///
/// TYPE DOMAIN STRUCTURE:
///   S.2 contains the complete domain definition — bounds define constants,
///   macros operate on bounds, verify validates at compile-time, lookup
///   provides runtime tables, types wrap in enumerations.
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
 * @defgroup seed_type_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * PURPOSE: Establish all header dependencies in consistent order.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers (MODE-conditional, 9 layers)
 *   - S.1b INTERNAL  — Project headers (CPI-SI dependencies)
 *   - S.1c EXTERNAL  — Third-party headers (with justification)
 *   - S.1d CPP       — C++ linkage wrapper
 *
 * Section order: Standard → Internal → External → CPP linkage
 * Flow: system foundation → our code → third-party → C++ compatibility
 *
 * Builds FROM: SETUP block (configuration context)
 * Builds TO:   S.2 DEFINES (constants that may use included types)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_standard S.1a STANDARD — System Headers
 * @ingroup seed_type_includes
 * @brief    C standard library headers required by this module.
 *
 * FOR FOUNDATION FILES: Include system headers directly with MODE-conditional layers.
 * FOR DOMAIN FILES: Inherit from types.h S.1a — no direct system includes.
 *
 * COMMON HEADERS:
 *   <stdint.h>   — Fixed-width integers: int8_t, int16_t, int32_t, int64_t, uint8_t, etc.
 *   <stddef.h>   — Standard definitions: size_t, NULL, offsetof, ptrdiff_t
 *   <stdbool.h>  — Boolean type: bool, true, false
 *   <limits.h>   — Implementation limits: INT_MAX, UINT_MAX, CHAR_BIT
 *   <string.h>   — String operations: memcpy, memset, strlen, strcmp, strcpy
 *   <stdio.h>    — Input/output: FILE, printf, fprintf, snprintf, fopen
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Foundation file: Include system headers directly */
#include <stdint.h>   /**< Fixed-width integers: int8_t, int16_t, int32_t, int64_t, uint8_t, etc. */
#include <stddef.h>   /**< Standard definitions: size_t, NULL, offsetof, ptrdiff_t */
#include <stdbool.h>  /**< Boolean type: bool, true, false */

/* Domain file: Inherit from types.h S.1a */
/* [Inherited from types.h S.1a — no direct system includes] */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_internal S.1b INTERNAL — Project Headers
 * @ingroup seed_type_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * FOUNDATION FILES: May include specific component headers.
 * DOMAIN FILES: Include types.h for foundation types + macros.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Domain file: Include foundation */
/* #include "types.h" */  /**< Foundation: S.1 system headers + S.2 universal defines */

/* Template self-definitions (replace with types.h include when instantiated) */
typedef int seed_type;                              /**< Base type for seed domain */
#define CPISI_ONE           1                       /**< Unity constant */
#define CPISI_VOID          0                       /**< Void/zero constant */
#define CPISI_UNITY         1                       /**< Unity for arithmetic */
#define CPISI_CENTER_OF(x)  (((x) - 1) / 2)         /**< Center index of count */
#define CPISI_DIFF(a, b)    ((a) - (b))             /**< Difference operation */

/* Or specific internal headers */
// #include "type-header.h"  /**< additional context within project */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup seed_type_includes
 * @brief    External library headers with justification.
 *
 * JUSTIFICATION REQUIRED: Each external header must document why standard library
 * is insufficient and what capability it provides.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [None — standard library only] */
// #include <lib.h>  /**< justification — what stdlib lacks */

/** @} */ /* end S.1c EXTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_cpp S.1d CPP — C++ Linkage
 * @ingroup seed_type_includes
 * @brief    C++ compatibility wrapper.
 *
 * FOUNDATION FILES: Declare extern "C" wrapper (opens here, closes in CLOSING).
 * DOMAIN FILES: Inherit from types.h S.1d — extern "C" already active.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Foundation file: Open extern "C" wrapper */
#ifdef __cplusplus
extern "C" {
#endif

/* Domain file: Inherit from types.h S.1d */
/* [Inherited from types.h S.1d — extern "C" already active] */

/** @} */ /* end S.1d CPP */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Seed Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_seed_domain S.2 DEFINES — Seed Domain
 * @brief    Seed domain type definitions and constants.
 *
 * PURPOSE: Define domain-specific constants, operations, verification, and types.
 *
 * Builds FROM: S.1 INCLUDES (types.h foundation + system headers)
 * Builds TO:   S.3 TYPES (domain constants used in struct definitions)
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — SPECIAL CASE Domain-specific operations (if needed)
 *   - S.2a  BOUNDS  — Domain counts, ranges, and limits
 *   - S.2b  MACROS  — Operations on domain bounds
 *   - S.2c  VERIFY  — Compile-time assertions (tripwires)
 *   - S.2d  LOOKUP  — Runtime lookup tables
 *   - S.2e  TYPES   — Type-safe enumerations
 *
 * FLOW: Prelude provides operations → Bounds define → Macros operate → Verify validates → Lookup provides → Types wrap
 *
 * WHY PRELUDE IS SPECIAL CASE:
 *   Most domains use foundation operations directly (CPISI_SUM, CPISI_MAX_OF, etc.).
 *   Include PRELUDE only when domain needs operations BEFORE bounds can be defined.
 *   Example: HEALTH_SCALE(base, layer) used throughout health bounds.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2.0 PRELUDE — Seed Operations [PRELUDE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2_0_SEED S.2.0 SEED PRELUDE — Domain Operations
 * @ingroup cpisi_seed_domain
 * @brief    SPECIAL CASE Domain-specific operations needed before BOUNDS.
 *
 * PURPOSE: Provide operations that BOUNDS needs for derivation expressions.
 *          Foundation types.h PRELUDE provides universal operations.
 *          Domain PRELUDE provides domain-specific operations.
 *
 * WHEN TO INCLUDE:
 *   ✓ Domain has formulas used repeatedly in BOUNDS
 *   ✓ Domain needs intermediate calculations before defining values
 *   ✓ Domain has SCALE/FACTOR patterns specific to this domain
 *
 * WHEN TO RESERVE:
 *   ✓ Domain uses foundation operations directly (most domains)
 *   ✓ No complex formulas — just CPISI_SUM, CPISI_MAX_OF, etc.
 *
 * PURE BASIC PRELUDE (what belongs here):
 *   - Domain SCALE/FACTOR macros: SEED_SCALE(base, factor)
 *   - Domain ALIAS operations: clearer names for foundation ops in this context
 *   - Domain HELPER macros: operations BOUNDS will use repeatedly
 *
 * WHAT DOESN'T BELONG:
 *   - Values/constants (those go in S.2a BOUNDS)
 *   - Types (those go in S.2e TYPES)
 *   - Re-defining foundation operations (inherit from types.h)
 *
 * @{ */

/* [SPECIAL CASE: Include domain-specific operations if needed] */
/* [Most domains: Reserve this section — use foundation operations directly] */

/* [Reserved: Domain uses foundation PRELUDE operations] */

/* Example (uncomment if domain needs operations):
 *
 * #define SEED_SCALE(base, factor)  CPISI_PRODUCT((base), (factor))
 * #define SEED_OFFSET(value, dir)   CPISI_SUM((value), (dir))
 *
 */

/** @} */ /* end S.2.0 PRELUDE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BOUNDS — Seed Bounds [BOUNDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2a_SEED S.2a SEED BOUNDS — Domain Bounds
 * @ingroup cpisi_seed_domain
 * @brief    Domain counts, ranges, and limits.
 *
 * STRUCTURE: Flat 4-section organization
 *   - Counts:  Array sizes and capacities (domain COUNT values)
 *   - Levels:  Domain-specific literals (named positions, indices)
 *   - Bounds:  MIN/CENTER/MAX derived from counts (full docstrings)
 *   - Additional: Domain-specific categories as needed
 *
 * PATTERNS:
 *   - @name groups (not nested @defgroup)
 *   - Inline docstrings: value: description MIN/CENTER/MAX
 *   - Full docstrings for MIN/CENTER/MAX with @par Derivation
 *   - Config-driven: *_OF(COUNT) pattern for derived bounds
 *
 * Builds FROM: S.2.0 PRELUDE (if present) + Foundation types.h PRELUDE
 * Builds TO:   S.2b MACROS (provides values for operations)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Counts — Seed domain array sizes
 *  @brief SEED_COUNT (3 items), UNIT_COUNT (2 items).
 *  @{ */
#define CPISI_SEED_COUNT        CPISI_ONE       /**< N: domain value                     */
/** @} */ /* end Counts */

/** @name Seed Levels — Named positions
 *  @brief SEED_MIN/CENTER/MAX literals for array indexing.
 *  @{ */
#define CPISI_SEED_LEVEL_MIN    0                   /**< 0: instance MIN                      */
#define CPISI_SEED_LEVEL_CENTER 1                   /**< 1: instance CENTER                   */
#define CPISI_SEED_LEVEL_MAX    2                   /**< 2: instance MAX                      */
/** @} */ /* end Seed Levels */

/** @name Seed Bounds — MIN/CENTER/MAX from count
 *  @brief Config-driven: *_OF(SEED_COUNT) pattern.
 *  @{ */
/**
 * @brief 0: minimum seed — domain value.
 * @par Derivation
 *      SEED_MIN = VOID = MIN_OF(SEED_COUNT) = 0
 */
#define CPISI_SEED_MIN          CPISI_VOID
/**
 * @brief N: center seed — domain value.
 * @par Derivation
 *      SEED_CENTER = CENTER_OF(SEED_COUNT) = N
 */
#define CPISI_SEED_CENTER       CPISI_CENTER_OF(CPISI_SEED_COUNT)
/**
 * @brief M: maximum seed — domain value.
 * @par Derivation
 *      SEED_MAX = DIFF(SEED_COUNT, UNITY) = MAX_OF(SEED_COUNT) = M
 */
#define CPISI_SEED_MAX          CPISI_DIFF(CPISI_SEED_COUNT, CPISI_UNITY)
/** @} */ /* end Seed Bounds */

/** @} */ /* end S.2a BOUNDS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b MACROS — Seed Operations [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2b_SEED S.2b SEED MACROS — Operations
 * @ingroup cpisi_seed_domain
 * @brief    Operations on seed bounds.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Domain-specific macros here] */

/** @} */ /* end S.2b MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c VERIFY — Compile-time Assertions [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2c_SEED S.2c SEED VERIFY — Compile-time assertions
 * @ingroup cpisi_seed_domain
 * @brief    Static assertions validating seed architecture.
 *
 * PURPOSE: Compile-time tripwires that catch configuration errors before runtime.
 *          If any assertion fails, compilation stops with a descriptive message.
 *
 * TRIPWIRE PHILOSOPHY (three types):
 *
 *   VALUE TRIPWIRES — Verify literals match expected values
 *     "This constant IS this value"
 *     Example: CPISI_STATIC_ASSERT(CPISI_TERNARY == 3, "ternary must be 3")
 *
 *   RELATIONSHIP TRIPWIRES — Verify derivations are correct
 *     "A equals f(B)" - derived value matches formula
 *     Example: CPISI_STATIC_ASSERT(CPISI_CUBE == CPISI_THIRD_OF(CPISI_TERNARY),
 *                                   "CUBE must equal TERNARY³")
 *
 *   EQUIVALENCE TRIPWIRES — Connect related constants
 *     "These two things that should be equal ARE equal"
 *     Example: CPISI_STATIC_ASSERT(CPISI_LAYER_COUNT == CPISI_FACE,
 *                                   "layers must equal face count")
 *
 * STRUCTURE:
 *   S.2c.1 VALUES        — Literal verification (anchors)
 *   S.2c.2 RELATIONSHIPS — Derivation verification (formulas)
 *   S.2c.3 EQUIVALENCES  — Cross-reference verification (connections)
 *
 * Builds FROM: S.2a BOUNDS (values to verify), S.2b MACROS (derivation formulas)
 * Builds TO:   S.2d LOOKUP (verified values ready for runtime tables)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2c.1 VALUE TRIPWIRES — Literal verification
 *  @brief Verify constants equal expected literal values.
 *  @{ */

/* Example VALUE tripwires (uncomment and adapt):
 *
 * CPISI_STATIC_ASSERT(CPISI_SEED_COUNT == 3,
 *     "SEED_COUNT must be 3");
 *
 * CPISI_STATIC_ASSERT(CPISI_SEED_MIN == 0,
 *     "SEED_MIN must be 0 (VOID)");
 */

/* [Reserved: No domain-specific value tripwires] */

/** @} */ /* end S.2c.1 VALUE TRIPWIRES */

/** @name S.2c.2 RELATIONSHIP TRIPWIRES — Derivation verification
 *  @brief Verify derived values match their formulas.
 *  @{ */

/* Example RELATIONSHIP tripwires (uncomment and adapt):
 *
 * CPISI_STATIC_ASSERT(CPISI_SEED_MAX == CPISI_DIFF(CPISI_SEED_COUNT, CPISI_UNITY),
 *     "SEED_MAX must equal COUNT - 1");
 *
 * CPISI_STATIC_ASSERT(CPISI_SEED_CENTER == CPISI_CENTER_OF(CPISI_SEED_COUNT),
 *     "SEED_CENTER must equal CENTER_OF(COUNT)");
 */

/* [Reserved: No domain-specific relationship tripwires] */

/** @} */ /* end S.2c.2 RELATIONSHIP TRIPWIRES */

/** @name S.2c.3 EQUIVALENCE TRIPWIRES — Cross-reference verification
 *  @brief Verify related constants across domains are synchronized.
 *  @{ */

/* Example EQUIVALENCE tripwires (uncomment and adapt):
 *
 * CPISI_STATIC_ASSERT(CPISI_SEED_COUNT == CPISI_UNIT_COUNT,
 *     "SEED count must match UNIT count");
 */

/* [Reserved: No domain-specific equivalence tripwires] */

/** @} */ /* end S.2c.3 EQUIVALENCE TRIPWIRES */

/** @} */ /* end S.2c VERIFY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d LOOKUP — Seed Runtime Tables [LOOKUP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2d_SEED_LOOKUP S.2d SEED LOOKUP — Runtime Tables
 * @ingroup cpisi_seed_domain
 * @brief    Seed runtime lookup tables.
 *
 * Builds FROM: S.2a BOUNDS (values), S.2b MACROS (operations), S.2c VERIFY (assurance)
 * Builds TO:   S.2e TYPES (consumes tables via accessor macros)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{
 */

/**
 * @defgroup S2d_X_SEED S.2d.X SEED — Seed runtime tables
 * @ingroup S2d_SEED_LOOKUP
 * @brief Runtime tables for seed subsystem.
 *
 * STRUCTURE:
 *   S.2d.Xa TABLES       — All runtime lookup arrays
 *   S.2d.Xb ACCESSORS    — Macro wrappers for array access
 *   S.2d.Xc CONVERSION   — Reserved Format conversion utilities
 *
 * @{
 */

/**
 * @defgroup S2d_Xa_TABLES S.2d.Xa TABLES — Seed lookup arrays
 * @ingroup S2d_X_SEED
 * @brief All runtime lookup arrays for seed.
 * @{
 */

/* [Domain-specific lookup tables here] */

/** @} */ /* end S.2d.Xa TABLES */

/**
 * @defgroup S2d_Xb_ACCESSORS S.2d.Xb ACCESSORS — Seed accessor macros
 * @ingroup S2d_X_SEED
 * @brief Macro wrappers for seed array access.
 * @{
 */

/* [Domain-specific accessor macros here] */

/** @} */ /* end S.2d.Xb ACCESSORS */

/**
 * @defgroup S2d_Xc_CONVERSION S.2d.Xc CONVERSION Reserved
 * @ingroup S2d_X_SEED
 * @brief Reserved Format conversion utilities for seed.
 * @{
 */
/** @} */ /* end S.2d.Xc CONVERSION */

/** @} */ /* end S.2d.X DOMAIN */
/** @} */ /* end S.2d LOOKUP */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e TYPES — Seed Type Definitions [TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2e_SEED S.2e SEED TYPES — Enumerations
 * @ingroup cpisi_seed_domain
 * @brief    Type-safe enumerations for seed.
 *
 * Builds FROM: S.2d LOOKUP (tables for runtime access)
 * Builds TO:   S.3 TYPES (domain enums available for struct definitions)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Domain-specific enumerations here] */

/** @} */ /* end S.2e TYPES */

/** @} */ /* end S.2 SEED DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_types S.3 TYPES — Type Declarations
 * @ingroup seed_type
 * @brief    Data structures and type definitions.
 *
 * PURPOSE: Enumerations, structs, opaque types, error codes.
 *
 * CONVENTIONS:
 *   Enumerations:
 *     SEED_NONE    — default/invalid (always 0)
 *     SEED_COUNT   — sentinel for array sizing
 *
 *   Structs:
 *     // SeedType represents a domain instance.
 *     // Fields: value: additional context
 *
 *   Opaque Types:
 *     typedef struct SeedType SeedType; — internals hidden in .c
 *
 *   Error Codes:
 *     SEED_OK          — success (always 0)
 *     SEED_ERR_SPECIFIC  — specific error conditions
 *
 * STRUCTURE:
 *   S.3a ENUMS   — Enumeration types
 *   S.3b STRUCTS — Structure definitions
 *   S.3c OPAQUE  — Forward-declared opaque types
 *   S.3d ERRORS  — Error code enumeration
 *
 * Builds FROM: S.2 DEFINES (constants used in types)
 * Builds TO:   S.4 PROTOTYPES (types used in function signatures)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — Enumeration Types [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_types_enums S.3a ENUMS — Enumeration Types
 * @ingroup seed_type_types
 * @brief    Enumeration definitions for seed.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SeedEnum enumeration values for seed domain.
 *
 * Provides type-safe enumeration for seed domain values.
 *
 * @note SEED_NONE is always 0 (default/invalid).
 * @note SEED_COUNT is sentinel for array sizing.
 */
typedef enum {
    SEED_NONE = 0,   /**< @brief Default/invalid value (always 0). */
    SEED_ONE,    /**< @brief Seed enum value. */
    SEED_COUNT       /**< @brief Sentinel for array sizing (do not use as value). */
} SeedEnum;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — Structure Definitions [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_types_structs S.3b STRUCTS — Structure Definitions
 * @ingroup seed_type_types
 * @brief    Structure definitions for seed.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SeedType represents a domain instance.
 *
 * Fields:
 *   - value: primary instance value
 */
typedef struct {
    seed_type value;
} SeedType;

/** @} */ /* end S.3b STRUCTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3c OPAQUE — Opaque Types [OPAQUE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_types_opaque S.3c OPAQUE — Opaque Types
 * @ingroup seed_type_types
 * @brief    Forward-declared opaque types (internals hidden in .c).
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SeedType opaque handle.
 *
 * Internal structure hidden in implementation file.
 * Use seed_type_create() to allocate and seed_type_destroy() to free.
 *
 * @see seed_type_create
 * @see seed_type_destroy
 */
/* [Use EITHER S.3b STRUCTS -OR- S.3c OPAQUE, not both] */
/* typedef struct SeedType SeedType; */

/** @} */ /* end S.3c OPAQUE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d ERRORS — Error Codes [ERRORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_types_errors S.3d ERRORS — Error Codes
 * @ingroup seed_type_types
 * @brief    Error code enumeration for seed operations.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SeedError error codes for seed operations.
 *
 * All functions returning int use these codes.
 * Success is always 0; errors are negative.
 *
 * @note Check with: if (result != SEED_OK) { handle_error(); }
 */
typedef enum {
    SEED_OK = 0,           /**< @brief Success (always 0). */
    SEED_ERR_INVALID,      /**< @brief Invalid argument or state. */
    SEED_ERR_NOMEM,        /**< @brief Memory allocation failed. */
    SEED_ERR_IO,           /**< @brief I/O operation failed. */
    SEED_ERR_NOT_FOUND,    /**< @brief Requested item not found. */
    SEED_ERR_EXISTS,       /**< @brief Item already exists. */
} SeedError;

/** @} */ /* end S.3d ERRORS */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_prototypes S.4 PROTOTYPES — Function Declarations
 * @ingroup seed_type
 * @brief    Function declarations — public API (headers), static (sources).
 *
 * PURPOSE: Function signatures for external use.
 *
 * CONVENTIONS:
 *   Lifecycle:
 *     seed_type_create    — allocate + init, caller must destroy
 *     seed_type_init      — init existing (stack-allocated)
 *     seed_type_destroy   — free resources (NULL-safe)
 *     seed_type_clone     — deep copy
 *
 *   Accessors:
 *     seed_type_get_value — retrieve field value
 *     seed_type_set_value — update field value
 *     seed_type_is_valid  — boolean queries
 *
 *   Operations:
 *     seed_type_process    — action, returns 0 on success
 *     seed_type_reset     — reset to initial state
 *     seed_type_validate  — check invariants
 *
 * STRUCTURE:
 *   S.4a LIFECYCLE  — Creation, destruction, copying
 *   S.4b ACCESSORS  — Getters, setters, queries
 *   S.4c OPERATIONS — Actions and transformations
 *
 * Builds FROM: S.3 TYPES (types used in signatures)
 * Builds TO:   S.5 FILE-LEVEL (prototypes inform file-level organization)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Creation and Destruction [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_prototypes_lifecycle S.4a LIFECYCLE — Creation and Destruction
 * @ingroup seed_type_prototypes
 * @brief    Functions for creating, initializing, and destroying instances.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Create a new SeedType instance.
 *
 * Allocates and initializes a new SeedType.
 * Caller is responsible for calling seed_type_destroy() when done.
 *
 * @return Pointer to new SeedType, or NULL on allocation failure.
 *
 * @see seed_type_destroy
 */
static inline SeedType* seed_type_create(void) {
    return NULL;  /**< Stub: implement allocation in source file */
}

/**
 * @brief Initialize an existing SeedType instance.
 *
 * Initializes a stack-allocated or pre-allocated SeedType.
 * Does not allocate memory for the structure itself.
 *
 * @param[out] self  Pointer to SeedType to initialize. Must not be NULL.
 *
 * @return SEED_OK on success, error code on failure.
 */
static inline int seed_type_init(SeedType* self) {
    if (!self) return SEED_ERR_INVALID;
    self->value = 0;
    return SEED_OK;
}

/**
 * @brief Destroy a SeedType instance.
 *
 * Frees all resources associated with the SeedType.
 * Safe to call with NULL (no-op).
 *
 * @param[in,out] self  Pointer to SeedType to destroy, or NULL.
 *
 * @post self is invalid after this call.
 */
static inline void seed_type_destroy(SeedType* self) {
    (void)self;  /**< Stub: implement cleanup in source file */
}

/**
 * @brief Create a deep copy of a SeedType.
 *
 * Allocates a new SeedType with all fields copied from src.
 * Caller is responsible for calling seed_type_destroy() on the copy.
 *
 * @param[in] src  Pointer to SeedType to copy. Must not be NULL.
 *
 * @return Pointer to new SeedType copy, or NULL on failure.
 *
 * @see seed_type_destroy
 */
static inline SeedType* seed_type_clone(const SeedType* src) {
    (void)src;
    return NULL;  /**< Stub: implement cloning in source file */
}

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ACCESSORS — Getters and Setters [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_prototypes_accessors S.4b ACCESSORS — Getters and Setters
 * @ingroup seed_type_prototypes
 * @brief    Functions for accessing and modifying instance state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Get the value value from a SeedType.
 *
 * @param[in] self  Pointer to SeedType. Must not be NULL.
 *
 * @return Current value value.
 */
static inline int seed_type_get_value(const SeedType* self) {
    return self ? self->value : 0;
}

/**
 * @brief Set the value value on a SeedType.
 *
 * @param[in,out] self   Pointer to SeedType. Must not be NULL.
 * @param[in]     value  New value value to set.
 */
static inline void seed_type_set_value(SeedType* self, int val) {
    if (self) self->value = val;
}

/**
 * @brief Check if SeedType is in valid state.
 *
 * @param[in] self  Pointer to SeedType. Must not be NULL.
 *
 * @return true if in valid state, false otherwise.
 */
static inline bool seed_type_is_valid(const SeedType* self) {
    return self != NULL;
}

/** @} */ /* end S.4b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c OPERATIONS — Actions and Transformations [OPERATIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_prototypes_operations S.4c OPERATIONS — Actions and Transformations
 * @ingroup seed_type_prototypes
 * @brief    Functions that perform actions or transform state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Process the SeedType.
 *
 * Processes the seed instance with given parameters.
 *
 * @param[in,out] self  Pointer to SeedType. Must not be NULL.
 * @param[in]     ...   Additional parameters as needed.
 *
 * @return SEED_OK on success, error code on failure.
 */
static inline int seed_type_process(SeedType* self) {
    return self ? SEED_OK : SEED_ERR_INVALID;
}

/**
 * @brief Reset a SeedType to its initial state.
 *
 * Restores all fields to their default values as if newly initialized.
 * Does not deallocate the structure itself.
 *
 * @param[in,out] self  Pointer to SeedType. Must not be NULL.
 *
 * @return SEED_OK on success, error code on failure.
 */
static inline int seed_type_reset(SeedType* self) {
    if (!self) return SEED_ERR_INVALID;
    self->value = 0;
    return SEED_OK;
}

/**
 * @brief Validate SeedType invariants.
 *
 * Checks that all internal state is consistent and valid.
 * Use for debugging and assertions.
 *
 * @param[in] self  Pointer to SeedType. Must not be NULL.
 *
 * @return SEED_OK if valid, error code indicating first violation.
 */
static inline int seed_type_validate(const SeedType* self) {
    return self ? SEED_OK : SEED_ERR_INVALID;
}

/** @} */ /* end S.4c OPERATIONS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File-Level Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_file_level S.5 FILE-LEVEL — File-Level Declarations
 * @ingroup seed_type
 * @brief    File-level state — extern (headers), static (sources).
 *
 * PURPOSE: Global variables, function pointer types, infrastructure.
 *
 * CONVENTIONS:
 *   Variables:
 *     extern seed_type g_instance;     — shared state (headers)
 *     static seed_type instance;       — file state (sources)
 *
 *   Function Types:
 *     typedef int (*Seed_fn)(...); — callback signatures
 *
 *   Infrastructure:
 *     Logger, Inspector — Rails health tracking (sources only)
 *
 * STRUCTURE:
 *   S.5a VARIABLES      — Global/static variable declarations
 *   S.5b FUNC_TYPES     — Callback and function pointer typedefs
 *   S.5c INFRASTRUCTURE — Logger, inspector (sources only)
 *
 * Builds FROM: S.4 PROTOTYPES (prototypes inform file-level needs)
 * Builds TO:   S.6 OMISSION (file-level informs what can be reserved)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5a VARIABLES — Global/Static Variables [VARIABLES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_file_level_vars S.5a VARIABLES — Global/Static Variables
 * @ingroup seed_type_file_level
 * @brief    Variable declarations shared across compilation units.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief g_seed_instance global seed instance.
 *
 * Used for shared state across compilation units.
 *
 * @note Thread-safety: requires lock
 * @note Modified by: seed_type_init
 */
extern seed_type g_seed_instance;

/**
 * @brief g_seed_default default seed constant.
 *
 * Immutable after initialization.
 *
 * @note Immutable after initialization.
 */
extern const seed_type g_seed_default;

/** @} */ /* end S.5a VARIABLES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5b FUNC_TYPES — Function Pointer Types [FUNC_TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_file_level_func_types S.5b FUNC_TYPES — Function Pointer Types
 * @ingroup seed_type_file_level
 * @brief    Callback signatures and function pointer typedefs.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Callback for completion notification.
 *
 * Called when the operation completes.
 *
 * @param[in,out] ctx  User context pointer passed to registration.
 */
typedef void (*SeedCallback_fn)(void* ctx);

/**
 * @brief Callback for completion notification with result.
 *
 * Called when the operation completes.
 *
 * @param[in]     arg  Callback argument value.
 * @param[in,out] ctx  User context pointer passed to registration.
 *
 * @return 0 to continue, non-zero to stop processing.
 */
typedef int (*SeedHandler_fn)(seed_type arg, void* ctx);

/** @} */ /* end S.5b FUNC_TYPES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5c INFRASTRUCTURE — Health Tracking [INFRASTRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_file_level_infra S.5c INFRASTRUCTURE — Health Tracking
 * @ingroup seed_type_file_level
 * @brief    Logger and inspector infrastructure (sources only).
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Infrastructure lives in source files, not headers] */

/** @} */ /* end S.5c INFRASTRUCTURE */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Omission Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_omission S.6 OMISSION — Omission Guide
 * @ingroup seed_type
 * @brief    What can be reserved and why.
 *
 * PURPOSE: Document intentional omissions for this file.
 *
 * RULES:
 *   - All sections must be present
 *   - Empty sections use: [Reserved: reason]
 *   - Omit subsection content, never the subsection header itself
 *
 * Builds FROM: S.5 FILE-LEVEL (what's declared informs what's omitted)
 * Builds TO:   END SETUP (completes SETUP block)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: Document any intentional omissions here] */

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
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART    — Structure overview, flow, counts
///   - B.2 TYPES        — Domain type definitions (this is a type header)
///   - B.3 HELPERS      — Helper functions (validation, conversion, utilities)
///   - B.4 ERRORS       — Error handling patterns
///   - B.5 PUBLIC API   — Exported interface
///
/// Section order: Org Chart → Types → Helpers → Errors → Public API
/// Flow: map structure → define types → foundations → error handling → exported interface
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_orgchart B.1 ORG CHART — Structure Overview
 * @ingroup seed_type
 * @brief    Map structure — interface (headers), internals (sources).
 *
 * WHY: Understanding the file structure before diving into details.
 *      Org chart provides the mental model for navigation.
 *
 * SCOPE: File-level organization, implementation location, interface counts.
 *
 * CONTAINS:
 *   - B.1a Structure — Types, functions, externs count
 *   - B.1b Flow      — Implementation location and execution path
 *   - B.1c Counts    — Summary statistics
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Interface Overview [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_orgchart_structure B.1a STRUCTURE — Interface Overview
 * @ingroup seed_type_orgchart
 * @brief    Types, functions, and externs declared in this header.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      3 structs, 3 enums */
/* Functions:  3 lifecycle, 3 accessors, 3 operations */
/* Externs:    3 globals, 3 callbacks */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_orgchart_flow B.1b FLOW — Implementation Location
 * @ingroup seed_type_orgchart
 * @brief    Where implementations live.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: source.c */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_orgchart_counts B.1c COUNTS — Summary Statistics
 * @ingroup seed_type_orgchart
 * @brief    Interface element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      3 total */
/* Functions:  3 total */
/* Externs:    3 total */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 TYPES — Seed Type Definitions [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_types B.2 TYPES — Seed Type Definitions
 * @ingroup seed_type
 * @brief    Domain-specific type definitions — what this file OWNS.
 *
 * WHY: Each domain file owns its types. The root provides foundation,
 *      children provide specialization. This is domain ownership.
 *
 * SCOPE: Seed-specific types used throughout seed operations.
 *
 * CONTAINS:
 *   - B.2a SeedKind  — Type definition (3 values)
 *   - B.2b SeedState  — Type definition (3 values)
 *   - Add subsections as needed
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ SEED TYPE ARCHITECTURE                                                  │
 * ├─────────────────┬───────────────────────────────────────────────────────────┤
 * │ SeedKind         │ Domain enumeration (3 values)                            │
 * │ SeedState         │ Domain enumeration (3 values)                            │
 * └─────────────────┴───────────────────────────────────────────────────────────┘
 *
 * Builds FROM: S.2 DEFINES (domain constants)
 * Builds TO:   B.3 HELPERS (types used in helper functions)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a SEED_KIND — Domain value (3 values) SEED_KIND
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_types_type1 B.2a SEED_KIND — Domain value
 * @ingroup seed_type_types
 * @brief    Primary type enumeration for seed domain.
 *
 * WHY: Provides type-safe enumeration for domain values.
 *
 * SCOPE: Throughout seed domain operations.
 *
 * ┌─────────┬───────┬────────────────────────────────────────────────────────────┐
 * │ Value   │ Code  │ Description                                                │
 * ├─────────┼───────┼────────────────────────────────────────────────────────────┤
 * │ VALUE1 │   0   │ Domain value                                              │
 * │ VALUE2 │   1   │ Domain value                                              │
 * └─────────┴───────┴────────────────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SeedKind — seed domain enumeration type.
 *
 * Values are 0-indexed for direct array access.
 *
 * Values are 0-indexed for direct array access into SEED_KIND_INFO.
 */
typedef enum SeedKind {
    SEED_KIND_VALUE1 = 0,   /**< First domain value */
    SEED_KIND_VALUE2 = 1,   /**< Second domain value */
} SeedKind;

/** @name SeedKind Bounds
 *  @brief Range limits and count for SeedKind.
 *  @{ */
#define SEED_KIND_MIN     SEED_KIND_VALUE1  /**< Lower bound */
#define SEED_KIND_MAX     SEED_KIND_VALUE2  /**< Upper bound */
#define SEED_KIND_COUNT   (SEED_KIND_MAX + 1)  /**< Total values */
#define SEED_KIND_DEFAULT SEED_KIND_VALUE1  /**< Default value */
/** @} */ /* end SeedKind Bounds */

/**
 * @brief SeedKind information record — unified lookup.
 */
typedef struct SeedKindInfo {
    const char* name;   /**< Lowercase name */
    const char* emoji;  /**< Visual symbol */
} SeedKindInfo;

/**
 * @brief MASTER lookup table: SeedKind → all metadata.
 */
static const SeedKindInfo SEED_KIND_INFO[SEED_KIND_COUNT] = {
    [SEED_KIND_VALUE1] = { "value1", "🔵" },
    [SEED_KIND_VALUE2] = { "value2", "🟢" },
};

/**
 * @brief Safe lookup: SeedKind → SeedKindInfo pointer.
 * @param t SeedKind enum value.
 * @return Pointer to SeedKindInfo record. Never NULL.
 */
static inline const SeedKindInfo* seed_kind_info(SeedKind t) {
    if (t < 0 || t >= SEED_KIND_COUNT) t = SEED_KIND_DEFAULT;
    return &SEED_KIND_INFO[t];
}

/** @} */ /* end B.2a SEED_KIND */

/* Add B.2b, B.2c, etc. as needed following same pattern */

/** @} */ /* end B.2 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 HELPERS — Helper Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_helpers B.3 HELPERS — Helper Functions
 * @ingroup seed_type
 * @brief    Foundation functions — validation, conversion, utilities.
 *
 * WHY: Separate helper functions from public API. Helpers are the building
 *      blocks that public API functions compose.
 *
 * SCOPE: Internal use primarily. Declare in headers only if needed across
 *        multiple .c files in the same component.
 *
 * CONTAINS:
 *   - B.3a VALIDATION  — Input checking, bounds enforcement
 *   - B.3b CONVERSION  — Type transforms, format changes
 *   - B.3c UTILITIES   — Common operations
 *
 * Note: Most helpers are static in sources. Reserve sections as needed.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a VALIDATION — Input Checking [VALIDATION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_helpers_validation B.3a VALIDATION — Input Checking
 * @ingroup seed_type_helpers
 * @brief    Bounds checking, clamping, validity tests.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Validation helpers typically static in source file] */

/** @} */ /* end B.3a VALIDATION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b CONVERSION — Type Transforms [CONVERSION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_helpers_conversion B.3b CONVERSION — Type Transforms
 * @ingroup seed_type_helpers
 * @brief    Converting between types and formats.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Conversion helpers typically static in source file] */

/** @} */ /* end B.3b CONVERSION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3c UTILITIES — Common Operations [UTILITIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_helpers_utilities B.3c UTILITIES — Common Operations
 * @ingroup seed_type_helpers
 * @brief    Utility operations with potential side effects.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Utility helpers typically static in source file] */

/** @} */ /* end B.3c UTILITIES */

/** @} */ /* end B.3 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Error Handling [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_errors B.4 ERRORS — Error Handling
 * @ingroup seed_type
 * @brief    Error handling — query declarations (headers).
 *
 * WHY: Consistent error handling enables reliable operation and debugging.
 *
 * SCOPE: Error queries, recovery helpers.
 *
 * DESIGN PRINCIPLE: Non-blocking
 * Example: Non-blocking — seed_type failures never interrupt core processing.
 *
 * RECOVERY STRATEGY:
 *   - Validation errors: Graceful degradation (fallback behavior)
 *   - Memory errors: Fallback to alternative
 *   - I/O errors: Caught and logged, no panics
 *
 * CONTAINS:
 *   - B.4a QUERIES  — Error state queries
 *   - B.4b RECOVERY — State restoration functions
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.4a QUERIES — Error State Queries [QUERIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_errors_queries B.4a QUERIES — Error State Queries
 * @ingroup seed_type_errors
 * @brief    Functions to query error state and get human-readable messages.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Get human-readable error description.
 *
 * @param[in] err  Error code from SeedError enum.
 *
 * @return Static string describing the error (do not free).
 */
static inline const char* seed_type_get_error_string(SeedError err) {
    switch (err) {
        case SEED_OK:            return "success";
        case SEED_ERR_INVALID:   return "invalid argument";
        case SEED_ERR_NOMEM:     return "out of memory";
        case SEED_ERR_IO:        return "I/O error";
        case SEED_ERR_NOT_FOUND: return "not found";
        case SEED_ERR_EXISTS:    return "already exists";
        default:                 return "unknown error";
    }
}

/**
 * @brief Check if result indicates an error.
 *
 * @param[in] result  Return value to check.
 *
 * @return true if error, false otherwise.
 */
static inline bool seed_type_is_error(int result) {
    return result != SEED_OK;
}

/** @} */ /* end B.4a QUERIES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.4b RECOVERY — State Restoration [RECOVERY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_errors_recovery B.4b RECOVERY — State Restoration
 * @ingroup seed_type_errors
 * @brief    Functions to restore valid state after error.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* seed_type_reset defined in S.4c OPERATIONS */

/** @} */ /* end B.4b RECOVERY */

/** @} */ /* end B.4 ERRORS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Exported Interface [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_api B.5 PUBLIC API — Exported Interface
 * @ingroup seed_type
 * @brief    Public API — declarations (headers).
 *
 * WHY: Top rungs of the ladder — orchestrate helpers and core operations.
 *      Simple by design — complexity lives in helpers and core operations.
 *
 * SCOPE: Lifecycle, operations, health queries, utilities.
 *
 * CONTAINS:
 *   - B.5a LIFECYCLE  — Creation, initialization, destruction
 *   - B.5b OPERATIONS — Primary functionality
 *   - B.5c HEALTH     — Health score queries
 *   - B.5d UTILITIES  — Helper functions for consumers
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a LIFECYCLE — Creation/Destruction [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_api_lifecycle B.5a LIFECYCLE — Creation/Destruction
 * @ingroup seed_type_api
 * @brief    Creation, initialization, and destruction.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* seed_type_create, seed_type_init, seed_type_destroy defined in S.4a LIFECYCLE */

/** @} */ /* end B.5a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b OPERATIONS — Primary Functionality [OPS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_api_ops B.5b OPERATIONS — Primary Functionality
 * @ingroup seed_type_api
 * @brief    Primary functionality exposed to consumers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief seed_type_execute executes seed operation.
 *
 * Executes the primary seed operation and stores result.
 *
 * @param[in,out] self  Instance to operate on. Must not be NULL..
 * @param[out]    result       Pointer to store result. Must not be NULL.
 *
 * @return 0 on success, error code on failure.
 *
 * @note Health Impact: Success +X, Validation failure -X, Processing failure -X.
 */
static inline int seed_type_execute(SeedType* self, int* result) {
    if (!self || !result) return SEED_ERR_INVALID;
    *result = self->value;
    return SEED_OK;
}

/** @} */ /* end B.5b OPERATIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c HEALTH — Health Score Queries [HEALTH]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_api_health B.5c HEALTH — Health Score Queries
 * @ingroup seed_type_api
 * @brief    Health score queries and status reporting.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Get current health score.
 *
 * @param[in] self  Instance to query. Must not be NULL.
 *
 * @return Health score (-100 to +100).
 */
static inline int seed_type_health(const SeedType* self) {
    return self ? 0 : -100;  /**< Stub: implement health tracking in source */
}

/** @} */ /* end B.5c HEALTH */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d UTILITIES — Helper Functions [UTILITIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_type_api_util B.5d UTILITIES — Helper Functions
 * @ingroup seed_type_api
 * @brief    Helper functions for consumers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* seed_type_clone defined in S.4a LIFECYCLE */
/* seed_type_validate defined in S.4c OPERATIONS */

/** @} */ /* end B.5d UTILITIES */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — What Can Be Reserved [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_omission_body B.6 OMISSION — What Can Be Reserved
 * @ingroup seed_type
 * @brief    Guidance for headers and sources on what can be reserved.
 *
 * All sections must be present. Empty sections use: [Reserved: reason]
 *
 * BODY SECTIONS:
 *   - B.1 Org Chart:  Rarely reserved — documents interface/internal structure
 *   - B.2 Types:      Core content for type headers
 *   - B.3 Helpers:    Often reserved — most helpers are static in source files
 *   - B.4 Errors:     Add queries/recovery as patterns emerge
 *   - B.5 Public API: Rarely reserved — this IS the public interface
 *
 * MULTI-FILE COMPONENTS:
 *   - Main file: Org Chart, Public APIs, Error Handling
 *   - Module files: Helpers, Core Operations
 *   - Document extraction: [Reserved: Extracted to module.c (main orchestrates)]
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING [CLOSING]
///
/// 4-Block Code Structure: Operational guidance and closing notes
///
/// Section order: Validation → Execution → Cleanup | Policy → Extension → Troubleshooting → Reference → Note → Template
/// Flow: verify → run → release | modify → extend → debug → lookup → ground → instantiate
///
/// CONTAINS:
///   - X.1 VALIDATION    — Build, syntax, self-containment verification
///   - X.2 EXECUTION     — Usage patterns and examples
///   - X.3 CLEANUP       — Ownership, memory management
///   - X.4 POLICY        — Modification guidance
///   - X.5 EXTENSION     — How to expand this component
///   - X.6 TROUBLESHOOT  — Common problems and solutions
///   - X.7 REFERENCE     — Quick lookup and copy-paste examples
///   - X.8 NOTE          — Closing guidance and grounding
///   - X.9 TEMPLATE      — Template instantiation guidance
///   - X.10 OMISSION     — What can be reserved in CLOSING
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build, syntax, and self-containment verification.
 *
 * CONTAINS:
 *   - X.1a BUILD     — Compilation commands
 *   - X.1b CHECKS    — Verification checklist
 *   - X.1c SELFTEST  — Minimal isolation test
 *   - X.1d MEMORY    — Memory leak detection
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -fsyntax-only -Wall -Wextra type-header.h    # syntax check
 * gcc -c -Wall -Wextra source.c               # compile with header
 * cppcheck --enable=all type-header.h              # static analysis
 * @endcode
 *
 * @par X.1b CHECKS — Verification Checklist
 *   - Self-contained (includes without errors)
 *   - No missing type definitions
 *   - No circular include dependencies
 *   - Include guards work (no redefinition)
 *   - Prototypes match implementations in .c
 *
 * @par X.1c SELFTEST — Minimal Isolation Test
 * @code{.c}
 * // test.c — must compile without errors
 * #include "type-header.h"
 * int main(void) { return 0; }
 * @endcode
 *
 * @par X.1d MEMORY — Memory Leak Detection
 * @code{.sh}
 * valgrind --leak-check=full ./test             # memory check
 * valgrind --track-origins=yes ./test           # uninitialized values
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_execution X.2 EXECUTION — Usage Patterns
 * @brief    Usage patterns and examples.
 *
 * Note: Headers declare, .c files implement. No code executes at include time.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.2a INCLUDE — Include Directive
 * @code{.c}
 * #include "type-header.h"
 * @endcode
 *
 * @par X.2b EXIT_CODES — Exit Code Meanings
 *   - 0: Success
 *   - 1: General error
 *   - 2: Usage/argument error
 *   - 3: Specific error meaning
 *
 * @par X.2c USAGE — Usage Example
 * @code{.c}
 * #include "type-header.h"
 *
 * int main(void) {
 *     SeedType instance;
 *     seed_type_init(&instance);
 *
 *     int result = seed_type_process(&instance);
 *     if (seed_type_is_error(result)) {
 *         fprintf(stderr, "%s\n", seed_type_get_error_string(result));
 *         return 1;
 *     }
 *
 *     seed_type_destroy(&instance);
 *     return 0;
 * }
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership and Memory [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_cleanup_guide X.3 CLEANUP — Ownership and Memory
 * @brief    Ownership patterns and memory management.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.3a OWNERSHIP — Ownership Conventions
 *   - _create() returns: Caller owns, use _destroy() to free
 *   - _init() initializes: Caller-owned memory, use _destroy() to cleanup
 *   - Pointer params: Caller retains ownership unless documented
 *   - Pointer returns: Caller owns, must free
 *
 * @par X.3b MEMORY — Memory Patterns
 * @code{.c}
 * // Stack allocation
 * SeedType local;
 * seed_type_init(&local);
 * // ... use ...
 * seed_type_destroy(&local);
 *
 * // Heap allocation
 * SeedType* ptr = seed_type_create();
 * // ... use ...
 * seed_type_destroy(ptr);
 * @endcode
 *
 * @par X.3c SHUTDOWN — Graceful Shutdown
 *   - Signal handler catches SIGINT/SIGTERM
 *   - In-progress operations complete or rollback
 *   - Resources released in reverse order
 *   - Exit with appropriate code
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
 * @defgroup seed_type_policy X.4 POLICY — Modification Guidance
 * @brief    Guide future maintainers — what's safe to change.
 *
 * SAFE TO MODIFY:
 *   - Add new functions/types/constants (follow existing patterns)
 *   - Add new helper functions in appropriate groups
 *   - Extend existing features
 *
 * MODIFY WITH CARE:
 *   - Public API signatures — breaks all calling code
 *   - Struct fields — breaks code accessing fields directly
 *   - Core behavior — affects all consumers
 *
 * NEVER MODIFY:
 *   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
 *   - Include guard pattern
 *   - Ownership conventions (_create/_destroy)
 *
 * Validation: See X.1 for build, runtime, and memory checks.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_extension X.5 EXTENSION — How to Expand
 * @brief    How to expand this component — mirrors M.10 Roadmap.
 *
 * ADDING FUNCTIONS:
 *   - Declaration here, implementation in .c file
 *   - Follow lifecycle pattern: _create/_init/_destroy
 *   - Add to BODY B.5 Public API with Health Impact
 *   - Update BODY B.1 Organizational Chart
 *
 * ADDING TYPES:
 *   - Follow existing naming conventions
 *   - Add to BODY B.2 Types with documentation
 *
 * Architecture: See BODY B.1 for Ladder/Baton structure.
 * Roadmap: See M.10 for planned features and research areas.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_troubleshoot X.6 TROUBLESHOOTING — Common Problems
 * @brief    Common problems and solutions for header issues.
 *
 * "Redefinition of seed_type":
 *   - Check include guards are unique and correct
 *
 * "Incomplete type":
 *   - Add forward declaration or include required header
 *
 * "Undefined reference to seed_type_process":
 *   - Implementation missing in .c file, or not linked
 *
 * Function-specific troubleshooting in BODY function docstrings.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_reference X.7 REFERENCE — Quick Lookup
 * @brief    Copy-paste ready examples for common operations.
 *
 * @par Basic Setup
 * @code{.c}
 * #include "type-header.h"
 * SeedType instance;
 * seed_type_init(&instance);
 * // ... use ...
 * seed_type_destroy(&instance);
 * @endcode
 *
 * @par Error Handling
 * @code{.c}
 * int result = seed_type_process(SeedType* self, &output);
 * if (result != 0) {
 *     fprintf(stderr, "Error: %s\n", seed_type_get_error_string(result));
 *     return result;
 * }
 * @endcode
 *
 * @par Health Tracking
 * @code{.c}
 * if (result == 0) {
 *     logger_success(component_logger, "operation complete", +10);
 * } else {
 *     logger_failure(component_logger, "operation failed", "reason", -10);
 * }
 * @endcode
 *
 * @par Compile
 * @code{.sh}
 * gcc -c -Wall -Wextra -std=c11 source.c
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding — echoes M.8 Scripture.
 *
 * This header declares the interface for seed_type.
 * Modify thoughtfully — changes affect all consumers.
 *
 * @par Scripture
 * "Prepare thy work without, and make it fit for thyself in the field."
 *   — Proverbs 24:27
 *
 * Establish foundations before building.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 TEMPLATE — Template Guidance [TEMPLATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_template X.9 TEMPLATE — Template Guidance
 * @brief    How to use this template — remove when instantiating.
 *
 * Usage: cp type-header.h dest.h → update pragma → fill sections
 *
 * UPDATE ON INSTANTIATION:
 *   - M.1: Key, Title
 *   - M.2: Created/Updated dates
 *   - M.4: Path
 *   - M.5: Derives_from (point to this template)
 *   - M.9: Dependencies
 *   - M.10: Roadmap
 *   - Include guard: SEED_TYPE_H
 *
 * Remove this section (X.9) when instantiating.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 OMISSION — What Can Be Reserved in CLOSING [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_type_closing_omission X.10 OMISSION — What Can Be Reserved in CLOSING
 * @brief    Guidance for CLOSING sections — what can be reserved.
 *
 * All sections must be present. Empty sections use: [Reserved: reason]
 *
 * CLOSING SECTIONS:
 *   - X.1 Validation:      Rarely reserved — all code needs verification
 *   - X.2 Execution:       Usage examples — rarely reserved
 *   - X.3 Cleanup:         Rarely reserved — ownership rules critical
 *   - X.4 Policy:          Rarely reserved — always guides maintainers
 *   - X.5 Extension:       Reference back to M.10 Roadmap
 *   - X.6 Troubleshooting: Add issues as they emerge
 *   - X.7 Reference:       Rarely reserved — examples help users
 *   - X.8 Closing Note:    Rarely reserved — grounding matters
 *   - X.9 Template:        Remove when instantiating
 *
 * Key principle: CLOSING synthesizes, METADATA/SETUP/BODY contain details.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
// END CLOSING
// ═══════════════════════════════════════════════════════════════════════════════

#endif // SEED_TYPE_H


