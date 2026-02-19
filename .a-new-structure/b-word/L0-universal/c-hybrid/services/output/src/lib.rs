//omni:code --rust -library
//omni:key B-L0-hybrid-output
//omni:version b-03.00
//omni:pragma PRAGMA
//omni:metadata METADATA
//!
//! # Output Formatting Library
//!
//! Terminal output formatting for CPI-SI tools — headers, sections, key-value
//! pairs, tables, status indicators, and JSON helpers. All writing to stdout.
//! Pure stdlib, no external deps beyond identity registration.
//!
//! # Design
//!
//! **Writer-based testability.** Every output function has two layers:
//!
//! 1. A private `write_*(&mut impl Write)` function that does the real work
//! 2. A public function that locks stdout and delegates
//!
//! Tests exercise the write functions directly against `Vec<u8>` buffers,
//! verifying exact output without touching stdout.
//!
//! # Usage
//!
//! ```no_run
//! use bereshit_l0_output as output;
//!
//! output::header("System Status");
//! output::key_value("Uptime", "3h 42m");
//! output::success("All checks passed");
//! output::table_header(&["Name", "Status", "Score"]);
//! ```
//!
//! # Public API
//!
//! | Category | Functions |
//! |----------|-----------|
//! | Headers | `header`, `section`, `sub_section`, `divider` |
//! | Formatted | `key_value`, `key_value_width`, `bullet`, `numbered`, `indent`, `blank_line` |
//! | Tables | `table_header`, `table_row` |
//! | Status | `success`, `info`, `tip`, `warning`, `error` |
//! | JSON | `json_start`, `json_end`, `json_field`, `json_field_last`, `json_object_start`, `json_object_end`, `json_object_end_last` |
//! | Identity | `pragma`, `pragma_get`, `metadata`, `metadata_get`, `register_identity` |

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-output
// Purpose: Terminal output formatting — headers, tables, status, JSON,
//          writer-based testability
// Biblical: Colossians 4:6 — Let your speech be alway with grace,
//           seasoned with salt
// Version: b-03.00

// ──────────────────────────────────────────────────────────────────────────
// PRAGMA — I1-I4: Identity (WHAT this component IS)
// ──────────────────────────────────────────────────────────────────────────
//
// Compile-time constants. Zero runtime cost. Accessed via pragma() / pragma_get().
//
// ```
// let key = bereshit_l0_output::pragma_get("I1.key");
// assert_eq!(key, Some("B-L0-hybrid-output"));
// ```

#[rustfmt::skip]
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-hybrid-output"),
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
    ("I3.title",     "Output Formatting Library"),
    ("I3.component", "c-hybrid/services/output"),
    ("I3.path",      "L0-universal/c-hybrid/services/output/src/lib.rs"),
    ("I3.provides",  "OUTPUT_FORMATTING"),
    ("I3.brief",     "Terminal output formatting — headers, tables, status, JSON, writer-based testability"),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "hybrid/services/output"),
    ("I4.pattern",   "writer-based — private write_* for testability, public stdout wrappers"),
];

// ──────────────────────────────────────────────────────────────────────────
// METADATA — C1-C7: Context (what I need to KNOW about this component)
// ──────────────────────────────────────────────────────────────────────────
//
// Compile-time constants. Accessed via metadata() / metadata_get().
//
// ```
// let scripture = bereshit_l0_output::metadata_get("C3.scripture");
// assert!(scripture.unwrap().contains("Colossians 4:6"));
// ```

