// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key: B-tov-demo-phase-3-demo-config-reader
// Title: Config Reader Demo
// Type: Demo
// Component: Demonstration
// Role: Validates TOML config loading and typed access
//
// Status: Active
// Version: a-03.00
// Created: 2026-01-13
// Updated: 2026-01-13
//
// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
// Copyright: (c) 2026 CreativeWorkzStudio LLC. All rights reserved.
//
// Path: tov/demo/phase-3/demo-config-reader/main.c
//
// Purpose: Demonstrate loading TOML specs into typed C structs
// Core Design: Load word/core/*.toml and display values
// Philosophy: Specifications drive code through typed configuration
//
// Scripture: Proverbs 24:27 - Prepare thy work without
// Principle: Configuration prepares before implementation executes
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// print_separator prints a visual separator
static void print_separator(const char* title) {
    printf("\n");
    printf("================================================================\n");
    printf("%s\n", title);
    printf("================================================================\n");
}

// print_trit_core displays TritCore configuration
static void print_trit_core(const TritCore* trit) {
    printf("\n[trit] Core Trit Values:\n");
    printf("  values:       [%d, %d, %d]\n",
           trit->values[0], trit->values[1], trit->values[2]);
    printf("  labels:       [\"%s\", \"%s\", \"%s\"]\n",
           trit->labels[0], trit->labels[1], trit->labels[2]);
    printf("  short_labels: [\"%s\", \"%s\", \"%s\"]\n",
           trit->short_labels[0], trit->short_labels[1], trit->short_labels[2]);
    printf("  states:       %d\n", trit->states);
    printf("  base:         %d\n", trit->base);
    printf("  identity_sum: %d\n", trit->identity_sum);
}

// print_packing displays PackingConfig
static void print_packing(const PackingConfig* pack) {
    printf("\n[packing] Trit-Byte Packing:\n");
    printf("  trits_per_byte: %d\n", pack->trits_per_byte);
    printf("  max_trit_value: %d (3^5 = 243)\n", pack->max_trit_value);
    printf("  byte_capacity:  %d (2^8 = 256)\n", pack->byte_capacity);
    printf("  gap:            %d (WEB translation variants)\n", pack->gap);
    printf("  trit5_powers:   [");
    for (int i = 0; i < TRIT5_POWERS_COUNT; i++) {
        printf("%ld%s", (long)pack->trit5_powers[i],
               i < TRIT5_POWERS_COUNT - 1 ? ", " : "]\n");
    }
}

// print_arithmetic displays ArithmeticConfig
static void print_arithmetic(const ArithmeticConfig* arith) {
    printf("\n[arithmetic] Basic Operations:\n");
    printf("  negation: [%d, %d, %d]\n",
           arith->negation[0], arith->negation[1], arith->negation[2]);
    printf("  multiplication table:\n");
    for (int i = 0; i < 3; i++) {
        printf("    [%2d, %2d, %2d]\n",
               arith->multiplication[i][0],
               arith->multiplication[i][1],
               arith->multiplication[i][2]);
    }
}

// print_building_block displays BuildingBlockConfig
static void print_building_block(const BuildingBlockConfig* bb) {
    printf("\n[building_block] CPI-SI Reasoning Method:\n");
    printf("  operations: [\"%s\", \"%s\", \"%s\"]\n",
           bb->operations[0], bb->operations[1], bb->operations[2]);
    printf("  equation:   \"%s\"\n", bb->equation);
}

// print_dimensions displays DimensionsConfig
static void print_dimensions(const DimensionsConfig* dims) {
    printf("\n[dimensions] Cognitive Navigation:\n");
    printf("  temporal (TIME):    trit=%+d, question=\"%s\", building_block=\"%s\"\n",
           dims->temporal.trit, dims->temporal.question, dims->temporal.building_block);
    printf("  spatial (SPACE):    trit=%+d, question=\"%s\", building_block=\"%s\"\n",
           dims->spatial.trit, dims->spatial.question, dims->spatial.building_block);
    printf("  material (MATTER):  trit=%+d, question=\"%s\", building_block=\"%s\"\n",
           dims->material.trit, dims->material.question, dims->material.building_block);
}

