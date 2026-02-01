// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_ide.c — OmniCode IDE Core Implementation
// Text editing with structure awareness

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide-impl
// Grounding: Proverbs 22:29 - "Seest thou a man diligent in his work?"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "omni_ide.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "kernel/dar/phase/detect.phase.h"  // CPI-SI state-aware logging

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Internal Helpers
// -----------------------------------------------------------------------------

static void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr && size > 0) {
        LOG_ERROR("ide", "Memory allocation failed");
        exit(1);
    }
    return ptr;
}

static void* safe_realloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        LOG_ERROR("ide", "Memory reallocation failed");
        exit(1);
    }
    return new_ptr;
}

static char* str_dup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char* dup = safe_malloc(len + 1);
    memcpy(dup, s, len + 1);
    return dup;
}

// -----------------------------------------------------------------------------
// B.2 Line Operations
// -----------------------------------------------------------------------------

static IDELine* line_create(const char* text) {
    IDELine* line = safe_malloc(sizeof(IDELine));
    memset(line, 0, sizeof(IDELine));

    if (text) {
        line->length = strlen(text);
        line->capacity = line->length + 64;
        line->text = safe_malloc(line->capacity);
        strcpy(line->text, text);
    } else {
        line->length = 0;
        line->capacity = 64;
        line->text = safe_malloc(line->capacity);
        line->text[0] = '\0';
    }

    line->token_capacity = 16;
    line->tokens = safe_malloc(line->token_capacity * sizeof(IDEToken));
    line->token_count = 0;

    return line;
}

static void line_free(IDELine* line) {
    if (line) {
        free(line->text);
        free(line->tokens);
    }
}

static void line_ensure_capacity(IDELine* line, uint32_t needed) {
    if (needed >= line->capacity) {
        line->capacity = needed + 64;
        line->text = safe_realloc(line->text, line->capacity);
    }
}

static void line_insert(IDELine* line, uint32_t col, const char* text, uint32_t len) {
    if (col > line->length) col = line->length;

    line_ensure_capacity(line, line->length + len + 1);

    // Shift existing text
    memmove(line->text + col + len, line->text + col, line->length - col + 1);

    // Insert new text
    memcpy(line->text + col, text, len);
    line->length += len;
}

static void line_delete(IDELine* line, uint32_t col, uint32_t len) {
    if (col >= line->length) return;
    if (col + len > line->length) len = line->length - col;

    memmove(line->text + col, line->text + col + len, line->length - col - len + 1);
    line->length -= len;
}

// -----------------------------------------------------------------------------
// B.3 Buffer Lifecycle
// -----------------------------------------------------------------------------

IDEBuffer* ide_buffer_create(void) {
    IDEBuffer* buffer = safe_malloc(sizeof(IDEBuffer));
    memset(buffer, 0, sizeof(IDEBuffer));

    // Initialize with one empty line
    buffer->line_capacity = 64;
    buffer->lines = safe_malloc(buffer->line_capacity * sizeof(IDELine));
    buffer->lines[0] = *line_create("");
    buffer->line_count = 1;

    // Undo stack
    buffer->undo_stack = safe_malloc(IDE_UNDO_STACK_SIZE * sizeof(IDEUndoEntry));
    memset(buffer->undo_stack, 0, IDE_UNDO_STACK_SIZE * sizeof(IDEUndoEntry));

    buffer->visible_lines = 24;  // Default terminal height
    buffer->visible_cols = 80;   // Default terminal width

    return buffer;
}

IDEBuffer* ide_buffer_load(const char* filepath) {
    FILE* f = fopen(filepath, "r");
    if (!f) {
        LOG_ERROR("ide", "Failed to open: %s", filepath);
        return NULL;
    }

    IDEBuffer* buffer = ide_buffer_create();
    buffer->filepath = str_dup(filepath);

    // Clear the default empty line
    line_free(&buffer->lines[0]);
    buffer->line_count = 0;

    char line_buf[IDE_MAX_LINE_LENGTH];
    while (fgets(line_buf, sizeof(line_buf), f)) {
        // Remove trailing newline
        size_t len = strlen(line_buf);
        while (len > 0 && (line_buf[len-1] == '\n' || line_buf[len-1] == '\r')) {
            line_buf[--len] = '\0';
        }

        // Expand line array if needed
        if (buffer->line_count >= buffer->line_capacity) {
            buffer->line_capacity *= 2;
            buffer->lines = safe_realloc(buffer->lines,
                                          buffer->line_capacity * sizeof(IDELine));
        }

        buffer->lines[buffer->line_count++] = *line_create(line_buf);
    }

    fclose(f);

    // Ensure at least one line
    if (buffer->line_count == 0) {
        buffer->lines[0] = *line_create("");
        buffer->line_count = 1;
    }

    // Analyze structure
    ide_detect_structure(buffer);
    ide_tokenize_all(buffer);

    LOG_INFO("ide", "Loaded: %s (%u lines)", filepath, buffer->line_count);
    return buffer;
}

