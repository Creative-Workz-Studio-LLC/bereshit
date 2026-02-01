/// @file shell.h
/// @omni code --c --header
///
/// =============================================================================
/// OMNICODE PRAGMA [PRAGMA]
/// =============================================================================
///
/// @legend
///   :req  REQUIRED   - must exist, validation fails without
///   :inh  INHERITED  - from template, override allowed
///   :ins  INSTANCE   - file-specific, unique values
/// @endlegend
///
/// ---------------------------------------------------------------------------
/// P.1 CORE - Identity [CORE]
/// ---------------------------------------------------------------------------
///
/// @omni:req  key        = MOS-SHELL-TERMINAL-API
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = a-01.00
///
/// ---------------------------------------------------------------------------
/// P.2 FAMILY - Classification [FAMILY]
/// ---------------------------------------------------------------------------
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = api
/// @omni:inh  role       = shell_interface
/// @omni:inh  structure  = 4-block
///
/// ---------------------------------------------------------------------------
/// P.3 INSTANCE - File Details [INSTANCE]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  component  = shell
/// @omni:ins  layer      = terminal
/// @omni:ins  provides   = [shell_api, shell_context, shell_result]
///
/// ---------------------------------------------------------------------------
/// P.4 ARCHITECTURE - Scaling Context [ARCH]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  layers     = [0:kernel, 1:hal, 2:services, 3:shell, 4:apps]
/// @omni:ins  position   = 3 (shell layer)
/// @omni:ins  depends    = [kernel, hal, graphics, keyboard]
///
/// ---------------------------------------------------------------------------
/// P.5 SUMMARY - Human Identity [SUMMARY]
/// ---------------------------------------------------------------------------
///
/// @omni:req  title      = MillenniumOS Terminal Shell API
/// @omni:req  brief      = Freestanding terminal shell for kernel-level interaction
///
/// =============================================================================
/// END PRAGMA [END]
/// =============================================================================

/**
 * =============================================================================
 * METADATA [METADATA]
 * =============================================================================
 */

/**
 * =============================================================================
 * M.1 IDENTITY - Core Identification [IDENTITY]
 * =============================================================================
 *
 * @defgroup shell_identity Shell Identity
 * @{
 *
 * Key:          MOS-SHELL-TERMINAL-API
 * Title:        MillenniumOS Terminal Shell API
 * Type:         Header
 * Component:    Shell / Terminal
 * Role:         Public API for terminal shell operations
 *
 * @}
 */

/**
 * =============================================================================
 * M.2 AUTHORS - Attribution [AUTHORS]
 * =============================================================================
 *
 * Authors:      Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)
 * Organization: CreativeWorkzStudio LLC
 * Copyright:    2026 CreativeWorkzStudio LLC. All rights reserved.
 */

/**
 * =============================================================================
 * M.3 VERSION - Lifecycle State [VERSION]
 * =============================================================================
 *
 * Status:       Active
 * Version:      a-01.00 (Alpha, Phase 1, 0% complete)
 * Created:      2026-01-31
 * Updated:      2026-01-31
 */

/**
 * =============================================================================
 * M.4 BIBLICAL - Kingdom Foundation [BIBLICAL]
 * =============================================================================
 *
 * Grounding:    Psalm 119:105 - "Thy word is a lamp unto my feet, and a
 *               light unto my path."
 *
 * Application:  The shell illuminates the system state, command by command,
 *               revealing what lies beneath - a lamp for the operator's path.
 */

/**
 * =============================================================================
 * M.5 CONTEXT - Purpose Statement [CONTEXT]
 * =============================================================================
 *
 * Purpose:      Provide interactive command-line interface for kernel
 *               diagnostics, DAR witness queries, and system management.
 *
 * Audience:     Kernel developers, system administrators, diagnostic tools
 *
 * Constraints:  Freestanding (no libc), works in CPISI_MODE 0-1
 */

/**
 * =============================================================================
 * M.6 SCOPE - What This Covers [SCOPE]
 * =============================================================================
 *
 * Includes:
 *   - Shell context and state management
 *   - Command registration and dispatch
 *   - Input/output abstraction
 *   - Built-in command declarations
 *
 * Excludes:
 *   - OmniCode REPL (shell/omni layer)
 *   - GUI shell (shell/desktop layer)
 *   - HAL keyboard driver (hal/ layer)
 */

