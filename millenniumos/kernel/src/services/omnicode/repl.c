// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-omnicode-repl-impl
// Title:       MillenniumOS OmniCode REPL Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / OmniCode Subsystem
// Role:        Interactive Read-Eval-Print Loop implementation
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

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"

// In freestanding kernel mode (CPISI_MODE 0-1), REPL service is not available.
#if CPISI_MODE >= 2

#include "services/omnicode/repl.h"
#include "services/omnicode/bible.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Session State
// -----------------------------------------------------------------------------

static bool g_repl_running = false;

// -----------------------------------------------------------------------------
// B.2 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_repl_create(MosReplSession* session) {
    if (!session) return MOS_RESULT_ERROR;

    memset(session, 0, sizeof(MosReplSession));

    session->mode = MOS_REPL_MODE_NORMAL;
    session->state = MOS_REPL_STATE_READY;
    session->health = MOS_HEALTH_DEFAULT;
    session->echo = true;
    session->timing = false;
    session->debug = false;
    session->color = true;
    session->bible_hints = true;

    return MOS_RESULT_OK;
}

void mos_repl_destroy(MosReplSession* session) {
    if (!session) return;
    memset(session, 0, sizeof(MosReplSession));
}

void mos_repl_reset(MosReplSession* session) {
    if (!session) return;

    session->input_len = 0;
    session->block_len = 0;
    session->output_len = 0;
    session->mode = MOS_REPL_MODE_NORMAL;
    session->state = MOS_REPL_STATE_READY;
    session->input[0] = '\0';
    session->block_buffer[0] = '\0';
    session->output[0] = '\0';
}

// -----------------------------------------------------------------------------
// B.3 Input Handling
// -----------------------------------------------------------------------------

mos_result_t mos_repl_input_char(MosReplSession* session, char c) {
    if (!session) return MOS_RESULT_ERROR;

    // Handle special characters
    if (c == '\n' || c == '\r') {
        return mos_repl_input_line(session, session->input);
    }

    if (c == '\b' || c == 127) {  // Backspace
        if (session->input_len > 0) {
            session->input[--session->input_len] = '\0';
        }
        return MOS_RESULT_OK;
    }

    if (c == '\t') {
        // TODO: Tab completion
        return MOS_RESULT_OK;
    }

    // Add character to input
    if (session->input_len < MOS_REPL_MAX_INPUT - 1) {
        session->input[session->input_len++] = c;
        session->input[session->input_len] = '\0';
    }

    return MOS_RESULT_OK;
}

mos_result_t mos_repl_input_line(MosReplSession* session, const char* line) {
    if (!session || !line) return MOS_RESULT_ERROR;

    // Copy to input buffer
    strncpy(session->input, line, MOS_REPL_MAX_INPUT - 1);
    session->input[MOS_REPL_MAX_INPUT - 1] = '\0';
    session->input_len = strlen(session->input);

    // Handle block mode
    if (session->mode == MOS_REPL_MODE_BLOCK) {
        // Check for end of block
        if (strcmp(line, "}") == 0) {
            strncat(session->block_buffer, line,
                    MOS_OMNI_MAX_SOURCE - session->block_len - 1);
            session->block_len += strlen(line);
            session->mode = MOS_REPL_MODE_NORMAL;
            // Evaluate the complete block
            strncpy(session->input, session->block_buffer, MOS_REPL_MAX_INPUT);
            session->input_len = strlen(session->input);
            session->block_buffer[0] = '\0';
            session->block_len = 0;
        } else {
            // Add to block buffer
            strncat(session->block_buffer, line,
                    MOS_OMNI_MAX_SOURCE - session->block_len - 1);
            session->block_len += strlen(line);
            strncat(session->block_buffer, "\n",
                    MOS_OMNI_MAX_SOURCE - session->block_len - 1);
            session->block_len++;
            return MOS_RESULT_OK;  // Don't evaluate yet
        }
    }

    // Check for block start
    const char* brace = strchr(line, '{');
    if (brace && !strchr(line, '}')) {
        session->mode = MOS_REPL_MODE_BLOCK;
        strncpy(session->block_buffer, line, MOS_OMNI_MAX_SOURCE - 1);
        session->block_len = strlen(line);
        strncat(session->block_buffer, "\n",
                MOS_OMNI_MAX_SOURCE - session->block_len - 1);
        session->block_len++;
        return MOS_RESULT_OK;
    }

    return mos_repl_eval(session);
}

