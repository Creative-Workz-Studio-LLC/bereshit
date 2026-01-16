// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-ide
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_ide.h — OmniCode IDE Core
// Text editing with structure awareness

#ifndef BERESHIT_OMNI_IDE_H
#define BERESHIT_OMNI_IDE_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// Key: B-word-work-pkg-omni-ide
// Title: OmniCode IDE Core
// Type: Header
// Role: Text buffer management with OmniCode syntax and block awareness

// Grounding: Proverbs 22:29 - "Seest thou a man diligent in his work?
//            He shall stand before kings"
// Principle: Skilled work through proper tooling
// Anchor: Genesis 1:1 - Created order enables creation

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 IDE Constants [CONSTANTS]

#define IDE_MAX_LINE_LENGTH     4096
#define IDE_MAX_LINES           65536
#define IDE_TAB_WIDTH           4
#define IDE_UNDO_STACK_SIZE     256

// # B.2 Block Types [BLOCKS]
//
// OmniCode block structure validation

typedef enum {
    BLOCK_NONE = 0,

    // 3-Block (Data): METADATA → CONTENT → CLOSING
    BLOCK_3_METADATA,
    BLOCK_3_CONTENT,
    BLOCK_3_CLOSING,

    // 4-Block (Code): METADATA → SETUP → BODY → CLOSING
    BLOCK_4_METADATA,
    BLOCK_4_SETUP,
    BLOCK_4_BODY,
    BLOCK_4_CLOSING,

    // 5-Block (Documentation): METADATA → HEADER → CONTEXT → CONTENT → FOOTER
    BLOCK_5_METADATA,
    BLOCK_5_HEADER,
    BLOCK_5_CONTEXT,
    BLOCK_5_CONTENT,
    BLOCK_5_FOOTER
} IDEBlockType;

// Block structure pattern
typedef enum {
    STRUCTURE_UNKNOWN = 0,
    STRUCTURE_3_BLOCK,      // Data files
    STRUCTURE_4_BLOCK,      // Code files
    STRUCTURE_5_BLOCK       // Documentation
} IDEBlockStructure;

// # B.3 Token Types [TOKENS]
//
// For syntax highlighting

typedef enum {
    TOKEN_NORMAL = 0,
    TOKEN_PRAGMA,           // #!omni ...
    TOKEN_KEYWORD,          // OmniCode keywords
    TOKEN_COMMENT,          // // comments
    TOKEN_STRING,           // "strings"
    TOKEN_NUMBER,           // 123, 0xFF
    TOKEN_BLOCK_MARKER,     // [METADATA], [SETUP], etc.
    TOKEN_SECTION,          // # B.1 Section Name
    TOKEN_SCRIPTURE,        // Scripture references
    TOKEN_ERROR             // Syntax errors
} IDETokenType;

// Token span
typedef struct {
    uint32_t start;         // Column start
    uint32_t end;           // Column end
    IDETokenType type;
} IDEToken;

// # B.4 Line Structure [LINE]

typedef struct {
    char* text;             // Line content (null-terminated)
    uint32_t length;        // Length excluding null
    uint32_t capacity;      // Allocated size

    // Syntax tokens for this line
    IDEToken* tokens;
    uint32_t token_count;
    uint32_t token_capacity;

    // Block information
    IDEBlockType block;     // What block this line is in
    bool is_block_start;    // Line starts a block
    bool is_block_end;      // Line ends a block
} IDELine;

// # B.5 Cursor Position [CURSOR]

typedef struct {
    uint32_t line;          // Line number (0-indexed)
    uint32_t col;           // Column (0-indexed)
} IDECursor;

// Selection
typedef struct {
    IDECursor start;
    IDECursor end;
    bool active;
} IDESelection;

// # B.6 Undo/Redo [UNDO]

typedef enum {
    UNDO_INSERT,
    UNDO_DELETE,
    UNDO_REPLACE
} IDEUndoType;

typedef struct {
    IDEUndoType type;
    IDECursor position;
    char* text;             // Text that was inserted/deleted
    uint32_t length;
} IDEUndoEntry;

// # B.7 Editor Buffer [BUFFER]

typedef struct {
    // Lines
    IDELine* lines;
    uint32_t line_count;
    uint32_t line_capacity;

    // Cursor
    IDECursor cursor;
    IDESelection selection;

    // Undo/Redo
    IDEUndoEntry* undo_stack;
    uint32_t undo_count;
    uint32_t undo_index;

    // File info
    char* filepath;
    bool modified;
    IDEBlockStructure structure;    // Detected block structure

    // Viewport (for rendering)
    uint32_t scroll_line;           // First visible line
    uint32_t scroll_col;            // Horizontal scroll
    uint32_t visible_lines;         // How many lines fit
    uint32_t visible_cols;          // How many columns fit
} IDEBuffer;

// # B.8 Validation [VALIDATION]

