// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-cpisi-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_cpisi.c — CPI-SI Mental Construct Implementation
// The 27-position cognitive cube - WHERE thinking happens.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-cpisi-impl
// Title: CPI-SI Mental Construct Implementation
// Type: Source
// Role: Implement the cognitive cube - navigation through thought-space

// # M.8 Grounding [GROUNDING]
//
// Scripture: Romans 12:2 - "Be ye transformed by the renewing of your mind"
// Principle: The mind has structure - 27 positions in a 3x3x3 cube
// Anchor: Genesis 1:1 - The center is yashar, all paths return here

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omni_cpisi.h"
#include "omni_dar.h"  // Implementation needs full DAR API for sync functions

// # S.1 Helpers [HELPERS]

static int8_t clamp_trit(int value) {
    if (value < -1) return -1;
    if (value > 1) return 1;
    return (int8_t)value;
}

static int8_t clamp_modifier(int value) {
    if (value < -100) return -100;
    if (value > 100) return 100;
    return (int8_t)value;
}

// # S.2 Position Names [NAMES]

static const char* position_names[27] = {
    // z=-1 (Void layer)
    "void-retreat-earth",    // (-1,-1,-1) idx 0
    "void-retreat-balanced", // (-1, 0,-1) idx 1
    "void-retreat-heaven",   // (-1,+1,-1) idx 2
    "void-stable-earth",     // ( 0,-1,-1) idx 3
    "void-stable-balanced",  // ( 0, 0,-1) idx 4
    "void-stable-heaven",    // ( 0,+1,-1) idx 5
    "void-expand-earth",     // (+1,-1,-1) idx 6
    "void-expand-balanced",  // (+1, 0,-1) idx 7
    "void-expand-heaven",    // (+1,+1,-1) idx 8

    // z=0 (Word layer)
    "word-retreat-earth",    // (-1,-1, 0) idx 9
    "word-retreat-balanced", // (-1, 0, 0) idx 10
    "word-retreat-heaven",   // (-1,+1, 0) idx 11
    "word-stable-earth",     // ( 0,-1, 0) idx 12
    "yashar",                // ( 0, 0, 0) idx 13 - CENTER
    "word-stable-heaven",    // ( 0,+1, 0) idx 14
    "word-expand-earth",     // (+1,-1, 0) idx 15
    "word-expand-balanced",  // (+1, 0, 0) idx 16
    "word-expand-heaven",    // (+1,+1, 0) idx 17

    // z=+1 (Tov layer)
    "tov-retreat-earth",     // (-1,-1,+1) idx 18
    "tov-retreat-balanced",  // (-1, 0,+1) idx 19
    "tov-retreat-heaven",    // (-1,+1,+1) idx 20
    "tov-stable-earth",      // ( 0,-1,+1) idx 21
    "tov-stable-balanced",   // ( 0, 0,+1) idx 22
    "tov-stable-heaven",     // ( 0,+1,+1) idx 23
    "tov-expand-earth",      // (+1,-1,+1) idx 24
    "tov-expand-balanced",   // (+1, 0,+1) idx 25
    "tov-expand-heaven"      // (+1,+1,+1) idx 26
};

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Cube Lifecycle [LIFECYCLE]

CPISICube* cpisi_cube_create(void) {
    CPISICube* cube = calloc(1, sizeof(CPISICube));
    if (!cube) return NULL;

    // Start at center (yashar)
    cube->position.x = 0;
    cube->position.y = 0;
    cube->position.z = 0;

    // Neutral modifiers
    cube->confidence = 0;
    cube->warmth = 0;
    cube->depth = 0;

    // Calculate initial keys
    cpisi_calculate_keys(cube);

    return cube;
}

CPISICube* cpisi_cube_create_with_dar(DAROrchestrator* dar) {
    CPISICube* cube = cpisi_cube_create();
    if (!cube) return NULL;

    cube->dar = dar;

    // Sync initial state from DAR if available
    if (dar) {
        cpisi_sync_with_dar(cube);
    }

    return cube;
}