// print_temporal_states displays TemporalStatesConfig
static void print_temporal_states(const TemporalStatesConfig* ts) {
    printf("\n[temporal_states] 9-State Temporal Model:\n");
    printf("  Base States:\n");
    printf("    past:    trit=%+d, meaning=\"%s\"\n", ts->past.trit, ts->past.meaning);
    printf("    present: trit=%+d, meaning=\"%s\"\n", ts->present.trit, ts->present.meaning);
    printf("    future:  trit=%+d, meaning=\"%s\"\n", ts->future.trit, ts->future.meaning);
    printf("  Counts:\n");
    printf("    base_count:     %d\n", ts->base_count);
    printf("    compound_count: %d\n", ts->compound_count);
    printf("    total_modes:    %d\n", ts->total_modes);
}

// print_primitive displays PrimitiveTypeConfig
static void print_primitive(const char* name, const PrimitiveTypeConfig* pt) {
    printf("  [%s]\n", name);
    printf("    name:         \"%s\"\n", pt->name);
    if (pt->size_trits >= 0) {
        printf("    size_trits:   %d\n", pt->size_trits);
    } else {
        printf("    size_trits:   variable\n");
    }
    printf("    min_value:    %ld\n", (long)pt->min_value);
    printf("    max_value:    %ld\n", (long)pt->max_value);
    printf("    total_states: %ld\n", (long)pt->total_states);
    printf("    signed:       %s\n", pt->is_signed ? "true" : "false");
    printf("    nullable:     %s\n", pt->is_nullable ? "true" : "false");
}

// print_primitives displays PrimitivesConfig
static void print_primitives(const PrimitivesConfig* prims) {
    printf("\nPrimitive Types:\n");
    print_primitive("trit", &prims->trit);
    print_primitive("trit5", &prims->trit5);
    print_primitive("int9", &prims->int9);
    print_primitive("uint9", &prims->uint9);
    print_primitive("int27", &prims->int27);
    print_primitive("uint27", &prims->uint27);
    print_primitive("bool3", &prims->bool3);
}

int main(int argc, char* argv[]) {
    printf("=== Demo Config Reader (Phase 3) ===\n");
    printf("Loading TOML specifications into typed C structs\n");

    // Determine core directory path
    const char* core_dir = "word/core";
    if (argc > 1) {
        core_dir = argv[1];
    }

    printf("\nCore directory: %s\n", core_dir);

    // Initialize config
    Config cfg;
    int result = config_init(&cfg);
    if (result != CONFIG_OK) {
        fprintf(stderr, "Error: config_init failed: %s\n",
                config_get_error_string(result));
        return 1;
    }

    // Load configuration from directory
    result = config_load_from_dir(&cfg, core_dir);
    if (result != CONFIG_OK) {
        fprintf(stderr, "Error: config_load_from_dir failed: %s\n",
                config_get_error_string(result));
        return 1;
    }

    printf("\nConfiguration loaded successfully!\n");

    // === Display ternary.toml contents ===
    print_separator("ternary.toml - Ternary Math Constants");

    print_trit_core(&cfg.math.trit);
    print_packing(&cfg.math.packing);
    print_arithmetic(&cfg.math.arithmetic);
    print_building_block(&cfg.math.building_block);
    print_dimensions(&cfg.math.dimensions);
    print_temporal_states(&cfg.math.temporal_states);

    // === Display primitives.toml contents ===
    print_separator("primitives.toml - Primitive Types");

    print_primitives(&cfg.primitives);

    // === Summary ===
    print_separator("Summary");

    printf("\nConfig reader demonstration complete.\n");
    printf("  Loaded: ternary.toml, primitives.toml\n");
    printf("  Status: %s\n", config_is_loaded(&cfg) ? "LOADED" : "NOT LOADED");
    printf("\n\"Prepare thy work without, and make it fit for thyself in the field.\"\n");
    printf("  - Proverbs 24:27\n\n");

    // Cleanup
    config_destroy(&cfg);

    return 0;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Build: make
// Run:   make run
// Clean: make clean
//
// =============================================================================
// END CLOSING
// =============================================================================
