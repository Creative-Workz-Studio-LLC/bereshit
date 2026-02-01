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
; @omni:req  key        = MOS-boot-stage0b
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
; @omni:ins  component  = stage0b
; @omni:ins  layer      = boot/stage0
; @omni:ins  includes   = none (self-contained second stage)
; @omni:ins  provides   = [BOOT_SELECTOR, A20_ENABLE, VESA_SETUP, MEMORY_MAP,
;                          KERNEL_LOADER, PROTECTED_MODE, LONG_MODE, DAR_WITNESS]
;
; ───────────────────────────────────────────────────────────────────────────────
; P.4 ARCHITECTURE — Scaling Context [ARCH]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  layers     = [0:boot]
; @omni:ins  scale      = 16KB (32 sectors)
; @omni:ins  pattern    = multi-stage-boot (0a → 0b → kernel)
; @omni:ins  anchor     = genesis_1_2_3 → "Spirit moved... Let there be light"
;
; ───────────────────────────────────────────────────────────────────────────────
; P.5 SUMMARY — Human Identity [SUMMARY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:req  title      = MillenniumOS Stage 0b - Boot Selector & Loader
; @omni:req  brief      = Preparation before the kernel — menu, A20, VESA, memory
;                         map, kernel load, protected mode, long mode transition.
;
; ═══════════════════════════════════════════════════════════════════════════════
; END PRAGMA [END]
; ═══════════════════════════════════════════════════════════════════════════════
;
; DEPENDENCY CLASSIFICATION: [DEPENDED]
;   - Needs: Stage 0a loads this to 0x7E00
;   - Needs: BIOS INT 0x10 (video), INT 0x13 (disk), INT 0x15 (memory)
;   - Provides: Kernel loaded to 0x100000, CPU in 64-bit long mode
;
; ═══════════════════════════════════════════════════════════════════════════════
; METADATA BLOCK [METADATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Identity and context for Stage 0b bootloader.
;
; Stage 0b is the "heavy lifter" of the boot process — it:
;   1. Shows the boot selector menu (user choice)
;   2. Enables A20 line for full memory access
;   3. Sets VESA video mode with tripwire fallback
;   4. Acquires memory map from BIOS (E820)
;   5. Loads 448KB kernel in 7 chunks
;   6. Transitions: Real Mode → Protected Mode → Long Mode
;   7. Jumps to kernel at 0x100000
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; M.1 IDENTITY — Core Identity [IDENTITY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @file      stage0b.asm
; @brief     Boot Selector & Loader — prepares system for kernel execution.
;
; Key:       MOS-boot-stage0b
; Title:     MillenniumOS Stage 0b - Boot Selector & Loader
; Type:      Source (Bootloader)
; Component: Ladder — second rung, builds on Stage 0a foundation
; Role:      User menu + CPU mode transitions + kernel loading

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
; Path:      millenniumos/boot/stage0/stage0b.asm

; ═══════════════════════════════════════════════════════════════════════════════
; M.5 DERIVATION — Template Lineage [DERIVATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Derives:   bereshit/word/seed/code/asm/source.asm (template)

; ═══════════════════════════════════════════════════════════════════════════════
; M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Tags:      [asm, nasm, boot, x86, x86-64, real-mode, protected-mode,
;             long-mode, stage0b, vesa, a20, memory-map, kernel-loader]

; ═══════════════════════════════════════════════════════════════════════════════
; M.7 INTENT — Purpose Statement [INTENT]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Purpose:   Bridge the gap between Stage 0a (512-byte boot sector) and the
;            kernel (64-bit long mode). This stage handles all the complex
;            CPU mode transitions, hardware initialization, and user interface.
;
; Core Design: Chain-loader with UI + mode transitions.
;              Genesis 1:2-3 principle: "Spirit moved... Let there be light"
;              The preparation (Spirit moving) before creation (light/kernel).
;
; Key Features:
;   - Boot selector menu (user control over boot process)
;   - A20 line enable (BIOS or keyboard controller)
;   - VESA video mode with tripwire fallback chain
;   - E820 memory map acquisition
;   - Multi-chunk kernel loading (7 × 64KB = 448KB)
;   - Protected mode transition (GDT, CR0.PE)
;   - Long mode transition (PAE, PML4, CR4, EFER.LME)
;   - DAR Witness event buffer for boot forensics

; ═══════════════════════════════════════════════════════════════════════════════
; M.8 GROUNDING — Biblical Foundation [GROUNDING]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Scripture: Genesis 1:2-3 — "And the Spirit of God moved upon the face of
;            the waters. And God said, Let there be light: and there was light."
; Principle: Stage 0b is the Spirit moving — the preparation before the kernel
;            (light) appears. The boot selector is the moment of choice before
;            creation proceeds. The CPU mode transitions are the waters being
;            separated (void → form → function).

; ═══════════════════════════════════════════════════════════════════════════════
; M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; CPU Features:
;   - 8086/Real Mode (16-bit) — entry point
;   - 386+ Protected Mode (32-bit) — intermediate
;   - x86-64 Long Mode (64-bit) — final state
;   - BIOS INT 0x10 (video), 0x13 (disk), 0x15 (memory, A20), 0x16 (keyboard)
;
; External Symbols:
;   - None (self-contained)
;
; Memory Layout:
;   - Loaded at 0x7E00 by Stage 0a
;   - Kernel loaded to 0x10000-0x8FFFF (low memory)
;   - Page tables at 0x70000 (24KB reserved)
;   - Boot info at 0x9000
;   - Kernel copied to 0x100000 (1MB) by protected mode
;
; Used by:
;   - Stage 0a jumps here
;   - Kernel expects boot info at 0x9000, entry at 0x100000
;
; Assemble:
;   nasm -f bin stage0b.asm -o stage0b.bin
;
; Formats:
;   - bin: Flat binary (chain-loaded, no ELF headers)

; ═══════════════════════════════════════════════════════════════════════════════
; M.10 ROADMAP — Version History [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; History:
;   a-01.00 (2026-01-30) — Initial creation with boot selector, VESA, long mode
;   a-01.01 (2026-01-30) — DAR Witness boot event buffer integration
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
;   - S.2 CONSTANTS  — EQU definitions organized by category
;   - S.3 EXTERNAL   — Not used (self-contained)
;   - S.4 DATA       — Deferred to DATA SECTION after BODY
;   - S.5 BSS        — Not used (flat binary)
;   - S.6 DEBUG      — DAR Witness infrastructure (constants here, impl in BODY)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; S.1 DIRECTIVES — Assembler Configuration [DIRECTIVES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Configuration telling NASM how to assemble this file.
;
; NOTE: Stage 0b contains code for 16-bit, 32-bit, AND 64-bit modes.
;       Mode is switched via `bits 16/32/64` directives at appropriate points.
;       Initial entry is 16-bit real mode from Stage 0a.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.1a MODE — CPU Bit Mode [MODE]
; ───────────────────────────────────────────────────────────────────────────────
;
; Real Mode (16-bit) — Stage 0a hands off in real mode.
; Mode changes occur in BODY: real → protected → long.

bits 16

; ───────────────────────────────────────────────────────────────────────────────
; S.1b ORIGIN — Load Address [ORIGIN]
; ───────────────────────────────────────────────────────────────────────────────
;
; Stage 0b origin 0x7E00 — loaded by Stage 0a immediately after boot sector.
; Boot sector (0x7C00) + 512 bytes = 0x7E00.

org 0x7E00

; ═══════════════════════════════════════════════════════════════════════════════
; S.2 CONSTANTS — EQU Definitions [CONSTANTS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Named compile-time values — no memory used at runtime.
;
; PURPOSE: Define boot-critical constants organized by category.
;
; CONTAINS:
;   - S.2a ADDRESSES — Memory layout constants
;   - S.2b KERNEL    — Kernel loading parameters
;   - S.2c VIDEO     — VESA modes and VGA attributes
;   - S.2d MEMORY    — Memory map constants
;   - S.2e DAR       — DAR Witness boot event buffer
;   - S.2f TERNARY   — Balanced ternary semantic values
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.2a ADDRESSES — Memory Layout Constants [ADDRESSES]
; ───────────────────────────────────────────────────────────────────────────────

; KERNEL_LOAD_ADDR — Final kernel location.
;
; @brief Kernel is loaded in chunks to low memory, then copied here.
;        1MB is the standard "high memory" boundary.
;
; @value 0x100000 — 1MB physical address.
KERNEL_LOAD_ADDR    equ 0x100000

; BOOT_INFO_ADDR — Boot information structure location.
;
; @brief MosBootInfo structure passed to kernel in RDI.
;        Contains framebuffer, memory map, kernel address, etc.
;
; @value 0x9000 — Below EBDA, above stack danger zone.
BOOT_INFO_ADDR      equ 0x9000

; ───────────────────────────────────────────────────────────────────────────────
; S.2b KERNEL — Kernel Loading Parameters [KERNEL]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Kernel loading strategy: 7 × 64KB chunks via BIOS INT 0x13.
;
; Memory Layout for Loading (below 1MB):
;   0x10000-0x1FFFF: Chunk 1 (64KB) — First kernel segment
;   0x20000-0x2FFFF: Chunk 2 (64KB)
;   0x30000-0x3FFFF: Chunk 3 (64KB)
;   0x40000-0x4FFFF: Chunk 4 (64KB)
;   0x50000-0x5FFFF: Chunk 5 (64KB)
;   0x60000-0x6FFFF: Chunk 6 (64KB) — Last before page tables
;   0x70000-0x75FFF: PAGE TABLES    — *** RESERVED ***
;   0x80000-0x8FFFF: Chunk 7 (64KB) — After page tables
;
; Total: 7 × 64KB = 448KB kernel capacity via BIOS INT 0x13.
; After boot, kernel can load additional modules via PIO disk access.
;

; KERNEL_SECTORS_N — Sector count per chunk.
;
; @brief Each chunk is 64KB = 128 sectors (128 × 512 = 65536).
KERNEL_SECTORS_1    equ 128             ; Chunk 1: 64KB @ 0x10000
KERNEL_SECTORS_2    equ 128             ; Chunk 2: 64KB @ 0x20000
KERNEL_SECTORS_3    equ 128             ; Chunk 3: 64KB @ 0x30000
KERNEL_SECTORS_4    equ 128             ; Chunk 4: 64KB @ 0x40000
KERNEL_SECTORS_5    equ 128             ; Chunk 5: 64KB @ 0x50000
KERNEL_SECTORS_6    equ 128             ; Chunk 6: 64KB @ 0x60000
KERNEL_SECTORS_7    equ 128             ; Chunk 7: 64KB @ 0x80000

; KERNEL_CHUNK_COUNT — Number of chunks to load.
;
; @brief 7 chunks × 64KB = 448KB kernel.
KERNEL_CHUNK_COUNT  equ 7

; KERNEL_PHASE1_SIZE — Total bytes loaded via BIOS.
;
; @brief 448KB = 458,752 bytes.
KERNEL_PHASE1_SIZE  equ 448 * 1024

; KERNEL_MAX_SIZE — Maximum supported kernel size.
;
; @brief 1MB limit. Future: PIO extends beyond BIOS.
KERNEL_MAX_SIZE     equ 1024 * 1024

; KERNEL_TOTAL_SECTORS — Total sectors for phase 1 loading.
;
; @brief 448KB ÷ 512 = 896 sectors.
KERNEL_TOTAL_SECTORS equ 896

; ───────────────────────────────────────────────────────────────────────────────
; S.2c VIDEO — VESA Modes and VGA Attributes [VIDEO]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Video mode constants for tripwire fallback chain.
;
; Fallback Chain: 1024x768 → 800x600 → 640x480 → VGA Text
; Each mode tried in sequence until one succeeds.
;

; VESA Mode Numbers — Standard VESA mode codes.
;
; @brief VESA VBE mode numbers with 32-bit color depth.
VESA_MODE_1024x768  equ 0x118           ; 1024x768x32 (preferred)
VESA_MODE_800x600   equ 0x115           ; 800x600x32 (fallback 1)
VESA_MODE_640x480   equ 0x112           ; 640x480x32 (fallback 2)
VGA_MODE_TEXT       equ 0x03            ; 80x25 text (ultimate fallback)

; VGA Text Mode Attributes — Color byte format: background[7:4] | foreground[3:0]
;
; @brief Color attributes for boot selector menu.
ATTR_TITLE          equ 0x1E            ; Yellow on blue
ATTR_TEXT           equ 0x1F            ; White on blue
ATTR_SCRIPTURE      equ 0x1A            ; Green on blue
ATTR_INFO           equ 0x17            ; Gray on blue
ATTR_HIGHLIGHT      equ 0x70            ; Black on gray (selected)
ATTR_BORDER         equ 0x1B            ; Cyan on blue

; VGA Text Mode Constants — Screen dimensions and memory.
;
; @brief Standard 80x25 text mode parameters.
VIDEO_MEM           equ 0xB800          ; Text mode video memory segment
SCREEN_WIDTH        equ 80              ; Characters per row
SCREEN_HEIGHT       equ 25              ; Rows on screen

; ───────────────────────────────────────────────────────────────────────────────
; S.2d MEMORY — Memory Map Constants [MEMORY]
; ───────────────────────────────────────────────────────────────────────────────

; MMAP_MAX_ENTRIES — Maximum E820 memory map entries.
;
; @brief 3^5 = 243 (trit5 inspired — fits ternary paradigm).
;        Typical systems have 10-30 entries.
;
; @value 243 — Upper bound for memory map array.
MMAP_MAX_ENTRIES    equ 243

; ───────────────────────────────────────────────────────────────────────────────
; S.2e DAR — Boot Event Buffer Constants [DAR]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief DAR Witness boot event buffer — matches debug.h MosBootEventBuffer.
;
; Structure Layout:
;   Offset  Size  Field
;   0x00    4     magic ("EVNT")
;   0x04    4     count (number of events stored)
;   0x08    4     sequence (global sequence counter)
;   0x0C    4     reserved
;   0x10    512   events[16] (16 events × 32 bytes each)
;
; Total size: 16 + 512 = 528 bytes
;

; BOOT_EVENT_BUFFER — Buffer location.
;
; @brief Placed at end of boot info area.
;
; @value 0x9000 + 0x1E00 = 0xAE00
BOOT_EVENT_BUFFER   equ BOOT_INFO_ADDR + 0x1E00

; BOOT_EVENT_MAGIC — Magic value for buffer validation.
;
; @brief "EVNT" in little-endian ASCII.
;
; @value 0x45564E54
BOOT_EVENT_MAGIC    equ 0x45564E54

; BOOT_EVENT_MAX — Maximum events in buffer.
;
; @brief 16 events × 32 bytes = 512 bytes.
BOOT_EVENT_MAX      equ 16

; BOOT_EVENT_SIZE — Size of each event in bytes.
;
; @brief Matches MosBootEvent structure.
BOOT_EVENT_SIZE     equ 32

; ───────────────────────────────────────────────────────────────────────────────
; S.2f TERNARY — Balanced Ternary Semantic Values [TERNARY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Ternary semantics for DAR phases and severity levels.
;
; These values map to balanced ternary: -1, 0, +1
; Used throughout boot for consistent semantic meaning.
;

; DAR Phases — Detect-Assess-Restore cycle phases.
;
; @brief Maps boot events to DAR phases.
;   -1 = DETECT — Witness the event (observation)
;    0 = ASSESS — Evaluate significance (analysis)
;   +1 = RESTORE — Apply learning (action)
DAR_DETECT          equ -1              ; Witness the event
DAR_ASSESS          equ 0               ; Evaluate significance
DAR_RESTORE         equ 1               ; Apply learning

; Severity Levels — Event importance for triage.
;
; @brief Maps event severity to ternary.
;   -1 = MILD — Informational (normal operation)
;    0 = MEDIUM — Noteworthy (deserves attention)
;   +1 = CRITICAL — Significant (requires action)
SEV_MILD            equ -1              ; Informational
SEV_MEDIUM          equ 0               ; Noteworthy
SEV_CRITICAL        equ 1               ; Significant

; ═══════════════════════════════════════════════════════════════════════════════
; S.3 EXTERNAL — Symbol Declarations [EXTERNAL]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Not used — Stage 0b is self-contained.
;
; Stage 0b is a flat binary chain-loaded by Stage 0a.
; All symbols are internal.
;
; ═══════════════════════════════════════════════════════════════════════════════

; (none — self-contained)

; ═══════════════════════════════════════════════════════════════════════════════
; S.4 DATA — Initialized Data [DATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Deferred to DATA SECTION after BODY.
;
; Stage 0b has extensive data (strings, tables, GDT).
; Placed after BODY code for clarity and to allow code addresses
; to be calculated before data is defined.
;
; ═══════════════════════════════════════════════════════════════════════════════

; (see DATA SECTION after BODY)

; ═══════════════════════════════════════════════════════════════════════════════
; S.5 BSS — Uninitialized Data [BSS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Not used — flat binary has no BSS section.
;
; ═══════════════════════════════════════════════════════════════════════════════

; (none — flat binary)

; ═══════════════════════════════════════════════════════════════════════════════
; S.6 DEBUG — Debug Infrastructure [DEBUG]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief DAR Witness system for boot forensics.
;
; Constants defined above in S.2e DAR.
; Implementation in BODY (store_boot_event, serial_print).
; Events stored in buffer for kernel to ingest into witness system.
;
; ═══════════════════════════════════════════════════════════════════════════════

; (implementation in BODY)

; ═══════════════════════════════════════════════════════════════════════════════
; END SETUP [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; BODY BLOCK [BODY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief The actual work — complete boot sequence from real mode to kernel.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; SECTION ORDER (Assembly-optimized):
;   B.1 ORG CHART      — Map first, reader knows the terrain
;   B.2 CORE OPS       — WHAT it does: main execution flow
;   B.3 HELPERS        — HOW it supports: utility subroutines
;   B.4 ERROR HANDLING — Exceptional paths, failure handlers
;   B.5 PUBLIC APIS    — Exported symbols (N/A for flat binary bootloader)
;
; WHY THIS ORDER:
;   Assembly doesn't require forward declarations like C. NASM resolves forward
;   references automatically. So we organize for READING ORDER: show WHAT the
;   code does (CORE OPS) before HOW the utilities work (HELPERS).
;
; CONTAINS:
;   - B.1 ORG CHART       — Label hierarchy, execution flow, statistics
;   - B.2 CORE OPS        — Entry → Menu → Boot → Protected → Long → Kernel
;   - B.3 HELPERS         — UI (16-bit), Tech (16-bit), Mode-specific (32/64)
;   - B.4 ERROR HANDLING  — a20_failed, kernel_load_failed, halt
;   - B.5 PUBLIC APIS     — (none — flat binary, no exports)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; B.1 ORG CHART — Label Hierarchy [ORGCHART]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Map internal structure — label hierarchy, baton flow, execution path.
;
; CONTAINS:
;   - B.1a STRUCTURE  — Labels grouped by section (CORE OPS, HELPERS, ERRORS)
;   - B.1b FLOW       — Baton execution path diagram
;   - B.1c COUNTS     — Statistics (labels per section)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.1a STRUCTURE — Label Hierarchy by Section [STRUCTURE]
; ───────────────────────────────────────────────────────────────────────────────
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.2 CORE OPS — Main Execution Flow (WHAT it does)                           │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; B.2a ENTRY (16-bit Real Mode)
; └── stage0b_start      → serial init, boot event buffer, video mode
;
; B.2b MENU (16-bit Real Mode)
; ├── boot_menu          → draw UI, system info
; ├── countdown_loop     → 10-second auto-boot timer
; └── show_about         → about screen display
;
; B.2c BOOT_KERNEL (16-bit Real Mode)
; ├── boot_kernel        → loading screen, orchestrate boot sequence
; ├── .setup_video       → VESA tripwire chain (1024→800→640→text)
; ├── .get_mmap          → E820 memory map acquisition
; └── .enter_protected   → GDT load, CR0.PE set, far jump
;
; B.2d PROTECTED (32-bit Protected Mode)
; ├── protected_mode     → segment setup, kernel copy (7 chunks)
; └── setup_paging       → PML4, PDPT, PD tables (identity map 4GB)
;
; B.2e LONG_MODE (64-bit Long Mode)
; └── long_mode          → segment setup, framebuffer test, kernel jump
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.3 HELPERS — Utility Subroutines (HOW it supports)                         │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; B.3a UI_16 — Display Helpers (16-bit)
; ├── clear_screen       → blue background fill
; ├── print_centered     → center text on row
; ├── print_at           → positioned text output
; ├── print_char_at      → single character at position
; ├── print_string_teletype → BIOS TTY output
; ├── print_dec          → decimal number output
; └── draw_hline         → horizontal line drawing
;
; B.3b TECH_16 — Technical Helpers (16-bit)
; ├── enable_a20         → A20 line enable (BIOS/keyboard fallback)
; ├── serial_print       → string to COM1
; ├── serial_char        → character to COM1
; └── store_boot_event   → DAR Witness event buffer
;
; B.3c MODE_32 — Protected Mode Helpers (32-bit)
; ├── serial_print32     → string to COM1 (32-bit)
; ├── serial_char32      → character to COM1 (32-bit)
; └── store_boot_event32 → DAR event (32-bit)
;
; B.3d MODE_64 — Long Mode Helpers (64-bit)
; ├── serial_print64     → string to COM1 (64-bit)
; ├── serial_char64      → character to COM1 (64-bit)
; └── store_boot_event64 → DAR event (64-bit)
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.4 ERROR HANDLING — Failure Paths                                          │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; B.4a ERRORS — Error Handlers
; ├── .a20_failed        → A20 enable failed, return to menu
; ├── .kernel_load_failed → Disk read failed, return to menu
; ├── do_reboot          → Keyboard controller CPU reset
; └── do_halt            → CLI/HLT infinite loop
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.5 PUBLIC APIS — Exported Symbols (N/A)                                    │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; (none — flat binary bootloader, no exports)

; ───────────────────────────────────────────────────────────────────────────────
; B.1b FLOW — Baton Execution Path [FLOW]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Execution flow through B.2 CORE OPS — the main boot sequence.
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.2a ENTRY + B.2b MENU — Real Mode (16-bit)                                 │
; ├─────────────────────────────────────────────────────────────────────────────┤
; │ stage0b_start                                                               │
; │   ├─ Save boot drive                                                        │
; │   ├─ Initialize COM1 serial (115200 8N1)           [B.3b: serial_*]         │
; │   ├─ Initialize boot event buffer                  [B.3b: store_boot_event] │
; │   ├─ Store boot event: "Stage 0b entry"                                     │
; │   └─ Set text mode, hide cursor                    [B.3a: clear_screen]     │
; │         ↓                                                                   │
; │ boot_menu → Draw UI → countdown_loop (10 second auto-boot)                  │
; │         ↓ (user selects 1 or timeout)                                       │
; │         ├─ '2' → show_about → boot_menu                                     │
; │         ├─ '3' → do_reboot                         [B.4: ERROR HANDLING]    │
; │         └─ ESC → do_halt                           [B.4: ERROR HANDLING]    │
; └─────────────────────────────────────────────────────────────────────────────┘
;                                    ↓ '1' or timeout
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.2c BOOT_KERNEL — Real Mode (16-bit)                                       │
; ├─────────────────────────────────────────────────────────────────────────────┤
; │ boot_kernel                                                                 │
; │   ├─ enable_a20 (BIOS or keyboard controller)      [B.3b: enable_a20]       │
; │   │   └─ failure → .a20_failed                     [B.4: ERROR HANDLING]    │
; │   ├─ .setup_video (VESA tripwire: 1024 → 800 → 640 → text)                  │
; │   ├─ .get_mmap (E820 memory map)                                            │
; │   ├─ Build MosBootInfo structure                                            │
; │   ├─ Load kernel (7 chunks × 64KB = 448KB)                                  │
; │   │   └─ failure → .kernel_load_failed             [B.4: ERROR HANDLING]    │
; │   └─ lgdt [gdt], set CR0.PE, jmp 0x08:protected_mode                        │
; └─────────────────────────────────────────────────────────────────────────────┘
;                                    ↓
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.2d PROTECTED — Protected Mode (32-bit)                                    │
; ├─────────────────────────────────────────────────────────────────────────────┤
; │ protected_mode                                                              │
; │   ├─ Set up 32-bit segments (DS, ES, FS, GS, SS)                            │
; │   ├─ Copy kernel: low memory → 0x100000 (7 chunks)                          │
; │   ├─ setup_paging (identity map 4GB)                                        │
; │   ├─ Enable PAE (CR4.PAE)                                                   │
; │   ├─ Set CR3 to PML4                                                        │
; │   ├─ Enable Long Mode (EFER.LME)                   [B.3c: serial_print32]   │
; │   ├─ Enable Paging (CR0.PG)                        [B.3c: store_boot_event32]│
; │   └─ lgdt [gdt64], jmp 0x08:long_mode                                       │
; └─────────────────────────────────────────────────────────────────────────────┘
;                                    ↓
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.2e LONG_MODE — Long Mode (64-bit)                                         │
; ├─────────────────────────────────────────────────────────────────────────────┤
; │ long_mode                                                                   │
; │   ├─ Set up 64-bit segments                                                 │
; │   ├─ Set up temporary stack (0x90000)                                       │
; │   ├─ Visual test: fill 100 lines with Kingdom blue                          │
; │   ├─ Store final boot event: "Kernel jump"         [B.3d: store_boot_event64]│
; │   ├─ RDI = BOOT_INFO_ADDR (kernel parameter)                                │
; │   └─ jmp 0x100000 (KERNEL_LOAD_ADDR) ─────────────→ KERNEL                  │
; └─────────────────────────────────────────────────────────────────────────────┘

; ───────────────────────────────────────────────────────────────────────────────
; B.1c COUNTS — Label Statistics by Section [COUNTS]
; ───────────────────────────────────────────────────────────────────────────────
;
; ┌───────────────────────────────────────────────────────────────────────────┐
; │ Section          │ Count │ Labels                                        │
; ├───────────────────────────────────────────────────────────────────────────┤
; │ B.2 CORE OPS     │   12  │ stage0b_start, boot_menu, countdown_loop,     │
; │                  │       │ show_about, boot_kernel, .setup_video,        │
; │                  │       │ .get_mmap, .enter_protected, protected_mode,  │
; │                  │       │ setup_paging, long_mode, .load_chunk          │
; ├───────────────────────────────────────────────────────────────────────────┤
; │ B.3 HELPERS      │   17  │ UI_16: clear_screen, print_centered,          │
; │                  │       │        print_at, print_char_at,               │
; │                  │       │        print_string_teletype, print_dec,      │
; │                  │       │        draw_hline (7)                         │
; │                  │       │ TECH_16: enable_a20, serial_print,            │
; │                  │       │          serial_char, store_boot_event (4)    │
; │                  │       │ MODE_32: serial_print32, serial_char32,       │
; │                  │       │          store_boot_event32 (3)               │
; │                  │       │ MODE_64: serial_print64, serial_char64,       │
; │                  │       │          store_boot_event64 (3)               │
; ├───────────────────────────────────────────────────────────────────────────┤
; │ B.4 ERROR HANDLING│   4  │ .a20_failed, .kernel_load_failed,             │
; │                  │       │ do_reboot, do_halt                            │
; ├───────────────────────────────────────────────────────────────────────────┤
; │ B.5 PUBLIC APIS  │   0  │ (none — flat binary)                          │
; └───────────────────────────────────────────────────────────────────────────┘
;
; TOTAL: ~33 primary labels + local labels (~60 total with .sublabels)

; ═══════════════════════════════════════════════════════════════════════════════
; B.2 CORE OPS — Main Execution Flow [COREOPS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief WHAT it does — main execution flow from entry to kernel jump.
;
; This is the heart of Stage 0b: the complete boot sequence from receiving
; control from Stage 0a through CPU mode transitions to kernel handoff.
;
; CONTAINS:
;   - B.2a ENTRY       — Stage 0b entry, serial init, boot event buffer
;   - B.2b MENU        — Boot selector UI and input handling
;   - B.2c BOOT_KERNEL — A20, VESA, memory map, disk loading (16-bit)
;   - B.2d PROTECTED   — 32-bit protected mode, kernel copy, paging
;   - B.2e LONG_MODE   — 64-bit long mode, framebuffer test, kernel jump
;
; Builds FROM: B.1 ORG CHART (structure understanding)
; Builds TO:   B.3 HELPERS (utilities called by core ops)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.2a ENTRY — Stage 0b Entry Point (16-bit Real Mode) [ENTRY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Entry point from Stage 0a — first code executed after load.
;
; ───────────────────────────────────────────────────────────────────────────────

; stage0b_start is the main entry point from Stage 0a.
;
; @brief  Entry point — initialize serial, boot event buffer, set video mode.
;         Stage 0a jumps here after loading Stage 0b to 0x7E00.
;
; Parameters (registers):
;   DL: Boot drive number (passed from Stage 0a, originally from BIOS).
;
; Returns:
;   Never returns — falls through to boot_menu.
;
; Clobbers: All registers (entry point, fresh context).
;
; Execution Flow:
;   1. Save boot drive number
;   2. Initialize COM1 serial port (115200 8N1)
;   3. Initialize boot event buffer (DAR Witness)
;   4. Store "Stage 0b entry" event
;   5. Send serial banner
;   6. Set text video mode (80x25)
;   7. Hide cursor
;   8. Fall through to boot_menu
;
stage0b_start:

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Save Boot Drive
    ; ─────────────────────────────────────────────────────────────────────────
    ; DL contains boot drive from Stage 0a (originally from BIOS).
    ; Save for INT 0x13 disk operations later.

    mov [boot_drive], dl

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Initialize Serial Port (COM1)
    ; ─────────────────────────────────────────────────────────────────────────
    ; Configuration: 115200 baud, 8N1 (8 data bits, no parity, 1 stop bit).
    ; This enables debug output via serial before and during boot.
    ; DAR Witness uses this for real-time boot logging.

    ; Disable all serial interrupts
    mov dx, 0x3F9                       ; COM1 Interrupt Enable Register
    xor al, al
    out dx, al

    ; Enable DLAB (Divisor Latch Access Bit) to set baud rate
    mov dx, 0x3FB                       ; COM1 Line Control Register
    mov al, 0x80                        ; DLAB = 1
    out dx, al

    ; Set baud rate divisor = 1 (115200 baud)
    mov dx, 0x3F8                       ; COM1 Divisor Latch Low
    mov al, 1                           ; Divisor low byte = 1
    out dx, al

    mov dx, 0x3F9                       ; COM1 Divisor Latch High
    xor al, al                          ; Divisor high byte = 0
    out dx, al

    ; Set line format: 8N1 (8 data bits, no parity, 1 stop bit)
    mov dx, 0x3FB                       ; COM1 Line Control Register
    mov al, 0x03                        ; 8 bits, no parity, 1 stop, DLAB=0
    out dx, al

    ; Enable and clear FIFOs with 14-byte threshold
    mov dx, 0x3FA                       ; COM1 FIFO Control Register
    mov al, 0xC7                        ; Enable FIFO, clear RX/TX, 14-byte
    out dx, al

    ; Set modem control (DTR, RTS, OUT2 for interrupt enable)
    mov dx, 0x3FC                       ; COM1 Modem Control Register
    mov al, 0x0B                        ; DTR + RTS + OUT2
    out dx, al

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Initialize Boot Event Buffer (DAR Witness)
    ; ─────────────────────────────────────────────────────────────────────────
    ; This buffer stores boot events for kernel to ingest into witness system.
    ; Format matches debug.h MosBootEventBuffer structure.
    ; Location: BOOT_INFO_ADDR + 0x1E00 = 0xAE00

    mov dword [BOOT_EVENT_BUFFER + 0], BOOT_EVENT_MAGIC  ; magic = "EVNT"
    mov dword [BOOT_EVENT_BUFFER + 4], 0                 ; count = 0
    mov dword [BOOT_EVENT_BUFFER + 8], 0                 ; sequence = 0
    mov dword [BOOT_EVENT_BUFFER + 12], 0                ; reserved = 0

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 4: Store First Boot Event
    ; ─────────────────────────────────────────────────────────────────────────
    ; Event: "Stage 0b entry" — marks successful Stage 0b initialization.
    ; Severity: MILD (informational)
    ; Phase: DETECT (witnessing)

    push word SEV_MILD                  ; Severity: informational
    push word DAR_DETECT                ; Phase: witness the event
    mov si, evt_src_bios                ; Source: "BIOS"
    mov di, evt_msg_0b_entry            ; Message: "Stage 0b entry"
    call store_boot_event

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 5: Send Serial Banner
    ; ─────────────────────────────────────────────────────────────────────────
    ; Announce boot path on serial for debugging.

    mov si, msg_serial_banner           ; "[MOS-BIOS] MillenniumOS Boot..."
    call serial_print

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 6: Set Video Mode
    ; ─────────────────────────────────────────────────────────────────────────
    ; Set text video mode: 80x25, 16 colors (mode 3).
    ; Used for boot selector menu display.

    mov ah, 0x00                        ; BIOS function: set video mode
    mov al, 0x03                        ; Mode 3: 80x25 text, 16 colors
    int 0x10

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 7: Hide Cursor
    ; ─────────────────────────────────────────────────────────────────────────
    ; Hide text cursor for cleaner menu display.

    mov ah, 0x01                        ; BIOS function: set cursor type
    mov ch, 0x20                        ; Bit 5 = cursor off
    int 0x10

    ; Fall through to boot_menu...

; ───────────────────────────────────────────────────────────────────────────────
; B.2b MENU — Boot Selector UI (16-bit Real Mode) [MENU]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Boot selector menu — user chooses boot option or auto-boot.
;
; This section handles the user-facing boot menu:
;   1. Display the menu UI with title, options, system info
;   2. Run 10-second countdown for auto-boot
;   3. Handle key input (1=boot, 2=about, 3=reboot, ESC=halt)
;   4. Transfer control based on user choice
;
; CONTAINS:
;   - boot_menu      — Draw the main menu UI
;   - countdown_loop — 10-second auto-boot countdown with key polling
;   - show_about     — About screen display (returns to menu)
;
; NOTE: do_reboot and do_halt are in B.4 ERROR HANDLING (they're exit paths).
;
; ───────────────────────────────────────────────────────────────────────────────

; boot_menu draws the boot selector UI and handles user input.
;
; @brief  Draw boot menu with title, system info, options, and countdown.
;         Falls through to countdown_loop for auto-boot timer.
;
; Parameters: None (uses global state).
;
; Returns:
;   Never returns — transfers to boot_kernel, show_about, do_reboot, or do_halt.
;
; Clobbers: All registers (menu drawing).
;
boot_menu:

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 1: Clear screen — start with clean slate, blue background
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Previous screen content (Stage 0a messages) must be cleared.
    ;      Blue background matches Kingdom Technology branding.

    call clear_screen

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 2: Draw frame — visual boundary establishes "this is a menu"
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Double-line border (═) at top/bottom creates visual containment.
    ;      Row 0 = top edge, row 24 = bottom edge (25-row screen).
    ;      Full width (80 chars) spans entire screen.

    ; Top border (row 0)
    mov dh, 0                           ; DH = row (0 = top of screen)
    mov dl, 0                           ; DL = column (0 = left edge)
    mov cx, SCREEN_WIDTH                ; CX = length (80 chars = full width)
    mov al, 0xCD                        ; AL = char (═ double horizontal line)
    mov ah, ATTR_BORDER                 ; AH = color (cyan on blue)
    call draw_hline

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 3: Draw header — title, subtitle, scripture
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Title at row 2 (not 1) gives breathing room from border.
    ;      Centered text looks professional, uses print_centered helper.
    ;      Scripture grounds the boot experience in Kingdom purpose.

    ; Title (row 2) — "MillenniumOS" in yellow, maximum visibility
    mov dh, 2                           ; DH = row
    mov si, msg_title                   ; SI = string pointer
    mov bl, ATTR_TITLE                  ; BL = color (yellow on blue)
    call print_centered

    ; Subtitle (row 3) — version info in white, secondary importance
    mov dh, 3
    mov si, msg_subtitle
    mov bl, ATTR_TEXT                   ; White on blue
    call print_centered

    ; Scripture (rows 5-6) — Genesis 1:1 in green, spiritual grounding
    ; WHY: Row 5 (not 4) creates visual gap between subtitle and scripture.
    ;      Two lines because full verse doesn't fit on one 80-char line.
    mov dh, 5
    mov si, msg_scripture1              ; "In the beginning God created..."
    mov bl, ATTR_SCRIPTURE              ; Green on blue
    call print_centered

    mov dh, 6
    mov si, msg_scripture2              ; "...the heaven and the earth."
    mov bl, ATTR_SCRIPTURE
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 4: Draw separator — visual break between header and content
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Single line (─) is lighter than double, indicates section break.
    ;      Inset (col 5, width 70) doesn't touch edges — more elegant.

    mov dh, 8                           ; Row 8 — gap after scripture
    mov dl, 5                           ; Start at column 5 (inset)
    mov cx, 70                          ; Width 70 (not full 80)
    mov al, 0xC4                        ; Single horizontal line (─)
    mov ah, ATTR_BORDER
    call draw_hline

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 5: Draw system info — shows user what they're booting
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Users want to see version, memory, boot mode before committing.
    ;      Left-aligned at col 25 creates a clean info block.
    ;      Gray text (ATTR_INFO) is readable but doesn't compete with title.

    ; Section header (row 10)
    mov dh, 10
    mov si, msg_sysinfo                 ; "System Information"
    mov bl, ATTR_TEXT
    call print_centered

    ; Version (row 12) — left-aligned info block starts
    mov dh, 12
    mov dl, 25                          ; Column 25 — consistent left edge
    mov si, msg_version                 ; "Version: a-01.xx"
    mov bl, ATTR_INFO                   ; Gray on blue
    call print_at

    ; Copyright (row 13)
    mov dh, 13
    mov dl, 25
    mov si, msg_copyright               ; "© 2026 CreativeWorkzStudio"
    mov bl, ATTR_INFO
    call print_at

    ; Memory (row 14) — dynamic value from BIOS
    mov dh, 14
    mov dl, 25
    mov si, msg_memory                  ; "Memory: "
    mov bl, ATTR_INFO
    call print_at

    ; WHY: INT 0x12 returns conventional memory in KB in AX.
    ;      This is the only reliable way to get base memory in real mode.
    ;      We print it as decimal, then append "KB" suffix.
    int 0x12                            ; BIOS: Get conventional memory size
                                        ; Returns: AX = KB of base memory
    call print_dec                      ; Print AX as decimal number
    mov si, msg_kb                      ; " KB"
    call print_string_teletype          ; Append using BIOS TTY

    ; Boot mode (row 15)
    mov dh, 15
    mov dl, 25
    mov si, msg_bootmode                ; "Boot Mode: BIOS Legacy"
    mov bl, ATTR_INFO
    call print_at

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 6: Draw menu separator — break before options
    ; ─────────────────────────────────────────────────────────────────────────

    mov dh, 16
    mov dl, 5
    mov cx, 70
    mov al, 0xC4
    mov ah, ATTR_BORDER
    call draw_hline

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 7: Draw menu options — the actual choices
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Options are numbered 1-3 for keyboard selection.
    ;      Centered for visual balance. White text for readability.
    ;      UEFI note explains why there's no UEFI option (BIOS path).

    ; Option 1 (row 18) — primary action
    mov dh, 18
    mov si, msg_opt1                    ; "[1] Boot MillenniumOS"
    mov bl, ATTR_TEXT
    call print_centered

    ; Option 2 (row 19) — informational
    mov dh, 19
    mov si, msg_opt2                    ; "[2] About MillenniumOS"
    mov bl, ATTR_TEXT
    call print_centered

    ; Option 3 (row 20) — system action
    mov dh, 20
    mov si, msg_opt3                    ; "[3] Reboot"
    mov bl, ATTR_TEXT
    call print_centered

    ; UEFI note (row 21) — explains this is BIOS path
    mov dh, 21
    mov si, msg_uefi_note               ; "(UEFI boot uses separate path)"
    mov bl, ATTR_INFO                   ; Gray — informational, not action
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 8: Draw prompt and footer
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Prompt tells user what to do. Yellow draws attention.
    ;      Bottom border at row 24 closes the visual frame.

    ; Prompt (row 23)
    mov dh, 23
    mov si, msg_prompt                  ; "Press 1, 2, or 3..."
    mov bl, ATTR_TITLE                  ; Yellow — call to action
    call print_centered

    ; Bottom border (row 24)
    mov dh, 24
    mov dl, 0
    mov cx, SCREEN_WIDTH
    mov al, 0xCD                        ; Double line (═) matches top
    mov ah, ATTR_BORDER
    call draw_hline

    ; Fall through to countdown_loop...

; ───────────────────────────────────────────────────────────────────────────────
; countdown_loop — Auto-boot Countdown [COUNTDOWN]
; ───────────────────────────────────────────────────────────────────────────────

; countdown_loop implements the 10-second auto-boot timer.
;
; @brief  Display countdown and wait for keypress or timeout.
;         Auto-boots to kernel if countdown reaches zero.
;         User can select menu option (1-3) or ESC to halt.
;
; Parameters: None (uses [countdown] variable).
;
; Returns:
;   Never returns — jumps to boot_kernel, show_about, do_reboot, or do_halt.
;
; Clobbers: AX, BX, CX, DX, SI.
;
; Key Bindings:
;   '1' — Boot MillenniumOS (jump to boot_kernel)
;   '2' — Show About screen (jump to show_about)
;   '3' — Reboot system (jump to do_reboot)
;   ESC — Halt system (jump to do_halt)
;
; Timing:
;   Uses BIOS INT 0x1A (tick count) — 18.2 ticks per second.
;   ~18 ticks = 1 second.
;

    ; ─────────────────────────────────────────────────────────────────────────
    ; Initialize countdown timer
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: 10 seconds is long enough to read the screen, short enough to not
    ;      annoy users who just want to boot. Industry standard for boot menus.

    mov byte [countdown], 10            ; Start at 10 seconds

countdown_loop:

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 1: Display countdown message
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Row 22 is below the menu options (18-21) but above the prompt (23).
    ;      Redrawn every iteration because the number changes.

    mov dh, 22                          ; DH = row 22
    mov si, msg_autoboot                ; "Auto-boot in   seconds..."
    mov bl, ATTR_INFO                   ; Gray — informational, not distracting
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 2: Display countdown number (handles 1-10)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: The message has a gap where the number goes. We need to handle
    ;      both "10" (two chars) and "1"-"9" (one char + space to erase "1").
    ;      Column 27-28 is where the number appears in the centered message.

    mov al, [countdown]                 ; AL = current countdown value
    cmp al, 10                          ; Is it still 10?
    jb .single_digit                    ; If less than 10, single digit path

    ; ─── Two digits: display "10" ───
    ; WHY: Only case where we need two characters.
    mov dh, 22
    mov dl, 27                          ; Column 27 = first digit position
    mov al, '1'                         ; ASCII '1'
    mov ah, ATTR_TITLE                  ; Yellow — draws attention
    call print_char_at

    mov dl, 28                          ; Column 28 = second digit position
    mov al, '0'                         ; ASCII '0'
    mov ah, ATTR_TITLE
    call print_char_at
    jmp .wait_one_second

.single_digit:
    ; ─── Single digit: display " N" ───
    ; WHY: Leading space overwrites the "1" from "10" when transitioning 10→9.
    ;      Without this, "10" → "90" instead of "10" → " 9".
    mov dh, 22
    mov dl, 27                          ; Column 27 = where "1" was
    mov al, ' '                         ; Space erases the "1"
    mov ah, ATTR_INFO                   ; Same color as message (invisible)
    call print_char_at

    mov dl, 28                          ; Column 28 = digit position
    mov al, [countdown]                 ; Get countdown value (1-9)
    add al, '0'                         ; Convert to ASCII ('1'-'9')
                                        ; WHY: ASCII '0' = 0x30, so 1 + 0x30 = '1'
    mov ah, ATTR_TITLE                  ; Yellow for visibility
    call print_char_at

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 3: Wait one second (with key polling)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: BIOS doesn't have a "sleep" function. We use the tick counter.
    ;      BIOS timer ticks at 18.2 Hz (18.2 ticks per second).
    ;      We poll for keys while waiting so user can interrupt countdown.

.wait_one_second:
    ; Get starting tick count
    ; WHY: INT 0x1A AH=0 returns tick count since midnight in CX:DX.
    ;      We only use DX (low 16 bits) — good for ~1 hour of counting.
    mov ah, 0x00                        ; Function: read tick count
    int 0x1A                            ; BIOS timer services
                                        ; Returns: CX:DX = tick count
    mov [tick_start], dx                ; Save starting tick (low word only)

.tick_loop:
    ; ─── Poll for keypress (non-blocking) ───
    ; WHY: INT 0x16 AH=1 checks keyboard buffer WITHOUT removing the key.
    ;      ZF=0 means key is available, ZF=1 means no key.
    ;      This lets us respond instantly to user input during countdown.
    mov ah, 0x01                        ; Function: check keyboard buffer
    int 0x16                            ; BIOS keyboard services
                                        ; Returns: ZF=0 if key available
    jnz .key_pressed                    ; Key available — go process it

    ; ─── Check if 1 second has elapsed ───
    ; WHY: We compare current tick count to starting tick count.
    ;      18 ticks ≈ 1 second (actually 18.2, but close enough).
    mov ah, 0x00                        ; Function: read tick count
    int 0x1A                            ; Get current tick count
    sub dx, [tick_start]                ; DX = elapsed ticks
    cmp dx, 18                          ; Has 1 second passed?
    jb .tick_loop                       ; No — keep polling

    ; ─── One second passed — update countdown ───
    dec byte [countdown]                ; Decrement countdown
    jz boot_kernel                      ; If zero, auto-boot now!
                                        ; WHY: JZ = jump if zero flag set

    ; Continue countdown display loop
    jmp countdown_loop

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 4: Handle keypress
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: User pressed a key — we need to read it and dispatch to handler.
    ;      INT 0x16 AH=0 reads AND REMOVES key from buffer.
    ;      AL = ASCII code, AH = scan code (we only use AL for these keys).

.key_pressed:
    ; Read the key (consume from buffer)
    xor ah, ah                          ; AH = 0 = read key function
    int 0x16                            ; BIOS keyboard services
                                        ; Returns: AL = ASCII, AH = scan code

    ; ─── Dispatch based on key pressed ───
    ; WHY: Simple compare-and-jump chain. Each valid key has a target.
    ;      Invalid keys fall through to continue countdown (no error beep).

    cmp al, '1'                         ; '1' = Boot MillenniumOS
    je boot_kernel                      ; Jump to boot sequence

    cmp al, '2'                         ; '2' = About screen
    je show_about                       ; Jump to about display

    cmp al, '3'                         ; '3' = Reboot
    je do_reboot                        ; Jump to reboot handler (B.4)

    cmp al, 0x1B                        ; 0x1B = ESC key (ASCII escape)
    je do_halt                          ; Jump to halt handler (B.4)
                                        ; WHY: ESC is standard "exit/cancel" key

    ; Invalid key — ignore and continue countdown
    ; WHY: No error message or beep. User might have bumped keyboard.
    ;      Just keep counting down as if nothing happened.
    jmp countdown_loop

; ───────────────────────────────────────────────────────────────────────────────
; show_about — About Screen Display (part of B.2b MENU)
; ───────────────────────────────────────────────────────────────────────────────

; show_about displays the About MillenniumOS information screen.
;
; @brief  Clear screen and display About information.
;         Waits for keypress, then returns to boot_menu.
;
; Parameters: None.
;
; Returns:
;   Jumps to boot_menu after keypress.
;
; Clobbers: AX, BX, DX, SI.
;
; Display Contents:
;   - Title: "About MillenniumOS"
;   - Description of biblical-foundation OS
;   - Genesis 1:1 grounding statement
;   - Kingdom Technology Stack diagram
;   - Website and technology notes
;
show_about:

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 1: Clear screen — fresh canvas for about info
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: We want a clean display, not overlaid on the menu.

    call clear_screen

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 2: Display title and description
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Row 2 gives breathing room from top. Centered for visual balance.
    ;      Title in yellow (ATTR_TITLE) draws attention.
    ;      Description in white (ATTR_TEXT) for readability.

    mov dh, 2                           ; Row 2
    mov si, msg_about_title             ; "About MillenniumOS"
    mov bl, ATTR_TITLE                  ; Yellow on blue
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 3: Description — what is MillenniumOS?
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Users selecting "About" want to know what they're booting.
    ;      Two lines because full description doesn't fit on one.

    mov dh, 5                           ; Row 5 (gap after title)
    mov si, msg_about1                  ; "MillenniumOS is a biblical-foundation..."
    mov bl, ATTR_TEXT
    call print_centered

    mov dh, 6
    mov si, msg_about2                  ; "built on OmniCode and CPI-SI paradigm"
    mov bl, ATTR_TEXT
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 4: Scripture grounding — the anchor
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Genesis 1:1 is THE foundation of Kingdom Technology.
    ;      Green (ATTR_SCRIPTURE) distinguishes scripture from description.

    mov dh, 8
    mov si, msg_about3                  ; "Genesis 1:1 grounds everything."
    mov bl, ATTR_SCRIPTURE              ; Green on blue
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 5: Kingdom Technology Stack — where MOS fits
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Shows MOS is part of larger vision, not standalone.
    ;      Stack: Bereshit → OmniCode → CPI-SI → MillenniumOS → FaithNet

    mov dh, 10
    mov si, msg_about4                  ; "Part of the Kingdom Technology Stack:"
    mov bl, ATTR_TEXT
    call print_centered

    mov dh, 11
    mov si, msg_about5                  ; "Bereshit -> OmniCode -> CPI-SI -> ..."
    mov bl, ATTR_TEXT
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 6: Technical info and credits
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: Gray (ATTR_INFO) for less important but still useful info.

    mov dh, 13
    mov si, msg_about6                  ; Website/attribution
    mov bl, ATTR_INFO                   ; Gray on blue
    call print_centered

    mov dh, 15
    mov si, msg_about7                  ; Additional tech notes
    mov bl, ATTR_INFO
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; Step 7: Prompt and wait for key
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: User needs to know they can exit. Yellow draws attention.
    ;      INT 0x16 AH=0 blocks until key pressed — simple "pause" behavior.

    mov dh, 20
    mov si, msg_anykey                  ; "Press any key to return..."
    mov bl, ATTR_TITLE                  ; Yellow — call to action
    call print_centered

    ; Wait for keypress (blocking)
    xor ah, ah                          ; AH = 0 = read key (waits for key)
    int 0x16                            ; BIOS keyboard services
                                        ; WHY: We don't care WHICH key, just wait

    ; Return to boot menu
    ; WHY: About screen is informational — always returns to menu.
    jmp boot_menu

; ───────────────────────────────────────────────────────────────────────────────
; B.2c BOOT_KERNEL — Technical Boot Sequence (16-bit Real Mode) [BOOT_KERNEL]
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief Technical boot sequence — the heavy lifting before mode transitions.
;
; This is where the real work happens. The menu was user-facing; this is
; machine-facing. We prepare the system for the kernel:
;
;   1. Enable A20 line — access memory above 1MB
;   2. Set VESA video mode — graphical framebuffer for kernel
;   3. Get E820 memory map — tell kernel what RAM is available
;   4. Build MosBootInfo structure — package all info for kernel
;   5. Load kernel from disk — 7 chunks × 64KB = 448KB
;   6. Enter protected mode — leave 16-bit real mode behind
;
; WHY THIS ORDER:
;   - A20 first: needed for high memory access (kernel loads to 1MB)
;   - VESA before memory map: INT 0x10 might touch memory
;   - Memory map before kernel load: kernel might overwrite BIOS areas
;   - Boot info after all data gathered: needs final values
;   - Kernel load last: once loaded, we're committed
;
; ───────────────────────────────────────────────────────────────────────────────

; boot_kernel orchestrates the technical boot sequence.
;
; @brief  Display loading screen, enable A20, set video, get memory map,
;         load kernel, and transition to protected mode.
;
; Parameters: None (uses saved [boot_drive]).
;
; Returns:
;   Never returns — transitions to protected_mode then long_mode then kernel.
;
; Clobbers: All registers (fresh context for boot).
;
; Error Handling:
;   - A20 failure: Display error, wait for key, return to menu
;   - VESA failure: Fall back to text mode (continue boot)
;   - Disk failure: Display error, wait for key, return to menu
;
boot_kernel:

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 1: Display Loading Screen
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY: User feedback that something is happening. The boot process
    ;      takes a few seconds and silence feels like a hang.

    call clear_screen

    mov dh, 10                          ; Center vertically
    mov si, msg_loading                 ; "Loading MillenniumOS..."
    mov bl, ATTR_TITLE                  ; Yellow — important
    call print_centered

    mov dh, 12
    mov si, msg_please_wait             ; "Please wait..."
    mov bl, ATTR_TEXT                   ; White — secondary
    call print_centered

    ; Show boot progress indicator
    ; WHY: Gives user something to watch. Updates as each phase completes.
    mov dh, 14
    mov dl, 20                          ; Left-aligned for status updates
    mov si, msg_boot_progress           ; "Boot progress: "
    mov bl, ATTR_INFO                   ; Gray
    call print_at

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 2: Enable A20 Line
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY A20:
    ;   The 8086 could only address 1MB (20 address lines: A0-A19).
    ;   When IBM made the 286 (24-bit addressing), they found old software
    ;   relied on address wraparound at 1MB. So they added a gate on A20
    ;   that defaults to OFF for compatibility.
    ;
    ;   We MUST enable A20 to access memory above 1MB. Our kernel loads
    ;   at 0x100000 (1MB), so without A20 we'd actually access 0x000000!
    ;
    ; WHY CHECK CARRY:
    ;   enable_a20 returns CF=1 on failure. We can't proceed without A20.

    call enable_a20                     ; Try BIOS, then keyboard controller
    jc .a20_failed                      ; CF=1 means A20 enable failed

    ; ─── A20 Success: Log and continue ───
    mov si, msg_serial_a20              ; "[A20] Enabled"
    call serial_print                   ; Log to serial for debugging

    ; ─── WITNESS: Log A20 success for forensics ───
    ; WHY: DAR Witness system captures boot events for debugging.
    ;      If something fails later, we know A20 was successful.
    ;      SEV_MILD = informational (not a problem)
    ;      DAR_DETECT = observation phase (witnessing, not fixing)
    push word SEV_MILD                  ; Severity on stack (calling convention)
    push word DAR_DETECT                ; Phase on stack
    mov si, evt_src_a20                 ; Source: "A20"
    mov di, evt_msg_a20_ok              ; Message: "A20 enabled"
    call store_boot_event
    ; WHY stack args: store_boot_event uses hybrid calling (regs + stack)

    jmp .setup_video                    ; Continue to Phase 3

    ; ─────────────────────────────────────────────────────────────────────────
    ; A20 Error Handler (jumps here from boot_kernel on enable_a20 failure)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY SEPARATE: Error path is kept out of happy path for clarity.
    ;               This handler displays error, waits for key, returns to menu.
    ;               We DON'T halt — user might want to try again or reboot.

.a20_failed:
    ; ─── WITNESS: Log A20 failure ───
    ; WHY SEV_CRITICAL: A20 failure is fatal — can't proceed without it.
    push word SEV_CRITICAL
    push word DAR_DETECT
    mov si, evt_src_a20
    mov di, evt_msg_a20_fail            ; "A20 FAILED"
    call store_boot_event

    ; ─── Display error message ───
    ; WHY: User needs to know what happened and that boot failed.
    mov dh, 14
    mov dl, 25
    mov si, msg_a20_fail                ; "A20 gate enable failed"
    mov bl, ATTR_INFO
    call print_at

    mov dh, 16
    mov si, msg_boot_failed             ; "Boot failed!"
    mov bl, ATTR_TITLE                  ; Yellow — error attention
    call print_centered

    mov dh, 18
    mov si, msg_anykey                  ; "Press any key..."
    mov bl, ATTR_TEXT
    call print_centered

    ; ─── Wait for key and return to menu ───
    ; WHY: Give user control. Maybe they want to try option 2 or 3.
    xor ah, ah                          ; AH=0 = wait for key
    int 0x16                            ; BIOS keyboard
    jmp boot_menu                       ; Return to menu (not halt)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Phase 3: Set Video Mode (VESA Tripwire Fallback Chain)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY VESA:
    ;   The kernel needs a linear framebuffer for graphics. VESA (VBE) provides
    ;   standard mode setting across different video cards. We try high-res
    ;   first and fall back to lower resolutions if hardware doesn't support.
    ;
    ; WHY TRIPWIRE FALLBACK:
    ;   Different hardware supports different modes. Rather than fail, we try:
    ;   1024x768 → 800x600 → 640x480 → VGA text mode
    ;   This maximizes compatibility. Even ancient hardware can boot.
    ;
    ; DEBUG_TEXT_MODE:
    ;   When set to 1, skips VESA entirely. Useful for debugging when you
    ;   need to see text output throughout boot (VESA clears the screen).

DEBUG_TEXT_MODE equ 0                   ; Set to 1 to skip VESA for debugging

.setup_video:
%if DEBUG_TEXT_MODE
    ; DEBUG: Stay in text mode - show boot progress visibly
    mov byte [boot_in_text_mode], 1

    ; Show debug stage marker
    mov ax, VIDEO_MEM
    mov es, ax
    mov di, 160 * 12                    ; Row 12
    mov ax, 0x4F31                      ; '1' in yellow on blue
    stosw
    mov ax, 0x4F3A                      ; ':'
    stosw
    mov ax, 0x4F56                      ; 'V'
    stosw
    mov ax, 0x4F49                      ; 'I'
    stosw
    mov ax, 0x4F44                      ; 'D'
    stosw
    mov ax, 0x4F45                      ; 'E'
    stosw
    mov ax, 0x4F4F                      ; 'O'
    stosw
    mov ax, 0x4F20                      ; ' '
    stosw
    mov ax, 0x4F53                      ; 'S'
    stosw
    mov ax, 0x4F4B                      ; 'K'
    stosw
    mov ax, 0x4F49                      ; 'I'
    stosw
    mov ax, 0x4F50                      ; 'P'
    stosw

    ; Store dummy framebuffer info (text mode fallback)
    ; Kernel detects text mode by checking fb_addr == 0xB8000
    mov dword [BOOT_INFO_ADDR + 0x40], 0xB8000  ; Text mode video memory
    mov dword [BOOT_INFO_ADDR + 0x44], 0        ; High bits
    mov dword [BOOT_INFO_ADDR + 0x48], 80       ; Width (chars)
    mov dword [BOOT_INFO_ADDR + 0x4C], 25       ; Height (chars)
    mov dword [BOOT_INFO_ADDR + 0x50], 160      ; Pitch (80 * 2)
    mov dword [BOOT_INFO_ADDR + 0x54], 16       ; BPP (text mode pseudo)

    ; Color fields set to 0 for text mode (kernel checks fb_addr for detection)
    mov byte [BOOT_INFO_ADDR + 0x58], 0         ; red_size = 0
    mov byte [BOOT_INFO_ADDR + 0x59], 0         ; red_shift = 0
    mov byte [BOOT_INFO_ADDR + 0x5A], 0         ; green_size = 0
    mov byte [BOOT_INFO_ADDR + 0x5B], 0         ; green_shift = 0
    mov byte [BOOT_INFO_ADDR + 0x5C], 0         ; blue_size = 0
    mov byte [BOOT_INFO_ADDR + 0x5D], 0         ; blue_shift = 0

    jmp .get_mmap
%else
    ; =========================================================================
    ; VESA/VGA Tripwire Fallback Chain
    ; =========================================================================
    ;
    ; WHY VESA BIOS EXTENSION (VBE):
    ;   VBE provides a standardized way to set video modes across different
    ;   video cards. Before VBE, each video card needed custom drivers.
    ;   VBE functions use INT 0x10 with AX = 0x4Fxx.
    ;
    ; WHY TRIPWIRE PATTERN:
    ;   We try the highest resolution first, then progressively lower.
    ;   Each attempt is a "wire" — if it trips (fails), we try the next.
    ;   Chain: 1024x768 → 800x600 → 640x480 → VGA text
    ;   This maximizes quality while guaranteeing bootability.
    ;
    ; VBE RETURN VALUE:
    ;   AX = 0x004F means success. Anything else means failure.
    ;   AL = 0x4F = "VBE signature" (confirms VBE function)
    ;   AH = 0x00 = success, 0x01 = fail, 0x02 = not supported, 0x03 = invalid
    ;
    ; CRITICAL: VBE INT 10h AX=4F01 writes mode info to ES:DI!
    ;   Must set ES to our data segment, not assume it's correct.
    ;
    ; =========================================================================

    ; WHY: VBE functions use ES:DI as write destination.
    ;      DS already points to our data segment. Copy DS → ES.
    push ds
    pop es                              ; ES = DS (same segment as vesa_mode_info)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Tripwire 1: Try VESA 1024x768x32 (highest quality, preferred)
    ; ─────────────────────────────────────────────────────────────────────────

    ; WHY AX=4F01: "Get Mode Information" — fills ES:DI with 256-byte struct
    ;      containing resolution, framebuffer address, pixel format, etc.
    mov ax, 0x4F01                      ; VBE function: Get Mode Information
    mov cx, VESA_MODE_1024x768          ; Mode number to query
    mov di, vesa_mode_info              ; Destination buffer (ES:DI)
    int 0x10                            ; Call VBE
    ; WHY CHECK: AX=004F means mode exists and info was written.
    cmp ax, 0x004F                      ; VBE success?
    jne .try_800x600                    ; No → try next resolution

    ; WHY AX=4F02: "Set VBE Mode" — actually switches to the video mode.
    ;      The 0x4000 flag requests LINEAR framebuffer (vs banked memory).
    mov ax, 0x4F02                      ; VBE function: Set Mode
    mov bx, VESA_MODE_1024x768 | 0x4000 ; Mode + linear framebuffer flag
                                        ; WHY 0x4000: Bit 14 = use linear FB
    int 0x10                            ; Call VBE
    cmp ax, 0x004F                      ; Mode set successfully?
    je .vesa_success                    ; Yes → done with video setup

.try_800x600:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Tripwire 2: Try VESA 800x600x32 (fallback 1)
    ; ─────────────────────────────────────────────────────────────────────────

    mov ax, 0x4F01                      ; VBE: Get Mode Information
    mov cx, VESA_MODE_800x600           ; Query this mode
    mov di, vesa_mode_info              ; Same destination buffer
    int 0x10
    cmp ax, 0x004F                      ; Mode available?
    jne .try_640x480                    ; No → try even lower

    mov ax, 0x4F02                      ; VBE: Set Mode
    mov bx, VESA_MODE_800x600 | 0x4000  ; Request linear framebuffer
    int 0x10
    cmp ax, 0x004F                      ; Success?
    je .vesa_success                    ; Yes → done

.try_640x480:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Tripwire 3: Try VESA 640x480x32 (fallback 2, minimum graphical)
    ; ─────────────────────────────────────────────────────────────────────────

    mov ax, 0x4F01                      ; VBE: Get Mode Information
    mov cx, VESA_MODE_640x480           ; Query this mode
    mov di, vesa_mode_info
    int 0x10
    cmp ax, 0x004F                      ; Mode available?
    jne .vesa_failed                    ; No → all graphical modes failed

    mov ax, 0x4F02                      ; VBE: Set Mode
    mov bx, VESA_MODE_640x480 | 0x4000  ; Linear framebuffer
    int 0x10
    cmp ax, 0x004F                      ; Success?
    je .vesa_success                    ; Yes → done

    ; WHY FALLTHROUGH: If 640x480 mode set failed, we fall through to
    ;      .vesa_failed which sets up VGA text mode as ultimate fallback.
    jmp .vesa_failed

.vesa_success:
    ; ─────────────────────────────────────────────────────────────────────────
    ; VESA Mode Set Successfully
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY FLAG: Kernel needs to know if it has graphics or text mode.
    ;      boot_in_text_mode = 0 means framebuffer is graphical.
    mov byte [boot_in_text_mode], 0     ; Mark as graphics mode

    ; WHY SERIAL: Debug breadcrumb for serial console monitoring.
    ;      Visible even if screen goes blank (useful for debugging).
    mov si, msg_serial_vesa
    call serial_print

    ; WHY WITNESS: DAR forensics. Record that VESA succeeded.
    ;      SEV_MILD = informational, not a problem.
    ;      DAR_DETECT = observation phase (we detected the mode works).
    push word SEV_MILD                  ; Severity: informational
    push word DAR_DETECT                ; Phase: detection (observing)
    mov si, evt_src_vesa                ; Source: "VESA"
    mov di, evt_msg_vesa_ok             ; Message: "VESA mode set"
    call store_boot_event

    jmp .get_mmap                       ; Continue to memory map

.vesa_failed:
    ; ─────────────────────────────────────────────────────────────────────────
    ; VESA Failed — Fall Back to VGA Text Mode (Ultimate Fallback)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY TEXT MODE FALLBACK:
    ;   Even if all VESA modes fail, we can still boot in text mode.
    ;   Better to boot with limited display than not boot at all.
    ;   Kernel can detect text mode via fb_addr == 0xB8000.
    ;
    ; WHY VGA MODE 3:
    ;   Standard 80x25 color text mode. Universal support since 1987.
    ;   INT 10h AH=00 AL=03 is the most compatible video call.
    ;
    mov byte [boot_in_text_mode], 1     ; Mark as text mode

    ; WHY: Reset to known state. Previous VESA attempts may have left
    ;      video hardware in undefined state.
    mov ax, VGA_MODE_TEXT               ; VGA mode 3 (80x25 text)
    int 0x10                            ; Set video mode

    ; WHY DEBUG DISPLAY: Visual confirmation that we're in text fallback.
    ;      Yellow on red = warning color (not error, but not optimal).
    push es
    mov ax, 0xB800                      ; WHY: VGA text buffer segment
    mov es, ax                          ;      (0xB8000 linear = B800:0000)
    mov di, 160 * 13                    ; Row 13 (160 = 80 chars × 2 bytes)
    ; WHY FORMAT: Each word = char + attribute. 0x6E = yellow on brown/red.
    mov word [es:di], 0x6E56            ; 'V' (0x56) yellow on red (0x6E)
    mov word [es:di+2], 0x6E47          ; 'G'
    mov word [es:di+4], 0x6E41          ; 'A'
    mov word [es:di+6], 0x6E20          ; ' '
    mov word [es:di+8], 0x6E54          ; 'T'
    mov word [es:di+10], 0x6E45         ; 'E'
    mov word [es:di+12], 0x6E58         ; 'X'
    mov word [es:di+14], 0x6E54         ; 'T'
    pop es

    ; WHY STORE INFO: Kernel needs framebuffer info even for text mode.
    ;      We provide the text buffer address so kernel can draw to it.
    ;      Kernel detects text mode by: fb_addr == 0xB8000
    mov dword [BOOT_INFO_ADDR + 0x40], 0xB8000  ; Text buffer address
    mov dword [BOOT_INFO_ADDR + 0x44], 0        ; High 32 bits = 0
    mov dword [BOOT_INFO_ADDR + 0x48], 80       ; Width: 80 characters
    mov dword [BOOT_INFO_ADDR + 0x4C], 25       ; Height: 25 rows
    mov dword [BOOT_INFO_ADDR + 0x50], 160      ; Pitch: 80 × 2 bytes/char
    mov dword [BOOT_INFO_ADDR + 0x54], 16       ; BPP: pseudo (text mode)

    ; WHY ZERO COLORS: Text mode doesn't have RGB channels.
    ;      Kernel checks these + fb_addr to detect text mode.
    mov byte [BOOT_INFO_ADDR + 0x58], 0         ; red_size = 0
    mov byte [BOOT_INFO_ADDR + 0x59], 0         ; red_shift = 0
    mov byte [BOOT_INFO_ADDR + 0x5A], 0         ; green_size = 0
    mov byte [BOOT_INFO_ADDR + 0x5B], 0         ; green_shift = 0
    mov byte [BOOT_INFO_ADDR + 0x5C], 0         ; blue_size = 0
    mov byte [BOOT_INFO_ADDR + 0x5D], 0         ; blue_shift = 0

    ; WHY WITNESS: DAR forensics. Record that we fell back to text.
    ;      SEV_MEDIUM = warning (not critical, but not ideal).
    ;      DAR_ASSESS = evaluation phase (we assessed and chose fallback).
    push word SEV_MEDIUM                ; Severity: warning
    push word DAR_ASSESS                ; Phase: assessment
    mov si, evt_src_vesa                ; Source: "VESA"
    mov di, evt_msg_text_mode           ; Message: "Text mode fallback"
    call store_boot_event

    jmp .get_mmap                       ; Continue to memory map
%endif

; ───────────────────────────────────────────────────────────────────────────────
; B.2.3 Get Memory Map (E820)
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY E820:
;   INT 0x15 AX=E820 is the BIOS memory map function. It tells us which
;   memory regions are usable vs reserved (BIOS, ACPI, devices).
;   Without this, kernel doesn't know where safe RAM is.
;
; WHY "SMAP":
;   "SMAP" (0x534D4150) is a magic signature. BIOS returns it in EAX
;   to confirm E820 is supported and result is valid. Acts as handshake.
;
; ENTRY FORMAT (24 bytes each):
;   0-7:   Base address (64-bit)
;   8-15:  Length (64-bit)
;   16-19: Type (1=usable, 2=reserved, 3=ACPI reclaimable, etc.)
;   20-23: Extended attributes (optional, usually 0)
;
; CONTINUATION:
;   EBX = 0 on first call, then BIOS returns continuation value.
;   When BIOS returns EBX = 0, no more entries exist.
;

.get_mmap:
    ; WHY: EBX=0 signals "start from beginning" to BIOS.
    xor ebx, ebx                        ; Continuation = 0 (first call)
    mov di, memory_map                  ; Destination for entries
    xor bp, bp                          ; Entry count = 0

.mmap_loop:
    ; WHY AX=E820: This specific function number requests memory map.
    ;      It's been standard since 1990s, supported by all modern BIOS.
    mov eax, 0xE820                     ; E820 = query memory map
    ; WHY 24: Each entry is 24 bytes (base + length + type + extended).
    mov ecx, 24                         ; Buffer size = 24 bytes
    ; WHY EDX=SMAP: Signature handshake. "SMAP" backwards = 0x534D4150.
    mov edx, 0x534D4150                 ; "SMAP" signature
    int 0x15                            ; Call BIOS
    ; WHY CF: Carry flag set means E820 failed (not supported or done).
    jc .mmap_done                       ; CF=1 → error or no more entries

    ; WHY CHECK EAX: BIOS echoes "SMAP" to confirm valid response.
    ;      If EAX != SMAP, response is corrupt or unsupported.
    cmp eax, 0x534D4150                 ; BIOS should return "SMAP"
    jne .mmap_done                      ; Invalid → stop

    ; WHY ADD 24: Move to next entry slot in our buffer.
    add di, 24                          ; Advance buffer pointer
    inc bp                              ; Increment entry count

    ; WHY LIMIT: Prevent buffer overflow. MMAP_MAX_ENTRIES = 256.
    cmp bp, MMAP_MAX_ENTRIES            ; Hit maximum?
    jge .mmap_done                      ; Yes → stop (safety limit)

    ; WHY TEST EBX: BIOS returns EBX=0 when no more entries.
    ;      Non-zero means "call again with this value to continue."
    test ebx, ebx                       ; More entries available?
    jnz .mmap_loop                      ; Yes → continue loop

.mmap_done:
    ; WHY STORE COUNT: Kernel needs to know how many entries to process.
    mov [mmap_count], bp                ; Save entry count

    ; WHY SERIAL: Debug breadcrumb — memory map is critical.
    mov si, msg_serial_mmap
    call serial_print

    ; WHY WITNESS: DAR forensics. Record successful memory map acquisition.
    push word SEV_MILD                  ; Severity: informational
    push word DAR_DETECT                ; Phase: detection
    mov si, evt_src_mmap                ; Source: "MMAP"
    mov di, evt_msg_mmap_ok             ; Message: "Memory map acquired"
    call store_boot_event

; ───────────────────────────────────────────────────────────────────────────────
; B.2.4 Build Unified Boot Info Structure (MosBootInfo v2)
; ───────────────────────────────────────────────────────────────────────────────
;
; Structure offsets (must match bootinfo.h):
;   HEADER (0x0000-0x003F):
;     0x00: magic (4), 0x04: version (1), 0x05: boot_source (1), 0x06: boot_state (1)
;     0x08: capabilities (4), 0x0C: memory_map_count (4)
;     0x10: total_memory (8), 0x18: kernel_physical (8), 0x20: kernel_size (8)
;     0x28: boot_tsc (8), 0x30: entry_tsc (8)
;   FRAMEBUFFER (0x0040-0x006F) - MosFramebuffer structure (40 bytes):
;     0x40: address (8), 0x48: width (4), 0x4C: height (4), 0x50: pitch (4), 0x54: bpp (4)
;     0x58: red_size (1), 0x59: red_shift (1), 0x5A: green_size (1), 0x5B: green_shift (1)
;     0x5C: blue_size (1), 0x5D: blue_shift (1), 0x5E-0x67: reserved (10 bytes)
;   MEMORY ZONES (0x0070-0x008F):
;     0x70: zone_void_end (8), 0x78: zone_word_end (8)
;   MEMORY MAP (0x0100+):
;     256 entries * 24 bytes each
;

    ; ─────────────────────────────────────────────────────────────────────────
    ; Clear boot info area
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY CLEAR: Start with zeros so any field we don't explicitly set
    ;      defaults to 0/NULL. Prevents garbage from confusing kernel.
    mov di, BOOT_INFO_ADDR              ; Destination = boot info structure
    mov cx, 512                         ; Clear 512 bytes (header + FB + zones)
    xor al, al                          ; Fill value = 0
    rep stosb                           ; WHY REP STOSB: Fast byte fill

    ; ═══════════════════════════════════════════════════════════════════════
    ; HEADER (0x0000-0x003F) — Core boot metadata
    ; ═══════════════════════════════════════════════════════════════════════

    ; WHY MAGIC: "MOSB" signature lets kernel verify structure is valid.
    ;      If magic != MOSB, kernel knows memory is corrupt or wrong address.
    ;      0x4D4F5342 = "MOSB" (MillenniumOS Boot) in little-endian.
    mov dword [BOOT_INFO_ADDR + 0x00], 0x4D4F5342

    ; WHY VERSION: Allows kernel to handle different structure layouts.
    ;      Version 2 = unified BIOS/UEFI structure with ternary fields.
    mov byte [BOOT_INFO_ADDR + 0x04], 2         ; Structure version

    ; WHY BOOT SOURCE: Kernel may need different behavior for BIOS vs UEFI.
    ;      Ternary encoding: -1 = BIOS (legacy), 0 = unknown, +1 = UEFI (modern).
    mov byte [BOOT_INFO_ADDR + 0x05], 0xFF      ; -1 = BIOS path

    ; WHY BOOT STATE: Initial state for error tracking.
    ;      Ternary: -1 = error, 0 = OK, +1 = extra good (with diagnostics).
    mov byte [BOOT_INFO_ADDR + 0x06], 0         ; 0 = OK

    ; WHY CAPABILITIES: Bitfield of features available on this boot path.
    ;      BIOS path has fewer capabilities than UEFI (no 64-bit FB, etc.).
    mov dword [BOOT_INFO_ADDR + 0x08], 0        ; No special capabilities

    ; WHY MAP COUNT: Kernel needs to know how many E820 entries to process.
    ;      32-bit field even though count is small (for alignment).
    movzx eax, word [mmap_count]                ; Zero-extend 16→32 bits
    mov [BOOT_INFO_ADDR + 0x0C], eax            ; Store count

    ; WHY TOTAL MEMORY: Convenience field — kernel calculates from map.
    ;      We leave it 0; kernel sums usable regions from memory map.
    mov dword [BOOT_INFO_ADDR + 0x10], 0        ; Low 32 bits
    mov dword [BOOT_INFO_ADDR + 0x14], 0        ; High 32 bits (64-bit field)

    ; WHY KERNEL PHYSICAL: Tells kernel where it was loaded in RAM.
    ;      0x100000 (1MB) is traditional kernel load address — above real mode.
    mov dword [BOOT_INFO_ADDR + 0x18], KERNEL_LOAD_ADDR
    mov dword [BOOT_INFO_ADDR + 0x1C], 0        ; High 32 bits

    ; WHY KERNEL SIZE: Kernel needs to know its own size for memory mapping.
    ;      Calculated from sectors * 512 bytes/sector.
    mov dword [BOOT_INFO_ADDR + 0x20], KERNEL_TOTAL_SECTORS * 512
    mov dword [BOOT_INFO_ADDR + 0x24], 0        ; High 32 bits

    ; WHY BOOT TSC: Timestamp for boot timing diagnostics.
    ;      RDTSC reads CPU timestamp counter — cycles since boot.
    ;      Enables measuring: "how long did boot take?"
    rdtsc                                       ; EDX:EAX = timestamp
    mov [BOOT_INFO_ADDR + 0x28], eax            ; Low 32 bits
    mov [BOOT_INFO_ADDR + 0x2C], edx            ; High 32 bits

    ; WHY NO ENTRY TSC HERE: entry_tsc is set later when we jump to kernel.
    ;      Captures the exact moment of kernel entry (in long mode).

    ; ═══════════════════════════════════════════════════════════════════════
    ; FRAMEBUFFER (0x0040-0x006F) — Display surface information
    ; ═══════════════════════════════════════════════════════════════════════
    ;
    ; WHY: Kernel needs to know where to draw pixels and how they're formatted.
    ;      VESA mode info contains framebuffer address, dimensions, pixel format.
    ;      If VESA failed, .vesa_failed already set text mode values (0xB8000).
    ;
    ; WHY CONDITIONAL: Skip copy if we're in text mode — values already set.

    cmp byte [boot_in_text_mode], 0             ; Graphics mode?
    jne .skip_vesa_copy                         ; No → text mode values already set

    ; WHY PHYS BASE PTR: This is the linear address of the framebuffer.
    ;      VESA mode info offset 40 = PhysBasePtr (where framebuffer lives).
    ;      Write pixels here → they appear on screen.
    mov eax, [vesa_mode_info + 40]              ; PhysBasePtr (32-bit in VESA)
    mov [BOOT_INFO_ADDR + 0x40], eax            ; Low 32 bits of address
    mov dword [BOOT_INFO_ADDR + 0x44], 0        ; High 32 bits = 0 (VESA is 32-bit)

    ; WHY DIMENSIONS: Kernel needs width/height for bounds checking and layout.
    ;      VESA stores as 16-bit values; we expand to 32-bit for uniformity.
    movzx eax, word [vesa_mode_info + 18]       ; XResolution (width in pixels)
    mov [BOOT_INFO_ADDR + 0x48], eax
    movzx eax, word [vesa_mode_info + 20]       ; YResolution (height in pixels)
    mov [BOOT_INFO_ADDR + 0x4C], eax

    ; WHY PITCH: Bytes per scan line. May differ from width × bpp due to alignment.
    ;      Kernel uses: pixel_addr = base + (y * pitch) + (x * bytes_per_pixel)
    movzx eax, word [vesa_mode_info + 16]       ; BytesPerScanLine
    mov [BOOT_INFO_ADDR + 0x50], eax

    ; WHY BPP: Bits per pixel. Usually 32 (8R + 8G + 8B + 8 padding) or 24.
    ;      Determines how to pack color values into framebuffer words.
    movzx eax, byte [vesa_mode_info + 25]       ; BitsPerPixel
    mov [BOOT_INFO_ADDR + 0x54], eax

    ; WHY COLOR MASKS: Different video cards arrange RGB differently.
    ;      RedMaskSize/Position tells how to pack red component.
    ;      E.g., for 0xRRGGBB00: red_size=8, red_shift=24.
    ;      Kernel uses these to construct pixel values correctly.
    mov al, [vesa_mode_info + 31]               ; RedMaskSize (bits for red)
    mov [BOOT_INFO_ADDR + 0x58], al
    mov al, [vesa_mode_info + 32]               ; RedFieldPosition (shift)
    mov [BOOT_INFO_ADDR + 0x59], al
    mov al, [vesa_mode_info + 33]               ; GreenMaskSize
    mov [BOOT_INFO_ADDR + 0x5A], al
    mov al, [vesa_mode_info + 34]               ; GreenFieldPosition
    mov [BOOT_INFO_ADDR + 0x5B], al
    mov al, [vesa_mode_info + 35]               ; BlueMaskSize
    mov [BOOT_INFO_ADDR + 0x5C], al
    mov al, [vesa_mode_info + 36]               ; BlueFieldPosition
    mov [BOOT_INFO_ADDR + 0x5D], al

.skip_vesa_copy:

    ; ═══════════════════════════════════════════════════════════════════════
    ; MEMORY ZONES (0x0070-0x008F) — Ternary memory organization
    ; ═══════════════════════════════════════════════════════════════════════
    ;
    ; WHY TERNARY ZONES: MillenniumOS uses Genesis-inspired 3-zone memory:
    ;   VOID (0 to zone_void_end):     Formless — BIOS/boot, untouchable
    ;   WORD (void_end to word_end):   Definitions — kernel code/data
    ;   TOV  (word_end to RAM end):    Completion — heap, user space
    ;
    ; These are initial hints. Kernel refines based on actual memory map.

    ; WHY 1MB: Traditional PC memory map has BIOS/legacy stuff below 1MB.
    ;      Kernel loads at 1MB, so everything below is "void" (untouchable).
    mov dword [BOOT_INFO_ADDR + 0x70], 0x100000 ; zone_void_end = 1MB
    mov dword [BOOT_INFO_ADDR + 0x74], 0        ; High 32 bits

    ; WHY KERNEL END: Word zone extends from 1MB to end of kernel.
    ;      Kernel will refine this once it knows its full layout.
    mov dword [BOOT_INFO_ADDR + 0x78], KERNEL_LOAD_ADDR + (KERNEL_TOTAL_SECTORS * 512)
    mov dword [BOOT_INFO_ADDR + 0x7C], 0        ; High 32 bits

    ; ═══════════════════════════════════════════════════════════════════════
    ; MEMORY MAP (0x0100+) — E820 entries for kernel
    ; ═══════════════════════════════════════════════════════════════════════
    ;
    ; WHY COPY: Memory map was built in temporary buffer during E820 loop.
    ;      Copy to its permanent home in MosBootInfo structure.
    ;      Kernel expects it at fixed offset 0x100.

    mov si, memory_map                          ; Source: temporary buffer
    mov di, BOOT_INFO_ADDR + 0x100              ; Dest: boot info offset 0x100
    mov cx, [mmap_count]                        ; Number of entries
    ; WHY IMUL 24: Each E820 entry is 24 bytes.
    imul cx, cx, 24                             ; Total bytes = count × 24
    rep movsb                                   ; Copy bytes

; ───────────────────────────────────────────────────────────────────────────────
; B.2.5 Load Kernel from Disk (7 chunks = 448KB for real OS)
; ───────────────────────────────────────────────────────────────────────────────
;
; Real mode segment limit is 64KB, so we load in 7 chunks.
; Memory layout (carefully avoiding page tables at 0x70000):
;
;   ┌─────────────────────────────────────────────────────────────────────┐
;   │ 0x10000-0x1FFFF │ Chunk 1 (64KB) │ First kernel segment            │
;   │ 0x20000-0x2FFFF │ Chunk 2 (64KB) │                                  │
;   │ 0x30000-0x3FFFF │ Chunk 3 (64KB) │                                  │
;   │ 0x40000-0x4FFFF │ Chunk 4 (64KB) │                                  │
;   │ 0x50000-0x5FFFF │ Chunk 5 (64KB) │                                  │
;   │ 0x60000-0x6FFFF │ Chunk 6 (64KB) │ Last before page tables          │
;   │ 0x70000-0x75FFF │ PAGE TABLES    │ *** RESERVED ***                 │
;   │ 0x80000-0x8FFFF │ Chunk 7 (64KB) │ After page tables                │
;   └─────────────────────────────────────────────────────────────────────┘
;
; Total: 7 × 64KB = 448KB kernel capacity via BIOS INT 13h
; After boot, kernel can load additional modules via PIO disk access.
;

    ; WHY SERIAL: Debug breadcrumb for kernel load — critical phase.
    mov si, msg_serial_load
    call serial_print

    ; WHY WITNESS: DAR forensics — record that kernel load started.
    push word SEV_MILD                  ; Severity: informational
    push word DAR_DETECT                ; Phase: detection
    mov si, evt_src_disk                ; Source: "DISK"
    mov di, evt_msg_load_start          ; Message: "Kernel load starting"
    call store_boot_event

    ; WHY DISPLAY: User feedback — kernel loading takes time.
    mov dh, 13                          ; Row 13
    mov si, msg_loading_kernel          ; "Loading kernel..."
    mov bl, ATTR_TEXT                   ; Standard text color
    call print_centered

    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY DAP (Disk Address Packet): Modern BIOS INT 13h extension.
    ;   - dap_sectors: number of 512-byte sectors to read
    ;   - dap_segment:dap_offset: destination address in seg:off format
    ;   - dap_lba: Logical Block Address (sector number from disk start)
    ;
    ; WHY CHUNKING: Real mode segment limit = 64KB (0xFFFF offset max).
    ;   Can't load 448KB at once. Load in 7 × 64KB chunks.
    ;
    ; WHY LBA 34: Kernel starts at sector 34 on disk.
    ;   Sectors 0-33 = bootloader (stage0a + stage0b).
    ;
    ; WHY 128 SECTORS = 64KB: 128 × 512 bytes = 65536 bytes = 64KB.
    ; ─────────────────────────────────────────────────────────────────────────

    ; ─── Chunk 1: 64KB to 0x10000 ───
    ; WHY 0x1000 segment: 0x1000:0000 = linear 0x10000 (64KB mark)
    mov word [dap_sectors], KERNEL_SECTORS_1  ; 128 sectors = 64KB
    mov word [dap_segment], 0x1000            ; Segment = 0x1000
    mov word [dap_offset], 0                  ; Offset = 0
    mov dword [dap_lba_lo], 34                ; LBA = 34 (kernel start)
    mov dword [dap_lba_hi], 0                 ; High 32 bits = 0
    call .load_chunk
    jc .kernel_load_failed                    ; CF=1 → disk read failed

    ; ─── Chunk 2: 64KB to 0x20000 ───
    mov word [dap_segment], 0x2000            ; Segment = 0x2000
    mov dword [dap_lba_lo], 34 + 128          ; LBA = 34 + 128 = 162
    call .load_chunk
    jc .kernel_load_failed

    ; ─── Chunk 3: 64KB to 0x30000 ───
    mov word [dap_segment], 0x3000
    mov dword [dap_lba_lo], 34 + 256          ; LBA = 290
    call .load_chunk
    jc .kernel_load_failed

    ; ─── Chunk 4: 64KB to 0x40000 ───
    mov word [dap_segment], 0x4000
    mov dword [dap_lba_lo], 34 + 384          ; LBA = 418
    call .load_chunk
    jc .kernel_load_failed

    ; ─── Chunk 5: 64KB to 0x50000 ───
    mov word [dap_segment], 0x5000
    mov dword [dap_lba_lo], 34 + 512          ; LBA = 546
    call .load_chunk
    jc .kernel_load_failed

    ; ─── Chunk 6: 64KB to 0x60000 ───
    mov word [dap_segment], 0x6000
    mov dword [dap_lba_lo], 34 + 640          ; LBA = 674
    call .load_chunk
    jc .kernel_load_failed

    ; ─── Chunk 7: 64KB to 0x80000 (skip 0x70000 = page tables) ───
    ; WHY SKIP 0x70000: Page tables live at 0x70000-0x75FFF.
    ;   Must not overwrite them or protected mode transition will fail.
    mov word [dap_segment], 0x8000
    mov dword [dap_lba_lo], 34 + 768          ; LBA = 802
    call .load_chunk
    jc .kernel_load_failed

    ; WHY WITNESS: DAR forensics — record successful kernel load.
    ;   If we get here, all 7 chunks loaded without error.
    push word SEV_MILD                  ; Severity: informational
    push word DAR_DETECT                ; Phase: detection
    mov si, evt_src_disk                ; Source: "DISK"
    mov di, evt_msg_load_ok             ; Message: "Kernel loaded"
    call store_boot_event

    jmp .enter_protected                ; Continue to protected mode

; ─────────────────────────────────────────────────────────────────────────
; .load_chunk — Load one 64KB chunk via INT 13h Extended Read
; ─────────────────────────────────────────────────────────────────────────
;
; WHY INT 13h EXTENDED (AH=42h):
;   Classic INT 13h (AH=02h) uses CHS addressing — limited to 8GB.
;   Extended read uses LBA (Logical Block Addressing) — supports huge disks.
;   DAP (Disk Address Packet) at DS:SI describes the read operation.
;
; Returns: CF=0 success, CF=1 failure
;
.load_chunk:
    mov ah, 0x42                        ; AH=42h = Extended Read Sectors
    mov dl, [boot_drive]                ; DL = drive number (saved at boot)
    mov si, disk_address_packet         ; DS:SI = pointer to DAP structure
    int 0x13                            ; BIOS disk services
    ; WHY RET PRESERVES CF: Caller checks CF to detect disk read failure.
    ret

; ─────────────────────────────────────────────────────────────────────────
; .kernel_load_failed — Handle Disk Read Failure
; ─────────────────────────────────────────────────────────────────────────
;
; WHY SEPARATE ERROR PATH:
;   Disk failure is unrecoverable — can't boot without kernel.
;   Show error message, let user return to menu (might be transient).
;
.kernel_load_failed:
    ; WHY WITNESS: DAR forensics — record critical failure for diagnosis.
    push word SEV_CRITICAL              ; Severity: critical
    push word DAR_DETECT                ; Phase: detection
    mov si, evt_src_disk                ; Source: "DISK"
    mov di, evt_msg_load_fail           ; Message: "Kernel load failed"
    call store_boot_event

    ; WHY TEXT MODE: Must show error message. VESA might be set.
    ;   Switch to VGA text mode to ensure message is visible.
    mov ah, 0x00                        ; AH=00h = Set Video Mode
    mov al, 0x03                        ; AL=03h = 80x25 text mode
    int 0x10

    call clear_screen

    ; WHY ERROR DISPLAY: User needs to know what happened.
    mov dh, 10                          ; Row 10
    mov si, msg_kernel_fail             ; "KERNEL LOAD FAILED"
    mov bl, ATTR_TITLE                  ; Highlight color
    call print_centered

    mov dh, 12                          ; Row 12
    mov si, msg_boot_failed             ; Explanation
    mov bl, ATTR_TEXT
    call print_centered

    mov dh, 14                          ; Row 14
    mov si, msg_anykey                  ; "Press any key..."
    mov bl, ATTR_TEXT
    call print_centered

    ; WHY WAIT: Let user read error before returning to menu.
    xor ah, ah                          ; AH=00h = Wait for key
    int 0x16                            ; BIOS keyboard
    jmp boot_menu                       ; Return to menu (let user retry or reboot)

; ───────────────────────────────────────────────────────────────────────────────
; B.2.6 Enter Protected Mode — Transition from 16-bit Real Mode
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY PROTECTED MODE:
;   Real mode (16-bit) has severe limitations:
;   - Can only address 1MB of memory (20-bit addresses)
;   - No memory protection (any code can overwrite anything)
;   - Segment:offset addressing is cumbersome
;
;   Protected mode (32-bit) provides:
;   - 4GB address space (32-bit linear addresses)
;   - Memory protection via segment descriptors
;   - Prerequisite for long mode (64-bit)
;
; THE TRANSITION SEQUENCE:
;   1. CLI — disable interrupts (no BIOS handlers in protected mode)
;   2. LGDT — load Global Descriptor Table (segment definitions)
;   3. Set CR0.PE — enable protection enable bit
;   4. Far JMP — flush prefetch queue, load CS with 32-bit selector
;

.enter_protected:
    ; WHY SERIAL: Last 16-bit debug output — about to leave real mode.
    mov si, msg_serial_prot
    call serial_print

    ; WHY WITNESS: DAR forensics — record protected mode entry attempt.
    push word SEV_MILD
    push word DAR_DETECT
    mov si, evt_src_prot
    mov di, evt_msg_prot_enter
    call store_boot_event

    ; WHY DEBUG MARKER: Visual breadcrumb in video memory.
    ;   "2:PROT" = about to enter protected mode.
    mov ax, VIDEO_MEM                   ; VGA text buffer segment
    mov es, ax
    mov di, 160 * 13                    ; Row 13 (160 bytes per row)
    mov ax, 0x2F32                      ; '2' = stage 2, green on white
    stosw
    mov ax, 0x2F3A                      ; ':'
    stosw
    mov ax, 0x2F50                      ; 'P'
    stosw
    mov ax, 0x2F52                      ; 'R'
    stosw
    mov ax, 0x2F4F                      ; 'O'
    stosw
    mov ax, 0x2F54                      ; 'T'
    stosw

    ; ─────────────────────────────────────────────────────────────────────────
    ; POINT OF NO RETURN: After CLI, no more BIOS calls possible.
    ; Real mode interrupt handlers won't work in protected mode.
    ; ─────────────────────────────────────────────────────────────────────────

    ; WHY CLI: Disable interrupts. BIOS handlers are real mode code.
    ;   If an interrupt fires after CR0.PE is set, CPU will crash
    ;   trying to execute 16-bit handler in 32-bit mode.
    cli

    ; WHY LGDT: Load Global Descriptor Table register.
    ;   GDT defines memory segments (code/data) with base, limit, permissions.
    ;   CPU needs GDT to interpret segment selectors in protected mode.
    lgdt [gdt_descriptor]

    ; WHY CR0.PE: Control Register 0, Protection Enable bit (bit 0).
    ;   Setting this bit switches CPU from real mode to protected mode.
    ;   After this, segment registers are selectors into GDT, not paragraphs.
    mov eax, cr0                        ; Read current CR0
    or eax, 1                           ; Set bit 0 (PE = Protection Enable)
    mov cr0, eax                        ; Write back — NOW IN PROTECTED MODE

    ; WHY FAR JUMP: Flush instruction prefetch queue.
    ;   CPU may have prefetched instructions decoded for real mode.
    ;   Far jump forces CPU to refetch using protected mode decoding.
    ;   0x08 = code segment selector (GDT entry 1, RPL 0).
    jmp 0x08:protected_mode

; ───────────────────────────────────────────────────────────────────────────────
; B.2d PROTECTED MODE — 32-bit Execution (Transition Phase)
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY 32-BIT PROTECTED MODE:
;   This is a TRANSITION phase, not the final destination.
;   We need 32-bit to:
;   1. Access memory above 1MB (copy kernel to 0x100000)
;   2. Set up page tables for long mode
;   3. Prepare 64-bit GDT entries
;
;   CPU path: 16-bit Real → 32-bit Protected → 64-bit Long Mode
;   We can't jump directly from real mode to long mode.
;

bits 32                                 ; WHY: Tell assembler to emit 32-bit code

protected_mode:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Set up 32-bit segment registers
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY 0x10: This is GDT entry 2 (0x10 = 16 = index 2 × 8 bytes per entry).
    ;   GDT layout: 0x00 = null, 0x08 = 32-bit code, 0x10 = 32-bit data.
    ;   All data segments point to same flat 4GB descriptor.
    mov ax, 0x10                        ; 0x10 = 32-bit data segment selector
    mov ds, ax                          ; Data segment
    mov es, ax                          ; Extra segment
    mov fs, ax                          ; FS segment
    mov gs, ax                          ; GS segment
    mov ss, ax                          ; Stack segment

    ; WHY SERIAL: Debug output using direct port I/O (no BIOS).
    mov esi, msg_serial32_entry
    call serial_print32

    ; WHY WITNESS: DAR forensics — record successful mode transition.
    mov esi, evt32_src_prot
    mov edi, evt32_msg_entry
    mov eax, (SEV_MILD << 8) | (DAR_DETECT & 0xFF)
    call store_boot_event32

    ; WHY DEBUG MARKER: "3:32BIT" = we're in 32-bit protected mode.
    ;   Direct video memory write at 0xB8000 (linear address).
    mov edi, 0xB8000 + 160 * 14         ; Row 14
    mov dword [edi], 0x2F3A2F33         ; '3' ':' (little-endian pairs)
    mov dword [edi+4], 0x2F322F33       ; '3' '2'
    mov dword [edi+8], 0x2F492F42       ; 'B' 'I'
    mov word [edi+12], 0x2F54           ; 'T'

    ; ─────────────────────────────────────────────────────────────────────────
    ; Copy kernel from low memory to 0x100000 (1MB)
    ; ─────────────────────────────────────────────────────────────────────────
    ;
    ; WHY COPY: We loaded kernel via BIOS INT 13h into low memory (< 1MB).
    ;   BIOS can only access low memory in real mode. Now in protected mode,
    ;   we can access the full 4GB address space. Copy to standard location.
    ;
    ; WHY 0x100000: Traditional kernel load address. Below 1MB is:
    ;   - Real mode IVT (0x000-0x3FF)
    ;   - BIOS data area (0x400-0x4FF)
    ;   - Bootloader code/data (0x7C00-0x9FFFF)
    ;   - Video memory (0xA0000-0xBFFFF)
    ;   - ROM area (0xC0000-0xFFFFF)
    ;   1MB mark is first safe address for kernel.
    ;
    ; WHY 7 CHUNKS: BIOS loaded 7 × 64KB = 448KB in scattered low memory.
    ;   Now combine into contiguous region for kernel execution.
    ;
    ; Source → Destination mapping:
    ;   0x10000 → 0x100000 (Chunk 1)
    ;   0x20000 → 0x110000 (Chunk 2)
    ;   0x30000 → 0x120000 (Chunk 3)
    ;   0x40000 → 0x130000 (Chunk 4)
    ;   0x50000 → 0x140000 (Chunk 5)
    ;   0x60000 → 0x150000 (Chunk 6)
    ;   0x80000 → 0x160000 (Chunk 7, skipped 0x70000 page tables)
    ;
    ; ─────────────────────────────────────────────────────────────────────────

    ; WHY CLD: Clear Direction flag. REP MOVSD increments ESI/EDI.
    ;   Without CLD, direction is undefined — copy could go backwards.
    cld

    ; WHY 64KB/4: REP MOVSD copies dwords (4 bytes). 64KB = 16384 dwords.
    mov edi, KERNEL_LOAD_ADDR           ; Dest: 0x100000 (1MB)
    mov ebx, 64 * 1024 / 4              ; 16384 dwords per chunk (reuse in EBX)

    ; ─── Chunk 1: 0x10000 → 0x100000 ───
    mov esi, 0x10000                    ; Source: low memory chunk 1
    mov ecx, ebx                        ; Count: 16384 dwords = 64KB
    rep movsd                           ; Copy dwords, EDI auto-increments

    ; ─── Chunk 2: 0x20000 → 0x110000 ───
    mov esi, 0x20000                    ; Source: chunk 2
    mov ecx, ebx                        ; Reset count
    rep movsd                           ; EDI continues at 0x110000

    ; ─── Chunk 3: 0x30000 → 0x120000 ───
    mov esi, 0x30000
    mov ecx, ebx
    rep movsd

    ; ─── Chunk 4: 0x40000 → 0x130000 ───
    mov esi, 0x40000
    mov ecx, ebx
    rep movsd

    ; ─── Chunk 5: 0x50000 → 0x140000 ───
    mov esi, 0x50000
    mov ecx, ebx
    rep movsd

    ; ─── Chunk 6: 0x60000 → 0x150000 ───
    mov esi, 0x60000
    mov ecx, ebx
    rep movsd

    ; ─── Chunk 7: 0x80000 → 0x160000 (0x70000 has page tables) ───
    mov esi, 0x80000                    ; Source: after page tables gap
    mov ecx, ebx
    rep movsd

    ; WHY DONE: Kernel now contiguous at 0x100000-0x16FFFF (448KB)

    ; Serial: Kernel copied
    mov esi, msg_serial32_copy
    call serial_print32

    ; WITNESS: Kernel copied to 1MB
    mov esi, evt32_src_copy
    mov edi, evt32_msg_copy
    mov eax, (SEV_MILD << 8) | (DAR_DETECT & 0xFF)
    call store_boot_event32

    ; DEBUG: Show "4:COPY" - kernel copied to high memory
    mov edi, 0xB8000 + 160 * 15         ; Row 15
    mov dword [edi], 0x2F3A2F34         ; '4' ':'
    mov dword [edi+4], 0x2F4F2F43       ; 'C' 'O'
    mov dword [edi+8], 0x2F592F50       ; 'P' 'Y'

    ; Set up paging for long mode (identity map first 4GB)
    call setup_paging

    ; DEBUG: Show "5:PAGE" - page tables set up
    mov edi, 0xB8000 + 160 * 16         ; Row 16
    mov dword [edi], 0x2F3A2F35         ; '5' ':'
    mov dword [edi+4], 0x2F412F50       ; 'P' 'A'
    mov dword [edi+8], 0x2F452F47       ; 'G' 'E'

    ; ─────────────────────────────────────────────────────────────────────────
    ; Enable Long Mode Prerequisites
    ; ─────────────────────────────────────────────────────────────────────────
    ; Long mode requires:
    ;   1. PAE (Physical Address Extension) — larger page table entries
    ;   2. Page tables set up with 4-level hierarchy (done above)
    ;   3. LME (Long Mode Enable) in EFER MSR
    ;   4. Paging enabled in CR0
    ; ─────────────────────────────────────────────────────────────────────────

    ; WHY PAE (bit 5): Physical Address Extension enables 4-level page tables.
    ;      Required for long mode — allows 64-bit virtual addresses.
    ; WHY PGE (bit 7): Page Global Enable — performance optimization.
    ;      Allows marking pages as "global" (not flushed on CR3 reload).
    mov eax, cr4                        ; Read current CR4
    or eax, 0x20 | 0x80                 ; Set PAE (0x20) | PGE (0x80)
    mov cr4, eax                        ; Write back

    ; WHY CR3: Control Register 3 holds the physical address of PML4.
    ;      CPU uses this to find the top-level page table.
    mov eax, PML4_ADDR                  ; PML4 at 0x70000
    mov cr3, eax                        ; Load page table base

    ; WHY EFER: Extended Feature Enable Register (MSR 0xC0000080).
    ;      LME (bit 8) = Long Mode Enable. Must set BEFORE enabling paging.
    ;      After paging + LME, CPU enters long mode.
    mov ecx, 0xC0000080                 ; EFER MSR address
    rdmsr                               ; Read EFER → EDX:EAX
    or eax, 0x100                       ; Set LME (bit 8)
    wrmsr                               ; Write back — LME now enabled

    ; WHY DEBUG MARKER: "6:LME" = Long Mode Enable set in EFER.
    mov edi, 0xB8000 + 160 * 17         ; Row 17
    mov dword [edi], 0x2F3A2F36         ; '6' ':'
    mov dword [edi+4], 0x2F4D2F4C       ; 'L' 'M'
    mov word [edi+8], 0x2F45            ; 'E'

    ; WHY SERIAL: Debug output before the final transition.
    mov esi, msg_serial32_page
    call serial_print32

    ; WHY WITNESS: DAR forensics — about to enable paging and enter long mode.
    mov esi, evt32_src_page
    mov edi, evt32_msg_long
    mov eax, (SEV_MILD << 8) | (DAR_DETECT & 0xFF)
    call store_boot_event32

    ; ─────────────────────────────────────────────────────────────────────────
    ; Enable Paging — This Activates Long Mode!
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY CR0.PG: Paging Enable (bit 31). When set with LME already on,
    ;      CPU transitions from protected mode to long mode.
    ;      This is the actual mode switch moment.
    mov eax, cr0                        ; Read current CR0
    or eax, 0x80000000                  ; Set PG (bit 31)
    mov cr0, eax                        ; NOW IN LONG MODE (compatibility sub-mode)

    ; WHY 64-BIT GDT: We need code segment with L bit (long mode) set.
    ;      32-bit GDT has D bit (32-bit default); 64-bit GDT has L bit.
    lgdt [gdt64_descriptor]             ; Load 64-bit GDT

    ; WHY FAR JUMP: Switch to 64-bit code segment.
    ;      0x08 = 64-bit code segment selector in gdt64.
    ;      This flushes prefetch queue and loads CS with 64-bit descriptor.
    jmp 0x08:long_mode                  ; Jump to 64-bit code!

; ───────────────────────────────────────────────────────────────────────────────
; setup_paging — Identity Map First 4GB for Long Mode
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY PAGING REQUIRED:
;   Long mode (64-bit) REQUIRES paging. CPU won't enter long mode without it.
;   Paging provides virtual→physical address translation.
;
; WHY IDENTITY MAP:
;   Virtual address = Physical address. Code at 0x100000 runs as if at 0x100000.
;   Simplest setup — kernel can set up real page tables later.
;
; WHY 4GB:
;   Map entire 32-bit address space so any address works.
;   4 page directories × 512 entries × 2MB pages = 4GB.
;
; PAGE TABLE HIERARCHY (4-level for long mode):
;   PML4 (Page Map Level 4) → PDPT → PD → (2MB pages, no PT needed)
;   Each table = 4KB (512 × 8-byte entries).
;
; MEMORY LAYOUT:
;   0x70000: PML4 (4KB)
;   0x71000: PDPT (4KB)
;   0x72000: PD[0] (4KB) — maps 0GB-1GB
;   0x73000: PD[1] (4KB) — maps 1GB-2GB
;   0x74000: PD[2] (4KB) — maps 2GB-3GB
;   0x75000: PD[3] (4KB) — maps 3GB-4GB
;

setup_paging:
    ; WHY CLEAR: Start with all zeros. Any non-present entry = page fault.
    mov edi, PML4_ADDR                  ; Start at PML4 (0x70000)
    mov ecx, 4096 * 6 / 4               ; 6 pages × 4KB ÷ 4 bytes = 6144 dwords
    xor eax, eax                        ; Zero
    rep stosd                           ; Clear all page tables

    ; ─────────────────────────────────────────────────────────────────────────
    ; Set up page table hierarchy
    ; Entry format: Address | Flags (bit 0=Present, bit 1=Writable)
    ; ─────────────────────────────────────────────────────────────────────────

    ; WHY PML4[0]: First PML4 entry points to PDPT.
    ;      PML4[0] maps virtual 0x0000_0000_0000 - 0x007F_FFFF_FFFF (512GB)
    mov eax, PDPT_ADDR                  ; PDPT at 0x71000
    or eax, 0x03                        ; Present + Writable
    mov [PML4_ADDR], eax

    ; WHY 4 PDPT ENTRIES: Each PDPT entry maps 1GB via one Page Directory.
    ;      PDPT[0-3] together map 4GB (0x0000_0000 - 0xFFFF_FFFF).

    ; PDPT[0] → PD[0] (maps 0-1GB)
    mov eax, PD_ADDR                    ; PD at 0x72000
    or eax, 0x03                        ; Present + Writable
    mov [PDPT_ADDR], eax

    ; PDPT[1] → PD[1] (maps 1-2GB)
    mov eax, PD_ADDR
    add eax, 0x1000                     ; Next 4KB page = 0x73000
    or eax, 0x03
    mov [PDPT_ADDR + 8], eax            ; Each entry is 8 bytes

    ; PDPT[2] → PD[2] (maps 2-3GB)
    mov eax, PD_ADDR
    add eax, 0x2000                     ; 0x74000
    or eax, 0x03
    mov [PDPT_ADDR + 16], eax

    ; PDPT[3] → PD[3] (maps 3-4GB)
    mov eax, PD_ADDR
    add eax, 0x3000                     ; 0x75000
    or eax, 0x03
    mov [PDPT_ADDR + 24], eax

    ; ─────────────────────────────────────────────────────────────────────────
    ; Fill Page Directory entries with 2MB pages (no PT level needed)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY 2MB PAGES: Bit 7 (PS) = Page Size = 1 means 2MB page, no PT needed.
    ;      This simplifies setup. 512 × 2MB = 1GB per PD.
    ;      4 PDs × 512 entries = 2048 entries × 2MB = 4GB mapped.

    mov edi, PD_ADDR                    ; Start at PD[0] (0x72000)
    mov eax, 0x83                       ; 0x83 = Present(1) + RW(2) + PS(0x80)
    mov ecx, 512 * 4                    ; 512 entries × 4 page directories

.pd_loop:
    mov [edi], eax                      ; Write PD entry
    add eax, 0x200000                   ; Next 2MB physical address
    add edi, 8                          ; Next 8-byte entry
    loop .pd_loop

    ret

; ───────────────────────────────────────────────────────────────────────────────
; serial_print32 / serial_char32 — Serial Output (32-bit Protected Mode)
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY SERIAL OUTPUT:
;   Debug breadcrumbs visible on serial console (QEMU -serial stdio).
;   Works even when video is broken. Essential for debugging mode transitions.
;
; WHY DIRECT PORT I/O:
;   No BIOS available in protected mode — must talk to hardware directly.
;   COM1 = 0x3F8 base, standard PC serial port since IBM PC.
;
; WHY POLLING (not interrupts):
;   We're in a transition phase — interrupt handlers not set up yet.
;   Polling is simple and reliable for debug output.
;

; ───────────────────────────────────────────────────────────────────────────────
; serial_print32 — Print null-terminated string to COM1 (32-bit)
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Iterate string, send each character to serial port.
;
; @param  ESI  Pointer to null-terminated string
;
; @return None (preserves all registers)
;
serial_print32:
    push eax                            ; WHY: Preserve caller's registers
    push edx                            ;      (we modify AL and DX)
    push esi                            ;      (LODSB modifies ESI)
.loop32:
    lodsb                               ; WHY: Load byte at [ESI] into AL, ESI++
    test al, al                         ; WHY: Check for null terminator (0)
    jz .done32                          ;      NULL → end of string
    call serial_char32                  ; WHY: Send character in AL to COM1
    jmp .loop32                         ;      Continue to next character
.done32:
    pop esi                             ; WHY: Restore in reverse order (LIFO)
    pop edx
    pop eax
    ret

; ───────────────────────────────────────────────────────────────────────────────
; serial_char32 — Print single character to COM1 (32-bit)
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Wait for transmit buffer empty, then send character.
;
; @param  AL  Character to send
;
; @return None (preserves all registers)
;
; WHY WAIT FOR THRE:
;   UART can only send one byte at a time. If we write too fast,
;   characters get lost. Line Status Register bit 5 (THRE = Transmit
;   Holding Register Empty) tells us when it's safe to write.
;
serial_char32:
    push edx                            ; WHY: Preserve DX (we use it for port)
    push eax                            ; WHY: Preserve AL (we read LSR into it)
    ; WHY 0x3FD: COM1 Line Status Register = base (0x3F8) + 5
    mov dx, 0x3FD                       ; Line Status Register
.wait32:
    in al, dx                           ; WHY: Read LSR into AL
    ; WHY 0x20: Bit 5 = THRE (Transmit Holding Register Empty)
    ;      When set, UART is ready for next byte.
    test al, 0x20                       ; THRE bit set?
    jz .wait32                          ; No → keep polling
    pop eax                             ; WHY: Restore AL (character to send)
    ; WHY 0x3F8: COM1 Data Register (THR = Transmit Holding Register)
    mov dx, 0x3F8                       ; Data Register
    out dx, al                          ; WHY: Write character to UART
    pop edx                             ; WHY: Restore DX
    ret

; ───────────────────────────────────────────────────────────────────────────────
; store_boot_event32 — DAR Witness Event Recording (32-bit Protected Mode)
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY DAR WITNESS:
;   Detect-Assess-Restore paradigm for boot forensics.
;   Records events during boot so kernel can analyze what happened.
;   Critical for debugging when display isn't available.
;
; WHY BUFFER AT 0x1E00:
;   Boot event buffer lives at BOOT_INFO_ADDR + 0x1E00.
;   This puts it after the memory map in MosBootInfo structure.
;   Kernel reads this buffer to import boot events into DAR witness.
;
; BUFFER STRUCTURE (header at offset 0):
;   +0x00: magic (4 bytes) = "BEVT"
;   +0x04: count (4 bytes) = number of events stored
;   +0x08: sequence (4 bytes) = next sequence number
;   +0x0C: reserved (4 bytes)
;   +0x10: events start (32 bytes each)
;
; EVENT STRUCTURE (32 bytes each):
;   +0x00: sequence (4 bytes)
;   +0x04: phase (1 byte) - DAR phase (DETECT/ASSESS/RESTORE)
;   +0x05: severity (1 byte) - MILD/MEDIUM/CRITICAL
;   +0x06: health_delta (1 byte)
;   +0x07: src_len (1 byte)
;   +0x08: source (8 bytes) - component name
;   +0x10: message (16 bytes) - event description
;
; @param  ESI  Pointer to source string (8 chars max)
; @param  EDI  Pointer to message string (16 chars max)
; @param  EAX  Low byte (AL) = DAR phase, high byte (AH) = severity
;
; @return None (preserves all registers)
;

store_boot_event32:
    ; WHY STACK FRAME: Standard function prologue for debugging.
    push ebp
    mov ebp, esp
    ; WHY PUSH ALL: Preserve all registers we'll modify.
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi

    ; WHY SAVE EAX: Contains phase/severity, but we'll overwrite EAX.
    push eax                              ; Save phase/severity for later

    ; ─────────────────────────────────────────────────────────────────────────
    ; Check if buffer full
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY HARDCODE: In protected mode, we use linear addresses.
    ;      0x9000 = BOOT_INFO_ADDR, + 0x1E00 = event buffer offset.
    mov ebx, 0x9000 + 0x1E00              ; EBX = buffer base address
    mov eax, [ebx + 4]                    ; EAX = current event count
    cmp eax, BOOT_EVENT_MAX               ; Room for more events?
    jae .event_full32                     ; No → skip recording

    ; ─────────────────────────────────────────────────────────────────────────
    ; Calculate slot address: buffer + 16 + (count * 32)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY +16: Skip the 16-byte header to reach events array.
    ; WHY *32: Each event is 32 bytes.
    mov ecx, eax                          ; ECX = count
    shl ecx, 5                            ; ECX = count * 32 (shift left 5 = ×32)
    add ecx, ebx                          ; ECX = buffer + (count * 32)
    add ecx, 16                           ; ECX = event slot address

    ; ─────────────────────────────────────────────────────────────────────────
    ; Store sequence number (auto-incrementing)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY SEQUENCE: Provides ordering even if events are processed out of order.
    mov eax, [ebx + 8]                    ; EAX = current sequence
    mov [ecx + 0], eax                    ; Store in event slot
    inc eax                               ; Increment for next event
    mov [ebx + 8], eax                    ; Update header's sequence counter

    ; ─────────────────────────────────────────────────────────────────────────
    ; Store phase and severity
    ; ─────────────────────────────────────────────────────────────────────────
    pop eax                               ; Restore saved phase/severity
    mov byte [ecx + 4], al                ; Phase (DETECT/ASSESS/RESTORE)
    mov byte [ecx + 5], ah                ; Severity (MILD/MEDIUM/CRITICAL)
    mov byte [ecx + 6], 0                 ; Health delta = 0 (not computed yet)
    mov byte [ecx + 7], 0                 ; Source length (computed later)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Copy source string (ESI → event+8, max 8 bytes)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY FIXED SIZE: Source field is exactly 8 bytes, pad with zeros.
    push ecx                              ; Save event base
    add ecx, 8                            ; Point to source field
    mov edx, 8                            ; Counter: 8 bytes max
.copy_src32:
    lodsb                                 ; AL = [ESI++]
    test al, al                           ; Null terminator?
    jz .pad_src32                         ; Yes → pad rest with zeros
    mov [ecx], al                         ; Store character
    inc ecx
    dec edx
    jnz .copy_src32                       ; Continue if not 8 bytes yet
    jmp .src_done32
.pad_src32:
    mov byte [ecx], 0                     ; Zero-pad remaining bytes
    inc ecx
    dec edx
    jnz .pad_src32
.src_done32:
    pop ecx                               ; Restore event base

    ; ─────────────────────────────────────────────────────────────────────────
    ; Copy message string (EDI → event+16, max 16 bytes)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY FIXED SIZE: Message field is exactly 16 bytes, pad with zeros.
    push ecx                              ; Save event base
    add ecx, 16                           ; Point to message field
    mov esi, edi                          ; ESI = message pointer (was in EDI)
    mov edx, 16                           ; Counter: 16 bytes max
.copy_msg32:
    lodsb                                 ; AL = [ESI++]
    test al, al                           ; Null terminator?
    jz .pad_msg32                         ; Yes → pad rest with zeros
    mov [ecx], al                         ; Store character
    inc ecx
    dec edx
    jnz .copy_msg32                       ; Continue if not 16 bytes yet
    jmp .msg_done32
.pad_msg32:
    mov byte [ecx], 0                     ; Zero-pad remaining bytes
    inc ecx
    dec edx
    jnz .pad_msg32
.msg_done32:
    pop ecx                               ; Restore event base

    ; ─────────────────────────────────────────────────────────────────────────
    ; Increment event count
    ; ─────────────────────────────────────────────────────────────────────────
    mov ebx, 0x9000 + 0x1E00              ; Buffer base
    mov eax, [ebx + 4]                    ; Current count
    inc eax                               ; count++
    mov [ebx + 4], eax                    ; Store updated count
    jmp .done32_event

.event_full32:
    ; WHY POP: Clean up stack — we pushed EAX earlier but didn't use it.
    pop eax

.done32_event:
    ; WHY RESTORE IN REVERSE: LIFO order — last pushed = first popped.
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret

; 32-bit serial message strings (in code section for simplicity)
msg_serial32_entry: db "[MOS-BIOS] 32-bit protected mode", 13, 10, 0
msg_serial32_copy:  db "[MOS-BIOS] Kernel copied to 1MB", 13, 10, 0
msg_serial32_page:  db "[MOS-BIOS] Paging enabled, entering long mode", 13, 10, 0

; 32-bit event strings
evt32_src_prot:     db "PMODE32", 0
evt32_src_copy:     db "COPY", 0
evt32_src_page:     db "PAGE", 0
evt32_msg_entry:    db "Prot mode OK", 0
evt32_msg_copy:     db "Kernel @1MB", 0
evt32_msg_page:     db "Page tables OK", 0
evt32_msg_long:     db "Long mode prep", 0

; ───────────────────────────────────────────────────────────────────────────────
; B.2e LONG MODE — 64-bit Execution (Final Boot Stage)
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY 64-BIT LONG MODE:
;   The destination. Modern CPUs are 64-bit. This gives us:
;   - Full 64-bit registers (RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, R8-R15)
;   - Expanded address space (256TB with 4-level paging)
;   - Better calling conventions (parameters in registers)
;   - Required for MillenniumOS kernel
;
; CPU PATH COMPLETE:
;   16-bit Real → 32-bit Protected → 64-bit Long Mode ✓
;

bits 64                                 ; WHY: Tell assembler to emit 64-bit code

long_mode:
    ; ═══════════════════════════════════════════════════════════════════════
    ; 64-BIT LONG MODE - We made it!
    ; ═══════════════════════════════════════════════════════════════════════
    ;
    ; "And God said, Let there be light: and there was light." — Genesis 1:3
    ;
    ; This is the moment of creation — the OS begins in 64-bit mode.
    ; We show a visual confirmation before handing off to the kernel.
    ;
    ; ═══════════════════════════════════════════════════════════════════════

    ; ─────────────────────────────────────────────────────────────────────────
    ; Set up 64-bit segment registers
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY 0x10: GDT entry 2 (index × 8) = 64-bit data segment.
    ;      In long mode, segment bases are ignored (except FS/GS for TLS).
    ;      But we still need valid selectors for CPU to not fault.
    mov ax, 0x10                        ; 64-bit data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; WHY STACK: Kernel will set up its own stack, but we need a temporary
    ;      one for function calls (serial_print64, store_boot_event64).
    ;      0x90000 is safe — above kernel load area, below 1MB.
    mov rsp, 0x90000                    ; Temporary stack at 576KB

    ; WHY SERIAL: Debug output — we're in 64-bit mode!
    mov rsi, msg_serial64_long
    call serial_print64

    ; WHY WITNESS: DAR forensics — record successful 64-bit entry.
    mov rsi, evt64_src_long
    mov rdi, evt64_msg_entry
    mov eax, (SEV_MILD << 8) | (DAR_DETECT & 0xFF)
    call store_boot_event64

    ; WHY DEBUG MARKER: "7:64BIT" = We're in 64-bit long mode!
    ;      White on red (0x4F) = IMPORTANT milestone.
    mov rdi, 0xB8000 + 160 * 18         ; Row 18 (linear address — no segments)
    mov dword [rdi], 0x4F3A4F37         ; '7' ':'
    mov dword [rdi+4], 0x4F344F36       ; '6' '4'
    mov dword [rdi+8], 0x4F494F42       ; 'B' 'I'
    mov word [rdi+12], 0x4F54           ; 'T'

    ; WHY PAUSE: Let the debug marker be visible briefly.
    ;      Without pause, we'd immediately jump to kernel and miss it.
    mov rcx, 10000000                   ; Delay loop count
.pause7:
    dec rcx
    jnz .pause7

    ; ═══════════════════════════════════════════════════════════════════════
    ; VISUAL LANDING SPOT — Framebuffer Test
    ; ═══════════════════════════════════════════════════════════════════════
    ;
    ; WHY VISUAL TEST:
    ;   Verify framebuffer works before kernel takes over.
    ;   Fill top 100 lines with Kingdom blue — visible proof of success.
    ;   If VESA failed (text mode), skip this section.
    ;
    ; WHY KINGDOM BLUE:
    ;   0xFF0A1628 = BGRA (Blue, Green, Red, Alpha)
    ;   This is the CreativeWorkzStudio signature color.
    ;

    ; WHY CHECK FB ADDRESS: If fb_addr is 0 or 0xB8000, we're in text mode.
    mov rsi, BOOT_INFO_ADDR
    mov rdi, [rsi + 0x40]               ; fb->address (64-bit)
    test rdi, rdi                       ; Is address 0?
    jz .skip_visual                     ; Yes → skip (no framebuffer)

    ; WHY LOAD DIMENSIONS: Need width for pixel count, pitch for line advance.
    mov r8d, [rsi + 0x48]               ; width (pixels per line)
    mov r9d, [rsi + 0x50]               ; pitch (bytes per scanline)

    ; WHY KINGDOM BLUE: 0xFF0A1628 = full alpha, dark blue (BGRA format).
    mov edx, 0xFF0A1628                 ; Pixel color
    mov r10, 100                        ; Number of lines to fill

.fill_lines:
    ; WHY LINE-BY-LINE: Pitch may differ from (width × 4) due to alignment.
    ;      Can't just fill width × height pixels contiguously.
    mov rcx, r8                         ; Pixel count = width
    push rdi                            ; Save line start address

.fill_pixels:
    mov [rdi], edx                      ; Write 4-byte pixel
    add rdi, 4                          ; Advance 4 bytes (32bpp = 4 bytes)
    dec rcx
    jnz .fill_pixels

    ; WHY PITCH: Pitch is bytes per scanline (may include padding).
    ;      Next line = current line + pitch, not + (width × 4).
    pop rdi                             ; Restore line start
    add rdi, r9                         ; Advance by pitch to next line
    dec r10                             ; Decrement line count
    jnz .fill_lines

.skip_visual:

    ; ═══════════════════════════════════════════════════════════════════════
    ; HANDOFF TO KERNEL — The Final Transition
    ; ═══════════════════════════════════════════════════════════════════════
    ;
    ; WHY THIS IS THE END:
    ;   Bootloader's job is done. Kernel takes control.
    ;   We've set up: memory map, framebuffer, page tables, 64-bit mode.
    ;   All that info is in MosBootInfo at BOOT_INFO_ADDR.
    ;
    ; WHY RDI: System V AMD64 ABI — first argument passed in RDI.
    ;   Kernel entry: void kernel_main(MosBootInfo* boot_info)
    ;

    ; WHY SERIAL: Last debug output before kernel.
    mov rsi, msg_serial64_jump
    call serial_print64

    ; WHY WITNESS: Final boot event — "about to jump to kernel."
    ;   Kernel will import this event buffer into its witness system.
    mov rsi, evt64_src_kern
    mov rdi, evt64_msg_jump
    mov eax, (SEV_MILD << 8) | (DAR_DETECT & 0xFF)
    call store_boot_event64

    ; WHY DEBUG MARKER: "8:JUMP" = about to transfer control.
    ;   Yellow on red (0x4E) = final milestone before kernel.
    mov rdi, 0xB8000 + 160 * 19         ; Row 19
    mov dword [rdi], 0x4E3A4E38         ; '8' ':'
    mov dword [rdi+4], 0x4E554E4A       ; 'J' 'U'
    mov dword [rdi+8], 0x4E504E4D       ; 'M' 'P'

    ; WHY RDI = BOOT_INFO: Kernel expects boot info pointer in RDI.
    ;   This is the ONLY parameter we pass to kernel_main().
    mov rdi, BOOT_INFO_ADDR

    ; ─────────────────────────────────────────────────────────────────────────
    ; "Let there be light" — Kernel speaks and creation continues.
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY JMP (not CALL): We're not coming back. Kernel owns the machine now.
    ;   Using CALL would push return address we'll never use.
    mov rax, KERNEL_LOAD_ADDR           ; Kernel at 0x100000 (1MB)
    jmp rax                             ; Transfer control to kernel!

; ───────────────────────────────────────────────────────────────────────────────
; serial_print64 / serial_char64 — Serial Output (64-bit Long Mode)
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY 64-BIT VERSION:
;   Same logic as 32-bit, but uses 64-bit registers (RSI, RAX, RDX).
;   Required because 64-bit mode has different register sizes.
;
; WHY DIRECT PORT I/O:
;   No BIOS in long mode. Talk to UART hardware directly via ports.
;   COM1 base = 0x3F8, LSR = 0x3FD.
;

; ───────────────────────────────────────────────────────────────────────────────
; serial_print64 — Print null-terminated string to COM1 (64-bit)
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Iterate string, send each character to serial port.
;
; @param  RSI  Pointer to null-terminated string
;
; @return None (preserves all registers)
;
serial_print64:
    push rax                            ; WHY: Preserve caller's RAX
    push rdx                            ;      We use DX for port number
    push rsi                            ;      LODSB modifies RSI
.loop64:
    lodsb                               ; WHY: Load byte [RSI] → AL, RSI++
    test al, al                         ; WHY: Check for null terminator
    jz .done64                          ;      Zero → end of string
    call serial_char64                  ; WHY: Send character in AL
    jmp .loop64                         ;      Continue to next
.done64:
    pop rsi                             ; WHY: Restore in reverse order
    pop rdx
    pop rax
    ret

; ───────────────────────────────────────────────────────────────────────────────
; serial_char64 — Print single character to COM1 (64-bit)
; ───────────────────────────────────────────────────────────────────────────────
;
; @brief  Wait for THRE, then send character.
;
; @param  AL  Character to send
;
; @return None (preserves all registers)
;
serial_char64:
    push rdx                            ; WHY: Preserve RDX (we use DX)
    push rax                            ; WHY: Preserve RAX (we read into AL)
    ; WHY 0x3FD: Line Status Register = COM1 base + 5
    mov dx, 0x3FD                       ; LSR port
.wait64:
    in al, dx                           ; WHY: Read LSR
    ; WHY 0x20: Bit 5 = THRE (Transmit Holding Register Empty)
    test al, 0x20                       ; Ready to transmit?
    jz .wait64                          ; No → keep polling
    pop rax                             ; WHY: Restore character to send
    ; WHY 0x3F8: Data Register (THR)
    mov dx, 0x3F8                       ; THR port
    out dx, al                          ; WHY: Send character
    pop rdx                             ; WHY: Restore RDX
    ret

; 64-bit serial message strings
msg_serial64_long:  db "[MOS-BIOS] 64-bit long mode", 13, 10, 0
msg_serial64_jump:  db "[MOS-BIOS] Jumping to kernel at 0x100000", 13, 10, 0

; ───────────────────────────────────────────────────────────────────────────────
; store_boot_event64 — DAR Witness Event Recording (64-bit Long Mode)
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY 64-BIT VERSION:
;   Same logic as 32-bit, but uses 64-bit registers (RSI, RDI, RBX, RCX).
;   Required because long mode uses 64-bit register operations.
;
; WHY SAME BUFFER:
;   Uses same buffer as 16-bit and 32-bit versions.
;   All boot events end up in one place for kernel to import.
;
; BUFFER LOCATION:
;   BOOT_INFO_ADDR (0x9000) + 0x1E00 = 0xAE00
;
; @param  RSI  Pointer to source string (8 chars max)
; @param  RDI  Pointer to message string (16 chars max)
; @param  EAX  Low byte (AL) = DAR phase, high byte (AH) = severity
;
; @return None (preserves all registers)
;

BOOT_EVENT_BUFFER64 equ 0x9000 + 0x1E00

store_boot_event64:
    ; WHY STACK FRAME: Standard function prologue.
    push rbp
    mov rbp, rsp
    ; WHY PUSH ALL: Preserve all registers we'll modify.
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8                               ; WHY R8: Extra register to save phase/severity

    ; WHY SAVE TO R8: We need EAX for other operations.
    ;      R8D preserves the phase (AL) and severity (AH) values.
    mov r8d, eax

    ; ─────────────────────────────────────────────────────────────────────────
    ; Check if buffer full
    ; ─────────────────────────────────────────────────────────────────────────
    mov rbx, BOOT_EVENT_BUFFER64          ; RBX = buffer base
    mov eax, [rbx + 4]                    ; EAX = event count
    cmp eax, BOOT_EVENT_MAX               ; Room for more?
    jae .event_full64                     ; No → skip

    ; ─────────────────────────────────────────────────────────────────────────
    ; Calculate slot: buffer + 16 + (count * 32)
    ; ─────────────────────────────────────────────────────────────────────────
    mov ecx, eax                          ; ECX = count
    shl ecx, 5                            ; ECX = count * 32
    add rcx, rbx                          ; RCX = buffer + (count * 32)
    add rcx, 16                           ; RCX = event slot address

    ; ─────────────────────────────────────────────────────────────────────────
    ; Store sequence number
    ; ─────────────────────────────────────────────────────────────────────────
    mov eax, [rbx + 8]                    ; EAX = current sequence
    mov [rcx + 0], eax                    ; Store in event
    inc eax                               ; sequence++
    mov [rbx + 8], eax                    ; Update header

    ; ─────────────────────────────────────────────────────────────────────────
    ; Store phase and severity (from R8D)
    ; ─────────────────────────────────────────────────────────────────────────
    mov eax, r8d                          ; Restore phase/severity
    mov byte [rcx + 4], al                ; Phase
    mov byte [rcx + 5], ah                ; Severity
    mov byte [rcx + 6], 0                 ; Health delta = 0
    mov byte [rcx + 7], 0                 ; Source length (unused)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Copy source string (RSI → event+8, max 8 bytes)
    ; ─────────────────────────────────────────────────────────────────────────
    push rcx                              ; Save event base
    add rcx, 8                            ; Point to source field
    mov rdx, 8                            ; 8 bytes max
.copy_src64:
    lodsb                                 ; AL = [RSI++]
    test al, al                           ; Null?
    jz .pad_src64                         ; Yes → pad
    mov [rcx], al                         ; Store char
    inc rcx
    dec rdx
    jnz .copy_src64
    jmp .src_done64
.pad_src64:
    mov byte [rcx], 0                     ; Zero-pad
    inc rcx
    dec rdx
    jnz .pad_src64
.src_done64:
    pop rcx                               ; Restore event base

    ; ─────────────────────────────────────────────────────────────────────────
    ; Copy message (RDI → event+16, max 16 bytes)
    ; ─────────────────────────────────────────────────────────────────────────
    push rcx                              ; Save event base
    add rcx, 16                           ; Point to message field
    mov rsi, rdi                          ; RSI = message pointer
    mov rdx, 16                           ; 16 bytes max
.copy_msg64:
    lodsb                                 ; AL = [RSI++]
    test al, al                           ; Null?
    jz .pad_msg64                         ; Yes → pad
    mov [rcx], al                         ; Store char
    inc rcx
    dec rdx
    jnz .copy_msg64
    jmp .msg_done64
.pad_msg64:
    mov byte [rcx], 0                     ; Zero-pad
    inc rcx
    dec rdx
    jnz .pad_msg64
.msg_done64:
    pop rcx                               ; Restore event base

    ; ─────────────────────────────────────────────────────────────────────────
    ; Increment event count
    ; ─────────────────────────────────────────────────────────────────────────
    mov rbx, BOOT_EVENT_BUFFER64          ; Buffer base
    mov eax, [rbx + 4]                    ; Current count
    inc eax                               ; count++
    mov [rbx + 4], eax                    ; Store
    jmp .done64_event

.event_full64:
    ; WHY EMPTY: Buffer full — nothing to clean up (didn't push extra).

.done64_event:
    ; WHY REVERSE ORDER: LIFO — pop in reverse of push order.
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
    ret

; 64-bit event strings
evt64_src_long:     db "LMODE64", 0
evt64_src_kern:     db "KERNEL", 0
evt64_msg_entry:    db "64-bit mode OK", 0
evt64_msg_jump:     db "Kernel jump", 0

; ═══════════════════════════════════════════════════════════════════════════════
; B.3 HELPERS — Utility Subroutines (16-bit Real Mode) [HELPERS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; WHY SEPARATE SECTION:
;   Helpers are the HOW — they support the WHAT (core ops).
;   Reader understands WHAT the code does (B.2) before seeing HOW utilities work.
;
; SUBSECTIONS:
;   - B.3a UI — Display functions (clear_screen, print_centered, print_at)
;   - B.3b SERIAL — Debug output (serial_print, serial_char)
;   - B.3c WITNESS — DAR event recording (store_boot_event)
;
; NOTE: 32-bit and 64-bit helpers are located near their respective mode
;       sections (B.2d and B.2e) for locality of reference.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.3a UI — Display Helper Functions (16-bit)
; ───────────────────────────────────────────────────────────────────────────────

bits 16

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ clear_screen — Fill screen with blue background                             │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY THIS EXISTS:
;   Provides a clean visual slate before drawing boot UI.
;   Blue background (0x17) is classic BIOS setup style.
;
; WHY DIRECT VIDEO MEMORY:
;   Using rep stosw to video memory is MUCH faster than BIOS INT 10h
;   (which would require 2000 separate calls). Real mode has direct
;   access to video memory at segment 0xB800.
;
; PARAMETERS: None.
; RETURNS: Nothing (screen cleared).
; CLOBBERS: None (pusha/popa preserve all registers).
;
clear_screen:
    pusha                               ; WHY: Preserve all general registers
    push es                             ;      ES will point to video memory

    ; WHY 0xB800: VGA text mode video memory starts here
    ;   Each character = 2 bytes: [char][attribute]
    ;   Screen = 80x25 = 2000 characters = 4000 bytes
    mov ax, VIDEO_MEM                   ; VIDEO_MEM = 0xB800
    mov es, ax                          ; ES → video segment
    xor di, di                          ; DI = 0 (start of video memory)

    ; WHY 0x1720: Attribute byte 0x17 = white on blue (classic BIOS)
    ;   High nibble 1 = blue background
    ;   Low nibble 7 = white foreground (actually light gray)
    ;   Character 0x20 = space (blank)
    mov cx, SCREEN_WIDTH * SCREEN_HEIGHT ; 80 * 25 = 2000 words
    mov ax, 0x1720                      ; AH=attr (0x17), AL=char (space)
    rep stosw                           ; WHY REP STOSW: Write AX to [ES:DI], DI+=2, CX--
                                        ;   Fills entire screen in one instruction

    pop es                              ; WHY: Restore ES to caller's segment
    popa                                ;      Restore all registers
    ret

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ print_centered — Print string centered horizontally on specified row        │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY THIS EXISTS:
;   Boot UI needs centered text for titles, messages, prompts.
;   Calculating center manually for each string is tedious and error-prone.
;
; ALGORITHM:
;   1. Measure string length (scan for null terminator)
;   2. Calculate column: (80 - length) / 2
;   3. Delegate to print_at for actual rendering
;
; PARAMETERS:
;   DH = row (0-24)
;   SI = pointer to null-terminated string
;   BL = attribute byte (foreground/background colors)
;
; RETURNS: Nothing.
; CLOBBERS: None (pusha/popa preserve all registers).
;
print_centered:
    pusha                               ; WHY: Preserve all registers for caller

    ; ─────────────────────────────────────────────────────────────────────────
    ; PHASE 1: Measure string length
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY MEASURE FIRST: Need length to calculate center position.
    ;   Can't center without knowing how wide the string is.
    push si                             ; WHY: Save SI — we'll need it again for print_at
    xor cx, cx                          ; CX = length counter (starts at 0)
.len:
    lodsb                               ; WHY LODSB: Load [SI] into AL, SI++
    test al, al                         ; WHY TEST: Check for null terminator (faster than CMP)
    jz .len_done                        ;   NULL → end of string
    inc cx                              ;   Otherwise, increment length
    jmp .len                            ;   Continue scanning
.len_done:
    pop si                              ; WHY: Restore SI to start of string

    ; ─────────────────────────────────────────────────────────────────────────
    ; PHASE 2: Calculate center column
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY THIS FORMULA: (screen_width - string_length) / 2
    ;   This gives equal padding on both sides.
    ;   Example: 80-column screen, 10-char string → (80-10)/2 = 35 → start at col 35
    mov ax, SCREEN_WIDTH                ; AX = 80
    sub ax, cx                          ; AX = 80 - length = remaining space
    shr ax, 1                           ; WHY SHR: Divide by 2 (shift right = fast divide)
    mov dl, al                          ; DL = starting column for print_at

    ; ─────────────────────────────────────────────────────────────────────────
    ; PHASE 3: Delegate to print_at
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY DELEGATE: print_at does the actual rendering.
    ;   DRY principle — don't duplicate the video memory logic.
    call print_at                       ; DH=row, DL=col, SI=string, BL=attr
    popa                                ; WHY: Restore all registers
    ret

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ print_at — Print string at specific row and column                          │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY THIS EXISTS:
;   Core text rendering primitive for boot UI.
;   Writes directly to video memory (faster than BIOS INT 10h).
;
; ALGORITHM:
;   1. Calculate video memory offset from row/col
;   2. Loop: write each char + attribute word
;   3. Stop at null terminator
;
; VIDEO MEMORY LAYOUT:
;   Address = 0xB800:0 + (row * 80 + col) * 2
;   Each character position = 2 bytes: [char byte][attribute byte]
;
; PARAMETERS:
;   DH = row (0-24)
;   DL = column (0-79)
;   SI = pointer to null-terminated string
;   BL = attribute byte (foreground/background colors)
;
; RETURNS: Nothing.
; CLOBBERS: None (pusha/popa preserve all registers).
;
print_at:
    pusha                               ; WHY: Preserve all registers for caller
    push es                             ;      ES will point to video memory

    ; ─────────────────────────────────────────────────────────────────────────
    ; Calculate video memory offset
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY THIS FORMULA: offset = (row * 80 + col) * 2
    ;   Row * 80 = position at start of row (80 chars per row)
    ;   + col = position within row
    ;   * 2 = each position is 2 bytes (char + attr)
    mov ax, VIDEO_MEM                   ; VIDEO_MEM = 0xB800
    mov es, ax                          ; WHY ES: STOSW writes to [ES:DI]
    movzx ax, dh                        ; WHY MOVZX: Zero-extend DH (row) to AX
    mov cx, SCREEN_WIDTH                ; CX = 80
    mul cx                              ; AX = row * 80 (DX:AX, but fits in AX)
    movzx cx, dl                        ; WHY: Zero-extend DL (col) to CX
    add ax, cx                          ; AX = row * 80 + col
    shl ax, 1                           ; WHY SHL: Multiply by 2 (each char = 2 bytes)
    mov di, ax                          ; DI = video memory offset

    ; ─────────────────────────────────────────────────────────────────────────
    ; Write string loop
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY AH=BL: STOSW writes AX (AL=char, AH=attr)
    ;   By keeping AH constant, each character gets same attribute.
    mov ah, bl                          ; AH = attribute (stays constant)
.loop:
    lodsb                               ; WHY LODSB: Load [SI] into AL, SI++
    test al, al                         ; WHY TEST: Check for null terminator
    jz .done                            ;   NULL → end of string
    stosw                               ; WHY STOSW: Write AX (char+attr) to [ES:DI], DI+=2
    jmp .loop                           ;   Continue to next character
.done:
    pop es                              ; WHY: Restore ES to caller's segment
    popa                                ;      Restore all registers
    ret

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ print_char_at — Print single character at specific row and column           │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY THIS EXISTS:
;   Sometimes need to draw single characters (box corners, status indicators).
;   More efficient than print_at for single chars.
;
; PARAMETERS:
;   DH = row (0-24)
;   DL = column (0-79)
;   AL = character to print
;   AH = attribute byte (foreground/background colors)
;
; RETURNS: Nothing.
; CLOBBERS: None (pusha/popa preserve all registers).
;
print_char_at:
    pusha                               ; WHY: Preserve all registers
    push es                             ;      ES will point to video memory

    ; WHY SAVE AX FIRST: We need AX for address calculation,
    ;   but it also contains our char+attr to write.
    mov bx, ax                          ; BX = save char (AL) + attr (AH)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Calculate video memory offset (same formula as print_at)
    ; ─────────────────────────────────────────────────────────────────────────
    mov ax, VIDEO_MEM                   ; VIDEO_MEM = 0xB800
    mov es, ax                          ; ES → video segment
    movzx ax, dh                        ; AX = row (zero-extended)
    mov cx, SCREEN_WIDTH                ; CX = 80
    mul cx                              ; AX = row * 80
    movzx cx, dl                        ; CX = col (zero-extended)
    add ax, cx                          ; AX = row * 80 + col
    shl ax, 1                           ; AX = (row * 80 + col) * 2
    mov di, ax                          ; DI = video memory offset

    ; ─────────────────────────────────────────────────────────────────────────
    ; Write single character with attribute
    ; ─────────────────────────────────────────────────────────────────────────
    mov ax, bx                          ; WHY: Restore char+attr from BX
    stosw                               ; WHY STOSW: Write AX to [ES:DI]

    pop es                              ; WHY: Restore ES
    popa                                ;      Restore all registers
    ret

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ print_string_teletype — Print string using BIOS teletype mode               │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY THIS EXISTS:
;   BIOS teletype mode handles cursor advancement and scrolling automatically.
;   Useful for sequential output (like debug messages) where you don't need
;   precise positioning.
;
; WHY USE BIOS (INT 10h):
;   - Automatic cursor tracking (no need to manage cursor position)
;   - Automatic scrolling when screen fills
;   - Handles newlines (\n) and carriage returns (\r)
;   - Works regardless of video mode details
;
; TRADE-OFF:
;   Slower than direct video memory access, but more convenient for
;   sequential/debug output.
;
; PARAMETERS:
;   SI = pointer to null-terminated string
;
; RETURNS: Nothing.
; CLOBBERS: None (pusha/popa preserve all registers).
;
print_string_teletype:
    pusha                               ; WHY: Preserve all registers
.loop:
    lodsb                               ; WHY LODSB: Load [SI] into AL, SI++
    test al, al                         ; WHY TEST: Check for null terminator
    jz .done                            ;   NULL → end of string
    mov ah, 0x0E                        ; WHY 0x0E: BIOS teletype function
                                        ;   - Writes char at cursor
                                        ;   - Advances cursor
                                        ;   - Handles scrolling
    int 0x10                            ; WHY INT 10h: BIOS video services
    jmp .loop                           ;   Continue to next character
.done:
    popa                                ; WHY: Restore all registers
    ret

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ print_dec — Print unsigned decimal number using BIOS teletype               │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY THIS EXISTS:
;   Display numeric values (memory sizes, addresses, counts) in human-readable
;   decimal format during boot.
;
; ALGORITHM:
;   1. Repeatedly divide by 10, pushing remainders onto stack
;   2. Pop and print digits (LIFO reverses order → most significant first)
;
; WHY STACK FOR REVERSAL:
;   Division gives us digits in REVERSE order (least significant first).
;   Example: 123 ÷ 10 → 12 remainder 3, then 1 remainder 2, then 0 remainder 1
;   Stack (LIFO) reverses: push 3,2,1 → pop 1,2,3 → prints "123"
;
; PARAMETERS:
;   AX = unsigned 16-bit number to print (0-65535)
;
; RETURNS: Nothing.
; CLOBBERS: None (pusha/popa preserve all registers).
;
print_dec:
    pusha                               ; WHY: Preserve all registers
    mov bx, 10                          ; WHY 10: Decimal base for division
    xor cx, cx                          ; WHY: CX = digit counter (starts at 0)

    ; ─────────────────────────────────────────────────────────────────────────
    ; PHASE 1: Extract digits (right to left)
    ; ─────────────────────────────────────────────────────────────────────────
.divide:
    xor dx, dx                          ; WHY: Clear DX for 32-bit dividend (DX:AX)
    div bx                              ; WHY DIV: AX = quotient, DX = remainder
                                        ;   remainder (0-9) is this digit
    push dx                             ; WHY PUSH: Save digit for later printing
    inc cx                              ; WHY: Count digits (for loop .print)
    test ax, ax                         ; WHY TEST: More digits? (quotient ≠ 0)
    jnz .divide                         ;   Yes → continue dividing

    ; ─────────────────────────────────────────────────────────────────────────
    ; PHASE 2: Print digits (left to right via stack reversal)
    ; ─────────────────────────────────────────────────────────────────────────
.print:
    pop dx                              ; WHY POP: Get digit (LIFO reverses order)
    add dl, '0'                         ; WHY ADD '0': Convert 0-9 to ASCII '0'-'9'
    mov ah, 0x0E                        ; WHY 0x0E: BIOS teletype function
    mov al, dl                          ; AL = character to print
    int 0x10                            ; WHY INT 10h: BIOS video services
    loop .print                         ; WHY LOOP: Decrement CX, jump if not zero

    popa                                ; WHY: Restore all registers
    ret

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ draw_hline — Draw horizontal line of repeated characters                    │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY THIS EXISTS:
;   Draw box borders, separators, progress bars in boot UI.
;   Using rep stosw for bulk fill is much faster than loop + individual writes.
;
; PARAMETERS:
;   DH = row (0-24)
;   DL = starting column (0-79)
;   CX = length (number of characters)
;   AL = character to repeat (e.g., '─' for line, '█' for block)
;   AH = attribute byte (foreground/background colors)
;
; RETURNS: Nothing.
; CLOBBERS: None (pusha/popa preserve all registers).
;
draw_hline:
    pusha                               ; WHY: Preserve all registers
    push es                             ;      ES will point to video memory
    push ax                             ; WHY: Save char+attr (we need AX for address calc)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Calculate video memory starting offset
    ; ─────────────────────────────────────────────────────────────────────────
    mov ax, VIDEO_MEM                   ; VIDEO_MEM = 0xB800
    mov es, ax                          ; ES → video segment
    movzx ax, dh                        ; AX = row (zero-extended)
    push cx                             ; WHY: Save length (MUL clobbers CX's use)
    mov cx, SCREEN_WIDTH                ; CX = 80
    mul cx                              ; AX = row * 80
    pop cx                              ; WHY: Restore length for rep stosw
    movzx bx, dl                        ; BX = starting column (zero-extended)
    add ax, bx                          ; AX = row * 80 + col
    shl ax, 1                           ; AX = (row * 80 + col) * 2
    mov di, ax                          ; DI = starting offset in video memory

    ; ─────────────────────────────────────────────────────────────────────────
    ; Draw the line using rep stosw
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY REP STOSW: Single instruction fills CX words (char+attr pairs).
    ;   CPU-optimized bulk memory fill — faster than loop.
    pop ax                              ; WHY: Restore char (AL) + attr (AH)
    rep stosw                           ; WHY: Write AX to [ES:DI], DI+=2, CX--
                                        ;   Repeats CX times (draws full line)

    pop es                              ; WHY: Restore ES
    popa                                ;      Restore all registers
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3b TECH — Technical Helper Functions (16-bit)
; ───────────────────────────────────────────────────────────────────────────────

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ enable_a20 — Enable A20 address line for memory above 1MB                   │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY A20 MATTERS:
;   The 8086 had 20 address lines (A0-A19), addressing 1MB.
;   When 80286 added A20 (21st line), IBM kept it disabled by default for
;   8086 compatibility (wrap-around at 1MB boundary).
;
;   Without A20 enabled:
;     Address 0x100000 wraps to 0x000000 (can't access memory above 1MB!)
;     Address 0x100001 wraps to 0x000001
;     etc.
;
;   With A20 enabled:
;     Full 32-bit addressing works (essential for protected/long mode)
;
; WHY TWO METHODS:
;   Tripwire pattern — try BIOS first (clean, standard), fall back to
;   keyboard controller method (works on older/weird BIOSes).
;
; METHOD 1: BIOS INT 15h, AX=2401h
;   Modern, clean, recommended. Not all BIOSes support it.
;
; METHOD 2: Keyboard Controller
;   The 8042 keyboard controller has an unused output pin connected to A20.
;   This is a hardware hack from the IBM PC/AT era.
;
; PARAMETERS: None.
; RETURNS: CF=0 on success (always, we assume success).
; CLOBBERS: AX (but called with pusha in most cases).
;
enable_a20:
    ; ─────────────────────────────────────────────────────────────────────────
    ; METHOD 1: Try BIOS first (modern, clean)
    ; ─────────────────────────────────────────────────────────────────────────
    mov ax, 0x2401                      ; WHY 0x2401: BIOS function "Enable A20 Gate"
    int 0x15                            ; WHY INT 15h: System services interrupt
    jnc .a20_done                       ; WHY JNC: CF=0 means success, we're done

    ; ─────────────────────────────────────────────────────────────────────────
    ; METHOD 2: Keyboard controller fallback
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY THIS WORKS: The 8042 keyboard controller has an output port with
    ;   bit 1 controlling the A20 gate. This is a legacy hardware interface.
    ;
    ; PORT MAP:
    ;   0x60 = Data port (read/write data to/from controller)
    ;   0x64 = Command/Status port
    ;         Read: Status register
    ;         Write: Command register
    ;
    ; STATUS REGISTER BITS:
    ;   Bit 0 (0x01): Output buffer full (data ready to read from 0x60)
    ;   Bit 1 (0x02): Input buffer full (don't write yet, controller busy)

    ; Step 1: Disable keyboard
    call .a20_wait_input                ; WHY: Wait until controller can accept command
    mov al, 0xAD                        ; WHY 0xAD: "Disable keyboard" command
    out 0x64, al                        ; WHY PORT 0x64: Command port

    ; Step 2: Read output port
    call .a20_wait_input                ; WHY: Wait for controller ready
    mov al, 0xD0                        ; WHY 0xD0: "Read output port" command
    out 0x64, al                        ; Send command

    call .a20_wait_output               ; WHY: Wait for data to be ready
    in al, 0x60                         ; WHY PORT 0x60: Read output port value
    push ax                             ; WHY PUSH: Save current output port state

    ; Step 3: Write output port with A20 enabled
    call .a20_wait_input                ; WHY: Wait for controller ready
    mov al, 0xD1                        ; WHY 0xD1: "Write output port" command
    out 0x64, al                        ; Send command

    call .a20_wait_input                ; WHY: Wait for controller ready
    pop ax                              ; WHY POP: Restore output port value
    or al, 2                            ; WHY OR 2: Set bit 1 (A20 gate enable)
    out 0x60, al                        ; WHY PORT 0x60: Write modified value

    ; Step 4: Re-enable keyboard
    call .a20_wait_input                ; WHY: Wait for controller ready
    mov al, 0xAE                        ; WHY 0xAE: "Enable keyboard" command
    out 0x64, al                        ; Send command

    call .a20_wait_input                ; WHY: Final wait to ensure command processed

.a20_done:
    clc                                 ; WHY CLC: Clear carry flag (indicate success)
    ret

; ─────────────────────────────────────────────────────────────────────────────
; A20 helper: Wait for keyboard controller input buffer to be empty
; ─────────────────────────────────────────────────────────────────────────────
; WHY: Can't send commands while controller is still processing previous one.
;   Bit 1 of status = "input buffer full" = controller busy.
.a20_wait_input:
    in al, 0x64                         ; WHY: Read status register
    test al, 2                          ; WHY TEST 2: Check bit 1 (input buffer full)
    jnz .a20_wait_input                 ;   Still full → keep waiting
    ret

; ─────────────────────────────────────────────────────────────────────────────
; A20 helper: Wait for keyboard controller output buffer to have data
; ─────────────────────────────────────────────────────────────────────────────
; WHY: Can't read data until controller has placed it in output buffer.
;   Bit 0 of status = "output buffer full" = data ready.
.a20_wait_output:
    in al, 0x64                         ; WHY: Read status register
    test al, 1                          ; WHY TEST 1: Check bit 0 (output buffer full)
    jz .a20_wait_output                 ;   Empty → keep waiting
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3c SERIAL — Serial Port Functions (16-bit)
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY SERIAL OUTPUT:
;   Serial port provides debug output visible in QEMU/real hardware
;   even when video fails or before video is initialized.
;   Essential for boot debugging.
;
; PORT MAP (COM1):
;   0x3F8 = Data Register (read/write characters)
;   0x3F9 = Interrupt Enable Register
;   0x3FA = FIFO Control Register
;   0x3FB = Line Control Register
;   0x3FC = Modem Control Register
;   0x3FD = Line Status Register (LSR)
;   0x3FE = Modem Status Register
;
; LSR BITS (0x3FD):
;   Bit 0 (0x01): Data Ready
;   Bit 5 (0x20): Transmit Holding Register Empty (THRE)
;

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ serial_print — Print null-terminated string to serial port (16-bit)         │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY THIS EXISTS:
;   Send debug messages to serial port for boot diagnostics.
;   Works identically to 32-bit version but in real mode.
;
; PARAMETERS:
;   SI = pointer to null-terminated string
;
; RETURNS: Nothing.
; CLOBBERS: None (pusha/popa preserve all registers).
;
serial_print:
    pusha                               ; WHY: Preserve all registers
.loop:
    lodsb                               ; WHY LODSB: Load [SI] into AL, SI++
    test al, al                         ; WHY TEST: Check for null terminator
    jz .done                            ;   NULL → end of string
    call serial_char                    ; WHY: Send character in AL to COM1
    jmp .loop                           ;   Continue to next character
.done:
    popa                                ; WHY: Restore all registers
    ret

; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ serial_char — Print single character to serial port (16-bit)                │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY THIS EXISTS:
;   Low-level character output to COM1. Used by serial_print and directly
;   for single-character output.
;
; WHY WAIT FOR THRE:
;   Serial ports are SLOW (115200 baud = ~11.5 KB/s max).
;   Must wait for transmit buffer empty before sending next byte,
;   or characters get dropped.
;
; PARAMETERS:
;   AL = character to send
;
; RETURNS: Nothing.
; CLOBBERS: None (push/pop preserve used registers).
;
serial_char:
    push dx                             ; WHY: Preserve DX (we use it for port I/O)
    push ax                             ;      Preserve AX (character to send)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Wait for transmit buffer empty
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY WAIT: Can't send while UART is still transmitting previous byte.
    ;   Polling THRE bit ensures transmit holding register is available.
    mov dx, 0x3FD                       ; WHY 0x3FD: COM1 Line Status Register
.wait:
    in al, dx                           ; WHY: Read LSR
    test al, 0x20                       ; WHY 0x20: Check THRE (bit 5)
    jz .wait                            ;   Buffer full → keep waiting

    ; ─────────────────────────────────────────────────────────────────────────
    ; Send character
    ; ─────────────────────────────────────────────────────────────────────────
    pop ax                              ; WHY: Restore character to send
    mov dx, 0x3F8                       ; WHY 0x3F8: COM1 Data Register
    out dx, al                          ; WHY OUT: Write byte to transmit

    pop dx                              ; WHY: Restore DX
    ret

; ───────────────────────────────────────────────────────────────────────────────
; B.3d WITNESS — Store Boot Event to Buffer (DAR Witness, 16-bit)
; ───────────────────────────────────────────────────────────────────────────────
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ store_boot_event — Record DAR witness event for kernel ingestion (16-bit)   │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; WHY DAR WITNESS:
;   Detect-Assess-Restore paradigm for boot forensics.
;   Records events during boot so kernel can analyze what happened.
;   Essential for debugging boot failures and understanding boot sequence.
;
; WHY BUFFER (not serial):
;   Serial output is ephemeral (scrolls away, may not be captured).
;   Buffer persists in memory for kernel to ingest, parse, and store.
;   Enables post-boot analysis and logging to persistent storage.
;
; BUFFER STRUCTURE (header at offset 0):
;   +0x00: magic (4 bytes) = "BEVT" (Boot EVenT)
;   +0x04: count (2 bytes) = number of events stored
;   +0x06: reserved (2 bytes)
;   +0x08: sequence (4 bytes) = next sequence number
;   +0x0C: reserved (4 bytes)
;   +0x10: events start (32 bytes each)
;
; EVENT STRUCTURE (32 bytes each):
;   +0x00: sequence (4 bytes) - unique event ID
;   +0x04: phase (1 byte) - DAR phase (-1=DETECT, 0=ASSESS, 1=RESTORE)
;   +0x05: severity (1 byte) - (-1=MILD, 0=MEDIUM, 1=CRITICAL)
;   +0x06: health (1 byte) - health delta (signed)
;   +0x07: src_len (1 byte) - source string length
;   +0x08: source (8 bytes) - component name, null-padded
;   +0x10: message (16 bytes) - event description, null-padded
;
; PARAMETERS:
;   SI = pointer to source string (8 chars max)
;   DI = pointer to message string (16 chars max)
;   Stack: [SP+2] = DAR phase, [SP+4] = severity
;
; RETURNS: Nothing.
; CLOBBERS: AX, BX, CX, DX (inside pusha frame).
;

store_boot_event:
    ; ─────────────────────────────────────────────────────────────────────────
    ; Prologue: Set up stack frame
    ; ─────────────────────────────────────────────────────────────────────────
    push bp                             ; WHY BP: Need stable base for stack parameters
    mov bp, sp                          ;   [bp+0]=old bp, [bp+2]=return, [bp+4]=arg1, [bp+6]=arg2
    pusha                               ; WHY: Preserve all general registers
    push es                             ;      Preserve ES (might be used by caller)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Check buffer capacity
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY CHECK: Can't write past buffer end (would corrupt memory).
    mov ax, [BOOT_EVENT_BUFFER + 4]     ; WHY +4: Event count field in header
    cmp ax, BOOT_EVENT_MAX              ; WHY CMP: Compare against max capacity
    jae .event_full                     ;   Full → skip storing (silent drop)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Calculate event slot address
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY FORMULA: slot = BUFFER + 16 + (count * 32)
    ;   16 = header size (magic, count, sequence)
    ;   32 = event size
    mov bx, ax                          ; BX = count
    shl bx, 5                           ; WHY SHL 5: Multiply by 32 (2^5)
    add bx, BOOT_EVENT_BUFFER           ; BX = BUFFER + (count * 32)
    add bx, 16                          ; BX = slot address (skip header)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Store sequence number (atomically increment)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY SEQUENCE: Unique ID for each event, enables ordering and gap detection.
    mov eax, [BOOT_EVENT_BUFFER + 8]    ; WHY +8: Sequence field in header
    mov [bx + 0], eax                   ; WHY +0: Sequence field in event
    inc eax                             ; WHY INC: Next event gets sequence+1
    mov [BOOT_EVENT_BUFFER + 8], eax    ; WHY: Update global sequence counter

    ; ─────────────────────────────────────────────────────────────────────────
    ; Store DAR phase and severity from stack
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY FROM STACK: Caller pushes phase/severity before call.
    ;   Stack layout: [bp+4]=phase, [bp+6]=severity
    mov ax, [bp + 4]                    ; WHY [bp+4]: First pushed argument
    mov byte [bx + 4], al               ; WHY +4: Phase field in event
    mov ax, [bp + 6]                    ; WHY [bp+6]: Second pushed argument
    mov byte [bx + 5], al               ; WHY +5: Severity field in event

    ; WHY HEALTH=0: Neutral health delta (no change).
    ;   Future: Could calculate based on phase/severity.
    mov byte [bx + 6], 0

    ; ─────────────────────────────────────────────────────────────────────────
    ; Copy source string (max 8 bytes, null-padded)
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY NULL-PADDED: Fixed-width field for predictable parsing.
    push di                             ; WHY: Save DI (message pointer) for later
    mov di, bx
    add di, 8                           ; WHY +8: Source field offset in event
    mov cx, 8                           ; WHY 8: Source field width
    push si                             ; WHY: Save SI for length calculation
.copy_src:
    lodsb                               ; WHY LODSB: Load [SI] into AL, SI++
    test al, al                         ; WHY TEST: Check for null terminator
    jz .pad_src                         ;   NULL → switch to padding
    stosb                               ; WHY STOSB: Store AL to [DI], DI++
    dec cx                              ; WHY DEC: Track remaining width
    jnz .copy_src                       ;   More space → continue
    jmp .src_done                       ;   Field full → done
.pad_src:
    xor al, al                          ; WHY XOR: AL = 0 for null padding
.pad_src_loop:
    stosb                               ; WHY STOSB: Write null byte
    dec cx                              ; WHY DEC: Track remaining width
    jnz .pad_src_loop                   ;   More padding needed → continue
.src_done:
    pop si                              ; WHY: Restore SI for length calculation
    pop di                              ; WHY: Restore DI (message pointer)

    ; ─────────────────────────────────────────────────────────────────────────
    ; Calculate and store source length
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY SEPARATE LENGTH: Enables efficient parsing without scanning for null.
    push di                             ; WHY: Preserve DI again
    xor cx, cx                          ; WHY XOR: CX = 0 (length counter)
.len_src:
    lodsb                               ; WHY LODSB: Scan source string
    test al, al                         ; WHY TEST: Check for null
    jz .len_done                        ;   NULL → done counting
    inc cx                              ; WHY INC: Increment length
    cmp cx, 8                           ; WHY CMP: Don't count beyond field width
    jb .len_src                         ;   Under 8 → continue counting
.len_done:
    mov byte [bx + 7], cl               ; WHY +7: src_len field in event
    pop di                              ; WHY: Restore DI

    ; ─────────────────────────────────────────────────────────────────────────
    ; Copy message string (max 16 bytes, null-padded)
    ; ─────────────────────────────────────────────────────────────────────────
    push bx                             ; WHY: Preserve event base address
    add bx, 16                          ; WHY +16: Message field offset
    mov si, di                          ; WHY: SI = message source (from caller's DI)
    mov cx, 16                          ; WHY 16: Message field width
.copy_msg:
    lodsb                               ; WHY LODSB: Load message byte
    test al, al                         ; WHY TEST: Check for null
    jz .pad_msg                         ;   NULL → switch to padding
    mov [bx], al                        ; WHY: Store byte (not STOSB, different pointer)
    inc bx                              ; WHY: Advance destination
    dec cx                              ; WHY: Track remaining width
    jnz .copy_msg                       ;   More space → continue
    jmp .msg_done                       ;   Field full → done
.pad_msg:
    xor al, al                          ; WHY XOR: AL = 0 for null padding
.pad_msg_loop:
    mov [bx], al                        ; WHY: Write null byte
    inc bx                              ; WHY: Advance destination
    dec cx                              ; WHY: Track remaining width
    jnz .pad_msg_loop                   ;   More padding needed → continue
.msg_done:
    pop bx                              ; WHY: Restore event base address

    ; ─────────────────────────────────────────────────────────────────────────
    ; Increment event count
    ; ─────────────────────────────────────────────────────────────────────────
    ; WHY LAST: Only increment after successful write.
    mov ax, [BOOT_EVENT_BUFFER + 4]     ; WHY +4: Event count field
    inc ax                              ; WHY INC: count++
    mov [BOOT_EVENT_BUFFER + 4], ax     ; WHY: Store updated count

.event_full:
    ; WHY SILENT DROP: Buffer full is not fatal — boot must continue.
    ;   Event loss is acceptable; boot completion is not.

    ; ─────────────────────────────────────────────────────────────────────────
    ; Epilogue: Restore and return
    ; ─────────────────────────────────────────────────────────────────────────
    pop es                              ; WHY: Restore ES
    popa                                ; WHY: Restore all general registers
    pop bp                              ; WHY: Restore caller's BP
    ret 4                               ; WHY RET 4: Clean 4 bytes from stack (2 words)
                                        ;   Caller pushed 2 word arguments

; ═══════════════════════════════════════════════════════════════════════════════
; B.4 ERROR HANDLING — Failure Paths (All CPU Modes) [ERRORS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Exit paths for error conditions and user-requested termination.
;
; These handlers are reached when:
;   - User selects reboot (option 3 in menu)
;   - User presses ESC to halt
;   - An unrecoverable error occurs (A20 failure, disk failure, etc.)
;
; WHY SEPARATE SECTION: Error handlers are NOT part of the happy path.
;   Keeping them separate makes the core ops flow cleaner to read.
;   Also enables future expansion (error logging, recovery attempts).
;
; WHY AT END OF BODY: Section order is B.1 → B.2 → B.3 → B.4 → B.5.
;   Error handling comes after helpers because it's exceptional — code that
;   should rarely execute. Reader encounters it last, after understanding
;   the normal flow.
;
; CONTAINS:
;   - do_reboot — System reboot via keyboard controller (16-bit)
;   - do_halt   — CPU halt (infinite cli/hlt loop, 16-bit)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; do_reboot — System Reboot via Keyboard Controller (16-bit)
; ───────────────────────────────────────────────────────────────────────────────

; do_reboot triggers a system reboot via the keyboard controller.
;
; @brief  Send reset pulse to keyboard controller to trigger CPU reset.
;         Falls through to do_halt if keyboard controller method fails.
;
; Parameters: None.
;
; Returns:
;   Never returns — system reboots or halts.
;
; Method:
;   Write 0xFE to port 0x64 (keyboard controller command port).
;   This pulses the CPU reset line.
;
; WHY THIS WORKS:
;   The 8042 keyboard controller (or compatible) has control over the CPU
;   reset line. Command 0xFE tells it to pulse that line low, triggering
;   a hardware reset identical to pressing the reset button.
;
; WHY FALLTHROUGH TO HALT:
;   Some systems (especially VMs) may not support keyboard controller reset.
;   If the OUT instruction returns (reset didn't happen), we halt instead
;   of leaving the system in an undefined state.
;
do_reboot:
    ; WHY: Port 0x64 is the keyboard controller command port.
    ;      Command 0xFE = "pulse output port" = CPU reset.
    mov al, 0xFE                        ; 0xFE = pulse CPU reset line
    out 0x64, al                        ; Send to keyboard controller
    ; If we get here, reboot failed — fall through to halt

; ───────────────────────────────────────────────────────────────────────────────
; do_halt — System Halt (Infinite Loop, 16-bit)
; ───────────────────────────────────────────────────────────────────────────────

; do_halt stops the CPU in an infinite halt loop.
;
; @brief  Disable interrupts and halt CPU forever.
;         Loop handles spurious wakeups from NMI or SMI.
;
; Parameters: None.
;
; Returns:
;   Never returns — CPU halted.
;
; WHY CLI:
;   Disabling interrupts prevents any interrupt handler from running.
;   Without this, timer interrupts would wake the CPU from HLT every ~55ms.
;
; WHY HLT IN A LOOP:
;   HLT stops the CPU until an interrupt occurs. But NMI (Non-Maskable
;   Interrupt) and SMI (System Management Interrupt) can wake the CPU
;   even with IF=0. The loop ensures we immediately halt again.
;
do_halt:
    cli                                 ; Disable interrupts (IF=0)
                                        ; WHY: Prevents maskable interrupts
.halt_loop:
    hlt                                 ; Halt CPU until interrupt
                                        ; WHY: Low power state, CPU stops
    jmp .halt_loop                      ; If woken by NMI/SMI, halt again
                                        ; WHY: NMI can't be disabled by CLI

; ═══════════════════════════════════════════════════════════════════════════════
; END BODY [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; DATA SECTION [DATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Initialized data — placed after code in flat binary.
;
; PURPOSE: Strings, tables, and initialized variables used by boot code.
;          In flat binary mode, data is loaded as part of the image.
;
; CONTAINS:
;   - D.1 NUMERIC   — State variables (boot drive, counters)
;   - D.2 STRINGS   — UI messages, serial debug, DAR witness strings
;   - D.3 TABLES    — GDT, disk packet, mode info buffers
;   - D.4 BUFFERS   — Memory map, VESA info (reserved space)
;   - D.5 CONSTANTS — Page table addresses
;
; Builds FROM: BODY (code references this data)
; Builds TO:   CLOSING (data complete, ready for synthesis)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; D.1 NUMERIC — State Variables [NUMERIC]
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY SEPARATE: Boot state tracked across multiple functions.
;   Global variables avoid passing via stack in 16-bit mode.
;

boot_drive:     db 0                    ; BIOS drive number (DL on entry)
mmap_count:     dw 0                    ; Number of E820 memory map entries
countdown:      db 0                    ; Auto-boot countdown (seconds)
tick_start:     dw 0                    ; Timer tick start for countdown
boot_in_text_mode: db 0                 ; 1 = VGA text mode fallback, 0 = graphics

; ───────────────────────────────────────────────────────────────────────────────
; D.2 STRINGS — Message Strings [STRINGS]
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY GROUPED: Easier localization and maintenance.
;   All user-visible strings in one place.
;

; ─────────────────────────────────────────
; D.2a Serial Debug Messages
; ─────────────────────────────────────────
; WHY SERIAL MESSAGES: Debug output visible in QEMU even when video fails.
;   Format: [MOS-BIOS] prefix identifies BIOS boot path.

msg_serial_banner:  db 13, 10, "[MOS-BIOS] MillenniumOS Boot - BIOS Path", 13, 10, 0
msg_serial_a20:     db "[MOS-BIOS] A20 line enabled", 13, 10, 0
msg_serial_vesa:    db "[MOS-BIOS] VESA mode set", 13, 10, 0
msg_serial_mmap:    db "[MOS-BIOS] Memory map acquired", 13, 10, 0
msg_serial_load:    db "[MOS-BIOS] Loading kernel...", 13, 10, 0
msg_serial_prot:    db "[MOS-BIOS] Entering protected mode", 13, 10, 0
msg_serial_long:    db "[MOS-BIOS] Entering long mode", 13, 10, 0
msg_serial_jump:    db "[MOS-BIOS] Jumping to kernel", 13, 10, 0

; ─────────────────────────────────────────
; D.2b DAR Witness Event Strings
; ─────────────────────────────────────────
; WHY DAR STRINGS: Fixed-width fields for boot event buffer.
;   Sources: max 8 chars, Messages: max 16 chars.

; Event Sources (component identifiers)
evt_src_bios:       db "BIOS", 0
evt_src_a20:        db "A20", 0
evt_src_vesa:       db "VESA", 0
evt_src_mmap:       db "MMAP", 0
evt_src_disk:       db "DISK", 0
evt_src_prot:       db "PMODE", 0
evt_src_long:       db "LMODE", 0
evt_src_kern:       db "KERNEL", 0

; Event Messages (what happened)
evt_msg_0b_entry:   db "Stage 0b entry", 0
evt_msg_a20_ok:     db "A20 enabled", 0
evt_msg_a20_fail:   db "A20 FAILED", 0
evt_msg_vesa_ok:    db "VESA mode set", 0
evt_msg_vesa_fail:  db "VESA fallback", 0
evt_msg_text_mode:  db "Text mode only", 0
evt_msg_mmap_ok:    db "Memory mapped", 0
evt_msg_mmap_fail:  db "Mmap failed", 0
evt_msg_load_start: db "Kernel loading", 0
evt_msg_load_ok:    db "Kernel loaded", 0
evt_msg_load_fail:  db "Load FAILED", 0
evt_msg_prot_enter: db "Prot mode entry", 0
evt_msg_long_enter: db "Long mode entry", 0
evt_msg_kern_jump:  db "Jumping kernel", 0

; ─────────────────────────────────────────
; D.2c Boot Selector UI Messages
; ─────────────────────────────────────────
; WHY UI STRINGS: User-visible text for boot menu.
;   Kingdom Technology branding with Genesis 1:1 foundation.

msg_title:      db "M I L L E N N I U M O S", 0
msg_subtitle:   db "Kingdom Technology Operating System", 0
msg_scripture1: db '"In the beginning God created the heaven and the earth."', 0
msg_scripture2: db "- Genesis 1:1", 0

msg_sysinfo:    db "[ System Information ]", 0
msg_version:    db "Version: a-01.00 (Alpha)", 0
msg_copyright:  db "(C) 2026 CreativeWorkzStudio LLC", 0
msg_memory:     db "Conventional Memory: ", 0
msg_kb:         db " KB", 0
msg_bootmode:   db "Boot Mode: BIOS (Legacy)", 0

msg_opt1:       db "[1] Boot MillenniumOS (BIOS/VESA)", 0
msg_opt2:       db "[2] About MillenniumOS", 0
msg_opt3:       db "[3] Reboot", 0
msg_prompt:     db "Select an option (1-3) or ESC to halt", 0
msg_uefi_note:  db "For UEFI/GOP boot: Use UEFI firmware boot menu", 0
msg_autoboot:   db "Auto-boot in   seconds...", 0

; ─────────────────────────────────────────
; D.2d Boot Status Messages
; ─────────────────────────────────────────
; WHY STATUS MESSAGES: Feedback during boot process and error states.

msg_loading:    db "Booting MillenniumOS...", 0
msg_please_wait: db "Please wait while the system initializes.", 0
msg_a20_fail:   db "A20 line enable failed", 0
msg_kernel_fail: db "Kernel Load Failed", 0
msg_boot_failed: db "Boot process encountered an error.", 0
msg_anykey:     db "Press any key to return to menu", 0
msg_boot_progress: db "Initializing system...", 0
msg_loading_kernel: db "Loading MillenniumOS kernel (448KB)...", 0

; ─────────────────────────────────────────
; D.2e About Screen Messages
; ─────────────────────────────────────────
; WHY ABOUT SCREEN: Explains Kingdom Technology philosophy.

msg_about_title: db "About MillenniumOS", 0
msg_about1:     db "MillenniumOS is a biblical-foundation operating system", 0
msg_about2:     db "built on OmniCode and the CPI-SI paradigm.", 0
msg_about3:     db "Genesis 1:1 grounds everything.", 0
msg_about4:     db "Part of the Kingdom Technology Stack:", 0
msg_about5:     db "Bereshit -> OmniCode -> CPI-SI -> MillenniumOS", 0
msg_about6:     db "creativeworkzstudio.com", 0
msg_about7:     db "Built with libtrit (balanced ternary foundation)", 0

; ───────────────────────────────────────────────────────────────────────────────
; D.3 TABLES — Descriptor Tables and Structures [TABLES]
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY TABLES: Hardware-defined structures with specific layouts.
;   GDT: Required for protected/long mode segment descriptors.
;   DAP: BIOS INT 13h extended read packet structure.
;

; ─────────────────────────────────────────
; D.3a Disk Address Packet (DAP)
; ─────────────────────────────────────────
; WHY DAP: INT 13h AH=42h (extended read) requires this structure.
;   Fields modified during multi-chunk kernel loading.

disk_address_packet:
    db 16                               ; Size of packet (always 16)
    db 0                                ; Reserved (must be 0)
dap_sectors:
    dw 0                                ; Number of sectors (set per chunk)
dap_offset:
    dw 0x0000                           ; Offset (always 0 for segment:0)
dap_segment:
    dw 0                                ; Segment (set per chunk)
dap_lba_lo:
    dd 0                                ; LBA low 32 bits (set per chunk)
dap_lba_hi:
    dd 0                                ; LBA high 32 bits (always 0 for <2TB)

; ─────────────────────────────────────────
; D.3b Global Descriptor Table (32-bit)
; ─────────────────────────────────────────
; WHY GDT32: Protected mode requires segment descriptors.
;   Flat model: code and data segments span 4GB.

align 16
gdt:
    dq 0                                ; Null descriptor (required, index 0)
    ; Code segment (32-bit): base=0, limit=4GB, executable, readable
    dw 0xFFFF, 0x0000                   ; Limit 15:0, Base 15:0
    db 0x00, 0x9A, 0xCF, 0x00           ; Base 23:16, Access, Flags+Limit, Base 31:24
    ; Data segment (32-bit): base=0, limit=4GB, writable
    dw 0xFFFF, 0x0000                   ; Limit 15:0, Base 15:0
    db 0x00, 0x92, 0xCF, 0x00           ; Base 23:16, Access, Flags+Limit, Base 31:24
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt - 1                ; Limit (size - 1)
    dd gdt                              ; Base address

; ─────────────────────────────────────────
; D.3c Global Descriptor Table (64-bit)
; ─────────────────────────────────────────
; WHY GDT64: Long mode requires different descriptor format.
;   In 64-bit mode, base/limit ignored for code/data (flat assumed).

align 16
gdt64:
    dq 0                                ; Null descriptor (required, index 0)
    ; Code segment (64-bit): L=1 (long mode), D=0 (64-bit default)
    dq 0x00209A0000000000               ; Access=0x9A (exec/read), L=1
    ; Data segment (64-bit): writable
    dq 0x0000920000000000               ; Access=0x92 (read/write)
gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64 - 1            ; Limit (size - 1)
    dd gdt64                            ; Base address

; ───────────────────────────────────────────────────────────────────────────────
; D.4 BUFFERS — Reserved Data Areas [BUFFERS]
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY BUFFERS: Space for BIOS-returned data.
;   Alignment ensures hardware compatibility.
;

; ─────────────────────────────────────────
; D.4a VESA Mode Info Buffer
; ─────────────────────────────────────────
; WHY 256 BYTES: VBE mode info structure is 256 bytes.
;   BIOS INT 10h AX=4F01h fills this buffer.

align 256
vesa_mode_info:
    times 256 db 0

; ─────────────────────────────────────────
; D.4b E820 Memory Map Buffer
; ─────────────────────────────────────────
; WHY 243 ENTRIES: Practical maximum for E820 map.
;   Each entry = 24 bytes (base 8, length 8, type 4, ACPI ext 4).
;   243 * 24 = 5832 bytes (~6KB).

align 16
memory_map:
    times 243 * 24 db 0                 ; 243 entries * 24 bytes each

; ───────────────────────────────────────────────────────────────────────────────
; D.5 CONSTANTS — Page Table Addresses [CONSTANTS]
; ───────────────────────────────────────────────────────────────────────────────
;
; WHY CONSTANTS: Page tables at fixed addresses, set up at runtime.
;   Must be 4K aligned for x86-64 paging requirements.
;
; MEMORY LAYOUT:
;   0x70000 — PML4  (4KB) — Page Map Level 4
;   0x71000 — PDPT  (4KB) — Page Directory Pointer Table
;   0x72000 — PD    (16KB) — Page Directory (4 × 4KB for 4GB mapping)
;
; WHY 0x70000: Safe area that doesn't overlap with:
;   - Boot info at 0x9000-0xAE00
;   - Temporary stack at 0x90000
;   - Kernel load area at 0x10000-0x6FFFF
;

PAGE_TABLE_BASE     equ 0x70000
PML4_ADDR           equ PAGE_TABLE_BASE           ; 0x70000
PDPT_ADDR           equ PAGE_TABLE_BASE + 0x1000  ; 0x71000
PD_ADDR             equ PAGE_TABLE_BASE + 0x2000  ; 0x72000

; ═══════════════════════════════════════════════════════════════════════════════
; END DATA [END]
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
;   - X.1 VALIDATION — Build and test verification
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
;   nasm -f bin stage0b.asm -o stage0b.bin
;   # Must assemble without errors or warnings
;
; Size Verification:
;   ls -l stage0b.bin
;   # Expected size: ~14-16KB (fits in 32 sectors)
;   # If larger than 16KB: reduce strings or optimize code
;
; Full Image Build:
;   make  # In millenniumos/ root
;   # Combines stage0a + stage0b + kernel into disk image
;
; Boot Testing:
;   qemu-system-x86_64 -drive format=raw,file=build/millenniumos.img
;   # Should show boot selector menu
;   # Press 1 or wait 10 seconds for auto-boot
;   # Should transition through all debug markers (1:VIDEO through 8:JUMP)
;
; Serial Testing:
;   qemu-system-x86_64 -drive format=raw,file=build/millenniumos.img -serial stdio
;   # Should see [MOS-BIOS] messages for each boot phase
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.2 EXECUTION — Entry Point & Flow [EXECUTION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Entry point and execution summary.
;
; Entry Point: stage0b_start (at ORG 0x7E00)
;
; Execution Flow:
;   1. Stage 0a loads Stage 0b to 0x7E00
;   2. Stage 0a jumps to 0x7E00 (stage0b_start)
;   3. Stage 0b initializes serial, displays boot menu
;   4. User selects boot option (or 10-second auto-boot)
;   5. Stage 0b enables A20, sets VESA, gets memory map
;   6. Stage 0b loads kernel chunks to low memory
;   7. Stage 0b enters protected mode, copies kernel to 1MB
;   8. Stage 0b sets up paging, enters long mode
;   9. Stage 0b jumps to kernel at 0x100000
;
; Register State on Entry (from Stage 0a):
;   DL: Boot drive number
;   CS: Usually 0x0000
;   IP: 0x7E00
;
; Register State on Exit (to kernel):
;   RDI: BOOT_INFO_ADDR (0x9000) — kernel parameter
;   RSP: 0x90000 (temporary stack)
;   All others: Undefined (kernel sets its own)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.3 CLEANUP — Register & Resource Management [CLEANUP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Resource management across mode transitions.
;
; Resource Management:
;   - Registers: Mode-specific (16→32→64 bit transitions reset registers)
;   - Stack: Temporary at each mode (0x90000 for long mode)
;   - Memory: Page tables at 0x70000, boot info at 0x9000
;   - Serial: COM1 initialized, used across all modes
;
; Mode Transitions:
;   - Real → Protected: GDT load, CR0.PE set, far jump
;   - Protected → Long: PAE enable, CR3 set, EFER.LME, CR0.PG, far jump
;
; Callee-Saved: UI helpers preserve registers via PUSHA
; Caller-Saved: Mode entry points clobber all (fresh context)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.4 OVERVIEW — Module Summary [OVERVIEW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief High-level summary — references METADATA for details.
;
; Purpose:  See METADATA M.7 INTENT
; Provides: Boot menu, CPU mode transitions, kernel loading, DAR Witness
;
; Quick summary:
;   Stage 0b is the "heavy lifter" of boot — it bridges Stage 0a (512-byte
;   boot sector) and the kernel (64-bit long mode). It handles the boot
;   selector UI, A20 line enable, VESA video mode with tripwire fallback,
;   E820 memory map, multi-chunk kernel loading, GDT/paging setup, and
;   the complete transition from 16-bit real mode to 64-bit long mode.
;   DAR Witness boot events provide forensics for debugging boot issues.
;
; Architecture: Ladder — second rung, builds on Stage 0a foundation
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.5 POLICY — Modification Guidelines [POLICY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Guide future maintainers on what's safe to change.
;
; Safe to Modify (Extension Points):
;   ✅ UI strings and colors
;   ✅ Countdown timer duration
;   ✅ Add new menu options
;   ✅ Witness event messages
;   ✅ Debug marker colors
;
; Modify with Extreme Care (Breaking Changes):
;   ⚠️ Memory addresses — affects entire boot chain
;   ⚠️ Kernel chunk layout — must match protected mode copy
;   ⚠️ GDT entries — affects segment selectors
;   ⚠️ Page table addresses — must be 4K aligned
;   ⚠️ Boot info structure — must match bootinfo.h
;
; NEVER Modify (Foundational Rails):
;   ❌ Mode transition sequence (real → protected → long)
;   ❌ Page table identity mapping (4GB needed for boot)
;   ❌ Kernel parameter convention (RDI = boot info)
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
;   Ladder: Stage 0a → Stage 0b (this file) → Kernel
;   Baton:  Real Mode → Protected Mode → Long Mode → Kernel
;
; Mode Flow:
;   16-bit: Entry, menu, A20, VESA, mmap, disk load
;   32-bit: Segment setup, kernel copy, paging setup
;   64-bit: Visual test, kernel jump
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.7 PERF — Performance Considerations [PERF]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Boot time optimization considerations.
;
; Boot Speed Factors:
;   - Disk read time: ~1-2 seconds for 448KB (BIOS INT 0x13)
;   - VESA mode set: ~100ms
;   - Memory map: ~10ms
;   - Kernel copy: ~10ms (REP MOVSD at 32-bit)
;   - Mode transitions: <1ms each
;
; Total boot time: ~3-5 seconds typical
;
; Key Optimizations Applied:
;   - REP MOVSD for kernel copy (4 bytes per iteration)
;   - LBA disk access (no CHS calculation overhead)
;   - Chunked loading (maximizes segment limits)
;
; Further Optimization Possible:
;   - Reduce countdown timer (currently 10 seconds)
;   - Skip VESA fallback chain if known hardware
;   - Async disk read (not supported by BIOS)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.8 DEBUG — Troubleshooting Guide [DEBUG]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Common issues and debugging techniques.
;
; Problem: "Boot menu appears but hangs when selecting Boot"
;   Cause:    A20 line enable failure
;   Solution: Check serial output for "[MOS-BIOS] A20 line enabled"
;             Try both BIOS and keyboard controller methods
;
; Problem: "VGA TEXT" shown instead of graphics
;   Cause:    All VESA modes failed (tripwire fallback)
;   Solution: This is intentional fallback — kernel still boots
;             Check QEMU settings (-vga std, -vga vmware)
;
; Problem: "Triple fault / immediate reboot after mode transition"
;   Cause:    GDT/segment setup error, or paging misconfiguration
;   Solution: Check debug markers (1:VIDEO through 8:JUMP)
;             Last marker shown indicates failure point
;
; Problem: "Kernel loads but framebuffer is black"
;   Cause:    Framebuffer address or pitch mismatch
;   Solution: Check VESA mode info buffer populated correctly
;             Verify boot info structure matches bootinfo.h
;
; Debug Markers (VGA text mode, colored):
;   1:VIDEO  — VESA mode set (skipped if DEBUG_TEXT_MODE=1)
;   2:PROT   — Entering protected mode
;   3:32BIT  — 32-bit protected mode entry
;   4:COPY   — Kernel copy to 1MB complete
;   5:PAGE   — Page tables set up
;   6:LME    — Long mode enabled in EFER
;   7:64BIT  — 64-bit long mode entry (WHITE on RED)
;   8:JUMP   — About to jump to kernel
;
; Serial Debug Commands:
;   qemu-system-x86_64 -hda build/millenniumos.img -serial stdio
;   # All [MOS-BIOS] messages visible
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
;   Dependencies: Stage 0a, BIOS services (0x10, 0x13, 0x15, 0x16)
;   Dependents:   Kernel (expects boot info at 0x9000)
;
; Related Files:
;   - boot/stage0/stage0a.asm   — Loads this file
;   - boot/stage0/bootinfo.h    — Boot info structure definition
;   - boot/stage0/bootinfo.inc  — Assembly constants for boot info
;   - kernel/entry.asm          — Kernel entry point (receives RDI)
;   - kernel/kernel.c           — Main kernel (uses MosBootInfo)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.10 ROADMAP — Future Expansions [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Planned features and improvements.
;
; Planned Features:
;   ✓ Boot selector menu — COMPLETED
;   ✓ DAR Witness boot events — COMPLETED
;   ✓ VESA tripwire fallback — COMPLETED
;   ✓ Multi-chunk kernel loading — COMPLETED
;   ✓ Full mode transition chain — COMPLETED
;   ⏳ ACPI RSDP detection (pass to kernel)
;   ⏳ SMBIOS detection (system info)
;   ⏳ Bootloader configuration file
;   ⏳ Multiple kernel selection
;
; Research Areas:
;   - EFI stub for UEFI boot (currently separate UEFI bootloader)
;   - Secure boot signature verification
;   - Compression for larger kernels
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.11 REFERENCE — Quick Reference [REFERENCE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Copy-paste ready commands and patterns.
;
; Assemble (flat binary):
;     nasm -f bin stage0b.asm -o stage0b.bin
;
; Full Build:
;     make  # In millenniumos/ root
;
; Test with QEMU (BIOS):
;     qemu-system-x86_64 -drive format=raw,file=build/millenniumos.img
;
; Test with Serial Output:
;     qemu-system-x86_64 -drive format=raw,file=build/millenniumos.img \
;                        -serial stdio
;
; Test with GDB:
;     qemu-system-x86_64 -drive format=raw,file=build/millenniumos.img \
;                        -s -S -serial stdio
;     gdb -ex "target remote :1234" -ex "set architecture i8086"
;
; Memory Map (Key Addresses):
;     0x7C00  — Stage 0a (boot sector)
;     0x7E00  — Stage 0b (this file)
;     0x9000  — Boot info structure
;     0x10000 — Kernel chunk 1
;     0x70000 — Page tables (24KB)
;     0x90000 — Temporary stack
;     0xAE00  — Boot event buffer
;     0x100000 — Kernel final location
;
; Closing Note:
;   Stage 0b is the preparation before creation — the Spirit moving upon
;   the waters. It handles the complex transitions that bridge the ancient
;   BIOS world to the modern 64-bit kernel. The boot selector gives the
;   user a moment of choice, just as the boot process is a series of
;   decisions leading to the light of the running kernel.
;
;   "And God said, Let there be light: and there was light." — Genesis 1:3
;
; ═══════════════════════════════════════════════════════════════════════════════
; END CLOSING [END]
; ═══════════════════════════════════════════════════════════════════════════════
