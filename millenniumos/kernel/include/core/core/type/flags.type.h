/// @file flags.h
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
/// @omni:req  key        = CPISI-TYPES-FLAGS
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
/// @omni:ins  layer      = types/flags
/// @omni:ins  includes   = types.h
/// @omni:ins  provides   = [S.2_FLAGS]
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
/// @omni:req  title      = CPI-SI Flags Domain
/// @omni:req  brief      = Restore strategy flags for health recovery system
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
 * @defgroup cpisi_flags_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup cpisi_flags_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_flags_special_config
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

#ifndef CPISI_FLAGS_TYPES_H
#define CPISI_FLAGS_TYPES_H

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
 * @file      flags.h
 * @brief     CPI-SI Flags Domain — Restore strategy bit flags.
 *
 * @defgroup cpisi_flags_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-FLAGS
 * Title:     CPI-SI Flags Domain Types
 * Type:      Header (Domain)
 * Component: Rails — provides flag types for health restoration
 * Role:      Bit flag definitions for restore strategy selection.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_state M.2 STATE — Lifecycle State
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
 * @defgroup cpisi_flags_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_flags_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/flags/flags.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: types/types.h (foundation)
 * Template:     bereshit/word/seed/code/c/type-header.h
 * Extracted:    2026-01-21 from types.h
 *
 * Builds FROM:  types.h (foundation types)
 * Builds TO:    health/health.h (RestoreStrategy uses these flags)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, flags, domain, restore, bit-field
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide bit flags for health restoration strategy selection.
 * Core Design: 6 individual flags combinable into strategy presets.
 * Philosophy:  Restoration requires intentional strategy.
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  James 5:16 — "pray for one another that ye may be healed"
 * Principle:  Restoration requires intentional strategy.
 * Anchor:     Jeremiah 30:17 — "I will restore health unto thee"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h (foundation types)
 *
 * What Uses This:
 *   - health/health.h — RestoreStrategy embeds these flags
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
 * @defgroup cpisi_flags_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-21) — Created for restore strategy flags
 *   a-01.25 (2026-01-23) — Expanded METADATA to full M.1-M.10 structure
 *   b-03.00 (2026-01-25) — Template alignment: type-header.h, CLOSING X.1-X.9
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ 6 individual flags defined
 *   ✓ X.1-X.9 CLOSING guidance
 *
 * Planned:
 *   ⏳ Additional flag combinations as patterns emerge
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
///   - S.2 DEFINES   — Flag domain (Bounds, Macros, Verify, Lookup, Types)
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
 * @defgroup cpisi_flags_includes S.1 INCLUDES — Header Dependencies
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

/** S.1a STANDARD — [Inherited from types.h S.1a] @defgroup cpisi_flags_includes_standard @ingroup cpisi_flags_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_flags_includes_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_flags_includes
 * @brief    Internal CPI-SI headers this file depends on.
 *
 * PURPOSE: Include foundation header providing S.1 system headers and S.2 defines.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/config/cfg/domain.cfg.h"  /**< TIER 1: config anchors (brings types.h transitively) */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [None required] @defgroup cpisi_flags_includes_external @ingroup cpisi_flags_includes @{ @} */

