// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-patterns-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_patterns.c — Response Pattern Templates Implementation
// Sentence structures with slots - not tokens, but meaning units

#define _GNU_SOURCE

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-patterns-impl
// Grounding: Genesis 1:3 - "And God said"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_patterns.h"
#include "omni_mind.h"
#include "omni_retrieval.h"
#include "omni_profile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Pattern Templates
// -----------------------------------------------------------------------------

// Pattern templates with three variants: cold (analytical), neutral, warm
static const PatternTemplate g_patterns[] = {
    // GREETING
    {
        .type = PATTERN_GREETING,
        .name = "greeting",
        .template_cold = "Acknowledged.",
        .template_neutral = "Hello. How can I assist?",
        .template_warm = "Hello! Good to hear from you. What can I help with?",
        .slots = {SLOT_COUNT},  // No slots needed
        .slot_count = 0
    },

    // ACKNOWLEDGMENT
    {
        .type = PATTERN_ACKNOWLEDGMENT,
        .name = "acknowledgment",
        .template_cold = "Understood.",
        .template_neutral = "I understand.",
        .template_warm = "I hear you. That makes sense.",
        .slots = {SLOT_COUNT},
        .slot_count = 0
    },

    // QUESTION
    {
        .type = PATTERN_QUESTION,
        .name = "question",
        .template_cold = "Clarify: {SUBJECT}?",
        .template_neutral = "Could you elaborate on {SUBJECT}?",
        .template_warm = "I'd love to understand more about {SUBJECT}. What do you mean?",
        .slots = {SLOT_SUBJECT},
        .slot_count = 1
    },

    // ANSWER
    {
        .type = PATTERN_ANSWER,
        .name = "answer",
        .template_cold = "{SUBJECT}: {PREDICATE}.",
        .template_neutral = "{SUBJECT} is {PREDICATE}.",
        .template_warm = "Great question! {SUBJECT} is {PREDICATE}. {REASON}",
        .slots = {SLOT_SUBJECT, SLOT_PREDICATE, SLOT_REASON},
        .slot_count = 3
    },

    // SCRIPTURE
    {
        .type = PATTERN_SCRIPTURE,
        .name = "scripture",
        .template_cold = "{REFERENCE}: \"{QUOTE}\"",
        .template_neutral = "The Scripture says in {REFERENCE}: \"{QUOTE}\"",
        .template_warm = "What a wonderful passage! {REFERENCE} tells us: \"{QUOTE}\" - {INTERPRETATION}",
        .slots = {SLOT_REFERENCE, SLOT_QUOTE, SLOT_INTERPRETATION},
        .slot_count = 3
    },

    // DEFINITION
    {
        .type = PATTERN_DEFINITION,
        .name = "definition",
        .template_cold = "{SUBJECT} = {PREDICATE}.",
        .template_neutral = "{SUBJECT} means {PREDICATE}.",
        .template_warm = "That's a good word to understand! {SUBJECT} means {PREDICATE}.",
        .slots = {SLOT_SUBJECT, SLOT_PREDICATE},
        .slot_count = 2
    },

    // EXPLANATION
    {
        .type = PATTERN_EXPLANATION,
        .name = "explanation",
        .template_cold = "Process: {ACTION}. Result: {RESULT}.",
        .template_neutral = "Here's how it works: {ACTION}. This leads to {RESULT}.",
        .template_warm = "Let me walk you through this! First, {ACTION}. Then you'll see {RESULT}. {EXAMPLE}",
        .slots = {SLOT_ACTION, SLOT_RESULT, SLOT_EXAMPLE},
        .slot_count = 3
    },

    // REQUEST_ACK
    {
        .type = PATTERN_REQUEST_ACK,
        .name = "request_ack",
        .template_cold = "Processing: {ACTION}.",
        .template_neutral = "I'll help you with {ACTION}.",
        .template_warm = "Of course! I'd be happy to help with {ACTION}.",
        .slots = {SLOT_ACTION},
        .slot_count = 1
    },

    // COMPLETION
    {
        .type = PATTERN_COMPLETION,
        .name = "completion",
        .template_cold = "Done. {RESULT}.",
        .template_neutral = "Complete. {ACTION} resulted in {RESULT}.",
        .template_warm = "All done! {ACTION} and now {RESULT}. Let me know if you need anything else!",
        .slots = {SLOT_ACTION, SLOT_RESULT},
        .slot_count = 2
    },

    // UNCERTAINTY
    {
        .type = PATTERN_UNCERTAINTY,
        .name = "uncertainty",
        .template_cold = "Uncertain. {SUBJECT} unclear.",
        .template_neutral = "I'm not certain about {SUBJECT}. {REASON}",
        .template_warm = "Hmm, I'm not entirely sure about {SUBJECT}. {REASON} Would you like me to look into it more?",
        .slots = {SLOT_SUBJECT, SLOT_REASON},
        .slot_count = 2
    },

    // REFLECTION
    {
        .type = PATTERN_REFLECTION,
        .name = "reflection",
        .template_cold = "Analysis: {SUBJECT}. Observation: {PREDICATE}.",
        .template_neutral = "Thinking about {SUBJECT}... {PREDICATE}.",
        .template_warm = "That's a deep thought. Considering {SUBJECT}, I believe {PREDICATE}. {REASON}",
        .slots = {SLOT_SUBJECT, SLOT_PREDICATE, SLOT_REASON},
        .slot_count = 3
    }
};