bool ide_buffer_save(IDEBuffer* buffer) {
    if (!buffer || !buffer->filepath) return false;
    return ide_buffer_save_as(buffer, buffer->filepath);
}

bool ide_buffer_save_as(IDEBuffer* buffer, const char* filepath) {
    if (!buffer || !filepath) return false;

    FILE* f = fopen(filepath, "w");
    if (!f) {
        LOG_ERROR("ide", "Failed to save: %s", filepath);
        return false;
    }

    for (uint32_t i = 0; i < buffer->line_count; i++) {
        fprintf(f, "%s\n", buffer->lines[i].text);
    }

    fclose(f);

    // Update filepath if different
    if (!buffer->filepath || strcmp(buffer->filepath, filepath) != 0) {
        free(buffer->filepath);
        buffer->filepath = str_dup(filepath);
    }

    buffer->modified = false;
    LOG_INFO("ide", "Saved: %s (%u lines)", filepath, buffer->line_count);
    return true;
}

void ide_buffer_free(IDEBuffer* buffer) {
    if (!buffer) return;

    for (uint32_t i = 0; i < buffer->line_count; i++) {
        line_free(&buffer->lines[i]);
    }
    free(buffer->lines);

    for (uint32_t i = 0; i < buffer->undo_count; i++) {
        free(buffer->undo_stack[i].text);
    }
    free(buffer->undo_stack);

    free(buffer->filepath);
    free(buffer);
}

// -----------------------------------------------------------------------------
// B.4 Cursor Movement
// -----------------------------------------------------------------------------

void ide_cursor_left(IDEBuffer* buffer) {
    if (!buffer) return;

    if (buffer->cursor.col > 0) {
        buffer->cursor.col--;
    } else if (buffer->cursor.line > 0) {
        buffer->cursor.line--;
        buffer->cursor.col = buffer->lines[buffer->cursor.line].length;
    }
}

void ide_cursor_right(IDEBuffer* buffer) {
    if (!buffer) return;

    IDELine* line = &buffer->lines[buffer->cursor.line];
    if (buffer->cursor.col < line->length) {
        buffer->cursor.col++;
    } else if (buffer->cursor.line < buffer->line_count - 1) {
        buffer->cursor.line++;
        buffer->cursor.col = 0;
    }
}

void ide_cursor_up(IDEBuffer* buffer) {
    if (!buffer) return;

    if (buffer->cursor.line > 0) {
        buffer->cursor.line--;
        IDELine* line = &buffer->lines[buffer->cursor.line];
        if (buffer->cursor.col > line->length) {
            buffer->cursor.col = line->length;
        }
    }
}

void ide_cursor_down(IDEBuffer* buffer) {
    if (!buffer) return;

    if (buffer->cursor.line < buffer->line_count - 1) {
        buffer->cursor.line++;
        IDELine* line = &buffer->lines[buffer->cursor.line];
        if (buffer->cursor.col > line->length) {
            buffer->cursor.col = line->length;
        }
    }
}

void ide_cursor_home(IDEBuffer* buffer) {
    if (buffer) buffer->cursor.col = 0;
}

void ide_cursor_end(IDEBuffer* buffer) {
    if (!buffer) return;
    buffer->cursor.col = buffer->lines[buffer->cursor.line].length;
}

void ide_cursor_page_up(IDEBuffer* buffer) {
    if (!buffer) return;

    if (buffer->cursor.line >= buffer->visible_lines) {
        buffer->cursor.line -= buffer->visible_lines;
    } else {
        buffer->cursor.line = 0;
    }

    IDELine* line = &buffer->lines[buffer->cursor.line];
    if (buffer->cursor.col > line->length) {
        buffer->cursor.col = line->length;
    }
}

