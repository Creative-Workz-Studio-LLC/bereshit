// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-diagnostic-impl
// Title:       MillenniumOS Diagnostic Service Implementation
// Type:        Source
// Component:   Kernel / Diagnostic Subsystem
// Role:        Assertions, probes, state queries, visual debugging
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Proverbs 27:23 — "Be thou diligent to know the state of thy flocks"
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "diagnostic.h"
#include "hal/hal.h"
#include "graphics/graphics.h"
#include "cpisi/cpisi.h"
#include "trit/shared/palette.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global Diagnostic State
// -----------------------------------------------------------------------------

static MosDiagService g_diag;

// String utilities (no stdlib)
static mos_size_t diag_strlen(const char* s) {
    mos_size_t len = 0;
    while (s && *s++) len++;
    return len;
}

static void diag_strcpy(char* dst, const char* src, mos_size_t max) {
    mos_size_t i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static int diag_strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a - *b;
}

// Integer to string (for assertion messages)
static void diag_itoa(int64_t val, char* buf, mos_size_t size) {
    if (size == 0) return;

    bool neg = val < 0;
    if (neg) val = -val;

    // Build string backwards
    char tmp[24];
    int i = 0;
    do {
        tmp[i++] = '0' + (val % 10);
        val /= 10;
    } while (val && i < 23);

    if (neg && i < 23) tmp[i++] = '-';

    // Reverse into output buffer
    mos_size_t j = 0;
    while (i > 0 && j < size - 1) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}

// -----------------------------------------------------------------------------
// B.2 Assertion Handlers
// -----------------------------------------------------------------------------
//
// These functions dump diagnostic state and halt the system.
// They do NOT return.
//

// Halt the CPU - never returns
__attribute__((noreturn))
static void diag_halt(void) {
    __asm__ volatile("cli");  // Disable interrupts
    for (;;) {
        __asm__ volatile("hlt");  // Halt
    }
}

// Draw assertion failure screen
static void diag_draw_crash_screen(const char* title, const char* details) {
    // Get primary display
    const MosDisplay* disp = mos_hal_display(0);
    if (!disp || !disp->active) {
        diag_halt();
    }

    // Create graphics context
    MosGraphicsContext ctx;
    if (mos_graphics_create_context(&ctx, 0) != MOS_OK) {
        diag_halt();
    }

    // Clear to dark red
    mos_gfx_clear(&ctx, MOS_RGB(80, 0, 0));

    // Draw title banner
    MosRect banner = {0, 0, (int32_t)disp->mode.width, 60};
    mos_gfx_fill_rect(&ctx, banner, MOS_RGB(160, 0, 0));

    // Title text
    mos_gfx_string(&ctx, 20, 22, "ASSERTION FAILED", MOS_COLOR_WHITE, MOS_RGB(160, 0, 0));

    // Details
    int y = 80;
    mos_gfx_string(&ctx, 20, y, title, MOS_COLOR_GOLD, MOS_RGB(80, 0, 0));
    y += 30;

    mos_gfx_string(&ctx, 20, y, details, MOS_COLOR_WHITE, MOS_RGB(80, 0, 0));
    y += 30;

    // Diagnostic state
    y += 20;
    mos_gfx_string(&ctx, 20, y, "System State:", MOS_COLOR_GOLD, MOS_RGB(80, 0, 0));
    y += 20;

    char buf[128];
    diag_strcpy(buf, "  Assertions checked: ", sizeof(buf));
    char num[24];
    diag_itoa(g_diag.assertions_checked, num, sizeof(num));
    // Manual concat
    mos_size_t len = diag_strlen(buf);
    diag_strcpy(buf + len, num, sizeof(buf) - len);
    mos_gfx_string(&ctx, 20, y, buf, MOS_COLOR_TEXT, MOS_RGB(80, 0, 0));
    y += 20;

    // Probe status
    y += 10;
    mos_gfx_string(&ctx, 20, y, "Probe Status:", MOS_COLOR_GOLD, MOS_RGB(80, 0, 0));
    y += 20;

    for (mos_size_t i = 0; i < g_diag.probe_count && y < (int32_t)disp->mode.height - 40; i++) {
        MosDiagProbe* probe = &g_diag.probes[i];
        if (!probe->active) continue;

        // Query probe state
        MosDiagState state;
        if (probe->query && probe->query(&state) == MOS_OK) {
            // Determine color based on health
            MosColor color = MOS_COLOR_TEXT;
            if (state.health > 50) color = MOS_COLOR_SUCCESS;
            else if (state.health > 0) color = MOS_COLOR_WARNING;
            else if (state.health < 0) color = MOS_COLOR_ERROR;

            diag_strcpy(buf, "  ", sizeof(buf));
            diag_strcpy(buf + 2, probe->name, sizeof(buf) - 2);
            mos_size_t l = diag_strlen(buf);
            diag_strcpy(buf + l, ": health=", sizeof(buf) - l);
            l = diag_strlen(buf);
            diag_itoa(state.health, buf + l, sizeof(buf) - l);

            mos_gfx_string(&ctx, 20, y, buf, color, MOS_RGB(80, 0, 0));
            y += 18;
        }
    }

    // Footer
    y = disp->mode.height - 40;
    mos_gfx_string(&ctx, 20, y, "System halted. Reboot required.",
                   MOS_COLOR_TEXT_DIM, MOS_RGB(80, 0, 0));
}

