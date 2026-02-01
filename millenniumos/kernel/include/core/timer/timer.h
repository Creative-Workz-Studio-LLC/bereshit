/// @file timer.h
/// @omni code --c --type-header
///
// =============================================================================
// OMNICODE PRAGMA [PRAGMA]
// =============================================================================
//
// @omni:req  key        = MOS-kernel-core-timer
// @omni:req  from       = bereshit/word/seed/code/c/type-header.h
// @omni:req  at         = a-01.00
// @omni:inh  type       = header
// @omni:inh  role       = type_definitions
// @omni:ins  component  = kernel/core/timer
// @omni:ins  provides   = [MOS_TIMER, TIMER_TYPES, TIMER_API]
// @omni:req  title      = Timer Subsystem
// @omni:req  brief      = System timer and tick management
//
// =============================================================================
// END PRAGMA [END]
// =============================================================================

// =============================================================================
// METADATA BLOCK [METADATA]
// =============================================================================
//
// @brief Identity and context for timer subsystem.
//
// STRUCTURE: M.1-M.10 sections for complete documentation.
//
// =============================================================================

/**
 * =============================================================================
 * M.1 IDENTITY — What This Is [IDENTITY]
 * =============================================================================
 *
 * @defgroup timer M.1 IDENTITY — Timer Subsystem
 * @brief    System timer for tick counting, scheduling, and timing services.
 *
 * SUMMARY:
 *   The timer subsystem provides the heartbeat for the kernel. It uses the
 *   Programmable Interval Timer (PIT) on BIOS systems and HPET/APIC Timer
 *   on UEFI systems to generate periodic interrupts (IRQ 0) that drive
 *   the scheduler and provide timing services.
 *
 * =============================================================================
 * @{ */

/** @} */ /* end M.1 IDENTITY */

