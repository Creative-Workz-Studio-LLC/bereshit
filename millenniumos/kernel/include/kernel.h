// =============================================================================
// OMNICODE PRAGMA [PRAGMA]
// =============================================================================
//
// @legend
//   :req  REQUIRED   — must exist, validation fails without
//   :inh  INHERITED  — from template, override allowed
//   :ins  INSTANCE   — file-specific, unique values
// @endlegend
//
// -----------------------------------------------------------------------------
// P.1 CORE — Identity [CORE]
// -----------------------------------------------------------------------------
//
// @omni:req  key        = MOS-kernel-kernel-h
// @omni:req  from       = bereshit/word/seed/code/c/header.h
// @omni:req  at         = document
//
// -----------------------------------------------------------------------------
// P.2 FAMILY — Classification [FAMILY]
// -----------------------------------------------------------------------------
//
// @omni:inh  type       = header
// @omni:inh  subtype    = public-api
// @omni:inh  role       = interface
// @omni:inh  structure  = 4-block
//
// -----------------------------------------------------------------------------
// P.3 INSTANCE — File Details [INSTANCE]
// -----------------------------------------------------------------------------
//
// @omni:ins  component  = kernel
// @omni:ins  layer      = 0 (kernel layer)
// @omni:ins  cpisi_mode = 0 (freestanding)
// @omni:ins  provides   = [MosKernelInfo, mos_kernel_main, boot path API]
//
// -----------------------------------------------------------------------------
// P.4 ARCHITECTURE — Scaling Context [ARCH]
// -----------------------------------------------------------------------------
//
// @omni:ins  layers     = [0:kernel, 1:system, 2:runtime, 3:framework,
//                          4:health, 5:network, 6:storage, 7:engine, 8:app]
// @omni:ins  scale      = 3^n [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
// @omni:ins  pattern    = 4+1+4 [below:0-3, center:4, above:5-8]
// @omni:ins  anchor     = genesis_1_1 -> void(0) -> unity(1) -> derivation
//
// -----------------------------------------------------------------------------
// P.5 SUMMARY — Human Identity [SUMMARY]
// -----------------------------------------------------------------------------
//
// @omni:req  title      = MillenniumOS Kernel Core Header
// @omni:req  brief      = Public API for kernel services, boot path, and state
//
// =============================================================================
// END PRAGMA
// =============================================================================

// =============================================================================
// METADATA BLOCK [METADATA]
// =============================================================================
//
// @brief Identity and context for the kernel core header.
//
// STRUCTURE: M.1-M.10 sections for complete documentation.
//
// =============================================================================

/**
 * =============================================================================
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * =============================================================================
 *
 * @file      kernel.h
 * @brief     MillenniumOS Kernel — Public API header.
 *
 * Key:       MOS-kernel-kernel-h
 * Title:     MillenniumOS Kernel Core Header
 * Type:      Header (Public API)
 * Component: Kernel — Layer 0 (freestanding)
 * Role:      Public interface for kernel services, state, and boot path
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.2 STATE — Lifecycle State [STATE]
 * =============================================================================
 *
 * @version   a-01.50
 * @date      2026-01-31
 *
 * Status:    Active
 * Created:   2026-01-30
 * Updated:   2026-01-31
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
 * =============================================================================
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Implementation)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.4 LOCATION — File Position [LOCATION]
 * =============================================================================
 *
 * Path:      millenniumos/kernel/include/kernel.h
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.5 DERIVATION — Template Lineage [DERIVATION]
 * =============================================================================
 *
 * Derives:   bereshit/word/seed/code/c/header.h (4-block C header template)
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
 * =============================================================================
 *
 * Tags:      kernel, header, api, freestanding, boot-path
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.7 INTENT — Purpose Statement [INTENT]
 * =============================================================================
 *
 * Purpose:   Provide the public API for:
 *            - Kernel state and info access
 *            - Boot path detection (UEFI vs BIOS)
 *            - Boot feature flags
 *            - Subsystem initialization
 *            - Panic/halt handlers
 *            - CPI-SI stubs for freestanding mode
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * =============================================================================
 *
 * Scripture: Genesis 1:1 — "In the beginning God created the heaven and the earth."
 * Principle: The kernel is the creative force bringing order from void.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
 * =============================================================================
 *
 * Depends:
 *   - trit/matter/matter.h — libtrit trit types (trit_t, trit5_t, etc.)
 *   - boot/include/bootinfo.h — MosBootInfo structure
 *
 * Used by:
 *   - kernel/src/kernel.c       — Main kernel implementation
 *   - kernel/src/core/          — All kernel subsystems
 *   - kernel/src/services/      — Kernel services
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.10 ROADMAP — Version History [ROADMAP]
 * =============================================================================
 *
 * History:
 *   a-01.00 (2026-01-30) — Initial kernel header with types and stubs
 *   a-01.50 (2026-01-31) — Full OmniCode 4-block, boot path API
 *
 * Planned:
 *   - Full DAR Witness API
 *   - Runtime services (UEFI-specific)
 *   - Shell service integration
 *
 * =============================================================================
 */

// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_KERNEL_H
#define MILLENNIUMOS_KERNEL_H

// =============================================================================
// SETUP BLOCK [SETUP]
// =============================================================================
//
// @brief Configuration and declarations before body.
//
// 4-Block Header Structure: METADATA → SETUP → BODY → CLOSING
//
// CONTAINS:
//   - S.1 MODE     — CPISI mode selection (freestanding)
//   - S.2 INCLUDES — Foundation type includes
//   - S.3 DEFINES  — Constants and feature flags
//   - S.4 TYPES    — Kernel type definitions
//   - S.5 BOOT     — Boot handoff includes
//   - S.6 OMISSION — Reserved section guide
//
// =============================================================================

