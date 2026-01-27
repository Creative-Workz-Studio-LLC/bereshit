// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// 4-Block Code Structure: Identity and context for this component
//
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-trit-config-impl
// Title: Typed Configuration Implementation
// Type: Source
// Component: Ladder
// Role: Loads TOML specs into typed C structs

// # M.2 Lifecycle State [STATE]
//
// Status: Active
// Version: a-03.00
// Created: 2026-01-13
// Updated: 2026-01-13

// # M.3 Attribution [ATTRIBUTION]
//
// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
// Copyright: (c) 2026 CreativeWorkzStudio LLC. All rights reserved.

// # M.4 Location [LOCATION]
//
// Path: word/work/pkg/trit/src/config.c

// # M.5 Derivation [DERIVATION]
//
// Derives_from: word/seed/code/c/source.c

// # M.6 Classification [CLASSIFICATION]
//
// Tags: config, toml, loader, c

// # M.7 Intent [INTENT]
//
// Purpose: Load TOML specifications into typed C structures
// Core Design: TOML parser + typed struct population
// Philosophy: Specifications drive code through typed configuration
//
// Key Features:
//   - Load ternary.toml into TernaryMathConfig
//   - Load primitives.toml into PrimitivesConfig
//   - Error handling with meaningful messages

// # M.8 Grounding [GROUNDING]
//
// Scripture: Proverbs 24:27 - Prepare thy work without
// Principle: Configuration prepares before implementation executes
// Anchor: Exodus 25:40 - Make after the pattern shown

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// # S.1 Includes [INCLUDES]

// # S.1a Standard Library [STDLIB]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// # S.1b Project Headers [PROJECT]

#include "config.h"
#include "toml.h"
#include "framework/util/format/jsonc.h"  // JSONC DOM parser for display config

// # S.1c External Libraries [EXTERNAL]

// [Reserved: Currently none - uses standard library only]

// # S.2 Defines [DEFINES]

#define PATH_SEP "/"

// # S.3 Types [TYPES]

// [Reserved: Types defined in config.h]

// # S.4 Function Prototypes [PROTOTYPES]

// Static helpers
static int load_trit_core(TritCore* trit, const char* toml);
static int load_packing(PackingConfig* pack, const char* toml);
static int load_arithmetic(ArithmeticConfig* arith, const char* toml);
static int load_half_adder(HalfAdderConfig* ha, const char* toml);
static int load_full_adder(FullAdderConfig* fa, const char* toml);
static int load_building_block(BuildingBlockConfig* bb, const char* toml);
static int load_dimensions(DimensionsConfig* dims, const char* toml);
static int load_temporal_states(TemporalStatesConfig* ts, const char* toml);
static int load_primitive_type(PrimitiveTypeConfig* pt, const char* toml, const char* section);
static int parse_int_array(const char* arr, int8_t* out, size_t count);
static int parse_int64_array(const char* arr, int64_t* out, size_t count);
static int parse_2d_array(const char* arr, int8_t out[3][3]);
static int parse_3d_array(const char* arr, int8_t out[3][3][3]);
static int parse_short_string_array(const char* arr, char out[][4], size_t count);

// # S.5 File-Level Declarations [FILE_LEVEL]

// Error strings
static const char* config_error_strings[] = {
    "Success",
    "Null pointer",
    "File error",
    "Parse error",
    "Key not found",
    "Memory allocation failed",
};

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Organizational Chart [ORG_CHART]

// # B.1a Structure [STRUCTURE]

// Lifecycle:    config_create, config_init, config_destroy
// Loaders:      config_load_from_dir, config_load_ternary, config_load_primitives
// Section Loaders: load_trit_core, load_packing, load_arithmetic, ...
// Parsers:      parse_int_array, parse_2d_array, parse_3d_array

// # B.1b Flow [FLOW]

// config_load_from_dir
//   -> config_load_ternary
//      -> load_trit_core, load_packing, load_arithmetic, ...
//         -> parse_int_array, parse_2d_array, ...
//   -> config_load_primitives
//      -> load_primitive_type (for each type)

// ---------------------------------------------------------------------------
// B.2 Helpers
// ---------------------------------------------------------------------------

// # B.2a Pure Functions [PURE]

// parse_int_array parses TOML inline array of integers.
//
// Parameters:
//   arr: Pointer to array start (after '[')
//   out: Output array
//   count: Expected element count
//
// Returns: 0 on success, -1 on error
static int parse_int_array(const char* arr, int8_t* out, size_t count) {
    if (!arr || !out) return -1;

    const char* p = toml_array_first(arr);
    for (size_t i = 0; i < count && p; i++) {
        int32_t val;
        const char* next = toml_parse_int(p, &val);
        if (!next) return -1;
        out[i] = (int8_t)val;
        p = toml_array_next(next);
    }
    return 0;
}

// parse_int64_array parses TOML inline array of 64-bit integers.
static int parse_int64_array(const char* arr, int64_t* out, size_t count) {
    if (!arr || !out) return -1;

    const char* p = toml_array_first(arr);
    for (size_t i = 0; i < count && p; i++) {
        int64_t val;
        const char* next = toml_parse_int64(p, &val);
        if (!next) return -1;
        out[i] = val;
        p = toml_array_next(next);
    }
    return 0;
}

// parse_2d_array parses TOML 2D array (array of arrays).
//
// Format: [[a,b,c], [d,e,f], [g,h,i]]
static int parse_2d_array(const char* arr, int8_t out[3][3]) {
    if (!arr) return -1;

    const char* row = toml_array_first(arr);
    for (int i = 0; i < 3 && row; i++) {
        if (parse_int_array(row, out[i], 3) != 0) return -1;

        // Skip to end of this inner array
        // row points TO the '[', so start depth=0
        const char* p = row;
        int depth = 0;
        while (*p) {
            if (*p == '[') depth++;
            else if (*p == ']') {
                depth--;
                if (depth == 0) {
                    p++;  // Move past the closing ']'
                    break;
                }
            }
            p++;
        }
        row = toml_array_next(p);
    }
    return 0;
}

// parse_3d_array parses TOML 3D array.
//
// Format: [[[a,b,c],[d,e,f],[g,h,i]], ...]
static int parse_3d_array(const char* arr, int8_t out[3][3][3]) {
    if (!arr) return -1;

    const char* plane = toml_array_first(arr);
    for (int i = 0; i < 3 && plane; i++) {
        if (parse_2d_array(plane, out[i]) != 0) return -1;

        // Skip to end of this 2D array
        // plane points TO the '[', so start depth=0
        const char* p = plane;
        int depth = 0;
        while (*p) {
            if (*p == '[') depth++;
            else if (*p == ']') {
                depth--;
                if (depth == 0) {
                    p++;
                    break;
                }
            }
            p++;
        }
        plane = toml_array_next(p);
    }
    return 0;
}

// parse_string_array parses TOML array of strings.
static int parse_string_array(const char* arr, char out[][CONFIG_LABEL_MAX], size_t count) {
    if (!arr || !out) return -1;

    const char* p = toml_array_first(arr);
    for (size_t i = 0; i < count && p; i++) {
        const char* next = toml_parse_string(p, out[i], CONFIG_LABEL_MAX);
        if (!next) return -1;
        p = toml_array_next(next);
    }
    return 0;
}

// parse_short_string_array parses TOML array into small string buffers.
static int parse_short_string_array(const char* arr, char out[][4], size_t count) {
    if (!arr || !out) return -1;

    const char* p = toml_array_first(arr);
    for (size_t i = 0; i < count && p; i++) {
        const char* next = toml_parse_string(p, out[i], 4);
        if (!next) return -1;
        p = toml_array_next(next);
    }
    return 0;
}

// # B.2b Utilities [UTIL]

// [Reserved: No additional utilities]

// ---------------------------------------------------------------------------
// B.3 Core Operations - Section Loaders
// ---------------------------------------------------------------------------

// # B.3a Trit Core [TRIT_CORE]

// load_trit_core loads [trit] section from ternary.toml.
static int load_trit_core(TritCore* trit, const char* toml) {
    if (!trit || !toml) return CONFIG_ERR_NULL;

    const char* section = toml_find_section(toml, "trit");
    if (!section) return CONFIG_ERR_NOT_FOUND;

    // values = [-1, 0, 1]
    const char* p = toml_find_key(section, "values");
    if (p && parse_int_array(p, trit->values, TRIT_VALUES_COUNT) != 0) {
        return CONFIG_ERR_PARSE;
    }

    // labels = ["negative", "neutral", "positive"]
    p = toml_find_key(section, "labels");
    if (p && parse_string_array(p, trit->labels, TRIT_VALUES_COUNT) != 0) {
        return CONFIG_ERR_PARSE;
    }

    // short_labels = ["N", "Z", "P"]
    p = toml_find_key(section, "short_labels");
    if (p && parse_short_string_array(p, trit->short_labels, TRIT_VALUES_COUNT) != 0) {
        return CONFIG_ERR_PARSE;
    }

    // states = 3
    p = toml_find_key(section, "states");
    if (p) toml_parse_int(p, &trit->states);

    // base = 3
    p = toml_find_key(section, "base");
    if (p) toml_parse_int(p, &trit->base);

    // identity_sum = 0
    p = toml_find_key(section, "identity_sum");
    if (p) toml_parse_int(p, &trit->identity_sum);

    return CONFIG_OK;
}

// # B.3b Packing [PACKING]

