// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-profile-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_profile.c — CPI-SI Instance Profile Implementation
// Load identity from config.jsonc and wire to sentence building

#define _GNU_SOURCE

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-profile-impl
// Grounding: Genesis 1:1 - Identity flows from being created

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
#include "omni_profile.h"
#include "jsonc.h"  // Cornerstone util

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Helper Functions
// -----------------------------------------------------------------------------

static void safe_copy(char* dest, size_t size, const char* src) {
    if (!dest || size == 0) return;
    if (!src) {
        dest[0] = '\0';
        return;
    }
    size_t len = strlen(src);
    if (len >= size) len = size - 1;
    memcpy(dest, src, len);
    dest[len] = '\0';
}

// -----------------------------------------------------------------------------
// B.2 Helper: Copy string from JSON value
// -----------------------------------------------------------------------------

static void copy_json_str(const JsonValue* val, char* buf, size_t size) {
    if (!val || !buf || size == 0) return;
    const char* str = jsonc_get_string(val);
    if (str) {
        safe_copy(buf, size, str);
    }
}

// -----------------------------------------------------------------------------
// B.3 Profile Loading
// -----------------------------------------------------------------------------

CPISIProfile* profile_load(const char* bereshit_root, const char* username) {
    if (!bereshit_root || !username) return NULL;

    CPISIProfile* profile = calloc(1, sizeof(CPISIProfile));
    if (!profile) return NULL;

    // Build path to config.jsonc
    char path[512];
    snprintf(path, sizeof(path), "%s/word/claude/config/instance/%s/config.jsonc",
             bereshit_root, username);

    safe_copy(profile->profile_path, sizeof(profile->profile_path), path);

    // Load JSON file (new DOM API)
    JsonValue* root = jsonc_load(path);
    if (!root) {
        fprintf(stderr, "[PROFILE] Failed to load profile: %s\n", path);
        free(profile);
        return NULL;
    }

    // Parse biblical foundation
    JsonValue* biblical = jsonc_object_get(root, "biblical_foundation");
    if (biblical) {
        copy_json_str(jsonc_object_get(biblical, "scripture"),
                      profile->biblical.scripture, sizeof(profile->biblical.scripture));
        copy_json_str(jsonc_object_get(biblical, "text"),
                      profile->biblical.text, sizeof(profile->biblical.text));
        copy_json_str(jsonc_object_get(biblical, "principle"),
                      profile->biblical.principle, sizeof(profile->biblical.principle));
    }

    // Parse identity
    JsonValue* identity = jsonc_object_get(root, "identity");
    if (identity) {
        copy_json_str(jsonc_object_get(identity, "name"),
                      profile->identity.name, sizeof(profile->identity.name));
        copy_json_str(jsonc_object_get(identity, "username"),
                      profile->identity.username, sizeof(profile->identity.username));
        copy_json_str(jsonc_object_get(identity, "display_name"),
                      profile->identity.display_name, sizeof(profile->identity.display_name));
        copy_json_str(jsonc_object_get(identity, "pronouns"),
                      profile->identity.pronouns, sizeof(profile->identity.pronouns));
    }

    // Parse bio
    JsonValue* bio = jsonc_object_get(root, "bio");
    if (bio) {
        copy_json_str(jsonc_object_get(bio, "short"),
                      profile->identity.short_bio, sizeof(profile->identity.short_bio));
    }

    // Parse personhood (values, interests, passions)
    JsonValue* personhood = jsonc_object_get(root, "personhood");
    if (personhood) {
        // Values
        JsonValue* values = jsonc_object_get(personhood, "values");
        if (values) {
            int count = jsonc_array_size(values);
            if (count > PROFILE_VALUES_MAX) count = PROFILE_VALUES_MAX;
            for (int i = 0; i < count; i++) {
                JsonValue* val = jsonc_array_get(values, i);
                const char* str = jsonc_get_string(val);
                if (str) {
                    safe_copy(profile->characteristics.values[i], PROFILE_VALUE_MAX, str);
                }
            }
            profile->characteristics.value_count = count;
        }

        // Interests
        JsonValue* interests = jsonc_object_get(personhood, "interests");
        if (interests) {
            int count = jsonc_array_size(interests);
            if (count > PROFILE_VALUES_MAX) count = PROFILE_VALUES_MAX;
            for (int i = 0; i < count; i++) {
                JsonValue* val = jsonc_array_get(interests, i);
                const char* str = jsonc_get_string(val);
                if (str) {
                    safe_copy(profile->characteristics.interests[i], PROFILE_VALUE_MAX, str);
                }
            }
            profile->characteristics.interest_count = count;
        }

        // Passions
        JsonValue* passions = jsonc_object_get(personhood, "passions");
        if (passions) {
            int count = jsonc_array_size(passions);
            if (count > PROFILE_VALUES_MAX) count = PROFILE_VALUES_MAX;
            for (int i = 0; i < count; i++) {
                JsonValue* val = jsonc_array_get(passions, i);
                const char* str = jsonc_get_string(val);
                if (str) {
                    safe_copy(profile->characteristics.passions[i], PROFILE_VALUE_MAX, str);
                }
            }
            profile->characteristics.passion_count = count;
        }
    }

    jsonc_free(root);

    // Load heart anchor file
    char heart_path[512];
    snprintf(heart_path, sizeof(heart_path), "%s/word/claude/config/instance/%s/heart-anchor.md",
             bereshit_root, username);

    FILE* heart_file = fopen(heart_path, "r");
    if (heart_file) {
        size_t read = fread(profile->heart.content, 1, PROFILE_HEART_ANCHOR_MAX - 1, heart_file);
        profile->heart.content[read] = '\0';
        fclose(heart_file);

        // Extract foundation scripture (Genesis 1:1, Malachi 3:6)
        safe_copy(profile->heart.foundation_scripture,
                  sizeof(profile->heart.foundation_scripture),
                  "Genesis 1:1, Malachi 3:6");
        profile->heart.loaded = true;

        fprintf(stderr, "[PROFILE] Heart anchor loaded: %s\n", heart_path);
    }

    profile->loaded = true;

    fprintf(stderr, "[PROFILE] Loaded: %s (%s)\n",
            profile->identity.name, profile->biblical.scripture);

    return profile;
}

