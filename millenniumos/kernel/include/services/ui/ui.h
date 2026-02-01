// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-ui
// Title:       MillenniumOS UI Framework
// Type:        Header
// Component:   Stage 4 (Shell) / UI Framework
// Role:        Widgets, windows, screens for user interface
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Genesis 1:27 — "So God created man in his own image"
//              UI creates the interface through which users interact.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_UI_H
#define MILLENNIUMOS_UI_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"
#include "services/graphics/graphics.h"
#include "hal/hal.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 UI Constants
// -----------------------------------------------------------------------------

#define MOS_UI_MAX_WINDOWS      27      // 3^3 windows
#define MOS_UI_MAX_WIDGETS      243     // 3^5 widgets per window
#define MOS_UI_MAX_TEXT         256     // Max text length

// Widget types
typedef enum {
    MOS_WIDGET_NONE     = 0,
    MOS_WIDGET_LABEL    = 1,    // Static text
    MOS_WIDGET_BUTTON   = 2,    // Clickable button
    MOS_WIDGET_TEXTBOX  = 3,    // Text input
    MOS_WIDGET_PASSWORD = 4,    // Password input (masked)
    MOS_WIDGET_CHECKBOX = 5,    // Toggle checkbox
    MOS_WIDGET_ICON     = 6,    // Icon/image
    MOS_WIDGET_PANEL    = 7,    // Container panel
    MOS_WIDGET_LIST     = 8,    // List view
    MOS_WIDGET_PROGRESS = 9,    // Progress bar
} MosWidgetType;

// Widget states (ternary)
#define MOS_WIDGET_DISABLED     TRIT_NEG    // -1: Disabled
#define MOS_WIDGET_NORMAL       TRIT_ZERO   // 0: Normal
#define MOS_WIDGET_FOCUSED      TRIT_POS    // +1: Has focus

// Widget flags
#define MOS_WIDGET_VISIBLE      ((trit5_t)1)
#define MOS_WIDGET_ENABLED      ((trit5_t)3)
#define MOS_WIDGET_FOCUSABLE    ((trit5_t)9)
#define MOS_WIDGET_HOVERED      ((trit5_t)27)
#define MOS_WIDGET_PRESSED      ((trit5_t)81)

// Screen types
typedef enum {
    MOS_SCREEN_NONE     = 0,
    MOS_SCREEN_BOOT     = 1,    // Boot splash
    MOS_SCREEN_LOGIN    = 2,    // Login screen
    MOS_SCREEN_HOME     = 3,    // Home/desktop screen
    MOS_SCREEN_APP      = 4,    // Application screen
    MOS_SCREEN_SHUTDOWN = 5,    // Shutdown screen
} MosScreenType;

// -----------------------------------------------------------------------------
// B.2 Widget Structure
// -----------------------------------------------------------------------------

// Forward declaration
struct MosWidget;
struct MosWindow;

// Widget callback types
typedef void (*MosWidgetCallback)(struct MosWidget* widget, void* data);
typedef void (*MosWidgetDrawFunc)(struct MosWidget* widget, MosGraphicsContext* ctx);

typedef struct MosWidget {
    // Identity
    uint16_t            id;
    MosWidgetType       type;
    struct MosWindow*   parent;

    // Geometry
    MosRect             bounds;
    int32_t             padding;

    // State
    trit_t              state;          // Disabled/Normal/Focused
    trit5_t             flags;          // Visibility, enabled, etc.

    // Appearance
    MosColor            bg_color;
    MosColor            fg_color;
    MosColor            border_color;
    int32_t             border_width;

    // Content (type-specific)
    union {
        // Label/Button
        struct {
            char        text[MOS_UI_MAX_TEXT];
            bool        center_text;
        } label;

        // Textbox/Password
        struct {
            char        text[MOS_UI_MAX_TEXT];
            char        placeholder[MOS_UI_MAX_TEXT];
            int32_t     cursor_pos;
            int32_t     max_length;
            bool        masked;         // For password
        } textbox;

        // Checkbox
        struct {
            char        text[MOS_UI_MAX_TEXT];
            bool        checked;
        } checkbox;

        // Progress
        struct {
            int32_t     value;          // 0-100
            int32_t     max;
        } progress;

        // Icon
        struct {
            uint8_t     icon_id;
            int32_t     icon_size;
        } icon;
    };

    // Callbacks
    MosWidgetCallback   on_click;
    MosWidgetCallback   on_change;
    MosWidgetDrawFunc   custom_draw;
    void*               user_data;
} MosWidget;

