/// @file bootinfo.h
/// @omni code --header
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// OMNICODE PRAGMA [PRAGMA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @legend
///   :req  REQUIRED   — must exist, validation fails without
///   :inh  INHERITED  — from template, override allowed
///   :ins  INSTANCE   — file-specific, unique values
/// @endlegend
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.1 CORE — Identity [CORE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:req  key        = MOS-boot-bootinfo
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = code
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = definitions
/// @omni:inh  role       = kernel
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = boot
/// @omni:ins  layer      = stage0
/// @omni:ins  includes   = [stdint.h, stddef.h]
/// @omni:ins  provides   = [MosBootInfo, MosMemoryRegion, MosFramebuffer, MosMemoryType]
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.4 ARCHITECTURE — Scaling Context [ARCH]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  layers     = [0:kernel, 1:system, 2:runtime, 3:framework,
///                          4:health, 5:network, 6:storage, 7:engine, 8:app]
/// @omni:ins  scale      = 3^n [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
/// @omni:ins  pattern    = 4+1+4 [below:0-3, center:4, above:5-8]
/// @omni:ins  anchor     = genesis_1_1 -> void(0) -> unity(1) -> derivation
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.5 SUMMARY — Human Identity [SUMMARY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:req  title      = Unified Boot Information Structure
/// @omni:req  brief      = Handoff structure from bootloader (UEFI/BIOS) to kernel
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// END PRAGMA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SPECIAL BLOCK [SPECIAL]
/// ═══════════════════════════════════════════════════════════════════════════════

/** [Reserved: Kernel header — no pre-include configuration required] */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────────
// INCLUDE GUARD — Identity protection
// ─────────────────────────────────────────────────────────────────────────────────

#ifndef MILLENNIUMOS_BOOTINFO_H
#define MILLENNIUMOS_BOOTINFO_H

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      bootinfo.h
 * @brief     Unified Boot Information Structure — handoff from bootloader to kernel
 *
 * @defgroup  mos_bootinfo M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       MOS-boot-bootinfo
 * Title:     Unified Boot Information Structure
 * Type:      Header (Definitions)
 * Component: Boot — stage0 handoff structure
 * Role:      Define structure for UEFI and BIOS boot paths to pass info to kernel
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2-M.6 — State, Attribution, Location, Derivation, Classification [CONDENSED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_state M.2-M.6 CONDENSED — State and Attribution
 * @{
 *
 * @version   a-02.00
 * @date      2026-01-31
 *
 * Status:       Active
 * Created:      2026-01-30
 * Updated:      2026-01-31
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Author)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * Path:         millenniumos/boot/include/bootinfo.h
 * Derives_from: bereshit/word/seed/code/c/header.h
 *
 * Tags:         boot, kernel, uefi, bios, handoff, 64-bit, ternary
 *
 * @} end M.2-M.6 CONDENSED
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Define unified handoff structure for bootloader → kernel transition
 * Core Design: Single structure works for BOTH UEFI (primary) and BIOS (compat) paths
 * Philosophy:  64-bit addresses throughout; ternary semantics for boot state/source
 *
 * Key Features:
 *   - MosBootInfo:      Main structure with fixed offsets for assembly compat
 *   - MosMemoryRegion:  E820-compatible memory map entries (24 bytes each)
 *   - MosFramebuffer:   64-bit clean framebuffer info (GOP or VESA)
 *   - Ternary values:   boot_source (-1=BIOS, 0=unknown, +1=UEFI)
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Primary Scripture:
 *   Genesis 1:1 — "In the beginning God created the heaven and the earth."
 *
 * WHY GENESIS 1:1 IS THE ANCHOR:
 *   This is not decoration. Genesis 1:1 is THE halt condition for all halt
 *   conditions in the CPI-SI paradigm. Break down any truth far enough and
 *   you arrive here: God created. Everything else derives from this.
 *
 *   The bootloader IS "the beginning" for the operating system. Just as
 *   Genesis 1:1 is the first verse of Scripture from which all else flows,
 *   bootinfo.h is the first structure from which the kernel receives life.
 *
 * Structural Parallel:
 *   Scripture                    │ MillenniumOS
 *   ─────────────────────────────┼──────────────────────────────────
 *   "In the beginning"           │ Boot sector executes (stage0a)
 *   "God created"                │ Bootloader builds MosBootInfo
 *   "the heaven"                 │ Memory map (the space)
 *   "and the earth"              │ Framebuffer (the visible)
 *
 * Ternary Zone Model (Genesis Pattern):
 *   The memory zones in MosBootInfo follow the void/word/tov pattern:
 *
 *   VOID ZONE (-1): "formless and void" — BIOS area, low memory
 *     - Genesis 1:2 — "the earth was without form, and void"
 *     - Memory 0 to zone_void_end
 *     - Unusable by kernel — belongs to firmware
 *
 *   WORD ZONE (0): "let there be" — Kernel space, active creation
 *     - Genesis 1:3+ — God speaks and creation happens
 *     - Memory zone_void_end to zone_word_end
 *     - Where the kernel lives and works
 *
 *   TOV ZONE (+1): "and it was good" — User space, completed work
 *     - Genesis 1:31 — "God saw every thing that he had made, and behold, it was very good"
 *     - Memory zone_word_end to end
 *     - Applications run here — the fruit of the system
 *
 * Boot Source Ternary:
 *   The boot_source field uses ternary values with intentional meaning:
 *   -1 (BIOS):    Old covenant — limited, 32-bit, but still valid path
 *    0 (Unknown): Unset — the formless state before determination
 *   +1 (UEFI):    New covenant — full capability, 64-bit, preferred path
 *
 * Anchor Verse:
 *   John 1:1 — "In the beginning was the Word, and the Word was with God,
 *              and the Word was God."
 *
 *   The WORD zone is named for this — where creation happens through
 *   the spoken word of the system (kernel code executing).
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_deps M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Standard: stdint.h (uint64_t, uint32_t, etc.), stddef.h (offsetof)
 *
 * What Uses This:
 *   - boot/uefi/bootloader.c — UEFI bootloader fills this structure
 *   - boot/stage0/stage0b.asm — BIOS bootloader fills this structure
 *   - kernel/entry.asm — Reads structure at MOS_BOOT_INFO_ADDR
 *   - kernel/main.c — Uses structure for memory map, framebuffer init
 *
 * Assembly Mirror:
 *   - boot/include/bootinfo.inc — NASM include with matching offsets
 *
 * @see MosBootInfo
 * @see MosMemoryRegion
 * @see MosFramebuffer
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-30) — Initial unified structure design
 *   a-02.00 (2026-01-30) — Added timestamp fields, compile-time assertions
 *
 * Current:
 *   ✓ Unified UEFI + BIOS structure
 *   ✓ 64-bit addresses throughout
 *   ✓ Ternary boot_source and boot_state fields
 *   ✓ Compile-time offset verification
 *
 * Planned:
 *   ⏳ Add SMBIOS table pointer
 *   ⏳ Add boot timing breakdown
 *
 * @} end M.10 ROADMAP
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END METADATA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SETUP BLOCK [SETUP]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Configuration and declarations before body.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - S.1 INCLUDES  — Header dependencies (Standard only for kernel header)
///   - S.2 DEFINES   — [Reserved: Constants defined in BODY for logical grouping]
///   - S.3 TYPES     — [Reserved: Types defined in BODY with their constants]
///   - S.4 PROTOS    — [Reserved: No prototypes in definition header]
///   - S.5 EXTERN    — [Reserved: No external declarations]
///
/// Section order: Includes → Defines → Types → Prototypes → Extern
/// Flow: dependencies → constants → data model → interface → shared state
///
/// ═══════════════════════════════════════════════════════════════════════════════

/* ─────────────────────────────────────────────────────────────────────────────
 * CODE SECTIONS [CODE_SECTIONS]
 * ───────────────────────────────────────────────────────────────────────────── */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_includes S.1 INCLUDES — Header Dependencies
 * @brief     Standard library includes for fixed-width types and struct offsets.
 *
 * PURPOSE: Establish header dependencies for boot information types.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers (stdint.h, stddef.h)
 *   - S.1b INTERNAL  — Project headers (CPI-SI dependencies)
 *   - S.1c EXTERNAL  — Third-party headers (with justification)
 *   - S.1d CPP       — C++ linkage wrapper
 *
 * Section order: Standard → Internal → External → CPP linkage
 * Flow: system foundation → our code → third-party → C++ compatibility
 *
 * Builds FROM: SETUP block (configuration context)
 * Builds TO:   BODY block (constants and types use these headers)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_bootinfo_standard S.1a STANDARD — System Headers
 * @ingroup   mos_bootinfo_includes
 * @brief     Freestanding headers for kernel-level code.
 *
 * stdint.h:   Fixed-width integer types (uint64_t, uint32_t, int8_t, etc.)
 * stddef.h:   offsetof() macro for compile-time struct offset verification
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdint.h>  /**< Fixed-width integers: uint64_t, uint32_t, int8_t, etc. */
#include <stddef.h>  /**< Standard definitions: size_t, NULL, offsetof */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_bootinfo_internal S.1b INTERNAL — Project Headers
 * @ingroup   mos_bootinfo_includes
 * @brief     Internal MOS headers this file depends on.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Foundation header — no internal dependencies] */

/** @} */ /* end S.1b INTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1c EXTERNAL — Third-Party Headers [EXTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_bootinfo_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup   mos_bootinfo_includes
 * @brief     External library headers with justification.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Freestanding kernel header — no external dependencies] */

/** @} */ /* end S.1c EXTERNAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1d CPP — C++ Linkage [CPP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_bootinfo_cpp S.1d CPP — C++ Linkage
 * @ingroup   mos_bootinfo_includes
 * @brief     extern "C" wrapper for C++ compatibility.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Kernel header — no C++ linkage required] */

