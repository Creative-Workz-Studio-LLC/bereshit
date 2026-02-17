/// @file config.h
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
/// @omni:req  key        = CPISI-IO-CONFIG
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = b-03.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = io
/// @omni:inh  role       = config
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = io
/// @omni:ins  includes   = [types/config/log.h, kernel/types/composite/type/phase.type.h, dar/phases/assess.h]
/// @omni:ins  provides   = [CpisiFlags, CpisiConfig, CpisiConfigError, CPISI_CONFIG_DEFAULT, config_loading, env_overrides]
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
/// @omni:req  title      = CPI-SI Configuration Types and Loader
/// @omni:req  brief      = CpisiFlags, CpisiConfig types + TOML config loading
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
 * @defgroup cpisi_io_config_special SP.1 CONFIG — Compile-time Settings
 * @brief    Feature macros and environment configuration before includes.
 *
 * WHY: Settings that affect what headers provide must precede \#include.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * SP.1a POSIX — Feature Test [POSIX]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_io_config_special
 * @brief    Enables POSIX.1-2008 functions in standard headers.
 *
 * WHY: File operations, path handling, and POSIX APIs require this macro.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L  /**< POSIX.1-2008 — enables strdup, realpath, etc. */
#endif

/** @} */ /* end SP.1a POSIX */

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef CPISI_IO_CONFIG_H
/** @def CPISI_IO_CONFIG_H
 *  @brief Include guard — prevents multiple inclusion of this config header.
 */
#define CPISI_IO_CONFIG_H

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
 * @file      config.h
 * @brief     CPI-SI Configuration Types and Loader — CpisiFlags, CpisiConfig, TOML loading.
 *
 * @defgroup cpisi_io_config_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-IO-CONFIG
 * Title:     CPI-SI Configuration Types and Loader
 * Type:      Header (IO)
 * Component: Foundation — provides config types that cpisi.h uses
 * Role:      Configuration types (CpisiFlags, CpisiConfig) + TOML file loading
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   b-03.00
 * @date      2026-01-25
 *
 * Status:    Active
 * Created:   2026-01-20
 * Updated:   2026-01-25
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authors and Copyright [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_io_config_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/io/config.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Template:     bereshit/word/seed/code/c/header.h
 * Derives_from: types/config/log.h, kernel/types/composite/type/phase.type.h, dar/phases/assess.h
 *
 * Builds FROM:  log.h (LogLevel), phase.h (RestoreStrategy), assess.h (DebugLevel)
 * Builds TO:    cpisi.h (unified orchestrator uses CpisiConfig)
 *
 * Architecture: types/ → io/config.h → cpisi.h (correct ladder)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: io, config, types, toml, loader, flags, cpisi
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide configuration types and TOML loading for CPI-SI system.
 * Core Design: Types defined here, cpisi.h includes this (not the other way around).
 * Philosophy:  Configuration counted before execution begins.
 *
 * Key Features:
 *   - CpisiFlags — subsystem enable/disable flags
 *   - CpisiConfig — unified configuration struct
 *   - CPISI_CONFIG_DEFAULT — sensible default values
 *   - TOML file loading with fallback to defaults
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture:  Luke 14:28 — "For which of you, intending to build a tower,
 *             sitteth not down first, and counteth the cost"
 * Principle:  Configuration is counted before building begins.
 * Anchor:     Proverbs 24:27 — "Prepare thy work without"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: types/config/log.h (LogLevel)
 *   Internal: kernel/types/composite/type/phase.type.h (RestoreStrategy)
 *   Internal: dar/phases/assess.h (DebugLevel)
 *   Standard: stdbool.h, stddef.h
 *
 * What Uses This:
 *   - cpisi.h — unified orchestrator (uses CpisiConfig)
 *   - cpisi.c — implementation (uses loading functions)
 *
 * @see types/types.h for foundation types
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-20) — Initial creation
 *   a-02.00 (2026-01-24) — Types moved from cpisi.h (architecture fix)
 *   b-03.00 (2026-01-25) — Full header.h template alignment
 *
 * Current:
 *   ✓ CpisiFlags, CpisiConfig, CpisiConfigError types
 *   ✓ TOML loading functions
 *   ✓ Correct ladder chain (types → io/config → cpisi)
 *   ✓ Environment variable override support (CPISI_ENV_*)
 *   ✓ Config validation with detailed error reporting
 *
 * Planned:
 *   ⏳ Path environment expansion ($HOME, etc.)
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
/// @brief Configuration and declarations before body.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - S.1 INCLUDES  — Header dependencies
///   - S.2 DEFINES   — Path constants
///   - S.3 TYPES     — CpisiFlags, CpisiConfig
///   - S.4 PROTOS    — [Reserved]
///   - S.5 FILE-LEVEL — [Reserved]
///   - S.6 OMISSION  — What's excluded
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * PURPOSE: Establish all header dependencies in consistent order.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_io_config_includes
 * @brief    C standard library headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdbool.h>  /**< Boolean type: bool, true, false */