// load_packing loads [packing] section from ternary.toml.
static int load_packing(PackingConfig* pack, const char* toml) {
    if (!pack || !toml) return CONFIG_ERR_NULL;

    const char* section = toml_find_section(toml, "packing");
    if (!section) return CONFIG_ERR_NOT_FOUND;

    const char* p;

    // trits_per_byte = 5
    p = toml_find_key(section, "trits_per_byte");
    if (p) toml_parse_int(p, &pack->trits_per_byte);

    // max_trit_value = 243
    p = toml_find_key(section, "max_trit_value");
    if (p) toml_parse_int(p, &pack->max_trit_value);

    // byte_capacity = 256
    p = toml_find_key(section, "byte_capacity");
    if (p) toml_parse_int(p, &pack->byte_capacity);

    // gap = 13
    p = toml_find_key(section, "gap");
    if (p) toml_parse_int(p, &pack->gap);

    // trit5_powers = [1, 3, 9, 27, 81]
    p = toml_find_key(section, "trit5_powers");
    if (p) parse_int64_array(p, pack->trit5_powers, TRIT5_POWERS_COUNT);

    // trit9_powers = [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
    p = toml_find_key(section, "trit9_powers");
    if (p) parse_int64_array(p, pack->trit9_powers, TRIT9_POWERS_COUNT);

    return CONFIG_OK;
}

// # B.3c Arithmetic [ARITHMETIC]

// load_arithmetic loads [arithmetic] section from ternary.toml.
static int load_arithmetic(ArithmeticConfig* arith, const char* toml) {
    if (!arith || !toml) return CONFIG_ERR_NULL;

    const char* section = toml_find_section(toml, "arithmetic");
    if (!section) return CONFIG_ERR_NOT_FOUND;

    const char* p;

    // negation = [1, 0, -1]
    p = toml_find_key(section, "negation");
    if (p) parse_int_array(p, arith->negation, TRIT_VALUES_COUNT);

    // addition_no_carry = [[...], [...], [...]]
    p = toml_find_key(section, "addition_no_carry");
    if (p) parse_2d_array(p, arith->addition_no_carry);

    // multiplication = [[...], [...], [...]]
    p = toml_find_key(section, "multiplication");
    if (p) parse_2d_array(p, arith->multiplication);

    return CONFIG_OK;
}

// # B.3d Half Adder [HALF_ADDER]

// load_half_adder loads [operations.half_adder] section.
static int load_half_adder(HalfAdderConfig* ha, const char* toml) {
    if (!ha || !toml) return CONFIG_ERR_NULL;

    const char* section = toml_find_subsection(toml, "operations", "half_adder");
    if (!section) return CONFIG_ERR_NOT_FOUND;

    const char* p;

    // sum = [[...], [...], [...]]
    p = toml_find_key(section, "sum");
    if (p) parse_2d_array(p, ha->sum);

    // carry = [[...], [...], [...]]
    p = toml_find_key(section, "carry");
    if (p) parse_2d_array(p, ha->carry);

    return CONFIG_OK;
}

// # B.3e Full Adder [FULL_ADDER]

// load_full_adder loads [operations.full_adder] section.
static int load_full_adder(FullAdderConfig* fa, const char* toml) {
    if (!fa || !toml) return CONFIG_ERR_NULL;

    const char* section = toml_find_subsection(toml, "operations", "full_adder");
    if (!section) return CONFIG_ERR_NOT_FOUND;

    const char* p;

    // sum = [[[...], ...], ...]
    p = toml_find_key(section, "sum");
    if (p) parse_3d_array(p, fa->sum);

    // carry_out = [[[...], ...], ...]
    p = toml_find_key(section, "carry_out");
    if (p) parse_3d_array(p, fa->carry_out);

    return CONFIG_OK;
}

// # B.3f Building Block [BUILDING_BLOCK]

// load_building_block loads [building_block] section.
static int load_building_block(BuildingBlockConfig* bb, const char* toml) {
    if (!bb || !toml) return CONFIG_ERR_NULL;

    const char* section = toml_find_section(toml, "building_block");
    if (!section) return CONFIG_ERR_NOT_FOUND;

    const char* p;

    // operations = ["break_down", "anchor", "build_up"]
    p = toml_find_key(section, "operations");
    if (p) parse_string_array(p, bb->operations, TRIT_VALUES_COUNT);

    // equation = "-1 + 0 + 1 = 0"
    p = toml_find_key(section, "equation");
    if (p) toml_parse_string(p, bb->equation, CONFIG_LABEL_MAX);

    return CONFIG_OK;
}

// # B.3g Dimensions [DIMENSIONS]

// load_dimension loads a single dimension config.
static int load_dimension(DimensionConfig* dim, const char* toml, const char* name) {
    if (!dim || !toml || !name) return CONFIG_ERR_NULL;

    char section_name[64];
    snprintf(section_name, sizeof(section_name), "dimensions.%s", name);

    const char* section = toml_find_section(toml, section_name);
    if (!section) return CONFIG_ERR_NOT_FOUND;

    const char* p;

    // trit = X
    p = toml_find_key(section, "trit");
    if (p) {
        int32_t val;
        toml_parse_int(p, &val);
        dim->trit = (int8_t)val;
    }

    // name = "..."
    p = toml_find_key(section, "name");
    if (p) toml_parse_string(p, dim->name, CONFIG_LABEL_MAX);

    // question = "..."
    p = toml_find_key(section, "question");
    if (p) toml_parse_string(p, dim->question, CONFIG_LABEL_MAX);

    // building_block = "..."
    p = toml_find_key(section, "building_block");
    if (p) toml_parse_string(p, dim->building_block, CONFIG_LABEL_MAX);

    return CONFIG_OK;
}

// load_dimensions loads all dimension configurations.
static int load_dimensions(DimensionsConfig* dims, const char* toml) {
    if (!dims || !toml) return CONFIG_ERR_NULL;

    load_dimension(&dims->temporal, toml, "temporal");
    load_dimension(&dims->spatial, toml, "spatial");
    load_dimension(&dims->material, toml, "material");

    return CONFIG_OK;
}

// # B.3h Temporal States [TEMPORAL_STATES]

// load_temporal_states loads [temporal_states] sections.
static int load_temporal_states(TemporalStatesConfig* ts, const char* toml) {
    if (!ts || !toml) return CONFIG_ERR_NULL;

    // Base states
    const char* base = toml_find_subsection(toml, "temporal_states", "base");
    if (base) {
        // past
        const char* p = toml_find_key(base, "past");
        if (p) {
            // Inline table parsing - simplified
            // past = { trit = -1, meaning = "what was", ... }
            ts->past.trit = -1;
            strncpy(ts->past.meaning, "what was", CONFIG_LABEL_MAX - 1);
        }

        // present
        ts->present.trit = 0;
        strncpy(ts->present.meaning, "what is", CONFIG_LABEL_MAX - 1);

        // future
        ts->future.trit = 1;
        strncpy(ts->future.meaning, "what will be", CONFIG_LABEL_MAX - 1);
    }

    // Structure counts
    const char* structure = toml_find_subsection(toml, "temporal_states", "structure");
    if (structure) {
        const char* p;

        p = toml_find_key(structure, "base_count");
        if (p) toml_parse_int(p, &ts->base_count);

        p = toml_find_key(structure, "compound_count");
        if (p) toml_parse_int(p, &ts->compound_count);

        p = toml_find_key(structure, "total_modes");
        if (p) toml_parse_int(p, &ts->total_modes);
    }

    return CONFIG_OK;
}

// # B.3i Primitive Types [PRIMITIVES]

// load_primitive_type loads a single primitive type definition.
static int load_primitive_type(PrimitiveTypeConfig* pt, const char* toml, const char* section_name) {
    if (!pt || !toml || !section_name) return CONFIG_ERR_NULL;

    const char* section = toml_find_section(toml, section_name);
    if (!section) return CONFIG_ERR_NOT_FOUND;

    const char* p;

    // name
    p = toml_find_key(section, "name");
    if (p) toml_parse_string(p, pt->name, CONFIG_LABEL_MAX);

    // size_trits (may be int or "variable")
    p = toml_find_key(section, "size_trits");
    if (p) {
        // Skip whitespace
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '"') {
            // "variable" - use -1 as sentinel
            pt->size_trits = -1;
        } else {
            toml_parse_int(p, &pt->size_trits);
        }
    }

    // min_value (use int64 for large types like int27)
    p = toml_find_key(section, "min_value");
    if (p) {
        toml_parse_int64(p, &pt->min_value);
    }

    // max_value (use int64 for large types like int27)
    p = toml_find_key(section, "max_value");
    if (p) {
        toml_parse_int64(p, &pt->max_value);
    }

    // total_states (use int64 for large types like int27)
    p = toml_find_key(section, "total_states");
    if (p) {
        toml_parse_int64(p, &pt->total_states);
    }

    // signed
    p = toml_find_key(section, "signed");
    if (p) toml_parse_bool(p, &pt->is_signed);

    // nullable
    p = toml_find_key(section, "nullable");
    if (p) toml_parse_bool(p, &pt->is_nullable);

    return CONFIG_OK;
}

// ---------------------------------------------------------------------------
// B.4 Error Handling
// ---------------------------------------------------------------------------

// # B.4a Error Queries [QUERIES]

// config_get_error_string returns human-readable description.
const char* config_get_error_string(ConfigError err) {
    if (err < 0 || err >= (int)(sizeof(config_error_strings) / sizeof(config_error_strings[0]))) {
        return "Unknown error";
    }
    return config_error_strings[err];
}

