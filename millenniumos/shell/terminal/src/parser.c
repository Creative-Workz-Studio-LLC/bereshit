/// @file parser.c
/// @omni code --c --source
///
/// =============================================================================
/// OMNICODE PRAGMA [PRAGMA]
/// =============================================================================
///
/// @omni:req  key        = MOS-SHELL-PARSER-IMPL
/// @omni:req  from       = bereshit/word/seed/code/c/source.c
/// @omni:req  at         = a-01.00
/// @omni:inh  type       = source
/// @omni:inh  role       = command_parsing
/// @omni:ins  component  = shell/terminal
/// @omni:ins  implements = [command.h]
/// @omni:req  title      = Shell Command Parser
/// @omni:req  brief      = Tokenize command lines into argc/argv
///
/// =============================================================================
/// END PRAGMA [END]
/// =============================================================================

/**
 * =============================================================================
 * METADATA [METADATA]
 * =============================================================================
 *
 * Key:          MOS-SHELL-PARSER-IMPL
 * Title:        Shell Command Parser
 * Type:         Source
 * Component:    Shell / Terminal / Parser
 *
 * Authors:      Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)
 * Organization: CreativeWorkzStudio LLC
 *
 * Status:       Active
 * Version:      a-01.00
 * Created:      2026-01-31
 *
 * Grounding:    Ecclesiastes 12:11 - "The words of the wise are as goads"
 *               Careful parsing extracts meaning from user input.
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
// B.1 Character Classification
// ---------------------------------------------------------------------------

/**
 * @defgroup parser_classify Character Classification
 * @{
 */

bool cmd_is_separator(char c) {
    return (c == ' ' || c == '\t');
}

bool cmd_is_quote(char c) {
    return (c == '"' || c == '\'');
}

/**
 * @brief Check if character is escape
 */
static bool is_escape(char c) {
    return (c == CMD_ESCAPE);
}

/**
 * @brief Check if character is comment start
 */
static bool is_comment(char c) {
    return (c == CMD_COMMENT);
}

/** @} */

// ---------------------------------------------------------------------------
// B.2 Token Navigation
// ---------------------------------------------------------------------------

/**
 * @defgroup parser_nav Token Navigation
 * @{
 */

char* cmd_skip_whitespace(char* str) {
    if (!str) return NULL;
    while (*str && cmd_is_separator(*str)) {
        str++;
    }
    return str;
}

char* cmd_find_token_end(char* str) {
    if (!str || !*str) return str;

    char quote = 0;

    /* Check if starting with quote */
    if (cmd_is_quote(*str)) {
        quote = *str;
        str++;

        /* Find matching quote */
        while (*str) {
            if (is_escape(*str) && *(str + 1)) {
                str += 2;  /* Skip escape sequence */
                continue;
            }
            if (*str == quote) {
                str++;  /* Include closing quote */
                break;
            }
            str++;
        }
    } else {
        /* Find separator or end */
        while (*str && !cmd_is_separator(*str)) {
            if (is_escape(*str) && *(str + 1)) {
                str += 2;  /* Skip escape sequence */
                continue;
            }
            str++;
        }
    }

    return str;
}

/** @} */

// ---------------------------------------------------------------------------
// B.3 Main Parser
// ---------------------------------------------------------------------------

/**
 * @defgroup parser_main Main Parser
 * @{
 */

/**
 * @brief Remove quotes from token and handle escapes
 *
 * Modifies string in-place.
 *
 * @param token  Token to process
 * @return       Processed token (same pointer)
 */
static char* process_token(char* token) {
    if (!token || !*token) return token;

    char* src = token;
    char* dst = token;
    char quote = 0;

    /* Check for leading quote */
    if (cmd_is_quote(*src)) {
        quote = *src;
        src++;
    }

    while (*src) {
        /* Stop at closing quote */
        if (quote && *src == quote) {
            break;
        }

        /* Handle escape */
        if (is_escape(*src) && *(src + 1)) {
            src++;
            switch (*src) {
                case 'n':  *dst++ = '\n'; break;
                case 't':  *dst++ = '\t'; break;
                case 'r':  *dst++ = '\r'; break;
                case '\\': *dst++ = '\\'; break;
                case '"':  *dst++ = '"';  break;
                case '\'': *dst++ = '\''; break;
                default:   *dst++ = *src; break;
            }
            src++;
            continue;
        }

        *dst++ = *src++;
    }

    *dst = '\0';
    return token;
}

