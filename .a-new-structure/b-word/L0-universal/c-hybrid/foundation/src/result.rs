//omni:code --rust -library
//omni:key B-L0-foundation-result-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # Result Types (Rust)
//!
//! Universal result types for operation outcomes across all layers.
//!
//! Promoted from L3-cpisi to L0-universal — these are generic wrappers,
//! not CPI-SI specific.
//!
//! # Types
//!
//! - `OpResult` — generic operation result (found/message/data)
//! - `LookupResult` — search result (query/matches/count)
//! - `ValidationResult` — validation outcome (valid/errors/warnings)

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};
use std::fmt;

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────

/// Generic operation result.
///
/// Named `OpResult` to avoid collision with `std::result::Result`.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct OpResult {
    pub found: bool,
    pub message: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub data: Option<serde_json::Value>,
}

/// Lookup/search result with match tracking.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LookupResult {
    pub found: bool,
    pub query: String,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub format: String,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub message: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub matches: Option<serde_json::Value>,
    pub count: usize,
}

/// Validation result with errors and warnings.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ValidationResult {
    pub valid: bool,
    #[serde(default, skip_serializing_if = "Vec::is_empty")]
    pub errors: Vec<String>,
    #[serde(default, skip_serializing_if = "Vec::is_empty")]
    pub warnings: Vec<String>,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub file_path: String,
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 OpResult Constructors
// ────────────────────────────────────────────────────────────────

impl OpResult {
    /// Successful result with optional data.
    pub fn ok(message: impl Into<String>, data: Option<serde_json::Value>) -> Self {
        Self { found: true, message: message.into(), data }
    }

    /// Not-found result.
    pub fn not_found(message: impl Into<String>) -> Self {
        Self { found: false, message: message.into(), data: None }
    }

    /// Error result.
    pub fn error(message: impl Into<String>) -> Self {
        Self { found: false, message: message.into(), data: None }
    }
}

impl fmt::Display for OpResult {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let status = if self.found { "OK" } else { "FAIL" };
        write!(f, "[{status}] {}", self.message)
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 LookupResult Constructors
// ────────────────────────────────────────────────────────────────

impl LookupResult {
    /// Successful lookup with matches.
    pub fn found(query: impl Into<String>, matches: serde_json::Value, count: usize) -> Self {
        Self {
            found: true,
            query: query.into(),
            format: String::new(),
            message: String::new(),
            matches: Some(matches),
            count,
        }
    }

    /// Empty lookup — no matches.
    pub fn no_match(query: impl Into<String>, format: impl Into<String>) -> Self {
        Self {
            found: false,
            query: query.into(),
            format: format.into(),
            message: "No matches found".into(),
            matches: None,
            count: 0,
        }
    }
}

impl fmt::Display for LookupResult {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if self.found {
            write!(f, "Found {} match(es) for '{}'", self.count, self.query)
        } else {
            write!(f, "No matches for '{}'", self.query)
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 ValidationResult Constructors + Methods
// ────────────────────────────────────────────────────────────────

impl ValidationResult {
    /// Passing validation.
    pub fn valid(file_path: impl Into<String>) -> Self {
        Self {
            valid: true,
            errors: Vec::new(),
            warnings: Vec::new(),
            file_path: file_path.into(),
        }
    }

    /// Failing validation with errors.
    pub fn invalid(file_path: impl Into<String>, errors: Vec<String>) -> Self {
        Self {
            valid: false,
            errors,
            warnings: Vec::new(),
            file_path: file_path.into(),
        }
    }

    /// Add an error (sets valid = false).
    pub fn add_error(&mut self, err: impl Into<String>) {
        self.errors.push(err.into());
        self.valid = false;
    }

    /// Add a warning (does not affect validity).
    pub fn add_warning(&mut self, warn: impl Into<String>) {
        self.warnings.push(warn.into());
    }

    /// True if there are errors.
    pub fn has_errors(&self) -> bool {
        !self.errors.is_empty()
    }

    /// True if there are warnings.
    pub fn has_warnings(&self) -> bool {
        !self.warnings.is_empty()
    }
}

impl fmt::Display for ValidationResult {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let status = if self.valid { "VALID" } else { "INVALID" };
        write!(f, "[{status}] {} ({} errors, {} warnings)",
            self.file_path, self.errors.len(), self.warnings.len())
    }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Universal result types — promoted from L3 to serve all layers.
// "Commit thy works unto the LORD" — Proverbs 16:3

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn op_result_ok() {
        let r = OpResult::ok("success", None);
        assert!(r.found);
        assert_eq!(r.message, "success");
    }

    #[test]
    fn op_result_not_found() {
        let r = OpResult::not_found("missing");
        assert!(!r.found);
    }

    #[test]
    fn lookup_found() {
        let r = LookupResult::found("test", serde_json::json!([1, 2]), 2);
        assert!(r.found);
        assert_eq!(r.count, 2);
    }

    #[test]
    fn lookup_no_match() {
        let r = LookupResult::no_match("test", "json");
        assert!(!r.found);
        assert_eq!(r.count, 0);
    }

    #[test]
    fn validation_valid() {
        let v = ValidationResult::valid("test.go");
        assert!(v.valid);
        assert!(!v.has_errors());
    }

    #[test]
    fn validation_add_error() {
        let mut v = ValidationResult::valid("test.go");
        v.add_error("something broke");
        assert!(!v.valid);
        assert!(v.has_errors());
        assert_eq!(v.errors.len(), 1);
    }

    #[test]
    fn validation_add_warning() {
        let mut v = ValidationResult::valid("test.go");
        v.add_warning("heads up");
        assert!(v.valid); // warnings don't invalidate
        assert!(v.has_warnings());
    }
}
