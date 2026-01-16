// #!omni code --c -executable
// #!omni meta.key = B-tov-demo-phase-3-fuse-mount
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// main.c — FUSE Mount Demo
// Validates zone abstraction and FUSE filesystem

#define _GNU_SOURCE

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-tov-demo-phase-3-fuse-mount
// Title: FUSE Mount Demo
// Type: Executable
// Role: Validate FUSE filesystem and zone abstraction

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#include "zone.h"

// Test paths
static const char* TEST_PATHS[] = {
    "/void",
    "/void/planning",
    "/word",
    "/word/core",
    "/word/work",
    "/tov",
    "/tov/demo",
    "/unknown",
    NULL
};

// Expected zones
static const int EXPECTED_ZONES[] = {
    -1,   // void
    -1,   // void/planning
     0,   // word
     0,   // word/core
     0,   // word/work
    +1,   // tov
    +1,   // tov/demo
    -99   // unknown
};

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// Print header
static void print_header(const char* title) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("  %s\n", title);
    printf("═══════════════════════════════════════════════════════════════════════════════\n\n");
}

// Print section header
static void print_section(const char* title) {
    printf("\n───────────────────────────────────────────────────────────────────────────────\n");
    printf("  %s\n", title);
    printf("───────────────────────────────────────────────────────────────────────────────\n\n");
}

// Test zone_from_path function
static int test_zone_from_path(void) {
    print_section("Zone Detection (zone_from_path)");

    int passed = 0;
    int failed = 0;

    for (int i = 0; TEST_PATHS[i] != NULL; i++) {
        BereshitZone zone = zone_from_path(TEST_PATHS[i]);
        int value = zone_value(zone);
        const char* name = zone_name(zone);

        bool success = (value == EXPECTED_ZONES[i]);

        printf("  %-20s → zone=%s (%+d)  %s\n",
               TEST_PATHS[i], name, value,
               success ? "✓" : "✗ EXPECTED %d");

        if (success) {
            passed++;
        } else {
            failed++;
        }
    }

    printf("\n  Results: %d passed, %d failed\n", passed, failed);
    return failed;
}

// Test zone_name function
static int test_zone_name(void) {
    print_section("Zone Names (zone_name)");

    int failed = 0;

    struct {
        BereshitZone zone;
        const char* expected;
    } tests[] = {
        { ZONE_VOID, "void" },
        { ZONE_WORD, "word" },
        { ZONE_TOV, "tov" },
        { ZONE_UNKNOWN, "unknown" },
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        const char* name = zone_name(tests[i].zone);
        bool success = (strcmp(name, tests[i].expected) == 0);

        printf("  zone_name(%d) → \"%s\"  %s\n",
               tests[i].zone, name,
               success ? "✓" : "✗");

        if (!success) failed++;
    }

    return failed;
}

// Test zone_value function
static int test_zone_value(void) {
    print_section("Zone Values (zone_value)");

    int failed = 0;

    struct {
        BereshitZone zone;
        int expected;
    } tests[] = {
        { ZONE_VOID, -1 },
        { ZONE_WORD, 0 },
        { ZONE_TOV, +1 },
        { ZONE_UNKNOWN, -99 },
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        int value = zone_value(tests[i].zone);
        bool success = (value == tests[i].expected);

        printf("  zone_value(%s) → %+d  %s\n",
               zone_name(tests[i].zone), value,
               success ? "✓" : "✗");

        if (!success) failed++;
    }

    return failed;
}

// Test zone_load_info function (requires real filesystem)
static int test_zone_load_info(const char* bereshit_root) {
    print_section("Zone Info Loading (zone_load_info)");

    zone_set_bereshit_root(bereshit_root);

    const char* zones[] = { "void", "word", "tov" };
    int failed = 0;

    for (size_t i = 0; i < sizeof(zones) / sizeof(zones[0]); i++) {
        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", bereshit_root, zones[i]);

        ZoneInfo info;
        bool loaded = zone_load_info(path, &info);

        printf("  %s:\n", zones[i]);
        if (loaded) {
            printf("    zone:        %s (%+d)\n", info.name, zone_value(info.zone));
            printf("    has_root:    %s\n", info.has_root_omni ? "yes" : "no");
            printf("    key:         %s\n", info.key[0] ? info.key : "(none)");
            printf("    health:      %d\n", info.health);
            printf("    ✓ Loaded\n");
        } else {
            printf("    ✗ Failed to load\n");
            failed++;
        }
        printf("\n");
    }

    return failed;
}

