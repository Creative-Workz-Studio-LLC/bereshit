// #!omni code --c -executable
// #!omni meta.key = B-word-work-pkg-omni-ide-main
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_ide_main.c — OmniCode IDE Entry Point
// Launches TUI (ncurses) or GUI (Cornerstone) editor

#define _POSIX_C_SOURCE 200809L

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-main
// Grounding: Proverbs 22:29 - "Seest thou a man diligent in his work?"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_ide.h"

// CLI base support - uses display_* abstraction only
#ifdef IDE_CLI_BASE
#include "omni_ide_cli.h"
#include "display.h"  // For DisplayMode type
#else
#include "omni_ide_tui.h"
#endif

// GUI support - conditionally compiled if Cornerstone available
#ifdef IDE_GUI_ENABLED
#include "omni_ide_gui.h"
#include "engine/platform/common/platform.h"  // Platform layer for GUI
#include "engine/graphics/include/renderer.h" // Renderer for GUI
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// Use the display layer's global shutdown flag (signal-safe)
#include "display.h"  // For g_display_shutdown_requested

// Signal handling for clean shutdown (enables screenshot capture)
volatile sig_atomic_t g_shutdown_requested = 0;

static void signal_handler(int sig) {
    (void)sig;
    g_shutdown_requested = 1;
    // Also set the display layer's shutdown flag (signal-safe atomic write)
    g_display_shutdown_requested = 1;
}

static void setup_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
}

// Display mode selection
typedef enum {
    MODE_TUI,   // ncurses terminal (default)
    MODE_GUI,   // Cornerstone graphics
    MODE_CLI    // CLI base (display_* only)
} IDEDisplayMode;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

static void print_usage(const char* progname) {
    printf("OmniCode IDE - Kingdom Technology Editor\n");
    printf("\n");
    printf("Usage: %s [options] [file]\n", progname);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -v, --version  Show version information\n");
#ifdef IDE_GUI_ENABLED
    printf("  --gui          Use graphical interface (default)\n");
    printf("  --tui          Use terminal interface\n");
#else
    printf("  --tui          Use terminal interface (default)\n");
#endif
    printf("  --cli          Use pure ANSI terminal\n");
    printf("\n");
    printf("If no file is specified, opens a new buffer.\n");
    printf("\n");
    printf("Keybindings:\n");
    printf("  Ctrl+S         Save file\n");
    printf("  Ctrl+Q         Quit (prompts if unsaved)\n");
    printf("  Ctrl+O         Open file\n");
    printf("  Ctrl+G         Go to line\n");
    printf("  Ctrl+F         Find text\n");
    printf("  F10            Open menu\n");
    printf("  Arrow keys     Move cursor\n");
    printf("  Page Up/Down   Scroll page\n");
    printf("  Home/End       Start/end of line\n");
    printf("\n");
    printf("\"Kingdom Technology excludes no one.\"\n");
}

static void print_version(void) {
    printf("OmniCode IDE a-01.00\n");
    printf("Part of the Bereshit Kingdom Technology Foundation\n");
    printf("\n");
    printf("Display modes: TUI (ncurses)");
#ifdef IDE_GUI_ENABLED
    printf(", GUI (Cornerstone)");
#endif
    printf("\n\n");
    printf("\"In the beginning was the Word.\" — John 1:1\n");
}

int main(int argc, char* argv[]) {
    // Set up signal handlers for clean shutdown (SIGTERM, SIGINT)
    setup_signal_handlers();

    const char* filepath = NULL;
#ifdef IDE_GUI_ENABLED
    IDEDisplayMode mode = MODE_GUI;  // Default to GUI when available
#else
    IDEDisplayMode mode = MODE_TUI;  // Fallback to TUI
#endif

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        }
        if (strcmp(argv[i], "--tui") == 0) {
            mode = MODE_TUI;
            continue;
        }
        if (strcmp(argv[i], "--gui") == 0) {
#ifdef IDE_GUI_ENABLED
            mode = MODE_GUI;
#else
            fprintf(stderr, "Warning: GUI mode not available, using TUI\n");
            mode = MODE_TUI;
#endif
            continue;
        }
        if (strcmp(argv[i], "--cli") == 0) {
            mode = MODE_CLI;
            continue;
        }
        // Assume it's a filename
        filepath = argv[i];
    }

    // Create or load buffer
    IDEBuffer* buffer = NULL;
    if (filepath) {
        buffer = ide_buffer_load(filepath);
        if (!buffer) {
            fprintf(stderr, "Error: Could not open file: %s\n", filepath);
            return 1;
        }
    } else {
        buffer = ide_buffer_create();
        if (!buffer) {
            fprintf(stderr, "Error: Could not create buffer\n");
            return 1;
        }
    }

    // Analyze structure
    ide_tokenize_all(buffer);
    ide_update_blocks(buffer);

    // Run editor based on mode
    int result = 0;