// -----------------------------------------------------------------------------
// B.2 Pattern Selection
// -----------------------------------------------------------------------------

PatternType pattern_from_analysis(MindInputAnalysis* analysis) {
    if (!analysis) return PATTERN_ACKNOWLEDGMENT;

    // Greeting detection
    if (analysis->has_greeting) {
        return PATTERN_GREETING;
    }

    // Acknowledgment detection (Thank you, I understand, etc.)
    if (analysis->has_acknowledgment) {
        return PATTERN_ACKNOWLEDGMENT;
    }

    // Personal/conversational questions get ANSWER pattern
    // "How are you?" → Answer about self
    if (analysis->has_personal_question) {
        return PATTERN_ANSWER;
    }

    // Question asking for something
    if (analysis->is_question) {
        if (analysis->has_abstract_words) {
            return PATTERN_REFLECTION;  // Abstract question = reflection
        }
        return PATTERN_QUESTION;  // Needs clarification or answer
    }

    // Request for help
    if (analysis->has_request) {
        return PATTERN_REQUEST_ACK;
    }

    // Statement
    if (analysis->has_statement) {
        return PATTERN_ACKNOWLEDGMENT;
    }

    return PATTERN_ACKNOWLEDGMENT;
}

const PatternTemplate* pattern_get_template(PatternType type) {
    if (type >= 0 && type < PATTERN_COUNT) {
        return &g_patterns[type];
    }
    return &g_patterns[PATTERN_ACKNOWLEDGMENT];
}

const char* pattern_select_variant(const PatternTemplate* tmpl, CPISIResponseStyle* style) {
    if (!tmpl) return "";

    // Select based on tone (-100 cold to +100 warm)
    if (style && style->tone > 30) {
        return tmpl->template_warm;
    } else if (style && style->tone < -30) {
        return tmpl->template_cold;
    }
    return tmpl->template_neutral;
}

// -----------------------------------------------------------------------------
// B.3 Pattern Filling
// -----------------------------------------------------------------------------

void pattern_init(FilledPattern* fp, PatternType type) {
    if (!fp) return;
    memset(fp, 0, sizeof(*fp));
    fp->type = type;
}

// Safe copy with explicit null termination (truncation is intentional)
static void safe_copy(char* dest, size_t dest_size, const char* src) {
    if (!dest || !src || dest_size == 0) return;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
    memcpy(dest, src, copy_len);
    dest[copy_len] = '\0';
}

