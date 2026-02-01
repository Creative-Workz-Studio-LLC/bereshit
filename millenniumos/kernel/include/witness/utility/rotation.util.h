/// @file rotation.h
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
/// @omni:req  key        = CPISI-DAR-ROTATION
/// @omni:req  from       = templates/code/c/header.h
/// @omni:req  at         = b-03.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = dar-io
/// @omni:inh  role       = rotation
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi/dar/io
/// @omni:ins  layer      = dar/io/rotation
/// @omni:ins  includes   = types/config/config.h
/// @omni:ins  provides   = [DARRotationConfig, DARRotationState, rotation functions]
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
/// @omni:req  title      = CPI-SI DAR File Rotation System
/// @omni:req  brief      = Dated folder rotation, archival, and retention
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
 * @defgroup cpisi_rotation_special_config SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1a POSIX — Feature Test [POSIX]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_rotation_special_config
 * @brief    Enables POSIX.1-2008 functions in standard headers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L  /**< POSIX.1-2008 — enables strdup, mkdir, etc. */
#endif

/** @} */ /* end SP.1a POSIX */

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef CPISI_DAR_ROTATION_H
#define CPISI_DAR_ROTATION_H

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
 * @file      rotation.h
 * @brief     CPI-SI DAR File Rotation — Dated folders, archival, and retention.
 *
 * @defgroup cpisi_rotation_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-DAR-ROTATION
 * Title:     CPI-SI DAR File Rotation System
 * Type:      Header (DAR I/O)
 * Component: DAR I/O — shared rotation for all DAR phases
 * Role:      Manage witness file lifecycle with dated folders and retention.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_state M.2 STATE — Lifecycle State
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
 * @defgroup cpisi_rotation_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_rotation_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/dar/io/rotation.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Derives_from: types/config/config.h (limits and system types)
 * Template:     bereshit/word/seed/code/c/header.h
 *
 * Builds FROM:  config.h (CPISI_PATH_MAX, CPISI_NAME_MAX, all system types)
 * Builds TO:    detect.h (log rotation), assess.h (report rotation), restore.h
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: dar, io, rotation, archival, retention, dated-folders
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Manage DAR witness file lifecycle with organized storage.
 * Core Design: Dated folder structure: base_path/YYYY/MM/DD/prefix_YYYYMMDD_NNN.ext
 * Philosophy:  Witness data is preserved, organized, and eventually archived.
 *
 * RETENTION POLICY (from log.toml):
 *   - Active:  Current day's files
 *   - Recent:  90 days (detailed, accessible)
 *   - Archive: 90-180 days (compressed)
 *   - Delete:  After 180 days
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Revelation 20:12 — "And the books were opened"
 * Principle:  Witness data is preserved and organized for future reference.
 * Anchor:     Ecclesiastes 3:1 — "To every thing there is a season"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types/config/config.h (CPISI_PATH_MAX, CPISI_NAME_MAX, all types)
 *
 * What Uses This:
 *   - detect.h — DETECT phase uses this for log file rotation
 *   - assess.h — ASSESS phase uses this for report rotation
 *   - restore.h — RESTORE phase uses this for history rotation
 *
 * @see config.h for shared limit constants
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-02.00 (2026-01-04) — Initial rotation implementation
 *   a-02.50 (2026-01-17) — Added archival, retention policy
 *   b-03.00 (2026-01-25) — Full header.h template alignment (4-block)
 *
 * Current:
 *   ✓ M.1-M.10 METADATA structure
 *   ✓ Dated folder organization
 *   ✓ Size-based rotation
 *   ✓ Entry-based rotation
 *   ✓ Retention policy enforcement
 *
 * Current (continued):
 *   ✓ Compression backend support (gzip, zstd, none)
 *   ✓ Cross-phase aggregation (unified witness streams)
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
 * @defgroup cpisi_rotation_includes S.1 INCLUDES — Header Dependencies
 * @brief    Required headers for rotation functionality.
 *
 * STRUCTURE:
 *   - S.1a STANDARD  — System headers (inherited via config.h)
 *   - S.1b INTERNAL  — CPI-SI headers (config.h)
 *   - S.1c EXTERNAL  — Third-party (none)
 *   - S.1d CPP       — C++ linkage wrapper
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* S.1a STANDARD — [inherited: all system headers via config.h → types.h] */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — CPI-SI Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_includes_internal S.1b INTERNAL — CPI-SI Headers
 * @ingroup  cpisi_rotation_includes
 * @brief    Internal CPI-SI configuration and type dependencies.
 *
 * DEPENDENCY LADDER:
 *   config.h → types.h → (all system headers)
 *
 * WHAT config.h PROVIDES (directly):
 *   - CPISI_PATH_MAX     — Maximum path length
 *   - CPISI_NAME_MAX     — Maximum name length
 *   - CPISI_RETENTION_*  — Retention policy constants
 *   - CPISI_ROTATION_*   — Rotation constants
 *
 * WHAT config.h PROVIDES (via types.h):
 *   - time_t, FILE*, bool, size_t, uint32_t (all system types)
 *   - stdint.h, stdbool.h, stdio.h, time.h, etc.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Configuration constants and system types.
 *
 * WHY: config.h provides all the limits (CPISI_PATH_MAX, CPISI_NAME_MAX) and
 *      transitively includes types.h which provides all system headers. This
 *      single include gives rotation.h everything it needs.
 *
 * PROVIDES:
 *   - CPISI_PATH_MAX, CPISI_NAME_MAX (limits)
 *   - CPISI_RETENTION_RECENT, CPISI_RETENTION_ARCHIVE (policy)
 *   - time_t, FILE*, bool, size_t, uint32_t (via types.h)
 *
 * CHAINS TO: types.h → (all system headers)
 */
// Freestanding kernel mode: use kernel.h for base types
#include "kernel.h"

// Freestanding stubs for hosted types
#if CPISI_MODE == 0  // KERNEL freestanding mode
typedef void FILE;                    // Stub - no filesystem in freestanding
typedef uint64_t time_t;              // Stub - kernel uses mos_time_t
#else
#include <stdio.h>
#include <time.h>
#endif

// Domain constants (from cornerstone domain.cfg.h)
#ifndef CPISI_PATH_MAX
#define CPISI_PATH_MAX          1024
#endif
#ifndef CPISI_NAME_MAX
#define CPISI_NAME_MAX          256
#endif
#ifndef CPISI_RETENTION_RECENT
#define CPISI_RETENTION_RECENT  7     // Days to keep recent logs
#endif
#ifndef CPISI_RETENTION_ARCHIVE
#define CPISI_RETENTION_ARCHIVE 30    // Days to keep archived logs
#endif

// Error types (from cornerstone error.type.h)
typedef uint16_t CpisiError;
#define CPISI_OK                0
#define CPISI_ERR_BASE_STORAGE  162
#define CPISI_ERR_IO            (CPISI_ERR_BASE_STORAGE + 0)
#define CPISI_ERR_PERMISSION    (CPISI_ERR_BASE_STORAGE + 1)
#define CPISI_ERR_NOT_FOUND     (CPISI_ERR_BASE_STORAGE + 2)

/** @} */ /* end S.1b INTERNAL */

