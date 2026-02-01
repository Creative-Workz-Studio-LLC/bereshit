/// @file shell.c
/// @omni code --c --source
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
/// @omni:req  key        = MOS-SHELL-TERMINAL-IMPL
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = a-01.00
///
/// ---------------------------------------------------------------------------
/// P.2 FAMILY - Classification [FAMILY]
/// ---------------------------------------------------------------------------
///
/// @omni:inh  type       = source
/// @omni:inh  subtype    = implementation
/// @omni:inh  role       = shell_core
/// @omni:inh  structure  = 4-block
///
/// ---------------------------------------------------------------------------
/// P.3 INSTANCE - File Details [INSTANCE]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  component  = shell
/// @omni:ins  layer      = terminal
/// @omni:ins  implements = [shell.h]
/// @omni:ins  provides   = [shell_init, shell_run, shell_exec, shell_io]
///
/// ---------------------------------------------------------------------------
/// P.4 ARCHITECTURE - Scaling Context [ARCH]
/// ---------------------------------------------------------------------------
///
/// @omni:ins  layers     = [0:kernel, 1:hal, 2:services, 3:shell, 4:apps]
/// @omni:ins  position   = 3 (shell layer)
/// @omni:ins  depends    = [kernel, graphics]
///
/// ---------------------------------------------------------------------------
/// P.5 SUMMARY - Human Identity [SUMMARY]
/// ---------------------------------------------------------------------------
///
/// @omni:req  title      = MillenniumOS Terminal Shell Implementation
/// @omni:req  brief      = Core shell functionality - init, run, I/O
///
/// =============================================================================
/// END PRAGMA [END]
/// =============================================================================

/**
 * =============================================================================
 * METADATA [METADATA]
 * =============================================================================
 *
 * Key:          MOS-SHELL-TERMINAL-IMPL
 * Title:        MillenniumOS Terminal Shell Implementation
 * Type:         Source
 * Component:    Shell / Terminal
 * Role:         Core shell loop and I/O operations
 *
 * Authors:      Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)
 * Organization: CreativeWorkzStudio LLC
 *
 * Status:       Active
 * Version:      a-01.00
 * Created:      2026-01-31
 * Updated:      2026-01-31
 *
 * Grounding:    Psalm 119:105 - "Thy word is a lamp unto my feet"
 *               The shell illuminates system state, command by command.
 *
 * =============================================================================
 */

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// ---------------------------------------------------------------------------
// S.1 Project Headers
// ---------------------------------------------------------------------------

#include "shell.h"
#include "command.h"

// ---------------------------------------------------------------------------
// S.2 Freestanding String Operations
// ---------------------------------------------------------------------------

/**
 * @brief Freestanding strlen
 */
static size_t shell_strlen(const char* s) {
    size_t len = 0;
    while (s && *s++) len++;
    return len;
}

/**
 * @brief Freestanding strncpy
 */
static char* shell_strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

/**
 * @brief Freestanding strcmp
 */
static int shell_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/**
 * @brief Freestanding memset
 */
static void* shell_memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

// ---------------------------------------------------------------------------
// S.3 Output State
// ---------------------------------------------------------------------------

/** Current cursor X position */
static int g_cursor_x = 0;

/** Current cursor Y position */
static int g_cursor_y = 0;

/** Characters per line */
#define SHELL_COLS  80

/** Lines per screen */
#define SHELL_ROWS  25

// ---------------------------------------------------------------------------
// S.4 Shell Prompt
// ---------------------------------------------------------------------------

/** Default shell prompt */
static const char* SHELL_PROMPT = "mos> ";

