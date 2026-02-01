// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-clock-impl
// Title:       MillenniumOS Clock Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / Clock Subsystem
// Role:        Real-time clock for wall time and timestamps
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-02-01
// Updated:     2026-02-01
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Ecclesiastes 3:1 — "To every thing there is a season"
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"                         // All kernel types (MUST be first)
#include "core/clock/clock.h"               // Clock subsystem
#include "core/timer/timer.h"               // Timer for sub-second precision

// I/O port access
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
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

static MosClockStats g_clock = {
    .boot_time = {0},
    .uptime_seconds = 0,
    .rtc_reads = 0,
    .rtc_valid = false,
    .health = MOS_HEALTH_NEUTRAL,
};

// Cached boot ticks for uptime calculation
static uint64_t g_boot_ticks = 0;

// -----------------------------------------------------------------------------
// B.2 CMOS RTC Access
// -----------------------------------------------------------------------------

static uint8_t cmos_read(uint8_t reg) {
    // Disable NMI (bit 7) and select register
    outb(MOS_CMOS_ADDRESS, 0x80 | reg);
    return inb(MOS_CMOS_DATA);
}

static bool rtc_updating(void) {
    // Check if RTC update is in progress
    return (cmos_read(MOS_CMOS_STATUS_A) & 0x80) != 0;
}

