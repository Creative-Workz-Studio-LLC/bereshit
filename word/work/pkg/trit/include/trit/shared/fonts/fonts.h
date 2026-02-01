// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         TRIT-shared-fonts
// Title:       Kingdom Technology Bitmap Font System
// Type:        Header
// Component:   libtrit/shared
// Role:        Unified bitmap font definitions for CLI/TUI/GUI rendering
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Proverbs 25:11 — "A word fitly spoken is like apples of gold"
// Principle:   Words communicate truth; fonts make words visible
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef TRIT_SHARED_FONTS_H
#define TRIT_SHARED_FONTS_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include "trit/shared/palette.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Font Types
// -----------------------------------------------------------------------------

// Font style flags (ternary-based)
typedef enum {
    KT_FONT_STYLE_REGULAR   = 0,
    KT_FONT_STYLE_BOLD      = 1,
    KT_FONT_STYLE_ITALIC    = -1,
} KtFontStyle;

// Font size presets (Kingdom standard sizes)
typedef enum {
    KT_FONT_SIZE_TINY       = 6,    // 6x12 compact
    KT_FONT_SIZE_SMALL      = 8,    // 8x16 standard
    KT_FONT_SIZE_MEDIUM     = 10,   // 10x20 medium
    KT_FONT_SIZE_LARGE      = 12,   // 12x24 large
    KT_FONT_SIZE_TITLE      = 16,   // 16x32 title
} KtFontSize;

// Bitmap font descriptor
typedef struct {
    uint8_t         width;          // Character width in pixels
    uint8_t         height;         // Character height in pixels
    uint8_t         first_char;     // First ASCII character code (usually 32)
    uint8_t         char_count;     // Number of characters in font
    const uint8_t*  data;           // Pointer to bitmap data
    KtFontStyle     style;          // Font style
    const char*     name;           // Human-readable name
} KtBitmapFont;

// -----------------------------------------------------------------------------
// B.2 Font Data Includes
// -----------------------------------------------------------------------------

// Include all bitmap font data
#include "trit/shared/fonts/font_8x16.h"
#include "trit/shared/fonts/font_6x12.h"

// -----------------------------------------------------------------------------
// B.3 Pre-defined Fonts
// -----------------------------------------------------------------------------

// Standard system font (8x16)
static const KtBitmapFont KT_FONT_STANDARD = {
    .width = 8,
    .height = 16,
    .first_char = 32,
    .char_count = 95,
    .data = kt_font_8x16_data,
    .style = KT_FONT_STYLE_REGULAR,
    .name = "System 8x16"
};

// Compact font (6x12) for small text
static const KtBitmapFont KT_FONT_COMPACT = {
    .width = 6,
    .height = 12,
    .first_char = 32,
    .char_count = 95,
    .data = kt_font_6x12_data,
    .style = KT_FONT_STYLE_REGULAR,
    .name = "Compact 6x12"
};

// -----------------------------------------------------------------------------
// B.4 Font Utilities
// -----------------------------------------------------------------------------

// Calculate text width in pixels
static inline int32_t kt_font_text_width(const KtBitmapFont* font, const char* text) {
    if (!font || !text) return 0;
    int32_t len = 0;
    while (text[len]) len++;
    return len * font->width;
}

// Calculate text height in pixels
static inline int32_t kt_font_text_height(const KtBitmapFont* font) {
    return font ? font->height : 0;
}

// Get character bitmap data offset
static inline const uint8_t* kt_font_char_data(const KtBitmapFont* font, char c) {
    if (!font || !font->data) return NULL;
    int idx = c - font->first_char;
    if (idx < 0 || idx >= font->char_count) return NULL;
    return font->data + (idx * font->height);
}

// Check if font supports character
static inline bool kt_font_has_char(const KtBitmapFont* font, char c) {
    if (!font) return false;
    int idx = c - font->first_char;
    return idx >= 0 && idx < font->char_count;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Kingdom Technology Bitmap Font System provides:
//   - Unified font structure for CLI/TUI/GUI
//   - Standard 8x16 and compact 6x12 fonts
//   - Font measurement utilities
//   - Shared across MillenniumOS and Cornerstone
//
// "A word fitly spoken is like apples of gold in pictures of silver."
// — Proverbs 25:11
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // TRIT_SHARED_FONTS_H
