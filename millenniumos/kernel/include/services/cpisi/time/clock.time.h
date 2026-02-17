/// @file clock.h
/// @omni code --c --library
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// OMNICODE PRAGMA [PRAGMA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @omni:req  key        = CPISI-INTERNAL-CLOCK
/// @omni:req  from       = templates/code/c/header.h
/// @omni:req  at         = a-01.00
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = system
/// @omni:inh  role       = clock
/// @omni:inh  structure  = 4-block
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = kernel/cpisi
/// @omni:ins  includes   = [stdint.h, stdbool.h]
/// @omni:ins  provides   = [CircadianPeriod, ClockContext, clock functions]
///
/// @omni:req  title      = CPI-SI Internal Clock
/// @omni:req  brief      = Time awareness for sessions — circadian patterns and duration tracking
///
/// ═══════════════════════════════════════════════════════════════════════════════

#ifndef CPISI_CLOCK_H
#define CPISI_CLOCK_H

// ═══════════════════════════════════════════════════════════════════════════════
// METADATA BLOCK [METADATA]
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @file clock.h
 * @brief CPI-SI Internal Clock — Time awareness for sessions.
 *
 * PURPOSE:
 *   Track time within CPI-SI sessions for:
 *   - Session duration (elapsed time since session start)
 *   - Circadian awareness (morning, afternoon, evening, night)
 *   - Time-based patterns (work rhythms, natural stopping points)
 *
 * GROUNDING:
 *   "To every thing there is a season, and a time to every purpose"
 *   — Ecclesiastes 3:1
 *
 *   Time awareness enables natural rhythms in work and rest.
 *
 * INTEGRATION:
 *   - DAR Witness: timestamps in all logs
 *   - Health System: circadian can influence baseline
 *   - Session Database: duration tracking for analytics
 */

// ═══════════════════════════════════════════════════════════════════════════════
// SETUP BLOCK [SETUP]
// ═══════════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────
// S.1 INCLUDES [INCLUDES]
// ─────────────────────────────────────────────────────────────────────────────

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ─────────────────────────────────────────────────────────────────────────────
// S.2 DEFINES [DEFINES]
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Milliseconds per second */
#define CLOCK_MS_PER_SEC     1000

/** @brief Milliseconds per minute */
#define CLOCK_MS_PER_MIN     (60 * CLOCK_MS_PER_SEC)

/** @brief Milliseconds per hour */
#define CLOCK_MS_PER_HOUR    (60 * CLOCK_MS_PER_MIN)

/** @brief Default healthy session duration (2 hours) */
#define CLOCK_HEALTHY_SESSION_MS  (2 * CLOCK_MS_PER_HOUR)

/** @brief Extended session warning threshold (4 hours) */
#define CLOCK_EXTENDED_SESSION_MS (4 * CLOCK_MS_PER_HOUR)

// ─────────────────────────────────────────────────────────────────────────────
// S.3 TYPES [TYPES]
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Circadian period of day.
 *
 * Natural divisions of the day that affect energy levels and work patterns.
 * Based on typical circadian rhythms.
 */
typedef enum CircadianPeriod {
    CIRCADIAN_NIGHT,        /**< 00:00-05:59 — Rest period, low energy */
    CIRCADIAN_MORNING,      /**< 06:00-11:59 — Fresh energy, high focus */
    CIRCADIAN_AFTERNOON,    /**< 12:00-17:59 — Post-lunch dip, then recovery */
    CIRCADIAN_EVENING,      /**< 18:00-23:59 — Winding down, creative time */
    CIRCADIAN_COUNT         /**< Sentinel for array sizing */
} CircadianPeriod;

/**
 * @brief Session duration category.
 *
 * Categorizes session length for pattern analysis and health monitoring.
 */
typedef enum SessionDurationClass {
    DURATION_BRIEF,         /**< < 15 minutes — quick check */
    DURATION_SHORT,         /**< 15-60 minutes — focused task */
    DURATION_NORMAL,        /**< 1-2 hours — typical work session */
    DURATION_EXTENDED,      /**< 2-4 hours — long session */
    DURATION_MARATHON,      /**< > 4 hours — needs break reminder */
    DURATION_COUNT          /**< Sentinel for array sizing */
} SessionDurationClass;

/**
 * @brief Internal clock context for CPI-SI sessions.
 *
 * Tracks time-related state for a session.
 */
typedef struct ClockContext {
    /* Session timing */
    uint64_t         session_start_ms;   /**< Session start (epoch ms) */
    uint64_t         last_activity_ms;   /**< Last user activity (epoch ms) */

    /* Current time snapshot */
    int              hour;               /**< Current hour (0-23) */
    int              minute;             /**< Current minute (0-59) */
    int              day_of_week;        /**< Day of week (0=Sun, 6=Sat) */
    CircadianPeriod  period;             /**< Current circadian period */

    /* Session statistics */
    uint64_t         active_time_ms;     /**< Estimated active work time */
    uint64_t         idle_time_ms;       /**< Time spent idle */
    uint32_t         activity_count;     /**< Number of activities recorded */

    /* Flags */
    bool             initialized;        /**< Whether context is initialized */
    bool             extended_warning;   /**< Extended session warning issued */
    bool             marathon_warning;   /**< Marathon session warning issued */
} ClockContext;

// ─────────────────────────────────────────────────────────────────────────────
// S.4 PROTOTYPES [PROTOTYPES]
// ─────────────────────────────────────────────────────────────────────────────

