// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-TOOLS-OMNICODE-VIEWER-IMPL
// Title:       OmniCode Viewer Implementation
// Type:        Source
// Component:   Tools / OmniCode Learning System
// Role:        Implementation of OmniCode parser and visual viewer
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Parse OmniCode files and render visual structure with learning mode.
//
// Grounding:   Isaiah 28:10 — "For precept must be upon precept, line upon line"
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "viewer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Static Data - Names and Colors
// -----------------------------------------------------------------------------

const char* OMNI_PRAGMA_NAMES[OMNI_PRAGMA_COUNT] = {
    "CORE",         // P.1
    "FAMILY",       // P.2
    "INSTANCE",     // P.3
    "ARCHITECTURE", // P.4
    "SUMMARY"       // P.5
};

const char* OMNI_BLOCK_NAMES[OMNI_BLOCK_COUNT] = {
    "METADATA",
    "HEADER",
    "CONTEXT",
    "SETUP",
    "BODY",
    "CONTENT",
    "CLOSING",
    "FOOTER"
};

const MosColor OMNI_BLOCK_COLORS[OMNI_BLOCK_COUNT] = {
    MOS_RGB(100, 149, 237),  // METADATA - cornflower blue
    MOS_RGB(144, 238, 144),  // HEADER - light green
    MOS_RGB(255, 218, 185),  // CONTEXT - peach
    MOS_RGB(173, 216, 230),  // SETUP - light blue
    MOS_RGB(221, 160, 221),  // BODY - plum
    MOS_RGB(255, 228, 181),  // CONTENT - moccasin
    MOS_RGB(176, 196, 222),  // CLOSING - light steel blue
    MOS_RGB(211, 211, 211)   // FOOTER - light gray
};

const char* OMNI_MARKER_NAMES[OMNI_MARKER_COUNT] = {
    "req",  // Required
    "inh",  // Inherited
    "ins",  // Instance
    "opt"   // Optional
};

const MosColor OMNI_MARKER_COLORS[OMNI_MARKER_COUNT] = {
    MOS_RGB(255, 99, 71),    // REQ - tomato (must have)
    MOS_RGB(255, 215, 0),    // INH - gold (from template)
    MOS_RGB(50, 205, 50),    // INS - lime green (file-specific)
    MOS_RGB(135, 206, 235)   // OPT - sky blue (optional)
};

// -----------------------------------------------------------------------------
// B.2 Learning Mode Explanations
// -----------------------------------------------------------------------------

static const char* PRAGMA_EXPLANATIONS[OMNI_PRAGMA_COUNT] = {
    // CORE
    "CORE identifies the file.\n\n"
    "- key: Unique identifier (like a name)\n"
    "- from: Template this derives from\n"
    "- at: Current version\n\n"
    "Every OmniCode file has these three things.",

    // FAMILY
    "FAMILY classifies the file.\n\n"
    "- type: source, header, config, document\n"
    "- subtype: implementation, interface, test\n"
    "- structure: 3-block, 4-block, 5-block\n\n"
    "These are inherited from the template.",

    // INSTANCE
    "INSTANCE is what makes this file unique.\n\n"
    "- component: Which system it belongs to\n"
    "- layer: Where in the architecture (0-8)\n"
    "- includes: What it needs\n"
    "- provides: What it gives to others\n\n"
    "These values are specific to THIS file.",

    // ARCHITECTURE
    "ARCHITECTURE positions the file in the system.\n\n"
    "The 9-layer pyramid:\n"
    "  0-3: Below (foundation layers)\n"
    "  4: Center (health/balance)\n"
    "  5-8: Above (application layers)\n\n"
    "This shows WHERE the file fits.",

    // SUMMARY
    "SUMMARY is the human-readable identity.\n\n"
    "- title: What to call this file\n"
    "- brief: One sentence description\n\n"
    "If you only read one section, read this."
};

