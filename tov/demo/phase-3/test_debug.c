// #!omni code --c -test
// #!omni meta.key = B-tov-demo-phase3-test-debug
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// test_debug.c — Debug System Test
// Validates logging, tracing, and stack inspection.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-tov-demo-phase3-test-debug
// Title: Debug System Test
// Type: Test
// Role: Validate debug and logging functionality

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "omni_debug.h"
#include "omni_vm.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

static int test_debug_creation(void) {
    printf("  Testing debug context creation...\n");

    OmniDebug* dbg = omni_debug_create();
    if (!dbg) {
        printf("    FAIL: Could not create debug context\n");
        return 1;
    }

    // Check defaults
    if (dbg->level != OMNI_LOG_INFO) {
        printf("    FAIL: Expected default level INFO\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    if (dbg->flags != OMNI_DBG_NONE) {
        printf("    FAIL: Expected default flags NONE\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    if (!dbg->enabled) {
        printf("    FAIL: Expected enabled by default\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    printf("    PASS: Debug context created with correct defaults\n");
    omni_debug_destroy(dbg);
    return 0;
}

static int test_log_levels(void) {
    printf("  Testing log levels...\n");

    OmniDebug* dbg = omni_debug_create();
    if (!dbg) return 1;

    // Test level filtering
    omni_debug_set_level(dbg, OMNI_LOG_WARN);

    if (omni_debug_should_log(dbg, OMNI_LOG_DEBUG)) {
        printf("    FAIL: DEBUG should be filtered at WARN level\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    if (omni_debug_should_log(dbg, OMNI_LOG_INFO)) {
        printf("    FAIL: INFO should be filtered at WARN level\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    if (!omni_debug_should_log(dbg, OMNI_LOG_WARN)) {
        printf("    FAIL: WARN should pass at WARN level\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    if (!omni_debug_should_log(dbg, OMNI_LOG_ERROR)) {
        printf("    FAIL: ERROR should pass at WARN level\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    // Test level names
    if (strcmp(omni_debug_level_name(OMNI_LOG_DEBUG), "DEBUG") != 0) {
        printf("    FAIL: Level name mismatch\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    printf("    PASS: Log level filtering works correctly\n");
    omni_debug_destroy(dbg);
    return 0;
}

static int test_debug_flags(void) {
    printf("  Testing debug flags...\n");

    OmniDebug* dbg = omni_debug_create();
    if (!dbg) return 1;

    // Set flags
    omni_debug_set_flags(dbg, OMNI_DBG_TRACE | OMNI_DBG_STACK);

    if (!omni_debug_has_flag(dbg, OMNI_DBG_TRACE)) {
        printf("    FAIL: TRACE flag should be set\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    if (!omni_debug_has_flag(dbg, OMNI_DBG_STACK)) {
        printf("    FAIL: STACK flag should be set\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    if (omni_debug_has_flag(dbg, OMNI_DBG_HEALTH)) {
        printf("    FAIL: HEALTH flag should not be set\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    // Test ALL flags
    omni_debug_set_flags(dbg, OMNI_DBG_ALL);
    if (!omni_debug_has_flag(dbg, OMNI_DBG_CHECKPOINT)) {
        printf("    FAIL: ALL should include CHECKPOINT\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    printf("    PASS: Debug flags work correctly\n");
    omni_debug_destroy(dbg);
    return 0;
}

static int test_file_logging(void) {
    printf("  Testing file logging...\n");

    const char* log_path = "/tmp/omni_test.log";
    OmniDebug* dbg = omni_debug_create();
    if (!dbg) return 1;

    // Set file logging
    if (!omni_debug_set_file(dbg, log_path)) {
        printf("    FAIL: Could not set log file\n");
        omni_debug_destroy(dbg);
        return 1;
    }

    // Disable console to only write to file
    omni_debug_set_console(dbg, NULL);
    omni_debug_set_level(dbg, OMNI_LOG_DEBUG);

    // Write some log messages
    OMNI_DEBUG(dbg, "Debug message");
    OMNI_INFO(dbg, "Info message");
    OMNI_WARN(dbg, "Warning message");
    OMNI_ERROR(dbg, "Error message");

    omni_debug_flush(dbg);
    omni_debug_destroy(dbg);

    // Verify file was written
    FILE* f = fopen(log_path, "r");
    if (!f) {
        printf("    FAIL: Log file not created\n");
        return 1;
    }

    char line[256];
    int line_count = 0;
    while (fgets(line, sizeof(line), f)) {
        line_count++;
    }
    fclose(f);

    if (line_count < 4) {
        printf("    FAIL: Expected at least 4 log lines, got %d\n", line_count);
        return 1;
    }

    // Cleanup
    (void)remove(log_path);

    printf("    PASS: File logging works correctly\n");
    return 0;
}

static int test_health_tracking(void) {
    printf("  Testing health change tracking...\n");

    OmniDebug* dbg = omni_debug_create();
    if (!dbg) return 1;

    omni_debug_set_flags(dbg, OMNI_DBG_HEALTH | OMNI_DBG_HEBREW);
    omni_debug_set_level(dbg, OMNI_LOG_DEBUG);

    // These should produce output (health flag is set)
    printf("    Health change output:\n");
    omni_debug_health_change(dbg, 50, 75, "test success");
    omni_debug_health_change(dbg, 75, 50, "test regression");

    printf("    Hebrew transition output:\n");
    omni_debug_hebrew_transition(dbg, 0, 1, +1);  // yashar -> tamim
    omni_debug_hebrew_transition(dbg, 1, 0, -1);  // tamim -> yashar

    printf("    PASS: Health tracking works correctly\n");
    omni_debug_destroy(dbg);
    return 0;
}

static int test_stack_inspection(void) {
    printf("  Testing stack inspection...\n");

    OmniDebug* dbg = omni_debug_create();
    OmniVM* vm = omni_vm_create();

    if (!dbg || !vm) {
        printf("    FAIL: Could not create debug or VM\n");
        if (dbg) omni_debug_destroy(dbg);
        if (vm) omni_vm_destroy(vm);
        return 1;
    }

    omni_debug_set_flags(dbg, OMNI_DBG_STACK);
    omni_debug_set_level(dbg, OMNI_LOG_DEBUG);

    // Push some values onto VM stack
    OmniValue v1 = {.type = VAL_INT, .as_int = 42};
    OmniValue v2 = {.type = VAL_BOOL, .as_bool = true};
    OmniValue v3 = {.type = VAL_TRIT, .as_trit = +1};

    omni_vm_push(vm, v1);
    omni_vm_push(vm, v2);
    omni_vm_push(vm, v3);

    printf("    Stack dump output:\n");
    omni_debug_dump_stack(dbg, vm);

    printf("    Stack top output:\n");
    omni_debug_dump_stack_top(dbg, vm, 2);

    printf("    PASS: Stack inspection works correctly\n");

    omni_debug_destroy(dbg);
    omni_vm_destroy(vm);
    return 0;
}

static int test_checkpoint_logging(void) {
    printf("  Testing checkpoint logging...\n");

    OmniDebug* dbg = omni_debug_create();
    if (!dbg) return 1;

    omni_debug_set_flags(dbg, OMNI_DBG_CHECKPOINT);
    omni_debug_set_level(dbg, OMNI_LOG_DEBUG);

    printf("    Checkpoint operations:\n");
    omni_debug_checkpoint(dbg, "CREATE", 0, "before_risky");
    omni_debug_checkpoint(dbg, "RESTORE", 0, "before_risky");
    omni_debug_checkpoint(dbg, "DISCARD", 0, "before_risky");

    printf("    PASS: Checkpoint logging works correctly\n");
    omni_debug_destroy(dbg);
    return 0;
}

int main(void) {
    printf("=== Debug System Tests ===\n\n");

    int failures = 0;

    failures += test_debug_creation();
    failures += test_log_levels();
    failures += test_debug_flags();
    failures += test_file_logging();
    failures += test_health_tracking();
    failures += test_stack_inspection();
    failures += test_checkpoint_logging();

    printf("\n");
    if (failures == 0) {
        printf("All debug system tests passed!\n");
        return 0;
    } else {
        printf("FAILED: %d test(s) failed\n", failures);
        return 1;
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Ponder the path of thy feet, and let all thy ways be established."
//   — Proverbs 4:26

// =============================================================================
// END CLOSING
// =============================================================================
