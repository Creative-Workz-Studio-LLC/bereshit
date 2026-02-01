// =============================================================================
// OMNICODE PRAGMA [PRAGMA]
// =============================================================================
//
// @legend
//   :req  REQUIRED   — must exist, validation fails without
//   :inh  INHERITED  — from template, override allowed
//   :ins  INSTANCE   — file-specific, unique values
// @endlegend
//
// -----------------------------------------------------------------------------
// P.1 CORE — Identity [CORE]
// -----------------------------------------------------------------------------
//
// @omni:req  key        = MOS-kernel-kernel-impl
// @omni:req  from       = bereshit/word/seed/code/c/source.c
// @omni:req  at         = document
//
// -----------------------------------------------------------------------------
// P.2 FAMILY — Classification [FAMILY]
// -----------------------------------------------------------------------------
//
// @omni:inh  type       = source
// @omni:inh  subtype    = implementation
// @omni:inh  role       = orchestrator
// @omni:inh  structure  = 4-block
//
// -----------------------------------------------------------------------------
// P.3 INSTANCE — File Details [INSTANCE]
// -----------------------------------------------------------------------------
//
// @omni:ins  component  = kernel
// @omni:ins  layer      = 0 (kernel layer)
// @omni:ins  cpisi_mode = 0 (freestanding)
// @omni:ins  provides   = [kernel_main, kernel entry point]
//
// -----------------------------------------------------------------------------
// P.4 ARCHITECTURE — Scaling Context [ARCH]
// -----------------------------------------------------------------------------
//
// @omni:ins  layers     = [0:kernel, 1:system, 2:runtime, 3:framework,
//                          4:health, 5:network, 6:storage, 7:engine, 8:app]
// @omni:ins  scale      = 3^n [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
// @omni:ins  pattern    = 4+1+4 [below:0-3, center:4, above:5-8]
// @omni:ins  anchor     = genesis_1_1 -> void(0) -> unity(1) -> derivation
//
// -----------------------------------------------------------------------------
// P.5 SUMMARY — Human Identity [SUMMARY]
// -----------------------------------------------------------------------------
//
// @omni:req  title      = MillenniumOS Kernel Core Implementation
// @omni:req  brief      = Main kernel entry point and orchestration
//
// =============================================================================
// END PRAGMA
// =============================================================================

// =============================================================================
// METADATA BLOCK [METADATA]
// =============================================================================
//
// @brief Identity and context for the kernel core.
//
// STRUCTURE: M.1-M.10 sections for complete documentation.
//
// =============================================================================

/**
 * =============================================================================
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * =============================================================================
 *
 * @file      kernel.c
 * @brief     MillenniumOS Kernel — Main entry and orchestration.
 *
 * Key:       MOS-kernel-kernel-impl
 * Title:     MillenniumOS Kernel Core Implementation
 * Type:      Source (Orchestrator)
 * Component: Kernel — Layer 0 (freestanding)
 * Role:      Main kernel entry point after boot handoff from entry.asm
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.2 STATE — Lifecycle State [STATE]
 * =============================================================================
 *
 * @version   a-01.50
 * @date      2026-01-31
 *
 * Status:    Active
 * Created:   2026-01-30
 * Updated:   2026-01-31
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
 * =============================================================================
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Implementation)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.4 LOCATION — File Position [LOCATION]
 * =============================================================================
 *
 * Path:      millenniumos/kernel/src/kernel.c
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.5 DERIVATION — Template Lineage [DERIVATION]
 * =============================================================================
 *
 * Derives:   bereshit/word/seed/code/c/source.c (4-block C template)
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
 * =============================================================================
 *
 * Tags:      kernel, boot, entry, orchestrator, freestanding
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.7 INTENT — Purpose Statement [INTENT]
 * =============================================================================
 *
 * Purpose:   Receive control from entry.asm, initialize kernel subsystems,
 *            and orchestrate the 7-stage Genesis boot sequence to bring the
 *            system to operational Sabbath rest.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * =============================================================================
 *
 * Scripture: Genesis 1:3 — "And God said, Let there be light: and there was light."
 * Principle: The kernel speaks, and systems come into being. From void to order.
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
 * =============================================================================
 *
 * Depends:
 *   - kernel.h      — Kernel types and declarations
 *   - memory.h      — Memory management subsystem
 *   - interrupt.h   — Interrupt handling
 *   - process.h     — Process management
 *   - vfs.h         — Virtual filesystem
 *   - cpisi.h       — CPI-SI integration
 *   - hal.h         — Hardware abstraction layer
 *   - graphics.h    — Graphics subsystem
 *   - serial.h      — Serial port constants
 *
 * Used by:
 *   - entry.asm     — Assembly entry point calls kernel_main()
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * M.10 ROADMAP — Version History [ROADMAP]
 * =============================================================================
 *
 * History:
 *   a-01.00 (2026-01-30) — Initial kernel with unified boot info
 *   a-01.50 (2026-01-31) — Full OmniCode 4-block template alignment
 *   a-02.00 (2026-01-31) — Shell integration, M.10 completion
 *
 * Completed:
 *   ✓ 7-stage Genesis boot sequence (Void→Light→Firmament→...→Sabbath)
 *   ✓ Boot info validation and forensics display
 *   ✓ Memory subsystem initialization (mos_memory_init)
 *   ✓ Interrupt subsystem initialization (mos_interrupt_init)
 *   ✓ Process subsystem initialization (mos_process_init)
 *   ✓ VFS initialization (mos_vfs_init)
 *   ✓ CPI-SI initialization (mos_cpisi_init)
 *   ✓ Diagnostic subsystem initialization (mos_diag_init)
 *   ✓ HAL initialization with PS/2 keyboard/mouse (mos_hal_init)
 *   ✓ Graphics initialization with GOP/VESA framebuffer (mos_graphics_init)
 *   ✓ UI framework initialization (mos_ui_init)
 *   ✓ DAR Witness system with boot event import
 *   ✓ Early event buffer for pre-witness logging
 *   ✓ Installer detection and launch
 *   ✓ Serial port debug output throughout boot
 *
 * Planned:
 *   ⏳ Shell service integration with kernel main loop
 *   ⏳ UEFI Runtime Services stub (get_time, set_variable)
 *   ⏳ Persistent boot log writer service
 *   ⏳ Dynamic memory allocator enhancements
 *   ⏳ Multi-process scheduler activation
 *   ⏳ Network stack initialization
 *
 * =============================================================================
 */

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP BLOCK [SETUP]
// =============================================================================
//
// @brief Configuration and declarations before body.
//
// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
//
// CONTAINS:
//   - S.1 INCLUDES  — Header dependencies
//   - S.2 DEFINES   — Constants and macros
//   - S.3 TYPES     — Type declarations
//   - S.4 PROTOS    — Static function prototypes
//   - S.5 STATIC    — Static/file-scope variables
//   - S.6 OMISSION  — Reserved section guide
//
// =============================================================================

/**
 * =============================================================================
 * S.1 INCLUDES — Header Dependencies [INCLUDES]
 * =============================================================================
 *
 * @brief Standard, internal, and external library includes.
 *
 * NOTE: kernel.h MUST be first (sets CPISI_MODE for freestanding build).
 * NOTE: DO NOT include full trit/trit.h - it conflicts with cornerstone types.
 *
 * =============================================================================
 */

/**
 * -----------------------------------------------------------------------------
 * S.1a PROJECT — Project Headers [PROJECT]
 * -----------------------------------------------------------------------------
 *
 * @brief Kernel and subsystem headers (CPISI_MODE=0 freestanding).
 *
 * -----------------------------------------------------------------------------
 */

#include "kernel.h"               /* Kernel types — MUST BE FIRST (sets CPISI_MODE) */

/* Kernel subsystems */
#include "memory/memory.h"        /* Memory management subsystem */
#include "interrupt/interrupt.h"  /* Interrupt handling */
#include "process/process.h"      /* Process management */
#include "vfs/vfs.h"              /* Virtual filesystem */
#include "cpisi/cpisi.h"          /* CPI-SI integration */
#include "diagnostic/diagnostic.h"/* Diagnostic probes */
#include "hal/hal.h"              /* Hardware abstraction layer */
#include "graphics/graphics.h"    /* Graphics subsystem */
#include "ui/ui.h"                /* UI framework */
#include "installer/installer.h"  /* Installer service */
#include "shell.h"                /* Terminal shell service */

/* libtrit math operations (avoid full trit.h which has HebrewState conflict) */
#include "trit/matter/math.op.h"  /* For trit_result_t and trit_hsub */

/* Shared serial port constants (single source of truth) */
#include "boot/include/serial.h"  /* COM1 base address and constants */

/**
 * =============================================================================
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * =============================================================================
 *
 * @brief Constants and macros for kernel operations.
 *
 * =============================================================================
 */

/**
 * -----------------------------------------------------------------------------
 * S.2a SIZES — Buffer and Array Sizes [SIZES]
 * -----------------------------------------------------------------------------
 */

#define KERNEL_CHAR_WIDTH    8    /**< Character cell width in pixels */
#define KERNEL_CHAR_HEIGHT   16   /**< Character cell height in pixels */

/**
 * -----------------------------------------------------------------------------
 * S.2b LIMITS — Value Bounds [LIMITS]
 * -----------------------------------------------------------------------------
 */

#define KERNEL_FB_MIN_ADDR   0x100000  /**< Min valid framebuffer address (1MB) */

/**
 * -----------------------------------------------------------------------------
 * S.2c FEATURES — Boot Feature Flags [FEATURES]
 * -----------------------------------------------------------------------------
 *
 * @brief Feature flags populated during kernel_early_init().
 *
 * Use mos_has_boot_feature(flag) to check availability.
 */

