// #!omni code --c -executable
// #!omni meta.key = B-tov-demo-phase2-omni-parser
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// demo-omni-parser: OmniCode Parser Validation Demo
// Demonstrates parsing .omni files and detecting block structure

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-tov-demo-phase2-omni-parser
// Title: OmniCode Parser Demo
// Type: Executable Demo
// Phase: 2 (OmniCode Parser)
//
// Grounding: "Precept upon precept; line upon line" — Isaiah 28:10

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omni_parser.h"
#include "omni_chunks.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

static char* read_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        perror(path);
        return NULL;
    }
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

static const char* pattern_name(OmniBlockPattern pattern) {
    switch (pattern) {
        case OMNI_PATTERN_3BLOCK: return "3-Block (Data)";
        case OMNI_PATTERN_4BLOCK: return "4-Block (Code)";
        case OMNI_PATTERN_5BLOCK: return "5-Block (Documentation)";
        default: return "Unknown";
    }
}

static void show_usage(const char* prog) {
    fprintf(stderr, "Usage: %s <file.omni>\n", prog);
    fprintf(stderr, "\nOmniCode Parser Demo - Phase 2 Validation\n");
    fprintf(stderr, "Parses .omni files and displays structure analysis.\n");
}

// Walk AST and print block nodes
static void print_blocks_visitor(OmniAstNode* node, int depth, void* ctx) {
    (void)ctx;
    if (node->type == AST_BLOCK) {
        const char* action = node->secondary;  // "START" or "END"
        const char* block_name = node->value;

        // Indent based on depth
        for (int i = 0; i < depth; i++) printf("  ");

        if (strcmp(action, "START") == 0) {
            printf("┌── [%s] at line %d\n", block_name, node->line);
        } else {
            printf("└── [END %s] at line %d\n", block_name, node->line);
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        show_usage(argv[0]);
        return 1;
    }

    const char* path = argv[1];
    char* source = read_file(path);
    if (!source) {
        return 1;
    }

    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║          OmniCode Parser Demo - Phase 2 Validation               ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n\n");
    printf("File: %s\n", path);
    printf("Size: %zu bytes\n\n", strlen(source));

    // Step 1: Initialize chunk lexer
    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    // Step 2: Initialize parser
    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    // Step 3: Parse the document
    OmniDocument* doc = omni_parser_parse(&parser);
    if (!doc) {
        fprintf(stderr, "ERROR: Failed to parse document\n");
        free(source);
        return 1;
    }

    // Display parsed information
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ Document Metadata                                                │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ Key:     %-54s │\n", doc->meta_key[0] ? doc->meta_key : "(none)");
    printf("│ Type:    %-54s │\n", doc->pragma_type[0] ? doc->pragma_type : "(none)");
    printf("│ Pattern: %-54s │\n", pattern_name(doc->expected_pattern));
    printf("└─────────────────────────────────────────────────────────────────┘\n");

    // Show detected block pattern
    printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ Block Structure                                                  │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ Total AST nodes:  %-46d │\n", doc->node_count);
    printf("│ Has METADATA:     %-46s │\n", doc->has_metadata ? "✓ Yes" : "✗ No");

    // Show pattern-specific blocks
    switch (doc->expected_pattern) {
        case OMNI_PATTERN_3BLOCK:
            printf("│ Has CONTENT:      %-46s │\n", doc->has_content ? "✓ Yes" : "✗ No");
            printf("│ Has CLOSING:      %-46s │\n", doc->has_closing ? "✓ Yes" : "✗ No");
            break;
        case OMNI_PATTERN_4BLOCK:
            printf("│ Has SETUP:        %-46s │\n", doc->has_setup ? "✓ Yes" : "✗ No");
            printf("│ Has BODY:         %-46s │\n", doc->has_body ? "✓ Yes" : "✗ No");
            printf("│ Has CLOSING:      %-46s │\n", doc->has_closing ? "✓ Yes" : "✗ No");
            break;
        case OMNI_PATTERN_5BLOCK:
            printf("│ Has HEADER:       %-46s │\n", doc->has_header ? "✓ Yes" : "✗ No");
            printf("│ Has CONTEXT:      %-46s │\n", doc->has_context ? "✓ Yes" : "✗ No");
            printf("│ Has CONTENT:      %-46s │\n", doc->has_content ? "✓ Yes" : "✗ No");
            printf("│ Has FOOTER:       %-46s │\n", doc->has_footer ? "✓ Yes" : "✗ No");
            break;
        default:
            break;
    }
    printf("└─────────────────────────────────────────────────────────────────┘\n");

    // Walk AST to show block structure
    printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ AST Block Structure                                              │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");

    if (doc->root) {
        omni_ast_walk(doc->root, print_blocks_visitor, NULL);
    } else {
        printf("│ (No AST root)                                                   │\n");
    }
    printf("└─────────────────────────────────────────────────────────────────┘\n");

    // Verify pattern completeness
    printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ Pattern Verification                                             │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");

    bool complete = omni_document_is_complete(doc);
    printf("│ Pattern complete: %-46s │\n", complete ? "✓ Yes" : "✗ No");

    if (doc->error_count > 0) {
        printf("│ Errors found:     %-46d │\n", doc->error_count);
    }

    printf("│                                                                  │\n");
    printf("│ Overall: %-54s │\n", complete ? "✓ VALID" : "✗ ISSUES DETECTED");
    printf("└─────────────────────────────────────────────────────────────────┘\n");

    // Cleanup
    omni_document_destroy(doc);
    free(source);

    printf("\n\"Precept upon precept; line upon line\" — Isaiah 28:10\n");
    return complete ? 0 : 1;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Validation: make && make test-all
// Closing: The parser divides, the structure reveals meaning.

// =============================================================================
// END CLOSING
// =============================================================================