// ───────────────────────────────────────────────────────────────────────────────
// S.4a LIFECYCLE — Context management [LIFECYCLE]
// ───────────────────────────────────────────────────────────────────────────────

/**
 * @brief Initialize clock context.
 *
 * Sets session_start_ms to current time, initializes all fields.
 *
 * @param[out] ctx  Context to initialize.
 *
 * @return true on success.
 */
bool cpisi_clock_init(ClockContext* ctx);

/**
 * @brief Shutdown clock context.
 *
 * Logs final session duration statistics.
 *
 * @param[in,out] ctx  Context to shutdown.
 */
void cpisi_clock_shutdown(ClockContext* ctx);

/**
 * @brief Update clock context with current time.
 *
 * Should be called periodically (e.g., each frame or command) to keep
 * time fields current and check for session duration warnings.
 *
 * @param[in,out] ctx  Context to update.
 */
void cpisi_clock_update(ClockContext* ctx);

// ───────────────────────────────────────────────────────────────────────────────
// S.4b ACCESSORS — Time queries [ACCESSORS]
// ───────────────────────────────────────────────────────────────────────────────

/**
 * @brief Get current timestamp in milliseconds (epoch time).
 *
 * @return Current time in milliseconds since Unix epoch.
 */
uint64_t cpisi_clock_now_ms(void);

/**
 * @brief Get session elapsed time in milliseconds.
 *
 * @param[in] ctx  Clock context.
 *
 * @return Milliseconds since session start.
 */
uint64_t cpisi_clock_elapsed_ms(const ClockContext* ctx);

/**
 * @brief Get session elapsed time formatted as string.
 *
 * Format: "HH:MM:SS" or "MM:SS" for shorter durations.
 *
 * @param[in]  ctx       Clock context.
 * @param[out] buf       Buffer to receive formatted string.
 * @param[in]  buf_size  Size of buffer.
 *
 * @return Pointer to buf, or NULL on error.
 */
const char* cpisi_clock_elapsed_str(const ClockContext* ctx, char* buf, size_t buf_size);

/**
 * @brief Get current circadian period.
 *
 * @param[in] ctx  Clock context (or NULL to calculate from current time).
 *
 * @return Current CircadianPeriod.
 */
CircadianPeriod cpisi_clock_get_period(const ClockContext* ctx);

/**
 * @brief Get circadian period name string.
 *
 * @param[in] period  CircadianPeriod value.
 *
 * @return Static string: "night", "morning", "afternoon", "evening".
 */
const char* cpisi_clock_period_name(CircadianPeriod period);

/**
 * @brief Get session duration classification.
 *
 * @param[in] ctx  Clock context.
 *
 * @return SessionDurationClass based on elapsed time.
 */
SessionDurationClass cpisi_clock_get_duration_class(const ClockContext* ctx);

/**
 * @brief Get duration class name string.
 *
 * @param[in] class_  SessionDurationClass value.
 *
 * @return Static string: "brief", "short", "normal", "extended", "marathon".
 */
const char* cpisi_clock_duration_class_name(SessionDurationClass class_);

// ───────────────────────────────────────────────────────────────────────────────
// S.4c ACTIVITY — Track user activity [ACTIVITY]
// ───────────────────────────────────────────────────────────────────────────────

/**
 * @brief Record user activity (command, input, interaction).
 *
 * Updates last_activity_ms and tracks active vs idle time.
 *
 * @param[in,out] ctx  Clock context.
 */
void cpisi_clock_record_activity(ClockContext* ctx);

/**
 * @brief Get time since last activity in milliseconds.
 *
 * @param[in] ctx  Clock context.
 *
 * @return Milliseconds since last recorded activity.
 */
uint64_t cpisi_clock_idle_ms(const ClockContext* ctx);

/**
 * @brief Check if user appears to be idle (> 5 minutes no activity).
 *
 * @param[in] ctx  Clock context.
 *
 * @return true if idle for > 5 minutes.
 */
bool cpisi_clock_is_idle(const ClockContext* ctx);

// ═══════════════════════════════════════════════════════════════════════════════
// END SETUP
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// BODY BLOCK [BODY]
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @defgroup cpisi_clock CPI-SI Internal Clock
 * @brief Time awareness for CPI-SI sessions.
 *
 * USAGE:
 * @code
 * ClockContext clock = {0};
 * cpisi_clock_init(&clock);
 *
 * // During session...
 * cpisi_clock_update(&clock);
 * cpisi_clock_record_activity(&clock);
 *
 * // Query time
 * printf("Period: %s\n", cpisi_clock_period_name(clock.period));
 * printf("Elapsed: %llu ms\n", cpisi_clock_elapsed_ms(&clock));
 *
 * // At shutdown
 * cpisi_clock_shutdown(&clock);
 * @endcode
 *
 * CIRCADIAN PERIODS:
 *   - Night (00-06): Low energy, rest time
 *   - Morning (06-12): Peak focus, deep work
 *   - Afternoon (12-18): Variable, post-lunch dip
 *   - Evening (18-24): Creative time, winding down
 *
 * @{
 */

/** @} */ /* end cpisi_clock */

// ═══════════════════════════════════════════════════════════════════════════════
// END BODY
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// CLOSING BLOCK [CLOSING]
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief Future enhancements
 *
 * - Timezone awareness (for distributed sessions)
 * - Work pattern learning (when user typically works)
 * - Break reminders based on circadian + duration
 * - Integration with health score (circadian-adjusted baseline)
 */

// ═══════════════════════════════════════════════════════════════════════════════
// END CLOSING
// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_CLOCK_H */
