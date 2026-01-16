// #!omni code --c -test
// #!omni meta.key = B-tov-demo-phase3-test-dar
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// test_dar.c — DAR System Test
// Validates Detect-Assess-Recover functionality.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-tov-demo-phase3-test-dar
// Title: DAR System Test
// Type: Test
// Role: Validate DAR checkpoint and recovery functionality

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
#include "omni_dar.h"
#include "omni_vm.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

static int test_context_creation(void) {
    printf("  Testing DAR context creation...\n");

    DARContext* ctx = dar_create(NULL);
    if (!ctx) {
        printf("    FAIL: Could not create context\n");
        return 1;
    }

    printf("    Initial health: %d\n", dar_health_get(ctx));
    printf("    Initial state: %s\n", dar_hebrew_state_name(dar_hebrew_state(ctx)));

    if (dar_health_get(ctx) != 50) {
        printf("    FAIL: Expected initial health 50\n");
        dar_destroy(ctx);
        return 1;
    }

    if (dar_hebrew_state(ctx) != HEBREW_YASHAR) {
        printf("    FAIL: Expected initial state yashar\n");
        dar_destroy(ctx);
        return 1;
    }

    printf("    PASS: Context created with correct defaults\n");
    dar_destroy(ctx);
    return 0;
}

static int test_health_operations(void) {
    printf("  Testing health operations...\n");

    DARContext* ctx = dar_create(NULL);
    if (!ctx) return 1;

    // Test set
    dar_health_set(ctx, 75);
    if (dar_health_get(ctx) != 75) {
        printf("    FAIL: health_set failed\n");
        dar_destroy(ctx);
        return 1;
    }

    // Test add
    dar_health_add(ctx, 25);
    if (dar_health_get(ctx) != 100) {
        printf("    FAIL: health_add failed\n");
        dar_destroy(ctx);
        return 1;
    }

    // Test clamping
    dar_health_add(ctx, 50);  // Should clamp at 100
    if (dar_health_get(ctx) != 100) {
        printf("    FAIL: health clamping failed\n");
        dar_destroy(ctx);
        return 1;
    }

    // Test negative
    dar_health_set(ctx, -100);
    if (dar_health_get(ctx) != -100) {
        printf("    FAIL: negative health failed\n");
        dar_destroy(ctx);
        return 1;
    }

    printf("    PASS: Health operations work correctly\n");
    dar_destroy(ctx);
    return 0;
}

static int test_hebrew_transitions(void) {
    printf("  Testing Hebrew state transitions...\n");

    DARContext* ctx = dar_create(NULL);
    if (!ctx) return 1;

    // Start at yashar (0)
    printf("    Initial: %s\n", dar_hebrew_state_name(dar_hebrew_state(ctx)));

    // Expand toward tov (+1, +1, +1)
    dar_hebrew_transition(ctx, +1);
    printf("    After +1: %s\n", dar_hebrew_state_name(dar_hebrew_state(ctx)));
    if (dar_hebrew_state(ctx) != HEBREW_TAMIM) {
        printf("    FAIL: Expected tamim after +1\n");
        dar_destroy(ctx);
        return 1;
    }

    dar_hebrew_transition(ctx, +1);
    if (dar_hebrew_state(ctx) != HEBREW_SHALEM) {
        printf("    FAIL: Expected shalem after +2\n");
        dar_destroy(ctx);
        return 1;
    }

    dar_hebrew_transition(ctx, +1);
    if (dar_hebrew_state(ctx) != HEBREW_TOV) {
        printf("    FAIL: Expected tov after +3\n");
        dar_destroy(ctx);
        return 1;
    }

    // Can't go beyond tov
    dar_hebrew_transition(ctx, +1);
    if (dar_hebrew_state(ctx) != HEBREW_TOV) {
        printf("    FAIL: Should stay at tov\n");
        dar_destroy(ctx);
        return 1;
    }

    // Retreat all the way to shavar
    for (int i = 0; i < 6; i++) {
        dar_hebrew_transition(ctx, -1);
    }
    if (dar_hebrew_state(ctx) != HEBREW_SHAVAR) {
        printf("    FAIL: Expected shavar after full retreat\n");
        dar_destroy(ctx);
        return 1;
    }

    printf("    Final: %s\n", dar_hebrew_state_name(dar_hebrew_state(ctx)));
    printf("    PASS: Hebrew transitions work correctly\n");
    dar_destroy(ctx);
    return 0;
}

static int test_checkpoints(void) {
    printf("  Testing checkpoint operations...\n");

    DARContext* ctx = dar_create(NULL);
    if (!ctx) return 1;

    // Set initial state
    dar_health_set(ctx, 80);
    dar_hebrew_transition(ctx, +1);  // tamim

    // Create checkpoint
    int cp_idx = dar_checkpoint_create(ctx, "before_risky");
    if (cp_idx < 0) {
        printf("    FAIL: Could not create checkpoint\n");
        dar_destroy(ctx);
        return 1;
    }
    printf("    Created checkpoint at index %d\n", cp_idx);

    // Modify state (simulate risky operation failing)
    dar_health_set(ctx, -50);
    dar_hebrew_transition(ctx, -1);
    dar_hebrew_transition(ctx, -1);

    printf("    After failure: health=%d, state=%s\n",
           dar_health_get(ctx), dar_hebrew_state_name(dar_hebrew_state(ctx)));

    // Assess - should recommend recovery
    int8_t assessment = dar_assess(ctx);
    printf("    Assessment: %d (expected -1 for recovery needed)\n", assessment);
    if (assessment != -1) {
        printf("    WARNING: Expected assessment -1 for critical state\n");
    }

    // Restore checkpoint
    if (!dar_checkpoint_restore(ctx, cp_idx)) {
        printf("    FAIL: Could not restore checkpoint\n");
        dar_destroy(ctx);
        return 1;
    }

    printf("    After restore: health=%d, state=%s\n",
           dar_health_get(ctx), dar_hebrew_state_name(dar_hebrew_state(ctx)));

    if (dar_health_get(ctx) != 80) {
        printf("    FAIL: Health not restored\n");
        dar_destroy(ctx);
        return 1;
    }

    printf("    PASS: Checkpoint save/restore works\n");
    dar_destroy(ctx);
    return 0;
}

