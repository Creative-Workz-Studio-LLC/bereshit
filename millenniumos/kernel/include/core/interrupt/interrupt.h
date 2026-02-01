/// @file interrupt.h
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
/// @omni:req  key        = MOS-kernel-core-interrupt
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
/// @omni:ins  component  = kernel/core/interrupt
/// @omni:ins  layer      = 0 (kernel layer)
/// @omni:ins  cpisi_mode = 0 (freestanding)
/// @omni:ins  provides   = [MosInterruptFrame, MosInterruptHandler, MosIdtEntry,
///                          mos_interrupt_init, mos_interrupt_register, mos_sti, mos_cli]
///
/// ---------------------------------------------------------------------------
/// P.4 ARCHITECTURE — Scaling Context [ARCH]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  layers     = [0:kernel, 1:hal, 2:services, 3:framework, 4:app]
/// @omni:ins  position   = 0 (kernel foundation)
/// @omni:ins  depends    = [kernel.h]
///
/// ---------------------------------------------------------------------------
/// P.5 SUMMARY — Human Identity [SUMMARY]
/// ---------------------------------------------------------------------------
///
/// @omni:req  title      = MillenniumOS Interrupt Handling
/// @omni:req  brief      = IDT, handlers, exceptions with ternary organization
///
/// =============================================================================
/// END PRAGMA
/// =============================================================================

// =============================================================================
// METADATA BLOCK [METADATA]
// =============================================================================
//
// @brief Identity and context for interrupt subsystem.
//
// STRUCTURE: M.1-M.10 sections for complete documentation.
//
// =============================================================================

