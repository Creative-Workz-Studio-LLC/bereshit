// #!omni code --c -executable
// #!omni meta.key = B-tov-demo-phase-3-demo-omni-compiler-main
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// main.c — OmniCode Compiler Demo Entry Point
// Demonstrates complete OmniCode → C → executable pipeline.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-tov-demo-phase-3-demo-omni-compiler-main
// Title: OmniCode Compiler Demo
// Type: Executable
// Role: Validate complete compilation pipeline

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:3 - "Let there be light: and there was light"
// Principle: Words that CREATE - OmniCode sentences become executables
// Anchor: John 1:1 - "In the beginning was the Word"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "omni_lexer.h"
#include "omni_chunks.h"
#include "omni_parser.h"
#include "omni_codegen.h"
#include "trit.h"

// Demo configuration
#define DEMO_NAME "OmniCode Compiler Demo"
#define DEMO_INPUT "../hello.omni"
#define DEMO_OUTPUT "hello_generated.c"
#define DEMO_BINARY "hello_omni"
#define DEMO_EXPECTED "[INFO] Access granted\n"

// Health tracking (ternary system)
static int health = 0;

static void health_adjust(int delta, const char* reason) {
    health += delta;
    if (health > 100) health = 100;
    if (health < -100) health = -100;
    printf("  [HEALTH %+d] %s (now %d)\n", delta, reason, health);
}

static const char* health_state(void) {
    if (health > 50) return "GRANTED";
    if (health < -50) return "DENIED";
    return "DEFERRED";
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

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

static bool phase_parse(const char* source, OmniDocument** doc_out) {
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Phase 1: Parsing OmniCode\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    // Initialize chunk lexer
    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, strlen(source));

    // Initialize parser
    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    // Parse
    OmniDocument* doc = omni_parser_parse(&parser);
    if (!doc) {
        printf("  ✗ Parse failed\n");
        health_adjust(-30, "parse failed");
        return false;
    }

    printf("  ✓ Parsed successfully\n");
    printf("  ✓ Pattern: %s\n", omni_block_pattern_name(doc->expected_pattern));
    printf("  ✓ Nodes: %d\n", doc->node_count);
    printf("\n");

    health_adjust(+15, "OmniCode parsed");
    *doc_out = doc;
    return true;
}

static bool phase_codegen(OmniDocument* doc, const char* output_path) {
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Phase 2: Generating C Code\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    // Initialize code generator with file output
    OmniCodeGen gen;
    if (!omni_codegen_init_file(&gen, output_path)) {
        printf("  ✗ Could not open %s for writing\n", output_path);
        health_adjust(-20, "codegen file open failed");
        return false;
    }

    // Configure
    omni_codegen_set_comments(&gen, true);
    omni_codegen_set_debug(&gen, false);

    // Generate
    OmniCodeGenResult result = omni_codegen_generate(&gen, doc);

    printf("  ✓ Generated: %s\n", output_path);
    printf("  ✓ Lines: %d\n", omni_codegen_lines(&gen));
    printf("  ✓ Functions: %d\n", omni_codegen_functions(&gen));
    printf("  ✓ Codegen health: %d (%s)\n",
           omni_codegen_health_get(&gen),
           omni_codegen_health_state(&gen));
    printf("\n");

    omni_codegen_destroy(&gen);

    if (result == CODEGEN_DENIED) {
        printf("  ✗ Code generation failed\n");
        health_adjust(-20, "codegen failed");
        return false;
    }

    health_adjust(+20, "C code generated");
    return true;
}

static bool phase_compile(const char* source_path, const char* binary_path) {
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Phase 3: Compiling Generated C\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    // Build gcc command
    // Assumes libtrit is at ../../../../word/work/pkg/trit/
    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
        "gcc -std=c99 -Wall -Wextra -Wno-unused-variable -o %s %s "
        "-I../../../../word/work/pkg/trit/include "
        "-L../../../../word/work/pkg/trit/build -ltrit 2>&1",
        binary_path, source_path);

    printf("  Compiling: gcc %s → %s\n", source_path, binary_path);

    int ret = system(cmd);
    if (WIFEXITED(ret) && WEXITSTATUS(ret) != 0) {
        printf("  ✗ Compilation failed (exit %d)\n", WEXITSTATUS(ret));
        health_adjust(-25, "gcc compilation failed");
        return false;
    }

    printf("  ✓ Compiled: %s\n\n", binary_path);
    health_adjust(+25, "gcc compilation succeeded");
    return true;
}