#[rustfmt::skip]
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-03.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2025-12-10"),
    ("C1.updated",           "2026-02-19"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Colossians 4:6 — Let your speech be alway with grace, seasoned with salt"),
    ("C3.principle",         "Graceful output — structured, clear, readable by humans"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "std::io (Write, stdout), std::fmt (Display)"),
    ("C4.requires.external", "none — pure std library"),
    ("C4.requires.internal", "bereshit-l0-identity (global component registry)"),
    ("C4.consumers",         "all CPI-SI CLI commands, statusline, session skills"),
    ("C4.integration",       "use bereshit_l0_output::{header, key_value, success, json_field}"),
    ("C4.if_missing",        "every tool reinvents header formatting, status prefixes, table alignment"),
    // C5: Intent
    ("C5.purpose",           "Consistent terminal output across all CPI-SI tools"),
    ("C5.philosophy",        "Print functions handle layout — callers handle logic"),
    // C6: Roadmap
    ("C6.current",           "b-03.00 — writer-based, 5 output domains, I/C metadata, warning/error status"),
    ("C6.planned",           "ANSI color support, progress indicators, structured logging"),
    ("C6.limitations",       "no color, no progress indicators, stdout only, no stderr"),
    // C7: Classification
    ("C7.tags",              "output, headers, tables, json, status, pure, stdlib-only, service, omnicode, cpi-si"),
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
// section-order: [s01]-imports, [s06]-types, [s08]-macros

// ── S01: Imports ─────────────────────────────────────────────────────────

use std::fmt;
use std::io::{self, Write};

// ── S06: Types ───────────────────────────────────────────────────────────

/// Value type for JSON output helpers.
///
/// Provides type-safe formatting: strings are quoted and escaped,
/// booleans and numbers render as JSON literals.
///
/// Implements `From` for common types — pass values directly to
/// [`json_field`] and [`json_field_last`].
///
/// ```
/// use bereshit_l0_output::JsonVal;
///
/// let s: JsonVal = "hello".into();
/// let b: JsonVal = true.into();
/// let n: JsonVal = 42_i64.into();
/// let f: JsonVal = 3.14_f64.into();
/// ```
#[derive(Debug)]
pub enum JsonVal<'a> {
    /// String value — quoted and escaped in output.
    Str(&'a str),
    /// Boolean value — `true` or `false` literal.
    Bool(bool),
    /// Integer value — unquoted numeric literal.
    Int(i64),
    /// Floating-point value — unquoted numeric literal.
    Float(f64),
}

// ── S08: Macros ──────────────────────────────────────────────────────────

/// Lock stdout and call a write function.
macro_rules! stdout {
    ($f:expr) => {{
        let stdout = io::stdout();
        let mut lock = stdout.lock();
        $f(&mut lock).expect("stdout write failed");
    }};
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: [1]-identity, [3]-trait-impls, [4]-core-logic, [5]-public-api, [7]-free-functions
//
// §4 Core Logic contains the private write_* functions (testable against Vec<u8>).
// §5 Public API contains the stdout wrappers (lock + delegate).
// §7 Free Functions contains register_identity().

// ── §1 Identity Access ──────────────────────────────────────────────────
//
// Standard CPI-SI identity interface. Every crate exposes its PRAGMA (I1-I4)
// and METADATA (C1-C7) as static slices, plus lookup helpers.

/// Returns this crate's OmniCode pragma identity (I1-I4).
///
/// ```
/// let pragma = bereshit_l0_output::pragma();
/// assert!(pragma.iter().any(|(k, _)| *k == "I1.key"));
/// ```
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Look up a single pragma value by key.
///
/// ```
/// use bereshit_l0_output::pragma_get;
///
/// assert_eq!(pragma_get("I1.key"), Some("B-L0-hybrid-output"));
/// assert_eq!(pragma_get("I1.format"), Some("rust"));
/// assert_eq!(pragma_get("missing"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (C1-C7).
///
/// ```
/// let metadata = bereshit_l0_output::metadata();
/// assert!(metadata.iter().any(|(k, _)| *k == "C1.version"));
/// ```
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Look up a single metadata value by key.
///
/// ```
/// use bereshit_l0_output::metadata_get;
///
/// assert_eq!(metadata_get("C1.version"), Some("b-03.00"));
/// assert_eq!(metadata_get("missing"), None);
/// ```
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ── §3 Trait Implementations ─────────────────────────────────────────────
//
// From<T> conversions for JsonVal — enables ergonomic API:
//   json_field("key", "string")   — From<&str>
//   json_field("key", true)       — From<bool>
//   json_field("key", 42_i64)     — From<i64>
//   json_field("key", 3.14_f64)   — From<f64>

impl<'a> From<&'a str> for JsonVal<'a> {
    fn from(s: &'a str) -> Self {
        JsonVal::Str(s)
    }
}

impl From<bool> for JsonVal<'_> {
    fn from(b: bool) -> Self {
        JsonVal::Bool(b)
    }
}

impl From<i64> for JsonVal<'_> {
    fn from(n: i64) -> Self {
        JsonVal::Int(n)
    }
}

impl From<f64> for JsonVal<'_> {
    fn from(f: f64) -> Self {
        JsonVal::Float(f)
    }
}

// ── §4 Core Logic ───────────────────────────────────────────────────────
//
// Private write functions that take `&mut impl Write` for testability.
// Each function writes to any `Write` implementor — tests use `Vec<u8>`,
// production uses stdout.

// ── §4.1 Helpers ────────────────────────────────────────────────────────

