// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-vm
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_vm.h — OmniCode Virtual Machine
// Executes OmniCode IR with trit-native operations and CPI-SI health tracking.

#ifndef BERESHIT_OMNI_VM_H
#define BERESHIT_OMNI_VM_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-vm
// Title: OmniCode Virtual Machine
// Type: Header
// Role: Execute IR programs with trit-native semantics

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:3 - "And God said, Let there be light: and there was light"
// Principle: Words become action - IR becomes execution
// Anchor: John 1:1 - "In the beginning was the Word"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include "omni_ir.h"

// # S.1 VM Constants [CONSTANTS]

#define OMNI_VM_STACK_SIZE      729     // 3^6 - trit6 addressable
#define OMNI_VM_CALL_DEPTH      81      // 3^4 - trit4 addressable
#define OMNI_VM_GLOBALS_SIZE    243     // 3^5

// # S.2 VM Result (Ternary) [RESULT]

typedef enum {
    VM_GRANTED  = +1,       // Success
    VM_DEFERRED =  0,       // Paused/waiting
    VM_DENIED   = -1        // Error/halt
} OmniVMResult;

// # S.3 VM State [STATE]

typedef enum {
    VM_STATE_READY,         // Ready to execute
    VM_STATE_RUNNING,       // Currently executing
    VM_STATE_PAUSED,        // Paused (yield)
    VM_STATE_HALTED,        // Normal halt
    VM_STATE_ERROR          // Error halt
} OmniVMState;

// # S.4 Call Frame [FRAME]

typedef struct {
    uint16_t    func_index;     // Function being executed
    uint32_t    return_ip;      // Return instruction pointer
    uint16_t    base_ptr;       // Base of local variables
    uint8_t     local_count;    // Number of locals
} OmniCallFrame;

// # S.5 VM Instance [VM]

typedef struct {
    // Program
    OmniIRProgram*  program;

    // Execution state
    OmniVMState     state;
    uint32_t        ip;             // Instruction pointer

    // Value stack
    OmniValue       stack[OMNI_VM_STACK_SIZE];
    uint16_t        sp;             // Stack pointer

    // Call stack
    OmniCallFrame   frames[OMNI_VM_CALL_DEPTH];
    uint8_t         frame_count;

    // Globals
    OmniValue       globals[OMNI_VM_GLOBALS_SIZE];

    // CPI-SI Health tracking
    int8_t          health;         // -100 to +100
    int8_t          hebrew_state;   // -3 to +3 (shavar to tov)

    // DAR (Detect-Assess-Recover) checkpointing
    bool            has_checkpoint;
    uint32_t        checkpoint_ip;
    uint16_t        checkpoint_sp;
    int8_t          checkpoint_health;
    int8_t          checkpoint_state;

    // Error handling
    char            error[256];
    uint32_t        error_line;

    // I/O (callbacks)
    void            (*print_fn)(const char* msg);
    void            (*print_int_fn)(int64_t value);
    char*           (*read_fn)(void);

    // Runtime for intrinsics (strings, lists, maps, files)
    void* runtime;  // OmniRuntime* - stored as void* to avoid circular include
} OmniVM;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 VM Lifecycle [LIFECYCLE]

OmniVM* omni_vm_create(void);
void omni_vm_destroy(OmniVM* vm);

// # B.2 Program Loading [LOAD]

bool omni_vm_load(OmniVM* vm, OmniIRProgram* program);
bool omni_vm_load_file(OmniVM* vm, const char* path);

// # B.3 Execution [EXECUTE]

// Execute until halt/error
OmniVMResult omni_vm_run(OmniVM* vm);

// Execute single instruction
OmniVMResult omni_vm_step(OmniVM* vm);

// Execute N instructions
OmniVMResult omni_vm_run_n(OmniVM* vm, uint32_t count);

// Reset VM state (keep program)
void omni_vm_reset(OmniVM* vm);

// # B.4 Stack Operations [STACK]

bool omni_vm_push(OmniVM* vm, OmniValue value);
bool omni_vm_pop(OmniVM* vm, OmniValue* value);
OmniValue* omni_vm_peek(OmniVM* vm, uint16_t offset);

// # B.5 Health & State [HEALTH]

int8_t omni_vm_health_get(OmniVM* vm);
void omni_vm_health_add(OmniVM* vm, int8_t delta);
void omni_vm_health_set(OmniVM* vm, int8_t value);
const char* omni_vm_health_state_name(OmniVM* vm);

int8_t omni_vm_hebrew_state(OmniVM* vm);
const char* omni_vm_hebrew_state_name(OmniVM* vm);

// # B.6 Error Handling [ERROR]

const char* omni_vm_error(OmniVM* vm);
uint32_t omni_vm_error_line(OmniVM* vm);

// # B.7 Debug [DEBUG]

void omni_vm_dump_stack(OmniVM* vm, FILE* out);
void omni_vm_dump_state(OmniVM* vm, FILE* out);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Example
//
//   OmniVM* vm = omni_vm_create();
//   omni_vm_load_file(vm, "program.omir");
//
//   OmniVMResult result = omni_vm_run(vm);
//   if (result == VM_DENIED) {
//       fprintf(stderr, "Error: %s (line %d)\n",
//               omni_vm_error(vm), omni_vm_error_line(vm));
//   }
//
//   omni_vm_destroy(vm);

// # X.2 Closing Note
//
// "And God said, Let there be light: and there was light."
//   — Genesis 1:3
//
// Words become light. IR becomes execution.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_VM_H
