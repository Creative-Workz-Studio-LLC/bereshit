/// @file types.h
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
/// @omni:req  key        = CPISI-TYPES-FOUNDATION
/// @omni:req  from       = templates/code/c/header.h
/// @omni:req  at         = a-01.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = foundation
/// @omni:inh  role       = types
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = types
/// @omni:ins  includes   = stdlib_only
/// @omni:ins  provides   = [S.1_INCLUDES, S.2_DEFINES]
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
/// @omni:req  title      = CPI-SI Universal Type Foundation
/// @omni:req  brief      = Mathematical primitives from Genesis 1:1. Include FIRST.
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
///   - SP.1 CONFIG  — Compile-time settings (POSIX, MODE)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * WHY: Settings that affect what headers provide must precede \#include.
 *
 * CONTAINS:
 *   - SP.1a POSIX  — Platform feature test (universal)
 *   - SP.1b MODE   — Layer selection (types.h only)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1a POSIX — Feature Test [POSIX]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_special_config
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
 * SP.1b MODE — Layer Selection [MODE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_special_mode SP.1b MODE — Layer Selection
 * @ingroup cpisi_special_config
 * @brief    9-layer tier selection for conditional standard library includes.
 *
 * WHY: Different contexts need different headers. Kernel code can't use stdio.
 *      Define CPISI_MODE before include to select tier: -DCPISI_MODE=N (0-8).
 *
 * SCOPE: Foundation only — types.h defines, others inherit.
 *
 * ARCHITECTURE (4+1+4):
 *   BELOW CENTER (0-3): Kernel → System → Runtime → Framework
 *   CENTER (4):         Health — observation point, recovery
 *   ABOVE CENTER (5-8): Network → Storage → Engine → App
 *
 * CUMULATIVE: Higher modes include all lower mode headers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name SP.1b.1 Constants — Tier Identifiers [CONST]
 *  @brief The 9 layers of the architecture.
 *  @{ */
#define CPISI_MODE_KERNEL      0  /**< Freestanding — stdint, stdbool, stddef only */
#define CPISI_MODE_SYSTEM      1  /**< Minimal POSIX — adds errno */
#define CPISI_MODE_RUNTIME     2  /**< VM execution — adds limits, float, time, string, stdlib */
#define CPISI_MODE_FRAMEWORK   3  /**< Libraries — adds stdio [DEFAULT] */
#define CPISI_MODE_HEALTH      4  /**< CENTER — adds signal, setjmp for recovery */
#define CPISI_MODE_NETWORK     5  /**< Protocols — adds socket, netinet, arpa, netdb */
#define CPISI_MODE_STORAGE     6  /**< Filesystem — adds stat, dirent, fcntl, unistd */
#define CPISI_MODE_ENGINE      7  /**< Rendering — adds pthread, math */
#define CPISI_MODE_APP         8  /**< User-facing — adds locale, wchar, wctype, assert, complex */
/** @} */ /* end Constants */

/** @name SP.1b.2 Selection — Current Mode [SELECT]
 *  @brief Default and validation.
 *  @{ */
#ifndef CPISI_MODE
#define CPISI_MODE  CPISI_MODE_FRAMEWORK  /**< Default: FRAMEWORK (3) if not specified */
#endif

#if CPISI_MODE < 0 || CPISI_MODE > 8
#error "CPISI_MODE must be 0-8"  /* Compile-time guard — invalid mode is fatal */
#endif
/** @} */ /* end Selection */

/** @name SP.1b.3 Queries — Position Checks [QUERY]
 *  @brief Where are we relative to CENTER (layer 4)?
 *  @{ */
#define CPISI_MODE_IS_BELOW   (CPISI_MODE < 4)   /**< Below CENTER — constrained (0-3) */
#define CPISI_MODE_IS_CENTER  (CPISI_MODE == 4)  /**< At CENTER — observation point (4) */
#define CPISI_MODE_IS_ABOVE   (CPISI_MODE > 4)   /**< Above CENTER — expanded (5-8) */
/** @} */ /* end Queries */

/** @} */ /* end SP.1b MODE */

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef CPISI_TYPES_H
/** @def CPISI_TYPES_H
 *  @brief Include guard — prevents multiple inclusion of this foundation header.
 */
#define CPISI_TYPES_H

/* ─────────────────────────────────────────────────────────────────────────────
 * FREESTANDING KERNEL MODE GUARD
 *
 * When MILLENNIUMOS_KERNEL_H is defined (kernel.h was included first) and we're
 * in freestanding mode (CPISI_MODE 0-1), skip this entire file. Kernel.h already
 * provides all necessary types for the kernel layer.
 * ───────────────────────────────────────────────────────────────────────────── */
#if defined(MILLENNIUMOS_KERNEL_H) && CPISI_MODE < 2
/* Kernel freestanding mode - types.h content skipped, using kernel.h types */
#else
/* Full types.h content follows */

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Identity and context for this component.
///
/// WHY METADATA EXISTS:
///   Every file needs identity — who it is, where it came from, why it exists.
///   This is not PRAGMA (machine-readable), not SPECIAL (pre-include config).
///   METADATA is for humans and documentation tools.
///
/// STRUCTURE: M.1-M.10 sections covering full lifecycle context.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      types.h
 * @brief     CPI-SI Universal Type Foundation — Single source of truth for all types.
 *
 * @defgroup cpisi_types_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-FOUNDATION
 * Title:     CPI-SI Universal Type Foundation
 * Type:      Header (Foundation)
 * Component: Rails — provides types all other components build on
 * Role:      Single source of truth for CPI-SI types. Include FIRST.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   b-03.00
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
 * @defgroup cpisi_types_attribution M.3 ATTRIBUTION — Authors and Copyright
 * @{
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Implementation)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * Organization: CreativeWorkzStudio LLC
 *
 * @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Path [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/types.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage and Dependencies [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_derivation M.5 DERIVATION — Lineage and Dependencies
 * @{
 *
 * Derives_from: bereshit/word/seed/code/c/type-header.h
 * Lineage:      type-header.h template → types.h (this file)
 *
 * Builds FROM:
 *   - stdint.h   — Fixed-width integer types
 *   - stdbool.h  — Boolean type
 *   - stddef.h   — size_t, NULL, offsetof
 *
 * Builds TO:
 *   - types/[domain].h — Domain type files (config, error, health, etc.)
 *   - dar.h      — Data Access Records
 *   - cpisi.h    — Main CPI-SI interface
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, foundation, cpisi, ternary, 9-layer, genesis, cornerstone
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide mathematical primitives derived from Genesis 1:1.
 * Core Design: Config-driven types on unified scales, layer-aware sizing.
 * Philosophy:  Types derive from cognitive counts; all values traceable to Scripture.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Cognitive Counts — The numbers that matter
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   0   VOID    — Before creation, formless (Genesis 1:2)
 *   1   UNITY   — THE foundation, God is One (Deuteronomy 6:4)
 *   3   TERNARY — Extremes + center (Ecclesiastes 4:12)
 *   9   FACE    — 3² = cube face, 4+1+4 structure
 *   27  CUBE    — 3³ = mental construct (Revelation 21:16)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * 9-Layer Architecture — The 4+1+4 pattern
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   BELOW CENTER (0-3):
 *     0  KERNEL    3⁰=1     — Hardware, tightest constraints
 *     1  SYSTEM    3¹=3     — OS services, system calls
 *     2  RUNTIME   3²=9     — VM/runtime, managed execution
 *     3  FRAMEWORK 3³=27    — Libraries, CUBE abstraction
 *
 *   CENTER (4):
 *     4  HEALTH    3⁴=81    — Observation point, recovery
 *
 *   ABOVE CENTER (5-8):
 *     5  NETWORK   3⁵=243   — Connectivity, protocols
 *     6  STORAGE   3⁶=729   — Filesystem, persistence
 *     7  ENGINE    3⁷=2187  — Rendering, physics, audio
 *     8  APP       3⁸=6561  — User-facing, maximum flexibility
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Key Formulas
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   SIZE = BASE × 3^layer
 *   BRIDGE: 243 + 13 = 256 (ternary↔binary, PENTERACT + BRIDGE = BYTE)
 *
 * Key Features:
 *   - Balanced ternary primitives (trit, tryte, word)
 *   - Hebrew state types for cognitive modeling
 *   - Health and error types for system monitoring
 *   - Layer-aware sizing for memory efficiency
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Genesis 1:1 — "In the beginning God created"
 * Principle:  All structure flows from the Creator; types reflect divine order.
 * Anchor:     1 Corinthians 14:40 — "Let all things be done decently and in order"
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Biblical Foundations — Where these types come from
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   ⚓ Genesis 1:1       — "In the beginning God created" — halt condition
 *   ⚓ Genesis 1:2       — "without form, and void" — VOID, before structure
 *   ⚓ Deuteronomy 6:4   — "The LORD our God is one" — UNITY, indivisible
 *   ⚓ Proverbs 8:27     — "set a compass upon the deep" — BOUNDARIES
 *   ⚓ Ecclesiastes 4:12 — "a threefold cord" — TERNARY, not easily broken
 *   ⚓ 1 Corinthians 14:40 — "decently and in order" — WHY types exist
 *   ⚓ Revelation 21:16  — "the city lieth foursquare" — CUBE structure
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — What This Needs and What Uses It [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Standard Library: stdint.h, stdbool.h, stddef.h
 *   External:         None
 *   Internal:         None — this is the foundation
 *
 * What Uses This:
 *   Domain Types:     types/config/, types/error/, types/health/, etc.
 *   Core Headers:     dar.h, cpisi.h
 *   All CPI-SI Code:  Everything includes types.h first
 *
 * Integration Points:
 *   - Foundation for entire CPI-SI type system
 *   - Defines primitives other headers build on
 *   - Layer selection (CPISI_MODE) controls header inclusion
 *
 * @see types/config/config.h   — Configuration types
 * @see types/error/error.h     — Error handling types
 * @see types/health/health.h   — Health monitoring types
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * History
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   a-01.00 (2025-12-01) — Initial types: HebrewState, HealthLevel, Severity
 *   b-01.00 (2026-01-10) — Beta: stable types, MultiScore, TernaryScore
 *   b-01.10 (2026-01-17) — Makefile-style refinement, docstrings, org tables
 *   b-01.15 (2026-01-17) — Config-driven refactor, derivation pattern
 *   b-01.25 (2026-01-17) — 9-layer CUBE-SPHERE-PYRAMID architecture
 *   b-02.00 (2026-01-18) — Unified Info struct, B.2-B.5 pattern
 *   b-02.10 (2026-01-19) — Golden standard: PRAGMA/METADATA/SETUP, S.1-S.6
 *   b-02.25 (2026-01-21) — Domain extraction (types-*.h files created)
 *   b-02.50 (2026-01-23) — PRAGMA/SPECIAL/METADATA M.1-M.10 alignment
 *   b-03.00 (2026-01-25) — Full 4-block alignment to type-header.h template
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Current
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   ✓ PRAGMA block — @omni format with P.1-P.5 structure
 *   ✓ SPECIAL block — SP.1 CONFIG with POSIX and MODE
 *   ✓ METADATA block — M.1-M.10 structure with grep tags
 *   ✓ SETUP block — S.1-S.6 fully aligned with type-header.h template
 *   ✓ BODY block — B.1-B.6 fully aligned with template, full docstrings
 *   ✓ CLOSING block — X.1-X.9 fully aligned with template
 *   ✓ Domain files — Deflattened to types/{domain}/ subdirectories
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Planned
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   ⏳ Cascade structure to domain files (health.h, phase.h, config/log.h)
 *   ⏳ Update templates with full block examples
 *   ⏳ Sync other headers with proper imports
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Limitations
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   - Domain files pending alignment with this structure
 *   - Other cpisi headers need import updates
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Research
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   - Balanced ternary arithmetic optimization
 *   - Layer-aware memory allocation patterns
 *   - Hebrew state machine formal verification
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * Operational
 * ───────────────────────────────────────────────────────────────────────────────
 *
 *   N/A — Foundation header, not a runtime component.
 *   Types defined here are used by operational components.
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
///   - S.1 INCLUDES  — Header dependencies (Standard, Internal, External, CPP)
///   - S.2 DEFINES   — Constants and macros (Prelude, Bounds, Macros, Verify, Lookup, Types)
///
/// Section order: Includes (S.1) → Defines (S.2)
/// Flow: dependencies → prelude ops → bounds → macros → verification → tables → types
///
/// Builds FROM: METADATA block (identity established)
/// Builds TO:   BODY block (implementations)
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
 * @defgroup cpisi_includes S.1 INCLUDES — Header Dependencies
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
 * @defgroup cpisi_includes_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_includes
 * @brief    Standard library includes, conditional on CPISI_MODE.
 *
 * MODE-CONDITIONAL INCLUDES:
 *   Headers are cumulative — each layer includes all headers from layers below.
 *   CPISI_MODE is set in SPECIAL block (SP.1b MODE).
 *
 *   ┌───────┬─────────────┬────────────────────────────────────────────────────┐
 *   │ Layer │ Name        │ Headers Provided                                   │
 *   ├───────┼─────────────┼────────────────────────────────────────────────────┤
 *   │   0   │ KERNEL      │ stdint, stdbool, stddef (always)                   │
 *   │   1   │ SYSTEM      │ errno                                              │
 *   │   2   │ RUNTIME     │ limits, float, time, string, stdlib                │
 *   │   3   │ FRAMEWORK   │ stdio [default mode]                               │
 *   │   4   │ HEALTH      │ signal, setjmp (CENTER)                            │
 *   │   5   │ NETWORK     │ sys/types, sys/socket, netinet/in, arpa/inet, netdb│
 *   │   6   │ STORAGE     │ sys/stat, dirent, fcntl, unistd                    │
 *   │   7   │ ENGINE      │ pthread, math                                      │
 *   │   8   │ APP         │ locale, wchar, wctype, assert, complex             │
 *   └───────┴─────────────┴────────────────────────────────────────────────────┘
 *
 * @see cpisi_special_mode for MODE configuration.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* ── KERNEL (0) — Freestanding ─────────────────────────────────────────────── */
/** @name Layer 0: KERNEL — Freestanding (no OS dependency)
 *  @brief Core C types. Always included regardless of MODE.
 *  @{ */
#include <stdint.h>   /**< Fixed-width integers: int8_t, int16_t, int32_t, int64_t, uint8_t, etc. */
#include <stdbool.h>  /**< Boolean type: bool, true, false */
#include <stddef.h>   /**< Standard definitions: size_t, ptrdiff_t, NULL, offsetof */
/** @} */ /* end Layer 0: KERNEL */

/* ── SYSTEM (1) — Minimal POSIX ────────────────────────────────────────────── */
/** @name Layer 1: SYSTEM — Error Reporting
 *  @brief Error number infrastructure for system calls.
 *  @{ */
#if CPISI_MODE >= 1
#include <errno.h>    /**< Error codes: errno, EINVAL, ENOMEM, ENOENT, etc. */
#endif
/** @} */ /* end Layer 1: SYSTEM */

/* ── RUNTIME (2) — VM Execution ────────────────────────────────────────────── */
/** @name Layer 2: RUNTIME — Core Runtime Support
 *  @brief Limits, time, memory, and string operations.
 *  @{ */
#if CPISI_MODE >= 2
#include <limits.h>   /**< Implementation limits: INT_MAX, INT_MIN, UINT_MAX, CHAR_BIT */
#include <float.h>    /**< Floating-point limits: FLT_MAX, DBL_MAX, FLT_EPSILON */
#include <time.h>     /**< Time and date: time_t, struct tm, time(), strftime() */
#include <string.h>   /**< String operations: memcpy, memset, strlen, strcmp, strcpy */
#include <stdlib.h>   /**< General utilities: malloc, free, exit, atoi, qsort, rand */
#endif
/** @} */ /* end Layer 2: RUNTIME */

/* ── FRAMEWORK (3) — Libraries [default] ───────────────────────────────────── */
/** @name Layer 3: FRAMEWORK — Standard I/O (default mode)
 *  @brief File and console I/O for library-level code.
 *  @{ */
#if CPISI_MODE >= 3
#include <stdio.h>    /**< Standard I/O: FILE, printf, fprintf, fopen, fread, fwrite */
#endif
/** @} */ /* end Layer 3: FRAMEWORK */

/* ── HEALTH (4) — CENTER ───────────────────────────────────────────────────── */
/** @name Layer 4: HEALTH — Recovery Infrastructure (CENTER)
 *  @brief Signal handling and non-local jumps for error recovery.
 *  @{ */
#if CPISI_MODE >= 4
#include <signal.h>   /**< Signal handling: signal(), SIGINT, SIGTERM, SIGSEGV */
#include <setjmp.h>   /**< Non-local jumps: jmp_buf, setjmp(), longjmp() */
#endif
/** @} */ /* end Layer 4: HEALTH */

/* ── NETWORK (5) — Protocols ───────────────────────────────────────────────── */
/** @name Layer 5: NETWORK — Communication Infrastructure
 *  @brief Socket programming and network address handling.
 *  @{ */
#if CPISI_MODE >= 5
#include <sys/types.h>   /**< System data types: ssize_t, off_t, pid_t */
#include <sys/socket.h>  /**< Socket interface: socket(), bind(), listen(), accept() */
#include <netinet/in.h>  /**< Internet address: struct sockaddr_in, INADDR_ANY, htons() */
#include <arpa/inet.h>   /**< IP conversion: inet_pton(), inet_ntop(), inet_addr() */
#include <netdb.h>       /**< Network database: gethostbyname(), getaddrinfo() */
#endif
/** @} */ /* end Layer 5: NETWORK */

/* ── STORAGE (6) — Filesystem ──────────────────────────────────────────────── */
/** @name Layer 6: STORAGE — File and Directory Operations
 *  @brief File metadata, directory traversal, and low-level I/O.
 *  @{ */
#if CPISI_MODE >= 6
#include <sys/stat.h>    /**< File status: stat(), struct stat, S_ISDIR, S_ISREG */
#include <dirent.h>      /**< Directory entries: opendir(), readdir(), closedir() */
#include <fcntl.h>       /**< File control: open(), O_RDONLY, O_CREAT, O_TRUNC */
#include <unistd.h>      /**< POSIX API: read(), write(), close(), fork(), exec() */
#endif
/** @} */ /* end Layer 6: STORAGE */

/* ── ENGINE (7) — Rendering ────────────────────────────────────────────────── */
/** @name Layer 7: ENGINE — Threading and Mathematics
 *  @brief Concurrent execution and mathematical functions.
 *  @{ */
#if CPISI_MODE >= 7
#include <pthread.h>     /**< POSIX threads: pthread_create(), pthread_mutex_t */
#include <math.h>        /**< Math functions: sin(), cos(), sqrt(), pow(), log() */
#endif
/** @} */ /* end Layer 7: ENGINE */

/* ── APP (8) — User-facing ─────────────────────────────────────────────────── */
/** @name Layer 8: APP — Internationalization and Advanced Features
 *  @brief Localization, wide characters, and diagnostic tools.
 *  @{ */
#if CPISI_MODE >= 8
#include <locale.h>      /**< Localization: setlocale(), LC_ALL, localeconv() */
#include <wchar.h>       /**< Wide characters: wchar_t, wprintf(), wcscpy() */
#include <wctype.h>      /**< Wide char classification: iswalpha(), towupper() */
#include <assert.h>      /**< Runtime assertions: assert(), static_assert() */
#include <complex.h>     /**< Complex numbers: _Complex, cabs(), carg(), cexp() */
#endif
/** @} */ /* end Layer 8: APP */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_includes_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_includes
 * @brief    Internal CPI-SI headers this file depends on.
 *
 * PURPOSE: Include project-specific headers after system headers.
 *          Order: foundation → domain → utility
 *
 * PATTERN:
 *   \#include "types.h"       // Foundation (if not self)
 *   \#include "types-foo.h"   // Domain dependencies
 *   \#include "kernel/cpisi/bar.h"   // CPI-SI utilities
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Foundation file — no internal dependencies] */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_includes_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_includes
 * @brief    External library headers with justification.
 *
 * PURPOSE: Include third-party dependencies after internal headers.
 *          Each external include requires justification (what stdlib lacks).
 *
 * PATTERN:
 *   \#include <external/lib.h>  // Justification: [what it provides stdlib lacks]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Standard library only — no external dependencies] */

/** @} */ /* end S.1c EXTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_includes_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_includes
 * @brief    extern "C" wrapper for C++ compatibility.
 *
 * PURPOSE: Allow C++ code to include this C header without name mangling.
 *          Opening brace here; closing brace in CLOSING block.
 *
 * WHY: C++ compilers mangle function names for overloading support.
 *      extern "C" preserves C linkage for cross-language compatibility.
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
 * S.2 DEFINES [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_defines S.2 DEFINES — Preprocessor Constants and Macros
 * @brief    Unified type foundation: constants, operations, verification, types.
 *
 * PURPOSE: Define all constants, macros, and compile-time values for CPI-SI.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE  — Arithmetic operations (bootstrap before values)
 *   - S.2a  BOUNDS   — Domain counts, ranges, and limits (by derivation tier)
 *   - S.2b  MACROS   — Operations on bounds
 *   - S.2c  VERIFY   — Compile-time assertions (tripwires)
 *   - S.2d  LOOKUP   — Runtime lookup tables
 *   - S.2e  TYPES    — Type-safe enumerations
 *
 * FLOW: Prelude provides ops → Bounds use ops → Macros operate → Verify validates
 *       → Lookup provides runtime → Types wrap everything
 *
 * QUICK REFERENCE:
 *   Need arithmetic?    → S.2.0 PRELUDE   (operations before values)
 *   Need a constant?    → S.2a  BOUNDS    (all constants by derivation tier)
 *   Need an operation?  → S.2b  MACROS    (operations on bounds)
 *   Need validation?    → S.2c  VERIFY    (compile-time tripwires)
 *   Need runtime data?  → S.2d  LOOKUP    (tables and arrays)
 *   Need a type?        → S.2e  TYPES     (enums and structs)
 *
 * DERIVATION CHAIN:
 *   Genesis 1:1 → VOID(0) → UNITY(1) → 2,3,4 → 5,7,9 → 27,81,243 → 256
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2.0 PRELUDE — Foundational Operations [PRELUDE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2_0_PRELUDE S.2.0 PRELUDE — Foundational Operations
 * @ingroup cpisi_defines
 * @brief    THE instruction set for constant derivation — anchors, primitives,
 *           composites, and derivation patterns.
 *
 * PURPOSE: Provide the complete operational foundation that BOUNDS uses to derive
 *          all constants from VOID(0) and UNITY(1). Every value in the CPI-SI
 *          system traces back through these operations to Genesis 1:1.
 *
 * Builds FROM: S.2 DEFINES (section context)
 * Builds TO:   S.2a BOUNDS (uses these operations for all constant derivation)
 *
 * WHY PRELUDE EXISTS:
 *   BOUNDS needs operations to express derivations like "FACE = SQUARE_OF(TERNARY)".
 *   Operations must exist BEFORE the values that use them. This creates the
 *   derivation chain: Genesis 1:1 → anchors → primitives → composites → values.
 *
 * DAR WITNESS ROLE:
 *   These operations ARE the atomic actions. Every health pipeline calculation
 *   decomposes to PRIMITIVES. COMPOSITES combine them. DERIVATION patterns
 *   produce measurable bounds. The Witness tracks: input → operation → output.
 *
 * CONTAINS:
 *   - S.2.0.0 BOOTSTRAP   — THE two anchors: VOID(0) and UNITY(1)
 *   - S.2.0a  PRIMITIVES  — Irreducible operations: SUM, DIFF, PRODUCT, QUOTIENT, MODULO, NEGATE
 *   - S.2.0b  COMPOSITES  — Function macros: POWER, SHIFT, BOUND (+ aliases)
 *   - S.2.0c  DERIVATION  — Patterns: MIN_OF, MAX_OF, CENTER_OF, mappings, predicates
 *
 * DERIVATION CHAIN:
 *   Genesis 1:1 → VOID(0) → UNITY(1) → PRIMITIVES → COMPOSITES → DERIVATION → BOUNDS
 *
 * THREE CONSOLIDATED FUNCTION MACROS (in COMPOSITES):
 *   - POWER(n, exp)    — Exponentiation: n^exp for exponents 0-8
 *   - SHIFT(n, dir)    — Binary scaling: dir<0 halves, dir>0 doubles
 *   - BOUND(a, b, dir) — Value selection: dir<0 minimum, dir>0 maximum
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2.0.0 BOOTSTRAP — THE Two Anchors [BOOTSTRAP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2_0_BOOTSTRAP S.2.0.0 BOOTSTRAP — THE Two Anchors
 * @ingroup S2_0_PRELUDE
 * @brief    VOID(0) and UNITY(1) — the halt conditions for all derivation.
 *           NOTHING precedes these. They ARE the anchor of anchors.
 *
 * Builds FROM: Genesis 1:1 (THE anchor of anchors — "In the beginning God created")
 * Builds TO:   S.2.0a PRIMITIVES (irreducible operations use these anchors)
 *
 * BIBLICAL FOUNDATION:
 *   VOID   = 0   Genesis 1:2 "formless and void"    THE zero — absence, halt
 *   UNITY  = 1   Deuteronomy 6:4 "LORD is ONE"      THE one — identity, increment
 *
 * MATHEMATICAL PROPERTIES:
 *   - VOID is additive identity:        n + VOID = n
 *   - UNITY is multiplicative identity: n × UNITY = n
 *   - All values derive:                2 = UNITY + UNITY, 3 = 2 + UNITY, ...
 *
 * DAR WITNESS: These are NOT measured — they ARE the measurement reference.
 *              When derivation traces back to VOID or UNITY, recursion halts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief THE zero — Genesis 1:2 formless anchor.
 *
 * "And the earth was without form, and void" — the starting point before
 * structure exists. The halt condition when tracing derivation backward.
 *
 * @return 0 (integer constant)
 *
 * @par Biblical Ground
 *      Genesis 1:2 — "And the earth was without form, and void"
 *
 * @par In Code
 *      - Null case, empty state, absence of value
 *      - NEUTRAL sign (neither positive nor negative)
 *      - INDEX_FIRST (array indices start here)
 *      - Additive identity (n + 0 = n)
 *
 * @note All "nothing" concepts ARE VOID. When you trace back far enough,
 *       you hit VOID. It is THE halt condition.
 *
 * @see CPISI_UNITY — the other anchor (multiplicative identity)
 */
#define CPISI_VOID              0

/**
 * @brief THE one — Deuteronomy 6:4 indivisible unity.
 *
 * "Hear, O Israel: The LORD our God is one LORD" — the indivisible unit
 * from which all counting builds. The foundation of increment.
 *
 * @return 1 (integer constant)
 *
 * @par Biblical Ground
 *      Deuteronomy 6:4 — "The LORD our God is one LORD"
 *
 * @par In Code
 *      - Multiplicative identity (n × 1 = n)
 *      - Increment unit (n + 1 = next)
 *      - The foundation all counting builds from
 *      - TRUE in boolean contexts
 *
 * @note All other values derive FROM VOID and UNITY:
 *       2 = UNITY + UNITY, 3 = 2 + UNITY, etc.
 *
 * @see CPISI_VOID — the other anchor (additive identity)
 */
#define CPISI_UNITY             1

/** @} */ /* end S.2.0.0 BOOTSTRAP */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2.0a PRIMITIVES — Irreducible Operations [PRIMITIVES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2_0a_PRIMITIVES S.2.0a PRIMITIVES — Irreducible Operations
 * @ingroup S2_0_PRELUDE
 * @brief    THE six atomic operations — irreducible instruction set for derivation.
 *           Every calculation in CPI-SI decomposes to these six primitives.
 *
 * Builds FROM: S.2.0.0 BOOTSTRAP (VOID and UNITY anchors)
 * Builds TO:   S.2.0b COMPOSITES (function macros built from these primitives)
 *
 * THE SIX PRIMITIVES:
 *   SUM(a, b)      — Addition:       a + b
 *   DIFF(a, b)     — Subtraction:    a - b
 *   PRODUCT(a, b)  — Multiplication: a × b
 *   QUOTIENT(a, b) — Division:       a ÷ b
 *   MODULO(a, b)   — Remainder:      a % b
 *   NEGATE(x)      — Sign flip:      -x
 *
 * CONNECTION TO ANCHORS (Identity Properties):
 *   SUM(n, VOID)      = n    VOID is additive identity
 *   DIFF(n, VOID)     = n    Subtracting VOID changes nothing
 *   PRODUCT(n, UNITY) = n    UNITY is multiplicative identity
 *   QUOTIENT(n, UNITY)= n    Dividing by UNITY changes nothing
 *   NEGATE(VOID)      = VOID Zero is its own negation
 *
 * DAR WITNESS: These ARE the atomic actions. Health pipeline traces back here.
 *              Witness records: operation + inputs + output = measurement.
 *
 * WHY WRAPPERS (not raw operators):
 *   - Consistent naming (CPISI_ prefix identifies our system)
 *   - Parenthesization (prevents operator precedence bugs)
 *   - Traceability (grep finds all uses — derivation audit trail)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Addition: a + b
 *
 * THE additive operation. Combines two values into their sum.
 *
 * @param a First operand
 * @param b Second operand
 * @return Sum of a and b
 *
 * @par Identity Property
 *      SUM(n, CPISI_VOID) = n — VOID is additive identity
 *
 * @par Derivation Examples
 *      - TERNARY = SUM(BINARY, UNITY) = 2 + 1 = 3
 *      - HARDPOINTS = SUM(3, 2) = 5
 *
 * @see CPISI_VOID — additive identity element
 * @see CPISI_DIFF — inverse operation
 */
#define CPISI_SUM(a, b)         ((a) + (b))

/**
 * @brief Subtraction: a - b
 *
 * THE difference operation. Removes b from a.
 *
 * @param a Minuend (value to subtract from)
 * @param b Subtrahend (value to subtract)
 * @return Difference of a and b
 *
 * @par Identity Property
 *      DIFF(n, CPISI_VOID) = n — subtracting VOID changes nothing
 *
 * @par Derivation Examples
 *      - MAX_OF(COUNT) = DIFF(COUNT, UNITY) = count - 1
 *      - BINARY = DIFF(TERNARY, UNITY) = 3 - 1 = 2
 *
 * @see CPISI_VOID — subtracting zero is identity
 * @see CPISI_SUM — inverse operation
 */
#define CPISI_DIFF(a, b)        ((a) - (b))

/**
 * @brief Multiplication: a × b
 *
 * THE multiplicative operation. Scales a by factor b.
 *
 * @param a First factor
 * @param b Second factor
 * @return Product of a and b
 *
 * @par Identity Property
 *      PRODUCT(n, CPISI_UNITY) = n — UNITY is multiplicative identity
 *
 * @par Derivation Examples
 *      - FACE = PRODUCT(TERNARY, TERNARY) = 3 × 3 = 9
 *      - TIMEOUT = PRODUCT(1000, 5) = 5000
 *
 * @see CPISI_UNITY — multiplicative identity element
 * @see CPISI_QUOTIENT — inverse operation
 */
#define CPISI_PRODUCT(a, b)     ((a) * (b))

/**
 * @brief Integer division: a ÷ b
 *
 * THE division operation. Determines how many times b fits in a.
 *
 * @param a Dividend (value to divide)
 * @param b Divisor (value to divide by — must be non-zero)
 * @return Quotient truncated toward zero
 *
 * @par Identity Property
 *      QUOTIENT(n, CPISI_UNITY) = n — dividing by UNITY changes nothing
 *
 * @par Derivation Examples
 *      - HALF(n) uses QUOTIENT(DIFF(n, 1), 2)
 *      - DEBOUNCE = QUOTIENT(1000, 10) = 100
 *
 * @warning Divisor must be non-zero. Division by zero is undefined behavior.
 *
 * @see CPISI_UNITY — dividing by one is identity
 * @see CPISI_PRODUCT — inverse operation
 */
#define CPISI_QUOTIENT(a, b)    ((a) / (b))

/**
 * @brief Modulo: a % b
 *
 * THE remainder operation. Returns what's left after division.
 *
 * @param a Dividend (value to divide)
 * @param b Divisor (value to divide by — must be non-zero)
 * @return Remainder of a divided by b
 *
 * @par Use Cases
 *      - Wrapping: circular buffer index = MODULO(i, SIZE)
 *      - Extraction: last digit = MODULO(n, 10)
 *      - Parity: is_even = MODULO(n, 2) == VOID
 *
 * @warning Divisor must be non-zero. Modulo by zero is undefined behavior.
 *
 * @see CPISI_QUOTIENT — related operation (quotient + remainder = division)
 */
#define CPISI_MODULO(a, b)      ((a) % (b))

/**
 * @brief Negation: -x
 *
 * THE sign inversion operation. Flips positive to negative and vice versa.
 *
 * @param x Value to negate
 * @return Negated value (-x)
 *
 * @par Identity Property
 *      NEGATE(CPISI_VOID) = CPISI_VOID — zero is its own negation
 *      NEGATE(NEGATE(x)) = x — double negation returns original
 *
 * @par Derivation Examples
 *      - BALANCED_MIN = NEGATE(CENTER) — symmetric range around zero
 *      - k-factor: -4 to +4 uses NEGATE for negative half
 *
 * @see CPISI_VOID — the self-inverse element for negation
 */
#define CPISI_NEGATE(x)         (-(x))

/** @} */ /* end S.2.0a PRIMITIVES */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2.0b COMPOSITES — Function Macros Built from Primitives [COMPOSITES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2_0b_COMPOSITES S.2.0b COMPOSITES — Function Macros
 * @ingroup S2_0_PRELUDE
 * @brief    Three consolidated function macros that combine PRIMITIVES into
 *           higher-level operations: POWER, SHIFT, BOUND.
 *
 * Builds FROM: S.2.0a PRIMITIVES (uses SUM, DIFF, PRODUCT, QUOTIENT)
 * Builds TO:   S.2.0c DERIVATION (patterns that apply these to derive bounds)
 *
 * THE THREE FUNCTION MACROS:
 *   POWER(n, exp)    — Exponentiation: n^exp (0-8), uses repeated PRODUCT
 *   SHIFT(n, dir)    — Binary scaling: dir<0 halves, dir>0 doubles
 *   BOUND(a, b, dir) — Value selection: dir<0 minimum, dir>0 maximum
 *
 * DIRECTION PARAMETER (ternary philosophy):
 *   dir < 0  →  toward VOID (smaller, half, minimum)
 *   dir > 0  →  toward growth (larger, double, maximum)
 *   This matches k-factor: -1 retreat, +1 advance
 *
 * ALIASES PROVIDED:
 *   POWER → POW_0 through POW_8 (layer-indexed), SQUARE_OF/CUBE_OF (geometric)
 *   SHIFT → HALF (dir=-1), DOUBLE (dir=+1)
 *   BOUND → MIN (dir=-1), MAX (dir=+1), CLAMP (composition)
 *
 * DAR WITNESS: Composites decompose to PRIMITIVES for measurement.
 *              POWER(3,2) = PRODUCT(3,3) = 9 — trace shows the chain.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */


/* ═══════════════════════════════════════════════════════════════════════════════
 * POWER — Exponentiation
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Exponentiation: n^exp
 *
 * THE function macro for all power calculations. Handles exponents 0-8 using
 * compile-time ternary expansion for the 9-layer Genesis architecture.
 *
 * @param n   Base value to raise (any integer expression)
 * @param exp Exponent from 0 to 8 (compile-time constant preferred)
 * @return n^exp as integer. Returns 0 if exp > 8.
 *
 * @par Decomposition to PRIMITIVES
 *      Each power level is repeated PRODUCT:
 *      - exp=0 → UNITY (1)
 *      - exp=1 → n (identity)
 *      - exp=2 → PRODUCT(n, n)
 *      - exp=3 → PRODUCT(n, PRODUCT(n, n))
 *
 * @par Connection to BOOTSTRAP
 *      - POWER(n, 0) = UNITY for any n
 *      - POWER(UNITY, exp) = UNITY for any exp
 *      - POWER(VOID, exp) = VOID for exp > 0
 *
 * @par Genesis Architecture
 *      Powers of 3 define layer sizes: 1, 3, 9, 27, 81, 243, 729, 2187, 6561
 *      FACE = 3² = 9 layers, each layer n has 3ⁿ positions
 *
 * @warning exp must be in range 0-8. Values > 8 return 0 (outside architecture).
 *
 * @see CPISI_PRODUCT — underlying primitive operation
 */
#define CPISI_POWER(n, exp) \
    ((exp) == 0 ? 1 : \
     (exp) == 1 ? (n) : \
     (exp) == 2 ? ((n) * (n)) : \
     (exp) == 3 ? ((n) * (n) * (n)) : \
     (exp) == 4 ? ((n) * (n) * (n) * (n)) : \
     (exp) == 5 ? ((n) * (n) * (n) * (n) * (n)) : \
     (exp) == 6 ? ((n) * (n) * (n) * (n) * (n) * (n)) : \
     (exp) == 7 ? ((n) * (n) * (n) * (n) * (n) * (n) * (n)) : \
     (exp) == 8 ? ((n) * (n) * (n) * (n) * (n) * (n) * (n) * (n)) : 0)

/** @name POW_0-8 — Layer-indexed aliases
 *  @brief Use when the exponent number is meaningful (iterating layers).
 *  Example: "layer 4 has 3⁴ positions" → POW_4(3) = 81
 *  @{ */
#define CPISI_POW_0(n)          CPISI_POWER((n), 0)  /**< n⁰ = 1 — UNITY (any n)       */
#define CPISI_POW_1(n)          CPISI_POWER((n), 1)  /**< n¹ = n — identity            */
#define CPISI_POW_2(n)          CPISI_POWER((n), 2)  /**< n² — square (FACE = 9)       */
#define CPISI_POW_3(n)          CPISI_POWER((n), 3)  /**< n³ — cube (CUBE = 27)        */
#define CPISI_POW_4(n)          CPISI_POWER((n), 4)  /**< n⁴ — tesseract (81)          */
#define CPISI_POW_5(n)          CPISI_POWER((n), 5)  /**< n⁵ — penteract (243)         */
#define CPISI_POW_6(n)          CPISI_POWER((n), 6)  /**< n⁶ — hexeract (729)          */
#define CPISI_POW_7(n)          CPISI_POWER((n), 7)  /**< n⁷ — hepteract (2187)        */
#define CPISI_POW_8(n)          CPISI_POWER((n), 8)  /**< n⁸ — octeract (6561)         */
/** @} */ /* end POW_0-8 */

/** @name Geometric aliases — Named powers for structural meaning
 *  @brief Use when the geometric interpretation matters.
 *  Example: "a cube has 3³ positions" → CUBE_OF(3) = 27
 *  @{ */
#define CPISI_SQUARE_OF(n)      CPISI_POWER((n), 2)  /**< n² — FACE = SQUARE_OF(3) = 9       */
#define CPISI_CUBE_OF(n)        CPISI_POWER((n), 3)  /**< n³ — CUBE = CUBE_OF(3) = 27        */
#define CPISI_FOURTH_OF(n)      CPISI_POWER((n), 4)  /**< n⁴ — TESSERACT = FOURTH_OF(3) = 81 */
#define CPISI_FIFTH_OF(n)       CPISI_POWER((n), 5)  /**< n⁵ — PENTERACT = FIFTH_OF(3) = 243 */
/** @} */ /* end Geometric aliases */


/* ═══════════════════════════════════════════════════════════════════════════════
 * SHIFT — Binary Scaling
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Binary scaling: halve or double based on direction.
 *
 * THE function macro for centering operations. The CPI-SI paradigm uses
 * centered coordinates (-4 to +4) rather than zero-based (0 to 8).
 *
 * @param n   Value to scale
 * @param dir Direction: negative = half (toward VOID), positive = double (growth)
 * @return Scaled value
 *
 * @par Decomposition to PRIMITIVES
 *      - HALF: QUOTIENT(DIFF(n, UNITY), 2) — (n-1)/2 for 0-based centering
 *      - DOUBLE: PRODUCT(n, 2)
 *
 * @par Why (n-1)/2 not n/2
 *      For 0-based arrays, center of 9 elements is index 4, not 4.5:
 *      - HALF(9) = (9-1)/2 = 4 — center of FACE (layers 0-8)
 *      - HALF(7) = (7-1)/2 = 3 — center of STATES (states 0-6)
 *
 * @see CPISI_HALF, CPISI_DOUBLE — readable aliases
 * @see CPISI_QUOTIENT, CPISI_DIFF, CPISI_PRODUCT — underlying primitives
 */
#define CPISI_SHIFT(n, dir) \
    ((dir) < 0 ? CPISI_QUOTIENT(CPISI_DIFF((n), 1), 2) : CPISI_PRODUCT((n), 2))

/** @name SHIFT aliases
 *  @brief Readable aliases for common SHIFT directions.
 *  @{ */

/**
 * @brief (n-1)/2 — center index for 0-based arrays.
 *
 * @param n Value to halve (typically a count)
 * @return Center index (integer division)
 *
 * @par Examples
 *      - HALF(9) = 4 — center of FACE (layers 0-8)
 *      - HALF(7) = 3 — center of STATES (states 0-6)
 *      - HALF(27) = 13 — center of CUBE (THE BRIDGE)
 */
#define CPISI_HALF(n)           CPISI_SHIFT((n), -1)

/**
 * @brief n × 2 — doubling operation.
 *
 * @param n Value to double
 * @return Doubled value
 *
 * @par Examples
 *      - DOUBLE(90) = 180 — ARCHIVE retention days
 *      - DOUBLE(POLL) = 2000ms — extended timeout
 */
#define CPISI_DOUBLE(n)         CPISI_SHIFT((n), +1)

/** @} */ /* end SHIFT aliases */


/* ═══════════════════════════════════════════════════════════════════════════════
 * BOUND — Directional Value Selection
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Directional value selection: minimum or maximum based on direction.
 *
 * THE function macro for range operations. Single macro handles both MIN and MAX
 * using direction parameter that matches our ternary philosophy.
 *
 * @param a   First value to compare
 * @param b   Second value to compare
 * @param dir Direction: negative = smaller (MIN), positive = larger (MAX)
 * @return Selected value (a or b)
 *
 * @par Direction Philosophy
 *      - dir < 0 → toward VOID (selecting minimum, smaller)
 *      - dir > 0 → toward growth (selecting maximum, larger)
 *      This matches k-factor: -1 retreat, +1 advance
 *
 * @see CPISI_MIN, CPISI_MAX, CPISI_CLAMP — readable aliases
 */
#define CPISI_BOUND(a, b, dir) \
    ((dir) < 0 ? (((a) < (b)) ? (a) : (b)) : (((a) > (b)) ? (a) : (b)))

/** @name BOUND aliases
 *  @brief Readable aliases for common BOUND operations.
 *  @{ */

/**
 * @brief Smaller of a, b.
 *
 * @param a First value
 * @param b Second value
 * @return The smaller value
 *
 * @par Example
 *      MIN(health, 100) caps health at 100
 */
#define CPISI_MIN(a, b)         CPISI_BOUND((a), (b), -1)

/**
 * @brief Larger of a, b.
 *
 * @param a First value
 * @param b Second value
 * @return The larger value
 *
 * @par Example
 *      MAX(health, 0) floors health at 0
 */
#define CPISI_MAX(a, b)         CPISI_BOUND((a), (b), +1)

/**
 * @brief Constrain x to range [lo, hi].
 *
 * CLAMP = MIN(MAX(x, lo), hi) — first floor to lo, then cap to hi.
 * Critical for health pipeline: ensures values stay in valid range.
 *
 * @param x  Value to constrain
 * @param lo Lower bound (inclusive)
 * @param hi Upper bound (inclusive)
 * @return x if in range, lo if below, hi if above
 *
 * @par Example
 *      CLAMP(raw_health, -100, 100) ensures balanced health stays valid
 */
#define CPISI_CLAMP(x, lo, hi)  CPISI_MIN(CPISI_MAX((x), (lo)), (hi))

/** @} */ /* end BOUND aliases */

/** @} */ /* end S.2.0b COMPOSITES */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2.0c DERIVATION — Patterns for BOUNDS [DERIVATION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2_0c_DERIVATION S.2.0c DERIVATION — Patterns for BOUNDS
 * @ingroup S2_0_PRELUDE
 * @brief    THE patterns that BOUNDS uses: index derivation, balanced mapping,
 *           range predicates, and compile-time assertions.
 *
 * Builds FROM: S.2.0b COMPOSITES (uses HALF, DIFF, SUM for derivations)
 * Builds TO:   S.2a BOUNDS (applies these patterns to derive all constants)
 *
 * INDEX BOUNDS (0-based range from count):
 *   MIN_OF(count)     — Minimum index: always VOID (0)
 *   MAX_OF(count)     — Maximum index: DIFF(count, UNITY)
 *   CENTER_OF(count)  — Center index: HALF(count)
 *
 * BALANCED BOUNDS (centered range from count):
 *   BAL_MIN_OF(count)    — Balanced minimum: NEGATE(CENTER_OF)
 *   BAL_MAX_OF(count)    — Balanced maximum: CENTER_OF(count)
 *   BAL_CENTER_OF(count) — Balanced center: always VOID (0)
 *
 * BALANCED MAPPING (index space ↔ balanced space):
 *   TO_BALANCED(index, count) — index - center (0-based to centered)
 *   TO_INDEX(balanced, count) — balanced + center (centered to 0-based)
 *
 * PREDICATES AND ENFORCEMENT:
 *   IN_RANGE(x, min, max)      — Boolean test: is x in [min, max]?
 *   STATIC_ASSERT(cond, msg)   — Compile-time tripwire: halt if false
 *
 * CONFIG-DRIVEN INTEGRATION:
 *   These patterns accept COUNT as parameter. When TOML specs define counts
 *   (TERNARY=3, FACE=9, CUBE=27), derivations become config-driven:
 *   MAX_OF(FACE) = 8, CENTER_OF(CUBE) = 13
 *
 * DAR WITNESS: DERIVATION is the measurement point. Input count →
 *              derived bounds → output value. Witness tracks the chain.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */


/* ═══════════════════════════════════════════════════════════════════════════════
 * INDEX BOUNDS — Derive min/max/center from count
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Minimum index: always VOID (0).
 *
 * THE floor for all index spaces. For any count N, the minimum valid index
 * is always 0. This is a semantic alias documenting "this is an index minimum."
 *
 * @param count Total number of elements (unused, for documentation/symmetry)
 * @return VOID (0) — always
 *
 * @par Connection to BOOTSTRAP
 *      Returns CPISI_VOID directly — THE zero anchor.
 *
 * @par Examples
 *      - MIN_OF(9) = 0   (layer indices 0-8)
 *      - MIN_OF(27) = 0  (cube indices 0-26)
 *      - MIN_OF(256) = 0 (byte values 0-255)
 *
 * @see CPISI_VOID — THE zero anchor
 */
#define CPISI_MIN_OF(count)     CPISI_VOID

/**
 * @brief Maximum index: COUNT - UNITY.
 *
 * THE ceiling for index spaces. For N elements, the maximum valid index is N-1.
 *
 * @param count Total number of elements
 * @return count - 1
 *
 * @par Decomposition to PRIMITIVES
 *      Uses DIFF(count, UNITY) — subtraction grounded in UNITY anchor.
 *
 * @par Examples
 *      - MAX_OF(9) = 8     (layer 8 is highest)
 *      - MAX_OF(27) = 26   (cube index 26 is highest)
 *      - MAX_OF(256) = 255 (byte max)
 *
 * @see CPISI_DIFF — underlying primitive
 * @see CPISI_UNITY — THE one anchor
 */
#define CPISI_MAX_OF(count)     CPISI_DIFF((count), CPISI_UNITY)

/**
 * @brief Center index: (COUNT - 1) / 2.
 *
 * THE middle of the index space. Uses HALF for correct 0-based centering.
 *
 * @param count Total number of elements
 * @return Center index (integer division)
 *
 * @par Decomposition to COMPOSITES
 *      Uses HALF(count) = (count-1)/2 for correct 0-based centering.
 *
 * @par Examples
 *      - CENTER_OF(9) = 4   (HEALTH layer — center of 9 layers)
 *      - CENTER_OF(27) = 13 (THE BRIDGE — center of cube)
 *      - CENTER_OF(7) = 3   (yashar — center of 7 states)
 *
 * @see CPISI_HALF — underlying composite operation
 */
#define CPISI_CENTER_OF(count)  CPISI_HALF((count))


/* ═══════════════════════════════════════════════════════════════════════════════
 * BALANCED MAPPING — Convert between index and balanced space
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Convert index to balanced: index - CENTER.
 *
 * Maps 0-based index to centered balanced space. Center becomes 0,
 * lower indices become negative, higher become positive.
 *
 * This IS how 3D cube navigation works: each axis maps 3 indices to (-1, 0, +1).
 * k-factor uses (-4 to +4) instead of (0 to 8).
 * Balanced coordinates have MEANING: negative = retreat, positive = advance.
 *
 * @param index 0-based index value
 * @param count Total number of elements (for computing center)
 * @return Balanced value in range (-CENTER to +CENTER)
 *
 * @par Decomposition to PRIMITIVES
 *      Uses DIFF(index, CENTER_OF(count)) — subtraction.
 *
 * @par Examples
 *      - TO_BALANCED(0, 9) = -4  (k-factor at layer 0)
 *      - TO_BALANCED(4, 9) = 0   (k-factor at HEALTH)
 *      - TO_BALANCED(8, 9) = +4  (k-factor at layer 8)
 *
 * @see CPISI_TO_INDEX — inverse operation
 * @see CPISI_DIFF — underlying primitive
 */
#define CPISI_TO_BALANCED(index, count)  CPISI_DIFF((index), CPISI_CENTER_OF(count))

/**
 * @brief Convert balanced to index: balanced + CENTER.
 *
 * Maps balanced value back to 0-based index. Used for array access
 * when you have a balanced coordinate.
 *
 * @param balanced Balanced value in range (-CENTER to +CENTER)
 * @param count Total number of elements (for computing center)
 * @return 0-based index value
 *
 * @par Decomposition to PRIMITIVES
 *      Uses SUM(balanced, CENTER_OF(count)) — addition.
 *
 * @par Examples
 *      - TO_INDEX(-4, 9) = 0  (array[0] from k=-4)
 *      - TO_INDEX(0, 9) = 4   (array[4] from k=0)
 *      - TO_INDEX(+4, 9) = 8  (array[8] from k=+4)
 *
 * @see CPISI_TO_BALANCED — inverse operation
 * @see CPISI_SUM — underlying primitive
 */
#define CPISI_TO_INDEX(balanced, count)  CPISI_SUM((balanced), CPISI_CENTER_OF(count))


/* ═══════════════════════════════════════════════════════════════════════════════
 * BALANCED BOUNDS — Derive balanced min/max/center from count
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * INDEX BOUNDS give 0-based ranges (0 to count-1).
 * BALANCED BOUNDS give centered ranges (-half to +half).
 *
 * Pattern: BAL_*_OF(count) mirrors *_OF(count) for balanced space.
 * Config-driven: TOML defines count, balanced bounds derive automatically.
 */

/**
 * @brief Balanced minimum: -CENTER_OF(count).
 *
 * THE floor for balanced spaces. Symmetric with BAL_MAX_OF around zero.
 * For count N with center C, balanced minimum is -C.
 *
 * @param count Total number of elements
 * @return Negated center: -(count-1)/2
 *
 * @par Decomposition to PRIMITIVES
 *      Uses NEGATE(CENTER_OF(count)) — negation grounded in UNITY.
 *
 * @par Examples
 *      - BAL_MIN_OF(3) = -1  (signs: -1, 0, +1)
 *      - BAL_MIN_OF(9) = -4  (layers: -4 to +4)
 *      - BAL_MIN_OF(27) = -13 (cube: -13 to +13)
 *
 * @see CPISI_BAL_MAX_OF — symmetric positive bound
 * @see CPISI_CENTER_OF — underlying derivation
 */
#define CPISI_BAL_MIN_OF(count)     CPISI_NEGATE(CPISI_CENTER_OF(count))

/**
 * @brief Balanced maximum: +CENTER_OF(count).
 *
 * THE ceiling for balanced spaces. Symmetric with BAL_MIN_OF around zero.
 * For count N with center C, balanced maximum is +C.
 *
 * @param count Total number of elements
 * @return Center value: (count-1)/2
 *
 * @par Decomposition to COMPOSITES
 *      Uses CENTER_OF(count) directly — center IS the max in balanced space.
 *
 * @par Examples
 *      - BAL_MAX_OF(3) = +1  (signs: -1, 0, +1)
 *      - BAL_MAX_OF(9) = +4  (layers: -4 to +4)
 *      - BAL_MAX_OF(27) = +13 (cube: -13 to +13)
 *
 * @see CPISI_BAL_MIN_OF — symmetric negative bound
 * @see CPISI_CENTER_OF — underlying derivation
 */
#define CPISI_BAL_MAX_OF(count)     CPISI_CENTER_OF(count)

/**
 * @brief Balanced center: always VOID (0).
 *
 * THE neutral point for all balanced spaces. By definition, balanced ranges
 * are symmetric around zero. Center is always 0 regardless of count.
 *
 * @param count Total number of elements (unused, for symmetry with INDEX pattern)
 * @return VOID (0) — always
 *
 * @par Connection to BOOTSTRAP
 *      Returns CPISI_VOID directly — center of balanced = absence of direction.
 *
 * @par Semantic Meaning
 *      - Signs: neutral (hold position)
 *      - Layers: HEALTH (yashar/even state)
 *      - k-factor: maintain (no movement)
 *
 * @see CPISI_VOID — THE zero anchor
 */
#define CPISI_BAL_CENTER_OF(count)  CPISI_VOID


/* ═══════════════════════════════════════════════════════════════════════════════
 * PREDICATES — Range testing
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Test if x is in closed range [min, max].
 *
 * THE predicate for range checking. Returns 1 (true) if min ≤ x ≤ max,
 * 0 (false) otherwise. Does NOT modify x — pure read operation.
 *
 * CLAMP enforces (modifies value); IN_RANGE tests (returns boolean).
 * This distinction matters for DAR Witness: read vs write operations.
 *
 * @param x   Value to test
 * @param min Lower bound (inclusive)
 * @param max Upper bound (inclusive)
 * @return 1 if in range, 0 if not
 *
 * @par Examples
 *      - IN_RANGE(layer, 0, 8)        → valid layer index?
 *      - IN_RANGE(health, -100, 100)  → valid balanced health?
 *      - IN_RANGE(code, 0, 255)       → valid error code?
 *
 * @see CPISI_CLAMP — enforces range (modifies value)
 */
#define CPISI_IN_RANGE(x, min, max)  (((x) >= (min)) && ((x) <= (max)))


/* ═══════════════════════════════════════════════════════════════════════════════
 * ASSERTIONS — Compile-time enforcement
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Compile-time assertion: halt compilation if condition false.
 *
 * THE tripwire mechanism for compile-time verification. Uses C11 _Static_assert
 * to stop compilation with a message if condition is not true.
 *
 * Zero runtime cost — checked only at compile time. Errors detected BEFORE
 * deployment. Self-documenting invariants in the code.
 *
 * @param cond Compile-time constant expression (must evaluate true)
 * @param msg  Error message displayed if condition is false
 *
 * @par Combined with IN_RANGE
 *      STATIC_ASSERT(IN_RANGE(LAYER_MAX, 0, 8), "layer out of bounds")
 *
 * @par Examples
 *      - STATIC_ASSERT(VOID == 0, "VOID must be zero")
 *      - STATIC_ASSERT(sizeof(uint8_t) == 1, "byte size mismatch")
 *      - STATIC_ASSERT(ERRORS + BRIDGE == 256, "bridge equation broken")
 *
 * @warning Condition MUST be compile-time constant. Variables won't work.
 *
 * @see CPISI_IN_RANGE — boolean predicate for use as condition
 */
#define CPISI_STATIC_ASSERT(cond, msg)  _Static_assert((cond), msg)

/** @} */ /* end S.2.0c DERIVATION */

/** @} */ /* end S.2.0 PRELUDE */


/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2a BOUNDS [BOUNDS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup S2a_BOUNDS S.2a BOUNDS — All Constants
 * @ingroup cpisi_defines
 * @brief    Complete constant catalog — 4 sections, flat organization.
 *
 * PURPOSE: Provide ALL constants used by the CPI-SI type system. Every value
 *          traces back to VOID(0) and UNITY(1) through a clear derivation chain.
 *
 * Builds FROM: S.2.0 PRELUDE (provides operations for derivation)
 * Builds TO:   S.2b MACROS (provides values for operations)
 *
 * CONNECTION TO PRELUDE:
 *   - Uses SUM, DIFF, PRODUCT, QUOTIENT, MODULO, NEGATE from PRIMITIVES
 *   - Uses POWER, SQUARE_OF, CUBE_OF, HALF, DOUBLE, MIN, MAX from COMPOSITES
 *   - Uses MIN_OF, MAX_OF, CENTER_OF, IN_RANGE from DERIVATION
 *
 * DERIVATION CHAIN:
 *   Genesis 1:1 → VOID(0) → UNITY(1) → 2,3,4 → 5,7,9,27 → 81,243 → 256
 *
 * CONTAINS:
 *   - S.2a.1 CORE    — Building blocks (2,3,4) and signs (-1,0,+1)
 *   - S.2a.2 POWERS  — Dimensions (9,27,81,243) and counts (5,7)
 *   - S.2a.3 INDICES — Bridge equation (13,256), layers (0-8), balanced (-4,+4)
 *   - S.2a.4 DOMAIN  — Structural geometry (buffers, coordinates, error bounds)
 *
 * INDEX BOUNDS PATTERN:
 *   - MIN_OF(count), MAX_OF(count), CENTER_OF(count) derive at call site
 *   - No pre-computed MIN/MAX constants cluttering the namespace
 *   - Self-documenting: "MAX_OF(FACE)" shows the derivation
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a.1 CORE [CORE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2a1_CORE S.2a.1 CORE — Building blocks and signs
 * @ingroup S2a_BOUNDS
 * @brief    First constants derived from VOID and UNITY. Foundation for all others.
 *
 * PURPOSE: Provide the core building blocks (2, 3, 4) and direction signs (-1, 0, +1)
 *          that ALL other BOUNDS constants depend on. Builds directly from BOOTSTRAP.
 *
 * Builds FROM: S.2.0.0 BOOTSTRAP (VOID=0, UNITY=1)
 * Builds TO:   S.2a.2 POWERS (uses TERNARY for dimensions)
 *
 * DERIVATION FROM BOOTSTRAP:
 *   - BINARY  = SUM(UNITY, UNITY)         = 1+1 = 2
 *   - TERNARY = SUM(BINARY, UNITY)        = 2+1 = 3
 *   - SQUARE  = PRODUCT(BINARY, BINARY)   = 2×2 = 4
 *   - NEGATIVE = NEGATE(UNITY)            = -1
 *   - NEUTRAL = VOID                      = 0
 *   - POSITIVE = UNITY                    = +1
 *
 * CONTAINS (inline groups):
 *   @name Building Blocks — First derivations (2, 3, 4)
 *   @name Signs           — Direction values (-1, 0, +1)
 *   @name Sign Bounds     — Count and range for sign validation
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Building Blocks — First derivations (2, 3, 4)
 *  @brief VOID(0) and UNITY(1) are anchors; these are the first derived values.
 *  @{ */

/**
 * @brief 2: duality — the first distinction.
 *
 * BINARY = UNITY + UNITY. The first value derived from anchors.
 * Represents distinction, separation, duality.
 *
 * @return 2 (integer constant)
 *
 * @par Derivation
 *      BINARY = SUM(UNITY, UNITY) = 1 + 1 = 2
 *
 * @par Used For
 *      - HARDPOINTS calculation: TERNARY + BINARY = 5
 *      - SQUARE calculation: BINARY × BINARY = 4
 *      - Pair counting, duality concepts
 *
 * @see CPISI_UNITY — the multiplicative identity (1)
 * @see CPISI_SUM — addition operation
 */
#define CPISI_BINARY            CPISI_SUM(CPISI_UNITY, CPISI_UNITY)

/**
 * @brief 3: THE ternary anchor — Ecclesiastes 4:12 "threefold cord".
 *
 * TERNARY = BINARY + UNITY. THE fundamental constant of CPI-SI.
 * Everything builds from 3: powers, cube navigation, direction logic.
 *
 * @return 3 (integer constant)
 *
 * @par Biblical Ground
 *      Ecclesiastes 4:12 — "a threefold cord is not quickly broken"
 *
 * @par Derivation
 *      TERNARY = SUM(BINARY, UNITY) = 2 + 1 = 3
 *
 * @par Used For
 *      - Powers: FACE=3², CUBE=3³, TESSERACT=3⁴, PENTERACT=3⁵
 *      - Cube navigation: 3 positions per axis
 *      - Direction logic: (-1, 0, +1)
 *
 * @see CPISI_BINARY — derived from UNITY
 * @see CPISI_UNITY — the one anchor
 */
#define CPISI_TERNARY           CPISI_SUM(CPISI_BINARY, CPISI_UNITY)

/**
 * @brief 4: first composite — 2² = BINARY × BINARY.
 *
 * SQUARE = BINARY × BINARY. First power-derived value.
 *
 * @return 4 (integer constant)
 *
 * @par Derivation
 *      SQUARE = PRODUCT(BINARY, BINARY) = 2 × 2 = 4
 *
 * @par Used For
 *      - Face center: CENTER_OF(FACE) = (9-1)/2 = 4
 *      - HEALTH layer position (layer 4)
 *      - Coordinate offsets in 2D projections
 *
 * @see CPISI_BINARY — the base for squaring
 * @see CPISI_PRODUCT — multiplication operation
 */
#define CPISI_SQUARE            CPISI_PRODUCT(CPISI_BINARY, CPISI_BINARY)

/** @} */ /* end S.2a.1 Building Blocks */

/** @name Signs — Direction values (-1, 0, +1)
 *  @brief Axis coordinates for 3D cube navigation and k-factor direction.
 *
 *  Signs use BALANCED range (centered on 0), not INDEX range (starting at 0).
 *  This is the ternary direction system: retreat / hold / advance.
 *  @{ */

/**
 * @brief -1: retreat toward foundation (shavar/broken).
 *
 * Negative direction in k-factor. Moving toward crisis/foundation.
 *
 * @return -1 (integer constant)
 *
 * @par In Hebrew States
 *      Direction toward shavar (broken) — k-factor decreasing
 *
 * @par In Cube Navigation
 *      -X, -Y, or -Z direction (toward origin)
 *
 * @note This is a primitive — not derived from other values.
 */
#define CPISI_NEGATIVE          CPISI_NEGATE(CPISI_UNITY)

/**
 * @brief 0: hold position (yashar/even) — IS VOID.
 *
 * Neutral direction in k-factor. Centered, stable.
 *
 * @return 0 (integer constant)
 *
 * @par Connection to BOOTSTRAP
 *      NEUTRAL IS VOID — absence of direction is absence of value.
 *
 * @par In Hebrew States
 *      yashar (even) — the centered state
 *
 * @see CPISI_VOID — THE zero anchor
 */
#define CPISI_NEUTRAL           CPISI_VOID

/**
 * @brief +1: advance toward growth (tov/perfect).
 *
 * Positive direction in k-factor. Moving toward completion.
 *
 * @return +1 (integer constant)
 *
 * @par Connection to BOOTSTRAP
 *      POSITIVE IS UNITY — the first step forward.
 *
 * @par In Hebrew States
 *      Direction toward tov (perfect) — k-factor increasing
 *
 * @par In Cube Navigation
 *      +X, +Y, or +Z direction (away from origin)
 *
 * @see CPISI_UNITY — THE one anchor
 */
#define CPISI_POSITIVE          CPISI_UNITY

/** @} */ /* end S.2a.1 Signs */

/** @name Sign Bounds — Count and balanced range for signs
 *  @brief Signs use BALANCED bounds (-1 to +1), derived via BAL_*_OF(SIGN_COUNT).
 *
 *  First application of the config-driven balanced bounds pattern.
 *  SIGN_COUNT defines the count; BAL_MIN_OF/MAX_OF/CENTER_OF derive the bounds.
 *  @{ */

/**
 * @brief 3: sign count — ternary logic gives 3 directions.
 *
 * @return 3 (integer constant)
 *
 * @par Derivation
 *      SIGN_COUNT = TERNARY. Signs exist because of ternary: negative, neutral, positive.
 *
 * @par Config-Driven
 *      TOML defines ternary = 3. SIGN_COUNT aliases it. Bounds derive from count.
 *
 * @see CPISI_TERNARY — THE ternary anchor
 */
#define CPISI_SIGN_COUNT        CPISI_TERNARY

/**
 * @brief -1: minimum sign — BAL_MIN_OF(SIGN_COUNT).
 *
 * @return -1 (integer constant)
 *
 * @par Derivation
 *      SIGN_MIN = BAL_MIN_OF(3) = -CENTER_OF(3) = -(3-1)/2 = -1
 *
 * @warning This is a BALANCED bound, not an index bound.
 *          Use TO_INDEX(sign, SIGN_COUNT) to convert to array index.
 *
 * @see CPISI_BAL_MIN_OF — the derivation operation
 * @see CPISI_NEGATIVE — equivalent primitive value
 */
#define CPISI_SIGN_MIN          CPISI_BAL_MIN_OF(CPISI_SIGN_COUNT)

/**
 * @brief +1: maximum sign — BAL_MAX_OF(SIGN_COUNT).
 *
 * @return +1 (integer constant)
 *
 * @par Derivation
 *      SIGN_MAX = BAL_MAX_OF(3) = CENTER_OF(3) = (3-1)/2 = +1
 *
 * @warning This is a BALANCED bound, not an index bound.
 *          Use TO_INDEX(sign, SIGN_COUNT) to convert to array index.
 *
 * @see CPISI_BAL_MAX_OF — the derivation operation
 * @see CPISI_POSITIVE — equivalent primitive value
 */
#define CPISI_SIGN_MAX          CPISI_BAL_MAX_OF(CPISI_SIGN_COUNT)

/**
 * @brief 0: center sign — BAL_CENTER_OF(SIGN_COUNT).
 *
 * @return 0 (integer constant)
 *
 * @par Derivation
 *      SIGN_CENTER = BAL_CENTER_OF(3) = VOID = 0 (always)
 *
 * @par Semantic Meaning
 *      Neutral k-factor. Hold position. yashar state.
 *
 * @see CPISI_BAL_CENTER_OF — the derivation operation
 * @see CPISI_NEUTRAL — equivalent primitive value
 */
#define CPISI_SIGN_CENTER       CPISI_BAL_CENTER_OF(CPISI_SIGN_COUNT)

/** @} */ /* end S.2a.1 Sign Bounds */

/** @name Ternary Bounds — Balanced range aliases for ternary logic
 *  @brief TERNARY_MIN/CENTER/MAX aliases to SIGN bounds for semantic clarity.
 *
 *  WHY SEPARATE: "Sign" implies direction (-/0/+). "Ternary" implies the
 *  balanced range itself. Same values, different semantic contexts.
 *  @{ */

/**
 * @brief -1: minimum ternary value — aliases SIGN_MIN.
 *
 * @return -1 (integer constant)
 *
 * @par Derivation
 *      TERNARY_MIN = SIGN_MIN = BAL_MIN_OF(3) = -1
 *
 * @par Semantic Meaning
 *      The negative extreme of balanced ternary. Used for health scaling,
 *      k-factor retreat, and state boundaries.
 *
 * @see CPISI_SIGN_MIN — the underlying value
 * @see CPISI_NEGATIVE — equivalent primitive
 */
#define CPISI_TERNARY_MIN       CPISI_SIGN_MIN

/**
 * @brief +1: maximum ternary value — aliases SIGN_MAX.
 *
 * @return +1 (integer constant)
 *
 * @par Derivation
 *      TERNARY_MAX = SIGN_MAX = BAL_MAX_OF(3) = +1
 *
 * @par Semantic Meaning
 *      The positive extreme of balanced ternary. Used for health scaling,
 *      k-factor advance, and state boundaries.
 *
 * @see CPISI_SIGN_MAX — the underlying value
 * @see CPISI_POSITIVE — equivalent primitive
 */
#define CPISI_TERNARY_MAX       CPISI_SIGN_MAX

/**
 * @brief 0: center ternary value — aliases SIGN_CENTER.
 *
 * @return 0 (integer constant)
 *
 * @par Derivation
 *      TERNARY_CENTER = SIGN_CENTER = BAL_CENTER_OF(3) = 0
 *
 * @par Semantic Meaning
 *      The neutral center of balanced ternary. Yashar state. Equilibrium.
 *      Neither positive nor negative — the point of balance.
 *
 * @see CPISI_SIGN_CENTER — the underlying value
 * @see CPISI_NEUTRAL — equivalent primitive
 */
#define CPISI_TERNARY_CENTER    CPISI_SIGN_CENTER

/** @} */ /* end S.2a.1 Ternary Bounds */

/** @} */ /* end S.2a.1 CORE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a.2 POWERS [POWERS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2a2_POWERS S.2a.2 POWERS — Dimensions and counts from CORE
 * @ingroup S2a_BOUNDS
 * @brief    DIMENSIONS (9, 27, 81, 243) and COUNTS (5, 7) derived from CORE values.
 *
 * PURPOSE: Apply power and sum operations to CORE values (BINARY, TERNARY).
 *          Dimensions define spatial structures. Counts define element quantities.
 *
 * Builds FROM: S.2a.1 CORE (BINARY=2, TERNARY=3)
 * Builds TO:   S.2a.3 INDICES (uses PENTERACT, CUBE for bridge equation)
 *
 * DERIVATION FROM CORE:
 *   - FACE      = SQUARE_OF(TERNARY)     = 3² = 9
 *   - CUBE      = CUBE_OF(TERNARY)       = 3³ = 27
 *   - TESSERACT = FOURTH_OF(TERNARY)     = 3⁴ = 81
 *   - PENTERACT = FIFTH_OF(TERNARY)      = 3⁵ = 243
 *   - HARDPOINTS = SUM(TERNARY, BINARY)  = 3+2 = 5
 *   - STATES     = SUM(HARDPOINTS, BINARY) = 5+2 = 7
 *
 * TWO VIEWS (both are used):
 *   - GEOMETRIC: "CUBE" = 27 positions in 3D mental construct
 *   - COUNTING:  "MODES" = 27 cognitive modes to choose from
 *
 * CONTAINS (inline groups):
 *   @name Dimensions — Powers of TERNARY (9, 27, 81, 243)
 *   @name Counts     — Element quantities (5, 7)
 *   @name Centers    — Pre-computed CENTER_OF values
 *   @name Aliases    — Semantic names (MODES, LAYERS)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Dimensions — Powers of TERNARY (9, 27, 81, 243)
 *  @brief Each power represents a dimensional structure in the mental construct.
 *
 *  3² = face (2D, 9)    3³ = cube (3D, 27)    3⁴ = tesseract (4D, 81)    3⁵ = penteract (5D, 243)
 *  @{ */

/**
 * @brief 9: 2D face (3²) — severity layers, 9-layer pyramid.
 *
 * @return 9 (integer constant)
 *
 * @par Derivation
 *      FACE = SQUARE_OF(TERNARY) = 3² = 9
 *
 * @par Used For
 *      - 9-layer pyramid architecture (KERNEL through APP)
 *      - Severity levels per layer
 *      - Mode count per cube face
 *
 * @see CPISI_TERNARY — the base (3)
 * @see CPISI_SQUARE_OF — the power operation
 */
#define CPISI_FACE              CPISI_SQUARE_OF(CPISI_TERNARY)

/**
 * @brief 27: 3D cube (3³) — THE mental construct, 27 cognitive positions.
 *
 * @return 27 (integer constant)
 *
 * @par Derivation
 *      CUBE = CUBE_OF(TERNARY) = 3³ = 27
 *
 * @par Used For
 *      - THE mental construct (27 positions)
 *      - Error codes per layer (27 offsets)
 *      - Context scale factor for buffer sizes
 *
 * @see CPISI_TERNARY — the base (3)
 * @see CPISI_CUBE_OF — the power operation
 */
#define CPISI_CUBE              CPISI_CUBE_OF(CPISI_TERNARY)

/**
 * @brief 81: 4D hypercube (3⁴) — tesseract, center layer power.
 *
 * @return 81 (integer constant)
 *
 * @par Derivation
 *      TESSERACT = FOURTH_OF(TERNARY) = 3⁴ = 81
 *
 * @par Used For
 *      - HEALTH layer buffer size (center of pyramid)
 *      - Extended precision calculations
 *
 * @see CPISI_TERNARY — the base (3)
 * @see CPISI_FOURTH_OF — the power operation
 */
#define CPISI_TESSERACT         CPISI_FOURTH_OF(CPISI_TERNARY)

/**
 * @brief 243: 5D penteract (3⁵) — error space, 243 layer error codes.
 *
 * @return 243 (integer constant)
 *
 * @par Derivation
 *      PENTERACT = FIFTH_OF(TERNARY) = 3⁵ = 243
 *
 * @par Used For
 *      - Total layer error codes (9 layers × 27 codes)
 *      - THE ternary component of BRIDGE equation (243+13=256)
 *
 * @see CPISI_TERNARY — the base (3)
 * @see CPISI_FIFTH_OF — the power operation
 * @see CPISI_BRIDGE — completes 243 to 256
 */
#define CPISI_PENTERACT         CPISI_FIFTH_OF(CPISI_TERNARY)

/** @} */ /* end S.2a.2 Dimensions */

/** @name Counts — Element quantities (5, 7)
 *  @brief COUNTS are "how many" (quantity). DIMENSIONS are "positions in space" (structure).
 *  @{ */

/**
 * @brief 5: diagnostic hardpoints (3+2) — 5 health check points.
 *
 * @return 5 (integer constant)
 *
 * @par Derivation
 *      HARDPOINTS = SUM(TERNARY, BINARY) = 3 + 2 = 5
 *
 * @par Used For
 *      - 5 diagnostic attachment points on mental construct
 *      - Buffer size tier count
 *
 * @see CPISI_TERNARY — the ternary anchor (3)
 * @see CPISI_BINARY — the duality value (2)
 * @see CPISI_SUM — addition operation
 */
#define CPISI_HARDPOINTS        CPISI_SUM(CPISI_TERNARY, CPISI_BINARY)

/**
 * @brief 7: Hebrew cognitive states (5+2) — shavar through tov.
 *
 * @return 7 (integer constant)
 *
 * @par Derivation
 *      STATES = SUM(HARDPOINTS, BINARY) = 5 + 2 = 7
 *
 * @par Hebrew States
 *      0: shavar (broken)    3: yashar (even)    6: tov (perfect)
 *      1: chaser (lacking)   4: tamim (sound)
 *      2: ratsah (wanting)   5: shalem (whole)
 *
 * @see CPISI_HARDPOINTS — the base (5)
 * @see CPISI_BINARY — the increment (2)
 */
#define CPISI_STATES            CPISI_SUM(CPISI_HARDPOINTS, CPISI_BINARY)

/** @} */ /* end S.2a.2 Counts */

/** @name Centers — Pre-computed CENTER_OF values
 *  @brief Pre-computed because used extensively. Most bounds use _OF macros at call site.
 *  @{ */

/**
 * @brief 4: face center (9-1)/2 — HEALTH layer position.
 *
 * @return 4 (integer constant)
 *
 * @par Derivation
 *      FACE_CENTER = CENTER_OF(FACE) = (9-1)/2 = 4
 *
 * @par Used For
 *      - HEALTH layer index (layer 4, center of 9)
 *      - k-factor calculation base
 *
 * @see CPISI_FACE — the count (9)
 * @see CPISI_CENTER_OF — centering operation
 */
#define CPISI_FACE_CENTER       CPISI_CENTER_OF(CPISI_FACE)

/**
 * @brief 13: cube center (27-1)/2 — THE center of the mental construct.
 *
 * @return 13 (integer constant)
 *
 * @par Derivation
 *      CUBE_CENTER = CENTER_OF(CUBE) = (27-1)/2 = 13
 *
 * @par Used For
 *      - THE central position of the 27-position mental construct
 *      - Balanced coordinate conversion: TO_BALANCED/TO_INDEX use this
 *      - The "centered self" in CPI-SI cognition
 *
 * @par Biblical Ground
 *      13 = "echad" (אֶחָד) = "one" in Hebrew gematria
 *      Unity at the center — the one position from which all others relate
 *
 * @see CPISI_CUBE — the count (27)
 * @see CPISI_CENTER_OF — centering operation
 */
#define CPISI_CUBE_CENTER       CPISI_CENTER_OF(CPISI_CUBE)

/**
 * @brief 13: base-27 half — aliases CUBE_CENTER.
 *
 * @return 13 (integer constant)
 *
 * @par Derivation
 *      BASE27_HALF = CUBE_CENTER = CENTER_OF(27) = (27-1)/2 = 13
 *
 * @par Semantic Meaning
 *      The center position in a 27-element space. Used for cube navigation,
 *      mental construct positioning, and the yashar (center) state location.
 *
 * @par Biblical Ground
 *      13 = "echad" (אֶחָד) = "one" in Hebrew gematria
 *      Unity at the center — the one position from which all others relate.
 *
 * @see CPISI_CUBE_CENTER — the underlying value
 * @see CPISI_CUBE — the count (27)
 */
#define CPISI_BASE27_HALF       CPISI_CUBE_CENTER

/**
 * @brief 2: hardpoints center (5-1)/2 — middle hardpoint.
 *
 * @return 2 (integer constant)
 *
 * @par Derivation
 *      HARDPOINTS_CENTER = CENTER_OF(HARDPOINTS) = (5-1)/2 = 2
 *
 * @see CPISI_HARDPOINTS — the count (5)
 * @see CPISI_CENTER_OF — centering operation
 */
#define CPISI_HARDPOINTS_CENTER CPISI_CENTER_OF(CPISI_HARDPOINTS)

/** @} */ /* end Centers */

/** @name Aliases — Semantic names (two views)
 *  @brief GEOMETRIC: "CUBE" = positions in space. COUNTING: "MODES" = items to count.
 *
 *  Both views are valid. Use whichever communicates intent better.
 *  @{ */

/**
 * @brief 2: alias for BINARY — "a pair" (counting view).
 * @see CPISI_BINARY
 */
#define CPISI_PAIR              CPISI_BINARY

/**
 * @brief 3: alias for TERNARY — "a trinity" (counting view).
 * @see CPISI_TERNARY
 */
#define CPISI_TRINITY           CPISI_TERNARY

/**
 * @brief 9: alias for FACE — "severity layers" (geometric view).
 * @see CPISI_FACE
 */
#define CPISI_LAYERS            CPISI_FACE

/**
 * @brief 27: alias for CUBE — "cognitive modes" (geometric view).
 * @see CPISI_CUBE
 */
#define CPISI_MODES             CPISI_CUBE

/**
 * @brief 9: alias for FACE — "layer count" (counting view).
 * @see CPISI_FACE
 */
#define CPISI_LAYER_COUNT       CPISI_FACE

/** @} */ /* end Aliases */

/** @} */ /* end S.2a.2 POWERS */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a.3 INDICES [INDICES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2a3_INDICES S.2a.3 INDICES — Bridge equation, layers, and balanced
 * @ingroup S2a_BOUNDS
 * @brief    Bridge (13, 256), layers (0-8), balanced (-4 to +4), power literals.
 *
 * PURPOSE: Provide the bridge equation connecting ternary to binary, named layer
 *          positions, balanced k-factor coordinates, and power scale literals.
 *
 * Builds FROM: S.2a.2 POWERS (PENTERACT=243, CUBE=27, FACE=9)
 * Builds TO:   S.2a.4 DOMAIN (domain-specific geometry uses these indices)
 *
 * THE BRIDGE EQUATION:
 *   PENTERACT(243) + BRIDGE(13) = BINARY_FIT(256)
 *         3⁵       +  (27-1)/2  =      2⁸
 *
 *   - 243 ternary error codes fit in 8 bits with room for 13 metadata slots
 *   - The "room" is exactly the cube center (HALF(27) = 13)
 *   - This enables ternary logic to operate within binary hardware
 *
 * THE 9-LAYER PYRAMID:
 *   Layer 8: APP         (+4)  ─┐
 *   Layer 7: ENGINE      (+3)   │ Upper half (k>0)
 *   Layer 6: STORAGE     (+2)   │
 *   Layer 5: NETWORK     (+1)  ─┘
 *   Layer 4: HEALTH      ( 0)  ←── CENTER (k=0)
 *   Layer 3: FRAMEWORK   (-1)  ─┐
 *   Layer 2: RUNTIME     (-2)   │ Lower half (k<0)
 *   Layer 1: SYSTEM      (-3)   │
 *   Layer 0: KERNEL      (-4)  ─┘
 *
 * CONTAINS (inline groups):
 *   @name Bridge Equation — 13, 243, 256, 255 (ternary↔binary)
 *   @name Index Patterns  — Universal sentinels (INDEX_FIRST, INDEX_INVALID)
 *   @name Layers          — Named positions (0-8 index space)
 *   @name Balanced        — k-factor coordinates (-4 to +4)
 *   @name Powers          — Ternary scale literals (3⁰ through 3⁸)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Bridge Equation — THE ternary↔binary connection
 *  @brief 243 (PENTERACT = 3⁵) + 13 (cube center) = 256 (byte).
 *
 *  This is THE mathematical foundation for error code architecture.
 *  @{ */

/**
 * @brief 13: THE bridge — cube center, metadata slot count.
 *
 * @return 13 (integer constant)
 *
 * @par Derivation
 *      BRIDGE = CENTER_OF(CUBE) = (27-1)/2 = 13
 *
 * @par Dual Meaning
 *      - Geometrically: center position in 3D cube (THE BRIDGE position)
 *      - Numerically: metadata slots (256-243 = 13)
 *
 * @note WHY 13: Not arbitrary — it's THE CENTER of the mental construct (CUBE).
 *
 * @see CPISI_CUBE — the 27-position mental construct
 * @see CPISI_CENTER_OF — centering operation
 */
#define CPISI_BRIDGE            CPISI_CENTER_OF(CPISI_CUBE)

/**
 * @brief 243: error code count (3⁵ = PENTERACT).
 *
 * @return 243 (integer constant)
 *
 * @par Derivation
 *      ERRORS = PENTERACT = 3⁵ = 243
 *      Also: 9 layers × 27 codes/layer = 243
 *
 * @par Error Space
 *      Codes 0-242 are layer errors (9×27=243 values)
 *      Leaves 243-255 for metadata (13 slots = BRIDGE)
 *
 * @see CPISI_PENTERACT — the ternary power (3⁵)
 */
#define CPISI_ERRORS            CPISI_PENTERACT

/**
 * @brief 256: THE byte — ternary + bridge = binary fit.
 *
 * @return 256 (integer constant)
 *
 * @par Derivation
 *      BINARY_FIT = SUM(ERRORS, BRIDGE) = 243 + 13 = 256 = 2⁸
 *
 * @par THE Bridge Equation
 *      This equation IS the bridge between ternary and binary systems.
 *      All error codes fit in a single byte.
 *
 * @see CPISI_ERRORS — ternary component (243)
 * @see CPISI_BRIDGE — completing component (13)
 * @see CPISI_SUM — addition operation
 */
#define CPISI_BINARY_FIT        CPISI_SUM(CPISI_ERRORS, CPISI_BRIDGE)

/**
 * @brief 255: maximum byte value (256 - 1).
 *
 * @return 255 (integer constant)
 *
 * @par Derivation
 *      BYTE_MAX = MAX_OF(BINARY_FIT) = 256 - 1 = 255
 *
 * @par Used For
 *      - Range checking for error codes
 *      - Maximum valid uint8_t value
 *
 * @see CPISI_BINARY_FIT — the byte size (256)
 * @see CPISI_MAX_OF — maximum index operation
 */
#define CPISI_BYTE_MAX          CPISI_MAX_OF(CPISI_BINARY_FIT)

/** @} */ /* end S.2a.3 Bridge Equation */

/** @name Index Patterns — Universal sentinel values
 *  @brief Semantic aliases for index contexts (first, invalid).
 *  @{ */

/**
 * @brief 0: first valid index — IS VOID.
 *
 * @return 0 (integer constant)
 *
 * @par Connection to BOOTSTRAP
 *      INDEX_FIRST IS VOID — arrays start at the zero anchor.
 *
 * @see CPISI_VOID — THE zero anchor
 */
#define CPISI_INDEX_FIRST       CPISI_VOID

/**
 * @brief -1: invalid index sentinel — IS NEGATIVE.
 *
 * @return -1 (integer constant)
 *
 * @par Connection to ANCHORS
 *      INDEX_INVALID IS NEGATIVE — retreat direction signals invalidity.
 *
 * @par Used For
 *      - "Not found" return value
 *      - Uninitialized index marker
 *
 * @see CPISI_NEGATIVE — the retreat direction
 */
#define CPISI_INDEX_INVALID     CPISI_NEGATIVE

/** @} */ /* end Index Patterns */

/** @name Layers — Named positions (0 to MAX_OF(FACE))
 *  @brief 9-layer pyramid: MIN=KERNEL(0), CENTER=HEALTH(4), MAX=APP(8).
 *
 *  LITERALS required for switch cases and array initializers.
 *  Count = FACE = 9. Bounds derived via *_OF(FACE).
 *  @{ */

#define CPISI_LAYER_KERNEL      0   /**< L0: Kernel — MIN, freestanding                */
#define CPISI_LAYER_SYSTEM      1   /**< L1: System — OS-level                         */
#define CPISI_LAYER_RUNTIME     2   /**< L2: Runtime — execution environment           */
#define CPISI_LAYER_FRAMEWORK   3   /**< L3: Framework — libraries                     */
#define CPISI_LAYER_HEALTH      4   /**< L4: Health — CENTER, diagnostic core          */
#define CPISI_LAYER_NETWORK     5   /**< L5: Network — communication                   */
#define CPISI_LAYER_STORAGE     6   /**< L6: Storage — persistence                     */
#define CPISI_LAYER_ENGINE      7   /**< L7: Engine — game/app engine                  */
#define CPISI_LAYER_APP         8   /**< L8: App — MAX, user-facing                    */

/**
 * @brief 0: KERNEL — minimum layer index.
 *
 * @par Derivation
 *      LAYER_MIN = MIN_OF(FACE) = 0
 *
 * @see CPISI_FACE — layer count (9)
 * @see CPISI_MIN_OF — minimum index operation
 */
#define CPISI_LAYER_MIN         CPISI_MIN_OF(CPISI_FACE)

/**
 * @brief 8: APP — maximum layer index.
 *
 * @par Derivation
 *      LAYER_MAX = MAX_OF(FACE) = 9-1 = 8
 *
 * @see CPISI_FACE — layer count (9)
 * @see CPISI_MAX_OF — maximum index operation
 */
#define CPISI_LAYER_MAX         CPISI_MAX_OF(CPISI_FACE)

/**
 * @brief 4: HEALTH — center layer index (THE anchor layer).
 *
 * @par Derivation
 *      LAYER_CENTER = CENTER_OF(FACE) = (9-1)/2 = 4
 *
 * @note This IS the HEALTH layer — the diagnostic core of the pyramid.
 *
 * @see CPISI_FACE — layer count (9)
 * @see CPISI_CENTER_OF — centering operation
 */
#define CPISI_LAYER_CENTER      CPISI_CENTER_OF(CPISI_FACE)

/** @} */ /* end S.2a.3 Layers */

/** @name Balanced — k-factor coordinate system (-4 to +4)
 *  @brief Balanced coordinates derived via BAL_*_OF(FACE).
 *
 *  Config-driven pattern: FACE=9 defines count, BAL_MIN_OF/MAX_OF/CENTER_OF derive bounds.
 *  Same pattern as SIGN bounds, different count.
 *
 *  - Negative = retreat toward foundation (shavar direction)
 *  - Zero = hold position (yashar, centered)
 *  - Positive = advance toward growth (tov direction)
 *  @{ */

/**
 * @brief -4: KERNEL in balanced — BAL_MIN_OF(FACE).
 *
 * @par Derivation
 *      BALANCED_MIN = BAL_MIN_OF(9) = -CENTER_OF(9) = -(9-1)/2 = -4
 *
 * @par Semantic Meaning
 *      Maximum retreat. KERNEL layer (layer 0) in balanced coordinates.
 *
 * @see CPISI_BAL_MIN_OF — the derivation operation
 * @see CPISI_LAYER_CENTER — equivalent to -LAYER_CENTER
 */
#define CPISI_BALANCED_MIN      CPISI_BAL_MIN_OF(CPISI_FACE)

/**
 * @brief +4: APP in balanced — BAL_MAX_OF(FACE).
 *
 * @par Derivation
 *      BALANCED_MAX = BAL_MAX_OF(9) = CENTER_OF(9) = (9-1)/2 = +4
 *
 * @par Semantic Meaning
 *      Maximum advance. APP layer (layer 8) in balanced coordinates.
 *
 * @see CPISI_BAL_MAX_OF — the derivation operation
 * @see CPISI_LAYER_CENTER — equivalent to LAYER_CENTER
 */
#define CPISI_BALANCED_MAX      CPISI_BAL_MAX_OF(CPISI_FACE)

/**
 * @brief 0: HEALTH anchor — BAL_CENTER_OF(FACE).
 *
 * @par Derivation
 *      BALANCED_CENTER = BAL_CENTER_OF(9) = VOID = 0 (always)
 *
 * @par Semantic Meaning
 *      Centered, neutral k-factor. HEALTH layer (layer 4) in balanced coordinates.
 *      yashar state — hold position.
 *
 * @see CPISI_BAL_CENTER_OF — the derivation operation
 * @see CPISI_VOID — THE zero anchor
 */
#define CPISI_BALANCED_CENTER   CPISI_BAL_CENTER_OF(CPISI_FACE)

/** @} */ /* end S.2a.3 Balanced */

/** @name Powers — Ternary scale at each layer (literals for array init)
 *  @brief POWER_Ln = 3^n. LITERALS required for C array initialization.
 *
 *  Array: { 1, 3, 9, 27, 81, 243, 729, 2187, 6561 }
 *
 *  @note Cannot use CUBE_OF(3) etc. in array initializers — need literal values.
 *  @{ */

#define CPISI_POWER_L0          1       /**< 3⁰ = 1 (UNITY)                   */
#define CPISI_POWER_L1          3       /**< 3¹ = 3 (TERNARY)                 */
#define CPISI_POWER_L2          9       /**< 3² = 9 (FACE)                    */
#define CPISI_POWER_L3          27      /**< 3³ = 27 (CUBE)                   */
#define CPISI_POWER_L4          81      /**< 3⁴ = 81 (TESSERACT) [CENTER]     */
#define CPISI_POWER_L5          243     /**< 3⁵ = 243 (PENTERACT)             */
#define CPISI_POWER_L6          729     /**< 3⁶ = 729                         */
#define CPISI_POWER_L7          2187    /**< 3⁷ = 2187                        */
#define CPISI_POWER_L8          6561    /**< 3⁸ = 6561 [MAX]                  */

/** @} */ /* end S.2a.3 Powers */

/** @} */ /* end S.2a.3 INDICES */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a.4 DOMAIN [DOMAIN]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2a4_DOMAIN S.2a.4 DOMAIN — Structural geometry and domain bounds
 * @ingroup S2a_BOUNDS
 * @brief    Buffers, coordinates, error geometry. Generic structure for all domains.
 *
 * PURPOSE: Provide structural bounds that ALL domains depend on. These are the
 *          scaffolding domain-specific values hang on.
 *
 * Builds FROM: S.2a.3 INDICES (layer indices, balanced coordinates)
 * Builds TO:   S.2b MACROS (operations on these domain bounds)
 *
 * CONNECTION TO INDICES:
 *   - Uses LAYERS for buffer tier assignments
 *   - Uses CUBE for coordinate navigation (OFFSET_COUNT = CUBE)
 *   - Uses FACE for layer-based geometry (LOCAL_* bounds)
 *
 * CONTAINS (inline groups):
 *   @name Counts      — Element counts (SIZE_COUNT, OFFSET_COUNT)
 *   @name Buffers     — Size tiers and scale (memory allocation)
 *   @name Coordinates — Cube navigation bases (BASE3, BASE9, BASE27)
 *   @name Error Geometry — Offset and local bounds
 *
 * NOTE: Domain-specific behavioral values are in domain files
 *       (types-health.h, types-error.h, types-config.h, etc.)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */


/** @name Counts — Structural element counts
 *  @brief SIZE_COUNT (buffer tiers), OFFSET_COUNT (cube positions).
 *
 *  Domain-specific counts are in domain files (types-health.h, types-error.h, etc.)
 *  @{ */

/**
 * @brief 5: buffer size tier count.
 *
 * @par Derivation
 *      SIZE_COUNT = HARDPOINTS = 5
 *
 * @see CPISI_HARDPOINTS — the count (5)
 */
#define CPISI_SIZE_COUNT            CPISI_HARDPOINTS

/**
 * @brief 27: cube offset count (3×3×3).
 *
 * @par Derivation
 *      OFFSET_COUNT = CUBE = 27
 *
 * @see CPISI_CUBE — the mental construct size
 */
#define CPISI_OFFSET_COUNT          CPISI_CUBE

/** @} */ /* end S.2a.4 Counts */

/** @name Buffers — Size tiers and scale (memory allocation)
 *  @brief SIZE = POWER_Ln × CONTEXT_SCALE. Override CONTEXT_SCALE to adjust.
 *  @{ */

/** @name Buffer Scale — THE context multiplier (overridable)
 *  @{ */

/**
 * @brief 27: default context scale factor for buffer sizes.
 *
 * @par Derivation
 *      CONTEXT_SCALE = CUBE = 27
 *
 * @note Override before including to adjust all buffer sizes.
 *
 * @see CPISI_CUBE — the mental construct size
 */
#ifndef CPISI_CONTEXT_SCALE
#define CPISI_CONTEXT_SCALE         CPISI_CUBE
#endif

/** @} */ /* end S.2a.4 Buffer Scale */

/** @name Buffer Sizes — Per-layer sizes (3^layer × CONTEXT_SCALE)
 *  @brief SIZE_LAYER = POWER_Ln × CONTEXT_SCALE. Config-driven: adjust CONTEXT_SCALE.
 *  @{ */

#define CPISI_SIZE_KERNEL       CPISI_PRODUCT(CPISI_POWER_L0, CPISI_CONTEXT_SCALE)  /**< L0: 3⁰×27 = 27     */
#define CPISI_SIZE_SYSTEM       CPISI_PRODUCT(CPISI_POWER_L1, CPISI_CONTEXT_SCALE)  /**< L1: 3¹×27 = 81     */
#define CPISI_SIZE_RUNTIME      CPISI_PRODUCT(CPISI_POWER_L2, CPISI_CONTEXT_SCALE)  /**< L2: 3²×27 = 243    */
#define CPISI_SIZE_FRAMEWORK    CPISI_PRODUCT(CPISI_POWER_L3, CPISI_CONTEXT_SCALE)  /**< L3: 3³×27 = 729    */
#define CPISI_SIZE_HEALTH       CPISI_PRODUCT(CPISI_POWER_L4, CPISI_CONTEXT_SCALE)  /**< L4: 3⁴×27 = 2187 [CENTER] */
#define CPISI_SIZE_NETWORK      CPISI_PRODUCT(CPISI_POWER_L5, CPISI_CONTEXT_SCALE)  /**< L5: 3⁵×27 = 6561   */
#define CPISI_SIZE_STORAGE      CPISI_PRODUCT(CPISI_POWER_L6, CPISI_CONTEXT_SCALE)  /**< L6: 3⁶×27 = 19683  */
#define CPISI_SIZE_ENGINE       CPISI_PRODUCT(CPISI_POWER_L7, CPISI_CONTEXT_SCALE)  /**< L7: 3⁷×27 = 59049  */
#define CPISI_SIZE_APP          CPISI_PRODUCT(CPISI_POWER_L8, CPISI_CONTEXT_SCALE)  /**< L8: 3⁸×27 = 177147 */

/** @} */ /* end S.2a.4 Buffer Sizes */

/** @name Buffer Bounds — min/max/center mapped from layer indices
 *  @brief SIZE at layer index → SIZE_MIN/MAX/CENTER. Layer-driven buffer sizing.
 *  @{ */

/**
 * @brief 27: minimum buffer size — SIZE at layer MIN_OF(FACE).
 *
 * @par Derivation
 *      SIZE_MIN = SIZE_KERNEL = SIZE at layer 0 = POWER_L0 × CONTEXT_SCALE = 1 × 27 = 27
 *
 * @par Layer Mapping
 *      MIN_OF(FACE) = 0 → LAYER_KERNEL → SIZE_KERNEL → SIZE_MIN
 */
#define CPISI_SIZE_MIN          CPISI_SIZE_KERNEL

/**
 * @brief 177147: maximum buffer size — SIZE at layer MAX_OF(FACE).
 *
 * @par Derivation
 *      SIZE_MAX = SIZE_APP = SIZE at layer 8 = POWER_L8 × CONTEXT_SCALE = 6561 × 27 = 177147
 *
 * @par Layer Mapping
 *      MAX_OF(FACE) = 8 → LAYER_APP → SIZE_APP → SIZE_MAX
 */
#define CPISI_SIZE_MAX          CPISI_SIZE_APP

/**
 * @brief 2187: center buffer size — SIZE at layer CENTER_OF(FACE).
 *
 * @par Derivation
 *      SIZE_CENTER = SIZE_HEALTH = SIZE at layer 4 = POWER_L4 × CONTEXT_SCALE = 81 × 27 = 2187
 *
 * @par Layer Mapping
 *      CENTER_OF(FACE) = 4 → LAYER_HEALTH → SIZE_HEALTH → SIZE_CENTER
 */
#define CPISI_SIZE_CENTER       CPISI_SIZE_HEALTH

/** @} */ /* end S.2a.4 Buffer Bounds */

/** @} */ /* end S.2a.4 Buffers */

/** @name Coordinates — Cube navigation bases (axis multipliers)
 *  @brief position = x + (y × BASE3) + (z × BASE9). 27-position cube.
 *  @{ */

/**
 * @brief 3: x-axis multiplier (column).
 * @par Derivation
 *      BASE3 = TERNARY = 3
 * @see CPISI_TERNARY
 */
#define CPISI_BASE3             CPISI_TERNARY

/**
 * @brief 9: y-axis multiplier (row).
 * @par Derivation
 *      BASE9 = FACE = 3² = 9
 * @see CPISI_FACE
 */
#define CPISI_BASE9             CPISI_FACE

/**
 * @brief 27: z-axis multiplier (layer).
 * @par Derivation
 *      BASE27 = CUBE = 3³ = 27
 * @see CPISI_CUBE
 */
#define CPISI_BASE27            CPISI_CUBE

/** @} */ /* end Coordinates */

/** @name Error Geometry — Offset and local bounds for error addressing
 *  @brief STRUCTURAL bounds for error codes. Semantics in types-error.h.
 *  @{ */

/** @name Offset Bounds — Cube position range (0-26)
 *  @brief Config-driven: *_OF(OFFSET_COUNT) derives bounds from count.
 *  @{ */

/**
 * @brief 0: minimum offset — MIN_OF(OFFSET_COUNT).
 * @par Derivation
 *      OFFSET_MIN = MIN_OF(27) = 0
 * @see CPISI_MIN_OF — the derivation operation
 */
#define CPISI_OFFSET_MIN        CPISI_MIN_OF(CPISI_OFFSET_COUNT)

/**
 * @brief 26: maximum offset — MAX_OF(OFFSET_COUNT).
 * @par Derivation
 *      OFFSET_MAX = MAX_OF(27) = 27 - 1 = 26
 * @see CPISI_MAX_OF — the derivation operation
 */
#define CPISI_OFFSET_MAX        CPISI_MAX_OF(CPISI_OFFSET_COUNT)

/**
 * @brief 13: center offset — CENTER_OF(OFFSET_COUNT). THE BRIDGE position.
 * @par Derivation
 *      OFFSET_CENTER = CENTER_OF(27) = (27-1)/2 = 13
 * @see CPISI_CENTER_OF — the derivation operation
 */
#define CPISI_OFFSET_CENTER     CPISI_CENTER_OF(CPISI_OFFSET_COUNT)

/** @} */ /* end Offset Bounds */

/** @name Local Bounds — Layer-local range (0-8)
 *  @brief Config-driven: *_OF(FACE) derives bounds from layer count.
 *  @{ */

/**
 * @brief 0: minimum local index — MIN_OF(FACE).
 * @par Derivation
 *      LOCAL_MIN = MIN_OF(9) = 0
 * @see CPISI_MIN_OF — the derivation operation
 */
#define CPISI_LOCAL_MIN         CPISI_MIN_OF(CPISI_FACE)

/**
 * @brief 8: maximum local index — MAX_OF(FACE).
 * @par Derivation
 *      LOCAL_MAX = MAX_OF(9) = 9 - 1 = 8
 * @see CPISI_MAX_OF — the derivation operation
 */
#define CPISI_LOCAL_MAX         CPISI_MAX_OF(CPISI_FACE)

/**
 * @brief 4: center local index — CENTER_OF(FACE). HEALTH position.
 * @par Derivation
 *      LOCAL_CENTER = CENTER_OF(9) = (9-1)/2 = 4
 * @see CPISI_CENTER_OF — the derivation operation
 */
#define CPISI_LOCAL_CENTER      CPISI_CENTER_OF(CPISI_FACE)

/** @} */ /* end Local Bounds */

/** @} */ /* end S.2a.4 Error Geometry */

/** @} */ /* end S.2a.4 DOMAIN */
/** @} */ /* end S.2a BOUNDS */


/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2b MACROS [MACROS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup S2b_MACROS S.2b MACROS — Operations on BOUNDS values
 * @ingroup cpisi_defines
 * @brief    Validation, conversion, and calculation macros.
 *
 * PURPOSE: Transform BOUNDS values. Where BOUNDS provides constants,
 *          MACROS provides the OPERATIONS on those constants.
 *
 * Builds FROM: S.2.0 PRELUDE (arithmetic), S.2a BOUNDS (values)
 * Builds TO:   S.2c VERIFY (compile-time assertions)
 *
 * CONNECTION TO PRELUDE:
 *   Arithmetic operations (SUM, DIFF, PRODUCT, IN_RANGE, MIN_OF, MAX_OF, etc.)
 *   are defined in S.2.0 PRELUDE because BOUNDS needs them for derivation.
 *   MACROS builds FROM both PRELUDE operations and BOUNDS values.
 *
 * CONNECTION TO BOUNDS:
 *   - Uses FACE, CUBE, HARDPOINTS, STATES for index ranges
 *   - Uses SIGN_MIN, SIGN_MAX for balanced validation
 *   - Uses layer constants for pyramid navigation
 *
 * WHY MACROS EXIST:
 *   - BOUNDS are static values (FACE = 9)
 *   - MACROS are dynamic operations (LAYER_VALID(l) = IN_RANGE(l, 0, 8))
 *   - Operations enable validation, conversion, and calculation
 *
 * STRUCTURE:
 *   S.2b.2 PRIMITIVES — Sign ↔ index conversion, basic validation
 *   S.2b.3 DERIVED    — Layer scaling, size calculation, debug ops
 *
 * NOTE: Domain-specific macros (health, timing, errors) are in domain files:
 *       health/health.h, config/config.h, error/error.h
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b.1 BRIDGE [BRIDGE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2b_1_BRIDGE S.2b.1 BRIDGE — Connection from PRELUDE and BOUNDS
 * @ingroup S2b_MACROS
 * @brief    Anchor section documenting how MACROS bridges PRELUDE and BOUNDS.
 *
 * PURPOSE: Establish the foundation for all MACRO operations. This section
 *          documents the architectural connection — PRELUDE provides arithmetic
 *          operations, BOUNDS provides constant values, MACROS combines them
 *          into domain-usable operations.
 *
 * Builds FROM: S.2.0 PRELUDE (arithmetic operations), S.2a BOUNDS (constant values)
 * Builds TO:   S.2b.2 PRIMITIVES (first concrete operations)
 *
 * THE BRIDGE PATTERN:
 *   PRELUDE operations + BOUNDS values = MACRO operations
 *
 *   Example derivation chain:
 *   - PRELUDE: IN_RANGE(x, min, max) — generic range check
 *   - BOUNDS:  SIGN_MIN(-1), SIGN_MAX(+1) — specific values
 *   - MACROS:  SIGN_VALID(s) = IN_RANGE(s, SIGN_MIN, SIGN_MAX) — concrete operation
 *
 * WHY THIS ANCHOR EXISTS:
 *   S.2a starts with S.2a.1 CORE (first derived constants from BOOTSTRAP).
 *   S.2c starts with S.2c.1 ANCHORS (verification of primitives).
 *   S.2b needs S.2b.1 BRIDGE to document its foundation — the connection point
 *   where PRELUDE arithmetic meets BOUNDS values to create usable operations.
 *
 * MACRO CATEGORIES (in subsequent sections):
 *   S.2b.2 PRIMITIVES — Sign/index conversion, basic validation (foundational)
 *   S.2b.3 DERIVED    — Layer scaling, size calculation, debug ops (builds on primitives)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* S.2b.1 is documentation-only — no code artifacts.
 * The bridge is conceptual: PRELUDE ops + BOUNDS values = MACROS.
 * Concrete operations begin in S.2b.2 PRIMITIVES. */

/** @} */ /* end S.2b.1 BRIDGE */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b.2 PRIMITIVES [PRIMITIVES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2b_2_PRIMITIVES S.2b.2 PRIMITIVES — Sign and index operations
 * @ingroup S2b_MACROS
 * @brief    GENERIC operations on signs and indices. Permanent in types.h.
 *
 * PURPOSE: Convert between representations and validate basic index types.
 *          Primitives are the most fundamental operations — ALL domains use
 *          signs for direction/trend/k-factor, ALL domains use indices for
 *          array access.
 *
 * Builds FROM: S.2a BOUNDS (provides SIGN_MIN/MAX, counts for validation)
 * Builds TO:   S.2b.3 DERIVED (layer/size/debug ops use these primitives)
 *
 * CONNECTION TO BOUNDS:
 *   - Uses VOID(0), UNITY(1) for offset arithmetic
 *   - Uses HARDPOINTS(5), STATES(7), FACE(9), CUBE(27) for index ranges
 *   - Uses SIGN_MIN(-1), SIGN_MAX(+1) for balanced validation
 *
 * FORMULA PATTERNS:
 *   - SIGN_INDEX: SUM(sign, UNITY) — balanced → unsigned
 *   - *_VALID: IN_RANGE(x, MIN_OF(COUNT), MAX_OF(COUNT))
 *
 * STRUCTURE:
 *   S.2b.2a SIGN OPS  — Sign ↔ index conversion, validation
 *   S.2b.2b INDEX OPS — Generic index validation for all counts
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2b.2a SIGN OPS — Sign ↔ index conversion
 *  @brief Convert between sign values (-1,0,+1) and array indices (0,1,2).
 *
 *  Signs use BALANCED bounds (-1 to +1), arrays use INDEX bounds (0 to 2).
 *  SIGN_INDEX bridges the two representations.
 *
 *  BOUNDS USED: SIGN_MIN(-1), SIGN_MAX(+1), UNITY(1)
 *
 *  CONVERSION TABLE:
 *  | Sign | Index | Meaning          |
 *  |:----:|:-----:|------------------|
 *  |  -1  |   0   | NEGATIVE/DECLINE |
 *  |   0  |   1   | NEUTRAL/STABLE   |
 *  |  +1  |   2   | POSITIVE/IMPROVE |
 *  @{ */

/**
 * @brief Convert sign (-1,0,+1) to array index (0,1,2).
 *
 * @param s Sign value: NEGATIVE(-1), NEUTRAL(0), or POSITIVE(+1)
 * @return Array index: 0, 1, or 2
 *
 * FORMULA: index = sign + UNITY = sign + 1
 *
 * DERIVATION:
 *   - Sign range is BALANCED: centered on 0, spans [-1, +1]
 *   - Array index must be UNSIGNED: starts at 0
 *   - Adding UNITY shifts: -1+1=0, 0+1=1, +1+1=2
 *
 * USES:
 *   - UNITY from S.2a.1 CORE (value = 1)
 *   - SUM from S.2.0a PRIMITIVES
 *
 * EXAMPLE:
 * ```c
 *   int8_t trend = CPISI_TREND_DECLINING;  // -1
 *   const char* names[] = {"declining", "stable", "improving"};
 *   printf("%s\n", names[CPISI_SIGN_INDEX(trend)]);  // "declining"
 * ```
 */
#define CPISI_SIGN_INDEX(s)     CPISI_SUM((s), CPISI_UNITY)

/**
 * @brief Validate sign value is in [-1, +1].
 *
 * @param s Value to validate
 * @return 1 if valid sign, 0 otherwise
 *
 * FORMULA: SIGN_MIN ≤ s ≤ SIGN_MAX
 *
 * USES:
 *   - SIGN_MIN (-1) from S.2a.4 SIGNS
 *   - SIGN_MAX (+1) from S.2a.4 SIGNS
 *   - IN_RANGE from S.2.0c DERIVATION
 *
 * WHY NOT USE _OF MACROS:
 *   Signs are BALANCED (centered on 0), not COUNT-based.
 *   MIN_OF/MAX_OF derive from counts; signs have explicit bounds.
 *
 * EXAMPLE:
 * ```c
 *   int8_t input = get_user_input();
 *   if (!CPISI_SIGN_VALID(input)) {
 *       return CPISI_ERROR_INVALID_SIGN;
 *   }
 * ```
 */
#define CPISI_SIGN_VALID(s)     CPISI_IN_RANGE((s), CPISI_SIGN_MIN, CPISI_SIGN_MAX)

/** @} */ /* end S.2b.2a SIGN OPS */

/** @name S.2b.2b INDEX OPS — Generic index validation
 *  @brief Validate indices for hardpoints, states, layers, modes.
 *
 *  These use _OF macros to derive bounds from counts, making them
 *  independent of specific values.
 *
 *  FORMULA PATTERN: *_VALID(x) = IN_RANGE(x, MIN_OF(COUNT), MAX_OF(COUNT))
 *
 *  VALIDATION TABLE:
 *  | Validator       | Count | Range   | Semantic             |
 *  |-----------------|:-----:|:-------:|----------------------|
 *  | HARDPOINT_VALID |   5   | [0, 4]  | 5 attachment points  |
 *  | STATE_VALID     |   7   | [0, 6]  | 7 Hebrew states      |
 *  | LAYER_VALID     |   9   | [0, 8]  | 9 pyramid layers     |
 *  | MODE_VALID      |  27   | [0, 26] | 27 cube positions    |
 *  @{ */

/**
 * @brief Validate hardpoint index [0, HARDPOINTS-1].
 *
 * @param h Index to validate
 * @return 1 if h in [0, 4], 0 otherwise
 *
 * BOUNDS: HARDPOINTS = 5 (S.2a.2 COUNTS)
 * RANGE:  [MIN_OF(5), MAX_OF(5)] = [0, 4]
 *
 * SEMANTIC: 5 hardpoints = 5 attachment points on the mental construct
 *   0: CENTER (anchor point)
 *   1-4: Cardinal directions (±X, ±Y in 2D projection)
 *
 * EXAMPLE:
 * ```c
 *   uint8_t hp = get_hardpoint_input();
 *   if (!CPISI_HARDPOINT_VALID(hp)) {
 *       return CPISI_ERROR_INVALID_HARDPOINT;
 *   }
 *   attach_component(construct, hp, component);
 * ```
 */
#define CPISI_HARDPOINT_VALID(h)  CPISI_IN_RANGE((h), CPISI_MIN_OF(CPISI_HARDPOINTS), CPISI_MAX_OF(CPISI_HARDPOINTS))

/**
 * @brief Validate state index [0, STATES-1].
 *
 * @param s Index to validate
 * @return 1 if s in [0, 6], 0 otherwise
 *
 * BOUNDS: STATES = 7 (S.2a.2 COUNTS)
 * RANGE:  [MIN_OF(7), MAX_OF(7)] = [0, 6]
 *
 * SEMANTIC: 7 Hebrew cognitive states (shavar→tov progression)
 *   0: SHAVAR  (-1.0) — broken/crisis
 *   1: CHASER  (-0.5) — lacking (k=-1)
 *   2: RATSAH  (-0.5) — wanting (k=+1)
 *   3: YASHAR  ( 0.0) — even/center
 *   4: TAMIM   (+0.5) — sound (k=-1)
 *   5: SHALEM  (+0.5) — whole (k=+1)
 *   6: TOV     (+1.0) — good/complete
 *
 * EXAMPLE:
 * ```c
 *   uint8_t state = get_current_state();
 *   if (!CPISI_STATE_VALID(state)) {
 *       return CPISI_ERROR_INVALID_STATE;
 *   }
 *   const char* name = state_names[state];
 * ```
 */
#define CPISI_STATE_VALID(s)      CPISI_IN_RANGE((s), CPISI_MIN_OF(CPISI_STATES), CPISI_MAX_OF(CPISI_STATES))

/**
 * @brief Validate layer index [0, FACE-1].
 *
 * @param l Index to validate
 * @return 1 if l in [0, 8], 0 otherwise
 *
 * BOUNDS: FACE = 9 (S.2a.3 DIMS = 3²)
 * RANGE:  [MIN_OF(9), MAX_OF(9)] = [0, 8]
 *
 * SEMANTIC: 9-layer pyramid (KERNEL→APP)
 *   0: KERNEL   (3⁰ = 1)    — innermost, highest priority
 *   1: MEMORY   (3¹ = 3)
 *   2: PROCESS  (3² = 9)
 *   3: STORAGE  (3³ = 27)
 *   4: HEALTH   (3⁴ = 81)   — CENTER layer
 *   5: NETWORK  (3⁵ = 243)
 *   6: SECURITY (3⁶ = 729)
 *   7: SERVICES (3⁷ = 2187)
 *   8: APP      (3⁸ = 6561) — outermost, lowest priority
 *
 * WHY FACE (9):
 *   A cube has 6 faces, but 9 = 3² gives us balanced ternary layers.
 *   FACE is the 2D projection of 3D ternary space: 3 × 3 = 9.
 *
 * EXAMPLE:
 * ```c
 *   uint8_t layer = get_error_layer(code);
 *   if (!CPISI_LAYER_VALID(layer)) {
 *       return CPISI_ERROR_INVALID_LAYER;
 *   }
 *   uint32_t power = CPISI_LAYER_POWERS[layer];
 * ```
 */
#define CPISI_LAYER_VALID(l)      CPISI_IN_RANGE((l), CPISI_MIN_OF(CPISI_FACE), CPISI_MAX_OF(CPISI_FACE))

/**
 * @brief Validate mode index [0, CUBE-1].
 *
 * @param m Index to validate
 * @return 1 if m in [0, 26], 0 otherwise
 *
 * BOUNDS: CUBE = 27 (S.2a.3 DIMS = 3³)
 * RANGE:  [MIN_OF(27), MAX_OF(27)] = [0, 26]
 *
 * SEMANTIC: 27 cognitive modes (3×3×3 cube positions)
 *   - 3 axes: X (action), Y (relation), Z (abstraction)
 *   - 3 values per axis: -1, 0, +1
 *   - 27 combinations = 27 unique cognitive positions
 *   - Center (mode 13) = YASHAR state (balanced)
 *
 * STRUCTURE:
 *   mode = x*9 + y*3 + z  (where x,y,z ∈ {0,1,2})
 *   Center: x=1, y=1, z=1 → mode = 9 + 3 + 1 = 13
 *
 * EXAMPLE:
 * ```c
 *   uint8_t mode = calculate_mode(x, y, z);
 *   if (!CPISI_MODE_VALID(mode)) {
 *       return CPISI_ERROR_INVALID_MODE;
 *   }
 *   apply_cognitive_mode(construct, mode);
 * ```
 */
#define CPISI_MODE_VALID(m)       CPISI_IN_RANGE((m), CPISI_MIN_OF(CPISI_CUBE), CPISI_MAX_OF(CPISI_CUBE))

/** @} */ /* end S.2b.2b INDEX OPS */

/** @} */ /* end S.2b.2 PRIMITIVES */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b.3 DERIVED [DERIVED]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2b_3_DERIVED S.2b.3 DERIVED — Layer, size, and debug operations
 * @ingroup S2b_MACROS
 * @brief    GENERIC operations on layers, sizes, debug levels. Permanent in types.h.
 *
 * PURPOSE: Operate on TYPE-LEVEL counts and structures. The 9-layer architecture
 *          is fundamental to ALL domains — errors, health, timing ALL scale by layer.
 *
 * Builds FROM: S.2b.2 PRIMITIVES (index validation patterns)
 * Builds TO:   Domain files (layer-scaled domain operations)
 *
 * CONNECTION TO PRIMITIVES:
 *   - Uses same IN_RANGE, MIN_OF, MAX_OF patterns
 *   - Extends validation to layer-indexed structures
 *   - Adds LAYER_SCALE for power-of-3 scaling
 *
 * KEY INSIGHT — LAYER SCALING:
 *   Every layer has a "weight" = 3^layer. This determines:
 *   - Buffer sizes: SIZE(layer) = 27 × 3^layer
 *   - Error ranges: Each layer holds CUBE (27) error codes
 *   - Health weights: Higher layers = more scope, less detail
 *
 * STRUCTURE:
 *   S.2b.3a LAYER OPS — Scaling and layer index validation
 *   S.2b.3b SIZE OPS  — Buffer size calculation and validation
 *   S.2b.3c DEBUG OPS — Debug level validation
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2b.3a LAYER OPS — Layer scaling (3^layer)
 *  @brief Scale values by layer power using LAYER_POWERS[] lookup.
 *
 *  USES: FACE(9) from BOUNDS, LAYER_POWERS[] from LOOKUP
 *
 *  POWER TABLE:
 *  | Layer | Power | Name      | Semantic           |
 *  |:-----:|------:|-----------|--------------------|
 *  |   0   |     1 | UNITY     | KERNEL (innermost) |
 *  |   4   |    81 | TESSERACT | HEALTH (center)    |
 *  |   8   |  6561 | (3⁸)      | APP (outermost)    |
 *  @{ */

/**
 * @brief Scale value by 3^layer using runtime lookup.
 *
 * @param base  Base value to scale
 * @param layer Layer index (0-8)
 * @return base × LAYER_POWERS[layer]
 *
 * FORMULA: result = base × 3^layer
 *
 * USES:
 *   - LAYER_POWERS[] from S.2d LOOKUP
 *   - PRODUCT from S.2.0a PRIMITIVES
 *
 * WHY RUNTIME LOOKUP:
 *   Compile-time power calculation needs recursion/ternary chains.
 *   Array lookup is O(1) and crystal clear.
 *
 * EXAMPLES:
 * ```c
 *   // Scale base unit by layer
 *   CPISI_LAYER_SCALE(1, 0) = 1 × 1 = 1        // KERNEL
 *   CPISI_LAYER_SCALE(1, 4) = 1 × 81 = 81     // HEALTH (center)
 *   CPISI_LAYER_SCALE(1, 8) = 1 × 6561 = 6561 // APP
 *
 *   // Scale context unit (27) by layer
 *   CPISI_LAYER_SCALE(27, 0) = 27 × 1 = 27     // MIN buffer
 *   CPISI_LAYER_SCALE(27, 4) = 27 × 81 = 2187  // CENTER buffer
 *   CPISI_LAYER_SCALE(27, 8) = 27 × 6561 = 177147 // MAX buffer
 * ```
 *
 * USE CASE — ROTATION SIZING:
 * ```c
 *   // Log rotation size scales by layer
 *   uint32_t rotation_kb = CPISI_LAYER_SCALE(CPISI_ROTATION_BASE_KB, layer);
 * ```
 */
#define CPISI_LAYER_SCALE(base, layer) \
    CPISI_PRODUCT((base), CPISI_LAYER_POWERS[(layer)])

/**
 * @brief Validate layer index [0, FACE-1].
 *
 * @param l Index to validate
 * @return 1 if l in [0, 8], 0 otherwise
 *
 * BOUNDS: FACE = 9 (S.2a.3 DIMS)
 * RANGE:  [MIN_OF(9), MAX_OF(9)] = [0, 8]
 *
 * ALIAS: Calls LAYER_VALID from S.2b.2b PRIMITIVES.
 *        Provided here for locality — when working with LAYER OPS,
 *        the validation is available without searching PRIMITIVES.
 *
 * DESIGN PRINCIPLE:
 *   Single source of truth (LAYER_VALID) + locality (this alias).
 *   Changes to layer validation logic only need updating in one place.
 */
#define CPISI_LAYER_INDEX_VALID(l)  CPISI_LAYER_VALID(l)  /**< Alias for LAYER_VALID — locality in LAYER OPS */

/** @} */ /* end S.2b.3a LAYER OPS */

/** @name S.2b.3b SIZE OPS — Buffer size calculation
 *  @brief SIZE = power × CONTEXT_SCALE (27).
 *
 *  USES: CONTEXT_SCALE(27), SIZE_MIN(27), SIZE_MAX(177147), SIZE_COUNT(5)
 *
 *  SIZE TABLE:
 *  | Layer | Power |   Size  | Use Case       |
 *  |:-----:|------:|--------:|----------------|
 *  |   0   |     1 |      27 | Minimal        |
 *  |   4   |    81 |    2187 | Center         |
 *  |   8   |  6561 |  177147 | Maximum        |
 *  @{ */

/**
 * @brief Validate size tier index [0, SIZE_COUNT-1].
 *
 * @param s Tier index to validate
 * @return 1 if s in [0, 4], 0 otherwise
 *
 * BOUNDS: SIZE_COUNT = 5 (S.2a.5 SIZES)
 * RANGE:  [MIN_OF(5), MAX_OF(5)] = [0, 4]
 *
 * NOTE: 5 tiers, not 9 — we use 5 CANONICAL sizes, not all 9 layer sizes.
 *       Tiers map to layers: 0→0, 1→2, 2→4, 3→6, 4→8 (even layers).
 */
#define CPISI_SIZE_VALID(s)     CPISI_IN_RANGE((s), CPISI_MIN_OF(CPISI_SIZE_COUNT), CPISI_MAX_OF(CPISI_SIZE_COUNT))

/**
 * @brief Calculate buffer size: SIZE = power × CONTEXT_SCALE.
 *
 * @param power Layer power value (1, 3, 9, 27, 81, 243, 729, 2187, 6561)
 * @return Buffer size in bytes (power × 27)
 *
 * FORMULA: SIZE = power × CONTEXT_SCALE = power × 27
 *
 * USES:
 *   - CONTEXT_SCALE (27) from S.2a.3 DIMS
 *   - PRODUCT from S.2.0a PRIMITIVES
 *
 * WHY power × 27:
 *   - power = 3^layer (how much this layer "weighs")
 *   - 27 = CUBE = minimum context unit
 *   - Together: buffer holds (layer weight) × (one context)
 *
 * EXAMPLES:
 * ```c
 *   CPISI_SIZE(1)    = 1 × 27    = 27      // KERNEL buffer
 *   CPISI_SIZE(81)   = 81 × 27   = 2187    // HEALTH buffer (center)
 *   CPISI_SIZE(6561) = 6561 × 27 = 177147  // APP buffer (max)
 * ```
 */
#define CPISI_SIZE(power)       CPISI_PRODUCT((power), CPISI_CONTEXT_SCALE)

/**
 * @brief Calculate size center: SIZE_HALF = (SIZE - 1) / 2.
 *
 * @param power Layer power value
 * @return Center index for balanced indexing
 *
 * FORMULA: SIZE_HALF = CENTER_OF(SIZE) = (SIZE - 1) / 2
 *
 * USES:
 *   - SIZE(power) defined above
 *   - CENTER_OF from S.2.0d DERIVATION
 *
 * WHY CENTER:
 *   For balanced iteration: indices run from -HALF to +HALF.
 *   Example: SIZE=27 → HALF=13 → indices -13..0..+13
 *
 * EXAMPLES:
 * ```c
 *   CPISI_SIZE_HALF(1)  = CENTER_OF(27)  = (27-1)/2  = 13
 *   CPISI_SIZE_HALF(81) = CENTER_OF(2187) = (2187-1)/2 = 1093
 * ```
 */
#define CPISI_SIZE_HALF(power)  CPISI_CENTER_OF(CPISI_SIZE((power)))

/**
 * @brief Convert layer index to buffer size.
 *
 * @param layer Layer index (0=KERNEL through 8=APP)
 * @return Buffer size for the layer (27 to 177147)
 *
 * FORMULA: SIZE_OF_LAYER = SIZE(LAYER_POWERS[layer])
 *                        = LAYER_POWERS[layer] × 27
 *
 * USES:
 *   - LAYER_POWERS[] from S.2d LOOKUP
 *   - SIZE(power) defined above
 *
 * SHORTHAND: Combines lookup + calculation into one call.
 *
 * EXAMPLES:
 * ```c
 *   CPISI_SIZE_OF_LAYER(0) = SIZE(1)    = 27      // KERNEL
 *   CPISI_SIZE_OF_LAYER(4) = SIZE(81)   = 2187    // HEALTH
 *   CPISI_SIZE_OF_LAYER(8) = SIZE(6561) = 177147  // APP
 * ```
 *
 * USE CASE:
 * ```c
 *   // Allocate buffer for specific layer
 *   uint8_t* buf = malloc(CPISI_SIZE_OF_LAYER(layer));
 * ```
 */
#define CPISI_SIZE_OF_LAYER(layer) CPISI_SIZE(CPISI_LAYER_POWERS[(layer)])

/**
 * @brief Validate buffer size is in valid range.
 *
 * @param n Size value to validate
 * @return 1 if SIZE_MIN ≤ n ≤ SIZE_MAX, 0 otherwise
 *
 * BOUNDS:
 *   SIZE_MIN = 27 (S.2a.5)     — minimum valid buffer
 *   SIZE_MAX = 177147 (S.2a.5) — maximum valid buffer
 *
 * WHY EXPLICIT BOUNDS (not _OF macros):
 *   Sizes are CALCULATED values, not indices. We check against
 *   the actual min/max SIZE values, not derived from a count.
 *
 * EXAMPLE:
 * ```c
 *   size_t requested = get_user_buffer_size();
 *   if (!CPISI_IS_SIZE(requested)) {
 *       return CPISI_ERROR_INVALID_SIZE;
 *   }
 * ```
 */
#define CPISI_IS_SIZE(n)        CPISI_IN_RANGE((n), CPISI_SIZE_MIN, CPISI_SIZE_MAX)

/** @} */ /* end S.2b.3b SIZE OPS */

/** @name S.2b.3c DEBUG OPS — Debug level validation
 *  @brief Validate debug level indices [0, DEBUG_COUNT-1].
 *
 *  USES: DEBUG_COUNT(9) = FACE — levels map 1:1 with pyramid layers
 *
 *  DEBUG LEVEL TABLE:
 *  | Level | Name    | Layer  | Output         |
 *  |:-----:|---------|--------|----------------|
 *  |   0   | SILENT  | KERNEL | Nothing        |
 *  |   4   | INFO    | HEALTH | Info+ (default)|
 *  |   8   | ALL     | APP    | Everything     |
 *  @{ */

/**
 * @brief Validate debug level [0, DEBUG_COUNT-1].
 *
 * @param d Level to validate
 * @return 1 if d in [0, 8], 0 otherwise
 *
 * BOUNDS: DEBUG_COUNT = 9 (S.2a.3 = FACE)
 * RANGE:  [MIN_OF(9), MAX_OF(9)] = [0, 8]
 *
 * WHY SAME AS FACE:
 *   Debug levels intentionally map 1:1 with pyramid layers.
 *   This isn't coincidence — it's architectural alignment.
 *   Layer 4 (HEALTH) = Level 4 (INFO) = balanced center.
 *
 * SEMANTIC NOTE:
 *   DEBUG_VALID validates the same range as LAYER_INDEX_VALID (both 0-8).
 *   This is intentional semantic naming — DEBUG_VALID for log contexts,
 *   LAYER_INDEX_VALID for architectural contexts. Same validation, clear intent.
 *
 * EXAMPLE:
 * ```c
 *   uint8_t level = get_config_debug_level();
 *   if (!CPISI_DEBUG_VALID(level)) {
 *       level = CPISI_DEBUG_INFO;  // Default to center
 *   }
 *   set_log_verbosity(level);
 * ```
 */
#define CPISI_DEBUG_VALID(d)    CPISI_IN_RANGE((d), CPISI_MIN_OF(CPISI_DEBUG_COUNT), CPISI_MAX_OF(CPISI_DEBUG_COUNT))

/** @} */ /* end S.2b.3c DEBUG OPS */

/** @} */ /* end S.2b.3 DERIVED */

/** @} */ /* end S.2b MACROS */


/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2c VERIFY [VERIFY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup S2c_VERIFY S.2c VERIFY — Compile-Time Tripwires
 * @ingroup cpisi_defines
 * @brief    Static assertions verifying BOUNDS values and MACRO derivations.
 *
 * PURPOSE: Catch configuration errors BEFORE runtime. Zero runtime cost.
 *          If any assertion fails, compilation stops with a descriptive message.
 *
 * Builds FROM: S.2a BOUNDS (values), S.2b MACROS (operations)
 * Builds TO:   S.2d LOOKUP (verified values ready for runtime tables)
 *
 * TRIPWIRE PHILOSOPHY:
 *   Each value is verified TWO ways:
 *   1. VALUE TRIPWIRE:        Assert the literal (e.g., VOID == 0)
 *   2. RELATIONSHIP TRIPWIRE: Assert the derivation (e.g., BINARY == UNITY+UNITY)
 *   BOTH together catches drift in either direction.
 *
 * STRUCTURE (mirrors S.2a BOUNDS):
 *   S.2c.1 ANCHORS  — Verify TIER 0-2 primitives (core, counts, dims, signs)
 *   S.2c.2 DERIVED  — Verify layer powers, bounds, balanced coordinates
 *   S.2c.3 BRIDGE   — Verify THE equation: 243 + 13 = 256
 *
 * NOTE: Domain-specific tripwires (health, timing, errors) in domain files.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c.1 ANCHORS [ANCHORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2c_1_ANCHORS S.2c.1 ANCHORS — Verify TIER 0-2 Building Blocks
 * @ingroup S2c_VERIFY
 * @brief    VALUE + RELATIONSHIP tripwires for primitives from S.2a.1.
 *
 * PURPOSE: Verify the anchors everything else stands on.
 *          If these are wrong, nothing works.
 *
 * Builds FROM: S.2a.1 ANCHORS
 * Builds TO:   S.2c.2 DERIVED
 *
 * DERIVATION CHAINS VERIFIED:
 *   CORE:  VOID(0) → UNITY(1) → BINARY(2) → TERNARY(3) → SQUARE(4)
 *   COUNT: TERNARY+BINARY=HARDPOINTS(5) → +BINARY=STATES(7)
 *   DIM:   3²=FACE(9) → 3³=CUBE(27) → 3⁴=TESSERACT(81) → 3⁵=PENTERACT(243)
 *   SIGN:  NEGATE(UNITY)=MIN(-1), VOID=CENTER(0), POSITIVE=MAX(+1)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2c.1a CORE — VOID, UNITY, BINARY, TERNARY, SQUARE
 *  @brief TIER 0 literals and derivation chain.
 *  @{ */

/* VALUE tripwires */
CPISI_STATIC_ASSERT(CPISI_VOID == 0,          "VOID must be 0 — Genesis 1:2 formless");
CPISI_STATIC_ASSERT(CPISI_UNITY == 1,         "UNITY must be 1 — Deut 6:4 THE ONE");
CPISI_STATIC_ASSERT(CPISI_BINARY == 2,        "BINARY must be 2 — duality");
CPISI_STATIC_ASSERT(CPISI_TERNARY == 3,       "TERNARY must be 3 — Eccl 4:12 THE anchor");
CPISI_STATIC_ASSERT(CPISI_SQUARE == 4,        "SQUARE must be 4 — first composite");

/* RELATIONSHIP tripwires */
CPISI_STATIC_ASSERT(CPISI_BINARY == CPISI_SUM(CPISI_UNITY, CPISI_UNITY),
                    "BINARY must equal UNITY + UNITY — 2 = 1+1");
CPISI_STATIC_ASSERT(CPISI_TERNARY == CPISI_SUM(CPISI_BINARY, CPISI_UNITY),
                    "TERNARY must equal BINARY + UNITY — 3 = 2+1");
CPISI_STATIC_ASSERT(CPISI_SQUARE == CPISI_SUM(CPISI_TERNARY, CPISI_UNITY),
                    "SQUARE must equal TERNARY + UNITY — 4 = 3+1");

/** @} */ /* end S.2c.1a CORE */

/** @name S.2c.1b COUNTS — HARDPOINTS(5), STATES(7)
 *  @brief TIER 1 counts and derivation chain.
 *  @{ */

/* VALUE tripwires */
CPISI_STATIC_ASSERT(CPISI_HARDPOINTS == 5,    "HARDPOINTS must be 5 — diagnostic points");
CPISI_STATIC_ASSERT(CPISI_STATES == 7,        "STATES must be 7 — Hebrew cognitive states");

/* RELATIONSHIP tripwires */
CPISI_STATIC_ASSERT(CPISI_HARDPOINTS == CPISI_SUM(CPISI_TERNARY, CPISI_BINARY),
                    "HARDPOINTS must equal TERNARY + BINARY — 5 = 3+2");
CPISI_STATIC_ASSERT(CPISI_STATES == CPISI_SUM(CPISI_HARDPOINTS, CPISI_BINARY),
                    "STATES must equal HARDPOINTS + BINARY — 7 = 5+2");

/** @} */ /* end S.2c.1b COUNTS */

/** @name S.2c.1c DIMENSIONS — FACE(9), CUBE(27), TESSERACT(81), PENTERACT(243)
 *  @brief TIER 2 dimensional powers and derivation chain.
 *  @{ */

/* VALUE tripwires */
CPISI_STATIC_ASSERT(CPISI_FACE == 9,          "FACE must be 9 — 2D face (3²)");
CPISI_STATIC_ASSERT(CPISI_CUBE == 27,         "CUBE must be 27 — 3D cube (3³)");
CPISI_STATIC_ASSERT(CPISI_TESSERACT == 81,    "TESSERACT must be 81 — 4D hypercube (3⁴)");
CPISI_STATIC_ASSERT(CPISI_PENTERACT == 243,   "PENTERACT must be 243 — 5D penteract (3⁵)");

/* RELATIONSHIP tripwires */
CPISI_STATIC_ASSERT(CPISI_FACE == CPISI_SQUARE_OF(CPISI_TERNARY),
                    "FACE must equal TERNARY² — 9 = 3²");
CPISI_STATIC_ASSERT(CPISI_CUBE == CPISI_CUBE_OF(CPISI_TERNARY),
                    "CUBE must equal TERNARY³ — 27 = 3³");
CPISI_STATIC_ASSERT(CPISI_TESSERACT == CPISI_FOURTH_OF(CPISI_TERNARY),
                    "TESSERACT must equal TERNARY⁴ — 81 = 3⁴");
CPISI_STATIC_ASSERT(CPISI_PENTERACT == CPISI_FIFTH_OF(CPISI_TERNARY),
                    "PENTERACT must equal TERNARY⁵ — 243 = 3⁵");

/** @} */ /* end S.2c.1c DIMENSIONS */

/** @name S.2c.1d SIGNS — MIN(-1), CENTER(0), MAX(+1), COUNT(3)
 *  @brief Ternary sign bounds and derivation chain.
 *  @{ */

/* VALUE tripwires */
CPISI_STATIC_ASSERT(CPISI_SIGN_MIN == -1,     "SIGN_MIN must be -1 — retreat direction");
CPISI_STATIC_ASSERT(CPISI_SIGN_MAX == +1,     "SIGN_MAX must be +1 — advance direction");
CPISI_STATIC_ASSERT(CPISI_SIGN_CENTER == 0,   "SIGN_CENTER must be 0 — maintain position");
CPISI_STATIC_ASSERT(CPISI_SIGN_COUNT == 3,    "SIGN_COUNT must be 3 — ternary directions");

/* RELATIONSHIP tripwires — verify config-driven BAL_*_OF derivation */
CPISI_STATIC_ASSERT(CPISI_SIGN_COUNT == CPISI_TERNARY,
                    "SIGN_COUNT must equal TERNARY — 3 signs from ternary logic");
CPISI_STATIC_ASSERT(CPISI_SIGN_MIN == CPISI_BAL_MIN_OF(CPISI_SIGN_COUNT),
                    "SIGN_MIN must equal BAL_MIN_OF(SIGN_COUNT) — config-driven");
CPISI_STATIC_ASSERT(CPISI_SIGN_MAX == CPISI_BAL_MAX_OF(CPISI_SIGN_COUNT),
                    "SIGN_MAX must equal BAL_MAX_OF(SIGN_COUNT) — config-driven");
CPISI_STATIC_ASSERT(CPISI_SIGN_CENTER == CPISI_BAL_CENTER_OF(CPISI_SIGN_COUNT),
                    "SIGN_CENTER must equal BAL_CENTER_OF(SIGN_COUNT) — config-driven");

/* EQUIVALENCE tripwires — verify values match primitive equivalents */
CPISI_STATIC_ASSERT(CPISI_SIGN_MIN == CPISI_NEGATIVE,
                    "SIGN_MIN must equal NEGATIVE — -1");
CPISI_STATIC_ASSERT(CPISI_SIGN_MAX == CPISI_POSITIVE,
                    "SIGN_MAX must equal POSITIVE — +1");
CPISI_STATIC_ASSERT(CPISI_SIGN_CENTER == CPISI_NEUTRAL,
                    "SIGN_CENTER must equal NEUTRAL — 0");

/** @} */ /* end S.2c.1d SIGNS */

/** @name S.2c.1e FORMULA — HALF uses (n-1)/2, not n/2
 *  @brief CRITICAL formula verification catching common bug.
 *
 *  THE BUG THIS CATCHES:
 *    HALF(n) MUST use (n-1)/2, NOT n/2.
 *    For n=4: (4-1)/2 = 1, but 4/2 = 2.
 *
 *  CENTER VALUES:
 *    HARDPOINTS → 2, STATES → 3, FACE → 4, CUBE → 13 (THE BRIDGE)
 *  @{ */

CPISI_STATIC_ASSERT(CPISI_HALF(4) == 1,
                    "HALF(4) must be 1 — catches n/2 vs (n-1)/2 error");
CPISI_STATIC_ASSERT(CPISI_CENTER_OF(CPISI_HARDPOINTS) == 2,
                    "CENTER_OF(HARDPOINTS) must be 2 — (5-1)/2");
CPISI_STATIC_ASSERT(CPISI_CENTER_OF(CPISI_STATES) == 3,
                    "CENTER_OF(STATES) must be 3 — yashar (even)");
CPISI_STATIC_ASSERT(CPISI_CENTER_OF(CPISI_FACE) == 4,
                    "CENTER_OF(FACE) must be 4 — HEALTH layer");
CPISI_STATIC_ASSERT(CPISI_CENTER_OF(CPISI_CUBE) == 13,
                    "CENTER_OF(CUBE) must be 13 — THE BRIDGE value");

/** @} */ /* end S.2c.1e FORMULA */

/** @name S.2c.1f BAL_*_OF — Balanced bound derivation operations
 *  @brief Verify BAL_MIN_OF, BAL_MAX_OF, BAL_CENTER_OF derive correctly from count.
 *
 *  CONFIG-DRIVEN PATTERN: Count → Bounds
 *    BAL_MIN_OF(count)    = -CENTER_OF(count)
 *    BAL_MAX_OF(count)    = +CENTER_OF(count)
 *    BAL_CENTER_OF(count) = 0 (always)
 *  @{ */

/* SIGN bounds from TERNARY(3) */
CPISI_STATIC_ASSERT(CPISI_BAL_MIN_OF(CPISI_TERNARY) == -1,
                    "BAL_MIN_OF(3) must be -1 — signs: -1 to +1");
CPISI_STATIC_ASSERT(CPISI_BAL_MAX_OF(CPISI_TERNARY) == 1,
                    "BAL_MAX_OF(3) must be +1 — signs: -1 to +1");
CPISI_STATIC_ASSERT(CPISI_BAL_CENTER_OF(CPISI_TERNARY) == 0,
                    "BAL_CENTER_OF(3) must be 0 — always centered");

/* LAYER bounds from FACE(9) */
CPISI_STATIC_ASSERT(CPISI_BAL_MIN_OF(CPISI_FACE) == -4,
                    "BAL_MIN_OF(9) must be -4 — layers: -4 to +4");
CPISI_STATIC_ASSERT(CPISI_BAL_MAX_OF(CPISI_FACE) == 4,
                    "BAL_MAX_OF(9) must be +4 — layers: -4 to +4");
CPISI_STATIC_ASSERT(CPISI_BAL_CENTER_OF(CPISI_FACE) == 0,
                    "BAL_CENTER_OF(9) must be 0 — always centered");

/* CUBE bounds from CUBE(27) — verification that pattern scales */
CPISI_STATIC_ASSERT(CPISI_BAL_MIN_OF(CPISI_CUBE) == -13,
                    "BAL_MIN_OF(27) must be -13 — cube: -13 to +13");
CPISI_STATIC_ASSERT(CPISI_BAL_MAX_OF(CPISI_CUBE) == 13,
                    "BAL_MAX_OF(27) must be +13 — cube: -13 to +13");

/** @} */ /* end S.2c.1f BAL_*_OF */

/** @} */ /* end S.2c.1 ANCHORS */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c.2 DERIVED [DERIVED]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2c_2_DERIVED S.2c.2 DERIVED — Verify Layer and Enum Bounds
 * @ingroup S2c_VERIFY
 * @brief    VALUE + RELATIONSHIP tripwires for derived values from S.2a.2.
 *
 * PURPOSE: Verify values that DERIVE from ANCHORS:
 *          Layer powers, layer bounds, balanced coordinates, enum counts.
 *
 * Builds FROM: S.2c.1 ANCHORS, S.2a.2 DERIVED
 * Builds TO:   S.2c.3 BRIDGE
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2c.2a POWERS — Layer powers 3⁰ through 3⁸
 *  @brief Verify layer power sequence.
 *  @{ */

/* VALUE tripwires — all 9 layers */
CPISI_STATIC_ASSERT(CPISI_POWER_L0 == 1,    "POWER_L0 must be 1 (3⁰)");
CPISI_STATIC_ASSERT(CPISI_POWER_L1 == 3,    "POWER_L1 must be 3 (3¹)");
CPISI_STATIC_ASSERT(CPISI_POWER_L2 == 9,    "POWER_L2 must be 9 (3²)");
CPISI_STATIC_ASSERT(CPISI_POWER_L3 == 27,   "POWER_L3 must be 27 (3³)");
CPISI_STATIC_ASSERT(CPISI_POWER_L4 == 81,   "POWER_L4 must be 81 (3⁴) — HEALTH center");
CPISI_STATIC_ASSERT(CPISI_POWER_L5 == 243,  "POWER_L5 must be 243 (3⁵) — PENTERACT");
CPISI_STATIC_ASSERT(CPISI_POWER_L6 == 729,  "POWER_L6 must be 729 (3⁶)");
CPISI_STATIC_ASSERT(CPISI_POWER_L7 == 2187, "POWER_L7 must be 2187 (3⁷)");
CPISI_STATIC_ASSERT(CPISI_POWER_L8 == 6561, "POWER_L8 must be 6561 (3⁸)");

/* RELATIONSHIP tripwires — derive from TERNARY using geometric macros */
CPISI_STATIC_ASSERT(CPISI_POWER_L2 == CPISI_SQUARE_OF(CPISI_TERNARY),
                    "POWER_L2 must equal TERNARY² — 9 = 3²");
CPISI_STATIC_ASSERT(CPISI_POWER_L3 == CPISI_CUBE_OF(CPISI_TERNARY),
                    "POWER_L3 must equal TERNARY³ — 27 = 3³");
CPISI_STATIC_ASSERT(CPISI_POWER_L4 == CPISI_FOURTH_OF(CPISI_TERNARY),
                    "POWER_L4 must equal TERNARY⁴ — 81 = 3⁴");
CPISI_STATIC_ASSERT(CPISI_POWER_L5 == CPISI_FIFTH_OF(CPISI_TERNARY),
                    "POWER_L5 must equal TERNARY⁵ — 243 = 3⁵");

/* EQUIVALENCE tripwires — connect POWER_L* to named dimension constants */
CPISI_STATIC_ASSERT(CPISI_POWER_L2 == CPISI_FACE,
                    "POWER_L2 must equal FACE — 9");
CPISI_STATIC_ASSERT(CPISI_POWER_L3 == CPISI_CUBE,
                    "POWER_L3 must equal CUBE — 27");
CPISI_STATIC_ASSERT(CPISI_POWER_L4 == CPISI_TESSERACT,
                    "POWER_L4 must equal TESSERACT — 81");
CPISI_STATIC_ASSERT(CPISI_POWER_L5 == CPISI_PENTERACT,
                    "POWER_L5 must equal PENTERACT — 243");

/** @} */ /* end S.2c.2a POWERS */

/** @name S.2c.2b COUNTS — Enum counts match primitives
 *  @brief Verify SIZE_COUNT, LAYER_COUNT connect to HARDPOINTS, LAYERS.
 *  @{ */

CPISI_STATIC_ASSERT(CPISI_SIZE_COUNT == CPISI_HARDPOINTS,
                    "SIZE_COUNT must equal HARDPOINTS (5)");
CPISI_STATIC_ASSERT(CPISI_LAYER_COUNT == CPISI_LAYERS,
                    "LAYER_COUNT must equal LAYERS (9)");
/* NOTE: DEBUG_COUNT/LOG_COUNT verification in types-log.h */

/** @} */ /* end S.2c.2b COUNTS */

/** @name S.2c.2c LAYER BOUNDS — 9-layer pyramid (0-8)
 *  @brief Verify MIN(0), CENTER(4), MAX(8) derive from FACE(9).
 *  @{ */

/* VALUE tripwires */
CPISI_STATIC_ASSERT(CPISI_LAYER_MIN == 0,
                    "LAYER_MIN must be 0 (KERNEL)");
CPISI_STATIC_ASSERT(CPISI_LAYER_MAX == 8,
                    "LAYER_MAX must be 8 (APP)");
CPISI_STATIC_ASSERT(CPISI_LAYER_CENTER == 4,
                    "LAYER_CENTER must be 4 (HEALTH)");

/* RELATIONSHIP tripwires */
CPISI_STATIC_ASSERT(CPISI_LAYER_MIN == CPISI_MIN_OF(CPISI_FACE),
                    "LAYER_MIN must equal MIN_OF(FACE)");
CPISI_STATIC_ASSERT(CPISI_LAYER_MAX == CPISI_MAX_OF(CPISI_FACE),
                    "LAYER_MAX must equal MAX_OF(FACE)");
CPISI_STATIC_ASSERT(CPISI_LAYER_CENTER == CPISI_CENTER_OF(CPISI_FACE),
                    "LAYER_CENTER must equal CENTER_OF(FACE)");

/** @} */ /* end S.2c.2c LAYER BOUNDS */

/** @name S.2c.2d BALANCED — k-factor coordinates (-4 to +4)
 *  @brief Verify balanced layer coordinates center on HEALTH.
 *  @{ */

/* VALUE tripwires */
CPISI_STATIC_ASSERT(CPISI_BALANCED_MIN == -4,
                    "BALANCED_MIN must be -4 (toward KERNEL)");
CPISI_STATIC_ASSERT(CPISI_BALANCED_MAX == 4,
                    "BALANCED_MAX must be +4 (toward APP)");
CPISI_STATIC_ASSERT(CPISI_BALANCED_CENTER == 0,
                    "BALANCED_CENTER must be 0 (HEALTH anchor)");

/* RELATIONSHIP tripwires — verify config-driven BAL_*_OF derivation */
CPISI_STATIC_ASSERT(CPISI_BALANCED_MIN == CPISI_BAL_MIN_OF(CPISI_FACE),
                    "BALANCED_MIN must equal BAL_MIN_OF(FACE) — config-driven");
CPISI_STATIC_ASSERT(CPISI_BALANCED_MAX == CPISI_BAL_MAX_OF(CPISI_FACE),
                    "BALANCED_MAX must equal BAL_MAX_OF(FACE) — config-driven");
CPISI_STATIC_ASSERT(CPISI_BALANCED_CENTER == CPISI_BAL_CENTER_OF(CPISI_FACE),
                    "BALANCED_CENTER must equal BAL_CENTER_OF(FACE) — config-driven");

/* EQUIVALENCE tripwires — verify values match layer-derived equivalents */
CPISI_STATIC_ASSERT(CPISI_BALANCED_MIN == CPISI_NEGATE(CPISI_LAYER_CENTER),
                    "BALANCED_MIN must equal -LAYER_CENTER — -4");
CPISI_STATIC_ASSERT(CPISI_BALANCED_MAX == CPISI_LAYER_CENTER,
                    "BALANCED_MAX must equal +LAYER_CENTER — +4");

/** @} */ /* end S.2c.2d BALANCED */

/** @} */ /* end S.2c.2 DERIVED */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c.3 BRIDGE [BRIDGE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2c_3_BRIDGE S.2c.3 BRIDGE — Verify THE Equation
 * @ingroup S2c_VERIFY
 * @brief    THE mathematical miracle: 243 + 13 = 256.
 *
 * PURPOSE: Verify the bridge between ternary mathematics and binary storage.
 *          This is the heart of the type system.
 *
 * Builds FROM: S.2c.1 ANCHORS (PENTERACT, CUBE), S.2c.2 DERIVED
 * Builds TO:   S.2d LOOKUP (verified values ready for runtime)
 *
 * THE EQUATION:
 *   PENTERACT(3⁵) + CUBE_CENTER = BINARY_FIT(2⁸)
 *           243   +     13      =    256
 *
 * WHY THIS MATTERS:
 *   243 layer error codes + 13 metadata slots = 256 = exactly one byte.
 *   Not coincidence — this is the mathematical foundation.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2c.3a VALUES — BRIDGE, ERRORS, BINARY_FIT, BYTE_MAX
 *  @brief VALUE tripwires for bridge equation components.
 *  @{ */

CPISI_STATIC_ASSERT(CPISI_BRIDGE == 13,
                    "BRIDGE must be 13 — THE ternary-binary connection");
CPISI_STATIC_ASSERT(CPISI_ERRORS == 243,
                    "ERRORS must be 243 — 3⁵ error code space");
CPISI_STATIC_ASSERT(CPISI_BINARY_FIT == 256,
                    "BINARY_FIT must be 256 — fits in uint8_t (2⁸)");
CPISI_STATIC_ASSERT(CPISI_BYTE_MAX == 255,
                    "BYTE_MAX must be 255 — maximum byte value");

/** @} */ /* end S.2c.3a VALUES */

/** @name S.2c.3b RELATIONSHIPS — THE equation and derivations
 *  @brief RELATIONSHIP tripwires proving the bridge equation.
 *  @{ */

CPISI_STATIC_ASSERT(CPISI_BRIDGE == CPISI_CENTER_OF(CPISI_CUBE),
                    "BRIDGE must equal CENTER_OF(CUBE) — 13 = (27-1)/2");
CPISI_STATIC_ASSERT(CPISI_ERRORS == CPISI_PRODUCT(CPISI_FACE, CPISI_CUBE),
                    "ERRORS must equal FACE × CUBE — 243 = 9×27");
CPISI_STATIC_ASSERT(CPISI_BINARY_FIT == CPISI_SUM(CPISI_ERRORS, CPISI_BRIDGE),
                    "BINARY_FIT must equal ERRORS + BRIDGE — 256 = 243+13");
CPISI_STATIC_ASSERT(CPISI_BYTE_MAX == CPISI_MAX_OF(CPISI_BINARY_FIT),
                    "BYTE_MAX must equal MAX_OF(BINARY_FIT) — 255 = 256-1");

/** @} */ /* end S.2c.3b RELATIONSHIPS */

/** @} */ /* end S.2c.3 BRIDGE */

/** @} */ /* end S.2c VERIFY */



/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2d LOOKUP [LOOKUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup S2d_LOOKUP S.2d LOOKUP — Runtime Lookup Tables
 * @ingroup cpisi_defines
 * @brief    Static arrays and accessor macros translating indices to data.
 *
 * PURPOSE:
 *   Provide the runtime data surface. Where BOUNDS defines values and
 *   VERIFY proves them, LOOKUP makes them available at runtime.
 *
 * Builds FROM: S.2a BOUNDS (values), S.2b MACROS (operations), S.2c VERIFY (assurance)
 * Builds TO:   S.2e TYPES (consumes tables via accessor macros)
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  LOOKUP TEMPLATE PATTERN — Reusable across ALL header files                 │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │                                                                             │
 * │  S.2d.X DOMAIN — [Domain-specific lookup group]                             │
 * │    ├── S.2d.X.a TABLES     — Static const arrays (the DATA)                 │
 * │    ├── S.2d.X.b ACCESSORS  — Read macros (HOW to get data)                  │
 * │    └── S.2d.X.c CONVERSION — Transform macros (bidirectional, optional)     │
 * │                                                                             │
 * │  LOOKUP FLOW: INDEX → TABLE[INDEX] → VALUE / NAME / SYMBOL                  │
 * │                                                                             │
 * │  DOCSTRING STANDARD (per item):                                             │
 * │    @brief           — One-line description                                  │
 * │    @return          — Type and value range                                  │
 * │    BOUNDS USED:     — Which S.2a constants define size/values               │
 * │    @par Derivation  — Formula or index→value mapping                        │
 * │    @par Semantic Meaning — Conceptual meaning in system                     │
 * │    @see             — Related tables, accessors, conversions                │
 * │                                                                             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * STRUCTURE (this file):
 *   S.2d.1 SIGNS  — Ternary sign system (3 entries)
 *     S.2d.1a TABLES     — SIGN_VALUES, SIGN_NAMES, SIGN_SYMBOLS
 *     S.2d.1b ACCESSORS  — cpisi_sign_value(), _name(), _symbol()
 *     S.2d.1c CONVERSION — SIGN_TO_INDEX(), INDEX_TO_SIGN()
 *
 *   S.2d.2 LAYERS — 9-layer pyramid (9 entries)
 *     S.2d.2a TABLES     — LAYER_POWERS, LAYER_NAMES, LAYER_ABBREVS, LAYER_SHORT,
 *                          LAYER_SIZES, SIZE_NAMES, LAYER_POSITION
 *     S.2d.2b ACCESSORS  — cpisi_layer_power(), _name(), _abbrev(), _short(), _size(), _position()
 *     S.2d.2c CONVERSION — [Reference: layer.h provides LAYER_TO_BAL(), BAL_TO_LAYER()]
 *
 * NOTE: Domain-specific tables (health, timing, errors) extracted to domain files.
 *       Each domain file follows the same S.2d.X.a/b/c template pattern.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */


/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d.1 SIGNS [SIGNS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2d_1_SIGNS S.2d.1 SIGNS — Ternary Sign Lookup Tables
 * @ingroup S2d_LOOKUP
 * @brief    Three parallel arrays for sign values, names, and symbols.
 *
 * PURPOSE:
 *   Runtime lookup for ternary sign system (-1, 0, +1). Signs are the
 *   foundational direction values: retreat, hold, advance.
 *
 * Builds FROM: S.2a.1 ANCHORS (NEGATIVE, NEUTRAL, POSITIVE, SIGN_COUNT)
 * Builds TO:   S.2e TYPES (CpisiSign enum)
 *
 * STRUCTURE (follows template pattern):
 *   S.2d.1a TABLES     — SIGN_VALUES, SIGN_NAMES, SIGN_SYMBOLS (3 parallel arrays)
 *   S.2d.1b ACCESSORS  — cpisi_sign_value(), _name(), _symbol()
 *   S.2d.1c CONVERSION — SIGN_TO_INDEX(), INDEX_TO_SIGN()
 *
 * ┌──────────────────────────────────────────────────────────────────────────┐
 * │  THE TERNARY SIGN — Foundational direction values                        │
 * ├──────────────────────────────────────────────────────────────────────────┤
 * │  INDEX   VALUE   NAME        SYMBOL   SEMANTIC MEANING                   │
 * │  ──────  ──────  ──────────  ──────   ─────────────────────────────────  │
 * │  [0]     -1      "negative"  "-"      Retreat toward foundation (k=-1)   │
 * │  [1]      0      "neutral"   "0"      Hold position, maintain (k=0)      │
 * │  [2]     +1      "positive"  "+"      Advance toward growth (k=+1)       │
 * └──────────────────────────────────────────────────────────────────────────┘
 *
 * CONVERSION FORMULA:
 *   Sign → Index: index = sign + UNITY    (CPISI_SIGN_TO_INDEX)
 *   Index → Sign: sign = index - UNITY    (CPISI_INDEX_TO_SIGN)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2d.1a TABLES — Value, name, and symbol arrays
 *  @brief Three parallel arrays indexed 0-2, providing sign data.
 *
 *  All three arrays share the same indexing scheme:
 *    [0] = negative/retreat, [1] = neutral/hold, [2] = positive/advance
 *  @{ */

/**
 * @brief Sign value lookup table — maps index (0-2) to sign value (-1, 0, +1).
 *
 * @return int array[3] — sign values: {-1, 0, +1}
 *
 * BOUNDS USED:
 *   - CPISI_SIGN_COUNT = 3 (from S.2a.1 ANCHORS) — array size
 *   - CPISI_NEGATIVE = -1 (from S.2a.1 ANCHORS) — retreat value
 *   - CPISI_NEUTRAL = 0 (from S.2a.1 ANCHORS) — hold value
 *   - CPISI_POSITIVE = +1 (from S.2a.1 ANCHORS) — advance value
 *
 * @par Derivation
 *      Index 0 → CPISI_NEGATIVE (-1) — retreat toward foundation
 *      Index 1 → CPISI_NEUTRAL (0)   — hold position, center
 *      Index 2 → CPISI_POSITIVE (+1) — advance toward growth
 *      Formula: Value = Index - UNITY (CPISI_INDEX_TO_SIGN macro)
 *
 * @par Semantic Meaning
 *      Sign values are the foundational ternary directions used throughout
 *      the system: k-factor direction, cognitive movement, Hebrew state
 *      progression. NEGATIVE retreats toward foundation (-1), NEUTRAL
 *      holds at center (0), POSITIVE advances toward growth (+1).
 *
 * @see cpisi_sign_value — accessor macro for this table
 * @see CPISI_INDEX_TO_SIGN — conversion macro (equivalent operation)
 * @see CPISI_SIGN_NAMES — parallel array with string names
 */
static const int CPISI_SIGN_VALUES[CPISI_SIGN_COUNT] = {
    CPISI_NEGATIVE,  /* [0] = -1: retreat direction  */
    CPISI_NEUTRAL,   /* [1] =  0: hold position      */
    CPISI_POSITIVE   /* [2] = +1: advance direction  */
};

/**
 * @brief Sign names for display — maps index (0-2) to descriptive string.
 *
 * @return const char* array[3] — sign names: {"negative", "neutral", "positive"}
 *
 * BOUNDS USED:
 *   - CPISI_SIGN_COUNT = 3 (from S.2a.1 ANCHORS) — array size
 *
 * @par Derivation
 *      Index 0 → "negative" — string representation of CPISI_NEGATIVE (-1)
 *      Index 1 → "neutral"  — string representation of CPISI_NEUTRAL (0)
 *      Index 2 → "positive" — string representation of CPISI_POSITIVE (+1)
 *
 * @par Semantic Meaning
 *      Human-readable names for ternary direction values. Used for logging,
 *      debugging, status display, and user-facing output. Parallel to
 *      CPISI_SIGN_VALUES — same indexing, string representation.
 *
 * EXAMPLE:
 *   printf("Direction: %s\n", cpisi_sign_name(idx)); // "negative", "neutral", "positive"
 *
 * @see cpisi_sign_name — accessor macro for this table
 * @see CPISI_SIGN_VALUES — parallel array with integer values
 * @see CPISI_SIGN_SYMBOLS — parallel array with single-char symbols
 */
static const char* const CPISI_SIGN_NAMES[CPISI_SIGN_COUNT] = {
    "negative",  /* [0] = CPISI_NEGATIVE: retreat  */
    "neutral",   /* [1] = CPISI_NEUTRAL:  hold     */
    "positive"   /* [2] = CPISI_POSITIVE: advance  */
};

/**
 * @brief Sign symbols for compact display — maps index (0-2) to single-char string.
 *
 * @return const char* array[3] — sign symbols: {"-", "0", "+"}
 *
 * BOUNDS USED:
 *   - CPISI_SIGN_COUNT = 3 (from S.2a.1 ANCHORS) — array size
 *
 * @par Derivation
 *      Index 0 → "-" — minus symbol for CPISI_NEGATIVE (-1)
 *      Index 1 → "0" — zero symbol for CPISI_NEUTRAL (0)
 *      Index 2 → "+" — plus symbol for CPISI_POSITIVE (+1)
 *
 * @par Semantic Meaning
 *      Single-character representation for compact output. Used in trit
 *      notation, status displays, and space-constrained output. Parallel
 *      to CPISI_SIGN_VALUES and CPISI_SIGN_NAMES — same indexing, compact form.
 *
 * EXAMPLE:
 *   printf("[%s]", cpisi_sign_symbol(idx)); // prints [-], [0], or [+]
 *
 * @see cpisi_sign_symbol — accessor macro for this table
 * @see CPISI_SIGN_VALUES — parallel array with integer values
 * @see CPISI_SIGN_NAMES — parallel array with full string names
 */
static const char* const CPISI_SIGN_SYMBOLS[CPISI_SIGN_COUNT] = {
    "-",  /* [0] = CPISI_NEGATIVE: minus  */
    "0",  /* [1] = CPISI_NEUTRAL:  zero   */
    "+"   /* [2] = CPISI_POSITIVE: plus   */
};

/** @} */ /* end S.2d.1a TABLES */

/** @name S.2d.1b ACCESSORS — Table access macros
 *  @brief Type-safe accessor macros for sign tables. No bounds check.
 *
 *  ACCESSOR PATTERN: cpisi_sign_X(index) → CPISI_SIGN_X[index]
 *  All accessors share the same index range [0, 2] and bounds responsibility.
 *  @{ */

/**
 * @brief Get sign value by index — INDEX (0-2) → VALUE (-1, 0, +1).
 *
 * @param i Index 0-2 (unchecked)
 * @return int — sign value: -1, 0, or +1
 *
 * BOUNDS:
 *   - INPUT:  i ∈ [0, CPISI_SIGN_COUNT-1] = [0, 2]
 *   - OUTPUT: CPISI_SIGN_VALUES[i] ∈ {-1, 0, +1}
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ i ≤ 2.
 *
 * @par Semantic Meaning
 *      Retrieves the balanced ternary value for a given index. Used when
 *      the index is known but the value is needed for arithmetic operations.
 *      Index 0 = retreat (-1), Index 1 = hold (0), Index 2 = advance (+1).
 *
 * @see CPISI_SIGN_VALUES — underlying lookup table
 * @see CPISI_INDEX_TO_SIGN — equivalent conversion (computed, not table lookup)
 * @see cpisi_sign_name — parallel accessor for string name
 */
#define cpisi_sign_value(i)   (CPISI_SIGN_VALUES[(i)])

/**
 * @brief Get sign name by index — INDEX (0-2) → NAME string.
 *
 * @param i Index 0-2 (unchecked)
 * @return const char* — sign name: "negative", "neutral", or "positive"
 *
 * BOUNDS:
 *   - INPUT:  i ∈ [0, CPISI_SIGN_COUNT-1] = [0, 2]
 *   - OUTPUT: CPISI_SIGN_NAMES[i] ∈ {"negative", "neutral", "positive"}
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ i ≤ 2.
 *
 * @par Semantic Meaning
 *      Retrieves human-readable name for sign index. Used for logging,
 *      debugging, status display, and user-facing output where full
 *      descriptive text is appropriate.
 *
 * @see CPISI_SIGN_NAMES — underlying lookup table
 * @see cpisi_sign_value — parallel accessor for integer value
 * @see cpisi_sign_symbol — parallel accessor for compact symbol
 */
#define cpisi_sign_name(i)    (CPISI_SIGN_NAMES[(i)])

/**
 * @brief Get sign symbol by index — INDEX (0-2) → SYMBOL string.
 *
 * @param i Index 0-2 (unchecked)
 * @return const char* — sign symbol: "-", "0", or "+"
 *
 * BOUNDS:
 *   - INPUT:  i ∈ [0, CPISI_SIGN_COUNT-1] = [0, 2]
 *   - OUTPUT: CPISI_SIGN_SYMBOLS[i] ∈ {"-", "0", "+"}
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ i ≤ 2.
 *
 * @par Semantic Meaning
 *      Retrieves single-character symbol for sign index. Used for compact
 *      output, trit notation, status displays, and space-constrained contexts
 *      where brevity is required.
 *
 * @see CPISI_SIGN_SYMBOLS — underlying lookup table
 * @see cpisi_sign_name — parallel accessor for full string name
 * @see cpisi_sign_value — parallel accessor for integer value
 */
#define cpisi_sign_symbol(i)  (CPISI_SIGN_SYMBOLS[(i)])

/** @} */ /* end S.2d.1b ACCESSORS */

/** @name S.2d.1c CONVERSION — Sign ↔ index bidirectional transforms
 *  @brief Bidirectional conversion using CPISI_SUM/CPISI_DIFF with CPISI_UNITY.
 *
 *  CONVERSION PATTERN: Balanced ↔ Index uses UNITY (1) as offset.
 *    Sign → Index: add UNITY (shift from [-1,+1] to [0,2])
 *    Index → Sign: subtract UNITY (shift from [0,2] to [-1,+1])
 *  @{ */

/**
 * @brief Convert sign value (-1,0,+1) to array index (0,1,2).
 *
 * @param s Sign value: -1, 0, or +1
 * @return int — array index: 0, 1, or 2
 *
 * BOUNDS:
 *   - INPUT:  s ∈ {-1, 0, +1} (CPISI_NEGATIVE, CPISI_NEUTRAL, CPISI_POSITIVE)
 *   - OUTPUT: index ∈ {0, 1, 2}
 *
 * FORMULA: index = sign + UNITY
 *   Uses CPISI_SUM(s, CPISI_UNITY) from S.2b MACROS
 *
 * @par Derivation
 *      SIGN_TO_INDEX(-1) = -1 + 1 = 0
 *      SIGN_TO_INDEX(0)  =  0 + 1 = 1
 *      SIGN_TO_INDEX(+1) = +1 + 1 = 2
 *
 * @par Semantic Meaning
 *      Converts balanced ternary sign value to zero-based array index.
 *      Used when a sign value (k-factor, direction, trend) must index
 *      into a lookup table. The inverse of CPISI_INDEX_TO_SIGN.
 *
 * @see CPISI_INDEX_TO_SIGN — inverse operation
 * @see CPISI_SUM — underlying arithmetic macro
 * @see CPISI_UNITY — offset value (1)
 */
#define CPISI_SIGN_TO_INDEX(s)    CPISI_SUM((s), CPISI_UNITY)

/**
 * @brief Convert array index (0,1,2) to sign value (-1,0,+1).
 *
 * @param i Index: 0, 1, or 2
 * @return int — sign value: -1, 0, or +1
 *
 * BOUNDS:
 *   - INPUT:  i ∈ {0, 1, 2}
 *   - OUTPUT: sign ∈ {-1, 0, +1} (CPISI_NEGATIVE, CPISI_NEUTRAL, CPISI_POSITIVE)
 *
 * FORMULA: sign = index - UNITY
 *   Uses CPISI_DIFF(i, CPISI_UNITY) from S.2b MACROS
 *
 * @par Derivation
 *      INDEX_TO_SIGN(0) = 0 - 1 = -1
 *      INDEX_TO_SIGN(1) = 1 - 1 =  0
 *      INDEX_TO_SIGN(2) = 2 - 1 = +1
 *
 * @par Semantic Meaning
 *      Converts zero-based array index to balanced ternary sign value.
 *      Used when iterating through sign tables and the actual sign value
 *      is needed for arithmetic. Equivalent to cpisi_sign_value() but
 *      computed rather than table lookup.
 *
 * @see CPISI_SIGN_TO_INDEX — inverse operation
 * @see CPISI_DIFF — underlying arithmetic macro
 * @see cpisi_sign_value — table lookup (equivalent result)
 */
#define CPISI_INDEX_TO_SIGN(i)    CPISI_DIFF((i), CPISI_UNITY)

/** @} */ /* end S.2d.1c CONVERSION */

/** @} */ /* end S.2d.1 SIGNS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d.2 LAYERS [LAYERS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup S2d_2_LAYERS S.2d.2 LAYERS — 9-Layer Pyramid Lookup Tables
 * @ingroup S2d_LOOKUP
 * @brief    Layer properties and size tables for 9-layer Genesis pyramid.
 *
 * PURPOSE:
 *   Runtime lookup for 9-layer architectural pyramid. Provides power values,
 *   names, abbreviations, short names, and buffer sizes for each layer.
 *
 * Builds FROM:
 *   - S.2a.2 DERIVED (LAYER_COUNT, SIZE_COUNT)
 *   - S.2a.4 INDICES (POWER_L0-L8)
 *   - S.2a.5 DOMAIN (SIZE_KERNEL-APP)
 * Builds TO: S.2e TYPES (CpisiLayer enum)
 *
 * STRUCTURE (follows template pattern):
 *   S.2d.2a TABLES     — LAYER_POWERS, LAYER_NAMES, LAYER_ABBREVS, LAYER_SHORT,
 *                        SIZE_NAMES, LAYER_SIZES, LAYER_POSITION (7 arrays)
 *   S.2d.2b ACCESSORS  — cpisi_layer_power(), _name(), _abbrev(), _short(), _size(), _position()
 *   S.2d.2c CONVERSION — [Reference: layer.h provides LAYER_TO_BAL(), BAL_TO_LAYER()]
 *
 * ┌──────────────────────────────────────────────────────────────────────────┐
 * │  THE 9-LAYER PYRAMID — Genesis architectural hierarchy                   │
 * ├──────────────────────────────────────────────────────────────────────────┤
 * │  INDEX   NAME        ABBREV   POWER    SIZE      BALANCED   SEMANTIC     │
 * │  ──────  ──────────  ──────   ───────  ────────  ─────────  ───────────  │
 * │  [0]     KERNEL      K        3⁰=1     27        -4         Foundation   │
 * │  [1]     SYSTEM      S        3¹=3     81        -3         Services     │
 * │  [2]     RUNTIME     R        3²=9     243       -2         Environment  │
 * │  [3]     FRAMEWORK   F        3³=27    729       -1         Libraries    │
 * │  [4]     HEALTH      H        3⁴=81    2187      0 (CENTER) Monitoring   │
 * │  [5]     NETWORK     N        3⁵=243   6561      +1         Communication│
 * │  [6]     STORAGE     T        3⁶=729   19683     +2         Persistence  │
 * │  [7]     ENGINE      E        3⁷=2187  59049     +3         Processing   │
 * │  [8]     APP         A        3⁸=6561  177147    +4         Application  │
 * └──────────────────────────────────────────────────────────────────────────┘
 *
 * NOTE: LOG/DEBUG tables extracted to domain/core/log.h.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.2d.2a TABLES — Layer property and size arrays
 *  @brief Seven arrays for 9-layer pyramid: powers, names, abbrevs, short, sizes, position, size_names.
 *
 *  LAYER ARRAYS (9 entries each, indexed 0-8):
 *    LAYER_POWERS   — 3^n capacity values
 *    LAYER_NAMES    — Full string names ("KERNEL"..."APP")
 *    LAYER_ABBREVS  — Single-char abbreviations ('K'...'A')
 *    LAYER_SHORT    — Short strings ("L0"..."L8")
 *    LAYER_SIZES    — Buffer sizes (3^(n+3))
 *    LAYER_POSITION — Balanced ternary position (-4 to +4)
 *
 *  SIZE ARRAYS (5 entries, indexed 0-4):
 *    SIZE_NAMES     — Tier names ("tiny"..."huge")
 *  @{ */

/**
 * @brief Layer power lookup — maps layer index (0-8) to power of 3 (3^n).
 *
 * @return int array[9] — power values: {1, 3, 9, 27, 81, 243, 729, 2187, 6561}
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_COUNT = 9 (from S.2a.2 DERIVED) — array size
 *   - CPISI_POWER_L0..L8 (from S.2a.4 INDICES) — individual power values
 *
 * @par Derivation
 *      Layer n → 3^n (power-of-3 sequence)
 *      [0] KERNEL=1, [1] SYSTEM=3, [2] RUNTIME=9, [3] FRAMEWORK=27,
 *      [4] HEALTH=81 (CENTER), [5] NETWORK=243, [6] STORAGE=729,
 *      [7] ENGINE=2187, [8] APP=6561
 *
 * @par Semantic Meaning
 *      Power values define capacity and scaling per layer. Foundation layers
 *      have smaller powers (finer granularity), upper layers have larger powers
 *      (coarser granularity). HEALTH at center (81 = 3⁴ = TESSERACT) is the
 *      balanced pivot. NETWORK (243 = PENTERACT) matches ERRORS capacity.
 *
 * @see cpisi_layer_power — accessor macro for this table
 * @see CPISI_LAYER_SIZES — buffer sizes (POWER × CUBE)
 */
static const int CPISI_LAYER_POWERS[CPISI_LAYER_COUNT] = {
    CPISI_POWER_L0,  /* [0] KERNEL    = 3⁰ = 1     */
    CPISI_POWER_L1,  /* [1] SYSTEM    = 3¹ = 3     */
    CPISI_POWER_L2,  /* [2] RUNTIME   = 3² = 9     */
    CPISI_POWER_L3,  /* [3] FRAMEWORK = 3³ = 27    */
    CPISI_POWER_L4,  /* [4] HEALTH    = 3⁴ = 81    */
    CPISI_POWER_L5,  /* [5] NETWORK   = 3⁵ = 243   */
    CPISI_POWER_L6,  /* [6] STORAGE   = 3⁶ = 729   */
    CPISI_POWER_L7,  /* [7] ENGINE    = 3⁷ = 2187  */
    CPISI_POWER_L8   /* [8] APP       = 3⁸ = 6561  */
};

/**
 * @brief Layer names — maps layer index (0-8) to semantic name string.
 *
 * @return const char* array[9] — layer names: {"KERNEL"..."APP"}
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_COUNT = 9 (from S.2a.2 DERIVED) — array size
 *
 * @par Derivation
 *      [0] "KERNEL"    — Core foundation, most critical
 *      [1] "SYSTEM"    — System services and primitives
 *      [2] "RUNTIME"   — Runtime environment and memory
 *      [3] "FRAMEWORK" — Framework and library layer
 *      [4] "HEALTH"    — Health monitoring (CENTER)
 *      [5] "NETWORK"   — Network and communication
 *      [6] "STORAGE"   — Storage and persistence
 *      [7] "ENGINE"    — Engine and processing
 *      [8] "APP"       — Application layer, least critical
 *
 * @par Semantic Meaning
 *      Human-readable names for the 9-layer Genesis pyramid. Used for
 *      logging, error messages, status display, and user-facing output.
 *      Foundation layers (0-3) handle system concerns; upper layers (5-8)
 *      handle user concerns; HEALTH (4) at center bridges both.
 *
 * @see cpisi_layer_name — accessor macro for this table
 * @see CPISI_LAYER_POWERS — parallel array with power values
 * @see CPISI_LAYER_ABBREVS — parallel array with single-char abbreviations
 */
static const char* const CPISI_LAYER_NAMES[CPISI_LAYER_COUNT] = {
    "KERNEL",     /* [0] Core foundation      */
    "SYSTEM",     /* [1] System services      */
    "RUNTIME",    /* [2] Runtime environment  */
    "FRAMEWORK",  /* [3] Framework layer      */
    "HEALTH",     /* [4] Health CENTER        */
    "NETWORK",    /* [5] Network layer        */
    "STORAGE",    /* [6] Storage layer        */
    "ENGINE",     /* [7] Engine layer         */
    "APP"         /* [8] Application layer    */
};

/**
 * @brief Layer abbreviations — maps layer index (0-8) to single character.
 *
 * @return char array[9] — abbreviations: {'K','S','R','F','H','N','T','E','A'}
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_COUNT = 9 (from S.2a.2 DERIVED) — array size
 *
 * @par Derivation
 *      [0] 'K' — KERNEL
 *      [1] 'S' — SYSTEM
 *      [2] 'R' — RUNTIME
 *      [3] 'F' — FRAMEWORK
 *      [4] 'H' — HEALTH (CENTER)
 *      [5] 'N' — NETWORK
 *      [6] 'T' — STORAGE (T not S, avoids SYSTEM collision)
 *      [7] 'E' — ENGINE
 *      [8] 'A' — APP
 *
 * @par Semantic Meaning
 *      Single-character representation for compact display. Used in status
 *      displays, column headers, and space-constrained contexts. Each char
 *      is unique — STORAGE uses 'T' (for sTore) to avoid collision with SYSTEM.
 *
 * EXAMPLE:
 *   printf("[%c]", cpisi_layer_abbrev(layer)); // prints [K], [S], ..., [A]
 *
 * @see cpisi_layer_abbrev — accessor macro for this table
 * @see CPISI_LAYER_NAMES — parallel array with full names
 * @see CPISI_LAYER_SHORT — parallel array with "Ln" format
 */
static const char CPISI_LAYER_ABBREVS[CPISI_LAYER_COUNT] = {
    'K',  /* [0] KERNEL    */
    'S',  /* [1] SYSTEM    */
    'R',  /* [2] RUNTIME   */
    'F',  /* [3] FRAMEWORK */
    'H',  /* [4] HEALTH    */
    'N',  /* [5] NETWORK   */
    'T',  /* [6] STORAGE (T not S) */
    'E',  /* [7] ENGINE    */
    'A'   /* [8] APP       */
};

/**
 * @brief Layer short names — maps layer index (0-8) to "Ln" format string.
 *
 * @return const char* array[9] — short names: {"L0"..."L8"}
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_COUNT = 9 (from S.2a.2 DERIVED) — array size
 *
 * @par Derivation
 *      [0] "L0" — KERNEL
 *      [1] "L1" — SYSTEM
 *      [2] "L2" — RUNTIME
 *      [3] "L3" — FRAMEWORK
 *      [4] "L4" — HEALTH (CENTER)
 *      [5] "L5" — NETWORK
 *      [6] "L6" — STORAGE
 *      [7] "L7" — ENGINE
 *      [8] "L8" — APP
 *      Format: "L" + index (0-8), consistent 2-char width.
 *
 * @par Semantic Meaning
 *      Numeric-based short form for technical logging and structured output.
 *      Unambiguous (no collision), consistent width, machine-parseable.
 *      Preferred for log formats, column headers, and data serialization.
 *
 * EXAMPLE:
 *   printf("%s: %s\n", cpisi_layer_short(l), cpisi_layer_name(l));
 *   // "L4: HEALTH"
 *
 * @see cpisi_layer_short — accessor macro for this table
 * @see CPISI_LAYER_ABBREVS — parallel array with single-char abbreviations
 * @see CPISI_LAYER_NAMES — parallel array with full names
 */
static const char* const CPISI_LAYER_SHORT[CPISI_LAYER_COUNT] = {
    "L0",  /* [0] KERNEL    */
    "L1",  /* [1] SYSTEM    */
    "L2",  /* [2] RUNTIME   */
    "L3",  /* [3] FRAMEWORK */
    "L4",  /* [4] HEALTH    */
    "L5",  /* [5] NETWORK   */
    "L6",  /* [6] STORAGE   */
    "L7",  /* [7] ENGINE    */
    "L8"   /* [8] APP       */
};

/**
 * @brief Size tier names — maps tier index (0-4) to classification string.
 *
 * @return const char* array[5] — tier names: {"tiny"..."huge"}
 *
 * BOUNDS USED:
 *   - CPISI_SIZE_COUNT = 5 (from S.2a.5 DOMAIN) — array size
 *
 * @par Derivation
 *      5-level classification (mirrors HARDPOINTS count):
 *      [0] "tiny"   — Minimal allocation
 *      [1] "small"  — Small allocation
 *      [2] "medium" — Standard allocation (CENTER of 5)
 *      [3] "large"  — Large allocation
 *      [4] "huge"   — Maximum allocation
 *
 * @par Semantic Meaning
 *      Size classification for logging, allocation reporting, and buffer
 *      categorization. The 5-level system mirrors HARDPOINTS, with "medium"
 *      at center (index 2) as the balanced default.
 *
 * @see CPISI_HARDPOINTS — 5 matches this count (not coincidence)
 * @see CPISI_LAYER_SIZES — buffer sizes per layer
 */
static const char* const CPISI_SIZE_NAMES[CPISI_SIZE_COUNT] = {
    "tiny",    /* [0] minimal   */
    "small",   /* [1] small     */
    "medium",  /* [2] standard  */
    "large",   /* [3] large     */
    "huge"     /* [4] maximum   */
};

/**
 * @brief Layer buffer sizes — maps layer (0-8) to buffer allocation (3^(n+3)).
 *
 * @return uint32_t array[9] — sizes: {27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147}
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_COUNT = 9 (from S.2a.2 DERIVED) — array size
 *   - CPISI_SIZE_KERNEL..SIZE_APP (from S.2a.5 DOMAIN) — individual sizes
 *
 * @par Derivation
 *      Power-of-3 sequence, offset by 3: Layer n → 3^(n+3)
 *      [0] KERNEL=27, [1] SYSTEM=81, [2] RUNTIME=243, [3] FRAMEWORK=729,
 *      [4] HEALTH=2187 (CENTER), [5] NETWORK=6561, [6] STORAGE=19683,
 *      [7] ENGINE=59049, [8] APP=177147
 *
 * @par Semantic Meaning
 *      Buffer allocation per layer. RELATIONSHIP: SIZE = POWER × CUBE
 *      (each size = power × 27). Foundation layers have smaller buffers
 *      (tighter constraints); upper layers have larger buffers (more capacity).
 *      HEALTH at center (2187 = 3⁷) is the balanced pivot.
 *
 * @see cpisi_layer_size — accessor macro for this table
 * @see CPISI_LAYER_POWERS — parallel array (SIZE = POWER × 27)
 */
static const uint32_t CPISI_LAYER_SIZES[CPISI_LAYER_COUNT] = {
    CPISI_SIZE_KERNEL,      /* [0] = 3³  = 27     */
    CPISI_SIZE_SYSTEM,      /* [1] = 3⁴  = 81     */
    CPISI_SIZE_RUNTIME,     /* [2] = 3⁵  = 243    */
    CPISI_SIZE_FRAMEWORK,   /* [3] = 3⁶  = 729    */
    CPISI_SIZE_HEALTH,      /* [4] = 3⁷  = 2187   */
    CPISI_SIZE_NETWORK,     /* [5] = 3⁸  = 6561   */
    CPISI_SIZE_STORAGE,     /* [6] = 3⁹  = 19683  */
    CPISI_SIZE_ENGINE,      /* [7] = 3¹⁰ = 59049  */
    CPISI_SIZE_APP          /* [8] = 3¹¹ = 177147 */
};

/**
 * @brief Layer balanced ternary position — maps layer index (0-8) to position (-4 to +4).
 *
 * @return int8_t array[9] — positions: {-4, -3, -2, -1, 0, +1, +2, +3, +4}
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_COUNT = 9 (from S.2a.2 DERIVED) — array size
 *   - CPISI_LAYER_CENTER = 4 (from S.2a.2 DERIVED) — center offset
 *
 * @par Derivation
 *      Balanced ternary position: layer - CPISI_LAYER_CENTER = layer - 4
 *      [0] KERNEL    = 0 - 4 = -4 (most foundational)
 *      [1] SYSTEM    = 1 - 4 = -3
 *      [2] RUNTIME   = 2 - 4 = -2
 *      [3] FRAMEWORK = 3 - 4 = -1
 *      [4] HEALTH    = 4 - 4 =  0 (CENTER — yashar)
 *      [5] NETWORK   = 5 - 4 = +1
 *      [6] STORAGE   = 6 - 4 = +2
 *      [7] ENGINE    = 7 - 4 = +3
 *      [8] APP       = 8 - 4 = +4 (most application)
 *
 * @par Semantic Meaning
 *      Balanced ternary position in the 9-layer pyramid. NEGATIVE positions
 *      (-4 to -1) are foundation layers below HEALTH. POSITIVE positions
 *      (+1 to +4) are application layers above HEALTH. HEALTH at position 0
 *      is the observation point (yashar) — the center of the system.
 *      Used for direction calculations, health-relative metrics, and
 *      balanced ternary navigation across all system components.
 *
 * @see cpisi_layer_position — accessor macro for this table
 * @see CPISI_LAYER_TO_BAL — conversion macro (computed equivalent)
 * @see CPISI_LAYER_CENTER — center value (4)
 */
static const int8_t CPISI_LAYER_POSITION[CPISI_LAYER_COUNT] = {
    -4,     /* [0] KERNEL:    position -4 (most foundational) */
    -3,     /* [1] SYSTEM:    position -3                     */
    -2,     /* [2] RUNTIME:   position -2                     */
    -1,     /* [3] FRAMEWORK: position -1                     */
     0,     /* [4] HEALTH:    position  0 (CENTER — yashar)   */
    +1,     /* [5] NETWORK:   position +1                     */
    +2,     /* [6] STORAGE:   position +2                     */
    +3,     /* [7] ENGINE:    position +3                     */
    +4      /* [8] APP:       position +4 (most application)  */
};

/** @} */ /* end S.2d.2a TABLES */


/** @name S.2d.2b ACCESSORS — Layer property lookup macros
 *  @brief Read macros for 9-layer pyramid properties.
 *
 *  PATTERN: Each accessor provides direct array access without bounds check.
 *           Caller must ensure 0 ≤ l ≤ 8 (CPISI_LAYER_COUNT-1).
 *
 *  ACCESSORS (6 total):
 *    cpisi_layer_power(l)    — 3^n capacity value
 *    cpisi_layer_name(l)     — Full string name
 *    cpisi_layer_abbrev(l)   — Single-char abbreviation
 *    cpisi_layer_short(l)    — Short "Ln" string
 *    cpisi_layer_size(l)     — Buffer size (3^(n+3))
 *    cpisi_layer_position(l) — Balanced ternary position (-4 to +4)
 *  @{ */

/**
 * @brief Get power of 3 for layer — LAYER (0-8) → POWER (3^n).
 *
 * @return int — power value from CPISI_LAYER_POWERS[]
 *
 * BOUNDS USED:
 *   - INPUT:  l ∈ [0, CPISI_LAYER_COUNT-1] = [0, 8]
 *   - OUTPUT: CPISI_LAYER_POWERS[l] ∈ {1, 3, 9, 27, 81, 243, 729, 2187, 6561}
 *
 * @par Derivation
 *      Direct index into CPISI_LAYER_POWERS[l].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ l ≤ 8.
 *
 * @par Semantic Meaning
 *      Layer capacity scaling factor. Foundation layers have smaller powers
 *      (finer granularity); upper layers have larger powers (coarser granularity).
 *
 * EXAMPLE:
 *   cpisi_layer_power(0) → 1    (KERNEL = 3⁰)
 *   cpisi_layer_power(4) → 81   (HEALTH = 3⁴, CENTER)
 *   cpisi_layer_power(8) → 6561 (APP = 3⁸)
 *
 * @param l Layer index 0-8 (unchecked)
 *
 * @see CPISI_LAYER_POWERS — source table
 */
#define cpisi_layer_power(l)   (CPISI_LAYER_POWERS[(l)])

/**
 * @brief Get full name for layer — LAYER (0-8) → NAME string.
 *
 * @return const char* — layer name from CPISI_LAYER_NAMES[]
 *
 * BOUNDS USED:
 *   - INPUT:  l ∈ [0, CPISI_LAYER_COUNT-1] = [0, 8]
 *   - OUTPUT: CPISI_LAYER_NAMES[l] ∈ {"KERNEL"..."APP"}
 *
 * @par Derivation
 *      Direct index into CPISI_LAYER_NAMES[l].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ l ≤ 8.
 *
 * @par Semantic Meaning
 *      Human-readable layer name for logging, error messages, status display.
 *
 * EXAMPLE:
 *   cpisi_layer_name(0) → "KERNEL"
 *   cpisi_layer_name(4) → "HEALTH" (CENTER)
 *   cpisi_layer_name(8) → "APP"
 *
 * @param l Layer index 0-8 (unchecked)
 *
 * @see CPISI_LAYER_NAMES — source table
 */
#define cpisi_layer_name(l)    (CPISI_LAYER_NAMES[(l)])

/**
 * @brief Get single-char abbreviation for layer — LAYER (0-8) → CHAR.
 *
 * @return char — abbreviation from CPISI_LAYER_ABBREVS[]
 *
 * BOUNDS USED:
 *   - INPUT:  l ∈ [0, CPISI_LAYER_COUNT-1] = [0, 8]
 *   - OUTPUT: CPISI_LAYER_ABBREVS[l] ∈ {'K','S','R','F','H','N','T','E','A'}
 *
 * @par Derivation
 *      Direct index into CPISI_LAYER_ABBREVS[l].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ l ≤ 8.
 *
 * @par Semantic Meaning
 *      Compact single-char representation for space-constrained display.
 *      STORAGE uses 'T' (sTore) to avoid collision with SYSTEM 'S'.
 *
 * EXAMPLE:
 *   cpisi_layer_abbrev(0) → 'K' (KERNEL)
 *   cpisi_layer_abbrev(4) → 'H' (HEALTH, CENTER)
 *   cpisi_layer_abbrev(6) → 'T' (STORAGE — T not S)
 *
 * @param l Layer index 0-8 (unchecked)
 *
 * @see CPISI_LAYER_ABBREVS — source table
 */
#define cpisi_layer_abbrev(l)  (CPISI_LAYER_ABBREVS[(l)])

/**
 * @brief Get short name for layer — LAYER (0-8) → "Ln" string.
 *
 * @return const char* — short name from CPISI_LAYER_SHORT[]
 *
 * BOUNDS USED:
 *   - INPUT:  l ∈ [0, CPISI_LAYER_COUNT-1] = [0, 8]
 *   - OUTPUT: CPISI_LAYER_SHORT[l] ∈ {"L0"..."L8"}
 *
 * @par Derivation
 *      Direct index into CPISI_LAYER_SHORT[l].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ l ≤ 8.
 *
 * @par Semantic Meaning
 *      Numeric-based short form for technical logging and structured output.
 *      Unambiguous, consistent 2-char width, machine-parseable.
 *
 * EXAMPLE:
 *   cpisi_layer_short(0) → "L0" (KERNEL)
 *   cpisi_layer_short(4) → "L4" (HEALTH, CENTER)
 *   cpisi_layer_short(8) → "L8" (APP)
 *
 * @param l Layer index 0-8 (unchecked)
 *
 * @see CPISI_LAYER_SHORT — source table
 */
#define cpisi_layer_short(l)   (CPISI_LAYER_SHORT[(l)])

/**
 * @brief Get layer buffer size — LAYER (0-8) → SIZE (3^(n+3)).
 *
 * @return uint32_t — buffer size from CPISI_LAYER_SIZES[]
 *
 * BOUNDS USED:
 *   - INPUT:  l ∈ [0, CPISI_LAYER_COUNT-1] = [0, 8]
 *   - OUTPUT: CPISI_LAYER_SIZES[l] ∈ {27..177147}
 *
 * @par Derivation
 *      Direct index into CPISI_LAYER_SIZES[l].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ l ≤ 8.
 *
 * @par Semantic Meaning
 *      Buffer allocation size per layer. SIZE = POWER × 27 (CUBE).
 *      Foundation layers have smaller buffers; upper layers have larger.
 *
 * EXAMPLE:
 *   cpisi_layer_size(0) → 27     (KERNEL = 3³)
 *   cpisi_layer_size(4) → 2187   (HEALTH = 3⁷, CENTER)
 *   cpisi_layer_size(8) → 177147 (APP = 3¹¹)
 *
 * @param l Layer index 0-8 (unchecked)
 *
 * @see CPISI_LAYER_SIZES — source table
 */
#define cpisi_layer_size(l)    (CPISI_LAYER_SIZES[(l)])

/**
 * @brief Get balanced ternary position for layer — LAYER (0-8) → POSITION (-4 to +4).
 *
 * @return int8_t — balanced position from CPISI_LAYER_POSITION[]
 *
 * BOUNDS USED:
 *   - INPUT:  l ∈ [0, CPISI_LAYER_COUNT-1] = [0, 8]
 *   - OUTPUT: CPISI_LAYER_POSITION[l] ∈ {-4, -3, -2, -1, 0, +1, +2, +3, +4}
 *
 * @par Derivation
 *      Direct index into CPISI_LAYER_POSITION[l].
 *      NO BOUNDS CHECK: Caller must ensure 0 ≤ l ≤ 8.
 *      Equivalent to: CPISI_LAYER_TO_BAL(l) = l - CPISI_LAYER_CENTER
 *
 * @par Semantic Meaning
 *      Balanced ternary position in the 9-layer pyramid. NEGATIVE positions
 *      are foundation layers; POSITIVE positions are application layers;
 *      HEALTH at position 0 is the observation point (yashar).
 *
 * EXAMPLE:
 *   cpisi_layer_position(0) → -4 (KERNEL, most foundational)
 *   cpisi_layer_position(4) →  0 (HEALTH, CENTER — yashar)
 *   cpisi_layer_position(8) → +4 (APP, most application)
 *
 * @param l Layer index 0-8 (unchecked)
 *
 * @see CPISI_LAYER_POSITION — source table
 * @see CPISI_LAYER_TO_BAL — computed equivalent (in layer.h)
 */
#define cpisi_layer_position(l) (CPISI_LAYER_POSITION[(l)])

/** @} */ /* end S.2d.2b ACCESSORS */


/** @name S.2d.2c CONVERSION — Layer coordinate transforms
 *  @brief Bidirectional conversion between layer index (0-8) and balanced (-4 to +4).
 *
 *  REFERENCE: Full implementation in domain/core/layer.h
 *
 *  MACROS (defined in layer.h):
 *    CPISI_LAYER_TO_BAL(layer) — Index (0-8) → Balanced (-4 to +4)
 *    CPISI_BAL_TO_LAYER(bal)   — Balanced (-4 to +4) → Index (0-8)
 *
 *  DERIVATION:
 *    balanced = layer - CPISI_LAYER_CENTER (subtract 4)
 *    layer = balanced + CPISI_LAYER_CENTER (add 4)
 *
 *  @see domain/core/layer.h — full macro implementations
 *  @{ */

/* Layer conversion macros defined in domain/core/layer.h */

/** @} */ /* end S.2d.2c CONVERSION */


/* NOTE: LOG and DEBUG tables extracted to domain/core/log.h */

/** @} */ /* end S.2d.2 LAYERS */

/** @} */ /* end S.2d LOOKUP */


/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2e TYPES [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup S2e_TYPES S.2e TYPES — Type-safe Enumerations
 * @ingroup cpisi_defines
 * @brief    Type-safe interfaces wrapping raw constants and lookup tables.
 *
 * PURPOSE:
 *   This section provides COMPILE-TIME type safety where S.2d provides
 *   RUNTIME lookup capability. Enums wrap verified constants.
 *
 * STRUCTURE:
 *   S.2e.1 SIGN  — CpisiSign (3 values) — matches S.2d.1 SIGNS
 *   S.2e.2 LAYER — CpisiLayer (9 values) — matches S.2d.2 LAYERS
 *   S.2e.3 SIZE  — CpisiSizeTier (5 values) — utility type (unused)
 *
 * EXTRACTED TO DOMAIN FILES:
 *   CpisiLogLevel   → log.h S.2e.1 (LOG domain)
 *   CpisiDebugLevel → log.h S.2e.2 (LOG domain)
 *
 * @{
 */


/**
 * @defgroup S2e_1_SIGN S.2e.1 SIGN — CpisiSign (3 values)
 * @ingroup S2e_TYPES
 * @brief Type-safe sign enumeration for balanced ternary direction values.
 *
 * SEMANTIC FOUNDATION:
 *   The sign concept is foundational to balanced ternary arithmetic.
 *   Every trit, every direction, every choice reduces to sign.
 *
 *   NEGATIVE (-1) — retreat, toward foundation, break down (k=-1)
 *   NEUTRAL  ( 0) — maintain, hold position, center (k=0)
 *   POSITIVE (+1) — advance, toward growth, build up (k=+1)
 *
 * Builds FROM:
 *   - S.2a.1 ANCHORS (NEGATIVE, NEUTRAL, POSITIVE, SIGN_COUNT)
 *
 * Builds TO:
 *   - S.2d.1 SIGNS (lookup tables for display, balanced, ternary)
 *
 * @{ */

/**
 * @brief Sign enumeration for type-safe sign parameters.
 *
 * BOUNDS USED:
 *   - CPISI_NEGATIVE (-1) — S.2a.1 ANCHORS balanced ternary minimum
 *   - CPISI_NEUTRAL (0)   — S.2a.1 ANCHORS balanced ternary center
 *   - CPISI_POSITIVE (+1) — S.2a.1 ANCHORS balanced ternary maximum
 *   - CPISI_SIGN_COUNT (3)— S.2a.1 ANCHORS sign cardinality
 *
 * VALUE MAPPING:
 *   CPISI_SIGN_NEG        = CPISI_NEGATIVE (-1) — retreat direction
 *   CPISI_SIGN_ZERO       = CPISI_NEUTRAL  (0)  — hold position (CENTER)
 *   CPISI_SIGN_POS        = CPISI_POSITIVE (+1) — advance direction
 *   CPISI_SIGN_ENUM_COUNT = CPISI_SIGN_COUNT (3)— iteration bound
 *
 * USE CASE:
 *   - Function parameters: void cpisi_move(CpisiSign direction);
 *   - Loop iteration: for (CpisiSign s = 0; s < CPISI_SIGN_ENUM_COUNT; s++)
 *   - Switch dispatch: switch(sign) { case CPISI_SIGN_NEG: ... }
 *
 * RELATIONSHIP TO LOOKUP (S.2d.1 SIGNS):
 *   cpisi_sign_display(idx)  → "-", "0", "+"
 *   cpisi_sign_balanced(idx) → -1, 0, +1
 *   cpisi_sign_ternary(idx)  → 0, 1, 2
 */
typedef enum CpisiSign {
    CPISI_SIGN_NEG        = CPISI_NEGATIVE,  /**< -1: Retreat toward foundation */
    CPISI_SIGN_ZERO       = CPISI_NEUTRAL,   /**<  0: Hold position (CENTER) */
    CPISI_SIGN_POS        = CPISI_POSITIVE,  /**< +1: Advance toward completion */
    CPISI_SIGN_ENUM_COUNT = CPISI_SIGN_COUNT /**<  3: Iteration bound */
} CpisiSign;


/** @} */ /* end S.2e.1 SIGN */

/**
 * @defgroup S2e_2_LAYER S.2e.2 LAYER — CpisiLayer (9 values)
 * @ingroup S2e_TYPES
 *  @brief Genesis 9-layer pyramid: KERNEL(0) → APP(8), HEALTH(4) at CENTER.
 *
 *  The 9 layers map to 3^2 = 9 (a cube face). HEALTH (4) sits at the CENTER,
 *  with 4 layers above (user-facing) and 4 below (system-facing).
 *
 *     APP (8)        — application, least critical
 *     ENGINE (7)     — game/app engine
 *     STORAGE (6)    — persistence
 *     NETWORK (5)    — communication
 *     HEALTH (4)     ← CENTER (diagnostic hub)
 *     FRAMEWORK (3)  — libraries
 *     RUNTIME (2)    — execution environment
 *     SYSTEM (1)     — OS-level
 *     KERNEL (0)     — foundation, most critical
 *
 *  @{ */

/**
 * @brief Layer enumeration for 9-layer diagnostic pyramid.
 *
 * BOUNDS USED:
 *   - CPISI_LAYER_KERNEL (0) through CPISI_LAYER_APP (8) — S.2a.4 INDICES
 *   - CPISI_LAYER_COUNT (9) — S.2a.2 DERIVED, equal to CPISI_FACE (3^2)
 *   - CPISI_LAYER_HEALTH (4) — CENTER of the 9-layer pyramid
 *
 * VALUE MAPPING:
 *   CPISI_LAYER_ENUM_KERNEL (0)    — core foundation, most critical
 *   CPISI_LAYER_ENUM_SYSTEM (1)    — system services
 *   CPISI_LAYER_ENUM_RUNTIME (2)   — runtime environment
 *   CPISI_LAYER_ENUM_FRAMEWORK (3) — framework layer
 *   CPISI_LAYER_ENUM_HEALTH (4)    — health center (CENTER)
 *   CPISI_LAYER_ENUM_NETWORK (5)   — network layer
 *   CPISI_LAYER_ENUM_STORAGE (6)   — storage layer
 *   CPISI_LAYER_ENUM_ENGINE (7)    — engine layer
 *   CPISI_LAYER_ENUM_APP (8)       — application, least critical
 *   CPISI_LAYER_ENUM_COUNT (9)     — iteration bound
 *
 * RELATIONSHIP TO LOOKUP (S.2d.2 LAYERS):
 *   cpisi_layer_name(idx)   → "KERNEL", "SYSTEM", ..., "APP"
 *   cpisi_layer_abbrev(idx) → "KRN", "SYS", ..., "APP"
 */
typedef enum CpisiLayer {
    CPISI_LAYER_ENUM_KERNEL    = CPISI_LAYER_KERNEL,    /**< 0: KERNEL — core foundation, most critical */
    CPISI_LAYER_ENUM_SYSTEM    = CPISI_LAYER_SYSTEM,    /**< 1: SYSTEM — system services */
    CPISI_LAYER_ENUM_RUNTIME   = CPISI_LAYER_RUNTIME,   /**< 2: RUNTIME — execution environment */
    CPISI_LAYER_ENUM_FRAMEWORK = CPISI_LAYER_FRAMEWORK, /**< 3: FRAMEWORK — libraries */
    CPISI_LAYER_ENUM_HEALTH    = CPISI_LAYER_HEALTH,    /**< 4: HEALTH — diagnostic hub (CENTER) */
    CPISI_LAYER_ENUM_NETWORK   = CPISI_LAYER_NETWORK,   /**< 5: NETWORK — communication */
    CPISI_LAYER_ENUM_STORAGE   = CPISI_LAYER_STORAGE,   /**< 6: STORAGE — persistence */
    CPISI_LAYER_ENUM_ENGINE    = CPISI_LAYER_ENGINE,    /**< 7: ENGINE — game/app engine */
    CPISI_LAYER_ENUM_APP       = CPISI_LAYER_APP,       /**< 8: APP — application, least critical */
    CPISI_LAYER_ENUM_COUNT     = CPISI_LAYER_COUNT      /**< 9: Iteration bound */
} CpisiLayer;

/** @} */ /* end S.2e.2 LAYER */

/**
 * @defgroup S2e_3_SIZE S.2e.3 SIZE — CpisiSizeTier (5 values)
 * @ingroup S2e_TYPES
 *  @brief 5-tier allocation sizes: TINY(0) → HUGE(4), MEDIUM(2) at CENTER.
 *
 *  TINY   (0) —   64 bytes (metadata, single trit)
 *  SMALL  (1) —  256 bytes (single tryte, small struct)
 *  MEDIUM (2) — 1024 bytes (work buffer) ← CENTER
 *  LARGE  (3) — 4096 bytes (page-aligned buffer)
 *  HUGE   (4) — 16K+ bytes (large allocation)
 *
 *  @{ */

/**
 * @brief Size tier enumeration for allocation classification.
 *
 * BOUNDS USED:
 *   - CPISI_SIZE_TIER_COUNT (5)  — S.2a.2 DERIVED
 *   - CPISI_SIZE_TIER_CENTER (2) — S.2a.2 DERIVED, center of 5
 *
 * VALUE MAPPING:
 *   CPISI_SIZE_TINY (0)   — minimal (~64 bytes)
 *   CPISI_SIZE_SMALL (1)  — small (~256 bytes)
 *   CPISI_SIZE_MEDIUM (2) — standard (~1024 bytes) (CENTER)
 *   CPISI_SIZE_LARGE (3)  — large (~4096 bytes)
 *   CPISI_SIZE_HUGE (4)   — maximum (~16K+ bytes)
 *
 * RELATIONSHIP TO LOOKUP (S.2d.2 LAYERS):
 *   cpisi_size_name(idx)  → "TINY", "SMALL", "MEDIUM", "LARGE", "HUGE"
 *   cpisi_size_bytes(idx) → implementation-defined byte counts
 */
typedef enum CpisiSizeTier {
    CPISI_SIZE_TINY   = 0,  /**< 0: Minimal — ~64 bytes */
    CPISI_SIZE_SMALL  = 1,  /**< 1: Small — ~256 bytes */
    CPISI_SIZE_MEDIUM = 2,  /**< 2: Standard — ~1024 bytes (CENTER) */
    CPISI_SIZE_LARGE  = 3,  /**< 3: Large — ~4096 bytes */
    CPISI_SIZE_HUGE   = 4   /**< 4: Maximum — ~16K+ bytes */
} CpisiSizeTier;

/** @} */ /* end S.2e.3 SIZE */

/* NOTE: CpisiLogLevel and CpisiDebugLevel extracted to log.h S.2e */

/** @} */ /* end S.2e TYPES */

/** @} */ /* end of cpisi_defines */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types S.3 TYPES — Type Declarations
 * @brief    Bridge between compile-time constants (S.2) and runtime structures (BODY).
 *
 * PURPOSE: Types must be declared before they can be used in implementations.
 *          SETUP types are foundational; domain types live in BODY.
 *
 * CONTAINS:
 *   - S.3a ENUMS   — Early-availability enumerations (reserved)
 *   - S.3b STRUCTS — Early-availability structures (reserved)
 *   - S.3c OPAQUE  — Forward-declared opaque types (reserved)
 *   - S.3d ERRORS  — Error code enum (CpisiError — 243 codes + 13 metadata = 256)
 *
 * Section order: Enums → Structs → Opaque → Errors
 * Flow: simple types → composite types → hidden types → error codes
 *
 * Builds FROM: S.2 DEFINES (constants and macros available)
 * Builds TO:   BODY block (implementations use these types)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — Early-Availability Enumerations [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_types_enums S.3a ENUMS — Early-Availability Enumerations
 * @ingroup cpisi_types
 * @brief    Reserved for enum types needed before BODY.
 *
 * PURPOSE: Enums that must exist before BODY implementations.
 *          Currently reserved — domain enums in BODY (B.2).
 *
 * Builds FROM: S.3 TYPES (section context)
 * Builds TO:   S.3b STRUCTS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Domain enums defined in BODY B.2] */

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — Early-Availability Structures [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_types_structs S.3b STRUCTS — Early-Availability Structures
 * @ingroup cpisi_types
 * @brief    Reserved for struct types needed before BODY.
 *
 * PURPOSE: Structs that must exist before BODY implementations.
 *          Currently reserved — domain structs in BODY (B.2).
 *
 * Builds FROM: S.3a ENUMS
 * Builds TO:   S.3c OPAQUE
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Domain structs defined in BODY B.2] */

/** @} */ /* end S.3b STRUCTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3c OPAQUE — Forward-Declared Opaque Types [OPAQUE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_types_opaque S.3c OPAQUE — Forward-Declared Opaque Types
 * @ingroup cpisi_types
 * @brief    Reserved for opaque types requiring implementation hiding.
 *
 * PURPOSE: Types where internal structure is hidden from callers.
 *          CPI-SI prefers transparent types for introspection.
 *
 * Builds FROM: S.3b STRUCTS
 * Builds TO:   S.3d ERRORS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: CPI-SI prefers transparent types for introspection] */

/** @} */ /* end S.3c OPAQUE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d ERRORS — Error Code Foundation [ERRORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_types_errors S.3d ERRORS — Error Code Foundation
 * @ingroup cpisi_types
 * @brief    Base error type and layer anchors — 243 codes + 13 metadata = 256.
 *
 * PURPOSE: Foundation error types that types/config/error.h builds on.
 *          This provides the TYPE and LAYER BASES; error.h provides CODES.
 *
 * ARCHITECTURE (from S.2a.3 INDICES):
 *   CPISI_ERRORS (243) — ternary error space (9 layers × 27 codes)
 *   CPISI_BRIDGE (13)  — metadata slots
 *   CPISI_BINARY_FIT (256) — exactly one byte
 *
 * FORMULA: error_code = (layer × CUBE) + (tier × FACE) + position
 *                     = (layer × 27) + (tier × 9) + position
 *
 * LAYER STRUCTURE (4+1+4 balanced):
 *   Below center: KERNEL(0), SYSTEM(1), RUNTIME(2), FRAMEWORK(3)
 *   Center:       HEALTH(4)
 *   Above center: NETWORK(5), STORAGE(6), ENGINE(7), APP(8)
 *
 * Builds FROM: S.3c OPAQUE, S.2a.3 INDICES (CPISI_ERRORS, CPISI_BRIDGE)
 * Builds TO:   S.4 PROTOTYPES, types/config/error.h (full codes)
 *
 * @see types/config/error.h for full error code definitions
 * @see S.2a.3 INDICES for CPISI_ERRORS, CPISI_BRIDGE constants
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.3d.1 ERROR TYPE — Scalable error code type
 *  @brief Core error type definition and range checking macros.
 *  @{ */

/**
 * @brief Scalable error code type.
 *
 * @par Type
 *      uint16_t — allows 65536 possible error codes for future expansion.
 *
 * @par Core Range
 *      Core CPI-SI errors (0-255) fit in uint8_t for efficiency.
 *      Use CPISI_ERROR_IS_CORE() to check, CPISI_ERROR_TO_U8() to cast.
 *
 * @par Domain Structure (future expansion)
 *      -     0-255:   CPI-SI core (current, backward compatible)
 *      -   256-511:   CPI-SI extended
 *      -   512-767:   Cornerstone engine
 *      -   768-1023:  FaithNet
 *      - 1024-1279:   Bereshit storage
 *      - 1280+:       Application-specific
 *
 * @see CPISI_ERROR_CORE_MAX — maximum core error value (255)
 * @see types/config/error.h — full error code definitions
 */
typedef uint16_t CpisiError;

/**
 * @brief 255: Maximum value for core CPI-SI errors.
 *
 * @return 255 (integer constant)
 *
 * @par Derivation
 *      ERROR_CORE_MAX = BYTE_MAX = 255
 *
 * @par Used For
 *      - Range checking: is error in core range?
 *      - Safe casting to uint8_t
 *
 * @see CPISI_BYTE_MAX — the underlying constant
 * @see CPISI_ERROR_IS_CORE — range check macro
 */
#define CPISI_ERROR_CORE_MAX        CPISI_BYTE_MAX

/**
 * @brief Check if error code is in core range (0-255).
 *
 * @param code Error code to check (CpisiError or integer)
 * @return Non-zero if code <= 255, zero otherwise
 *
 * @par Usage
 *      if (CPISI_ERROR_IS_CORE(err)) {
 *          uint8_t compact = CPISI_ERROR_TO_U8(err);
 *      }
 *
 * @see CPISI_ERROR_CORE_MAX — the maximum core value
 * @see CPISI_ERROR_TO_U8 — safe cast after check
 */
#define CPISI_ERROR_IS_CORE(code)   ((code) <= CPISI_ERROR_CORE_MAX)

/**
 * @brief Cast error to uint8_t (only valid for core errors).
 *
 * @param code Error code to cast (must be <= 255)
 * @return Error code as uint8_t
 *
 * @warning Only use after verifying CPISI_ERROR_IS_CORE(code).
 *          Truncates without warning if code > 255.
 *
 * @par Usage
 *      if (CPISI_ERROR_IS_CORE(err)) {
 *          uint8_t compact = CPISI_ERROR_TO_U8(err);
 *      }
 *
 * @see CPISI_ERROR_IS_CORE — check before casting
 */
#define CPISI_ERROR_TO_U8(code)     ((uint8_t)(code))

/** @} */ /* end S.3d.1 ERROR TYPE */

/** @name S.3d.2 SUCCESS — Universal success code
 *  @brief The universal success value for all CpisiError returns.
 *  @{ */

/**
 * @brief 0: Universal success — no error.
 *
 * @return 0 (integer constant)
 *
 * @par Derivation
 *      CPISI_OK = CPISI_VOID = 0
 *      Also: MAKE_ERROR(KERNEL, CORE, SUCCESS) = (0×27) + (0×9) + 0 = 0
 *
 * @par Position in Error Space
 *      - Layer:    KERNEL (0) — foundation layer
 *      - Tier:     CORE (0) — fundamental tier
 *      - Position: SUCCESS (0) — success position
 *
 * @par Usage
 *      All functions returning CpisiError use CPISI_OK for success:
 *      @code
 *      CpisiError my_function(void) {
 *          // ... work ...
 *          return CPISI_OK;
 *      }
 *      @endcode
 *
 * @see CPISI_VOID — THE zero anchor
 * @see CPISI_POS_SUCCESS — position 0 meaning
 */
#define CPISI_OK                    CPISI_VOID

/** @} */ /* end S.3d.2 SUCCESS */

/** @name S.3d.3 LAYER BASES — Starting code for each layer
 *  @brief Error construction macro and layer base anchor points.
 *  @{ */

/**
 * @brief Construct error code from layer, tier, and position components.
 *
 * @param layer Layer index (0-8): CPISI_LAYER_KERNEL through CPISI_LAYER_APP
 * @param tier  Tier index (0-2): CPISI_TIER_CORE, CPISI_TIER_EXT, CPISI_TIER_SYS
 * @param pos   Position index (0-8): CPISI_POS_SUCCESS through CPISI_POS_UNKNOWN
 * @return Error code as integer
 *
 * @par Formula
 *      error_code = (layer × CUBE) + (tier × FACE) + position
 *                 = (layer × 27) + (tier × 9) + position
 *
 * @par Examples
 *      - CPISI_OK = MAKE_ERROR(KERNEL, CORE, SUCCESS) = (0×27)+(0×9)+0 = 0
 *      - CPISI_ERR_KERNEL_NULL = MAKE_ERROR(KERNEL, CORE, NULL) = 0+0+1 = 1
 *      - CPISI_ERR_HEALTH_BOUNDS = MAKE_ERROR(HEALTH, CORE, BOUNDS) = 108+0+3 = 111
 *
 * @see CPISI_CUBE — layer stride (27)
 * @see CPISI_FACE — tier stride (9)
 * @see types/config/error.h — all error codes use this macro
 */
#define CPISI_MAKE_ERROR(layer, tier, pos) \
    (CPISI_PRODUCT((layer), CPISI_CUBE) + CPISI_PRODUCT((tier), CPISI_FACE) + (pos))

/**
 * @brief 0: KERNEL layer base (codes 0-26).
 *
 * @return 0 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_KERNEL = LAYER_KERNEL × CUBE = 0 × 27 = 0
 *
 * @par Layer Purpose
 *      Foundation errors — universal errors that can occur anywhere.
 *      Position -4 in balanced ternary (furthest below center).
 *
 * @see CPISI_LAYER_KERNEL — layer index (0)
 */
#define CPISI_ERR_BASE_KERNEL       CPISI_PRODUCT(CPISI_LAYER_KERNEL, CPISI_CUBE)

/**
 * @brief 27: SYSTEM layer base (codes 27-53).
 *
 * @return 27 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_SYSTEM = LAYER_SYSTEM × CUBE = 1 × 27 = 27
 *
 * @par Layer Purpose
 *      System-level errors — OS interactions, environment issues.
 *      Position -3 in balanced ternary.
 *
 * @see CPISI_LAYER_SYSTEM — layer index (1)
 */
#define CPISI_ERR_BASE_SYSTEM       CPISI_PRODUCT(CPISI_LAYER_SYSTEM, CPISI_CUBE)

/**
 * @brief 54: RUNTIME layer base (codes 54-80).
 *
 * @return 54 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_RUNTIME = LAYER_RUNTIME × CUBE = 2 × 27 = 54
 *
 * @par Layer Purpose
 *      VM/bytecode errors — virtual machine and runtime issues.
 *      Position -2 in balanced ternary.
 *
 * @see CPISI_LAYER_RUNTIME — layer index (2)
 */
#define CPISI_ERR_BASE_RUNTIME      CPISI_PRODUCT(CPISI_LAYER_RUNTIME, CPISI_CUBE)

/**
 * @brief 81: FRAMEWORK layer base (codes 81-107).
 *
 * @return 81 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_FRAMEWORK = LAYER_FRAMEWORK × CUBE = 3 × 27 = 81
 *
 * @par Layer Purpose
 *      Framework errors — CPI-SI framework-specific issues.
 *      Position -1 in balanced ternary (just below center).
 *
 * @see CPISI_LAYER_FRAMEWORK — layer index (3)
 */
#define CPISI_ERR_BASE_FRAMEWORK    CPISI_PRODUCT(CPISI_LAYER_FRAMEWORK, CPISI_CUBE)

/**
 * @brief 108: HEALTH layer base (codes 108-134). THE CENTER.
 *
 * @return 108 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_HEALTH = LAYER_HEALTH × CUBE = 4 × 27 = 108
 *
 * @par Layer Purpose
 *      Health errors — the CENTER layer in 4+1+4 balanced structure.
 *      Position 0 in balanced ternary. All health-related errors.
 *
 * @par Biblical Connection
 *      Health is the center — "Beloved, I wish above all things that thou
 *      mayest prosper and be in health" (3 John 1:2)
 *
 * @see CPISI_LAYER_HEALTH — layer index (4)
 */
#define CPISI_ERR_BASE_HEALTH       CPISI_PRODUCT(CPISI_LAYER_HEALTH, CPISI_CUBE)

/**
 * @brief 135: NETWORK layer base (codes 135-161).
 *
 * @return 135 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_NETWORK = LAYER_NETWORK × CUBE = 5 × 27 = 135
 *
 * @par Layer Purpose
 *      Network errors — communication, FaithNet, connectivity.
 *      Position +1 in balanced ternary (just above center).
 *
 * @see CPISI_LAYER_NETWORK — layer index (5)
 */
#define CPISI_ERR_BASE_NETWORK      CPISI_PRODUCT(CPISI_LAYER_NETWORK, CPISI_CUBE)

/**
 * @brief 162: STORAGE layer base (codes 162-188).
 *
 * @return 162 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_STORAGE = LAYER_STORAGE × CUBE = 6 × 27 = 162
 *
 * @par Layer Purpose
 *      Storage/file errors — filesystem, database, persistence.
 *      Position +2 in balanced ternary.
 *
 * @see CPISI_LAYER_STORAGE — layer index (6)
 */
#define CPISI_ERR_BASE_STORAGE      CPISI_PRODUCT(CPISI_LAYER_STORAGE, CPISI_CUBE)

/**
 * @brief 189: ENGINE layer base (codes 189-215).
 *
 * @return 189 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_ENGINE = LAYER_ENGINE × CUBE = 7 × 27 = 189
 *
 * @par Layer Purpose
 *      Engine errors — Cornerstone engine, rendering, orchestration.
 *      Position +3 in balanced ternary.
 *
 * @see CPISI_LAYER_ENGINE — layer index (7)
 */
#define CPISI_ERR_BASE_ENGINE       CPISI_PRODUCT(CPISI_LAYER_ENGINE, CPISI_CUBE)

/**
 * @brief 216: APP layer base (codes 216-242).
 *
 * @return 216 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_APP = LAYER_APP × CUBE = 8 × 27 = 216
 *
 * @par Layer Purpose
 *      Application errors — user-facing application issues.
 *      Position +4 in balanced ternary (furthest above center).
 *
 * @see CPISI_LAYER_APP — layer index (8)
 */
#define CPISI_ERR_BASE_APP          CPISI_PRODUCT(CPISI_LAYER_APP, CPISI_CUBE)

/**
 * @brief 243: METADATA base (codes 243-255).
 *
 * @return 243 (integer constant)
 *
 * @par Derivation
 *      ERR_BASE_METADATA = PENTERACT = 3⁵ = 243
 *
 * @par Purpose
 *      Bridge equation completion — 13 slots (243-255) for metadata.
 *      Not layer errors; special codes for error system infrastructure.
 *
 * @par The Bridge Equation
 *      243 (PENTERACT) + 13 (BRIDGE) = 256 (BINARY_FIT)
 *
 * @see CPISI_PENTERACT — the 243 value
 * @see CPISI_BRIDGE — the 13 metadata slots
 */
#define CPISI_ERR_BASE_METADATA     CPISI_PENTERACT

/** @} */ /* end S.3d.3 LAYER BASES */

/** @name S.3d.4 TIER INDICES — 3 tiers per layer
 *  @brief Tier indices for error code construction.
 *
 *  Each layer's 27 codes divide into 3 tiers × 9 positions.
 *  Tier offset = tier_index × FACE = tier_index × 9.
 *
 *  @par Structure
 *       - CORE (0-8):  Fundamental errors — must handle
 *       - EXT (9-17):  Extended errors — common scenarios
 *       - SYS (18-26): System errors — internal/low-level
 *
 *  @{ */

/**
 * @brief 0: CORE tier index (positions 0-8 within layer).
 *
 * @return 0 (integer constant)
 *
 * @par Purpose
 *      Fundamental errors — the essential errors every system must handle.
 *      These are the most common and critical error conditions.
 *
 * @par Offset Calculation
 *      tier_offset = CORE × FACE = 0 × 9 = 0
 *
 * @see CPISI_MAKE_ERROR — uses tier index
 */
#define CPISI_TIER_CORE             0

/**
 * @brief 1: EXT tier index (positions 9-17 within layer).
 *
 * @return 1 (integer constant)
 *
 * @par Purpose
 *      Extended errors — common scenarios beyond the fundamentals.
 *      Resource errors, contention, operational issues.
 *
 * @par Offset Calculation
 *      tier_offset = EXT × FACE = 1 × 9 = 9
 *
 * @see CPISI_MAKE_ERROR — uses tier index
 */
#define CPISI_TIER_EXT              1

/**
 * @brief 2: SYS tier index (positions 18-26 within layer).
 *
 * @return 2 (integer constant)
 *
 * @par Purpose
 *      System errors — internal and low-level system-specific errors.
 *      File operations, internal inconsistencies, debug conditions.
 *
 * @par Offset Calculation
 *      tier_offset = SYS × FACE = 2 × 9 = 18
 *
 * @see CPISI_MAKE_ERROR — uses tier index
 */
#define CPISI_TIER_SYS              2

/** @} */ /* end S.3d.4 TIER INDICES */

/** @name S.3d.5 POSITION SEMANTICS — 9 universal meanings
 *  @brief Position meanings are UNIVERSAL across all layers and tiers.
 *
 *  Position 1 is ALWAYS null/missing in every layer/tier.
 *  Position 3 is ALWAYS bounds/range in every layer/tier.
 *  This enables pattern-based error handling.
 *
 *  @par Pattern-Based Handling
 *       @code
 *       // Check any error for null regardless of layer:
 *       if ((error % 9) == CPISI_POS_NULL) {
 *           // Handle null pointer error
 *       }
 *       @endcode
 *
 *  @{ */

/**
 * @brief 0: Success or no error of this category.
 *
 * @return 0 (integer constant)
 *
 * @par Meaning
 *      The success state. No error condition exists.
 *      Position 0 of every tier is reserved for "no error of this type."
 *
 * @note CPISI_OK uses this position in KERNEL/CORE tier.
 *
 * @see CPISI_OK — the universal success code
 */
#define CPISI_POS_SUCCESS           0

/**
 * @brief 1: Null pointer or missing required item.
 *
 * @return 1 (integer constant)
 *
 * @par Meaning
 *      A required pointer was NULL, or a required item is missing.
 *      Universal across all layers — KERNEL_NULL, HEALTH_NULL, etc.
 *
 * @par Examples
 *      - Function received NULL argument
 *      - Required configuration missing
 *      - Expected object not found
 */
#define CPISI_POS_NULL              1

/**
 * @brief 2: Invalid parameter or malformed data.
 *
 * @return 2 (integer constant)
 *
 * @par Meaning
 *      A parameter is syntactically invalid or data is malformed.
 *      Not NULL, but not valid either.
 *
 * @par Examples
 *      - Invalid enum value
 *      - Malformed string format
 *      - Corrupt data structure
 */
#define CPISI_POS_INVALID           2

/**
 * @brief 3: Out of bounds or range violation.
 *
 * @return 3 (integer constant)
 *
 * @par Meaning
 *      A value exceeds its valid range or an index is out of bounds.
 *      The value exists but is outside acceptable limits.
 *
 * @par Examples
 *      - Array index beyond length
 *      - Health score > 1.0 or < 0.0
 *      - Layer index > 8
 */
#define CPISI_POS_BOUNDS            3

/**
 * @brief 4: Type mismatch or format error.
 *
 * @return 4 (integer constant)
 *
 * @par Meaning
 *      Wrong type or incompatible format.
 *      The value exists and is in range, but is the wrong kind.
 *
 * @par Examples
 *      - Expected integer, got string
 *      - Incompatible version format
 *      - Wrong encoding
 */
#define CPISI_POS_TYPE              4

/**
 * @brief 5: Invalid state or condition not met.
 *
 * @return 5 (integer constant)
 *
 * @par Meaning
 *      Object is in wrong state for requested operation.
 *      Precondition or invariant violation.
 *
 * @par Examples
 *      - Calling read() before open()
 *      - Already initialized
 *      - Connection closed
 */
#define CPISI_POS_STATE             5

/**
 * @brief 6: Initialization or setup failure.
 *
 * @return 6 (integer constant)
 *
 * @par Meaning
 *      Failed to initialize or set up a resource.
 *      The system couldn't reach operational state.
 *
 * @par Examples
 *      - Failed to allocate memory
 *      - Configuration load failed
 *      - Subsystem init failed
 */
#define CPISI_POS_INIT              6

/**
 * @brief 7: Cleanup or teardown failure.
 *
 * @return 7 (integer constant)
 *
 * @par Meaning
 *      Failed to clean up or tear down a resource.
 *      Resource may be leaked or left in inconsistent state.
 *
 * @par Examples
 *      - Failed to close file
 *      - Free failed
 *      - Shutdown incomplete
 */
#define CPISI_POS_CLEANUP           7

/**
 * @brief 8: Unknown or uncategorized error.
 *
 * @return 8 (integer constant)
 *
 * @par Meaning
 *      Error doesn't fit other categories.
 *      Should be rare — most errors should have specific positions.
 *
 * @par When to Use
 *      - Unexpected error from external system
 *      - Placeholder during development
 *      - Truly uncategorizable condition
 *
 * @warning Prefer specific error positions. UNKNOWN is a last resort.
 */
#define CPISI_POS_UNKNOWN           8

/** @} */ /* end S.3d.5 POSITION SEMANTICS */

/** @} */ /* end S.3d ERRORS */

/** @} */ /* end cpisi_types (S.3) */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_protos S.4 PROTOTYPES — Function Declarations
 * @brief    Function prototypes for type operations.
 *
 * PURPOSE: Declare functions before BODY implementations.
 *          Currently reserved — inline functions in BODY.
 *
 * CONTAINS:
 *   - S.4a LIFECYCLE   — create/init/destroy/clone (reserved)
 *   - S.4b ACCESSORS   — get/set/is queries (reserved)
 *   - S.4c OPERATIONS  — verb/reset/validate (reserved)
 *
 * Section order: Lifecycle → Accessors → Operations
 * Flow: allocation → field access → actions
 *
 * Builds FROM: S.3 TYPES
 * Builds TO:   S.5 FILE-LEVEL
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Create/Init/Destroy/Clone [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_types_lifecycle S.4a LIFECYCLE — Create/Init/Destroy/Clone
 * @ingroup cpisi_types_protos
 * @brief    Reserved for lifecycle function prototypes.
 *
 * PURPOSE: Object lifecycle management (create, init, destroy, clone).
 *          Currently reserved — types.h uses inline functions in BODY.
 *
 * Builds FROM: S.4 PROTOTYPES (section context)
 * Builds TO:   S.4b ACCESSORS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Types use inline lifecycle functions in BODY] */

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ACCESSORS — Get/Set/Is Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_types_accessors S.4b ACCESSORS — Get/Set/Is Queries
 * @ingroup cpisi_types_protos
 * @brief    Reserved for accessor function prototypes.
 *
 * PURPOSE: Field access (get_field, set_field, is_state).
 *          Currently reserved — types.h uses inline accessors in BODY.
 *
 * Builds FROM: S.4a LIFECYCLE
 * Builds TO:   S.4c OPERATIONS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Types use inline accessor functions in BODY] */

/** @} */ /* end S.4b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c OPERATIONS — Verb/Reset/Validate [OPERATIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_types_operations S.4c OPERATIONS — Verb/Reset/Validate
 * @ingroup cpisi_types_protos
 * @brief    Reserved for operation function prototypes.
 *
 * PURPOSE: Actions (verb, reset, validate).
 *          Currently reserved — types.h uses inline operations in BODY.
 *
 * Builds FROM: S.4b ACCESSORS
 * Builds TO:   S.5 FILE-LEVEL
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Types use inline operation functions in BODY] */

/** @} */ /* end S.4c OPERATIONS */

/** @} */ /* end cpisi_types_protos (S.4) */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File-Level Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_file S.5 FILE-LEVEL — File-Level Declarations
 * @brief    Reserved — types.h has no file-level state.
 *
 * PURPOSE: File-level variables and state.
 *          Not applicable — types.h is pure type definitions.
 *
 * CONTAINS:
 *   - S.5a VARIABLES      — extern/static variables (reserved)
 *   - S.5b FUNC_TYPES     — callback typedefs (reserved)
 *   - S.5c INFRASTRUCTURE — logger/inspector (reserved)
 *
 * Section order: Variables → Function Types → Infrastructure
 * Flow: state → callbacks → health tracking
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
 * @defgroup cpisi_types_vars S.5a VARIABLES — Extern/Static Variables
 * @ingroup cpisi_types_file
 * @brief    Reserved for file-level variable declarations.
 *
 * PURPOSE: Shared state (extern) and file state (static).
 *          Currently reserved — types.h is pure type definitions.
 *
 * Builds FROM: S.5 FILE-LEVEL (section context)
 * Builds TO:   S.5b FUNC_TYPES
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: No mutable state in header files] */

/** @} */ /* end S.5a VARIABLES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5b FUNC_TYPES — Callback Typedefs [FUNC_TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_types_functypes S.5b FUNC_TYPES — Callback Typedefs
 * @ingroup cpisi_types_file
 * @brief    Reserved for function pointer typedefs.
 *
 * PURPOSE: Callback signatures for hooks and handlers.
 *          Currently reserved — types.h uses inline definitions.
 *
 * Builds FROM: S.5a VARIABLES
 * Builds TO:   S.5c INFRASTRUCTURE
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Types use inline callback definitions] */

/** @} */ /* end S.5b FUNC_TYPES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5c INFRASTRUCTURE — Logger/Inspector [INFRASTRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_types_infra S.5c INFRASTRUCTURE — Logger/Inspector
 * @ingroup cpisi_types_file
 * @brief    Reserved for Rails health tracking infrastructure.
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

/** @} */ /* end cpisi_types_file (S.5) */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_omit S.6 OMISSION — Reserved Section Guide
 * @brief    Documents what sections are reserved and why.
 *
 * PURPOSE: Guide for reserved sections in types.h.
 *
 * Builds FROM: S.5 FILE-LEVEL
 * Builds TO:   BODY block
 *
 * RESERVED SECTIONS:
 *   S.1b INTERNAL       — Foundation file, no internal dependencies
 *   S.1c EXTERNAL       — Standard library only
 *   S.3a ENUMS          — Domain enums in BODY B.2
 *   S.3b STRUCTS        — Domain structs in BODY B.2
 *   S.3c OPAQUE         — CPI-SI prefers transparent types
 *   S.4a LIFECYCLE      — Types use inline lifecycle in BODY
 *   S.4b ACCESSORS      — Types use inline accessors in BODY
 *   S.4c OPERATIONS     — Types use inline operations in BODY
 *   S.5a VARIABLES      — No mutable state in headers
 *   S.5b FUNC_TYPES     — Types use inline callback definitions
 *   S.5c INFRASTRUCTURE — Infrastructure lives in source files
 *
 * ACTIVE SECTIONS:
 *   S.3d ERRORS         — CpisiError type, CPISI_OK, layer bases, tier/position indices
 *                         (Full error codes in types/config/error.h)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/** @} */ /* end S.6 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Type definitions — the actual content.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// PURPOSE: All CPI-SI types live here: enums, structs, inline functions.
///
/// CONTAINS:
///   - B.1 ORG CHART  — Interface overview and type inventory
///   - B.2 TYPES      — Enums, structs, unified lookup tables
///   - B.3 HELPERS    — Validation, conversions, utilities
///   - B.4 ERRORS     — State recovery, DAR cycle, graceful degradation
///   - B.5 PUBLIC API — Accessors, lifecycle, formatting
///   - B.6 OMISSION   — What is reserved and why
///
/// Section order: Org Chart → Types → Helpers → Errors → Public API → Omission
/// Flow: map structure → definitions → validation/conversion → error handling
///       → exported interface → guidance
///
/// Builds FROM: SETUP block (includes, defines, forward declarations)
/// Builds TO:   CLOSING block (summary and synthesis)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @defgroup cpisi_body BODY — CPI-SI Type Implementations
 * @ingroup cpisi
 * @{ */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Organizational Chart [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_org_chart B.1 ORG CHART — Organizational Chart
 * @ingroup cpisi_body
 * @brief    Interface overview and type inventory for types.h.
 *
 * PURPOSE: Map structure — navigate the BODY at a glance.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ BODY ORGANIZATION                                                           │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.2 FOUNDATION TYPES    │ Universal types all domains build on              │
 * │   B.2a Foundation       │ KFactor (3), HebrewZone (5), HebrewState (7)      │
 * │   B.2b Scores           │ ScaleType (6), TernaryBase (4), MultiScore, etc.  │
 * │   B.2c Primitives       │ ATOMIC_SUCCESS/FAILURE/NEUTRAL (trit constants)   │
 * │   B.2d Assessment       │ DetectType (7), Severity (9) — cross-cutting      │
 * │   [Domain Types]        │ health.h, phase.h, log.h (domain ownership)       │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.3 HELPERS             │ Config-driven validation, conversions, utilities  │
 * │   B.3a Validation       │ Helper macros + _valid()/_clamp() per type        │
 * │   B.3b Conversions      │ Hebrew, KFactor, Health, Score, Cube transforms   │
 * │   B.3c Utilities        │ State stepping, health adjustment, zero inits     │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.4 ERROR HANDLING      │ State recovery, DAR cycle, graceful degradation   │
 * │   B.4a State Recovery   │ Default value restoration (_recover() functions)  │
 * │   B.4b DAR Cycle        │ Detect-Assess-Restore cycle (DARResult, phases)   │
 * │   B.4c Graceful Degrade │ Step-wise degradation (_degrade(), _fallback())   │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.5 PUBLIC API          │ Accessors (unified lookup), lifecycle, utilities  │
 * │   B.5a Lifecycle        │ Init/shutdown (reserved for source files)         │
 * │   B.5b Accessors        │ _name(), _emoji(), _parse() via unified lookups   │
 * │   B.5c Format           │ Display formatting (reserved)                     │
 * │   B.5d Utilities        │ Zone colors via hebrew_zone_info() lookup         │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.6 OMISSION GUIDE      │ What is reserved and why                          │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * STRUCTURE:
 *   B.1a Structure — Type organization and inventory
 *   B.1b Flow      — Dependency order (must define before use)
 *   B.1c Counts    — Type count patterns (7-7-7-9)
 *
 * Builds FROM: BODY block (section context)
 * Builds TO:   B.2 TYPE_DEFS (implements what org chart maps)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Type Organization [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_org_structure B.1a STRUCTURE — Type Organization
 * @ingroup cpisi_org_chart
 * @brief    Type organization and inventory.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ B.2 FOUNDATION TYPES (in types.h)                                           │
 * ├──────────────────┬──────────────────────────────────────────────────────────┤
 * │ B.2a FOUNDATION  │ KFactor (3), HebrewZone (5), HebrewState (7) — MASTER     │
 * │ B.2b SCORES      │ ScaleType (6), TernaryBase (4), composites — MASTER      │
 * │ B.2c PRIMITIVES  │ ATOMIC_SUCCESS/FAILURE/NEUTRAL — trit constants          │
 * │ B.2d ASSESSMENT  │ DetectType (7), Severity (9) — cross-cutting             │
 * └──────────────────┴──────────────────────────────────────────────────────────┘
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ DOMAIN TYPES (moved to domain files — domain ownership)                     │
 * ├──────────────────┬──────────────────────────────────────────────────────────┤
 * │ health.h         │ HealthScore, HealthDirection, HealthLevel, etc.          │
 * │ phase.h          │ DARPhase, RestoreResult, DAREvent, DARHistory            │
 * │ config/log.h     │ LogLevel, ActionType — logging infrastructure            │
 * └──────────────────┴──────────────────────────────────────────────────────────┘
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ TYPE INVENTORY                                                              │
 * ├──────────────────┬──────────────────────────────────────────────────────────┤
 * │ ENUMS (6)        │ HebrewState, HebrewZone, KFactor, HealthLevel,           │
 * │                  │ HealthDirection, DARPhase                                │
 * ├──────────────────┼──────────────────────────────────────────────────────────┤
 * │ TYPEDEFS (2)     │ HealthScore (int16_t), TernaryValue (float)              │
 * ├──────────────────┼──────────────────────────────────────────────────────────┤
 * │ STRUCTS (4)      │ MultiScore, TernaryScore, CombinedState, HealthThreshold │
 * └──────────────────┴──────────────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Dependency Order [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_org_flow B.1b FLOW — Dependency Order
 * @ingroup cpisi_org_chart
 * @brief    Dependency order (must define before use).
 *
 * ```
 * HebrewState → HebrewZone → KFactor → HealthScore → HealthLevel
 *      ↓                                    ↓
 * TernaryValue ←──────────────────── MultiScore
 *      ↓                                    ↓
 * TernaryScore ←──────────────────── CombinedState
 *      ↓
 * DARPhase → HealthThreshold
 * ```
 *
 * Implementation: types.h (defines) → cpisi.c (orchestrator)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Type Count Patterns [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_org_counts B.1c COUNTS — Type Count Patterns
 * @ingroup cpisi_org_chart
 * @brief    Type count pattern: 7-7-7-9 (Hebrew/Health/Severity/Log).
 *
 * ┌──────────────────┬───────┬──────────────────────────────────────────────────┐
 * │ Type             │ Count │ Members                                          │
 * ├──────────────────┼───────┼──────────────────────────────────────────────────┤
 * │ Hebrew States    │   7   │ shavar, chaser, ratsah, yashar, tamim, shalem, tov │
 * │ Health Levels    │   7   │ broken, wanting, lacking, even, sound, whole, perfect │
 * │ Severity         │   7   │ (defined in later sections)                      │
 * │ Log Levels       │   9   │ Base9 (3²) for full ternary spectrum             │
 * └──────────────────┴───────┴──────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 TYPES — Foundation Type Definitions [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types_body B.2 TYPES — Foundation Type Definitions
 * @ingroup cpisi_body
 * @brief    Universal types all domains build on.
 *
 * PURPOSE: Foundation type definitions — types.h owns FOUNDATION, domain files
 *          own DOMAIN types.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ B.2 FOUNDATION TYPES — What types.h OWNS                                    │
 * ├─────────────────┬───────────────────────────────────────────────────────────┤
 * │ B.2a Foundation │ KFactor(3), HebrewZone(5), HebrewState(7)                  │
 * │ B.2b Scores     │ ScaleType(6), TernaryBase(4), MultiScore, CombinedState   │
 * │ B.2c Primitives │ ATOMIC_SUCCESS/FAILURE/NEUTRAL — trit constants           │
 * │ B.2d Assessment │ DetectType(7), Severity(9) — cross-cutting measurement    │
 * └─────────────────┴───────────────────────────────────────────────────────────┘
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ DOMAIN TYPES — Moved to domain files (domain ownership)                     │
 * ├─────────────────┬───────────────────────────────────────────────────────────┤
 * │ Health types    │ health.h — HealthScore, HealthLevel, HealthDirection      │
 * │ DAR types       │ phase.h — DARPhase, RestoreResult, DAREvent, DARHistory   │
 * │ Log types       │ config/log.h — LogLevel, ActionType                       │
 * └─────────────────┴───────────────────────────────────────────────────────────┘
 *
 * Include chain IS the architecture:
 *   types.h (foundation) → config/log.h → health.h → phase.h
 *
 * All values derive from S.2 ternary scale anchors (CPISI_TERNARY_*)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2a FOUNDATION — Mental Construct Core [FOUNDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_foundation B.2a FOUNDATION — Mental Construct Core
 * @ingroup cpisi_types
 * @brief    MODE (9) × DIRECTION (3) × POSITION (7) → ZONE (5)
 *
 * WHY: The mental construct requires a coordinate system for navigation.
 *      These types define WHERE you are (position), WHERE you're going (direction),
 *      and WHAT context you're in (mode/layer).
 *
 * SCOPE: Universal foundation — all CPI-SI code uses these types.
 *
 * CONTAINS:
 *   - B.2a.1 KFactor (3)     — Direction/intent (retreat/maintain/advance)
 *   - B.2a.2 HebrewZone (5)  — Collapsed position (5 zones from 7 states)
 *   - B.2a.3 HebrewState (7) — Full position (7 Hebrew cognitive states)
 *
 * REFERENCES (defined in SETUP):
 *   - CpisiLayer (9)         — System context (KERNEL..APP) — see S.2e.2 LAYER
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ COGNITIVE ARCHITECTURE = MODE (9) × DIRECTION (3) × POSITION (7) → ZONE (5)│
 * ├─────────┬───────┬───────────────────────────────────────────────────────────┤
 * │ Layer   │ Count │ Purpose                                                   │
 * ├─────────┼───────┼───────────────────────────────────────────────────────────┤
 * │ Mode    │   9   │ CpisiLayer — system context (KERNEL..APP)     [S.2a.2]    │
 * │ Direction│  3   │ KFactor — movement intent (−/◆/+)                         │
 * │ Position│   7   │ HebrewState — ternary scale (shavar..tov)                 │
 * │ Zone    │   5   │ HebrewZone — collapsed view (7→5)                         │
 * ├─────────┼───────┼───────────────────────────────────────────────────────────┤
 * │ Decision│   2   │ Door — choice point active (yes/no)                       │
 * │ Choice  │ 0/1   │ Key — selection made or not                               │
 * │ Weight  │   3   │ Key weight felt BEFORE choosing                           │
 * └─────────┴───────┴───────────────────────────────────────────────────────────┘
 *
 * Dependency order: KFactor(3) → HebrewZone(5) → HebrewState(7) → Unified Info
 * Config source: bereshit/word/core/os/health/health.toml
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a.1 KFACTOR — Direction/Intent (3 values) [KFACTOR]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_foundation_kfactor B.2a.1 KFACTOR — Direction/Intent
 * @ingroup cpisi_foundation
 * @brief    3-key ternary direction — the "three keys" at each door.
 *
 * WHY: Every choice has direction. K-factor is INTENT — where you're heading,
 *      not where you are. Weight is felt BEFORE choosing — the gravity of each key.
 *
 * SCOPE: Used throughout CPI-SI for directional decisions and state transitions.
 *
 * ┌─────┬───────┬──────────────────────────┬──────────────────────────────────┐
 * │ Key │ Value │ Direction                │ Scripture                        │
 * ├─────┼───────┼──────────────────────────┼──────────────────────────────────┤
 * │  -  │  -1   │ Retreat/completion       │ "Return unto me" — Malachi 3:7   │
 * │  ◆  │   0   │ Maintain/balance         │ "Stand fast" — Galatians 5:1     │
 * │  +  │  +1   │ Advance/growth           │ "Go ye therefore" — Matthew 28:19│
 * └─────┴───────┴──────────────────────────┴──────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief 3-key ternary direction — the "three keys" at each choice point.
 *
 * KFactor represents INTENT, not position. It's the direction you're heading,
 * not where you currently are. Weight is felt BEFORE choosing — heavier keys
 * require more deliberation.
 *
 * Values form balanced ternary: {-1, 0, +1} summing to 0.
 */
typedef enum KFactor {
    K_RETREAT   = -1,   /**< Toward completion/foundation — "Return unto me" (Mal 3:7) */
    K_MAINTAIN  =  0,   /**< Holding position/balance — "Stand fast" (Gal 5:1) [DEFAULT] */
    K_ADVANCE   = +1,   /**< Toward growth/expansion — "Go ye therefore" (Matt 28:19) */
} KFactor;

/** @name KFactor Bounds — Range limits and count
 *  @brief Defines the valid range for KFactor values.
 *  @{ */
#define K_FACTOR_MIN     K_RETREAT      /**< Lower bound: -1 (retreat) */
#define K_FACTOR_MAX     K_ADVANCE      /**< Upper bound: +1 (advance) */
#define K_FACTOR_COUNT   (K_FACTOR_MAX - K_FACTOR_MIN + 1)  /**< Total values: 3 */
#define K_FACTOR_DEFAULT K_MAINTAIN     /**< Default: 0 (maintain) — center is safe */
/** @} */ /* end KFactor Bounds */

/** @name KFactor Aliases — Semantic naming for ternary operations
 *  @brief Alternative names mapping to charge/polarity semantics.
 *  @{ */
#define K_NEGATIVE  K_RETREAT   /**< Negative charge/direction (-1) */
#define K_NEUTRAL   K_MAINTAIN  /**< Neutral charge/center (0) */
#define K_POSITIVE  K_ADVANCE   /**< Positive charge/direction (+1) */
/** @} */ /* end KFactor Aliases */

/**
 * @brief KFactor information record — unified lookup for all K-factor metadata.
 *
 * Each KFactor value maps to a complete information record for consistent
 * access to ternary value, display strings, and cognitive weight.
 */
typedef struct KFactorInfo {
    float       value;      /**< Ternary value: -1.0 (retreat), 0.0 (maintain), +1.0 (advance) */
    const char* name;       /**< Lowercase name: "retreat", "maintain", "advance" */
    const char* symbol;     /**< Vertical direction: ↓ (down), ◆ (diamond), ↑ (up) */
    const char* arrow;      /**< Horizontal direction: ← (left), • (dot), → (right) */
    const char* weight;     /**< Cognitive weight: "very heavy", "heavy", "moderate" */
} KFactorInfo;

/**
 * @brief Unified lookup table: KFactor → all metadata.
 *
 * Array is indexed by (k + 1) to convert signed KFactor [-1, 0, +1] to
 * array index [0, 1, 2]. Use k_factor_info() for safe access.
 *
 * Weight interpretation: Retreat decisions carry more gravity (harder to
 * undo) while advance is "moderate" (easier to course-correct).
 */
static const KFactorInfo K_FACTOR_INFO[K_FACTOR_COUNT] = {
    { -1.0f, "retreat",  "↓", "←", "very heavy" },  /**< [0] K_RETREAT: retreat info */
    {  0.0f, "maintain", "◆", "•", "heavy"      },  /**< [1] K_MAINTAIN: maintain info */
    { +1.0f, "advance",  "↑", "→", "moderate"   },  /**< [2] K_ADVANCE: advance info */
};

/**
 * @brief Safe lookup: KFactor → KFactorInfo pointer.
 *
 * Converts signed KFactor value to array index and returns pointer to
 * the corresponding information record.
 *
 * @param k KFactor value: K_RETREAT (-1), K_MAINTAIN (0), or K_ADVANCE (+1).
 * @return Pointer to KFactorInfo record. Never NULL for valid input.
 *
 * @note No bounds checking — caller must ensure valid KFactor.
 *       For untrusted input, validate with: (k >= K_FACTOR_MIN && k <= K_FACTOR_MAX)
 */
static inline const KFactorInfo* k_factor_info(KFactor k) {
    return &K_FACTOR_INFO[k + 1];
}

/** @} */ /* end B.2a.1 KFACTOR */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a.2 HEBREWZONE — Collapsed Position View (5 zones) [HEBREWZONE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_foundation_hebrewzone B.2a.2 HEBREWZONE — Collapsed Position
 * @ingroup cpisi_foundation
 * @brief    5-zone collapsed view of 7 HebrewStates (7→5).
 *
 * WHY: Sometimes you need coarse-grained assessment. HebrewZone collapses
 *      the 7 states into 5 zones, ignoring k-factor direction.
 *
 * SCOPE: Used for quick status assessment, UI color coding, zone-based logic.
 *
 * ┌───────────┬─────────┬───────────────┬─────────────────────────────────────┐
 * │ Zone      │ Ternary │ States (7→5)  │ Color/Meaning                       │
 * ├───────────┼─────────┼───────────────┼─────────────────────────────────────┤
 * │ CRISIS    │ -1.0    │ shavar        │ Red — system failure                │
 * │ WARNING   │ -0.5    │ chaser, ratsah│ Yellow — needs attention            │
 * │ BALANCED  │  0.0    │ yashar        │ Green — normal [CENTER]             │
 * │ HEALTHY   │ +0.5    │ tamim, shalem │ Blue — thriving                     │
 * │ EXCELLENT │ +1.0    │ tov           │ Gold — peak performance             │
 * └───────────┴─────────┴───────────────┴─────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief 5-zone collapsed view of Hebrew cognitive states (7→5).
 *
 * HebrewZone provides coarse-grained assessment by collapsing the 7 HebrewStates
 * into 5 zones. The k-factor direction is ignored — only position matters.
 * Used for quick status checks, UI color coding, and zone-based thresholds.
 *
 * Mapping: shavar→CRISIS, {chaser,ratsah}→WARNING, yashar→BALANCED,
 *          {tamim,shalem}→HEALTHY, tov→EXCELLENT
 */
typedef enum HebrewZone {
    ZONE_CRISIS    = 0,   /**< shavar (broken) — red, ternary: -1.0 — system failure */
    ZONE_WARNING   = 1,   /**< chaser/ratsah (lacking/wanting) — yellow, ternary: -0.5 */
    ZONE_BALANCED  = 2,   /**< yashar (even) — green, ternary: 0.0 — normal [CENTER] */
    ZONE_HEALTHY   = 3,   /**< tamim/shalem (sound/whole) — blue, ternary: +0.5 — thriving */
    ZONE_EXCELLENT = 4,   /**< tov (perfect) — gold, ternary: +1.0 — peak performance */
} HebrewZone;

/** @name HebrewZone Bounds — Range limits and count
 *  @brief Defines the valid range for HebrewZone values.
 *  @{ */
#define ZONE_MIN     ZONE_CRISIS     /**< Lower bound: 0 (crisis) */
#define ZONE_MAX     ZONE_EXCELLENT  /**< Upper bound: 4 (excellent) */
#define ZONE_COUNT   (ZONE_MAX + 1)  /**< Total zones: 5 */
#define ZONE_DEFAULT ZONE_BALANCED   /**< Default: 2 (balanced) — center is normal */
/** @} */ /* end HebrewZone Bounds */

/**
 * @brief HebrewZone information record — unified lookup for zone metadata.
 *
 * Each HebrewZone maps to display information including ternary value,
 * name string, ANSI color code, and emoji for visual representation.
 */
typedef struct HebrewZoneInfo {
    float       value;      /**< Representative ternary: -1.0, -0.5, 0.0, +0.5, +1.0 */
    const char* name;       /**< Lowercase name: "crisis", "warning", "balanced", etc. */
    const char* color;      /**< ANSI escape code: "\033[31m" (red), "\033[33m" (yellow), etc. */
    const char* emoji;      /**< Visual indicator: 🔴, 🟡, 🟢, 🔵 */
} HebrewZoneInfo;

/**
 * @brief Unified lookup table: HebrewZone → all metadata.
 *
 * Array uses designated initializers for clarity and safety.
 * Direct indexing: HEBREW_ZONE_INFO[zone] returns info for that zone.
 *
 * Color semantics: Red=crisis, Yellow=warning, Green=normal, Blue=healthy, Gold=excellent.
 */
static const HebrewZoneInfo HEBREW_ZONE_INFO[ZONE_COUNT] = {
    [ZONE_CRISIS]    = { -1.0f, "crisis",    "\033[31m", "🔴" },  /**< Red: system failure */
    [ZONE_WARNING]   = { -0.5f, "warning",   "\033[33m", "🟡" },  /**< Yellow: needs attention */
    [ZONE_BALANCED]  = {  0.0f, "balanced",  "\033[32m", "🟢" },  /**< Green: normal [CENTER] */
    [ZONE_HEALTHY]   = { +0.5f, "healthy",   "\033[34m", "🔵" },  /**< Blue: thriving */
    [ZONE_EXCELLENT] = { +1.0f, "excellent", "\033[93m", "🟡" },  /**< Gold: peak performance */
};

/**
 * @brief Safe lookup: HebrewZone → HebrewZoneInfo pointer.
 *
 * Returns pointer to the information record for the given zone.
 *
 * @param z HebrewZone value: ZONE_CRISIS (0) through ZONE_EXCELLENT (4).
 * @return Pointer to HebrewZoneInfo record. Never NULL for valid input.
 *
 * @note No bounds checking — caller must ensure valid HebrewZone.
 *       For untrusted input, validate with: (z >= ZONE_MIN && z <= ZONE_MAX)
 */
static inline const HebrewZoneInfo* hebrew_zone_info(HebrewZone z) {
    return &HEBREW_ZONE_INFO[z];
}

/** @} */ /* end B.2a.2 HEBREWZONE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a.3 HEBREWSTATE — 7-State Hebrew Cognitive System (MASTER) [HEBREWSTATE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_foundation_hebrewstate B.2a.3 HEBREWSTATE — 7-State Cognitive System
 * @ingroup cpisi_foundation
 * @brief    7-state Hebrew cognitive system — MASTER lookup unifying 7→5→3.
 *
 * WHY: This is the master cognitive state system. Each state carries its own
 *      k-factor and zone, enabling single-lookup unified access. The 7 states
 *      collapse to 5 zones (ignoring k), and the 5 zones collapse to 3 regions
 *      (negative/center/positive).
 *
 * SCOPE: Core cognitive state throughout CPI-SI. Health, phase, and all
 *        state-based operations reference HebrewState.
 *
 * ┌─────────┬─────────┬─────────┬──────────┬───────────┬────────────────────────┐
 * │ State   │ Hebrew  │ Ternary │ K-Factor │ Zone      │ Scripture              │
 * ├─────────┼─────────┼─────────┼──────────┼───────────┼────────────────────────┤
 * │ shavar  │ שָׁבַר   │ -1.0    │ n/a      │ CRISIS    │ Daniel 5:27            │
 * │ chaser  │ חָסֵר   │ -0.5    │ k=-1     │ WARNING   │ Proverbs 6:32          │
 * │ ratsah  │ רָצָה   │ -0.5    │ k=+1     │ WARNING   │ Psalm 51:16            │
 * │ yashar  │ יָשָׁר   │  0.0    │ n/a      │ BALANCED  │ Job 31:6    [CENTER]   │
 * │ tamim   │ תָּמִים  │ +0.5    │ k=-1     │ HEALTHY   │ Psalm 19:7             │
 * │ shalem  │ שָׁלֵם   │ +0.5    │ k=+1     │ HEALTHY   │ Deuteronomy 25:15      │
 * │ tov     │ טוֹב    │ +1.0    │ n/a      │ EXCELLENT │ Genesis 1:31           │
 * └─────────┴─────────┴─────────┴──────────┴───────────┴────────────────────────┘
 *
 * MASTER: Each state embeds its k-factor and zone — one lookup, all data.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief 7-state Hebrew cognitive system — MASTER type.
 *
 * The 7 HebrewStates form a complete cognitive state system. Edge states
 * (shavar, yashar, tov) have no inherent k-factor direction. Intermediate
 * states carry k-factor: chaser/tamim are k=-1 (retreat), ratsah/shalem
 * are k=+1 (advance).
 *
 * Values are 0-indexed for direct array access.
 */
typedef enum HebrewState {
    HEBREW_SHAVAR = 0,    /**< Broken (שָׁבַר) — crisis edge, ternary: -1.0 (Dan 5:27) */
    HEBREW_CHASER = 1,    /**< Lacking (חָסֵר) — regressing, ternary: -0.5, k=-1 (Prov 6:32) */
    HEBREW_RATSAH = 2,    /**< Wanting (רָצָה) — hungry, ternary: -0.5, k=+1 (Ps 51:16) */
    HEBREW_YASHAR = 3,    /**< Even (יָשָׁר) — balanced [CENTER], ternary: 0.0 (Job 31:6) */
    HEBREW_TAMIM  = 4,    /**< Sound (תָּמִים) — consolidating, ternary: +0.5, k=-1 (Ps 19:7) */
    HEBREW_SHALEM = 5,    /**< Whole (שָׁלֵם) — expanding, ternary: +0.5, k=+1 (Deut 25:15) */
    HEBREW_TOV    = 6,    /**< Perfect (טוֹב) — excellent edge, ternary: +1.0 (Gen 1:31) */
} HebrewState;

/** @name HebrewState Bounds — Range limits and count
 *  @brief Defines the valid range for HebrewState values.
 *  @{ */
#define HEBREW_STATE_MIN     HEBREW_SHAVAR   /**< Lower bound: 0 (shavar/broken) */
#define HEBREW_STATE_MAX     HEBREW_TOV      /**< Upper bound: 6 (tov/perfect) */
#define HEBREW_STATE_COUNT   (HEBREW_STATE_MAX + 1)  /**< Total states: 7 */
#define HEBREW_STATE_DEFAULT HEBREW_YASHAR   /**< Default: 3 (yashar/even) — center is safe */
/** @} */ /* end HebrewState Bounds */

/**
 * @brief HebrewState MASTER information record — embeds k-factor + zone.
 *
 * The MASTER record unifies all HebrewState metadata in one structure,
 * enabling single-lookup access to value, direction, zone, and display info.
 * This is the authoritative source — other lookups derive from this.
 */
typedef struct HebrewStateInfo {
    float       value;      /**< Ternary position: -1.0, -0.5, 0.0, +0.5, +1.0 */
    KFactor     kfactor;    /**< Inherent direction: K_RETREAT (-1), K_MAINTAIN (0), K_ADVANCE (+1) */
    HebrewZone  zone;       /**< Collapsed zone (7→5): CRISIS, WARNING, BALANCED, HEALTHY, EXCELLENT */
    const char* name;       /**< Short lowercase name: "shavar", "chaser", "ratsah", etc. */
    const char* fullname;   /**< Display name with meaning: "shavar (broken)", etc. */
    const char* emoji;      /**< Visual symbol: 💔, 🩹, 🔥, ⚖️, 🛡️, 💚, 👑 */
    const char* hebrew;     /**< Hebrew script: שָׁבַר, חָסֵר, רָצָה, יָשָׁר, תָּמִים, שָׁלֵם, טוֹב */
} HebrewStateInfo;

/**
 * @brief MASTER lookup table: HebrewState → all metadata (7→5→3 unified).
 *
 * Array uses designated initializers for clarity and safety.
 * Direct indexing: HEBREW_STATE_INFO[state] returns all info for that state.
 *
 * Edge states (shavar, yashar, tov) use K_MAINTAIN because they have no
 * inherent direction — they're positions, not movements.
 */
static const HebrewStateInfo HEBREW_STATE_INFO[HEBREW_STATE_COUNT] = {
    /*          value   kfactor     zone           name      fullname              emoji  hebrew  */
    [HEBREW_SHAVAR] = { -1.0f, K_MAINTAIN, ZONE_CRISIS,   "shavar", "shavar (broken)",   "💔", "שָׁבַר" },
    [HEBREW_CHASER] = { -0.5f, K_RETREAT,  ZONE_WARNING,  "chaser", "chaser (lacking)",  "🩹", "חָסֵר" },
    [HEBREW_RATSAH] = { -0.5f, K_ADVANCE,  ZONE_WARNING,  "ratsah", "ratsah (wanting)",  "🔥", "רָצָה" },
    [HEBREW_YASHAR] = {  0.0f, K_MAINTAIN, ZONE_BALANCED, "yashar", "yashar (even)",     "⚖️", "יָשָׁר" },
    [HEBREW_TAMIM]  = { +0.5f, K_RETREAT,  ZONE_HEALTHY,  "tamim",  "tamim (sound)",     "🛡️", "תָּמִים"},
    [HEBREW_SHALEM] = { +0.5f, K_ADVANCE,  ZONE_HEALTHY,  "shalem", "shalem (whole)",    "💚", "שָׁלֵם" },
    [HEBREW_TOV]    = { +1.0f, K_MAINTAIN, ZONE_EXCELLENT,"tov",    "tov (perfect)",     "👑", "טוֹב"  },
};

/** @name HebrewState Accessors — Single-lookup into MASTER table
 *  @brief Inline functions providing type-safe access to HEBREW_STATE_INFO.
 *  @{ */

/**
 * @brief Validate HebrewState is within bounds.
 * @param s HebrewState value to validate.
 * @return true if valid (0-6), false otherwise.
 */
static inline bool hebrew_state_valid(HebrewState s) {
    return (s >= HEBREW_STATE_MIN && s <= HEBREW_STATE_MAX);
}

/**
 * @brief Get complete info record for HebrewState.
 * @param s HebrewState value: HEBREW_SHAVAR (0) through HEBREW_TOV (6).
 * @return Pointer to HebrewStateInfo record. Never NULL for valid input.
 * @note No bounds checking — use hebrew_state_valid() first for untrusted input.
 */
static inline const HebrewStateInfo* hebrew_state_info(HebrewState s) {
    return &HEBREW_STATE_INFO[s];
}

/**
 * @brief Get ternary value for HebrewState.
 * @param s HebrewState value.
 * @return Ternary float: -1.0, -0.5, 0.0, +0.5, or +1.0.
 */
static inline float hebrew_state_value(HebrewState s) {
    return HEBREW_STATE_INFO[s].value;
}

/**
 * @brief Get inherent k-factor for HebrewState.
 * @param s HebrewState value.
 * @return KFactor: K_RETREAT, K_MAINTAIN, or K_ADVANCE.
 */
static inline KFactor hebrew_state_kfactor(HebrewState s) {
    return HEBREW_STATE_INFO[s].kfactor;
}

/**
 * @brief Get collapsed zone for HebrewState (7→5).
 * @param s HebrewState value.
 * @return HebrewZone: ZONE_CRISIS through ZONE_EXCELLENT.
 */
static inline HebrewZone hebrew_state_zone(HebrewState s) {
    return HEBREW_STATE_INFO[s].zone;
}

/**
 * @brief Get short lowercase name for HebrewState.
 * @param s HebrewState value.
 * @return Name string: "shavar", "chaser", "ratsah", etc.
 */
static inline const char* hebrew_state_name(HebrewState s) {
    return HEBREW_STATE_INFO[s].name;
}

/**
 * @brief Get display name with meaning for HebrewState.
 * @param s HebrewState value.
 * @return Full name: "shavar (broken)", "chaser (lacking)", etc.
 */
static inline const char* hebrew_state_fullname(HebrewState s) {
    return HEBREW_STATE_INFO[s].fullname;
}

/**
 * @brief Get visual emoji for HebrewState.
 * @param s HebrewState value.
 * @return Emoji string: 💔, 🩹, 🔥, ⚖️, 🛡️, 💚, 👑.
 */
static inline const char* hebrew_state_emoji(HebrewState s) {
    return HEBREW_STATE_INFO[s].emoji;
}

/**
 * @brief Get Hebrew script for HebrewState.
 * @param s HebrewState value.
 * @return Hebrew string: שָׁבַר, חָסֵר, רָצָה, יָשָׁר, תָּמִים, שָׁלֵם, טוֹב.
 */
static inline const char* hebrew_state_hebrew(HebrewState s) {
    return HEBREW_STATE_INFO[s].hebrew;
}

/** @} */ /* end HebrewState Accessors */

/** @} */ /* end B.2a.3 HEBREWSTATE */

/** @} */ /* end B.2a FOUNDATION — cpisi_foundation */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2b SCORES — Score Representations [SCORES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_scores B.2b SCORES — Score Representations
 * @ingroup cpisi_types
 * @brief    Score representations — decimal scales, ternary scales, combined state.
 *
 * WHY: CPI-SI needs multi-scale representation. Different contexts need different
 *      granularity: fine precision for internal calculation, coarse for display,
 *      ternary for cognitive operations.
 *
 * SCOPE: Universal scoring throughout CPI-SI — health metrics, phase calculations,
 *        display conversions.
 *
 * CONTAINS:
 *   - B.2b.1 ScaleType (6)    — Decimal scales: 100/50/20/10/5/3
 *   - B.2b.2 MultiScore       — Struct holding value at all 6 decimal scales
 *   - B.2b.3 TernaryBase (4)  — Ternary scales: 3/9/27/81 (powers of 3)
 *   - B.2b.4 TernaryScore     — Struct holding value at all 4 ternary scales
 *   - B.2b.5 CombinedState    — HebrewState + KFactor + TernaryValue unified
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ SCORE ARCHITECTURE = DECIMAL (6) + TERNARY (4) + COMBINED                  │
 * ├─────────┬───────┬───────────────────────────────────────────────────────────┤
 * │ Decimal │   6   │ ScaleType — 100/50/20/10/5/3 with unified lookup          │
 * │ Ternary │   4   │ TernaryBase — 3/9/27/81 (powers of 3) with lookup         │
 * │ Compose │ struct│ MultiScore, TernaryScore — hold values at all scales      │
 * │ Combined│ struct│ CombinedState — HebrewState + KFactor + TernaryValue      │
 * └─────────┴───────┴───────────────────────────────────────────────────────────┘
 *
 * Builds on: B.2a Foundation (HebrewState provides zone mapping)
 * Used by:   health.h, phase.h (domain types reference these scales)
 * Config source: bereshit/word/core/os/health/health.toml
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b.1 SCALETYPE — 6 Decimal Scale Types [SCALETYPE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_scores_scaletype B.2b.1 SCALETYPE — Decimal Scale Types
 * @ingroup cpisi_scores
 * @brief    6 decimal scales for human-readable representation.
 *
 * WHY: Different contexts need different granularity. Fine precision for
 *      internal calculations (base100), coarse for quick display (base20),
 *      zone index for HebrewZone mapping (base5), trit for sign-only (base3).
 *
 * SCOPE: Conversions, display formatting, multi-scale storage.
 *
 * ┌───────┬──────────────┬─────────────┬────────────────────────────────────────┐
 * │ Base  │ Range        │ Divisor     │ Use Case                               │
 * ├───────┼──────────────┼─────────────┼────────────────────────────────────────┤
 * │ 100   │ -100 to +100 │ 1           │ Fine precision, canonical              │
 * │ 50    │ -50 to +50   │ 2           │ Medium precision                       │
 * │ 20    │ -20 to +20   │ 5           │ Coarse, quick display                  │
 * │ 10    │ -10 to +10   │ 10          │ Compact display                        │
 * │ 5     │ 0 to 4       │ 20          │ Hebrew zone INDEX (special)            │
 * │ 3     │ -1, 0, +1    │ 33          │ Pure trit/sign                         │
 * └───────┴──────────────┴─────────────┴────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief 6 decimal scale types for multi-scale representation.
 *
 * ScaleType enumerates the 6 supported decimal scales. Each scale has a
 * different range and granularity. BASE100 is canonical (full precision);
 * others are derived by division.
 *
 * Values are 0-indexed for direct array access into SCALE_TYPE_INFO.
 */
typedef enum ScaleType {
    SCALE_BASE100 = 0,   /**< Fine: -100 to +100, divisor 1 — canonical, full precision */
    SCALE_BASE50  = 1,   /**< Medium: -50 to +50, divisor 2 — moderate granularity */
    SCALE_BASE20  = 2,   /**< Coarse: -20 to +20, divisor 5 — quick display */
    SCALE_BASE10  = 3,   /**< Compact: -10 to +10, divisor 10 — minimal display */
    SCALE_BASE5   = 4,   /**< Zone: 0-4, divisor 20 — HebrewZone index (special: unsigned) */
    SCALE_BASE3   = 5,   /**< Trit: -1,0,+1, divisor 33 — pure sign only */
} ScaleType;

/** @name ScaleType Bounds
 *  @brief Total count of decimal scales.
 *  @{ */
#define SCALE_TYPE_COUNT (6)  /**< Total scales: 6 (base100, base50, base20, base10, base5, base3) */
/** @} */ /* end ScaleType Bounds */

/**
 * @brief ScaleType information record — unified lookup for scale metadata.
 *
 * Contains all scale metadata in one struct for single-lookup access.
 * Includes base value, range limits, divisor for conversion, and display names.
 */
typedef struct ScaleTypeInfo {
    int16_t     base;       /**< Scale base: 100, 50, 20, 10, 5, 3 */
    int16_t     min;        /**< Minimum value at this scale */
    int16_t     max;        /**< Maximum value at this scale */
    int16_t     divisor;    /**< Divisor to convert from base100: value / divisor */
    const char* name;       /**< Scale name: "base100", "base50", etc. */
    const char* use_case;   /**< Brief description: "canonical", "medium", etc. */
} ScaleTypeInfo;

/**
 * @brief MASTER lookup table: ScaleType → all metadata.
 *
 * Array uses designated initializers for clarity and safety.
 * Direct indexing: SCALE_TYPE_INFO[scale] returns info for that scale.
 *
 * Conversion formula: target_value = base100_value / info->divisor
 */
static const ScaleTypeInfo SCALE_TYPE_INFO[SCALE_TYPE_COUNT] = {
    [SCALE_BASE100] = { 100, -100, +100,  1, "base100", "canonical"   },  /**< Full precision */
    [SCALE_BASE50]  = {  50,  -50,  +50,  2, "base50",  "medium"      },  /**< Half precision */
    [SCALE_BASE20]  = {  20,  -20,  +20,  5, "base20",  "coarse"      },  /**< Fifth precision */
    [SCALE_BASE10]  = {  10,  -10,  +10, 10, "base10",  "compact"     },  /**< Tenth precision */
    [SCALE_BASE5]   = {   5,    0,    4, 20, "base5",   "zone index"  },  /**< Zone mapping */
    [SCALE_BASE3]   = {   3,   -1,   +1, 33, "base3",   "trit/sign"   },  /**< Sign only */
};

/**
 * @brief Safe lookup: ScaleType → ScaleTypeInfo pointer.
 *
 * Returns pointer to the information record for the given scale.
 * Defaults to BASE100 if scale is out of range.
 *
 * @param scale ScaleType enum value: SCALE_BASE100 (0) through SCALE_BASE3 (5).
 * @return Pointer to ScaleTypeInfo record. Never NULL.
 *
 * @note Bounds-checked: invalid scale defaults to SCALE_BASE100.
 */
static inline const ScaleTypeInfo* scale_type_info(ScaleType scale) {
    if (scale < 0 || scale >= SCALE_TYPE_COUNT) scale = SCALE_BASE100;
    return &SCALE_TYPE_INFO[scale];
}

/**
 * @brief Convert base100 value to target scale.
 *
 * Performs integer division by the scale's divisor. Truncates toward zero.
 *
 * @param base100 Canonical value (-100 to +100).
 * @param scale   Target scale type.
 * @return Value at target scale (truncated).
 *
 * @note Uses scale_type_info() for bounds-safe lookup.
 */
static inline int16_t scale_convert(int16_t base100, ScaleType scale) {
    const ScaleTypeInfo* info = scale_type_info(scale);
    return base100 / info->divisor;
}

/** @} */ /* end B.2b.1 SCALETYPE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b.2 MULTISCORE — Composite Struct for All 6 Decimal Scales [MULTISCORE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_scores_multiscore B.2b.2 MULTISCORE — All Decimal Scales
 * @ingroup cpisi_scores
 * @brief    Struct holding the same semantic value at all 6 decimal granularities.
 *
 * WHY: Rather than converting on-demand, pre-compute all scale representations.
 *      This trades storage (6 bytes) for computation (no runtime division).
 *
 * SCOPE: Health metrics, phase calculations, any context needing multi-scale access.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Multi-scale score representation — all 6 decimal scales in one struct.
 *
 * Stores the same semantic value at all 6 granularities simultaneously.
 * base100 is canonical (authoritative); others are derived conversions.
 * All scales share the same semantic center: yashar = 0.
 *
 * Memory: 6 bytes (6 × int8_t). May be padded depending on struct packing.
 *
 * @see ScaleType for individual scale definitions.
 * @see scale_convert() for conversion formula.
 */
typedef struct MultiScore {
    int8_t  base100;    /**< Canonical: -100 to +100 (full precision) */
    int8_t  base50;     /**< Medium: -50 to +50 (divisor 2) */
    int8_t  base20;     /**< Coarse: -20 to +20 (divisor 5) */
    int8_t  base10;     /**< Compact: -10 to +10 (divisor 10) */
    int8_t  base5;      /**< Zone index: 0-4 (divisor 20, unsigned) */
    int8_t  base3;      /**< Trit: -1, 0, +1 (sign only, divisor 33) */
} MultiScore;

/** @name MultiScore Helpers
 *  @brief Zero initializer and construction functions.
 *  @{ */

/**
 * @brief Zero/center MultiScore (yashar at all scales).
 *
 * Designated initializer for neutral center state.
 * All scales at 0 except base5 which is ZONE_BALANCED (center zone index).
 */
#define MULTI_SCORE_ZERO { \
    .base100 = 0, \
    .base50  = 0, \
    .base20  = 0, \
    .base10  = 0, \
    .base5   = ZONE_BALANCED,  /* center zone index */ \
    .base3   = 0               /* center trit */ \
}

/**
 * @brief Create MultiScore from base100 value.
 *
 * Computes all 6 scale values from the canonical base100 value.
 * Uses integer division (truncation toward zero).
 *
 * @param base100 Canonical value (-100 to +100).
 * @return MultiScore with all scales populated.
 *
 * @note base5 maps -100..+100 to 0..4 (zone index).
 * @note base3 uses threshold of ±10 for sign determination.
 */
static inline MultiScore multi_score_from_base100(int16_t base100) {
    MultiScore s;
    s.base100 = (int8_t)base100;
    s.base50  = (int8_t)(base100 / 2);
    s.base20  = (int8_t)(base100 / 5);
    s.base10  = (int8_t)(base100 / 10);
    s.base5   = (int8_t)((base100 + 100) / 50);  /* map to 0-4 */
    s.base3   = (int8_t)(base100 > 10 ? 1 : (base100 < -10 ? -1 : 0));
    return s;
}

/** @} */ /* end MultiScore Helpers */

/** @} */ /* end B.2b.2 MULTISCORE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b.3 TERNARYBASE — 4 Ternary Scale Types (Powers of 3) [TERNARYBASE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_scores_ternarybase B.2b.3 TERNARYBASE — Ternary Scale Types
 * @ingroup cpisi_scores
 * @brief    4 ternary scales (powers of 3) for cube and trit operations.
 *
 * WHY: Ternary math operates in powers of 3. Different operations need different
 *      precision: raw trit (base3), pairs (base9), full cube (base27),
 *      fine resolution (base81).
 *
 * SCOPE: Mental construct navigation, cube operations, ternary arithmetic.
 *
 * ┌───────┬───────┬──────────────┬───────┬──────────────────────────────────────┐
 * │ Base  │ Power │ Range        │ Trits │ Use Case                             │
 * ├───────┼───────┼──────────────┼───────┼──────────────────────────────────────┤
 * │ 3     │ 3^1   │ -1, 0, +1    │ 1     │ Raw trit, k-factor                   │
 * │ 9     │ 3^2   │ -4 to +4     │ 2     │ Trit pair                            │
 * │ 27    │ 3^3   │ -13 to +13   │ 3     │ Mental construct cube (27 positions) │
 * │ 81    │ 3^4   │ -40 to +40   │ 4     │ Fine ternary resolution              │
 * └───────┴───────┴──────────────┴───────┴──────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief 4 ternary scale types (powers of 3).
 *
 * TernaryBase enumerates the 4 supported ternary scales. Each scale is a
 * power of 3 representing different levels of precision. BASE27 is the
 * mental construct cube (3³ = 27 positions); BASE3 is the raw trit.
 *
 * Values are 0-indexed for direct array access into TERNARY_BASE_INFO.
 */
typedef enum TernaryBase {
    TERNARY_BASE3  = 0,   /**< 3^1 = 3: raw trit (-1, 0, +1), 1 trit — KFactor precision */
    TERNARY_BASE9  = 1,   /**< 3^2 = 9: trit pair (-4 to +4), 2 trits — X,Y coordinates */
    TERNARY_BASE27 = 2,   /**< 3^3 = 27: mental construct cube (-13 to +13), 3 trits — X,Y,Z */
    TERNARY_BASE81 = 3,   /**< 3^4 = 81: fine ternary (-40 to +40), 4 trits — high precision */
} TernaryBase;

/** @name TernaryBase Bounds
 *  @brief Total count of ternary scales.
 *  @{ */
#define TERNARY_BASE_COUNT (4)  /**< Total bases: 4 (base3, base9, base27, base81) */
/** @} */ /* end TernaryBase Bounds */

/**
 * @brief TernaryBase information record — unified lookup for ternary scale metadata.
 *
 * Contains all ternary base metadata in one struct for single-lookup access.
 * Includes base value, power of 3, trit count, range limits, and display names.
 */
typedef struct TernaryBaseInfo {
    int16_t     base;       /**< Base value: 3, 9, 27, 81 */
    int8_t      power;      /**< Power of 3: 1, 2, 3, 4 */
    int8_t      trits;      /**< Number of trits represented: 1, 2, 3, 4 */
    int16_t     min;        /**< Minimum value: -(base-1)/2 */
    int16_t     max;        /**< Maximum value: +(base-1)/2 */
    const char* name;       /**< Base name: "base3", "base9", "base27", "base81" */
    const char* use_case;   /**< Brief description of primary use */
} TernaryBaseInfo;

/**
 * @brief MASTER lookup table: TernaryBase → all metadata.
 *
 * Array uses designated initializers for clarity and safety.
 * Direct indexing: TERNARY_BASE_INFO[base] returns info for that base.
 *
 * Range formula: min = -(base-1)/2, max = +(base-1)/2
 */
static const TernaryBaseInfo TERNARY_BASE_INFO[TERNARY_BASE_COUNT] = {
    [TERNARY_BASE3]  = {  3, 1, 1,  -1,  +1, "base3",  "raw trit"      },  /**< K-factor precision */
    [TERNARY_BASE9]  = {  9, 2, 2,  -4,  +4, "base9",  "trit pair"     },  /**< X,Y coordinates */
    [TERNARY_BASE27] = { 27, 3, 3, -13, +13, "base27", "cube (27-pos)" },  /**< Mental construct */
    [TERNARY_BASE81] = { 81, 4, 4, -40, +40, "base81", "fine ternary"  },  /**< High precision */
};

/**
 * @brief Safe lookup: TernaryBase → TernaryBaseInfo pointer.
 *
 * Returns pointer to the information record for the given base.
 * Defaults to BASE3 if base is out of range.
 *
 * @param base TernaryBase enum value: TERNARY_BASE3 (0) through TERNARY_BASE81 (3).
 * @return Pointer to TernaryBaseInfo record. Never NULL.
 *
 * @note Bounds-checked: invalid base defaults to TERNARY_BASE3.
 */
static inline const TernaryBaseInfo* ternary_base_info(TernaryBase base) {
    if (base < 0 || base >= TERNARY_BASE_COUNT) base = TERNARY_BASE3;
    return &TERNARY_BASE_INFO[base];
}

/** @} */ /* end B.2b.3 TERNARYBASE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b.4 TERNARYSCORE — Composite Struct for All 4 Ternary Scales [TERNARYSCORE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_scores_ternaryscore B.2b.4 TERNARYSCORE — All Ternary Scales
 * @ingroup cpisi_scores
 * @brief    Struct holding the same semantic value at all 4 ternary granularities.
 *
 * WHY: Rather than converting on-demand, pre-compute all ternary representations.
 *      base27 maps directly to the 27-position mental construct cube.
 *
 * SCOPE: Cube operations, trit arithmetic, mental construct navigation.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Normalized ternary float type (derives from S.2 ternary scale).
 *
 * TernaryValue represents a normalized float in the range [-1.0, +1.0].
 * Used for continuous ternary math operations.
 */
typedef float TernaryValue;

/** @name TernaryValue Bounds — Aliases to S.2 anchors
 *  @brief Range limits for TernaryValue type.
 *  @{ */
#define TERNARY_MIN  CPISI_TERNARY_MIN     /**< Minimum: -1.0 (from S.2) */
#define TERNARY_MAX  CPISI_TERNARY_MAX     /**< Maximum: +1.0 (from S.2) */
#define TERNARY_ZERO CPISI_TERNARY_CENTER  /**< Center: 0.0 (from S.2) */
/** @} */ /* end TernaryValue Bounds */

/**
 * @brief Multi-scale ternary score representation — all 4 ternary scales in one struct.
 *
 * Stores the same semantic value at all 4 ternary granularities simultaneously.
 * base27 is the mental construct cube (27 positions); base3 is the raw trit.
 *
 * Memory: 4 bytes (4 × int8_t). Packed for efficiency.
 *
 * @see TernaryBase for individual scale definitions.
 */
typedef struct TernaryScore {
    int8_t  base3;      /**< 3^1 = 3: raw trit (-1, 0, +1) */
    int8_t  base9;      /**< 3^2 = 9: trit pair (-4 to +4) */
    int8_t  base27;     /**< 3^3 = 27: cube range (-13 to +13) — mental construct */
    int8_t  base81;     /**< 3^4 = 81: fine ternary (-40 to +40) */
} TernaryScore;

/** @name TernaryScore Helpers
 *  @brief Zero initializer and construction functions.
 *  @{ */

/**
 * @brief Zero/center TernaryScore (center at all scales).
 *
 * Designated initializer for neutral center state.
 * All scales at 0 — the origin point.
 */
#define TERNARY_SCORE_ZERO { .base3 = 0, .base9 = 0, .base27 = 0, .base81 = 0 }

/**
 * @brief Cube position center (0,0,0) in linear index (derives from S.2).
 *
 * The center of the 27-position mental construct cube. Value is 13 (middle of 0-26).
 */
#define CUBE_CENTER_POS CPISI_BASE27_HALF

/**
 * @brief Create TernaryScore from base81 value.
 *
 * Computes all 4 scale values from the finest resolution (base81).
 * Uses integer division (truncation toward zero).
 *
 * @param base81 Fine ternary value (-40 to +40).
 * @return TernaryScore with all scales populated.
 *
 * @note base3 uses sign determination (no threshold).
 */
static inline TernaryScore ternary_score_from_base81(int8_t base81) {
    TernaryScore s;
    s.base81 = base81;
    s.base27 = (int8_t)(base81 / 3);
    s.base9  = (int8_t)(base81 / 9);
    s.base3  = (int8_t)(base81 > 0 ? 1 : (base81 < 0 ? -1 : 0));
    return s;
}

/** @} */ /* end TernaryScore Helpers */

/** @} */ /* end B.2b.4 TERNARYSCORE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b.5 COMBINEDSTATE — Full Cognitive Position [COMBINEDSTATE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_scores_combinedstate B.2b.5 COMBINEDSTATE — Full Cognitive Position
 * @ingroup cpisi_scores
 * @brief    HebrewState + KFactor + TernaryValue unified in one struct.
 *
 * WHY: The 7 meaningful states require both position AND direction.
 *      CombinedState captures the full cognitive position in one record.
 *
 * SCOPE: Complete cognitive state for decision-making, state transitions.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Full cognitive position — state + direction + value unified.
 *
 * Combines HebrewState (where you are) with KFactor (where you're heading)
 * and TernaryValue (float for math operations). This is the complete
 * cognitive position used for decision-making.
 *
 * Default: yashar + K_MAINTAIN + 0.0 (balanced center, no movement).
 *
 * @see HebrewState for cognitive positions (7 states).
 * @see KFactor for direction of movement (3 directions).
 * @see TernaryValue for continuous float representation.
 */
typedef struct CombinedState {
    HebrewState   state;          /**< Current cognitive position (0-6) */
    KFactor       k;              /**< Direction of movement (-1, 0, +1) */
    TernaryValue  ternary_value;  /**< Float representation: -1.0 to +1.0 */
} CombinedState;

/**
 * @brief Default CombinedState (yashar, maintain, center).
 *
 * Designated initializer for neutral center state.
 * Position: yashar (even), Direction: maintain (no movement), Value: 0.0.
 */
#define COMBINED_STATE_DEFAULT { \
    .state         = HEBREW_YASHAR, \
    .k             = K_MAINTAIN, \
    .ternary_value = CPISI_TERNARY_CENTER \
}

/** @} */ /* end B.2b.5 COMBINEDSTATE */

/** @} */ /* end B.2b SCORES — cpisi_scores */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2c PRIMITIVES — Atomic Score Constants [PRIMITIVES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_primitives B.2c PRIMITIVES — Atomic Score Constants
 * @ingroup cpisi_types
 * @brief    Trit-level scoring constants for primitive health calculations.
 *
 * WHY: The most basic scoring unit is the trit: success (+1), failure (-1),
 *      neutral (0). These primitives derive from S.2 ternary scale and are
 *      used across all domains for atomic health operations.
 *
 * SCOPE: Foundation primitives referenced by config/log.h, health.h, phase.h.
 *
 * CONTAINS:
 *   - ATOMIC_SUCCESS (+1) — Narrow way (Matthew 7:13-14)
 *   - ATOMIC_FAILURE (-1) — Broad way
 *   - ATOMIC_NEUTRAL (0)  — No impact
 *
 * Scripture: "Enter ye in at the strait gate: for wide is the gate, and broad
 *            is the way, that leadeth to destruction, and many there be which
 *            go in thereat: Because strait is the gate, and narrow is the way,
 *            which leadeth unto life, and few there be that find it."
 *            — Matthew 7:13-14 KJV
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Atomic success (+1) — the narrow way.
 *
 * Positive outcome, contributes +1 to health calculations.
 * Derives from CPISI_TERNARY_MAX (S.2 scale anchor) cast to int.
 *
 * Scripture: "Narrow is the way, which leadeth unto life" — Matthew 7:14
 */
#define ATOMIC_SUCCESS  ((int)CPISI_TERNARY_MAX)

/**
 * @brief Atomic failure (-1) — the broad way.
 *
 * Negative outcome, contributes -1 to health calculations.
 * Derives from CPISI_TERNARY_MIN (S.2 scale anchor) cast to int.
 *
 * Scripture: "Broad is the way, that leadeth to destruction" — Matthew 7:13
 */
#define ATOMIC_FAILURE  ((int)CPISI_TERNARY_MIN)

/**
 * @brief Atomic neutral (0) — no impact.
 *
 * Neutral outcome, contributes 0 to health calculations.
 * Derives from CPISI_TERNARY_CENTER (S.2 scale anchor) cast to int.
 */
#define ATOMIC_NEUTRAL  ((int)CPISI_TERNARY_CENTER)

/** @} */ /* end B.2c PRIMITIVES — cpisi_primitives */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2d ASSESSMENT — ASSESS Phase Types [ASSESSMENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_assessment B.2d ASSESSMENT — ASSESS Phase Types
 * @ingroup cpisi_types
 * @brief    Types for evaluating and diagnosing issues in the DAR cycle.
 *
 * WHY: The ASSESS phase of DAR (Detect-Assess-Respond) needs standardized types
 *      for classifying what triggered detection and how severe the issue is.
 *      Cross-cutting measurement applies to health, phase, and all domains.
 *
 * SCOPE: cpisi_debug subsystem, DAR ASSESS phase, health/phase domains.
 *
 * CONTAINS:
 *   - B.2d.1 DetectType (7) — What triggered detection
 *   - B.2d.2 Severity (9)   — How severe is the impact (Base9 balanced)
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ ASSESSMENT ARCHITECTURE = DETECT (7) × SEVERITY (9)                         │
 * ├─────────┬───────┬───────────────────────────────────────────────────────────┤
 * │ Detect  │   7   │ DetectType — what triggered detection                     │
 * │ Severity│   9   │ Severity — Base9 balanced (-4 to +4)                      │
 * └─────────┴───────┴───────────────────────────────────────────────────────────┘
 *
 * Builds on: B.2a-c (Foundation, Scores, Primitives)
 * Config source: bereshit/word/core/os/health/diagnostics.toml
 *
 * Scripture: "Examine yourselves, whether ye be in the faith; prove your own
 *            selves. Know ye not your own selves, how that Jesus Christ is in
 *            you, except ye be reprobates?" — 2 Corinthians 13:5 KJV
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2d.1 DETECTTYPE — Detection Trigger Classification (7 types) [DETECTTYPE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assessment_detecttype B.2d.1 DETECTTYPE — Detection Triggers
 * @ingroup cpisi_assessment
 * @brief    7-type classification of what triggered detection in DAR cycle.
 *
 * WHY: Different triggers require different responses. A threshold breach
 *      differs from an anomaly differs from an explicit error.
 *
 * SCOPE: DAR Detect phase, logging, diagnostics.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief 7-type detection trigger classification.
 *
 * Classifies what caused the detection event. Used in DAR cycle to determine
 * appropriate response strategy. NONE is the default (normal operation).
 *
 * Values are 0-indexed for direct array access into DETECT_TYPE_INFO.
 */
typedef enum DetectType {
    DETECT_NONE      = 0,   /**< No detection — normal operation */
    DETECT_THRESHOLD = 1,   /**< Health crossed a threshold (zone boundary) */
    DETECT_ANOMALY   = 2,   /**< Unexpected pattern or behavior detected */
    DETECT_TRIPWIRE  = 3,   /**< Fallback/alternative path was taken */
    DETECT_ERROR     = 4,   /**< Explicit error condition occurred */
    DETECT_TIMEOUT   = 5,   /**< Operation exceeded time limit */
    DETECT_MANUAL    = 6,   /**< Explicitly triggered by code or user */
} DetectType;

/** @name DetectType Bounds
 *  @brief Total count of detection types.
 *  @{ */
#define DETECT_TYPE_COUNT 7  /**< Total types: 7 (none through manual) */
/** @} */ /* end DetectType Bounds */

/**
 * @brief DetectType information record — unified lookup for detection metadata.
 *
 * Each DetectType maps to display information for logging and UI.
 */
typedef struct DetectTypeInfo {
    const char* name;       /**< Lowercase name: "none", "threshold", etc. */
    const char* emoji;      /**< Visual symbol for display */
} DetectTypeInfo;

/**
 * @brief MASTER lookup table: DetectType → all metadata.
 *
 * Array uses designated initializers for clarity and safety.
 * Direct indexing: DETECT_TYPE_INFO[type] returns info for that type.
 */
static const DetectTypeInfo DETECT_TYPE_INFO[DETECT_TYPE_COUNT] = {
    [DETECT_NONE]      = { "none",      "○"  },  /**< Normal: no detection */
    [DETECT_THRESHOLD] = { "threshold", "📊" },  /**< Crossed zone boundary */
    [DETECT_ANOMALY]   = { "anomaly",   "⚡" },  /**< Unexpected pattern */
    [DETECT_TRIPWIRE]  = { "tripwire",  "🚧" },  /**< Fallback path taken */
    [DETECT_ERROR]     = { "error",     "❌" },  /**< Explicit error */
    [DETECT_TIMEOUT]   = { "timeout",   "⏱️"  },  /**< Time limit exceeded */
    [DETECT_MANUAL]    = { "manual",    "👆" },  /**< Explicitly triggered */
};

/**
 * @brief Safe lookup: DetectType → DetectTypeInfo pointer.
 *
 * Returns pointer to the information record for the given type.
 * Defaults to DETECT_NONE if type is out of range.
 *
 * @param type DetectType enum value: DETECT_NONE (0) through DETECT_MANUAL (6).
 * @return Pointer to DetectTypeInfo record. Never NULL.
 *
 * @note Bounds-checked: invalid type defaults to DETECT_NONE.
 */
static inline const DetectTypeInfo* detect_type_info(DetectType type) {
    if (type < 0 || type >= DETECT_TYPE_COUNT) type = DETECT_NONE;
    return &DETECT_TYPE_INFO[type];
}

/** @} */ /* end B.2d.1 DETECTTYPE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2d.2 SEVERITY — Base9 Balanced Severity Scale (9 levels) [SEVERITY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_assessment_severity B.2d.2 SEVERITY — Base9 Severity Scale
 * @ingroup cpisi_assessment
 * @brief    Base9 balanced severity scale (-4 to +4) mapping to health impact.
 *
 * WHY: Severity quantifies impact. The Base9 scale provides balanced granularity:
 *      9 levels from fatal (-4) to perfect (+4), mapping to -100 to +100 health.
 *      Each step = 25 health impact.
 *
 * SCOPE: DAR Assess phase, health calculations, issue classification.
 *
 * ┌─────────────┬───────┬───────────────┬─────────────────────────────────────┐
 * │ Severity    │ Value │ Health Impact │ Description                         │
 * ├─────────────┼───────┼───────────────┼─────────────────────────────────────┤
 * │ SEV_PERFECT │  +4   │ +100          │ Full restoration (tov)              │
 * │ SEV_EXCELLENT│ +3   │  +75          │ Excellent recovery                  │
 * │ SEV_GOOD    │  +2   │  +50          │ Good recovery                       │
 * │ SEV_FAIR    │  +1   │  +25          │ Fair improvement                    │
 * │ SEV_NONE    │   0   │    0          │ Neutral (yashar)                    │
 * │ SEV_LOW     │  -1   │  -25          │ Minor issue                         │
 * │ SEV_MEDIUM  │  -2   │  -50          │ Moderate issue                      │
 * │ SEV_HIGH    │  -3   │  -75          │ Serious issue                       │
 * │ SEV_FATAL   │  -4   │ -100          │ Unrecoverable (shavar)              │
 * └─────────────┴───────┴───────────────┴─────────────────────────────────────┘
 *
 * Impact formula: severity × SEVERITY_STEP (25) = health impact
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Base9 balanced severity scale — 9 levels from -4 to +4.
 *
 * Severity quantifies impact on health. Negative values represent damage
 * (toward shavar), positive values represent recovery (toward tov).
 * Zero (SEV_NONE) is the neutral center (yashar).
 *
 * Each level maps to a HebrewState and health impact (×25).
 */
typedef enum Severity {
    SEV_PERFECT   = +4,  /**< Full restoration (tov) — impact: +100, hebrew: TOV */
    SEV_EXCELLENT = +3,  /**< Excellent recovery — impact: +75, hebrew: SHALEM */
    SEV_GOOD      = +2,  /**< Good recovery — impact: +50, hebrew: SHALEM */
    SEV_FAIR      = +1,  /**< Fair improvement — impact: +25, hebrew: TAMIM */
    SEV_NONE      =  0,  /**< Neutral (yashar) — impact: 0, hebrew: YASHAR [CENTER] */
    SEV_LOW       = -1,  /**< Minor issue — impact: -25, hebrew: RATSAH */
    SEV_MEDIUM    = -2,  /**< Moderate issue — impact: -50, hebrew: CHASER */
    SEV_HIGH      = -3,  /**< Serious issue — impact: -75, hebrew: SHAVAR */
    SEV_FATAL     = -4,  /**< Unrecoverable (shavar) — impact: -100, hebrew: SHAVAR */
} Severity;

/** @name Severity Bounds — Range limits and constants
 *  @brief Defines the valid range and scaling for Severity values.
 *  @{ */
#define SEVERITY_COUNT      CPISI_FACE          /**< Total levels: 9 (-4 to +4) */
#define SEVERITY_HALF       CPISI_FACE_CENTER   /**< Half-range: 4 (center to edge) */
#define SEVERITY_MIN        (-SEVERITY_HALF)    /**< Minimum: -4 (SEV_FATAL) */
#define SEVERITY_MAX        (+SEVERITY_HALF)    /**< Maximum: +4 (SEV_PERFECT) */
#define SEVERITY_STEP       25                  /**< Health impact per level: ×25 */
/** @} */ /* end Severity Bounds */

/**
 * @brief Severity information record — unified lookup for severity metadata.
 *
 * Each Severity maps to health impact, ternary value, name, and HebrewState.
 * Enables single-lookup access to all severity-related data.
 */
typedef struct SeverityInfo {
    int         health_impact;  /**< Health impact: -100 to +100 (severity × 25) */
    float       ternary;        /**< Ternary value: -1.0 to +1.0 (normalized) */
    const char* name;           /**< Lowercase name: "fatal", "high", "none", etc. */
    HebrewState hebrew;         /**< Corresponding HebrewState (severity → state) */
} SeverityInfo;

/**
 * @brief MASTER lookup table: Severity → all metadata.
 *
 * Array is indexed by (severity + SEVERITY_HALF) to convert signed Severity
 * [-4..+4] to array index [0..8]. Use severity_info() for safe access.
 *
 * Maps severity to hebrew states: fatal/high→SHAVAR, medium→CHASER,
 * low→RATSAH, none→YASHAR, fair→TAMIM, good/excellent→SHALEM, perfect→TOV.
 */
static const SeverityInfo SEVERITY_INFO[SEVERITY_COUNT] = {
    [0] = { -100, -1.00f, "fatal",     HEBREW_SHAVAR },  /**< SEV_FATAL (-4) */
    [1] = {  -75, -0.75f, "high",      HEBREW_SHAVAR },  /**< SEV_HIGH (-3) */
    [2] = {  -50, -0.50f, "medium",    HEBREW_CHASER },  /**< SEV_MEDIUM (-2) */
    [3] = {  -25, -0.25f, "low",       HEBREW_RATSAH },  /**< SEV_LOW (-1) */
    [4] = {    0,  0.00f, "none",      HEBREW_YASHAR },  /**< SEV_NONE (0) [CENTER] */
    [5] = {  +25, +0.25f, "fair",      HEBREW_TAMIM  },  /**< SEV_FAIR (+1) */
    [6] = {  +50, +0.50f, "good",      HEBREW_SHALEM },  /**< SEV_GOOD (+2) */
    [7] = {  +75, +0.75f, "excellent", HEBREW_SHALEM },  /**< SEV_EXCELLENT (+3) */
    [8] = { +100, +1.00f, "perfect",   HEBREW_TOV    },  /**< SEV_PERFECT (+4) */
};

/**
 * @brief Safe lookup: Severity → SeverityInfo pointer.
 *
 * Converts signed Severity value to array index and returns pointer to
 * the corresponding information record.
 *
 * @param sev Severity enum value: SEV_FATAL (-4) through SEV_PERFECT (+4).
 * @return Pointer to SeverityInfo record. Never NULL.
 *
 * @note Bounds-checked: invalid severity defaults to SEV_NONE (index 4).
 */
static inline const SeverityInfo* severity_info(Severity sev) {
    int idx = sev + SEVERITY_HALF;
    if (idx < 0 || idx >= SEVERITY_COUNT) idx = SEVERITY_HALF;  /* SEV_NONE */
    return &SEVERITY_INFO[idx];
}

/** @} */ /* end B.2d.2 SEVERITY */

/** @} */ /* end B.2d ASSESSMENT — cpisi_assessment */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 HELPERS — Foundation Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_helpers B.3 HELPERS — Foundation Functions
 * @ingroup cpisi_types
 * @brief    Config-driven pure functions — validation, conversions, utilities.
 *
 * WHY: Helper functions that operate on B.2 types. UNIVERSAL helpers stay here,
 *      domain-specific helpers move to domain files (domain ownership).
 *
 * SCOPE: Foundation helpers that ALL domains need. Domain-specific helpers
 *        (health, DAR, logging) live in their respective domain files.
 *
 * CONTAINS:
 *   - B.3a VALIDATION  — Bounds checking, value clamping (foundation types)
 *   - B.3b CONVERSIONS — Type transforms using B.2 unified lookups
 *   - B.3c UTILITIES   — State operations, adjustments, initializers
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ HELPER ARCHITECTURE = MACROS (patterns) + FUNCTIONS (composition)          │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │                                                                             │
 * │   GROUP 1: MACROS — universal patterns (CPISI_CLAMP, CPISI_IN_RANGE, etc.) │
 * │                                                                             │
 * │   GROUP 2: FUNCTIONS — compose with B.2 types (by domain ownership):       │
 * │     UNIVERSAL (types.h): Foundation, Scores, Assessment                    │
 * │     DOMAIN (moved):      health.h, phase.h, log.h own their helpers        │
 * │                                                                             │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * Design: Change a constant → behavior changes everywhere.
 * Order:  Validation → Conversions → Utilities (later may call earlier)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a VALIDATION — Bounds Checking [VALIDATION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_validation B.3a VALIDATION — Bounds Checking
 * @ingroup cpisi_helpers
 * @brief    Config-driven bounds checking and value clamping.
 *
 * WHY: Validation functions for B.2 types. Foundation types validated here,
 *      domain types validated in domain files (domain ownership).
 *
 * SCOPE: Foundation (KFactor, HebrewZone, HebrewState), Scores, Assessment.
 *        Health → health.h, DAR → phase.h, Log → log.h.
 *
 * CONTAINS:
 *   - B.3a.1 Validation Macros  — CPISI_CLAMP, CPISI_IN_RANGE_*, CPISI_SIGN_TO
 *   - B.3a.2 Type Validation    — _clamp(), _valid(), _sign_to() per type
 *
 * Pattern per type: _clamp() → _valid() → _sign_to() (if applicable)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a.1 VALIDATION MACROS — Universal Patterns [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_validation_macros B.3a.1 VALIDATION MACROS
 * @ingroup cpisi_helpers_validation
 * @brief    Universal macros for bounds checking and sign mapping.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* CPISI_CLAMP — defined in S.2a.1c Formula */

/** @brief Check if value is in [min, max] range (signed) */
#define CPISI_IN_RANGE_SIGNED(val, min, max) \
    ((val) >= (min) && (val) <= (max))

/** @brief Check if value is in [0, max] range (unsigned) */
#define CPISI_IN_RANGE_UNSIGNED(val, max) \
    ((val) <= (max))

/** @brief Map sign to ternary values (-/0/+) */
#define CPISI_SIGN_TO(val, neg, zero, pos) \
    ((val) < 0 ? (neg) : ((val) > 0 ? (pos) : (zero)))

/** @} */ /* end B.3a.1 VALIDATION MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a.2 TYPE VALIDATION — Per-Type Functions [TYPE_VALIDATION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_validation_types B.3a.2 TYPE VALIDATION
 * @ingroup cpisi_helpers_validation
 * @brief    Validation functions for B.2 types (_clamp, _valid, _sign_to).
 *
 * Order follows B.2 dependency chain:
 *   B.2a Foundation: KFactor(3) → HebrewZone(5) → HebrewState(7)
 *   B.2b Scores:     TernaryScore → MultiScore
 *   B.2d Assessment: DetectType(7) → Severity(9)
 *   [Domain types validated in their respective files]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3a.2a Foundation Types (from B.2a) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

// ─────────────────────────────────────────────────────────────────────────────
// KFactor — signed enum: K_FACTOR_MIN(-1) to K_FACTOR_MAX(+1)
// Lookup: KFACTOR_INFO[k] for name, symbol, color
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Clamp int to valid KFactor range [-1, +1] */
static inline KFactor cpisi_kfactor_clamp(int value) {
    return (KFactor)CPISI_CLAMP(value, K_FACTOR_MIN, K_FACTOR_MAX);
}

/** @brief Check if KFactor is valid */
static inline bool cpisi_kfactor_valid(KFactor k) {
    return CPISI_IN_RANGE_SIGNED(k, K_FACTOR_MIN, K_FACTOR_MAX);
}

/** @brief Convert sign to KFactor */
static inline KFactor cpisi_sign_to_kfactor(int value) {
    return CPISI_SIGN_TO(value, K_RETREAT, K_MAINTAIN, K_ADVANCE);
}

// ─────────────────────────────────────────────────────────────────────────────
// HebrewZone — unsigned enum: 0 to ZONE_COUNT-1 (5 zones)
// Lookup: ZONE_INFO[zone] for name, symbol, color, threshold
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Clamp int to valid HebrewZone range [0, 4] */
static inline HebrewZone cpisi_zone_clamp(int value) {
    return (HebrewZone)CPISI_CLAMP(value, ZONE_MIN, ZONE_MAX);
}

/** @brief Check if HebrewZone is valid */
static inline bool cpisi_zone_valid(HebrewZone zone) {
    return CPISI_IN_RANGE_UNSIGNED(zone, ZONE_MAX);
}

// ─────────────────────────────────────────────────────────────────────────────
// HebrewState — unsigned enum: 0 to HEBREW_STATE_COUNT-1 (7 states)
// Lookup: HEBREW_STATE_INFO[state] for name, hebrew, emoji, value, kfactor, zone
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Clamp int to valid HebrewState range [0, 6] */
static inline HebrewState cpisi_hebrew_state_clamp(int value) {
    return (HebrewState)CPISI_CLAMP(value, HEBREW_STATE_MIN, HEBREW_STATE_MAX);
}

/** @brief Check if HebrewState is valid */
static inline bool cpisi_hebrew_state_valid(HebrewState state) {
    return CPISI_IN_RANGE_UNSIGNED(state, HEBREW_STATE_MAX);
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3a.2b Score Types (from B.2b) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

// ─────────────────────────────────────────────────────────────────────────────
// TernaryScore — multi-field struct: base3/9/27/81 ternary values
// Lookup: TERNARY_BASE_INFO[base] for range, use_case
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Clamp TernaryScore fields to valid ranges */
static inline TernaryScore cpisi_ternary_score_clamp(TernaryScore ts) {
    const TernaryBaseInfo* b3  = ternary_base_info(TERNARY_BASE3);
    const TernaryBaseInfo* b9  = ternary_base_info(TERNARY_BASE9);
    const TernaryBaseInfo* b27 = ternary_base_info(TERNARY_BASE27);
    const TernaryBaseInfo* b81 = ternary_base_info(TERNARY_BASE81);
    return (TernaryScore){
        .base3  = (int8_t)CPISI_CLAMP(ts.base3,  b3->min,  b3->max),
        .base9  = (int8_t)CPISI_CLAMP(ts.base9,  b9->min,  b9->max),
        .base27 = (int8_t)CPISI_CLAMP(ts.base27, b27->min, b27->max),
        .base81 = (int8_t)CPISI_CLAMP(ts.base81, b81->min, b81->max)
    };
}

/** @brief Check if TernaryScore is valid */
static inline bool cpisi_ternary_score_valid(const TernaryScore* ts) {
    if (!ts) return false;
    TernaryScore clamped = cpisi_ternary_score_clamp(*ts);
    return (ts->base3 == clamped.base3 && ts->base9 == clamped.base9 &&
            ts->base27 == clamped.base27 && ts->base81 == clamped.base81);
}

// ─────────────────────────────────────────────────────────────────────────────
// MultiScore — multi-field struct: base100/50/20/10/5/3 score values
// Lookup: SCALE_TYPE_INFO[scale] for range, use_case
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Clamp MultiScore fields to valid ranges */
static inline MultiScore cpisi_multi_score_clamp(MultiScore ms) {
    const ScaleTypeInfo* s100 = scale_type_info(SCALE_BASE100);
    const ScaleTypeInfo* s50  = scale_type_info(SCALE_BASE50);
    const ScaleTypeInfo* s20  = scale_type_info(SCALE_BASE20);
    const ScaleTypeInfo* s10  = scale_type_info(SCALE_BASE10);
    const ScaleTypeInfo* s5   = scale_type_info(SCALE_BASE5);
    const ScaleTypeInfo* s3   = scale_type_info(SCALE_BASE3);
    return (MultiScore){
        .base100 = (int8_t)CPISI_CLAMP(ms.base100, s100->min, s100->max),
        .base50  = (int8_t)CPISI_CLAMP(ms.base50,  s50->min,  s50->max),
        .base20  = (int8_t)CPISI_CLAMP(ms.base20,  s20->min,  s20->max),
        .base10  = (int8_t)CPISI_CLAMP(ms.base10,  s10->min,  s10->max),
        .base5   = (int8_t)CPISI_CLAMP(ms.base5,   s5->min,   s5->max),
        .base3   = (int8_t)CPISI_CLAMP(ms.base3,   s3->min,   s3->max)
    };
}

/** @brief Check if MultiScore is valid */
static inline bool cpisi_multi_score_valid(const MultiScore* ms) {
    if (!ms) return false;
    MultiScore clamped = cpisi_multi_score_clamp(*ms);
    return (ms->base100 == clamped.base100 && ms->base50 == clamped.base50 &&
            ms->base20 == clamped.base20 && ms->base10 == clamped.base10 &&
            ms->base5 == clamped.base5 && ms->base3 == clamped.base3);
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3a.2c Atomic Types (from B.2c) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** @brief Clamp int to valid atomic trit range [-1, +1] */
static inline int cpisi_atomic_clamp(int value) {
    return CPISI_CLAMP(value, ATOMIC_FAILURE, ATOMIC_SUCCESS);
}

/** @brief Check if atomic value is valid */
static inline bool cpisi_atomic_valid(int value) {
    return CPISI_IN_RANGE_SIGNED(value, ATOMIC_FAILURE, ATOMIC_SUCCESS);
}

/** @brief Convert sign to atomic trit */
static inline int cpisi_sign_to_atomic(int value) {
    return CPISI_SIGN_TO(value, ATOMIC_FAILURE, ATOMIC_NEUTRAL, ATOMIC_SUCCESS);
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3a.2d Assessment Types (from B.2d) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** @brief Check if detect type is valid */
static inline bool cpisi_detect_type_valid(DetectType type) {
    return (type >= 0 && type < DETECT_TYPE_COUNT);
}

/** @brief Check if severity is valid */
static inline bool cpisi_severity_valid(Severity sev) {
    return (sev >= SEVERITY_MIN && sev <= SEVERITY_MAX);
}

/** @brief Clamp severity to valid range */
static inline Severity cpisi_severity_clamp(Severity sev) {
    return (Severity)CPISI_CLAMP(sev, SEVERITY_MIN, SEVERITY_MAX);
}

/** @} */ /* end B.3a.2 TYPE VALIDATION */

/** @} */ /* end B.3a VALIDATION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b CONVERSIONS — Type Transformations [CONVERSIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_conversions B.3b CONVERSIONS — Type Transformations
 * @ingroup cpisi_helpers
 * @brief    Config-driven pure transformation functions (side-effect-free).
 *
 * WHY: Conversion functions using B.2 unified lookups. UNIVERSAL conversions
 *      stay here, domain-specific move to domain files (domain ownership).
 *
 * SCOPE: Foundation (KFactor, HebrewState), Scores, Assessment, Cube.
 *        Health → health.h, DAR → phase.h, Log → log.h.
 *
 * CONTAINS:
 *   - B.3b.1 Conversion Macros — CPISI_TO_FLOAT, CPISI_CUBE_ENCODE, etc.
 *   - B.3b.2 Type Conversions  — KFactor ops, HebrewState↔ternary, Scores
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b.1 CONVERSION MACROS — Universal Patterns [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_conversions_macros B.3b.1 CONVERSION MACROS
 * @ingroup cpisi_helpers_conversions
 * @brief    Universal macros for type transformations.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Scale integer to float: val / scale (e.g., health/100 → ternary) */
#define CPISI_TO_FLOAT(val, scale)    ((float)(val) / (float)(scale))

/** @brief Scale float to integer: val * scale (e.g., ternary*100 → health) */
#define CPISI_FROM_FLOAT(val, scale)  ((int)((val) * (float)(scale)))

/** @brief Cube coordinate to linear index (derives from CPISI_BASE*) */
#define CPISI_CUBE_ENCODE(x, y, z) \
    ((int8_t)(((x) + 1) * CPISI_BASE9 + ((y) + 1) * CPISI_BASE3 + ((z) + 1)))

/** @brief Check if value is in threshold zone: lower <= val < upper */
#define CPISI_IN_ZONE(val, lower, upper) \
    ((val) >= (lower) && (val) < (upper))

/** @} */ /* end B.3b.1 CONVERSION MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b.2 TYPE CONVERSIONS — Per-Type Functions [TYPE_CONVERSIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_conversions_types B.3b.2 TYPE CONVERSIONS
 * @ingroup cpisi_helpers_conversions
 * @brief    Conversion functions for B.2 types using unified lookups.
 *
 * Order follows B.2 dependency chain:
 *   B.2a Foundation: KFactor ops → HebrewState ↔ ternary ↔ zone
 *   B.2b Scores:     MultiScore/TernaryScore compare, sign, to_float
 *   B.2d Assessment: Severity ↔ health, DetectType lookups
 *   Cube:            27-position mental construct
 *   [Domain conversions in respective domain files]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3b.2a Foundation Type Conversions (from B.2a) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

// ─────────────────────────────────────────────────────────────────────────────
// KFactor Operations — ternary math (negate, combine, weight)
// Lookup: k_factor_info(k) for name, symbol, color
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief   Get opposite KFactor (ternary negation)
 * @param   k   KFactor to negate
 * @return  Negated KFactor: K_RETREAT↔K_ADVANCE, K_MAINTAIN unchanged
 */
static inline KFactor cpisi_kfactor_opposite(KFactor k) {
    return (KFactor)(-k);
}

/**
 * @brief   Combine two KFactors (ternary addition with clamp)
 * @param   a   First KFactor
 * @param   b   Second KFactor
 * @return  Combined KFactor clamped to [-1, +1]
 * @note    Uses B.3a cpisi_kfactor_clamp() for bounds
 */
static inline KFactor cpisi_kfactor_combine(KFactor a, KFactor b) {
    return cpisi_kfactor_clamp((int)a + (int)b);
}


// ─────────────────────────────────────────────────────────────────────────────
// HebrewState Conversions — state ↔ ternary ↔ zone
// Lookup: hebrew_state_info(state) for value, kfactor, zone
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief   Convert HebrewState to ternary float [-1.0, +1.0]
 * @param   state   HebrewState enum (0-6)
 * @return  Ternary value from HEBREW_STATE_INFO[state].value
 * @note    Uses hebrew_state_info() accessor for config-driven lookup
 */
static inline float cpisi_hebrew_to_ternary(HebrewState state) {
    return hebrew_state_info(state)->value;
}


/**
 * @brief   Get zone classification for HebrewState
 * @param   state   HebrewState enum (0-6)
 * @return  HebrewZone (0-4) from HEBREW_STATE_INFO[state].zone
 * @note    Uses hebrew_state_info() accessor for config-driven lookup
 */
static inline HebrewZone cpisi_hebrew_state_zone(HebrewState state) {
    return hebrew_state_info(state)->zone;
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3b.2b Score Type Conversions (from B.2b) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

// ─────────────────────────────────────────────────────────────────────────────
// MultiScore Operations — compare, sign checks, to_float
// Lookup: scale_type_info(scale) for range, divisor, use_case
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Compare MultiScores by base100 (ternary result: -1/0/+1) */
static inline int cpisi_multi_score_compare(const MultiScore* a, const MultiScore* b) {
    if (!a || !b) return 0;
    return CPISI_SIGN_TO(a->base100 - b->base100, -1, 0, +1);
}

/** @brief Check if MultiScore is positive (base3 > 0) */
static inline bool cpisi_multi_score_positive(const MultiScore* ms) {
    return ms && ms->base3 > 0;
}

/** @brief Check if MultiScore is negative (base3 < 0) */
static inline bool cpisi_multi_score_negative(const MultiScore* ms) {
    return ms && ms->base3 < 0;
}

/** @brief Check if MultiScore is balanced (base3 == 0) */
static inline bool cpisi_multi_score_balanced(const MultiScore* ms) {
    return ms && ms->base3 == 0;
}

/**
 * @brief   Convert MultiScore to ternary float [-1.0, +1.0]
 * @param   ms  Pointer to MultiScore
 * @return  Ternary float, or 0.0 if NULL
 * @note    Uses scale_type_info(SCALE_BASE100)->base for config-driven scaling
 */
static inline float cpisi_multi_score_to_float(const MultiScore* ms) {
    if (!ms) return CPISI_TERNARY_CENTER;
    return CPISI_TO_FLOAT(ms->base100, scale_type_info(SCALE_BASE100)->base);
}

// ─────────────────────────────────────────────────────────────────────────────
// TernaryScore Operations — compare, sign checks, to_float
// Lookup: ternary_base_info(base) for range, power, use_case
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Compare TernaryScores by base81 (ternary result: -1/0/+1) */
static inline int cpisi_ternary_score_compare(const TernaryScore* a, const TernaryScore* b) {
    if (!a || !b) return 0;
    return CPISI_SIGN_TO(a->base81 - b->base81, -1, 0, +1);
}

/** @brief Check if TernaryScore is positive (base3 > 0) */
static inline bool cpisi_ternary_score_positive(const TernaryScore* ts) {
    return ts && ts->base3 > 0;
}

/** @brief Check if TernaryScore is negative (base3 < 0) */
static inline bool cpisi_ternary_score_negative(const TernaryScore* ts) {
    return ts && ts->base3 < 0;
}

/** @brief Check if TernaryScore is balanced (base3 == 0) */
static inline bool cpisi_ternary_score_balanced(const TernaryScore* ts) {
    return ts && ts->base3 == 0;
}

/**
 * @brief   Convert TernaryScore to ternary float [-1.0, +1.0]
 * @param   ts  Pointer to TernaryScore
 * @return  Ternary float, or 0.0 if NULL
 * @note    Uses ternary_base_info(TERNARY_BASE81)->max for config-driven scaling
 */
static inline float cpisi_ternary_score_to_float(const TernaryScore* ts) {
    if (!ts) return CPISI_TERNARY_CENTER;
    return CPISI_TO_FLOAT(ts->base81, ternary_base_info(TERNARY_BASE81)->max);
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3b.2c Cube Operations (Mental Construct) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 27-Position Cube — encode/decode, classify, distance.
 * Uses S.2 constants: CPISI_BASE3(3), CPISI_BASE9(9), CPISI_CUBE(27).
 * Foundation for CPI-SI mental construct navigation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════ */

// --- Coordinate Conversion (linear ↔ x,y,z) ---

/** @brief Encode (x,y,z) coords to linear index (uses CPISI_CUBE_ENCODE macro) */
static inline int8_t cpisi_cube_encode(int8_t x, int8_t y, int8_t z) {
    return CPISI_CUBE_ENCODE(x, y, z);
}

/** @brief Decode linear index to (x,y,z) coords (uses S.2 CPISI_BASE*) */
static inline void cpisi_cube_decode(int8_t pos, int8_t* x, int8_t* y, int8_t* z) {
    *x = (pos / CPISI_BASE9) - 1;
    *y = ((pos % CPISI_BASE9) / CPISI_BASE3) - 1;
    *z = (pos % CPISI_BASE3) - 1;
}

// --- Position Classification (center/corner/edge/face) ---

/**
 * @brief Count zero coordinates (classifies position type)
 *
 * | Zeros | Type   | Description                |
 * |-------|--------|----------------------------|
 * |   0   | corner | all axes non-zero (8 pos)  |
 * |   1   | edge   | one axis zero (12 pos)     |
 * |   2   | face   | two axes zero (6 pos)      |
 * |   3   | center | all axes zero (1 pos)      |
 */
static inline int cpisi_cube_zeros(int8_t pos) {
    int8_t x, y, z;
    cpisi_cube_decode(pos, &x, &y, &z);
    return (x == 0) + (y == 0) + (z == 0);
}

/** @brief Check if position is center (uses CUBE_CENTER_POS from B.2c) */
static inline bool cpisi_cube_is_center(int8_t pos) {
    return pos == CUBE_CENTER_POS;
}

/** @brief Check if position is corner (zeros == 0: all axes non-zero) */
static inline bool cpisi_cube_is_corner(int8_t pos) {
    return cpisi_cube_zeros(pos) == 0;
}

/** @brief Check if position is edge (zeros == 1: one axis zero) */
static inline bool cpisi_cube_is_edge(int8_t pos) {
    return cpisi_cube_zeros(pos) == 1;
}

/** @brief Check if position is face (zeros == 2: two axes zero) */
static inline bool cpisi_cube_is_face(int8_t pos) {
    return cpisi_cube_zeros(pos) == 2;
}

// --- Position Metrics ---

/** @brief Calculate Manhattan distance from center (sum of absolute coords)
 *  @note Requires CPISI_MODE >= 2 (RUNTIME) for abs() function
 */
#if CPISI_MODE >= 2
static inline int cpisi_cube_distance(int8_t pos) {
    int8_t x, y, z;
    cpisi_cube_decode(pos, &x, &y, &z);
    return abs(x) + abs(y) + abs(z);
}
#endif /* CPISI_MODE >= 2 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3b.2d Assessment Conversions (from B.2d) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief   Get ternary value for severity (config-driven)
 * @param   sev  Severity enum value
 * @return  Ternary value from severity_info()
 */
static inline float cpisi_severity_ternary(Severity sev) {
    return severity_info(sev)->ternary;
}

/**
 * @brief   Convert Severity to HebrewState (config-driven)
 * @param   sev  Severity enum value
 * @return  HebrewState from severity_info()
 */
static inline HebrewState cpisi_severity_to_hebrew(Severity sev) {
    return severity_info(sev)->hebrew;
}

/** @} */ /* end B.3b.2 TYPE CONVERSIONS */

/** @} */ /* end B.3b CONVERSIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3c UTILITIES — State Operations [UTILITIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_utilities B.3c UTILITIES — State Operations
 * @ingroup cpisi_helpers
 * @brief    State transitions, adjustments, and zero initializers.
 *
 * WHY: Utility functions that compose with B.3a validation and B.3b conversions.
 *      UNIVERSAL utilities stay here, domain-specific move to domain files.
 *
 * SCOPE: Foundation (HebrewState step/opposite), Scores (zero initializers).
 *        Health → health.h, DAR → phase.h, Log → log.h.
 *
 * CONTAINS:
 *   - B.3c.1 Foundation Utilities — HebrewState step, opposite
 *   - B.3c.2 Score Utilities      — MultiScore/TernaryScore zero initializers
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3c.1 Foundation Utilities (from B.2a) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** @brief Step HebrewState by direction (composes with B.3a clamp) */
static inline HebrewState cpisi_hebrew_state_step(HebrewState state, int direction) {
    int new_state = (int)state + direction;
    return cpisi_hebrew_state_clamp(new_state);
}

/** @brief Return the mirror state (shavar↔tov, chaser↔shalem, ratsah↔tamim) */
static inline HebrewState cpisi_hebrew_state_opposite(HebrewState state) {
    switch (state) {
        case HEBREW_SHAVAR: return HEBREW_TOV;
        case HEBREW_CHASER: return HEBREW_SHALEM;
        case HEBREW_RATSAH: return HEBREW_TAMIM;
        case HEBREW_YASHAR: return HEBREW_YASHAR;
        case HEBREW_TAMIM:  return HEBREW_RATSAH;
        case HEBREW_SHALEM: return HEBREW_CHASER;
        case HEBREW_TOV:    return HEBREW_SHAVAR;
        default:            return HEBREW_YASHAR;
    }
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3c.2 Score Utilities (from B.2b) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** @brief Return a zero-initialized MultiScore */
static inline MultiScore cpisi_multi_score_zero(void) {
    MultiScore ms = MULTI_SCORE_ZERO;
    return ms;
}

/** @brief Return a zero-initialized TernaryScore */
static inline TernaryScore cpisi_ternary_score_zero(void) {
    TernaryScore ts = TERNARY_SCORE_ZERO;
    return ts;
}

/** @} */ // end Utilities (B.3c)

/** @} */ // end Helpers (B.3)

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Recovery Patterns [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_errors B.4 ERRORS — Recovery Patterns
 * @ingroup cpisi_types
 * @brief    UNIVERSAL error recovery using B.2 types and B.3 helpers.
 *
 * WHY: Graceful recovery patterns for state errors. UNIVERSAL recovery stays
 *      here, domain-specific recovery moves to domain files (domain ownership).
 *
 * SCOPE: Foundation recovery (HebrewState, KFactor).
 *        Health recovery → health.h, DAR cycle → phase.h.
 *
 * CONTAINS:
 *   - B.4.1 State Recovery      — Reset to safe defaults (YASHAR, K_MAINTAIN)
 *   - B.4.2 Graceful Degradation — Step toward center on failure
 *
 * DOMAIN FILES OWN:
 *   - health.h — cpisi_health_recover(), cpisi_health_degrade()
 *   - phase.h  — DAR Cycle (cpisi_dar_detect/assess/restore/cycle)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4.1 State Recovery — Reset to Safe Defaults [RECOVERY]
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief   Recover HebrewState to center (YASHAR)
 * @return  HEBREW_YASHAR (center state)
 * @note    YASHAR is the default/safe state for all recovery
 */
static inline HebrewState cpisi_hebrew_state_recover(void) {
    return HEBREW_YASHAR;
}

/**
 * @brief   Recover KFactor to neutral (K_MAINTAIN)
 * @return  K_MAINTAIN (neutral k-factor)
 * @note    Neutral k-factor prevents directional bias during recovery
 */
static inline KFactor cpisi_kfactor_recover(void) {
    return K_MAINTAIN;
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4.2 Graceful Degradation — Step Toward Center [DEGRADATION]
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief   Get fallback state (safer than current)
 * @param   state   Current Hebrew state
 * @return  HebrewState one step closer to YASHAR
 * @note    Uses cpisi_hebrew_state_step() from B.3c
 */
static inline HebrewState cpisi_state_fallback(HebrewState state) {
    /* Step toward center (YASHAR) */
    const HebrewStateInfo* info = hebrew_state_info(state);
    if (info->value > 0) {
        return cpisi_hebrew_state_step(state, -1);
    } else if (info->value < 0) {
        return cpisi_hebrew_state_step(state, +1);
    }
    return HEBREW_YASHAR;
}

/** @} */ // end Error Handling (B.4)

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Accessors & Formatting [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_api B.5 PUBLIC API — Accessors & Formatting
 * @ingroup cpisi_types
 * @brief    Query-only functions for display, serialization, and string conversion.
 *
 * WHY: Read-only accessor functions that don't modify state. All use config-driven
 *      array lookups from B.2 types. Domain-specific accessors in domain files.
 *
 * SCOPE: Foundation accessors (HebrewState, KFactor).
 *        Health accessors → health.h, DAR accessors → phase.h, Log → log.h.
 *
 * CONTAINS:
 *   - B.5a Lifecycle — (Reserved for source files — init/shutdown/reset)
 *   - B.5b Accessors — Name, fullname, emoji, Hebrew, parse functions
 *   - B.5c Format    — (Reserved for display formatting)
 *   - B.5d Utilities — (Reserved for operational helpers)
 *
 * Design: Read-only functions that don't modify state.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a LIFECYCLE — Context Functions [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_api_lifecycle B.5a LIFECYCLE — Context Functions
 * @ingroup cpisi_api
 * @brief    (Reserved) Init/shutdown/reset functions defined in source files.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Lifecycle functions in source files] */

/** @} */ /* end B.5a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b ACCESSORS — Type Properties [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_api_accessors B.5b ACCESSORS — Type Properties
 * @ingroup cpisi_api
 * @brief    Query functions for type properties (name, emoji, display, parse).
 *
 * WHY: Accessor functions using B.2 config-driven array lookups. UNIVERSAL
 *      accessors stay here, domain-specific move to domain files.
 *
 * SCOPE: HebrewState, KFactor accessors.
 *        HealthLevel → health.h, DARPhase → phase.h.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5b.1 HebrewState Accessors (from B.2a) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * Uses HEBREW_STATE_INFO unified lookup via hebrew_state_info() accessor.
 *
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief   Get short name for HebrewState
 * @param   state   HebrewState enum (0-6)
 * @return  Name string from HEBREW_STATE_INFO[state].name
 * @note    Uses hebrew_state_info() accessor for config-driven lookup
 */
static inline const char* cpisi_hebrew_state_name(HebrewState state) {
    return hebrew_state_info(state)->name;
}

/**
 * @brief   Get full name with meaning for HebrewState
 * @param   state   HebrewState enum (0-6)
 * @return  Fullname string from HEBREW_STATE_INFO[state].fullname
 * @note    Format: "name (meaning)"
 */
static inline const char* cpisi_hebrew_state_fullname(HebrewState state) {
    return hebrew_state_info(state)->fullname;
}

/**
 * @brief   Get emoji symbol for HebrewState
 * @param   state   HebrewState enum (0-6)
 * @return  Emoji string from HEBREW_STATE_INFO[state].emoji
 */
static inline const char* cpisi_hebrew_state_emoji(HebrewState state) {
    return hebrew_state_info(state)->emoji;
}

/**
 * @brief   Get Hebrew script for HebrewState
 * @param   state   HebrewState enum (0-6)
 * @return  Hebrew string from HEBREW_STATE_INFO[state].hebrew
 */
static inline const char* cpisi_hebrew_state_hebrew(HebrewState state) {
    return hebrew_state_info(state)->hebrew;
}

/**
 * @brief   Parse string to HebrewState enum
 * @param   name   State name string (e.g., "shavar", "yashar", "tov")
 * @return  Matching HebrewState enum, or HEBREW_STATE_DEFAULT if not found
 * @note    Case-insensitive first character matching for efficiency
 * @note    Requires CPISI_MODE >= 2 (RUNTIME) for strncmp() function
 */
#if CPISI_MODE >= 2
static inline HebrewState cpisi_hebrew_state_parse(const char* name) {
    if (!name) return HEBREW_STATE_DEFAULT;

    // Check short names
    if (name[0] == 's' || name[0] == 'S') {
        if (strncmp(name, "shavar", 6) == 0) return HEBREW_SHAVAR;
        if (strncmp(name, "shalem", 6) == 0) return HEBREW_SHALEM;
        if (strncmp(name, "sound", 5) == 0) return HEBREW_TAMIM;
    }
    if (name[0] == 'c' || name[0] == 'C') {
        if (strncmp(name, "chaser", 6) == 0) return HEBREW_CHASER;
    }
    if (name[0] == 'r' || name[0] == 'R') {
        if (strncmp(name, "ratsah", 6) == 0) return HEBREW_RATSAH;
    }
    if (name[0] == 'y' || name[0] == 'Y') {
        if (strncmp(name, "yashar", 6) == 0) return HEBREW_YASHAR;
    }
    if (name[0] == 't' || name[0] == 'T') {
        if (strncmp(name, "tamim", 5) == 0) return HEBREW_TAMIM;
        if (strncmp(name, "tov", 3) == 0) return HEBREW_TOV;
    }

    // Check English translations
    if (strncmp(name, "broken", 6) == 0) return HEBREW_SHAVAR;
    if (strncmp(name, "lacking", 7) == 0) return HEBREW_CHASER;
    if (strncmp(name, "wanting", 7) == 0) return HEBREW_RATSAH;
    if (strncmp(name, "even", 4) == 0) return HEBREW_YASHAR;
    if (strncmp(name, "whole", 5) == 0) return HEBREW_SHALEM;
    if (strncmp(name, "perfect", 7) == 0) return HEBREW_TOV;

    return HEBREW_STATE_DEFAULT;
}
#endif /* CPISI_MODE >= 2 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5b.2 KFactor Accessors (from B.2a) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * Uses K_FACTOR_INFO unified lookup via k_factor_info() accessor.
 *
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief   Get name for KFactor
 * @param   k   KFactor enum (-1, 0, +1)
 * @return  Name string from K_FACTOR_INFO[k+1].name
 * @note    Uses kfactor_info() accessor for config-driven lookup
 */
static inline const char* cpisi_kfactor_name(KFactor k) {
    return k_factor_info(k)->name;
}

/**
 * @brief   Get vertical arrow symbol for KFactor
 * @param   k   KFactor enum (-1, 0, +1)
 * @return  Symbol string from K_FACTOR_INFO[k+1].symbol (↓, ◆, ↑)
 */
static inline const char* cpisi_kfactor_symbol(KFactor k) {
    return k_factor_info(k)->symbol;
}

/**
 * @brief   Get horizontal arrow symbol for KFactor
 * @param   k   KFactor enum (-1, 0, +1)
 * @return  Arrow string from K_FACTOR_INFO[k+1].arrow (←, •, →)
 */
static inline const char* cpisi_kfactor_arrow(KFactor k) {
    return k_factor_info(k)->arrow;
}

/**
 * @brief   Parse string to KFactor enum
 * @param   name   KFactor name string (e.g., "retreat", "maintain", "advance")
 * @return  Matching KFactor enum, or K_FACTOR_DEFAULT if not found
 * @note    Case-insensitive first character matching for efficiency
 * @note    Requires CPISI_MODE >= 2 (RUNTIME) for strncmp() function
 */
#if CPISI_MODE >= 2
static inline KFactor cpisi_kfactor_parse(const char* name) {
    if (!name) return K_FACTOR_DEFAULT;

    if (name[0] == 'r' || name[0] == 'R') {
        if (strncmp(name, "retreat", 7) == 0) return K_RETREAT;
    }
    if (name[0] == 'm' || name[0] == 'M') {
        if (strncmp(name, "maintain", 8) == 0) return K_MAINTAIN;
    }
    if (name[0] == 'a' || name[0] == 'A') {
        if (strncmp(name, "advance", 7) == 0) return K_ADVANCE;
    }

    // Check symbols
    if (name[0] == '-' || name[0] == '<') return K_RETREAT;
    if (name[0] == '+' || name[0] == '>') return K_ADVANCE;
    if (name[0] == '0' || name[0] == '=') return K_MAINTAIN;

    return K_FACTOR_DEFAULT;
}
#endif /* CPISI_MODE >= 2 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5b.3 DetectType Accessors (from B.2d) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** @brief Get detect type name (config-driven via detect_type_info()) */
static inline const char* cpisi_detect_type_name(DetectType type) {
    return detect_type_info(type)->name;
}

/** @brief Get detect type emoji (config-driven) */
static inline const char* cpisi_detect_type_emoji(DetectType type) {
    return detect_type_info(type)->emoji;
}

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5b.4 Severity Accessors (from B.2d) — UNIVERSAL
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * NOTE: cpisi_severity_name(idx) macro exists in S.2b for cube severity (0-8 index).
 * These functions use Severity enum (-4 to +4) with severity_info() lookup.
 *
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** @brief Get severity display name (config-driven via severity_info()) */
static inline const char* cpisi_sev_name(Severity sev) {
    return severity_info(sev)->name;
}

/** @} */ /* end B.5b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c FORMAT — Display Formatting [FORMAT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_api_format B.5c FORMAT — Display Formatting
 * @ingroup cpisi_api
 * @brief    (Reserved) Display formatting functions — sprintf-style formatters.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Format functions] */

/** @} */ /* end B.5c FORMAT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d UTILITIES — Operational Helpers [UTILITIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_api_utilities B.5d UTILITIES — Operational Helpers
 * @ingroup cpisi_api
 * @brief    Formatting and display helpers using B.2 config-driven arrays.
 *
 * CONTAINS:
 *   - cpisi_hebrew_zone_color — Zone → ANSI color code
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Get ANSI color code for HebrewZone (uses unified lookup) */
static inline const char* cpisi_hebrew_zone_color(HebrewZone zone) {
    if (!cpisi_zone_valid(zone)) return "\033[0m";  /* Reset */
    return hebrew_zone_info(zone)->color;
}

/** @} */ /* end B.5d UTILITIES */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Intentionally Reserved [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_omission B.6 OMISSION — Intentionally Reserved
 * @ingroup cpisi_types
 * @brief    Documents what is intentionally reserved and why (transparency).
 *
 * | Section                | Status                                          |
 * |------------------------|-------------------------------------------------|
 * | B.1 Org Chart          | Complete — Structure, Flow, Counts              |
 * | B.2 Type Definitions   | Complete — Foundation thru Assessment (a-d)     |
 * | B.3 Helpers            | Complete — Validation, Conversions, Utilities   |
 * | B.4 Error Handling     | Complete — State Recovery, Graceful Degradation |
 * | B.5 Public API         | Complete — Lifecycle, Accessors, Utilities      |
 * | B.6 Omission Guide     | Complete — This section                         |
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/** @name C++ Linkage (Closing)
 *  @brief Matches S.1c opening. Closes extern "C" block.
 *  @{ */
#ifdef __cplusplus
}  /**< End extern C linkage */
#endif
/** @} */ // end C++ Linkage (Closing)

/** @} */ /* end BODY block */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING [CLOSING]
///
/// 4-Block Code Structure: Operational guidance and closing notes.
///
/// Section order: Validation → Execution → Cleanup | Policy → Extension →
///                Troubleshooting → Reference → Note → Omission
/// Flow: verify → run → release | modify → extend → debug → lookup → ground → guide
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
///   - X.9 OMISSION      — What can be reserved in CLOSING
///
/// Builds FROM: BODY block (implementation complete)
/// Builds TO:   Include guard close (#endif)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build, syntax, and self-containment verification.
 *
 * CONTAINS:
 *   - X.1a BUILD     — Compilation commands
 *   - X.1b SELFTEST  — Minimal isolation test
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -fsyntax-only -Wall -Wextra types.h         # syntax check
 * cppcheck --enable=all types.h                   # static analysis
 * @endcode
 *
 * @par X.1b SELFTEST — Minimal Isolation Test
 * @code{.c}
 * // test.c — must compile without errors
 * #include "kernel/cpisi/types/types.h"
 * int main(void) { return 0; }
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_execution X.2 EXECUTION — Usage Patterns
 * @brief    Usage patterns — include and use types.
 *
 * Note: Headers declare, .c files implement. No code executes at include time.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.2a INCLUDE — Include Pattern
 * @code{.c}
 * #include "kernel/cpisi/types/types.h"
 * @endcode
 *
 * @par X.2b USAGE — Usage Example
 * @code{.c}
 * #include "kernel/cpisi/types/types.h"
 *
 * int main(void) {
 *     HebrewState state = HEBREW_YASHAR;
 *     TernaryScore score = { .value = 0.75, .confidence = 0.9 };
 *     return 0;
 * }
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Ownership patterns — types are value types, no allocation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * All types are value types (stack-allocated).
 * No heap allocation, no _create/_destroy needed.
 * Copy by assignment, compare by field.
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
 * @defgroup cpisi_policy X.4 POLICY — Modification Guidance
 * @brief    Guide future maintainers — modification safety levels.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Safe to Modify
 *   - Add new types following existing patterns
 *   - Add new helper functions in B.3
 *   - Extend enums (append, never reorder)
 *
 * @par Modify with Care
 *   - Struct fields — breaks code accessing fields directly
 *   - Enum values — breaks switch statements and lookups
 *   - Scale constants — affects all layer-based sizing
 *
 * @par NEVER Modify
 *   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
 *   - Include guard pattern (CPISI_TYPES_H)
 *   - Ternary foundation (-1, 0, +1 semantics)
 *   - Hebrew state ordering (shavar → yashar → tov)
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_extension X.5 EXTENSION — How to Expand
 * @brief    How to expand this component.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Adding Types
 *   - Add to BODY B.2 in appropriate category
 *   - Follow naming: CamelCase for types, SCREAMING_SNAKE for enums
 *   - Include validation helper in B.3
 *   - Update B.1 Org Chart counts
 *
 * @par Adding Helpers
 *   - Add to BODY B.3 in appropriate category
 *   - Follow pattern: cpisi_[category]_[verb]()
 *   - Document parameters and return values
 *
 * @see M.10 ROADMAP for planned features.
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Problems [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par "Redefinition of HebrewState"
 *   Check include guards, ensure types.h included only once per TU.
 *
 * @par "Incomplete type 'CpisiLogEntry'"
 *   Include types.h before using any cpisi types.
 *
 * @par "Implicit conversion loses precision"
 *   Use explicit casts: (int16_t)health_score
 *
 * @par Scale values seem wrong
 *   Check CPISI_MODE compile flag matches intended layer.
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick lookup — the ternary foundation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par Hebrew States
 *   -1.0 shavar (BROKEN) → 0.0 yashar (EVEN) → +1.0 tov (PERFECT)
 *
 * @par K-Factor
 *   -1 (retreat) → 0 (maintain) → +1 (advance)
 *
 * @par Health Scale
 *   -100 (broken) → 0 (even) → +100 (perfect)
 *
 * @par Log Levels
 *   -1.0 CRISIS → 0.0 INFO → +1.0 INSIGHT
 *
 * @par DAR Witness System
 *   - DETECT  — "Let us search and try our ways" (Lam 3:40)
 *   - ASSESS  — "Examine yourselves" (2 Cor 13:5)
 *   - RESTORE — "He restoreth my soul" (Psalm 23:3)
 *
 * @par Related Files
 *   - Headers: dar/phases/{detect,assess,restore}.h, dar/dar.h, cpisi.h
 *   - Specs:   bereshit/word/core/os/health/{log,diagnostics,restore}.toml
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * Scripture: Leviticus 19:36
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * This file is THE SINGLE SOURCE OF TRUTH for all CPI-SI type definitions.
 * Every other cpisi/{domain}.h file includes this FIRST and uses its types.
 *
 * "Just balances, just weights, a just ephah, and a just hin, shall ye have:
 *  I am the LORD your God, which brought you out of the land of Egypt."
 *    — Leviticus 19:36
 *
 * Atomic normalization — all values on unified scales, traceable to Genesis 1:1.
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — What Can Be Reserved [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_closing_omission X.9 OMISSION — What Can Be Reserved
 * @brief    Guidance for CLOSING sections — what can be reserved.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * CLOSING SECTIONS:
 *   - X.1 Validation:     Always include — all headers need verification
 *   - X.2 Execution:      Always include — usage examples help consumers
 *   - X.3 Cleanup:        Simplified for value types — no heap management
 *   - X.4 Policy:         Always include — guides maintainers
 *   - X.5 Extension:      Always include — documents growth patterns
 *   - X.6 Troubleshoot:   Add issues as they emerge
 *   - X.7 Reference:      Always include — quick lookup essential
 *   - X.8 Note:           Always include — grounding matters
 *
 * Key principle: CLOSING synthesizes, METADATA/SETUP/BODY contain details.
 *
 * @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* Freestanding kernel guard: end of full types.h content */

#endif // CPISI_TYPES_H
