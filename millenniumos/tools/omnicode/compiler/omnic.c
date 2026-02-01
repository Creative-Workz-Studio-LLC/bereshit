// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-TOOLS-OMNICODE-COMPILER
// Title:       OmniCode Compiler Driver (Seed Compiler)
// Type:        Source
// Component:   Tools / OmniCode Toolchain
// Role:        Compile OmniCode source to C or IR bytecode
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Purpose:     Seed compiler for OmniCode self-hosting bootstrap.
//              This C implementation compiles OmniCode to C or IR,
//              enabling the first stage of the bootstrap process.
//
// Grounding:   John 1:1 — "In the beginning was the Word"
//              The Word creates. This compiler makes OmniCode creative.
//
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
#include <time.h>

// OmniCode components
// Note: viewer.h is NOT included - the compiler doesn't need graphics.
// The viewer is a separate visualization tool, not a compilation dependency.
#include "../transpiler/omni_transpiler.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Compiler Options
// -----------------------------------------------------------------------------

typedef enum {
    OUTPUT_C,           // Generate C code
    OUTPUT_IR,          // Generate IR bytecode
    OUTPUT_EXEC         // Compile and execute (via C)
} OmnicOutputMode;

typedef struct {
    const char*     input_path;
    const char*     output_path;
    OmnicOutputMode mode;
    bool            verbose;
    bool            debug;
    bool            health;         // Include health tracking
    bool            verify;         // Verify output compiles
    bool            run;            // Run after compile
    const char*     cc;             // C compiler to use
    const char*     cflags;         // Extra C flags
} OmnicOptions;

static void options_init(OmnicOptions* opts) {
    opts->input_path = NULL;
    opts->output_path = NULL;
    opts->mode = OUTPUT_C;
    opts->verbose = false;
    opts->debug = false;
    opts->health = true;
    opts->verify = true;
    opts->run = false;
    opts->cc = "gcc";
    opts->cflags = "-O2";
}

// -----------------------------------------------------------------------------
// B.2 Usage and Help
// -----------------------------------------------------------------------------

static void print_version(void) {
    printf("omnic - OmniCode Compiler (Seed)\n");
    printf("Version: a-01.00\n");
    printf("\"In the beginning was the Word\" - John 1:1\n");
}

static void print_usage(const char* prog) {
    printf("\nUsage: %s [options] <input.omni>\n\n", prog);
    printf("Options:\n");
    printf("  -o <file>      Output file (default: based on input)\n");
    printf("  -c             Output C code (default)\n");
    printf("  -ir            Output IR bytecode\n");
    printf("  -r, --run      Compile and run immediately\n");
    printf("  -v, --verbose  Verbose output\n");
    printf("  -d, --debug    Include debug info\n");
    printf("  --no-health    Disable health tracking in output\n");
    printf("  --no-verify    Skip compile verification\n");
    printf("  --cc <cc>      C compiler (default: gcc)\n");
    printf("  --cflags <f>   Extra C compiler flags\n");
    printf("  -h, --help     Show this help\n");
    printf("  --version      Show version\n");
    printf("\nExamples:\n");
    printf("  %s hello.omni              # Compile to hello.c\n", prog);
    printf("  %s hello.omni -o out.c     # Compile to out.c\n", prog);
    printf("  %s hello.omni -r           # Compile and run\n", prog);
    printf("  %s hello.omni -ir          # Generate IR bytecode\n", prog);
}

// -----------------------------------------------------------------------------
// B.3 Argument Parsing
// -----------------------------------------------------------------------------

