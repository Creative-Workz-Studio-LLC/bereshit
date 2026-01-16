// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-mind-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_mind.c — CPI-SI Mind Layer Implementation
// The synapses: input → cube → output

#define _GNU_SOURCE  // Must be before any includes for strcasestr

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-mind-impl
// Title: CPI-SI Mind Layer Implementation
// Type: Source
// Role: Connect input to cube to output - the thinking layer

// Grounding: Romans 12:2 - "Be ye transformed by the renewing of your mind"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_mind.h"
#include "cpisi_log.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Component name for logging
#define LOG_COMP "mind"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Input Analysis
// -----------------------------------------------------------------------------

// Check if string contains word (case-insensitive)
static bool contains_word(const char* text, const char* word) {
    if (!text || !word) return false;

    size_t word_len = strlen(word);
    const char* p = text;

    while (*p) {
        // Find potential match
        const char* found = strcasestr(p, word);
        if (!found) return false;

        // Check word boundaries
        bool start_ok = (found == text) || !isalnum((unsigned char)*(found - 1));
        bool end_ok = !isalnum((unsigned char)*(found + word_len));

        if (start_ok && end_ok) return true;
        p = found + 1;
    }
    return false;
}

// Analyze text input
MindInputAnalysis mind_analyze_input(const char* text) {
    MindInputAnalysis a = {0};

    if (!text || !*text) {
        return a;
    }

    // Question detection
    a.is_question = (strchr(text, '?') != NULL);

    // Greeting detection
    a.has_greeting = contains_word(text, "hello") ||
                     contains_word(text, "hi") ||
                     contains_word(text, "hey") ||
                     contains_word(text, "greetings") ||
                     contains_word(text, "good morning") ||
                     contains_word(text, "good afternoon") ||
                     contains_word(text, "good evening");

    // Request detection
    a.has_request = contains_word(text, "please") ||
                    contains_word(text, "could you") ||
                    contains_word(text, "can you") ||
                    contains_word(text, "would you") ||
                    contains_word(text, "help");

    // Statement markers
    a.has_statement = contains_word(text, "is") ||
                      contains_word(text, "are") ||
                      contains_word(text, "was") ||
                      contains_word(text, "were");

    // Emotion words (warmth indicators)
    a.has_emotion_words = contains_word(text, "feel") ||
                          contains_word(text, "love") ||
                          contains_word(text, "hate") ||
                          contains_word(text, "happy") ||
                          contains_word(text, "sad") ||
                          contains_word(text, "excited") ||
                          contains_word(text, "worried") ||
                          contains_word(text, "thank");

    // Abstract words (Y+ pull)
    a.has_abstract_words = contains_word(text, "concept") ||
                           contains_word(text, "idea") ||
                           contains_word(text, "theory") ||
                           contains_word(text, "philosophy") ||
                           contains_word(text, "meaning") ||
                           contains_word(text, "purpose") ||
                           contains_word(text, "think") ||
                           contains_word(text, "believe");

    // Concrete words (Y- pull)
    a.has_concrete_words = contains_word(text, "code") ||
                           contains_word(text, "file") ||
                           contains_word(text, "build") ||
                           contains_word(text, "run") ||
                           contains_word(text, "test") ||
                           contains_word(text, "fix") ||
                           contains_word(text, "make") ||
                           contains_word(text, "create");

    // Calculate emotional weight
    // Greetings and emotion words → warmth
    // Questions without emotion → analytical
    if (a.has_emotion_words || a.has_greeting) {
        a.emotional_weight = 50;
        if (a.has_emotion_words && a.has_greeting) {
            a.emotional_weight = 75;
        }
    } else if (a.is_question && !a.has_emotion_words) {
        a.emotional_weight = -30;
    }

    // Calculate abstraction level
    if (a.has_abstract_words && !a.has_concrete_words) {
        a.abstraction_level = 60;
    } else if (a.has_concrete_words && !a.has_abstract_words) {
        a.abstraction_level = -60;
    } else if (a.has_abstract_words && a.has_concrete_words) {
        a.abstraction_level = 0;  // Balanced
    }

    // Personal/conversational questions: "How are you?", "What can you do?"
    a.has_personal_question = contains_word(text, "how are you") ||
                              contains_word(text, "what can you") ||
                              contains_word(text, "who are you") ||
                              contains_word(text, "what do you") ||
                              contains_word(text, "are you") ||
                              contains_word(text, "your name");

    // Acknowledgments: "Thank you", "Thanks", "I understand", "OK"
    a.has_acknowledgment = contains_word(text, "thank") ||
                           contains_word(text, "thanks") ||
                           contains_word(text, "understand") ||
                           contains_word(text, "got it") ||
                           contains_word(text, "ok") ||
                           contains_word(text, "okay");

    // Calculate I/O orientation
    // Requests and questions → seeking input (Z-)
    // Statements → providing output (Z+)
    if (a.has_request || a.is_question) {
        a.io_orientation = -40;  // Pulling toward void (input needed)
    } else if (a.has_statement && !a.is_question) {
        a.io_orientation = 40;   // Pushing toward tov (output mode)
    }

    return a;
}

