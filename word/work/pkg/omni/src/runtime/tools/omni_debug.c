// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-debug-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_debug.c — OmniCode Debug & Logging Implementation
// Structured logging, instruction tracing, and stack inspection.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-debug-impl
// Title: OmniCode Debug & Logging Implementation
// Type: Source
// Role: Implement structured logging and debugging for VM and DAR

// # M.8 Grounding [GROUNDING]
//
// Scripture: Proverbs 4:26 - "Ponder the path of thy feet"
// Principle: Visibility enables correction - we must see to fix
// Anchor: Psalm 139:23 - "Search me, O God, and know my heart"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "omni_ir.h"
#include "omni_vm.h"
#include "omni_debug.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Debug Context Lifecycle [LIFECYCLE]

OmniDebug* omni_debug_create(void) {
    OmniDebug* dbg = calloc(1, sizeof(OmniDebug));
    if (!dbg) return NULL;

    omni_debug_init(dbg);
    return dbg;
}

void omni_debug_init(OmniDebug* dbg) {
    if (!dbg) return;

    dbg->level = OMNI_LOG_INFO;
    dbg->flags = OMNI_DBG_NONE;
    dbg->log_file = NULL;
    dbg->console = stderr;
    dbg->enabled = true;
    dbg->message_count = 0;
    dbg->show_timestamp = true;
    dbg->show_level = true;
    dbg->show_location = false;
    dbg->trace_start = 0;
    dbg->trace_end = 0;
}

void omni_debug_destroy(OmniDebug* dbg) {
    if (!dbg) return;

    if (dbg->log_file && dbg->log_file != stdout && dbg->log_file != stderr) {
        fclose(dbg->log_file);
    }

    free(dbg);
}

// # B.2 Configuration [CONFIG]

void omni_debug_set_level(OmniDebug* dbg, OmniLogLevel level) {
    if (!dbg) return;
    dbg->level = level;
}

void omni_debug_set_flags(OmniDebug* dbg, uint8_t flags) {
    if (!dbg) return;
    dbg->flags = flags;
}

void omni_debug_enable(OmniDebug* dbg, bool enabled) {
    if (!dbg) return;
    dbg->enabled = enabled;
}

bool omni_debug_set_file(OmniDebug* dbg, const char* path) {
    if (!dbg) return false;

    // Close existing file if open
    if (dbg->log_file && dbg->log_file != stdout && dbg->log_file != stderr) {
        fclose(dbg->log_file);
        dbg->log_file = NULL;
    }

    if (!path) return true;  // NULL disables file logging

    dbg->log_file = fopen(path, "a");
    return dbg->log_file != NULL;
}

void omni_debug_set_console(OmniDebug* dbg, FILE* console) {
    if (!dbg) return;
    dbg->console = console;
}

void omni_debug_set_trace_range(OmniDebug* dbg, uint32_t start, uint32_t end) {
    if (!dbg) return;
    dbg->trace_start = start;
    dbg->trace_end = end;
}

// # B.3 Logging Functions [LOG]

static const char* level_prefixes[] = {
    "[DEBUG]",
    "[INFO ]",
    "[WARN ]",
    "[ERROR]",
    ""
};

static const char* level_colors[] = {
    "\033[36m",     // Cyan for DEBUG
    "\033[32m",     // Green for INFO
    "\033[33m",     // Yellow for WARN
    "\033[31m",     // Red for ERROR
    ""
};

static const char* color_reset = "\033[0m";

void omni_debug_log(OmniDebug* dbg, OmniLogLevel level,
                    const char* file, int line, const char* func,
                    const char* fmt, ...) {
    if (!dbg || !dbg->enabled) return;
    if (level < dbg->level) return;

    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    char timestamp[32] = "";
    if (dbg->show_timestamp) {
        time_t now = time(NULL);
        struct tm* tm = localtime(&now);
        strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm);
    }

    // Format location
    char location[256] = "";
    if (dbg->show_location && file) {
        const char* basename = strrchr(file, '/');
        basename = basename ? basename + 1 : file;
        snprintf(location, sizeof(location), " [%s:%d %s]", basename, line, func);
    }

    // Write to console
    if (dbg->console) {
        if (dbg->show_timestamp) {
            fprintf(dbg->console, "%s ", timestamp);
        }
        if (dbg->show_level) {
            fprintf(dbg->console, "%s%s%s ", level_colors[level], level_prefixes[level], color_reset);
        }
        fprintf(dbg->console, "%s%s\n", message, location);
    }

    // Write to file (no colors)
    if (dbg->log_file) {
        if (dbg->show_timestamp) {
            fprintf(dbg->log_file, "%s ", timestamp);
        }
        if (dbg->show_level) {
            fprintf(dbg->log_file, "%s ", level_prefixes[level]);
        }
        fprintf(dbg->log_file, "%s%s\n", message, location);
    }

    dbg->message_count++;
}

// # B.4 Instruction Tracing [TRACE]

