// #!omni code --c -executable
// #!omni meta.key = B-tov-demo-phase3-test-cpisi
// #!omni meta.from = word/seed/code/c/source.c
//
// test_cpisi_pipeline.c — Test CPI-SI Operations from OmniCode
// Full pipeline: OmniCode → Parser → IR → VM with health/state/DAR

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-tov-demo-phase3-test-cpisi
// Title: CPI-SI Pipeline Integration Test
// Type: Executable
// Purpose: Verify CPI-SI operations work from OmniCode language level

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../word/work/pkg/omni/include/omni_chunks.h"
#include "../../../word/work/pkg/omni/include/omni_parser.h"
#include "../../../word/work/pkg/omni/include/omni_ir.h"
#include "../../../word/work/pkg/omni/include/omni_vm.h"

// =============================================================================
// BODY [BODY]
// =============================================================================

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    printf("=== CPI-SI Pipeline Integration Test ===\n\n");

    // Test source with CPI-SI operations
    // Note: IR emitter expects BODY block syntax, not FUNCTION definitions
    const char* source =
        "#!omni document --omni -executable\n"
        "PROGRAM \"cpisi_test\"\n"
        "\n"
        "// =============================================================================\n"
        "// BODY [BODY]\n"
        "// =============================================================================\n"
        "\n"
        "witness \"Starting CPI-SI test\"\n"
        "\n"
        "// Test health operations\n"
        "health get\n"
        "witness \"Got health value\"\n"
        "\n"
        "health add 25\n"
        "witness \"Added 25 to health\"\n"
        "\n"
        "health set 80\n"
        "witness \"Set health to 80\"\n"
        "\n"
        "// Test state operations\n"
        "state get\n"
        "witness \"Got Hebrew state\"\n"
        "\n"
        "state transition expand\n"
        "witness \"Expanded state toward tov\"\n"
        "\n"
        "// Test checkpoint/restore\n"
        "checkpoint\n"
        "witness \"Created checkpoint\"\n"
        "\n"
        "health add 10\n"
        "witness \"Modified health after checkpoint\"\n"
        "\n"
        "witness \"CPI-SI test complete!\"\n"
        "\n"
        "// =============================================================================\n"
        "// END BODY\n"
        "// =============================================================================\n";

    // Phase 1: Parse OmniCode to AST
    printf("Phase 1: Parsing OmniCode...\n");

    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    OmniDocument* doc = omni_parser_parse(&parser);
    if (!doc) {
        printf("ERROR: Failed to parse OmniCode\n");
        return 1;
    }
    printf("  ✓ Parsed %d AST nodes\n", doc->node_count);

    // Phase 2: Convert AST to IR
    printf("\nPhase 2: Generating IR...\n");
    OmniIRProgram* ir = omni_ir_from_ast(doc);
    if (!ir) {
        printf("ERROR: Failed to generate IR\n");
        omni_document_destroy(doc);
        return 1;
    }
    printf("  ✓ Generated %d instructions\n", ir->code_length);

    // Phase 3: Execute in VM
    printf("\nPhase 3: Executing in VM...\n");
    printf("--- VM Output ---\n");

    OmniVM* vm = omni_vm_create();
    if (!omni_vm_load(vm, ir)) {
        printf("ERROR: Failed to load program into VM\n");
        omni_ir_destroy(ir);
        omni_document_destroy(doc);
        return 1;
    }

    // Show initial state
    printf("\nInitial VM state:\n");
    printf("  Health: %d (%s)\n", vm->health, omni_vm_health_state_name(vm));
    printf("  Hebrew: %s\n\n", omni_vm_hebrew_state_name(vm));

    // Run program
    OmniVMResult result = omni_vm_run(vm);

    // Show final state
    printf("\n--- End VM Output ---\n");
    printf("\nFinal VM state:\n");
    printf("  Result: %s\n",
           result == VM_GRANTED ? "GRANTED" :
           result == VM_DENIED ? "DENIED" : "DEFERRED");
    printf("  Health: %d (%s)\n", vm->health, omni_vm_health_state_name(vm));
    printf("  Hebrew: %s\n", omni_vm_hebrew_state_name(vm));
    printf("  Has checkpoint: %s\n", vm->has_checkpoint ? "yes" : "no");

    if (result == VM_DENIED) {
        printf("  Error: %s (line %d)\n",
               omni_vm_error(vm), omni_vm_error_line(vm));
    }

    // Cleanup
    omni_vm_destroy(vm);
    omni_ir_destroy(ir);
    omni_document_destroy(doc);

    printf("\n=== Test %s ===\n",
           result == VM_GRANTED ? "PASSED" : "FAILED");
    return result == VM_GRANTED ? 0 : 1;
}

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "And God said, Let there be light: and there was light." — Genesis 1:3
// Words become action. OmniCode becomes CPI-SI operations.
