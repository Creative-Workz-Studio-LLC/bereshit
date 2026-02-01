/// @file process.h
/// @omni code --c --header
///
/// =============================================================================
/// OMNICODE PRAGMA [PRAGMA]
/// =============================================================================
///
/// @legend
///   :req  REQUIRED   — must exist, validation fails without
///   :inh  INHERITED  — from template, override allowed
///   :ins  INSTANCE   — file-specific, unique values
/// @endlegend
///
/// ---------------------------------------------------------------------------
/// P.1 CORE — Identity [CORE]
/// ---------------------------------------------------------------------------
///
/// @omni:req  key        = MOS-kernel-core-process
/// @omni:req  from       = bereshit/word/seed/code/c/type-header.h
/// @omni:req  at         = a-02.00
///
/// ---------------------------------------------------------------------------
/// P.2 FAMILY — Classification [FAMILY]
/// ---------------------------------------------------------------------------
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = api
/// @omni:inh  role       = subsystem
/// @omni:inh  structure  = 4-block
///
/// ---------------------------------------------------------------------------
/// P.3 INSTANCE — File Details [INSTANCE]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  component  = kernel/core/process
/// @omni:ins  layer      = 0 (kernel layer)
/// @omni:ins  cpisi_mode = 0 (freestanding)
/// @omni:ins  provides   = [MosProcess, MosScheduler, MosCpuContext,
///                          mos_process_create, mos_schedule, mos_yield]
///
/// ---------------------------------------------------------------------------
/// P.4 ARCHITECTURE — Scaling Context [ARCH]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  layers     = [0:kernel, 1:hal, 2:services, 3:framework, 4:app]
/// @omni:ins  position   = 0 (kernel foundation)
/// @omni:ins  depends    = [kernel.h, interrupt.h]
///
/// ---------------------------------------------------------------------------
/// P.5 SUMMARY — Human Identity [SUMMARY]
/// ---------------------------------------------------------------------------
///
/// @omni:req  title      = MillenniumOS Process Management
/// @omni:req  brief      = Hybrid microkernel process/thread management with ternary PIDs
///
/// =============================================================================
/// END PRAGMA
/// =============================================================================

// =============================================================================
// METADATA BLOCK [METADATA]
// =============================================================================
//
// @brief Identity and context for process subsystem.
//
// STRUCTURE: M.1-M.10 sections for complete documentation.
//
// =============================================================================

