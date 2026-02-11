#if 0  // TEMPLATE: Remove this block when instantiating
/// @file header.h
/// @omni template --header
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
/// @omni:req  key        = B-word-seed-code-c-header
/// @omni:req  from       = bereshit/word/omni/seed/B-word-omni-seed-code.omni
/// @omni:req  at         = template
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = template
/// @omni:inh  role       = seed
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = seed
/// @omni:ins  layer      = code/c
/// @omni:ins  includes   = none
/// @omni:ins  provides   = [C_HEADER_TEMPLATE]
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
/// @omni:req  title      = C Header File Template
/// @omni:req  brief      = 4-block structure template for C header files
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// END PRAGMA [END]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// USAGE: cp header.h dest.h → update pragma → fill sections
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
 * @defgroup seed_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * WHY: Settings that affect what headers provide must precede #include.
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
 * @defgroup seed_special_posix SP.1a POSIX — Feature Test
 * @ingroup seed_special_config
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

#ifndef SEED_COMPONENT_H
#define SEED_COMPONENT_H

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
 * @file      seed_component.h
 * @brief     Seed Component — Template demonstrating header structure.
 *
 * @defgroup seed_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       SEED-COMPONENT-HEADER
 * Title:     Seed Component Header Template
 * Type:      Header (Template)
 * Component: Ladder — foundational type definitions
 * Role:      Demonstrates 4-block header structure with compilable examples
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   a-01.00
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
 * @defgroup seed_attribution M.3 ATTRIBUTION — Authors and Copyright
 * @{
 *
 * @author    Nova Dawn (Architect)
 * @author    Seanje Lenox-Wise (Lead)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Path [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_location M.4 LOCATION — File Path
 * @{
 *
 * Path: bereshit/word/seed/code/c/header.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: None — canonical template source
 * Extracted:    2025-12-01 initial template creation
 *
 * Builds FROM:  None (root template)
 * Builds TO:    All C header files in project
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: template, header, c, seed, 4-block
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Compilable template for C header files with seed_ schema
 * Core Design: 4-block structure (METADATA, SETUP, BODY, CLOSING)
 * Philosophy:  Structure enables consistency; examples enable learning
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Genesis 1:11 — "Let the earth bring forth... seed"
 * Principle:  Seeds contain the pattern for what they produce
 * Anchor:     Galatians 6:7 — "whatsoever a man soweth, that shall he also reap"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: Standard library headers only (stdint, stdbool, stddef)
 *
 * What Uses This:
 *   - All C header files created from this template
 *
 * @see source.c for corresponding source template
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2025-12-01) — Initial template creation
 *   a-01.25 (2026-01-25) — Added seed_ schema for compilable examples
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ Compilable with gcc -fsyntax-only
 *
 * Planned:
 *   ⏳ Doxygen generation validation
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
///   - S.2 DEFINES   — Constants and macros (Sizes, Limits, Flags, Macros)
///   - S.3 TYPES     — Type declarations
///   - S.4 PROTOS    — Function prototypes
///   - S.5 EXTERN    — External declarations
///
/// Section order: Includes → Defines → Types → Prototypes → Extern
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
 * @defgroup seed_includes S.1 INCLUDES — Header Dependencies
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
 * @defgroup seed_standard S.1a STANDARD — System Headers
 * @ingroup seed_includes
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
 * @defgroup seed_internal S.1b INTERNAL — Project Headers
 * @ingroup seed_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * FOUNDATION FILES: May include specific component headers.
 * DOMAIN FILES: Include types.h for foundation types + macros.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Foundation file: No internal includes needed (we ARE the foundation) */
/* Domain file: Uncomment to include foundation types */
// #include "types.h"  /**< Foundation: S.1 system headers + S.2 universal defines */

/* Or specific internal headers */
// #include "seed_internal.h"  /**< Internal component header */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup seed_includes
 * @brief    External library headers with justification.
 *
 * JUSTIFICATION REQUIRED: Each external header must document why standard library
 * is insufficient and what capability it provides.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [None — standard library only] */
// #include <external.h>  /**< [justification — what stdlib lacks] */

/** @} */ /* end S.1c EXTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_cpp S.1d CPP — C++ Linkage
 * @ingroup seed_includes
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
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Constants and macros for this component.
 *
 * PURPOSE: Define component-specific constants and utility macros.
 *
 * Builds FROM: S.1 INCLUDES (types from included headers)
 * Builds TO:   S.3 TYPES (constants used in type definitions)
 *
 * CONTAINS:
 *   - S.2a SIZES   — Buffer and array sizes
 *   - S.2b LIMITS  — Value bounds (min, max, default)
 *   - S.2c FLAGS   — Boolean and bit flags
 *   - S.2d MACROS  — Utility macros (MIN, MAX, CLAMP)
 *   - S.2e VERIFY  — Compile-time assertions (tripwires)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a SIZES — Buffer and Array Sizes [SIZES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_sizes S.2a SIZES — Buffer and Array Sizes
 * @ingroup seed_defines
 * @brief    Buffer sizes, array capacities, element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Sizes — Capacity and count constants
 *  @brief Buffer sizes, array capacities, element counts.
 *  @{ */
#define SEED_VALUE_SIZE                 1024                /**< 1024: buffer size                    */
#define SEED_VALUE_CAPACITY             256                 /**< 256: array capacity                  */
#define SEED_VALUE_COUNT                32                  /**< 32: element count                    */
/** @} */ /* end Sizes */

/** @} */ /* end S.2a SIZES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b LIMITS — Value Bounds [LIMITS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_limits S.2b LIMITS — Value Bounds
 * @ingroup seed_defines
 * @brief    Minimum, maximum, and default values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Limits — Minimum, maximum, and default values
 *  @brief Value bounds with MIN/CENTER/MAX derivation pattern.
 *  @{ */
/**
 * @brief 0: minimum seed value — lower bound.
 * @par Derivation
 *      SEED_VALUE_MIN = 0 (explicit minimum)
 */
#define SEED_VALUE_MIN                  0
/**
 * @brief 255: maximum seed value — upper bound.
 * @par Derivation
 *      SEED_VALUE_MAX = 255 (explicit maximum)
 */
#define SEED_VALUE_MAX                  255
/**
 * @brief 100: default seed value — typical starting value.
 * @par Derivation
 *      SEED_VALUE_DEFAULT = 100 (explicit default)
 */
#define SEED_VALUE_DEFAULT              100
/** @} */ /* end Limits */

/** @} */ /* end S.2b LIMITS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c FLAGS — Boolean and Bit Flags [FLAGS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_flags S.2c FLAGS — Boolean and Bit Flags
 * @ingroup seed_defines
 * @brief    Feature toggles and bit patterns.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#define SEED_VALUE_ENABLED 1
#define SEED_VALUE_DISABLED 0
#define SEED_VALUE_FLAG_ACTIVE (1 << 0)

/** @} */ /* end S.2c FLAGS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d MACROS — Utility Macros [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_macros S.2d MACROS — Utility Macros
 * @ingroup seed_defines
 * @brief    Utility macros for common operations.
 *
 * STRUCTURE:
 *   S.2d.1a OPERATIONS   — Arithmetic helpers (MIN, MAX, CLAMP)
 *   S.2d.1b HELPERS      — Utility helpers (ARRAY_LEN, UNUSED)
 *   S.2d.1c CONVERSION   — [Reserved] Format conversion macros
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup seed_macros_ops S.2d.1a OPERATIONS — Arithmetic helpers
 * @ingroup seed_macros
 * @brief MIN, MAX, CLAMP operations.
 * @{
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) (MIN(MAX(x, lo), hi))
/** @} */ /* end S.2d.1a OPERATIONS */

/**
 * @defgroup seed_macros_helpers S.2d.1b HELPERS — Utility helpers
 * @ingroup seed_macros
 * @brief ARRAY_LEN, UNUSED helpers.
 * @{
 */
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#define UNUSED(x) ((void)(x))
/** @} */ /* end S.2d.1b HELPERS */

/**
 * @defgroup seed_macros_conv S.2d.1c CONVERSION [Reserved]
 * @ingroup seed_macros
 * @brief [Reserved] Format conversion macros.
 * @{
 */
/** @} */ /* end S.2d.1c CONVERSION */

/** @} */ /* end S.2d MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e VERIFY — Compile-time Assertions [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_verify S.2e VERIFY — Compile-time Assertions
 * @ingroup seed_defines
 * @brief    Static assertions validating configuration at compile time.
 *
 * PURPOSE: Tripwires that catch configuration errors before runtime.
 *          If any assertion fails, compilation stops with a descriptive message.
 *
 * TRIPWIRE TYPES:
 *   VALUE        — Verify literals: "This constant IS this value"
 *   RELATIONSHIP — Verify derivations: "A equals f(B)"
 *   EQUIVALENCE  — Verify connections: "These two things ARE equal"
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Example tripwires (uncomment and adapt):
 *
 * // VALUE: Verify literal
 * _Static_assert([CONSTANT] == [VALUE], "[CONSTANT] must be [VALUE]");
 *
 * // RELATIONSHIP: Verify derivation
 * _Static_assert([DERIVED] == [FORMULA], "[DERIVED] must equal [FORMULA]");
 *
 * // EQUIVALENCE: Verify connection
 * _Static_assert([A] == [B], "[A] must equal [B]");
 */

/* [Reserved: Add component-specific assertions as configuration emerges] */

/** @} */ /* end S.2e VERIFY */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_types S.3 TYPES — Type Declarations
 * @ingroup seed
 * @brief    Data structures and type definitions.
 *
 * PURPOSE: Enumerations, structs, opaque types, error codes.
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
 * @defgroup seed_enums S.3a ENUMS — Enumeration Types
 * @ingroup seed_types
 * @brief    Type-safe enumeration definitions.
 *
 * PURPOSE: State values, mode selectors, status codes.
 *
 * PATTERN:
 *   SEED_NONE  = 0  — default/invalid (always 0)
 *   SEED_COUNT      — sentinel for array sizing
 *
 * Builds FROM: S.3 TYPES (section context)
 * Builds TO:   S.3b STRUCTS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SeedStatus represents component operational states.
 *
 * Tracks whether the seed component is inactive, active, or in an error state.
 * Used for state machine transitions and status reporting.
 *
 * @note SEED_NONE is always 0 (default/invalid).
 * @note SEED_COUNT is sentinel for array sizing.
 */
typedef enum {
    SEED_NONE = 0,   /**< @brief Default/invalid value (always 0). */
    SEED_ACTIVE,     /**< @brief Component is active and operational. */
    SEED_COUNT       /**< @brief Sentinel for array sizing (do not use as value). */
} SeedStatus;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — Structure Definitions [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_structs S.3b STRUCTS — Structure Definitions
 * @ingroup seed_types
 * @brief    Concrete data structure definitions.
 *
 * PURPOSE: Model domain concepts with explicit field layout.
 *
 * PATTERN:
 *   SeedType represents a basic data container.
 *   Fields: value: primary integer value
 *
 * Builds FROM: S.3a ENUMS (enum types used in fields)
 * Builds TO:   S.3c OPAQUE
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SeedType represents a basic data container.
 *
 * Primary structure for demonstrating struct patterns in templates.
 * Contains a single value field for illustration purposes.
 *
 * Fields:
 *   - value: primary integer data value
 */
typedef struct {
    int value;  /**< @brief Primary value field. */
} SeedType;

/** @} */ /* end S.3b STRUCTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3c OPAQUE — Forward-Declared Opaque Types [OPAQUE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_opaque S.3c OPAQUE — Forward-Declared Opaque Types
 * @ingroup seed_types
 * @brief    Opaque types with hidden implementation.
 *
 * PURPOSE: Hide internal details — implementation in .c files.
 *
 * PATTERN:
 *   typedef struct SeedType SeedType; — internals hidden in source
 *
 * Builds FROM: S.3b STRUCTS
 * Builds TO:   S.3d ERRORS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SeedOpaque opaque handle (example pattern).
 *
 * Internal structure hidden in implementation file.
 * Use seed_opaque_create() to allocate and seed_opaque_destroy() to free.
 *
 * @note This is an ALTERNATIVE pattern to S.3b STRUCTS.
 *       Use either concrete structs OR opaque types, not both for same type.
 *
 * @see seed_opaque_create
 * @see seed_opaque_destroy
 */
typedef struct SeedOpaque SeedOpaque;

/** @} */ /* end S.3c OPAQUE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d ERRORS — Error Code Enumeration [ERRORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_errors S.3d ERRORS — Error Code Enumeration
 * @ingroup seed_types
 * @brief    Domain-specific error codes.
 *
 * PURPOSE: Type-safe error return values.
 *
 * PATTERN:
 *   SEED_OK          = 0  — success (always 0)
 *   SEED_ERR_*            — specific error conditions
 *
 * Builds FROM: S.3c OPAQUE
 * Builds TO:   S.4 PROTOTYPES
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief SeedError error codes for seed component operations.
 *
 * All functions returning int use these codes.
 * Success is always 0; errors are positive (enumerated).
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
 * @defgroup seed_prototypes S.4 PROTOTYPES — Function Declarations
 * @ingroup seed
 * @brief    Public API function declarations.
 *
 * PURPOSE: Function signatures for lifecycle, accessors, operations.
 *
 * STRUCTURE:
 *   S.4a LIFECYCLE  — create, init, destroy, clone
 *   S.4b ACCESSORS  — get, set, is queries
 *   S.4c OPERATIONS — action verbs, reset, validate
 *
 * Builds FROM: S.3 TYPES (types used in signatures)
 * Builds TO:   S.5 FILE-LEVEL (extern declarations)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Create/Init/Destroy/Clone [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_lifecycle S.4a LIFECYCLE — Create/Init/Destroy/Clone
 * @ingroup seed_prototypes
 * @brief    Object lifecycle management functions.
 *
 * PURPOSE: Allocation, initialization, cleanup, copying.
 *
 * PATTERN:
 *   seed_create  — allocate + init, caller must destroy
 *   seed_init    — init existing (stack-allocated)
 *   seed_destroy — free resources (NULL-safe)
 *   seed_clone   — deep copy
 *
 * Builds FROM: S.4 PROTOTYPES (section context)
 * Builds TO:   S.4b ACCESSORS
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
SeedType* seed_type_create(void);

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
int seed_type_init(SeedType* self);

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
void seed_type_destroy(SeedType* self);

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
SeedType* seed_type_clone(const SeedType* src);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ACCESSORS — Get/Set/Is Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_accessors S.4b ACCESSORS — Get/Set/Is Queries
 * @ingroup seed_prototypes
 * @brief    Field access and state query functions.
 *
 * PURPOSE: Read/write fields, query boolean state.
 *
 * PATTERN:
 *   seed_get_value — retrieve field value
 *   seed_set_value — update field value
 *   seed_is_valid  — boolean queries
 *
 * Builds FROM: S.4a LIFECYCLE
 * Builds TO:   S.4c OPERATIONS
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
int seed_type_get_value(const SeedType* self);

/**
 * @brief Set the value value on a SeedType.
 *
 * @param[in,out] self   Pointer to SeedType. Must not be NULL.
 * @param[in]     value  New value value to set.
 */
void seed_type_set_value(SeedType* self, int value);

/**
 * @brief Check if SeedType is in valid state.
 *
 * @param[in] self  Pointer to SeedType. Must not be NULL.
 *
 * @return true if in valid state, false otherwise.
 */
bool seed_type_is_valid(const SeedType* self);

/** @} */ /* end S.4b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c OPERATIONS — Verb/Reset/Validate [OPERATIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_operations S.4c OPERATIONS — Verb/Reset/Validate
 * @ingroup seed_prototypes
 * @brief    Action functions that modify state.
 *
 * PURPOSE: Domain actions, state reset, invariant checking.
 *
 * PATTERN:
 *   seed_process    — action, returns 0 on success
 *   seed_reset     — reset to initial state
 *   seed_validate  — check invariants
 *
 * Builds FROM: S.4b ACCESSORS
 * Builds TO:   S.5 FILE-LEVEL
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Process the SeedType with variadic arguments.
 *
 * Performs the primary operation on a SeedType instance.
 * Variadic arguments allow flexible parameter passing.
 *
 * @param[in,out] self  Pointer to SeedType. Must not be NULL.
 * @param[in]     ...   Additional parameters as needed by implementation.
 *
 * @return SEED_OK on success, error code on failure.
 */
int seed_type_process(SeedType* self, ...);

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
int seed_type_reset(SeedType* self);

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
int seed_type_validate(const SeedType* self);

/** @} */ /* end S.4c OPERATIONS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File Scope Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_file S.5 FILE-LEVEL — File Scope Declarations
 * @ingroup seed
 * @brief    File-level state and type declarations.
 *
 * PURPOSE: Extern variables (headers), static (sources), callbacks.
 *
 * STRUCTURE:
 *   S.5a VARIABLES      — extern/static variable declarations
 *   S.5b FUNC_TYPES     — callback function typedefs
 *   S.5c INFRASTRUCTURE — logger, inspector (sources only)
 *
 * Builds FROM: S.4 PROTOTYPES (function types reference these)
 * Builds TO:   S.6 OMISSION
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5a VARIABLES — Extern/Static Variables [VARIABLES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_vars S.5a VARIABLES — Extern/Static Variables
 * @ingroup seed_file
 * @brief    File-scope variable declarations.
 *
 * PURPOSE: Shared state (extern in headers), file state (static in sources).
 *
 * PATTERN:
 *   extern SeedType g_seed_instance;       — shared state (headers)
 *   static SeedType s_seed_state;       — file state (sources)
 *
 * Builds FROM: S.5 FILE-LEVEL (section context)
 * Builds TO:   S.5b FUNC_TYPES
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief g_seed_instance is the global seed component instance.
 *
 * Provides a single shared instance for simple use cases.
 * For multi-instance scenarios, use seed_type_create() instead.
 *
 * @note Thread-safety: unsafe — requires external synchronization
 * @note Modified by: seed_type_init(), seed_type_reset()
 */
extern SeedType g_seed_instance;

/**
 * @brief g_seed_defaults holds immutable default configuration.
 *
 * Contains the default values used when initializing new instances.
 * Use for resetting or comparing against baseline state.
 *
 * @note Immutable after initialization.
 */
extern const SeedType g_seed_defaults;

/** @} */ /* end S.5a VARIABLES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5b FUNC_TYPES — Callback Typedefs [FUNC_TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_func_types S.5b FUNC_TYPES — Callback Typedefs
 * @ingroup seed_file
 * @brief    Callback function type definitions.
 *
 * PURPOSE: Function pointer types for callbacks and hooks.
 *
 * PATTERN:
 *   typedef int (*SeedCallbackFn)(...); — callback signature
 *
 * Builds FROM: S.5a VARIABLES
 * Builds TO:   S.5c INFRASTRUCTURE
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Simple callback for events.
 *
 * Called when an event occurs that requires no return value.
 *
 * @param[in,out] ctx  User context pointer passed to registration.
 */
typedef void (*SeedEventFn)(void* ctx);

/**
 * @brief Callback with result for iterating/filtering.
 *
 * Called when processing items that may be filtered or stopped.
 *
 * @param[in]     arg  The item being processed.
 * @param[in,out] ctx  User context pointer passed to registration.
 *
 * @return 0 to continue, non-zero to stop processing.
 */
typedef int (*SeedIteratorFn)(int arg, void* ctx);

/** @} */ /* end S.5b FUNC_TYPES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5c INFRASTRUCTURE — Logger/Inspector [INFRASTRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_infra S.5c INFRASTRUCTURE — Logger/Inspector
 * @ingroup seed_file
 * @brief    Reserved for infrastructure declarations.
 *
 * PURPOSE: Logger, inspector — health tracking infrastructure.
 *          Lives in source files, not headers.
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
 * @defgroup seed_omission S.6 OMISSION — Reserved Section Guide
 * @ingroup seed
 * @brief    Documents what sections are reserved and why.
 *
 * PURPOSE: Explicit documentation of intentionally empty sections.
 *          All sections must be present. Empty sections use: [Reserved: reason]
 *
 * RESERVED SECTIONS (typical for headers):
 *   S.5c INFRASTRUCTURE — Lives in source files, not headers
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
/// @brief The actual work — declarations (headers), implementations (sources).
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART    — Structure overview, flow, counts
///   - B.2 HELPERS      — Foundation functions (pure, utilities)
///   - B.3 CORE OPS     — Business logic by category
///   - B.4 ERRORS       — Error handling patterns
///   - B.5 PUBLIC API   — Exported interface
///
/// Section order: Org Chart → Helpers → Core Ops → Errors → Public API
/// Flow: map structure → foundations → business logic → error handling → exported interface
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_orgchart B.1 ORG CHART — Structure Overview
 * @ingroup seed
 * @brief    Map structure — interface (headers), internals (sources).
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
 * @defgroup seed_orgchart_structure B.1a STRUCTURE — Interface Overview
 * @ingroup seed_orgchart
 * @brief    Types, functions, and externs declared in this header.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      2 structs (SeedType, SeedOpaque), 2 enums (SeedStatus, SeedError) */
/* Functions:  4 lifecycle, 3 accessors, 3 operations */
/* Externs:    2 globals, 2 callbacks */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_orgchart_flow B.1b FLOW — Implementation Location
 * @ingroup seed_orgchart
 * @brief    Where implementations live.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: seed_component.c */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_orgchart_counts B.1c COUNTS — Summary Statistics
 * @ingroup seed_orgchart
 * @brief    Interface element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      4 total */
/* Functions:  10 total */
/* Externs:    4 total */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Foundation Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_helpers B.2 HELPERS — Foundation Functions
 * @ingroup seed
 * @brief    Foundation functions — building blocks for higher operations.
 *
 * WHY: Separate helper functions from public API. Helpers are the building
 *      blocks that public API functions compose.
 *
 * SCOPE: Internal use primarily. Declare in headers only if needed across
 *        multiple .c files in the same component.
 *
 * CONTAINS:
 *   - B.2a PURE       — Side-effect-free transformations
 *   - B.2b UTILITIES  — Common operations with potential side effects
 *
 * Note: Most helpers are static in sources. Reserve sections as needed.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a PURE — Side-Effect-Free Functions [PURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_helpers_pure B.2a PURE — Side-Effect-Free Functions
 * @ingroup seed_helpers
 * @brief    Pure functions — same input always produces same output.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Pure helpers typically static in source file] */

/** @} */ /* end B.2a PURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b UTILITIES — Common Operations [UTILITIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_helpers_utilities B.2b UTILITIES — Common Operations
 * @ingroup seed_helpers
 * @brief    Utility operations with potential side effects.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Utility helpers typically static in source file] */

/** @} */ /* end B.2b UTILITIES */

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Component-Specific Functionality [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_core_ops B.3 CORE OPS — Component-Specific Functionality
 * @ingroup seed
 * @brief    Business logic organized by operational category.
 *
 * WHY: Separate core operations from helpers (B.2) and public API (B.5).
 *      Operations implement the domain logic; API orchestrates them.
 *
 * SCOPE: Component-specific functionality — declarations (headers),
 *        implementations (sources).
 *
 * CONTAINS:
 *   - B.3a [Category]  — [Description of category]
 *   - B.3b [Category]  — [Description of category]
 *
 * COMMON CATEGORIES:
 *   - Validation:  Input checking, constraint verification
 *   - Conversion:  Data transformation between formats
 *   - Processing:  Core algorithms and computations
 *   - Formatting:  Output preparation
 *   - Analysis:    Data examination and metrics
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a [CATEGORY] — [Category Name] [CATEGORY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_ops_category B.3a [CATEGORY] — [Category Name]
 * @ingroup seed_core_ops
 * @brief    [High-level description of this category].
 *
 * WHY SEPARATED: [Reasoning for this grouping — what makes these operations
 *                 distinct from other categories].
 *
 * EXTENSION POINT:
 *   To add new [operation type]:
 *     1. Create function following [naming pattern]
 *     2. Implement with [specific behavior]
 *     3. Integrate with [orchestration function]
 *     4. Update tests
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief seed_transform [does what].
 *
 * [Detailed description of what this function does and why.]
 *
 * @param[in]     paramName  [Purpose and expected values].
 * @param[out]    result     [Output parameter description].
 *
 * @return [returnType] on success.
 * @retval 0              Success.
 * @retval -1             [Error condition and meaning].
 *
 * @par Health Impact
 *   - Success: +X points ([reasoning for value])
 *   - Failure: -X points ([reasoning for value])
 *
 * @par Troubleshooting
 *   - Problem: "[common error message]"
 *     - Check: [What to verify]
 *     - Solution: [How to fix]
 *
 * @see [related_function]
 */
int seed_transform(SeedType* self);

/** @} */ /* end B.3a [CATEGORY] */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b [CATEGORY2] — [Category 2 Name] [CATEGORY2]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_ops_category2 B.3b [CATEGORY2] — [Category 2 Name]
 * @ingroup seed_core_ops
 * @brief    [Reserved: Additional category as needed].
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Add operations as component develops] */

/** @} */ /* end B.3b [CATEGORY2] */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Error Handling [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_errors_handling B.4 ERRORS — Error Handling
 * @ingroup seed
 * @brief    Error handling — query declarations (headers), management patterns (sources).
 *
 * WHY: Centralize error handling patterns for consistent behavior across
 *      the component. Separate from core operations for clarity.
 *
 * SCOPE: Error state queries, human-readable messages, recovery functions.
 *
 * CONTAINS:
 *   - B.4a QUERIES   — Error state queries and messages
 *   - B.4b RECOVERY  — Functions to restore valid state
 *
 * DESIGN PRINCIPLE: [Blocking/Non-blocking]
 *   Example: Non-blocking - seed_component failures never interrupt [main operation]
 *
 * RECOVERY STRATEGY:
 *   - [Error type 1]: Graceful degradation (fallback behavior)
 *   - [Error type 2]: Fallback to alternative
 *   - [Error type 3]: Caught and logged, no panics
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.4a QUERIES — Error State Queries [QUERIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_error_queries B.4a QUERIES — Error State Queries
 * @ingroup seed_errors_handling
 * @brief    Functions to query error state and get human-readable messages.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Get human-readable error description.
 *
 * Returns a static string describing the error. Do not free.
 *
 * @param[in] err  Error code from SeedError enum.
 *
 * @return Static string (do not free).
 */
const char* seed_get_error_string(SeedError err);

/**
 * @brief Check if result indicates error.
 *
 * Utility function for consistent error checking.
 *
 * @param[in] result  Return value to check.
 *
 * @return true if error, false otherwise.
 */
bool seed_is_error(int result);

/** @} */ /* end B.4a QUERIES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.4b RECOVERY — State Recovery [RECOVERY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_error_recovery B.4b RECOVERY — State Recovery
 * @ingroup seed_errors_handling
 * @brief    Functions to restore valid state after error.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Restore instance to valid state after error.
 *
 * Resets internal state to known-good values. Use after catching
 * recoverable errors to continue operation.
 *
 * @param[in,out] self  Instance to reset. Must not be NULL.
 *
 * @return 0 on success, error code on failure.
 */
int seed_reset(SeedType* self);

/** @} */ /* end B.4b RECOVERY */

/** @} */ /* end B.4 ERRORS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Exported Interface [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_public_api B.5 PUBLIC API — Exported Interface
 * @ingroup seed
 * @brief    Public API — declarations (headers), implementations (sources).
 *
 * WHY: Separate public interface from internal helpers (B.2) and core
 *      operations (B.3). Public APIs are the "top rungs of the ladder" —
 *      they orchestrate proven pieces.
 *
 * SCOPE: Lifecycle management, primary operations, health queries, utilities.
 *
 * CONTAINS:
 *   - B.5a LIFECYCLE   — Creation, initialization, destruction
 *   - B.5b OPERATIONS  — Primary functionality exposed to consumers
 *   - B.5c HEALTH      — Health score queries and status reporting
 *   - B.5d UTILITIES   — Helper functions for consumers
 *
 * DESIGN: Simple by design — complexity lives in helpers and core operations,
 *         Public APIs orchestrate proven pieces.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a LIFECYCLE — Creation/Initialization/Destruction [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_api_lifecycle B.5a LIFECYCLE — Creation/Initialization/Destruction
 * @ingroup seed_public_api
 * @brief    Instance lifecycle management.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Allocate and initialize a new instance.
 *
 * Creates a heap-allocated instance. Caller is responsible for
 * calling seed_type_destroy() when done.
 *
 * @return New instance (caller owns), or NULL on failure.
 *
 * @par Health Impact
 *   - Success: +10 points (component ready)
 *   - Failure: -20 points (initialization failed)
 *
 * @see seed_type_destroy
 */
SeedType* seed_type_create(void);

/**
 * @brief Initialize caller-owned memory.
 *
 * Initializes a stack-allocated or pre-allocated instance.
 * Does not allocate memory for the structure itself.
 *
 * @param[out] self  Instance to initialize. Must not be NULL.
 *
 * @return 0 on success, error code on failure.
 */
int seed_type_init(SeedType* self);

/**
 * @brief Free resources (NULL-safe).
 *
 * Releases all resources associated with the instance.
 * Safe to call with NULL (no-op).
 *
 * @param[in,out] self  Instance to destroy, or NULL.
 *
 * @post self is invalid after this call.
 */
void seed_type_destroy(SeedType* self);

/** @} */ /* end B.5a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b OPERATIONS — Primary Functionality [OPS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_api_operations B.5b OPERATIONS — Primary Functionality
 * @ingroup seed_public_api
 * @brief    Primary functionality exposed to consumers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief seed_execute [does what at high level].
 *
 * [Detailed description of what this function does and why.]
 *
 * @param[in]  paramName  [Purpose and expected values].
 * @param[out] result     [Output parameter description].
 *
 * @return 0 on success, error code on failure.
 *
 * @par Health Impact
 *   - Success: +X points ([reasoning])
 *   - Validation failure: -X points ([reasoning])
 *   - Processing failure: -X points ([reasoning])
 */
int seed_execute(SeedType* self, int* result);

/** @} */ /* end B.5b OPERATIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c HEALTH — Health Queries [HEALTH]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_api_health B.5c HEALTH — Health Queries
 * @ingroup seed_public_api
 * @brief    Health score queries and status reporting.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Get current health score.
 *
 * Returns the component's health score representing operational status.
 *
 * @param[in] self  Instance to query. Must not be NULL.
 *
 * @return Health score (-100 to +100).
 */
int seed_type_health(const SeedType* self);

/** @} */ /* end B.5c HEALTH */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d UTILITIES — Consumer Helpers [UTIL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_api_utilities B.5d UTILITIES — Consumer Helpers
 * @ingroup seed_public_api
 * @brief    Helper functions for consumers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Create a deep copy.
 *
 * Allocates and returns a new instance with all fields copied from src.
 * Caller is responsible for calling seed_type_destroy() on the copy.
 *
 * @param[in] src  Instance to copy. Must not be NULL.
 *
 * @return New instance (caller owns), or NULL on failure.
 *
 * @see seed_type_destroy
 */
SeedType* seed_type_clone(const SeedType* src);

/**
 * @brief Check invariants.
 *
 * Validates that all internal state is consistent and valid.
 * Use for debugging and assertions.
 *
 * @param[in] self  Instance to validate. Must not be NULL.
 *
 * @return 0 if valid, error code indicating first violation.
 */
int seed_type_validate(const SeedType* self);

/** @} */ /* end B.5d UTILITIES */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_body_omission B.6 OMISSION — Reserved Section Guide
 * @ingroup seed
 * @brief    What can be reserved and why — guidance for headers and sources.
 *
 * WHY: All sections must be present for structural consistency.
 *      Empty sections use: [Reserved: reason]
 *
 * SCOPE: BODY section reservation guidelines.
 *
 * SECTION GUIDELINES:
 *   - B.1 Org Chart:   Rarely reserved — documents interface/internal structure
 *   - B.2 Helpers:     Often reserved — most helpers are static in source files
 *   - B.3 Core Ops:    Add categories as component develops
 *   - B.4 Errors:      Add queries/recovery as patterns emerge
 *   - B.5 Public API:  Rarely reserved — this IS the public interface
 *
 * MULTI-FILE COMPONENTS:
 *   - Main file: Org Chart, Public APIs, Error Handling
 *   - Module files: Helpers, Core Operations
 *   - Document extraction: [Reserved: Extracted to module.c (main orchestrates)]
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: B.6 is documentation-only — no code content] */

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
///   - X.1 VALIDATION      — Verify correctness (build, checks, tests)
///   - X.2 EXECUTION       — Usage patterns and examples
///   - X.3 CLEANUP         — Ownership and memory patterns
///   - X.4 POLICY          — Guide future maintainers
///   - X.5 EXTENSION       — How to expand this component
///   - X.6 TROUBLESHOOTING — Common problems and solutions
///   - X.7 REFERENCE       — Quick lookup (copy-paste ready)
///   - X.8 NOTE            — Final guidance and grounding
///   - X.9 TEMPLATE        — How to use this template
///   - X.10 OMISSION       — What can be reserved and why
///
/// Section order: Validation → Execution → Cleanup | Policy → Extension →
///                Troubleshooting → Reference → Note → Template → Omission
/// Flow: verify → run → release | modify → extend → debug → lookup → ground → instantiate
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_validation X.1 VALIDATION — Verify Correctness
 * @ingroup seed
 * @brief    Verify correctness — headers (syntax, self-containment),
 *           sources (build, runtime, memory).
 *
 * WHY: All code needs verification before deployment. Validation sections
 *      provide the commands and checks to ensure correctness.
 *
 * SCOPE: Build commands, correctness checks, memory verification.
 *
 * CONTAINS:
 *   - X.1a BUILD     — Compilation commands
 *   - X.1b CHECKS    — Correctness verification
 *   - X.1c SELF_TEST — Self-containment test
 *   - X.1d MEMORY    — Memory leak and error detection
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.1a BUILD — Compilation Commands [BUILD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_validation_build X.1a BUILD — Compilation Commands
 * @ingroup seed_validation
 * @brief    Commands to build and validate this component.
 *
 * @code{.sh}
 * gcc -fsyntax-only -Wall -Wextra seed_component.h    # syntax check
 * gcc -c -Wall -Wextra seed_component.c               # compile with header
 * cppcheck --enable=all seed_component.h              # static analysis
 * @endcode
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Build commands documented in defgroup above] */

/** @} */ /* end X.1a BUILD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.1b CHECKS — Correctness Verification [CHECKS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_validation_checks X.1b CHECKS — Correctness Verification
 * @ingroup seed_validation
 * @brief    Correctness checks for headers and sources.
 *
 * VERIFICATION CHECKLIST:
 *   - Self-contained (includes without errors)
 *   - No missing type definitions
 *   - No circular include dependencies
 *   - Include guards work (no redefinition)
 *   - Prototypes match implementations in .c
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Verification checklist documented in defgroup above] */

/** @} */ /* end X.1b CHECKS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.1c SELF_TEST — Self-Containment Test [SELF_TEST]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_validation_selftest X.1c SELF_TEST — Self-Containment Test
 * @ingroup seed_validation
 * @brief    Test that header is self-contained.
 *
 * @code{.c}
 * // test.c — must compile without errors
 * #include "seed_component.h"
 * int main(void) { return 0; }
 * @endcode
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Self-test documented in defgroup above] */

/** @} */ /* end X.1c SELF_TEST */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.1d MEMORY — Memory Verification [MEMORY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_validation_memory X.1d MEMORY — Memory Verification
 * @ingroup seed_validation
 * @brief    Memory leak and error detection commands.
 *
 * @code{.sh}
 * valgrind --leak-check=full ./test             # memory check
 * valgrind --track-origins=yes ./test           # uninitialized values
 * @endcode
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Memory commands documented in defgroup above] */

/** @} */ /* end X.1d MEMORY */

/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_execution X.2 EXECUTION — Usage Patterns
 * @ingroup seed
 * @brief    Execution — usage patterns (headers), entry point and flow (sources).
 *
 * WHY: Headers declare, .c files implement. No code executes at include time.
 *      This section documents how to use the declared interface.
 *
 * SCOPE: Include patterns, exit codes, usage examples.
 *
 * CONTAINS:
 *   - X.2a INCLUDE    — How to include this header
 *   - X.2b EXIT_CODES — Standard exit code meanings
 *   - X.2c USAGE      — Complete usage example
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.2a INCLUDE — How to Include [INCLUDE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_execution_include X.2a INCLUDE — How to Include
 * @ingroup seed_execution
 * @brief    Standard include pattern.
 *
 * @code{.c}
 * #include "seed_component.h"
 * @endcode
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Include pattern documented in defgroup above] */

/** @} */ /* end X.2a INCLUDE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.2b EXIT_CODES — Standard Exit Codes [EXIT_CODES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_execution_exitcodes X.2b EXIT_CODES — Standard Exit Codes
 * @ingroup seed_execution
 * @brief    Exit code meanings for this component.
 *
 * EXIT CODES:
 *   - 0 — Success
 *   - 1 — General error
 *   - 2 — Usage/argument error
 *   - [N] — [Specific error meaning]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Exit codes documented in defgroup above] */

/** @} */ /* end X.2b EXIT_CODES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.2c USAGE — Complete Usage Example [USAGE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_execution_usage X.2c USAGE — Complete Usage Example
 * @ingroup seed_execution
 * @brief    Complete example of using this component.
 *
 * @code{.c}
 * #include "seed_component.h"
 *
 * int main(void) {
 *     SeedType instance;
 *     seed_type_init(&instance);
 *
 *     int result = seed_type_process(&instance);
 *     if (seed_is_error(result)) {
 *         fprintf(stderr, "%s\n", seed_get_error_string(result));
 *         return 1;
 *     }
 *
 *     seed_type_destroy(&instance);
 *     return 0;
 * }
 * @endcode
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Usage example documented in defgroup above] */

/** @} */ /* end X.2c USAGE */

/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership and Memory [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_cleanup X.3 CLEANUP — Ownership and Memory
 * @ingroup seed
 * @brief    Cleanup — ownership patterns (headers), memory and shutdown (sources).
 *
 * WHY: Clear ownership rules prevent memory leaks and use-after-free bugs.
 *      Shutdown patterns ensure graceful termination.
 *
 * SCOPE: Ownership conventions, memory patterns, shutdown procedures.
 *
 * CONTAINS:
 *   - X.3a OWNERSHIP — Who owns what
 *   - X.3b MEMORY    — Stack vs heap patterns
 *   - X.3c SHUTDOWN  — Graceful termination
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.3a OWNERSHIP — Who Owns What [OWNERSHIP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_cleanup_ownership X.3a OWNERSHIP — Who Owns What
 * @ingroup seed_cleanup
 * @brief    Memory ownership conventions.
 *
 * OWNERSHIP RULES:
 *   - _create() returns:   Caller owns, use _destroy() to free
 *   - _init() initializes: Caller-owned memory, use _destroy() to cleanup
 *   - Pointer params:      Caller retains ownership unless documented
 *   - Pointer returns:     Caller owns, must free
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Ownership rules documented in defgroup above] */

/** @} */ /* end X.3a OWNERSHIP */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.3b MEMORY — Stack vs Heap Patterns [MEMORY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_cleanup_memory X.3b MEMORY — Stack vs Heap Patterns
 * @ingroup seed_cleanup
 * @brief    Memory allocation patterns.
 *
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
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Memory patterns documented in defgroup above] */

/** @} */ /* end X.3b MEMORY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.3c SHUTDOWN — Graceful Termination [SHUTDOWN]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup seed_cleanup_shutdown X.3c SHUTDOWN — Graceful Termination
 * @ingroup seed_cleanup
 * @brief    Graceful shutdown procedures.
 *
 * SHUTDOWN SEQUENCE:
 *   - Signal handler catches SIGINT/SIGTERM
 *   - In-progress operations complete or rollback
 *   - Resources released in reverse order
 *   - Exit with appropriate code
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Shutdown sequence documented in defgroup above] */

/** @} */ /* end X.3c SHUTDOWN */

/** @} */ /* end X.3 CLEANUP */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_policy X.4 POLICY — Modification Guidelines
 * @ingroup seed
 * @brief    Guide future maintainers — what's safe to change.
 *
 * WHY: Clear modification guidelines prevent accidental breakage and
 *      guide new contributors toward safe changes.
 *
 * SCOPE: Safe, careful, and forbidden modifications.
 *
 * SAFE TO MODIFY:
 *   - ✅ Add new functions/types/constants (follow existing patterns)
 *   - ✅ Add new helper functions in appropriate groups
 *   - ✅ Extend existing features
 *
 * MODIFY WITH CARE:
 *   - ⚠️ Public API signatures — breaks all calling code
 *   - ⚠️ Struct fields — breaks code accessing fields directly
 *   - ⚠️ Core behavior — affects all consumers
 *
 * NEVER MODIFY:
 *   - ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
 *   - ❌ Include guard pattern
 *   - ❌ Ownership conventions (_create/_destroy)
 *
 * @see X.1 VALIDATION for build, runtime, and memory checks.
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
 * @defgroup seed_extension X.5 EXTENSION — How to Expand
 * @ingroup seed
 * @brief    How to expand this component — mirrors M.10 Roadmap.
 *
 * WHY: Clear extension patterns ensure consistency as the component grows.
 *
 * SCOPE: Adding functions, adding types, architectural guidance.
 *
 * ADDING FUNCTIONS:
 *   - Declaration here, implementation in .c file
 *   - Follow lifecycle pattern: _create/_init/_destroy
 *   - Add to BODY B.3 Core Operations with Health Impact
 *   - Update BODY B.1 Organizational Chart
 *
 * ADDING TYPES:
 *   - Follow existing naming conventions
 *   - Add to SETUP S.3 Types with documentation
 *
 * @see B.1 ORG CHART for Ladder/Baton structure.
 * @see M.10 ROADMAP for planned features and research areas.
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
 * @defgroup seed_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @ingroup seed
 * @brief    Common problems and solutions — header and runtime issues.
 *
 * WHY: Document known issues to save future debugging time.
 *
 * SCOPE: Compilation errors, runtime issues, common mistakes.
 *
 * COMMON ERRORS:
 *
 * "Redefinition of seed_type":
 *   → Check include guards are unique and correct
 *
 * "Incomplete type":
 *   → Add forward declaration or include required header
 *
 * "Undefined reference to seed_operation":
 *   → Implementation missing in .c file, or not linked
 *
 * @note Function-specific troubleshooting in BODY B.3 function docstrings.
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
 * @defgroup seed_reference X.7 REFERENCE — Quick Lookup
 * @ingroup seed
 * @brief    Quick lookup — copy-paste ready examples.
 *
 * WHY: Provide ready-to-use code snippets for common operations.
 *
 * SCOPE: Basic setup, error handling, health tracking, compilation.
 *
 * BASIC SETUP:
 * @code{.c}
 * #include "seed_component.h"
 * SeedType instance;
 * seed_type_init(&instance);
 * // ... use ...
 * seed_type_destroy(&instance);
 * @endcode
 *
 * ERROR HANDLING:
 * @code{.c}
 * int result = seed_operation(SeedType* self, &output);
 * if (result != 0) {
 *     fprintf(stderr, "Error: %s\n", seed_get_error_string(result));
 *     return result;
 * }
 * @endcode
 *
 * HEALTH TRACKING:
 * @code{.c}
 * if (success) {
 *     logger_success(component_logger, "operation complete", +10);
 * } else {
 *     logger_failure(component_logger, "operation failed", "reason", -10);
 * }
 * @endcode
 *
 * COMPILE:
 * @code{.sh}
 * gcc -c -Wall -Wextra -std=c11 seed_component.c
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
 * @defgroup seed_note X.8 NOTE — Final Guidance
 * @ingroup seed
 * @brief    Final guidance and grounding — echoes M.8.
 *
 * This header declares the interface for seed_component.
 * Modify thoughtfully — changes affect all consumers.
 *
 * @par Biblical Grounding
 * "Prepare thy work without, and make it fit for thyself in the field."
 * — Proverbs 24:27
 *
 * Establish foundations before building.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Final guidance documented in defgroup above] */

/** @} */ /* end X.8 NOTE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 TEMPLATE — Template Guidance [TEMPLATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_template X.9 TEMPLATE — Template Guidance
 * @ingroup seed
 * @brief    How to use this template — remove when instantiating.
 *
 * USAGE: cp header.h dest.h → update pragma → fill sections
 *
 * UPDATE ON INSTANTIATION:
 *   - M.1: Key, Title
 *   - M.2: Created/Updated dates
 *   - M.4: Path
 *   - M.5: Derives_from (point to this template)
 *   - M.9: Dependencies
 *   - M.10: Roadmap
 *   - Include guard: SEED_COMPONENT_H
 *
 * @note Remove this section (X.9) when instantiating.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Template guidance documented in defgroup above] */

/** @} */ /* end X.9 TEMPLATE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup seed_closing_omission X.10 OMISSION — Reserved Section Guide
 * @ingroup seed
 * @brief    What can be reserved and why — guidance for CLOSING sections.
 *
 * WHY: All sections must be present for structural consistency.
 *      Empty sections use: [Reserved: reason]
 *
 * SCOPE: CLOSING section reservation guidelines.
 *
 * SECTION GUIDELINES:
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
 * KEY PRINCIPLE: CLOSING synthesizes, METADATA/SETUP/BODY contain details.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: X.10 is documentation-only — no code content] */

/** @} */ /* end X.10 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * C++ Linkage Closure (Foundation file)
 * ───────────────────────────────────────────────────────────────────────────── */
#ifdef __cplusplus
}
#endif

#endif /* SEED_COMPONENT_H */