void ide_cursor_page_down(IDEBuffer* buffer) {
    if (!buffer) return;

    buffer->cursor.line += buffer->visible_lines;
    if (buffer->cursor.line >= buffer->line_count) {
        buffer->cursor.line = buffer->line_count - 1;
    }

    IDELine* line = &buffer->lines[buffer->cursor.line];
    if (buffer->cursor.col > line->length) {
        buffer->cursor.col = line->length;
    }
}

void ide_cursor_goto(IDEBuffer* buffer, uint32_t line, uint32_t col) {
    if (!buffer) return;

    if (line >= buffer->line_count) line = buffer->line_count - 1;
    buffer->cursor.line = line;

    IDELine* l = &buffer->lines[line];
    if (col > l->length) col = l->length;
    buffer->cursor.col = col;
}

// -----------------------------------------------------------------------------
// B.5 Editing Operations
// -----------------------------------------------------------------------------

void ide_insert_char(IDEBuffer* buffer, char c) {
    if (!buffer) return;

    IDELine* line = &buffer->lines[buffer->cursor.line];
    char str[2] = {c, '\0'};
    line_insert(line, buffer->cursor.col, str, 1);
    buffer->cursor.col++;
    buffer->modified = true;

    ide_tokenize_line(buffer, buffer->cursor.line);
}

void ide_insert(IDEBuffer* buffer, const char* text) {
    if (!buffer || !text) return;

    while (*text) {
        if (*text == '\n') {
            ide_newline(buffer);
        } else {
            ide_insert_char(buffer, *text);
        }
        text++;
    }
}

void ide_backspace(IDEBuffer* buffer) {
    if (!buffer) return;

    if (buffer->cursor.col > 0) {
        IDELine* line = &buffer->lines[buffer->cursor.line];
        buffer->cursor.col--;
        line_delete(line, buffer->cursor.col, 1);
        buffer->modified = true;
        ide_tokenize_line(buffer, buffer->cursor.line);
    } else if (buffer->cursor.line > 0) {
        // Join with previous line
        uint32_t prev = buffer->cursor.line - 1;
        IDELine* prev_line = &buffer->lines[prev];
        IDELine* curr_line = &buffer->lines[buffer->cursor.line];

        uint32_t new_col = prev_line->length;
        line_insert(prev_line, prev_line->length, curr_line->text, curr_line->length);

        // Remove current line
        line_free(curr_line);
        memmove(&buffer->lines[buffer->cursor.line],
                &buffer->lines[buffer->cursor.line + 1],
                (buffer->line_count - buffer->cursor.line - 1) * sizeof(IDELine));
        buffer->line_count--;

        buffer->cursor.line = prev;
        buffer->cursor.col = new_col;
        buffer->modified = true;

        ide_tokenize_line(buffer, buffer->cursor.line);
    }
}

void ide_delete(IDEBuffer* buffer) {
    if (!buffer) return;

    IDELine* line = &buffer->lines[buffer->cursor.line];

    if (buffer->cursor.col < line->length) {
        line_delete(line, buffer->cursor.col, 1);
        buffer->modified = true;
        ide_tokenize_line(buffer, buffer->cursor.line);
    } else if (buffer->cursor.line < buffer->line_count - 1) {
        // Join with next line
        IDELine* next_line = &buffer->lines[buffer->cursor.line + 1];
        line_insert(line, line->length, next_line->text, next_line->length);

        // Remove next line
        line_free(next_line);
        memmove(&buffer->lines[buffer->cursor.line + 1],
                &buffer->lines[buffer->cursor.line + 2],
                (buffer->line_count - buffer->cursor.line - 2) * sizeof(IDELine));
        buffer->line_count--;
        buffer->modified = true;

        ide_tokenize_line(buffer, buffer->cursor.line);
    }
}