#include <stddef.h>   /**< Standard definitions: size_t, NULL */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_io_config_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "kernel/types/config/cfg/log.cfg.h"             /**< LogLevel, log config types */
#include "kernel/types/composite/type/phase.type.h" /**< RestoreStrategy, phase types */
#include "kernel/dar/phase/assess.phase.h"            /**< DebugLevel enum */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [Reserved: standard library only] @defgroup cpisi_io_config_external @ingroup cpisi_io_config_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_io_config_includes
 * @brief    C++ compatibility wrapper.
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
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Configuration path constants and default macro.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a PATHS — Config File Paths [PATHS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_paths S.2a PATHS — Config File Paths
 * @ingroup cpisi_io_config_defines
 * @brief    Standard configuration file name and search paths.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Standard config file name. */
#define CPISI_CONFIG_FILENAME       "cpisi_config.toml"

/** @brief Search path: current directory (development). */
#define CPISI_CONFIG_PATH_CWD       "."

/** @brief Search path: project config directory. */
#define CPISI_CONFIG_PATH_CONFIG    "./config"

/** @brief Search path: user config directory (XDG). */
#define CPISI_CONFIG_PATH_HOME      "$HOME/.config/cornerstone"

/** @brief Search path: system-wide config directory. */
#define CPISI_CONFIG_PATH_ETC       "/etc/cornerstone"

/** @} */ /* end S.2a PATHS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b ENVVARS — Environment Variable Overrides [ENVVARS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_envvars S.2b ENVVARS — Environment Variables
 * @ingroup cpisi_io_config_defines
 * @brief    Environment variable names for config overrides.
 *
 * WHY: Environment variables allow runtime configuration without modifying
 *      TOML files. Overrides take precedence over file values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/** @brief Override config file path. */
#define CPISI_ENV_CONFIG_FILE       "CPISI_CONFIG_FILE"

/** @brief Override log level (TRACE, DEBUG, INFO, WARN, ERROR, FATAL, CRISIS). */
#define CPISI_ENV_LOG_LEVEL         "CPISI_LOG_LEVEL"

/** @brief Override log file path. */
#define CPISI_ENV_LOG_FILE          "CPISI_LOG_FILE"

/** @brief Override debug level (OFF, BASIC, VERBOSE, FULL, TRACE). */
#define CPISI_ENV_DEBUG_LEVEL       "CPISI_DEBUG_LEVEL"

/** @brief Override restore strategy (NONE, MINIMAL, STANDARD, FULL, AGGRESSIVE). */
#define CPISI_ENV_RESTORE_STRATEGY  "CPISI_RESTORE_STRATEGY"

/** @brief Override subsystem flags (comma-separated: LOG,DEBUG,RESTORE). */
#define CPISI_ENV_FLAGS             "CPISI_FLAGS"

/** @} */ /* end S.2b ENVVARS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c DEFAULTS — Default Configuration Macro [DEFAULTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_defaults S.2c DEFAULTS — Default Configuration
 * @ingroup cpisi_io_config_defines
 * @brief    Default configuration values for CpisiConfig.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def CPISI_CONFIG_DEFAULT
 * @brief Default configuration initializer.
 *
 * Provides sensible defaults for CpisiConfig:
 *   - flags: CPISI_FLAG_FULL (all subsystems enabled)
 *   - log_level: LOG_INFO
 *   - debug_level: DEBUG_LEVEL_BASIC
 *   - restore_strategy: RESTORE_STRATEGY_FULL
 */
#define CPISI_CONFIG_DEFAULT { \
    .flags            = CPISI_FLAG_FULL, \
    .log_level        = LOG_INFO, \
    .log_file         = NULL, \
    .debug_level      = DEBUG_LEVEL_BASIC, \
    .break_on_assert  = true, \
    .break_on_error   = false, \
    .restore_strategy = RESTORE_STRATEGY_FULL, \
    .search_paths     = NULL, \
    .search_path_count = 0, \
}

