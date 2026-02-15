//omni:code --rust -library
//omni:key B-L1-omnicode-hybrid-pragma-reader
//omni:version a-01.00

//! OmniCode pragma reader — the first thing read in any OmniCode file.
//!
//! Parses `#!omni` pragma lines from any file format by stripping
//! format-specific comment prefixes (`//`, `#`, `<!-- -->`).
//!
//! No L0 dependencies — parsing is L1's domain.
//!
//! Genesis 1:3 — "And God said, Let there be light."
//! The pragma is the first word spoken — identity before content.

use std::io::{BufRead, BufReader};
use std::path::Path;

use crate::types::{FileType, Format, Modifier, Pragma, PragmaError};

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────

/// The universal OmniCode pragma marker.
pub const PRAGMA_PREFIX: &str = "#!omni";

/// Maximum lines to scan for pragmas at the top of a file.
pub const MAX_PRAGMA_LINES: usize = 20;

/// Maximum allowed length for a single line during pragma scanning (100KB).
/// Prevents DoS via files with extremely long lines — Rust's `read_line` is
/// unbounded by default. Matches Go's `MaxLineLength` constant.
pub const MAX_LINE_LENGTH: usize = 100 * 1024;

// ────────────────────────────────────────────────────────────────
// Comment Prefix Stripping
// ────────────────────────────────────────────────────────────────

/// Strip format-specific comment markers to reveal the bare `#!omni` pragma.
///
/// Supported prefixes (from syntax.omni PRAGMA PLACEMENT):
///   - `.omni` files: bare (no prefix)
///   - `.go`/`.c`/`.rs`: `//` prefix
///   - `.toml`/`.yaml`: `#` prefix
///   - `.md`/`.html`: `<!-- -->` wrapping
///
/// Returns `None` if the line is not a pragma.
///
/// Go equivalent: `func stripCommentPrefix(line string) string`
pub fn strip_comment_prefix(line: &str) -> Option<&str> {
    let trimmed = line.trim();

    // Already bare pragma
    if trimmed.starts_with(PRAGMA_PREFIX) {
        return Some(trimmed);
    }

    // // #!omni ... (Go, C, Rust, AsciiDoc)
    if let Some(rest) = trimmed.strip_prefix("//") {
        let inner = rest.trim_start();
        if inner.starts_with(PRAGMA_PREFIX) {
            return Some(inner);
        }
    }

    // # #!omni ... (TOML, YAML, shell)
    // But NOT #!omni itself (already handled above)
    if trimmed.starts_with('#') && !trimmed.starts_with("#!") {
        let rest = trimmed.strip_prefix('#').unwrap();
        let inner = rest.trim_start();
        if inner.starts_with(PRAGMA_PREFIX) {
            return Some(inner);
        }
    }

    // <!-- #!omni ... --> (Markdown, HTML)
    if let Some(rest) = trimmed.strip_prefix("<!--") {
        let inner = rest.strip_suffix("-->").unwrap_or(rest).trim();
        if inner.starts_with(PRAGMA_PREFIX) {
            return Some(inner);
        }
    }

    None
}

// ────────────────────────────────────────────────────────────────
// Line Parsing
// ────────────────────────────────────────────────────────────────

/// Parse line 1: `#!omni [type] --[format] -[modifier]`
///
/// Go equivalent: `func parseLine1(line string) (Type, Format, Modifier)`
pub fn parse_line1(line: &str) -> (FileType, Format, Modifier) {
    let rest = line.strip_prefix(PRAGMA_PREFIX).unwrap_or(line).trim();
    let parts: Vec<&str> = rest.split_whitespace().collect();

    let mut file_type = FileType::Data; // default
    let mut format = Format::Omni; // default
    let mut modifier = Modifier::None;

    for part in parts {
        if let Some(fmt_str) = part.strip_prefix("--") {
            if let Some(f) = Format::from_str_loose(fmt_str) {
                format = f;
            }
        } else if let Some(mod_str) = part.strip_prefix('-') {
            if let Some(m) = Modifier::from_str_loose(mod_str) {
                modifier = m;
            }
        } else if let Some(t) = FileType::from_str_loose(part) {
            file_type = t;
        }
    }

    (file_type, format, modifier)
}

/// Parse a meta line: `#!omni meta.[field] = [value]`
///
/// Returns `(field, value)` or `None` if not a meta line.
///
/// Go equivalent: `func parseMetaLine(line string) (field, value string)`
pub fn parse_meta_line(line: &str) -> Option<(&str, &str)> {
    let rest = line.strip_prefix(PRAGMA_PREFIX)?.trim();

    let rest = rest.strip_prefix("meta.")?;

    let (field, value) = rest.split_once('=')?;
    Some((field.trim(), value.trim()))
}

// ────────────────────────────────────────────────────────────────
// Public API
// ────────────────────────────────────────────────────────────────