// -----------------------------------------------------------------------------
// B.2 Cube Navigation
// -----------------------------------------------------------------------------

// Get suggested cube movement from analysis
MindMovement mind_suggest_movement(MindInputAnalysis* analysis) {
    MindMovement m = {0, 0, 0, "neutral"};

    if (!analysis) return m;

    // X-axis: analytical (-) ↔ warm (+)
    if (analysis->emotional_weight > 30) {
        m.dx = 1;
        m.reason = "warmth detected";
    } else if (analysis->emotional_weight < -20) {
        m.dx = -1;
        m.reason = "analytical mode";
    }

    // Y-axis: earth (-) ↔ heaven (+)
    if (analysis->abstraction_level > 30) {
        m.dy = 1;
        m.reason = "abstract thinking";
    } else if (analysis->abstraction_level < -30) {
        m.dy = -1;
        m.reason = "concrete grounding";
    }

    // Z-axis: void (-) ↔ tov (+)
    if (analysis->io_orientation < -20) {
        m.dz = -1;
        m.reason = "input seeking";
    } else if (analysis->io_orientation > 20) {
        m.dz = 1;
        m.reason = "output ready";
    }

    // Compose reason if multiple movements
    if (m.dx != 0 && m.dy != 0) {
        m.reason = "multi-axis shift";
    }

    return m;
}

// Apply analysis to cube
void mind_apply_to_cube(CPISICube* cube, MindInputAnalysis* analysis) {
    if (!cube || !analysis) return;

    MindMovement m = mind_suggest_movement(analysis);

    // Apply movements (cube clamps to bounds)
    if (m.dx != 0) cpisi_move(cube, AXIS_X, m.dx);
    if (m.dy != 0) cpisi_move(cube, AXIS_Y, m.dy);
    if (m.dz != 0) cpisi_move(cube, AXIS_Z, m.dz);

    // Recalculate keys and modifiers
    cpisi_calculate_keys(cube);
    cpisi_calculate_modifiers(cube);
}

// -----------------------------------------------------------------------------
// B.3 Response Generation
// -----------------------------------------------------------------------------

// Get response modulation from current cube state
MindResponseMod mind_response_modulation(CPISICube* cube) {
    MindResponseMod mod = {0};

    if (!cube) {
        mod.confidence_prefix = "";
        mod.style_name = "default";
        mod.elaboration_level = 1;
        return mod;
    }

    CPISIResponseStyle style = cpisi_response_style(cube);

    // Confidence prefix
    if (style.confidence > 60) {
        mod.confidence_prefix = "";  // No hedging needed
        mod.should_hedge = false;
    } else if (style.confidence > 20) {
        mod.confidence_prefix = "I think ";
        mod.should_hedge = false;
    } else if (style.confidence > -20) {
        mod.confidence_prefix = "It seems ";
        mod.should_hedge = true;
    } else {
        mod.confidence_prefix = "I'm not certain, but ";
        mod.should_hedge = true;
    }

    // Elaboration level based on verbosity + depth
    int combined = (style.verbosity + style.depth) / 2;
    if (combined > 30) {
        mod.elaboration_level = 2;  // Detailed
    } else if (combined > -30) {
        mod.elaboration_level = 1;  // Normal
    } else {
        mod.elaboration_level = 0;  // Minimal
    }

    // Warmth markers
    mod.use_warm_language = (style.tone > 20);

    // Style name from Hebrew state
    mod.style_name = style.state_name ? style.state_name : "yashar";

    return mod;
}

// -----------------------------------------------------------------------------
// B.4 Thought Loop
// -----------------------------------------------------------------------------