static int test_health_files(void) {
    printf("  Testing health file operations...\n");

    // Create temp directory
    const char* test_dir = "/tmp/dar_test";
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", test_dir);
    (void)system(cmd);

    // Write health file
    if (!dar_health_write_dir(test_dir, 75)) {
        printf("    FAIL: Could not write health file\n");
        return 1;
    }

    // Read it back
    int8_t health = dar_health_read_dir(test_dir);
    printf("    Wrote 75, read back: %d\n", health);

    if (health != 75) {
        printf("    FAIL: Health file read/write mismatch\n");
        return 1;
    }

    // Test negative health
    dar_health_write_dir(test_dir, -50);
    health = dar_health_read_dir(test_dir);
    printf("    Wrote -50, read back: %d\n", health);

    if (health != -50) {
        printf("    FAIL: Negative health read/write failed\n");
        return 1;
    }

    // Cleanup
    snprintf(cmd, sizeof(cmd), "rm -rf %s", test_dir);
    (void)system(cmd);

    printf("    PASS: Health file operations work\n");
    return 0;
}

static int test_session_persistence(void) {
    printf("  Testing session persistence...\n");

    const char* session_file = "/tmp/test_session.dar";

    // Create context with session file
    DARContext* ctx = dar_create(session_file);
    if (!ctx) {
        printf("    FAIL: Could not create context\n");
        return 1;
    }

    // Modify state
    dar_health_set(ctx, 85);
    dar_hebrew_transition(ctx, +1);
    dar_hebrew_transition(ctx, +1);

    // Create some checkpoints
    dar_checkpoint_create(ctx, "cp1");
    dar_checkpoint_create(ctx, "cp2");

    printf("    Before save: health=%d, state=%s\n",
           dar_health_get(ctx), dar_hebrew_state_name(dar_hebrew_state(ctx)));

    // Save session
    if (!dar_session_save_default(ctx)) {
        printf("    FAIL: Could not save session\n");
        dar_destroy(ctx);
        return 1;
    }

    dar_destroy(ctx);

    // Create new context and load session
    ctx = dar_create(session_file);
    if (!ctx) {
        printf("    FAIL: Could not create new context\n");
        return 1;
    }

    printf("    After load: health=%d, state=%s\n",
           dar_health_get(ctx), dar_hebrew_state_name(dar_hebrew_state(ctx)));

    if (dar_health_get(ctx) != 85) {
        printf("    FAIL: Health not persisted\n");
        dar_destroy(ctx);
        return 1;
    }

    if (dar_hebrew_state(ctx) != HEBREW_SHALEM) {
        printf("    FAIL: Hebrew state not persisted\n");
        dar_destroy(ctx);
        return 1;
    }

    // Cleanup
    dar_destroy(ctx);
    (void)remove(session_file);

    printf("    PASS: Session persistence works\n");
    return 0;
}

static int test_vm_connection(void) {
    printf("  Testing VM connection...\n");

    DARContext* ctx = dar_create(NULL);
    OmniVM* vm = omni_vm_create();

    if (!ctx || !vm) {
        printf("    FAIL: Could not create context or VM\n");
        if (ctx) dar_destroy(ctx);
        if (vm) omni_vm_destroy(vm);
        return 1;
    }

    // Set DAR state
    dar_health_set(ctx, 90);
    dar_hebrew_transition(ctx, +1);

    // Connect to VM
    dar_connect_vm(ctx, vm);

    // Check VM has DAR state
    printf("    DAR health: %d, VM health: %d\n", dar_health_get(ctx), vm->health);

    // Modify VM directly
    vm->health = 60;
    vm->hebrew_state = HEBREW_TOV;

    // Sync from VM
    dar_sync_from_vm(ctx);

    printf("    After sync: DAR health=%d, state=%s\n",
           dar_health_get(ctx), dar_hebrew_state_name(dar_hebrew_state(ctx)));

    if (dar_health_get(ctx) != 60) {
        printf("    FAIL: Sync from VM failed\n");
        dar_destroy(ctx);
        omni_vm_destroy(vm);
        return 1;
    }

    dar_disconnect_vm(ctx);
    dar_destroy(ctx);
    omni_vm_destroy(vm);

    printf("    PASS: VM connection works\n");
    return 0;
}

int main(void) {
    printf("=== DAR System Tests ===\n\n");

    int failures = 0;

    failures += test_context_creation();
    failures += test_health_operations();
    failures += test_hebrew_transitions();
    failures += test_checkpoints();
    failures += test_health_files();
    failures += test_session_persistence();
    failures += test_vm_connection();

    printf("\n");
    if (failures == 0) {
        printf("All DAR system tests passed!\n");
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

// "A just man falleth seven times, and riseth up again."
//   — Proverbs 24:16

// =============================================================================
// END CLOSING
// =============================================================================