void mos_diag_assertion_failed(const char* file, int line,
                                const char* cond, const char* msg) {
    // Record failure
    g_diag.assertions_failed++;
    g_diag.last_fail_file = file;
    g_diag.last_fail_line = line;
    g_diag.last_fail_msg = msg;

    // Build title: "file:line"
    char title[128];
    diag_strcpy(title, file, sizeof(title));
    mos_size_t len = diag_strlen(title);
    title[len++] = ':';
    diag_itoa(line, title + len, sizeof(title) - len);

    // Build details: "Condition: ... Message: ..."
    char details[256];
    diag_strcpy(details, "Condition: ", sizeof(details));
    len = diag_strlen(details);
    diag_strcpy(details + len, cond, sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, " | ", sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, msg, sizeof(details) - len);

    diag_draw_crash_screen(title, details);
    diag_halt();
}

void mos_diag_assert_eq_failed(const char* file, int line,
                                const char* a_expr, const char* b_expr,
                                int64_t a_val, int64_t b_val, const char* msg) {
    g_diag.assertions_failed++;
    g_diag.last_fail_file = file;
    g_diag.last_fail_line = line;
    g_diag.last_fail_msg = msg;

    char title[128];
    diag_strcpy(title, file, sizeof(title));
    mos_size_t len = diag_strlen(title);
    title[len++] = ':';
    diag_itoa(line, title + len, sizeof(title) - len);

    char details[256];
    diag_strcpy(details, a_expr, sizeof(details));
    len = diag_strlen(details);
    diag_strcpy(details + len, " (", sizeof(details) - len);
    len = diag_strlen(details);
    diag_itoa(a_val, details + len, sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, ") != ", sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, b_expr, sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, " (", sizeof(details) - len);
    len = diag_strlen(details);
    diag_itoa(b_val, details + len, sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, ") | ", sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, msg, sizeof(details) - len);

    diag_draw_crash_screen(title, details);
    diag_halt();
}

void mos_diag_assert_ne_failed(const char* file, int line,
                                const char* a_expr, const char* b_expr,
                                int64_t val, const char* msg) {
    g_diag.assertions_failed++;
    g_diag.last_fail_file = file;
    g_diag.last_fail_line = line;
    g_diag.last_fail_msg = msg;

    char title[128];
    diag_strcpy(title, file, sizeof(title));
    mos_size_t len = diag_strlen(title);
    title[len++] = ':';
    diag_itoa(line, title + len, sizeof(title) - len);

    char details[256];
    diag_strcpy(details, a_expr, sizeof(details));
    len = diag_strlen(details);
    diag_strcpy(details + len, " == ", sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, b_expr, sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, " (both ", sizeof(details) - len);
    len = diag_strlen(details);
    diag_itoa(val, details + len, sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, ") | ", sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, msg, sizeof(details) - len);

    diag_draw_crash_screen(title, details);
    diag_halt();
}

