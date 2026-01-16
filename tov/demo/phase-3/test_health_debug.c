// Debug test for health operations
#include <stdio.h>
#include <string.h>
#include "../../../word/work/pkg/omni/include/omni_chunks.h"
#include "../../../word/work/pkg/omni/include/omni_parser.h"
#include "../../../word/work/pkg/omni/include/omni_ir.h"
#include "../../../word/work/pkg/omni/include/omni_vm.h"

int main(void) {
    printf("=== Health Operations Debug Test ===\n\n");

    // Minimal test with just health operations
    const char* source =
        "#!omni document --omni -executable\n"
        "PROGRAM \"health_test\"\n"
        "// BODY [BODY]\n"
        "witness \"Initial health is 50\"\n"
        "health add 25\n"
        "witness \"After add 25, should be 75\"\n"
        "health set 80\n"
        "witness \"After set 80, should be 80\"\n"
        "health add 10\n"
        "witness \"After add 10, should be 90\"\n"
        "// END BODY\n";

    // Parse
    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));
    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);
    OmniDocument* doc = omni_parser_parse(&parser);
    printf("Parsed %d nodes\n\n", doc ? doc->node_count : 0);

    // Generate IR
    OmniIRProgram* ir = omni_ir_from_ast(doc);
    printf("Generated %d instructions:\n", ir->code_length);

    // Disassemble IR
    for (uint32_t i = 0; i < ir->code_length; i++) {
        printf("  [%03d] %s", i, omni_opcode_name(ir->code[i].opcode));
        if (ir->code[i].opcode == OP_HEALTH_ADD ||
            ir->code[i].opcode == OP_HEALTH_SET ||
            ir->code[i].opcode == OP_STATE_TRANS) {
            printf(" %d", (int8_t)ir->code[i].operand1);
        }
        printf("\n");
    }

    // Execute
    printf("\nExecuting:\n");
    OmniVM* vm = omni_vm_create();
    omni_vm_load(vm, ir);
    printf("  Initial health: %d\n", vm->health);

    while (vm->state != VM_STATE_HALTED && vm->state != VM_STATE_ERROR) {
        uint32_t ip = vm->ip;
        OmniInstruction* inst = &vm->program->code[ip];

        if (inst->opcode == OP_HEALTH_ADD || inst->opcode == OP_HEALTH_SET) {
            printf("  [%03d] Executing %s %d, health before: %d\n",
                   ip, omni_opcode_name(inst->opcode),
                   (int8_t)inst->operand1, vm->health);
        }

        omni_vm_step(vm);

        if (inst->opcode == OP_HEALTH_ADD || inst->opcode == OP_HEALTH_SET) {
            printf("        health after: %d\n", vm->health);
        }
    }

    printf("\nFinal health: %d\n", vm->health);
    printf("Expected: 90\n");
    printf("Test %s\n", vm->health == 90 ? "PASSED" : "FAILED");

    omni_vm_destroy(vm);
    omni_ir_destroy(ir);
    omni_document_destroy(doc);
    return 0;
}
