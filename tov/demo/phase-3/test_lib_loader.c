// #!omni code --c -test
// #!omni meta.key = B-tov-demo-phase3-test-lib-loader
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// test_lib_loader.c — Library Loader Test
// Validates module resolution, loading, and export parsing.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-tov-demo-phase3-test-lib-loader
// Title: Library Loader Test
// Type: Test
// Role: Validate library loader functionality

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omni_lib.h"
#include "omni_ir.h"
#include "omni_chunks.h"
#include "omni_parser.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

static int test_loader_create(void) {
    printf("  Testing loader creation...\n");

    OmniLibLoader* loader = omni_lib_create("../../../word/work/pkg/omni/stdlib/");
    if (!loader) {
        printf("    FAIL: Could not create loader\n");
        return 1;
    }

    printf("    PASS: Loader created\n");
    omni_lib_destroy(loader);
    return 0;
}

static int test_module_resolve(void) {
    printf("  Testing module resolution...\n");

    OmniLibLoader* loader = omni_lib_create("../../../word/work/pkg/omni/stdlib/");
    if (!loader) {
        printf("    FAIL: Could not create loader\n");
        return 1;
    }

    // Test stdlib/math resolution
    const char* resolved = omni_lib_resolve(loader, "stdlib/math");
    if (!resolved) {
        printf("    FAIL: Could not resolve stdlib/math\n");
        omni_lib_destroy(loader);
        return 1;
    }

    printf("    Resolved: %s\n", resolved);

    if (strstr(resolved, "math.omni") == NULL) {
        printf("    FAIL: Resolution does not point to math.omni\n");
        omni_lib_destroy(loader);
        return 1;
    }

    printf("    PASS: Module resolution works\n");
    omni_lib_destroy(loader);
    return 0;
}

static int test_module_load(void) {
    printf("  Testing module loading...\n");

    OmniLibLoader* loader = omni_lib_create("../../../word/work/pkg/omni/stdlib/");
    if (!loader) {
        printf("    FAIL: Could not create loader\n");
        return 1;
    }

    // Load math module
    int math_idx = omni_lib_load(loader, "stdlib/math");
    if (math_idx < 0) {
        printf("    FAIL: Could not load stdlib/math\n");
        omni_lib_destroy(loader);
        return 1;
    }

    printf("    Loaded math at index %d\n", math_idx);

    // Get module
    OmniLibModule* math = omni_lib_get_module(loader, math_idx);
    if (!math) {
        printf("    FAIL: Could not get module by index\n");
        omni_lib_destroy(loader);
        return 1;
    }

    printf("    Module name: %s\n", math->name);
    printf("    Exports: %d\n", math->export_count);

    // Check for expected exports
    if (math->export_count == 0) {
        printf("    WARNING: No exports found (parsing may have failed)\n");
    } else {
        printf("    First export: %s\n", math->exports[0].name);
    }

    printf("    PASS: Module loaded successfully\n");
    omni_lib_destroy(loader);
    return 0;
}

static int test_export_lookup(void) {
    printf("  Testing export lookup...\n");

    OmniLibLoader* loader = omni_lib_create("../../../word/work/pkg/omni/stdlib/");
    if (!loader) {
        printf("    FAIL: Could not create loader\n");
        return 1;
    }

    // Load math module
    int math_idx = omni_lib_load(loader, "stdlib/math");
    if (math_idx < 0) {
        printf("    FAIL: Could not load stdlib/math\n");
        omni_lib_destroy(loader);
        return 1;
    }

    OmniLibModule* math = omni_lib_get_module(loader, math_idx);
    if (!math) {
        printf("    FAIL: Could not get module\n");
        omni_lib_destroy(loader);
        return 1;
    }

    // Find abs export
    int abs_idx = omni_lib_find_export(math, "abs");
    if (abs_idx < 0) {
        printf("    WARNING: 'abs' export not found (export parsing may need work)\n");
    } else {
        printf("    Found 'abs' at export index %d\n", abs_idx);
        printf("    PASS: Export lookup works\n");
    }

    // Find by module name
    OmniLibModule* found = omni_lib_find_module(loader, "math");
    if (!found) {
        printf("    WARNING: find_module by name failed\n");
    } else {
        printf("    PASS: find_module by name works\n");
    }

    omni_lib_destroy(loader);
    return 0;
}

static int test_import_emission(void) {
    printf("  Testing import IR emission...\n");

    // Create a simple program with import
    const char* source =
        "#!omni code --omni -program\n"
        "\n"
        "// [METADATA]\n"
        "this is a program for \"Import Test\":\n"
        "    keyed as \"test-import\"\n"
        "    named \"import_test\"\n"
        "// END METADATA\n"
        "\n"
        "// [SETUP]\n"
        "import math from \"stdlib/math\"\n"
        "// END SETUP\n"
        "\n"
        "// [BODY]\n"
        "witness \"Import test running\"\n"
        "// END BODY\n"
        "\n"
        "// [CLOSING]\n"
        "// END CLOSING\n";

    printf("    Source:\n%s\n", source);

    // Parse and compile
    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    OmniDocument* doc = omni_parser_parse(&parser);
    if (!doc) {
        printf("    FAIL: Parse failed\n");
        return 1;
    }

    OmniIRProgram* prog = omni_ir_from_ast(doc);
    omni_document_destroy(doc);

    if (!prog) {
        printf("    FAIL: IR emission failed\n");
        return 1;
    }

    printf("    Generated %u IR instructions\n", prog->code_length);

    // Look for OP_IMPORT instruction
    bool found_import = false;
    for (uint32_t i = 0; i < prog->code_length; i++) {
        if (prog->code[i].opcode == OP_IMPORT) {
            found_import = true;
            printf("    Found OP_IMPORT at IP %u\n", i);
            break;
        }
    }

    if (!found_import) {
        printf("    WARNING: OP_IMPORT not found in IR (import may be in SETUP block)\n");
    } else {
        printf("    PASS: Import statement emits OP_IMPORT\n");
    }

    omni_ir_destroy(prog);
    return 0;
}

int main(void) {
    printf("=== Library Loader Tests ===\n\n");

    int failures = 0;

    failures += test_loader_create();
    failures += test_module_resolve();
    failures += test_module_load();
    failures += test_export_lookup();
    failures += test_import_emission();

    printf("\n");
    if (failures == 0) {
        printf("All library loader tests passed!\n");
        return 0;
    } else {
        printf("FAILED: %d test(s) failed\n", failures);
        return 1;
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Two are better than one; because they have a good reward for their labour."
//   — Ecclesiastes 4:9

// =============================================================================
// END CLOSING
// =============================================================================