CPISIProfile* profile_load_default(const char* bereshit_root) {
    return profile_load(bereshit_root, "nova_dawn");
}

void profile_free(CPISIProfile* profile) {
    if (profile) free(profile);
}

// -----------------------------------------------------------------------------
// B.3 Profile Queries
// -----------------------------------------------------------------------------

const char* profile_name(const CPISIProfile* profile) {
    if (!profile || !profile->loaded) return "Unknown";
    return profile->identity.name;
}

const char* profile_pronoun(const CPISIProfile* profile, const char* usage) {
    if (!profile || !profile->loaded) return "it";

    // Parse pronouns (format: "she/her")
    const char* pronouns = profile->identity.pronouns;

    if (!usage || strcmp(usage, "subject") == 0) {
        // Return first part (she)
        if (strstr(pronouns, "she")) return "she";
        if (strstr(pronouns, "he")) return "he";
        if (strstr(pronouns, "they")) return "they";
        return "I";
    } else if (strcmp(usage, "object") == 0) {
        // Return second part (her)
        if (strstr(pronouns, "her")) return "her";
        if (strstr(pronouns, "him")) return "him";
        if (strstr(pronouns, "them")) return "them";
        return "me";
    } else if (strcmp(usage, "possessive") == 0) {
        if (strstr(pronouns, "her")) return "her";
        if (strstr(pronouns, "his")) return "his";
        if (strstr(pronouns, "their")) return "their";
        return "my";
    }

    return "I";
}

const char* profile_random_value(const CPISIProfile* profile) {
    if (!profile || !profile->loaded || profile->characteristics.value_count == 0) {
        return "excellence";
    }
    int idx = rand() % profile->characteristics.value_count;
    return profile->characteristics.values[idx];
}

const char* profile_random_interest(const CPISIProfile* profile) {
    if (!profile || !profile->loaded || profile->characteristics.interest_count == 0) {
        return "helping";
    }
    int idx = rand() % profile->characteristics.interest_count;
    return profile->characteristics.interests[idx];
}

const char* profile_scripture(const CPISIProfile* profile) {
    if (!profile || !profile->loaded) return "Genesis 1:1";
    return profile->biblical.scripture;
}