/**
 * =============================================================================
 * S.1 MODE — CPISI Mode Selection [MODE]
 * =============================================================================
 *
 * @brief Set CPISI_MODE=0 (KERNEL) for freestanding environment.
 *
 * MUST be set BEFORE including any headers that check CPISI_MODE.
 * This restricts includes to stdint.h, stdbool.h, stddef.h only.
 *
 * =============================================================================
 */

#define CPISI_MODE 0  /**< @brief CPISI_MODE_KERNEL - freestanding environment */

/**
 * =============================================================================
 * S.2 INCLUDES — Foundation Types [INCLUDES]
 * =============================================================================
 *
 * @brief libtrit MATTER layer only for trit types.
 *
 * NOTE: Do NOT include full trit/trit.h - it has HebrewState conflict.
 * NOTE: cornerstone types.h disabled until CPISI_MODE guards added.
 *
 * =============================================================================
 */

#include "trit/matter/matter.h"   /**< @brief libtrit: trit_t, trit5_t, trit9_t, trit27_t */

/**
 * =============================================================================
 * S.3 DEFINES — Constants and Feature Flags [DEFINES]
 * =============================================================================
 *
 * @brief Boot feature flags and kernel constants.
 *
 * =============================================================================
 */

/**
 * @defgroup kernel_features Boot Feature Flags
 * @brief Feature flags detected during kernel_early_init().
 *
 * Use mos_has_boot_feature(flag) to check availability.
 * @{
 */
#define MOS_FEAT_ACPI        0x01  /**< @brief ACPI RSDP provided by firmware */
#define MOS_FEAT_HIMEM_FB    0x02  /**< @brief High memory framebuffer (>4GB, UEFI typical) */
#define MOS_FEAT_FRAMEBUFFER 0x04  /**< @brief Valid framebuffer available */
#define MOS_FEAT_MEMMAP      0x08  /**< @brief Memory map available */
#define MOS_FEAT_RUNTIME     0x10  /**< @brief UEFI runtime services (future) */
/** @} */

/**
 * =============================================================================
 * S.4 TYPES — Kernel Type Definitions [TYPES]
 * =============================================================================
 *
 * @brief Kernel types defined directly for freestanding mode.
 *
 * These will be replaced once cornerstone types.h supports CPISI_MODE guards.
 *
 * =============================================================================
 */

/**
 * @defgroup kernel_types S.4 Kernel Type Definitions
 * @brief Types for kernel state and operations.
 * @{
 */

/**
 * @enum MosHebrewState
 * @brief Hebrew cognitive states (shavar → yashar → tov).
 *
 * Maps to cornerstone HebrewState values.
 * Used for kernel and process cognitive state tracking.
 */
typedef enum MosHebrewState {
    MOS_HEBREW_SHAVAR = 0,   /**< @brief Broken (crisis) */
    MOS_HEBREW_CHASER = 1,   /**< @brief Lacking (regressing) */
    MOS_HEBREW_RATSAH = 2,   /**< @brief Wanting (hungry for growth) */
    MOS_HEBREW_YASHAR = 3,   /**< @brief Even (balanced) [CENTER] */
    MOS_HEBREW_TAMIM  = 4,   /**< @brief Sound (consolidating) */
    MOS_HEBREW_SHALEM = 5,   /**< @brief Whole (expanding) */
    MOS_HEBREW_TOV    = 6,   /**< @brief Perfect (excellent) */
} MosHebrewState;

/**
 * @enum MosHealthZone
 * @brief Health zones for cognitive/diagnostic states.
 *
 * Not privilege zones — used for health monitoring display.
 */
typedef enum MosHealthZone {
    MOS_HEALTH_ZONE_CRISIS    = 0,  /**< @brief Red (system failure) */
    MOS_HEALTH_ZONE_WARNING   = 1,  /**< @brief Yellow (needs attention) */
    MOS_HEALTH_ZONE_BALANCED  = 2,  /**< @brief Green (normal) [CENTER] */
    MOS_HEALTH_ZONE_HEALTHY   = 3,  /**< @brief Blue (thriving) */
    MOS_HEALTH_ZONE_EXCELLENT = 4,  /**< @brief Gold (peak performance) */
} MosHealthZone;

/**
 * @enum MosZone
 * @brief Privilege zones for access control.
 *
 * Genesis model: void → word → tov (entry → work → completion)
 */
typedef enum MosZone {
    MOS_ZONE_VOID = 0,   /**< @brief Lowest privilege (uninitialized/entry) */
    MOS_ZONE_WORD = 1,   /**< @brief Normal privilege (standard operations) */
    MOS_ZONE_TOV  = 2,   /**< @brief Highest privilege (kernel operations) */
} MosZone;

/**
 * @name Kernel Type Aliases
 * @brief Type aliases for kernel-specific semantics.
 * @{
 */
typedef int16_t        mos_health_t;      /**< @brief Health score (-100 to +100) */
typedef uint64_t       mos_time_t;        /**< @brief Timestamp (TSC or ticks) */
typedef trit_t         mos_result_t;      /**< @brief Ternary result (-1=error, 0=ok, +1=info) */
typedef trit27_t       mos_vaddr_t;       /**< @brief Virtual address (ternary word) */
typedef trit27_t       mos_paddr_t;       /**< @brief Physical address (ternary word) */
typedef trit27_t       mos_size_t;        /**< @brief Size (ternary word) */
typedef trit9_t        mos_pid_t;         /**< @brief Process ID (9 trits = 19,683 max) */
typedef trit9_t        mos_uid_t;         /**< @brief User ID (9 trits) */
typedef trit9_t        mos_gid_t;         /**< @brief Group ID (9 trits) */
typedef uint32_t       mos_flags_t;       /**< @brief Generic flags type */
/** @} */

