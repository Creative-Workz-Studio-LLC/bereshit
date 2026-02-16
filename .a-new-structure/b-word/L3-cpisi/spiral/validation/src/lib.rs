//omni:code --rust -library
//omni:key B-L3-validation
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L3-validation
// Purpose: CPI-SI data validation helpers
// Biblical: "Prove all things, hold fast that which is good"
//   -- 1 Thessalonians 5:21
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-16
//
// Lightweight validation helpers ported from the Go validation package.
// Provides domain-specific validators for hebrew states, K:ALIGN values,
// health scores, session IDs, key values, and trajectory sections.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};

// ────────────────────────────────────────────────────────────────
// S.1 Constants
// ────────────────────────────────────────────────────────────────

/// The seven valid hebrew states in CPI-SI.
/// These represent spiritual alignment conditions from scripture.
const VALID_HEBREW_STATES: &[&str] = &[
    "shavar",  // broken
    "chaser",  // lacking
    "ratsah",  // accepted / pleasing
    "yashar",  // upright
    "tamim",   // complete / blameless
    "shalem",  // whole / at peace
    "tov",     // good
];

/// Maximum reasonable length for a session ID.
const SESSION_ID_MAX_LEN: usize = 256;

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 ValidationResult
// ────────────────────────────────────────────────────────────────

/// Accumulates errors and warnings from validation checks.
#[derive(Clone, Debug, Default, Serialize, Deserialize)]
pub struct ValidationResult {
    /// Validation errors (any present means invalid)
    pub errors: Vec<String>,
    /// Non-critical warnings
    pub warnings: Vec<String>,
}

impl ValidationResult {
    /// Create a new empty result (initially valid).
    pub fn new() -> Self {
        Self {
            errors: Vec::new(),
            warnings: Vec::new(),
        }
    }

    /// Whether validation passed (no errors).
    pub fn is_valid(&self) -> bool {
        self.errors.is_empty()
    }

    /// Add an error message. Any error makes the result invalid.
    pub fn add_error(&mut self, msg: impl Into<String>) {
        self.errors.push(msg.into());
    }

    /// Add a warning message. Warnings do not affect validity.
    pub fn add_warning(&mut self, msg: impl Into<String>) {
        self.warnings.push(msg.into());
    }

    /// Number of errors accumulated.
    pub fn error_count(&self) -> usize {
        self.errors.len()
    }

    /// Number of warnings accumulated.
    pub fn warning_count(&self) -> usize {
        self.warnings.len()
    }

    /// Merge another result into this one (collects all errors and warnings).
    pub fn merge(&mut self, other: &ValidationResult) {
        self.errors.extend(other.errors.iter().cloned());
        self.warnings.extend(other.warnings.iter().cloned());
    }
}

impl std::fmt::Display for ValidationResult {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        if self.is_valid() {
            write!(f, "valid")?;
        } else {
            write!(f, "invalid ({} error(s))", self.error_count())?;
            for err in &self.errors {
                write!(f, "\n  - {err}")?;
            }
        }
        if !self.warnings.is_empty() {
            write!(f, "\n  warnings ({}):", self.warning_count())?;
            for warn in &self.warnings {
                write!(f, "\n  ~ {warn}")?;
            }
        }
        Ok(())
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Domain Validators
// ────────────────────────────────────────────────────────────────

/// Validate a hebrew state string against the 7 known states.
///
/// Valid states: shavar, chaser, ratsah, yashar, tamim, shalem, tov
pub fn validate_hebrew_state(state: &str) -> bool {
    VALID_HEBREW_STATES.contains(&state)
}

/// Validate K:ALIGN value is within the moral compass range.
///
/// K must be in [-1.0, 1.0] inclusive.
pub fn validate_k_align(k: f64) -> bool {
    (-1.0..=1.0).contains(&k)
}

/// Validate a health score is within the ternary range.
///
/// Score must be in [-100.0, 100.0] inclusive.
pub fn validate_health_score(score: f64) -> bool {
    (-100.0..=100.0).contains(&score)
}

/// Validate a session ID is non-empty and of reasonable length.
///
/// Must be non-empty, trimmed, and no longer than 256 characters.
pub fn validate_session_id(id: &str) -> bool {
    let trimmed = id.trim();
    !trimmed.is_empty() && trimmed.len() <= SESSION_ID_MAX_LEN
}

/// Validate a ternary key value (-1, 0, or 1).
///
/// These represent the fundamental ternary states: negative, neutral, positive.
pub fn validate_key_value(k: i32) -> bool {
    matches!(k, -1 | 0 | 1)
}

/// Validate a trajectory section identifier.
///
/// Must match the pattern B.1, B.2, B.3, or B.4 (the four body subsections).
pub fn validate_trajectory_section(section: &str) -> bool {
    matches!(section, "B.1" | "B.2" | "B.3" | "B.4")
}

// ────────────────────────────────────────────────────────────────
// B.3 Composite Validation
// ────────────────────────────────────────────────────────────────

/// A named validation check: label + validator function.
pub struct ValidationCheck<'a> {
    /// Human-readable label for the check
    pub label: &'a str,
    /// Validation function returning true if valid
    pub valid: bool,
}

