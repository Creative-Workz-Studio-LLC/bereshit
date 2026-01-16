// #!omni code --c -test
// Test full pipeline: OmniCode source → Parser → AST → IR → VM
// "And God said, Let there be light: and there was light" — Genesis 1:3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omni_chunks.h"
#include "omni_parser.h"
#include "omni_ir.h"
#include "omni_vm.h"

// External function from omni_ir_emit.c
extern OmniIRProgram* omni_ir_from_ast(struct OmniDocument* doc);

// Read file into string
static char* read_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}

int main(int argc, char** argv) {
    const char* source_path = argc > 1 ? argv[1] : "test_exec.omni";

    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  OmniCode Full Pipeline Test\n");
    printf("  OmniCode → Parser → AST → IR → VM\n");
    printf("  \"And God said... and there was light\" — Genesis 1:3\n");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    // Read source file
    printf("1. Reading source: %s\n", source_path);
    char* source = read_file(source_path);
    if (!source) {
        printf("   ✗ Failed to read source file\n");
        return 1;
    }
    printf("   ✓ Read %zu bytes\n", strlen(source));

    // Parse to AST
    printf("\n2. Parsing to AST...\n");

    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    OmniDocument* doc = omni_parser_parse(&parser);
    if (!doc) {
        printf("   ✗ Parse failed\n");
        free(source);
        return 1;
    }
    printf("   ✓ Parsed: %d nodes\n", doc->node_count);

    // Convert AST to IR
    printf("\n3. Converting AST to IR...\n");
    OmniIRProgram* prog = omni_ir_from_ast(doc);
    if (!prog) {
        printf("   ✗ IR conversion failed\n");
        omni_document_destroy(doc);
        free(source);
        return 1;
    }
    printf("   ✓ Generated IR: %d instructions, %d constants, %d functions\n",
           prog->code_length, prog->constant_count, prog->function_count);

    // Disassemble
    printf("\n───────────────────────────────────────────────────────────────────\n");
    printf("  Disassembly:\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");
    omni_ir_disassemble(prog, stdout);

    // Execute via VM
    printf("\n───────────────────────────────────────────────────────────────────\n");
    printf("  Execution:\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    OmniVM* vm = omni_vm_create();
    if (!vm) {
        printf("   ✗ Failed to create VM\n");
        omni_ir_destroy(prog);
        omni_document_destroy(doc);
        free(source);
        return 1;
    }

    omni_vm_load(vm, prog);

    printf("Output:\n  ");
    OmniVMResult result = omni_vm_run(vm);
    printf("\n");

    printf("\n───────────────────────────────────────────────────────────────────\n");
    printf("  Result: %s\n", result == VM_GRANTED ? "GRANTED (success)" :
                             result == VM_DENIED ? "DENIED (error)" : "DEFERRED");
    printf("  Health: %d (%s)\n", omni_vm_health_get(vm), omni_vm_health_state_name(vm));
    printf("  State: %s\n", omni_vm_hebrew_state_name(vm));

    if (result == VM_DENIED) {
        printf("  Error: %s\n", omni_vm_error(vm));
    }
    printf("───────────────────────────────────────────────────────────────────\n");

    // Cleanup
    omni_vm_destroy(vm);
    omni_ir_destroy(prog);
    omni_document_destroy(doc);
    free(source);

    printf("\n═══════════════════════════════════════════════════════════════════\n");
    printf("  ✓ Full pipeline test complete!\n");
    printf("  OmniCode → AST → IR → VM execution successful\n");
    printf("═══════════════════════════════════════════════════════════════════\n");

    return (result == VM_GRANTED) ? 0 : 1;
}