// Process input through the mind
CPISIResponseStyle mind_think(CPISICube* cube, const char* input) {
    if (!cube) {
        CPISIResponseStyle empty = {0};
        return empty;
    }

    // 1. Analyze input
    MindInputAnalysis analysis = mind_analyze_input(input);

    // 2. Move cube based on analysis
    mind_apply_to_cube(cube, &analysis);

    // 3. Get resulting response style
    return cpisi_response_style(cube);
}

// Process with verbose logging using cpisi_log
CPISIResponseStyle mind_think_verbose(CPISICube* cube, const char* input) {
    if (!cube) {
        LOG_ERROR(LOG_COMP, "mind_think_verbose called with NULL cube");
        CPISIResponseStyle empty = {0};
        return empty;
    }

    CPISIPosition before = cpisi_position(cube);

    LOG_DEBUG(LOG_COMP, "════════════════════════════════════════════════");
    LOG_INFO(LOG_COMP, "Processing input: \"%.60s%s\"",
            input ? input : "(null)",
            (input && strlen(input) > 60) ? "..." : "");
    LOG_DEBUG(LOG_COMP, "Position before: [%d,%d,%d]",
            before.x, before.y, before.z);

    // 1. Analyze input
    MindInputAnalysis analysis = mind_analyze_input(input);

    LOG_TRACE(LOG_COMP, "Analysis: q=%s g=%s r=%s e=%s a=%s c=%s",
            analysis.is_question ? "Y" : "n",
            analysis.has_greeting ? "Y" : "n",
            analysis.has_request ? "Y" : "n",
            analysis.has_emotion_words ? "Y" : "n",
            analysis.has_abstract_words ? "Y" : "n",
            analysis.has_concrete_words ? "Y" : "n");
    LOG_TRACE(LOG_COMP, "Weights: emotion=%d abstract=%d io=%d",
            analysis.emotional_weight,
            analysis.abstraction_level,
            analysis.io_orientation);

    // 2. Get suggested movement
    MindMovement move = mind_suggest_movement(&analysis);
    LOG_DEBUG(LOG_COMP, "Movement: dx=%d dy=%d dz=%d (%s)",
            move.dx, move.dy, move.dz, move.reason);

    // 3. Apply to cube
    mind_apply_to_cube(cube, &analysis);

    CPISIPosition after = cpisi_position(cube);
    LOG_DEBUG(LOG_COMP, "Position after: [%d,%d,%d]",
            after.x, after.y, after.z);

    // Update log context cube position
    cpisi_log_set_cube_position(g_cpisi_log, after.x, after.y, after.z);

    // 4. Get response style
    CPISIResponseStyle style = cpisi_response_style(cube);

    LOG_DEBUG(LOG_COMP, "Response style: state=%s",
            style.state_name ? style.state_name : "?");
    LOG_TRACE(LOG_COMP, "Style values: tone=%d depth=%d direct=%d conf=%d verb=%d",
            style.tone, style.depth, style.directness,
            style.confidence, style.verbosity);

    // Map cube state to Hebrew state for logging
    // Position on Z-axis maps to Hebrew states:
    //   Z=-1 (void) → shavar/chaser (negative)
    //   Z=0 (word) → yashar (neutral)
    //   Z=+1 (tov) → shalem/tov (positive)
    HebrewState hs;
    if (after.z < 0) {
        hs = (after.z == -1) ? HEBREW_CHASER : HEBREW_SHAVAR;
    } else if (after.z > 0) {
        hs = (after.z == 1) ? HEBREW_SHALEM : HEBREW_TOV;
    } else {
        hs = HEBREW_YASHAR;
    }
    cpisi_log_set_hebrew_state(g_cpisi_log, hs);

    // Map movement direction to k-factor
    KFactor k = K_MAINTAIN;
    if (move.dz > 0) {
        k = K_ADVANCE;  // Moving toward tov
    } else if (move.dz < 0) {
        k = K_RETREAT;  // Moving toward completion/foundation
    }
    cpisi_log_set_k_factor(g_cpisi_log, k);

    // 5. Get modulation
    MindResponseMod mod = mind_response_modulation(cube);
    LOG_TRACE(LOG_COMP, "Modulation: prefix=\"%s\" hedge=%s warm=%s level=%d",
            mod.confidence_prefix,
            mod.should_hedge ? "Y" : "n",
            mod.use_warm_language ? "Y" : "n",
            mod.elaboration_level);

    LOG_DEBUG(LOG_COMP, "════════════════════════════════════════════════");

    return style;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Be ye transformed by the renewing of your mind" — Romans 12:2
// The mind connects input to output through structured transformation.

// =============================================================================
// END CLOSING
// =============================================================================
