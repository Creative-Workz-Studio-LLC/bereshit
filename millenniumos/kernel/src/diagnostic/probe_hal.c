// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-diagnostic-probe-hal
// Title:       HAL Diagnostic Probe
// Type:        Source
// Component:   Kernel / Diagnostic Subsystem
// Role:        Diagnostic probe for Hardware Abstraction Layer
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Register HAL with diagnostic service for state queries,
//              invariant checks, and visual demonstration.
//
// Grounding:   Genesis 1:6 — "Let there be a firmament"
//              HAL is the firmament - this probe observes its health.
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

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 HAL State Query
// -----------------------------------------------------------------------------

static mos_result_t hal_probe_query(MosDiagState* out) {
    if (!out) return MOS_ERR_INVALID;

    const MosHal* hal = mos_hal_info();
    if (!hal) return MOS_ERR_NODEV;

    // Copy name
    out->name[0] = 'H'; out->name[1] = 'A'; out->name[2] = 'L';
    out->name[3] = '\0';

    // Get health
    out->health = mos_hal_health();

    // Determine cognitive state based on health
    if (out->health > 50) {
        out->state = MOS_STATE_TOV;  // Excellent
    } else if (out->health > 0) {
        out->state = MOS_STATE_SHALEM;  // Good
    } else if (out->health == 0) {
        out->state = MOS_STATE_YASHAR;  // Neutral
    } else if (out->health > -50) {
        out->state = MOS_STATE_CHASER;  // Degraded
    } else {
        out->state = MOS_STATE_SHAVAR;  // Critical
    }

    // Event count
    out->event_count = hal->total_events;
    out->error_count = 0;  // HAL doesn't track errors separately

    // Status message based on state
    switch (out->state) {
        case MOS_STATE_TOV:     out->status_message = "HAL operating excellently"; break;
        case MOS_STATE_SHALEM:  out->status_message = "HAL operating well"; break;
        case MOS_STATE_YASHAR:  out->status_message = "HAL nominal"; break;
        case MOS_STATE_CHASER:  out->status_message = "HAL degraded"; break;
        case MOS_STATE_SHAVAR:  out->status_message = "HAL critical"; break;
        default:                out->status_message = "HAL status unknown"; break;
    }

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.2 HAL Invariant Check
// -----------------------------------------------------------------------------

static mos_result_t hal_probe_check(void) {
    const MosHal* hal = mos_hal_info();
    if (!hal) return MOS_ERR_NODEV;

    mos_size_t errors = 0;

    // Check: Display count should be valid
    if (hal->display_count > MOS_MAX_DISPLAYS) {
        errors++;
    }

    // Check: Primary display should be within count
    if (hal->display_count > 0 && hal->primary_display >= hal->display_count) {
        errors++;
    }

    // Check: If display is active, framebuffer should be non-null
    for (uint8_t i = 0; i < hal->display_count; i++) {
        if (hal->displays[i].active && hal->displays[i].mapped_fb == 0) {
            errors++;
        }
    }

    // Check: Event queue indices should be valid
    if (hal->event_head >= MOS_EVENT_QUEUE_SIZE ||
        hal->event_tail >= MOS_EVENT_QUEUE_SIZE) {
        errors++;
    }

    // Check: Event count should match head/tail
    mos_size_t expected_count;
    if (hal->event_head >= hal->event_tail) {
        expected_count = hal->event_head - hal->event_tail;
    } else {
        expected_count = MOS_EVENT_QUEUE_SIZE - hal->event_tail + hal->event_head;
    }
    if (hal->event_count != expected_count) {
        errors++;
    }

    return (errors == 0) ? MOS_OK : (mos_result_t)(-errors);
}

// -----------------------------------------------------------------------------
// B.3 HAL Visual Demo
// -----------------------------------------------------------------------------

static void hal_probe_demo(MosGraphicsContext* ctx, MosRect bounds) {
    if (!ctx) return;

    const MosHal* hal = mos_hal_info();
    if (!hal) return;

    int32_t x = bounds.x + 20;
    int32_t y = bounds.y + 20;

    // Title
    mos_gfx_string(ctx, x, y, "HAL Subsystem State", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
    y += 30;

    // Display info
    mos_gfx_string(ctx, x, y, "DISPLAYS:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    char buf[80];
    for (uint8_t i = 0; i < hal->display_count; i++) {
        const MosDisplay* disp = &hal->displays[i];

        // Format: "  [0] 1920x1080 32bpp (active)"
        buf[0] = ' '; buf[1] = ' '; buf[2] = '[';
        buf[3] = '0' + i;
        buf[4] = ']'; buf[5] = ' ';

        int pos = 6;
        // Width
        char num[12];
        int w = disp->mode.width;
        int digits = 0;
        do { num[digits++] = '0' + (w % 10); w /= 10; } while (w);
        while (digits > 0) buf[pos++] = num[--digits];
        buf[pos++] = 'x';
        // Height
        int h = disp->mode.height;
        digits = 0;
        do { num[digits++] = '0' + (h % 10); h /= 10; } while (h);
        while (digits > 0) buf[pos++] = num[--digits];
        buf[pos++] = ' ';
        // BPP
        int bpp = disp->mode.bpp;
        digits = 0;
        do { num[digits++] = '0' + (bpp % 10); bpp /= 10; } while (bpp);
        while (digits > 0) buf[pos++] = num[--digits];
        buf[pos++] = 'b'; buf[pos++] = 'p'; buf[pos++] = 'p';

        if (disp->active) {
            buf[pos++] = ' '; buf[pos++] = '(';
            buf[pos++] = 'a'; buf[pos++] = 'c'; buf[pos++] = 't';
            buf[pos++] = 'i'; buf[pos++] = 'v'; buf[pos++] = 'e';
            buf[pos++] = ')';
        }
        buf[pos] = '\0';

        MosColor color = disp->active ? MOS_COLOR_SUCCESS : MOS_COLOR_TEXT_DIM;
        mos_gfx_string(ctx, x, y, buf, color, MOS_COLOR_BG_DARK);
        y += 18;
    }

    y += 10;

    // Input devices
    mos_gfx_string(ctx, x, y, "INPUT DEVICES:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Keyboards
    for (uint8_t i = 0; i < hal->keyboard_count; i++) {
        if (hal->keyboards[i].connected) {
            mos_gfx_string(ctx, x, y, "  Keyboard: ", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
            mos_gfx_string(ctx, x + 100, y, hal->keyboards[i].name,
                           MOS_COLOR_SUCCESS, MOS_COLOR_BG_DARK);
            y += 18;
        }
    }

    // Mice
    for (uint8_t i = 0; i < hal->mouse_count; i++) {
        if (hal->mice[i].connected) {
            mos_gfx_string(ctx, x, y, "  Mouse: ", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
            mos_gfx_string(ctx, x + 100, y, hal->mice[i].name,
                           MOS_COLOR_SUCCESS, MOS_COLOR_BG_DARK);
            y += 18;
        }
    }

    y += 20;

    // Event queue status
    mos_gfx_string(ctx, x, y, "EVENT QUEUE:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Draw queue visualization
    int32_t qx = x;
    int32_t qy = y;
    int32_t qw = 200;
    int32_t qh = 20;

    // Background
    mos_gfx_fill_rect(ctx, (MosRect){qx, qy, qw, qh}, MOS_RGB(40, 40, 40));

    // Fill proportion
    if (MOS_EVENT_QUEUE_SIZE > 0) {
        int32_t fill_w = (int32_t)((hal->event_count * qw) / MOS_EVENT_QUEUE_SIZE);
        if (fill_w > 0) {
            MosColor fill_color = MOS_COLOR_INFO;
            if (hal->event_count > MOS_EVENT_QUEUE_SIZE * 3 / 4) {
                fill_color = MOS_COLOR_WARNING;
            }
            if (hal->event_count >= MOS_EVENT_QUEUE_SIZE - 1) {
                fill_color = MOS_COLOR_ERROR;
            }
            mos_gfx_fill_rect(ctx, (MosRect){qx, qy, fill_w, qh}, fill_color);
        }
    }

    // Border
    mos_gfx_draw_rect(ctx, (MosRect){qx, qy, qw, qh}, MOS_COLOR_TEXT_DIM, 1);

    // Count text
    char count_buf[32];
    int cpos = 0;
    mos_size_t cnt = hal->event_count;
    int cdigits = 0;
    char cnum[12];
    do { cnum[cdigits++] = '0' + (cnt % 10); cnt /= 10; } while (cnt);
    while (cdigits > 0) count_buf[cpos++] = cnum[--cdigits];
    count_buf[cpos++] = '/';
    cnt = MOS_EVENT_QUEUE_SIZE;
    cdigits = 0;
    do { cnum[cdigits++] = '0' + (cnt % 10); cnt /= 10; } while (cnt);
    while (cdigits > 0) count_buf[cpos++] = cnum[--cdigits];
    count_buf[cpos] = '\0';

    mos_gfx_string(ctx, qx + qw + 10, qy + 3, count_buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);

    y += 40;

    // Mouse position visualization
    mos_gfx_string(ctx, x, y, "MOUSE POSITION:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Draw mini display with mouse dot
    int32_t mx = x;
    int32_t my = y;
    int32_t mw = 160;
    int32_t mh = 90;

    mos_gfx_fill_rect(ctx, (MosRect){mx, my, mw, mh}, MOS_RGB(30, 30, 50));
    mos_gfx_draw_rect(ctx, (MosRect){mx, my, mw, mh}, MOS_COLOR_TEXT_DIM, 1);

    // Map mouse position to mini display
    const MosDisplay* primary = mos_hal_primary_display();
    if (primary && primary->active) {
        int32_t dot_x = mx + (hal->mouse_x * mw) / primary->mode.width;
        int32_t dot_y = my + (hal->mouse_y * mh) / primary->mode.height;

        // Draw crosshair
        mos_gfx_hline(ctx, mx, dot_y, mw, MOS_RGB(60, 60, 80));
        mos_gfx_vline(ctx, dot_x, my, mh, MOS_RGB(60, 60, 80));

        // Draw mouse dot
        mos_gfx_fill_circle(ctx, dot_x, dot_y, 4, MOS_COLOR_GOLD);
    }

    // Coordinates text
    char coord_buf[32];
    int ppos = 0;
    coord_buf[ppos++] = '(';
    int mx_val = hal->mouse_x;
    int mdigits = 0;
    char mnum[12];
    if (mx_val < 0) { coord_buf[ppos++] = '-'; mx_val = -mx_val; }
    do { mnum[mdigits++] = '0' + (mx_val % 10); mx_val /= 10; } while (mx_val);
    while (mdigits > 0) coord_buf[ppos++] = mnum[--mdigits];
    coord_buf[ppos++] = ',';
    coord_buf[ppos++] = ' ';
    int my_val = hal->mouse_y;
    mdigits = 0;
    if (my_val < 0) { coord_buf[ppos++] = '-'; my_val = -my_val; }
    do { mnum[mdigits++] = '0' + (my_val % 10); my_val /= 10; } while (my_val);
    while (mdigits > 0) coord_buf[ppos++] = mnum[--mdigits];
    coord_buf[ppos++] = ')';
    coord_buf[ppos] = '\0';

    mos_gfx_string(ctx, mx + mw + 10, my + mh/2 - 8, coord_buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);

    // Footer instructions
    y = bounds.y + bounds.height - 30;
    mos_gfx_string(ctx, x, y, "[N]ext probe  [P]revious probe  [ESC] Exit demo",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
}

// -----------------------------------------------------------------------------
// B.4 Registration
// -----------------------------------------------------------------------------

mos_result_t mos_diag_probe_hal_register(void) {
    return mos_diag_register_probe("HAL",
                                    hal_probe_query,
                                    hal_probe_check,
                                    hal_probe_demo);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// HAL Diagnostic Probe provides:
//   - State query: display info, input devices, event count
//   - Invariant check: valid indices, framebuffer presence
//   - Visual demo: live display/input/event visualization
//
// =============================================================================
// END CLOSING
// =============================================================================
