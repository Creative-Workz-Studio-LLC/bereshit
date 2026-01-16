// #!omni code --c -test
// #!omni meta.key = B-word-work-pkg-omni-test-input
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// test_input.c — Interactive Input → Output Tester
// Send input, trace the flow, see the output

#define _GNU_SOURCE

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-test-input
// Grounding: Proverbs 27:17 - "Iron sharpeneth iron"

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
#include <unistd.h>
#include "omni_cpisi.h"
#include "omni_dar.h"
#include "omni_mind.h"
#include "omni_patterns.h"
#include "omni_retrieval.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

#define COLOR_RESET   "\033[0m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_BLUE    "\033[34m"

static char* get_bereshit_root(void) {
    static char root[512];

    // Try BERESHIT_ROOT env var first
    const char* env = getenv("BERESHIT_ROOT");
    if (env) {
        strncpy(root, env, sizeof(root) - 1);
        return root;
    }

    // Get from current working directory
    if (getcwd(root, sizeof(root))) {
        // Navigate up from word/work/pkg/omni to bereshit root
        char* word = strstr(root, "/word/work");
        if (word) {
            *word = '\0';
            return root;
        }
    }

    return "/media/seanje-lenox-wise/Project/Bereshit";
}

static void print_analysis(MindInputAnalysis* a) {
    printf(COLOR_YELLOW "┌─ Analysis ───────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_YELLOW "│" COLOR_RESET " Question: %s  Greeting: %s  Request: %s\n",
           a->is_question ? "✓" : "·",
           a->has_greeting ? "✓" : "·",
           a->has_request ? "✓" : "·");
    printf(COLOR_YELLOW "│" COLOR_RESET " Emotion words: %s  Abstract: %s  Concrete: %s\n",
           a->has_emotion_words ? "✓" : "·",
           a->has_abstract_words ? "✓" : "·",
           a->has_concrete_words ? "✓" : "·");
    printf(COLOR_YELLOW "│" COLOR_RESET " Personal Q: %s  Acknowledgment: %s\n",
           a->has_personal_question ? "✓" : "·",
           a->has_acknowledgment ? "✓" : "·");
    printf(COLOR_YELLOW "│" COLOR_RESET " Weights: emotion=%+d  abstraction=%+d  io=%+d\n",
           a->emotional_weight, a->abstraction_level, a->io_orientation);
    printf(COLOR_YELLOW "└───────────────────────────────────────────────────────────\n" COLOR_RESET);
}

static const char* pattern_type_name(PatternType type) {
    switch (type) {
        case PATTERN_GREETING: return "greeting";
        case PATTERN_QUESTION: return "question";
        case PATTERN_ANSWER: return "answer";
        case PATTERN_SCRIPTURE: return "scripture";
        case PATTERN_DEFINITION: return "definition";
        case PATTERN_EXPLANATION: return "explanation";
        case PATTERN_REFLECTION: return "reflection";
        case PATTERN_REQUEST_ACK: return "acknowledgment";
        default: return "unknown";
    }
}

