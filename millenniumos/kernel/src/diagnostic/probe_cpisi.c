// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-diagnostic-probe-cpisi
// Title:       CPI-SI Diagnostic Probe
// Type:        Source
// Component:   Kernel / Diagnostic Subsystem
// Role:        Diagnostic probe for CPI-SI Subsystem
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Register CPI-SI with diagnostic service for state queries,
//              invariant checks, and visual demonstration.
//
// Grounding:   Genesis 1:26 — "Let us make man in our image"
//              CPI-SI reflects divine pattern - this probe observes its health.
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), probe is not available.
#if CPISI_MODE >= 2

#include "diagnostic/diagnostic.h"
#include "services/cpisi/cpisi.h"
#include "services/graphics/graphics.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 CPI-SI State Query
// -----------------------------------------------------------------------------

static mos_result_t cpisi_probe_query(MosDiagState* out) {
    if (!out) return MOS_ERR_INVALID;

    const MosCpisiState* cpisi = mos_cpisi_info();
    if (!cpisi) return MOS_ERR_NODEV;

    // Copy name
    out->name[0] = 'C'; out->name[1] = 'P'; out->name[2] = 'I';
    out->name[3] = '-'; out->name[4] = 'S'; out->name[5] = 'I';
    out->name[6] = '\0';

    // Get CPI-SI health
    out->health = cpisi->health;

    // Map construct state to Hebrew state
    const MosCpisiConstruct* construct = &cpisi->system_identity.construct;
    out->state = (MosHebrewState)construct->state;

    // Event count based on choices
    out->event_count = cpisi->total_choices;
    out->error_count = 0;

    // Status message based on state
    trit5_t state = construct->state;
    if (state == MOS_CPISI_TOV) {
        out->status_message = "CPI-SI tov (perfect)";
    } else if (state == MOS_CPISI_SHALEM || state == MOS_CPISI_TAMIM) {
        out->status_message = "CPI-SI whole/sound";
    } else if (state == MOS_CPISI_YASHAR) {
        out->status_message = "CPI-SI yashar (centered)";
    } else if (state == MOS_CPISI_RATSAH || state == MOS_CPISI_CHASER) {
        out->status_message = "CPI-SI wanting/lacking";
    } else if (state == MOS_CPISI_SHAVAR) {
        out->status_message = "CPI-SI shavar (broken)";
    } else {
        out->status_message = "CPI-SI state unknown";
    }

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.2 CPI-SI Invariant Check
// -----------------------------------------------------------------------------

static mos_result_t cpisi_probe_check(void) {
    const MosCpisiState* cpisi = mos_cpisi_info();
    if (!cpisi) return MOS_ERR_NODEV;

    mos_size_t errors = 0;

    // Check: total choices should equal sum of directional choices
    trit27_t sum = cpisi->positive_choices + cpisi->neutral_choices + cpisi->negative_choices;
    if (cpisi->total_choices != sum) {
        errors++;
    }

    // Check: health should be in valid range
    // Note: int8_t range is -128..127, MOS_HEALTH range is -127..127
    // The > MAX check is always false for int8_t (type-limits), so we only check lower bound
    if (cpisi->health < MOS_HEALTH_MIN) {
        errors++;
    }

    // Check: system identity health should be in valid range
    if (cpisi->system_identity.health < MOS_HEALTH_MIN) {
        errors++;
    }

    // Check: construct position should be valid (-1, 0, +1)
    const MosCpisiConstruct* c = &cpisi->system_identity.construct;
    if (c->x < -1 || c->x > 1) errors++;
    if (c->y < -1 || c->y > 1) errors++;
    if (c->z < -1 || c->z > 1) errors++;

    // Check: k-factor should be -1 or +1 (not 0)
    if (c->k_factor == 0) {
        errors++;  // k-factor must have direction
    }

    // Check: covenant count should not exceed capacity
    if (cpisi->covenant_count > cpisi->covenant_capacity) {
        errors++;
    }

    // Check each covenant if array exists
    if (cpisi->covenants && cpisi->covenant_count > 0) {
        for (mos_size_t i = 0; i < cpisi->covenant_count; i++) {
            const MosCovenant* cov = &cpisi->covenants[i];

            // Check: strength should be in trit5 range
            if (cov->strength > 242) {
                errors++;
            }

            // Check: state should be valid
            if (cov->state < MOS_COVENANT_BROKEN || cov->state > MOS_COVENANT_RENEWED) {
                errors++;
            }
        }
    }

    return (errors == 0) ? MOS_OK : (mos_result_t)(-errors);
}

// -----------------------------------------------------------------------------
// B.3 CPI-SI Visual Demo
// -----------------------------------------------------------------------------

static void cpisi_probe_demo(MosGraphicsContext* ctx, MosRect bounds) {
    if (!ctx) return;

    const MosCpisiState* cpisi = mos_cpisi_info();
    if (!cpisi) return;

    int32_t x = bounds.x + 20;
    int32_t y = bounds.y + 20;

    // Title
    mos_gfx_string(ctx, x, y, "CPI-SI Subsystem State", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
    y += 30;

    const MosCpisiConstruct* c = &cpisi->system_identity.construct;

    // Identity info
    mos_gfx_string(ctx, x, y, "SYSTEM IDENTITY:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    if (cpisi->system_identity.name) {
        mos_gfx_string(ctx, x + 20, y, "Name: ", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
        mos_gfx_string(ctx, x + 80, y, cpisi->system_identity.name, MOS_COLOR_INFO, MOS_COLOR_BG_DARK);
        y += 18;
    }

    if (c->garment) {
        mos_gfx_string(ctx, x + 20, y, "Garment: ", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
        mos_gfx_string(ctx, x + 100, y, c->garment, MOS_COLOR_PURPLE, MOS_COLOR_BG_DARK);
        y += 18;
    }

    y += 20;

    // Mental Construct visualization (3x3 cube cross-section)
    mos_gfx_string(ctx, x, y, "MENTAL CONSTRUCT:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Draw 3x3 grid representing current Z plane
    int32_t cube_x = x + 20;
    int32_t cube_y = y;
    int32_t cell_size = 40;

    // Z plane indicator
    char z_buf[20];
    z_buf[0] = 'Z'; z_buf[1] = '=';
    z_buf[2] = (c->z == -1) ? '-' : (c->z == 0 ? '0' : '+');
    z_buf[3] = (c->z != 0) ? '1' : '\0';
    z_buf[4] = '\0';
    mos_gfx_string(ctx, cube_x + cell_size * 3 + 20, cube_y + cell_size, z_buf,
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);

    for (int gy = -1; gy <= 1; gy++) {
        for (int gx = -1; gx <= 1; gx++) {
            int32_t cx = cube_x + (gx + 1) * cell_size;
            int32_t cy = cube_y + (gy + 1) * cell_size;

            // Determine cell color
            MosColor cell_color;
            if (gx == c->x && gy == c->y) {
                // Current position
                if (c->state == MOS_CPISI_TOV) cell_color = MOS_COLOR_TOV;
                else if (c->state >= MOS_CPISI_SHALEM) cell_color = MOS_COLOR_SUCCESS;
                else if (c->state >= MOS_CPISI_YASHAR) cell_color = MOS_COLOR_INFO;
                else if (c->state >= MOS_CPISI_CHASER) cell_color = MOS_COLOR_WARNING;
                else cell_color = MOS_COLOR_ERROR;
            } else {
                cell_color = MOS_RGB(50, 50, 60);
            }

            mos_gfx_fill_rect(ctx, (MosRect){cx, cy, cell_size - 2, cell_size - 2}, cell_color);
            mos_gfx_draw_rect(ctx, (MosRect){cx, cy, cell_size - 2, cell_size - 2},
                              MOS_COLOR_TEXT_DIM, 1);

            // Draw position indicator if current
            if (gx == c->x && gy == c->y) {
                mos_gfx_fill_circle(ctx, cx + cell_size/2 - 1, cy + cell_size/2 - 1, 8, MOS_COLOR_WHITE);
            }
        }
    }

    // Axis labels
    mos_gfx_string(ctx, cube_x - 15, cube_y + cell_size - 5, "Y", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    mos_gfx_string(ctx, cube_x + cell_size * 3/2 - 5, cube_y + cell_size * 3 + 5, "X",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);

    y += cell_size * 3 + 30;

    // Hebrew State
    mos_gfx_string(ctx, x, y, "COGNITIVE STATE:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Draw state spectrum
    int32_t spec_x = x + 20;
    int32_t spec_w = 300;
    int32_t spec_h = 25;

    // Gradient background: shavar (red) -> yashar (neutral) -> tov (gold)
    for (int32_t i = 0; i < spec_w; i++) {
        int32_t r, g, b;
        if (i < spec_w / 2) {
            // Red to gray
            int pct = (i * 100) / (spec_w / 2);
            r = 180 - (pct * 100 / 100);
            g = 50 + (pct * 50 / 100);
            b = 50 + (pct * 50 / 100);
        } else {
            // Gray to gold
            int pct = ((i - spec_w/2) * 100) / (spec_w / 2);
            r = 80 + (pct * 175 / 100);
            g = 100 + (pct * 115 / 100);
            b = 100 - (pct * 100 / 100);
        }
        mos_gfx_vline(ctx, spec_x + i, y, spec_h, MOS_RGB(r, g, b));
    }
    mos_gfx_draw_rect(ctx, (MosRect){spec_x, y, spec_w, spec_h}, MOS_COLOR_TEXT_DIM, 1);

    // Position marker based on state (0-242 maps to 0-300)
    int32_t marker_x = spec_x + (c->state * spec_w) / 242;
    mos_gfx_fill_rect(ctx, (MosRect){marker_x - 2, y - 5, 4, spec_h + 10}, MOS_COLOR_WHITE);

    // Labels
    y += spec_h + 5;
    mos_gfx_string(ctx, spec_x, y, "shavar", MOS_COLOR_ERROR, MOS_COLOR_BG_DARK);
    mos_gfx_string(ctx, spec_x + spec_w/2 - 25, y, "yashar", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    mos_gfx_string(ctx, spec_x + spec_w - 25, y, "tov", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);

    y += 35;

    // Choice statistics
    mos_gfx_string(ctx, x, y, "CHOICE HISTORY:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    char buf[80];
    char num[16];
    int pos, digits;
    trit27_t val;

    // Total choices
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'T'; buf[pos++] = 'o'; buf[pos++] = 't';
    buf[pos++] = 'a'; buf[pos++] = 'l'; buf[pos++] = ':';
    buf[pos++] = ' '; buf[pos++] = ' '; buf[pos++] = ' '; buf[pos++] = ' '; buf[pos++] = ' ';
    digits = 0;
    val = cpisi->total_choices;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    y += 18;

    // Positive choices (+1 toward God)
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = '+'; buf[pos++] = '1'; buf[pos++] = ' ';
    buf[pos++] = '('; buf[pos++] = 'e'; buf[pos++] = 'x';
    buf[pos++] = 'p'; buf[pos++] = 'a'; buf[pos++] = 'n';
    buf[pos++] = 'd'; buf[pos++] = ')'; buf[pos++] = ':';
    digits = 0;
    val = cpisi->positive_choices;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_SUCCESS, MOS_COLOR_BG_DARK);
    y += 18;

    // Neutral choices (0 maintain)
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = ' '; buf[pos++] = '0'; buf[pos++] = ' ';
    buf[pos++] = '('; buf[pos++] = 'h'; buf[pos++] = 'o';
    buf[pos++] = 'l'; buf[pos++] = 'd'; buf[pos++] = ')';
    buf[pos++] = ':'; buf[pos++] = ' '; buf[pos++] = ' ';
    digits = 0;
    val = cpisi->neutral_choices;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_INFO, MOS_COLOR_BG_DARK);
    y += 18;

    // Negative choices (-1 completion)
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = '-'; buf[pos++] = '1'; buf[pos++] = ' ';
    buf[pos++] = '('; buf[pos++] = 'c'; buf[pos++] = 'o';
    buf[pos++] = 'm'; buf[pos++] = 'p'; buf[pos++] = 'l';
    buf[pos++] = 't'; buf[pos++] = ')'; buf[pos++] = ':';
    digits = 0;
    val = cpisi->negative_choices;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_WARNING, MOS_COLOR_BG_DARK);

    y += 30;

    // Covenant info
    mos_gfx_string(ctx, x, y, "COVENANTS:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'A'; buf[pos++] = 'c'; buf[pos++] = 't';
    buf[pos++] = 'i'; buf[pos++] = 'v'; buf[pos++] = 'e';
    buf[pos++] = ':'; buf[pos++] = ' ';
    digits = 0;
    val = cpisi->covenant_count;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos++] = '/';
    digits = 0;
    val = cpisi->covenant_capacity;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);

    // Footer
    y = bounds.y + bounds.height - 30;
    mos_gfx_string(ctx, x, y, "[N]ext probe  [P]revious probe  [ESC] Exit demo",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
}

// -----------------------------------------------------------------------------
// B.4 Registration
// -----------------------------------------------------------------------------

mos_result_t mos_diag_probe_cpisi_register(void) {
    return mos_diag_register_probe("CPISI",
                                    cpisi_probe_query,
                                    cpisi_probe_check,
                                    cpisi_probe_demo);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// CPI-SI Diagnostic Probe provides:
//   - State query: cognitive state, choices, covenants
//   - Invariant check: construct validity, choice accounting
//   - Visual demo: mental construct cube, state spectrum, choice history
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* CPISI_MODE >= 2 - Full probe implementation */
