// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-graphics-impl
// Title:       MillenniumOS Graphics Implementation
// Type:        Source
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
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "services/graphics/graphics.h"
#include "trit/shared/fonts/fonts.h"  // Kingdom Technology unified fonts

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Built-in Font (8x16)
// -----------------------------------------------------------------------------

// Standard 8x16 font - uses Kingdom Technology shared fonts
const MosFont MOS_FONT_DEFAULT = {
    .width = 8,
    .height = 16,
    .first_char = 32,
    .char_count = 95,
    .data = kt_font_8x16_data
};

// Compact 6x12 font for status bars, labels, small text
const MosFont MOS_FONT_COMPACT = {
    .width = 6,
    .height = 12,
    .first_char = 32,
    .char_count = 95,
    .data = kt_font_6x12_data
};

// -----------------------------------------------------------------------------
// B.2 Helper Functions
// -----------------------------------------------------------------------------

// Clamp value to range
static inline int32_t gfx_clamp(int32_t val, int32_t min, int32_t max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

// Absolute value
static inline int32_t gfx_abs(int32_t val) {
    return (val < 0) ? -val : val;
}

// Swap red and blue channels in a color (for RGB format framebuffers)
// KT_RGB produces 0xAARRGGBB; RGB framebuffers expect 0xAABBGGRR
static inline uint32_t gfx_swap_rb(uint32_t color) {
    return (color & 0xFF00FF00) |                  // Keep A and G
           ((color & 0x00FF0000) >> 16) |          // Move R to B position
           ((color & 0x000000FF) << 16);           // Move B to R position
}

// Check if point is in clip region
static inline bool gfx_in_clip(MosGraphicsContext* ctx, int32_t x, int32_t y) {
    return x >= ctx->clip.x && x < MOS_RECT_RIGHT(ctx->clip) &&
           y >= ctx->clip.y && y < MOS_RECT_BOTTOM(ctx->clip);
}

// Set pixel (32bpp)
// Handles color format conversion for RGB framebuffers (swap R/B)
static inline void set_pixel_32(MosGraphicsContext* ctx, int32_t x, int32_t y, MosColor color) {
    if (!gfx_in_clip(ctx, x, y)) return;
    uint32_t* fb = (uint32_t*)(ctx->framebuffer + y * ctx->pitch + x * 4);
    *fb = ctx->swap_rb ? gfx_swap_rb(color) : color;
}

// Get pixel (32bpp)
static inline MosColor get_pixel_32(MosGraphicsContext* ctx, int32_t x, int32_t y) {
    if (!gfx_in_clip(ctx, x, y)) return 0;
    uint32_t* fb = (uint32_t*)(ctx->framebuffer + y * ctx->pitch + x * 4);
    return *fb;
}

// -----------------------------------------------------------------------------
// B.3 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_graphics_init(void) {
    // Nothing to initialize globally yet
    return MOS_OK;
}

mos_result_t mos_graphics_create_context(MosGraphicsContext* ctx, uint8_t display) {
    if (!ctx) return MOS_ERR_INVALID;

    const MosDisplay* disp = mos_hal_display(display);
    if (!disp) return MOS_ERR_NOTFOUND;

    ctx->framebuffer = disp->mapped_fb;
    ctx->width = disp->mode.width;
    ctx->height = disp->mode.height;
    ctx->pitch = disp->mode.pitch;
    ctx->bpp = disp->mode.bpp;
    ctx->swap_rb = disp->swap_rb;  // Color format from display

    ctx->fg_color = MOS_COLOR_TEXT;
    ctx->bg_color = MOS_COLOR_BG_DARK;
    ctx->font = &MOS_FONT_DEFAULT;

    ctx->clip.x = 0;
    ctx->clip.y = 0;
    ctx->clip.width = ctx->width;
    ctx->clip.height = ctx->height;

    ctx->cursor_x = 0;
    ctx->cursor_y = 0;

    return MOS_OK;
}