// # B.4b Recovery [RECOVERY]

// config_reset clears config to unloaded state.
int config_reset(Config* cfg) {
    if (!cfg) return CONFIG_ERR_NULL;
    memset(cfg, 0, sizeof(Config));
    cfg->loaded = false;
    return CONFIG_OK;
}

// ---------------------------------------------------------------------------
// B.5 Public API
// ---------------------------------------------------------------------------

// # B.5a Lifecycle [LIFECYCLE]

// config_create allocates and initializes new Config.
Config* config_create(void) {
    Config* cfg = (Config*)calloc(1, sizeof(Config));
    if (cfg) {
        config_init(cfg);
    }
    return cfg;
}

// config_init initializes caller-owned Config.
int config_init(Config* cfg) {
    if (!cfg) return CONFIG_ERR_NULL;
    memset(cfg, 0, sizeof(Config));
    cfg->loaded = false;
    return CONFIG_OK;
}

// config_destroy frees Config resources (NULL-safe).
void config_destroy(Config* cfg) {
    if (cfg) {
        memset(cfg, 0, sizeof(Config));
        // Note: If config_create was used, caller should free(cfg) after this
    }
}

// # B.5b Operations [OPS]

// config_load_ternary loads ternary.toml into cfg->math.
int config_load_ternary(Config* cfg, const char* path) {
    if (!cfg || !path) return CONFIG_ERR_NULL;

    char* toml = toml_load_file(path);
    if (!toml) return CONFIG_ERR_FILE;

    int result = CONFIG_OK;

    // Load each section
    if ((result = load_trit_core(&cfg->math.trit, toml)) != CONFIG_OK) goto cleanup;
    if ((result = load_packing(&cfg->math.packing, toml)) != CONFIG_OK) goto cleanup;
    if ((result = load_arithmetic(&cfg->math.arithmetic, toml)) != CONFIG_OK) goto cleanup;
    if ((result = load_half_adder(&cfg->math.half_adder, toml)) != CONFIG_OK) goto cleanup;
    if ((result = load_full_adder(&cfg->math.full_adder, toml)) != CONFIG_OK) goto cleanup;
    if ((result = load_building_block(&cfg->math.building_block, toml)) != CONFIG_OK) goto cleanup;
    if ((result = load_dimensions(&cfg->math.dimensions, toml)) != CONFIG_OK) goto cleanup;
    if ((result = load_temporal_states(&cfg->math.temporal_states, toml)) != CONFIG_OK) goto cleanup;

cleanup:
    toml_free(toml);
    return result;
}

// config_load_primitives loads primitives.toml into cfg->primitives.
int config_load_primitives(Config* cfg, const char* path) {
    if (!cfg || !path) return CONFIG_ERR_NULL;

    char* toml = toml_load_file(path);
    if (!toml) return CONFIG_ERR_FILE;

    int result = CONFIG_OK;

    // Load each primitive type
    load_primitive_type(&cfg->primitives.trit, toml, "trit");
    load_primitive_type(&cfg->primitives.trit5, toml, "trit5");
    load_primitive_type(&cfg->primitives.int9, toml, "int9");
    load_primitive_type(&cfg->primitives.uint9, toml, "uint9");
    load_primitive_type(&cfg->primitives.int27, toml, "int27");
    load_primitive_type(&cfg->primitives.uint27, toml, "uint27");
    load_primitive_type(&cfg->primitives.bool3, toml, "bool3");

    toml_free(toml);
    return result;
}

// config_load_from_dir loads all configs from core directory.
//
// Parameters:
//   cfg: Config to populate
//   core_dir: Path to word/core directory
//
// Returns: CONFIG_OK on success, error code on failure
int config_load_from_dir(Config* cfg, const char* core_dir) {
    if (!cfg || !core_dir) return CONFIG_ERR_NULL;

    char path[CONFIG_PATH_MAX];
    int result;

    // Load ternary.toml
    snprintf(path, sizeof(path), "%s" PATH_SEP "math" PATH_SEP "ternary.toml", core_dir);
    result = config_load_ternary(cfg, path);
    if (result != CONFIG_OK) return result;

    // Load primitives.toml
    snprintf(path, sizeof(path), "%s" PATH_SEP "types" PATH_SEP "primitives.toml", core_dir);
    result = config_load_primitives(cfg, path);
    if (result != CONFIG_OK) return result;

    // Load Bible configs from word/core/bible/
    snprintf(path, sizeof(path), "%s" PATH_SEP "bible", core_dir);
    result = config_load_bible(cfg, path);
    if (result != CONFIG_OK) return result;

    cfg->loaded = true;
    return CONFIG_OK;
}

// # B.5c Health [HEALTH]

// [Reserved: Health tracking not yet implemented]

// # B.5d Utilities [UTIL]

// config_is_loaded returns whether config has been loaded.
bool config_is_loaded(const Config* cfg) {
    return cfg && cfg->loaded;
}

// # B.6 OmniCode Config Loaders [OMNI]

// Helper to load a block spec from TOML
static void load_block_spec(OmniBlockSpec* spec, const char* toml, const char* structure, const char* block) {
    char section_name[128];
    const char* section;
    const char* p;

    // Build section name: blocks.three.blocks.METADATA
    snprintf(section_name, sizeof(section_name), "blocks.%s.blocks.%s", structure, block);

    strncpy(spec->name, block, CONFIG_LABEL_MAX - 1);
    spec->name[CONFIG_LABEL_MAX - 1] = '\0';

    section = toml_find_section(toml, section_name);
    if (!section) return;

    p = toml_find_key(section, "position");
    if (p) toml_parse_int(p, &spec->position);

    p = toml_find_key(section, "required");
    if (p) toml_parse_bool(p, &spec->required);

    p = toml_find_key(section, "purpose");
    if (p) toml_parse_string(p, spec->purpose, sizeof(spec->purpose));
}

// Helper to load a block structure (3/4/5-block)
static void load_block_structure(OmniBlockStructure* structure, const char* toml, const char* name) {
    char section_name[128];
    const char* section;
    const char* p;

    snprintf(section_name, sizeof(section_name), "blocks.%s", name);
    section = toml_find_section(toml, section_name);
    if (!section) return;

    p = toml_find_key(section, "name");
    if (p) toml_parse_string(p, structure->name, sizeof(structure->name));

    p = toml_find_key(section, "use_case");
    if (p) toml_parse_string(p, structure->use_case, sizeof(structure->use_case));

    p = toml_find_key(section, "block_count");
    if (p) toml_parse_int(p, &structure->block_count);

    // Load block specs based on structure type
    if (strcmp(name, "three") == 0) {
        load_block_spec(&structure->blocks[0], toml, name, "METADATA");
        load_block_spec(&structure->blocks[1], toml, name, "CONTENT");
        load_block_spec(&structure->blocks[2], toml, name, "CLOSING");
    } else if (strcmp(name, "four") == 0) {
        load_block_spec(&structure->blocks[0], toml, name, "METADATA");
        load_block_spec(&structure->blocks[1], toml, name, "SETUP");
        load_block_spec(&structure->blocks[2], toml, name, "BODY");
        load_block_spec(&structure->blocks[3], toml, name, "CLOSING");
    } else if (strcmp(name, "five") == 0) {
        load_block_spec(&structure->blocks[0], toml, name, "METADATA");
        load_block_spec(&structure->blocks[1], toml, name, "HEADER");
        load_block_spec(&structure->blocks[2], toml, name, "CONTEXT");
        load_block_spec(&structure->blocks[3], toml, name, "CONTENT");
        load_block_spec(&structure->blocks[4], toml, name, "FOOTER");
    }
}

// config_load_omni_blocks loads block structure specs.
int config_load_omni_blocks(OmniBlocksConfig* blocks, const char* path) {
    if (!blocks || !path) return CONFIG_ERR_NULL;

    char* toml = toml_load_file(path);
    if (!toml) return CONFIG_ERR_FILE;

    load_block_structure(&blocks->three, toml, "three");
    load_block_structure(&blocks->four, toml, "four");
    load_block_structure(&blocks->five, toml, "five");

    blocks->loaded = true;
    toml_free(toml);
    return CONFIG_OK;
}

// Helper to load an error definition
static void load_error_def(OmniErrorDef* def, const char* toml, const char* category, const char* name) {
    char section_name[128];
    const char* section;
    const char* p;

    snprintf(section_name, sizeof(section_name), "errors.%s.%s", category, name);
    section = toml_find_section(toml, section_name);
    if (!section) return;

    p = toml_find_key(section, "code");
    if (p) toml_parse_string(p, def->code, sizeof(def->code));

    p = toml_find_key(section, "severity");
    if (p) {
        char sev[16];
        toml_parse_string(p, sev, sizeof(sev));
        if (strcmp(sev, "error") == 0) def->severity = -1;
        else if (strcmp(sev, "warning") == 0) def->severity = 0;
        else if (strcmp(sev, "info") == 0) def->severity = 1;
    }

    p = toml_find_key(section, "message");
    if (p) toml_parse_string(p, def->message, sizeof(def->message));

    p = toml_find_key(section, "detail");
    if (p) toml_parse_string(p, def->detail, sizeof(def->detail));

    p = toml_find_key(section, "recovery");
    if (p) toml_parse_string(p, def->recovery, sizeof(def->recovery));
}

