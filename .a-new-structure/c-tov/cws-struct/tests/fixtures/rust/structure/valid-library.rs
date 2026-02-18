// #!omni template --rust -library
// #!omni meta.key = B-test-fixture-valid-library
// #!omni meta.from = b-word/seed/code/L0/rust/root.omni
// #!omni meta.at = template
// ============================================================================
// METADATA
// ============================================================================
//
// TEMPLATE: Test Fixture — Valid Library
//
// derives_from: b-word/seed/code/L0/rust/root.omni

//! # Valid Library Fixture
//!
//! A complete, well-formed Rust library template with all I/C fields.
//! Used as the happy-path test fixture.

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for this crate.
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-test-fixture-valid-library"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/root.omni"),
    ("I1.at",        "template"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I2.role",      "seed"),
    // I3: Instance
    ("I3.file",      "valid-library.rs"),
    ("I3.title",     "Valid Library Fixture"),
    ("I3.component", "Test fixture for Rust linter"),
    ("I3.path",      "tests/fixtures/rust/valid-library.rs"),
    ("I3.provides",  "TEST_VALID_LIBRARY"),
    ("I3.brief",     "Complete valid library for linter testing."),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "seed"),
    ("I4.pattern",   "test fixture"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context for this crate.
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "a-01.00"),
    ("C1.status",            "draft"),
    ("C1.created",           "2026-02-17"),
    ("C1.updated",           "2026-02-17"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Nova Dawn"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Prove all things; hold fast that which is good. — 1 Thessalonians 5:21"),
    ("C3.principle",         "Tests prove truth. Fixtures embody it."),
    ("C3.anchor",            "Genesis 1:1"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none"),
    ("C4.consumers",         "Rust linter test suite"),
    ("C4.integration",       "test fixture — not compiled"),
    ("C4.if_missing",        "Rust linter tests cannot verify happy path"),
    // C5: Intent
    ("C5.purpose",           "Test fixture: valid library with complete I/C metadata"),
    ("C5.philosophy",        "Ground truth for what a correct file looks like"),
    // C6: Roadmap
    ("C6.current",           "a-01.00 — Initial fixture"),
    ("C6.planned",           "none"),
    ("C6.limitations",       "Test fixture only — not compiled"),
    // C7: Classification
    ("C7.tags",              "test, fixture, rust, library, 4-block"),
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

// ──────────────────────────────────────────────────────────────────────────
// Imports
// ──────────────────────────────────────────────────────────────────────────

// (none — test fixture)

// ──────────────────────────────────────────────────────────────────────────
// Error Types
// ──────────────────────────────────────────────────────────────────────────

// (none — test fixture)

// ──────────────────────────────────────────────────────────────────────────
// Core Types
// ──────────────────────────────────────────────────────────────────────────

// (none — test fixture)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// (empty — test fixture)

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Valid library fixture — all checks should pass with zero errors.
//
// ============================================================================
// END CLOSING
// ============================================================================
