/// @file cpisi.c
/// @omni code --c -library
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// OMNICODE PRAGMA [PRAGMA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.1 CORE — Identity [CORE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:req  key        = CORNERSTONE-CORE-CPISI-UNIFIED-IMPL
/// @omni:req  from       = cornerstone/templates/code/c/source.c
/// @omni:req  at         = a-02.00
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = source
/// @omni:inh  subtype    = implementation
/// @omni:inh  role       = orchestrator
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = core
/// @omni:ins  includes   = [cpisi.h, io/config.h]
/// @omni:ins  provides   = [CPI-SI Unified Orchestrator]
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
/// @omni:req  title      = CPI-SI Unified System Orchestrator
/// @omni:req  brief      = Coordinates DETECT/ASSESS/RESTORE with cognitive state
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
///   - SP.1 CONFIG  — Compile-time settings (POSIX for clock_gettime)
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
 * WHY: Settings that affect what headers provide must precede #include.
 *
 * CONTAINS:
 *   - SP.1a POSIX  — Platform feature test (for clock_gettime)
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
 * @brief    Enables POSIX.1-2001 functions for clock_gettime.
 *
 * WHY: clock_gettime requires _POSIX_C_SOURCE >= 199309L.
 *      Must be defined BEFORE <time.h> is included.
 *
 * SCOPE: Required for monotonic timestamp generation.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L  /**< POSIX.1-2001 — enables clock_gettime */
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
 * @file      cpisi.c
 * @brief     CPI-SI Unified Orchestrator — coordinates DAR phases with cognitive state.
 *
 * @defgroup cpisi_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CORNERSTONE-CORE-CPISI-UNIFIED-IMPL
 * Title:     CPI-SI Unified System Orchestrator Implementation
 * Type:      Source (Library Implementation)
 * Component: Core — CPI-SI infrastructure orchestration
 * Role:      Coordinates DETECT/ASSESS/RESTORE with unified cognitive state
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_state M.2 STATE — Lifecycle State
 * @{
 *
 * @version   a-02.00
 * @date      2025-01-25
 *
 * Status:    Active
 * Created:   2025-01-20
 * Updated:   2025-01-25
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_attribution M.3 ATTRIBUTION — Authorship & Rights
 * @{
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Implementation)
 * @copyright © 2025 CreativeWorkzStudio LLC. All rights reserved.
 *
 * @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Position [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_location M.4 LOCATION — File Position
 * @{
 *
 * Path:      cornerstone/engine/core/src/cpisi/cpisi.c
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Template Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_derivation M.5 DERIVATION — Template Lineage
 * @{
 *
 * Derives:   bereshit/word/seed/code/c/source.c (4-block template)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_classification M.6 CLASSIFICATION — Categorization
 * @{
 *
 * Tags:      cpisi, orchestrator, dar, health, cognitive-state
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose Statement [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_intent M.7 INTENT — Purpose Statement
 * @{
 *
 * Purpose:   Unified orchestration of CPI-SI DAR phases for resilient execution.
 *
 * This orchestrator provides:
 *   - Initialization and coordination of all three DAR phases
 *   - Unified cognitive state tracking (HebrewState, KFactor, HealthScore)
 *   - State propagation across all phase contexts
 *   - Type-safe health tracking with proper enum usage
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Scripture: Ecclesiastes 4:12 — "A threefold cord is not quickly broken"
 * Principle: The three DAR phases (DETECT, ASSESS, RESTORE) strengthen each
 *            other through coordination. Unity in diversity creates resilience.
 *
 * Anchor:    Proverbs 15:22 — "Without counsel purposes are disappointed"
 *            The orchestrator provides counsel to all phases.
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dependencies M.9 DEPENDENCIES — Required Components
 * @{
 *
 * Depends:
 *   - cpisi/cpisi.h        — Orchestrator header (brings all DAR + types)
 *   - cpisi/io/config.h    — TOML configuration loading
 *   - <stdlib.h>           — malloc, free
 *   - <string.h>           — memset, snprintf
 *   - <stdio.h>            — printf, fprintf
 *   - <time.h>             — clock_gettime (POSIX)
 *
 * Used by:
 *   - Application code using CPI-SI system
 *   - Demo programs validating DAR functionality
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — Version History [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_roadmap M.10 ROADMAP — Version History
 * @{
 *
 * History:
 *   a-01.00 (2025-01-20) — Initial implementation with basic DAR coordination
 *   a-02.00 (2025-01-25) — Full template alignment, proper Doxygen docstrings
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
///   - S.1 INCLUDES  — Header dependencies
///   - S.2 DEFINES   — Constants and macros
///   - S.3 TYPES     — Internal type declarations
///   - S.4 PROTOS    — Static function prototypes
///   - S.5 STATIC    — Static/file-scope variables
///   - S.6 OMISSION  — Reserved section guide
///
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * CODE SECTIONS [CODE_SECTIONS]
 * ───────────────────────────────────────────────────────────────────────────── */

// Include kernel.h first to get CPISI_MODE
#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), CPISI manager is stubs only.
// The full implementation requires hosted environment with full types.
#if CPISI_MODE >= 2

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
 *   - S.1a STANDARD  — System headers
 *   - S.1b PROJECT   — Project headers
 *   - S.1c EXTERNAL  — Third-party headers (none)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_standard S.1a STANDARD — System Headers
 * @ingroup cpisi_includes
 * @brief    C standard library headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdlib.h>   /**< Memory management: malloc, free */
#include <string.h>   /**< String operations: memset, snprintf */
#include <stdio.h>    /**< I/O: printf, fprintf, snprintf */
#include <time.h>     /**< Time: clock_gettime (with POSIX feature) */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b PROJECT — Project Headers [PROJECT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_project S.1b PROJECT — Project Headers
 * @ingroup cpisi_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * LADDER: cpisi.h → dar.h → phases → types
 *                → config.h → log.h, phase.h
 *                → json.h
 * Single include brings entire CPI-SI subsystem.
 */
#include "services/cpisi/cpisi.mgr.h"  /**< LADDER: cpisi.h brings dar, config, json, types */

/** @} */ /* end S.1b PROJECT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup cpisi_includes
 * @brief    External library headers with justification.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [None — standard library only] */

/** @} */ /* end S.1c EXTERNAL */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Constants and macros for this component.
 *
 * PURPOSE: Define component-specific constants. Most come from cpisi.h.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: All constants defined in cpisi.h and types.h] */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types S.3 TYPES — Type Declarations
 * @brief    File-local data structures and type definitions.
 *
 * PURPOSE: Internal types for this compilation unit.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a HEADER TYPES — Types from Header [HEADER_TYPES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_header_types S.3a HEADER TYPES — Types from Header
 * @ingroup cpisi_types
 * @brief    Types available from cpisi.h — DO NOT REDEFINE.
 *
 * The following types are defined in headers and available here:
 *   - CpisiOrchContext  — Unified orchestrator context
 *   - CpisiConfig       — Configuration structure
 *   - HebrewState       — 7-level cognitive state (shavar → tov)
 *   - KFactor           — Direction indicator (-1, 0, +1)
 *   - HealthScore       — Health value (-100 to +100)
 *   - HealthLevel       — Discrete 7-level health enum
 *   - Severity          — Error severity (NONE to FATAL)
 *   - RestoreResult     — Recovery result (-3 to +3)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* All public types come from cpisi.h and types.h — no redefinition needed */

/** @} */ /* end S.3a HEADER TYPES */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Internal Static Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_prototypes S.4 PROTOTYPES — Internal Static Function Declarations
 * @brief    Forward declarations for internal static functions.
 *
 * PURPOSE: Declare static helper functions before use (enables top-down reading).
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a HELPERS — Pure Helpers [HELPERS_PROTO]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_proto S.4a HELPERS — Pure Helpers
 * @ingroup cpisi_prototypes
 * @brief    Static helper functions for value transformation.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Clamp health score to valid range [-100, +100].
 *
 * @param[in] val  Raw health value to clamp.
 *
 * @return Clamped value within HEALTH_MIN to HEALTH_MAX.
 */
static inline HealthScore clamp_health(HealthScore val);

/**
 * @brief Map Severity enum to health point deduction.
 *
 * @param[in] sev  Error severity level.
 *
 * @return Positive health impact value (higher severity = larger deduction).
 */
static HealthScore severity_to_impact(Severity sev);

/**
 * @brief Map RestoreResult enum to health point recovery.
 *
 * @param[in] result  Recovery result code.
 *
 * @return Health bonus/penalty (positive for success, negative for failure).
 */
static HealthScore restore_to_bonus(RestoreResult result);

/**
 * @brief Map health score to appropriate Hebrew cognitive state.
 *
 * Uses 7-level system: shavar(-1) → yashar(0) → tov(+1)
 * Middle states depend on k-factor direction.
 *
 * @param[in] health  Current health score.
 * @param[in] k       Current k-factor direction.
 *
 * @return Hebrew state corresponding to health and direction.
 */
static HebrewState health_to_hebrew_state(HealthScore health, KFactor k);

/**
 * @brief Map health score to discrete 7-level enum.
 *
 * @param[in] health  Current health score.
 *
 * @return HealthLevel enum value (BROKEN to PERFECT).
 */
static HealthLevel health_to_level(HealthScore health);

/* hebrew_state_name: Uses inline version from types.h - no local definition needed */

/**
 * @brief Get current monotonic timestamp in milliseconds.
 *
 * Uses CLOCK_MONOTONIC for session-relative timing.
 *
 * @return Milliseconds since some fixed point (not wall clock).
 */
static uint64_t get_timestamp_ms(void);

/** @} */ /* end S.4a HELPERS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — File Scope Declarations [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_file S.5 FILE-LEVEL — File Scope Declarations
 * @brief    File-level state and infrastructure.
 *
 * PURPOSE: Static variables, global context pointer, internal storage.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5a GLOBAL — Global Context Pointer [GLOBAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_global S.5a GLOBAL — Global Context Pointer
 * @ingroup cpisi_file
 * @brief    Global CPI-SI context pointer for convenience macros.
 *
 * WHY: Allows LOG_*, DEBUG_*, RESTORE_* macros to work without explicit context.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Global CPI-SI orchestrator context.
 *
 * Set during cpisi_orch_init(), cleared during cpisi_orch_shutdown().
 * Used by convenience macros and functions that accept NULL context.
 */
CpisiOrchContext* g_cpisi = NULL;

/** @} */ /* end S.5a GLOBAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5b STORAGE — Internal Storage [STORAGE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_storage S.5b STORAGE — Internal Storage
 * @ingroup cpisi_file
 * @brief    Static storage for default context and subsystems.
 *
 * WHY: Provides default storage when NULL is passed to cpisi_orch_init.
 *      Avoids dynamic allocation for simple use cases.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Default orchestrator context for simple use cases.
 *
 * Used when NULL is passed to cpisi_orch_init().
 */
static CpisiOrchContext s_default_cpisi_ctx = {0};

/**
 * @brief Internal DETECT (logging) subsystem context.
 */
static LogContext s_internal_log = {0};

/**
 * @brief Internal ASSESS (debug) subsystem context.
 */
static DebugContext s_internal_debug = {0};

/**
 * @brief Internal RESTORE (recovery) subsystem context.
 */
static RestoreContext s_internal_restore = {0};

/**
 * @brief Internal DAR orchestrator for coordinated phase execution.
 *
 * WHY: The CPI-SI orchestrator uses DAR internally for its own witness system.
 *      "Dogfooding" — we use what we build.
 */
static DAROrchestrator s_internal_dar = {0};

/**
 * @brief Internal health log configuration for structured health event logging.
 */
static HealthLogConfig s_internal_health_log = {0};

/**
 * @brief Flag indicating health log is initialized.
 */
static bool s_health_log_initialized = false;

/**
 * @brief Internal clock context for session time tracking.
 */
static ClockContext s_internal_clock = {0};

/** @} */ /* end S.5b STORAGE */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_omission S.6 OMISSION — Reserved Section Guide
 * @brief    Documents what sections are reserved and why.
 *
 * RESERVED SECTIONS:
 *   - S.2 DEFINES: All constants in headers
 *   - S.1c EXTERNAL: Standard library only
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
///   - B.1 ORG CHART    — Ladder hierarchy, baton flow
///   - B.2 HELPERS      — Foundation implementations (pure, utilities)
///   - B.3 CORE OPS     — Business logic implementations
///   - B.4 ERRORS       — Error handling implementations
///   - B.5 PUBLIC API   — Exported interface implementations
///   - B.6 OMISSION     — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Ladder Hierarchy [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_orgchart B.1 ORG CHART — Ladder Hierarchy
 * @brief    Map internal structure — ladder hierarchy, baton flow.
 *
 * WHY: Understanding the file structure before diving into details.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Ladder Hierarchy [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_orgchart_structure B.1a STRUCTURE — Ladder Hierarchy
 * @ingroup cpisi_orgchart
 * @brief    Function call hierarchy — who calls whom.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/*
 * Public APIs (Top Rungs - Orchestration)
 * ├── cpisi_orch_init()           → uses helpers, initializes subsystems
 * ├── cpisi_orch_shutdown()       → uses helpers, shuts down in reverse order
 * ├── cpisi_orch_reset()          → uses shutdown + init
 * ├── cpisi_orch_init_from_file() → uses config loader + init
 * │
 * ├── cpisi_orch_set_hebrew_state() → uses sync_state
 * ├── cpisi_orch_set_k_factor()     → uses sync_state
 * ├── cpisi_orch_set_health()       → uses health_to_level, sync_state
 * ├── cpisi_orch_update_cube_pos()  → pure assignment
 * │
 * ├── cpisi_orch_record_error()     → uses severity_to_impact, health_to_level
 * ├── cpisi_orch_record_recovery()  → uses restore_to_bonus, health_to_level
 * ├── cpisi_orch_record_degradation() → uses severity_to_impact
 * ├── cpisi_orch_sync_state()       → propagates state to subsystems
 * │
 * ├── cpisi_orch_status_report()    → uses hebrew_state_name
 * └── cpisi_orch_status_brief()     → uses hebrew_state_name
 *
 * Helpers (Bottom Rungs - Foundations)
 * ├── clamp_health()           → pure function
 * ├── severity_to_impact()     → pure function
 * ├── restore_to_bonus()       → pure function
 * ├── health_to_hebrew_state() → pure function
 * ├── health_to_level()        → pure function
 * ├── hebrew_state_name()      → pure function
 * └── get_timestamp_ms()       → system call wrapper
 *
 * Module Dependencies (Orchestrator Pattern):
 *   cpisi.c (orchestrator) → detect.c (logging)
 *                         → assess.c (debugging)
 *                         → restore.c (recovery)
 *                         → config.c (TOML loading)
 */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Baton Execution Path [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_orgchart_flow B.1b FLOW — Baton Execution Path
 * @ingroup cpisi_orgchart
 * @brief    How execution flows through the file.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/*
 * INIT Flow:
 *   Entry → cpisi_orch_init()
 *     ↓
 *   Initialize context (memset, timestamps)
 *     ↓
 *   Initialize cognitive state (yashar, neutral, 0 health)
 *     ↓
 *   Initialize thresholds from HEALTH_LEVEL bounds
 *     ↓
 *   DETECT subsystem (cpisi_log_init)
 *     ↓
 *   ASSESS subsystem (cpisi_debug_init)
 *     ↓
 *   RESTORE subsystem (cpisi_restore_init)
 *     ↓
 *   Set global pointer, log success
 *     ↓
 *   Exit → return 0
 *
 * SHUTDOWN Flow (reverse order):
 *   Entry → cpisi_orch_shutdown()
 *     ↓
 *   Log final status
 *     ↓
 *   RESTORE shutdown (cpisi_restore_shutdown)
 *     ↓
 *   ASSESS shutdown (cpisi_debug_shutdown)
 *     ↓
 *   DETECT shutdown (cpisi_log_shutdown) — last, need logging
 *     ↓
 *   Clear global pointer
 *     ↓
 *   Exit → return
 */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — APU Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_orgchart_counts B.1c COUNTS — APU Statistics
 * @ingroup cpisi_orgchart
 * @brief    Atomic Processing Unit counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Functions:       20 total */
/* Helpers:          7 (pure foundations) */
/* Lifecycle:        4 (init, shutdown, reset, init_from_file) */
/* State:            8 (setters + getters) */
/* Health:           4 (record_error, record_recovery, record_degradation, sync_state) */
/* Status:           2 (status_report, status_brief) */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Foundation Implementations [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_helpers B.2 HELPERS — Foundation Implementations
 * @brief    Foundation function implementations — building blocks.
 *
 * WHY: Helpers are the bottom rungs of the ladder. They do ONE thing well.
 *      Higher functions compose these proven pieces.
 *
 * CONTAINS:
 *   - B.2a PURE       — Side-effect-free implementations
 *   - B.2b UTILITIES  — Implementations with side effects
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a PURE — Side-Effect-Free Implementations [PURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_pure B.2a PURE — Side-Effect-Free Implementations
 * @ingroup cpisi_helpers
 * @brief    Pure function implementations — same input always produces same output.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Clamp health score to valid range.
 *
 * @param[in] val  Raw health value.
 *
 * @return Value clamped to [HEALTH_MIN, HEALTH_MAX].
 */
static inline HealthScore clamp_health(HealthScore val) {
    if (val < HEALTH_MIN) return HEALTH_MIN;
    if (val > HEALTH_MAX) return HEALTH_MAX;
    return val;
}

/**
 * @brief Map Severity enum to health point impact (PROPORTIONAL).
 *
 * Uses the canonical formula: severity × SEVERITY_STEP (25).
 * This gives proportional impacts aligned with the 9-level severity scale.
 *
 * Scripture: "Wide is the gate that leads to destruction" — Matthew 7:13
 * The impact matches the severity's distance from center (yashar).
 *
 * @param[in] sev  Error severity level (-4 to +4).
 *
 * @return Health impact (negative for errors, positive for success):
 *
 * | Severity    | Value | Impact |
 * |-------------|-------|--------|
 * | SEV_FATAL   |  -4   | -100   |
 * | SEV_HIGH    |  -3   |  -75   |
 * | SEV_MEDIUM  |  -2   |  -50   |
 * | SEV_LOW     |  -1   |  -25   |
 * | SEV_NONE    |   0   |    0   |
 * | SEV_FAIR    |  +1   |  +25   |
 * | SEV_GOOD    |  +2   |  +50   |
 * | SEV_EXCELLENT| +3   |  +75   |
 * | SEV_PERFECT |  +4   | +100   |
 */
static HealthScore severity_to_impact(Severity sev) {
    /* Use the canonical lookup table from types.h for accuracy */
    const SeverityInfo* info = severity_info(sev);
    return (HealthScore)info->health_impact;
}

/**
 * @brief Map RestoreResult enum to health point recovery (PROPORTIONAL).
 *
 * Uses proportional values aligned with SEVERITY_STEP (25).
 * Recovery results map to severity-equivalent impacts:
 *   - Fatal recovery failure = worse than original error (SEV_FATAL impact)
 *   - Failed recovery = moderate negative (SEV_MEDIUM impact)
 *   - Degraded = minor positive (SEV_FAIR impact)
 *   - Success = moderate positive (SEV_GOOD impact)
 *   - Full OK = excellent (SEV_EXCELLENT impact)
 *
 * Scripture: "A just man falleth seven times, and riseth up again" — Proverbs 24:16
 * Recovery rewards persistence proportionally.
 *
 * @param[in] result  Recovery result code.
 *
 * @return Health bonus (positive) or penalty (negative):
 *
 * | Result          | Equiv Sev  | Impact |
 * |-----------------|------------|--------|
 * | RESTORE_FATAL   | SEV_FATAL  | -100   | Made things worse
 * | RESTORE_FAIL    | SEV_MEDIUM |  -50   | Recovery failed
 * | RESTORE_INVALID | SEV_NONE   |    0   | Invalid input, no change
 * | RESTORE_DEGRADED| SEV_FAIR   |  +25   | Partial success
 * | RESTORE_FILLED  | SEV_GOOD   |  +50   | Value filled
 * | RESTORE_LOCATED | SEV_GOOD   |  +50   | Resource found
 * | RESTORE_OK      | SEV_EXCELLENT| +75  | Full recovery
 */
static HealthScore restore_to_bonus(RestoreResult result) {
    switch (result) {
        case RESTORE_FATAL:    return -100;  /* Made things catastrophically worse */
        case RESTORE_FAIL:     return -50;   /* Recovery failed (SEV_MEDIUM equiv) */
        case RESTORE_INVALID:  return 0;     /* Invalid input, no action */
        case RESTORE_DEGRADED: return +25;   /* Partial success (SEV_FAIR equiv) */
        case RESTORE_FILLED:   return +50;   /* Value filled (SEV_GOOD equiv) */
        case RESTORE_LOCATED:  return +50;   /* Resource found (SEV_GOOD equiv) */
        case RESTORE_OK:       return +75;   /* Full recovery (SEV_EXCELLENT equiv) */
        default:               return 0;
    }
}

/**
 * @brief Map health score to appropriate Hebrew cognitive state.
 *
 * Uses 7-level system from diagnostics.toml:
 *   - Edge states (shavar, tov) don't depend on k-factor
 *   - Middle states depend on k-factor direction
 *
 * @param[in] health  Current health score (-100 to +100).
 * @param[in] k       Current k-factor direction.
 *
 * @return Hebrew state:
 *
 * | Health Range | k=+1    | k=-1/0  |
 * |--------------|---------|---------|
 * | <= -75       | shavar  | shavar  |
 * | -74 to -26   | ratsah  | chaser  |
 * | -25 to +25   | yashar  | yashar  |
 * | +26 to +74   | shalem  | tamim   |
 * | >= +75       | tov     | tov     |
 */
static HebrewState health_to_hebrew_state(HealthScore health, KFactor k) {
    /* Edge states (don't depend on k-factor) */
    if (health <= -75) return HEBREW_SHAVAR;  /* Broken */
    if (health >= 75)  return HEBREW_TOV;     /* Perfect */

    /* Middle states depend on k-factor direction */
    if (health < -25) {
        return (k == K_POSITIVE) ? HEBREW_RATSAH : HEBREW_CHASER;
    }
    if (health > 25) {
        return (k == K_POSITIVE) ? HEBREW_SHALEM : HEBREW_TAMIM;
    }

    /* Center */
    return HEBREW_YASHAR;
}

/**
 * @brief Map health score to discrete 7-level enum.
 *
 * Uses HEALTH_LEVEL bounds from types.h.
 *
 * @param[in] health  Current health score.
 *
 * @return HealthLevel enum value.
 *
 * | Health Range | Level   |
 * |--------------|---------|
 * | <= -50       | BROKEN  |
 * | -49 to -26   | WANTING |
 * | -25 to -1    | LACKING |
 * | 0            | EVEN    |
 * | 1 to 25      | SOUND   |
 * | 26 to 50     | WHOLE   |
 * | >= 51        | PERFECT |
 */
static HealthLevel health_to_level(HealthScore health) {
    if (health <= -50) return HEALTH_LEVEL_BROKEN;
    if (health <= -25) return HEALTH_LEVEL_WANTING;
    if (health <    0) return HEALTH_LEVEL_LACKING;
    if (health ==   0) return HEALTH_LEVEL_EVEN;
    if (health <=  25) return HEALTH_LEVEL_SOUND;
    if (health <=  50) return HEALTH_LEVEL_WHOLE;
    return HEALTH_LEVEL_PERFECT;
}

/* hebrew_state_name: Uses inline version from types.h - no local implementation */

/**
 * @brief Map LogLevel to Severity for health tracking.
 *
 * LogLevel uses 9 levels (0-8) centered on 4 (INFO).
 * Severity uses 9 levels (-4 to +4) centered on 0 (NONE).
 *
 * @param[in] level  Log level (0=CRISIS to 8=INSIGHT).
 *
 * @return Severity value:
 *
 * | LogLevel    | Value | Severity     | Health Impact |
 * |-------------|-------|--------------|---------------|
 * | LOG_CRISIS  |   0   | SEV_FATAL    | -100          |
 * | LOG_FATAL   |   1   | SEV_HIGH     | -75           |
 * | LOG_ERROR   |   2   | SEV_MEDIUM   | -50           |
 * | LOG_WARN    |   3   | SEV_LOW      | -25           |
 * | LOG_INFO    |   4   | SEV_NONE     | 0             |
 * | LOG_NOTICE  |   5   | SEV_FAIR     | +25           |
 * | LOG_DEBUG   |   6   | SEV_GOOD     | +50           |
 * | LOG_TRACE   |   7   | SEV_EXCELLENT| +75           |
 * | LOG_INSIGHT |   8   | SEV_PERFECT  | +100          |
 */
static Severity log_level_to_severity(LogLevel level) {
    /* Center is INFO (4) mapping to SEV_NONE (0) */
    /* Formula: severity = level - LOG_INFO (4) = level - 4 */
    int sev_val = (int)level - 4;  /* -4 to +4 range */

    /* Clamp to valid Severity range */
    if (sev_val < SEV_FATAL) return SEV_FATAL;
    if (sev_val > SEV_PERFECT) return SEV_PERFECT;
    return (Severity)sev_val;
}

/**
 * @brief Health event callback from detect layer.
 *
 * Called by cpisi_log_v() for each log event. Maps log level to severity
 * and updates orchestrator health using ternary-derived impacts.
 *
 * FLOW: LOG_ERROR() → cpisi_log_v() → this callback → cpisi_orch_record_error()
 *
 * @param[in] level      Log level that triggered the callback.
 * @param[in] component  Component that generated the log.
 * @param[in] user_data  CpisiOrchContext* (orchestrator context).
 */
static void on_detect_health_event(LogLevel level, const char* component, void* user_data) {
    /* ─── Recursion Guard ─── */
    /* Prevents feedback loop: record_error() → LOG_ERROR() → callback → record_error() */
    static bool s_in_callback = false;
    if (s_in_callback) return;
    s_in_callback = true;

    CpisiOrchContext* ctx = (CpisiOrchContext*)user_data;
    if (!ctx) ctx = g_cpisi;
    if (!ctx) {
        s_in_callback = false;
        return;
    }

    /* ─── Filter internal health system logs ─── */
    /* These are diagnostic notifications about health state changes, not actual events.
     * Scoring them would create feedback loops and misrepresent the actual system state. */
    if (component != NULL) {
        /* Skip cpisi internal logs (threshold crossings, state changes) */
        if (strcmp(component, "cpisi") == 0 ||
            strcmp(component, "health") == 0 ||
            strcmp(component, "assess") == 0 ||
            strcmp(component, "restore") == 0 ||
            strcmp(component, "clock") == 0) {
            s_in_callback = false;
            return;
        }
    }

    /* ─── Full Ternary Range ─── */
    /* Use the FULL 9-level logging spectrum for health tracking:
     * Negative (CRISIS..WARN) → errors/failures
     * Neutral (INFO) → no change
     * Positive (NOTICE..INSIGHT) → successes/working properly
     *
     * The normalization system handles the imbalance:
     * "More fail cases than success cases" → ternary-derived weighting adjusts */

    /* Map log level to severity using full ternary range */
    Severity sev = log_level_to_severity(level);

    /* Dispatch based on polarity */
    if (sev < SEV_NONE) {
        /* Negative: error event (CRISIS, FATAL, ERROR, WARN) */
        cpisi_orch_record_error(ctx, sev);
    } else if (sev > SEV_NONE) {
        /* Positive: success event (NOTICE only - DEBUG+ filtered above) */
        cpisi_orch_record_success(ctx, sev);
    }
    /* SEV_NONE (LOG_INFO) = neutral, no health change */

    s_in_callback = false;
}

/**
 * @brief DAR state change callback to sync health back to CpisiOrchContext.
 *
 * When the DAR orchestrator's health changes (e.g., via cube movements),
 * this callback ensures the parent CpisiOrchContext stays synchronized.
 *
 * @param[in] state   Current Hebrew state (informational).
 * @param[in] health  Current health score to sync.
 * @param[in] user_data  Pointer to CpisiOrchContext.
 */
static void dar_state_change_callback(HebrewState state, HealthScore health, void* user_data) {
    CpisiOrchContext* ctx = (CpisiOrchContext*)user_data;
    if (!ctx) return;

    /* Sync health and derived values */
    ctx->health = clamp_health(health);
    ctx->health_level = health_to_level(ctx->health);
    ctx->current_state = state;
}

/** @} */ /* end B.2a PURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b UTILITIES — Implementations With Side Effects [UTILITIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_helpers_utilities B.2b UTILITIES — Implementations With Side Effects
 * @ingroup cpisi_helpers
 * @brief    Utility implementations — may access system state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Get current monotonic timestamp in milliseconds.
 *
 * Uses CLOCK_MONOTONIC for session-relative timing that doesn't
 * jump on system clock changes.
 *
 * @return Milliseconds since some fixed point (not wall clock).
 *
 * @note Requires _POSIX_C_SOURCE >= 199309L defined before <time.h>.
 */
static uint64_t get_timestamp_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

/** @} */ /* end B.2b UTILITIES */

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Business Logic Implementations [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_coreops B.3 CORE OPS — Business Logic Implementations
 * @brief    Component-specific functionality implementations.
 *
 * WHY: Core operations contain the actual business logic.
 *
 * CONTAINS:
 *   - B.3a SYNC — State synchronization across subsystems
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a SYNC — State Synchronization [SYNC]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_sync B.3a SYNC — State Synchronization
 * @ingroup cpisi_coreops
 * @brief    Propagate orchestrator state to all subsystems.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Synchronize orchestrator state to all subsystem contexts.
 *
 * Propagates health and cognitive state to DETECT, ASSESS, and RESTORE
 * contexts. Called after any state change to maintain consistency.
 *
 * @param[in,out] ctx  Orchestrator context. Uses global if NULL.
 *
 * @note This keeps all DAR phases in sync with unified state.
 */
void cpisi_orch_sync_state(CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    /* ─── Use DAR orchestrator if available (preferred) ─── */
    if (ctx->dar) {
        /* DAR orchestrator handles coordinated sync to all phases */
        cpisi_dar_set_health(ctx->dar, ctx->health);
        cpisi_dar_set_hebrew_state(ctx->dar, ctx->current_state);
        cpisi_dar_set_k_factor(ctx->dar, ctx->current_k);
        cpisi_dar_sync_to_phases(ctx->dar);
        return;
    }

    /* ─── Fallback: Direct propagation to subsystems ─── */

    /* Propagate to DETECT (logging) */
    if (ctx->log) {
        ctx->log->health = ctx->health;
        ctx->log->current_state = ctx->current_state;
    }

    /* Propagate to ASSESS (debugging) */
    if (ctx->debug) {
        ctx->debug->health = ctx->health;
        ctx->debug->current_state = ctx->current_state;
    }

    /* Propagate to RESTORE (recovery) */
    if (ctx->restore) {
        ctx->restore->health = ctx->health;
        ctx->restore->current_state = ctx->current_state;
    }
}

/** @} */ /* end B.3a SYNC */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Error Handling Patterns [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_errors_impl B.4 ERRORS — Error Handling Patterns
 * @brief    Error handling, logging, cleanup patterns.
 *
 * DESIGN: Non-blocking — CPI-SI failures never interrupt main operation.
 *         Graceful degradation when subsystems fail to initialize.
 *
 * RECOVERY:
 *   - Subsystem init failure: Log warning, continue without subsystem
 *   - Config load failure: Use defaults, continue
 *   - NULL context: Use global context
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* Error handling is integrated into public API implementations below */

/** @} */ /* end B.4 ERRORS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Exported Interface Implementations [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_api B.5 PUBLIC API — Exported Interface Implementations
 * @brief    Public API implementations matching header declarations.
 *
 * WHY: Top rungs of the ladder — orchestrate helpers and core operations.
 *
 * CONTAINS:
 *   - B.5a LIFECYCLE  — Creation, initialization, destruction
 *   - B.5b COGNITIVE  — State getters and setters
 *   - B.5c HEALTH     — Health tracking operations
 *   - B.5d STATUS     — Status reporting utilities
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a LIFECYCLE — Creation/Destruction [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_api_lifecycle B.5a LIFECYCLE — Creation/Destruction
 * @ingroup cpisi_api
 * @brief    Initialization, shutdown, and reset implementations.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize the CPI-SI unified orchestrator.
 *
 * Sets up all three DAR phases (DETECT, ASSESS, RESTORE) based on
 * configuration flags. Uses default context if ctx is NULL.
 *
 * @param[in,out] ctx     Orchestrator context to initialize. Uses default if NULL.
 * @param[in]     config  Configuration settings. Uses defaults if NULL.
 *
 * @return 0 on success, -1 on critical failure.
 *
 * @note Health Impact: No health changes during init (starts at 0).
 *
 * @see cpisi_orch_shutdown() for cleanup.
 * @see cpisi_orch_init_from_file() for TOML-based initialization.
 */
int cpisi_orch_init(CpisiOrchContext* ctx, const CpisiConfig* config) {
    /* ─── Phase 1: CONTEXT SETUP ─── */
    if (!ctx) {
        ctx = &s_default_cpisi_ctx;
    }

    CpisiConfig default_config = CPISI_CONFIG_DEFAULT;
    if (!config) {
        config = &default_config;
    }

    /* ─── Phase 2: CLEAR & INITIALIZE ─── */
    memset(ctx, 0, sizeof(CpisiOrchContext));
    ctx->flags = config->flags;
    ctx->session_start = get_timestamp_ms();

    /* Initialize cognitive state to yashar (centered, neutral) */
    ctx->current_state = HEBREW_YASHAR;
    ctx->current_k = K_NEUTRAL;
    ctx->health = 0;
    ctx->health_level = HEALTH_LEVEL_SOUND;

    /* Initialize thresholds (from HEALTH_LEVEL bounds) */
    ctx->thresholds.critical_below = -50;  /* WANTING → BROKEN transition */
    ctx->thresholds.warn_below     = -25;  /* LACKING → WANTING transition */
    ctx->thresholds.recover_above  = 25;   /* Recovery target (SOUND level) */
    ctx->thresholds.debounce_ms    = 1000; /* 1 second debounce */

    /* ─── Phase 3: DETECT SUBSYSTEM (Logging) ─── */
    if (config->flags & CPISI_FLAG_LOG) {
        ctx->log = &s_internal_log;
        cpisi_log_init(ctx->log);
        cpisi_log_set_level(ctx->log, config->log_level);

        if (config->log_file) {
            if (cpisi_log_enable_structured(ctx->log, config->log_file) != 0) {
                /* DETECT: Log file failed - continue with stderr (graceful degradation) */
                fprintf(stderr, "[cpisi] Warning: Could not open log file: %s\n",
                        config->log_file);
                ctx->degradation_count++;
            }
        }

        g_cpisi_log = ctx->log;

        /* Register health event callback to connect detect → assess layers */
        cpisi_log_set_health_callback(on_detect_health_event, ctx);
    }

    /* ─── Phase 4: ASSESS SUBSYSTEM (Debug) ─── */
    if (config->flags & CPISI_FLAG_DEBUG) {
        ctx->debug = &s_internal_debug;
        cpisi_debug_init(ctx->debug, config->debug_level);
        cpisi_debug_set_break_on_assert(ctx->debug, config->break_on_assert);
        cpisi_debug_set_break_on_error(ctx->debug, config->break_on_error);

        g_cpisi_debug = ctx->debug;
    }

    /* ─── Phase 5: RESTORE SUBSYSTEM (Recovery) ─── */
    if (config->flags & CPISI_FLAG_RESTORE) {
        ctx->restore = &s_internal_restore;
        cpisi_restore_init(ctx->restore, config->restore_strategy);

        if (config->search_paths && config->search_path_count > 0) {
            for (size_t i = 0; i < config->search_path_count; i++) {
                cpisi_restore_add_search_path(ctx->restore, config->search_paths[i]);
            }
        }

        g_cpisi_restore = ctx->restore;
    }

    /* ─── Phase 6: DAR ORCHESTRATOR (Coordinated phases) ─── */
    cpisi_dar_init(&s_internal_dar);
    cpisi_dar_link_all(&s_internal_dar, ctx->log, ctx->debug, ctx->restore);
    ctx->dar = &s_internal_dar;

    /* Wire callback to sync DAR health back to CpisiOrchContext */
    ctx->dar->on_state_change = dar_state_change_callback;
    ctx->dar->user_data = ctx;

    /* ─── Phase 7: HEALTH LOG (Structured witness) ─── */
    if (config->log_file) {
        /* Extract directory from log file path for health log */
        char health_log_dir[256] = ".";
        const char* last_slash = strrchr(config->log_file, '/');
        if (last_slash) {
            size_t dir_len = (size_t)(last_slash - config->log_file);
            if (dir_len < sizeof(health_log_dir)) {
                memcpy(health_log_dir, config->log_file, dir_len);
                health_log_dir[dir_len] = '\0';
            }
        }

        if (cpisi_health_log_init(&s_internal_health_log, health_log_dir) == 0) {
            s_health_log_initialized = true;
        }
    }

    /* ─── Phase 8: INTERNAL CLOCK (Time awareness) ─── */
    if (cpisi_clock_init(&s_internal_clock)) {
        ctx->clock = &s_internal_clock;
        LOG_DEBUG("cpisi", "  CLOCK: period=%s, hour=%02d:%02d",
                  cpisi_clock_period_name(ctx->clock->period),
                  ctx->clock->hour, ctx->clock->minute);
    }

    /* ─── Phase 9: FINALIZE ─── */
    ctx->initialized = true;
    g_cpisi = ctx;

    /* DETECT: Log successful initialization via structured health log */
    if (s_health_log_initialized) {
        cpisi_health_log_append(&s_internal_health_log, ACTION_SUCCESS,
                                +1, "cpisi_init",
                                "CPI-SI orchestrator initialized");
    }

    /* Also log to console/file via DETECT subsystem */
    if (ctx->log) {
        LOG_INFO("cpisi", "CPI-SI initialized: flags=0x%x, state=%s, health=%d",
                 config->flags, hebrew_state_name(ctx->current_state), ctx->health);

        if (ctx->dar) {
            LOG_DEBUG("cpisi", "  DAR: orchestrator linked (detect=%s, assess=%s, restore=%s)",
                      ctx->log ? "yes" : "no",
                      ctx->debug ? "yes" : "no",
                      ctx->restore ? "yes" : "no");
        }

        if (ctx->debug) {
            LOG_DEBUG("cpisi", "  ASSESS: level=%d, break_assert=%d, break_error=%d",
                      (int)config->debug_level, config->break_on_assert,
                      config->break_on_error);
        }

        if (ctx->restore) {
            LOG_DEBUG("cpisi", "  RESTORE: strategy=0x%x, search_paths=%zu",
                      config->restore_strategy, config->search_path_count);
        }
    }

    return 0;
}

/**
 * @brief Shut down the CPI-SI unified orchestrator.
 *
 * Shuts down all subsystems in reverse initialization order:
 * RESTORE → ASSESS → DETECT (logging last, need it until the end).
 *
 * @param[in,out] ctx  Orchestrator context to shut down. Uses global if NULL.
 *
 * @note Logs final status before shutting down logging subsystem.
 *
 * @see cpisi_orch_init() for initialization.
 */
void cpisi_orch_shutdown(CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx || !ctx->initialized) return;

    /* ─── Phase 1: FINAL STATUS via Health Log ─── */
    if (s_health_log_initialized) {
        cpisi_health_log_append(&s_internal_health_log, ACTION_NEUTRAL,
                                0, "cpisi_shutdown",
                                "CPI-SI orchestrator shutting down");
    }

    if (ctx->log) {
        uint64_t duration_ms = get_timestamp_ms() - ctx->session_start;
        LOG_INFO("cpisi", "CPI-SI shutdown: state=%s, health=%d, duration=%lums",
                 hebrew_state_name(ctx->current_state), ctx->health,
                 (unsigned long)duration_ms);
        LOG_INFO("cpisi", "  Counters: errors=%u, recoveries=%u, degradations=%u, fatals=%u",
                 ctx->error_count, ctx->recovery_count,
                 ctx->degradation_count, ctx->fatal_count);
    }

    /* ─── Phase 2: CLOCK SHUTDOWN (Log duration stats before health log closes) ─── */
    if (ctx->clock) {
        cpisi_clock_shutdown(ctx->clock);
        ctx->clock = NULL;
    }

    /* ─── Phase 3: HEALTH LOG SHUTDOWN ─── */
    if (s_health_log_initialized) {
        cpisi_health_log_shutdown(&s_internal_health_log);
        s_health_log_initialized = false;
    }

    /* ─── Phase 4: DAR ORCHESTRATOR SHUTDOWN ─── */
    if (ctx->dar) {
        cpisi_dar_shutdown(ctx->dar);
        ctx->dar = NULL;
    }

    /* ─── Phase 5: RESTORE SHUTDOWN (reverse order) ─── */
    if (ctx->restore) {
        cpisi_restore_shutdown(ctx->restore);
        if (g_cpisi_restore == ctx->restore) {
            g_cpisi_restore = NULL;
        }
        ctx->restore = NULL;
    }

    /* ─── Phase 6: ASSESS SHUTDOWN ─── */
    if (ctx->debug) {
        cpisi_debug_shutdown(ctx->debug);
        if (g_cpisi_debug == ctx->debug) {
            g_cpisi_debug = NULL;
        }
        ctx->debug = NULL;
    }

    /* ─── Phase 7: DETECT SHUTDOWN (last - need logging until the end) ─── */
    if (ctx->log) {
        /* Clear health callback before shutdown to prevent stale pointer */
        cpisi_log_clear_health_callback();

        cpisi_log_shutdown(ctx->log);
        if (g_cpisi_log == ctx->log) {
            g_cpisi_log = NULL;
        }
        ctx->log = NULL;
    }

    /* ─── Phase 9: FINALIZE ─── */
    ctx->initialized = false;
    if (g_cpisi == ctx) {
        g_cpisi = NULL;
    }
}

/**
 * @brief Reset the CPI-SI system with new configuration.
 *
 * Convenience wrapper that shuts down and reinitializes.
 *
 * @param[in,out] ctx     Orchestrator context. Uses global if NULL.
 * @param[in]     config  New configuration. Uses defaults if NULL.
 *
 * @return 0 on success, -1 on failure.
 */
int cpisi_orch_reset(CpisiOrchContext* ctx, const CpisiConfig* config) {
    cpisi_orch_shutdown(ctx);
    return cpisi_orch_init(ctx, config);
}

/**
 * @brief Initialize CPI-SI from a TOML configuration file.
 *
 * Loads configuration from file (or auto-detected locations) and
 * initializes the orchestrator.
 *
 * @param[in,out] ctx          Orchestrator context. Uses default if NULL.
 * @param[in]     config_path  Path to TOML file. Auto-detects if NULL.
 *
 * @return 0 if config loaded from file, 1 if using defaults, -1 on critical failure.
 *
 * @see cpisi_config_load_or_defaults() for auto-detection logic.
 */
int cpisi_orch_init_from_file(CpisiOrchContext* ctx, const char* config_path) {
    CpisiConfig config;
    int load_result = cpisi_config_load_or_defaults(config_path, &config);

    if (load_result < 0) {
        /* DETECT: Critical error loading config */
        fprintf(stderr, "[cpisi] Error: Failed to load config from: %s\n",
                config_path ? config_path : "(auto-detect)");
        return -1;
    }

    int init_result = cpisi_orch_init(ctx, &config);
    if (init_result != 0) {
        return -1;
    }

    /* DETECT: Log config source */
    if (g_cpisi && g_cpisi->log) {
        if (load_result == 0) {
            LOG_INFO("cpisi", "Config loaded from: %s",
                     config_path ? config_path : "(auto-detected)");
        } else {
            LOG_DEBUG("cpisi", "Using default configuration");
        }
    }

    return load_result;
}

/** @} */ /* end B.5a LIFECYCLE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b COGNITIVE — State Getters and Setters [COGNITIVE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_api_cognitive B.5b COGNITIVE — State Getters and Setters
 * @ingroup cpisi_api
 * @brief    Cognitive state access and modification.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Set the Hebrew cognitive state directly.
 *
 * @param[in,out] ctx    Orchestrator context. Uses global if NULL.
 * @param[in]     state  New Hebrew state.
 *
 * @note Logs state transitions and synchronizes to subsystems.
 */
void cpisi_orch_set_hebrew_state(CpisiOrchContext* ctx, HebrewState state) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    HebrewState old_state = ctx->current_state;
    ctx->current_state = state;

    /* DETECT: Log significant state transitions */
    if (ctx->log && old_state != state) {
        LOG_DEBUG("cpisi", "State transition: %s -> %s",
                  hebrew_state_name(old_state), hebrew_state_name(state));
    }

    cpisi_orch_sync_state(ctx);
}

/**
 * @brief Set the k-factor direction.
 *
 * @param[in,out] ctx  Orchestrator context. Uses global if NULL.
 * @param[in]     k    New k-factor (-1, 0, +1).
 */
void cpisi_orch_set_k_factor(CpisiOrchContext* ctx, KFactor k) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    ctx->current_k = k;
    cpisi_orch_sync_state(ctx);
}

/**
 * @brief Set the health score directly.
 *
 * Clamps to valid range and updates derived state (level, Hebrew state).
 *
 * @param[in,out] ctx     Orchestrator context. Uses global if NULL.
 * @param[in]     health  New health score (will be clamped).
 */
void cpisi_orch_set_health(CpisiOrchContext* ctx, HealthScore health) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    ctx->health = clamp_health(health);
    ctx->health_level = health_to_level(ctx->health);
    ctx->current_state = health_to_hebrew_state(ctx->health, ctx->current_k);

    cpisi_orch_sync_state(ctx);
}

/**
 * @brief Update the cube position in 3D cognitive space.
 *
 * @param[in,out] ctx  Orchestrator context. Uses global if NULL.
 * @param[in]     x    X coordinate (-1, 0, +1).
 * @param[in]     y    Y coordinate (-1, 0, +1).
 * @param[in]     z    Z coordinate (-1, 0, +1).
 */
void cpisi_orch_update_cube_pos(CpisiOrchContext* ctx,
                                 int32_t x, int32_t y, int32_t z) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    /* Clamp to valid cube positions (-1, 0, +1) */
    ctx->cube_pos[0] = (x < -1) ? -1 : (x > 1) ? 1 : x;
    ctx->cube_pos[1] = (y < -1) ? -1 : (y > 1) ? 1 : y;
    ctx->cube_pos[2] = (z < -1) ? -1 : (z > 1) ? 1 : z;
}

/**
 * @brief Get the current Hebrew cognitive state.
 *
 * @param[in] ctx  Orchestrator context. Uses global if NULL.
 *
 * @return Current Hebrew state, or YASHAR if no context.
 */
HebrewState cpisi_orch_get_hebrew_state(const CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return HEBREW_YASHAR;
    return ctx->current_state;
}

/**
 * @brief Get the current k-factor direction.
 *
 * @param[in] ctx  Orchestrator context. Uses global if NULL.
 *
 * @return Current k-factor, or K_NEUTRAL if no context.
 */
KFactor cpisi_orch_get_k_factor(const CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return K_NEUTRAL;
    return ctx->current_k;
}

/**
 * @brief Get the current health score.
 *
 * @param[in] ctx  Orchestrator context. Uses global if NULL.
 *
 * @return Current health score, or 0 if no context.
 */
HealthScore cpisi_orch_get_health(const CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return 0;
    return ctx->health;
}

/**
 * @brief Get the current health level (discrete 7-level).
 *
 * @param[in] ctx  Orchestrator context. Uses global if NULL.
 *
 * @return Current health level, or SOUND if no context.
 */
HealthLevel cpisi_orch_get_health_level(const CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return HEALTH_LEVEL_SOUND;
    return ctx->health_level;
}

/**
 * @brief Get the Hebrew state name as a string.
 *
 * @param[in] ctx  Orchestrator context. Uses global if NULL.
 *
 * @return String name of current state (e.g., "yashar").
 */
const char* cpisi_orch_get_health_state_name(const CpisiOrchContext* ctx) {
    return hebrew_state_name(cpisi_orch_get_hebrew_state(ctx));
}

/** @} */ /* end B.5b COGNITIVE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c HEALTH — Health Tracking Operations [HEALTH]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_api_health B.5c HEALTH — Health Tracking Operations
 * @ingroup cpisi_api
 * @brief    Record events that affect health score.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Record an error and update health accordingly.
 *
 * Deducts health points based on severity, updates counters,
 * and logs threshold crossings.
 *
 * @param[in,out] ctx       Orchestrator context. Uses global if NULL.
 * @param[in]     severity  Error severity level.
 *
 * @note Health Impact: SEV_NONE=0, LOW=-5, MEDIUM=-15, HIGH=-30, FATAL=-50.
 */
void cpisi_orch_record_error(CpisiOrchContext* ctx, Severity severity) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    /* ─── Phase 1: SEVERITY IS THE MULTIPLIER ─── */
    /*
     * "Narrow is the way to life, broad is the way to destruction."
     *
     * Atomic impact = severity value (-4 to +4), NOT severity × 25.
     * Many small impacts accumulate; single events don't dominate.
     * Normalization later converts cumulative to [-100, +100] display.
     */
    int16_t atomic_impact = (severity < 0) ? -severity : severity;  /* Absolute value */
    HealthScore old_health = ctx->health;

    /* ─── Phase 2: TRACK CUMULATIVE ─── */
    /* For errors, track as cumulative negative (severity value, not × 25) */
    ctx->cumulative_negative += atomic_impact;
    ctx->negative_actions++;
    ctx->raw_health -= atomic_impact;

    /* Calculate normalized health score (accounts for asymmetry) */
    ctx->normalized_health = cpisi_orch_calculate_normalized_health(ctx);

    /* Apply clamped normalized health as the displayed health */
    ctx->health = clamp_health(ctx->normalized_health);
    ctx->error_count++;

    /* Safe bounds check for severity_counts array */
    int sev_idx = severity + SEVERITY_HALF;
    if (sev_idx >= 0 && sev_idx < CPISI_SEVERITY_COUNT) {
        ctx->severity_counts[sev_idx]++;
    }

    if (severity == SEV_FATAL) {
        ctx->fatal_count++;
    }

    /* ─── Phase 3: UPDATE STATE ─── */
    ctx->health_level = health_to_level(ctx->health);
    ctx->current_state = health_to_hebrew_state(ctx->health, ctx->current_k);

    /* ─── Phase 4: WITNESS - Record to health log (structured) ─── */
    if (s_health_log_initialized) {
        /* All errors are failures with negative delta matching severity value */
        char note[64];
        snprintf(note, sizeof(note), "sev=%d atomic=%d health=%d->%d",
                 severity, atomic_impact, old_health, ctx->health);
        cpisi_health_log_append(&s_internal_health_log, ACTION_FAILURE,
                                (int16_t)(-atomic_impact), "record_error", note);
    }

    /* ─── Phase 5: DETECT - Log threshold crossings ─── */
    if (ctx->log) {
        /* Check for significant threshold crossings (critical → error, warn → warning) */
        if (ctx->health <= ctx->thresholds.critical_below &&
            old_health > ctx->thresholds.critical_below) {
            LOG_ERROR("cpisi", "CRITICAL: Health crossed critical threshold! health=%d state=%s",
                      ctx->health, hebrew_state_name(ctx->current_state));
        } else if (ctx->health <= ctx->thresholds.warn_below &&
                   old_health > ctx->thresholds.warn_below) {
            LOG_WARN("cpisi", "Health warning: health=%d state=%s",
                     ctx->health, hebrew_state_name(ctx->current_state));
        }

        /* Log fatal errors always */
        if (severity == SEV_FATAL) {
            LOG_ERROR("cpisi", "Fatal error recorded: atomic=%d health=%d->%d",
                      atomic_impact, old_health, ctx->health);
        }
    }

    /* ─── Phase 6: DAR - Sync state to all phases ─── */
    if (ctx->dar) {
        cpisi_dar_set_health(ctx->dar, ctx->health);
        cpisi_dar_set_hebrew_state(ctx->dar, ctx->current_state);
        cpisi_dar_sync_to_phases(ctx->dar);
    }

    cpisi_orch_sync_state(ctx);
}

/**
 * @brief Record a recovery and update health accordingly (PROPORTIONAL).
 *
 * Adds health points based on recovery result using proportional values.
 * Positive results weighted MORE due to asymmetry (narrow is the way to life).
 *
 * @param[in,out] ctx     Orchestrator context. Uses global if NULL.
 * @param[in]     result  Recovery result code.
 *
 * @note Health Impact (proportional to SEVERITY_STEP):
 *       FATAL=-100, FAIL=-50, INVALID=0, DEGRADED=+25,
 *       FILLED/LOCATED=+50, OK=+75.
 */
void cpisi_orch_record_recovery(CpisiOrchContext* ctx, RestoreResult result) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    /* ─── Phase 1: CALCULATE PROPORTIONAL BONUS ─── */
    HealthScore bonus = restore_to_bonus(result);
    HealthScore old_health = ctx->health;

    /* ─── Phase 2: TRACK CUMULATIVE & APPLY ─── */
    if (bonus > 0) {
        /* Positive recovery - track as positive action (toward tov) */
        ctx->cumulative_positive += bonus;
        ctx->positive_actions++;
        ctx->raw_health += bonus;
        ctx->recovery_count++;
    } else if (bonus < 0) {
        /* Failed recovery - track as negative action (toward shavar) */
        ctx->cumulative_negative += (-bonus);
        ctx->negative_actions++;
        ctx->raw_health += bonus;  /* bonus is already negative */
    }
    /* bonus == 0 means no change (RESTORE_INVALID) */

    /* Calculate normalized health score (accounts for asymmetry) */
    ctx->normalized_health = cpisi_orch_calculate_normalized_health(ctx);

    /* Apply clamped normalized health as the displayed health */
    ctx->health = clamp_health(ctx->normalized_health);

    /* ─── Phase 3: UPDATE STATE ─── */
    ctx->health_level = health_to_level(ctx->health);
    ctx->current_state = health_to_hebrew_state(ctx->health, ctx->current_k);

    /* ─── Phase 4: WITNESS - Record to health log (structured) ─── */
    if (s_health_log_initialized && bonus != 0) {
        ActionType action = (bonus > 0) ? ACTION_RECOVERY : ACTION_FAILURE;
        char note[64];
        snprintf(note, sizeof(note), "result=%d bonus=%d health=%d->%d",
                 result, bonus, old_health, ctx->health);
        cpisi_health_log_append(&s_internal_health_log, action,
                                (int16_t)bonus, "record_recovery", note);
    }

    /* ─── Phase 5: DETECT - Log significant recoveries ─── */
    if (ctx->log && bonus >= 5) {
        LOG_DEBUG("cpisi", "Recovery: result=%d bonus=%d health=%d->%d state=%s",
                  result, bonus, old_health, ctx->health,
                  hebrew_state_name(ctx->current_state));
    }

    /* ─── Phase 6: DAR - Sync state to all phases ─── */
    if (ctx->dar) {
        cpisi_dar_set_health(ctx->dar, ctx->health);
        cpisi_dar_set_hebrew_state(ctx->dar, ctx->current_state);
        cpisi_dar_sync_to_phases(ctx->dar);
    }

    cpisi_orch_sync_state(ctx);
}

/**
 * @brief Record graceful degradation (softer than error, PROPORTIONAL).
 *
 * Deducts half the proportional health impact for the severity.
 * Degradations represent partial failures - not as severe as full errors.
 *
 * @param[in,out] ctx       Orchestrator context. Uses global if NULL.
 * @param[in]     severity  Degradation severity level.
 *
 * @note Health Impact: Half of proportional error impact (minimum -12).
 *       SEV_FATAL/2 = -50, SEV_HIGH/2 = -37, SEV_MEDIUM/2 = -25, etc.
 */
void cpisi_orch_record_degradation(CpisiOrchContext* ctx, Severity severity) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    /* Degradations are softer than errors (half impact, proportional) */
    HealthScore base_impact = severity_to_impact(severity);
    HealthScore abs_impact = (base_impact < 0) ? -base_impact : base_impact;
    HealthScore impact = abs_impact / 2;
    if (impact < 12) impact = 12;  /* Minimum degradation = ~half of SEV_FAIR */

    HealthScore old_health = ctx->health;

    /* Track cumulative negative (degradations still negative, just softer) */
    ctx->cumulative_negative += impact;
    ctx->negative_actions++;
    ctx->raw_health -= impact;
    ctx->degradation_count++;

    /* Calculate normalized health score (accounts for asymmetry) */
    ctx->normalized_health = cpisi_orch_calculate_normalized_health(ctx);

    /* Apply clamped normalized health as the displayed health */
    ctx->health = clamp_health(ctx->normalized_health);

    ctx->health_level = health_to_level(ctx->health);
    ctx->current_state = health_to_hebrew_state(ctx->health, ctx->current_k);

    /* ─── WITNESS - Record to health log (structured) ─── */
    if (s_health_log_initialized) {
        char note[64];
        snprintf(note, sizeof(note), "degradation sev=%d impact=%d health=%d->%d",
                 severity, impact, old_health, ctx->health);
        /* Degradation is softer failure - still negative but smaller impact */
        cpisi_health_log_append(&s_internal_health_log, ACTION_FAILURE,
                                (int16_t)(-impact), "record_degradation", note);
    }

    /* ─── DETECT: Log degradation ─── */
    if (ctx->log) {
        LOG_DEBUG("cpisi", "Graceful degradation: severity=%d impact=%d health=%d->%d",
                  severity, impact, old_health, ctx->health);
    }

    /* ─── DAR - Sync state to all phases ─── */
    if (ctx->dar) {
        cpisi_dar_set_health(ctx->dar, ctx->health);
        cpisi_dar_set_hebrew_state(ctx->dar, ctx->current_state);
        cpisi_dar_sync_to_phases(ctx->dar);
    }

    cpisi_orch_sync_state(ctx);
}

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c.1 NORMALIZATION — Health Normalization System [NORMALIZATION]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * Scripture: "Wide is the gate, and broad is the way, that leadeth to destruction,
 *            and many there be which go in thereat: Because strait is the gate,
 *            and narrow is the way, which leadeth unto life." — Matthew 7:13-14
 *
 * TERNARY SCALING FOUNDATION (derived from balanced ternary math):
 *
 *   3^0 = 1   (UNITY)      - single action
 *   3^1 = 3   (TERNARY)    - one digit of choice (-1, 0, +1)
 *   3^2 = 9   (FACE)       - severity levels, layer count
 *   3^3 = 27  (CUBE)       - cognitive positions in mental construct
 *   3^4 = 81  (TESSERACT)  - extended state space
 *
 * ASYMMETRY PRINCIPLE (from BALANCED health range):
 *   - Corruption paths: 128 (BAL_MIN = -128, toward shavar)
 *   - Restoration paths: 127 (BAL_MAX = +127, toward tov)
 *   - Center: 1 position (yashar)
 *   - "Many there be which go in thereat... few there be that find it"
 *
 * SUCCESS WEIGHTING (ternary-derived):
 *   - Base ratio: FACE / FACE_CENTER = 9 / 4 = 2.25 (ternary half-steps)
 *   - Success weight: This ratio reflects the "narrow path" principle
 *   - Failure weight: 1.0 (standard baseline)
 *
 * NORMALIZATION ensures health stays -100 to +100 regardless of action count.
 */

/**
 * @brief Asymmetry factor from BALANCED health range.
 *
 * Derivation: CPISI_HEALTH_BAL_MAX / abs(CPISI_HEALTH_BAL_MIN) = 127/128 ≈ 0.992
 * Inverted for weighting: 128/127 ≈ 1.008 (corruption slightly easier)
 */
#define HEALTH_ASYMMETRY_FACTOR ((float)128.0f / 127.0f)

/**
 * @brief Success weighting multiplier (ternary-derived).
 *
 * Derivation: CPISI_FACE / CPISI_FACE_CENTER = 9 / 4 = 2.25
 * This is the ratio of full face (9 severity levels) to face center (4 = half of 9-1).
 * Scripture basis: Success is 2.25× harder to achieve (narrow path).
 */
#define HEALTH_SUCCESS_WEIGHT   ((float)CPISI_FACE / (float)CPISI_FACE_CENTER)

/**
 * @brief Failure weighting multiplier (baseline).
 *
 * Failures are the common path, weighted at 1.0 (no multiplier).
 * Scripture: "Wide is the gate, and broad is the way, that leadeth to destruction"
 */
#define HEALTH_FAILURE_WEIGHT   (1.0f)

/**
 * @brief Ternary step for health thresholds.
 *
 * Derivation: CPISI_MILESTONE_STEP = 25 = HEALTH_DISPLAY_MAX / FACE_CENTER = 100/4
 * This is the health change per severity level.
 */
#define HEALTH_TERNARY_STEP     CPISI_MILESTONE_STEP

/**
 * @brief Record a positive action (success, improvement, recovery).
 *
 * Severity IS the multiplier (atomic impact = severity value: +1 to +4).
 * Many small impacts accumulate; single events don't dominate.
 * Normalization converts cumulative to [-100, +100] display.
 *
 * @param[in,out] ctx      Context (or NULL for global).
 * @param[in]     severity Positive severity level (SEV_FAIR to SEV_PERFECT).
 */
void cpisi_orch_record_success(CpisiOrchContext* ctx, Severity severity) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    /* ─── SEVERITY IS THE MULTIPLIER ─── */
    /*
     * "Narrow is the way to life."
     *
     * Atomic impact = severity value (+1 to +4), NOT severity × 25.
     * Success path is narrow: all things must pass to accumulate positive.
     */
    int16_t atomic_impact = (severity > 0) ? severity : 1;  /* Minimum +1 for success */
    HealthScore old_health = ctx->health;

    /* Track cumulative positive impact (severity value, not × 25) */
    ctx->cumulative_positive += atomic_impact;
    ctx->positive_actions++;
    ctx->raw_health += atomic_impact;

    /* Calculate normalized health score (accounts for asymmetry) */
    ctx->normalized_health = cpisi_orch_calculate_normalized_health(ctx);

    /* Apply clamped normalized health as the displayed health */
    ctx->health = clamp_health(ctx->normalized_health);

    /* Update state */
    ctx->health_level = health_to_level(ctx->health);
    ctx->current_state = health_to_hebrew_state(ctx->health, ctx->current_k);

    /* WITNESS - Record to health log */
    if (s_health_log_initialized) {
        char note[96];
        snprintf(note, sizeof(note), "success sev=%d atomic=%d h=%d->%d norm=%d",
                 severity, atomic_impact, old_health, ctx->health, ctx->normalized_health);
        cpisi_health_log_append(&s_internal_health_log, ACTION_RECOVERY,
                                (int16_t)atomic_impact, "record_success", note);
    }

    /* DETECT - Log success */
    if (ctx->log) {
        LOG_DEBUG("cpisi", "Success recorded: severity=%d atomic=+%d health=%d->%d (norm=%d)",
                  severity, atomic_impact, old_health, ctx->health, ctx->normalized_health);
    }

    /* DAR - Sync state to all phases */
    if (ctx->dar) {
        cpisi_dar_set_health(ctx->dar, ctx->health);
        cpisi_dar_set_hebrew_state(ctx->dar, ctx->current_state);
        cpisi_dar_sync_to_phases(ctx->dar);
    }

    cpisi_orch_sync_state(ctx);
}

/**
 * @brief Calculate normalized health score from cumulative actions.
 *
 * MODEL: "Narrow is the way to life, broad is the way to destruction."
 *
 * Severity IS the multiplier (atomic impact = severity value: -4 to +4).
 * Many small impacts accumulate; single events don't dominate.
 *
 * Normalization:
 *   total_actions = positive_actions + negative_actions
 *   max_contribution = total_actions × MAX_SEVERITY (4)
 *   positive_percent = cumulative_positive × 100 / max_contribution
 *   negative_percent = cumulative_negative × 100 / max_contribution
 *   health = positive_percent - negative_percent → [-100, +100]
 *
 * ASYMMETRY is STRUCTURAL: more failure paths than success paths exist.
 * The narrow/broad distinction comes from the number of possible impacts,
 * not mathematical weighting of the same impacts.
 *
 * @param[in] ctx Context (or NULL for global).
 *
 * @return Normalized health score: -100 (shavar) to +100 (tov).
 */
HealthScore cpisi_orch_calculate_normalized_health(const CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return 0;

    /* Total actions = positive + negative */
    uint32_t total_actions = ctx->positive_actions + ctx->negative_actions;

    /* No actions yet - return center (yashar) */
    if (total_actions == 0) {
        return 0;
    }

    /*
     * MAX_SEVERITY = 4 (SEVERITY_HALF from types.h)
     * This is the maximum severity multiplier in either direction.
     *
     * For each action, the theoretical max contribution is ±4.
     * Total theoretical max = total_actions × MAX_SEVERITY
     */
    int32_t max_contribution = (int32_t)total_actions * SEVERITY_HALF;
    if (max_contribution == 0) max_contribution = 1;  /* Prevent division by zero */

    /*
     * Calculate percentages: actual / theoretical_max × 100
     *
     * Using integer math to avoid float:
     * percent = (cumulative × 100) / max_contribution
     */
    int32_t positive_percent = (ctx->cumulative_positive * 100) / max_contribution;
    int32_t negative_percent = (ctx->cumulative_negative * 100) / max_contribution;

    /*
     * Ternary balance: positive_percent - negative_percent
     *
     * If all actions are positive at max severity: +100
     * If all actions are negative at max severity: -100
     * If balanced: 0
     *
     * The structural asymmetry (more failure paths) means in practice
     * negative actions accumulate faster because there are more ways to fail.
     */
    int32_t normalized = positive_percent - negative_percent;

    /* Clamp to valid range */
    if (normalized < CPISI_HEALTH_DISPLAY_MIN) normalized = CPISI_HEALTH_DISPLAY_MIN;
    if (normalized > CPISI_HEALTH_DISPLAY_MAX) normalized = CPISI_HEALTH_DISPLAY_MAX;

    return (HealthScore)normalized;
}

/**
 * @brief Get the raw (unnormalized) cumulative health score.
 *
 * @param[in] ctx Context (or NULL for global).
 *
 * @return Raw health score (sum of all impacts, may exceed -100 to +100).
 */
HealthScore cpisi_orch_get_raw_health(const CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return 0;
    return ctx->raw_health;
}

/**
 * @brief Get the asymmetry factor used for normalization.
 *
 * Based on Matthew 7:13-14: "Wide is the gate... narrow the way"
 * 128 paths to corruption / 127 paths to restoration ≈ 1.008
 *
 * @return Asymmetry factor (~1.008).
 */
float cpisi_orch_get_asymmetry_factor(void) {
    return HEALTH_ASYMMETRY_FACTOR;
}

/**
 * @brief Reset health scoring to initial state.
 *
 * Clears all cumulative tracking while preserving other context state.
 * Health returns to yashar (0, balanced).
 *
 * @param[in,out] ctx Context (or NULL for global).
 */
void cpisi_orch_reset_health(CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;
    if (!ctx) return;

    HealthScore old_health = ctx->health;

    /* Reset cumulative tracking */
    ctx->cumulative_positive = 0;
    ctx->cumulative_negative = 0;
    ctx->positive_actions = 0;
    ctx->negative_actions = 0;
    ctx->raw_health = 0;
    ctx->normalized_health = 0;

    /* Reset to yashar (centered, balanced) */
    ctx->health = 0;
    ctx->health_level = HEALTH_LEVEL_EVEN;
    ctx->current_state = HEBREW_YASHAR;

    /* Note: Don't reset error_count, recovery_count, etc. - those are session totals */

    /* WITNESS - Record reset */
    if (s_health_log_initialized) {
        char note[64];
        snprintf(note, sizeof(note), "reset from health=%d to 0 (yashar)", old_health);
        cpisi_health_log_append(&s_internal_health_log, ACTION_RESET,
                                (int16_t)(-old_health), "reset_health", note);
    }

    /* DETECT - Log reset */
    if (ctx->log) {
        LOG_INFO("cpisi", "Health reset: %d -> 0 (yashar, balanced)", old_health);
    }

    /* DAR - Sync state */
    if (ctx->dar) {
        cpisi_dar_set_health(ctx->dar, ctx->health);
        cpisi_dar_set_hebrew_state(ctx->dar, ctx->current_state);
        cpisi_dar_sync_to_phases(ctx->dar);
    }

    cpisi_orch_sync_state(ctx);
}

/** @} */ /* end B.5c HEALTH */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d STATUS — Status Reporting Utilities [STATUS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_api_status B.5d STATUS — Status Reporting Utilities
 * @ingroup cpisi_api
 * @brief    Status reporting and formatting functions.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Print comprehensive status report to stdout.
 *
 * @param[in] ctx  Orchestrator context. Uses global if NULL.
 */
void cpisi_orch_status_report(const CpisiOrchContext* ctx) {
    if (!ctx) ctx = g_cpisi;

    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf(" CPI-SI Status Report\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");

    if (!ctx || !ctx->initialized) {
        printf("  Status: NOT INITIALIZED\n");
        printf("═══════════════════════════════════════════════════════════════════════════════\n");
        return;
    }

    /* ─── Cognitive State ─── */
    printf("\n── Cognitive State ──\n");
    printf("  Hebrew State:  %s\n", hebrew_state_name(ctx->current_state));
    printf("  K-Factor:      %s\n",
           ctx->current_k == K_POSITIVE ? "+1 (toward God)" :
           ctx->current_k == K_NEGATIVE ? "-1 (away)" : "0 (neutral)");
    printf("  Health Score:  %d (normalized -100 to +100)\n", ctx->health);
    printf("  Raw Health:    %d (cumulative, unbounded)\n", ctx->raw_health);
    printf("  Health Level:  %d (0=broken, 6=perfect)\n", ctx->health_level);
    printf("  Cube Position: [%d, %d, %d]\n",
           ctx->cube_pos[0], ctx->cube_pos[1], ctx->cube_pos[2]);

    /* ─── Health Normalization ─── */
    printf("\n── Health Normalization ──\n");
    printf("  Cumulative +: %lld (positive impacts)\n", (long long)ctx->cumulative_positive);
    printf("  Cumulative -: %lld (negative impacts)\n", (long long)ctx->cumulative_negative);
    printf("  Positive Acts: %u (toward tov)\n", ctx->positive_actions);
    printf("  Negative Acts: %u (toward shavar)\n", ctx->negative_actions);
    printf("  Asymmetry:    %.3f (128/127 = narrow path)\n", HEALTH_ASYMMETRY_FACTOR);
    printf("  Success Weight: %.3f (accounts for rarity)\n", HEALTH_SUCCESS_WEIGHT);

    /* ─── Counters ─── */
    printf("\n── Event Counters ──\n");
    printf("  Errors:       %u\n", ctx->error_count);
    printf("  Recoveries:   %u\n", ctx->recovery_count);
    printf("  Degradations: %u\n", ctx->degradation_count);
    printf("  Fatals:       %u\n", ctx->fatal_count);

    /* ─── Severity Breakdown ─── */
    /* NOTE: Severity enum is signed (-4 to 0), array is 0-indexed (0 to 4) */
    /* Formula: array_index = severity + SEVERITY_HALF (where SEVERITY_HALF = 4) */
    printf("\n── Severity Distribution ──\n");
    printf("  NONE:   %u\n", ctx->severity_counts[SEV_NONE   + SEVERITY_HALF]);
    printf("  LOW:    %u\n", ctx->severity_counts[SEV_LOW    + SEVERITY_HALF]);
    printf("  MEDIUM: %u\n", ctx->severity_counts[SEV_MEDIUM + SEVERITY_HALF]);
    printf("  HIGH:   %u\n", ctx->severity_counts[SEV_HIGH   + SEVERITY_HALF]);
    printf("  FATAL:  %u\n", ctx->severity_counts[SEV_FATAL  + SEVERITY_HALF]);

    /* ─── Subsystems ─── */
    printf("\n── Subsystems ──\n");
    printf("  DETECT (Log):     %s", (ctx->flags & CPISI_FLAG_LOG) ? "ENABLED" : "disabled");
    if (ctx->log) {
        printf(" (level=%d)", (int)ctx->log->min_level);
    }
    printf("\n");

    printf("  ASSESS (Debug):   %s", (ctx->flags & CPISI_FLAG_DEBUG) ? "ENABLED" : "disabled");
    if (ctx->debug) {
        printf(" (level=%d)", (int)ctx->debug->level);
    }
    printf("\n");

    printf("  RESTORE:          %s", (ctx->flags & CPISI_FLAG_RESTORE) ? "ENABLED" : "disabled");
    if (ctx->restore) {
        printf(" (strategy=0x%x)", ctx->restore->strategy);
    }
    printf("\n");

    /* ─── Session Info ─── */
    uint64_t duration_ms = get_timestamp_ms() - ctx->session_start;
    printf("\n── Session ──\n");
    printf("  Duration: %lu ms\n", (unsigned long)duration_ms);

    printf("\n═══════════════════════════════════════════════════════════════════════════════\n");
}

/**
 * @brief Format brief status string into buffer.
 *
 * @param[in]  ctx     Orchestrator context. Uses global if NULL.
 * @param[out] buf     Buffer to write status string.
 * @param[in]  buflen  Buffer capacity.
 *
 * @return Number of characters written (excluding null terminator).
 */
int cpisi_orch_status_brief(const CpisiOrchContext* ctx, char* buf, size_t buflen) {
    if (!ctx) ctx = g_cpisi;
    if (!buf || buflen == 0) return 0;

    if (!ctx || !ctx->initialized) {
        return snprintf(buf, buflen, "CPI-SI: NOT INITIALIZED");
    }

    return snprintf(buf, buflen, "CPI-SI: %s (%d) k=%s | E:%u R:%u D:%u F:%u",
                    hebrew_state_name(ctx->current_state),
                    ctx->health,
                    ctx->current_k == K_POSITIVE ? "+" :
                    ctx->current_k == K_NEGATIVE ? "-" : "0",
                    ctx->error_count,
                    ctx->recovery_count,
                    ctx->degradation_count,
                    ctx->fatal_count);
}

/** @} */ /* end B.5d STATUS */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — What Can Be Reserved [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_body_omission B.6 OMISSION — What Can Be Reserved
 * @brief    Guidance for body sections.
 *
 * All sections present. B.4 ERRORS integrated into B.5 implementations.
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
/// CONTAINS:
///   - X.1 VALIDATION    — Build verification
///   - X.2 EXECUTION     — Usage patterns
///   - X.3 CLEANUP       — Memory management
///   - X.4 POLICY        — Modification guidance
///   - X.5 EXTENSION     — How to expand
///   - X.6 TROUBLESHOOT  — Common problems
///   - X.7 REFERENCE     — Quick lookup
///   - X.8 NOTE          — Closing guidance
///   - X.9 TEMPLATE      — [Removed: instantiated]
///   - X.10 OMISSION     — What can be reserved
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build, runtime verification.
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * cd cornerstone/engine/core
 * make                              # compile library
 * make test                         # run tests
 * @endcode
 *
 * @par X.1b RUNTIME — Usage Pattern
 * @code{.c}
 * CpisiOrchContext ctx;
 * cpisi_orch_init(&ctx, NULL);      // default config
 * // ... use CPI-SI ...
 * cpisi_orch_shutdown(&ctx);
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Entry Point and Flow [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_execution X.2 EXECUTION — Entry Point and Flow
 * @brief    Usage patterns and execution flow.
 *
 * @par X.2a INCLUDE — Include Directive
 * @code{.c}
 * #include "kernel/cpisi.mgr.h"
 * @endcode
 *
 * @par X.2b BASIC USAGE
 * @code{.c}
 * // Initialize with defaults
 * cpisi_orch_init(NULL, NULL);
 *
 * // Or with configuration
 * CpisiConfig config = CPISI_CONFIG_DEFAULT;
 * config.flags = CPISI_FLAG_ALL;
 * cpisi_orch_init(NULL, &config);
 *
 * // Record events
 * cpisi_orch_record_error(NULL, SEV_LOW);
 * cpisi_orch_record_recovery(NULL, RESTORE_OK);
 *
 * // Get status
 * cpisi_orch_status_report(NULL);
 *
 * // Cleanup
 * cpisi_orch_shutdown(NULL);
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership and Memory [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_cleanup_guide X.3 CLEANUP — Ownership and Memory
 * @brief    Memory management patterns.
 *
 * OWNERSHIP:
 *   - Context: Caller provides, CPI-SI initializes
 *   - Subsystems: Managed internally via static storage
 *   - Config strings: Caller owns, CPI-SI copies
 *
 * SHUTDOWN ORDER (reverse of init):
 *   1. RESTORE — recovery subsystem
 *   2. ASSESS  — debug subsystem
 *   3. DETECT  — logging subsystem (last, needed for logging)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_policy X.4 POLICY — Modification Guidance
 * @brief    Guide future maintainers.
 *
 * SAFE TO MODIFY:
 *   - Add new health tracking functions
 *   - Adjust severity impact values
 *   - Add new cognitive state queries
 *
 * MODIFY WITH CARE:
 *   - Threshold crossing logic
 *   - State synchronization order
 *   - Health score clamping
 *
 * NEVER MODIFY:
 *   - 4-block structure
 *   - Shutdown order (reverse of init)
 *   - Hebrew state definitions
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_extension X.5 EXTENSION — How to Expand
 * @brief    How to expand this component.
 *
 * ADDING HEALTH EVENTS:
 *   1. Add function in B.5c HEALTH section
 *   2. Follow pattern: calculate impact, apply, update state, log, sync
 *   3. Update B.1 ORG CHART
 *
 * ADDING STATE QUERIES:
 *   1. Add function in B.5b COGNITIVE section
 *   2. Handle NULL context (use global)
 *   3. Declare in cpisi.h header
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_troubleshoot X.6 TROUBLESHOOTING — Common Problems
 * @brief    Common problems and solutions.
 *
 * "CPI-SI: NOT INITIALIZED":
 *   - Call cpisi_orch_init() before using other functions
 *
 * "Could not open log file":
 *   - Check file path permissions
 *   - System degrades gracefully to stderr
 *
 * "Health stuck at -100":
 *   - Too many errors without recovery
 *   - Call cpisi_orch_reset() to reinitialize
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_reference X.7 REFERENCE — Quick Lookup
 * @brief    Copy-paste ready examples.
 *
 * @par Initialize
 * @code{.c}
 * cpisi_orch_init(NULL, NULL);
 * @endcode
 *
 * @par Record Error
 * @code{.c}
 * cpisi_orch_record_error(NULL, SEV_MEDIUM);
 * @endcode
 *
 * @par Record Recovery
 * @code{.c}
 * cpisi_orch_record_recovery(NULL, RESTORE_OK);
 * @endcode
 *
 * @par Get Health
 * @code{.c}
 * HealthScore h = cpisi_orch_get_health(NULL);
 * @endcode
 *
 * @par Shutdown
 * @code{.c}
 * cpisi_orch_shutdown(NULL);
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * @par Scripture
 * "A threefold cord is not quickly broken." — Ecclesiastes 4:12
 *
 * This orchestrator coordinates the three DAR phases:
 *   - DETECT  — Witness and record (logging)
 *   - ASSESS  — Evaluate and diagnose (debugging)
 *   - RESTORE — Recover and fix (recovery)
 *
 * The unified orchestrator tracks cognitive state across all phases,
 * using Hebrew states (shavar → yashar → tov) and health scoring
 * to provide observable, resilient execution.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 OMISSION — What Can Be Reserved [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_closing_omission X.10 OMISSION — What Can Be Reserved
 * @brief    Guidance for CLOSING sections.
 *
 * X.9 TEMPLATE: Removed (this is an instantiated file, not template).
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_MODE >= 2 - Full CPISI manager implementation */