#define MOS_FEAT_ACPI        0x01  /**< ACPI RSDP provided by firmware */
#define MOS_FEAT_HIMEM_FB    0x02  /**< High memory framebuffer (>4GB, UEFI typical) */
#define MOS_FEAT_FRAMEBUFFER 0x04  /**< Valid framebuffer available */
#define MOS_FEAT_MEMMAP      0x08  /**< Memory map available */
#define MOS_FEAT_RUNTIME     0x10  /**< UEFI runtime services (future) */

/**
 * =============================================================================
 * S.3 TYPES — Type Declarations [TYPES]
 * =============================================================================
 *
 * @brief File-local type definitions.
 *
 * NOTE: Main types defined in kernel.h. This section for internal types only.
 *
 * =============================================================================
 */

/* [Reserved: Internal types defined as needed in BODY sections] */

/**
 * =============================================================================
 * S.4 PROTOS — Static Function Prototypes [PROTOS]
 * =============================================================================
 *
 * @brief Forward declarations for internal static functions.
 *
 * =============================================================================
 */

/**
 * -----------------------------------------------------------------------------
 * S.4a INIT — Initialization Functions [INIT]
 * -----------------------------------------------------------------------------
 */

static mos_result_t kernel_early_init(const MosBootInfo* boot);
static void kernel_print_banner(void);
static void kernel_enter_sabbath(void);

/**
 * -----------------------------------------------------------------------------
 * S.4b SERIAL — Serial Output Functions [SERIAL]
 * -----------------------------------------------------------------------------
 */

static void serial_init(void);
void serial_putchar(char c);      /* Non-static: used by shell */
static void serial_puts(const char* s);
static void serial_hex(uint64_t value);

/**
 * -----------------------------------------------------------------------------
 * S.4c CONSOLE — Console Output Functions [CONSOLE]
 * -----------------------------------------------------------------------------
 */

static void console_putchar_boot(const MosBootInfo* boot, char c);
static void console_puts(const MosBootInfo* boot, const char* s);

/**
 * =============================================================================
 * S.5 STATIC — File-Scope Variables [STATIC]
 * =============================================================================
 *
 * @brief File-level state declarations.
 *
 * NOTE: Console state and kernel info are file-scope for boot simplicity.
 *
 * =============================================================================
 */

/* [Defined in BODY B.1 and B.2 for clarity with their sections] */

/**
 * =============================================================================
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * =============================================================================
 *
 * @brief Documents intentionally minimal sections.
 *
 * RESERVED SECTIONS:
 *   - S.1b STANDARD  — No standard includes (freestanding kernel)
 *   - S.1c EXTERNAL  — No external dependencies
 *   - S.3 TYPES      — Types defined in kernel.h
 *
 * =============================================================================
 */

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY BLOCK [BODY]
// =============================================================================
//
// @brief The actual kernel implementation — Genesis boot sequence.
//
// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
//
// CONTAINS:
//   - B.1 STATE      — Kernel global state
//   - B.2 CONSOLE    — Early console output (serial + framebuffer)
//   - B.3 BANNER     — Kernel banner display
//   - B.4 EARLY      — Early kernel initialization
//   - B.5 CPISI      — CPI-SI message helper
//   - B.6 SERVICES   — Kernel service accessors
//   - B.7 PANIC      — Panic and halt handlers
//   - B.8 SECURITY   — Stack protection (GCC canary)
//   - B.9 ZONE       — Zone permission operations
//   - B.10 SABBATH   — Enter operational rest
//   - B.11 MAIN      — Main kernel entry point (kernel_main)
//
// =============================================================================

/**
 * =============================================================================
 * B.1 STATE — Kernel Global State [STATE]
 * =============================================================================
 *
 * @defgroup kernel_state B.1 STATE — Kernel Global State
 * @brief    Single instance of kernel info, initialized during boot.
 *
 * All fields use ternary types from libtrit:
 *   - version_stage: trit5 stage (a=-1, b=0, c=+1)
 *   - health: -3 (critical) to +3 (optimal)
 *   - hebrew_state: shavar→yashar→tov progression
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Global kernel state instance.
 *
 * Initialized at compile time with default values.
 * Updated throughout boot and runtime.
 *
 * @note Static linkage — accessed via mos_kernel_info().
 */
static MosKernelInfo g_kernel = {
    .version_stage  = MOS_VERSION_STAGE,
    .version_phase  = MOS_VERSION_PHASE,
    .version_comp   = MOS_VERSION_COMP,
    .version_string = MOS_VERSION_STRING,
    .codename       = MOS_CODENAME,
    .state          = MOS_KERNEL_BOOTING,
    .stage          = MOS_KERNEL_STAGE0,
    .hebrew_state   = MOS_STATE_YASHAR,    // Start centered
    .health         = MOS_HEALTH_NEUTRAL,
    .boot_info      = NULL,
    .boot_time      = 0,
    .uptime         = 0,
    .context_switches = 0,
    .syscalls       = 0,
    .interrupts     = 0,
};

/**
 * @brief Boot path context — tracks boot source and available features.
 *
 * Populated during kernel_early_init(), used throughout init stages.
 *
 * Boot path (ternary):
 *   - +1 (TRIT_POS): UEFI/EDK2 — full 64-bit, GOP, ACPI provided
 *   - 0 (TRIT_ZERO): Unknown — minimal mode, no assumptions
 *   - -1 (TRIT_NEG): BIOS — legacy compat, VESA, may need ACPI search
 *
 * Feature flags:
 *   - 0x01: ACPI RSDP provided by firmware
 *   - 0x02: High memory framebuffer (>4GB, typical UEFI)
 *   - 0x04: Valid framebuffer (any address)
 *   - 0x08: Memory map available
 *   - 0x10: Runtime services available (UEFI only)
 */
typedef struct MosBootContext {
    trit_t      path;           /**< @brief Boot path: +1=UEFI, 0=unknown, -1=BIOS */
    uint32_t    features;       /**< @brief Feature flags detected */
    uint32_t    boot_events;    /**< @brief Number of boot events from bootloader */
    uint16_t    last_phase;     /**< @brief Last boot phase code */
    uint16_t    last_status;    /**< @brief Last boot phase status (0=OK, 1=WARN, 0xFFFF=FAIL) */
} MosBootContext;

/** @brief Global boot context — set during early init. */
static MosBootContext g_boot_ctx = {
    .path        = TRIT_ZERO,
    .features    = 0,
    .boot_events = 0,
    .last_phase  = 0,
    .last_status = 0,
};

/** @} */ /* end B.1 STATE */

/**
 * =============================================================================
 * B.2 CONSOLE — Early Console Output [CONSOLE]
 * =============================================================================
 *
 * @defgroup kernel_console B.2 CONSOLE — Early Console Output
 * @brief    Simple output before memory management is available.
 *
 * Uses direct framebuffer writes from bootinfo and serial COM1.
 * Serial ALWAYS works, even if graphics fail — essential for debugging.
 *
 * =============================================================================
 * @{
 */

/** @brief Current console X position (character cells). */
static uint32_t console_x = 0;

/** @brief Current console Y position (pixels from top). */
static uint32_t console_y = 0;

/**
 * -----------------------------------------------------------------------------
 * B.2a SERIAL — Serial Console (COM1) [SERIAL]
 * -----------------------------------------------------------------------------
 *
 * @defgroup kernel_serial B.2a SERIAL — Serial Console
 * @ingroup kernel_console
 * @brief    COM1 serial output — always works, even if graphics fail.
 *
 * COM1 constants come from serial.h (single source of truth).
 *
 * -----------------------------------------------------------------------------
 * @{
 */

/**
 * @brief Write byte to I/O port.
 *
 * @param[in] port  I/O port address.
 * @param[in] val   Value to write.
 */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * @brief Read byte from I/O port.
 *
 * @param[in] port  I/O port address.
 *
 * @return Byte read from port.
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * @brief Initialize COM1 serial port.
 *
 * Configures COM1 for 115200 baud, 8N1 (8 bits, no parity, 1 stop bit).
 * Enables FIFO and IRQs.
 *
 * @note Must be called before any serial output.
 * @note 115200 matches QEMU and OVMF expectations.
 */
static void serial_init(void) {
    outb(MOS_COM1_BASE + 1, 0x00);    /* Disable interrupts */
    outb(MOS_COM1_BASE + 3, 0x80);    /* Enable DLAB */
    outb(MOS_COM1_BASE + 0, 0x01);    /* 115200 baud (divisor=1) */
    outb(MOS_COM1_BASE + 1, 0x00);    /* (divisor high) */
    outb(MOS_COM1_BASE + 3, 0x03);    /* 8 bits, no parity, one stop bit */
    outb(MOS_COM1_BASE + 2, 0xC7);    /* Enable FIFO */
    outb(MOS_COM1_BASE + 4, 0x0B);    /* IRQs enabled, RTS/DSR set */
}

/**
 * @brief Send single character to COM1.
 *
 * @param[in] c  Character to send.
 *
 * @note Blocks until transmit buffer is ready.
 * @note Non-static: used by shell for output.
 */
void serial_putchar(char c) {
    while ((inb(MOS_COM1_BASE + 5) & 0x20) == 0);  /* Wait for transmit ready */
    outb(MOS_COM1_BASE, c);
}

/**
 * @brief Send null-terminated string to COM1.
 *
 * @param[in] s  String to send.
 *
 * @note Converts LF to CR+LF for proper terminal display.
 */
static void serial_puts(const char* s) {
    while (*s) {
        if (*s == '\n') serial_putchar('\r');
        serial_putchar(*s++);
    }
}

/**
 * @brief Print 64-bit hex value to serial.
 *
 * @param[in] value  Value to print in hexadecimal.
 *
 * Outputs "0x" prefix followed by 16 hex digits.
 */
static void serial_hex(uint64_t value) {
    static const char hex[] = "0123456789ABCDEF";
    serial_puts("0x");
    for (int i = 15; i >= 0; i--) {
        serial_putchar(hex[(value >> (i * 4)) & 0xF]);
    }
}