mos_result_t mos_graphics_create_buffer_context(MosGraphicsContext* ctx,
                                                  mos_vaddr_t buffer,
                                                  uint32_t width, uint32_t height,
                                                  uint32_t pitch, uint8_t bpp) {
    if (!ctx || !buffer) return MOS_ERR_INVALID;

    ctx->framebuffer = buffer;
    ctx->width = width;
    ctx->height = height;
    ctx->pitch = pitch;
    ctx->bpp = bpp;
    ctx->swap_rb = false;  // Memory buffers always use standard KT_RGB format

    ctx->fg_color = MOS_COLOR_TEXT;
    ctx->bg_color = MOS_COLOR_BG_DARK;
    ctx->font = &MOS_FONT_DEFAULT;

    ctx->clip.x = 0;
    ctx->clip.y = 0;
    ctx->clip.width = width;
    ctx->clip.height = height;

    ctx->cursor_x = 0;
    ctx->cursor_y = 0;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.4 Basic Drawing
// -----------------------------------------------------------------------------

void mos_gfx_clear(MosGraphicsContext* ctx, MosColor color) {
    if (!ctx) return;
    mos_gfx_fill_rect(ctx, ctx->clip, color);
}

void mos_gfx_pixel(MosGraphicsContext* ctx, int32_t x, int32_t y, MosColor color) {
    if (!ctx) return;
    if (ctx->bpp == 32) {
        set_pixel_32(ctx, x, y, color);
    }
    // TODO: Support other bpp
}

MosColor mos_gfx_get_pixel(MosGraphicsContext* ctx, int32_t x, int32_t y) {
    if (!ctx) return 0;
    if (ctx->bpp == 32) {
        return get_pixel_32(ctx, x, y);
    }
    return 0;
}

void mos_gfx_hline(MosGraphicsContext* ctx, int32_t x, int32_t y,
                   int32_t width, MosColor color) {
    if (!ctx || width <= 0) return;
    if (y < ctx->clip.y || y >= MOS_RECT_BOTTOM(ctx->clip)) return;

    // Clip x range
    int32_t x1 = gfx_clamp(x, ctx->clip.x, MOS_RECT_RIGHT(ctx->clip) - 1);
    int32_t x2 = gfx_clamp(x + width - 1, ctx->clip.x, MOS_RECT_RIGHT(ctx->clip) - 1);
    if (x1 > x2) return;

    if (ctx->bpp == 32) {
        uint32_t* fb = (uint32_t*)(ctx->framebuffer + y * ctx->pitch + x1 * 4);
        for (int32_t i = x1; i <= x2; i++) {
            *fb++ = color;
        }
    }
}

void mos_gfx_vline(MosGraphicsContext* ctx, int32_t x, int32_t y,
                   int32_t height, MosColor color) {
    if (!ctx || height <= 0) return;
    if (x < ctx->clip.x || x >= MOS_RECT_RIGHT(ctx->clip)) return;

    int32_t y1 = gfx_clamp(y, ctx->clip.y, MOS_RECT_BOTTOM(ctx->clip) - 1);
    int32_t y2 = gfx_clamp(y + height - 1, ctx->clip.y, MOS_RECT_BOTTOM(ctx->clip) - 1);
    if (y1 > y2) return;

    if (ctx->bpp == 32) {
        for (int32_t cy = y1; cy <= y2; cy++) {
            uint32_t* fb = (uint32_t*)(ctx->framebuffer + cy * ctx->pitch + x * 4);
            *fb = color;
        }
    }
}

void mos_gfx_line(MosGraphicsContext* ctx, int32_t x1, int32_t y1,
                  int32_t x2, int32_t y2, MosColor color) {
    if (!ctx) return;

    int32_t dx = gfx_abs(x2 - x1);
    int32_t dy = gfx_abs(y2 - y1);
    int32_t sx = (x1 < x2) ? 1 : -1;
    int32_t sy = (y1 < y2) ? 1 : -1;
    int32_t err = dx - dy;

    while (1) {
        mos_gfx_pixel(ctx, x1, y1, color);

        if (x1 == x2 && y1 == y2) break;

        int32_t e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// -----------------------------------------------------------------------------
// B.5 Rectangle Drawing
// -----------------------------------------------------------------------------

void mos_gfx_fill_rect(MosGraphicsContext* ctx, MosRect rect, MosColor color) {
    if (!ctx) return;

    // Clip rectangle
    int32_t x1 = gfx_clamp(rect.x, ctx->clip.x, MOS_RECT_RIGHT(ctx->clip));
    int32_t y1 = gfx_clamp(rect.y, ctx->clip.y, MOS_RECT_BOTTOM(ctx->clip));
    int32_t x2 = gfx_clamp(MOS_RECT_RIGHT(rect), ctx->clip.x, MOS_RECT_RIGHT(ctx->clip));
    int32_t y2 = gfx_clamp(MOS_RECT_BOTTOM(rect), ctx->clip.y, MOS_RECT_BOTTOM(ctx->clip));

    if (x1 >= x2 || y1 >= y2) return;

    if (ctx->bpp == 32) {
        for (int32_t y = y1; y < y2; y++) {
            uint32_t* fb = (uint32_t*)(ctx->framebuffer + y * ctx->pitch + x1 * 4);
            for (int32_t x = x1; x < x2; x++) {
                *fb++ = color;
            }
        }
    }
}

void mos_gfx_draw_rect(MosGraphicsContext* ctx, MosRect rect,
                       MosColor color, int32_t thickness) {
    if (!ctx || thickness <= 0) return;

    // Top
    mos_gfx_fill_rect(ctx, MOS_RECT(rect.x, rect.y, rect.width, thickness), color);
    // Bottom
    mos_gfx_fill_rect(ctx, MOS_RECT(rect.x, rect.y + rect.height - thickness,
                                     rect.width, thickness), color);
    // Left
    mos_gfx_fill_rect(ctx, MOS_RECT(rect.x, rect.y + thickness,
                                     thickness, rect.height - 2 * thickness), color);
    // Right
    mos_gfx_fill_rect(ctx, MOS_RECT(rect.x + rect.width - thickness, rect.y + thickness,
                                     thickness, rect.height - 2 * thickness), color);
}

void mos_gfx_rounded_rect(MosGraphicsContext* ctx, MosRect rect,
                          int32_t radius, MosColor fill, MosColor border) {
    if (!ctx) return;

    // Clamp radius to half the smallest dimension
    int32_t max_radius = (rect.width < rect.height ? rect.width : rect.height) / 2;
    if (radius > max_radius) radius = max_radius;

    // Simplified: just draw regular rect for now
    // Full implementation would draw quarter circles at corners with given radius
    // For now, radius is validated but corners are square
    (void)radius;  // Will be used when quarter-circle drawing is implemented

    mos_gfx_fill_rect(ctx, rect, fill);
    if (border != fill) {
        mos_gfx_draw_rect(ctx, rect, border, 1);
    }
}

void mos_gfx_gradient_rect(MosGraphicsContext* ctx, MosRect rect,
                           MosColor top, MosColor bottom) {
    if (!ctx || rect.height <= 0) return;

    for (int32_t y = 0; y < rect.height; y++) {
        // Linear interpolation
        uint8_t t = (y * 255) / rect.height;
        MosColor color = mos_gfx_blend(bottom, top, t);
        mos_gfx_hline(ctx, rect.x, rect.y + y, rect.width, color);
    }
}

// -----------------------------------------------------------------------------
// B.6 Circle Drawing
// -----------------------------------------------------------------------------

void mos_gfx_fill_circle(MosGraphicsContext* ctx, int32_t cx, int32_t cy,
                         int32_t radius, MosColor color) {
    if (!ctx || radius <= 0) return;

    int32_t x = 0;
    int32_t y = radius;
    int32_t d = 3 - 2 * radius;

    while (y >= x) {
        // Draw horizontal lines for filled circle
        mos_gfx_hline(ctx, cx - x, cy - y, 2 * x + 1, color);
        mos_gfx_hline(ctx, cx - y, cy - x, 2 * y + 1, color);
        mos_gfx_hline(ctx, cx - y, cy + x, 2 * y + 1, color);
        mos_gfx_hline(ctx, cx - x, cy + y, 2 * x + 1, color);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void mos_gfx_draw_circle(MosGraphicsContext* ctx, int32_t cx, int32_t cy,
                         int32_t radius, MosColor color) {
    if (!ctx || radius <= 0) return;

    int32_t x = 0;
    int32_t y = radius;
    int32_t d = 3 - 2 * radius;

    while (y >= x) {
        mos_gfx_pixel(ctx, cx + x, cy - y, color);
        mos_gfx_pixel(ctx, cx + y, cy - x, color);
        mos_gfx_pixel(ctx, cx + y, cy + x, color);
        mos_gfx_pixel(ctx, cx + x, cy + y, color);
        mos_gfx_pixel(ctx, cx - x, cy + y, color);
        mos_gfx_pixel(ctx, cx - y, cy + x, color);
        mos_gfx_pixel(ctx, cx - y, cy - x, color);
        mos_gfx_pixel(ctx, cx - x, cy - y, color);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// -----------------------------------------------------------------------------
// B.7 Text Drawing
// -----------------------------------------------------------------------------

void mos_gfx_set_font(MosGraphicsContext* ctx, const MosFont* font) {
    if (!ctx) return;
    ctx->font = font ? font : &MOS_FONT_DEFAULT;
}

void mos_gfx_char(MosGraphicsContext* ctx, int32_t x, int32_t y,
                  char c, MosColor fg, MosColor bg) {
    if (!ctx || !ctx->font) return;

    const MosFont* font = ctx->font;

    // Check if character is in font
    if (c < font->first_char || c >= font->first_char + font->char_count) {
        c = '?';  // Unknown character
        if (c < font->first_char || c >= font->first_char + font->char_count) {
            return;  // Font doesn't have '?' either
        }
    }

    int32_t char_index = c - font->first_char;
    const uint8_t* glyph = font->data + char_index * font->height;

    // Draw character
    for (int32_t row = 0; row < font->height; row++) {
        uint8_t bits = glyph[row];
        for (int32_t col = 0; col < font->width; col++) {
            MosColor color = (bits & (0x80 >> col)) ? fg : bg;
            if (color != bg || bg != 0) {  // Skip transparent background
                mos_gfx_pixel(ctx, x + col, y + row, color);
            }
        }
    }
}

void mos_gfx_string(MosGraphicsContext* ctx, int32_t x, int32_t y,
                    const char* str, MosColor fg, MosColor bg) {
    if (!ctx || !str) return;

    const MosFont* font = ctx->font ? ctx->font : &MOS_FONT_DEFAULT;

    while (*str) {
        if (*str == '\n') {
            x = ctx->cursor_x;
            y += font->height;
        } else {
            mos_gfx_char(ctx, x, y, *str, fg, bg);
            x += font->width;
        }
        str++;
    }
}

void mos_gfx_string_n(MosGraphicsContext* ctx, int32_t x, int32_t y,
                      const char* str, mos_size_t len, MosColor fg, MosColor bg) {
    if (!ctx || !str) return;

    const MosFont* font = ctx->font ? ctx->font : &MOS_FONT_DEFAULT;

    for (mos_size_t i = 0; i < len && str[i]; i++) {
        if (str[i] == '\n') {
            x = ctx->cursor_x;
            y += font->height;
        } else {
            mos_gfx_char(ctx, x, y, str[i], fg, bg);
            x += font->width;
        }
    }
}

int32_t mos_gfx_string_width(MosGraphicsContext* ctx, const char* str) {
    if (!ctx || !str) return 0;

    const MosFont* font = ctx->font ? ctx->font : &MOS_FONT_DEFAULT;
    int32_t width = 0;
    int32_t max_width = 0;

    while (*str) {
        if (*str == '\n') {
            if (width > max_width) max_width = width;
            width = 0;
        } else {
            width += font->width;
        }
        str++;
    }

    return (width > max_width) ? width : max_width;
}

void mos_gfx_string_centered(MosGraphicsContext* ctx, MosRect rect,
                              const char* str, MosColor fg, MosColor bg) {
    if (!ctx || !str) return;

    const MosFont* font = ctx->font ? ctx->font : &MOS_FONT_DEFAULT;
    int32_t width = mos_gfx_string_width(ctx, str);
    int32_t x = rect.x + (rect.width - width) / 2;
    int32_t y = rect.y + (rect.height - font->height) / 2;

    mos_gfx_string(ctx, x, y, str, fg, bg);
}

// -----------------------------------------------------------------------------
// B.8 Blit Operations
// -----------------------------------------------------------------------------

void mos_gfx_blit(MosGraphicsContext* dst, int32_t dx, int32_t dy,
                  MosGraphicsContext* src, MosRect src_rect) {
    if (!dst || !src) return;

    for (int32_t y = 0; y < src_rect.height; y++) {
        for (int32_t x = 0; x < src_rect.width; x++) {
            MosColor color = mos_gfx_get_pixel(src, src_rect.x + x, src_rect.y + y);
            mos_gfx_pixel(dst, dx + x, dy + y, color);
        }
    }
}

void mos_gfx_blit_transparent(MosGraphicsContext* dst, int32_t dx, int32_t dy,
                               MosGraphicsContext* src, MosRect src_rect,
                               MosColor transparent) {
    if (!dst || !src) return;

    for (int32_t y = 0; y < src_rect.height; y++) {
        for (int32_t x = 0; x < src_rect.width; x++) {
            MosColor color = mos_gfx_get_pixel(src, src_rect.x + x, src_rect.y + y);
            if (color != transparent) {
                mos_gfx_pixel(dst, dx + x, dy + y, color);
            }
        }
    }
}

void mos_gfx_fill_pattern(MosGraphicsContext* ctx, MosRect rect,
                           const uint8_t* pattern, int32_t pattern_size,
                           MosColor fg, MosColor bg) {
    if (!ctx || !pattern || pattern_size <= 0) return;

    for (int32_t y = 0; y < rect.height; y++) {
        uint8_t bits = pattern[y % pattern_size];
        for (int32_t x = 0; x < rect.width; x++) {
            MosColor color = (bits & (0x80 >> (x % 8))) ? fg : bg;
            mos_gfx_pixel(ctx, rect.x + x, rect.y + y, color);
        }
    }
}

// -----------------------------------------------------------------------------
// B.9 Clipping
// -----------------------------------------------------------------------------

void mos_gfx_set_clip(MosGraphicsContext* ctx, MosRect clip) {
    if (!ctx) return;
    ctx->clip = clip;
}

void mos_gfx_reset_clip(MosGraphicsContext* ctx) {
    if (!ctx) return;
    ctx->clip.x = 0;
    ctx->clip.y = 0;
    ctx->clip.width = ctx->width;
    ctx->clip.height = ctx->height;
}

void mos_gfx_push_clip(MosGraphicsContext* ctx, MosRect clip) {
    if (!ctx) return;

    // Intersect with current clip
    int32_t x1 = gfx_clamp(clip.x, ctx->clip.x, MOS_RECT_RIGHT(ctx->clip));
    int32_t y1 = gfx_clamp(clip.y, ctx->clip.y, MOS_RECT_BOTTOM(ctx->clip));
    int32_t x2 = gfx_clamp(MOS_RECT_RIGHT(clip), ctx->clip.x, MOS_RECT_RIGHT(ctx->clip));
    int32_t y2 = gfx_clamp(MOS_RECT_BOTTOM(clip), ctx->clip.y, MOS_RECT_BOTTOM(ctx->clip));

    ctx->clip.x = x1;
    ctx->clip.y = y1;
    ctx->clip.width = x2 - x1;
    ctx->clip.height = y2 - y1;
}

// -----------------------------------------------------------------------------
// B.10 Utility
// -----------------------------------------------------------------------------

MosColor mos_gfx_blend(MosColor fg, MosColor bg, uint8_t alpha) {
    uint32_t r = (MOS_RED(fg) * alpha + MOS_RED(bg) * (255 - alpha)) / 255;
    uint32_t g = (MOS_GREEN(fg) * alpha + MOS_GREEN(bg) * (255 - alpha)) / 255;
    uint32_t b = (MOS_BLUE(fg) * alpha + MOS_BLUE(bg) * (255 - alpha)) / 255;
    return MOS_RGB(r, g, b);
}

MosColor mos_gfx_darken(MosColor color, uint8_t amount) {
    int32_t r = MOS_RED(color) - amount;
    int32_t g = MOS_GREEN(color) - amount;
    int32_t b = MOS_BLUE(color) - amount;
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    return MOS_RGB(r, g, b);
}

MosColor mos_gfx_lighten(MosColor color, uint8_t amount) {
    int32_t r = MOS_RED(color) + amount;
    int32_t g = MOS_GREEN(color) + amount;
    int32_t b = MOS_BLUE(color) + amount;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    return MOS_RGB(r, g, b);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Graphics implementation provides:
//   - Pixel, line, rect, circle drawing
//   - 8x16 bitmap font rendering
//   - Blit operations with transparency
//   - Clipping support
//   - Color blending utilities
//
// =============================================================================
// END CLOSING
// =============================================================================