/** @} */ /* end S.2c DEFAULTS */

/** S.2d FLAGS — [Reserved: uses CpisiFlags enum] @defgroup cpisi_io_config_s2flags @ingroup cpisi_io_config_defines @{ @} */
/** S.2e MACROS — [Reserved: no utility macros] @defgroup cpisi_io_config_macros @ingroup cpisi_io_config_defines @{ @} */
/** S.2f VERIFY — [Reserved: no static assertions] @defgroup cpisi_io_config_verify @ingroup cpisi_io_config_defines @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_types S.3 TYPES — Type Declarations
 * @brief    Configuration types: CpisiFlags enum, CpisiConfig struct.
 *
 * WHY HERE: Types defined in io/config.h so cpisi.h can include this.
 *           Fixes ladder chain: types → io/config → cpisi (correct direction).
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a ENUMS — CpisiFlags [ENUMS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_enums S.3a ENUMS — CpisiFlags
 * @ingroup cpisi_io_config_types
 * @brief    Subsystem enable/disable flags.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief CpisiFlags controls which subsystems are initialized.
 *
 * Bitfield flags for enabling/disabling CPI-SI subsystems.
 * Use CPISI_FLAG_FULL for all subsystems, or combine individual flags.
 *
 * @note CPISI_FLAG_NONE = 0 (no subsystems enabled)
 */
typedef enum CpisiFlags {
    CPISI_FLAG_NONE     = 0,          /**< No subsystems enabled. */
    CPISI_FLAG_LOG      = (1 << 0),   /**< Enable logging subsystem (DETECT). */
    CPISI_FLAG_DEBUG    = (1 << 1),   /**< Enable debug subsystem (ASSESS). */
    CPISI_FLAG_RESTORE  = (1 << 2),   /**< Enable restore subsystem (RESTORE). */

    /* Common combinations */
    CPISI_FLAG_MINIMAL  = CPISI_FLAG_LOG,                              /**< Logging only. */
    CPISI_FLAG_STANDARD = CPISI_FLAG_LOG | CPISI_FLAG_RESTORE,         /**< Log + Restore. */
    CPISI_FLAG_FULL     = CPISI_FLAG_LOG | CPISI_FLAG_DEBUG | CPISI_FLAG_RESTORE, /**< All subsystems. */
} CpisiFlags;

/** @} */ /* end S.3a ENUMS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b STRUCTS — CpisiConfig [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_structs S.3b STRUCTS — CpisiConfig
 * @ingroup cpisi_io_config_types
 * @brief    Unified configuration structure for CPI-SI initialization.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief CpisiConfig holds all configuration for CPI-SI initialization.
 *
 * Contains settings for all three DAR phase subsystems:
 *   - Log (DETECT phase)
 *   - Debug (ASSESS phase)
 *   - Restore (RESTORE phase)
 *
 * Use CPISI_CONFIG_DEFAULT for sensible defaults, or populate manually.
 *
 * @see cpisi_config_load_file() for TOML loading
 * @see cpisi_config_set_defaults() for programmatic defaults
 */
typedef struct CpisiConfig {
    /* Subsystem selection */
    CpisiFlags       flags;             /**< Which subsystems to enable. */

    /* Log configuration (DETECT phase) */
    LogLevel         log_level;         /**< Logging verbosity level. */
    const char*      log_file;          /**< Log file path, or NULL for stderr. */

    /* Debug configuration (ASSESS phase) */
    DebugLevel       debug_level;       /**< Debug output verbosity. */
    bool             break_on_assert;   /**< Break into debugger on assert. */
    bool             break_on_error;    /**< Break into debugger on error. */

    /* Restore configuration (RESTORE phase) */
    RestoreStrategy  restore_strategy;  /**< Recovery strategy selection. */
    const char**     search_paths;      /**< Config search paths, or NULL. */
    size_t           search_path_count; /**< Number of search paths. */

} CpisiConfig;