void mos_diag_assert_range_failed(const char* file, int line,
                                   const char* expr, int64_t val,
                                   int64_t min, int64_t max, const char* msg) {
    g_diag.assertions_failed++;
    g_diag.last_fail_file = file;
    g_diag.last_fail_line = line;
    g_diag.last_fail_msg = msg;

    char title[128];
    diag_strcpy(title, file, sizeof(title));
    mos_size_t len = diag_strlen(title);
    title[len++] = ':';
    diag_itoa(line, title + len, sizeof(title) - len);

    char details[256];
    diag_strcpy(details, expr, sizeof(details));
    len = diag_strlen(details);
    diag_strcpy(details + len, "=", sizeof(details) - len);
    len = diag_strlen(details);
    diag_itoa(val, details + len, sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, " not in [", sizeof(details) - len);
    len = diag_strlen(details);
    diag_itoa(min, details + len, sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, ", ", sizeof(details) - len);
    len = diag_strlen(details);
    diag_itoa(max, details + len, sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, "] | ", sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, msg, sizeof(details) - len);

    diag_draw_crash_screen(title, details);
    diag_halt();
}

void mos_diag_assert_null_failed(const char* file, int line,
                                  const char* expr, const char* msg) {
    g_diag.assertions_failed++;
    g_diag.last_fail_file = file;
    g_diag.last_fail_line = line;
    g_diag.last_fail_msg = msg;

    char title[128];
    diag_strcpy(title, file, sizeof(title));
    mos_size_t len = diag_strlen(title);
    title[len++] = ':';
    diag_itoa(line, title + len, sizeof(title) - len);

    char details[256];
    diag_strcpy(details, expr, sizeof(details));
    len = diag_strlen(details);
    diag_strcpy(details + len, " is NULL | ", sizeof(details) - len);
    len = diag_strlen(details);
    diag_strcpy(details + len, msg, sizeof(details) - len);

    diag_draw_crash_screen(title, details);
    diag_halt();
}

// -----------------------------------------------------------------------------
// B.3 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_diag_init(void) {
    // Clear state
    for (mos_size_t i = 0; i < MOS_DIAG_MAX_PROBES; i++) {
        g_diag.probes[i].active = false;
        g_diag.probes[i].query = NULL;
        g_diag.probes[i].check = NULL;
        g_diag.probes[i].demo = NULL;
    }
    g_diag.probe_count = 0;
    g_diag.assertions_checked = 0;
    g_diag.assertions_failed = 0;
    g_diag.last_fail_file = NULL;
    g_diag.last_fail_line = 0;
    g_diag.last_fail_msg = NULL;
    g_diag.system_health = MOS_HEALTH_NEUTRAL;
    g_diag.system_state = MOS_STATE_YASHAR;
    g_diag.demo_active = false;
    g_diag.demo_probe_index = 0;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.4 Probe Registration
// -----------------------------------------------------------------------------

mos_result_t mos_diag_register_probe(const char* name,
                                      MosDiagQueryFn query,
                                      MosDiagCheckFn check,
                                      MosDiagDemoFn demo) {
    if (!name) return MOS_ERR_INVALID;
    if (g_diag.probe_count >= MOS_DIAG_MAX_PROBES) return MOS_ERR_NOMEM;

    // Check for duplicate
    if (mos_diag_find_probe(name)) return MOS_ERR_BUSY;

    // Find free slot
    for (mos_size_t i = 0; i < MOS_DIAG_MAX_PROBES; i++) {
        if (!g_diag.probes[i].active) {
            MosDiagProbe* probe = &g_diag.probes[i];
            diag_strcpy(probe->name, name, MOS_DIAG_NAME_LEN);
            probe->query = query;
            probe->check = check;
            probe->demo = demo;
            probe->active = true;
            probe->last_health = MOS_HEALTH_NEUTRAL;
            probe->query_count = 0;
            g_diag.probe_count++;
            return MOS_OK;
        }
    }

    return MOS_ERR_NOMEM;
}

mos_result_t mos_diag_unregister_probe(const char* name) {
    MosDiagProbe* probe = mos_diag_find_probe(name);
    if (!probe) return MOS_ERR_NOTFOUND;

    probe->active = false;
    g_diag.probe_count--;
    return MOS_OK;
}