int cmd_parse(char* line, char** argv, int max) {
    if (!line || !argv || max <= 0) {
        return -1;
    }

    int argc = 0;
    char* p = line;

    while (*p && argc < max - 1) {  /* Leave room for NULL terminator */
        /* Skip leading whitespace */
        p = cmd_skip_whitespace(p);

        /* Check for end or comment */
        if (!*p || is_comment(*p)) {
            break;
        }

        /* Mark start of token */
        argv[argc] = p;

        /* Find end of token */
        char* end = cmd_find_token_end(p);

        /* Null-terminate this token */
        if (*end) {
            *end = '\0';
            p = end + 1;
        } else {
            p = end;
        }

        /* Process token (remove quotes, handle escapes) */
        process_token(argv[argc]);

        /* Only count non-empty tokens */
        if (*argv[argc]) {
            argc++;
        }
    }

    /* NULL-terminate argv */
    argv[argc] = NULL;

    return argc;
}

/** @} */

// ---------------------------------------------------------------------------
// B.4 Argument Helpers
// ---------------------------------------------------------------------------

/**
 * @defgroup parser_helpers Argument Helpers
 * @{
 */

/**
 * @brief Freestanding strcmp
 */
static int parser_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/**
 * @brief Freestanding strncmp
 */
static int parser_strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

bool cmd_is_option(const char* arg, const char* short_opt, const char* long_opt) {
    if (!arg) return false;

    if (short_opt && parser_strcmp(arg, short_opt) == 0) {
        return true;
    }
    if (long_opt && parser_strcmp(arg, long_opt) == 0) {
        return true;
    }

    return false;
}

const char* cmd_get_option_value(int argc, char** argv, int* idx, const char* opt) {
    if (!argv || !idx || !opt || *idx >= argc) {
        return NULL;
    }

    const char* arg = argv[*idx];
    size_t opt_len = 0;

    /* Calculate option length */
    while (opt[opt_len]) opt_len++;

    /* Check for -o=value form */
    if (parser_strncmp(arg, opt, opt_len) == 0 && arg[opt_len] == '=') {
        return &arg[opt_len + 1];
    }

    /* Check for -o value form */
    if (parser_strcmp(arg, opt) == 0 && *idx + 1 < argc) {
        (*idx)++;
        return argv[*idx];
    }

    return NULL;
}

bool cmd_parse_int(const char* str, int* result) {
    if (!str || !result) return false;

    int value = 0;
    int sign = 1;
    const char* p = str;

    /* Handle sign */
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    /* Must have at least one digit */
    if (*p < '0' || *p > '9') {
        return false;
    }

    /* Parse digits */
    while (*p >= '0' && *p <= '9') {
        value = value * 10 + (*p - '0');
        p++;
    }

    /* Must be at end of string */
    if (*p != '\0') {
        return false;
    }

    *result = value * sign;
    return true;
}

bool cmd_parse_hex(const char* str, uint32_t* result) {
    if (!str || !result) return false;

    uint32_t value = 0;
    const char* p = str;

    /* Skip optional 0x prefix */
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }

    /* Must have at least one hex digit */
    bool valid = false;

    while (*p) {
        char c = *p;
        uint32_t digit;

        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            digit = c - 'A' + 10;
        } else {
            return false;
        }

        value = (value << 4) | digit;
        valid = true;
        p++;
    }

    if (!valid) return false;

    *result = value;
    return true;
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
 *   - shell/terminal/include/command.h  - API declarations
 *   - shell/terminal/src/dispatch.c     - Command dispatch
 *   - shell/terminal/src/shell.c        - Main shell
 *
 * =============================================================================
 * Generated by Nova Dawn for MillenniumOS
 * CreativeWorkzStudio LLC - Kingdom Technology
 * =============================================================================
 */

// =============================================================================
// END CLOSING
// =============================================================================