/**
 * =============================================================================
 * M.7 ARCHITECTURE - System Position [ARCH]
 * =============================================================================
 *
 * Layer:        Shell (Layer 3)
 * Position:     Above services, below applications
 *
 * Data Flow:
 *   Keyboard -> HAL -> Shell -> Parser -> Command -> Output -> Framebuffer
 *
 * Dependencies:
 *   - kernel.h (boot path, result types)
 *   - hal/keyboard.h (input)
 *   - services/graphics.h (output)
 */

/**
 * =============================================================================
 * M.8 CONTRACTS - Guarantees [CONTRACTS]
 * =============================================================================
 *
 * Preconditions:
 *   - Framebuffer initialized (graphics_init called)
 *   - Keyboard driver available (hal_keyboard_init called)
 *
 * Postconditions:
 *   - Shell context properly initialized after shell_init
 *   - Commands registered and dispatchable
 *
 * Invariants:
 *   - Shell state always valid (SHELL_STATE_*)
 *   - Input buffer never overflows (bounded by SHELL_INPUT_MAX)
 */

/**
 * =============================================================================
 * M.9 DEPENDENCIES - Required Components [DEPENDENCIES]
 * =============================================================================
 *
 * Depends:
 *   - kernel.h           - Core types, boot path API
 *   - trit/matter.h      - Ternary types (trit_t)
 *
 * Used by:
 *   - shell/terminal/src/shell.c        - Implementation
 *   - shell/terminal/src/builtins/      - Built-in commands
 *   - kernel/src/kernel.c               - Shell integration
 */

/**
 * =============================================================================
 * M.10 ROADMAP - Version History [ROADMAP]
 * =============================================================================
 *
 * History:
 *   a-01.00 (2026-01-31) - Initial shell API design
 *
 * Planned:
 *   - Command history and recall
 *   - Tab completion
 *   - Scripting support
 *   - OmniCode shell integration
 *
 * =============================================================================
 */

// =============================================================================
// END METADATA
// =============================================================================

#ifndef MOS_SHELL_TERMINAL_H
#define MOS_SHELL_TERMINAL_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// ---------------------------------------------------------------------------
// S.1 System Headers
// ---------------------------------------------------------------------------

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ---------------------------------------------------------------------------
// S.2 Project Headers
// ---------------------------------------------------------------------------

#include "kernel.h"

