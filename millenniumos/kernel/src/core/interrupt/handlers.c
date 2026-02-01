// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// Key:         MOS-kernel-interrupt-impl
// Title:       MillenniumOS Interrupt Handling Implementation
// Type:        Source
// Component:   Stage 1 (Kernel) / Interrupt Subsystem
// Role:        IDT setup, interrupt handlers, and exception management
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
#include "core/interrupt/interrupt.h"       // Interrupt subsystem
#include "core/process/process.h"           // Process subsystem (for timer→scheduler)
#include "core/timer/timer.h"               // Timer subsystem
#include "trit/matter/matter.h"             // libtrit types

// I/O port access
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// -----------------------------------------------------------------------------
// B.1 Global State
// -----------------------------------------------------------------------------

// IDT table (256 entries)
static MosIdtEntry g_idt[MOS_IDT_ENTRIES] __attribute__((aligned(16)));
static MosIdtPointer g_idt_ptr;

// Handler descriptors
static MosInterruptDescriptor g_handlers[MOS_IDT_ENTRIES];

// Interrupt statistics
static uint64_t g_int_count[MOS_IDT_ENTRIES];
static trit27_t g_total_interrupts = 0;

// Spurious interrupt tracking (M.10 Phase 1)
static uint64_t g_spurious_irq7 = 0;
static uint64_t g_spurious_irq15 = 0;

// Latency measurement (M.10 Phase 1)
static uint64_t g_last_int_tsc = 0;           // TSC at last interrupt
static uint64_t g_total_latency_tsc = 0;      // Sum of handler latencies
static uint64_t g_max_latency_tsc = 0;        // Worst-case latency
static uint64_t g_min_latency_tsc = ~0ULL;    // Best-case latency

// Timer tick tracking moved to timer.c - use mos_timer_tick() and mos_timer_ticks()

// Read timestamp counter
static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

// PIC ports
#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1

// -----------------------------------------------------------------------------
// B.2 IDT Entry Setup
// -----------------------------------------------------------------------------

static void idt_set_gate(uint8_t num, uint64_t handler, uint16_t selector,
                          uint8_t ist, uint8_t type_attr) {
    g_idt[num].offset_low  = handler & 0xFFFF;
    g_idt[num].selector    = selector;
    g_idt[num].ist         = ist;
    g_idt[num].type_attr   = type_attr;
    g_idt[num].offset_mid  = (handler >> 16) & 0xFFFF;
    g_idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    g_idt[num].reserved    = 0;
}

// -----------------------------------------------------------------------------
// B.3 Assembly Stubs (External)
// -----------------------------------------------------------------------------
//
// These are defined in interrupt_stubs.asm
// Each stub pushes interrupt number and calls common handler
//

extern void isr0(void);   extern void isr1(void);   extern void isr2(void);
extern void isr3(void);   extern void isr4(void);   extern void isr5(void);
extern void isr6(void);   extern void isr7(void);   extern void isr8(void);
extern void isr9(void);   extern void isr10(void);  extern void isr11(void);
extern void isr12(void);  extern void isr13(void);  extern void isr14(void);
extern void isr15(void);  extern void isr16(void);  extern void isr17(void);
extern void isr18(void);  extern void isr19(void);  extern void isr20(void);
extern void isr21(void);  extern void isr22(void);  extern void isr23(void);
extern void isr24(void);  extern void isr25(void);  extern void isr26(void);
extern void isr27(void);  extern void isr28(void);  extern void isr29(void);
extern void isr30(void);  extern void isr31(void);

extern void irq0(void);   extern void irq1(void);   extern void irq2(void);
extern void irq3(void);   extern void irq4(void);   extern void irq5(void);
extern void irq6(void);   extern void irq7(void);   extern void irq8(void);
extern void irq9(void);   extern void irq10(void);  extern void irq11(void);
extern void irq12(void);  extern void irq13(void);  extern void irq14(void);
extern void irq15(void);

// -----------------------------------------------------------------------------
// B.4 Default Handlers
// -----------------------------------------------------------------------------

static void default_exception_handler(MosInterruptFrame* frame) {
    // Default: log and halt for unhandled exceptions
    (void)frame;
    // In real implementation: log to console, dump registers
    __asm__ volatile("cli; hlt");
}

/**
 * @brief Check if IRQ 7 is spurious
 *
 * A spurious IRQ 7 occurs when the PIC signals an interrupt but the ISR
 * bit is not set. We check the ISR before sending EOI.
 *
 * @return true if spurious (don't send EOI), false if real
 */
static bool is_spurious_irq7(void) {
    // Read PIC1 ISR (OCW3: read ISR)
    outb(PIC1_CMD, 0x0B);
    uint8_t isr = inb(PIC1_CMD);
    return (isr & 0x80) == 0;  // Bit 7 = IRQ7
}

