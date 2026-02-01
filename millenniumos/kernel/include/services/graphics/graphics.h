// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-graphics
// Title:       MillenniumOS Graphics Subsystem
// Type:        Header
// Component:   Stage 3 (Framework) / Graphics
// Role:        Drawing primitives, text rendering, image handling
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:16 — "And God made two great lights"
//              Graphics brings light to the display.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_GRAPHICS_H
#define MILLENNIUMOS_GRAPHICS_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"
#include "hal/hal.h"
#include "trit/shared/palette.h"  // Kingdom Technology unified palette

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Color Types
// -----------------------------------------------------------------------------
//
// Uses unified Kingdom Technology palette from trit/shared/palette.h
// MOS_* aliases provided for compatibility
//

typedef kt_color_t MosColor;

// Color creation macros (alias to KT_* macros)
#define MOS_RGB(r, g, b)        KT_RGB(r, g, b)
#define MOS_RGBA(r, g, b, a)    KT_RGBA(r, g, b, a)

// Color extraction (alias to KT_* macros)
#define MOS_RED(c)              KT_RED(c)
#define MOS_GREEN(c)            KT_GREEN(c)
#define MOS_BLUE(c)             KT_BLUE(c)
#define MOS_ALPHA(c)            KT_ALPHA(c)

// Standard colors (alias to Kingdom Technology palette)
#define MOS_COLOR_BLACK         KT_COLOR_BLACK
#define MOS_COLOR_WHITE         KT_COLOR_WHITE
#define MOS_COLOR_RED           KT_COLOR_RED
#define MOS_COLOR_GREEN         KT_COLOR_GREEN
#define MOS_COLOR_BLUE          KT_COLOR_BLUE
#define MOS_COLOR_YELLOW        KT_COLOR_YELLOW
#define MOS_COLOR_CYAN          KT_COLOR_CYAN
#define MOS_COLOR_MAGENTA       KT_COLOR_MAGENTA

// Kingdom zone colors
#define MOS_COLOR_VOID          KT_COLOR_VOID
#define MOS_COLOR_WORD          KT_COLOR_WORD
#define MOS_COLOR_TOV           KT_COLOR_TOV
#define MOS_COLOR_GOLD          KT_COLOR_GOLD
#define MOS_COLOR_PURPLE        KT_COLOR_PURPLE

// UI colors
#define MOS_COLOR_BG_DARK       KT_COLOR_BG_DARK
#define MOS_COLOR_BG_LIGHT      KT_COLOR_BG_LIGHT
#define MOS_COLOR_ACCENT        KT_COLOR_ACCENT
#define MOS_COLOR_TEXT          KT_COLOR_TEXT
#define MOS_COLOR_TEXT_DIM      KT_COLOR_TEXT_DIM

// Additional colors from shared palette
#define MOS_COLOR_SUCCESS       KT_COLOR_SUCCESS
#define MOS_COLOR_WARNING       KT_COLOR_WARNING
#define MOS_COLOR_ERROR         KT_COLOR_ERROR
#define MOS_COLOR_INFO          KT_COLOR_INFO

// -----------------------------------------------------------------------------
// B.2 Geometry Types
// -----------------------------------------------------------------------------

typedef struct MosPoint {
    int32_t x;
    int32_t y;
} MosPoint;

typedef struct MosSize {
    int32_t width;
    int32_t height;
} MosSize;

typedef struct MosRect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} MosRect;

// Rect helpers
#define MOS_RECT(x, y, w, h)    ((MosRect){(x), (y), (w), (h)})
#define MOS_RECT_RIGHT(r)       ((r).x + (r).width)
#define MOS_RECT_BOTTOM(r)      ((r).y + (r).height)

static inline bool mos_rect_contains(MosRect r, int32_t x, int32_t y) {
    return x >= r.x && x < MOS_RECT_RIGHT(r) &&
           y >= r.y && y < MOS_RECT_BOTTOM(r);
}

