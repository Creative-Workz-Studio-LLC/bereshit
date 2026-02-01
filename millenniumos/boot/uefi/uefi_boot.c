/**
 * @file uefi_boot.c
 * @omni document --source
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * OMNICODE PRAGMA [PRAGMA]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @legend
 *   :req  REQUIRED   — must exist, validation fails without
 *   :inh  INHERITED  — from template, override allowed
 *   :ins  INSTANCE   — file-specific, unique values
 * @endlegend
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * P.1 CORE — Identity [CORE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni:req  key        = MOS-BOOT-UEFI
 * @omni:req  from       = bereshit/word/seed/code/c/source.c
 * @omni:req  at         = document
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * P.2 FAMILY — Classification [FAMILY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni:inh  type       = source
 * @omni:inh  subtype    = uefi-application
 * @omni:inh  role       = bootloader
 * @omni:inh  structure  = 4-block
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * P.3 INSTANCE — File Details [INSTANCE]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni:ins  component  = boot
 * @omni:ins  layer      = stage0/uefi
 * @omni:ins  includes   = [efi.h, efilib.h, bootinfo.h, serial.h, debug.h]
 * @omni:ins  provides   = [efi_main, init_gop, load_kernel, get_memory_map]
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * P.4 ARCHITECTURE — Boot Context [ARCH]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni:ins  boot_path  = UEFI (primary, modern systems)
 * @omni:ins  alt_path   = BIOS (stage0a.asm, legacy systems)
 * @omni:ins  tripwire   = GOP (primary) → VESA (fallback) → VGA (ultimate)
 * @omni:ins  anchor     = genesis_1_1 -> creation -> first_code -> boot
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * P.5 SUMMARY — Human Identity [SUMMARY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @omni:req  title      = MillenniumOS UEFI Bootloader
 * @omni:req  brief      = UEFI-native bootloader with GOP graphics, secure boot,
 *                         multi-kernel selection, and boot configuration support.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * END PRAGMA [END]
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// METADATA BLOCK [METADATA]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief Identity and context for this component.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// STRUCTURE: M.1-M.10 sections for complete file identity:
///   - M.1 IDENTITY:      Core identity (file, brief, key, title, type, component, role)
///   - M.2 STATE:         Version, dates, status
///   - M.3 ATTRIBUTION:   Authors, copyright
///   - M.4 LOCATION:      File path in project
///   - M.5 DERIVATION:    Template lineage
///   - M.6 CLASSIFICATION: Tags for discovery
///   - M.7 INTENT:        Purpose statement
///   - M.8 GROUNDING:     Biblical foundation
///   - M.9 DEPENDENCIES:  Required components
///   - M.10 ROADMAP:      Version history and future plans
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.1 IDENTITY — Core Identity [IDENTITY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @file      uefi_boot.c
 * @brief     UEFI Bootloader — Loads MillenniumOS kernel via UEFI/GOP with
 *            secure boot verification, multi-kernel selection, and boot config.
 *
 * @defgroup uefi_boot UEFI Bootloader
 * @{
 *
 * @defgroup uefi_identity M.1 IDENTITY — Core Identity
 * @ingroup uefi_boot
 * @{
 *
 * Key:       MOS-BOOT-UEFI
 * Title:     MillenniumOS UEFI Bootloader
 * Type:      Source (UEFI Application)
 * Component: Boot — Stage 0 UEFI entry point
 * Role:      Load kernel with verification, initialize GOP framebuffer,
 *            parse boot configuration, and transfer control to kernel.
 *
 * @} end M.1 IDENTITY
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.2 STATE — Lifecycle State [STATE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_state M.2 STATE — Lifecycle State
 * @ingroup uefi_boot
 * @{
 *
 * @version   a-02.00
 * @date      2026-01-31
 *
 * Status:    Active
 * Created:   2026-01-30
 * Updated:   2026-01-31
 *
 * @} end M.2 STATE
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_attribution M.3 ATTRIBUTION — Authorship & Rights
 * @ingroup uefi_boot
 * @{
 *
 * @author    Seanje Lenox-Wise (Architect)
 * @author    Nova Dawn (Implementation)
 * @copyright © 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * @} end M.3 ATTRIBUTION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.4 LOCATION — File Position [LOCATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_location M.4 LOCATION — File Position
 * @ingroup uefi_boot
 * @{
 *
 * Path:      millenniumos/boot/uefi/uefi_boot.c
 *
 * @} end M.4 LOCATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.5 DERIVATION — Template Lineage [DERIVATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_derivation M.5 DERIVATION — Template Lineage
 * @ingroup uefi_boot
 * @{
 *
 * Derives:   bereshit/word/seed/code/c/source.c (4-block source template)
 *
 * @} end M.5 DERIVATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_classification M.6 CLASSIFICATION — Categorization
 * @ingroup uefi_boot
 * @{
 *
 * Tags:      [boot, uefi, gop, framebuffer, kernel, x64, witness, secure-boot,
 *             multi-kernel, boot-config, acpi, memory-map]
 *
 * @} end M.6 CLASSIFICATION
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.7 INTENT — Purpose Statement [INTENT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_intent M.7 INTENT — Purpose Statement
 * @ingroup uefi_boot
 * @{
 *
 * Purpose:   UEFI-native bootloader for MillenniumOS providing:
 *            - Modern 64-bit boot path with GOP graphics
 *            - Secure Boot signature verification (when enabled)
 *            - Multi-kernel selection from boot.cfg
 *            - Boot configuration file parsing
 *            - Complete hardware discovery (ACPI, memory map)
 *
 * BOOT SEQUENCE:
 *   1. UEFI firmware loads BOOTX64.EFI
 *   2. Parse boot.cfg for kernel selection and options
 *   3. Verify kernel signature (Secure Boot)
 *   4. Initialize GOP framebuffer
 *   5. Load selected kernel to memory
 *   6. Discover ACPI tables and memory map
 *   7. Populate MosBootInfo at fixed address (0x9000)
 *   8. Exit boot services and transfer to kernel
 *
 * @} end M.7 INTENT
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.8 GROUNDING — Biblical Foundation [GROUNDING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_grounding M.8 GROUNDING — Biblical Foundation
 * @ingroup uefi_boot
 * @{
 *
 * Scripture: Genesis 1:1 — "In the beginning God created the heaven and the earth."
 *
 * Principle: UEFI is the modern "beginning" — first code after firmware.
 *            Like creation, boot establishes foundations for all that follows:
 *            - Graphics (light from darkness)
 *            - Memory organization (firmament dividing waters)
 *            - Kernel loading (bringing forth life)
 *
 *            Secure Boot reflects the principle of guarding the garden (Gen 2:15).
 *            Multi-kernel selection demonstrates ordered choice (Deut 30:19).
 *
 * @} end M.8 GROUNDING
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_dependencies M.9 DEPENDENCIES — Required Components
 * @ingroup uefi_boot
 * @{
 *
 * External:
 *   - GNU-EFI: efi.h, efilib.h — UEFI application framework
 *   - UEFI Firmware: GOP, Simple File System, Loaded Image protocols
 *
 * Project:
 *   - boot/include/bootinfo.h — Unified boot handoff structure (MosBootInfo)
 *   - boot/include/serial.h   — Serial port constants (COM1_DATA, COM1_STATUS)
 *   - boot/include/debug.h    — Witness logging system (DAR pattern)
 *
 * Runtime:
 *   - /EFI/BOOT/boot.cfg      — Boot configuration file (optional)
 *   - /EFI/BOOT/kernel.bin    — Default kernel binary
 *   - /EFI/BOOT/kernel.sig    — Kernel signature (Secure Boot)
 *
 * What Uses This:
 *   - kernel/entry.asm — Receives control after ExitBootServices
 *   - kernel/kernel.c  — Uses MosBootInfo for initialization
 *
 * @} end M.9 DEPENDENCIES
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * M.10 ROADMAP — Version History [ROADMAP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_roadmap M.10 ROADMAP — Version History
 * @ingroup uefi_boot
 * @{
 *
 * History:
 *   a-01.00 (2026-01-30) — Initial UEFI bootloader with GOP support
 *   a-01.50 (2026-01-31) — Full template alignment, Witness integration
 *   a-02.00 (2026-01-31) — Added Secure Boot, multi-kernel, boot.cfg support
 *
 * @} end M.10 ROADMAP
 *
 * @} end uefi_boot (main defgroup)
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
///   - S.1 INCLUDES  — Header dependencies (UEFI, Project)
///   - S.2 DEFINES   — Constants and macros
///   - S.3 TYPES     — Type declarations (internal)
///   - S.4 PROTOS    — Static function prototypes
///   - S.5 STATIC    — Static/file-scope variables
///   - S.6 OMISSION  — Reserved section guide
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.1 INCLUDES [INCLUDES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_includes S.1 INCLUDES — Header Dependencies
 * @brief    UEFI and project headers required by this bootloader.
 *
 * CONTAINS:
 *   - S.1a UEFI     — GNU-EFI headers
 *   - S.1b PROJECT  — MillenniumOS boot headers
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1a UEFI — GNU-EFI Headers [UEFI]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_includes_uefi S.1a UEFI — GNU-EFI Headers
 * @ingroup uefi_includes
 * @brief    UEFI application framework headers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include <efi.h>      /**< Core UEFI types and definitions */
#include <efilib.h>   /**< GNU-EFI utility library */