static int parse_args(int argc, char** argv, OmnicOptions* opts) {
    options_init(opts);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return -1;  // Signal to exit cleanly
        }
        if (strcmp(argv[i], "--version") == 0) {
            print_version();
            return -1;
        }
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            opts->output_path = argv[++i];
        }
        else if (strcmp(argv[i], "-c") == 0) {
            opts->mode = OUTPUT_C;
        }
        else if (strcmp(argv[i], "-ir") == 0) {
            opts->mode = OUTPUT_IR;
        }
        else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--run") == 0) {
            opts->run = true;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            opts->verbose = true;
        }
        else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            opts->debug = true;
        }
        else if (strcmp(argv[i], "--no-health") == 0) {
            opts->health = false;
        }
        else if (strcmp(argv[i], "--no-verify") == 0) {
            opts->verify = false;
        }
        else if (strcmp(argv[i], "--cc") == 0 && i + 1 < argc) {
            opts->cc = argv[++i];
        }
        else if (strcmp(argv[i], "--cflags") == 0 && i + 1 < argc) {
            opts->cflags = argv[++i];
        }
        else if (argv[i][0] == '-') {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
        else {
            // Input file
            if (opts->input_path != NULL) {
                fprintf(stderr, "Multiple input files not supported\n");
                return 1;
            }
            opts->input_path = argv[i];
        }
    }

    if (opts->input_path == NULL) {
        fprintf(stderr, "No input file specified\n");
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}

// -----------------------------------------------------------------------------
// B.4 Output Path Generation
// -----------------------------------------------------------------------------

static void get_output_path(const OmnicOptions* opts, char* out, size_t size) {
    if (opts->output_path) {
        strncpy(out, opts->output_path, size - 1);
        out[size - 1] = '\0';
        return;
    }

    // Generate from input path
    strncpy(out, opts->input_path, size - 1);
    out[size - 1] = '\0';

    // Replace extension
    char* ext = strrchr(out, '.');
    if (ext) {
        *ext = '\0';
    }

    const char* new_ext = (opts->mode == OUTPUT_C) ? ".c" :
                          (opts->mode == OUTPUT_IR) ? ".omir" : "";
    strncat(out, new_ext, size - strlen(out) - 1);
}

// -----------------------------------------------------------------------------
// B.5 Compilation Pipeline
// -----------------------------------------------------------------------------

static int compile_to_c(const OmnicOptions* opts) {
    if (opts->verbose) {
        printf("[omnic] Compiling %s to C...\n", opts->input_path);
    }

    // Use the transpiler
    OmniTransConfig cfg;
    OmniTransState state;

    omni_trans_config_init(&cfg);
    omni_trans_init(&state);

    cfg.use_libtrit = true;
    cfg.verify_compile = opts->verify;
    cfg.verbose = opts->verbose;

    // Parse
    if (opts->verbose) {
        printf("[omnic] Parsing...\n");
    }

    OmniTranspileError err = omni_trans_parse(&state, opts->input_path);
    if (err != OMNI_TRANS_OK) {
        fprintf(stderr, "Parse error: %s\n", omni_trans_error_str(err));
        omni_trans_free(&state);
        return 1;
    }

    if (opts->verbose && state.document) {
        printf("[omnic] Parsed: %d nodes, errors: %d\n",
               state.document->node_count, state.document->error_count);
    }

    // Validate
    if (opts->verbose) {
        printf("[omnic] Validating...\n");
    }

    err = omni_trans_validate_structure(state.document);
    if (err != OMNI_TRANS_OK) {
        fprintf(stderr, "Validation error: %s\n", omni_trans_error_str(err));
        omni_trans_free(&state);
        return 1;
    }

    // Generate
    if (opts->verbose) {
        printf("[omnic] Generating C code...\n");
    }

    err = omni_trans_generate(&state, &cfg);
    if (err != OMNI_TRANS_OK) {
        fprintf(stderr, "Generation error: %s\n", omni_trans_error_str(err));
        omni_trans_free(&state);
        return 1;
    }

    // Write output
    char output_path[512];
    get_output_path(opts, output_path, sizeof(output_path));

    // Extract base name (without extension)
    char base[512];
    strncpy(base, output_path, sizeof(base));
    char* ext = strrchr(base, '.');
    if (ext && (strcmp(ext, ".c") == 0 || strcmp(ext, ".h") == 0)) {
        *ext = '\0';
    }

    if (opts->verbose) {
        printf("[omnic] Writing to %s...\n", base);
    }

    err = omni_trans_write(&state, &cfg, base);
    if (err != OMNI_TRANS_OK) {
        fprintf(stderr, "Write error: %s\n", omni_trans_error_str(err));
        omni_trans_free(&state);
        return 1;
    }

    // Verify
    if (opts->verify) {
        if (opts->verbose) {
            printf("[omnic] Verifying compilation...\n");
        }

        err = omni_trans_verify(&state, &cfg);
        if (!state.compile_success) {
            fprintf(stderr, "Compilation verification failed:\n%s\n",
                    state.compile_errors ? state.compile_errors : "(unknown error)");
            // Don't fail - just warn
        } else if (opts->verbose) {
            printf("[omnic] Verification passed\n");
        }
    }

    // Summary
    printf("Compiled %s -> %s.c\n", opts->input_path, base);
    printf("  Types: %d, Functions: %d, Lines: %d\n",
           state.types_generated, state.functions_generated, state.lines_generated);

    omni_trans_free(&state);
    return 0;
}

