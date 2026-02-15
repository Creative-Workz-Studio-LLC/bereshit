//omni:code --rust -library
//omni:key B-L0-hybrid-format
//omni:version b-02.00
//omni:pragma PRAGMA
//omni:metadata METADATA

// ============================================================================
// METADATA
// ============================================================================

//! # Format Utilities Library
//!
//! Pure value formatting — durations, paths, numbers, strings, and model names.
//! All functions are pure (no side effects except `shorten_path` which reads
//! `$HOME`). No external dependencies.
//!
//! # Usage
//!
//! ```
//! use std::time::Duration;
//!
//! assert_eq!(bereshit_l0_format::duration(Duration::from_secs(222)), "3m 42s");
//! assert_eq!(bereshit_l0_format::shorten_path("/short"), "/short");
//! assert_eq!(bereshit_l0_format::truncate("hello world", 8), "hello...");
//! assert_eq!(bereshit_l0_format::percent(75.0, 100.0), "75%");
//! ```
//!
//! See [`PRAGMA`] and [`METADATA`] for OmniCode identity.

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

/// OmniCode pragma identity (P1-P5). Static, zero-cost at runtime.
pub static PRAGMA: &[(&str, &str)] = &[
    // P1: Core Identity
    ("P1.key", "B-L0-hybrid-format"),
    ("P1.type", "code"),
    ("P1.format", "rust"),
    ("P1.style", "library"),
    // P2: Structure
    ("P2.blocks", "4-block"),
    // P3: Derivation
    ("P3.from", "b-word/seed/code/L0/rust/library.rs"),
    ("P3.derives", "B-seed-code-L0-rust-library"),
    ("P3.note", "Converted from Go — pure function library"),
    // P4: Version
    ("P4.at", "b-02.00"),
    // P5: Summary
    ("P5.title", "Format Utilities Library"),
    (
        "P5.summary",
        "Pure formatting for durations, paths, numbers, strings, and model names",
    ),
];

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

