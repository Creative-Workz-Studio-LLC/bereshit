; =============================================================================
; METADATA [METADATA]
; =============================================================================
;
; Key:         MOS-kernel-context-switch
; Title:       MillenniumOS Context Switch Assembly
; Type:        Assembly (NASM)
; Component:   Stage 1 (Kernel) / Process Subsystem
; Role:        Low-level register save/restore for process switching
;
; Status:      Active
; Version:     a-01.00
; Created:     2026-01-30
; Updated:     2026-01-30
;
; Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
; Organization: CreativeWorkzStudio LLC
;
; Grounding:   Genesis 1:14 — "Let there be lights...to divide the day from the night"
;              Context switches divide execution between processes.
;
; =============================================================================
; END METADATA
; =============================================================================

; =============================================================================
; SETUP [SETUP]
; =============================================================================

bits 64
section .text

; Export symbols for C
global mos_context_switch
global mos_context_save
global mos_context_restore
global mos_switch_to_user

; =============================================================================
; END SETUP
; =============================================================================

; =============================================================================
; BODY [BODY]
; =============================================================================

; -----------------------------------------------------------------------------
; B.1 MosCpuContext Structure Offsets
; -----------------------------------------------------------------------------
;
; Must match MosCpuContext in process.h:
;   typedef struct {
;       uint64_t rax, rbx, rcx, rdx;       // 0, 8, 16, 24
;       uint64_t rsi, rdi, rbp, rsp;       // 32, 40, 48, 56
;       uint64_t r8, r9, r10, r11;         // 64, 72, 80, 88
;       uint64_t r12, r13, r14, r15;       // 96, 104, 112, 120
;       uint64_t rip;                      // 128
;       uint64_t rflags;                   // 136
;       uint64_t cs, ss, ds, es, fs, gs;   // 144, 152, 160, 168, 176, 184
;       uint64_t cr3;                      // 192
;       void* fpu_state;                   // 200
;   } MosCpuContext;
;

%define CTX_RAX     0
%define CTX_RBX     8
%define CTX_RCX     16
%define CTX_RDX     24
%define CTX_RSI     32
%define CTX_RDI     40
%define CTX_RBP     48
%define CTX_RSP     56
%define CTX_R8      64
%define CTX_R9      72
%define CTX_R10     80
%define CTX_R11     88
%define CTX_R12     96
%define CTX_R13     104
%define CTX_R14     112
%define CTX_R15     120
%define CTX_RIP     128
%define CTX_RFLAGS  136
%define CTX_CS      144
%define CTX_SS      152
%define CTX_DS      160
%define CTX_ES      168
%define CTX_FS      176
%define CTX_GS      184
%define CTX_CR3     192
%define CTX_FPU     200

; -----------------------------------------------------------------------------
; B.2 mos_context_save - Save current CPU context
; -----------------------------------------------------------------------------
;
; void mos_context_save(MosCpuContext* ctx);
;
; Saves all general-purpose registers, flags, and segment registers
; to the provided context structure.
;
; Arguments:
;   rdi = pointer to MosCpuContext
;

