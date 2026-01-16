// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-debug
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_debug.h — OmniCode Debug & Logging System
// Structured logging, instruction tracing, and stack inspection.

#ifndef BERESHIT_OMNI_DEBUG_H
#define BERESHIT_OMNI_DEBUG_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-debug
// Title: OmniCode Debug & Logging System
// Type: Header
// Role: Provide structured logging and debugging for VM and DAR

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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "omni_vm.h"

// # S.1 Debug Levels [LEVELS]
//
// Log levels follow severity ordering (higher = more severe)

typedef enum {
    OMNI_LOG_DEBUG = 0,     // Detailed debugging information
    OMNI_LOG_INFO  = 1,     // General operational messages
    OMNI_LOG_WARN  = 2,     // Warning conditions
    OMNI_LOG_ERROR = 3,     // Error conditions
    OMNI_LOG_NONE  = 4      // Disable all logging
} OmniLogLevel;

// # S.2 Debug Flags [FLAGS]
//
// Bit flags for enabling specific debug features

#define OMNI_DBG_NONE         0x00    // No debugging
#define OMNI_DBG_TRACE        0x01    // Instruction tracing
#define OMNI_DBG_STACK        0x02    // Stack operations
#define OMNI_DBG_HEALTH       0x04    // Health changes
#define OMNI_DBG_HEBREW       0x08    // Hebrew state transitions
#define OMNI_DBG_CHECKPOINT   0x10    // Checkpoint operations
#define OMNI_DBG_MEMORY       0x20    // Memory operations
#define OMNI_DBG_ALL          0xFF    // All debugging

// # S.3 Debug Context [CONTEXT]

typedef struct {
    // Log level filtering
    OmniLogLevel level;

    // Debug feature flags
    uint8_t flags;

    // Output destinations
    FILE* log_file;         // File output (NULL = disabled)
    FILE* console;          // Console output (NULL = disabled)

    // State
    bool enabled;
    uint32_t message_count;

    // Formatting
    bool show_timestamp;
    bool show_level;
    bool show_location;

    // Instruction trace state
    uint32_t trace_start;   // Start IP for trace range
    uint32_t trace_end;     // End IP for trace range (0 = no limit)
} OmniDebug;

// # S.4 Log Entry [ENTRY]

typedef struct {
    OmniLogLevel level;
    const char* file;
    int line;
    const char* func;
    const char* message;
    uint64_t timestamp;
} OmniLogEntry;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Debug Context Lifecycle [LIFECYCLE]

// Create debug context with default settings
OmniDebug* omni_debug_create(void);

// Initialize existing debug context
void omni_debug_init(OmniDebug* dbg);

// Destroy debug context (closes log file if open)
void omni_debug_destroy(OmniDebug* dbg);

// # B.2 Configuration [CONFIG]

// Set minimum log level
void omni_debug_set_level(OmniDebug* dbg, OmniLogLevel level);

// Set debug flags
void omni_debug_set_flags(OmniDebug* dbg, uint8_t flags);

// Enable/disable debug output
void omni_debug_enable(OmniDebug* dbg, bool enabled);

// Set log file (NULL to disable file logging)
bool omni_debug_set_file(OmniDebug* dbg, const char* path);

// Set console output (NULL to disable, defaults to stderr)
void omni_debug_set_console(OmniDebug* dbg, FILE* console);

// Set trace range (0,0 = all instructions)
void omni_debug_set_trace_range(OmniDebug* dbg, uint32_t start, uint32_t end);

// # B.3 Logging Functions [LOG]

// Core logging function (use macros below instead)
void omni_debug_log(OmniDebug* dbg, OmniLogLevel level,
                    const char* file, int line, const char* func,
                    const char* fmt, ...);

// Convenience macros for logging
#define OMNI_DEBUG(dbg, fmt, ...) \
    omni_debug_log(dbg, OMNI_LOG_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define OMNI_INFO(dbg, fmt, ...) \
    omni_debug_log(dbg, OMNI_LOG_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define OMNI_WARN(dbg, fmt, ...) \
    omni_debug_log(dbg, OMNI_LOG_WARN, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define OMNI_ERROR(dbg, fmt, ...) \
    omni_debug_log(dbg, OMNI_LOG_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

// # B.4 Instruction Tracing [TRACE]

// Trace single instruction execution
void omni_debug_trace_instruction(OmniDebug* dbg, OmniVM* vm, uint32_t ip);

// Trace before/after instruction execution
void omni_debug_trace_pre(OmniDebug* dbg, OmniVM* vm);
void omni_debug_trace_post(OmniDebug* dbg, OmniVM* vm);

// # B.5 Stack Inspection [STACK]

// Dump current stack state
void omni_debug_dump_stack(OmniDebug* dbg, OmniVM* vm);

// Dump top N stack values
void omni_debug_dump_stack_top(OmniDebug* dbg, OmniVM* vm, int count);

// Log stack push/pop operations
void omni_debug_stack_push(OmniDebug* dbg, const char* value_str);
void omni_debug_stack_pop(OmniDebug* dbg, const char* value_str);

// # B.6 Health & State Tracking [STATE]

// Log health change
void omni_debug_health_change(OmniDebug* dbg, int8_t old_health, int8_t new_health, const char* reason);

// Log Hebrew state transition
void omni_debug_hebrew_transition(OmniDebug* dbg, int8_t old_state, int8_t new_state, int8_t k_factor);

// Log checkpoint operation
void omni_debug_checkpoint(OmniDebug* dbg, const char* operation, int index, const char* name);

// # B.7 Utilities [UTIL]

// Get level name string
const char* omni_debug_level_name(OmniLogLevel level);

// Get flag name string
const char* omni_debug_flag_name(uint8_t flag);

// Check if flag is enabled
bool omni_debug_has_flag(OmniDebug* dbg, uint8_t flag);

// Check if level passes filter
bool omni_debug_should_log(OmniDebug* dbg, OmniLogLevel level);

// Flush log output
void omni_debug_flush(OmniDebug* dbg);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Example
//
//   // Create debug context
//   OmniDebug* dbg = omni_debug_create();
//
//   // Configure
//   omni_debug_set_level(dbg, OMNI_LOG_DEBUG);
//   omni_debug_set_flags(dbg, OMNI_DBG_TRACE | OMNI_DBG_STACK);
//   omni_debug_set_file(dbg, "vm.log");
//
//   // Use logging
//   OMNI_INFO(dbg, "Starting VM execution");
//   OMNI_DEBUG(dbg, "Health changed: %d -> %d", old, new);
//
//   // Stack inspection
//   omni_debug_dump_stack(dbg, vm);
//
//   // Cleanup
//   omni_debug_destroy(dbg);

// # X.2 Closing Note
//
// "Ponder the path of thy feet, and let all thy ways be established."
//   — Proverbs 4:26
//
// Visibility enables correction. We must see clearly to walk rightly.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_DEBUG_H