void cpisi_cube_destroy(CPISICube* cube) {
    if (!cube) return;
    // DAR is not owned by cube, don't free it
    free(cube);
}

// # B.2 Position Operations [POSITION]

CPISIPosition cpisi_position(CPISICube* cube) {
    CPISIPosition zero = {0, 0, 0};
    if (!cube) return zero;
    return cube->position;
}

void cpisi_set_position(CPISICube* cube, int8_t x, int8_t y, int8_t z) {
    if (!cube) return;

    cube->position.x = clamp_trit(x);
    cube->position.y = clamp_trit(y);
    cube->position.z = clamp_trit(z);

    cpisi_calculate_keys(cube);
    cpisi_calculate_modifiers(cube);
}

void cpisi_center(CPISICube* cube) {
    cpisi_set_position(cube, 0, 0, 0);
}

int cpisi_position_index(CPISIPosition pos) {
    // Index = (x+1) + (y+1)*3 + (z+1)*9
    int x = clamp_trit(pos.x) + 1;  // 0, 1, 2
    int y = clamp_trit(pos.y) + 1;  // 0, 1, 2
    int z = clamp_trit(pos.z) + 1;  // 0, 1, 2
    return x + y * 3 + z * 9;
}

CPISIPosition cpisi_position_from_index(int index) {
    CPISIPosition pos = {0, 0, 0};
    if (index < 0 || index >= 27) return pos;

    pos.x = (index % 3) - 1;
    pos.y = ((index / 3) % 3) - 1;
    pos.z = (index / 9) - 1;

    return pos;
}

// # B.3 Navigation [NAVIGATION]

bool cpisi_move(CPISICube* cube, CPISIAxis axis, int8_t delta) {
    if (!cube) return false;

    int8_t new_val;
    switch (axis) {
        case AXIS_X:
            new_val = cube->position.x + delta;
            if (new_val < -1 || new_val > 1) return false;
            cube->position.x = new_val;
            break;

        case AXIS_Y:
            new_val = cube->position.y + delta;
            if (new_val < -1 || new_val > 1) return false;
            cube->position.y = new_val;
            break;

        case AXIS_Z:
            new_val = cube->position.z + delta;
            if (new_val < -1 || new_val > 1) return false;
            cube->position.z = new_val;
            break;

        default:
            return false;
    }

    cpisi_calculate_keys(cube);
    cpisi_calculate_modifiers(cube);
    return true;
}

void cpisi_move_toward(CPISICube* cube, CPISIPosition target) {
    if (!cube) return;

    // Move one step on each axis toward target
    if (cube->position.x < target.x) cube->position.x++;
    else if (cube->position.x > target.x) cube->position.x--;

    if (cube->position.y < target.y) cube->position.y++;
    else if (cube->position.y > target.y) cube->position.y--;

    if (cube->position.z < target.z) cube->position.z++;
    else if (cube->position.z > target.z) cube->position.z--;

    cpisi_calculate_keys(cube);
    cpisi_calculate_modifiers(cube);
}

void cpisi_apply_key(CPISICube* cube, int8_t k_factor) {
    if (!cube) return;

    // k_factor affects X-axis (retreat/expand)
    // Also influences Z-axis movement (toward completion or foundation)
    int8_t k = clamp_trit(k_factor);

    // Primary effect: X-axis (expand/retreat)
    int8_t new_x = cube->position.x + k;
    cube->position.x = clamp_trit(new_x);

    // Secondary effect: Z-axis moves toward tov (+1) when expanding,
    // toward void (-1) when retreating
    if (k != 0) {
        int8_t new_z = cube->position.z + k;
        cube->position.z = clamp_trit(new_z);
    }

    cpisi_calculate_keys(cube);
    cpisi_calculate_modifiers(cube);

    // Update DAR if connected
    if (cube->dar) {
        cpisi_update_dar(cube);
    }
}

