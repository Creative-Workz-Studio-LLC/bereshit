// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-TOOLS-OMNICODE-PROGRESS
// Title:       OmniCode Progress Visualization
// Type:        Header
// Component:   Tools / OmniCode Learning System
// Role:        Visualize the transformation from OmniCode → Generated Code
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Show the journey from OmniCode specification to generated code.
//              Animated visualization of the transpilation process.
//              "Watch specs become code."
//
// Grounding:   Genesis 1:3 — "And God said, Let there be light: and there was light."
//              The word becomes reality. Specification becomes implementation.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_OMNICODE_PROGRESS_H
#define MOS_OMNICODE_PROGRESS_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "../../kernel/types.h"
#include "../../kernel/graphics/graphics.h"
#include "viewer.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Transpilation Phases
// -----------------------------------------------------------------------------
//
// The journey from OmniCode to generated code.
//

typedef enum {
    OMNI_PHASE_SOURCE,      // Original .omni file
    OMNI_PHASE_PARSE,       // Parsing pragma and structure
    OMNI_PHASE_VALIDATE,    // Validating structure
    OMNI_PHASE_TRANSFORM,   // Transforming to target format
    OMNI_PHASE_EMIT,        // Emitting generated code
    OMNI_PHASE_COMPLETE,    // Generation complete
    OMNI_PHASE_COUNT
} OmniTranspilePhase;

// Phase names for display
extern const char* OMNI_PHASE_NAMES[OMNI_PHASE_COUNT];

// Phase descriptions
extern const char* OMNI_PHASE_DESCRIPTIONS[OMNI_PHASE_COUNT];

// -----------------------------------------------------------------------------
// B.2 Progress State
// -----------------------------------------------------------------------------

typedef struct {
    // Source
    const char*         source_path;
    OmniParsedFile*     parsed;

    // Progress
    OmniTranspilePhase  current_phase;
    float               phase_progress;     // 0.0 - 1.0 within phase
    float               total_progress;     // 0.0 - 1.0 overall

    // Output
    char*               generated_code;
    mos_size_t          generated_size;

    // Animation
    uint32_t            anim_tick;
    bool                animating;
    bool                step_mode;          // Manual stepping

    // Current focus (for highlighting)
    int32_t             focus_line;
    OmniBlockType       focus_block;
} OmniProgressState;

// -----------------------------------------------------------------------------
// B.3 Progress Functions
// -----------------------------------------------------------------------------

// Initialize progress visualization
mos_result_t omni_progress_init(OmniProgressState* state);

// Start transpilation visualization
mos_result_t omni_progress_start(OmniProgressState* state,
                                  const char* source_path);

// Start from parsed file
mos_result_t omni_progress_start_parsed(OmniProgressState* state,
                                         OmniParsedFile* file);

// Step forward (for step mode)
void omni_progress_step(OmniProgressState* state);

// Update animation (call each frame)
void omni_progress_update(OmniProgressState* state, uint32_t delta_ms);

// Toggle step mode
void omni_progress_toggle_step(OmniProgressState* state);

// Reset to beginning
void omni_progress_reset(OmniProgressState* state);

// -----------------------------------------------------------------------------
// B.4 Rendering Functions
// -----------------------------------------------------------------------------

// Render the progress view
void omni_progress_render(OmniProgressState* state, MosGraphicsContext* ctx,
                           MosRect bounds);

// Render phase indicator
void omni_progress_render_phases(OmniProgressState* state, MosGraphicsContext* ctx,
                                  MosRect bounds);

// Render source-to-output comparison
void omni_progress_render_comparison(OmniProgressState* state, MosGraphicsContext* ctx,
                                      MosRect bounds);

// -----------------------------------------------------------------------------
// B.5 Building Blocks Visualization
// -----------------------------------------------------------------------------
//
// Show how the building blocks connect during transformation.
//

typedef struct {
    const char*     label;
    MosRect         bounds;
    MosColor        color;
    bool            active;
    bool            complete;
} OmniVisualBlock;

typedef struct {
    OmniVisualBlock blocks[OMNI_BLOCK_COUNT];
    mos_size_t      block_count;

    // Connections between blocks
    struct {
        mos_size_t  from;
        mos_size_t  to;
        bool        active;
    } connections[16];
    mos_size_t      connection_count;
} OmniBlockGraph;

// Build block graph from parsed file
mos_result_t omni_build_graph(OmniParsedFile* file, OmniBlockGraph* graph,
                               MosRect area);

// Render block graph with animation
void omni_render_graph(OmniBlockGraph* graph, MosGraphicsContext* ctx,
                        OmniTranspilePhase phase, float progress);

// =============================================================================
// END BODY
// =============================================================================

#endif // MOS_OMNICODE_PROGRESS_H

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Progress Visualization shows:
//   1. SOURCE: Original OmniCode file
//   2. PARSE: Breaking into pragma and blocks
//   3. VALIDATE: Checking structure
//   4. TRANSFORM: Converting to target format
//   5. EMIT: Writing generated code
//   6. COMPLETE: Ready to compile
//
// "And God said, Let there be light: and there was light."
//   — Genesis 1:3
//
// The specification (word) becomes the implementation (reality).
// OmniCode → Generated Code, step by step.
//
// =============================================================================
// END CLOSING
// =============================================================================