static void print_cube(CPISICube* cube) {
    CPISIPosition pos = cpisi_position(cube);
    DARHebrewState state = cpisi_hebrew_state(cube);
    const char* hebrew = dar_hebrew_state_name(state);

    printf(COLOR_CYAN "┌─ Cube State ──────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_CYAN "│" COLOR_RESET " Position: (%+d, %+d, %+d)\n", pos.x, pos.y, pos.z);
    printf(COLOR_CYAN "│" COLOR_RESET " Hebrew: %s\n", hebrew ? hebrew : "yashar");
    printf(COLOR_CYAN "│" COLOR_RESET " X: %s\n",
           pos.x > 0 ? "warm (+)" : pos.x < 0 ? "analytical (-)" : "balanced (0)");
    printf(COLOR_CYAN "│" COLOR_RESET " Y: %s\n",
           pos.y > 0 ? "abstract (+)" : pos.y < 0 ? "concrete (-)" : "balanced (0)");
    printf(COLOR_CYAN "│" COLOR_RESET " Z: %s\n",
           pos.z > 0 ? "output (+)" : pos.z < 0 ? "input (-)" : "balanced (0)");
    printf(COLOR_CYAN "└───────────────────────────────────────────────────────────\n" COLOR_RESET);
}

int main(int argc, char** argv) {
    char* bereshit_root = get_bereshit_root();
    char input[1024];

    // Suppress init messages to stderr
    FILE* saved_stderr = stderr;
    stderr = fopen("/dev/null", "w");

    // Initialize
    srand(time(NULL));
    pattern_init_knowledge(bereshit_root);

    stderr = saved_stderr;

    // Create cube
    CPISICube* cube = cpisi_cube_create();
    if (!cube) {
        fprintf(stderr, "Failed to create cube\n");
        return 1;
    }

    printf(COLOR_MAGENTA "\n═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_MAGENTA "  CPI-SI Input → Output Tester\n" COLOR_RESET);
    printf(COLOR_MAGENTA "  \"Let your speech be alway with grace\" — Colossians 4:6\n" COLOR_RESET);
    printf(COLOR_MAGENTA "═══════════════════════════════════════════════════════════════\n\n" COLOR_RESET);

    // Interactive mode or single input
    if (argc > 1) {
        // Single input from command line
        snprintf(input, sizeof(input), "%s", argv[1]);

        printf(COLOR_GREEN "Input: " COLOR_RESET "%s\n\n", input);

        // 1. Analyze
        MindInputAnalysis analysis = mind_analyze_input(input);
        print_analysis(&analysis);

        // 2. Move cube
        printf("\n");
        MindMovement move = mind_suggest_movement(&analysis);
        if (move.dx || move.dy || move.dz) {
            printf(COLOR_BLUE "Movement: dx=%+d dy=%+d dz=%+d (%s)\n\n" COLOR_RESET,
                   move.dx, move.dy, move.dz, move.reason);
        }
        mind_apply_to_cube(cube, &analysis);
        print_cube(cube);

        // 3. Generate response
        printf("\n");
        FilledPattern fp = pattern_respond(cube, input, bereshit_root);

        printf(COLOR_GREEN "┌─ Output ──────────────────────────────────────────────────\n" COLOR_RESET);
        printf(COLOR_GREEN "│" COLOR_RESET " Pattern: %s\n", pattern_type_name(fp.type));
        printf(COLOR_GREEN "│" COLOR_RESET " Response: %s\n", fp.output);
        printf(COLOR_GREEN "└───────────────────────────────────────────────────────────\n" COLOR_RESET);

    } else {
        // Interactive REPL
        printf("Type input and press Enter (Ctrl+D to exit)\n\n");

        while (1) {
            printf(COLOR_MAGENTA "> " COLOR_RESET);
            fflush(stdout);

            if (!fgets(input, sizeof(input), stdin)) {
                break;  // EOF
            }

            // Strip newline
            input[strcspn(input, "\n")] = '\0';

            if (strlen(input) == 0) continue;

            printf("\n");

            // 1. Analyze
            MindInputAnalysis analysis = mind_analyze_input(input);
            print_analysis(&analysis);

            // 2. Move cube
            printf("\n");
            MindMovement move = mind_suggest_movement(&analysis);
            if (move.dx || move.dy || move.dz) {
                printf(COLOR_BLUE "Movement: dx=%+d dy=%+d dz=%+d (%s)\n\n" COLOR_RESET,
                       move.dx, move.dy, move.dz, move.reason);
            }
            mind_apply_to_cube(cube, &analysis);
            print_cube(cube);

            // 3. Generate response
            printf("\n");
            FilledPattern fp = pattern_respond(cube, input, bereshit_root);

            printf(COLOR_GREEN "┌─ Output ──────────────────────────────────────────────────\n" COLOR_RESET);
            printf(COLOR_GREEN "│" COLOR_RESET " Pattern: %s\n", pattern_type_name(fp.type));
            printf(COLOR_GREEN "│" COLOR_RESET " Response: %s\n", fp.output);
            printf(COLOR_GREEN "└───────────────────────────────────────────────────────────\n\n" COLOR_RESET);
        }
    }

    cpisi_cube_destroy(cube);
    pattern_shutdown_knowledge();

    printf("\n");
    return 0;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Let your speech be alway with grace, seasoned with salt" — Colossians 4:6
// Input in, trace the flow, output out.

// =============================================================================
// END CLOSING
// =============================================================================
