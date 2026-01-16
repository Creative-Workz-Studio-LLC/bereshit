// #!omni code --c -demo
// #!omni meta.key = B-tov-demo-phase-4-demo-omni-lexer
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// demo-omni-lexer — Demonstrates OmniCode lexer tokenization
// Tokenizes an OmniCode file and displays all tokens

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-tov-demo-phase-4-demo-omni-lexer
// Title: OmniCode Lexer Demo
// Type: Demo
// Role: Validates OmniCode lexer by tokenizing real .omni files

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "omni_lexer.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// Read file into string
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
    printf("=== Demo OmniCode Lexer (Phase 4) ===\n");
    printf("Tokenizing OmniCode files\n\n");

    // Default file to tokenize
    const char* file_path = "void/root.omni";
    if (argc > 1) {
        file_path = argv[1];
    }

    printf("File: %s\n\n", file_path);

    // Read file
    size_t size;
    char* source = read_file(file_path, &size);
    if (!source) {
        return 1;
    }

    // Initialize lexer
    OmniLexer lexer;
    omni_lexer_init(&lexer, source, size);

    // Token statistics
    int token_counts[TOK_COUNT_TYPES] = {0};
    int total_tokens = 0;
    int errors = 0;

    // Tokenize and display
    printf("%-4s %-4s %-20s %s\n", "LINE", "COL", "TYPE", "TEXT");
    printf("%-4s %-4s %-20s %s\n", "----", "---", "--------------------", "----");

    OmniToken tok;
    while ((tok = omni_lexer_next(&lexer)).type != TOK_EOF) {
        // Skip newlines and comments for cleaner output (but count them)
        if (tok.type == TOK_NEWLINE || tok.type == TOK_COMMENT) {
            token_counts[tok.type]++;
            total_tokens++;
            continue;
        }

        // Display token (truncate long strings)
        char display_text[50];
        if (strlen(tok.text) > 45) {
            strncpy(display_text, tok.text, 42);
            display_text[42] = '\0';
            strcat(display_text, "...");
        } else {
            strcpy(display_text, tok.text);
        }

        printf("%-4d %-4d %-20s %s\n",
               tok.line, tok.column,
               omni_token_type_name(tok.type),
               display_text);

        token_counts[tok.type]++;
        total_tokens++;

        if (tok.type == TOK_ERROR) {
            errors++;
        }
    }

    // Summary
    printf("\n");
    printf("================================================================\n");
    printf("Summary\n");
    printf("================================================================\n");
    printf("Total tokens:    %d\n", total_tokens);
    printf("Errors:          %d\n", errors);
    printf("Newlines:        %d\n", token_counts[TOK_NEWLINE]);
    printf("Comments:        %d\n", token_counts[TOK_COMMENT]);
    printf("Keywords:        %d\n",
           token_counts[TOK_THIS] + token_counts[TOK_FOR] + token_counts[TOK_IS] +
           token_counts[TOK_KEYED] + token_counts[TOK_AS] + token_counts[TOK_FOLLOWING] +
           token_counts[TOK_GROUNDED] + token_counts[TOK_IN] + token_counts[TOK_BE] +
           token_counts[TOK_OF] + token_counts[TOK_BECAUSE] + token_counts[TOK_WHEN]);
    printf("Strings:         %d\n", token_counts[TOK_STRING]);
    printf("Numbers:         %d\n", token_counts[TOK_NUMBER]);
    printf("Identifiers:     %d\n", token_counts[TOK_IDENTIFIER]);
    printf("Pragmas:         %d\n",
           token_counts[TOK_PRAGMA] + token_counts[TOK_META_KEY] +
           token_counts[TOK_META_FROM] + token_counts[TOK_META_AT]);

    printf("\n");
    if (errors == 0) {
        printf("Lexer demonstration complete - NO ERRORS\n");
    } else {
        printf("Lexer demonstration complete - %d ERRORS\n", errors);
    }

    printf("\n\"Precept upon precept; line upon line\"\n");
    printf("  — Isaiah 28:10\n");

    free(source);
    return errors > 0 ? 1 : 0;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage [USAGE]
//
// make run                     # Tokenizes void/root.omni
// make run FILE=tov/root.omni  # Tokenizes specified file

// =============================================================================
// END CLOSING
// =============================================================================