/** @} */ /* end B.2a SERIAL */

/**
 * -----------------------------------------------------------------------------
 * B.2b FRAMEBUFFER — Framebuffer Console [FRAMEBUFFER]
 * -----------------------------------------------------------------------------
 *
 * @defgroup kernel_fb B.2b FRAMEBUFFER — Framebuffer Console
 * @ingroup kernel_console
 * @brief    Direct framebuffer output for graphical display.
 *
 * Uses MosFramebuffer from unified MosBootInfo structure.
 * Works with both UEFI (64-bit GOP) and BIOS (32-bit VESA zero-extended).
 *
 * -----------------------------------------------------------------------------
 * @{
 */

/**
 * @brief Output single character to boot console.
 *
 * @param[in] boot  Boot info with framebuffer configuration.
 * @param[in] c     Character to output.
 *
 * Always outputs to serial (debugging fallback).
 * Outputs to framebuffer if valid address provided.
 *
 * @note Simple 8x16 character cells. Full font requires initrd.
 */
static void console_putchar_boot(const MosBootInfo* boot, char c) {
    // Always output to serial for debugging
    if (c == '\n') serial_putchar('\r');
    serial_putchar(c);

    // Skip framebuffer if invalid
    if (!boot || !boot->framebuffer.address) return;

    // Validate framebuffer address is in reasonable range
    // - Must be above 1MB (0x100000) to avoid low memory regions
    // - Text mode fallback at 0xB8000 is handled elsewhere
    // - NO upper limit: UEFI GOP can place framebuffer above 4GB
    uint64_t fb_addr = boot->framebuffer.address;
    if (fb_addr < 0x100000) return;

    const MosFramebuffer* fb = &boot->framebuffer;

    // Simple 8x16 character cell
    const uint32_t char_width = 8;
    const uint32_t char_height = 16;

    if (c == '\n') {
        console_x = 0;
        console_y += char_height;
        return;
    }

    if (c == '\r') {
        console_x = 0;
        return;
    }

    // Wrap at screen edge
    if (console_x + char_width > fb->width) {
        console_x = 0;
        console_y += char_height;
    }

    // Scroll if needed (simple wrap for now)
    if (console_y + char_height > fb->height) {
        console_y = 0;  // Wrap to top (proper scrolling needs memory)
    }

    // Draw character placeholder (white rectangle for now)
    // Full font rendering requires font data loaded from initrd
    // Note: framebuffer.address is 64-bit (works for all systems)
    uint32_t* pixel = (uint32_t*)((uint8_t*)(uintptr_t)fb->address +
                                   console_y * fb->pitch +
                                   console_x * (fb->bpp / 8));

    for (uint32_t y = 0; y < char_height && (console_y + y) < fb->height; y++) {
        for (uint32_t x = 0; x < char_width && (console_x + x) < fb->width; x++) {
            // Simple: draw white for non-space, nothing for space
            if (c != ' ') {
                pixel[x] = 0xFFFFFFFF;  // White
            }
        }
        pixel = (uint32_t*)((uint8_t*)pixel + fb->pitch);
    }

    console_x += char_width;
}

/**
 * @brief Output null-terminated string to boot console.
 *
 * @param[in] boot  Boot info with framebuffer configuration.
 * @param[in] s     String to output.
 *
 * @note Outputs nothing if boot or s is NULL.
 */
static void console_puts(const MosBootInfo* boot, const char* s) {
    if (!boot || !s) return;
    while (*s) {
        console_putchar_boot(boot, *s++);
    }
}

/** @} */ /* end B.2b FRAMEBUFFER */

/** @} */ /* end B.2 CONSOLE */

/**
 * =============================================================================
 * B.3 BANNER — Kernel Banner Display [BANNER]
 * =============================================================================
 *
 * @defgroup kernel_banner B.3 BANNER — Kernel Banner Display
 * @brief    ASCII art banner and identification.
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Print MillenniumOS ASCII banner to console.
 *
 * Displays the MOS logo and Genesis 1:1 grounding.
 *
 * @note Uses g_kernel.boot_info for console output.
 */
static void kernel_print_banner(void) {
    const MosBootInfo* boot = g_kernel.boot_info;
    console_puts(boot, "\n");
    console_puts(boot, "  __  __  ___  ____\n");
    console_puts(boot, " |  \\/  |/ _ \\/ ___|\n");
    console_puts(boot, " | |\\/| | | | \\___ \\\n");
    console_puts(boot, " | |  | | |_| |___) |\n");
    console_puts(boot, " |_|  |_|\\___/|____/\n");
    console_puts(boot, "\n");
    console_puts(boot, " MillenniumOS - Kingdom Technology\n");
    console_puts(boot, " Genesis 1:1 - In the beginning...\n");
    console_puts(boot, "\n");
}

/** @} */ /* end B.3 BANNER */

/**
 * =============================================================================
 * B.4 EARLY — Early Kernel Initialization [EARLY]
 * =============================================================================
 *
 * @defgroup kernel_early B.4 EARLY — Early Kernel Initialization
 * @brief    Phase 1 initialization — before memory manager.
 *
 * Validates boot info and sets up minimal state for subsystem init.
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Perform early kernel initialization.
 *
 * @param[in] boot  Boot info from bootloader.
 *
 * @return MOS_OK on success, MOS_ERR_INVALID if boot info invalid.
 *
 * Actions:
 *   - Validate boot info magic and version
 *   - Store boot info reference in g_kernel
 *   - Perform path-specific validation (UEFI vs BIOS)
 *   - Set Hebrew state based on boot path health
 *   - Set stage to MOS_KERNEL_STAGE1
 *   - Print early identification message
 *   - Print boot source with feature summary
 *
 * @note Called before memory manager — no dynamic allocation.
 */
static mos_result_t kernel_early_init(const MosBootInfo* boot) {
    /* =========================================================================
     * Phase 1: Basic validation
     * ========================================================================= */

    /* Validate boot info (unified structure, version 2+) */
    if (!mos_boot_valid(boot)) {
        serial_puts("[EARLY] ERROR: Boot info validation failed\n");
        return MOS_ERR_INVALID;
    }

    /* Store boot info reference */
    g_kernel.boot_info = boot;
    g_kernel.boot_time = boot->entry_tsc;

    /* Set stage to kernel init */
    g_kernel.stage = MOS_KERNEL_STAGE1;

    /* =========================================================================
     * Phase 2: Boot path detection and path-specific validation
     * ========================================================================= */

    trit_t boot_path = TRIT_ZERO;  /* -1=BIOS, 0=unknown, +1=UEFI */
    uint32_t boot_features = 0;    /* Feature flags detected */

    /* Initialize boot context */
    g_boot_ctx.boot_events = boot->boot_event_count;
    g_boot_ctx.last_phase = boot->boot_last_phase;
    g_boot_ctx.last_status = boot->boot_last_status;

    if (mos_boot_is_uefi(boot)) {
        boot_path = TRIT_POS;

        /* UEFI-specific validation */
        serial_puts("[EARLY] Boot path: UEFI (+1)\n");

        /* Check UEFI-specific features */
        if (boot->rsdp_address != 0) {
            boot_features |= 0x01;  /* ACPI provided by firmware */
            serial_puts("[EARLY]   ACPI RSDP: ");
            serial_hex(boot->rsdp_address);
            serial_puts("\n");
        }

        /* Validate GOP framebuffer (UEFI should provide 64-bit address) */
        if (boot->framebuffer.address >= 0x100000000ULL) {
            boot_features |= 0x02;  /* High memory framebuffer (typical UEFI) */
            serial_puts("[EARLY]   Framebuffer: High memory (64-bit)\n");
        } else if (boot->framebuffer.address >= 0x100000) {
            boot_features |= 0x04;  /* Low but valid framebuffer */
            serial_puts("[EARLY]   Framebuffer: Low memory (32-bit range)\n");
        }

        /* Check memory map (UEFI provides EFI memory map) */
        if (boot->memory_map_count > 0 && boot->total_memory > 0) {
            boot_features |= 0x08;  /* Memory map available */
            serial_puts("[EARLY]   Memory map: ");
            serial_hex(boot->memory_map_count);
            serial_puts(" entries, ");
            serial_hex(boot->total_memory >> 20);  /* MB */
            serial_puts(" MB total\n");
        }

        /* UEFI with full features starts at SHALEM (whole, expanding) */
        if (boot_features >= 0x0B) {  /* ACPI + FB + MMAP */
            g_kernel.hebrew_state = MOS_STATE_SHALEM;
            g_kernel.health = MOS_HEALTH_SLIGHT_UP;
            serial_puts("[EARLY]   Hebrew state: SHALEM (whole)\n");
        } else {
            g_kernel.hebrew_state = MOS_STATE_YASHAR;
            g_kernel.health = MOS_HEALTH_NEUTRAL;
            serial_puts("[EARLY]   Hebrew state: YASHAR (even)\n");
        }

    } else if (mos_boot_is_bios(boot)) {
        boot_path = TRIT_NEG;

        /* BIOS-specific validation */
        serial_puts("[EARLY] Boot path: BIOS (-1)\n");

        /* BIOS may not have ACPI RSDP - need to search for it */
        if (boot->rsdp_address != 0) {
            boot_features |= 0x01;  /* ACPI provided (rare for BIOS path) */
            serial_puts("[EARLY]   ACPI RSDP: Provided\n");
        } else {
            serial_puts("[EARLY]   ACPI RSDP: Will search (TODO)\n");
        }

        /* Validate VESA framebuffer (BIOS uses 32-bit addresses) */
        if (boot->framebuffer.address >= 0x100000 &&
            boot->framebuffer.address < 0x100000000ULL) {
            boot_features |= 0x04;  /* Valid VESA framebuffer */
            serial_puts("[EARLY]   Framebuffer: VESA (32-bit)\n");
        } else if (boot->framebuffer.address == 0xB8000) {
            serial_puts("[EARLY]   Framebuffer: Text mode fallback\n");
        }

        /* Check memory map (BIOS provides E820 map) */
        if (boot->memory_map_count > 0 && boot->total_memory > 0) {
            boot_features |= 0x08;  /* Memory map available */
            serial_puts("[EARLY]   Memory map (E820): ");
            serial_hex(boot->memory_map_count);
            serial_puts(" entries\n");
        }

        /* BIOS path starts at YASHAR (even, centered) - legacy compat */
        g_kernel.hebrew_state = MOS_STATE_YASHAR;
        g_kernel.health = MOS_HEALTH_NEUTRAL;
        serial_puts("[EARLY]   Hebrew state: YASHAR (even)\n");

    } else {
        /* Unknown boot path */
        serial_puts("[EARLY] Boot path: UNKNOWN (0)\n");
        serial_puts("[EARLY]   WARNING: Cannot determine boot source\n");

        /* Unknown starts at CHASER (lacking, seeking clarity) */
        g_kernel.hebrew_state = MOS_STATE_CHASER;
        g_kernel.health = MOS_HEALTH_SLIGHT_DOWN;
        serial_puts("[EARLY]   Hebrew state: CHASER (lacking)\n");
    }

    /* =========================================================================
     * Phase 3: Process boot events from bootloader
     * ========================================================================= */

    /* Check boot event status from bootloader */
    if (boot->boot_event_count > 0) {
        uint16_t last_status = boot->boot_last_status;

        serial_puts("[EARLY] Boot events: ");
        serial_hex(boot->boot_event_count);
        serial_puts(" recorded\n");

        /* Check if any boot phase failed */
        if (last_status == 0xFFFF) {
            serial_puts("[EARLY]   WARNING: Boot phase reported failure\n");
            /* Degrade Hebrew state for boot issues */
            if (g_kernel.hebrew_state > MOS_STATE_RATSAH) {
                g_kernel.hebrew_state = MOS_STATE_RATSAH;  /* Wanting/hungry */
            }
        } else if (last_status == 0x0001) {
            serial_puts("[EARLY]   NOTE: Boot phase reported warning\n");
        }
    }

    /* =========================================================================
     * Phase 4: Print identification
     * ========================================================================= */

    /* Print kernel identification */
    console_puts(boot, "MillenniumOS ");
    console_puts(boot, MOS_VERSION_STRING);
    console_puts(boot, " \"");
    console_puts(boot, MOS_CODENAME);
    console_puts(boot, "\"\n");

    /* Print boot source with ternary notation */
    if (boot_path == TRIT_POS) {
        console_puts(boot, "Boot: UEFI (+1) - Full 64-bit, GOP graphics\n");
    } else if (boot_path == TRIT_NEG) {
        console_puts(boot, "Boot: BIOS (-1) - Legacy compat, VESA graphics\n");
    } else {
        console_puts(boot, "Boot: Unknown (0) - Minimal mode\n");
    }

    /* Print feature summary */
    console_puts(boot, "Features: ");
    if (boot_features & 0x01) console_puts(boot, "ACPI ");
    if (boot_features & 0x02) console_puts(boot, "HiMem ");
    if (boot_features & 0x04) console_puts(boot, "FB ");
    if (boot_features & 0x08) console_puts(boot, "MMAP ");
    if (boot_features == 0) console_puts(boot, "(none)");
    console_puts(boot, "\n");

    console_puts(boot, "In the beginning God created...\n\n");

    /* =========================================================================
     * Phase 5: Store boot context for later stages
     * ========================================================================= */

    g_boot_ctx.path = boot_path;
    g_boot_ctx.features = boot_features;

    return MOS_OK;
}

