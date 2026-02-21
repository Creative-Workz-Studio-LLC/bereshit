// #!omni document --rust -module
// #!omni meta.key = B-test-fixture-form-missing-required
// #!omni meta.from = b-word/seed/code/L0/rust/module.rs
// #!omni meta.at = derived
// ============================================================================
// METADATA
// ============================================================================
//
// Test Fixture — Module Missing Required Section
//
// derives_from: b-word/seed/code/L0/rust/module.rs

//! # Module Missing Required Section
//!
//! This module file intentionally omits S1 Imports (required for modules).

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key",       "B-test-fixture-form-missing-required"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/module.rs"),
    ("I1.at",        "derived"),
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "module"),
    ("I2.role",      "seed"),
    ("I3.file",      "module-missing-required.rs"),
    ("I3.title",     "Module Missing Required Section"),
    ("I3.component", "Test fixture"),
    ("I3.path",      "tests/fixtures/rust/form/module-missing-required.rs"),
    ("I3.provides",  "TEST_FORM_MISSING_REQUIRED"),
    ("I3.brief",     "Module missing S1 Imports for form testing."),
    ("I4.layer",     "L0"),
    ("I4.position",  "seed"),
    ("I4.pattern",   "test fixture"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

pub static METADATA: &[(&str, &str)] = &[
    ("C1.version",           "a-01.00"),
    ("C1.status",            "Draft"),
    ("C1.created",           "2026-02-20"),
    ("C1.updated",           "2026-02-20"),
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Nova Dawn"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    ("C3.scripture",         "1 Corinthians 12:18"),
    ("C3.principle",         "Members in the body, each with their function."),
    ("C3.anchor",            "Genesis 1:1"),
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none"),
    ("C4.consumers",         "Rust linter test suite"),
    ("C4.integration",       "test fixture — not compiled"),
    ("C4.if_missing",        "Cannot test form/required-section-missing"),
    ("C5.purpose",           "Test fixture: module missing required S1 Imports"),
    ("C5.philosophy",        "Prove form constraints catch missing required sections"),
    ("C6.current",           "a-01.00 — Initial fixture"),
    ("C6.planned",           "none"),
    ("C6.limitations",       "Test fixture only — not compiled"),
    ("C7.tags",              "test, fixture, rust, module, form"),
    ("C7.category",          "Testing"),
    ("C7.domain",            "bereshit"),
    ("C7.paradigm",          "CPI-SI"),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// NOTE: S1 Imports is intentionally ABSENT — this tests form/required-section-missing

// ──────────────────────────────────────────────────────────────────────────
// Core Types
// ──────────────────────────────────────────────────────────────────────────

struct InternalType {
    value: u32,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// (minimal — test fixture)

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Module missing required section — should trigger form/required-section-missing.
//
// ============================================================================
// END CLOSING
// ============================================================================
