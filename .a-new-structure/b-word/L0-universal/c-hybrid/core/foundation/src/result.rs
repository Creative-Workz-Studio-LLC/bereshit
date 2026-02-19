//omni:code --rust -library
//omni:key B-L0-foundation-result-rs
//omni:version b-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-foundation-result-rs
// Purpose: Universal result types for operation outcomes across all layers
// Biblical: Proverbs 16:3 — Commit thy works unto the LORD
// Version: b-02.00

//! # Result Types
//!
//! Universal result types for operation outcomes across all layers.
//!
//! Three types — each models a different kind of outcome:
//!
//! | Type | Models | When |
//! |------|--------|------|
//! | [`OpResult`] | Success/failure with optional payload | Generic operations |
//! | [`LookupResult`] | Search with match tracking | Queries, discovery |
//! | [`ValidationResult`] | Pass/fail with errors and warnings | File/data validation |
//!
//! All three implement `Display` for human-readable output and derive
//! `Serialize`/`Deserialize` for JSON transport.

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use serde::{Deserialize, Serialize};
use std::fmt;

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

/// Generic operation result — did it work, what happened, any data?
///
/// Named `OpResult` to avoid collision with `std::result::Result`.
/// The `data` field carries arbitrary JSON — use when the caller
/// needs unstructured payload. For typed results, use a custom struct.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::OpResult;
///
/// let ok = OpResult::ok("loaded 5 specs");
/// assert!(ok.found);
///
/// let fail = OpResult::not_found("spec 'xyz' not in database");
/// assert!(!fail.found);
/// ```
#[derive(Debug, Clone, Serialize, Deserialize)]
#[derive(Default)]
pub struct OpResult {
    /// Whether the operation succeeded.
    pub found: bool,
    /// Human-readable outcome description.
    pub message: String,
    /// Optional unstructured payload (JSON value).
    #[serde(skip_serializing_if = "Option::is_none")]
    pub data: Option<serde_json::Value>,
}

/// Lookup/search result with match tracking.
///
/// Tracks what was searched for (`query`), what format was checked (`format`),
/// and how many matches were found. The `matches` field carries the actual
/// results as JSON — type-erased for flexibility across different query types.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::LookupResult;
///
/// let found = LookupResult::found("trit", serde_json::json!(["trit_t", "trit_pack"]), 2);
/// assert!(found.found);
/// assert_eq!(found.count, 2);
///
/// let empty = LookupResult::no_match("xyz", "toml");
/// assert!(!empty.found);
/// ```
#[derive(Debug, Clone, Serialize, Deserialize)]
#[derive(Default)]
pub struct LookupResult {
    /// Whether any matches were found.
    pub found: bool,
    /// The search query that produced this result.
    pub query: String,
    /// Format being searched (e.g., "toml", "go", "rust").
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub format: String,
    /// Human-readable status message.
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub message: String,
    /// Matched items as JSON (type-erased).
    #[serde(skip_serializing_if = "Option::is_none")]
    pub matches: Option<serde_json::Value>,
    /// Number of matches found.
    pub count: usize,
}

