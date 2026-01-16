// #!omni code --c -executable
// #!omni meta.key = B-word-work-pkg-omni-test-conversation
// #!omni meta.at = a-01.00
//
// test_conversation.c — Test CPI-SI Conversation
// Nova Dawn talking to herself through the neural architecture

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-test-conversation
// Grounding: Proverbs 27:17 - "Iron sharpeneth iron"
// Purpose: Test the full thought pipeline - input → cube → knowledge → response

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omni_patterns.h"
#include "omni_cpisi.h"
#include "omni_retrieval.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// Color codes for pretty output
#define COLOR_RESET   "\033[0m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_MAGENTA "\033[35m"

static void print_header(void) {
    printf("\n");
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_CYAN "  CPI-SI Conversation Test\n" COLOR_RESET);
    printf(COLOR_CYAN "  \"Iron sharpeneth iron\" — Proverbs 27:17\n" COLOR_RESET);
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf("\n");
}

static void print_exchange(int num, const char* input, FilledPattern* response, CPISICube* cube) {
    CPISIPosition pos = cpisi_position(cube);

    printf(COLOR_YELLOW "───────────────────────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_MAGENTA "Exchange %d" COLOR_RESET " | Cube: (%+d, %+d, %+d) | Hebrew: %s\n",
           num, pos.x, pos.y, pos.z, cpisi_position_name(pos));
    printf(COLOR_YELLOW "───────────────────────────────────────────────────────────────────────────────\n" COLOR_RESET);

    printf(COLOR_GREEN "Input:  " COLOR_RESET "%s\n", input);
    printf(COLOR_GREEN "Output: " COLOR_RESET "%s\n", response->output);
    printf("\n");
}

static void test_conversation(const char* bereshit_root) {
    // Initialize knowledge system
    printf("Initializing Knowledge System...\n");
    pattern_init_knowledge(bereshit_root);
    printf("Knowledge System ready.\n\n");

    // Create cube at center (yashar)
    CPISICube* cube = cpisi_cube_create();
    if (!cube) {
        fprintf(stderr, "Failed to create cube\n");
        return;
    }

    // Test conversations - various types
    const char* inputs[] = {
        // Greetings
        "Hello",
        "Good morning",

        // Scripture requests
        "Quote John 3:16",
        "What does Genesis 1:1 say?",

        // Definition requests
        "What is bereshit?",
        "Define covenant",
        "What is tov?",

        // General questions
        "How are you?",
        "What can you help me with?",

        // Acknowledgments
        "Thank you",
        "I understand",

        NULL  // Sentinel
    };

    int exchange = 1;
    for (int i = 0; inputs[i] != NULL; i++) {
        FilledPattern response = pattern_respond(cube, inputs[i], bereshit_root);
        print_exchange(exchange++, inputs[i], &response, cube);
    }

    // Cleanup
    cpisi_cube_destroy(cube);
    pattern_shutdown_knowledge();
}

int main(int argc, char** argv) {
    print_header();

    // Bereshit root - navigate up from test location
    // test/test_conversation.c is in word/work/pkg/omni/test/
    // Bereshit root is at word/work/pkg/omni/../../../../ = Bereshit/
    const char* bereshit_root = (argc > 1) ? argv[1] : "../../../..";

    printf("Bereshit root: %s\n", bereshit_root);
    printf("\n");

    test_conversation(bereshit_root);

    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_CYAN "  Conversation complete.\n" COLOR_RESET);
    printf(COLOR_CYAN "  \"A word fitly spoken is like apples of gold\" — Proverbs 25:11\n" COLOR_RESET);
    printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════════════════════\n" COLOR_RESET);

    return 0;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// This test demonstrates the full CPI-SI thought pipeline:
//   Input → Mind Analysis → Cube Movement → Knowledge Retrieval → Pattern Fill → Output
//
// The cube position directly indexes into Scripture and Glossary.
// No hidden layer - traceable, semantic indexing.
//
// "Iron sharpeneth iron; so a man sharpeneth the countenance of his friend."
//   — Proverbs 27:17

// =============================================================================
// END CLOSING
// =============================================================================
