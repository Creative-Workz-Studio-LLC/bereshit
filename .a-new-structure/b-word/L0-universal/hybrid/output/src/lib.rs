//omni:code --rust -library
//omni:key B-L0-hybrid-output
//omni:version b-02.00
//omni:pragma PRAGMA
//omni:metadata METADATA

//! # bereshit-l0-output
//!
//! Terminal output formatting for CPI-SI tools.
//!
//! Headers, sections, key-value pairs, tables, status indicators, and JSON
//! helpers — all writing to stdout. Pure stdlib, no external deps.
//!
//! ```no_run
//! use bereshit_l0_output as output;
//!
//! output::header("System Status");
//! output::key_value("Uptime", "3h 42m");
//! output::success("All checks passed");
//! output::table_header(&["Name", "Status", "Score"]);
//! ```

// ============================================================================
// METADATA
// ============================================================================

/// OmniCode pragma identity (P1-P5). Compile-time constants.
pub static PRAGMA: &[(&str, &str)] = &[
    // P1: Core Identity
    ("P1.key", "B-L0-hybrid-output"),
    ("P1.type", "code"),
    ("P1.format", "rust"),
    ("P1.style", "library"),
    // P2: Structure
    ("P2.blocks", "4-block"),
    // P3: Derivation
    ("P3.from", "b-word/seed/code/L0/rust/library.rs"),
    ("P3.derives", "B-seed-code-L0-rust-library"),
    // P4: Version
    ("P4.at", "b-02.00"),
    // P5: Summary
    ("P5.title", "Output Formatting Library"),
    ("P5.summary", "Terminal output formatting for CPI-SI tools"),
];

