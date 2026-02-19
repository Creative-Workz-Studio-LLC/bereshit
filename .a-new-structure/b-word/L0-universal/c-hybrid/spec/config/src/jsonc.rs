//! JSONC support — strip comments and trailing commas for clean JSON parsing.

use crate::error::ConfigError;

/// Strip JSONC comments and trailing commas, returning valid JSON.
///
/// Handles single-line (`//`), multi-line (`/* */`) comments, and
/// trailing commas before `}` or `]`. Preserves string contents.
///
/// ```
/// use bereshit_l0_config::strip_jsonc_comments;
///
/// let jsonc = r#"{ "key": "value", // comment
///   "num": 42, /* block */ }"#;
/// let json = strip_jsonc_comments(jsonc);
/// assert!(json.contains("\"key\""));
/// assert!(!json.contains("//"));
/// ```
pub fn strip_jsonc_comments(input: &str) -> String {
    // Pass 1: strip comments (preserving strings)
    let no_comments = strip_comments(input);
    // Pass 2: strip trailing commas (now comments can't confuse lookahead)
    strip_trailing_commas(&no_comments)
}

/// Pass 1: strip single-line and multi-line comments, preserving strings.
fn strip_comments(input: &str) -> String {
    let bytes = input.as_bytes();
    let len = bytes.len();
    let mut out = Vec::with_capacity(len);
    let mut i = 0;

    while i < len {
        // String literal — pass through unchanged
        if bytes[i] == b'"' {
            out.push(bytes[i]);
            i += 1;
            while i < len && bytes[i] != b'"' {
                if bytes[i] == b'\\' && i + 1 < len {
                    out.push(bytes[i]);
                    out.push(bytes[i + 1]);
                    i += 2;
                } else {
                    out.push(bytes[i]);
                    i += 1;
                }
            }
            if i < len {
                out.push(bytes[i]); // closing quote
                i += 1;
            }
            continue;
        }

        // Single-line comment
        if i + 1 < len && bytes[i] == b'/' && bytes[i + 1] == b'/' {
            while i < len && bytes[i] != b'\n' {
                i += 1;
            }
            continue;
        }

        // Multi-line comment
        if i + 1 < len && bytes[i] == b'/' && bytes[i + 1] == b'*' {
            i += 2;
            while i + 1 < len && !(bytes[i] == b'*' && bytes[i + 1] == b'/') {
                i += 1;
            }
            if i + 1 < len {
                i += 2; // skip */
            }
            continue;
        }

        out.push(bytes[i]);
        i += 1;
    }

    // SAFETY: Input is &str (valid UTF-8). Only ASCII bytes are removed
    // (comment markers `//`, `/*`, `*/` are all ASCII). Multi-byte UTF-8
    // sequences are never split, so the output is always valid UTF-8.
    String::from_utf8(out).unwrap_or_else(|e| {
        // Unreachable: algorithm only removes ASCII bytes from valid UTF-8.
        // Graceful degradation rather than panic if invariant somehow breaks.
        String::from_utf8_lossy(&e.into_bytes()).into_owned()
    })
}

/// Pass 2: strip trailing commas before `}` or `]`, preserving strings.
fn strip_trailing_commas(input: &str) -> String {
    let bytes = input.as_bytes();
    let len = bytes.len();
    let mut out = Vec::with_capacity(len);
    let mut i = 0;

    while i < len {
        // String literal — pass through
        if bytes[i] == b'"' {
            out.push(bytes[i]);
            i += 1;
            while i < len && bytes[i] != b'"' {
                if bytes[i] == b'\\' && i + 1 < len {
                    out.push(bytes[i]);
                    out.push(bytes[i + 1]);
                    i += 2;
                } else {
                    out.push(bytes[i]);
                    i += 1;
                }
            }
            if i < len {
                out.push(bytes[i]);
                i += 1;
            }
            continue;
        }

        // Trailing comma: comma followed by optional whitespace then } or ]
        if bytes[i] == b',' {
            let mut j = i + 1;
            while j < len && matches!(bytes[j], b' ' | b'\t' | b'\n' | b'\r') {
                j += 1;
            }
            if j < len && (bytes[j] == b'}' || bytes[j] == b']') {
                i += 1; // skip the comma
                continue;
            }
        }

        out.push(bytes[i]);
        i += 1;
    }

    // SAFETY: Same invariant as strip_comments — only ASCII commas removed.
    String::from_utf8(out).unwrap_or_else(|e| String::from_utf8_lossy(&e.into_bytes()).into_owned())
}

/// Parse JSONC content into a typed value via serde.
pub fn parse_jsonc<T: serde::de::DeserializeOwned>(input: &str) -> Result<T, ConfigError> {
    let clean = strip_jsonc_comments(input);
    serde_json::from_str(&clean).map_err(|e| ConfigError::Load {
        file: String::new(),
        op: "parse_jsonc".to_owned(),
        source: e.to_string(),
    })
}
