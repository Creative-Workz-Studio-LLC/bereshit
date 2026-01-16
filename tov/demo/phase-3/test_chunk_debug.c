// Debug test for chunk parsing
#include <stdio.h>
#include <string.h>
#include "../../../word/work/pkg/omni/include/omni_chunks.h"
#include "../../../word/work/pkg/omni/include/omni_parser.h"

int main(void) {
    printf("=== Chunk Parsing Debug ===\n\n");

    const char* source =
        "#!omni document --omni -executable\n"
        "PROGRAM \"test\"\n"
        "// BODY [BODY]\n"
        "health add 25\n"
        "witness \"test\"\n"
        "// END BODY\n";

    printf("Source:\n%s\n", source);

    // Parse chunks
    printf("Chunks (showing first 30, skipping newlines):\n");
    OmniChunkLexer lexer;
    omni_chunk_init(&lexer, source, strlen(source));

    OmniChunk chunk;
    int count = 0;
    while ((chunk = omni_chunk_next(&lexer)).type != CHUNK_EOF && count < 30) {
        if (chunk.type == CHUNK_NEWLINE) continue;  // Skip newlines
        printf("  [%2d] type=%3d  primary='%s' secondary='%s'\n",
               count++, chunk.type, chunk.primary_value, chunk.secondary_value);

        // Check specifically for HEALTH
        if (chunk.type == CHUNK_HEALTH) {
            printf("       *** FOUND CHUNK_HEALTH (type=%d) ***\n", CHUNK_HEALTH);
        }
    }

    printf("\nExpected CHUNK_HEALTH type value: %d\n", CHUNK_HEALTH);
    printf("Expected CHUNK_WITNESS type value: %d\n", CHUNK_WITNESS);

    return 0;
}