void omni_debug_trace_instruction(OmniDebug* dbg, OmniVM* vm, uint32_t ip) {
    if (!dbg || !dbg->enabled) return;
    if (!(dbg->flags & OMNI_DBG_TRACE)) return;
    if (!vm || !vm->program) return;

    // Check trace range
    if (dbg->trace_end > 0 && (ip < dbg->trace_start || ip > dbg->trace_end)) {
        return;
    }

    if (ip >= vm->program->code_length) return;

    OmniInstruction* inst = &vm->program->code[ip];
    const char* name = omni_opcode_name(inst->opcode);

    // Format operands
    char operands[64] = "";
    switch (inst->opcode) {
        case OP_PUSH_CONST:
        case OP_STORE_GLOBAL:
        case OP_PUSH_GLOBAL:
        case OP_STORE_LOCAL:
        case OP_PUSH_LOCAL:
        case OP_WITNESS:
        case OP_FAIL:
            snprintf(operands, sizeof(operands), " %u", inst->operand1);
            break;

        case OP_JUMP:
        case OP_JUMP_POS:
        case OP_JUMP_NEG:
        case OP_JUMP_ZERO:
            snprintf(operands, sizeof(operands), " -> %u", inst->operand1);
            break;

        case OP_CALL:
            snprintf(operands, sizeof(operands), " func[%u] args=%u", inst->operand1, inst->operand2);
            break;

        default:
            break;
    }

    OMNI_DEBUG(dbg, "  %04u: %-12s%s  [sp=%u health=%d]",
               ip, name, operands, vm->sp, vm->health);
}

void omni_debug_trace_pre(OmniDebug* dbg, OmniVM* vm) {
    if (!dbg || !dbg->enabled) return;
    if (!(dbg->flags & OMNI_DBG_TRACE)) return;

    OMNI_DEBUG(dbg, ">>> Execution start: ip=%u sp=%u", vm->ip, vm->sp);
}

void omni_debug_trace_post(OmniDebug* dbg, OmniVM* vm) {
    if (!dbg || !dbg->enabled) return;
    if (!(dbg->flags & OMNI_DBG_TRACE)) return;

    OMNI_DEBUG(dbg, "<<< Execution end: ip=%u sp=%u result=%d", vm->ip, vm->sp, vm->state);
}

// # B.5 Stack Inspection [STACK]

static const char* value_type_name(OmniValueType type) {
    switch (type) {
        case VAL_NONE:   return "none";
        case VAL_INT:    return "int";
        case VAL_FLOAT:  return "float";
        case VAL_BOOL:   return "bool";
        case VAL_STRING: return "string";
        case VAL_TRIT:   return "trit";
        case VAL_FUNC:   return "func";
        case VAL_LIST:   return "list";
        case VAL_MAP:    return "map";
        case VAL_FILE:   return "file";
        default:         return "?";
    }
}

static void format_value(char* buf, size_t size, OmniValue* val) {
    switch (val->type) {
        case VAL_NONE:
            snprintf(buf, size, "none");
            break;
        case VAL_INT:
            snprintf(buf, size, "%lld", (long long)val->as_int);
            break;
        case VAL_FLOAT:
            snprintf(buf, size, "%.4f", val->as_float);
            break;
        case VAL_BOOL:
            snprintf(buf, size, "%s", val->as_bool ? "true" : "false");
            break;
        case VAL_STRING:
            snprintf(buf, size, "str[%u]", val->as_string);
            break;
        case VAL_TRIT:
            snprintf(buf, size, "%+d", val->as_trit);
            break;
        case VAL_FUNC:
            snprintf(buf, size, "func[%u]", val->as_func);
            break;
        case VAL_LIST:
            snprintf(buf, size, "list@%p", val->as_list);
            break;
        case VAL_MAP:
            snprintf(buf, size, "map@%p", val->as_map);
            break;
        case VAL_FILE:
            snprintf(buf, size, "file@%p", val->as_file);
            break;
        default:
            snprintf(buf, size, "?");
    }
}

void omni_debug_dump_stack(OmniDebug* dbg, OmniVM* vm) {
    if (!dbg || !dbg->enabled || !vm) return;
    if (!(dbg->flags & OMNI_DBG_STACK)) return;

    OMNI_DEBUG(dbg, "=== Stack Dump (sp=%u) ===", vm->sp);

    if (vm->sp == 0) {
        OMNI_DEBUG(dbg, "%s", "  (empty)");
        return;
    }

    for (int i = vm->sp - 1; i >= 0; i--) {
        char buf[128];
        format_value(buf, sizeof(buf), &vm->stack[i]);
        OMNI_DEBUG(dbg, "  [%3d] %s (%s)", i, buf, value_type_name(vm->stack[i].type));
    }

    OMNI_DEBUG(dbg, "%s", "=== End Stack ===");
}

