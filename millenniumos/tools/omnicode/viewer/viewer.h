// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-TOOLS-OMNICODE-VIEWER
// Title:       OmniCode Viewer
// Type:        Header
// Component:   Tools / OmniCode Learning System
// Role:        Visual display of OmniCode structure and building blocks
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Parse and display OmniCode files with visual structure highlighting.
//              Shows pragma sections, blocks, and how they connect.
//              Enables learning through visualization.
//
// Grounding:   Isaiah 28:10 — "For precept must be upon precept, line upon line"
//              Learning happens layer by layer, block by block.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_OMNICODE_VIEWER_H
#define MOS_OMNICODE_VIEWER_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// MillenniumOS kernel (for graphics and HAL)
#include "../../kernel/types.h"
#include "../../kernel/graphics/graphics.h"
#include "../../kernel/hal/hal.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Visualization Constants
// -----------------------------------------------------------------------------
//
// These are viewer-specific types for visual display.
// Parsing is done by libomni (OmniDocument, OmniParser).
// The viewer adds colors and view modes on top.
//

// Pragma section types (for visual grouping)
typedef enum {
    OMNI_PRAGMA_CORE,       // P.1: key, from, at
    OMNI_PRAGMA_FAMILY,     // P.2: type, subtype, role, structure
    OMNI_PRAGMA_INSTANCE,   // P.3: component, layer, includes, provides
    OMNI_PRAGMA_ARCH,       // P.4: layers, scale, pattern, anchor
    OMNI_PRAGMA_SUMMARY,    // P.5: title, brief
    OMNI_PRAGMA_COUNT
} OmniPragmaSection;

// Pragma section names
extern const char* OMNI_PRAGMA_NAMES[OMNI_PRAGMA_COUNT];

// -----------------------------------------------------------------------------
// B.2 Block Visual Types
// -----------------------------------------------------------------------------
//
// Block types for visualization coloring.
// Maps to libomni's OmniBlockContext enum values.
//

typedef enum {
    OMNI_BLOCK_METADATA,    // Identity and purpose
    OMNI_BLOCK_HEADER,      // Human-readable intro (docs only)
    OMNI_BLOCK_CONTEXT,     // Prerequisites (docs only)
    OMNI_BLOCK_SETUP,       // Imports, constants (code only)
    OMNI_BLOCK_BODY,        // Main content
    OMNI_BLOCK_CONTENT,     // Main content (docs only)
    OMNI_BLOCK_CLOSING,     // Usage, reference (code only)
    OMNI_BLOCK_FOOTER,      // References (docs only)
    OMNI_BLOCK_COUNT
} OmniBlockType;

// Block type names
extern const char* OMNI_BLOCK_NAMES[OMNI_BLOCK_COUNT];

// Block colors for visualization
extern const MosColor OMNI_BLOCK_COLORS[OMNI_BLOCK_COUNT];

// -----------------------------------------------------------------------------
// B.3 Marker Visual Types
// -----------------------------------------------------------------------------
//
// Markers indicate requirement level (for coloring).
//

typedef enum {
    OMNI_MARKER_REQ,        // @omni:req - Required
    OMNI_MARKER_INH,        // @omni:inh - Inherited
    OMNI_MARKER_INS,        // @omni:ins - Instance
    OMNI_MARKER_OPT,        // @omni:opt - Optional
    OMNI_MARKER_COUNT
} OmniMarkerType;

// Marker names
extern const char* OMNI_MARKER_NAMES[OMNI_MARKER_COUNT];

// Marker colors
extern const MosColor OMNI_MARKER_COLORS[OMNI_MARKER_COUNT];

// -----------------------------------------------------------------------------
// B.4 Legacy Parsed Structure (for viewer compatibility)
// -----------------------------------------------------------------------------
//
// NOTE: This is a legacy structure for backward compatibility.
// New code should use libomni's OmniDocument instead.
// The viewer will transition to OmniDocument in the future.
//

#define OMNI_MAX_FIELDS     64
#define OMNI_MAX_BLOCKS     8
#define OMNI_MAX_LINE_LEN   256

// A single field (key = value)
typedef struct {
    OmniMarkerType      marker;
    char                key[32];
    char                value[128];
    int32_t             line_number;
} OmniField;

// A parsed block
typedef struct {
    OmniBlockType       type;
    int32_t             start_line;
    int32_t             end_line;
    int32_t             content_lines;
} OmniBlock;