/**
 * =============================================================================
 * M.2 HEALTH — Ternary State Tracking [HEALTH]
 * =============================================================================
 *
 * @defgroup timer_health M.2 HEALTH — Ternary State Tracking
 * @brief    Timer subsystem health using ternary values.
 *
 * Health tracked via:
 *   - Tick regularity (drift from expected frequency)
 *   - Interrupt delivery reliability
 *   - Timer resolution capability
 *
 * @par Ternary Semantics
 *   - tov(+1):   Timer stable, no drift, high resolution
 *   - yashar(0): Timer operational, minor drift
 *   - shavar(-1): Timer failing, major drift, missed ticks
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.3 STATUS — Project Position [STATUS]
 * =============================================================================
 *
 * Status:    Active (Phase 1 implementation)
 * Version:   a-01.00
 * Created:   2026-02-01
 * Updated:   2026-02-01
 *
 * Authors:   Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)
 * Org:       CreativeWorkzStudio LLC
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.4 WHY — Purpose and Grounding [WHY]
 * =============================================================================
 *
 * PROBLEM:
 *   The kernel needs a consistent time source for scheduling, sleeping,
 *   timeouts, and performance measurement.
 *
 * SOLUTION:
 *   Hardware timer (PIT/HPET/APIC Timer) generates IRQ 0 at known frequency.
 *   Software counts ticks and provides timing APIs.
 *
 * GROUNDING:
 *   Genesis 1:14 — "And God said, Let there be lights in the firmament of
 *   the heaven to divide the day from the night; and let them be for signs,
 *   and for seasons, and for days, and years"
 *
 *   The timer divides the continuous flow of execution into measurable units,
 *   just as the heavenly lights divide time into seasons and days.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.5 HOW — Quick Start [HOW]
 * =============================================================================
 *
 * TYPICAL USAGE:
 * @code{.c}
 * // Initialize timer (after interrupt subsystem)
 * mos_timer_init(MOS_TIMER_FREQ_1000HZ);  // 1ms ticks
 *
 * // Get current tick count
 * uint64_t now = mos_timer_ticks();
 *
 * // Calculate elapsed time
 * uint64_t elapsed_ms = mos_timer_elapsed_ms(start_ticks);
 *
 * // Sleep for duration (uses scheduler)
 * mos_timer_sleep_ms(100);  // Sleep 100ms
 * @endcode
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.6 WHAT — Technical Specification [WHAT]
 * =============================================================================
 *
 * KEY CONSTANTS:
 *   - MOS_TIMER_FREQ_100HZ   — 10ms ticks (low resolution)
 *   - MOS_TIMER_FREQ_1000HZ  — 1ms ticks (default)
 *   - MOS_TIMER_FREQ_10000HZ — 100us ticks (high resolution)
 *
 * KEY TYPES:
 *   - MosTimerConfig  — Timer configuration structure
 *   - MosTimerStats   — Timer statistics
 *
 * KEY FUNCTIONS:
 *   - mos_timer_init()       — Initialize timer subsystem
 *   - mos_timer_ticks()      — Get current tick count
 *   - mos_timer_elapsed_ms() — Calculate elapsed milliseconds
 *   - mos_timer_sleep_ms()   — Sleep for duration
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.7 INTENT — Design Philosophy [INTENT]
 * =============================================================================
 *
 * HARDWARE ABSTRACTION:
 *   Timer subsystem abstracts over PIT (BIOS) and HPET/APIC Timer (UEFI).
 *   Same API regardless of underlying hardware.
 *
 * TERNARY FREQUENCY SCALING:
 *   Tick frequencies follow powers of 3:
 *     - 3^5 = 243 Hz (coarse)
 *     - 3^6 = 729 Hz (medium)
 *     - 3^7 = 2187 Hz (fine)
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.8 LADDER — Include Dependencies [LADDER]
 * =============================================================================
 *
 * @par Depends On:
 *   kernel.h     — Core kernel types
 *   interrupt.h  — IRQ registration
 *
 * @par Depended By:
 *   process.h    — Scheduler tick integration
 *   sleep API    — Timing services
 *
 * @par Ladder Diagram
 * @code{.txt}
 *                kernel.h (foundation)
 *                    ↓
 *              interrupt.h (IRQ 0)
 *                    ↓
 *            ┌─────────────┐
 *            │   timer.h   │ ← THIS FILE
 *            └─────────────┘
 *                    ↓
 *               process.h (scheduler)
 * @endcode
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.9 DEPENDENCIES — External Requirements [DEPENDENCIES]
 * =============================================================================
 *
 * @par M.9a HARDWARE
 *   - PIT (8254): Legacy timer, 1.193182 MHz base
 *   - HPET: High Precision Event Timer (UEFI systems)
 *   - APIC Timer: Local APIC integrated timer (SMP systems)
 *
 * @par M.9b BATON — Initialization Chain
 * @code{.txt}
 *   interrupt_init() ──────────────→ timer_init()
 *        │                               │
 *        │ Provides IRQ 0                │ Configures PIT
 *        │ registration                  │ Registers handler
 *        ↓                               ↓
 *   interrupts enabled ←──────────── scheduler uses ticks
 * @endcode
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.10 ROADMAP — Version History [ROADMAP]
 * =============================================================================
 *
 * M.10a HISTORY — Version Timeline
 * ---------------------------------
 *   a-01.00 (2026-02-01) — Initial timer subsystem
 *     • PIT initialization at 1000 Hz
 *     • Tick counting
 *     • Basic timing API
 *
 * M.10b COMPLETED — What's Done
 * -----------------------------
 *   ✓ PIT configuration (divisor calculation)
 *   ✓ IRQ 0 handler registration
 *   ✓ Tick counter
 *   ✓ Millisecond conversion
 *   ✓ Elapsed time calculation
 *
 * M.10c PHASE 1 — Basic Timer (a-01.xx) ✓ COMPLETE
 * -------------------------------------------------
 *   ✓ PIT initialization
 *   ✓ Tick counting
 *   ✓ Time conversion API
 *   ✓ Scheduler integration
 *
 * M.10d PHASE 2 — HPET Support (a-02.xx)
 * --------------------------------------
 *   ⏳ HPET detection via ACPI
 *   ⏳ HPET initialization
 *   ⏳ Nanosecond resolution
 *   ⏳ Multiple timers for different purposes
 *
 * M.10e PHASE 3 — APIC Timer (a-03.xx)
 * ------------------------------------
 *   ⏳ Local APIC timer calibration
 *   ⏳ One-shot mode for precision
 *   ⏳ Per-CPU timers for SMP
 *
 * =============================================================================
 */

// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_KERNEL_CORE_TIMER_H
#define MOS_KERNEL_CORE_TIMER_H

// =============================================================================
// SETUP BLOCK [SETUP]
// =============================================================================
//
// @brief Configuration and dependencies before body.
//
// CONTAINS:
//   - S.1 INCLUDES   — Header dependencies (kernel.h)
//   - S.2 DEFINES    — Timer constants, frequencies
//   - S.3 TYPES      — Type declarations (config, stats)
//   - S.4 PROTOTYPES — Function declarations
//   - S.5 FILE-LEVEL — [Reserved: no file-level state]
//   - S.6 OMISSION   — Reserved section guide
//
// =============================================================================

