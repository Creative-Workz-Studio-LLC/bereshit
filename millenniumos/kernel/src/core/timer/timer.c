// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-timer-impl
// Title:       MillenniumOS Timer Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / Timer Subsystem
// Role:        System timer for tick counting and timing services
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-02-01
// Updated:     2026-02-01
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:14 — "Let there be lights...for seasons, and for days"
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"                         // All kernel types (MUST be first)
#include "core/timer/timer.h"               // Timer subsystem
#include "core/interrupt/interrupt.h"       // Interrupt registration
#include "core/process/process.h"           // Scheduler integration

// I/O port access
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global State
// -----------------------------------------------------------------------------

static MosTimerStats g_timer = {
    .ticks = 0,
    .missed_ticks = 0,
    .uptime_ms = 0,
    .frequency = 0,
    .health = MOS_HEALTH_NEUTRAL,
    .source = MOS_TIMER_PIT,
};

static uint32_t g_ms_per_tick = 1;          // Milliseconds per tick
static uint64_t g_last_tick_tsc = 0;        // TSC at last tick (for drift detection)
static uint64_t g_expected_tsc_delta = 0;   // Expected TSC between ticks

// Read timestamp counter
static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

// -----------------------------------------------------------------------------
// B.2 PIT Configuration
// -----------------------------------------------------------------------------

static mos_result_t pit_init(uint32_t frequency) {
    if (frequency == 0 || frequency > MOS_PIT_BASE_FREQ) {
        return MOS_ERR_INVALID;
    }

    // Calculate divisor
    uint16_t divisor = (uint16_t)(MOS_PIT_BASE_FREQ / frequency);
    if (divisor < 1) divisor = 1;

    // PIT command: channel 0, lobyte/hibyte, rate generator
    outb(MOS_PIT_CMD_PORT, 0x36);

    // Send divisor (low byte first, then high byte)
    outb(MOS_PIT_CH0_PORT, divisor & 0xFF);
    outb(MOS_PIT_CH0_PORT, (divisor >> 8) & 0xFF);

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.3 Timer Tick Handler
// -----------------------------------------------------------------------------
//
// Called from interrupt handlers.c timer_handler().
// NOTE: The actual IRQ handling is in handlers.c - this tracks state.
//

void mos_timer_tick(void) {
    uint64_t now_tsc = rdtsc();

    // Increment tick counter
    g_timer.ticks++;

    // Update uptime
    g_timer.uptime_ms = g_timer.ticks * g_ms_per_tick;

    // Drift detection (health scoring)
    if (g_last_tick_tsc != 0 && g_expected_tsc_delta != 0) {
        uint64_t actual_delta = now_tsc - g_last_tick_tsc;
        int64_t drift_percent = 0;

        if (g_expected_tsc_delta > 0) {
            drift_percent = ((int64_t)actual_delta - (int64_t)g_expected_tsc_delta) * 100
                          / (int64_t)g_expected_tsc_delta;
        }

        // Update health based on drift
        if (drift_percent < 0) drift_percent = -drift_percent;  // Absolute value

        if (drift_percent < MOS_TIMER_DRIFT_TOV) {
            g_timer.health = MOS_HEALTH_MAX / 2;  // tov
        } else if (drift_percent < MOS_TIMER_DRIFT_YASHAR) {
            g_timer.health = MOS_HEALTH_NEUTRAL;  // yashar
        } else {
            g_timer.health = MOS_HEALTH_MIN / 2;  // shavar
        }
    }

    g_last_tick_tsc = now_tsc;
}

// -----------------------------------------------------------------------------
// B.4 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_timer_init(uint32_t frequency) {
    if (frequency == 0) {
        return MOS_ERR_INVALID;
    }

    // Initialize PIT
    mos_result_t result = pit_init(frequency);
    if (MOS_FAILED(result)) {
        return result;
    }

    // Store frequency and calculate ms per tick
    g_timer.frequency = frequency;
    g_ms_per_tick = 1000 / frequency;
    if (g_ms_per_tick == 0) g_ms_per_tick = 1;

    // Initialize TSC calibration
    // Assume ~3GHz CPU for now (would calibrate properly in production)
    g_expected_tsc_delta = 3000000000ULL / frequency;

    // Reset counters
    g_timer.ticks = 0;
    g_timer.missed_ticks = 0;
    g_timer.uptime_ms = 0;
    g_timer.health = MOS_HEALTH_NEUTRAL;
    g_timer.source = MOS_TIMER_PIT;

    g_last_tick_tsc = rdtsc();

    // Enable timer IRQ
    mos_irq_enable(0);

    return MOS_OK;
}

mos_result_t mos_timer_init_default(void) {
    return mos_timer_init(MOS_TIMER_FREQ_DEFAULT);
}

// -----------------------------------------------------------------------------
// B.5 Accessors
// -----------------------------------------------------------------------------

uint64_t mos_timer_ticks(void) {
    return g_timer.ticks;
}

uint64_t mos_timer_uptime_ms(void) {
    return g_timer.uptime_ms;
}

uint64_t mos_timer_elapsed_ms(uint64_t start_ticks) {
    uint64_t current = g_timer.ticks;
    if (current < start_ticks) {
        // Handle wraparound (unlikely with 64-bit counter)
        return (UINT64_MAX - start_ticks + current) * g_ms_per_tick;
    }
    return (current - start_ticks) * g_ms_per_tick;
}

uint32_t mos_timer_frequency(void) {
    return g_timer.frequency;
}

const MosTimerStats* mos_timer_stats(void) {
    return &g_timer;
}

mos_health_t mos_timer_health(void) {
    return g_timer.health;
}

// -----------------------------------------------------------------------------
// B.6 Timing Functions
// -----------------------------------------------------------------------------

void mos_timer_sleep_ms(uint32_t ms) {
    // Convert ms to ticks
    uint64_t ticks_to_wait = ms / g_ms_per_tick;
    if (ticks_to_wait == 0) ticks_to_wait = 1;

    // Calculate target wake tick
    uint64_t target = g_timer.ticks + ticks_to_wait;

    // Use scheduler sleep
    mos_result_t result = mos_sleep(ticks_to_wait);

    // If scheduler sleep fails, busy-wait to target
    if (MOS_FAILED(result)) {
        while (g_timer.ticks < target) {
            __asm__ volatile("pause");
        }
    }
}

void mos_timer_delay_ms(uint32_t ms) {
    // Busy wait (use sparingly)
    uint64_t start = g_timer.ticks;
    uint64_t ticks_to_wait = ms / g_ms_per_tick;
    if (ticks_to_wait == 0) ticks_to_wait = 1;

    while ((g_timer.ticks - start) < ticks_to_wait) {
        __asm__ volatile("pause");
    }
}

void mos_timer_delay_us(uint32_t us) {
    // TSC-based microsecond delay
    // Assume ~3GHz CPU
    uint64_t cycles = (uint64_t)us * 3000;
    uint64_t start = rdtsc();

    while ((rdtsc() - start) < cycles) {
        __asm__ volatile("pause");
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Timer implementation provides:
//   - PIT configuration at specified frequency
//   - Tick counting with 64-bit counter
//   - Millisecond time conversion
//   - Drift detection and health scoring
//   - Sleep and delay functions
//
// =============================================================================
// END CLOSING
// =============================================================================
