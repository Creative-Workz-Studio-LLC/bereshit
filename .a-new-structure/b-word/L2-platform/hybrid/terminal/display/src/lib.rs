//omni:code --rust -library
//omni:key B-L2-display
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L2-display
// Purpose: Shared terminal display colors and formatting
// Biblical: Proverbs 25:11 — "A word fitly spoken is like apples of gold"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// Used by: statusline, skills with CLI output
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// --- Reset ---

pub const RESET: &str = "\x1b[0m";

// --- Styles ---

pub const BOLD: &str = "\x1b[1m";
pub const DIM: &str = "\x1b[2m";
pub const ITALIC: &str = "\x1b[3m";
pub const UNDERLINE: &str = "\x1b[4m";

// --- Standard Colors ---

pub const BLACK: &str = "\x1b[30m";
pub const RED: &str = "\x1b[31m";
pub const GREEN: &str = "\x1b[32m";
pub const YELLOW: &str = "\x1b[33m";
pub const BLUE: &str = "\x1b[34m";
pub const MAGENTA: &str = "\x1b[35m";
pub const CYAN: &str = "\x1b[36m";
pub const WHITE: &str = "\x1b[37m";
pub const GRAY: &str = "\x1b[90m";

// --- Bright Colors ---

pub const BRIGHT_BLACK: &str = "\x1b[90m";
pub const BRIGHT_RED: &str = "\x1b[91m";
pub const BRIGHT_GREEN: &str = "\x1b[92m";
pub const BRIGHT_YELLOW: &str = "\x1b[93m";
pub const BRIGHT_BLUE: &str = "\x1b[94m";
pub const BRIGHT_MAGENTA: &str = "\x1b[95m";
pub const BRIGHT_CYAN: &str = "\x1b[96m";
pub const BRIGHT_WHITE: &str = "\x1b[97m";

// --- Backgrounds ---

pub const BG_BLACK: &str = "\x1b[40m";
pub const BG_RED: &str = "\x1b[41m";
pub const BG_GREEN: &str = "\x1b[42m";
pub const BG_YELLOW: &str = "\x1b[43m";
pub const BG_BLUE: &str = "\x1b[44m";
pub const BG_MAGENTA: &str = "\x1b[45m";
pub const BG_CYAN: &str = "\x1b[46m";
pub const BG_WHITE: &str = "\x1b[47m";

// ============================================================================
// BODY
// ============================================================================

/// Wraps text in color codes.
pub fn colorize(text: &str, color: &str) -> String {
    format!("{color}{text}{RESET}")
}

/// Wraps text in bold + color.
pub fn bold_colorize(text: &str, color: &str) -> String {
    format!("{BOLD}{color}{text}{RESET}")
}

/// Applies dim styling to text.
pub fn dim_text(text: &str) -> String {
    format!("{DIM}{text}{RESET}")
}

/// Formats text as green (success).
pub fn success(text: &str) -> String {
    format!("{GREEN}{text}{RESET}")
}

/// Formats text as yellow (warning).
pub fn warning(text: &str) -> String {
    format!("{YELLOW}{text}{RESET}")
}

/// Formats text as red (error).
pub fn error(text: &str) -> String {
    format!("{RED}{text}{RESET}")
}

/// Formats text as cyan (info).
pub fn info(text: &str) -> String {
    format!("{CYAN}{text}{RESET}")
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn colorize_wraps_with_reset() {
        let result = colorize("hello", GREEN);
        assert!(result.starts_with(GREEN));
        assert!(result.ends_with(RESET));
        assert!(result.contains("hello"));
    }

    #[test]
    fn bold_colorize_includes_bold() {
        let result = bold_colorize("test", RED);
        assert!(result.starts_with(BOLD));
        assert!(result.contains(RED));
        assert!(result.ends_with(RESET));
    }

    #[test]
    fn semantic_helpers_use_correct_colors() {
        assert!(success("ok").starts_with(GREEN));
        assert!(warning("warn").starts_with(YELLOW));
        assert!(error("err").starts_with(RED));
        assert!(info("msg").starts_with(CYAN));
    }
}