/**
 * @name Additional Type Aliases
 * @brief Extended type aliases for filesystem and flags.
 * @{
 */
typedef trit27_t       mos_ino_t;         /**< @brief Inode number (ternary word) */
typedef trit27_t       mos_off_t;         /**< @brief File offset (ternary word) */
typedef bool           mos_bool_t;        /**< @brief Boolean (from stdbool.h) */
/** @} */

/**
 * @name Kernel Constants
 * @brief System-wide constants.
 * @{
 */
#define MOS_MAX_NAME    256               /**< @brief Maximum filename length */
#define MOS_MAX_PATH    1024              /**< @brief Maximum path length */
#define MOS_PID_KERNEL  ((mos_pid_t)0)    /**< @brief Kernel process ID */
#define MOS_PID_INIT    ((mos_pid_t)1)    /**< @brief Init process ID */
#define MOS_FLAG_NONE   ((mos_flags_t)0)  /**< @brief No flags set */
/** @} */

/**
 * @name Hebrew State Aliases
 * @brief Backward compatibility aliases for Hebrew states.
 * @{
 */
#define MOS_STATE_SHAVAR   MOS_HEBREW_SHAVAR  /**< @brief Alias for SHAVAR */
#define MOS_STATE_CHASER   MOS_HEBREW_CHASER  /**< @brief Alias for CHASER */
#define MOS_STATE_RATSAH   MOS_HEBREW_RATSAH  /**< @brief Alias for RATSAH */
#define MOS_STATE_YASHAR   MOS_HEBREW_YASHAR  /**< @brief Alias for YASHAR */
#define MOS_STATE_TAMIM    MOS_HEBREW_TAMIM   /**< @brief Alias for TAMIM */
#define MOS_STATE_SHALEM   MOS_HEBREW_SHALEM  /**< @brief Alias for SHALEM */
#define MOS_STATE_TOV      MOS_HEBREW_TOV     /**< @brief Alias for TOV */
#define MOS_CPISI_YASHAR   MOS_HEBREW_YASHAR  /**< @brief CPISI naming alias */
/** @} */

/**
 * @name Cornerstone Compatibility Types
 * @brief Types for witness/DAR system compatibility.
 * @{
 */
typedef MosHebrewState HebrewState;   /**< @brief Alias for cornerstone compatibility */
typedef int8_t KFactor;               /**< @brief Direction of choice */
typedef int16_t HealthScore;          /**< @brief Health score type */

/**
 * @enum ActionType
 * @brief Action result types for witness system.
 */
typedef enum ActionType {
    ACTION_SUCCESS  = 0,   /**< @brief Action succeeded */
    ACTION_FAILURE  = 1,   /**< @brief Action failed */
    ACTION_NEUTRAL  = 2,   /**< @brief Action had no effect */
    ACTION_RECOVERY = 3,   /**< @brief Recovery action */
    ACTION_RESET    = 4,   /**< @brief Reset action */
} ActionType;
/** @} */

/**
 * @name KFactor Constants
 * @brief Direction constants from cornerstone.
 * @{
 */
#define K_RETREAT   (-1)              /**< @brief Moving away from goal */
#define K_MAINTAIN  (0)               /**< @brief Holding position */
#define K_ADVANCE   (1)               /**< @brief Moving toward goal */
#define K_HOLD      K_MAINTAIN        /**< @brief Alias for MAINTAIN */
#define K_ANY       (0)               /**< @brief Any direction */
#define K_FACTOR_DEFAULT K_MAINTAIN   /**< @brief Default k-factor */
/** @} */

/**
 * @name HebrewState Cornerstone Aliases
 * @brief Direct aliases for cornerstone code.
 * @{
 */
#define HEBREW_SHAVAR   MOS_HEBREW_SHAVAR  /**< @brief Cornerstone alias */
#define HEBREW_CHASER   MOS_HEBREW_CHASER  /**< @brief Cornerstone alias */
#define HEBREW_RATSAH   MOS_HEBREW_RATSAH  /**< @brief Cornerstone alias */
#define HEBREW_YASHAR   MOS_HEBREW_YASHAR  /**< @brief Cornerstone alias */
#define HEBREW_TAMIM    MOS_HEBREW_TAMIM   /**< @brief Cornerstone alias */
#define HEBREW_SHALEM   MOS_HEBREW_SHALEM  /**< @brief Cornerstone alias */
#define HEBREW_TOV      MOS_HEBREW_TOV     /**< @brief Cornerstone alias */
#define HEBREW_STATE_DEFAULT HEBREW_YASHAR /**< @brief Default state */
/** @} */

/** @} */ /* end kernel_types */

/**
 * @defgroup kernel_scores Multi-base Score Structures
 * @brief Freestanding stubs for multi-base scoring.
 * @{
 */

/**
 * @struct MultiScore
 * @brief Multi-base score representation.
 */
typedef struct MultiScore {
    int16_t base100;
    int8_t  base50;
    int8_t  base20;
    int8_t  base10;
    int8_t  base5;
    int8_t  base3;
} MultiScore;

typedef struct TernaryScore {
    int8_t base3;
    int8_t base9;
    int8_t base27;
    int8_t base81;
} TernaryScore;