bool mos_repl_input_complete(MosReplSession* session) {
    if (!session) return true;
    return session->mode == MOS_REPL_MODE_NORMAL;
}

void mos_repl_input_clear(MosReplSession* session) {
    if (!session) return;
    session->input[0] = '\0';
    session->input_len = 0;
}

// -----------------------------------------------------------------------------
// B.4 Command Processing
// -----------------------------------------------------------------------------

bool mos_repl_is_command(const char* input) {
    return input && input[0] == ':';
}

mos_result_t mos_repl_command(MosReplSession* session, const char* command) {
    if (!session || !command) return MOS_RESULT_ERROR;

    // Skip the colon
    const char* cmd = command + 1;

    // Parse command
    if (strncmp(cmd, "help", 4) == 0 || strncmp(cmd, "h", 1) == 0) {
        mos_repl_print(session,
            "OmniCode REPL Commands:\n"
            "  :help, :h       Show this help\n"
            "  :quit, :q       Exit REPL\n"
            "  :clear, :c      Clear screen\n"
            "  :history        Show command history\n"
            "  :debug          Toggle debug mode\n"
            "  :timing         Toggle timing display\n"
            "  :bible          Toggle Bible mode\n"
            "  :verse <ref>    Look up Bible verse\n"
            "  :health         Show runtime health\n"
            "  :state          Show Hebrew state\n"
            "  :modules        List loaded modules\n"
            "  :info           Show session info\n"
            "  :reset          Reset session\n");
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "quit", 4) == 0 || strncmp(cmd, "q", 1) == 0) {
        mos_repl_exit(session);
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "clear", 5) == 0 || strncmp(cmd, "c", 1) == 0) {
        // Clear screen (ANSI escape)
        mos_repl_print(session, "\033[2J\033[H");
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "history", 7) == 0) {
        mos_repl_print(session, "Command History:\n");
        for (mos_size_t i = 0; i < session->history_count; i++) {
            mos_repl_print(session, "  %zu: %s %s\n", i + 1,
                          session->history[i].success ? "✓" : "✗",
                          session->history[i].input);
        }
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "debug", 5) == 0) {
        session->debug = !session->debug;
        mos_repl_print(session, "Debug mode: %s\n",
                      session->debug ? "ON" : "OFF");
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "timing", 6) == 0) {
        session->timing = !session->timing;
        mos_repl_print(session, "Timing display: %s\n",
                      session->timing ? "ON" : "OFF");
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "bible", 5) == 0) {
        mos_repl_bible_mode(session,
                           session->mode != MOS_REPL_MODE_BIBLE);
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "verse ", 6) == 0) {
        return mos_repl_bible_lookup(session, cmd + 6);
    }

    if (strncmp(cmd, "health", 6) == 0) {
        mos_health_t health = mos_omni_health();
        mos_repl_print(session, "Runtime health: %+d\n", health);
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "state", 5) == 0) {
        MosHebrewState state = mos_omni_state();
        const char* names[] = {
            "SHAVAR (broken)", "CHASER (lacking)", "RATSAH (wanting)",
            "YASHAR (even)", "TAMIM (sound)", "SHALEM (whole)", "TOV (perfect)"
        };
        int idx = (state + 3);
        if (idx >= 0 && idx < 7) {
            mos_repl_print(session, "Hebrew state: %s\n", names[idx]);
        }
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "modules", 7) == 0) {
        const MosOmniRuntime* rt = mos_omni_info();
        mos_repl_print(session, "Loaded modules: %zu\n", rt->module_count);
        for (mos_size_t i = 0; i < rt->module_count; i++) {
            if (rt->modules[i]) {
                mos_repl_print(session, "  - %s\n", rt->modules[i]->name);
            }
        }
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "info", 4) == 0) {
        mos_repl_print(session,
            "Session Info:\n"
            "  Lines evaluated: %d\n"
            "  Errors: %d\n"
            "  Commands: %d\n"
            "  Mode: %s\n"
            "  Health: %+d\n",
            (int)session->lines_evaluated,
            (int)session->errors,
            (int)session->commands,
            session->mode == MOS_REPL_MODE_BLOCK ? "BLOCK" :
            session->mode == MOS_REPL_MODE_BIBLE ? "BIBLE" : "NORMAL",
            session->health);
        return MOS_RESULT_OK;
    }

    if (strncmp(cmd, "reset", 5) == 0) {
        mos_repl_reset(session);
        mos_repl_print(session, "Session reset.\n");
        return MOS_RESULT_OK;
    }

    mos_repl_error(session, "Unknown command. Type :help for help.");
    return MOS_RESULT_ERROR;
}