/// OmniCode metadata (M1-M10). Static, zero-cost at runtime.
pub static METADATA: &[(&str, &str)] = &[
    // -- M1: Core Identity ------------------------------------------------
    ("M1.key", "B-L0-hybrid-format"),
    ("M1.component_type", "Foundation"),
    ("M1.architect", "Seanje Lenox-Wise"),
    ("M1.implementation", "Nova Dawn"),
    ("M1.created", "2025-12-10"),
    // -- M2: Version History ----------------------------------------------
    (
        "M2.b-02.00",
        "2026-02-14 — RUST: Rewritten as pure Rust, no external deps",
    ),
    (
        "M2.b-01.01",
        "2026-02-14 — GO: promoted from hybrid/util/ to hybrid/ top-level",
    ),
    (
        "M2.b-01.00",
        "2026-02-14 — GO: BASELINE with identity registration",
    ),
    ("M2.a-01.00", "2025-12-10 — Initial creation (Go)"),
    // -- M3: Interface ----------------------------------------------------
    ("M3.requires.stdlib", "std::time, std::env"),
    ("M3.requires.external", "none"),
    ("M3.requires.internal", "none (self-contained at L0)"),
    (
        "M3.used_by",
        "statusline, session skills, CLI output formatting",
    ),
    ("M3.import", "bereshit_l0_format"),
    (
        "M3.pattern",
        "format::duration(d), format::shorten_path(p), format::truncate(s, n)",
    ),
    // -- M4: Public API ---------------------------------------------------
    (
        "M4.duration",
        "duration, duration_short, duration_ms, minutes",
    ),
    ("M4.path", "shorten_path, base_name"),
    ("M4.number", "percent"),
    ("M4.string", "truncate, pad_right, pad_left"),
    ("M4.model", "short_model_name"),
    ("M4.identity", "pragma, pragma_get, metadata, metadata_get"),
    // -- M5: Operational --------------------------------------------------
    ("M5.blocking", "no — pure value formatting only"),
    (
        "M5.health",
        "Foundation | granted: formatted | deferred: n/a | denied: n/a",
    ),
    // -- M6: Classification -----------------------------------------------
    (
        "M6.tags",
        "format, duration, path, number, string, pure, no-deps",
    ),
    ("M6.category", "Foundation"),
    ("M6.domain", "output-formatting"),
    ("M6.layer", "L0-universal"),
    ("M6.paradigm", "CPI-SI"),
    // -- M7: Intent -------------------------------------------------------
    (
        "M7.purpose",
        "Consistent human-readable formatting across all CPI-SI tools",
    ),
    (
        "M7.philosophy",
        "Pure functions, no side effects — format values, never print them",
    ),
    (
        "M7.provides",
        "Duration, path, number, string, and model name formatting",
    ),
    // -- M8: Grounding ----------------------------------------------------
    (
        "M8.scripture",
        "Proverbs 4:26 — Ponder the path of thy feet",
    ),
    (
        "M8.principle",
        "Clarity in presentation — format data so humans can ponder it",
    ),
    (
        "M8.anchor",
        "Genesis 1:1 — In the beginning God created the heaven and the earth",
    ),
    // -- M9: Dependencies -------------------------------------------------
    ("M9.needs.stdlib", "std::time (Duration), std::env (HOME)"),
    ("M9.needs.external", "none"),
    ("M9.needs.internal", "none (self-contained)"),
    (
        "M9.used_by",
        "statusline, session skills, all CPI-SI CLI commands",
    ),
    ("M9.layer_deps", "L0 only — no L1+ dependencies"),
    // -- M10: Roadmap -----------------------------------------------------
    (
        "M10.current",
        "b-02.00 — RUST: pure functions, zero dependencies",
    ),
    (
        "M10.planned",
        "Byte size formatting, color/ANSI, locale awareness",
    ),
    (
        "M10.limitations",
        "No color output, no locale, no streaming formatters",
    ),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// No types or error enums — all functions are pure value transformers.
// Imports kept minimal: std::time for Duration, std::env for HOME.

use std::env;
use std::time::Duration;

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Identity -> Duration -> Path -> Number -> String -> Model

// ────────────────────────────────────────────────────────────────
// Identity — Static OmniCode Access
// ────────────────────────────────────────────────────────────────

/// Returns this crate's OmniCode pragma identity (P1-P5).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Looks up a specific pragma key. Returns `None` if not found.
///
/// ```
/// assert_eq!(bereshit_l0_format::pragma_get("P1.key"), Some("B-L0-hybrid-format"));
/// assert_eq!(bereshit_l0_format::pragma_get("P99.nope"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (M1-M10).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Looks up a specific metadata key. Returns `None` if not found.
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Register this crate's identity with the global registry.
///
/// Rust equivalent of Go's `init() { identity.Register(key, pragma, metadata) }`.
/// Call once during application startup to make this crate discoverable
/// via `bereshit_l0_identity::lookup()`.
pub fn register_identity() {
    let key = pragma_get("P1.key").expect("P1.key must exist in PRAGMA");
    let pragma_map = PRAGMA
        .iter()
        .map(|(k, v)| ((*k).to_owned(), (*v).to_owned()))
        .collect();
    let metadata_map = METADATA
        .iter()
        .map(|(k, v)| ((*k).to_owned(), (*v).to_owned()))
        .collect();
    bereshit_l0_identity::register(key, pragma_map, metadata_map);
}

// ────────────────────────────────────────────────────────────────
// Duration Formatting
// ────────────────────────────────────────────────────────────────

/// Formats a [`Duration`] as a human-readable string.
///
/// Returns `"Xh Ym"` for hours+, `"Xm Ys"` for minutes+, `"Xs"` for seconds.
///
/// ```
/// use std::time::Duration;
/// use bereshit_l0_format::duration;
///
/// assert_eq!(duration(Duration::from_secs(3_723)), "1h 2m");
/// assert_eq!(duration(Duration::from_secs(222)), "3m 42s");
/// assert_eq!(duration(Duration::from_secs(7)), "7s");
/// assert_eq!(duration(Duration::from_secs(0)), "0s");
/// ```
pub fn duration(d: Duration) -> String {
    let total = d.as_secs();
    let hours = total / 3600;
    let minutes = (total % 3600) / 60;
    let seconds = total % 60;

    if hours > 0 {
        format!("{hours}h {minutes}m")
    } else if minutes > 0 {
        format!("{minutes}m {seconds}s")
    } else {
        format!("{seconds}s")
    }
}

/// Formats a duration compactly without spaces.
///
/// Returns `"XhYm"` for hours+, `"Xm"` for minutes.
///
/// ```
/// use std::time::Duration;
/// use bereshit_l0_format::duration_short;
///
/// assert_eq!(duration_short(Duration::from_secs(3_723)), "1h2m");
/// assert_eq!(duration_short(Duration::from_secs(222)), "3m");
/// ```
pub fn duration_short(d: Duration) -> String {
    let total = d.as_secs();
    let hours = total / 3600;
    let minutes = (total % 3600) / 60;

    if hours > 0 {
        format!("{hours}h{minutes}m")
    } else {
        format!("{minutes}m")
    }
}

/// Formats a millisecond count as a human-readable duration.
///
/// Returns `"<1s"` for values under 1000ms.
///
/// ```
/// use bereshit_l0_format::duration_ms;
///
/// assert_eq!(duration_ms(500), "<1s");
/// assert_eq!(duration_ms(65_000), "1m 5s");
/// ```
pub fn duration_ms(ms: u64) -> String {
    if ms < 1000 {
        "<1s".to_string()
    } else {
        duration(Duration::from_millis(ms))
    }
}

/// Formats a minute count as a human-readable duration.
///
/// ```
/// use bereshit_l0_format::minutes;
///
/// assert_eq!(minutes(90), "1h 30m");
/// assert_eq!(minutes(5), "5m 0s");
/// ```
pub fn minutes(m: u64) -> String {
    duration(Duration::from_secs(m * 60))
}

// ────────────────────────────────────────────────────────────────
// Path Formatting
// ────────────────────────────────────────────────────────────────

/// Shortens a path for display by replacing `$HOME` with `~` and
/// truncating long paths to the last two components.
///
/// ```
/// use bereshit_l0_format::shorten_path;
///
/// assert_eq!(shorten_path(""), "");
/// assert_eq!(shorten_path("/short"), "/short");
/// ```
pub fn shorten_path(path: &str) -> String {
    if path.is_empty() {
        return String::new();
    }

    let mut result = path.to_string();

    // Replace home with ~
    if let Ok(home) = env::var("HOME") {
        if let Some(rest) = result.strip_prefix(&home) {
            result = format!("~{rest}");
        }
    }

    // If still too long, truncate with .../last/two
    if result.len() > 50 {
        let parts: Vec<&str> = result.split('/').collect();
        if parts.len() > 3 {
            return format!(".../{}", parts[parts.len() - 2..].join("/"));
        }
    }

    result
}

/// Returns the last component of a slash-separated path.
///
/// ```
/// use bereshit_l0_format::base_name;
///
/// assert_eq!(base_name("/usr/local/bin"), "bin");
/// assert_eq!(base_name("single"), "single");
/// assert_eq!(base_name(""), "");
/// ```
pub fn base_name(path: &str) -> &str {
    path.rsplit('/').next().unwrap_or(path)
}

// ────────────────────────────────────────────────────────────────
// Number Formatting
// ────────────────────────────────────────────────────────────────

/// Formats a value/total ratio as a percentage string.
///
/// Returns `"0%"` if total is zero.
///
/// ```
/// use bereshit_l0_format::percent;
///
/// assert_eq!(percent(75.0, 100.0), "75%");
/// assert_eq!(percent(1.0, 3.0), "33%");
/// assert_eq!(percent(0.0, 0.0), "0%");
/// ```
pub fn percent(value: f64, total: f64) -> String {
    if total == 0.0 {
        return "0%".to_string();
    }
    let pct = (value / total) * 100.0;
    format!("{}%", pct as i64)
}

// ────────────────────────────────────────────────────────────────
// String Formatting
// ────────────────────────────────────────────────────────────────

/// Truncates a string to `max` bytes, appending `"..."` if shortened.
///
/// If `max <= 3`, truncates without ellipsis.
///
/// ```
/// use bereshit_l0_format::truncate;
///
/// assert_eq!(truncate("hello world", 8), "hello...");
/// assert_eq!(truncate("short", 10), "short");
/// assert_eq!(truncate("ab", 2), "ab");
/// ```
pub fn truncate(s: &str, max: usize) -> String {
    if s.len() <= max {
        return s.to_string();
    }
    if max <= 3 {
        return s[..max].to_string();
    }
    format!("{}...", &s[..max - 3])
}

/// Pads a string with trailing spaces to reach `width`.
///
/// Returns the string unchanged if already at or beyond `width`.
///
/// ```
/// use bereshit_l0_format::pad_right;
///
/// assert_eq!(pad_right("hi", 5), "hi   ");
/// assert_eq!(pad_right("hello", 3), "hello");
/// ```
pub fn pad_right(s: &str, width: usize) -> String {
    if s.len() >= width {
        return s.to_string();
    }
    format!("{}{}", s, " ".repeat(width - s.len()))
}

/// Pads a string with leading spaces to reach `width`.
///
/// Returns the string unchanged if already at or beyond `width`.
///
/// ```
/// use bereshit_l0_format::pad_left;
///
/// assert_eq!(pad_left("42", 5), "   42");
/// assert_eq!(pad_left("hello", 3), "hello");
/// ```
pub fn pad_left(s: &str, width: usize) -> String {
    if s.len() >= width {
        return s.to_string();
    }
    format!("{}{}", " ".repeat(width - s.len()), s)
}

// ────────────────────────────────────────────────────────────────
// Model Formatting
// ────────────────────────────────────────────────────────────────

/// Extracts a short model name for display from a full Claude model name.
///
/// Returns `"Unknown"` for empty strings.
///
/// ```
/// use bereshit_l0_format::short_model_name;
///
/// assert_eq!(short_model_name("Claude Opus 4"), "Opus 4");
/// assert_eq!(short_model_name("Claude 3.5 Sonnet"), "Sonnet 3.5");
/// assert_eq!(short_model_name("Sonnet"), "Sonnet");
/// assert_eq!(short_model_name(""), "Unknown");
/// ```
pub fn short_model_name(display_name: &str) -> String {
    if display_name.is_empty() {
        return "Unknown".to_string();
    }

    let short = match display_name {
        "Claude 3.5 Sonnet" => "Sonnet 3.5",
        "Claude 3.5 Haiku" => "Haiku 3.5",
        "Claude 3 Opus" => "Opus 3",
        "Claude 3 Sonnet" => "Sonnet 3",
        "Claude Opus 4" => "Opus 4",
        "Claude Opus 4.5" => "Opus 4.5",
        "Claude Opus 4.6" => "Opus 4.6",
        "Claude Sonnet 4" => "Sonnet 4",
        "Claude Sonnet 4.5" => "Sonnet 4.5",
        "Sonnet" => "Sonnet",
        "Opus" => "Opus",
        "Haiku" => "Haiku",
        _ => {
            return if display_name.len() > 15 {
                format!("{}...", &display_name[..15])
            } else {
                display_name.to_string()
            };
        }
    };

    short.to_string()
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// Pure formatting utilities — take values in, return strings out.
// No side effects (except shorten_path reads $HOME), no shared state.
// Safe for concurrent use. Zero external dependencies.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new formatting functions (new categories or variants)
//   - Add new model name mappings to short_model_name
//   - Improve truncation/padding with char-aware versions
//
// Modify with Care:
//   - Existing function signatures — breaks callers
//   - Output format strings — downstream may parse them
//
// Never Modify:
//   - 4-block structure
//   - PRAGMA/METADATA static data format
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:  std only (time, env)
// Dependents:    statusline, session skills, CPI-SI CLI commands
// Supersedes:    hybrid/format/format.go (Go version, b-01.01)
// Parallel:      hybrid/output (terminal rendering), hybrid/cli (flag parsing)
//
// "Ponder the path of thy feet." — Proverbs 4:26
//
// ============================================================================
// END CLOSING
// ============================================================================

// ============================================================================
// Tests
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    // -- Duration -------------------------------------------------

    #[test]
    fn duration_zero() {
        assert_eq!(duration(Duration::from_secs(0)), "0s");
    }

    #[test]
    fn duration_seconds_only() {
        assert_eq!(duration(Duration::from_secs(7)), "7s");
    }

    #[test]
    fn duration_minutes_and_seconds() {
        assert_eq!(duration(Duration::from_secs(222)), "3m 42s");
    }

    #[test]
    fn duration_hours_and_minutes() {
        assert_eq!(duration(Duration::from_secs(3_723)), "1h 2m");
    }

    #[test]
    fn duration_short_hours() {
        assert_eq!(duration_short(Duration::from_secs(3_723)), "1h2m");
    }

    #[test]
    fn duration_short_minutes() {
        assert_eq!(duration_short(Duration::from_secs(222)), "3m");
    }

    #[test]
    fn duration_ms_under_one_second() {
        assert_eq!(duration_ms(500), "<1s");
    }

    #[test]
    fn duration_ms_over_one_second() {
        assert_eq!(duration_ms(65_000), "1m 5s");
    }

    #[test]
    fn minutes_with_hours() {
        assert_eq!(minutes(90), "1h 30m");
    }

    #[test]
    fn minutes_under_hour() {
        assert_eq!(minutes(5), "5m 0s");
    }

    // -- Path -----------------------------------------------------

    #[test]
    fn shorten_path_empty() {
        assert_eq!(shorten_path(""), "");
    }

    #[test]
    fn shorten_path_short() {
        assert_eq!(shorten_path("/short"), "/short");
    }

    #[test]
    fn base_name_multi_component() {
        assert_eq!(base_name("/usr/local/bin"), "bin");
    }

    #[test]
    fn base_name_single() {
        assert_eq!(base_name("single"), "single");
    }

    #[test]
    fn base_name_empty() {
        assert_eq!(base_name(""), "");
    }

    // -- Number ---------------------------------------------------

    #[test]
    fn percent_normal() {
        assert_eq!(percent(75.0, 100.0), "75%");
    }

    #[test]
    fn percent_fraction() {
        assert_eq!(percent(1.0, 3.0), "33%");
    }

    #[test]
    fn percent_zero_total() {
        assert_eq!(percent(0.0, 0.0), "0%");
    }

    #[test]
    fn percent_full() {
        assert_eq!(percent(100.0, 100.0), "100%");
    }

    // -- String ---------------------------------------------------

    #[test]
    fn truncate_short_string() {
        assert_eq!(truncate("short", 10), "short");
    }

    #[test]
    fn truncate_exact_length() {
        assert_eq!(truncate("exact", 5), "exact");
    }

    #[test]
    fn truncate_with_ellipsis() {
        assert_eq!(truncate("hello world", 8), "hello...");
    }

    #[test]
    fn truncate_tiny_max() {
        assert_eq!(truncate("hello", 2), "he");
    }

    #[test]
    fn pad_right_shorter() {
        assert_eq!(pad_right("hi", 5), "hi   ");
    }

    #[test]
    fn pad_right_already_wide() {
        assert_eq!(pad_right("hello", 3), "hello");
    }

    #[test]
    fn pad_left_shorter() {
        assert_eq!(pad_left("42", 5), "   42");
    }

    #[test]
    fn pad_left_already_wide() {
        assert_eq!(pad_left("hello", 3), "hello");
    }

    // -- Model ----------------------------------------------------

    #[test]
    fn short_model_name_known() {
        assert_eq!(short_model_name("Claude Opus 4"), "Opus 4");
        assert_eq!(short_model_name("Claude 3.5 Sonnet"), "Sonnet 3.5");
        assert_eq!(short_model_name("Claude Opus 4.6"), "Opus 4.6");
    }

    #[test]
    fn short_model_name_passthrough() {
        assert_eq!(short_model_name("Sonnet"), "Sonnet");
        assert_eq!(short_model_name("Custom Model"), "Custom Model");
    }

    #[test]
    fn short_model_name_empty() {
        assert_eq!(short_model_name(""), "Unknown");
    }

    #[test]
    fn short_model_name_long_truncates() {
        let long = "Very Long Model Name That Exceeds Limit";
        let result = short_model_name(long);
        assert!(result.ends_with("..."));
        assert!(result.len() <= 18); // 15 + "..."
    }

    // -- Identity -------------------------------------------------

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("P1.key"), Some("B-L0-hybrid-format"));
    }

    #[test]
    fn pragma_contains_format() {
        assert_eq!(pragma_get("P1.format"), Some("rust"));
    }

    #[test]
    fn pragma_get_returns_none_for_missing() {
        assert_eq!(pragma_get("P99.nonexistent"), None);
    }

    #[test]
    fn metadata_contains_grounding() {
        let scripture = metadata_get("M8.scripture").unwrap();
        assert!(scripture.contains("Proverbs 4:26"));
    }
}