/**
 * =============================================================================
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * =============================================================================
 *
 * @defgroup timer_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 * @{ */

/** S.1a STANDARD — [Reserved: freestanding kernel, no libc] @{ @} */

/**
 * ---------------------------------------------------------------------------
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ---------------------------------------------------------------------------
 * @{ */
#include "kernel.h"                    /**< Core kernel types (MUST be first) */
/** @} */ /* end S.1b INTERNAL */

/** @} */ /* end S.1 INCLUDES */

/**
 * =============================================================================
 * S.2 DEFINES — Constants and Macros [DEFINES]
 * =============================================================================
 *
 * @defgroup timer_defines S.2 DEFINES — Constants
 * @brief    Timer constants, frequencies, and PIT configuration.
 *
 * CONTAINS:
 *   - S.2a BOUNDS — Health thresholds
 *   - S.2b CONSTS — Timer frequencies
 * @{ */

/**
 * ---------------------------------------------------------------------------
 * S.2a BOUNDS — Health Scoring Thresholds [BOUNDS]
 * ---------------------------------------------------------------------------
 *
 * @par Health Scoring
 * | Metric           | tov(+1) | yashar(0) | shavar(-1) |
 * |------------------|---------|-----------|------------|
 * | Tick drift       | <1%     | 1-5%      | >5%        |
 * | Missed ticks     | 0       | 1-10      | >10        |
 * | Resolution       | <1ms    | 1-10ms    | >10ms      |
 *
 * @{ */

/* Timer health thresholds */
#define MOS_TIMER_DRIFT_TOV     1       /**< <1% drift = tov */
#define MOS_TIMER_DRIFT_YASHAR  5       /**< 1-5% drift = yashar */
#define MOS_TIMER_MISSED_TOV    0       /**< 0 missed = tov */
#define MOS_TIMER_MISSED_SHAVAR 10      /**< >10 missed = shavar */

/** @} */ /* end S.2a BOUNDS */

/**
 * ---------------------------------------------------------------------------
 * S.2b CONSTS — Timer Constants [CONSTS]
 * ---------------------------------------------------------------------------
 * @{ */

/** @name PIT Hardware Constants
 *  @brief 8254 PIT configuration.
 *  @{ */
#define MOS_PIT_BASE_FREQ       1193182 /**< PIT base frequency in Hz */
#define MOS_PIT_CMD_PORT        0x43    /**< PIT command port */
#define MOS_PIT_CH0_PORT        0x40    /**< PIT channel 0 data port */
/** @} */ /* end PIT Hardware Constants */

/** @name Timer Frequencies
 *  @brief Supported tick frequencies.
 *  @{ */
#define MOS_TIMER_FREQ_100HZ    100     /**< 10ms ticks (low resolution) */
#define MOS_TIMER_FREQ_250HZ    250     /**< 4ms ticks */
#define MOS_TIMER_FREQ_1000HZ   1000    /**< 1ms ticks (default) */
#define MOS_TIMER_FREQ_DEFAULT  MOS_TIMER_FREQ_1000HZ
/** @} */ /* end Timer Frequencies */

/** @name Ternary Frequencies
 *  @brief Powers of 3 for ternary-aligned frequencies.
 *  @{ */
#define MOS_TIMER_FREQ_T5       243     /**< 3^5 Hz = 4.12ms */
#define MOS_TIMER_FREQ_T6       729     /**< 3^6 Hz = 1.37ms */
#define MOS_TIMER_FREQ_T7       2187    /**< 3^7 Hz = 0.46ms */
/** @} */ /* end Ternary Frequencies */

/** @} */ /* end S.2b CONSTS */

/** @} */ /* end S.2 DEFINES */

/**
 * =============================================================================
 * S.3 TYPES — Type Definitions [TYPES]
 * =============================================================================
 *
 * @defgroup timer_types S.3 TYPES — Type Definitions
 * @brief    Timer structures and enumerations.
 *
 * CONTAINS:
 *   - S.3a ENUMS   — Timer source enumeration
 *   - S.3b STRUCTS — Configuration and statistics
 * @{ */

/**
 * ---------------------------------------------------------------------------
 * S.3a ENUMS — Timer Enumerations [ENUMS]
 * ---------------------------------------------------------------------------
 * @{ */