void omni_debug_dump_stack_top(OmniDebug* dbg, OmniVM* vm, int count) {
    if (!dbg || !dbg->enabled || !vm) return;
    if (!(dbg->flags & OMNI_DBG_STACK)) return;

    int start = vm->sp - 1;
    int end = start - count + 1;
    if (end < 0) end = 0;

    OMNI_DEBUG(dbg, "--- Stack Top %d (sp=%u) ---", count, vm->sp);

    for (int i = start; i >= end; i--) {
        char buf[128];
        format_value(buf, sizeof(buf), &vm->stack[i]);
        OMNI_DEBUG(dbg, "  [%3d] %s", i, buf);
    }
}

void omni_debug_stack_push(OmniDebug* dbg, const char* value_str) {
    if (!dbg || !dbg->enabled) return;
    if (!(dbg->flags & OMNI_DBG_STACK)) return;

    OMNI_DEBUG(dbg, "  PUSH: %s", value_str);
}

void omni_debug_stack_pop(OmniDebug* dbg, const char* value_str) {
    if (!dbg || !dbg->enabled) return;
    if (!(dbg->flags & OMNI_DBG_STACK)) return;

    OMNI_DEBUG(dbg, "  POP: %s", value_str);
}

// # B.6 Health & State Tracking [STATE]

static const char* hebrew_state_names[] = {
    "shavar (broken)",
    "chaser (lacking)",
    "ratsah (wanting)",
    "yashar (even)",
    "tamim (sound)",
    "shalem (whole)",
    "tov (perfect)"
};

static const char* get_hebrew_name(int8_t state) {
    int idx = state + 3;  // -3 to +3 → 0 to 6
    if (idx < 0) idx = 0;
    if (idx > 6) idx = 6;
    return hebrew_state_names[idx];
}

void omni_debug_health_change(OmniDebug* dbg, int8_t old_health, int8_t new_health, const char* reason) {
    if (!dbg || !dbg->enabled) return;
    if (!(dbg->flags & OMNI_DBG_HEALTH)) return;

    int delta = new_health - old_health;
    const char* direction = delta > 0 ? "↑" : (delta < 0 ? "↓" : "=");

    OMNI_INFO(dbg, "Health %s %d → %d (%+d): %s",
              direction, old_health, new_health, delta, reason ? reason : "");
}

void omni_debug_hebrew_transition(OmniDebug* dbg, int8_t old_state, int8_t new_state, int8_t k_factor) {
    if (!dbg || !dbg->enabled) return;
    if (!(dbg->flags & OMNI_DBG_HEBREW)) return;

    const char* k_str = k_factor > 0 ? "+1 (expand)" : (k_factor < 0 ? "-1 (retreat)" : "0 (hold)");

    OMNI_INFO(dbg, "Hebrew: %s → %s [k=%s]",
              get_hebrew_name(old_state), get_hebrew_name(new_state), k_str);
}

void omni_debug_checkpoint(OmniDebug* dbg, const char* operation, int index, const char* name) {
    if (!dbg || !dbg->enabled) return;
    if (!(dbg->flags & OMNI_DBG_CHECKPOINT)) return;

    OMNI_INFO(dbg, "Checkpoint %s: [%d] %s", operation, index, name ? name : "(unnamed)");
}

// # B.7 Utilities [UTIL]

const char* omni_debug_level_name(OmniLogLevel level) {
    switch (level) {
        case OMNI_LOG_DEBUG: return "DEBUG";
        case OMNI_LOG_INFO:  return "INFO";
        case OMNI_LOG_WARN:  return "WARN";
        case OMNI_LOG_ERROR: return "ERROR";
        case OMNI_LOG_NONE:  return "NONE";
        default:             return "?";
    }
}

const char* omni_debug_flag_name(uint8_t flag) {
    switch (flag) {
        case OMNI_DBG_TRACE:      return "TRACE";
        case OMNI_DBG_STACK:      return "STACK";
        case OMNI_DBG_HEALTH:     return "HEALTH";
        case OMNI_DBG_HEBREW:     return "HEBREW";
        case OMNI_DBG_CHECKPOINT: return "CHECKPOINT";
        case OMNI_DBG_MEMORY:     return "MEMORY";
        default:                  return "?";
    }
}

bool omni_debug_has_flag(OmniDebug* dbg, uint8_t flag) {
    if (!dbg) return false;
    return (dbg->flags & flag) != 0;
}

bool omni_debug_should_log(OmniDebug* dbg, OmniLogLevel level) {
    if (!dbg || !dbg->enabled) return false;
    return level >= dbg->level;
}

void omni_debug_flush(OmniDebug* dbg) {
    if (!dbg) return;

    if (dbg->console) fflush(dbg->console);
    if (dbg->log_file) fflush(dbg->log_file);
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "Ponder the path of thy feet, and let all thy ways be established."
//   — Proverbs 4:26
//
// Visibility enables correction. We see clearly to walk rightly.

// =============================================================================
// END CLOSING
// =============================================================================
