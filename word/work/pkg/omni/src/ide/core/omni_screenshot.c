// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-screenshot
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_screenshot.c — Session screenshot capture for UI development feedback
// Captures last known state before shutdown for visual debugging

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-screenshot
// Scripture: Proverbs 29:18 - "Where there is no vision, the people perish"
// Purpose: Enable visual feedback loop for UI development

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// CPI-SI state-aware logging
#include "kernel/dar/phase/detect.phase.h"

#ifdef IDE_GUI_ENABLED
#include "platform.hal.h"
#endif

#ifdef IDE_TUI_ENABLED
#include <ncurses.h>
#endif

// Screenshot base directory (relative to working directory)
#define SCREENSHOT_BASE "build/screenshots"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Directory Management
// -----------------------------------------------------------------------------

// Ensure directory exists, creating parent directories as needed
static int ensure_directory(const char* path) {
    char tmp[512];
    char* p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    // Remove trailing slash
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }

    // Create each directory in path
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }

    // Create final directory
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
        return -1;
    }

    return 0;
}

// Get timestamp string for filename (YYYY-MM-DD_HH-MM-SS)
static void get_timestamp(char* buf, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buf, size, "%Y-%m-%d_%H-%M-%S", tm_info);
}

// Get date string for directory (YYYY-MM-DD)
static void get_date_dir(char* buf, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buf, size, "%Y-%m-%d", tm_info);
}

// -----------------------------------------------------------------------------
// B.2 GUI Screenshot
// -----------------------------------------------------------------------------

#ifdef IDE_GUI_ENABLED
// Capture GUI framebuffer to PNG
int screenshot_gui(const char* label) {
    char date_dir[32];
    char timestamp[32];
    char dir_path[256];
    char file_path[512];

    get_date_dir(date_dir, sizeof(date_dir));
    get_timestamp(timestamp, sizeof(timestamp));

    // Create directory: build/screenshots/gui/YYYY-MM-DD/
    snprintf(dir_path, sizeof(dir_path), "%s/gui/%s", SCREENSHOT_BASE, date_dir);
    if (ensure_directory(dir_path) != 0) {
        LOG_ERROR("screenshot", "Failed to create directory: %s", dir_path);
        return -1;
    }

    // Build filename: timestamp_label.png
    if (label && label[0]) {
        snprintf(file_path, sizeof(file_path), "%s/%s_%s.png", dir_path, timestamp, label);
    } else {
        snprintf(file_path, sizeof(file_path), "%s/%s.png", dir_path, timestamp);
    }

    // Capture using platform screenshot
    if (platform_screenshot(file_path) == 0) {
        LOG_INFO("screenshot", "GUI saved: %s", file_path);

        // Also save as "last-session.png" for quick access
        char last_path[512];
        snprintf(last_path, sizeof(last_path), "%s/gui/last-session.png", SCREENSHOT_BASE);
        ensure_directory(SCREENSHOT_BASE "/gui");
        platform_screenshot(last_path);

        return 0;
    }

    LOG_ERROR("screenshot", "GUI capture failed");
    return -1;
}
#endif

// -----------------------------------------------------------------------------
// B.3 TUI Screenshot
// -----------------------------------------------------------------------------

