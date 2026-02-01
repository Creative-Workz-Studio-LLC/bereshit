/// @file clock.h
/// @omni code --c --type-header
///
// =============================================================================
// OMNICODE PRAGMA [PRAGMA]
// =============================================================================
//
// @omni:req  key        = MOS-kernel-core-clock
// @omni:req  from       = bereshit/word/seed/code/c/type-header.h
// @omni:req  at         = a-01.00
// @omni:inh  type       = header
// @omni:inh  role       = type_definitions
// @omni:ins  component  = kernel/core/clock
// @omni:ins  provides   = [MOS_CLOCK, DATETIME_TYPES, TIME_API]
// @omni:req  title      = Clock Subsystem
// @omni:req  brief      = Wall-clock time and date awareness
//
// =============================================================================
// END PRAGMA [END]
// =============================================================================

// =============================================================================
// METADATA BLOCK [METADATA]
// =============================================================================
//
// @brief Identity and context for clock subsystem.
//
// STRUCTURE: M.1-M.10 sections for complete documentation.
//
// =============================================================================

/**
 * =============================================================================
 * M.1 IDENTITY — What This Is [IDENTITY]
 * =============================================================================
 *
 * @defgroup clock M.1 IDENTITY — Clock Subsystem
 * @brief    Real-time clock for wall time, debugging, and timestamping.
 *
 * SUMMARY:
 *   The clock subsystem provides calendar-aware time for:
 *   - Logging timestamps
 *   - Debug output
 *   - File system timestamps
 *   - User-facing time display
 *
 *   Unlike the timer (which counts ticks for scheduling), the clock
 *   tracks actual wall-clock time (year, month, day, hour, minute, second).
 *
 * =============================================================================
 * @{ */

/** @} */ /* end M.1 IDENTITY */

/**
 * =============================================================================
 * M.4 WHY — Purpose and Grounding [WHY]
 * =============================================================================
 *
 * PROBLEM:
 *   Debugging and logging require meaningful timestamps. Tick counts alone
 *   don't tell you "when" something happened in human terms.
 *
 * SOLUTION:
 *   Read RTC (Real-Time Clock) hardware and maintain wall-clock time.
 *   Combine with timer ticks for sub-second precision.
 *
 * GROUNDING:
 *   Ecclesiastes 3:1 — "To every thing there is a season, and a time to
 *   every purpose under the heaven"
 *
 *   Understanding WHEN things happen enables wisdom about sequence,
 *   causality, and timing in debugging.
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
 * // Initialize clock (after timer)
 * mos_clock_init();
 *
 * // Get current time
 * MosDateTime now;
 * mos_clock_now(&now);
 *
 * // Format for logging
 * char buf[32];
 * mos_clock_format(&now, buf, sizeof(buf));
 * // buf = "2026-02-01 08:30:45"
 *
 * // Get timestamp for debugging
 * uint64_t ts = mos_clock_timestamp_ms();
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
 *   a-01.00 (2026-02-01) — Initial clock subsystem
 *     • RTC reading (CMOS)
 *     • DateTime structure
 *     • Formatting API
 *
 * M.10b COMPLETED — What's Done
 * -----------------------------
 *   ✓ CMOS RTC reading
 *   ✓ MosDateTime structure
 *   ✓ Basic formatting
 *   ✓ Uptime tracking
 *
 * M.10c PHASE 1 — Basic Clock (a-01.xx) ✓ COMPLETE
 * -------------------------------------------------
 *   ✓ CMOS RTC reading
 *   ✓ DateTime structure
 *   ✓ Format to string
 *   ✓ Uptime in seconds
 *
 * M.10d PHASE 2 — Enhanced Time (a-02.xx)
 * ----------------------------------------
 *   ⏳ NTP time sync (when networking available)
 *   ⏳ Timezone support
 *   ⏳ Daylight saving time
 *
 * =============================================================================
 */

// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_KERNEL_CORE_CLOCK_H
#define MOS_KERNEL_CORE_CLOCK_H

// =============================================================================
// SETUP BLOCK [SETUP]
// =============================================================================

/**
 * =============================================================================
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * =============================================================================
 * @{ */

#include "kernel.h"                    /**< Core kernel types */

/** @} */ /* end S.1 INCLUDES */

/**
 * =============================================================================
 * S.2 DEFINES — Constants and Macros [DEFINES]
 * =============================================================================
 * @{ */

/** @name CMOS RTC Ports
 *  @brief I/O ports for CMOS RTC access.
 *  @{ */
#define MOS_CMOS_ADDRESS    0x70    /**< CMOS address port */
#define MOS_CMOS_DATA       0x71    /**< CMOS data port */
/** @} */ /* end CMOS RTC Ports */

/** @name CMOS RTC Registers
 *  @brief Register addresses for time components.
 *  @{ */
#define MOS_CMOS_SECONDS    0x00
#define MOS_CMOS_MINUTES    0x02
#define MOS_CMOS_HOURS      0x04
#define MOS_CMOS_DAY        0x07
#define MOS_CMOS_MONTH      0x08
#define MOS_CMOS_YEAR       0x09
#define MOS_CMOS_CENTURY    0x32    /**< May not exist on all systems */
#define MOS_CMOS_STATUS_A   0x0A
#define MOS_CMOS_STATUS_B   0x0B
/** @} */ /* end CMOS RTC Registers */