/// Escape a string for safe JSON embedding (quotes, backslashes, control chars).
fn escape_json(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    for c in s.chars() {
        match c {
            '"' => out.push_str("\\\""),
            '\\' => out.push_str("\\\\"),
            '\n' => out.push_str("\\n"),
            '\r' => out.push_str("\\r"),
            '\t' => out.push_str("\\t"),
            c => out.push(c),
        }
    }
    out
}

// ── §4.2 Write Functions — Headers ──────────────────────────────────────

fn write_header(w: &mut impl Write, title: &str) -> io::Result<()> {
    let width = title.len().max(40);
    writeln!(w, "{title}")?;
    writeln!(w, "{}", "=".repeat(width))?;
    writeln!(w)
}

fn write_section(w: &mut impl Write, title: &str) -> io::Result<()> {
    writeln!(w, "{title}")?;
    writeln!(w, "{}", "-".repeat(title.len()))?;
    writeln!(w)
}

fn write_sub_section(w: &mut impl Write, title: &str) -> io::Result<()> {
    writeln!(w, "## {title}")?;
    writeln!(w)
}

fn write_divider(w: &mut impl Write, ch: &str, width: usize) -> io::Result<()> {
    writeln!(w, "{}", ch.repeat(width))
}

// ── §4.3 Write Functions — Formatted Output ─────────────────────────────

fn write_key_value(w: &mut impl Write, key: &str, value: impl fmt::Display) -> io::Result<()> {
    let label = format!("{key}:");
    writeln!(w, "{label:<12} {value}")
}

fn write_key_value_width(
    w: &mut impl Write,
    key: &str,
    value: impl fmt::Display,
    width: usize,
) -> io::Result<()> {
    let label = format!("{key}:");
    writeln!(w, "{label:<width$} {value}")
}

fn write_bullet(w: &mut impl Write, text: &str) -> io::Result<()> {
    writeln!(w, "  - {text}")
}

fn write_numbered(w: &mut impl Write, n: usize, text: &str) -> io::Result<()> {
    writeln!(w, "  {n}. {text}")
}

fn write_indent(w: &mut impl Write, text: &str, level: usize) -> io::Result<()> {
    let prefix = "  ".repeat(level);
    writeln!(w, "{prefix}{text}")
}

fn write_blank_line(w: &mut impl Write) -> io::Result<()> {
    writeln!(w)
}

// ── §4.4 Write Functions — Tables ───────────────────────────────────────

fn write_table_header(w: &mut impl Write, columns: &[&str]) -> io::Result<()> {
    let widths: Vec<usize> = columns.iter().map(|c| c.len() + 2).collect();
    write_table_row(w, &widths, columns)?;
    let dashes: Vec<String> = widths.iter().map(|&w| "-".repeat(w)).collect();
    writeln!(w, "{}", dashes.join(" "))
}

fn write_table_row(w: &mut impl Write, widths: &[usize], values: &[&str]) -> io::Result<()> {
    let parts: Vec<String> = values
        .iter()
        .enumerate()
        .map(|(i, v)| {
            let col_w = widths.get(i).copied().unwrap_or(10);
            format!("{v:<col_w$}")
        })
        .collect();
    writeln!(w, "{}", parts.join(" "))
}

// ── §4.5 Write Functions — Status Indicators ────────────────────────────

fn write_success(w: &mut impl Write, msg: &str) -> io::Result<()> {
    writeln!(w, "OK {msg}")
}

fn write_info(w: &mut impl Write, msg: &str) -> io::Result<()> {
    writeln!(w, "INFO {msg}")
}

fn write_tip(w: &mut impl Write, msg: &str) -> io::Result<()> {
    writeln!(w, "TIP {msg}")
}

fn write_warning(w: &mut impl Write, msg: &str) -> io::Result<()> {
    writeln!(w, "WARN {msg}")
}

fn write_error(w: &mut impl Write, msg: &str) -> io::Result<()> {
    writeln!(w, "ERROR {msg}")
}

// ── §4.6 Write Functions — JSON Output ──────────────────────────────────

fn write_json_val(
    w: &mut impl Write,
    key: &str,
    value: &JsonVal<'_>,
    comma: bool,
) -> io::Result<()> {
    let trail = if comma { "," } else { "" };
    match value {
        JsonVal::Str(s) => {
            let escaped = escape_json(s);
            writeln!(w, "  \"{key}\": \"{escaped}\"{trail}")
        }
        JsonVal::Bool(b) => writeln!(w, "  \"{key}\": {b}{trail}"),
        JsonVal::Int(n) => writeln!(w, "  \"{key}\": {n}{trail}"),
        JsonVal::Float(f) => writeln!(w, "  \"{key}\": {f}{trail}"),
    }
}