void ide_newline(IDEBuffer* buffer) {
    if (!buffer) return;

    IDELine* line = &buffer->lines[buffer->cursor.line];

    // Split the current line
    char* remainder = str_dup(line->text + buffer->cursor.col);
    line->text[buffer->cursor.col] = '\0';
    line->length = buffer->cursor.col;

    // Expand line array if needed
    if (buffer->line_count >= buffer->line_capacity) {
        buffer->line_capacity *= 2;
        buffer->lines = safe_realloc(buffer->lines,
                                      buffer->line_capacity * sizeof(IDELine));
    }

    // Insert new line
    memmove(&buffer->lines[buffer->cursor.line + 2],
            &buffer->lines[buffer->cursor.line + 1],
            (buffer->line_count - buffer->cursor.line - 1) * sizeof(IDELine));

    buffer->lines[buffer->cursor.line + 1] = *line_create(remainder);
    free(remainder);

    buffer->line_count++;
    buffer->cursor.line++;
    buffer->cursor.col = 0;
    buffer->modified = true;

    ide_tokenize_line(buffer, buffer->cursor.line - 1);
    ide_tokenize_line(buffer, buffer->cursor.line);
}

// -----------------------------------------------------------------------------
// B.6 Syntax Analysis
// -----------------------------------------------------------------------------

static void add_token(IDELine* line, uint32_t start, uint32_t end, IDETokenType type) {
    if (line->token_count >= line->token_capacity) {
        line->token_capacity *= 2;
        line->tokens = safe_realloc(line->tokens,
                                     line->token_capacity * sizeof(IDEToken));
    }
    line->tokens[line->token_count++] = (IDEToken){start, end, type};
}

void ide_tokenize_line(IDEBuffer* buffer, uint32_t line_num) {
    if (!buffer || line_num >= buffer->line_count) return;

    IDELine* line = &buffer->lines[line_num];
    line->token_count = 0;

    const char* text = line->text;
    uint32_t i = 0;

    // Skip leading whitespace
    while (text[i] && isspace(text[i])) i++;
    if (!text[i]) return;

    // Check for pragma: #!omni
    if (strncmp(text + i, "#!omni", 6) == 0) {
        add_token(line, i, line->length, TOKEN_PRAGMA);
        return;
    }

    // Check for comment: //
    if (strncmp(text + i, "//", 2) == 0) {
        // Check for block markers: // [METADATA], // [SETUP], etc.
        const char* bracket = strchr(text + i, '[');
        if (bracket && strchr(bracket, ']')) {
            add_token(line, i, i + 2, TOKEN_COMMENT);
            add_token(line, bracket - text, strchr(bracket, ']') - text + 1, TOKEN_BLOCK_MARKER);
        } else {
            add_token(line, i, line->length, TOKEN_COMMENT);
        }
        return;
    }

    // Check for section header: # B.1 Section Name
    if (text[i] == '#' && text[i+1] == ' ') {
        add_token(line, i, line->length, TOKEN_SECTION);
        return;
    }

    // Check for block boundary markers: // ====...
    if (strncmp(text + i, "// ===", 6) == 0) {
        add_token(line, i, line->length, TOKEN_BLOCK_MARKER);
        return;
    }

    // Tokenize remaining content
    while (text[i]) {
        // String
        if (text[i] == '"') {
            uint32_t start = i++;
            while (text[i] && text[i] != '"') {
                if (text[i] == '\\' && text[i+1]) i++;
                i++;
            }
            if (text[i] == '"') i++;
            add_token(line, start, i, TOKEN_STRING);
            continue;
        }

        // Number
        if (isdigit(text[i]) || (text[i] == '-' && isdigit(text[i+1]))) {
            uint32_t start = i++;
            while (isxdigit(text[i]) || text[i] == 'x' || text[i] == '.') i++;
            add_token(line, start, i, TOKEN_NUMBER);
            continue;
        }

        // Skip other characters
        i++;
    }
}

void ide_tokenize_all(IDEBuffer* buffer) {
    if (!buffer) return;
    for (uint32_t i = 0; i < buffer->line_count; i++) {
        ide_tokenize_line(buffer, i);
    }
}

// -----------------------------------------------------------------------------
// B.7 Structure Detection
// -----------------------------------------------------------------------------

void ide_detect_structure(IDEBuffer* buffer) {
    if (!buffer) return;

    buffer->structure = STRUCTURE_UNKNOWN;

    // Look for pragma to determine structure
    for (uint32_t i = 0; i < buffer->line_count && i < 10; i++) {
        const char* line = buffer->lines[i].text;
        if (strstr(line, "#!omni code")) {
            buffer->structure = STRUCTURE_4_BLOCK;
            break;
        } else if (strstr(line, "#!omni data") || strstr(line, "#!omni config")) {
            buffer->structure = STRUCTURE_3_BLOCK;
            break;
        } else if (strstr(line, "#!omni document") || strstr(line, "#!omni template")) {
            buffer->structure = STRUCTURE_5_BLOCK;
            break;
        }
    }

    ide_update_blocks(buffer);
}

