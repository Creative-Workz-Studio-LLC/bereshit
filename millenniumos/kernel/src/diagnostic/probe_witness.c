// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-diagnostic-probe-witness
// Title:       Witness Diagnostic Probe
// Type:        Source
// Component:   Kernel / Diagnostic Subsystem
// Role:        Diagnostic probe for CPI-SI Witness (learning system)
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Register Witness with diagnostic service for state queries,
//              invariant checks, and visual demonstration.
//
// Grounding:   Lamentations 3:40 — "Let us search and try our ways"
//              The witness searches (DETECT), tries (ASSESS), and turns (RESTORE).
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
// B.1 Witness State Query
// -----------------------------------------------------------------------------

static mos_result_t witness_probe_query(MosDiagState* out) {
    if (!out) return MOS_ERR_INVALID;

    const MosWitnessState* witness = mos_witness_info();
    if (!witness) return MOS_ERR_NODEV;

    // Copy name
    out->name[0] = 'W'; out->name[1] = 'I'; out->name[2] = 'T';
    out->name[3] = 'N'; out->name[4] = 'E'; out->name[5] = 'S';
    out->name[6] = 'S'; out->name[7] = '\0';

    // Health based on understanding level and learning direction
    int32_t health = (int32_t)witness->understanding_level - 121;  // Center is 0
    if (witness->learning_direction == TRIT_POS) {
        health += 10;  // Growing = healthier
    } else if (witness->learning_direction == TRIT_NEG) {
        health -= 10;  // Consolidating = slightly less active
    }

    // Bonus for patterns and wisdom
    health += (int32_t)witness->pattern_count * 2;
    health += (int32_t)witness->wisdom_count * 5;

    // Clamp
    if (health > MOS_HEALTH_MAX) health = MOS_HEALTH_MAX;
    if (health < MOS_HEALTH_MIN) health = MOS_HEALTH_MIN;
    out->health = (mos_health_t)health;

    // Map understanding level to Hebrew state
    if (witness->understanding_level > 200) {
        out->state = MOS_STATE_TOV;
    } else if (witness->understanding_level > 160) {
        out->state = MOS_STATE_SHALEM;
    } else if (witness->understanding_level > 121) {
        out->state = MOS_STATE_TAMIM;
    } else if (witness->understanding_level == 121) {
        out->state = MOS_STATE_YASHAR;
    } else if (witness->understanding_level > 80) {
        out->state = MOS_STATE_RATSAH;
    } else if (witness->understanding_level > 40) {
        out->state = MOS_STATE_CHASER;
    } else {
        out->state = MOS_STATE_SHAVAR;
    }

    out->event_count = witness->total_witnessed;
    out->error_count = 0;

    // Status message
    if (witness->wisdom_count > 10) {
        out->status_message = "Witness: deep understanding";
    } else if (witness->pattern_count > 5) {
        out->status_message = "Witness: recognizing patterns";
    } else if (witness->event_count > 50) {
        out->status_message = "Witness: actively learning";
    } else {
        out->status_message = "Witness: beginning to observe";
    }

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.2 Witness Invariant Check
// -----------------------------------------------------------------------------

static mos_result_t witness_probe_check(void) {
    const MosWitnessState* witness = mos_witness_info();
    if (!witness) return MOS_ERR_NODEV;

    mos_size_t errors = 0;

    // Check: event_count should not exceed buffer size
    if (witness->event_count > MOS_MAX_WITNESS_EVENTS) {
        errors++;
    }

    // Check: event_head should be within bounds
    if (witness->event_head >= MOS_MAX_WITNESS_EVENTS) {
        errors++;
    }

    // Check: pattern_count should not exceed max
    if (witness->pattern_count > MOS_MAX_PATTERNS) {
        errors++;
    }

    // Check: wisdom_count should not exceed max
    if (witness->wisdom_count > MOS_MAX_WISDOM) {
        errors++;
    }

    // Check: understanding_level should be valid trit5
    if (witness->understanding_level > 242) {
        errors++;
    }

    // Check: learning_direction should be valid trit
    if (witness->learning_direction < -1 || witness->learning_direction > 1) {
        errors++;
    }

    // Check patterns have valid confidence
    for (mos_size_t i = 0; i < witness->pattern_count; i++) {
        if (witness->patterns[i].confidence > 242) {
            errors++;
        }
    }

    // Check wisdom has valid depth
    for (mos_size_t i = 0; i < witness->wisdom_count; i++) {
        if (witness->wisdom[i].depth > 242) {
            errors++;
        }
    }

    return (errors == 0) ? MOS_OK : (mos_result_t)(-errors);
}

// -----------------------------------------------------------------------------
// B.3 Witness Visual Demo
// -----------------------------------------------------------------------------

static void witness_probe_demo(MosGraphicsContext* ctx, MosRect bounds) {
    if (!ctx) return;

    const MosWitnessState* witness = mos_witness_info();
    if (!witness) return;

    int32_t x = bounds.x + 20;
    int32_t y = bounds.y + 20;

    // Title
    mos_gfx_string(ctx, x, y, "CPI-SI Witness (Learning System)", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
    y += 30;

    char buf[80];
    char num[16];
    int pos, digits;
    trit27_t val;

    // Understanding level visualization
    mos_gfx_string(ctx, x, y, "UNDERSTANDING LEVEL:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Draw understanding bar (0-242 scale)
    int32_t bar_x = x;
    int32_t bar_w = 242;
    int32_t bar_h = 20;

    mos_gfx_fill_rect(ctx, (MosRect){bar_x, y, bar_w, bar_h}, MOS_RGB(40, 40, 50));
    mos_gfx_fill_rect(ctx, (MosRect){bar_x, y, witness->understanding_level, bar_h},
                      MOS_COLOR_PURPLE);
    mos_gfx_draw_rect(ctx, (MosRect){bar_x, y, bar_w, bar_h}, MOS_COLOR_TEXT_DIM, 1);

    // Center marker (yashar = 121)
    mos_gfx_vline(ctx, bar_x + 121, y, bar_h, MOS_COLOR_GOLD);

    // Level text
    pos = 0;
    digits = 0;
    val = witness->understanding_level;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos++] = '/'; buf[pos++] = '2'; buf[pos++] = '4'; buf[pos++] = '2';
    buf[pos] = '\0';
    mos_gfx_string(ctx, bar_x + bar_w + 10, y + 3, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);

    y += bar_h + 15;

    // Learning direction
    mos_gfx_string(ctx, x, y, "Direction: ", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    if (witness->learning_direction == TRIT_POS) {
        mos_gfx_string(ctx, x + 90, y, "+1 GROWING", MOS_COLOR_SUCCESS, MOS_COLOR_BG_DARK);
    } else if (witness->learning_direction == TRIT_NEG) {
        mos_gfx_string(ctx, x + 90, y, "-1 CONSOLIDATING", MOS_COLOR_INFO, MOS_COLOR_BG_DARK);
    } else {
        mos_gfx_string(ctx, x + 90, y, " 0 STABLE", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    }
    y += 30;

    // Event statistics
    mos_gfx_string(ctx, x, y, "WITNESSED EVENTS:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'T'; buf[pos++] = 'o'; buf[pos++] = 't'; buf[pos++] = 'a';
    buf[pos++] = 'l'; buf[pos++] = ':'; buf[pos++] = ' ';
    digits = 0;
    val = witness->total_witnessed;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos++] = ' '; buf[pos++] = '|'; buf[pos++] = ' ';
    buf[pos++] = 'B'; buf[pos++] = 'u'; buf[pos++] = 'f'; buf[pos++] = ':';
    buf[pos++] = ' ';
    digits = 0;
    val = witness->event_count;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    if (digits == 0) num[digits++] = '0';
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos++] = '/';
    digits = 0;
    val = MOS_MAX_WITNESS_EVENTS;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    y += 30;

    // Patterns learned
    mos_gfx_string(ctx, x, y, "PATTERNS RECOGNIZED:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    if (witness->pattern_count == 0) {
        mos_gfx_string(ctx, x + 20, y, "(still learning...)", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
        y += 18;
    } else {
        for (mos_size_t i = 0; i < witness->pattern_count && i < 5; i++) {
            const MosWitnessPattern* p = &witness->patterns[i];
            if (!p->name) continue;

            pos = 0;
            buf[pos++] = ' '; buf[pos++] = ' ';

            // Pattern name
            const char* n = p->name;
            while (*n && pos < 25) buf[pos++] = *n++;
            while (pos < 25) buf[pos++] = ' ';

            // Confidence
            buf[pos++] = '[';
            int conf_pct = (p->confidence * 100) / 242;
            digits = 0;
            val = conf_pct;
            do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
            if (digits == 0) num[digits++] = '0';
            while (digits > 0) buf[pos++] = num[--digits];
            buf[pos++] = '%'; buf[pos++] = ']';
            buf[pos] = '\0';

            MosColor color = (p->confidence > 160) ? MOS_COLOR_SUCCESS :
                            (p->confidence > 80) ? MOS_COLOR_INFO : MOS_COLOR_WARNING;
            mos_gfx_string(ctx, x, y, buf, color, MOS_COLOR_BG_DARK);
            y += 18;
        }
        if (witness->pattern_count > 5) {
            mos_gfx_string(ctx, x + 20, y, "...", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
            y += 18;
        }
    }

    y += 10;

    // Wisdom accumulated
    mos_gfx_string(ctx, x, y, "WISDOM ACCUMULATED:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    if (witness->wisdom_count == 0) {
        mos_gfx_string(ctx, x + 20, y, "(no wisdom yet - serve to learn)",
                       MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
        y += 18;
    } else {
        for (mos_size_t i = 0; i < witness->wisdom_count && i < 4; i++) {
            const MosWisdom* w = &witness->wisdom[i];
            if (!w->domain) continue;

            pos = 0;
            buf[pos++] = ' '; buf[pos++] = ' ';

            // Domain
            const char* d = w->domain;
            while (*d && pos < 20) buf[pos++] = *d++;
            while (pos < 20) buf[pos++] = ' ';

            // Depth
            buf[pos++] = 'd'; buf[pos++] = '=';
            digits = 0;
            val = w->depth;
            do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
            if (digits == 0) num[digits++] = '0';
            while (digits > 0) buf[pos++] = num[--digits];

            // Covenant indicator
            if (w->covenant_gained) {
                buf[pos++] = ' '; buf[pos++] = '['; buf[pos++] = 'C';
                buf[pos++] = 'O'; buf[pos++] = 'V'; buf[pos++] = ']';
            }
            buf[pos] = '\0';

            MosColor color = w->covenant_gained ? MOS_COLOR_GOLD : MOS_COLOR_TEXT;
            mos_gfx_string(ctx, x, y, buf, color, MOS_COLOR_BG_DARK);
            y += 18;
        }
    }

    y += 20;

    // Biblical grounding
    mos_gfx_string(ctx, x, y, "\"Let us search and try our ways,",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 18;
    mos_gfx_string(ctx, x, y, " and turn again to the LORD.\"",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 18;
    mos_gfx_string(ctx, x, y, "                 - Lamentations 3:40",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);

    // Footer
    y = bounds.y + bounds.height - 30;
    mos_gfx_string(ctx, x, y, "[N]ext probe  [P]revious probe  [ESC] Exit demo",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
}

// -----------------------------------------------------------------------------
// B.4 Registration
// -----------------------------------------------------------------------------

mos_result_t mos_diag_probe_witness_register(void) {
    return mos_diag_register_probe("WITNESS",
                                    witness_probe_query,
                                    witness_probe_check,
                                    witness_probe_demo);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Witness Diagnostic Probe provides:
//   - State query: understanding level, learning direction
//   - Invariant check: buffer bounds, value ranges
//   - Visual demo: understanding bar, patterns, wisdom
//
// The system learns through serving (covenant), not surveillance.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* CPISI_MODE >= 2 - Full probe implementation */