// CPISI string size constants
#define CPISI_SOURCE_MAX    64
#define CPISI_DETAIL_MAX    256
#define CPISI_MSG_MAX       512

// CPISI severity and health constants
#define CPISI_SEVERITY_COUNT 3
#define CPISI_DEBUG_LEVEL_NONE    0
#define CPISI_DEBUG_LEVEL_ERROR   1
#define CPISI_DEBUG_LEVEL_WARN    2
#define CPISI_DEBUG_LEVEL_INFO    3
#define CPISI_DEBUG_LEVEL_DEBUG   4
#define CPISI_DEBUG_LEVEL_TRACE   5
#define CPISI_DEBUG_LEVEL_BASIC   1
#define CPISI_DEBUG_LEVEL_VERBOSE 2
#define CPISI_DEBUG_LEVEL_FULL    3

// CPISI error codes (DIAG tier stubs)
#define CPISI_OK_DIAG           117
#define CPISI_ERR_DIAG_NULL     118
#define CPISI_ERR_DIAG_INVALID  119
#define CPISI_ERR_DIAG_BOUNDS   120
#define CPISI_ERR_DIAG_STATE    122

// Additional cornerstone types (for witness/DAR system)
typedef enum DetectType { DETECT_NONE = 0, DETECT_ERROR, DETECT_WARNING, DETECT_INFO } DetectType;
typedef enum Severity { SEVERITY_NONE = 0, SEVERITY_LOW, SEVERITY_MEDIUM, SEVERITY_HIGH, SEVERITY_CRITICAL } Severity;
typedef enum HealthLevel {
    HEALTH_CRISIS = 0, HEALTH_POOR, HEALTH_LOW, HEALTH_NORMAL, HEALTH_GOOD, HEALTH_EXCELLENT, HEALTH_OPTIMAL
} HealthLevel;
// Note: health_level_t is defined in libtrit/matter/math.op.h

typedef struct HealthThreshold {
    int16_t crisis;
    int16_t poor;
    int16_t low;
    int16_t good;
    int16_t excellent;
} HealthThreshold;

// Restore types (for witness/DAR restore phase)
typedef enum RestoreResult {
    RESTORE_OK = 0,
    RESTORE_PARTIAL,
    RESTORE_FAILED,
    RESTORE_SKIPPED,
    RESTORE_PENDING
} RestoreResult;

typedef enum RestoreStrategy {
    RESTORE_STRATEGY_NONE = 0,
    RESTORE_STRATEGY_CHECKPOINT,
    RESTORE_STRATEGY_ROLLBACK,
    RESTORE_STRATEGY_REBUILD,
    RESTORE_STRATEGY_FAILOVER
} RestoreStrategy;

// Static assert macro for compile-time checks
#ifndef CPISI_STATIC_ASSERT
#define CPISI_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#endif

// Health score constants
#define MOS_HEALTH_MIN     (-100)         // Minimum health (broken)
#define MOS_HEALTH_NEUTRAL (0)            // Neutral health (balanced)
#define MOS_HEALTH_MAX     (100)          // Maximum health (perfect)

// Backward compatibility aliases (used by omni_dar)
#define HEALTH_MIN         MOS_HEALTH_MIN
#define HEALTH_MAX         MOS_HEALTH_MAX
#define HEALTH_NEUTRAL     MOS_HEALTH_NEUTRAL

// Health delta constants (for witness events)
#define MOS_HEALTH_MAJOR_DOWN   (-20)     // Significant degradation
#define MOS_HEALTH_SLIGHT_DOWN  (-5)      // Minor degradation
#define MOS_HEALTH_SLIGHT_UP    (5)       // Minor improvement
#define MOS_HEALTH_MAJOR_UP     (20)      // Significant improvement

// Result codes (ternary: -1=error, 0=ok, +1=info)
#define MOS_OK             TRIT_ZERO      // Success (0)
#define MOS_ERR_GENERIC    TRIT_NEG       // Generic error (-1)
#define MOS_ERR_INVALID    TRIT_NEG       // Invalid argument (-1)
#define MOS_ERR_NOMEM      TRIT_NEG       // Out of memory (-1)
#define MOS_ERR_NOTFOUND   TRIT_NEG       // Not found (-1)
#define MOS_ERR_PERM       TRIT_NEG       // Permission denied (-1)
#define MOS_ERR_BUSY       TRIT_NEG       // Resource busy (-1)
#define MOS_ERR_EXISTS     TRIT_NEG       // Already exists (-1)
#define MOS_ERR_IO         TRIT_NEG       // I/O error (-1)
#define MOS_ERR_NODEV      TRIT_NEG       // No such device (-1)
#define MOS_ERR_TIMEOUT    TRIT_NEG       // Operation timed out (-1)
#define MOS_INFO           TRIT_POS       // Informational (+1)

// Result check macros
#define MOS_FAILED(r)      ((r) < 0)      // True if result is error
#define MOS_SUCCEEDED(r)   ((r) >= 0)     // True if result is ok or info

// Address constants
#define MOS_ADDR_NULL      ((mos_vaddr_t)0)  // Null address value

// Zone constants (extra values not in enum)
#define MOS_ZONE_COUNT     3              // Number of zones
#define MOS_ZONE_UNKNOWN   (-1)           // Unknown/invalid zone

// Zone validation
#define MOS_ZONE_VALID(z)  ((z) >= MOS_ZONE_VOID && (z) < MOS_ZONE_COUNT)