static inline bool mos_rect_intersects(MosRect a, MosRect b) {
    return a.x < MOS_RECT_RIGHT(b) && MOS_RECT_RIGHT(a) > b.x &&
           a.y < MOS_RECT_BOTTOM(b) && MOS_RECT_BOTTOM(a) > b.y;
}

// -----------------------------------------------------------------------------
// B.3 Font Types
// -----------------------------------------------------------------------------

#define MOS_FONT_CHAR_WIDTH     8
#define MOS_FONT_CHAR_HEIGHT    16
#define MOS_FONT_FIRST_CHAR     32      // Space
#define MOS_FONT_LAST_CHAR      126     // Tilde
#define MOS_FONT_CHAR_COUNT     (MOS_FONT_LAST_CHAR - MOS_FONT_FIRST_CHAR + 1)

typedef struct MosFont {
    uint8_t         width;          // Character width
    uint8_t         height;         // Character height
    uint8_t         first_char;     // First character code
    uint8_t         char_count;     // Number of characters
    const uint8_t*  data;           // Bitmap data
} MosFont;

// Built-in fonts
extern const MosFont MOS_FONT_DEFAULT;    // 8x16 - default system font
extern const MosFont MOS_FONT_COMPACT;    // 6x12 - compact font for small text

// -----------------------------------------------------------------------------
// B.4 Graphics Context
// -----------------------------------------------------------------------------

typedef struct MosGraphicsContext {
    // Target
    mos_vaddr_t     framebuffer;    // Framebuffer address
    uint32_t        width;          // Width in pixels
    uint32_t        height;         // Height in pixels
    uint32_t        pitch;          // Bytes per row
    uint8_t         bpp;            // Bits per pixel

    // Color format
    bool            swap_rb;        // Swap R/B channels? (true for RGB framebuffers)

    // Drawing state
    MosColor        fg_color;       // Foreground color
    MosColor        bg_color;       // Background color
    const MosFont*  font;           // Current font
    MosRect         clip;           // Clipping rectangle

    // Cursor position (for text)
    int32_t         cursor_x;
    int32_t         cursor_y;
} MosGraphicsContext;

// -----------------------------------------------------------------------------
// B.5 Initialization
// -----------------------------------------------------------------------------

// Initialize graphics subsystem
mos_result_t mos_graphics_init(void);

// Create graphics context from display
mos_result_t mos_graphics_create_context(MosGraphicsContext* ctx, uint8_t display);

// Create context for custom buffer
mos_result_t mos_graphics_create_buffer_context(MosGraphicsContext* ctx,
                                                  mos_vaddr_t buffer,
                                                  uint32_t width, uint32_t height,
                                                  uint32_t pitch, uint8_t bpp);

// -----------------------------------------------------------------------------
// B.6 Basic Drawing
// -----------------------------------------------------------------------------

// Clear entire context
void mos_gfx_clear(MosGraphicsContext* ctx, MosColor color);

// Set single pixel
void mos_gfx_pixel(MosGraphicsContext* ctx, int32_t x, int32_t y, MosColor color);

// Get pixel color
MosColor mos_gfx_get_pixel(MosGraphicsContext* ctx, int32_t x, int32_t y);

// Draw horizontal line (fast)
void mos_gfx_hline(MosGraphicsContext* ctx, int32_t x, int32_t y,
                   int32_t width, MosColor color);

// Draw vertical line
void mos_gfx_vline(MosGraphicsContext* ctx, int32_t x, int32_t y,
                   int32_t height, MosColor color);

// Draw line (Bresenham)
void mos_gfx_line(MosGraphicsContext* ctx, int32_t x1, int32_t y1,
                  int32_t x2, int32_t y2, MosColor color);

// -----------------------------------------------------------------------------
// B.7 Rectangle Drawing
// -----------------------------------------------------------------------------

// Draw filled rectangle
void mos_gfx_fill_rect(MosGraphicsContext* ctx, MosRect rect, MosColor color);