/** Prompt length */
#define SHELL_PROMPT_LEN  5

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// ---------------------------------------------------------------------------
// B.1 String Helpers (State-to-String)
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_strings String Conversions
 * @{
 */

const char* shell_state_str(MosShellState state) {
    switch (state) {
        case SHELL_STATE_UNINIT:  return "UNINIT";
        case SHELL_STATE_READY:   return "READY";
        case SHELL_STATE_RUNNING: return "RUNNING";
        case SHELL_STATE_EXIT:    return "EXIT";
        case SHELL_STATE_ERROR:   return "ERROR";
        default:                  return "UNKNOWN";
    }
}

const char* shell_result_str(MosShellResult result) {
    switch (result) {
        case SHELL_OK:           return "OK";
        case SHELL_ERROR:        return "Error";
        case SHELL_NOT_FOUND:    return "Command not found";
        case SHELL_INVALID_ARG:  return "Invalid argument";
        case SHELL_BUFFER_FULL:  return "Buffer full";
        case SHELL_EXIT_REQUEST: return "Exit requested";
        default:                 return "Unknown result";
    }
}

/** @} */

// ---------------------------------------------------------------------------
// B.2 I/O Operations
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_io_impl I/O Implementation
 * @{
 */

/**
 * @brief Write a character to the shell output
 *
 * Uses kernel's graphics subsystem for output.
 */
static void shell_putchar(MosShellContext* ctx, char c) {
    (void)ctx;  /* Context may be used for output routing later */

    if (c == '\n') {
        g_cursor_x = 0;
        g_cursor_y++;
        if (g_cursor_y >= SHELL_ROWS) {
            /* TODO: Scroll screen */
            g_cursor_y = SHELL_ROWS - 1;
        }
    } else if (c == '\r') {
        g_cursor_x = 0;
    } else if (c == '\t') {
        g_cursor_x = (g_cursor_x + 8) & ~7;
        if (g_cursor_x >= SHELL_COLS) {
            g_cursor_x = 0;
            g_cursor_y++;
        }
    } else {
        /* TODO: Use graphics API to draw character */
        /* For now, use serial output if available */
        extern void serial_putchar(char c);
        serial_putchar(c);

        g_cursor_x++;
        if (g_cursor_x >= SHELL_COLS) {
            g_cursor_x = 0;
            g_cursor_y++;
        }
    }
}

void shell_puts(MosShellContext* ctx, const char* str) {
    if (!str) return;
    while (*str) {
        shell_putchar(ctx, *str++);
    }
}

void shell_newline(MosShellContext* ctx) {
    shell_putchar(ctx, '\n');
}

void shell_clear(MosShellContext* ctx) {
    /* TODO: Use graphics API to clear screen */
    g_cursor_x = 0;
    g_cursor_y = 0;

    /* Send ANSI clear sequence to serial */
    shell_puts(ctx, "\033[2J\033[H");
}

/**
 * @brief Print unsigned integer
 */
static void shell_print_uint(MosShellContext* ctx, uint32_t value) {
    char buf[12];
    int i = 11;
    buf[i] = '\0';

    if (value == 0) {
        shell_putchar(ctx, '0');
        return;
    }

    while (value > 0 && i > 0) {
        buf[--i] = '0' + (value % 10);
        value /= 10;
    }

    shell_puts(ctx, &buf[i]);
}

/**
 * @brief Print signed integer
 */
__attribute__((unused))
static void shell_print_int(MosShellContext* ctx, int32_t value) {
    if (value < 0) {
        shell_putchar(ctx, '-');
        value = -value;
    }
    shell_print_uint(ctx, (uint32_t)value);
}

/**
 * @brief Print hex value
 */
__attribute__((unused))
static void shell_print_hex(MosShellContext* ctx, uint32_t value, int width) {
    static const char hex[] = "0123456789abcdef";
    char buf[9];
    int i;

    for (i = 7; i >= 0; i--) {
        buf[i] = hex[value & 0xF];
        value >>= 4;
    }
    buf[8] = '\0';

    /* Skip leading zeros unless width specified */
    int start = 8 - width;
    if (start < 0) start = 0;

    shell_puts(ctx, &buf[start]);
}

int shell_printf(MosShellContext* ctx, const char* fmt, ...) {
    /* Simple printf implementation for freestanding environment */
    /* Supports: %s, %d, %u, %x, %p, %c, %% */

    if (!fmt) return -1;

    int count = 0;
    const char* p = fmt;

    /* We need varargs - use a simple approach for now */
    /* TODO: Implement proper varargs for freestanding */

    while (*p) {
        if (*p == '%' && *(p+1)) {
            p++;
            switch (*p) {
                case '%':
                    shell_putchar(ctx, '%');
                    count++;
                    break;
                case 's':
                    /* Would need vararg here - skip for now */
                    shell_puts(ctx, "(str)");
                    count += 5;
                    break;
                case 'd':
                case 'i':
                    shell_puts(ctx, "(int)");
                    count += 5;
                    break;
                case 'u':
                    shell_puts(ctx, "(uint)");
                    count += 6;
                    break;
                case 'x':
                case 'X':
                    shell_puts(ctx, "(hex)");
                    count += 5;
                    break;
                case 'p':
                    shell_puts(ctx, "(ptr)");
                    count += 5;
                    break;
                case 'c':
                    shell_putchar(ctx, '?');
                    count++;
                    break;
                default:
                    shell_putchar(ctx, '%');
                    shell_putchar(ctx, *p);
                    count += 2;
                    break;
            }
        } else {
            shell_putchar(ctx, *p);
            count++;
        }
        p++;
    }

    return count;
}

/** @} */

// ---------------------------------------------------------------------------
// B.3 Shell Lifecycle
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_lifecycle_impl Lifecycle Implementation
 * @{
 */

MosShellResult shell_init(MosShellContext* ctx, uint32_t features) {
    if (!ctx) {
        return SHELL_INVALID_ARG;
    }

    /* Clear context */
    shell_memset(ctx, 0, sizeof(MosShellContext));

    /* Set initial state */
    ctx->state = SHELL_STATE_READY;
    ctx->features = features;
    ctx->health = TRIT_ZERO;  /* YASHAR - centered */

    /* Register built-in commands */
    MosShellResult res = cmd_register_builtins(ctx);
    if (res != SHELL_OK) {
        ctx->state = SHELL_STATE_ERROR;
        return res;
    }

    return SHELL_OK;
}

void shell_cleanup(MosShellContext* ctx) {
    if (!ctx) return;

    ctx->state = SHELL_STATE_UNINIT;
    shell_memset(ctx, 0, sizeof(MosShellContext));
}

/**
 * @brief Display shell prompt
 */
static void shell_show_prompt(MosShellContext* ctx) {
    shell_puts(ctx, SHELL_PROMPT);
}

/**
 * @brief Read a line of input
 *
 * Blocking read until newline or buffer full.
 * TODO: Needs HAL keyboard driver
 */
static MosShellResult shell_read_line(MosShellContext* ctx) {
    /* Clear input buffer */
    ctx->input_len = 0;
    ctx->cursor_pos = 0;

    /* TODO: Implement keyboard input via HAL */
    /* For now, this is a stub that returns immediately */

    /* Placeholder: would read from keyboard until Enter */
    /*
    while (ctx->input_len < SHELL_INPUT_MAX - 1) {
        char c = hal_keyboard_getchar();
        if (c == '\n' || c == '\r') {
            break;
        }
        if (c == '\b' && ctx->input_len > 0) {
            ctx->input_len--;
            ctx->cursor_pos--;
            continue;
        }
        ctx->input[ctx->input_len++] = c;
        ctx->cursor_pos++;
        shell_putchar(ctx, c);  // Echo
    }
    */

    ctx->input[ctx->input_len] = '\0';
    return SHELL_OK;
}

MosShellResult shell_run(MosShellContext* ctx) {
    if (!ctx || ctx->state != SHELL_STATE_READY) {
        return SHELL_INVALID_ARG;
    }

    /* Show welcome banner */
    shell_puts(ctx, "\n");
    shell_puts(ctx, "MillenniumOS Terminal Shell v0.1\n");
    shell_puts(ctx, "Type 'help' for available commands.\n");
    shell_puts(ctx, "\n");

    /* Main loop */
    while (ctx->state == SHELL_STATE_READY) {
        /* Show prompt */
        shell_show_prompt(ctx);

        /* Read input line */
        MosShellResult res = shell_read_line(ctx);
        if (res != SHELL_OK) {
            ctx->state = SHELL_STATE_ERROR;
            break;
        }

        /* Skip empty lines */
        if (ctx->input_len == 0) {
            shell_newline(ctx);
            continue;
        }

        /* Echo newline */
        shell_newline(ctx);

        /* Execute command */
        res = shell_exec(ctx, ctx->input);
        if (res == SHELL_EXIT_REQUEST) {
            ctx->state = SHELL_STATE_EXIT;
            break;
        }

        /* Track statistics */
        ctx->cmd_executed++;
        if (res != SHELL_OK && res != SHELL_EXIT_REQUEST) {
            ctx->cmd_failed++;
        }
    }

    return (ctx->state == SHELL_STATE_EXIT) ? SHELL_OK : SHELL_ERROR;
}

MosShellResult shell_exec(MosShellContext* ctx, const char* line) {
    if (!ctx || !line) {
        return SHELL_INVALID_ARG;
    }

    /* Copy line to input buffer for parsing */
    size_t len = shell_strlen(line);
    if (len >= SHELL_INPUT_MAX) {
        return SHELL_BUFFER_FULL;
    }
    shell_strncpy(ctx->input, line, SHELL_INPUT_MAX - 1);
    ctx->input[len] = '\0';

    /* Parse into argc/argv */
    ctx->argc = cmd_parse(ctx->input, ctx->argv, SHELL_ARGS_MAX);
    if (ctx->argc <= 0) {
        return SHELL_OK;  /* Empty or comment line */
    }

    /* Dispatch command */
    return cmd_dispatch(ctx, ctx->argc, ctx->argv);
}

/** @} */

// ---------------------------------------------------------------------------
// B.4 Command Registry
// ---------------------------------------------------------------------------

/**
 * @defgroup shell_registry_impl Registry Implementation
 * @{
 */

MosShellResult shell_register(MosShellContext* ctx,
                              const char* name,
                              MosShellCmdFn handler,
                              const char* help_brief,
                              const char* help_usage) {
    if (!ctx || !name || !handler) {
        return SHELL_INVALID_ARG;
    }

    if (ctx->cmd_count >= SHELL_COMMANDS_MAX) {
        return SHELL_BUFFER_FULL;
    }

    MosShellCommand* cmd = &ctx->commands[ctx->cmd_count];

    shell_strncpy(cmd->name, name, SHELL_CMD_NAME_MAX - 1);
    cmd->name[SHELL_CMD_NAME_MAX - 1] = '\0';
    cmd->handler = handler;
    cmd->help_brief = help_brief;
    cmd->help_usage = help_usage;

    ctx->cmd_count++;

    return SHELL_OK;
}

const MosShellCommand* shell_find_command(MosShellContext* ctx,
                                          const char* name) {
    if (!ctx || !name) return NULL;

    for (size_t i = 0; i < ctx->cmd_count; i++) {
        if (shell_strcmp(ctx->commands[i].name, name) == 0) {
            return &ctx->commands[i];
        }
    }

    return NULL;
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
 * Witness:      DAR Witness Entry
 * Health:       YASHAR (centered, ready)
 *
 * Related:
 *   - shell/terminal/include/shell.h    - Public API
 *   - shell/terminal/src/parser.c       - Command parsing
 *   - shell/terminal/src/dispatch.c     - Command dispatch
 *   - shell/terminal/src/builtins/      - Built-in commands
 *
 * =============================================================================
 * Generated by Nova Dawn for MillenniumOS
 * CreativeWorkzStudio LLC - Kingdom Technology
 * =============================================================================
 */

// =============================================================================
// END CLOSING
// =============================================================================