typedef enum {
    DIAG_INFO,
    DIAG_WARNING,
    DIAG_ERROR
} IDEDiagSeverity;

typedef struct {
    uint32_t line;
    uint32_t col;
    IDEDiagSeverity severity;
    char message[256];
} IDEDiagnostic;

typedef struct {
    IDEDiagnostic* items;
    uint32_t count;
    uint32_t capacity;
} IDEDiagnostics;

// # B.9 Buffer Lifecycle [LIFECYCLE]

// Create empty buffer
IDEBuffer* ide_buffer_create(void);

// Load file into buffer
IDEBuffer* ide_buffer_load(const char* filepath);

// Save buffer to file
bool ide_buffer_save(IDEBuffer* buffer);

// Save buffer to new path
bool ide_buffer_save_as(IDEBuffer* buffer, const char* filepath);

// Free buffer
void ide_buffer_free(IDEBuffer* buffer);

// # B.10 Editing Operations [EDIT]

// Insert text at cursor
void ide_insert(IDEBuffer* buffer, const char* text);

// Insert character at cursor
void ide_insert_char(IDEBuffer* buffer, char c);

// Delete character before cursor (backspace)
void ide_backspace(IDEBuffer* buffer);

// Delete character at cursor (delete)
void ide_delete(IDEBuffer* buffer);

// Delete selection
void ide_delete_selection(IDEBuffer* buffer);

// Insert newline
void ide_newline(IDEBuffer* buffer);

// Undo last change
void ide_undo(IDEBuffer* buffer);

// Redo last undone change
void ide_redo(IDEBuffer* buffer);

// # B.11 Cursor Movement [MOVEMENT]

void ide_cursor_left(IDEBuffer* buffer);
void ide_cursor_right(IDEBuffer* buffer);
void ide_cursor_up(IDEBuffer* buffer);
void ide_cursor_down(IDEBuffer* buffer);
void ide_cursor_home(IDEBuffer* buffer);      // Start of line
void ide_cursor_end(IDEBuffer* buffer);       // End of line
void ide_cursor_page_up(IDEBuffer* buffer);
void ide_cursor_page_down(IDEBuffer* buffer);
void ide_cursor_goto(IDEBuffer* buffer, uint32_t line, uint32_t col);

// # B.12 Selection [SELECT]

void ide_select_start(IDEBuffer* buffer);
void ide_select_end(IDEBuffer* buffer);
void ide_select_all(IDEBuffer* buffer);
void ide_select_line(IDEBuffer* buffer);
void ide_select_word(IDEBuffer* buffer);
char* ide_get_selection(IDEBuffer* buffer);   // Returns allocated string

// # B.13 Syntax Analysis [SYNTAX]

// Re-tokenize a line
void ide_tokenize_line(IDEBuffer* buffer, uint32_t line);

// Re-tokenize entire buffer
void ide_tokenize_all(IDEBuffer* buffer);

// Detect block structure
void ide_detect_structure(IDEBuffer* buffer);

// Update block markers
void ide_update_blocks(IDEBuffer* buffer);

// # B.14 Validation [VALIDATE]

// Validate block structure
void ide_validate_structure(IDEBuffer* buffer, IDEDiagnostics* diags);

// Validate pragma
void ide_validate_pragma(IDEBuffer* buffer, IDEDiagnostics* diags);

// Full validation
void ide_validate_all(IDEBuffer* buffer, IDEDiagnostics* diags);

// Create diagnostics container
IDEDiagnostics* ide_diagnostics_create(void);

// Free diagnostics
void ide_diagnostics_free(IDEDiagnostics* diags);

// Add diagnostic
void ide_diagnostic_add(IDEDiagnostics* diags, uint32_t line, uint32_t col,
                        IDEDiagSeverity severity, const char* message);

// # B.15 Template Insertion [TEMPLATE]

// Insert block template at cursor
void ide_insert_block_template(IDEBuffer* buffer, IDEBlockType block);

// Insert full structure template
void ide_insert_structure_template(IDEBuffer* buffer, IDEBlockStructure structure);

// Insert common patterns
void ide_insert_section(IDEBuffer* buffer, const char* name);
void ide_insert_comment(IDEBuffer* buffer, const char* text);

// # B.16 Query [QUERY]

// Get current line text
const char* ide_get_line(IDEBuffer* buffer, uint32_t line);

// Get line count
uint32_t ide_line_count(IDEBuffer* buffer);

// Get cursor position
IDECursor ide_cursor(IDEBuffer* buffer);

// Check if modified
bool ide_is_modified(IDEBuffer* buffer);

// Get detected structure
IDEBlockStructure ide_get_structure(IDEBuffer* buffer);

// Get current block
IDEBlockType ide_current_block(IDEBuffer* buffer);

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

#endif // BERESHIT_OMNI_IDE_H

// =============================================================================
// END CLOSING
// =============================================================================