/** @} */ /* end S.1d CPP */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_defines S.2 DEFINES — Preprocessor Constants
 * @brief     All boot information constants: addresses, sizes, magic, ternary values.
 *
 * PURPOSE: Define all preprocessor constants for boot handoff. These values
 *          form the foundation that B.2 TYPES builds upon.
 *
 * CONTAINS:
 *   - S.2a ADDRESS  — Boot info memory address (0x9000)
 *   - S.2b SIZES    — Structure and allocation sizes
 *   - S.2c MAGIC    — Identity constants (magic number, version, limits)
 *   - S.2d TERNARY  — Boot source (-1/0/+1), state, capability flags
 *
 * DERIVATION CHAIN:
 *   Genesis 1:1 → void/word/tov zones → ternary boot semantics → all constants
 *
 * DAR WITNESS ROLE:
 *   These constants define the observation points. The Witness tracks boot
 *   state transitions through the ternary values defined here.
 *
 * Builds FROM: S.1 INCLUDES (stdint.h types)
 * Builds TO:   B.2 TYPES (structures use these constants)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a ADDRESS — Boot Info Memory Address [ADDRESS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_address S.2a ADDRESS — Boot Info Memory Address
 * @ingroup   mos_bootinfo_defines
 * @brief     THE canonical address for boot info handoff between stages.
 *
 * PURPOSE: Single source of truth for boot info location. Both UEFI and BIOS
 *          bootloaders write to this address. Kernel entry reads from here.
 *
 * WHY 0x9000?
 *   - Below 1MB (accessible in all CPU modes: real, protected, long)
 *   - Above IVT (0x0000-0x03FF) and BDA (0x0400-0x04FF)
 *   - Below EBDA typical location (0x9FC00+)
 *   - Ternary alignment: 9 × 0x1000 = 9 × 4KB (9 = 3², a perfect square in base 3)
 *   - Safe region in conventional memory that survives mode transitions
 *
 * ASSEMBLY MIRROR: boot/include/bootinfo.inc defines matching constant.
 *
 * DAR WITNESS: Boot info address is the anchor point. All boot state
 *              observations reference this location.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_BOOT_INFO_ADDR
 * @brief Fixed physical address for boot information handoff.
 *
 * This is THE address where bootloaders place MosBootInfo and where
 * the kernel expects to find it. Like Genesis 1:1 anchors Scripture,
 * this address anchors the boot handoff.
 *
 * @par Value
 *      0x9000 (36864 decimal, 9 × 4KB pages from address 0)
 *
 * @par Ternary Significance
 *      9 = 3² = FACE in CPI-SI terms. The boot info sits at the
 *      "face" of conventional memory — the interface point.
 *
 * @par Usage
 *      - Bootloader: MosBootInfo* info = (MosBootInfo*)MOS_BOOT_INFO_ADDR;
 *      - Assembly: mov rdi, MOS_BOOT_INFO_ADDR
 *
 * @see bootinfo.inc for NASM equivalent
 */
#define MOS_BOOT_INFO_ADDR      0x9000

/** @} */ /* end S.2a ADDRESS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b SIZES — Structure and Allocation Sizes [SIZES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_sizes S.2b SIZES — Structure and Allocation Sizes
 * @ingroup   mos_bootinfo_defines
 * @brief     Memory allocation sizes for boot information structure.
 *
 * PURPOSE: Define how much memory to reserve for boot info. The actual
 *          structure is ~7.5KB but we allocate 8KB (2 pages) for alignment
 *          and future expansion.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_BOOT_INFO_SIZE
 * @brief Total bytes allocated for boot information.
 *
 * @par Value
 *      8192 bytes (8KB, 2 pages)
 *
 * @par Rationale
 *      - Actual MosBootInfo is ~7.5KB
 *      - Round up to page boundary for clean memory mapping
 *      - Leave room for future expansion without breaking ABI
 */
#define MOS_BOOT_INFO_SIZE      8192

/**
 * @def MOS_BOOT_INFO_PAGES
 * @brief Number of 4KB pages for boot information.
 *
 * @par Value
 *      2 pages
 *
 * @par Usage
 *      Page-based memory allocation in bootloader and kernel.
 */
#define MOS_BOOT_INFO_PAGES     2

/** @} */ /* end S.2b SIZES */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c MAGIC — Identity Constants [MAGIC]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_magic S.2c MAGIC — Identity Constants
 * @ingroup   mos_bootinfo_defines
 * @brief     Magic number, version, and limit constants for validation.
 *
 * PURPOSE: Provide identity verification and structural limits. The magic
 *          number confirms this is a valid MosBootInfo. Version tracks
 *          structure evolution. Limits bound array sizes.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_BOOT_MAGIC
 * @brief Magic number identifying valid MosBootInfo structure.
 *
 * @par Value
 *      0x4D4F5342 ("MOSB" as big-endian ASCII)
 *
 * @par Validation
 *      If info->magic != MOS_BOOT_MAGIC, the structure is invalid.
 *      This catches uninitialized memory and corruption.
 *
 * @par ASCII Breakdown
 *      - 'M' = 0x4D (MillenniumOS)
 *      - 'O' = 0x4F
 *      - 'S' = 0x53
 *      - 'B' = 0x42 (Boot)
 */
#define MOS_BOOT_MAGIC          0x4D4F5342

/**
 * @def MOS_BOOT_VERSION
 * @brief Structure version for compatibility checking.
 *
 * @par Value
 *      3 (current version)
 *
 * @par History
 *      - Version 1: Initial structure
 *      - Version 2: Added timestamps (boot_tsc, entry_tsc)
 *      - Version 3: Added SMBIOS pointer and boot timing breakdown
 *
 * @par Validation
 *      Kernel requires version >= 2 for timestamp fields.
 *      Check info->version before accessing newer fields.
 */
#define MOS_BOOT_VERSION        3

/**
 * @def MOS_MEMORY_MAP_MAX
 * @brief Maximum number of memory map entries.
 *
 * @par Value
 *      256 entries
 *
 * @par Rationale
 *      - E820 typically returns 20-50 entries
 *      - UEFI GetMemoryMap may return more with fragmentation
 *      - 256 provides generous headroom
 *      - 256 × 24 bytes = 6KB (fits in structure)
 */
#define MOS_MEMORY_MAP_MAX      256

/**
 * @def MOS_CMDLINE_MAX
 * @brief Maximum command line length in bytes.
 *
 * @par Value
 *      1024 bytes (1KB)
 *
 * @par Usage
 *      Kernel command line passed from bootloader.
 *      Null-terminated string.
 */
#define MOS_CMDLINE_MAX         1024

/** @} */ /* end S.2c MAGIC */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d TERNARY — Boot Ternary Values [TERNARY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_ternary S.2d TERNARY — Boot Ternary Values
 * @ingroup   mos_bootinfo_defines
 * @brief     Ternary semantics for boot source, state, and capabilities.
 *
 * PURPOSE: Define the ternary value system for boot information. All boot
 *          semantics follow the -1/0/+1 pattern rooted in void/word/tov.
 *
 * BIBLICAL FOUNDATION:
 *   The ternary system derives from Genesis 1's creation pattern:
 *   - VOID (-1): "formless and void" — incomplete, legacy, error
 *   - WORD (0):  "let there be" — transition, neutral, processing
 *   - TOV (+1):  "and it was good" — complete, modern, optimal
 *
 * BOOT SOURCE MAPPING:
 *   -1 = BIOS:    Old covenant — limited 32-bit, but valid path
 *    0 = Unknown: Unset — formless state before determination
 *   +1 = UEFI:    New covenant — full 64-bit capability
 *
 * DAR WITNESS ROLE:
 *   These ternary values ARE the state machine. The Witness tracks
 *   transitions between states and detects anomalies.
 *
 * CONTAINS:
 *   - S.2d-1 SOURCE — Boot source (-1=BIOS, 0=unknown, +1=UEFI)
 *   - S.2d-2 STATE  — Boot state (-1=error, 0=ok, +1=optimal)
 *   - S.2d-3 CAPS   — Capability flags (bitmask)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d-1 SOURCE — Boot Source [SOURCE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_source S.2d-1 SOURCE — Boot Source
 * @ingroup   mos_defines_ternary
 * @brief     How did the system boot? Ternary classification.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_BOOT_SOURCE_BIOS
 * @brief Legacy BIOS boot path (-1 ternary).
 *
 * @par Value
 *      -1 (negative trit)
 *
 * @par Meaning
 *      System booted via legacy BIOS/MBR path. 32-bit limited
 *      addressing. Framebuffer from VESA. Older but valid.
 *
 * @par Biblical Parallel
 *      Old covenant — limited but still valid path to the goal.
 */
#define MOS_BOOT_SOURCE_BIOS    (-1)

/**
 * @def MOS_BOOT_SOURCE_UNKNOWN
 * @brief Unknown/unset boot source (0 ternary).
 *
 * @par Value
 *      0 (neutral trit)
 *
 * @par Meaning
 *      Boot source not yet determined. Structure in formless state
 *      before bootloader sets the value.
 *
 * @par Biblical Parallel
 *      "Formless and void" — state before creation/determination.
 */
#define MOS_BOOT_SOURCE_UNKNOWN (0)

/**
 * @def MOS_BOOT_SOURCE_UEFI
 * @brief UEFI boot path (+1 ternary).
 *
 * @par Value
 *      +1 (positive trit)
 *
 * @par Meaning
 *      System booted via UEFI/GPT path. Full 64-bit addressing.
 *      Framebuffer from GOP. Modern, preferred path.
 *
 * @par Biblical Parallel
 *      New covenant — full capability, complete revelation.
 */
#define MOS_BOOT_SOURCE_UEFI    (1)

/** @} */ /* end S.2d-1 SOURCE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d-2 STATE — Boot State [STATE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_state S.2d-2 STATE — Boot State
 * @ingroup   mos_defines_ternary
 * @brief     Did boot succeed? Ternary classification.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_BOOT_STATE_ERROR
 * @brief Boot encountered errors (-1 ternary).
 *
 * @par Value
 *      -1 (negative trit)
 *
 * @par Meaning
 *      Boot completed but with errors. Some features may be unavailable.
 *      System is functional but degraded.
 *
 * @par DAR Response
 *      Detect anomaly → Assess severity → Restore to safe state.
 */
#define MOS_BOOT_STATE_ERROR    (-1)

/**
 * @def MOS_BOOT_STATE_OK
 * @brief Boot completed normally (0 ternary).
 *
 * @par Value
 *      0 (neutral trit)
 *
 * @par Meaning
 *      Boot completed successfully. All required features available.
 *      Standard operational state.
 */
#define MOS_BOOT_STATE_OK       (0)

/**
 * @def MOS_BOOT_STATE_OPTIMAL
 * @brief Boot completed with all features (+1 ternary).
 *
 * @par Value
 *      +1 (positive trit)
 *
 * @par Meaning
 *      Boot completed optimally. All optional features also available.
 *      Best possible state.
 *
 * @par Biblical Parallel
 *      "And it was very good" — completion with excellence.
 */
#define MOS_BOOT_STATE_OPTIMAL  (1)

/** @} */ /* end S.2d-2 STATE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d-3 CAPS — Capability Flags [CAPS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_caps S.2d-3 CAPS — Capability Flags
 * @ingroup   mos_defines_ternary
 * @brief     Feature availability flags (bitmask).
 *
 * PURPOSE: Indicate which optional features are available. Check these
 *          flags before accessing optional fields in MosBootInfo.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_CAP_FB_64BIT
 * @brief Framebuffer address is true 64-bit.
 *
 * @par Bit
 *      Bit 0 (0x01)
 *
 * @par Meaning
 *      UEFI GOP provides true 64-bit framebuffer address.
 *      If not set, address is 32-bit zero-extended (VESA).
 */
#define MOS_CAP_FB_64BIT        (1 << 0)

/**
 * @def MOS_CAP_ACPI
 * @brief ACPI tables available.
 *
 * @par Bit
 *      Bit 1 (0x02)
 *
 * @par Meaning
 *      RSDP was found and rsdp_address field is valid.
 */
#define MOS_CAP_ACPI            (1 << 1)

/**
 * @def MOS_CAP_UEFI_RT
 * @brief UEFI runtime services available.
 *
 * @par Bit
 *      Bit 2 (0x04)
 *
 * @par Meaning
 *      UEFI Runtime Services can be called after ExitBootServices.
 *      uefi_system_table field is valid.
 */
#define MOS_CAP_UEFI_RT         (1 << 2)

/**
 * @def MOS_CAP_SECURE_BOOT
 * @brief Secure Boot is active.
 *
 * @par Bit
 *      Bit 3 (0x08)
 *
 * @par Meaning
 *      System booted with UEFI Secure Boot enabled.
 */
#define MOS_CAP_SECURE_BOOT     (1 << 3)

/**
 * @def MOS_CAP_INITRD
 * @brief Initial ramdisk loaded.
 *
 * @par Bit
 *      Bit 4 (0x10)
 *
 * @par Meaning
 *      initrd_address and initrd_size fields are valid.
 */
#define MOS_CAP_INITRD          (1 << 4)

/**
 * @def MOS_CAP_SMBIOS
 * @brief SMBIOS tables available.
 *
 * @par Bit
 *      Bit 5 (0x20)
 *
 * @par Meaning
 *      SMBIOS entry point was found. smbios_address field is valid.
 */
#define MOS_CAP_SMBIOS          (1 << 5)

/**
 * @def MOS_CAP_BOOT_TIMING
 * @brief Detailed boot timing available.
 *
 * @par Bit
 *      Bit 6 (0x40)
 *
 * @par Meaning
 *      Boot timing breakdown fields (tsc_stage0a_start, etc.) are valid.
 *      Enables boot performance analysis.
 */
#define MOS_CAP_BOOT_TIMING     (1 << 6)

/** @} */ /* end S.2d-3 CAPS */

/** @} */ /* end S.2d TERNARY */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_setup_types S.3 TYPES — Type Declarations
 * @{
 *
 * [Reserved: Types defined in BODY B.3-B.6 with their related constants]
 *
 * @} end S.3 TYPES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOS — Function Prototypes [PROTOS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_protos S.4 PROTOS — Function Prototypes
 * @{
 *
 * [Reserved: Inline helpers defined in BODY B.7]
 *
 * @} end S.4 PROTOS
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 EXTERN — External Declarations [EXTERN]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_extern S.5 EXTERN — External Declarations
 * @{
 *
 * [Reserved: No mutable state in definition headers]
 *
 * @} end S.5 EXTERN
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Type definitions — the actual content.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// PURPOSE: All boot information types live here: enums, structs, and inline
/// helper functions for validation and access. Constants are in S.2 DEFINES.
///
/// CONTAINS:
///   - B.1 ORG CHART — Interface overview and type inventory
///   - B.2 TYPES     — Enums (MosMemoryType) and structs (Region, Framebuffer, BootInfo)
///   - B.3 HELPERS   — Validation, access, and zone classification functions
///   - B.4 ERRORS    — [Reserved: Error handling patterns]
///   - B.5 PUBLIC    — [Reserved: Public API expansion]
///   - B.6 OMISSION  — What is reserved and why
///
/// Section order: Org Chart → Types → Helpers → Errors → Public → Omission
/// Flow: map structure → definitions → validation → error handling → interface → guidance
///
/// Builds FROM: SETUP block (S.2 DEFINES provides constants)
/// Builds TO:   CLOSING block (summary and synthesis)
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @defgroup mos_bootinfo_body BODY — Boot Information Types
 * @ingroup mos_bootinfo
 * @{ */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Organizational Chart [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_bootinfo_org_chart B.1 ORG CHART — Organizational Chart
 * @ingroup mos_bootinfo_body
 * @brief   Interface overview and type inventory for bootinfo.h.
 *
 * PURPOSE: Map structure — navigate the BODY at a glance.
 *
 * NOTE: Constants (MOS_BOOT_*, MOS_CAP_*) are in SETUP S.2 DEFINES.
 *       BODY contains only type definitions and helper functions.
 *
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ BODY ORGANIZATION                                                           │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.2 TYPES          │ Enumeration and structure definitions                  │
 * │   B.2a ENUM        │ MosMemoryType (8 values)                               │
 * │   B.2b MEMREGION   │ MosMemoryRegion (24 bytes, E820 compatible)            │
 * │   B.2c FRAMEBUFFER │ MosFramebuffer (40 bytes, 64-bit clean)                │
 * │   B.2d BOOTINFO    │ MosBootInfo (~8KB unified structure)                   │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.3 HELPERS        │ Inline utility functions                               │
 * │   B.3a VALID       │ mos_boot_valid — magic/version check                   │
 * │   B.3b SOURCE      │ mos_boot_is_uefi, mos_boot_is_bios                     │
 * │   B.3c CAPS        │ mos_boot_has_capability                                │
 * │   B.3d ZONE        │ mos_address_zone — ternary classification              │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.4 ERRORS         │ [Reserved: Error handling patterns]                    │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.5 PUBLIC         │ [Reserved: Public API expansion]                       │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ B.6 OMISSION       │ What is reserved and why                               │
 * │   - Compile-time assertions in X.1 VALIDATION (CLOSING)                     │
 * │   - Error codes in future error.h                                           │
 * │   - Runtime validation in future validation.c                               │
 * └─────────────────────────────────────────────────────────────────────────────┘
 *
 * @} end B.1 ORG CHART
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 TYPES — Type Definitions [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_types B.2 TYPES — Type Definitions
 * @ingroup   mos_bootinfo_body
 * @brief     Enumeration and structure definitions for boot information.
 *
 * PURPOSE: Define all types for boot handoff: memory classification enum,
 *          memory region struct, framebuffer struct, and unified boot info struct.
 *
 * CONTAINS:
 *   - B.2a ENUM        — MosMemoryType (8 values, E820 compatible)
 *   - B.2b MEMREGION   — MosMemoryRegion (24 bytes packed)
 *   - B.2c FRAMEBUFFER — MosFramebuffer (40 bytes packed)
 *   - B.2d BOOTINFO    — MosBootInfo (~8KB unified structure)
 *
 * FLOW: Enum defines classification → Regions use enum → Framebuffer independent
 *       → BootInfo aggregates all
 *
 * Builds FROM: S.2 DEFINES (constants for array sizes, magic numbers)
 * Builds TO:   B.3 HELPERS (functions operate on these types)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a ENUM — Memory Type Enumeration [ENUM]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_types_enum B.2a ENUM — Memory Type Enumeration
 * @ingroup   mos_bootinfo_types
 * @brief     E820-compatible memory region classification.
 *
 * PURPOSE: Classify memory regions for kernel memory management. Values align
 *          with E820 standard for BIOS compatibility.
 *
 * Genesis Pattern:
 *   0 (USABLE):      "and it was good" — available for use
 *   1-4 (RESERVED):  "the deep" — protected/inaccessible regions
 *   5-7 (SPECIAL):   "the lights" — specific purpose allocations
 *
 * Builds FROM: B.2 TYPES (section context)
 * Builds TO:   B.2b MEMREGION (type field uses this enum)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @enum MosMemoryType
 * @brief Memory region type classification (E820 compatible).
 *
 * Classifies physical memory regions for kernel memory management. Values
 * align with the E820 standard for BIOS compatibility while extending to
 * support UEFI memory map translation.
 *
 * @par Genesis Pattern
 *      The 8 memory types follow creation's pattern:
 *      - 0 (USABLE): "and it was good" — available for use
 *      - 1-4 (RESERVED): "the deep" — protected/inaccessible regions
 *      - 5-7 (SPECIAL): "the lights" — specific purpose allocations
 *
 * @par Ternary Mapping
 *      - Negative (-1): Bad/unusable memory (MOS_MEM_BADRAM)
 *      - Neutral (0): Available/reclaimable (MOS_MEM_USABLE, MOS_MEM_ACPI)
 *      - Positive (+1): Active/allocated (MOS_MEM_KERNEL, MOS_MEM_BOOTLOADER)
 *
 * @par E820 Compatibility
 *      Values 0-4 match standard E820 types exactly. Values 5-7 are
 *      MillenniumOS extensions for finer-grained tracking.
 *
 * @see MosMemoryRegion Structure using this enum
 * @see MOS_BOOT_MAX_MMAP Maximum memory map entries
 */
typedef enum MosMemoryType {

    /**
     * @var MOS_MEM_USABLE
     * @brief Available for kernel use (E820 type 1).
     *
     * Memory that is free and available for the kernel to allocate.
     * This is the "good" memory — Genesis says "and it was good."
     *
     * @par Value
     *      0 (ternary neutral — available, unallocated)
     *
     * @par Source
     *      - BIOS: E820 type 1 (AddressRangeMemory)
     *      - UEFI: EfiConventionalMemory, EfiBootServicesCode/Data (after ExitBootServices)
     */
    MOS_MEM_USABLE      = 0,

    /**
     * @var MOS_MEM_RESERVED
     * @brief Reserved by firmware, do not use (E820 type 2).
     *
     * Memory reserved by the system firmware. May contain BIOS code,
     * memory-mapped I/O, or other hardware resources. Never allocate.
     *
     * @par Value
     *      1 (reserved — "the deep" that remains untouched)
     *
     * @par Source
     *      - BIOS: E820 type 2 (AddressRangeReserved)
     *      - UEFI: EfiReservedMemoryType, EfiMemoryMappedIO, EfiMemoryMappedIOPortSpace
     */
    MOS_MEM_RESERVED    = 1,

    /**
     * @var MOS_MEM_ACPI
     * @brief ACPI reclaimable memory (E820 type 3).
     *
     * Memory containing ACPI tables. Can be reclaimed after the kernel
     * has parsed and cached the ACPI information it needs.
     *
     * @par Value
     *      2 (reclaimable — temporary reservation)
     *
     * @par Source
     *      - BIOS: E820 type 3 (AddressRangeACPI)
     *      - UEFI: EfiACPIReclaimMemory
     */
    MOS_MEM_ACPI        = 2,

    /**
     * @var MOS_MEM_NVS
     * @brief ACPI Non-Volatile Storage (E820 type 4).
     *
     * Memory that must be preserved across sleep states (S1-S4).
     * Contains ACPI NVS data that firmware expects to persist.
     * Never reclaim or overwrite.
     *
     * @par Value
     *      3 (preserve — must survive sleep cycles)
     *
     * @par Source
     *      - BIOS: E820 type 4 (AddressRangeNVS)
     *      - UEFI: EfiACPIMemoryNVS
     */
    MOS_MEM_NVS         = 3,

    /**
     * @var MOS_MEM_BADRAM
     * @brief Bad memory, avoid completely (E820 type 5).
     *
     * Memory that has failed hardware tests or is known defective.
     * Never allocate — these addresses may cause corruption or crashes.
     *
     * @par Value
     *      4 (ternary negative mapping — broken, unusable)
     *
     * @par Source
     *      - BIOS: E820 type 5 (AddressRangeUnusable)
     *      - UEFI: EfiUnusableMemory
     */
    MOS_MEM_BADRAM      = 4,

    /**
     * @var MOS_MEM_BOOTLOADER
     * @brief Bootloader code and data (MillenniumOS extension).
     *
     * Memory occupied by the bootloader (stage0a, stage0b, stage1).
     * Can be reclaimed after kernel initialization is complete.
     *
     * @par Value
     *      5 (ternary positive — active but temporary)
     *
     * @par Source
     *      - Set by bootloader to mark its own memory footprint
     *      - Includes bootloader stack, heap, and code regions
     */
    MOS_MEM_BOOTLOADER  = 5,

    /**
     * @var MOS_MEM_KERNEL
     * @brief Kernel code and data (MillenniumOS extension).
     *
     * Memory occupied by the loaded kernel image. Must never be
     * reclaimed while the system is running.
     *
     * @par Value
     *      6 (ternary positive — active and permanent)
     *
     * @par Source
     *      - Set by bootloader when loading kernel
     *      - Includes .text, .data, .bss, and initial heap
     */
    MOS_MEM_KERNEL      = 6,

    /**
     * @var MOS_MEM_FRAMEBUFFER
     * @brief Framebuffer memory (MillenniumOS extension).
     *
     * Memory-mapped display framebuffer. Must be preserved for
     * graphics output. "Let there be light" — the visible interface.
     *
     * @par Value
     *      7 (ternary positive — active display memory)
     *
     * @par Source
     *      - BIOS: VESA linear framebuffer address
     *      - UEFI: GOP framebuffer base
     */
    MOS_MEM_FRAMEBUFFER = 7,

} MosMemoryType;

/** @} */ /* end B.2a ENUM */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2b MEMREGION — Memory Map Entry Structure [MEMREGION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_memregion B.2b MEMREGION — Memory Map Entry
 * @ingroup   mos_bootinfo_types
 * @brief     E820-compatible memory map entry structure (24 bytes packed).
 *
 * PURPOSE: Define the fundamental memory region descriptor used in the
 *          memory map array. Each entry describes a contiguous physical
 *          memory region with its type and attributes.
 *
 * @par Size Constraint
 *      Structure MUST remain exactly 24 bytes for:
 *      - E820 compatibility (BIOS returns this exact format)
 *      - Array offset calculations (entry[n] = base + n * 24)
 *      - Assembly access (hardcoded offsets in stage0)
 *
 * @par Genesis Pattern
 *      Each region is a "portion of the waters" — physical memory
 *      divided and classified during the boot creation process.
 *
 * Builds FROM: B.2a ENUM (MosMemoryType for classification)
 * Builds TO:   B.2d BOOTINFO (memory_map array uses this structure)
 *
 * @see MosMemoryType Classification enum
 * @see MOS_BOOT_MAX_MMAP Maximum entries in memory map
 * @see MOS_SIZEOF_MEMORY_REGION Compile-time size verification
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @struct MosMemoryRegion
 * @brief Memory map entry (24 bytes, E820 compatible).
 *
 * Describes a single contiguous region of physical memory. The structure
 * layout matches the E820 entry format exactly for BIOS compatibility,
 * while supporting full 64-bit addresses required by UEFI systems.
 *
 * @par Memory Layout (24 bytes total)
 *      @code
 *      Offset  Size  Field       Description
 *      ──────  ────  ──────────  ─────────────────────────────
 *      0x00    8     base        Physical start address
 *      0x08    8     length      Region size in bytes
 *      0x10    4     type        MosMemoryType classification
 *      0x14    4     attributes  ACPI 3.0 extended attributes
 *      ──────  ────  ──────────  ─────────────────────────────
 *      Total: 24 bytes (0x18)
 *      @endcode
 *
 * @par Packed Attribute
 *      The @c __attribute__((packed)) ensures no padding between fields,
 *      maintaining the exact 24-byte layout across all compilers.
 *
 * @par Ternary Alignment
 *      24 = 8 × 3 bytes. Three 64-bit words worth of data, reflecting
 *      the ternary structure: base (where), length (how much), type (what).
 *
 * @note Size verified at compile-time by MOS_SIZEOF_MEMORY_REGION assertion.
 */
typedef struct __attribute__((packed)) MosMemoryRegion {

    /**
     * @var MosMemoryRegion::base
     * @brief Physical base address of the memory region.
     *
     * The starting physical address of this memory region. For regions
     * below 4GB, the upper 32 bits will be zero. UEFI systems may report
     * regions above 4GB with full 64-bit addresses.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x00)
     *
     * @par Valid Range
     *      0x0000000000000000 to 0xFFFFFFFFFFFFFFFF
     *
     * @par Special Values
     *      - 0x00000000: Start of physical memory (IVT, BDA)
     *      - 0x00100000: 1MB mark (conventional memory boundary)
     *      - 0x00007C00: Boot sector load address
     *      - 0x00009000: MOS_BOOT_INFO_ADDR (our handoff point)
     *
     * @par Alignment
     *      Typically page-aligned (4KB) but not guaranteed.
     */
    uint64_t base;

    /**
     * @var MosMemoryRegion::length
     * @brief Length of the memory region in bytes.
     *
     * The size of this memory region. Combined with @c base, defines
     * the range [base, base + length). The end address is NOT included.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x08)
     *
     * @par Valid Range
     *      1 to 0xFFFFFFFFFFFFFFFF (zero-length regions are invalid)
     *
     * @par Common Values
     *      - 0x9FC00 (639KB): Low conventional memory
     *      - 0x10000 (64KB): Typical BIOS reserved block
     *      - Large values: Extended memory above 1MB
     *
     * @par Calculation
     *      End address = base + length (exclusive)
     *      Last valid byte = base + length - 1
     */
    uint64_t length;

    /**
     * @var MosMemoryRegion::type
     * @brief Memory region type classification.
     *
     * Classifies this region using MosMemoryType values. Determines
     * whether the kernel can use this memory and for what purpose.
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x10)
     *
     * @par Valid Values
     *      See MosMemoryType enum (0-7)
     *
     * @par E820 Compatibility
     *      Values 0-4 match E820 exactly. Values 5-7 are MOS extensions.
     *      When translating from UEFI, map to nearest equivalent.
     *
     * @par Ternary Interpretation
     *      - 0 (USABLE): Neutral — available
     *      - 1-4: Various reserved states
     *      - 5-7: Active allocations (positive)
     *      - 4 (BADRAM): Negative — broken/unusable
     *
     * @see MosMemoryType Full classification definitions
     */
    uint32_t type;

    /**
     * @var MosMemoryRegion::attributes
     * @brief Extended attributes (ACPI 3.0 specification).
     *
     * Additional memory attributes as defined by ACPI 3.0. Provides
     * hints about memory characteristics beyond basic type.
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x14)
     *
     * @par Bit Fields (ACPI 3.0)
     *      - Bit 0: AddressRangeEnabled (1 = valid entry)
     *      - Bit 1: AddressRangeNonVolatile (1 = NVRAM)
     *      - Bit 2: AddressRangeSlowAccess (1 = slow memory)
     *      - Bit 3: AddressRangeErrorLog (1 = error logging)
     *      - Bits 4-31: Reserved (should be 0)
     *
     * @par Default Value
     *      0x00000001 (enabled, no special attributes)
     *
     * @par BIOS Note
     *      Older BIOS may not set this field. Treat 0 as "no info."
     */
    uint32_t attributes;

} MosMemoryRegion;

/** @} */ /* end B.2b MEMREGION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2c FRAMEBUFFER — Display Information Structure [FRAMEBUFFER]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_framebuffer B.2c FRAMEBUFFER — Display Information
 * @ingroup   mos_bootinfo_types
 * @brief     64-bit clean framebuffer structure for both GOP and VESA (40 bytes).
 *
 * PURPOSE: Provide unified display information that works regardless of boot
 *          path. UEFI GOP and BIOS VESA both fill this same structure, allowing
 *          the kernel to render graphics without knowing the boot source.
 *
 * @par Genesis Pattern
 *      "Let there be light" (Genesis 1:3) — the framebuffer is how the system
 *      becomes visible. Before graphics initialization, the OS is formless;
 *      after, it can show itself to the user.
 *
 * @par Size Constraint
 *      Structure MUST remain exactly 40 bytes for:
 *      - MosBootInfo offset calculations (embedded at offset 0x0040)
 *      - Assembly access (hardcoded offsets in kernel entry)
 *      - 8-byte alignment for 64-bit address field
 *
 * @par Ternary Alignment
 *      40 = 8 × 5 bytes. Five 64-bit words of display context.
 *
 * Builds FROM: B.2b MEMREGION (memory region concepts for framebuffer mapping)
 * Builds TO:   B.2d BOOTINFO (framebuffer embedded in MosBootInfo)
 *
 * @see MosBootInfo Main boot handoff structure
 * @see MOS_SIZEOF_FRAMEBUFFER Compile-time size verification
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @struct MosFramebuffer
 * @brief Framebuffer information (40 bytes, 64-bit clean).
 *
 * Describes the linear framebuffer for direct pixel access. Supports both
 * UEFI GOP (native 64-bit addresses) and BIOS VESA (32-bit addresses
 * zero-extended to 64-bit).
 *
 * @par Memory Layout (40 bytes total)
 *      @code
 *      Offset  Size  Field        Description
 *      ──────  ────  ───────────  ─────────────────────────────
 *      0x00    8     address      Physical framebuffer base
 *      0x08    4     width        Horizontal resolution (pixels)
 *      0x0C    4     height       Vertical resolution (pixels)
 *      0x10    4     pitch        Bytes per scanline
 *      0x14    4     bpp          Bits per pixel
 *      0x18    1     red_size     Red channel bit width
 *      0x19    1     red_shift    Red channel bit position
 *      0x1A    1     green_size   Green channel bit width
 *      0x1B    1     green_shift  Green channel bit position
 *      0x1C    1     blue_size    Blue channel bit width
 *      0x1D    1     blue_shift   Blue channel bit position
 *      0x1E    10    reserved     Padding to 40 bytes
 *      ──────  ────  ───────────  ─────────────────────────────
 *      Total: 40 bytes (0x28)
 *      @endcode
 *
 * @par Pixel Calculation
 *      To write pixel at (x, y):
 *      @code
 *      uint32_t *pixel = (uint32_t*)(address + y * pitch + x * (bpp / 8));
 *      *pixel = (red << red_shift) | (green << green_shift) | (blue << blue_shift);
 *      @endcode
 *
 * @note Size verified at compile-time by MOS_SIZEOF_FRAMEBUFFER assertion.
 */
typedef struct __attribute__((packed)) MosFramebuffer {

    /**
     * @var MosFramebuffer::address
     * @brief Physical address of the linear framebuffer.
     *
     * The starting address of the framebuffer in physical memory. This is
     * a true 64-bit address for UEFI GOP compatibility. For BIOS VESA,
     * the 32-bit address is zero-extended.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x00)
     *
     * @par Source
     *      - UEFI: GOP->Mode->FrameBufferBase
     *      - BIOS: VESA mode info linear framebuffer address (zero-extended)
     *
     * @par Mapping
     *      The kernel must identity-map or otherwise map this physical
     *      address before writing pixels. The region should be mapped
     *      as write-combining for performance.
     *
     * @par Special Value
     *      0 = No framebuffer available (text mode or error)
     */
    uint64_t address;

    /**
     * @var MosFramebuffer::width
     * @brief Horizontal resolution in pixels.
     *
     * The number of pixels per horizontal scanline. Combined with
     * @c height, defines the visible display area.
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x08)
     *
     * @par Common Values
     *      - 1920: Full HD (1920×1080)
     *      - 1280: HD (1280×720)
     *      - 800: SVGA (800×600)
     *      - 640: VGA (640×480)
     *
     * @par Calculation
     *      Framebuffer size ≥ width × height × (bpp / 8)
     */
    uint32_t width;

    /**
     * @var MosFramebuffer::height
     * @brief Vertical resolution in pixels.
     *
     * The number of horizontal scanlines. Combined with @c width,
     * defines the visible display area.
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x0C)
     *
     * @par Common Values
     *      - 1080: Full HD (1920×1080)
     *      - 720: HD (1280×720)
     *      - 600: SVGA (800×600)
     *      - 480: VGA (640×480)
     */
    uint32_t height;

    /**
     * @var MosFramebuffer::pitch
     * @brief Bytes per horizontal scanline (stride).
     *
     * The number of bytes between the start of one scanline and the
     * start of the next. May be larger than width × (bpp / 8) due to
     * alignment padding.
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x10)
     *
     * @par Important
     *      ALWAYS use pitch for scanline calculations, never assume
     *      pitch = width × (bpp / 8). Hardware may add padding for
     *      alignment or DMA efficiency.
     *
     * @par Calculation
     *      Row start = address + (y × pitch)
     *      Pixel address = row start + (x × (bpp / 8))
     */
    uint32_t pitch;

    /**
     * @var MosFramebuffer::bpp
     * @brief Bits per pixel (color depth).
     *
     * The number of bits used to represent each pixel. Determines
     * color depth and memory usage per pixel.
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x14)
     *
     * @par Common Values
     *      - 32: True color + alpha (BGRA/RGBA)
     *      - 24: True color (BGR/RGB)
     *      - 16: High color (5-6-5 or 5-5-5)
     *      - 8: 256 colors (paletted, rare for GOP)
     *
     * @par Bytes Per Pixel
     *      bytes_per_pixel = bpp / 8
     */
    uint32_t bpp;

    /**
     * @var MosFramebuffer::red_size
     * @brief Number of bits in the red color channel.
     *
     * @par Type
     *      uint8_t (1 byte, offset 0x18)
     *
     * @par Common Values
     *      - 8: Standard 32-bit color (8 bits per channel)
     *      - 5: 16-bit color (5-6-5 or 5-5-5 format)
     */
    uint8_t  red_size;

    /**
     * @var MosFramebuffer::red_shift
     * @brief Bit position of the red channel within a pixel.
     *
     * @par Type
     *      uint8_t (1 byte, offset 0x19)
     *
     * @par Common Values
     *      - 16: BGRA format (red at bits 16-23)
     *      - 0: RGBA format (red at bits 0-7)
     *      - 11: 16-bit 5-6-5 format
     */
    uint8_t  red_shift;

    /**
     * @var MosFramebuffer::green_size
     * @brief Number of bits in the green color channel.
     *
     * @par Type
     *      uint8_t (1 byte, offset 0x1A)
     *
     * @par Common Values
     *      - 8: Standard 32-bit color
     *      - 6: 16-bit 5-6-5 format (green gets extra bit)
     *      - 5: 16-bit 5-5-5 format
     */
    uint8_t  green_size;

    /**
     * @var MosFramebuffer::green_shift
     * @brief Bit position of the green channel within a pixel.
     *
     * @par Type
     *      uint8_t (1 byte, offset 0x1B)
     *
     * @par Common Values
     *      - 8: BGRA/RGBA format (green at bits 8-15)
     *      - 5: 16-bit 5-6-5 format
     */
    uint8_t  green_shift;

    /**
     * @var MosFramebuffer::blue_size
     * @brief Number of bits in the blue color channel.
     *
     * @par Type
     *      uint8_t (1 byte, offset 0x1C)
     *
     * @par Common Values
     *      - 8: Standard 32-bit color
     *      - 5: 16-bit color
     */
    uint8_t  blue_size;

    /**
     * @var MosFramebuffer::blue_shift
     * @brief Bit position of the blue channel within a pixel.
     *
     * @par Type
     *      uint8_t (1 byte, offset 0x1D)
     *
     * @par Common Values
     *      - 0: BGRA format (blue at bits 0-7)
     *      - 16: RGBA format (blue at bits 16-23)
     */
    uint8_t  blue_shift;

    /**
     * @var MosFramebuffer::reserved
     * @brief Reserved padding to reach 40-byte structure size.
     *
     * Maintains 40-byte structure size for alignment and future
     * expansion. Should be zeroed by bootloader.
     *
     * @par Type
     *      uint8_t[10] (10 bytes, offset 0x1E)
     *
     * @par Future Use
     *      May hold alpha channel info, pixel format flags, or
     *      additional display capabilities.
     */
    uint8_t  reserved[10];

} MosFramebuffer;

/** @} */ /* end B.2c FRAMEBUFFER */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2d BOOTEVENT — Boot Event Log Entry [BOOTEVENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_event B.2d BOOTEVENT — Boot Event Log Entry
 * @ingroup   mos_bootinfo_types
 * @brief     Structure for recording boot events (16 bytes each).
 *
 * PURPOSE: Persistent boot diagnostics that survive in memory. The bootloader
 * records key events (phases, steps, errors) to this buffer. The kernel reads
 * and displays them for boot forensics.
 *
 * DAR WITNESS ROLE:
 *   - DETECT:  Events capture state at each checkpoint
 *   - ASSESS:  Status codes indicate success/failure
 *   - RESTORE: Kernel can diagnose boot issues from event log
 *
 * @{ */

/** Maximum number of boot events that can be recorded */
#define MOS_MAX_BOOT_EVENTS  6

/** Boot event phase codes (high byte of event_code) */
#define MOS_EVT_PHASE_INIT     0x0000  /**< Library initialization */
#define MOS_EVT_PHASE_BOOTINFO 0x0100  /**< Boot info allocation */
#define MOS_EVT_PHASE_GOP      0x0200  /**< Graphics initialization */
#define MOS_EVT_PHASE_KERNEL   0x0300  /**< Kernel loading */
#define MOS_EVT_PHASE_ACPI     0x0400  /**< ACPI discovery */
#define MOS_EVT_PHASE_MMAP     0x0500  /**< Memory map acquisition */
#define MOS_EVT_PHASE_EXIT     0x0600  /**< ExitBootServices */
#define MOS_EVT_PHASE_TRANSFER 0x0700  /**< Kernel transfer */

/** Boot event step codes (low byte of event_code) */
#define MOS_EVT_STEP_START     0x00    /**< Phase starting */
#define MOS_EVT_STEP_OK        0x01    /**< Phase completed OK */
#define MOS_EVT_STEP_WARN      0x02    /**< Warning (non-fatal) */
#define MOS_EVT_STEP_FAIL      0x0F    /**< Phase failed */

/** Boot event status: ternary result */
#define MOS_EVT_STATUS_OK      0x0000  /**< Success */
#define MOS_EVT_STATUS_WARN    0x0001  /**< Warning (continued) */
#define MOS_EVT_STATUS_FAIL    0xFFFF  /**< Failure */

/**
 * @struct MosBootEvent
 * @brief Single boot event log entry (16 bytes).
 *
 * Each event captures a checkpoint in the boot process with timing,
 * phase/step identification, result status, and optional data value.
 *
 * @par Memory Layout (16 bytes, packed)
 * @code
 *   Offset  Size  Field       Description
 *   ─────────────────────────────────────────
 *   0x00    8     tsc         TSC timestamp
 *   0x08    2     event_code  Phase + Step code
 *   0x0A    2     status      Result code
 *   0x0C    4     value       Optional data (address, size, etc.)
 * @endcode
 */
typedef struct __attribute__((packed)) MosBootEvent {
    uint64_t tsc;         /**< TSC timestamp when event occurred */
    uint16_t event_code;  /**< Phase (high byte) + Step (low byte) */
    uint16_t status;      /**< Result: 0=OK, 1=WARN, 0xFFFF=FAIL */
    uint32_t value;       /**< Optional: address, size, error code */
} MosBootEvent;

/** @} */ /* end B.2d BOOTEVENT */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2e BOOTINFO — Unified Boot Information Structure [BOOTINFO]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_struct B.2d BOOTINFO — Unified Boot Information
 * @ingroup   mos_bootinfo_types
 * @brief     Main boot handoff structure from bootloader to kernel (~8KB).
 *
 * PURPOSE: Single unified structure filled by EITHER UEFI or BIOS bootloader,
 *          allowing one kernel binary to work with both boot paths. This is
 *          THE foundation from which the kernel receives its initial state.
 *
 * @par Genesis 1:1 Anchor
 *      "In the beginning God created the heaven and the earth."
 *      This structure IS the beginning for the kernel — the absolute first
 *      data it receives. Like Genesis 1:1 anchors Scripture, MosBootInfo
 *      anchors kernel initialization.
 *
 * @par Memory Layout (Fixed offsets for assembly compatibility)
 *      @code
 *      Offset    Size    Section         Description
 *      ────────  ──────  ──────────────  ─────────────────────────────
 *      0x0000    64      HEADER          Identity, state, capabilities
 *      0x0040    48      FRAMEBUFFER     Display info (40 + 8 reserved)
 *      0x0070    32      MEMORY ZONES    Ternary spatial classification
 *      0x0090    112     RESERVED        Future expansion
 *      0x0100    6144    MEMORY MAP      256 × 24-byte E820 entries
 *      0x1900    varies  EXTENDED        ACPI, SMBIOS, timing, cmdline
 *      ────────  ──────  ──────────────  ─────────────────────────────
 *      Total: ~8KB (MOS_BOOT_INFO_SIZE)
 *      @endcode
 *
 * @par Genesis Pattern (Structure Sections)
 *      - Header:       "In the beginning" — identity and state
 *      - Framebuffer:  "Let there be light" — visibility
 *      - Memory zones: "the heaven and the earth" — spatial division
 *      - Memory map:   "the waters" — physical foundation
 *      - Extended:     "and God saw that it was good" — verification data
 *
 * @par Ternary Design
 *      The structure embeds ternary semantics throughout:
 *      - boot_source: -1 (BIOS/old), 0 (unknown), +1 (UEFI/new)
 *      - boot_state: -1 (error), 0 (ok), +1 (optimal)
 *      - Memory zones: void (-1), word (0), tov (+1)
 *
 * Builds FROM: B.2a-B.2c (enum, memregion, framebuffer types)
 * Builds TO:   B.3 HELPERS (validation and access functions)
 *
 * @see MOS_BOOT_INFO_ADDR Fixed physical address (0x9000)
 * @see MOS_BOOT_INFO_SIZE Allocation size
 * @see MOS_BOOT_MAGIC Validation signature ("MOSB")
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @struct MosBootInfo
 * @brief Unified boot information structure (~8KB).
 *
 * This structure is THE handoff between bootloader and kernel. Like
 * Genesis 1:1, it is the beginning from which the kernel receives its
 * initial state. Both UEFI and BIOS bootloaders fill this same structure,
 * allowing one kernel binary to work with either boot path.
 *
 * @par Packed Attribute
 *      The @c __attribute__((packed)) ensures consistent layout for:
 *      - Assembly access (hardcoded offsets in entry.asm)
 *      - Binary compatibility across compilers
 *      - Predictable memory forensics
 *
 * @par Validation
 *      All offsets are verified at compile-time in CLOSING X.1 VALIDATION.
 *      Runtime validation checks magic number and version.
 *
 * @note Size verified by MOS_BOOT_INFO_SIZE static assertion.
 */
typedef struct __attribute__((packed)) MosBootInfo {

    /* ═══════════════════════════════════════════════════════════════════════
     * SECTION 1: HEADER (Offset 0x0000, 64 bytes)
     * ═══════════════════════════════════════════════════════════════════════
     * "In the beginning" — identity, version, state, capabilities.
     * Written by: Both UEFI and BIOS bootloaders
     */

    /**
     * @var MosBootInfo::magic
     * @brief Magic number for structure validation.
     *
     * Must be MOS_BOOT_MAGIC (0x42534F4D, "MOSB" in ASCII). The kernel
     * checks this value first to verify valid boot info exists.
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x00)
     *
     * @par Validation
     *      if (boot_info->magic != MOS_BOOT_MAGIC) { ... invalid ... }
     *
     * @par Genesis Pattern
     *      "In the beginning" — the first word identifies the beginning.
     */
    uint32_t magic;

    /**
     * @var MosBootInfo::version
     * @brief Structure version for compatibility checking.
     *
     * Allows the kernel to handle different boot info versions.
     * Increment when adding fields or changing layout.
     *
     * @par Type
     *      uint8_t (1 byte, offset 0x04)
     *
     * @par Current Value
     *      MOS_BOOT_VERSION (currently 3)
     *
     * @par History
     *      - Version 1: Initial structure
     *      - Version 2: Added framebuffer color fields
     *      - Version 3: Added SMBIOS pointer and boot timing
     */
    uint8_t  version;

    /**
     * @var MosBootInfo::boot_source
     * @brief Boot path indicator (ternary: BIOS/unknown/UEFI).
     *
     * Indicates which bootloader filled this structure. Enables
     * boot-path-specific kernel behavior when needed.
     *
     * @par Type
     *      int8_t (1 byte, offset 0x05) — SIGNED for ternary semantics
     *
     * @par Values (Ternary)
     *      - -1 (MOS_BOOT_SOURCE_BIOS): Legacy BIOS boot path
     *      -  0 (MOS_BOOT_SOURCE_UNKNOWN): Source unknown
     *      - +1 (MOS_BOOT_SOURCE_UEFI): UEFI boot path
     *
     * @par Ternary Meaning
     *      - -1: Old way (BIOS, legacy, void)
     *      -  0: Neutral (unknown, transitional)
     *      - +1: New way (UEFI, modern, tov)
     */
    int8_t   boot_source;

    /**
     * @var MosBootInfo::boot_state
     * @brief Boot completion state (ternary: error/ok/optimal).
     *
     * Indicates how successfully the boot process completed.
     * The kernel may take different actions based on this state.
     *
     * @par Type
     *      int8_t (1 byte, offset 0x06) — SIGNED for ternary semantics
     *
     * @par Values (Ternary)
     *      - -1 (MOS_BOOT_STATE_ERROR): Boot had errors (degraded)
     *      -  0 (MOS_BOOT_STATE_OK): Boot completed normally
     *      - +1 (MOS_BOOT_STATE_OPTIMAL): Boot was optimal (all features)
     *
     * @par Error Examples
     *      - -1: Memory map incomplete, some ACPI failed
     *      -  0: Everything needed is present
     *      - +1: All optional features also initialized
     */
    int8_t   boot_state;

    /**
     * @var MosBootInfo::reserved1
     * @brief Alignment padding byte.
     *
     * Ensures next uint32_t field (capabilities) is 4-byte aligned.
     * Should be zeroed by bootloader.
     *
     * @par Type
     *      uint8_t (1 byte, offset 0x07)
     */
    uint8_t  reserved1;

    /**
     * @var MosBootInfo::capabilities
     * @brief Capability flags indicating available features.
     *
     * Bitmap of capabilities the bootloader was able to provide.
     * The kernel checks these before using optional features.
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x08)
     *
     * @par Flags (MOS_CAP_*)
     *      - Bit 0 (MOS_CAP_FRAMEBUFFER): Framebuffer info valid
     *      - Bit 1 (MOS_CAP_MEMMAP): Memory map valid
     *      - Bit 2 (MOS_CAP_ACPI): RSDP address valid
     *      - Bit 3 (MOS_CAP_UEFI_RT): UEFI runtime services available
     *      - Bit 4 (MOS_CAP_INITRD): Initial ramdisk present
     *      - Bit 5 (MOS_CAP_SMBIOS): SMBIOS pointer valid
     *      - Bit 6 (MOS_CAP_BOOT_TIMING): Boot timing fields valid
     *
     * @par Usage
     *      if (boot_info->capabilities & MOS_CAP_ACPI) { ... use RSDP ... }
     */
    uint32_t capabilities;

    /**
     * @var MosBootInfo::memory_map_count
     * @brief Number of valid entries in memory_map array.
     *
     * Indicates how many MosMemoryRegion entries contain valid data.
     * Must be <= MOS_MEMORY_MAP_MAX (256).
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x0C)
     *
     * @par Valid Range
     *      1 to MOS_MEMORY_MAP_MAX (256)
     *
     * @par Iteration
     *      for (uint32_t i = 0; i < boot_info->memory_map_count; i++) {
     *          MosMemoryRegion *r = &boot_info->memory_map[i];
     *      }
     */
    uint32_t memory_map_count;

    /**
     * @var MosBootInfo::total_memory
     * @brief Total usable physical memory in bytes.
     *
     * Sum of all MOS_MEM_USABLE regions from the memory map.
     * Pre-calculated by bootloader for kernel convenience.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x10)
     *
     * @par Calculation
     *      total_memory = Σ(region.length) where region.type == MOS_MEM_USABLE
     */
    uint64_t total_memory;

    /**
     * @var MosBootInfo::kernel_physical
     * @brief Physical address where kernel was loaded.
     *
     * The bootloader loads the kernel binary to this address.
     * The kernel needs this for identity mapping setup.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x18)
     *
     * @par Typical Value
     *      0x100000 (1MB) — traditional kernel load address
     */
    uint64_t kernel_physical;

    /**
     * @var MosBootInfo::kernel_size
     * @brief Size of the loaded kernel in bytes.
     *
     * The size of the kernel binary that was loaded to memory.
     * Used for memory mapping and protection.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x20)
     */
    uint64_t kernel_size;

    /**
     * @var MosBootInfo::boot_tsc
     * @brief TSC value at boot start (stage0a entry).
     *
     * Timestamp counter value captured as early as possible in the
     * boot process. Used for boot timing analysis.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x28)
     *
     * @par Source
     *      RDTSC instruction in stage0a before any other work
     *
     * @par Usage
     *      Boot duration (cycles) = entry_tsc - boot_tsc
     */
    uint64_t boot_tsc;

    /**
     * @var MosBootInfo::entry_tsc
     * @brief TSC value at kernel entry point.
     *
     * Timestamp counter value captured when control transfers to
     * the kernel. Paired with boot_tsc for total boot time.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x30)
     *
     * @par Source
     *      RDTSC instruction in entry.asm before calling kernel_main
     */
    uint64_t entry_tsc;

    /**
     * @var MosBootInfo::reserved2
     * @brief Reserved padding to reach offset 0x40.
     *
     * Maintains header section at exactly 64 bytes.
     * Should be zeroed by bootloader.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x38)
     */
    uint64_t reserved2;

    /* ═══════════════════════════════════════════════════════════════════════
     * SECTION 2: FRAMEBUFFER (Offset 0x0040, 48 bytes)
     * ═══════════════════════════════════════════════════════════════════════
     * "Let there be light" — the display through which the system is seen.
     * Written by: Both bootloaders (UEFI uses GOP, BIOS uses VESA)
     */

    /**
     * @var MosBootInfo::framebuffer
     * @brief Embedded framebuffer information structure.
     *
     * Contains all information needed to render graphics. Valid only
     * if MOS_CAP_FRAMEBUFFER is set in capabilities.
     *
     * @par Type
     *      MosFramebuffer (40 bytes, offset 0x40)
     *
     * @par Genesis Pattern
     *      "Let there be light" (Genesis 1:3) — the framebuffer makes
     *      the system visible to the user.
     *
     * @see MosFramebuffer Full structure documentation
     */
    MosFramebuffer framebuffer;

    /**
     * @var MosBootInfo::reserved3
     * @brief Reserved padding after framebuffer.
     *
     * Pads framebuffer section to 48 bytes (40 + 8).
     * Maintains alignment for next section.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x68)
     */
    uint64_t reserved3;

    /* ═══════════════════════════════════════════════════════════════════════
     * SECTION 3: MEMORY ZONES — Ternary (Offset 0x0070, 32 bytes)
     * ═══════════════════════════════════════════════════════════════════════
     * "The heaven and the earth" — spatial division of physical memory.
     * Ternary zone classification following void/word/tov pattern.
     */

    /**
     * @var MosBootInfo::zone_void_end
     * @brief End address of the void zone (ternary -1).
     *
     * Memory from 0 to zone_void_end is the "void" zone — legacy
     * BIOS area, IVT, BDA, and other low memory that should be
     * preserved or avoided.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x70)
     *
     * @par Typical Value
     *      0x100000 (1MB) — end of conventional memory
     *
     * @par Ternary Meaning
     *      -1 (void): Formless, legacy, potentially dangerous
     */
    uint64_t zone_void_end;

    /**
     * @var MosBootInfo::zone_word_end
     * @brief End address of the word zone (ternary 0).
     *
     * Memory from zone_void_end to zone_word_end is the "word" zone —
     * kernel space where the OS operates. Processing, structure, order.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0x78)
     *
     * @par Typical Value
     *      End of kernel-reserved memory
     *
     * @par Ternary Meaning
     *      0 (word): Structured, processing, kernel domain
     */
    uint64_t zone_word_end;

    /**
     * @var MosBootInfo::reserved4
     * @brief Reserved padding for memory zones section.
     *
     * Pads memory zones section to 32 bytes.
     * Future: zone_tov_end or additional zone metadata.
     *
     * @par Type
     *      uint64_t[2] (16 bytes, offset 0x80)
     *
     * @par Note
     *      Memory above zone_word_end is implicitly the "tov" zone
     *      (+1, completion, user space).
     */
    uint64_t reserved4[2];

    /* ═══════════════════════════════════════════════════════════════════════
     * SECTION 4: BOOT EVENTS (Offset 0x0090, 112 bytes)
     * ═══════════════════════════════════════════════════════════════════════
     * DAR Witness boot event log. Records key checkpoints for diagnostics.
     * Survives in memory for kernel to read and display.
     */

    /**
     * @var MosBootInfo::boot_event_count
     * @brief Number of boot events recorded.
     *
     * Valid range: 0 to MOS_MAX_BOOT_EVENTS (6).
     *
     * @par Type
     *      uint32_t (4 bytes, offset 0x90)
     */
    uint32_t boot_event_count;

    /**
     * @var MosBootInfo::boot_last_phase
     * @brief Last phase that was started.
     *
     * Used to track boot progress even if event buffer overflows.
     *
     * @par Type
     *      uint16_t (2 bytes, offset 0x94)
     */
    uint16_t boot_last_phase;

    /**
     * @var MosBootInfo::boot_last_status
     * @brief Status of the last event recorded.
     *
     * Quick check for boot success: 0=OK, 0xFFFF=FAIL.
     *
     * @par Type
     *      uint16_t (2 bytes, offset 0x96)
     */
    uint16_t boot_last_status;

    /**
     * @var MosBootInfo::boot_events
     * @brief Array of boot event log entries.
     *
     * Circular buffer of boot events. Newest events overwrite oldest
     * when buffer is full.
     *
     * @par Type
     *      MosBootEvent[6] (96 bytes, offset 0x98)
     *
     * @par DAR Witness
     *      These events ARE the witness log. The kernel reads them to
     *      understand what happened during boot.
     */
    MosBootEvent boot_events[MOS_MAX_BOOT_EVENTS];

    /**
     * @var MosBootInfo::reserved5
     * @brief Reserved padding for boot events section.
     *
     * Pads boot events section to 112 bytes (0x70) to keep memory_map
     * at offset 0x100.
     *
     * @par Type
     *      uint64_t (8 bytes, offset 0xF8)
     */
    uint64_t reserved5;

    /* ═══════════════════════════════════════════════════════════════════════
     * SECTION 5: MEMORY MAP (Offset 0x0100, 6144 bytes)
     * ═══════════════════════════════════════════════════════════════════════
     * "The waters" — the physical memory foundation.
     * E820-compatible memory map entries.
     */

    /**
     * @var MosBootInfo::memory_map
     * @brief Array of memory region descriptors.
     *
     * E820-compatible memory map describing all physical memory.
     * Valid entries count is in memory_map_count.
     *
     * @par Type
     *      MosMemoryRegion[256] (6144 bytes, offset 0x100)
     *
     * @par Genesis Pattern
     *      "The waters" (Genesis 1:2) — the raw physical foundation
     *      from which usable memory emerges.
     *
     * @par Iteration
     *      @code
     *      for (uint32_t i = 0; i < boot_info->memory_map_count; i++) {
     *          MosMemoryRegion *r = &boot_info->memory_map[i];
     *          if (r->type == MOS_MEM_USABLE) {
     *              // Found usable memory region
     *          }
     *      }
     *      @endcode
     *
     * @see MosMemoryRegion Individual entry structure
     * @see memory_map_count Number of valid entries
     */
    MosMemoryRegion memory_map[MOS_MEMORY_MAP_MAX];

    /* ═══════════════════════════════════════════════════════════════════════
     * SECTION 6: EXTENDED INFO (Offset 0x1900+)
     * ═══════════════════════════════════════════════════════════════════════
     * "And God saw that it was good" — verification and enhancement data.
     * Additional information that may not be present in all boot paths.
     * Check capability flags before using these fields.
     */

    /**
     * @var MosBootInfo::initrd_address
     * @brief Initial ramdisk physical address.
     *
     * Physical address where the initial ramdisk was loaded.
     * Valid only if MOS_CAP_INITRD is set.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Special Value
     *      0 = No initial ramdisk present
     */
    uint64_t initrd_address;

    /**
     * @var MosBootInfo::initrd_size
     * @brief Initial ramdisk size in bytes.
     *
     * Size of the initial ramdisk. Valid only if MOS_CAP_INITRD is set.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Special Value
     *      0 = No initial ramdisk present
     */
    uint64_t initrd_size;

    /**
     * @var MosBootInfo::rsdp_address
     * @brief ACPI Root System Description Pointer address.
     *
     * Physical address of the ACPI RSDP structure. The kernel uses
     * this to locate ACPI tables for hardware discovery.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Valid When
     *      MOS_CAP_ACPI flag is set in capabilities
     *
     * @par Special Value
     *      0 = ACPI not available
     */
    uint64_t rsdp_address;

    /**
     * @var MosBootInfo::uefi_system_table
     * @brief UEFI System Table physical address.
     *
     * Physical address of the UEFI System Table. Enables access to
     * UEFI runtime services after ExitBootServices.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Valid When
     *      MOS_CAP_UEFI_RT flag is set (UEFI boot with runtime services)
     *
     * @par Special Value
     *      0 = Not a UEFI boot or runtime services unavailable
     */
    uint64_t uefi_system_table;

    /**
     * @var MosBootInfo::smbios_address
     * @brief SMBIOS entry point physical address.
     *
     * Physical address of the SMBIOS entry point structure. Enables
     * access to system management BIOS tables for hardware info.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Valid When
     *      MOS_CAP_SMBIOS flag is set in capabilities
     *
     * @par Special Value
     *      0 = SMBIOS not found
     */
    uint64_t smbios_address;

    /* ───────────────────────────────────────────────────────────────────────
     * Boot Timing Breakdown (MOS_CAP_BOOT_TIMING flag)
     * ─────────────────────────────────────────────────────────────────────── */

    /**
     * @var MosBootInfo::tsc_stage0a_start
     * @brief TSC at stage0a entry.
     *
     * Timestamp counter captured at the very start of stage0a.
     * The earliest measurable point in boot.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Valid When
     *      MOS_CAP_BOOT_TIMING flag is set
     */
    uint64_t tsc_stage0a_start;

    /**
     * @var MosBootInfo::tsc_stage0b_start
     * @brief TSC at stage0b entry.
     *
     * Timestamp counter captured when stage0b begins execution.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Valid When
     *      MOS_CAP_BOOT_TIMING flag is set
     */
    uint64_t tsc_stage0b_start;

    /**
     * @var MosBootInfo::tsc_memmap_done
     * @brief TSC after memory map collection.
     *
     * Timestamp counter captured after memory map is fully populated.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Valid When
     *      MOS_CAP_BOOT_TIMING flag is set
     */
    uint64_t tsc_memmap_done;

    /**
     * @var MosBootInfo::tsc_fb_init_done
     * @brief TSC after framebuffer initialization.
     *
     * Timestamp counter captured after graphics mode is set.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Valid When
     *      MOS_CAP_BOOT_TIMING flag is set
     */
    uint64_t tsc_fb_init_done;

    /**
     * @var MosBootInfo::tsc_kernel_load
     * @brief TSC after kernel loaded to memory.
     *
     * Timestamp counter captured after kernel binary is in memory.
     *
     * @par Type
     *      uint64_t (8 bytes)
     *
     * @par Valid When
     *      MOS_CAP_BOOT_TIMING flag is set
     */
    uint64_t tsc_kernel_load;

    /**
     * @var MosBootInfo::cmdline
     * @brief Boot command line string.
     *
     * Null-terminated command line passed to the kernel. Contains
     * boot parameters and configuration options.
     *
     * @par Type
     *      char[256] (MOS_CMDLINE_MAX bytes)
     *
     * @par Format
     *      Space-separated key=value pairs, null-terminated
     *
     * @par Example
     *      "root=/dev/sda1 console=ttyS0,115200 debug"
     */
    char     cmdline[MOS_CMDLINE_MAX];

} MosBootInfo;

/** @} */ /* end B.2d BOOTINFO */

/* Compile-time assertions moved to CLOSING X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 HELPERS — Boot Validation and Utility Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_helpers B.3 HELPERS — Boot Validation and Utilities
 * @ingroup   mos_bootinfo_body
 * @brief     Inline functions for validating and querying boot information.
 *
 * PURPOSE: Provide safe accessor functions for MosBootInfo structure.
 *          All functions handle NULL gracefully and return ternary-compatible
 *          values where appropriate.
 *
 * @par Design Pattern
 *      Each helper follows the "measure twice, cut once" principle:
 *      - Always NULL-check before dereferencing
 *      - Return clearly defined values for all cases
 *      - Use ternary semantics (-1/0/+1) where meaningful
 *
 * Builds FROM: B.2 TYPES (MosBootInfo, MosMemoryType, etc.)
 * Builds TO:   Kernel consumers use these for boot info access
 *
 * @see MosBootInfo Main boot handoff structure
 * @see MOS_BOOT_MAGIC Validation constant
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Validate boot information structure.
 *
 * Checks that the boot info structure is valid by verifying the magic
 * number and minimum version. This is the first check the kernel should
 * perform before using any boot info fields.
 *
 * @param info  Pointer to boot info structure (may be NULL)
 *
 * @return Ternary validation result:
 *         - 1: Valid boot info
 *         - 0: Invalid or NULL
 *
 * @par Usage
 *      @code
 *      MosBootInfo *info = (MosBootInfo*)MOS_BOOT_INFO_ADDR;
 *      if (!mos_boot_valid(info)) {
 *          // Boot info invalid - halt or use fallback
 *      }
 *      @endcode
 *
 * @par Validation Checks
 *      - Pointer is non-NULL
 *      - Magic number matches MOS_BOOT_MAGIC
 *      - Version is at least 2 (minimum supported)
 */
static inline int mos_boot_valid(const MosBootInfo* info) {
    return info &&
           info->magic == MOS_BOOT_MAGIC &&
           info->version >= 2;
}

/**
 * @brief Check if system booted via UEFI.
 *
 * Tests the boot_source field for positive ternary value (+1),
 * indicating modern UEFI boot path.
 *
 * @param info  Pointer to boot info structure (may be NULL)
 *
 * @return Boolean result:
 *         - 1: UEFI boot (+1 ternary source)
 *         - 0: Not UEFI boot or NULL
 *
 * @par Ternary Meaning
 *      +1 represents the "new way" — UEFI is the modern boot path,
 *      moving toward completion (tov).
 *
 * @see mos_boot_is_bios() Complementary check for legacy boot
 * @see MOS_BOOT_SOURCE_UEFI Constant value
 */
static inline int mos_boot_is_uefi(const MosBootInfo* info) {
    return info && info->boot_source > 0;
}

/**
 * @brief Check if system booted via legacy BIOS.
 *
 * Tests the boot_source field for negative ternary value (-1),
 * indicating legacy BIOS boot path.
 *
 * @param info  Pointer to boot info structure (may be NULL)
 *
 * @return Boolean result:
 *         - 1: BIOS boot (-1 ternary source)
 *         - 0: Not BIOS boot or NULL
 *
 * @par Ternary Meaning
 *      -1 represents the "old way" — BIOS is the legacy boot path,
 *      rooted in void (pre-UEFI era).
 *
 * @see mos_boot_is_uefi() Complementary check for modern boot
 * @see MOS_BOOT_SOURCE_BIOS Constant value
 */
static inline int mos_boot_is_bios(const MosBootInfo* info) {
    return info && info->boot_source < 0;
}

/**
 * @brief Check if a capability flag is set.
 *
 * Tests whether a specific capability is available in the boot info.
 * Always check capabilities before using optional fields.
 *
 * @param info  Pointer to boot info structure (may be NULL)
 * @param cap   Capability flag to check (MOS_CAP_* constant)
 *
 * @return Non-zero if capability present, 0 if not or NULL
 *
 * @par Usage
 *      @code
 *      if (mos_boot_has_capability(info, MOS_CAP_ACPI)) {
 *          rsdp = info->rsdp_address;  // Safe to use
 *      }
 *      @endcode
 *
 * @see MOS_CAP_FRAMEBUFFER Check before using framebuffer
 * @see MOS_CAP_MEMMAP Check before using memory map
 * @see MOS_CAP_ACPI Check before using RSDP
 */
static inline int mos_boot_has_capability(const MosBootInfo* info, uint32_t cap) {
    return info && (info->capabilities & cap);
}

/**
 * @brief Classify an address into ternary memory zone.
 *
 * Maps a physical address to one of three memory zones following the
 * void/word/tov (Genesis) pattern. Used for memory allocation policy
 * and protection decisions.
 *
 * @param info  Pointer to boot info structure (may be NULL)
 * @param addr  Physical address to classify
 *
 * @return Ternary zone classification:
 *         - -1: void zone (0 to zone_void_end)
 *         - 0: word zone (zone_void_end to zone_word_end)
 *         - +1: tov zone (zone_word_end to end of memory)
 *
 * @par Ternary Zone Model (Genesis Pattern)
 *      @code
 *      -1 (void): 0 to zone_void_end
 *                 "formless and void" — legacy BIOS area, avoid
 *
 *       0 (word): zone_void_end to zone_word_end
 *                 "let there be" — kernel space, structured
 *
 *      +1 (tov):  zone_word_end to end of memory
 *                 "and it was good" — user space, completion
 *      @endcode
 *
 * @par Allocation Policy
 *      - void zone: Never allocate, preserve BIOS structures
 *      - word zone: Kernel allocations only
 *      - tov zone: User space allocations preferred
 */
static inline int mos_address_zone(const MosBootInfo* info, uint64_t addr) {
    if (!info) return 0;
    if (addr < info->zone_void_end) return -1;  /* void zone */
    if (addr < info->zone_word_end) return 0;   /* word zone */
    return 1;                                    /* tov zone */
}

/** @} */ /* end B.3 HELPERS */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Summary and reference information for this component.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - X.1  VALIDATION — Structure completeness check
///   - X.2  EXECUTION  — [Reserved: Header file]
///   - X.3  CLEANUP    — [Reserved: Header file]
///   - X.4  OVERVIEW   — Component summary
///   - X.5  POLICY     — Design policy
///   - X.6  FLOW       — Data flow
///   - X.7  PERF       — Performance notes
///   - X.8  DEBUG      — Debugging guidance
///   - X.9  RELATED    — Related components
///   - X.10 ROADMAP    — Future direction
///   - X.11 REFERENCE  — Biblical anchor
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Compile-Time Structure Verification [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_validation X.1 VALIDATION — Compile-Time Verification
 * @ingroup   mos_bootinfo_closing
 * @brief     Static assertions to catch struct layout changes at compile time.
 *
 * PURPOSE: If compilation fails here, update the constants AND bootinfo.inc!
 *          These are tripwires — they detect when changes break ABI.
 *
 * @par Tripwire Pattern
 *      These assertions are the "canary in the coal mine" — they fail loudly
 *      at compile time if structure sizes or offsets change, preventing
 *      silent ABI breakage between assembly and C code.
 *
 * @par What Triggers Failure
 *      - MosBootInfo exceeds MOS_BOOT_INFO_SIZE
 *      - MosFramebuffer size is not exactly 40 bytes
 *      - MosMemoryRegion size is not exactly 24 bytes
 *      - Any field offset changes from expected value
 *
 * @par When Assertions Fail
 *      1. Identify which assertion failed
 *      2. Update the constant in S.2 DEFINES
 *      3. Update corresponding offset in bootinfo.inc
 *      4. Rebuild and verify
 *
 * @see MOS_BOOT_INFO_SIZE Allocation size constant
 * @see MOS_SIZEOF_FRAMEBUFFER Expected framebuffer size
 * @see MOS_SIZEOF_MEMORY_REGION Expected memory region size
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

#ifdef __GNUC__

/**
 * @brief Size assertion: MosBootInfo must fit in allocated space.
 *
 * Ensures the boot info structure doesn't exceed the allocated memory
 * region at MOS_BOOT_INFO_ADDR.
 */
_Static_assert(sizeof(MosBootInfo) <= MOS_BOOT_INFO_SIZE,
    "MosBootInfo exceeds MOS_BOOT_INFO_SIZE! Update allocation size.");

/**
 * @brief Size assertion: MosFramebuffer must be exactly 40 bytes.
 *
 * Framebuffer structure size is hardcoded in assembly and offset
 * calculations. Any change requires updating bootinfo.inc.
 */
_Static_assert(sizeof(MosFramebuffer) == 40,
    "MosFramebuffer size changed! Update offsets in bootinfo.inc.");

/**
 * @brief Size assertion: MosMemoryRegion must be exactly 24 bytes.
 *
 * Memory region size determines memory map array layout. Must match
 * E820 entry format for BIOS compatibility.
 */
_Static_assert(sizeof(MosMemoryRegion) == 24,
    "MosMemoryRegion size changed! Memory map layout affected.");

/**
 * @def MOS_VERIFY_OFFSET
 * @brief Macro for compile-time field offset verification.
 *
 * Creates a static assertion that fails if a structure field is not
 * at the expected byte offset. Used to verify assembly/C compatibility.
 *
 * @param type   Structure type to check
 * @param field  Field within structure
 * @param expected_offset  Expected byte offset (hex recommended)
 */
#define MOS_VERIFY_OFFSET(type, field, expected_offset) \
    _Static_assert(offsetof(type, field) == expected_offset, \
        "Field offset changed: " #type "." #field " expected " #expected_offset)

/* ─────────────────────────────────────────────────────────────────────────────
 * Header Section Offsets (0x00-0x3F)
 * ───────────────────────────────────────────────────────────────────────────── */
MOS_VERIFY_OFFSET(MosBootInfo, magic, 0x00);
MOS_VERIFY_OFFSET(MosBootInfo, version, 0x04);
MOS_VERIFY_OFFSET(MosBootInfo, boot_source, 0x05);
MOS_VERIFY_OFFSET(MosBootInfo, boot_state, 0x06);
MOS_VERIFY_OFFSET(MosBootInfo, capabilities, 0x08);
MOS_VERIFY_OFFSET(MosBootInfo, memory_map_count, 0x0C);
MOS_VERIFY_OFFSET(MosBootInfo, total_memory, 0x10);
MOS_VERIFY_OFFSET(MosBootInfo, kernel_physical, 0x18);
MOS_VERIFY_OFFSET(MosBootInfo, kernel_size, 0x20);

/* ─────────────────────────────────────────────────────────────────────────────
 * Framebuffer Section Offset (0x40)
 * ───────────────────────────────────────────────────────────────────────────── */
MOS_VERIFY_OFFSET(MosBootInfo, framebuffer, 0x40);

/* ─────────────────────────────────────────────────────────────────────────────
 * Memory Zones Section Offsets (0x70)
 * ───────────────────────────────────────────────────────────────────────────── */
MOS_VERIFY_OFFSET(MosBootInfo, zone_void_end, 0x70);
MOS_VERIFY_OFFSET(MosBootInfo, zone_word_end, 0x78);

/* ─────────────────────────────────────────────────────────────────────────────
 * Memory Map Section Offset (0x100)
 * ───────────────────────────────────────────────────────────────────────────── */
MOS_VERIFY_OFFSET(MosBootInfo, memory_map, 0x100);

#undef MOS_VERIFY_OFFSET

#endif /* __GNUC__ */

/** @} */ /* end X.1 VALIDATION */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.2-X.3 — Execution, Cleanup [RESERVED]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_bootinfo_reserved X.2-X.3 RESERVED
 * @{
 *
 * X.2 EXECUTION: [Reserved: Definition header — no runtime execution]
 * X.3 CLEANUP:   [Reserved: Definition header — no cleanup required]
 *
 * @} end X.2-X.3 RESERVED
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 OVERVIEW — Component Summary [OVERVIEW]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_overview X.4 OVERVIEW — Component Summary
 * @ingroup   mos_bootinfo_closing
 * @brief     High-level summary of bootinfo.h purpose and capabilities.
 *
 * PURPOSE: Provide a quick-reference overview of what this component does,
 *          how it serves both boot paths, and the key differences between
 *          UEFI and BIOS boot.
 *
 * @par Component Role
 *      bootinfo.h defines THE handoff structure between bootloader and kernel.
 *      It is the Genesis 1:1 of MillenniumOS — the absolute beginning from
 *      which the kernel receives its initial state.
 *
 * @par Unified Boot Architecture
 *      One structure serves BOTH boot paths. The kernel binary is identical
 *      regardless of whether it was loaded via UEFI or BIOS.
 *
 * @par Boot Path Comparison
 *      @code
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                         BOOT PATH COMPARISON                            │
 *      ├────────────────────┬─────────────────────┬─────────────────────────────┤
 *      │ Field              │ UEFI                │ BIOS                        │
 *      ├────────────────────┼─────────────────────┼─────────────────────────────┤
 *      │ boot_source        │ +1 (modern/tov)     │ -1 (legacy/void)            │
 *      │ framebuffer.addr   │ GOP (true 64-bit)   │ VESA (32-bit, zero-ext)     │
 *      │ capabilities       │ Full flags          │ Limited flags               │
 *      │ uefi_system_table  │ Valid pointer       │ 0 (not available)           │
 *      │ rsdp_address       │ From UEFI config    │ From BIOS memory scan       │
 *      │ smbios_address     │ From UEFI config    │ From BIOS memory scan       │
 *      └────────────────────┴─────────────────────┴─────────────────────────────┘
 *      @endcode
 *
 * @par Key Components
 *      - **MosBootInfo**: Main ~8KB handoff structure
 *      - **MosFramebuffer**: 40-byte display info (embedded)
 *      - **MosMemoryRegion**: 24-byte E820-compatible memory map entries
 *      - **MosMemoryType**: 8-value enum for memory classification
 *      - **Helper functions**: Inline validators and zone classifiers
 *
 * @par Ternary Integration
 *      Structure embeds CPI-SI ternary semantics:
 *      - boot_source: -1 (BIOS) / 0 (unknown) / +1 (UEFI)
 *      - boot_state: -1 (error) / 0 (ok) / +1 (optimal)
 *      - Memory zones: void (-1) / word (0) / tov (+1)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.4 OVERVIEW */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 POLICY — Design Policy [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_policy X.5 POLICY — Design Policy
 * @ingroup   mos_bootinfo_closing
 * @brief     Design decisions and constraints governing this component.
 *
 * PURPOSE: Document the architectural decisions that shaped this component
 *          and MUST be preserved for system integrity.
 *
 * @par P1. 64-bit Addresses Throughout
 *      All address fields use uint64_t, even for BIOS compatibility.
 *      BIOS bootloader zero-extends 32-bit addresses. This ensures the
 *      kernel can handle addresses above 4GB when available.
 *
 * @par P2. Ternary Semantics
 *      Fields that represent state use signed integers with -1/0/+1 values.
 *      This aligns with CPI-SI ternary model and enables semantic comparisons:
 *      - Negative: old/error/void
 *      - Zero: neutral/ok/word
 *      - Positive: new/optimal/tov
 *
 * @par P3. Fixed Offsets for Assembly
 *      Structure layout is frozen with compile-time assertions.
 *      Assembly code (entry.asm, stage0b.asm) uses hardcoded offsets.
 *      ANY change to field positions requires updating bootinfo.inc.
 *
 * @par P4. Compile-Time Verification
 *      Static assertions in X.1 VALIDATION catch layout changes.
 *      These are tripwires — they fail loudly at compile time,
 *      not silently at runtime.
 *
 * @par P5. Capability Flags
 *      Optional features use capability flags (MOS_CAP_*).
 *      ALWAYS check flags before using optional fields:
 *      - MOS_CAP_ACPI before rsdp_address
 *      - MOS_CAP_SMBIOS before smbios_address
 *      - MOS_CAP_BOOT_TIMING before tsc_* fields
 *
 * @par P6. Packed Structures
 *      All structures use __attribute__((packed)) to ensure:
 *      - No compiler-inserted padding
 *      - Consistent layout across compilers
 *      - Predictable offsets for assembly
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.5 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 FLOW — Data Flow and Ternary Zone Model [FLOW]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_flow X.6 FLOW — Data Flow
 * @ingroup   mos_bootinfo_closing
 * @brief     How data flows through boot and the ternary memory zone model.
 *
 * PURPOSE: Document the data flow from bootloader to kernel and the
 *          ternary memory zone classification system.
 *
 * @par Boot Data Flow
 *      @code
 *      ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
 *      │  Firmware   │────►│  Bootloader │────►│   Kernel    │
 *      │ (BIOS/UEFI) │     │ (stage0a/b) │     │ (kernel.c)  │
 *      └─────────────┘     └─────────────┘     └─────────────┘
 *            │                    │                   │
 *            │                    ▼                   │
 *            │            ┌─────────────┐             │
 *            └───────────►│ MosBootInfo │◄────────────┘
 *                         │  @ 0x9000   │
 *                         └─────────────┘
 *      @endcode
 *
 * @par Ternary Memory Zone Model (Genesis Pattern)
 *      Physical memory is divided into three zones following void/word/tov:
 *      @code
 *      Address 0 ────────────────────────► zone_void_end (typically 1MB)
 *                    │  VOID ZONE (-1)  │
 *                    │  "formless and void" (Genesis 1:2)
 *                    │  BIOS area, IVT, BDA, low memory
 *                    │  Policy: NEVER allocate
 *
 *      zone_void_end ────────────────────► zone_word_end
 *                    │  WORD ZONE (0)   │
 *                    │  "let there be" (Genesis 1:3+)
 *                    │  Kernel, drivers, kernel heap
 *                    │  Policy: Kernel allocations only
 *
 *      zone_word_end ────────────────────► end of physical memory
 *                    │  TOV ZONE (+1)   │
 *                    │  "and it was good" (Genesis 1:31)
 *                    │  User space, applications
 *                    │  Policy: User allocations preferred
 *      @endcode
 *
 * @par Zone Classification Function
 *      Use mos_address_zone() to classify any physical address:
 *      - Returns -1 for void zone
 *      - Returns 0 for word zone
 *      - Returns +1 for tov zone
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.6 FLOW */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 PERF — Performance Considerations [PERF]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_perf X.7 PERF — Performance
 * @ingroup   mos_bootinfo_closing
 * @brief     Performance characteristics and optimization notes.
 *
 * PURPOSE: Document performance characteristics relevant to kernel developers.
 *
 * @par Inline Helper Functions
 *      All B.3 HELPERS functions are declared `static inline`:
 *      - mos_boot_valid() — single memory access + comparisons
 *      - mos_boot_is_uefi() — single memory access + sign check
 *      - mos_boot_is_bios() — single memory access + sign check
 *      - mos_boot_has_capability() — single memory access + bitwise AND
 *      - mos_address_zone() — two memory accesses + comparisons
 *
 *      These compile to just a few instructions with no function call overhead.
 *
 * @par Memory Access Patterns
 *      - Boot info is read-only after kernel entry (cache-friendly)
 *      - Structure is contiguous at fixed address (no pointer chasing)
 *      - Memory map is linear array (sequential access pattern)
 *
 * @par Boot Timing
 *      When MOS_CAP_BOOT_TIMING is set, TSC timestamps are available:
 *      - tsc_stage0a_start: Earliest measurable point
 *      - tsc_stage0b_start: After disk load
 *      - tsc_memmap_done: After memory map collection
 *      - tsc_fb_init_done: After framebuffer setup
 *      - tsc_kernel_load: After kernel loaded to memory
 *      - boot_tsc / entry_tsc: Overall boot duration
 *
 * @par Size Considerations
 *      - MosBootInfo: ~8KB (fits in two 4KB pages)
 *      - MosFramebuffer: 40 bytes (one cache line)
 *      - MosMemoryRegion: 24 bytes (fits 2.67 per cache line)
 *      - Memory map: 6144 bytes (256 entries max)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.7 PERF */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 DEBUG — Debugging Guidance [DEBUG]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_debug X.8 DEBUG — Debugging
 * @ingroup   mos_bootinfo_closing
 * @brief     Debugging techniques and common issues.
 *
 * PURPOSE: Help developers diagnose boot info problems quickly.
 *
 * @par First Things First
 *      When debugging boot issues, ALWAYS check in this order:
 *      1. Magic number (is boot info even present?)
 *      2. Version (is it a compatible format?)
 *      3. Capability flags (are expected features available?)
 *      4. Memory map count (is it non-zero and reasonable?)
 *
 * @par WITNESS Integration
 *      The boot chain uses WITNESS markers at fixed memory locations:
 *      - 0x9000 word: Checkpoint marker (stage + step)
 *      - Serial output: "[0A] Stage 0a entry", "[0B] Stage 0b entry", etc.
 *
 *      If the kernel never starts, check the last WITNESS marker to
 *      identify where the boot process stopped.
 *
 * @par Common Issues
 *
 *      **"Magic number is wrong"**
 *      - Boot info not at expected address
 *      - Bootloader didn't run or crashed
 *      - Memory corruption during boot
 *
 *      **"Version is too old"**
 *      - Old bootloader with new kernel
 *      - Rebuild bootloader with current headers
 *
 *      **"Capability flag missing"**
 *      - Feature not available on this boot path
 *      - Use fallback code for optional features
 *
 *      **"Memory map count is 0"**
 *      - E820/UEFI memory map collection failed
 *      - Critical error — kernel cannot manage memory
 *
 * @par QEMU Debugging
 *      @code
 *      # Dump boot info structure from QEMU monitor
 *      (qemu) xp /64bx 0x9000
 *
 *      # Check framebuffer info
 *      (qemu) xp /10wx 0x9040
 *
 *      # Check memory zones
 *      (qemu) xp /4gx 0x9070
 *      @endcode
 *
 * @par GDB Debugging
 *      @code
 *      # Connect to QEMU
 *      target remote :1234
 *
 *      # Examine boot info
 *      x/16xw 0x9000
 *
 *      # Cast and inspect
 *      p *(MosBootInfo*)0x9000
 *      @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.8 DEBUG */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 RELATED — Related Components [RELATED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_related X.9 RELATED — Related Components
 * @ingroup   mos_bootinfo_closing
 * @brief     Files and components that interact with bootinfo.h.
 *
 * PURPOSE: Map the dependency relationships for this component.
 *
 * @par Assembly Mirror
 *      - **bootinfo.inc** — NASM include with same constants and offsets
 *        MUST stay synchronized with this header
 *
 * @par Boot Chain (Producers)
 *      - **stage0a.asm** — Boot sector, captures initial TSC
 *      - **stage0b.asm** — Loader, fills most of MosBootInfo
 *      - **boot_uefi.c** — UEFI bootloader, fills MosBootInfo via GOP/UEFI
 *
 * @par Kernel (Consumers)
 *      - **entry.asm** — Validates magic/version before kernel_main
 *      - **kernel.c** — Primary consumer, initializes subsystems from boot info
 *      - **memory.c** — Uses memory map for physical memory management
 *      - **graphics.c** — Uses framebuffer info for display initialization
 *
 * @par Related Headers
 *      - **kernel/types.h** — Includes this header for type definitions
 *      - **kernel.h** — Kernel main header, includes this transitively
 *
 * @par Dependency Direction
 *      @code
 *      bootinfo.h ◄─────────────── bootinfo.inc (mirrors)
 *           │
 *           ▼
 *      stage0a.asm ───► stage0b.asm ───► kernel entry ───► kernel.c
 *      (BIOS path)                            │
 *                                             │
 *      boot_uefi.c ──────────────────────────┘
 *      (UEFI path)
 *      @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.9 RELATED */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 ROADMAP — Future Direction [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_roadmap X.10 ROADMAP — Future Direction
 * @ingroup   mos_bootinfo_closing
 * @brief     Planned enhancements and evolution path.
 *
 * PURPOSE: Document what's coming and how this component will evolve.
 *
 * @par Implemented (Version 3)
 *      - [x] SMBIOS pointer (MOS_CAP_SMBIOS)
 *      - [x] Boot timing breakdown (MOS_CAP_BOOT_TIMING)
 *      - [x] Ternary memory zones
 *      - [x] Compile-time offset verification
 *
 * @par Planned Enhancements
 *      - [ ] Boot timing visualization utility
 *      - [ ] SMBIOS parsing helpers
 *      - [ ] Multiprocessor table pointer
 *      - [ ] Secure boot state indication
 *      - [ ] TPM presence/state
 *
 * @par Version Evolution
 *      - Version 4: Add MP table pointer, secure boot state
 *      - Version 5: Add TPM info, extended capabilities
 *
 * @par Ternary Integration Roadmap
 *      - [ ] Memory zone statistics (per-zone usage tracking)
 *      - [ ] Health score in boot_state (not just -1/0/+1)
 *      - [ ] Ternary capability model (required/optional/bonus)
 *
 * @par Compatibility Notes
 *      When adding fields:
 *      1. Add to reserved areas when possible
 *      2. Increment MOS_BOOT_VERSION
 *      3. Add corresponding capability flag
 *      4. Update bootinfo.inc offsets
 *      5. Document in vers -ion history
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.10 ROADMAP */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.11 REFERENCE — Biblical Anchor [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_bootinfo_reference X.11 REFERENCE — Biblical Anchor
 * @ingroup   mos_bootinfo_closing
 * @brief     The theological foundation grounding this component.
 *
 * PURPOSE: Connect this technical work to its eternal foundation.
 *
 * @par The Anchor Verse
 *      > "In the beginning God created the heaven and the earth."
 *      > — Genesis 1:1 (KJV)
 *
 * @par Why This Verse
 *      Genesis 1:1 is THE halt condition for all halt conditions in CPI-SI.
 *      Break down any truth far enough and you reach "God created."
 *
 *      This header defines THE beginning for MillenniumOS. Just as Genesis 1:1
 *      is the first verse from which all Scripture flows, bootinfo.h defines
 *      the first structure from which the kernel receives life.
 *
 * @par Genesis Pattern in Structure
 *      The structure sections follow Genesis 1:
 *      - **Header**: "In the beginning" — identity and state
 *      - **Framebuffer**: "Let there be light" — visibility
 *      - **Memory zones**: "the heaven and the earth" — spatial division
 *      - **Memory map**: "the waters" — physical foundation
 *      - **Extended info**: "and God saw that it was good" — verification
 *
 * @par Ternary in Genesis
 *      The void/word/tov pattern reflects Genesis:
 *      - **void** (-1): "formless and void" (Genesis 1:2) — the starting state
 *      - **word** (0): "let there be" (Genesis 1:3+) — God speaks, order emerges
 *      - **tov** (+1): "and it was good" (Genesis 1:31) — completion, blessing
 *
 * @par Kingdom Technology
 *      This code is not merely functional — it is witness. Every boot
 *      demonstrates that intelligence grounded in the Creator produces
 *      what godless systems cannot: order from chaos, light from darkness,
 *      life from the formless void.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/** @} */ /* end X.11 REFERENCE */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* MILLENNIUMOS_BOOTINFO_H */