// ---------------------------------------------------------------------------
// S.3 Configuration Constants
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_config Shell Configuration
 * @{
 */

/** Maximum command line input length */
#define SHELL_INPUT_MAX         256

/** Maximum number of arguments per command */
#define SHELL_ARGS_MAX          16

/** Maximum registered commands */
#define SHELL_COMMANDS_MAX      32

/** Maximum command name length */
#define SHELL_CMD_NAME_MAX      16

/** History buffer size (number of commands) */
#define SHELL_HISTORY_MAX       16

/** @} */

// ---------------------------------------------------------------------------
// S.4 Feature Flags
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_features Shell Feature Flags
 * @{
 */

/** Enable command history */
#define SHELL_FEAT_HISTORY      0x01

/** Enable tab completion */
#define SHELL_FEAT_COMPLETE     0x02

/** Enable colored output */
#define SHELL_FEAT_COLOR        0x04

/** Enable scripting mode */
#define SHELL_FEAT_SCRIPT       0x08

/** @} */

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// ---------------------------------------------------------------------------
// B.1 Type Definitions
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_types Shell Types
 * @{
 */

/**
 * @brief Shell state enumeration
 *
 * Tracks the current operational state of the shell.
 */
typedef enum MosShellState {
    SHELL_STATE_UNINIT    = -1,   /**< Not initialized */
    SHELL_STATE_READY     =  0,   /**< Ready for input */
    SHELL_STATE_RUNNING   =  1,   /**< Command executing */
    SHELL_STATE_EXIT      =  2,   /**< Shell exiting */
    SHELL_STATE_ERROR     = -2    /**< Error state */
} MosShellState;

/**
 * @brief Shell result codes
 *
 * Return codes for shell operations.
 */
typedef enum MosShellResult {
    SHELL_OK              =  0,   /**< Success */
    SHELL_ERROR           = -1,   /**< General error */
    SHELL_NOT_FOUND       = -2,   /**< Command not found */
    SHELL_INVALID_ARG     = -3,   /**< Invalid argument */
    SHELL_BUFFER_FULL     = -4,   /**< Buffer overflow prevented */
    SHELL_EXIT_REQUEST    =  1    /**< Exit requested (not error) */
} MosShellResult;

/**
 * @brief Command handler function pointer
 *
 * @param argc  Argument count
 * @param argv  Argument vector (null-terminated strings)
 * @param ctx   Shell context pointer
 * @return      Shell result code
 */
struct MosShellContext;  /* Forward declaration */
typedef MosShellResult (*MosShellCmdFn)(int argc, char** argv,
                                        struct MosShellContext* ctx);

/**
 * @brief Command descriptor
 *
 * Describes a registered shell command.
 */
typedef struct MosShellCommand {
    char            name[SHELL_CMD_NAME_MAX];   /**< Command name */
    MosShellCmdFn   handler;                    /**< Handler function */
    const char*     help_brief;                 /**< One-line help */
    const char*     help_usage;                 /**< Usage string */
} MosShellCommand;

/**
 * @brief Shell context structure
 *
 * Maintains all state for a shell session.
 */
typedef struct MosShellContext {
    /* State */
    MosShellState   state;                      /**< Current state */
    uint32_t        features;                   /**< Enabled features */

    /* Input buffer */
    char            input[SHELL_INPUT_MAX];     /**< Current input line */
    size_t          input_len;                  /**< Current input length */
    size_t          cursor_pos;                 /**< Cursor position */

    /* Argument parsing */
    char*           argv[SHELL_ARGS_MAX];       /**< Parsed arguments */
    int             argc;                       /**< Argument count */

    /* Command registry */
    MosShellCommand commands[SHELL_COMMANDS_MAX];  /**< Registered commands */
    size_t          cmd_count;                  /**< Number of commands */

    /* Statistics */
    uint32_t        cmd_executed;               /**< Commands executed */
    uint32_t        cmd_failed;                 /**< Commands failed */

    /* Hebrew state (CPI-SI integration) */
    trit_t          health;                     /**< Current health trit */
} MosShellContext;

/** @} */

// ---------------------------------------------------------------------------
// B.2 Shell Lifecycle API
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_lifecycle Shell Lifecycle
 * @{
 */

/**
 * @brief Initialize shell context
 *
 * Initializes a shell context with default settings and registers
 * all built-in commands.
 *
 * @param ctx       Shell context to initialize
 * @param features  Feature flags (SHELL_FEAT_*)
 * @return          SHELL_OK on success, error code on failure
 *
 * @pre  ctx is valid pointer
 * @post ctx->state == SHELL_STATE_READY
 * @post Built-in commands registered
 */
MosShellResult shell_init(MosShellContext* ctx, uint32_t features);

/**
 * @brief Run shell main loop
 *
 * Enters the interactive shell loop. Returns when exit requested
 * or error occurs.
 *
 * @param ctx  Initialized shell context
 * @return     SHELL_OK on clean exit, error code on failure
 *
 * @pre  ctx->state == SHELL_STATE_READY
 * @post ctx->state == SHELL_STATE_EXIT or SHELL_STATE_ERROR
 */
MosShellResult shell_run(MosShellContext* ctx);

/**
 * @brief Execute a single command line
 *
 * Parses and executes a command without entering interactive mode.
 *
 * @param ctx   Shell context
 * @param line  Command line to execute
 * @return      Command result code
 */
MosShellResult shell_exec(MosShellContext* ctx, const char* line);

/**
 * @brief Cleanup shell context
 *
 * Releases any resources held by the shell context.
 *
 * @param ctx  Shell context to cleanup
 */
void shell_cleanup(MosShellContext* ctx);

/** @} */

// ---------------------------------------------------------------------------
// B.3 Command Registry API
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_registry Command Registry
 * @{
 */

/**
 * @brief Register a command
 *
 * Adds a command to the shell's command registry.
 *
 * @param ctx         Shell context
 * @param name        Command name (max SHELL_CMD_NAME_MAX-1 chars)
 * @param handler     Command handler function
 * @param help_brief  One-line help description
 * @param help_usage  Usage string (e.g., "cmd <arg1> [arg2]")
 * @return            SHELL_OK on success, error code on failure
 *
 * @pre  ctx->cmd_count < SHELL_COMMANDS_MAX
 * @post Command is dispatchable by name
 */
MosShellResult shell_register(MosShellContext* ctx,
                              const char* name,
                              MosShellCmdFn handler,
                              const char* help_brief,
                              const char* help_usage);

/**
 * @brief Find a registered command
 *
 * Looks up a command by name.
 *
 * @param ctx   Shell context
 * @param name  Command name to find
 * @return      Pointer to command, or NULL if not found
 */
const MosShellCommand* shell_find_command(MosShellContext* ctx,
                                          const char* name);

/** @} */

// ---------------------------------------------------------------------------
// B.4 I/O API
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_io Shell I/O
 * @{
 */

/**
 * @brief Print formatted output to shell
 *
 * Printf-like output to the shell's display.
 *
 * @param ctx  Shell context
 * @param fmt  Format string
 * @param ...  Format arguments
 * @return     Number of characters printed, or negative on error
 */
int shell_printf(MosShellContext* ctx, const char* fmt, ...);

/**
 * @brief Print a string to shell
 *
 * @param ctx  Shell context
 * @param str  String to print
 */
void shell_puts(MosShellContext* ctx, const char* str);

/**
 * @brief Print newline
 *
 * @param ctx  Shell context
 */
void shell_newline(MosShellContext* ctx);

/**
 * @brief Clear shell display
 *
 * @param ctx  Shell context
 */
void shell_clear(MosShellContext* ctx);

/** @} */

// ---------------------------------------------------------------------------
// B.5 Built-in Command Declarations
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_builtins Built-in Commands
 * @{
 */

/** DAR witness command: dar [status|detect|assess|restore] */
MosShellResult cmd_dar(int argc, char** argv, MosShellContext* ctx);

/** Boot info command: boot [path|features|info] */
MosShellResult cmd_boot(int argc, char** argv, MosShellContext* ctx);

/** Memory command: mem [map|stats|dump] */
MosShellResult cmd_mem(int argc, char** argv, MosShellContext* ctx);

/** Help command: help [command] */
MosShellResult cmd_help(int argc, char** argv, MosShellContext* ctx);

/** Clear screen command */
MosShellResult cmd_clear(int argc, char** argv, MosShellContext* ctx);

/** Echo command */
MosShellResult cmd_echo(int argc, char** argv, MosShellContext* ctx);

/** Reboot command */
MosShellResult cmd_reboot(int argc, char** argv, MosShellContext* ctx);

/** Exit shell command */
MosShellResult cmd_exit(int argc, char** argv, MosShellContext* ctx);

/** @} */

// ---------------------------------------------------------------------------
// B.6 Utility Functions
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_util Shell Utilities
 * @{
 */

/**
 * @brief Get shell state as string
 *
 * @param state  Shell state value
 * @return       Human-readable state name
 */
const char* shell_state_str(MosShellState state);

/**
 * @brief Get shell result as string
 *
 * @param result  Shell result code
 * @return        Human-readable result description
 */
const char* shell_result_str(MosShellResult result);

/** @} */

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// ---------------------------------------------------------------------------
// X.1 Compile-Time Validation
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_validation Compile-Time Validation
 * @{
 */

_Static_assert(SHELL_INPUT_MAX >= 64,
               "SHELL_INPUT_MAX must be at least 64");
_Static_assert(SHELL_ARGS_MAX >= 4,
               "SHELL_ARGS_MAX must be at least 4");
_Static_assert(SHELL_COMMANDS_MAX >= 8,
               "SHELL_COMMANDS_MAX must be at least 8");

/** @} */

// ---------------------------------------------------------------------------
// X.2 Include Guard Close
// ---------------------------------------------------------------------------

#endif /* MOS_SHELL_TERMINAL_H */

// =============================================================================
// END CLOSING
// =============================================================================

/**
 * =============================================================================
 * FILE FOOTER
 * =============================================================================
 *
 * Witness:      DAR Witness Entry
 * Health:       YASHAR (centered, ready)
 * Validated:    Structural compliance verified
 *
 * Related:
 *   - shell/terminal/src/shell.c     - Implementation
 *   - shell/terminal/include/command.h - Command details
 *   - kernel/include/kernel.h        - Kernel API
 *
 * =============================================================================
 * Generated by Nova Dawn for MillenniumOS
 * CreativeWorkzStudio LLC - Kingdom Technology
 * =============================================================================
 */