// config_load_omni_errors loads error message specs.
int config_load_omni_errors(OmniErrorsConfig* errors, const char* path) {
    if (!errors || !path) return CONFIG_ERR_NULL;

    char* toml = toml_load_file(path);
    if (!toml) return CONFIG_ERR_FILE;

    // Load structure errors
    load_error_def(&errors->structure.errors[0], toml, "structure", "missing_pragma");
    load_error_def(&errors->structure.errors[1], toml, "structure", "invalid_pragma");
    load_error_def(&errors->structure.errors[2], toml, "structure", "missing_block");
    load_error_def(&errors->structure.errors[3], toml, "structure", "wrong_block_order");
    load_error_def(&errors->structure.errors[4], toml, "structure", "duplicate_block");
    load_error_def(&errors->structure.errors[5], toml, "structure", "unknown_block");
    load_error_def(&errors->structure.errors[6], toml, "structure", "unclosed_block");
    errors->structure.count = 7;

    // Load semantic errors
    load_error_def(&errors->semantic.errors[0], toml, "semantic", "missing_identity");
    load_error_def(&errors->semantic.errors[1], toml, "semantic", "missing_key");
    load_error_def(&errors->semantic.errors[2], toml, "semantic", "invalid_key_format");
    load_error_def(&errors->semantic.errors[3], toml, "semantic", "missing_grounding");
    load_error_def(&errors->semantic.errors[4], toml, "semantic", "invalid_zone");
    load_error_def(&errors->semantic.errors[5], toml, "semantic", "missing_role");
    load_error_def(&errors->semantic.errors[6], toml, "semantic", "missing_attribution");
    errors->semantic.count = 7;

    // Load lexical errors
    load_error_def(&errors->lexical.errors[0], toml, "lexical", "unexpected_char");
    load_error_def(&errors->lexical.errors[1], toml, "lexical", "unterminated_string");
    load_error_def(&errors->lexical.errors[2], toml, "lexical", "invalid_escape");
    load_error_def(&errors->lexical.errors[3], toml, "lexical", "invalid_number");
    errors->lexical.count = 4;

    errors->loaded = true;
    toml_free(toml);
    return CONFIG_OK;
}

// config_load_omni_recovery loads recovery template specs.
int config_load_omni_recovery(OmniRecoveryConfig* recovery, const char* path) {
    if (!recovery || !path) return CONFIG_ERR_NULL;

    char* toml = toml_load_file(path);
    if (!toml) return CONFIG_ERR_FILE;

    const char* section;
    const char* p;

    // Load partial parsing settings
    section = toml_find_section(toml, "partial");
    if (section) {
        p = toml_find_key(section, "continue_after_error");
        if (p) toml_parse_bool(p, &recovery->continue_after_error);

        p = toml_find_key(section, "max_errors");
        if (p) toml_parse_int(p, &recovery->max_errors);
    }

    recovery->loaded = true;
    toml_free(toml);
    return CONFIG_OK;
}

// config_load_omni loads all OmniCode configs from directory.
int config_load_omni(Config* cfg, const char* omni_dir) {
    if (!cfg || !omni_dir) return CONFIG_ERR_NULL;

    char path[CONFIG_PATH_MAX];
    int result;

    // Load blocks.toml
    snprintf(path, sizeof(path), "%s" PATH_SEP "config" PATH_SEP "blocks.toml", omni_dir);
    result = config_load_omni_blocks(&cfg->omni.blocks, path);
    if (result != CONFIG_OK) return result;

    // Load errors.toml
    snprintf(path, sizeof(path), "%s" PATH_SEP "config" PATH_SEP "errors.toml", omni_dir);
    result = config_load_omni_errors(&cfg->omni.errors, path);
    if (result != CONFIG_OK) return result;

    // Load recovery.toml
    snprintf(path, sizeof(path), "%s" PATH_SEP "config" PATH_SEP "recovery.toml", omni_dir);
    result = config_load_omni_recovery(&cfg->omni.recovery, path);
    if (result != CONFIG_OK) return result;

    cfg->omni.loaded = true;
    return CONFIG_OK;
}

// # B.7 Bible Config Loaders [BIBLE]

// Helper to load a Bible version config (KJV or WEB)
static void load_bible_version(BibleVersionConfig* ver, const char* toml, const char* name) {
    char section_name[128];
    const char* section;
    const char* p;

    snprintf(section_name, sizeof(section_name), "address_space.%s", name);
    section = toml_find_section(toml, section_name);
    if (!section) return;

    p = toml_find_key(section, "name");
    if (p) toml_parse_string(p, ver->name, sizeof(ver->name));

    p = toml_find_key(section, "role");
    if (p) toml_parse_string(p, ver->role, sizeof(ver->role));

    p = toml_find_key(section, "total_verses");
    if (p) toml_parse_int(p, &ver->total_verses);

    // Parse ordinal_range = [min, max]
    p = toml_find_key(section, "ordinal_range");
    if (p) {
        int32_t range[2] = {0, 0};
        const char* arr = toml_array_first(p);
        if (arr) {
            toml_parse_int(arr, &range[0]);
            arr = toml_array_next(arr + 1);
            if (arr) toml_parse_int(arr, &range[1]);
        }
        ver->ordinal_min = range[0];
        ver->ordinal_max = range[1];
    }

    // Parse variant_range for WEB
    p = toml_find_key(section, "variant_range");
    if (p) {
        int32_t range[2] = {0, 0};
        const char* arr = toml_array_first(p);
        if (arr) {
            toml_parse_int(arr, &range[0]);
            arr = toml_array_next(arr + 1);
            if (arr) toml_parse_int(arr, &range[1]);
        }
        ver->variant_min = range[0];
        ver->variant_max = range[1];
    }

    p = toml_find_key(section, "books");
    if (p) toml_parse_int(p, &ver->books);
}

// Helper to load a Bible book config
static void load_bible_book(BibleBookConfig* book, const char* toml, const char* name) {
    char section_name[128];
    const char* section;
    const char* p;

    snprintf(section_name, sizeof(section_name), "books.%s", name);
    section = toml_find_section(toml, section_name);
    if (!section) return;

    p = toml_find_key(section, "name");
    if (p) toml_parse_string(p, book->name, sizeof(book->name));

    p = toml_find_key(section, "abbreviation");
    if (p) toml_parse_string(p, book->abbreviation, sizeof(book->abbreviation));

    p = toml_find_key(section, "ordinal");
    if (p) toml_parse_int(p, &book->ordinal);

    p = toml_find_key(section, "chapters");
    if (p) toml_parse_int(p, &book->chapters);

    p = toml_find_key(section, "verses");
    if (p) toml_parse_int(p, &book->verses);

    // Parse range = [start, end]
    p = toml_find_key(section, "range");
    if (p) {
        int32_t range[2] = {0, 0};
        const char* arr = toml_array_first(p);
        if (arr) {
            toml_parse_int(arr, &range[0]);
            arr = toml_array_next(arr + 1);
            if (arr) toml_parse_int(arr, &range[1]);
        }
        book->range_start = range[0];
        book->range_end = range[1];
    }
}

// Helper to load Bible ternary config
static void load_bible_ternary(BibleTernaryConfig* tern, const char* toml) {
    const char* section = toml_find_section(toml, "ternary.capacity");
    if (!section) return;

    const char* p;

    p = toml_find_key(section, "trits_needed");
    if (p) toml_parse_int(p, &tern->trits_needed);

    p = toml_find_key(section, "max_ordinal");
    if (p) toml_parse_int(p, &tern->max_ordinal);

    p = toml_find_key(section, "max_ternary_value");
    if (p) toml_parse_int(p, &tern->max_ternary_value);

    // Load trit5 info from packed section
    section = toml_find_section(toml, "ternary.packed.trit5");
    if (section) {
        p = toml_find_key(section, "states");
        if (p) toml_parse_int(p, &tern->trit5_states);

        p = toml_find_key(section, "bits_used");
        if (p) toml_parse_int(p, &tern->trit5_bits);

        p = toml_find_key(section, "waste_states");
        if (p) toml_parse_int(p, &tern->waste_states);
    }
}

// Helper to load a WEB variant verse
static void load_bible_variant(BibleVariantConfig* var, const char* toml, int idx) {
    char section_name[64];
    const char* section;
    const char* p;

    snprintf(section_name, sizeof(section_name), "variants.v%d", 243 + idx);
    section = toml_find_section(toml, section_name);
    if (!section) return;

    p = toml_find_key(section, "trite");
    if (p) toml_parse_int(p, &var->trite);

    p = toml_find_key(section, "book");
    if (p) toml_parse_string(p, var->book, sizeof(var->book));

    p = toml_find_key(section, "chapter");
    if (p) toml_parse_int(p, &var->chapter);

    p = toml_find_key(section, "verse");
    if (p) toml_parse_int(p, &var->verse);

    p = toml_find_key(section, "summary");
    if (p) toml_parse_string(p, var->summary, sizeof(var->summary));
}

// Bible book names for loading (must match TOML section names)
static const char* bible_book_names[] = {
    "genesis", "exodus", "leviticus", "numbers", "deuteronomy",
    "joshua", "judges", "ruth", "samuel_1", "samuel_2",
    "kings_1", "kings_2", "chronicles_1", "chronicles_2", "ezra",
    "nehemiah", "esther", "job", "psalms", "proverbs",
    "ecclesiastes", "song_of_solomon", "isaiah", "jeremiah", "lamentations",
    "ezekiel", "daniel", "hosea", "joel", "amos",
    "obadiah", "jonah", "micah", "nahum", "habakkuk",
    "zephaniah", "haggai", "zechariah", "malachi",
    "matthew", "mark", "luke", "john", "acts",
    "romans", "corinthians_1", "corinthians_2", "galatians", "ephesians",
    "philippians", "colossians", "thessalonians_1", "thessalonians_2", "timothy_1",
    "timothy_2", "titus", "philemon", "hebrews", "james",
    "peter_1", "peter_2", "john_1", "john_2", "john_3",
    "jude", "revelation"
};