static const char* BLOCK_EXPLANATIONS[OMNI_BLOCK_COUNT] = {
    // METADATA
    "METADATA is the file's identity card.\n\n"
    "Contains:\n"
    "- Who created it\n"
    "- What it does\n"
    "- Why it exists\n"
    "- Biblical grounding\n\n"
    "Machines read this to understand the file.",

    // HEADER
    "HEADER welcomes human readers.\n\n"
    "Contains:\n"
    "- Visual title\n"
    "- Navigation/table of contents\n"
    "- Quick status overview\n\n"
    "Only in documentation (5-block).",

    // CONTEXT
    "CONTEXT prepares the reader.\n\n"
    "Contains:\n"
    "- Prerequisites (what you need to know)\n"
    "- Key definitions\n"
    "- Scope (what's covered, what's not)\n\n"
    "Only in documentation (5-block).",

    // SETUP
    "SETUP prepares the code to run.\n\n"
    "Contains:\n"
    "- #include directives\n"
    "- Constants and macros\n"
    "- Type definitions\n"
    "- Forward declarations\n\n"
    "Nothing executable here - just preparation.",

    // BODY
    "BODY is where the work happens.\n\n"
    "Contains:\n"
    "- Functions and implementations\n"
    "- The actual logic\n"
    "- Helper functions first\n"
    "- Public API last\n\n"
    "This is the heart of the file.",

    // CONTENT
    "CONTENT is the main documentation.\n\n"
    "Contains:\n"
    "- The actual content being documented\n"
    "- Organized by topic\n"
    "- Examples and explanations\n\n"
    "Only in documentation (5-block).",

    // CLOSING
    "CLOSING shows how to use the code.\n\n"
    "Contains:\n"
    "- Usage examples\n"
    "- Troubleshooting\n"
    "- Quick reference\n"
    "- Biblical anchoring\n\n"
    "Read this after BODY to use the code.",

    // FOOTER
    "FOOTER connects to other resources.\n\n"
    "Contains:\n"
    "- Related documents\n"
    "- External references\n"
    "- Version history\n\n"
    "Only in documentation (5-block)."
};

static const char* MARKER_EXPLANATIONS[OMNI_MARKER_COUNT] = {
    // REQ
    "@omni:req = REQUIRED\n\n"
    "This field MUST exist.\n"
    "Validation fails without it.\n\n"
    "Examples: key, title, from",

    // INH
    "@omni:inh = INHERITED\n\n"
    "This field comes from the template.\n"
    "You CAN override it if needed.\n\n"
    "Examples: type, structure, role",

    // INS
    "@omni:ins = INSTANCE\n\n"
    "This field is specific to THIS file.\n"
    "Not from template - unique values.\n\n"
    "Examples: component, layer, provides",

    // OPT
    "@omni:opt = OPTIONAL\n\n"
    "This field is optional.\n"
    "Include it if relevant, omit if not.\n\n"
    "Examples: notes, deprecated"
};

// -----------------------------------------------------------------------------
// B.3 Parsing Implementation
// -----------------------------------------------------------------------------

// Helper: Check if line starts with string
static bool starts_with(const char* line, const char* prefix) {
    return strncmp(line, prefix, strlen(prefix)) == 0;
}

// Helper: Parse marker type from string
static OmniMarkerType parse_marker(const char* marker) {
    if (strcmp(marker, "req") == 0) return OMNI_MARKER_REQ;
    if (strcmp(marker, "inh") == 0) return OMNI_MARKER_INH;
    if (strcmp(marker, "ins") == 0) return OMNI_MARKER_INS;
    if (strcmp(marker, "opt") == 0) return OMNI_MARKER_OPT;
    return OMNI_MARKER_OPT;  // Default to optional
}