/** @} */ /* end B.4 EARLY */

/**
 * =============================================================================
 * B.5 CPISI — CPI-SI Message Helper [CPISI]
 * =============================================================================
 *
 * @defgroup kernel_cpisi B.5 CPISI — CPI-SI Message Helper
 * @brief    CPI-SI initialization message output.
 *
 * CPI-SI implementation is in cpisi/cpisi.c.
 * This helper prints the init message using console.
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Print CPI-SI initialization status message.
 *
 * @note Uses g_kernel.boot_info for console output.
 */
static void kernel_cpisi_message(void) {
    const MosBootInfo* boot = g_kernel.boot_info;
    console_puts(boot, "[Stage 1] CPI-SI: OK\n");
}

/** @} */ /* end B.5 CPISI */

/**
 * =============================================================================
 * B.6 SERVICES — Kernel Service Accessors [SERVICES]
 * =============================================================================
 *
 * @defgroup kernel_services B.6 SERVICES — Kernel Service Accessors
 * @brief    Public API for accessing kernel state.
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Get pointer to kernel info structure.
 *
 * @return Pointer to global MosKernelInfo (never NULL).
 */
const MosKernelInfo* mos_kernel_info(void) {
    return &g_kernel;
}

/**
 * @brief Get boot path (UEFI vs BIOS).
 *
 * @return Ternary boot path:
 *   - +1 (TRIT_POS): UEFI/EDK2 — full 64-bit, GOP graphics
 *   - 0 (TRIT_ZERO): Unknown — minimal mode
 *   - -1 (TRIT_NEG): BIOS — legacy compat, VESA graphics
 */
trit_t mos_boot_path(void) {
    return g_boot_ctx.path;
}

/**
 * @brief Check if booted via UEFI.
 *
 * @return true if UEFI boot path, false otherwise.
 */
bool mos_is_uefi_boot(void) {
    return g_boot_ctx.path == TRIT_POS;
}

/**
 * @brief Check if booted via BIOS.
 *
 * @return true if BIOS boot path, false otherwise.
 */
bool mos_is_bios_boot(void) {
    return g_boot_ctx.path == TRIT_NEG;
}

/**
 * @brief Get boot feature flags.
 *
 * @return Feature flags:
 *   - 0x01: ACPI RSDP provided
 *   - 0x02: High memory framebuffer (>4GB)
 *   - 0x04: Valid framebuffer
 *   - 0x08: Memory map available
 */
uint32_t mos_boot_features(void) {
    return g_boot_ctx.features;
}

/**
 * @brief Check if specific boot feature is available.
 *
 * @param[in] feature  Feature flag to check.
 *
 * @return true if feature available, false otherwise.
 */
bool mos_has_boot_feature(uint32_t feature) {
    return (g_boot_ctx.features & feature) != 0;
}

/**
 * @brief Get boot info structure.
 *
 * Returns pointer to the boot info passed from bootloader.
 * Used by shell commands to inspect boot-time data.
 *
 * @return Pointer to boot info, or NULL if not available.
 */
const MosBootInfo* mos_get_boot_info(void) {
    return g_kernel.boot_info;
}

/**
 * @brief Get current kernel health score.
 *
 * @return Health score: -3 (critical) to +3 (optimal).
 */
mos_health_t mos_kernel_health(void) {
    return g_kernel.health;
}

/**
 * @brief Get current Hebrew cognitive state.
 *
 * @return Hebrew state (shavar/chaser/ratsah/yashar/tamim/shalem/tov).
 */
MosHebrewState mos_kernel_hebrew_state(void) {
    return g_kernel.hebrew_state;
}

/**
 * @brief Get health trend direction.
 *
 * @return Ternary trend: -1 degrading, 0 stable, +1 improving.
 *
 * Compares current health to baseline (yashar = neutral).
 */
trit_t mos_kernel_health_trend(void) {
    if (g_kernel.health < MOS_HEALTH_NEUTRAL) {
        return TRIT_NEG;
    } else if (g_kernel.health > MOS_HEALTH_NEUTRAL) {
        return TRIT_POS;
    }
    return TRIT_ZERO;
}

/**
 * @brief Get current Genesis boot stage.
 *
 * @return Stage 0-7 (entry through sabbath).
 */
MosKernelStateEx mos_current_stage(void) {
    return g_kernel.stage;
}

/**
 * @brief Advance to next Genesis boot stage.
 *
 * @return MOS_OK if advanced, MOS_ERR_GENERIC if panic, TRIT_POS if at sabbath.
 *
 * Stages saturate at SABBATH (cannot advance beyond stage 7).
 */
mos_result_t mos_advance_stage(void) {
    /* Can only advance if current stage is successful */
    if (g_kernel.state == MOS_KERNEL_PANIC) {
        return MOS_ERR_GENERIC;
    }

    /* Advance stage (saturate at SABBATH) */
    if (g_kernel.stage < MOS_KERNEL_SABBATH) {
        g_kernel.stage++;
        return MOS_OK;
    }

    return TRIT_POS;  /* Already at sabbath (success with info) */
}

/** @} */ /* end B.6 SERVICES */

/**
 * =============================================================================
 * B.7 PANIC — Panic and Halt Handlers [PANIC]
 * =============================================================================
 *
 * @defgroup kernel_panic B.7 PANIC — Panic and Halt Handlers
 * @brief    Critical error handling and CPU halt.
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Kernel panic — unrecoverable error.
 *
 * @param[in] message  Panic reason message.
 *
 * Actions:
 *   - Set state to MOS_KERNEL_PANIC
 *   - Set Hebrew state to SHAVAR (broken)
 *   - Set health to minimum
 *   - Print panic message
 *   - Halt CPU
 *
 * @note This function never returns.
 */
void mos_panic(const char* message) {
    g_kernel.state = MOS_KERNEL_PANIC;
    g_kernel.hebrew_state = MOS_STATE_SHAVAR;  // Broken state
    g_kernel.health = MOS_HEALTH_MIN;

    const MosBootInfo* boot = g_kernel.boot_info;
    console_puts(boot, "\n!!! KERNEL PANIC !!!\n");
    console_puts(boot, message);
    console_puts(boot, "\n");

    mos_halt();
}