/**
 * =============================================================================
 * M.1 IDENTITY — Core Identification [IDENTITY]
 * =============================================================================
 *
 * @file      interrupt.h
 * @brief     MillenniumOS Interrupt Handling — IDT, handlers, and exceptions.
 *
 * Key:       MOS-kernel-core-interrupt
 * Title:     MillenniumOS Interrupt Handling
 * Type:      Header (API)
 * Component: Kernel / Core / Interrupt Subsystem
 * Role:      IDT setup, interrupt handlers, and exception management
 *
 * INTERFACE SUMMARY:
 *   Types (4):     MosInterruptType, MosInterruptFrame, MosInterruptDescriptor,
 *                  MosIdtEntry, MosIdtPointer
 *   Functions (14): 2 lifecycle, 3 accessors, 9 operations (4 inline)
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
 * Path:      millenniumos/kernel/include/core/interrupt/interrupt.h
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
 * Tags:      kernel, interrupt, idt, exception, irq, handler, ternary
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.7 INTENT — Purpose Statement [INTENT]
 * =============================================================================
 *
 * Purpose:   Provide interrupt descriptor table (IDT) management, exception
 *            handlers, IRQ routing, and interrupt state control using
 *            ternary organization: exceptions (-1), system (0), IRQs (+1).
 *
 * TERNARY ARCHITECTURE:
 *   - Exceptions (0-31):  CPU-generated errors → negative semantic (-1)
 *   - System (0x80):      Syscall vector → neutral semantic (0)
 *   - IRQs (32-47):       Hardware events → positive semantic (+1)
 *
 * LAYER POSITION:
 *
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │ LAYER 4: Application                                                │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ LAYER 3: Framework                                                  │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ LAYER 2: Services                                                   │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ LAYER 1: HAL (keyboard, mouse drivers use IRQs)                     │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ LAYER 0: Kernel Core                                                │
 *   │   ├── memory.h    — Frame allocation, heap                          │
 *   │   ├── interrupt.h — IDT, handlers, IRQ control  ◄── THIS FILE       │
 *   │   └── process.h   — Scheduling, context switch                      │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * =============================================================================
 *
 * Scripture: Genesis 1:4 — "And God saw the light, that it was good:
 *            and God divided the light from the darkness."
 *
 * Principle: Interrupts divide execution flow — separating events from main
 *            flow, darkness of busy-waiting from light of event-driven.
 *
 * TERNARY MAPPING:
 *   - Exceptions (-1): Darkness — errors that disrupt normal flow
 *   - System (0):      Boundary — the dividing point (syscalls)
 *   - IRQs (+1):       Light — events that enable progress
 *
 * The division is GOOD — it creates order from chaos.
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
 *   │ x86-64 CPU                   — IDT, IDTR, PIC hardware              │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ kernel.h                     — Core types (mos_result_t, trit_t)    │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ interrupt.h                  ◄── THIS FILE                          │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ process.h                    — Timer IRQ for scheduling             │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │ hal/keyboard.c, hal/mouse.c  — IRQ handlers                         │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * BATON (initialization chain):
 *
 *   kernel.c (init)
 *        │
 *        ▼
 *   mos_interrupt_init() ──► IDT loaded, exceptions installed
 *        │
 *        ▼
 *   mos_pic_init() ───────► PIC remapped, IRQs routed to 32-47
 *        │
 *        ▼
 *   HAL drivers call mos_interrupt_register() for their IRQs
 *        │
 *        ▼
 *   mos_sti() ────────────► Interrupts enabled, system responsive
 *
 * KEY INTERFACES (baton passed between components):
 *   - MosInterruptHandler: Callback signature all handlers must match
 *   - MosInterruptFrame*:  Context passed to every handler
 *   - mos_irq_save/restore: Critical section primitive
 *
 * SIGNALS:
 *   Sent:     Timer tick → scheduler, Keyboard/Mouse IRQ → HAL drivers
 *   Received: mos_irq_eoi() ← HAL drivers, mos_cli/sti ← critical sections
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
 *   a-01.00 (2026-01-30) — Initial interrupt subsystem
 *     • IDT structure with 256 entries
 *     • PIC initialization and remapping
 *     • Basic exception handling
 *   a-02.00 (2026-02-01) — Full OmniCode template alignment
 *     • 4-block structure with type-header pattern
 *     • LADDER/BATON documentation
 *     • Health scoring thresholds
 *     • Ternary interrupt classification
 *
 * M.10b COMPLETED — What's Done
 * -----------------------------
 *   ✓ x86-64 IDT structure (256 entries)
 *   ✓ Exception vectors (0-31 with error codes)
 *   ✓ PIC remapping (IRQs to 32-47)
 *   ✓ Handler registration API
 *   ✓ Inline interrupt control (sti/cli)
 *   ✓ IRQ save/restore for critical sections
 *   ✓ Per-vector statistics tracking
 *   ✓ Ternary type classification (-1/0/+1)
 *   ✓ DAR Witness integration
 *
 * M.10c PHASE 1 — Single-Core Foundation (a-02.xx) ✓ COMPLETE
 * ------------------------------------------------------------
 *   ✓ Spurious interrupt detection (IRQ 7/15)
 *   ✓ Latency measurement (TSC-based per interrupt)
 *   ✓ Timer→Scheduler integration (preemptive multitasking)
 *   ⏳ Interrupt descriptor bounds checking
 *   ⏳ Stack overflow detection on IRQ
 *
 *   Dependencies: timer.h for timestamp sourcing
 *   Integration: DAR Witness for interrupt logging
 *
 * M.10d PHASE 2 — APIC Support (a-03.xx)
 * --------------------------------------
 *   ⏳ Local APIC detection (CPUID, ACPI MADT)
 *   ⏳ APIC initialization replacing PIC
 *   ⏳ APIC timer for scheduler tick
 *   ⏳ Vector remapping for APIC (base 32+)
 *   ⏳ End-of-interrupt for APIC vs PIC
 *
 *   Dependencies: ACPI parsing for MADT table
 *   Integration: process.h scheduler for APIC timer
 *
 * M.10e PHASE 3 — SMP Interrupts (a-04.xx)
 * ----------------------------------------
 *   ⏳ I/O APIC initialization
 *   ⏳ Interrupt routing table (IRQ → CPU)
 *   ⏳ Inter-Processor Interrupts (IPI)
 *     - TLB shootdown IPI
 *     - Scheduler kick IPI
 *     - Panic IPI (halt all CPUs)
 *   ⏳ IRQ affinity API
 *   ⏳ Per-CPU interrupt statistics
 *
 *   Dependencies: SMP boot (AP startup via SIPI)
 *   Integration: process.h for cross-CPU scheduling
 *
 * M.10f PHASE 4 — MSI/MSI-X (b-01.xx)
 * ------------------------------------
 *   ⏳ MSI capability detection (PCIe config)
 *   ⏳ MSI vector allocation
 *   ⏳ MSI-X table management
 *   ⏳ Per-device interrupt vectors
 *
 *   Dependencies: PCI/PCIe driver framework
 *   Integration: HAL device driver model
 *
 * M.10g PHASE 5 — Advanced Features (b-02.xx)
 * --------------------------------------------
 *   ⏳ Nested interrupt handling (priority inversion)
 *   ⏳ Interrupt coalescing for high-rate devices
 *   ⏳ Interrupt load balancing
 *   ⏳ Real-time interrupt priorities
 *
 * M.10h MICROKERNEL INTEGRATION
 * -----------------------------
 *   The interrupt subsystem is CORE KERNEL — remains in kernel space.
 *   Device drivers (HAL) run in user space and receive interrupts via:
 *     1. Kernel catches IRQ in IDT handler
 *     2. Kernel sends message to registered driver process (IPC)
 *     3. Driver handles interrupt in user space
 *     4. Driver acks completion via IPC
 *     5. Kernel issues EOI
 *
 *   This maintains separation while enabling efficient interrupt delivery.
 *
 * M.10i BOOT PATH SUPPORT
 * -----------------------
 *   UEFI Path: ExitBootServices → kernel → mos_interrupt_init() → PIC/APIC
 *   BIOS Path: Stage 0b → kernel → mos_interrupt_init() → PIC only
 *
 *   Both paths converge at mos_interrupt_init(). The APIC vs PIC decision
 *   is made based on hardware detection, not boot path.
 *
 * =============================================================================
 */

// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_KERNEL_CORE_INTERRUPT_H
#define MOS_KERNEL_CORE_INTERRUPT_H

