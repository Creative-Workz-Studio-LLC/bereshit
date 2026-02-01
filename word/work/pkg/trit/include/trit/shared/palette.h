// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         TRIT-shared-palette
// Title:       Kingdom Technology Color Palette
// Type:        Header
// Component:   libtrit/shared
// Role:        Unified color definitions for CLI/TUI/GUI rendering
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
//              Light and color make the invisible visible.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef TRIT_SHARED_PALETTE_H
#define TRIT_SHARED_PALETTE_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Color Type
// -----------------------------------------------------------------------------

typedef uint32_t kt_color_t;  // Kingdom Technology color (32-bit ARGB)

// Color creation macros
#define KT_RGB(r, g, b)        (((uint32_t)0xFF << 24) | ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))
#define KT_RGBA(r, g, b, a)    (((uint32_t)(a) << 24) | ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

// Color extraction
#define KT_RED(c)              (((c) >> 16) & 0xFF)
#define KT_GREEN(c)            (((c) >> 8) & 0xFF)
#define KT_BLUE(c)             ((c) & 0xFF)
#define KT_ALPHA(c)            (((c) >> 24) & 0xFF)

// -----------------------------------------------------------------------------
// B.2 Standard Colors
// -----------------------------------------------------------------------------

// Basic colors
#define KT_COLOR_BLACK         KT_RGB(0, 0, 0)
#define KT_COLOR_WHITE         KT_RGB(255, 255, 255)
#define KT_COLOR_RED           KT_RGB(255, 0, 0)
#define KT_COLOR_GREEN         KT_RGB(0, 255, 0)
#define KT_COLOR_BLUE          KT_RGB(0, 0, 255)
#define KT_COLOR_YELLOW        KT_RGB(255, 255, 0)
#define KT_COLOR_CYAN          KT_RGB(0, 255, 255)
#define KT_COLOR_MAGENTA       KT_RGB(255, 0, 255)

// -----------------------------------------------------------------------------
// B.3 Kingdom Technology Palette (Zone Colors)
// -----------------------------------------------------------------------------
//
// The three zones map to a color gradient:
//   void (-1): Entry gate, permissive - dark, formless
//   word (0):  Processing, standard - medium, working
//   tov (+1):  Exit gate, complete - light, refined
//

// Zone colors (primary palette)
#define KT_COLOR_VOID          KT_RGB(26, 26, 46)       // Dark blue-gray
#define KT_COLOR_WORD          KT_RGB(46, 46, 86)       // Medium blue-gray
#define KT_COLOR_TOV           KT_RGB(86, 106, 146)     // Light blue-gray

// Kingdom accent colors
#define KT_COLOR_GOLD          KT_RGB(218, 165, 32)     // Kingdom gold
#define KT_COLOR_PURPLE        KT_RGB(128, 0, 128)      // Royal purple
#define KT_COLOR_SILVER        KT_RGB(192, 192, 210)    // Kingdom silver

// -----------------------------------------------------------------------------
// B.4 UI Colors
// -----------------------------------------------------------------------------

// Background colors
#define KT_COLOR_BG_DARK       KT_RGB(30, 30, 40)
#define KT_COLOR_BG_LIGHT      KT_RGB(50, 50, 65)
#define KT_COLOR_BG_HOVER      KT_RGB(60, 60, 80)
#define KT_COLOR_BG_ACTIVE     KT_RGB(70, 70, 95)

// Accent colors
#define KT_COLOR_ACCENT        KT_RGB(70, 130, 180)     // Steel blue
#define KT_COLOR_ACCENT_LIGHT  KT_RGB(100, 160, 210)
#define KT_COLOR_ACCENT_DARK   KT_RGB(50, 100, 150)

// Text colors
#define KT_COLOR_TEXT          KT_RGB(220, 220, 230)
#define KT_COLOR_TEXT_DIM      KT_RGB(140, 140, 160)
#define KT_COLOR_TEXT_BRIGHT   KT_RGB(255, 255, 255)

// Semantic colors
#define KT_COLOR_SUCCESS       KT_RGB(76, 175, 80)      // Green
#define KT_COLOR_WARNING       KT_RGB(255, 193, 7)      // Amber
#define KT_COLOR_ERROR         KT_RGB(244, 67, 54)      // Red
#define KT_COLOR_INFO          KT_RGB(33, 150, 243)     // Blue

// -----------------------------------------------------------------------------
// B.5 Hebrew State Colors
// -----------------------------------------------------------------------------
//
// Color-coding for the 7 Hebrew cognitive states:
//