/// OmniCode metadata (M1-M10). Compile-time constants.
pub static METADATA: &[(&str, &str)] = &[
    // ── M1: Core Identity ─────────────────────────────────────
    ("M1.key", "B-L0-hybrid-output"),
    ("M1.component_type", "Foundation"),
    ("M1.architect", "Seanje Lenox-Wise"),
    ("M1.implementation", "Nova Dawn"),
    ("M1.created", "2025-12-10"),
    // ── M2: Version History ───────────────────────────────────
    (
        "M2.b-02.00",
        "2026-02-14 — Rust conversion: writer-based, testable, zero deps",
    ),
    (
        "M2.b-01.01",
        "2026-02-14 — REORG: promoted to hybrid/ top-level domain",
    ),
    (
        "M2.b-01.00",
        "2026-02-14 — BASELINE: 4-block, identity, pragma/metadata",
    ),
    ("M2.a-01.00", "2025-12-10 — Initial creation (Go)"),
    // ── M3: Interface ─────────────────────────────────────────
    ("M3.requires.stdlib", "std::io, std::fmt"),
    ("M3.requires.external", "none"),
    (
        "M3.pattern",
        "output::header(title), output::key_value(k, v)",
    ),
    // ── M4: Public API ────────────────────────────────────────
    ("M4.headers", "header, section, sub_section, divider"),
    (
        "M4.formatted",
        "key_value, key_value_width, bullet, numbered, indent",
    ),
    ("M4.tables", "table_header, table_row"),
    ("M4.status", "success, info, tip"),
    (
        "M4.json",
        "json_start, json_end, json_field, json_field_last, json_object_start, json_object_end, json_object_end_last",
    ),
    ("M4.self", "pragma, pragma_get, metadata, metadata_get"),
    // ── M5: Operational ───────────────────────────────────────
    ("M5.blocking", "no — stdout writes only"),
    ("M5.health", "Foundation | granted: printed | deferred: n/a"),
    // ── M6: Classification ────────────────────────────────────
    (
        "M6.tags",
        "output, headers, tables, json, status, pure, stdlib-only",
    ),
    ("M6.category", "Foundation"),
    ("M6.domain", "output-formatting"),
    ("M6.layer", "L0-universal"),
    // ── M7: Intent ────────────────────────────────────────────
    (
        "M7.purpose",
        "Consistent terminal output across all CPI-SI tools",
    ),
    (
        "M7.philosophy",
        "Print functions handle layout — callers handle logic",
    ),
    // ── M8: Grounding ─────────────────────────────────────────
    (
        "M8.scripture",
        "Colossians 4:6 — Let your speech be alway with grace",
    ),
    (
        "M8.principle",
        "Graceful output — structured, clear, readable by humans",
    ),
    (
        "M8.anchor",
        "Genesis 1:1 — In the beginning God created the heaven and the earth",
    ),
    // ── M9: Dependencies ──────────────────────────────────────
    (
        "M9.needs.stdlib",
        "std::io (Write, stdout), std::fmt (Display)",
    ),
    ("M9.needs.external", "none"),
    ("M9.layer_deps", "L0 only — no L1+ dependencies"),
    // ── M10: Roadmap ──────────────────────────────────────────
    (
        "M10.current",
        "b-02.00 — Rust conversion with full test suite",
    ),
    ("M10.planned", "ANSI color support, progress indicators"),
    (
        "M10.limitations",
        "No color, no progress indicators, stdout only",
    ),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

use std::fmt;
use std::io::{self, Write};

// ── Types ────────────────────────────────────────────────────────

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

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Helpers → Write Functions → Public API → Identity
//
// Write functions take `&mut impl Write` for testability.
// Public functions lock stdout and delegate.

// ── Helpers ──────────────────────────────────────────────────────

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

/// Lock stdout and call a write function.
macro_rules! stdout {
    ($f:expr) => {{
        let stdout = io::stdout();
        let mut lock = stdout.lock();
        $f(&mut lock).expect("stdout write failed");
    }};
}

// ── Write Functions (private, testable) ──────────────────────────

// Headers and Sections

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

// Formatted Output

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

// Tables

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

// Status Indicators

fn write_success(w: &mut impl Write, msg: &str) -> io::Result<()> {
    writeln!(w, "OK {msg}")
}

fn write_info(w: &mut impl Write, msg: &str) -> io::Result<()> {
    writeln!(w, "INFO {msg}")
}

fn write_tip(w: &mut impl Write, msg: &str) -> io::Result<()> {
    writeln!(w, "TIP {msg}")
}

// JSON Output

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

// ── Public API (stdout) ──────────────────────────────────────────

/// Print a title with a double-line `=` separator beneath it.
/// Minimum separator width is 40 characters.
pub fn header(title: &str) {
    stdout!(|w| write_header(w, title));
}

/// Print a section title with a single-line `-` separator beneath it.
pub fn section(title: &str) {
    stdout!(|w| write_section(w, title));
}

/// Print a subsection title with `##` prefix.
pub fn sub_section(title: &str) {
    stdout!(|w| write_sub_section(w, title));
}

/// Print a line of `ch` repeated to `width`.
pub fn divider(ch: &str, width: usize) {
    stdout!(|w| write_divider(w, ch, width));
}

/// Print a key-value pair with the key left-aligned to 12 characters.
pub fn key_value(key: &str, value: impl fmt::Display) {
    let stdout = io::stdout();
    let mut lock = stdout.lock();
    write_key_value(&mut lock, key, value).expect("stdout write failed");
}

/// Print a key-value pair with a custom key column width.
pub fn key_value_width(key: &str, value: impl fmt::Display, width: usize) {
    let stdout = io::stdout();
    let mut lock = stdout.lock();
    write_key_value_width(&mut lock, key, value, width).expect("stdout write failed");
}

/// Print a bulleted list item with `  - ` prefix.
pub fn bullet(text: &str) {
    stdout!(|w| write_bullet(w, text));
}

/// Print a numbered list item with `  N. ` prefix.
pub fn numbered(n: usize, text: &str) {
    stdout!(|w| write_numbered(w, n, text));
}

/// Print text indented by `level` (2 spaces per level).
pub fn indent(text: &str, level: usize) {
    stdout!(|w| write_indent(w, text, level));
}

/// Print a table header row with auto-sized columns and dashed separator.
pub fn table_header(columns: &[&str]) {
    stdout!(|w| write_table_header(w, columns));
}

/// Print a table row with values aligned to specified column widths.
/// Columns without a corresponding width default to 10 characters.
pub fn table_row(widths: &[usize], values: &[&str]) {
    stdout!(|w| write_table_row(w, widths, values));
}

/// Print a success message prefixed with `OK `.
pub fn success(msg: &str) {
    stdout!(|w| write_success(w, msg));
}

/// Print an informational message prefixed with `INFO `.
pub fn info(msg: &str) {
    stdout!(|w| write_info(w, msg));
}

/// Print a guidance message prefixed with `TIP `.
pub fn tip(msg: &str) {
    stdout!(|w| write_tip(w, msg));
}

/// Print the opening brace of a JSON object.
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

// ── Identity ─────────────────────────────────────────────────────

/// Returns this crate's OmniCode pragma identity (P1-P5).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Look up a single pragma value by key.
///
/// ```
/// use bereshit_l0_output::pragma_get;
///
/// assert_eq!(pragma_get("P1.key"), Some("B-L0-hybrid-output"));
/// assert_eq!(pragma_get("P1.format"), Some("rust"));
/// assert_eq!(pragma_get("missing"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (M1-M10).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Look up a single metadata value by key.
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

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

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

    // ── Headers and Sections ─────────────────────────────────

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
    fn section_title_and_separator() {
        let out = capture(|w| write_section(w, "Details"));
        assert!(out.starts_with("Details\n"));
        assert!(out.contains(&"-".repeat(7)));
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

    // ── Formatted Output ─────────────────────────────────────

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
        // "Score:" = 6 chars, padded to 20.
        assert!(out.starts_with("Score:"));
        assert!(out.trim_end().ends_with("42"));
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
    fn indent_levels() {
        let out = capture(|w| write_indent(w, "deep", 3));
        assert_eq!(out, "      deep\n"); // 3 levels x 2 spaces = 6
    }

    #[test]
    fn indent_zero() {
        let out = capture(|w| write_indent(w, "top", 0));
        assert_eq!(out, "top\n");
    }

    // ── Tables ───────────────────────────────────────────────

    #[test]
    fn table_header_with_separator() {
        let out = capture(|w| write_table_header(w, &["Name", "Status"]));
        let lines: Vec<&str> = out.lines().collect();
        assert_eq!(lines.len(), 2);
        assert!(lines[0].contains("Name"));
        assert!(lines[0].contains("Status"));
        // "Name" + 2 = 6 dashes, "Status" + 2 = 8 dashes.
        assert!(lines[1].contains("------"));
        assert!(lines[1].contains("--------"));
    }

    #[test]
    fn table_row_aligns_columns() {
        let out = capture(|w| write_table_row(w, &[10, 8], &["Nova", "Active"]));
        // "Nova" padded to 10 chars, "Active" padded to 8.
        assert!(out.starts_with("Nova      "));
    }

    #[test]
    fn table_row_default_width() {
        // Fewer widths than values — extra columns default to 10.
        let out = capture(|w| write_table_row(w, &[5], &["A", "B"]));
        assert!(out.contains("A"));
        assert!(out.contains("B"));
    }

    // ── Status Indicators ────────────────────────────────────

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

    // ── JSON Output ──────────────────────────────────────────

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
    fn json_object_start_format() {
        let out = capture(|w| write_json_object_start(w, "config"));
        assert_eq!(out, "  \"config\": {\n");
    }

    // ── JSON Escaping ────────────────────────────────────────

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
    fn json_field_escapes_string() {
        let val = JsonVal::Str("say \"hi\"");
        let out = capture(|w| write_json_val(w, "msg", &val, true));
        assert!(out.contains("say \\\"hi\\\""));
    }

    // ── From Conversions ─────────────────────────────────────

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
    fn json_val_from_i64() {
        let val: JsonVal = 99_i64.into();
        assert!(matches!(val, JsonVal::Int(99)));
    }

    #[test]
    fn json_val_from_f64() {
        let val: JsonVal = 1.5_f64.into();
        assert!(matches!(val, JsonVal::Float(_)));
    }

    // ── Identity ─────────────────────────────────────────────

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("P1.key"), Some("B-L0-hybrid-output"));
    }

    #[test]
    fn pragma_contains_format() {
        assert_eq!(pragma_get("P1.format"), Some("rust"));
    }

    #[test]
    fn pragma_get_missing() {
        assert_eq!(pragma_get("nonexistent"), None);
    }

    #[test]
    fn metadata_contains_grounding() {
        let scripture = metadata_get("M8.scripture").unwrap();
        assert!(scripture.contains("Colossians 4:6"));
    }
}

// ============================================================================
// END CLOSING
// ============================================================================
