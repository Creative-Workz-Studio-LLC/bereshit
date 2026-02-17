/// @file clock.c
/// @omni code --c --module
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// OMNICODE PRAGMA [PRAGMA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @omni:req  key        = CPISI-INTERNAL-CLOCK-IMPL
/// @omni:req  from       = templates/code/c/source.c
/// @omni:req  at         = a-01.00
///
/// @omni:inh  type       = source
/// @omni:inh  subtype    = system
/// @omni:inh  role       = clock
/// @omni:inh  structure  = 4-block
///
/// @omni:ins  component  = cpisi
/// @omni:ins  layer      = kernel/cpisi
/// @omni:ins  implements = clock.h
///
/// @omni:req  title      = CPI-SI Internal Clock Implementation
/// @omni:req  brief      = Time awareness implementation
///
/// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// METADATA BLOCK [METADATA]
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @file clock.c
 * @brief CPI-SI Internal Clock Implementation
 *
 * Grounding: Ecclesiastes 3:1 — "To every thing there is a season"
 */

// ═══════════════════════════════════════════════════════════════════════════════
// SETUP BLOCK [SETUP]
// ═══════════════════════════════════════════════════════════════════════════════

// Include kernel.h first to get CPISI_MODE
#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), clock is not available.
// The full implementation requires hosted environment with POSIX time.
#if CPISI_MODE >= 2

#define _POSIX_C_SOURCE 200809L

#include "services/cpisi/time/clock.time.h"
#include "witness/phase/detect.phase.h"  // LOG_* macros

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

// ─────────────────────────────────────────────────────────────────────────────
// S.1 INTERNAL CONSTANTS [CONSTANTS]
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Idle threshold in milliseconds (5 minutes) */
#define IDLE_THRESHOLD_MS (5 * CLOCK_MS_PER_MIN)

/** @brief Period names indexed by CircadianPeriod */
static const char* PERIOD_NAMES[] = {
    "night",      // CIRCADIAN_NIGHT
    "morning",    // CIRCADIAN_MORNING
    "afternoon",  // CIRCADIAN_AFTERNOON
    "evening",    // CIRCADIAN_EVENING
};

/** @brief Duration class names indexed by SessionDurationClass */
static const char* DURATION_CLASS_NAMES[] = {
    "brief",      // DURATION_BRIEF
    "short",      // DURATION_SHORT
    "normal",     // DURATION_NORMAL
    "extended",   // DURATION_EXTENDED
    "marathon",   // DURATION_MARATHON
};

// ═══════════════════════════════════════════════════════════════════════════════
// BODY BLOCK [BODY]
// ═══════════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────
// B.1 TIME HELPERS [HELPERS]
// ─────────────────────────────────────────────────────────────────────────────

uint64_t cpisi_clock_now_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
}

static CircadianPeriod hour_to_period(int hour) {
    if (hour >= 0 && hour < 6) {
        return CIRCADIAN_NIGHT;
    } else if (hour >= 6 && hour < 12) {
        return CIRCADIAN_MORNING;
    } else if (hour >= 12 && hour < 18) {
        return CIRCADIAN_AFTERNOON;
    } else {
        return CIRCADIAN_EVENING;
    }
}