/**
 * @brief Check if IRQ 15 is spurious
 *
 * Similar to IRQ 7, but for the slave PIC. Note: We still need to send
 * EOI to master because the cascade interrupt (IRQ 2) was real.
 *
 * @return true if spurious, false if real
 */
static bool is_spurious_irq15(void) {
    // Read PIC2 ISR
    outb(PIC2_CMD, 0x0B);
    uint8_t isr = inb(PIC2_CMD);
    return (isr & 0x80) == 0;  // Bit 7 = IRQ7 of slave = IRQ15
}

static void default_irq_handler(MosInterruptFrame* frame) {
    uint8_t irq = frame->int_no - MOS_IRQ_BASE;

    // Spurious interrupt detection (M.10 Phase 1)
    if (irq == 7) {
        if (is_spurious_irq7()) {
            g_spurious_irq7++;
            return;  // Don't send EOI for spurious
        }
    } else if (irq == 15) {
        if (is_spurious_irq15()) {
            g_spurious_irq15++;
            outb(PIC1_CMD, 0x20);  // EOI to master (cascade was real)
            return;
        }
    }

    // Acknowledge real interrupt
    mos_irq_eoi(irq);
}

static void page_fault_handler(MosInterruptFrame* frame) {
    // Get faulting address from CR2
    uint64_t fault_addr;
    __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));

    // For now, just halt
    // Real implementation would:
    // 1. Check if valid page fault (demand paging)
    // 2. Allocate page if valid
    // 3. Kill process if invalid
    (void)fault_addr;
    (void)frame;

    __asm__ volatile("cli; hlt");
}

static void timer_handler(MosInterruptFrame* frame) {
    (void)frame;

    // Update timer subsystem (tick counting, drift detection)
    mos_timer_tick();

    // Acknowledge first (before potentially long scheduler work)
    mos_irq_eoi(0);

    // Scheduler integration (M.10 Phase 1)
    // Check if current process has exhausted its time slice
    MosProcess* current = mos_process_current();
    if (current && current->time_slice > 0) {
        current->time_slice--;
        current->total_time++;

        // Time slice exhausted - preempt
        if (current->time_slice == 0) {
            mos_schedule();
        }
    }
}

static void keyboard_handler(MosInterruptFrame* frame) {
    // Read scancode
    uint8_t scancode = inb(0x60);
    (void)scancode;
    (void)frame;

    // Acknowledge
    mos_irq_eoi(1);

    // In real implementation: decode and buffer keypress
}

// -----------------------------------------------------------------------------
// B.5 PIC Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_pic_init(void) {
    // Save masks
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // ICW1: Initialize + ICW4 needed
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);

    // ICW2: Vector offset (remap IRQs to 32-47)
    outb(PIC1_DATA, MOS_IRQ_BASE);
    outb(PIC2_DATA, MOS_IRQ_BASE + 8);

    // ICW3: Cascade configuration
    outb(PIC1_DATA, 0x04);  // PIC2 at IRQ2
    outb(PIC2_DATA, 0x02);  // Cascade identity

    // ICW4: 8086 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Restore masks (or set default)
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.6 Interrupt Initialization
// -----------------------------------------------------------------------------

