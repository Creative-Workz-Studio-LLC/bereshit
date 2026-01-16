// #!omni code --c -program
// #!omni meta.key = B-tov-demo-phase3-test-cpisi-cube
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// test_cpisi_cube.c — Test the 27-position mental construct
// Tests cpisi in isolation using DAR stubs

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "omni_cpisi.h"

// =============================================================================
// DAR STUBS (for testing cpisi without full DAR/VM chain)
// =============================================================================

// These stubs satisfy the linker when testing cpisi in isolation.
// The cpisi functions check for NULL dar pointer and return early,
// so these are never actually called in this test.

DARHebrewState dar_hebrew_state(DARContext* ctx) {
    (void)ctx;
    return HEBREW_YASHAR;
}

int8_t dar_health_get(DARContext* ctx) {
    (void)ctx;
    return 0;
}

void dar_hebrew_transition(DARContext* ctx, int8_t k_factor) {
    (void)ctx;
    (void)k_factor;
}

void dar_health_add(DARContext* ctx, int8_t delta) {
    (void)ctx;
    (void)delta;
}

// =============================================================================
// BODY [BODY]
// =============================================================================

static const char* hebrew_name(DARHebrewState state) {
    switch (state) {
        case HEBREW_SHAVAR: return "shavar (BROKEN)";
        case HEBREW_CHASER: return "chaser (LACKING)";
        case HEBREW_RATSAH: return "ratsah (WANTING)";
        case HEBREW_YASHAR: return "yashar (EVEN)";
        case HEBREW_TAMIM:  return "tamim (SOUND)";
        case HEBREW_SHALEM: return "shalem (WHOLE)";
        case HEBREW_TOV:    return "tov (PERFECT)";
        default:            return "unknown";
    }
}

