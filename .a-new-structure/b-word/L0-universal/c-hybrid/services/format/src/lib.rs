//omni:code --rust -library
//omni:key B-L0-hybrid-format
//omni:version b-03.00
//omni:pragma PRAGMA
//omni:metadata METADATA
//!
//! # Format Utilities Library
//!
//! Pure value formatting — durations, paths, numbers, strings, and model names.
//! All functions are pure (no side effects except `shorten_path` which reads
//! `$HOME`). No external dependencies beyond identity registration.
//!
//! # Design
//!
//! Format values, never print them. Every function takes a value in and returns
//! a `String` out. The output crate handles terminal rendering; this crate handles
//! the formatting that feeds it.
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
//! assert_eq!(bereshit_l0_format::byte_size(1_536), "1.5 KB");
//! assert_eq!(bereshit_l0_format::count(3, "error", "errors"), "3 errors");
//! ```
//!
//! # OmniCode Identity
//!
//! | Static | Sections | Purpose | Query |
//! |--------|----------|---------|-------|
//! | [`PRAGMA`] | I1-I4 | Identity — who this file is | [`pragma_get`] |
//! | [`METADATA`] | C1-C7 | Context — when, where, why, how | [`metadata_get`] |

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-format
// Purpose: Pure value formatting for durations, paths, numbers, strings, model names
// Biblical: Proverbs 25:11 — A word fitly spoken is like apples of gold in pictures of silver
// Version: b-03.00

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for this crate.
///
/// Carries the I-sections (Identity) of the OmniCode metadata standard.
/// These fields establish WHO this file is in the project graph.
/// Zero-cost at runtime — compiled into the binary as static data.
///
/// # Sections
///
/// | Section | Purpose | Required | Defined |
/// |---------|---------|----------|---------|
/// | **I1** | Core | `key`, `format`, `from`, `at` | — |
/// | **I2** | Family | `type`, `structure` | `subtype`, `role` |
/// | **I3** | Instance | `file`, `title` | `component`, `path`, `provides`, `brief` |
/// | **I4** | Architecture | — | `layer`, `position`, `pattern` |
///
/// # Example
///
/// ```rust
/// let key = bereshit_l0_format::pragma_get("I1.key").unwrap_or("unknown");
/// assert_eq!(key, "B-L0-hybrid-format");
/// ```
#[rustfmt::skip]
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-hybrid-format"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/library.rs"),
    ("I1.at",        "b-03.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I2.role",      "service"),
    // I3: Instance
    ("I3.file",      "lib.rs"),
    ("I3.title",     "Format Utilities Library"),
    ("I3.component", "c-hybrid/services/format"),
    ("I3.path",      "L0-universal/c-hybrid/services/format/src/lib.rs"),
    ("I3.provides",  "FORMAT_UTILITIES"),
    ("I3.brief",     "Pure value formatting — durations, paths, numbers, strings, model names"),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "hybrid/services/format"),
    ("I4.pattern",   "pure functions — take values in, return strings out"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context for this crate.
///
/// Carries the C-sections (Context) of the OmniCode metadata standard.
/// These fields establish WHEN, WHERE, WHY, and HOW for this file.
/// Zero-cost at runtime — compiled into the binary as static data.
///
/// # Sections
///
/// | Section | Purpose | Required | Defined |
/// |---------|---------|----------|---------|
/// | **C1** | State | `version`, `status` | `created`, `updated` |
/// | **C2** | Attribution | `organization` | `architect`, `implementation`, `copyright` |
/// | **C3** | Grounding | `scripture` | `principle`, `anchor` |
/// | **C4** | Dependencies | `requires`, `consumers` | `integration`, `if_missing` |
/// | **C5** | Intent | — | `purpose`, `philosophy` |
/// | **C6** | Roadmap | — | `current`, `planned`, `limitations` |
/// | **C7** | Classification | — | `tags`, `category`, `domain`, `paradigm` |
///
/// # Example
///
/// ```rust
/// let version = bereshit_l0_format::metadata_get("C1.version").unwrap_or("unknown");
/// assert_eq!(version, "b-03.00");
/// ```
#[rustfmt::skip]
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-03.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2025-12-10"),
    ("C1.updated",           "2026-02-18"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Proverbs 25:11 — A word fitly spoken is like apples of gold in pictures of silver"),
    ("C3.principle",         "Format data so humans can ponder it — clarity in presentation honors the reader"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "std::time (Duration), std::env (HOME)"),
    ("C4.requires.external", "none — pure std library"),
    ("C4.requires.internal", "bereshit-l0-identity (global component registry)"),
    ("C4.consumers",         "statusline, session skills, all CPI-SI CLI commands, output crate"),
    ("C4.integration",       "use bereshit_l0_format::{duration, shorten_path, percent, byte_size}"),
    ("C4.if_missing",        "every tool reinvents duration formatting, path shortening, percentage display"),
    // C5: Intent
    ("C5.purpose",           "Consistent human-readable formatting across all CPI-SI tools"),
    ("C5.philosophy",        "Pure functions, no side effects — format values, never print them"),
    // C6: Roadmap
    ("C6.current",           "b-03.00 — 15 pure functions, I/C metadata, byte sizes, counted plurals"),
    ("C6.planned",           "color/ANSI formatting, locale awareness, number grouping (1,234)"),
    ("C6.limitations",       "no color output, no locale, no streaming formatters, byte-level truncation"),
    // C7: Classification
    ("C7.tags",              "format, duration, path, number, string, pure, no-deps, service, omnicode, cpi-si"),
    ("C7.category",          "Service"),
    ("C7.domain",            "output-formatting"),
    ("C7.paradigm",          "CPI-SI"),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// SETUP makes things EXIST. BODY makes things HAPPEN.
//
// Section order (dependency chain):
//
//   [S01] imports — What this file depends on

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

// --- Standard Library ---
use std::env;
use std::time::Duration;

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// BODY makes things HAPPEN. SETUP made things EXIST.
//
// Subsection order:
//   1. Identity Access       — OmniCode static accessor functions
//   4. Core Logic            — All formatting functions (duration, path, number, string, model)
//   7. Free Functions        — register_identity()

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────
//
// OmniCode static accessor functions for PRAGMA and METADATA.
// Every library crate exposes these four functions — they are the crate's
// self-awareness, providing runtime access to compile-time identity data.
//
// Pattern: each pair is (full_slice, key_lookup).
//   Full slice:  returns &[(&str, &str)] for iteration
//   Key lookup:  returns Option<&str> for targeted access

/// Returns this crate's OmniCode pragma identity (I1-I4).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Looks up a specific pragma key. Returns `None` if not found.
///
/// ```
/// assert_eq!(bereshit_l0_format::pragma_get("I1.key"), Some("B-L0-hybrid-format"));
/// assert_eq!(bereshit_l0_format::pragma_get("I1.format"), Some("rust"));
/// assert_eq!(bereshit_l0_format::pragma_get("I99.nope"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (C1-C7).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Looks up a specific metadata key. Returns `None` if not found.
///
/// ```
/// assert_eq!(bereshit_l0_format::metadata_get("C1.version"), Some("b-03.00"));
/// assert_eq!(bereshit_l0_format::metadata_get("C99.nope"), None);
/// ```
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────
//
// All formatting functions organized by domain. Every function is pure:
// takes a value in, returns a String out. No shared state, no mutation.
//
// Domain order: Duration → Path → Number → String → Model

// --- Duration Formatting ---

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

// --- Path Formatting ---

/// Shortens a path for display by replacing `$HOME` with `~` and
/// truncating long paths to the last two components.
///
/// This is the only function with a side effect: it reads `$HOME` from
/// the environment. The read is idempotent and safe for concurrent use.
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

// --- Number Formatting ---

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

/// Formats a byte count as a human-readable size string.
///
/// Uses binary units (1 KB = 1024 bytes). Displays one decimal place
/// for KB and above.
///
/// ```
/// use bereshit_l0_format::byte_size;
///
/// assert_eq!(byte_size(0), "0 B");
/// assert_eq!(byte_size(512), "512 B");
/// assert_eq!(byte_size(1_536), "1.5 KB");
/// assert_eq!(byte_size(2_621_440), "2.5 MB");
/// assert_eq!(byte_size(1_073_741_824), "1.0 GB");
/// ```
pub fn byte_size(bytes: u64) -> String {
    const KB: u64 = 1024;
    const MB: u64 = 1024 * KB;
    const GB: u64 = 1024 * MB;
    const TB: u64 = 1024 * GB;

    if bytes >= TB {
        format!("{:.1} TB", bytes as f64 / TB as f64)
    } else if bytes >= GB {
        format!("{:.1} GB", bytes as f64 / GB as f64)
    } else if bytes >= MB {
        format!("{:.1} MB", bytes as f64 / MB as f64)
    } else if bytes >= KB {
        format!("{:.1} KB", bytes as f64 / KB as f64)
    } else {
        format!("{bytes} B")
    }
}

// --- String Formatting ---

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

/// Formats a count with the appropriate singular or plural noun.
///
/// ```
/// use bereshit_l0_format::count;
///
/// assert_eq!(count(1, "error", "errors"), "1 error");
/// assert_eq!(count(3, "error", "errors"), "3 errors");
/// assert_eq!(count(0, "file", "files"), "0 files");
/// ```
pub fn count(n: usize, singular: &str, plural: &str) -> String {
    if n == 1 {
        format!("1 {singular}")
    } else {
        format!("{n} {plural}")
    }
}

// --- Model Formatting ---

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
        // Legacy Claude 3.x models
        "Claude 3.5 Sonnet" => "Sonnet 3.5",
        "Claude 3.5 Haiku" => "Haiku 3.5",
        "Claude 3 Opus" => "Opus 3",
        "Claude 3 Sonnet" => "Sonnet 3",
        // Claude 4.x models
        "Claude Opus 4" => "Opus 4",
        "Claude Opus 4.5" => "Opus 4.5",
        "Claude Opus 4.6" => "Opus 4.6",
        "Claude Sonnet 4" => "Sonnet 4",
        "Claude Sonnet 4.5" => "Sonnet 4.5",
        "Claude Sonnet 4.6" => "Sonnet 4.6",
        "Claude Haiku 4.5" => "Haiku 4.5",
        // Bare names
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

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────
//
// register_identity() — L0 identity system registration.

/// Register this crate's identity with the global registry.
///
/// Rust equivalent of Go's `init() { identity.Register(key, pragma, metadata) }`.
/// Call once during application startup to make this crate discoverable
/// via `bereshit_l0_identity::lookup()`.
pub fn register_identity() {
    let key = pragma_get("I1.key").expect("I1.key must exist in PRAGMA");
    bereshit_l0_identity::register(key, PRAGMA, METADATA);
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// CLOSING Zones Overview
// ──────────────────────────────────────────────────────────────────────────
//
// 3 Code Zones (operations):  Cv → Ce → Cc
// 5 Doc Sections (guidance):  X1 → X2 → X3 → X4 → X5
//
// Three-tier ordering:
//   1. All code zones before any doc sections
//   2. Within code: Cv (Validation) → Ce (Execution) → Cc (Cleanup)
//   3. Within docs: X1 (Policy) → X2 (Extension) → X3 (Troubleshooting)
//                   → X4 (Reference) → X5 (Note)

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────
//
// 55 tests organized by BODY subsection:
//   §1 Identity (10)  §4 Duration (12)  §4 Path (5)
//   §4 Number (8)     §4 String (11)    §4 Model (5)
//   §7 Registration (4 — doc test coverage)

#[cfg(test)]
mod tests {
    use super::*;

    // ── §1 Identity ─────────────────────────────────────────────────

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("I1.key"), Some("B-L0-hybrid-format"));
    }

    #[test]
    fn pragma_contains_format() {
        assert_eq!(pragma_get("I1.format"), Some("rust"));
    }

    #[test]
    fn pragma_contains_subtype() {
        assert_eq!(pragma_get("I2.subtype"), Some("library"));
    }

    #[test]
    fn pragma_contains_role() {
        assert_eq!(pragma_get("I2.role"), Some("service"));
    }

    #[test]
    fn pragma_get_returns_none_for_missing() {
        assert_eq!(pragma_get("I99.nonexistent"), None);
    }

    #[test]
    fn metadata_contains_version() {
        assert_eq!(metadata_get("C1.version"), Some("b-03.00"));
    }

    #[test]
    fn metadata_contains_grounding() {
        let scripture = metadata_get("C3.scripture").unwrap();
        assert!(scripture.contains("Proverbs 25:11"));
    }

    #[test]
    fn metadata_contains_domain() {
        assert_eq!(metadata_get("C7.domain"), Some("output-formatting"));
    }

    #[test]
    fn metadata_get_returns_none_for_missing() {
        assert_eq!(metadata_get("C99.nope"), None);
    }

    #[test]
    fn pragma_slice_has_all_sections() {
        for prefix in ["I1.", "I2.", "I3.", "I4."] {
            assert!(
                PRAGMA.iter().any(|(k, _)| k.starts_with(prefix)),
                "missing pragma section {prefix}"
            );
        }
    }

    #[test]
    fn metadata_slice_has_all_sections() {
        for prefix in ["C1.", "C2.", "C3.", "C4.", "C5.", "C6.", "C7."] {
            assert!(
                METADATA.iter().any(|(k, _)| k.starts_with(prefix)),
                "missing metadata section {prefix}"
            );
        }
    }

    // ── §4 Duration ─────────────────────────────────────────────────

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
    fn duration_exact_hour() {
        assert_eq!(duration(Duration::from_secs(3_600)), "1h 0m");
    }

    #[test]
    fn duration_exact_minute() {
        assert_eq!(duration(Duration::from_secs(60)), "1m 0s");
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
    fn duration_short_zero() {
        assert_eq!(duration_short(Duration::from_secs(0)), "0m");
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
    fn duration_ms_zero() {
        assert_eq!(duration_ms(0), "<1s");
    }

    #[test]
    fn minutes_with_hours() {
        assert_eq!(minutes(90), "1h 30m");
    }

    #[test]
    fn minutes_under_hour() {
        assert_eq!(minutes(5), "5m 0s");
    }

    // ── §4 Path ─────────────────────────────────────────────────────

    #[test]
    fn shorten_path_empty() {
        assert_eq!(shorten_path(""), "");
    }

    #[test]
    fn shorten_path_short() {
        assert_eq!(shorten_path("/short"), "/short");
    }

    #[test]
    fn shorten_path_no_home() {
        assert_eq!(shorten_path("/usr/local/bin"), "/usr/local/bin");
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

    #[test]
    fn base_name_trailing_slash() {
        assert_eq!(base_name("/usr/local/"), "");
    }

    // ── §4 Number ───────────────────────────────────────────────────

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

    #[test]
    fn byte_size_zero() {
        assert_eq!(byte_size(0), "0 B");
    }

    #[test]
    fn byte_size_bytes() {
        assert_eq!(byte_size(512), "512 B");
    }

    #[test]
    fn byte_size_kilobytes() {
        assert_eq!(byte_size(1_536), "1.5 KB");
    }

    #[test]
    fn byte_size_megabytes() {
        assert_eq!(byte_size(2_621_440), "2.5 MB");
    }

    #[test]
    fn byte_size_gigabytes() {
        assert_eq!(byte_size(1_073_741_824), "1.0 GB");
    }

    #[test]
    fn byte_size_terabytes() {
        assert_eq!(byte_size(1_099_511_627_776), "1.0 TB");
    }

    #[test]
    fn byte_size_exact_kb() {
        assert_eq!(byte_size(1024), "1.0 KB");
    }

    // ── §4 String ───────────────────────────────────────────────────

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
    fn truncate_max_three() {
        assert_eq!(truncate("hello", 3), "hel");
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
    fn pad_right_exact_width() {
        assert_eq!(pad_right("abc", 3), "abc");
    }

    #[test]
    fn pad_left_shorter() {
        assert_eq!(pad_left("42", 5), "   42");
    }

    #[test]
    fn pad_left_already_wide() {
        assert_eq!(pad_left("hello", 3), "hello");
    }

    #[test]
    fn count_singular() {
        assert_eq!(count(1, "error", "errors"), "1 error");
    }

    #[test]
    fn count_plural() {
        assert_eq!(count(3, "error", "errors"), "3 errors");
    }

    #[test]
    fn count_zero() {
        assert_eq!(count(0, "file", "files"), "0 files");
    }

    // ── §4 Model ────────────────────────────────────────────────────

    #[test]
    fn short_model_name_known() {
        assert_eq!(short_model_name("Claude Opus 4"), "Opus 4");
        assert_eq!(short_model_name("Claude 3.5 Sonnet"), "Sonnet 3.5");
        assert_eq!(short_model_name("Claude Opus 4.6"), "Opus 4.6");
        assert_eq!(short_model_name("Claude Sonnet 4.6"), "Sonnet 4.6");
        assert_eq!(short_model_name("Claude Haiku 4.5"), "Haiku 4.5");
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

    #[test]
    fn short_model_name_exact_fifteen() {
        let name = "Exactly Fifteen"; // 15 chars
        assert_eq!(short_model_name(name), "Exactly Fifteen");
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library crate — imported and called, not executed directly.
// Identity registration via register_identity() in BODY §7.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Pure functions. No resources to clean up. No Drop needed.

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Remove not the ancient landmark, which thy fathers have set."
// — Proverbs 22:28
//
// Never:
//   - Change 4-block structure (METADATA → SETUP → BODY → CLOSING)
//   - Change &[(&str, &str)] identity format (I1-I4, C1-C7)
//   - Add non-std dependencies (this crate MUST stay zero-dep + identity)
//   - Remove existing public functions (breaks callers)
//
// Careful:
//   - Existing function signatures — breaks all call sites
//   - Output format strings — downstream may parse them (e.g. "1h 2m")
//   - Model name mappings — status line depends on consistent short names
//   - Truncation/padding behavior — subtle changes break layout
//
// Safe:
//   - Adding new formatting functions (new categories or variants)
//   - Adding new model name mappings
//   - Documentation improvements
//   - New tests

// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Completed (b-03.00):
//   - I1-I4/C1-C7 metadata alignment
//   - Byte size formatting (KB/MB/GB/TB)
//   - Counted plurals (count function)
//   - Updated model name table (Sonnet 4.5/4.6, Haiku 4.5)
//   - register_identity() uses direct slice passing
//   - Numbered BODY subsections matching seed template
//   - Standard CLOSING zones (Cv, Ce, Cc, X1-X5)
//
// Designed Growth Points:
//   - Color/ANSI formatting (feature-gated)
//   - Locale-aware number formatting (1,234 vs 1.234)
//   - Number grouping (thousands separators)
//   - Char-aware truncation (Unicode grapheme boundaries)
//   - Center-pad function
//   - Relative time ("2 hours ago", "just now")
//
// Future Considerations:
//   - C ABI bridge for libtrit formatting
//   - Streaming formatters (Write-based for large outputs)

// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lack wisdom, let him ask of God." — James 1:5
//
// shorten_path returns unchanged path:
//   $HOME env var may not be set. Only replaces exact prefix match.
//   Test with: echo $HOME && cargo test -p bereshit-l0-format
//
// Truncation produces unexpected output:
//   truncate() operates on bytes, not characters. Multi-byte UTF-8
//   strings may truncate mid-character. Use with ASCII or check
//   char boundaries manually.
//
// byte_size shows .0 on exact boundaries:
//   This is intentional — "1.0 KB" is more visually consistent than
//   "1 KB" vs "1.5 KB". All KB+ values show one decimal.
//
// Doc tests fail:
//   The PRAGMA/METADATA accessor doc tests use the crate name
//   (bereshit_l0_format). Ensure Cargo.toml name matches.

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Dependencies:   std only (time, env) + bereshit-l0-identity
// Dependents:     statusline, session skills, all CPI-SI CLI commands
// Parallel:       services/format/go/format.go (Go version)
// Template:       b-word/seed/code/L0/rust/library.rs
//
// Validation:
//   Build:    cargo build -p bereshit-l0-format
//   Test:     cargo test -p bereshit-l0-format
//   Clippy:   cargo clippy -p bereshit-l0-format -- -D warnings
//   Doc:      cargo doc -p bereshit-l0-format --no-deps
//   All:      cargo clippy -p bereshit-l0-format -- -D warnings && cargo test -p bereshit-l0-format

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// Format utilities — the bridge between raw data and human comprehension.
// Every duration displayed, every path shortened, every percentage
// calculated serves the same purpose: making information accessible
// so humans can ponder it and act wisely.
//
// "A word fitly spoken is like apples of gold in pictures of silver."
// — Proverbs 25:11
//
// Fitly spoken — not just spoken, but FORMATTED for the setting.
// Gold in silver. The right presentation makes value visible.
//
// Genesis 1:1 — In the beginning God created the heaven and the earth.

// ============================================================================
// END CLOSING
// ============================================================================
