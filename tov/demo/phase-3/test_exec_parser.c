// #!omni code --c -test
// Test parser for executable OmniCode constructs
// "Let there be light: and there was light" — Genesis 1:3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "omni_lexer.h"
#include "omni_chunks.h"
#include "omni_parser.h"

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

    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  OmniCode Executable Parser Test\n");
    printf("  \"Let there be light: and there was light\" — Genesis 1:3\n");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    printf("Input: %s\n\n", input_file);

    // Read source
    char* source = read_file(input_file);
    if (!source) {
        printf("✗ Could not read %s\n", input_file);
        return 1;
    }

    // Initialize chunk lexer
    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    // First, show chunks
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Chunks:\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    // Reinit for fresh pass
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    OmniChunk chunk;
    int chunk_count = 0;
    while ((chunk = omni_chunk_next(&chunk_lexer)).type != CHUNK_EOF) {
        if (chunk.type == CHUNK_NEWLINE || chunk.type == CHUNK_COMMENT) continue;

        printf("  [%s]", omni_chunk_type_name(chunk.type));
        if (chunk.primary_value[0]) {
            printf(" \"%s\"", chunk.primary_value);
        }
        if (chunk.secondary_value[0]) {
            printf(" (type: %s)", chunk.secondary_value);
        }
        if (chunk.tertiary_value[0]) {
            printf(" (init: %s)", chunk.tertiary_value);
        }
        printf(" @%d:%d\n", chunk.line, chunk.column);
        chunk_count++;
    }
    printf("\n  Total: %d chunks\n\n", chunk_count);

    // Reinit for parsing
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

    printf("  ✓ Parsed successfully\n");
    printf("  ✓ Pattern: %s\n", omni_block_pattern_name(doc->expected_pattern));
    printf("  ✓ Nodes: %d\n\n", doc->node_count);

    // Print AST
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  AST Tree:\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    omni_ast_print(doc);

    // Count specific node types
    int let_count = 0, func_count = 0, when_count = 0, witness_count = 0;

    void count_visitor(OmniAstNode* node, int depth, void* ctx) {
        (void)depth; (void)ctx;
        switch (node->type) {
            case AST_LET_STMT: let_count++; break;
            case AST_FUNC_DEF: func_count++; break;
            case AST_WHEN_STMT: when_count++; break;
            case AST_WITNESS_STMT: witness_count++; break;
            default: break;
        }
    }

    omni_ast_walk(doc->root, count_visitor, NULL);

    printf("\n───────────────────────────────────────────────────────────────────\n");
    printf("  Node Type Counts:\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");
    printf("  LET_STMT:     %d\n", let_count);
    printf("  FUNC_DEF:     %d\n", func_count);
    printf("  WHEN_STMT:    %d\n", when_count);
    printf("  WITNESS_STMT: %d\n", witness_count);

    printf("\n═══════════════════════════════════════════════════════════════════\n");
    printf("  ✓ Executable code parsing complete!\n");
    printf("═══════════════════════════════════════════════════════════════════\n");

    // Cleanup
    omni_document_destroy(doc);
    free(source);

    return 0;
}