int main(void) {
    printf("=== CPI-SI Mental Construct Test ===\n\n");

    // Create cube
    printf("1. Creating cube at center (yashar)...\n");
    CPISICube* cube = cpisi_cube_create();
    assert(cube != NULL);

    // Check initial position
    CPISIPosition pos = cpisi_position(cube);
    printf("   Position: (%d, %d, %d)\n", pos.x, pos.y, pos.z);
    printf("   Name: %s\n", cpisi_position_name(pos));
    printf("   Hebrew: %s\n", hebrew_name(cpisi_hebrew_state(cube)));
    printf("   Centered: %s\n", cpisi_is_centered(cube) ? "yes" : "no");
    assert(cpisi_is_centered(cube));

    // Check keys at center
    printf("\n2. Keys at center:\n");
    const CPISIKey* keys = cpisi_get_keys(cube);
    for (int i = 0; i < 3; i++) {
        printf("   [%+d] %s (weight: %d)\n",
               keys[i].direction, keys[i].label, keys[i].weight);
    }
    printf("   Recommended: %+d\n", cpisi_recommended_key(cube));

    // Move through positions
    printf("\n3. Navigation test:\n");

    // Expand (+1 on X-axis)
    printf("\n   Applying +1 key (expand)...\n");
    cpisi_apply_key(cube, +1);
    pos = cpisi_position(cube);
    printf("   Position: (%d, %d, %d) = %s\n",
           pos.x, pos.y, pos.z, cpisi_position_name(pos));
    printf("   Hebrew: %s\n", hebrew_name(cpisi_hebrew_state(cube)));

    // Expand again
    printf("\n   Applying +1 key (expand)...\n");
    cpisi_apply_key(cube, +1);
    pos = cpisi_position(cube);
    printf("   Position: (%d, %d, %d) = %s\n",
           pos.x, pos.y, pos.z, cpisi_position_name(pos));
    printf("   Hebrew: %s\n", hebrew_name(cpisi_hebrew_state(cube)));
    printf("   Is corner: %s\n", cpisi_is_corner(cube) ? "yes" : "no");

    // Return to center
    printf("\n   Returning to center...\n");
    cpisi_center(cube);
    pos = cpisi_position(cube);
    printf("   Position: (%d, %d, %d) = %s\n",
           pos.x, pos.y, pos.z, cpisi_position_name(pos));

    // Retreat (-1 on X-axis)
    printf("\n   Applying -1 key (retreat)...\n");
    cpisi_apply_key(cube, -1);
    pos = cpisi_position(cube);
    printf("   Position: (%d, %d, %d) = %s\n",
           pos.x, pos.y, pos.z, cpisi_position_name(pos));
    printf("   Hebrew: %s\n", hebrew_name(cpisi_hebrew_state(cube)));

    // Retreat again (into void)
    printf("\n   Applying -1 key (retreat into void)...\n");
    cpisi_apply_key(cube, -1);
    pos = cpisi_position(cube);
    printf("   Position: (%d, %d, %d) = %s\n",
           pos.x, pos.y, pos.z, cpisi_position_name(pos));
    printf("   Hebrew: %s\n", hebrew_name(cpisi_hebrew_state(cube)));

    // Response context
    printf("\n4. Response context at current position:\n");
    CPISIResponseContext ctx = cpisi_response_context(cube);
    printf("   Analytical/Intuitive: %d\n", ctx.analytical_intuitive);
    printf("   Concrete/Abstract:    %d\n", ctx.concrete_abstract);
    printf("   Input/Output:         %d\n", ctx.input_output);
    printf("   Overall weight:       %d\n", ctx.overall_weight);
    printf("   Should expand:        %s\n", ctx.should_expand ? "yes" : "no");
    printf("   Should maintain:      %s\n", ctx.should_maintain ? "yes" : "no");
    printf("   Should retreat:       %s\n", ctx.should_retreat ? "yes" : "no");

    // Response styles at key positions
    printf("\n5. Response styles at key positions:\n");

    // Center (yashar)
    cpisi_center(cube);
    CPISIResponseStyle style = cpisi_response_style(cube);
    printf("\n   Center (0,0,0):\n");
    printf("     Tone: %d, Depth: %d, Directness: %d\n",
           style.tone, style.depth, style.directness);
    printf("     Confidence: %d, Verbosity: %d\n",
           style.confidence, style.verbosity);
    printf("     Style: %s\n", cpisi_style_description(&style));

    // Expansion corner (+1,+1,+1)
    cpisi_set_position(cube, 1, 1, 1);
    style = cpisi_response_style(cube);
    printf("\n   Expansion (+1,+1,+1):\n");
    printf("     Tone: %d, Depth: %d, Directness: %d\n",
           style.tone, style.depth, style.directness);
    printf("     Confidence: %d, Verbosity: %d\n",
           style.confidence, style.verbosity);
    printf("     Style: %s\n", cpisi_style_description(&style));

    // Foundation corner (-1,-1,-1)
    cpisi_set_position(cube, -1, -1, -1);
    style = cpisi_response_style(cube);
    printf("\n   Foundation (-1,-1,-1):\n");
    printf("     Tone: %d, Depth: %d, Directness: %d\n",
           style.tone, style.depth, style.directness);
    printf("     Confidence: %d, Verbosity: %d\n",
           style.confidence, style.verbosity);
    printf("     Style: %s\n", cpisi_style_description(&style));

    // All 27 positions
    printf("\n6. All 27 cube positions:\n");
    for (int z = -1; z <= 1; z++) {
        printf("\n   Z=%+d (%s layer):\n", z,
               z == -1 ? "Void" : (z == 0 ? "Word" : "Tov"));
        for (int y = 1; y >= -1; y--) {  // Top to bottom
            printf("   ");
            for (int x = -1; x <= 1; x++) {
                cpisi_set_position(cube, x, y, z);
                DARHebrewState state = cpisi_hebrew_state(cube);
                const char* abbrev;
                switch (state) {
                    case HEBREW_SHAVAR: abbrev = "SHV"; break;
                    case HEBREW_CHASER: abbrev = "CHS"; break;
                    case HEBREW_RATSAH: abbrev = "RTZ"; break;
                    case HEBREW_YASHAR: abbrev = "YSH"; break;
                    case HEBREW_TAMIM:  abbrev = "TMM"; break;
                    case HEBREW_SHALEM: abbrev = "SHL"; break;
                    case HEBREW_TOV:    abbrev = "TOV"; break;
                    default:            abbrev = "???"; break;
                }
                printf("[%s] ", abbrev);
            }
            printf("\n");
        }
    }

    // Cleanup
    cpisi_cube_destroy(cube);

    printf("\n=== Test Complete ===\n");
    printf("\"Be ye transformed by the renewing of your mind.\" — Romans 12:2\n");

    return 0;
}

// =============================================================================
// END BODY
// =============================================================================