/** @} */ /* end S.3b STRUCTS */

/** S.3c OPAQUE — [Reserved: no opaque types] @defgroup cpisi_io_config_opaque @ingroup cpisi_io_config_types @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3d ERRORS — Configuration Error Codes [ERRORS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_errors S.3d ERRORS — Configuration Errors
 * @ingroup cpisi_io_config_types
 * @brief    Error codes for configuration operations.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Error codes for configuration loading and validation.
 *
 * Used by cpisi_config_load_file() and cpisi_config_validate() to report
 * specific error conditions. Enables detailed error reporting and recovery.
 */
typedef enum CpisiConfigError {
    CPISI_CONFIG_OK            =  0,  /**< Success — no error. */
    CPISI_CONFIG_ERR_NONE      =  0,  /**< Alias for OK. */

    /* File errors (1-9) */
    CPISI_CONFIG_ERR_NOT_FOUND =  1,  /**< Config file not found. */
    CPISI_CONFIG_ERR_READ      =  2,  /**< File read error. */
    CPISI_CONFIG_ERR_PARSE     =  3,  /**< TOML parse error. */
    CPISI_CONFIG_ERR_ACCESS    =  4,  /**< Permission denied. */

    /* Value errors (10-19) */
    CPISI_CONFIG_ERR_NULL      = 10,  /**< NULL config pointer. */
    CPISI_CONFIG_ERR_RANGE     = 11,  /**< Value out of range. */
    CPISI_CONFIG_ERR_TYPE      = 12,  /**< Type mismatch in TOML. */
    CPISI_CONFIG_ERR_MISSING   = 13,  /**< Required field missing. */

    /* Override errors (20-29) */
    CPISI_CONFIG_ERR_ENV_PARSE = 20,  /**< Environment variable parse error. */
    CPISI_CONFIG_ERR_ENV_RANGE = 21,  /**< Environment value out of range. */

    /* Validation warnings (negative = correctable) */
    CPISI_CONFIG_WARN_CLAMPED  = -1,  /**< Value clamped to valid range. */
    CPISI_CONFIG_WARN_DEFAULT  = -2,  /**< Using default value. */

} CpisiConfigError;

/** @} */ /* end S.3d ERRORS */

/** @} */ /* end S.3 TYPES */

/** S.4 PROTOTYPES — [Reserved: prototypes in BODY B.5] @defgroup cpisi_io_config_protos @{ @} */
/** S.5 FILE-LEVEL — [Reserved: no global state] @defgroup cpisi_io_config_file @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_s6_omission S.6 OMISSION — Excluded Content
 * @brief    Documents what SETUP sections are intentionally reserved.
 *
 * | Section | Status | Reason |
 * |---------|--------|--------|
 * | S.1c EXTERNAL | Reserved | Standard library only |
 * | S.2c-e | Reserved | No utility macros, static assertions |
 * | S.3c-d | Reserved | No opaque types, uses foundation errors |
 * | S.4 PROTOTYPES | Reserved | Function prototypes in BODY B.5 |
 * | S.5 FILE-LEVEL | Reserved | No global state needed |
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: S.6 is documentation-only] */

/** @} */ /* end S.6 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief The actual work — function declarations for config loading.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART   — Structure overview
///   - B.2 HELPERS     — [Reserved]
///   - B.3 CORE OPS    — [Reserved]
///   - B.4 ERRORS      — [Reserved]
///   - B.5 PUBLIC API  — Config loading functions
///   - B.6 OMISSION    — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_orgchart B.1 ORG CHART — Structure Overview
 * @brief    Map structure — types, functions, file organization.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Interface Overview [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_structure B.1a STRUCTURE — Interface Overview
 * @ingroup cpisi_io_config_orgchart
 * @brief    Types, functions, and externs declared in this header.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      2 enum (CpisiFlags, CpisiConfigError), 1 struct (CpisiConfig) */