// Spare metadata values (trit5 values 243-255 for special states)
#define MOS_SPARE_INVALID  243            // Invalid/uninitialized
#define MOS_SPARE_ERROR    244            // Error state
#define MOS_SPARE_PENDING  245            // Pending operation
#define MOS_SPARE_LOCKED   246            // Hard locked

// Severity levels (for diagnostic/witness events)
#define MOS_SEV_NONE       0              // No severity
#define MOS_SEV_MILD       1              // Mild issue
#define MOS_SEV_MEDIUM     2              // Medium severity
#define MOS_SEV_HIGH       3              // High severity
#define MOS_SEV_CRITICAL   4              // Critical issue

// DAR (Detect-Assess-Restore) phase constants
#define MOS_DAR_DETECT     0              // Detection phase
#define MOS_DAR_ASSESS     1              // Assessment phase
#define MOS_DAR_RESTORE    2              // Restoration phase

/**
 * =============================================================================
 * S.5 BOOT — Boot Handoff [BOOT]
 * =============================================================================
 *
 * @brief Include boot info structure for kernel-bootloader interface.
 *
 * =============================================================================
 */

#include "boot/include/bootinfo.h"  /**< @brief MosBootInfo structure */

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY BLOCK [BODY]
// =============================================================================
//
// @brief Public API declarations for kernel services.
//
// 4-Block Header Structure: METADATA → SETUP → BODY → CLOSING
//
// CONTAINS:
//   - B.1 VERSION   — Kernel version constants
//   - B.2 STATE     — Kernel state types
//   - B.3 INFO      — Kernel info structure
//   - B.4 INIT      — Initialization functions
//   - B.5 SERVICES  — Kernel service API
//   - B.5a CPISI    — CPI-SI stubs (freestanding)
//   - B.5b INSTALL  — Installer stubs
//   - B.5c DAR      — DAR/Witness functions
//   - B.5d DIAG     — Diagnostic stubs
//   - B.6 ZONE      — Zone operations
//
// =============================================================================

/**
 * =============================================================================
 * B.1 VERSION — Kernel Version Constants [VERSION]
 * =============================================================================
 *
 * @defgroup kernel_version B.1 VERSION — Kernel Version
 * @brief Ternary-encoded version: stage.phase.completion
 *
 * Stage: -1=alpha, 0=beta, +1=stable (maps to a/b/c)
 * Phase: trit5 value (0-242 phases)
 * Completion: trit5 value (0-242 completion %)
 *
 * @{
 */
#define MOS_VERSION_STAGE   TRIT_NEG        /**< @brief Alpha (-1) */
#define MOS_VERSION_PHASE   ((trit5_t)1)    /**< @brief Phase 1 */
#define MOS_VERSION_COMP    ((trit5_t)0)    /**< @brief 0% complete */
#define MOS_VERSION_STRING  "a-01.50"       /**< @brief Human-readable version */
#define MOS_CODENAME        "Genesis"       /**< @brief Release codename */
/** @} */

/**
 * =============================================================================
 * B.2 STATE — Kernel State Types [STATE]
 * =============================================================================
 *
 * @defgroup kernel_state B.2 STATE — Kernel State
 * @brief Ternary-encoded kernel state.
 *
 * Negative: Problem states (panic, halting)
 * Zero: Transitional (booting)
 * Positive: Operational (running, sabbath)
 *
 * @{
 */

typedef trit_t MosKernelState;   /**< @brief Basic kernel state (trit) */

#define MOS_KERNEL_PANIC    TRIT_NEG   /**< @brief Unrecoverable error (-1) */
#define MOS_KERNEL_BOOTING  TRIT_ZERO  /**< @brief Initialization (0) */
#define MOS_KERNEL_RUNNING  TRIT_POS   /**< @brief Operational (+1) */

typedef trit5_t MosKernelStateEx;  /**< @brief Extended state (trit5) */

#define MOS_KERNEL_STAGE0   ((MosKernelStateEx)0)  /**< @brief Bootloader */
#define MOS_KERNEL_STAGE1   ((MosKernelStateEx)1)  /**< @brief Kernel init */
#define MOS_KERNEL_STAGE2   ((MosKernelStateEx)2)  /**< @brief HAL init */
#define MOS_KERNEL_STAGE3   ((MosKernelStateEx)3)  /**< @brief Framework init */
#define MOS_KERNEL_STAGE4   ((MosKernelStateEx)4)  /**< @brief Shell init */
#define MOS_KERNEL_STAGE5   ((MosKernelStateEx)5)  /**< @brief User init */
#define MOS_KERNEL_SABBATH  ((MosKernelStateEx)6)  /**< @brief Operational rest */
/** @} */

/**
 * =============================================================================
 * B.3 INFO — Kernel Info Structure [INFO]
 * =============================================================================
 *
 * @defgroup kernel_info B.3 INFO — Kernel Info
 * @brief Global kernel state structure.
 *
 * All fields use ternary types from libtrit.
 *
 * @{
 */

/**
 * @struct MosKernelInfo
 * @brief Global kernel state and statistics.
 */