/**
 * =============================================================================
 * M.1 IDENTITY — Core Identification [IDENTITY]
 * =============================================================================
 *
 * @file      process.h
 * @brief     MillenniumOS Process Management — Hybrid microkernel scheduling.
 *
 * Key:       MOS-kernel-core-process
 * Title:     MillenniumOS Process Management
 * Type:      Header (API)
 * Component: Kernel / Core / Process Subsystem
 * Role:      Process creation, scheduling, and lifecycle using ternary PIDs
 *
 * INTERFACE SUMMARY:
 *   Types (4):     MosCpuContext, MosProcess, MosScheduler, MosProcessState
 *   Functions (24): 1 init, 7 lifecycle, 5 state, 6 scheduling, 3 context, 2 info
 *
 * HYBRID MICROKERNEL:
 *   - Minimal kernel: memory, interrupts, IPC, scheduling
 *   - Services run in user-space (when available)
 *   - Clean separation for future exokernel/microkernel evolution
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.2 STATE — Lifecycle State [STATE]
 * =============================================================================
 *
 * @version   a-02.00
 * @date      2026-02-01
 *
 * Status:    Active
 * Created:   2026-01-30
 * Updated:   2026-02-01
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
 * =============================================================================
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Implementation)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.4 LOCATION — File Position [LOCATION]
 * =============================================================================
 *
 * Path:      millenniumos/kernel/include/core/process/process.h
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.5 DERIVATION — Template Lineage [DERIVATION]
 * =============================================================================
 *
 * Derives:   bereshit/word/seed/code/c/type-header.h (4-block C template)
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
 * =============================================================================
 *
 * Tags:      kernel, process, scheduler, context, microkernel, ternary, pid
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.7 INTENT — Purpose Statement [INTENT]
 * =============================================================================
 *
 * Purpose:   Provide process creation, scheduling, and lifecycle management
 *            for the hybrid microkernel. Uses ternary PIDs where negative
 *            values indicate system processes, zero is kernel, positive
 *            indicates user processes.
 *
 * HYBRID MICROKERNEL DESIGN:
 *
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │                        USER SPACE                                   │
 *   │   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐              │
 *   │   │  VFS    │  │ Network │  │ Display │  │  Audio  │  (Services)  │
 *   │   └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘              │
 *   └────────┼────────────┼────────────┼────────────┼────────────────────┘
 *            │            │            │            │
 *            ▼            ▼            ▼            ▼
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │                     IPC / MESSAGE PASSING                           │
 *   └────────────────────────────────┬────────────────────────────────────┘
 *                                    │
 *   ┌────────────────────────────────▼────────────────────────────────────┐
 *   │                       KERNEL CORE                                   │
 *   │   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐              │
 *   │   │ Memory  │  │Interrupt│  │ Process │  │   IPC   │              │
 *   │   │ (done)  │  │ (done)  │  │ (this)  │  │(planned)│              │
 *   │   └─────────┘  └─────────┘  └─────────┘  └─────────┘              │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * TERNARY PID SPACE (trit9 = 19,683 values):
 *   - [0, 9840]:     System processes (kernel threads, drivers)
 *   - [9841]:        Kernel pseudo-process (idle, init)
 *   - [9842, 19682]: User processes
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * =============================================================================
 *
 * Scripture: Genesis 1:11 — "And God said, Let the earth bring forth grass,
 *            the herb yielding seed, and the fruit tree yielding fruit
 *            after his kind."
 *
 * Principle: Each process brings forth work, categorized and tracked by kind.
 *            System processes (herbs) provide infrastructure, user processes
 *            (fruit trees) yield the actual fruit of computation.
 *
 * TERNARY MAPPING:
 *   - System (-): Infrastructure that enables (like grass/herbs)
 *   - Kernel (0): The earth itself (foundation)
 *   - User (+):   Fruit-bearing work (applications)
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
 * =============================================================================
 *
 * LADDER (bottom to top — what this depends on):
 *
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │ x86-64 CPU                   — Context switch, registers, CR3       │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ kernel.h                     — Core types (mos_pid_t, trit_t, etc.) │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ interrupt.h                  — Timer IRQ for preemption             │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ memory.h                     — Stack allocation, page tables        │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ process.h                    ◄── THIS FILE                          │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ IPC (planned)                — Inter-process communication          │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ Services (user-space)        — VFS, network, display (microkernel)  │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * BATON (initialization chain):
 *
 *   kernel.c (init)
 *        │
 *        ▼
 *   mos_memory_init() ─────► Memory available for stacks/page tables
 *        │
 *        ▼
 *   mos_interrupt_init() ──► Timer IRQ configured for preemption
 *        │
 *        ▼
 *   mos_process_init() ────► Scheduler ready, kernel process created
 *        │
 *        ▼
 *   mos_process_create() ──► First user process (init or shell)
 *        │
 *        ▼
 *   mos_schedule() ────────► Round-robin scheduling begins
 *        │
 *        ▼
 *   mos_sti() ─────────────► Timer interrupts enable preemption
 *
 * BOOT PATH SUPPORT:
 *   - UEFI (EDK2): Boot services provide memory map → process init
 *   - BIOS: Legacy boot → memory probe → process init
 *   - Both paths converge at mos_process_init()
 *
 * KEY INTERFACES (baton passed between components):
 *   - MosProcess*:    Process control block passed to scheduler
 *   - MosCpuContext*: CPU state saved/restored on context switch
 *   - Timer IRQ:      Calls mos_schedule() for preemption
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.10 ROADMAP — Version History [ROADMAP]
 * =============================================================================
 *
 * M.10a HISTORY — Version Timeline
 * ---------------------------------
 *   a-01.00 (2026-01-30) — Initial process subsystem stub
 *     • Process control block placeholder
 *     • Basic state definitions
 *   a-02.00 (2026-02-01) — Full OmniCode alignment, hybrid microkernel design
 *     • 4-block structure with type-header pattern
 *     • Hybrid microkernel architecture design
 *     • LADDER/BATON documentation
 *     • Health scoring thresholds
 *     • Ternary PID space (trit9 = 19,683 values)
 *     • Zone-based process classification
 *
 * M.10b COMPLETED — What's Done
 * -----------------------------
 *   ✓ Process control block (MosProcess) with ternary fields
 *   ✓ CPU context structure for x86-64 (15 registers)
 *   ✓ Scheduler state with priority queues
 *   ✓ Ternary PID space definition (system/kernel/user zones)
 *   ✓ Process states using trit5 with metadata (0-242 + 243-255)
 *   ✓ Priority levels centered at 121
 *   ✓ Quantum sizes (3, 9, 27 ticks)
 *   ✓ Context switch declarations
 *   ✓ Process zone classification
 *   ✓ DAR Witness integration points
 *
 * M.10c PHASE 1 — Basic Scheduling (a-02.xx) ✓ COMPLETE
 * -------------------------------------------------------
 *   ✓ Process creation (mos_process_create, mos_process_create_ex)
 *   ✓ Process termination (mos_process_exit, mos_process_kill)
 *   ✓ Priority-based scheduler (high/normal/low queues)
 *   ✓ Context switch in assembly (context_switch.asm)
 *   ✓ Kernel stack per process (4KB)
 *   ✓ Idle process
 *   ✓ Kernel process initialization
 *   ✓ Timer→Scheduler preemption (via IRQ 0)
 *
 *   Dependencies: memory.h for stack allocation
 *   Integration: interrupt.h for timer tick (IRQ 0)
 *
 * M.10d PHASE 2 — Priority Scheduling (a-03.xx)
 * ---------------------------------------------
 *   ⏳ Multi-level feedback queue
 *   ⏳ Priority decay (prevent starvation)
 *   ⏳ Real-time priority class
 *   ⏳ Quantum adjustment by priority
 *   ⏳ Sleep/wake primitives
 *   ⏳ Blocked state for I/O
 *
 *   Dependencies: timer.h for sleep timing
 *   Integration: IPC for blocking operations
 *
 * M.10e PHASE 3 — User Space (a-04.xx)
 * -------------------------------------
 *   ⏳ User/kernel mode separation (Ring 0 / Ring 3)
 *   ⏳ User-space stack allocation
 *   ⏳ System call interface (syscall instruction)
 *   ⏳ TSS setup for privilege transitions
 *   ⏳ Page tables per process
 *   ⏳ Address space isolation
 *
 *   Dependencies: memory.h for paging
 *   Integration: HAL for privilege management
 *
 * M.10f PHASE 4 — IPC Framework (b-01.xx)
 * ---------------------------------------
 *   ⏳ Synchronous message passing (send/receive)
 *   ⏳ Asynchronous notification (signals)
 *   ⏳ Shared memory regions
 *   ⏳ Port-based communication
 *   ⏳ Capability-based access control
 *
 *   This is the HEART of the microkernel design — all inter-process
 *   communication goes through kernel IPC primitives.
 *
 *   Integration: All user-space services communicate via IPC
 *
 * M.10g PHASE 5 — User-Space Services (b-02.xx)
 * ---------------------------------------------
 *   ⏳ VFS service (filesystem operations)
 *   ⏳ Device manager service (driver coordination)
 *   ⏳ Network stack service
 *   ⏳ Graphics service (compositor)
 *
 *   These run in user space (MOS_ZONE_WORD) and receive requests via IPC.
 *   The kernel only provides:
 *     - Memory management
 *     - Interrupt handling
 *     - IPC messaging
 *     - Basic scheduling
 *
 * M.10h PHASE 6 — SMP Support (b-03.xx)
 * -------------------------------------
 *   ⏳ Per-CPU scheduler state (cpu_id in MosScheduler)
 *   ⏳ Spinlocks for scheduler queues
 *   ⏳ Cross-CPU IPI for load balancing
 *   ⏳ CPU affinity API
 *   ⏳ Migration between CPUs
 *
 *   Dependencies: interrupt.h APIC/IPI support
 *   Integration: memory.h per-CPU allocation
 *
 * M.10i MICROKERNEL ARCHITECTURE
 * ------------------------------
 *   @code{.txt}
 *   ┌──────────────────────────────────────────────────────────────────────┐
 *   │                         USER SPACE (Ring 3)                          │
 *   │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐             │
 *   │  │   VFS    │  │  Device  │  │ Network  │  │ Graphics │  Services   │
 *   │  │ Service  │  │ Manager  │  │  Stack   │  │ Service  │  (IPC)      │
 *   │  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘             │
 *   │       │              │              │              │                 │
 *   │       └──────────────┴──────┬───────┴──────────────┘                 │
 *   │                             │ IPC                                    │
 *   ├─────────────────────────────┼────────────────────────────────────────┤
 *   │                             ▼                                        │
 *   │  ┌─────────────────────────────────────────────────────────────────┐ │
 *   │  │                    MICROKERNEL (Ring 0)                         │ │
 *   │  │  ┌───────────┐  ┌───────────┐  ┌───────────┐  ┌───────────┐    │ │
 *   │  │  │  Memory   │  │ Interrupt │  │    IPC    │  │ Scheduler │    │ │
 *   │  │  │  Manager  │  │  Handler  │  │  Router   │  │           │    │ │
 *   │  │  └───────────┘  └───────────┘  └───────────┘  └───────────┘    │ │
 *   │  └─────────────────────────────────────────────────────────────────┘ │
 *   │                         KERNEL SPACE                                 │
 *   └──────────────────────────────────────────────────────────────────────┘
 *   @endcode
 *
 * M.10j BOOT PATH SUPPORT
 * -----------------------
 *   UEFI Path: ExitBootServices → kernel → scheduler init → idle process
 *   BIOS Path: Stage 0b → kernel → scheduler init → idle process
 *
 *   Both paths converge at scheduler initialization. The idle process
 *   (PID 0) runs when no other process is ready. First user process
 *   (init) is created after all kernel services are initialized.
 *
 * M.10k TERNARY PID SPACE RATIONALE
 * ---------------------------------
 *   Traditional PIDs: 0-32767 (signed int16) or 1-65535 (unsigned)
 *   MillenniumOS PIDs: 0-19682 (trit9 = 3^9)
 *
 *   Semantic zones:
 *     VOID [0, 9840]:     System processes (kernel helpers, drivers)
 *     WORD [9841]:        Kernel pseudo-process (scheduling anchor)
 *     TOV  [9842, 19682]: User processes (applications)
 *
 *   This mirrors Genesis 1:1 structure where God (center) creates
 *   the heavens (system) and earth (user space) from void.
 *
 * =============================================================================
 */

// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_KERNEL_CORE_PROCESS_H
#define MOS_KERNEL_CORE_PROCESS_H

// =============================================================================
// SETUP BLOCK [SETUP]
// =============================================================================
//
// @brief Configuration and dependencies before body.
//
// CONTAINS:
//   - S.1 INCLUDES   — Header dependencies (kernel.h, interrupt.h)
//   - S.2 DEFINES    — Process constants, states, priorities, quantum
//   - S.3 TYPES      — Type declarations (context, PCB, scheduler)
//   - S.4 PROTOTYPES — Function declarations (lifecycle, scheduling, context)
//   - S.5 FILE-LEVEL — [Reserved: no file-level state]
//   - S.6 OMISSION   — Reserved section guide
//
// TYPE HEADER STRUCTURE:
//   S.3 contains full type definitions, S.4 contains all function prototypes.
//   BODY is reserved (all declarations in SETUP for type-header pattern).
//
// =============================================================================

/**
 * =============================================================================
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * =============================================================================
 *
 * @defgroup process_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — [Reserved: freestanding kernel]
 *   - S.1b INTERNAL  — Project headers (kernel.h, interrupt.h)
 *   - S.1c EXTERNAL  — [Reserved: none]
 *   - S.1d CPP       — [Reserved: kernel C code]
 *
 * =============================================================================
 * @{ */

/** S.1a STANDARD — [Reserved: freestanding kernel, no libc] @{ @} */

/**
 * ---------------------------------------------------------------------------
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ---------------------------------------------------------------------------
 *
 * @defgroup process_internal S.1b INTERNAL — Project Headers
 * @ingroup process_includes
 * @brief    Internal kernel headers required by this module.
 * @{ */

#include "kernel.h"                    /**< LAYER 0: Foundation (MUST be first) */
#include "core/interrupt/interrupt.h"  /**< Timer IRQ for preemption */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [Reserved: no external dependencies] @{ @} */

/** S.1d CPP — [Reserved: kernel C code] @{ @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * =============================================================================
 * S.2 DEFINES — Process Domain Definitions [DEFINES]
 * =============================================================================
 *
 * @defgroup process_domain S.2 DEFINES — Process Domain
 * @brief    Process constants, states, priorities, quantum, health thresholds.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: uses kernel.h primitives]
 *   - S.2a  BOUNDS  — Max processes, states, priorities, quantum, health
 *   - S.2b  MACROS  — [Reserved: operations in kernel.h]
 *   - S.2c  VERIFY  — [Reserved: see CLOSING X.1]
 *   - S.2d  LOOKUP  — [Reserved: no runtime tables in header]
 *   - S.2e  TYPES   — [Reserved: enums in S.3a]
 *
 * =============================================================================
 * @{ */

/** S.2.0 PRELUDE — [Reserved: uses kernel.h primitives] @{ @} */

/**
 * ---------------------------------------------------------------------------
 * S.2a BOUNDS — Process Constants [BOUNDS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup process_bounds S.2a BOUNDS — Process Constants
 * @ingroup process_domain
 * @brief    Process limits, states, priorities, quantum sizes.
 *
 * TERNARY PID ORGANIZATION (trit9 = 19,683 values):
 *   - [0, 9840]:     System processes (negative semantic)
 *   - [9841]:        Kernel pseudo-process (neutral semantic)
 *   - [9842, 19682]: User processes (positive semantic)
 *
 * @{ */

/** @name Process Limits
 *  @brief Maximum process count based on trit9.
 *  @note If kernel.h defines MOS_PID_KERNEL as 0 (legacy), these guards
 *        allow override. For ternary PID space, kernel.h should be updated.
 *  @{ */