/**
 * @enum MosTimerSource
 * @brief Hardware timer source.
 */
typedef enum MosTimerSource {
    MOS_TIMER_PIT   = 0,    /**< Programmable Interval Timer (legacy) */
    MOS_TIMER_HPET  = 1,    /**< High Precision Event Timer */
    MOS_TIMER_APIC  = 2,    /**< Local APIC Timer */
} MosTimerSource;

/** @} */ /* end S.3a ENUMS */

/**
 * ---------------------------------------------------------------------------
 * S.3b STRUCTS — Timer Structures [STRUCTS]
 * ---------------------------------------------------------------------------
 * @{ */

/**
 * @struct MosTimerConfig
 * @brief Timer configuration.
 */
typedef struct MosTimerConfig {
    MosTimerSource source;      /**< Hardware timer source */
    uint32_t frequency;         /**< Tick frequency in Hz */
    uint16_t divisor;           /**< PIT divisor value */
} MosTimerConfig;

/**
 * @struct MosTimerStats
 * @brief Timer statistics.
 */
typedef struct MosTimerStats {
    uint64_t ticks;             /**< Total tick count since boot */
    uint64_t missed_ticks;      /**< Missed/lost ticks */
    uint64_t uptime_ms;         /**< Uptime in milliseconds */
    uint32_t frequency;         /**< Current frequency */
    mos_health_t health;        /**< Timer health */
    MosTimerSource source;      /**< Active timer source */
} MosTimerStats;

/** @} */ /* end S.3b STRUCTS */

/** @} */ /* end S.3 TYPES */

/**
 * =============================================================================
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * =============================================================================
 *
 * @defgroup timer_prototypes S.4 PROTOTYPES — Function Declarations
 * @brief    Timer API function prototypes.
 *
 * CONTAINS:
 *   - S.4a LIFECYCLE — init, shutdown
 *   - S.4b ACCESSORS — ticks, time queries
 *   - S.4c TIMING    — sleep, delay
 * @{ */

/**
 * ---------------------------------------------------------------------------
 * S.4a LIFECYCLE — Initialization and Shutdown [LIFECYCLE]
 * ---------------------------------------------------------------------------
 * @{ */

/**
 * @brief Initialize timer subsystem.
 *
 * Configures the PIT (or HPET if available) to generate interrupts
 * at the specified frequency. Registers IRQ 0 handler.
 *
 * @param frequency Tick frequency in Hz (use MOS_TIMER_FREQ_* constants).
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_timer_init(uint32_t frequency);

/**
 * @brief Initialize timer with default frequency.
 *
 * Calls mos_timer_init(MOS_TIMER_FREQ_DEFAULT).
 *
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_timer_init_default(void);

/**
 * @brief Process timer tick.
 *
 * Called from IRQ 0 handler (timer_handler in handlers.c) on each tick.
 * Updates tick count, uptime, and performs drift detection.
 *
 * @note Internal function - called automatically from interrupt handler.
 */