#ifdef IDE_TUI_ENABLED
// Capture TUI screen to text file (ncurses buffer dump)
int screenshot_tui(const char* label) {
    char date_dir[32];
    char timestamp[32];
    char dir_path[256];
    char file_path[512];

    get_date_dir(date_dir, sizeof(date_dir));
    get_timestamp(timestamp, sizeof(timestamp));

    // Create directory: build/screenshots/tui/YYYY-MM-DD/
    snprintf(dir_path, sizeof(dir_path), "%s/tui/%s", SCREENSHOT_BASE, date_dir);
    if (ensure_directory(dir_path) != 0) {
        LOG_ERROR("screenshot", "Failed to create directory: %s", dir_path);
        return -1;
    }

    // Build filename: timestamp_label.txt
    if (label && label[0]) {
        snprintf(file_path, sizeof(file_path), "%s/%s_%s.txt", dir_path, timestamp, label);
    } else {
        snprintf(file_path, sizeof(file_path), "%s/%s.txt", dir_path, timestamp);
    }

    // Capture screen content
    FILE* fp = fopen(file_path, "w");
    if (!fp) {
        LOG_ERROR("screenshot", "Failed to open: %s", file_path);
        return -1;
    }

    // Get screen dimensions
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // Write header
    fprintf(fp, "=== TUI Screenshot: %s ===\n", timestamp);
    fprintf(fp, "Dimensions: %dx%d\n", cols, rows);
    fprintf(fp, "=====================================\n\n");

    // Dump each line of the screen
    char line[512];
    for (int y = 0; y < rows && y < 100; y++) {
        for (int x = 0; x < cols && x < (int)sizeof(line) - 1; x++) {
            chtype ch = mvwinch(stdscr, y, x);
            line[x] = (ch & A_CHARTEXT) ? (char)(ch & A_CHARTEXT) : ' ';
        }
        line[cols < (int)sizeof(line) ? cols : (int)sizeof(line) - 1] = '\0';

        // Trim trailing whitespace
        int len = strlen(line);
        while (len > 0 && line[len-1] == ' ') {
            line[--len] = '\0';
        }

        fprintf(fp, "%s\n", line);
    }

    fclose(fp);
    LOG_INFO("screenshot", "TUI saved: %s", file_path);

    // Also save as "last-session.txt" for quick access
    char last_path[512];
    snprintf(last_path, sizeof(last_path), "%s/tui/last-session.txt", SCREENSHOT_BASE);
    ensure_directory(SCREENSHOT_BASE "/tui");

    FILE* last_fp = fopen(last_path, "w");
    if (last_fp) {
        // Re-read and copy
        fp = fopen(file_path, "r");
        if (fp) {
            int c;
            while ((c = fgetc(fp)) != EOF) {
                fputc(c, last_fp);
            }
            fclose(fp);
        }
        fclose(last_fp);
    }

    return 0;
}
#endif

// -----------------------------------------------------------------------------
// B.4 CLI Screenshot
// -----------------------------------------------------------------------------

// CLI mode can't capture screen (no access to terminal buffer)
// But we can at least log that a session ended
int screenshot_cli(const char* label) {
    char date_dir[32];
    char timestamp[32];
    char dir_path[256];
    char file_path[512];

    get_date_dir(date_dir, sizeof(date_dir));
    get_timestamp(timestamp, sizeof(timestamp));

    // Create directory: build/screenshots/cli/YYYY-MM-DD/
    snprintf(dir_path, sizeof(dir_path), "%s/cli/%s", SCREENSHOT_BASE, date_dir);
    if (ensure_directory(dir_path) != 0) {
        return -1;
    }

    // Build filename
    if (label && label[0]) {
        snprintf(file_path, sizeof(file_path), "%s/%s_%s.log", dir_path, timestamp, label);
    } else {
        snprintf(file_path, sizeof(file_path), "%s/%s.log", dir_path, timestamp);
    }

    // Just log session end
    FILE* fp = fopen(file_path, "w");
    if (fp) {
        fprintf(fp, "CLI Session ended: %s\n", timestamp);
        if (label) fprintf(fp, "Label: %s\n", label);
        fclose(fp);
        LOG_INFO("screenshot", "CLI session logged: %s", file_path);
        return 0;
    }

    return -1;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Directory structure:
//   build/screenshots/
//   ├── gui/
//   │   ├── last-session.png        <- Quick access
//   │   └── YYYY-MM-DD/
//   │       └── HH-MM-SS_label.png  <- Timestamped history
//   ├── tui/
//   │   ├── last-session.txt
//   │   └── YYYY-MM-DD/
//   │       └── HH-MM-SS_label.txt
//   └── cli/
//       └── YYYY-MM-DD/
//           └── HH-MM-SS_label.log

// =============================================================================
// END CLOSING
// =============================================================================
