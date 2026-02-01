// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-ui-impl
// Title:       MillenniumOS UI Framework Implementation
// Type:        Source
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
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "services/ui/ui.h"
#include "core/memory/memory.h"
#include "services/cpisi/cpisi.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global UI State
// -----------------------------------------------------------------------------

static MosUI g_ui;

// Static screens
static MosScreen g_boot_screen;
static MosScreen g_login_screen;
static MosScreen g_home_screen;
static MosScreen g_shutdown_screen;

// Static windows (for simplicity)
static MosWindow g_login_window;
static MosWindow g_home_window;

// Widget pool
#define WIDGET_POOL_SIZE 243
static MosWidget g_widget_pool[WIDGET_POOL_SIZE];
static mos_size_t g_widget_next = 0;

// -----------------------------------------------------------------------------
// B.2 String Helpers
// -----------------------------------------------------------------------------

static mos_size_t ui_strlen(const char* s) {
    mos_size_t len = 0;
    while (s && s[len]) len++;
    return len;
}

static void ui_strcpy(char* dst, const char* src, mos_size_t max) {
    mos_size_t i = 0;
    while (src && src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static int ui_strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

static void ui_memset(void* ptr, uint8_t val, mos_size_t size) {
    uint8_t* p = (uint8_t*)ptr;
    while (size--) *p++ = val;
}

// -----------------------------------------------------------------------------
// B.3 Widget Drawing
// -----------------------------------------------------------------------------

static void draw_label(MosWidget* w, MosGraphicsContext* ctx) {
    // Draw background if not transparent
    if (w->bg_color != 0) {
        mos_gfx_fill_rect(ctx, w->bounds, w->bg_color);
    }

    // Draw text
    if (w->label.center_text) {
        mos_gfx_string_centered(ctx, w->bounds, w->label.text, w->fg_color, w->bg_color);
    } else {
        mos_gfx_string(ctx, w->bounds.x + w->padding, w->bounds.y + w->padding,
                       w->label.text, w->fg_color, w->bg_color);
    }
}

static void draw_button(MosWidget* w, MosGraphicsContext* ctx) {
    MosColor bg = w->bg_color;
    MosColor border = w->border_color;

    // Modify colors based on state
    if (w->flags & MOS_WIDGET_PRESSED) {
        bg = mos_gfx_darken(bg, 30);
    } else if (w->flags & MOS_WIDGET_HOVERED) {
        bg = mos_gfx_lighten(bg, 20);
    }

    // Accessibility: High-contrast focus ring
    if (w->state == MOS_WIDGET_FOCUSED) {
        // Draw outer focus ring (2px outside bounds)
        MosRect focus_ring = {
            w->bounds.x - 2, w->bounds.y - 2,
            w->bounds.width + 4, w->bounds.height + 4
        };
        mos_gfx_draw_rect(ctx, focus_ring, MOS_COLOR_GOLD, 2);
        border = MOS_COLOR_ACCENT;
    }

    // Draw button background
    mos_gfx_fill_rect(ctx, w->bounds, bg);

    // Draw border
    mos_gfx_draw_rect(ctx, w->bounds, border, w->border_width);

    // Draw text centered
    mos_gfx_string_centered(ctx, w->bounds, w->label.text, w->fg_color, bg);
}

static void draw_textbox(MosWidget* w, MosGraphicsContext* ctx) {
    MosColor bg = w->bg_color;
    MosColor border = w->border_color;

    // Accessibility: High-contrast focus ring
    if (w->state == MOS_WIDGET_FOCUSED) {
        MosRect focus_ring = {
            w->bounds.x - 2, w->bounds.y - 2,
            w->bounds.width + 4, w->bounds.height + 4
        };
        mos_gfx_draw_rect(ctx, focus_ring, MOS_COLOR_GOLD, 2);
        border = MOS_COLOR_ACCENT;
    }

    // Draw background
    mos_gfx_fill_rect(ctx, w->bounds, bg);

    // Draw border
    mos_gfx_draw_rect(ctx, w->bounds, border, w->border_width);

    // Draw text or placeholder
    const char* display_text = w->textbox.text;
    MosColor text_color = w->fg_color;

    if (w->textbox.text[0] == '\0' && w->textbox.placeholder[0] != '\0') {
        display_text = w->textbox.placeholder;
        text_color = MOS_COLOR_TEXT_DIM;
    }

    // For password, show asterisks
    if (w->textbox.masked && w->textbox.text[0] != '\0') {
        char masked[MOS_UI_MAX_TEXT];
        mos_size_t len = ui_strlen(w->textbox.text);
        for (mos_size_t i = 0; i < len && i < MOS_UI_MAX_TEXT - 1; i++) {
            masked[i] = '*';
        }
        masked[len] = '\0';
        display_text = masked;
    }

    int32_t text_x = w->bounds.x + w->padding;
    int32_t text_y = w->bounds.y + (w->bounds.height - 16) / 2;
    mos_gfx_string(ctx, text_x, text_y, display_text, text_color, bg);

    // Draw cursor if focused
    if (w->state == MOS_WIDGET_FOCUSED) {
        int32_t cursor_x = text_x + w->textbox.cursor_pos * 8;
        mos_gfx_vline(ctx, cursor_x, text_y, 16, w->fg_color);
    }
}

static void draw_checkbox(MosWidget* w, MosGraphicsContext* ctx) {
    // Draw checkbox box
    MosRect box = { w->bounds.x, w->bounds.y + (w->bounds.height - 16) / 2, 16, 16 };
    MosColor border = w->border_color;

    // Accessibility: High-contrast focus ring
    if (w->state == MOS_WIDGET_FOCUSED) {
        MosRect focus_ring = {
            box.x - 2, box.y - 2,
            box.width + 4, box.height + 4
        };
        mos_gfx_draw_rect(ctx, focus_ring, MOS_COLOR_GOLD, 2);
        border = MOS_COLOR_ACCENT;
    }

    mos_gfx_fill_rect(ctx, box, w->bg_color);
    mos_gfx_draw_rect(ctx, box, border, 1);

    // Draw check mark if checked
    if (w->checkbox.checked) {
        mos_gfx_line(ctx, box.x + 3, box.y + 8, box.x + 6, box.y + 12, w->fg_color);
        mos_gfx_line(ctx, box.x + 6, box.y + 12, box.x + 13, box.y + 3, w->fg_color);
    }

    // Draw label
    mos_gfx_string(ctx, w->bounds.x + 24, w->bounds.y + (w->bounds.height - 16) / 2,
                   w->checkbox.text, w->fg_color, 0);
}

static void draw_progress(MosWidget* w, MosGraphicsContext* ctx) {
    // Draw background
    mos_gfx_fill_rect(ctx, w->bounds, w->bg_color);
    mos_gfx_draw_rect(ctx, w->bounds, w->border_color, 1);

    // Draw progress bar
    int32_t fill_width = (w->bounds.width - 4) * w->progress.value / w->progress.max;
    MosRect fill = { w->bounds.x + 2, w->bounds.y + 2, fill_width, w->bounds.height - 4 };
    mos_gfx_fill_rect(ctx, fill, MOS_COLOR_ACCENT);
}

void mos_ui_draw_widget(MosWidget* widget, MosGraphicsContext* ctx) {
    if (!widget || !ctx) return;
    if (!(widget->flags & MOS_WIDGET_VISIBLE)) return;

    // Use custom draw if provided
    if (widget->custom_draw) {
        widget->custom_draw(widget, ctx);
        return;
    }

    switch (widget->type) {
        case MOS_WIDGET_LABEL:
            draw_label(widget, ctx);
            break;
        case MOS_WIDGET_BUTTON:
            draw_button(widget, ctx);
            break;
        case MOS_WIDGET_TEXTBOX:
        case MOS_WIDGET_PASSWORD:
            draw_textbox(widget, ctx);
            break;
        case MOS_WIDGET_CHECKBOX:
            draw_checkbox(widget, ctx);
            break;
        case MOS_WIDGET_PROGRESS:
            draw_progress(widget, ctx);
            break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// B.4 Window Management
// -----------------------------------------------------------------------------

void mos_ui_draw_window(MosWindow* window, MosGraphicsContext* ctx) {
    if (!window || !ctx || !window->active) return;

    // Draw window background
    mos_gfx_fill_rect(ctx, window->bounds, window->bg_color);

    // Draw title bar if shown
    if (window->show_titlebar) {
        MosRect titlebar = {
            window->bounds.x, window->bounds.y,
            window->bounds.width, 30
        };
        mos_gfx_gradient_rect(ctx, titlebar, MOS_COLOR_WORD, MOS_COLOR_VOID);
        mos_gfx_string(ctx, titlebar.x + 10, titlebar.y + 7,
                       window->title, MOS_COLOR_TEXT, 0);
    }

    // Draw border if shown
    if (window->show_border) {
        mos_gfx_draw_rect(ctx, window->bounds, MOS_COLOR_TOV, 1);
    }

    // Draw all widgets
    for (mos_size_t i = 0; i < window->widget_count; i++) {
        if (window->widgets[i]) {
            mos_ui_draw_widget(window->widgets[i], ctx);
        }
    }
}

MosWindow* mos_ui_create_window(const char* title, MosRect bounds) {
    // Use parameters for future implementation
    (void)title;
    (void)bounds;

    // For simplicity, use static windows
    // Real implementation would allocate dynamically
    return NULL;
}

void mos_ui_destroy_window(MosWindow* window) {
    if (!window) return;
    window->active = false;
}

void mos_ui_window_fullscreen(MosWindow* window, bool fullscreen) {
    if (!window) return;
    window->fullscreen = fullscreen;
    if (fullscreen) {
        window->bounds.x = 0;
        window->bounds.y = 0;
        window->bounds.width = g_ui.screen_width;
        window->bounds.height = g_ui.screen_height;
    }
}

// -----------------------------------------------------------------------------
// B.5 Widget Management
// -----------------------------------------------------------------------------

static MosWidget* alloc_widget(void) {
    if (g_widget_next >= WIDGET_POOL_SIZE) return NULL;
    MosWidget* w = &g_widget_pool[g_widget_next++];
    ui_memset(w, 0, sizeof(MosWidget));
    return w;
}

MosWidget* mos_ui_create_widget(MosWindow* window, MosWidgetType type, MosRect bounds) {
    if (!window || window->widget_count >= MOS_UI_MAX_WIDGETS) return NULL;

    MosWidget* w = alloc_widget();
    if (!w) return NULL;

    w->id = window->widget_count;
    w->type = type;
    w->parent = window;
    w->bounds = bounds;
    w->padding = 5;
    w->state = MOS_WIDGET_NORMAL;
    w->flags = MOS_WIDGET_VISIBLE | MOS_WIDGET_ENABLED;

    // Default colors
    w->bg_color = MOS_COLOR_BG_LIGHT;
    w->fg_color = MOS_COLOR_TEXT;
    w->border_color = MOS_COLOR_TOV;
    w->border_width = 1;

    window->widgets[window->widget_count++] = w;
    return w;
}

MosWidget* mos_ui_create_label(MosWindow* window, MosRect bounds, const char* text) {
    MosWidget* w = mos_ui_create_widget(window, MOS_WIDGET_LABEL, bounds);
    if (w) {
        ui_strcpy(w->label.text, text, MOS_UI_MAX_TEXT);
        w->label.center_text = false;
        w->bg_color = 0;  // Transparent
        w->flags &= ~MOS_WIDGET_FOCUSABLE;
    }
    return w;
}

MosWidget* mos_ui_create_button(MosWindow* window, MosRect bounds, const char* text,
                                 MosWidgetCallback on_click) {
    MosWidget* w = mos_ui_create_widget(window, MOS_WIDGET_BUTTON, bounds);
    if (w) {
        ui_strcpy(w->label.text, text, MOS_UI_MAX_TEXT);
        w->label.center_text = true;
        w->on_click = on_click;
        w->flags |= MOS_WIDGET_FOCUSABLE;
        w->bg_color = MOS_COLOR_ACCENT;
        w->border_width = 2;
    }
    return w;
}

MosWidget* mos_ui_create_textbox(MosWindow* window, MosRect bounds,
                                  const char* placeholder) {
    MosWidget* w = mos_ui_create_widget(window, MOS_WIDGET_TEXTBOX, bounds);
    if (w) {
        w->textbox.text[0] = '\0';
        ui_strcpy(w->textbox.placeholder, placeholder, MOS_UI_MAX_TEXT);
        w->textbox.cursor_pos = 0;
        w->textbox.max_length = MOS_UI_MAX_TEXT - 1;
        w->textbox.masked = false;
        w->flags |= MOS_WIDGET_FOCUSABLE;
        w->bg_color = MOS_COLOR_BG_DARK;
    }
    return w;
}

MosWidget* mos_ui_create_password(MosWindow* window, MosRect bounds,
                                   const char* placeholder) {
    MosWidget* w = mos_ui_create_textbox(window, bounds, placeholder);
    if (w) {
        w->type = MOS_WIDGET_PASSWORD;
        w->textbox.masked = true;
    }
    return w;
}

MosWidget* mos_ui_create_checkbox(MosWindow* window, MosRect bounds,
                                   const char* text, bool checked) {
    MosWidget* w = mos_ui_create_widget(window, MOS_WIDGET_CHECKBOX, bounds);
    if (w) {
        ui_strcpy(w->checkbox.text, text, MOS_UI_MAX_TEXT);
        w->checkbox.checked = checked;
        w->flags |= MOS_WIDGET_FOCUSABLE;
        w->bg_color = MOS_COLOR_BG_DARK;
    }
    return w;
}

void mos_ui_widget_set_text(MosWidget* widget, const char* text) {
    if (!widget || !text) return;
    switch (widget->type) {
        case MOS_WIDGET_LABEL:
        case MOS_WIDGET_BUTTON:
            ui_strcpy(widget->label.text, text, MOS_UI_MAX_TEXT);
            break;
        case MOS_WIDGET_TEXTBOX:
        case MOS_WIDGET_PASSWORD:
            ui_strcpy(widget->textbox.text, text, MOS_UI_MAX_TEXT);
            widget->textbox.cursor_pos = ui_strlen(text);
            break;
        case MOS_WIDGET_CHECKBOX:
            ui_strcpy(widget->checkbox.text, text, MOS_UI_MAX_TEXT);
            break;
        default:
            break;
    }
}

const char* mos_ui_widget_get_text(MosWidget* widget) {
    if (!widget) return "";
    switch (widget->type) {
        case MOS_WIDGET_LABEL:
        case MOS_WIDGET_BUTTON:
            return widget->label.text;
        case MOS_WIDGET_TEXTBOX:
        case MOS_WIDGET_PASSWORD:
            return widget->textbox.text;
        case MOS_WIDGET_CHECKBOX:
            return widget->checkbox.text;
        default:
            return "";
    }
}

// -----------------------------------------------------------------------------
// B.6 Login Screen
// -----------------------------------------------------------------------------

static void login_button_click(MosWidget* widget, void* data);

static void setup_login_screen(void) {
    // Initialize login window
    ui_memset(&g_login_window, 0, sizeof(g_login_window));
    g_login_window.id = 1;
    ui_strcpy(g_login_window.title, "MillenniumOS Login", MOS_UI_MAX_TEXT);
    g_login_window.active = true;
    g_login_window.fullscreen = true;
    g_login_window.bounds.x = 0;
    g_login_window.bounds.y = 0;
    g_login_window.bounds.width = g_ui.screen_width;
    g_login_window.bounds.height = g_ui.screen_height;
    g_login_window.bg_color = MOS_COLOR_VOID;
    g_login_window.show_titlebar = false;
    g_login_window.show_border = false;

    // Center position for login box
    int32_t box_width = 400;
    int32_t box_height = 300;
    int32_t box_x = (g_ui.screen_width - box_width) / 2;
    int32_t box_y = (g_ui.screen_height - box_height) / 2;

    // Title label
    MosWidget* title = mos_ui_create_label(&g_login_window,
        MOS_RECT(box_x, box_y, box_width, 40), "MillenniumOS");
    if (title) {
        title->label.center_text = true;
        title->fg_color = MOS_COLOR_GOLD;
    }

    // Subtitle
    MosWidget* subtitle = mos_ui_create_label(&g_login_window,
        MOS_RECT(box_x, box_y + 40, box_width, 20), "Kingdom Technology Operating System");
    if (subtitle) {
        subtitle->label.center_text = true;
        subtitle->fg_color = MOS_COLOR_TEXT_DIM;
    }

    // Username label
    mos_ui_create_label(&g_login_window,
        MOS_RECT(box_x + 50, box_y + 100, 100, 20), "Username:");

    // Username field
    g_login_screen.login.username_field = mos_ui_create_textbox(&g_login_window,
        MOS_RECT(box_x + 50, box_y + 125, box_width - 100, 35), "Enter username");

    // Password label
    mos_ui_create_label(&g_login_window,
        MOS_RECT(box_x + 50, box_y + 170, 100, 20), "Password:");

    // Password field
    g_login_screen.login.password_field = mos_ui_create_password(&g_login_window,
        MOS_RECT(box_x + 50, box_y + 195, box_width - 100, 35), "Enter password");

    // Login button
    g_login_screen.login.login_button = mos_ui_create_button(&g_login_window,
        MOS_RECT(box_x + 50, box_y + 250, box_width - 100, 40), "Login",
        login_button_click);

    // Set initial focus
    g_login_window.focused_widget = g_login_screen.login.username_field;
    if (g_login_screen.login.username_field) {
        g_login_screen.login.username_field->state = MOS_WIDGET_FOCUSED;
    }

    // Setup screen
    g_login_screen.type = MOS_SCREEN_LOGIN;
    g_login_screen.window = &g_login_window;
    g_login_screen.needs_redraw = true;
}

static void login_button_click(MosWidget* widget, void* data) {
    (void)widget;
    (void)data;

    const char* username = mos_ui_widget_get_text(g_login_screen.login.username_field);
    const char* password = mos_ui_widget_get_text(g_login_screen.login.password_field);

    // Simple authentication (in real OS, this would check against user database)
    // Accept any non-empty username/password for demo
    if (username[0] != '\0') {
        mos_ui_login(username, password);
        mos_ui_show_home();
    }
}

// -----------------------------------------------------------------------------
// B.7 Home Screen
// -----------------------------------------------------------------------------

static void app_icon_click(MosWidget* widget, void* data);

static void setup_home_screen(void) {
    // Initialize home window
    ui_memset(&g_home_window, 0, sizeof(g_home_window));
    g_home_window.id = 2;
    ui_strcpy(g_home_window.title, "Home", MOS_UI_MAX_TEXT);
    g_home_window.active = true;
    g_home_window.fullscreen = true;
    g_home_window.bounds.x = 0;
    g_home_window.bounds.y = 0;
    g_home_window.bounds.width = g_ui.screen_width;
    g_home_window.bounds.height = g_ui.screen_height;
    g_home_window.bg_color = MOS_COLOR_VOID;
    g_home_window.show_titlebar = false;
    g_home_window.show_border = false;

    // Top bar
    MosWidget* topbar_bg = mos_ui_create_widget(&g_home_window, MOS_WIDGET_PANEL,
        MOS_RECT(0, 0, g_ui.screen_width, 40));
    if (topbar_bg) {
        topbar_bg->bg_color = MOS_COLOR_WORD;
        topbar_bg->flags &= ~MOS_WIDGET_FOCUSABLE;
    }

    // System title
    MosWidget* sys_title = mos_ui_create_label(&g_home_window,
        MOS_RECT(10, 10, 200, 20), "MillenniumOS");
    if (sys_title) {
        sys_title->fg_color = MOS_COLOR_GOLD;
    }

    // User name (will be updated when logged in)
    MosWidget* user_label = mos_ui_create_label(&g_home_window,
        MOS_RECT(g_ui.screen_width - 210, 10, 200, 20), "Welcome, User");
    if (user_label) {
        user_label->fg_color = MOS_COLOR_TEXT;
    }

    // CPI-SI state indicator (shows system cognitive state)
#if CPISI_MODE >= 2
    // Full CPISI available - display actual state
    const MosCpisiState* cpisi = mos_cpisi_info();
    const char* state_name = mos_cpisi_state_name(cpisi->system_identity.construct.state);
    MosWidget* cpisi_label = mos_ui_create_label(&g_home_window,
        MOS_RECT(g_ui.screen_width / 2 - 50, 10, 100, 20), state_name);
    if (cpisi_label) {
        // Color based on state: gold for yashar (centered), green for positive, red for negative
        trit5_t state = cpisi->system_identity.construct.state;
        if (state == MOS_CPISI_YASHAR) {
            cpisi_label->fg_color = MOS_COLOR_GOLD;     // Center = gold
        } else if (state >= MOS_CPISI_TAMIM) {
            cpisi_label->fg_color = MOS_COLOR_GREEN;    // Positive states = green
        } else {
            cpisi_label->fg_color = MOS_COLOR_RED;      // Negative states = red
        }
    }
#else
    // Freestanding mode - show default "yashar" state
    MosWidget* cpisi_label = mos_ui_create_label(&g_home_window,
        MOS_RECT(g_ui.screen_width / 2 - 50, 10, 100, 20), "yashar");
    if (cpisi_label) {
        cpisi_label->fg_color = MOS_COLOR_GOLD;  // Center = gold (default)
    }
#endif

    // Application grid area
    int32_t grid_start_y = 80;
    int32_t icon_size = 80;
    int32_t icon_spacing = 20;
    int32_t icons_per_row = 6;

    // Create app icons
    for (mos_size_t i = 0; i < g_ui.app_count && i < 27; i++) {
        int32_t row = i / icons_per_row;
        int32_t col = i % icons_per_row;
        int32_t x = 50 + col * (icon_size + icon_spacing);
        int32_t y = grid_start_y + row * (icon_size + icon_spacing + 20);

        MosWidget* icon = mos_ui_create_button(&g_home_window,
            MOS_RECT(x, y, icon_size, icon_size + 20), g_ui.apps[i].name,
            app_icon_click);
        if (icon) {
            icon->user_data = (void*)(uintptr_t)i;
            icon->bg_color = MOS_COLOR_BG_LIGHT;
            g_home_screen.home.app_icons[i] = icon;
        }
    }
    g_home_screen.home.app_count = g_ui.app_count;

    // Logout button
    MosWidget* logout = mos_ui_create_button(&g_home_window,
        MOS_RECT(g_ui.screen_width - 100, g_ui.screen_height - 50, 80, 35), "Logout",
        NULL);  // Will handle in event loop
    if (logout) {
        logout->bg_color = MOS_COLOR_VOID;  // Use void color for logout
    }

    // Setup screen
    g_home_screen.type = MOS_SCREEN_HOME;
    g_home_screen.window = &g_home_window;
    g_home_screen.needs_redraw = true;
}

static void app_icon_click(MosWidget* widget, void* data) {
    (void)data;  // Context data for future use
    if (!widget) return;
    mos_size_t app_idx = (mos_size_t)(uintptr_t)widget->user_data;
    if (app_idx < g_ui.app_count) {
        mos_ui_launch_app(g_ui.apps[app_idx].name);
    }
}

// -----------------------------------------------------------------------------
// B.8 Screen Management
// -----------------------------------------------------------------------------

mos_result_t mos_ui_show_boot(void) {
    // Simple boot splash
    mos_gfx_clear(&g_ui.screen_ctx, MOS_COLOR_VOID);

    int32_t center_x = g_ui.screen_width / 2;
    int32_t center_y = g_ui.screen_height / 2;

    // Draw logo text
    MosRect title_rect = { center_x - 150, center_y - 40, 300, 40 };
    mos_gfx_string_centered(&g_ui.screen_ctx, title_rect, "MillenniumOS",
                            MOS_COLOR_GOLD, 0);

    MosRect sub_rect = { center_x - 200, center_y, 400, 20 };
    mos_gfx_string_centered(&g_ui.screen_ctx, sub_rect, "Kingdom Technology Operating System",
                            MOS_COLOR_TEXT_DIM, 0);

    MosRect verse_rect = { center_x - 250, center_y + 60, 500, 20 };
    mos_gfx_string_centered(&g_ui.screen_ctx, verse_rect,
                            "\"In the beginning God created...\" - Genesis 1:1",
                            MOS_COLOR_TEXT_DIM, 0);

    g_ui.current_screen = &g_boot_screen;
    g_ui.screen_type = MOS_SCREEN_BOOT;

    return MOS_OK;
}

mos_result_t mos_ui_show_login(void) {
    setup_login_screen();

    g_ui.current_screen = &g_login_screen;
    g_ui.screen_type = MOS_SCREEN_LOGIN;

    mos_ui_redraw();
    return MOS_OK;
}

mos_result_t mos_ui_show_home(void) {
    setup_home_screen();

    g_ui.current_screen = &g_home_screen;
    g_ui.screen_type = MOS_SCREEN_HOME;

    mos_ui_redraw();
    return MOS_OK;
}

mos_result_t mos_ui_show_shutdown(void) {
    mos_gfx_clear(&g_ui.screen_ctx, MOS_COLOR_BLACK);

    int32_t center_x = g_ui.screen_width / 2;
    int32_t center_y = g_ui.screen_height / 2;

    MosRect msg_rect = { center_x - 150, center_y - 10, 300, 20 };
    mos_gfx_string_centered(&g_ui.screen_ctx, msg_rect, "Shutting down...",
                            MOS_COLOR_TEXT, MOS_COLOR_BLACK);

    g_ui.current_screen = &g_shutdown_screen;
    g_ui.screen_type = MOS_SCREEN_SHUTDOWN;

    return MOS_OK;
}

MosScreen* mos_ui_current_screen(void) {
    return g_ui.current_screen;
}

void mos_ui_redraw(void) {
    if (!g_ui.current_screen) return;

    MosScreen* screen = g_ui.current_screen;

    // Clear screen
    mos_gfx_clear(&screen->ctx, MOS_COLOR_VOID);

    // Draw window if present
    if (screen->window) {
        mos_ui_draw_window(screen->window, &screen->ctx);
    }

    // Draw cursor
    if (g_ui.cursor_visible) {
        mos_ui_draw_cursor(&screen->ctx);
    }

    screen->needs_redraw = false;
}

// -----------------------------------------------------------------------------
// B.9 Input Handling
// -----------------------------------------------------------------------------

static MosWidget* find_widget_at(MosWindow* window, int32_t x, int32_t y) {
    if (!window) return NULL;

    // Search in reverse order (top widgets first)
    for (int i = window->widget_count - 1; i >= 0; i--) {
        MosWidget* w = window->widgets[i];
        if (w && (w->flags & MOS_WIDGET_VISIBLE) && (w->flags & MOS_WIDGET_ENABLED)) {
            if (mos_rect_contains(w->bounds, x, y)) {
                return w;
            }
        }
    }
    return NULL;
}

static void focus_widget(MosWindow* window, MosWidget* widget) {
    if (!window) return;

    if (window->focused_widget && window->focused_widget != widget) {
        window->focused_widget->state = MOS_WIDGET_NORMAL;
    }

    window->focused_widget = widget;
    if (widget) {
        widget->state = MOS_WIDGET_FOCUSED;
    }
}

static void focus_next(MosWindow* window) {
    if (!window || window->widget_count == 0) return;

    int start = 0;
    if (window->focused_widget) {
        start = window->focused_widget->id + 1;
    }

    for (mos_size_t i = 0; i < window->widget_count; i++) {
        int idx = (start + i) % window->widget_count;
        MosWidget* w = window->widgets[idx];
        if (w && (w->flags & MOS_WIDGET_FOCUSABLE) && (w->flags & MOS_WIDGET_ENABLED)) {
            focus_widget(window, w);
            return;
        }
    }
}

// Accessibility: Shift+Tab reverse navigation
static void focus_prev(MosWindow* window) {
    if (!window || window->widget_count == 0) return;

    int start = window->widget_count - 1;
    if (window->focused_widget) {
        start = window->focused_widget->id - 1;
        if (start < 0) start = window->widget_count - 1;
    }

    for (mos_size_t i = 0; i < window->widget_count; i++) {
        int idx = start - i;
        if (idx < 0) idx += window->widget_count;
        MosWidget* w = window->widgets[idx];
        if (w && (w->flags & MOS_WIDGET_FOCUSABLE) && (w->flags & MOS_WIDGET_ENABLED)) {
            focus_widget(window, w);
            return;
        }
    }
}

void mos_ui_handle_key(MosKeyEvent* event) {
    if (!event || !g_ui.current_screen || !g_ui.current_screen->window) return;

    MosWindow* window = g_ui.current_screen->window;
    MosWidget* focused = window->focused_widget;

    if (event->state != MOS_KEY_DOWN) return;

    // Accessibility: Tab / Shift+Tab for focus navigation
    if (event->keycode == '\t') {
        if (event->modifiers & MOS_MOD_SHIFT) {
            focus_prev(window);  // Shift+Tab = go backward
        } else {
            focus_next(window);  // Tab = go forward
        }
        g_ui.current_screen->needs_redraw = true;
        return;
    }

    // Accessibility: Escape key handling
    if (event->keycode == 0x1B) {  // Escape
        // Clear focus (deselect current widget)
        if (focused) {
            focused->state = MOS_WIDGET_NORMAL;
            window->focused_widget = NULL;
            g_ui.current_screen->needs_redraw = true;
        }
        return;
    }

    // Handle Enter for button activation
    if (event->keycode == '\n' && focused && focused->type == MOS_WIDGET_BUTTON) {
        if (focused->on_click) {
            focused->on_click(focused, focused->user_data);
        }
        return;
    }

    // Handle Space for checkbox toggle (accessibility pattern)
    if (event->keycode == ' ' && focused && focused->type == MOS_WIDGET_CHECKBOX) {
        focused->checkbox.checked = !focused->checkbox.checked;
        if (focused->on_change) {
            focused->on_change(focused, focused->user_data);
        }
        g_ui.current_screen->needs_redraw = true;
        return;
    }

    // Handle text input
    if (focused && (focused->type == MOS_WIDGET_TEXTBOX ||
                    focused->type == MOS_WIDGET_PASSWORD)) {
        mos_size_t len = ui_strlen(focused->textbox.text);

        if (event->keycode == '\b') {  // Backspace
            if (len > 0) {
                focused->textbox.text[len - 1] = '\0';
                focused->textbox.cursor_pos = len - 1;
            }
        } else if (event->character >= 32 && event->character < 127) {
            if (len < (mos_size_t)focused->textbox.max_length) {
                focused->textbox.text[len] = event->character;
                focused->textbox.text[len + 1] = '\0';
                focused->textbox.cursor_pos = len + 1;
            }
        }
        g_ui.current_screen->needs_redraw = true;
    }
}

void mos_ui_handle_mouse(MosMouseEvent* event, MosEventType type) {
    if (!event || !g_ui.current_screen || !g_ui.current_screen->window) return;

    MosWindow* window = g_ui.current_screen->window;

    // Update cursor position
    g_ui.cursor_x = event->x;
    g_ui.cursor_y = event->y;

    // Find widget under cursor
    MosWidget* under = find_widget_at(window, event->x, event->y);

    // Clear hover state on all widgets
    for (mos_size_t i = 0; i < window->widget_count; i++) {
        if (window->widgets[i]) {
            window->widgets[i]->flags &= ~MOS_WIDGET_HOVERED;
            window->widgets[i]->flags &= ~MOS_WIDGET_PRESSED;
        }
    }

    // Set hover state
    if (under) {
        under->flags |= MOS_WIDGET_HOVERED;
    }

    // Handle click
    if (type == MOS_EVENT_MOUSE_BUTTON && (event->buttons & MOS_MOUSE_LEFT)) {
        if (under) {
            under->flags |= MOS_WIDGET_PRESSED;
            focus_widget(window, under);

            // Trigger callback
            if (under->on_click) {
                under->on_click(under, under->user_data);
            }

            // Handle checkbox toggle
            if (under->type == MOS_WIDGET_CHECKBOX) {
                under->checkbox.checked = !under->checkbox.checked;
                if (under->on_change) {
                    under->on_change(under, under->user_data);
                }
            }
        }
    }

    g_ui.current_screen->needs_redraw = true;
}

void mos_ui_process_event(MosEvent* event) {
    if (!event) return;

    switch (event->type) {
        case MOS_EVENT_KEY:
            mos_ui_handle_key(&event->key);
            break;
        case MOS_EVENT_MOUSE_MOVE:
        case MOS_EVENT_MOUSE_BUTTON:
            mos_ui_handle_mouse(&event->mouse, event->type);
            break;
        case MOS_EVENT_QUIT:
            mos_ui_show_shutdown();
            break;
        default:
            break;
    }
}

void mos_ui_event_loop(void) {
    MosEvent event;

    while (g_ui.screen_type != MOS_SCREEN_SHUTDOWN) {
        // Process all pending events
        while (mos_hal_poll_event(&event)) {
            mos_ui_process_event(&event);
        }

        // Redraw if needed
        if (g_ui.current_screen && g_ui.current_screen->needs_redraw) {
            mos_ui_redraw();
        }

        // Wait for next event
        mos_hal_wait_event(&event);
        mos_ui_process_event(&event);
    }
}

// -----------------------------------------------------------------------------
// B.10 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_ui_init(void) {
    ui_memset(&g_ui, 0, sizeof(g_ui));
    ui_memset(&g_boot_screen, 0, sizeof(g_boot_screen));
    ui_memset(&g_login_screen, 0, sizeof(g_login_screen));
    ui_memset(&g_home_screen, 0, sizeof(g_home_screen));
    ui_memset(&g_shutdown_screen, 0, sizeof(g_shutdown_screen));
    ui_memset(g_widget_pool, 0, sizeof(g_widget_pool));

    g_widget_next = 0;
    g_ui.health = MOS_HEALTH_NEUTRAL;
    g_ui.cursor_visible = true;

    return MOS_OK;
}

mos_result_t mos_ui_init_graphics(void) {
    // Create screen graphics context
    mos_result_t res = mos_graphics_create_context(&g_ui.screen_ctx, 0);
    if (res != MOS_OK) return res;

    g_ui.screen_width = g_ui.screen_ctx.width;
    g_ui.screen_height = g_ui.screen_ctx.height;

    // Also set for screens
    g_boot_screen.ctx = g_ui.screen_ctx;
    g_login_screen.ctx = g_ui.screen_ctx;
    g_home_screen.ctx = g_ui.screen_ctx;
    g_shutdown_screen.ctx = g_ui.screen_ctx;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.11 Application Management
// -----------------------------------------------------------------------------

mos_result_t mos_ui_register_app(const char* name, const char* path,
                                  uint8_t icon_id, MosZone zone) {
    if (g_ui.app_count >= 27) return MOS_ERR_NOMEM;

    MosAppEntry* app = &g_ui.apps[g_ui.app_count++];
    ui_strcpy(app->name, name, 64);
    ui_strcpy(app->path, path, MOS_MAX_PATH);
    app->icon_id = icon_id;
    app->zone = zone;
    app->installed = true;

    return MOS_OK;
}

mos_result_t mos_ui_launch_app(const char* name) {
    // Find app
    for (mos_size_t i = 0; i < g_ui.app_count; i++) {
        if (ui_strcmp(g_ui.apps[i].name, name) == 0) {
            // TODO: Actually launch the application via exec
            // For now, just log it
            return MOS_OK;
        }
    }
    return MOS_ERR_NOTFOUND;
}

const MosAppEntry* mos_ui_get_apps(mos_size_t* count) {
    if (count) *count = g_ui.app_count;
    return g_ui.apps;
}

// -----------------------------------------------------------------------------
// B.12 User Session
// -----------------------------------------------------------------------------

mos_result_t mos_ui_login(const char* username, const char* password) {
    (void)password;  // Would validate in real implementation

    if (!username || username[0] == '\0') {
        return MOS_ERR_INVALID;
    }

    ui_strcpy(g_ui.current_user, username, 64);
    g_ui.logged_in = true;

    return MOS_OK;
}

mos_result_t mos_ui_logout(void) {
    g_ui.logged_in = false;
    g_ui.current_user[0] = '\0';
    return MOS_OK;
}

bool mos_ui_is_logged_in(void) {
    return g_ui.logged_in;
}

const char* mos_ui_current_user(void) {
    return g_ui.current_user;
}

// -----------------------------------------------------------------------------
// B.13 Cursor
// -----------------------------------------------------------------------------

void mos_ui_draw_cursor(MosGraphicsContext* ctx) {
    if (!ctx || !g_ui.cursor_visible) return;

    // Simple arrow cursor
    int32_t x = g_ui.cursor_x;
    int32_t y = g_ui.cursor_y;

    // White arrow with black outline
    for (int dy = 0; dy < 16; dy++) {
        int width = (dy < 12) ? (dy / 2 + 1) : (12 - (dy - 12));
        for (int dx = 0; dx < width; dx++) {
            mos_gfx_pixel(ctx, x + dx, y + dy, MOS_COLOR_WHITE);
        }
    }
    // Outline
    mos_gfx_line(ctx, x, y, x, y + 15, MOS_COLOR_BLACK);
    mos_gfx_line(ctx, x, y, x + 6, y + 12, MOS_COLOR_BLACK);
}

void mos_ui_show_cursor(bool visible) {
    g_ui.cursor_visible = visible;
}

// -----------------------------------------------------------------------------
// B.14 Information
// -----------------------------------------------------------------------------

const MosUI* mos_ui_info(void) {
    return &g_ui;
}

mos_health_t mos_ui_health(void) {
    return g_ui.health;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// UI implementation provides:
//   - Boot splash screen
//   - Login screen with username/password
//   - Home screen with application icons
//   - Widget system (labels, buttons, textboxes, etc.)
//   - Input handling (keyboard, mouse)
//   - User session management
//
// The UI is the face of MillenniumOS, through which users
// interact with the Kingdom Technology system.
//
// =============================================================================
// END CLOSING
// =============================================================================