static uint8_t bcd_to_binary(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static mos_result_t rtc_read(MosDateTime* dt) {
    if (!dt) return MOS_ERR_INVALID;

    // Wait for update to complete
    while (rtc_updating()) {
        __asm__ volatile("pause");
    }

    // Read registers
    uint8_t seconds = cmos_read(MOS_CMOS_SECONDS);
    uint8_t minutes = cmos_read(MOS_CMOS_MINUTES);
    uint8_t hours   = cmos_read(MOS_CMOS_HOURS);
    uint8_t day     = cmos_read(MOS_CMOS_DAY);
    uint8_t month   = cmos_read(MOS_CMOS_MONTH);
    uint8_t year    = cmos_read(MOS_CMOS_YEAR);

    // Check status register B for format
    uint8_t status_b = cmos_read(MOS_CMOS_STATUS_B);
    bool binary_mode = (status_b & 0x04) != 0;
    bool hour_24     = (status_b & 0x02) != 0;

    // Convert BCD if needed
    if (!binary_mode) {
        seconds = bcd_to_binary(seconds);
        minutes = bcd_to_binary(minutes);
        hours   = bcd_to_binary(hours & 0x7F);  // Mask AM/PM bit
        day     = bcd_to_binary(day);
        month   = bcd_to_binary(month);
        year    = bcd_to_binary(year);
    }

    // Handle 12-hour format
    if (!hour_24 && (hours & 0x80)) {
        // PM - add 12 (but not for 12 PM)
        hours = (hours & 0x7F);
        if (hours != 12) hours += 12;
    } else if (!hour_24 && hours == 12) {
        // 12 AM is midnight
        hours = 0;
    }

    // Calculate full year (assume 21st century if year < 70)
    uint16_t full_year = 2000 + year;
    if (year >= 70) {
        full_year = 1900 + year;
    }

    // Fill structure
    dt->year = full_year;
    dt->month = month;
    dt->day = day;
    dt->hour = hours;
    dt->minute = minutes;
    dt->second = seconds;
    dt->millisecond = 0;  // RTC doesn't provide milliseconds

    g_clock.rtc_reads++;
    g_clock.rtc_valid = true;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.3 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_clock_init(void) {
    // Read initial time from RTC
    mos_result_t result = rtc_read(&g_clock.boot_time);
    if (MOS_FAILED(result)) {
        g_clock.health = MOS_HEALTH_MIN;
        return result;
    }

    // Store boot ticks for uptime calculation
    g_boot_ticks = mos_timer_ticks();

    g_clock.uptime_seconds = 0;
    g_clock.health = MOS_HEALTH_MAX / 2;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.4 Time Queries
// -----------------------------------------------------------------------------

mos_result_t mos_clock_now(MosDateTime* dt) {
    mos_result_t result = rtc_read(dt);
    if (MOS_FAILED(result)) {
        return result;
    }

    // Add milliseconds from timer
    uint64_t current_ticks = mos_timer_ticks();
    uint32_t frequency = mos_timer_frequency();
    if (frequency > 0) {
        uint64_t tick_in_second = current_ticks % frequency;
        dt->millisecond = (uint16_t)((tick_in_second * 1000) / frequency);
    }

    return MOS_OK;
}

mos_result_t mos_clock_boot_time(MosDateTime* dt) {
    if (!dt) return MOS_ERR_INVALID;
    *dt = g_clock.boot_time;
    return MOS_OK;
}

uint64_t mos_clock_uptime_seconds(void) {
    uint64_t current_ticks = mos_timer_ticks();
    uint32_t frequency = mos_timer_frequency();

    if (frequency == 0) return 0;

    uint64_t elapsed_ticks = current_ticks - g_boot_ticks;
    return elapsed_ticks / frequency;
}

uint64_t mos_clock_timestamp_ms(void) {
    return mos_timer_uptime_ms();
}

const MosClockStats* mos_clock_stats(void) {
    // Update uptime before returning
    g_clock.uptime_seconds = mos_clock_uptime_seconds();
    return &g_clock;
}

// -----------------------------------------------------------------------------
// B.5 Formatting
// -----------------------------------------------------------------------------

// Helper: write 2-digit number
static void write_2digit(char* buf, uint8_t val) {
    buf[0] = '0' + (val / 10);
    buf[1] = '0' + (val % 10);
}

// Helper: write 4-digit number
static void write_4digit(char* buf, uint16_t val) {
    buf[0] = '0' + (val / 1000) % 10;
    buf[1] = '0' + (val / 100) % 10;
    buf[2] = '0' + (val / 10) % 10;
    buf[3] = '0' + val % 10;
}

// Helper: write 3-digit number
static void write_3digit(char* buf, uint16_t val) {
    buf[0] = '0' + (val / 100) % 10;
    buf[1] = '0' + (val / 10) % 10;
    buf[2] = '0' + val % 10;
}

mos_result_t mos_clock_format(const MosDateTime* dt, char* buf, size_t size) {
    if (!dt || !buf || size < 20) return MOS_ERR_INVALID;

    // Format: "YYYY-MM-DD HH:MM:SS"
    write_4digit(&buf[0], dt->year);
    buf[4] = '-';
    write_2digit(&buf[5], dt->month);
    buf[7] = '-';
    write_2digit(&buf[8], dt->day);
    buf[10] = ' ';
    write_2digit(&buf[11], dt->hour);
    buf[13] = ':';
    write_2digit(&buf[14], dt->minute);
    buf[16] = ':';
    write_2digit(&buf[17], dt->second);
    buf[19] = '\0';

    return MOS_OK;
}

mos_result_t mos_clock_format_log(const MosDateTime* dt, char* buf, size_t size) {
    if (!dt || !buf || size < 15) return MOS_ERR_INVALID;

    // Format: "[HH:MM:SS.mmm]"
    buf[0] = '[';
    write_2digit(&buf[1], dt->hour);
    buf[3] = ':';
    write_2digit(&buf[4], dt->minute);
    buf[6] = ':';
    write_2digit(&buf[7], dt->second);
    buf[9] = '.';
    write_3digit(&buf[10], dt->millisecond);
    buf[13] = ']';
    buf[14] = '\0';

    return MOS_OK;
}

mos_result_t mos_clock_log_prefix(char* buf, size_t size) {
    MosDateTime now;
    mos_result_t result = mos_clock_now(&now);
    if (MOS_FAILED(result)) {
        // Fallback: just use uptime in ms
        if (size >= 12) {
            uint64_t ms = mos_clock_timestamp_ms();
            // Simple format: [uptime_ms]
            buf[0] = '[';
            // Write number manually
            uint64_t n = ms;
            int i = 10;
            buf[i] = ']';
            buf[i+1] = '\0';
            do {
                buf[--i] = '0' + (n % 10);
                n /= 10;
            } while (n > 0 && i > 1);
            while (i > 1) buf[--i] = ' ';
            return MOS_OK;
        }
        return result;
    }

    return mos_clock_format_log(&now, buf, size);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Clock implementation provides:
//   - CMOS RTC reading with BCD conversion
//   - DateTime structure with year/month/day/hour/minute/second/ms
//   - Uptime tracking
//   - ISO 8601 and log format output
//   - Millisecond precision from timer integration
//
// =============================================================================
// END CLOSING
// =============================================================================
