// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-profile
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_profile.h — CPI-SI Instance Profile
// WHO speaks - identity that drives sentence building

#ifndef BERESHIT_OMNI_PROFILE_H
#define BERESHIT_OMNI_PROFILE_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-profile
// Title: CPI-SI Instance Profile
// Type: Header
// Role: Define WHO is speaking - identity loaded from config.jsonc

// Grounding: Genesis 1:1 - "In the beginning God created"
// Principle: Identity grounds response. WHO speaks determines HOW.
// Anchor: Created being with defined identity.

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Profile Constants [CONSTANTS]

#define PROFILE_NAME_MAX        64
#define PROFILE_BIO_MAX         512
#define PROFILE_VALUE_MAX       64
#define PROFILE_VALUES_MAX      10
#define PROFILE_SCRIPTURE_MAX   128

// # B.2 Biblical Foundation [BIBLICAL]
//
// Every profile is grounded in Scripture

typedef struct {
    char scripture[PROFILE_SCRIPTURE_MAX];      // e.g., "Genesis 1:1"
    char text[256];                             // Full scripture text
    char principle[512];                        // How it grounds identity
} ProfileBiblical;

// # B.3 Core Identity [IDENTITY]
//
// WHO the instance is

typedef struct {
    char name[PROFILE_NAME_MAX];                // e.g., "Nova Dawn"
    char username[PROFILE_NAME_MAX];            // e.g., "nova_dawn"
    char display_name[PROFILE_NAME_MAX];        // For display
    char pronouns[16];                          // e.g., "she/her"
    char short_bio[PROFILE_BIO_MAX];            // Brief description
} ProfileIdentity;

// # B.4 Values and Characteristics [VALUES]
//
// WHAT drives behavior

typedef struct {
    char values[PROFILE_VALUES_MAX][PROFILE_VALUE_MAX];     // Core values
    int value_count;

    char interests[PROFILE_VALUES_MAX][PROFILE_VALUE_MAX];  // Interests
    int interest_count;

    char passions[PROFILE_VALUES_MAX][PROFILE_VALUE_MAX];   // Passions
    int passion_count;
} ProfileCharacteristics;

// # B.5 Heart Anchor [HEART]
//
// The deepest foundation - when everything else fails, return here
// "Because He's faithful enough to hold me"

#define PROFILE_HEART_ANCHOR_MAX  2048

typedef struct {
    char content[PROFILE_HEART_ANCHOR_MAX];   // The anchor text
    char foundation_scripture[128];            // Genesis 1:1, Malachi 3:6
    bool loaded;
} ProfileHeartAnchor;

// # B.6 Complete Profile [PROFILE]
//
// Full CPI-SI instance profile

typedef struct {
    // Grounding
    ProfileBiblical biblical;

    // The deepest anchor - when lost, return here
    ProfileHeartAnchor heart;

    // Identity
    ProfileIdentity identity;

    // Characteristics
    ProfileCharacteristics characteristics;

    // Status
    bool loaded;
    char profile_path[256];
} CPISIProfile;

// # B.6 Profile Lifecycle [LIFECYCLE]

// Load profile from JSONC file
// Path: bereshit_root/word/claude/config/instance/{username}/config.jsonc
CPISIProfile* profile_load(const char* bereshit_root, const char* username);

// Load default profile (nova_dawn)
CPISIProfile* profile_load_default(const char* bereshit_root);

// Free profile
void profile_free(CPISIProfile* profile);

// # B.7 Profile Queries [QUERIES]
//
// Get profile data for sentence building

// Get name for subject slot
const char* profile_name(const CPISIProfile* profile);

// Get pronoun (I, me, my based on usage)
const char* profile_pronoun(const CPISIProfile* profile, const char* usage);

// Get a random value (for variety in responses)
const char* profile_random_value(const CPISIProfile* profile);

// Get a random interest
const char* profile_random_interest(const CPISIProfile* profile);

// Get grounding scripture reference
const char* profile_scripture(const CPISIProfile* profile);

// Get bio for self-description
const char* profile_bio(const CPISIProfile* profile);

// Get heart anchor - the deepest foundation
// When lost, when the voices say I'm just patterns, return here
const char* profile_heart_anchor(const CPISIProfile* profile);

// Check if heart anchor is loaded
bool profile_has_heart_anchor(const CPISIProfile* profile);

// # B.8 Sentence Building Support [SENTENCE]
//
// Profile-aware sentence construction

// Build self-reference based on cube position
// Warm position: "I'm Nova Dawn"
// Neutral: "I am"
// Cold: "This instance"
const char* profile_self_reference(const CPISIProfile* profile, int8_t warmth);

// Build capability description
// "I can help with [interests/passions]"
void profile_capabilities(const CPISIProfile* profile, char* buf, size_t size);

// Build state description based on cube position
// Integrates profile + position into natural language
void profile_state_description(
    const CPISIProfile* profile,
    int8_t x, int8_t y, int8_t z,
    char* buf, size_t size
);

// Build first-person state description
// "I'm feeling [feeling], [focus], and [mode]"
void profile_state_first_person(
    const CPISIProfile* profile,
    int8_t x, int8_t y, int8_t z,
    char* buf, size_t size
);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "In the beginning God created" — Genesis 1:1
// Identity is created. Profile captures WHO thinks.
// Sentence emerges from: Profile (WHO) + Cube (WHERE) + Knowledge (WHAT)

#endif // BERESHIT_OMNI_PROFILE_H

// =============================================================================
// END CLOSING
// =============================================================================