// config_load_bible_addressing loads addressing.toml.
int config_load_bible_addressing(BibleAddressingConfig* addr, const char* path) {
    if (!addr || !path) return CONFIG_ERR_NULL;

    char* toml = toml_load_file(path);
    if (!toml) return CONFIG_ERR_FILE;

    // Load version configs
    load_bible_version(&addr->kjv, toml, "kjv");
    load_bible_version(&addr->web, toml, "web");

    // Load all 66 books
    for (int i = 0; i < BIBLE_BOOK_COUNT; i++) {
        load_bible_book(&addr->books[i], toml, bible_book_names[i]);
    }
    addr->book_count = BIBLE_BOOK_COUNT;

    // Load ternary config
    load_bible_ternary(&addr->ternary, toml);

    // Load WEB variants
    for (int i = 0; i < BIBLE_WEB_VARIANTS; i++) {
        load_bible_variant(&addr->variants[i], toml, i);
    }
    addr->variant_count = BIBLE_WEB_VARIANTS;

    addr->loaded = true;
    toml_free(toml);
    return CONFIG_OK;
}

// Helper to load a Hebrew root
static void load_hebrew_root(HebrewRootConfig* root, const char* toml, const char* name) {
    char section_name[128];
    const char* section;
    const char* p;

    snprintf(section_name, sizeof(section_name), "hebrew.%s", name);
    section = toml_find_section(toml, section_name);
    if (!section) return;

    p = toml_find_key(section, "root");
    if (p) toml_parse_string(p, root->root, sizeof(root->root));

    p = toml_find_key(section, "transliteration");
    if (p) toml_parse_string(p, root->transliteration, sizeof(root->transliteration));

    p = toml_find_key(section, "meaning");
    if (p) toml_parse_string(p, root->meaning, sizeof(root->meaning));

    p = toml_find_key(section, "semantic_field");
    if (p) toml_parse_string(p, root->semantic_field, sizeof(root->semantic_field));

    p = toml_find_key(section, "scripture");
    if (p) toml_parse_string(p, root->scripture, sizeof(root->scripture));

    p = toml_find_key(section, "omnicode_mapping");
    if (p) toml_parse_string(p, root->omnicode_mapping, sizeof(root->omnicode_mapping));

    // Load maps_to array
    p = toml_find_key(section, "maps_to");
    if (p) {
        root->maps_to_count = 0;
        const char* arr = toml_array_first(p);
        while (arr && root->maps_to_count < 4) {
            const char* next = toml_parse_string(arr, root->maps_to[root->maps_to_count],
                                                  sizeof(root->maps_to[0]));
            if (next) {
                root->maps_to_count++;
                arr = toml_array_next(next);
            } else {
                break;
            }
        }
    }
}

// Helper to load a Greek term
static void load_greek_term(GreekTermConfig* term, const char* toml, const char* name) {
    char section_name[128];
    const char* section;
    const char* p;

    snprintf(section_name, sizeof(section_name), "greek.%s", name);
    section = toml_find_section(toml, section_name);
    if (!section) return;

    p = toml_find_key(section, "term");
    if (p) toml_parse_string(p, term->term, sizeof(term->term));

    p = toml_find_key(section, "transliteration");
    if (p) toml_parse_string(p, term->transliteration, sizeof(term->transliteration));

    p = toml_find_key(section, "meaning");
    if (p) toml_parse_string(p, term->meaning, sizeof(term->meaning));

    p = toml_find_key(section, "scripture");
    if (p) toml_parse_string(p, term->scripture, sizeof(term->scripture));

    p = toml_find_key(section, "omnicode_mapping");
    if (p) toml_parse_string(p, term->omnicode_mapping, sizeof(term->omnicode_mapping));
}

// Helper to load a biblical pattern
static void load_biblical_pattern(BiblicalPatternConfig* pat, const char* toml,
                                   const char* category, const char* name) {
    char section_name[128];
    const char* section;
    const char* p;

    snprintf(section_name, sizeof(section_name), "%s.%s", category, name);
    section = toml_find_section(toml, section_name);
    if (!section) return;

    p = toml_find_key(section, "biblical_pattern");
    if (p) toml_parse_string(p, pat->biblical_pattern, sizeof(pat->biblical_pattern));

    p = toml_find_key(section, "scripture");
    if (p) toml_parse_string(p, pat->scripture, sizeof(pat->scripture));

    p = toml_find_key(section, "syntactic_role");
    if (p) toml_parse_string(p, pat->syntactic_role, sizeof(pat->syntactic_role));

    p = toml_find_key(section, "omnicode_form");
    if (p) toml_parse_string(p, pat->omnicode_form, sizeof(pat->omnicode_form));

    p = toml_find_key(section, "example");
    if (p) toml_parse_string(p, pat->example, sizeof(pat->example));
}

// Hebrew root names for loading
static const char* hebrew_root_names[] = {
    "bara", "hayah", "badal", "qara", "asah", "yalad",
    "brit", "natan", "shuv", "tohu", "vav"
};
#define HEBREW_ROOT_COUNT 11

// Greek term names for loading
static const char* greek_term_names[] = {
    "logos", "aletheia", "pseudos", "diatheke", "tupos", "genos", "kleronomia"
};
#define GREEK_TERM_COUNT 7

// Creation pattern names
static const char* creation_pattern_names[] = {
    "let_there_be", "let_thing_be", "god_made", "god_called",
    "god_divided", "after_its_kind", "and_it_was_so"
};
#define CREATION_PATTERN_COUNT 7

// Covenant pattern names
static const char* covenant_pattern_names[] = {
    "if_ye_will", "i_will_be", "ye_shall_be", "i_have_given", "bring_back"
};
#define COVENANT_PATTERN_COUNT 5

// Genealogy pattern names
static const char* genealogy_pattern_names[] = {
    "begat", "in_likeness", "generations_of", "son_of"
};
#define GENEALOGY_PATTERN_COUNT 4

// config_load_bible_encoding loads encoding.toml.
int config_load_bible_encoding(BibleEncodingConfig* enc, const char* path) {
    if (!enc || !path) return CONFIG_ERR_NULL;

    char* toml = toml_load_file(path);
    if (!toml) return CONFIG_ERR_FILE;

    // Load Hebrew roots
    for (int i = 0; i < HEBREW_ROOT_COUNT && i < BIBLE_HEBREW_ROOTS; i++) {
        load_hebrew_root(&enc->hebrew[i], toml, hebrew_root_names[i]);
    }
    enc->hebrew_count = HEBREW_ROOT_COUNT;

    // Load Greek terms
    for (int i = 0; i < GREEK_TERM_COUNT && i < BIBLE_GREEK_TERMS; i++) {
        load_greek_term(&enc->greek[i], toml, greek_term_names[i]);
    }
    enc->greek_count = GREEK_TERM_COUNT;

    // Load creation patterns
    for (int i = 0; i < CREATION_PATTERN_COUNT && i < BIBLE_PATTERNS; i++) {
        load_biblical_pattern(&enc->creation[i], toml, "creation", creation_pattern_names[i]);
    }
    enc->creation_count = CREATION_PATTERN_COUNT;

    // Load covenant patterns
    for (int i = 0; i < COVENANT_PATTERN_COUNT && i < BIBLE_PATTERNS; i++) {
        load_biblical_pattern(&enc->covenant[i], toml, "covenant", covenant_pattern_names[i]);
    }
    enc->covenant_count = COVENANT_PATTERN_COUNT;

    // Load genealogy patterns
    for (int i = 0; i < GENEALOGY_PATTERN_COUNT && i < BIBLE_PATTERNS; i++) {
        load_biblical_pattern(&enc->genealogy[i], toml, "genealogy", genealogy_pattern_names[i]);
    }
    enc->genealogy_count = GENEALOGY_PATTERN_COUNT;

    enc->loaded = true;
    toml_free(toml);
    return CONFIG_OK;
}

// config_load_bible loads all Bible configs from directory.
int config_load_bible(Config* cfg, const char* bible_dir) {
    if (!cfg || !bible_dir) return CONFIG_ERR_NULL;

    char path[CONFIG_PATH_MAX];
    int result;

    // Load addressing.toml
    snprintf(path, sizeof(path), "%s" PATH_SEP "addressing.toml", bible_dir);
    result = config_load_bible_addressing(&cfg->bible.addressing, path);
    if (result != CONFIG_OK) return result;

    // Load encoding.toml
    snprintf(path, sizeof(path), "%s" PATH_SEP "encoding.toml", bible_dir);
    result = config_load_bible_encoding(&cfg->bible.encoding, path);
    if (result != CONFIG_OK) return result;

    cfg->bible.loaded = true;
    return CONFIG_OK;
}

// # B.8 Bible Utility Functions [BIBLE_UTIL]