mos_context_save:
    ; Save general-purpose registers
    mov [rdi + CTX_RAX], rax
    mov [rdi + CTX_RBX], rbx
    mov [rdi + CTX_RCX], rcx
    mov [rdi + CTX_RDX], rdx
    mov [rdi + CTX_RSI], rsi
    ; Note: rdi contains ctx pointer, save it from stack later
    mov [rdi + CTX_RBP], rbp

    ; Save r8-r15
    mov [rdi + CTX_R8], r8
    mov [rdi + CTX_R9], r9
    mov [rdi + CTX_R10], r10
    mov [rdi + CTX_R11], r11
    mov [rdi + CTX_R12], r12
    mov [rdi + CTX_R13], r13
    mov [rdi + CTX_R14], r14
    mov [rdi + CTX_R15], r15

    ; Save rdi (it was the argument, but we need to save its original value)
    ; For now, save current rdi (the ctx pointer itself)
    mov [rdi + CTX_RDI], rdi

    ; Save stack pointer (caller's rsp is rsp + 8 because of return address)
    lea rax, [rsp + 8]
    mov [rdi + CTX_RSP], rax

    ; Save return address as rip
    mov rax, [rsp]
    mov [rdi + CTX_RIP], rax

    ; Save flags
    pushfq
    pop rax
    mov [rdi + CTX_RFLAGS], rax

    ; Save segment registers
    xor rax, rax
    mov ax, cs
    mov [rdi + CTX_CS], rax
    mov ax, ss
    mov [rdi + CTX_SS], rax
    mov ax, ds
    mov [rdi + CTX_DS], rax
    mov ax, es
    mov [rdi + CTX_ES], rax
    mov ax, fs
    mov [rdi + CTX_FS], rax
    mov ax, gs
    mov [rdi + CTX_GS], rax

    ; Save CR3
    mov rax, cr3
    mov [rdi + CTX_CR3], rax

    ret

; -----------------------------------------------------------------------------
; B.3 mos_context_restore - Restore CPU context
; -----------------------------------------------------------------------------
;
; void mos_context_restore(MosCpuContext* ctx);
;
; Restores all registers from context and jumps to saved rip.
; This function does not return - it jumps to the restored context.
;
; Arguments:
;   rdi = pointer to MosCpuContext
;

mos_context_restore:
    ; Load CR3 first (page table switch)
    mov rax, [rdi + CTX_CR3]
    mov cr3, rax

    ; Load segment registers
    mov ax, [rdi + CTX_DS]
    mov ds, ax
    mov ax, [rdi + CTX_ES]
    mov es, ax
    ; Note: CS and SS are loaded via iretq

    ; Restore r8-r15
    mov r8,  [rdi + CTX_R8]
    mov r9,  [rdi + CTX_R9]
    mov r10, [rdi + CTX_R10]
    mov r11, [rdi + CTX_R11]
    mov r12, [rdi + CTX_R12]
    mov r13, [rdi + CTX_R13]
    mov r14, [rdi + CTX_R14]
    mov r15, [rdi + CTX_R15]

    ; Restore general-purpose registers
    mov rbx, [rdi + CTX_RBX]
    mov rcx, [rdi + CTX_RCX]
    mov rdx, [rdi + CTX_RDX]
    mov rsi, [rdi + CTX_RSI]
    mov rbp, [rdi + CTX_RBP]

    ; Load new stack
    mov rsp, [rdi + CTX_RSP]

    ; Push iretq frame for return
    ; SS
    mov rax, [rdi + CTX_SS]
    push rax
    ; RSP (already on stack)
    mov rax, [rdi + CTX_RSP]
    push rax
    ; RFLAGS
    mov rax, [rdi + CTX_RFLAGS]
    push rax
    ; CS
    mov rax, [rdi + CTX_CS]
    push rax
    ; RIP
    mov rax, [rdi + CTX_RIP]
    push rax

    ; Restore rax and rdi last
    mov rax, [rdi + CTX_RAX]
    mov rdi, [rdi + CTX_RDI]

    ; Return to new context
    iretq

; -----------------------------------------------------------------------------
; B.4 mos_context_switch - Switch between two contexts
; -----------------------------------------------------------------------------
;
; void mos_context_switch(MosProcess* next);
;
; Saves current context and switches to next process.
; Called from scheduler when switching processes.
;
; Arguments:
;   rdi = pointer to next MosProcess (contains context at known offset)
;
; MosProcess structure offsets (from process.h):
;   context starts at offset 132 (after identity + state fields)
;   Actually need to calculate properly...
;
; For simplicity, we pass the context pointer directly from C.
;

; External C helper to get current process context
extern mos_process_current_context
extern mos_process_set_current

mos_context_switch:
    ; Save callee-saved registers per System V ABI
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; Save next process pointer
    push rdi

    ; Get current process context pointer
    call mos_process_current_context
    ; rax = pointer to current process's MosCpuContext

    ; If no current process (first switch), skip save
    test rax, rax
    jz .no_save

    ; Save current context
    mov rdi, rax

    ; Save general-purpose registers
    mov [rdi + CTX_RBX], rbx
    mov [rdi + CTX_RCX], rcx
    mov [rdi + CTX_RDX], rdx
    mov [rdi + CTX_RSI], rsi
    mov [rdi + CTX_RBP], rbp
    mov [rdi + CTX_R8], r8
    mov [rdi + CTX_R9], r9
    mov [rdi + CTX_R10], r10
    mov [rdi + CTX_R11], r11
    mov [rdi + CTX_R12], r12
    mov [rdi + CTX_R13], r13
    mov [rdi + CTX_R14], r14
    mov [rdi + CTX_R15], r15

    ; Save rsp (adjusted for our pushes: 6 callee-saved + 1 rdi = 56 bytes)
    lea rax, [rsp + 56]
    mov [rdi + CTX_RSP], rax

    ; Save return address as rip
    mov rax, [rsp + 56]     ; Return address above our pushes
    mov [rdi + CTX_RIP], rax

    ; Save flags
    pushfq
    pop rax
    mov [rdi + CTX_RFLAGS], rax

    ; Save CR3
    mov rax, cr3
    mov [rdi + CTX_CR3], rax

.no_save:
    ; Get next process pointer back
    pop rdi

    ; Set next as current process
    call mos_process_set_current

    ; Get next process context pointer
    mov rdi, rax            ; rax = next process from set_current
    call mos_process_current_context
    mov rdi, rax            ; rdi = next process context

    ; Load new CR3 if different
    mov rax, [rdi + CTX_CR3]
    mov rcx, cr3
    cmp rax, rcx
    je .same_cr3
    mov cr3, rax
.same_cr3:

    ; Restore general-purpose registers
    mov rbx, [rdi + CTX_RBX]
    mov rcx, [rdi + CTX_RCX]
    mov rdx, [rdi + CTX_RDX]
    mov rsi, [rdi + CTX_RSI]
    mov rbp, [rdi + CTX_RBP]
    mov r8,  [rdi + CTX_R8]
    mov r9,  [rdi + CTX_R9]
    mov r10, [rdi + CTX_R10]
    mov r11, [rdi + CTX_R11]
    mov r12, [rdi + CTX_R12]
    mov r13, [rdi + CTX_R13]
    mov r14, [rdi + CTX_R14]
    mov r15, [rdi + CTX_R15]

    ; Load new stack
    mov rsp, [rdi + CTX_RSP]

    ; Restore flags
    push qword [rdi + CTX_RFLAGS]
    popfq

    ; Load rax and jump to new rip
    mov rax, [rdi + CTX_RAX]
    mov rdi, [rdi + CTX_RDI]

    ; Pop callee-saved (on new stack) and return
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    ret

; -----------------------------------------------------------------------------
; B.5 mos_switch_to_user - Switch to user mode
; -----------------------------------------------------------------------------
;
; void mos_switch_to_user(uint64_t entry, uint64_t stack, uint64_t arg);
;
; Switches to ring 3 (user mode) and jumps to entry point.
; Used when starting a user process for the first time.
;
; Arguments:
;   rdi = entry point address
;   rsi = user stack pointer
;   rdx = argument to pass in rdi
;

; GDT segment selectors (must match kernel GDT setup)
%define KERNEL_CS   0x08
%define KERNEL_DS   0x10
%define USER_CS     0x1B    ; 0x18 | 3 (RPL=3)
%define USER_DS     0x23    ; 0x20 | 3 (RPL=3)

mos_switch_to_user:
    ; Save arguments
    mov rcx, rdi            ; entry point
    mov r8, rsi             ; user stack
    mov r9, rdx             ; argument

    ; Set up user data segments
    mov ax, USER_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Build iretq frame on current stack
    ; (kernel stack, since we're switching TO user mode)

    ; SS - user data segment
    push USER_DS

    ; RSP - user stack pointer
    push r8

    ; RFLAGS - enable interrupts (IF=1), clear others
    pushfq
    pop rax
    or rax, 0x200           ; Set IF (interrupt flag)
    and rax, ~0x100         ; Clear TF (trap flag)
    push rax

    ; CS - user code segment
    push USER_CS

    ; RIP - entry point
    push rcx

    ; Set argument in rdi (for user program)
    mov rdi, r9

    ; Clear other registers for clean start
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rbp, rbp
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

    ; Return to user mode
    iretq

; =============================================================================
; END BODY
; =============================================================================

; =============================================================================
; CLOSING [CLOSING]
; =============================================================================
;
; Context switch assembly provides:
;   - mos_context_save: Save all registers to MosCpuContext
;   - mos_context_restore: Restore registers and jump
;   - mos_context_switch: Full process switch with CR3 handling
;   - mos_switch_to_user: Ring 3 transition for new processes
;
; Ternary connection:
;   Context switching divides time between processes,
;   like the lights divide day from night (Genesis 1:14).
;   Each context represents a complete execution state.
;
; =============================================================================
; END CLOSING
; =============================================================================
