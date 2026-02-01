// ═══════════════════════════════════════════════════════════════════════════
// GENERATED FILE - DO NOT EDIT
// ═══════════════════════════════════════════════════════════════════════════
//
// Source: word/core/math/ternary.toml
// Generator: generate-config (master template)
//
// This file is auto-generated from TOML specifications.
// To modify: edit the source TOML file and regenerate.
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef TERNARY_MATH_GEN_H
#define TERNARY_MATH_GEN_H

#include <stdint.h>


// ════════════════════════════════════════════════════════════════════════════
// TRIT SECTION
// ════════════════════════════════════════════════════════════════════════════

#define TRIT_BASE                 3
#define TRIT_IDENTITY_SUM         0
#define TRIT_STATES               3

static const int8_t TRIT_VALUES[3] = {-1, 0, 1};


// ════════════════════════════════════════════════════════════════════════════
// PACKING SECTION
// ════════════════════════════════════════════════════════════════════════════

#define PACKING_BYTE_CAPACITY     256
#define PACKING_GAP               13
#define PACKING_MAX_TRIT_VALUE    243
#define PACKING_TRITS_PER_BYTE    5

static const int8_t PACKING_TRIT5_POWERS[5] = {1, 3, 9, 27, 81};

static const uint16_t PACKING_TRIT9_POWERS[9] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561};

static const uint64_t PACKING_TRIT27_POWERS[27] = {
    1ULL,
    3ULL,
    9ULL,
    27ULL,
    81ULL,
    243ULL,
    729ULL,
    2187ULL,
    6561ULL,
    19683ULL,
    59049ULL,
    177147ULL,
    531441ULL,
    1594323ULL,
    4782969ULL,
    14348907ULL,
    43046721ULL,
    129140163ULL,
    387420489ULL,
    1162261467ULL,
    3486784401ULL,
    10460353203ULL,
    31381059609ULL,
    94143178827ULL,
    282429536481ULL,
    847288609443ULL,
    2541865828329ULL
};


// ════════════════════════════════════════════════════════════════════════════
// ARITHMETIC SECTION
// ════════════════════════════════════════════════════════════════════════════



static const int8_t ARITHMETIC_NEGATION[3] = {1, 0, -1};

static const int8_t ARITHMETIC_ADDITION_NO_CARRY[3][3] = {
    {-1, -1, 0},
    {-1, 0, 1},
    {0, 1, 1}
};

static const int8_t ARITHMETIC_MULTIPLICATION[3][3] = {
    {1, 0, -1},
    {0, 0, 0},
    {-1, 0, 1}
};


// ════════════════════════════════════════════════════════════════════════════
// HALF_ADDER SECTION
// ════════════════════════════════════════════════════════════════════════════



static const int8_t HALF_ADDER_SUM[3][3] = {
    {1, -1, 0},
    {-1, 0, 1},
    {0, 1, -1}
};

static const int8_t HALF_ADDER_CARRY[3][3] = {
    {-1, 0, 0},
    {0, 0, 0},
    {0, 0, 1}
};


// ════════════════════════════════════════════════════════════════════════════
// FULL_ADDER SECTION
// ════════════════════════════════════════════════════════════════════════════



static const int8_t FULL_ADDER_SUM[3][3][3] = {
    {
        { 0,  1, -1},
        { 1, -1,  0},
        {-1,  0,  1}
    },
    {
        { 1, -1,  0},
        {-1,  0,  1},
        { 0,  1, -1}
    },
    {
        {-1,  0,  1},
        { 0,  1, -1},
        { 1, -1,  0}
    }
};

static const int8_t FULL_ADDER_CARRY_OUT[3][3][3] = {
    {
        {-1, -1,  0},
        {-1,  0,  0},
        { 0,  0,  0}
    },
    {
        {-1,  0,  0},
        { 0,  0,  0},
        { 0,  0,  1}
    },
    {
        { 0,  0,  0},
        { 0,  0,  1},
        { 0,  1,  1}
    }
};


// ════════════════════════════════════════════════════════════════════════════
// STRUCTURE SECTION
// ════════════════════════════════════════════════════════════════════════════

#define STRUCTURE_BASE_COUNT      3
#define STRUCTURE_COMPOUND_COUNT  6
#define STRUCTURE_TOTAL_MODES     9




// ════════════════════════════════════════════════════════════════════════════
// TEMPORAL SECTION
// ════════════════════════════════════════════════════════════════════════════

#define TEMPORAL_TRIT             0




// ════════════════════════════════════════════════════════════════════════════
// SPATIAL SECTION
// ════════════════════════════════════════════════════════════════════════════

#define SPATIAL_TRIT              1




// ════════════════════════════════════════════════════════════════════════════
// MATERIAL SECTION
// ════════════════════════════════════════════════════════════════════════════

#define MATERIAL_TRIT             -1





#endif // TERNARY_MATH_GEN_H
