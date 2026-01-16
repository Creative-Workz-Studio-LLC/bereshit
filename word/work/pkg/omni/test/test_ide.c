// #!omni code --c -test
// #!omni meta.key = B-word-work-pkg-omni-test-ide
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// test_ide.c — OmniCode IDE Test
// Load file, show structure, validate

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-test-ide
// Grounding: Proverbs 22:29 - Diligent work

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include "omni_ide.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

static const char* structure_name(IDEBlockStructure s) {
    switch (s) {
        case STRUCTURE_3_BLOCK: return "3-Block (Data)";
        case STRUCTURE_4_BLOCK: return "4-Block (Code)";
        case STRUCTURE_5_BLOCK: return "5-Block (Documentation)";
        default: return "Unknown";
    }
}

static const char* block_name(IDEBlockType b) {
    switch (b) {
        case BLOCK_3_METADATA: return "METADATA (3)";
        case BLOCK_3_CONTENT: return "CONTENT (3)";
        case BLOCK_3_CLOSING: return "CLOSING (3)";
        case BLOCK_4_METADATA: return "METADATA (4)";
        case BLOCK_4_SETUP: return "SETUP";
        case BLOCK_4_BODY: return "BODY";
        case BLOCK_4_CLOSING: return "CLOSING (4)";
        case BLOCK_5_METADATA: return "METADATA (5)";
        case BLOCK_5_HEADER: return "HEADER";
        case BLOCK_5_CONTEXT: return "CONTEXT";
        case BLOCK_5_CONTENT: return "CONTENT (5)";
        case BLOCK_5_FOOTER: return "FOOTER";
        default: return "(none)";
    }
}

static const char* block_color(IDEBlockType b) {
    switch (b) {
        case BLOCK_3_METADATA:
        case BLOCK_4_METADATA:
        case BLOCK_5_METADATA: return COLOR_CYAN;
        case BLOCK_4_SETUP: return COLOR_BLUE;
        case BLOCK_5_HEADER: return COLOR_MAGENTA;
        case BLOCK_5_CONTEXT: return COLOR_YELLOW;
        case BLOCK_4_BODY:
        case BLOCK_3_CONTENT:
        case BLOCK_5_CONTENT: return COLOR_GREEN;
        case BLOCK_3_CLOSING:
        case BLOCK_4_CLOSING:
        case BLOCK_5_FOOTER: return COLOR_RED;
        default: return COLOR_RESET;
    }
}

static const char* severity_name(IDEDiagSeverity s) {
    switch (s) {
        case DIAG_INFO: return "INFO";
        case DIAG_WARNING: return "WARNING";
        case DIAG_ERROR: return "ERROR";
        default: return "?";
    }
}

static const char* severity_color(IDEDiagSeverity s) {
    switch (s) {
        case DIAG_INFO: return COLOR_BLUE;
        case DIAG_WARNING: return COLOR_YELLOW;
        case DIAG_ERROR: return COLOR_RED;
        default: return COLOR_RESET;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <file.omni>\n", argv[0]);
        printf("\nLoads an OmniCode file and shows its structure.\n");
        return 1;
    }

    printf(COLOR_MAGENTA "\n═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_MAGENTA "  OmniCode IDE Test\n" COLOR_RESET);
    printf(COLOR_MAGENTA "  \"Seest thou a man diligent in his work?\" — Proverbs 22:29\n" COLOR_RESET);
    printf(COLOR_MAGENTA "═══════════════════════════════════════════════════════════════\n\n" COLOR_RESET);

    // Load buffer
    IDEBuffer* buffer = ide_buffer_load(argv[1]);
    if (!buffer) {
        printf(COLOR_RED "Failed to load file\n" COLOR_RESET);
        return 1;
    }

    // Show structure info
    printf(COLOR_CYAN "Structure: " COLOR_RESET "%s\n", structure_name(ide_get_structure(buffer)));
    printf(COLOR_CYAN "Lines:     " COLOR_RESET "%u\n\n", ide_line_count(buffer));

    // Show first 30 lines with block coloring
    printf(COLOR_CYAN "─── Content (first 30 lines) ─────────────────────────────────\n" COLOR_RESET);

    uint32_t max_lines = ide_line_count(buffer);
    if (max_lines > 30) max_lines = 30;

    IDEBlockType last_block = BLOCK_NONE;
    for (uint32_t i = 0; i < max_lines; i++) {
        IDELine* line = &buffer->lines[i];
        IDEBlockType b = line->block;

        // Show block change
        if (b != last_block && b != BLOCK_NONE) {
            printf(COLOR_YELLOW "  ┌─ %s ─┐\n" COLOR_RESET, block_name(b));
            last_block = b;
        }

        // Show line with color
        printf("%s%4u│%s %s\n",
               block_color(b), i + 1, COLOR_RESET,
               ide_get_line(buffer, i));
    }

    if (ide_line_count(buffer) > 30) {
        printf(COLOR_YELLOW "  ... (%u more lines)\n" COLOR_RESET,
               ide_line_count(buffer) - 30);
    }

    // Validation
    printf(COLOR_CYAN "\n─── Validation ───────────────────────────────────────────────\n" COLOR_RESET);

    IDEDiagnostics* diags = ide_diagnostics_create();
    ide_validate_all(buffer, diags);

    if (diags->count == 0) {
        printf(COLOR_GREEN "  ✓ No issues found\n" COLOR_RESET);
    } else {
        for (uint32_t i = 0; i < diags->count; i++) {
            IDEDiagnostic* d = &diags->items[i];
            printf("%s  [%s] Line %u: %s%s\n",
                   severity_color(d->severity),
                   severity_name(d->severity),
                   d->line + 1,
                   d->message,
                   COLOR_RESET);
        }
    }

    ide_diagnostics_free(diags);

    // Block summary
    printf(COLOR_CYAN "\n─── Block Summary ────────────────────────────────────────────\n" COLOR_RESET);

    uint32_t block_counts[16] = {0};
    for (uint32_t i = 0; i < buffer->line_count; i++) {
        IDEBlockType b = buffer->lines[i].block;
        if (b < 16) block_counts[b]++;
    }

    for (int b = 1; b < 16; b++) {
        if (block_counts[b] > 0) {
            printf("  %s%-16s%s %u lines\n",
                   block_color(b), block_name(b), COLOR_RESET, block_counts[b]);
        }
    }

    printf("\n");

    ide_buffer_free(buffer);
    return 0;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// IDE testing validates structure awareness.

// =============================================================================
// END CLOSING
// =============================================================================