#ifndef MOS_MAX_PROCESSES
#define MOS_MAX_PROCESSES       19683   /**< 3^9 = trit9 max values */
#endif
#ifndef MOS_PID_KERNEL
#define MOS_PID_KERNEL          9841    /**< Kernel pseudo-process (center) */
#endif
#ifndef MOS_PID_SYSTEM_MAX
#define MOS_PID_SYSTEM_MAX      9840    /**< Max system process PID */
#endif
#ifndef MOS_PID_USER_MIN
#define MOS_PID_USER_MIN        9842    /**< Min user process PID */
#endif
/** @} */ /* end Process Limits */

/** @name Process States (ternary semantic)
 *  @brief Core state using trit values.
 *  @{ */
#define MOS_PROC_DEAD           TRIT_NEG    /**< -1: Terminated */
#define MOS_PROC_READY          TRIT_ZERO   /**< 0: Ready to run */
#define MOS_PROC_RUNNING        TRIT_POS    /**< +1: Currently executing */
/** @} */ /* end Process States */

/** @name Extended States (trit5 with metadata)
 *  @brief Fine-grained states using trit5 range (0-242) plus spare (243-255).
 *  @{ */
#define MOS_PROC_STATE_DEAD         ((trit5_t)0)    /**< Dead/zombie */
#define MOS_PROC_STATE_EMBRYO       ((trit5_t)40)   /**< Being created */
#define MOS_PROC_STATE_BLOCKED      ((trit5_t)80)   /**< Blocked on I/O */
#define MOS_PROC_STATE_SLEEPING     ((trit5_t)81)   /**< Sleeping (timed) */
#define MOS_PROC_STATE_READY        ((trit5_t)121)  /**< Ready (center) */
#define MOS_PROC_STATE_RUNNING      ((trit5_t)161)  /**< Running */
#define MOS_PROC_STATE_STOPPED      ((trit5_t)41)   /**< Stopped (signal) */
/* Metadata states (spare values 243-255) */
#define MOS_PROC_STATE_INVALID      MOS_SPARE_INVALID   /**< 243: Invalid */
#define MOS_PROC_STATE_ERROR        MOS_SPARE_ERROR     /**< 244: Error */
#define MOS_PROC_STATE_PENDING      MOS_SPARE_PENDING   /**< 245: Pending */
/** @} */ /* end Extended States */

/** @name Priority Levels (trit5 centered at 121)
 *  @brief Priority values from idle (0) to realtime (242).
 *  @{ */
#define MOS_PRIO_IDLE           ((trit5_t)0)    /**< Lowest priority */
#define MOS_PRIO_LOW            ((trit5_t)60)   /**< Low priority */
#define MOS_PRIO_NORMAL         ((trit5_t)121)  /**< Default (center) */
#define MOS_PRIO_HIGH           ((trit5_t)180)  /**< High priority */
#define MOS_PRIO_REALTIME       ((trit5_t)242)  /**< Highest priority */
/** @} */ /* end Priority Levels */

/** @name Quantum Sizes (ternary-inspired tick counts)
 *  @brief Time slice sizes based on powers of 3.
 *  @{ */
#define MOS_QUANTUM_MIN         3       /**< 3^1 ticks (short) */
#define MOS_QUANTUM_DEFAULT     9       /**< 3^2 ticks (normal) */
#define MOS_QUANTUM_MAX         27      /**< 3^3 ticks (long) */
/** @} */ /* end Quantum Sizes */

/** @name Health Thresholds
 *  @brief Threshold values for process health scoring.
 *
 *  SCORING (ternary -1/0/+1 per metric):
 *    Metric            | -1 (Degraded)  | 0 (Normal)     | +1 (Optimal)
 *    ------------------|----------------|----------------|----------------
 *    Fault Rate        | >10 faults/sec | 1-10 faults    | <1 fault/sec
 *    Memory Usage      | >90% limit     | 50-90%         | <50% limit
 *    CPU Time          | >90% slice     | 50-90%         | <50% slice
 *    Block Duration    | >1s average    | 100ms-1s       | <100ms
 *
 *  HEALTH RANGE: -4 to +4 (4 metrics × trit)
 *
 *  @{ */
#define MOS_PROC_HEALTH_FAULT_HIGH    10    /**< Faults per second */
#define MOS_PROC_HEALTH_FAULT_LOW     1     /**< Faults per second */
#define MOS_PROC_HEALTH_MEM_HIGH      90    /**< Percent of limit */
#define MOS_PROC_HEALTH_MEM_LOW       50    /**< Percent of limit */
#define MOS_PROC_HEALTH_CPU_HIGH      90    /**< Percent of slice */
#define MOS_PROC_HEALTH_CPU_LOW       50    /**< Percent of slice */
#define MOS_PROC_HEALTH_BLOCK_HIGH    1000  /**< Milliseconds */
#define MOS_PROC_HEALTH_BLOCK_LOW     100   /**< Milliseconds */
/** @} */ /* end Health Thresholds */

/** @} */ /* end S.2a BOUNDS */

/** S.2b MACROS — [Reserved: operations in kernel.h] @{ @} */

/** S.2c VERIFY — [Reserved: see CLOSING X.1] @{ @} */

/** S.2d LOOKUP — [Reserved: no runtime tables in header] @{ @} */

/** S.2e TYPES — [Reserved: enums in S.3a] @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * =============================================================================
 * S.3 TYPES — Process Type Declarations [TYPES]
 * =============================================================================
 *
 * @defgroup process_types S.3 TYPES — Type Declarations
 * @brief    CPU context, process control block, scheduler structures.
 *
 * CONTAINS:
 *   - S.3a ENUMS   — [Reserved: states as defines for flexibility]
 *   - S.3b STRUCTS — MosCpuContext, MosProcess, MosScheduler
 *   - S.3c OPAQUE  — [Reserved: no opaque types]
 *   - S.3d ERRORS  — [Reserved: uses mos_result_t]
 *
 * =============================================================================
 * @{ */

/** S.3a ENUMS — [Reserved: states as defines for trit5 compatibility] @{ @} */

/**
 * ---------------------------------------------------------------------------
 * S.3b STRUCTS — Process Structures [STRUCTS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup process_types_structs S.3b STRUCTS — Structure Definitions
 * @ingroup process_types
 * @brief    CPU context, process control block, scheduler state.
 * @{ */

/**
 * @brief CPU context for context switching.
 *
 * Saved/restored on context switch. Matches x86-64 register set.
 */