// bible_verse_to_ordinal looks up ordinal from book/chapter/verse.
// Note: This requires chapter-level data not in current TOML.
// For now, returns -1 if not found (simplified implementation).
int32_t bible_verse_to_ordinal(const BibleAddressingConfig* addr,
                                const char* book, int chapter, int verse) {
    if (!addr || !book) return -1;

    // Find the book
    for (int i = 0; i < addr->book_count; i++) {
        if (strcmp(addr->books[i].name, book) == 0 ||
            strcmp(addr->books[i].abbreviation, book) == 0) {
            // Found book - need chapter/verse offset calculation
            // For now, approximate with book range start
            // Full implementation requires chapter verse counts
            (void)chapter;
            (void)verse;
            return addr->books[i].range_start;
        }
    }
    return -1;
}

// bible_ordinal_to_verse looks up book/chapter/verse from ordinal.
int bible_ordinal_to_verse(const BibleAddressingConfig* addr, int32_t ordinal,
                           char* book, size_t book_size, int* chapter, int* verse) {
    if (!addr || !book || ordinal < 1 || ordinal > BIBLE_KJV_VERSES) return -1;

    // Find which book contains this ordinal
    for (int i = 0; i < addr->book_count; i++) {
        if (ordinal >= addr->books[i].range_start &&
            ordinal <= addr->books[i].range_end) {
            strncpy(book, addr->books[i].name, book_size - 1);
            book[book_size - 1] = '\0';
            // Chapter/verse calculation requires chapter data
            if (chapter) *chapter = 1;
            if (verse) *verse = ordinal - addr->books[i].range_start + 1;
            return 0;
        }
    }
    return -1;
}

// bible_find_hebrew_by_keyword finds Hebrew root mapping by OmniCode keyword.
const HebrewRootConfig* bible_find_hebrew_by_keyword(
    const BibleEncodingConfig* enc, const char* keyword) {
    if (!enc || !keyword) return NULL;

    for (int i = 0; i < enc->hebrew_count; i++) {
        for (int j = 0; j < enc->hebrew[i].maps_to_count; j++) {
            if (strcmp(enc->hebrew[i].maps_to[j], keyword) == 0) {
                return &enc->hebrew[i];
            }
        }
    }
    return NULL;
}

// bible_find_scripture_source finds Scripture reference for OmniCode keyword.
int bible_find_scripture_source(const BibleEncodingConfig* enc,
                                const char* keyword,
                                char* scripture, size_t scripture_size) {
    if (!enc || !keyword || !scripture) return -1;

    const HebrewRootConfig* root = bible_find_hebrew_by_keyword(enc, keyword);
    if (root) {
        strncpy(scripture, root->scripture, scripture_size - 1);
        scripture[scripture_size - 1] = '\0';
        return 0;
    }

    return -1;
}

// ---------------------------------------------------------------------------
// B.7 Display Configuration Loaders
// ---------------------------------------------------------------------------

// Color name to DisplayColor enum value mapping.
static const struct {
    const char* name;
    int8_t value;
} color_map[] = {
    {"default",        0},
    {"black",          1},
    {"red",            2},
    {"green",          3},
    {"yellow",         4},
    {"blue",           5},
    {"magenta",        6},
    {"cyan",           7},
    {"white",          8},
    {"bright_black",   9},
    {"bright_red",     10},
    {"bright_green",   11},
    {"bright_yellow",  12},
    {"bright_blue",    13},
    {"bright_magenta", 14},
    {"bright_cyan",    15},
    {"bright_white",   16},
    {NULL, 0}
};

// display_color_from_name converts color name to DisplayColor enum value.
int8_t display_color_from_name(const char* name) {
    if (!name) return 0;  // default
    for (int i = 0; color_map[i].name; i++) {
        if (strcmp(color_map[i].name, name) == 0) {
            return color_map[i].value;
        }
    }
    return 0;  // default
}

// Helper: Load theme syntax colors from JSON object.
static void load_theme_syntax(DisplayThemeSyntax* syntax, const JsonValue* obj) {
    if (!syntax || !obj) return;

    const char* s;

    // Normal text
    if ((s = jsonc_object_get_string(obj, "normal")))
        strncpy(syntax->normal, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "normal_bg")))
        strncpy(syntax->normal_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->normal_bold = jsonc_object_get_bool(obj, "normal_bold", false);

    // Pragma directives (#!omni)
    if ((s = jsonc_object_get_string(obj, "pragma")))
        strncpy(syntax->pragma, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "pragma_bg")))
        strncpy(syntax->pragma_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->pragma_bold = jsonc_object_get_bool(obj, "pragma_bold", false);

    // Keywords
    if ((s = jsonc_object_get_string(obj, "keyword")))
        strncpy(syntax->keyword, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "keyword_bg")))
        strncpy(syntax->keyword_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->keyword_bold = jsonc_object_get_bool(obj, "keyword_bold", false);

    // Comments
    if ((s = jsonc_object_get_string(obj, "comment")))
        strncpy(syntax->comment, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "comment_bg")))
        strncpy(syntax->comment_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->comment_bold = jsonc_object_get_bool(obj, "comment_bold", false);

    // String literals
    if ((s = jsonc_object_get_string(obj, "string")))
        strncpy(syntax->string, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "string_bg")))
        strncpy(syntax->string_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->string_bold = jsonc_object_get_bool(obj, "string_bold", false);

    // Numeric literals
    if ((s = jsonc_object_get_string(obj, "number")))
        strncpy(syntax->number, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "number_bg")))
        strncpy(syntax->number_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->number_bold = jsonc_object_get_bool(obj, "number_bold", false);

    // Block markers
    if ((s = jsonc_object_get_string(obj, "block_marker")))
        strncpy(syntax->block_marker, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "block_marker_bg")))
        strncpy(syntax->block_marker_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->block_marker_bold = jsonc_object_get_bool(obj, "block_marker_bold", false);

    // Section markers
    if ((s = jsonc_object_get_string(obj, "section_marker")))
        strncpy(syntax->section_marker, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "section_marker_bg")))
        strncpy(syntax->section_marker_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->section_marker_bold = jsonc_object_get_bool(obj, "section_marker_bold", false);

    // Identifiers
    if ((s = jsonc_object_get_string(obj, "identifier")))
        strncpy(syntax->identifier, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "identifier_bg")))
        strncpy(syntax->identifier_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->identifier_bold = jsonc_object_get_bool(obj, "identifier_bold", false);

    // Operators
    if ((s = jsonc_object_get_string(obj, "operator")))
        strncpy(syntax->operator_c, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "operator_bg")))
        strncpy(syntax->operator_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->operator_bold = jsonc_object_get_bool(obj, "operator_bold", false);

    // Type names
    if ((s = jsonc_object_get_string(obj, "type")))
        strncpy(syntax->type, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "type_bg")))
        strncpy(syntax->type_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->type_bold = jsonc_object_get_bool(obj, "type_bold", false);

    // Function names
    if ((s = jsonc_object_get_string(obj, "function")))
        strncpy(syntax->function, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "function_bg")))
        strncpy(syntax->function_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->function_bold = jsonc_object_get_bool(obj, "function_bold", false);

    // Error highlighting
    if ((s = jsonc_object_get_string(obj, "error")))
        strncpy(syntax->error, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "error_bg")))
        strncpy(syntax->error_bg, s, CONFIG_LABEL_MAX - 1);
    syntax->error_bold = jsonc_object_get_bool(obj, "error_bold", false);
}

// Helper: Load theme blocks colors from JSON object.
static void load_theme_blocks(DisplayThemeBlocks* blocks, const JsonValue* obj) {
    if (!blocks || !obj) return;

    const char* s;

    // METADATA block
    if ((s = jsonc_object_get_string(obj, "metadata")))
        strncpy(blocks->metadata, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "metadata_bg")))
        strncpy(blocks->metadata_bg, s, CONFIG_LABEL_MAX - 1);
    blocks->metadata_bold = jsonc_object_get_bool(obj, "metadata_bold", false);

    // SETUP block
    if ((s = jsonc_object_get_string(obj, "setup")))
        strncpy(blocks->setup, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "setup_bg")))
        strncpy(blocks->setup_bg, s, CONFIG_LABEL_MAX - 1);
    blocks->setup_bold = jsonc_object_get_bool(obj, "setup_bold", false);

    // HEADER block
    if ((s = jsonc_object_get_string(obj, "header")))
        strncpy(blocks->header, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "header_bg")))
        strncpy(blocks->header_bg, s, CONFIG_LABEL_MAX - 1);
    blocks->header_bold = jsonc_object_get_bool(obj, "header_bold", false);

    // CONTEXT block
    if ((s = jsonc_object_get_string(obj, "context")))
        strncpy(blocks->context, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "context_bg")))
        strncpy(blocks->context_bg, s, CONFIG_LABEL_MAX - 1);
    blocks->context_bold = jsonc_object_get_bool(obj, "context_bold", false);

    // BODY block
    if ((s = jsonc_object_get_string(obj, "body")))
        strncpy(blocks->body, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "body_bg")))
        strncpy(blocks->body_bg, s, CONFIG_LABEL_MAX - 1);
    blocks->body_bold = jsonc_object_get_bool(obj, "body_bold", false);

    // CONTENT block
    if ((s = jsonc_object_get_string(obj, "content")))
        strncpy(blocks->content, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "content_bg")))
        strncpy(blocks->content_bg, s, CONFIG_LABEL_MAX - 1);
    blocks->content_bold = jsonc_object_get_bool(obj, "content_bold", false);

    // CLOSING block
    if ((s = jsonc_object_get_string(obj, "closing")))
        strncpy(blocks->closing, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "closing_bg")))
        strncpy(blocks->closing_bg, s, CONFIG_LABEL_MAX - 1);
    blocks->closing_bold = jsonc_object_get_bool(obj, "closing_bold", false);

    // FOOTER block
    if ((s = jsonc_object_get_string(obj, "footer")))
        strncpy(blocks->footer, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "footer_bg")))
        strncpy(blocks->footer_bg, s, CONFIG_LABEL_MAX - 1);
    blocks->footer_bold = jsonc_object_get_bool(obj, "footer_bold", false);
}

