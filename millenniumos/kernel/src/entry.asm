; ═══════════════════════════════════════════════════════════════════════════════
; OMNICODE PRAGMA [PRAGMA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @legend
;   :req  REQUIRED   — must exist, validation fails without
;   :inh  INHERITED  — from template, override allowed
;   :ins  INSTANCE   — file-specific, unique values
; @endlegend
;
; ───────────────────────────────────────────────────────────────────────────────
; P.1 CORE — Identity [CORE]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:req  key        = MOS-kernel-entry
; @omni:req  from       = bereshit/word/seed/code/asm/source.asm
; @omni:req  at         = instance
;
; ───────────────────────────────────────────────────────────────────────────────
; P.2 FAMILY — Classification [FAMILY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:inh  type       = source
; @omni:inh  subtype    = kernel-entry
; @omni:inh  role       = foundation
; @omni:inh  structure  = 4-block
;
; ───────────────────────────────────────────────────────────────────────────────
; P.3 INSTANCE — File Details [INSTANCE]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  component  = kernel/entry
; @omni:ins  layer      = kernel
; @omni:ins  includes   = boot/include/bootinfo.inc
; @omni:ins  provides   = [_start, kernel_stack_top, BSS_CLEAR, BOOT_VALIDATE]
;
; ───────────────────────────────────────────────────────────────────────────────
; P.4 ARCHITECTURE — Scaling Context [ARCH]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  layers     = [0:kernel, 1:system, 2:runtime, 3:framework,
;                          4:health, 5:network, 6:storage, 7:engine, 8:app]
; @omni:ins  scale      = 3^n [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
; @omni:ins  pattern    = boot-handoff (stage0b/uefi → entry → kernel_main)
; @omni:ins  anchor     = genesis_1_3 → "And God said, Let there be light"
;
; ───────────────────────────────────────────────────────────────────────────────
; P.5 SUMMARY — Human Identity [SUMMARY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:req  title      = MillenniumOS Kernel Entry Point
; @omni:req  brief      = Assembly bridge from bootloader to C kernel — validates
;                         boot info, sets up stack, clears BSS, calls mos_kernel_main
;
; ═══════════════════════════════════════════════════════════════════════════════
; END PRAGMA [END]
; ═══════════════════════════════════════════════════════════════════════════════
;
; DEPENDENCY CLASSIFICATION: [DEPENDED]
;   - Needs: Bootloader (Stage 0b or UEFI) provides MosBootInfo at 0x9000
;   - Needs: Linker script defines __bss_start, __bss_end, _start
;   - Provides: Stack, cleared BSS, validated boot info for C kernel
;
; ═══════════════════════════════════════════════════════════════════════════════
; METADATA BLOCK [METADATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Identity and context for kernel entry point.
;
; The kernel entry is the bridge where assembly hands off to C.
; It receives control from the bootloader in 64-bit long mode and:
;   1. Disables interrupts (void state — no IDT yet)
;   2. Sets up kernel stack (word state — foundation for calls)
;   3. Clears BSS (preparation for C runtime)
;   4. Validates boot info (trust but verify)
;   5. Records entry TSC (boot timing forensics)
;   6. Calls mos_kernel_main() (tov state — "Let there be light")
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; M.1 IDENTITY — Core Identity [IDENTITY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @file      entry.asm
; @brief     Kernel Entry Point — bridges bootloader to C kernel.
;
; Key:       MOS-kernel-entry
; Title:     MillenniumOS Kernel Entry Point
; Type:      Source (Kernel Entry)
; Component: Ladder — top of boot chain, bottom of kernel
; Role:      Validate boot handoff, prepare C environment, call kernel

; ═══════════════════════════════════════════════════════════════════════════════
; M.2 STATE — Lifecycle State [STATE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @version   a-02.00
; @date      2026-01-31
;
; Status:    Active
; Created:   2026-01-30
; Updated:   2026-01-31

; ═══════════════════════════════════════════════════════════════════════════════
; M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @author    Seanje Lenox-Wise (Architect)
; @author    Nova Dawn (Implementation)
; @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.

; ═══════════════════════════════════════════════════════════════════════════════
; M.4 LOCATION — File Position [LOCATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Path:      millenniumos/kernel/src/entry.asm

; ═══════════════════════════════════════════════════════════════════════════════
; M.5 DERIVATION — Template Lineage [DERIVATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Derives:   bereshit/word/seed/code/asm/source.asm (template)

; ═══════════════════════════════════════════════════════════════════════════════
; M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Tags:      [asm, nasm, kernel, x86-64, long-mode, entry, boot-handoff]

; ═══════════════════════════════════════════════════════════════════════════════
; M.7 INTENT — Purpose Statement [INTENT]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Purpose:   Bridge the bootloader-to-kernel transition. This assembly code
;            receives control from Stage 0b (BIOS) or UEFI bootloader in 64-bit
;            long mode, validates the boot info handoff, prepares the C runtime
;            environment, and transfers control to mos_kernel_main().
;
; Core Design: Trust-but-verify handoff with forensic timestamps.
;              Genesis 1:3 principle: "And God said, Let there be light"
;              The kernel speaks (assembly calls C) and creation begins.
;
; Key Features:
;   - Boot info validation (magic, version, framebuffer)
;   - Stack setup (16KB kernel stack)
;   - BSS clearing (zero-initialize uninitialized data)
;   - Entry TSC recording (boot timing measurement)
;   - Serial debug output (works even if graphics fail)
;   - VGA debug markers (visible boot progress)
;   - Graceful halt on validation failure

; ═══════════════════════════════════════════════════════════════════════════════
; M.8 GROUNDING — Biblical Foundation [GROUNDING]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Scripture: Genesis 1:3 — "And God said, Let there be light: and there was light."
; Principle: The kernel entry is where the system "speaks" — assembly calls C
;            and the kernel comes to life. The bootloader prepared everything
;            (void to form), now the kernel can create (light, life, function).
;            This is the moment of transition from boot to runtime.

; ═══════════════════════════════════════════════════════════════════════════════
; M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; CPU Features:
;   - x86-64 Long Mode (64-bit) — bootloader already transitioned
;   - TSC (timestamp counter) for boot timing
;
; Include Files:
;   - boot/include/bootinfo.inc — MOS_BOOT_INFO_ADDR, MOS_BOOT_MAGIC, offsets
;
; External Symbols:
;   - mos_kernel_main (C entry point)
;   - __bss_start, __bss_end (linker-defined BSS bounds)
;
; Memory Layout:
;   - Boot info at MOS_BOOT_INFO_ADDR (0x9000)
;   - Kernel loaded at 0x100000 (1MB)
;   - Kernel stack at kernel_stack_top (16KB)
;
; Used by:
;   - Stage 0b (BIOS path) jumps here after long mode transition
;   - UEFI bootloader jumps here after loading kernel
;   - Kernel expects valid boot info and prepared environment
;
; Assemble:
;   nasm -f elf64 entry.asm -o entry.o -I boot/include/
;
; Link:
;   Linked with kernel.o into kernel.bin via linker script

; ═══════════════════════════════════════════════════════════════════════════════
; M.10 ROADMAP — Version History [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; History:
;   a-01.00 (2026-01-30) — Initial creation with basic boot validation
;   a-02.00 (2026-01-31) — Full OmniCode template alignment, enhanced docs
;
; Completed:
;   ✓ Boot info magic validation (prevents corrupted handoff)
;   ✓ Version compatibility check (future-proofs protocol)
;   ✓ Framebuffer address sanity check (1MB-4GB range)
;   ✓ Memory map count validation (0-256 entries)
;   ✓ Total memory non-zero verification
;   ✓ BSS section clearing (zero-initialized globals)
;   ✓ Kernel stack setup (RSP at kernel_stack_top)
;   ✓ TSC timestamp capture (entry_tsc for boot timing)
;   ✓ Serial port debug output on validation failure
;   ✓ Proper C ABI setup (RDI = boot_info pointer)
;
; Planned:
;   ⏳ Memory map validation (contiguous, non-overlapping)
;   ⏳ ACPI/RSDP pointer validation from boot info
;   ⏳ Boot source-specific validation (UEFI vs BIOS paths)
;   ⏳ DAR Witness pre-kernel event buffer import
;   ⏳ Early exception handler installation

; ═══════════════════════════════════════════════════════════════════════════════
; END METADATA [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; SETUP BLOCK [SETUP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Configuration and declarations before body.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; CONTAINS:
;   - S.1 DIRECTIVES — Assembler configuration (BITS, section)
;   - S.2 INCLUDES   — Include files (bootinfo.inc)
;   - S.3 EXTERNAL   — External symbol declarations
;   - S.4 EXPORTS    — Exported symbols (global)
;   - S.5 CONSTANTS  — EQU definitions
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; S.1 DIRECTIVES — Assembler Configuration [DIRECTIVES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief NASM directives for 64-bit kernel code.
;
; BITS 64: We're in long mode (bootloader already transitioned)
; section .text: Code section (executable)
;
; ═══════════════════════════════════════════════════════════════════════════════

bits 64
section .text

; ═══════════════════════════════════════════════════════════════════════════════
; S.2 INCLUDES — Include Files [INCLUDES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Boot information constants (SINGLE SOURCE OF TRUTH).
;
; bootinfo.inc mirrors bootinfo.h — provides MOS_BOOT_INFO_ADDR and offsets.
; This ensures kernel entry reads from the same address bootloader writes to.
;
; ═══════════════════════════════════════════════════════════════════════════════

%include "boot/include/bootinfo.inc"

; ═══════════════════════════════════════════════════════════════════════════════
; S.3 EXTERNAL — External Symbol Declarations [EXTERNAL]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Symbols defined elsewhere (C kernel, linker script).
;
; mos_kernel_main: C entry point in kernel.c
; __bss_start, __bss_end: Linker-defined BSS section bounds
;
; ═══════════════════════════════════════════════════════════════════════════════

extern mos_kernel_main
extern __bss_start
extern __bss_end

; ═══════════════════════════════════════════════════════════════════════════════
; S.4 EXPORTS — Exported Symbols [EXPORTS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Symbols this file provides to other modules.
;
; _start: Entry point (linker script uses this)
; kernel_stack_top: Stack pointer for debugging
;
; ═══════════════════════════════════════════════════════════════════════════════

global _start
global kernel_stack_top

; ═══════════════════════════════════════════════════════════════════════════════
; S.5 CONSTANTS — EQU Definitions [CONSTANTS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Compile-time constants for kernel entry.
;
; NOTE: MOS_BOOT_INFO_ADDR, MOS_BOOT_MAGIC, MOS_BOOT_VERSION, and all
;       BOOTINFO_OFF_* constants come from bootinfo.inc (single source of truth).
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.5a STACK — Kernel Stack Configuration [STACK]
; ───────────────────────────────────────────────────────────────────────────────

; Stack size: 16KB (practical alignment, ~27 × 512 bytes trit-inspired)
KERNEL_STACK_SIZE   equ 16384

; ───────────────────────────────────────────────────────────────────────────────
; S.5b TERNARY — Ternary State Values [TERNARY]
; ───────────────────────────────────────────────────────────────────────────────

; Ternary stage values (for state tracking)
TRIT_NEG            equ -1              ; Negative / MATTER / void
TRIT_ZERO           equ 0               ; Zero / TIME / word
TRIT_POS            equ 1               ; Positive / SPACE / tov

; ───────────────────────────────────────────────────────────────────────────────
; S.5c SERIAL — Debug Serial Port [SERIAL]
; ───────────────────────────────────────────────────────────────────────────────

; Serial port constants (COM1 for debug output)
COM1_PORT           equ 0x3F8
COM1_STATUS         equ 0x3FD

; ───────────────────────────────────────────────────────────────────────────────
; S.5d VALIDATION — Error Codes [VALIDATION]
; ───────────────────────────────────────────────────────────────────────────────

; Validation error codes (displayed in VGA and serial)
VALIDATE_OK         equ 0x00            ; Validation passed
VALIDATE_BAD_MAGIC  equ 0xE1            ; Magic number mismatch
VALIDATE_BAD_VERSION equ 0xE2           ; Version too low
VALIDATE_BAD_FB     equ 0xE3            ; Framebuffer address invalid

; ═══════════════════════════════════════════════════════════════════════════════
; END SETUP [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; BODY BLOCK [BODY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief The actual work — kernel entry and execution.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; CONTAINS:
;   - B.1 ENTRY      — _start entry point and boot phases
;   - B.2 DAR        — DAR Witness event logging functions
;   - B.3 SERIAL     — Serial debug output functions
;   - B.4 STRINGS    — Debug message strings (rodata)
;   - B.5 DATA       — Stack and BSS allocations
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; B.1 ENTRY — Kernel Entry Point [ENTRY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief _start — THE entry point. Called from bootloader in long mode.
;
; Entry State:
;   - CPU in 64-bit long mode
;   - Interrupts disabled (no IDT yet)
;   - RDI may contain boot info pointer (not used — we use fixed address)
;   - Boot info at MOS_BOOT_INFO_ADDR (0x9000)
;
; Boot Phases (ternary-inspired, with DAR Witness events):
;   Phase 1: Entry reached           → DAR event: MOS_EVT_ENTRY_START
;   Phase 2: Disable interrupts      (-1: void state)
;   Phase 3: Set up stack            → DAR event: MOS_EVT_ENTRY_STACK
;   Phase 4: Clear BSS               → DAR event: MOS_EVT_ENTRY_BSS
;   Phase 5: Validate boot info      → DAR event: MOS_EVT_ENTRY_VALIDATE
;   Phase 6: Record entry TSC        → DAR event: MOS_EVT_ENTRY_TSC
;   Phase 7: Enter kernel            → DAR event: MOS_EVT_ENTRY_CALL
;
; ═══════════════════════════════════════════════════════════════════════════════

_start:
    ; =========================================================================
    ; Phase 1: Entry Point Reached — DAR Witness: MOS_EVT_ENTRY_START
    ; =========================================================================
    ; DEBUG VGA: Show "9:KRNL" — WE ENTERED THE KERNEL!
    mov rdi, 0xB8000 + 160 * 20
    mov dword [rdi], 0x5F3A5F39         ; '9' ':'
    mov dword [rdi+4], 0x5F525F4B       ; 'K' 'R'
    mov dword [rdi+8], 0x5F4C5F4E       ; 'N' 'L'

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Disable Interrupts (void state — -1)
    ; ─────────────────────────────────────────────────────────────────────────
    ; Interrupts disabled until IDT is set up. System is in "formless" state.
    cli

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Set Up Stack (word state — 0)
    ; ─────────────────────────────────────────────────────────────────────────
    ; Stack provides the foundation for function calls.
    mov rsp, kernel_stack_top

    ; Clear direction flag (string operations go forward — toward tov)
    cld

    ; ─── DAR Witness: Log entry start (can only do this after stack is set) ───
    mov di, MOS_EVT_ENTRY_START         ; Event code
    xor si, si                          ; Status = OK
    xor ecx, ecx                        ; Value = 0
    call store_boot_event

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Clear BSS (preparation for tov state)
    ; ─────────────────────────────────────────────────────────────────────────
    ; Zero-initialize uninitialized data section.
    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi
    push rcx                            ; Save BSS size for event
    xor al, al
    rep stosb
    pop rcx                             ; Restore BSS size

    ; ─── DAR Witness: Log BSS cleared ───
    mov di, MOS_EVT_ENTRY_BSS
    xor si, si                          ; Status = OK
    ; ECX still has BSS size
    call store_boot_event

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 5: Validate Boot Info
    ; ─────────────────────────────────────────────────────────────────────────
    ; CRITICAL: Validate boot info before trusting ANY data from bootloader.
    ; This prevents crashes from corrupted handoff data.
    ;
    ; Validations:
    ;   1. Magic number == MOS_BOOT_MAGIC (0x42534F4D "MOSB")
    ;   2. Version >= MOS_BOOT_VERSION (3)
    ;   3. Framebuffer address is in reasonable range (or 0 for no graphics)
    ;   4. Memory map count is reasonable (0-256)
    ;   5. Total memory is non-zero
    ;
    ; Serial output for debugging (works even if graphics fail)
    call serial_print_entry

    mov r12, MOS_BOOT_INFO_ADDR         ; R12 = boot info pointer (preserved)

    ; --- Validation 1: Check magic number ---
    mov eax, [r12 + BOOTINFO_OFF_MAGIC]
    cmp eax, MOS_BOOT_MAGIC
    jne .validate_bad_magic

    ; --- Validation 2: Check version ---
    movzx eax, byte [r12 + BOOTINFO_OFF_VERSION]
    cmp al, MOS_BOOT_VERSION
    jb .validate_bad_version

    ; --- Validation 3: Check framebuffer (optional) ---
    mov rax, [r12 + BOOTINFO_OFF_FB_ADDR]
    test rax, rax                       ; 0 = no graphics (valid)
    jz .validate_mmap
    ; If non-zero, check it's in reasonable range (1MB to 4GB)
    cmp rax, 0x100000                   ; >= 1MB
    jb .validate_bad_fb
    mov rcx, 0x100000000                ; 4GB boundary
    cmp rax, rcx
    jae .validate_bad_fb                ; >= 4GB is suspicious

.validate_mmap:
    ; --- Validation 4: Check memory map count (0-256 reasonable) ---
    mov eax, [r12 + BOOTINFO_OFF_MMAP_COUNT]
    cmp eax, 256
    ja .validate_bad_mmap

    ; --- Validation 5: Check total memory is non-zero ---
    mov rax, [r12 + BOOTINFO_OFF_TOTAL_MEMORY]
    test rax, rax
    jz .validate_bad_memory

    ; --- All validations passed ---
    call serial_print_validated

    ; DEBUG VGA: Show "V:OK" (validation passed, green)
    mov rdi, 0xB8000 + 160 * 20 + 16
    mov dword [rdi], 0x2F4B2F4F         ; 'O' 'K'

    ; ─── DAR Witness: Log validation passed ───
    mov di, MOS_EVT_ENTRY_VALIDATE
    xor si, si                          ; Status = OK
    mov ecx, [r12 + BOOTINFO_OFF_CAPABILITIES]  ; Value = capabilities
    call store_boot_event

    jmp .validation_done

.validate_bad_magic:
    call serial_print_bad_magic
    mov di, MOS_EVT_ENTRY_VALIDATE
    mov si, MOS_EVT_STATUS_FAIL
    mov ecx, VALIDATE_BAD_MAGIC
    call store_boot_event
    mov al, VALIDATE_BAD_MAGIC
    jmp .validation_fail

.validate_bad_version:
    call serial_print_bad_version
    mov di, MOS_EVT_ENTRY_VALIDATE
    mov si, MOS_EVT_STATUS_FAIL
    mov ecx, VALIDATE_BAD_VERSION
    call store_boot_event
    mov al, VALIDATE_BAD_VERSION
    jmp .validation_fail

.validate_bad_fb:
    call serial_print_bad_fb
    mov di, MOS_EVT_ENTRY_VALIDATE
    mov si, MOS_EVT_STATUS_FAIL
    mov ecx, VALIDATE_BAD_FB
    call store_boot_event
    mov al, VALIDATE_BAD_FB
    jmp .validation_fail

.validate_bad_mmap:
    ; Log and fail on bad memory map
    mov di, MOS_EVT_ENTRY_VALIDATE
    mov si, MOS_EVT_STATUS_FAIL
    mov ecx, 0xE4                       ; Error code for bad mmap
    call store_boot_event
    mov al, 0xE4
    jmp .validation_fail

.validate_bad_memory:
    ; Log and fail on zero memory
    mov di, MOS_EVT_ENTRY_VALIDATE
    mov si, MOS_EVT_STATUS_FAIL
    mov ecx, 0xE5                       ; Error code for zero memory
    call store_boot_event
    mov al, 0xE5
    jmp .validation_fail

.validation_fail:
    ; Show error on VGA: "E:xx" where xx is error code (red)
    mov rdi, 0xB8000 + 160 * 24         ; Row 24 (bottom)
    mov dword [rdi], 0x4F3A4F45         ; 'E' ':'
    ; Convert error code to hex digits
    mov ah, al
    shr ah, 4
    and al, 0x0F
    add ah, '0'
    cmp ah, '9'
    jbe .digit1_ok
    add ah, 7                           ; A-F
.digit1_ok:
    add al, '0'
    cmp al, '9'
    jbe .digit2_ok
    add al, 7                           ; A-F
.digit2_ok:
    mov byte [rdi+4], ah
    mov byte [rdi+5], 0x4F
    mov byte [rdi+6], al
    mov byte [rdi+7], 0x4F

    ; Halt — cannot continue with invalid boot info
    cli
.validation_halt:
    hlt
    jmp .validation_halt

.validation_done:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 6: Record Entry TSC
    ; ─────────────────────────────────────────────────────────────────────────
    ; Store timestamp counter at kernel entry for boot time measurement.
    rdtsc                               ; EDX:EAX = TSC
    shl rdx, 32
    or rax, rdx                         ; RAX = full 64-bit TSC
    mov [r12 + BOOTINFO_OFF_ENTRY_TSC], rax

    ; ─── DAR Witness: Log TSC recorded ───
    mov di, MOS_EVT_ENTRY_TSC
    xor si, si                          ; Status = OK
    mov ecx, eax                        ; Value = low 32 bits of TSC
    call store_boot_event

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 7: Enter Kernel (tov state — +1)
    ; ─────────────────────────────────────────────────────────────────────────
    ; DEBUG VGA: Show "A:CALL" — about to call kernel main (orange)
    mov rdi, 0xB8000 + 160 * 21
    mov dword [rdi], 0x6F3A6F41         ; 'A' ':'
    mov dword [rdi+4], 0x6F416F43       ; 'C' 'A'
    mov dword [rdi+8], 0x6F4C6F4C       ; 'L' 'L'

    ; ─── DAR Witness: Log kernel call ───
    mov di, MOS_EVT_ENTRY_CALL
    xor si, si                          ; Status = OK
    xor ecx, ecx                        ; Value = 0
    call store_boot_event

    ; Restore RDI for kernel call (boot info pointer per System V AMD64 ABI)
    mov rdi, MOS_BOOT_INFO_ADDR

    ; Call C kernel entry — "Let there be light"
    call mos_kernel_main

    ; DEBUG VGA: Show "B:RET" — kernel returned (shouldn't happen!)
    mov rdi, 0xB8000 + 160 * 22
    mov dword [rdi], 0xCF3ACF42         ; 'B' ':' (blinking red)
    mov dword [rdi+4], 0xCF45CF52       ; 'R' 'E'
    mov word [rdi+8], 0xCF54            ; 'T'

    ; ─────────────────────────────────────────────────────────────────────────
    ; Fallback: Halt if kernel returns (shavar state — broken)
    ; ─────────────────────────────────────────────────────────────────────────
    ; Should never reach here. If we do, system is broken.
    cli
.halt:
    hlt
    jmp .halt

; ═══════════════════════════════════════════════════════════════════════════════
; B.2 DAR — DAR Witness Event Logging [DAR]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief DAR Witness boot event logging for forensic debugging.
;
; The kernel entry logs events to the boot_events[] array in MosBootInfo,
; allowing the kernel's DAR Witness system to import them for analysis.
; This provides forensic data even if the system fails before the kernel
; can fully initialize its logging infrastructure.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.2a store_boot_event — Store a boot event in MosBootInfo [STOREEVENT]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Store a boot event for DAR Witness forensics.
;
; Input:
;   DI  = event_code (16-bit: phase in high byte, step in low byte)
;   SI  = status (16-bit: 0=OK, 1=WARN, 0xFFFF=FAIL)
;   ECX = value (32-bit: optional data like address, size, error code)
;
; Output: None
;
; Clobbers: RAX, RDX, R8, R9 (all others preserved)
;
; Structure: MosBootEvent (16 bytes)
;   Offset  Size  Field       Description
;   0x00    8     tsc         TSC timestamp
;   0x08    2     event_code  Phase + Step code
;   0x0A    2     status      Result code
;   0x0C    4     value       Optional data
;
; ───────────────────────────────────────────────────────────────────────────────
store_boot_event:
    push rbx
    push r12

    ; Check if we have room for another event
    mov r8, MOS_BOOT_INFO_ADDR
    mov eax, [r8 + BOOTINFO_OFF_EVENT_COUNT]
    cmp eax, MOS_MAX_BOOT_EVENTS
    jae .store_event_full               ; Buffer full, skip

    ; Calculate event slot address: boot_info + EVENTS + (count * 16)
    mov r9d, eax                        ; R9 = current count
    shl r9d, 4                          ; R9 = count * 16 (event size)
    lea r12, [r8 + BOOTINFO_OFF_EVENTS]
    add r12, r9                         ; R12 = address of this event slot

    ; Store TSC timestamp
    rdtsc                               ; EDX:EAX = TSC
    shl rdx, 32
    or rax, rdx
    mov [r12 + BOOTEVENT_OFF_TSC], rax

    ; Store event_code (DI)
    mov [r12 + BOOTEVENT_OFF_CODE], di

    ; Store status (SI)
    mov [r12 + BOOTEVENT_OFF_STATUS], si

    ; Store value (ECX)
    mov [r12 + BOOTEVENT_OFF_VALUE], ecx

    ; Increment event count
    mov eax, [r8 + BOOTINFO_OFF_EVENT_COUNT]
    inc eax
    mov [r8 + BOOTINFO_OFF_EVENT_COUNT], eax

.store_event_full:
    pop r12
    pop rbx
    ret

; ═══════════════════════════════════════════════════════════════════════════════
; B.3 SERIAL — Serial Debug Output Functions [SERIAL]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Serial output for boot debugging.
;
; These functions output debug messages to COM1 serial port.
; Works even when graphics fail — essential for debugging boot issues.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.3a serial_char — Send single character to COM1 [CHAR]
; ───────────────────────────────────────────────────────────────────────────────
;
; Input:  AL = character to send
; Output: None
; Clobbers: None (all registers preserved)
;
serial_char:
    push rdx
    push rax
    mov dx, COM1_STATUS
.wait:
    in al, dx
    test al, 0x20                       ; Bit 5 = transmit buffer empty
    jz .wait
    pop rax
    mov dx, COM1_PORT
    out dx, al
    pop rdx
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3b serial_string — Print null-terminated string [STRING]
; ───────────────────────────────────────────────────────────────────────────────
;
; Input:  RSI = pointer to null-terminated string
; Output: None
; Clobbers: None (all registers preserved)
;
serial_string:
    push rax
    push rsi
.loop:
    lodsb
    test al, al
    jz .done
    call serial_char
    jmp .loop
.done:
    pop rsi
    pop rax
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3c serial_newline — Print CR+LF [NEWLINE]
; ───────────────────────────────────────────────────────────────────────────────
;
serial_newline:
    push rax
    mov al, 0x0D                        ; CR
    call serial_char
    mov al, 0x0A                        ; LF
    call serial_char
    pop rax
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3d Debug Message Printers [MESSAGES]
; ───────────────────────────────────────────────────────────────────────────────

serial_print_entry:
    push rsi
    lea rsi, [rel msg_entry]
    call serial_string
    call serial_newline
    pop rsi
    ret

serial_print_validated:
    push rsi
    lea rsi, [rel msg_validated]
    call serial_string
    call serial_newline
    pop rsi
    ret

serial_print_bad_magic:
    push rsi
    lea rsi, [rel msg_bad_magic]
    call serial_string
    call serial_newline
    pop rsi
    ret

serial_print_bad_version:
    push rsi
    lea rsi, [rel msg_bad_version]
    call serial_string
    call serial_newline
    pop rsi
    ret

serial_print_bad_fb:
    push rsi
    lea rsi, [rel msg_bad_fb]
    call serial_string
    call serial_newline
    pop rsi
    ret

; ═══════════════════════════════════════════════════════════════════════════════
; B.4 STRINGS — Debug Message Strings [STRINGS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Read-only strings for serial debug output.
;
; ═══════════════════════════════════════════════════════════════════════════════

section .rodata

msg_entry:      db "[ENTRY] Validating boot info at 0x9000...", 0
msg_validated:  db "[ENTRY] Boot info validated OK", 0
msg_bad_magic:  db "[ENTRY] ERROR: Invalid magic number!", 0
msg_bad_version: db "[ENTRY] ERROR: Invalid version!", 0
msg_bad_fb:     db "[ENTRY] ERROR: Invalid framebuffer address!", 0

; ═══════════════════════════════════════════════════════════════════════════════
; B.5 DATA — Stack and BSS [DATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Data allocations for kernel entry.
;
; ═══════════════════════════════════════════════════════════════════════════════

section .bss

; Kernel stack (16KB, 16-byte aligned per AMD64 ABI)
align 16
kernel_stack_bottom:
    resb KERNEL_STACK_SIZE
kernel_stack_top:

; ═══════════════════════════════════════════════════════════════════════════════
; END BODY [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; CLOSING BLOCK [CLOSING]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Synthesis, verification, and forward guidance.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; CONTAINS:
;   - X.1 VALIDATION — Build verification
;   - X.2 EXECUTION  — Entry flow summary
;   - X.3 CLEANUP    — Resource management
;   - X.4 OVERVIEW   — Module summary
;   - X.5 POLICY     — Modification guidelines
;   - X.6 FLOW       — Dependency chain
;   - X.7 PERF       — Performance considerations
;   - X.8 DEBUG      — Troubleshooting guide
;   - X.9 RELATED    — Related components
;   - X.10 ROADMAP   — Future expansions
;   - X.11 REFERENCE — Quick reference
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.1 VALIDATION — Build & Test Verification [VALIDATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Prove correctness.
;
; Build:
;   nasm -f elf64 kernel/src/entry.asm -o build/entry.o -I boot/include/
;
; Verification:
;   - objdump -d build/entry.o | grep _start  # Entry point exists
;   - nm build/entry.o | grep kernel_stack    # Stack symbol exported
;   - objdump -t build/entry.o                # All symbols present
;
; Runtime Verification:
;   - Serial: "[ENTRY] Boot info validated OK" appears
;   - VGA: "9:KRNL" marker visible at row 20
;   - VGA: "V:OK" marker visible after validation
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.2 EXECUTION — Entry Point & Flow [EXECUTION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Entry flow summary.
;
; Entry Point: _start (global symbol, linker uses this)
;
; Execution Flow:
;   1. [DEBUG] VGA marker "9:KRNL"
;   2. CLI (disable interrupts)
;   3. Stack setup (RSP = kernel_stack_top)
;   4. CLD (string direction forward)
;   5. BSS clear (zero __bss_start to __bss_end)
;   6. Serial: "[ENTRY] Validating..."
;   7. Validate magic, version, framebuffer
;   8. [PASS] Serial: "validated OK", VGA: "V:OK"
;      [FAIL] Serial: error message, VGA: "E:xx", HLT
;   9. Record entry TSC
;  10. VGA marker "A:CALL"
;  11. CALL mos_kernel_main(boot_info)
;  12. [RETURN] VGA "B:RET", HLT (should never happen)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.3 CLEANUP — Register & Resource Management [CLEANUP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Resource state at entry and exit.
;
; Entry State (from bootloader):
;   - CPU: 64-bit long mode, interrupts disabled
;   - Memory: Kernel at 0x100000, boot info at 0x9000
;   - Stack: Undefined (bootloader stack not usable)
;
; Exit State (to C kernel):
;   - RSP: kernel_stack_top (16KB stack ready)
;   - RDI: MOS_BOOT_INFO_ADDR (first parameter per AMD64 ABI)
;   - BSS: Zeroed
;   - Interrupts: Still disabled (kernel sets up IDT)
;
; No Cleanup Needed:
;   - This is one-way transition — no return expected
;   - On unexpected return, system halts
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.4 OVERVIEW — Module Summary [OVERVIEW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief High-level summary.
;
; PURPOSE: Bridge bootloader to C kernel with validated handoff.
;
; PROVIDES:
;   - _start entry point for linker
;   - Boot info validation (magic, version, framebuffer)
;   - Kernel stack (16KB)
;   - BSS clearing
;   - Entry TSC recording
;   - Serial and VGA debug output
;
; ARCHITECTURE: Ladder — top of boot chain, bottom of kernel.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.5 POLICY — Modification Guidelines [POLICY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Guide future maintainers.
;
; Safe to Modify:
;   ✅ Add more validation checks
;   ✅ Add more debug output
;   ✅ Increase stack size
;   ✅ Improve error messages
;
; Modify with Care:
;   ⚠️ Boot info address — must match bootinfo.inc and bootloader
;   ⚠️ Entry point name — linker script depends on _start
;   ⚠️ ABI compliance — RDI must be first parameter
;
; NEVER Modify:
;   ❌ Remove validation — boot must be verified
;   ❌ Change MOS_BOOT_MAGIC check — breaks handoff
;   ❌ Skip BSS clearing — C runtime depends on zeroed BSS
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.6 FLOW — Dependency Chain [FLOW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Boot chain position.
;
; Boot Chain:
;   BIOS/UEFI → Stage 0a → Stage 0b → entry.asm → kernel.c
;                          └── or ──→ UEFI boot ─┘
;
; Dependencies:
;   entry.asm
;        ↑ (jumped from)
;   stage0b.asm / uefi_boot.c
;        ↓ (calls)
;   mos_kernel_main() in kernel.c
;
; Include Chain:
;   entry.asm → bootinfo.inc → (mirrors) → bootinfo.h
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.7 PERF — Performance Considerations [PERF]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Boot time impact.
;
; Critical Path:
;   - BSS clear: O(n) where n = BSS size — typically < 1ms
;   - Validation: ~3 memory reads — negligible
;   - Serial output: ~1ms per message at 115200 baud
;
; Non-Critical:
;   - VGA writes: Direct memory access — negligible
;   - TSC read: 1 instruction — negligible
;
; Total Impact: < 10ms typical
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.8 DEBUG — Troubleshooting Guide [DEBUG]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Common issues and debugging.
;
; Problem: "9:KRNL" not visible
;   Cause:    Never reached entry — bootloader failed
;   Solution: Check stage0b serial output, verify kernel load
;
; Problem: "E:E1" (bad magic)
;   Cause:    Boot info not written or wrong address
;   Solution: Verify bootloader writes to 0x9000, check MOS_BOOT_MAGIC
;
; Problem: "E:E2" (bad version)
;   Cause:    Old bootloader with version < 3
;   Solution: Rebuild bootloader with current bootinfo.h
;
; Problem: "E:E3" (bad framebuffer)
;   Cause:    Framebuffer address out of expected range
;   Solution: Check VESA/GOP setup, verify address in 1MB-4GB range
;
; Problem: "B:RET" visible
;   Cause:    Kernel returned — should never happen
;   Solution: Check kernel for return statement, add infinite loop
;
; Serial Debug:
;   Use: minicom -D /dev/ttyUSB0 -b 115200
;   Or:  QEMU -serial stdio
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.9 RELATED — Related Components [RELATED]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Related files and components.
;
; Boot Chain:
;   - boot/stage0/stage0a.asm — Boot sector (BIOS path)
;   - boot/stage0/stage0b.asm — Second stage (BIOS path)
;   - boot/uefi/uefi_boot.c — UEFI bootloader
;
; Shared Constants:
;   - boot/include/bootinfo.inc — Assembly constants (this file includes)
;   - boot/include/bootinfo.h — C constants (kernel includes)
;
; Kernel:
;   - kernel/src/kernel.c — C entry point (mos_kernel_main)
;   - kernel/linker.ld — Linker script (defines _start, BSS symbols)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.10 ROADMAP — Future Expansions [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Planned improvements.
;
; Completed:
;   ✓ Basic validation (magic, version, framebuffer)
;   ✓ Serial debug output
;   ✓ VGA debug markers
;   ✓ Entry TSC recording
;   ✓ Full OmniCode template alignment
;
; Planned:
;   ⏳ Memory map validation
;   ⏳ ACPI/RSDP validation
;   ⏳ Boot source-specific validation
;   ⏳ DAR Witness integration (pre-kernel event buffer)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.11 REFERENCE — Quick Reference [REFERENCE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Copy-paste ready patterns.
;
; Build:
;   nasm -f elf64 kernel/src/entry.asm -o build/entry.o -I boot/include/
;
; Link (in kernel Makefile):
;   ld -T kernel/linker.ld -o kernel.bin build/entry.o build/kernel.o
;
; Debug VGA Markers:
;   "9:KRNL" — Kernel entry reached
;   "V:OK"   — Validation passed
;   "A:CALL" — About to call mos_kernel_main
;   "E:xx"   — Validation failed (xx = error code)
;   "B:RET"  — Kernel returned (shouldn't happen)
;
; Serial Messages:
;   "[ENTRY] Validating boot info at 0x9000..."
;   "[ENTRY] Boot info validated OK"
;   "[ENTRY] ERROR: Invalid magic number!"
;   "[ENTRY] ERROR: Invalid version!"
;   "[ENTRY] ERROR: Invalid framebuffer address!"
;
; Closing Note:
;   This assembly is the BRIDGE — the moment where the boot chain ends
;   and the kernel begins. "And God said, Let there be light" — assembly
;   speaks (calls C) and the kernel comes to life.
;
;   "And God said, Let there be light: and there was light."
;   — Genesis 1:3
;
; ═══════════════════════════════════════════════════════════════════════════════
; END CLOSING [END]
; ═══════════════════════════════════════════════════════════════════════════════