/// Parse pragma lines from a file path.
///
/// Scans up to [`MAX_PRAGMA_LINES`] from the beginning.
///
/// Results are cached by filepath — subsequent calls for the same path
/// return the cached result without disk I/O. Use [`crate::cache::invalidate()`]
/// or [`crate::cache::invalidate_all()`] after file modifications.
///
/// Go equivalent: `func Parse(path string) (*Pragma, error)`
pub fn parse(path: &Path) -> Result<Pragma, PragmaError> {
    // Normalize path to prevent cache misses from different representations
    // of the same file (e.g., "./foo.toml" vs "/abs/path/foo.toml").
    // Fallback to original path if canonicalization fails (file doesn't exist yet).
    let canonical = std::fs::canonicalize(path).unwrap_or_else(|_| path.to_path_buf());

    // Check cache first (read lock — concurrent readers allowed)
    if let Some(cached) = crate::cache::get(&canonical) {
        return Ok(cached);
    }

    // Cache miss — read from disk (use original path for error messages)
    let file = std::fs::File::open(path)?;
    let pragma = parse_reader(&mut BufReader::new(file))?;

    // Store in cache (write lock — exclusive access)
    crate::cache::put(canonical, pragma.clone());

    Ok(pragma)
}

/// Parse pragma lines from a reader.
///
/// Go equivalent: `func ParseReader(r *bufio.Reader) (*Pragma, error)`
pub fn parse_reader(reader: &mut impl BufRead) -> Result<Pragma, PragmaError> {
    let mut pragma = Pragma {
        file_type: FileType::Data,
        format: Format::Omni,
        modifier: Modifier::None,
        key: String::new(),
        from: String::new(),
        at: String::new(),
        raw_lines: Vec::new(),
    };

    let mut line_num = 0;
    let mut found_pragma = false;
    let mut buf = String::new();

    loop {
        buf.clear();
        let bytes_read = reader.read_line(&mut buf)?;
        if bytes_read == 0 {
            break; // EOF
        }

        // Guard against extremely long lines (DoS prevention).
        // Rust's read_line is unbounded; we cap at MAX_LINE_LENGTH.
        if buf.len() > MAX_LINE_LENGTH {
            return Err(PragmaError::LineTooLong {
                line: line_num + 1,
                length: buf.len(),
                max: MAX_LINE_LENGTH,
            });
        }

        line_num += 1;
        if line_num > MAX_PRAGMA_LINES {
            break;
        }

        let stripped = match strip_comment_prefix(&buf) {
            Some(s) => s.to_owned(),
            None => {
                // Non-pragma line — if we already found pragmas, stop scanning.
                // Pragmas must be contiguous at the top.
                if found_pragma {
                    break;
                }
                continue;
            }
        };

        found_pragma = true;
        pragma.raw_lines.push(stripped.clone());

        // First pragma line = type declaration
        if pragma.raw_lines.len() == 1 {
            let (ft, fmt, modi) = parse_line1(&stripped);
            pragma.file_type = ft;
            pragma.format = fmt;
            pragma.modifier = modi;
            continue;
        }

        // Subsequent lines = meta fields
        if let Some((field, value)) = parse_meta_line(&stripped) {
            match field {
                "key" => pragma.key = value.to_owned(),
                "from" => pragma.from = value.to_owned(),
                "at" => pragma.at = value.to_owned(),
                _ => {} // Unknown meta fields silently ignored
            }
        }
    }

    if !found_pragma {
        return Err(PragmaError::NoPragma {
            max_lines: MAX_PRAGMA_LINES,
        });
    }

    Ok(pragma)
}

/// Parse pragma from a string (convenience for testing).
///
/// Go equivalent: `func ParseString(content string) (*Pragma, error)`
pub fn parse_str(content: &str) -> Result<Pragma, PragmaError> {
    parse_reader(&mut BufReader::new(content.as_bytes()))
}