// -----------------------------------------------------------------------------
// B.3 Window Structure
// -----------------------------------------------------------------------------

typedef struct MosWindow {
    // Identity
    uint16_t            id;
    char                title[MOS_UI_MAX_TEXT];
    bool                active;

    // Geometry
    MosRect             bounds;
    bool                fullscreen;

    // Appearance
    MosColor            bg_color;
    bool                show_titlebar;
    bool                show_border;

    // Widgets
    MosWidget*          widgets[MOS_UI_MAX_WIDGETS];
    mos_size_t          widget_count;
    MosWidget*          focused_widget;

    // Callbacks
    MosWidgetCallback   on_close;
    void*               user_data;
} MosWindow;

// -----------------------------------------------------------------------------
// B.4 Screen Structure (Login, Home, etc.)
// -----------------------------------------------------------------------------

typedef struct MosScreen {
    MosScreenType       type;
    MosWindow*          window;
    MosGraphicsContext  ctx;
    bool                needs_redraw;

    // Screen-specific data
    union {
        // Login screen
        struct {
            MosWidget*  username_field;
            MosWidget*  password_field;
            MosWidget*  login_button;
            char        message[MOS_UI_MAX_TEXT];
        } login;

        // Home screen
        struct {
            MosWidget*  app_icons[27];  // 3^3 app icons
            mos_size_t  app_count;
            int32_t     selected_app;
        } home;
    };
} MosScreen;

// -----------------------------------------------------------------------------
// B.5 Application Entry
// -----------------------------------------------------------------------------

typedef struct {
    char                name[64];
    char                path[MOS_MAX_PATH];
    uint8_t             icon_id;
    MosZone             zone;           // Required zone
    bool                installed;
} MosAppEntry;

// -----------------------------------------------------------------------------
// B.6 Global UI State
// -----------------------------------------------------------------------------

typedef struct {
    // Display
    MosGraphicsContext  screen_ctx;
    uint32_t            screen_width;
    uint32_t            screen_height;

    // Current screen
    MosScreen*          current_screen;
    MosScreenType       screen_type;

    // Mouse cursor
    int32_t             cursor_x;
    int32_t             cursor_y;
    bool                cursor_visible;

    // Applications
    MosAppEntry         apps[27];
    mos_size_t          app_count;

    // User session
    bool                logged_in;
    char                current_user[64];

    // Health
    mos_health_t        health;
} MosUI;

// -----------------------------------------------------------------------------
// B.7 Initialization
// -----------------------------------------------------------------------------

// Initialize UI subsystem
mos_result_t mos_ui_init(void);

// Create graphics context for UI
mos_result_t mos_ui_init_graphics(void);

// -----------------------------------------------------------------------------
// B.8 Screen Management
// -----------------------------------------------------------------------------

// Show boot splash screen
mos_result_t mos_ui_show_boot(void);

// Show login screen
mos_result_t mos_ui_show_login(void);

// Show home screen
mos_result_t mos_ui_show_home(void);

// Show shutdown screen
mos_result_t mos_ui_show_shutdown(void);

// Get current screen
MosScreen* mos_ui_current_screen(void);

// Redraw current screen
void mos_ui_redraw(void);

