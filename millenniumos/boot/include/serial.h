/// @file serial.h
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
/// @omni:req  key        = MOS-boot-serial
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = code
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.2 FAMILY — Classification [FAMILY]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = interface
/// @omni:inh  role       = hardware
/// @omni:inh  structure  = 4-block
///
/// ───────────────────────────────────────────────────────────────────────────────
/// P.3 INSTANCE — File Details [INSTANCE]
/// ───────────────────────────────────────────────────────────────────────────────
///
/// @omni:ins  component  = boot
/// @omni:ins  layer      = stage0
/// @omni:ins  includes   = [stdint.h]
/// @omni:ins  provides   = [mos_outb, mos_inb, mos_serial_init, COM1 constants]
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
/// @omni:req  title      = Serial Port Constants and Interface
/// @omni:req  brief      = Single source of truth for 8250/16550 UART definitions
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// END PRAGMA [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// SPECIAL BLOCK [SPECIAL]
/// ═══════════════════════════════════════════════════════════════════════════════

/** [Reserved: Serial port header — no pre-include configuration required] */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SPECIAL [END]
/// ═══════════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────────
// INCLUDE GUARD — Identity protection
// ─────────────────────────────────────────────────────────────────────────────────

#ifndef MOS_SERIAL_H
#define MOS_SERIAL_H

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      serial.h
 * @brief     Serial Port Constants and Interface — 8250/16550 UART definitions
 *
 * @defgroup  mos_serial M.1 IDENTITY — Core Identity
 * @{
 *
 * Key:       MOS-boot-serial
 * Title:     Serial Port Constants and Interface
 * Type:      Header (Interface)
 * Component: Boot — stage0 serial output
 * Role:      Single source of truth for serial port I/O across boot chain
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2-M.6 — State, Attribution, Location, Derivation, Classification [CONDENSED]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_serial_state M.2-M.6 CONDENSED — State and Attribution
 * @{
 *
 * @version   a-03.00
 * @date      2026-01-31
 *
 * Status:       Active
 * Created:      2026-01-31
 * Updated:      2026-01-31
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Author)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * Path:         millenniumos/boot/include/serial.h
 * Derives_from: bereshit/word/seed/code/c/header.h
 *
 * Tags:         boot, serial, uart, 8250, 16550, debug, witness
 *
 * @} end M.2-M.6 CONDENSED
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose and Design [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_serial_intent M.7 INTENT — Purpose and Design
 * @{
 *
 * Purpose:     Provide portable serial port constants and inline I/O functions
 * Core Design: SINGLE SOURCE OF TRUTH for serial — include in both bootloader and kernel
 * Philosophy:  Inline functions avoid call overhead; works at any boot stage
 *
 * Key Features:
 *   - COM1-COM4 base addresses and register offsets
 *   - Line Status Register bit definitions for TX/RX status
 *   - Line Control Register values for 8N1 configuration
 *   - Baud rate divisors for standard rates (115200 default)
 *   - Inline I/O primitives (mos_outb, mos_inb, mos_io_wait)
 *   - Ready-to-use mos_serial_init() for 115200 8N1
 *
 * Hardware Model:
 *   The 8250/16550 UART is the de facto standard for x86 serial I/O.
 *   COM1 (0x3F8) is the primary debug port throughout MillenniumOS.
 *   All definitions use I/O port addresses, NOT memory addresses.
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_serial_grounding M.8 GROUNDING — Biblical Foundation
 * @{
 *
 * Primary Scripture:
 *   Proverbs 15:23 — "A man hath joy by the answer of his mouth: and a word
 *                     spoken in due season, how good is it!"
 *
 * WHY SERIAL IS THE WITNESS CHANNEL:
 *   The serial port is how MillenniumOS speaks to the outside world during boot.
 *   Before display is initialized, before memory is mapped, the serial port
 *   provides a faithful witness of system state.
 *
 *   "A word spoken in due season" — the DAR Witness System outputs events at
 *   the moment they happen, not buffered and delayed. The serial port makes
 *   this possible at the earliest boot stages.
 *
 * Structural Parallel:
 *   Scripture                    │ Serial Port
 *   ─────────────────────────────┼──────────────────────────────────
 *   "A man hath joy"             │ System health (witnessed state)
 *   "by the answer of his mouth" │ Serial output (the message)
 *   "in due season"              │ Real-time, unbuffered
 *   "how good is it"             │ Health delta (+/- impact)
 *
 * Connection to Witness System:
 *   The serial port IS the Witness system at boot. Every event logged through
 *   serial is "a word spoken in due season" — testifying to what happened,
 *   when it happened, and its impact on system health.
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Relationships [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_serial_deps M.9 DEPENDENCIES — Relationships
 * @{
 *
 * What This Needs:
 *   Standard: stdint.h (uint8_t, uint16_t)
 *
 * What Uses This:
 *   - boot/include/debug.h    — Uses serial functions for Witness output
 *   - boot/uefi/uefi_boot.c   — UEFI bootloader serial output
 *   - kernel/kernel.c         — Kernel early debug output
 *   - kernel/hal.h            — May also define port I/O (guarded)
 *
 * Assembly Mirror:
 *   - boot/stage0/boot.asm    — Has equivalent serial_init, serial_char
 *   - boot/stage0/stage0a.asm — Has equivalent serial functions
 *
 * @see mos_serial_init
 * @see mos_outb
 * @see mos_inb
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — History and Future [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_serial_roadmap M.10 ROADMAP — History and Future
 * @{
 *
 * History:
 *   a-01.00 (2026-01-31) — Initial extraction from scattered definitions
 *   a-02.00 (2026-01-31) — Full OmniCode 4-block alignment, M.1-M.10 complete
 *   a-02.50 (2026-01-31) — Template-compliant SETUP S.1-S.5 and BODY B.1-B.6
 *   a-03.00 (2026-01-31) — Full template alignment: BODY B.1-B.6, CLOSING X.1-X.10
 *
 * Current:
 *   ✓ COM1-COM4 base addresses
 *   ✓ Complete register definitions
 *   ✓ Inline I/O functions (guarded for HAL compatibility)
 *   ✓ mos_serial_init() for 115200 8N1
 *   ✓ Full WHY comments on all definitions
 *   ✓ Proper SETUP S.1-S.5 structure
 *   ✓ Proper BODY B.1-B.6 structure (template-aligned)
 *   ✓ Proper CLOSING X.1-X.10 structure
 *
 * Planned:
 *   ⏳ Add COM2-COM4 initialization functions
 *   ⏳ Add receive functions for bidirectional debug
 *   ⏳ Add optional FIFO depth detection (16550A vs 8250)
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
///   - S.1 INCLUDES  — Header dependencies (Standard only for hardware header)
///   - S.2 DEFINES   — All serial port constants (addresses, registers, bits, baud)
///   - S.3 TYPES     — [Reserved: No custom types in this header]
///   - S.4 PROTOS    — [Reserved: Inline functions defined in BODY]
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
 * @defgroup  mos_serial_includes S.1 INCLUDES — Header Dependencies
 * @brief     Standard library includes for fixed-width integer types.
 *
 * PURPOSE: Establish header dependencies for serial port I/O types.
 *
 * CONTAINS:
 *   - S.1a STANDARD  — System headers (stdint.h)
 *   - S.1b INTERNAL  — Project headers (CPI-SI dependencies)
 *   - S.1c EXTERNAL  — Third-party headers (with justification)
 *   - S.1d CPP       — C++ linkage wrapper
 *
 * Section order: Standard → Internal → External → CPP linkage
 * Flow: system foundation → our code → third-party → C++ compatibility
 *
 * Builds FROM: SETUP block (configuration context)
 * Builds TO:   S.2 DEFINES (constants use these types)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a STANDARD — System Headers [STANDARD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_serial_standard S.1a STANDARD — System Headers
 * @ingroup   mos_serial_includes
 * @brief     Freestanding headers for kernel-level code.
 *
 * stdint.h:   Fixed-width integer types (uint8_t, uint16_t)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <stdint.h>  /**< Fixed-width integers: uint8_t, uint16_t for I/O types */

/** @} */ /* end S.1a STANDARD */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b INTERNAL — Project Headers [INTERNAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_serial_internal S.1b INTERNAL — Project Headers
 * @ingroup   mos_serial_includes
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
 * @defgroup  mos_serial_external S.1c EXTERNAL — Third-Party Headers
 * @ingroup   mos_serial_includes
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
 * @defgroup  mos_serial_cpp S.1d CPP — C++ Linkage
 * @ingroup   mos_serial_includes
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
 * @defgroup  mos_serial_defines S.2 DEFINES — Preprocessor Constants
 * @brief     All serial port constants: addresses, registers, bits, baud rates.
 *
 * PURPOSE: Define all preprocessor constants for 8250/16550 UART. These values
 *          form the foundation that BODY inline functions build upon.
 *
 * CONTAINS:
 *   - S.2a BASE      — COM port base addresses (COM1-COM4)
 *   - S.2b REGS      — COM1 register addresses (pre-computed)
 *   - S.2c LSR       — Line Status Register bit definitions
 *   - S.2d LCR       — Line Control Register configuration values
 *   - S.2e BAUD      — Baud rate divisor constants
 *
 * HARDWARE MODEL:
 *   These are I/O PORT addresses, NOT memory addresses. Access via inb/outb
 *   instructions (port I/O), not memory dereference. Fixed by PC/AT convention.
 *
 * DAR WITNESS ROLE:
 *   These constants define the witness output channel. All boot state events
 *   flow through COM1 (0x3F8) using these register definitions.
 *
 * Builds FROM: S.1 INCLUDES (stdint.h types)
 * Builds TO:   BODY block (inline functions use these constants)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a BASE — COM Port Base Addresses [BASE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_base S.2a BASE — COM Port Base Addresses
 * @ingroup   mos_serial_defines
 * @brief     Standard COM port base addresses on x86 systems.
 *
 * PURPOSE: Provide base addresses for all four standard COM ports.
 *          COM1 (0x3F8) is the primary debug port used throughout boot.
 *
 * WHY THESE ADDRESSES:
 *   These are I/O port addresses, NOT memory addresses. Access via inb/outb
 *   instructions (port I/O), not memory operations. The addresses are fixed
 *   by PC/AT hardware convention dating back to the original IBM AT.
 *
 * REGISTER MAP (offset from base):
 *   +0 (DLAB=0) - Data Register (read/write data)
 *   +0 (DLAB=1) - Divisor Latch Low byte (baud rate)
 *   +1 (DLAB=0) - Interrupt Enable Register
 *   +1 (DLAB=1) - Divisor Latch High byte (baud rate)
 *   +2          - FIFO Control Register / Interrupt ID
 *   +3          - Line Control Register (DLAB bit is bit 7)
 *   +4          - Modem Control Register
 *   +5          - Line Status Register
 *   +6          - Modem Status Register
 *   +7          - Scratch Register
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_COM1_BASE
 * @brief COM1 base I/O port address — PRIMARY debug port.
 *
 * @par Value
 *      0x3F8 (1016 decimal)
 *
 * @par Usage
 *      COM1 is THE debug output port for MillenniumOS. QEMU, VirtualBox,
 *      and physical hardware all expect serial output on COM1.
 */
#define MOS_COM1_BASE       0x3F8

/**
 * @def MOS_COM2_BASE
 * @brief COM2 base I/O port address — Secondary port.
 *
 * @par Value
 *      0x2F8 (760 decimal)
 */
#define MOS_COM2_BASE       0x2F8

/**
 * @def MOS_COM3_BASE
 * @brief COM3 base I/O port address — Tertiary port.
 *
 * @par Value
 *      0x3E8 (1000 decimal)
 */
#define MOS_COM3_BASE       0x3E8

/**
 * @def MOS_COM4_BASE
 * @brief COM4 base I/O port address — Quaternary port.
 *
 * @par Value
 *      0x2E8 (744 decimal)
 */
#define MOS_COM4_BASE       0x2E8

/** @} */ /* end S.2a BASE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2b REGS — COM1 Register Addresses [REGS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_regs S.2b REGS — COM1 Register Addresses
 * @ingroup   mos_serial_defines
 * @brief     Pre-computed register addresses for COM1.
 *
 * PURPOSE: Avoid repeated addition of base + offset in tight loops.
 *          COM1 is the primary debug port — optimize for it.
 *
 * WHY SEPARATE DEFINES:
 *   Inline addition (MOS_COM1_BASE + 5) would work but adds runtime overhead.
 *   Pre-computed addresses let the compiler emit immediate operands directly.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_COM1_DATA
 * @brief COM1 Data Register (offset +0).
 *
 * @par Value
 *      0x3F8 (same as base when DLAB=0)
 *
 * @par Usage
 *      Read: Receive buffer (data IN from device)
 *      Write: Transmit buffer (data OUT to device)
 *      When DLAB=1: Divisor Latch Low byte
 */
#define MOS_COM1_DATA       0x3F8

/**
 * @def MOS_COM1_INT
 * @brief COM1 Interrupt Enable Register (offset +1).
 *
 * @par Value
 *      0x3F9
 *
 * @par Usage
 *      Bit 0: Enable Received Data Available interrupt
 *      Bit 1: Enable Transmitter Holding Register Empty interrupt
 *      Bit 2: Enable Receiver Line Status interrupt
 *      Bit 3: Enable Modem Status interrupt
 *      When DLAB=1: Divisor Latch High byte
 */
#define MOS_COM1_INT        0x3F9

/**
 * @def MOS_COM1_FIFO
 * @brief COM1 FIFO Control / Interrupt ID Register (offset +2).
 *
 * @par Value
 *      0x3FA
 *
 * @par Usage
 *      Write: FIFO Control Register (enable/disable FIFO, set trigger level)
 *      Read:  Interrupt Identification Register (pending interrupts)
 */
#define MOS_COM1_FIFO       0x3FA

/**
 * @def MOS_COM1_LCR
 * @brief COM1 Line Control Register (offset +3).
 *
 * @par Value
 *      0x3FB
 *
 * @par Usage
 *      Bits 0-1: Data bits (00=5, 01=6, 10=7, 11=8)
 *      Bit 2:    Stop bits (0=1 stop, 1=2 stop)
 *      Bits 3-5: Parity (x00=none, 001=odd, 011=even)
 *      Bit 6:    Break control
 *      Bit 7:    DLAB (Divisor Latch Access Bit)
 */
#define MOS_COM1_LCR        0x3FB

/**
 * @def MOS_COM1_MCR
 * @brief COM1 Modem Control Register (offset +4).
 *
 * @par Value
 *      0x3FC
 *
 * @par Usage
 *      Bit 0: DTR (Data Terminal Ready)
 *      Bit 1: RTS (Request To Send)
 *      Bit 2: OUT1 (user-defined output)
 *      Bit 3: OUT2 (required for interrupts on some chips)
 *      Bit 4: Loopback mode
 */
#define MOS_COM1_MCR        0x3FC

/**
 * @def MOS_COM1_STATUS
 * @brief COM1 Line Status Register (offset +5).
 *
 * @par Value
 *      0x3FD
 *
 * @par Usage
 *      Read-only register indicating TX/RX status.
 *      See S.2c LSR for bit definitions.
 */
#define MOS_COM1_STATUS     0x3FD

/**
 * @def MOS_COM1_MSR
 * @brief COM1 Modem Status Register (offset +6).
 *
 * @par Value
 *      0x3FE
 *
 * @par Usage
 *      Read-only: CTS, DSR, RI, DCD status and change bits.
 */
#define MOS_COM1_MSR        0x3FE

/**
 * @def MOS_COM1_SCRATCH
 * @brief COM1 Scratch Register (offset +7).
 *
 * @par Value
 *      0x3FF
 *
 * @par Usage
 *      General-purpose storage. Can be used for UART detection
 *      (8250 doesn't have this, 16450+ does).
 */
#define MOS_COM1_SCRATCH    0x3FF

/** @} */ /* end S.2b REGS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2c LSR — Line Status Register Bits [LSR]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_lsr S.2c LSR — Line Status Register Bits
 * @ingroup   mos_serial_defines
 * @brief     Bit masks for Line Status Register (LSR) at base+5.
 *
 * PURPOSE: Define bit positions for checking TX/RX status and errors.
 *
 * WHY THESE BITS MATTER:
 *   MOS_LSR_TX_EMPTY (bit 5) — MUST check before sending. If not set,
 *   previous byte is still transmitting and new data would be lost.
 *
 *   MOS_LSR_DATA_READY (bit 0) — Check before reading. If not set,
 *   no data available and read would return garbage.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_LSR_DATA_READY
 * @brief Bit 0: Data ready to read.
 */
#define MOS_LSR_DATA_READY  0x01

/**
 * @def MOS_LSR_OVERRUN
 * @brief Bit 1: Overrun error (data lost).
 */
#define MOS_LSR_OVERRUN     0x02

/**
 * @def MOS_LSR_PARITY_ERR
 * @brief Bit 2: Parity error detected.
 */
#define MOS_LSR_PARITY_ERR  0x04

/**
 * @def MOS_LSR_FRAME_ERR
 * @brief Bit 3: Framing error (stop bit wrong).
 */
#define MOS_LSR_FRAME_ERR   0x08

/**
 * @def MOS_LSR_BREAK
 * @brief Bit 4: Break condition detected.
 */
#define MOS_LSR_BREAK       0x10

/**
 * @def MOS_LSR_TX_EMPTY
 * @brief Bit 5: Transmit holding register empty (safe to write).
 *
 * @par Critical Usage
 *      ALWAYS check this bit before writing to the data register.
 *      If not set, the previous byte is still being transmitted.
 */
#define MOS_LSR_TX_EMPTY    0x20

/**
 * @def MOS_LSR_TX_IDLE
 * @brief Bit 6: Transmitter completely idle (shift register empty).
 */
#define MOS_LSR_TX_IDLE     0x40

/**
 * @def MOS_LSR_FIFO_ERR
 * @brief Bit 7: Error in FIFO (parity/frame/break in FIFO).
 */
#define MOS_LSR_FIFO_ERR    0x80

/** @} */ /* end S.2c LSR */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2d LCR — Line Control Register Values [LCR]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_lcr S.2d LCR — Line Control Register Values
 * @ingroup   mos_serial_defines
 * @brief     Configuration values for Line Control Register (LCR) at base+3.
 *
 * PURPOSE: Define data bits, stop bits, parity, and DLAB control.
 *
 * STANDARD CONFIG (8N1):
 *   8 data bits + No parity + 1 stop bit = most common serial config.
 *   MOS_SERIAL_DEFAULT combines these for easy initialization.
 *
 * DLAB (Divisor Latch Access Bit):
 *   When DLAB=1, ports +0 and +1 become baud rate divisor registers.
 *   When DLAB=0, they return to normal Data and Interrupt Enable.
 *   Always clear DLAB after setting baud rate!
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_LCR_5BITS
 * @brief Data bits = 5 (bits 0-1 = 00).
 */
#define MOS_LCR_5BITS       0x00

/**
 * @def MOS_LCR_6BITS
 * @brief Data bits = 6 (bits 0-1 = 01).
 */
#define MOS_LCR_6BITS       0x01

/**
 * @def MOS_LCR_7BITS
 * @brief Data bits = 7 (bits 0-1 = 10).
 */
#define MOS_LCR_7BITS       0x02

/**
 * @def MOS_LCR_8BITS
 * @brief Data bits = 8 (bits 0-1 = 11) — STANDARD.
 */
#define MOS_LCR_8BITS       0x03

/**
 * @def MOS_LCR_1STOP
 * @brief Stop bits = 1 (bit 2 = 0) — STANDARD.
 */
#define MOS_LCR_1STOP       0x00

/**
 * @def MOS_LCR_2STOP
 * @brief Stop bits = 2 (bit 2 = 1).
 */
#define MOS_LCR_2STOP       0x04

/**
 * @def MOS_LCR_NO_PARITY
 * @brief No parity (bits 3-5 = 000) — STANDARD.
 */
#define MOS_LCR_NO_PARITY   0x00

/**
 * @def MOS_LCR_ODD_PARITY
 * @brief Odd parity (bits 3-5 = 001).
 */
#define MOS_LCR_ODD_PARITY  0x08

/**
 * @def MOS_LCR_EVEN_PARITY
 * @brief Even parity (bits 3-5 = 011).
 */
#define MOS_LCR_EVEN_PARITY 0x18

/**
 * @def MOS_LCR_DLAB
 * @brief Divisor Latch Access Bit (bit 7 = 1).
 *
 * @par Usage
 *      Set this bit to access baud rate divisor registers.
 *      Clear after setting baud rate to restore normal operation.
 */
#define MOS_LCR_DLAB        0x80

/**
 * @def MOS_SERIAL_DEFAULT
 * @brief Standard 8N1 configuration (8 data, No parity, 1 stop).
 *
 * @par Value
 *      0x03 (MOS_LCR_8BITS | MOS_LCR_1STOP | MOS_LCR_NO_PARITY)
 */
#define MOS_SERIAL_DEFAULT  (MOS_LCR_8BITS | MOS_LCR_1STOP | MOS_LCR_NO_PARITY)

/** @} */ /* end S.2d LCR */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2e BAUD — Baud Rate Divisors [BAUD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup  mos_defines_baud S.2e BAUD — Baud Rate Divisors
 * @ingroup   mos_serial_defines
 * @brief     Divisor values for standard baud rates.
 *
 * PURPOSE: Set baud rate via divisor latch registers.
 *
 * FORMULA: Divisor = 115200 / desired_baud
 *
 * WHY 115200 BASE:
 *   The 8250/16550 UART has a 1.8432 MHz oscillator divided by 16,
 *   giving 115200 as the base rate. Divisor = 1 gives 115200 baud.
 *
 * 115200 BAUD (divisor=1) IS DEFAULT:
 *   Maximum speed, compatible with all modern serial terminals.
 *   QEMU serial, minicom, screen all default to 115200.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @def MOS_BAUD_115200
 * @brief 115200 baud (divisor = 1) — MAXIMUM, DEFAULT.
 */
#define MOS_BAUD_115200     1

/**
 * @def MOS_BAUD_57600
 * @brief 57600 baud (divisor = 2) — Half speed.
 */
#define MOS_BAUD_57600      2

/**
 * @def MOS_BAUD_38400
 * @brief 38400 baud (divisor = 3).
 */
#define MOS_BAUD_38400      3

/**
 * @def MOS_BAUD_19200
 * @brief 19200 baud (divisor = 6).
 */
#define MOS_BAUD_19200      6

/**
 * @def MOS_BAUD_9600
 * @brief 9600 baud (divisor = 12) — Conservative.
 */
#define MOS_BAUD_9600       12

/**
 * @def MOS_BAUD_4800
 * @brief 4800 baud (divisor = 24) — Legacy.
 */
#define MOS_BAUD_4800       24

/**
 * @def MOS_BAUD_2400
 * @brief 2400 baud (divisor = 48) — Very slow.
 */
#define MOS_BAUD_2400       48

/**
 * @def MOS_BAUD_1200
 * @brief 1200 baud (divisor = 96) — Modem era.
 */
#define MOS_BAUD_1200       96

/** @} */ /* end S.2e BAUD */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_serial_setup_types S.3 TYPES — Type Declarations
 * @{
 *
 * [Reserved: No custom types in this header — uses uint8_t, uint16_t from stdint.h]
 *
 * @} end S.3 TYPES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOS — Function Prototypes [PROTOS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_serial_protos S.4 PROTOS — Function Prototypes
 * @{
 *
 * [Reserved: Inline functions defined in BODY B.2-B.3 — no separate prototypes]
 *
 * @} end S.4 PROTOS
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 EXTERN — External Declarations [EXTERN]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup  mos_serial_extern S.5 EXTERN — External Declarations
 * @{
 *
 * [Reserved: No mutable state in hardware interface headers]
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
/// @brief The actual work — declarations (headers), implementations (sources).
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART    — Structure overview, flow, counts
///   - B.2 HELPERS      — Foundation functions (port I/O primitives)
///   - B.3 CORE OPS     — Serial initialization logic
///   - B.4 ERRORS       — [Reserved: Hardware header — error handling in callers]
///   - B.5 PUBLIC API   — Exported interface (init, aliases)
///   - B.6 OMISSION     — What is reserved and why
///
/// Section order: Org Chart → Helpers → Core Ops → Errors → Public API → Omission
/// Flow: map structure → foundations → business logic → error handling → exported interface
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @defgroup mos_serial_body BODY — Serial Port Functions
 * @ingroup mos_serial
 * @{ */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Structure Overview [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_org_chart B.1 ORG CHART — Structure Overview
 * @ingroup mos_serial_body
 * @brief   Map structure — interface (headers), internals (sources).
 *
 * WHY: Understanding the file structure before diving into details.
 *      Org chart provides the mental model for navigation.
 *
 * SCOPE: File-level organization, implementation location, interface counts.
 *
 * CONTAINS:
 *   - B.1a Structure — Types, functions, externs count
 *   - B.1b Flow      — Implementation location
 *   - B.1c Counts    — Summary statistics
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1a STRUCTURE — Interface Overview [STRUCTURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_structure B.1a STRUCTURE — Interface Overview
 * @ingroup mos_serial_org_chart
 * @brief    Types, functions, and externs declared in this header.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      0 (uses stdint.h)
 * Functions:  4 inline (mos_outb, mos_inb, mos_io_wait, mos_serial_init)
 * Constants:  35+ defines (addresses, registers, bits, baud)
 * Externs:    0 */

/** @} */ /* end B.1a STRUCTURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1b FLOW — Implementation Location [FLOW]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_flow B.1b FLOW — Implementation Location
 * @ingroup mos_serial_org_chart
 * @brief    Where implementations live.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Implementation: All inline in this header (no .c file)
 * Why:            Hardware I/O must be inline for performance
 * Assembly Mirror: boot.asm, stage0a.asm have equivalent routines */

/** @} */ /* end B.1b FLOW */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.1c COUNTS — Summary Statistics [COUNTS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_counts B.1c COUNTS — Summary Statistics
 * @ingroup mos_serial_org_chart
 * @brief    Interface element counts.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* Types:      0
 * Functions:  4 inline
 * Constants:  35+ defines
 * Externs:    0 */

/** @} */ /* end B.1c COUNTS */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Foundation Functions [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_helpers B.2 HELPERS — Foundation Functions
 * @ingroup mos_serial_body
 * @brief    Foundation functions — building blocks for higher operations.
 *
 * WHY: Separate helper functions from public API. Helpers are the building
 *      blocks that public API functions compose.
 *
 * SCOPE: Low-level x86 I/O port access primitives.
 *
 * CONTAINS:
 *   - B.2a PURE       — Side-effect-free transformations
 *   - B.2b UTILITIES  — I/O port operations (mos_outb, mos_inb, mos_io_wait)
 *
 * Note: These are inline for performance — hardware I/O cannot have call overhead.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a PURE — Side-Effect-Free Functions [PURE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_pure B.2a PURE — Side-Effect-Free Functions
 * @ingroup mos_serial_helpers
 * @brief    Pure functions — same input always produces same output.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/* [Reserved: I/O port operations inherently have side effects] */

/** @} */ /* end B.2a PURE */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b UTILITIES — Common Operations [UTILITIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_utilities B.2b UTILITIES — Common Operations
 * @ingroup mos_serial_helpers
 * @brief    Utility operations — x86 I/O port access primitives.
 *
 * WHY INLINE:
 *   Function call overhead would be significant in tight TX loops.
 *   Inline lets compiler emit the outb/inb directly at call site.
 *
 * WHY GUARDED:
 *   hal.h also defines these functions for kernel use. The guard prevents
 *   redefinition errors when both headers are included.
 *
 * CONSTRAINTS:
 *   - "a" constraint: value must be in AL register (x86 port I/O requirement)
 *   - "Nd" constraint: port can be immediate or in DX register
 *   - volatile: prevents compiler from optimizing away I/O operations
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#ifndef MOS_PORT_IO_DEFINED
#define MOS_PORT_IO_DEFINED

/**
 * @brief Write byte to I/O port.
 *
 * @param port  I/O port address (0x0000-0xFFFF)
 * @param val   Byte value to write
 *
 * @par Assembly
 *      OUT DX, AL — write byte from AL to port in DX
 *
 * @par Example
 *      mos_outb(MOS_COM1_DATA, 'H');  // Send 'H' to serial port
 */
static inline void mos_outb(uint16_t port, uint8_t val) {
    __asm__ volatile(
        "outb %0, %1"                       /* OUT AL, DX — write byte to port */
        :                                   /* No outputs */
        : "a"(val),                         /* Input: val in AL register */
          "Nd"(port)                        /* Input: port in DX or immediate */
    );
}

/**
 * @brief Read byte from I/O port.
 *
 * @param port  I/O port address (0x0000-0xFFFF)
 * @return      Byte value read from port
 *
 * @par Assembly
 *      IN AL, DX — read byte from port in DX into AL
 *
 * @par Example
 *      uint8_t status = mos_inb(MOS_COM1_STATUS);
 *      if (status & MOS_LSR_TX_EMPTY) { ... }
 */
static inline uint8_t mos_inb(uint16_t port) {
    uint8_t ret;                            /* Return value buffer */
    __asm__ volatile(
        "inb %1, %0"                        /* IN AL, DX — read byte from port */
        : "=a"(ret)                         /* Output: ret from AL register */
        : "Nd"(port)                        /* Input: port in DX or immediate */
    );
    return ret;                             /* Return the read value */
}

/**
 * @brief I/O delay via write to unused port.
 *
 * @par Purpose
 *      Provide small delay for slow I/O devices.
 *
 * @par Port
 *      0x80 (POST diagnostic port) — write has no effect, just delays
 *
 * @par Why Port 0x80
 *      Port 0x80 is the POST diagnostic port — writing to it has no effect
 *      but provides a small delay (~1us) for slow I/O devices.
 */
static inline void mos_io_wait(void) {
    mos_outb(0x80, 0);                      /* Write 0 to POST port (no effect, just delay) */
}

#endif /* MOS_PORT_IO_DEFINED */

/** @} */ /* end B.2b UTILITIES */

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Component-Specific Functionality [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_core_ops B.3 CORE OPS — Component-Specific Functionality
 * @ingroup mos_serial_body
 * @brief    Business logic organized by operational category.
 *
 * WHY: Separate core operations from helpers (B.2) and public API (B.5).
 *      Operations implement the domain logic; API orchestrates them.
 *
 * SCOPE: Serial port initialization — the core functionality.
 *
 * CONTAINS:
 *   - B.3a INIT — Serial port initialization sequence
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a INIT — Serial Port Initialization [INIT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_init B.3a INIT — Serial Port Initialization
 * @ingroup mos_serial_core_ops
 * @brief    Initialize COM1 for 115200 8N1 operation.
 *
 * WHY THIS SEQUENCE:
 *   1. Disable interrupts first — we use polling mode in boot
 *   2. Enable DLAB to access baud rate divisor
 *   3. Set divisor to 1 (115200 baud)
 *   4. Set 8N1 format (clears DLAB)
 *   5. Enable FIFO for better throughput
 *   6. Set modem control for normal operation
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize COM1 for 115200 8N1 operation.
 *
 * @par Sequence
 *      1. Disable all interrupts (IER = 0)
 *      2. Set DLAB=1 to access divisor registers
 *      3. Set baud rate divisor = 1 (115200 baud)
 *      4. Set line format 8N1, clear DLAB
 *      5. Enable FIFO with 14-byte threshold
 *      6. Set DTR, RTS, OUT2 for normal operation
 *
 * @par Example
 *      mos_serial_init();  // Call once at boot
 *      // Now ready to send/receive on COM1
 */
static inline void mos_serial_init(void) {
    /* ─────────────────────────────────────────────────────────────────────────
     * Step 1: Disable all UART interrupts
     * ─────────────────────────────────────────────────────────────────────────
     * We use polling mode — no interrupt handler needed during boot.
     */
    mos_outb(MOS_COM1_INT, 0x00);           /* IER = 0x00 — all interrupts disabled */

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 2: Enable DLAB (Divisor Latch Access Bit)
     * ─────────────────────────────────────────────────────────────────────────
     * While DLAB=1, ports +0 and +1 become the baud rate divisor.
     */
    mos_outb(MOS_COM1_LCR, MOS_LCR_DLAB);   /* LCR = 0x80 — enable divisor access */

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 3: Set baud rate divisor
     * ─────────────────────────────────────────────────────────────────────────
     * Divisor = 1 gives 115200 baud (115200 / 1 = 115200).
     */
    mos_outb(MOS_COM1_DATA, MOS_BAUD_115200);  /* Divisor low byte = 1 */
    mos_outb(MOS_COM1_INT, 0x00);              /* Divisor high byte = 0 */

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 4: Set line format (8N1) and clear DLAB
     * ─────────────────────────────────────────────────────────────────────────
     * 8 data bits, no parity, 1 stop bit — the universal standard.
     */
    mos_outb(MOS_COM1_LCR, MOS_SERIAL_DEFAULT);  /* LCR = 0x03 — 8N1, DLAB=0 */

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 5: Enable and configure FIFO
     * ─────────────────────────────────────────────────────────────────────────
     * 0xC7 = Enable FIFO (bit 0), Clear RX FIFO (bit 1), Clear TX FIFO (bit 2),
     *        14-byte trigger level (bits 6-7 = 11).
     */
    mos_outb(MOS_COM1_FIFO, 0xC7);          /* FCR = 0xC7 — FIFO on, 14-byte threshold */

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 6: Set modem control for normal operation
     * ─────────────────────────────────────────────────────────────────────────
     * 0x0B = DTR (bit 0) + RTS (bit 1) + OUT2 (bit 3).
     * OUT2 is required on some systems for proper operation.
     */
    mos_outb(MOS_COM1_MCR, 0x0B);           /* MCR = 0x0B — DTR, RTS, OUT2 enabled */
}

/** @} */ /* end B.3a INIT */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Error Handling [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_errors B.4 ERRORS — Error Handling
 * @ingroup mos_serial_body
 * @brief    Error handling — query declarations (headers), management patterns (sources).
 *
 * WHY: Centralize error handling patterns for consistent behavior across
 *      the component. Separate from core operations for clarity.
 *
 * SCOPE: [Reserved: Hardware header — error handling in callers]
 *
 * CONTAINS:
 *   - B.4a QUERIES   — Error state queries and messages
 *   - B.4b RECOVERY  — Functions to restore valid state
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.4a QUERIES — Error State Queries [QUERIES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_error_queries B.4a QUERIES — Error State Queries
 * @ingroup mos_serial_errors
 * @{
 *
 * [Reserved: Hardware errors detectable via LSR bits — callers check MOS_LSR_*]
 *
 * @} end B.4a QUERIES
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.4b RECOVERY — State Recovery [RECOVERY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_error_recovery B.4b RECOVERY — State Recovery
 * @ingroup mos_serial_errors
 * @{
 *
 * [Reserved: Recovery = call mos_serial_init() again to reset UART]
 *
 * @} end B.4b RECOVERY
 */

/** @} */ /* end B.4 ERRORS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — Exported Interface [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_api B.5 PUBLIC API — Exported Interface
 * @ingroup mos_serial_body
 * @brief    Public API — declarations (headers), implementations (sources).
 *
 * WHY: Separate public interface from internal helpers (B.2) and core
 *      operations (B.3). Public APIs are the "top rungs of the ladder" —
 *      they orchestrate proven pieces.
 *
 * SCOPE: Lifecycle (init), utilities (legacy aliases).
 *
 * CONTAINS:
 *   - B.5a LIFECYCLE   — mos_serial_init (defined in B.3a, exposed here)
 *   - B.5b OPERATIONS  — [Reserved: Future operations]
 *   - B.5c HEALTH      — [Reserved: No health tracking at this level]
 *   - B.5d UTILITIES   — Legacy compatibility aliases
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5a LIFECYCLE — Creation/Initialization/Destruction [LIFECYCLE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_lifecycle B.5a LIFECYCLE — Creation/Initialization/Destruction
 * @ingroup mos_serial_api
 * @{
 *
 * Primary lifecycle function: mos_serial_init() — defined in B.3a INIT
 *
 * @see mos_serial_init
 *
 * @} end B.5a LIFECYCLE
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5b OPERATIONS — Primary Functionality [OPS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_operations B.5b OPERATIONS — Primary Functionality
 * @ingroup mos_serial_api
 * @{
 *
 * [Reserved: Higher-level operations defined in debug.h]
 *
 * @} end B.5b OPERATIONS
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5c HEALTH — Health Queries [HEALTH]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_health B.5c HEALTH — Health Queries
 * @ingroup mos_serial_api
 * @{
 *
 * [Reserved: Health tracking at Witness level, not hardware level]
 *
 * @} end B.5c HEALTH
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.5d UTILITIES — Consumer Helpers [UTIL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup mos_serial_api_utilities B.5d UTILITIES — Consumer Helpers
 * @ingroup mos_serial_api
 * @brief    Legacy compatibility aliases.
 *
 * PURPOSE: Allow gradual migration from old naming convention.
 *          New code should use MOS_* prefixed names.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#define COM1_PORT           MOS_COM1_DATA   /**< Legacy: COM1 data port */
#define COM1_DATA           MOS_COM1_DATA   /**< Legacy: COM1 data register */
#define COM1_STATUS         MOS_COM1_STATUS /**< Legacy: COM1 status register */

/** @} */ /* end B.5d UTILITIES */

/** @} */ /* end B.5 PUBLIC API */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_omission B.6 OMISSION — Reserved Section Guide
 * @ingroup mos_serial_body
 * @brief    What can be reserved and why — guidance for headers and sources.
 *
 * WHY: All sections must be present for structural consistency.
 *      Empty sections use: [Reserved: reason]
 *
 * SCOPE: BODY section reservation guidelines.
 *
 * NOT IN THIS HEADER:
 *   - mos_serial_char()  — Defined in debug.h (higher-level with wait loop)
 *   - mos_serial_print() — Defined in debug.h (string output)
 *   - mos_serial_recv()  — Planned for bidirectional debug
 *   - Error detection    — Future: check for framing/parity errors
 *   - Interrupt mode     — Not needed for boot diagnostics
 *
 * WHY SEPARATE:
 *   This header provides LOW-LEVEL constants and primitives.
 *   Higher-level functions belong in debug.h to maintain separation
 *   between hardware interface and debug API.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/* [Reserved: B.6 is documentation-only — no code content] */

/** @} */ /* end B.6 OMISSION */

/** @} */ /* end mos_serial_body */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING BLOCK [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Operational guidance and closing notes.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - X.1 VALIDATION      — Verify correctness (build, checks, tests)
///   - X.2 EXECUTION       — Usage patterns and examples
///   - X.3 CLEANUP         — Ownership and memory patterns
///   - X.4 POLICY          — Guide future maintainers
///   - X.5 EXTENSION       — How to expand this component
///   - X.6 TROUBLESHOOTING — Common problems and solutions
///   - X.7 REFERENCE       — Quick lookup (copy-paste ready)
///   - X.8 NOTE            — Final guidance and grounding
///   - X.9 OMISSION        — What can be reserved and why
///
/// Section order: Validation → Execution → Cleanup | Policy → Extension →
///                Troubleshooting → Reference → Note → Omission
/// Flow: verify → run → release | modify → extend → debug → lookup → ground
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Verify Correctness [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_validation X.1 VALIDATION — Verify Correctness
 * @ingroup mos_serial
 * @brief    Verify correctness — headers (syntax, self-containment).
 *
 * CONTAINS:
 *   - X.1a BUILD     — Compilation commands
 *   - X.1b CHECKS    — Correctness verification
 *   - X.1c SELF_TEST — Self-containment test
 *   - X.1d MEMORY    — [N/A for header-only]
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.1a BUILD — Compilation Commands [BUILD]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @code{.sh}
 * gcc -ffreestanding -fsyntax-only -Wall -Wextra serial.h    # syntax check
 * cppcheck --enable=all serial.h                              # static analysis
 * @endcode
 *
 * @{ */
/* [Build commands documented in defgroup above] */
/** @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.1b CHECKS — Correctness Verification [CHECKS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * VERIFICATION CHECKLIST:
 *   - Self-contained (includes without errors)
 *   - No missing type definitions
 *   - No circular include dependencies
 *   - Include guards work (no redefinition)
 *   - MOS_PORT_IO_DEFINED guard prevents HAL conflicts
 *
 * @{ */
/* [Verification checklist documented in defgroup above] */
/** @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.1c SELF_TEST — Self-Containment Test [SELF_TEST]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @code{.c}
 * // test.c — must compile without errors
 * #include "serial.h"
 * int main(void) {
 *     mos_serial_init();
 *     return 0;
 * }
 * @endcode
 *
 * @{ */
/* [Self-test documented in defgroup above] */
/** @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.1d MEMORY — Memory Verification [MEMORY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * [N/A: Header-only — no dynamic memory allocation]
 *
 * @{ */
/* [Memory verification N/A for header-only] */
/** @} */

/** @} */ /* end X.1 VALIDATION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Usage Patterns [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_execution X.2 EXECUTION — Usage Patterns
 * @ingroup mos_serial
 * @brief    Execution — usage patterns (headers), entry point and flow (sources).
 *
 * CONTAINS:
 *   - X.2a INCLUDE    — How to include this header
 *   - X.2b EXIT_CODES — [N/A for header]
 *   - X.2c USAGE      — Complete usage example
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.2a INCLUDE — How to Include [INCLUDE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @code{.c}
 * #include "serial.h"
 * @endcode
 *
 * @{ */
/* [Include pattern documented in defgroup above] */
/** @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.2b EXIT_CODES — Standard Exit Codes [EXIT_CODES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * [N/A: Header-only — no exit codes]
 *
 * @{ */
/* [Exit codes N/A for header-only] */
/** @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.2c USAGE — Complete Usage Example [USAGE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @code{.c}
 * #include "serial.h"
 *
 * void boot_early_init(void) {
 *     // Initialize serial port (call once)
 *     mos_serial_init();
 *
 *     // Output a character (wait for TX empty, then send)
 *     while ((mos_inb(MOS_COM1_STATUS) & MOS_LSR_TX_EMPTY) == 0);
 *     mos_outb(MOS_COM1_DATA, 'H');
 *
 *     while ((mos_inb(MOS_COM1_STATUS) & MOS_LSR_TX_EMPTY) == 0);
 *     mos_outb(MOS_COM1_DATA, 'i');
 *
 *     while ((mos_inb(MOS_COM1_STATUS) & MOS_LSR_TX_EMPTY) == 0);
 *     mos_outb(MOS_COM1_DATA, '\n');
 * }
 * @endcode
 *
 * @{ */
/* [Usage example documented in defgroup above] */
/** @} */

/** @} */ /* end X.2 EXECUTION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Ownership and Memory [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_cleanup X.3 CLEANUP — Ownership and Memory
 * @ingroup mos_serial
 * @brief    Cleanup — ownership patterns (headers), memory and shutdown (sources).
 *
 * CONTAINS:
 *   - X.3a OWNERSHIP — Who owns what
 *   - X.3b MEMORY    — Stack vs heap patterns
 *   - X.3c SHUTDOWN  — Graceful termination
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.3a OWNERSHIP — Who Owns What [OWNERSHIP]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * OWNERSHIP RULES:
 *   - No dynamic allocation — all inline, no ownership concerns
 *   - Hardware registers are global system resources
 *   - Serial port is shared — coordinate access at higher levels
 *
 * @{ */
/* [Ownership rules documented in defgroup above] */
/** @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.3b MEMORY — Stack vs Heap Patterns [MEMORY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * [N/A: Header-only — no dynamic memory]
 *
 * @{ */
/* [Memory patterns N/A for header-only] */
/** @} */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * X.3c SHUTDOWN — Graceful Termination [SHUTDOWN]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * SHUTDOWN: No explicit shutdown needed.
 * The UART continues operating; caller responsible for any cleanup.
 *
 * @{ */
/* [Shutdown documented in defgroup above] */
/** @} */

/** @} */ /* end X.3 CLEANUP */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidelines [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_policy X.4 POLICY — Modification Guidelines
 * @ingroup mos_serial
 * @brief    Guide future maintainers — what's safe to change.
 *
 * SAFE TO MODIFY:
 *   - Add new COM port initialization functions (COM2-COM4)
 *   - Add new baud rate divisors
 *   - Add receive functions
 *
 * MODIFY WITH CARE:
 *   - Existing register addresses — affects all consumers
 *   - LSR/LCR bit definitions — hardware-defined, rarely need changing
 *   - mos_serial_init sequence — affects boot reliability
 *
 * NEVER MODIFY:
 *   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
 *   - Include guard pattern (MOS_SERIAL_H)
 *   - MOS_PORT_IO_DEFINED guard (prevents HAL conflicts)
 *   - Hardware port addresses (fixed by PC/AT specification)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Policy documented in defgroup above] */
/** @} */ /* end X.4 POLICY */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_extension X.5 EXTENSION — How to Expand
 * @ingroup mos_serial
 * @brief    How to expand this component — mirrors M.10 Roadmap.
 *
 * ADDING FUNCTIONS:
 *   - Add inline function in B.2 HELPERS (if utility) or B.3 CORE OPS (if operation)
 *   - Update B.1 ORG CHART counts
 *   - Add usage example in X.2c USAGE
 *
 * PLANNED EXTENSIONS:
 *   - mos_serial_init_port(port, baud) — Initialize any COM port
 *   - mos_serial_recv() — Receive character (blocking)
 *   - mos_serial_recv_ready() — Check if data available
 *
 * @see M.10 ROADMAP for planned features.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Extension patterns documented in defgroup above] */
/** @} */ /* end X.5 EXTENSION */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_troubleshooting X.6 TROUBLESHOOTING — Common Problems
 * @ingroup mos_serial
 * @brief    Common problems and solutions — header and runtime issues.
 *
 * COMMON ERRORS:
 *
 * "Redefinition of mos_outb/mos_inb":
 *   → Both serial.h and hal.h included. MOS_PORT_IO_DEFINED guard should prevent.
 *   → Check include order and ensure guards are correct.
 *
 * "No serial output":
 *   → Call mos_serial_init() before any output
 *   → Check QEMU -serial stdio flag
 *   → Verify TX_EMPTY wait loop is present
 *
 * "Garbled serial output":
 *   → Baud rate mismatch — ensure both ends use 115200
 *   → Check 8N1 configuration matches receiver
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Troubleshooting documented in defgroup above] */
/** @} */ /* end X.6 TROUBLESHOOTING */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_reference X.7 REFERENCE — Quick Lookup
 * @ingroup mos_serial
 * @brief    Quick lookup — copy-paste ready examples.
 *
 * BASIC SETUP:
 * @code{.c}
 * #include "serial.h"
 * mos_serial_init();  // Call once at boot
 * @endcode
 *
 * SEND CHARACTER:
 * @code{.c}
 * while ((mos_inb(MOS_COM1_STATUS) & MOS_LSR_TX_EMPTY) == 0);
 * mos_outb(MOS_COM1_DATA, 'X');
 * @endcode
 *
 * CHECK ERRORS:
 * @code{.c}
 * uint8_t lsr = mos_inb(MOS_COM1_STATUS);
 * if (lsr & MOS_LSR_OVERRUN)    { ... }  // overrun error
 * if (lsr & MOS_LSR_PARITY_ERR) { ... }  // parity error
 * if (lsr & MOS_LSR_FRAME_ERR)  { ... }  // framing error
 * @endcode
 *
 * RELATED FILES:
 *   - debug.h      — Higher-level debug logging using serial output
 *   - bootinfo.h   — Boot info structure (includes serial state)
 *   - kernel/hal.h — Kernel HAL with serial driver
 *   - boot.asm     — serial_init, serial_char in assembly
 *   - stage0a.asm  — stage0 serial functions
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Reference examples documented in defgroup above] */
/** @} */ /* end X.7 REFERENCE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Final Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_note X.8 NOTE — Final Guidance
 * @ingroup mos_serial
 * @brief    Final guidance and grounding — echoes M.8.
 *
 * Serial port constants and I/O primitives for MillenniumOS boot chain.
 * This header is the SINGLE SOURCE OF TRUTH for serial port definitions.
 *
 * IMPORTANT: These are I/O port addresses, NOT memory addresses.
 * Use mos_inb/mos_outb inline functions, not memory dereference.
 *
 * @par Biblical Grounding
 * "A man hath joy by the answer of his mouth: and a word spoken in due season,
 * how good is it!" — Proverbs 15:23
 *
 * The serial port provides the faithful witness of system state — a word spoken
 * in due season. This is Kingdom technology at its foundation.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Final guidance documented in defgroup above] */
/** @} */ /* end X.8 NOTE */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup mos_serial_closing_omission X.9 OMISSION — Reserved Section Guide
 * @ingroup mos_serial
 * @brief    What can be reserved and why — guidance for CLOSING sections.
 *
 * SECTION STATUS:
 *   - X.1 Validation:      Active — build commands provided
 *   - X.2 Execution:       Active — usage examples provided
 *   - X.3 Cleanup:         Partial — no dynamic memory
 *   - X.4 Policy:          Active — modification guidelines provided
 *   - X.5 Extension:       Active — roadmap cross-referenced
 *   - X.6 Troubleshooting: Active — common errors documented
 *   - X.7 Reference:       Active — copy-paste examples provided
 *   - X.8 Closing Note:    Active — grounding provided
 *
 * KEY PRINCIPLE: CLOSING synthesizes, METADATA/SETUP/BODY contain details.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */
/* [Reserved: X.9 is documentation-only — no code content] */
/** @} */ /* end X.9 OMISSION */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════

#endif /* MOS_SERIAL_H */