#ifdef IDE_CLI_BASE
    // CLI Base: Single implementation using display_* abstraction
    // The mode determines which display backend gets used
    DisplayMode display_mode = DISPLAY_TUI;  // Default
    if (mode == MODE_GUI) display_mode = DISPLAY_GUI;
    else if (mode == MODE_CLI) display_mode = DISPLAY_CLI;

#ifdef IDE_GUI_ENABLED
    // Initialize platform for GUI mode (creates GLFW window)
    bool platform_initialized = false;
    if (display_mode == DISPLAY_GUI) {
        PlatformConfig pconfig = {
            .title = "OmniCode IDE",
            .width = 1280,
            .height = 720,
            .vsync = true,
            .app_id = "omni-ide"
        };
        if (platform_init(&pconfig) != PLATFORM_OK) {
            fprintf(stderr, "Warning: Platform init failed, falling back to TUI\n");
            display_mode = DISPLAY_TUI;
        } else {
            // Initialize renderer (required for GUI display layer)
            RendererError rerr = renderer_init(pconfig.width, pconfig.height);
            if (rerr != RENDERER_OK) {
                fprintf(stderr, "Warning: Renderer init failed, falling back to TUI\n");
                platform_shutdown();
                display_mode = DISPLAY_TUI;
            } else {
                platform_initialized = true;
            }
        }
    }
#endif

    IDECLI* cli = ide_cli_create(buffer, display_mode);
    if (!cli) {
        fprintf(stderr, "Error: Could not initialize IDE\n");
#ifdef IDE_GUI_ENABLED
        if (platform_initialized) {
            renderer_shutdown();
            platform_shutdown();
        }
#endif
        ide_buffer_free(buffer);
        return 1;
    }

    ide_cli_run(cli);
    ide_cli_destroy(cli);

#ifdef IDE_GUI_ENABLED
    if (platform_initialized) {
        renderer_shutdown();
        platform_shutdown();
    }
#endif
#else
    // Traditional mode: Separate TUI/GUI implementations
    if (mode == MODE_TUI || mode == MODE_CLI) {
        // TUI Mode (ncurses) - also handles CLI mode in traditional build
        IDETUI* tui = ide_tui_create(buffer);
        if (!tui) {
            fprintf(stderr, "Error: Could not initialize TUI\n");
            ide_buffer_free(buffer);
            return 1;
        }

        ide_tui_run(tui);
        ide_tui_destroy(tui);
    }
#ifdef IDE_GUI_ENABLED
    else if (mode == MODE_GUI) {
        // GUI Mode (Cornerstone)
        IDEGUI* gui = ide_gui_create(buffer);
        if (!gui) {
            fprintf(stderr, "Error: Could not initialize GUI, trying TUI...\n");

            // Fallback to TUI
            IDETUI* tui = ide_tui_create(buffer);
            if (!tui) {
                fprintf(stderr, "Error: Could not initialize TUI either\n");
                ide_buffer_free(buffer);
                return 1;
            }
            ide_tui_run(tui);
            ide_tui_destroy(tui);
        } else {
            ide_gui_run(gui);
            ide_gui_destroy(gui);
        }
    }
#endif
#endif  // IDE_CLI_BASE

    // Clean up
    ide_buffer_free(buffer);

    return result;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Kingdom Technology excludes no one."
// TUI works everywhere. GUI serves modern systems.
// Both provide the same editing experience.

// =============================================================================
// END CLOSING
// =============================================================================