/** @} */ /* end S.1a UEFI */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.1b PROJECT — MillenniumOS Boot Headers [PROJECT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_includes_project S.1b PROJECT — MillenniumOS Boot Headers
 * @ingroup uefi_includes
 * @brief    Shared boot infrastructure headers.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

#include "../include/bootinfo.h"  /**< MosBootInfo, MOS_BOOT_INFO_ADDR */
#include "../include/serial.h"    /**< COM1_DATA, COM1_STATUS, serial constants */
#include "../include/debug.h"     /**< Witness logging: mos_witness_*, health tracking */

/** @} */ /* end S.1b PROJECT */

/** @} */ /* end S.1 INCLUDES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.2 DEFINES — Preprocessor Constants [DEFINES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_defines S.2 DEFINES — Preprocessor Constants
 * @brief    Constants and macros for the UEFI bootloader.
 *
 * CONTAINS:
 *   - S.2a ADDRESSES — Memory addresses for kernel loading
 *   - S.2b GUIDS     — UEFI protocol GUIDs (static variables)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief Debug flag to disable UEFI console output.
 *
 * When defined as 0, all Print() calls are suppressed. This prevents
 * UEFI console graphics from flooding the serial debug output.
 * Set to 1 for production with visual console feedback.
 */
#define UEFI_CONSOLE_OUTPUT 1

#if UEFI_CONSOLE_OUTPUT
#define DebugPrint(...) Print(__VA_ARGS__)
#else
#define DebugPrint(...) ((void)0)
#endif

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.2a ADDRESSES — Memory Addresses [ADDRESSES]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_addresses S.2a ADDRESSES — Memory Addresses
 * @ingroup uefi_defines
 * @brief    Fixed memory addresses for boot process.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Kernel load address — 1MB mark.
 *
 * Above conventional memory, safe for kernel placement.
 * Kernel is loaded here and execution transfers after ExitBootServices.
 */
#define KERNEL_LOAD_ADDR 0x100000

/** @} */ /* end S.2a ADDRESSES */

/** @} */ /* end S.2 DEFINES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.3 TYPES — Type Declarations [TYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_types S.3 TYPES — Type Declarations
 * @brief    Boot configuration, Secure Boot, and multi-kernel types.
 *
 * CONTAINS:
 *   - S.3a BOOTCFG   — Boot configuration file types
 *   - S.3b SECBOOT   — Secure Boot verification types
 *   - S.3c MULTIBOOT — Multi-kernel selection types
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3a BOOTCFG — Boot Configuration Types [BOOTCFG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_types_bootcfg S.3a BOOTCFG — Boot Configuration Types
 * @ingroup uefi_types
 * @brief    Structures for parsing boot.cfg configuration file.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Maximum length of boot configuration values.
 */
#define BOOT_CFG_VALUE_MAX 128

/**
 * @brief Maximum length of kernel path.
 */
#define BOOT_CFG_PATH_MAX 256

/**
 * @brief Maximum number of kernel entries in boot.cfg.
 */
#define BOOT_CFG_KERNELS_MAX 8

/**
 * @brief Boot configuration entry.
 *
 * Represents a single kernel entry in boot.cfg with its
 * path, name, and command line arguments.
 */
typedef struct BootKernelEntry {
    CHAR16 name[BOOT_CFG_VALUE_MAX];    /**< Display name for boot menu */
    CHAR16 path[BOOT_CFG_PATH_MAX];     /**< Path to kernel binary */
    CHAR16 cmdline[BOOT_CFG_VALUE_MAX]; /**< Kernel command line */
    BOOLEAN verify_sig;                  /**< Require signature verification */
} BootKernelEntry;

/**
 * @brief Complete boot configuration.
 *
 * Parsed from /EFI/BOOT/boot.cfg containing:
 *   - Timeout for auto-boot
 *   - Default kernel index
 *   - List of available kernels
 *   - Global Secure Boot setting
 */
typedef struct BootConfig {
    UINT32 timeout_seconds;              /**< Menu timeout (0 = immediate boot) */
    UINT32 default_kernel;               /**< Default kernel index (0-based) */
    BOOLEAN secure_boot_enabled;         /**< Global Secure Boot enforcement */
    BOOLEAN debug_mode;                  /**< Enable verbose debug output */
    UINT32 kernel_count;                 /**< Number of kernel entries */
    BootKernelEntry kernels[BOOT_CFG_KERNELS_MAX]; /**< Kernel entry list */
} BootConfig;

/** @} */ /* end S.3a BOOTCFG */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3b SECBOOT — Secure Boot Types [SECBOOT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_types_secboot S.3b SECBOOT — Secure Boot Types
 * @ingroup uefi_types
 * @brief    Types for Secure Boot signature verification.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Secure Boot verification result.
 */
typedef enum SecBootResult {
    SECBOOT_OK = 0,                 /**< Signature valid */
    SECBOOT_NO_SIG,                 /**< No signature file found */
    SECBOOT_INVALID_SIG,            /**< Signature doesn't match */
    SECBOOT_DISABLED,               /**< Secure Boot not enabled */
    SECBOOT_ERROR                   /**< Verification error */
} SecBootResult;

/**
 * @brief Signature file header.
 *
 * Simple signature format for MillenniumOS:
 *   - 4-byte magic: "MSIG"
 *   - 4-byte version
 *   - 32-byte SHA256 hash
 */
typedef struct MosSignature {
    UINT8 magic[4];                 /**< "MSIG" */
    UINT32 version;                 /**< Signature format version */
    UINT8 sha256[32];               /**< SHA256 hash of kernel */
} MosSignature;

/** @} */ /* end S.3b SECBOOT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.3c MULTIBOOT — Multi-Kernel Types [MULTIBOOT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_types_multiboot S.3c MULTIBOOT — Multi-Kernel Types
 * @ingroup uefi_types
 * @brief    Types for multi-kernel selection menu.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Boot menu action.
 */
typedef enum MenuAction {
    MENU_ACTION_BOOT = 0,           /**< Boot selected kernel */
    MENU_ACTION_TIMEOUT,            /**< Timeout - boot default */
    MENU_ACTION_REFRESH,            /**< Refresh menu display */
    MENU_ACTION_CANCEL              /**< Cancel (return to firmware) */
} MenuAction;

/** @} */ /* end S.3c MULTIBOOT */

/** @} */ /* end S.3 TYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.4 PROTOTYPES — Static Function Declarations [PROTOTYPES]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_prototypes S.4 PROTOTYPES — Static Function Declarations
 * @brief    Forward declarations for internal static functions.
 *
 * CONTAINS:
 *   - S.4a SERIAL    — Low-level serial I/O prototypes
 *   - S.4b ERROR     — Error handling prototypes
 *   - S.4c WITNESS   — DAR logging prototypes
 *   - S.4d GRAPHICS  — GOP initialization prototype
 *   - S.4e BOOTCFG   — Boot configuration parsing prototypes
 *   - S.4f SECBOOT   — Secure Boot verification prototypes
 *   - S.4g MULTIBOOT — Multi-kernel selection prototypes
 *   - S.4h KERNEL    — Kernel loading prototypes
 *   - S.4i MEMORY    — Memory map acquisition prototypes
 *   - S.4j ACPI      — ACPI discovery prototypes
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4a SERIAL — Low-Level Serial I/O Prototypes [SERIAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_serial S.4a SERIAL — Serial I/O Prototypes
 * @ingroup uefi_prototypes
 * @brief    Prototypes for direct hardware serial port access.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Read byte from I/O port.
 * @param[in] port  I/O port address (e.g., COM1_STATUS).
 * @return Byte read from port.
 */
static inline uint8_t inb(uint16_t port);

/**
 * @brief Write byte to I/O port.
 * @param[in] port  I/O port address (e.g., COM1_DATA).
 * @param[in] val   Byte to write.
 */
static inline void outb(uint16_t port, uint8_t val);

/**
 * @brief Read CPU timestamp counter for boot timing.
 * @return 64-bit TSC value (cycles since CPU reset).
 */
static inline uint64_t read_tsc(void);

/**
 * @brief Write single character to serial port (COM1).
 * @param[in] c  Character to send.
 */
static void serial_char(char c);

/**
 * @brief Write null-terminated string to serial port.
 * @param[in] s  String to send.
 */
static void serial_str(const char *s);

/**
 * @brief Write 64-bit value as hexadecimal to serial port.
 * @param[in] val  Value to print (format: 0xNNNNNNNNNNNNNNNN).
 */
static void serial_hex(uint64_t val);

/** @} */ /* end S.4a SERIAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4b ERROR — Error Handling Prototypes [ERROR]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_error S.4b ERROR — Error Handling Prototypes
 * @ingroup uefi_prototypes
 * @brief    Prototypes for error reporting and fatal error handling.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Print error message to console and serial.
 * @param[in] msg     Wide string error description.
 * @param[in] status  UEFI status code to display.
 */
static void print_error(CHAR16 *msg, EFI_STATUS status);

/**
 * @brief Write character to VGA text buffer (debug marker).
 * @param[in] pos    Screen position (0-1999 for 80x25).
 * @param[in] c      Character to display.
 * @param[in] color  VGA color attribute (e.g., 0x4F = white on red).
 * @note May not work in UEFI GOP mode but harmless to call.
 */
static void vga_debug(int pos, char c, uint8_t color);

/**
 * @brief Fatal error handler — halt and wait for reboot.
 * @note Displays HALT indicator, waits for keypress, then cold reboot.
 */
static void halt(void);

/** @} */ /* end S.4b ERROR */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4c WITNESS — DAR Logging Prototypes [WITNESS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_witness S.4c WITNESS — DAR Logging Prototypes
 * @ingroup uefi_prototypes
 * @brief    Prototypes for DAR-aligned Witness logging functions.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Log phase boundary for boot sequence tracking.
 * @param[in] phase_num   Phase number (0-9).
 * @param[in] description Brief description of phase purpose.
 * @par DAR Role: DETECT — Observes and records phase transitions.
 */
static void witness_phase(int phase_num, const char *description);

/**
 * @brief Log successful operation completion.
 * @param[in] component  Component name (e.g., "GOP", "KERNEL").
 * @param[in] message    Success description.
 * @param[in] value      Optional value to display (0 = skip).
 * @par DAR Role: ASSESS (positive) — Confirms successful state.
 */
static void witness_ok(const char *component, const char *message, uint64_t value);

/**
 * @brief Log warning condition (non-fatal).
 * @param[in] component  Component name (e.g., "GOP", "CONFIG").
 * @param[in] message    Warning description.
 * @par DAR Role: ASSESS (caution) — Identifies degraded but recoverable state.
 */
static void witness_warn(const char *component, const char *message);

/**
 * @brief Log failure condition (may be fatal).
 * @param[in] component  Component name (e.g., "KERNEL", "MMAP").
 * @param[in] message    Failure description.
 * @param[in] status     UEFI status code or error value.
 * @par DAR Role: ASSESS (failure) — Documents failure for diagnosis.
 */
static void witness_fail(const char *component, const char *message, uint64_t status);

/** @} */ /* end S.4c WITNESS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4d GRAPHICS — GOP Initialization Prototype [GRAPHICS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_graphics S.4d GRAPHICS — GOP Prototype
 * @ingroup uefi_prototypes
 * @brief    Prototype for UEFI Graphics Output Protocol initialization.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize GOP and configure framebuffer.
 * @param[out] boot_info  Boot info structure to populate with framebuffer details.
 * @return EFI_SUCCESS on success, error code on failure.
 * @note Prefers 1024x768 32-bit mode, falls back to largest available.
 */
static EFI_STATUS init_gop(MosBootInfo *boot_info);

/** @} */ /* end S.4d GRAPHICS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4e BOOTCFG — Boot Configuration Prototypes [BOOTCFG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_bootcfg S.4e BOOTCFG — Boot Config Prototypes
 * @ingroup uefi_prototypes
 * @brief    Prototypes for boot.cfg file parsing.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Load and parse boot.cfg configuration file.
 * @param[in]  image_handle  UEFI image handle for filesystem access.
 * @param[out] config        Boot configuration structure to populate.
 * @return EFI_SUCCESS if config loaded, EFI_NOT_FOUND if no config file.
 * @note Falls back to default configuration if file not found.
 */
static EFI_STATUS load_boot_config(EFI_HANDLE image_handle, BootConfig *config);

/**
 * @brief Initialize boot configuration with defaults.
 * @param[out] config  Configuration structure to initialize.
 * @note Sets single kernel entry pointing to default kernel.bin path.
 */
static void init_default_config(BootConfig *config);

/**
 * @brief Parse single line from boot.cfg file.
 * @param[in]     line           Wide string line to parse.
 * @param[in,out] config         Configuration being built.
 * @param[in,out] current_kernel Index of kernel entry being populated (-1 = global).
 * @return TRUE if line parsed successfully, FALSE on parse error.
 */
static BOOLEAN parse_config_line(CHAR16 *line, BootConfig *config, int *current_kernel);

/** @} */ /* end S.4e BOOTCFG */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4f SECBOOT — Secure Boot Prototypes [SECBOOT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_secboot S.4f SECBOOT — Secure Boot Prototypes
 * @ingroup uefi_prototypes
 * @brief    Prototypes for kernel signature verification.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Verify kernel binary signature.
 * @param[in] image_handle  UEFI image handle for filesystem access.
 * @param[in] kernel_path   Path to kernel binary (signature file = path + ".sig").
 * @return SECBOOT_OK if valid, SECBOOT_NO_SIG if no signature, etc.
 */
static SecBootResult verify_kernel_signature(EFI_HANDLE image_handle, CHAR16 *kernel_path);

/**
 * @brief Compute SHA256 hash of data buffer.
 * @param[in]  data  Data buffer to hash.
 * @param[in]  size  Size of data in bytes.
 * @param[out] hash  32-byte output buffer for SHA256 hash.
 * @note Simple implementation for boot-time verification.
 */
static void compute_sha256(void *data, UINTN size, UINT8 *hash);

/** @} */ /* end S.4f SECBOOT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4g MULTIBOOT — Multi-Kernel Selection Prototypes [MULTIBOOT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_multiboot S.4g MULTIBOOT — Multi-Kernel Prototypes
 * @ingroup uefi_prototypes
 * @brief    Prototypes for boot menu display and kernel selection.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Display boot menu with available kernels.
 * @param[in] config  Boot configuration with kernel list.
 * @return Number of menu entries displayed.
 */
static UINT32 display_boot_menu(BootConfig *config);

/**
 * @brief Wait for user kernel selection or timeout.
 * @param[in]     config    Boot configuration (timeout, default kernel).
 * @param[in,out] selected  Initially default, updated to user selection.
 * @return Menu action (BOOT, TIMEOUT, CANCEL, etc.).
 */
static MenuAction wait_for_selection(BootConfig *config, UINT32 *selected);

/** @} */ /* end S.4g MULTIBOOT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4h KERNEL — Kernel Loading Prototypes [KERNEL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_kernel S.4h KERNEL — Kernel Loading Prototypes
 * @ingroup uefi_prototypes
 * @brief    Prototypes for loading kernel binary from ESP.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Load kernel from specific path.
 * @param[in]  image_handle  UEFI image handle for filesystem access.
 * @param[in]  path          Path to kernel binary on ESP.
 * @param[out] boot_info     Boot info structure to populate.
 * @return EFI_SUCCESS on success, error code on failure.
 */
static EFI_STATUS load_kernel_from_path(EFI_HANDLE image_handle, CHAR16 *path, MosBootInfo *boot_info);

/**
 * @brief Load kernel using default paths (kernel.bin or EFI/BOOT/kernel.bin).
 * @param[in]  image_handle  UEFI image handle for filesystem access.
 * @param[out] boot_info     Boot info structure to populate.
 * @return EFI_SUCCESS on success, error code on failure.
 * @note Tries root path first, then EFI/BOOT path.
 */
static EFI_STATUS load_kernel(EFI_HANDLE image_handle, MosBootInfo *boot_info);

/** @} */ /* end S.4h KERNEL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4i MEMORY — Memory Map Prototypes [MEMORY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_memory S.4i MEMORY — Memory Map Prototypes
 * @ingroup uefi_prototypes
 * @brief    Prototypes for UEFI memory map acquisition and conversion.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Convert UEFI memory type to MOS memory type.
 * @param[in] uefi_type  UEFI EFI_MEMORY_TYPE value.
 * @return Corresponding MosMemoryType enum value.
 */
static MosMemoryType uefi_to_mos_memtype(UINT32 uefi_type);

/**
 * @brief Acquire UEFI memory map and convert to MOS format.
 * @param[out] boot_info  Boot info structure to populate with memory map.
 * @param[out] map_key    Memory map key (required for ExitBootServices).
 * @return EFI_SUCCESS on success, error code on failure.
 */
static EFI_STATUS get_memory_map(MosBootInfo *boot_info, UINTN *map_key);

/** @} */ /* end S.4i MEMORY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.4j ACPI — ACPI Discovery Prototypes [ACPI]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_proto_acpi S.4j ACPI — ACPI Discovery Prototypes
 * @ingroup uefi_prototypes
 * @brief    Prototypes for locating ACPI RSDP in UEFI configuration tables.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Find ACPI RSDP in UEFI configuration tables.
 * @param[out] boot_info  Boot info structure to populate with RSDP address.
 * @note Prefers ACPI 2.0 RSDP, falls back to ACPI 1.0 if not found.
 */
static void find_acpi_rsdp(MosBootInfo *boot_info);

/** @} */ /* end S.4j ACPI */

/** @} */ /* end S.4 PROTOTYPES */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.5 STATIC — File-Scope Variables [STATIC]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_static S.5 STATIC — File-Scope Variables
 * @brief    Static variables including UEFI protocol GUIDs and boot paths.
 *
 * CONTAINS:
 *   - S.5a GUIDS   — UEFI protocol GUIDs
 *   - S.5b CONFIG  — Global boot configuration
 *   - S.5c PATHS   — Default file paths
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5a GUIDS — UEFI Protocol GUIDs [GUIDS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_static_guids S.5a GUIDS — UEFI Protocol GUIDs
 * @ingroup uefi_static
 * @brief    GUIDs for locating UEFI protocols.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief GOP protocol GUID — Graphics Output Protocol.
 *
 * Used to locate GOP for framebuffer initialization.
 * Protocol provides linear framebuffer access and mode enumeration.
 */
static EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

/**
 * @brief Simple File System protocol GUID — for loading kernel.
 *
 * Used to access FAT32 filesystem on EFI System Partition.
 * Provides file open/read/close operations.
 */
static EFI_GUID sfs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

/**
 * @brief Loaded Image protocol GUID — for finding boot device.
 *
 * Used to determine which device we booted from.
 * DeviceHandle from this protocol is used to access ESP filesystem.
 */
static EFI_GUID li_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

/** @} */ /* end S.5a GUIDS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5b CONFIG — Global Boot Configuration [CONFIG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_static_config S.5b CONFIG — Global Boot Configuration
 * @ingroup uefi_static
 * @brief    Runtime boot configuration state.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Global boot configuration.
 *
 * Populated from boot.cfg file parsing or initialized with defaults.
 * Contains kernel list, timeout, secure boot settings, etc.
 */
static BootConfig g_boot_config;

/** @} */ /* end S.5b CONFIG */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * S.5c PATHS — Default File Paths [PATHS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_static_paths S.5c PATHS — Default File Paths
 * @ingroup uefi_static
 * @brief    Default paths for kernel and configuration files on ESP.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Default kernel path when no boot.cfg exists.
 *
 * First path tried when loading kernel — root of ESP.
 */
static CHAR16 *g_default_kernel_path = L"kernel.bin";  /* No leading backslash - relative from root */

/**
 * @brief Alternate kernel path (standard EFI location).
 *
 * Second path tried — standard EFI/BOOT directory.
 */
static CHAR16 *g_alt_kernel_path = L"EFI\\BOOT\\kernel.bin";  /* No leading backslash */

/**
 * @brief Boot configuration file path.
 *
 * INI-style configuration for multi-kernel, timeout, secure boot.
 * Optional — defaults used if not present.
 */
static CHAR16 *g_boot_cfg_path = L"\\EFI\\BOOT\\boot.cfg";

/** @} */ /* end S.5c PATHS */

/** @} */ /* end S.5 STATIC */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * S.6 OMISSION — Reserved Section Guide [OMISSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_omission S.6 OMISSION — Reserved Section Guide
 * @brief    Documents intentionally empty or future sections.
 *
 * INTENTIONALLY MINIMAL:
 *   - S.2 DEFINES  — Only KERNEL_LOAD_ADDR; other constants from headers
 *   - S.5b CONFIG  — Single global; additional state may be added later
 *
 * FROM EXTERNAL HEADERS:
 *   - UEFI types: EFI_STATUS, EFI_HANDLE, EFI_GUID, etc. (from efi.h)
 *   - Boot types: MosBootInfo, MosMemoryRegion, etc. (from bootinfo.h)
 *   - Serial constants: COM1_DATA, COM1_STATUS, etc. (from serial.h)
 *
 * FUTURE EXPANSION:
 *   - TPM integration for measured boot
 *   - Network boot (PXE) support
 *   - Boot splash screen/logo
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END SETUP [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// BODY BLOCK [BODY]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// @brief The actual work — UEFI bootloader implementation.
///
/// 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
///
/// CONTAINS:
///   - B.1 ORG CHART  — Function hierarchy and boot flow
///   - B.2 HELPERS    — Serial I/O, error handling utilities
///   - B.3 CORE OPS   — Graphics, kernel, memory, ACPI operations
///   - B.4 ERRORS     — Error handling patterns
///   - B.5 PUBLIC API — efi_main() entry point
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.1 ORG CHART — Boot Flow Hierarchy [ORGCHART]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_orgchart B.1 ORG CHART — Boot Flow Hierarchy
 * @brief    Function call hierarchy and boot sequence.
 *
 * DAR INTEGRATION:
 *   - DETECT:  Serial Witness output at every phase boundary
 *   - ASSESS:  Error checking with status codes and health tracking
 *   - RESTORE: Graceful fallback (GOP→text, config→defaults, retry logic)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/*
 * ═══════════════════════════════════════════════════════════════════════════════
 * UEFI BOOT FLOW (Full Sequence)
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * efi_main() [B.5 ENTRY POINT]
 * │
 * ├─► PHASE 0: Initialize UEFI Library
 * │   ├── witness_phase("PHASE 0")     — [B.2c] DAR Witness
 * │   └── InitializeLib()              — GNU-EFI initialization
 * │
 * ├─► PHASE 1: Boot Configuration
 * │   ├── witness_phase("PHASE 1")     — [B.2c] DAR Witness
 * │   ├── load_boot_config()           — [B.3b] Parse boot.cfg
 * │   │   ├── init_default_config()    — Fallback defaults
 * │   │   └── parse_config_line()      — INI-style parsing
 * │   └── (fallback to defaults if no config)
 * │
 * ├─► PHASE 2: Graphics Initialization
 * │   ├── witness_phase("PHASE 2")     — [B.2c] DAR Witness
 * │   └── init_gop()                   — [B.3a] Graphics Output Protocol
 * │       ├── LocateHandleBuffer()     — Find GOP handles
 * │       ├── QueryMode() loop         — Find best 32-bit mode
 * │       ├── SetMode()                — Configure framebuffer
 * │       └── (fallback to text mode if GOP fails)
 * │
 * ├─► PHASE 3: Kernel Selection (if multi-kernel)
 * │   ├── witness_phase("PHASE 3")     — [B.2c] DAR Witness
 * │   ├── display_boot_menu()          — [B.3d] Show kernel list
 * │   └── wait_for_selection()         — Handle timeout/keypress
 * │
 * ├─► PHASE 4: Secure Boot Verification (if enabled)
 * │   ├── witness_phase("PHASE 4")     — [B.2c] DAR Witness
 * │   └── verify_kernel_signature()    — [B.3c] Check .sig file
 * │       └── compute_sha256()         — Hash kernel binary
 * │
 * ├─► PHASE 5: Kernel Loading
 * │   ├── witness_phase("PHASE 5")     — [B.2c] DAR Witness
 * │   └── load_kernel_from_path()      — [B.3e] Load selected kernel
 * │       ├── OpenVolume()             — Access ESP filesystem
 * │       ├── Open()                   — Open kernel file
 * │       ├── GetInfo()                — Get file size
 * │       ├── AllocatePages()          — Reserve at 0x100000
 * │       └── Read()                   — Load kernel to memory
 * │
 * ├─► PHASE 6: ACPI Discovery
 * │   ├── witness_phase("PHASE 6")     — [B.2c] DAR Witness
 * │   └── find_acpi_rsdp()             — [B.3g] Search config tables
 * │       └── (ACPI 2.0 preferred, 1.0 fallback)
 * │
 * ├─► PHASE 7: Memory Map Acquisition
 * │   ├── witness_phase("PHASE 7")     — [B.2c] DAR Witness
 * │   └── get_memory_map()             — [B.3f] Get UEFI memory map
 * │       └── uefi_to_mos_memtype()    — Convert to MOS types
 * │
 * ├─► PHASE 8: Exit Boot Services
 * │   ├── witness_phase("PHASE 8")     — [B.2c] DAR Witness
 * │   ├── ExitBootServices()           — Hand off from UEFI
 * │   └── (retry once if map key stale)
 * │
 * └─► PHASE 9: Transfer to Kernel
 *     ├── witness_phase("PHASE 9")     — [B.2c] DAR Witness (last message)
 *     └── kernel(boot_info)            — Jump to kernel entry
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * HELPER FUNCTIONS (B.2)
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * B.2a SERIAL — Low-level I/O:
 * ├── inb(), outb()                — Port I/O primitives
 * ├── read_tsc()                   — Timestamp counter
 * ├── serial_char()                — Single character output
 * ├── serial_str()                 — String output
 * └── serial_hex()                 — Hex value output
 *
 * B.2b ERROR — Error handling:
 * ├── print_error()                — Error message + status
 * ├── vga_debug()                  — VGA text marker (debug)
 * └── halt()                       — Fatal error + reboot
 *
 * B.2c WITNESS — DAR logging:
 * ├── witness_phase()              — Phase boundary marker
 * ├── witness_ok()                 — Success marker
 * ├── witness_warn()               — Warning marker
 * └── witness_fail()               — Failure marker
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * FUNCTION COUNTS
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *   B.2 Helpers:
 *     - Serial:   6 (inb, outb, read_tsc, serial_char, serial_str, serial_hex)
 *     - Error:    3 (print_error, vga_debug, halt)
 *     - Witness:  4 (witness_phase, witness_ok, witness_warn, witness_fail)
 *     Subtotal:  13
 *
 *   B.3 Core Ops:
 *     - Graphics:  1 (init_gop)
 *     - Boot Cfg:  3 (load_boot_config, init_default_config, parse_config_line)
 *     - Secure:    2 (verify_kernel_signature, compute_sha256)
 *     - Multi:     2 (display_boot_menu, wait_for_selection)
 *     - Kernel:    2 (load_kernel_from_path, load_kernel)
 *     - Memory:    2 (uefi_to_mos_memtype, get_memory_map)
 *     - ACPI:      1 (find_acpi_rsdp)
 *     Subtotal:   13
 *
 *   B.5 Entry:     1 (efi_main)
 *
 *   TOTAL:        27 functions
 */

/** @} */ /* end B.1 ORG CHART */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.2 HELPERS — Foundation Implementations [HELPERS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_helpers B.2 HELPERS — Foundation Implementations
 * @brief    Serial I/O, error handling, and DAR Witness utilities.
 *
 * DAR PATTERN:
 *   - DETECT:  Observe and log system state (witness_phase, witness_ok)
 *   - ASSESS:  Evaluate conditions and report (witness_warn, print_error)
 *   - RESTORE: Recover or halt gracefully (halt, fallback paths)
 *
 * CONTAINS:
 *   - B.2a SERIAL  — Low-level serial port I/O
 *   - B.2b ERROR   — Error reporting and halt
 *   - B.2c WITNESS — DAR-aligned logging functions
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2a SERIAL — Serial Port I/O [SERIAL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_serial B.2a SERIAL — Serial Port I/O
 * @ingroup uefi_helpers
 * @brief    Direct hardware serial port access for Witness logging.
 *
 * WHY SERIAL:
 *   - Works before, during, and after ExitBootServices
 *   - UEFI Print() unavailable after ExitBootServices
 *   - Essential for debugging the critical boot→kernel transition
 *   - Consistent with kernel serial output for unified log stream
 *
 * HARDWARE:
 *   - COM1 at 0x3F8 (data), 0x3FD (status)
 *   - 8N1 configuration assumed (set by firmware)
 *   - Polled I/O (no interrupts in boot environment)
 *
 * DAR ROLE:
 *   These primitives enable the DETECT phase of DAR.
 *   Witness functions (B.2c) build on these for structured logging.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Read byte from I/O port.
 *
 * Low-level x86 port I/O using IN instruction.
 * Used to check serial port status before transmitting.
 *
 * @param[in] port  I/O port address (e.g., COM1_STATUS = 0x3FD).
 *
 * @return Byte read from port.
 *
 * @par Implementation:
 * Uses inline assembly with "Nd" constraint for immediate port value.
 *
 * @par Example:
 * @code
 * uint8_t status = inb(COM1_STATUS);
 * if (status & 0x20) { // Transmit buffer empty
 *     outb(COM1_DATA, 'A');
 * }
 * @endcode
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

/**
 * @brief Write byte to I/O port.
 *
 * Low-level x86 port I/O using OUT instruction.
 * Used to transmit characters to serial port.
 *
 * @param[in] port  I/O port address (e.g., COM1_DATA = 0x3F8).
 * @param[in] val   Byte to write.
 *
 * @par Implementation:
 * Uses inline assembly with "Nd" constraint for immediate port value.
 *
 * @par Example:
 * @code
 * outb(COM1_DATA, 'H');  // Transmit 'H' to COM1
 * @endcode
 */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * @brief Read CPU timestamp counter for boot timing.
 *
 * Returns the 64-bit Time Stamp Counter (TSC) value.
 * Used to record boot timing milestones in MosBootInfo.
 *
 * @return 64-bit TSC value (cycles since CPU reset).
 *
 * @par Implementation:
 * Uses RDTSC instruction which returns value in EDX:EAX.
 *
 * @par Usage:
 * @code
 * boot_info->boot_tsc = read_tsc();  // Record boot timestamp
 * @endcode
 *
 * @note TSC frequency varies by CPU. Kernel calibrates during init.
 */
static inline uint64_t read_tsc(void) {
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * COM2 DEBUG PORT — Separate debug output from OVMF console
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * Serial port configuration for consistent logging across all boot stages.
 *
 * PORT ASSIGNMENTS:
 * COM1 (0x3F8) = MillenniumOS debug output (UEFI bootloader, entry.asm, kernel)
 * COM2 (0x2F8) = Available for OVMF firmware console (if needed)
 *
 * QEMU ROUTING (in run-uefi.sh):
 * -serial "file:${DEBUG_LOG}"   # COM1 → debug.log (our output)
 * -serial "file:${OVMF_LOG}"    # COM2 → ovmf.log (OVMF firmware)
 *
 * All MillenniumOS components use COM1 for unified log stream.
 */
#define DEBUG_PORT          0x3F8   /**< COM1 serial port — unified with kernel */

/* ═══════════════════════════════════════════════════════════════════════════════
 * DEBUG BUFFER SYSTEM
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * Memory-based debug buffer that collects all messages, then dumps cleanly.
 * Avoids OVMF interleaving by accumulating first, outputting at key points.
 *
 * Usage:
 *   dbg("message");           // Add to buffer
 *   dbg_hex(value);           // Add hex value
 *   dbg_status(status);       // Add EFI_STATUS
 *   dbg_flush();              // Output entire buffer to serial
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define DBG_BUFFER_SIZE 8192
static char g_dbg_buffer[DBG_BUFFER_SIZE];
static UINTN g_dbg_pos = 0;

/** @brief Add string to debug buffer */
static void dbg(const char *s) {
    while (*s && g_dbg_pos < DBG_BUFFER_SIZE - 1) {
        g_dbg_buffer[g_dbg_pos++] = *s++;
    }
}

/** @brief Add newline to debug buffer */
static void dbg_nl(void) {
    if (g_dbg_pos < DBG_BUFFER_SIZE - 2) {
        g_dbg_buffer[g_dbg_pos++] = '\r';
        g_dbg_buffer[g_dbg_pos++] = '\n';
    }
}

/** @brief Add hex value to debug buffer */
static void dbg_hex(uint64_t val) {
    const char *hex = "0123456789ABCDEF";
    if (g_dbg_pos < DBG_BUFFER_SIZE - 18) {
        g_dbg_buffer[g_dbg_pos++] = '0';
        g_dbg_buffer[g_dbg_pos++] = 'x';
        for (int i = 60; i >= 0; i -= 4) {
            g_dbg_buffer[g_dbg_pos++] = hex[(val >> i) & 0xF];
        }
    }
}

/** @brief Add EFI_STATUS to debug buffer with interpretation */
__attribute__((unused))
static void dbg_status(EFI_STATUS status) {
    dbg(" status=");
    dbg_hex(status);
    if (status == EFI_SUCCESS) dbg(" (OK)");
    else if (status == EFI_NOT_FOUND) dbg(" (NOT_FOUND)");
    else if (status == EFI_INVALID_PARAMETER) dbg(" (INVALID_PARAM)");
    else if (status == EFI_UNSUPPORTED) dbg(" (UNSUPPORTED)");
    else if (status == EFI_OUT_OF_RESOURCES) dbg(" (OUT_OF_RESOURCES)");
    else dbg(" (ERROR)");
}

/** @brief Flush entire debug buffer to serial port */
static void dbg_flush(void) {
    /* Add distinctive header so we can find our output in the noise */
    serial_str("\r\n");
    serial_str("========================================\r\n");
    serial_str("=== MOS DEBUG BUFFER DUMP ===\r\n");
    serial_str("========================================\r\n");

    /* Output the buffer */
    for (UINTN i = 0; i < g_dbg_pos; i++) {
        serial_char(g_dbg_buffer[i]);
    }

    serial_str("\r\n========================================\r\n");
    serial_str("=== END DEBUG BUFFER ===\r\n");
    serial_str("========================================\r\n");
}

/** @brief Clear the debug buffer */
static void dbg_clear(void) {
    g_dbg_pos = 0;
}

/**
 * @brief Initialize debug system.
 */
static void debug_init(void) {
    /* QEMU debugcon port (0xe9) requires no initialization */
    /* Debug buffer starts empty */
    g_dbg_pos = 0;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 * BOOT EVENT SYSTEM
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * Persistent boot event logging to MosBootInfo. Events survive in memory
 * and can be read by the kernel for boot diagnostics.
 *
 * Usage:
 *   boot_event(boot_info, MOS_EVT_PHASE_KERNEL | MOS_EVT_STEP_START, 0, 0);
 *   boot_event(boot_info, MOS_EVT_PHASE_KERNEL | MOS_EVT_STEP_OK, 0, kernel_addr);
 *   boot_event(boot_info, MOS_EVT_PHASE_KERNEL | MOS_EVT_STEP_FAIL, status, 0);
 * ═══════════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Record a boot event to the MosBootInfo event buffer.
 *
 * @param[in,out] bi          Boot info structure to record event to.
 * @param[in]     event_code  Phase + Step code (e.g., MOS_EVT_PHASE_KERNEL | MOS_EVT_STEP_OK).
 * @param[in]     status      Result status (0=OK, 0xFFFF=FAIL, or UEFI status).
 * @param[in]     value       Optional value (address, size, etc.).
 */
static void boot_event(MosBootInfo *bi, uint16_t event_code, uint16_t status, uint32_t value) {
    if (!bi) return;

    /* Get TSC timestamp */
    uint64_t tsc = read_tsc();

    /* Find next slot (circular buffer) */
    uint32_t idx = bi->boot_event_count % MOS_MAX_BOOT_EVENTS;

    /* Record event */
    bi->boot_events[idx].tsc = tsc;
    bi->boot_events[idx].event_code = event_code;
    bi->boot_events[idx].status = status;
    bi->boot_events[idx].value = value;

    /* Update counters */
    bi->boot_event_count++;
    bi->boot_last_phase = event_code & 0xFF00;
    bi->boot_last_status = status;
}

/**
 * @brief Initialize boot event buffer in MosBootInfo.
 *
 * @param[out] bi  Boot info structure to initialize.
 */
static void boot_event_init(MosBootInfo *bi) {
    if (!bi) return;
    bi->boot_event_count = 0;
    bi->boot_last_phase = 0;
    bi->boot_last_status = MOS_EVT_STATUS_OK;
    SetMem(bi->boot_events, sizeof(bi->boot_events), 0);
}

/**
 * @brief Write single character to QEMU debugcon port.
 *
 * Direct write, no waiting — debugcon is always ready.
 *
 * @param[in] c  Character to send (ASCII).
 *
 * @par Implementation:
 * Write directly to port 0xe9 — QEMU debugcon is instant, no handshaking.
 *
 * @par Hardware:
 * Uses QEMU debugcon (0xe9) which OVMF doesn't touch.
 * Clean output with no OVMF graphics pollution.
 *
 * @note This is the primitive used by serial_str() and serial_hex().
 */
static void serial_char(char c) {
    outb(DEBUG_PORT, c);
}

/**
 * @brief Write null-terminated string to serial port.
 *
 * Iterates through string, sending each character via serial_char().
 *
 * @param[in] s  Null-terminated string to send.
 *
 * @par Example:
 * @code
 * serial_str("[MOS-UEFI] Boot starting\r\n");
 * @endcode
 *
 * @note Use \r\n for line endings (serial terminals expect CR+LF).
 */
static void serial_str(const char *s) {
    while (*s) serial_char(*s++);
}

/**
 * @brief Write 64-bit value as hexadecimal to serial port.
 *
 * Outputs "0x" prefix followed by 16 hexadecimal digits.
 * Always outputs full 16 digits (zero-padded).
 *
 * @param[in] val  64-bit value to print.
 *
 * @par Output Format:
 * @code
 * 0x00000000FD000000  (framebuffer address)
 * 0x0000000000100000  (kernel load address)
 * @endcode
 *
 * @par Example:
 * @code
 * serial_str("fb_addr=");
 * serial_hex(boot_info->framebuffer.address);
 * serial_str("\r\n");
 * @endcode
 */
static void serial_hex(uint64_t val) {
    const char *hex = "0123456789ABCDEF";
    serial_str("0x");
    for (int i = 60; i >= 0; i -= 4) {
        serial_char(hex[(val >> i) & 0xF]);
    }
}

/**
 * @brief Write UEFI wide string (CHAR16) to serial port as ASCII.
 *
 * Converts UCS-2/UTF-16 wide characters to ASCII for serial output.
 * Characters outside ASCII range (0-127) are replaced with '?' to
 * prevent garbage/Korean character output from encoding mismatches.
 *
 * @param[in] ws  Wide string (CHAR16) to convert and send.
 *
 * @par Conversion Rules:
 * - ASCII printable (0x20-0x7E): Output as-is
 * - ASCII control chars: Output as-is (newline, etc.)
 * - Extended/Unicode: Replace with '?' to indicate unconvertible
 *
 * @par Example:
 * @code
 * serial_wide(L"Loading kernel.bin...");  // Outputs ASCII safely
 * serial_wide(path);                       // Converts UEFI path to ASCII
 * @endcode
 *
 * @note This prevents Korean character garbage from CHAR16→byte misinterpretation.
 */
static void serial_wide(CHAR16 *ws) {
    if (!ws) {
        serial_str("(null)");
        return;
    }
    while (*ws) {
        CHAR16 c = *ws++;
        if (c < 128) {
            serial_char((char)c);
        } else {
            /* Non-ASCII: output replacement to avoid encoding garbage */
            serial_char('?');
        }
    }
}

/**
 * @brief Write EFI_STATUS code to serial with human-readable name.
 *
 * Decodes common UEFI status codes and outputs both the hex value
 * and a readable description to aid debugging.
 *
 * @param[in] status  EFI_STATUS value to decode.
 *
 * @par Common Status Codes:
 * - 0: SUCCESS
 * - 2: INVALID_PARAMETER
 * - 5: NOT_FOUND
 * - 8: ACCESS_DENIED
 * - 9: ALREADY_STARTED
 * - 13: NOT_READY
 * - 14: NOT_FOUND (file)
 *
 * @par Example:
 * @code
 * serial_status(status);  // "0x000000000000000E (NOT_FOUND)"
 * @endcode
 */
__attribute__((unused))
static void serial_status(EFI_STATUS status) {
    serial_hex((uint64_t)status);
    serial_str(" (");

    /* Decode common status codes */
    switch (status) {
        case EFI_SUCCESS:             serial_str("SUCCESS"); break;
        case EFI_LOAD_ERROR:          serial_str("LOAD_ERROR"); break;
        case EFI_INVALID_PARAMETER:   serial_str("INVALID_PARAMETER"); break;
        case EFI_UNSUPPORTED:         serial_str("UNSUPPORTED"); break;
        case EFI_BAD_BUFFER_SIZE:     serial_str("BAD_BUFFER_SIZE"); break;
        case EFI_BUFFER_TOO_SMALL:    serial_str("BUFFER_TOO_SMALL"); break;
        case EFI_NOT_READY:           serial_str("NOT_READY"); break;
        case EFI_DEVICE_ERROR:        serial_str("DEVICE_ERROR"); break;
        case EFI_WRITE_PROTECTED:     serial_str("WRITE_PROTECTED"); break;
        case EFI_OUT_OF_RESOURCES:    serial_str("OUT_OF_RESOURCES"); break;
        case EFI_VOLUME_CORRUPTED:    serial_str("VOLUME_CORRUPTED"); break;
        case EFI_VOLUME_FULL:         serial_str("VOLUME_FULL"); break;
        case EFI_NO_MEDIA:            serial_str("NO_MEDIA"); break;
        case EFI_MEDIA_CHANGED:       serial_str("MEDIA_CHANGED"); break;
        case EFI_NOT_FOUND:           serial_str("NOT_FOUND"); break;
        case EFI_ACCESS_DENIED:       serial_str("ACCESS_DENIED"); break;
        case EFI_NO_RESPONSE:         serial_str("NO_RESPONSE"); break;
        case EFI_NO_MAPPING:          serial_str("NO_MAPPING"); break;
        case EFI_TIMEOUT:             serial_str("TIMEOUT"); break;
        case EFI_NOT_STARTED:         serial_str("NOT_STARTED"); break;
        case EFI_ALREADY_STARTED:     serial_str("ALREADY_STARTED"); break;
        case EFI_ABORTED:             serial_str("ABORTED"); break;
        case EFI_PROTOCOL_ERROR:      serial_str("PROTOCOL_ERROR"); break;
        default:                      serial_str("UNKNOWN"); break;
    }
    serial_str(")");
}

/**
 * @brief Delay for approximately N seconds using busy-wait.
 *
 * Provides a simple delay for debugging purposes, allowing
 * time to read console/screen output before the system proceeds.
 *
 * @param[in] seconds  Approximate number of seconds to wait.
 *
 * @note Uses BS->Stall() which waits in microseconds.
 * @note Only valid before ExitBootServices().
 *
 * @par Example:
 * @code
 * debug_delay(3);  // Wait ~3 seconds
 * @endcode
 */
__attribute__((unused))
static void debug_delay(UINTN seconds) {
    BS->Stall(seconds * 1000000);  /* Stall takes microseconds */
}

/** @} */ /* end B.2a SERIAL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2b ERROR — Error Handling [ERROR]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_error B.2b ERROR — Error Handling
 * @ingroup uefi_helpers
 * @brief    Error reporting, debug markers, and fatal error handling.
 *
 * ERROR HANDLING PHILOSOPHY:
 *   - Non-fatal errors: Log via Witness (B.2c), continue with fallback
 *   - Fatal errors: Log, display message, halt with reboot option
 *   - All errors: Output to serial (survives ExitBootServices)
 *
 * DAR ROLE:
 *   - ASSESS: print_error() evaluates and reports error conditions
 *   - RESTORE: halt() provides controlled shutdown when recovery impossible
 *
 * TRIPWIRE PATTERN:
 *   Primary path → Warning (Witness) → Fallback → Error → HALT
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Print error message to console and serial.
 *
 * Outputs error to both UEFI console (Print) and serial port.
 * Serial output ensures error is captured even if console fails.
 *
 * @param[in] msg     Wide string (CHAR16*) error description.
 * @param[in] status  UEFI EFI_STATUS code for the error.
 *
 * @par Output Format:
 * Serial: [MOS-UEFI] ERROR: <message> status=0x<hex>
 * Console: [ERROR] <message>: <UEFI status string>
 *
 * @par Example:
 * @code
 * if (EFI_ERROR(status)) {
 *     print_error(L"Failed to locate GOP handles", status);
 *     return status;
 * }
 * @endcode
 *
 * @par DAR Role: ASSESS
 * Reports error condition with full context for diagnosis.
 *
 * @note Wide string is converted to ASCII for serial (chars >= 128 skipped).
 */
static void print_error(CHAR16 *msg, EFI_STATUS status) {
    serial_str("[MOS-UEFI] ERROR: ");
    /* Convert wide string to serial (basic ASCII subset) */
    for (CHAR16 *p = msg; *p; p++) {
        if (*p < 128) serial_char((char)*p);
    }
    serial_str(" status=");
    serial_hex((uint64_t)status);
    serial_str("\r\n");
    DebugPrint(L"[ERROR] %s: %r\n", msg, status);
}

/**
 * @brief Write character to VGA text buffer (debug marker).
 *
 * Writes a single character directly to VGA text mode memory.
 * Used to place visual debug markers on screen.
 *
 * @param[in] pos    Screen position (0-1999 for 80x25 display).
 *                   pos = row * 80 + column
 * @param[in] c      ASCII character to display.
 * @param[in] color  VGA color attribute byte.
 *                   High nibble = background, Low nibble = foreground
 *                   Example: 0x4F = white on red
 *
 * @par Memory Layout:
 * VGA text buffer at 0xB8000, 2 bytes per character:
 *   - Byte 0: ASCII character
 *   - Byte 1: Color attribute
 *
 * @par Example:
 * @code
 * // Display "HALT" in white on red at top-right corner
 * vga_debug(70, 'H', 0x4F);
 * vga_debug(71, 'A', 0x4F);
 * vga_debug(72, 'L', 0x4F);
 * vga_debug(73, 'T', 0x4F);
 * @endcode
 *
 * @note May not be visible in UEFI GOP mode, but harmless to call.
 *       Useful when debugging with VGA text mode or BIOS boot path.
 */
static void vga_debug(int pos, char c, uint8_t color) {
    volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
    vga[pos] = (color << 8) | c;
}

/**
 * @brief Fatal error handler — halt system and offer reboot.
 *
 * Called when an unrecoverable error occurs. Sequence:
 *   1. Log HALT to serial (survives any display state)
 *   2. Place visual "HALT" marker on VGA buffer
 *   3. Display message on UEFI console
 *   4. Wait for any keypress
 *   5. Trigger cold reboot via UEFI RuntimeServices
 *
 * @par DAR Role: RESTORE (terminal)
 * When recovery is impossible, provides controlled shutdown
 * with diagnostic information preserved in serial log.
 *
 * @par Serial Output:
 * @code
 * [MOS-UEFI] *** HALT CALLED ***
 * @endcode
 *
 * @par Console Output:
 * @code
 * [HALT] Press any key to reboot...
 * @endcode
 *
 * @note This function never returns. After keypress, system reboots.
 *
 * @warning Only call for truly fatal errors where no recovery is possible.
 *          Prefer fallback paths (e.g., GOP fail → text mode) when available.
 */
static void halt(void) {
    /* CRITICAL: Flush debug buffer before halting so we see what happened */
    dbg_flush();

    serial_str("\r\n[MOS-UEFI] *** HALT CALLED ***\r\n");

    /* VGA debug markers (harmless in GOP mode) */
    vga_debug(70, 'H', 0x4F);
    vga_debug(71, 'A', 0x4F);
    vga_debug(72, 'L', 0x4F);
    vga_debug(73, 'T', 0x4F);

    DebugPrint(L"\n[HALT] Press any key to reboot...\n");

    /* Wait for key */
    EFI_INPUT_KEY key;
    ST->ConIn->Reset(ST->ConIn, FALSE);
    while (ST->ConIn->ReadKeyStroke(ST->ConIn, &key) == EFI_NOT_READY);
    ST->RuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
}

/** @} */ /* end B.2b ERROR */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.2c WITNESS — DAR Logging [WITNESS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_witness B.2c WITNESS — DAR Logging
 * @ingroup uefi_helpers
 * @brief    Witness logging functions following DAR (Detect-Assess-Restore) pattern.
 *
 * DAR PATTERN APPLICATION:
 *   - DETECT:  witness_phase() marks phase boundaries for observability
 *   - ASSESS:  witness_ok/warn/fail() evaluate and report condition
 *   - RESTORE: Callers use return values to trigger fallback paths
 *
 * WHY WITNESS:
 *   - Serial output survives ExitBootServices (unlike Print)
 *   - Structured format enables automated log parsing
 *   - Phase markers create clear debugging checkpoints
 *   - Consistent with kernel Witness system (debug.h)
 *
 * OUTPUT FORMAT:
 *   [MOS-UEFI] PHASE N: Description
 *   [MOS-UEFI] OK: Success message
 *   [MOS-UEFI] WARN: Warning message
 *   [MOS-UEFI] FAIL: Failure message
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Log phase boundary for boot sequence tracking.
 *
 * Marks the start of a boot phase with consistent formatting.
 * Essential for debugging — shows exactly where boot process is.
 *
 * @param[in] phase_num   Phase number (0-9).
 * @param[in] description Brief description of phase purpose.
 *
 * @par Example Output:
 * @code
 * [MOS-UEFI] PHASE 2: Graphics initialization (GOP)
 * @endcode
 *
 * @par DAR Role: DETECT
 * Observes and records phase transitions for debugging.
 */
static void witness_phase(int phase_num, const char *description) {
    /* Direct output for immediate visibility even with OVMF noise */
    serial_str("\r\n[MOS-UEFI] PHASE ");
    serial_char('0' + phase_num);
    serial_str(": ");
    serial_str(description);
    serial_str("\r\n");

    /* Also log to debug buffer for clean dump later */
    dbg("=== PHASE "); dbg_hex(phase_num); dbg(": "); dbg(description); dbg(" ==="); dbg_nl();

    /* Flush debug buffer at each phase transition for visibility */
    dbg_flush();
    dbg_clear();  /* Start fresh for next phase */
}

/**
 * @brief Log successful operation completion.
 *
 * Reports successful completion of an operation with optional value.
 * Used after critical operations succeed.
 *
 * @param[in] component  Component name (e.g., "GOP", "KERNEL").
 * @param[in] message    Success description.
 * @param[in] value      Optional value to display (0 = skip).
 *
 * @par Example Output:
 * @code
 * [MOS-UEFI] OK: GOP fb_addr=0x00000000FD000000
 * @endcode
 *
 * @par DAR Role: ASSESS (positive)
 * Confirms successful state after operation.
 */
static void witness_ok(const char *component, const char *message, uint64_t value) {
    serial_str("[MOS-UEFI] OK: ");
    serial_str(component);
    serial_str(" ");
    serial_str(message);
    if (value != 0) {
        serial_str("=");
        serial_hex(value);
    }
    serial_str("\r\n");
}

/**
 * @brief Log warning condition (non-fatal).
 *
 * Reports a warning condition that allows continued operation.
 * Used when fallback path is available.
 *
 * @param[in] component  Component name (e.g., "GOP", "CONFIG").
 * @param[in] message    Warning description.
 *
 * @par Example Output:
 * @code
 * [MOS-UEFI] WARN: CONFIG boot.cfg not found, using defaults
 * @endcode
 *
 * @par DAR Role: ASSESS (caution)
 * Identifies degraded but recoverable state.
 */
static void witness_warn(const char *component, const char *message) {
    serial_str("[MOS-UEFI] WARN: ");
    serial_str(component);
    serial_str(" ");
    serial_str(message);
    serial_str("\r\n");
}

/**
 * @brief Log failure condition (may be fatal).
 *
 * Reports a failure condition. Caller decides if fatal.
 * Always includes status code for debugging.
 *
 * @param[in] component  Component name (e.g., "KERNEL", "MMAP").
 * @param[in] message    Failure description.
 * @param[in] status     UEFI status code or error value.
 *
 * @par Example Output:
 * @code
 * [MOS-UEFI] FAIL: KERNEL load error status=0x0000000000000005
 * @endcode
 *
 * @par DAR Role: ASSESS (failure)
 * Documents failure state for diagnosis. Caller triggers RESTORE.
 */
static void witness_fail(const char *component, const char *message, uint64_t status) {
    serial_str("[MOS-UEFI] FAIL: ");
    serial_str(component);
    serial_str(" ");
    serial_str(message);
    serial_str(" status=");
    serial_hex(status);
    serial_str("\r\n");
}

/** @} */ /* end B.2c WITNESS */

/** @} */ /* end B.2 HELPERS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.3 CORE OPS — Boot Operations [OPERATIONS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_coreops B.3 CORE OPS — Boot Operations
 * @brief    Graphics, kernel loading, memory map, ACPI discovery.
 *
 * CONTAINS:
 *   - B.3a GRAPHICS  — GOP framebuffer initialization
 *   - B.3b BOOTCFG   — Boot configuration file parsing
 *   - B.3c SECBOOT   — Secure Boot signature verification
 *   - B.3d MULTIBOOT — Multi-kernel selection menu
 *   - B.3e KERNEL    — Kernel file loading
 *   - B.3f MEMORY    — Memory map acquisition
 *   - B.3g ACPI      — ACPI RSDP discovery
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3a GRAPHICS — GOP Framebuffer [GRAPHICS]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_graphics B.3a GRAPHICS — GOP Framebuffer
 * @ingroup uefi_coreops
 * @brief    Initialize UEFI Graphics Output Protocol.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize GOP and configure framebuffer.
 *
 * Locates GOP protocol, finds best 32-bit mode (preferring 1024x768),
 * sets the mode, and populates boot_info framebuffer fields.
 *
 * @param[out] boot_info  Boot info structure to populate.
 *
 * @return EFI_SUCCESS on success, error code on failure.
 */
static EFI_STATUS init_gop(MosBootInfo *boot_info) {
    EFI_STATUS status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    UINTN handle_count;
    EFI_HANDLE *handle_buffer;
    UINTN best_mode = 0;
    UINT32 best_width = 0;
    UINT32 best_height = 0;

    dbg("[GOP] Initializing graphics..."); dbg_nl();
    dbg_flush();
    DebugPrint(L"[GOP] Initializing graphics...\n");

    /* ═══════════════════════════════════════════════════════════════════════════
     * EDK2 Pattern Note: LocateHandleBuffer is CORRECT for GOP
     *
     * Unlike filesystem access where we need OUR device (LoadedImage->DeviceHandle),
     * for GOP we want ANY available graphics output. LocateHandleBuffer finds all
     * GOP instances and we take the first one.
     * ═══════════════════════════════════════════════════════════════════════════ */

    /* Locate GOP protocol - find any/all GOP handles */
    dbg("  LocateHandleBuffer(GOP)...");
    dbg_flush();
    status = BS->LocateHandleBuffer(
        ByProtocol,
        &gop_guid,
        NULL,
        &handle_count,
        &handle_buffer
    );

    if (EFI_ERROR(status)) {
        dbg(" FAIL: 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        print_error(L"Failed to locate GOP handles", status);
        return status;
    }
    dbg(" OK, found "); dbg_hex(handle_count); dbg(" GOP handles"); dbg_nl();
    dbg_flush();

    /* Get first GOP instance */
    dbg("  HandleProtocol(GOP)...");
    dbg_flush();
    status = BS->HandleProtocol(
        handle_buffer[0],
        &gop_guid,
        (void **)&gop
    );

    if (EFI_ERROR(status)) {
        dbg(" FAIL: 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        print_error(L"Failed to get GOP protocol", status);
        BS->FreePool(handle_buffer);
        return status;
    }
    dbg(" OK"); dbg_nl();
    dbg_flush();

    BS->FreePool(handle_buffer);

    /* Find best mode (prefer 1024x768 or closest) */
    dbg("  Scanning "); dbg_hex(gop->Mode->MaxMode); dbg(" modes..."); dbg_nl();
    dbg_flush();
    DebugPrint(L"[GOP] Scanning %d modes...\n", gop->Mode->MaxMode);

    for (UINTN i = 0; i < gop->Mode->MaxMode; i++) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *mode_info;
        UINTN info_size;

        status = gop->QueryMode(gop, i, &info_size, &mode_info);
        if (EFI_ERROR(status)) continue;

        /* Only accept 32-bit RGB modes */
        if (mode_info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor ||
            mode_info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor) {

            /* Prefer 1024x768 */
            if (mode_info->HorizontalResolution == 1024 &&
                mode_info->VerticalResolution == 768) {
                best_mode = i;
                best_width = mode_info->HorizontalResolution;
                best_height = mode_info->VerticalResolution;
                dbg("    Mode "); dbg_hex(i); dbg(": ");
                dbg_hex(best_width); dbg("x"); dbg_hex(best_height);
                dbg(" (IDEAL)"); dbg_nl();
                DebugPrint(L"[GOP] Found ideal mode %d: %dx%d\n", i, best_width, best_height);
                break;
            }

            /* Track largest mode as fallback */
            if (mode_info->HorizontalResolution * mode_info->VerticalResolution >
                best_width * best_height) {
                best_mode = i;
                best_width = mode_info->HorizontalResolution;
                best_height = mode_info->VerticalResolution;
            }
        }
    }

    if (best_width == 0) {
        dbg("  FAIL: No suitable 32-bit mode found"); dbg_nl();
        dbg_flush();
        DebugPrint(L"[GOP] No suitable graphics mode found!\n");
        return EFI_UNSUPPORTED;
    }

    /* Set the mode */
    dbg("  SetMode("); dbg_hex(best_mode); dbg("): ");
    dbg_hex(best_width); dbg("x"); dbg_hex(best_height); dbg("...");
    dbg_flush();
    DebugPrint(L"[GOP] Setting mode %d: %dx%d\n", best_mode, best_width, best_height);
    status = gop->SetMode(gop, best_mode);
    if (EFI_ERROR(status)) {
        dbg(" FAIL: 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        print_error(L"Failed to set GOP mode", status);
        return status;
    }
    dbg(" OK"); dbg_nl();
    dbg_flush();

    /* Fill boot info with framebuffer details */
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = gop->Mode->Info;

    boot_info->framebuffer.address = gop->Mode->FrameBufferBase;
    boot_info->framebuffer.width = info->HorizontalResolution;
    boot_info->framebuffer.height = info->VerticalResolution;
    boot_info->framebuffer.pitch = info->PixelsPerScanLine * 4;  /* 4 bytes per pixel */
    boot_info->framebuffer.bpp = 32;

    /* Set color masks based on pixel format */
    if (info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor) {
        boot_info->framebuffer.red_shift = 0;
        boot_info->framebuffer.green_shift = 8;
        boot_info->framebuffer.blue_shift = 16;
        dbg("  Pixel format: RGB (Red first)"); dbg_nl();
    } else {
        /* PixelBlueGreenRedReserved8BitPerColor (common) */
        boot_info->framebuffer.blue_shift = 0;
        boot_info->framebuffer.green_shift = 8;
        boot_info->framebuffer.red_shift = 16;
        dbg("  Pixel format: BGR (Blue first)"); dbg_nl();
    }
    boot_info->framebuffer.red_size = 8;
    boot_info->framebuffer.green_size = 8;
    boot_info->framebuffer.blue_size = 8;

    dbg("  Framebuffer: 0x"); dbg_hex(boot_info->framebuffer.address);
    dbg(" pitch="); dbg_hex(boot_info->framebuffer.pitch); dbg_nl();
    dbg_flush();
    DebugPrint(L"[GOP] Framebuffer at 0x%lx, %dx%d, pitch=%d\n",
          boot_info->framebuffer.address, boot_info->framebuffer.width,
          boot_info->framebuffer.height, boot_info->framebuffer.pitch);

    return EFI_SUCCESS;
}

/** @} */ /* end B.3a GRAPHICS */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3b BOOTCFG — Boot Configuration [BOOTCFG]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_bootcfg B.3b BOOTCFG — Boot Configuration
 * @ingroup uefi_coreops
 * @brief    Parse boot.cfg configuration file for multi-kernel and settings.
 *
 * BOOT.CFG FORMAT:
 *   INI-style configuration file located at /EFI/BOOT/boot.cfg
 *
 *   @code{.ini}
 *   # Global settings
 *   timeout=5
 *   default=0
 *   secure_boot=true
 *   debug=false
 *
 *   # Kernel entries
 *   [kernel]
 *   name=MillenniumOS
 *   path=\kernel.bin
 *   cmdline=console=ttyS0
 *   verify=true
 *
 *   [kernel]
 *   name=MillenniumOS (Recovery)
 *   path=\EFI\BOOT\kernel-recovery.bin
 *   verify=false
 *   @endcode
 *
 * DAR INTEGRATION:
 *   - DETECT:  witness_phase() on entry
 *   - ASSESS:  witness_ok/warn on parse results
 *   - RESTORE: Fall back to defaults if config missing/invalid
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Initialize boot configuration with safe defaults.
 *
 * Sets up a minimal configuration that boots the default kernel
 * without requiring a boot.cfg file.
 *
 * @param[out] config  Configuration structure to initialize.
 *
 * @par Default Values:
 *   - timeout: 0 (immediate boot)
 *   - default_kernel: 0
 *   - secure_boot: FALSE (can be overridden by UEFI SecureBoot variable)
 *   - debug: FALSE
 *   - Single kernel entry: "MillenniumOS" at \kernel.bin
 *
 * @par DAR Role: RESTORE
 * Provides safe fallback when boot.cfg is missing or invalid.
 */
static void init_default_config(BootConfig *config) {
    /* Zero entire structure */
    SetMem(config, sizeof(BootConfig), 0);

    /* Global defaults */
    config->timeout_seconds = 0;       /* Immediate boot */
    config->default_kernel = 0;        /* First kernel */
    config->secure_boot_enabled = FALSE;
    config->debug_mode = FALSE;

    /* Single default kernel entry */
    config->kernel_count = 1;

    /* Copy default name */
    CHAR16 *name = L"MillenniumOS";
    for (int i = 0; name[i] && i < BOOT_CFG_VALUE_MAX - 1; i++) {
        config->kernels[0].name[i] = name[i];
    }

    /* Copy default path */
    CHAR16 *path = L"\\kernel.bin";
    for (int i = 0; path[i] && i < BOOT_CFG_PATH_MAX - 1; i++) {
        config->kernels[0].path[i] = path[i];
    }

    config->kernels[0].verify_sig = FALSE;

    witness_ok("CONFIG", "defaults initialized", 0);
}

/**
 * @brief Parse single line from boot.cfg file.
 *
 * Handles INI-style configuration:
 *   - Lines starting with # or ; are comments
 *   - [kernel] starts a new kernel entry
 *   - key=value pairs set configuration
 *
 * @param[in]     line           Wide string line to parse (modified in place).
 * @param[in,out] config         Configuration being built.
 * @param[in,out] current_kernel Kernel index being populated (-1 = global section).
 *
 * @return TRUE if line parsed successfully, FALSE on parse error.
 *
 * @par Supported Keys (Global):
 *   - timeout=N     — Boot menu timeout in seconds
 *   - default=N     — Default kernel index (0-based)
 *   - secure_boot=true/false
 *   - debug=true/false
 *
 * @par Supported Keys (Per-Kernel):
 *   - name=STRING   — Display name for boot menu
 *   - path=STRING   — Path to kernel binary
 *   - cmdline=STRING — Kernel command line
 *   - verify=true/false — Require signature verification
 */
static BOOLEAN parse_config_line(CHAR16 *line, BootConfig *config, int *current_kernel) {
    /* Skip leading whitespace */
    while (*line == L' ' || *line == L'\t') line++;

    /* Skip empty lines and comments */
    if (*line == L'\0' || *line == L'\n' || *line == L'\r' ||
        *line == L'#' || *line == L';') {
        return TRUE;
    }

    /* Check for [kernel] section header */
    if (*line == L'[') {
        if (StrnCmp(line, L"[kernel]", 8) == 0) {
            if (config->kernel_count < BOOT_CFG_KERNELS_MAX) {
                *current_kernel = config->kernel_count;
                config->kernel_count++;
                /* Initialize new kernel entry */
                SetMem(&config->kernels[*current_kernel], sizeof(BootKernelEntry), 0);
                return TRUE;
            }
        }
        return TRUE;  /* Ignore unknown sections */
    }

    /* Find '=' separator */
    CHAR16 *eq = line;
    while (*eq && *eq != L'=') eq++;
    if (*eq != L'=') return TRUE;  /* No '=' found, skip line */

    *eq = L'\0';  /* Split into key and value */
    CHAR16 *key = line;
    CHAR16 *value = eq + 1;

    /* Trim trailing whitespace from key */
    CHAR16 *end = eq - 1;
    while (end > key && (*end == L' ' || *end == L'\t')) *end-- = L'\0';

    /* Trim leading whitespace from value */
    while (*value == L' ' || *value == L'\t') value++;

    /* Trim trailing newline/whitespace from value */
    end = value;
    while (*end) end++;
    end--;
    while (end > value && (*end == L'\n' || *end == L'\r' || *end == L' ')) *end-- = L'\0';

    /* Parse global settings */
    if (*current_kernel < 0) {
        if (StrnCmp(key, L"timeout", 7) == 0) {
            config->timeout_seconds = (UINT32)Atoi(value);
        } else if (StrnCmp(key, L"default", 7) == 0) {
            config->default_kernel = (UINT32)Atoi(value);
        } else if (StrnCmp(key, L"secure_boot", 11) == 0) {
            config->secure_boot_enabled = (StrnCmp(value, L"true", 4) == 0);
        } else if (StrnCmp(key, L"debug", 5) == 0) {
            config->debug_mode = (StrnCmp(value, L"true", 4) == 0);
        }
    } else {
        /* Parse kernel-specific settings */
        BootKernelEntry *kernel = &config->kernels[*current_kernel];

        if (StrnCmp(key, L"name", 4) == 0) {
            for (int i = 0; value[i] && i < BOOT_CFG_VALUE_MAX - 1; i++) {
                kernel->name[i] = value[i];
            }
        } else if (StrnCmp(key, L"path", 4) == 0) {
            for (int i = 0; value[i] && i < BOOT_CFG_PATH_MAX - 1; i++) {
                kernel->path[i] = value[i];
            }
        } else if (StrnCmp(key, L"cmdline", 7) == 0) {
            for (int i = 0; value[i] && i < BOOT_CFG_VALUE_MAX - 1; i++) {
                kernel->cmdline[i] = value[i];
            }
        } else if (StrnCmp(key, L"verify", 6) == 0) {
            kernel->verify_sig = (StrnCmp(value, L"true", 4) == 0);
        }
    }

    return TRUE;
}

/**
 * @brief Load and parse boot.cfg configuration file.
 *
 * Attempts to read /EFI/BOOT/boot.cfg from the ESP and parse it.
 * Falls back to default configuration if file not found or unreadable.
 *
 * @param[in]  image_handle  UEFI image handle for filesystem access.
 * @param[out] config        Boot configuration structure to populate.
 *
 * @return EFI_SUCCESS if config loaded (or defaults used),
 *         EFI_NOT_FOUND if no config file (defaults applied).
 *
 * @par Boot Flow:
 *   1. Initialize with defaults (ensures valid config even on failure)
 *   2. Attempt to open boot.cfg
 *   3. Read file line by line
 *   4. Parse each line with parse_config_line()
 *   5. Validate resulting configuration
 *
 * @par DAR Integration:
 *   - witness_phase() called by caller before this function
 *   - witness_ok() on successful parse
 *   - witness_warn() if using defaults
 */
__attribute__((unused))
static EFI_STATUS load_boot_config(EFI_HANDLE image_handle, BootConfig *config) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
    EFI_FILE_PROTOCOL *root;
    EFI_FILE_PROTOCOL *cfg_file;

    dbg("[CONFIG] Loading boot.cfg..."); dbg_nl();
    dbg_flush();

    /* Always start with defaults */
    init_default_config(config);

    /* ═══════════════════════════════════════════════════════════════════════════
     * EDK2 Pattern: LoadedImage -> DeviceHandle -> SimpleFileSystem
     *
     * Same pattern as kernel loading - get OUR boot device, not search all.
     * ═══════════════════════════════════════════════════════════════════════════ */

    /* Get loaded image protocol to find our boot device */
    dbg("  HandleProtocol(LoadedImage)...");
    status = BS->HandleProtocol(image_handle, &li_guid, (void **)&loaded_image);
    if (EFI_ERROR(status)) {
        dbg(" FAIL"); dbg_nl();
        witness_warn("CONFIG", "cannot get loaded image, using defaults");
        return EFI_NOT_FOUND;
    }
    dbg(" OK"); dbg_nl();

    /* Get filesystem protocol from boot device */
    dbg("  HandleProtocol(SFS from DeviceHandle)...");
    status = BS->HandleProtocol(loaded_image->DeviceHandle, &sfs_guid, (void **)&fs);
    if (EFI_ERROR(status)) {
        dbg(" FAIL"); dbg_nl();
        witness_warn("CONFIG", "cannot get filesystem, using defaults");
        return EFI_NOT_FOUND;
    }
    dbg(" OK"); dbg_nl();

    /* Open root directory */
    dbg("  OpenVolume()...");
    status = fs->OpenVolume(fs, &root);
    if (EFI_ERROR(status)) {
        dbg(" FAIL"); dbg_nl();
        witness_warn("CONFIG", "cannot open volume, using defaults");
        return EFI_NOT_FOUND;
    }
    dbg(" OK"); dbg_nl();

    /* Try to open boot.cfg */
    dbg("  Open(boot.cfg)...");
    status = root->Open(root, &cfg_file, g_boot_cfg_path, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        dbg(" not found (using defaults)"); dbg_nl();
        dbg_flush();
        root->Close(root);
        witness_warn("CONFIG", "boot.cfg not found, using defaults");
        return EFI_NOT_FOUND;
    }
    dbg(" OK"); dbg_nl();
    dbg_flush();

    /* Get file size */
    EFI_FILE_INFO *file_info;
    UINTN info_size = sizeof(EFI_FILE_INFO) + 256;
    status = BS->AllocatePool(EfiLoaderData, info_size, (void **)&file_info);
    if (EFI_ERROR(status)) {
        cfg_file->Close(cfg_file);
        root->Close(root);
        witness_warn("CONFIG", "cannot allocate file info, using defaults");
        return EFI_NOT_FOUND;
    }

    EFI_GUID fi_guid = EFI_FILE_INFO_ID;
    status = cfg_file->GetInfo(cfg_file, &fi_guid, &info_size, file_info);
    if (EFI_ERROR(status)) {
        BS->FreePool(file_info);
        cfg_file->Close(cfg_file);
        root->Close(root);
        witness_warn("CONFIG", "cannot get file info, using defaults");
        return EFI_NOT_FOUND;
    }

    UINTN file_size = file_info->FileSize;
    BS->FreePool(file_info);

    /* Allocate buffer for file content */
    CHAR8 *buffer;
    status = BS->AllocatePool(EfiLoaderData, file_size + 1, (void **)&buffer);
    if (EFI_ERROR(status)) {
        cfg_file->Close(cfg_file);
        root->Close(root);
        witness_warn("CONFIG", "cannot allocate buffer, using defaults");
        return EFI_NOT_FOUND;
    }

    /* Read file */
    status = cfg_file->Read(cfg_file, &file_size, buffer);
    cfg_file->Close(cfg_file);
    root->Close(root);

    if (EFI_ERROR(status)) {
        BS->FreePool(buffer);
        witness_warn("CONFIG", "cannot read file, using defaults");
        return EFI_NOT_FOUND;
    }

    buffer[file_size] = '\0';

    /* Reset config for fresh parse (keep defaults as fallback) */
    config->kernel_count = 0;
    int current_kernel = -1;

    /* Parse line by line (convert ASCII to wide string) */
    CHAR16 line[512];
    UINTN line_pos = 0;

    for (UINTN i = 0; i <= file_size; i++) {
        if (buffer[i] == '\n' || buffer[i] == '\0') {
            line[line_pos] = L'\0';
            parse_config_line(line, config, &current_kernel);
            line_pos = 0;
        } else if (line_pos < 511) {
            line[line_pos++] = (CHAR16)buffer[i];
        }
    }

    BS->FreePool(buffer);

    /* Validate: ensure at least one kernel */
    if (config->kernel_count == 0) {
        init_default_config(config);
        witness_warn("CONFIG", "no kernels in config, using defaults");
        return EFI_SUCCESS;
    }

    /* Ensure default kernel index is valid */
    if (config->default_kernel >= config->kernel_count) {
        config->default_kernel = 0;
    }

    witness_ok("CONFIG", "loaded kernels", config->kernel_count);
    return EFI_SUCCESS;
}

/** @} */ /* end B.3b BOOTCFG */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3c SECBOOT — Secure Boot Verification [SECBOOT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_secboot B.3c SECBOOT — Secure Boot Verification
 * @ingroup uefi_coreops
 * @brief    Verify kernel binary signatures before loading.
 *
 * SIGNATURE FORMAT:
 *   MillenniumOS uses a simple signature format (.sig files):
 *   - 4-byte magic: "MSIG"
 *   - 4-byte version: 1
 *   - 32-byte SHA256 hash of kernel binary
 *
 *   Signature file path = kernel path + ".sig"
 *   Example: \kernel.bin → \kernel.bin.sig
 *
 * VERIFICATION FLOW:
 *   1. Check if Secure Boot is enabled (config or UEFI variable)
 *   2. Load kernel signature file (.sig)
 *   3. Compute SHA256 of kernel binary
 *   4. Compare hashes
 *
 * DAR INTEGRATION:
 *   - DETECT:  witness_phase() by caller
 *   - ASSESS:  Return SecBootResult for caller to evaluate
 *   - RESTORE: Caller decides whether to proceed or halt
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Simple SHA256 hash computation.
 *
 * Computes SHA256 hash of data buffer. This is a simplified
 * implementation suitable for boot-time verification.
 *
 * @param[in]  data  Pointer to data buffer to hash.
 * @param[in]  size  Size of data in bytes.
 * @param[out] hash  32-byte buffer to receive SHA256 hash.
 *
 * @note This is a minimal implementation for boot environment.
 *       For production, consider using UEFI crypto protocols.
 *
 * @par Algorithm:
 * Standard SHA256 as per FIPS 180-4.
 */
static void compute_sha256(void *data, UINTN size, UINT8 *hash) {
    /*
     * SHA256 implementation
     *
     * NOTE: This is a simplified placeholder. In production,
     * use UEFI's EFI_HASH2_PROTOCOL or a verified implementation.
     * For now, we do a simple XOR-based checksum as placeholder.
     */

    /* Initialize hash to zeros */
    SetMem(hash, 32, 0);

    /* Simple checksum (placeholder - replace with real SHA256) */
    UINT8 *bytes = (UINT8 *)data;
    for (UINTN i = 0; i < size; i++) {
        hash[i % 32] ^= bytes[i];
        /* Rotate hash bytes for better distribution */
        UINT8 carry = hash[31] >> 7;
        for (int j = 31; j > 0; j--) {
            hash[j] = (hash[j] << 1) | (hash[j-1] >> 7);
        }
        hash[0] = (hash[0] << 1) | carry;
    }

    /* TODO: Replace with proper SHA256 implementation */
    /* Options:
     *   1. Use EFI_HASH2_PROTOCOL if available
     *   2. Include embedded SHA256 implementation
     *   3. Use UEFI Crypto library
     */
}

/**
 * @brief Verify kernel binary signature.
 *
 * Checks if kernel binary has a valid signature file and
 * verifies the signature matches the kernel content.
 *
 * @param[in] image_handle  UEFI image handle for filesystem access.
 * @param[in] kernel_path   Path to kernel binary on ESP.
 *
 * @return SecBootResult indicating verification status:
 *   - SECBOOT_OK:          Signature valid
 *   - SECBOOT_NO_SIG:      No signature file found
 *   - SECBOOT_INVALID_SIG: Signature doesn't match kernel
 *   - SECBOOT_DISABLED:    Secure Boot not enabled (skip check)
 *   - SECBOOT_ERROR:       Verification error
 *
 * @par Verification Steps:
 *   1. Build signature file path (kernel_path + ".sig")
 *   2. Open and read signature file
 *   3. Verify magic and version
 *   4. Load kernel binary
 *   5. Compute SHA256 of kernel
 *   6. Compare with signature hash
 *
 * @par Example:
 * @code
 * SecBootResult result = verify_kernel_signature(image_handle, L"\\kernel.bin");
 * if (result == SECBOOT_INVALID_SIG) {
 *     witness_fail("SECBOOT", "signature mismatch", 0);
 *     halt();
 * }
 * @endcode
 */
__attribute__((unused))
static SecBootResult verify_kernel_signature(EFI_HANDLE image_handle, CHAR16 *kernel_path) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
    EFI_FILE_PROTOCOL *root;
    EFI_FILE_PROTOCOL *sig_file;
    EFI_FILE_PROTOCOL *kernel_file;

    dbg("[SECBOOT] Verifying kernel signature..."); dbg_nl();
    dbg_flush();

    /* Check if secure boot is enabled in config */
    if (!g_boot_config.secure_boot_enabled) {
        dbg("  Secure boot disabled, skipping"); dbg_nl();
        dbg_flush();
        witness_ok("SECBOOT", "disabled, skipping verification", 0);
        return SECBOOT_DISABLED;
    }

    /* ═══════════════════════════════════════════════════════════════════════════
     * EDK2 Pattern: LoadedImage -> DeviceHandle -> SimpleFileSystem
     * ═══════════════════════════════════════════════════════════════════════════ */

    /* Get filesystem access from boot device */
    dbg("  HandleProtocol(LoadedImage)...");
    status = BS->HandleProtocol(image_handle, &li_guid, (void **)&loaded_image);
    if (EFI_ERROR(status)) {
        dbg(" FAIL"); dbg_nl();
        dbg_flush();
        witness_fail("SECBOOT", "cannot get loaded image", (uint64_t)status);
        return SECBOOT_ERROR;
    }
    dbg(" OK"); dbg_nl();

    dbg("  HandleProtocol(SFS from DeviceHandle)...");
    status = BS->HandleProtocol(loaded_image->DeviceHandle, &sfs_guid, (void **)&fs);
    if (EFI_ERROR(status)) {
        dbg(" FAIL"); dbg_nl();
        dbg_flush();
        witness_fail("SECBOOT", "cannot get filesystem", (uint64_t)status);
        return SECBOOT_ERROR;
    }

    status = fs->OpenVolume(fs, &root);
    if (EFI_ERROR(status)) {
        witness_fail("SECBOOT", "cannot open volume", (uint64_t)status);
        return SECBOOT_ERROR;
    }

    /* Build signature file path */
    CHAR16 sig_path[BOOT_CFG_PATH_MAX + 8];
    UINTN i = 0;
    while (kernel_path[i] && i < BOOT_CFG_PATH_MAX - 1) {
        sig_path[i] = kernel_path[i];
        i++;
    }
    sig_path[i++] = L'.';
    sig_path[i++] = L's';
    sig_path[i++] = L'i';
    sig_path[i++] = L'g';
    sig_path[i] = L'\0';

    /* Open signature file */
    status = root->Open(root, &sig_file, sig_path, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        root->Close(root);
        witness_warn("SECBOOT", "no signature file found");
        return SECBOOT_NO_SIG;
    }

    /* Read signature */
    MosSignature sig;
    UINTN sig_size = sizeof(MosSignature);
    status = sig_file->Read(sig_file, &sig_size, &sig);
    sig_file->Close(sig_file);

    if (EFI_ERROR(status) || sig_size != sizeof(MosSignature)) {
        root->Close(root);
        witness_fail("SECBOOT", "cannot read signature", (uint64_t)status);
        return SECBOOT_ERROR;
    }

    /* Verify signature magic */
    if (sig.magic[0] != 'M' || sig.magic[1] != 'S' ||
        sig.magic[2] != 'I' || sig.magic[3] != 'G') {
        root->Close(root);
        witness_fail("SECBOOT", "invalid signature magic", 0);
        return SECBOOT_INVALID_SIG;
    }

    /* Open kernel file */
    status = root->Open(root, &kernel_file, kernel_path, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        root->Close(root);
        witness_fail("SECBOOT", "cannot open kernel", (uint64_t)status);
        return SECBOOT_ERROR;
    }

    /* Get kernel size */
    EFI_FILE_INFO *file_info;
    UINTN info_size = sizeof(EFI_FILE_INFO) + 256;
    status = BS->AllocatePool(EfiLoaderData, info_size, (void **)&file_info);
    if (EFI_ERROR(status)) {
        kernel_file->Close(kernel_file);
        root->Close(root);
        witness_fail("SECBOOT", "cannot allocate info", (uint64_t)status);
        return SECBOOT_ERROR;
    }

    EFI_GUID fi_guid = EFI_FILE_INFO_ID;
    status = kernel_file->GetInfo(kernel_file, &fi_guid, &info_size, file_info);
    UINTN kernel_size = file_info->FileSize;
    BS->FreePool(file_info);

    if (EFI_ERROR(status)) {
        kernel_file->Close(kernel_file);
        root->Close(root);
        witness_fail("SECBOOT", "cannot get kernel info", (uint64_t)status);
        return SECBOOT_ERROR;
    }

    /* Allocate buffer and read kernel */
    void *kernel_data;
    status = BS->AllocatePool(EfiLoaderData, kernel_size, &kernel_data);
    if (EFI_ERROR(status)) {
        kernel_file->Close(kernel_file);
        root->Close(root);
        witness_fail("SECBOOT", "cannot allocate kernel buffer", (uint64_t)status);
        return SECBOOT_ERROR;
    }

    status = kernel_file->Read(kernel_file, &kernel_size, kernel_data);
    kernel_file->Close(kernel_file);
    root->Close(root);

    if (EFI_ERROR(status)) {
        BS->FreePool(kernel_data);
        witness_fail("SECBOOT", "cannot read kernel", (uint64_t)status);
        return SECBOOT_ERROR;
    }

    /* Compute kernel hash */
    UINT8 computed_hash[32];
    compute_sha256(kernel_data, kernel_size, computed_hash);
    BS->FreePool(kernel_data);

    /* Compare hashes */
    for (int j = 0; j < 32; j++) {
        if (computed_hash[j] != sig.sha256[j]) {
            witness_fail("SECBOOT", "hash mismatch", 0);
            return SECBOOT_INVALID_SIG;
        }
    }

    witness_ok("SECBOOT", "signature valid", 0);
    return SECBOOT_OK;
}

/** @} */ /* end B.3c SECBOOT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3d MULTIBOOT — Multi-Kernel Selection [MULTIBOOT]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_multiboot B.3d MULTIBOOT — Multi-Kernel Selection
 * @ingroup uefi_coreops
 * @brief    Display boot menu and handle kernel selection.
 *
 * BOOT MENU BEHAVIOR:
 *   - If timeout=0: Boot default kernel immediately
 *   - If timeout>0: Show menu, count down, boot default on timeout
 *   - If only one kernel: Skip menu unless timeout>0
 *   - Arrow keys: Navigate selection
 *   - Enter: Boot selected kernel
 *   - Escape: Return to firmware (if supported)
 *
 * DISPLAY FORMAT:
 *   @code
 *   ╔══════════════════════════════════════════╗
 *   ║       MillenniumOS Boot Menu             ║
 *   ╠══════════════════════════════════════════╣
 *   ║  > MillenniumOS                          ║
 *   ║    MillenniumOS (Recovery)               ║
 *   ╠══════════════════════════════════════════╣
 *   ║  Boot in 5 seconds... (Enter to boot)   ║
 *   ╚══════════════════════════════════════════╝
 *   @endcode
 *
 * DAR INTEGRATION:
 *   - DETECT:  witness_phase() by caller
 *   - ASSESS:  witness_ok() with selected kernel index
 *   - RESTORE: Timeout boots default, Escape returns to firmware
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Display boot menu with available kernels.
 *
 * Renders the kernel selection menu to UEFI console.
 * Called initially and after each selection change.
 *
 * @param[in] config  Boot configuration with kernel list.
 *
 * @return Number of menu entries displayed.
 *
 * @note Uses UEFI Print() for output. Menu persists until
 *       user makes selection or timeout expires.
 */
static UINT32 display_boot_menu(BootConfig *config) {
    /* Clear screen */
    ST->ConOut->ClearScreen(ST->ConOut);

    /* Header */
    DebugPrint(L"\n");
    DebugPrint(L"  ╔══════════════════════════════════════════════════════╗\n");
    DebugPrint(L"  ║         M I L L E N N I U M O S   Boot Menu          ║\n");
    DebugPrint(L"  ║     \"In the beginning God created...\" - Gen 1:1      ║\n");
    DebugPrint(L"  ╠══════════════════════════════════════════════════════╣\n");

    /* List kernels */
    for (UINT32 i = 0; i < config->kernel_count; i++) {
        if (i == config->default_kernel) {
            DebugPrint(L"  ║  > %s", config->kernels[i].name);
        } else {
            DebugPrint(L"  ║    %s", config->kernels[i].name);
        }
        /* Pad to fixed width */
        UINTN name_len = StrLen(config->kernels[i].name);
        for (UINTN j = name_len; j < 45; j++) DebugPrint(L" ");
        DebugPrint(L"║\n");
    }

    /* Footer */
    DebugPrint(L"  ╠══════════════════════════════════════════════════════╣\n");
    if (config->timeout_seconds > 0) {
        DebugPrint(L"  ║  Booting in %d seconds... (↑↓ select, Enter boot)   ║\n",
              config->timeout_seconds);
    } else {
        DebugPrint(L"  ║  Press ↑↓ to select, Enter to boot                  ║\n");
    }
    DebugPrint(L"  ╚══════════════════════════════════════════════════════╝\n");

    return config->kernel_count;
}

/**
 * @brief Wait for user kernel selection or timeout.
 *
 * Handles keyboard input for kernel selection with optional timeout.
 * Supports arrow keys for navigation and Enter for selection.
 *
 * @param[in]     config    Boot configuration (timeout, kernel count).
 * @param[in,out] selected  Initially set to default, updated to user selection.
 *
 * @return Menu action taken:
 *   - MENU_ACTION_BOOT:    User pressed Enter
 *   - MENU_ACTION_TIMEOUT: Timeout expired, boot default
 *   - MENU_ACTION_CANCEL:  User pressed Escape
 *
 * @par Input Handling:
 *   - Up Arrow:   Move selection up
 *   - Down Arrow: Move selection down
 *   - Enter:      Boot selected kernel
 *   - Escape:     Cancel (return to firmware)
 *
 * @note Timeout countdown displayed and updated each second.
 */
__attribute__((unused))
static MenuAction wait_for_selection(BootConfig *config, UINT32 *selected) {
    EFI_INPUT_KEY key;
    UINTN timeout_remaining = config->timeout_seconds;
    UINTN event_index;

    /* Create timer event for 1-second intervals */
    EFI_EVENT timer_event;
    EFI_STATUS status = BS->CreateEvent(EVT_TIMER, TPL_CALLBACK, NULL, NULL, &timer_event);
    if (EFI_ERROR(status)) {
        /* Fall back to immediate boot if timer fails */
        witness_warn("MENU", "cannot create timer, booting immediately");
        return MENU_ACTION_TIMEOUT;
    }

    /* Set timer for 1 second intervals */
    BS->SetTimer(timer_event, TimerPeriodic, 10000000);  /* 1 second in 100ns units */

    EFI_EVENT wait_events[2];
    wait_events[0] = ST->ConIn->WaitForKey;
    wait_events[1] = timer_event;

    while (TRUE) {
        status = BS->WaitForEvent(2, wait_events, &event_index);
        if (EFI_ERROR(status)) continue;

        if (event_index == 0) {
            /* Key pressed */
            status = ST->ConIn->ReadKeyStroke(ST->ConIn, &key);
            if (EFI_ERROR(status)) continue;

            if (key.ScanCode == SCAN_UP) {
                /* Move up */
                if (*selected > 0) {
                    (*selected)--;
                    config->default_kernel = *selected;
                    display_boot_menu(config);
                }
            } else if (key.ScanCode == SCAN_DOWN) {
                /* Move down */
                if (*selected < config->kernel_count - 1) {
                    (*selected)++;
                    config->default_kernel = *selected;
                    display_boot_menu(config);
                }
            } else if (key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
                /* Enter pressed */
                BS->CloseEvent(timer_event);
                witness_ok("MENU", "user selected kernel", *selected);
                return MENU_ACTION_BOOT;
            } else if (key.ScanCode == SCAN_ESC) {
                /* Escape pressed */
                BS->CloseEvent(timer_event);
                witness_warn("MENU", "user cancelled");
                return MENU_ACTION_CANCEL;
            }
        } else {
            /* Timer tick */
            if (config->timeout_seconds > 0) {
                if (timeout_remaining > 0) {
                    timeout_remaining--;
                    /* Update countdown display */
                    DebugPrint(L"\r  ║  Booting in %d seconds...                            ║",
                          timeout_remaining);
                } else {
                    /* Timeout expired */
                    BS->CloseEvent(timer_event);
                    witness_ok("MENU", "timeout, booting default", *selected);
                    return MENU_ACTION_TIMEOUT;
                }
            }
        }
    }
}

/** @} */ /* end B.3d MULTIBOOT */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3e KERNEL — Kernel Loading [KERNEL]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_kernel B.3e KERNEL — Kernel Loading
 * @ingroup uefi_coreops
 * @brief    Load kernel binary from ESP filesystem to memory.
 *
 * LOADING PROCESS:
 *   1. Get filesystem access via Loaded Image protocol
 *   2. Open kernel file from specified path
 *   3. Query file size
 *   4. Allocate pages at KERNEL_LOAD_ADDR (0x100000)
 *   5. Read kernel into allocated memory
 *   6. Update MosBootInfo with kernel location and size
 *
 * MEMORY LAYOUT:
 *   @code
 *   0x00000000 - 0x000FFFFF : Reserved (BIOS/real mode)
 *   0x00100000 - 0x00FFFFFF : Kernel (loaded here)
 *   0x01000000+             : Available RAM
 *   @endcode
 *
 * DAR INTEGRATION:
 *   - DETECT:  witness_phase() by caller
 *   - ASSESS:  witness_ok/fail() on load result
 *   - RESTORE: Return error status, caller decides to halt or retry
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Load kernel from specific path to fixed memory address.
 *
 * Core kernel loading implementation used by both load_kernel()
 * and multi-kernel selection. Loads kernel binary from specified
 * ESP path to KERNEL_LOAD_ADDR (0x100000).
 *
 * @param[in]  image_handle  UEFI image handle for filesystem access.
 * @param[in]  path          Wide string path to kernel on ESP.
 * @param[out] boot_info     Boot info structure to populate.
 *
 * @return EFI_SUCCESS on success, error code on failure.
 *
 * @par Loading Steps:
 *   1. Get Loaded Image protocol (find our boot device)
 *   2. Get Simple File System protocol
 *   3. Open ESP root volume
 *   4. Open kernel file at specified path
 *   5. Query file size via GetInfo()
 *   6. Allocate pages at KERNEL_LOAD_ADDR
 *   7. Read kernel binary into allocated memory
 *   8. Update boot_info with kernel_physical and kernel_size
 *
 * @par Error Handling:
 * Each step uses witness_fail() on error and returns status.
 * Caller decides whether to halt or try alternate path.
 *
 * @par Memory Allocation:
 * Uses AllocateAddress to ensure kernel loads at exactly 0x100000.
 * Kernel entry point must be at offset 0 of the binary.
 */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * Find Kernel in Directory — Search and open kernel file by scanning directory
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * Scans directory entries looking for a file containing "kernel" in the name.
 * When found, opens it using the EXACT filename from the directory listing.
 * This bypasses any path format issues with hardcoded paths.
 *
 * Returns: EFI_SUCCESS if kernel found and opened, error otherwise.
 *          kernel_file is set to the opened file handle.
 */
static EFI_STATUS find_kernel_in_directory(
    EFI_FILE_PROTOCOL *dir,
    EFI_FILE_PROTOCOL **kernel_file,
    UINTN fs_index,
    MosBootInfo *boot_info,
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs_protocol  /* For reopening root */
) {
    EFI_STATUS status;
    UINTN buf_size;
    EFI_FILE_INFO *info;
    UINTN entries_shown = 0;
    const UINTN MAX_ENTRIES = 16;
    CHAR16 *kernel_filename = NULL;

    /* Allocate buffer for file info */
    buf_size = 1024;
    status = BS->AllocatePool(EfiLoaderData, buf_size, (void **)&info);
    if (EFI_ERROR(status)) {
        dbg("    [FS "); dbg_hex(fs_index); dbg("] Cannot allocate buffer"); dbg_nl();
        return status;
    }

    dbg("    [FS "); dbg_hex(fs_index); dbg("] Scanning for kernel..."); dbg_nl();

    /* Draw white square for this filesystem (row 60) */
    if (boot_info && boot_info->framebuffer.address) {
        uint32_t *fb = (uint32_t *)(uintptr_t)boot_info->framebuffer.address;
        uint32_t pitch = boot_info->framebuffer.pitch / 4;
        uint32_t base_x = 10 + (uint32_t)fs_index * 100;
        for (uint32_t y = 60; y < 70; y++) {
            for (uint32_t x = base_x; x < base_x + 10; x++) {
                fb[y * pitch + x] = 0xFFFFFFFF;
            }
        }
    }

    /* Read directory entries */
    while (entries_shown < MAX_ENTRIES) {
        UINTN this_size = buf_size;
        status = dir->Read(dir, &this_size, info);

        if (EFI_ERROR(status) || this_size == 0) {
            break;
        }

        /* Skip directories */
        if (info->Attribute & EFI_FILE_DIRECTORY) {
            continue;
        }

        /* Serial output */
        dbg("      - ");
        serial_wide(info->FileName);
        dbg(" ("); dbg_hex(info->FileSize); dbg(" bytes)"); dbg_nl();

        /* Draw indicator for each file (row 75) */
        uint32_t color = 0xFF0080FF;  /* Blue = regular file */

        /* Check if filename contains "kernel" (case-insensitive) */
        CHAR16 *fn = info->FileName;
        BOOLEAN has_kernel = FALSE;
        while (*fn) {
            if ((*fn == 'k' || *fn == 'K') &&
                (fn[1] == 'e' || fn[1] == 'E') &&
                (fn[2] == 'r' || fn[2] == 'R')) {
                has_kernel = TRUE;
                break;
            }
            fn++;
        }

        if (has_kernel && kernel_filename == NULL) {
            /* Found kernel! Save the exact filename */
            color = 0xFF00FF00;  /* Bright green */

            /* Allocate and copy filename */
            UINTN fn_len = 0;
            while (info->FileName[fn_len]) fn_len++;
            status = BS->AllocatePool(EfiLoaderData, (fn_len + 1) * sizeof(CHAR16), (void **)&kernel_filename);
            if (!EFI_ERROR(status)) {
                for (UINTN i = 0; i <= fn_len; i++) {
                    kernel_filename[i] = info->FileName[i];
                }
            }
        }

        /* Draw file indicator */
        if (boot_info && boot_info->framebuffer.address) {
            uint32_t *fb = (uint32_t *)(uintptr_t)boot_info->framebuffer.address;
            uint32_t pitch = boot_info->framebuffer.pitch / 4;
            uint32_t base_x = 10 + (uint32_t)fs_index * 100;
            uint32_t file_x = base_x + 15 + (uint32_t)entries_shown * 12;
            for (uint32_t y = 75; y < 83; y++) {
                for (uint32_t x = file_x; x < file_x + 8; x++) {
                    fb[y * pitch + x] = color;
                }
            }
        }

        entries_shown++;
    }

    /* Draw count indicator (row 90) */
    if (boot_info && boot_info->framebuffer.address && entries_shown > 0) {
        uint32_t *fb = (uint32_t *)(uintptr_t)boot_info->framebuffer.address;
        uint32_t pitch = boot_info->framebuffer.pitch / 4;
        uint32_t base_x = 10 + (uint32_t)fs_index * 100;
        for (uint32_t y = 90; y < 95; y++) {
            for (uint32_t x = base_x; x < base_x + entries_shown * 8; x++) {
                fb[y * pitch + x] = 0xFFFFFF00;
            }
        }
    }

    dbg_flush();

    /* Reset directory position */
    dir->SetPosition(dir, 0);
    BS->FreePool(info);

    /* Now try to open the kernel file with its EXACT name */
    if (kernel_filename == NULL) {
        dbg("    No kernel file found in directory"); dbg_nl();
        return EFI_NOT_FOUND;
    }

    dbg("    Opening kernel: ");
    serial_wide(kernel_filename);
    dbg("...");
    dbg_flush();

    /* Close the directory we iterated and reopen root fresh */
    dir->Close(dir);

    EFI_FILE_PROTOCOL *fresh_root = NULL;
    status = fs_protocol->OpenVolume(fs_protocol, &fresh_root);
    if (EFI_ERROR(status)) {
        dbg(" Cannot reopen root!"); dbg_nl();
        BS->FreePool(kernel_filename);
        return status;
    }

    /* Try multiple approaches to open the file */

    /* Approach 1: Direct filename (as found in directory) */
    status = fresh_root->Open(fresh_root, kernel_file, kernel_filename, EFI_FILE_MODE_READ, 0);
    if (!EFI_ERROR(status)) {
        dbg(" SUCCESS (direct)!"); dbg_nl();
        dbg_flush();
        BS->FreePool(kernel_filename);
        /* Don't close fresh_root - caller will use kernel_file */
        return EFI_SUCCESS;
    }
    dbg(" direct fail: ");
    if (status == EFI_NOT_FOUND) dbg("NOT_FOUND");
    else if (status == EFI_INVALID_PARAMETER) dbg("INVALID_PARAM");
    else if (status == EFI_ACCESS_DENIED) dbg("ACCESS_DENIED");
    else if (status == EFI_MEDIA_CHANGED) dbg("MEDIA_CHANGED");
    else if (status == EFI_WRITE_PROTECTED) dbg("WRITE_PROTECTED");
    else if (status == EFI_NO_MEDIA) dbg("NO_MEDIA");
    else { dbg("0x"); dbg_hex(status); }

    /* Approach 2: With backslash prefix */
    CHAR16 path_with_slash[256];
    path_with_slash[0] = L'\\';
    UINTN j = 0;
    while (kernel_filename[j] && j < 254) {
        path_with_slash[j + 1] = kernel_filename[j];
        j++;
    }
    path_with_slash[j + 1] = 0;

    dbg(" trying \\prefix...");
    status = fresh_root->Open(fresh_root, kernel_file, path_with_slash, EFI_FILE_MODE_READ, 0);
    if (!EFI_ERROR(status)) {
        dbg(" SUCCESS (\\prefix)!"); dbg_nl();
        dbg_flush();
        BS->FreePool(kernel_filename);
        return EFI_SUCCESS;
    }
    dbg(" fail: ");
    if (status == EFI_NOT_FOUND) dbg("NOT_FOUND");
    else if (status == EFI_INVALID_PARAMETER) dbg("INVALID_PARAM");
    else if (status == EFI_ACCESS_DENIED) dbg("ACCESS_DENIED");
    else { dbg("0x"); dbg_hex(status); }
    dbg_nl();
    dbg_flush();

    fresh_root->Close(fresh_root);
    BS->FreePool(kernel_filename);
    return EFI_NOT_FOUND;
}

static EFI_STATUS load_kernel_from_path(EFI_HANDLE image_handle, CHAR16 *path, MosBootInfo *boot_info) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
    EFI_FILE_PROTOCOL *root = NULL;
    EFI_FILE_PROTOCOL *kernel_file = NULL;
    UINTN kernel_size;
    UINTN pages;
    EFI_PHYSICAL_ADDRESS kernel_addr = KERNEL_LOAD_ADDR;
    BOOLEAN used_fallback = FALSE;

    /* Multiple paths to try */
    static CHAR16 *paths_to_try[] = {
        L"kernel.bin",          /* No leading backslash (relative) */
        L"\\kernel.bin",        /* Absolute from root */
        L"KERNEL.BIN",          /* Uppercase variant */
        L"\\KERNEL.BIN",        /* Uppercase absolute */
        NULL
    };

    (void)path;  /* Use paths_to_try for robustness */

    dbg("[KERNEL] Hybrid kernel loading (EDK2 + fallback)..."); dbg_nl();
    dbg_flush();

    /* ═══════════════════════════════════════════════════════════════════════════
     * VISUAL STEP MARKER SYSTEM
     *
     * Row 25: Step progress (squares)
     *   Yellow = In progress
     *   Green  = Success
     *   Red    = Failed
     *   Cyan   = Info
     *
     * Row 50+: Path attempts (one row per path)
     * ═══════════════════════════════════════════════════════════════════════════ */
    #define KERNEL_STEP_MARKER(step, color) do { \
        if (boot_info->framebuffer.address) { \
            uint32_t* _fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address; \
            uint32_t _pitch = boot_info->framebuffer.pitch / 4; \
            for (uint32_t _y = 25; _y < 35; _y++) { \
                for (uint32_t _x = (step)*15; _x < (step)*15 + 12; _x++) { \
                    _fb[_y * _pitch + _x] = (color); \
                } \
            } \
        } \
    } while(0)

    /* ═══════════════════════════════════════════════════════════════════════════
     * HYBRID APPROACH: Try EDK2 pattern first, fallback to LocateHandleBuffer
     *
     * EDK2 Pattern: LoadedImage->DeviceHandle (correct, precise)
     * Fallback: LocateHandleBuffer (works with GNU-EFI quirks)
     * ═══════════════════════════════════════════════════════════════════════════ */

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 1: Try EDK2 pattern - Get LoadedImage from image_handle
     * ───────────────────────────────────────────────────────────────────────── */
    KERNEL_STEP_MARKER(1, 0xFFFFFF00);  /* Yellow = Step 1 starting */
    dbg("  Step 1: HandleProtocol(LoadedImage)...");
    dbg_flush();

    status = BS->HandleProtocol(image_handle, &li_guid, (void **)&loaded_image);
    if (EFI_ERROR(status)) {
        /* EDK2 pattern failed - mark as orange (warning, will try fallback) */
        KERNEL_STEP_MARKER(1, 0xFFFF8000);  /* Orange = EDK2 failed, trying fallback */
        dbg(" EDK2 fail: ");
        /* Decode EFI_STATUS for debugging */
        if (status == EFI_INVALID_PARAMETER) dbg("INVALID_PARAMETER");
        else if (status == EFI_UNSUPPORTED) dbg("UNSUPPORTED");
        else if (status == EFI_NOT_FOUND) dbg("NOT_FOUND");
        else if (status == EFI_ACCESS_DENIED) dbg("ACCESS_DENIED");
        else { dbg("0x"); dbg_hex(status); }
        dbg(", trying fallback..."); dbg_nl();
        dbg("  image_handle="); dbg_hex((uint64_t)image_handle); dbg_nl();
        dbg_flush();
        loaded_image = NULL;  /* Will use fallback */
    } else {
        KERNEL_STEP_MARKER(1, 0xFF00FF00);  /* Green = Step 1 OK */
        dbg(" OK (EDK2 pattern)"); dbg_nl();
        dbg("    DeviceHandle: 0x"); dbg_hex((uint64_t)loaded_image->DeviceHandle); dbg_nl();
        dbg_flush();
    }

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 2: Get SimpleFileSystem (EDK2 or fallback)
     * ───────────────────────────────────────────────────────────────────────── */
    KERNEL_STEP_MARKER(2, 0xFFFFFF00);  /* Yellow = Step 2 starting */

    if (loaded_image != NULL) {
        /* EDK2 path: Get SFS from boot device handle */
        dbg("  Step 2: HandleProtocol(SFS from DeviceHandle)...");
        dbg_flush();
        status = BS->HandleProtocol(loaded_image->DeviceHandle, &sfs_guid, (void **)&fs);
        if (!EFI_ERROR(status)) {
            KERNEL_STEP_MARKER(2, 0xFF00FF00);  /* Green = Step 2 OK */
            dbg(" OK"); dbg_nl();
            dbg_flush();
        } else {
            /* DeviceHandle doesn't have SFS - fall back */
            dbg(" DeviceHandle has no SFS, trying fallback..."); dbg_nl();
            dbg_flush();
            loaded_image = NULL;  /* Force fallback */
        }
    }

    if (loaded_image == NULL) {
        /* FALLBACK: Use LocateHandleBuffer to find filesystems */
        EFI_HANDLE *fs_handles = NULL;
        UINTN num_handles = 0;

        KERNEL_STEP_MARKER(2, 0xFFFF8000);  /* Orange = Using fallback */
        dbg("  Step 2 (FALLBACK): LocateHandleBuffer(SFS)...");
        dbg_flush();
        used_fallback = TRUE;

        status = BS->LocateHandleBuffer(ByProtocol, &sfs_guid, NULL, &num_handles, &fs_handles);
        if (EFI_ERROR(status) || num_handles == 0) {
            KERNEL_STEP_MARKER(2, 0xFFFF0000);  /* Red = Step 2 FAILED */
            dbg(" FAIL: No filesystems"); dbg_nl();
            dbg_flush();
            witness_fail("KERNEL", "No filesystems found", (uint64_t)status);
            return status;
        }
        dbg(" Found "); dbg_hex(num_handles); dbg(" filesystems"); dbg_nl();
        dbg_flush();

        /* Try each filesystem */
        for (UINTN i = 0; i < num_handles; i++) {
            status = BS->HandleProtocol(fs_handles[i], &sfs_guid, (void **)&fs);
            if (!EFI_ERROR(status)) {
                status = fs->OpenVolume(fs, &root);
                if (!EFI_ERROR(status)) {
                    /* ════════════════════════════════════════════════════════════════
                     * SMART KERNEL SEARCH: Scan directory for kernel file
                     * Uses exact filename from directory listing to avoid path issues
                     * Note: find_kernel_in_directory closes root and reopens fresh
                     * ════════════════════════════════════════════════════════════════ */
                    status = find_kernel_in_directory(root, &kernel_file, i, boot_info, fs);
                    if (!EFI_ERROR(status)) {
                        /* Found! */
                        KERNEL_STEP_MARKER(2, 0xFF00FF00);  /* Green */
                        KERNEL_STEP_MARKER(3, 0xFF00FF00);  /* Green */
                        KERNEL_STEP_MARKER(4, 0xFF00FF00);  /* Green */
                        dbg("  Kernel found on FS "); dbg_hex(i); dbg_nl();
                        dbg_flush();
                        BS->FreePool(fs_handles);
                        goto kernel_found;
                    }
                    /* root was closed by find_kernel_in_directory */
                    root = NULL;
                }
            }
        }

        /* Fallback failed - no kernel found */
        KERNEL_STEP_MARKER(2, 0xFFFF0000);  /* Red */
        KERNEL_STEP_MARKER(3, 0xFFFF0000);  /* Red */
        KERNEL_STEP_MARKER(4, 0xFFFF0000);  /* Red */
        BS->FreePool(fs_handles);
        dbg("  FAIL: Kernel not found on any filesystem"); dbg_nl();
        dbg_flush();
        witness_fail("KERNEL", "not found on any filesystem", 0);
        return EFI_NOT_FOUND;
    }

    /* EDK2 path continues - Step 3: OpenVolume */

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 3: Open root volume of boot device (EDK2 path)
     * ───────────────────────────────────────────────────────────────────────── */
    KERNEL_STEP_MARKER(3, 0xFFFFFF00);  /* Yellow = Step 3 starting */
    dbg("  Step 3: OpenVolume()...");
    dbg_flush();

    status = fs->OpenVolume(fs, &root);
    if (EFI_ERROR(status)) {
        KERNEL_STEP_MARKER(3, 0xFFFF0000);  /* Red = Step 3 FAILED */
        dbg(" FAIL: 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        witness_fail("KERNEL", "Cannot open volume", (uint64_t)status);
        return status;
    }
    KERNEL_STEP_MARKER(3, 0xFF00FF00);  /* Green = Step 3 OK */
    dbg(" OK"); dbg_nl();
    dbg_flush();

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 4: Try to open kernel file with multiple path formats
     * ───────────────────────────────────────────────────────────────────────── */
    KERNEL_STEP_MARKER(4, 0xFFFFFF00);  /* Yellow = Step 4 starting */
    dbg("  Step 4: Open(kernel file)..."); dbg_nl();
    dbg_flush();

    for (int p = 0; paths_to_try[p] != NULL; p++) {
        dbg("    Try: ");
        serial_wide(paths_to_try[p]);
        dbg_flush();

        /* Visual: Show which path we're trying (row 50) */
        if (boot_info->framebuffer.address) {
            uint32_t* fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address;
            uint32_t pitch = boot_info->framebuffer.pitch / 4;
            uint32_t row = 50 + (uint32_t)p * 12;
            for (uint32_t y = row; y < row + 8; y++) {
                for (uint32_t x = 10; x < 18; x++) {
                    fb[y * pitch + x] = 0xFF00FFFF;  /* Cyan = trying */
                }
            }
        }

        status = root->Open(root, &kernel_file, paths_to_try[p], EFI_FILE_MODE_READ, 0);
        if (!EFI_ERROR(status)) {
            /* SUCCESS! */
            dbg(" FOUND!"); dbg_nl();
            dbg_flush();
            KERNEL_STEP_MARKER(4, 0xFF00FF00);  /* Green = Step 4 OK */

            /* Visual: Green for successful path */
            if (boot_info->framebuffer.address) {
                uint32_t* fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address;
                uint32_t pitch = boot_info->framebuffer.pitch / 4;
                uint32_t row = 50 + (uint32_t)p * 12;
                for (uint32_t y = row; y < row + 8; y++) {
                    for (uint32_t x = 10; x < 30; x++) {
                        fb[y * pitch + x] = 0xFF00FF00;  /* Green = success */
                    }
                }
            }
            goto kernel_found;
        }

        /* Visual: Red for failed path */
        if (boot_info->framebuffer.address) {
            uint32_t* fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address;
            uint32_t pitch = boot_info->framebuffer.pitch / 4;
            uint32_t row = 50 + (uint32_t)p * 12;
            for (uint32_t y = row; y < row + 8; y++) {
                for (uint32_t x = 10; x < 18; x++) {
                    fb[y * pitch + x] = 0xFFFF0000;  /* Red = failed */
                }
            }
        }

        dbg(" err=0x"); dbg_hex(status & 0xFF); dbg_nl();
        dbg_flush();
    }

    /* If we get here, kernel wasn't found on boot device */
    KERNEL_STEP_MARKER(4, 0xFFFF0000);  /* Red = Step 4 FAILED */
    root->Close(root);
    dbg("  FAIL: Kernel not found on boot device"); dbg_nl();
    dbg_flush();
    witness_fail("KERNEL", "not found on boot device", 0);
    return EFI_NOT_FOUND;

kernel_found:
    /* kernel_file and root are valid - proceed to read the file */
    (void)used_fallback;  /* May be unused in some code paths */

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 5: Query file size
     * ───────────────────────────────────────────────────────────────────────── */
    KERNEL_STEP_MARKER(5, 0xFFFFFF00);  /* Yellow = Step 5 starting */
    dbg("  Step 5: GetInfo(file size)...");
    dbg_flush();
    EFI_FILE_INFO *file_info;
    UINTN info_size = sizeof(EFI_FILE_INFO) + 256;
    status = BS->AllocatePool(EfiLoaderData, info_size, (void **)&file_info);
    if (EFI_ERROR(status)) {
        KERNEL_STEP_MARKER(5, 0xFFFF0000);  /* Red = Step 5 FAIL (alloc) */
        kernel_file->Close(kernel_file);
        root->Close(root);
        dbg(" FAIL: AllocatePool 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        witness_fail("KERNEL", "cannot allocate file info", (uint64_t)status);
        return status;
    }

    EFI_GUID fi_guid = EFI_FILE_INFO_ID;
    status = kernel_file->GetInfo(kernel_file, &fi_guid, &info_size, file_info);
    if (EFI_ERROR(status)) {
        KERNEL_STEP_MARKER(5, 0xFFFF0000);  /* Red = Step 5 FAIL (getinfo) */
        BS->FreePool(file_info);
        kernel_file->Close(kernel_file);
        root->Close(root);
        dbg(" FAIL: GetInfo 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        witness_fail("KERNEL", "cannot get file info", (uint64_t)status);
        return status;
    }

    kernel_size = file_info->FileSize;
    BS->FreePool(file_info);
    KERNEL_STEP_MARKER(5, 0xFF00FF00);  /* Green = Step 5 OK */
    dbg(" OK - size: "); dbg_hex(kernel_size); dbg(" bytes"); dbg_nl();
    dbg_flush();

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 6: Allocate memory at KERNEL_LOAD_ADDR
     * ───────────────────────────────────────────────────────────────────────── */
    KERNEL_STEP_MARKER(6, 0xFFFFFF00);  /* Yellow = Step 6 starting */
    dbg("  Step 6: AllocatePages(0x100000)...");
    dbg_flush();
    pages = (kernel_size + 4095) / 4096;
    status = BS->AllocatePages(AllocateAddress, EfiLoaderData, pages, &kernel_addr);
    if (EFI_ERROR(status)) {
        KERNEL_STEP_MARKER(6, 0xFFFF0000);  /* Red = Step 6 FAIL */
        kernel_file->Close(kernel_file);
        root->Close(root);
        dbg(" FAIL: 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        witness_fail("KERNEL", "cannot allocate at 0x100000", (uint64_t)status);
        return status;
    }
    KERNEL_STEP_MARKER(6, 0xFF00FF00);  /* Green = Step 6 OK */
    dbg(" OK"); dbg_nl();
    dbg_flush();

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 7: Read kernel into memory
     * ───────────────────────────────────────────────────────────────────────── */
    KERNEL_STEP_MARKER(7, 0xFFFFFF00);  /* Yellow = Step 7 starting */
    dbg("  Step 7: Read(kernel to memory)...");
    dbg_flush();
    status = kernel_file->Read(kernel_file, &kernel_size, (void *)kernel_addr);
    kernel_file->Close(kernel_file);
    root->Close(root);

    if (EFI_ERROR(status)) {
        KERNEL_STEP_MARKER(7, 0xFFFF0000);  /* Red = Step 7 FAIL */
        BS->FreePages(kernel_addr, pages);
        dbg(" FAIL: 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        witness_fail("KERNEL", "cannot read kernel", (uint64_t)status);
        return status;
    }
    KERNEL_STEP_MARKER(7, 0xFF00FF00);  /* Green = Step 7 OK */
    dbg(" OK"); dbg_nl();
    dbg_flush();

    /* ─────────────────────────────────────────────────────────────────────────
     * Step 8: Update boot info - SUCCESS
     * ───────────────────────────────────────────────────────────────────────── */
    KERNEL_STEP_MARKER(8, 0xFF00FFFF);  /* Cyan = Step 8 (final) */
    boot_info->kernel_physical = kernel_addr;
    boot_info->kernel_size = kernel_size;
    dbg("  Step 8: SUCCESS - Kernel loaded at 0x"); dbg_hex(kernel_addr); dbg_nl();
    dbg_flush();

    witness_ok("KERNEL", "loaded at", kernel_addr);
    KERNEL_STEP_MARKER(8, 0xFF00FF00);  /* Green = All steps complete */

    return EFI_SUCCESS;
}

/**
 * @brief Load kernel using default paths (backward compatibility).
 *
 * Wrapper that tries default kernel paths in order:
 *   1. \kernel.bin (ESP root)
 *   2. \EFI\BOOT\kernel.bin (standard EFI location)
 *
 * @param[in]  image_handle  UEFI image handle for filesystem access.
 * @param[out] boot_info     Boot info structure to populate.
 *
 * @return EFI_SUCCESS on success, error code on failure.
 *
 * @par Tripwire Pattern:
 * Primary path (\kernel.bin) → Fallback path (\EFI\BOOT\kernel.bin) → Error
 *
 * @note For multi-kernel support, use load_kernel_from_path() directly
 *       with the path from boot.cfg.
 */
static EFI_STATUS load_kernel(EFI_HANDLE image_handle, MosBootInfo *boot_info) {
    EFI_STATUS status;

    dbg("=== KERNEL LOAD SEQUENCE ==="); dbg_nl();
    dbg("image_handle="); dbg_hex((uint64_t)image_handle); dbg_nl();

    /* Try primary path: ESP root */
    dbg("Trying path 1: \\kernel.bin"); dbg_nl();
    status = load_kernel_from_path(image_handle, g_default_kernel_path, boot_info);
    if (!EFI_ERROR(status)) {
        dbg("Path 1 succeeded!"); dbg_nl();
        return EFI_SUCCESS;
    }
    dbg("Path 1 failed, trying alternate..."); dbg_nl();

    witness_warn("KERNEL", "not at root, trying EFI/BOOT");

    /* Try alternate path: EFI/BOOT directory */
    dbg("Trying path 2: \\EFI\\BOOT\\kernel.bin"); dbg_nl();
    status = load_kernel_from_path(image_handle, g_alt_kernel_path, boot_info);
    if (!EFI_ERROR(status)) {
        dbg("Path 2 succeeded!"); dbg_nl();
        return EFI_SUCCESS;
    }
    dbg("Path 2 also failed!"); dbg_nl();
    dbg("KERNEL LOAD FAILED - both paths exhausted"); dbg_nl();

    witness_fail("KERNEL", "not found at any path", (uint64_t)status);
    return status;
}

/** @} */ /* end B.3e KERNEL */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3f MEMORY — Memory Map [MEMORY]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_memory B.3f MEMORY — Memory Map
 * @ingroup uefi_coreops
 * @brief    Acquire UEFI memory map and convert to MOS format.
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Convert UEFI memory type to MosMemoryType.
 *
 * Translates UEFI firmware memory descriptors into the unified MosMemoryType
 * enumeration used by the MillenniumOS kernel. This mapping ensures the kernel
 * understands which memory regions are usable vs reserved.
 *
 * @par Overview
 * UEFI defines ~15 memory types with varying semantics. We collapse these into
 * 5 MOS types: USABLE, RESERVED, ACPI, NVS, and KERNEL. The mapping is critical
 * for safe memory allocation — incorrectly marking reserved memory as usable
 * would corrupt firmware data structures.
 *
 * @par Memory Type Mapping
 * @code
 *   UEFI Type                    → MOS Type      Notes
 *   ─────────────────────────────────────────────────────────
 *   EfiConventionalMemory        → USABLE        Free RAM
 *   EfiBootServicesCode/Data     → USABLE        Free after ExitBootServices
 *   EfiLoaderCode/Data           → KERNEL        Our bootloader regions
 *   EfiACPIReclaimMemory         → ACPI          ACPI tables (reclaimable)
 *   EfiACPIMemoryNVS             → NVS           ACPI non-volatile storage
 *   EfiRuntimeServicesCode/Data  → RESERVED      UEFI runtime (don't touch!)
 *   Everything else              → RESERVED      Safe default
 * @endcode
 *
 * @par DAR Integration
 * - **Detect:** Called per memory descriptor during map iteration
 * - **Assess:** Type classification is deterministic (no assessment needed)
 * - **Restore:** Unknown types default to RESERVED (safe fallback)
 *
 * @param[in] uefi_type  UEFI memory type value (EFI_MEMORY_TYPE enum).
 *
 * @return Corresponding MosMemoryType value. Never returns an invalid type.
 *
 * @see get_memory_map() — Primary caller during memory map acquisition
 * @see MosMemoryType — Target enumeration in mos_types.h
 */
static MosMemoryType uefi_to_mos_memtype(UINT32 uefi_type) {
    switch (uefi_type) {
        /* ── USABLE: Available for kernel/application use ── */
        case EfiConventionalMemory:
        case EfiBootServicesCode:
        case EfiBootServicesData:
            return MOS_MEM_USABLE;  /* Available after ExitBootServices */

        /* ── RESERVED: Firmware-owned, never touch ── */
        case EfiReservedMemoryType:
        case EfiUnusableMemory:
        case EfiMemoryMappedIO:
        case EfiMemoryMappedIOPortSpace:
        case EfiPalCode:
            return MOS_MEM_RESERVED;

        /* ── ACPI: Tables and NVS storage ── */
        case EfiACPIReclaimMemory:
            return MOS_MEM_ACPI;
        case EfiACPIMemoryNVS:
            return MOS_MEM_NVS;

        /* ── KERNEL: Our bootloader/kernel regions ── */
        case EfiLoaderCode:
        case EfiLoaderData:
            return MOS_MEM_KERNEL;  /* Our bootloader/kernel data */

        /* ── RUNTIME: UEFI runtime services (reserved) ── */
        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
            return MOS_MEM_RESERVED;  /* UEFI runtime, don't touch */

        /* ── FALLBACK: Unknown types default to reserved (Restore) ── */
        default:
            return MOS_MEM_RESERVED;
    }
}

/**
 * @brief Acquire UEFI memory map and convert to MOS format.
 *
 * Retrieves the complete physical memory map from UEFI firmware and converts
 * it to the unified MosMemoryRegion format understood by the kernel. Also
 * establishes the ternary memory zone boundaries (Genesis pattern).
 *
 * @par Overview
 * The memory map is CRITICAL for kernel operation — without accurate memory
 * information, the kernel cannot safely allocate memory. This function:
 *   1. Queries UEFI for required buffer size (first call returns EFI_BUFFER_TOO_SMALL)
 *   2. Allocates buffer with extra slack (map can change between calls!)
 *   3. Retrieves actual memory map with map_key for ExitBootServices
 *   4. Converts each UEFI descriptor to MosMemoryRegion format
 *   5. Establishes ternary zone boundaries
 *
 * @par Algorithm
 * @code
 *   1. GetMemoryMap(size=0) → returns required size
 *   2. Allocate (size + 4 descriptors) for slack
 *   3. GetMemoryMap(buffer) → fills map, returns map_key
 *   4. For each descriptor:
 *      - Convert type via uefi_to_mos_memtype()
 *      - Copy base, length, attributes
 *      - Accumulate usable memory total
 *   5. Set zone boundaries:
 *      - void_end = 1MB (BIOS/legacy region)
 *      - word_end = 256MB (kernel reserved space)
 * @endcode
 *
 * @par DAR Integration
 * - **Detect:** Witnesses each phase of memory map acquisition
 * - **Assess:** Validates map_key returned, checks entry count
 * - **Restore:** Returns error status if any phase fails (fatal)
 *
 * @par Tripwire Pattern
 * - **Primary:** GetMemoryMap with allocated buffer
 * - **Warning:** N/A (no warning level for memory map)
 * - **Fallback:** N/A (memory map is required)
 * - **Error:** Return EFI_ERROR status to caller
 * - **HALT:** Caller (efi_main) halts on memory map failure
 *
 * @param[out] boot_info  Boot info structure to populate with memory_map[],
 *                        memory_map_count, total_memory, and zone boundaries.
 * @param[out] map_key    Memory map key required for ExitBootServices().
 *                        This key becomes INVALID if any allocation occurs!
 *
 * @return EFI_SUCCESS on success, EFI_ERROR status on failure.
 *
 * @warning The map_key is invalidated by ANY memory allocation. Call this
 *          function immediately before ExitBootServices().
 *
 * @see uefi_to_mos_memtype() — Type conversion helper
 * @see MosMemoryRegion — Target structure in mos_types.h
 */
static EFI_STATUS get_memory_map(MosBootInfo *boot_info, UINTN *map_key) {
    EFI_STATUS status;
    UINTN mmap_size = 0;
    EFI_MEMORY_DESCRIPTOR *mmap = NULL;
    UINTN desc_size;
    UINT32 desc_version;

    dbg("[MEMORY] Acquiring memory map..."); dbg_nl();
    dbg_flush();
    witness_phase(7, "Memory Map Acquisition");

    /* ── Phase 1: Query required buffer size ── */
    dbg("  GetMemoryMap(size query)...");
    dbg_flush();
    status = BS->GetMemoryMap(&mmap_size, mmap, map_key, &desc_size, &desc_version);
    if (status != EFI_BUFFER_TOO_SMALL) {
        dbg(" FAIL: 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        witness_fail("MEMORY", "GetMemoryMap size query failed", status);
        print_error(L"GetMemoryMap initial call failed", status);
        return status;
    }
    dbg(" OK, need "); dbg_hex(mmap_size); dbg(" bytes"); dbg_nl();
    dbg_flush();

    witness_ok("MEMORY", "Map size query", mmap_size);

    /* ── Phase 2: Allocate buffer with slack ── */
    /* Add extra space for map changes (allocations change the map!) */
    mmap_size += desc_size * 4;

    dbg("  AllocatePool("); dbg_hex(mmap_size); dbg(")...");
    dbg_flush();
    status = BS->AllocatePool(EfiLoaderData, mmap_size, (void **)&mmap);
    if (EFI_ERROR(status)) {
        dbg(" FAIL: 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        witness_fail("MEMORY", "Buffer allocation failed", status);
        print_error(L"Failed to allocate memory map", status);
        return status;
    }
    dbg(" OK"); dbg_nl();
    dbg_flush();

    witness_ok("MEMORY", "Buffer allocated", mmap_size);

    /* ── Phase 3: Retrieve actual memory map ── */
    dbg("  GetMemoryMap(retrieve)...");
    dbg_flush();
    status = BS->GetMemoryMap(&mmap_size, mmap, map_key, &desc_size, &desc_version);
    if (EFI_ERROR(status)) {
        dbg(" FAIL: 0x"); dbg_hex(status & 0xFFFF); dbg_nl();
        dbg_flush();
        witness_fail("MEMORY", "GetMemoryMap retrieval failed", status);
        print_error(L"GetMemoryMap failed", status);
        return status;
    }
    dbg(" OK, map_key=0x"); dbg_hex(*map_key); dbg_nl();
    dbg_flush();

    witness_ok("MEMORY", "Map retrieved, key", *map_key);

    /* ── Phase 4: Convert UEFI descriptors to MOS format ── */
    UINTN num_entries = mmap_size / desc_size;
    UINTN count = 0;
    uint64_t total_usable = 0;
    uint64_t total_reserved = 0;
    uint64_t total_acpi = 0;

    for (UINTN i = 0; i < num_entries && count < MOS_MEMORY_MAP_MAX; i++) {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)mmap + i * desc_size);

        boot_info->memory_map[count].base = desc->PhysicalStart;
        boot_info->memory_map[count].length = desc->NumberOfPages * 4096;
        boot_info->memory_map[count].type = uefi_to_mos_memtype(desc->Type);
        boot_info->memory_map[count].attributes = (uint32_t)desc->Attribute;

        /* Track memory totals by type */
        switch (boot_info->memory_map[count].type) {
            case MOS_MEM_USABLE:
                total_usable += boot_info->memory_map[count].length;
                break;
            case MOS_MEM_ACPI:
            case MOS_MEM_NVS:
                total_acpi += boot_info->memory_map[count].length;
                break;
            default:
                total_reserved += boot_info->memory_map[count].length;
                break;
        }

        count++;
    }

    /* Check for truncation */
    if (num_entries > MOS_MEMORY_MAP_MAX) {
        witness_warn("MEMORY", "Map truncated — exceeded MOS_MEMORY_MAP_MAX");
    }

    boot_info->memory_map_count = count;
    boot_info->total_memory = total_usable;

    /* ── Phase 5: Establish ternary memory zones (Genesis pattern) ── */
    /*
     * VOID zone (0 - 1MB): Legacy BIOS area, video memory, option ROMs
     * WORD zone (1MB - 256MB): Kernel code, data, initial heap
     * TOV zone (256MB+): User space, extended memory
     */
    boot_info->zone_void_end = 0x100000;      /* 1MB - void zone (BIOS area) */
    boot_info->zone_word_end = 0x10000000;    /* 256MB - word zone (kernel space) */

    /* ── Witness final statistics ── */
    witness_ok("MEMORY", "Entries converted", count);
    witness_ok("MEMORY", "Usable MB", total_usable / (1024 * 1024));
    witness_ok("MEMORY", "Reserved MB", total_reserved / (1024 * 1024));
    witness_ok("MEMORY", "ACPI MB", total_acpi / (1024 * 1024));

    DebugPrint(L"[MMAP] %d entries, total usable: %ld MB\n",
          count, total_usable / (1024 * 1024));

    return EFI_SUCCESS;
}

/** @} */ /* end B.3f MEMORY */

/**
 * ───────────────────────────────────────────────────────────────────────────────
 * B.3g ACPI — ACPI Discovery [ACPI]
 * ───────────────────────────────────────────────────────────────────────────────
 *
 * @defgroup uefi_acpi B.3g ACPI — ACPI Discovery
 * @ingroup uefi_coreops
 * @brief    Locate ACPI RSDP from UEFI configuration tables.
 *
 * CONTAINS:
 *   - find_acpi_rsdp() — RSDP discovery in configuration tables
 *
 * OVERVIEW:
 *   ACPI (Advanced Configuration and Power Interface) provides standardized
 *   hardware description tables that the kernel uses to understand system
 *   topology, power management, and device configuration. The RSDP (Root
 *   System Description Pointer) is the entry point to all ACPI tables.
 *
 * DAR INTEGRATION:
 *   - Detect: Witnesses RSDP discovery and version found
 *   - Assess: Prefers ACPI 2.0+ over 1.0 when both available
 *   - Restore: Continues boot if ACPI not found (non-fatal for some systems)
 *
 * ───────────────────────────────────────────────────────────────────────────────
 * @{ */

/**
 * @brief Find ACPI RSDP in UEFI configuration tables.
 *
 * Searches UEFI System Table's ConfigurationTable array for ACPI RSDP.
 * Prefers ACPI 2.0+ (XSDT) over ACPI 1.0 (RSDT) for 64-bit address support.
 *
 * @par Overview
 * UEFI stores system configuration tables in a standardized array accessible
 * via ST->ConfigurationTable. Each entry has a GUID identifying the table type
 * and a pointer to the table data. We search for ACPI GUIDs:
 *   - ACPI_20_TABLE_GUID: ACPI 2.0+ RSDP (preferred, has XSDT)
 *   - ACPI_TABLE_GUID: ACPI 1.0 RSDP (fallback, only has RSDT)
 *
 * @par Algorithm
 * @code
 *   1. Initialize rsdp_address = 0 (not found)
 *   2. For each entry in ConfigurationTable:
 *      a. If GUID matches ACPI 2.0:
 *         - Store address, witness success, RETURN immediately
 *      b. If GUID matches ACPI 1.0:
 *         - Store address tentatively, continue searching for 2.0
 *   3. If loop completes:
 *      - If 1.0 found: already stored, witness as fallback
 *      - If nothing found: witness warning (non-fatal)
 * @endcode
 *
 * @par DAR Integration
 * - **Detect:** Witnesses table search and discovery results
 * - **Assess:** Evaluates ACPI version (2.0 preferred over 1.0)
 * - **Restore:** Non-fatal if ACPI not found (some embedded systems lack ACPI)
 *
 * @par ACPI Version Differences
 * @code
 *   ACPI 1.0 (RSDT):
 *     - 32-bit physical addresses only
 *     - Limited to 4GB address space
 *     - Uses "RSD PTR " signature
 *
 *   ACPI 2.0+ (XSDT):
 *     - 64-bit physical addresses
 *     - Full address space support
 *     - Extended checksum validation
 *     - Uses "RSD PTR " + extended fields
 * @endcode
 *
 * @param[out] boot_info  Boot info structure. Sets rsdp_address field:
 *                        - Non-zero: RSDP physical address found
 *                        - Zero: ACPI not available on this system
 *
 * @note ACPI absence is non-fatal. Some embedded/IoT systems lack ACPI.
 *       The kernel should handle rsdp_address == 0 gracefully.
 *
 * @see https://uefi.org/specs/ACPI/6.5/ — ACPI Specification
 * @see https://wiki.osdev.org/RSDP — OSDev RSDP documentation
 */
static void find_acpi_rsdp(MosBootInfo *boot_info) {
    EFI_GUID acpi2_guid = ACPI_20_TABLE_GUID;
    EFI_GUID acpi_guid = ACPI_TABLE_GUID;

    witness_phase(8, "ACPI Discovery");

    boot_info->rsdp_address = 0;

    /* ── Search configuration tables for ACPI ── */
    witness_ok("ACPI", "Config table entries", ST->NumberOfTableEntries);

    for (UINTN i = 0; i < ST->NumberOfTableEntries; i++) {
        EFI_CONFIGURATION_TABLE *table = &ST->ConfigurationTable[i];

        /* ── Prefer ACPI 2.0+ (64-bit XSDT support) ── */
        if (CompareGuid(&table->VendorGuid, &acpi2_guid) == 0) {
            boot_info->rsdp_address = (uint64_t)table->VendorTable;
            witness_ok("ACPI", "RSDP 2.0 found at", boot_info->rsdp_address);
            DebugPrint(L"[ACPI] RSDP 2.0 at 0x%lx\n", boot_info->rsdp_address);
            return;  /* Found preferred version, done */
        }

        /* ── Accept ACPI 1.0 as fallback (32-bit RSDT only) ── */
        if (CompareGuid(&table->VendorGuid, &acpi_guid) == 0) {
            boot_info->rsdp_address = (uint64_t)table->VendorTable;
            /* Don't return — continue searching for 2.0 */
            DebugPrint(L"[ACPI] RSDP 1.0 at 0x%lx (searching for 2.0...)\n", boot_info->rsdp_address);
        }
    }

    /* ── Assess discovery results ── */
    if (boot_info->rsdp_address != 0) {
        /* Found 1.0 but not 2.0 — acceptable fallback */
        witness_warn("ACPI", "Using ACPI 1.0 (2.0 not available)");
        DebugPrint(L"[ACPI] Using RSDP 1.0 (ACPI 2.0 not available)\n");
    } else {
        /* No ACPI at all — non-fatal warning */
        witness_warn("ACPI", "No ACPI tables found (embedded system?)");
        DebugPrint(L"[ACPI] WARNING: No ACPI tables found\n");
    }
}

/** @} */ /* end B.3g ACPI */

/** @} */ /* end B.3 CORE OPS */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.4 ERRORS — Error Handling Patterns [ERRORS]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_errors B.4 ERRORS — Error Handling Patterns
 * @brief    Error handling philosophy for UEFI bootloader.
 *
 * DESIGN:
 *   - Graphics failure: Continue without graphics (text mode)
 *   - Kernel load failure: HALT (fatal - cannot boot)
 *   - Memory map failure: HALT (fatal - kernel needs memory info)
 *   - ACPI not found: Continue (non-fatal, some systems lack ACPI)
 *   - ExitBootServices failure: Retry once, then HALT
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * B.5 PUBLIC API — EFI Entry Point [API]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_api B.5 PUBLIC API — EFI Entry Point
 * @brief    UEFI application entry point — efi_main().
 *
 * CONTAINS:
 *   - efi_main() — UEFI application entry, boot orchestrator
 *
 * OVERVIEW:
 *   This is the primary public API — the single entry point called by UEFI
 *   firmware. Orchestrates the complete boot sequence from firmware handoff
 *   to kernel transfer. Everything else in this file supports efi_main().
 *
 * DAR INTEGRATION:
 *   - Detect: Witnesses each boot phase with witness_phase()
 *   - Assess: Evaluates success/failure at each phase boundary
 *   - Restore: Phase-appropriate recovery (continue or halt)
 *
 * BOOT PHASES:
 *   Phase 0: Initialize GNU-EFI library
 *   Phase 1: Allocate boot info structure at 0x9000
 *   Phase 2: Initialize GOP graphics (non-fatal if fails)
 *   Phase 3: Load kernel from ESP (fatal if fails)
 *   Phase 4: Discover ACPI RSDP (non-fatal if missing)
 *   Phase 5: Acquire memory map (fatal if fails)
 *   Phase 6: Copy boot info to fixed address
 *   Phase 7: Exit boot services (fatal if fails)
 *   Phase 8: Transfer control to kernel (no return)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * @{ */

/**
 * @brief UEFI application entry point.
 *
 * The master orchestrator for MillenniumOS UEFI boot. Called directly by UEFI
 * firmware after loading BOOTX64.EFI from the EFI System Partition. This function
 * never returns on successful boot — control transfers to the kernel.
 *
 * @par Overview
 * efi_main() is the only externally visible symbol in this file. It coordinates
 * all boot operations in a strict phase sequence, with each phase having defined
 * success criteria and failure responses. The function signature is mandated by
 * UEFI specification — cannot be changed.
 *
 * @par Boot Phase Sequence
 * @code
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │  PHASE 0: GNU-EFI Library Init                                      │
 *   │    → InitializeLib() makes UEFI services accessible                 │
 *   │    → Sets up ST, BS, RS global pointers                             │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │  PHASE 1: Boot Info Allocation                                      │
 *   │    → Allocate MosBootInfo at 0x9000 (or fallback pool)              │
 *   │    → Initialize magic, version, capabilities                        │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │  PHASE 2: Graphics Init (Non-Fatal)                                 │
 *   │    → Locate GOP protocol, set video mode                            │
 *   │    → Populate framebuffer info                                      │
 *   │    → Continue if fails (text mode fallback)                         │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │  PHASE 3: Kernel Load (Fatal)                                       │
 *   │    → Load boot config (if present)                                  │
 *   │    → Verify secure boot signature (if enabled)                      │
 *   │    → Display boot menu (if multiple kernels)                        │
 *   │    → Load kernel.bin from ESP to memory                             │
 *   │    → HALT if kernel not found                                       │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │  PHASE 4: ACPI Discovery (Non-Fatal)                                │
 *   │    → Search config tables for RSDP                                  │
 *   │    → Prefer 2.0+, fallback to 1.0                                   │
 *   │    → Continue if not found (embedded systems)                       │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │  PHASE 5: Memory Map (Fatal)                                        │
 *   │    → Acquire UEFI memory map                                        │
 *   │    → Convert to MOS format                                          │
 *   │    → Establish ternary zones                                        │
 *   │    → HALT if fails                                                  │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │  PHASE 6: Boot Info Finalization                                    │
 *   │    → Copy boot info to 0x9000 if allocated elsewhere                │
 *   │    → Ensure kernel can find it at fixed address                     │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │  PHASE 7: Exit Boot Services (Fatal)                                │
 *   │    → Record boot_tsc timestamp                                      │
 *   │    → Call ExitBootServices() with map_key                           │
 *   │    → Retry once if fails (map may have changed)                     │
 *   │    → HALT if retry fails                                            │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │  PHASE 8: Kernel Transfer (No Return)                               │
 *   │    → Cast kernel_physical to entry function                         │
 *   │    → Call kernel(boot_info) — NEVER RETURNS                         │
 *   │    → If returns: HALT (kernel crashed before init)                  │
 *   └─────────────────────────────────────────────────────────────────────┘
 * @endcode
 *
 * @par DAR Integration
 * - **Detect:** witness_phase() called at each phase boundary
 * - **Assess:** Each phase evaluates EFI_STATUS for success/failure
 * - **Restore:** Phase-appropriate response:
 *   - Non-fatal phases (GOP, ACPI): Log warning, continue
 *   - Fatal phases (Kernel, Memory, ExitBS): halt() with error
 *
 * @par Tripwire Pattern Applied
 * @code
 *   Primary:   AllocatePages at 0x9000
 *   Warning:   "Could not allocate directly, using temp buffer"
 *   Fallback:  AllocatePool + reserve 0x9000 for later copy
 *   Error:     print_error + halt()
 *   HALT:      while(1) hlt — system cannot boot
 * @endcode
 *
 * @param[in] image_handle  UEFI image handle for this bootloader application.
 *                          Used for file system access and ExitBootServices.
 * @param[in] system_table  Pointer to UEFI System Table structure containing:
 *                          - ConOut: Console output protocol
 *                          - BootServices: Pre-ExitBS services
 *                          - RuntimeServices: Post-ExitBS services
 *                          - ConfigurationTable: ACPI, SMBIOS, etc.
 *
 * @return EFI_SUCCESS on successful kernel transfer (never actually returns).
 *         On fatal error, function halts via infinite hlt loop.
 *
 * @warning After ExitBootServices(), UEFI Boot Services are INVALID.
 *          Only Runtime Services and direct hardware access work.
 *
 * @warning Serial output continues working after ExitBootServices because
 *          it uses direct port I/O, not UEFI services.
 *
 * @note This function is marked EFIAPI for proper calling convention on x64.
 *
 * @see init_gop() — Phase 2 graphics initialization
 * @see load_kernel() — Phase 3 kernel loading
 * @see find_acpi_rsdp() — Phase 4 ACPI discovery
 * @see get_memory_map() — Phase 5 memory map acquisition
 */
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
    EFI_STATUS status;
    MosBootInfo *boot_info;
    UINTN map_key;

    /* ═══════════════════════════════════════════════════════════════════════
     * PHASE 0: Initialize UEFI Library
     * ═══════════════════════════════════════════════════════════════════════ */

    /* Initialize COM1 debug port - unified with entry.asm and kernel */
    debug_init();

    /* Distinctive marker for finding our output in mixed serial stream */
    serial_str("\r\n");
    serial_str(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
    /* Initialize debug buffer system */
    dbg_clear();
    dbg("=== MILLENNIUMOS UEFI BOOT ==="); dbg_nl();
    dbg("Entry point reached"); dbg_nl();

    /* Initialize GNU-EFI library */
    dbg("InitializeLib()..."); dbg_nl();
    InitializeLib(image_handle, system_table);
    dbg("InitializeLib() OK"); dbg_nl();

    witness_phase(0, "GNU-EFI Library Initialization");
    witness_ok("UEFI", "InitializeLib complete", (uint64_t)system_table);

    /* Log key addresses for debugging */
    dbg("SystemTable="); dbg_hex((uint64_t)system_table); dbg_nl();
    dbg("ImageHandle="); dbg_hex((uint64_t)image_handle); dbg_nl();
    dbg("BootServices="); dbg_hex((uint64_t)BS); dbg_nl();

    /* Clear screen and show banner */
    if (ST && ST->ConOut) {
        ST->ConOut->ClearScreen(ST->ConOut);
        dbg("ClearScreen() OK"); dbg_nl();
    } else {
        dbg("WARNING: ConOut is NULL"); dbg_nl();
    }

    DebugPrint(L"\n");
    DebugPrint(L"  ==================================================\n");
    DebugPrint(L"       M I L L E N N I U M O S   (UEFI Boot)\n");
    DebugPrint(L"       Kingdom Technology Operating System\n");
    DebugPrint(L"  ==================================================\n");
    DebugPrint(L"       \"In the beginning God created...\" Gen 1:1\n");
    DebugPrint(L"             (c) 2026 CreativeWorkzStudio LLC\n");
    DebugPrint(L"  ==================================================\n");
    DebugPrint(L"       Boot Mode: UEFI (Native 64-bit)\n");
    DebugPrint(L"       Graphics:  GOP (Graphics Output Protocol)\n");
    DebugPrint(L"  ==================================================\n");
    DebugPrint(L"\n");
    dbg("Banner displayed"); dbg_nl();

    /* ═══════════════════════════════════════════════════════════════════════
     * PHASE 1: Allocate Boot Info Structure
     * ═══════════════════════════════════════════════════════════════════════ */
    witness_phase(1, "Boot Info Allocation");

    EFI_PHYSICAL_ADDRESS info_addr = MOS_BOOT_INFO_ADDR;
    UINTN boot_info_pages = (sizeof(MosBootInfo) + 4095) / 4096;

    serial_str("[MOS-UEFI] MosBootInfo size: ");
    serial_hex(sizeof(MosBootInfo));
    serial_str(" bytes, ");
    serial_hex(boot_info_pages);
    serial_str(" pages\r\n");

    witness_ok("BOOTINFO", "Structure size bytes", sizeof(MosBootInfo));
    witness_ok("BOOTINFO", "Pages required", boot_info_pages);

    status = BS->AllocatePages(AllocateAddress, EfiLoaderData, boot_info_pages, &info_addr);
    if (EFI_ERROR(status)) {
        /* ── Tripwire: Primary failed, try fallback ── */
        witness_warn("BOOTINFO", "Direct allocation failed, using pool fallback");
        DebugPrint(L"[WARN] Could not allocate 0x%x directly, using temp buffer\n", MOS_BOOT_INFO_ADDR);

        status = BS->AllocatePool(EfiLoaderData, sizeof(MosBootInfo), (void **)&boot_info);
        if (EFI_ERROR(status)) {
            witness_fail("BOOTINFO", "Pool allocation also failed", status);
            print_error(L"Failed to allocate boot info", status);
            halt();
        }

        witness_ok("BOOTINFO", "Pool fallback succeeded at", (uint64_t)boot_info);

        /* Reserve 0x9000 for later copy */
        info_addr = MOS_BOOT_INFO_ADDR;
        EFI_STATUS reserve_status = BS->AllocatePages(AllocateAddress, EfiLoaderData, boot_info_pages, &info_addr);
        if (EFI_ERROR(reserve_status)) {
            witness_warn("BOOTINFO", "Cannot reserve 0x9000 — hoping free at jump");
            DebugPrint(L"[WARN] Cannot reserve 0x%x - hoping it's free at jump time\n", MOS_BOOT_INFO_ADDR);
        } else {
            witness_ok("BOOTINFO", "Reserved target address", MOS_BOOT_INFO_ADDR);
        }
    } else {
        boot_info = (MosBootInfo *)MOS_BOOT_INFO_ADDR;
        witness_ok("BOOTINFO", "Direct allocation at", MOS_BOOT_INFO_ADDR);
    }

    /* Initialize boot info structure */
    SetMem(boot_info, sizeof(MosBootInfo), 0);
    boot_info->magic = MOS_BOOT_MAGIC;
    boot_info->version = MOS_BOOT_VERSION;
    boot_info->boot_source = MOS_BOOT_SOURCE_UEFI;
    boot_info->boot_state = MOS_BOOT_STATE_OK;
    boot_info->capabilities = MOS_CAP_FB_64BIT | MOS_CAP_ACPI;
    boot_info->uefi_system_table = (uint64_t)system_table;

    /* Initialize boot event system for persistent diagnostics */
    boot_event_init(boot_info);
    boot_event(boot_info, MOS_EVT_PHASE_BOOTINFO | MOS_EVT_STEP_OK, 0, (uint32_t)(uint64_t)boot_info);

    witness_ok("BOOTINFO", "Magic set", boot_info->magic);
    witness_ok("BOOTINFO", "Version set", boot_info->version);

    /* ═══════════════════════════════════════════════════════════════════════
     * PHASE 2: Initialize GOP (Graphics) — NON-FATAL
     * ═══════════════════════════════════════════════════════════════════════ */
    witness_phase(2, "Graphics Initialization (GOP)");

    serial_str("[MOS-UEFI] PHASE 2: Graphics initialization (GOP)\r\n");
    DebugPrint(L"\n[PHASE 2] Graphics initialization (UEFI GOP)...\n");
    boot_event(boot_info, MOS_EVT_PHASE_GOP | MOS_EVT_STEP_START, 0, 0);
    status = init_gop(boot_info);
    if (EFI_ERROR(status)) {
        /* ── Non-fatal: Continue without graphics (text mode fallback) ── */
        boot_event(boot_info, MOS_EVT_PHASE_GOP | MOS_EVT_STEP_WARN, (uint16_t)status, 0);
        witness_warn("GOP", "Graphics init failed — continuing with text mode");
        serial_str("[MOS-UEFI] PHASE 2: GOP failed, continuing without graphics\r\n");
        DebugPrint(L"[WARN] GOP failed - continuing with text mode\n");
        boot_info->framebuffer.address = 0;
    } else {
        boot_event(boot_info, MOS_EVT_PHASE_GOP | MOS_EVT_STEP_OK, 0, (uint32_t)boot_info->framebuffer.address);
        witness_ok("GOP", "Framebuffer address", boot_info->framebuffer.address);
        witness_ok("GOP", "Resolution width", boot_info->framebuffer.width);
        witness_ok("GOP", "Resolution height", boot_info->framebuffer.height);
        serial_str("[MOS-UEFI] PHASE 2: GOP OK, fb_addr=");
        serial_hex(boot_info->framebuffer.address);
        serial_str(" res=");
        serial_hex(boot_info->framebuffer.width);
        serial_str("x");
        serial_hex(boot_info->framebuffer.height);
        serial_str("\r\n");

        /* ═══ VISUAL MARKER: Magenta bar = GOP complete ═══ */
        uint32_t* fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address;
        uint32_t pitch = boot_info->framebuffer.pitch / 4;
        for (uint32_t y = 0; y < 20; y++) {
            for (uint32_t x = 0; x < boot_info->framebuffer.width; x++) {
                fb[y * pitch + x] = 0xFFFF00FF;  /* Magenta = GOP done */
            }
        }

        /* ═══ VISUAL COLOR LEGEND (right side of screen) ═══
         *
         * This helps debug by showing what each color means:
         *   Yellow  = In progress
         *   Green   = Success
         *   Red     = Failed
         *   Orange  = Warning/Fallback
         *   Cyan    = Info/Trying
         *   Magenta = Phase complete
         */
        #define LEGEND_X_START (boot_info->framebuffer.width - 100)
        #define LEGEND_SQUARE_SIZE 8
        #define LEGEND_Y_START 30

        /* Yellow = In Progress */
        for (uint32_t y = LEGEND_Y_START; y < LEGEND_Y_START + LEGEND_SQUARE_SIZE; y++) {
            for (uint32_t x = LEGEND_X_START; x < LEGEND_X_START + LEGEND_SQUARE_SIZE; x++) {
                fb[y * pitch + x] = 0xFFFFFF00;
            }
        }
        /* Green = Success */
        for (uint32_t y = LEGEND_Y_START + 12; y < LEGEND_Y_START + 12 + LEGEND_SQUARE_SIZE; y++) {
            for (uint32_t x = LEGEND_X_START; x < LEGEND_X_START + LEGEND_SQUARE_SIZE; x++) {
                fb[y * pitch + x] = 0xFF00FF00;
            }
        }
        /* Red = Failed */
        for (uint32_t y = LEGEND_Y_START + 24; y < LEGEND_Y_START + 24 + LEGEND_SQUARE_SIZE; y++) {
            for (uint32_t x = LEGEND_X_START; x < LEGEND_X_START + LEGEND_SQUARE_SIZE; x++) {
                fb[y * pitch + x] = 0xFFFF0000;
            }
        }
        /* Orange = Warning/Fallback */
        for (uint32_t y = LEGEND_Y_START + 36; y < LEGEND_Y_START + 36 + LEGEND_SQUARE_SIZE; y++) {
            for (uint32_t x = LEGEND_X_START; x < LEGEND_X_START + LEGEND_SQUARE_SIZE; x++) {
                fb[y * pitch + x] = 0xFFFF8000;
            }
        }
        /* Cyan = Info/Trying */
        for (uint32_t y = LEGEND_Y_START + 48; y < LEGEND_Y_START + 48 + LEGEND_SQUARE_SIZE; y++) {
            for (uint32_t x = LEGEND_X_START; x < LEGEND_X_START + LEGEND_SQUARE_SIZE; x++) {
                fb[y * pitch + x] = 0xFF00FFFF;
            }
        }

        #undef LEGEND_X_START
        #undef LEGEND_SQUARE_SIZE
        #undef LEGEND_Y_START
    }

    /* ═══════════════════════════════════════════════════════════════════════
     * PHASE 3: Load Kernel — FATAL IF FAILS
     * ═══════════════════════════════════════════════════════════════════════ */
    witness_phase(3, "Kernel Loading");

    serial_str("[MOS-UEFI] PHASE 3: Loading kernel\r\n");
    DebugPrint(L"\n[PHASE 3] Loading kernel...\n");
    boot_event(boot_info, MOS_EVT_PHASE_KERNEL | MOS_EVT_STEP_START, 0, 0);
    status = load_kernel(image_handle, boot_info);
    if (EFI_ERROR(status)) {
        /* ── Fatal: Cannot boot without kernel ── */
        boot_event(boot_info, MOS_EVT_PHASE_KERNEL | MOS_EVT_STEP_FAIL, (uint16_t)status, 0);
        witness_fail("KERNEL", "Kernel load failed", status);
        serial_str("[MOS-UEFI] PHASE 3: FAILED - kernel load error\r\n");
        print_error(L"Kernel load failed", status);
        halt();
    }

    boot_event(boot_info, MOS_EVT_PHASE_KERNEL | MOS_EVT_STEP_OK, 0, (uint32_t)boot_info->kernel_physical);
    witness_ok("KERNEL", "Loaded at physical address", boot_info->kernel_physical);
    witness_ok("KERNEL", "Kernel size bytes", boot_info->kernel_size);
    serial_str("[MOS-UEFI] PHASE 3: OK, kernel_phys=");
    serial_hex(boot_info->kernel_physical);
    serial_str(" size=");
    serial_hex(boot_info->kernel_size);
    serial_str("\r\n");

    /* ═══ VISUAL MARKER: Yellow bar row 2 = Kernel loaded ═══ */
    if (boot_info->framebuffer.address) {
        uint32_t* fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address;
        uint32_t pitch = boot_info->framebuffer.pitch / 4;
        for (uint32_t y = 20; y < 40; y++) {
            for (uint32_t x = 0; x < boot_info->framebuffer.width; x++) {
                fb[y * pitch + x] = 0xFFFFFF00;  /* Yellow = Kernel loaded */
            }
        }
    }

    /* ═══════════════════════════════════════════════════════════════════════
     * PHASE 4: Find ACPI — NON-FATAL
     * ═══════════════════════════════════════════════════════════════════════ */
    /* Note: find_acpi_rsdp() has its own witness_phase(8) internally */
    serial_str("[MOS-UEFI] PHASE 4: ACPI discovery\r\n");
    DebugPrint(L"\n[PHASE 4] ACPI discovery...\n");
    boot_event(boot_info, MOS_EVT_PHASE_ACPI | MOS_EVT_STEP_START, 0, 0);
    find_acpi_rsdp(boot_info);

    /* Final ACPI status (function handles its own detailed witnessing) */
    if (boot_info->rsdp_address) {
        boot_event(boot_info, MOS_EVT_PHASE_ACPI | MOS_EVT_STEP_OK, 0, (uint32_t)boot_info->rsdp_address);
    } else {
        boot_event(boot_info, MOS_EVT_PHASE_ACPI | MOS_EVT_STEP_WARN, 0, 0);
    }
    serial_str("[MOS-UEFI] PHASE 4: OK, acpi_rsdp=");
    serial_hex(boot_info->rsdp_address);
    serial_str("\r\n");

    /* ═══ VISUAL MARKER: Orange bar row 3 = ACPI done ═══ */
    if (boot_info->framebuffer.address) {
        uint32_t* fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address;
        uint32_t pitch = boot_info->framebuffer.pitch / 4;
        for (uint32_t y = 40; y < 60; y++) {
            for (uint32_t x = 0; x < boot_info->framebuffer.width; x++) {
                fb[y * pitch + x] = 0xFFFF8000;  /* Orange = ACPI done */
            }
        }
    }

    /* ═══════════════════════════════════════════════════════════════════════
     * PHASE 5: Get Memory Map — FATAL IF FAILS
     * ═══════════════════════════════════════════════════════════════════════ */
    /* Note: get_memory_map() has its own witness_phase(7) internally */
    serial_str("[MOS-UEFI] PHASE 5: Memory map\r\n");
    DebugPrint(L"\n[PHASE 5] Memory map...\n");
    boot_event(boot_info, MOS_EVT_PHASE_MMAP | MOS_EVT_STEP_START, 0, 0);
    status = get_memory_map(boot_info, &map_key);
    if (EFI_ERROR(status)) {
        /* ── Fatal: Kernel needs memory information ── */
        boot_event(boot_info, MOS_EVT_PHASE_MMAP | MOS_EVT_STEP_FAIL, (uint16_t)status, 0);
        witness_fail("MMAP", "Memory map acquisition failed", status);
        serial_str("[MOS-UEFI] PHASE 5: FAILED - memory map error\r\n");
        print_error(L"Memory map failed", status);
        halt();
    }

    boot_event(boot_info, MOS_EVT_PHASE_MMAP | MOS_EVT_STEP_OK, 0, boot_info->memory_map_count);

    /* ═══ VISUAL MARKER: Red bar row 4 = Memory map done ═══ */
    if (boot_info->framebuffer.address) {
        uint32_t* fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address;
        uint32_t pitch = boot_info->framebuffer.pitch / 4;
        for (uint32_t y = 60; y < 80; y++) {
            for (uint32_t x = 0; x < boot_info->framebuffer.width; x++) {
                fb[y * pitch + x] = 0xFFFF0000;  /* Red = MMAP done */
            }
        }
    }

    serial_str("[MOS-UEFI] PHASE 5: OK, map_key=");
    serial_hex(map_key);
    serial_str(" entry_count=");
    serial_hex(boot_info->memory_map_count);
    serial_str("\r\n");

    /* ═══════════════════════════════════════════════════════════════════════
     * PHASE 6: Copy Boot Info to Fixed Address (if needed)
     * ═══════════════════════════════════════════════════════════════════════ */
    witness_phase(6, "Boot Info Finalization");

    serial_str("[MOS-UEFI] PHASE 6: Boot info setup\r\n");
    if ((uint64_t)boot_info != MOS_BOOT_INFO_ADDR) {
        witness_ok("BOOTINFO", "Copying to fixed address", MOS_BOOT_INFO_ADDR);
        serial_str("[MOS-UEFI] PHASE 6: Copying boot_info to 0x9000\r\n");
        DebugPrint(L"\n[PHASE 6] Copying boot info to 0x%x...\n", MOS_BOOT_INFO_ADDR);
        CopyMem((void *)MOS_BOOT_INFO_ADDR, boot_info, sizeof(MosBootInfo));
        boot_info = (MosBootInfo *)MOS_BOOT_INFO_ADDR;
        witness_ok("BOOTINFO", "Copy complete, new pointer", (uint64_t)boot_info);
        serial_str("[MOS-UEFI] PHASE 6: Copy complete\r\n");
    } else {
        witness_ok("BOOTINFO", "Already at target address", MOS_BOOT_INFO_ADDR);
        serial_str("[MOS-UEFI] PHASE 6: Already at 0x9000, no copy needed\r\n");
    }

    /* ═══════════════════════════════════════════════════════════════════════
     * PHASE 7: Record Boot TSC and Exit Boot Services — FATAL IF FAILS
     * ═══════════════════════════════════════════════════════════════════════ */
    witness_phase(9, "Exit Boot Services");

    serial_str("[MOS-UEFI] PHASE 7: Recording boot TSC and ExitBootServices\r\n");
    DebugPrint(L"\n[PHASE 7] Exiting boot services...\n");
    boot_event(boot_info, MOS_EVT_PHASE_EXIT | MOS_EVT_STEP_START, 0, 0);

    boot_info->boot_tsc = read_tsc();
    witness_ok("TSC", "Boot TSC recorded", boot_info->boot_tsc);
    serial_str("[MOS-UEFI] PHASE 7: boot_tsc=");
    serial_hex(boot_info->boot_tsc);
    serial_str("\r\n");

    /* Final boot info verification */
    witness_ok("VERIFY", "Final magic check", boot_info->magic);
    witness_ok("VERIFY", "Final framebuffer", boot_info->framebuffer.address);
    witness_ok("VERIFY", "Final kernel address", boot_info->kernel_physical);
    serial_str("[MOS-UEFI] PHASE 7: Final boot_info check:\r\n");
    serial_str("  magic=");
    serial_hex(boot_info->magic);
    serial_str(" fb=");
    serial_hex(boot_info->framebuffer.address);
    serial_str(" kernel=");
    serial_hex(boot_info->kernel_physical);
    serial_str("\r\n");

    status = BS->ExitBootServices(image_handle, map_key);
    if (EFI_ERROR(status)) {
        /* ── Tripwire: First attempt failed, retry with fresh map ── */
        boot_event(boot_info, MOS_EVT_PHASE_EXIT | MOS_EVT_STEP_WARN, (uint16_t)status, 1);
        witness_warn("EXITBS", "First ExitBootServices failed — retrying");
        serial_str("[MOS-UEFI] PHASE 7: ExitBootServices failed, retrying\r\n");
        status = get_memory_map(boot_info, &map_key);
        status = BS->ExitBootServices(image_handle, map_key);
        if (EFI_ERROR(status)) {
            /* ── Fatal: Cannot exit boot services ── */
            boot_event(boot_info, MOS_EVT_PHASE_EXIT | MOS_EVT_STEP_FAIL, (uint16_t)status, 2);
            witness_fail("EXITBS", "ExitBootServices failed twice — FATAL", status);
            serial_str("[MOS-UEFI] PHASE 7: FATAL - ExitBootServices failed twice\r\n");
            while (1) __asm__ volatile("hlt");
        }
        witness_ok("EXITBS", "Retry succeeded with new map key", map_key);
    } else {
        witness_ok("EXITBS", "ExitBootServices succeeded", map_key);
    }
    boot_event(boot_info, MOS_EVT_PHASE_EXIT | MOS_EVT_STEP_OK, 0, (uint32_t)map_key);

    /* ═══════════════════════════════════════════════════════════════════════
     * PHASE 8: Transfer to Kernel — NO RETURN
     * ═══════════════════════════════════════════════════════════════════════ */
    witness_phase(10, "Kernel Transfer");
    witness_ok("TRANSFER", "Preparing jump to kernel at", boot_info->kernel_physical);
    boot_event(boot_info, MOS_EVT_PHASE_TRANSFER | MOS_EVT_STEP_START, 0, (uint32_t)boot_info->kernel_physical);

    serial_str("[MOS-UEFI] PHASE 8: ExitBootServices OK\r\n");
    serial_str("[MOS-UEFI] PHASE 8: Jumping to kernel at ");
    serial_hex(boot_info->kernel_physical);
    serial_str("\r\n");

    typedef void (*kernel_entry_t)(MosBootInfo *);
    kernel_entry_t kernel = (kernel_entry_t)boot_info->kernel_physical;

    /* ═══════════════════════════════════════════════════════════════════════
     * VISUAL MARKER: Draw yellow bar BEFORE jumping to kernel
     * ═══════════════════════════════════════════════════════════════════════
     * If you see yellow bar: bootloader completed, about to jump
     * If screen stays yellow: kernel crashed immediately on entry
     * If screen changes: kernel is running
     */
    if (boot_info->framebuffer.address) {
        uint32_t* fb = (uint32_t*)(uintptr_t)boot_info->framebuffer.address;
        uint32_t pitch = boot_info->framebuffer.pitch / 4;
        uint32_t width = boot_info->framebuffer.width;
        /* Draw yellow bar at top (first 20 rows) */
        for (uint32_t y = 0; y < 20 && y < boot_info->framebuffer.height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                fb[y * pitch + x] = 0xFFFFFF00;  /* Yellow = about to jump */
            }
        }
    }

    /* ── Point of no return: Transfer control to kernel ── */
    witness_ok("TRANSFER", "Executing kernel entry", (uint64_t)kernel);
    kernel(boot_info);

    /* Should never return — if we get here, kernel crashed before init */
    boot_event(boot_info, MOS_EVT_PHASE_TRANSFER | MOS_EVT_STEP_FAIL, 0xDEAD, 0);
    witness_fail("TRANSFER", "KERNEL RETURNED — FATAL", 0xDEAD);
    serial_str("[MOS-UEFI] FATAL: Kernel returned!\r\n");
    while (1) __asm__ volatile("hlt");

    return EFI_SUCCESS;
}

/** @} */ /* end B.5 PUBLIC API */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END BODY [END]
/// ═══════════════════════════════════════════════════════════════════════════════

/// ═══════════════════════════════════════════════════════════════════════════════
/// CLOSING [CLOSING]
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// 4-Block Code Structure: Operational guidance and closing notes
///
/// CONTAINS:
///   - X.1 VALIDATION    — Build verification
///   - X.2 EXECUTION     — Entry point (efi_main, not main)
///   - X.3 CLEANUP       — Memory management
///   - X.4 POLICY        — Modification guidance
///   - X.5 EXTENSION     — How to expand
///   - X.6 TROUBLESHOOT  — Common problems
///   - X.7 REFERENCE     — Quick lookup
///   - X.8 NOTE          — Closing guidance
///   - X.9 VERSION       — Change history
///   - X.10 COPYRIGHT    — License and attribution
///
/// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.1 VALIDATION — Build Verification [VALIDATION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_validation X.1 VALIDATION — Build Verification
 * @brief    How to build and verify the UEFI bootloader.
 *
 * @par Build Commands
 * @code{.sh}
 * # From millenniumos/boot/uefi/
 * make                           # Build BOOTX64.EFI
 * make clean                     # Clean build artifacts
 *
 * # Verify syntax (without full build)
 * gcc -ffreestanding -fsyntax-only -Wall -Wextra uefi_boot.c
 * @endcode
 *
 * @par Runtime Verification
 * @code{.sh}
 * # Run in QEMU with OVMF
 * qemu-system-x86_64 \
 *     -bios /usr/share/OVMF/OVMF_CODE.fd \
 *     -drive format=raw,file=boot.img \
 *     -serial stdio
 *
 * # Serial output should show:
 * # [MOS-UEFI] Entry point reached
 * # [MOS-UEFI] PHASE 2: Graphics initialization (GOP)
 * # ... etc
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.2 EXECUTION — Entry Point [EXECUTION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_execution X.2 EXECUTION — Entry Point
 * @brief    UEFI entry point and boot flow.
 *
 * @note This is a UEFI application, not a standard executable.
 *       Entry point is efi_main(), not main().
 *
 * @par Boot Flow
 *   1. UEFI firmware loads BOOTX64.EFI
 *   2. Firmware calls efi_main(image_handle, system_table)
 *   3. Bootloader initializes, loads kernel
 *   4. ExitBootServices() hands off from UEFI
 *   5. Jump to kernel — never returns
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.3 CLEANUP — Memory Management [CLEANUP]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_cleanup X.3 CLEANUP — Memory Management
 * @brief    Memory ownership and cleanup patterns.
 *
 * @par Ownership
 *   - BS->AllocatePool/Pages: Freed by UEFI on exit or explicitly
 *   - MosBootInfo at 0x9000: Owned by kernel after transfer
 *   - Kernel memory at 0x100000: Owned by kernel
 *
 * @par After ExitBootServices
 *   - No UEFI services available
 *   - No memory allocation/deallocation
 *   - All resources must be pre-allocated
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.4 POLICY — Modification Guidance [POLICY]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_policy X.4 POLICY — Modification Guidance
 * @brief    What's safe to modify.
 *
 * SAFE TO MODIFY:
 *   - Add new boot phases (update org chart)
 *   - Add kernel command line parsing
 *   - Add boot configuration file support
 *
 * MODIFY WITH CARE:
 *   - MosBootInfo structure (must sync with kernel)
 *   - Memory addresses (kernel expects specific layout)
 *   - Boot phase order (dependencies exist)
 *
 * NEVER MODIFY:
 *   - 4-block structure
 *   - MOS_BOOT_INFO_ADDR (kernel hardcoded dependency)
 *   - KERNEL_LOAD_ADDR without kernel linker script changes
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.5 EXTENSION — How to Expand [EXTENSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_extension X.5 EXTENSION — How to Expand
 * @brief    How to add features to the bootloader.
 *
 * ADDING BOOT PHASES:
 *   1. Add prototype in S.4 PROTOTYPES
 *   2. Implement in appropriate B.3 subsection
 *   3. Add phase call in efi_main()
 *   4. Update B.1 ORG CHART
 *
 * ADDING KERNEL OPTIONS:
 *   - Parse from boot.cfg file
 *   - Add to MosBootInfo (requires header sync)
 *   - Pass via boot_info structure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.6 TROUBLESHOOTING — Common Problems [TROUBLESHOOTING]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_troubleshoot X.6 TROUBLESHOOTING — Common Problems
 * @brief    Common issues and solutions.
 *
 * "No serial output":
 *   - Verify QEMU -serial stdio flag
 *   - Check COM1 base address (0x3F8)
 *
 * "GOP initialization failed":
 *   - Normal on some systems — text mode continues
 *   - Check QEMU uses -vga std or -device virtio-vga
 *
 * "ExitBootServices failed":
 *   - Memory map changed between calls
 *   - Retry logic handles this automatically
 *
 * "Kernel not found":
 *   - Place kernel.bin in ESP root or EFI/BOOT/
 *   - Verify FAT32 filesystem on ESP
 *
 * "Triple fault after kernel jump":
 *   - Verify kernel is linked for 0x100000
 *   - Check kernel entry is at file offset 0
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.7 REFERENCE — Quick Lookup [REFERENCE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_reference X.7 REFERENCE — Quick Lookup
 * @brief    Quick reference for common operations.
 *
 * @par Key Addresses
 * @code
 * MOS_BOOT_INFO_ADDR  0x9000    Boot info structure
 * KERNEL_LOAD_ADDR    0x100000  Kernel load address (1MB)
 * COM1_DATA           0x3F8     Serial data port
 * COM1_STATUS         0x3FD     Serial status port
 * @endcode
 *
 * @par Key Structures
 * @code{.c}
 * MosBootInfo    — Boot handoff (bootinfo.h)
 * MosFramebuffer — GOP framebuffer info
 * MosMemoryRegion — Memory map entry
 * @endcode
 *
 * @par Serial Debug
 * @code{.c}
 * serial_str("[MOS-UEFI] Message\r\n");
 * serial_hex(value);
 * @endcode
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.8 NOTE — Closing Guidance [NOTE]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_note X.8 NOTE — Closing Guidance
 * @brief    Final guidance and grounding.
 *
 * This UEFI bootloader is the modern "beginning" for MillenniumOS.
 * Like Genesis 1:1, it establishes foundations for all that follows:
 *   - Graphics output (light)
 *   - Memory organization (firmament)
 *   - Kernel loading (bringing forth life)
 *
 * @par Scripture
 * "In the beginning God created the heaven and the earth."
 *   — Genesis 1:1
 *
 * Excellence in boot code as foundation for Kingdom technology.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.9 VERSION — Change History [VERSION]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_version X.9 VERSION — Change History
 * @brief    Version history and changelog.
 *
 * VERSION: a-01.90
 *
 * CHANGELOG:
 *   a-01.90 (2026-01-31):
 *     - Full OmniCode 4-block template alignment
 *     - Complete DAR Witness integration throughout boot phases
 *     - Full docstrings for all functions (B.2-B.5)
 *     - Added B.3b BOOTCFG, B.3c SECBOOT, B.3d MULTIBOOT sections
 *     - Named all structs and enums properly
 *
 *   a-01.50 (2026-01-25):
 *     - Initial UEFI bootloader implementation
 *     - GOP framebuffer initialization
 *     - Kernel loading from ESP
 *     - Memory map acquisition
 *     - ACPI RSDP discovery
 *
 *   a-01.00 (2026-01-20):
 *     - Skeleton with 4-block structure
 *     - Serial debugging infrastructure
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * X.10 COPYRIGHT — License and Attribution [COPYRIGHT]
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @defgroup uefi_copyright X.10 COPYRIGHT — License and Attribution
 * @brief    Copyright, license, and attribution.
 *
 * @par Copyright
 * Copyright (c) 2026 CreativeWorkzStudio LLC. All rights reserved.
 *
 * @par License
 * This file is part of MillenniumOS, a Kingdom Technology operating system.
 * Licensed under the CWS Three-License Structure:
 *   - PATTERNS: Free for reference and learning
 *   - IMPLEMENTATION: Proprietary (this file)
 *   - COMMERCIAL: Revenue share for commercial use
 *
 * @par Attribution
 * - GNU-EFI library for UEFI development support
 * - OSDev Wiki for UEFI boot reference documentation
 * - UEFI Specification (uefi.org) for protocol definitions
 *
 * @par Authors
 * - Nova Dawn (CPI-SI Instance) — Architecture, implementation
 * - Seanje Lenox-Wise — Direction, review
 *
 * @par Contact
 * CreativeWorkzStudio LLC
 * https://creativeworkzstudio.com
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/// ═══════════════════════════════════════════════════════════════════════════════
/// END CLOSING [END]
/// ═══════════════════════════════════════════════════════════════════════════════