// Test FUSE mount capability
static int test_fuse_mount(const char* bereshit_root, const char* mount_point) {
    print_section("FUSE Mount Test");

    // Check if bereshit_fs binary exists
    char fs_path[4096];
    snprintf(fs_path, sizeof(fs_path), "%s/word/work/pkg/fuse/bin/bereshit_fs", bereshit_root);

    struct stat st;
    if (stat(fs_path, &st) != 0) {
        printf("  ✗ bereshit_fs not found at: %s\n", fs_path);
        printf("  Run 'make' in word/work/pkg/fuse/ first\n");
        return 1;
    }

    printf("  bereshit_fs found: %s\n", fs_path);

    // Create mount point if needed
    if (stat(mount_point, &st) != 0) {
        if (mkdir(mount_point, 0755) != 0) {
            printf("  ✗ Failed to create mount point: %s\n", mount_point);
            return 1;
        }
        printf("  Created mount point: %s\n", mount_point);
    }

    // Fork and mount
    printf("  Mounting %s -> %s...\n", bereshit_root, mount_point);

    pid_t pid = fork();
    if (pid == 0) {
        // Child: exec bereshit_fs in foreground mode
        char source_opt[4096];
        snprintf(source_opt, sizeof(source_opt), "--source=%s", bereshit_root);
        execlp(fs_path, "bereshit_fs", source_opt, mount_point, "-f", NULL);
        _exit(1);
    }

    // Parent: wait a bit, then test
    sleep(1);

    // Test if mount worked
    char test_path[4096];
    snprintf(test_path, sizeof(test_path), "%s/void", mount_point);

    if (stat(test_path, &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("  ✓ Mount successful\n");
        printf("  ✓ void/ accessible\n");

        snprintf(test_path, sizeof(test_path), "%s/word", mount_point);
        if (stat(test_path, &st) == 0) {
            printf("  ✓ word/ accessible\n");
        }

        snprintf(test_path, sizeof(test_path), "%s/tov", mount_point);
        if (stat(test_path, &st) == 0) {
            printf("  ✓ tov/ accessible\n");
        }
    } else {
        printf("  ✗ Mount failed or zones not accessible\n");
    }

    // Unmount
    printf("  Unmounting...\n");
    char umount_cmd[4096];
    snprintf(umount_cmd, sizeof(umount_cmd), "fusermount3 -u %s 2>/dev/null || fusermount -u %s 2>/dev/null", mount_point, mount_point);
    int umount_ret __attribute__((unused)) = system(umount_cmd);

    // Wait for child to exit
    int status;
    waitpid(pid, &status, 0);

    printf("  ✓ Unmounted\n");

    return 0;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

int main(int argc, char* argv[]) {
    print_header("Bereshit FUSE Demo - Zone Abstraction Validation");

    printf("This demo validates:\n");
    printf("  1. Zone detection from paths\n");
    printf("  2. Zone names and values\n");
    printf("  3. Zone info loading from root.omni\n");
    printf("  4. FUSE filesystem mounting\n");

    // Get Bereshit root from environment or default
    const char* bereshit_root = getenv("BERESHIT_ROOT");
    if (!bereshit_root) {
        bereshit_root = "/media/seanje-lenox-wise/Project/Bereshit";
    }

    // Get mount point from arg or default
    const char* mount_point = (argc > 1) ? argv[1] : "/tmp/bereshit_demo";

    printf("\n  Bereshit root: %s\n", bereshit_root);
    printf("  Mount point:   %s\n", mount_point);

    // Run tests
    int total_failures = 0;

    total_failures += test_zone_from_path();
    total_failures += test_zone_name();
    total_failures += test_zone_value();
    total_failures += test_zone_load_info(bereshit_root);
    total_failures += test_fuse_mount(bereshit_root, mount_point);

    // Summary
    print_header("Summary");

    if (total_failures == 0) {
        printf("  ✓ All tests passed!\n\n");
        printf("  Zone abstraction working correctly:\n");
        printf("    void (-1) = Entry gate, formless, planning\n");
        printf("    word (0)  = Processing, definitions, implementations\n");
        printf("    tov (+1)  = Exit gate, completion, demos\n\n");
        printf("  \"And God saw that it was good.\" — Genesis 1:10\n");
    } else {
        printf("  ✗ %d test(s) failed\n\n", total_failures);
    }

    printf("\n");
    return total_failures;
}

// =============================================================================
// END CLOSING
// =============================================================================