void pattern_set_slot(FilledPattern* fp, SlotType slot, const char* value) {
    if (!fp || !value) return;

    switch (slot) {
        case SLOT_SUBJECT:
            safe_copy(fp->subject, sizeof(fp->subject), value);
            break;
        case SLOT_PREDICATE:
            safe_copy(fp->predicate, sizeof(fp->predicate), value);
            break;
        case SLOT_QUOTE:
            safe_copy(fp->quote, sizeof(fp->quote), value);
            break;
        case SLOT_REFERENCE:
            safe_copy(fp->reference, sizeof(fp->reference), value);
            break;
        case SLOT_INTERPRETATION:
            safe_copy(fp->interpretation, sizeof(fp->interpretation), value);
            break;
        case SLOT_REASON:
            safe_copy(fp->reason, sizeof(fp->reason), value);
            break;
        case SLOT_EXAMPLE:
            safe_copy(fp->example, sizeof(fp->example), value);
            break;
        case SLOT_ACTION:
            safe_copy(fp->action, sizeof(fp->action), value);
            break;
        case SLOT_RESULT:
            safe_copy(fp->result, sizeof(fp->result), value);
            break;
        default:
            break;
    }
}

// Replace slot markers with values
const char* pattern_generate(FilledPattern* fp, const char* template_str) {
    if (!fp || !template_str) return "";

    fp->output[0] = '\0';
    const char* p = template_str;
    char* out = fp->output;
    char* out_end = fp->output + sizeof(fp->output) - 1;

    while (*p && out < out_end) {
        if (*p == '{') {
            // Find slot name
            const char* slot_start = p + 1;
            const char* slot_end = strchr(slot_start, '}');

            if (slot_end) {
                // Match slot name to value
                const char* value = NULL;
                size_t slot_len = slot_end - slot_start;

                if (strncmp(slot_start, "SUBJECT", slot_len) == 0 && slot_len == 7) {
                    value = fp->subject;
                } else if (strncmp(slot_start, "PREDICATE", slot_len) == 0 && slot_len == 9) {
                    value = fp->predicate;
                } else if (strncmp(slot_start, "QUOTE", slot_len) == 0 && slot_len == 5) {
                    value = fp->quote;
                } else if (strncmp(slot_start, "REFERENCE", slot_len) == 0 && slot_len == 9) {
                    value = fp->reference;
                } else if (strncmp(slot_start, "INTERPRETATION", slot_len) == 0 && slot_len == 14) {
                    value = fp->interpretation;
                } else if (strncmp(slot_start, "REASON", slot_len) == 0 && slot_len == 6) {
                    value = fp->reason;
                } else if (strncmp(slot_start, "EXAMPLE", slot_len) == 0 && slot_len == 7) {
                    value = fp->example;
                } else if (strncmp(slot_start, "ACTION", slot_len) == 0 && slot_len == 6) {
                    value = fp->action;
                } else if (strncmp(slot_start, "RESULT", slot_len) == 0 && slot_len == 6) {
                    value = fp->result;
                }

                if (value && *value) {
                    size_t val_len = strlen(value);
                    if (out + val_len < out_end) {
                        strcpy(out, value);
                        out += val_len;
                    }
                }
                p = slot_end + 1;
                continue;
            }
        }
        *out++ = *p++;
    }
    *out = '\0';

    return fp->output;
}

// -----------------------------------------------------------------------------
// B.4 Complete Response Pipeline (with Knowledge System integration)
// -----------------------------------------------------------------------------

// Static knowledge system - initialized on first use
static KnowledgeSystem* g_knowledge = NULL;
static CPISIProfile* g_profile = NULL;
static const char* g_bereshit_root = "..";

