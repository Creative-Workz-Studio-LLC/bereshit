// #!omni code --c -executable
// #!omni meta.key = B-word-work-pkg-omni-test-codegen
// #!omni meta.at = a-01.00
//
// test_codegen.c — Test OmniCode Code Generator
// Parses OmniCode and generates C code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omni_lexer.h"
#include "omni_chunks.h"
#include "omni_parser.h"
#include "omni_codegen.h"

static char* read_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t read = fread(buf, 1, size, f);
    buf[read] = '\0';
    fclose(f);

    return buf;
}

int main(int argc, char** argv) {
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("  OmniCode Code Generator Test\n");
    printf("  \"Let there be light\" — Genesis 1:3\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

    // Get input file
    const char* input_file = (argc > 1) ? argv[1] : "test/test_4block.omni";
    const char* output_file = (argc > 2) ? argv[2] : NULL;

    printf("Input:  %s\n", input_file);
    printf("Output: %s\n\n", output_file ? output_file : "(stdout)");

    // Read source
    char* source = read_file(input_file);
    if (!source) {
        fprintf(stderr, "Error: Could not read %s\n", input_file);
        return 1;
    }

    printf("───────────────────────────────────────────────────────────────────────────────\n");
    printf("  Phase 1: Parsing OmniCode\n");
    printf("───────────────────────────────────────────────────────────────────────────────\n\n");

    // Initialize chunk lexer
    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    // Initialize parser
    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    // Parse
    OmniDocument* doc = omni_parser_parse(&parser);
    if (!doc) {
        fprintf(stderr, "Error: Parse failed\n");
        free(source);
        return 1;
    }

    printf("  ✓ Parsed successfully\n");
    printf("  ✓ Pattern: %s\n", omni_block_pattern_name(doc->expected_pattern));
    printf("  ✓ Nodes: %d\n", doc->node_count);
    printf("  ✓ Key: %s\n", doc->meta_key[0] ? doc->meta_key : "(none)");
    printf("\n");

    printf("───────────────────────────────────────────────────────────────────────────────\n");
    printf("  Phase 2: Generating C Code\n");
    printf("───────────────────────────────────────────────────────────────────────────────\n\n");

    // Initialize code generator
    OmniCodeGen gen;
    if (output_file) {
        if (!omni_codegen_init_file(&gen, output_file)) {
            fprintf(stderr, "Error: Could not open %s for writing\n", output_file);
            omni_document_destroy(doc);
            free(source);
            return 1;
        }
    } else {
        omni_codegen_init(&gen, stdout);
    }

    // Configure
    omni_codegen_set_comments(&gen, true);
    omni_codegen_set_debug(&gen, false);

    printf("  Generating...\n\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("  GENERATED C CODE\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

    // Generate
    OmniCodeGenResult result = omni_codegen_generate(&gen, doc);

    printf("\n═══════════════════════════════════════════════════════════════════════════════\n");
    printf("  END GENERATED CODE\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

    printf("───────────────────────────────────────────────────────────────────────────────\n");
    printf("  Results\n");
    printf("───────────────────────────────────────────────────────────────────────────────\n\n");

    printf("  Result:     %s\n", omni_codegen_result_name(result));
    printf("  Health:     %d (%s)\n", omni_codegen_health_get(&gen),
           omni_codegen_health_state(&gen));
    printf("  Lines:      %d\n", omni_codegen_lines(&gen));
    printf("  Functions:  %d\n", omni_codegen_functions(&gen));
    printf("  Errors:     %d\n", omni_codegen_error_count(&gen));
    printf("  Warnings:   %d\n", omni_codegen_warning_count(&gen));
    printf("\n");

    if (omni_codegen_error_count(&gen) > 0) {
        printf("  Errors:\n");
        omni_codegen_print_errors(&gen);
    }

    // Cleanup
    omni_codegen_destroy(&gen);
    omni_document_destroy(doc);
    free(source);

    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    if (result == CODEGEN_GRANTED) {
        printf("  ✓ Code generation successful!\n");
    } else if (result == CODEGEN_DEFERRED) {
        printf("  ◆ Code generation partial (some warnings)\n");
    } else {
        printf("  ✗ Code generation failed\n");
    }
    printf("═══════════════════════════════════════════════════════════════════════════════\n");

    return (result == CODEGEN_DENIED) ? 1 : 0;
}