typedef struct MosKernelInfo {
    /* Version (ternary encoded) */
    trit_t      version_stage;      /**< @brief -1=alpha, 0=beta, +1=stable */
    trit5_t     version_phase;      /**< @brief Phase number (0-242) */
    trit5_t     version_comp;       /**< @brief Completion % (0-242) */
    const char* version_string;     /**< @brief Human-readable version */
    const char* codename;           /**< @brief Release name */

    /* State (ternary) */
    MosKernelState    state;        /**< @brief Basic state (trit) */
    MosKernelStateEx  stage;        /**< @brief Boot stage (trit5) */
    MosHebrewState    hebrew_state; /**< @brief Cognitive state */
    mos_health_t      health;       /**< @brief Health score */

    /* Boot info reference */
    const MosBootInfo* boot_info;   /**< @brief Boot handoff structure */

    /* Timing (ternary words) */
    mos_time_t boot_time;           /**< @brief Time boot started */
    mos_time_t uptime;              /**< @brief Current uptime */

    /* Statistics (ternary) */
    trit27_t context_switches;      /**< @brief Count as ternary word */
    trit27_t syscalls;              /**< @brief Syscall count */
    trit27_t interrupts;            /**< @brief Interrupt count */
} MosKernelInfo;
/** @} */

/**
 * =============================================================================
 * B.4 INIT — Kernel Initialization Functions [INIT]
 * =============================================================================
 *
 * @defgroup kernel_init B.4 INIT — Initialization
 * @brief Stage 1 kernel initialization functions.
 * @{
 */

/**
 * @brief Main kernel entry point.
 * @param[in] boot_info  Boot info from bootloader.
 * @note Called from entry.asm after Stage 0.
 */
void mos_kernel_main(MosBootInfo* boot_info);

/**
 * @brief Initialize memory management subsystem.
 * @param[in] boot  Boot info with memory map.
 * @return MOS_OK on success.
 */
mos_result_t mos_memory_init(const MosBootInfo* boot);

/**
 * @brief Initialize interrupt handling.
 * @return MOS_OK on success.
 */
mos_result_t mos_interrupt_init(void);

/**
 * @brief Initialize process management.
 * @return MOS_OK on success.
 */
mos_result_t mos_process_init(void);

/**
 * @brief Initialize virtual filesystem.
 * @return MOS_OK on success.
 */
mos_result_t mos_vfs_init(void);

/**
 * @brief Initialize diagnostic service.
 * @return MOS_OK on success.
 */
mos_result_t mos_diag_init(void);

/**
 * @brief Initialize OmniCode runtime.
 * @return MOS_OK on success.
 */
mos_result_t mos_omnicode_init(void);
/** @} */

/**
 * =============================================================================
 * B.5 SERVICES — Kernel Service API [SERVICES]
 * =============================================================================
 *
 * @defgroup kernel_services B.5 SERVICES — Kernel API
 * @brief Public kernel service functions.
 * @{
 */

/**
 * @name Kernel Info Access
 * @{
 */

/**
 * @brief Get pointer to kernel info structure.
 * @return Pointer to MosKernelInfo (never NULL).
 */
const MosKernelInfo* mos_kernel_info(void);

/**
 * @brief Get current kernel health score.
 * @return Health score (-100 to +100).
 */
mos_health_t mos_kernel_health(void);

/**
 * @brief Get current Hebrew cognitive state.
 * @return Hebrew state (shavar → yashar → tov).
 */
MosHebrewState mos_kernel_hebrew_state(void);

/**
 * @brief Get health trend direction.
 * @return Ternary trend: -1=degrading, 0=stable, +1=improving.
 */
trit_t mos_kernel_health_trend(void);
/** @} */

/**
 * @name Boot Path API
 * @brief Functions for querying boot source and features.
 * @{
 */

/**
 * @brief Get boot path (UEFI vs BIOS).
 * @return Ternary boot path:
 *   - +1 (TRIT_POS): UEFI/EDK2
 *   - 0 (TRIT_ZERO): Unknown
 *   - -1 (TRIT_NEG): BIOS
 */
trit_t mos_boot_path(void);

/**
 * @brief Check if booted via UEFI.
 * @return true if UEFI boot path.
 */
bool mos_is_uefi_boot(void);

/**
 * @brief Check if booted via BIOS.
 * @return true if BIOS boot path.
 */
bool mos_is_bios_boot(void);

/**
 * @brief Get boot feature flags.
 * @return Feature flags (MOS_FEAT_* bitmask).
 */
uint32_t mos_boot_features(void);

/**
 * @brief Check if boot feature is available.
 * @param[in] feature  Feature flag to check (MOS_FEAT_*).
 * @return true if feature available.
 */
bool mos_has_boot_feature(uint32_t feature);

/**
 * @brief Get boot info structure.
 *
 * Returns pointer to the boot info passed from bootloader.
 * Used by shell commands to inspect boot-time data.
 *
 * @return Pointer to boot info, or NULL if not available.
 */
const MosBootInfo* mos_get_boot_info(void);
/** @} */

/**
 * @name Stage Management
 * @{
 */

/**
 * @brief Get current Genesis boot stage.
 * @return Stage 0-7 (entry through sabbath).
 */
MosKernelStateEx mos_current_stage(void);

/**
 * @brief Advance to next Genesis boot stage.
 * @return MOS_OK if advanced, error if cannot.
 */
mos_result_t mos_advance_stage(void);
/** @} */

/**
 * @name Panic and Halt
 * @{
 */

/**
 * @brief Kernel panic — unrecoverable error.
 * @param[in] message  Panic reason.
 * @note This function never returns.
 */
void mos_panic(const char* message) __attribute__((noreturn));

/**
 * @brief Halt CPU permanently.
 * @note This function never returns.
 */
void mos_halt(void) __attribute__((noreturn));
/** @} */

/** @} */ /* end kernel_services */

// -----------------------------------------------------------------------------
// B.5a CPI-SI Stubs (freestanding kernel mode)
// -----------------------------------------------------------------------------
//
// Minimal CPI-SI functions for kernel mode. Full implementation in services.
//