void ide_update_blocks(IDEBuffer* buffer) {
    if (!buffer) return;

    IDEBlockType current = BLOCK_NONE;

    for (uint32_t i = 0; i < buffer->line_count; i++) {
        IDELine* line = &buffer->lines[i];
        const char* text = line->text;

        line->block = current;
        line->is_block_start = false;
        line->is_block_end = false;

        // Look for block markers
        if (strstr(text, "[METADATA]")) {
            line->is_block_start = true;
            current = (buffer->structure == STRUCTURE_3_BLOCK) ? BLOCK_3_METADATA :
                      (buffer->structure == STRUCTURE_4_BLOCK) ? BLOCK_4_METADATA :
                      BLOCK_5_METADATA;
            line->block = current;
        } else if (strstr(text, "END METADATA")) {
            line->is_block_end = true;
        } else if (strstr(text, "[SETUP]")) {
            line->is_block_start = true;
            current = BLOCK_4_SETUP;
            line->block = current;
        } else if (strstr(text, "END SETUP")) {
            line->is_block_end = true;
        } else if (strstr(text, "[HEADER]")) {
            line->is_block_start = true;
            current = BLOCK_5_HEADER;
            line->block = current;
        } else if (strstr(text, "END HEADER")) {
            line->is_block_end = true;
        } else if (strstr(text, "[CONTEXT]")) {
            line->is_block_start = true;
            current = BLOCK_5_CONTEXT;
            line->block = current;
        } else if (strstr(text, "END CONTEXT")) {
            line->is_block_end = true;
        } else if (strstr(text, "[BODY]")) {
            line->is_block_start = true;
            current = BLOCK_4_BODY;
            line->block = current;
        } else if (strstr(text, "END BODY")) {
            line->is_block_end = true;
        } else if (strstr(text, "[CONTENT]")) {
            line->is_block_start = true;
            current = (buffer->structure == STRUCTURE_3_BLOCK) ? BLOCK_3_CONTENT :
                      BLOCK_5_CONTENT;
            line->block = current;
        } else if (strstr(text, "END CONTENT")) {
            line->is_block_end = true;
        } else if (strstr(text, "[CLOSING]")) {
            line->is_block_start = true;
            current = (buffer->structure == STRUCTURE_3_BLOCK) ? BLOCK_3_CLOSING :
                      BLOCK_4_CLOSING;
            line->block = current;
        } else if (strstr(text, "END CLOSING")) {
            line->is_block_end = true;
        } else if (strstr(text, "[FOOTER]")) {
            line->is_block_start = true;
            current = BLOCK_5_FOOTER;
            line->block = current;
        } else if (strstr(text, "END FOOTER")) {
            line->is_block_end = true;
        }
    }
}

// -----------------------------------------------------------------------------
// B.8 Query Functions
// -----------------------------------------------------------------------------

const char* ide_get_line(IDEBuffer* buffer, uint32_t line) {
    if (!buffer || line >= buffer->line_count) return NULL;
    return buffer->lines[line].text;
}

uint32_t ide_line_count(IDEBuffer* buffer) {
    return buffer ? buffer->line_count : 0;
}

IDECursor ide_cursor(IDEBuffer* buffer) {
    IDECursor empty = {0, 0};
    return buffer ? buffer->cursor : empty;
}

bool ide_is_modified(IDEBuffer* buffer) {
    return buffer ? buffer->modified : false;
}

IDEBlockStructure ide_get_structure(IDEBuffer* buffer) {
    return buffer ? buffer->structure : STRUCTURE_UNKNOWN;
}

IDEBlockType ide_current_block(IDEBuffer* buffer) {
    if (!buffer) return BLOCK_NONE;
    return buffer->lines[buffer->cursor.line].block;
}

// -----------------------------------------------------------------------------
// B.9 Validation
// -----------------------------------------------------------------------------

