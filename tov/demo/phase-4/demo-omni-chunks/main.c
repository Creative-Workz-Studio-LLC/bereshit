// #!omni code --c -demo
// #!omni meta.key = B-tov-demo-phase-4-demo-omni-chunks
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// demo-omni-chunks — Demonstrates semantic chunk lexer
// Shows how the lexer now UNDERSTANDS, not just tokenizes

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-tov-demo-phase-4-demo-omni-chunks
// Title: OmniCode Chunk Lexer Demo
// Type: Demo
// Role: Show intelligent chunking of OmniCode

// # M.8 Grounding [GROUNDING]
//
// Scripture: John 1:1 - In the beginning was the Word
// Principle: Words carry meaning - chunking IS understanding

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "omni_chunks.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

static char* read_file(const char* path, size_t* out_size) {
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Cannot open file: %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t read = fread(buf, 1, size, f);
    buf[read] = '\0';
    fclose(f);

    if (out_size) *out_size = read;
    return buf;
}

int main(int argc, char** argv) {
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  OmniCode Semantic Chunk Lexer Demo\n");
    printf("  \"In the beginning was the Word\" — John 1:1\n");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    // Default file
    const char* file_path = "word/work/pkg/omni/lang/test/minimal.omni";
    if (argc > 1) {
        file_path = argv[1];
    }

    printf("File: %s\n\n", file_path);

    // Read file
    size_t size;
    char* source = read_file(file_path, &size);
    if (!source) return 1;

    // Initialize chunk lexer
    OmniChunkLexer lexer;
    omni_chunk_init(&lexer, source, size);

    printf("───────────────────────────────────────────────────────────────────\n");
    printf("SEMANTIC CHUNKS (the lexer now understands)\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    int chunk_count = 0;
    int meaningful_count = 0;

    OmniChunk chunk;
    while ((chunk = omni_chunk_next(&lexer)).type != CHUNK_EOF) {
        chunk_count++;

        // Skip newlines and comments for cleaner output
        if (chunk.type == CHUNK_NEWLINE) continue;
        if (chunk.type == CHUNK_COMMENT) continue;
        if (chunk.type == CHUNK_RAW_TOKEN) continue;

        meaningful_count++;

        // Show chunk type and value
        printf("[%s]", omni_chunk_type_name(chunk.type));

        // Show primary value if present
        if (chunk.primary_value[0]) {
            printf(" → \"%s\"", chunk.primary_value);
        }

        // Show secondary value if present
        if (chunk.secondary_value[0]) {
            printf(" (%s)", chunk.secondary_value);
        }

        // Show scripture details if present
        if (chunk.book[0]) {
            printf(" [%s %s:%s]", chunk.book, chunk.chapter, chunk.verse);
        }

        printf("\n");
    }

    printf("\n───────────────────────────────────────────────────────────────────\n");
    printf("Summary: %d total chunks, %d meaningful\n", chunk_count, meaningful_count);
    printf("───────────────────────────────────────────────────────────────────\n\n");

    printf("The lexer now UNDERSTANDS OmniCode:\n");
    printf("  • \"this is a folder for\" → ENTITY_FOLDER chunk\n");
    printf("  • \"keyed as\"             → IDENTITY_KEY chunk\n");
    printf("  • \"scripture ... from\"   → GROUND_SCRIPTURE chunk with parsed ref\n");
    printf("\n");
    printf("This is what \"OmniCode IS English\" means.\n");
    printf("Words carry meaning. The lexer should understand, not just tokenize.\n");

    free(source);
    return 0;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage [USAGE]
//
// make run                              # Parse minimal.omni
// make run FILE=word/root.omni          # Parse word root

// =============================================================================
// END CLOSING
// =============================================================================
