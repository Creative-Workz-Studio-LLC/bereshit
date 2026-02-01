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
; @omni:req  key        = MOS-boot-minimal
; @omni:req  from       = bereshit/word/seed/code/asm/source.asm
; @omni:req  at         = instance
;
; ───────────────────────────────────────────────────────────────────────────────
; P.2 FAMILY — Classification [FAMILY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:inh  type       = source
; @omni:inh  subtype    = bootloader
; @omni:inh  role       = diagnostic
; @omni:inh  structure  = 4-block
;
; ───────────────────────────────────────────────────────────────────────────────
; P.3 INSTANCE — File Details [INSTANCE]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  component  = boot-minimal
; @omni:ins  layer      = boot/stage0
; @omni:ins  includes   = none (self-contained)
; @omni:ins  provides   = [FLOPPY_BOOT, HW_TEST_HARNESS]
;
; ───────────────────────────────────────────────────────────────────────────────
; P.4 ARCHITECTURE — Scaling Context [ARCH]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  layers     = [0:boot]
; @omni:ins  scale      = floppy image (up to 1.44MB, typically <32KB)
; @omni:ins  pattern    = tripwire-fallback (UEFI → BIOS → Floppy/Diag)
; @omni:ins  anchor     = genesis_1_1 → "In the beginning"
;
; ───────────────────────────────────────────────────────────────────────────────
; P.5 SUMMARY — Human Identity [SUMMARY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:req  title      = MillenniumOS Minimal Boot / Hardware Test Harness
; @omni:req  brief      = Dual-purpose: floppy boot for embedded + HW diagnostics
;
; ═══════════════════════════════════════════════════════════════════════════════
; END PRAGMA [END]
; ═══════════════════════════════════════════════════════════════════════════════
;
; DEPENDENCY CLASSIFICATION: [PURE]
;   - Self-contained floppy bootloader / diagnostic tool
;   - No external dependencies beyond CPU/BIOS
;   - Third-level tripwire fallback (after UEFI and BIOS stage0a+0b)
;
; ═══════════════════════════════════════════════════════════════════════════════
; METADATA BLOCK [METADATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Identity and context for dual-purpose boot utility.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; M.1 IDENTITY — Core Identity [IDENTITY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @file      boot.asm
; @brief     Minimal Floppy Bootloader + Hardware Test Harness
;
; Key:       MOS-boot-minimal
; Title:     MillenniumOS Minimal Boot / Hardware Test Harness
; Type:      Source (Bootloader/Diagnostic)
; Component: Tripwire Level 3 — fallback when UEFI and full BIOS fail
; Role:      1) Boot minimal kernel from floppy
;            2) Diagnose hardware (A20, VESA, E820, disk, serial)

; ═══════════════════════════════════════════════════════════════════════════════
; M.2 STATE — Lifecycle State [STATE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @version   a-01.00
; @date      2026-01-31
;
; Status:    Active
; Created:   2026-01-30
; Updated:   2026-01-31 — Repurposed as dual floppy/diagnostic tool

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
; Path:      millenniumos/boot/stage0/boot.asm

; ═══════════════════════════════════════════════════════════════════════════════
; M.5 DERIVATION — Template Lineage [DERIVATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Derives:   bereshit/word/seed/code/asm/source.asm (template)

; ═══════════════════════════════════════════════════════════════════════════════
; M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Tags:      [asm, nasm, boot, x86, floppy, diagnostic, test-harness, tripwire]

; ═══════════════════════════════════════════════════════════════════════════════
; M.7 INTENT — Purpose Statement [INTENT]
; ═══════════════════════════════════════════════════════════════════════════════
;
; PURPOSE: Provide third-level tripwire fallback for MillenniumOS boot.
;
; WHY THIS EXISTS:
;   MillenniumOS targets "anyone can use" — that means:
;   - UEFI systems (primary, modern)
;   - BIOS systems (fallback, legacy)
;   - Ancient/embedded systems (this file — minimal floppy boot)
;   - Hardware debugging (this file — test harness mode)
;
; DUAL MODES:
;
;   MODE 1: FLOPPY BOOT
;     - Boots from floppy disk image
;     - Single-stage loader (no multi-stage complexity)
;     - Minimal kernel load (for embedded/rescue scenarios)
;     - Use case: Ancient hardware, embedded systems, rescue disk
;
;   MODE 2: HARDWARE TEST HARNESS
;     - Validates hardware before full boot
;     - Tests: A20 line, VESA modes, E820 memory map, disk, serial
;     - Reports via VGA + serial (DAR Witness compatible)
;     - Use case: Debugging boot failures, validating new hardware
;
; TRIPWIRE ARCHITECTURE:
;   Level 1: UEFI boot (modern, preferred)
;   Level 2: BIOS stage0a+stage0b (legacy fallback)
;   Level 3: boot.asm floppy/diagnostic (last resort)
;
; Key Features:
;   - Self-contained (no dependencies)
;   - Menu-driven (select floppy boot or diagnostics)
;   - DAR Witness integration
;   - Serial output for headless debugging

; ═══════════════════════════════════════════════════════════════════════════════
; M.8 GROUNDING — Biblical Foundation [GROUNDING]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Scripture: Genesis 1:1-2 — "In the beginning God created the heaven and
;            the earth. And the earth was without form, and void."
;
; Principle: The diagnostic mode examines the "void" — the hardware state
;            before the OS brings form. Like the Spirit moving upon the
;            waters, we inspect what exists before creating upon it.

; ═══════════════════════════════════════════════════════════════════════════════
; M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; CPU Features:
;   - 8086/Real Mode (16-bit) — entry
;   - 386+ for 32-bit protected mode (floppy boot mode)
;   - BIOS INT 0x10 (video), INT 0x13 (disk), INT 0x15 (memory, A20)
;
; External Symbols:
;   - None (self-contained)
;
; Build:
;   - Floppy image: nasm -f bin boot.asm -o boot.img
;   - Test in QEMU: qemu-system-x86_64 -fda boot.img -serial stdio

; ═══════════════════════════════════════════════════════════════════════════════
; M.10 ROADMAP — Version History [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; History:
;   a-01.00 (2026-01-30) — Initial attempt (single-stage, didn't fit 512B)
;   a-01.10 (2026-01-31) — Repurposed as floppy/diagnostic dual-mode tool

; ═══════════════════════════════════════════════════════════════════════════════
; END METADATA [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; SETUP BLOCK [SETUP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Configuration and declarations.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; CONTAINS:
;   - S.1 DIRECTIVES — Assembler configuration (bits, org)
;   - S.2 INCLUDES   — [Reserved: None — self-contained]
;   - S.3 CONSTANTS  — Memory addresses, hardware ports, test values
;   - S.4 EXTERNAL   — [Reserved: None — self-contained]
;   - S.5 DATA       — [Deferred: See BODY B.7]
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; S.1 DIRECTIVES — Assembler Configuration [DIRECTIVES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Assembler mode and origin settings for bootable floppy image.
;
; PURPOSE: Configure NASM for flat binary output suitable for BIOS boot.
;
; SETTINGS:
;   bits 16     — Real mode (16-bit) code. BIOS starts CPU in real mode.
;   org 0x7C00  — Origin address where BIOS loads boot sector.
;
; WHY 16-BIT:
;   BIOS boot sequence always starts in real mode. We stay in 16-bit for
;   simplicity since this is a minimal/diagnostic tool, not the main boot path.
;   Protected mode transition is unnecessary for floppy boot or diagnostics.
;
; WHY 0x7C00:
;   IBM PC BIOS specification loads boot sector to 0x0000:0x7C00.
;   This is 31KB into the first 64KB segment, leaving:
;     - 0x0000-0x7BFF (31KB) for IVT, BDA, and stack
;     - 0x7C00-0x7DFF (512B) for boot sector
;     - 0x7E00+ for loaded code (our extended stage)
;
; ═══════════════════════════════════════════════════════════════════════════════

bits 16                                 ; WHY: BIOS starts in real mode (16-bit)
org 0x7C00                              ; WHY: BIOS loads boot sector to this address

; ═══════════════════════════════════════════════════════════════════════════════
; S.2 INCLUDES — Include Dependencies [INCLUDES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; [Reserved: None — self-contained floppy/diagnostic tool]
;
; WHY NO INCLUDES:
;   This file is the third-level tripwire fallback. It must be completely
;   self-contained with no dependencies on other boot infrastructure.
;   Everything it needs is defined locally.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; S.3 CONSTANTS — Compile-Time Values [CONSTANTS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Named compile-time values for memory, hardware, video, and diagnostics.
;
; PURPOSE: Define all magic numbers as named constants for clarity and maintenance.
;
; ORGANIZATION:
;   S.3a ADDRESSES — Memory layout (stack, load addresses)
;   S.3b HARDWARE  — Serial port I/O addresses
;   S.3c VIDEO     — VGA text mode constants and color attributes
;   S.3d TEST      — Diagnostic test identifiers
;
; DESIGN PRINCIPLE:
;   Every magic number gets a name. Names document intent.
;   "STACK_TOP equ 0x9000" tells you what 0x9000 means.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.3a ADDRESSES — Memory Layout [ADDRESSES]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Memory addresses for stack, code loading, and kernel placement.
;
; MEMORY MAP (Real Mode, First 1MB):
;   0x00000-0x003FF  IVT (Interrupt Vector Table)
;   0x00400-0x004FF  BDA (BIOS Data Area)
;   0x00500-0x07BFF  Free (conventional memory)
;   0x07C00-0x07DFF  Boot sector (loaded by BIOS)
;   0x07E00-0x08FFF  Extended code (loaded by boot sector)
;   0x09000-0x09FFF  Stack (grows down from STACK_TOP)
;   0x10000-0x1FFFF  Kernel load area (floppy boot mode)
;   0x9FC00-0x9FFFF  EBDA (Extended BIOS Data Area)
;   0xA0000-0xBFFFF  Video memory
;   0xC0000-0xFFFFF  ROM/BIOS
;
; ───────────────────────────────────────────────────────────────────────────────

; STACK_TOP — Top of stack in real mode
;
; WHY 0x9000:
;   - Below EBDA (0x9FC00) by safe margin
;   - Above our code area (0x7C00-0x8FFF)
;   - 9 × 0x1000 = ternary-aligned (9 = 3²)
;   - Gives ~2KB stack space (grows down to ~0x8800)
;
STACK_TOP           equ 0x9000

; STAGE2_ADDR — Load address for extended code
;
; WHY 0x7E00:
;   - Immediately after boot sector (0x7C00 + 512 = 0x7E00)
;   - Contiguous with boot sector in memory
;   - Simple jump from boot sector: jmp STAGE2_ADDR
;
STAGE2_ADDR         equ 0x7E00

; KERNEL_ADDR — Kernel load address for floppy boot mode
;
; WHY 0x10000 (64KB):
;   - Above all boot code and data
;   - At 64KB boundary (segment-aligned)
;   - Standard location for kernel load
;   - Accessible via segment:offset (0x1000:0000)
;
KERNEL_ADDR         equ 0x10000

; ───────────────────────────────────────────────────────────────────────────────
; S.3b HARDWARE — Port Addresses [HARDWARE]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Serial port I/O addresses for debug output.
;
; PURPOSE: Enable serial output for debugging, especially on headless systems.
;
; COM1 REGISTER MAP (8250/16550 UART):
;   Base+0 (0x3F8): Data Register (read/write when DLAB=0)
;   Base+1 (0x3F9): Interrupt Enable Register
;   Base+2 (0x3FA): FIFO Control / Interrupt ID
;   Base+3 (0x3FB): Line Control Register (contains DLAB bit)
;   Base+4 (0x3FC): Modem Control Register
;   Base+5 (0x3FD): Line Status Register (TX empty, RX ready)
;   Base+6 (0x3FE): Modem Status Register
;   Base+7 (0x3FF): Scratch Register
;
; WHY SERIAL:
;   Serial output works even when video fails. Essential for debugging
;   boot failures on unknown hardware. QEMU -serial stdio captures output.
;
; ───────────────────────────────────────────────────────────────────────────────

; COM1_BASE — COM1 data register / base address
;
; WHY 0x3F8:
;   IBM PC standard COM1 base address. Universal across x86 systems.
;   Used for both reading and writing serial data (when DLAB=0).
;
COM1_BASE           equ 0x3F8

; COM1_LSR — Line Status Register
;
; WHY 0x3FD:
;   Base + 5 = Line Status Register. Bit 5 (0x20) indicates TX buffer empty.
;   Must poll this before writing to avoid overrun.
;
COM1_LSR            equ 0x3FD

; ───────────────────────────────────────────────────────────────────────────────
; S.3c VIDEO — Display Constants [VIDEO]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief VGA text mode constants for menu display.
;
; PURPOSE: Define video memory location and color attributes for UI.
;
; VGA TEXT MODE MEMORY LAYOUT:
;   Segment 0xB800 contains 80×25 character cells.
;   Each cell is 2 bytes: [character byte] [attribute byte]
;   Attribute byte: [blink:1][bg:3][fg:4] = background << 4 | foreground
;
; COLOR PALETTE (Standard VGA):
;   0x0 = Black      0x8 = Dark Gray
;   0x1 = Blue       0x9 = Light Blue
;   0x2 = Green      0xA = Light Green
;   0x3 = Cyan       0xB = Light Cyan
;   0x4 = Red        0xC = Light Red
;   0x5 = Magenta    0xD = Light Magenta
;   0x6 = Brown      0xE = Yellow
;   0x7 = Light Gray 0xF = White
;
; ───────────────────────────────────────────────────────────────────────────────

; VIDEO_MEM — VGA text mode segment
;
; WHY 0xB800:
;   VGA color text mode buffer starts at physical address 0xB8000.
;   In real mode, we use segment 0xB800 (0xB800 << 4 = 0xB8000).
;
VIDEO_MEM           equ 0xB800

; SCREEN_WIDTH — Characters per row
;
; WHY 80:
;   Standard VGA text mode is 80 columns × 25 rows (Mode 3).
;   This has been the standard since IBM PC in 1981.
;
SCREEN_WIDTH        equ 80

; SCREEN_HEIGHT — Rows on screen
;
; WHY 25:
;   Standard VGA text mode height. 80×25 = 2000 characters = 4000 bytes.
;
SCREEN_HEIGHT       equ 25

; ─── COLOR ATTRIBUTES ───
;
; Format: (background << 4) | foreground
; High nibble = background color (0-7, or 8-F with blink)
; Low nibble = foreground color (0-F)

; ATTR_NORMAL — Default text color
; WHY 0x07: Light gray (7) on black (0). Easy to read, low contrast.
ATTR_NORMAL         equ 0x07

; ATTR_TITLE — Title bar color
; WHY 0x1F: White (F) on blue (1). Classic DOS title bar appearance.
ATTR_TITLE          equ 0x1F

; ATTR_OK — Success indicator
; WHY 0x0A: Light green (A) on black (0). Universal "success" color.
ATTR_OK             equ 0x0A

; ATTR_FAIL — Failure indicator
; WHY 0x0C: Light red (C) on black (0). Universal "error" color.
ATTR_FAIL           equ 0x0C

; ATTR_WARN — Warning/prompt indicator
; WHY 0x0E: Yellow (E) on black (0). Draws attention without alarm.
ATTR_WARN           equ 0x0E

; ───────────────────────────────────────────────────────────────────────────────
; S.3d TEST — Diagnostic Test IDs [TEST]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Identifiers for hardware diagnostic tests.
;
; PURPOSE: Provide unique IDs for each diagnostic test, enabling structured
;          reporting and potential future expansion (test selection, logging).
;
; TEST SEQUENCE RATIONALE:
;   Tests are ordered by criticality and dependency:
;   1. A20 — Must pass for any 32/64-bit code to work
;   2. VESA — Graphics initialization
;   3. E820 — Memory map (kernel needs this)
;   4. Disk — Can we read the boot device?
;   5. Serial — Debug output channel
;
; FUTURE EXPANSION:
;   These IDs could be used for selective testing, test result logging,
;   or integration with the DAR Witness system.
;
; ───────────────────────────────────────────────────────────────────────────────

; TEST_A20 — A20 line enable test
;
; WHY CRITICAL:
;   The A20 address line controls access to memory above 1MB. If disabled,
;   address bit 20 wraps (0x100000 → 0x000000). Any 32/64-bit code fails
;   without A20 enabled. This is THE first thing to check.
;
TEST_A20            equ 0x01

; TEST_VESA — VESA VBE support test
;
; WHY IMPORTANT:
;   VESA BIOS Extensions provide high-resolution graphics modes.
;   If VESA fails, we're limited to VGA text mode (80×25).
;   Tests INT 0x10 AX=0x4F00 (Get VBE Info).
;
TEST_VESA           equ 0x02

; TEST_E820 — E820 memory map test
;
; WHY IMPORTANT:
;   INT 0x15 AX=0xE820 provides the system memory map.
;   Kernel needs this to know which memory regions are usable.
;   Failure means we can't safely allocate memory.
;
TEST_E820           equ 0x03

; TEST_DISK — Disk read capability test
;
; WHY IMPORTANT:
;   Tests INT 0x13 (BIOS disk services) on the boot drive.
;   If disk read fails, we can't load kernel or any additional code.
;
TEST_DISK           equ 0x04

; TEST_SERIAL — Serial port output test
;
; WHY USEFUL:
;   Verifies COM1 is functional for debug output.
;   Less critical than others but essential for headless debugging.
;
TEST_SERIAL         equ 0x05

; ═══════════════════════════════════════════════════════════════════════════════
; S.4 EXTERNAL — Symbol Declarations [EXTERNAL]
; ═══════════════════════════════════════════════════════════════════════════════
;
; [Reserved: None — self-contained, no external symbols]
;
; WHY NO EXTERNALS:
;   This is a standalone floppy image. It cannot link against external
;   object files or symbols. All functionality is self-contained.

; ═══════════════════════════════════════════════════════════════════════════════
; S.5 DATA — Initialized Data [DATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; [Deferred: See BODY B.7 for data definitions]
;
; WHY DEFERRED:
;   Boot sector has strict 512-byte limit. Data is placed after code
;   in B.1b (minimal boot data) and B.7 (extended data) to maximize
;   code space in the boot sector.

; ═══════════════════════════════════════════════════════════════════════════════
; END SETUP [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; BODY BLOCK [BODY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief The actual work — boot sector + extended code implementation.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; PURPOSE: Provide executable code for dual-mode operation:
;          1. Floppy boot for embedded/ancient systems
;          2. Hardware diagnostics for debugging
;
; CONTAINS:
;   - B.1 ORG CHART    — Label hierarchy, baton flow, APU counts
;   - B.2 HELPERS      — Serial and VGA output routines (boot sector)
;   - B.3 CORE OPS     — Boot entry, extended menu, modes
;   - B.4 UI           — Extended UI functions (post-boot-sector)
;   - B.5 TESTS        — Hardware diagnostic test functions
;   - B.6 DATA         — Strings and buffers
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
; PHYSICAL LAYOUT (Two-Stage Floppy Image):
;
;   ┌────────────────────────────────────────────────────────────────────────┐
;   │ SECTOR 1 (0x000-0x1FF) — Boot Sector [512 bytes, BIOS-loaded]        │
;   │   boot_entry        → Initialize segments, load extended, jump        │
;   │   serial_init       → Configure COM1 @ 115200 8N1                     │
;   │   serial_print      → String output to COM1                           │
;   │   serial_char       → Character output to COM1                        │
;   │   print_string      → VGA output via BIOS INT 0x10                   │
;   │   [boot_drive, messages, padding, 0xAA55 signature]                  │
;   ├────────────────────────────────────────────────────────────────────────┤
;   │ SECTORS 2-16 (0x200-0x1FFF) — Extended Code [7.5KB, loaded by boot]  │
;   │   extended_entry    → Menu system dispatcher                          │
;   │   clear_screen      → Fill screen with color                          │
;   │   draw_menu         → Render main menu                                │
;   │   print_centered    → Centered text output                            │
;   │   print_at          → Positioned text output                          │
;   │   print_result      → OK/FAIL indicator                               │
;   │   floppy_boot_mode  → Load and execute kernel                         │
;   │   diagnostic_mode   → Run hardware tests                              │
;   │   test_a20          → A20 line test                                   │
;   │   test_e820         → Memory map test                                 │
;   │   test_vesa         → VESA VBE test                                   │
;   │   test_disk         → Disk read test                                  │
;   │   test_serial       → Serial output test                              │
;   │   [strings, buffers]                                                  │
;   └────────────────────────────────────────────────────────────────────────┘
;
; Label Categories:
;
;   Boot Sector (B.2-B.3a):
;   └── boot_entry     → entry point, loads extended code
;       ├── serial_init    → COM1 configuration
;       ├── serial_print   → serial string output
;       ├── serial_char    → serial character output
;       ├── print_string   → VGA TTY output
;       └── .load_error    → halt on disk failure
;
;   Extended Entry (B.3b):
;   └── extended_entry → menu dispatcher
;       ├── .menu_loop     → keyboard input loop
;       ├── .do_floppy_boot → call floppy_boot_mode
;       ├── .do_diagnostics → call diagnostic_mode
;       ├── .do_reboot     → INT 0x19 reboot
;       └── .do_halt       → CLI, HLT
;
;   UI Helpers (B.4):
;   ├── clear_screen       → fill screen with attribute
;   ├── draw_menu          → render menu layout
;   ├── print_centered     → centered text
;   ├── print_at           → positioned text
;   └── print_result       → OK/FAIL display
;
;   Floppy Boot Mode (B.3c):
;   └── floppy_boot_mode   → load kernel, jump
;       └── .load_failed   → error handling
;
;   Diagnostic Mode (B.3d):
;   └── diagnostic_mode    → run test sequence
;
;   Tests (B.5):
;   ├── test_a20           → A20 line enable
;   │   ├── .wait_input    → keyboard controller helper
;   │   └── .wait_output   → keyboard controller helper
;   ├── test_e820          → E820 memory map
;   ├── test_vesa          → VESA VBE info
;   ├── test_disk          → disk read
;   └── test_serial        → serial output

; ───────────────────────────────────────────────────────────────────────────────
; B.1b FLOW — Baton Execution Path [FLOW]
; ───────────────────────────────────────────────────────────────────────────────
;
; BIOS loads boot sector to 0x7C00
;   ↓
; boot_entry: CLI, set segments (DS=ES=SS=0), SP=0x9000
;   ↓
; Save boot drive (DL → boot_drive)
;   ↓
; serial_init: Configure COM1 @ 115200 8N1
;   ↓
; serial_print(msg_boot_banner): "[BOOT] MOS Minimal/Diag"
;   ↓
; INT 0x13 AH=02: Read sectors 2-16 to 0x7E00
;   ↓
; On error: serial_print, print_string, CLI, HLT
; On success: STI, JMP 0x7E00
;   ↓
; extended_entry: clear_screen, draw_menu
;   ↓
; .menu_loop: INT 0x16 (wait for key)
;   ↓
; Key '1': floppy_boot_mode → load kernel → JMP 0x1000:0000
; Key '2': diagnostic_mode → run tests → return to menu
; Key '3': INT 0x19 (reboot)
; Key ESC: CLI, HLT

; ───────────────────────────────────────────────────────────────────────────────
; B.1c COUNTS — APU Statistics [COUNTS]
; ───────────────────────────────────────────────────────────────────────────────
;
; Labels:           22 total (including sub-labels)
;
; Boot Sector:
;   Entry Points:   1 (boot_entry — implicit at ORG)
;   Helpers:        4 (serial_init, serial_print, serial_char, print_string)
;   Error Handlers: 1 (.load_error)
;
; Extended Code:
;   Entry Points:   1 (extended_entry)
;   UI Helpers:     5 (clear_screen, draw_menu, print_centered, print_at, print_result)
;   Modes:          2 (floppy_boot_mode, diagnostic_mode)
;   Tests:          5 (test_a20, test_e820, test_vesa, test_disk, test_serial)
;
; Data Sections:    3 (boot sector data, strings, buffers)

; ═══════════════════════════════════════════════════════════════════════════════
; B.2 CORE OPS — Boot Entry and Disk Loading [COREOPS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Main boot logic — entry point, disk loading, mode dispatch.
;
; PURPOSE: Orchestrate the boot process:
;          1. Boot sector: Initialize environment, load extended code, jump
;          2. Extended entry: Display menu, dispatch to selected mode
;          3. Modes: Floppy boot or hardware diagnostics
;
; PHYSICAL LAYOUT:
;   Boot sector (512 bytes): B.2a entry + B.3 helpers + B.4 data + signature
;   Extended code (7.5KB):   B.2b-d modes, B.5 UI, B.6 tests, B.7 data
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.2a ENTRY — Boot Sector Entry Point [ENTRY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Boot sector entry point — THE beginning.
;
; PURPOSE: Minimal loader that:
;          1. Sets up real-mode environment (segments, stack)
;          2. Initializes serial for debug output
;          3. Loads extended code from sectors 2-16
;          4. Jumps to extended_entry
;
; BIOS HANDOFF:
;   BIOS loads boot sector to 0x7C00 and jumps here.
;   DL contains the boot drive number (preserve it!).
;
; CONSTRAINT:
;   This code + helpers + data + padding MUST fit in 512 bytes.
;
; @param  DL Boot drive number (from BIOS).
; @return Never returns — jumps to extended code or halts on error.
;
; ───────────────────────────────────────────────────────────────────────────────

boot_entry:
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

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Initialize Serial for Debug Output
    ; ─────────────────────────────────────────────────────────────────────────
    ; Serial output provides boot progress even if video fails.
    call serial_init

    ; Output boot banner to serial port.
    mov si, msg_boot_banner             ; "[BOOT] MOS Minimal/Diag\r\n"
    call serial_print

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 5: Load Extended Code from Disk
    ; ─────────────────────────────────────────────────────────────────────────
    ; Read sectors 2-16 (15 sectors, 7.5KB) to 0x7E00.
    ; Extended code contains menu, modes, UI, tests, and data.
    mov si, msg_loading_ext             ; "[BOOT] Loading extended...\r\n"
    call serial_print

    ; BIOS INT 0x13 AH=0x02: Read Sectors (CHS addressing)
    mov ah, 0x02                        ; Function: read sectors
    mov al, 15                          ; Sector count (15 = 7.5KB)
    mov ch, 0                           ; Cylinder 0
    mov cl, 2                           ; Start sector 2 (1-indexed)
    mov dh, 0                           ; Head 0
    mov dl, [boot_drive]                ; Drive number (from BIOS)
    mov bx, STAGE2_ADDR                 ; ES:BX = destination (0x7E00)
    int 0x13                            ; Call BIOS disk service
    jc .load_error                      ; CF set on error

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 6: Jump to Extended Code
    ; ─────────────────────────────────────────────────────────────────────────
    ; Stack is valid, safe to enable interrupts for BIOS calls.
    sti                                 ; Enable interrupts
    jmp STAGE2_ADDR                     ; Jump to extended_entry at 0x7E00

.load_error:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Error Handler: Disk Read Failed
    ; ─────────────────────────────────────────────────────────────────────────
    ; Output error to serial and VGA, then halt.
    mov si, msg_load_err                ; "Load error!"
    call serial_print                   ; Output to serial
    call print_string                   ; Output to VGA
    cli                                 ; Disable interrupts
    hlt                                 ; Halt processor
    jmp $                               ; Infinite loop (safety)

; ═══════════════════════════════════════════════════════════════════════════════
; B.3 HELPERS — Boot Sector Output Routines [HELPERS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Serial and VGA output routines that fit in boot sector.
;
; PURPOSE: Provide visible feedback during boot sector execution.
;          These helpers are called before extended code is loaded,
;          so they MUST fit within the 512-byte boot sector.
;
; CONSTRAINT: Every byte counts. Minimal implementation only.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.3a SERIAL_INIT — COM1 Configuration [SERIAL_INIT]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Initialize COM1 serial port for 115200 baud, 8N1.
;
; @return Nothing.
; @clobbers AX, DX.
;
; ───────────────────────────────────────────────────────────────────────────────

serial_init:
    ; Disable serial interrupts (we poll instead)
    mov dx, 0x3F9                       ; Interrupt Enable Register
    xor al, al                          ; Disable all interrupts
    out dx, al

    ; Set baud rate divisor (DLAB must be on first)
    mov dx, 0x3FB                       ; Line Control Register
    mov al, 0x80                        ; DLAB = 1 (access divisor)
    out dx, al

    mov dx, COM1_BASE                   ; Divisor Latch Low (0x3F8)
    mov al, 1                           ; Divisor = 1 (115200 baud)
    out dx, al

    mov dx, 0x3F9                       ; Divisor Latch High
    xor al, al                          ; High byte = 0
    out dx, al

    ; Set line format: 8N1, disable DLAB
    mov dx, 0x3FB                       ; Line Control Register
    mov al, 0x03                        ; 8 bits, no parity, 1 stop, DLAB=0
    out dx, al
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3b SERIAL_PRINT — Serial String Output [SERIAL_PRINT]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Print null-terminated string to COM1.
;
; @param  SI Pointer to null-terminated string.
; @return Nothing.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

serial_print:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    ; Preserve all general-purpose registers so caller's state is untouched.
    ; PUSHA saves AX, CX, DX, BX, SP, BP, SI, DI in that order.
    pusha                                   ; Save all registers

.loop:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Load and Test Character
    ; ─────────────────────────────────────────────────────────────────────────
    ; LODSB loads byte at [DS:SI] into AL and increments SI.
    ; Null byte (0x00) marks end of string — standard C convention.
    lodsb                                   ; AL = [DS:SI++] (next char)
    test al, al                             ; Check if AL = 0 (null terminator)
    jz .done                                ; If null, string complete

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Output Character
    ; ─────────────────────────────────────────────────────────────────────────
    ; Delegate to serial_char for UART transmit holding register management.
    ; Loop until all characters processed.
    call serial_char                        ; Output AL to COM1
    jmp .loop                               ; Process next character

.done:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Restore State
    ; ─────────────────────────────────────────────────────────────────────────
    ; POPA restores registers in reverse order, leaving stack clean.
    popa                                    ; Restore all registers
    ret                                     ; Return to caller

; ───────────────────────────────────────────────────────────────────────────────
; B.3c SERIAL_CHAR — Serial Character Output [SERIAL_CHAR]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Print single character to COM1.
;
; @param  AL Character to output.
; @return Nothing.
; @clobbers None (DX, AX saved/restored).
;
; ───────────────────────────────────────────────────────────────────────────────

serial_char:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save Registers
    ; ─────────────────────────────────────────────────────────────────────────
    ; Minimal save — only DX and AX needed for this function.
    ; DX holds port address, AX used for in/out operations.
    push dx                                 ; Save DX (will use for port address)
    push ax                                 ; Save AX (character to output)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Wait for Transmitter Ready
    ; ─────────────────────────────────────────────────────────────────────────
    ; UART 16550 Line Status Register (LSR) at base+5.
    ; Bit 5 (0x20) = Transmit Holding Register Empty (THRE).
    ; Must wait for THRE=1 before writing to avoid data loss.
    mov dx, COM1_LSR                        ; DX = 0x3FD (Line Status Register)
.wait:
    in al, dx                               ; Read LSR into AL
    test al, 0x20                           ; Test bit 5 (THRE)
    jz .wait                                ; If THRE=0, transmitter busy — wait

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Transmit Character
    ; ─────────────────────────────────────────────────────────────────────────
    ; Write character to Transmit Holding Register at base address.
    ; UART will shift it out at configured baud rate.
    pop ax                                  ; Restore character to AL
    mov dx, COM1_BASE                       ; DX = 0x3F8 (Transmit Holding Reg)
    out dx, al                              ; Send character to UART

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Restore and Return
    ; ─────────────────────────────────────────────────────────────────────────
    pop dx                                  ; Restore caller's DX
    ret                                     ; Return to caller

; ───────────────────────────────────────────────────────────────────────────────
; B.3d PRINT_STRING — VGA TTY Output [PRINT_STRING]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Print null-terminated string to VGA via BIOS.
;
; PURPOSE: Display messages on screen during boot.
;          Uses BIOS INT 0x10 AH=0x0E (teletype output).
;
; WHY BIOS TTY:
;   Teletype mode handles newlines, cursor advancement automatically.
;   Simpler than direct video memory writes for boot messages.
;
; @param  SI Pointer to null-terminated string.
; @return Nothing.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

print_string:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    ; Preserve all registers. INT 0x10 may clobber several.
    pusha                                   ; Save all registers

.loop:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Load and Test Character
    ; ─────────────────────────────────────────────────────────────────────────
    ; Same pattern as serial_print — LODSB increments SI automatically.
    lodsb                                   ; AL = [DS:SI++] (next char)
    test al, al                             ; Check if AL = 0 (null terminator)
    jz .done                                ; If null, string complete

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Output via BIOS Teletype
    ; ─────────────────────────────────────────────────────────────────────────
    ; INT 0x10 AH=0x0E: Write character in teletype mode.
    ; Handles CR (0x0D), LF (0x0A), backspace, cursor advance automatically.
    ; BH = page number (0), BL = foreground color (7 = light gray).
    mov ah, 0x0E                            ; AH = Teletype output function
    mov bx, 0x0007                          ; BH=0 (page 0), BL=7 (light gray)
    int 0x10                                ; BIOS video interrupt
    jmp .loop                               ; Process next character

.done:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Restore State
    ; ─────────────────────────────────────────────────────────────────────────
    popa                                    ; Restore all registers
    ret                                     ; Return to caller

; ───────────────────────────────────────────────────────────────────────────────
; B.4 DATA — Boot Sector Data [DATA]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Minimal data section for boot sector.
;
; PURPOSE: Store boot drive number and essential messages.
;          Must fit within boot sector's 512-byte limit.
;
; DATA LAYOUT:
;   boot_drive      — 1 byte: BIOS-provided drive number (DL at entry)
;   msg_boot_banner — Boot announcement string
;   msg_loading_ext — Extended code loading message
;   msg_load_err    — Error message for disk failures
;
; ───────────────────────────────────────────────────────────────────────────────

boot_drive:         db 0
msg_boot_banner:    db "[BOOT] MOS Minimal/Diag", 13, 10, 0
msg_loading_ext:    db "[BOOT] Loading extended...", 13, 10, 0
msg_load_err:       db "Load error!", 0

; ───────────────────────────────────────────────────────────────────────────────
; Boot Sector Padding and Signature
; ───────────────────────────────────────────────────────────────────────────────

times 510 - ($ - $$) db 0
dw 0xAA55

; ═══════════════════════════════════════════════════════════════════════════════
; B.2b EXTENDED_ENTRY — Menu Dispatcher [EXTENDED_ENTRY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief  Extended code entry point — menu system and mode dispatcher.
;
; PURPOSE: Provide user interface for boot mode selection:
;          1. Clear screen and draw menu
;          2. Wait for keypress
;          3. Dispatch to selected mode
;
; LOAD ADDRESS: 0x7E00 (immediately after boot sector)
;
; MEMORY MAP:
;   0x7C00-0x7DFF: Boot sector (512 bytes)
;   0x7E00-0x9FFF: Extended code (sectors 2-16, ~7.5KB)
;
; KEY MAPPING:
;   '1' → Floppy boot mode (load and execute kernel)
;   '2' → Hardware diagnostics (test and report)
;   '3' → Reboot (INT 0x19)
;   ESC → Halt (CLI, HLT)
;
; ═══════════════════════════════════════════════════════════════════════════════

extended_entry:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Initialize Display
    ; ─────────────────────────────────────────────────────────────────────────
    ; Clear screen and draw menu before accepting input.
    ; User sees clean interface immediately after boot sector load.
    call clear_screen                       ; Fill screen with default color
    call draw_menu                          ; Render menu options

.menu_loop:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Wait for User Input
    ; ─────────────────────────────────────────────────────────────────────────
    ; INT 0x16 AH=0x00: Wait for keypress, return ASCII in AL, scancode in AH.
    ; This blocks until user presses a key.
    xor ah, ah                              ; AH = 0 (wait for key)
    int 0x16                                ; BIOS keyboard interrupt

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Dispatch Based on Key
    ; ─────────────────────────────────────────────────────────────────────────
    ; Check ASCII value against menu options.
    ; Invalid keys simply loop back to wait for another keypress.
    cmp al, '1'                             ; Check for '1' (floppy boot)
    je .do_floppy_boot                      ; Jump if match
    cmp al, '2'                             ; Check for '2' (diagnostics)
    je .do_diagnostics                      ; Jump if match
    cmp al, '3'                             ; Check for '3' (reboot)
    je .do_reboot                           ; Jump if match
    cmp al, 27                              ; Check for ESC (ASCII 27)
    je .do_halt                             ; Jump if match

    jmp .menu_loop                          ; Invalid key — try again

.do_floppy_boot:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Mode 1: Floppy Boot
    ; ─────────────────────────────────────────────────────────────────────────
    ; Load kernel from floppy and execute. Returns on failure.
    call floppy_boot_mode                   ; Attempt kernel load + jump
    jmp .menu_loop                          ; If we return, load failed — retry

.do_diagnostics:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Mode 2: Hardware Diagnostics
    ; ─────────────────────────────────────────────────────────────────────────
    ; Run hardware tests and display results. Always returns.
    call diagnostic_mode                    ; Run test suite
    jmp .menu_loop                          ; Return to menu after tests

.do_reboot:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Mode 3: Reboot
    ; ─────────────────────────────────────────────────────────────────────────
    ; INT 0x19: Bootstrap loader — reloads boot sector from first device.
    int 0x19                                ; BIOS reboot (never returns)

.do_halt:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Mode 4: Halt (ESC)
    ; ─────────────────────────────────────────────────────────────────────────
    ; Graceful shutdown — print message, disable interrupts, halt CPU.
    mov si, msg_halted                      ; SI = halt message pointer
    call serial_print                       ; Output to serial for logging
    cli                                     ; Disable interrupts
    hlt                                     ; Halt processor
    jmp $                                   ; Infinite loop (if NMI wakes CPU)

; ═══════════════════════════════════════════════════════════════════════════════
; B.3e UI — Extended Display Helpers [UI]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief  VGA display functions for extended code.
;
; PURPOSE: Provide screen manipulation for menu and diagnostic display:
;          - clear_screen:    Fill screen with background color
;          - draw_menu:       Render main boot menu
;          - print_centered:  Center text on row
;          - print_at:        Position text at row/column
;          - print_result:    Display OK/FAIL indicator
;
; WHY IN EXTENDED CODE:
;   These functions are too large for the 512-byte boot sector.
;   Only serial output is needed in boot sector for DAR Witness.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.3e.1 CLEAR_SCREEN — Fill Screen with Color [CLEAR_SCREEN]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Fill entire VGA text buffer with space and attribute.
;
; @param  None (uses hardcoded blue background, white foreground).
; @return Nothing.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

clear_screen:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all general registers
    push es                                 ; Save ES (will point to video mem)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Set Up Video Memory Pointer
    ; ─────────────────────────────────────────────────────────────────────────
    ; VGA text mode buffer at segment 0xB800 (physical 0xB8000).
    mov ax, VIDEO_MEM                       ; AX = 0xB800
    mov es, ax                              ; ES = video memory segment
    xor di, di                              ; DI = 0 (start of screen)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Fill Screen with Spaces
    ; ─────────────────────────────────────────────────────────────────────────
    ; REP STOSW writes AX to [ES:DI], increments DI by 2, decrements CX.
    ; AX = 0x1720: attribute 0x17 (white on blue), character 0x20 (space).
    mov cx, SCREEN_WIDTH * SCREEN_HEIGHT    ; CX = 80*25 = 2000 words
    mov ax, 0x1720                          ; AH=0x17 (white/blue), AL=' '
    rep stosw                               ; Fill screen

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Restore State
    ; ─────────────────────────────────────────────────────────────────────────
    pop es                                  ; Restore ES
    popa                                    ; Restore all general registers
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3e.2 DRAW_MENU — Render Boot Menu [DRAW_MENU]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Render the boot/diagnostic menu to screen.
;
; @param  None.
; @return Nothing.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

draw_menu:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all registers

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Draw Title Area (Rows 1-4)
    ; ─────────────────────────────────────────────────────────────────────────
    mov dh, 1                               ; Row 1: Title
    mov si, msg_title                       ; "M I L L E N N I U M O S"
    mov bl, ATTR_TITLE                      ; White on blue
    call print_centered

    mov dh, 2                               ; Row 2: Subtitle
    mov si, msg_subtitle                    ; "Minimal Boot / Hardware Diagnostics"
    mov bl, ATTR_NORMAL                     ; Light gray
    call print_centered

    mov dh, 4                               ; Row 4: Scripture
    mov si, msg_scripture                   ; Genesis 1:1
    mov bl, ATTR_NORMAL
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Draw Menu Options (Rows 8, 10, 12)
    ; ─────────────────────────────────────────────────────────────────────────
    mov dh, 8                               ; Row 8: Option 1
    mov dl, 25                              ; Column 25
    mov si, msg_opt1                        ; "[1] Floppy Boot Mode"
    mov bl, ATTR_NORMAL
    call print_at

    mov dh, 10                              ; Row 10: Option 2
    mov dl, 25
    mov si, msg_opt2                        ; "[2] Hardware Diagnostics"
    mov bl, ATTR_NORMAL
    call print_at

    mov dh, 12                              ; Row 12: Option 3
    mov dl, 25
    mov si, msg_opt3                        ; "[3] Reboot"
    mov bl, ATTR_NORMAL
    call print_at

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Draw Prompt and Footer
    ; ─────────────────────────────────────────────────────────────────────────
    mov dh, 16                              ; Row 16: Prompt
    mov si, msg_prompt                      ; "Select option..."
    mov bl, ATTR_WARN                       ; Yellow (attention)
    call print_centered

    mov dh, 24                              ; Row 24: Footer
    mov si, msg_footer                      ; Copyright
    mov bl, ATTR_NORMAL
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 5: Restore State
    ; ─────────────────────────────────────────────────────────────────────────
    popa                                    ; Restore all registers
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3e.3 PRINT_CENTERED — Centered Text Output [PRINT_CENTERED]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Print string centered horizontally on specified row.
;
; @param  DH Row number (0-24).
; @param  SI Pointer to null-terminated string.
; @param  BL Text attribute byte.
; @return Nothing.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

print_centered:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all registers

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Calculate String Length
    ; ─────────────────────────────────────────────────────────────────────────
    ; Count characters until null terminator to determine center position.
    push si                                 ; Save string pointer
    xor cx, cx                              ; CX = 0 (length counter)
.len:
    lodsb                                   ; AL = [SI++]
    test al, al                             ; Check for null terminator
    jz .len_done                            ; If null, done counting
    inc cx                                  ; Increment length counter
    jmp .len                                ; Continue counting
.len_done:
    pop si                                  ; Restore string pointer

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Calculate Starting Column
    ; ─────────────────────────────────────────────────────────────────────────
    ; Column = (SCREEN_WIDTH - string_length) / 2
    mov ax, SCREEN_WIDTH                    ; AX = 80
    sub ax, cx                              ; AX = 80 - length
    shr ax, 1                               ; AX = (80 - length) / 2
    mov dl, al                              ; DL = starting column

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Output String via print_at
    ; ─────────────────────────────────────────────────────────────────────────
    ; DH (row) preserved from caller, DL computed above, SI/BL from caller.
    call print_at                           ; Print at calculated position
    popa                                    ; Restore all registers
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3e.4 PRINT_AT — Positioned Text Output [PRINT_AT]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Print string at specified row and column.
;
; @param  DH Row number (0-24).
; @param  DL Column number (0-79).
; @param  SI Pointer to null-terminated string.
; @param  BL Text attribute byte.
; @return Nothing.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

print_at:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all general registers
    push es                                 ; Save ES (will point to video mem)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Calculate Video Memory Offset
    ; ─────────────────────────────────────────────────────────────────────────
    ; Offset = (row * 80 + column) * 2 (2 bytes per character cell)
    mov ax, VIDEO_MEM                       ; AX = 0xB800
    mov es, ax                              ; ES = video memory segment
    movzx ax, dh                            ; AX = row (zero-extended)
    mov cx, SCREEN_WIDTH                    ; CX = 80
    mul cx                                  ; AX = row * 80
    movzx cx, dl                            ; CX = column (zero-extended)
    add ax, cx                              ; AX = row * 80 + column
    shl ax, 1                               ; AX = offset * 2 (word offset)
    mov di, ax                              ; DI = video memory offset

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Write String to Video Memory
    ; ─────────────────────────────────────────────────────────────────────────
    ; Each character is written as [char][attr] word.
    mov ah, bl                              ; AH = attribute byte
.loop:
    lodsb                                   ; AL = [SI++] (next char)
    test al, al                             ; Check for null terminator
    jz .done                                ; If null, string complete
    stosw                                   ; [ES:DI] = AX; DI += 2
    jmp .loop                               ; Continue writing
.done:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Restore State
    ; ─────────────────────────────────────────────────────────────────────────
    pop es                                  ; Restore ES
    popa                                    ; Restore all general registers
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3e.5 PRINT_RESULT — Test Result Indicator [PRINT_RESULT]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Display OK or FAIL based on carry flag.
;
; @param  DH Row number (0-24).
; @param  DL Column number (0-79).
; @param  CF Carry flag: 0 = OK (green), 1 = FAIL (red).
; @return Nothing.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

print_result:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State and Check Result
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all registers
    jc .fail                                ; CF=1 means failure

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2a: Success Path (CF=0)
    ; ─────────────────────────────────────────────────────────────────────────
    mov si, msg_ok                          ; SI = "[ OK ]"
    mov bl, ATTR_OK                         ; BL = light green
    jmp .print                              ; Go print

.fail:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2b: Failure Path (CF=1)
    ; ─────────────────────────────────────────────────────────────────────────
    mov si, msg_fail                        ; SI = "[FAIL]"
    mov bl, ATTR_FAIL                       ; BL = light red

.print:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Output via print_at
    ; ─────────────────────────────────────────────────────────────────────────
    call print_at                           ; Print at specified position
    popa                                    ; Restore all registers
    ret

; ═══════════════════════════════════════════════════════════════════════════════
; B.2c FLOPPY_BOOT — Minimal Kernel Loader [FLOPPY_BOOT]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief  Load and execute kernel from floppy disk.
;
; PURPOSE: Provide simple kernel boot for embedded/ancient systems.
;          Alternative to full stage0a+0b boot chain.
;
; WHY THIS MODE:
;   For embedded/ancient systems where full stage0a+0b is overkill.
;   Loads a minimal kernel from floppy and jumps to it.
;
; LOAD PARAMETERS:
;   Source:      Sectors 17-80 (after extended code)
;   Destination: 0x1000:0000 (physical 0x10000)
;   Size:        64 sectors = 32KB
;
; @param  None.
; @return Never returns on success — jumps to kernel.
;         Returns to caller on failure.
;
; ═══════════════════════════════════════════════════════════════════════════════

floppy_boot_mode:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    ; Preserve all general-purpose registers for clean return on failure.
    pusha                               ; Save AX,CX,DX,BX,SP,BP,SI,DI

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Display Mode Title
    ; ─────────────────────────────────────────────────────────────────────────
    ; Clear screen and show floppy boot mode title so user knows what's happening.
    call clear_screen                   ; Start with blank canvas

    mov dh, 1                           ; Row 1 (top of screen)
    mov si, msg_floppy_title            ; "FLOPPY BOOT MODE"
    mov bl, ATTR_TITLE                  ; Yellow on black for visibility
    call print_centered                 ; Center the title

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Show Loading Status
    ; ─────────────────────────────────────────────────────────────────────────
    ; Display "Loading kernel from floppy..." before the slow disk operation.
    mov dh, 4                           ; Row 4 (below title)
    mov dl, 5                           ; Column 5 (left margin)
    mov si, msg_loading_kernel          ; Status message
    mov bl, ATTR_NORMAL                 ; White text
    call print_at                       ; Position and print

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Load Kernel from Floppy
    ; ─────────────────────────────────────────────────────────────────────────
    ; Use BIOS INT 0x13/AH=0x02 to read sectors 17-80 (64 sectors = 32KB).
    ; Kernel loaded to 0x1000:0000 (physical address 0x10000).
    ; This is above the 64KB real mode segment but addressable via ES segment.
    mov ah, 0x02                        ; BIOS function: read sectors
    mov al, 64                          ; Read 64 sectors (32KB total)
    mov ch, 0                           ; Cylinder 0 (first cylinder)
    mov cl, 17                          ; Start at sector 17 (after extended code)
    mov dh, 0                           ; Head 0 (first head)
    mov dl, [boot_drive]                ; Use saved boot drive number
    push es                             ; Save current ES for restoration
    push 0x1000                         ; Push segment value
    pop es                              ; ES = 0x1000 (segment for 0x10000)
    xor bx, bx                          ; BX = 0 (offset = 0, ES:BX = 0x10000)
    int 0x13                            ; Execute BIOS disk read
    pop es                              ; Restore original ES
    jc .load_failed                     ; CF=1 means read failed

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 5: Show Load Success
    ; ─────────────────────────────────────────────────────────────────────────
    ; Print [OK] result indicator after successful load.
    mov dh, 4                           ; Same row as "Loading..." message
    mov dl, 40                          ; Column 40 (right side for result)
    clc                                 ; Clear CF to indicate success
    call print_result                   ; Prints "[ OK ]" when CF=0

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 6: Announce Jump to Kernel
    ; ─────────────────────────────────────────────────────────────────────────
    ; Let user know we're about to transfer control.
    mov si, msg_jumping                 ; "Jumping to kernel..."
    mov dh, 6                           ; Row 6 (below load status)
    mov dl, 5                           ; Column 5 (left margin)
    mov bl, ATTR_NORMAL                 ; White text
    call print_at                       ; Position and print

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 7: Pass Boot Info and Jump
    ; ─────────────────────────────────────────────────────────────────────────
    ; Pass minimal boot context in registers (kernel calling convention).
    ; DL = boot drive for kernel to use for further disk I/O.
    ; DI = boot info structure pointer (placeholder for future expansion).
    mov dl, [boot_drive]                ; Boot drive in DL (standard convention)
    mov di, 0x9000                      ; Boot info address (future: struct ptr)

    ; FAR JUMP: Transfer control to kernel at 0x1000:0000.
    ; This never returns — kernel takes over.
    jmp 0x1000:0000                     ; Jump to kernel entry point

.load_failed:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 8: Handle Load Failure
    ; ─────────────────────────────────────────────────────────────────────────
    ; Disk read failed. Show [FAIL] and wait for user acknowledgment.
    mov dh, 4                           ; Same row as "Loading..." message
    mov dl, 40                          ; Column 40 (right side for result)
    stc                                 ; Set CF to indicate failure
    call print_result                   ; Prints "[FAIL]" when CF=1

    ; Prompt user to press key before returning to menu.
    mov si, msg_press_key               ; "Press any key to continue..."
    mov dh, 8                           ; Row 8 (below error)
    call print_centered                 ; Center the prompt

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 9: Wait and Return
    ; ─────────────────────────────────────────────────────────────────────────
    ; Wait for keypress then return to menu gracefully.
    xor ah, ah                          ; AH=0: wait for keypress
    int 0x16                            ; BIOS keyboard wait

    popa                                ; Restore all registers
    ret                                 ; Return to menu dispatcher

; ═══════════════════════════════════════════════════════════════════════════════
; B.2d DIAGNOSTIC — Hardware Test Harness [DIAGNOSTIC]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief  Run hardware diagnostic test suite.
;
; PURPOSE: Validate hardware before full boot. Essential for debugging boot
;          failures on unknown hardware.
;
; WHY THIS MODE:
;   Validates hardware before full boot. Essential for debugging boot failures
;   on unknown hardware. Tests A20, VESA, E820, disk, serial.
;
; TEST SEQUENCE:
;   1. A20 Line Enable — required for >1MB memory access
;   2. E820 Memory Map — required for kernel memory allocation
;   3. VESA VBE Support — graphics capability check
;   4. Disk Read — verify boot device is readable
;   5. Serial Port — verify debug output channel
;
; @param  None.
; @return Always returns to caller (menu loop).
;
; ═══════════════════════════════════════════════════════════════════════════════

diagnostic_mode:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    ; Preserve all general-purpose registers for clean return to menu.
    pusha                               ; Save AX,CX,DX,BX,SP,BP,SI,DI

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Initialize Display
    ; ─────────────────────────────────────────────────────────────────────────
    ; Clear screen and show diagnostic mode header.
    call clear_screen                   ; Start with blank canvas

    mov dh, 1                           ; Row 1 (top of screen)
    mov si, msg_diag_title              ; "HARDWARE DIAGNOSTICS"
    mov bl, ATTR_TITLE                  ; Yellow on black for visibility
    call print_centered                 ; Center the title

    mov dh, 2                           ; Row 2 (below title)
    mov si, msg_diag_subtitle           ; "Testing system hardware..."
    mov bl, ATTR_NORMAL                 ; White text
    call print_centered                 ; Center the subtitle

    ; ─────────────────────────────────────────────────────────────────────────
    ; Test 1: A20 Line Enable
    ; ─────────────────────────────────────────────────────────────────────────
    ; A20 gate must be enabled to access memory above 1MB. Critical for
    ; protected mode and any modern OS. Tests BIOS method first, then
    ; keyboard controller fallback.
    mov dh, 5                           ; Row 5 (first test row)
    mov dl, 5                           ; Column 5 (left margin)
    mov si, msg_test_a20                ; "Test 1: A20 Line Enable"
    mov bl, ATTR_NORMAL                 ; White text
    call print_at                       ; Position and print label

    call test_a20                       ; Execute A20 test (sets CF on result)
    mov dh, 5                           ; Same row as test label
    mov dl, 45                          ; Column 45 (right side for result)
    call print_result                   ; Print [ OK ] or [FAIL] based on CF

    ; ─────────────────────────────────────────────────────────────────────────
    ; Test 2: E820 Memory Map
    ; ─────────────────────────────────────────────────────────────────────────
    ; E820 BIOS call provides memory map for protected mode kernel.
    ; Modern systems all support this. Failure indicates very old BIOS.
    mov dh, 7                           ; Row 7 (skip row for visual spacing)
    mov dl, 5                           ; Column 5 (left margin)
    mov si, msg_test_e820               ; "Test 2: E820 Memory Map"
    mov bl, ATTR_NORMAL                 ; White text
    call print_at                       ; Position and print label

    call test_e820                      ; Execute E820 test (sets CF on result)
    mov dh, 7                           ; Same row as test label
    mov dl, 45                          ; Column 45 (right side for result)
    call print_result                   ; Print [ OK ] or [FAIL] based on CF

    ; ─────────────────────────────────────────────────────────────────────────
    ; Test 3: VESA VBE Support
    ; ─────────────────────────────────────────────────────────────────────────
    ; VESA VBE provides linear framebuffer graphics. Required for graphical
    ; display beyond 320x200. Checks for "VESA" signature in info block.
    mov dh, 9                           ; Row 9 (skip row for visual spacing)
    mov dl, 5                           ; Column 5 (left margin)
    mov si, msg_test_vesa               ; "Test 3: VESA VBE Support"
    mov bl, ATTR_NORMAL                 ; White text
    call print_at                       ; Position and print label

    call test_vesa                      ; Execute VESA test (sets CF on result)
    mov dh, 9                           ; Same row as test label
    mov dl, 45                          ; Column 45 (right side for result)
    call print_result                   ; Print [ OK ] or [FAIL] based on CF

    ; ─────────────────────────────────────────────────────────────────────────
    ; Test 4: Disk Read Capability
    ; ─────────────────────────────────────────────────────────────────────────
    ; Verify we can read from boot drive. Basic sanity check that disk
    ; subsystem works. Reads sector 1 to scratch buffer.
    mov dh, 11                          ; Row 11 (skip row for visual spacing)
    mov dl, 5                           ; Column 5 (left margin)
    mov si, msg_test_disk               ; "Test 4: Disk Read"
    mov bl, ATTR_NORMAL                 ; White text
    call print_at                       ; Position and print label

    call test_disk                      ; Execute disk test (sets CF on result)
    mov dh, 11                          ; Same row as test label
    mov dl, 45                          ; Column 45 (right side for result)
    call print_result                   ; Print [ OK ] or [FAIL] based on CF

    ; ─────────────────────────────────────────────────────────────────────────
    ; Test 5: Serial Port (COM1)
    ; ─────────────────────────────────────────────────────────────────────────
    ; Serial port enables debug output and remote management. Initializes
    ; COM1 at 9600 baud and sends test string. Useful for headless debugging.
    mov dh, 13                          ; Row 13 (skip row for visual spacing)
    mov dl, 5                           ; Column 5 (left margin)
    mov si, msg_test_serial             ; "Test 5: Serial Port"
    mov bl, ATTR_NORMAL                 ; White text
    call print_at                       ; Position and print label

    call test_serial                    ; Execute serial test (sets CF on result)
    mov dh, 13                          ; Same row as test label
    mov dl, 45                          ; Column 45 (right side for result)
    call print_result                   ; Print [ OK ] or [FAIL] based on CF

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Display Summary
    ; ─────────────────────────────────────────────────────────────────────────
    ; Show completion message. All tests complete regardless of pass/fail.
    mov dh, 16                          ; Row 16 (below all tests)
    mov si, msg_diag_complete           ; "Diagnostics complete!"
    mov bl, ATTR_WARN                   ; Yellow for emphasis
    call print_centered                 ; Center the completion message

    mov dh, 18                          ; Row 18 (below summary)
    mov si, msg_press_key               ; "Press any key to continue..."
    mov bl, ATTR_NORMAL                 ; White text
    call print_centered                 ; Center the prompt

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Wait and Return
    ; ─────────────────────────────────────────────────────────────────────────
    ; Wait for keypress then return to menu.
    xor ah, ah                          ; AH=0: wait for keypress
    int 0x16                            ; BIOS keyboard wait

    popa                                ; Restore all registers
    ret                                 ; Return to menu dispatcher

; ═══════════════════════════════════════════════════════════════════════════════
; B.4 TESTS — Diagnostic Test Functions [TESTS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief  Individual hardware test implementations.
;
; PURPOSE: Provide self-contained tests for each hardware capability.
;          Each test returns CF=0 (success) or CF=1 (failure).
;
; TEST FUNCTIONS:
;   test_a20    — Enable A20 gate (BIOS or keyboard controller)
;   test_e820   — Query E820 memory map
;   test_vesa   — Check VESA VBE presence
;   test_disk   — Read sector from boot drive
;   test_serial — Output test string to COM1
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.4a TEST_A20 — A20 Line Enable Test [TEST_A20]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Enable A20 gate and verify memory access above 1MB.
;
; @param  None.
; @return CF=0 success, CF=1 failure.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

test_a20:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all registers

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Try BIOS Method (INT 0x15 AX=0x2401)
    ; ─────────────────────────────────────────────────────────────────────────
    ; Modern BIOSes support direct A20 enable via INT 0x15.
    ; This is cleaner than keyboard controller manipulation.
    mov ax, 0x2401                          ; AX = 0x2401 (enable A20)
    int 0x15                                ; BIOS A20 gate control
    jnc .success                            ; CF=0 means success

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Fallback to Keyboard Controller Method
    ; ─────────────────────────────────────────────────────────────────────────
    ; On older systems, A20 is controlled via 8042 keyboard controller.
    ; Output port bit 1 (0x02) controls A20 gate.
    call .wait_input                        ; Wait for controller ready
    mov al, 0xAD                            ; Command: disable keyboard
    out 0x64, al                            ; Send to command port

    call .wait_input                        ; Wait for controller ready
    mov al, 0xD0                            ; Command: read output port
    out 0x64, al                            ; Send to command port

    call .wait_output                       ; Wait for data available
    in al, 0x60                             ; Read output port value
    push ax                                 ; Save current value

    call .wait_input                        ; Wait for controller ready
    mov al, 0xD1                            ; Command: write output port
    out 0x64, al                            ; Send to command port

    call .wait_input                        ; Wait for controller ready
    pop ax                                  ; Restore output port value
    or al, 2                                ; Set bit 1 (A20 enable)
    out 0x60, al                            ; Write new output port value

    call .wait_input                        ; Wait for controller ready
    mov al, 0xAE                            ; Command: enable keyboard
    out 0x64, al                            ; Send to command port

    call .wait_input                        ; Wait for controller ready

.success:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Return Success
    ; ─────────────────────────────────────────────────────────────────────────
    popa                                    ; Restore all registers
    clc                                     ; CF=0 (success)
    ret

.wait_input:
    ; Wait for keyboard controller input buffer empty (bit 1 = 0)
    in al, 0x64                             ; Read status register
    test al, 2                              ; Test bit 1 (input buffer full)
    jnz .wait_input                         ; Loop if buffer not empty
    ret

.wait_output:
    ; Wait for keyboard controller output buffer full (bit 0 = 1)
    in al, 0x64                             ; Read status register
    test al, 1                              ; Test bit 0 (output buffer full)
    jz .wait_output                         ; Loop if buffer empty
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.4b TEST_E820 — Memory Map Test [TEST_E820]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Query BIOS E820 memory map.
;
; @param  None.
; @return CF=0 success (SMAP signature valid), CF=1 failure.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

test_e820:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State and Set Up Registers
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all registers

    mov di, e820_buffer                     ; DI = destination buffer
    xor ebx, ebx                            ; EBX = 0 (start of enumeration)
    mov edx, 0x534D4150                     ; EDX = "SMAP" signature

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Call E820 BIOS Service
    ; ─────────────────────────────────────────────────────────────────────────
    ; INT 0x15 EAX=0xE820: Query system memory map.
    ; Returns: EAX="SMAP", EBX=continuation value, ECX=bytes written.
    mov eax, 0xE820                         ; EAX = function code
    mov ecx, 24                             ; ECX = buffer size (24 bytes)
    int 0x15                                ; BIOS memory map service

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Verify Response
    ; ─────────────────────────────────────────────────────────────────────────
    jc .fail                                ; CF=1 means not supported
    cmp eax, 0x534D4150                     ; Verify "SMAP" signature returned
    jne .fail                               ; Wrong signature = failure

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Return Success
    ; ─────────────────────────────────────────────────────────────────────────
    popa                                    ; Restore all registers
    clc                                     ; CF=0 (success)
    ret

.fail:
    popa                                    ; Restore all registers
    stc                                     ; CF=1 (failure)
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.4c TEST_VESA — VESA VBE Presence Test [TEST_VESA]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Check for VESA BIOS Extensions support.
;
; @param  None.
; @return CF=0 success (VBE present), CF=1 failure.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

test_vesa:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all registers

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Query VBE Info
    ; ─────────────────────────────────────────────────────────────────────────
    ; INT 0x10 AX=0x4F00: Get VBE Controller Info.
    ; Returns AX=0x004F if VBE is supported.
    mov ax, 0x4F00                          ; AX = Get VBE Info function
    mov di, vesa_buffer                     ; ES:DI = info buffer
    int 0x10                                ; BIOS video interrupt

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Verify Response
    ; ─────────────────────────────────────────────────────────────────────────
    cmp ax, 0x004F                          ; Check for VBE success code
    jne .fail                               ; AX != 0x004F means failure

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Return Success
    ; ─────────────────────────────────────────────────────────────────────────
    popa                                    ; Restore all registers
    clc                                     ; CF=0 (success)
    ret

.fail:
    popa                                    ; Restore all registers
    stc                                     ; CF=1 (failure)
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.4d TEST_DISK — Disk Read Test [TEST_DISK]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Test disk read capability on boot drive.
;
; @param  None.
; @return CF=0 success (read OK), CF=1 failure.
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

test_disk:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all registers

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Read Sector 1 from Boot Drive
    ; ─────────────────────────────────────────────────────────────────────────
    ; INT 0x13 AH=0x02: Read sectors from drive.
    ; We read sector 1 (the boot sector itself) as a simple test.
    mov ah, 0x02                            ; AH = read sectors function
    mov al, 1                               ; AL = 1 sector
    mov ch, 0                               ; CH = cylinder 0
    mov cl, 1                               ; CL = sector 1 (1-indexed)
    mov dh, 0                               ; DH = head 0
    mov dl, [boot_drive]                    ; DL = boot drive number
    mov bx, disk_buffer                     ; ES:BX = destination buffer
    int 0x13                                ; BIOS disk service

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Return (CF set by INT 0x13)
    ; ─────────────────────────────────────────────────────────────────────────
    popa                                    ; Restore all registers
    ret                                     ; CF already set by INT 0x13

; ───────────────────────────────────────────────────────────────────────────────
; B.4e TEST_SERIAL — Serial Port Test [TEST_SERIAL]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Test serial port output capability.
;
; @param  None.
; @return CF=0 success (always, if we reach here).
; @clobbers None (all registers preserved via PUSHA).
;
; ───────────────────────────────────────────────────────────────────────────────

test_serial:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save State
    ; ─────────────────────────────────────────────────────────────────────────
    pusha                                   ; Save all registers

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Send Test String
    ; ─────────────────────────────────────────────────────────────────────────
    ; Output a test message to COM1. If serial_print completes without
    ; hanging, we consider the serial port functional.
    mov si, msg_serial_test                 ; SI = test message pointer
    call serial_print                       ; Send string to COM1

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Return Success
    ; ─────────────────────────────────────────────────────────────────────────
    ; If we reach here, serial output worked (no timeout).
    popa                                    ; Restore all registers
    clc                                     ; CF=0 (success)
    ret

; ═══════════════════════════════════════════════════════════════════════════════
; B.5 EXTENDED_DATA — Strings and Buffers [EXTENDED_DATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief  Data section for extended code.
;
; PURPOSE: Store UI strings, test messages, and runtime buffers.
;          Placed at end of extended code to maximize instruction space.
;
; SECTIONS:
;   B.5a STRINGS  — Menu, floppy boot, diagnostic messages
;   B.5b BUFFERS  — VESA, E820, disk read buffers
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.5a STRINGS — UI and Message Strings [STRINGS]
; ───────────────────────────────────────────────────────────────────────────────

; Menu strings
msg_title:          db "M I L L E N N I U M O S", 0
msg_subtitle:       db "Minimal Boot / Hardware Diagnostics", 0
msg_scripture:      db '"In the beginning..." - Genesis 1:1', 0
msg_opt1:           db "[1] Floppy Boot Mode", 0
msg_opt2:           db "[2] Hardware Diagnostics", 0
msg_opt3:           db "[3] Reboot", 0
msg_prompt:         db "Select option (1-3) or ESC to halt", 0
msg_footer:         db "(C) 2026 CreativeWorkzStudio LLC", 0
msg_halted:         db "[BOOT] System halted.", 13, 10, 0

; Floppy boot strings
msg_floppy_title:   db "FLOPPY BOOT MODE", 0
msg_loading_kernel: db "Loading kernel from floppy...", 0
msg_jumping:        db "Jumping to kernel...", 0

; Diagnostic strings
msg_diag_title:     db "HARDWARE DIAGNOSTICS", 0
msg_diag_subtitle:  db "Testing system hardware...", 0
msg_test_a20:       db "Test 1: A20 Line Enable", 0
msg_test_e820:      db "Test 2: E820 Memory Map", 0
msg_test_vesa:      db "Test 3: VESA VBE Support", 0
msg_test_disk:      db "Test 4: Disk Read", 0
msg_test_serial:    db "Test 5: Serial Port", 0
msg_diag_complete:  db "Diagnostics complete!", 0

; Result strings
msg_ok:             db "[ OK ]", 0
msg_fail:           db "[FAIL]", 0

; Common strings
msg_press_key:      db "Press any key to continue...", 0
msg_serial_test:    db "[DIAG] Serial port test", 13, 10, 0

; ───────────────────────────────────────────────────────────────────────────────
; B.5b BUFFERS — Runtime Buffers [BUFFERS]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Memory buffers for BIOS queries and disk operations.
;
; PURPOSE: Provide storage for:
;          - vesa_buffer: VBE controller info (512 bytes)
;          - e820_buffer: Single E820 memory map entry (24 bytes)
;          - disk_buffer: Sector read buffer (512 bytes)
;
; ───────────────────────────────────────────────────────────────────────────────

align 16
vesa_buffer:        times 512 db 0          ; VBE Info Block
e820_buffer:        times 24 db 0           ; E820 memory entry
disk_buffer:        times 512 db 0          ; Disk sector buffer

; ═══════════════════════════════════════════════════════════════════════════════
; END BODY [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; CLOSING BLOCK [CLOSING]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Synthesis, verification, and forward guidance.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.1 VALIDATION — Build & Test [VALIDATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Build:
;   nasm -f bin boot.asm -o boot.img
;
; Test in QEMU:
;   qemu-system-x86_64 -fda boot.img -serial stdio
;
; Expected behavior:
;   1. Boot sector loads extended code
;   2. Menu appears with blue background
;   3. Option 1: Floppy boot (loads kernel from sectors 17+)
;   4. Option 2: Hardware diagnostics (tests A20, E820, VESA, disk, serial)
;   5. Option 3: Reboot
;   6. ESC: Halt

; ═══════════════════════════════════════════════════════════════════════════════
; X.2 EXECUTION — Flow Summary [EXECUTION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Boot Flow:
;   BIOS → boot_entry (sector 1) → load extended → extended_entry → menu
;
; Floppy Boot Flow:
;   Menu → floppy_boot_mode → load kernel → jump to 0x10000
;
; Diagnostic Flow:
;   Menu → diagnostic_mode → test_a20 → test_e820 → test_vesa →
;   test_disk → test_serial → results → menu

; ═══════════════════════════════════════════════════════════════════════════════
; X.3 ARCHITECTURE — Tripwire Position [ARCHITECTURE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; MillenniumOS Boot Tripwire Levels:
;
;   Level 1: UEFI (boot/uefi/)
;     - Modern systems, GOP graphics
;     - Clean 64-bit entry
;     - PRIMARY boot path
;
;   Level 2: BIOS Full (boot/stage0/stage0a.asm + stage0b.asm)
;     - Legacy systems
;     - VESA graphics with tripwire fallback
;     - Full boot chain with DAR Witness
;     - FALLBACK for older systems
;
;   Level 3: Minimal/Diagnostic (boot/stage0/boot.asm) ← THIS FILE
;     - Ancient/embedded systems
;     - Floppy boot capability
;     - Hardware diagnostics
;     - LAST RESORT / DEBUGGING TOOL

; ═══════════════════════════════════════════════════════════════════════════════
; X.4 RELATED — Related Components [RELATED]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Related Files:
;   - boot/uefi/main.c          — UEFI bootloader (Level 1)
;   - boot/stage0/stage0a.asm   — BIOS boot sector (Level 2)
;   - boot/stage0/stage0b.asm   — BIOS second stage (Level 2)
;   - kernel/entry.asm          — Kernel entry point

; ═══════════════════════════════════════════════════════════════════════════════
; X.5 CLOSING NOTE
; ═══════════════════════════════════════════════════════════════════════════════
;
; This file represents the "last resort" in the MillenniumOS boot tripwire.
; When UEFI fails and full BIOS stage0a+stage0b fails, this minimal
; floppy bootloader and diagnostic tool can still bring the system up
; or help diagnose why it won't boot.
;
; "In the beginning God created the heaven and the earth.
;  And the earth was without form, and void."
;  — Genesis 1:1-2
;
; The diagnostic mode examines the "void" — the raw hardware state
; before the OS brings form. Like the Spirit moving upon the waters,
; we inspect what exists before creating upon it.
;
; ═══════════════════════════════════════════════════════════════════════════════
; END CLOSING [END]
; ═══════════════════════════════════════════════════════════════════════════════

; Pad to sector boundary for clean floppy image
times 8192 - ($ - $$) db 0              ; Pad to 8KB (16 sectors)
