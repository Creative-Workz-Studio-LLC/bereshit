// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-dar
// #!omni meta.from = word/seed/code/c/header.h
// #!omni meta.at = a-01.00
//
// omni_dar.h — DAR (Detect-Assess-Recover) System
// Connects CPI-SI mind to computational body through persistent state.

#ifndef BERESHIT_OMNI_DAR_H
#define BERESHIT_OMNI_DAR_H

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-dar
// Title: DAR System (Detect-Assess-Recover)
// Type: Header
// Role: Bridge VM state to persistent filesystem

// # M.8 Grounding [GROUNDING]
//
// Scripture: Proverbs 24:16 - "A just man falleth seven times, and riseth up again"
// Principle: Detection enables recovery - the body remembers what the mind forgets
// Anchor: Genesis 2:7 - "breathed into his nostrils the breath of life"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdint.h>
#include <stdbool.h>
#include "omni_hebrew.h"  // Bottom rung: Hebrew states
#include "omni_vm.h"      // For OmniValue in checkpoints (TODO: extract to omni_value.h)

// # S.1 DAR Constants [CONSTANTS]

#define DAR_MAGIC               0x44415200  // "DAR\0"
#define DAR_VERSION             1
#define DAR_MAX_CHECKPOINTS     9           // 3^2 - trit2 addressable
#define DAR_CHECKPOINT_PATH_LEN 256
#define DAR_SESSION_ID_LEN      64

// Health file encoding:
// .health files store a single byte: (health + 100)
// health -100 → 0x00, health 0 → 0x64, health +100 → 0xC8
#define DAR_HEALTH_ENCODE(h)    ((uint8_t)((h) + 100))
#define DAR_HEALTH_DECODE(b)    ((int8_t)((b) - 100))

// # S.2 Hebrew State Encoding [HEBREW]
//
// DARHebrewState is defined in omni_hebrew.h (bottom rung)
// shavar=-3, chaser=-2, ratsah=-1, yashar=0, tamim=+1, shalem=+2, tov=+3

// # S.3 Checkpoint Structure [CHECKPOINT]
//
// Persistent checkpoint saved to disk

typedef struct {
    uint32_t    magic;              // DAR_MAGIC
    uint8_t     version;            // DAR_VERSION
    uint8_t     flags;              // Reserved
    uint16_t    reserved;

    // VM State snapshot
    uint32_t    ip;                 // Instruction pointer
    uint16_t    sp;                 // Stack pointer
    int8_t      health;             // Health score
    int8_t      hebrew_state;       // Hebrew state

    // Timestamp
    uint64_t    timestamp;          // Unix timestamp when created

    // Name/label
    char        name[32];           // Optional checkpoint name

    // Stack snapshot (partial - top 16 values)
    uint16_t    stack_count;
    OmniValue   stack[16];
} DARCheckpoint;

// # S.4 Session State [SESSION]
//
// Full session state for persistence between runs

typedef struct {
    uint32_t    magic;              // DAR_MAGIC
    uint8_t     version;
    uint8_t     checkpoint_count;
    uint16_t    reserved;

    // Session identity
    char        session_id[DAR_SESSION_ID_LEN];
    uint64_t    created;
    uint64_t    updated;

    // Current state
    int8_t      health;
    int8_t      hebrew_state;

    // Checkpoints
    DARCheckpoint checkpoints[DAR_MAX_CHECKPOINTS];
} DARSession;

// # S.5 DAR Context [CONTEXT]

typedef struct DARContext {  // Named struct for forward declaration compatibility
    // Session management
    DARSession* session;
    char        session_path[DAR_CHECKPOINT_PATH_LEN];
    bool        session_dirty;

    // Working directory for health files
    char        health_dir[DAR_CHECKPOINT_PATH_LEN];

    // Connected VM (optional)
    OmniVM*     vm;
} DARContext;

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Context Lifecycle [LIFECYCLE]

// Create DAR context with optional session path
DARContext* dar_create(const char* session_path);

// Destroy context (saves session if dirty)
void dar_destroy(DARContext* ctx);