static bool phase_execute(const char* binary_path, const char* expected) {
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Phase 4: Executing Compiled Program\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    // Build command with output capture
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "./%s", binary_path);

    printf("  Running: %s\n", cmd);

    // Open pipe to capture output
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        printf("  ✗ Could not execute %s\n", binary_path);
        health_adjust(-30, "execution failed");
        return false;
    }

    // Read output
    char output[1024] = {0};
    size_t total = 0;
    size_t bytes;
    while ((bytes = fread(output + total, 1, sizeof(output) - total - 1, pipe)) > 0) {
        total += bytes;
    }
    output[total] = '\0';

    int status = pclose(pipe);
    int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    printf("  Output: %s", output);
    printf("  Exit code: %d\n\n", exit_code);

    // Verify output
    if (strcmp(output, expected) != 0) {
        printf("  ✗ Output mismatch!\n");
        printf("    Expected: %s", expected);
        printf("    Got:      %s\n", output);
        health_adjust(-30, "output mismatch");
        return false;
    }

    printf("  ✓ Output matches expected!\n\n");
    health_adjust(+30, "execution output correct");
    return true;
}

int main(int argc, char** argv) {
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  %s\n", DEMO_NAME);
    printf("  \"Let there be light: and there was light\" — Genesis 1:3\n");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    // Get paths
    const char* input_file = (argc > 1) ? argv[1] : DEMO_INPUT;
    const char* output_file = (argc > 2) ? argv[2] : DEMO_OUTPUT;
    const char* binary_file = (argc > 3) ? argv[3] : DEMO_BINARY;

    printf("  Input:  %s (OmniCode)\n", input_file);
    printf("  Output: %s (Generated C)\n", output_file);
    printf("  Binary: %s (Executable)\n\n", binary_file);

    health_adjust(+10, "demo initialized");

    // Read source
    char* source = read_file(input_file);
    if (!source) {
        printf("  ✗ Could not read %s\n", input_file);
        health_adjust(-50, "input file not found");
        goto done;
    }

    // Phase 1: Parse OmniCode
    OmniDocument* doc = NULL;
    if (!phase_parse(source, &doc)) {
        free(source);
        goto done;
    }

    // Phase 2: Generate C code
    if (!phase_codegen(doc, output_file)) {
        omni_document_destroy(doc);
        free(source);
        goto done;
    }

    // Phase 3: Compile C code
    if (!phase_compile(output_file, binary_file)) {
        omni_document_destroy(doc);
        free(source);
        goto done;
    }

    // Phase 4: Execute and verify
    if (!phase_execute(binary_file, DEMO_EXPECTED)) {
        omni_document_destroy(doc);
        free(source);
        goto done;
    }

    // Cleanup
    omni_document_destroy(doc);
    free(source);

done:
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  Pipeline Results\n");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    printf("  Final Health: %d (%s)\n\n", health, health_state());

    // Ternary result interpretation
    if (health > 50) {
        printf("  ✓ GRANTED: OmniCode → C → Executable pipeline working!\n");
        printf("    Words that CREATE — \"Let there be light.\"\n");
    } else if (health < -50) {
        printf("  ✗ DENIED: Pipeline broken — needs repair.\n");
    } else {
        printf("  ◆ DEFERRED: Pipeline partially functional.\n");
    }

    printf("\n═══════════════════════════════════════════════════════════════════\n");

    // Return ternary-based exit code
    return (health > 50) ? 0 : (health < -50) ? 1 : 2;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// Validation:
//   make && ./bin/demo-omni-compiler
//   Expected: "[INFO] Access granted" from compiled OmniCode
//
// Pipeline:
//   hello.omni → libomni parser → omni_codegen → hello_generated.c
//   hello_generated.c → gcc + libtrit → hello_omni
//   hello_omni → "[INFO] Access granted"
//
// "Let there be light: and there was light." — Genesis 1:3
//
// Words that CREATE. OmniCode sentences become executable code.
// The covenant type (granted/deferred/denied) maps to libtrit's trit_t.
// This is the foundation of executable scripture.

// =============================================================================
// END CLOSING
// =============================================================================
