// #!omni code --c -tool
// #!omni meta.key = B-tov-demo-phase3-omni-runner
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_runner.c — OmniCode File Runner
// Executes .omni files with optional debug output.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-tov-demo-phase3-omni-runner
// Title: OmniCode File Runner
// Type: Tool
// Role: Execute .omni files with full pipeline (parse → IR → VM)

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:3 - "And God said, Let there be light: and there was light"
// Principle: Words become action - code becomes execution

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "omni_chunks.h"
#include "omni_parser.h"
#include "omni_ir.h"
#include "omni_vm.h"
#include "omni_debug.h"
#include "omni_dar.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

static void print_usage(const char* prog) {
    fprintf(stderr, "Usage: %s [options] <file.omni>\n", prog);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  -d, --debug      Enable debug output\n");
    fprintf(stderr, "  -t, --trace      Enable instruction tracing\n");
    fprintf(stderr, "  -s, --stack      Enable stack inspection\n");
    fprintf(stderr, "  -a, --all        Enable all debug features\n");
    fprintf(stderr, "  -l, --log FILE   Write log to file\n");
    fprintf(stderr, "  -D, --disasm     Show disassembly before execution\n");
    fprintf(stderr, "  -h, --help       Show this help\n");
}

static char* read_file(const char* path, size_t* out_size) {
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t read = fread(buffer, 1, size, f);
    buffer[read] = '\0';
    fclose(f);

    if (out_size) *out_size = read;
    return buffer;
}

static const char* result_name(OmniVMResult result) {
    switch (result) {
        case VM_GRANTED:  return "GRANTED";
        case VM_DEFERRED: return "DEFERRED";
        case VM_DENIED:   return "DENIED";
        default:          return "UNKNOWN";
    }
}

static const char* hebrew_name(int8_t state) {
    switch (state) {
        case -3: return "shavar (broken)";
        case -2: return "chaser (lacking)";
        case -1: return "ratsah (wanting)";
        case  0: return "yashar (even)";
        case +1: return "tamim (sound)";
        case +2: return "shalem (whole)";
        case +3: return "tov (perfect)";
        default: return "unknown";
    }
}

int main(int argc, char** argv) {
    // Parse options
    bool debug_enabled = false;
    bool trace_enabled = false;
    bool stack_enabled = false;
    bool show_disasm = false;
    const char* log_file = NULL;

    static struct option long_options[] = {
        {"debug",  no_argument,       0, 'd'},
        {"trace",  no_argument,       0, 't'},
        {"stack",  no_argument,       0, 's'},
        {"all",    no_argument,       0, 'a'},
        {"log",    required_argument, 0, 'l'},
        {"disasm", no_argument,       0, 'D'},
        {"help",   no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "dtsaDl:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd': debug_enabled = true; break;
            case 't': trace_enabled = true; break;
            case 's': stack_enabled = true; break;
            case 'a':
                debug_enabled = true;
                trace_enabled = true;
                stack_enabled = true;
                break;
            case 'l': log_file = optarg; break;
            case 'D': show_disasm = true; break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 2;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return 2;
    }

    const char* input_file = argv[optind];

    // Setup debug context
    OmniDebug* dbg = NULL;
    if (debug_enabled || trace_enabled || stack_enabled || log_file) {
        dbg = omni_debug_create();
        if (dbg) {
            omni_debug_set_level(dbg, OMNI_LOG_DEBUG);

            uint8_t flags = 0;
            if (trace_enabled) flags |= OMNI_DBG_TRACE;
            if (stack_enabled) flags |= OMNI_DBG_STACK;
            if (debug_enabled) flags |= OMNI_DBG_HEALTH | OMNI_DBG_HEBREW | OMNI_DBG_CHECKPOINT;
            omni_debug_set_flags(dbg, flags);

            if (log_file) {
                omni_debug_set_file(dbg, log_file);
            }
        }
    }

    // Read source file
    size_t source_size;
    char* source = read_file(input_file, &source_size);
    if (!source) {
        if (dbg) omni_debug_destroy(dbg);
        return 1;
    }

    if (dbg) OMNI_INFO(dbg, "Loaded %s (%zu bytes)", input_file, source_size);

    // Parse to AST
    OmniChunkLexer chunk_lexer;
    omni_chunk_init(&chunk_lexer, source, source_size);

    OmniParser parser;
    omni_parser_init(&parser, &chunk_lexer);

    OmniDocument* doc = omni_parser_parse(&parser);
    if (!doc) {
        fprintf(stderr, "Error: Failed to parse '%s'\n", input_file);
        free(source);
        if (dbg) omni_debug_destroy(dbg);
        return 1;
    }

    if (dbg) OMNI_INFO(dbg, "Parsed %u AST nodes", doc->node_count);

    // Convert to IR
    OmniIRProgram* program = omni_ir_from_ast(doc);
    omni_document_destroy(doc);

    if (!program) {
        fprintf(stderr, "Error: Failed to generate IR\n");
        free(source);
        if (dbg) omni_debug_destroy(dbg);
        return 1;
    }

    if (dbg) OMNI_INFO(dbg, "Generated IR: %u instructions, %u constants, %u functions",
                       program->code_length, program->constant_count, program->function_count);

    // Show disassembly if requested
    if (show_disasm) {
        printf("\n--- Disassembly ---\n");
        omni_ir_disassemble(program, stdout);
        printf("--- End Disassembly ---\n\n");
    }

    // Create VM and execute
    OmniVM* vm = omni_vm_create();
    if (!vm) {
        fprintf(stderr, "Error: Failed to create VM\n");
        omni_ir_destroy(program);
        free(source);
        if (dbg) omni_debug_destroy(dbg);
        return 1;
    }

    if (!omni_vm_load(vm, program)) {
        fprintf(stderr, "Error: Failed to load program\n");
        omni_vm_destroy(vm);
        omni_ir_destroy(program);
        free(source);
        if (dbg) omni_debug_destroy(dbg);
        return 1;
    }

    // Execute
    if (dbg) {
        omni_debug_trace_pre(dbg, vm);
    }

    printf("--- Output ---\n");
    OmniVMResult result = omni_vm_run(vm);
    printf("--- End Output ---\n\n");

    if (dbg) {
        omni_debug_trace_post(dbg, vm);
    }

    // Report result
    printf("Result: %s\n", result_name(result));
    printf("Health: %d\n", vm->health);
    printf("State:  %s\n", hebrew_name(vm->hebrew_state));

    if (result == VM_DENIED && vm->error[0]) {
        printf("Error:  %s\n", vm->error);
    }

    // Cleanup
    omni_vm_destroy(vm);
    omni_ir_destroy(program);
    free(source);

    if (dbg) {
        omni_debug_flush(dbg);
        omni_debug_destroy(dbg);
    }

    return result == VM_DENIED ? 1 : 0;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "And God said, Let there be light: and there was light."
//   — Genesis 1:3
//
// Words become action. Code becomes execution.

// =============================================================================
// END CLOSING
// =============================================================================