mos_result_t mos_interrupt_init(void) {
    // Clear handler table
    for (int i = 0; i < MOS_IDT_ENTRIES; i++) {
        g_handlers[i].handler = NULL;
        g_handlers[i].type = TRIT_ZERO;
        g_handlers[i].flags = MOS_SPARE_INVALID;
        g_handlers[i].name = NULL;
        g_int_count[i] = 0;
    }

    // Set up IDT entries for exceptions (ISRs 0-31)
    // Type: 0x8E = Present, DPL=0, 64-bit interrupt gate
    idt_set_gate(0,  (uint64_t)isr0,  0x08, 0, 0x8E);
    idt_set_gate(1,  (uint64_t)isr1,  0x08, 0, 0x8E);
    idt_set_gate(2,  (uint64_t)isr2,  0x08, 0, 0x8E);
    idt_set_gate(3,  (uint64_t)isr3,  0x08, 0, 0x8E);
    idt_set_gate(4,  (uint64_t)isr4,  0x08, 0, 0x8E);
    idt_set_gate(5,  (uint64_t)isr5,  0x08, 0, 0x8E);
    idt_set_gate(6,  (uint64_t)isr6,  0x08, 0, 0x8E);
    idt_set_gate(7,  (uint64_t)isr7,  0x08, 0, 0x8E);
    idt_set_gate(8,  (uint64_t)isr8,  0x08, 1, 0x8E);  // Double fault uses IST1
    idt_set_gate(9,  (uint64_t)isr9,  0x08, 0, 0x8E);
    idt_set_gate(10, (uint64_t)isr10, 0x08, 0, 0x8E);
    idt_set_gate(11, (uint64_t)isr11, 0x08, 0, 0x8E);
    idt_set_gate(12, (uint64_t)isr12, 0x08, 0, 0x8E);
    idt_set_gate(13, (uint64_t)isr13, 0x08, 0, 0x8E);
    idt_set_gate(14, (uint64_t)isr14, 0x08, 0, 0x8E);  // Page fault
    idt_set_gate(15, (uint64_t)isr15, 0x08, 0, 0x8E);
    idt_set_gate(16, (uint64_t)isr16, 0x08, 0, 0x8E);
    idt_set_gate(17, (uint64_t)isr17, 0x08, 0, 0x8E);
    idt_set_gate(18, (uint64_t)isr18, 0x08, 0, 0x8E);
    idt_set_gate(19, (uint64_t)isr19, 0x08, 0, 0x8E);
    idt_set_gate(20, (uint64_t)isr20, 0x08, 0, 0x8E);
    idt_set_gate(21, (uint64_t)isr21, 0x08, 0, 0x8E);
    idt_set_gate(22, (uint64_t)isr22, 0x08, 0, 0x8E);
    idt_set_gate(23, (uint64_t)isr23, 0x08, 0, 0x8E);
    idt_set_gate(24, (uint64_t)isr24, 0x08, 0, 0x8E);
    idt_set_gate(25, (uint64_t)isr25, 0x08, 0, 0x8E);
    idt_set_gate(26, (uint64_t)isr26, 0x08, 0, 0x8E);
    idt_set_gate(27, (uint64_t)isr27, 0x08, 0, 0x8E);
    idt_set_gate(28, (uint64_t)isr28, 0x08, 0, 0x8E);
    idt_set_gate(29, (uint64_t)isr29, 0x08, 0, 0x8E);
    idt_set_gate(30, (uint64_t)isr30, 0x08, 0, 0x8E);
    idt_set_gate(31, (uint64_t)isr31, 0x08, 0, 0x8E);

    // Set up IDT entries for IRQs (32-47)
    idt_set_gate(32, (uint64_t)irq0,  0x08, 0, 0x8E);
    idt_set_gate(33, (uint64_t)irq1,  0x08, 0, 0x8E);
    idt_set_gate(34, (uint64_t)irq2,  0x08, 0, 0x8E);
    idt_set_gate(35, (uint64_t)irq3,  0x08, 0, 0x8E);
    idt_set_gate(36, (uint64_t)irq4,  0x08, 0, 0x8E);
    idt_set_gate(37, (uint64_t)irq5,  0x08, 0, 0x8E);
    idt_set_gate(38, (uint64_t)irq6,  0x08, 0, 0x8E);
    idt_set_gate(39, (uint64_t)irq7,  0x08, 0, 0x8E);
    idt_set_gate(40, (uint64_t)irq8,  0x08, 0, 0x8E);
    idt_set_gate(41, (uint64_t)irq9,  0x08, 0, 0x8E);
    idt_set_gate(42, (uint64_t)irq10, 0x08, 0, 0x8E);
    idt_set_gate(43, (uint64_t)irq11, 0x08, 0, 0x8E);
    idt_set_gate(44, (uint64_t)irq12, 0x08, 0, 0x8E);
    idt_set_gate(45, (uint64_t)irq13, 0x08, 0, 0x8E);
    idt_set_gate(46, (uint64_t)irq14, 0x08, 0, 0x8E);
    idt_set_gate(47, (uint64_t)irq15, 0x08, 0, 0x8E);

    // Register default handlers
    for (int i = 0; i < 32; i++) {
        g_handlers[i].handler = default_exception_handler;
        g_handlers[i].type = TRIT_NEG;  // Exception
        g_handlers[i].flags = 0;
        g_handlers[i].name = "default_exception";
    }

    // Register specific exception handlers
    g_handlers[14].handler = page_fault_handler;
    g_handlers[14].name = "page_fault";

    // Register IRQ handlers
    for (int i = 32; i < 48; i++) {
        g_handlers[i].handler = default_irq_handler;
        g_handlers[i].type = TRIT_POS;  // IRQ
        g_handlers[i].flags = 0;
        g_handlers[i].name = "default_irq";
    }

    // Register specific IRQ handlers
    g_handlers[32].handler = timer_handler;
    g_handlers[32].name = "timer";
    g_handlers[33].handler = keyboard_handler;
    g_handlers[33].name = "keyboard";

    // Initialize PIC
    mos_pic_init();

    // Load IDT
    g_idt_ptr.limit = sizeof(g_idt) - 1;
    g_idt_ptr.base = (uint64_t)&g_idt;
    __asm__ volatile("lidt %0" : : "m"(g_idt_ptr));

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.7 Common Interrupt Handler (called from assembly stubs)
// -----------------------------------------------------------------------------

void mos_interrupt_handler(MosInterruptFrame* frame) {
    uint64_t entry_tsc = rdtsc();  // Latency measurement start
    uint8_t int_no = frame->int_no;

    // Update statistics
    g_int_count[int_no]++;
    g_total_interrupts++;

    // Call registered handler
    if (g_handlers[int_no].handler != NULL) {
        g_handlers[int_no].handler(frame);
    }

    // Latency measurement end (M.10 Phase 1)
    uint64_t exit_tsc = rdtsc();
    uint64_t latency = exit_tsc - entry_tsc;
    g_total_latency_tsc += latency;
    if (latency > g_max_latency_tsc) {
        g_max_latency_tsc = latency;
    }
    if (latency < g_min_latency_tsc) {
        g_min_latency_tsc = latency;
    }
    g_last_int_tsc = exit_tsc;
}

// -----------------------------------------------------------------------------
// B.8 Handler Registration
// -----------------------------------------------------------------------------

mos_result_t mos_interrupt_register(uint8_t vector, MosInterruptHandler handler,
                                     trit_t type, const char* name) {
    // No bounds check needed: uint8_t (0-255) fits MOS_IDT_ENTRIES (256)

    g_handlers[vector].handler = handler;
    g_handlers[vector].type = type;
    g_handlers[vector].flags = 0;
    g_handlers[vector].name = name;

    return MOS_OK;
}

mos_result_t mos_interrupt_unregister(uint8_t vector) {
    // No bounds check needed: uint8_t (0-255) fits MOS_IDT_ENTRIES (256)

    // Restore default
    if (vector < 32) {
        g_handlers[vector].handler = default_exception_handler;
        g_handlers[vector].type = TRIT_NEG;
    } else if (vector < 48) {
        g_handlers[vector].handler = default_irq_handler;
        g_handlers[vector].type = TRIT_POS;
    } else {
        g_handlers[vector].handler = NULL;
        g_handlers[vector].type = TRIT_ZERO;
    }
    g_handlers[vector].flags = MOS_SPARE_INVALID;
    g_handlers[vector].name = NULL;

    return MOS_OK;
}

// -----------------------------------------------------------------------------
// B.9 IRQ Control
// -----------------------------------------------------------------------------

void mos_irq_enable(uint8_t irq) {
    uint16_t port;
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    uint8_t mask = inb(port) & ~(1 << irq);
    outb(port, mask);
}

void mos_irq_disable(uint8_t irq) {
    uint16_t port;
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    uint8_t mask = inb(port) | (1 << irq);
    outb(port, mask);
}

void mos_irq_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, 0x20);  // EOI to slave
    }
    outb(PIC1_CMD, 0x20);      // EOI to master
}