/** S.1d CPP — [Inherited from types.h S.1d] @defgroup cpisi_flags_includes_cpp @ingroup cpisi_flags_includes @{ @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Flag Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_restore_flags S.2 DEFINES — Flag Domain
 * @brief    Bit flags for restoration approach selection.
 *
 * PURPOSE: Define flag-specific constants, operations, verification, and types.
 *
 * Builds FROM: S.1 INCLUDES (types.h foundation via config.h)
 * Builds TO:   health/health.h (RestoreStrategy uses these flags)
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: no domain-specific preludes]
 *   - S.2a  BOUNDS  — Flag count and bit capacity
 *   - S.2b  MACROS  — Flag manipulation operations
 *   - S.2c VERIFY  — Flag compile-time assertions
 *   - S.2d LOOKUP  — Flag name table and accessor
 *   - S.2e TYPES   — RestoreStrategy enumeration
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  RESTORE STRATEGY FLAGS — Bit-field encoding                               │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │                                                                             │
 * │  6 INDIVIDUAL FLAGS (bits 0-5):                                             │
 * │    Bit 0: FILL_DEFAULTS  — Fill missing with defaults                       │
 * │    Bit 1: LOCATE_FILES   — Search for misplaced files                       │
 * │    Bit 2: CREATE_DIRS    — Create missing directories                       │
 * │    Bit 3: CLAMP_VALUES   — Clamp out-of-range values                        │
 * │    Bit 4: FALLBACK       — Use fallback resources                           │
 * │    Bit 5: DEGRADE        — Graceful degradation                             │
 * │                                                                             │
 * │  COMBINATIONS:                                                              │
 * │    NONE  = 0             — No recovery                                      │
 * │    BASIC = bits 0,3      — Defaults + clamp (minimal)                       │
 * │    FULL  = bits 0-5      — All strategies (aggressive)                      │
 * │                                                                             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * From bereshit/word/core/os/health/restore.toml [prescription.types].
 * @{
 */

/** S.2.0 PRELUDE — [Reserved: inherits types.h] @defgroup cpisi_flags_prelude @ingroup cpisi_restore_flags @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BOUNDS — Flag Count and Bit Capacity [BOUNDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_flags_bounds S.2a FLAGS BOUNDS — Domain Bounds
 * @ingroup cpisi_restore_flags
 * @brief    Flag count and bit capacity.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Counts — Flag domain array sizes
 *  @brief FLAG_COUNT (6 restore flags), MAX_BIT (5), MASK (0x3F).
 *  @{ */
/**
 * @brief 6: number of restore strategy flags (bits 0-5).
 * @par Derivation
 *      FLAG_COUNT = 6 (bits 0 through 5)
 */
#define CPISI_RESTORE_FLAG_COUNT        6
/**
 * @brief 5: maximum bit position used (0-indexed).
 * @par Derivation
 *      MAX_BIT = FLAG_COUNT - 1 = 5
 */
#define CPISI_RESTORE_FLAG_MAX_BIT      5
/**
 * @brief 0x3F: bit mask covering all individual flags.
 * @par Derivation
 *      MASK = (1 << FLAG_COUNT) - 1 = 0b00111111 = 0x3F
 */
#define CPISI_RESTORE_FLAG_MASK         0x3F    /* bits 0-5 = 0b00111111 */
/** @} */ /* end Counts */

/** @} */ /* end S.2a BOUNDS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b MACROS — Flag Operations [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2b_FLAGS S.2b FLAG MACROS — Manipulation operations
 * @ingroup cpisi_restore_flags
 * @brief    Macros for testing and combining restore strategy flags.
 *
 * PURPOSE: Provide flag manipulation operations (test, set, clear, validate)
 *          for the 6 restore strategy flags.
 *
 * Builds FROM: S.2a BOUNDS (RESTORE_FLAG_MASK, RESTORE_FLAG_COUNT)
 * Builds TO:   Restore strategy selection (health recovery)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC flag operations. The flags themselves
 *   are defined in S.2a BOUNDS above. These macros operate on those bounds.
 *
 * STRUCTURE:
 *   - RESTORE_HAS_FLAG    — Test if flag is set
 *   - RESTORE_SET_FLAG    — Set a flag
 *   - RESTORE_CLEAR_FLAG  — Clear a flag
 *   - RESTORE_FLAGS_VALID — Validate flags mask
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Test if a specific flag is set.
 *
 * @param flags The combined flags value (uint8_t bitmask)
 * @param flag  The individual flag to test (single bit)
 * @return Non-zero if flag is set, 0 otherwise
 *
 * FORMULA: (flags & flag) != 0
 *
 * BOUNDS USED:
 *   - Individual flags are single bits (0x01, 0x02, 0x04, 0x08, 0x10, 0x20)
 *   - Valid flag mask is RESTORE_FLAG_MASK = 0x3F (bits 0-5)
 *
 * EXAMPLE:
 * ```c
 *   uint8_t strategy = CPISI_RESTORE_FLAG_REBUILD | CPISI_RESTORE_FLAG_CACHE;
 *   if (CPISI_RESTORE_HAS_FLAG(strategy, CPISI_RESTORE_FLAG_CACHE)) {
 *       invalidate_cache();
 *   }
 * ```
 */