/**
 * @brief Halt CPU permanently.
 *
 * Disables interrupts and enters infinite HLT loop.
 *
 * @note This function never returns.
 */
void mos_halt(void) {
    /* Disable interrupts and halt */
    __asm__ volatile (
        "cli\n"
        "1: hlt\n"
        "jmp 1b"
    );
    __builtin_unreachable();
}

/** @} */ /* end B.7 PANIC */

/**
 * =============================================================================
 * B.8 SECURITY — Stack Protection (GCC Canary) [SECURITY]
 * =============================================================================
 *
 * @defgroup kernel_security B.8 SECURITY — Stack Protection
 * @brief    Stack canary verification failure handling.
 *
 * Called by GCC's stack protector when stack corruption is detected.
 * This is a CRITICAL security event — potential buffer overflow attack
 * or memory corruption.
 *
 * Implementation follows CPI-SI security principles:
 *   - Immediate transition to shavar (broken) state
 *   - Log forensic data while stack is still partially intact
 *   - No return — corruption means we cannot trust ANY stack data
 *   - Halt with interrupts disabled to prevent further exploitation
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Stack forensics structure.
 *
 * Captured before corruption analysis to preserve evidence.
 */
typedef struct MosStackForensics {
    uint64_t rsp;               /**< @brief Stack pointer at detection. */
    uint64_t rbp;               /**< @brief Base pointer. */
    uint64_t rip;               /**< @brief Return address (may be corrupted). */
    uint64_t canary_expected;   /**< @brief Expected canary value. */
    uint64_t canary_actual;     /**< @brief Actual canary value (corrupted). */
} MosStackForensics;

/**
 * @brief Capture current CPU state for forensics.
 *
 * @param[out] forensics  Structure to fill with captured state.
 *
 * @note Called in compromised state — be careful with memory access.
 */
static void capture_stack_state(MosStackForensics* forensics) {
    if (!forensics) return;

    // Capture registers directly - we're in a compromised state so be careful
    __asm__ volatile (
        "mov %%rsp, %0\n"
        "mov %%rbp, %1\n"
        : "=r" (forensics->rsp), "=r" (forensics->rbp)
    );

    // Return address is at [rbp + 8] in standard calling convention
    // But be careful - the stack is corrupted
    if (forensics->rbp != 0) {
        uint64_t* return_addr_ptr = (uint64_t*)(forensics->rbp + 8);
        // Only dereference if it looks like a valid kernel address
        if ((uint64_t)return_addr_ptr > 0xFFFF800000000000ULL) {
            forensics->rip = *return_addr_ptr;
        } else {
            forensics->rip = 0;  // Can't safely read
        }
    }
}

/**
 * @brief Print hex value (minimal security context implementation).
 *
 * @param[in] boot   Boot info for console output.
 * @param[in] value  Value to print in hexadecimal.
 *
 * @note Separate from serial_hex to avoid dependencies in security path.
 */
static void security_print_hex(const MosBootInfo* boot, uint64_t value) {
    static const char hex[] = "0123456789ABCDEF";
    char buf[19] = "0x";  // 0x + 16 hex digits + null

    for (int i = 15; i >= 0; i--) {
        buf[2 + (15 - i)] = hex[(value >> (i * 4)) & 0xF];
    }
    buf[18] = '\0';
    console_puts(boot, buf);
}

/**
 * @brief Stack canary check failure handler.
 *
 * Called by GCC when stack corruption is detected.
 *
 * Actions:
 *   - Transition to SHAVAR (broken) state
 *   - Disable interrupts immediately
 *   - Capture forensic data
 *   - Print security alert with forensics
 *   - Halt CPU permanently
 *
 * @warning This function NEVER returns — the stack is compromised.
 */
void __stack_chk_fail(void) {
    // IMMEDIATELY transition to broken state - trust nothing
    g_kernel.state = MOS_KERNEL_PANIC;
    g_kernel.hebrew_state = MOS_STATE_SHAVAR;  // Broken - cannot continue
    g_kernel.health = MOS_HEALTH_MIN;

    // Disable interrupts immediately - prevent exploitation
    __asm__ volatile ("cli");

    // Capture forensic data while we still can
    MosStackForensics forensics = {0};
    capture_stack_state(&forensics);

    // Output security alert (if boot info is available)
    const MosBootInfo* boot = g_kernel.boot_info;
    if (boot) {
        console_puts(boot, "\n");
        console_puts(boot, "!!! SECURITY ALERT: STACK CORRUPTION DETECTED !!!\n");
        console_puts(boot, "===============================================\n");
        console_puts(boot, "\n");
        console_puts(boot, "Stack canary verification failed.\n");
        console_puts(boot, "This indicates possible buffer overflow or memory corruption.\n");
        console_puts(boot, "\n");
        console_puts(boot, "Forensic Data (may be corrupted):\n");
        console_puts(boot, "  RSP: ");
        security_print_hex(boot, forensics.rsp);
        console_puts(boot, "\n");
        console_puts(boot, "  RBP: ");
        security_print_hex(boot, forensics.rbp);
        console_puts(boot, "\n");
        console_puts(boot, "  RIP: ");
        security_print_hex(boot, forensics.rip);
        console_puts(boot, "\n");
        console_puts(boot, "\n");
        console_puts(boot, "Hebrew State: SHAVAR (broken) - Cannot recover\n");
        console_puts(boot, "Zone: VOID (permissive entry) - Security boundary breached\n");
        console_puts(boot, "\n");
        console_puts(boot, "\"The heart is deceitful above all things,\n");
        console_puts(boot, " and desperately wicked.\" - Jeremiah 17:9\n");
        console_puts(boot, "\n");
        console_puts(boot, "System halted for security.\n");
    }

    // Infinite halt - we CANNOT return with corrupted stack
    __asm__ volatile (
        "1: hlt\n"
        "jmp 1b"
    );

    /* Compiler hint: this code is unreachable */
    __builtin_unreachable();
}

/**
 * @brief Alternate stack check failure handler.
 *
 * Some compilers use this variant. Delegates to __stack_chk_fail().
 */
void __stack_chk_fail_local(void) {
    __stack_chk_fail();
}

/**
 * @brief Stack guard canary value.
 *
 * GCC looks for this symbol. Initialized at compile time.
 *
 * @todo Randomize at boot for better security.
 */
uint64_t __stack_chk_guard = 0x595E9FBD94FDA766ULL;

/** @} */ /* end B.8 SECURITY */

/**
 * =============================================================================
 * B.9 ZONE — Zone Permission Operations [ZONE]
 * =============================================================================
 *
 * @defgroup kernel_zone B.9 ZONE — Zone Permission Operations
 * @brief    Ternary zone permission checking and transitions.
 *
 * Zones: VOID (entry) → WORD (work) → TOV (completion)
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Check zone permission for operation.
 *
 * @param[in] zone   Zone to check.
 * @param[in] flags  Operation flags.
 *
 * @return MOS_OK if permitted, MOS_ERR_INVALID if zone invalid.
 */
mos_result_t mos_zone_check(MosZone zone, mos_flags_t flags) {
    // Validate zone transition based on flags
    if (!MOS_ZONE_VALID(zone)) {
        return MOS_ERR_INVALID;
    }
    (void)flags;
    return MOS_OK;
}

/**
 * @brief Get zone transition direction.
 *
 * @param[in] from  Source zone.
 * @param[in] to    Target zone.
 *
 * @return Ternary direction:
 *   - +1: Forward (void→word→tov)
 *   - 0:  Same zone
 *   - -1: Backward (tov→word→void)
 *
 * Uses trit_hsub from libtrit for ternary subtraction.
 */
trit_t mos_zone_direction(MosZone from, MosZone to) {
    trit_result_t result = trit_hsub(to, from);
    return result.value;
}

/** @} */ /* end B.9 ZONE */

/**
 * =============================================================================
 * B.10 SABBATH — Enter Operational Rest [SABBATH]
 * =============================================================================
 *
 * @defgroup kernel_sabbath B.10 SABBATH — Enter Operational Rest
 * @brief    Final stage of Genesis boot — system operational.
 *
 * "And God saw every thing that he had made, and, behold, it was very good."
 *   — Genesis 1:31
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Enter Sabbath state (operational rest).
 *
 * Actions:
 *   - Set stage to MOS_KERNEL_SABBATH
 *   - Set state to MOS_KERNEL_RUNNING
 *   - Set Hebrew state to TOV (perfect)
 *   - Set health to maximum
 *   - Print completion message
 *
 * @note Uses g_kernel.boot_info for console output.
 */
static void kernel_enter_sabbath(void) {
    g_kernel.stage = MOS_KERNEL_SABBATH;
    g_kernel.state = MOS_KERNEL_RUNNING;
    g_kernel.hebrew_state = MOS_STATE_TOV;  // Perfect operational state
    g_kernel.health = MOS_HEALTH_MAX;

    const MosBootInfo* boot = g_kernel.boot_info;
    console_puts(boot, "\n[Stage 6] Sabbath: System operational\n");
    console_puts(boot, "\"And God saw every thing that he had made,\n");
    console_puts(boot, " and, behold, it was very good.\"\n");
    console_puts(boot, "                    - Genesis 1:31\n");
}

/** @} */ /* end B.10 SABBATH */

/**
 * =============================================================================
 * B.11 MAIN — Main Kernel Entry Point [MAIN]
 * =============================================================================
 *
 * @defgroup kernel_main B.11 MAIN — Main Kernel Entry Point
 * @brief    Called from entry.asm after Stage 0 completes.
 *
 * Implements the 7-stage Genesis boot sequence:
 *   - Stage 0: Entry (received from bootloader)
 *   - Stage 1: Light (serial + display init)
 *   - Stage 2: Firmament (memory management)
 *   - Stage 3: Land/Sea (process boundaries)
 *   - Stage 4: Luminaries (timers + scheduling)
 *   - Stage 5: Creatures (initial processes)
 *   - Stage 6: Humanity (user services)
 *   - Stage 7: Sabbath (operational rest)
 *
 * =============================================================================
 * @{
 */