/// Validation result with errors and warnings.
///
/// Used by linters, schema validators, and file alignment checks.
/// Errors make the result invalid; warnings don't.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::ValidationResult;
///
/// let mut v = ValidationResult::valid("config.toml");
/// v.add_warning("missing optional field: C6_roadmap");
/// assert!(v.valid); // warnings don't invalidate
///
/// v.add_error("missing required field: I1_core");
/// assert!(!v.valid); // errors do
/// assert_eq!(v.errors.len(), 1);
/// assert_eq!(v.warnings.len(), 1);
/// ```
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ValidationResult {
    /// Whether validation passed (no errors).
    pub valid: bool,
    /// Blocking issues that make the result invalid.
    #[serde(default, skip_serializing_if = "Vec::is_empty")]
    pub errors: Vec<String>,
    /// Non-blocking issues (informational).
    #[serde(default, skip_serializing_if = "Vec::is_empty")]
    pub warnings: Vec<String>,
    /// Path to the file that was validated.
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub file_path: String,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Subsection order:
//   3. Constructors   — Creating result instances
//   5. Queries        — Predicates and accessors
//   6. Output         — Display formatting

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors
// ──────────────────────────────────────────────────────────────────────────

impl OpResult {
    /// Successful result with a message and no data.
    pub fn ok(message: impl Into<String>) -> Self {
        Self {
            found: true,
            message: message.into(),
            data: None,
        }
    }

    /// Successful result carrying a JSON payload.
    pub fn ok_with_data(message: impl Into<String>, data: serde_json::Value) -> Self {
        Self {
            found: true,
            message: message.into(),
            data: Some(data),
        }
    }

    /// Not-found result — the operation completed but found nothing.
    pub fn not_found(message: impl Into<String>) -> Self {
        Self {
            found: false,
            message: message.into(),
            data: None,
        }
    }

    /// Error result — the operation itself failed.
    ///
    /// Semantically distinct from `not_found`: not_found means "looked,
    /// nothing there"; error means "couldn't even look."
    pub fn error(message: impl Into<String>) -> Self {
        Self {
            found: false,
            message: message.into(),
            data: None,
        }
    }
}


impl LookupResult {
    /// Successful lookup with matches.
    pub fn found(
        query: impl Into<String>,
        matches: serde_json::Value,
        count: usize,
    ) -> Self {
        Self {
            found: true,
            query: query.into(),
            format: String::new(),
            message: String::new(),
            matches: Some(matches),
            count,
        }
    }

    /// Empty lookup — no matches for the given query.
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


impl ValidationResult {
    /// Passing validation — no errors, no warnings.
    pub fn valid(file_path: impl Into<String>) -> Self {
        Self {
            valid: true,
            errors: Vec::new(),
            warnings: Vec::new(),
            file_path: file_path.into(),
        }
    }

    /// Failing validation with initial errors.
    pub fn invalid(file_path: impl Into<String>, errors: Vec<String>) -> Self {
        Self {
            valid: false,
            errors,
            warnings: Vec::new(),
            file_path: file_path.into(),
        }
    }
}

impl Default for ValidationResult {
    fn default() -> Self {
        Self {
            valid: true,
            errors: Vec::new(),
            warnings: Vec::new(),
            file_path: String::new(),
        }
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Queries & Accessors
// ──────────────────────────────────────────────────────────────────────────

impl ValidationResult {
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

    /// Total issue count (errors + warnings).
    pub fn issue_count(&self) -> usize {
        self.errors.len() + self.warnings.len()
    }
}

impl OpResult {
    /// True if the operation succeeded.
    pub fn is_ok(&self) -> bool {
        self.found
    }

    /// True if the operation carries data.
    pub fn has_data(&self) -> bool {
        self.data.is_some()
    }
}

impl LookupResult {
    /// True if any matches were found.
    pub fn has_matches(&self) -> bool {
        self.found && self.count > 0
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 6. Output & Display
// ──────────────────────────────────────────────────────────────────────────

impl fmt::Display for OpResult {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let status = if self.found { "OK" } else { "FAIL" };
        write!(f, "[{status}] {}", self.message)
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

impl fmt::Display for ValidationResult {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let status = if self.valid { "VALID" } else { "INVALID" };
        write!(
            f,
            "[{status}] {} ({} errors, {} warnings)",
            self.file_path,
            self.errors.len(),
            self.warnings.len()
        )
    }
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "Commit thy works unto the LORD, and thy thoughts shall be established."
// — Proverbs 16:3

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    // -- OpResult ---------------------------------------------------------

    #[test]
    fn op_ok_default() {
        let r = OpResult::ok("success");
        assert!(r.found);
        assert!(r.is_ok());
        assert!(!r.has_data());
        assert_eq!(r.message, "success");
    }

    #[test]
    fn op_ok_with_data() {
        let r = OpResult::ok_with_data("found it", serde_json::json!({"key": "value"}));
        assert!(r.found);
        assert!(r.has_data());
    }

    #[test]
    fn op_not_found() {
        let r = OpResult::not_found("missing");
        assert!(!r.found);
        assert!(!r.is_ok());
    }

    #[test]
    fn op_error() {
        let r = OpResult::error("connection failed");
        assert!(!r.found);
        assert_eq!(r.message, "connection failed");
    }

    #[test]
    fn op_default() {
        let r = OpResult::default();
        assert!(!r.found);
        assert!(r.message.is_empty());
        assert!(r.data.is_none());
    }

    #[test]
    fn op_display() {
        assert_eq!(OpResult::ok("done").to_string(), "[OK] done");
        assert_eq!(OpResult::not_found("nope").to_string(), "[FAIL] nope");
    }

    #[test]
    fn op_serde_roundtrip() {
        let original = OpResult::ok_with_data("test", serde_json::json!(42));
        let json = serde_json::to_string(&original).unwrap();
        let restored: OpResult = serde_json::from_str(&json).unwrap();
        assert_eq!(restored.found, original.found);
        assert_eq!(restored.message, original.message);
        assert_eq!(restored.data, original.data);
    }

    #[test]
    fn op_serde_skips_none_data() {
        let r = OpResult::ok("success");
        let json = serde_json::to_string(&r).unwrap();
        assert!(!json.contains(r#""data""#)); // skip_serializing_if works
    }

    // -- LookupResult -----------------------------------------------------

    #[test]
    fn lookup_found() {
        let r = LookupResult::found("test", serde_json::json!([1, 2]), 2);
        assert!(r.found);
        assert!(r.has_matches());
        assert_eq!(r.count, 2);
    }

    #[test]
    fn lookup_no_match() {
        let r = LookupResult::no_match("test", "json");
        assert!(!r.found);
        assert!(!r.has_matches());
        assert_eq!(r.count, 0);
        assert_eq!(r.format, "json");
    }

    #[test]
    fn lookup_default() {
        let r = LookupResult::default();
        assert!(!r.found);
        assert!(r.query.is_empty());
    }

    #[test]
    fn lookup_display() {
        let found = LookupResult::found("abc", serde_json::json!([]), 3);
        assert_eq!(found.to_string(), "Found 3 match(es) for 'abc'");

        let empty = LookupResult::no_match("xyz", "toml");
        assert_eq!(empty.to_string(), "No matches for 'xyz'");
    }

    #[test]
    fn lookup_serde_roundtrip() {
        let original = LookupResult::found("q", serde_json::json!(["a", "b"]), 2);
        let json = serde_json::to_string(&original).unwrap();
        let restored: LookupResult = serde_json::from_str(&json).unwrap();
        assert_eq!(restored.count, 2);
        assert_eq!(restored.query, "q");
    }

    // -- ValidationResult -------------------------------------------------

    #[test]
    fn validation_valid() {
        let v = ValidationResult::valid("test.go");
        assert!(v.valid);
        assert!(!v.has_errors());
        assert!(!v.has_warnings());
        assert_eq!(v.issue_count(), 0);
    }

    #[test]
    fn validation_invalid() {
        let v = ValidationResult::invalid("bad.go", vec!["missing block".into()]);
        assert!(!v.valid);
        assert!(v.has_errors());
        assert_eq!(v.errors.len(), 1);
    }

    #[test]
    fn validation_add_error_invalidates() {
        let mut v = ValidationResult::valid("test.go");
        assert!(v.valid);
        v.add_error("something broke");
        assert!(!v.valid);
        assert!(v.has_errors());
        assert_eq!(v.errors.len(), 1);
    }

    #[test]
    fn validation_add_warning_keeps_valid() {
        let mut v = ValidationResult::valid("test.go");
        v.add_warning("heads up");
        assert!(v.valid); // warnings don't invalidate
        assert!(v.has_warnings());
        assert_eq!(v.issue_count(), 1);
    }

    #[test]
    fn validation_default_is_valid() {
        let v = ValidationResult::default();
        assert!(v.valid);
        assert!(v.file_path.is_empty());
    }

    #[test]
    fn validation_display() {
        let v = ValidationResult::valid("config.toml");
        assert_eq!(v.to_string(), "[VALID] config.toml (0 errors, 0 warnings)");

        let mut v2 = ValidationResult::valid("bad.rs");
        v2.add_error("err1");
        v2.add_warning("warn1");
        assert_eq!(v2.to_string(), "[INVALID] bad.rs (1 errors, 1 warnings)");
    }

    #[test]
    fn validation_serde_roundtrip() {
        let mut original = ValidationResult::valid("test.toml");
        original.add_error("missing field");
        original.add_warning("deprecated field");
        let json = serde_json::to_string(&original).unwrap();
        let restored: ValidationResult = serde_json::from_str(&json).unwrap();
        assert!(!restored.valid);
        assert_eq!(restored.errors.len(), 1);
        assert_eq!(restored.warnings.len(), 1);
    }

    #[test]
    fn validation_serde_skips_empty_vecs() {
        let v = ValidationResult::valid("clean.go");
        let json = serde_json::to_string(&v).unwrap();
        assert!(!json.contains("errors")); // skip_serializing_if works
        assert!(!json.contains("warnings"));
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library module — no entry point.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Serde types — no resources to clean up.

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Don't move the ancient boundary stone." — Proverbs 22:28
//
// Safe to Modify:
//   - Add new query/accessor methods on existing types
//   - Add new constructors (follow existing naming pattern)
//   - Add #[cfg(test)] tests
//
// Modify with Care:
//   - Struct fields — breaks construction, pattern matching, serde
//   - Constructor signatures — breaks all call sites
//   - Display format — consumers may parse output
//
// Never Modify:
//   - Type names (OpResult, LookupResult, ValidationResult)
//   - Core semantics (found = success, valid = no errors)

// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Designed Growth Points:
//   - New accessor methods on any type
//   - From<> impls for common conversions
//   - Into<OpResult> for ergonomic returns
//
// Future Considerations:
//   - Typed OpResult<T> replacing serde_json::Value
//   - Error categorization (severity levels on ValidationResult)
//   - Merge/combine operations for ValidationResult

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// Universal result types — promoted from L3 to serve all layers.
// Three types, three kinds of outcome, one consistent pattern.
//
// "Commit thy works unto the LORD, and thy thoughts shall be established."
// — Proverbs 16:3

// ============================================================================
// END CLOSING
// ============================================================================