void mos_timer_tick(void);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ---------------------------------------------------------------------------
 * S.4b ACCESSORS — Time Queries [ACCESSORS]
 * ---------------------------------------------------------------------------
 * @{ */

/**
 * @brief Get current tick count.
 *
 * Returns the number of timer interrupts since boot.
 * Monotonically increasing.
 *
 * @return Current tick count.
 */
uint64_t mos_timer_ticks(void);

/**
 * @brief Get uptime in milliseconds.
 *
 * @return Milliseconds since timer initialization.
 */
uint64_t mos_timer_uptime_ms(void);

/**
 * @brief Calculate elapsed milliseconds.
 *
 * @param start_ticks Tick count at start of measurement.
 * @return Milliseconds elapsed since start_ticks.
 */
uint64_t mos_timer_elapsed_ms(uint64_t start_ticks);

/**
 * @brief Get current timer frequency.
 *
 * @return Timer frequency in Hz.
 */
uint32_t mos_timer_frequency(void);

/**
 * @brief Get timer statistics.
 *
 * @return Pointer to timer statistics structure.
 */
const MosTimerStats* mos_timer_stats(void);

/**
 * @brief Get timer health.
 *
 * @return Current timer health value.
 */
mos_health_t mos_timer_health(void);

/** @} */ /* end S.4b ACCESSORS */

/**
 * ---------------------------------------------------------------------------
 * S.4c TIMING — Sleep and Delay [TIMING]
 * ---------------------------------------------------------------------------
 * @{ */

/**
 * @brief Sleep for specified milliseconds.
 *
 * Puts current process to sleep. Requires scheduler to be running.
 *
 * @param ms Milliseconds to sleep.
 */
void mos_timer_sleep_ms(uint32_t ms);

/**
 * @brief Busy-wait delay in milliseconds.
 *
 * Does NOT yield to scheduler. Use sparingly.
 *
 * @param ms Milliseconds to delay.
 */
void mos_timer_delay_ms(uint32_t ms);

/**
 * @brief Busy-wait delay in microseconds.
 *
 * @param us Microseconds to delay.
 */
void mos_timer_delay_us(uint32_t us);

/** @} */ /* end S.4c TIMING */

/** @} */ /* end S.4 PROTOTYPES */

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY BLOCK [BODY]
// =============================================================================
//
// @brief Reserved — type-header pattern places all declarations in SETUP.
//
// =============================================================================

/** B.1-B.5 — [Reserved: types in S.3, prototypes in S.4] @{ @} */

// =============================================================================
// END BODY
// =============================================================================

/// =============================================================================
/// CLOSING [CLOSING]
///
/// 4-Block Code Structure: Operational guidance and closing notes
/// =============================================================================

/**
 * =============================================================================
 * X.1 VALIDATION — Build and Verification [VALIDATION]
 * =============================================================================
 *
 * @defgroup timer_validation X.1 VALIDATION — Build and Verification
 * @brief    Build commands, checks, and tripwires.
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * make clean && make              # full build
 * make run                        # test in QEMU
 * @endcode
 *
 * @par X.1b CHECKS — Verification Checklist
 *   - Self-contained (includes without errors)
 *   - Timer frequency within PIT capability
 *   - Statistics structure matches implementation
 *
 * @} */

/* X.1c TRIPWIRES — Compile-time assertions */
_Static_assert(MOS_PIT_BASE_FREQ == 1193182,
    "PIT base frequency must be 1193182 Hz");
_Static_assert(MOS_TIMER_FREQ_DEFAULT >= 100 && MOS_TIMER_FREQ_DEFAULT <= 10000,
    "Default frequency must be between 100-10000 Hz");

/**
 * =============================================================================
 * X.2 EXECUTION — Runtime Flow [EXECUTION]
 * =============================================================================
 *
 * @defgroup timer_execution X.2 EXECUTION — Runtime Flow
 * @brief    Timer tick flow and scheduler integration.
 *
 * @par Tick Flow
 * @code{.txt}
 *   PIT fires IRQ 0
 *        ↓
 *   IDT vector 32
 *        ↓
 *   mos_interrupt_handler()
 *        ↓
 *   timer_handler() {
 *     g_timer_ticks++;
 *     check process time slice
 *     if exhausted: mos_schedule()
 *   }
 * @endcode
 *
 * @} */

/**
 * =============================================================================
 * X.3 CLEANUP — Resource Management [CLEANUP]
 * =============================================================================
 *
 * @defgroup timer_cleanup X.3 CLEANUP — Resource Management
 * @brief    Timer uses no dynamic resources to clean up.
 *
 * Timer runs continuously once initialized. No explicit cleanup needed.
 *
 * @} */

/**
 * =============================================================================
 * X.4-X.9 — [Reserved sections - see OMISSION]
 * =============================================================================
 */

/**
 * =============================================================================
 * X.9 OMISSION — Intentional Omissions [OMISSION]
 * =============================================================================
 *
 * @defgroup timer_omission X.9 OMISSION — Intentional Omissions
 * @brief    Sections omitted for this file.
 *
 * Reserved Sections:
 *   - X.4 POLICY: No special policies
 *   - X.5 EXTENSION: See M.10 ROADMAP
 *   - X.6 TROUBLESHOOTING: Timer failures are rare
 *   - X.7 REFERENCE: See M.8 LADDER
 *   - X.8 NOTE: No additional notes
 *
 * @} */

// =============================================================================
// END CLOSING
// =============================================================================

#endif /* MOS_KERNEL_CORE_TIMER_H */

// =============================================================================
// FILE FOOTER
// =============================================================================
//
// Related:
//   - kernel/src/core/timer/timer.c      - Timer implementation
//   - kernel/include/core/interrupt/interrupt.h - IRQ handling
//   - kernel/include/core/process/process.h     - Scheduler integration
//
// =============================================================================
// Generated by Nova Dawn for MillenniumOS
// CreativeWorkzStudio LLC - Kingdom Technology
// =============================================================================