// Helper: Parse block type from tag
static OmniBlockType parse_block_type(const char* tag) {
    if (strcmp(tag, "METADATA") == 0) return OMNI_BLOCK_METADATA;
    if (strcmp(tag, "HEADER") == 0) return OMNI_BLOCK_HEADER;
    if (strcmp(tag, "CONTEXT") == 0) return OMNI_BLOCK_CONTEXT;
    if (strcmp(tag, "SETUP") == 0) return OMNI_BLOCK_SETUP;
    if (strcmp(tag, "BODY") == 0) return OMNI_BLOCK_BODY;
    if (strcmp(tag, "CONTENT") == 0) return OMNI_BLOCK_CONTENT;
    if (strcmp(tag, "CLOSING") == 0) return OMNI_BLOCK_CLOSING;
    if (strcmp(tag, "FOOTER") == 0) return OMNI_BLOCK_FOOTER;
    return OMNI_BLOCK_BODY;  // Default
}

mos_result_t omni_parse_buffer(const char* buffer, mos_size_t size,
                                OmniParsedFile* out) {
    if (!buffer || !out) return MOS_ERR_INVALID;

    memset(out, 0, sizeof(OmniParsedFile));

    const char* line_start = buffer;
    const char* buffer_end = buffer + size;
    int32_t line_number = 0;
    OmniBlock* current_block = NULL;

    while (line_start < buffer_end) {
        line_number++;

        // Find line end
        const char* line_end = line_start;
        while (line_end < buffer_end && *line_end != '\n') {
            line_end++;
        }

        // Copy line to temporary buffer
        char line[OMNI_MAX_LINE_LEN];
        size_t line_len = line_end - line_start;
        if (line_len >= OMNI_MAX_LINE_LEN) line_len = OMNI_MAX_LINE_LEN - 1;
        memcpy(line, line_start, line_len);
        line[line_len] = '\0';

        // Skip leading whitespace for parsing
        const char* trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;

        // Statistics
        out->total_lines++;
        if (line_len == 0 || (trimmed[0] == '\0')) {
            out->blank_lines++;
        } else if (starts_with(trimmed, "///") || starts_with(trimmed, "//") ||
                   starts_with(trimmed, "/*") || starts_with(trimmed, "*")) {
            out->comment_lines++;
        } else {
            out->code_lines++;
        }

        // Parse pragma line
        if (starts_with(trimmed, "/// @omni ") || starts_with(trimmed, "/// @omni\t")) {
            // Format line: @omni code --c --source
            const char* args = trimmed + 10;
            sscanf(args, "%31s", out->format);
            // Parse --language and --type flags
            char* lang_ptr = strstr(args, "--");
            if (lang_ptr) {
                lang_ptr += 2;
                sscanf(lang_ptr, "%15s", out->language);
                char* type_ptr = strstr(lang_ptr, "--");
                if (type_ptr) {
                    type_ptr += 2;
                    sscanf(type_ptr, "%15s", out->filetype);
                }
            }
        }

        // Parse field (@omni:marker key = value)
        if (starts_with(trimmed, "/// @omni:") && out->field_count < OMNI_MAX_FIELDS) {
            OmniField* field = &out->fields[out->field_count];
            field->line_number = line_number;

            char marker[8];
            char key[32];
            char value[128] = {0};

            // Parse: @omni:marker key = value
            if (sscanf(trimmed + 10, "%7s %31s = %127[^\n]", marker, key, value) >= 2) {
                field->marker = parse_marker(marker);
                strncpy(field->key, key, 31);
                strncpy(field->value, value, 127);
                out->field_count++;
            }
        }

        // Parse block start
        if (strstr(trimmed, "BLOCK [") != NULL) {
            char tag[32];
            if (sscanf(strstr(trimmed, "[") + 1, "%31[^]]", tag) == 1) {
                if (out->block_count < OMNI_MAX_BLOCKS) {
                    current_block = &out->blocks[out->block_count];
                    current_block->type = parse_block_type(tag);
                    current_block->start_line = line_number;
                    out->block_count++;
                }
            }
        }

        // Parse block end
        if (starts_with(trimmed, "/// END ") && current_block) {
            current_block->end_line = line_number;
            current_block->content_lines = line_number - current_block->start_line - 1;
            current_block = NULL;
        }

        // Move to next line
        line_start = line_end + 1;
    }

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.4 Viewer Implementation
// -----------------------------------------------------------------------------

mos_result_t omni_viewer_init(OmniViewerState* state) {
    if (!state) return MOS_ERR_INVALID;

    memset(state, 0, sizeof(OmniViewerState));
    state->mode = OMNI_VIEW_STRUCTURE;
    state->selected_field = -1;
    state->selected_block = -1;
    state->show_explanations = true;

    return MOS_OK;
}

void omni_viewer_set_mode(OmniViewerState* state, OmniViewMode mode) {
    if (state) {
        state->mode = mode;
    }
}

// -----------------------------------------------------------------------------
// B.5 Rendering Implementation
// -----------------------------------------------------------------------------

void omni_viewer_render(OmniViewerState* state, MosGraphicsContext* ctx,
                         MosRect bounds) {
    if (!state || !ctx || !state->file) return;

    int32_t x = bounds.x + 20;
    int32_t y = bounds.y + 20;

    // Title
    mos_gfx_string(ctx, x, y, "OmniCode Viewer", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
    y += 25;

    // File info
    char info[128];
    snprintf(info, sizeof(info), "Format: %s | Language: %s | Type: %s",
             state->file->format, state->file->language, state->file->filetype);
    mos_gfx_string(ctx, x, y, info, MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
    y += 30;

    // Mode indicator
    const char* mode_names[] = {"STRUCTURE", "PRAGMA", "BLOCKS", "LEARNING"};
    snprintf(info, sizeof(info), "[Mode: %s]", mode_names[state->mode]);
    mos_gfx_string(ctx, x, y, info, MOS_COLOR_INFO, MOS_COLOR_BG_DARK);
    y += 25;

    switch (state->mode) {
        case OMNI_VIEW_STRUCTURE:
            // Show block structure visually
            mos_gfx_string(ctx, x, y, "BLOCK STRUCTURE:", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
            y += 20;

            for (mos_size_t i = 0; i < state->file->block_count; i++) {
                OmniBlock* blk = &state->file->blocks[i];
                MosColor color = OMNI_BLOCK_COLORS[blk->type];

                // Draw block bar
                int32_t bar_width = blk->content_lines * 3;
                if (bar_width < 20) bar_width = 20;
                if (bar_width > 300) bar_width = 300;

                MosRect bar = {x, y, bar_width, 18};
                mos_gfx_fill_rect(ctx, bar, color);
                mos_gfx_draw_rect(ctx, bar, MOS_COLOR_TEXT_DIM, 1);

                // Block name
                snprintf(info, sizeof(info), " %s (%d lines)",
                         OMNI_BLOCK_NAMES[blk->type], blk->content_lines);
                mos_gfx_string(ctx, x + bar_width + 10, y + 2, info,
                               MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
                y += 25;
            }
            break;

        case OMNI_VIEW_PRAGMA:
            // Show pragma fields with color-coded markers
            mos_gfx_string(ctx, x, y, "PRAGMA FIELDS:", MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
            y += 20;

            for (mos_size_t i = 0; i < state->file->field_count; i++) {
                OmniField* fld = &state->file->fields[i];
                MosColor marker_color = OMNI_MARKER_COLORS[fld->marker];

                // Marker indicator
                snprintf(info, sizeof(info), "[%s]", OMNI_MARKER_NAMES[fld->marker]);
                mos_gfx_string(ctx, x, y, info, marker_color, MOS_COLOR_BG_DARK);

                // Key = Value
                snprintf(info, sizeof(info), " %s = %s", fld->key, fld->value);
                mos_gfx_string(ctx, x + 50, y, info, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);

                y += 18;
                if (y > bounds.y + bounds.height - 40) break;
            }
            break;

        case OMNI_VIEW_LEARNING:
            // Learning mode with explanations
            mos_gfx_string(ctx, x, y, "LEARNING MODE", MOS_COLOR_GOLD, MOS_COLOR_BG_DARK);
            y += 20;

            if (state->selected_block >= 0 &&
                (mos_size_t)state->selected_block < state->file->block_count) {
                OmniBlockType bt = state->file->blocks[state->selected_block].type;
                const char* explanation = BLOCK_EXPLANATIONS[bt];

                mos_gfx_string(ctx, x, y, OMNI_BLOCK_NAMES[bt], MOS_COLOR_INFO, MOS_COLOR_BG_DARK);
                y += 25;

                // Render explanation line by line
                const char* line_start = explanation;
                while (*line_start) {
                    const char* line_end = line_start;
                    while (*line_end && *line_end != '\n') line_end++;

                    char line_buf[80];
                    size_t len = line_end - line_start;
                    if (len > 79) len = 79;
                    memcpy(line_buf, line_start, len);
                    line_buf[len] = '\0';

                    mos_gfx_string(ctx, x, y, line_buf, MOS_COLOR_TEXT, MOS_COLOR_BG_DARK);
                    y += 16;

                    line_start = *line_end ? line_end + 1 : line_end;
                    if (y > bounds.y + bounds.height - 40) break;
                }
            } else {
                mos_gfx_string(ctx, x, y, "Select a block (1-8) to learn about it",
                               MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
            }
            break;

        default:
            break;
    }

    // Footer with controls
    y = bounds.y + bounds.height - 30;
    mos_gfx_string(ctx, x, y, "[S]tructure  [P]ragma  [L]earning  [1-8] Select block  [ESC] Exit",
                   MOS_COLOR_TEXT_DIM, MOS_COLOR_BG_DARK);
}

// -----------------------------------------------------------------------------
// B.6 Input Handling
// -----------------------------------------------------------------------------

bool omni_viewer_input(OmniViewerState* state, const MosEvent* event) {
    if (!state || !event) return false;

    if (event->type == MOS_EVENT_KEY) {
        char c = event->key.character;
        switch (c) {
            case 's': case 'S':
                state->mode = OMNI_VIEW_STRUCTURE;
                return true;
            case 'p': case 'P':
                state->mode = OMNI_VIEW_PRAGMA;
                return true;
            case 'l': case 'L':
                state->mode = OMNI_VIEW_LEARNING;
                return true;
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8':
                state->selected_block = c - '1';
                return true;
            default:
                break;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------
// B.7 Explanation Functions
// -----------------------------------------------------------------------------

const char* omni_explain_pragma(OmniPragmaSection section) {
    if (section >= OMNI_PRAGMA_COUNT) return "Unknown pragma section.";
    return PRAGMA_EXPLANATIONS[section];
}

const char* omni_explain_block(OmniBlockType block) {
    if (block >= OMNI_BLOCK_COUNT) return "Unknown block type.";
    return BLOCK_EXPLANATIONS[block];
}

const char* omni_explain_marker(OmniMarkerType marker) {
    if (marker >= OMNI_MARKER_COUNT) return "Unknown marker type.";
    return MARKER_EXPLANATIONS[marker];
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Usage:
//   OmniViewerState viewer;
//   OmniParsedFile file;
//
//   omni_viewer_init(&viewer);
//   omni_parse_buffer(source, len, &file);
//   viewer.file = &file;
//
//   // In render loop:
//   omni_viewer_render(&viewer, ctx, bounds);
//
//   // Handle input:
//   omni_viewer_input(&viewer, &event);
//
// Controls:
//   S - Structure view (see blocks visually)
//   P - Pragma view (see all fields)
//   L - Learning mode (explanations)
//   1-8 - Select a block to learn about
//
// =============================================================================
// END CLOSING
// =============================================================================