typedef struct MosCpuContext {
    /* General purpose registers */
    uint64_t rax;       /**< Accumulator */
    uint64_t rbx;       /**< Base */
    uint64_t rcx;       /**< Counter */
    uint64_t rdx;       /**< Data */
    uint64_t rsi;       /**< Source index */
    uint64_t rdi;       /**< Destination index */
    uint64_t rbp;       /**< Base pointer */
    uint64_t rsp;       /**< Stack pointer */
    uint64_t r8;        /**< Extended register */
    uint64_t r9;        /**< Extended register */
    uint64_t r10;       /**< Extended register */
    uint64_t r11;       /**< Extended register */
    uint64_t r12;       /**< Extended register */
    uint64_t r13;       /**< Extended register */
    uint64_t r14;       /**< Extended register */
    uint64_t r15;       /**< Extended register */

    /* Instruction pointer and flags */
    uint64_t rip;       /**< Instruction pointer */
    uint64_t rflags;    /**< CPU flags */

    /* Segment registers */
    uint64_t cs;        /**< Code segment */
    uint64_t ss;        /**< Stack segment */
    uint64_t ds;        /**< Data segment */
    uint64_t es;        /**< Extra segment */
    uint64_t fs;        /**< FS segment (TLS) */
    uint64_t gs;        /**< GS segment (per-CPU) */

    /* Control registers */
    uint64_t cr3;       /**< Page table base (physical) */

    /* FPU/SSE state pointer (NULL if not used) */
    void* fpu_state;    /**< Saved FPU/SSE state */
} MosCpuContext;

/**
 * @brief Process Control Block (PCB).
 *
 * Contains all state for a process. Linked into scheduler queues.
 */
typedef struct MosProcess {
    /* Identity (ternary PIDs) */
    mos_pid_t           pid;            /**< Process ID (trit9) */
    mos_pid_t           ppid;           /**< Parent PID */
    mos_uid_t           uid;            /**< User ID (trit9) */
    mos_gid_t           gid;            /**< Group ID (trit9) */

    /* State (ternary) */
    trit5_t             state;          /**< Process state (0-242 or 243-255) */
    trit5_t             priority;       /**< Scheduling priority */
    MosZone             zone;           /**< Security zone (void/word/tov) */
    MosHebrewState      hebrew_state;   /**< Cognitive state for health */

    /* Execution context */
    MosCpuContext       context;        /**< Saved CPU state */
    mos_vaddr_t         kernel_stack;   /**< Kernel stack base */
    mos_vaddr_t         user_stack;     /**< User stack base */

    /* Memory */
    mos_paddr_t         page_table;     /**< CR3 value (page table root) */
    mos_vaddr_t         heap_start;     /**< Heap start address */
    mos_vaddr_t         heap_end;       /**< Current heap end */
    mos_size_t          memory_used;    /**< Total memory used (bytes) */

    /* Scheduling */
    trit9_t             time_slice;     /**< Remaining time slice (ticks) */
    trit27_t            total_time;     /**< Total CPU time used */
    trit27_t            sleep_until;    /**< Wake time (if sleeping) */

    /* Health tracking */
    mos_health_t        health;         /**< Process health score */
    trit27_t            fault_count;    /**< Fault counter */

    /* Name and metadata */
    char                name[32];       /**< Process name */
    mos_flags_t         flags;          /**< Process flags (trit5) */

    /* Linked list pointers (scheduler queues) */
    struct MosProcess*  next;           /**< Next in queue */
    struct MosProcess*  prev;           /**< Previous in queue */
} MosProcess;

/**
 * @brief Scheduler state.
 *
 * Contains ready queues, current process, and statistics.
 */
typedef struct MosScheduler {
    /* Current process */
    MosProcess*         current;        /**< Currently running process */

    /* Process table (sparse, trit5 buckets) */
    MosProcess*         table[MOS_MAX_PROCESSES / 243];  /**< Process table */
    mos_size_t          process_count;  /**< Total process count */

    /* Ready queues (by priority band) */
    MosProcess*         ready_high;     /**< High priority queue head */
    MosProcess*         ready_normal;   /**< Normal priority queue head */
    MosProcess*         ready_low;      /**< Low priority queue head */

    /* Blocked/sleeping lists */
    MosProcess*         blocked;        /**< Blocked on I/O */
    MosProcess*         sleeping;       /**< Sleeping (timed) */

    /* Statistics */
    trit27_t            context_switches; /**< Total context switches */
    trit27_t            idle_time;        /**< Total idle time (ticks) */
    mos_health_t        health;           /**< Scheduler health */
} MosScheduler;

/** @} */ /* end S.3b STRUCTS */

/** S.3c OPAQUE — [Reserved: no opaque types] @{ @} */

/** S.3d ERRORS — [Reserved: uses mos_result_t] @{ @} */

/** @} */ /* end S.3 TYPES */

/**
 * =============================================================================
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * =============================================================================
 *
 * @defgroup process_prototypes S.4 PROTOTYPES — Function Declarations
 * @brief    Process subsystem API.
 *
 * CONTAINS:
 *   - S.4a LIFECYCLE  — Initialization, create, fork, exec, exit, kill, wait
 *   - S.4b ACCESSORS  — Get process, current, state, priority, zone, health
 *   - S.4c OPERATIONS — Schedule, yield, sleep, wake, block, context switch
 *
 * =============================================================================
 * @{ */

/**
 * ---------------------------------------------------------------------------
 * S.4a LIFECYCLE — Process Lifecycle [LIFECYCLE]
 * ---------------------------------------------------------------------------
 *
 * @defgroup process_lifecycle S.4a LIFECYCLE — Process Lifecycle
 * @ingroup process_prototypes
 * @brief    Initialize, create, fork, exec, exit, kill, wait.
 * @{ */