/// Run multiple validation checks and collect results.
///
/// Each check that fails adds an error to the result with its label.
///
/// # Example
///
/// ```
/// use bereshit_l3_validation::{validate_all, ValidationCheck};
///
/// let result = validate_all(&[
///     ValidationCheck { label: "hebrew state", valid: true },
///     ValidationCheck { label: "k-align", valid: false },
/// ]);
/// assert!(!result.is_valid());
/// assert_eq!(result.error_count(), 1);
/// ```
pub fn validate_all(checks: &[ValidationCheck<'_>]) -> ValidationResult {
    let mut result = ValidationResult::new();
    for check in checks {
        if !check.valid {
            result.add_error(format!("{}: validation failed", check.label));
        }
    }
    result
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    // ── Hebrew State Tests ─────────────────────────────────────

    #[test]
    fn hebrew_state_valid_all_seven() {
        for state in VALID_HEBREW_STATES {
            assert!(
                validate_hebrew_state(state),
                "expected '{state}' to be valid"
            );
        }
    }

    #[test]
    fn hebrew_state_invalid_rejects_unknown() {
        assert!(!validate_hebrew_state("invalid"));
        assert!(!validate_hebrew_state(""));
        assert!(!validate_hebrew_state("TOV")); // case-sensitive
        assert!(!validate_hebrew_state("shalom")); // close but not valid
    }

    // ── K:ALIGN Tests ──────────────────────────────────────────

    #[test]
    fn k_align_valid_boundaries() {
        assert!(validate_k_align(-1.0));
        assert!(validate_k_align(0.0));
        assert!(validate_k_align(1.0));
        assert!(validate_k_align(0.5));
        assert!(validate_k_align(-0.5));
    }

    #[test]
    fn k_align_invalid_out_of_range() {
        assert!(!validate_k_align(-1.01));
        assert!(!validate_k_align(1.01));
        assert!(!validate_k_align(100.0));
        assert!(!validate_k_align(-100.0));
    }

    // ── Health Score Tests ──────────────────────────────────────

    #[test]
    fn health_score_valid_range() {
        assert!(validate_health_score(-100.0));
        assert!(validate_health_score(0.0));
        assert!(validate_health_score(100.0));
        assert!(validate_health_score(42.5));
        assert!(validate_health_score(-75.0));
    }

    #[test]
    fn health_score_invalid_out_of_range() {
        assert!(!validate_health_score(-100.1));
        assert!(!validate_health_score(100.1));
        assert!(!validate_health_score(1000.0));
    }

    // ── Session ID Tests ────────────────────────────────────────

    #[test]
    fn session_id_valid() {
        assert!(validate_session_id("abc-123"));
        assert!(validate_session_id("a")); // single char is fine
        assert!(validate_session_id("session-2026-02-16-001"));
    }

    #[test]
    fn session_id_invalid_empty_or_whitespace() {
        assert!(!validate_session_id(""));
        assert!(!validate_session_id("   "));
        assert!(!validate_session_id("\t\n"));
    }

    #[test]
    fn session_id_invalid_too_long() {
        let long_id = "x".repeat(SESSION_ID_MAX_LEN + 1);
        assert!(!validate_session_id(&long_id));
    }

    // ── Key Value Tests ─────────────────────────────────────────

    #[test]
    fn key_value_valid_ternary() {
        assert!(validate_key_value(-1));
        assert!(validate_key_value(0));
        assert!(validate_key_value(1));
    }

    #[test]
    fn key_value_invalid_non_ternary() {
        assert!(!validate_key_value(-2));
        assert!(!validate_key_value(2));
        assert!(!validate_key_value(100));
    }

    // ── Trajectory Section Tests ────────────────────────────────

    #[test]
    fn trajectory_section_valid() {
        assert!(validate_trajectory_section("B.1"));
        assert!(validate_trajectory_section("B.2"));
        assert!(validate_trajectory_section("B.3"));
        assert!(validate_trajectory_section("B.4"));
    }

    #[test]
    fn trajectory_section_invalid() {
        assert!(!validate_trajectory_section("B.0"));
        assert!(!validate_trajectory_section("B.5"));
        assert!(!validate_trajectory_section("A.1"));
        assert!(!validate_trajectory_section(""));
    }

    // ── ValidationResult Tests ──────────────────────────────────

    #[test]
    fn result_starts_valid() {
        let result = ValidationResult::new();
        assert!(result.is_valid());
        assert_eq!(result.error_count(), 0);
        assert_eq!(result.warning_count(), 0);
    }

    #[test]
    fn result_accumulates_errors_and_warnings() {
        let mut result = ValidationResult::new();
        assert!(result.is_valid());

        result.add_warning("minor issue");
        assert!(result.is_valid()); // warnings don't invalidate
        assert_eq!(result.warning_count(), 1);

        result.add_error("bad value");
        assert!(!result.is_valid());
        assert_eq!(result.error_count(), 1);

        result.add_error("another bad value");
        assert_eq!(result.error_count(), 2);
    }

    #[test]
    fn result_merge_combines() {
        let mut a = ValidationResult::new();
        a.add_error("error-a");
        a.add_warning("warn-a");

        let mut b = ValidationResult::new();
        b.add_error("error-b");

        a.merge(&b);
        assert_eq!(a.error_count(), 2);
        assert_eq!(a.warning_count(), 1);
        assert!(!a.is_valid());
    }

    // ── validate_all Tests ──────────────────────────────────────

    #[test]
    fn validate_all_passes_when_all_valid() {
        let result = validate_all(&[
            ValidationCheck { label: "hebrew", valid: true },
            ValidationCheck { label: "k-align", valid: true },
            ValidationCheck { label: "health", valid: true },
        ]);
        assert!(result.is_valid());
        assert_eq!(result.error_count(), 0);
    }

    #[test]
    fn validate_all_collects_failures() {
        let result = validate_all(&[
            ValidationCheck { label: "hebrew state", valid: validate_hebrew_state("tov") },
            ValidationCheck { label: "k-align", valid: validate_k_align(5.0) },
            ValidationCheck { label: "health score", valid: validate_health_score(999.0) },
            ValidationCheck { label: "session id", valid: validate_session_id("ok") },
        ]);
        assert!(!result.is_valid());
        assert_eq!(result.error_count(), 2); // k-align and health score fail
    }
}
