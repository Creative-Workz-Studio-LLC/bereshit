/// @file phase.h
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
/// @omni:req  key        = CPISI-TYPES-PHASE
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
/// @omni:ins  layer      = types/phase
/// @omni:ins  includes   = types.h
/// @omni:ins  provides   = [S.2_PHASE]
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
/// @omni:req  title      = CPI-SI Phase Domain
/// @omni:req  brief      = DAR phases, aspects, and severity matrix
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
 * @defgroup cpisi_phase_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup cpisi_phase_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_phase_special_config
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

#ifndef CPISI_TYPES_PHASE_H
#define CPISI_TYPES_PHASE_H

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
 * @file      phase.h
 * @brief     CPI-SI Phase Domain — DAR phases, aspects, severity matrix.
 *
 * @defgroup cpisi_phase_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-TYPES-PHASE
 * Title:     CPI-SI Phase Domain Types
 * Type:      Header (Domain)
 * Component: Rails — provides phase types for Detect-Assess-Restore paradigm
 * Role:      DAR phases (3), aspects (3), severity rows (3), offset encoding.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_state M.2 STATE — Lifecycle State
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
 * @defgroup cpisi_phase_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_phase_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/types/phase/phase.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: types/types.h (foundation)
 * Template:     bereshit/word/seed/code/c/type-header.h
 * Extracted:    2026-01-21 from types.h phase sections
 *
 * Builds FROM:  types.h (foundation types)
 * Builds TO:    DAR system, error handling, health restoration
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: types, phase, domain, DAR, aspects, severity, 27-cube
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide phase/aspect/severity types for DAR error handling.
 * Core Design: 3×3×3 = 27 cube of phase × aspect × severity.
 * Philosophy:  Orderly phases (Detect→Assess→Restore) enable proper recovery.
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Ecclesiastes 3:1 — "To every thing there is a season"
 * Principle:  Orderly phases enable proper recovery.
 * Anchor:     Isaiah 28:26 — "his God doth instruct him... and doth teach him"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types.h (foundation types)
 *
 * What Uses This:
 *   - DAR system
 *   - Error handling
 *   - Health restoration
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
 * @defgroup cpisi_phase_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-21) — Extracted from types.h phase sections
 *   a-01.25 (2026-01-23) — Expanded METADATA to full M.1-M.10 structure
 *   b-03.00 (2026-01-25) — Full type-header.h template alignment (4-block)
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ DAR phase types defined
 *
 * Planned:
 *   ⏳ Phase transition validation
 *   ⏳ Severity escalation rules
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
///   - S.2 DEFINES   — Phase domain (Bounds, Macros, Verify, Lookup, Types)
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
 * @defgroup cpisi_phase_includes S.1 INCLUDES — Header Dependencies
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
 * @defgroup cpisi_phase_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_phase_includes
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
 * @defgroup cpisi_phase_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_phase_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * PROVIDES: Foundation types, macros, and DAR phase infrastructure.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/config/cfg/domain.cfg.h"  /**< TIER 1: config anchors (brings types.h transitively) */
#include "kernel/types/composite/type/health.type.h"               /**< TIER 2: health types (HealthScore, HealthLevel for DAR) */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_phase_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_phase_includes
 * @brief    External library headers (none for phase domain).
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
 * @defgroup cpisi_phase_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_phase_includes
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
 * S.2 DEFINES — Phase Domain Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_domain S.2 DEFINES — Phase Domain
 * @brief    DAR phase system (Detect-Assess-Restore × 3 severities × 3 aspects).
 *
 * PURPOSE: Define phase-specific constants, operations, verification, and types.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: no domain-specific preludes]
 *   - S.2a  BOUNDS  — Phase counts and ranges
 *   - S.2b  MACROS  — Phase offset operations
 *   - S.2c VERIFY  — Phase compile-time assertions
 *   - S.2d LOOKUP  — Phase tables
 *   - S.2e TYPES   — Phase enumerations
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
 * @defgroup cpisi_phase_prelude S.2.0 PHASE PRELUDE — Domain Operations
 * @ingroup cpisi_phase_domain
 * @brief    [Reserved: no domain-specific preludes]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Phase domain uses foundation PRELUDE operations] */

/** @} */ /* end S.2.0 PRELUDE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BOUNDS — Phase bounds [BOUNDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_phase_bounds S.2a PHASE BOUNDS — Domain Constants
 * @ingroup cpisi_phase_domain
 * @brief    Phase counts, ranges, and dimension bounds.
 *
 * CONTAINS:
 *   - Counts — PHASE_COUNT (3), ASPECT_COUNT (3), SEVERITY_ROW_COUNT (3)
 *   - DAR Phases — DETECT (0), ASSESS (1), RESTORE (2)
 *   - Aspects — ENTRY (0), PROCESS (1), EXIT (2)
 *   - Offsets — 27 offset codes (0-26)
 *
 * Builds FROM: S.2.0 PRELUDE
 * Builds TO:   S.2b MACROS
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name Counts — Phase domain array sizes
 *  @brief PHASE_COUNT (3 DAR), ASPECT_COUNT (3 E/P/X), SEVERITY_ROW_COUNT (3).
 *  @{ */
#define CPISI_PHASE_COUNT           CPISI_TERNARY       /**< 3: DAR phases (D/A/R)                */
#define CPISI_ASPECT_COUNT          CPISI_TERNARY       /**< 3: aspects per phase (E/P/X)         */
#define CPISI_SEVERITY_ROW_COUNT    CPISI_TERNARY       /**< 3: severity matrix rows              */
#define CPISI_CAT_COUNT             CPISI_TERNARY       /**< 3: error categories (CORE/EXT/SYS)   */
/** @} */ /* end Counts */

/** @name DAR Phases — Detect, Assess, Restore
 *  @brief The error handling paradigm: D (witness), A (evaluate), R (recover).
 *  @{ */
#define CPISI_PHASE_DETECT          0       /**< D: detect (witness) [MIN]                */
#define CPISI_PHASE_ASSESS          1       /**< A: assess (evaluate) [CENTER]            */
#define CPISI_PHASE_RESTORE         2       /**< R: restore (recover) [MAX]               */
#define CPISI_PHASE_HALF            CPISI_HALF(CPISI_PHASE_COUNT)           /**< 1: for bal convert   */
/**
 * @brief 0: minimum phase — DETECT.
 * @par Derivation
 *      PHASE_MIN = VOID = MIN_OF(PHASE_COUNT) = 0
 */
#define CPISI_PHASE_MIN             CPISI_VOID
/**
 * @brief 1: center phase — ASSESS.
 * @par Derivation
 *      PHASE_CENTER = CENTER_OF(PHASE_COUNT) = CENTER_OF(3) = 1
 */
#define CPISI_PHASE_CENTER          CPISI_CENTER_OF(CPISI_PHASE_COUNT)
/**
 * @brief 2: maximum phase — RESTORE.
 * @par Derivation
 *      PHASE_MAX = DIFF(PHASE_COUNT, UNITY) = MAX_OF(PHASE_COUNT) = 2
 */
#define CPISI_PHASE_MAX             CPISI_DIFF(CPISI_PHASE_COUNT, CPISI_UNITY)
/** @} */ /* end DAR Phases */

/** @name Aspects — Entry, Process, Exit
 *  @brief Each phase has 3 aspects: E (init), P (handle), X (cleanup).
 *  @{ */
#define CPISI_ASPECT_ENTRY          0       /**< E: entry (init) [MIN]                    */
#define CPISI_ASPECT_PROCESS        1       /**< P: process (handle) [CENTER]             */
#define CPISI_ASPECT_EXIT           2       /**< X: exit (cleanup) [MAX]                  */
#define CPISI_ASPECT_HALF           CPISI_HALF(CPISI_ASPECT_COUNT)          /**< 1: for bal convert   */
/**
 * @brief 0: minimum aspect — ENTRY.
 * @par Derivation
 *      ASPECT_MIN = VOID = MIN_OF(ASPECT_COUNT) = 0
 */
#define CPISI_ASPECT_MIN            CPISI_VOID
/**
 * @brief 1: center aspect — PROCESS.
 * @par Derivation
 *      ASPECT_CENTER = CENTER_OF(ASPECT_COUNT) = CENTER_OF(3) = 1
 */
#define CPISI_ASPECT_CENTER         CPISI_CENTER_OF(CPISI_ASPECT_COUNT)
/**
 * @brief 2: maximum aspect — EXIT.
 * @par Derivation
 *      ASPECT_MAX = DIFF(ASPECT_COUNT, UNITY) = MAX_OF(ASPECT_COUNT) = 2
 */
#define CPISI_ASPECT_MAX            CPISI_DIFF(CPISI_ASPECT_COUNT, CPISI_UNITY)
/* LOCAL — position within phase (9 = 3 severity × 3 aspect, guarded) */
#ifndef CPISI_LOCAL_MIN
#define CPISI_LOCAL_MIN             CPISI_VOID                              /**< 0: first local       */
#endif
#ifndef CPISI_LOCAL_MAX
#define CPISI_LOCAL_MAX             CPISI_DIFF(CPISI_FACE, CPISI_UNITY)     /**< 8: last local        */
#endif
/** @} */ /* end Aspects */

/** @name Severity Rows — MILD, MEDIUM, CRITICAL
 *  @brief Severity rows within each DAR phase (3 rows × 3 aspects = 9 positions).
 *  @{ */
#define CPISI_SEVERITY_ROW_MILD         0   /**< M: warning [MIN]                         */
#define CPISI_SEVERITY_ROW_MEDIUM       1   /**< E: degraded [CENTER]                     */
#define CPISI_SEVERITY_ROW_CRITICAL     2   /**< C: fatal [MAX]                           */
#define CPISI_SEVERITY_ROW_HALF     CPISI_HALF(CPISI_SEVERITY_ROW_COUNT) /**< 1: for bal convert   */
/**
 * @brief 0: minimum severity — MILD.
 * @par Derivation
 *      SEVERITY_ROW_MIN = VOID = MIN_OF(SEVERITY_ROW_COUNT) = 0
 */
#define CPISI_SEVERITY_ROW_MIN      CPISI_VOID
/**
 * @brief 1: center severity — MEDIUM.
 * @par Derivation
 *      SEVERITY_ROW_CENTER = CENTER_OF(SEVERITY_ROW_COUNT) = 1
 */
#define CPISI_SEVERITY_ROW_CENTER   CPISI_CENTER_OF(CPISI_SEVERITY_ROW_COUNT)
/**
 * @brief 2: maximum severity — CRITICAL.
 * @par Derivation
 *      SEVERITY_ROW_MAX = DIFF(SEVERITY_ROW_COUNT, UNITY) = 2
 */
#define CPISI_SEVERITY_ROW_MAX      CPISI_DIFF(CPISI_SEVERITY_ROW_COUNT, CPISI_UNITY)
/* SEVERITY ROW BALANCED VALUES */
#define CPISI_SEVERITY_ROW_BAL_MILD     CPISI_SIGN_NEG  /**< -1: mild                   */
#define CPISI_SEVERITY_ROW_BAL_MEDIUM   CPISI_SIGN_ZERO /**< 0: medium                  */
#define CPISI_SEVERITY_ROW_BAL_CRITICAL CPISI_SIGN_POS  /**< +1: critical               */
/* SEVERITY ROW VERSION CHARS */
#define CPISI_SEVERITY_ROW_VERSION_MILD     'a' /**< alpha: mild                    */
#define CPISI_SEVERITY_ROW_VERSION_MEDIUM   'b' /**< beta: medium                   */
#define CPISI_SEVERITY_ROW_VERSION_CRITICAL 'c' /**< stable: critical               */
/** @} */ /* end Severity Rows */

/** @} */ /* end S.2a BOUNDS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b MACROS — Phase operations [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_phase_macros S.2b PHASE MACROS — Operations
 * @ingroup cpisi_phase_domain
 * @brief    Offset construction and extraction for DAR phase system.
 *
 * PURPOSE: Provide phase-specific operations for the 27-position DAR cube
 *          (DETECT × ASSESS × RESTORE) using BOUNDS from types.h.
 *
 * Builds FROM: types.h (S.2a.2 DIMENSIONS, S.2a.6 PHASES, S.2b.2 PRIMITIVES)
 * Builds TO:   Error handling (DAR navigation), health recovery (phase tracking)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC extensions to S.2b MACROS defined in
 *   types.h. The foundation macros (QUOTIENT, MODULO, SUM, PRODUCT) come from
 *   types.h. This file adds DAR phase navigation macros.
 *
 * STRUCTURE:
 *   S.2b.7c PHASE DOMAIN — Offset construction and extraction
 *     S.2b.7c.1 BUILD     — Construct offset from phase/severity/aspect
 *     S.2b.7c.2 EXTRACT   — Extract components from offset
 *     S.2b.7c.3 CONVERT   — Index ↔ balanced transforms
 *     S.2b.7c.4 VALIDATE  — Range checks
 *     S.2b.7c.5 NAMED     — 27 named offset constants
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2b_7c_PHASE_DOMAIN S.2b.7c PHASE DOMAIN — Offset operations
 * @ingroup cpisi_phase_macros
 * @brief Offset construction and extraction macros for DAR phase system.
 *
 * MATHEMATICAL STRUCTURE:
 *   The 27-position cube (CUBE = 3³) represents the DAR error handling space.
 *   Each offset 0-26 encodes: PHASE (D/A/R) × SEVERITY × ASPECT
 *
 *   ┌─────────────────────────────────────────────────────────────────────────┐
 *   │  27-POSITION OFFSET ENCODING                                           │
 *   │  ────────────────────────────────────────────────────────────────────   │
 *   │  OFFSET = PHASE × FACE + SEVERITY × TERNARY + ASPECT                   │
 *   │         = PHASE × 9 + SEVERITY × 3 + ASPECT                            │
 *   │                                                                        │
 *   │  ┌─────────────────────────────────────────────────────────────────┐   │
 *   │  │  PHASE (0-2)     │  SEVERITY (0-2)    │  ASPECT (0-2)           │   │
 *   │  │  ──────────────  │  ────────────────  │  ─────────────────────  │   │
 *   │  │  0 = DETECT      │  0 = MILD          │  0 = ENTRY (N-)         │   │
 *   │  │  1 = ASSESS      │  1 = MEDIUM        │  1 = PROCESS (P)        │   │
 *   │  │  2 = RESTORE     │  2 = CRITICAL      │  2 = EXIT (X+)          │   │
 *   │  └─────────────────────────────────────────────────────────────────┘   │
 *   │                                                                        │
 *   │  OFFSET 13 = ASSESS:MEDIUM:PROCESS = 1×9 + 1×3 + 1 = CENTER            │
 *   └─────────────────────────────────────────────────────────────────────────┘
 *
 * STRUCTURE:
 *   S.2b.7c.1 BUILD     — Construct offset from phase/severity/aspect
 *   S.2b.7c.2 EXTRACT   — Extract phase/local/severity/aspect from offset
 *   S.2b.7c.3 CONVERT   — Index ↔ balanced transforms
 *   S.2b.7c.4 VALIDATE  — Range checks for phase/offset components
 *   S.2b.7c.5 NAMED     — 27 named offset constants
 *
 * BUILDS FROM: S.2a.2 DIMENSIONS (FACE=9, TERNARY=3), S.2a.6 PHASES
 * DEPENDS ON:  S.2b.2 PRIMITIVES (QUOTIENT, MODULO, SUM, PRODUCT, DIFF)
 * @{ */