/**
 * @brief Main kernel entry point.
 *
 * @param[in] boot_info  Boot info from bootloader (at MOS_BOOT_INFO_ADDR).
 *
 * Called from entry.asm after BSS clear and boot validation.
 * Implements full 7-stage Genesis boot sequence.
 *
 * @note This function never returns — enters Sabbath and then halts.
 */
void mos_kernel_main(MosBootInfo* boot_info) {
    mos_result_t result;

    // =========================================================================
    // ABSOLUTE FIRST: Visual boot event display (before ANY other code)
    // =========================================================================
    // Draw colored blocks showing boot progress. No font needed.
    // Each boot phase gets a colored block. This proves kernel is alive
    // AND shows what phases completed.
    if (boot_info && boot_info->magic == 0x4D4F5342 &&
        boot_info->framebuffer.address >= 0x100000) {

        uint32_t* fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address;
        uint32_t width = boot_info->framebuffer.width;
        uint32_t height = boot_info->framebuffer.height;
        uint32_t pitch = boot_info->framebuffer.pitch / 4;  // pixels per row

        // First: Fill entire screen with dark blue (proves FB access)
        for (uint32_t y = 0; y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                fb[y * pitch + x] = 0xFF000040;  // Dark blue
            }
        }

        // Draw "KERNEL ALIVE" indicator: Green bar at top
        for (uint32_t y = 0; y < 20 && y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                fb[y * pitch + x] = 0xFF00FF00;  // Bright green
            }
        }

        // Draw boot event blocks (each phase = colored square)
        // Phase colors: INIT=white, BOOTINFO=cyan, GOP=magenta,
        //               KERNEL=yellow, ACPI=orange, MMAP=red, EXIT=purple, TRANSFER=green
        uint32_t phase_colors[8] = {
            0xFFFFFFFF,  // 0x0000 INIT - white
            0xFF00FFFF,  // 0x0100 BOOTINFO - cyan
            0xFFFF00FF,  // 0x0200 GOP - magenta
            0xFFFFFF00,  // 0x0300 KERNEL - yellow
            0xFFFF8000,  // 0x0400 ACPI - orange
            0xFFFF0000,  // 0x0500 MMAP - red
            0xFF8000FF,  // 0x0600 EXIT - purple
            0xFF00FF00   // 0x0700 TRANSFER - green
        };

        // Draw recorded boot events as colored blocks
        uint32_t event_count = boot_info->boot_event_count;
        if (event_count > 6) event_count = 6;

        uint32_t block_size = 40;
        uint32_t block_y = 30;  // Below green bar

        for (uint32_t i = 0; i < event_count; i++) {
            uint16_t code = boot_info->boot_events[i].event_code;
            uint16_t phase = (code >> 8) & 0x07;  // Extract phase index (0-7)
            uint16_t step = code & 0x0F;
            uint32_t color = phase_colors[phase];

            // Dim color if WARN, red border if FAIL
            if (step == 0x02) color = (color & 0xFF7F7F7F);  // WARN: dim
            if (step == 0x0F) color = 0xFFFF0000;            // FAIL: red

            // Draw block
            uint32_t block_x = 10 + i * (block_size + 10);
            for (uint32_t y = block_y; y < block_y + block_size && y < height; y++) {
                for (uint32_t x = block_x; x < block_x + block_size && x < width; x++) {
                    fb[y * pitch + x] = color;
                }
            }
        }

        // Draw last phase indicator as large block on right
        uint16_t last_phase = (boot_info->boot_last_phase >> 8) & 0x07;
        uint32_t last_color = phase_colors[last_phase];
        uint32_t big_x = width - 100;
        for (uint32_t y = block_y; y < block_y + 60 && y < height; y++) {
            for (uint32_t x = big_x; x < big_x + 80 && x < width; x++) {
                fb[y * pitch + x] = last_color;
            }
        }
    } else {
        // boot_info invalid - try writing directly to common FB addresses
        // This is a last-resort diagnostic
        uint32_t* fb_guess = (uint32_t*)0xFD000000;  // Common UEFI GOP address
        for (int i = 0; i < 1000; i++) {
            fb_guess[i] = 0xFFFF0000;  // Red pixels = boot_info invalid
        }
    }

    // =========================================================================
    // FIRST: Initialize serial console for debugging
    // =========================================================================
    // This MUST happen before anything else so we can see debug output
    // even if graphics fail.
    serial_init();
    serial_puts("\n\n=== MillenniumOS Kernel Starting ===\n");
    serial_puts("Serial console initialized (COM1 @ 115200)\n");

    // =========================================================================
    // IMMEDIATE: Display boot event status (before anything else)
    // =========================================================================
    // This shows what the bootloader recorded - diagnostic breadcrumbs
    if (boot_info && boot_info->magic == 0x4D4F5342) {
        serial_puts("[BOOT-EVENTS] Last phase: ");
        serial_hex(boot_info->boot_last_phase);
        serial_puts(" status: ");
        serial_hex(boot_info->boot_last_status);
        serial_puts(" count: ");
        serial_hex(boot_info->boot_event_count);
        serial_puts("\n");

        // Decode last phase for human readability
        const char* phase_name = "UNKNOWN";
        switch (boot_info->boot_last_phase) {
            case 0x0000: phase_name = "INIT"; break;
            case 0x0100: phase_name = "BOOTINFO"; break;
            case 0x0200: phase_name = "GOP"; break;
            case 0x0300: phase_name = "KERNEL"; break;
            case 0x0400: phase_name = "ACPI"; break;
            case 0x0500: phase_name = "MMAP"; break;
            case 0x0600: phase_name = "EXIT"; break;
            case 0x0700: phase_name = "TRANSFER"; break;
        }
        serial_puts("[BOOT-EVENTS] Phase: ");
        serial_puts(phase_name);
        serial_puts("\n");

        // List recorded events
        uint32_t count = boot_info->boot_event_count;
        if (count > 6) count = 6;  // Max buffer size
        for (uint32_t i = 0; i < count; i++) {
            serial_puts("  Event[");
            serial_hex(i);
            serial_puts("]: code=");
            serial_hex(boot_info->boot_events[i].event_code);
            serial_puts(" status=");
            serial_hex(boot_info->boot_events[i].status);
            serial_puts(" value=");
            serial_hex(boot_info->boot_events[i].value);
            serial_puts("\n");
        }
    }

    // =========================================================================
    // IMMEDIATE VISUAL TEST: Fill screen with blue to prove FB access works
    // =========================================================================
    // If we see blue, the kernel can access the framebuffer!
    serial_puts("[DEBUG] boot_info address: ");
    serial_hex((uint64_t)(uintptr_t)boot_info);
    serial_puts("\n");

    if (boot_info) {
        serial_puts("[DEBUG] magic: ");
        serial_hex(boot_info->magic);
        serial_puts(" (expected 0x4D4F5342)\n");

        // Verify magic FIRST
        if (boot_info->magic != 0x4D4F5342) {
            serial_puts("[DEBUG] ERROR: Invalid boot_info magic! Halting.\n");
            mos_halt();
        }

        serial_puts("[DEBUG] framebuffer.address: ");
        serial_hex(boot_info->framebuffer.address);
        serial_puts("\n");
    } else {
        serial_puts("[DEBUG] ERROR: boot_info is NULL!\n");
        mos_halt();
    }

    if (boot_info->framebuffer.address) {
        serial_puts("[DEBUG] Framebuffer test - filling screen with blue...\n");
        uint64_t fb_addr = boot_info->framebuffer.address;
        uint32_t width = boot_info->framebuffer.width;
        uint32_t height = boot_info->framebuffer.height;
        uint32_t pitch = boot_info->framebuffer.pitch;
        serial_puts("[DEBUG] FB: ");
        serial_hex(fb_addr);
        serial_puts(" ");
        serial_hex(width);
        serial_puts("x");
        serial_hex(height);
        serial_puts(" pitch=");
        serial_hex(pitch);
        serial_puts("\n");

        // Validate framebuffer address is in reasonable range
        // - BIOS/VESA: typically 0xC0000000-0xFFFFFFFF (below 4GB)
        // - UEFI/GOP: can be anywhere, including above 4GB
        // - Text mode fallback uses 0xB8000 which we detect and skip
        if (fb_addr == 0xB8000) {
            serial_puts("[DEBUG] Text mode fallback detected (0xB8000) - skipping graphics\n");
        } else if (fb_addr >= 0x100000 &&
            width > 0 && width <= 4096 &&
            height > 0 && height <= 4096 &&
            pitch > 0 && pitch <= 16384) {

            serial_puts("[DEBUG] FB address looks valid, attempting single pixel test...\n");

            // Test: write just ONE pixel first
            uint32_t* fb = (uint32_t*)(uintptr_t)fb_addr;
            serial_puts("[DEBUG] Writing single test pixel...\n");
            fb[0] = 0xFFFF0000;  // Red pixel at (0,0)
            serial_puts("[DEBUG] Single pixel OK!\n");

            // Test: write a few more pixels
            serial_puts("[DEBUG] Writing small test block...\n");
            for (int i = 0; i < 100; i++) {
                fb[i] = 0xFFFF0000;  // Red pixels
            }
            serial_puts("[DEBUG] Small block OK!\n");

            // Now try filling the screen
            serial_puts("[DEBUG] Attempting full screen fill...\n");
            for (uint32_t y = 0; y < height; y++) {
                uint32_t* row = (uint32_t*)((uint8_t*)fb + y * pitch);
                for (uint32_t x = 0; x < width; x++) {
                    row[x] = 0xFFFF0000;  // Red
                }
            }
            serial_puts("[DEBUG] Full fill complete!\n");
        } else {
            serial_puts("[DEBUG] ERROR: FB address/dimensions invalid!\n");
            serial_puts("[DEBUG] fb_addr >= 0x100000: ");
            serial_hex(fb_addr >= 0x100000 ? 1 : 0);
            serial_puts("\n");
        }
    } else {
        serial_puts("[DEBUG] ERROR: No valid framebuffer in boot_info!\n");
    }

    // -------------------------------------------------------------------------
    // Stage 1: Kernel Initialization ("God")
    // -------------------------------------------------------------------------

    serial_puts("[KERNEL] Stage 1: Early init...\n");

    // Early event: Kernel entry (before CPISI is ready)
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "kernel", "Stage 1 entry", MOS_HEALTH_NEUTRAL);

    result = kernel_early_init(boot_info);
    if (MOS_FAILED(result)) {
        mos_early_event(MOS_DAR_DETECT, MOS_SEV_CRITICAL,
                        "kernel", "Early init FAILED", MOS_HEALTH_MAJOR_DOWN);
        serial_puts("[KERNEL] PANIC: Early init failed!\n");
        mos_panic("Early init failed");
    }
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "kernel", "Early init OK", MOS_HEALTH_SLIGHT_UP);
    serial_puts("[KERNEL] Early init complete\n");

    // Print the kernel banner
    kernel_print_banner();

    // Initialize memory first (needed by everything else)
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "memory", "Init starting", MOS_HEALTH_NEUTRAL);
    result = mos_memory_init(boot_info);
    if (MOS_FAILED(result)) {
        mos_early_event(MOS_DAR_DETECT, MOS_SEV_CRITICAL,
                        "memory", "Init FAILED", MOS_HEALTH_MAJOR_DOWN);
        mos_panic("Memory init failed");
    }
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "memory", "Init OK", MOS_HEALTH_SLIGHT_UP);

    // Initialize interrupts
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "intr", "Init starting", MOS_HEALTH_NEUTRAL);
    result = mos_interrupt_init();
    if (MOS_FAILED(result)) {
        mos_early_event(MOS_DAR_DETECT, MOS_SEV_CRITICAL,
                        "intr", "Init FAILED", MOS_HEALTH_MAJOR_DOWN);
        mos_panic("Interrupt init failed");
    }
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "intr", "Init OK", MOS_HEALTH_SLIGHT_UP);

    // Initialize process management
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "proc", "Init starting", MOS_HEALTH_NEUTRAL);
    result = mos_process_init();
    if (MOS_FAILED(result)) {
        mos_early_event(MOS_DAR_DETECT, MOS_SEV_CRITICAL,
                        "proc", "Init FAILED", MOS_HEALTH_MAJOR_DOWN);
        mos_panic("Process init failed");
    }
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "proc", "Init OK", MOS_HEALTH_SLIGHT_UP);

    // Initialize VFS
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "vfs", "Init starting", MOS_HEALTH_NEUTRAL);
    result = mos_vfs_init();
    if (MOS_FAILED(result)) {
        mos_early_event(MOS_DAR_DETECT, MOS_SEV_CRITICAL,
                        "vfs", "Init FAILED", MOS_HEALTH_MAJOR_DOWN);
        mos_panic("VFS init failed");
    }
    mos_early_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                    "vfs", "Init OK", MOS_HEALTH_SLIGHT_UP);

    // Initialize CPI-SI subsystem
    result = mos_cpisi_init();
    if (MOS_FAILED(result)) {
        mos_panic("CPI-SI init failed");
    }

    // Import boot events from Stage 0 into witness system
    // This connects the full boot chain into the DAR learning system
    result = mos_witness_import_boot_events(boot_info);
    if (MOS_FAILED(result)) {
        serial_puts("[KERNEL] Warning: Boot event import failed\n");
    }

    // Import early kernel events into witness system
    result = mos_witness_import_early_events();
    if (MOS_FAILED(result)) {
        serial_puts("[KERNEL] Warning: Early event import failed\n");
    }

    // Mark CPISI as ready for direct witness calls
    mos_cpisi_mark_ready();

    // Log that witness system is now fully operational
    mos_witness_event(MOS_DAR_DETECT, MOS_SEV_MILD,
                      "cpisi", "Witness system ready",
                      MOS_HEALTH_SLIGHT_UP);

    kernel_cpisi_message();

    // Initialize Diagnostic service
    result = mos_diag_init();
    if (MOS_FAILED(result)) {
        mos_panic("Diagnostic init failed");
    }
    console_puts(boot_info, "[Stage 1] Diagnostic: OK\n");

    // Advance to Stage 2
    mos_advance_stage();
    console_puts(boot_info, "[Stage 1] Kernel: Complete\n\n");

    // -------------------------------------------------------------------------
    // Stage 2: HAL Initialization ("created")
    // -------------------------------------------------------------------------

    console_puts(boot_info, "[Stage 2] HAL: Initializing...\n");

    // Initialize HAL core
    result = mos_hal_init();
    if (MOS_FAILED(result)) {
        mos_panic("HAL init failed");
    }
    console_puts(boot_info, "  - Core: OK\n");

    // Initialize display with boot info framebuffer
    result = mos_hal_init_display(boot_info);
    if (MOS_FAILED(result)) {
        mos_panic("Display init failed");
    }
    console_puts(boot_info, "  - Display: OK\n");

    // Initialize input (keyboard + mouse)
    result = mos_hal_init_input();
    if (MOS_FAILED(result)) {
        mos_panic("Input init failed");
    }
    console_puts(boot_info, "  - Input: OK\n");

    mos_advance_stage();

    // -------------------------------------------------------------------------
    // Stage 3: Graphics Initialization ("the heaven")
    // -------------------------------------------------------------------------

    console_puts(boot_info, "[Stage 3] Graphics: Initializing...\n");

    result = mos_graphics_init();
    if (MOS_FAILED(result)) {
        mos_panic("Graphics init failed");
    }
    console_puts(boot_info, "  - Rendering: OK\n");
    mos_advance_stage();

    // -------------------------------------------------------------------------
    // Stage 4: UI Initialization ("and")
    // -------------------------------------------------------------------------

    console_puts(boot_info, "[Stage 4] UI: Initializing...\n");

    // Initialize UI subsystem
    result = mos_ui_init();
    if (MOS_FAILED(result)) {
        mos_panic("UI init failed");
    }
    console_puts(boot_info, "  - Framework: OK\n");

    // Initialize UI graphics context
    result = mos_ui_init_graphics();
    if (MOS_FAILED(result)) {
        mos_panic("UI graphics init failed");
    }
    console_puts(boot_info, "  - Graphics: OK\n");

    // Register Cornerstone Engine as application
    result = mos_ui_register_app(
        "Cornerstone",
        "/apps/cornerstone",
        1,              // Icon ID 1
        MOS_ZONE_TOV    // Requires tov zone (highest privilege)
    );
    if (MOS_FAILED(result)) {
        console_puts(boot_info, "  - Cornerstone: WARN (not registered)\n");
    } else {
        console_puts(boot_info, "  - Cornerstone: Registered\n");
    }

    // Register additional system apps
    mos_ui_register_app("Terminal", "/apps/terminal", 2, MOS_ZONE_WORD);
    mos_ui_register_app("Files", "/apps/files", 3, MOS_ZONE_WORD);
    mos_ui_register_app("Settings", "/apps/settings", 4, MOS_ZONE_WORD);

    // Register diagnostic probes (all subsystems now initialized)
    result = mos_diag_register_all_probes();
    if (MOS_FAILED(result)) {
        console_puts(boot_info, "  - Diagnostic probes: WARN\n");
    } else {
        console_puts(boot_info, "  - Diagnostic probes: Registered\n");
    }

    mos_advance_stage();

    // -------------------------------------------------------------------------
    // Stage 5: User Init ("the earth")
    // -------------------------------------------------------------------------

    console_puts(boot_info, "[Stage 5] Init: Starting user interface...\n");

    // Show boot splash briefly
    result = mos_ui_show_boot();
    if (MOS_FAILED(result)) {
        console_puts(boot_info, "  - Boot splash: WARN\n");
    }

    // Short delay for splash (in real OS would be async)
    for (volatile int i = 0; i < 10000000; i++) { }

    // -------------------------------------------------------------------------
    // Check for Fresh Install
    // -------------------------------------------------------------------------
    //
    // "Let the waters... be gathered... and let the dry land appear."
    // If this is a fresh install, we need to establish the foundation first.
    //

    MosInstallState install_state = mos_installer_check_state();
    console_puts(boot_info, "  - Install check: ");

    if (install_state == MOS_INSTALL_FRESH) {
        console_puts(boot_info, "Fresh install detected\n");
        console_puts(boot_info, "  - Starting installer...\n");

        // Run the installer wizard
        result = mos_installer_run();
        if (MOS_FAILED(result)) {
            mos_panic("Installation failed");
        }
        console_puts(boot_info, "  - Installation complete!\n");

        // Reboot after installation
        console_puts(boot_info, "\nSystem will restart...\n");
        for (volatile int i = 0; i < 50000000; i++) { }
        // In real implementation: mos_hal_reboot();
    } else {
        console_puts(boot_info, "System installed\n");
    }

    // Show login screen (after installation or on normal boot)
    result = mos_ui_show_login();
    if (MOS_FAILED(result)) {
        mos_panic("Login screen failed");
    }
    console_puts(boot_info, "  - Login screen: Ready\n");

    mos_advance_stage();

    // -------------------------------------------------------------------------
    // Stage 6: Sabbath (Operational Rest)
    // -------------------------------------------------------------------------

    kernel_enter_sabbath();

    // -------------------------------------------------------------------------
    // Main Loop - Shell / UI Event Loop
    // -------------------------------------------------------------------------
    //
    // The kernel enters sabbath rest. The shell provides command-line interface
    // for diagnostics, DAR queries, and system management. The UI event loop
    // handles graphical interaction.
    //
    // "And on the seventh day God ended his work which he had made;
    //  and he rested on the seventh day from all his work." - Genesis 2:2
    //

    // Initialize and run the terminal shell
    // Shell provides: DAR witness queries, boot info, memory inspection, help
    MosShellContext shell_ctx;
    MosShellResult shell_result = shell_init(&shell_ctx,
        SHELL_FEAT_HISTORY | SHELL_FEAT_COLOR);

    if (shell_result == SHELL_OK) {
        console_puts(boot_info, "  - Shell: Initialized\n");
        serial_puts("[KERNEL] Shell initialized, entering main loop\r\n");

        // Run the shell main loop (returns on 'exit' or error)
        shell_result = shell_run(&shell_ctx);

        // Clean up shell resources
        shell_cleanup(&shell_ctx);

        if (shell_result == SHELL_EXIT_REQUEST) {
            console_puts(boot_info, "  - Shell: Exit requested\n");
        } else if (shell_result != SHELL_OK) {
            console_puts(boot_info, "  - Shell: Error occurred\n");
        }
    } else {
        console_puts(boot_info, "  - Shell: Init failed, falling back to UI\n");
        serial_puts("[KERNEL] Shell init failed, running UI event loop\r\n");
        // Fallback to UI event loop if shell init fails
        mos_ui_event_loop();
    }

    // Show shutdown screen
    mos_ui_show_shutdown();

    /* Halt after shutdown */
    mos_halt();
}

