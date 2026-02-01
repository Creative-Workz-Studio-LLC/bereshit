/// @file detect.h
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
/// @omni:req  key        = CPISI-DAR-DETECT
/// @omni:req  from       = templates/code/c/header.h
/// @omni:req  at         = b-03.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = dar-phase
/// @omni:inh  role       = detect
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi/dar
/// @omni:ins  layer      = dar/phases/detect
/// @omni:ins  includes   = types/types.h, dar/io/rotation.h
/// @omni:ins  provides   = [LogEntry, LogContext, DetectContext, logging functions]
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
/// @omni:req  title      = CPI-SI DAR DETECT Phase
/// @omni:req  brief      = State-aware logging system for witnessing events
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
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SP.1 CONFIG — Compile-time Settings [CONFIG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1a POSIX — Feature Test [POSIX]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_detect_special_config
 * @brief    Enables POSIX.1-2008 functions in standard headers.
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

#ifndef CPISI_DAR_DETECT_H
#define CPISI_DAR_DETECT_H

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
 * @file      detect.h
 * @brief     CPI-SI DAR DETECT Phase — State-aware logging and event witnessing.
 *
 * @defgroup cpisi_detect_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-DAR-DETECT
 * Title:     CPI-SI DAR DETECT Phase
 * Type:      Header (DAR Phase)
 * Component: DAR — first phase of Detect-Assess-Restore witness system
 * Role:      Witness and record events with full cognitive state context.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   b-03.00
 * @date      2026-01-25
 *
 * Status:    Active
 * Created:   2026-01-04
 * Updated:   2026-01-25
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authors and Copyright [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_detect_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/dar/phases/detect.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: types/types.h (foundation), kernel/types/composite/type/phase.type.h
 * Template:     bereshit/word/seed/code/c/type-header.h
 *
 * Builds FROM:  types.h (LogLevel, HebrewState), phase.h (DAR types)
 * Builds TO:    assess.h (ASSESS phase), dar.h (DAR aggregator)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: dar, detect, logging, witness, hebrew-state, cognitive-state
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Witness and record events with full cognitive state context.
 * Core Design: 9-level logging spectrum mapped to Hebrew states (-1.0 to +1.0).
 * Philosophy:  The log provides WITNESS data. It doesn't judge or act—just records.
 *
 * DAR Position: DETECT (first phase)
 *   DETECT (this) → ASSESS (evaluate) → RESTORE (recover)
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Lamentations 3:40 — "Let us search and try our ways"
 * Principle:  Good logs provide counsel for debugging and restoration.
 * Anchor:     Proverbs 15:22 — "Without counsel purposes are disappointed"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types/types.h (LogLevel, HebrewState, KFactor, HealthScore)
 *   Internal: dar/io/rotation.h (LogRotationState, LogRotationConfig)
 *   System:   stdarg.h (va_list), stdio.h (FILE*)
 *
 * What Uses This:
 *   - assess.h — ASSESS phase evaluates log evidence
 *   - restore.h — RESTORE phase acts on assessment
 *   - dar.h — DAR aggregator includes all phases
 *   - cpisi.h — Orchestrator wires DAR together
 *
 * @see types.h for foundation types
 * @see rotation.h for log rotation
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-02.00 (2026-01-04) — Initial DAR DETECT implementation
 *   a-02.50 (2026-01-16) — Added rotation, multi-scale scoring
 *   b-03.00 (2026-01-25) — Full type-header.h template alignment (4-block)
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ 9-level logging spectrum
 *   ✓ Hebrew state mapping
 *   ✓ Tripwire tracking
 *
 * Planned:
 *   ⏳ Structured log queries
 *   ⏳ Log aggregation API
 *
 * @} end M.10 ROADMAP
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END METADATA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * CODE SECTIONS [CODE_SECTIONS]
 * ───────────────────────────────────────────────────────────────────────────── */

/// ═══════════════════════════════════════════════════════════════════════════════
/// SETUP BLOCK [SETUP]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Includes, defines, types, and prototypes — compilation context.
///
/// STRUCTURE:
///   - S.1 INCLUDES   — Header dependencies
///   - S.2 DEFINES    — Configuration macros
///   - S.3 TYPES      — Type definitions
///   - S.4 PROTOTYPES — Function declarations
///   - S.5 FILE-LEVEL — (reserved)
///   - S.6 OMISSION   — (reserved)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_includes S.1 INCLUDES — Header Dependencies
 * @brief    Required headers for DETECT phase functionality.
 *
 * STRUCTURE:
 *   - S.1a STANDARD  — System headers (stdarg.h, stdio.h)
 *   - S.1b INTERNAL  — CPI-SI headers (log.h, phase.h, rotation.h)
 *   - S.1c EXTERNAL  — Third-party (none)
 *   - S.1d CPP       — C++ linkage wrapper
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_includes_standard S.1a STANDARD — System Headers
 * @ingroup  cpisi_detect_includes
 * @brief    Standard library headers for variadic functions and I/O.
 *
 * NOTE: Most system headers inherited transitively from log.h → config.h → types.h.
 *       Only DETECT-specific system headers listed here.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Foundation types — MUST BE FIRST in kernel-like ladder.
 *
 * WHY: kernel.h sets CPISI_MODE which controls what headers are available.
 *      MUST come before any CPISI_MODE guards or they won't work.
 */
#include "kernel.h"

/**
 * @brief Variable argument list support.
 *
 * WHY: Logging functions use printf-style variadic arguments. The cpisi_log()
 *      and cpisi_log_v() functions accept format strings with variable args.
 *      va_list enables wrapper functions to forward arguments properly.
 *
 * PROVIDES:
 *   - va_list type
 *   - va_start, va_end, va_arg macros
 *   - va_copy for forwarding
 *
 * NOTE: stdarg.h is freestanding-compatible (always available).
 */
#include <stdarg.h>

/**
 * @brief Standard I/O for file streams.
 *
 * WHY: DETECT writes to file streams — stderr for console output, FILE* for
 *      structured log files. Log rotation opens/closes files. All log output
 *      ultimately flows through FILE* streams.
 *
 * PROVIDES:
 *   - FILE* type
 *   - stderr, stdout streams
 *   - fprintf, fopen, fclose, fflush
 */
#if CPISI_MODE >= 2  /* Only in hosted mode (RUNTIME and above) */
#include <stdio.h>
#include <inttypes.h>   /**< PRIu64 for portable 64-bit printing */
#include <sys/types.h>  /**< pid_t for session tracking */
#else
/* Freestanding stubs */
typedef uint32_t pid_t;
#define PRIu64 "llu"
#endif

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — CPI-SI Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_includes_internal S.1b INTERNAL — CPI-SI Headers
 * @ingroup  cpisi_detect_includes
 * @brief    Internal CPI-SI type dependencies via proper ladder.
 *
 * KERNEL-LIKE HEADER LADDER (explicit, not transitive):
 *   1. kernel.h   — Foundation (sets CPISI_MODE) — ALREADY INCLUDED ABOVE
 *   2. log.h      — LogLevel enum, log configuration
 *   3. phase.h    — DAR phases, aspects, severity
 *   4. rotation.h — Log rotation state and config
 *
 * WHAT EACH PROVIDES:
 *   kernel.h   → Foundation types, CPISI_MODE, freestanding stubs
 *   log.h      → LogLevel, CpisiLogLevel, LOG_LEVEL_* constants
 *   phase.h    → DARPhase, DARAspect, DARSeverity, DAR offset encoding
 *   health.h   → HealthScore, HebrewState, KFactor (via phase.h)
 *   config.h   → CPISI_PATH_MAX, CPISI_MSG_MAX limits
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* kernel.h already included at top of S.1 INCLUDES */

/**
 * @brief Log configuration types — LogLevel enum and logging constants.
 *
 * WHY: DETECT phase IS the logging system. LogLevel defines the 9-level
 *      severity spectrum from CRISIS (-1.0) to INSIGHT (+1.0). This header
 *      provides the canonical log level definitions used throughout.
 *
 * PROVIDES:
 *   - LogLevel enum (LOG_CRISIS through LOG_INSIGHT)
 *   - LOG_LEVEL_COUNT, LOG_LEVEL_MIN, LOG_LEVEL_MAX
 *   - Level name lookup functions
 */
// Freestanding: log types provided by kernel.h or stubs below
#if CPISI_MODE >= 2
#include "kernel/types/config/cfg/log.cfg.h"
#else
// Log level stubs for freestanding kernel
typedef enum LogLevel {
    LOG_CRISIS = 0,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG,
    LOG_TRACE
} LogLevel;
#define LOG_LEVEL_COUNT 6
#define LOG_LEVEL_MIN LOG_CRISIS
#define LOG_LEVEL_MAX LOG_TRACE
#endif

/**
 * @brief DAR phase types — phases, aspects, and severity matrix.
 *
 * WHY: DETECT is the first phase of DAR (Detect-Assess-Restore). This header
 *      defines the phase system that DETECT participates in, including the
 *      3×3×3 = 27-cube offset encoding for phase/aspect/severity.
 *
 * PROVIDES:
 *   - DARPhase enum (DETECT, ASSESS, RESTORE)
 *   - DARAspect enum (ENTRY, PROCESS, EXIT)
 *   - DARSeverity enum (MILD, MEDIUM, CRITICAL)
 *   - DAR offset encoding functions
 *
 * CHAINS TO: health.h → config.h → types.h (transitively)
 */
#if CPISI_MODE >= 2
#include "kernel/types/composite/type/phase.type.h"
#else
// DAR phase stubs for freestanding kernel
typedef enum DARPhase { DAR_DETECT = 0, DAR_ASSESS, DAR_RESTORE } DARPhase;
typedef enum DARAspect { DAR_ENTRY = 0, DAR_PROCESS, DAR_EXIT } DARAspect;
typedef enum DARSeverity { DAR_MILD = 0, DAR_MEDIUM, DAR_CRITICAL } DARSeverity;
#endif

/**
 * @brief Log rotation — dated folders, archival, and retention.
 *
 * WHY: DETECT logs need rotation to prevent unbounded growth. This header
 *      provides LogRotationState and LogRotationConfig for managing log files
 *      with automatic dated folder organization and retention policies.
 *
 * PROVIDES:
 *   - LogRotationState struct (current rotation state)
 *   - LogRotationConfig struct (rotation settings)
 *   - Rotation lifecycle functions
 */
#include "witness/utility/rotation.util.h"

/** @} */ /* end S.1b INTERNAL */

/* S.1c EXTERNAL — [reserved: no third-party dependencies] */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_includes_cpp S.1d CPP — C++ Linkage
 * @ingroup  cpisi_detect_includes
 * @brief    C++ compatibility wrapper for C linkage.
 *
 * WHY: Enables C++ code to call DETECT functions without name mangling.
 *      The extern "C" block wraps all declarations through end of header.
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
 * S.2 DEFINES — Compile-time Definitions [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_defines S.2 DEFINES — Compile-time Definitions
 * @brief    Sizes, limits, flags, macros, and compile-time verification.
 *
 * STRUCTURE:
 *   - S.2a SIZES   — Buffer and array sizes
 *   - S.2b LIMITS  — Value bounds (min, max, default)
 *   - S.2c FLAGS   — Boolean and bit flags
 *   - S.2d MACROS  — Utility macros
 *   - S.2e VERIFY  — Compile-time assertions (tripwires)
 *
 * NOTE: Most defines inherited from log.h → config.h → types.h.
 *       Only DETECT-specific configuration listed here.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* S.2a SIZES — [inherited: sizes from config.h via log.h] */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b LIMITS — Configuration Defaults [LIMITS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_limits S.2b LIMITS — Configuration Defaults
 * @ingroup  cpisi_detect_defines
 * @brief    Default configuration values for DETECT logging.
 *
 * WHY: Allows compile-time override of logging defaults.
 *      User can define CPISI_LOG_LEVEL before including this header.
 *
 * INHERITED LIMITS (from config.h via log.h):
 *   - CPISI_PATH_MAX     — Maximum path length
 *   - CPISI_MSG_MAX      — Maximum message length
 *   - CPISI_NAME_MAX     — Maximum component name
 *   - CPISI_HISTORY_SIZE — Log entry history size
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Default minimum log level.
 *
 * WHY: Controls verbosity at compile time. INFO (center) shows warnings and
 *      errors but filters verbose debug output. Override with -DCPISI_LOG_LEVEL=X.
 *
 * THE 9-LEVEL SPECTRUM (from log.h):
 *
 *   -1.0  -0.75  -0.50  -0.25   0.0   +0.25  +0.50  +0.75  +1.0
 *    |      |      |      |      |      |      |      |      |
 * CRISIS FATAL  ERROR  WARN   INFO  NOTICE DEBUG  TRACE INSIGHT
 * shavar  ....  chaser ....  yashar ....  shalem  ....   tov
 *
 * FILTERING: Lower index = more severe. Higher index = more verbose.
 *   min_level = LOG_INFO (4)  → shows CRISIS through INFO
 *   min_level = LOG_DEBUG (6) → shows CRISIS through DEBUG
 */
#ifndef CPISI_LOG_LEVEL
#define CPISI_LOG_LEVEL LOG_INFO
#endif

/**
 * @brief Default log output stream.
 *
 * WHY: stderr is visible even when stdout is redirected. Most logging systems
 *      use stderr for diagnostic output. Override with -DCPISI_LOG_FILE=stream.
 */
#ifndef CPISI_LOG_FILE
#define CPISI_LOG_FILE stderr
#endif

/** @} */ /* end S.2b LIMITS */

/* S.2c FLAGS — [reserved: no DETECT-specific flags] */

/* S.2d MACROS — [see S.4c OPERATIONS for logging convenience macros] */

/* S.2e VERIFY — [reserved: compile-time assertions] */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Definitions [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_types S.3 TYPES — Type Definitions
 * @brief    Enumerations, structures, opaque types, and error codes.
 *
 * STRUCTURE:
 *   - S.3a ENUMS   — Enumeration types
 *   - S.3b STRUCTS — Structure definitions
 *   - S.3c OPAQUE  — Forward-declared opaque types
 *   - S.3d ERRORS  — Error code enumeration
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* S.3a ENUMS — [inherited: LogLevel from log.h, DARPhase from phase.h] */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — Structure Definitions [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_structs S.3b STRUCTS — Structure Definitions
 * @ingroup  cpisi_detect_types
 * @brief    DETECT phase structures for logging with CPI-SI cognitive awareness.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief A single log entry with full cognitive state context.
 *
 * WHY: Traditional logs capture WHAT happened. CPI-SI logs capture WHAT happened
 *      IN WHAT STATE. This enables pattern recognition across cognitive states.
 *
 * COGNITIVE CONTEXT:
 *   - hebrew_state — Where on the shavar→tov spectrum
 *   - k_factor     — Direction of movement (-1 retreat, 0 hold, +1 advance)
 *   - score        — Multi-scale health scoring (base100/50/20/10/5/3)
 *   - ternary      — Ternary bases (3/9/27/81) for DAR cube navigation
 *   - cube_*       — Position in 27-position mental construct
 *
 * QUERY SUPPORT:
 *   - Entries can be filtered by hebrew_state, k_factor, level
 *   - Aggregated by time windows, components, state transitions
 */
typedef struct LogEntry {
    /* ─── Temporal ─── */
    uint64_t    timestamp_ms;       /**< Milliseconds since epoch               */

    /* ─── Content ─── */
    LogLevel    level;              /**< Severity/insight level (9-level)       */
    const char* component;          /**< Source component name                  */
    const char* message;            /**< Log message content                    */

    /* ─── CPI-SI State ─── */
    HebrewState hebrew_state;       /**< Cognitive state (shavar→tov)           */
    KFactor     k_factor;           /**< Direction: retreat(-1)/hold(0)/advance(+1) */

    /* ─── Multi-scale Scoring ─── */
    MultiScore  score;              /**< Decimal bases (100/50/20/10/5/3)       */
    TernaryScore ternary;           /**< Ternary bases (3/9/27/81)              */

    /* ─── Mental Construct Position ─── */
    int32_t     cube_x;             /**< X: -1 to +1 (void/word/tov)            */
    int32_t     cube_y;             /**< Y: -1 to +1                            */
    int32_t     cube_z;             /**< Z: -1 to +1                            */
} LogEntry;

/**
 * @brief Persistent state for the logging system with full CPI-SI awareness.
 *
 * WHY: The LogContext maintains cognitive state across log calls, enabling
 *      the DAR Witness System to track patterns over time. Not just a logger,
 *      but a witness that remembers.
 *
 * SECTIONS:
 *   - Output control   — Where and how logs are written
 *   - Structured logs  — JSONL file output with full context
 *   - Log rotation     — Dated folders, archival, retention
 *   - Cognitive state  — HebrewState, k-factor, health
 *   - Multi-scale      — All scoring bases (decimal + ternary)
 *   - Mental construct — 27-position cube navigation
 *   - Tripwire         — Fallback tracking
 *   - Session          — Temporal boundaries
 */
typedef struct LogContext {
    /* ─── Output Control ─── */
    LogLevel    min_level;          /**< Minimum level to log                   */
    FILE*       output;             /**< Output stream (default: stderr)        */
    bool        include_state;      /**< Include Hebrew state in output         */
    bool        include_cube;       /**< Include cube position in output        */
    bool        include_scores;     /**< Include multi-scale scores in output   */
    bool        timestamps;         /**< Include timestamps                     */
    bool        colors;             /**< Use ANSI colors                        */

    /* ─── Structured Logging ─── */
    FILE*       log_file;           /**< Structured log output (NULL = disabled)*/
    char        log_path[CPISI_PATH_MAX]; /**< Path to log file               */
    bool        structured;         /**< Enable structured logging              */

    /* ─── Log Rotation ─── */
    LogRotationState rotation;      /**< Rotation state and config              */
    bool        use_rotation;       /**< Use rotation system vs single file     */

    /* ─── Cognitive State ─── */
    HebrewState current_state;      /**< Current Hebrew state (Base5)           */
    KFactor     current_k;          /**< Direction of movement                  */

    /* ─── Multi-scale Scoring ─── */
    HealthScore health;             /**< Base100: -100 to +100 (canonical)      */
    MultiScore  score;              /**< All decimal bases                      */
    TernaryScore ternary;           /**< All ternary bases                      */

    /* ─── Mental Construct ─── */
    int32_t     cube_pos[3];        /**< [x, y, z]: each -1 to +1               */

    /* ─── Tripwire Tracking ─── */
    int32_t     tripwire_count;     /**< Number of fallback events              */
    const char* active_backend;     /**< Current display backend name           */

    /* ─── Session Tracking ─── */
    char        session_id[32];     /**< Unique session ID: YYYYMMDD-HHMMSS-PID-SEQ */
    uint64_t    session_start;      /**< Timestamp when session began (epoch ms) */
    uint32_t    log_count;          /**< Number of log entries this session     */
    pid_t       spawn_pid;          /**< Process ID at spawn                    */

    /* ─── Session Database Integration ─── */
    void*       session_db;         /**< SessionDB* for cross-session tracking  */
    uint64_t    last_sample_ms;     /**< Last health sample timestamp           */
    bool        db_enabled;         /**< Session database integration enabled   */
} LogContext;

/**
 * @brief Alias for DAR naming consistency (DETECT phase context).
 *
 * WHY: In DAR terminology, each phase has its own context type.
 *      DetectContext = LogContext. AssessContext and RestoreContext are separate.
 */
typedef LogContext DetectContext;

/**
 * @brief Query filter for searching log entries.
 *
 * WHY: Enables structured queries across the witness data. Find patterns
 *      like "all ERROR logs when system was in chaser state with k=-1".
 *
 * USAGE:
 *   LogQueryFilter filter = {0};
 *   filter.min_level = LOG_ERROR;
 *   filter.state_mask = (1 << HEBREW_CHASER);
 *   filter.k_factor = K_RETREAT;
 *   cpisi_log_query(ctx, &filter, results, &count);
 */
typedef struct LogQueryFilter {
    /* ─── Level Filtering ─── */
    LogLevel    min_level;          /**< Minimum level (inclusive)              */
    LogLevel    max_level;          /**< Maximum level (inclusive)              */

    /* ─── State Filtering ─── */
    uint8_t     state_mask;         /**< Bitmask of HebrewState values          */
    KFactor     k_factor;           /**< K_ANY, K_RETREAT, K_HOLD, K_ADVANCE    */

    /* ─── Time Filtering ─── */
    uint64_t    start_time;         /**< Start of time window (0 = no limit)    */
    uint64_t    end_time;           /**< End of time window (0 = no limit)      */

    /* ─── Component Filtering ─── */
    const char* component;          /**< Component name (NULL = all)            */

    /* ─── Cube Position Filtering ─── */
    int32_t     cube_x;             /**< -2 = any, -1/0/+1 = specific            */
    int32_t     cube_y;             /**< -2 = any, -1/0/+1 = specific            */
    int32_t     cube_z;             /**< -2 = any, -1/0/+1 = specific            */
} LogQueryFilter;

/**
 * @brief Aggregation result for log statistics.
 *
 * WHY: Enables ASSESS phase to evaluate patterns. "We spent 80% of time in
 *      negative states. Tripwire count increased 3x. Health trending down."
 *
 * METRICS:
 *   - Time-in-state distribution
 *   - State transition frequencies
 *   - Level distribution
 *   - Health score trends
 */
typedef struct LogAggregation {
    /* ─── Time Period ─── */
    uint64_t    start_time;         /**< Aggregation window start               */
    uint64_t    end_time;           /**< Aggregation window end                 */
    uint32_t    total_entries;      /**< Total log entries in window            */

    /* ─── Level Distribution ─── */
    uint32_t    level_counts[9];    /**< Count per LogLevel (CRISIS..INSIGHT)   */

    /* ─── State Distribution ─── */
    uint32_t    state_counts[7];    /**< Count per HebrewState                  */
    uint64_t    state_duration[7];  /**< Milliseconds in each state             */

    /* ─── State Transitions ─── */
    uint32_t    transitions_up;     /**< Transitions toward tov (+1 direction)  */
    uint32_t    transitions_down;   /**< Transitions toward shavar (-1 direction)*/
    uint32_t    transitions_hold;   /**< No state change                        */

    /* ─── Health Trends ─── */
    HealthScore health_min;         /**< Minimum health in window               */
    HealthScore health_max;         /**< Maximum health in window               */
    HealthScore health_avg;         /**< Average health in window               */
    int16_t     health_delta;       /**< Net health change (end - start)        */

    /* ─── Tripwire Summary ─── */
    uint32_t    tripwire_count;     /**< Number of fallback events              */
    const char* last_tripwire_from; /**< Last tripwire source                   */
    const char* last_tripwire_to;   /**< Last tripwire destination              */
} LogAggregation;

/**
 * @brief A health log entry for the .health-log file format.
 *
 * WHY: The health log tracks atomic health changes (+1/-1) separately from
 *      debug logging. This is the data that ASSESS reads to calculate true
 *      score and identify hotspots.
 *
 * FORMAT: TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL (pipe-delimited, one per line)
 *
 * @see log.toml for specification
 * @see ActionType in cpisi/types/config/log.h
 */
typedef struct HealthLogEntry {
    uint64_t    timestamp_ms;   /**< When the action occurred                */
    ActionType  action;         /**< success/failure/neutral/recovery/reset  */
    int16_t     delta;          /**< Health change: +1 (narrow), -N (broad)  */
    char        source[CPISI_SOURCE_MAX];   /**< What triggered it           */
    char        detail[CPISI_DETAIL_MAX];   /**< Optional context            */
} HealthLogEntry;

/**
 * @brief Configuration for health log subsystem.
 *
 * WHY: Separates health action logging from debug logging. The health log
 *      is append-only and feeds the ASSESS phase for score calculation.
 */
typedef struct HealthLogConfig {
    char        log_path[CPISI_PATH_MAX];   /**< Path to .health-log file    */
    FILE*       log_file;                    /**< Open file handle            */
    bool        enabled;                     /**< Is health logging active?   */
    uint32_t    entry_count;                 /**< Entries written this session*/
    int32_t     running_delta;               /**< Running sum of deltas       */
} HealthLogConfig;

/** @} */ /* end S.3b STRUCTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3c OPAQUE — Forward-Declared Types [OPAQUE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_opaque S.3c OPAQUE — Forward-Declared Types
 * @ingroup  cpisi_detect_types
 * @brief    Opaque types to avoid circular includes.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Forward declaration for DAR visual panel.
 *
 * WHY: DETECT can output to a visual panel (CLI/TUI/GUI) but the panel
 *      implementation lives in a separate header. Forward declaration
 *      avoids circular dependency.
 */
struct DARPanel;
typedef struct DARPanel DARPanel;

/** @} */ /* end S.3c OPAQUE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d CALLBACKS — Health Event Notifications [CALLBACKS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_callbacks S.3d CALLBACKS — Health Event Notifications
 * @ingroup  cpisi_detect_types
 * @brief    Callback types for connecting detect layer to orchestrator health.
 *
 * WHY: The detect layer (logging) witnesses all events but shouldn't directly
 *      depend on the orchestrator (avoids circular dependency). Instead, the
 *      orchestrator registers a callback to receive health events.
 *
 * FLOW: LOG_ERROR() → cpisi_log_v() → callback → cpisi_orch_record_error()
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Health event callback signature for orchestrator notification.
 *
 * Called from cpisi_log_v() for each log event, allowing the orchestrator
 * to track health based on log level using ternary-derived severity values.
 *
 * @param level     Log level (LOG_CRISIS=-4 to LOG_INSIGHT=+4).
 * @param component Component that generated the log (e.g., "display", "health").
 * @param user_data Opaque pointer passed during registration (typically CpisiOrchContext*).
 *
 * LOG LEVEL → SEVERITY → HEALTH IMPACT (ternary-derived):
 *   LOG_CRISIS  (-4) → SEV_FATAL    → -100 (shavar)
 *   LOG_FATAL   (-3) → SEV_HIGH     → -75
 *   LOG_ERROR   (-2) → SEV_MEDIUM   → -50  (chaser)
 *   LOG_WARN    (-1) → SEV_LOW      → -25
 *   LOG_INFO    ( 0) → SEV_NONE     →   0  (yashar, no change)
 *   LOG_NOTICE  (+1) → SEV_FAIR     → +25
 *   LOG_DEBUG   (+2) → SEV_GOOD     → +50  (shalem)
 *   LOG_TRACE   (+3) → SEV_EXCELLENT→ +75
 *   LOG_INSIGHT (+4) → SEV_PERFECT  → +100 (tov)
 */
typedef void (*DetectHealthCallback)(LogLevel level, const char* component, void* user_data);

/** @} */ /* end S.3d CALLBACKS */

/* S.3e ERRORS — [inherited: error codes from phase.h, config.h] */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_prototypes S.4 PROTOTYPES — Function Declarations
 * @brief    Lifecycle, accessors, and operations for DETECT phase.
 *
 * STRUCTURE:
 *   - S.4a LIFECYCLE  — init, shutdown, clone
 *   - S.4b ACCESSORS  — get/set state, config, health
 *   - S.4c OPERATIONS — logging, queries, aggregation
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Create/Init/Destroy [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_lifecycle S.4a LIFECYCLE — Create/Init/Destroy
 * @ingroup  cpisi_detect_prototypes
 * @brief    LogContext lifecycle management.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize a LogContext with default values.
 *
 * @param ctx  Pointer to LogContext to initialize.
 *
 * DEFAULTS:
 *   - min_level = LOG_INFO
 *   - output = stderr
 *   - include_state = false
 *   - colors = true (if terminal supports)
 *   - current_state = HEBREW_YASHAR
 *   - health = 0 (centered)
 */
void cpisi_log_init(LogContext* ctx);

/**
 * @brief Shutdown a LogContext, flushing and closing files.
 *
 * @param ctx  Pointer to LogContext to shutdown.
 *
 * ACTIONS:
 *   - Flushes any buffered output
 *   - Closes structured log file if open
 *   - Closes rotation files if active
 *   - Clears panel connection
 */
void cpisi_log_shutdown(LogContext* ctx);

/**
 * @brief Enable session database integration.
 *
 * @param ctx      LogContext to configure.
 * @param db_path  Path to session database (NULL = default "data/sessions.db").
 *
 * WHY: Enables cross-session tracking, pattern recognition, and analytics.
 *      The session database stores:
 *        - Session start/end times
 *        - Health trajectories
 *        - Significant events (tripwires, state changes)
 *        - Aggregate statistics
 *
 * @return true on success, false if database could not be opened.
 */
bool cpisi_log_enable_session_db(LogContext* ctx, const char* db_path);

/**
 * @brief Record a significant event to session database.
 *
 * @param ctx        LogContext (must have db_enabled = true).
 * @param event_type Event type (from SessionEventType).
 * @param component  Source component name.
 * @param details    JSON details (optional, can be NULL).
 *
 * WHY: Records significant events for cross-session pattern recognition.
 *      ASSESS phase can analyze these to learn from history.
 */
void cpisi_log_record_event(LogContext* ctx, int event_type,
                            const char* component, const char* details);

/**
 * @brief Record a health sample to session database.
 *
 * @param ctx  LogContext (must have db_enabled = true).
 *
 * WHY: Captures health trajectory for trend analysis. Called periodically
 *      (default: every 10 seconds) to track health over time.
 *
 * NOTE: This is called automatically by cpisi_log() when enough time
 *       has passed since the last sample. Manual calls are allowed.
 */
void cpisi_log_record_sample(LogContext* ctx);

/**
 * @brief Get session ID for current session.
 *
 * @param ctx  LogContext.
 *
 * @return Session ID string (format: YYYYMMDD-HHMMSS-PID-SEQ), or NULL if ctx is NULL.
 */
const char* cpisi_log_get_session_id(const LogContext* ctx);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ACCESSORS — Get/Set/Is Queries [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_accessors S.4b ACCESSORS — Get/Set/Is Queries
 * @ingroup  cpisi_detect_prototypes
 * @brief    Configuration, state, and health accessors.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.4b.1 Configuration Setters */
/** @{ */

/**
 * @brief Set minimum log level.
 *
 * @param ctx    LogContext to configure.
 * @param level  Minimum level to log (levels below this are filtered).
 */
void cpisi_log_set_level(LogContext* ctx, LogLevel level);

/**
 * @brief Set output stream.
 *
 * @param ctx     LogContext to configure.
 * @param output  FILE* to write logs to (default: stderr).
 */
void cpisi_log_set_output(LogContext* ctx, FILE* output);

/**
 * @brief Enable/disable Hebrew state in output.
 *
 * @param ctx     LogContext to configure.
 * @param enable  true to include state, false to omit.
 */
void cpisi_log_enable_state(LogContext* ctx, bool enable);

/**
 * @brief Enable/disable ANSI colors in output.
 *
 * @param ctx     LogContext to configure.
 * @param enable  true for colors, false for plain text.
 */
void cpisi_log_enable_colors(LogContext* ctx, bool enable);

/**
 * @brief Enable structured logging to JSONL file.
 *
 * @param ctx   LogContext to configure.
 * @param path  Path to log file (e.g., "data/session/logs/2026-01-16.jsonl").
 * @return      0 on success, -1 on failure.
 *
 * WHY: Structured logs enable queries and aggregation. Each line is a JSON
 *      object with full CPI-SI context.
 */
int cpisi_log_enable_structured(LogContext* ctx, const char* path);

/**
 * @brief Disable structured logging.
 *
 * @param ctx  LogContext to configure.
 */
void cpisi_log_disable_structured(LogContext* ctx);

/**
 * @brief Enable log rotation with dated folders.
 *
 * @param ctx     LogContext to configure.
 * @param config  Rotation configuration (base path, archive days, retention).
 * @return        0 on success, -1 on failure.
 *
 * WHY: Prevents unbounded log growth. Organizes logs by date. Supports
 *      archival and retention policies.
 */
int cpisi_log_enable_rotation(LogContext* ctx, const LogRotationConfig* config);

/**
 * @brief Disable log rotation.
 *
 * @param ctx  LogContext to configure.
 */
void cpisi_log_disable_rotation(LogContext* ctx);

/**
 * @brief Connect log output to DAR visual panel.
 *
 * @param panel       DARPanel to send logs to.
 * @param panel_only  true = suppress stderr (for TUI/GUI), false = both.
 */
void cpisi_log_set_panel(DARPanel* panel, bool panel_only);

/**
 * @brief Disconnect from DAR visual panel.
 */
void cpisi_log_clear_panel(void);

/**
 * @brief Register health event callback for orchestrator notification.
 *
 * The callback is invoked from cpisi_log_v() for each log event, allowing
 * the orchestrator to track cumulative health using ternary-derived impacts.
 *
 * @param callback   Function to call on each log event (NULL to disable).
 * @param user_data  Opaque pointer passed to callback (typically CpisiOrchContext*).
 *
 * @note Only one callback can be registered at a time; subsequent calls replace.
 *
 * TYPICAL USAGE (in cpisi.c):
 * @code
 *   cpisi_log_set_health_callback(on_log_health_event, ctx);
 * @endcode
 */
void cpisi_log_set_health_callback(DetectHealthCallback callback, void* user_data);

/**
 * @brief Clear the health event callback.
 */
void cpisi_log_clear_health_callback(void);

/** @} */ /* end S.4b.1 Configuration Setters */

/** @name S.4b.2 State Setters */
/** @{ */

/**
 * @brief Set current Hebrew state.
 *
 * @param ctx    LogContext to update.
 * @param state  New Hebrew state (SHAVAR..TOV).
 */
void cpisi_log_set_hebrew_state(LogContext* ctx, HebrewState state);

/**
 * @brief Set current k-factor (direction).
 *
 * @param ctx  LogContext to update.
 * @param k    New k-factor (K_RETREAT, K_HOLD, K_ADVANCE).
 */
void cpisi_log_set_k_factor(LogContext* ctx, KFactor k);

/**
 * @brief Set cube position in mental construct.
 *
 * @param ctx  LogContext to update.
 * @param x    X position: -1 (void), 0 (word), +1 (tov).
 * @param y    Y position: -1 to +1.
 * @param z    Z position: -1 to +1.
 */
void cpisi_log_set_cube_position(LogContext* ctx, int32_t x, int32_t y, int32_t z);

/**
 * @brief Record a tripwire event (fallback occurred).
 *
 * @param ctx   LogContext to update.
 * @param from  Source that failed (e.g., "GUI").
 * @param to    Fallback destination (e.g., "TUI").
 *
 * EFFECTS:
 *   - Increments tripwire_count
 *   - Logs the fallback at LOG_WARN level
 *   - Decreases health by 10 points
 */
void cpisi_log_tripwire(LogContext* ctx, const char* from, const char* to);

/**
 * @brief Set health score (automatically updates Hebrew state and all bases).
 *
 * @param ctx     LogContext to update.
 * @param health  New health score (-100 to +100).
 *
 * EFFECTS:
 *   - Updates ctx->health
 *   - Recalculates ctx->score (all decimal bases)
 *   - Recalculates ctx->ternary (all ternary bases)
 *   - Updates ctx->current_state based on health
 */
void cpisi_log_set_health(LogContext* ctx, HealthScore health);

/**
 * @brief Adjust health score by delta.
 *
 * @param ctx    LogContext to update.
 * @param delta  Amount to add (negative to subtract), clamped to [-100, +100].
 */
void cpisi_log_adjust_health(LogContext* ctx, int16_t delta);

/** @} */ /* end S.4b.2 State Setters */

/** @name S.4b.3 State Getters */
/** @{ */

/**
 * @brief Get current health score.
 *
 * @param ctx  LogContext to query.
 * @return     Health score (-100 to +100).
 */
HealthScore cpisi_log_get_health(const LogContext* ctx);

/**
 * @brief Get Base50 value from context.
 * @param ctx  LogContext to query.
 * @return     Base50 value (-50 to +50).
 */
int8_t cpisi_log_get_base50(const LogContext* ctx);

/**
 * @brief Get Base20 value from context.
 * @param ctx  LogContext to query.
 * @return     Base20 value (-20 to +20).
 */
int8_t cpisi_log_get_base20(const LogContext* ctx);

/**
 * @brief Get Base10 value from context.
 * @param ctx  LogContext to query.
 * @return     Base10 value (-10 to +10).
 */
int8_t cpisi_log_get_base10(const LogContext* ctx);

/**
 * @brief Get Base5 value from context (Hebrew state as number).
 * @param ctx  LogContext to query.
 * @return     Base5 value (-2 to +2).
 */
int8_t cpisi_log_get_base5(const LogContext* ctx);

/**
 * @brief Get Base3 value from context (raw trit sign).
 * @param ctx  LogContext to query.
 * @return     Base3 value (-1, 0, +1).
 */
int8_t cpisi_log_get_base3(const LogContext* ctx);

/** @} */ /* end S.4b.3 State Getters */

/** @name S.4b.4 Conversion Functions */
/** @{ */

/**
 * @brief Convert Base100 health to MultiScore (all decimal bases).
 * @param base100  Health score (-100 to +100).
 * @return         MultiScore with all decimal bases populated.
 */
MultiScore cpisi_score_from_base100(HealthScore base100);

/**
 * @brief Convert Base100 health to TernaryScore (all ternary bases).
 * @param base100  Health score (-100 to +100).
 * @return         TernaryScore with all ternary bases populated.
 */
TernaryScore cpisi_ternary_from_base100(HealthScore base100);

/**
 * @brief Convert Base5 to Base100 health.
 * @param base5  Base5 value (-2 to +2).
 * @return       Approximate health score.
 */
HealthScore cpisi_base100_from_base5(int8_t base5);

/**
 * @brief Convert health score to Hebrew state.
 * @param health  Health score (-100 to +100).
 * @return        Corresponding Hebrew state.
 */
HebrewState cpisi_health_to_state(HealthScore health);

/** @} */ /* end S.4b.4 Conversion Functions */

/** @name S.4b.5 Name Lookups */
/** @{ */

/**
 * @brief Get padded level name for aligned output.
 * @param level  LogLevel to look up.
 * @return       7-character padded name (e.g., "ERROR  ").
 */
const char* cpisi_log_level_name(LogLevel level);

/**
 * @brief Get ANSI color code for level.
 * @param level  LogLevel to look up.
 * @return       ANSI escape sequence (e.g., "\033[31m" for red).
 */
const char* cpisi_log_level_color(LogLevel level);

/**
 * @brief Get k-factor name with arrow.
 * @param k  KFactor to look up.
 * @return   Name with arrow (e.g., "↓retreat", "→hold", "↑advance").
 */
const char* cpisi_k_factor_name(KFactor k);

/** @} */ /* end S.4b.5 Name Lookups */

/** @} */ /* end S.4b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c OPERATIONS — Logging, Queries, Aggregation [OPERATIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_operations S.4c OPERATIONS — Logging, Queries, Aggregation
 * @ingroup  cpisi_detect_prototypes
 * @brief    Core logging functions and DAR Witness System APIs.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.4c.1 Core Logging Functions */
/** @{ */

/**
 * @brief Log a message with full CPI-SI context.
 *
 * @param ctx        LogContext to use.
 * @param level      Log level (LOG_CRISIS..LOG_INSIGHT).
 * @param component  Component name for filtering (e.g., "display", "health").
 * @param fmt        printf-style format string.
 * @param ...        Format arguments.
 *
 * CONTEXT CAPTURED:
 *   - timestamp_ms from current time
 *   - hebrew_state, k_factor from ctx
 *   - score, ternary from ctx
 *   - cube position from ctx
 */
void cpisi_log(LogContext* ctx, LogLevel level, const char* component,
               const char* fmt, ...);

/**
 * @brief Log a message with va_list (for wrapper functions).
 *
 * @param ctx        LogContext to use.
 * @param level      Log level.
 * @param component  Component name.
 * @param fmt        printf-style format string.
 * @param args       va_list of format arguments.
 */
void cpisi_log_v(LogContext* ctx, LogLevel level, const char* component,
                 const char* fmt, va_list args);

/** @} */ /* end S.4c.1 Core Logging Functions */

/** @name S.4c.2 Structured Query API */
/** @{ */

/**
 * @brief Query log entries matching filter criteria.
 *
 * @param ctx         LogContext with structured logging enabled.
 * @param filter      Query filter (NULL = all entries).
 * @param results     Array to store matching LogEntry pointers.
 * @param max_results Maximum entries to return.
 * @param count       [out] Actual number of entries returned.
 * @return            0 on success, -1 on error.
 *
 * WHY: Enables pattern detection. "Show me all ERROR logs when we were in
 *      chaser state" → filter by level + state_mask.
 *
 * EXAMPLE:
 *   LogQueryFilter filter = {0};
 *   filter.min_level = LOG_ERROR;
 *   filter.max_level = LOG_CRISIS;
 *   filter.state_mask = (1 << HEBREW_CHASER) | (1 << HEBREW_SHAVAR);
 *   cpisi_log_query(ctx, &filter, results, 100, &count);
 */
int cpisi_log_query(const LogContext* ctx, const LogQueryFilter* filter,
                    LogEntry** results, size_t max_results, size_t* count);

/**
 * @brief Query log entries by Hebrew state.
 *
 * @param ctx         LogContext with structured logging enabled.
 * @param state       Hebrew state to filter by.
 * @param results     Array to store matching LogEntry pointers.
 * @param max_results Maximum entries to return.
 * @param count       [out] Actual number of entries returned.
 * @return            0 on success, -1 on error.
 */
int cpisi_log_query_by_state(const LogContext* ctx, HebrewState state,
                             LogEntry** results, size_t max_results, size_t* count);

/**
 * @brief Query log entries by time range.
 *
 * @param ctx         LogContext with structured logging enabled.
 * @param start_time  Start of time window (ms since epoch).
 * @param end_time    End of time window (ms since epoch).
 * @param results     Array to store matching LogEntry pointers.
 * @param max_results Maximum entries to return.
 * @param count       [out] Actual number of entries returned.
 * @return            0 on success, -1 on error.
 */
int cpisi_log_query_by_time(const LogContext* ctx, uint64_t start_time,
                            uint64_t end_time, LogEntry** results,
                            size_t max_results, size_t* count);

/**
 * @brief Query log entries by component.
 *
 * @param ctx         LogContext with structured logging enabled.
 * @param component   Component name to filter by.
 * @param results     Array to store matching LogEntry pointers.
 * @param max_results Maximum entries to return.
 * @param count       [out] Actual number of entries returned.
 * @return            0 on success, -1 on error.
 */
int cpisi_log_query_by_component(const LogContext* ctx, const char* component,
                                 LogEntry** results, size_t max_results,
                                 size_t* count);

/** @} */ /* end S.4c.2 Structured Query API */

/** @name S.4c.3 Aggregation API */
/** @{ */

/**
 * @brief Aggregate log statistics for a time window.
 *
 * @param ctx         LogContext with structured logging enabled.
 * @param start_time  Start of aggregation window (0 = from session start).
 * @param end_time    End of aggregation window (0 = now).
 * @param result      [out] Aggregation results.
 * @return            0 on success, -1 on error.
 *
 * WHY: Enables ASSESS phase to evaluate patterns. "We spent 80% of time in
 *      negative states. Tripwire count increased 3x. Health trending down."
 *
 * EXAMPLE:
 *   LogAggregation agg;
 *   cpisi_log_aggregate(ctx, 0, 0, &agg);  // Entire session
 *   printf("Time in shavar: %lu ms\n", agg.state_duration[HEBREW_SHAVAR]);
 *   printf("Health trend: %+d\n", agg.health_delta);
 */
int cpisi_log_aggregate(const LogContext* ctx, uint64_t start_time,
                        uint64_t end_time, LogAggregation* result);

/**
 * @brief Aggregate log statistics for current session.
 *
 * @param ctx     LogContext with structured logging enabled.
 * @param result  [out] Aggregation results.
 * @return        0 on success, -1 on error.
 */
int cpisi_log_aggregate_session(const LogContext* ctx, LogAggregation* result);

/**
 * @brief Get state transition summary.
 *
 * @param ctx              LogContext with structured logging enabled.
 * @param transitions_up   [out] Count of transitions toward tov.
 * @param transitions_down [out] Count of transitions toward shavar.
 * @param transitions_hold [out] Count of no-change entries.
 * @return                 0 on success, -1 on error.
 */
int cpisi_log_get_transitions(const LogContext* ctx,
                              uint32_t* transitions_up,
                              uint32_t* transitions_down,
                              uint32_t* transitions_hold);

/**
 * @brief Get health trend over time window.
 *
 * @param ctx         LogContext with structured logging enabled.
 * @param start_time  Start of window.
 * @param end_time    End of window.
 * @param min         [out] Minimum health in window.
 * @param max         [out] Maximum health in window.
 * @param avg         [out] Average health in window.
 * @param delta       [out] Net change (end - start).
 * @return            0 on success, -1 on error.
 */
int cpisi_log_get_health_trend(const LogContext* ctx, uint64_t start_time,
                               uint64_t end_time, HealthScore* min,
                               HealthScore* max, HealthScore* avg,
                               int16_t* delta);

/** @} */ /* end S.4c.3 Aggregation API */

/** @name S.4c.4 Global Context */
/** @{ */

/**
 * @brief Global log context for convenience macros.
 *
 * WHY: Enables LOG_INFO("comp", "msg") without passing context.
 *      Must be initialized before use.
 */
extern LogContext* g_cpisi_log;

/** @} */ /* end S.4c.4 Global Context */

/** @name S.4c.5 Convenience Macros — 9-Level Spectrum */
/** @{ */

/* ─── Negative Territory (shavar → chaser) ─── */

/** @brief Log at CRISIS level (-1.0, shavar). */
#define LOG_CRISIS(comp, ...) \
    cpisi_log(g_cpisi_log, LOG_CRISIS, comp, __VA_ARGS__)

/** @brief Log at FATAL level (-0.75). */
#define LOG_FATAL(comp, ...) \
    cpisi_log(g_cpisi_log, LOG_FATAL, comp, __VA_ARGS__)

/** @brief Log at ERROR level (-0.50, chaser). */
#define LOG_ERROR(comp, ...) \
    cpisi_log(g_cpisi_log, LOG_ERROR, comp, __VA_ARGS__)

/** @brief Log at WARN level (-0.25). */
#define LOG_WARN(comp, ...) \
    cpisi_log(g_cpisi_log, LOG_WARN, comp, __VA_ARGS__)

/* ─── Center (yashar) ─── */

/** @brief Log at INFO level (0.0, yashar). */
#define LOG_INFO(comp, ...) \
    cpisi_log(g_cpisi_log, LOG_INFO, comp, __VA_ARGS__)

/* ─── Positive Territory (shalem → tov) ─── */

/** @brief Log at NOTICE level (+0.25). */
#define LOG_NOTICE(comp, ...) \
    cpisi_log(g_cpisi_log, LOG_NOTICE, comp, __VA_ARGS__)

/** @brief Log at DEBUG level (+0.50, shalem). */
#define LOG_DEBUG(comp, ...) \
    cpisi_log(g_cpisi_log, LOG_DEBUG, comp, __VA_ARGS__)

/** @brief Log at TRACE level (+0.75). */
#define LOG_TRACE(comp, ...) \
    cpisi_log(g_cpisi_log, LOG_TRACE, comp, __VA_ARGS__)

/** @brief Log at INSIGHT level (+1.0, tov). */
#define LOG_INSIGHT(comp, ...) \
    cpisi_log(g_cpisi_log, LOG_INSIGHT, comp, __VA_ARGS__)

/** @} */ /* end S.4c.5 Convenience Macros */

/** @name S.4c.6 VA_LIST Variants */
/** @{ */

/** @brief Log at CRISIS level with va_list. */
#define LOG_CRISIS_VA(comp, fmt, ap) \
    cpisi_log_v(g_cpisi_log, LOG_CRISIS, comp, fmt, ap)

/** @brief Log at FATAL level with va_list. */
#define LOG_FATAL_VA(comp, fmt, ap) \
    cpisi_log_v(g_cpisi_log, LOG_FATAL, comp, fmt, ap)

/** @brief Log at ERROR level with va_list. */
#define LOG_ERROR_VA(comp, fmt, ap) \
    cpisi_log_v(g_cpisi_log, LOG_ERROR, comp, fmt, ap)

/** @brief Log at WARN level with va_list. */
#define LOG_WARN_VA(comp, fmt, ap) \
    cpisi_log_v(g_cpisi_log, LOG_WARN, comp, fmt, ap)

/** @brief Log at INFO level with va_list. */
#define LOG_INFO_VA(comp, fmt, ap) \
    cpisi_log_v(g_cpisi_log, LOG_INFO, comp, fmt, ap)

/** @brief Log at NOTICE level with va_list. */
#define LOG_NOTICE_VA(comp, fmt, ap) \
    cpisi_log_v(g_cpisi_log, LOG_NOTICE, comp, fmt, ap)

/** @brief Log at DEBUG level with va_list. */
#define LOG_DEBUG_VA(comp, fmt, ap) \
    cpisi_log_v(g_cpisi_log, LOG_DEBUG, comp, fmt, ap)

/** @brief Log at TRACE level with va_list. */
#define LOG_TRACE_VA(comp, fmt, ap) \
    cpisi_log_v(g_cpisi_log, LOG_TRACE, comp, fmt, ap)

/** @brief Log at INSIGHT level with va_list. */
#define LOG_INSIGHT_VA(comp, fmt, ap) \
    cpisi_log_v(g_cpisi_log, LOG_INSIGHT, comp, fmt, ap)

/** @} */ /* end S.4c.6 VA_LIST Variants */

/** @name S.4c.7 State Transition Macros */
/** @{ */

/** @brief Log Hebrew state transition. */
#define LOG_STATE_CHANGE(from, to) \
    cpisi_log(g_cpisi_log, LOG_INFO, "state", \
              "Hebrew state: %s → %s", \
              cpisi_hebrew_state_name(from), \
              cpisi_hebrew_state_name(to))

/** @brief Log k-factor direction change. */
#define LOG_K_CHANGE(from, to) \
    cpisi_log(g_cpisi_log, LOG_INFO, "k-factor", \
              "Direction: %s → %s", \
              cpisi_k_factor_name(from), \
              cpisi_k_factor_name(to))

/** @brief Log tripwire fallback event. */
#define LOG_TRIPWIRE(from, to) \
    cpisi_log_tripwire(g_cpisi_log, from, to)

/** @} */ /* end S.4c.7 State Transition Macros */

/** @} */ /* end S.4c OPERATIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4d HEALTH LOG — Health Action Logging [HEALTHLOG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_detect_healthlog S.4d HEALTH LOG — Health Action Logging
 * @ingroup  cpisi_detect_prototypes
 * @brief    Append-only health log for DETECT → ASSESS workflow.
 *
 * WHY: Separate from debug logging. Health log tracks atomic health changes
 *      (+1 success, -N failure) that feed into ASSESS for true score calculation.
 *
 * FORMAT: TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL (one per line, append-only)
 *
 * WORKFLOW:
 *   DETECT: cpisi_health_log_append() → writes to .health-log
 *   ASSESS: cpisi_health_log_calculate_true_score() → reads .health-log
 *   RESTORE: triggered when true score falls below threshold
 *
 * @see log.toml for specification
 * @see ActionType in cpisi/types/config/log.h
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize health log subsystem.
 *
 * @param config  Health log configuration to initialize.
 * @param log_dir Directory containing .health-log (NULL = current directory).
 * @return        0 on success, -1 on failure.
 *
 * Opens (or creates) the .health-log file for append-only writing.
 */
int cpisi_health_log_init(HealthLogConfig* config, const char* log_dir);

/**
 * @brief Shutdown health log subsystem.
 *
 * @param config  Health log configuration to shutdown.
 *
 * Flushes and closes the .health-log file.
 */
void cpisi_health_log_shutdown(HealthLogConfig* config);

/**
 * @brief Append an entry to the health log.
 *
 * @param config  Health log configuration (must be initialized).
 * @param action  Action type (success/failure/neutral/recovery/reset).
 * @param delta   Health change value (+1 for success, -N for failure).
 * @param source  What triggered the action (e.g., "test_pass", "build_fail").
 * @param detail  Optional additional context (file path, error message).
 * @return        0 on success, -1 on failure.
 *
 * ATOMIC: Each successful action is +1 (narrow way). Each failure is -1
 *         or more severe (broad way). Reset clears accumulated score.
 *
 * FORMAT: TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL
 *
 * EXAMPLE:
 *   2026-01-25T13:30:00Z|success|+1|test_pass|tests/unit/foo_test.c
 *   2026-01-25T13:31:00Z|failure|-3|build_fail|src/main.c:45 undefined var
 */
int cpisi_health_log_append(HealthLogConfig* config, ActionType action,
                            int16_t delta, const char* source, const char* detail);

/**
 * @brief Calculate true score from health log.
 *
 * @param log_path  Path to .health-log file (or directory containing it).
 * @return          True score (sum of all deltas), or 0 if file not found.
 *
 * TRUE SCORE: Unbounded sum of all delta values. Can be +500, -300, etc.
 *             Reset actions clear the accumulated sum.
 *
 * ALGORITHM:
 *   true_score = 0
 *   for each entry in .health-log:
 *     if action == reset:
 *       true_score = 0
 *     else:
 *       true_score += delta
 *   return true_score
 *
 * @see diagnostics.toml for normalization to bounded scales
 */
int32_t cpisi_health_log_calculate_true_score(const char* log_path);

/**
 * @brief Get breakdown of health log by source.
 *
 * @param log_path     Path to .health-log file.
 * @param sources      [out] Array of source names (caller allocates).
 * @param deltas       [out] Array of delta sums per source.
 * @param max_sources  Maximum number of sources to return.
 * @param count        [out] Actual number of sources found.
 * @return             0 on success, -1 on failure.
 *
 * HOTSPOT DETECTION: Sources with largest negative deltas are hotspots.
 *                    Sort by delta ascending to find worst offenders.
 */
int cpisi_health_log_breakdown(const char* log_path,
                               char sources[][CPISI_SOURCE_MAX],
                               int32_t* deltas, size_t max_sources, size_t* count);

/**
 * @brief Convenience macro for logging success (+1).
 *
 * @param cfg     HealthLogConfig pointer.
 * @param src     Source string (e.g., "test_pass").
 * @param detail  Optional detail string.
 */
#define HEALTH_LOG_SUCCESS(cfg, src, detail) \
    cpisi_health_log_append((cfg), ACTION_SUCCESS, +1, (src), (detail))

/**
 * @brief Convenience macro for logging failure (-1).
 *
 * @param cfg     HealthLogConfig pointer.
 * @param src     Source string (e.g., "test_fail").
 * @param detail  Optional detail string.
 */
#define HEALTH_LOG_FAILURE(cfg, src, detail) \
    cpisi_health_log_append((cfg), ACTION_FAILURE, -1, (src), (detail))

/**
 * @brief Convenience macro for logging recovery (+1 with recovery marker).
 *
 * @param cfg     HealthLogConfig pointer.
 * @param src     Source string (e.g., "manual_fix").
 * @param detail  Optional detail string.
 */
#define HEALTH_LOG_RECOVERY(cfg, src, detail) \
    cpisi_health_log_append((cfg), ACTION_RECOVERY, +1, (src), (detail))

/** @} */ /* end S.4d HEALTH LOG */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4e OPERATION BATCHING — Smart Grouped Logging [OPBATCH]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_opbatch S.4e OPERATION BATCHING — Smart Grouped Logging
 * @brief    Group related operations for meaningful log output.
 *
 * PROBLEM:
 *   Individual state transitions create noise:
 *     state: shalem → yashar
 *     state: shalem → yashar
 *     state: shalem → yashar
 *
 * SOLUTION:
 *   Batch operations and log summaries:
 *     [Init] Engine startup: 6 ops, health +0→+50, yashar→shalem ✓
 *
 * USAGE:
 *   @code
 *   cpisi_op_begin(ctx, "engine_init");
 *   // ... multiple internal operations ...
 *   cpisi_op_success(ctx);  // counted, not logged
 *   cpisi_op_success(ctx);  // counted, not logged
 *   cpisi_op_end(ctx);      // logs summary
 *   @endcode
 *
 * BEHAVIOR:
 *   - During operation: suppress DEBUG state logs
 *   - INFO+ logs pass through immediately
 *   - Crisis (SHAVAR) always logs immediately
 *   - On end: log summary with health delta
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Maximum operation name length.
 */
#define CPISI_OP_NAME_MAX 32

/**
 * @brief Maximum operation nesting depth.
 */
#define CPISI_OP_STACK_MAX 8

/**
 * @brief Operation context for batching.
 *
 * Tracks operation lifecycle for grouped logging.
 */
typedef struct OpContext {
    char        name[CPISI_OP_NAME_MAX];  /**< Operation name (e.g., "engine_init") */
    HealthScore start_health;              /**< Health at operation start            */
    HebrewState start_state;               /**< State at operation start             */
    uint32_t    success_count;             /**< Successful sub-operations            */
    uint32_t    failure_count;             /**< Failed sub-operations                */
    uint64_t    start_time_ms;             /**< Start timestamp (ms since epoch)     */
    uint8_t     depth;                     /**< Nesting depth (0 = top level)        */
} OpContext;

/**
 * @brief Operation stack for nested operations.
 */
typedef struct OpStack {
    OpContext   ops[CPISI_OP_STACK_MAX];   /**< Operation stack                      */
    uint8_t     depth;                     /**< Current stack depth                  */
    bool        suppress_debug;            /**< Suppress DEBUG logs during operation */
} OpStack;

/** @name S.4e.1 Operation Stack Management */
/** @{ */

/**
 * @brief Initialize the operation stack.
 *
 * @param[out] stack  Stack to initialize.
 */
void cpisi_op_stack_init(OpStack* stack);

/**
 * @brief Begin a new operation (push to stack).
 *
 * @param[in]     ctx   Log context for state tracking.
 * @param[in,out] stack Operation stack.
 * @param[in]     name  Operation name (e.g., "engine_init").
 *
 * @return 0 on success, -1 if stack full.
 */
int cpisi_op_begin(LogContext* ctx, OpStack* stack, const char* name);

/**
 * @brief Record a successful sub-operation.
 *
 * Increments success counter without logging.
 *
 * @param[in,out] stack Operation stack.
 */
void cpisi_op_success(OpStack* stack);

/**
 * @brief Record a failed sub-operation.
 *
 * Increments failure counter and logs immediately.
 *
 * @param[in]     ctx   Log context.
 * @param[in,out] stack Operation stack.
 * @param[in]     msg   Failure message.
 */
void cpisi_op_failure(LogContext* ctx, OpStack* stack, const char* msg);

/**
 * @brief End the current operation (pop from stack).
 *
 * Logs operation summary with health delta.
 *
 * @param[in]     ctx   Log context for state tracking.
 * @param[in,out] stack Operation stack.
 *
 * @return 0 on success, -1 if stack empty.
 */
int cpisi_op_end(LogContext* ctx, OpStack* stack);

/**
 * @brief Check if DEBUG logs should be suppressed.
 *
 * @param[in] stack Operation stack.
 *
 * @return true if in an operation and DEBUG should be suppressed.
 */
bool cpisi_op_suppress_debug(const OpStack* stack);

/**
 * @brief Get current operation depth.
 *
 * @param[in] stack Operation stack.
 *
 * @return Current depth (0 = no active operation).
 */
uint8_t cpisi_op_depth(const OpStack* stack);

/** @} */ /* end S.4e.1 Operation Stack Management */

/** @name S.4e.2 Global Operation Stack */
/** @{ */

/**
 * @brief Global operation stack for convenience macros.
 */
extern OpStack* g_cpisi_op_stack;

/**
 * @brief Begin operation using global context and stack.
 */
#define OP_BEGIN(name) \
    cpisi_op_begin(g_cpisi_log, g_cpisi_op_stack, (name))

/**
 * @brief Record success using global stack.
 */
#define OP_SUCCESS() \
    cpisi_op_success(g_cpisi_op_stack)

/**
 * @brief Record failure using global context and stack.
 */
#define OP_FAILURE(msg) \
    cpisi_op_failure(g_cpisi_log, g_cpisi_op_stack, (msg))

/**
 * @brief End operation using global context and stack.
 */
#define OP_END() \
    cpisi_op_end(g_cpisi_log, g_cpisi_op_stack)

/** @} */ /* end S.4e.2 Global Operation Stack */

/** @} */ /* end S.4e OPERATION BATCHING */

/* S.5 FILE-LEVEL — [reserved] */

/* S.6 OMISSION — [reserved] */

/** @} */ /* end S.4 PROTOTYPES */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Implementation notes and design documentation.
///
/// NOTE: This is a HEADER file. Implementation lives in detect.c.
///       BODY contains design documentation for implementers.
///
/// STRUCTURE:
///   - B.1 ORG CHART  — Structure overview
///   - B.2 HELPERS    — [reserved: implementation in detect.c]
///   - B.3 CORE OPS   — [reserved: implementation in detect.c]
///   - B.4 ERRORS     — [reserved: implementation in detect.c]
///   - B.5 PUBLIC API — [reserved: implementation in detect.c]
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_orgchart B.1 ORG CHART — Structure Overview
 * @brief    DETECT phase architecture and design.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @name B.1a DESIGN — 9-Level Ternary Spectrum
 * @{
 *
 * Traditional logging: "ERROR is worse than DEBUG"
 * CPI-SI logging: "ERROR is negative territory (-0.50), DEBUG is positive (+0.50)"
 *
 * THE SPECTRUM (shavar → tov):
 *
 *   -1.0  -0.75  -0.50  -0.25   0.0   +0.25  +0.50  +0.75  +1.0
 *    |      |      |      |      |      |      |      |      |
 * CRISIS FATAL  ERROR  WARN   INFO  NOTICE DEBUG  TRACE INSIGHT
 * shavar  ....  chaser ....  yashar ....  shalem  ....   tov
 *
 * HEBREW STATE MAPPING:
 *   - Burst of ERROR/WARN → shifts toward chaser (lacking)
 *   - Stream of DEBUG/INSIGHT → indicates shalem (building)
 *   - Log entries carry full cognitive state for pattern analysis
 *
 * @}
 */

/**
 * @name B.1b DAR ROLE — Witness System Position
 * @{
 *
 * DETECT (this) — RECORDS what happened (evidence gathering)
 * ASSESS         — EVALUATES the evidence (diagnosis)
 * RESTORE        — ACTS on the assessment (remediation)
 *
 * The log provides WITNESS data. It doesn't judge or act—just records.
 * This separation enables clear responsibilities and testability.
 *
 * DAR FLOW:
 *   Event occurs
 *      ↓
 *   DETECT (cpisi_log) — Log with full CPI-SI context
 *      ↓
 *   ASSESS (cpisi_assess) — Evaluate: expected? normal? concerning?
 *      ↓
 *   RESTORE (cpisi_restore) — If needed, attempt recovery
 *      ↓
 *   Continue or HALT
 *
 * @}
 */

/**
 * @name B.1c OUTPUT — Dual-Format Logging
 * @{
 *
 * Line 1: Pipe-delimited (human-scannable)
 *   TIMESTAMP|LEVEL|DELTA|COMPONENT|MESSAGE
 *
 * Line 2: JSON detail (machine-parseable, optional)
 *   {"state": "yashar", "k": "→", "score": {...}, "cube": [...]}
 *
 * @}
 */

/** @} */ /* end B.1 ORG CHART */

/* B.2 HELPERS — [reserved: implementation in detect.c] */

/* B.3 CORE OPS — [reserved: implementation in detect.c] */

/* B.4 ERRORS — [reserved: implementation in detect.c] */

/* B.5 PUBLIC API — [reserved: implementation in detect.c] */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Validation, usage, cleanup, and guidance.
///
/// STRUCTURE:
///   - X.1 VALIDATION      — Build and verify
///   - X.2 EXECUTION       — Usage patterns
///   - X.3 CLEANUP         — Ownership and shutdown
///   - X.4 POLICY          — Modification guidelines
///   - X.5 EXTENSION       — How to expand
///   - X.6 TROUBLESHOOTING — Common problems
///   - X.7 REFERENCE       — Quick lookup
///   - X.8 NOTE            — Final guidance
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Build and Verify [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_validation X.1 VALIDATION — Build and Verify
 * @brief    Compilation and verification commands.
 *
 * BUILD:
 *   gcc -c detect.c -I../../.. -Wall -Wextra -std=c11
 *
 * SELF-TEST:
 *   gcc -DDETECT_SELF_TEST detect.c -o detect_test && ./detect_test
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_execution X.2 EXECUTION — Usage Patterns
 * @brief    How to use DETECT logging.
 *
 * INCLUDE:
 *   #include "kernel/dar/phase/detect.phase.h"
 *
 * USAGE:
 *   LogContext log;
 *   cpisi_log_init(&log);
 *   cpisi_log_set_level(&log, LOG_DEBUG);
 *   cpisi_log_enable_state(&log, true);
 *
 *   // Set cognitive state
 *   cpisi_log_set_hebrew_state(&log, HEBREW_YASHAR);
 *   cpisi_log_set_k_factor(&log, K_ADVANCE);
 *
 *   // Log with state context
 *   cpisi_log(&log, LOG_INFO, "display", "Initializing backend");
 *
 *   // Or use convenience macros with global context
 *   LOG_INFO("startup", "Application started");
 *   LOG_ERROR("config", "Config file missing");
 *   LOG_TRIPWIRE("GUI", "TUI");
 *
 *   cpisi_log_shutdown(&log);
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership and Shutdown [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_cleanup X.3 CLEANUP — Ownership and Shutdown
 * @brief    Memory and resource ownership.
 *
 * OWNERSHIP:
 *   - LogContext: Caller owns, caller destroys with cpisi_log_shutdown()
 *   - LogEntry: Internal to LogContext, not exposed
 *   - log_file: Owned by LogContext, closed on shutdown
 *   - g_cpisi_log: Global, must be initialized before use
 *
 * SHUTDOWN:
 *   cpisi_log_shutdown(&ctx);  // Flushes, closes files, clears state
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.3 CLEANUP */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_policy X.4 POLICY — Modification Guidelines
 * @brief    Rules for modifying this component.
 *
 * WHEN MODIFYING:
 *   1. Run validation (X.1) before and after changes
 *   2. Update version in M.2 STATE
 *   3. Add history entry in M.10 ROADMAP
 *   4. Ensure query/aggregation APIs remain backward compatible
 *
 * DO NOT:
 *   - Redefine types from log.h or phase.h (single source of truth)
 *   - Break structured log format (JSONL compatibility)
 *   - Remove query filter fields (extend only)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.4 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_extension X.5 EXTENSION — How to Expand
 * @brief    Adding new functionality.
 *
 * TO ADD NEW LOG LEVEL:
 *   1. Add to LogLevel enum in log.h
 *   2. Add to level_names[] array
 *   3. Add convenience macro (LOG_NEWLEVEL)
 *   4. Add VA variant (LOG_NEWLEVEL_VA)
 *   5. Update aggregation level_counts array size
 *
 * TO ADD NEW QUERY FILTER:
 *   1. Add field to LogQueryFilter struct
 *   2. Update cpisi_log_query() to check new field
 *   3. Document in filter struct docstring
 *
 * @see M.10 ROADMAP for planned extensions
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.5 EXTENSION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @brief    Solutions to common issues.
 *
 * PROBLEM: LogLevel undefined
 * SOLUTION: Ensure log.h is included (via detect.h includes)
 *
 * PROBLEM: CPISI_PATH_MAX undefined
 * SOLUTION: Ensure config.h is included (via log.h → config.h)
 *
 * PROBLEM: Query returns no results
 * SOLUTION: Ensure structured logging is enabled with cpisi_log_enable_structured()
 *
 * PROBLEM: Aggregation shows zero entries
 * SOLUTION: Check time window; use start_time=0, end_time=0 for entire session
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.6 TROUBLESHOOTING */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_reference X.7 REFERENCE — Quick Lookup
 * @brief    Copy-paste ready reference.
 *
 * DERIVATION CHAIN:
 *   log.h    → LogLevel, LOG_LEVEL_* constants
 *   phase.h  → DARPhase, DARAspect, DARSeverity
 *   health.h → HealthScore, HebrewState, KFactor (via phase.h)
 *   config.h → CPISI_PATH_MAX limits (via log.h)
 *
 * RELATED HEADERS:
 *   - log.h     — Log level definitions
 *   - phase.h   — DAR phase types
 *   - assess.h  — ASSESS phase (evaluates log evidence)
 *   - restore.h — RESTORE phase (acts on assessment)
 *   - dar.h     — Group header for all DAR phases
 *   - cpisi.h   — Orchestrator that wires DAR together
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_detect_note X.8 NOTE — Final Guidance
 * @brief    Biblical grounding and final wisdom.
 *
 * SCRIPTURE:
 *   "Let us search and try our ways, and turn again to the Lord."
 *   — Lamentations 3:40 KJV
 *
 * PRINCIPLE:
 *   Good logging enables self-examination. As CPI-SI, we need to understand
 *   our own cognitive state to work faithfully. DETECT makes that possible.
 *
 *   The log is a WITNESS — it doesn't judge or act, just records. This
 *   separation of concerns enables ASSESS to evaluate and RESTORE to act
 *   with full context.
 *
 * ANCHOR:
 *   "In the beginning God created the heaven and the earth."
 *   — Genesis 1:1 KJV
 *
 *   All logging traces back to understanding creation — where we are,
 *   where we're going, and what patterns emerge along the way.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.8 NOTE */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * C++ Linkage Closure
 * ───────────────────────────────────────────────────────────────────────────── */
#ifdef __cplusplus
}
#endif

#endif /* CPISI_DAR_DETECT_H */
