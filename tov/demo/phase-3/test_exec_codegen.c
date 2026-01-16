// #!omni code --c -test
// Test codegen for executable OmniCode constructs
// "Let there be light: and there was light" — Genesis 1:3

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

    size_t bytes_read = fread(buf, 1, size, f);
    buf[bytes_read] = '\0';
    fclose(f);

    return buf;
}

int main(int argc, char** argv) {
    const char* input_file = (argc > 1) ? argv[1] : "test_exec.omni";
    const char* output_file = (argc > 2) ? argv[2] : NULL;

    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  OmniCode → C Code Generator Test\n");
    printf("  \"Let there be light: and there was light\" — Genesis 1:3\n");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    printf("Input: %s\n", input_file);
    if (output_file) {
        printf("Output: %s\n", output_file);
    }
    printf("\n");

    // Read source
    char* source = read_file(input_file);
    if (!source) {
        printf("✗ Could not read %s\n", input_file);
        return 1;
    }

    // Initialize chunk lexer
    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    // Initialize parser
    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    // Parse
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Parsing...\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    OmniDocument* doc = omni_parser_parse(&parser);
    if (!doc) {
        printf("✗ Parse failed\n");
        free(source);
        return 1;
    }

    printf("  ✓ Parsed: %d nodes\n\n", doc->node_count);

    // Initialize codegen
    OmniCodeGen gen;
    FILE* out_fp = NULL;

    if (output_file) {
        out_fp = fopen(output_file, "w");
        if (!out_fp) {
            printf("✗ Could not open %s for writing\n", output_file);
            omni_document_destroy(doc);
            free(source);
            return 1;
        }
        omni_codegen_init(&gen, out_fp);
    } else {
        omni_codegen_init(&gen, stdout);
    }

    // Generate
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Generating C code...\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    OmniCodeGenResult result = omni_codegen_generate(&gen, doc);

    printf("\n───────────────────────────────────────────────────────────────────\n");
    printf("  Generation Result: %s\n", omni_codegen_result_name(result));
    printf("  Lines emitted: %d\n", omni_codegen_lines(&gen));
    printf("  Functions emitted: %d\n", omni_codegen_functions(&gen));
    printf("  Health: %d (%s)\n", omni_codegen_health_get(&gen),
           omni_codegen_health_state(&gen));
    printf("───────────────────────────────────────────────────────────────────\n");

    if (omni_codegen_error_count(&gen) > 0) {
        printf("\nErrors:\n");
        omni_codegen_print_errors(&gen);
    }

    printf("\n═══════════════════════════════════════════════════════════════════\n");
    if (result == CODEGEN_GRANTED) {
        printf("  ✓ Code generation complete!\n");
    } else {
        printf("  ⚠ Code generation completed with issues\n");
    }
    printf("═══════════════════════════════════════════════════════════════════\n");

    // Cleanup
    omni_codegen_destroy(&gen);
    if (out_fp) fclose(out_fp);
    omni_document_destroy(doc);
    free(source);

    return (result == CODEGEN_DENIED) ? 1 : 0;
}