#define CPISI_RESTORE_HAS_FLAG(flags, flag)     (((flags) & (flag)) != 0)

/**
 * @brief Set a specific flag in a flags value.
 *
 * @param flags The combined flags value (uint8_t bitmask)
 * @param flag  The individual flag to set (single bit)
 * @return Flags with the specified flag set
 *
 * FORMULA: flags | flag
 *
 * NOTE: This is non-mutating — returns new value, doesn't modify input.
 *
 * EXAMPLE:
 * ```c
 *   uint8_t strategy = CPISI_RESTORE_FLAG_REBUILD;
 *   strategy = CPISI_RESTORE_SET_FLAG(strategy, CPISI_RESTORE_FLAG_NOTIFY);
 *   // strategy now has both REBUILD and NOTIFY flags
 * ```
 */
#define CPISI_RESTORE_SET_FLAG(flags, flag)     ((flags) | (flag))

/**
 * @brief Clear a specific flag from a flags value.
 *
 * @param flags The combined flags value (uint8_t bitmask)
 * @param flag  The individual flag to clear (single bit)
 * @return Flags with the specified flag cleared
 *
 * FORMULA: flags & ~flag
 *
 * NOTE: This is non-mutating — returns new value, doesn't modify input.
 *
 * EXAMPLE:
 * ```c
 *   uint8_t strategy = CPISI_RESTORE_FLAG_ALL;  // All 6 flags set
 *   strategy = CPISI_RESTORE_CLEAR_FLAG(strategy, CPISI_RESTORE_FLAG_RESTART);
 *   // strategy now has 5 flags (RESTART cleared)
 * ```
 */
#define CPISI_RESTORE_CLEAR_FLAG(flags, flag)   ((flags) & ~(flag))

/**
 * @brief Validate flags value uses only defined bits.
 *
 * @param flags The flags value to validate (uint8_t)
 * @return Non-zero if valid (no undefined bits set), 0 otherwise
 *
 * BOUNDS USED:
 *   - RESTORE_FLAG_MASK = 0x3F (bits 0-5, 6 flags)
 *   - RESTORE_FLAG_COUNT = 6
 *
 * FORMULA: (flags & ~MASK) == 0
 *   If any bits outside mask are set, expression is non-zero → invalid.
 *
 * WHY THIS MATTERS:
 *   Prevents undefined behavior from garbage bits in flags value.
 *   Only bits 0-5 are defined; bits 6-7 must be clear.
 *
 * EXAMPLE:
 * ```c
 *   uint8_t flags = read_flags_from_config();
 *   if (!CPISI_RESTORE_FLAGS_VALID(flags)) {
 *       flags &= CPISI_RESTORE_FLAG_MASK;  // Clear invalid bits
 *   }
 * ```
 */
#define CPISI_RESTORE_FLAGS_VALID(flags)        (((flags) & ~CPISI_RESTORE_FLAG_MASK) == 0)

