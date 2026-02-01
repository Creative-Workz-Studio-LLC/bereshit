/// @file mem.c
/// @omni code --c --source
///
/// =============================================================================
/// OMNICODE PRAGMA [PRAGMA]
/// =============================================================================
///
/// @omni:req  key        = MOS-SHELL-BUILTINS-MEM
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = a-01.00
/// @omni:inh  type       = source
/// @omni:inh  role       = builtin_commands
/// @omni:ins  component  = shell/terminal/builtins
/// @omni:ins  provides   = [cmd_mem]
/// @omni:req  title      = Shell Memory Command
/// @omni:req  brief      = Display memory map and statistics
///
/// =============================================================================
/// END PRAGMA [END]
/// =============================================================================

/**
 * =============================================================================
 * METADATA [METADATA]
 * =============================================================================
 *
 * Key:          MOS-SHELL-BUILTINS-MEM
 * Title:        Shell Memory Command
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
 * Grounding:    Proverbs 2:10 - "When wisdom entereth into thine heart"
 *               Memory inspection reveals the system's stored knowledge.
 *
 * =============================================================================
 */

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "shell.h"
#include "command.h"

// Forward declarations for boot info access
extern const MosBootInfo* mos_get_boot_info(void);

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// ---------------------------------------------------------------------------
// B.1 Formatting Helpers
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_mem_helpers Memory Formatting Helpers
 * @{
 */

/**
 * @brief Print hex value
 */
static void mem_print_hex64(MosShellContext* ctx, uint64_t value) {
    static const char hex[] = "0123456789abcdef";
    char buf[17];

    for (int i = 15; i >= 0; i--) {
        buf[i] = hex[value & 0xF];
        value >>= 4;
    }
    buf[16] = '\0';

    shell_puts(ctx, "0x");
    shell_puts(ctx, buf);
}

/**
 * @brief Print size in human readable format (KB/MB/GB)
 */
static void mem_print_size(MosShellContext* ctx, uint64_t bytes) {
    char buf[12];
    int i;
    const char* suffix;
    uint64_t value;

    if (bytes >= (1ULL << 30)) {
        value = bytes >> 30;
        suffix = " GB";
    } else if (bytes >= (1ULL << 20)) {
        value = bytes >> 20;
        suffix = " MB";
    } else if (bytes >= (1ULL << 10)) {
        value = bytes >> 10;
        suffix = " KB";
    } else {
        value = bytes;
        suffix = " B";
    }

    /* Print number */
    i = 11;
    buf[i] = '\0';

    if (value == 0) {
        shell_puts(ctx, "0");
    } else {
        while (value > 0 && i > 0) {
            buf[--i] = '0' + (value % 10);
            value /= 10;
        }
        shell_puts(ctx, &buf[i]);
    }

    shell_puts(ctx, suffix);
}

/**
 * @brief Get memory type name
 */
static const char* mem_type_name(uint32_t type) {
    switch (type) {
        case 0:  return "Reserved";
        case 1:  return "Available";
        case 2:  return "Reserved";
        case 3:  return "ACPI Reclaim";
        case 4:  return "ACPI NVS";
        case 5:  return "Bad Memory";
        case 6:  return "Disabled";
        case 7:  return "Persistent";
        case 8:  return "Undefined";
        case 9:  return "Loader Code";
        case 10: return "Loader Data";
        case 11: return "Boot Services Code";
        case 12: return "Boot Services Data";
        case 13: return "Runtime Code";
        case 14: return "Runtime Data";
        default: return "Unknown";
    }
}

/** @} */

// ---------------------------------------------------------------------------
// B.2 Memory Map Display
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_mem_map Memory Map
 * @{
 */

/**
 * @brief Print unsigned integer helper
 */
static void mem_print_uint(MosShellContext* ctx, uint32_t value) {
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
 * @brief Display memory map
 */
static void show_mem_map(MosShellContext* ctx) {
    shell_puts(ctx, "=== Memory Map ===\n\n");

    /* Check if memory map is available */
    if (!mos_has_boot_feature(MOS_FEAT_MEMMAP)) {
        shell_puts(ctx, "Memory map not available.\n");
        return;
    }

    const MosBootInfo* boot = mos_get_boot_info();
    if (!boot) {
        shell_puts(ctx, "Boot info not available.\n");
        return;
    }

    shell_puts(ctx, "Entries: ");
    mem_print_uint(ctx, boot->memory_map_count);
    shell_newline(ctx);
    shell_newline(ctx);

    /* Header */
    shell_puts(ctx, "Base             Length           Type\n");
    shell_puts(ctx, "---------------- ---------------- ----------------\n");

    /* Print entries */
    for (uint32_t i = 0; i < boot->memory_map_count && i < 32; i++) {
        const MosMemoryRegion* entry = &boot->memory_map[i];

        mem_print_hex64(ctx, entry->base);
        shell_puts(ctx, " ");
        mem_print_hex64(ctx, entry->length);
        shell_puts(ctx, " ");
        shell_puts(ctx, mem_type_name(entry->type));
        shell_newline(ctx);
    }

    if (boot->memory_map_count > 32) {
        shell_puts(ctx, "... (");
        mem_print_uint(ctx, boot->memory_map_count - 32);
        shell_puts(ctx, " more entries)\n");
    }
}

/** @} */

// ---------------------------------------------------------------------------
// B.3 Memory Statistics
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_mem_stats Memory Statistics
 * @{
 */

/**
 * @brief Display memory statistics
 */
static void show_mem_stats(MosShellContext* ctx) {
    shell_puts(ctx, "=== Memory Statistics ===\n\n");

    /* Check if memory map is available */
    if (!mos_has_boot_feature(MOS_FEAT_MEMMAP)) {
        shell_puts(ctx, "Memory map not available.\n");
        return;
    }

    const MosBootInfo* boot = mos_get_boot_info();
    if (!boot) {
        shell_puts(ctx, "Boot info not available.\n");
        return;
    }

    /* Calculate totals */
    uint64_t total_usable = 0;
    uint64_t total_reserved = 0;
    uint64_t total_acpi = 0;
    uint64_t highest_addr = 0;

    for (uint32_t i = 0; i < boot->memory_map_count; i++) {
        const MosMemoryRegion* entry = &boot->memory_map[i];
        uint64_t end = entry->base + entry->length;

        if (end > highest_addr) {
            highest_addr = end;
        }

        switch (entry->type) {
            case 1:  /* Available */
                total_usable += entry->length;
                break;
            case 3:  /* ACPI Reclaim */
            case 4:  /* ACPI NVS */
                total_acpi += entry->length;
                break;
            default:
                total_reserved += entry->length;
                break;
        }
    }

    /* Display statistics */
    shell_puts(ctx, "Usable Memory:   ");
    mem_print_size(ctx, total_usable);
    shell_newline(ctx);

    shell_puts(ctx, "Reserved Memory: ");
    mem_print_size(ctx, total_reserved);
    shell_newline(ctx);

    shell_puts(ctx, "ACPI Memory:     ");
    mem_print_size(ctx, total_acpi);
    shell_newline(ctx);

    shell_newline(ctx);

    shell_puts(ctx, "Highest Address: ");
    mem_print_hex64(ctx, highest_addr);
    shell_newline(ctx);

    shell_puts(ctx, "Map Entries:     ");
    mem_print_uint(ctx, boot->memory_map_count);
    shell_newline(ctx);

    /* Framebuffer info if available */
    if (mos_has_boot_feature(MOS_FEAT_FRAMEBUFFER)) {
        shell_newline(ctx);
        shell_puts(ctx, "Framebuffer:\n");
        shell_puts(ctx, "  Base:   ");
        mem_print_hex64(ctx, boot->framebuffer.address);
        shell_newline(ctx);
        shell_puts(ctx, "  Size:   ");
        mem_print_size(ctx, (uint64_t)boot->framebuffer.pitch * boot->framebuffer.height);
        shell_newline(ctx);
        shell_puts(ctx, "  Res:    ");
        mem_print_uint(ctx, boot->framebuffer.width);
        shell_puts(ctx, "x");
        mem_print_uint(ctx, boot->framebuffer.height);
        shell_puts(ctx, "x");
        mem_print_uint(ctx, boot->framebuffer.bpp);
        shell_newline(ctx);
    }
}

/** @} */

// ---------------------------------------------------------------------------
// B.4 Command Entry Point
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_mem_entry Memory Command Entry
 * @{
 */

/**
 * @brief Freestanding strcmp
 */
static int mem_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

MosShellResult cmd_mem(int argc, char** argv, MosShellContext* ctx) {
    if (!ctx) return SHELL_INVALID_ARG;

    if (argc == 1) {
        /* No subcommand - show stats */
        show_mem_stats(ctx);
        return SHELL_OK;
    }

    const char* subcmd = argv[1];

    if (mem_strcmp(subcmd, "map") == 0) {
        show_mem_map(ctx);
    } else if (mem_strcmp(subcmd, "stats") == 0) {
        show_mem_stats(ctx);
    } else {
        shell_puts(ctx, "Unknown subcommand: ");
        shell_puts(ctx, subcmd);
        shell_newline(ctx);
        shell_puts(ctx, "Usage: mem [map|stats]\n");
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
 *   - kernel/src/core/memory/memory.c   - Memory manager
 *   - boot/include/bootinfo.h           - Boot info structures
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