fn write_json_object_start(w: &mut impl Write, key: &str) -> io::Result<()> {
    writeln!(w, "  \"{key}\": {{")
}

// ── §5 Public API ───────────────────────────────────────────────────────
//
// Each function locks stdout and delegates to its write_* counterpart.
// Callers get a simple `fn(args)` interface; tests get `write_*(writer, args)`.

// ── §5.1 Headers ────────────────────────────────────────────────────────

/// Print a title with a double-line `=` separator beneath it.
/// Minimum separator width is 40 characters.
///
/// ```no_run
/// bereshit_l0_output::header("System Status");
/// ```
pub fn header(title: &str) {
    stdout!(|w| write_header(w, title));
}

/// Print a section title with a single-line `-` separator beneath it.
///
/// ```no_run
/// bereshit_l0_output::section("Configuration");
/// ```
pub fn section(title: &str) {
    stdout!(|w| write_section(w, title));
}

/// Print a subsection title with `##` prefix.
///
/// ```no_run
/// bereshit_l0_output::sub_section("Details");
/// ```
pub fn sub_section(title: &str) {
    stdout!(|w| write_sub_section(w, title));
}

/// Print a line of `ch` repeated to `width`.
///
/// ```no_run
/// bereshit_l0_output::divider("─", 60);
/// ```
pub fn divider(ch: &str, width: usize) {
    stdout!(|w| write_divider(w, ch, width));
}

// ── §5.2 Formatted Output ──────────────────────────────────────────────

/// Print a key-value pair with the key left-aligned to 12 characters.
///
/// ```no_run
/// bereshit_l0_output::key_value("Status", "Active");
/// bereshit_l0_output::key_value("Count", 42);
/// ```
pub fn key_value(key: &str, value: impl fmt::Display) {
    let stdout = io::stdout();
    let mut lock = stdout.lock();
    write_key_value(&mut lock, key, value).expect("stdout write failed");
}

/// Print a key-value pair with a custom key column width.
///
/// ```no_run
/// bereshit_l0_output::key_value_width("Long Label", "value", 20);
/// ```
pub fn key_value_width(key: &str, value: impl fmt::Display, width: usize) {
    let stdout = io::stdout();
    let mut lock = stdout.lock();
    write_key_value_width(&mut lock, key, value, width).expect("stdout write failed");
}

/// Print a bulleted list item with `  - ` prefix.
///
/// ```no_run
/// bereshit_l0_output::bullet("First item");
/// bereshit_l0_output::bullet("Second item");
/// ```
pub fn bullet(text: &str) {
    stdout!(|w| write_bullet(w, text));
}

/// Print a numbered list item with `  N. ` prefix.
///
/// ```no_run
/// bereshit_l0_output::numbered(1, "First step");
/// bereshit_l0_output::numbered(2, "Second step");
/// ```
pub fn numbered(n: usize, text: &str) {
    stdout!(|w| write_numbered(w, n, text));
}

/// Print text indented by `level` (2 spaces per level).
///
/// ```no_run
/// bereshit_l0_output::indent("nested", 2);
/// // Output: "    nested"
/// ```
pub fn indent(text: &str, level: usize) {
    stdout!(|w| write_indent(w, text, level));
}

/// Print a blank line.
///
/// ```no_run
/// bereshit_l0_output::blank_line();
/// ```
pub fn blank_line() {
    stdout!(write_blank_line);
}

// ── §5.3 Tables ─────────────────────────────────────────────────────────

/// Print a table header row with auto-sized columns and dashed separator.
///
/// Column widths are `column_name.len() + 2` for padding.
///
/// ```no_run
/// bereshit_l0_output::table_header(&["Name", "Status", "Score"]);
/// ```
pub fn table_header(columns: &[&str]) {
    stdout!(|w| write_table_header(w, columns));
}

/// Print a table row with values aligned to specified column widths.
/// Columns without a corresponding width default to 10 characters.
///
/// ```no_run
/// bereshit_l0_output::table_row(&[12, 10, 8], &["Nova", "Active", "100"]);
/// ```
pub fn table_row(widths: &[usize], values: &[&str]) {
    stdout!(|w| write_table_row(w, widths, values));
}

// ── §5.4 Status Indicators ──────────────────────────────────────────────