// =============================================================================
// SETUP BLOCK [SETUP]
// =============================================================================
//
// @brief Configuration and dependencies before body.
//
// CONTAINS:
//   - S.1 INCLUDES   — Header dependencies (kernel.h)
//   - S.2 DEFINES    — Interrupt vectors, IDT constants, health thresholds
//   - S.3 TYPES      — Type declarations (frame, handler, IDT)
//   - S.4 PROTOTYPES — Function declarations (init, register, control)
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
 * @defgroup interrupt_includes S.1 INCLUDES — Header Dependencies
 * @brief    Standard, internal, and external library includes.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — [Reserved: freestanding kernel]
 *   - S.1b INTERNAL  — Project headers (kernel.h)
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
 * @defgroup interrupt_internal S.1b INTERNAL — Project Headers
 * @ingroup interrupt_includes
 * @brief    Internal kernel headers required by this module.
 * @{ */

#include "kernel.h"    /**< LAYER 0: Foundation (MUST be first) */

/** @} */ /* end S.1b INTERNAL */

/** S.1c EXTERNAL — [Reserved: no external dependencies] @{ @} */

/** S.1d CPP — [Reserved: kernel C code] @{ @} */

/** @} */ /* end S.1 INCLUDES */

/**
 * =============================================================================
 * S.2 DEFINES — Interrupt Domain Definitions [DEFINES]
 * =============================================================================
 *
 * @defgroup interrupt_domain S.2 DEFINES — Interrupt Domain
 * @brief    Interrupt vectors, IDT constants, IRQ mappings, health thresholds.
 *
 * CONTAINS:
 *   - S.2.0 PRELUDE — [Reserved: uses kernel.h primitives]
 *   - S.2a  BOUNDS  — Exception, IRQ, syscall, IDT, health constants
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
 * S.2a BOUNDS — Interrupt Vector Constants [BOUNDS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup interrupt_bounds S.2a BOUNDS — Interrupt Vectors
 * @ingroup interrupt_domain
 * @brief    Exception, IRQ, syscall, IDT, and health threshold definitions.
 *
 * TERNARY ORGANIZATION:
 *   - Exceptions (0-31):   CPU errors → negative semantic
 *   - System (0x80):       Syscall → neutral semantic
 *   - IRQs (32-47):        Hardware → positive semantic
 *
 * @{ */

/** @name Exception Vectors — CPU-generated (mapped to negative semantic)
 *  @brief Vectors 0-31 reserved by x86 architecture.
 *  @{ */
#define MOS_INT_DIVIDE_ERROR    0       /**< #DE: Division by zero */
#define MOS_INT_DEBUG           1       /**< #DB: Debug exception */
#define MOS_INT_NMI             2       /**< NMI: Non-maskable interrupt */
#define MOS_INT_BREAKPOINT      3       /**< #BP: Breakpoint (INT3) */
#define MOS_INT_OVERFLOW        4       /**< #OF: Overflow (INTO) */
#define MOS_INT_BOUND           5       /**< #BR: BOUND range exceeded */
#define MOS_INT_INVALID_OP      6       /**< #UD: Invalid/undefined opcode */
#define MOS_INT_DEVICE_NA       7       /**< #NM: Device not available */
#define MOS_INT_DOUBLE_FAULT    8       /**< #DF: Double fault (error code) */
#define MOS_INT_COPROC_SEG      9       /**< Reserved: Coprocessor segment */
#define MOS_INT_INVALID_TSS     10      /**< #TS: Invalid TSS (error code) */
#define MOS_INT_SEGMENT_NP      11      /**< #NP: Segment not present (error code) */
#define MOS_INT_STACK_FAULT     12      /**< #SS: Stack segment fault (error code) */
#define MOS_INT_GPF             13      /**< #GP: General protection (error code) */
#define MOS_INT_PAGE_FAULT      14      /**< #PF: Page fault (error code) */
#define MOS_INT_RESERVED_15     15      /**< Reserved */
#define MOS_INT_X87_FPU         16      /**< #MF: x87 FPU error */
#define MOS_INT_ALIGNMENT       17      /**< #AC: Alignment check (error code) */
#define MOS_INT_MACHINE_CHECK   18      /**< #MC: Machine check */
#define MOS_INT_SIMD_FPU        19      /**< #XM/#XF: SIMD floating-point */
#define MOS_INT_VIRT            20      /**< #VE: Virtualization exception */
#define MOS_INT_CONTROL_PROT    21      /**< #CP: Control protection */
/** @} */ /* end Exception Vectors */

/** @name IRQ Vectors — Hardware interrupts (mapped to positive semantic)
 *  @brief Vectors 32-47 for legacy PIC (remapped from 0-15).
 *  @{ */
#define MOS_IRQ_BASE            32      /**< IRQ base after PIC remap */
#define MOS_IRQ_TIMER           (MOS_IRQ_BASE + 0)   /**< IRQ0: PIT/HPET timer */
#define MOS_IRQ_KEYBOARD        (MOS_IRQ_BASE + 1)   /**< IRQ1: PS/2 keyboard */
#define MOS_IRQ_CASCADE         (MOS_IRQ_BASE + 2)   /**< IRQ2: Cascade to slave */
#define MOS_IRQ_COM2            (MOS_IRQ_BASE + 3)   /**< IRQ3: Serial COM2/COM4 */
#define MOS_IRQ_COM1            (MOS_IRQ_BASE + 4)   /**< IRQ4: Serial COM1/COM3 */
#define MOS_IRQ_LPT2            (MOS_IRQ_BASE + 5)   /**< IRQ5: LPT2 / sound */
#define MOS_IRQ_FLOPPY          (MOS_IRQ_BASE + 6)   /**< IRQ6: Floppy disk */
#define MOS_IRQ_LPT1            (MOS_IRQ_BASE + 7)   /**< IRQ7: LPT1 / spurious */
#define MOS_IRQ_RTC             (MOS_IRQ_BASE + 8)   /**< IRQ8: RTC */
#define MOS_IRQ_FREE1           (MOS_IRQ_BASE + 9)   /**< IRQ9: Free / ACPI */
#define MOS_IRQ_FREE2           (MOS_IRQ_BASE + 10)  /**< IRQ10: Free */
#define MOS_IRQ_FREE3           (MOS_IRQ_BASE + 11)  /**< IRQ11: Free */
#define MOS_IRQ_MOUSE           (MOS_IRQ_BASE + 12)  /**< IRQ12: PS/2 mouse */
#define MOS_IRQ_COPROC          (MOS_IRQ_BASE + 13)  /**< IRQ13: FPU / IPC */
#define MOS_IRQ_ATA_PRIMARY     (MOS_IRQ_BASE + 14)  /**< IRQ14: Primary ATA */
#define MOS_IRQ_ATA_SECONDARY   (MOS_IRQ_BASE + 15)  /**< IRQ15: Secondary ATA */
/** @} */ /* end IRQ Vectors */

/** @name System Vectors — Neutral semantic
 *  @brief Syscall and system-level vectors.
 *  @{ */
#define MOS_INT_SYSCALL         0x80    /**< Traditional Linux-style syscall */
/** @} */ /* end System Vectors */

/** @name IDT Configuration
 *  @brief IDT size and entry counts.
 *  @{ */
#define MOS_IDT_ENTRIES         256     /**< Total IDT entries (x86-64) */
#define MOS_EXCEPTION_COUNT     32      /**< Reserved exception vectors */
#define MOS_IRQ_COUNT           16      /**< Legacy PIC IRQ count */
/** @} */ /* end IDT Configuration */

/** @name Health Thresholds
 *  @brief Threshold values for interrupt subsystem health scoring.
 *
 *  SCORING (ternary -1/0/+1 per metric):
 *    Metric            | -1 (Degraded)  | 0 (Normal)     | +1 (Optimal)
 *    ------------------|----------------|----------------|----------------
 *    Handler Coverage  | <80% registered| 80-95%         | >95% registered
 *    Error Rate        | >10 exc/sec    | 1-10 exc/sec   | <1 exc/sec
 *    Latency           | >100µs avg     | 10-100µs       | <10µs avg
 *    Spurious Count    | >100/min       | 10-100/min     | <10/min
 *    Nested Depth      | >4 levels      | 2-4 levels     | ≤1 level
 *
 *  HEALTH RANGE: -5 to +5 (5 metrics × trit)
 *
 *  @{ */
#define MOS_INT_HEALTH_HANDLER_THRESHOLD  (MOS_IDT_ENTRIES * 80 / 100)  /**< 80% coverage */
#define MOS_INT_HEALTH_ERROR_HIGH         10      /**< Exceptions per second */
#define MOS_INT_HEALTH_ERROR_LOW          1       /**< Exceptions per second */
#define MOS_INT_HEALTH_LATENCY_HIGH       100     /**< Microseconds */
#define MOS_INT_HEALTH_LATENCY_LOW        10      /**< Microseconds */
#define MOS_INT_HEALTH_SPURIOUS_HIGH      100     /**< Per minute */
#define MOS_INT_HEALTH_SPURIOUS_LOW       10      /**< Per minute */
#define MOS_INT_HEALTH_NEST_HIGH          4       /**< Nesting levels */
#define MOS_INT_HEALTH_NEST_LOW           1       /**< Nesting levels */
/** @} */ /* end Health Thresholds */

/** @} */ /* end S.2a BOUNDS */

/** S.2b MACROS — [Reserved: operations in kernel.h] @{ @} */

/** S.2c VERIFY — [Reserved: see CLOSING X.1] @{ @} */

/** S.2d LOOKUP — [Reserved: no runtime tables in header] @{ @} */

/** S.2e TYPES — [Reserved: enums in S.3a] @{ @} */

/** @} */ /* end S.2 DEFINES */

/**
 * =============================================================================
 * S.3 TYPES — Interrupt Type Declarations [TYPES]
 * =============================================================================
 *
 * @defgroup interrupt_types S.3 TYPES — Type Declarations
 * @brief    Interrupt frame, handler, and IDT structures.
 *
 * CONTAINS:
 *   - S.3a ENUMS   — Interrupt type classification
 *   - S.3b STRUCTS — Frame, handler descriptor, IDT entry
 *   - S.3c OPAQUE  — [Reserved: no opaque types]
 *   - S.3d ERRORS  — [Reserved: uses mos_result_t]
 *
 * =============================================================================
 * @{ */

/**
 * ---------------------------------------------------------------------------
 * S.3a ENUMS — Interrupt Enumerations [ENUMS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup interrupt_types_enums S.3a ENUMS — Enumeration Types
 * @ingroup interrupt_types
 * @brief    Interrupt type classification using ternary semantics.
 * @{ */

/**
 * @brief Interrupt type classification (ternary semantic).
 *
 * Maps interrupt sources to ternary values:
 *   -1: Exception (CPU error)
 *    0: System (syscall, reserved)
 *   +1: IRQ (hardware event)
 */
typedef enum MosInterruptType {
    MOS_INT_TYPE_EXCEPTION = -1,  /**< CPU-generated exception (negative) */
    MOS_INT_TYPE_SYSTEM    =  0,  /**< System call or reserved (neutral) */
    MOS_INT_TYPE_IRQ       = +1,  /**< Hardware interrupt (positive) */
} MosInterruptType;

/** @} */ /* end S.3a ENUMS */

/**
 * ---------------------------------------------------------------------------
 * S.3b STRUCTS — Interrupt Structures [STRUCTS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup interrupt_types_structs S.3b STRUCTS — Structure Definitions
 * @ingroup interrupt_types
 * @brief    Interrupt frame, handler descriptor, IDT structures.
 * @{ */

/**
 * @brief Interrupt frame pushed by CPU and stub.
 *
 * When an interrupt occurs, CPU pushes SS:RSP, RFLAGS, CS:RIP, and
 * optionally an error code. Our stubs push remaining registers.
 *
 * @note Packed to match hardware-defined layout.
 */
typedef struct __attribute__((packed)) MosInterruptFrame {
    /* Pushed by our stubs (in reverse order) */
    uint64_t r15;           /**< General register R15 */
    uint64_t r14;           /**< General register R14 */
    uint64_t r13;           /**< General register R13 */
    uint64_t r12;           /**< General register R12 */
    uint64_t r11;           /**< General register R11 */
    uint64_t r10;           /**< General register R10 */
    uint64_t r9;            /**< General register R9 */
    uint64_t r8;            /**< General register R8 */
    uint64_t rbp;           /**< Base pointer */
    uint64_t rdi;           /**< Destination index */
    uint64_t rsi;           /**< Source index */
    uint64_t rdx;           /**< Data register */
    uint64_t rcx;           /**< Counter register */
    uint64_t rbx;           /**< Base register */
    uint64_t rax;           /**< Accumulator */

    /* Pushed by stub: interrupt metadata */
    uint64_t int_no;        /**< Interrupt vector number */
    uint64_t error_code;    /**< Error code (or 0 if none) */

    /* Pushed by CPU automatically */
    uint64_t rip;           /**< Return instruction pointer */
    uint64_t cs;            /**< Code segment selector */
    uint64_t rflags;        /**< CPU flags */
    uint64_t rsp;           /**< Stack pointer */
    uint64_t ss;            /**< Stack segment selector */
} MosInterruptFrame;

/**
 * @brief Interrupt handler function signature.
 *
 * @param frame Pointer to interrupt frame with register state.
 */
typedef void (*MosInterruptHandler)(MosInterruptFrame* frame);

/**
 * @brief Handler registration descriptor.
 *
 * Tracks registered handler with metadata for debugging and statistics.
 */
typedef struct MosInterruptDescriptor {
    MosInterruptHandler handler;    /**< Handler function pointer */
    trit_t              type;       /**< -1=exception, 0=system, +1=IRQ */
    trit5_t             flags;      /**< Handler flags (reserved) */
    const char*         name;       /**< Handler name for debugging */
} MosInterruptDescriptor;

/**
 * @brief IDT entry (x86-64 gate descriptor).
 *
 * 16-byte IDT gate descriptor for 64-bit mode.
 *
 * @note Packed to match hardware-defined layout.
 */
typedef struct __attribute__((packed)) MosIdtEntry {
    uint16_t offset_low;    /**< Offset bits 0-15 */
    uint16_t selector;      /**< Code segment selector */
    uint8_t  ist;           /**< Interrupt Stack Table index (0-7) */
    uint8_t  type_attr;     /**< Type and attributes (P, DPL, type) */
    uint16_t offset_mid;    /**< Offset bits 16-31 */
    uint32_t offset_high;   /**< Offset bits 32-63 */
    uint32_t reserved;      /**< Reserved (must be zero) */
} MosIdtEntry;

/**
 * @brief IDT pointer for LIDT instruction.
 */
typedef struct __attribute__((packed)) MosIdtPointer {
    uint16_t limit;         /**< IDT size - 1 */
    uint64_t base;          /**< IDT base address */
} MosIdtPointer;

/** @} */ /* end S.3b STRUCTS */

/** S.3c OPAQUE — [Reserved: no opaque types] @{ @} */

/** S.3d ERRORS — [Reserved: uses mos_result_t] @{ @} */

/** @} */ /* end S.3 TYPES */

/**
 * =============================================================================
 * S.4 PROTOTYPES — Function Declarations [PROTOTYPES]
 * =============================================================================
 *
 * @defgroup interrupt_prototypes S.4 PROTOTYPES — Function Declarations
 * @brief    Interrupt subsystem API.
 *
 * CONTAINS:
 *   - S.4a LIFECYCLE  — Initialization
 *   - S.4b ACCESSORS  — Statistics and queries
 *   - S.4c OPERATIONS — Handler registration, IRQ/interrupt control
 *
 * =============================================================================
 * @{ */

/**
 * ---------------------------------------------------------------------------
 * S.4a LIFECYCLE — Initialization [LIFECYCLE]
 * ---------------------------------------------------------------------------
 *
 * @defgroup interrupt_lifecycle S.4a LIFECYCLE — Initialization
 * @ingroup interrupt_prototypes
 * @brief    Interrupt subsystem initialization.
 * @{ */

/**
 * @brief Initialize interrupt subsystem.
 *
 * Sets up IDT, installs default handlers, and loads IDTR.
 * Must be called before enabling interrupts.
 *
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_interrupt_init(void);

/**
 * @brief Initialize PIC (8259) controller.
 *
 * Remaps IRQs 0-15 to vectors 32-47 to avoid conflict with exceptions.
 * Must be called after IDT is set up.
 *
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_pic_init(void);

/** @} */ /* end S.4a LIFECYCLE */

/**
 * ---------------------------------------------------------------------------
 * S.4b ACCESSORS — Statistics and Queries [ACCESSORS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup interrupt_accessors S.4b ACCESSORS — Statistics
 * @ingroup interrupt_prototypes
 * @brief    Interrupt statistics and queries.
 * @{ */

/**
 * @brief Get total interrupt count.
 *
 * @return Total number of interrupts handled since boot.
 */
trit27_t mos_interrupt_count(void);

/**
 * @brief Get interrupt count for specific vector.
 *
 * @param vector Interrupt vector number (0-255).
 * @return Number of times this vector has been triggered.
 */
uint64_t mos_interrupt_vector_count(uint8_t vector);

/**
 * @brief Check if interrupts are enabled.
 *
 * @return true if IF flag is set, false otherwise.
 */
static inline bool mos_irq_enabled(void) {
    uint64_t flags;
    __asm__ volatile("pushfq; pop %0" : "=r"(flags));
    return (flags & 0x200) != 0;  /* IF flag at bit 9 */
}

/** @name Spurious Interrupt Statistics (M.10 Phase 1)
 *  @brief Track spurious interrupts on IRQ 7 and IRQ 15.
 *  @{ */

/**
 * @brief Get spurious IRQ 7 count.
 *
 * Spurious IRQ 7 occurs when PIC signals interrupt but ISR bit is not set.
 * Common on old hardware or during PIC reconfiguration.
 *
 * @return Number of spurious IRQ 7 events detected.
 */
uint64_t mos_spurious_irq7_count(void);

/**
 * @brief Get spurious IRQ 15 count.
 *
 * Similar to IRQ 7 but for slave PIC.
 *
 * @return Number of spurious IRQ 15 events detected.
 */
uint64_t mos_spurious_irq15_count(void);

/**
 * @brief Get total spurious interrupt count.
 *
 * @return Sum of IRQ 7 and IRQ 15 spurious events.
 */
uint64_t mos_spurious_total(void);

/** @} */ /* end Spurious Interrupt Statistics */

/** @name Latency Statistics (M.10 Phase 1)
 *  @brief Track interrupt handler execution latency.
 *  @{ */

/**
 * @brief Get average interrupt latency in TSC cycles.
 *
 * @return Average TSC cycles spent in interrupt handlers.
 */
uint64_t mos_interrupt_latency_avg(void);

/**
 * @brief Get maximum interrupt latency in TSC cycles.
 *
 * @return Worst-case latency since boot.
 */
uint64_t mos_interrupt_latency_max(void);

/**
 * @brief Get minimum interrupt latency in TSC cycles.
 *
 * @return Best-case latency since boot.
 */
uint64_t mos_interrupt_latency_min(void);

/** @} */ /* end Latency Statistics */

/** @name Timer
 *  @brief Timer tick counter.
 *  @{ */

/**
 * @brief Get timer tick count since boot.
 *
 * @return Number of timer interrupts (IRQ 0) since kernel start.
 */
uint64_t mos_timer_ticks(void);

/** @} */ /* end Timer */

/** @} */ /* end S.4b ACCESSORS */

/**
 * ---------------------------------------------------------------------------
 * S.4c OPERATIONS — Handler and Control [OPERATIONS]
 * ---------------------------------------------------------------------------
 *
 * @defgroup interrupt_operations S.4c OPERATIONS — Handler and Control
 * @ingroup interrupt_prototypes
 * @brief    Handler registration and interrupt control.
 * @{ */

/** @name Handler Registration
 *  @brief Register and unregister interrupt handlers.
 *  @{ */

/**
 * @brief Register interrupt handler.
 *
 * @param vector  Interrupt vector number (0-255).
 * @param handler Handler function to call.
 * @param type    Ternary type: -1=exception, 0=system, +1=IRQ.
 * @param name    Handler name for debugging.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_interrupt_register(uint8_t vector, MosInterruptHandler handler,
                                     trit_t type, const char* name);

/**
 * @brief Unregister interrupt handler.
 *
 * @param vector Interrupt vector number.
 * @return MOS_OK on success, error code on failure.
 */
mos_result_t mos_interrupt_unregister(uint8_t vector);

/** @} */ /* end Handler Registration */

/** @name Global Interrupt Control
 *  @brief Enable, disable, save, and restore interrupt state.
 *  @{ */

/**
 * @brief Enable interrupts (STI instruction).
 */
static inline void mos_sti(void) {
    __asm__ volatile("sti");
}

/**
 * @brief Disable interrupts (CLI instruction).
 */
static inline void mos_cli(void) {
    __asm__ volatile("cli");
}

/**
 * @brief Save interrupt state and disable interrupts.
 *
 * For critical sections. Pairs with mos_irq_restore().
 *
 * @return Previous RFLAGS value for restoration.
 */
static inline uint64_t mos_irq_save(void) {
    uint64_t flags;
    __asm__ volatile(
        "pushfq\n\t"
        "pop %0\n\t"
        "cli"
        : "=r"(flags)
    );
    return flags;
}

/**
 * @brief Restore interrupt state.
 *
 * Restores RFLAGS including IF bit.
 *
 * @param flags Previous RFLAGS from mos_irq_save().
 */
static inline void mos_irq_restore(uint64_t flags) {
    __asm__ volatile(
        "push %0\n\t"
        "popfq"
        :
        : "r"(flags)
        : "memory"
    );
}

/** @} */ /* end Global Interrupt Control */

/** @name IRQ-Specific Control
 *  @brief Enable, disable, and acknowledge specific IRQs.
 *  @{ */

/**
 * @brief Enable specific IRQ at PIC.
 *
 * @param irq IRQ number (0-15).
 */
void mos_irq_enable(uint8_t irq);

/**
 * @brief Disable specific IRQ at PIC.
 *
 * @param irq IRQ number (0-15).
 */
void mos_irq_disable(uint8_t irq);

/**
 * @brief Send End-Of-Interrupt to PIC.
 *
 * @param irq IRQ number (0-15).
 */
void mos_irq_eoi(uint8_t irq);

/** @} */ /* end IRQ-Specific Control */

/** @} */ /* end S.4c OPERATIONS */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * =============================================================================
 * S.5 FILE-LEVEL — File-Level Declarations [FILE_LEVEL]
 * =============================================================================
 *
 * @defgroup interrupt_file_level S.5 FILE-LEVEL — File-Level Declarations
 * @brief    Global variables and function types.
 *
 * CONTAINS:
 *   - S.5a VARIABLES      — [Reserved: no globals in header]
 *   - S.5b FUNC_TYPES     — [Reserved: handler typedef in S.3b]
 *   - S.5c INFRASTRUCTURE — [Reserved: in source file]
 *
 * =============================================================================
 * @{ */

/** S.5a VARIABLES — [Reserved: state in source file] @{ @} */

/** S.5b FUNC_TYPES — [Reserved: MosInterruptHandler in S.3b] @{ @} */

/** S.5c INFRASTRUCTURE — [Reserved: in source file] @{ @} */

/** @} */ /* end S.5 FILE-LEVEL */

/**
 * =============================================================================
 * S.6 OMISSION — Omission Guide [OMISSION]
 * =============================================================================
 *
 * @defgroup interrupt_omission S.6 OMISSION — Omission Guide
 * @brief    Intentional omissions for this file.
 *
 * Reserved Sections:
 *   - S.1a: Freestanding kernel, no system includes
 *   - S.1c: No external dependencies
 *   - S.1d: Kernel C code, no C++ linkage
 *   - S.2.0: Uses kernel.h primitives
 *   - S.2b-e: Operations, verification, lookup in implementation
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
 * @defgroup interrupt_validation X.1 VALIDATION — Verify Correctness
 * @brief    Build, syntax, and self-containment verification.
 *
 * CONTAINS:
 *   - X.1a BUILD     — Compilation commands
 *   - X.1b CHECKS    — Verification checklist
 *   - X.1c TRIPWIRES — Compile-time assertions
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
 *   - No missing type definitions
 *   - IDT structures match x86-64 hardware spec
 *   - Include guards work (no redefinition)
 *   - Prototypes match implementations in handlers.c
 *
 * @} */

/* X.1c TRIPWIRES — Compile-time assertions */

/* Value tripwires: vector constants */
_Static_assert(MOS_INT_DIVIDE_ERROR == 0,
    "Divide error must be vector 0");
_Static_assert(MOS_INT_PAGE_FAULT == 14,
    "Page fault must be vector 14");
_Static_assert(MOS_IRQ_BASE == 32,
    "IRQ base must be 32 (after exceptions)");
_Static_assert(MOS_INT_SYSCALL == 0x80,
    "Syscall must be vector 0x80");

/* Relationship tripwires: IRQ mapping */
_Static_assert(MOS_IRQ_TIMER == MOS_IRQ_BASE + 0,
    "Timer IRQ must be first IRQ");
_Static_assert(MOS_IRQ_KEYBOARD == MOS_IRQ_BASE + 1,
    "Keyboard IRQ must be second IRQ");

/* Size tripwires: structure layout */
_Static_assert(sizeof(MosIdtEntry) == 16,
    "IDT entry must be 16 bytes (x86-64)");
_Static_assert(sizeof(MosIdtPointer) == 10,
    "IDT pointer must be 10 bytes");

/**
 * =============================================================================
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * =============================================================================
 *
 * @defgroup interrupt_execution X.2 EXECUTION — Usage Patterns
 * @brief    Usage patterns and interrupt handling flow.
 *
 * =============================================================================
 * @{
 *
 * @par X.2a FLOW — Interrupt Handling Flow
 * @code{.txt}
 *   ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐
 *   │ Hardware │ -> │ CPU/PIC  │ -> │ IDT Gate │ -> │ Handler  │
 *   │  Event   │    │  Signal  │    │  Lookup  │    │  Called  │
 *   └──────────┘    └──────────┘    └──────────┘    └──────────┘
 *         │                                               │
 *         ▼                                               ▼
 *   ┌──────────┐                                   ┌──────────┐
 *   │   EOI    │ <-------------------------------- │  Handle  │
 *   │   Sent   │                                   │  Event   │
 *   └──────────┘                                   └──────────┘
 * @endcode
 *
 * @par X.2b USAGE — Registration Example
 * @code{.c}
 * void my_keyboard_handler(MosInterruptFrame* frame) {
 *     uint8_t scancode = inb(0x60);
 *     // process scancode
 *     mos_irq_eoi(1);  // acknowledge keyboard IRQ
 * }
 *
 * // In initialization:
 * mos_interrupt_register(MOS_IRQ_KEYBOARD, my_keyboard_handler,
 *                        MOS_INT_TYPE_IRQ, "keyboard");
 * mos_irq_enable(1);
 * @endcode
 *
 * @par X.2c CRITICAL — Critical Section Pattern
 * @code{.c}
 * uint64_t flags = mos_irq_save();
 * // critical section - interrupts disabled
 * // modify shared data structures
 * mos_irq_restore(flags);
 * @endcode
 *
 * @} */

/**
 * =============================================================================
 * X.3 CLEANUP — Ownership and Memory [CLEANUP]
 * =============================================================================
 *
 * @defgroup interrupt_cleanup X.3 CLEANUP — Ownership and Memory
 * @brief    Ownership patterns and memory management.
 *
 * =============================================================================
 * @{
 *
 * @par X.3a OWNERSHIP — Ownership Conventions
 *   - Handler name strings: Caller retains ownership (must be static/literal)
 *   - MosInterruptFrame*: Owned by interrupt system, valid only during handler
 *   - IDT entries: Owned by interrupt subsystem
 *
 * @par X.3b MEMORY — Memory Patterns
 *   - IDT is statically allocated (no heap)
 *   - Handler table is statically allocated
 *   - No dynamic memory in interrupt paths (stack only)
 *
 * @} */

/**
 * =============================================================================
 * X.4 POLICY — Modification Guidance [POLICY]
 * =============================================================================
 *
 * @defgroup interrupt_policy X.4 POLICY — Modification Guidance
 * @brief    Guide future maintainers — what's safe to change.
 *
 * SAFE TO MODIFY:
 *   - Add new handler registrations
 *   - Add new IRQ support
 *   - Extend statistics tracking
 *
 * MODIFY WITH CARE:
 *   - MosInterruptFrame layout — must match ASM stubs
 *   - IDT entry format — must match x86-64 spec
 *   - PIC remap base — breaks all IRQ handlers
 *
 * NEVER MODIFY:
 *   - Exception vector assignments (CPU-defined)
 *   - IDT size (256 entries for x86-64)
 *   - Packed struct attributes
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * =============================================================================
 *
 * @defgroup interrupt_extension X.5 EXTENSION — How to Expand
 * @brief    How to expand this component — mirrors M.10 Roadmap.
 *
 * ADDING NEW IRQ HANDLER:
 *   1. Define handler function matching MosInterruptHandler signature
 *   2. Call mos_interrupt_register() with appropriate vector
 *   3. Call mos_irq_enable() to unmask the IRQ
 *   4. Call mos_irq_eoi() at end of handler
 *
 * ADDING APIC SUPPORT:
 *   1. Detect APIC via CPUID
 *   2. Add mos_apic_init() alongside mos_pic_init()
 *   3. Remap vectors for APIC (different base)
 *   4. Update mos_irq_eoi() for APIC
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
 * @defgroup interrupt_troubleshoot X.6 TROUBLESHOOTING — Common Problems
 * @brief    Common problems and solutions.
 *
 * "Triple fault on interrupt":
 *   - Check IDT entry is properly set (present bit, correct selector)
 *   - Verify handler address is valid
 *   - Check stack pointer is valid
 *
 * "IRQ not firing":
 *   - Check PIC mask (call mos_irq_enable)
 *   - Verify interrupts enabled (call mos_sti)
 *   - Check hardware is configured correctly
 *
 * "Spurious IRQ7/IRQ15":
 *   - Normal PIC behavior, check for real IRQ first
 *   - Don't send EOI for spurious IRQs
 *
 * "GPF in handler":
 *   - Check handler is accessing valid memory
 *   - Verify stack frame layout matches expectations
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * =============================================================================
 *
 * @defgroup interrupt_reference X.7 REFERENCE — Quick Lookup
 * @brief    Copy-paste ready examples and quick reference.
 *
 * @par Types
 *   - MosInterruptType    — Ternary classification (-1/0/+1)
 *   - MosInterruptFrame   — CPU context (176 bytes)
 *   - MosInterruptHandler — void (*)(MosInterruptFrame*)
 *   - MosIdtEntry         — IDT gate (16 bytes)
 *   - MosIdtPointer       — IDTR value (10 bytes)
 *
 * @par Functions
 *   Lifecycle:  mos_interrupt_init(), mos_pic_init()
 *   Accessors:  mos_interrupt_count(), mos_interrupt_vector_count(),
 *               mos_irq_enabled()
 *   Register:   mos_interrupt_register(), mos_interrupt_unregister()
 *   Control:    mos_sti(), mos_cli(), mos_irq_save(), mos_irq_restore()
 *   IRQ:        mos_irq_enable(), mos_irq_disable(), mos_irq_eoi()
 *
 * @par Quick Copy-Paste
 * @code{.c}
 * // Register keyboard handler
 * mos_interrupt_register(MOS_IRQ_KEYBOARD, keyboard_handler,
 *                        MOS_INT_TYPE_IRQ, "keyboard");
 * mos_irq_enable(1);
 *
 * // Critical section
 * uint64_t flags = mos_irq_save();
 * // ... protected code ...
 * mos_irq_restore(flags);
 * @endcode
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.8 NOTE — Closing Guidance [NOTE]
 * =============================================================================
 *
 * @defgroup interrupt_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding — echoes M.8 Scripture.
 *
 * This header declares the interrupt subsystem interface.
 * Modify thoughtfully — changes affect all interrupt handlers.
 *
 * @par Scripture
 * "And God saw the light, that it was good:
 *  and God divided the light from the darkness."
 *   — Genesis 1:4
 *
 * Interrupts divide the darkness of busy-waiting from the light of
 * event-driven execution. The ternary classification reflects this:
 * exceptions (-1) disrupt, syscalls (0) mediate, IRQs (+1) enable.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.9 OMISSION — What Can Be Reserved [OMISSION]
 * =============================================================================
 *
 * @defgroup interrupt_closing_omission X.9 OMISSION — What Can Be Reserved
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

#endif /* MOS_KERNEL_CORE_INTERRUPT_H */
