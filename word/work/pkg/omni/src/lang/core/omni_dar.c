// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-dar-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_dar.c — DAR (Detect-Assess-Recover) Implementation
// Connects CPI-SI mind to computational body through persistent state.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-dar-impl
// Title: DAR System Implementation
// Type: Source
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "omni_dar.h"

// # S.1 Helpers [HELPERS]

static void safe_strcpy(char* dest, size_t size, const char* src) {
    if (!dest || size == 0) return;
    if (!src) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

static int8_t clamp_health(int value) {
    if (value < -100) return -100;
    if (value > 100) return 100;
    return (int8_t)value;
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Context Lifecycle [LIFECYCLE]

DARContext* dar_create(const char* session_path) {
    DARContext* ctx = calloc(1, sizeof(DARContext));
    if (!ctx) return NULL;

    ctx->session = calloc(1, sizeof(DARSession));
    if (!ctx->session) {
        free(ctx);
        return NULL;
    }

    // Initialize session
    ctx->session->magic = DAR_MAGIC;
    ctx->session->version = DAR_VERSION;
    ctx->session->health = 50;  // Default: deferred
    ctx->session->hebrew_state = DAR_YASHAR;
    ctx->session->created = (uint64_t)time(NULL);
    ctx->session->updated = ctx->session->created;

    if (session_path) {
        safe_strcpy(ctx->session_path, sizeof(ctx->session_path), session_path);

        // Try to load existing session
        if (!dar_session_load(ctx, session_path)) {
            // New session - generate ID
            snprintf(ctx->session->session_id, sizeof(ctx->session->session_id),
                     "dar_%llu", (unsigned long long)ctx->session->created);
        }
    }

    return ctx;
}

void dar_destroy(DARContext* ctx) {
    if (!ctx) return;

    // Save session if dirty
    if (ctx->session_dirty && ctx->session_path[0]) {
        dar_session_save_default(ctx);
    }

    if (ctx->session) {
        free(ctx->session);
    }

    free(ctx);
}

// # B.2 VM Connection [VM]

void dar_connect_vm(DARContext* ctx, OmniVM* vm) {
    if (!ctx) return;
    ctx->vm = vm;

    // Sync initial state from VM
    if (vm) {
        dar_sync_from_vm(ctx);
    }
}

void dar_disconnect_vm(DARContext* ctx) {
    if (!ctx) return;

    // Sync final state to VM before disconnect
    if (ctx->vm) {
        dar_sync_to_vm(ctx);
    }

    ctx->vm = NULL;
}

void dar_sync_from_vm(DARContext* ctx) {
    if (!ctx || !ctx->vm || !ctx->session) return;

    ctx->session->health = ctx->vm->health;
    ctx->session->hebrew_state = ctx->vm->hebrew_state;
    ctx->session->updated = (uint64_t)time(NULL);
    ctx->session_dirty = true;
}

void dar_sync_to_vm(DARContext* ctx) {
    if (!ctx || !ctx->vm || !ctx->session) return;

    ctx->vm->health = ctx->session->health;
    ctx->vm->hebrew_state = ctx->session->hebrew_state;
}

// # B.3 Health File Operations [HEALTH_FILE]

int8_t dar_health_read(const char* path) {
    if (!path) return 0;

    FILE* f = fopen(path, "rb");
    if (!f) return 0;

    uint8_t byte = 0;
    size_t read = fread(&byte, 1, 1, f);
    fclose(f);

    if (read != 1) return 0;

    return DAR_HEALTH_DECODE(byte);
}

bool dar_health_write(const char* path, int8_t health) {
    if (!path) return false;

    FILE* f = fopen(path, "wb");
    if (!f) return false;

    uint8_t byte = DAR_HEALTH_ENCODE(health);
    size_t written = fwrite(&byte, 1, 1, f);
    fclose(f);

    return written == 1;
}

int8_t dar_health_read_dir(const char* dir_path) {
    if (!dir_path) return 0;

    char path[512];
    snprintf(path, sizeof(path), "%s/.health", dir_path);

    return dar_health_read(path);
}

bool dar_health_write_dir(const char* dir_path, int8_t health) {
    if (!dir_path) return false;

    char path[512];
    snprintf(path, sizeof(path), "%s/.health", dir_path);

    return dar_health_write(path, health);
}

// # B.4 Checkpoint Operations [CHECKPOINT]

int dar_checkpoint_create(DARContext* ctx, const char* name) {
    if (!ctx || !ctx->session) return -1;

    // Find next available slot
    int index = ctx->session->checkpoint_count;
    if (index >= DAR_MAX_CHECKPOINTS) {
        // Circular buffer - overwrite oldest
        index = 0;
        // Shift checkpoints
        for (int i = 0; i < DAR_MAX_CHECKPOINTS - 1; i++) {
            ctx->session->checkpoints[i] = ctx->session->checkpoints[i + 1];
        }
        index = DAR_MAX_CHECKPOINTS - 1;
    } else {
        ctx->session->checkpoint_count++;
    }

    DARCheckpoint* cp = &ctx->session->checkpoints[index];
    memset(cp, 0, sizeof(DARCheckpoint));

    cp->magic = DAR_MAGIC;
    cp->version = DAR_VERSION;
    cp->timestamp = (uint64_t)time(NULL);

    if (name) {
        safe_strcpy(cp->name, sizeof(cp->name), name);
    }

    // Capture current state
    cp->health = ctx->session->health;
    cp->hebrew_state = ctx->session->hebrew_state;

    // If connected to VM, capture VM state
    if (ctx->vm) {
        cp->ip = ctx->vm->ip;
        cp->sp = ctx->vm->sp;

        // Capture top of stack
        cp->stack_count = ctx->vm->sp > 16 ? 16 : ctx->vm->sp;
        for (uint16_t i = 0; i < cp->stack_count; i++) {
            cp->stack[i] = ctx->vm->stack[ctx->vm->sp - cp->stack_count + i];
        }
    }

    ctx->session->updated = cp->timestamp;
    ctx->session_dirty = true;

    return index;
}

bool dar_checkpoint_restore(DARContext* ctx, int index) {
    if (!ctx || !ctx->session) return false;
    if (index < 0 || index >= ctx->session->checkpoint_count) return false;

    DARCheckpoint* cp = &ctx->session->checkpoints[index];
    if (cp->magic != DAR_MAGIC) return false;

    // Restore state
    ctx->session->health = cp->health;
    ctx->session->hebrew_state = cp->hebrew_state;

    // If connected to VM, restore VM state
    if (ctx->vm) {
        ctx->vm->ip = cp->ip;
        ctx->vm->sp = 0;

        // Restore stack
        for (uint16_t i = 0; i < cp->stack_count; i++) {
            ctx->vm->stack[i] = cp->stack[i];
        }
        ctx->vm->sp = cp->stack_count;

        ctx->vm->health = cp->health;
        ctx->vm->hebrew_state = cp->hebrew_state;
    }

    ctx->session->updated = (uint64_t)time(NULL);
    ctx->session_dirty = true;

    return true;
}

bool dar_checkpoint_restore_last(DARContext* ctx) {
    if (!ctx || !ctx->session) return false;
    if (ctx->session->checkpoint_count == 0) return false;

    return dar_checkpoint_restore(ctx, ctx->session->checkpoint_count - 1);
}

bool dar_checkpoint_discard(DARContext* ctx, int index) {
    if (!ctx || !ctx->session) return false;
    if (index < 0 || index >= ctx->session->checkpoint_count) return false;

    // Shift remaining checkpoints
    for (int i = index; i < ctx->session->checkpoint_count - 1; i++) {
        ctx->session->checkpoints[i] = ctx->session->checkpoints[i + 1];
    }

    ctx->session->checkpoint_count--;
    ctx->session_dirty = true;

    return true;
}

void dar_checkpoint_discard_all(DARContext* ctx) {
    if (!ctx || !ctx->session) return;

    ctx->session->checkpoint_count = 0;
    ctx->session_dirty = true;
}

DARCheckpoint* dar_checkpoint_get(DARContext* ctx, int index) {
    if (!ctx || !ctx->session) return NULL;
    if (index < 0 || index >= ctx->session->checkpoint_count) return NULL;

    return &ctx->session->checkpoints[index];
}

// # B.5 Session Persistence [SESSION]

bool dar_session_load(DARContext* ctx, const char* path) {
    if (!ctx || !ctx->session || !path) return false;

    FILE* f = fopen(path, "rb");
    if (!f) return false;

    DARSession temp;
    size_t read = fread(&temp, sizeof(DARSession), 1, f);
    fclose(f);

    if (read != 1) return false;
    if (temp.magic != DAR_MAGIC) return false;
    if (temp.version > DAR_VERSION) return false;

    *ctx->session = temp;
    ctx->session_dirty = false;

    return true;
}

bool dar_session_save(DARContext* ctx, const char* path) {
    if (!ctx || !ctx->session || !path) return false;

    ctx->session->updated = (uint64_t)time(NULL);

    FILE* f = fopen(path, "wb");
    if (!f) return false;

    size_t written = fwrite(ctx->session, sizeof(DARSession), 1, f);
    fclose(f);

    if (written == 1) {
        ctx->session_dirty = false;
        return true;
    }

    return false;
}

bool dar_session_save_default(DARContext* ctx) {
    if (!ctx || !ctx->session_path[0]) return false;
    return dar_session_save(ctx, ctx->session_path);
}

void dar_session_new(DARContext* ctx, const char* session_id) {
    if (!ctx || !ctx->session) return;

    memset(ctx->session, 0, sizeof(DARSession));
    ctx->session->magic = DAR_MAGIC;
    ctx->session->version = DAR_VERSION;
    ctx->session->health = 50;
    ctx->session->hebrew_state = DAR_YASHAR;
    ctx->session->created = (uint64_t)time(NULL);
    ctx->session->updated = ctx->session->created;

    if (session_id) {
        safe_strcpy(ctx->session->session_id, sizeof(ctx->session->session_id), session_id);
    } else {
        snprintf(ctx->session->session_id, sizeof(ctx->session->session_id),
                 "dar_%llu", (unsigned long long)ctx->session->created);
    }

    ctx->session_dirty = true;
}

// # B.6 State Operations [STATE]

int8_t dar_health_get(DARContext* ctx) {
    if (!ctx || !ctx->session) return 0;
    return ctx->session->health;
}

void dar_health_set(DARContext* ctx, int8_t health) {
    if (!ctx || !ctx->session) return;

    ctx->session->health = clamp_health(health);
    ctx->session->updated = (uint64_t)time(NULL);
    ctx->session_dirty = true;

    // Sync to VM if connected
    if (ctx->vm) {
        ctx->vm->health = ctx->session->health;
    }
}

void dar_health_add(DARContext* ctx, int8_t delta) {
    if (!ctx || !ctx->session) return;

    int new_health = ctx->session->health + delta;
    dar_health_set(ctx, clamp_health(new_health));
}

DARHebrewState dar_hebrew_state(DARContext* ctx) {
    if (!ctx || !ctx->session) return DAR_YASHAR;
    return (DARHebrewState)ctx->session->hebrew_state;
}

const char* dar_hebrew_state_name(DARHebrewState state) {
    switch (state) {
        case DAR_SHAVAR: return "shavar (broken)";
        case DAR_CHASER: return "chaser (lacking)";
        case DAR_RATSAH: return "ratsah (wanting)";
        case DAR_YASHAR: return "yashar (even)";
        case DAR_TAMIM:  return "tamim (sound)";
        case DAR_SHALEM: return "shalem (whole)";
        case DAR_TOV:    return "tov (perfect)";
        default:            return "unknown";
    }
}

void dar_hebrew_transition(DARContext* ctx, int8_t k_factor) {
    if (!ctx || !ctx->session) return;

    int8_t new_state = ctx->session->hebrew_state;

    if (k_factor > 0) {
        // Expand toward tov
        if (new_state < DAR_TOV) new_state++;
    } else if (k_factor < 0) {
        // Retreat toward shavar
        if (new_state > DAR_SHAVAR) new_state--;
    }

    ctx->session->hebrew_state = new_state;
    ctx->session->updated = (uint64_t)time(NULL);
    ctx->session_dirty = true;

    // Sync to VM if connected
    if (ctx->vm) {
        ctx->vm->hebrew_state = new_state;
    }
}

// # B.7 Detection & Assessment [DETECT]

bool dar_is_critical(DARContext* ctx) {
    if (!ctx || !ctx->session) return false;
    return ctx->session->health <= -50;
}

bool dar_is_healthy(DARContext* ctx) {
    if (!ctx || !ctx->session) return false;
    return ctx->session->health >= 50;
}

int8_t dar_assess(DARContext* ctx) {
    if (!ctx || !ctx->session) return -1;

    int8_t health = ctx->session->health;
    int8_t hebrew = ctx->session->hebrew_state;

    // Critical state - recovery needed
    if (health <= -50 || hebrew <= DAR_SHAVAR) {
        return -1;
    }

    // Warning state - checkpoint recommended
    if (health <= 0 || hebrew <= DAR_RATSAH) {
        return 0;
    }

    // Healthy - continue
    return +1;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// "A just man falleth seven times, and riseth up again."
//   — Proverbs 24:16
//
// The DAR system enables graceful recovery - the body remembers
// what the mind forgets, allowing resurrection after failure.

// =============================================================================
// END CLOSING
// =============================================================================
