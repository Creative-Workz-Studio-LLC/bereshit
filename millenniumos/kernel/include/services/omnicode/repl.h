// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-omnicode-repl
// Title:       MillenniumOS OmniCode REPL
// Type:        Header
// Component:   Stage 1 (Kernel) / OmniCode Subsystem
// Role:        Interactive Read-Eval-Print Loop for OmniCode
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// Grounding:   Psalm 119:105 — "Thy word is a lamp unto my feet"
//              The REPL illuminates, step by step.
//
// =============================================================================
// END METADATA
// =============================================================================

#ifndef MILLENNIUMOS_OMNICODE_REPL_H
#define MILLENNIUMOS_OMNICODE_REPL_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"
#include "services/omnicode/omnicode.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 REPL Constants
// -----------------------------------------------------------------------------

#define MOS_REPL_MAX_INPUT          1024        // Max input line
#define MOS_REPL_MAX_HISTORY        81          // 3^4 history entries
#define MOS_REPL_MAX_OUTPUT         4096        // Max output buffer

// REPL prompts
#define MOS_REPL_PROMPT_NORMAL      "omni> "
#define MOS_REPL_PROMPT_CONTINUE    "    > "
#define MOS_REPL_PROMPT_BLOCK       "[...] "

// REPL modes (ternary)
#define MOS_REPL_MODE_NORMAL        TRIT_ZERO   // Single-line evaluation
#define MOS_REPL_MODE_BLOCK         TRIT_POS    // Multi-line block entry
#define MOS_REPL_MODE_BIBLE         TRIT_NEG    // Bible reference mode

// REPL state (Hebrew)
#define MOS_REPL_STATE_READY        MOS_HEBREW_YASHAR
#define MOS_REPL_STATE_PROCESSING   MOS_HEBREW_RATSAH
#define MOS_REPL_STATE_ERROR        MOS_HEBREW_SHAVAR
#define MOS_REPL_STATE_COMPLETE     MOS_HEBREW_TOV

// -----------------------------------------------------------------------------
// B.2 REPL Structures
// -----------------------------------------------------------------------------

// History entry
typedef struct {
    char        input[MOS_REPL_MAX_INPUT];
    trit27_t    result;
    mos_time_t  timestamp;
    bool        success;
} MosReplHistoryEntry;

// REPL session state
typedef struct {
    // Input state
    char            input[MOS_REPL_MAX_INPUT];
    mos_size_t      input_len;
    char            block_buffer[MOS_OMNI_MAX_SOURCE];
    mos_size_t      block_len;

    // Mode
    trit_t          mode;
    MosHebrewState  state;
    mos_health_t    health;

    // History
    MosReplHistoryEntry history[MOS_REPL_MAX_HISTORY];
    mos_size_t      history_count;
    mos_size_t      history_pos;

    // Output
    char            output[MOS_REPL_MAX_OUTPUT];
    mos_size_t      output_len;

    // Statistics
    trit27_t        lines_evaluated;
    trit27_t        errors;
    trit27_t        commands;

    // Configuration
    bool            echo;           // Echo input
    bool            timing;         // Show execution time
    bool            debug;          // Debug mode
    bool            color;          // Color output
    bool            bible_hints;    // Show Bible references
} MosReplSession;

// -----------------------------------------------------------------------------
// B.3 Initialization
// -----------------------------------------------------------------------------

// Create new REPL session
mos_result_t mos_repl_create(MosReplSession* session);

// Destroy REPL session
void mos_repl_destroy(MosReplSession* session);

// Reset session state
void mos_repl_reset(MosReplSession* session);

// -----------------------------------------------------------------------------
// B.4 Input Handling
// -----------------------------------------------------------------------------

// Process single character input
mos_result_t mos_repl_input_char(MosReplSession* session, char c);

// Process complete line
mos_result_t mos_repl_input_line(MosReplSession* session, const char* line);

// Check if input is complete (for multi-line)
bool mos_repl_input_complete(MosReplSession* session);