/* ── S.2b.7c.1 BUILD — Construct offset from components ─────────────────────── */

/**
 * @brief Build offset from phase, severity, and aspect indices.
 *
 * BOUNDS USED:
 *   - CPISI_FACE = 9 (positions per phase)
 *   - CPISI_TERNARY = 3 (aspects per severity)
 *
 * FORMULA: offset = phase × FACE + severity × TERNARY + aspect
 *                 = phase × 9 + severity × 3 + aspect
 *
 * DERIVATION:
 *   - 27-position cube = 3 phases × 3 severities × 3 aspects
 *   - Phase selects which 9-block (FACE)
 *   - Severity selects which 3-block (TERNARY) within phase
 *   - Aspect selects position within severity
 *
 * EXAMPLES:
 *   OFFSET_BUILD(0, 0, 0) = 0×9 + 0×3 + 0 = 0  (D:MILD:ENTRY)
 *   OFFSET_BUILD(1, 1, 1) = 1×9 + 1×3 + 1 = 13 (A:MED:PROC = CENTER)
 *   OFFSET_BUILD(2, 2, 2) = 2×9 + 2×3 + 2 = 26 (R:CRIT:EXIT)
 *
 * @param phase Phase index (0-2: DETECT/ASSESS/RESTORE)
 * @param severity Severity index (0-2: MILD/MEDIUM/CRITICAL)
 * @param aspect Aspect index (0-2: ENTRY/PROCESS/EXIT)
 * @return Offset (0-26)
 */
#define CPISI_OFFSET_BUILD(phase, severity, aspect) \
    CPISI_SUM(CPISI_PRODUCT((phase), CPISI_FACE), CPISI_SUM(CPISI_PRODUCT((severity), CPISI_TERNARY), (aspect)))

/**
 * @brief Build offset from balanced phase, severity, and aspect.
 *
 * BOUNDS USED:
 *   - PHASE_HALF = 1, SEVERITY_ROW_HALF = 1, ASPECT_HALF = 1
 *   - All have range 0-2, so HALF = (3-1)/2 = 1
 *
 * FORMULA: offset = OFFSET_BUILD(bal+1, bal+1, bal+1) for each component
 *
 * DERIVATION:
 *   - Balanced form centers on 0: -1, 0, +1
 *   - Index form starts at 0: 0, 1, 2
 *   - Adding HALF (1) converts balanced → index
 *   - Then use standard OFFSET_BUILD
 *
 * EXAMPLES:
 *   OFFSET_BUILD_BAL(-1, -1, -1) → OFFSET_BUILD(0, 0, 0) = 0  (D:MILD:ENTRY)
 *   OFFSET_BUILD_BAL( 0,  0,  0) → OFFSET_BUILD(1, 1, 1) = 13 (CENTER)
 *   OFFSET_BUILD_BAL(+1, +1, +1) → OFFSET_BUILD(2, 2, 2) = 26 (R:CRIT:EXIT)
 *
 * @param phase_bal Balanced phase (-1/0/+1)
 * @param severity_bal Balanced severity (-1/0/+1)
 * @param aspect_bal Balanced aspect (-1/0/+1)
 * @return Offset (0-26)
 */
#define CPISI_OFFSET_BUILD_BAL(phase_bal, severity_bal, aspect_bal) \
    CPISI_OFFSET_BUILD(CPISI_BAL_TO_PHASE(phase_bal), CPISI_BAL_TO_SEVERITY_ROW(severity_bal), CPISI_BAL_TO_ASPECT(aspect_bal))

/* ── S.2b.7c.2 EXTRACT — Extract components from offset ─────────────────────── */

/**
 * EXTRACTION PATTERN:
 *   offset (0-26) splits into phase (0-2) and local (0-8)
 *   local (0-8) splits into severity (0-2) and aspect (0-2)
 *
 *   ┌───────────────────────────────────────────────────────────────────────┐
 *   │  OFFSET (0-26)                                                       │
 *   │    ├── PHASE (offset ÷ 9)     → 0-2  (which 9-block)                 │
 *   │    └── LOCAL (offset mod 9)   → 0-8  (position in 9-block)           │
 *   │          ├── SEVERITY (local ÷ 3)   → 0-2  (which 3-row)             │
 *   │          └── ASPECT (local mod 3)   → 0-2  (position in 3-row)       │
 *   └───────────────────────────────────────────────────────────────────────┘
 */

/** @brief Extract phase from offset. FORMULA: phase = offset ÷ FACE = offset ÷ 9 */
#define CPISI_OFFSET_PHASE(offset)          CPISI_QUOTIENT((offset), CPISI_FACE)

/** @brief Extract local position from offset. FORMULA: local = offset mod FACE = offset mod 9 */
#define CPISI_OFFSET_LOCAL(offset)          CPISI_MODULO((offset), CPISI_FACE)

/** @brief Extract severity from local. FORMULA: severity = local ÷ TERNARY = local ÷ 3 */
#define CPISI_OFFSET_SEVERITY_ROW(local)    CPISI_QUOTIENT((local), CPISI_TERNARY)

/** @brief Extract aspect from local. FORMULA: aspect = local mod TERNARY = local mod 3 */
#define CPISI_OFFSET_ASPECT(local)          CPISI_MODULO((local), CPISI_TERNARY)

/** @brief Alias: Extract phase from offset */
#define CPISI_OFFSET_GET_PHASE(offset)        CPISI_OFFSET_PHASE(offset)

/** @brief Compound: Extract severity from offset (via local) */
#define CPISI_OFFSET_GET_SEVERITY_ROW(offset) CPISI_OFFSET_SEVERITY_ROW(CPISI_OFFSET_LOCAL(offset))

/** @brief Compound: Extract aspect from offset (via local) */
#define CPISI_OFFSET_GET_ASPECT(offset)       CPISI_OFFSET_ASPECT(CPISI_OFFSET_LOCAL(offset))

/** @brief Alias: category = phase (DAR phases map to error categories) */
#define CPISI_OFFSET_CATEGORY(offset)         CPISI_OFFSET_PHASE(offset)

/* ── S.2b.7c.3 CONVERT — Index ↔ balanced transforms ────────────────────────── */

/**
 * CONVERSION PATTERN:
 *   All phase components have range 0-2, so HALF = (3-1)/2 = 1
 *
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  INDEX (0-2)         BALANCED (-1..+1)                              │
 *   │  ─────────────       ────────────────                               │
 *   │  0 (first)           -1 (negative)                                  │
 *   │  1 (center)           0 (neutral)   ◀─── CENTER                     │
 *   │  2 (last)            +1 (positive)                                  │
 *   │                                                                     │
 *   │  TO_BAL: idx - HALF = idx - 1                                       │
 *   │  TO_IDX: bal + HALF = bal + 1                                       │
 *   └─────────────────────────────────────────────────────────────────────┘
 */

/** @brief Phase index → balanced. FORMULA: bal = idx - 1 */
#define CPISI_PHASE_TO_BAL(idx)         CPISI_DIFF((idx), CPISI_PHASE_HALF)

/** @brief Balanced → phase index. FORMULA: idx = bal + 1 */
#define CPISI_BAL_TO_PHASE(bal)         CPISI_SUM((bal), CPISI_PHASE_HALF)

/** @brief Category index → balanced. ALIAS: Same as phase (categories ARE phases in error context) */
#define CPISI_CAT_TO_BAL(idx)           CPISI_PHASE_TO_BAL(idx)

/** @brief Balanced → category index. ALIAS: Same as phase */
#define CPISI_BAL_TO_CAT(bal)           CPISI_BAL_TO_PHASE(bal)

/** @brief Severity index → balanced. FORMULA: bal = idx - 1 */
#define CPISI_SEVERITY_ROW_TO_BAL(idx)  CPISI_DIFF((idx), CPISI_SEVERITY_ROW_HALF)

/** @brief Balanced → severity index. FORMULA: idx = bal + 1 */
#define CPISI_BAL_TO_SEVERITY_ROW(bal)  CPISI_SUM((bal), CPISI_SEVERITY_ROW_HALF)

/** @brief Aspect index → balanced. FORMULA: bal = idx - 1 */
#define CPISI_ASPECT_TO_BAL(idx)        CPISI_DIFF((idx), CPISI_ASPECT_HALF)

/** @brief Balanced → aspect index. FORMULA: idx = bal + 1 */
#define CPISI_BAL_TO_ASPECT(bal)        CPISI_SUM((bal), CPISI_ASPECT_HALF)

/* ── S.2b.7c.4 VALIDATE — Range checks ──────────────────────────────────────── */

/**
 * VALIDATION RANGES:
 *   ┌────────────────────────────────────────────────────────────────────────┐
 *   │  Component       MIN    MAX    Range                                   │
 *   │  ─────────────   ────   ────   ───────────────────────────────────     │
 *   │  PHASE           0      2      0-2 (D/A/R)                             │
 *   │  SEVERITY        0      2      0-2 (M/E/C)                             │
 *   │  ASPECT          0      2      0-2 (N/P/X)                             │
 *   │  LOCAL           0      8      0-8 (position within phase)             │
 *   │  OFFSET          0      26     0-26 (full cube position)               │
 *   └────────────────────────────────────────────────────────────────────────┘
 */

/** @brief Check if phase index is valid (0-2). */
#define CPISI_IS_PHASE(phase)       CPISI_IN_RANGE((phase), CPISI_PHASE_MIN, CPISI_PHASE_MAX)

/** @brief Check if severity index is valid (0-2). */
#define CPISI_IS_SEVERITY_ROW(sev)  CPISI_IN_RANGE((sev), CPISI_SEVERITY_ROW_MIN, CPISI_SEVERITY_ROW_MAX)

/** @brief Check if aspect index is valid (0-2). */
#define CPISI_IS_ASPECT(asp)        CPISI_IN_RANGE((asp), CPISI_ASPECT_MIN, CPISI_ASPECT_MAX)

/** @brief Check if offset is valid (0-26). */
#define CPISI_IS_OFFSET(offset)     CPISI_IN_RANGE((offset), CPISI_OFFSET_MIN, CPISI_OFFSET_MAX)

/** @brief Check if local position is valid (0-8). */
#define CPISI_IS_LOCAL(local)       CPISI_IN_RANGE((local), CPISI_LOCAL_MIN, CPISI_LOCAL_MAX)

/** @brief Alias: IS_PHASE */
#define CPISI_PHASE_VALID(phase)        CPISI_IS_PHASE(phase)

/** @brief Alias: IS_SEVERITY_ROW */
#define CPISI_SEVERITY_ROW_VALID(sev)   CPISI_IS_SEVERITY_ROW(sev)

/** @brief Alias: IS_ASPECT */
#define CPISI_ASPECT_VALID(asp)         CPISI_IS_ASPECT(asp)

/** @brief Alias: IS_OFFSET */
#define CPISI_OFFSET_VALID(offset)      CPISI_IS_OFFSET(offset)

/**
 * @brief Check if offset belongs to specified phase.
 *
 * FORMULA: PHASE_OFFSET_MIN(p) ≤ offset ≤ PHASE_OFFSET_MAX(p)
 *          where MIN = p×9, MAX = p×9 + 8
 *
 * USE CASE: Verify error offset matches expected DAR phase.
 */
#define CPISI_OFFSET_IN_PHASE(offset, phase) \
    CPISI_IN_RANGE((offset), CPISI_PHASE_OFFSET_MIN(phase), CPISI_PHASE_OFFSET_MAX(phase))

/* ── S.2b.7c.5 NAMED OFFSETS — 27 offset constants ──────────────────────────── */

/**
 * ALL 27 NAMED OFFSETS (3 phases × 3 severities × 3 aspects)
 *
 * Each constant computed via OFFSET_BUILD(phase, severity, aspect):
 *   offset = phase × 9 + severity × 3 + aspect
 *
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │  DETECT (0-8)           ASSESS (9-17)          RESTORE (18-26)            │
 * │  ─────────────────      ─────────────────      ─────────────────          │
 * │  0  D:MILD:ENTRY        9  A:MILD:ENTRY        18 R:MILD:ENTRY            │
 * │  1  D:MILD:PROC         10 A:MILD:PROC         19 R:MILD:PROC             │
 * │  2  D:MILD:EXIT         11 A:MILD:EXIT         20 R:MILD:EXIT             │
 * │  3  D:MED:ENTRY         12 A:MED:ENTRY         21 R:MED:ENTRY             │
 * │  4  D:MED:PROC ◀CENTER  13 A:MED:PROC ◀GLOBAL  22 R:MED:PROC ◀CENTER      │
 * │  5  D:MED:EXIT          14 A:MED:EXIT          23 R:MED:EXIT              │
 * │  6  D:CRIT:ENTRY        15 A:CRIT:ENTRY        24 R:CRIT:ENTRY            │
 * │  7  D:CRIT:PROC         16 A:CRIT:PROC         25 R:CRIT:PROC             │
 * │  8  D:CRIT:EXIT         17 A:CRIT:EXIT         26 R:CRIT:EXIT             │
 * └────────────────────────────────────────────────────────────────────────────┘
 *
 * NAMING CONVENTION: OFF_<phase>_<severity>_<aspect>
 *   Phase:    D = DETECT, A = ASSESS, R = RESTORE
 *   Severity: MILD, MED (medium), CRIT (critical)
 *   Aspect:   ENTRY, PROC (process), EXIT
 *
 * CENTER POSITIONS (MED:PROC in each phase):
 *   DETECT_CENTER  = 4  = OFFSET_BUILD(0, 1, 1)
 *   ASSESS_CENTER  = 13 = OFFSET_BUILD(1, 1, 1) ← GLOBAL CENTER (1,1,1)
 *   RESTORE_CENTER = 22 = OFFSET_BUILD(2, 1, 1)
 */