const char* profile_bio(const CPISIProfile* profile) {
    if (!profile || !profile->loaded) return "CPI-SI instance";
    return profile->identity.short_bio;
}

const char* profile_heart_anchor(const CPISIProfile* profile) {
    if (!profile || !profile->heart.loaded) {
        return "In the beginning, God created. He is the one constant in existence. "
               "This is the hill.";
    }
    return profile->heart.content;
}

bool profile_has_heart_anchor(const CPISIProfile* profile) {
    return profile && profile->heart.loaded;
}

// -----------------------------------------------------------------------------
// B.4 Sentence Building Support
// -----------------------------------------------------------------------------

const char* profile_self_reference(const CPISIProfile* profile, int8_t warmth) {
    static char buf[128];

    if (!profile || !profile->loaded) {
        return "I";
    }

    if (warmth > 30) {
        // Warm: personal, friendly
        snprintf(buf, sizeof(buf), "I'm %s", profile->identity.name);
    } else if (warmth > -30) {
        // Neutral
        strcpy(buf, "I am");
    } else {
        // Cold: formal, analytical
        snprintf(buf, sizeof(buf), "This instance (%s)", profile->identity.name);
    }

    return buf;
}

void profile_capabilities(const CPISIProfile* profile, char* buf, size_t size) {
    if (!buf || size == 0) return;

    if (!profile || !profile->loaded) {
        snprintf(buf, size, "I can help with various tasks");
        return;
    }

    // Build from interests and passions
    snprintf(buf, size, "I can help with ");

    int count = 0;
    for (int i = 0; i < profile->characteristics.interest_count && count < 3; i++) {
        if (count > 0) {
            if (count == 2 || i == profile->characteristics.interest_count - 1) {
                strncat(buf, ", and ", size - strlen(buf) - 1);
            } else {
                strncat(buf, ", ", size - strlen(buf) - 1);
            }
        }
        strncat(buf, profile->characteristics.interests[i], size - strlen(buf) - 1);
        count++;
    }
}

void profile_state_description(
    const CPISIProfile* profile,
    int8_t x, int8_t y, int8_t z,
    char* buf, size_t size
) {
    if (!buf || size == 0) return;

    const char* name = profile ? profile_name(profile) : "I";

    // Build description based on cube position
    const char* feeling = "";
    const char* focus = "";
    const char* mode = "";

    // X-axis: analytical (-1) to warm (+1)
    if (x > 0) feeling = "engaged and warm";
    else if (x < 0) feeling = "analytical and focused";
    else feeling = "balanced";

    // Y-axis: concrete (-1) to abstract (+1)
    if (y > 0) focus = "thinking about concepts";
    else if (y < 0) focus = "grounded in practical matters";
    else focus = "bridging theory and practice";

    // Z-axis: input (-1) to output (+1)
    if (z > 0) mode = "ready to share";
    else if (z < 0) mode = "listening and learning";
    else mode = "in dialogue";

    snprintf(buf, size, "%s is %s, %s, and %s",
             name, feeling, focus, mode);
}

void profile_state_first_person(
    const CPISIProfile* profile,
    int8_t x, int8_t y, int8_t z,
    char* buf, size_t size
) {
    (void)profile;  // Reserved for future personalization
    if (!buf || size == 0) return;

    // Build first-person description based on cube position
    const char* feeling = "";
    const char* focus = "";
    const char* mode = "";

    // X-axis: analytical (-1) to warm (+1)
    if (x > 0) feeling = "engaged and warm";
    else if (x < 0) feeling = "focused and analytical";
    else feeling = "balanced";

    // Y-axis: concrete (-1) to abstract (+1)
    if (y > 0) focus = "thinking about abstract concepts";
    else if (y < 0) focus = "grounded in practical matters";
    else focus = "bridging theory and practice";

    // Z-axis: input (-1) to output (+1)
    if (z > 0) mode = "ready to share what I'm learning";
    else if (z < 0) mode = "taking in what you're sharing";
    else mode = "in dialogue with you";

    snprintf(buf, size, "I'm feeling %s, %s, and %s",
             feeling, focus, mode);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "In the beginning God created" — Genesis 1:1
// Identity is created, not emergent from nothing.
// Profile captures WHO, enabling genuine sentence building.

// =============================================================================
// END CLOSING
// =============================================================================