IDEDiagnostics* ide_diagnostics_create(void) {
    IDEDiagnostics* diags = safe_malloc(sizeof(IDEDiagnostics));
    diags->capacity = 32;
    diags->items = safe_malloc(diags->capacity * sizeof(IDEDiagnostic));
    diags->count = 0;
    return diags;
}

void ide_diagnostics_free(IDEDiagnostics* diags) {
    if (diags) {
        free(diags->items);
        free(diags);
    }
}

void ide_diagnostic_add(IDEDiagnostics* diags, uint32_t line, uint32_t col,
                        IDEDiagSeverity severity, const char* message) {
    if (!diags) return;

    if (diags->count >= diags->capacity) {
        diags->capacity *= 2;
        diags->items = safe_realloc(diags->items,
                                     diags->capacity * sizeof(IDEDiagnostic));
    }

    IDEDiagnostic* d = &diags->items[diags->count++];
    d->line = line;
    d->col = col;
    d->severity = severity;
    strncpy(d->message, message, sizeof(d->message) - 1);
    d->message[sizeof(d->message) - 1] = '\0';
}

void ide_validate_pragma(IDEBuffer* buffer, IDEDiagnostics* diags) {
    if (!buffer || !diags) return;

    bool found_pragma = false;
    for (uint32_t i = 0; i < buffer->line_count && i < 5; i++) {
        if (strstr(buffer->lines[i].text, "#!omni")) {
            found_pragma = true;
            break;
        }
    }

    if (!found_pragma) {
        ide_diagnostic_add(diags, 0, 0, DIAG_ERROR,
                          "Missing #!omni pragma (should be on first line)");
    }
}

void ide_validate_structure(IDEBuffer* buffer, IDEDiagnostics* diags) {
    if (!buffer || !diags) return;

    // Check block structure based on detected type
    bool has_metadata = false, has_closing = false;
    bool has_setup = false, has_body = false;
    bool has_header = false, has_context = false, has_content = false, has_footer = false;

    for (uint32_t i = 0; i < buffer->line_count; i++) {
        const char* text = buffer->lines[i].text;
        if (strstr(text, "[METADATA]")) has_metadata = true;
        if (strstr(text, "[SETUP]")) has_setup = true;
        if (strstr(text, "[BODY]")) has_body = true;
        if (strstr(text, "[HEADER]")) has_header = true;
        if (strstr(text, "[CONTEXT]")) has_context = true;
        if (strstr(text, "[CONTENT]")) has_content = true;
        if (strstr(text, "[CLOSING]")) has_closing = true;
        if (strstr(text, "[FOOTER]")) has_footer = true;
    }

    if (!has_metadata) {
        ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [METADATA] block");
    }

    switch (buffer->structure) {
        case STRUCTURE_3_BLOCK:
            if (!has_content) {
                ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [CONTENT] block (3-block requires METADATA → CONTENT → CLOSING)");
            }
            if (!has_closing) {
                ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [CLOSING] block");
            }
            break;

        case STRUCTURE_4_BLOCK:
            if (!has_setup) {
                ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [SETUP] block (4-block requires METADATA → SETUP → BODY → CLOSING)");
            }
            if (!has_body) {
                ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [BODY] block");
            }
            if (!has_closing) {
                ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [CLOSING] block");
            }
            break;

        case STRUCTURE_5_BLOCK:
            if (!has_header) {
                ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [HEADER] block (5-block requires METADATA → HEADER → CONTEXT → CONTENT → FOOTER)");
            }
            if (!has_context) {
                ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [CONTEXT] block");
            }
            if (!has_content) {
                ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [CONTENT] block");
            }
            if (!has_footer) {
                ide_diagnostic_add(diags, 0, 0, DIAG_ERROR, "Missing [FOOTER] block");
            }
            break;

        default:
            ide_diagnostic_add(diags, 0, 0, DIAG_WARNING,
                              "Unknown block structure - add #!omni pragma to specify");
            break;
    }
}

void ide_validate_all(IDEBuffer* buffer, IDEDiagnostics* diags) {
    if (!buffer || !diags) return;
    ide_validate_pragma(buffer, diags);
    ide_validate_structure(buffer, diags);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Seest thou a man diligent in his work? He shall stand before kings."
//   — Proverbs 22:29
//
// The IDE enables skilled work through proper tooling.
// Structure-aware editing honors the architecture.

// =============================================================================
// END CLOSING
// =============================================================================