/**
 * @brief Initialize process subsystem.
 *
 * Creates kernel pseudo-process, initializes scheduler queues.
 * Must be called after memory and interrupts are initialized.
 *
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_init(void);

/**
 * @brief Create new process.
 *
 * @param[out] pid  Receives new process ID.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_create(mos_pid_t* pid);

/**
 * @brief Create process with specific attributes.
 *
 * @param[out] pid    Receives new process ID.
 * @param[in]  name   Process name (max 31 chars).
 * @param[in]  parent Parent process ID.
 * @param[in]  zone   Security zone (void/word/tov).
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_create_ex(mos_pid_t* pid, const char* name,
                                    mos_pid_t parent, MosZone zone);

/**
 * @brief Fork current process.
 *
 * Creates child process as copy of current process.
 *
 * @param[out] child_pid  Receives child process ID.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_fork(mos_pid_t* child_pid);

/**
 * @brief Execute program in current process.
 *
 * Replaces current process image with new program.
 *
 * @param[in] path  Path to executable.
 * @param[in] argv  Argument vector (NULL-terminated).
 * @return Does not return on success, error code on failure.
 */
mos_result_t mos_process_exec(const char* path, const char** argv);

/**
 * @brief Terminate process normally.
 *
 * @param pid        Process ID to terminate.
 * @param exit_code  Exit code for parent.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_exit(mos_pid_t pid, int exit_code);

/**
 * @brief Kill process (forced termination).
 *
 * @param pid  Process ID to kill.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_kill(mos_pid_t pid);

/**
 * @brief Wait for child process.
 *
 * Blocks until child terminates.
 *
 * @param pid      Child process ID (or -1 for any).
 * @param[out] status  Receives exit status.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_wait(mos_pid_t pid, int* status);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ---------------------------------------------------------------------------
 * S.4b ACCESSORS — Process State Queries [ACCESSORS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup process_accessors S.4b ACCESSORS — State Queries
 * @ingroup process_prototypes
 * @brief    Get process, current, state, priority, zone, health.
 * @{ */

/**
 * @brief Get process by PID.
 *
 * @param pid  Process ID.
 * @return Process pointer, or NULL if not found.
 */
MosProcess* mos_process_get(mos_pid_t pid);

/**
 * @brief Get current running process.
 *
 * @return Current process pointer.
 */
MosProcess* mos_process_current(void);

/**
 * @brief Set process state.
 *
 * @param pid    Process ID.
 * @param state  New state (trit5).
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_set_state(mos_pid_t pid, trit5_t state);

/**
 * @brief Set process priority.
 *
 * @param pid       Process ID.
 * @param priority  New priority (trit5).
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_set_priority(mos_pid_t pid, trit5_t priority);

/**
 * @brief Get process security zone.
 *
 * @param pid  Process ID.
 * @return Security zone.
 */
MosZone mos_process_zone(mos_pid_t pid);

/**
 * @brief Set process security zone.
 *
 * Requires appropriate privileges for escalation.
 *
 * @param pid   Process ID.
 * @param zone  New security zone.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_process_set_zone(mos_pid_t pid, MosZone zone);

/**
 * @brief Get process health score.
 *
 * @param pid  Process ID.
 * @return Health score.
 */
mos_health_t mos_process_health(mos_pid_t pid);

/**
 * @brief Get total process count.
 *
 * @return Number of active processes.
 */
mos_size_t mos_process_count(void);

/**
 * @brief Get scheduler state.
 *
 * @return Pointer to scheduler (read-only).
 */
const MosScheduler* mos_scheduler_info(void);

/** @} */ /* end S.4b ACCESSORS */

/**
 * ---------------------------------------------------------------------------
 * S.4c OPERATIONS — Scheduling and Context [OPERATIONS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup process_operations S.4c OPERATIONS — Scheduling
 * @ingroup process_prototypes
 * @brief    Schedule, yield, sleep, wake, block, context switch.
 * @{ */

/** @name Scheduling
 *  @brief Scheduler control functions.
 *  @{ */

/**
 * @brief Yield CPU voluntarily.
 *
 * Gives up remaining time slice, reschedules.
 */
void mos_yield(void);

/**
 * @brief Schedule next process.
 *
 * Called from timer interrupt for preemption.
 * Selects next process and performs context switch.
 */
void mos_schedule(void);

/**
 * @brief Sleep for specified duration.
 *
 * @param duration  Sleep time (nanoseconds as trit27).
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_sleep(mos_time_t duration);

/**
 * @brief Wake sleeping process.
 *
 * @param pid  Process ID to wake.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_wake(mos_pid_t pid);

/**
 * @brief Block current process.
 *
 * Moves current process to blocked queue.
 *
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_block(void);

/**
 * @brief Unblock process.
 *
 * Moves process from blocked to ready queue.
 *
 * @param pid  Process ID to unblock.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_unblock(mos_pid_t pid);

/** @} */ /* end Scheduling */

/** @name Context Switch
 *  @brief Low-level context switch functions (called from schedule).
 *  @{ */

/**
 * @brief Switch to process.
 *
 * Low-level context switch. Saves current, restores next.
 *
 * @param next  Process to switch to.
 */
void mos_context_switch(MosProcess* next);

/**
 * @brief Save current CPU context.
 *
 * @param[out] ctx  Context structure to fill.
 */
void mos_context_save(MosCpuContext* ctx);

/**
 * @brief Restore CPU context.
 *
 * @param[in] ctx  Context to restore.
 */
void mos_context_restore(MosCpuContext* ctx);

/** @} */ /* end Context Switch */

/** @} */ /* end S.4c OPERATIONS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * =============================================================================
 * S.5 FILE-LEVEL — File-Level Declarations [FILE_LEVEL]
 * =============================================================================
 *
 * @defgroup process_file_level S.5 FILE-LEVEL — File-Level Declarations
 * @brief    Global variables and function types.
 *
 * CONTAINS:
 *   - S.5a VARIABLES      — [Reserved: scheduler state in source]
 *   - S.5b FUNC_TYPES     — [Reserved: no function types needed]
 *   - S.5c INFRASTRUCTURE — [Reserved: in source file]
 *
 * =============================================================================
 * @{ */

/** S.5a VARIABLES — [Reserved: scheduler state in process.c] @{ @} */

/** S.5b FUNC_TYPES — [Reserved: no function types needed] @{ @} */

