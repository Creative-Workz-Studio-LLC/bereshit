// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-TOOLS-OMNICODE-PROGRESS-IMPL
// Title:       OmniCode Progress Visualization Implementation
// Type:        Source
// Component:   Tools / OmniCode Learning System
// Role:        Implementation of transpilation progress visualization
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Animate the journey from OmniCode to generated code.
//
// Grounding:   Genesis 1:3 — "And God said, Let there be light: and there was light."
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "progress.h"
#include <string.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Static Data
// -----------------------------------------------------------------------------

const char* OMNI_PHASE_NAMES[OMNI_PHASE_COUNT] = {
    "SOURCE",
    "PARSE",
    "VALIDATE",
    "TRANSFORM",
    "EMIT",
    "COMPLETE"
};

const char* OMNI_PHASE_DESCRIPTIONS[OMNI_PHASE_COUNT] = {
    "Reading the OmniCode source file...",
    "Parsing pragma sections and block structure...",
    "Validating structure against template...",
    "Transforming to target language format...",
    "Emitting generated code...",
    "Generation complete! Ready to compile."
};

// -----------------------------------------------------------------------------
// B.2 Initialization
// -----------------------------------------------------------------------------

mos_result_t omni_progress_init(OmniProgressState* state) {
    if (!state) return MOS_ERR_INVALID;

    memset(state, 0, sizeof(OmniProgressState));
    state->current_phase = OMNI_PHASE_SOURCE;
    state->animating = false;
    state->step_mode = false;
    state->focus_line = -1;

    return MOS_OK;
}