// Clear current input
void mos_repl_input_clear(MosReplSession* session);

// -----------------------------------------------------------------------------
// B.5 Evaluation
// -----------------------------------------------------------------------------

// Evaluate current input
mos_result_t mos_repl_eval(MosReplSession* session);

// Evaluate arbitrary code
mos_result_t mos_repl_eval_string(MosReplSession* session, const char* code,
                                   trit27_t* result);

// Execute REPL command (starts with :)
mos_result_t mos_repl_command(MosReplSession* session, const char* command);

// -----------------------------------------------------------------------------
// B.6 Output
// -----------------------------------------------------------------------------

// Get current prompt string
const char* mos_repl_prompt(MosReplSession* session);

// Get last output
const char* mos_repl_output(MosReplSession* session);

// Print value to output
mos_result_t mos_repl_print(MosReplSession* session, const char* format, ...);

// Print error to output
mos_result_t mos_repl_error(MosReplSession* session, const char* message);

// -----------------------------------------------------------------------------
// B.7 History
// -----------------------------------------------------------------------------

// Navigate history up
mos_result_t mos_repl_history_up(MosReplSession* session);

// Navigate history down
mos_result_t mos_repl_history_down(MosReplSession* session);

// Search history
mos_result_t mos_repl_history_search(MosReplSession* session, const char* prefix);

// Clear history
void mos_repl_history_clear(MosReplSession* session);

// -----------------------------------------------------------------------------
// B.8 Built-in Commands
// -----------------------------------------------------------------------------

// Available commands (use with :command)
//
// :help              - Show help
// :quit / :q         - Exit REPL
// :clear / :c        - Clear screen
// :history / :h      - Show history
// :load <file>       - Load and execute file
// :save <file>       - Save history to file
// :debug             - Toggle debug mode
// :timing            - Toggle timing display
// :bible             - Toggle Bible mode
// :verse <ref>       - Look up Bible verse
// :health            - Show runtime health
// :state             - Show Hebrew state
// :modules           - List loaded modules
// :info              - Show session info
// :reset             - Reset session

// Check if input is a command
bool mos_repl_is_command(const char* input);

// -----------------------------------------------------------------------------
// B.9 Bible Integration
// -----------------------------------------------------------------------------

// Enter Bible mode
void mos_repl_bible_mode(MosReplSession* session, bool enable);

// Look up verse reference
mos_result_t mos_repl_bible_lookup(MosReplSession* session, const char* reference);

// Get verse for current context (shows relevant wisdom)
mos_result_t mos_repl_bible_hint(MosReplSession* session);

// -----------------------------------------------------------------------------
// B.10 Main Loop
// -----------------------------------------------------------------------------

// Run REPL main loop (blocks until exit)
mos_result_t mos_repl_run(MosReplSession* session);

// Run single REPL iteration (non-blocking)
mos_result_t mos_repl_step(MosReplSession* session);

// Check if REPL should continue
bool mos_repl_running(MosReplSession* session);

// Request REPL exit
void mos_repl_exit(MosReplSession* session);

// -----------------------------------------------------------------------------
// B.11 Convenience Entry Point
// -----------------------------------------------------------------------------

// Start REPL with default settings
mos_result_t mos_repl_start(void);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// The OmniCode REPL provides interactive exploration:
//
// omni> let x = 3^5
// 243
//
// omni> health()
// +87 (SHALEM)
//
// omni> :verse Gen 1:1
// "In the beginning God created the heaven and the earth."
//
// omni> :bible
// [Bible mode enabled - type references directly]
//
// omni> John 1:1
// "In the beginning was the Word..."
//
// Features:
//   - Multi-line block entry with {...}
//   - Command history with up/down arrows
//   - Built-in Bible reference lookup
//   - Health and state monitoring
//   - Module loading and management
//
// "Thy word is a lamp unto my feet, and a light unto my path."
// — Psalm 119:105
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif // MILLENNIUMOS_OMNICODE_REPL_H