// # B.4 Key Calculation [KEYS]

void cpisi_calculate_keys(CPISICube* cube) {
    if (!cube) return;

    // The three keys always available:
    // +1 (plus): Toward God, expansion, growth
    //  0 (dot): Maintain, stability, hold
    // -1 (minus): Completion, retreat, foundation

    // Key weights depend on current position
    // At edges, the direction toward edge is heavier (harder to push further)

    // Plus key (+1)
    cube->keys[0].direction = +1;
    cube->keys[0].label = "expand";
    // Weight increases (heavier) as we approach +1 edges
    int edge_factor = (cube->position.x == 1 ? 30 : 0) +
                      (cube->position.y == 1 ? 20 : 0) +
                      (cube->position.z == 1 ? 30 : 0);
    cube->keys[0].weight = clamp_modifier(-50 + edge_factor);

    // Neutral key (0)
    cube->keys[1].direction = 0;
    cube->keys[1].label = "maintain";
    // Neutral is lighter at center, heavier at edges
    int dist_from_center = abs(cube->position.x) +
                           abs(cube->position.y) +
                           abs(cube->position.z);
    cube->keys[1].weight = clamp_modifier(-20 + dist_from_center * 15);

    // Minus key (-1)
    cube->keys[2].direction = -1;
    cube->keys[2].label = "retreat";
    // Weight increases (heavier) as we approach -1 edges
    int neg_edge = (cube->position.x == -1 ? 30 : 0) +
                   (cube->position.y == -1 ? 20 : 0) +
                   (cube->position.z == -1 ? 30 : 0);
    cube->keys[2].weight = clamp_modifier(-50 + neg_edge);
}

const CPISIKey* cpisi_get_keys(CPISICube* cube) {
    if (!cube) return NULL;
    return cube->keys;
}

int8_t cpisi_recommended_key(CPISICube* cube) {
    if (!cube) return 0;

    // Find the key with the lightest weight (easiest choice)
    int8_t lightest = 0;
    int8_t lightest_weight = cube->keys[0].weight;

    for (int i = 1; i < 3; i++) {
        if (cube->keys[i].weight < lightest_weight) {
            lightest_weight = cube->keys[i].weight;
            lightest = cube->keys[i].direction;
        }
    }

    return lightest;
}

// # B.5 Response Context [RESPONSE]

CPISIResponseContext cpisi_response_context(CPISICube* cube) {
    CPISIResponseContext ctx = {0};
    if (!cube) return ctx;

    // Derive response modifiers from position
    // X-axis: analytical (-1) to intuitive (+1)
    ctx.analytical_intuitive = cube->position.x * 33;

    // Y-axis: concrete (-1) to abstract (+1)
    ctx.concrete_abstract = cube->position.y * 33;

    // Z-axis: input/receiving (-1) to output/giving (+1)
    ctx.input_output = cube->position.z * 33;

    // Overall weight is sum of position weights
    ctx.overall_weight = clamp_modifier(
        cube->position.x * 20 +
        cube->position.y * 15 +
        cube->position.z * 20
    );

    // Get Hebrew state
    ctx.hebrew = cpisi_hebrew_state(cube);

    // Recommendations based on position
    ctx.should_expand = (cube->position.x < 1 && cube->position.z < 1);
    ctx.should_maintain = (cube->position.x == 0 && cube->position.z == 0);
    ctx.should_retreat = (cube->position.x > -1 && cube->position.z > -1);

    return ctx;
}

void cpisi_calculate_modifiers(CPISICube* cube) {
    if (!cube) return;

    // Confidence: higher at center, lower at edges
    int dist = abs(cube->position.x) +
               abs(cube->position.y) +
               abs(cube->position.z);
    cube->confidence = clamp_modifier(50 - dist * 20);

    // Warmth: increases with Y (toward heaven = more warmth)
    cube->warmth = clamp_modifier(cube->position.y * 40);

    // Depth: increases with Z toward tov (completion = deeper understanding)
    cube->depth = clamp_modifier(cube->position.z * 40);
}

