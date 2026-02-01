// ═══════════════════════════════════════════════════════════════════════════
// GENERATED FILE - DO NOT EDIT
// ═══════════════════════════════════════════════════════════════════════════
//
// Source: word/core/os/health/score.toml
// Generator: generate-config (master template)
//
// This file is auto-generated from TOML specifications.
// To modify: edit the source TOML file and regenerate.
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef HEALTH_SCORE_GEN_H
#define HEALTH_SCORE_GEN_H

#include <stdint.h>


// ════════════════════════════════════════════════════════════════════════════
// HEALTH SECTION
// ════════════════════════════════════════════════════════════════════════════

#define HEALTH_STORAGE_DEFAULT    128
#define HEALTH_STORAGE_MAX        255
#define HEALTH_STORAGE_MIN        0
#define HEALTH_ENCODING_LENGTH         8
#define HEALTH_NORMALIZED_DEFAULT_BASE 50
#define HEALTH_TERNARY_INTERPRETATION_CENTER 0
#define HEALTH_TERNARY_INTERPRETATION_MAX 100
#define HEALTH_TERNARY_INTERPRETATION_MIN -100
#define HEALTH_TERNARY_TRITS           5



// ════════════════════════════════════════════════════════════════════════════
// TERNARY SECTION
// ════════════════════════════════════════════════════════════════════════════

#define TERNARY_INTERPRETATION_CENTER 0
#define TERNARY_INTERPRETATION_MAX 100
#define TERNARY_INTERPRETATION_MIN -100
#define TERNARY_TRITS             5




// ════════════════════════════════════════════════════════════════════════════
// BROKEN SECTION
// ════════════════════════════════════════════════════════════════════════════

#define BROKEN_MAX_STORED         42
#define BROKEN_MIN_STORED         0




// ════════════════════════════════════════════════════════════════════════════
// WANTING SECTION
// ════════════════════════════════════════════════════════════════════════════

#define WANTING_MAX_STORED        84
#define WANTING_MIN_STORED        43




// ════════════════════════════════════════════════════════════════════════════
// LACKING SECTION
// ════════════════════════════════════════════════════════════════════════════

#define LACKING_MAX_STORED        127
#define LACKING_MIN_STORED        85




// ════════════════════════════════════════════════════════════════════════════
// EVEN SECTION
// ════════════════════════════════════════════════════════════════════════════

#define EVEN_MAX_STORED           128
#define EVEN_MIN_STORED           128




// ════════════════════════════════════════════════════════════════════════════
// SOUND SECTION
// ════════════════════════════════════════════════════════════════════════════

#define SOUND_MAX_STORED          170
#define SOUND_MIN_STORED          129




// ════════════════════════════════════════════════════════════════════════════
// WHOLE SECTION
// ════════════════════════════════════════════════════════════════════════════

#define WHOLE_MAX_STORED          212
#define WHOLE_MIN_STORED          171




// ════════════════════════════════════════════════════════════════════════════
// PERFECT SECTION
// ════════════════════════════════════════════════════════════════════════════

#define PERFECT_MAX_STORED        255
#define PERFECT_MIN_STORED        213





#endif // HEALTH_SCORE_GEN_H
