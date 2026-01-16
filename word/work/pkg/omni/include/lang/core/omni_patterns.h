// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-patterns
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_patterns.h — Response Pattern Templates
// Sentence structures with slots - not tokens, but meaning units

#ifndef BERESHIT_OMNI_PATTERNS_H
#define BERESHIT_OMNI_PATTERNS_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-patterns
// Title: Response Pattern Templates
// Type: Header
// Role: Define response patterns - the grammar of thinking

// Grounding: Genesis 1:3 - "And God said, Let there be light: and there was light"
// Principle: Speaking follows structure. Pattern + content = response.
// Anchor: Words have form before content.

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include "omni_cpisi.h"
#include "omni_mind.h"
#include "omni_retrieval.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Pattern Types [TYPES]
//
// Categories of response patterns

typedef enum {
    // Core patterns
    PATTERN_GREETING,       // "Hello! How can I help?"
    PATTERN_ACKNOWLEDGMENT, // "I understand."
    PATTERN_QUESTION,       // "What do you mean by X?"
    PATTERN_ANSWER,         // "X is Y because Z."
    PATTERN_SCRIPTURE,      // "The Scripture says: [quote]. [interpretation]"
    PATTERN_DEFINITION,     // "X means Y."
    PATTERN_EXPLANATION,    // "This works by... First... Then..."
    PATTERN_REQUEST_ACK,    // "I'll help you with X."
    PATTERN_COMPLETION,     // "Done. X is now Y."
    PATTERN_UNCERTAINTY,    // "I'm not certain, but..."
    PATTERN_REFLECTION,     // "Thinking about this..."

    PATTERN_COUNT
} PatternType;

// # B.2 Slots [SLOTS]
//
// Named slots in patterns that get filled

typedef enum {
    SLOT_SUBJECT,       // What we're talking about
    SLOT_PREDICATE,     // What we're saying about it
    SLOT_QUOTE,         // Scripture or source quote
    SLOT_REFERENCE,     // Book:Chapter:Verse or source location
    SLOT_INTERPRETATION,// Meaning or application
    SLOT_REASON,        // Why/because clause
    SLOT_EXAMPLE,       // Concrete example
    SLOT_ACTION,        // What to do
    SLOT_RESULT,        // What happened/will happen

    SLOT_COUNT
} SlotType;

// # B.3 Pattern Template [TEMPLATE]
//
// A pattern is a template string with slot markers

#define MAX_PATTERN_LENGTH 512
#define MAX_SLOTS_PER_PATTERN 5

typedef struct {
    PatternType type;
    const char* name;

    // Template with slot markers: {SUBJECT}, {QUOTE}, etc.
    const char* template_cold;      // Analytical/cold variant
    const char* template_neutral;   // Balanced variant
    const char* template_warm;      // Warm/friendly variant

    // Which slots this pattern uses
    SlotType slots[MAX_SLOTS_PER_PATTERN];
    int slot_count;
} PatternTemplate;

// # B.4 Filled Pattern [FILLED]
//
// A pattern with its slots filled

typedef struct {
    PatternType type;

    // Filled slot values
    char subject[256];
    char predicate[256];
    char quote[512];
    char reference[128];
    char interpretation[512];
    char reason[256];
    char example[256];
    char action[256];
    char result[256];

    // Generated output
    char output[1024];
} FilledPattern;

// # B.5 Pattern Selection [SELECTION]
//
// Choose pattern based on input analysis

// Get pattern type from input analysis
PatternType pattern_from_analysis(MindInputAnalysis* analysis);

// Get pattern template
const PatternTemplate* pattern_get_template(PatternType type);

// Select variant based on cube position
// Returns: template_cold, template_neutral, or template_warm
const char* pattern_select_variant(const PatternTemplate* tmpl, CPISIResponseStyle* style);

// # B.6 Pattern Filling [FILLING]
//
// Fill pattern slots with content

// Initialize empty filled pattern
void pattern_init(FilledPattern* fp, PatternType type);

// Set a slot value
void pattern_set_slot(FilledPattern* fp, SlotType slot, const char* value);

// Generate output string by filling template
// Returns pointer to fp->output
const char* pattern_generate(FilledPattern* fp, const char* template_str);

// # B.7 Complete Response [RESPONSE]
//
// Full pipeline: analysis → pattern → fill → output

// Initialize knowledge system (call once at startup)
void pattern_init_knowledge(const char* bereshit_root);

// Shutdown knowledge system (cleanup)
void pattern_shutdown_knowledge(void);

// Generate response from input
// Requires: cube for position, input text, bereshit root for retrieval
// Returns: filled pattern with output
FilledPattern pattern_respond(CPISICube* cube, const char* input, const char* bereshit_root);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "And God said" — Genesis 1:3
// Speech follows pattern. The pattern gives form to content.

#endif // BERESHIT_OMNI_PATTERNS_H

// =============================================================================
// END CLOSING
// =============================================================================