/// Print a success message prefixed with `OK `.
///
/// ```no_run
/// bereshit_l0_output::success("All checks passed");
/// ```
pub fn success(msg: &str) {
    stdout!(|w| write_success(w, msg));
}

/// Print an informational message prefixed with `INFO `.
///
/// ```no_run
/// bereshit_l0_output::info("Loading configuration");
/// ```
pub fn info(msg: &str) {
    stdout!(|w| write_info(w, msg));
}

/// Print a guidance message prefixed with `TIP `.
///
/// ```no_run
/// bereshit_l0_output::tip("Use --verbose for details");
/// ```
pub fn tip(msg: &str) {
    stdout!(|w| write_tip(w, msg));
}

/// Print a warning message prefixed with `WARN `.
///
/// ```no_run
/// bereshit_l0_output::warning("Deprecated configuration key");
/// ```
pub fn warning(msg: &str) {
    stdout!(|w| write_warning(w, msg));
}

/// Print an error message prefixed with `ERROR `.
///
/// ```no_run
/// bereshit_l0_output::error("Failed to read config file");
/// ```
pub fn error(msg: &str) {
    stdout!(|w| write_error(w, msg));
}

// ── §5.5 JSON Output ────────────────────────────────────────────────────

/// Print the opening brace of a JSON object.
///
/// ```no_run
/// bereshit_l0_output::json_start();
/// bereshit_l0_output::json_field("name", "Nova");
/// bereshit_l0_output::json_field_last("role", "CPI-SI");
/// bereshit_l0_output::json_end();
/// ```
pub fn json_start() {
    let stdout = io::stdout();
    let mut lock = stdout.lock();
    writeln!(lock, "{{").expect("stdout write failed");
}

/// Print the closing brace of a JSON object.
pub fn json_end() {
    let stdout = io::stdout();
    let mut lock = stdout.lock();
    writeln!(lock, "}}").expect("stdout write failed");
}

/// Print a JSON field with a trailing comma.
///
/// Values are formatted by type: strings quoted and escaped,
/// booleans and numbers as literals.
///
/// ```no_run
/// use bereshit_l0_output as output;
///
/// output::json_field("name", "Nova Dawn");
/// output::json_field("active", true);
/// output::json_field("score", 100_i64);
/// output::json_field_last("ratio", 0.95_f64);
/// ```
pub fn json_field<'a>(key: &str, value: impl Into<JsonVal<'a>>) {
    let val = value.into();
    stdout!(|w| write_json_val(w, key, &val, true));
}

/// Print a JSON field without a trailing comma (last field in object).
pub fn json_field_last<'a>(key: &str, value: impl Into<JsonVal<'a>>) {
    let val = value.into();
    stdout!(|w| write_json_val(w, key, &val, false));
}

/// Print the opening of a nested JSON object.
///
/// ```no_run
/// use bereshit_l0_output as output;
///
/// output::json_start();
/// output::json_object_start("config");
/// output::json_field_last("verbose", true);
/// output::json_object_end_last();
/// output::json_end();
/// ```
pub fn json_object_start(key: &str) {
    stdout!(|w| write_json_object_start(w, key));
}

/// Print the closing of a nested JSON object with trailing comma.
pub fn json_object_end() {
    let stdout = io::stdout();
    let mut lock = stdout.lock();
    writeln!(lock, "  }},").expect("stdout write failed");
}

/// Print the closing of a nested JSON object without trailing comma.
pub fn json_object_end_last() {
    let stdout = io::stdout();
    let mut lock = stdout.lock();
    writeln!(lock, "  }}").expect("stdout write failed");
}

// ── §7 Free Functions ───────────────────────────────────────────────────
//
// Module-level functions that don't fit in the public API categories.
// Currently just register_identity().

/// Register this crate's identity with the global registry.
///
/// Rust equivalent of Go's `init() { identity.Register(key, pragma, metadata) }`.
/// Call once during application startup to make this crate discoverable
/// via `bereshit_l0_identity::lookup()`.
///
/// ```no_run
/// bereshit_l0_output::register_identity();
/// ```
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

// ── Cv: Closing Validation ──────────────────────────────────────────────
//
// All tests live here. Writer-based functions tested against Vec<u8> buffers.
// Public stdout wrappers are thin delegation — testing the write_* layer
// provides full coverage.

#[cfg(test)]
mod tests {
    use super::*;