/* Functions:  7 total (load, find, defaults, combined, validate, env_overrides, complete) */
/* Externs:    0 */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_flow B.1b FLOW — Implementation Location
 * @ingroup cpisi_io_config_orgchart
 * @brief    Where implementations live.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: cpisi/io/config.c */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_counts B.1c COUNTS — Summary Statistics
 * @ingroup cpisi_io_config_orgchart
 * @brief    Interface element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      3 total (CpisiFlags, CpisiConfig, CpisiConfigError) */
/* Functions:  7 total */
/* Externs:    0 total */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/** B.2 HELPERS — [Reserved: no helpers] @defgroup cpisi_io_config_helpers @{ @} */
/** B.3 CORE OPS — [Reserved: loading is the operation] @defgroup cpisi_io_config_ops @{ @} */
/** B.4 ERRORS — [Reserved: uses foundation errors] @defgroup cpisi_io_config_b4_errors @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Config Loading Functions [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_api B.5 PUBLIC API — Config Loading Functions
 * @brief    TOML configuration loading and path search functions.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a LOADING — File Loading Functions [LOADING]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_loading B.5a LOADING — File Loading
 * @ingroup cpisi_io_config_api
 * @brief    Load CpisiConfig from TOML files.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Load CpisiConfig from a TOML file.
 *
 * Parses the specified TOML file and populates the config struct.
 * Missing keys use defaults from CPISI_CONFIG_DEFAULT.
 *
 * @param[in]  path   Path to TOML config file.
 * @param[out] config Config struct to populate.
 *
 * @return 0 on success, -1 on failure (file not found or parse error).
 *
 * @par Example TOML
 * @code{.toml}
 * [cpisi]
 * flags = "FULL"
 *
 * [cpisi.log]
 * level = "INFO"
 * file = "/var/log/cornerstone/cpisi.log"
 *
 * [cpisi.debug]
 * level = "BASIC"
 * break_on_assert = true
 *
 * [cpisi.restore]
 * strategy = "FULL"
 * search_paths = ["./config", "/etc/cornerstone"]
 * @endcode
 */
int cpisi_config_load_file(const char* path, CpisiConfig* config);

/**
 * @brief Locate config file in standard paths.
 *
 * Searches for cpisi_config.toml in standard locations:
 *   1. ./cpisi_config.toml (current directory - development)
 *   2. ./config/cpisi_config.toml (project config dir)
 *   3. $HOME/.config/cornerstone/ (user config - XDG)
 *   4. /etc/cornerstone/ (system-wide)
 *
 * @param[out] found_path Buffer to store found path (>= CPISI_PATH_MAX bytes).
 * @param[in]  path_size  Size of buffer.
 *
 * @return 0 if found (path copied), -1 if not found.
 */
int cpisi_config_find_file(char* found_path, size_t path_size);