// -----------------------------------------------------------------------------
// B.5 Evaluation
// -----------------------------------------------------------------------------

mos_result_t mos_repl_eval(MosReplSession* session) {
    if (!session || session->input_len == 0) return MOS_RESULT_OK;

    session->state = MOS_REPL_STATE_PROCESSING;
    session->commands++;

    // Check for command
    if (mos_repl_is_command(session->input)) {
        mos_result_t result = mos_repl_command(session, session->input);
        session->state = MOS_REPL_STATE_READY;
        mos_repl_input_clear(session);
        return result;
    }

    // Bible mode - treat input as reference
    if (session->mode == MOS_REPL_MODE_BIBLE) {
        mos_result_t result = mos_repl_bible_lookup(session, session->input);
        session->state = MOS_REPL_STATE_READY;
        mos_repl_input_clear(session);
        return result;
    }

    // Evaluate OmniCode
    trit27_t result = 0;
    mos_result_t eval_result = mos_omni_eval(session->input, session->input_len,
                                              &result);

    // Add to history
    if (session->history_count < MOS_REPL_MAX_HISTORY) {
        MosReplHistoryEntry* entry = &session->history[session->history_count++];
        strncpy(entry->input, session->input, MOS_REPL_MAX_INPUT - 1);
        entry->result = result;
        entry->success = (eval_result == MOS_RESULT_OK);
    }

    session->lines_evaluated++;

    if (eval_result == MOS_RESULT_OK) {
        mos_repl_print(session, "%d\n", (int)result);
        session->state = MOS_REPL_STATE_COMPLETE;
    } else {
        session->errors++;
        mos_repl_error(session, "Evaluation error");
        session->state = MOS_REPL_STATE_ERROR;
    }

    mos_repl_input_clear(session);
    session->state = MOS_REPL_STATE_READY;
    return eval_result;
}

mos_result_t mos_repl_eval_string(MosReplSession* session, const char* code,
                                   trit27_t* result) {
    if (!session || !code) return MOS_RESULT_ERROR;

    strncpy(session->input, code, MOS_REPL_MAX_INPUT - 1);
    session->input_len = strlen(session->input);

    mos_result_t res = mos_omni_eval(code, strlen(code), result);

    mos_repl_input_clear(session);
    return res;
}

// -----------------------------------------------------------------------------
// B.6 Output
// -----------------------------------------------------------------------------

const char* mos_repl_prompt(MosReplSession* session) {
    if (!session) return MOS_REPL_PROMPT_NORMAL;

    switch (session->mode) {
        case MOS_REPL_MODE_BLOCK:
            return MOS_REPL_PROMPT_BLOCK;
        case MOS_REPL_MODE_BIBLE:
            return "bible> ";
        default:
            return MOS_REPL_PROMPT_NORMAL;
    }
}