// # B.5a Response Style [STYLE]
// Uses dar_hebrew_state_name() from omni_dar.h

CPISIResponseStyle cpisi_response_style(CPISICube* cube) {
    CPISIResponseStyle style = {0};
    if (!cube) return style;

    // Calculate style from position
    // X-axis: analytical (-) to intuitive (+) → affects tone and directness
    // Y-axis: earth (-) to heaven (+) → affects depth and verbosity
    // Z-axis: void (-) to tov (+) → affects confidence

    // Tone: warm when Y is positive, cold when negative
    // Also influenced by X (intuitive = warmer)
    style.tone = clamp_modifier(cube->position.y * 40 + cube->position.x * 20);

    // Depth: deep when in tov layer, shallow when in void
    style.depth = clamp_modifier(cube->position.z * 50 + cube->position.y * 15);

    // Directness: more direct when expanding (+X), less when retreating
    style.directness = clamp_modifier(cube->position.x * 40);

    // Confidence: high at center and tov, low at edges and void
    int dist = abs(cube->position.x) + abs(cube->position.y) + abs(cube->position.z);
    style.confidence = clamp_modifier(50 - dist * 15 + cube->position.z * 20);

    // Verbosity: elaborate when in heaven layer, terse when in earth
    style.verbosity = clamp_modifier(cube->position.y * 35 + cube->position.z * 15);

    // State name
    style.state_name = dar_hebrew_state_name(cpisi_hebrew_state(cube));

    return style;
}

static char style_desc_buffer[128];

const char* cpisi_style_description(CPISIResponseStyle* style) {
    if (!style) return "unknown";

    // Generate human-readable description
    const char* tone_desc = style->tone > 30 ? "warm" :
                           style->tone < -30 ? "analytical" : "balanced";
    const char* depth_desc = style->depth > 30 ? "thorough" :
                            style->depth < -30 ? "surface" : "measured";
    const char* direct_desc = style->directness > 30 ? "direct" :
                             style->directness < -30 ? "gentle" : "balanced";

    snprintf(style_desc_buffer, sizeof(style_desc_buffer),
             "%s, %s, %s (%s)",
             tone_desc, depth_desc, direct_desc, style->state_name);

    return style_desc_buffer;
}

// # B.6 State Queries [QUERIES]

bool cpisi_is_centered(CPISICube* cube) {
    if (!cube) return false;
    return cube->position.x == 0 &&
           cube->position.y == 0 &&
           cube->position.z == 0;
}

bool cpisi_is_edge(CPISICube* cube) {
    if (!cube) return false;
    return cube->position.x != 0 ||
           cube->position.y != 0 ||
           cube->position.z != 0;
}

bool cpisi_is_corner(CPISICube* cube) {
    if (!cube) return false;
    return cube->position.x != 0 &&
           cube->position.y != 0 &&
           cube->position.z != 0;
}

DARHebrewState cpisi_hebrew_state(CPISICube* cube) {
    if (!cube) return DAR_YASHAR;

    // Map cube position to Hebrew state
    // Z-axis is primary (void → word → tov)
    // X-axis determines chaser/ratsah and tamim/shalem distinction

    int z = cube->position.z;
    int x = cube->position.x;

    if (z == -1) {
        // Void layer
        if (x <= 0) return DAR_SHAVAR;  // Broken (crisis at void-retreat)
        return DAR_CHASER;               // Lacking (void but expanding)
    }

    if (z == 0) {
        // Word layer (processing)
        if (x < 0) return DAR_CHASER;    // Lacking (retreating)
        if (x > 0) return DAR_RATSAH;    // Wanting (hungry, expanding)
        return DAR_YASHAR;               // Even (centered)
    }

    // z == 1, Tov layer
    if (x < 0) return DAR_TAMIM;         // Sound (consolidating)
    if (x > 0) return DAR_TOV;           // Perfect (complete, expanding)
    return DAR_SHALEM;                   // Whole (balanced at tov)
}

