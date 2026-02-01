; =============================================================================
; METADATA [METADATA]
; =============================================================================
;
; Key:         MOS-kernel-interrupt-asm
; Title:       MillenniumOS Interrupt Assembly Handlers
; Type:        Assembly (NASM)
; Component:   Stage 1 (Kernel) / Interrupt Subsystem
; Role:        Low-level interrupt entry points and context switching
;
; Status:      Active
; Version:     a-01.00
; Created:     2026-01-30
; Updated:     2026-01-30
;
; Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
; Organization: CreativeWorkzStudio LLC
;
; Grounding:   Genesis 1:4 — "And God divided the light from the darkness"
;              Interrupts divide execution flow.
;
; =============================================================================
; END METADATA
; =============================================================================

; =============================================================================
; SETUP [SETUP]
; =============================================================================

bits 64
section .text

; External C handler
extern mos_interrupt_handler

; Export all ISR and IRQ entry points
global isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7
global isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15
global isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
global isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31

global irq0,  irq1,  irq2,  irq3,  irq4,  irq5,  irq6,  irq7
global irq8,  irq9,  irq10, irq11, irq12, irq13, irq14, irq15

; =============================================================================
; END SETUP
; =============================================================================

; =============================================================================
; BODY [BODY]
; =============================================================================

; -----------------------------------------------------------------------------
; B.1 ISR Macros (Exception Handlers)
; -----------------------------------------------------------------------------
;
; Two types of exceptions:
;   - Without error code: We push dummy 0
;   - With error code: CPU already pushed it
;

; ISR without error code
%macro ISR_NOERR 1
isr%1:
    push qword 0            ; Dummy error code
    push qword %1           ; Interrupt number
    jmp isr_common
%endmacro

; ISR with error code (CPU pushes it)
%macro ISR_ERR 1
isr%1:
    push qword %1           ; Interrupt number (error code already on stack)
    jmp isr_common
%endmacro

; IRQ handler
%macro IRQ 2
irq%1:
    push qword 0            ; Dummy error code
    push qword %2           ; Interrupt number (32 + IRQ number)
    jmp isr_common
%endmacro

; -----------------------------------------------------------------------------
; B.2 Exception Entry Points (ISR 0-31)
; -----------------------------------------------------------------------------

; Exceptions without error code
ISR_NOERR 0     ; #DE Divide Error
ISR_NOERR 1     ; #DB Debug
ISR_NOERR 2     ; NMI
ISR_NOERR 3     ; #BP Breakpoint
ISR_NOERR 4     ; #OF Overflow
ISR_NOERR 5     ; #BR Bound Range
ISR_NOERR 6     ; #UD Invalid Opcode
ISR_NOERR 7     ; #NM Device Not Available

; Exception with error code
ISR_ERR 8       ; #DF Double Fault

; Exception without error code
ISR_NOERR 9     ; Coprocessor Segment Overrun (reserved)

; Exceptions with error code
ISR_ERR 10      ; #TS Invalid TSS
ISR_ERR 11      ; #NP Segment Not Present
ISR_ERR 12      ; #SS Stack Fault
ISR_ERR 13      ; #GP General Protection
ISR_ERR 14      ; #PF Page Fault

; Exception without error code
ISR_NOERR 15    ; Reserved

ISR_NOERR 16    ; #MF x87 FPU Error

; Exception with error code
ISR_ERR 17      ; #AC Alignment Check

; Exceptions without error code
ISR_NOERR 18    ; #MC Machine Check
ISR_NOERR 19    ; #XM/#XF SIMD FPU
ISR_NOERR 20    ; #VE Virtualization

; Exception with error code
ISR_ERR 21      ; #CP Control Protection

; Reserved exceptions
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

; -----------------------------------------------------------------------------
; B.3 IRQ Entry Points (IRQ 0-15 → INT 32-47)
; -----------------------------------------------------------------------------

IRQ 0,  32      ; Timer
IRQ 1,  33      ; Keyboard
IRQ 2,  34      ; Cascade
IRQ 3,  35      ; COM2
IRQ 4,  36      ; COM1
IRQ 5,  37      ; LPT2
IRQ 6,  38      ; Floppy
IRQ 7,  39      ; LPT1 / Spurious
IRQ 8,  40      ; RTC
IRQ 9,  41      ; Free
IRQ 10, 42      ; Free
IRQ 11, 43      ; Free
IRQ 12, 44      ; Mouse
IRQ 13, 45      ; Coprocessor
IRQ 14, 46      ; ATA Primary
IRQ 15, 47      ; ATA Secondary

; -----------------------------------------------------------------------------
; B.4 Common Interrupt Handler
; -----------------------------------------------------------------------------
;
; Saves all registers, calls C handler, restores and returns.
;
; Stack layout on entry (after our pushes):
;   [rsp+0]   = interrupt number
;   [rsp+8]   = error code (or 0)
;   [rsp+16]  = rip (pushed by CPU)
;   [rsp+24]  = cs
;   [rsp+32]  = rflags
;   [rsp+40]  = rsp (if privilege change)
;   [rsp+48]  = ss  (if privilege change)
;
; We need to build the MosInterruptFrame structure for C.
;

isr_common:
    ; Save all general-purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Save segment registers (for debugging, usually not needed in 64-bit)
    ; mov ax, ds
    ; push rax

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax

    ; Call C handler with pointer to stack frame
    mov rdi, rsp                ; First argument: pointer to MosInterruptFrame
    call mos_interrupt_handler

    ; Restore segment registers
    ; pop rax
    ; mov ds, ax
    ; mov es, ax

    ; Restore general-purpose registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Remove error code and interrupt number from stack
    add rsp, 16

    ; Return from interrupt
    iretq

; =============================================================================
; END BODY
; =============================================================================

; =============================================================================
; CLOSING [CLOSING]
; =============================================================================
;
; Interrupt assembly provides:
;   - 32 exception entry points (ISR 0-31)
;   - 16 IRQ entry points (IRQ 0-15 → INT 32-47)
;   - Common handler that saves/restores full context
;   - Proper stack frame for C handler
;
; Context saved in MosInterruptFrame order:
;   r15, r14, r13, r12, r11, r10, r9, r8
;   rbp, rdi, rsi, rdx, rcx, rbx, rax
;   int_no, error_code
;   rip, cs, rflags, rsp, ss (CPU pushed)
;
; =============================================================================
; END CLOSING
; =============================================================================
