// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-diagnostic-probe-gfx
// Title:       Graphics Diagnostic Probe
// Type:        Source
// Component:   Kernel / Diagnostic Subsystem
// Role:        Diagnostic probe for Graphics Subsystem
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Register Graphics with diagnostic service for state queries,
//              invariant checks, and visual demonstration.
//
// Grounding:   Genesis 1:16 — "And God made two great lights"
//              Graphics brings light - this probe observes its state.
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
// B.1 Graphics State Query
// -----------------------------------------------------------------------------

static mos_result_t gfx_probe_query(MosDiagState* out) {
    if (!out) return MOS_ERR_INVALID;

    const MosHal* hal = mos_hal_info();
    if (!hal) return MOS_ERR_NODEV;

    // Copy name
    out->name[0] = 'G'; out->name[1] = 'F'; out->name[2] = 'X';
    out->name[3] = '\0';

    // Graphics health based on display state
    int32_t health = MOS_HEALTH_NEUTRAL;
    mos_size_t active_displays = 0;

    for (uint8_t i = 0; i < hal->display_count; i++) {
        if (hal->displays[i].active) {
            active_displays++;
            // Good resolution = health boost
            if (hal->displays[i].mode.width >= 1920 &&
                hal->displays[i].mode.height >= 1080) {
                health += 20;
            } else if (hal->displays[i].mode.width >= 1280) {
                health += 10;
            }
            // 32bpp = optimal
            if (hal->displays[i].mode.bpp == 32) {
                health += 10;
            }
        }
    }

    // No active displays = critical
    if (active_displays == 0) {
        health = MOS_HEALTH_MIN;
    }

    // Clamp to valid range
    if (health > MOS_HEALTH_MAX) health = MOS_HEALTH_MAX;
    if (health < MOS_HEALTH_MIN) health = MOS_HEALTH_MIN;

    out->health = (mos_health_t)health;

    // Determine cognitive state
    if (health > 50) {
        out->state = MOS_STATE_TOV;
    } else if (health > 0) {
        out->state = MOS_STATE_SHALEM;
    } else if (health == 0) {
        out->state = MOS_STATE_YASHAR;
    } else if (health > -50) {
        out->state = MOS_STATE_CHASER;
    } else {
        out->state = MOS_STATE_SHAVAR;
    }

    out->event_count = active_displays;
    out->error_count = 0;

    // Status message
    if (active_displays == 0) {
        out->status_message = "No active displays";
    } else if (health > 50) {
        out->status_message = "Graphics excellent";
    } else if (health > 0) {
        out->status_message = "Graphics operational";
    } else {
        out->status_message = "Graphics degraded";
    }

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.2 Graphics Invariant Check
// -----------------------------------------------------------------------------

static mos_result_t gfx_probe_check(void) {
    const MosHal* hal = mos_hal_info();
    if (!hal) return MOS_ERR_NODEV;

    mos_size_t errors = 0;

    for (uint8_t i = 0; i < hal->display_count; i++) {
        const MosDisplay* disp = &hal->displays[i];

        if (!disp->active) continue;

        // Check: width must be positive and reasonable
        if (disp->mode.width == 0 || disp->mode.width > 8192) {
            errors++;
        }

        // Check: height must be positive and reasonable
        if (disp->mode.height == 0 || disp->mode.height > 8192) {
            errors++;
        }

        // Check: bpp must be 8, 16, 24, or 32
        if (disp->mode.bpp != 8 && disp->mode.bpp != 16 &&
            disp->mode.bpp != 24 && disp->mode.bpp != 32) {
            errors++;
        }

        // Check: pitch should be at least width * bytes_per_pixel
        uint32_t min_pitch = disp->mode.width * (disp->mode.bpp / 8);
        if (disp->mode.pitch < min_pitch) {
            errors++;
        }

        // Check: framebuffer must be mapped if active
        if (disp->mapped_fb == 0) {
            errors++;
        }
    }

    return (errors == 0) ? MOS_OK : (mos_result_t)(-errors);
}

// -----------------------------------------------------------------------------
// B.3 Graphics Visual Demo
// -----------------------------------------------------------------------------

static void gfx_probe_demo(MosGraphicsContext* ctx, MosRect bounds) {
    if (!ctx) return;

    const MosHal* hal = mos_hal_info();
    if (!hal) return;

    int32_t x = bounds.x + 20;
    int32_t y = bounds.y + 20;

    // Title
    mos_gfx_string(ctx, x, y, "Graphics Subsystem Demo", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
    y += 30;

    // Color palette demo
    mos_gfx_string(ctx, x, y, "Kingdom Palette:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Draw color swatches
    int32_t swatch_size = 40;
    int32_t swatch_x = x;

    struct {
        MosColor color;
        const char* name;
    } swatches[] = {
        {MOS_COLOR_VOID, "void"},
        {MOS_COLOR_WORD, "word"},
        {MOS_COLOR_TOV, "tov"},
        {MOS_COLOR_GOLD, "gold"},
        {MOS_COLOR_PURPLE, "purple"},
        {MOS_COLOR_SUCCESS, "success"},
        {MOS_COLOR_WARNING, "warning"},
        {MOS_COLOR_ERROR, "error"},
    };

    for (int i = 0; i < 8; i++) {
        mos_gfx_fill_rect(ctx, (MosRect){swatch_x, y, swatch_size, swatch_size}, swatches[i].color);
        mos_gfx_draw_rect(ctx, (MosRect){swatch_x, y, swatch_size, swatch_size}, MOS_COLOR_TEXT_DIM, 1);
        mos_gfx_string(ctx, swatch_x, y + swatch_size + 4, swatches[i].name,
                       MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
        swatch_x += swatch_size + 20;
    }

    y += swatch_size + 30;

    // Primitives demo
    y += 20;
    mos_gfx_string(ctx, x, y, "Drawing Primitives:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 25;

    // Lines
    int32_t demo_x = x;
    for (int i = 0; i < 5; i++) {
        MosColor c = MOS_RGB(255 - i * 40, 100 + i * 30, i * 50);
        mos_gfx_line(ctx, demo_x, y, demo_x + 60, y + 40, c);
        demo_x += 20;
    }

    // Rectangles
    demo_x = x + 150;
    mos_gfx_fill_rect(ctx, (MosRect){demo_x, y, 60, 40}, MOS_COLOR_INFO);
    mos_gfx_draw_rect(ctx, (MosRect){demo_x + 70, y, 60, 40}, MOS_COLOR_ACCENT, 2);

    // Circles
    demo_x = x + 310;
    mos_gfx_fill_circle(ctx, demo_x + 20, y + 20, 20, MOS_COLOR_SUCCESS);
    mos_gfx_draw_circle(ctx, demo_x + 70, y + 20, 20, MOS_COLOR_WARNING);

    y += 60;

    // Text demo
    y += 20;
    mos_gfx_string(ctx, x, y, "Font Rendering:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 25;

    mos_gfx_string(ctx, x, y, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    y += 20;
    mos_gfx_string(ctx, x, y, "abcdefghijklmnopqrstuvwxyz", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    y += 20;
    mos_gfx_string(ctx, x, y, "0123456789 !@#$%^&*()-=+[]{}|;':\",./<>?",
                   MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);

    y += 40;

    // Gradient demo
    mos_gfx_string(ctx, x, y, "Gradient:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;
    mos_gfx_gradient_rect(ctx, (MosRect){x, y, 200, 30}, MOS_COLOR_VOID, MOS_COLOR_TOV);
    mos_gfx_draw_rect(ctx, (MosRect){x, y, 200, 30}, MOS_COLOR_TEXT_DIM, 1);

    // Footer
    y = bounds.y + bounds.height - 30;
    mos_gfx_string(ctx, x, y, "[N]ext probe  [P]revious probe  [ESC] Exit demo",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
}

// -----------------------------------------------------------------------------
// B.4 Registration
// -----------------------------------------------------------------------------

mos_result_t mos_diag_probe_gfx_register(void) {
    return mos_diag_register_probe("GFX",
                                    gfx_probe_query,
                                    gfx_probe_check,
                                    gfx_probe_demo);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Graphics Diagnostic Probe provides:
//   - State query: display info, color depth, resolution
//   - Invariant check: valid dimensions, mapped framebuffer
//   - Visual demo: color palette, primitives, font rendering
//
// =============================================================================
// END CLOSING
// =============================================================================