MosDiagProbe* mos_diag_find_probe(const char* name) {
    if (!name) return NULL;

    for (mos_size_t i = 0; i < MOS_DIAG_MAX_PROBES; i++) {
        if (g_diag.probes[i].active &&
            diag_strcmp(g_diag.probes[i].name, name) == 0) {
            return &g_diag.probes[i];
        }
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// B.5 State Queries
// -----------------------------------------------------------------------------

mos_result_t mos_diag_query(const char* probe_name, MosDiagState* out) {
    if (!out) return MOS_ERR_INVALID;

    MosDiagProbe* probe = mos_diag_find_probe(probe_name);
    if (!probe) return MOS_ERR_NOTFOUND;
    if (!probe->query) return MOS_ERR_PERM;

    probe->query_count++;
    mos_result_t res = probe->query(out);
    if (res == MOS_OK) {
        probe->last_health = out->health;
    }
    return res;
}

mos_size_t mos_diag_query_all(MosDiagState* out, mos_size_t max_count) {
    if (!out || max_count == 0) return 0;

    mos_size_t count = 0;
    for (mos_size_t i = 0; i < MOS_DIAG_MAX_PROBES && count < max_count; i++) {
        if (g_diag.probes[i].active && g_diag.probes[i].query) {
            if (g_diag.probes[i].query(&out[count]) == MOS_OK) {
                g_diag.probes[i].query_count++;
                g_diag.probes[i].last_health = out[count].health;
                count++;
            }
        }
    }

    // Update aggregate health
    if (count > 0) {
        int32_t total = 0;
        for (mos_size_t i = 0; i < count; i++) {
            total += out[i].health;
        }
        g_diag.system_health = (mos_health_t)(total / (int32_t)count);
    }

    return count;
}

mos_health_t mos_diag_system_health(void) {
    return g_diag.system_health;
}

MosHebrewState mos_diag_system_state(void) {
    return g_diag.system_state;
}

// -----------------------------------------------------------------------------
// B.6 Invariant Checking
// -----------------------------------------------------------------------------

mos_size_t mos_diag_check_all(void) {
    mos_size_t errors = 0;

    for (mos_size_t i = 0; i < MOS_DIAG_MAX_PROBES; i++) {
        if (g_diag.probes[i].active && g_diag.probes[i].check) {
            mos_result_t res = g_diag.probes[i].check();
            if (MOS_FAILED(res)) {
                errors++;
            }
        }
    }

    return errors;
}

mos_result_t mos_diag_check(const char* probe_name) {
    MosDiagProbe* probe = mos_diag_find_probe(probe_name);
    if (!probe) return MOS_ERR_NOTFOUND;
    if (!probe->check) return MOS_ERR_PERM;

    return probe->check();
}

// -----------------------------------------------------------------------------
// B.7 Visual Demos
// -----------------------------------------------------------------------------

mos_result_t mos_diag_demo_start(const char* probe_name) {
    MosDiagProbe* probe = mos_diag_find_probe(probe_name);
    if (!probe) return MOS_ERR_NOTFOUND;
    if (!probe->demo) return MOS_ERR_PERM;

    // Find probe index
    for (mos_size_t i = 0; i < MOS_DIAG_MAX_PROBES; i++) {
        if (&g_diag.probes[i] == probe) {
            g_diag.demo_probe_index = i;
            break;
        }
    }

    g_diag.demo_active = true;
    return MOS_OK;
}

void mos_diag_demo_stop(void) {
    g_diag.demo_active = false;
}

void mos_diag_demo_render(MosGraphicsContext* ctx) {
    if (!g_diag.demo_active || !ctx) return;

    MosDiagProbe* probe = &g_diag.probes[g_diag.demo_probe_index];
    if (!probe->active || !probe->demo) return;

    // Draw demo header
    MosRect header = {0, 0, (int32_t)ctx->width, 40};
    mos_gfx_fill_rect(ctx, header, MOS_COLOR_BG_DARK);
    mos_gfx_string(ctx, 10, 12, "DIAGNOSTIC DEMO: ", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    mos_gfx_string(ctx, 150, 12, probe->name, MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);

    // Demo area
    MosRect bounds = {0, 40, (int32_t)ctx->width, (int32_t)ctx->height - 40};
    probe->demo(ctx, bounds);
}

bool mos_diag_demo_input(const MosEvent* event) {
    if (!g_diag.demo_active || !event) return false;

    if (event->type == MOS_EVENT_KEY && event->key.state == MOS_KEY_DOWN) {
        switch (event->key.keycode) {
            case 27:  // Escape
                mos_diag_demo_stop();
                return true;
            case 'n':
            case 'N':
                mos_diag_demo_next();
                return true;
            case 'p':
            case 'P':
                mos_diag_demo_prev();
                return true;
        }
    }

    return false;
}

void mos_diag_demo_next(void) {
    if (!g_diag.demo_active) return;

    // Find next active probe with demo
    mos_size_t start = g_diag.demo_probe_index;
    mos_size_t i = (start + 1) % MOS_DIAG_MAX_PROBES;

    while (i != start) {
        if (g_diag.probes[i].active && g_diag.probes[i].demo) {
            g_diag.demo_probe_index = i;
            return;
        }
        i = (i + 1) % MOS_DIAG_MAX_PROBES;
    }
}

void mos_diag_demo_prev(void) {
    if (!g_diag.demo_active) return;

    // Find previous active probe with demo
    mos_size_t start = g_diag.demo_probe_index;
    mos_size_t i = (start + MOS_DIAG_MAX_PROBES - 1) % MOS_DIAG_MAX_PROBES;

    while (i != start) {
        if (g_diag.probes[i].active && g_diag.probes[i].demo) {
            g_diag.demo_probe_index = i;
            return;
        }
        i = (i + MOS_DIAG_MAX_PROBES - 1) % MOS_DIAG_MAX_PROBES;
    }
}

// -----------------------------------------------------------------------------
// B.8 CPI-SI Integration
// -----------------------------------------------------------------------------

mos_result_t mos_diag_cpisi_query(const char* query, char* response, mos_size_t size) {
    if (!query || !response || size < 64) return MOS_ERR_INVALID;

    // Simple query parser
    // Supported queries:
    //   "health" - system health
    //   "state" - system state
    //   "probes" - list all probes
    //   "probe:<name>" - specific probe state

    if (diag_strcmp(query, "health") == 0) {
        diag_strcpy(response, "{\"health\":", size);
        mos_size_t len = diag_strlen(response);
        diag_itoa(g_diag.system_health, response + len, size - len);
        len = diag_strlen(response);
        diag_strcpy(response + len, "}", size - len);
        return MOS_OK;
    }

    if (diag_strcmp(query, "state") == 0) {
        diag_strcpy(response, "{\"state\":", size);
        mos_size_t len = diag_strlen(response);
        diag_itoa(g_diag.system_state, response + len, size - len);
        len = diag_strlen(response);
        diag_strcpy(response + len, ",\"health\":", size - len);
        len = diag_strlen(response);
        diag_itoa(g_diag.system_health, response + len, size - len);
        len = diag_strlen(response);
        diag_strcpy(response + len, "}", size - len);
        return MOS_OK;
    }

    if (diag_strcmp(query, "probes") == 0) {
        diag_strcpy(response, "{\"count\":", size);
        mos_size_t len = diag_strlen(response);
        diag_itoa(g_diag.probe_count, response + len, size - len);
        len = diag_strlen(response);
        diag_strcpy(response + len, ",\"probes\":[", size - len);

        bool first = true;
        for (mos_size_t i = 0; i < MOS_DIAG_MAX_PROBES; i++) {
            if (g_diag.probes[i].active) {
                len = diag_strlen(response);
                if (!first) diag_strcpy(response + len, ",", size - len);
                first = false;
                len = diag_strlen(response);
                diag_strcpy(response + len, "\"", size - len);
                len = diag_strlen(response);
                diag_strcpy(response + len, g_diag.probes[i].name, size - len);
                len = diag_strlen(response);
                diag_strcpy(response + len, "\"", size - len);
            }
        }
        len = diag_strlen(response);
        diag_strcpy(response + len, "]}", size - len);
        return MOS_OK;
    }

    return MOS_ERR_INVALID;
}

void mos_diag_log(const char* probe_name, const char* event, mos_health_t health_delta) {
    // Update diagnostic system state based on health delta
    if (health_delta < 0) {
        if (g_diag.system_health > MOS_HEALTH_MIN - health_delta) {
            g_diag.system_health += health_delta;
        } else {
            g_diag.system_health = MOS_HEALTH_MIN;
        }
    } else if (health_delta > 0) {
        if (g_diag.system_health < MOS_HEALTH_MAX - health_delta) {
            g_diag.system_health += health_delta;
        } else {
            g_diag.system_health = MOS_HEALTH_MAX;
        }
    }

    // Feed to CPI-SI Witness system - this is where learning happens
    // DAR phase: DETECT (witnessing the event)
    // Severity based on health delta magnitude
    trit_t severity = MOS_SEV_MILD;
    if (health_delta < -20 || health_delta > 20) {
        severity = MOS_SEV_CRITICAL;
    } else if (health_delta != 0) {
        severity = MOS_SEV_MEDIUM;
    }

    mos_witness_event(MOS_DAR_DETECT, severity, probe_name, event, health_delta);
}

// -----------------------------------------------------------------------------
// B.9 Debug Output
// -----------------------------------------------------------------------------

void mos_diag_print_banner(MosGraphicsContext* ctx, int32_t x, int32_t y) {
    if (!ctx) return;

    char buf[64];
    diag_strcpy(buf, "Diagnostic Service | Probes: ", sizeof(buf));
    mos_size_t len = diag_strlen(buf);
    diag_itoa(g_diag.probe_count, buf + len, sizeof(buf) - len);
    len = diag_strlen(buf);
    diag_strcpy(buf + len, " | Health: ", sizeof(buf) - len);
    len = diag_strlen(buf);
    diag_itoa(g_diag.system_health, buf + len, sizeof(buf) - len);

    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
}

void mos_diag_print_status(MosGraphicsContext* ctx, int32_t x, int32_t y) {
    if (!ctx) return;

    int32_t row_y = y;

    for (mos_size_t i = 0; i < MOS_DIAG_MAX_PROBES; i++) {
        if (!g_diag.probes[i].active) continue;

        MosDiagProbe* probe = &g_diag.probes[i];

        // Determine color
        MosColor color = MOS_COLOR_TEXT;
        if (probe->last_health > 50) color = MOS_COLOR_SUCCESS;
        else if (probe->last_health > 0) color = MOS_COLOR_INFO;
        else if (probe->last_health < 0) color = MOS_COLOR_ERROR;

        // Build status line
        char buf[80];
        diag_strcpy(buf, probe->name, sizeof(buf));
        mos_size_t len = diag_strlen(buf);
        // Pad to column
        while (len < 20) buf[len++] = ' ';
        buf[len] = '\0';
        diag_itoa(probe->last_health, buf + len, sizeof(buf) - len);

        mos_gfx_string(ctx, x, row_y, buf, color, MOS_COLOR_BG_DARK);
        row_y += 18;
    }
}

void mos_diag_dump(void) {
    // Full state dump - would write to crash log in real implementation
    // For now, draws to screen
    const MosDisplay* disp = mos_hal_display(0);
    if (!disp || !disp->active) return;

    MosGraphicsContext ctx;
    if (mos_graphics_create_context(&ctx, 0) != MOS_OK) return;

    mos_gfx_clear(&ctx, MOS_COLOR_BG_DARK);
    mos_diag_print_banner(&ctx, 10, 10);
    mos_diag_print_status(&ctx, 10, 40);
}

// -----------------------------------------------------------------------------
// B.10 Service Access
// -----------------------------------------------------------------------------

const MosDiagService* mos_diag_info(void) {
    return &g_diag;
}

// -----------------------------------------------------------------------------
// B.11 Bulk Probe Registration
// -----------------------------------------------------------------------------

mos_result_t mos_diag_register_all_probes(void) {
    mos_result_t result;

    // HAL probe - displays, input, events
    result = mos_diag_probe_hal_register();
    if (MOS_FAILED(result)) return result;

    // Graphics probe - rendering capabilities
    result = mos_diag_probe_gfx_register();
    if (MOS_FAILED(result)) return result;

    // Memory probe - frame allocation
    result = mos_diag_probe_memory_register();
    if (MOS_FAILED(result)) return result;

    // VFS probe - filesystem state
    result = mos_diag_probe_vfs_register();
    if (MOS_FAILED(result)) return result;

    // CPI-SI probe - cognitive state
    result = mos_diag_probe_cpisi_register();
    if (MOS_FAILED(result)) return result;

    // Witness probe - learning state (CPI-SI consuming DAR)
    result = mos_diag_probe_witness_register();
    if (MOS_FAILED(result)) return result;

    return MOS_OK;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Diagnostic Service Implementation provides:
//   - Assertion handlers with crash screen
//   - Probe registration and querying
//   - Visual demo framework
//   - CPI-SI query interface
//   - Debug output utilities
//
// =============================================================================
// END CLOSING
// =============================================================================
