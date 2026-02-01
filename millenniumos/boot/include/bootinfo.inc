; @file bootinfo.inc
; @omni code --asm-include
;
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
; @omni:req  key        = MOS-boot-bootinfo-inc
; @omni:req  from       = bereshit/word/seed/code/asm/include.inc
; @omni:req  at         = code
;
; ───────────────────────────────────────────────────────────────────────────────
; P.2 FAMILY — Classification [FAMILY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:inh  type       = include
; @omni:inh  subtype    = constants
; @omni:inh  role       = rails
; @omni:inh  structure  = 4-block
;
; ───────────────────────────────────────────────────────────────────────────────
; P.3 INSTANCE — File Details [INSTANCE]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  component  = boot/stage0
; @omni:ins  layer      = 0:kernel
; @omni:ins  includes   = none
; @omni:ins  provides   = [MOS_BOOT_INFO_ADDR, MOS_BOOT_MAGIC, BOOTINFO_OFF_*]
;
; ───────────────────────────────────────────────────────────────────────────────
; P.4 ARCHITECTURE — Scaling Context [ARCH]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  layers     = [0:kernel, 1:system, 2:runtime, 3:framework,
;                          4:health, 5:network, 6:storage, 7:engine, 8:app]
; @omni:ins  scale      = 3^n [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
; @omni:ins  pattern    = 4+1+4 [below:0-3, center:4, above:5-8]
; @omni:ins  anchor     = genesis_1_1 -> void(0) -> unity(1) -> derivation
;
; ───────────────────────────────────────────────────────────────────────────────
; P.5 SUMMARY — Human Identity [SUMMARY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:req  title      = Boot Information Constants (Assembly Mirror)
; @omni:req  brief      = NASM constants mirroring bootinfo.h for assembly access
;
; ═══════════════════════════════════════════════════════════════════════════════
; END PRAGMA [END]
; ═══════════════════════════════════════════════════════════════════════════════
;
; USAGE: %include "boot/include/bootinfo.inc"
;
; DEPENDENCY CLASSIFICATION: PURE (no external include dependencies)
;
; IMPORTANT: This file MUST stay synchronized with bootinfo.h!
;            When updating bootinfo.h constants or offsets, update this file.
;            See X.1 VALIDATION for synchronization verification.
;
; Assembly mirror of C bootinfo.h — provides identical constants and
; structure offsets for assembly files (stage0a.asm, stage0b.asm, entry.asm).
;
; Boot Information Include — CPI-SI MillenniumOS
;
; ═══════════════════════════════════════════════════════════════════════════════
; METADATA BLOCK [METADATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Identity and context for this include file.
;
; STRUCTURE: M.1-M.10 sections.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; M.1 IDENTITY — Core Identity [IDENTITY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @file      bootinfo.inc
; @brief     Boot Information Constants — Assembly mirror of bootinfo.h.
;
; Key:       MOS-boot-bootinfo-inc
; Title:     Boot Information Constants (Assembly)
; Type:      Include (Constants)
; Component: Rails — foundational boot handoff definitions
; Role:      Provide identical constants to bootinfo.h for assembly access

; ═══════════════════════════════════════════════════════════════════════════════
; M.2 STATE — Lifecycle State [STATE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @version   a-02.00
; @date      2026-01-31
;
; Status:    Active
; Created:   2026-01-31
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
; Path:      millenniumos/boot/include/bootinfo.inc

; ═══════════════════════════════════════════════════════════════════════════════
; M.5 DERIVATION — Template Lineage [DERIVATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Derives:   bereshit/word/seed/code/asm/include.inc (template)
; Mirrors:   millenniumos/boot/include/bootinfo.h (C header)

; ═══════════════════════════════════════════════════════════════════════════════
; M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Tags:      [asm, nasm, include, constants, boot, kernel, x86, x64]

; ═══════════════════════════════════════════════════════════════════════════════
; M.7 INTENT — Purpose Statement [INTENT]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Purpose:   Provide assembly files with the same boot info constants defined
;            in bootinfo.h, ensuring C and assembly code use identical values.
;
; Core Design: Mirror pattern — definitions generated/maintained to match C.
;
; Key Features:
;   - Boot info address and size constants
;   - Magic number and version for validation
;   - Ternary boot source values (-1/0/+1)
;   - MosBootInfo structure field offsets for direct access
;   - Capability flag definitions

; ═══════════════════════════════════════════════════════════════════════════════
; M.8 GROUNDING — Biblical Foundation [GROUNDING]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Scripture: Genesis 1:1 — "In the beginning God created the heaven and the earth"
; Principle: This is THE beginning for assembly boot code. Just as bootinfo.h
;            anchors C code, this include anchors assembly boot stages.

; ═══════════════════════════════════════════════════════════════════════════════
; M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Other Includes:
;   - None (self-contained)
;
; C Header Equivalent:
;   - boot/include/bootinfo.h — MUST stay synchronized
;
; Used by:
;   - boot/stage0/stage0a.asm — Boot sector
;   - boot/stage0/stage0b.asm — Second stage loader
;   - kernel/entry.asm — Kernel entry point
;
; Usage:
;   %include "boot/include/bootinfo.inc"

; ═══════════════════════════════════════════════════════════════════════════════
; M.10 ROADMAP — Version History [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; History:
;   a-01.00 (2026-01-31) — Initial creation with basic constants
;   a-02.00 (2026-01-31) — Full alignment with INC template, version 3 support

; ═══════════════════════════════════════════════════════════════════════════════
; END METADATA [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; SETUP BLOCK [SETUP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Configuration before definitions — include guard.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; CONTAINS:
;   - S.1 GUARD    — Include guard (%ifndef/%define)
;   - S.2 INCLUDES — [None: Self-contained]
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; S.1 GUARD — Include Guard [GUARD]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Prevents multiple inclusion errors.
;
; PURPOSE: NASM uses %ifndef/%define pattern for include guards.
;          Guard name matches C header pattern: MILLENNIUMOS_BOOTINFO_INC
;
; ═══════════════════════════════════════════════════════════════════════════════

%ifndef MILLENNIUMOS_BOOTINFO_INC
%define MILLENNIUMOS_BOOTINFO_INC

; ═══════════════════════════════════════════════════════════════════════════════
; S.2 INCLUDES — Required Include Dependencies [INCLUDES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Other include files this one depends on.
;
; [None: bootinfo.inc is self-contained — foundational definitions]
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; END SETUP [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; BODY BLOCK [BODY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief The actual work — constant definitions (includes don't contain code).
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; CONTAINS:
;   - B.1 ORG CHART  — Definitions structure overview
;   - B.2 CONSTANTS  — EQU definitions mirroring bootinfo.h S.2 DEFINES
;   - B.3 MACROS     — [Reserved: None currently needed]
;   - B.4 STRUCTURES — [Reserved: Offsets defined as constants]
;   - B.5 EXTERNAL   — [Reserved: Include file]
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; B.1 ORG CHART — Definitions Structure [ORGCHART]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Map what this include provides — mirrors bootinfo.h S.2 DEFINES.
;
; Definitions Provided (by category):
;
;   Constants (EQU) — 35+ definitions
;   ├── B.2a ADDRESS — Boot info memory location
;   │   └── MOS_BOOT_INFO_ADDR = 0x9000
;   ├── B.2b SIZES — Structure and allocation sizes
;   │   ├── MOS_BOOT_INFO_SIZE = 8192
;   │   └── MOS_BOOT_INFO_PAGES = 2
;   ├── B.2c MAGIC — Identity constants
;   │   ├── MOS_BOOT_MAGIC = 0x4D4F5342
;   │   └── MOS_BOOT_VERSION = 3
;   ├── B.2d TERNARY — Boot source and state values
;   │   ├── MOS_BOOT_SOURCE_* (-1/0/+1)
;   │   ├── MOS_BOOT_STATE_* (-1/0/+1)
;   │   └── MOS_CAP_* (capability flags)
;   └── B.2e OFFSETS — MosBootInfo field offsets
;       ├── BOOTINFO_OFF_MAGIC = 0x00
;       ├── BOOTINFO_OFF_VERSION = 0x04
;       └── ... (all field offsets)
;
; Definition Counts:
;   35+ constants total
;   0 macros
;   0 structures (offsets as constants instead)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; B.2 CONSTANTS — EQU Definitions [CONSTANTS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Named compile-time values mirroring bootinfo.h S.2 DEFINES.
;
; PURPOSE: Provide assembly access to boot info constants. All values MUST
;          match bootinfo.h exactly — this is a MIRROR, not a copy.
;
; IMPORTANT: When bootinfo.h changes, update this file to match!
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.2a ADDRESS — Boot Info Memory Location [ADDRESS]
; ───────────────────────────────────────────────────────────────────────────────
;
; Fixed physical address for boot information handoff.
;
; WHY 0x9000?
;   - Below 1MB: Accessible in all CPU modes (real, protected, long)
;   - Above IVT (0x0000-0x03FF) and BDA (0x0400-0x04FF)
;   - 9 × 0x1000 = 9 × 4KB = ternary alignment (9 = 3²)
;   - Safe region in conventional memory
;
; MIRRORS: bootinfo.h S.2a ADDRESS — MOS_BOOT_INFO_ADDR
; ───────────────────────────────────────────────────────────────────────────────

; MOS_BOOT_INFO_ADDR — Fixed handoff address (THE anchor)
;
; This is THE address where bootloaders place MosBootInfo and where
; the kernel expects to find it. Like Genesis 1:1 anchors Scripture,
; this address anchors the boot handoff.
;
; Usage:
;   mov edi, MOS_BOOT_INFO_ADDR      ; Point to boot info
;   mov eax, [edi]                    ; Read magic number
;
MOS_BOOT_INFO_ADDR      equ 0x9000

; ───────────────────────────────────────────────────────────────────────────────
; B.2b SIZES — Structure and Allocation Sizes [SIZES]
; ───────────────────────────────────────────────────────────────────────────────
;
; Memory sizes for boot info allocation.
;
; MIRRORS: bootinfo.h S.2b SIZES
; ───────────────────────────────────────────────────────────────────────────────

; MOS_BOOT_INFO_SIZE — Total allocation size in bytes
;
; 8KB (2 pages) reserved for MosBootInfo structure.
; Actual structure is ~7.5KB; extra space for future expansion.
;
MOS_BOOT_INFO_SIZE      equ 8192

; MOS_BOOT_INFO_PAGES — Pages needed for boot info
;
; 2 × 4KB pages = 8KB total allocation.
;
MOS_BOOT_INFO_PAGES     equ 2

; MOS_MEMORY_MAP_MAX — Maximum memory map entries
;
; 256 entries × 24 bytes = 6144 bytes for memory map.
;
MOS_MEMORY_MAP_MAX      equ 256

; MOS_CMDLINE_MAX — Maximum command line length
;
; 256 bytes for null-terminated boot command line.
;
MOS_CMDLINE_MAX         equ 256

; ───────────────────────────────────────────────────────────────────────────────
; B.2c MAGIC — Identity Constants [MAGIC]
; ───────────────────────────────────────────────────────────────────────────────
;
; Magic number and version for structure validation.
;
; MIRRORS: bootinfo.h S.2c MAGIC
; ───────────────────────────────────────────────────────────────────────────────

; MOS_BOOT_MAGIC — "MOSB" signature (MillenniumOS Boot)
;
; Little-endian: 'M'=0x4D, 'O'=0x4F, 'S'=0x53, 'B'=0x42
; Stored as: 0x42534F4D (reading bytes 0x4D,0x4F,0x53,0x42)
;
; Validation:
;   mov eax, [MOS_BOOT_INFO_ADDR]
;   cmp eax, MOS_BOOT_MAGIC
;   jne .invalid_boot_info
;
MOS_BOOT_MAGIC          equ 0x42534F4D

; MOS_BOOT_VERSION — Current structure version
;
; Version history:
;   1: Initial structure
;   2: Added framebuffer color fields
;   3: Added SMBIOS pointer and boot timing (CURRENT)
;
MOS_BOOT_VERSION        equ 3

; ───────────────────────────────────────────────────────────────────────────────
; B.2d TERNARY — Boot Source, State, and Capability Values [TERNARY]
; ───────────────────────────────────────────────────────────────────────────────
;
; Ternary-aligned values for boot semantics (-1/0/+1 pattern).
;
; MIRRORS: bootinfo.h S.2d TERNARY
; ───────────────────────────────────────────────────────────────────────────────

; ─── B.2d-1 BOOT SOURCE (ternary: old/neutral/new) ───

; MOS_BOOT_SOURCE_BIOS — Legacy BIOS boot (-1)
;
; Ternary -1: The "old way" — BIOS is legacy, moving from void.
;
MOS_BOOT_SOURCE_BIOS    equ -1

; MOS_BOOT_SOURCE_UNKNOWN — Boot source unknown (0)
;
; Ternary 0: Neutral — source not determined.
;
MOS_BOOT_SOURCE_UNKNOWN equ 0

; MOS_BOOT_SOURCE_UEFI — Modern UEFI boot (+1)
;
; Ternary +1: The "new way" — UEFI is modern, moving toward tov.
;
MOS_BOOT_SOURCE_UEFI    equ 1

; ─── B.2d-2 BOOT STATE (ternary: error/ok/optimal) ───

; MOS_BOOT_STATE_ERROR — Boot had errors (-1)
;
; Ternary -1: Degraded boot — some features unavailable.
;
MOS_BOOT_STATE_ERROR    equ -1

; MOS_BOOT_STATE_OK — Boot completed normally (0)
;
; Ternary 0: Baseline — all required features present.
;
MOS_BOOT_STATE_OK       equ 0

; MOS_BOOT_STATE_OPTIMAL — Boot was optimal (+1)
;
; Ternary +1: All features initialized — best case.
;
MOS_BOOT_STATE_OPTIMAL  equ 1

; ─── B.2d-3 CAPABILITY FLAGS ───

; MOS_CAP_FRAMEBUFFER — Framebuffer info valid (bit 0)
;
; When set: framebuffer structure contains valid display info.
;
MOS_CAP_FRAMEBUFFER     equ (1 << 0)

; MOS_CAP_MEMMAP — Memory map valid (bit 1)
;
; When set: memory_map array and memory_map_count are valid.
;
MOS_CAP_MEMMAP          equ (1 << 1)

; MOS_CAP_ACPI — RSDP address valid (bit 2)
;
; When set: rsdp_address contains valid ACPI RSDP pointer.
;
MOS_CAP_ACPI            equ (1 << 2)

; MOS_CAP_UEFI_RT — UEFI runtime services available (bit 3)
;
; When set: uefi_system_table contains valid System Table pointer.
;
MOS_CAP_UEFI_RT         equ (1 << 3)

; MOS_CAP_INITRD — Initial ramdisk present (bit 4)
;
; When set: initrd_address and initrd_size are valid.
;
MOS_CAP_INITRD          equ (1 << 4)

; MOS_CAP_SMBIOS — SMBIOS pointer valid (bit 5)
;
; When set: smbios_address contains valid SMBIOS entry point.
;
MOS_CAP_SMBIOS          equ (1 << 5)

; MOS_CAP_BOOT_TIMING — Boot timing fields valid (bit 6)
;
; When set: tsc_stage0a_start through tsc_kernel_load are valid.
;
MOS_CAP_BOOT_TIMING     equ (1 << 6)

; ───────────────────────────────────────────────────────────────────────────────
; B.2e OFFSETS — MosBootInfo Structure Field Offsets [OFFSETS]
; ───────────────────────────────────────────────────────────────────────────────
;
; Byte offsets for direct field access in assembly.
; These MUST match bootinfo.h MosBootInfo struct layout exactly.
;
; Usage:
;   mov edi, MOS_BOOT_INFO_ADDR
;   mov eax, [edi + BOOTINFO_OFF_MAGIC]     ; Read magic
;   mov bl, [edi + BOOTINFO_OFF_VERSION]    ; Read version
;
; MIRRORS: bootinfo.h MosBootInfo struct field offsets
;          Verified by X.1 VALIDATION static assertions in bootinfo.h
; ───────────────────────────────────────────────────────────────────────────────

; ─── SECTION 1: HEADER (offset 0x0000, 64 bytes) ───

; BOOTINFO_OFF_MAGIC — uint32_t magic (4 bytes)
;
; Offset: 0x00 | Must equal MOS_BOOT_MAGIC for valid boot info.
;
BOOTINFO_OFF_MAGIC          equ 0x00

; BOOTINFO_OFF_VERSION — uint8_t version (1 byte)
;
; Offset: 0x04 | Structure version (MOS_BOOT_VERSION).
;
BOOTINFO_OFF_VERSION        equ 0x04

; BOOTINFO_OFF_BOOT_SOURCE — int8_t boot_source (1 byte, SIGNED)
;
; Offset: 0x05 | Ternary: -1=BIOS, 0=unknown, +1=UEFI.
;
BOOTINFO_OFF_BOOT_SOURCE    equ 0x05

; BOOTINFO_OFF_BOOT_STATE — int8_t boot_state (1 byte, SIGNED)
;
; Offset: 0x06 | Ternary: -1=error, 0=ok, +1=optimal.
;
BOOTINFO_OFF_BOOT_STATE     equ 0x06

; BOOTINFO_OFF_CAPABILITIES — uint32_t capabilities (4 bytes)
;
; Offset: 0x08 | Capability flags (MOS_CAP_*).
;
BOOTINFO_OFF_CAPABILITIES   equ 0x08

; BOOTINFO_OFF_MMAP_COUNT — uint32_t memory_map_count (4 bytes)
;
; Offset: 0x0C | Number of valid memory map entries.
;
BOOTINFO_OFF_MMAP_COUNT     equ 0x0C

; BOOTINFO_OFF_TOTAL_MEMORY — uint64_t total_memory (8 bytes)
;
; Offset: 0x10 | Total usable memory in bytes.
;
BOOTINFO_OFF_TOTAL_MEMORY   equ 0x10

; BOOTINFO_OFF_KERNEL_PHYS — uint64_t kernel_physical (8 bytes)
;
; Offset: 0x18 | Kernel physical load address.
;
BOOTINFO_OFF_KERNEL_PHYS    equ 0x18

; BOOTINFO_OFF_KERNEL_SIZE — uint64_t kernel_size (8 bytes)
;
; Offset: 0x20 | Kernel size in bytes.
;
BOOTINFO_OFF_KERNEL_SIZE    equ 0x20

; BOOTINFO_OFF_BOOT_TSC — uint64_t boot_tsc (8 bytes)
;
; Offset: 0x28 | TSC at boot start (stage0a).
;
BOOTINFO_OFF_BOOT_TSC       equ 0x28

; BOOTINFO_OFF_ENTRY_TSC — uint64_t entry_tsc (8 bytes)
;
; Offset: 0x30 | TSC at kernel entry.
;
BOOTINFO_OFF_ENTRY_TSC      equ 0x30

; ─── SECTION 2: FRAMEBUFFER (offset 0x0040, 48 bytes) ───

; BOOTINFO_OFF_FB_ADDR — uint64_t framebuffer.address (8 bytes)
;
; Offset: 0x40 | Framebuffer physical address (64-bit).
;
BOOTINFO_OFF_FB_ADDR        equ 0x40

; BOOTINFO_OFF_FB_WIDTH — uint32_t framebuffer.width (4 bytes)
;
; Offset: 0x48 | Horizontal resolution in pixels.
;
BOOTINFO_OFF_FB_WIDTH       equ 0x48

; BOOTINFO_OFF_FB_HEIGHT — uint32_t framebuffer.height (4 bytes)
;
; Offset: 0x4C | Vertical resolution in pixels.
;
BOOTINFO_OFF_FB_HEIGHT      equ 0x4C

; BOOTINFO_OFF_FB_PITCH — uint32_t framebuffer.pitch (4 bytes)
;
; Offset: 0x50 | Bytes per scanline.
;
BOOTINFO_OFF_FB_PITCH       equ 0x50

; BOOTINFO_OFF_FB_BPP — uint32_t framebuffer.bpp (4 bytes)
;
; Offset: 0x54 | Bits per pixel.
;
BOOTINFO_OFF_FB_BPP         equ 0x54

; BOOTINFO_OFF_FB_RED_SIZE — uint8_t framebuffer.red_size (1 byte)
;
; Offset: 0x58 | Red channel bits.
;
BOOTINFO_OFF_FB_RED_SIZE    equ 0x58

; BOOTINFO_OFF_FB_RED_SHIFT — uint8_t framebuffer.red_shift (1 byte)
;
; Offset: 0x59 | Red channel position.
;
BOOTINFO_OFF_FB_RED_SHIFT   equ 0x59

; BOOTINFO_OFF_FB_GREEN_SIZE — uint8_t framebuffer.green_size (1 byte)
;
; Offset: 0x5A | Green channel bits.
;
BOOTINFO_OFF_FB_GREEN_SIZE  equ 0x5A

; BOOTINFO_OFF_FB_GREEN_SHIFT — uint8_t framebuffer.green_shift (1 byte)
;
; Offset: 0x5B | Green channel position.
;
BOOTINFO_OFF_FB_GREEN_SHIFT equ 0x5B

; BOOTINFO_OFF_FB_BLUE_SIZE — uint8_t framebuffer.blue_size (1 byte)
;
; Offset: 0x5C | Blue channel bits.
;
BOOTINFO_OFF_FB_BLUE_SIZE   equ 0x5C

; BOOTINFO_OFF_FB_BLUE_SHIFT — uint8_t framebuffer.blue_shift (1 byte)
;
; Offset: 0x5D | Blue channel position.
;
BOOTINFO_OFF_FB_BLUE_SHIFT  equ 0x5D

; ─── SECTION 3: MEMORY ZONES (offset 0x0070, 32 bytes) ───

; BOOTINFO_OFF_ZONE_VOID_END — uint64_t zone_void_end (8 bytes)
;
; Offset: 0x70 | End of void zone (typically 1MB).
; Ternary: -1 zone boundary.
;
BOOTINFO_OFF_ZONE_VOID_END  equ 0x70

; BOOTINFO_OFF_ZONE_WORD_END — uint64_t zone_word_end (8 bytes)
;
; Offset: 0x78 | End of word zone (kernel space end).
; Ternary: 0 zone boundary.
;
BOOTINFO_OFF_ZONE_WORD_END  equ 0x78

; ─── SECTION 5: MEMORY MAP (offset 0x0100, 6144 bytes) ───

; BOOTINFO_OFF_MEMORY_MAP — MosMemoryRegion[] memory_map
;
; Offset: 0x100 | Array of 256 memory region entries (24 bytes each).
;
BOOTINFO_OFF_MEMORY_MAP     equ 0x100

; ─── MEMORY REGION STRUCTURE (24 bytes per entry) ───

; MosMemoryRegion field offsets (within each entry):
;   base:       offset 0, 8 bytes (uint64_t)
;   length:     offset 8, 8 bytes (uint64_t)
;   type:       offset 16, 4 bytes (uint32_t)
;   attributes: offset 20, 4 bytes (uint32_t)
;
MEMREGION_OFF_BASE          equ 0
MEMREGION_OFF_LENGTH        equ 8
MEMREGION_OFF_TYPE          equ 16
MEMREGION_OFF_ATTRIBUTES    equ 20
MEMREGION_SIZE              equ 24

; ───────────────────────────────────────────────────────────────────────────────
; B.2f BOOT EVENTS — DAR Witness Boot Event Fields [BOOTEVENTS]
; ───────────────────────────────────────────────────────────────────────────────
;
; Boot event buffer for DAR Witness forensic logging.
; Allows assembly code to log events that the kernel imports.
;
; MIRRORS: bootinfo.h boot_event_count and boot_events[] offsets
; ───────────────────────────────────────────────────────────────────────────────

; BOOTINFO_OFF_EVENT_COUNT — uint32_t boot_event_count (4 bytes)
;
; Offset: 0x90 | Number of valid events in boot_events array.
;
BOOTINFO_OFF_EVENT_COUNT    equ 0x90

; BOOTINFO_OFF_RESERVED_94 — uint16_t reserved (2 bytes)
;
; Offset: 0x94 | Reserved for alignment.
;
BOOTINFO_OFF_RESERVED_94    equ 0x94

; BOOTINFO_OFF_RESERVED_96 — uint16_t reserved (2 bytes)
;
; Offset: 0x96 | Reserved for alignment.
;
BOOTINFO_OFF_RESERVED_96    equ 0x96

; BOOTINFO_OFF_EVENTS — MosBootEvent[6] boot_events (96 bytes)
;
; Offset: 0x98 | Array of 6 boot event entries (16 bytes each).
;
BOOTINFO_OFF_EVENTS         equ 0x98

; ─── BOOT EVENT STRUCTURE (16 bytes per entry) ───

; MosBootEvent field offsets (within each entry):
;   tsc:        offset 0, 8 bytes (uint64_t) — TSC timestamp
;   event_code: offset 8, 2 bytes (uint16_t) — Phase + Step code
;   status:     offset 10, 2 bytes (uint16_t) — Result code
;   value:      offset 12, 4 bytes (uint32_t) — Optional data
;
BOOTEVENT_OFF_TSC           equ 0
BOOTEVENT_OFF_CODE          equ 8
BOOTEVENT_OFF_STATUS        equ 10
BOOTEVENT_OFF_VALUE         equ 12
BOOTEVENT_SIZE              equ 16

; Maximum boot events
MOS_MAX_BOOT_EVENTS         equ 6

; ─── BOOT EVENT STATUS CODES ───

MOS_EVT_STATUS_OK           equ 0x0000  ; Success
MOS_EVT_STATUS_WARN         equ 0x0001  ; Warning (continued)
MOS_EVT_STATUS_FAIL         equ 0xFFFF  ; Failure

; ─── BOOT EVENT PHASE CODES (high byte of event_code) ───

MOS_EVT_PHASE_STAGE0A       equ 0x00    ; Stage 0a (boot sector)
MOS_EVT_PHASE_STAGE0B       equ 0x10    ; Stage 0b (second stage)
MOS_EVT_PHASE_UEFI          equ 0x20    ; UEFI bootloader
MOS_EVT_PHASE_ENTRY         equ 0x30    ; Kernel entry (entry.asm)
MOS_EVT_PHASE_KERNEL        equ 0x40    ; Kernel init (kernel.c)

; ─── KERNEL ENTRY STEP CODES (low byte for PHASE_ENTRY) ───

MOS_EVT_ENTRY_START         equ 0x31    ; Entry point reached
MOS_EVT_ENTRY_STACK         equ 0x32    ; Stack setup complete
MOS_EVT_ENTRY_BSS           equ 0x33    ; BSS cleared
MOS_EVT_ENTRY_VALIDATE      equ 0x34    ; Validation passed
MOS_EVT_ENTRY_TSC           equ 0x35    ; TSC recorded
MOS_EVT_ENTRY_CALL          equ 0x36    ; About to call kernel

; ═══════════════════════════════════════════════════════════════════════════════
; B.3 MACROS — Reusable Code Patterns [MACROS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief [Reserved: No macros currently needed]
;
; Future candidates:
;   - VALIDATE_BOOT_INFO — check magic and version
;   - GET_BOOT_FIELD — safe field access with base pointer
;
; ═══════════════════════════════════════════════════════════════════════════════

; [Reserved: Add macros as patterns emerge]

; ═══════════════════════════════════════════════════════════════════════════════
; B.4 STRUCTURES — Data Structure Layouts [STRUCTURES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief [Reserved: Offsets defined as constants in B.2e instead]
;
; MosBootInfo layout is documented via BOOTINFO_OFF_* constants.
; This approach is simpler for assembly access than NASM struc.
;
; ═══════════════════════════════════════════════════════════════════════════════

; [Reserved: Using offset constants instead of struc]

; ═══════════════════════════════════════════════════════════════════════════════
; B.5 EXTERNAL — Symbol Declarations [EXTERNAL]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief [Reserved: Include file — no symbol declarations]
;
; Symbol declarations belong in source files, not includes.
;
; ═══════════════════════════════════════════════════════════════════════════════

; [Reserved: Symbols declared in consuming .asm files]

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
;   - X.1 VALIDATION — Synchronization verification
;   - X.2-X.3        — [Reserved: Include file]
;   - X.4 OVERVIEW   — Include summary
;   - X.5 POLICY     — Modification guidelines
;   - X.6 FLOW       — Dependency chain
;   - X.7 PERF       — Performance considerations
;   - X.8 DEBUG      — Troubleshooting guide
;   - X.9 RELATED    — Related components
;   - X.10 ROADMAP   — Future expansions
;   - X.11 REFERENCE — Quick reference patterns
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.1 VALIDATION — Synchronization Verification [VALIDATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Prove correctness — verify synchronization with bootinfo.h.
;
; CRITICAL: This file MUST stay synchronized with bootinfo.h!
;
; Manual Verification Checklist:
;   [ ] MOS_BOOT_INFO_ADDR matches bootinfo.h
;   [ ] MOS_BOOT_MAGIC matches bootinfo.h
;   [ ] MOS_BOOT_VERSION matches bootinfo.h (currently 3)
;   [ ] All BOOTINFO_OFF_* values match C struct offsets
;   [ ] All MOS_CAP_* flags match bootinfo.h
;   [ ] MOS_BOOT_SOURCE_* values match bootinfo.h
;   [ ] MOS_BOOT_STATE_* values match bootinfo.h
;
; Automated Verification:
;   bootinfo.h X.1 VALIDATION contains static assertions that verify
;   structure offsets at compile time. If bootinfo.h compiles, the
;   offsets are correct. This file must match those values.
;
; Test Include:
;   %include "boot/include/bootinfo.inc"
;   section .text
;   global _start
;   _start:
;       mov eax, MOS_BOOT_MAGIC           ; Test constant access
;       mov ebx, MOS_BOOT_INFO_ADDR       ; Test address
;       mov ecx, BOOTINFO_OFF_FB_ADDR     ; Test offset
;       hlt
;
;   nasm -f elf64 test.asm -o test.o      ; Must assemble without errors
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.2-X.3 — [Reserved: Include File] [RESERVED]
; ═══════════════════════════════════════════════════════════════════════════════
;
; X.2 EXECUTION: [Reserved: Includes don't execute]
; X.3 CLEANUP:   [Reserved: Compile-time only]
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.4 OVERVIEW — Include Summary [OVERVIEW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief High-level summary.
;
; PURPOSE: Assembly mirror of bootinfo.h — provides identical constants
;          for assembly boot code (stage0a, stage0b, kernel entry).
;
; PROVIDES:
;   - Boot info address (MOS_BOOT_INFO_ADDR = 0x9000)
;   - Magic number and version for validation
;   - Ternary boot source/state values
;   - All MosBootInfo field offsets for direct memory access
;   - Capability flags for feature detection
;
; ARCHITECTURE: Rails — foundational definitions that assembly code depends on.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.5 POLICY — Modification Guidelines [POLICY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Guide future maintainers on what's safe to change.
;
; Safe to Modify:
;   ✅ Add new constants (must also add to bootinfo.h)
;   ✅ Add new offset definitions (must match bootinfo.h struct)
;   ✅ Improve documentation/comments
;
; Modify with Extreme Care:
;   ⚠️ Changing constant values — must update bootinfo.h to match
;   ⚠️ Changing offset values — breaks assembly/C ABI
;   ⚠️ Removing constants — breaks consuming assembly code
;
; NEVER Modify Without Updating bootinfo.h:
;   ❌ MOS_BOOT_INFO_ADDR — breaks entire boot chain
;   ❌ MOS_BOOT_MAGIC — breaks boot validation
;   ❌ Any BOOTINFO_OFF_* value — breaks structure access
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.6 FLOW — Dependency Chain [FLOW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Include and mirroring relationships.
;
; Mirror Source:
;   bootinfo.h (C header)
;        ↓ (mirrors)
;   bootinfo.inc (assembly)  ← YOU ARE HERE
;
; Consumers:
;   bootinfo.inc
;        ↓ (%include)
;   ├── stage0a.asm — uses MOS_BOOT_INFO_ADDR, offset constants
;   ├── stage0b.asm — fills MosBootInfo fields using offsets
;   └── entry.asm   — validates boot info, reads fields
;
; Synchronization:
;   When bootinfo.h changes → update bootinfo.inc → rebuild all .asm
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.7 PERF — Performance Considerations [PERF]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Zero runtime overhead — compile-time only.
;
; All definitions are EQU constants:
;   - Assembled as immediate values in instructions
;   - No memory access for constant values
;   - Zero runtime overhead
;
; Assembly time impact:
;   - ~50 constant definitions = negligible assembly time
;   - Include guard prevents redundant processing
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.8 DEBUG — Troubleshooting Guide [DEBUG]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Common issues and debugging techniques.
;
; Problem: "Boot info validation fails"
;   Cause:    MOS_BOOT_MAGIC or MOS_BOOT_VERSION doesn't match bootinfo.h
;   Solution: Verify constants match, rebuild both C and assembly
;
; Problem: "Wrong field value read from boot info"
;   Cause:    BOOTINFO_OFF_* doesn't match actual struct offset
;   Solution: Check bootinfo.h struct layout, verify offset calculation
;
; Problem: "Assembler error: symbol redefined"
;   Cause:    Include guard not working or duplicate %include
;   Solution: Check %ifndef/%define pattern at top of file
;
; Debug: Verify offset at runtime
;   mov edi, MOS_BOOT_INFO_ADDR
;   mov eax, [edi + BOOTINFO_OFF_MAGIC]
;   ; EAX should be 0x42534F4D ("MOSB")
;   ; If not, offset is wrong or boot info not initialized
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.9 RELATED — Related Components [RELATED]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Related files and dependencies.
;
; C Header (source of truth):
;   - boot/include/bootinfo.h — MUST stay synchronized
;
; Assembly Consumers:
;   - boot/stage0/stage0a.asm — Boot sector
;   - boot/stage0/stage0b.asm — Second stage loader
;   - kernel/entry.asm — Kernel entry point
;
; Build System:
;   - Makefile — assembles .asm files with -I include path
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.10 ROADMAP — Future Expansions [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Planned definitions and improvements.
;
; Completed:
;   ✓ Basic constants (address, magic, version)
;   ✓ Boot source ternary values
;   ✓ Capability flags
;   ✓ All header field offsets
;   ✓ Framebuffer field offsets
;   ✓ Memory zone offsets
;   ✓ Version 3 support (SMBIOS, boot timing)
;
; Planned:
;   ⏳ Extended info field offsets (initrd, ACPI, SMBIOS)
;   ⏳ Boot timing field offsets
;   ⏳ Validation macros
;   ⏳ Memory type enum values
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.11 REFERENCE — Quick Reference [REFERENCE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Copy-paste ready patterns for common operations.
;
; Include:
;   %include "boot/include/bootinfo.inc"
;
; Validate boot info:
;   mov edi, MOS_BOOT_INFO_ADDR
;   mov eax, [edi + BOOTINFO_OFF_MAGIC]
;   cmp eax, MOS_BOOT_MAGIC
;   jne .invalid_boot_info
;   mov al, [edi + BOOTINFO_OFF_VERSION]
;   cmp al, 2
;   jb .version_too_old
;
; Read boot source:
;   mov al, [edi + BOOTINFO_OFF_BOOT_SOURCE]
;   cmp al, MOS_BOOT_SOURCE_UEFI
;   je .uefi_boot
;   cmp al, MOS_BOOT_SOURCE_BIOS
;   je .bios_boot
;
; Read framebuffer address:
;   mov rax, [edi + BOOTINFO_OFF_FB_ADDR]
;
; Check capability:
;   mov eax, [edi + BOOTINFO_OFF_CAPABILITIES]
;   test eax, MOS_CAP_ACPI
;   jz .no_acpi
;
; Iterate memory map:
;   mov ecx, [edi + BOOTINFO_OFF_MMAP_COUNT]
;   lea esi, [edi + BOOTINFO_OFF_MEMORY_MAP]
;   .loop:
;       mov rax, [esi + MEMREGION_OFF_BASE]
;       mov rbx, [esi + MEMREGION_OFF_LENGTH]
;       ; ... process entry ...
;       add esi, MEMREGION_SIZE
;       loop .loop
;
; Closing Note:
;   This include is RAILS — foundational definitions that assembly boot
;   code depends on. It mirrors bootinfo.h exactly, ensuring C and
;   assembly code use identical values for boot handoff.
;
;   Modify thoughtfully — changes affect the entire boot chain.
;   Always update bootinfo.h first, then sync this file.
;
;   "In the beginning God created the heaven and the earth." — Genesis 1:1
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; END CLOSING [END]
; ═══════════════════════════════════════════════════════════════════════════════

%endif ; MILLENNIUMOS_BOOTINFO_INC