/** @} */ /* end B.5a LOADING */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b DEFAULTS — Default Configuration [DEFAULTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_defaults_fn B.5b DEFAULTS — Default Configuration
 * @ingroup cpisi_io_config_api
 * @brief    Set config to default values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Populate config with default values.
 *
 * Sets all fields to sensible defaults:
 *   - flags: CPISI_FLAG_FULL
 *   - log_level: LOG_INFO
 *   - debug_level: DEBUG_LEVEL_BASIC
 *   - restore_strategy: RESTORE_STRATEGY_FULL
 *
 * @param[out] config Config struct to populate. Must not be NULL.
 */
void cpisi_config_set_defaults(CpisiConfig* config);

/**
 * @brief Load from file or use defaults.
 *
 * Recommended entry point. Behavior:
 *   1. If config_path provided, try to load it
 *   2. If config_path is NULL, search standard paths
 *   3. If no file found, use defaults (still success)
 *   4. Only returns error on parse failure
 *
 * @param[in]  config_path Path to TOML file (or NULL to search).
 * @param[out] config      Config struct to populate.
 *
 * @return 0 = success (file loaded), 1 = success (defaults used), -1 = error.
 */
int cpisi_config_load_or_defaults(const char* config_path, CpisiConfig* config);

/** @} */ /* end B.5b DEFAULTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c VALIDATION — Config Validation [VALIDATION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_validation B.5c VALIDATION — Config Validation
 * @ingroup cpisi_io_config_api
 * @brief    Validate configuration values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Validate config values and clamp if needed.
 *
 * Checks all configuration values are within acceptable ranges.
 * Invalid values are clamped to valid ranges.
 *
 * @param[in,out] config Config struct to validate.
 *
 * @return 0 if valid, count of clamped/fixed values if corrections made.
 */
int cpisi_config_validate(CpisiConfig* config);

/** @} */ /* end B.5c VALIDATION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d OVERRIDES — Environment Variable Overrides [OVERRIDES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_io_config_overrides B.5d OVERRIDES — Environment Overrides
 * @ingroup cpisi_io_config_api
 * @brief    Apply environment variable overrides to config.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Apply environment variable overrides to config.
 *
 * Checks CPISI_ENV_* environment variables and applies any found
 * as overrides to the existing config values. Environment variables
 * take precedence over file-loaded values.
 *
 * Checks (in order):
 *   - CPISI_LOG_LEVEL: TRACE/DEBUG/INFO/WARN/ERROR/FATAL/CRISIS
 *   - CPISI_LOG_FILE: path string
 *   - CPISI_DEBUG_LEVEL: OFF/BASIC/VERBOSE/FULL/TRACE
 *   - CPISI_RESTORE_STRATEGY: NONE/MINIMAL/STANDARD/FULL/AGGRESSIVE
 *   - CPISI_FLAGS: comma-separated (LOG,DEBUG,RESTORE)
 *
 * @param[in,out] config Config struct to apply overrides to.
 * @param[out]    error  Optional error output (or NULL to ignore).
 *
 * @return Count of overrides applied (0 if none), negative on error.
 *
 * @par Example
 * @code{.c}
 * CpisiConfig config = CPISI_CONFIG_DEFAULT;
 * cpisi_config_load_file("config.toml", &config);  // Load from file
 * cpisi_config_apply_env_overrides(&config, NULL); // Apply env overrides
 * @endcode
 */
int cpisi_config_apply_env_overrides(CpisiConfig* config, CpisiConfigError* error);

/**
 * @brief Load config with file + environment overrides.
 *
 * Complete initialization flow:
 *   1. Set defaults
 *   2. Load from file (if found)
 *   3. Apply environment overrides
 *   4. Validate final config
 *
 * @param[in]  config_path Path to TOML file (or NULL to search).
 * @param[out] config      Config struct to populate.
 * @param[out] error       Optional error output (or NULL to ignore).
 *
 * @return 0 on success, negative on error.
 */
int cpisi_config_load_complete(const char* config_path, CpisiConfig* config,
                                CpisiConfigError* error);

/** @} */ /* end B.5d OVERRIDES */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_b6_omission B.6 OMISSION — Reserved BODY Sections
 * @brief    Documents what BODY sections are intentionally reserved.
 *
 * | Section | Status | Reason |
 * |---------|--------|--------|
 * | B.2 HELPERS | Reserved | No internal helpers needed |
 * | B.3 CORE OPS | Reserved | Loading IS the operation |
 * | B.4 ERRORS | Reserved | Uses foundation error types |
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: B.6 is documentation-only] */

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
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_x1_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build and syntax verification.
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -fsyntax-only -Wall -Wextra cpisi/io/config.h    # syntax check
 * cppcheck --enable=all cpisi/io/config.h              # static analysis
 * @endcode
 *
 * @par X.1b SELFTEST — Minimal Isolation Test
 * @code{.c}
 * #include "kernel/cpisi/io/config.h"
 * int main(void) { CpisiConfig c = CPISI_CONFIG_DEFAULT; (void)c; return 0; }
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Validation documented in defgroup above] */

/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_x2_execution X.2 EXECUTION — Usage Patterns
 * @brief    Include pattern and usage examples.
 *
 * @par X.2a INCLUDE — Include Pattern
 * @code{.c}
 * #include "kernel/cpisi/io/config.h"
 * @endcode
 *
 * @par X.2b USAGE — Loading Configuration
 * @code{.c}
 * CpisiConfig config;
 * int result = cpisi_config_load_or_defaults(NULL, &config);
 * if (result < 0) {
 *     fprintf(stderr, "Config parse error\n");
 *     return 1;
 * }
 * // result == 0: file loaded, result == 1: defaults used
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Execution documented in defgroup above] */

