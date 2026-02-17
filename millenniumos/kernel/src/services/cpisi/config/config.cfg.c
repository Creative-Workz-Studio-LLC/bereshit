/// @file config.c
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
/// @omni:req  key        = CORNERSTONE-CORE-CPISI-CONFIG-IMPL
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = b-03.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = source
/// @omni:inh  subtype    = io
/// @omni:inh  role       = config
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = io/config
/// @omni:ins  includes   = [config.h, toml.h, stdlib.h, string.h, unistd.h, errno.h]
/// @omni:ins  provides   = [config_load, config_validate, env_overrides, load_complete]
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
/// @omni:req  title      = CPI-SI Configuration Loader Implementation
/// @omni:req  brief      = TOML config loading + environment overrides + validation
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
///   Feature macros and compile-time settings must be defined BEFORE #include.
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
 * @defgroup cpisi_config_special_config SP.1 CONFIG — Compile-time Settings
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
 * @defgroup cpisi_config_special_posix SP.1a POSIX — Feature Test
 * @ingroup cpisi_config_special_config
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
#define _POSIX_C_SOURCE 200809L  /**< POSIX.1-2008 — enables strdup, getenv, etc. */
#endif

/** @} */ /* end SP.1a POSIX */

/** @} */ /* end SP.1 CONFIG */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Identity and context for this component.
///
/// STRUCTURE: M.1-M.10 sections, grouped for readability.
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      config.c
 * @brief     CPI-SI Configuration Loader — TOML + env overrides + validation.
 *
 * @defgroup cpisi_config_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CORNERSTONE-CORE-CPISI-CONFIG-IMPL
 * Title:     CPI-SI Configuration Loader Implementation
 * Type:      Source (Library)
 * Component: Rails — provides config loading for CPI-SI orchestrator
 * Role:      Load TOML config, apply environment overrides, validate values
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_state M.2 STATE — Lifecycle State
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
 * M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_attribution M.3 ATTRIBUTION — Authorship & Rights
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
 * M.4 LOCATION — File Position [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_location M.4 LOCATION — File Position
 * @{
 *
 * Path: cornerstone/engine/core/src/cpisi/io/config.c
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Template Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_derivation M.5 DERIVATION — Template Lineage
 * @{
 *
 * Derives:   bereshit/word/seed/code/c/source.c (foundation template)
 * Header:    cpisi/io/config.h (declares 7 public functions)
 *
 * Builds FROM: types/domain/config/log.h (LogLevel, DebugLevel)
 *              types/domain/composite/phase.h (RestoreStrategy)
 *              toml.h (TOML parsing)
 *
 * Builds TO:   cpisi/cpisi.c (uses CpisiConfig for initialization)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_classification M.6 CLASSIFICATION — Categorization
 * @{
 *
 * Tags: cpisi, io, config, toml, environment, validation
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose Statement [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_intent M.7 INTENT — Purpose Statement
 * @{
 *
 * Purpose: Implement configuration loading for CPI-SI system
 *
 * Core Design: Load config from TOML file → Apply env overrides → Validate
 *
 * Key Features:
 *   - TOML file loading with fallback to defaults
 *   - Environment variable overrides (CPISI_ENV_*)
 *   - Config validation with detailed error codes
 *   - Complete initialization flow (file + env + validate)
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture: Luke 14:28 — "For which of you, intending to build a tower,
 *            sitteth not down first, and counteth the cost"
 *
 * Principle: Configuration is counted before execution begins.
 *            Proper preparation enables faithful execution.
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_dependencies M.9 DEPENDENCIES — Required Components
 * @{
 *
 * Depends:
 *   - cpisi/io/config.h — Public interface declarations
 *   - toml.h — TOML parsing functions
 *   - types/domain/config/log.h — LogLevel, DebugLevel enums
 *   - types/domain/composite/phase.h — RestoreStrategy enum
 *
 * Used by:
 *   - cpisi/cpisi.c — CPI-SI orchestrator uses config for init
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — Version History [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_roadmap M.10 ROADMAP — Version History
 * @{
 *
 * History:
 *   a-01.00 (2026-01-20) — Initial creation with basic TOML loading
 *   b-03.00 (2026-01-25) — Full template alignment, env overrides, validation
 *
 * Planned:
 *   - Array parsing for search_paths
 *   - Config file write-back support
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
///   - S.1 INCLUDES  — Header dependencies (Standard, Project, External)
///   - S.2 DEFINES   — Constants and macros
///   - S.3 TYPES     — Type declarations
///   - S.4 PROTOS    — Static function prototypes
///   - S.5 STATIC    — Static/file-scope variables
///   - S.6 OMISSION  — Reserved section guide
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
 * @defgroup cpisi_config_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * PURPOSE: Establish all header dependencies in consistent order.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers
 *   - S.1b PROJECT   — Project headers
 *   - S.1c EXTERNAL  — Third-party headers
 *
 * Builds FROM: SETUP block (configuration context)
 * Builds TO:   S.2 DEFINES (constants that may use included types)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

// Include kernel.h first to get CPISI_MODE
#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), config is not available.
// The full implementation requires hosted environment.
#if CPISI_MODE >= 2

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_config_includes
 * @brief    C standard library headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdlib.h>   /**< Memory: malloc, free, getenv, strtol              */
#include <string.h>   /**< Strings: memset, strlen, strncpy, strcmp          */
#include <strings.h>  /**< POSIX strings: strcasecmp                         */
#include <stdio.h>    /**< I/O: snprintf, fprintf                            */
#include <unistd.h>   /**< POSIX: access                                     */
#include <errno.h>    /**< Errors: errno for strtol parsing                  */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b PROJECT — Project Headers [PROJECT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_project S.1b PROJECT — Project Headers
 * @ingroup cpisi_config_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "services/cpisi/config/config.cfg.h"  /**< CpisiConfig, CpisiFlags, function declarations */

/** @} */ /* end S.1b PROJECT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_config_includes
 * @brief    External library headers with justification.
 *
 * JUSTIFICATION: toml.h provides TOML parsing which stdlib lacks.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "trit/toml.h"  /**< TOML parsing — stdlib lacks config format parsing */

/** @} */ /* end S.1c EXTERNAL */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Constants and macros for this component.
 *
 * PURPOSE: Define component-specific constants and utility macros.
 *
 * CONTAINS:
 *   - S.2a SIZES   — Buffer and array sizes
 *   - S.2b LIMITS  — Value bounds
 *   - S.2c FLAGS   — [Reserved]
 *   - S.2d MACROS  — [Reserved]
 *   - S.2e VERIFY  — [Reserved]
 *
 * Builds FROM: S.1 INCLUDES (types from included headers)
 * Builds TO:   S.3 TYPES (constants used in type definitions)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a SIZES — Buffer and Array Sizes [SIZES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_sizes S.2a SIZES — Buffer and Array Sizes
 * @ingroup cpisi_config_defines
 * @brief    Buffer sizes, array capacities, element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief 512: Maximum path buffer size for config files.
 *
 * @par Derivation
 *      CONFIG_PATH_MAX = 512 (covers typical Unix path lengths)
 */
#define CONFIG_PATH_MAX        512

/**
 * @brief 256: Maximum value buffer size for parsed strings.
 *
 * @par Derivation
 *      CONFIG_VALUE_MAX = 256 (covers typical config values)
 */
#define CONFIG_VALUE_MAX       256

/** @} */ /* end S.2a SIZES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b LIMITS — Value Bounds [LIMITS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_limits S.2b LIMITS — Value Bounds
 * @ingroup cpisi_config_defines
 * @brief    Minimum, maximum, and default values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Limits defined in header via CpisiConfig defaults] */

/** @} */ /* end S.2b LIMITS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c FLAGS — Boolean and Bit Flags [FLAGS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_flags S.2c FLAGS — Boolean and Bit Flags
 * @ingroup cpisi_config_defines
 * @brief    [Reserved: Flags defined in header]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: CpisiFlags defined in config.h] */

/** @} */ /* end S.2c FLAGS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d MACROS — Utility Macros [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_macros S.2d MACROS — Utility Macros
 * @ingroup cpisi_config_defines
 * @brief    [Reserved: No component-specific macros needed]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: No component-specific macros needed] */

/** @} */ /* end S.2d MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e VERIFY — Compile-time Assertions [VERIFY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_verify S.2e VERIFY — Compile-time Assertions
 * @ingroup cpisi_config_defines
 * @brief    [Reserved: Add assertions as configuration solidifies]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Add compile-time assertions as needed] */

/** @} */ /* end S.2e VERIFY */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_types S.3 TYPES — Type Declarations
 * @brief    File-local data structures and type definitions.
 *
 * PURPOSE: Internal types not exposed in headers.
 *
 * CONTAINS:
 *   - S.3a HEADER_TYPES — Types from header (documentation only)
 *   - S.3b INTERNAL     — Internal-only types
 *   - S.3c OPAQUE_IMPL  — [Reserved]
 *
 * Builds FROM: S.2 DEFINES (constants used in types)
 * Builds TO:   S.4 PROTOTYPES (types used in function signatures)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a HEADER TYPES — Types from Header [HEADER_TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_header_types S.3a HEADER TYPES — Types from Header
 * @ingroup cpisi_config_types
 * @brief    Types available from config.h — DO NOT REDEFINE.
 *
 * The following types are defined in config.h:
 *   - CpisiConfig      — Configuration structure
 *   - CpisiFlags       — Feature flag enumeration
 *   - CpisiConfigError — Error code enumeration
 *   - LogLevel         — Logging level (from log.h)
 *   - DebugLevel       — Debug verbosity (from assess.h)
 *   - RestoreStrategy  — Recovery strategy (from phase.h)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* All public types come from config.h — no redefinition needed */

/** @} */ /* end S.3a HEADER TYPES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b INTERNAL — Internal-Only Types [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_internal S.3b INTERNAL — Internal-Only Types
 * @ingroup cpisi_config_types
 * @brief    [Reserved: No internal types needed]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: No internal types needed for config loading] */

/** @} */ /* end S.3b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3c OPAQUE_IMPL — Opaque Type Implementations [OPAQUE_IMPL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_opaque_impl S.3c OPAQUE_IMPL — Opaque Type Implementations
 * @ingroup cpisi_config_types
 * @brief    [Reserved: No opaque types in this module]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: No opaque types — CpisiConfig is fully exposed in header] */

/** @} */ /* end S.3c OPAQUE_IMPL */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Internal Static Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_prototypes S.4 PROTOTYPES — Static Function Declarations
 * @brief    Forward declarations for INTERNAL static functions only.
 *
 * PURPOSE: Declare static helper functions before use (enables top-down reading).
 *
 * CONTAINS:
 *   - S.4a TOML_PARSERS    — TOML key/value parsing helpers
 *   - S.4b ENUM_PARSERS    — String-to-enum conversion helpers
 *
 * Builds FROM: S.3 TYPES (types used in signatures)
 * Builds TO:   S.5 FILE-LEVEL (infrastructure declarations)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a TOML_PARSERS — TOML Key/Value Parsing [TOML_PARSERS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_toml_parsers S.4a TOML_PARSERS — TOML Parsing Helpers
 * @ingroup cpisi_config_prototypes
 * @brief    Static functions for parsing TOML key/value pairs.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Parse boolean from TOML section.
 *
 * @param[in] toml        Raw TOML content string.
 * @param[in] section     Section name (e.g., "subsystems").
 * @param[in] key         Key name within section.
 * @param[in] default_val Value to return if key not found.
 *
 * @return Parsed boolean value, or default_val if not found/parse error.
 */
static bool parse_bool_key(const char* toml, const char* section,
                           const char* key, bool default_val);

/**
 * @brief Parse integer from TOML section.
 *
 * @param[in] toml        Raw TOML content string.
 * @param[in] section     Section name.
 * @param[in] key         Key name within section.
 * @param[in] default_val Value to return if key not found.
 *
 * @return Parsed integer value, or default_val if not found/parse error.
 */
static int32_t parse_int_key(const char* toml, const char* section,
                             const char* key, int32_t default_val);

/**
 * @brief Parse string from TOML section into provided buffer.
 *
 * @param[in]  toml        Raw TOML content string.
 * @param[in]  section     Section name.
 * @param[in]  key         Key name within section.
 * @param[out] out         Buffer to store parsed string.
 * @param[in]  max         Maximum buffer size.
 * @param[in]  default_val Default string if key not found (may be NULL).
 */
static void parse_string_key(const char* toml, const char* section,
                             const char* key, char* out, size_t max,
                             const char* default_val);

/** @} */ /* end S.4a TOML_PARSERS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ENUM_PARSERS — String-to-Enum Conversion [ENUM_PARSERS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_enum_parsers S.4b ENUM_PARSERS — Enum Parsing Helpers
 * @ingroup cpisi_config_prototypes
 * @brief    Static functions for parsing string to enum values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Parse LogLevel from string.
 *
 * @param[in] str  String to parse (case-insensitive).
 *
 * @return Matching LogLevel, or LOG_INFO if unrecognized.
 */
static LogLevel parse_log_level_string(const char* str);

/**
 * @brief Parse DebugLevel from string.
 *
 * @param[in] str  String to parse (case-insensitive).
 *
 * @return Matching DebugLevel, or DEBUG_LEVEL_BASIC if unrecognized.
 */
static DebugLevel parse_debug_level_string(const char* str);

/**
 * @brief Parse RestoreStrategy from string.
 *
 * @param[in] str  String to parse (case-insensitive).
 *
 * @return Matching RestoreStrategy flags, or RESTORE_STRATEGY_FULL if unrecognized.
 *
 * @note RestoreStrategy is bit flags. This parses named combinations
 *       (NONE, BASIC, FULL) or comma-separated individual flags.
 */
static RestoreStrategy parse_restore_strategy_string(const char* str);

/**
 * @brief Parse CpisiFlags from string.
 *
 * @param[in] str  String to parse (comma-separated or named combination).
 *
 * @return Matching CpisiFlags, or CPISI_FLAG_FULL if unrecognized.
 */
static CpisiFlags parse_flags_string(const char* str);

/** @} */ /* end S.4b ENUM_PARSERS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File Scope Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_file S.5 FILE-LEVEL — File Scope Declarations
 * @brief    File-level state and infrastructure.
 *
 * PURPOSE: Static variables for persistent string storage.
 *
 * CONTAINS:
 *   - S.5a VARIABLES      — Static variable declarations
 *   - S.5b FUNC_TYPES     — [Reserved]
 *   - S.5c INFRASTRUCTURE — [Reserved]
 *
 * Builds FROM: S.4 PROTOTYPES
 * Builds TO:   S.6 OMISSION
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5a VARIABLES — Static Variables [VARIABLES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_vars S.5a VARIABLES — Static Variables
 * @ingroup cpisi_config_file
 * @brief    File-scope variable declarations.
 *
 * PURPOSE: Static buffers for string values that outlive toml_free().
 *          CpisiConfig.log_file points to these buffers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Static buffer for log file path.
 *
 * Persists after toml_free() so config->log_file remains valid.
 * Thread-safety: Not thread-safe — single config load per process.
 */
static char s_log_file_path[CONFIG_PATH_MAX] = "";

/**
 * @brief Static buffer for structured log file path.
 *
 * Reserved for future structured logging support.
 */
static char s_structured_file_path[CONFIG_PATH_MAX] = "";

/** @} */ /* end S.5a VARIABLES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5b FUNC_TYPES — Callback Typedefs [FUNC_TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_func_types S.5b FUNC_TYPES — Callback Typedefs
 * @ingroup cpisi_config_file
 * @brief    [Reserved: No callbacks in config module]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: No callbacks needed for config loading] */

/** @} */ /* end S.5b FUNC_TYPES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5c INFRASTRUCTURE — Logger/Inspector [INFRASTRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_infra S.5c INFRASTRUCTURE — Logger/Inspector
 * @ingroup cpisi_config_file
 * @brief    [Reserved: Config loads before logger is initialized]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: Config is loaded before logger — cannot use logger here] */

/** @} */ /* end S.5c INFRASTRUCTURE */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_omission S.6 OMISSION — Reserved Section Guide
 * @brief    Documents what sections are reserved and why.
 *
 * Reserved Sections:
 *   - S.2c FLAGS:      CpisiFlags defined in header
 *   - S.2d MACROS:     No component-specific macros needed
 *   - S.2e VERIFY:     Add assertions as config solidifies
 *   - S.3b INTERNAL:   No internal types needed
 *   - S.3c OPAQUE:     CpisiConfig is fully exposed
 *   - S.5b FUNC_TYPES: No callbacks in config
 *   - S.5c INFRA:      Config loads before logger
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief The actual work — implementations.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART    — Ladder hierarchy, baton flow, APU counts
///   - B.2 HELPERS      — Foundation implementations (TOML parsing, enum parsing)
///   - B.3 CORE OPS     — Business logic (defaults, validation, loading)
///   - B.4 ERRORS       — [Reserved: Errors via return codes]
///   - B.5 PUBLIC API   — Exported interface implementations
///   - B.6 OMISSION     — Reserved section guide
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Ladder Hierarchy [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_orgchart B.1 ORG CHART — Ladder Hierarchy
 * @brief    Map internal structure — ladder hierarchy, baton flow, APU counts.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Ladder Hierarchy [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_orgchart_structure B.1a STRUCTURE — Ladder Hierarchy
 * @ingroup cpisi_config_orgchart
 * @brief    Function call hierarchy — who calls whom.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/*
 * Public APIs (Top Rungs - Orchestration)
 * ├── cpisi_config_load_complete()
 * │     ├── cpisi_config_set_defaults()
 * │     ├── cpisi_config_load_file()
 * │     ├── cpisi_config_apply_env_overrides()
 * │     └── cpisi_config_validate()
 * ├── cpisi_config_load_or_defaults()
 * │     ├── cpisi_config_find_file()
 * │     ├── cpisi_config_load_file()
 * │     └── cpisi_config_set_defaults()
 * └── cpisi_config_apply_env_overrides()
 *       └── parse_*_string() helpers
 *
 * Core Operations (Middle Rungs)
 * ├── cpisi_config_load_file() → parse_*_key() helpers
 * ├── cpisi_config_find_file() → access()
 * └── cpisi_config_validate()  → value clamping
 *
 * Helpers (Bottom Rungs - Foundations)
 * ├── parse_bool_key()
 * ├── parse_int_key()
 * ├── parse_string_key()
 * ├── parse_log_level_string()
 * ├── parse_debug_level_string()
 * ├── parse_restore_strategy_string()
 * └── parse_flags_string()
 */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Baton Execution Path [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_orgchart_flow B.1b FLOW — Baton Execution Path
 * @ingroup cpisi_config_orgchart
 * @brief    How execution flows through the file.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/*
 * Entry → cpisi_config_load_complete()
 *   ↓
 * cpisi_config_set_defaults()
 *   ↓
 * cpisi_config_find_file() → cpisi_config_load_file()
 *   ↓
 * cpisi_config_apply_env_overrides()
 *   ↓
 * cpisi_config_validate()
 *   ↓
 * Exit → return CpisiConfigError
 */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — APU Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_orgchart_counts B.1c COUNTS — APU Statistics
 * @ingroup cpisi_config_orgchart
 * @brief    Atomic Processing Unit counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Functions:       14 total                                */
/* Helpers:          7 (TOML and enum parsing)              */
/* Core Operations:  0 (no mid-level beyond helpers)        */
/* Public APIs:      7 (all declared in config.h)           */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Foundation Implementations [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_helpers B.2 HELPERS — Foundation Implementations
 * @brief    Foundation function implementations — building blocks.
 *
 * CONTAINS:
 *   - B.2a TOML_PARSERS — TOML key/value parsing implementations
 *   - B.2b ENUM_PARSERS — String-to-enum implementations
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a TOML_PARSERS — TOML Parsing Implementations [TOML_PARSERS_IMPL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_toml_impl B.2a TOML_PARSERS — TOML Parsing Implementations
 * @ingroup cpisi_config_helpers
 * @brief    TOML key/value parsing implementations.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Parse boolean from TOML section.
 *
 * Searches for key within [section] and parses as boolean.
 * Uses toml_find_key_in_section() and toml_parse_bool().
 *
 * @param[in] toml        Raw TOML content string.
 * @param[in] section     Section name (e.g., "subsystems").
 * @param[in] key         Key name within section.
 * @param[in] default_val Value to return if key not found.
 *
 * @return Parsed boolean value, or default_val if not found/parse error.
 */
static bool parse_bool_key(const char* toml, const char* section,
                           const char* key, bool default_val) {
    const char* val = toml_find_key_in_section(toml, section, key);
    if (!val) {
        return default_val;
    }

    bool result;
    if (toml_parse_bool(val, &result)) {
        return result;
    }
    return default_val;
}

/**
 * @brief Parse integer from TOML section.
 *
 * Searches for key within [section] and parses as int32_t.
 *
 * @param[in] toml        Raw TOML content string.
 * @param[in] section     Section name.
 * @param[in] key         Key name within section.
 * @param[in] default_val Value to return if key not found.
 *
 * @return Parsed integer value, or default_val if not found/parse error.
 */
static int32_t parse_int_key(const char* toml, const char* section,
                             const char* key, int32_t default_val) {
    const char* val = toml_find_key_in_section(toml, section, key);
    if (!val) {
        return default_val;
    }

    int32_t result;
    if (toml_parse_int(val, &result)) {
        return result;
    }
    return default_val;
}

/**
 * @brief Parse string from TOML section into provided buffer.
 *
 * Searches for key within [section] and copies string value to buffer.
 * Ensures null-termination within max bytes.
 *
 * @param[in]  toml        Raw TOML content string.
 * @param[in]  section     Section name.
 * @param[in]  key         Key name within section.
 * @param[out] out         Buffer to store parsed string.
 * @param[in]  max         Maximum buffer size.
 * @param[in]  default_val Default string if key not found (may be NULL).
 */
static void parse_string_key(const char* toml, const char* section,
                             const char* key, char* out, size_t max,
                             const char* default_val) {
    const char* val = toml_find_key_in_section(toml, section, key);
    if (!val) {
        if (default_val) {
            strncpy(out, default_val, max - 1);
            out[max - 1] = '\0';
        } else {
            out[0] = '\0';
        }
        return;
    }

    if (!toml_parse_string(val, out, max)) {
        /* Parse failed, use default */
        if (default_val) {
            strncpy(out, default_val, max - 1);
            out[max - 1] = '\0';
        } else {
            out[0] = '\0';
        }
    }
}

/** @} */ /* end B.2a TOML_PARSERS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b ENUM_PARSERS — String-to-Enum Implementations [ENUM_PARSERS_IMPL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_enum_impl B.2b ENUM_PARSERS — Enum Parsing Implementations
 * @ingroup cpisi_config_helpers
 * @brief    String-to-enum conversion implementations.
 *
 * Used for parsing environment variable values and string-based config values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Parse LogLevel from string.
 *
 * Case-insensitive matching. Returns LOG_INFO for unrecognized values.
 *
 * @param[in] str  String to parse (e.g., "DEBUG", "warn", "Error").
 *
 * @return Matching LogLevel enum value.
 *
 * @par Supported Values
 *   TRACE, DEBUG, INFO, WARN, ERROR, FATAL, CRISIS
 */
static LogLevel parse_log_level_string(const char* str) {
    if (!str || str[0] == '\0') {
        return LOG_INFO;
    }

    if (strcasecmp(str, "TRACE") == 0)  return LOG_TRACE;
    if (strcasecmp(str, "DEBUG") == 0)  return LOG_DEBUG;
    if (strcasecmp(str, "INFO") == 0)   return LOG_INFO;
    if (strcasecmp(str, "WARN") == 0)   return LOG_WARN;
    if (strcasecmp(str, "ERROR") == 0)  return LOG_ERROR;
    if (strcasecmp(str, "FATAL") == 0)  return LOG_FATAL;
    if (strcasecmp(str, "CRISIS") == 0) return LOG_CRISIS;

    return LOG_INFO;  /* Default for unrecognized */
}

/**
 * @brief Parse DebugLevel from string.
 *
 * Case-insensitive matching. Returns DEBUG_LEVEL_BASIC for unrecognized.
 *
 * @param[in] str  String to parse (e.g., "OFF", "verbose", "Full").
 *
 * @return Matching DebugLevel enum value.
 *
 * @par Supported Values
 *   OFF, BASIC, VERBOSE, FULL, TRACE
 */
static DebugLevel parse_debug_level_string(const char* str) {
    if (!str || str[0] == '\0') {
        return DEBUG_LEVEL_BASIC;
    }

    if (strcasecmp(str, "OFF") == 0)     return DEBUG_LEVEL_OFF;
    if (strcasecmp(str, "BASIC") == 0)   return DEBUG_LEVEL_BASIC;
    if (strcasecmp(str, "VERBOSE") == 0) return DEBUG_LEVEL_VERBOSE;
    if (strcasecmp(str, "FULL") == 0)    return DEBUG_LEVEL_FULL;
    if (strcasecmp(str, "TRACE") == 0)   return DEBUG_LEVEL_TRACE;

    return DEBUG_LEVEL_BASIC;  /* Default for unrecognized */
}

/**
 * @brief Parse RestoreStrategy from string.
 *
 * RestoreStrategy is bit flags. This parses:
 *   - Named combinations: NONE, BASIC, FULL
 *   - Comma-separated individual flags: FILL_DEFAULTS, LOCATE_FILES, etc.
 *
 * @param[in] str  String to parse.
 *
 * @return Matching RestoreStrategy bit flags.
 *
 * @par Named Combinations
 *   - NONE: No recovery
 *   - BASIC: FILL_DEFAULTS | CLAMP_VALUES
 *   - FULL: All flags enabled
 *
 * @par Individual Flags
 *   FILL_DEFAULTS, LOCATE_FILES, CREATE_DIRS, CLAMP_VALUES, FALLBACK, DEGRADE
 */
static RestoreStrategy parse_restore_strategy_string(const char* str) {
    if (!str || str[0] == '\0') {
        return RESTORE_STRATEGY_FULL;
    }

    /* Handle named combinations first */
    if (strcasecmp(str, "NONE") == 0)  return RESTORE_STRATEGY_NONE;
    if (strcasecmp(str, "BASIC") == 0) return RESTORE_STRATEGY_BASIC;
    if (strcasecmp(str, "FULL") == 0)  return RESTORE_STRATEGY_FULL;

    /* Parse comma-separated flags */
    RestoreStrategy flags = RESTORE_STRATEGY_NONE;
    char buf[CONFIG_VALUE_MAX];
    strncpy(buf, str, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char* token = strtok(buf, ",");
    while (token) {
        /* Trim leading whitespace */
        while (*token == ' ') token++;
        /* Trim trailing whitespace */
        char* end = token + strlen(token) - 1;
        while (end > token && *end == ' ') *end-- = '\0';

        if (strcasecmp(token, "FILL_DEFAULTS") == 0) flags |= RESTORE_STRATEGY_FILL_DEFAULTS;
        if (strcasecmp(token, "LOCATE_FILES") == 0)  flags |= RESTORE_STRATEGY_LOCATE_FILES;
        if (strcasecmp(token, "CREATE_DIRS") == 0)   flags |= RESTORE_STRATEGY_CREATE_DIRS;
        if (strcasecmp(token, "CLAMP_VALUES") == 0)  flags |= RESTORE_STRATEGY_CLAMP_VALUES;
        if (strcasecmp(token, "FALLBACK") == 0)      flags |= RESTORE_STRATEGY_FALLBACK;
        if (strcasecmp(token, "DEGRADE") == 0)       flags |= RESTORE_STRATEGY_DEGRADE;

        token = strtok(NULL, ",");
    }

    return flags;
}

/**
 * @brief Parse CpisiFlags from string.
 *
 * Parses named combinations or comma-separated individual flags.
 *
 * @param[in] str  String to parse.
 *
 * @return Matching CpisiFlags bit flags.
 *
 * @par Named Combinations
 *   - NONE: No subsystems enabled
 *   - MINIMAL: LOG only
 *   - STANDARD: LOG | DEBUG
 *   - FULL: LOG | DEBUG | RESTORE
 *
 * @par Individual Flags
 *   LOG, DEBUG, RESTORE
 */
static CpisiFlags parse_flags_string(const char* str) {
    if (!str || str[0] == '\0') {
        return CPISI_FLAG_FULL;
    }

    /* Handle named combinations first */
    if (strcasecmp(str, "NONE") == 0)     return CPISI_FLAG_NONE;
    if (strcasecmp(str, "MINIMAL") == 0)  return CPISI_FLAG_MINIMAL;
    if (strcasecmp(str, "STANDARD") == 0) return CPISI_FLAG_STANDARD;
    if (strcasecmp(str, "FULL") == 0)     return CPISI_FLAG_FULL;

    /* Parse comma-separated flags */
    CpisiFlags flags = CPISI_FLAG_NONE;
    char buf[CONFIG_VALUE_MAX];
    strncpy(buf, str, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char* token = strtok(buf, ",");
    while (token) {
        /* Trim whitespace */
        while (*token == ' ') token++;
        char* end = token + strlen(token) - 1;
        while (end > token && *end == ' ') *end-- = '\0';

        if (strcasecmp(token, "LOG") == 0)     flags |= CPISI_FLAG_LOG;
        if (strcasecmp(token, "DEBUG") == 0)   flags |= CPISI_FLAG_DEBUG;
        if (strcasecmp(token, "RESTORE") == 0) flags |= CPISI_FLAG_RESTORE;

        token = strtok(NULL, ",");
    }

    return flags;
}

/** @} */ /* end B.2b ENUM_PARSERS */

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Business Logic Implementations [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_coreops B.3 CORE OPS — Business Logic Implementations
 * @brief    [Reserved: All logic is in B.5 Public API for this module]
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: Config module has helpers (B.2) and public API (B.5) */
/*  with no mid-level core operations between them]                */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Error Handling Patterns [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_errors_impl B.4 ERRORS — Error Handling Patterns
 * @brief    [Reserved: Errors returned via CpisiConfigError enum]
 *
 * Error Patterns:
 *   - NULL pointer: Return -1 immediately, set *error = CPISI_CONFIG_ERR_NULL
 *   - File not found: Return -1, set *error = CPISI_CONFIG_ERR_FILE
 *   - Parse error: Use defaults, set *error = CPISI_CONFIG_WARN_DEFAULT
 *   - Validation fix: Clamp value, set *error = CPISI_CONFIG_WARN_CLAMPED
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Exported Interface Implementations [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_api B.5 PUBLIC API — Exported Interface Implementations
 * @brief    Public API implementations matching header declarations.
 *
 * CONTAINS:
 *   - B.5a DEFAULTS   — Default configuration
 *   - B.5b VALIDATION — Config validation
 *   - B.5c LOADING    — TOML file loading
 *   - B.5d PATHS      — Config file discovery
 *   - B.5e ENV        — Environment variable overrides
 *   - B.5f COMPLETE   — Full initialization flow
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a DEFAULTS — Default Configuration [DEFAULTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_api_defaults B.5a DEFAULTS — Default Configuration
 * @ingroup cpisi_config_api
 * @brief    Set configuration to safe defaults.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Set configuration to safe defaults.
 *
 * Initializes all CpisiConfig fields to sensible defaults.
 * Call before loading from file to ensure all fields have values.
 *
 * @param[out] config  Configuration struct to initialize. Must not be NULL.
 *
 * @par Default Values
 *   - flags: CPISI_FLAG_FULL (all subsystems enabled)
 *   - log_level: LOG_INFO
 *   - log_file: NULL (stdout)
 *   - debug_level: DEBUG_LEVEL_BASIC
 *   - break_on_assert: true
 *   - break_on_error: false
 *   - restore_strategy: RESTORE_STRATEGY_FULL
 *   - search_paths: NULL
 *   - search_path_count: 0
 *
 * @see CPISI_CONFIG_DEFAULT macro in config.h
 */
void cpisi_config_set_defaults(CpisiConfig* config) {
    if (!config) {
        return;
    }

    config->flags            = CPISI_FLAG_FULL;
    config->log_level        = LOG_INFO;
    config->log_file         = NULL;
    config->debug_level      = DEBUG_LEVEL_BASIC;
    config->break_on_assert  = true;
    config->break_on_error   = false;
    config->restore_strategy = RESTORE_STRATEGY_FULL;
    config->search_paths     = NULL;
    config->search_path_count = 0;
}

/** @} */ /* end B.5a DEFAULTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b VALIDATION — Config Validation [VALIDATION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_api_validation B.5b VALIDATION — Config Validation
 * @ingroup cpisi_config_api
 * @brief    Validate and clamp configuration values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Validate configuration values and clamp if out of range.
 *
 * Checks all configuration values are within valid ranges.
 * Out-of-range values are clamped to valid defaults.
 *
 * @param[in,out] config  Configuration to validate. Must not be NULL.
 *
 * @return Number of corrections made (0 = all valid), or -1 if config is NULL.
 *
 * @par Validation Rules
 *   - log_level: LOG_TRACE to LOG_CRISIS → default LOG_INFO
 *   - debug_level: DEBUG_LEVEL_OFF to DEBUG_LEVEL_TRACE → default DEBUG_LEVEL_BASIC
 *   - restore_strategy: Valid bit flags only (mask with valid bits)
 *   - flags: Valid bit flags only (mask with LOG|DEBUG|RESTORE)
 */
int cpisi_config_validate(CpisiConfig* config) {
    if (!config) {
        return -1;
    }

    int corrections = 0;

    /* Validate log_level (0-6) */
    if (config->log_level < LOG_TRACE || config->log_level > LOG_CRISIS) {
        config->log_level = LOG_INFO;
        corrections++;
    }

    /* Validate debug_level (0-4) */
    if (config->debug_level < DEBUG_LEVEL_OFF || config->debug_level > DEBUG_LEVEL_TRACE) {
        config->debug_level = DEBUG_LEVEL_BASIC;
        corrections++;
    }

    /* Validate restore_strategy (bit flags) */
    RestoreStrategy valid_restore_mask =
        RESTORE_STRATEGY_FILL_DEFAULTS |
        RESTORE_STRATEGY_LOCATE_FILES |
        RESTORE_STRATEGY_CREATE_DIRS |
        RESTORE_STRATEGY_CLAMP_VALUES |
        RESTORE_STRATEGY_FALLBACK |
        RESTORE_STRATEGY_DEGRADE;

    if (config->restore_strategy & ~valid_restore_mask) {
        config->restore_strategy &= valid_restore_mask;  /* Clear invalid bits */
        corrections++;
    }

    /* Validate flags (bit flags) */
    CpisiFlags valid_flags_mask = CPISI_FLAG_LOG | CPISI_FLAG_DEBUG | CPISI_FLAG_RESTORE;
    if (config->flags & ~valid_flags_mask) {
        config->flags &= valid_flags_mask;  /* Clear invalid bits */
        corrections++;
    }

    return corrections;
}

/** @} */ /* end B.5b VALIDATION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c LOADING — TOML File Loading [LOADING]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_api_loading B.5c LOADING — TOML File Loading
 * @ingroup cpisi_config_api
 * @brief    Load configuration from TOML file.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Load configuration from TOML file.
 *
 * Parses TOML file and populates CpisiConfig struct.
 * Starts with defaults, then overrides with file values.
 *
 * @param[in]  path    Path to TOML config file. Must not be NULL.
 * @param[out] config  Configuration struct to populate. Must not be NULL.
 *
 * @return 0 on success, -1 on error (file not found, parse error).
 *
 * @par TOML Sections Parsed
 *   - [subsystems]: log, debug, restore (booleans → flags)
 *   - [log]: min_level, file, structured_file
 *   - [debug]: level, break_on_assert, break_on_error
 *   - [restore]: strategy
 */
int cpisi_config_load_file(const char* path, CpisiConfig* config) {
    if (!path || !config) {
        return -1;
    }

    /* Load TOML file */
    char* toml = toml_load_file(path);
    if (!toml) {
        return -1;
    }

    /* Start with defaults */
    cpisi_config_set_defaults(config);

    /* [subsystems] section — parse flag booleans */
    bool log_enabled     = parse_bool_key(toml, "subsystems", "log", true);
    bool debug_enabled   = parse_bool_key(toml, "subsystems", "debug", true);
    bool restore_enabled = parse_bool_key(toml, "subsystems", "restore", true);

    config->flags = CPISI_FLAG_NONE;
    if (log_enabled)     config->flags |= CPISI_FLAG_LOG;
    if (debug_enabled)   config->flags |= CPISI_FLAG_DEBUG;
    if (restore_enabled) config->flags |= CPISI_FLAG_RESTORE;

    /* [log] section */
    config->log_level = (LogLevel)parse_int_key(toml, "log", "min_level", LOG_INFO);

    /* Parse log file path (uses static buffer) */
    parse_string_key(toml, "log", "file", s_log_file_path,
                     sizeof(s_log_file_path), "");
    config->log_file = (s_log_file_path[0] != '\0') ? s_log_file_path : NULL;

    /* Parse structured log file path (for future use) */
    parse_string_key(toml, "log", "structured_file", s_structured_file_path,
                     sizeof(s_structured_file_path), "");

    /* [debug] section */
    config->debug_level     = (DebugLevel)parse_int_key(toml, "debug", "level",
                                                        DEBUG_LEVEL_BASIC);
    config->break_on_assert = parse_bool_key(toml, "debug", "break_on_assert", true);
    config->break_on_error  = parse_bool_key(toml, "debug", "break_on_error", false);

    /* [restore] section */
    config->restore_strategy = (RestoreStrategy)parse_int_key(toml, "restore",
                                                              "strategy",
                                                              RESTORE_STRATEGY_FULL);

    /* Note: search_paths would need array parsing — not implemented yet */
    config->search_paths = NULL;
    config->search_path_count = 0;

    /* Free TOML content */
    toml_free(toml);

    return 0;
}

/** @} */ /* end B.5c LOADING */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d PATHS — Config File Discovery [PATHS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_api_paths B.5d PATHS — Config File Discovery
 * @ingroup cpisi_config_api
 * @brief    Search standard paths for config file.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Search standard paths for config file.
 *
 * Searches a list of standard locations for cpisi_config.toml.
 * Returns the first found path.
 *
 * @param[out] found_path  Buffer to store found path. Must be at least 256 bytes.
 * @param[in]  path_size   Size of found_path buffer.
 *
 * @return 0 if found (path stored in found_path), -1 if not found.
 *
 * @par Search Order
 *   1. ./cpisi_config.toml
 *   2. ./config/cpisi_config.toml
 *   3. ../config/cpisi_config.toml
 *   4. $HOME/.config/cornerstone/cpisi_config.toml
 *   5. /etc/cornerstone/cpisi_config.toml
 */
int cpisi_config_find_file(char* found_path, size_t path_size) {
    if (!found_path || path_size < 256) {
        return -1;
    }

    /* Standard search paths */
    const char* paths[] = {
        "./cpisi_config.toml",
        "./config/cpisi_config.toml",
        "../config/cpisi_config.toml",
        NULL  /* HOME/.config/cornerstone/cpisi_config.toml handled below */
    };

    /* Check each standard path */
    for (int i = 0; paths[i] != NULL; i++) {
        if (access(paths[i], R_OK) == 0) {
            strncpy(found_path, paths[i], path_size - 1);
            found_path[path_size - 1] = '\0';
            return 0;
        }
    }

    /* Check $HOME/.config/cornerstone/ */
    const char* home = getenv("HOME");
    if (home) {
        snprintf(found_path, path_size,
                 "%s/.config/cornerstone/cpisi_config.toml", home);
        if (access(found_path, R_OK) == 0) {
            return 0;
        }
    }

    /* Check /etc/cornerstone/ */
    snprintf(found_path, path_size, "/etc/cornerstone/cpisi_config.toml");
    if (access(found_path, R_OK) == 0) {
        return 0;
    }

    /* Not found */
    found_path[0] = '\0';
    return -1;
}

/**
 * @brief Load config from file or use defaults.
 *
 * Attempts to load from specified path, then searches standard paths,
 * falling back to defaults if no config file found.
 *
 * @param[in]  config_path  Optional path to config file (NULL to search).
 * @param[out] config       Configuration struct to populate.
 *
 * @return 0 if loaded from file, 1 if using defaults, -1 on error.
 */
int cpisi_config_load_or_defaults(const char* config_path, CpisiConfig* config) {
    if (!config) {
        return -1;
    }

    /* If path provided, try to load it */
    if (config_path) {
        if (cpisi_config_load_file(config_path, config) == 0) {
            return 0;  /* Loaded from specified file */
        }
        /* Fall through to search */
    }

    /* Search standard paths */
    char found_path[CONFIG_PATH_MAX];
    if (cpisi_config_find_file(found_path, sizeof(found_path)) == 0) {
        if (cpisi_config_load_file(found_path, config) == 0) {
            return 0;  /* Loaded from found file */
        }
    }

    /* Use defaults */
    cpisi_config_set_defaults(config);
    return 1;  /* Defaults used */
}

/** @} */ /* end B.5d PATHS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5e ENV — Environment Variable Overrides [ENV]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_api_env B.5e ENV — Environment Variable Overrides
 * @ingroup cpisi_config_api
 * @brief    Apply environment variable overrides to config.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Apply environment variable overrides to configuration.
 *
 * Checks for CPISI_* environment variables and overrides matching
 * config values. Enables runtime configuration without modifying files.
 *
 * @param[in,out] config  Configuration to modify. Must not be NULL.
 * @param[out]    error   Error code output (may be NULL).
 *
 * @return Number of overrides applied (0+), or -1 on error.
 *
 * @par Environment Variables Checked
 *   - CPISI_LOG_LEVEL: TRACE, DEBUG, INFO, WARN, ERROR, FATAL, CRISIS
 *   - CPISI_LOG_FILE: Path to log file
 *   - CPISI_DEBUG_LEVEL: OFF, BASIC, VERBOSE, FULL, TRACE
 *   - CPISI_RESTORE_STRATEGY: NONE, BASIC, FULL, or comma-separated flags
 *   - CPISI_FLAGS: NONE, MINIMAL, STANDARD, FULL, or comma-separated flags
 *
 * @see CPISI_ENV_* macros in config.h
 */
int cpisi_config_apply_env_overrides(CpisiConfig* config, CpisiConfigError* error) {
    if (!config) {
        if (error) *error = CPISI_CONFIG_ERR_NULL;
        return -1;
    }

    int overrides_applied = 0;
    const char* env_val;

    /* CPISI_LOG_LEVEL override */
    env_val = getenv(CPISI_ENV_LOG_LEVEL);
    if (env_val && env_val[0] != '\0') {
        LogLevel new_level = parse_log_level_string(env_val);
        if (new_level != config->log_level) {
            config->log_level = new_level;
            overrides_applied++;
        }
    }

    /* CPISI_LOG_FILE override */
    env_val = getenv(CPISI_ENV_LOG_FILE);
    if (env_val && env_val[0] != '\0') {
        strncpy(s_log_file_path, env_val, sizeof(s_log_file_path) - 1);
        s_log_file_path[sizeof(s_log_file_path) - 1] = '\0';
        config->log_file = s_log_file_path;
        overrides_applied++;
    }

    /* CPISI_DEBUG_LEVEL override */
    env_val = getenv(CPISI_ENV_DEBUG_LEVEL);
    if (env_val && env_val[0] != '\0') {
        DebugLevel new_level = parse_debug_level_string(env_val);
        if (new_level != config->debug_level) {
            config->debug_level = new_level;
            overrides_applied++;
        }
    }

    /* CPISI_RESTORE_STRATEGY override */
    env_val = getenv(CPISI_ENV_RESTORE_STRATEGY);
    if (env_val && env_val[0] != '\0') {
        RestoreStrategy new_strategy = parse_restore_strategy_string(env_val);
        if (new_strategy != config->restore_strategy) {
            config->restore_strategy = new_strategy;
            overrides_applied++;
        }
    }

    /* CPISI_FLAGS override */
    env_val = getenv(CPISI_ENV_FLAGS);
    if (env_val && env_val[0] != '\0') {
        CpisiFlags new_flags = parse_flags_string(env_val);
        if (new_flags != config->flags) {
            config->flags = new_flags;
            overrides_applied++;
        }
    }

    if (error) *error = CPISI_CONFIG_OK;
    return overrides_applied;
}

/** @} */ /* end B.5e ENV */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5f COMPLETE — Full Initialization Flow [COMPLETE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_config_api_complete B.5f COMPLETE — Full Initialization Flow
 * @ingroup cpisi_config_api
 * @brief    Complete configuration loading with all steps.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Complete configuration loading flow.
 *
 * Performs full initialization:
 *   1. Set defaults
 *   2. Load from file (or search standard paths)
 *   3. Apply environment variable overrides
 *   4. Validate and clamp values
 *
 * @param[in]  config_path  Optional path to config file (NULL to search).
 * @param[out] config       Configuration struct to populate. Must not be NULL.
 * @param[out] error        Error/warning code output (may be NULL).
 *
 * @return 0 on success (even with warnings), -1 on hard error.
 *
 * @par Error Codes
 *   - CPISI_CONFIG_OK: Success, no issues
 *   - CPISI_CONFIG_WARN_DEFAULT: Using defaults (file not found)
 *   - CPISI_CONFIG_WARN_CLAMPED: Some values were clamped
 *   - CPISI_CONFIG_ERR_NULL: NULL config pointer
 *   - CPISI_CONFIG_ERR_FILE: File specified but not readable
 *
 * @par Example Usage
 * @code{.c}
 * CpisiConfig config;
 * CpisiConfigError error;
 *
 * if (cpisi_config_load_complete(NULL, &config, &error) < 0) {
 *     fprintf(stderr, "Config error: %d\n", error);
 *     return 1;
 * }
 *
 * if (error == CPISI_CONFIG_WARN_DEFAULT) {
 *     fprintf(stderr, "Warning: Using default configuration\n");
 * }
 * @endcode
 */
int cpisi_config_load_complete(const char* config_path, CpisiConfig* config,
                               CpisiConfigError* error) {
    if (!config) {
        if (error) *error = CPISI_CONFIG_ERR_NULL;
        return -1;
    }

    CpisiConfigError local_error = CPISI_CONFIG_OK;

    /* Step 1: Set defaults first */
    cpisi_config_set_defaults(config);

    /* Step 2: Try to load from file */
    if (config_path) {
        int load_result = cpisi_config_load_file(config_path, config);
        if (load_result < 0) {
            /* Specified file not found — try standard paths */
            char found_path[CONFIG_PATH_MAX];
            if (cpisi_config_find_file(found_path, sizeof(found_path)) == 0) {
                if (cpisi_config_load_file(found_path, config) < 0) {
                    local_error = CPISI_CONFIG_WARN_DEFAULT;
                }
            } else {
                local_error = CPISI_CONFIG_WARN_DEFAULT;  /* Using defaults */
            }
        }
    } else {
        /* No path specified — search standard paths */
        char found_path[CONFIG_PATH_MAX];
        if (cpisi_config_find_file(found_path, sizeof(found_path)) == 0) {
            if (cpisi_config_load_file(found_path, config) < 0) {
                local_error = CPISI_CONFIG_WARN_DEFAULT;
            }
        } else {
            local_error = CPISI_CONFIG_WARN_DEFAULT;  /* Using defaults */
        }
    }

    /* Step 3: Apply environment overrides */
    CpisiConfigError env_error = CPISI_CONFIG_OK;
    int env_result = cpisi_config_apply_env_overrides(config, &env_error);
    if (env_result < 0) {
        local_error = env_error;
    }

    /* Step 4: Validate final config */
    int corrections = cpisi_config_validate(config);
    if (corrections > 0 && local_error == CPISI_CONFIG_OK) {
        local_error = CPISI_CONFIG_WARN_CLAMPED;
    }

    if (error) *error = local_error;

    /* Return 0 for success (even with warnings), negative for hard errors */
    return (local_error > 0) ? -1 : 0;
}

/** @} */ /* end B.5f COMPLETE */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — What Can Be Reserved [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_body_omission B.6 OMISSION — Reserved Section Guide
 * @brief    Documents reserved sections in BODY.
 *
 * Reserved Sections:
 *   - B.3 CORE OPS: All logic in helpers (B.2) or public API (B.5)
 *   - B.4 ERRORS: Errors returned via CpisiConfigError enum
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// 4-Block Code Structure: Operational guidance and closing notes
///
/// CONTAINS:
///   - X.1 VALIDATION    — Build, runtime, memory verification
///   - X.2 EXECUTION     — Library usage patterns
///   - X.3 CLEANUP       — Ownership, memory management
///   - X.4 POLICY        — Modification guidance
///   - X.5 EXTENSION     — How to expand
///   - X.6 TROUBLESHOOT  — Common problems
///   - X.7 REFERENCE     — Quick lookup
///   - X.8 NOTE          — Closing guidance
///   - X.10 OMISSION     — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build, runtime, and memory verification.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -c -Wall -Wextra -Werror -std=c11 cpisi/io/config.c -o config.o
 * clang --analyze cpisi/io/config.c
 * @endcode
 *
 * @par X.1b RUNTIME — Test Commands
 * @code{.sh}
 * # Integration test via cpisi executable
 * ./bin/cpisi --config=./test_config.toml
 *
 * # Environment override test
 * CPISI_LOG_LEVEL=DEBUG CPISI_FLAGS=LOG ./bin/cpisi
 * @endcode
 *
 * @par X.1c SELFTEST — Minimal Isolation Test
 * @code{.c}
 * #include "kernel/cpisi/config/config.cfg.h"
 *
 * int main(void) {
 *     CpisiConfig config;
 *     CpisiConfigError error;
 *
 *     int result = cpisi_config_load_complete(NULL, &config, &error);
 *     printf("Result: %d, Error: %d\n", result, error);
 *     printf("Log level: %d, Flags: 0x%02X\n", config.log_level, config.flags);
 *     return (result < 0 && error > 0) ? 1 : 0;
 * }
 * @endcode
 *
 * @par X.1d MEMORY — Memory Leak Detection
 * @code{.sh}
 * valgrind --leak-check=full ./bin/cpisi --config=./test.toml
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Library Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_execution X.2 EXECUTION — Library Usage Patterns
 * @brief    Usage patterns for this library module.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.2a INCLUDE — Include Directive
 * @code{.c}
 * #include "kernel/cpisi/config/config.cfg.h"
 * @endcode
 *
 * @par X.2b Return Codes
 *   - cpisi_config_load_complete(): 0 = success, -1 = error
 *   - cpisi_config_validate(): N = corrections made, -1 = NULL input
 *   - cpisi_config_apply_env_overrides(): N = overrides, -1 = NULL input
 *
 * @par X.2c Typical Usage Flow
 *   1. Declare CpisiConfig and CpisiConfigError
 *   2. Call cpisi_config_load_complete()
 *   3. Check return value and error code
 *   4. Use config values
 *
 * @par X.2d Complete Example
 * @code{.c}
 * CpisiConfig config;
 * CpisiConfigError error;
 *
 * if (cpisi_config_load_complete("./config.toml", &config, &error) < 0) {
 *     fprintf(stderr, "Config error: %d\n", error);
 *     return 1;
 * }
 *
 * // Use config
 * if (config.flags & CPISI_FLAG_LOG) {
 *     setup_logging(config.log_level, config.log_file);
 * }
 * @endcode
 *
 * @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership, Memory, Shutdown [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_cleanup_guide X.3 CLEANUP — Ownership and Memory
 * @brief    Ownership patterns and memory management.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{
 *
 * @par X.3a OWNERSHIP — Ownership Conventions
 *   - CpisiConfig struct: Caller-owned (stack or heap)
 *   - config->log_file: Points to static buffer (valid for process lifetime)
 *   - config->search_paths: Caller-owned if set (not currently used)
 *
 * @par X.3b MEMORY — Memory Management
 *   - No dynamic allocation in config loading (static buffers used)
 *   - TOML file content freed internally after parsing
 *   - Config struct can be stack-allocated (no _destroy needed)
 *
 * @par X.3c SHUTDOWN — Cleanup Pattern
 * @code{.c}
 * // No explicit cleanup needed for CpisiConfig
 * // Static buffers managed internally
 * @endcode
 *
 * @} */

/* ─────────────────────────────────────────────────────────────────────────────
 * END CODE SECTIONS [END_CODE_SECTIONS]
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_policy X.4 POLICY — Modification Guidance
 * @brief    Guide future maintainers — what's safe to change.
 *
 * SAFE TO MODIFY:
 *   - Add new environment variable overrides (follow pattern in B.5e)
 *   - Add new parse_*_string() helpers (follow pattern in B.2b)
 *   - Add new validation rules (follow pattern in B.5b)
 *   - Add new TOML sections to parse (follow pattern in B.5c)
 *
 * MODIFY WITH CARE:
 *   - Static buffer sizes (s_log_file_path, etc.) — affects all callers
 *   - Search path order in cpisi_config_find_file() — affects discovery
 *   - CpisiConfigError codes — breaks error handling in callers
 *
 * NEVER MODIFY:
 *   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
 *   - Public function signatures (breaks all callers)
 *   - CpisiConfig struct layout (ABI compatibility)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_extension X.5 EXTENSION — How to Expand
 * @brief    How to expand this component.
 *
 * ADDING ENVIRONMENT VARIABLES:
 *   1. Add CPISI_ENV_* macro in config.h
 *   2. Add parse_*_string() helper if new enum type
 *   3. Add getenv() call in cpisi_config_apply_env_overrides()
 *
 * ADDING TOML SECTIONS:
 *   1. Add fields to CpisiConfig struct in config.h
 *   2. Add default values in cpisi_config_set_defaults()
 *   3. Add parsing in cpisi_config_load_file()
 *   4. Add validation in cpisi_config_validate()
 *
 * ADDING ARRAY PARSING (search_paths):
 *   - Implement toml_parse_string_array() helper
 *   - Allocate memory for array (caller must free)
 *   - Document ownership in X.3a
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_troubleshoot X.6 TROUBLESHOOTING — Common Problems
 * @brief    Common problems and solutions.
 *
 * "Config file not found":
 *   - Check file exists at specified path
 *   - Check standard search paths (see B.5d)
 *   - Use absolute path for clarity
 *
 * "Environment override not working":
 *   - Check exact variable name (case-sensitive on Unix)
 *   - Check value format (see parse_*_string() helpers)
 *   - Export variable before running program
 *
 * "Values clamped unexpectedly":
 *   - Check valid ranges in cpisi_config_validate()
 *   - RestoreStrategy uses bit flags, not sequential values
 *   - LogLevel is 0-6, not arbitrary integers
 *
 * "log_file is NULL after loading":
 *   - Check [log] section has file = "path" (with quotes)
 *   - Empty string "" results in NULL (stdout)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_reference X.7 REFERENCE — Quick Lookup
 * @brief    Copy-paste ready examples.
 *
 * @par Basic Setup
 * @code{.c}
 * CpisiConfig config;
 * cpisi_config_load_complete(NULL, &config, NULL);
 * @endcode
 *
 * @par With Error Handling
 * @code{.c}
 * CpisiConfig config;
 * CpisiConfigError error;
 *
 * if (cpisi_config_load_complete("./config.toml", &config, &error) < 0) {
 *     fprintf(stderr, "Config error: %d\n", error);
 *     return 1;
 * }
 * @endcode
 *
 * @par Environment Override
 * @code{.sh}
 * export CPISI_LOG_LEVEL=DEBUG
 * export CPISI_FLAGS=LOG,DEBUG
 * ./your_program
 * @endcode
 *
 * @par Minimal TOML Config
 * @code{.toml}
 * [subsystems]
 * log = true
 * debug = true
 * restore = true
 *
 * [log]
 * min_level = 2  # INFO
 *
 * [debug]
 * level = 1  # BASIC
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * This module implements configuration loading for the CPI-SI system.
 * Configuration is the foundation upon which the system builds.
 *
 * @par Scripture
 * "For which of you, intending to build a tower, sitteth not down first,
 *  and counteth the cost, whether he have sufficient to finish it?"
 *   — Luke 14:28
 *
 * Configuration is counted before execution begins. Environment overrides
 * allow runtime adjustment without modifying TOML files. Validation ensures
 * no invalid values enter the system.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 OMISSION — What Can Be Reserved in CLOSING [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_config_closing_omission X.10 OMISSION — Reserved Sections
 * @brief    Documents reserved sections in CLOSING.
 *
 * Reserved Sections:
 *   - X.9 TEMPLATE: Not applicable (this is not a template)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_MODE >= 2 - Full config implementation */