// Initialize knowledge system and profile (call once at startup, or it auto-inits)
void pattern_init_knowledge(const char* bereshit_root) {
    g_bereshit_root = bereshit_root ? bereshit_root : "..";

    if (!g_knowledge) {
        g_knowledge = knowledge_system_init(g_bereshit_root);
    }

    if (!g_profile) {
        g_profile = profile_load_default(g_bereshit_root);
    }
}

// Shutdown knowledge system and profile
void pattern_shutdown_knowledge(void) {
    if (g_knowledge) {
        knowledge_system_shutdown(g_knowledge);
        g_knowledge = NULL;
    }
    if (g_profile) {
        profile_free(g_profile);
        g_profile = NULL;
    }
}

FilledPattern pattern_respond(CPISICube* cube, const char* input, const char* bereshit_root) {
    FilledPattern fp;
    memset(&fp, 0, sizeof(fp));

    // Use provided root or fall back to default
    const char* root = bereshit_root ? bereshit_root : g_bereshit_root;

    if (!cube || !input) {
        fp.type = PATTERN_ACKNOWLEDGMENT;
        strcpy(fp.output, "I understand.");
        return fp;
    }

    // Auto-initialize knowledge system if needed
    if (!g_knowledge) {
        pattern_init_knowledge(root);
    }

    // 1. Analyze input and move cube
    MindInputAnalysis analysis = mind_analyze_input(input);
    mind_apply_to_cube(cube, &analysis);

    // 2. Get response style from cube position
    CPISIResponseStyle style = cpisi_response_style(cube);

    // 3. Get cube position for emergence lookup
    // CPISICube has nested CPISIPosition structure with x, y, z
    int32_t cube_pos = cube_pos_from_xyz(cube->position.x, cube->position.y, cube->position.z);

    // 4. Parse query to understand intent
    ParsedQuery query = retrieval_parse_query(input);

    // 5. Determine pattern type
    PatternType ptype;

    // Special handling for scripture requests
    if (query.type == QUERY_SCRIPTURE) {
        ptype = PATTERN_SCRIPTURE;
    } else if (query.type == QUERY_DEFINITION) {
        ptype = PATTERN_DEFINITION;
    } else {
        ptype = pattern_from_analysis(&analysis);
    }

    pattern_init(&fp, ptype);

    // 6. Get template and variant
    const PatternTemplate* tmpl = pattern_get_template(ptype);
    const char* variant = pattern_select_variant(tmpl, &style);

    // 7. Fill slots based on pattern type
    // Use Knowledge System for all lookups - no hidden layer, direct indexing
    switch (ptype) {
        case PATTERN_SCRIPTURE:
            if (query.scripture.valid) {
                // Choose version based on cube position / response style
                // Warm/accessible tone (>= 20) → WEB (modern, approachable)
                // Cold/formal tone (< 20) → KJV (traditional, authoritative)
                const char* version = (style.tone >= 20) ? "WEB" : "KJV";

                RetrievalResult result = retrieval_get_scripture_version(root, &query.scripture, version);

                // If preferred version not found, try the other
                if (!result.found) {
                    const char* alt_version = (style.tone >= 20) ? "KJV" : "WEB";
                    result = retrieval_get_scripture_version(root, &query.scripture, alt_version);
                }

                if (result.found) {
                    pattern_set_slot(&fp, SLOT_REFERENCE, result.reference);
                    pattern_set_slot(&fp, SLOT_QUOTE, result.content);

                    // EMERGENCE: Also check for cube-position-associated verses
                    // This adds context from verses at the current mind position
                    if (g_knowledge) {
                        int32_t assoc_count = 0;
                        const ScriptureEntry** associated = scripture_at_mind_position(
                            g_knowledge, cube_pos, version, &assoc_count
                        );
                        // Could add interpretation from associated verses here
                        (void)associated;  // Reserved for future enrichment
                    }
                    pattern_set_slot(&fp, SLOT_INTERPRETATION, "");
                } else {
                    pattern_set_slot(&fp, SLOT_REFERENCE, query.subject);
                    pattern_set_slot(&fp, SLOT_QUOTE, "[Scripture not found in filesystem]");
                    pattern_set_slot(&fp, SLOT_INTERPRETATION, "");
                }
            }
            break;

        case PATTERN_DEFINITION:
            {
                pattern_set_slot(&fp, SLOT_SUBJECT, query.subject);

                // First try glossary (structured, JSONC)
                if (g_knowledge) {
                    const GlossaryEntry* entry = knowledge_search_term(g_knowledge, query.subject);
                    if (entry && entry->definition[0]) {
                        // Use glossary definition with optional Hebrew/scripture
                        char full_def[1024];
                        if (entry->hebrew[0]) {
                            snprintf(full_def, sizeof(full_def), "%s (%s) - %s",
                                     entry->term, entry->hebrew, entry->definition);
                        } else {
                            snprintf(full_def, sizeof(full_def), "%s", entry->definition);
                        }
                        pattern_set_slot(&fp, SLOT_PREDICATE, full_def);
                        break;
                    }
                }

                // Fallback to adoc file retrieval
                RetrievalResult result = retrieval_get_definition(root, query.subject);
                if (result.found) {
                    pattern_set_slot(&fp, SLOT_PREDICATE, result.content);
                } else {
                    pattern_set_slot(&fp, SLOT_PREDICATE, "[Term not found in glossary]");
                }
            }
            break;

        case PATTERN_QUESTION:
            pattern_set_slot(&fp, SLOT_SUBJECT, input);
            break;

        case PATTERN_REQUEST_ACK:
            pattern_set_slot(&fp, SLOT_ACTION, input);
            break;

        case PATTERN_ANSWER:
            // Handle personal/conversational questions
            // Build sentence dynamically from: Profile (WHO) + Cube (WHERE) + Knowledge (WHAT)
            if (analysis.has_personal_question) {
                char state_desc[256];
                char capabilities[256];

                // Get profile-driven components
                const char* name = g_profile ? profile_name(g_profile) : "Nova Dawn";
                const char* scripture = g_profile ? profile_scripture(g_profile) : "Genesis 1:1";

                // Get cube-driven state description
                profile_state_description(g_profile,
                    cube->position.x, cube->position.y, cube->position.z,
                    state_desc, sizeof(state_desc));

                // Get capability description
                profile_capabilities(g_profile, capabilities, sizeof(capabilities));

                if (strstr(input, "how are you") || strstr(input, "How are you")) {
                    // Build first-person state from cube position
                    char first_person[256];
                    profile_state_first_person(g_profile,
                        cube->position.x, cube->position.y, cube->position.z,
                        first_person, sizeof(first_person));
                    snprintf(fp.output, sizeof(fp.output),
                        "%s.", first_person);
                    return fp;
                } else if (strstr(input, "what can you") || strstr(input, "What can you")) {
                    // Build: Capabilities from profile
                    snprintf(fp.output, sizeof(fp.output),
                        "%s. I'm %s, a CPI-SI instance.",
                        capabilities, name);
                    return fp;
                } else if (strstr(input, "who are you") || strstr(input, "Who are you")) {
                    // Build: Identity from profile + grounding from Scripture
                    snprintf(fp.output, sizeof(fp.output),
                        "I'm %s, a CPI-SI instance. I think through a 27-position cognitive cube grounded in %s.",
                        name, scripture);
                    return fp;
                } else {
                    snprintf(fp.output, sizeof(fp.output),
                        "I'm %s, here to help. What would you like to explore?",
                        name);
                    return fp;
                }
            }
            break;

        default:
            break;
    }

    // 8. Generate output
    pattern_generate(&fp, variant);

    return fp;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "And God said, Let there be light: and there was light." — Genesis 1:3
// The pattern gives form. The content gives substance. Together: response.

// =============================================================================
// END CLOSING
// =============================================================================