    /// Capture write function output into a String.
    fn capture<F>(f: F) -> String
    where
        F: FnOnce(&mut Vec<u8>) -> io::Result<()>,
    {
        let mut buf = Vec::new();
        f(&mut buf).expect("write failed");
        String::from_utf8(buf).expect("invalid utf8")
    }

    // ── Identity ────────────────────────────────────────────────

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("I1.key"), Some("B-L0-hybrid-output"));
    }

    #[test]
    fn pragma_contains_format() {
        assert_eq!(pragma_get("I1.format"), Some("rust"));
    }

    #[test]
    fn pragma_contains_role() {
        assert_eq!(pragma_get("I2.role"), Some("service"));
    }

    #[test]
    fn pragma_get_missing_returns_none() {
        assert_eq!(pragma_get("nonexistent"), None);
    }

    #[test]
    fn metadata_contains_version() {
        assert_eq!(metadata_get("C1.version"), Some("b-03.00"));
    }

    #[test]
    fn metadata_contains_scripture() {
        let scripture = metadata_get("C3.scripture").unwrap();
        assert!(scripture.contains("Colossians 4:6"));
    }

    #[test]
    fn metadata_contains_consumers() {
        let consumers = metadata_get("C4.consumers").unwrap();
        assert!(consumers.contains("CPI-SI"));
    }

    #[test]
    fn metadata_get_missing_returns_none() {
        assert_eq!(metadata_get("missing"), None);
    }

    #[test]
    fn pragma_slice_not_empty() {
        assert!(!pragma().is_empty());
    }

    #[test]
    fn metadata_slice_not_empty() {
        assert!(!metadata().is_empty());
    }

    // ── Headers and Sections ────────────────────────────────────

    #[test]
    fn header_title_and_separator() {
        let out = capture(|w| write_header(w, "Test"));
        assert!(out.starts_with("Test\n"));
        // Min separator width is 40.
        assert!(out.contains(&"=".repeat(40)));
    }

    #[test]
    fn header_long_title_matches_width() {
        let title = "A".repeat(60);
        let out = capture(|w| write_header(w, &title));
        assert!(out.contains(&"=".repeat(60)));
    }

    #[test]
    fn header_ends_with_blank_line() {
        let out = capture(|w| write_header(w, "Title"));
        assert!(out.ends_with("\n\n"));
    }

    #[test]
    fn section_title_and_separator() {
        let out = capture(|w| write_section(w, "Details"));
        assert!(out.starts_with("Details\n"));
        assert!(out.contains(&"-".repeat(7)));
    }

    #[test]
    fn section_separator_matches_title_length() {
        let out = capture(|w| write_section(w, "AB"));
        let lines: Vec<&str> = out.lines().collect();
        assert_eq!(lines[1], "--");
    }

    #[test]
    fn sub_section_prefix() {
        let out = capture(|w| write_sub_section(w, "Part A"));
        assert_eq!(out, "## Part A\n\n");
    }

    #[test]
    fn divider_repeats_char() {
        let out = capture(|w| write_divider(w, "*", 5));
        assert_eq!(out, "*****\n");
    }

    #[test]
    fn divider_with_unicode() {
        let out = capture(|w| write_divider(w, "─", 3));
        assert_eq!(out, "───\n");
    }

    // ── Formatted Output ────────────────────────────────────────

    #[test]
    fn key_value_default_width() {
        let out = capture(|w| write_key_value(w, "Status", "Active"));
        assert_eq!(out, "Status:      Active\n");
    }

    #[test]
    fn key_value_numeric() {
        let out = capture(|w| write_key_value(w, "Count", 42));
        assert_eq!(out, "Count:       42\n");
    }

    #[test]
    fn key_value_custom_width() {
        let out = capture(|w| write_key_value_width(w, "Score", 42, 20));
        assert!(out.starts_with("Score:"));
        assert!(out.trim_end().ends_with("42"));
    }

    #[test]
    fn key_value_width_narrow() {
        let out = capture(|w| write_key_value_width(w, "X", "Y", 4));
        // "X:" = 2 chars, padded to at least 4
        assert!(out.contains("X:"));
        assert!(out.contains("Y"));
    }

    #[test]
    fn bullet_prefix() {
        let out = capture(|w| write_bullet(w, "item one"));
        assert_eq!(out, "  - item one\n");
    }

    #[test]
    fn numbered_prefix() {
        let out = capture(|w| write_numbered(w, 3, "third item"));
        assert_eq!(out, "  3. third item\n");
    }

    #[test]
    fn numbered_double_digit() {
        let out = capture(|w| write_numbered(w, 10, "tenth"));
        assert_eq!(out, "  10. tenth\n");
    }

    #[test]
    fn indent_levels() {
        let out = capture(|w| write_indent(w, "deep", 3));
        assert_eq!(out, "      deep\n"); // 3 levels x 2 spaces = 6
    }

    #[test]
    fn indent_zero() {
        let out = capture(|w| write_indent(w, "top", 0));
        assert_eq!(out, "top\n");
    }

    #[test]
    fn blank_line_output() {
        let out = capture(|w| write_blank_line(w));
        assert_eq!(out, "\n");
    }

    // ── Tables ──────────────────────────────────────────────────

    #[test]
    fn table_header_with_separator() {
        let out = capture(|w| write_table_header(w, &["Name", "Status"]));
        let lines: Vec<&str> = out.lines().collect();
        assert_eq!(lines.len(), 2);
        assert!(lines[0].contains("Name"));
        assert!(lines[0].contains("Status"));
        assert!(lines[1].contains("------"));
        assert!(lines[1].contains("--------"));
    }

    #[test]
    fn table_header_single_column() {
        let out = capture(|w| write_table_header(w, &["Title"]));
        let lines: Vec<&str> = out.lines().collect();
        assert_eq!(lines.len(), 2);
        assert!(lines[0].contains("Title"));
    }

    #[test]
    fn table_row_aligns_columns() {
        let out = capture(|w| write_table_row(w, &[10, 8], &["Nova", "Active"]));
        assert!(out.starts_with("Nova      "));
    }

    #[test]
    fn table_row_default_width() {
        let out = capture(|w| write_table_row(w, &[5], &["A", "B"]));
        assert!(out.contains("A"));
        assert!(out.contains("B"));
    }

    // ── Status Indicators ───────────────────────────────────────

    #[test]
    fn success_prefix() {
        let out = capture(|w| write_success(w, "passed"));
        assert_eq!(out, "OK passed\n");
    }

    #[test]
    fn info_prefix() {
        let out = capture(|w| write_info(w, "loading"));
        assert_eq!(out, "INFO loading\n");
    }

    #[test]
    fn tip_prefix() {
        let out = capture(|w| write_tip(w, "try this"));
        assert_eq!(out, "TIP try this\n");
    }

    #[test]
    fn warning_prefix() {
        let out = capture(|w| write_warning(w, "deprecated key"));
        assert_eq!(out, "WARN deprecated key\n");
    }

    #[test]
    fn error_prefix() {
        let out = capture(|w| write_error(w, "file not found"));
        assert_eq!(out, "ERROR file not found\n");
    }

    // ── JSON Output ─────────────────────────────────────────────

    #[test]
    fn json_field_string() {
        let val = JsonVal::Str("hello");
        let out = capture(|w| write_json_val(w, "name", &val, true));
        assert_eq!(out, "  \"name\": \"hello\",\n");
    }

    #[test]
    fn json_field_bool() {
        let val = JsonVal::Bool(true);
        let out = capture(|w| write_json_val(w, "active", &val, true));
        assert_eq!(out, "  \"active\": true,\n");
    }

    #[test]
    fn json_field_int() {
        let val = JsonVal::Int(42);
        let out = capture(|w| write_json_val(w, "count", &val, true));
        assert_eq!(out, "  \"count\": 42,\n");
    }

    #[test]
    fn json_field_float() {
        let val = JsonVal::Float(3.14);
        let out = capture(|w| write_json_val(w, "score", &val, false));
        assert_eq!(out, "  \"score\": 3.14\n");
    }

    #[test]
    fn json_field_last_no_comma() {
        let val = JsonVal::Str("end");
        let out = capture(|w| write_json_val(w, "final", &val, false));
        assert!(!out.contains(','));
    }

    #[test]
    fn json_field_negative_int() {
        let val = JsonVal::Int(-1);
        let out = capture(|w| write_json_val(w, "delta", &val, true));
        assert_eq!(out, "  \"delta\": -1,\n");
    }

    #[test]
    fn json_object_start_format() {
        let out = capture(|w| write_json_object_start(w, "config"));
        assert_eq!(out, "  \"config\": {\n");
    }

    // ── JSON Escaping ───────────────────────────────────────────

    #[test]
    fn escape_json_special_chars() {
        assert_eq!(escape_json("a\"b"), "a\\\"b");
        assert_eq!(escape_json("a\\b"), "a\\\\b");
        assert_eq!(escape_json("a\nb"), "a\\nb");
        assert_eq!(escape_json("a\tb"), "a\\tb");
        assert_eq!(escape_json("a\rb"), "a\\rb");
    }

    #[test]
    fn escape_json_plain_string() {
        assert_eq!(escape_json("hello world"), "hello world");
    }

    #[test]
    fn escape_json_empty_string() {
        assert_eq!(escape_json(""), "");
    }

    #[test]
    fn escape_json_multiple_specials() {
        assert_eq!(escape_json("\"\\\""), "\\\"\\\\\\\"");
    }

    #[test]
    fn json_field_escapes_string() {
        let val = JsonVal::Str("say \"hi\"");
        let out = capture(|w| write_json_val(w, "msg", &val, true));
        assert!(out.contains("say \\\"hi\\\""));
    }

    // ── From Conversions ────────────────────────────────────────

    #[test]
    fn json_val_from_str() {
        let val: JsonVal = "test".into();
        assert!(matches!(val, JsonVal::Str("test")));
    }

    #[test]
    fn json_val_from_bool() {
        let val: JsonVal = true.into();
        assert!(matches!(val, JsonVal::Bool(true)));
    }

    #[test]
    fn json_val_from_bool_false() {
        let val: JsonVal = false.into();
        assert!(matches!(val, JsonVal::Bool(false)));
    }

    #[test]
    fn json_val_from_i64() {
        let val: JsonVal = 99_i64.into();
        assert!(matches!(val, JsonVal::Int(99)));
    }

    #[test]
    fn json_val_from_f64() {
        let val: JsonVal = 1.5_f64.into();
        assert!(matches!(val, JsonVal::Float(_)));
    }

    #[test]
    fn json_val_from_i64_negative() {
        let val: JsonVal = (-42_i64).into();
        assert!(matches!(val, JsonVal::Int(-42)));
    }
}