/**
 * @brief Initialize CPI-SI subsystem
 * @return MOS_OK on success
 * @note  Stub for freestanding kernel - full impl in services/cpisi
 */
static inline mos_result_t mos_cpisi_init(void) {
    return MOS_OK;  // Success (no-op in freestanding mode)
}

/**
 * @brief Initialize CPI-SI identity for a process
 * @param pid      Process ID
 * @param name     Process name
 * @param desc     Identity description
 * @note  Stub for freestanding kernel - full impl in services/cpisi
 */
static inline void mos_cpisi_init_identity(mos_pid_t pid, const char* name, const char* desc) {
    (void)pid;   // Unused in stub
    (void)name;
    (void)desc;
    // Full implementation initializes CPI-SI tracking for this process
    // In freestanding mode, this is a no-op
}

// -----------------------------------------------------------------------------
// B.5a.2 DAR Witness Functions (Real Implementations)
// -----------------------------------------------------------------------------
//
// These functions are implemented in witness/witness.c
// They provide DETECT → ASSESS → RESTORE for graceful degradation
//

// Forward declarations (implemented in witness.c)
extern void mos_early_event_impl(int phase, int severity, const char* source,
                                  const char* message, int health_delta);
extern void mos_witness_event_impl(int phase, int severity, const char* source,
                                    const char* message, int health_delta);
extern mos_result_t mos_witness_import_boot_events_impl(const MosBootInfo* boot_info);
extern mos_result_t mos_witness_import_early_events_impl(void);
extern void mos_cpisi_mark_ready_impl(void);

/**
 * @brief Report an early event (before CPISI is ready)
 * @param phase        DAR phase (MOS_DAR_DETECT, MOS_DAR_ASSESS, MOS_DAR_RESTORE)
 * @param severity     Severity level (MOS_SEV_*)
 * @param source       Source component name
 * @param message      Event message
 * @param health_delta Health change (+/- delta)
 *
 * Buffers events and outputs to serial. Triggers ASSESS when thresholds crossed.
 */
static inline void mos_early_event(int phase, int severity, const char* source,
                                   const char* message, int health_delta) {
    mos_early_event_impl(phase, severity, source, message, health_delta);
}

/**
 * @brief Report a witness event (DAR pattern)
 * @param phase        DAR phase (MOS_DAR_DETECT, MOS_DAR_ASSESS, MOS_DAR_RESTORE)
 * @param severity     Severity level (MOS_SEV_*)
 * @param source       Source component name
 * @param message      Event message
 * @param health_delta Health change (+/- delta)
 *
 * Logs to DAR witness system. Triggers ASSESS/RESTORE as needed.
 */
static inline void mos_witness_event(int phase, int severity, const char* source,
                                     const char* message, int health_delta) {
    mos_witness_event_impl(phase, severity, source, message, health_delta);
}

/**
 * @brief Import boot events from Stage 0 into witness system
 * @param boot_info Boot information with event buffer
 * @return MOS_OK on success
 *
 * Connects boot chain events into kernel DAR system.
 */
static inline mos_result_t mos_witness_import_boot_events(const MosBootInfo* boot_info) {
    return mos_witness_import_boot_events_impl(boot_info);
}

/**
 * @brief Import early kernel events into witness system
 * @return MOS_OK on success
 *
 * Processes buffered early events into witness system.
 */
static inline mos_result_t mos_witness_import_early_events(void) {
    return mos_witness_import_early_events_impl();
}

/**
 * @brief Mark CPISI subsystem as ready for direct witness calls
 *
 * After this, mos_witness_event() has full capability.
 */
static inline void mos_cpisi_mark_ready(void) {
    mos_cpisi_mark_ready_impl();
}

// -----------------------------------------------------------------------------
// B.5b Installer Stubs (freestanding kernel mode)
// -----------------------------------------------------------------------------

// Installer state enum
typedef enum MosInstallState {
    MOS_INSTALL_UNKNOWN = 0,
    MOS_INSTALL_FRESH,
    MOS_INSTALL_COMPLETE,
    MOS_INSTALL_ERROR
} MosInstallState;

/**
 * @brief Check if system is installed
 * @return Installation state
 * @note  Stub for freestanding kernel - returns COMPLETE (skip installer)
 */
static inline MosInstallState mos_installer_check_state(void) {
    return MOS_INSTALL_COMPLETE;  // Skip installer in freestanding mode
}

/**
 * @brief Run the installer wizard
 * @return MOS_OK on success
 * @note  Stub for freestanding kernel - returns OK (no-op)
 */