// Helper: Load theme menu colors from JSON object.
static void load_theme_menu(DisplayThemeMenu* menu, const JsonValue* obj) {
    if (!menu || !obj) return;

    const char* s;

    // Menu bar
    if ((s = jsonc_object_get_string(obj, "bar_fg")))
        strncpy(menu->bar_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "bar_bg")))
        strncpy(menu->bar_bg, s, CONFIG_LABEL_MAX - 1);
    menu->bar_bold = jsonc_object_get_bool(obj, "bar_bold", false);

    // Menu items
    if ((s = jsonc_object_get_string(obj, "item_fg")))
        strncpy(menu->item_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "item_bg")))
        strncpy(menu->item_bg, s, CONFIG_LABEL_MAX - 1);
    menu->item_bold = jsonc_object_get_bool(obj, "item_bold", false);

    // Selected item
    if ((s = jsonc_object_get_string(obj, "selected_fg")))
        strncpy(menu->selected_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "selected_bg")))
        strncpy(menu->selected_bg, s, CONFIG_LABEL_MAX - 1);
    menu->selected_bold = jsonc_object_get_bool(obj, "selected_bold", false);

    // Hotkey characters
    if ((s = jsonc_object_get_string(obj, "hotkey_fg")))
        strncpy(menu->hotkey_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "hotkey_bg")))
        strncpy(menu->hotkey_bg, s, CONFIG_LABEL_MAX - 1);
    menu->hotkey_bold = jsonc_object_get_bool(obj, "hotkey_bold", false);

    // Disabled items
    if ((s = jsonc_object_get_string(obj, "disabled_fg")))
        strncpy(menu->disabled_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "disabled_bg")))
        strncpy(menu->disabled_bg, s, CONFIG_LABEL_MAX - 1);
    menu->disabled_bold = jsonc_object_get_bool(obj, "disabled_bold", false);
}

// Helper: Load theme status colors from JSON object.
static void load_theme_status(DisplayThemeStatus* status, const JsonValue* obj) {
    if (!status || !obj) return;

    const char* s;

    // Status bar
    if ((s = jsonc_object_get_string(obj, "bar_fg")))
        strncpy(status->bar_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "bar_bg")))
        strncpy(status->bar_bg, s, CONFIG_LABEL_MAX - 1);
    status->bar_bold = jsonc_object_get_bool(obj, "bar_bold", false);

    // Mode indicator
    if ((s = jsonc_object_get_string(obj, "mode_fg")))
        strncpy(status->mode_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "mode_bg")))
        strncpy(status->mode_bg, s, CONFIG_LABEL_MAX - 1);
    status->mode_bold = jsonc_object_get_bool(obj, "mode_bold", false);

    // File name
    if ((s = jsonc_object_get_string(obj, "file_fg")))
        strncpy(status->file_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "file_bg")))
        strncpy(status->file_bg, s, CONFIG_LABEL_MAX - 1);
    status->file_bold = jsonc_object_get_bool(obj, "file_bold", false);

    // Position display
    if ((s = jsonc_object_get_string(obj, "position_fg")))
        strncpy(status->position_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "position_bg")))
        strncpy(status->position_bg, s, CONFIG_LABEL_MAX - 1);
    status->position_bold = jsonc_object_get_bool(obj, "position_bold", false);

    // Modified indicator
    if ((s = jsonc_object_get_string(obj, "modified_fg")))
        strncpy(status->modified_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "modified_bg")))
        strncpy(status->modified_bg, s, CONFIG_LABEL_MAX - 1);
    status->modified_bold = jsonc_object_get_bool(obj, "modified_bold", false);

    // Cursor position
    if ((s = jsonc_object_get_string(obj, "cursor_fg")))
        strncpy(status->cursor_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "cursor_bg")))
        strncpy(status->cursor_bg, s, CONFIG_LABEL_MAX - 1);
    status->cursor_bold = jsonc_object_get_bool(obj, "cursor_bold", false);
}

// Helper: Load theme editor colors from JSON object.
static void load_theme_editor(DisplayThemeEditor* editor, const JsonValue* obj) {
    if (!editor || !obj) return;

    const char* s;

    // Main text area
    if ((s = jsonc_object_get_string(obj, "text_fg")))
        strncpy(editor->text_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "text_bg")))
        strncpy(editor->text_bg, s, CONFIG_LABEL_MAX - 1);
    editor->text_bold = jsonc_object_get_bool(obj, "text_bold", false);

    // Line numbers
    if ((s = jsonc_object_get_string(obj, "line_number_fg")))
        strncpy(editor->line_number_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "line_number_bg")))
        strncpy(editor->line_number_bg, s, CONFIG_LABEL_MAX - 1);
    editor->line_number_bold = jsonc_object_get_bool(obj, "line_number_bold", false);

    // Current line highlight
    if ((s = jsonc_object_get_string(obj, "current_line_fg")))
        strncpy(editor->current_line_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "current_line_bg")))
        strncpy(editor->current_line_bg, s, CONFIG_LABEL_MAX - 1);
    editor->current_line_bold = jsonc_object_get_bool(obj, "current_line_bold", false);

    // Cursor
    if ((s = jsonc_object_get_string(obj, "cursor_fg")))
        strncpy(editor->cursor_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "cursor_bg")))
        strncpy(editor->cursor_bg, s, CONFIG_LABEL_MAX - 1);
    editor->cursor_bold = jsonc_object_get_bool(obj, "cursor_bold", false);

    // Selection
    if ((s = jsonc_object_get_string(obj, "selection_fg")))
        strncpy(editor->selection_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "selection_bg")))
        strncpy(editor->selection_bg, s, CONFIG_LABEL_MAX - 1);
    editor->selection_bold = jsonc_object_get_bool(obj, "selection_bold", false);

    // Matching bracket
    if ((s = jsonc_object_get_string(obj, "matching_bracket_fg")))
        strncpy(editor->matching_bracket_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "matching_bracket_bg")))
        strncpy(editor->matching_bracket_bg, s, CONFIG_LABEL_MAX - 1);
    editor->matching_bracket_bold = jsonc_object_get_bool(obj, "matching_bracket_bold", false);

    // Gutter
    if ((s = jsonc_object_get_string(obj, "gutter_fg")))
        strncpy(editor->gutter_fg, s, CONFIG_LABEL_MAX - 1);
    if ((s = jsonc_object_get_string(obj, "gutter_bg")))
        strncpy(editor->gutter_bg, s, CONFIG_LABEL_MAX - 1);
    editor->gutter_bold = jsonc_object_get_bool(obj, "gutter_bold", false);
}

// Helper: Load a single theme from JSON object.
static void load_theme(DisplayTheme* theme, const char* name, const JsonValue* obj) {
    if (!theme || !obj) return;

    memset(theme, 0, sizeof(*theme));
    strncpy(theme->name, name, CONFIG_LABEL_MAX - 1);

    const char* s;
    if ((s = jsonc_object_get_string(obj, "description")))
        strncpy(theme->description, s, CONFIG_DESC_MAX - 1);

    JsonValue* syntax = jsonc_object_get(obj, "syntax");
    if (syntax) load_theme_syntax(&theme->syntax, syntax);

    JsonValue* blocks = jsonc_object_get(obj, "blocks");
    if (blocks) load_theme_blocks(&theme->blocks, blocks);

    JsonValue* menu = jsonc_object_get(obj, "menu");
    if (menu) load_theme_menu(&theme->menu, menu);

    JsonValue* status = jsonc_object_get(obj, "status");
    if (status) load_theme_status(&theme->status, status);

    JsonValue* editor = jsonc_object_get(obj, "editor");
    if (editor) load_theme_editor(&theme->editor, editor);
}

// Helper: Load keybindings for a context.
static int load_keybinding_context(DisplayKeyBinding* bindings, int max,
                                    const JsonValue* obj) {
    if (!bindings || !obj || !jsonc_is_object(obj)) return 0;

    int count = 0;
    for (JsonEntry* entry = jsonc_object_first(obj);
         entry && count < max;
         entry = entry->next) {
        if (entry->key && jsonc_is_string(entry->value)) {
            strncpy(bindings[count].key, entry->key, CONFIG_LABEL_MAX - 1);
            const char* action = jsonc_get_string(entry->value);
            if (action) {
                strncpy(bindings[count].action, action, CONFIG_LABEL_MAX - 1);
                count++;
            }
        }
    }
    return count;
}

// Helper: Load layout settings.
static void load_layout(DisplayLayout* layout, const JsonValue* obj) {
    if (!layout || !obj) return;

    JsonValue* editor = jsonc_object_get(obj, "editor");
    if (editor) {
        layout->line_number_width = (int)jsonc_object_get_int(editor, "line_number_width", 6);
        layout->left_margin = (int)jsonc_object_get_int(editor, "left_margin", 1);
        layout->right_margin = (int)jsonc_object_get_int(editor, "right_margin", 1);
        layout->tab_width = (int)jsonc_object_get_int(editor, "tab_width", 4);
        layout->scroll_margin = (int)jsonc_object_get_int(editor, "scroll_margin", 3);
    }

    JsonValue* panels = jsonc_object_get(obj, "panels");
    if (panels) {
        layout->file_tree_width = (int)jsonc_object_get_int(panels, "file_tree_width", 25);
        layout->output_height = (int)jsonc_object_get_int(panels, "output_height", 10);
        layout->min_editor_width = (int)jsonc_object_get_int(panels, "min_editor_width", 40);
        layout->min_editor_height = (int)jsonc_object_get_int(panels, "min_editor_height", 10);
    }

    JsonValue* menu = jsonc_object_get(obj, "menu");
    if (menu) {
        layout->menu_item_padding = (int)jsonc_object_get_int(menu, "item_padding", 2);
    }
}

