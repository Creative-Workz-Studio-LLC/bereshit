// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-process-impl
// Title:       MillenniumOS Process Management Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / Process Subsystem
// Role:        Process creation, scheduling, and lifecycle
//
// Status:      Active
// Version:     a-01.00
// Created:     2026-01-30
// Updated:     2026-01-30
//
// Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
//
// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "kernel.h"                         // All kernel types (MUST be first)
#include "core/process/process.h"           // Process subsystem
#include "core/memory/memory.h"             // Memory subsystem
#include "trit/matter/matter.h"             // libtrit types

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global State
// -----------------------------------------------------------------------------

static MosScheduler g_scheduler = {
    .current = NULL,
    .process_count = 0,
    .ready_high = NULL,
    .ready_normal = NULL,
    .ready_low = NULL,
    .blocked = NULL,
    .sleeping = NULL,
    .context_switches = 0,
    .idle_time = 0,
    .health = MOS_HEALTH_NEUTRAL,
};

// Process table (sparse, using trit5 buckets for 243 entries each)
#define PROCESS_BUCKETS     (MOS_MAX_PROCESSES / 243 + 1)
static MosProcess* g_process_table[PROCESS_BUCKETS];

// Kernel process (PID 0 = 9841 in raw trit9)
static MosProcess g_kernel_process;

// Next PID to allocate (starts at 9842 = first user process)
static mos_pid_t g_next_pid = MOS_PID_INIT;

// Idle process (runs when nothing else to do)
static MosProcess g_idle_process;

// -----------------------------------------------------------------------------
// B.2 PID Management
// -----------------------------------------------------------------------------

static mos_pid_t allocate_pid(void) {
    // Simple linear allocation for now
    // Real implementation would use bitmap or free list
    mos_pid_t pid = g_next_pid;
    g_next_pid++;

    if (g_next_pid > TRIT9_MAX) {
        // Wrap around (would need to check for reuse)
        g_next_pid = MOS_PID_INIT;
    }

    return pid;
}

static int pid_to_bucket(mos_pid_t pid) {
    // Map PID to bucket index (divide by 243 = trit5 states)
    return pid / 243;
}

static MosProcess** get_bucket(mos_pid_t pid) {
    int bucket = pid_to_bucket(pid);
    if (bucket < 0 || bucket >= PROCESS_BUCKETS) {
        return NULL;
    }
    return &g_process_table[bucket];
}

// -----------------------------------------------------------------------------
// B.3 Process Creation
// -----------------------------------------------------------------------------

static void init_process(MosProcess* proc, mos_pid_t pid, const char* name,
                         mos_pid_t parent, MosZone zone) {
    // Identity
    proc->pid = pid;
    proc->ppid = parent;
    proc->uid = 0;  // Root for now
    proc->gid = 0;

    // State
    proc->state = MOS_PROC_STATE_EMBRYO;
    proc->priority = MOS_PRIO_NORMAL;
    proc->zone = zone;
    proc->hebrew_state = MOS_CPISI_YASHAR;  // Start centered (yashar)

    // Context (will be set up by exec or fork)
    for (size_t i = 0; i < sizeof(proc->context) / sizeof(uint64_t); i++) {
        ((uint64_t*)&proc->context)[i] = 0;
    }
    proc->context.rflags = 0x202;  // IF enabled

    // Stacks
    proc->kernel_stack = 0;
    proc->user_stack = 0;

    // Memory
    proc->page_table = 0;
    proc->heap_start = 0;
    proc->heap_end = 0;
    proc->memory_used = 0;

    // Scheduling
    proc->time_slice = MOS_QUANTUM_DEFAULT;
    proc->total_time = 0;
    proc->sleep_until = 0;

    // Health
    proc->health = MOS_HEALTH_NEUTRAL;
    proc->fault_count = 0;

    // Name
    int i;
    for (i = 0; name[i] && i < 31; i++) {
        proc->name[i] = name[i];
    }
    proc->name[i] = '\0';

    // Flags
    proc->flags = MOS_FLAG_NONE;

    // List pointers
    proc->next = NULL;
    proc->prev = NULL;
}

mos_result_t mos_process_create(mos_pid_t* pid) {
    return mos_process_create_ex(pid, "unnamed", MOS_PID_KERNEL, MOS_ZONE_WORD);
}