static void update_time_fields(ClockContext* ctx) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    if (tm_info) {
        ctx->hour = tm_info->tm_hour;
        ctx->minute = tm_info->tm_min;
        ctx->day_of_week = tm_info->tm_wday;
        ctx->period = hour_to_period(ctx->hour);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// B.2 LIFECYCLE [LIFECYCLE]
// ─────────────────────────────────────────────────────────────────────────────

bool cpisi_clock_init(ClockContext* ctx) {
    if (!ctx) return false;

    memset(ctx, 0, sizeof(*ctx));

    uint64_t now = cpisi_clock_now_ms();
    ctx->session_start_ms = now;
    ctx->last_activity_ms = now;
    ctx->initialized = true;

    update_time_fields(ctx);

    LOG_DEBUG("clock", "Initialized: period=%s, hour=%02d:%02d",
              cpisi_clock_period_name(ctx->period), ctx->hour, ctx->minute);

    return true;
}

void cpisi_clock_shutdown(ClockContext* ctx) {
    if (!ctx || !ctx->initialized) return;

    SessionDurationClass duration_class = cpisi_clock_get_duration_class(ctx);

    char elapsed_str[32];
    cpisi_clock_elapsed_str(ctx, elapsed_str, sizeof(elapsed_str));

    LOG_INFO("clock", "Session duration: %s (%s)",
             elapsed_str, cpisi_clock_duration_class_name(duration_class));
    LOG_INFO("clock", "Activities: %u, Active: %llu ms, Idle: %llu ms",
             ctx->activity_count,
             (unsigned long long)ctx->active_time_ms,
             (unsigned long long)ctx->idle_time_ms);

    ctx->initialized = false;
}

void cpisi_clock_update(ClockContext* ctx) {
    if (!ctx || !ctx->initialized) return;

    update_time_fields(ctx);

    /* Check for session duration warnings */
    uint64_t elapsed = cpisi_clock_elapsed_ms(ctx);

    if (!ctx->extended_warning && elapsed >= CLOCK_EXTENDED_SESSION_MS) {
        ctx->extended_warning = true;
        LOG_WARN("clock", "Extended session: %llu ms (consider a break)",
                 (unsigned long long)elapsed);
    }

    if (!ctx->marathon_warning && elapsed >= CLOCK_EXTENDED_SESSION_MS * 2) {
        ctx->marathon_warning = true;
        LOG_WARN("clock", "Marathon session: %llu ms (please take a break!)",
                 (unsigned long long)elapsed);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// B.3 ACCESSORS [ACCESSORS]
// ─────────────────────────────────────────────────────────────────────────────

uint64_t cpisi_clock_elapsed_ms(const ClockContext* ctx) {
    if (!ctx || !ctx->initialized) return 0;
    return cpisi_clock_now_ms() - ctx->session_start_ms;
}

const char* cpisi_clock_elapsed_str(const ClockContext* ctx, char* buf, size_t buf_size) {
    if (!buf || buf_size == 0) return NULL;

    uint64_t elapsed = cpisi_clock_elapsed_ms(ctx);
    uint64_t hours = elapsed / CLOCK_MS_PER_HOUR;
    uint64_t minutes = (elapsed % CLOCK_MS_PER_HOUR) / CLOCK_MS_PER_MIN;
    uint64_t seconds = (elapsed % CLOCK_MS_PER_MIN) / CLOCK_MS_PER_SEC;

    if (hours > 0) {
        snprintf(buf, buf_size, "%llu:%02llu:%02llu",
                 (unsigned long long)hours,
                 (unsigned long long)minutes,
                 (unsigned long long)seconds);
    } else {
        snprintf(buf, buf_size, "%02llu:%02llu",
                 (unsigned long long)minutes,
                 (unsigned long long)seconds);
    }

    return buf;
}

CircadianPeriod cpisi_clock_get_period(const ClockContext* ctx) {
    if (ctx && ctx->initialized) {
        return ctx->period;
    }

    /* Calculate from current time if no context */
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    return tm_info ? hour_to_period(tm_info->tm_hour) : CIRCADIAN_MORNING;
}

const char* cpisi_clock_period_name(CircadianPeriod period) {
    if (period < 0 || period >= CIRCADIAN_COUNT) {
        return "unknown";
    }
    return PERIOD_NAMES[period];
}

SessionDurationClass cpisi_clock_get_duration_class(const ClockContext* ctx) {
    uint64_t elapsed = cpisi_clock_elapsed_ms(ctx);

    if (elapsed < 15 * CLOCK_MS_PER_MIN) {
        return DURATION_BRIEF;
    } else if (elapsed < CLOCK_MS_PER_HOUR) {
        return DURATION_SHORT;
    } else if (elapsed < CLOCK_HEALTHY_SESSION_MS) {
        return DURATION_NORMAL;
    } else if (elapsed < CLOCK_EXTENDED_SESSION_MS) {
        return DURATION_EXTENDED;
    } else {
        return DURATION_MARATHON;
    }
}

const char* cpisi_clock_duration_class_name(SessionDurationClass class_) {
    if (class_ < 0 || class_ >= DURATION_COUNT) {
        return "unknown";
    }
    return DURATION_CLASS_NAMES[class_];
}

// ─────────────────────────────────────────────────────────────────────────────
// B.4 ACTIVITY [ACTIVITY]
// ─────────────────────────────────────────────────────────────────────────────

void cpisi_clock_record_activity(ClockContext* ctx) {
    if (!ctx || !ctx->initialized) return;

    uint64_t now = cpisi_clock_now_ms();
    uint64_t since_last = now - ctx->last_activity_ms;

    /* Track idle vs active time */
    if (since_last > IDLE_THRESHOLD_MS) {
        ctx->idle_time_ms += since_last;
    } else {
        ctx->active_time_ms += since_last;
    }

    ctx->last_activity_ms = now;
    ctx->activity_count++;
}

uint64_t cpisi_clock_idle_ms(const ClockContext* ctx) {
    if (!ctx || !ctx->initialized) return 0;
    return cpisi_clock_now_ms() - ctx->last_activity_ms;
}

bool cpisi_clock_is_idle(const ClockContext* ctx) {
    return cpisi_clock_idle_ms(ctx) > IDLE_THRESHOLD_MS;
}

// ═══════════════════════════════════════════════════════════════════════════════
// END BODY
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// CLOSING BLOCK [CLOSING]
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * "To every thing there is a season, and a time to every purpose under heaven"
 * — Ecclesiastes 3:1
 *
 * Time awareness helps us work with natural rhythms, not against them.
 * The clock module provides this awareness without being intrusive.
 */

// ═══════════════════════════════════════════════════════════════════════════════
// END CLOSING
// ═══════════════════════════════════════════════════════════════════════════════

#endif /* CPISI_MODE >= 2 - Full clock implementation */