#define KT_COLOR_SHAVAR        KT_RGB(139, 0, 0)        // Dark red (broken)
#define KT_COLOR_CHASER        KT_RGB(205, 92, 92)      // Indian red (lacking)
#define KT_COLOR_RATSAH        KT_RGB(255, 165, 0)      // Orange (wanting)
#define KT_COLOR_YASHAR        KT_RGB(218, 165, 32)     // Gold (centered/even)
#define KT_COLOR_TAMIM         KT_RGB(144, 238, 144)    // Light green (sound)
#define KT_COLOR_SHALEM        KT_RGB(50, 205, 50)      // Lime green (whole)
#define KT_COLOR_TOV_STATE     KT_RGB(0, 128, 0)        // Green (perfect)

// -----------------------------------------------------------------------------
// B.6 Display Mode Enum
// -----------------------------------------------------------------------------
//
// Adaptive display modes for responsive rendering:
//   Full:    All details displayed (width >= 160)
//   Normal:  Standard display (~140 chars)
//   Compact: Reduced display (~100 chars)
//   Minimal: Essential only (~80 chars)
//

typedef enum {
    KT_DISPLAY_FULL     = 0,    // All details
    KT_DISPLAY_NORMAL   = 1,    // Standard
    KT_DISPLAY_COMPACT  = 2,    // Reduced
    KT_DISPLAY_MINIMAL  = 3,    // Essential only
} KtDisplayMode;

// Display mode thresholds (terminal width)
#define KT_DISPLAY_FULL_WIDTH       160
#define KT_DISPLAY_NORMAL_WIDTH     140
#define KT_DISPLAY_COMPACT_WIDTH    100
#define KT_DISPLAY_MINIMAL_WIDTH    80

// Helper to determine display mode from width
static inline KtDisplayMode kt_display_mode_from_width(int width) {
    if (width >= KT_DISPLAY_FULL_WIDTH) return KT_DISPLAY_FULL;
    if (width >= KT_DISPLAY_NORMAL_WIDTH) return KT_DISPLAY_NORMAL;
    if (width >= KT_DISPLAY_COMPACT_WIDTH) return KT_DISPLAY_COMPACT;
    return KT_DISPLAY_MINIMAL;
}

// -----------------------------------------------------------------------------
// B.7 Render Backend Enum
// -----------------------------------------------------------------------------
//
// Rendering backend types with tripwire fallback:
//   GUI: Full graphical interface (OpenGL, Vulkan, etc.)
//   TUI: Terminal UI (ncurses, ANSI)
//   CLI: Command-line interface (pure text)
//
// Fallback chain: GUI -> TUI -> CLI (CLI always available)
//

typedef enum {
    KT_BACKEND_CLI      = -1,   // Always available (anchor)
    KT_BACKEND_TUI      = 0,    // Terminal UI
    KT_BACKEND_GUI      = 1,    // Full graphics
} KtRenderBackend;

// -----------------------------------------------------------------------------
// B.8 Color Conversion Utilities
// -----------------------------------------------------------------------------
//
// Cross-system color conversion between:
//   - kt_color_t (uint32 ARGB) for kernel/framebuffer
//   - Float RGBA for OpenGL/GPU rendering
//

// Float RGBA color structure (for GPU rendering)
typedef struct {
    float r, g, b, a;
} kt_color_rgba_t;

// Convert kt_color_t (ARGB uint32) to float RGBA
static inline kt_color_rgba_t kt_color_to_rgba(kt_color_t c) {
    kt_color_rgba_t result;
    result.r = (float)KT_RED(c) / 255.0f;
    result.g = (float)KT_GREEN(c) / 255.0f;
    result.b = (float)KT_BLUE(c) / 255.0f;
    result.a = (float)KT_ALPHA(c) / 255.0f;
    return result;
}

// Convert float RGBA to kt_color_t (ARGB uint32)
static inline kt_color_t kt_rgba_to_color(float r, float g, float b, float a) {
    uint8_t ri = (uint8_t)(r * 255.0f);
    uint8_t gi = (uint8_t)(g * 255.0f);
    uint8_t bi = (uint8_t)(b * 255.0f);
    uint8_t ai = (uint8_t)(a * 255.0f);
    return KT_RGBA(ri, gi, bi, ai);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Kingdom Technology Color Palette provides:
//   - Unified color definitions across CLI/TUI/GUI
//   - Zone-based colors (void/word/tov)
//   - Hebrew cognitive state colors
//   - Adaptive display modes
//   - Render backend enum with tripwire semantics
//
// "And God made two great lights; the greater light to rule the day,
//  and the lesser light to rule the night: he made the stars also."
// — Genesis 1:16
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // TRIT_SHARED_PALETTE_H
