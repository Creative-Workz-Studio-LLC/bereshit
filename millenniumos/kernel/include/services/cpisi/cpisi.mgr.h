/// @file cpisi.h
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
/// @omni:req  key        = CPISI-UNIFIED-ORCHESTRATOR
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = b-03.10
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = orchestrator
/// @omni:inh  role       = unified
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = orchestration
/// @omni:ins  includes   = [dar/dar.h, io/config.h]
/// @omni:ins  provides   = [CpisiOrchContext, g_cpisi, lifecycle, state, health, hooks, persistence]
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
/// @omni:req  brief      = Single include for all CPI-SI — orchestrates DAR with cognitive state
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
/// ═══════════════════════════════════════════════════════════════════════════════

/** SP.1 CONFIG — [Reserved: no POSIX features needed] @defgroup cpisi_special @{ @} */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * INCLUDE GUARD — Identity protection
 * ───────────────────────────────────────────────────────────────────────────── */

#ifndef CPISI_H
/** @def CPISI_H
 *  @brief Include guard — prevents multiple inclusion of unified orchestrator.
 */
#define CPISI_H

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
 * @file      cpisi.h
 * @brief     CPI-SI Unified System Orchestrator — single include for all CPI-SI.
 *
 * @defgroup cpisi_identity M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       CPISI-UNIFIED-ORCHESTRATOR
 * Title:     CPI-SI Unified System Orchestrator
 * Type:      Header (Orchestrator)
 * Component: Unified — coordinates all DAR phases with cognitive state
 * Role:      Single include point, cognitive state tracking, health coordination
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
 * @version   b-03.10
 * @date      2026-01-25
 *
 * Status:    Active
 * Created:   2026-01-18
 * Updated:   2026-01-25
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authors and Copyright [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_attribution M.3 ATTRIBUTION — Authors and Copyright
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
 * @defgroup cpisi_location M.4 LOCATION — File Path
 * @{
 *
 * Path: cornerstone/engine/core/include/cpisi/cpisi.h
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_derivation M.5 DERIVATION — Lineage
 * @{
 *
 * Template:     bereshit/word/seed/code/c/header.h
 * Derives_from: dar/dar.h (DAR system), io/config.h (configuration)
 *
 * Architecture:
 *   types/ (foundation)
 *     ↓
 *   dar/ (phases: detect, assess, restore)
 *     ↓
 *   dar/dar.h (DAR group header)
 *     ↓
 *   io/config.h, io/json.h (IO layer)
 *     ↓
 *   cpisi.h (THIS — unified orchestrator)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Tags [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_classification M.6 CLASSIFICATION — Tags
 * @{
 *
 * Tags: orchestrator, unified, dar, cognitive, health, state, context
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Unified orchestration of all CPI-SI subsystems with cognitive state.
 * Core Design: Three DAR phases coordinated through single context with health tracking.
 * Philosophy:  "A threefold cord is not quickly broken" — strength through coordination.
 *
 * Key Features:
 *   - Single include point for all CPI-SI functionality
 *   - CpisiOrchContext tracks unified cognitive state
 *   - Health coordination across DETECT, ASSESS, RESTORE
 *   - TOML configuration loading via io/config.h
 *   - Event hooks for observable state transitions
 *   - State persistence (save/load) across sessions
 *   - Convenience macros for common operations
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
 * Scripture:  Ecclesiastes 4:12 — "A threefold cord is not quickly broken"
 * Principle:  The three DAR phases strengthen each other through coordination.
 * Anchor:     Proverbs 15:22 — "Without counsel purposes are disappointed"
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_dependencies M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Internal: dar/dar.h (complete DAR system)
 *   Internal: io/config.h (CpisiFlags, CpisiConfig, loading)
 *   Standard: stdint.h, stdbool.h, stddef.h
 *
 * What Uses This:
 *   - Application code (main entry point for CPI-SI)
 *   - Test harnesses
 *   - REPL and socket modes
 *
 * @see dar/dar.h for DAR phase system
 * @see io/config.h for configuration types
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-18) — Initial unified orchestrator
 *   a-02.00 (2026-01-22) — Added cognitive state tracking
 *   b-03.00 (2026-01-25) — Full header.h template alignment
 *   b-03.10 (2026-01-25) — Added event hooks and state persistence
 *
 * Current:
 *   ✓ CpisiOrchContext with full cognitive state
 *   ✓ DAR phase coordination
 *   ✓ Health tracking across phases
 *   ✓ TOML configuration loading
 *   ✓ Convenience macros
 *   ✓ Event hooks for observable state transitions
 *   ✓ State persistence (save/load) across sessions
 *
 * Planned:
 *   ⏳ Hook filtering by severity/threshold
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
///   - S.2 DEFINES   — [Reserved]
///   - S.3 TYPES     — CpisiOrchContext
///   - S.4 PROTOS    — [Reserved]
///   - S.5 FILE-LEVEL — Global context
///   - S.6 OMISSION  — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
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

#include <stdbool.h>  /**< Boolean type: bool, true, false */
#include <stddef.h>   /**< Standard definitions: size_t, NULL */
#include <stdint.h>   /**< Fixed-width integers: int32_t, uint32_t, uint64_t */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_internal S.1b INTERNAL — Project Headers
 * @ingroup cpisi_includes
 * @brief    CPI-SI project headers required by this module.
 *
 * WHY: The orchestrator coordinates DAR phases (dar/dar.h) using configuration
 *      types (io/config.h). These provide all types needed for unified operation.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "witness/manager/dar.mgr.h"              /**< Complete DAR system — detect, assess, restore phases */
#include "services/cpisi/config/config.cfg.h"     /**< CpisiFlags, CpisiConfig, CpisiConfigError, loading */
#include "services/cpisi/format/json.fmt.h"       /**< JSON output — cpisi_json_* functions for communication */
#include "services/cpisi/time/clock.time.h"       /**< Internal clock — circadian, duration, activity tracking */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [Reserved: no external libraries] @defgroup cpisi_external @ingroup cpisi_includes @{ @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_cpp S.1d CPP — C++ Linkage
 * @ingroup cpisi_includes
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
 * @defgroup cpisi_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Event hook constants for compile-time use.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a EVENTS — Event Type Constants [EVENTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_event_defines S.2a EVENTS — Event Type Constants
 * @ingroup cpisi_defines
 * @brief    Preprocessor defines for event hook types.
 *
 * WHY: Enables compile-time event masks, switch statements, and preprocessor
 *      conditionals. Matches CpisiEventType enum values.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Sentinel */
#define CPISI_EVT_NONE              0   /**< No event (sentinel). */

/* State transitions (1-9) */
#define CPISI_EVT_HEBREW_CHANGED    1   /**< HebrewState changed. */
#define CPISI_EVT_KFACTOR_CHANGED   2   /**< KFactor changed. */
#define CPISI_EVT_HEALTH_CHANGED    3   /**< HealthScore changed. */
#define CPISI_EVT_CUBE_MOVED        4   /**< 27-cube position changed. */

/* Health events (10-19) */
#define CPISI_EVT_ERROR_RECORDED   10   /**< Error was recorded. */
#define CPISI_EVT_RECOVERY_DONE    11   /**< Recovery completed. */
#define CPISI_EVT_DEGRADATION      12   /**< Graceful degradation occurred. */

/* Lifecycle events (20-29) */
#define CPISI_EVT_INIT_COMPLETE    20   /**< Initialization finished. */
#define CPISI_EVT_SHUTDOWN_START   21   /**< Shutdown beginning. */
#define CPISI_EVT_STATE_LOADED     22   /**< State loaded from persistence. */
#define CPISI_EVT_STATE_SAVED      23   /**< State saved to persistence. */

/* Count */
#define CPISI_EVT_COUNT            24   /**< Number of event types. */

/** @} */ /* end S.2a EVENTS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b HOOKS — Hook Configuration [HOOKS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_hook_defines S.2b HOOKS — Hook Configuration
 * @ingroup cpisi_defines
 * @brief    Hook system configuration constants.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#define CPISI_HOOKS_PER_EVENT       8   /**< Max hooks per event type. */
#define CPISI_STATE_PATH_MAX      256   /**< Max path length for state file. */

/** @} */ /* end S.2b HOOKS */

/** S.2c-f — [Reserved] @defgroup cpisi_s2c_reserved @ingroup cpisi_defines @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_types S.3 TYPES — Type Declarations
 * @brief    CpisiOrchContext — unified orchestrator context.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a FORWARD DECLARATIONS [FORWARDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_forwards S.3a FORWARD DECLARATIONS
 * @ingroup cpisi_types
 * @brief    Forward declarations for circular dependency resolution.
 *
 * WHY: CpisiOrchContext contains CpisiHookEntry (needs complete type for array),
 *      CpisiHookEntry contains CpisiEventHook (needs complete type),
 *      CpisiEventHook takes CpisiOrchContext* (only needs forward declaration).
 *
 *      Order: Forward CpisiOrchContext → Define callbacks → Define full CpisiOrchContext
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Forward declaration for CpisiOrchContext (needed by CpisiEventHook) */
typedef struct CpisiOrchContext CpisiOrchContext;

/** @} */ /* end S.3a FORWARD DECLARATIONS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b CALLBACKS — Event Hook Types [CALLBACKS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_callbacks S.3b CALLBACKS — Event Hook Types
 * @ingroup cpisi_types
 * @brief    Event types and callback signatures for state transition hooks.
 *
 * WHY: Observable system — callers can subscribe to state changes, errors,
 *      and recovery events without polling or modifying core code.
 *
 * ORDER: Must be defined BEFORE CpisiOrchContext (which uses CpisiHookEntry array).
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Event types that can trigger hooks.
 *
 * Hooks fire AFTER the event has occurred. Handlers receive the new state.
 * Values match CPISI_EVT_* defines from S.2a for preprocessor compatibility.
 */
typedef enum CpisiEventType {
    CPISI_EVENT_NONE            = CPISI_EVT_NONE,            /**< No event (sentinel). */

    /* State transitions */
    CPISI_EVENT_HEBREW_CHANGED  = CPISI_EVT_HEBREW_CHANGED,  /**< HebrewState changed. */
    CPISI_EVENT_KFACTOR_CHANGED = CPISI_EVT_KFACTOR_CHANGED, /**< KFactor changed. */
    CPISI_EVENT_HEALTH_CHANGED  = CPISI_EVT_HEALTH_CHANGED,  /**< HealthScore changed. */
    CPISI_EVENT_CUBE_MOVED      = CPISI_EVT_CUBE_MOVED,      /**< 27-cube position changed. */

    /* Health events */
    CPISI_EVENT_ERROR_RECORDED  = CPISI_EVT_ERROR_RECORDED,  /**< Error was recorded. */
    CPISI_EVENT_RECOVERY_DONE   = CPISI_EVT_RECOVERY_DONE,   /**< Recovery completed. */
    CPISI_EVENT_DEGRADATION     = CPISI_EVT_DEGRADATION,     /**< Graceful degradation occurred. */

    /* Lifecycle events */
    CPISI_EVENT_INIT_COMPLETE   = CPISI_EVT_INIT_COMPLETE,   /**< Initialization finished. */
    CPISI_EVENT_SHUTDOWN_START  = CPISI_EVT_SHUTDOWN_START,  /**< Shutdown beginning. */
    CPISI_EVENT_STATE_LOADED    = CPISI_EVT_STATE_LOADED,    /**< State loaded from persistence. */
    CPISI_EVENT_STATE_SAVED     = CPISI_EVT_STATE_SAVED,     /**< State saved to persistence. */

    /* Count */
    CPISI_EVENT_COUNT           = CPISI_EVT_COUNT            /**< Number of event types. */
} CpisiEventType;

/**
 * @brief Event data passed to hook callbacks.
 */
typedef struct CpisiEventData {
    CpisiEventType event;             /**< Which event occurred. */
    int32_t        old_value;         /**< Previous value (context-dependent). */
    int32_t        new_value;         /**< New value (context-dependent). */
    const char*    message;           /**< Optional message (may be NULL). */
} CpisiEventData;

/**
 * @brief Hook callback signature.
 *
 * @param[in] ctx       Orchestrator context (never NULL).
 * @param[in] event     Event data with old/new values.
 * @param[in] user_data User-provided context from registration.
 */
typedef void (*CpisiEventHook)(
    const CpisiOrchContext* ctx,
    const CpisiEventData*   event,
    void*                   user_data
);

/**
 * @brief Hook registration entry.
 */
typedef struct CpisiHookEntry {
    CpisiEventHook callback;  /**< Callback function. */
    void*          user_data; /**< User-provided context. */
    bool           active;    /**< Whether hook is active. */
} CpisiHookEntry;

/** @} */ /* end S.3b CALLBACKS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3c STRUCTS — CpisiOrchContext [STRUCTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_structs S.3c STRUCTS — CpisiOrchContext
 * @ingroup cpisi_types
 * @brief    Master context holding all subsystem contexts with cognitive state.
 *
 * WHY: The orchestrator needs unified view across all DAR phases. Single context
 *      tracks cognitive state (HebrewState, KFactor, health) and coordinates
 *      all three phase contexts (log, debug, restore).
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief CpisiOrchContext holds unified state across all DAR phases.
 *
 * This is the master context for CPI-SI operation:
 *   - Active subsystem flags and initialization state
 *   - Pointers to phase contexts (LogContext, DebugContext, RestoreContext)
 *   - Full cognitive state (HebrewState, KFactor, health scores)
 *   - Counters for errors, recoveries, degradations
 *   - Session timing
 *
 * @see cpisi_orch_init() for initialization
 * @see cpisi_orch_shutdown() for cleanup
 */
typedef struct CpisiOrchContext {
    /* Initialization state */
    CpisiFlags       flags;             /**< Which subsystems are enabled. */
    bool             initialized;       /**< Whether context is initialized. */

    /* Subsystem contexts (allocated internally) */
    LogContext*      log;               /**< DETECT phase context (logging). */
    DebugContext*    debug;             /**< ASSESS phase context (debugging). */
    RestoreContext*  restore;           /**< RESTORE phase context (recovery). */
    DAROrchestrator* dar;               /**< DAR orchestrator for coordinated phases. */

    /* Cognitive state — unified view across all phases */
    HebrewState      current_state;     /**< Cognitive position (shavar → yashar → tov). */
    KFactor          current_k;         /**< Direction of movement (-1, 0, +1). */
    HealthScore      health;            /**< 0-100 health score. */
    MultiScore       score;             /**< Multi-scale representation. */
    TernaryScore     ternary;           /**< Ternary representation. */
    HealthLevel      health_level;      /**< 7-level discrete health. */
    HealthThreshold  thresholds;        /**< Health zone thresholds. */
    int32_t          cube_pos[3];       /**< Position in 27-cube [x, y, z]. */

    /* Counters */
    uint32_t         error_count;       /**< Total errors recorded. */
    uint32_t         recovery_count;    /**< Successful recoveries. */
    uint32_t         degradation_count; /**< Graceful degradations. */
    uint32_t         fatal_count;       /**< Fatal issues encountered. */

    /* Severity tracking */
    uint32_t         severity_counts[CPISI_SEVERITY_COUNT]; /**< Counts per severity. */

    /* Cumulative health tracking for normalization
     * Used to calculate normalized -100 to +100 score with asymmetry weighting.
     * Scripture: "Wide is the gate that leads to destruction... narrow the way to life"
     * - More paths to failure than success, so success is weighted more heavily.
     */
    int64_t          cumulative_positive;   /**< Sum of positive impacts (recovery). */
    int64_t          cumulative_negative;   /**< Sum of negative impacts (errors). */
    uint32_t         positive_actions;      /**< Count of positive actions (k=+1). */
    uint32_t         negative_actions;      /**< Count of negative actions (k=-1). */
    HealthScore      raw_health;            /**< Unnormalized health (direct sum). */
    HealthScore      normalized_health;     /**< Normalized -100 to +100 score. */

    /* Session timing */
    uint64_t         session_start;     /**< Session start timestamp. */
    ClockContext*    clock;             /**< Internal clock context (owned, allocated). */

    /* Event hooks */
    CpisiHookEntry   hooks[CPISI_EVT_COUNT][CPISI_HOOKS_PER_EVENT]; /**< Hook registry. */
    uint8_t          hook_counts[CPISI_EVT_COUNT]; /**< Active hooks per event. */

    /* Persistence */
    char             state_path[CPISI_STATE_PATH_MAX]; /**< Path for state persistence (if set). */
    bool             auto_persist;      /**< Auto-save on shutdown. */

} CpisiOrchContext;

/** @} */ /* end S.3c STRUCTS */

/** S.3d ENUMS — [Reserved: uses CpisiFlags from io/config.h] @defgroup cpisi_enums @ingroup cpisi_types @{ @} */

/** S.3e ERRORS — [Reserved: uses CpisiConfigError from io/config.h] @defgroup cpisi_errors @ingroup cpisi_types @{ @} */

/** @} */ /* end S.3 TYPES */

/** S.4 PROTOTYPES — [Reserved: prototypes in BODY] @defgroup cpisi_protos @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 FILE-LEVEL — Global State [FILE_LEVEL]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_file S.5 FILE-LEVEL — Global State
 * @brief    Global context pointer for convenience macros.
 *
 * WHY: Many operations work on "the current context." Global pointer enables
 *      macros and NULL-defaulting functions without explicit context passing.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Global CPI-SI orchestrator context.
 *
 * Used by convenience macros (CPISI_ERROR, CPISI_LOG_ERROR, etc.)
 * and functions that accept NULL for "use global context."
 *
 * Set by cpisi_orch_init() when ctx parameter is NULL.
 */
extern CpisiOrchContext* g_cpisi;

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_s6_omission S.6 OMISSION — Excluded Content
 * @brief    Documents what SETUP sections are intentionally reserved.
 *
 * | Section | Status | Reason |
 * |---------|--------|--------|
 * | S.1c EXTERNAL | Reserved | No external libraries |
 * | S.2c-f | Reserved | No additional defines needed |
 * | S.3b ENUMS | Reserved | Uses CpisiFlags from io/config.h |
 * | S.3d ERRORS | Reserved | Uses CpisiConfigError from io/config.h |
 * | S.4 PROTOTYPES | Reserved | Prototypes in BODY |
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
/// @brief The actual work — orchestrator functions and macros.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART   — Structure overview
///   - B.2 HELPERS     — Subsystem access inlines
///   - B.3 CORE OPS    — Lifecycle functions
///   - B.4 ERRORS      — [Reserved]
///   - B.5 PUBLIC API  — State, health, status functions
///   - B.6 OMISSION    — Reserved sections
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_orgchart B.1 ORG CHART — Structure Overview
 * @brief    Map structure — types, functions, file organization.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Interface Overview [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_structure B.1a STRUCTURE — Interface Overview
 * @ingroup cpisi_orgchart
 * @brief    Types, functions, and externs declared in this header.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      5 total (3 structs, 1 enum, 1 callback typedef) */
/*   - CpisiOrchContext, CpisiEventData, CpisiHookEntry (structs) */
/*   - CpisiEventType (enum) */
/*   - CpisiEventHook (callback) */
/* Functions:  29 total (4 lifecycle, 9 state, 4 health, 2 status, 5 hooks, 6 persistence) */
/* Inlines:    3 (subsystem access) */
/* Defines:    14 (12 event types + 2 config) */
/* Macros:     10 (CPISI_HAS_*, CPISI_ERROR, CPISI_LOG_*, CPISI_RESTORE_*) */
/* Externs:    1 (g_cpisi) */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_flow B.1b FLOW — Implementation Location
 * @ingroup cpisi_orgchart
 * @brief    Where implementations live.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: cpisi/cpisi.c */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_counts B.1c COUNTS — Summary Statistics
 * @ingroup cpisi_orgchart
 * @brief    Interface element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      5 total */
/*   - Structs:   CpisiOrchContext, CpisiEventData, CpisiHookEntry */
/*   - Enums:     CpisiEventType */
/*   - Typedefs:  CpisiEventHook (callback) */
/* Defines:   14 total */
/*   - Events:    12 (CPISI_EVT_* for state, health, lifecycle) */
/*   - Config:     2 (CPISI_HOOKS_PER_EVENT, CPISI_STATE_PATH_MAX) */
/* Functions: 29 total */
/*   - Lifecycle:    4 (init, shutdown, reset, init_from_file) */
/*   - State:        9 (set_*, get_*, update_cube_pos) */
/*   - Health:       4 (record_error, record_recovery, record_degradation, sync_state) */
/*   - Status:       2 (status_report, status_brief) */
/*   - Hooks:        5 (register, unregister, unregister_callback, fire, count) */
/*   - Persistence:  6 (save, load, set_path, get_path, set_auto, get_auto) */
/* Inlines:    3 total (get_log_ctx, get_debug_ctx, get_restore_ctx) */
/* Macros:    10 total (CPISI_HAS_*, CPISI_ERROR, CPISI_LOG_*, CPISI_RESTORE_*) */
/* Externs:    1 total (g_cpisi) */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Subsystem Access [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_helpers B.2 HELPERS — Subsystem Access
 * @brief    Inline functions for accessing subsystem contexts.
 *
 * WHY: Direct access to phase contexts needed for phase-specific operations.
 *      Inlines avoid function call overhead for frequent access.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Get log context from orchestrator or global.
 * @param[in] ctx Orchestrator context (or NULL for global).
 * @return LogContext pointer (never NULL if system initialized).
 */
static inline LogContext* cpisi_get_log_ctx(CpisiOrchContext* ctx) {
    return ctx ? ctx->log : (g_cpisi ? g_cpisi->log : g_cpisi_log);
}

/**
 * @brief Get debug context from orchestrator or global.
 * @param[in] ctx Orchestrator context (or NULL for global).
 * @return DebugContext pointer (never NULL if system initialized).
 */
static inline DebugContext* cpisi_get_debug_ctx(CpisiOrchContext* ctx) {
    return ctx ? ctx->debug : (g_cpisi ? g_cpisi->debug : g_cpisi_debug);
}

/**
 * @brief Get restore context from orchestrator or global.
 * @param[in] ctx Orchestrator context (or NULL for global).
 * @return RestoreContext pointer (never NULL if system initialized).
 */
static inline RestoreContext* cpisi_get_restore_ctx(CpisiOrchContext* ctx) {
    return ctx ? ctx->restore : (g_cpisi ? g_cpisi->restore : g_cpisi_restore);
}

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Lifecycle Functions [CORE_OPS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_core_ops B.3 CORE OPS — Lifecycle Functions
 * @brief    Initialize, shutdown, and reset the unified orchestrator.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Initialize the unified CPI-SI system.
 *
 * Initializes all enabled subsystems based on config.flags:
 *   - CPISI_FLAG_LOG: Initialize logging (DETECT phase)
 *   - CPISI_FLAG_DEBUG: Initialize debugging (ASSESS phase)
 *   - CPISI_FLAG_RESTORE: Initialize restore (RESTORE phase)
 *
 * @param[in,out] ctx    Context to initialize (or NULL for internal allocation).
 * @param[in]     config Configuration (or NULL for defaults).
 *
 * @return 0 on success, -1 on failure.
 */
int cpisi_orch_init(CpisiOrchContext* ctx, const CpisiConfig* config);

/**
 * @brief Cleanly shut down all subsystems.
 *
 * @param[in,out] ctx Context to shutdown (or NULL for global).
 */
void cpisi_orch_shutdown(CpisiOrchContext* ctx);

/**
 * @brief Re-initialize with new configuration.
 *
 * Shuts down existing subsystems and reinitializes with new config.
 *
 * @param[in,out] ctx    Context to reset (or NULL for global).
 * @param[in]     config New configuration.
 *
 * @return 0 on success, -1 on failure.
 */
int cpisi_orch_reset(CpisiOrchContext* ctx, const CpisiConfig* config);

/**
 * @brief Initialize from TOML config file.
 *
 * Recommended entry point. Loads configuration from file then initializes.
 *
 * @param[in,out] ctx         Context to initialize (or NULL for internal allocation).
 * @param[in]     config_path Path to TOML config (or NULL to search standard paths).
 *
 * @return 0 on success (file loaded), 1 on success (defaults used), -1 on error.
 */
int cpisi_orch_init_from_file(CpisiOrchContext* ctx, const char* config_path);

/** @} */ /* end B.3 CORE OPS */

/** B.4 ERRORS — [Reserved: uses CpisiConfigError] @defgroup cpisi_b4_errors @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — State, Health, and Status [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_api B.5 PUBLIC API — State, Health, and Status
 * @brief    Cognitive state management, health tracking, status reporting.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a STATE — Cognitive State Management [STATE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_state_mgmt B.5a STATE — Cognitive State Management
 * @ingroup cpisi_api
 * @brief    Update and query orchestrator-level cognitive state.
 *
 * WHY: Unified cognitive state across all DAR phases. These functions
 *      synchronize state changes to all phase contexts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Setters */

/** @brief Set Hebrew cognitive state. */
void cpisi_orch_set_hebrew_state(CpisiOrchContext* ctx, HebrewState state);

/** @brief Set k-factor (direction of movement). */
void cpisi_orch_set_k_factor(CpisiOrchContext* ctx, KFactor k);

/** @brief Set health score. */
void cpisi_orch_set_health(CpisiOrchContext* ctx, HealthScore health);

/** @brief Update 27-cube position. */
void cpisi_orch_update_cube_pos(CpisiOrchContext* ctx, int32_t x, int32_t y, int32_t z);

/* Getters */

/** @brief Get current Hebrew state. */
HebrewState cpisi_orch_get_hebrew_state(const CpisiOrchContext* ctx);

/** @brief Get current k-factor. */
KFactor cpisi_orch_get_k_factor(const CpisiOrchContext* ctx);

/** @brief Get current health score. */
HealthScore cpisi_orch_get_health(const CpisiOrchContext* ctx);

/** @brief Get current health level (7-level discrete). */
HealthLevel cpisi_orch_get_health_level(const CpisiOrchContext* ctx);

/** @brief Get Hebrew state name string. */
const char* cpisi_orch_get_health_state_name(const CpisiOrchContext* ctx);

/** @} */ /* end B.5a STATE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b HEALTH — Health Tracking [HEALTH]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_health_track B.5b HEALTH — Health Tracking
 * @ingroup cpisi_api
 * @brief    Record events that affect health.
 *
 * WHY: Events (errors, recoveries, degradations) affect cognitive state.
 *      These functions update health and propagate to all phase contexts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Record an error and adjust health.
 *
 * @param[in,out] ctx      Context (or NULL for global).
 * @param[in]     severity Severity level (SEV_FATAL to SEV_PERFECT, -6 to +6).
 */
void cpisi_orch_record_error(CpisiOrchContext* ctx, Severity severity);

/**
 * @brief Record successful recovery.
 *
 * @param[in,out] ctx    Context (or NULL for global).
 * @param[in]     result Recovery result from types.h.
 */
void cpisi_orch_record_recovery(CpisiOrchContext* ctx, RestoreResult result);

/**
 * @brief Record graceful degradation.
 *
 * @param[in,out] ctx      Context (or NULL for global).
 * @param[in]     severity How severe the degradation is.
 */
void cpisi_orch_record_degradation(CpisiOrchContext* ctx, Severity severity);

/**
 * @brief Synchronize state across all DAR phase contexts.
 *
 * @param[in,out] ctx Context (or NULL for global).
 */
void cpisi_orch_sync_state(CpisiOrchContext* ctx);

/**
 * @brief Record a positive action (success, improvement, recovery).
 *
 * Uses proportional weighting: severity × SEVERITY_STEP (25).
 * Positive severities: SEV_FAIR(+25) to SEV_PERFECT(+100).
 * Success actions weighted MORE due to asymmetry (narrow is the way to life).
 *
 * @param[in,out] ctx      Context (or NULL for global).
 * @param[in]     severity Positive severity level (SEV_FAIR to SEV_PERFECT).
 */
void cpisi_orch_record_success(CpisiOrchContext* ctx, Severity severity);

/**
 * @brief Calculate normalized health score from cumulative actions.
 *
 * Normalization accounts for asymmetry between failure and success paths:
 * - More paths to corruption (128 steps) than restoration (127 steps)
 * - Success actions weighted proportionally higher
 * - Score normalized to -100 (shavar) to +100 (tov)
 *
 * Formula: normalized = (weighted_positive - weighted_negative) / total × 100
 *
 * @param[in] ctx Context (or NULL for global).
 *
 * @return Normalized health score: -100 (broken) to +100 (perfect).
 */
HealthScore cpisi_orch_calculate_normalized_health(const CpisiOrchContext* ctx);

/**
 * @brief Get the raw (unnormalized) cumulative health score.
 *
 * @param[in] ctx Context (or NULL for global).
 *
 * @return Raw health score (sum of all impacts, may exceed -100 to +100).
 */
HealthScore cpisi_orch_get_raw_health(const CpisiOrchContext* ctx);

/**
 * @brief Get the asymmetry factor used for normalization.
 *
 * Based on Scripture: "Wide is the gate... narrow the way" (Matthew 7:13-14).
 * More paths to failure than success.
 *
 * @return Asymmetry factor (typically ~1.008 = 128/127).
 */
float cpisi_orch_get_asymmetry_factor(void);

/**
 * @brief Reset health scoring to initial state.
 *
 * Clears all cumulative tracking while preserving other context state.
 * Health returns to yashar (0, balanced).
 *
 * @param[in,out] ctx Context (or NULL for global).
 */
void cpisi_orch_reset_health(CpisiOrchContext* ctx);

/** @} */ /* end B.5b HEALTH */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c STATUS — Status Reporting [STATUS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_status B.5c STATUS — Status Reporting
 * @ingroup cpisi_api
 * @brief    Comprehensive and brief status output.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Print comprehensive status report.
 *
 * @param[in] ctx Context (or NULL for global).
 */
void cpisi_orch_status_report(const CpisiOrchContext* ctx);

/**
 * @brief Generate one-line status string.
 *
 * @param[in]  ctx    Context (or NULL for global).
 * @param[out] buf    Buffer for output.
 * @param[in]  buflen Buffer size.
 *
 * @return Number of characters written.
 */
int cpisi_orch_status_brief(const CpisiOrchContext* ctx, char* buf, size_t buflen);

/** @} */ /* end B.5c STATUS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d MACROS — Convenience Macros [MACROS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_macros B.5d MACROS — Convenience Macros
 * @ingroup cpisi_api
 * @brief    Macros for common operations using global context.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Subsystem checks */
#define CPISI_HAS_LOG     (g_cpisi && (g_cpisi->flags & CPISI_FLAG_LOG))     /**< Log enabled? */
#define CPISI_HAS_DEBUG   (g_cpisi && (g_cpisi->flags & CPISI_FLAG_DEBUG))   /**< Debug enabled? */
#define CPISI_HAS_RESTORE (g_cpisi && (g_cpisi->flags & CPISI_FLAG_RESTORE)) /**< Restore enabled? */

/* Health recording */
#define CPISI_ERROR(sev)       cpisi_orch_record_error(NULL, (sev))        /**< Record error. */
#define CPISI_RECOVERED(res)   cpisi_orch_record_recovery(NULL, (res))     /**< Record recovery. */
#define CPISI_DEGRADED(sev)    cpisi_orch_record_degradation(NULL, (sev))  /**< Record degradation. */

/* Combined log + health tracking */
#define CPISI_LOG_ERROR(tag, fmt, ...) do { \
    LOG_ERROR(tag, fmt, ##__VA_ARGS__); \
    CPISI_ERROR(SEV_ERROR); \
} while(0)

#define CPISI_LOG_WARN(tag, fmt, ...) do { \
    LOG_WARN(tag, fmt, ##__VA_ARGS__); \
    CPISI_ERROR(SEV_WARN); \
} while(0)

/* Restore with automatic health tracking */
#define CPISI_RESTORE_INT(name, val, spec) do { \
    RestoreResult _r = cpisi_restore_int(g_cpisi_restore, name, val, spec); \
    if (_r == RESTORE_FILLED) CPISI_RECOVERED(_r); \
    else if (_r == RESTORE_DEGRADED) CPISI_DEGRADED(SEV_WARN); \
    else if (_r < 0) CPISI_ERROR(SEV_ERROR); \
} while(0)

#define CPISI_RESTORE_FILE(spec, found, found_size) do { \
    RestoreResult _r = cpisi_restore_file(g_cpisi_restore, spec, found, found_size); \
    if (_r == RESTORE_LOCATED) CPISI_RECOVERED(_r); \
    else if (_r == RESTORE_DEGRADED) CPISI_DEGRADED(SEV_WARN); \
    else if (_r < 0) CPISI_ERROR(SEV_ERROR); \
} while(0)

/** @} */ /* end B.5d MACROS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5e HOOKS — Event Hook Management [HOOKS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_hooks B.5e HOOKS — Event Hook Management
 * @ingroup cpisi_api
 * @brief    Register, unregister, and fire event hooks.
 *
 * WHY: Observable system. Callers subscribe to state transitions without
 *      modifying core logic. Enables metrics, logging, custom reactions.
 *
 * Usage:
 * @code{.c}
 *   void my_health_hook(const CpisiOrchContext* ctx,
 *                       const CpisiEventData* ev, void* user) {
 *       printf("Health: %d -> %d\n", ev->old_value, ev->new_value);
 *   }
 *   cpisi_orch_register_hook(NULL, CPISI_EVENT_HEALTH_CHANGED, my_health_hook, NULL);
 * @endcode
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Register a hook for an event type.
 *
 * @param[in,out] ctx       Context (or NULL for global).
 * @param[in]     event     Event type to subscribe to.
 * @param[in]     callback  Function to call when event fires.
 * @param[in]     user_data User context passed to callback.
 *
 * @return Hook ID (>= 0) on success, -1 if hook table full.
 */
int cpisi_orch_register_hook(CpisiOrchContext* ctx,
                             CpisiEventType    event,
                             CpisiEventHook    callback,
                             void*             user_data);

/**
 * @brief Unregister a hook by ID.
 *
 * @param[in,out] ctx     Context (or NULL for global).
 * @param[in]     event   Event type the hook was registered for.
 * @param[in]     hook_id Hook ID returned from register.
 *
 * @return 0 on success, -1 if hook not found.
 */
int cpisi_orch_unregister_hook(CpisiOrchContext* ctx,
                               CpisiEventType    event,
                               int               hook_id);

/**
 * @brief Unregister all hooks for a callback function.
 *
 * @param[in,out] ctx      Context (or NULL for global).
 * @param[in]     callback Callback to remove from all events.
 *
 * @return Number of hooks removed.
 */
int cpisi_orch_unregister_callback(CpisiOrchContext* ctx,
                                   CpisiEventHook    callback);

/**
 * @brief Fire hooks for an event (internal use, but exposed for extensibility).
 *
 * @param[in] ctx       Context (or NULL for global).
 * @param[in] event     Event type that occurred.
 * @param[in] old_value Previous value.
 * @param[in] new_value New value.
 * @param[in] message   Optional message (may be NULL).
 */
void cpisi_orch_fire_hooks(const CpisiOrchContext* ctx,
                           CpisiEventType          event,
                           int32_t                 old_value,
                           int32_t                 new_value,
                           const char*             message);

/**
 * @brief Get number of active hooks for an event type.
 *
 * @param[in] ctx   Context (or NULL for global).
 * @param[in] event Event type to query.
 *
 * @return Number of registered hooks.
 */
int cpisi_orch_hook_count(const CpisiOrchContext* ctx, CpisiEventType event);

/** @} */ /* end B.5e HOOKS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5f PERSISTENCE — State Save/Load [PERSISTENCE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup cpisi_persist B.5f PERSISTENCE — State Save/Load
 * @ingroup cpisi_api
 * @brief    Save and restore cognitive state across sessions.
 *
 * WHY: CPI-SI is stateful by design. Cognitive state (HebrewState, KFactor,
 *      health, cube position) persists across sessions. JSON format via io/json.h.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Save orchestrator state to file.
 *
 * Serializes cognitive state (HebrewState, KFactor, health, cube position,
 * counters, thresholds) to JSON file for session persistence.
 *
 * @param[in] ctx  Context to save (or NULL for global).
 * @param[in] path File path for state (or NULL to use ctx->state_path).
 *
 * @return 0 on success, -1 on failure.
 */
int cpisi_orch_save_state(const CpisiOrchContext* ctx, const char* path);

/**
 * @brief Load orchestrator state from file.
 *
 * Deserializes cognitive state from JSON file. Fires CPISI_EVENT_STATE_LOADED
 * hook on success.
 *
 * @param[in,out] ctx  Context to load into (or NULL for global).
 * @param[in]     path File path (or NULL to use ctx->state_path).
 *
 * @return 0 on success, 1 if file not found (state unchanged), -1 on parse error.
 */
int cpisi_orch_load_state(CpisiOrchContext* ctx, const char* path);

/**
 * @brief Set default state persistence path.
 *
 * @param[in,out] ctx  Context (or NULL for global).
 * @param[in]     path File path for state persistence.
 */
void cpisi_orch_set_state_path(CpisiOrchContext* ctx, const char* path);

/**
 * @brief Get current state persistence path.
 *
 * @param[in] ctx Context (or NULL for global).
 *
 * @return State path (may be empty string if not set).
 */
const char* cpisi_orch_get_state_path(const CpisiOrchContext* ctx);

/**
 * @brief Enable/disable auto-save on shutdown.
 *
 * When enabled, cpisi_orch_shutdown() automatically calls cpisi_orch_save_state().
 *
 * @param[in,out] ctx    Context (or NULL for global).
 * @param[in]     enable True to enable, false to disable.
 */
void cpisi_orch_set_auto_persist(CpisiOrchContext* ctx, bool enable);

/**
 * @brief Check if auto-persist is enabled.
 *
 * @param[in] ctx Context (or NULL for global).
 *
 * @return True if auto-persist enabled.
 */
bool cpisi_orch_get_auto_persist(const CpisiOrchContext* ctx);

/** @} */ /* end B.5f PERSISTENCE */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_b6_omission B.6 OMISSION — Reserved BODY Sections
 * @brief    Documents what BODY sections are intentionally reserved.
 *
 * | Section | Status | Reason |
 * |---------|--------|--------|
 * | B.4 ERRORS | Reserved | Uses CpisiConfigError from io/config.h |
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
 * @defgroup cpisi_x1_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build and syntax verification.
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * gcc -fsyntax-only -Wall -Wextra cpisi/cpisi.h    # syntax check
 * cppcheck --enable=all cpisi/cpisi.h              # static analysis
 * @endcode
 *
 * @par X.1b SELFTEST — Minimal Isolation Test
 * @code{.c}
 * #include "kernel/cpisi.mgr.h"
 * int main(void) { cpisi_orch_init(NULL, NULL); cpisi_orch_shutdown(NULL); return 0; }
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
 * @defgroup cpisi_x2_execution X.2 EXECUTION — Usage Patterns
 * @brief    Include pattern and usage examples.
 *
 * @par X.2a INCLUDE — Include Pattern
 * @code{.c}
 * #include "kernel/cpisi.mgr.h"  // Single include for all CPI-SI
 * @endcode
 *
 * @par X.2b USAGE — Basic Usage
 * @code{.c}
 * // Initialize from config file
 * int result = cpisi_orch_init_from_file(NULL, NULL);
 * if (result < 0) { fprintf(stderr, "Init failed\n"); return 1; }
 *
 * // Use CPI-SI...
 * CPISI_LOG_ERROR("app", "Something went wrong");
 *
 * // Clean shutdown
 * cpisi_orch_shutdown(NULL);
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
 * @defgroup cpisi_x3_cleanup X.3 CLEANUP — Ownership Patterns
 * @brief    Ownership notes — context lifecycle.
 *
 * OWNERSHIP:
 *   - CpisiOrchContext: Caller-owned if provided, internally allocated if NULL
 *   - Phase contexts (log, debug, restore): Internally allocated, freed on shutdown
 *   - g_cpisi: Set by init when ctx is NULL, cleared by shutdown
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
 * @defgroup cpisi_x4_policy X.4 POLICY — Modification Guidance
 * @brief    Guide maintainers — modification safety levels.
 *
 * SAFE TO MODIFY:
 *   - Add new state query functions
 *   - Add new convenience macros
 *   - Add new status output formats
 *
 * MODIFY WITH CARE:
 *   - CpisiOrchContext fields — affects all users
 *   - Macro implementations — widely used
 *
 * NEVER MODIFY:
 *   - Include order (dar.h before io/config.h)
 *   - 4-block structure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Policy documented in defgroup above] */

/** @} */ /* end X.4 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Add Features [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_x5_extension X.5 EXTENSION — How to Add Features
 * @brief    How to extend the orchestrator.
 *
 * ADDING NEW STATE FIELD:
 *   1. Add field to CpisiOrchContext in S.3a
 *   2. Add setter/getter functions in B.5a
 *   3. Update cpisi_orch_sync_state() if needed
 *   4. Update B.1c COUNTS
 *
 * ADDING NEW CONVENIENCE MACRO:
 *   1. Add to B.5d MACROS section
 *   2. Document parameters and behavior
 *   3. Update B.1c COUNTS
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
 * @defgroup cpisi_x6_troubleshoot X.6 TROUBLESHOOT — Common Problems
 * @brief    Common problems and solutions.
 *
 * "g_cpisi is NULL":
 *   → Call cpisi_orch_init() first
 *   → Pass NULL for ctx parameter to set global
 *
 * "Subsystem not initialized":
 *   → Check config.flags includes the subsystem flag
 *   → Use CPISI_HAS_* macros before subsystem operations
 *
 * "Config file not found":
 *   → Check search paths (see io/config.h)
 *   → Use cpisi_orch_init_from_file() which falls back to defaults
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
 * @defgroup cpisi_x7_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick lookup — key patterns.
 *
 * INITIALIZATION:
 *   cpisi_orch_init_from_file(NULL, NULL)  // Recommended
 *   cpisi_orch_init(NULL, &config)         // Custom config
 *
 * ERROR TRACKING:
 *   CPISI_LOG_ERROR(tag, fmt, ...)         // Log + track
 *   CPISI_ERROR(SEV_ERROR)                 // Track only
 *
 * STATE QUERY:
 *   cpisi_orch_get_health(NULL)            // Health score
 *   cpisi_orch_get_hebrew_state(NULL)      // Cognitive state
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
 * @defgroup cpisi_x8_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * Scripture: Ecclesiastes 4:12
 *
 * "A threefold cord is not quickly broken."
 *
 * The three DAR phases (DETECT, ASSESS, RESTORE) strengthen each other
 * through coordination. This unified orchestrator provides:
 *   - Single include point for all CPI-SI
 *   - Full cognitive state tracking (HebrewState, KFactor, health)
 *   - Coordinated health across all phases
 *   - Resilient, observable, recoverable execution
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Note documented in defgroup above] */

/** @} */ /* end X.8 NOTE */

/** X.9 TEMPLATE — [Reserved: instance file] @defgroup cpisi_x9_template @{ @} */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 OMISSION — Reserved Sections [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup cpisi_x10_omission X.10 OMISSION — Reserved Sections
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

#endif /* CPISI_H */