// Complete parsed file
typedef struct {
    // File info
    char                filename[128];
    char                format[32];         // "code", "data", "document"
    char                language[16];       // "c", "go", "toml", etc.
    char                filetype[16];       // "source", "header", etc.

    // Pragma fields
    OmniField           fields[OMNI_MAX_FIELDS];
    mos_size_t          field_count;

    // Blocks
    OmniBlock           blocks[OMNI_MAX_BLOCKS];
    mos_size_t          block_count;

    // Statistics
    int32_t             total_lines;
    int32_t             comment_lines;
    int32_t             code_lines;
    int32_t             blank_lines;
} OmniParsedFile;

// -----------------------------------------------------------------------------
// B.5 Viewer State
// -----------------------------------------------------------------------------

typedef enum {
    OMNI_VIEW_STRUCTURE,    // Show block structure
    OMNI_VIEW_PRAGMA,       // Show pragma details
    OMNI_VIEW_BLOCKS,       // Show block contents
    OMNI_VIEW_LEARNING      // Learning mode with explanations
} OmniViewMode;

typedef struct {
    OmniParsedFile*     file;
    OmniViewMode        mode;

    // Selection
    int32_t             selected_field;
    int32_t             selected_block;

    // Scroll
    int32_t             scroll_offset;
    int32_t             visible_lines;

    // Learning mode
    bool                show_explanations;
    const char*         current_explanation;
} OmniViewerState;

// -----------------------------------------------------------------------------
// B.6 Parsing Functions
// -----------------------------------------------------------------------------

// Parse an OmniCode file
mos_result_t omni_parse_file(const char* filepath, OmniParsedFile* out);

// Parse from memory
mos_result_t omni_parse_buffer(const char* buffer, mos_size_t size,
                                OmniParsedFile* out);

// Free parsed file resources
void omni_parsed_free(OmniParsedFile* file);

// -----------------------------------------------------------------------------
// B.7 Viewer Functions
// -----------------------------------------------------------------------------

// Initialize viewer
mos_result_t omni_viewer_init(OmniViewerState* state);

// Load file into viewer
mos_result_t omni_viewer_load(OmniViewerState* state, const char* filepath);

// Render viewer
void omni_viewer_render(OmniViewerState* state, MosGraphicsContext* ctx,
                         MosRect bounds);

// Handle input
bool omni_viewer_input(OmniViewerState* state, const MosEvent* event);

// Switch view mode
void omni_viewer_set_mode(OmniViewerState* state, OmniViewMode mode);

// -----------------------------------------------------------------------------
// B.8 Learning Mode Functions
// -----------------------------------------------------------------------------

// Get explanation for current selection
const char* omni_get_explanation(OmniViewerState* state);

// Get explanation for a pragma section
const char* omni_explain_pragma(OmniPragmaSection section);

// Get explanation for a block type
const char* omni_explain_block(OmniBlockType block);

// Get explanation for a marker type
const char* omni_explain_marker(OmniMarkerType marker);

// -----------------------------------------------------------------------------
// B.9 Building Block View
// -----------------------------------------------------------------------------
//
// Shows how components connect - the "see the building blocks" view.
//

typedef struct {
    const char*         name;
    const char*         description;
    OmniBlockType       block;
    int32_t             depends_on[4];  // Indices of dependencies
    int32_t             dep_count;
} OmniComponent;

// Get component graph for a parsed file
mos_result_t omni_get_components(OmniParsedFile* file,
                                  OmniComponent* out, mos_size_t max_count,
                                  mos_size_t* count);

// Render component graph
void omni_render_components(MosGraphicsContext* ctx, MosRect bounds,
                             OmniComponent* components, mos_size_t count);

// =============================================================================
// END BODY
// =============================================================================

#endif // MOS_OMNICODE_VIEWER_H

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// OmniCode Viewer provides:
//   - File parsing: Extract pragma, blocks, structure
//   - Structure view: See the 4/5-block layout
//   - Pragma view: Examine identity fields
//   - Learning mode: Explanations for each construct
//   - Building blocks view: See how components connect
//
// "For precept must be upon precept, line upon line,
//  here a little, and there a little."
//   — Isaiah 28:10
//
// Learning OmniCode happens by seeing, not just reading.
//
// =============================================================================
// END CLOSING
// =============================================================================