// -----------------------------------------------------------------------------
// B.10 Statistics
// -----------------------------------------------------------------------------

trit27_t mos_interrupt_count(void) {
    return g_total_interrupts;
}

uint64_t mos_interrupt_vector_count(uint8_t vector) {
    // No bounds check needed: uint8_t (0-255) fits MOS_IDT_ENTRIES (256)
    return g_int_count[vector];
}

// -----------------------------------------------------------------------------
// B.11 Spurious Interrupt Statistics (M.10 Phase 1)
// -----------------------------------------------------------------------------

uint64_t mos_spurious_irq7_count(void) {
    return g_spurious_irq7;
}

uint64_t mos_spurious_irq15_count(void) {
    return g_spurious_irq15;
}

uint64_t mos_spurious_total(void) {
    return g_spurious_irq7 + g_spurious_irq15;
}

// -----------------------------------------------------------------------------
// B.12 Latency Statistics (M.10 Phase 1)
// -----------------------------------------------------------------------------

uint64_t mos_interrupt_latency_avg(void) {
    if (g_total_interrupts == 0) return 0;
    return g_total_latency_tsc / g_total_interrupts;
}

uint64_t mos_interrupt_latency_max(void) {
    return g_max_latency_tsc;
}

uint64_t mos_interrupt_latency_min(void) {
    if (g_min_latency_tsc == ~0ULL) return 0;
    return g_min_latency_tsc;
}

// -----------------------------------------------------------------------------
// B.13 Timer Integration
// -----------------------------------------------------------------------------
// Timer tick access moved to core/timer/timer.c
// Use: mos_timer_ticks() from timer.h

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================
//
// Interrupt handling implementation:
//   - IDT with 256 entries
//   - PIC remapped to vectors 32-47
//   - Exception handlers (0-31)
//   - IRQ handlers (32-47)
//   - Statistics tracking with ternary counts
//
// =============================================================================
// END CLOSING
// =============================================================================