mos_result_t mos_process_create_ex(mos_pid_t* pid, const char* name,
                                    mos_pid_t parent, MosZone zone) {
    if (!pid || !name) {
        return MOS_ERR_INVALID;
    }

    // Allocate process structure
    MosProcess* proc = (MosProcess*)mos_kzalloc(sizeof(MosProcess));
    if (!proc) {
        return MOS_ERR_NOMEM;
    }

    // Allocate PID
    mos_pid_t new_pid = allocate_pid();

    // Initialize
    init_process(proc, new_pid, name, parent, zone);

    // Allocate kernel stack (4KB = 1 page)
    proc->kernel_stack = mos_kmalloc(4096);
    if (!proc->kernel_stack) {
        mos_kfree((mos_vaddr_t)proc);
        return MOS_ERR_NOMEM;
    }

    // Set up initial context
    proc->context.rsp = proc->kernel_stack + 4096 - 8;  // Top of stack
    proc->context.cs = 0x08;  // Kernel code segment
    proc->context.ss = 0x10;  // Kernel data segment

    // Add to process table
    MosProcess** bucket = get_bucket(new_pid);
    if (bucket) {
        proc->next = *bucket;
        if (*bucket) {
            (*bucket)->prev = proc;
        }
        *bucket = proc;
    }

    // Mark as ready
    proc->state = MOS_PROC_STATE_READY;

    // Add to scheduler (normal priority queue)
    proc->next = g_scheduler.ready_normal;
    if (g_scheduler.ready_normal) {
        g_scheduler.ready_normal->prev = proc;
    }
    g_scheduler.ready_normal = proc;

    g_scheduler.process_count++;

    // Initialize CPI-SI identity for this process
    // The process starts in the yashar (centered/even) state
    mos_cpisi_init_identity(new_pid, name, "Process identity");

    *pid = new_pid;
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.4 Process Termination
// -----------------------------------------------------------------------------

mos_result_t mos_process_exit(mos_pid_t pid, int exit_code) {
    MosProcess* proc = mos_process_get(pid);
    if (!proc) {
        return MOS_ERR_NOTFOUND;
    }

    (void)exit_code;  // TODO: Store for wait()

    // Mark as dead
    proc->state = MOS_PROC_STATE_DEAD;

    // Remove from scheduler queues
    if (proc->prev) proc->prev->next = proc->next;
    if (proc->next) proc->next->prev = proc->prev;

    // Update ready queue heads if necessary
    if (g_scheduler.ready_high == proc) g_scheduler.ready_high = proc->next;
    if (g_scheduler.ready_normal == proc) g_scheduler.ready_normal = proc->next;
    if (g_scheduler.ready_low == proc) g_scheduler.ready_low = proc->next;

    // Free resources
    if (proc->kernel_stack) {
        mos_kfree(proc->kernel_stack);
    }

    // TODO: Free page tables, user memory, etc.

    // If current process, schedule next
    if (g_scheduler.current == proc) {
        g_scheduler.current = NULL;
        mos_schedule();
    }

    return MOS_OK;
}

mos_result_t mos_process_kill(mos_pid_t pid) {
    // Same as exit for now
    return mos_process_exit(pid, -1);
}

// -----------------------------------------------------------------------------
// B.5 Process Query
// -----------------------------------------------------------------------------

MosProcess* mos_process_get(mos_pid_t pid) {
    // Special case: kernel process
    if (pid == MOS_PID_KERNEL) {
        return &g_kernel_process;
    }

    MosProcess** bucket = get_bucket(pid);
    if (!bucket) return NULL;

    MosProcess* proc = *bucket;
    while (proc) {
        if (proc->pid == pid) {
            return proc;
        }
        proc = proc->next;
    }

    return NULL;
}

MosProcess* mos_process_current(void) {
    return g_scheduler.current;
}

MosZone mos_process_zone(mos_pid_t pid) {
    MosProcess* proc = mos_process_get(pid);
    if (!proc) return MOS_ZONE_UNKNOWN;
    return proc->zone;
}

// -----------------------------------------------------------------------------
// B.6 Process State Management
// -----------------------------------------------------------------------------

mos_result_t mos_process_set_state(mos_pid_t pid, trit5_t state) {
    MosProcess* proc = mos_process_get(pid);
    if (!proc) return MOS_ERR_NOTFOUND;

    proc->state = state;
    return MOS_OK;
}

mos_result_t mos_process_set_priority(mos_pid_t pid, trit5_t priority) {
    MosProcess* proc = mos_process_get(pid);
    if (!proc) return MOS_ERR_NOTFOUND;

    proc->priority = priority;
    // TODO: Move to appropriate queue
    return MOS_OK;
}

mos_result_t mos_process_set_zone(mos_pid_t pid, MosZone zone) {
    MosProcess* proc = mos_process_get(pid);
    if (!proc) return MOS_ERR_NOTFOUND;

    // Check zone transition is valid
    // tov → word → void is allowed (privilege reduction)
    // void → word → tov requires escalation permission
    trit_result_t dir = trit_hsub(zone, proc->zone);

    if (dir.value > 0) {
        // Escalation - check permission
        // For now, only allow if in word zone or kernel
        if (proc->zone != MOS_ZONE_WORD && pid != MOS_PID_KERNEL) {
            return MOS_ERR_PERM;
        }
    }

    proc->zone = zone;
    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.7 Scheduling
// -----------------------------------------------------------------------------

static MosProcess* pick_next_process(void) {
    // Priority-based: check high, then normal, then low
    if (g_scheduler.ready_high) {
        return g_scheduler.ready_high;
    }
    if (g_scheduler.ready_normal) {
        return g_scheduler.ready_normal;
    }
    if (g_scheduler.ready_low) {
        return g_scheduler.ready_low;
    }

    // Nothing ready - return idle process
    return &g_idle_process;
}

static void remove_from_ready(MosProcess* proc) {
    if (proc->prev) proc->prev->next = proc->next;
    if (proc->next) proc->next->prev = proc->prev;

    // Update queue heads
    if (g_scheduler.ready_high == proc) g_scheduler.ready_high = proc->next;
    if (g_scheduler.ready_normal == proc) g_scheduler.ready_normal = proc->next;
    if (g_scheduler.ready_low == proc) g_scheduler.ready_low = proc->next;

    proc->next = NULL;
    proc->prev = NULL;
}

static void add_to_ready(MosProcess* proc) {
    MosProcess** queue;

    // Select queue based on priority
    if (proc->priority >= MOS_PRIO_HIGH) {
        queue = &g_scheduler.ready_high;
    } else if (proc->priority >= MOS_PRIO_LOW) {
        queue = &g_scheduler.ready_normal;
    } else {
        queue = &g_scheduler.ready_low;
    }

    // Add to front of queue
    proc->next = *queue;
    proc->prev = NULL;
    if (*queue) {
        (*queue)->prev = proc;
    }
    *queue = proc;
}

void mos_yield(void) {
    MosProcess* current = g_scheduler.current;
    if (current && current->state == MOS_PROC_STATE_RUNNING) {
        current->state = MOS_PROC_STATE_READY;
        add_to_ready(current);
    }
    mos_schedule();
}

void mos_schedule(void) {
    MosProcess* current = g_scheduler.current;
    MosProcess* next = pick_next_process();

    if (next == current) {
        // Same process, just continue
        return;
    }

    if (next) {
        // Remove from ready queue
        remove_from_ready(next);
        next->state = MOS_PROC_STATE_RUNNING;
        next->time_slice = MOS_QUANTUM_DEFAULT;

        // Context switch
        g_scheduler.current = next;
        g_scheduler.context_switches++;

        if (current && current != &g_idle_process) {
            // Save context and switch
            mos_context_switch(next);
        } else {
            // No previous context, just restore next
            mos_context_restore(&next->context);
        }
    }
}

mos_result_t mos_sleep(mos_time_t duration) {
    MosProcess* current = g_scheduler.current;
    if (!current) return MOS_ERR_INVALID;

    // TODO: Get current time and set wake time
    current->sleep_until = duration;  // Simplified
    current->state = MOS_PROC_STATE_SLEEPING;

    // Add to sleeping list
    current->next = g_scheduler.sleeping;
    if (g_scheduler.sleeping) {
        g_scheduler.sleeping->prev = current;
    }
    g_scheduler.sleeping = current;

    mos_schedule();
    return MOS_OK;
}

mos_result_t mos_wake(mos_pid_t pid) {
    MosProcess* proc = mos_process_get(pid);
    if (!proc) return MOS_ERR_NOTFOUND;

    if (proc->state != MOS_PROC_STATE_SLEEPING) {
        return MOS_ERR_INVALID;
    }

    // Remove from sleeping list
    if (proc->prev) proc->prev->next = proc->next;
    if (proc->next) proc->next->prev = proc->prev;
    if (g_scheduler.sleeping == proc) g_scheduler.sleeping = proc->next;

    // Add to ready queue
    proc->state = MOS_PROC_STATE_READY;
    add_to_ready(proc);

    return MOS_OK;
}

mos_result_t mos_block(void) {
    MosProcess* current = g_scheduler.current;
    if (!current) return MOS_ERR_INVALID;

    current->state = MOS_PROC_STATE_BLOCKED;

    // Add to blocked list
    current->next = g_scheduler.blocked;
    if (g_scheduler.blocked) {
        g_scheduler.blocked->prev = current;
    }
    g_scheduler.blocked = current;

    mos_schedule();
    return MOS_OK;
}

mos_result_t mos_unblock(mos_pid_t pid) {
    MosProcess* proc = mos_process_get(pid);
    if (!proc) return MOS_ERR_NOTFOUND;

    if (proc->state != MOS_PROC_STATE_BLOCKED) {
        return MOS_ERR_INVALID;
    }

    // Remove from blocked list
    if (proc->prev) proc->prev->next = proc->next;
    if (proc->next) proc->next->prev = proc->prev;
    if (g_scheduler.blocked == proc) g_scheduler.blocked = proc->next;

    // Add to ready queue
    proc->state = MOS_PROC_STATE_READY;
    add_to_ready(proc);

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.8 Context Switching (Assembly Helpers)
// -----------------------------------------------------------------------------
//
// The actual context switch is implemented in context_switch.asm.
// These C functions provide access to scheduler state for the assembly.
//

// Get pointer to current process's CPU context
// Called from context_switch.asm
MosCpuContext* mos_process_current_context(void) {
    if (!g_scheduler.current) {
        return NULL;
    }
    return &g_scheduler.current->context;
}

// Set current process and return it
// Called from context_switch.asm
MosProcess* mos_process_set_current(MosProcess* proc) {
    g_scheduler.current = proc;
    return proc;
}

// -----------------------------------------------------------------------------
// B.9 Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_process_init(void) {
    // Initialize process table
    for (int i = 0; i < PROCESS_BUCKETS; i++) {
        g_process_table[i] = NULL;
    }

    // Initialize kernel process
    init_process(&g_kernel_process, MOS_PID_KERNEL, "kernel",
                 MOS_PID_KERNEL, MOS_ZONE_WORD);
    g_kernel_process.state = MOS_PROC_STATE_RUNNING;
    g_kernel_process.priority = MOS_PRIO_REALTIME;

    // Initialize idle process
    init_process(&g_idle_process, MOS_PID_KERNEL - 1, "idle",
                 MOS_PID_KERNEL, MOS_ZONE_VOID);
    g_idle_process.state = MOS_PROC_STATE_READY;
    g_idle_process.priority = MOS_PRIO_IDLE;

    // Set kernel as current
    g_scheduler.current = &g_kernel_process;
    g_scheduler.process_count = 1;
    g_scheduler.health = MOS_HEALTH_MAX / 2;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.10 Information
// -----------------------------------------------------------------------------

const MosScheduler* mos_scheduler_info(void) {
    return &g_scheduler;
}

mos_health_t mos_process_health(mos_pid_t pid) {
    MosProcess* proc = mos_process_get(pid);
    if (!proc) return MOS_HEALTH_MIN;
    return proc->health;
}

mos_size_t mos_process_count(void) {
    return g_scheduler.process_count;
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Process management implementation:
//   - PID allocation using trit9 space
//   - Process table with trit5 bucketing (243 entries per bucket)
//   - Priority-based scheduling (high/normal/low queues)
//   - State management using trit5 with spare metadata
//   - Zone-based security (void/word/tov)
//
// =============================================================================
// END CLOSING
// =============================================================================
