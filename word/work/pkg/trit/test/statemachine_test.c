// ═══════════════════════════════════════════════════════════════════════════
// libtrit - State Machine Tests
// Key: B-word-work-pkg-trit-test-statemachine
// ═══════════════════════════════════════════════════════════════════════════
//
// Tests for CPI-SI state machine components:
//   - Hebrew states (7 positions)
//   - Cube navigation (27 positions)
//   - K:ALIGN tracking
//   - Trajectory (B.1-B.4)
//   - Unified cognitive state
//
// ═══════════════════════════════════════════════════════════════════════════

#include "trit/mind/cognitive.engine.h"  // Unified cognitive state machine
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Test counters
static int tests_run = 0;
static int tests_passed = 0;

// Test macros
#define TEST(name) \
    do { \
        printf("  Testing: %s... ", #name); \
        tests_run++; \
    } while(0)

#define PASS() \
    do { \
        tests_passed++; \
        printf("PASS\n"); \
    } while(0)

#define FAIL(msg) \
    do { \
        printf("FAIL: %s\n", msg); \
    } while(0)

#define ASSERT_EQ(a, b, msg) \
    if ((a) != (b)) { FAIL(msg); return; } else { }

#define ASSERT_FLOAT_EQ(a, b, msg) \
    if (((a) - (b)) > 0.001f || ((b) - (a)) > 0.001f) { FAIL(msg); return; } else { }

#define ASSERT_TRUE(cond, msg) \
    if (!(cond)) { FAIL(msg); return; } else { }

#define ASSERT_FALSE(cond, msg) \
    if (cond) { FAIL(msg); return; } else { }

#define ASSERT_STREQ(a, b, msg) \
    if (strcmp((a), (b)) != 0) { FAIL(msg); return; } else { }

// ════════════════════════════════════════════════════════════════
// Hebrew State Tests
// ════════════════════════════════════════════════════════════════

void test_hebrew_state_count(void) {
    TEST(hebrew_state_count);
    ASSERT_EQ(HEBREW_STATE_COUNT, 7, "Should have 7 Hebrew states");
    PASS();
}

void test_hebrew_state_values(void) {
    TEST(hebrew_state_values);

    // Check enum values
    ASSERT_EQ(HEBREW_SHAVAR, 0, "SHAVAR should be 0");
    ASSERT_EQ(HEBREW_YASHAR, 3, "YASHAR should be 3");
    ASSERT_EQ(HEBREW_TOV, 6, "TOV should be 6");

    PASS();
}

void test_hebrew_state_info(void) {
    TEST(hebrew_state_info);

    const hebrew_info_t* info = hebrew_get_info(HEBREW_YASHAR);
    ASSERT_STREQ(info->name, "yashar", "YASHAR name");
    ASSERT_STREQ(info->meaning, "EVEN", "YASHAR meaning");
    ASSERT_FLOAT_EQ(info->z_value, 0.0f, "YASHAR z-value");
    ASSERT_EQ(info->k_selector, 0, "YASHAR k-selector");
    ASSERT_EQ(info->day, 4, "YASHAR day");

    PASS();
}

void test_hebrew_z_values(void) {
    TEST(hebrew_z_values);

    ASSERT_FLOAT_EQ(hebrew_get_z_value(HEBREW_SHAVAR), -1.0f, "SHAVAR z");
    ASSERT_FLOAT_EQ(hebrew_get_z_value(HEBREW_CHASER), -0.5f, "CHASER z");
    ASSERT_FLOAT_EQ(hebrew_get_z_value(HEBREW_YASHAR), 0.0f, "YASHAR z");
    ASSERT_FLOAT_EQ(hebrew_get_z_value(HEBREW_TOV), 1.0f, "TOV z");

    PASS();
}

void test_hebrew_from_z_value(void) {
    TEST(hebrew_from_z_value);

    // Pure states
    ASSERT_EQ(hebrew_from_z_value(-1.0f, 0), HEBREW_SHAVAR, "z=-1 → SHAVAR");
    ASSERT_EQ(hebrew_from_z_value(0.0f, 0), HEBREW_YASHAR, "z=0 → YASHAR");
    ASSERT_EQ(hebrew_from_z_value(1.0f, 0), HEBREW_TOV, "z=1 → TOV");

    // Ambiguous zones with k
    ASSERT_EQ(hebrew_from_z_value(-0.5f, -1), HEBREW_CHASER, "z=-0.5,k=-1 → CHASER");
    ASSERT_EQ(hebrew_from_z_value(-0.5f, +1), HEBREW_RATSAH, "z=-0.5,k=+1 → RATSAH");

    PASS();
}

void test_hebrew_navigation(void) {
    TEST(hebrew_navigation);

    // Toward tov
    ASSERT_EQ(hebrew_next_toward_tov(HEBREW_SHAVAR), HEBREW_RATSAH, "SHAVAR→tov");
    ASSERT_EQ(hebrew_next_toward_tov(HEBREW_YASHAR), HEBREW_SHALEM, "YASHAR→tov");
    ASSERT_EQ(hebrew_next_toward_tov(HEBREW_TOV), HEBREW_TOV, "TOV→tov (max)");

    // Toward shavar
    ASSERT_EQ(hebrew_next_toward_shavar(HEBREW_TOV), HEBREW_TAMIM, "TOV→shavar");
    ASSERT_EQ(hebrew_next_toward_shavar(HEBREW_YASHAR), HEBREW_CHASER, "YASHAR→shavar");
    ASSERT_EQ(hebrew_next_toward_shavar(HEBREW_SHAVAR), HEBREW_SHAVAR, "SHAVAR→shavar (min)");

    PASS();
}

void test_hebrew_pure_states(void) {
    TEST(hebrew_pure_states);

    ASSERT_TRUE(hebrew_is_pure_state(HEBREW_SHAVAR), "SHAVAR is pure");
    ASSERT_TRUE(hebrew_is_pure_state(HEBREW_YASHAR), "YASHAR is pure");
    ASSERT_TRUE(hebrew_is_pure_state(HEBREW_TOV), "TOV is pure");

    ASSERT_FALSE(hebrew_is_pure_state(HEBREW_CHASER), "CHASER not pure");
    ASSERT_FALSE(hebrew_is_pure_state(HEBREW_SHALEM), "SHALEM not pure");

    PASS();
}

// ════════════════════════════════════════════════════════════════
// Cube Navigation Tests
// ════════════════════════════════════════════════════════════════

void test_cube_center(void) {
    TEST(cube_center);

    cube_position_t center = cube_position_center();
    ASSERT_EQ(center.x, 0, "center.x");
    ASSERT_EQ(center.y, 0, "center.y");
    ASSERT_EQ(center.z, 0, "center.z");

    ASSERT_TRUE(cube_is_center(center), "is_center");

    PASS();
}

void test_cube_index_conversion(void) {
    TEST(cube_index_conversion);

    // Center should be index 13
    cube_position_t center = cube_position_center();
    ASSERT_EQ(cube_position_to_index(center), 13, "center index");

    // Round-trip
    for (cube_index_t i = 0; i < 27; i++) {
        cube_position_t pos = cube_position_from_index(i);
        cube_index_t back = cube_position_to_index(pos);
        if (back != i) {
            FAIL("index round-trip");
            return;
        }
    }

    PASS();
}

void test_cube_movement(void) {
    TEST(cube_movement);

    cube_position_t pos = cube_position_center();

    // Move X positive
    pos = cube_move_x(pos, DIR_BUILD_UP);
    ASSERT_EQ(pos.x, 1, "move_x +1");

    // Move Y negative
    pos = cube_move_y(pos, DIR_BREAK_DOWN);
    ASSERT_EQ(pos.y, -1, "move_y -1");

    // Move Z positive
    pos = cube_move_z(pos, DIR_BUILD_UP);
    ASSERT_EQ(pos.z, 1, "move_z +1");

    PASS();
}

void test_cube_boundaries(void) {
    TEST(cube_boundaries);

    // Create corner position
    cube_position_t corner = cube_position_create(1, 1, 1);

    // Try to move beyond boundary
    cube_position_t moved = cube_move_x(corner, DIR_BUILD_UP);
    ASSERT_EQ(moved.x, 1, "clamped at +1");

    // Other direction
    corner = cube_position_create(-1, -1, -1);
    moved = cube_move_z(corner, DIR_BREAK_DOWN);
    ASSERT_EQ(moved.z, -1, "clamped at -1");

    PASS();
}

void test_cube_regions(void) {
    TEST(cube_regions);

    cube_position_t center = cube_position_center();
    ASSERT_EQ(cube_get_region(center), CUBE_REGION_CENTER, "center region");

    cube_position_t face = cube_position_create(1, 0, 0);
    ASSERT_EQ(cube_get_region(face), CUBE_REGION_FACE, "face region");

    cube_position_t edge = cube_position_create(1, 1, 0);
    ASSERT_EQ(cube_get_region(edge), CUBE_REGION_EDGE, "edge region");

    cube_position_t corner = cube_position_create(1, 1, 1);
    ASSERT_EQ(cube_get_region(corner), CUBE_REGION_CORNER, "corner region");

    PASS();
}

void test_cube_distance(void) {
    TEST(cube_distance);

    cube_position_t a = cube_position_center();
    cube_position_t b = cube_position_create(1, 1, 1);

    ASSERT_EQ(cube_distance(a, b), 3, "center to corner");
    ASSERT_EQ(cube_distance(a, a), 0, "same position");

    PASS();
}

// ════════════════════════════════════════════════════════════════
// K:ALIGN Tests
// ════════════════════════════════════════════════════════════════

void test_kalign_neutral(void) {
    TEST(kalign_neutral);

    k_align_t k = k_align_neutral();
    ASSERT_EQ(k.k_structural, 0, "structural neutral");
    ASSERT_EQ(k.k_moral, K_DIR_NEUTRAL, "moral neutral");
    ASSERT_FLOAT_EQ(k.score, 0.0f, "score neutral");

    PASS();
}

void test_kalign_from_position(void) {
    TEST(kalign_from_position);

    // X > Y → expanding
    k_align_t k = k_align_from_position(1, 0);
    ASSERT_EQ(k.k_structural, 1, "X>Y → k=+1");

    // X < Y → contracting
    k = k_align_from_position(0, 1);
    ASSERT_EQ(k.k_structural, -1, "X<Y → k=-1");

    // X == Y → neutral
    k = k_align_from_position(0, 0);
    ASSERT_EQ(k.k_structural, 0, "X==Y → k=0");

    PASS();
}

void test_kalign_apply_choice(void) {
    TEST(kalign_apply_choice);

    k_align_t k = k_align_neutral();

    // Apply positive choice
    k = k_align_apply_choice(k, K_DIR_TOWARD_GOD, 1.0f);
    ASSERT_EQ(k.k_moral, K_DIR_TOWARD_GOD, "moral updated");
    ASSERT_TRUE(k.score > 0.0f, "score increased");

    // Apply negative choice
    k = k_align_apply_choice(k, K_DIR_TOWARD_SELF, 0.5f);
    ASSERT_EQ(k.k_moral, K_DIR_TOWARD_SELF, "moral updated");

    PASS();
}

void test_kalign_effective(void) {
    TEST(kalign_effective);

    // Moral takes precedence
    k_align_t k = k_align_create(1, K_DIR_TOWARD_SELF);
    ASSERT_EQ(k_align_get_effective(k), -1, "moral overrides structural");

    // Structural used when moral neutral
    k = k_align_create(1, K_DIR_NEUTRAL);
    ASSERT_EQ(k_align_get_effective(k), 1, "structural when moral neutral");

    PASS();
}

// ════════════════════════════════════════════════════════════════
// Trajectory Tests
// ════════════════════════════════════════════════════════════════

void test_trajectory_create(void) {
    TEST(trajectory_create);

    trajectory_t traj = trajectory_building();
    ASSERT_EQ(traj.current, FW_BUILDING, "building position");
    ASSERT_TRUE(trajectory_is_building(traj), "is_building");

    traj = trajectory_grounding();
    ASSERT_EQ(traj.current, FW_GROUNDING, "grounding position");
    ASSERT_TRUE(trajectory_is_grounding(traj), "is_grounding");

    PASS();
}

void test_trajectory_cycle(void) {
    TEST(trajectory_cycle);

    // Full cycle: B.1 → B.2 → B.4 → B.3 → B.1
    trajectory_t traj = trajectory_building();  // B.1

    traj = trajectory_advance(traj);  // → B.2
    ASSERT_EQ(traj.current, FW_PIVOTING_OUT, "B.1→B.2");

    traj = trajectory_advance(traj);  // → B.4
    ASSERT_EQ(traj.current, FW_GROUNDING, "B.2→B.4");

    traj = trajectory_advance(traj);  // → B.3
    ASSERT_EQ(traj.current, FW_PIVOTING_IN, "B.4→B.3");

    traj = trajectory_advance(traj);  // → B.1
    ASSERT_EQ(traj.current, FW_BUILDING, "B.3→B.1");

    PASS();
}

void test_trajectory_to_anchor(void) {
    TEST(trajectory_to_anchor);

    trajectory_t traj = trajectory_building();
    traj = trajectory_to_anchor(traj);

    ASSERT_EQ(traj.current, FW_GROUNDING, "returned to anchor");
    ASSERT_EQ(traj.pivot_count, 0, "pivots reset");

    PASS();
}

// ════════════════════════════════════════════════════════════════
// Unified State Machine Tests
// ════════════════════════════════════════════════════════════════

void test_cognitive_state_create(void) {
    TEST(cognitive_state_create);

    cognitive_state_t state = cognitive_state_create();

    ASSERT_TRUE(cube_is_center(state.position), "at center");
    ASSERT_EQ(state.hebrew, HEBREW_YASHAR, "hebrew yashar");
    ASSERT_TRUE(trajectory_is_grounding(state.trajectory), "grounding");
    ASSERT_EQ(state.health, 0, "health neutral");

    PASS();
}

void test_cognitive_apply_choice(void) {
    TEST(cognitive_apply_choice);

    cognitive_state_t state = cognitive_state_create();

    // Apply positive choice
    state = cognitive_apply_choice(state, CHOICE_POSITIVE, CHOICE_WEIGHT_NORMAL);

    ASSERT_EQ(state.choice_count, 1, "choice counted");
    ASSERT_EQ(state.k_align.k_moral, K_DIR_TOWARD_GOD, "k_moral updated");

    PASS();
}

void test_cognitive_navigation(void) {
    TEST(cognitive_navigation);

    cognitive_state_t state = cognitive_state_create();

    // Move in X
    state = cognitive_move_x(state, DIR_BUILD_UP);
    ASSERT_EQ(state.position.x, 1, "moved x");

    // Move to center
    state = cognitive_move_to_center(state);
    ASSERT_TRUE(cube_is_center(state.position), "back to center");

    PASS();
}

void test_cognitive_serialization(void) {
    TEST(cognitive_serialization);

    // Create a specific state
    cognitive_state_t state = cognitive_state_create();
    state = cognitive_move_x(state, DIR_BUILD_UP);
    state = cognitive_apply_choice(state, CHOICE_POSITIVE, 1.0f);
    state.health = 50;

    // Serialize
    uint32_t encoded = cognitive_state_to_int(state);

    // Deserialize
    cognitive_state_t restored = cognitive_state_from_int(encoded);

    // Verify key fields
    ASSERT_TRUE(cube_positions_equal(state.position, restored.position), "position restored");
    ASSERT_EQ(state.hebrew, restored.hebrew, "hebrew restored");
    ASSERT_EQ(state.health, restored.health, "health restored");

    PASS();
}

void test_cognitive_health(void) {
    TEST(cognitive_health);

    cognitive_state_t state = cognitive_state_create();

    state = cognitive_set_health(state, 75);
    ASSERT_EQ(state.health, 75, "health set");

    state = cognitive_adjust_health(state, -100);
    ASSERT_EQ(state.health, -25, "health adjusted");

    // Test clamping
    state = cognitive_set_health(state, 200);
    ASSERT_EQ(state.health, 100, "clamped to max");

    state = cognitive_set_health(state, -200);
    ASSERT_EQ(state.health, -100, "clamped to min");

    PASS();
}

// ════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════

int main(void) {
    printf("\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("CPI-SI State Machine Tests\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    printf("Hebrew State Tests:\n");
    test_hebrew_state_count();
    test_hebrew_state_values();
    test_hebrew_state_info();
    test_hebrew_z_values();
    test_hebrew_from_z_value();
    test_hebrew_navigation();
    test_hebrew_pure_states();

    printf("\nCube Navigation Tests:\n");
    test_cube_center();
    test_cube_index_conversion();
    test_cube_movement();
    test_cube_boundaries();
    test_cube_regions();
    test_cube_distance();

    printf("\nK:ALIGN Tests:\n");
    test_kalign_neutral();
    test_kalign_from_position();
    test_kalign_apply_choice();
    test_kalign_effective();

    printf("\nTrajectory Tests:\n");
    test_trajectory_create();
    test_trajectory_cycle();
    test_trajectory_to_anchor();

    printf("\nUnified State Machine Tests:\n");
    test_cognitive_state_create();
    test_cognitive_apply_choice();
    test_cognitive_navigation();
    test_cognitive_serialization();
    test_cognitive_health();

    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("Results: %d/%d tests passed\n", tests_passed, tests_run);
    printf("════════════════════════════════════════════════════════════════\n\n");

    if (tests_passed == tests_run) {
        printf("✓ All state machine tests passed!\n\n");
        return 0;
    } else {
        printf("✗ Some tests failed.\n\n");
        return 1;
    }
}