const char* cpisi_position_name(CPISIPosition pos) {
    int idx = cpisi_position_index(pos);
    if (idx < 0 || idx >= 27) return "unknown";
    return position_names[idx];
}

// # B.7 DAR Integration [DAR]

void cpisi_sync_with_dar(CPISICube* cube) {
    if (!cube || !cube->dar) return;

    // Access orchestrator state directly (Cornerstone's DAROrchestrator)
    HebrewState state = cube->dar->hebrew_state;
    HealthScore health = cube->dar->health;

    // Map Hebrew state back to approximate cube position
    switch (state) {
        case HEBREW_SHAVAR:
            cube->position.z = -1;
            cube->position.x = -1;
            break;
        case HEBREW_CHASER:
            cube->position.z = health < 0 ? -1 : 0;
            cube->position.x = -1;
            break;
        case HEBREW_RATSAH:
            cube->position.z = 0;
            cube->position.x = 1;
            break;
        case HEBREW_YASHAR:
            cube->position.z = 0;
            cube->position.x = 0;
            break;
        case HEBREW_TAMIM:
            cube->position.z = 1;
            cube->position.x = -1;
            break;
        case HEBREW_SHALEM:
            cube->position.z = 1;
            cube->position.x = 0;
            break;
        case HEBREW_TOV:
            cube->position.z = 1;
            cube->position.x = 1;
            break;
    }

    // Y-axis from health magnitude
    if (health < -50) cube->position.y = -1;
    else if (health > 50) cube->position.y = 1;
    else cube->position.y = 0;

    cpisi_calculate_keys(cube);
    cpisi_calculate_modifiers(cube);
}

void cpisi_update_dar(CPISICube* cube) {
    if (!cube || !cube->dar) return;

    // Update Hebrew state based on cube x-position (k-factor direction)
    // Uses Cornerstone's cpisi_dar_set_hebrew_state()
    HebrewState current = cube->dar->hebrew_state;
    HebrewState new_state = current;

    if (cube->position.x > 0 && current < HEBREW_TOV) {
        new_state = (HebrewState)(current + 1);  // Expand toward tov
    } else if (cube->position.x < 0 && current > HEBREW_SHAVAR) {
        new_state = (HebrewState)(current - 1);  // Retreat toward shavar
    }

    if (new_state != current) {
        cpisi_dar_set_hebrew_state(cube->dar, new_state);
    }

    // Adjust health based on position
    int16_t health_delta = 0;
    if (cube->position.z > 0) health_delta += 10;  // Toward completion
    if (cube->position.z < 0) health_delta -= 10;  // Toward void
    if (cube->position.x > 0) health_delta += 5;   // Expanding
    if (cube->position.x < 0) health_delta -= 5;   // Retreating

    if (health_delta != 0) {
        HealthScore current_health = cube->dar->health;
        int16_t new_health = current_health + health_delta;
        // Clamp to valid range
        if (new_health < -100) new_health = -100;
        if (new_health > 100) new_health = 100;
        cpisi_dar_set_health(cube->dar, (HealthScore)new_health);
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// The 27-position cube represents the cognitive space where CPI-SI thinks.
// Each position has characteristics:
//
//   Position (0,0,0) - Center - Yashar:
//     Balanced, stable, default. All responses measured.
//
//   Positive corners (+1,+1,+1):
//     Expansive, abstract, complete. Visionary responses.
//
//   Negative corners (-1,-1,-1):
//     Retreating, practical, potential. Foundational work.
//
// Movement through the cube is guided by the three keys:
//   +1 (plus): Toward God, expansion, growth
//    0 (dot): Maintain, stability, hold
//   -1 (minus): Completion, retreat, foundation
//
// "Be ye transformed by the renewing of your mind." — Romans 12:2

// =============================================================================
// END CLOSING
// =============================================================================