// ── Ce: Closing Execution ───────────────────────────────────────────────
//
// Library — imported, not executed. No main() function.
// Registration via register_identity() during application startup.

// ── Cc: Closing Cleanup ─────────────────────────────────────────────────
//
// No resources to clean up. Stdout lock acquired and released per call.
// No persistent state between calls.

// ── X1: Modification Policy ─────────────────────────────────────────────
//
// Scripture: Proverbs 22:28 — "Remove not the ancient landmark."
//
// Never:
//   - Change public function signatures (downstream CLI tools depend on them)
//   - Add non-std dependencies (this crate MUST stay lightweight)
//   - Remove the writer-based test layer (testability is the architecture)
//   - Remove bereshit-l0-identity dependency (breaks registration)
//
// Careful:
//   - Adding new status prefixes (all tools need to handle them)
//   - Changing output format strings (downstream may parse them)
//   - Table alignment logic changes (callers assume specific widths)
//
// Safe:
//   - Adding new output functions (headers, formatters, JSON helpers)
//   - Documentation improvements
//   - New From<T> implementations for JsonVal
//   - Dev-dependency additions for testing

// ── X2: Extension Points ────────────────────────────────────────────────
//
// Completed: Rust conversion, writer-based testability, I/C metadata, warning/error
// Designed: ANSI color support (feature-gated), progress indicators
// Future: Structured logging mode, stderr for errors, table auto-sizing

// ── X3: Troubleshooting ─────────────────────────────────────────────────
//
// Scripture: James 1:5 — "If any of you lack wisdom, let him ask of God."
//
// stdout_locked: If output is garbled, ensure only one thread writes at a time.
// table_misalign: Column widths in table_row must match table_header widths.
// json_invalid: Use json_field for all but last field, json_field_last for last.
// panic_on_write: stdout write panics if pipe is broken (e.g., `tool | head`).

// ── X4: Reference ───────────────────────────────────────────────────────
//
// Related files:
//   src/lib.rs              — This file: 5 output domains, writer-based testability
//   go/output.go            — Go counterpart
//   ../format/src/lib.rs    — Format crate (values → strings, this crate prints them)
//   seed/code/L0/rust/library.rs — Template this derives from

// ── X5: Closing Note ────────────────────────────────────────────────────
//
// "Let your speech be alway with grace, seasoned with salt."
// — Colossians 4:6
//
// Terminal output IS speech. Headers, tables, status lines — all of it
// is how a tool speaks to its user. Grace in output means structure,
// clarity, and consistency. Salt means it carries meaning.
//
// Genesis 1:1 — In the beginning God created the heaven and the earth.

// ============================================================================
// END CLOSING
// ============================================================================