const char* mos_repl_output(MosReplSession* session) {
    return session ? session->output : "";
}

mos_result_t mos_repl_print(MosReplSession* session, const char* format, ...) {
    if (!session || !format) return MOS_RESULT_ERROR;

    va_list args;
    va_start(args, format);

    int written = vsnprintf(session->output + session->output_len,
                            MOS_REPL_MAX_OUTPUT - session->output_len,
                            format, args);

    va_end(args);

    if (written > 0) {
        session->output_len += written;
    }

    // Also print to console
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    return MOS_RESULT_OK;
}

mos_result_t mos_repl_error(MosReplSession* session, const char* message) {
    if (!session || !message) return MOS_RESULT_ERROR;

    if (session->color) {
        mos_repl_print(session, "\033[31mError: %s\033[0m\n", message);
    } else {
        mos_repl_print(session, "Error: %s\n", message);
    }

    session->health -= 5;
    return MOS_RESULT_OK;
}

// -----------------------------------------------------------------------------
// B.7 History Navigation
// -----------------------------------------------------------------------------

mos_result_t mos_repl_history_up(MosReplSession* session) {
    if (!session || session->history_count == 0) return MOS_RESULT_ERROR;

    if (session->history_pos > 0) {
        session->history_pos--;
    }

    strncpy(session->input,
            session->history[session->history_pos].input,
            MOS_REPL_MAX_INPUT - 1);
    session->input_len = strlen(session->input);

    return MOS_RESULT_OK;
}

mos_result_t mos_repl_history_down(MosReplSession* session) {
    if (!session) return MOS_RESULT_ERROR;

    if (session->history_pos < session->history_count - 1) {
        session->history_pos++;
        strncpy(session->input,
                session->history[session->history_pos].input,
                MOS_REPL_MAX_INPUT - 1);
        session->input_len = strlen(session->input);
    } else {
        mos_repl_input_clear(session);
        session->history_pos = session->history_count;
    }

    return MOS_RESULT_OK;
}

mos_result_t mos_repl_history_search(MosReplSession* session,
                                      const char* prefix) {
    if (!session || !prefix) return MOS_RESULT_ERROR;

    size_t prefix_len = strlen(prefix);
    for (mos_size_t i = session->history_count; i > 0; i--) {
        if (strncmp(session->history[i - 1].input, prefix, prefix_len) == 0) {
            strncpy(session->input, session->history[i - 1].input,
                    MOS_REPL_MAX_INPUT - 1);
            session->input_len = strlen(session->input);
            session->history_pos = i - 1;
            return MOS_RESULT_OK;
        }
    }

    return MOS_RESULT_ERROR;
}

void mos_repl_history_clear(MosReplSession* session) {
    if (!session) return;
    session->history_count = 0;
    session->history_pos = 0;
}

// -----------------------------------------------------------------------------
// B.8 Bible Integration
// -----------------------------------------------------------------------------

void mos_repl_bible_mode(MosReplSession* session, bool enable) {
    if (!session) return;

    if (enable) {
        session->mode = MOS_REPL_MODE_BIBLE;
        mos_repl_print(session,
            "Bible mode enabled. Type references directly (e.g., John 3:16)\n"
            "Type :bible again to exit Bible mode.\n");
    } else {
        session->mode = MOS_REPL_MODE_NORMAL;
        mos_repl_print(session, "Bible mode disabled.\n");
    }
}

mos_result_t mos_repl_bible_lookup(MosReplSession* session,
                                    const char* reference) {
    if (!session || !reference) return MOS_RESULT_ERROR;

    char text[MOS_BIBLE_MAX_VERSE];
    mos_result_t result = mos_bible_lookup(reference, text, sizeof(text));

    if (result == MOS_RESULT_OK) {
        if (session->color) {
            mos_repl_print(session, "\033[36m%s\033[0m\n", text);
        } else {
            mos_repl_print(session, "%s\n", text);
        }
    } else {
        mos_repl_error(session, "Verse not found");
    }

    return result;
}