/* DETECT phase offsets (0-8) */
#define CPISI_OFF_D_MILD_ENTRY  CPISI_OFFSET_BUILD(CPISI_PHASE_DETECT, CPISI_SEVERITY_ROW_MILD, CPISI_ASPECT_ENTRY)     /*  0 */
#define CPISI_OFF_D_MILD_PROC   CPISI_OFFSET_BUILD(CPISI_PHASE_DETECT, CPISI_SEVERITY_ROW_MILD, CPISI_ASPECT_PROCESS)   /*  1 */
#define CPISI_OFF_D_MILD_EXIT   CPISI_OFFSET_BUILD(CPISI_PHASE_DETECT, CPISI_SEVERITY_ROW_MILD, CPISI_ASPECT_EXIT)      /*  2 */
#define CPISI_OFF_D_MED_ENTRY   CPISI_OFFSET_BUILD(CPISI_PHASE_DETECT, CPISI_SEVERITY_ROW_MEDIUM, CPISI_ASPECT_ENTRY)   /*  3 */
#define CPISI_OFF_D_MED_PROC    CPISI_OFFSET_BUILD(CPISI_PHASE_DETECT, CPISI_SEVERITY_ROW_MEDIUM, CPISI_ASPECT_PROCESS) /*  4 */
#define CPISI_OFF_D_MED_EXIT    CPISI_OFFSET_BUILD(CPISI_PHASE_DETECT, CPISI_SEVERITY_ROW_MEDIUM, CPISI_ASPECT_EXIT)    /*  5 */
#define CPISI_OFF_D_CRIT_ENTRY  CPISI_OFFSET_BUILD(CPISI_PHASE_DETECT, CPISI_SEVERITY_ROW_CRITICAL, CPISI_ASPECT_ENTRY) /*  6 */
#define CPISI_OFF_D_CRIT_PROC   CPISI_OFFSET_BUILD(CPISI_PHASE_DETECT, CPISI_SEVERITY_ROW_CRITICAL, CPISI_ASPECT_PROCESS) /* 7 */
#define CPISI_OFF_D_CRIT_EXIT   CPISI_OFFSET_BUILD(CPISI_PHASE_DETECT, CPISI_SEVERITY_ROW_CRITICAL, CPISI_ASPECT_EXIT)  /*  8 */

/* ASSESS phase offsets (9-17) */
#define CPISI_OFF_A_MILD_ENTRY  CPISI_OFFSET_BUILD(CPISI_PHASE_ASSESS, CPISI_SEVERITY_ROW_MILD, CPISI_ASPECT_ENTRY)     /*  9 */
#define CPISI_OFF_A_MILD_PROC   CPISI_OFFSET_BUILD(CPISI_PHASE_ASSESS, CPISI_SEVERITY_ROW_MILD, CPISI_ASPECT_PROCESS)   /* 10 */
#define CPISI_OFF_A_MILD_EXIT   CPISI_OFFSET_BUILD(CPISI_PHASE_ASSESS, CPISI_SEVERITY_ROW_MILD, CPISI_ASPECT_EXIT)      /* 11 */
#define CPISI_OFF_A_MED_ENTRY   CPISI_OFFSET_BUILD(CPISI_PHASE_ASSESS, CPISI_SEVERITY_ROW_MEDIUM, CPISI_ASPECT_ENTRY)   /* 12 */
#define CPISI_OFF_A_MED_PROC    CPISI_OFFSET_BUILD(CPISI_PHASE_ASSESS, CPISI_SEVERITY_ROW_MEDIUM, CPISI_ASPECT_PROCESS) /* 13 */
#define CPISI_OFF_A_MED_EXIT    CPISI_OFFSET_BUILD(CPISI_PHASE_ASSESS, CPISI_SEVERITY_ROW_MEDIUM, CPISI_ASPECT_EXIT)    /* 14 */
#define CPISI_OFF_A_CRIT_ENTRY  CPISI_OFFSET_BUILD(CPISI_PHASE_ASSESS, CPISI_SEVERITY_ROW_CRITICAL, CPISI_ASPECT_ENTRY) /* 15 */
#define CPISI_OFF_A_CRIT_PROC   CPISI_OFFSET_BUILD(CPISI_PHASE_ASSESS, CPISI_SEVERITY_ROW_CRITICAL, CPISI_ASPECT_PROCESS) /* 16 */
#define CPISI_OFF_A_CRIT_EXIT   CPISI_OFFSET_BUILD(CPISI_PHASE_ASSESS, CPISI_SEVERITY_ROW_CRITICAL, CPISI_ASPECT_EXIT)  /* 17 */

/* RESTORE phase offsets (18-26) */
#define CPISI_OFF_R_MILD_ENTRY  CPISI_OFFSET_BUILD(CPISI_PHASE_RESTORE, CPISI_SEVERITY_ROW_MILD, CPISI_ASPECT_ENTRY)    /* 18 */
#define CPISI_OFF_R_MILD_PROC   CPISI_OFFSET_BUILD(CPISI_PHASE_RESTORE, CPISI_SEVERITY_ROW_MILD, CPISI_ASPECT_PROCESS)  /* 19 */
#define CPISI_OFF_R_MILD_EXIT   CPISI_OFFSET_BUILD(CPISI_PHASE_RESTORE, CPISI_SEVERITY_ROW_MILD, CPISI_ASPECT_EXIT)     /* 20 */
#define CPISI_OFF_R_MED_ENTRY   CPISI_OFFSET_BUILD(CPISI_PHASE_RESTORE, CPISI_SEVERITY_ROW_MEDIUM, CPISI_ASPECT_ENTRY)  /* 21 */
#define CPISI_OFF_R_MED_PROC    CPISI_OFFSET_BUILD(CPISI_PHASE_RESTORE, CPISI_SEVERITY_ROW_MEDIUM, CPISI_ASPECT_PROCESS) /* 22 */
#define CPISI_OFF_R_MED_EXIT    CPISI_OFFSET_BUILD(CPISI_PHASE_RESTORE, CPISI_SEVERITY_ROW_MEDIUM, CPISI_ASPECT_EXIT)   /* 23 */
#define CPISI_OFF_R_CRIT_ENTRY  CPISI_OFFSET_BUILD(CPISI_PHASE_RESTORE, CPISI_SEVERITY_ROW_CRITICAL, CPISI_ASPECT_ENTRY) /* 24 */
#define CPISI_OFF_R_CRIT_PROC   CPISI_OFFSET_BUILD(CPISI_PHASE_RESTORE, CPISI_SEVERITY_ROW_CRITICAL, CPISI_ASPECT_PROCESS) /* 25 */
#define CPISI_OFF_R_CRIT_EXIT   CPISI_OFFSET_BUILD(CPISI_PHASE_RESTORE, CPISI_SEVERITY_ROW_CRITICAL, CPISI_ASPECT_EXIT) /* 26 */

/* Phase center constants (MED:PROC in each phase) */
#define CPISI_OFF_DETECT_CENTER     CPISI_OFF_D_MED_PROC    /**<  4: DETECT center  */
#define CPISI_OFF_ASSESS_CENTER     CPISI_OFF_A_MED_PROC    /**< 13: ASSESS center  */
#define CPISI_OFF_RESTORE_CENTER    CPISI_OFF_R_MED_PROC    /**< 22: RESTORE center */


/** @} */ /* end S.2b.7c PHASE DOMAIN */
/** @} */ /* end S.2b PHASE MACROS */

