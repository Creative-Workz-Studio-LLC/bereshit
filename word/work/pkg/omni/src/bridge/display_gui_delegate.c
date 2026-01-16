// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-display-gui-delegate
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// display_gui_delegate.c — GUI delegating to TUI until Cornerstone graphics ready
// All GUI calls forward to TUI backend for functional fallback
// Use ide-full for real GUI; this file enables ide-gui to work without graphics

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-display-gui-delegate
// Purpose: Allow GUI mode to work by delegating to TUI
// Grounding: Kingdom Technology excludes no one — degrade gracefully

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "display.h"
#include <stdarg.h>  // For va_list in printf variants

// Forward declarations for TUI functions we delegate to
extern DisplayError tui_display_init(const DisplayConfig* config);
extern void tui_display_shutdown(void);
extern DisplayMode tui_display_get_mode(void);
extern bool tui_display_is_running(void);
extern void tui_display_get_size(int32_t* width, int32_t* height);
extern char tui_display_get_last_char(void);
extern void tui_display_clear(void);
extern void tui_display_refresh(void);
extern void tui_display_print(const char* text);
extern void tui_display_print_at(int32_t x, int32_t y, const char* text);
extern void tui_display_print_styled(const char* text, DisplayStyle style);
extern void tui_display_print_line(const char* text);
extern void tui_display_print_centered(int32_t y, const char* text);
extern void tui_display_draw_box(int32_t x, int32_t y, int32_t w, int32_t h);
extern void tui_display_draw_box_titled(int32_t x, int32_t y, int32_t w, int32_t h, const char* title);
extern void tui_display_set_cursor(int32_t x, int32_t y);
extern void tui_display_show_cursor(void);
extern void tui_display_hide_cursor(void);
extern void tui_display_clear_to_eol(void);
extern void tui_display_putchar(char c);
extern void tui_display_putchar_at(int32_t x, int32_t y, char c);
extern void tui_display_printf(const char* fmt, va_list args);
extern void tui_display_printf_at(int32_t x, int32_t y, const char* fmt, va_list args);
extern void tui_display_set_style(DisplayStyle style);
extern void tui_display_reset_style(void);
extern DisplayKey tui_display_poll_key(void);
extern DisplayKey tui_display_wait_key(void);
extern bool tui_display_key_pressed(DisplayKey key);
extern int32_t tui_display_choose(const char** options, int32_t count);
extern const char* tui_display_get_error_string(DisplayError err);
extern DisplayMouseEvent tui_display_get_mouse_event(void);

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// All GUI functions delegate to TUI - GUI mode works via TUI until
// Cornerstone graphics are implemented

DisplayError gui_display_init(const DisplayConfig* config) {
    // Delegate to TUI - GUI mode uses TUI rendering for now
    return tui_display_init(config);
}

void gui_display_shutdown(void) {
    tui_display_shutdown();
}

DisplayMode gui_display_get_mode(void) {
    return DISPLAY_GUI;  // Report as GUI even though using TUI
}

bool gui_display_is_running(void) {
    return tui_display_is_running();
}

void gui_display_get_size(int32_t* width, int32_t* height) {
    tui_display_get_size(width, height);
}

char gui_display_get_last_char(void) {
    return tui_display_get_last_char();
}

void gui_display_clear(void) {
    tui_display_clear();
}

void gui_display_refresh(void) {
    tui_display_refresh();
}

void gui_display_print(const char* text) {
    tui_display_print(text);
}

void gui_display_print_at(int32_t x, int32_t y, const char* text) {
    tui_display_print_at(x, y, text);
}

void gui_display_print_styled(const char* text, DisplayStyle style) {
    tui_display_print_styled(text, style);
}

void gui_display_print_line(const char* text) {
    tui_display_print_line(text);
}

void gui_display_print_centered(int32_t y, const char* text) {
    tui_display_print_centered(y, text);
}

void gui_display_draw_box(int32_t x, int32_t y, int32_t w, int32_t h) {
    tui_display_draw_box(x, y, w, h);
}

void gui_display_draw_box_titled(int32_t x, int32_t y, int32_t w, int32_t h, const char* title) {
    tui_display_draw_box_titled(x, y, w, h, title);
}

void gui_display_set_cursor(int32_t x, int32_t y) {
    tui_display_set_cursor(x, y);
}

void gui_display_show_cursor(void) {
    tui_display_show_cursor();
}

void gui_display_hide_cursor(void) {
    tui_display_hide_cursor();
}

void gui_display_clear_to_eol(void) {
    tui_display_clear_to_eol();
}

void gui_display_putchar(char c) {
    tui_display_putchar(c);
}

void gui_display_putchar_at(int32_t x, int32_t y, char c) {
    tui_display_putchar_at(x, y, c);
}

void gui_display_printf(const char* fmt, va_list args) {
    tui_display_printf(fmt, args);
}

void gui_display_printf_at(int32_t x, int32_t y, const char* fmt, va_list args) {
    tui_display_printf_at(x, y, fmt, args);
}

void gui_display_set_style(DisplayStyle style) {
    tui_display_set_style(style);
}

void gui_display_reset_style(void) {
    tui_display_reset_style();
}

DisplayKey gui_display_poll_key(void) {
    return tui_display_poll_key();
}

DisplayKey gui_display_wait_key(void) {
    return tui_display_wait_key();
}

bool gui_display_key_pressed(DisplayKey key) {
    return tui_display_key_pressed(key);
}

int32_t gui_display_choose(const char** options, int32_t count) {
    return tui_display_choose(options, count);
}

const char* gui_display_get_error_string(DisplayError err) {
    return tui_display_get_error_string(err);
}

DisplayMouseEvent gui_display_get_mouse_event(void) {
    return tui_display_get_mouse_event();
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// GUI mode now works by delegating to TUI.
// When Cornerstone graphics are ready, replace these with real implementations.

// =============================================================================
// END CLOSING
// =============================================================================
