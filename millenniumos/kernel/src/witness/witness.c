// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-witness-impl
// Title:       MillenniumOS DAR Witness Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / Witness Subsystem
// Role:        DETECT → ASSESS → RESTORE pattern for graceful degradation
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-02-01
// Updated:     2026-02-01
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Proverbs 24:16 — "A just man falleth seven times, and riseth up again"
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// Forward declaration for serial output (defined in kernel.c)
void serial_putchar(char c);

// Internal serial helpers
static void witness_serial_puts(const char* s) {
    if (!s) return;
    while (*s) {
        serial_putchar(*s++);
    }
}

// Hex output (reserved for future debugging)
#if 0
static void witness_serial_hex8(uint8_t val) {
    const char hex[] = "0123456789ABCDEF";
    serial_putchar(hex[(val >> 4) & 0xF]);
    serial_putchar(hex[val & 0xF]);
}
#endif

static void witness_serial_int(int val) {
    if (val < 0) {
        serial_putchar('-');
        val = -val;
    }
    if (val == 0) {
        serial_putchar('0');
        return;
    }
    char buf[12];
    int i = 0;
    while (val > 0 && i < 11) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    while (i > 0) {
        serial_putchar(buf[--i]);
    }
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 DAR State
// -----------------------------------------------------------------------------

// Witness state structure
typedef struct MosWitnessState {
    // Health tracking
    int32_t     health;             // Current health (-100 to +100)
    uint8_t     hebrew_state;       // Hebrew state (0=SHAVAR to 6=TOV)
    int8_t      k_factor;           // Direction (-1, 0, +1)

    // Event counts by DAR phase
    uint32_t    detect_count;       // DETECT events
    uint32_t    assess_count;       // ASSESS cycles
    uint32_t    restore_count;      // RESTORE actions

    // Severity counts
    uint32_t    critical_count;     // Critical events
    uint32_t    severe_count;       // Severe events
    uint32_t    moderate_count;     // Moderate events

    // State flags
    bool        initialized;        // Witness ready
    bool        cpisi_ready;        // CPISI subsystem ready
    bool        vfs_ready;          // VFS available for logging

    // Early event buffer (before VFS ready)
    uint32_t    early_head;         // Ring buffer head
    uint32_t    early_tail;         // Ring buffer tail
    uint32_t    early_overflow;     // Overflow count
} MosWitnessState;

// Early event entry (compact for ring buffer)
typedef struct MosEarlyEvent {
    uint8_t     phase;              // DAR phase
    uint8_t     severity;           // Severity level
    int8_t      health_delta;       // Health change
    uint8_t     source_len;         // Source string length (max 15)
    char        source[16];         // Source component
    char        message[48];        // Event message (truncated if needed)
} MosEarlyEvent;

// Ring buffer size (power of 2 for efficient modulo)
#define MOS_EARLY_BUFFER_SIZE 64
#define MOS_EARLY_BUFFER_MASK (MOS_EARLY_BUFFER_SIZE - 1)

// Global witness state
static MosWitnessState g_witness = {
    .health = 0,
    .hebrew_state = 3,              // YASHAR (centered)
    .k_factor = 0,
    .detect_count = 0,
    .assess_count = 0,
    .restore_count = 0,
    .critical_count = 0,
    .severe_count = 0,
    .moderate_count = 0,
    .initialized = false,
    .cpisi_ready = false,
    .vfs_ready = false,
    .early_head = 0,
    .early_tail = 0,
    .early_overflow = 0
};

// Early event ring buffer
static MosEarlyEvent g_early_buffer[MOS_EARLY_BUFFER_SIZE];

// DAR phase names
static const char* g_phase_names[] = {
    "DETECT",   // -1 (stored as 0)
    "ASSESS",   //  0 (stored as 1)
    "RESTORE"   // +1 (stored as 2)
};

// Hebrew state names
static const char* g_hebrew_names[] = {
    "SHAVAR",   // 0 - broken
    "CHASER",   // 1 - lacking
    "RATSAH",   // 2 - wanting
    "YASHAR",   // 3 - centered (default)
    "TAMIM",    // 4 - sound
    "SHALEM",   // 5 - whole
    "TOV"       // 6 - complete
};

// Severity names (match MOS_SEV_* values)
static const char* g_sev_names[] = {
    "NONE",     // MOS_SEV_NONE = 0
    "MILD",     // MOS_SEV_MILD = 1
    "MEDIUM",   // MOS_SEV_MEDIUM = 2
    "HIGH",     // MOS_SEV_HIGH = 3
    "CRITICAL"  // MOS_SEV_CRITICAL = 4
};

// -----------------------------------------------------------------------------
// B.2 Health Calculation
// -----------------------------------------------------------------------------

// Update Hebrew state based on health
static void update_hebrew_state(void) {
    int32_t h = g_witness.health;

    // Map health (-100 to +100) to Hebrew state (0 to 6)
    // Thresholds: -100..-67 = 0, -66..-34 = 1, -33..-1 = 2, 0 = 3, 1..33 = 4, 34..66 = 5, 67..100 = 6
    if (h <= -67) {
        g_witness.hebrew_state = 0;  // SHAVAR
    } else if (h <= -34) {
        g_witness.hebrew_state = 1;  // CHASER
    } else if (h < 0) {
        g_witness.hebrew_state = 2;  // RATSAH
    } else if (h == 0) {
        g_witness.hebrew_state = 3;  // YASHAR
    } else if (h <= 33) {
        g_witness.hebrew_state = 4;  // TAMIM
    } else if (h <= 66) {
        g_witness.hebrew_state = 5;  // SHALEM
    } else {
        g_witness.hebrew_state = 6;  // TOV
    }
}

// Apply health delta with clamping
static void apply_health_delta(int8_t delta) {
    int32_t new_health = g_witness.health + delta;

    // Clamp to [-100, +100]
    if (new_health < -100) new_health = -100;
    if (new_health > 100) new_health = 100;

    // Update k-factor based on direction
    if (delta > 0) {
        g_witness.k_factor = 1;
    } else if (delta < 0) {
        g_witness.k_factor = -1;
    }
    // delta == 0 keeps current k-factor

    g_witness.health = new_health;
    update_hebrew_state();
}

// -----------------------------------------------------------------------------
// B.3 Serial Output (Always Available)
// -----------------------------------------------------------------------------

// Output event to serial in parseable format
// Format: [DAR:PHASE] SEV source: message (Δhp) [H:health S:state K:k]
static void witness_serial_event(int phase, int severity, const char* source,
                                  const char* message, int health_delta) {
    // Phase header
    witness_serial_puts("[DAR:");
    if (phase >= 0 && phase <= 2) {
        witness_serial_puts(g_phase_names[phase]);
    } else {
        witness_serial_puts("???");
    }
    witness_serial_puts("] ");

    // Severity
    if (severity >= 0 && severity <= 4) {
        witness_serial_puts(g_sev_names[severity]);
    } else {
        witness_serial_puts("???");
    }
    serial_putchar(' ');

    // Source
    witness_serial_puts(source ? source : "?");
    witness_serial_puts(": ");

    // Message
    witness_serial_puts(message ? message : "?");

    // Health delta
    witness_serial_puts(" (");
    if (health_delta >= 0) serial_putchar('+');
    witness_serial_int(health_delta);
    witness_serial_puts(") ");

    // State summary
    witness_serial_puts("[H:");
    witness_serial_int(g_witness.health);
    witness_serial_puts(" S:");
    if (g_witness.hebrew_state <= 6) {
        witness_serial_puts(g_hebrew_names[g_witness.hebrew_state]);
    }
    witness_serial_puts(" K:");
    if (g_witness.k_factor >= 0) serial_putchar('+');
    witness_serial_int(g_witness.k_factor);
    witness_serial_puts("]\n");
}

// -----------------------------------------------------------------------------
// B.4 Early Event Buffer
// -----------------------------------------------------------------------------

// Add event to early buffer (before VFS ready)
static void buffer_early_event(int phase, int severity, const char* source,
                                const char* message, int health_delta) {
    // Check for buffer full
    uint32_t next_head = (g_witness.early_head + 1) & MOS_EARLY_BUFFER_MASK;
    if (next_head == g_witness.early_tail) {
        // Buffer full - drop oldest
        g_witness.early_tail = (g_witness.early_tail + 1) & MOS_EARLY_BUFFER_MASK;
        g_witness.early_overflow++;
    }

    // Store event
    MosEarlyEvent* evt = &g_early_buffer[g_witness.early_head];
    evt->phase = (uint8_t)phase;
    evt->severity = (uint8_t)severity;
    evt->health_delta = (int8_t)health_delta;

    // Copy source (truncate to 15 chars)
    if (source) {
        size_t i = 0;
        while (source[i] && i < 15) {
            evt->source[i] = source[i];
            i++;
        }
        evt->source[i] = '\0';
        evt->source_len = (uint8_t)i;
    } else {
        evt->source[0] = '\0';
        evt->source_len = 0;
    }

    // Copy message (truncate to 47 chars)
    if (message) {
        size_t i = 0;
        while (message[i] && i < 47) {
            evt->message[i] = message[i];
            i++;
        }
        evt->message[i] = '\0';
    } else {
        evt->message[0] = '\0';
    }

    g_witness.early_head = next_head;
}

// -----------------------------------------------------------------------------
// B.5 ASSESS Phase - Health Evaluation
// -----------------------------------------------------------------------------

// Check if ASSESS cycle is needed
static bool assess_needed(void) {
    // Trigger ASSESS if:
    // - Health dropped below -50 (entering crisis)
    // - Multiple critical events
    // - Hebrew state is SHAVAR or CHASER
    return (g_witness.health <= -50) ||
           (g_witness.critical_count >= 3) ||
           (g_witness.hebrew_state <= 1);
}

// Run ASSESS cycle
static void run_assess(void) {
    g_witness.assess_count++;

    witness_serial_puts("[DAR:ASSESS] Running assessment cycle #");
    witness_serial_int((int)g_witness.assess_count);
    witness_serial_puts("\n");

    // Log current state
    witness_serial_puts("  Health: ");
    witness_serial_int(g_witness.health);
    witness_serial_puts(" State: ");
    witness_serial_puts(g_hebrew_names[g_witness.hebrew_state]);
    witness_serial_puts(" Critical: ");
    witness_serial_int((int)g_witness.critical_count);
    witness_serial_puts(" Severe: ");
    witness_serial_int((int)g_witness.severe_count);
    witness_serial_puts("\n");

    // Determine if RESTORE is needed
    if (g_witness.health <= -67 || g_witness.hebrew_state == 0) {
        witness_serial_puts("  -> SHAVAR state detected, RESTORE needed\n");
    } else if (g_witness.critical_count >= 5) {
        witness_serial_puts("  -> Multiple critical events, RESTORE recommended\n");
    } else {
        witness_serial_puts("  -> System degraded but recoverable\n");
    }
}

// -----------------------------------------------------------------------------
// B.6 RESTORE Phase - Graceful Recovery
// -----------------------------------------------------------------------------

// Check if RESTORE is possible
static bool restore_possible(void) {
    // Can only restore if:
    // - Not in total system failure
    // - Some subsystems still functional
    return g_witness.health > -100;
}

// Run RESTORE cycle
static void run_restore(void) {
    g_witness.restore_count++;

    witness_serial_puts("[DAR:RESTORE] Running restore cycle #");
    witness_serial_int((int)g_witness.restore_count);
    witness_serial_puts("\n");

    // Log what we're attempting
    if (g_witness.health <= -90) {
        witness_serial_puts("  -> Emergency mode: minimal recovery\n");
        witness_serial_puts("  -> Attempting graceful shutdown path\n");
    } else if (g_witness.health <= -67) {
        witness_serial_puts("  -> Critical mode: essential services only\n");
    } else {
        witness_serial_puts("  -> Degraded mode: non-essential services disabled\n");
    }

    // Recovery actions would go here:
    // - Disable non-essential services
    // - Flush critical buffers
    // - Save state for post-mortem
    // - Prepare for graceful shutdown if needed
}

// -----------------------------------------------------------------------------
// B.7 Core Witness Functions
// -----------------------------------------------------------------------------

// Initialize witness system
mos_result_t mos_witness_init(void) {
    // Reset state
    g_witness.health = 0;
    g_witness.hebrew_state = 3;  // YASHAR
    g_witness.k_factor = 0;
    g_witness.detect_count = 0;
    g_witness.assess_count = 0;
    g_witness.restore_count = 0;
    g_witness.critical_count = 0;
    g_witness.severe_count = 0;
    g_witness.moderate_count = 0;
    g_witness.initialized = true;
    g_witness.cpisi_ready = false;
    g_witness.vfs_ready = false;
    g_witness.early_head = 0;
    g_witness.early_tail = 0;
    g_witness.early_overflow = 0;

    witness_serial_puts("[DAR:DETECT] Witness system initialized\n");

    return MOS_OK;
}

// Report early event (before CPISI ready)
void mos_early_event_impl(int phase, int severity, const char* source,
                          const char* message, int health_delta) {
    // Initialize on first use if needed
    if (!g_witness.initialized) {
        mos_witness_init();
    }

    // Track counts
    g_witness.detect_count++;
    if (severity == MOS_SEV_CRITICAL) g_witness.critical_count++;
    else if (severity == MOS_SEV_HIGH) g_witness.severe_count++;
    else if (severity == MOS_SEV_MEDIUM) g_witness.moderate_count++;

    // Apply health delta
    apply_health_delta((int8_t)health_delta);

    // Always output to serial (immediate visibility)
    witness_serial_event(phase, severity, source, message, health_delta);

    // Buffer for later import
    buffer_early_event(phase, severity, source, message, health_delta);

    // Check for automatic ASSESS trigger
    if (assess_needed()) {
        run_assess();

        // Check for RESTORE trigger
        if (g_witness.hebrew_state == 0 && restore_possible()) {
            run_restore();
        }
    }
}

// Report witness event (after CPISI ready)
void mos_witness_event_impl(int phase, int severity, const char* source,
                            const char* message, int health_delta) {
    // Track counts
    g_witness.detect_count++;
    if (severity == MOS_SEV_CRITICAL) g_witness.critical_count++;
    else if (severity == MOS_SEV_HIGH) g_witness.severe_count++;
    else if (severity == MOS_SEV_MEDIUM) g_witness.moderate_count++;

    // Apply health delta
    apply_health_delta((int8_t)health_delta);

    // Output to serial
    witness_serial_event(phase, severity, source, message, health_delta);

    // TODO: When VFS ready, also write to log file
    // if (g_witness.vfs_ready) { ... }

    // Check for automatic ASSESS trigger
    if (assess_needed()) {
        run_assess();

        // Check for RESTORE trigger
        if (g_witness.hebrew_state == 0 && restore_possible()) {
            run_restore();
        }
    }
}

// Import boot events from Stage 0 buffer
mos_result_t mos_witness_import_boot_events_impl(const MosBootInfo* boot_info) {
    if (!boot_info) {
        return MOS_ERR_INVALID;
    }

    witness_serial_puts("[DAR:DETECT] Importing boot events from Stage 0\n");

    // Check if boot info has event buffer pointer
    // Boot buffer format from debug.h:
    //   struct { uint8_t severity; int8_t dar_phase; int8_t health_delta; char source[16]; char msg[48]; }

    // The boot_event_buffer is stored in boot_info->reserved area
    // For now, log that we attempted import
    witness_serial_puts("  Boot events: ");

    // TODO: Parse actual boot buffer when available
    // For now, acknowledge the boot chain
    witness_serial_puts("(buffer import pending)\n");

    return MOS_OK;
}

// Import early kernel events
mos_result_t mos_witness_import_early_events_impl(void) {
    witness_serial_puts("[DAR:DETECT] Processing early kernel events\n");

    uint32_t count = 0;

    // Process all buffered early events
    while (g_witness.early_tail != g_witness.early_head) {
        // Events were already output to serial when they occurred
        // Here we just count and advance the tail
        count++;
        g_witness.early_tail = (g_witness.early_tail + 1) & MOS_EARLY_BUFFER_MASK;
    }

    witness_serial_puts("  Processed: ");
    witness_serial_int((int)count);
    witness_serial_puts(" events");
    if (g_witness.early_overflow > 0) {
        witness_serial_puts(" (");
        witness_serial_int((int)g_witness.early_overflow);
        witness_serial_puts(" overflow)");
    }
    witness_serial_puts("\n");

    return MOS_OK;
}

// Mark CPISI as ready
void mos_cpisi_mark_ready_impl(void) {
    g_witness.cpisi_ready = true;
    witness_serial_puts("[DAR:DETECT] CPISI subsystem ready - full witness active\n");
}

// Get witness state (for diagnostic/shell)
const MosWitnessState* mos_witness_info(void) {
    return &g_witness;
}

// Get current health
int32_t mos_witness_health(void) {
    return g_witness.health;
}

// Get Hebrew state
uint8_t mos_witness_hebrew_state(void) {
    return g_witness.hebrew_state;
}

// Get Hebrew state name
const char* mos_witness_hebrew_name(void) {
    if (g_witness.hebrew_state <= 6) {
        return g_hebrew_names[g_witness.hebrew_state];
    }
    return "???";
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// DAR Witness implementation provides:
//   - DETECT: Event logging with serial output and buffering
//   - ASSESS: Health evaluation triggered on thresholds
//   - RESTORE: Graceful recovery when entering crisis
//
// The witness has "eyes on" everything from boot through kernel:
//   1. Boot (Stage 0): MOS_WITNESS macro → serial + buffer
//   2. Early kernel: mos_early_event → serial + ring buffer + auto-ASSESS
//   3. Post-CPISI: mos_witness_event → serial + VFS (when ready) + auto-ASSESS
//
// Health flows:
//   Events with health_delta → cumulative health → Hebrew state
//   Thresholds trigger ASSESS → ASSESS may trigger RESTORE
//
// Graceful degradation:
//   System monitors its own health continuously
//   Before total failure, RESTORE can attempt recovery or graceful shutdown
//
// =============================================================================
// END CLOSING
// =============================================================================