mos_result_t mos_repl_bible_hint(MosReplSession* session) {
    if (!session || !session->bible_hints) return MOS_RESULT_OK;

    MosBibleVerse verse;
    mos_result_t result = mos_bible_health_verse(session->health, &verse);

    if (result == MOS_RESULT_OK) {
        char ref[MOS_BIBLE_MAX_REF];
        mos_bible_format_ref(&verse.ref, ref, sizeof(ref));
        if (session->color) {
            mos_repl_print(session, "\033[90m💡 %s - \"%s\"\033[0m\n",
                          ref, verse.text);
        } else {
            mos_repl_print(session, "Hint: %s - \"%s\"\n", ref, verse.text);
        }
    }

    return result;
}

// -----------------------------------------------------------------------------
// B.9 Main Loop
// -----------------------------------------------------------------------------

bool mos_repl_running(MosReplSession* session) {
    (void)session;
    return g_repl_running;
}

void mos_repl_exit(MosReplSession* session) {
    (void)session;
    g_repl_running = false;
}

mos_result_t mos_repl_step(MosReplSession* session) {
    if (!session) return MOS_RESULT_ERROR;

    // Print prompt
    printf("%s", mos_repl_prompt(session));
    fflush(stdout);

    // Read line
    char line[MOS_REPL_MAX_INPUT];
    if (!fgets(line, sizeof(line), stdin)) {
        return MOS_RESULT_ERROR;
    }

    // Remove trailing newline
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    // Process input
    return mos_repl_input_line(session, line);
}

mos_result_t mos_repl_run(MosReplSession* session) {
    if (!session) return MOS_RESULT_ERROR;

    g_repl_running = true;

    // Print banner
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║           OmniCode REPL - MillenniumOS Runtime               ║\n");
    printf("║                                                              ║\n");
    printf("║   \"In the beginning was the Word\" — John 1:1                 ║\n");
    printf("║                                                              ║\n");
    printf("║   Type :help for commands, :quit to exit                     ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    while (g_repl_running) {
        mos_result_t result = mos_repl_step(session);
        if (result < 0 && result != MOS_RESULT_ERROR) {
            break;  // Fatal error
        }
    }

    printf("\nGoodbye!\n");
    return MOS_RESULT_OK;
}

mos_result_t mos_repl_start(void) {
    MosReplSession session;
    mos_result_t result;

    // Initialize OmniCode runtime
    result = mos_omni_init();
    if (result != MOS_RESULT_OK) {
        fprintf(stderr, "Failed to initialize OmniCode runtime\n");
        return result;
    }

    // Create session
    result = mos_repl_create(&session);
    if (result != MOS_RESULT_OK) {
        fprintf(stderr, "Failed to create REPL session\n");
        return result;
    }

    // Run REPL
    result = mos_repl_run(&session);

    // Cleanup
    mos_repl_destroy(&session);
    mos_omni_shutdown();

    return result;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// OmniCode REPL provides interactive exploration:
//
//   omni> let x = 3 * 3 * 3
//   27
//
//   omni> :health
//   Runtime health: +87
//
//   omni> :bible
//   Bible mode enabled.
//
//   bible> John 1:1
//   "In the beginning was the Word..."
//
// The REPL serves as:
//   - Learning environment for OmniCode
//   - Quick testing and prototyping
//   - Bible reference lookup
//   - Runtime health monitoring
//   - Module exploration
//
// "Thy word is a lamp unto my feet, and a light unto my path."
// — Psalm 119:105
//
// =============================================================================
// END CLOSING
// =============================================================================

#endif /* CPISI_MODE >= 2 - Full REPL service implementation */