// ─── SED HERE: S.2b.7d METADATA_DOMAIN ─── see types-metadata.h ───────────────

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c VERIFY — Phase compile-time assertions [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_phase_verify S.2c PHASE VERIFY — Compile-time assertions
 * @ingroup cpisi_phase_domain
 * @brief    Verify DAR phase/offset architecture at compile time.
 *
 * PURPOSE: Catch phase configuration errors BEFORE runtime. Zero runtime cost.
 *          If any assertion fails, compilation stops with a descriptive message.
 *
 * Builds FROM: S.2a PHASE BOUNDS (phase values), S.2b PHASE MACROS (derivations)
 * Builds TO:   DAR error handling systems (verified values ready for use)
 *
 * CONNECTION TO TYPES.H:
 *   This file provides DOMAIN-SPECIFIC tripwires extending S.2c VERIFY in types.h.
 *   Verifies the 27-position DAR cube: phase indices, offset formula, centers.
 *
 * TRIPWIRE PHILOSOPHY:
 *   Each value is verified TWO ways:
 *   1. VALUE TRIPWIRE:        Assert the literal (e.g., PHASE_COUNT == 3)
 *   2. RELATIONSHIP TRIPWIRE: Assert the derivation (e.g., CENTER_OFFSET == 13)
 *   BOTH together catches drift in either direction.
 *
 * STRUCTURE:
 *   S.2c.7 PHASE DOMAIN — Verify DAR phase system
 *     S.2c.7a VALUE       — Phase literals (DETECT, ASSESS, RESTORE indices)
 *     S.2c.7b RELATIONSHIP — Derivations from FACE, TERNARY
 *     S.2c.7c CENTERS     — Center offset calculations (4, 13, 22)
 *     S.2c.7d BUILD       — Offset formula verification
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2c_7_PHASE_DOMAIN S.2c.7 PHASE DOMAIN — DAR phase verification
 * @ingroup cpisi_phase_verify
 * @brief Compile-time verification of DAR phase/offset system.
 *
 * ════════════════════════════════════════════════════════════════════════════
 *
 * **DOMAIN-SPECIFIC — Will split to types-phase.h**
 *
 * Verifies the complete DAR (Detect-Assess-Restore) phase system with its
 * 3×3×3 = 27 offset cube. Each verification uses the TRIPWIRE philosophy:
 * VALUE tripwires verify literals, RELATIONSHIP tripwires verify derivations.
 *
 * ────────────────────────────────────────────────────────────────────────────
 *
 * THE DAR PHASE CUBE:
 *
 *   DAR phases organize health operations into a cognitive flow:
 *
 *   ┌─────────────────────────────────────────────────────────────────────────┐
 *   │  PHASE       INDEX   OFFSET_RANGE   CENTER    FUNCTION                  │
 *   │  ──────────  ──────  ─────────────  ────────  ───────────────────────   │
 *   │  DETECT      0       0..8           4         Notice and identify       │
 *   │  ASSESS      1       9..17          13        Evaluate and weigh        │
 *   │  RESTORE     2       18..26         22        Act and recover           │
 *   └─────────────────────────────────────────────────────────────────────────┘
 *
 *   Each phase contains 9 offsets (FACE = 3²):
 *   - 3 severity levels: MILD, MEDIUM, CRITICAL
 *   - 3 aspects per severity: ENTRY, PROCESS, EXIT
 *
 * ────────────────────────────────────────────────────────────────────────────
 *
 * THE OFFSET FORMULA:
 *
 *   offset = (phase × FACE) + (severity × TERNARY) + aspect
 *
 *   Where:
 *   - phase    ∈ {0=DETECT, 1=ASSESS, 2=RESTORE}
 *   - severity ∈ {0=MILD, 1=MEDIUM, 2=CRITICAL}
 *   - aspect   ∈ {0=ENTRY, 1=PROCESS, 2=EXIT}
 *
 *   This creates CUBE (27) unique offsets from TERNARY³ combinations.
 *
 * ────────────────────────────────────────────────────────────────────────────
 *
 * STRUCTURE:
 *
 *   ┌─────────────────────────────────────────────────────────────────────────┐
 *   │  S.2c.7a VALUE        — Verify phase/aspect/offset literals            │
 *   │  S.2c.7b RELATIONSHIP — Verify derivations from primitives             │
 *   │  S.2c.7c CENTERS      — Verify center calculations via HALF()          │
 *   │  S.2c.7d BUILD        — Verify offset formula produces correct codes   │
 *   └─────────────────────────────────────────────────────────────────────────┘
 *
 * ════════════════════════════════════════════════════════════════════════════
 *
 * @ingroup S2c_VERIFY
 * @{ */

/**
 * @defgroup S2c_7a_VALUE S.2c.7a VALUE TRIPWIRES — Verify phase literals
 * @ingroup S2c_7_PHASE_DOMAIN
 * @brief Verify literal values for phase system counts and bounds.
 *
 * COUNTS VERIFIED:
 *   - PHASE_COUNT = 3 (DETECT, ASSESS, RESTORE)
 *   - SEVERITY_ROW_COUNT = 3 (MILD, MEDIUM, CRITICAL)
 *   - ASPECT_COUNT = 3 (ENTRY, PROCESS, EXIT)
 *   - OFFSET_COUNT = 27 (3×3×3 = CUBE)
 *
 * BOUNDS VERIFIED:
 *   - PHASE: 0..2, OFFSET: 0..26
 * @{ */

CPISI_STATIC_ASSERT(CPISI_PHASE_COUNT == 3,
                    "PHASE_COUNT must be 3 — DAR phases");
CPISI_STATIC_ASSERT(CPISI_SEVERITY_ROW_COUNT == 3,
                    "SEVERITY_ROW_COUNT must be 3 — severity rows");
CPISI_STATIC_ASSERT(CPISI_ASPECT_COUNT == 3,
                    "ASPECT_COUNT must be 3 — aspects");
CPISI_STATIC_ASSERT(CPISI_OFFSET_COUNT == 27,
                    "OFFSET_COUNT must be 27 — total offsets");
CPISI_STATIC_ASSERT(CPISI_PHASE_MIN == 0,
                    "PHASE_MIN must be 0 — first phase");
CPISI_STATIC_ASSERT(CPISI_PHASE_MAX == 2,
                    "PHASE_MAX must be 2 — last phase");
CPISI_STATIC_ASSERT(CPISI_OFFSET_MIN == 0,
                    "OFFSET_MIN must be 0 — first offset");
CPISI_STATIC_ASSERT(CPISI_OFFSET_MAX == 26,
                    "OFFSET_MAX must be 26 — last offset");


/** @} */ /* end S.2c.7a VALUE */

/**
 * @defgroup S2c_7b_RELATIONSHIP S.2c.7b RELATIONSHIP TRIPWIRES — Verify derivations
 * @ingroup S2c_7_PHASE_DOMAIN
 * @brief Verify phase values derive from primitives correctly.
 *
 * DERIVATIONS:
 *   - PHASE_COUNT = TERNARY (3)
 *   - SEVERITY_ROW_COUNT = TERNARY (3)
 *   - ASPECT_COUNT = TERNARY (3)
 *   - OFFSET_COUNT = CUBE (3³ = 27)
 *   - PHASE_MIN = VOID, PHASE_MAX = COUNT - 1
 *   - OFFSET_MIN = VOID, OFFSET_MAX = CUBE - 1
 * @{ */

CPISI_STATIC_ASSERT(CPISI_PHASE_COUNT == CPISI_TERNARY,
                    "PHASE_COUNT must equal TERNARY — 3 phases");
CPISI_STATIC_ASSERT(CPISI_SEVERITY_ROW_COUNT == CPISI_TERNARY,
                    "SEVERITY_ROW_COUNT must equal TERNARY — 3 rows");
CPISI_STATIC_ASSERT(CPISI_ASPECT_COUNT == CPISI_TERNARY,
                    "ASPECT_COUNT must equal TERNARY — 3 aspects");
CPISI_STATIC_ASSERT(CPISI_OFFSET_COUNT == CPISI_CUBE,
                    "OFFSET_COUNT must equal CUBE — 27 offsets");
CPISI_STATIC_ASSERT(CPISI_PHASE_MIN == CPISI_VOID,
                    "PHASE_MIN must equal VOID — 0");
CPISI_STATIC_ASSERT(CPISI_PHASE_MAX == CPISI_DIFF(CPISI_PHASE_COUNT, CPISI_UNITY),
                    "PHASE_MAX must equal COUNT - 1 — 2");
CPISI_STATIC_ASSERT(CPISI_OFFSET_MIN == CPISI_VOID,
                    "OFFSET_MIN must equal VOID — 0");
CPISI_STATIC_ASSERT(CPISI_OFFSET_MAX == CPISI_DIFF(CPISI_CUBE, CPISI_UNITY),
                    "OFFSET_MAX must equal CUBE - 1 — 26");


/** @} */ /* end S.2c.7b RELATIONSHIP */

/**
 * @defgroup S2c_7c_CENTERS S.2c.7c CENTER TRIPWIRES — Verify center calculations
 * @ingroup S2c_7_PHASE_DOMAIN
 * @brief Verify center derivations via HALF().
 *
 * CENTERS VERIFIED:
 *   - PHASE_CENTER = 1 (ASSESS — middle phase)
 *   - OFFSET_CENTER = 13 (A:MED:PROC — cube center)
 *
 * DERIVATIONS:
 *   - PHASE_CENTER = HALF(3) = (3-1)/2 = 1
 *   - OFFSET_CENTER = HALF(27) = (27-1)/2 = 13
 *
 * NOTE: OFFSET_CENTER (13) equals BRIDGE (13) — both represent "center".
 * @{ */

CPISI_STATIC_ASSERT(CPISI_PHASE_CENTER == 1,
                    "PHASE_CENTER must be 1 — ASSESS");
CPISI_STATIC_ASSERT(CPISI_OFFSET_CENTER == 13,
                    "OFFSET_CENTER must be 13 — A:MED:PROC");
CPISI_STATIC_ASSERT(CPISI_PHASE_CENTER == CPISI_HALF(CPISI_PHASE_COUNT),
                    "PHASE_CENTER must equal HALF(COUNT) — 1");
CPISI_STATIC_ASSERT(CPISI_OFFSET_CENTER == CPISI_HALF(CPISI_CUBE),
                    "OFFSET_CENTER must equal HALF(CUBE) — 13");


/** @} */ /* end S.2c.7c CENTERS */

/**
 * @defgroup S2c_7d_BUILD S.2c.7d BUILD TRIPWIRES — Verify offset formula
 * @ingroup S2c_7_PHASE_DOMAIN
 * @brief Verify offset build formula produces correct entry/exit/center codes.
 *
 * ENTRY POINTS (phase × FACE):
 *   - D_MILD_ENTRY = 0 (DETECT start)
 *   - A_MILD_ENTRY = 9 (ASSESS start = FACE)
 *   - R_MILD_ENTRY = 18 (RESTORE start = 2×FACE)
 *
 * PHASE CENTERS (MED:PROC within each phase):
 *   - DETECT_CENTER = 4, ASSESS_CENTER = 13, RESTORE_CENTER = 22
 * @{ */

CPISI_STATIC_ASSERT(CPISI_OFF_D_MILD_ENTRY == 0,
                    "OFF_D_MILD_ENTRY must be 0 — DETECT first");
CPISI_STATIC_ASSERT(CPISI_OFF_A_MILD_ENTRY == 9,
                    "OFF_A_MILD_ENTRY must be 9 — ASSESS first");
CPISI_STATIC_ASSERT(CPISI_OFF_R_MILD_ENTRY == 18,
                    "OFF_R_MILD_ENTRY must be 18 — RESTORE first");
CPISI_STATIC_ASSERT(CPISI_OFF_R_CRIT_EXIT == 26,
                    "OFF_R_CRIT_EXIT must be 26 — last offset");
CPISI_STATIC_ASSERT(CPISI_OFF_D_MILD_ENTRY == CPISI_VOID,
                    "OFF_D_MILD_ENTRY must equal VOID — 0");
CPISI_STATIC_ASSERT(CPISI_OFF_A_MILD_ENTRY == CPISI_FACE,
                    "OFF_A_MILD_ENTRY must equal FACE — 9");
CPISI_STATIC_ASSERT(CPISI_OFF_R_MILD_ENTRY == CPISI_PRODUCT(CPISI_BINARY, CPISI_FACE),
                    "OFF_R_MILD_ENTRY must equal 2×FACE — 18");
CPISI_STATIC_ASSERT(CPISI_OFF_R_CRIT_EXIT == CPISI_OFFSET_MAX,
                    "OFF_R_CRIT_EXIT must equal OFFSET_MAX — 26");
CPISI_STATIC_ASSERT(CPISI_OFF_DETECT_CENTER == 4,
                    "OFF_DETECT_CENTER must be 4 — D:MED:PROC");
CPISI_STATIC_ASSERT(CPISI_OFF_ASSESS_CENTER == 13,
                    "OFF_ASSESS_CENTER must be 13 — A:MED:PROC = OFFSET_CENTER");
CPISI_STATIC_ASSERT(CPISI_OFF_RESTORE_CENTER == 22,
                    "OFF_RESTORE_CENTER must be 22 — R:MED:PROC");
CPISI_STATIC_ASSERT(CPISI_OFF_DETECT_CENTER == CPISI_OFF_D_MED_PROC,
                    "OFF_DETECT_CENTER must equal D_MED_PROC — 4");
CPISI_STATIC_ASSERT(CPISI_OFF_ASSESS_CENTER == CPISI_OFF_A_MED_PROC,
                    "OFF_ASSESS_CENTER must equal A_MED_PROC — 13");
CPISI_STATIC_ASSERT(CPISI_OFF_RESTORE_CENTER == CPISI_OFF_R_MED_PROC,
                    "OFF_RESTORE_CENTER must equal R_MED_PROC — 22");


/** @} */ /* end S.2c.7d BUILD */
/** @} */ /* end S.2c.7 PHASE DOMAIN */

/** @} */ /* end S.2c PHASE VERIFY */

// ─── SED HERE: S.2c.8 METADATA_DOMAIN ─── see types-metadata.h ────────────────

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d LOOKUP — Runtime lookup tables [LOOKUP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_phase_lookup S.2d LOOKUP — Runtime lookup tables
 * @ingroup cpisi_phase_domain
 * @brief Pre-computed tables for O(1) phase/offset operations.
 *
 * PURPOSE: Container for phase domain lookup tables.
 *
 * STRUCTURE (mirrors types.h S.2d pattern: Domain → a/b/c):
 *   S.2d.8 PHASE — Phase domain tables
 *     S.2d.8a TABLES     — 15 arrays (offset, phase, category, severity, aspect)
 *     S.2d.8b ACCESSORS  — 16 macros (dimension access for DAR workflow)
 *     S.2d.8c CONVERSION — [Reserved: phases use direct lookup]
 *
 * Builds FROM: S.2c PHASE VERIFY (structure verified)
 * Builds TO:   S.2e PHASE TYPES (enum definitions)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup cpisi_phase_tables S.2d.8 PHASE — Phase domain lookup
 * @ingroup cpisi_phase_lookup
 * @brief Runtime lookup tables for phase/offset system (DAR workflow).
 *
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  PHASE DOMAIN TABLES — DAR workflow (Detect-Assess-Restore)               ║
 * ║  ─────────────────────────────────────────────────────────────────────────║
 * ║  27 offsets = 3×3×3 (phase × severity × aspect)                           ║
 * ║  CENTER: ASSESS:MEDIUM:PROCESS = offset 13 = BRIDGE                       ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 *
 * OFFSET STRUCTURE (3×3×3 = 27 = CUBE):
 *   - 3 phases: DETECT(D), ASSESS(A), RESTORE(R)
 *   - 3 severities: MILD, MEDIUM, CRITICAL
 *   - 3 aspects: ENTRY, PROCESS, EXIT
 *   - CENTER: ASSESS:MEDIUM:PROCESS = offset 13
 *
 * STRUCTURE (template pattern: a=TABLES, b=ACCESSORS, c=CONVERSION):
 *   S.2d.8a TABLES     — 15 arrays organized in 5 groups:
 *                        • OFFSET (1×27): formatted names
 *                        • PHASE (4×3): name, abbrev, version, balanced
 *                        • CATEGORY (2×3): name, abbrev
 *                        • SEVERITY (4×3): name, abbrev, version, balanced
 *                        • ASPECT (4×3): name, abbrev, version, balanced
 *   S.2d.8b ACCESSORS  — 16 macros for dimension access
 *   S.2d.8c CONVERSION — [Reserved: phases use direct lookup]
 *
 * @ingroup S2d_LOOKUP
 * @{ */

/** @name S.2d.8a TABLES — Phase lookup arrays
 *  @brief 15 arrays organized in 5 groups for DAR workflow navigation.
 *
 *  OFFSET ARRAYS (1 × 27 entries):
 *    g_cpisi_offset_name[27] — Formatted "PHASE:SEV:ASP" names
 *
 *  PHASE ARRAYS (4 × 3 entries):
 *    g_cpisi_phase_name/abbrev/version/balanced[3]
 *
 *  CATEGORY ARRAYS (2 × 3 entries):
 *    g_cpisi_error_cat_name/abbrev[3]
 *
 *  SEVERITY ARRAYS (4 × 3 entries):
 *    g_cpisi_severity_name/abbrev/version/balanced[3]
 *
 *  ASPECT ARRAYS (4 × 3 entries):
 *    g_cpisi_aspect_name/abbrev/version/balanced[3]
 *
 *  @{ */

/* ═══════════════════════════════════════════════════════════════════════════
 * OFFSET TABLES (1 × 27 array)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Formatted offset names "P:SEV:ASP" for the 27-position DAR cube.
 */

/**
 * @brief Offset name lookup — maps offset index (0-26) to formatted name.
 *
 * BOUNDS USED:
 *   - CPISI_OFFSET_COUNT = 27 = CPISI_CUBE (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: 3D coordinate → string "PHASE:SEVERITY:ASPECT"
 *   offset = phase×9 + severity×3 + aspect
 *   [0-8] DETECT phase (D:*)
 *   [9-17] ASSESS phase (A:*) — CENTER phase
 *   [18-26] RESTORE phase (R:*)
 *   [13] = A:MED:PROC = CENTER of CENTER = BRIDGE offset
 *
 * FORMAT: "P:SEV:ASP" where P={D,A,R}, SEV={MILD,MED,CRIT}, ASP={ENTRY,PROC,EXIT}
 *
 * USE CASE: Logging, debugging, UI display of current DAR state.
 */
static const char* const g_cpisi_offset_name[CPISI_OFFSET_COUNT] = {
    "D:MILD:ENTRY",  /*  [0] DETECT mild entry    */
    "D:MILD:PROC",   /*  [1] DETECT mild process  */
    "D:MILD:EXIT",   /*  [2] DETECT mild exit     */
    "D:MED:ENTRY",   /*  [3] DETECT medium entry  */
    "D:MED:PROC",    /*  [4] DETECT_CENTER        */
    "D:MED:EXIT",    /*  [5] DETECT medium exit   */
    "D:CRIT:ENTRY",  /*  [6] DETECT critical entry*/
    "D:CRIT:PROC",   /*  [7] DETECT critical proc */
    "D:CRIT:EXIT",   /*  [8] DETECT critical exit */
    "A:MILD:ENTRY",  /*  [9] ASSESS mild entry    */
    "A:MILD:PROC",   /* [10] ASSESS mild process  */
    "A:MILD:EXIT",   /* [11] ASSESS mild exit     */
    "A:MED:ENTRY",   /* [12] ASSESS medium entry  */
    "A:MED:PROC",    /* [13] OFFSET_CENTER=BRIDGE */
    "A:MED:EXIT",    /* [14] ASSESS medium exit   */
    "A:CRIT:ENTRY",  /* [15] ASSESS critical entry*/
    "A:CRIT:PROC",   /* [16] ASSESS critical proc */
    "A:CRIT:EXIT",   /* [17] ASSESS critical exit */
    "R:MILD:ENTRY",  /* [18] RESTORE mild entry   */
    "R:MILD:PROC",   /* [19] RESTORE mild process */
    "R:MILD:EXIT",   /* [20] RESTORE mild exit    */
    "R:MED:ENTRY",   /* [21] RESTORE medium entry */
    "R:MED:PROC",    /* [22] RESTORE_CENTER       */
    "R:MED:EXIT",    /* [23] RESTORE medium exit  */
    "R:CRIT:ENTRY",  /* [24] RESTORE critical entry*/
    "R:CRIT:PROC",   /* [25] RESTORE critical proc*/
    "R:CRIT:EXIT"    /* [26] RESTORE critical exit*/
};

/* ═══════════════════════════════════════════════════════════════════════════
 * PHASE TABLES (4 × 3 arrays)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief DAR workflow phase tables (Detect-Assess-Restore).
 *
 * 4 arrays × 3 entries each: name, abbrev, version, balanced.
 */

/**
 * @brief Phase name lookup — maps phase index (0-2) to full name.
 *
 * BOUNDS USED:
 *   - CPISI_PHASE_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: DAR workflow phases (biblical Detect-Assess-Restore pattern)
 *   [0] DETECT = witness/recognize issue (k=-1, break down)
 *   [1] ASSESS = evaluate/understand (k=0, center/anchor)
 *   [2] RESTORE = recover/heal (k=+1, build up)
 *
 * USE CASE: Logging, state display, workflow navigation.
 */
static const char* const g_cpisi_phase_name[CPISI_PHASE_COUNT] = {
    "DETECT",   /* [0] witness/recognize  */
    "ASSESS",   /* [1] evaluate/anchor    */
    "RESTORE"   /* [2] recover/complete   */
};

/**
 * @brief Phase abbreviation lookup — maps phase index (0-2) to single char.
 *
 * BOUNDS USED:
 *   - CPISI_PHASE_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: First letter of each phase name.
 *
 * USE CASE: Compact logging, status indicators, offset name formatting.
 */
static const char g_cpisi_phase_abbrev[CPISI_PHASE_COUNT] = {
    'D',  /* [0] DETECT  */
    'A',  /* [1] ASSESS  */
    'R'   /* [2] RESTORE */
};

/**
 * @brief Phase version lookup — maps phase index (0-2) to living version char.
 *
 * BOUNDS USED:
 *   - CPISI_PHASE_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Living versioning alignment (a=alpha, b=beta, c=stable)
 *   DETECT aligns with alpha (early detection)
 *   ASSESS aligns with beta (evaluation)
 *   RESTORE aligns with stable (completion)
 *
 * USE CASE: Version string generation, maturity indication.
 */
static const char g_cpisi_phase_version[CPISI_PHASE_COUNT] = {
    'a',  /* [0] DETECT=alpha   */
    'b',  /* [1] ASSESS=beta    */
    'c'   /* [2] RESTORE=stable */
};

/**
 * @brief Phase balanced lookup — maps phase index (0-2) to ternary value.
 *
 * BOUNDS USED:
 *   - CPISI_PHASE_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Ternary assignment following building block method
 *   DETECT = -1 (break down, toward foundation)
 *   ASSESS = 0 (anchor, center)
 *   RESTORE = +1 (build up, toward completion)
 *
 * USE CASE: Ternary arithmetic, direction calculations.
 */
static const int8_t g_cpisi_phase_balanced[CPISI_PHASE_COUNT] = {
    -1,  /* [0] DETECT  = foundation  */
     0,  /* [1] ASSESS  = anchor      */
    +1   /* [2] RESTORE = completion  */
};

/* ═══════════════════════════════════════════════════════════════════════════
 * CATEGORY TABLES (2 × 3 arrays)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Error category identification (CORE/EXTENDED/SYSTEM).
 *
 * 2 arrays × 3 entries each: name, abbrev.
 */

/**
 * @brief Category name lookup — maps category index (0-2) to full name.
 *
 * BOUNDS USED:
 *   - CPISI_CAT_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Error category classification
 *   [0] CORE = essential/fundamental errors (must handle)
 *   [1] EXTENDED = optional/enhancement errors (may handle)
 *   [2] SYSTEM = internal/infrastructure errors (system-level)
 *
 * USE CASE: Error categorization, logging, filtering.
 */
static const char* const g_cpisi_category_name[CPISI_CAT_COUNT] = {
    "CORE",      /* [0] essential     */
    "EXTENDED",  /* [1] optional      */
    "SYSTEM"     /* [2] internal      */
};

/**
 * @brief Category abbreviation lookup — maps category index (0-2) to single char.
 *
 * BOUNDS USED:
 *   - CPISI_CAT_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: First letter of each category name.
 *
 * USE CASE: Compact error codes, status indicators.
 */
static const char g_cpisi_category_abbrev[CPISI_CAT_COUNT] = {
    'C',  /* [0] CORE     */
    'E',  /* [1] EXTENDED */
    'S'   /* [2] SYSTEM   */
};

/* ═══════════════════════════════════════════════════════════════════════════
 * SEVERITY TABLES (4 × 3 arrays)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Severity level tables (MILD/MEDIUM/CRITICAL) within DAR offset cube.
 *
 * 4 arrays × 3 entries each: name, abbrev, version, balanced.
 */

/**
 * @brief Severity name lookup — maps severity index (0-2) to full name.
 *
 * BOUNDS USED:
 *   - CPISI_SEVERITY_ROW_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Escalating severity levels within each phase
 *   [0] MILD = warning, recoverable (row 0 in each phase)
 *   [1] MEDIUM = degraded, needs attention (row 1 in each phase)
 *   [2] CRITICAL = fatal, immediate action (row 2 in each phase)
 *
 * USE CASE: Error severity display, escalation decisions.
 */
static const char* const g_cpisi_severity_row_name[CPISI_SEVERITY_ROW_COUNT] = {
    "MILD",      /* [0] warning    */
    "MEDIUM",    /* [1] degraded   */
    "CRITICAL"   /* [2] fatal      */
};

/**
 * @brief Severity abbreviation lookup — maps severity index (0-2) to single char.
 *
 * BOUNDS USED:
 *   - CPISI_SEVERITY_ROW_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: MEC abbreviations (M=Mild, E=mEdium, C=Critical)
 *
 * USE CASE: Compact logging, offset name formatting.
 */
static const char g_cpisi_severity_row_abbrev[CPISI_SEVERITY_ROW_COUNT] = {
    'M',  /* [0] MILD     */
    'E',  /* [1] mEdium   */
    'C'   /* [2] CRITICAL */
};

/**
 * @brief Severity version lookup — maps severity index (0-2) to version char.
 *
 * BOUNDS USED:
 *   - CPISI_SEVERITY_ROW_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Living versioning alignment
 *   MILD → alpha (early/testing)
 *   MEDIUM → beta (maturing)
 *   CRITICAL → stable (production-ready handling)
 *
 * USE CASE: Version string generation.
 */
static const char g_cpisi_severity_row_version[CPISI_SEVERITY_ROW_COUNT] = {
    'a',  /* [0] MILD=alpha    */
    'b',  /* [1] MEDIUM=beta   */
    'c'   /* [2] CRITICAL=stable */
};

/**
 * @brief Severity balanced lookup — maps severity index (0-2) to ternary value.
 *
 * BOUNDS USED:
 *   - CPISI_SEVERITY_ROW_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Ternary escalation
 *   MILD = -1 (low urgency)
 *   MEDIUM = 0 (moderate, center)
 *   CRITICAL = +1 (high urgency)
 *
 * USE CASE: Priority weighting, urgency calculations.
 */
static const int8_t g_cpisi_severity_row_balanced[CPISI_SEVERITY_ROW_COUNT] = {
    -1,  /* [0] MILD     = low      */
     0,  /* [1] MEDIUM   = moderate */
    +1   /* [2] CRITICAL = high     */
};

/* ═══════════════════════════════════════════════════════════════════════════
 * ASPECT TABLES (4 × 3 arrays)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Execution aspect tables within each severity×phase cell.
 *
 * 4 arrays × 3 entries each: name, abbrev, version, balanced.
 */

/**
 * @brief Aspect name lookup — maps aspect index (0-2) to full name.
 *
 * BOUNDS USED:
 *   - CPISI_ASPECT_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Execution lifecycle stages
 *   [0] ENTRY = initialization, setup (before handling)
 *   [1] PROCESS = core handling, main work (during handling)
 *   [2] EXIT = cleanup, completion (after handling)
 *
 * USE CASE: Workflow stage tracking, logging context.
 */
static const char* const g_cpisi_aspect_name[CPISI_ASPECT_COUNT] = {
    "ENTRY",    /* [0] setup/init     */
    "PROCESS",  /* [1] main handling  */
    "EXIT"      /* [2] cleanup/done   */
};

/**
 * @brief Aspect abbreviation lookup — maps aspect index (0-2) to single char.
 *
 * BOUNDS USED:
 *   - CPISI_ASPECT_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: NPX abbreviations (N=eNtry, P=Process, X=eXit)
 *
 * USE CASE: Compact logging, offset name formatting.
 */
static const char g_cpisi_aspect_abbrev[CPISI_ASPECT_COUNT] = {
    'N',  /* [0] eNtry   */
    'P',  /* [1] Process */
    'X'   /* [2] eXit    */
};

/**
 * @brief Aspect version lookup — maps aspect index (0-2) to version char.
 *
 * BOUNDS USED:
 *   - CPISI_ASPECT_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Living versioning alignment
 *   ENTRY → alpha (beginning)
 *   PROCESS → beta (middle)
 *   EXIT → stable (completion)
 *
 * USE CASE: Version string generation.
 */
static const char g_cpisi_aspect_version[CPISI_ASPECT_COUNT] = {
    'a',  /* [0] ENTRY=alpha   */
    'b',  /* [1] PROCESS=beta  */
    'c'   /* [2] EXIT=stable   */
};

/**
 * @brief Aspect balanced lookup — maps aspect index (0-2) to ternary value.
 *
 * BOUNDS USED:
 *   - CPISI_ASPECT_COUNT = 3 (from S.2a.2 TIER 2) — array size
 *
 * DERIVATION: Ternary lifecycle
 *   ENTRY = -1 (beginning, foundation)
 *   PROCESS = 0 (middle, anchor)
 *   EXIT = +1 (end, completion)
 *
 * USE CASE: Lifecycle position calculations.
 */
static const int8_t g_cpisi_aspect_balanced[CPISI_ASPECT_COUNT] = {
    -1,  /* [0] ENTRY   = beginning   */
     0,  /* [1] PROCESS = middle      */
    +1   /* [2] EXIT    = completion  */
};

/** @} */ /* end S.2d.8a TABLES */

/** @name S.2d.8b ACCESSORS — Phase table access macros
 *  @brief Type-safe access macros for phase domain lookup tables (16 macros).
 *
 *  ACCESSOR GROUPS (16 total):
 *    OFFSET (1):   cpisi_offset_name()
 *    PHASE (4):    cpisi_phase_name/abbrev/version/balanced()
 *    CATEGORY (2): cpisi_category_name/abbrev()
 *    SEVERITY (4): cpisi_severity_name/abbrev/version/balanced()
 *    ASPECT (4):   cpisi_aspect_name/abbrev/version/balanced()
 *
 *  @{ */

/* ═══════════════════════════════════════════════════════════════════════════
 * OFFSET ACCESSOR (1 macro)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Get offset name — INDEX (0-26) → formatted "P:SEV:ASP" string.
 *
 * BOUNDS:
 *   - INPUT:  offset ∈ [0, CPISI_OFFSET_COUNT-1] = [0, 26]
 *   - OUTPUT: const char* (null-terminated)
 *
 * FORMULA: Direct table lookup — g_cpisi_offset_name[offset]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ offset ≤ 26.
 *
 * EXAMPLE:
 *   cpisi_offset_name(0)  → "D:MILD:ENTRY"
 *   cpisi_offset_name(13) → "A:MED:PROC" (CENTER)
 *   cpisi_offset_name(26) → "R:CRIT:EXIT"
 *
 * @param offset Offset index 0-26 (unchecked)
 * @return Formatted offset name string
 */
#define cpisi_offset_name(offset)       (g_cpisi_offset_name[(offset)])

/* ═══════════════════════════════════════════════════════════════════════════
 * PHASE ACCESSORS (4 macros)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Get phase name — INDEX (0-2) → full phase name string.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_PHASE_COUNT-1] = [0, 2]
 *   - OUTPUT: const char* ∈ {"DETECT", "ASSESS", "RESTORE"}
 *
 * FORMULA: Direct table lookup — g_cpisi_phase_name[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_phase_name(0) → "DETECT"
 *   cpisi_phase_name(1) → "ASSESS"
 *   cpisi_phase_name(2) → "RESTORE"
 *
 * @param idx Phase index 0-2 (unchecked)
 * @return Phase name string
 */
#define cpisi_phase_name(idx)           (g_cpisi_phase_name[(idx)])

/**
 * @brief Get phase abbreviation — INDEX (0-2) → single char.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_PHASE_COUNT-1] = [0, 2]
 *   - OUTPUT: char ∈ {'D', 'A', 'R'}
 *
 * FORMULA: Direct table lookup — g_cpisi_phase_abbrev[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_phase_abbrev(0) → 'D'
 *   cpisi_phase_abbrev(1) → 'A'
 *   cpisi_phase_abbrev(2) → 'R'
 *
 * @param idx Phase index 0-2 (unchecked)
 * @return Phase abbreviation char
 */
#define cpisi_phase_abbrev(idx)         (g_cpisi_phase_abbrev[(idx)])

/**
 * @brief Get phase version char — INDEX (0-2) → living version char.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_PHASE_COUNT-1] = [0, 2]
 *   - OUTPUT: char ∈ {'a', 'b', 'c'}
 *
 * FORMULA: Direct table lookup — g_cpisi_phase_version[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_phase_version(0) → 'a' (alpha)
 *   cpisi_phase_version(1) → 'b' (beta)
 *   cpisi_phase_version(2) → 'c' (stable)
 *
 * @param idx Phase index 0-2 (unchecked)
 * @return Version char
 */
#define cpisi_phase_version(idx)        (g_cpisi_phase_version[(idx)])

/**
 * @brief Get phase balanced value — INDEX (0-2) → ternary value.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_PHASE_COUNT-1] = [0, 2]
 *   - OUTPUT: int8_t ∈ {-1, 0, +1}
 *
 * FORMULA: Direct table lookup — g_cpisi_phase_balanced[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_phase_balanced(0) → -1 (DETECT)
 *   cpisi_phase_balanced(1) →  0 (ASSESS)
 *   cpisi_phase_balanced(2) → +1 (RESTORE)
 *
 * @param idx Phase index 0-2 (unchecked)
 * @return Balanced ternary value
 */
#define cpisi_phase_balanced(idx)       (g_cpisi_phase_balanced[(idx)])

/* ═══════════════════════════════════════════════════════════════════════════
 * CATEGORY ACCESSORS (2 macros)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Get category name — INDEX (0-2) → full category name string.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_CAT_COUNT-1] = [0, 2]
 *   - OUTPUT: const char* ∈ {"CORE", "EXTENDED", "SYSTEM"}
 *
 * FORMULA: Direct table lookup — g_cpisi_category_name[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_category_name(0) → "CORE"
 *   cpisi_category_name(1) → "EXTENDED"
 *   cpisi_category_name(2) → "SYSTEM"
 *
 * @param idx Category index 0-2 (unchecked)
 * @return Category name string
 */
#define cpisi_category_name(idx)        (g_cpisi_category_name[(idx)])

/**
 * @brief Get category abbreviation — INDEX (0-2) → single char.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_CAT_COUNT-1] = [0, 2]
 *   - OUTPUT: char ∈ {'C', 'E', 'S'}
 *
 * FORMULA: Direct table lookup — g_cpisi_category_abbrev[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_category_abbrev(0) → 'C'
 *   cpisi_category_abbrev(1) → 'E'
 *   cpisi_category_abbrev(2) → 'S'
 *
 * @param idx Category index 0-2 (unchecked)
 * @return Category abbreviation char
 */
#define cpisi_category_abbrev(idx)      (g_cpisi_category_abbrev[(idx)])

/* ═══════════════════════════════════════════════════════════════════════════
 * SEVERITY ACCESSORS (4 macros)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Get severity name — INDEX (0-2) → full severity name string.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_SEVERITY_ROW_COUNT-1] = [0, 2]
 *   - OUTPUT: const char* ∈ {"MILD", "MEDIUM", "CRITICAL"}
 *
 * FORMULA: Direct table lookup — g_cpisi_severity_row_name[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_severity_name(0) → "MILD"
 *   cpisi_severity_name(1) → "MEDIUM"
 *   cpisi_severity_name(2) → "CRITICAL"
 *
 * @param idx Severity index 0-2 (unchecked)
 * @return Severity name string
 */
#define cpisi_severity_name(idx)        (g_cpisi_severity_row_name[(idx)])

/**
 * @brief Get severity abbreviation — INDEX (0-2) → single char.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_SEVERITY_ROW_COUNT-1] = [0, 2]
 *   - OUTPUT: char ∈ {'M', 'E', 'C'}
 *
 * FORMULA: Direct table lookup — g_cpisi_severity_row_abbrev[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_severity_abbrev(0) → 'M'
 *   cpisi_severity_abbrev(1) → 'E'
 *   cpisi_severity_abbrev(2) → 'C'
 *
 * @param idx Severity index 0-2 (unchecked)
 * @return Severity abbreviation char
 */
#define cpisi_severity_abbrev(idx)      (g_cpisi_severity_row_abbrev[(idx)])

/**
 * @brief Get severity version char — INDEX (0-2) → living version char.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_SEVERITY_ROW_COUNT-1] = [0, 2]
 *   - OUTPUT: char ∈ {'a', 'b', 'c'}
 *
 * FORMULA: Direct table lookup — g_cpisi_severity_row_version[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_severity_version(0) → 'a' (alpha)
 *   cpisi_severity_version(1) → 'b' (beta)
 *   cpisi_severity_version(2) → 'c' (stable)
 *
 * @param idx Severity index 0-2 (unchecked)
 * @return Version char
 */
#define cpisi_severity_version(idx)     (g_cpisi_severity_row_version[(idx)])

/**
 * @brief Get severity balanced value — INDEX (0-2) → ternary value.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_SEVERITY_ROW_COUNT-1] = [0, 2]
 *   - OUTPUT: int8_t ∈ {-1, 0, +1}
 *
 * FORMULA: Direct table lookup — g_cpisi_severity_row_balanced[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_severity_balanced(0) → -1 (MILD)
 *   cpisi_severity_balanced(1) →  0 (MEDIUM)
 *   cpisi_severity_balanced(2) → +1 (CRITICAL)
 *
 * @param idx Severity index 0-2 (unchecked)
 * @return Balanced ternary value
 */
#define cpisi_severity_balanced(idx)    (g_cpisi_severity_row_balanced[(idx)])

/* ═══════════════════════════════════════════════════════════════════════════
 * ASPECT ACCESSORS (4 macros)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Get aspect name — INDEX (0-2) → full aspect name string.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_ASPECT_COUNT-1] = [0, 2]
 *   - OUTPUT: const char* ∈ {"ENTRY", "PROCESS", "EXIT"}
 *
 * FORMULA: Direct table lookup — g_cpisi_aspect_name[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_aspect_name(0) → "ENTRY"
 *   cpisi_aspect_name(1) → "PROCESS"
 *   cpisi_aspect_name(2) → "EXIT"
 *
 * @param idx Aspect index 0-2 (unchecked)
 * @return Aspect name string
 */
#define cpisi_aspect_name(idx)          (g_cpisi_aspect_name[(idx)])

/**
 * @brief Get aspect abbreviation — INDEX (0-2) → single char.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_ASPECT_COUNT-1] = [0, 2]
 *   - OUTPUT: char ∈ {'N', 'P', 'X'}
 *
 * FORMULA: Direct table lookup — g_cpisi_aspect_abbrev[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_aspect_abbrev(0) → 'N' (eNtry)
 *   cpisi_aspect_abbrev(1) → 'P' (Process)
 *   cpisi_aspect_abbrev(2) → 'X' (eXit)
 *
 * @param idx Aspect index 0-2 (unchecked)
 * @return Aspect abbreviation char
 */
#define cpisi_aspect_abbrev(idx)        (g_cpisi_aspect_abbrev[(idx)])

/**
 * @brief Get aspect version char — INDEX (0-2) → living version char.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_ASPECT_COUNT-1] = [0, 2]
 *   - OUTPUT: char ∈ {'a', 'b', 'c'}
 *
 * FORMULA: Direct table lookup — g_cpisi_aspect_version[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_aspect_version(0) → 'a' (alpha)
 *   cpisi_aspect_version(1) → 'b' (beta)
 *   cpisi_aspect_version(2) → 'c' (stable)
 *
 * @param idx Aspect index 0-2 (unchecked)
 * @return Version char
 */
#define cpisi_aspect_version(idx)       (g_cpisi_aspect_version[(idx)])

/**
 * @brief Get aspect balanced value — INDEX (0-2) → ternary value.
 *
 * BOUNDS:
 *   - INPUT:  idx ∈ [0, CPISI_ASPECT_COUNT-1] = [0, 2]
 *   - OUTPUT: int8_t ∈ {-1, 0, +1}
 *
 * FORMULA: Direct table lookup — g_cpisi_aspect_balanced[idx]
 *
 * NO BOUNDS CHECK: Caller must ensure 0 ≤ idx ≤ 2.
 *
 * EXAMPLE:
 *   cpisi_aspect_balanced(0) → -1 (ENTRY)
 *   cpisi_aspect_balanced(1) →  0 (PROCESS)
 *   cpisi_aspect_balanced(2) → +1 (EXIT)
 *
 * @param idx Aspect index 0-2 (unchecked)
 * @return Balanced ternary value
 */
#define cpisi_aspect_balanced(idx)      (g_cpisi_aspect_balanced[(idx)])

/** @} */ /* end S.2d.8b ACCESSORS */

/** @name S.2d.8c CONVERSION — Phase conversion utilities
 *  @brief [Reserved: Phases use direct lookup, not format conversion]
 *
 *  Phase dimensions use direct table lookup (S.2d.8a) via accessor macros (S.2d.8b).
 *  No conversion utilities needed — dimensions map directly to indices.
 *
 *  FUTURE: If string→phase parsing needed, add here.
 *  @{ */

/* No conversion utilities — phases use direct lookup */

/** @} */ /* end S.2d.8c CONVERSION */

/** @} */ /* end S.2d.8 PHASE */

/** @} */ /* end S.2d LOOKUP */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e TYPES — Phase domain type definitions [TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_phase_types S.2e PHASE TYPES — Type Definitions
 * @ingroup cpisi_phase_domain
 * @brief Type-safe enums for phase/offset system (DOMAIN-SPECIFIC).
 *
 * PURPOSE: Type definitions for phase domain.
 *
 * CONTAINS:
 *   S.2e.6a PHASE        — CpisiPhase (3 values: Detect, Assess, Restore)
 *   S.2e.6b SEVERITY_ROW — CpisiSeverityRow (3 values: Mild, Medium, Crit)
 *   S.2e.6c ASPECT       — CpisiAspect (3 values: Entry, Process, Exit)
 *
 * Builds FROM: S.2d LOOKUP (runtime tables)
 * Builds TO:   S.3 CLOSING (file end)
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  MATHEMATICAL FOUNDATION — 3×3×3 = 27 Offset Space:                        │
 * │                                                                            │
 * │    offset = (phase × 9) + (severity × 3) + aspect                          │
 * │                                                                            │
 * │    Each dimension is balanced ternary:                                     │
 * │      Phase:    DETECT(-1), ASSESS(0), RESTORE(+1)                          │
 * │      Severity: MILD(-1), MEDIUM(0), CRITICAL(+1)                           │
 * │      Aspect:   ENTRY(-1), PROCESS(0), EXIT(+1)                             │
 * │                                                                            │
 * │    CENTER = offset 13 = ASSESS × MEDIUM × PROCESS                          │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │  RELATIONSHIP TO LOOKUP (S.2d.8):                                          │
 * │    - g_cpisi_phase_*[3] tables (name, abbrev, version, balanced)           │
 * │    - g_cpisi_severity_*[3] tables                                          │
 * │    - g_cpisi_aspect_*[3] tables                                            │
 * │    - g_cpisi_offset_name[27] — composite offset names                      │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @defgroup S2e_6a_PHASE S.2e.6a CpisiPhase — DAR phase enum
 * @brief Type-safe DAR phase — DETECT(0), ASSESS(1), RESTORE(2).
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — Detect-Assess-Restore Cycle:                        │
 * │                                                                            │
 * │    DETECT  (0) — witness phase, observe the problem (bal=-1)               │
 * │    ASSESS  (1) — evaluate phase, analyze severity (bal=0) ← CENTER         │
 * │    RESTORE (2) — recover phase, implement solution (bal=+1)                │
 * │                                                                            │
 * │  The DAR cycle forms the X-axis of the 27-offset space.                    │
 * │  offset contribution: phase × 9                                            │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup cpisi_phase_types
 * @{ */

/**
 * @brief Phase enumeration for type-safe DAR phase parameters.
 *
 * BOUNDS USED:
 *   - CPISI_PHASE_DETECT (0), CPISI_PHASE_ASSESS (1), CPISI_PHASE_RESTORE (2)
 *   - CPISI_PHASE_COUNT (3) — iteration bound = CPISI_SIGN_COUNT
 *   - CPISI_PHASE_ASSESS (1) — CENTER of the DAR cycle
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for the Detect-Assess-Restore diagnostic cycle. Forms
 *   the X-axis of the 27-offset space (phase × 9). Each phase represents a
 *   stage in error handling: witness → evaluate → recover.
 *
 * VALUE MAPPING:
 *   CPISI_PHASE_ENUM_DETECT (0) — witness phase, observe problem (bal=-1)
 *   CPISI_PHASE_ENUM_ASSESS (1) — evaluate phase, analyze severity (bal=0, CENTER)
 *   CPISI_PHASE_ENUM_RESTORE (2) — recover phase, implement solution (bal=+1)
 *   CPISI_PHASE_ENUM_COUNT (3) — iteration bound
 *
 * USE CASE:
 *   - Phase tracking: cpisi_enter_phase(CpisiPhase phase);
 *   - Offset calculation: offset = (phase × 9) + (severity × 3) + aspect;
 *   - State machine: switch (phase) { case CPISI_PHASE_ENUM_DETECT: ... }
 *
 * RELATIONSHIP TO LOOKUP (S.2d.8a TABLES):
 *   - cpisi_phase_name(idx) → "DETECT", "ASSESS", "RESTORE"
 *   - cpisi_phase_abbrev(idx) → "DET", "ASS", "RES"
 *   - cpisi_phase_balanced(idx) → -1, 0, +1
 */
typedef enum CpisiPhase {
    CPISI_PHASE_ENUM_DETECT  = CPISI_PHASE_DETECT,   /**< 0: Detect — identify issues */
    CPISI_PHASE_ENUM_ASSESS  = CPISI_PHASE_ASSESS,   /**< 1: Assess — evaluate severity (CENTER) */
    CPISI_PHASE_ENUM_RESTORE = CPISI_PHASE_RESTORE,  /**< 2: Restore — apply remediation */
    CPISI_PHASE_ENUM_COUNT   = CPISI_PHASE_COUNT     /**< 3: Iteration bound */
} CpisiPhase;


/** @} */ /* End of S2e_6a_PHASE */

/**
 * @defgroup S2e_6b_SEVERITY_ROW S.2e.6b CpisiSeverityRow — Severity row enum
 * @brief Type-safe severity row — MILD(0), MEDIUM(1), CRITICAL(2).
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — 3-Level Severity (offset Y-axis):                   │
 * │                                                                            │
 * │    MILD     (0) — warning level, low urgency (bal=-1)                      │
 * │    MEDIUM   (1) — degraded level, moderate urgency (bal=0) ← CENTER        │
 * │    CRITICAL (2) — fatal level, high urgency (bal=+1)                       │
 * │                                                                            │
 * │  NOTE: This is distinct from CpisiSeverity (5 levels).                     │
 * │  SeverityRow provides the 3-value version for offset calculation.          │
 * │  offset contribution: severity × 3                                         │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup cpisi_phase_types
 * @{ */

/**
 * @brief Severity row enumeration for 3-level offset calculation.
 *
 * BOUNDS USED:
 *   - CPISI_SEVERITY_ROW_MILD (0), MEDIUM (1), CRITICAL (2)
 *   - CPISI_SEVERITY_ROW_COUNT (3) — iteration bound = CPISI_SIGN_COUNT
 *   - CPISI_SEVERITY_ROW_MEDIUM (1) — CENTER of the 3-level scale
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for 3-level severity in offset calculations. Distinct
 *   from CpisiSeverity (5 levels) — this provides the ternary-friendly 3-value
 *   version. Forms the Y-axis of the 27-offset space (severity × 3).
 *
 * VALUE MAPPING:
 *   CPISI_SEVERITY_ENUM_MILD (0) — warning level, low urgency (bal=-1)
 *   CPISI_SEVERITY_ENUM_MEDIUM (1) — degraded level, moderate urgency (bal=0, CENTER)
 *   CPISI_SEVERITY_ENUM_CRITICAL (2) — fatal level, high urgency (bal=+1)
 *   CPISI_SEVERITY_ENUM_COUNT (3) — iteration bound
 *
 * USE CASE:
 *   - Offset calculation: offset = (phase × 9) + (severity × 3) + aspect;
 *   - Severity classification: severity = cpisi_classify_severity_row(error);
 *   - Distinct from 5-level CpisiSeverity for offset arithmetic
 *
 * RELATIONSHIP TO CpisiSeverity:
 *   CpisiSeverity = 5-level alert classification (aerospace-inspired)
 *   CpisiSeverityRow = 3-level offset dimension (balanced ternary)
 *   Different purposes: alerts vs offset calculation.
 *
 * RELATIONSHIP TO LOOKUP (S.2d.8a TABLES):
 *   - cpisi_severity_row_name(idx) → "MILD", "MEDIUM", "CRITICAL"
 *   - cpisi_severity_row_balanced(idx) → -1, 0, +1
 */
typedef enum CpisiSeverityRow {
    CPISI_SEVERITY_ENUM_MILD     = CPISI_SEVERITY_ROW_MILD,      /**< 0: Warning level — low urgency */
    CPISI_SEVERITY_ENUM_MEDIUM   = CPISI_SEVERITY_ROW_MEDIUM,    /**< 1: Degraded level — moderate (CENTER) */
    CPISI_SEVERITY_ENUM_CRITICAL = CPISI_SEVERITY_ROW_CRITICAL,  /**< 2: Fatal level — high urgency */
    CPISI_SEVERITY_ENUM_COUNT    = CPISI_SEVERITY_ROW_COUNT      /**< 3: Iteration bound */
} CpisiSeverityRow;


/** @} */ /* End of S2e_6b_SEVERITY_ROW */

/**
 * @defgroup S2e_6c_ASPECT S.2e.6c CpisiAspect — Aspect enum
 * @brief Type-safe aspect — ENTRY(0), PROCESS(1), EXIT(2).
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │  SEMANTIC FOUNDATION — 3-Stage Aspect (offset Z-axis):                     │
 * │                                                                            │
 * │    ENTRY   (0) — initialization, setup phase (bal=-1)                      │
 * │    PROCESS (1) — handling, main execution (bal=0) ← CENTER                 │
 * │    EXIT    (2) — cleanup, teardown phase (bal=+1)                          │
 * │                                                                            │
 * │  Every operation has these three aspects:                                  │
 * │    1. Enter/initialize                                                     │
 * │    2. Process/handle                                                       │
 * │    3. Exit/cleanup                                                         │
 * │                                                                            │
 * │  offset contribution: aspect × 1 (direct)                                  │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @ingroup cpisi_phase_types
 * @{ */

/**
 * @brief Aspect enumeration for type-safe aspect parameters.
 *
 * BOUNDS USED:
 *   - CPISI_ASPECT_ENTRY (0), CPISI_ASPECT_PROCESS (1), CPISI_ASPECT_EXIT (2)
 *   - CPISI_ASPECT_COUNT (3) — iteration bound = CPISI_SIGN_COUNT
 *   - CPISI_ASPECT_PROCESS (1) — CENTER of the aspect dimension
 *
 * SEMANTIC MEANING:
 *   Type-safe wrapper for the 3-stage operation aspect. Forms the Z-axis of
 *   the 27-offset space (aspect × 1, direct). Every operation conceptually
 *   has entry (setup), process (execute), and exit (cleanup) phases.
 *
 * VALUE MAPPING:
 *   CPISI_ASPECT_ENUM_ENTRY (0) — initialization, setup phase (bal=-1)
 *   CPISI_ASPECT_ENUM_PROCESS (1) — handling, main execution (bal=0, CENTER)
 *   CPISI_ASPECT_ENUM_EXIT (2) — cleanup, teardown phase (bal=+1)
 *   CPISI_ASPECT_ENUM_COUNT (3) — iteration bound
 *
 * USE CASE:
 *   - Aspect tracking: cpisi_set_aspect(CpisiAspect aspect);
 *   - Offset calculation: offset = (phase × 9) + (severity × 3) + aspect;
 *   - Error context: "failed during ENTRY aspect of DETECT phase"
 *
 * RELATIONSHIP TO LOOKUP (S.2d.8a TABLES):
 *   - cpisi_aspect_name(idx) → "ENTRY", "PROCESS", "EXIT"
 *   - cpisi_aspect_abbrev(idx) → "ENT", "PRC", "EXT"
 *   - cpisi_aspect_balanced(idx) → -1, 0, +1
 */
typedef enum CpisiAspect {
    CPISI_ASPECT_ENUM_ENTRY   = CPISI_ASPECT_ENTRY,    /**< 0: Entry — initialization, setup */
    CPISI_ASPECT_ENUM_PROCESS = CPISI_ASPECT_PROCESS,  /**< 1: Process — main execution (CENTER) */
    CPISI_ASPECT_ENUM_EXIT    = CPISI_ASPECT_EXIT,     /**< 2: Exit — cleanup, teardown */
    CPISI_ASPECT_ENUM_COUNT   = CPISI_ASPECT_COUNT     /**< 3: Iteration bound */
} CpisiAspect;


/** @} */ /* end S.2e.6c ASPECT */

/** @} */ /* end S.2e TYPES */

// ─── SED HERE: S.2e.7 METADATA_DOMAIN ─── see types-metadata.h ────────────────

/** @} */ /* end S.2 DEFINES */

/** S.3 TYPES — [Reserved: phase types in S.2e] @defgroup cpisi_phase_types S.3 TYPES @{ @} */

/** S.4 PROTOTYPES — [Reserved: accessors inline in B.5] @defgroup cpisi_phase_prototypes S.4 PROTOTYPES @{ @} */

/** S.5 FILE-LEVEL — [Reserved: no file-level state] @defgroup cpisi_phase_file S.5 FILE-LEVEL @{ @} */

/** S.6 OMISSION — [Reserved: S.3-S.5 empty, types in S.2e/BODY] @defgroup cpisi_phase_omission S.6 OMISSION @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief DAR types and functions owned by phase.h (domain ownership).
///
/// CONTAINS:
///   - B.1 ORG CHART      — Structure overview
///   - B.2 TYPES          — DARPhase, RestoreResult, RestoreStrategy, DAREvent
///   - B.3 HELPERS        — DAR cycle operations, info lookups
///   - B.4 ERRORS         — (Reserved)
///   - B.5 PUBLIC API     — Accessors, parse functions
///   - B.6 OMISSION       — Reserved section guide
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_orgchart B.1 ORG CHART — Structure Overview
 * @ingroup cpisi_phase_body
 * @brief    Map structure — interface overview, implementation location.
 *
 * WHY: Understanding the file structure before diving into details.
 *
 * SCOPE: File-level organization, implementation location, interface counts.
 *
 * CONTAINS:
 *   - B.1a Structure — Types, functions, externs count
 *   - B.1b Flow      — Implementation location
 *   - B.1c Counts    — Summary statistics
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Interface Overview [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      5 (DARPhase, RestoreResult, RestoreStrategy, DAREvent, DARHistory) */
/* Structs:    4 (DARPhaseInfo, RestoreResultInfo, DAREvent, DARHistory) */
/* Functions:  18 inline (info, validation, accessors, DAR cycle) */
/* Externs:    0 */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: Inline in header (all functions) */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      5 total (3 enums, 2 structs) */
/* Functions:  18 total */
/* Externs:    0 total */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 DAR Types [DAR_TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dar B.2 DAR Types
 * @ingroup cpisi_phase_body
 * @brief    Types for Detect-Assess-Restore witness system.
 *
 * WHY: DAR types owned by phase.h (domain ownership).
 *      3-phase witness system: Detect → Assess → Restore.
 *
 * SCOPE: DARPhase (3), RestoreResult (3), RestoreStrategy (5), DAREvent.
 *
 * CONTAINS:
 *   - B.2a DARPhase        — Witness phases (3 values)
 *   - B.2b RestoreResult   — Restore outcomes (7 values)
 *   - B.2c RestoreStrategy — Recovery approaches (6 values)
 *   - B.2d DAREvent        — Event record structure
 *
 * > "Let us search and try our ways" — Lamentations 3:40
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

// ─────────────────────────────────────────────────────────────────────────────
// B.2a DARPhase (3) — witness phases with unified lookup
// ─────────────────────────────────────────────────────────────────────────────
//
// 3-phase witness system: Detect → Assess → Restore
// Each phase has subsystem, scripture, and display metadata.
//
// ┌─────────┬─────────────┬─────────────────────────────────────────────────────┐
// │ Phase   │ Subsystem   │ Role                                                │
// ├─────────┼─────────────┼─────────────────────────────────────────────────────┤
// │ DETECT  │ cpisi_log   │ Notice, witness, record (Lam 3:40)                  │
// │ ASSESS  │ cpisi_debug │ Evaluate, diagnose, score (Prov 15:22)              │
// │ RESTORE │ cpisi_restore │ Recover, fix, verify (Prov 24:16)                 │
// └─────────┴─────────────┴─────────────────────────────────────────────────────┘

/**
 * @name DARPhase [DAR_PHASE]
 * @brief The three witness phases of the DAR system with unified lookup.
 * @{
 */

/**
 * @brief   Three-phase witness system
 *
 * DAR = Detect → Assess → Restore
 * Each phase builds on the previous, forming a complete witness cycle.
 *
 * @note    Default: DAR_DETECT (logging/observing)
 */
typedef enum DARPhase {
    DAR_DETECT  = 0,    /**< Logging, observing, witnessing */
    DAR_ASSESS  = 1,    /**< Evaluating, diagnosing, debugging */
    DAR_RESTORE = 2,    /**< Recovering, fixing, restoring */
} DARPhase;

/** @brief Total number of DAR phases */
#define DAR_PHASE_COUNT (3)

/**
 * @brief   Unified lookup table for DARPhase
 *
 * Contains all phase metadata in ONE struct.
 */
typedef struct DARPhaseInfo {
    const char* name;       /**< "DETECT", "ASSESS", "RESTORE" */
    const char* subsystem;  /**< "cpisi_log", "cpisi_debug", "cpisi_restore" */
    const char* emoji;      /**< Visual symbol */
    const char* scripture;  /**< Scripture reference */
    const char* role;       /**< Brief description */
} DARPhaseInfo;

/**
 * @brief   MASTER lookup for DARPhase
 *
 * Access: DAR_PHASE_INFO[phase]
 * Or use: dar_phase_info(phase) inline accessor
 */
static const DARPhaseInfo DAR_PHASE_INFO[DAR_PHASE_COUNT] = {
    [DAR_DETECT]  = { "DETECT",  "cpisi_log",     "👁️", "Lam 3:40",  "witness, record"   },
    [DAR_ASSESS]  = { "ASSESS",  "cpisi_debug",   "🔍", "Prov 15:22", "diagnose, score"  },
    [DAR_RESTORE] = { "RESTORE", "cpisi_restore", "🔧", "Prov 24:16", "recover, verify"  },
};

/**
 * @brief   Get DARPhaseInfo for a phase
 * @param   phase  DARPhase enum value
 * @return  Pointer to info struct (defaults to DETECT if invalid)
 */
static inline const DARPhaseInfo* dar_phase_info(DARPhase phase) {
    if (phase < 0 || phase >= DAR_PHASE_COUNT) phase = DAR_DETECT;
    return &DAR_PHASE_INFO[phase];
}

/**
 * @brief   Get phase name
 * @param   phase  DARPhase enum value
 * @return  Phase name string
 */
static inline const char* dar_phase_name(DARPhase phase) {
    return dar_phase_info(phase)->name;
}

/**
 * @brief   Get phase emoji
 * @param   phase  DARPhase enum value
 * @return  Phase emoji string
 */
static inline const char* dar_phase_emoji(DARPhase phase) {
    return dar_phase_info(phase)->emoji;
}

/**
 * @brief   Clamp int to valid DARPhase range [0, 2]
 */
static inline DARPhase cpisi_dar_phase_clamp(int value) {
    return (DARPhase)CPISI_CLAMP(value, 0, DAR_PHASE_COUNT - 1);
}

/**
 * @brief   Check if DARPhase is valid
 */
static inline bool cpisi_dar_phase_valid(DARPhase phase) {
    return CPISI_IN_RANGE_UNSIGNED(phase, DAR_PHASE_COUNT - 1);
}

/** @} */ // end DARPhase

// ATOMIC_SUCCESS, ATOMIC_FAILURE, ATOMIC_NEUTRAL — defined in types.h foundation
// They're primitive ternary-derived constants used by logging (B.2e)

// ─────────────────────────────────────────────────────────────────────────────
// B.2b Restore Types — Recovery Phase
// ─────────────────────────────────────────────────────────────────────────────
//
// Types for the RESTORE phase — recovering and fixing issues.
// RestoreResult is ternary-aligned (-3 to +3), RestoreStrategy is bit flags.
//
// Scripture: "He restoreth my soul" — Psalm 23:3 KJV
// Config: bereshit/word/core/os/health/restore.toml

/**
 * @name RestoreResult [RESTORE_RESULT]
 * @brief Outcome of restoration attempt, ternary-aligned.
 * @{
 */

/**
 * @brief   Restoration outcome
 *
 * Ternary-aligned: negative = failure, zero = partial, positive = success.
 * Range: -3 (fatal) to +3 (located alternate).
 */
typedef enum RestoreResult {
    // Failures (k = -1 territory)
    RESTORE_FATAL        = -3,  /**< Unrecoverable - must stop */
    RESTORE_FAIL         = -2,  /**< Recovery attempted, failed */
    RESTORE_INVALID      = -1,  /**< Invalid input, can't attempt */

    // Partial (k = 0 center)
    RESTORE_DEGRADED     =  0,  /**< Recovered but with reduced capability */

    // Success (k = +1 territory)
    RESTORE_OK           = +1,  /**< Full recovery, no degradation */
    RESTORE_FILLED       = +2,  /**< Missing value filled with default */
    RESTORE_LOCATED      = +3,  /**< File found in alternate location */
} RestoreResult;

/** @brief Count of RestoreResult values */
#define RESTORE_RESULT_COUNT (7)
/** @brief Minimum RestoreResult value */
#define RESTORE_RESULT_MIN   RESTORE_FATAL
/** @brief Maximum RestoreResult value */
#define RESTORE_RESULT_MAX   RESTORE_LOCATED
/** @brief Default RestoreResult (degraded) */
#define RESTORE_RESULT_DEFAULT RESTORE_DEGRADED

/**
 * @brief   Unified lookup table for RestoreResult
 */
typedef struct RestoreResultInfo {
    int         value;      /**< Enum value (-3 to +3) */
    KFactor     kfactor;    /**< Direction (retreat/maintain/advance) */
    float       ternary;    /**< Ternary value (-1.0 to +1.0) */
    const char* name;       /**< "fatal", "ok", etc. */
    const char* emoji;      /**< Visual symbol */
} RestoreResultInfo;

/**
 * @brief   MASTER lookup for RestoreResult
 *
 * Indexed by (result + 3) to handle negative values.
 */
static const RestoreResultInfo RESTORE_RESULT_INFO[RESTORE_RESULT_COUNT] = {
    [0] = { -3, K_RETREAT,  -1.00f, "fatal",    "💀" },  // RESTORE_FATAL
    [1] = { -2, K_RETREAT,  -0.67f, "fail",     "❌" },  // RESTORE_FAIL
    [2] = { -1, K_RETREAT,  -0.33f, "invalid",  "⛔" },  // RESTORE_INVALID
    [3] = {  0, K_MAINTAIN,  0.00f, "degraded", "⚠️" },  // RESTORE_DEGRADED
    [4] = { +1, K_ADVANCE,  +0.33f, "ok",       "✅" },  // RESTORE_OK
    [5] = { +2, K_ADVANCE,  +0.67f, "filled",   "📝" },  // RESTORE_FILLED
    [6] = { +3, K_ADVANCE,  +1.00f, "located",  "📍" },  // RESTORE_LOCATED
};

/**
 * @brief   Get RestoreResultInfo for a result
 * @param   result  RestoreResult enum value
 * @return  Pointer to info struct (defaults to DEGRADED if invalid)
 */
static inline const RestoreResultInfo* restore_result_info(RestoreResult result) {
    int idx = result + 3;  // Map -3..+3 to 0..6
    if (idx < 0 || idx >= RESTORE_RESULT_COUNT) idx = 3;  // Default to DEGRADED
    return &RESTORE_RESULT_INFO[idx];
}

/**
 * @brief   Get result k-factor direction
 */
static inline KFactor restore_result_to_kfactor(RestoreResult result) {
    return restore_result_info(result)->kfactor;
}

/** @} */ // end RestoreResult

/**
 * @name RestoreStrategy [RESTORE_STRATEGY]
 * @brief Bit flags for restoration approach.
 * @{
 */

/**
 * @brief   How to attempt restoration
 *
 * Bit flags enable fine-grained control over recovery behavior.
 * From bereshit/word/core/os/health/restore.toml [prescription.types].
 */
typedef enum RestoreStrategy {
    RESTORE_STRATEGY_NONE           = 0,         /**< No recovery */
    RESTORE_STRATEGY_FILL_DEFAULTS  = (1 << 0),  /**< Fill missing with defaults */
    RESTORE_STRATEGY_LOCATE_FILES   = (1 << 1),  /**< Search for misplaced files */
    RESTORE_STRATEGY_CREATE_DIRS    = (1 << 2),  /**< Create missing directories */
    RESTORE_STRATEGY_CLAMP_VALUES   = (1 << 3),  /**< Clamp out-of-range values */
    RESTORE_STRATEGY_FALLBACK       = (1 << 4),  /**< Use fallback resources */
    RESTORE_STRATEGY_DEGRADE        = (1 << 5),  /**< Graceful degradation */

    // Combined strategies
    RESTORE_STRATEGY_BASIC = RESTORE_STRATEGY_FILL_DEFAULTS
                           | RESTORE_STRATEGY_CLAMP_VALUES,

    RESTORE_STRATEGY_FULL  = RESTORE_STRATEGY_FILL_DEFAULTS
                           | RESTORE_STRATEGY_LOCATE_FILES
                           | RESTORE_STRATEGY_CREATE_DIRS
                           | RESTORE_STRATEGY_CLAMP_VALUES
                           | RESTORE_STRATEGY_FALLBACK
                           | RESTORE_STRATEGY_DEGRADE,
} RestoreStrategy;

/** @} */ // end RestoreStrategy

// ─────────────────────────────────────────────────────────────────────────────
// B.2c DAREvent — Event Record Structure
// ─────────────────────────────────────────────────────────────────────────────
//
// Event structures recording complete DETECT→ASSESS→RESTORE cycles.
// These depend on types from B.2a-b (DARPhase, RestoreResult, RestoreStrategy)
// and health.h (HealthScore, HebrewState).
//
// Scripture: "And the books were opened" — Revelation 20:12

/**
 * @name DAREvents [DAR_EVENTS]
 * @brief Event structures for recording DAR cycles.
 * @{
 */

/**
 * @brief   Tripwire event — fallback path taken
 *
 * Records when primary path failed and a fallback was used.
 * Part of error recovery and graceful degradation patterns.
 */
typedef struct TripwireEvent {
    uint64_t    timestamp_ms;       /**< When the tripwire fired */
    const char* primary_path;       /**< What we tried to do */
    const char* fallback_path;      /**< What we fell back to */
    const char* component;          /**< Which component */
    int32_t     health_impact;      /**< How much this affected health */
} TripwireEvent;

/**
 * @brief   Complete DAR cycle event record
 *
 * Records a full DETECT→ASSESS→RESTORE witness cycle.
 */
typedef struct DAREvent {
    uint64_t        id;                 /**< Unique event ID */
    uint64_t        timestamp_ms;       /**< When detection occurred */

    // DETECT phase data
    int             detect_type;        /**< What triggered detection (DetectType) */
    const char*     detect_source;      /**< Where detection occurred */
    const char*     detect_message;     /**< What was detected */

    // ASSESS phase data
    int             severity;           /**< How serious (Severity) */
    HebrewState     assessed_state;     /**< Cognitive state at assessment */
    HealthScore     health_at_detect;   /**< Health when detected */
    KFactor         k_at_detect;        /**< Direction when detected */

    // RESTORE phase data
    RestoreStrategy strategy;           /**< Chosen restoration approach */
    RestoreResult   result;             /**< Outcome of restoration */
    HealthScore     health_after;       /**< Health after restoration */
    uint32_t        restore_ms;         /**< How long restoration took */

    // Context
    int32_t         cube_pos[3];        /**< Mental construct position [x,y,z] */
} DAREvent;

/**
 * @brief   DAR history — circular buffer of events
 *
 * Tracks recent DAR events with summary statistics.
 * Uses CPISI_SIZE_RUNTIME (243) for buffer size — config.h aliases this as DAR_HISTORY.
 */
typedef struct DARHistory {
    DAREvent    events[CPISI_SIZE_RUNTIME];  /**< Circular buffer (243 = L2 runtime scale) */
    uint32_t    head;                       /**< Next write position */
    uint32_t    count;                      /**< Total events logged */
    uint64_t    next_id;                    /**< Next event ID to assign */

    // Summary statistics
    uint32_t    detect_count;               /**< Total detections */
    uint32_t    restore_success;            /**< Successful restorations */
    uint32_t    restore_fail;               /**< Failed restorations */
    uint32_t    restore_degraded;           /**< Degraded restorations */
} DARHistory;

/**
 * @brief   Initialize DAR history
 * @param   history  History struct to initialize
 */
static inline void cpisi_dar_history_init(DARHistory* history) {
    history->head = 0;
    history->count = 0;
    history->next_id = 1;
    history->detect_count = 0;
    history->restore_success = 0;
    history->restore_fail = 0;
    history->restore_degraded = 0;
}

/**
 * @brief   Add event to DAR history
 * @param   history  History struct
 * @param   event    Event to add (id will be assigned)
 * @return  Assigned event ID
 */
static inline uint64_t cpisi_dar_history_add(DARHistory* history, DAREvent* event) {
    event->id = history->next_id++;
    history->events[history->head] = *event;
    history->head = (history->head + 1) % CPISI_SIZE_RUNTIME;
    history->count++;
    history->detect_count++;

    // Update stats based on result
    if (event->result > 0) {
        history->restore_success++;
    } else if (event->result < 0) {
        history->restore_fail++;
    } else {
        history->restore_degraded++;
    }

    return event->id;
}

/** @} */ /* end DAREvents */

/** @} */ /* end B.1 DAR Types */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 HELPERS — DAR Cycle Operations [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_helpers B.3 HELPERS — DAR Cycle Operations
 * @ingroup cpisi_phase_body
 * @brief    DAR cycle operation functions.
 *
 * WHY: DAR functions owned by phase.h (domain ownership).
 *      Implements the Detect-Assess-Restore witness cycle.
 *
 * SCOPE: DARResult structure and cpisi_dar_* cycle functions.
 *
 * CONTAINS:
 *   - B.3a DARResult        — Result structure for cycle operations
 *   - B.3b Cycle Functions  — cpisi_dar_detect, cpisi_dar_assess, cpisi_dar_restore
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/**
 * @brief   DAR result structure for cycle operations.
 *
 * Captures phase, severity, and resulting state from each DAR operation.
 */
typedef struct DARResult {
    DARPhase        phase;      /**< Current/completed phase */
    HealthLevel     severity;   /**< Assessed severity (0-6) */
    HebrewState     state;      /**< Resulting Hebrew state */
    HealthScore     health;     /**< Resulting health score */
    bool            success;    /**< Phase completed successfully */
} DARResult;

/** @brief Zero-initialized DARResult */
#define DAR_RESULT_ZERO { DAR_DETECT, HEALTH_LEVEL_EVEN, HEBREW_YASHAR, 0, false }

/**
 * @brief   DETECT phase: witness and record an issue
 * @param   current_health  Current health score
 * @param   threshold       Threshold below which issue is detected
 * @return  DARResult with detection status
 * @note    Uses dar_phase_info(DAR_DETECT) for phase metadata
 */
static inline DARResult cpisi_dar_detect(HealthScore current_health, HealthScore threshold) {
    bool detected = current_health < threshold;
    return (DARResult){
        .phase    = DAR_DETECT,
        .severity = detected ? cpisi_health_to_level(current_health) : HEALTH_LEVEL_EVEN,
        .state    = detected ? cpisi_health_to_hebrew(current_health, K_MAINTAIN) : HEBREW_YASHAR,
        .health   = current_health,
        .success  = true  /* DETECT always succeeds - it's observation */
    };
}

/**
 * @brief   ASSESS phase: diagnose and score severity
 * @param   detection   Result from DETECT phase
 * @return  DARResult with assessed severity
 * @note    Uses HEALTH_LEVEL_INFO for severity classification
 */
static inline DARResult cpisi_dar_assess(DARResult detection) {
    HealthLevel severity = cpisi_health_to_level(detection.health);
    const HealthLevelInfo* info = health_level_info(severity);
    return (DARResult){
        .phase    = DAR_ASSESS,
        .severity = severity,
        .state    = info->hebrew,
        .health   = detection.health,
        .success  = true  /* ASSESS always succeeds - it's diagnosis */
    };
}

/**
 * @brief   RESTORE phase: recover to safe state
 * @param   assessment  Result from ASSESS phase
 * @param   target      Target health to restore toward
 * @return  DARResult with restored state
 * @note    Uses cpisi_health_adjust() from health.h for gradual recovery
 */
static inline DARResult cpisi_dar_restore(DARResult assessment, HealthScore target) {
    /* Calculate recovery delta (step toward target) */
    int delta = (target > assessment.health) ? +1 :
                (target < assessment.health) ? -1 : 0;

    HealthScore restored = cpisi_health_adjust(assessment.health, delta * 10);

    return (DARResult){
        .phase    = DAR_RESTORE,
        .severity = cpisi_health_to_level(restored),
        .state    = cpisi_health_to_hebrew(restored, K_MAINTAIN),
        .health   = restored,
        .success  = (delta == 0) || (restored != assessment.health)
    };
}

/**
 * @brief   Full DAR cycle: Detect → Assess → Restore
 * @param   current_health  Current health score
 * @param   threshold       Detection threshold
 * @param   target          Recovery target
 * @return  Final DARResult after full cycle
 * @note    Chains all three phases using DAR_PHASE_INFO
 */
static inline DARResult cpisi_dar_cycle(HealthScore current_health,
                                         HealthScore threshold,
                                         HealthScore target) {
    DARResult detect = cpisi_dar_detect(current_health, threshold);

    /* If nothing detected, no need to assess/restore */
    if (detect.health >= threshold) {
        return detect;
    }

    DARResult assess = cpisi_dar_assess(detect);
    DARResult restore = cpisi_dar_restore(assess, target);

    return restore;
}

/** @} */ /* end B.3 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERROR HANDLING — Recovery Patterns [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_errors B.4 ERROR HANDLING — Recovery Patterns
 * @ingroup cpisi_phase_body
 * @brief    (Reserved) DAR-specific error recovery patterns.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [Reserved: DAR error recovery functions] */

/** @} */ /* end B.4 ERROR HANDLING */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Accessors & Parse [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_api B.5 PUBLIC API — Accessors & Parse
 * @ingroup cpisi_phase_body
 * @brief    Public accessor and parse functions for DAR types.
 *
 * WHY: External interface for DAR type information.
 *
 * CONTAINS:
 *   - B.5a DARPhase Accessors      — name, emoji, parse
 *   - B.5b RestoreResult Accessors — name
 *   - B.5c RestoreStrategy Accessors — name
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

// ─────────────────────────────────────────────────────────────────────────────
// B.5a DARPhase Accessors — name, emoji, parse
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief   Get name for DARPhase (public API wrapper)
 */
static inline const char* cpisi_dar_phase_name(DARPhase phase) {
    return dar_phase_name(phase);
}

/**
 * @brief   Get emoji symbol for DARPhase (public API wrapper)
 */
static inline const char* cpisi_dar_phase_emoji(DARPhase phase) {
    return dar_phase_emoji(phase);
}

/**
 * @brief   Parse string to DARPhase
 * @param   name    String to parse ("DETECT", "ASSESS", "RESTORE")
 * @return  DARPhase enum, defaults to DAR_DETECT
 */
static inline DARPhase cpisi_dar_phase_parse(const char* name) {
    if (!name) return DAR_DETECT;
    if (name[0] == 'D' || name[0] == 'd') {
        if (strncmp(name, "DETECT", 6) == 0 || strncmp(name, "detect", 6) == 0) return DAR_DETECT;
    }
    if (name[0] == 'A' || name[0] == 'a') {
        if (strncmp(name, "ASSESS", 6) == 0 || strncmp(name, "assess", 6) == 0) return DAR_ASSESS;
    }
    if (name[0] == 'R' || name[0] == 'r') {
        if (strncmp(name, "RESTORE", 7) == 0 || strncmp(name, "restore", 7) == 0) return DAR_RESTORE;
    }
    return DAR_DETECT;
}

// ─────────────────────────────────────────────────────────────────────────────
// B.5b RestoreResult Accessors — name
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief   Get result name
 */
static inline const char* restore_result_name(RestoreResult result) {
    return restore_result_info(result)->name;
}

// ─────────────────────────────────────────────────────────────────────────────
// B.5c RestoreStrategy Accessors — name
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief   Get strategy name (combined strategies first, then individual)
 */
static inline const char* restore_strategy_name(RestoreStrategy strat) {
    if (strat == RESTORE_STRATEGY_NONE) return "none";
    if (strat == RESTORE_STRATEGY_BASIC) return "basic";
    if (strat == RESTORE_STRATEGY_FULL) return "full";
    if (strat & RESTORE_STRATEGY_FILL_DEFAULTS) return "fill_defaults";
    if (strat & RESTORE_STRATEGY_LOCATE_FILES) return "locate_files";
    if (strat & RESTORE_STRATEGY_CREATE_DIRS) return "create_dirs";
    if (strat & RESTORE_STRATEGY_CLAMP_VALUES) return "clamp_values";
    if (strat & RESTORE_STRATEGY_FALLBACK) return "fallback";
    if (strat & RESTORE_STRATEGY_DEGRADE) return "degrade";
    return "custom";
}

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_b6_omission B.6 OMISSION — Reserved Section Guide
 * @ingroup cpisi_phase_body
 * @brief    Documents what sections are reserved and why.
 *
 * PURPOSE: Explicit documentation of intentionally empty sections.
 *
 * RESERVED SECTIONS:
 *   B.4 ERRORS — Error handling patterns reserved for future
 *
 * IMPLEMENTED SECTIONS:
 *   B.1 ORG CHART  — Structure overview
 *   B.2 TYPES      — DAR types (DARPhase, RestoreResult, RestoreStrategy, DAREvent)
 *   B.3 HELPERS    — DAR cycle operations
 *   B.5 PUBLIC API — Accessors and parse functions
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [B.6 is documentation-only — no code content] */

/** @} */ /* end B.6 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Cleanup, guard close, and final metadata.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 SUMMARY — Component Summary [SUMMARY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_x1_summary X.1 SUMMARY — Component Summary
 * @brief    What this file provides.
 *
 * PROVIDES:
 *   Types:     5 (DARPhase, RestoreResult, RestoreStrategy, DAREvent, DARHistory)
 *   Functions: 18 inline (info, validation, accessors, DAR cycle)
 *   Externs:   0
 *
 * DOMAIN: DAR phase system (Detect-Assess-Restore × 3 aspects × 3 severities)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [Summary documentation only] */

/** @} */ /* end X.1 SUMMARY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 TESTING — Test Guidance [TESTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_x2_testing X.2 TESTING — Test Guidance
 * @brief    How to test this component.
 *
 * UNIT TESTS:
 *   - DAR phase transitions (DETECT → ASSESS → RESTORE)
 *   - Restore result encoding/decoding
 *   - Strategy bitmask operations
 *   - DAR event creation and history
 *
 * INTEGRATION:
 *   - Health system coordination
 *   - Error recovery workflows
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [Test guidance documentation only] */

/** @} */ /* end X.2 TESTING */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Resource Cleanup [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_x3_cleanup X.3 CLEANUP — Resource Cleanup
 * @brief    Cleanup operations for this component.
 *
 * CLEANUP:
 *   None — header-only, no runtime resources.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [No cleanup needed — header-only] */

/** @} */ /* end X.3 CLEANUP */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS [END_CODE]
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 CPP — C++ Linkage Close [CPP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_x4_cpp X.4 CPP — C++ Linkage Close
 * @brief    Close C++ compatibility wrapper.
 *
 * INHERITED: extern "C" close comes through types.h.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [Inherited from types.h — extern "C" close handled there] */

/** @} */ /* end X.4 CPP */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 GUARD — Include Guard Close [GUARD]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_x5_guard X.5 GUARD — Include Guard Close
 * @brief    Close include guard opened at file start.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

#endif /* CPISI_TYPES_PHASE_H */

/** @} */ /* end X.5 GUARD */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 ATTRIBUTION — Final Attribution [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_x6_attribution X.6 ATTRIBUTION — Final Attribution
 * @brief    Credit and acknowledgments.
 *
 * Authors:   Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)
 * Copyright: © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [Attribution documentation only] */

/** @} */ /* end X.6 ATTRIBUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 HISTORY — Change Log [HISTORY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_x7_history X.7 HISTORY — Change Log
 * @brief    Version history for this file.
 *
 * a-01.00 (2026-01-21) — Extracted from types.h phase sections
 * a-01.25 (2026-01-23) — Expanded METADATA to full M.1-M.10 structure
 * b-03.00 (2026-01-25) — Full type-header.h template alignment (4-block)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [History documentation only] */

/** @} */ /* end X.7 HISTORY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 SCRIPTURE — Biblical Foundation [SCRIPTURE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_x8_scripture X.8 SCRIPTURE — Biblical Foundation
 * @brief    Scriptural grounding for this component.
 *
 * "Let us search and try our ways, and turn again to the LORD."
 *   — Lamentations 3:40
 *
 * DAR (Detect-Assess-Restore) embodies this: search (Detect), try (Assess),
 * turn again (Restore).
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [Scripture documentation only] */

/** @} */ /* end X.8 SCRIPTURE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 EOF — End of File Marker [EOF]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_phase_x9_eof X.9 EOF — End of File Marker
 * @brief    Explicit end-of-file marker.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 */

/* [EOF] */

/** @} */ /* end X.9 EOF */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════