// ────────────────────────────────────────────────────────────────
// Tests
// ────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    // ── strip_comment_prefix ─────────────────────────────────────

    #[test]
    fn test_strip_bare_pragma() {
        assert_eq!(
            strip_comment_prefix("#!omni code --go"),
            Some("#!omni code --go")
        );
    }

    #[test]
    fn test_strip_go_comment() {
        assert_eq!(
            strip_comment_prefix("// #!omni code --go -library"),
            Some("#!omni code --go -library")
        );
    }

    #[test]
    fn test_strip_toml_comment() {
        assert_eq!(
            strip_comment_prefix("# #!omni data --toml"),
            Some("#!omni data --toml")
        );
    }

    #[test]
    fn test_strip_html_comment() {
        assert_eq!(
            strip_comment_prefix("<!-- #!omni documentation --md -->"),
            Some("#!omni documentation --md")
        );
    }

    #[test]
    fn test_strip_non_pragma() {
        assert_eq!(strip_comment_prefix("package main"), None);
        assert_eq!(strip_comment_prefix("// just a comment"), None);
        assert_eq!(strip_comment_prefix("# not a pragma"), None);
        assert_eq!(strip_comment_prefix(""), None);
    }

    #[test]
    fn test_strip_with_whitespace() {
        assert_eq!(
            strip_comment_prefix("   // #!omni code --go   "),
            Some("#!omni code --go")
        );
    }

    // ── parse_line1 ──────────────────────────────────────────────

    #[test]
    fn test_parse_line1_full() {
        let (ft, fmt, m) = parse_line1("#!omni code --go -library");
        assert_eq!(ft, FileType::Code);
        assert_eq!(fmt, Format::Go);
        assert_eq!(m, Modifier::Library);
    }

    #[test]
    fn test_parse_line1_format_only() {
        let (ft, fmt, m) = parse_line1("#!omni data --toml");
        assert_eq!(ft, FileType::Data);
        assert_eq!(fmt, Format::Toml);
        assert_eq!(m, Modifier::None);
    }

    #[test]
    fn test_parse_line1_defaults_to_omni() {
        let (ft, fmt, _) = parse_line1("#!omni code");
        assert_eq!(ft, FileType::Code);
        assert_eq!(fmt, Format::Omni);
    }

    #[test]
    fn test_parse_line1_bare_pragma() {
        let (_, fmt, _) = parse_line1("#!omni");
        assert_eq!(fmt, Format::Omni);
    }

    // ── parse_meta_line ──────────────────────────────────────────

    #[test]
    fn test_parse_meta_key() {
        let result = parse_meta_line("#!omni meta.key = B-L1-test");
        assert_eq!(result, Some(("key", "B-L1-test")));
    }

    #[test]
    fn test_parse_meta_from() {
        let result = parse_meta_line("#!omni meta.from = bereshit/word/omni/syntax.omni");
        assert_eq!(result, Some(("from", "bereshit/word/omni/syntax.omni")));
    }

    #[test]
    fn test_parse_meta_at() {
        let result = parse_meta_line("#!omni meta.at = a-01.00");
        assert_eq!(result, Some(("at", "a-01.00")));
    }

    #[test]
    fn test_parse_meta_non_meta_line() {
        assert!(parse_meta_line("#!omni code --go").is_none());
        assert!(parse_meta_line("not a pragma").is_none());
    }

    // ── parse_str (full pragma) ──────────────────────────────────

    #[test]
    fn test_parse_go_file() {
        let content = "\
// #!omni code --go -library
// #!omni meta.key = B-L1-pragma
// #!omni meta.from = bereshit/word/omni/syntax.omni
// #!omni meta.at = a-01.00

package pragma
";
        let p = parse_str(content).unwrap();
        assert_eq!(p.file_type, FileType::Code);
        assert_eq!(p.format, Format::Go);
        assert_eq!(p.modifier, Modifier::Library);
        assert_eq!(p.key, "B-L1-pragma");
        assert_eq!(p.from, "bereshit/word/omni/syntax.omni");
        assert_eq!(p.at, "a-01.00");
        assert_eq!(p.raw_lines.len(), 4);
    }

    #[test]
    fn test_parse_toml_file() {
        let content = "\
# #!omni data --toml
# #!omni meta.key = B-L0-config
# #!omni meta.at = b-02.00

[section]
key = \"value\"
";
        let p = parse_str(content).unwrap();
        assert_eq!(p.file_type, FileType::Data);
        assert_eq!(p.format, Format::Toml);
        assert_eq!(p.key, "B-L0-config");
        assert_eq!(p.at, "b-02.00");
    }

    #[test]
    fn test_parse_html_file() {
        let content = "\
<!-- #!omni documentation --md -->
<!-- #!omni meta.key = B-doc-readme -->

# README
";
        let p = parse_str(content).unwrap();
        assert_eq!(p.file_type, FileType::Documentation);
        assert_eq!(p.format, Format::Md);
        assert_eq!(p.key, "B-doc-readme");
    }

    #[test]
    fn test_parse_bare_omni_file() {
        let content = "\
#!omni template --omni
#!omni meta.key = B-L0-seed
#!omni meta.at = a-01.00
";
        let p = parse_str(content).unwrap();
        assert_eq!(p.file_type, FileType::Template);
        assert_eq!(p.format, Format::Omni);
        assert_eq!(p.key, "B-L0-seed");
    }

    #[test]
    fn test_parse_no_pragma_error() {
        let content = "package main\n\nfunc main() {}\n";
        let err = parse_str(content).unwrap_err();
        assert!(matches!(err, PragmaError::NoPragma { max_lines: 20 }));
    }

    #[test]
    fn test_parse_contiguous_only() {
        // Pragma lines must be contiguous — non-pragma line between them
        // should stop scanning at the first non-pragma line.
        let content = "\
// #!omni code --go
// not a pragma
// #!omni meta.key = should-not-be-read
";
        let p = parse_str(content).unwrap();
        assert!(p.key.is_empty()); // key should NOT be read
        assert_eq!(p.raw_lines.len(), 1);
    }

    #[test]
    fn test_parse_preserves_raw_lines() {
        let content = "\
// #!omni code --go -library
// #!omni meta.key = test
";
        let p = parse_str(content).unwrap();
        assert_eq!(p.raw_lines.len(), 2);
        assert_eq!(p.raw_lines[0], "#!omni code --go -library");
        assert_eq!(p.raw_lines[1], "#!omni meta.key = test");
    }
}
