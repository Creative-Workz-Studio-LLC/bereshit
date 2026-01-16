// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide-panel
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_ide_panel.h — OmniCode IDE Panel System
// Sidebar panels for navigation, structure, and validation

#ifndef BERESHIT_OMNI_IDE_PANEL_H
#define BERESHIT_OMNI_IDE_PANEL_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-panel
// Title: OmniCode IDE Panel System
// Type: Header
// Role: Panel infrastructure for file browser, block outline, validation

// Grounding: Genesis 1:1 - "In the beginning God created"
// Principle: Structure enables navigation - know WHERE you are
// Anchor: Boundaries before content - panels show structure

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_ide.h"
#include <stdbool.h>
#include <stdint.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Panel Types [TYPES]

typedef enum {
    PANEL_NONE = 0,
    PANEL_FILE_BROWSER,     // void/word/tov navigation
    PANEL_BLOCK_OUTLINE,    // Document structure
    PANEL_VALIDATION,       // Diagnostics and issues
    PANEL_TEMPLATE_INFO,    // Derivation chain
    PANEL_COUNT
} IDEPanelType;

typedef enum {
    PANEL_POS_LEFT,
    PANEL_POS_RIGHT,
    PANEL_POS_BOTTOM
} IDEPanelPosition;

// # B.2 File Browser [FILE_BROWSER]

#define FILE_BROWSER_MAX_PATH   512
#define FILE_BROWSER_MAX_ITEMS  256

typedef enum {
    FILE_ITEM_DIR,
    FILE_ITEM_FILE,
    FILE_ITEM_ZONE_VOID,    // void/ directory (entry gate)
    FILE_ITEM_ZONE_WORD,    // word/ directory (processing)
    FILE_ITEM_ZONE_TOV      // tov/ directory (exit gate)
} IDEFileItemType;

typedef struct {
    char name[256];         // Max filename length on most filesystems
    char path[FILE_BROWSER_MAX_PATH];
    IDEFileItemType type;
    bool is_omni;           // Is .omni file?
    bool expanded;          // If directory, is it expanded?
    int depth;              // Nesting level
} IDEFileItem;

typedef struct {
    char root_path[FILE_BROWSER_MAX_PATH];  // Bereshit root
    IDEFileItem items[FILE_BROWSER_MAX_ITEMS];
    uint32_t item_count;
    uint32_t selected;      // Currently selected item
    uint32_t scroll;        // Scroll offset
} IDEFileBrowser;

// # B.3 Block Outline [BLOCK_OUTLINE]

#define BLOCK_OUTLINE_MAX_ITEMS 32

typedef struct {
    char label[64];         // Block name (e.g., "METADATA", "SETUP")
    IDEBlockType type;
    uint32_t start_line;
    uint32_t end_line;
    uint32_t line_count;
} IDEBlockItem;

typedef struct {
    IDEBlockItem items[BLOCK_OUTLINE_MAX_ITEMS];
    uint32_t item_count;
    uint32_t selected;      // Currently selected block
    IDEBlockStructure structure;  // 3/4/5-block
} IDEBlockOutline;

// # B.4 Validation Panel [VALIDATION]

typedef struct {
    IDEDiagnostics* diags;
    uint32_t scroll;
} IDEValidationPanel;

// # B.5 Template Info [TEMPLATE_INFO]

typedef struct {
    char key[128];              // meta.key value
    char derives_from[256];     // meta.from value
    char pragma_line[256];      // First pragma line
    IDEBlockStructure structure;
    bool valid;
} IDETemplateInfo;

// # B.6 Panel Container [CONTAINER]

typedef struct {
    IDEPanelType type;
    IDEPanelPosition position;
    int32_t width;          // Width in columns
    int32_t height;         // Height in rows (for bottom panels)
    bool visible;
    bool focused;

    // Panel-specific data
    union {
        IDEFileBrowser file_browser;
        IDEBlockOutline block_outline;
        IDEValidationPanel validation;
        IDETemplateInfo template_info;
    } data;
} IDEPanel;

// # B.7 Layout System [LAYOUT]

#define IDE_MAX_PANELS 4

typedef struct {
    IDEPanel panels[IDE_MAX_PANELS];
    uint32_t panel_count;
    int32_t left_width;     // Total left panel width
    int32_t right_width;    // Total right panel width
    int32_t bottom_height;  // Total bottom panel height
} IDELayout;

// # B.8 Panel Lifecycle [LIFECYCLE]

// Create panel of specific type
IDEPanel* ide_panel_create(IDEPanelType type, IDEPanelPosition pos, int32_t size);

// Free panel resources
void ide_panel_free(IDEPanel* panel);

// Create default layout (file browser left, block outline left below)
IDELayout* ide_layout_create_default(void);

// Free layout
void ide_layout_free(IDELayout* layout);

// # B.9 File Browser Operations [FILE_OPS]

// Initialize file browser at Bereshit root
void ide_file_browser_init(IDEFileBrowser* browser, const char* bereshit_root);

// Refresh file list
void ide_file_browser_refresh(IDEFileBrowser* browser);

// Navigate selection
void ide_file_browser_up(IDEFileBrowser* browser);
void ide_file_browser_down(IDEFileBrowser* browser);

// Toggle directory expansion
void ide_file_browser_toggle(IDEFileBrowser* browser);

// Get selected file path (returns NULL if directory selected)
const char* ide_file_browser_get_selected_path(IDEFileBrowser* browser);

// # B.10 Block Outline Operations [BLOCK_OPS]

// Update outline from buffer
void ide_block_outline_update(IDEBlockOutline* outline, IDEBuffer* buffer);

// Navigate to block
void ide_block_outline_up(IDEBlockOutline* outline);
void ide_block_outline_down(IDEBlockOutline* outline);

// Get selected block start line
uint32_t ide_block_outline_get_line(IDEBlockOutline* outline);

// # B.11 Template Info Operations [TEMPLATE_OPS]

// Extract template info from buffer
void ide_template_info_update(IDETemplateInfo* info, IDEBuffer* buffer);

// # B.12 Panel Drawing [DRAWING]

// Draw panel to ncurses window at position
void ide_panel_draw(IDEPanel* panel, int32_t x, int32_t y, int32_t w, int32_t h);

// Draw file browser
void ide_panel_draw_file_browser(IDEFileBrowser* browser, int32_t x, int32_t y, int32_t w, int32_t h, bool focused);

// Draw block outline
void ide_panel_draw_block_outline(IDEBlockOutline* outline, int32_t x, int32_t y, int32_t w, int32_t h, bool focused);

// Draw validation panel
void ide_panel_draw_validation(IDEValidationPanel* validation, int32_t x, int32_t y, int32_t w, int32_t h);

// Draw template info
void ide_panel_draw_template_info(IDETemplateInfo* info, int32_t x, int32_t y, int32_t w, int32_t h);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "In the beginning God created the heaven and the earth." — Genesis 1:1
// Panels show structure - WHERE you are, WHAT you're editing, WHERE it came from.

#endif // BERESHIT_OMNI_IDE_PANEL_H

// =============================================================================
// END CLOSING
// =============================================================================