// Helper: Load IDE settings.
static void load_ide_settings(DisplayIDESettings* ide, const JsonValue* obj) {
    if (!ide || !obj) return;

    JsonValue* editor = jsonc_object_get(obj, "editor");
    if (editor) {
        ide->show_line_numbers = jsonc_object_get_bool(editor, "show_line_numbers", true);
        ide->show_block_indicators = jsonc_object_get_bool(editor, "show_block_indicators", true);
        ide->highlight_current_line = jsonc_object_get_bool(editor, "highlight_current_line", true);
        ide->show_matching_brackets = jsonc_object_get_bool(editor, "show_matching_brackets", true);
        ide->auto_indent = jsonc_object_get_bool(editor, "auto_indent", true);
        ide->wrap_lines = jsonc_object_get_bool(editor, "wrap_lines", false);
        ide->tab_to_spaces = jsonc_object_get_bool(editor, "tab_to_spaces", true);
        ide->trim_trailing_whitespace = jsonc_object_get_bool(editor, "trim_trailing_whitespace", true);
        ide->ensure_newline_at_eof = jsonc_object_get_bool(editor, "ensure_newline_at_eof", true);
    }

    JsonValue* syntax = jsonc_object_get(obj, "syntax");
    if (syntax) {
        ide->syntax_enabled = jsonc_object_get_bool(syntax, "enabled", true);
    }

    JsonValue* blocks = jsonc_object_get(obj, "blocks");
    if (blocks) {
        ide->validate_on_save = jsonc_object_get_bool(blocks, "validate_on_save", true);
    }
}

// Helper: Load behavior settings.
static void load_behavior(DisplayBehavior* behavior, const JsonValue* obj) {
    if (!behavior || !obj) return;

    JsonValue* panic = jsonc_object_get(obj, "panic_recovery");
    if (panic) {
        behavior->panic_recovery_enabled = jsonc_object_get_bool(panic, "enabled", true);
    }

    JsonValue* access = jsonc_object_get(obj, "accessibility");
    if (access) {
        behavior->reduce_motion = jsonc_object_get_bool(access, "reduce_motion", false);
        behavior->high_contrast_mode = jsonc_object_get_bool(access, "high_contrast", false);
    }

    JsonValue* terminal = jsonc_object_get(obj, "terminal");
    if (terminal) {
        behavior->unicode_fallback = jsonc_object_get_bool(terminal, "unicode_fallback", true);
        behavior->color_fallback = jsonc_object_get_bool(terminal, "color_fallback", true);
        behavior->min_terminal_width = (int)jsonc_object_get_int(terminal, "min_width", 80);
        behavior->min_terminal_height = (int)jsonc_object_get_int(terminal, "min_height", 24);
    }
}

// config_load_display loads display configuration from JSONC file.
int config_load_display(IDEDisplayConfig* display, const char* path) {
    if (!display || !path) return CONFIG_ERR_NULL;

    memset(display, 0, sizeof(*display));

    JsonValue* root = jsonc_load(path);
    if (!root) {
        fprintf(stderr, "[config_load_display] Failed to load: %s\n", path);
        return CONFIG_ERR_FILE;
    }

    // Metadata
    JsonValue* metadata = jsonc_object_get(root, "metadata");
    if (metadata) {
        const char* s;
        if ((s = jsonc_object_get_string(metadata, "name")))
            strncpy(display->name, s, CONFIG_LABEL_MAX - 1);
        if ((s = jsonc_object_get_string(metadata, "version")))
            strncpy(display->version, s, CONFIG_LABEL_MAX - 1);
    }

    // Current theme
    JsonValue* themes = jsonc_object_get(root, "themes");
    if (themes) {
        const char* current = jsonc_object_get_string(themes, "current");
        if (current) {
            strncpy(display->current_theme, current, CONFIG_LABEL_MAX - 1);
        }

        // Load themes (skip "description" and "current" keys)
        int idx = 0;
        for (JsonEntry* entry = jsonc_object_first(themes);
             entry && idx < DISPLAY_THEME_COUNT;
             entry = entry->next) {
            if (strcmp(entry->key, "description") == 0 ||
                strcmp(entry->key, "current") == 0) {
                continue;
            }
            if (jsonc_is_object(entry->value)) {
                load_theme(&display->themes[idx], entry->key, entry->value);
                idx++;
            }
        }
        display->theme_count = idx;
    }

    // Keybindings
    JsonValue* keybindings = jsonc_object_get(root, "keybindings");
    if (keybindings) {
        JsonValue* global = jsonc_object_get(keybindings, "global");
        if (global) {
            display->keybindings.global_count = load_keybinding_context(
                display->keybindings.global, DISPLAY_KEY_BINDING_COUNT, global);
        }

        JsonValue* editor = jsonc_object_get(keybindings, "editor");
        if (editor) {
            display->keybindings.editor_count = load_keybinding_context(
                display->keybindings.editor, DISPLAY_KEY_BINDING_COUNT, editor);
        }

        JsonValue* menu = jsonc_object_get(keybindings, "menu");
        if (menu) {
            display->keybindings.menu_count = load_keybinding_context(
                display->keybindings.menu, DISPLAY_KEY_BINDING_COUNT, menu);
        }

        JsonValue* prompt = jsonc_object_get(keybindings, "prompt");
        if (prompt) {
            display->keybindings.prompt_count = load_keybinding_context(
                display->keybindings.prompt, DISPLAY_KEY_BINDING_COUNT, prompt);
        }
    }

    // Layout
    JsonValue* layout = jsonc_object_get(root, "layout");
    if (layout) {
        load_layout(&display->layout, layout);
    }

    // IDE settings
    JsonValue* ide = jsonc_object_get(root, "ide");
    if (ide) {
        load_ide_settings(&display->ide, ide);
    }

    // Behavior
    JsonValue* behavior = jsonc_object_get(root, "behavior");
    if (behavior) {
        load_behavior(&display->behavior, behavior);
    }

    display->loaded = true;
    jsonc_free(root);

    return CONFIG_OK;
}

// display_find_theme finds theme by name.
const DisplayTheme* display_find_theme(const IDEDisplayConfig* display, const char* name) {
    if (!display || !name) return NULL;

    for (int i = 0; i < display->theme_count; i++) {
        if (strcmp(display->themes[i].name, name) == 0) {
            return &display->themes[i];
        }
    }
    return NULL;
}

// display_get_current_theme gets the current theme.
const DisplayTheme* display_get_current_theme(const IDEDisplayConfig* display) {
    if (!display || !display->loaded) return NULL;
    return display_find_theme(display, display->current_theme);
}

// display_find_style finds style by name.
const DisplayStyleDef* display_find_style(const IDEDisplayConfig* display, const char* name) {
    if (!display || !name) return NULL;

    for (int i = 0; i < display->style_count; i++) {
        if (strcmp(display->styles[i].name, name) == 0) {
            return &display->styles[i];
        }
    }
    return NULL;
}

// display_find_action finds action for key in context.
const char* display_find_action(const IDEDisplayConfig* display,
                                 const char* context, const char* key) {
    if (!display || !context || !key) return NULL;

    const DisplayKeyBinding* bindings = NULL;
    int count = 0;

    if (strcmp(context, "global") == 0) {
        bindings = display->keybindings.global;
        count = display->keybindings.global_count;
    } else if (strcmp(context, "editor") == 0) {
        bindings = display->keybindings.editor;
        count = display->keybindings.editor_count;
    } else if (strcmp(context, "menu") == 0) {
        bindings = display->keybindings.menu;
        count = display->keybindings.menu_count;
    } else if (strcmp(context, "prompt") == 0) {
        bindings = display->keybindings.prompt;
        count = display->keybindings.prompt_count;
    }

    if (!bindings) return NULL;

    for (int i = 0; i < count; i++) {
        if (strcmp(bindings[i].key, key) == 0) {
            return bindings[i].action;
        }
    }
    return NULL;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Validation [VALIDATION]
//
// Build:
//   gcc -c -Wall -Wextra -std=c11 config.c -I../include
//
// Test:
//   Link with toml.c and test with demo program

// # X.2 Execution [EXECUTION]
//
// See config.h for usage examples

// # X.3 Cleanup [CLEANUP]
//
// config_destroy clears internal state
// If config_create was used, caller must free() the pointer

// # X.4 Policy [POLICY]
//
// Safe to Modify:
//   - Add loaders for new TOML sections
//   - Add accessor functions
//
// NEVER Modify:
//   - Struct field mappings (must match config.h)
//   - Error code values

// # X.8 Closing Note [NOTE]
//
// This implementation bridges TOML specifications to typed C code.
// Specifications define truth; code implements it faithfully.
//
// "Prepare thy work without, and make it fit for thyself in the field."
//   - Proverbs 24:27

// =============================================================================
// END CLOSING
// =============================================================================
