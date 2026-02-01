/// @file command.h
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
/// @omni:req  key        = MOS-SHELL-COMMAND-API
/// @omni:req  from       = bereshit/word/seed/code/c/header.h
/// @omni:req  at         = a-01.00
///
/// ---------------------------------------------------------------------------
/// P.2 FAMILY - Classification [FAMILY]
/// ---------------------------------------------------------------------------
///
/// @omni:inh  type       = header
/// @omni:inh  subtype    = internal
/// @omni:inh  role       = command_parsing
/// @omni:inh  structure  = 4-block
///
/// ---------------------------------------------------------------------------
/// P.3 INSTANCE - File Details [INSTANCE]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  component  = shell
/// @omni:ins  layer      = terminal/internal
/// @omni:ins  provides   = [parser_api, tokenizer, argument_handling]
///
/// ---------------------------------------------------------------------------
/// P.4 ARCHITECTURE - Scaling Context [ARCH]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  position   = internal (used by shell.c only)
/// @omni:ins  depends    = [shell.h]
///
/// ---------------------------------------------------------------------------
/// P.5 SUMMARY - Human Identity [SUMMARY]
/// ---------------------------------------------------------------------------
///
/// @omni:req  title      = Shell Command Parsing API
/// @omni:req  brief      = Internal command line parsing and tokenization
///
/// =============================================================================
/// END PRAGMA [END]
/// =============================================================================

/**
 * =============================================================================
 * METADATA [METADATA]
 * =============================================================================
 *
 * Key:          MOS-SHELL-COMMAND-API
 * Title:        Shell Command Parsing API
 * Type:         Internal Header
 * Component:    Shell / Terminal / Parser
 * Role:         Command line tokenization and argument parsing
 *
 * Authors:      Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)
 * Organization: CreativeWorkzStudio LLC
 *
 * Status:       Active
 * Version:      a-01.00
 * Created:      2026-01-31
 * Updated:      2026-01-31
 *
 * Grounding:    Proverbs 16:23 - "The heart of the wise teacheth his mouth,
 *               and addeth learning to his lips."
 *
 *               Careful parsing extracts meaning from input,
 *               teaching the shell to understand.
 *
 * =============================================================================
 */

#ifndef MOS_SHELL_COMMAND_H
#define MOS_SHELL_COMMAND_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include "shell.h"

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// ---------------------------------------------------------------------------
// B.1 Parser Configuration
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_config Parser Configuration
 * @{
 */

/** Token separator characters */
#define CMD_SEPARATORS      " \t"

/** Quote characters for grouping */
#define CMD_QUOTES          "\"'"

/** Escape character */
#define CMD_ESCAPE          '\\'

/** Comment character (rest of line ignored) */
#define CMD_COMMENT         '#'

/** @} */

// ---------------------------------------------------------------------------
// B.2 Parser API
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_parser Parser Functions
 * @{
 */

/**
 * @brief Parse command line into arguments
 *
 * Tokenizes a command line string into argc/argv format.
 * Modifies the input buffer in-place (adds null terminators).
 *
 * Handles:
 *   - Whitespace separation
 *   - Quoted strings (preserves spaces)
 *   - Escape sequences (\\, \", \')
 *   - Comments (# to end of line)
 *
 * @param line  Command line buffer (modified in-place)
 * @param argv  Output argument vector
 * @param max   Maximum arguments (size of argv)
 * @return      Argument count (argc), or negative on error
 *
 * @pre  line is null-terminated
 * @pre  argv has space for max pointers
 * @post argv[0..return-1] point into line
 * @post argv[return] == NULL
 *
 * Example:
 *   Input:  "echo \"hello world\" foo"
 *   Output: argv[0]="echo", argv[1]="hello world", argv[2]="foo"
 *   Return: 3
 */
int cmd_parse(char* line, char** argv, int max);

/**
 * @brief Check if character is a separator
 *
 * @param c  Character to check
 * @return   true if separator, false otherwise
 */
bool cmd_is_separator(char c);

/**
 * @brief Check if character is a quote
 *
 * @param c  Character to check
 * @return   true if quote character, false otherwise
 */
bool cmd_is_quote(char c);

/**
 * @brief Skip leading whitespace
 *
 * @param str  String pointer (updated to skip whitespace)
 * @return     Pointer to first non-whitespace, or end of string
 */
char* cmd_skip_whitespace(char* str);

/**
 * @brief Find end of token
 *
 * Finds the end of the current token, respecting quotes.
 *
 * @param str  Start of token
 * @return     Pointer past end of token
 */
char* cmd_find_token_end(char* str);

/** @} */

// ---------------------------------------------------------------------------
// B.3 Command Dispatch
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_dispatch Command Dispatch
 * @{
 */

/**
 * @brief Dispatch parsed command
 *
 * Looks up and executes the command specified by argv[0].
 *
 * @param ctx   Shell context
 * @param argc  Argument count
 * @param argv  Argument vector
 * @return      Command result, or SHELL_NOT_FOUND
 */
MosShellResult cmd_dispatch(MosShellContext* ctx, int argc, char** argv);

/**
 * @brief Register all built-in commands
 *
 * Registers the standard built-in commands (dar, boot, mem, help, etc.)
 *
 * @param ctx  Shell context
 * @return     SHELL_OK on success, error on failure
 */
MosShellResult cmd_register_builtins(MosShellContext* ctx);

/** @} */

// ---------------------------------------------------------------------------
// B.4 Argument Helpers
// ---------------------------------------------------------------------------

/**
 * @defgroup cmd_args Argument Helpers
 * @{
 */

/**
 * @brief Check if argument matches option
 *
 * Compares argument against short and long option forms.
 *
 * @param arg        Argument to check
 * @param short_opt  Short option (e.g., "-h"), or NULL
 * @param long_opt   Long option (e.g., "--help"), or NULL
 * @return           true if matches either form
 */
bool cmd_is_option(const char* arg, const char* short_opt, const char* long_opt);

/**
 * @brief Get option value
 *
 * Finds the value for an option (handles -o value and -o=value forms).
 *
 * @param argc  Argument count
 * @param argv  Argument vector
 * @param idx   Current index (updated if value consumed)
 * @param opt   Option to look for (e.g., "-f")
 * @return      Option value, or NULL if not found
 */
const char* cmd_get_option_value(int argc, char** argv, int* idx, const char* opt);

/**
 * @brief Parse integer argument
 *
 * @param str     String to parse
 * @param result  Output value
 * @return        true on success, false on parse error
 */
bool cmd_parse_int(const char* str, int* result);

/**
 * @brief Parse hex argument
 *
 * @param str     String to parse (with or without 0x prefix)
 * @param result  Output value
 * @return        true on success, false on parse error
 */
bool cmd_parse_hex(const char* str, uint32_t* result);

/** @} */

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

#endif /* MOS_SHELL_COMMAND_H */

// =============================================================================
// END CLOSING
// =============================================================================

/**
 * =============================================================================
 * FILE FOOTER
 * =============================================================================
 *
 * Related:
 *   - shell/terminal/include/shell.h    - Public shell API
 *   - shell/terminal/src/parser.c       - Parser implementation
 *   - shell/terminal/src/dispatch.c     - Dispatch implementation
 *
 * =============================================================================
 * Generated by Nova Dawn for MillenniumOS
 * CreativeWorkzStudio LLC - Kingdom Technology
 * =============================================================================
 */