/** S.5c INFRASTRUCTURE — [Reserved: in source file] @{ @} */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * =============================================================================
 * S.6 OMISSION — Omission Guide [OMISSION]
 * =============================================================================
 *
 * @defgroup process_omission S.6 OMISSION — Omission Guide
 * @brief    Intentional omissions for this file.
 *
 * Reserved Sections:
 *   - S.1a: Freestanding kernel, no system includes
 *   - S.1c: No external dependencies
 *   - S.1d: Kernel C code, no C++ linkage
 *   - S.2.0: Uses kernel.h primitives
 *   - S.2b-e: Operations, verification, lookup in implementation
 *   - S.3a: States as defines for trit5 flexibility
 *   - S.3c: No opaque types needed
 *   - S.3d: Uses mos_result_t from kernel.h
 *   - S.5: File-level state in source file
 *
 * =============================================================================
 * @{ */

/* [Documented in section headers above] */

/** @} */ /* end S.6 OMISSION */

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY BLOCK [BODY]
// =============================================================================
//
// @brief Reserved — type-header pattern places all declarations in SETUP.
//
// CONTAINS:
//   - B.1 ORG CHART    — [Reserved: types in S.3]
//   - B.2 TYPES        — [Reserved: types in S.3]
//   - B.3 HELPERS      — [Reserved: in implementation]
//   - B.4 ERRORS       — [Reserved: uses mos_result_t]
//   - B.5 PUBLIC API   — [Reserved: prototypes in S.4]
//
// =============================================================================

/** B.1 ORG CHART — [Reserved: types in S.3, prototypes in S.4] @{ @} */

/** B.2 TYPES — [Reserved: types in S.3] @{ @} */

/** B.3 HELPERS — [Reserved: in implementation] @{ @} */

/** B.4 ERRORS — [Reserved: uses mos_result_t] @{ @} */

/** B.5 PUBLIC API — [Reserved: prototypes in S.4] @{ @} */

// =============================================================================
// END BODY
// =============================================================================

/// =============================================================================
/// CLOSING [CLOSING]
///
/// 4-Block Code Structure: Operational guidance and closing notes
///
/// Section order: Validation → Execution → Cleanup | Policy → Extension →
///                Troubleshooting → Reference → Note → Omission
/// Flow: verify → run → release | modify → extend → debug → lookup → ground
///
/// CONTAINS:
///   - X.1 VALIDATION    — Build, syntax, self-containment verification
///   - X.2 EXECUTION     — Usage patterns and examples
///   - X.3 CLEANUP       — Ownership, memory management
///   - X.4 POLICY        — Modification guidance
///   - X.5 EXTENSION     — How to expand this component
///   - X.6 TROUBLESHOOT  — Common problems and solutions
///   - X.7 REFERENCE     — Quick lookup and copy-paste examples
///   - X.8 NOTE          — Closing guidance and grounding
///   - X.9 OMISSION      — What can be reserved in CLOSING
///
/// =============================================================================

/**
 * =============================================================================
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * =============================================================================
 *
 * @defgroup process_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build, syntax, and self-containment verification.
 *
 * =============================================================================
 * @{
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * make clean && make              # full build
 * make run                        # test in QEMU
 * @endcode
 *
 * @par X.1b CHECKS — Verification Checklist
 *   - Self-contained (includes without errors)
 *   - MosCpuContext matches context_switch.asm expectations
 *   - MosProcess layout stable (context switch depends on offsets)
 *   - trit5 state values within valid range (0-242) or metadata (243-255)
 *
 * @} */

/* X.1c TRIPWIRES — Compile-time assertions */

/* Value tripwires: process limits
 * NOTE: kernel.h defines MOS_PID_KERNEL=0 for legacy. When ternary PID
 * space is enabled in kernel.h, these tripwires verify the model. */
#if defined(MOS_TERNARY_PID_SPACE)
_Static_assert(MOS_MAX_PROCESSES == 19683,
    "Max processes must be 3^9 = 19683");
_Static_assert(MOS_PID_KERNEL == 9841,
    "Kernel PID must be center value");
_Static_assert(MOS_PID_KERNEL == MOS_MAX_PROCESSES / 2,
    "Kernel PID must be exactly center");
#endif /* MOS_TERNARY_PID_SPACE */

/* State tripwires: trit5 values */
_Static_assert(MOS_PROC_STATE_READY == 121,
    "Ready state must be center (121)");
_Static_assert(MOS_PRIO_NORMAL == 121,
    "Normal priority must be center (121)");

/* Quantum tripwires: powers of 3 */
_Static_assert(MOS_QUANTUM_MIN == 3,
    "Min quantum must be 3^1");
_Static_assert(MOS_QUANTUM_DEFAULT == 9,
    "Default quantum must be 3^2");
_Static_assert(MOS_QUANTUM_MAX == 27,
    "Max quantum must be 3^3");

/**
 * =============================================================================
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * =============================================================================
 *
 * @defgroup process_execution X.2 EXECUTION — Usage Patterns
 * @brief    Usage patterns and scheduling flow.
 *
 * =============================================================================
 * @{
 *
 * @par X.2a FLOW — Scheduling Flow
 * @code{.txt}
 *   Timer IRQ
 *       │
 *       ▼
 *   mos_schedule()
 *       │
 *       ├──► current->time_slice-- (decrement quantum)
 *       │
 *       ├──► if (time_slice > 0) return (continue current)
 *       │
 *       ├──► select next from ready queues (priority order)
 *       │
 *       ▼
 *   mos_context_switch(next)
 *       │
 *       ├──► save current registers to current->context
 *       ├──► load CR3 from next->page_table
 *       ├──► restore next registers from next->context
 *       │
 *       ▼
 *   next process runs
 * @endcode
 *
 * @par X.2b USAGE — Create and Run Process
 * @code{.c}
 * mos_pid_t pid;
 * mos_result_t r = mos_process_create_ex(&pid, "shell",
 *                                         MOS_PID_KERNEL, MOS_ZONE_WORD);
 * if (MOS_OK(r)) {
 *     // Process created, will be scheduled when ready
 * }
 * @endcode
 *
 * @par X.2c YIELD — Voluntary Yield Pattern
 * @code{.c}
 * while (waiting_for_io) {
 *     mos_yield();  // Give up CPU, try again next quantum
 * }
 * @endcode
 *
 * @} */

/**
 * =============================================================================
 * X.3 CLEANUP — Ownership and Memory [CLEANUP]
 * =============================================================================
 *
 * @defgroup process_cleanup X.3 CLEANUP — Ownership and Memory
 * @brief    Ownership patterns and memory management.
 *
 * =============================================================================
 * @{
 *
 * @par X.3a OWNERSHIP — Ownership Conventions
 *   - MosProcess*: Owned by scheduler, freed on mos_process_exit()
 *   - page_table: Owned by process, freed with process
 *   - kernel_stack/user_stack: Owned by process, freed with process
 *   - fpu_state: Owned by process, freed with process
 *
 * @par X.3b MEMORY — Memory Patterns
 *   - Process table is statically allocated (sparse buckets)
 *   - Stacks allocated from frame allocator
 *   - Page tables allocated from frame allocator
 *   - No heap allocation in scheduler hot paths
 *
 * @par X.3c SHUTDOWN — Process Cleanup
 *   - mos_process_exit() marks process as zombie
 *   - Parent calls mos_process_wait() to collect exit status
 *   - Zombie is then fully cleaned up (stacks, page tables freed)
 *
 * @} */

/**
 * =============================================================================
 * X.4 POLICY — Modification Guidance [POLICY]
 * =============================================================================
 *
 * @defgroup process_policy X.4 POLICY — Modification Guidance
 * @brief    Guide future maintainers — what's safe to change.
 *
 * SAFE TO MODIFY:
 *   - Add new process states (keep in trit5 range)
 *   - Add new priority levels
 *   - Extend MosProcess with new fields (at end)
 *   - Add new lifecycle functions
 *
 * MODIFY WITH CARE:
 *   - MosCpuContext layout — must match context_switch.asm
 *   - MosProcess layout — offset-sensitive for ASM access
 *   - Scheduler queue structure — affects scheduling fairness
 *
 * NEVER MODIFY:
 *   - Ternary PID semantics (negative=system, zero=kernel, positive=user)
 *   - trit5 value ranges (0-242 valid, 243-255 metadata)
 *   - Quantum power-of-3 pattern
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * =============================================================================
 *
 * @defgroup process_extension X.5 EXTENSION — How to Expand
 * @brief    How to expand this component — mirrors M.10 Roadmap.
 *
 * ADDING SMP SUPPORT:
 *   1. Add per-CPU scheduler state (cpu_id field in MosScheduler)
 *   2. Add spinlocks for scheduler queues
 *   3. Implement cross-CPU IPI for load balancing
 *   4. Add CPU affinity field to MosProcess
 *
 * ADDING IPC:
 *   1. Add message queue to MosProcess
 *   2. Implement mos_send() / mos_receive()
 *   3. Add blocking/waking for message wait
 *   4. Integrate with microkernel service model
 *
 * ADDING USER-SPACE SERVICES:
 *   1. Create service process with MOS_ZONE_WORD
 *   2. Implement system call interface
 *   3. Route requests through IPC
 *   4. Handle privilege transitions
 *
 * See M.10 for full roadmap.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * =============================================================================
 *
 * @defgroup process_troubleshoot X.6 TROUBLESHOOTING — Common Problems
 * @brief    Common problems and solutions.
 *
 * "Triple fault on context switch":
 *   - Check MosCpuContext layout matches ASM
 *   - Verify CR3 value is valid physical address
 *   - Check stack pointer is valid
 *
 * "Process never runs":
 *   - Check process state is READY
 *   - Verify process is in a ready queue
 *   - Check timer interrupt is firing
 *
 * "Scheduler starvation":
 *   - Check priority levels are reasonable
 *   - Verify no infinite loops in high-priority processes
 *   - Consider round-robin within priority levels
 *
 * "Memory leak on process exit":
 *   - Ensure parent calls mos_process_wait()
 *   - Check stacks and page tables are freed
 *   - Verify zombie cleanup completes
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * =============================================================================
 *
 * @defgroup process_reference X.7 REFERENCE — Quick Lookup
 * @brief    Copy-paste ready examples and quick reference.
 *
 * @par Types
 *   - MosCpuContext  — x86-64 register state
 *   - MosProcess     — Process control block
 *   - MosScheduler   — Scheduler state
 *
 * @par Functions
 *   Init:      mos_process_init()
 *   Lifecycle: mos_process_create(), mos_process_create_ex(),
 *              mos_process_fork(), mos_process_exec(),
 *              mos_process_exit(), mos_process_kill(), mos_process_wait()
 *   Accessors: mos_process_get(), mos_process_current(),
 *              mos_process_set_state(), mos_process_set_priority(),
 *              mos_process_zone(), mos_process_set_zone(),
 *              mos_process_health(), mos_process_count(), mos_scheduler_info()
 *   Schedule:  mos_yield(), mos_schedule(), mos_sleep(), mos_wake(),
 *              mos_block(), mos_unblock()
 *   Context:   mos_context_switch(), mos_context_save(), mos_context_restore()
 *
 * @par Quick Copy-Paste
 * @code{.c}
 * // Create process
 * mos_pid_t pid;
 * mos_process_create_ex(&pid, "init", MOS_PID_KERNEL, MOS_ZONE_TOV);
 *
 * // Yield CPU
 * mos_yield();
 *
 * // Sleep for 1 second (in nanoseconds)
 * mos_sleep(1000000000);
 * @endcode
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.8 NOTE — Closing Guidance [NOTE]
 * =============================================================================
 *
 * @defgroup process_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding — echoes M.8 Scripture.
 *
 * This header declares the process management interface for the hybrid
 * microkernel. The kernel keeps only essential services (memory, interrupts,
 * IPC, scheduling) while other services run in user-space.
 *
 * @par Scripture
 * "And God said, Let the earth bring forth grass, the herb yielding seed,
 *  and the fruit tree yielding fruit after his kind."
 *   — Genesis 1:11
 *
 * Each process brings forth work, categorized by kind:
 *   - System processes (-): Infrastructure that enables
 *   - Kernel (0): The foundation itself
 *   - User processes (+): Fruit-bearing applications
 *
 * The ternary PID space reflects this biblical taxonomy.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.9 OMISSION — What Can Be Reserved [OMISSION]
 * =============================================================================
 *
 * @defgroup process_closing_omission X.9 OMISSION — What Can Be Reserved
 * @brief    Guidance for CLOSING sections — what can be reserved.
 *
 * All sections present. Reserved items:
 *   - X.9 TEMPLATE: Removed (this is instantiated, not a template)
 *
 * =============================================================================
 */

// =============================================================================
// END CLOSING
// =============================================================================

#endif /* MOS_KERNEL_CORE_PROCESS_H */
