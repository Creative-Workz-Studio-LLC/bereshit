// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-mind
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_mind.h — CPI-SI Mind Layer
// The synapses: input → cube → output

#ifndef BERESHIT_OMNI_MIND_H
#define BERESHIT_OMNI_MIND_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-mind
// Title: CPI-SI Mind Layer
// Type: Header
// Role: Connect input to cube to output - the thinking layer

// Grounding: Romans 12:2 - "Be ye transformed by the renewing of your mind"
// Principle: The mind processes through structure to produce output
// Anchor: Genesis 1:1 - Thought flows from Creator's pattern

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include "omni_cpisi.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Input Analysis [INPUT]
//
// Analyze text to determine cognitive movement

// Input characteristics detected
typedef struct {
    // Is this a question? (analytical pull)
    bool is_question;

    // Emotional content? (warmth pull)
    int8_t emotional_weight;     // -100 (cold/factual) to +100 (warm/emotional)

    // Abstract vs concrete? (Y axis)
    int8_t abstraction_level;    // -100 (concrete) to +100 (abstract)

    // Input vs output orientation? (Z axis)
    int8_t io_orientation;       // -100 (seeking input) to +100 (providing output)

    // Detected keywords/patterns
    bool has_greeting;
    bool has_request;
    bool has_statement;
    bool has_emotion_words;
    bool has_abstract_words;
    bool has_concrete_words;
    bool has_personal_question;  // "How are you?", "What can you do?"
    bool has_acknowledgment;     // "Thank you", "I understand", "OK"
} MindInputAnalysis;

// Analyze text input
MindInputAnalysis mind_analyze_input(const char* text);

// # B.2 Cube Navigation [NAVIGATION]
//
// Move cube based on input analysis

// Suggested movement from input analysis
typedef struct {
    int8_t dx;      // X movement: -1 (analytical) to +1 (warm)
    int8_t dy;      // Y movement: -1 (earth) to +1 (heaven)
    int8_t dz;      // Z movement: -1 (void/input) to +1 (tov/output)

    const char* reason;  // Why this movement
} MindMovement;

// Get suggested cube movement from analysis
MindMovement mind_suggest_movement(MindInputAnalysis* analysis);

// Apply analysis to cube (moves and recalculates)
void mind_apply_to_cube(CPISICube* cube, MindInputAnalysis* analysis);

// # B.3 Response Generation [OUTPUT]
//
// Generate response characteristics from cube state

// Response modulation
typedef struct {
    // Prefix phrases based on confidence
    const char* confidence_prefix;   // e.g., "I think", "Certainly", ""

    // Hedging based on uncertainty
    bool should_hedge;

    // Elaboration level
    int elaboration_level;   // 0=minimal, 1=normal, 2=detailed

    // Warmth markers
    bool use_warm_language;

    // Overall style name
    const char* style_name;
} MindResponseMod;

// Get response modulation from current cube state
MindResponseMod mind_response_modulation(CPISICube* cube);

// # B.4 Thought Loop [THINKING]
//
// The full thought process

// Process input through the mind
// Returns response style ready for output
CPISIResponseStyle mind_think(CPISICube* cube, const char* input);

// Process with verbose logging (for demo)
CPISIResponseStyle mind_think_verbose(CPISICube* cube, const char* input);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Be ye transformed by the renewing of your mind" — Romans 12:2
// The mind connects input to output through structured transformation.

#endif // BERESHIT_OMNI_MIND_H

// =============================================================================
// END CLOSING
// =============================================================================