/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership Patterns [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_x3_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Ownership notes — config struct is caller-owned.
 *
 * OWNERSHIP:
 *   - CpisiConfig struct: Caller-owned (stack or heap)
 *   - log_file pointer: Caller retains ownership, must remain valid
 *   - search_paths array: Caller retains ownership, must remain valid
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Cleanup documented in defgroup above] */

/** @} */ /* end X.3 CLEANUP */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_x4_policy X.4 POLICY — Modification Guidance
 * @brief    Guide maintainers — modification safety levels.
 *
 * SAFE TO MODIFY:
 *   - Add new config fields to CpisiConfig
 *   - Add new CpisiFlags values
 *   - Add new loading/validation functions
 *
 * MODIFY WITH CARE:
 *   - CpisiConfig field types — affects all users
 *   - CPISI_CONFIG_DEFAULT — affects all default behavior
 *
 * NEVER MODIFY:
 *   - Ladder chain direction (this file BEFORE cpisi.h)
 *   - 4-block structure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Policy documented in defgroup above] */

/** @} */ /* end X.4 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Add Config [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_x5_extension X.5 EXTENSION — How to Add Config
 * @brief    How to add new configuration options.
 *
 * ADDING NEW CONFIG FIELD:
 *   1. Add field to CpisiConfig struct in S.3b
 *   2. Add default value to CPISI_CONFIG_DEFAULT in S.2b
 *   3. Update cpisi_config_load_file() to parse new field
 *   4. Update cpisi_config_validate() if constraints needed
 *   5. Update B.1c COUNTS
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Extension documented in defgroup above] */

/** @} */ /* end X.5 EXTENSION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOT — Common Problems [TROUBLESHOOT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_x6_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 *
 * "Undefined LogLevel/DebugLevel":
 *   → Ensure types/types.h is included before config.h
 *
 * "Config file not found":
 *   → Check CPISI_CONFIG_PATH_* search order
 *   → Use cpisi_config_load_or_defaults() for graceful fallback
 *
 * "Circular include with cpisi.h":
 *   → cpisi.h includes this, NOT the other way around
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Troubleshoot documented in defgroup above] */

/** @} */ /* end X.6 TROUBLESHOOT */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_x7_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick lookup — key config values and patterns.
 *
 * FLAG VALUES:
 *   CPISI_FLAG_NONE     = 0
 *   CPISI_FLAG_LOG      = 1
 *   CPISI_FLAG_DEBUG    = 2
 *   CPISI_FLAG_RESTORE  = 4
 *   CPISI_FLAG_FULL     = 7
 *
 * SEARCH ORDER:
 *   1. Current directory (./cpisi_config.toml)
 *   2. Config subdir (./config/cpisi_config.toml)
 *   3. User config ($HOME/.config/cornerstone/)
 *   4. System config (/etc/cornerstone/)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reference documented in defgroup above] */

/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_x8_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * Scripture: Luke 14:28
 *
 * This header provides the configuration foundation for CPI-SI.
 * Configuration is counted before building begins — the cost is
 * re-counted as circumstances change through TOML files.
 *
 * Architecture: types/types.h → io/config.h → cpisi.h
 * This file is FOUNDATION level — cpisi.h includes THIS.
 *
 * "For which of you, intending to build a tower, sitteth not down first,
 *  and counteth the cost, whether he have sufficient to finish it?"
 * — Luke 14:28
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Note documented in defgroup above] */

/** @} */ /* end X.8 NOTE */

/** X.9 TEMPLATE — [Reserved: instance file] @defgroup cpisi_io_config_x9_template @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 OMISSION — Reserved Sections [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_io_config_x10_omission X.10 OMISSION — Reserved Sections
 * @brief    CLOSING sections guidance.
 *
 * All CLOSING sections present except X.9 (template guidance not needed
 * for instance files).
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Omission documented in defgroup above] */

/** @} */ /* end X.10 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * C++ Linkage Closure
 * ───────────────────────────────────────────────────────────────────────────── */
#ifdef __cplusplus
}
#endif

#endif /* CPISI_IO_CONFIG_H */