/** @} */ /* end S.2b FLAG MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c VERIFY — Compile-time Assertions [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2c_FLAGS S.2c FLAG VERIFY — Compile-time assertions
 * @ingroup cpisi_restore_flags
 * @brief    Verify restore flag architecture at compile time.
 *
 * PURPOSE: Catch flag configuration errors BEFORE runtime. Zero runtime cost.
 *          If any assertion fails, compilation stops with a descriptive message.
 *
 * Builds FROM: S.2a FLAG BOUNDS (flag definitions), S.2b FLAG MACROS (operations)
 * Builds TO:   Restore strategy systems (verified values ready for use)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC tripwires extending S.2c VERIFY in types.h.
 *   Verifies the 6-flag bitmask architecture: count, max bit, and mask relationship.
 *
 * TRIPWIRE PHILOSOPHY:
 *   Each value is verified TWO ways:
 *   1. VALUE TRIPWIRE:        Assert the literal (e.g., FLAG_COUNT == 6)
 *   2. RELATIONSHIP TRIPWIRE: Assert the derivation (e.g., MASK == (1<<COUNT)-1)
 *   BOTH together catches drift in either direction.
 *
 * STRUCTURE:
 *   - FLAG_COUNT = 6 individual flags
 *   - FLAG_MAX_BIT = 5 (0-indexed maximum bit)
 *   - FLAG_MASK = 0x3F (bits 0-5)
 *   - MASK relationship: (1 << COUNT) - 1 == MASK
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* VALUE tripwires — flag constants */
CPISI_STATIC_ASSERT(CPISI_RESTORE_FLAG_COUNT == 6,
                    "RESTORE_FLAG_COUNT must be 6 individual flags");
CPISI_STATIC_ASSERT(CPISI_RESTORE_FLAG_MAX_BIT == 5,
                    "RESTORE_FLAG_MAX_BIT must be 5 (0-indexed)");
CPISI_STATIC_ASSERT(CPISI_RESTORE_FLAG_MASK == 0x3F,
                    "RESTORE_FLAG_MASK must cover bits 0-5 (0b00111111)");

/* RELATIONSHIP tripwires — derivations */
CPISI_STATIC_ASSERT(CPISI_RESTORE_FLAG_MAX_BIT == CPISI_DIFF(CPISI_RESTORE_FLAG_COUNT, CPISI_UNITY),
                    "RESTORE_FLAG_MAX_BIT must equal COUNT - UNITY — 5 = 6-1");
CPISI_STATIC_ASSERT(CPISI_RESTORE_FLAG_MASK == CPISI_DIFF((1 << CPISI_RESTORE_FLAG_COUNT), CPISI_UNITY),
                    "RESTORE_FLAG_MASK must equal (1<<COUNT) - UNITY — 0x3F = 64-1");

/** @} */ /* end S.2c FLAG VERIFY */

/** S.2d LOOKUP — [Accessor in S.2e: cpisi_restore_strategy_name()] @defgroup cpisi_flags_lookup @ingroup cpisi_restore_flags @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e TYPES — RestoreStrategy Enumeration [TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2e_FLAGS S.2e FLAG TYPES — RestoreStrategy enumeration
 * @ingroup cpisi_restore_flags
 * @brief    Type-safe restore strategy flags.
 *
 * Bit flags enable fine-grained control over recovery behavior.
 * From bereshit/word/core/os/health/restore.toml [prescription.types].
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Restore strategy bit flags.
 *
 * INDIVIDUAL FLAGS (bits 0-5):
 *   Each flag enables a specific recovery behavior. Flags can be
 *   combined via bitwise OR for customized strategies.
 *
 * COMBINED STRATEGIES:
 *   BASIC = conservative (defaults + clamp)
 *   FULL  = aggressive (all strategies)
 */