/** @} */ /* end B.11 MAIN */

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING BLOCK [CLOSING]
// =============================================================================
//
// 4-Block Code Structure: Operational guidance and closing notes.
//
// CONTAINS:
//   - X.1 VALIDATION    — Build & test verification
//   - X.2 EXECUTION     — Entry point & flow
//   - X.3 CLEANUP       — Resource management
//   - X.4 OVERVIEW      — Module summary
//   - X.5 POLICY        — Modification guidelines
//   - X.6 FLOW          — Dependency chain
//   - X.7 PERF          — Performance considerations
//   - X.8 DEBUG         — Troubleshooting guide
//   - X.9 RELATED       — Related components
//   - X.10 NOTE         — Closing guidance
//
// =============================================================================

/**
 * =============================================================================
 * X.1 VALIDATION — Build & Test Verification [VALIDATION]
 * =============================================================================
 *
 * @par X.1a BUILD — Compilation Commands
 * @code{.sh}
 * # Full kernel build (from millenniumos root)
 * make clean && make                    # Build kernel + bootloader
 * make V=1                              # Verbose build for debugging
 * @endcode
 *
 * @par X.1b RUNTIME — Test Commands
 * @code{.sh}
 * make run                              # Run in QEMU (UEFI mode)
 * make run-bios                         # Run in QEMU (BIOS mode)
 * make debug                            # Run with GDB attached
 * @endcode
 *
 * @par X.1c SERIAL — Serial Output Verification
 * @code{.sh}
 * # Serial output goes to stdio in QEMU
 * # Check for: "MillenniumOS Kernel" banner
 * # Check for: Subsystem initialization messages
 * @endcode
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.2 EXECUTION — Entry Point & Flow [EXECUTION]
 * =============================================================================
 *
 * @par X.2a ENTRY — kernel_main() Entry Point
 *
 * Called from: entry.asm (after BSS clear and boot validation)
 * Receives:    MosBootInfo* pointer at known address (MOS_BOOT_INFO_ADDR)
 *
 * @par X.2b FLOW — Execution Sequence
 *
 * 1. entry.asm validates boot info, clears BSS
 * 2. entry.asm calls kernel_main(boot_info)
 * 3. kernel_main initializes serial output
 * 4. kernel_main prints banner
 * 5. kernel_main initializes subsystems (memory, interrupts, processes...)
 * 6. kernel_main enters Sabbath (operational rest)
 *
 * @par X.2c STAGES — 7-Stage Genesis Boot
 *
 * Stage 0: Entry       — Receive control from bootloader
 * Stage 1: Light       — Initialize display/serial output
 * Stage 2: Firmament   — Initialize memory management
 * Stage 3: Land/Sea    — Initialize process boundaries
 * Stage 4: Luminaries  — Initialize timers and scheduling
 * Stage 5: Creatures   — Start initial processes
 * Stage 6: Humanity    — Start user services
 * Stage 7: Sabbath     — Operational rest
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.3 CLEANUP — Resource Management [CLEANUP]
 * =============================================================================
 *
 * @par X.3a OWNERSHIP — Ownership Conventions
 *
 * - g_kernel: Global kernel state (never freed — lives for kernel lifetime)
 * - boot_info: Provided by bootloader, read-only
 * - Subsystem handles: Each subsystem manages its own resources
 *
 * @par X.3b SHUTDOWN — Graceful Shutdown
 *
 * - kernel_enter_sabbath() enters operational rest
 * - mos_halt() halts CPU (infinite loop with HLT)
 * - No dynamic memory to free at kernel level (freestanding)
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.4 OVERVIEW — Module Summary [OVERVIEW]
 * =============================================================================
 *
 * The MillenniumOS kernel implements the 7-stage Genesis boot sequence.
 *
 * Every operation uses ternary types from libtrit:
 *   - Results: negative=error, zero=success, positive=info
 *   - Stages: trit5 progression through boot
 *   - States: Hebrew cognitive mapping (shavar → yashar → tov)
 *   - Zones: trit-based permission boundaries
 *
 * Architecture:
 *   - CPISI_MODE=0: Freestanding (no standard library)
 *   - Layer 0: Kernel foundation
 *   - Header ladder: kernel.h provides all types
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.5 POLICY — Modification Guidelines [POLICY]
 * =============================================================================
 *
 * SAFE TO MODIFY:
 *   - Add new subsystem initialization calls
 *   - Extend boot validation checks
 *   - Add DAR witness event logging
 *
 * MODIFY WITH CARE:
 *   - kernel_main signature (breaks entry.asm)
 *   - Boot info structure usage (affects UEFI/BIOS paths)
 *   - g_kernel initial state (affects all subsystems)
 *
 * NEVER MODIFY:
 *   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
 *   - Genesis stage ordering (0-7)
 *   - kernel.h must be first include
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.6 FLOW — Dependency Chain [FLOW]
 * =============================================================================
 *
 * @verbatim
 * BOOT CHAIN:
 *   BIOS/UEFI → stage0a → stage0b → entry.asm → kernel.c
 *
 * INCLUDE CHAIN:
 *   kernel.h → (all subsystem headers) → kernel.c
 *
 * INIT CHAIN:
 *   serial_init → banner → memory → interrupts → processes → sabbath
 * @endverbatim
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.7 PERF — Performance Considerations [PERF]
 * =============================================================================
 *
 * BOOT TIME:
 *   - Serial init: ~100 cycles (port configuration)
 *   - Banner print: ~10ms (character-by-character output)
 *   - Subsystem init: varies by subsystem
 *
 * MEMORY:
 *   - g_kernel: ~128 bytes (static, never freed)
 *   - Console state: ~8 bytes (x, y coordinates)
 *   - No heap allocation in kernel core
 *
 * OPTIMIZATION:
 *   - Inline serial I/O functions for speed
 *   - Direct framebuffer writes (no buffering)
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.8 DEBUG — Troubleshooting Guide [DEBUG]
 * =============================================================================
 *
 * "No serial output":
 *   - Check serial_init() is called
 *   - Verify MOS_COM1_BASE matches QEMU config
 *   - Try: make debug (attach GDB)
 *
 * "No framebuffer output":
 *   - Check boot_info->framebuffer.address is valid
 *   - Serial still works — check serial output for errors
 *   - Verify GOP/VESA framebuffer setup in bootloader
 *
 * "Kernel hangs":
 *   - Add serial_puts() markers to find hang location
 *   - Check for infinite loops in subsystem init
 *   - Use GDB: make debug, set breakpoint at kernel_main
 *
 * "Boot info invalid":
 *   - entry.asm validates magic (MOS_BOOT_MAGIC)
 *   - Check boot_info address matches MOS_BOOT_INFO_ADDR
 *   - Verify bootloader fills all required fields
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.9 RELATED — Related Components [RELATED]
 * =============================================================================
 *
 * UPSTREAM (calls this):
 *   - kernel/src/entry.asm     — Assembly entry, calls kernel_main
 *   - boot/stage0b.asm         — Bootloader, sets up boot_info
 *
 * DOWNSTREAM (this calls):
 *   - kernel/src/core/memory/  — Memory management
 *   - kernel/src/core/interrupt/ — Interrupt handling
 *   - kernel/src/core/process/   — Process management
 *   - kernel/src/services/       — Kernel services (VFS, graphics, etc.)
 *
 * PEERS (same level):
 *   - kernel.h                 — Kernel types and declarations
 *   - serial.h                 — Shared serial port constants
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * X.10 NOTE — Closing Guidance [NOTE]
 * =============================================================================
 *
 * Scripture: Genesis 1:1 — "In the beginning God created the heaven and
 *            the earth."
 *
 * The kernel speaks into the void and brings forth order. From the
 * formless state after boot handoff, the kernel creates structure:
 * memory, processes, filesystems, services.
 *
 * The final state is Sabbath — operational rest where the system serves
 * its purpose without the kernel actively creating. This is not idleness
 * but fulfilled purpose.
 *
 * "And on the seventh day God ended his work which he had made; and he
 * rested on the seventh day from all his work which he had made."
 *   — Genesis 2:2
 *
 * =============================================================================
 */

// =============================================================================
// END CLOSING
// =============================================================================