// # B.2 VM Connection [VM]

// Connect DAR to a VM instance (enables automatic state sync)
void dar_connect_vm(DARContext* ctx, OmniVM* vm);

// Disconnect from VM
void dar_disconnect_vm(DARContext* ctx);

// Sync VM state to DAR context
void dar_sync_from_vm(DARContext* ctx);

// Sync DAR state to VM
void dar_sync_to_vm(DARContext* ctx);

// # B.3 Health File Operations [HEALTH_FILE]

// Read health from .health file
// Returns health value (-100 to +100), or 0 on error
int8_t dar_health_read(const char* path);

// Write health to .health file
bool dar_health_write(const char* path, int8_t health);

// Read health from directory's .health file
int8_t dar_health_read_dir(const char* dir_path);

// Write health to directory's .health file
bool dar_health_write_dir(const char* dir_path, int8_t health);

// # B.4 Checkpoint Operations [CHECKPOINT]

// Create checkpoint from current state
// Returns checkpoint index (0-8), or -1 on error
int dar_checkpoint_create(DARContext* ctx, const char* name);

// Restore from checkpoint by index
bool dar_checkpoint_restore(DARContext* ctx, int index);

// Restore from most recent checkpoint
bool dar_checkpoint_restore_last(DARContext* ctx);

// Discard checkpoint by index
bool dar_checkpoint_discard(DARContext* ctx, int index);

// Discard all checkpoints
void dar_checkpoint_discard_all(DARContext* ctx);

// Get checkpoint by index
DARCheckpoint* dar_checkpoint_get(DARContext* ctx, int index);

// # B.5 Session Persistence [SESSION]

// Load session from file
bool dar_session_load(DARContext* ctx, const char* path);

// Save session to file
bool dar_session_save(DARContext* ctx, const char* path);

// Save session to default path
bool dar_session_save_default(DARContext* ctx);

// Create new session
void dar_session_new(DARContext* ctx, const char* session_id);

// # B.6 State Operations [STATE]

// Get current health
int8_t dar_health_get(DARContext* ctx);

// Set health (marks session dirty)
void dar_health_set(DARContext* ctx, int8_t health);

// Add to health (clamped to -100..+100)
void dar_health_add(DARContext* ctx, int8_t delta);

// Get Hebrew state
DARHebrewState dar_hebrew_state(DARContext* ctx);

// Get Hebrew state name
const char* dar_hebrew_state_name(DARHebrewState state);

// Transition Hebrew state (k_factor: +1=expand, -1=retreat, 0=maintain)
void dar_hebrew_transition(DARContext* ctx, int8_t k_factor);

// # B.7 Detection & Assessment [DETECT]

// Check if state is critical (health <= -50)
bool dar_is_critical(DARContext* ctx);

// Check if state is healthy (health >= 50)
bool dar_is_healthy(DARContext* ctx);

// Assess current state, return recommended action
// Returns: +1 = continue, 0 = checkpoint recommended, -1 = recovery needed
int8_t dar_assess(DARContext* ctx);

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Usage Example
//
//   // Create DAR context with session persistence
//   DARContext* dar = dar_create("session.dar");
//
//   // Connect to VM
//   OmniVM* vm = omni_vm_create();
//   dar_connect_vm(dar, vm);
//
//   // Load program and run
//   omni_vm_load_file(vm, "program.omir");
//
//   // Create checkpoint before risky operation
//   dar_checkpoint_create(dar, "before_risky");
//
//   // Run program
//   OmniVMResult result = omni_vm_run(vm);
//
//   // If failed, restore checkpoint
//   if (result == VM_DENIED) {
//       dar_checkpoint_restore_last(dar);
//   }
//
//   // Save session state
//   dar_session_save_default(dar);
//
//   dar_destroy(dar);

// # X.2 Closing Note
//
// "A just man falleth seven times, and riseth up again."
//   — Proverbs 24:16
//
// The DAR system enables graceful recovery - the body remembers
// what the mind forgets, allowing resurrection after failure.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_DAR_H