/* S.1c EXTERNAL — [reserved: no third-party dependencies] */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_includes_cpp S.1d CPP — C++ Linkage
 * @ingroup  cpisi_rotation_includes
 * @brief    C++ compatibility wrapper for C linkage.
 *
 * WHY: Enables C++ code to call rotation functions without name mangling.
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
 * @defgroup cpisi_rotation_defines S.2 DEFINES — Compile-time Definitions
 * @brief    Sizes, limits, flags, macros, and compile-time verification.
 *
 * STRUCTURE:
 *   - S.2a SIZES   — Buffer and array sizes
 *   - S.2b LIMITS  — Value bounds (min, max, default)
 *   - S.2c FLAGS   — Boolean and bit flags
 *   - S.2d MACROS  — Utility macros
 *   - S.2e VERIFY  — Compile-time assertions (tripwires)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a SIZES — Buffer Sizes [SIZES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_sizes S.2a SIZES — Buffer Sizes
 * @ingroup  cpisi_rotation_defines
 * @brief    Rotation-specific buffer sizes.
 *
 * WHY LOCAL: CPISI_PATH_MAX and CPISI_NAME_MAX come from config.h.
 *            Extension and subdir limits are rotation-specific so defined here.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Maximum file extension length (e.g., ".jsonl", ".log.gz").
 */
#define CPISI_ROTATION_EXT_MAX      16

/**
 * @brief Maximum subdirectory name length (e.g., "archive", "backup").
 */
#define CPISI_ROTATION_SUBDIR_MAX   32

/** @} */ /* end S.2a SIZES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b LIMITS — Value Bounds [LIMITS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_limits S.2b LIMITS — Value Bounds
 * @ingroup  cpisi_rotation_defines
 * @brief    Rotation policy limits and defaults.
 *
 * TOML SOURCE: bereshit/word/core/os/health/log.toml [system.rotation]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Days to keep detailed (uncompressed) files.
 */
#define CPISI_ROTATION_RECENT_DAYS   90

/**
 * @brief Days to keep archived (compressed) files.
 */
#define CPISI_ROTATION_ARCHIVE_DAYS  180

/**
 * @brief Default rotation size in KB (1MB).
 */
#define CPISI_ROTATION_SIZE_KB       1024

/**
 * @brief Hours between rotation checks.
 */
#define CPISI_ROTATION_CHECK_HOURS   1

/** @} */ /* end S.2b LIMITS */

/* S.2c FLAGS — [reserved: no rotation-specific flags] */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d MACROS — Date Format Macros [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_macros S.2d MACROS — Date Format Macros
 * @ingroup  cpisi_rotation_defines
 * @brief    Format strings for dated folder structure.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#define CPISI_DATE_FMT_YEAR   "%Y"        /**< 4-digit year */
#define CPISI_DATE_FMT_MONTH  "%m"        /**< 2-digit month */
#define CPISI_DATE_FMT_DAY    "%d"        /**< 2-digit day */
#define CPISI_DATE_FMT_FULL   "%Y%m%d"    /**< YYYYMMDD for filenames */
#define CPISI_DATE_FMT_ISO    "%Y-%m-%d"  /**< ISO 8601 for display */

/** @} */ /* end S.2d MACROS */

/* S.2e VERIFY — [reserved: compile-time assertions] */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Definitions [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_types S.3 TYPES — Type Definitions
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

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — Error Codes (from error.h) [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_enums S.3a ENUMS — Error Codes
 * @ingroup  cpisi_rotation_types
 * @brief    Rotation uses STORAGE layer errors from error.h.
 *
 * ERROR ARCHITECTURE (from error.h / error-system.toml):
 *   - 9 layers × 27 codes = 243, plus 13 metadata = 256 (bridge equation)
 *   - STORAGE layer (6) = codes 162-188
 *   - Formula: layer_base + (tier × 9) + position
 *
 * STORAGE LAYER CODES FROM error.h (CORE tier 162-170):
 *   - CPISI_ERR_STORAGE_NULL     (163) — Null pointer
 *   - CPISI_ERR_STORAGE_INVALID  (164) — Invalid parameter
 *   - CPISI_ERR_STORAGE_BOUNDS   (165) — Path too long, size exceeded
 *   - CPISI_ERR_STORAGE_TYPE     (166) — Wrong file type
 *   - CPISI_ERR_STORAGE_STATE    (167) — Not initialized
 *   - CPISI_ERR_STORAGE_INIT     (168) — Init failed
 *   - CPISI_ERR_STORAGE_SHUTDOWN (169) — Cleanup failed
 *   - CPISI_ERR_STORAGE_UNKNOWN  (170) — Unknown error
 *
 * TIER BASES FROM error.h:
 *   - CPISI_ERR_STORAGE_CORE (162) — core tier
 *   - CPISI_ERR_STORAGE_EXT  (171) — extended tier
 *   - CPISI_ERR_STORAGE_SYS  (180) — system tier
 *
 * USAGE: Functions return CpisiError. Use CPISI_OK (0) for success.
 *        Use CPISI_ERR_STORAGE_* for standard errors.
 *        Use DAR_ROTATION_ERR_* for rotation-specific extended/system tier.
 *
 * @see error.h for full error code system
 * @see error-system.toml for architecture spec
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @name Rotation Success
 * @brief Success code for rotation operations.
 * @{
 */
#define DAR_ROTATION_OK             CPISI_OK  /**< Global success (0) */
/** @} */

/**
 * @name Rotation CORE Tier Aliases
 * @brief CORE tier errors (162-170) — direct aliases to CPISI_ERR_STORAGE_*.
 *
 * These are simple aliases for readability. Use the CPISI_ERR_STORAGE_*
 * codes directly if you prefer — they're identical values.
 * @{
 */
#define DAR_ROTATION_ERR_NULL       CPISI_ERR_STORAGE_NULL      /**< 163: Null pointer */
#define DAR_ROTATION_ERR_INVALID    CPISI_ERR_STORAGE_INVALID   /**< 164: Invalid param */
#define DAR_ROTATION_ERR_BOUNDS     CPISI_ERR_STORAGE_BOUNDS    /**< 165: Path too long */
#define DAR_ROTATION_ERR_TYPE       CPISI_ERR_STORAGE_TYPE      /**< 166: Format mismatch */
#define DAR_ROTATION_ERR_STATE      CPISI_ERR_STORAGE_STATE     /**< 167: Not initialized */
#define DAR_ROTATION_ERR_INIT       CPISI_ERR_STORAGE_INIT      /**< 168: Init failed */
#define DAR_ROTATION_ERR_CLEANUP    CPISI_ERR_STORAGE_SHUTDOWN  /**< 169: Cleanup failed */
/** @} */

/**
 * @name Rotation EXT Tier (File Operations)
 * @brief Extended tier errors (171-179) — file I/O specific.
 *
 * These extend the CORE tier with specific file operation errors.
 * Built using CPISI_ERR_STORAGE_EXT base + offset.
 * @{
 */
#define DAR_ROTATION_ERR_OPEN       (CPISI_ERR_STORAGE_EXT + 1)  /**< 172: Open failed */
#define DAR_ROTATION_ERR_WRITE      (CPISI_ERR_STORAGE_EXT + 2)  /**< 173: Write failed */
#define DAR_ROTATION_ERR_READ       (CPISI_ERR_STORAGE_EXT + 3)  /**< 174: Read failed */
#define DAR_ROTATION_ERR_RENAME     (CPISI_ERR_STORAGE_EXT + 4)  /**< 175: Rename failed */
#define DAR_ROTATION_ERR_DELETE     (CPISI_ERR_STORAGE_EXT + 5)  /**< 176: Delete failed */
#define DAR_ROTATION_ERR_MKDIR      (CPISI_ERR_STORAGE_EXT + 6)  /**< 177: Mkdir failed */
#define DAR_ROTATION_ERR_ACCESS     (CPISI_ERR_STORAGE_EXT + 7)  /**< 178: Permission denied */
#define DAR_ROTATION_ERR_NOTFOUND   (CPISI_ERR_STORAGE_EXT + 8)  /**< 179: Not found */
/** @} */

/**
 * @name Rotation SYS Tier (Rotation-Specific)
 * @brief System tier errors (180-188) — rotation-specific operations.
 *
 * These are internal/system-level errors specific to rotation.
 * Built using CPISI_ERR_STORAGE_SYS base + offset.
 * @{
 */
#define DAR_ROTATION_ERR_ROTATE     (CPISI_ERR_STORAGE_SYS + 1)  /**< 181: Rotation failed */
#define DAR_ROTATION_ERR_ARCHIVE    (CPISI_ERR_STORAGE_SYS + 2)  /**< 182: Archive failed */
#define DAR_ROTATION_ERR_COMPRESS   (CPISI_ERR_STORAGE_SYS + 3)  /**< 183: Compress failed */
#define DAR_ROTATION_ERR_RETENTION  (CPISI_ERR_STORAGE_SYS + 4)  /**< 184: Retention error */
#define DAR_ROTATION_ERR_SEQUENCE   (CPISI_ERR_STORAGE_SYS + 5)  /**< 185: Sequence overflow */
#define DAR_ROTATION_ERR_TIME       (CPISI_ERR_STORAGE_SYS + 6)  /**< 186: Time error */
#define DAR_ROTATION_ERR_SYNC       (CPISI_ERR_STORAGE_SYS + 7)  /**< 187: Sync failed */
/** @} */

/**
 * @name Compression Backend
 * @brief Compression algorithms for archive operations.
 *
 * WHY ENUM: Allows configuration of compression method at runtime.
 *           Each backend has different tradeoffs (speed vs ratio).
 *
 * SELECTION CRITERIA:
 *   - NONE:  Maximum speed, no CPU overhead, no size reduction
 *   - GZIP:  Universal compatibility, moderate speed/ratio
 *   - ZSTD:  Best ratio, fast decompression, modern systems
 *
 * DEFAULT: GZIP — balance of compatibility and compression.
 * @{
 */

/**
 * @brief Compression backend selection for archive operations.
 *
 * WHY: Different backends suit different needs:
 *   - NONE: When I/O bound or storage is cheap
 *   - GZIP: When archives may be read by external tools
 *   - ZSTD: When maximum compression with fast decompression matters
 */
typedef enum DARCompressionBackend {
    DAR_COMPRESS_NONE  = 0,   /**< No compression — fastest, largest files */
    DAR_COMPRESS_GZIP  = 1,   /**< gzip (.gz) — universal compatibility */
    DAR_COMPRESS_ZSTD  = 2,   /**< zstd (.zst) — modern, best ratio */

    DAR_COMPRESS_COUNT = 3    /**< Sentinel for array sizing */
} DARCompressionBackend;

/**
 * @brief Default compression backend.
 */
#define DAR_COMPRESS_DEFAULT    DAR_COMPRESS_GZIP

/** @} */ /* end Compression Backend */

/**
 * @name DAR Phase Identifiers
 * @brief Phase identifiers for cross-phase aggregation.
 *
 * WHY: Cross-phase aggregation combines witness files from multiple DAR phases
 *      into unified streams. Phase identifiers track which phase produced data.
 *
 * AGGREGATION USE CASES:
 *   - Unified timeline: See DETECT→ASSESS→RESTORE flow in one view
 *   - Correlation: Match detection events to restoration actions
 *   - Metrics: Aggregate health trends across all phases
 * @{
 */

/**
 * @brief DAR phase identifier for aggregation tagging.
 *
 * WHY: When aggregating across phases, each entry needs its source phase.
 *      This enables filtering, sorting, and correlation of cross-phase data.
 */
typedef enum DARPhaseId {
    DAR_PHASE_ID_NONE    = 0,   /**< Unspecified phase */
    DAR_PHASE_ID_DETECT  = 1,   /**< DETECT phase — witness/logging */
    DAR_PHASE_ID_ASSESS  = 2,   /**< ASSESS phase — evaluation/debug */
    DAR_PHASE_ID_RESTORE = 3,   /**< RESTORE phase — recovery/history */

    DAR_PHASE_ID_COUNT   = 4    /**< Sentinel for array sizing */
} DARPhaseId;

/** @} */ /* end DAR Phase Identifiers */

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — Structure Definitions [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_structs S.3b STRUCTS — Structure Definitions
 * @ingroup  cpisi_rotation_types
 * @brief    Rotation configuration and state structures.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Configuration for DAR file rotation.
 *
 * WHY: Configures how witness files are organized, rotated, and retained.
 *      Used by all DAR phases (DETECT, ASSESS, RESTORE).
 *
 * SECTIONS:
 *   - Path Configuration:   Where files live
 *   - Folder Structure:     How directories are organized
 *   - File Naming:          How files are named
 *   - Rotation Triggers:    When to create new files
 *   - Retention Policy:     How long to keep files
 *   - Compression:          How to compress archives
 *   - Aggregation:          Cross-phase unified streams
 *
 * THREAD SAFETY: Configuration is immutable after init. Safe to share
 *                config across threads once initialized.
 */
typedef struct DARRotationConfig {

    /* ─────────────────────────────────────────────────────────────────────────
     * Path Configuration
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Base directory for witness files.
     *
     * WHY: All rotation files live under this directory. Structure is:
     *      base_path/YYYY/MM/DD/prefix_YYYYMMDD_NNN.ext
     *
     * EXAMPLE: "./logs/cpisi" → "./logs/cpisi/2026/01/25/cpisi_20260125_001.jsonl"
     */
    char        base_path[CPISI_PATH_MAX];

    /* ─────────────────────────────────────────────────────────────────────────
     * Folder Structure
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Use dated YYYY/MM/DD folder structure.
     *
     * WHY: Dated folders enable efficient retention policy enforcement and
     *      human-navigable archive browsing. When false, all files are flat.
     *
     * DEFAULT: true — organization matters for witness data.
     */
    bool        use_dated_folders;

    /* ─────────────────────────────────────────────────────────────────────────
     * File Naming
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief File name prefix (e.g., "detect", "assess", "restore").
     *
     * WHY: Distinguishes files from different DAR phases when archived together.
     *      Format: prefix_YYYYMMDD_NNN.ext
     */
    char        file_prefix[CPISI_NAME_MAX];

    /**
     * @brief File extension including dot (e.g., ".jsonl", ".log").
     *
     * WHY: Extension indicates format. Compression adds additional extension
     *      (e.g., ".jsonl.gz"). Keep base extension simple.
     */
    char        file_extension[CPISI_ROTATION_EXT_MAX];

    /**
     * @brief Create new file per engine spawn.
     *
     * WHY: Enables clear boundaries between sessions. When true, each spawn
     *      gets its own file instead of continuing the day's sequence.
     *      Format becomes: prefix_YYYYMMDD-HHMMSS-PID-SEQ.ext
     *
     * DEFAULT: true (recommended for session tracking)
     */
    bool        per_spawn_files;

    /**
     * @brief Session ID to include in filename (if per_spawn_files is true).
     *
     * WHY: Links log files to session database entries. Set by LogContext
     *      when session database is enabled.
     *
     * FORMAT: YYYYMMDD-HHMMSS-PID-SEQ (e.g., "20260129-143025-12345-001")
     */
    char        session_id[32];

    /* ─────────────────────────────────────────────────────────────────────────
     * Rotation Triggers
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Maximum file size in bytes before rotation.
     *
     * WHY: Prevents individual files from growing unbounded. Large files are
     *      slow to read, hard to transfer, and risky to corrupt.
     *
     * VALUE: 0 = no size limit (use entry limit or time-based instead)
     * DEFAULT: 1MB (CPISI_ROTATION_SIZE_KB * 1024)
     */
    size_t      max_file_size;

    /**
     * @brief Maximum entries before rotation.
     *
     * WHY: For structured formats like JSONL, entry count is more meaningful
     *      than byte size. Enables predictable file sizes.
     *
     * VALUE: 0 = no entry limit (use size limit instead)
     * DEFAULT: 0 (size-based rotation preferred)
     */
    uint32_t    max_entries;

    /* ─────────────────────────────────────────────────────────────────────────
     * Retention Policy
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Days to keep uncompressed files.
     *
     * WHY: Recent files are accessed frequently and should be fast to read.
     *      After this period, files are compressed to archive.
     *
     * LIFECYCLE: Active → Recent (uncompressed) → Archive (compressed) → Delete
     * DEFAULT: 90 days (CPISI_ROTATION_RECENT_DAYS)
     */
    uint32_t    recent_days;

    /**
     * @brief Total days to keep files (including archived).
     *
     * WHY: Eventually all data expires. This is the total retention from
     *      creation to deletion. archive_days > recent_days.
     *
     * DEFAULT: 180 days (CPISI_ROTATION_ARCHIVE_DAYS)
     */
    uint32_t    archive_days;

    /* ─────────────────────────────────────────────────────────────────────────
     * Compression Settings
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Enable compression when archiving files.
     *
     * WHY: Compressed archives save storage and bandwidth. Disable only when
     *      CPU is severely constrained or files are already compressed.
     *
     * DEFAULT: true
     */
    bool        compress_archive;

    /**
     * @brief Compression algorithm to use.
     *
     * WHY: Different algorithms suit different needs:
     *   - DAR_COMPRESS_GZIP: Universal compatibility (.gz)
     *   - DAR_COMPRESS_ZSTD: Better ratio, faster decompression (.zst)
     *   - DAR_COMPRESS_NONE: When compress_archive is false (ignored)
     *
     * DEFAULT: DAR_COMPRESS_GZIP
     */
    DARCompressionBackend compress_backend;

    /**
     * @brief Compression level (algorithm-specific).
     *
     * WHY: Higher levels = better compression but slower.
     *      - GZIP: 1-9, default 6
     *      - ZSTD: 1-22, default 3
     *
     * VALUE: 0 = use algorithm default
     */
    uint8_t     compress_level;

    /**
     * @brief Subdirectory name for archives.
     *
     * WHY: Separates compressed archives from active files for clarity.
     *      Archives go to: base_path/archive_subdir/YYYY/MM/
     *
     * DEFAULT: "archive"
     */
    char        archive_subdir[CPISI_ROTATION_SUBDIR_MAX];

    /* ─────────────────────────────────────────────────────────────────────────
     * Cross-Phase Aggregation
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Phase identifier for this rotation instance.
     *
     * WHY: When aggregating across phases, entries are tagged with source phase.
     *      This enables filtering and correlation in unified streams.
     *
     * VALUES: DAR_PHASE_ID_DETECT, DAR_PHASE_ID_ASSESS, DAR_PHASE_ID_RESTORE
     */
    DARPhaseId  phase_id;

    /**
     * @brief Enable cross-phase aggregation.
     *
     * WHY: When enabled, rotation can write to shared aggregation streams
     *      in addition to phase-specific files. Enables unified views.
     *
     * DEFAULT: false (phase isolation by default)
     */
    bool        enable_aggregation;

    /**
     * @brief Path for aggregated cross-phase output.
     *
     * WHY: Aggregation writes to a separate location to avoid mixing with
     *      phase-specific files. Often a shared directory or stream.
     *
     * ONLY USED WHEN: enable_aggregation is true
     */
    char        aggregation_path[CPISI_PATH_MAX];

} DARRotationConfig;

/**
 * @brief Alias for backward compatibility.
 */
typedef DARRotationConfig LogRotationConfig;

/**
 * @brief Default configuration initializer.
 *
 * DEFAULTS:
 *   - Dated folders enabled for organization
 *   - 1MB file size limit
 *   - 90 days recent, 180 days archive
 *   - GZIP compression at default level
 *   - Aggregation disabled (phase isolation)
 */
#define DAR_ROTATION_CONFIG_DEFAULT { \
    /* Path Configuration */ \
    .base_path = "./logs/cpisi", \
    /* Folder Structure */ \
    .use_dated_folders = true, \
    /* File Naming */ \
    .file_prefix = "cpisi", \
    .file_extension = ".jsonl", \
    /* Rotation Triggers */ \
    .max_file_size = CPISI_ROTATION_SIZE_KB * 1024, \
    .max_entries = 0, \
    /* Retention Policy */ \
    .recent_days = CPISI_ROTATION_RECENT_DAYS, \
    .archive_days = CPISI_ROTATION_ARCHIVE_DAYS, \
    /* Compression Settings */ \
    .compress_archive = true, \
    .compress_backend = DAR_COMPRESS_DEFAULT, \
    .compress_level = 0, \
    .archive_subdir = "archive", \
    /* Cross-Phase Aggregation */ \
    .phase_id = DAR_PHASE_ID_NONE, \
    .enable_aggregation = false, \
    .aggregation_path = "", \
}

/**
 * @brief Runtime state for DAR file rotation.
 *
 * WHY: Tracks the current file, timing, and rotation state for a DAR phase.
 *      Maintains both phase-specific and aggregation state when enabled.
 *
 * SECTIONS:
 *   - Current File:     Active file being written
 *   - Timing:           Timestamps for rotation/cleanup decisions
 *   - Aggregation:      Cross-phase unified stream state
 *   - Statistics:       Cumulative metrics for monitoring
 *   - Configuration:    Immutable settings copy
 *
 * THREAD SAFETY: State is NOT thread-safe. Each thread/phase needs its own
 *                DARRotationState. Aggregation uses file locking internally.
 */
typedef struct DARRotationState {

    /* ─────────────────────────────────────────────────────────────────────────
     * Current File
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Full path to currently open file.
     *
     * WHY: Needed for logging, debugging, and rotation decisions.
     *      Empty string when no file is open.
     */
    char        current_path[CPISI_PATH_MAX];

    /**
     * @brief Open file handle for writing.
     *
     * WHY: Kept open between writes for efficiency. Closed on rotation.
     *      NULL when no file is open.
     *
     * OWNERSHIP: Owned by state, closed on shutdown or rotation.
     */
    FILE*       current_file;

    /**
     * @brief Bytes written to current file.
     *
     * WHY: Tracks size for size-based rotation trigger.
     *      Reset on rotation.
     */
    size_t      current_size;

    /**
     * @brief Entries written to current file.
     *
     * WHY: Tracks entry count for entry-based rotation trigger.
     *      Reset on rotation.
     */
    uint32_t    current_entries;

    /**
     * @brief Sequence number for today's files.
     *
     * WHY: Multiple files per day are numbered: prefix_YYYYMMDD_001.ext,
     *      prefix_YYYYMMDD_002.ext, etc. Reset at midnight.
     */
    uint32_t    sequence_num;

    /* ─────────────────────────────────────────────────────────────────────────
     * Timing
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief When current file was created.
     *
     * WHY: Used for time-based rotation and age calculations.
     */
    time_t      file_created;

    /**
     * @brief When we last checked for rotation.
     *
     * WHY: Avoid checking too frequently. Check interval is configurable.
     */
    time_t      last_rotation_check;

    /**
     * @brief When we last ran cleanup.
     *
     * WHY: Cleanup (archive + delete) is expensive. Run periodically, not
     *      on every write.
     */
    time_t      last_cleanup;

    /* ─────────────────────────────────────────────────────────────────────────
     * Aggregation State
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Aggregation file path (when aggregation enabled).
     *
     * WHY: Cross-phase entries are written to shared aggregation file.
     *      Separate from phase-specific files.
     */
    char        agg_path[CPISI_PATH_MAX];

    /**
     * @brief Aggregation file handle.
     *
     * WHY: Shared across phases. Uses file locking for concurrent access.
     *      NULL when aggregation disabled.
     */
    FILE*       agg_file;

    /**
     * @brief Total entries written to aggregation stream.
     *
     * WHY: Metrics for monitoring cross-phase data volume.
     */
    uint64_t    agg_entries_total;

    /* ─────────────────────────────────────────────────────────────────────────
     * Statistics
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Total files rotated since init.
     *
     * WHY: Metrics for monitoring rotation frequency.
     */
    uint32_t    stats_files_rotated;

    /**
     * @brief Total bytes written since init.
     *
     * WHY: Metrics for monitoring data volume.
     */
    uint64_t    stats_bytes_total;

    /**
     * @brief Total entries written since init.
     *
     * WHY: Metrics for monitoring entry volume.
     */
    uint64_t    stats_entries_total;

    /**
     * @brief Files archived since init.
     *
     * WHY: Metrics for monitoring archive activity.
     */
    uint32_t    stats_files_archived;

    /**
     * @brief Files deleted since init.
     *
     * WHY: Metrics for monitoring cleanup activity.
     */
    uint32_t    stats_files_deleted;

    /* ─────────────────────────────────────────────────────────────────────────
     * Configuration
     * ───────────────────────────────────────────────────────────────────────── */

    /**
     * @brief Copy of configuration (immutable after init).
     *
     * WHY: State owns its config copy. Caller's config can be discarded
     *      after cpisi_dar_rotation_init().
     */
    DARRotationConfig config;

} DARRotationState;

/**
 * @brief Alias for backward compatibility.
 */
typedef DARRotationState LogRotationState;

/**
 * @brief Default state initializer.
 *
 * WHY: Zero-initialize all runtime state. Config gets defaults.
 *      Use cpisi_dar_rotation_init() for proper initialization.
 */
#define DAR_ROTATION_STATE_INIT { \
    /* Current File */ \
    .current_path = "", \
    .current_file = NULL, \
    .current_size = 0, \
    .current_entries = 0, \
    .sequence_num = 0, \
    /* Timing */ \
    .file_created = 0, \
    .last_rotation_check = 0, \
    .last_cleanup = 0, \
    /* Aggregation State */ \
    .agg_path = "", \
    .agg_file = NULL, \
    .agg_entries_total = 0, \
    /* Statistics */ \
    .stats_files_rotated = 0, \
    .stats_bytes_total = 0, \
    .stats_entries_total = 0, \
    .stats_files_archived = 0, \
    .stats_files_deleted = 0, \
    /* Configuration */ \
    .config = DAR_ROTATION_CONFIG_DEFAULT, \
}

/** @} */ /* end S.3b STRUCTS */

/* S.3c OPAQUE — [reserved: no opaque types] */

/* S.3d ERRORS — [see S.3a ENUMS for DARRotationError] */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_prototypes S.4 PROTOTYPES — Function Declarations
 * @brief    Lifecycle, accessors, and operations for file rotation.
 *
 * STRUCTURE:
 *   - S.4a LIFECYCLE  — init, shutdown
 *   - S.4b ACCESSORS  — validation, helpers
 *   - S.4c OPERATIONS — file access, rotation, maintenance
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a LIFECYCLE — Create/Init/Destroy [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_lifecycle S.4a LIFECYCLE — Create/Init/Destroy
 * @ingroup  cpisi_rotation_prototypes
 * @brief    Rotation state lifecycle management.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize the DAR rotation system.
 *
 * @param state   State struct to initialize.
 * @param config  Configuration (NULL for defaults).
 * @return        CPISI_OK on success, STORAGE layer error on failure.
 *
 * ERRORS:
 *   - DAR_ROTATION_ERR_NULL (163): state is NULL
 *   - DAR_ROTATION_ERR_STATE (167): already initialized
 *   - DAR_ROTATION_ERR_INIT (168): initialization failed
 */
CpisiError cpisi_dar_rotation_init(DARRotationState* state, const DARRotationConfig* config);

/**
 * @brief Shutdown rotation, closing files and cleaning up.
 *
 * @param state  State struct to shutdown.
 */
void cpisi_dar_rotation_shutdown(DARRotationState* state);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ACCESSORS — Validation and Helpers [ACCESSORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_accessors S.4b ACCESSORS — Validation and Helpers
 * @ingroup  cpisi_rotation_prototypes
 * @brief    Inline validation and helper functions.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Validate rotation configuration.
 * @param config  Configuration to validate.
 * @return        true if valid, false otherwise.
 */
static inline bool cpisi_dar_rotation_config_valid(const DARRotationConfig* config) {
    if (!config) return false;
    if (config->base_path[0] == '\0') return false;
    if (config->file_prefix[0] == '\0') return false;
    if (config->file_extension[0] == '\0') return false;
    return true;
}

/**
 * @brief Validate rotation state.
 * @param state  State to validate.
 * @return       true if valid, false otherwise.
 */
static inline bool cpisi_dar_rotation_state_valid(const DARRotationState* state) {
    if (!state) return false;
    return cpisi_dar_rotation_config_valid(&state->config);
}

/**
 * @brief Check if state has an open file.
 * @param state  State to check.
 * @return       true if file is open, false otherwise.
 */
static inline bool cpisi_dar_rotation_has_file(const DARRotationState* state) {
    return state && state->current_file != NULL;
}

/**
 * @brief Check if rotation is needed based on file size.
 * @param state  State to check.
 * @return       true if size limit exceeded, false otherwise.
 */
static inline bool cpisi_dar_rotation_needs_size_rotation(const DARRotationState* state) {
    if (!state || state->config.max_file_size == 0) return false;
    return state->current_size >= state->config.max_file_size;
}

/**
 * @brief Check if rotation is needed based on entry count.
 * @param state  State to check.
 * @return       true if entry limit exceeded, false otherwise.
 */
static inline bool cpisi_dar_rotation_needs_entry_rotation(const DARRotationState* state) {
    if (!state || state->config.max_entries == 0) return false;
    return state->current_entries >= state->config.max_entries;
}

/** @} */ /* end S.4b ACCESSORS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c OPERATIONS — File, Rotation, Maintenance [OPERATIONS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_rotation_operations S.4c OPERATIONS — File, Rotation, Maintenance
 * @ingroup  cpisi_rotation_prototypes
 * @brief    Core rotation operations.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @name S.4c.1 File Access */
/** @{ */

/**
 * @brief Get the current file for writing.
 *
 * Automatically rotates if needed. Creates dated directories if they don't exist.
 *
 * @param state  DAR rotation state.
 * @return       FILE* for writing, or NULL on error.
 */
FILE* cpisi_dar_rotation_get_file(DARRotationState* state);

/**
 * @brief Notify that bytes were written.
 *
 * Call after writing to update size and entry tracking.
 *
 * @param state    DAR rotation state.
 * @param bytes    Number of bytes written.
 * @param entries  Number of entries written.
 */
void cpisi_dar_rotation_notify_write(DARRotationState* state, size_t bytes, uint32_t entries);

/** @} */ /* end S.4c.1 File Access */

/** @name S.4c.2 Rotation Control */
/** @{ */

/**
 * @brief Check if rotation is needed.
 * @param state  DAR rotation state.
 * @return       true if rotation is needed, false otherwise.
 */
bool cpisi_dar_rotation_check(DARRotationState* state);

/**
 * @brief Force rotation to a new file.
 *
 * @param state  DAR rotation state.
 * @return       CPISI_OK on success, STORAGE layer error on failure.
 *
 * ERRORS:
 *   - DAR_ROTATION_ERR_NULL (163): state is NULL
 *   - DAR_ROTATION_ERR_STATE (167): not initialized
 *   - DAR_ROTATION_ERR_ROTATE (181): rotation operation failed
 */
CpisiError cpisi_dar_rotation_rotate(DARRotationState* state);

/** @} */ /* end S.4c.2 Rotation Control */

/** @name S.4c.3 Maintenance */
/** @{ */

/**
 * @brief Run archival and deletion.
 *
 * Archives files older than recent_days. Deletes archives older than archive_days.
 *
 * @param state  DAR rotation state.
 * @return       Number of files processed (archived + deleted).
 */
int cpisi_dar_rotation_cleanup(DARRotationState* state);

/**
 * @brief Archive files from a specific date.
 *
 * @param state      State struct.
 * @param timestamp  Archive files from this date and earlier.
 * @return           Number of files archived.
 */
int cpisi_dar_rotation_archive(DARRotationState* state, time_t timestamp);

/** @} */ /* end S.4c.3 Maintenance */

/** @name S.4c.4 Path Helpers */
/** @{ */

/**
 * @brief Build path for a specific date.
 *
 * @param state   State struct (for config).
 * @param date    Date to build path for.
 * @param buf     Output buffer.
 * @param buflen  Buffer size.
 * @return        Length written on success (>= 0), negative CpisiError on failure.
 *
 * ERRORS:
 *   - DAR_ROTATION_ERR_NULL (163): state or buf is NULL
 *   - DAR_ROTATION_ERR_BOUNDS (165): path exceeds buflen
 */
int cpisi_dar_rotation_build_path(const DARRotationState* state,
                                   time_t date,
                                   char* buf, size_t buflen);

/**
 * @brief Ensure directory exists.
 *
 * @param path  Directory path to ensure.
 * @return      CPISI_OK on success, STORAGE layer error on failure.
 *
 * ERRORS:
 *   - DAR_ROTATION_ERR_NULL (163): path is NULL
 *   - DAR_ROTATION_ERR_MKDIR (177): failed to create directory
 *   - DAR_ROTATION_ERR_ACCESS (178): permission denied
 */
CpisiError cpisi_dar_rotation_ensure_dir(const char* path);

/** @} */ /* end S.4c.4 Path Helpers */

/** @name S.4c.5 Compression Operations */
/** @{ */

/**
 * @brief Compress a file using the configured backend.
 *
 * WHY: Compresses archived files to save storage. Uses the backend and
 *      level specified in config. Original file is replaced with compressed.
 *
 * @param state      DAR rotation state (for config).
 * @param src_path   Source file to compress.
 * @param dst_path   Destination path for compressed file (NULL = auto).
 * @return           CPISI_OK on success, STORAGE layer error on failure.
 *
 * ERRORS:
 *   - DAR_ROTATION_ERR_NULL (163): state or src_path is NULL
 *   - DAR_ROTATION_ERR_NOTFOUND (179): source file not found
 *   - DAR_ROTATION_ERR_COMPRESS (183): compression failed
 *
 * AUTO-NAMING: When dst_path is NULL:
 *   - GZIP: src_path + ".gz"
 *   - ZSTD: src_path + ".zst"
 */
CpisiError cpisi_dar_rotation_compress(const DARRotationState* state,
                                        const char* src_path,
                                        const char* dst_path);

/**
 * @brief Decompress a file using the configured backend.
 *
 * WHY: Decompresses archived files for reading. Auto-detects format
 *      from file extension.
 *
 * @param state      DAR rotation state (for config).
 * @param src_path   Source compressed file.
 * @param dst_path   Destination path for decompressed file (NULL = auto).
 * @return           CPISI_OK on success, STORAGE layer error on failure.
 *
 * ERRORS:
 *   - DAR_ROTATION_ERR_NULL (163): state or src_path is NULL
 *   - DAR_ROTATION_ERR_NOTFOUND (179): source file not found
 *   - DAR_ROTATION_ERR_COMPRESS (183): decompression failed
 */
CpisiError cpisi_dar_rotation_decompress(const DARRotationState* state,
                                          const char* src_path,
                                          const char* dst_path);

/**
 * @brief Get file extension for compression backend.
 *
 * WHY: Allows callers to construct paths with correct extension.
 *
 * @param backend  Compression backend.
 * @return         Extension string (e.g., ".gz", ".zst", "").
 */
static inline const char* cpisi_dar_rotation_compress_ext(DARCompressionBackend backend) {
    switch (backend) {
        case DAR_COMPRESS_GZIP: return ".gz";
        case DAR_COMPRESS_ZSTD: return ".zst";
        case DAR_COMPRESS_NONE:
        default:                return "";
    }
}

/** @} */ /* end S.4c.5 Compression Operations */

/** @name S.4c.6 Aggregation Operations */
/** @{ */

/**
 * @brief Initialize aggregation for cross-phase unified streams.
 *
 * WHY: Sets up the aggregation file/stream for cross-phase data.
 *      Called after cpisi_dar_rotation_init() when aggregation is enabled.
 *
 * @param state  DAR rotation state (must have enable_aggregation = true).
 * @return       CPISI_OK on success, STORAGE layer error on failure.
 *
 * ERRORS:
 *   - DAR_ROTATION_ERR_NULL (163): state is NULL
 *   - DAR_ROTATION_ERR_STATE (167): aggregation not enabled in config
 *   - DAR_ROTATION_ERR_OPEN (172): failed to open aggregation file
 *
 * THREAD SAFETY: Uses file locking for concurrent access from multiple phases.
 */
CpisiError cpisi_dar_rotation_agg_init(DARRotationState* state);

/**
 * @brief Shutdown aggregation stream.
 *
 * WHY: Closes aggregation file and releases resources.
 *      Called before cpisi_dar_rotation_shutdown().
 *
 * @param state  DAR rotation state.
 */
void cpisi_dar_rotation_agg_shutdown(DARRotationState* state);

/**
 * @brief Write entry to aggregation stream.
 *
 * WHY: Writes a phase-tagged entry to the cross-phase unified stream.
 *      Entry is also written to phase-specific file via normal rotation.
 *
 * @param state   DAR rotation state.
 * @param data    Entry data to write.
 * @param len     Length of data in bytes.
 * @return        CPISI_OK on success, STORAGE layer error on failure.
 *
 * ERRORS:
 *   - DAR_ROTATION_ERR_NULL (163): state or data is NULL
 *   - DAR_ROTATION_ERR_STATE (167): aggregation not initialized
 *   - DAR_ROTATION_ERR_WRITE (173): write to aggregation file failed
 *
 * FORMAT: Entries are prefixed with phase identifier for filtering.
 */
CpisiError cpisi_dar_rotation_agg_write(DARRotationState* state,
                                         const void* data,
                                         size_t len);

/**
 * @brief Get phase name string for logging/display.
 *
 * WHY: Human-readable phase names for logs and debugging.
 *
 * @param phase  Phase identifier.
 * @return       Phase name string (e.g., "DETECT", "ASSESS", "RESTORE").
 */
static inline const char* cpisi_dar_phase_id_name(DARPhaseId phase) {
    switch (phase) {
        case DAR_PHASE_ID_DETECT:  return "DETECT";
        case DAR_PHASE_ID_ASSESS:  return "ASSESS";
        case DAR_PHASE_ID_RESTORE: return "RESTORE";
        case DAR_PHASE_ID_NONE:
        case DAR_PHASE_ID_COUNT:
        default:                   return "NONE";
    }
}

/**
 * @brief Get statistics snapshot from rotation state.
 *
 * WHY: Allows monitoring without direct access to state internals.
 *      Returns current values for all tracked metrics.
 *
 * @param state           DAR rotation state.
 * @param bytes_total     Output: total bytes written (NULL to skip).
 * @param entries_total   Output: total entries written (NULL to skip).
 * @param files_rotated   Output: files rotated (NULL to skip).
 * @param files_archived  Output: files archived (NULL to skip).
 * @param files_deleted   Output: files deleted (NULL to skip).
 */
static inline void cpisi_dar_rotation_stats(
    const DARRotationState* state,
    uint64_t* bytes_total,
    uint64_t* entries_total,
    uint32_t* files_rotated,
    uint32_t* files_archived,
    uint32_t* files_deleted
) {
    if (!state) return;
    if (bytes_total)    *bytes_total    = state->stats_bytes_total;
    if (entries_total)  *entries_total  = state->stats_entries_total;
    if (files_rotated)  *files_rotated  = state->stats_files_rotated;
    if (files_archived) *files_archived = state->stats_files_archived;
    if (files_deleted)  *files_deleted  = state->stats_files_deleted;
}

/** @} */ /* end S.4c.6 Aggregation Operations */

/** @} */ /* end S.4c OPERATIONS */

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
/// NOTE: This is a HEADER file. Implementation lives in rotation.c.
///       BODY contains design documentation for implementers.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_orgchart B.1 ORG CHART — Structure Overview
 * @brief    Rotation system architecture and design.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @name B.1a DESIGN — Folder Structure
 * @{
 *
 * DATED FOLDER STRUCTURE:
 *   base_path/YYYY/MM/DD/prefix_YYYYMMDD_NNN.ext
 *
 * EXAMPLE:
 *   ./logs/cpisi/2026/01/25/cpisi_20260125_001.jsonl
 *   ./logs/cpisi/2026/01/25/cpisi_20260125_002.jsonl
 *
 * ARCHIVE STRUCTURE:
 *   base_path/archive/YYYY/MM/prefix_YYYYMMDD.tar.gz
 *
 * @}
 */

/**
 * @name B.1b DAR ROLE — Shared Rotation
 * @{
 *
 * DETECT (logging)  → uses rotation for log files
 * ASSESS (debug)    → uses rotation for assessment reports
 * RESTORE (history) → uses rotation for recovery history
 *
 * Each phase creates its own DARRotationState with its own prefix.
 *
 * @}
 */

/** @} */ /* end B.1 ORG CHART */

/* B.2 HELPERS — [reserved: implementation in rotation.c] */

/* B.3 CORE OPS — [reserved: implementation in rotation.c] */

/* B.4 ERRORS — [reserved: implementation in rotation.c] */

/* B.5 PUBLIC API — [reserved: implementation in rotation.c] */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Validation, usage, cleanup, and guidance.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Build and Verify [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_validation X.1 VALIDATION — Build and Verify
 * @brief    Compilation and verification commands.
 *
 * BUILD:
 *   gcc -c rotation.c -I../../.. -Wall -Wextra -std=c11
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_execution X.2 EXECUTION — Usage Patterns
 * @brief    How to use DAR rotation.
 *
 * USAGE:
 *   DARRotationState state;
 *   DARRotationConfig config = DAR_ROTATION_CONFIG_DEFAULT;
 *   config.file_prefix = "detect";
 *
 *   cpisi_dar_rotation_init(&state, &config);
 *
 *   FILE* f = cpisi_dar_rotation_get_file(&state);
 *   fprintf(f, "{...}\n");
 *   cpisi_dar_rotation_notify_write(&state, strlen("{...}\n"), 1);
 *
 *   cpisi_dar_rotation_shutdown(&state);
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership and Shutdown [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_cleanup X.3 CLEANUP — Ownership and Shutdown
 * @brief    Memory and resource ownership.
 *
 * OWNERSHIP:
 *   - DARRotationState: Caller owns, caller destroys
 *   - current_file: Owned by state, closed on shutdown
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
 * @defgroup cpisi_rotation_policy X.4 POLICY — Modification Guidelines
 * @brief    Rules for modifying this header.
 *
 * MODIFICATION RULES:
 *   1. Error codes MUST use STORAGE layer (162-188) from error-system.toml
 *   2. New errors MUST fit the tier structure (CORE/EXT/SYS)
 *   3. Config fields MUST use CPISI_* limits from config.h
 *   4. Time handling MUST use time_t (POSIX)
 *
 * ADDING NEW FEATURES:
 *   1. Check if feature belongs in rotation (not phase-specific)
 *   2. Add config field if configurable
 *   3. Add state field if runtime tracking needed
 *   4. Add function prototype in S.4
 *   5. Implement in rotation.c
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.4 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_extension X.5 EXTENSION — How to Expand
 * @brief    How to extend rotation functionality.
 *
 * IMPLEMENTED FEATURES:
 *
 *   Compression (S.4c.5):
 *     - DARCompressionBackend enum: NONE, GZIP, ZSTD
 *     - DARRotationConfig.compress_backend: Select algorithm
 *     - DARRotationConfig.compress_level: Algorithm-specific level
 *     - cpisi_dar_rotation_compress(): Compress files
 *     - cpisi_dar_rotation_decompress(): Decompress files
 *     - cpisi_dar_rotation_compress_ext(): Get extension for backend
 *
 *   Cross-Phase Aggregation (S.4c.6):
 *     - DARPhaseId enum: DETECT, ASSESS, RESTORE
 *     - DARRotationConfig.phase_id: Tag entries with source phase
 *     - DARRotationConfig.enable_aggregation: Enable unified streams
 *     - DARRotationConfig.aggregation_path: Shared output path
 *     - cpisi_dar_rotation_agg_init(): Initialize aggregation
 *     - cpisi_dar_rotation_agg_shutdown(): Shutdown aggregation
 *     - cpisi_dar_rotation_agg_write(): Write to unified stream
 *     - cpisi_dar_phase_id_name(): Get phase name string
 *
 *   Statistics (S.4c.6):
 *     - DARRotationState.stats_*: Cumulative metrics
 *     - cpisi_dar_rotation_stats(): Get statistics snapshot
 *
 * EXTENDING FURTHER:
 *
 *   Add new compression backend:
 *     1. Add value to DARCompressionBackend enum
 *     2. Update cpisi_dar_rotation_compress_ext()
 *     3. Implement in rotation.c compress/decompress functions
 *
 *   Add new rotation trigger:
 *     1. Add trigger field to DARRotationConfig
 *     2. Add needs_*_rotation() inline in S.4b
 *     3. Update cpisi_dar_rotation_check() implementation
 *
 *   Add new aggregation format:
 *     1. Add format enum value
 *     2. Update cpisi_dar_rotation_agg_write() implementation
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.5 EXTENSION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @brief    Solutions to common rotation issues.
 *
 * PROBLEM: Files not rotating
 *   CAUSE: max_file_size or max_entries set to 0 (disabled)
 *   FIX: Set non-zero values in DARRotationConfig
 *
 * PROBLEM: DAR_ROTATION_ERR_MKDIR (177)
 *   CAUSE: Cannot create dated folder structure
 *   FIX: Check base_path permissions, ensure parent exists
 *
 * PROBLEM: Sequence number overflow
 *   CAUSE: More than 999 files in one day
 *   FIX: Reduce max_entries, increase max_file_size
 *
 * PROBLEM: Old files not deleted
 *   CAUSE: cleanup() not called, or retention days too high
 *   FIX: Call cpisi_dar_rotation_cleanup() periodically
 *
 * PROBLEM: DAR_ROTATION_ERR_TIME (186)
 *   CAUSE: System time functions failed
 *   FIX: Verify system clock, check localtime() errors
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.6 TROUBLESHOOTING */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick reference for rotation API.
 *
 * TYPES:
 *   DARRotationConfig      — Configuration struct (S.3b)
 *   DARRotationState       — Runtime state struct (S.3b)
 *   DARCompressionBackend  — Compression algorithm enum (S.3a)
 *   DARPhaseId             — DAR phase identifier enum (S.3a)
 *
 * LIFECYCLE (S.4a):
 *   cpisi_dar_rotation_init()       — Initialize rotation → CpisiError
 *   cpisi_dar_rotation_shutdown()   — Clean shutdown → void
 *
 * FILE ACCESS (S.4c.1):
 *   cpisi_dar_rotation_get_file()     — Get FILE* for writing → FILE*
 *   cpisi_dar_rotation_notify_write() — Update tracking → void
 *
 * ROTATION CONTROL (S.4c.2):
 *   cpisi_dar_rotation_check()   — Check if rotation needed → bool
 *   cpisi_dar_rotation_rotate()  — Force rotation → CpisiError
 *
 * MAINTENANCE (S.4c.3):
 *   cpisi_dar_rotation_cleanup()  — Archive old, delete expired → int
 *   cpisi_dar_rotation_archive()  — Archive specific date → int
 *
 * PATH HELPERS (S.4c.4):
 *   cpisi_dar_rotation_build_path()  — Build dated path → int
 *   cpisi_dar_rotation_ensure_dir()  — Create directory → CpisiError
 *
 * COMPRESSION (S.4c.5):
 *   cpisi_dar_rotation_compress()      — Compress file → CpisiError
 *   cpisi_dar_rotation_decompress()    — Decompress file → CpisiError
 *   cpisi_dar_rotation_compress_ext()  — Get extension → const char*
 *
 * AGGREGATION (S.4c.6):
 *   cpisi_dar_rotation_agg_init()      — Init aggregation → CpisiError
 *   cpisi_dar_rotation_agg_shutdown()  — Shutdown aggregation → void
 *   cpisi_dar_rotation_agg_write()     — Write to unified stream → CpisiError
 *   cpisi_dar_phase_id_name()             — Get phase name → const char*
 *   cpisi_dar_rotation_stats()         — Get statistics → void
 *
 * VALIDATION (S.4b):
 *   cpisi_dar_rotation_config_valid()          — Validate config → bool
 *   cpisi_dar_rotation_state_valid()           — Validate state → bool
 *   cpisi_dar_rotation_has_file()              — Check file open → bool
 *   cpisi_dar_rotation_needs_size_rotation()   — Size trigger → bool
 *   cpisi_dar_rotation_needs_entry_rotation()  — Entry trigger → bool
 *
 * ERROR CODES (S.3a):
 *   CORE tier (162-170): NULL, INVALID, BOUNDS, TYPE, STATE, INIT, CLEANUP
 *   EXT tier (171-179):  OPEN, WRITE, READ, RENAME, DELETE, MKDIR, ACCESS, NOTFOUND
 *   SYS tier (180-188):  ROTATE, ARCHIVE, COMPRESS, RETENTION, SEQUENCE, TIME, SYNC
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_rotation_note X.8 NOTE — Final Guidance
 * @brief    Biblical grounding and final wisdom.
 *
 * SCRIPTURE:
 *   "And the books were opened: and another book was opened, which is the book
 *   of life: and the dead were judged out of those things which were written
 *   in the books, according to their works."
 *   — Revelation 20:12 KJV
 *
 * PRINCIPLE:
 *   Witness data is preserved, organized, and eventually archived. The books
 *   are opened. Nothing is lost. But old things pass away — retention policy
 *   ensures sustainable storage.
 *
 * ANCHOR:
 *   "To every thing there is a season, and a time to every purpose under heaven."
 *   — Ecclesiastes 3:1 KJV
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

#endif /* CPISI_DAR_ROTATION_H */