mos_result_t omni_progress_start_parsed(OmniProgressState* state,
                                         OmniParsedFile* file) {
    if (!state || !file) return MOS_ERR_INVALID;

    state->parsed = file;
    state->current_phase = OMNI_PHASE_SOURCE;
    state->phase_progress = 0.0f;
    state->total_progress = 0.0f;
    state->animating = true;
    state->anim_tick = 0;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.3 Animation Update
// -----------------------------------------------------------------------------

void omni_progress_update(OmniProgressState* state, uint32_t delta_ms) {
    if (!state || !state->animating || state->step_mode) return;

    state->anim_tick += delta_ms;

    // Advance phase progress
    float speed = 0.001f;  // 1% per ms = ~1 second per phase
    state->phase_progress += speed * (float)delta_ms;

    // Phase complete?
    if (state->phase_progress >= 1.0f) {
        state->phase_progress = 0.0f;
        if (state->current_phase < OMNI_PHASE_COMPLETE) {
            state->current_phase++;
        } else {
            state->animating = false;
        }
    }

    // Calculate total progress
    float phase_count = (float)OMNI_PHASE_COUNT;
    state->total_progress = ((float)state->current_phase + state->phase_progress) / phase_count;

    // Update focus based on phase
    switch (state->current_phase) {
        case OMNI_PHASE_SOURCE:
            state->focus_line = (int32_t)(state->phase_progress * 10);
            break;
        case OMNI_PHASE_PARSE:
            // Focus on different blocks as we parse
            state->focus_block = (OmniBlockType)(state->phase_progress * 4);
            break;
        case OMNI_PHASE_VALIDATE:
        case OMNI_PHASE_TRANSFORM:
        case OMNI_PHASE_EMIT:
            // Focus moves through blocks
            if (state->parsed && state->parsed->block_count > 0) {
                int32_t block_idx = (int32_t)(state->phase_progress * state->parsed->block_count);
                if (block_idx >= (int32_t)state->parsed->block_count) {
                    block_idx = state->parsed->block_count - 1;
                }
                state->focus_block = state->parsed->blocks[block_idx].type;
            }
            break;
        default:
            break;
    }
}

void omni_progress_step(OmniProgressState* state) {
    if (!state) return;

    state->phase_progress += 0.25f;  // 25% per step

    if (state->phase_progress >= 1.0f) {
        state->phase_progress = 0.0f;
        if (state->current_phase < OMNI_PHASE_COMPLETE) {
            state->current_phase++;
        }
    }

    // Recalculate total
    float phase_count = (float)OMNI_PHASE_COUNT;
    state->total_progress = ((float)state->current_phase + state->phase_progress) / phase_count;
}

void omni_progress_toggle_step(OmniProgressState* state) {
    if (state) {
        state->step_mode = !state->step_mode;
        if (!state->step_mode) {
            state->animating = true;
        }
    }
}

void omni_progress_reset(OmniProgressState* state) {
    if (state) {
        state->current_phase = OMNI_PHASE_SOURCE;
        state->phase_progress = 0.0f;
        state->total_progress = 0.0f;
        state->animating = true;
    }
}

// -----------------------------------------------------------------------------
// B.4 Rendering
// -----------------------------------------------------------------------------

void omni_progress_render_phases(OmniProgressState* state, MosGraphicsContext* ctx,
                                  MosRect bounds) {
    if (!state || !ctx) return;

    int32_t x = bounds.x;
    int32_t y = bounds.y;
    int32_t phase_width = bounds.width / OMNI_PHASE_COUNT;

    for (int i = 0; i < OMNI_PHASE_COUNT; i++) {
        MosRect phase_rect = {x + i * phase_width, y, phase_width - 4, 24};

        MosColor bg_color;
        if (i < (int)state->current_phase) {
            bg_color = MOS_RGB(50, 150, 50);   // Complete - green
        } else if (i == (int)state->current_phase) {
            // In progress - animated
            int32_t fill_width = (int32_t)(phase_rect.width * state->phase_progress);
            mos_gfx_fill_rect(ctx, (MosRect){phase_rect.x, phase_rect.y, fill_width, phase_rect.height},
                              MOS_RGB(100, 200, 100));
            bg_color = MOS_RGB(60, 60, 80);
        } else {
            bg_color = MOS_RGB(40, 40, 50);    // Pending - dark
        }

        mos_gfx_fill_rect(ctx, phase_rect, bg_color);
        mos_gfx_draw_rect(ctx, phase_rect, MOS_COLOR_TEXT_DIM, 1);

        // Phase name (abbreviated)
        char abbrev[4];
        abbrev[0] = OMNI_PHASE_NAMES[i][0];
        abbrev[1] = OMNI_PHASE_NAMES[i][1];
        abbrev[2] = OMNI_PHASE_NAMES[i][2];
        abbrev[3] = '\0';
        mos_gfx_string(ctx, phase_rect.x + 4, phase_rect.y + 5, abbrev,
                       MOS_COLOR_TEXT, bg_color);
    }
}

void omni_progress_render(OmniProgressState* state, MosGraphicsContext* ctx,
                           MosRect bounds) {
    if (!state || !ctx) return;

    int32_t x = bounds.x + 20;
    int32_t y = bounds.y + 20;

    // Title
    mos_gfx_string(ctx, x, y, "OmniCode Transpilation Progress", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
    y += 30;

    // Phase bar
    omni_progress_render_phases(state, ctx, (MosRect){x, y, bounds.width - 40, 24});
    y += 40;

    // Current phase description
    const char* desc = OMNI_PHASE_DESCRIPTIONS[state->current_phase];
    mos_gfx_string(ctx, x, y, desc, MOS_COLOR_INFO, MOS_COLOR_BG_DARK);
    y += 25;

    // Total progress bar
    mos_gfx_string(ctx, x, y, "Total Progress:", MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 20;

    int32_t bar_width = bounds.width - 100;
    int32_t fill_width = (int32_t)(bar_width * state->total_progress);
    MosRect bar_bg = {x, y, bar_width, 16};
    MosRect bar_fill = {x, y, fill_width, 16};

    mos_gfx_fill_rect(ctx, bar_bg, MOS_RGB(40, 40, 50));
    mos_gfx_fill_rect(ctx, bar_fill, MOS_COLOR_SUCCESS);
    mos_gfx_draw_rect(ctx, bar_bg, MOS_COLOR_TEXT_DIM, 1);

    // Percentage
    char pct[8];
    int p = (int)(state->total_progress * 100);
    pct[0] = '0' + (p / 100) % 10;
    pct[1] = '0' + (p / 10) % 10;
    pct[2] = '0' + p % 10;
    pct[3] = '%';
    pct[4] = '\0';
    mos_gfx_string(ctx, x + bar_width + 10, y + 1, pct, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
    y += 35;

    // Visualization area
    if (state->parsed && state->parsed->block_count > 0) {
        mos_gfx_string(ctx, x, y, "Building Blocks:", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
        y += 25;

        // Draw blocks with highlighting
        for (mos_size_t i = 0; i < state->parsed->block_count; i++) {
            OmniBlock* blk = &state->parsed->blocks[i];
            MosColor color = OMNI_BLOCK_COLORS[blk->type];

            // Is this block in focus?
            bool active = (blk->type == state->focus_block &&
                          state->current_phase >= OMNI_PHASE_PARSE);

            int32_t block_width = 80;
            int32_t block_height = 30;
            MosRect block_rect = {x + (int32_t)i * (block_width + 10), y,
                                  block_width, block_height};

            // Glow effect for active block
            if (active) {
                MosRect glow = {block_rect.x - 2, block_rect.y - 2,
                               block_rect.width + 4, block_rect.height + 4};
                mos_gfx_fill_rect(ctx, glow, MOS_COLOR_GOLD);
            }

            // Completion indicator based on phase
            if (state->current_phase >= OMNI_PHASE_TRANSFORM) {
                float block_progress = state->phase_progress;
                if (i < state->parsed->block_count - 1) {
                    float per_block = 1.0f / (float)state->parsed->block_count;
                    float block_start = (float)i * per_block;
                    if (state->total_progress >= block_start + per_block) {
                        block_progress = 1.0f;  // Complete
                    }
                }
                int32_t fill_h = (int32_t)(block_rect.height * block_progress);
                MosRect fill = {block_rect.x, block_rect.y + block_rect.height - fill_h,
                               block_rect.width, fill_h};
                mos_gfx_fill_rect(ctx, fill, MOS_COLOR_SUCCESS);
            }

            mos_gfx_fill_rect(ctx, block_rect, color);
            mos_gfx_draw_rect(ctx, block_rect, active ? MOS_COLOR_GOLD : MOS_COLOR_TEXT_DIM, 1);

            // Block name
            const char* name = OMNI_BLOCK_NAMES[blk->type];
            mos_gfx_string(ctx, block_rect.x + 4, block_rect.y + 8, name,
                           MOS_COLOR_BG_DARK, color);
        }

        y += 50;

        // Arrow showing transformation
        if (state->current_phase >= OMNI_PHASE_TRANSFORM) {
            mos_gfx_string(ctx, x + 150, y, ">>>", MOS_COLOR_SUCCESS, MOS_COLOR_BG_DARK);
            mos_gfx_string(ctx, x + 180, y, "Generated Code", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
        }
    }

    // Controls
    y = bounds.y + bounds.height - 30;
    if (state->step_mode) {
        mos_gfx_string(ctx, x, y, "[SPACE] Step  [T] Toggle auto  [R] Reset  [ESC] Exit",
                       MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    } else {
        mos_gfx_string(ctx, x, y, "[T] Toggle step mode  [R] Reset  [ESC] Exit",
                       MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    }
}

// -----------------------------------------------------------------------------
// B.5 Block Graph
// -----------------------------------------------------------------------------

mos_result_t omni_build_graph(OmniParsedFile* file, OmniBlockGraph* graph,
                               MosRect area) {
    if (!file || !graph) return MOS_ERR_INVALID;

    memset(graph, 0, sizeof(OmniBlockGraph));

    int32_t x_spacing = area.width / (file->block_count + 1);

    for (mos_size_t i = 0; i < file->block_count; i++) {
        OmniBlock* blk = &file->blocks[i];
        OmniVisualBlock* vb = &graph->blocks[i];

        vb->label = OMNI_BLOCK_NAMES[blk->type];
        vb->color = OMNI_BLOCK_COLORS[blk->type];
        vb->bounds = (MosRect){
            area.x + (int32_t)(i + 1) * x_spacing - 40,
            area.y + area.height / 2 - 20,
            80, 40
        };
        vb->active = false;
        vb->complete = false;

        graph->block_count++;

        // Connect to previous block
        if (i > 0) {
            graph->connections[graph->connection_count].from = i - 1;
            graph->connections[graph->connection_count].to = i;
            graph->connections[graph->connection_count].active = false;
            graph->connection_count++;
        }
    }

    return MOS_OK;
}

void omni_render_graph(OmniBlockGraph* graph, MosGraphicsContext* ctx,
                        OmniTranspilePhase phase, float progress) {
    if (!graph || !ctx) return;

    // Draw connections first (behind blocks)
    for (mos_size_t i = 0; i < graph->connection_count; i++) {
        OmniVisualBlock* from = &graph->blocks[graph->connections[i].from];
        OmniVisualBlock* to = &graph->blocks[graph->connections[i].to];

        int32_t x1 = from->bounds.x + from->bounds.width;
        int32_t y1 = from->bounds.y + from->bounds.height / 2;
        int32_t x2 = to->bounds.x;
        int32_t y2 = to->bounds.y + to->bounds.height / 2;

        MosColor line_color = graph->connections[i].active ?
                              MOS_COLOR_SUCCESS : MOS_COLOR_TEXT_DIM;

        // Draw horizontal line
        mos_gfx_hline(ctx, x1, y1, x2 - x1, line_color);
    }

    // Draw blocks
    for (mos_size_t i = 0; i < graph->block_count; i++) {
        OmniVisualBlock* vb = &graph->blocks[i];

        MosColor border = vb->active ? MOS_COLOR_GOLD :
                         vb->complete ? MOS_COLOR_SUCCESS : MOS_COLOR_TEXT_DIM;

        mos_gfx_fill_rect(ctx, vb->bounds, vb->color);
        mos_gfx_draw_rect(ctx, vb->bounds, border, vb->active ? 2 : 1);

        // Label
        mos_gfx_string(ctx, vb->bounds.x + 4, vb->bounds.y + 12, vb->label,
                       MOS_COLOR_BG_DARK, vb->color);
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Usage:
//   OmniProgressState progress;
//   omni_progress_init(&progress);
//   omni_progress_start_parsed(&progress, &parsed_file);
//
//   // In game loop:
//   omni_progress_update(&progress, delta_ms);
//   omni_progress_render(&progress, ctx, bounds);
//
// Controls:
//   T - Toggle step/auto mode
//   SPACE - Step forward (in step mode)
//   R - Reset to beginning
//
// =============================================================================
// END CLOSING
// =============================================================================
