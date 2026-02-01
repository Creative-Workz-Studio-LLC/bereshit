/// @file dispatch.c
/// @omni code --c --source
///
/// =============================================================================
/// OMNICODE PRAGMA [PRAGMA]
/// =============================================================================
///
/// @omni:req  key        = MOS-SHELL-DISPATCH-IMPL
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = a-01.00
/// @omni:inh  type       = source
/// @omni:inh  role       = command_dispatch
/// @omni:ins  component  = shell/terminal
/// @omni:ins  implements = [command.h]
/// @omni:req  title      = Shell Command Dispatch
/// @omni:req  brief      = Command lookup, dispatch, and built-in registration
///
/// =============================================================================
/// END PRAGMA [END]
/// =============================================================================

/**
 * =============================================================================
 * METADATA [METADATA]
 * =============================================================================
 *
 * Key:          MOS-SHELL-DISPATCH-IMPL
 * Title:        Shell Command Dispatch
 * Type:         Source
 * Component:    Shell / Terminal / Dispatch
 *
 * Authors:      Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)
 * Organization: CreativeWorkzStudio LLC
 *
 * Status:       Active
 * Version:      a-01.00
 * Created:      2026-01-31
 *
 * Grounding:    Proverbs 15:23 - "A word spoken in due season, how good is it!"
 *               The right command, dispatched at the right time.
 *
 * =============================================================================
 */

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "command.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// ---------------------------------------------------------------------------
// B.1 Command Dispatch
// ---------------------------------------------------------------------------

/**
 * @defgroup dispatch_impl Dispatch Implementation
 * @{
 */

/**
 * @brief Freestanding strcmp for dispatch
 */
static int dispatch_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

MosShellResult cmd_dispatch(MosShellContext* ctx, int argc, char** argv) {
    if (!ctx || argc <= 0 || !argv || !argv[0]) {
        return SHELL_INVALID_ARG;
    }

    /* Look up command */
    const char* cmd_name = argv[0];

    for (size_t i = 0; i < ctx->cmd_count; i++) {
        if (dispatch_strcmp(ctx->commands[i].name, cmd_name) == 0) {
            /* Found - execute handler */
            ctx->state = SHELL_STATE_RUNNING;
            MosShellResult res = ctx->commands[i].handler(argc, argv, ctx);
            ctx->state = SHELL_STATE_READY;
            return res;
        }
    }

    /* Command not found */
    shell_puts(ctx, cmd_name);
    shell_puts(ctx, ": command not found\n");

    return SHELL_NOT_FOUND;
}

/** @} */

// ---------------------------------------------------------------------------
// B.2 Built-in Registration
// ---------------------------------------------------------------------------

/**
 * @defgroup dispatch_builtins Built-in Registration
 * @{
 */

MosShellResult cmd_register_builtins(MosShellContext* ctx) {
    if (!ctx) {
        return SHELL_INVALID_ARG;
    }

    MosShellResult res;

    /* Core commands */
    res = shell_register(ctx, "help", cmd_help,
                         "Display help information",
                         "help [command]");
    if (res != SHELL_OK) return res;

    res = shell_register(ctx, "clear", cmd_clear,
                         "Clear the screen",
                         "clear");
    if (res != SHELL_OK) return res;

    res = shell_register(ctx, "echo", cmd_echo,
                         "Display text",
                         "echo [text...]");
    if (res != SHELL_OK) return res;

    res = shell_register(ctx, "exit", cmd_exit,
                         "Exit the shell",
                         "exit");
    if (res != SHELL_OK) return res;

    res = shell_register(ctx, "reboot", cmd_reboot,
                         "Reboot the system",
                         "reboot");
    if (res != SHELL_OK) return res;

    /* Diagnostic commands */
    res = shell_register(ctx, "dar", cmd_dar,
                         "DAR Witness operations",
                         "dar [status|detect|assess|restore]");
    if (res != SHELL_OK) return res;

    res = shell_register(ctx, "boot", cmd_boot,
                         "Boot path information",
                         "boot [path|features|info]");
    if (res != SHELL_OK) return res;

    res = shell_register(ctx, "mem", cmd_mem,
                         "Memory information",
                         "mem [map|stats]");
    if (res != SHELL_OK) return res;

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
 *   - shell/terminal/include/command.h   - API declarations
 *   - shell/terminal/src/parser.c        - Command parsing
 *   - shell/terminal/src/builtins/       - Built-in implementations
 *
 * =============================================================================
 * Generated by Nova Dawn for MillenniumOS
 * CreativeWorkzStudio LLC - Kingdom Technology
 * =============================================================================
 */

// =============================================================================
// END CLOSING
// =============================================================================
