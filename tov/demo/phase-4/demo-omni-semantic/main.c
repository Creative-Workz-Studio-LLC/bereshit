// #!omni code --c -demo
// #!omni meta.key = B-tov-demo-phase-4-demo-omni-semantic
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// demo-omni-semantic — Demonstrates semantic understanding of OmniCode
// Queries an OmniCode file like asking questions in English

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-tov-demo-phase-4-demo-omni-semantic
// Title: OmniCode Semantic Parser Demo
// Type: Demo
// Role: Show intelligent understanding of OmniCode

// # M.8 Grounding [GROUNDING]
//
// Scripture: John 1:1 - In the beginning was the Word
// Principle: OmniCode IS English - ask it questions naturally

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "omni_semantic.h"

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
    printf("=== OmniCode Semantic Parser Demo ===\n");
    printf("Understanding OmniCode as natural language\n\n");

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

    // Parse semantically
    OmniSemanticParser parser;
    omni_semantic_init(&parser, source, size);
    OmniSemanticDoc* doc = omni_semantic_parse(&parser);

    if (!doc) {
        fprintf(stderr, "Failed to parse document\n");
        free(source);
        return 1;
    }

    // Ask questions about the document - natural language queries
    printf("════════════════════════════════════════════════════════════════\n");
    printf("SEMANTIC UNDERSTANDING\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    // What is this?
    printf("Q: What is this?\n");
    printf("A: This is a %s called \"%s\"\n\n",
           omni_semantic_entity_type(doc),
           omni_semantic_entity_name(doc));

    // What is its identity?
    printf("Q: What is its identity?\n");
    printf("A: Key: %s\n\n", omni_semantic_entity_key(doc));

    // Where does it belong?
    printf("Q: Where does it belong?\n");
    const char* zone = omni_semantic_zone(doc);
    if (zone[0]) {
        printf("A: Zone: %s (%d)\n", zone, omni_semantic_zone_value(doc));
        if (doc->zone_reason[0]) {
            printf("   Because: \"%s\"\n", doc->zone_reason);
        }
    } else {
        printf("A: (not specified)\n");
    }
    printf("\n");

    // Where does it come from?
    printf("Q: Where does it come from?\n");
    if (doc->derives_from[0]) {
        printf("A: Derives from: %s\n", doc->derives_from);
        if (doc->derives_as[0]) {
            printf("   As: \"%s\"\n", doc->derives_as);
        }
    } else {
        printf("A: (not specified)\n");
    }
    printf("\n");

    // What is it grounded in?
    printf("Q: What is it grounded in?\n");
    const char* scripture = omni_semantic_scripture(doc);
    if (scripture[0]) {
        printf("A: Scripture: %s\n", scripture);
    }
    const char* principle = omni_semantic_principle(doc);
    if (principle[0]) {
        printf("   Principle: \"%s\"\n", principle);
    }
    const char* anchor = omni_semantic_anchor(doc);
    if (anchor[0]) {
        printf("   Anchor: \"%s\"\n", anchor);
    }
    if (!scripture[0] && !principle[0] && !anchor[0]) {
        printf("A: (not specified)\n");
    }
    printf("\n");

    // What does it serve as?
    printf("Q: What does it serve as?\n");
    const char* role = omni_semantic_role(doc);
    if (role[0]) {
        printf("A: Role: \"%s\"\n", role);
    } else {
        printf("A: (not specified)\n");
    }
    printf("\n");

    // What is its purpose?
    printf("Q: What is its purpose?\n");
    const char* purpose = omni_semantic_purpose(doc);
    if (purpose[0]) {
        printf("A: Purpose: \"%s\"\n", purpose);
    } else {
        printf("A: (not specified)\n");
    }
    printf("\n");

    // Who created it?
    printf("Q: Who created it?\n");
    if (doc->architect[0]) {
        printf("A: Architect: %s\n", doc->architect);
    }
    if (doc->implementation[0]) {
        printf("   Implementation: %s\n", doc->implementation);
    }
    if (doc->created[0]) {
        printf("   Created: %s\n", doc->created);
    }
    if (doc->version[0]) {
        printf("   Version: %s\n", doc->version);
    }
    if (!doc->architect[0] && !doc->implementation[0]) {
        printf("A: (not specified)\n");
    }
    printf("\n");

    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("\"In the beginning was the Word\"\n");
    printf("  — John 1:1\n\n");
    printf("OmniCode IS English. Understanding precedes parsing.\n");

    // Cleanup
    omni_semantic_free(doc);
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
// make run FILE=tov/data/db/root.omni   # Parse database root

// =============================================================================
// END CLOSING
// =============================================================================