// Draw rectangle outline
void mos_gfx_draw_rect(MosGraphicsContext* ctx, MosRect rect,
                       MosColor color, int32_t thickness);

// Draw rounded rectangle
void mos_gfx_rounded_rect(MosGraphicsContext* ctx, MosRect rect,
                          int32_t radius, MosColor fill, MosColor border);

// Draw gradient rectangle (vertical)
void mos_gfx_gradient_rect(MosGraphicsContext* ctx, MosRect rect,
                           MosColor top, MosColor bottom);

// -----------------------------------------------------------------------------
// B.8 Circle Drawing
// -----------------------------------------------------------------------------

// Draw filled circle
void mos_gfx_fill_circle(MosGraphicsContext* ctx, int32_t cx, int32_t cy,
                         int32_t radius, MosColor color);

// Draw circle outline
void mos_gfx_draw_circle(MosGraphicsContext* ctx, int32_t cx, int32_t cy,
                         int32_t radius, MosColor color);

// -----------------------------------------------------------------------------
// B.9 Text Drawing
// -----------------------------------------------------------------------------

// Set current font
void mos_gfx_set_font(MosGraphicsContext* ctx, const MosFont* font);

// Draw single character
void mos_gfx_char(MosGraphicsContext* ctx, int32_t x, int32_t y,
                  char c, MosColor fg, MosColor bg);

// Draw string
void mos_gfx_string(MosGraphicsContext* ctx, int32_t x, int32_t y,
                    const char* str, MosColor fg, MosColor bg);

// Draw string with length limit
void mos_gfx_string_n(MosGraphicsContext* ctx, int32_t x, int32_t y,
                      const char* str, mos_size_t len, MosColor fg, MosColor bg);

// Measure string width
int32_t mos_gfx_string_width(MosGraphicsContext* ctx, const char* str);

// Draw centered string
void mos_gfx_string_centered(MosGraphicsContext* ctx, MosRect rect,
                              const char* str, MosColor fg, MosColor bg);

// -----------------------------------------------------------------------------
// B.10 Image/Blit Operations
// -----------------------------------------------------------------------------

// Copy rectangular region
void mos_gfx_blit(MosGraphicsContext* dst, int32_t dx, int32_t dy,
                  MosGraphicsContext* src, MosRect src_rect);

// Copy with transparency (color key)
void mos_gfx_blit_transparent(MosGraphicsContext* dst, int32_t dx, int32_t dy,
                               MosGraphicsContext* src, MosRect src_rect,
                               MosColor transparent);

// Fill pattern
void mos_gfx_fill_pattern(MosGraphicsContext* ctx, MosRect rect,
                           const uint8_t* pattern, int32_t pattern_size,
                           MosColor fg, MosColor bg);

// -----------------------------------------------------------------------------
// B.11 Clipping
// -----------------------------------------------------------------------------

// Set clipping rectangle
void mos_gfx_set_clip(MosGraphicsContext* ctx, MosRect clip);

// Reset clipping to full context
void mos_gfx_reset_clip(MosGraphicsContext* ctx);

// Push clip (intersect with current)
void mos_gfx_push_clip(MosGraphicsContext* ctx, MosRect clip);

// -----------------------------------------------------------------------------
// B.12 Utility
// -----------------------------------------------------------------------------

// Blend two colors
MosColor mos_gfx_blend(MosColor fg, MosColor bg, uint8_t alpha);

// Darken color
MosColor mos_gfx_darken(MosColor color, uint8_t amount);

// Lighten color
MosColor mos_gfx_lighten(MosColor color, uint8_t amount);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Graphics subsystem provides:
//   - Color types with Kingdom Technology palette
//   - Basic drawing primitives (pixel, line, rect, circle)
//   - Text rendering with bitmap fonts
//   - Blit operations for images
//   - Clipping support
//
// "And God made two great lights; the greater light to rule the day,
//  and the lesser light to rule the night: he made the stars also."
// — Genesis 1:16
//
// Graphics brings light to the display, making the system visible.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // MILLENNIUMOS_GRAPHICS_H