static inline mos_result_t mos_installer_run(void) {
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.5c DAR/CPISI Internal Functions
// -----------------------------------------------------------------------------
//
// cpisi_dar_set_hebrew_state() and cpisi_dar_set_health() are declared in
// witness/manager/dar.mgr.h for CPISI_MODE >= 2 (hosted mode).
// In freestanding mode (CPISI_MODE < 2), omni_dar_impl.c is not compiled,
// so no stubs are needed here.
//

// -----------------------------------------------------------------------------
// B.5d Diagnostic Probe Stubs (freestanding kernel mode)
// -----------------------------------------------------------------------------

/**
 * @brief Register CPISI diagnostic probe
 * @return MOS_OK on success
 * @note  Stub for freestanding kernel - returns OK (no-op)
 */
static inline mos_result_t mos_diag_probe_cpisi_register(void) {
    return MOS_OK;
}

/**
 * @brief Register Witness diagnostic probe
 * @return MOS_OK on success
 * @note  Stub for freestanding kernel - returns OK (no-op)
 */
static inline mos_result_t mos_diag_probe_witness_register(void) {
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.6 Ternary Kernel Operations
// -----------------------------------------------------------------------------
//
// Kernel operations use ternary arithmetic from libtrit.
//

// Allocate memory (size in ternary)
mos_vaddr_t mos_kmalloc(mos_size_t size);
void mos_kfree(mos_vaddr_t addr);

// Process operations (PIDs are trit9)
mos_result_t mos_process_create(mos_pid_t* pid);
mos_result_t mos_process_destroy(mos_pid_t pid);
MosZone mos_process_zone(mos_pid_t pid);

/**
 * =============================================================================
 * B.6 ZONE — Zone Permission Operations [ZONE]
 * =============================================================================
 *
 * @defgroup kernel_zone B.6 ZONE — Zone Operations
 * @brief Ternary zone permission checking.
 *
 * Zones: VOID (entry) → WORD (work) → TOV (completion)
 *
 * @{
 */

/**
 * @brief Check zone permission for operation.
 * @param[in] zone   Zone to check.
 * @param[in] flags  Operation flags.
 * @return MOS_OK if permitted.
 */
mos_result_t mos_zone_check(MosZone zone, mos_flags_t flags);

/**
 * @brief Get zone transition direction.
 * @param[in] from  Source zone.
 * @param[in] to    Target zone.
 * @return Ternary direction: -1=backward, 0=same, +1=forward.
 */
trit_t mos_zone_direction(MosZone from, MosZone to);
/** @} */

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING BLOCK [CLOSING]
// =============================================================================
//
// 4-Block Header Structure: Operational guidance and closing notes.
//
// CONTAINS:
//   - X.1 OVERVIEW   — Module summary
//   - X.2 ARCHITECTURE — Ternary foundation
//   - X.3 BOOT SEQUENCE — Genesis 7-stage pattern
//   - X.4 USAGE       — Include pattern
//   - X.5 RELATED     — Related headers
//
// =============================================================================

/**
 * =============================================================================
 * X.1 OVERVIEW — Module Summary [OVERVIEW]
 * =============================================================================
 *
 * The MillenniumOS kernel is built entirely on ternary foundations.
 *
 * This header provides:
 *   - Kernel state and info structures
 *   - Boot path detection (UEFI vs BIOS)
 *   - Subsystem initialization API
 *   - CPI-SI stubs for freestanding mode
 *   - Zone permission operations
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.2 ARCHITECTURE — Ternary Foundation [ARCHITECTURE]
 * =============================================================================
 *
 * Every type uses libtrit:
 *   - Results: ternary semantics (negative=error, zero=ok, positive=info)
 *   - Addresses: trit27 (ternary word)
 *   - PIDs: trit9 (ternary process space, max 19,683)
 *   - Zones: trit (void/word/tov)
 *   - States: trit5 (Hebrew cognitive states)
 *   - Features: uint32_t flags (MOS_FEAT_*)
 *
 * The kernel thinks in ternary. Binary is just the storage medium.
 * libtrit handles all ternary-to-binary conversion.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.3 BOOT SEQUENCE — Genesis 7-Stage Pattern [BOOT]
 * =============================================================================
 *
 * 7-Stage Boot Sequence (Genesis Pattern):
 *   Stage 0: Bootloader     — "In the beginning" (UEFI/BIOS → entry.asm)
 *   Stage 1: Kernel         — "God" (kernel_main, early init)
 *   Stage 2: HAL            — "created" (hardware abstraction)
 *   Stage 3: Framework      — "the heaven" (graphics, UI)
 *   Stage 4: Shell          — "and" (user services)
 *   Stage 5: Init           — "the earth" (applications)
 *   Stage 6: Sabbath        — Rest (operational state)
 *
 * Boot Path (ternary):
 *   +1: UEFI/EDK2 — Full 64-bit, GOP graphics, ACPI provided
 *    0: Unknown   — Minimal mode
 *   -1: BIOS      — Legacy compat, VESA graphics, ACPI search
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.4 USAGE — Include Pattern [USAGE]
 * =============================================================================
 *
 * @code{.c}
 * #include "kernel.h"  // MUST be first (sets CPISI_MODE=0)
 *
 * void my_kernel_function(void) {
 *     if (mos_is_uefi_boot()) {
 *         // UEFI-specific code
 *     }
 *
 *     if (mos_has_boot_feature(MOS_FEAT_ACPI)) {
 *         // ACPI available
 *     }
 *
 *     const MosKernelInfo* info = mos_kernel_info();
 *     // Access kernel state
 * }
 * @endcode
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.5 RELATED — Related Headers [RELATED]
 * =============================================================================
 *
 * Included by this header:
 *   - trit/matter/matter.h  — libtrit trit types
 *   - boot/include/bootinfo.h — Boot handoff structure
 *
 * Related headers:
 *   - kernel/src/core/memory/memory.h
 *   - kernel/src/core/interrupt/interrupt.h
 *   - kernel/src/core/process/process.h
 *   - kernel/src/services/cpisi/cpisi.h
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.6 NOTE — Closing Guidance [NOTE]
 * =============================================================================
 *
 * Scripture: Genesis 1:2 — "And the Spirit of God moved upon the face of
 *            the waters."
 *
 * The kernel brings order from void, structure from chaos. Each boot
 * stage speaks creation into being, culminating in Sabbath rest —
 * not idleness, but fulfilled operational purpose.
 *
 * =============================================================================
 */

// =============================================================================
// END CLOSING
// =============================================================================

#endif /* MILLENNIUMOS_KERNEL_H */