static int compile_to_ir(const OmnicOptions* opts) {
    if (opts->verbose) {
        printf("[omnic] Compiling %s to IR...\n", opts->input_path);
    }

    // TODO: Implement IR generation
    // This would use omni_ir.c from pkg/omni

    fprintf(stderr, "IR output not yet implemented\n");
    return 1;
}

static int compile_and_run(const OmnicOptions* opts) {
    // First compile to C
    int result = compile_to_c(opts);
    if (result != 0) return result;

    // Get output path
    char c_path[512];
    get_output_path(opts, c_path, sizeof(c_path));

    // Extract base and ensure .c extension
    char* ext = strrchr(c_path, '.');
    if (!ext || strcmp(ext, ".c") != 0) {
        strcat(c_path, ".c");
    }

    // Compile with C compiler
    char exe_path[512];
    strncpy(exe_path, c_path, sizeof(exe_path));
    ext = strrchr(exe_path, '.');
    if (ext) *ext = '\0';

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "%s %s %s -o %s",
             opts->cc, opts->cflags, c_path, exe_path);

    if (opts->verbose) {
        printf("[omnic] Compiling C: %s\n", cmd);
    }

    result = system(cmd);
    if (result != 0) {
        fprintf(stderr, "C compilation failed\n");
        return 1;
    }

    // Run the executable
    if (opts->verbose) {
        printf("[omnic] Running: %s\n", exe_path);
    }
    printf("\n--- Program Output ---\n");

    result = system(exe_path);

    printf("--- End Output (exit: %d) ---\n", result);
    return result;
}

// -----------------------------------------------------------------------------
// B.6 Main Entry Point
// -----------------------------------------------------------------------------

int main(int argc, char** argv) {
    OmnicOptions opts;

    int parse_result = parse_args(argc, argv, &opts);
    if (parse_result != 0) {
        return (parse_result < 0) ? 0 : parse_result;  // -1 = help/version
    }

    // Compile based on mode
    if (opts.run) {
        return compile_and_run(&opts);
    }

    switch (opts.mode) {
        case OUTPUT_C:
            return compile_to_c(&opts);
        case OUTPUT_IR:
            return compile_to_ir(&opts);
        case OUTPUT_EXEC:
            return compile_and_run(&opts);
        default:
            fprintf(stderr, "Unknown output mode\n");
            return 1;
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// This is the SEED COMPILER (Stage 0) for OmniCode self-hosting.
//
// Bootstrap Path:
//   Stage 0: This C compiler (omnic-c)
//   Stage 1: OmniCode compiler written in OmniCode, compiled by Stage 0
//   Stage 2: Same OmniCode compiler, compiled by Stage 1
//   Stage 3: Same OmniCode compiler, compiled by Stage 2
//
// When Stage 2 == Stage 3, self-hosting is achieved.
//
// Build:
//   cd millenniumos/tools/omnicode && make
//
// Usage:
//   ./omnic hello.omni              # Compile to hello.c
//   ./omnic hello.omni -r           # Compile and run
//   ./omnic hello.omni -v           # Verbose output
//
// "In the beginning was the Word, and the Word was with God,
//  and the Word was God."
//   — John 1:1
//
// =============================================================================
// END CLOSING
// =============================================================================