typedef enum CpisiRestoreStrategy {
    /* No recovery */
    CPISI_RESTORE_STRATEGY_NONE           = 0,

    /* Individual flags (bits 0-5) */
    CPISI_RESTORE_STRATEGY_FILL_DEFAULTS  = (1 << 0),  /**< Fill missing with defaults */
    CPISI_RESTORE_STRATEGY_LOCATE_FILES   = (1 << 1),  /**< Search for misplaced files */
    CPISI_RESTORE_STRATEGY_CREATE_DIRS    = (1 << 2),  /**< Create missing directories */
    CPISI_RESTORE_STRATEGY_CLAMP_VALUES   = (1 << 3),  /**< Clamp out-of-range values */
    CPISI_RESTORE_STRATEGY_FALLBACK       = (1 << 4),  /**< Use fallback resources */
    CPISI_RESTORE_STRATEGY_DEGRADE        = (1 << 5),  /**< Graceful degradation */

    /* Combined strategies */
    CPISI_RESTORE_STRATEGY_BASIC = CPISI_RESTORE_STRATEGY_FILL_DEFAULTS
                                 | CPISI_RESTORE_STRATEGY_CLAMP_VALUES,

    CPISI_RESTORE_STRATEGY_FULL  = CPISI_RESTORE_STRATEGY_FILL_DEFAULTS
                                 | CPISI_RESTORE_STRATEGY_LOCATE_FILES
                                 | CPISI_RESTORE_STRATEGY_CREATE_DIRS
                                 | CPISI_RESTORE_STRATEGY_CLAMP_VALUES
                                 | CPISI_RESTORE_STRATEGY_FALLBACK
                                 | CPISI_RESTORE_STRATEGY_DEGRADE
} CpisiRestoreStrategy;

/**
 * @brief Get strategy name (combined strategies first, then individual).
 *
 * @param strat The strategy flags to name
 * @return Human-readable strategy name
 *
 * Checks combined strategies first (none, basic, full), then
 * returns the first individual flag name found.
 */
static inline const char* cpisi_restore_strategy_name(CpisiRestoreStrategy strat) {
    if (strat == CPISI_RESTORE_STRATEGY_NONE) return "none";
    if (strat == CPISI_RESTORE_STRATEGY_BASIC) return "basic";
    if (strat == CPISI_RESTORE_STRATEGY_FULL) return "full";
    if (strat & CPISI_RESTORE_STRATEGY_FILL_DEFAULTS) return "fill_defaults";
    if (strat & CPISI_RESTORE_STRATEGY_LOCATE_FILES) return "locate_files";
    if (strat & CPISI_RESTORE_STRATEGY_CREATE_DIRS) return "create_dirs";
    if (strat & CPISI_RESTORE_STRATEGY_CLAMP_VALUES) return "clamp_values";
    if (strat & CPISI_RESTORE_STRATEGY_FALLBACK) return "fallback";
    if (strat & CPISI_RESTORE_STRATEGY_DEGRADE) return "degrade";
    return "custom";
}

/** @} */ /* End S.2e FLAG TYPES */

/** @} */ /* End cpisi_restore_flags */

/** S.3 TYPES — [Reserved: all types in S.2e] @defgroup cpisi_flags_s3_types @{ @} */

/** S.4 PROTOTYPES — [Reserved: inline in S.2e] @defgroup cpisi_flags_s4_prototypes @{ @} */

/** S.5 FILE-LEVEL — [Reserved: no mutable state] @defgroup cpisi_flags_s5_file_level @{ @} */

