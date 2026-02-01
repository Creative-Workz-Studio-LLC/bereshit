/// @file core.c
/// @omni code --c --source
///
/// =============================================================================
/// OMNICODE PRAGMA [PRAGMA]
/// =============================================================================
///
/// @omni:req  key        = MOS-SHELL-BUILTINS-CORE
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = a-01.00
/// @omni:inh  type       = source
/// @omni:inh  role       = builtin_commands
/// @omni:ins  component  = shell/terminal/builtins
/// @omni:ins  provides   = [cmd_help, cmd_clear, cmd_echo, cmd_exit, cmd_reboot]
/// @omni:req  title      = Shell Core Built-in Commands
/// @omni:req  brief      = Essential shell commands - help, clear, echo, exit, reboot
///
/// =============================================================================
/// END PRAGMA [END]
/// =============================================================================

/**
 * =============================================================================
 * METADATA [METADATA]
 * =============================================================================
 *
 * Key:          MOS-SHELL-BUILTINS-CORE
 * Title:        Shell Core Built-in Commands
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
 * Grounding:    Psalm 119:130 - "The entrance of thy words giveth light"
 *               Core commands illuminate the shell's capabilities.
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
// B.1 Help Command
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_help_impl Help Command
 * @{
 */

/**
 * @brief Freestanding strcmp for builtins
 */
__attribute__((unused))
static int builtin_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

MosShellResult cmd_help(int argc, char** argv, MosShellContext* ctx) {
    if (!ctx) return SHELL_INVALID_ARG;

    if (argc == 1) {
        /* List all commands */
        shell_puts(ctx, "Available commands:\n\n");

        for (size_t i = 0; i < ctx->cmd_count; i++) {
            shell_puts(ctx, "  ");
            shell_puts(ctx, ctx->commands[i].name);

            /* Pad to column 12 */
            size_t name_len = 0;
            const char* p = ctx->commands[i].name;
            while (*p++) name_len++;

            for (size_t pad = name_len; pad < 12; pad++) {
                shell_puts(ctx, " ");
            }

            shell_puts(ctx, ctx->commands[i].help_brief);
            shell_newline(ctx);
        }

        shell_newline(ctx);
        shell_puts(ctx, "Use 'help <command>' for detailed help.\n");

    } else {
        /* Help for specific command */
        const char* cmd_name = argv[1];
        const MosShellCommand* cmd = shell_find_command(ctx, cmd_name);

        if (cmd) {
            shell_puts(ctx, cmd->name);
            shell_puts(ctx, " - ");
            shell_puts(ctx, cmd->help_brief);
            shell_newline(ctx);
            shell_newline(ctx);
            shell_puts(ctx, "Usage: ");
            shell_puts(ctx, cmd->help_usage);
            shell_newline(ctx);
        } else {
            shell_puts(ctx, "Unknown command: ");
            shell_puts(ctx, cmd_name);
            shell_newline(ctx);
            return SHELL_NOT_FOUND;
        }
    }

    return SHELL_OK;
}

/** @} */

// ---------------------------------------------------------------------------
// B.2 Clear Command
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_clear_impl Clear Command
 * @{
 */

MosShellResult cmd_clear(int argc, char** argv, MosShellContext* ctx) {
    (void)argc;
    (void)argv;

    if (!ctx) return SHELL_INVALID_ARG;

    shell_clear(ctx);

    return SHELL_OK;
}

/** @} */

// ---------------------------------------------------------------------------
// B.3 Echo Command
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_echo_impl Echo Command
 * @{
 */

MosShellResult cmd_echo(int argc, char** argv, MosShellContext* ctx) {
    if (!ctx) return SHELL_INVALID_ARG;

    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            shell_puts(ctx, " ");
        }
        shell_puts(ctx, argv[i]);
    }
    shell_newline(ctx);

    return SHELL_OK;
}

/** @} */

// ---------------------------------------------------------------------------
// B.4 Exit Command
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_exit_impl Exit Command
 * @{
 */

MosShellResult cmd_exit(int argc, char** argv, MosShellContext* ctx) {
    (void)argc;
    (void)argv;

    if (!ctx) return SHELL_INVALID_ARG;

    shell_puts(ctx, "Exiting shell...\n");

    return SHELL_EXIT_REQUEST;
}

/** @} */

// ---------------------------------------------------------------------------
// B.5 Reboot Command
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_reboot_impl Reboot Command
 * @{
 */

MosShellResult cmd_reboot(int argc, char** argv, MosShellContext* ctx) {
    (void)argc;
    (void)argv;

    if (!ctx) return SHELL_INVALID_ARG;

    shell_puts(ctx, "Rebooting system...\n");

    /* Attempt ACPI reboot via keyboard controller */
    /* TODO: Use HAL reboot function when available */

    /* Triple fault method (fallback) */
    /* Load invalid IDT and trigger interrupt */
    __asm__ volatile (
        "cli\n"
        "lidt 0\n"
        "int $0x03\n"
    );

    /* If we get here, reboot failed */
    shell_puts(ctx, "Reboot failed!\n");

    return SHELL_ERROR;
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
 *   - shell/terminal/include/shell.h    - Shell API
 *   - shell/terminal/src/dispatch.c     - Command registration
 *   - shell/terminal/src/builtins/      - Other built-ins
 *
 * =============================================================================
 * Generated by Nova Dawn for MillenniumOS
 * CreativeWorkzStudio LLC - Kingdom Technology
 * =============================================================================
 */

// =============================================================================
// END CLOSING
// =============================================================================
