// #!omni code --c -test
// Test parsing of OmniCode stdlib libraries
// "In the beginning was the Word" — John 1:1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "omni_chunks.h"
#include "omni_parser.h"

static char* read_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = malloc(size + 1);
    if (!buf) { fclose(f); return NULL; }

    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}

static int parse_omni_file(const char* path, int verbose) {
    char* source = read_file(path);
    if (!source) {
        printf("   ✗ Failed to read: %s\n", path);
        return -1;
    }

    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    OmniDocument* doc = omni_parser_parse(&parser);
    if (!doc) {
        printf("   ✗ Parse failed: %s\n", path);
        free(source);
        return -1;
    }

    if (verbose) {
        printf("   ✓ %s: %d nodes\n", path, doc->node_count);
    }

    int nodes = doc->node_count;
    omni_document_destroy(doc);
    free(source);
    return nodes;
}

int main(int argc, char** argv) {
    const char* stdlib_path = argc > 1 ? argv[1] :
        "/media/seanje-lenox-wise/Project/Bereshit/word/work/pkg/omni/stdlib";

    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  OmniCode Standard Library Parse Test\n");
    printf("  \"In the beginning was the Word\" — John 1:1\n");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    const char* modules[] = {
        "root.omni",
        "covenant/covenant.omni",
        "trit/trit.omni",
        "health/health.omni",
        "math/math.omni",
        "io/io.omni",
        "string/string.omni",
        "state/state.omni",
        "collections/collections.omni",
        "test/test.omni",
        NULL
    };

    int total_files = 0;
    int total_nodes = 0;
    int success = 0;
    int failed = 0;

    printf("Parsing stdlib modules from: %s\n\n", stdlib_path);

    for (int i = 0; modules[i] != NULL; i++) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", stdlib_path, modules[i]);

        int nodes = parse_omni_file(full_path, 1);
        total_files++;

        if (nodes >= 0) {
            total_nodes += nodes;
            success++;
        } else {
            failed++;
        }
    }

    printf("\n───────────────────────────────────────────────────────────────────\n");
    printf("  Results:\n");
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Files parsed:  %d\n", total_files);
    printf("  Successful:    %d\n", success);
    printf("  Failed:        %d\n", failed);
    printf("  Total nodes:   %d\n", total_nodes);
    printf("───────────────────────────────────────────────────────────────────\n");

    if (failed == 0) {
        printf("\n═══════════════════════════════════════════════════════════════════\n");
        printf("  ✓ All stdlib modules parsed successfully!\n");
        printf("═══════════════════════════════════════════════════════════════════\n");
        return 0;
    } else {
        printf("\n═══════════════════════════════════════════════════════════════════\n");
        printf("  ✗ Some modules failed to parse\n");
        printf("═══════════════════════════════════════════════════════════════════\n");
        return 1;
    }
}
