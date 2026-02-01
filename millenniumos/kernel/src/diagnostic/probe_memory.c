// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-diagnostic-probe-memory
// Title:       Memory Diagnostic Probe
// Type:        Source
// Component:   Kernel / Diagnostic Subsystem
// Role:        Diagnostic probe for Memory Subsystem
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Register Memory with diagnostic service for state queries,
//              invariant checks, and visual demonstration.
//
// Grounding:   Genesis 1:9 — "Let the waters be gathered together"
//              Memory gathers and organizes - this probe observes its state.
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "diagnostic.h"
#include "memory/memory.h"
#include "graphics/graphics.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Memory State Query
// -----------------------------------------------------------------------------

static mos_result_t memory_probe_query(MosDiagState* out) {
    if (!out) return MOS_ERR_INVALID;

    const MosMemoryManager* mem = mos_memory_info();
    if (!mem) return MOS_ERR_NODEV;

    // Copy name
    out->name[0] = 'M'; out->name[1] = 'E'; out->name[2] = 'M';
    out->name[3] = '\0';

    // Get memory health
    out->health = mem->health;

    // Calculate health based on memory pressure
    if (mem->frame_count > 0) {
        // Free percentage
        int32_t free_pct = (int32_t)((mem->free_count * 100) / mem->frame_count);

        int32_t health = mem->health;
        if (free_pct > 50) {
            health = MOS_HEALTH_MAX;
        } else if (free_pct > 30) {
            health = 50;
        } else if (free_pct > 15) {
            health = 0;
        } else if (free_pct > 5) {
            health = -50;
        } else {
            health = MOS_HEALTH_MIN;
        }
        out->health = (mos_health_t)health;
    }

    // Determine cognitive state
    if (out->health > 50) {
        out->state = MOS_STATE_TOV;
    } else if (out->health > 0) {
        out->state = MOS_STATE_SHALEM;
    } else if (out->health == 0) {
        out->state = MOS_STATE_YASHAR;
    } else if (out->health > -50) {
        out->state = MOS_STATE_CHASER;
    } else {
        out->state = MOS_STATE_SHAVAR;
    }

    out->event_count = mem->alloc_count;
    out->error_count = mem->page_faults;

    // Status message
    if (out->health > 50) {
        out->status_message = "Memory abundant";
    } else if (out->health > 0) {
        out->status_message = "Memory adequate";
    } else if (out->health == 0) {
        out->status_message = "Memory neutral";
    } else if (out->health > -50) {
        out->status_message = "Memory pressure";
    } else {
        out->status_message = "Memory critical";
    }

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.2 Memory Invariant Check
// -----------------------------------------------------------------------------

static mos_result_t memory_probe_check(void) {
    const MosMemoryManager* mem = mos_memory_info();
    if (!mem) return MOS_ERR_NODEV;

    mos_size_t errors = 0;

    // Check: free + used should equal total frames
    if (mem->free_count + mem->used_count != mem->frame_count) {
        errors++;
    }

    // Check: usable memory should not exceed total
    if (mem->usable_memory > mem->total_memory) {
        errors++;
    }

    // Check: kernel memory should be within usable
    if (mem->kernel_memory > mem->usable_memory) {
        errors++;
    }

    // Check: health should be in valid range
    // Note: int8_t can't exceed MOS_HEALTH_MAX (127), only check lower bound
    if (mem->health < MOS_HEALTH_MIN) {
        errors++;
    }

    // Check: frame array should exist if frame_count > 0
    if (mem->frame_count > 0 && mem->frames == NULL) {
        errors++;
    }

    // Sample frame checks (check every 100th frame to avoid overhead)
    if (mem->frames && mem->frame_count > 0) {
        for (mos_size_t i = 0; i < mem->frame_count; i += 100) {
            const MosPageFrame* frame = &mem->frames[i];

            // Note: state is uint8_t, range check removed (always 0-255)

            // Check: address should be page-aligned
            if (frame->address % MOS_PAGE_SIZE != 0) {
                errors++;
            }
        }
    }

    return (errors == 0) ? MOS_OK : (mos_result_t)(-errors);
}

// -----------------------------------------------------------------------------
// B.3 Memory Visual Demo
// -----------------------------------------------------------------------------

static void memory_probe_demo(MosGraphicsContext* ctx, MosRect bounds) {
    if (!ctx) return;

    const MosMemoryManager* mem = mos_memory_info();
    if (!mem) return;

    int32_t x = bounds.x + 20;
    int32_t y = bounds.y + 20;

    // Title
    mos_gfx_string(ctx, x, y, "Memory Subsystem State", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
    y += 30;

    // Memory statistics
    mos_gfx_string(ctx, x, y, "STATISTICS:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    char buf[80];
    char num[16];
    int pos;

    // Total memory
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'T'; buf[pos++] = 'o'; buf[pos++] = 't';
    buf[pos++] = 'a'; buf[pos++] = 'l'; buf[pos++] = ':';
    buf[pos++] = ' ';
    // Convert bytes to MB
    mos_size_t total_mb = mem->total_memory / (1024 * 1024);
    int digits = 0;
    mos_size_t val = total_mb;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos++] = ' '; buf[pos++] = 'M'; buf[pos++] = 'B';
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    y += 18;

    // Usable memory
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'U'; buf[pos++] = 's'; buf[pos++] = 'a';
    buf[pos++] = 'b'; buf[pos++] = 'l'; buf[pos++] = 'e';
    buf[pos++] = ':'; buf[pos++] = ' ';
    mos_size_t usable_mb = mem->usable_memory / (1024 * 1024);
    digits = 0;
    val = usable_mb;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos++] = ' '; buf[pos++] = 'M'; buf[pos++] = 'B';
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    y += 18;

    // Kernel memory
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'K'; buf[pos++] = 'e'; buf[pos++] = 'r';
    buf[pos++] = 'n'; buf[pos++] = 'e'; buf[pos++] = 'l';
    buf[pos++] = ':'; buf[pos++] = ' ';
    mos_size_t kernel_kb = mem->kernel_memory / 1024;
    digits = 0;
    val = kernel_kb;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos++] = ' '; buf[pos++] = 'K'; buf[pos++] = 'B';
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    y += 30;

    // Frame usage bar
    mos_gfx_string(ctx, x, y, "FRAME USAGE:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    int32_t bar_x = x;
    int32_t bar_y = y;
    int32_t bar_w = 300;
    int32_t bar_h = 30;

    // Background
    mos_gfx_fill_rect(ctx, (MosRect){bar_x, bar_y, bar_w, bar_h}, MOS_RGB(40, 40, 40));

    // Used portion
    if (mem->frame_count > 0) {
        int32_t used_w = (int32_t)((mem->used_count * bar_w) / mem->frame_count);
        if (used_w > 0) {
            MosColor used_color = MOS_COLOR_INFO;
            int32_t used_pct = (int32_t)((mem->used_count * 100) / mem->frame_count);
            if (used_pct > 85) used_color = MOS_COLOR_ERROR;
            else if (used_pct > 70) used_color = MOS_COLOR_WARNING;

            mos_gfx_fill_rect(ctx, (MosRect){bar_x, bar_y, used_w, bar_h}, used_color);
        }
    }

    // Border
    mos_gfx_draw_rect(ctx, (MosRect){bar_x, bar_y, bar_w, bar_h}, MOS_COLOR_TEXT_DIM, 1);

    // Label
    pos = 0;
    digits = 0;
    val = mem->used_count;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos++] = '/';
    digits = 0;
    val = mem->frame_count;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos++] = ' '; buf[pos++] = 'f'; buf[pos++] = 'r';
    buf[pos++] = 'a'; buf[pos++] = 'm'; buf[pos++] = 'e';
    buf[pos++] = 's';
    buf[pos] = '\0';
    mos_gfx_string(ctx, bar_x + bar_w + 10, bar_y + 8, buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);

    y += bar_h + 20;

    // Allocation stats
    y += 10;
    mos_gfx_string(ctx, x, y, "OPERATIONS:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Allocations
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'A'; buf[pos++] = 'l'; buf[pos++] = 'l';
    buf[pos++] = 'o'; buf[pos++] = 'c'; buf[pos++] = 's';
    buf[pos++] = ':'; buf[pos++] = ' ';
    digits = 0;
    val = mem->alloc_count;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_SUCCESS, MOS_COLOR_BG_DARK);
    y += 18;

    // Frees
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'F'; buf[pos++] = 'r'; buf[pos++] = 'e';
    buf[pos++] = 'e'; buf[pos++] = 's'; buf[pos++] = ':';
    buf[pos++] = ' '; buf[pos++] = ' ';
    digits = 0;
    val = mem->free_ops;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    mos_gfx_string(ctx, x, y, buf, MOS_COLOR_INFO, MOS_COLOR_BG_DARK);
    y += 18;

    // Page faults
    pos = 0;
    buf[pos++] = ' '; buf[pos++] = ' ';
    buf[pos++] = 'F'; buf[pos++] = 'a'; buf[pos++] = 'u';
    buf[pos++] = 'l'; buf[pos++] = 't'; buf[pos++] = 's';
    buf[pos++] = ':'; buf[pos++] = ' ';
    digits = 0;
    val = mem->page_faults;
    do { num[digits++] = '0' + (val % 10); val /= 10; } while (val && digits < 15);
    while (digits > 0) buf[pos++] = num[--digits];
    buf[pos] = '\0';
    MosColor fault_color = mem->page_faults > 100 ? MOS_COLOR_WARNING : MOS_COLOR_TEXT;
    mos_gfx_string(ctx, x, y, buf, fault_color, MOS_COLOR_BG_DARK);

    y += 40;

    // Frame map visualization (sample)
    mos_gfx_string(ctx, x, y, "FRAME MAP (sample):", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    // Draw grid of frame states
    int32_t grid_x = x;
    int32_t grid_y = y;
    int32_t cell_size = 8;
    int32_t grid_cols = 40;
    int32_t grid_rows = 6;

    for (int32_t row = 0; row < grid_rows; row++) {
        for (int32_t col = 0; col < grid_cols; col++) {
            mos_size_t frame_idx = (row * grid_cols + col) * (mem->frame_count / (grid_cols * grid_rows + 1));
            if (frame_idx >= mem->frame_count) break;

            MosColor cell_color;
            if (mem->frames && frame_idx < mem->frame_count) {
                trit5_t state = mem->frames[frame_idx].state;
                if (state == MOS_FRAME_FREE) {
                    cell_color = MOS_COLOR_SUCCESS;  // Free = green
                } else if (state == MOS_FRAME_ALLOCATED) {
                    cell_color = MOS_COLOR_INFO;     // Allocated = blue
                } else if (state == MOS_FRAME_RESERVED) {
                    cell_color = MOS_COLOR_WARNING;  // Reserved = yellow
                } else {
                    cell_color = MOS_COLOR_ERROR;    // Special/error = red
                }
            } else {
                cell_color = MOS_RGB(60, 60, 60);  // Unknown
            }

            mos_gfx_fill_rect(ctx,
                (MosRect){grid_x + col * (cell_size + 1), grid_y + row * (cell_size + 1),
                          cell_size, cell_size},
                cell_color);
        }
    }

    // Legend
    y = grid_y + grid_rows * (cell_size + 1) + 15;
    mos_gfx_fill_rect(ctx, (MosRect){x, y, 10, 10}, MOS_COLOR_SUCCESS);
    mos_gfx_string(ctx, x + 15, y - 2, "Free", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    mos_gfx_fill_rect(ctx, (MosRect){x + 70, y, 10, 10}, MOS_COLOR_INFO);
    mos_gfx_string(ctx, x + 85, y - 2, "Alloc", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    mos_gfx_fill_rect(ctx, (MosRect){x + 150, y, 10, 10}, MOS_COLOR_WARNING);
    mos_gfx_string(ctx, x + 165, y - 2, "Rsrvd", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);

    // Footer
    y = bounds.y + bounds.height - 30;
    mos_gfx_string(ctx, x, y, "[N]ext probe  [P]revious probe  [ESC] Exit demo",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
}

// -----------------------------------------------------------------------------
// B.4 Registration
// -----------------------------------------------------------------------------

mos_result_t mos_diag_probe_memory_register(void) {
    return mos_diag_register_probe("MEM",
                                    memory_probe_query,
                                    memory_probe_check,
                                    memory_probe_demo);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Memory Diagnostic Probe provides:
//   - State query: total/usable/free memory, allocation stats
//   - Invariant check: frame consistency, address alignment
//   - Visual demo: memory usage bar, frame map visualization
//
// =============================================================================
// END CLOSING
// =============================================================================
