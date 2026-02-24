//omni:key B-test-body-section-violations
//omni:code --rust -library
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-body-section-violations
// Purpose: Test fixture — BODY with missing required sections and reserved
//          section with code. Library form requires 7 BODY sections.
//          Only CoreLogic present → 6 missing → CWS-R05-020 (×6).
//          "Helpers" is reserved for library → CWS-R05-021.
//
// Version: a-01.00
// Biblical: Psalm 119:130

//! # Body Section Violations Fixture
//!
//! Library with missing required BODY sections and reserved section content.

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key",       "B-test-body-section-violations"),
    ("I1.format",    "rust"),
    ("I1.from",      "tests/fixtures/rust/content/body-section-violations.rs"),
    ("I1.at",        "a-01.00"),
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I3.file",      "body-section-violations.rs"),
    ("I3.title",     "Body Section Violations"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

pub static METADATA: &[(&str, &str)] = &[
    ("C1.version",      "a-01.00"),
    ("C1.status",       "Active"),
    ("C1.created",      "2026-02-23"),
    ("C2.organization", "CreativeWorkzStudio LLC"),
    ("C3.scripture",    "Psalm 119:130"),
    ("C4.requires.std", "none"),
    ("C4.consumers",    "tests"),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// Imports
// ──────────────────────────────────────────────────────────────────────────

use std::fmt;

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

pub fn process(input: &str) -> String {
    format!("processed: {}", input)
}

pub fn validate(data: &str) -> bool {
    !data.is_empty()
}

// ──────────────────────────────────────────────────────────────────────────
// 8. Helpers
// ──────────────────────────────────────────────────────────────────────────

// NOTE: "Helpers" is RESERVED for library form. This section should NOT
// have code — the presence of code triggers CWS-R05-021.
fn internal_helper(s: &str) -> usize {
    s.len()
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// X1 — Change Log
//
// safe: Test fixture creation
// careful: None
//
// X5 — Scripture and Grounding
//
// scripture: Psalm 119:130
//
// ============================================================================
// END CLOSING
// ============================================================================