/** @} */ /* end S.2 DEFINES */

/**
 * =============================================================================
 * S.3 TYPES — Type Definitions [TYPES]
 * =============================================================================
 * @{ */

/**
 * @struct MosDateTime
 * @brief Date and time structure.
 */
typedef struct MosDateTime {
    uint16_t year;          /**< Year (e.g., 2026) */
    uint8_t  month;         /**< Month (1-12) */
    uint8_t  day;           /**< Day of month (1-31) */
    uint8_t  hour;          /**< Hour (0-23) */
    uint8_t  minute;        /**< Minute (0-59) */
    uint8_t  second;        /**< Second (0-59) */
    uint16_t millisecond;   /**< Millisecond (0-999) */
    uint8_t  weekday;       /**< Day of week (0=Sunday, 6=Saturday) */
} MosDateTime;

/**
 * @struct MosClockStats
 * @brief Clock statistics and state.
 */
typedef struct MosClockStats {
    MosDateTime boot_time;      /**< Time at boot */
    uint64_t uptime_seconds;    /**< Seconds since boot */
    uint64_t rtc_reads;         /**< Number of RTC reads */
    bool rtc_valid;             /**< RTC data is valid */
    mos_health_t health;        /**< Clock health */
} MosClockStats;

/** @} */ /* end S.3 TYPES */

/**
 * =============================================================================
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * =============================================================================
 * @{ */

/**
 * ---------------------------------------------------------------------------
 * S.4a LIFECYCLE — Initialization [LIFECYCLE]
 * ---------------------------------------------------------------------------
 * @{ */

/**
 * @brief Initialize clock subsystem.
 *
 * Reads initial time from RTC and sets up clock tracking.
 *
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_clock_init(void);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ---------------------------------------------------------------------------
 * S.4b ACCESSORS — Time Queries [ACCESSORS]
 * ---------------------------------------------------------------------------
 * @{ */

/**
 * @brief Get current date and time.
 *
 * @param[out] dt Pointer to MosDateTime to fill.
 * @return MOS_OK on success.
 */
mos_result_t mos_clock_now(MosDateTime* dt);

/**
 * @brief Get boot time.
 *
 * @param[out] dt Pointer to MosDateTime to fill.
 * @return MOS_OK on success.
 */
mos_result_t mos_clock_boot_time(MosDateTime* dt);

/**
 * @brief Get uptime in seconds.
 *
 * @return Seconds since boot.
 */
uint64_t mos_clock_uptime_seconds(void);

/**
 * @brief Get millisecond timestamp.
 *
 * Returns milliseconds since boot with sub-second precision
 * from timer ticks.
 *
 * @return Milliseconds since boot.
 */
uint64_t mos_clock_timestamp_ms(void);

/**
 * @brief Get clock statistics.
 *
 * @return Pointer to clock statistics structure.
 */
const MosClockStats* mos_clock_stats(void);

/** @} */ /* end S.4b ACCESSORS */

/**
 * ---------------------------------------------------------------------------
 * S.4c FORMATTING — Time String Formatting [FORMATTING]
 * ---------------------------------------------------------------------------
 * @{ */

/**
 * @brief Format datetime as ISO 8601 string.
 *
 * Format: "YYYY-MM-DD HH:MM:SS"
 *
 * @param dt    DateTime to format.
 * @param buf   Output buffer.
 * @param size  Buffer size (minimum 20 bytes).
 * @return MOS_OK on success.
 */
mos_result_t mos_clock_format(const MosDateTime* dt, char* buf, size_t size);

/**
 * @brief Format datetime for logging.
 *
 * Format: "[HH:MM:SS.mmm]"
 *
 * @param dt    DateTime to format.
 * @param buf   Output buffer.
 * @param size  Buffer size (minimum 15 bytes).
 * @return MOS_OK on success.
 */
mos_result_t mos_clock_format_log(const MosDateTime* dt, char* buf, size_t size);

/**
 * @brief Get current time as log prefix.
 *
 * @param buf   Output buffer.
 * @param size  Buffer size.
 * @return MOS_OK on success.
 */
mos_result_t mos_clock_log_prefix(char* buf, size_t size);

/** @} */ /* end S.4c FORMATTING */

/** @} */ /* end S.4 PROTOTYPES */

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY BLOCK [BODY]
// =============================================================================

/** B.1-B.5 — [Reserved: types in S.3, prototypes in S.4] @{ @} */

// =============================================================================
// END BODY
// =============================================================================

/// =============================================================================
/// CLOSING [CLOSING]
/// =============================================================================

/**
 * =============================================================================
 * X.1 VALIDATION — Build and Verification [VALIDATION]
 * =============================================================================
 * @{ */

/* Tripwires */
_Static_assert(sizeof(MosDateTime) <= 16,
    "MosDateTime should be compact");

/** @} */

// =============================================================================
// END CLOSING
// =============================================================================

#endif /* MOS_KERNEL_CORE_CLOCK_H */

// =============================================================================
// FILE FOOTER
// =============================================================================
//
// Related:
//   - kernel/src/core/clock/clock.c      - Clock implementation
//   - kernel/include/core/timer/timer.h  - Timer for sub-second precision
//
// =============================================================================
// Generated by Nova Dawn for MillenniumOS
// CreativeWorkzStudio LLC - Kingdom Technology
// =============================================================================