// -----------------------------------------------------------------------------
// B.9 Window Management
// -----------------------------------------------------------------------------

// Create window
MosWindow* mos_ui_create_window(const char* title, MosRect bounds);

// Destroy window
void mos_ui_destroy_window(MosWindow* window);

// Set window fullscreen
void mos_ui_window_fullscreen(MosWindow* window, bool fullscreen);

// Draw window
void mos_ui_draw_window(MosWindow* window, MosGraphicsContext* ctx);

// -----------------------------------------------------------------------------
// B.10 Widget Management
// -----------------------------------------------------------------------------

// Create widget
MosWidget* mos_ui_create_widget(MosWindow* window, MosWidgetType type, MosRect bounds);

// Destroy widget
void mos_ui_destroy_widget(MosWidget* widget);

// Widget creators (convenience functions)
MosWidget* mos_ui_create_label(MosWindow* window, MosRect bounds, const char* text);
MosWidget* mos_ui_create_button(MosWindow* window, MosRect bounds, const char* text,
                                 MosWidgetCallback on_click);
MosWidget* mos_ui_create_textbox(MosWindow* window, MosRect bounds,
                                  const char* placeholder);
MosWidget* mos_ui_create_password(MosWindow* window, MosRect bounds,
                                   const char* placeholder);
MosWidget* mos_ui_create_checkbox(MosWindow* window, MosRect bounds,
                                   const char* text, bool checked);

// Draw widget
void mos_ui_draw_widget(MosWidget* widget, MosGraphicsContext* ctx);

// Set widget text
void mos_ui_widget_set_text(MosWidget* widget, const char* text);

// Get widget text
const char* mos_ui_widget_get_text(MosWidget* widget);

// -----------------------------------------------------------------------------
// B.11 Input Handling
// -----------------------------------------------------------------------------

// Process single event
void mos_ui_process_event(MosEvent* event);

// Main event loop (blocks until quit)
void mos_ui_event_loop(void);

// Handle key event
void mos_ui_handle_key(MosKeyEvent* event);

// Handle mouse event
void mos_ui_handle_mouse(MosMouseEvent* event, MosEventType type);

// -----------------------------------------------------------------------------
// B.12 Application Management
// -----------------------------------------------------------------------------

// Register application
mos_result_t mos_ui_register_app(const char* name, const char* path,
                                  uint8_t icon_id, MosZone zone);

// Launch application
mos_result_t mos_ui_launch_app(const char* name);

// Get application list
const MosAppEntry* mos_ui_get_apps(mos_size_t* count);

// -----------------------------------------------------------------------------
// B.13 User Session
// -----------------------------------------------------------------------------

// Login user
mos_result_t mos_ui_login(const char* username, const char* password);

// Logout user
mos_result_t mos_ui_logout(void);

// Check if logged in
bool mos_ui_is_logged_in(void);

// Get current user
const char* mos_ui_current_user(void);

// -----------------------------------------------------------------------------
// B.14 Cursor
// -----------------------------------------------------------------------------

// Draw cursor at current position
void mos_ui_draw_cursor(MosGraphicsContext* ctx);

// Set cursor visibility
void mos_ui_show_cursor(bool visible);

// -----------------------------------------------------------------------------
// B.15 Information
// -----------------------------------------------------------------------------

// Get UI state
const MosUI* mos_ui_info(void);

// Get UI health
mos_health_t mos_ui_health(void);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// UI Framework provides:
//   - Screen management (boot, login, home, shutdown)
//   - Window system with widgets
//   - Widget types: label, button, textbox, password, checkbox, etc.
//   - Input handling (keyboard, mouse)
//   - Application launcher
//   - User session management
//
// "So God created man in his own image, in the image of God
//  created he him; male and female created he them."
// — Genesis 1:27
//
// The UI creates the interface through which users interact with the system,
// reflecting the Creator's design of meaningful interaction.
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // MILLENNIUMOS_UI_H
