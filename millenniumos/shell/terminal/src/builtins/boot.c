/// @file boot.c
/// @omni code --c --source
///
/// =============================================================================
/// OMNICODE PRAGMA [PRAGMA]
/// =============================================================================
///
/// @omni:req  key        = MOS-SHELL-BUILTINS-BOOT
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = a-01.00
/// @omni:inh  type       = source
/// @omni:inh  role       = builtin_commands
/// @omni:ins  component  = shell/terminal/builtins
/// @omni:ins  provides   = [cmd_boot]
/// @omni:req  title      = Shell Boot Info Command
/// @omni:req  brief      = Display boot path, features, and boot info
///
/// =============================================================================
/// END PRAGMA [END]
/// =============================================================================

/**
 * =============================================================================
 * METADATA [METADATA]
 * =============================================================================
 *
 * Key:          MOS-SHELL-BUILTINS-BOOT
 * Title:        Shell Boot Info Command
 * Type:         Source
 * Component:    Shell / Terminal / Builtins
 *
 * Authors:      Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)
 * Organization: CreativeWorkzStudio LLC
 *
 * Status:       Active
 * Version:      a-01.00
 * Created:      2026-01-31
 *
 * Grounding:    Psalm 18:33 - "He maketh my feet like hinds' feet"
 *               Understanding the boot path illuminates how we arrived.
 *
 * =============================================================================
 */

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "shell.h"
#include "command.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// ---------------------------------------------------------------------------
// B.1 Boot Path Display
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_boot_path Boot Path Display
 * @{
 */

/**
 * @brief Display boot path (UEFI/BIOS/Unknown)
 */
static void show_boot_path(MosShellContext* ctx) {
    trit_t path = mos_boot_path();

    shell_puts(ctx, "Boot Path: ");

    switch (path) {
        case TRIT_POS:
            shell_puts(ctx, "UEFI (+1)\n");
            shell_puts(ctx, "  Mode: 64-bit Long Mode\n");
            shell_puts(ctx, "  Graphics: GOP Framebuffer\n");
            break;

        case TRIT_NEG:
            shell_puts(ctx, "BIOS (-1)\n");
            shell_puts(ctx, "  Mode: 32-bit Protected Mode\n");
            shell_puts(ctx, "  Graphics: VESA/VBE Framebuffer\n");
            break;

        case TRIT_ZERO:
        default:
            shell_puts(ctx, "Unknown (0)\n");
            shell_puts(ctx, "  Mode: Undetermined\n");
            break;
    }
}

/** @} */

// ---------------------------------------------------------------------------
// B.2 Boot Features Display
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_boot_features Boot Features Display
 * @{
 */

/**
 * @brief Print hex value helper
 */
static void print_hex(MosShellContext* ctx, uint32_t value) {
    static const char hex[] = "0123456789abcdef";
    char buf[9];

    for (int i = 7; i >= 0; i--) {
        buf[i] = hex[value & 0xF];
        value >>= 4;
    }
    buf[8] = '\0';

    shell_puts(ctx, "0x");
    shell_puts(ctx, buf);
}

/**
 * @brief Print unsigned integer helper
 */
__attribute__((unused))
static void print_uint(MosShellContext* ctx, uint32_t value) {
    char buf[12];
    int i = 11;
    buf[i] = '\0';

    if (value == 0) {
        shell_puts(ctx, "0");
        return;
    }

    while (value > 0 && i > 0) {
        buf[--i] = '0' + (value % 10);
        value /= 10;
    }

    shell_puts(ctx, &buf[i]);
}

/**
 * @brief Display boot features
 */
static void show_boot_features(MosShellContext* ctx) {
    uint32_t features = mos_boot_features();

    shell_puts(ctx, "Boot Features: ");
    print_hex(ctx, features);
    shell_newline(ctx);
    shell_newline(ctx);

    shell_puts(ctx, "  [");
    shell_puts(ctx, mos_has_boot_feature(MOS_FEAT_ACPI) ? "X" : " ");
    shell_puts(ctx, "] ACPI RSDP\n");

    shell_puts(ctx, "  [");
    shell_puts(ctx, mos_has_boot_feature(MOS_FEAT_FRAMEBUFFER) ? "X" : " ");
    shell_puts(ctx, "] Framebuffer\n");

    shell_puts(ctx, "  [");
    shell_puts(ctx, mos_has_boot_feature(MOS_FEAT_HIMEM_FB) ? "X" : " ");
    shell_puts(ctx, "] High Memory FB (>4GB)\n");

    shell_puts(ctx, "  [");
    shell_puts(ctx, mos_has_boot_feature(MOS_FEAT_MEMMAP) ? "X" : " ");
    shell_puts(ctx, "] Memory Map\n");

    shell_puts(ctx, "  [");
    shell_puts(ctx, mos_has_boot_feature(MOS_FEAT_RUNTIME) ? "X" : " ");
    shell_puts(ctx, "] UEFI Runtime Services\n");
}

/** @} */

// ---------------------------------------------------------------------------
// B.3 Full Boot Info Display
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_boot_info Full Boot Info
 * @{
 */

/**
 * @brief Display full boot information
 */
static void show_boot_info(MosShellContext* ctx) {
    shell_puts(ctx, "=== MillenniumOS Boot Information ===\n\n");

    /* Boot path */
    show_boot_path(ctx);
    shell_newline(ctx);

    /* Features */
    show_boot_features(ctx);
    shell_newline(ctx);

    /* Convenience queries */
    shell_puts(ctx, "Convenience Queries:\n");
    shell_puts(ctx, "  mos_is_uefi_boot() = ");
    shell_puts(ctx, mos_is_uefi_boot() ? "true" : "false");
    shell_newline(ctx);
    shell_puts(ctx, "  mos_is_bios_boot() = ");
    shell_puts(ctx, mos_is_bios_boot() ? "true" : "false");
    shell_newline(ctx);
}

/** @} */

// ---------------------------------------------------------------------------
// B.4 Command Entry Point
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_boot_entry Boot Command Entry
 * @{
 */

/**
 * @brief Freestanding strcmp
 */
static int boot_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

MosShellResult cmd_boot(int argc, char** argv, MosShellContext* ctx) {
    if (!ctx) return SHELL_INVALID_ARG;

    if (argc == 1) {
        /* No subcommand - show all */
        show_boot_info(ctx);
        return SHELL_OK;
    }

    const char* subcmd = argv[1];

    if (boot_strcmp(subcmd, "path") == 0) {
        show_boot_path(ctx);
    } else if (boot_strcmp(subcmd, "features") == 0) {
        show_boot_features(ctx);
    } else if (boot_strcmp(subcmd, "info") == 0) {
        show_boot_info(ctx);
    } else {
        shell_puts(ctx, "Unknown subcommand: ");
        shell_puts(ctx, subcmd);
        shell_newline(ctx);
        shell_puts(ctx, "Usage: boot [path|features|info]\n");
        return SHELL_INVALID_ARG;
    }

    return SHELL_OK;
}

/** @} */

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

/**
 * =============================================================================
 * FILE FOOTER
 * =============================================================================
 *
 * Related:
 *   - kernel/include/kernel.h           - Boot path API
 *   - kernel/src/kernel.c               - Boot path implementation
 *   - shell/terminal/include/shell.h    - Shell API
 *
 * =============================================================================
 * Generated by Nova Dawn for MillenniumOS
 * CreativeWorkzStudio LLC - Kingdom Technology
 * =============================================================================
 */

// =============================================================================
// END CLOSING
// =============================================================================