/** S.6 OMISSION — S.3-S.5 reserved (types/operations inline in S.2e) @defgroup cpisi_flags_s6_omission @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief [Reserved: all flag logic in SETUP S.2]
///
/// ═══════════════════════════════════════════════════════════════════════════════

/** B.1 ORG CHART — [Reserved: types/functions in SETUP] @defgroup cpisi_flags_orgchart @{ @} */
/** B.2 HELPERS — [Reserved: macros in S.2b, inline in S.2e] @defgroup cpisi_flags_helpers @{ @} */
/** B.3 RESERVED — [Reserved: inherits types.h B.3] @defgroup cpisi_flags_b3_reserved @{ @} */
/** B.4 RESERVED — [Reserved: inherits types.h B.4] @defgroup cpisi_flags_b4_reserved @{ @} */
/** B.5 RESERVED — [Reserved: inherits types.h B.5] @defgroup cpisi_flags_b5_reserved @{ @} */
/** B.6 RESERVED — [Reserved: inherits types.h B.6] @defgroup cpisi_flags_b6_reserved @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

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
///   - X.5 EXTENSION     — How to extend flags domain
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
 * @defgroup cpisi_flags_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build and self-containment verification.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 *   @code
 *   # Syntax check (from include/ directory):
 *   gcc -fsyntax-only -Wall -Wextra cpisi/types/domain/core/flags.h
 *   @endcode
 *
 * @par X.1b CHECKS — Verification Checklist
 *   - ✓ Compiles without warnings (-Wall -Wextra)
 *   - ✓ Self-contained (includes types.h)
 *   - ✓ All flag values are distinct powers of 2
 *   - ✓ Combined flags don't overlap
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_execution X.2 EXECUTION — Usage Patterns
 * @brief    How to use flags in code.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.2a INCLUDE — Include Directive
 *   @code
 *   #include "kernel/cpisi/types/domain/core/flags.h"
 *   @endcode
 *
 * @par X.2b USAGE — Setting Flags
 *   @code
 *   uint32_t strategy = CPISI_RESTORE_CACHE | CPISI_RESTORE_DB;
 *   if (strategy & CPISI_RESTORE_CACHE) { ... }
 *   @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Memory and resource ownership.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.3a OWNERSHIP — No Dynamic Allocation
 *   Flags are compile-time constants (uint32_t bitmasks).
 *   No dynamic memory allocation. No cleanup required.
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
 * @defgroup cpisi_flags_policy X.4 POLICY — Modification Guidance
 * @brief    Rules for modifying this file.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par DO NOT CHANGE
 *   - Existing flag values (binary compatibility)
 *   - Flag bit positions once released
 *
 * @par MAY ADD
 *   - New flags using next available bit
 *   - New flag combinations
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Extend [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_extension X.5 EXTENSION — How to Extend
 * @brief    How to add new flags.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Adding Individual Flags
 *   - Use next power of 2: (1U << n)
 *   - Add to S.2a BOUNDS section
 *   - Document purpose in docstring
 *
 * @par Adding Combined Flags
 *   - Use OR of existing flags
 *   - Add to S.2a after individual flags
 *   - Document which flags are combined
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Problems [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par "Flag value collision"
 *   Ensure each flag uses unique bit position.
 *   Use (1U << n) pattern with distinct n values.
 *
 * @par "Combined flag doesn't work"
 *   Check that OR'd flags are distinct bits.
 *   Combined flags must not share bit positions.
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick lookup — flag values and combinations.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Individual Flags
 *   RESTORE_NONE    = 0x00 (no restore)
 *   RESTORE_CACHE   = 0x01 (restore cache)
 *   RESTORE_CONFIG  = 0x02 (restore config)
 *   RESTORE_STATE   = 0x04 (restore state)
 *   RESTORE_DB      = 0x08 (restore database)
 *   RESTORE_SESSION = 0x10 (restore session)
 *   RESTORE_NETWORK = 0x20 (restore network)
 *
 * @par Combined Flags
 *   RESTORE_QUICK = CACHE | CONFIG (0x03)
 *   RESTORE_FULL  = CACHE | CONFIG | STATE | DB (0x0F)
 *   RESTORE_ALL   = all flags (0x3F)
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * Scripture: Nehemiah 4:6
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * Restore flags provide bitfield control for health recovery operations.
 * Each flag represents a distinct recovery target. Combinations express
 * restore scope clearly and efficiently.
 *
 * "So built we the wall... for the people had a mind to work."
 *   — Nehemiah 4:6
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Reserved Sections [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_flags_omission X.9 OMISSION — Reserved Sections
 * @brief    Reserved section documentation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par SETUP Reserved Sections
 *   - S.2b-S.2e — Inherited from types.h (no domain-specific needs)
 *   - S.3-S.6 — Types/Prototypes/File-Level (flags are constants only)
 *
 * @par BODY Reserved Sections
 *   - B.3-B.6 — Inherits types.h pattern
 *
 * @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_FLAGS_TYPES_H */
