// #!omni template --rust -executable
// #!omni meta.key = B-test-fixture-valid-executable
// #!omni meta.from = b-word/seed/code/L0/rust/root.omni
// #!omni meta.at = template
// ============================================================================
// METADATA
// ============================================================================
//
// TEMPLATE: Test Fixture — Valid Executable

//! # Valid Executable Fixture
//!
//! Complete executable template with all I/C fields.

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key",       "B-test-fixture-valid-executable"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/root.omni"),
    ("I1.at",        "template"),
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "executable"),
    ("I2.role",      "seed"),
    ("I3.file",      "valid-executable.rs"),
    ("I3.title",     "Valid Executable Fixture"),
    ("I3.component", "Test fixture for Rust linter"),
    ("I3.path",      "tests/fixtures/rust/valid-executable.rs"),
    ("I3.provides",  "TEST_VALID_EXECUTABLE"),
    ("I3.brief",     "Complete valid executable for linter testing."),
    ("I4.layer",     "L0"),
    ("I4.position",  "seed"),
    ("I4.pattern",   "test fixture"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

pub static METADATA: &[(&str, &str)] = &[
    ("C1.version",           "a-01.00"),
    ("C1.status",            "draft"),
    ("C1.created",           "2026-02-17"),
    ("C1.updated",           "2026-02-17"),
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Nova Dawn"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    ("C3.scripture",         "Prove all things — 1 Thessalonians 5:21"),
    ("C3.principle",         "Tests prove truth."),
    ("C3.anchor",            "Genesis 1:1"),
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none"),
    ("C4.consumers",         "Rust linter test suite"),
    ("C4.integration",       "test fixture"),
    ("C4.if_missing",        "Rust linter tests incomplete"),
    ("C5.purpose",           "Test fixture: valid executable"),
    ("C5.philosophy",        "Ground truth for correct file"),
    ("C6.current",           "a-01.00"),
    ("C6.planned",           "none"),
    ("C6.limitations",       "Test fixture only"),
    ("C7.tags",              "test, fixture, rust, executable"),
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
// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
// ============================================================================
