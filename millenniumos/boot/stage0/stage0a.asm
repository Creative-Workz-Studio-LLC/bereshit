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
; @omni:req  key        = MOS-boot-stage0a
; @omni:req  from       = bereshit/word/seed/code/asm/source.asm
; @omni:req  at         = instance
;
; ───────────────────────────────────────────────────────────────────────────────
; P.2 FAMILY — Classification [FAMILY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:inh  type       = source
; @omni:inh  subtype    = bootloader
; @omni:inh  role       = foundation
; @omni:inh  structure  = 4-block
;
; ───────────────────────────────────────────────────────────────────────────────
; P.3 INSTANCE — File Details [INSTANCE]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  component  = stage0a
; @omni:ins  layer      = boot/stage0
; @omni:ins  includes   = none (self-contained boot sector)
; @omni:ins  provides   = [BOOT_ENTRY, STAGE0B_LOADER]
;
; ───────────────────────────────────────────────────────────────────────────────
; P.4 ARCHITECTURE — Scaling Context [ARCH]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  layers     = [0:boot]
; @omni:ins  scale      = 512 bytes (one sector)
; @omni:ins  pattern    = chain-loader (0a → 0b → kernel)
; @omni:ins  anchor     = genesis_1_1 → "In the beginning"
;
; ───────────────────────────────────────────────────────────────────────────────
; P.5 SUMMARY — Human Identity [SUMMARY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:req  title      = MillenniumOS Stage 0a - Boot Sector
; @omni:req  brief      = THE beginning — first code executed, loads Stage 0b
;
; ═══════════════════════════════════════════════════════════════════════════════
; END PRAGMA [END]
; ═══════════════════════════════════════════════════════════════════════════════
;
; DEPENDENCY CLASSIFICATION: [PURE]
;   - Self-contained - no external dependencies beyond CPU/BIOS
;   - First 512 bytes of disk, loaded at 0x7C00 by BIOS
;   - Chain-loads Stage 0b to 0x7E00
;
; ═══════════════════════════════════════════════════════════════════════════════
; METADATA BLOCK [METADATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Identity and context for Stage 0a boot sector.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; M.1 IDENTITY — Core Identity [IDENTITY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @file      stage0a.asm
; @brief     Boot Sector — THE first code executed by the CPU.
;
; Key:       MOS-boot-stage0a
; Title:     MillenniumOS Stage 0a - Boot Sector
; Type:      Source (Bootloader)
; Component: Ladder — bottom rung, everything builds from here
; Role:      Load Stage 0b from disk and transfer control

; ═══════════════════════════════════════════════════════════════════════════════
; M.2 STATE — Lifecycle State [STATE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @version   a-01.10
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
; Path:      millenniumos/boot/stage0/stage0a.asm

; ═══════════════════════════════════════════════════════════════════════════════
; M.5 DERIVATION — Template Lineage [DERIVATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Derives:   bereshit/word/seed/code/asm/source.asm (template)

; ═══════════════════════════════════════════════════════════════════════════════
; M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Tags:      [asm, nasm, boot, x86, real-mode, 16-bit, stage0a, MBR]

; ═══════════════════════════════════════════════════════════════════════════════
; M.7 INTENT — Purpose Statement [INTENT]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Purpose:   Provide the absolute first code execution path for MillenniumOS.
;            This 512-byte sector is loaded by BIOS at 0x7C00 and must:
;            1. Set up minimal real-mode environment (stack, segments)
;            2. Load Stage 0b from disk to memory
;            3. Transfer control to Stage 0b for further boot
;
; Core Design: Chain-loader pattern — keep Stage 0a minimal, delegate to 0b.
;              Genesis 1:1 principle: "In the beginning" — this IS the beginning.
;
; Key Features:
;   - Self-contained 512 bytes (one sector, ends with 0xAA55)
;   - BIOS INT 0x13 disk read (LBA or CHS)
;   - DAR Witness checkpoints for boot forensics
;   - Serial output for debugging

; ═══════════════════════════════════════════════════════════════════════════════
; M.8 GROUNDING — Biblical Foundation [GROUNDING]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Scripture: Genesis 1:1 — "In the beginning God created the heaven and
;            the earth."
; Principle: The boot sector IS "the beginning" — the absolute first code.
;            Everything else builds from this 512-byte foundation.
;            As God's creation began with separation (void → form), so does
;            the boot: BIOS → Stage0a → Stage0b → Kernel.

; ═══════════════════════════════════════════════════════════════════════════════
; M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; CPU Features:
;   - 8086/Real Mode (16-bit)
;   - BIOS INT 0x10 (video)
;   - BIOS INT 0x13 (disk)
;
; External Symbols:
;   - None (self-contained boot sector)
;
; Memory Layout:
;   - Loaded at 0x7C00 by BIOS
;   - Stack at 0x9000 (below EBDA)
;   - Stage 0b loaded to 0x7E00
;
; Used by:
;   - BIOS loads this as boot sector
;   - Transfers control to Stage 0b at 0x7E00
;
; Assemble:
;   nasm -f bin stage0a.asm -o stage0a.bin
;
; Formats:
;   - bin: Flat binary (REQUIRED — no ELF headers for MBR)

; ═══════════════════════════════════════════════════════════════════════════════
; M.10 ROADMAP — Version History [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; History:
;   a-01.00 (2026-01-30) — Initial creation with DAR Witness checkpoints
;   a-01.10 (2026-01-31) — Template alignment, full docstrings

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
;   - S.1 DIRECTIVES — Assembler configuration (BITS, ORG)
;   - S.2 CONSTANTS  — EQU definitions (addresses, sizes, ports)
;   - S.3 EXTERNAL   — Not used (self-contained boot sector)
;   - S.4 DATA       — Moved to end of BODY (space-sensitive boot sector)
;   - S.5 BSS        — Not used (no BSS in flat binary)
;   - S.6 DEBUG      — Witness infrastructure (inline with code)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; S.1 DIRECTIVES — Assembler Configuration [DIRECTIVES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Configuration telling NASM how to assemble this file.
;
; PURPOSE: Set CPU mode and origin address for boot sector.
;          BIOS loads MBR to 0x7C00, so we must match.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.1a MODE — CPU Bit Mode [MODE]
; ───────────────────────────────────────────────────────────────────────────────
;
; Real Mode (16-bit) — BIOS hands off in real mode.
; All registers are 16-bit, segment:offset addressing.

bits 16

; ───────────────────────────────────────────────────────────────────────────────
; S.1b ORIGIN — Load Address [ORIGIN]
; ───────────────────────────────────────────────────────────────────────────────
;
; Boot sector origin 0x7C00 — standard BIOS load address.
; This address is hardcoded in x86 BIOS since the IBM PC (1981).

org 0x7C00

; ═══════════════════════════════════════════════════════════════════════════════
; S.2 CONSTANTS — EQU Definitions [CONSTANTS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Named compile-time values — no memory used at runtime.
;
; PURPOSE: Define boot-critical constants. Addresses, sizes, magic values.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.2a ADDRESSES — Memory Layout Constants [ADDRESSES]
; ───────────────────────────────────────────────────────────────────────────────

; STAGE0B_ADDR — Load address for Stage 0b.
;
; @brief Stage 0b loads immediately after boot sector in memory.
;        0x7C00 (boot sector) + 512 (sector size) = 0x7E00.
;
; @value 0x7E00 — Linear address where Stage 0b code begins.
STAGE0B_ADDR        equ 0x7E00

; STACK_TOP — Top of real-mode stack.
;
; @brief Stack grows downward from 0x9000.
;        Below EBDA (Extended BIOS Data Area) at 0x9FC00.
;        Provides ~8KB stack space (0x7E00 to 0x9000).
;
; @value 0x9000 — Stack pointer initial value.
STACK_TOP           equ 0x9000

; WITNESS_ADDR — DAR Witness checkpoint storage.
;
; @brief Memory location for boot forensics.
;        Survives crashes — memory dump can find checkpoints.
;
; @value 0x9000 — Same as stack top (writes before stack use).
WITNESS_ADDR        equ 0x9000

; ───────────────────────────────────────────────────────────────────────────────
; S.2b SIZES — Sector and Count Constants [SIZES]
; ───────────────────────────────────────────────────────────────────────────────

; STAGE0B_SECTORS — Number of sectors to load for Stage 0b.
;
; @brief Load 32 sectors (16KB) for Stage 0b.
;        Stage 0b is larger — handles CPUID, memory map, mode switch.
;
; @value 32 — Sector count (32 × 512 = 16,384 bytes).
STAGE0B_SECTORS     equ 32

; ───────────────────────────────────────────────────────────────────────────────
; S.2c HARDWARE — Port Constants [HARDWARE]
; ───────────────────────────────────────────────────────────────────────────────

; COM1_BASE — Serial port 1 base address.
;
; @brief Standard COM1 I/O port for serial debug output.
;        Used by DAR Witness for boot logging.
;
; @value 0x3F8 — COM1 data/control base port.
COM1_BASE           equ 0x3F8

; COM1_LSR — COM1 Line Status Register.
;
; @brief Check TX buffer empty before sending character.
;        Bit 5 = TX holding register empty.
;
; @value 0x3FD — COM1 base + 5 (Line Status).
COM1_LSR            equ 0x3FD

; ───────────────────────────────────────────────────────────────────────────────
; S.2d WITNESS — DAR Checkpoint Values [WITNESS]
; ───────────────────────────────────────────────────────────────────────────────

; WITNESS_0A_ENTRY — Stage 0a entry checkpoint.
;
; @brief Written to WITNESS_ADDR on Stage 0a entry.
;        High byte 0x0A = Stage 0a, low byte = checkpoint number.
;
; @value 0x0A01 — Stage 0A, checkpoint 1 (entry).
WITNESS_0A_ENTRY    equ 0x0A01

; WITNESS_0A_LOAD — Stage 0a loading checkpoint.
;
; @brief Written before INT 0x13 disk read.
;
; @value 0x0A02 — Stage 0A, checkpoint 2 (loading).
WITNESS_0A_LOAD     equ 0x0A02

; WITNESS_0A_OK — Stage 0a load success checkpoint.
;
; @brief Written after successful Stage 0b load.
;
; @value 0x0A03 — Stage 0A, checkpoint 3 (success).
WITNESS_0A_OK       equ 0x0A03

; WITNESS_0A_JUMP — Stage 0a jump to 0b checkpoint.
;
; @brief Written immediately before JMP to Stage 0b.
;        If boot hangs here, Stage 0b is broken.
;
; @value 0x0A04 — Stage 0A, checkpoint 4 (jumping).
WITNESS_0A_JUMP     equ 0x0A04

; WITNESS_0A_ERR — Stage 0a error checkpoint.
;
; @brief Written on disk read failure.
;        0xFF indicates error state (-1 in ternary).
;
; @value 0x0AFF — Stage 0A, error (0xFF).
WITNESS_0A_ERR      equ 0x0AFF

; ═══════════════════════════════════════════════════════════════════════════════
; S.3 EXTERNAL — Symbol Declarations [EXTERNAL]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Not used — boot sector is self-contained.
;
; Boot sector cannot link with other object files.
; All code must fit in 512 bytes (510 code + 2 signature).
;
; ═══════════════════════════════════════════════════════════════════════════════

; (none — self-contained)

; ═══════════════════════════════════════════════════════════════════════════════
; S.4 DATA — Initialized Data [DATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Deferred to end of BODY section.
;
; In boot sectors, data is placed after code to maximize instruction space.
; See B.4 DATA section below.
;
; ═══════════════════════════════════════════════════════════════════════════════

; (see B.4 DATA below)

; ═══════════════════════════════════════════════════════════════════════════════
; S.5 BSS — Uninitialized Data [BSS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Not used — flat binary has no BSS section.
;
; Boot sector is flat binary (no sections). All data is initialized
; or uses reserved memory outside the boot sector.
;
; ═══════════════════════════════════════════════════════════════════════════════

; (none — flat binary)

; ═══════════════════════════════════════════════════════════════════════════════
; S.6 DEBUG — Debug Infrastructure [DEBUG]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief DAR Witness system for boot forensics.
;
; Witness checkpoints are embedded inline with boot code.
; Serial output provides real-time boot progress.
; Memory checkpoints survive crashes for post-mortem analysis.
;
; ═══════════════════════════════════════════════════════════════════════════════

; (inline with BODY code)

; ═══════════════════════════════════════════════════════════════════════════════
; END SETUP [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; BODY BLOCK [BODY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief The actual work — boot sector implementation.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; CONTAINS:
;   - B.1 ORG CHART    — Label hierarchy, baton flow, APU counts
;   - B.2 HELPERS      — VGA and serial output routines
;   - B.3 CORE OPS     — Entry point, serial init, disk loading
;   - B.4 ERRORS       — Disk error handling, halt
;   - B.5 DATA         — Strings and variables (at end for space)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; B.1 ORG CHART — Label Hierarchy [ORGCHART]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Map internal structure — label hierarchy, baton flow, APU counts.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.1a STRUCTURE — Label Hierarchy [STRUCTURE]
; ───────────────────────────────────────────────────────────────────────────────
;
; Entry Points (implicit — boot sector starts at ORG)
; └── boot_start → sets up segments, calls helpers, loads Stage 0b
;
; Core Operations (B.3)
; ├── boot_start     → entry point, orchestrates boot
; ├── .serial_init   → configure COM1 for 115200 8N1
; └── .load_stage0b  → BIOS INT 0x13 disk read
;
; Helpers (B.2)
; ├── print_string   → VGA TTY output via BIOS INT 0x10
; ├── serial_print   → serial string output via COM1
; └── serial_char    → single character to COM1
;
; Errors (B.4)
; ├── .disk_error    → handle disk read failure
; └── .halt          → infinite halt loop

; ───────────────────────────────────────────────────────────────────────────────
; B.1b FLOW — Baton Execution Path [FLOW]
; ───────────────────────────────────────────────────────────────────────────────
;
; BIOS loads MBR to 0x7C00
;   ↓
; boot_start: CLI, set segments, save boot drive
;   ↓
; .serial_init: Configure COM1 @ 115200
;   ↓
; serial_print(msg_witness): DAR Witness entry marker
;   ↓
; Write WITNESS_0A_ENTRY to memory
;   ↓
; STI, print_string(msg_boot): VGA banner
;   ↓
; Write WITNESS_0A_LOAD, INT 0x13 read sectors
;   ↓
; On error: .disk_error → .halt
; On success: Write WITNESS_0A_OK, serial_print(msg_witness_ok)
;   ↓
; print_string(msg_loaded): VGA confirmation
;   ↓
; Write WITNESS_0A_JUMP, JMP STAGE0B_ADDR

; ───────────────────────────────────────────────────────────────────────────────
; B.1c COUNTS — APU Statistics [COUNTS]
; ───────────────────────────────────────────────────────────────────────────────
;
; Labels:         8 total
; Entry Points:   1 (boot_start — implicit)
; Core Ops:       3 (boot_start, .serial_init, .load_stage0b inline)
; Helpers:        3 (print_string, serial_print, serial_char)
; Error Handlers: 2 (.disk_error, .halt)

; ═══════════════════════════════════════════════════════════════════════════════
; B.2 HELPERS — Foundation Routines [HELPERS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief VGA and serial output routines.
;
; PURPOSE: Provide visible feedback during boot.
;          VGA for user, serial for debug/DAR Witness.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.2a VGA — Video Output Helpers [VGA]
; ───────────────────────────────────────────────────────────────────────────────

; print_string outputs a null-terminated string to VGA via BIOS.
;
; @brief  Print string to VGA display using BIOS TTY mode.
;         Uses INT 0x10 AH=0x0E (teletype output).
;
; Parameters (registers):
;   SI: Pointer to null-terminated string.
;
; Returns:
;   Nothing (string printed to screen).
;
; Clobbers: None (all registers preserved via PUSHA).
;
; Example usage:
;     mov si, msg_hello
;     call print_string
;
print_string:
    pusha                               ; Preserve all registers
.loop:
    lodsb                               ; AL = [SI], SI++
    test al, al                         ; Check for null terminator
    jz .done                            ; If zero, string complete
    mov ah, 0x0E                        ; BIOS TTY function
    mov bh, 0                           ; Page 0
    int 0x10                            ; Call BIOS video service
    jmp .loop                           ; Next character
.done:
    popa                                ; Restore all registers
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.2b SERIAL — Serial Output Helpers [SERIAL]
; ───────────────────────────────────────────────────────────────────────────────

; serial_print outputs a null-terminated string to COM1.
;
; @brief  Print string to serial port for DAR Witness logging.
;         Calls serial_char for each character.
;
; Parameters (registers):
;   SI: Pointer to null-terminated string.
;
; Returns:
;   Nothing (string sent to COM1).
;
; Clobbers: None (all registers preserved via PUSHA).
;
; Example usage:
;     mov si, msg_debug
;     call serial_print
;
serial_print:
    pusha                               ; Preserve all registers
.sloop:
    lodsb                               ; AL = [SI], SI++
    test al, al                         ; Check for null terminator
    jz .sdone                           ; If zero, string complete
    call serial_char                    ; Send character
    jmp .sloop                          ; Next character
.sdone:
    popa                                ; Restore all registers
    ret

; serial_char sends a single character to COM1.
;
; @brief  Send one byte to COM1 with TX ready wait.
;         Polls Line Status Register for TX buffer empty.
;
; Parameters (registers):
;   AL: Character to send.
;
; Returns:
;   Nothing (character sent to COM1).
;
; Clobbers: None (DX and AX preserved via stack).
;
; Example usage:
;     mov al, 'A'
;     call serial_char
;
serial_char:
    push dx                             ; Preserve DX
    push ax                             ; Preserve character
    mov dx, COM1_LSR                    ; Line Status Register
.swait:
    in al, dx                           ; Read status
    test al, 0x20                       ; Bit 5 = TX holding reg empty
    jz .swait                           ; Wait if not ready
    pop ax                              ; Restore character
    mov dx, COM1_BASE                   ; Data register
    out dx, al                          ; Send character
    pop dx                              ; Restore DX
    ret

; ═══════════════════════════════════════════════════════════════════════════════
; B.3 CORE OPS — Boot Entry and Disk Loading [COREOPS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Main boot logic — entry point, serial init, Stage 0b loading.
;
; PURPOSE: Orchestrate the boot process:
;          1. Set up real-mode environment
;          2. Initialize serial for DAR Witness
;          3. Load Stage 0b from disk
;          4. Transfer control to Stage 0b
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.3a ENTRY — Boot Sector Entry Point [ENTRY]
; ───────────────────────────────────────────────────────────────────────────────

; boot_start is the first instruction executed by MillenniumOS.
;
; @brief  Boot sector entry point — THE beginning.
;         Called implicitly by BIOS after loading MBR to 0x7C00.
;         Sets up segments, stack, serial, then loads Stage 0b.
;
; Parameters (registers):
;   DL: Boot drive number (passed by BIOS).
;
; Returns:
;   Never returns — jumps to Stage 0b or halts on error.
;
; Clobbers: All registers (no preservation needed at boot).
;
boot_start:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Disable Interrupts
    ; ─────────────────────────────────────────────────────────────────────────
    ; Prevent interrupts during segment/stack setup.
    ; Interrupts re-enabled after stack is valid.
    cli

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Set Up Segments
    ; ─────────────────────────────────────────────────────────────────────────
    ; Zero all segment registers for flat real-mode addressing.
    ; Stack segment set before stack pointer for atomicity.
    xor ax, ax                          ; AX = 0
    mov ds, ax                          ; Data segment = 0
    mov es, ax                          ; Extra segment = 0
    mov ss, ax                          ; Stack segment = 0
    mov sp, STACK_TOP                   ; Stack pointer = 0x9000

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Save Boot Drive
    ; ─────────────────────────────────────────────────────────────────────────
    ; BIOS passes boot drive in DL. Save for INT 0x13 calls.
    mov [boot_drive], dl                ; Store boot drive number

; ───────────────────────────────────────────────────────────────────────────────
; B.3b SERIAL_INIT — COM1 Configuration [SERIAL_INIT]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Initialize COM1 serial port for 115200 baud, 8N1.
;         Used by DAR Witness system for boot logging.
;
; Configuration:
;   - Baud rate:  115200 (divisor = 1)
;   - Data bits:  8
;   - Stop bits:  1
;   - Parity:     None
;   - Flow ctrl:  None (no RTS/CTS)
;

    ; Disable serial interrupts
    mov dx, 0x3F9                       ; Interrupt Enable Register
    xor al, al                          ; Disable all interrupts
    out dx, al

    ; Set baud rate divisor (DLAB must be on)
    mov dx, 0x3FB                       ; Line Control Register
    mov al, 0x80                        ; DLAB = 1 (access divisor)
    out dx, al

    mov dx, COM1_BASE                   ; Divisor Latch Low
    mov al, 1                           ; Divisor = 1 (115200 baud)
    out dx, al

    mov dx, 0x3F9                       ; Divisor Latch High
    xor al, al                          ; High byte = 0
    out dx, al

    ; Set line format: 8N1, disable DLAB
    mov dx, 0x3FB                       ; Line Control Register
    mov al, 0x03                        ; 8 bits, no parity, 1 stop, DLAB=0
    out dx, al

    ; ─────────────────────────────────────────────────────────────────────────
    ; DAR WITNESS: Entry Checkpoint
    ; ─────────────────────────────────────────────────────────────────────────
    ; Output stage marker to serial for real-time logging.
    mov si, msg_witness                 ; "[0A] Stage 0a entry\r\n"
    call serial_print

    ; Store checkpoint in memory for post-mortem analysis.
    ; This survives crashes — memory dump can find it.
    mov word [WITNESS_ADDR], WITNESS_0A_ENTRY

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Enable Interrupts
    ; ─────────────────────────────────────────────────────────────────────────
    ; Stack is valid, safe to enable interrupts for BIOS calls.
    sti

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 5: VGA Banner
    ; ─────────────────────────────────────────────────────────────────────────
    ; Show user we're alive via VGA display.
    mov si, msg_boot                    ; "MOS 0a\r\n"
    call print_string

; ───────────────────────────────────────────────────────────────────────────────
; B.3c LOAD — Stage 0b Disk Read [LOAD]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Load Stage 0b from disk using BIOS INT 0x13.
;         Uses CHS addressing (cylinder 0, head 0, sector 2+).
;
; Disk Layout:
;   Sector 1:     Stage 0a (this boot sector, 512 bytes)
;   Sectors 2-33: Stage 0b (32 sectors, 16KB)
;

    ; DAR WITNESS: Loading checkpoint
    mov word [WITNESS_ADDR], WITNESS_0A_LOAD

    ; BIOS INT 0x13 AH=0x02: Read Sectors
    mov ah, 0x02                        ; Function: read sectors
    mov al, STAGE0B_SECTORS             ; Sector count (32)
    mov ch, 0                           ; Cylinder 0
    mov cl, 2                           ; Start sector 2 (1-indexed)
    mov dh, 0                           ; Head 0
    mov dl, [boot_drive]                ; Drive number
    mov bx, STAGE0B_ADDR                ; ES:BX = destination (0x7E00)
    int 0x13                            ; Call BIOS disk service
    jc .disk_error                      ; CF set on error

    ; Verify all sectors were read
    cmp al, STAGE0B_SECTORS             ; AL = sectors actually read
    jne .disk_error                     ; Partial read is failure

    ; DAR WITNESS: Success checkpoint
    mov word [WITNESS_ADDR], WITNESS_0A_OK
    mov si, msg_witness_ok              ; "[0A] Load OK\r\n"
    call serial_print

    ; VGA confirmation
    mov si, msg_loaded                  ; "->0b\r\n"
    call print_string

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 6: Transfer to Stage 0b
    ; ─────────────────────────────────────────────────────────────────────────
    ; DAR WITNESS: Jump checkpoint
    mov word [WITNESS_ADDR], WITNESS_0A_JUMP

    ; Pass boot drive to Stage 0b in DL (convention)
    mov dl, [boot_drive]
    jmp STAGE0B_ADDR                    ; Never returns

; ═══════════════════════════════════════════════════════════════════════════════
; B.4 ERRORS — Error Handling [ERRORS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Disk error handling and halt loop.
;
; PURPOSE: Handle unrecoverable boot failures gracefully.
;          Log error via DAR Witness, display to user, halt CPU.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.4a DISK_ERROR — Disk Read Failure Handler [DISK_ERROR]
; ───────────────────────────────────────────────────────────────────────────────

; .disk_error handles INT 0x13 read failures.
;
; @brief  Log disk error to DAR Witness, display to user, halt.
;         Called when CF is set after INT 0x13 or sector count mismatch.
;
; Flow: Set error checkpoint → VGA output → serial output → halt
;
.disk_error:
    ; DAR WITNESS: Error checkpoint (0xFF = failure state)
    mov word [WITNESS_ADDR], WITNESS_0A_ERR

    ; Display error to user (VGA)
    mov si, msg_disk_err                ; "ERR!"
    call print_string

    ; Log error to serial (DAR Witness)
    call serial_print                   ; SI still points to msg_disk_err
    ; Fall through to halt

; ───────────────────────────────────────────────────────────────────────────────
; B.4b HALT — Infinite Halt Loop [HALT]
; ───────────────────────────────────────────────────────────────────────────────

; .halt stops the CPU in an infinite loop.
;
; @brief  Disable interrupts and halt CPU forever.
;         Loop handles spurious wakeups (some hardware issues NMI).
;
.halt:
    cli                                 ; Disable interrupts
    hlt                                 ; Halt CPU
    jmp .halt                           ; Loop on spurious wakeup

; ═══════════════════════════════════════════════════════════════════════════════
; B.5 DATA — Initialized Data [DATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief String constants and variables.
;
; PURPOSE: Data placed after code to maximize instruction space.
;          Boot sector is space-critical — every byte counts.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.5a VARIABLES — Runtime State [VARIABLES]
; ───────────────────────────────────────────────────────────────────────────────

; boot_drive stores the BIOS boot drive number.
;
; @brief  Saved from DL on entry for INT 0x13 calls.
;         Passed to Stage 0b in DL register.
;
; @size   1 byte
; @init   0 (overwritten on boot)
;
boot_drive:     db 0

; ───────────────────────────────────────────────────────────────────────────────
; B.5b STRINGS — Message Constants [STRINGS]
; ───────────────────────────────────────────────────────────────────────────────

; msg_boot — VGA banner displayed on Stage 0a entry.
;
; @brief  Short banner identifying MillenniumOS Stage 0a.
;         Minimal to save space (6 chars + CRLF + null = 9 bytes).
;
msg_boot:       db "MOS 0a", 13, 10, 0

; msg_loaded — VGA confirmation after successful Stage 0b load.
;
; @brief  Arrow indicates transition to next stage.
;         User knows boot is progressing.
;
msg_loaded:     db "->0b", 13, 10, 0

; msg_disk_err — Error message for disk read failure.
;
; @brief  Minimal error indicator.
;         No newline to save space — halt follows immediately.
;
msg_disk_err:   db "ERR!", 0

; msg_witness — DAR Witness entry marker for serial log.
;
; @brief  Identifies Stage 0a entry in serial output.
;         Format: [XX] description (XX = stage ID).
;
msg_witness:    db "[0A] Stage 0a entry", 13, 10, 0

; msg_witness_ok — DAR Witness success marker for serial log.
;
; @brief  Confirms Stage 0b load completed successfully.
;
msg_witness_ok: db "[0A] Load OK", 13, 10, 0

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
; GROUP 1 — OPERATIONS:
;   - X.1 VALIDATION — Build verification, size constraints
;   - X.2 EXECUTION  — Boot flow summary
;   - X.3 CLEANUP    — Resource management
;
; GROUP 2 — DOCUMENTATION:
;   - X.4 OVERVIEW    — Module summary
;   - X.5 POLICY      — Modification guidelines
;   - X.6 FLOW        — Execution path
;   - X.7 PERF        — Performance considerations
;   - X.8 DEBUG       — Troubleshooting guide
;   - X.9 RELATED     — Related components
;   - X.10 ROADMAP    — Future expansions
;   - X.11 REFERENCE  — Quick reference
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.1 VALIDATION — Build & Test Verification [VALIDATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Prove correctness — assemble, size verify, boot test.
;
; Build Verification:
;   nasm -f bin stage0a.asm -o stage0a.bin
;   # Must assemble without errors or warnings
;
; Size Verification:
;   ls -l stage0a.bin
;   # MUST be exactly 512 bytes
;   # If larger: code/data overflow — reduce size
;   # If smaller: padding math error
;
; Signature Verification:
;   hexdump -C stage0a.bin | tail -1
;   # Last two bytes MUST be 55 AA (little-endian 0xAA55)
;
; Boot Testing:
;   qemu-system-x86_64 -drive format=raw,file=millenniumos.img
;   # Should display "MOS 0a" then "->0b"
;   # Serial log: "[0A] Stage 0a entry" then "[0A] Load OK"
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.2 EXECUTION — Entry Point & Flow [EXECUTION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Entry point and execution summary.
;
; Entry Point: boot_start (implicit at ORG 0x7C00)
;
; Execution Flow:
;   1. BIOS loads MBR (sector 1) to 0x7C00
;   2. BIOS jumps to 0x7C00 (boot_start)
;   3. Stage 0a sets up segments, stack
;   4. Stage 0a initializes COM1 serial
;   5. Stage 0a loads Stage 0b to 0x7E00
;   6. Stage 0a jumps to 0x7E00
;
; Register State on Entry (from BIOS):
;   DL: Boot drive number (0x00 = floppy, 0x80 = first HDD)
;   CS: Usually 0x0000 (but not guaranteed)
;   IP: 0x7C00
;
; Register State on Exit (to Stage 0b):
;   DL: Boot drive number (passed through)
;   All others: Undefined (Stage 0b must reinitialize)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.3 CLEANUP — Register & Resource Management [CLEANUP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Resource management in boot sector context.
;
; Resource Management:
;   - Registers: Helper functions use PUSHA/POPA (preserve all)
;   - Stack: Set to 0x9000, grows down, ~8KB space
;   - Memory: Witness checkpoint at 0x9000 (before stack use)
;   - Serial: COM1 initialized, no cleanup needed
;
; Callee-Saved: print_string, serial_print preserve all registers
; Caller-Saved: boot_start clobbers all (entry point, no return)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.4 OVERVIEW — Module Summary [OVERVIEW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief High-level summary — references METADATA for details.
;
; Purpose:  See METADATA M.7 INTENT
; Provides: Boot entry, serial logging, Stage 0b loading
;
; Quick summary:
;   Stage 0a is THE beginning — the absolute first code executed by
;   MillenniumOS. It exists in 512 bytes, sets up minimal environment,
;   loads Stage 0b, and transfers control. DAR Witness markers enable
;   boot forensics if something goes wrong.
;
; Architecture: Ladder — bottom rung, everything builds from here
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.5 POLICY — Modification Guidelines [POLICY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Guide future maintainers on what's safe to change.
;
; Safe to Modify (Extension Points):
;   ✅ Message strings (if size permits)
;   ✅ Witness checkpoint values (keep format)
;   ✅ Add VGA color (if space available)
;
; Modify with Extreme Care (Breaking Changes):
;   ⚠️ Memory addresses — affects entire boot chain
;   ⚠️ Sector count — must match Stage 0b actual size
;   ⚠️ Serial configuration — affects DAR Witness output
;
; NEVER Modify (Foundational Rails):
;   ❌ ORG 0x7C00 — BIOS-mandated load address
;   ❌ Boot signature 0xAA55 — BIOS won't recognize as bootable
;   ❌ Total size (must be exactly 512 bytes)
;   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.6 FLOW — Ladder & Baton [FLOW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Execution flow — references BODY B.1 ORG CHART.
;
; See BODY B.1 ORG CHART for complete structure.
;
; Quick summary:
;   Ladder: BIOS → Stage 0a (this file)
;   Baton:  boot_start → serial_init → disk_load → JMP Stage 0b
;
; Chain: Stage 0a (512B) → Stage 0b (16KB) → Kernel
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.7 PERF — Performance Considerations [PERF]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Performance is not a concern — correctness is.
;
; Boot sector runs once. Performance is irrelevant compared to:
;   - Correctness (must load Stage 0b perfectly)
;   - Reliability (must work on all BIOS implementations)
;   - Size (must fit in 512 bytes)
;
; The only "performance" metric that matters:
;   - Disk read time (limited by BIOS, ~100ms for 16KB)
;   - Serial output time (limited by 115200 baud, ~2ms per line)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.8 DEBUG — Troubleshooting Guide [DEBUG]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Common boot sector issues and debugging techniques.
;
; Problem: "BIOS doesn't boot from disk"
;   Cause:    Missing 0xAA55 signature at bytes 510-511
;   Solution: Check `dw 0xAA55` at end, verify file is 512 bytes
;
; Problem: "No VGA output, system hangs"
;   Cause:    Segments not set up, jump to wrong address
;   Solution: Check serial output for witness markers, debug with QEMU
;
; Problem: "ERR! displayed"
;   Cause:    INT 0x13 disk read failed
;   Solution: Check QEMU -hda argument, verify disk image structure
;
; Problem: "VGA shows garbage"
;   Cause:    Segment registers not zeroed
;   Solution: Ensure DS=ES=SS=0 before any memory access
;
; Debug Commands:
;   # QEMU with serial output to terminal
;   qemu-system-x86_64 -hda millenniumos.img -serial stdio
;
;   # QEMU with GDB stub for debugging
;   qemu-system-x86_64 -hda millenniumos.img -s -S
;   gdb -ex "target remote :1234" -ex "set architecture i8086"
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.9 RELATED — Related Components [RELATED]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Related modules — references METADATA M.9.
;
; See METADATA M.9 DEPENDENCIES for complete information.
;
; Quick summary:
;   Dependencies: BIOS INT 0x10, INT 0x13 (video, disk)
;   Dependents:   Stage 0b (loaded by this file)
;
; Related Files:
;   - boot/stage0/stage0b.asm   — Next stage (loaded to 0x7E00)
;   - boot/stage0/bootinfo.h    — Boot info structure (populated by 0b)
;   - boot/stage0/bootinfo.inc  — Assembly constants for boot info
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.10 ROADMAP — Future Expansions [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Planned features (space permitting).
;
; Planned Features:
;   ✓ DAR Witness checkpoints — COMPLETED
;   ✓ Serial logging — COMPLETED
;   ⏳ VGA color output (if space available)
;   ⏳ Boot drive validation (check BIOS reported drive)
;
; Research Areas:
;   - LBA fallback for modern disks
;   - A20 gate early enable (currently done in Stage 0b)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.11 REFERENCE — Quick Reference [REFERENCE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Copy-paste ready commands and patterns.
;
; Assemble (flat binary):
;     nasm -f bin stage0a.asm -o stage0a.bin
;
; Verify size:
;     ls -l stage0a.bin  # Must be 512 bytes
;
; Verify signature:
;     hexdump -C stage0a.bin | tail -1  # Ends with 55 aa
;
; Build full image:
;     make  # In millenniumos/ root
;
; Test with QEMU:
;     qemu-system-x86_64 -drive format=raw,file=build/millenniumos.img
;
; Test with serial:
;     qemu-system-x86_64 -drive format=raw,file=build/millenniumos.img \
;                        -serial stdio
;
; Closing Note:
;   Stage 0a is the bottom rung of the ladder — everything builds from here.
;   As Genesis 1:1 says "In the beginning," this IS the beginning of
;   MillenniumOS. Guard its simplicity. Resist scope creep. 512 bytes is
;   all you have.
;
;   "In the beginning God created the heaven and the earth." — Genesis 1:1
;
; ═══════════════════════════════════════════════════════════════════════════════
; END CLOSING [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; BOOT SECTOR PADDING AND SIGNATURE
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Pad to 510 bytes and add boot signature.
;
; The MBR (Master Boot Record) MUST be exactly 512 bytes:
;   - Bytes 0-509:   Code and data (510 bytes)
;   - Bytes 510-511: Boot signature 0xAA55 (little-endian)
;
; If code exceeds 510 bytes, assembly fails with:
;   "error: TIMES value -N is negative"
;
; ═══════════════════════════════════════════════════════════════════════════════

times 510 - ($ - $$) db 0               ; Pad with zeros to byte 510
dw 0xAA55                               ; BIOS boot signature (little-endian)
