// #!omni template --rust -module
// #!omni meta.key = B-test-fixture-valid-module
// #!omni meta.from = b-word/seed/code/L0/rust/module.rs
// #!omni meta.at = template
// ============================================================================
// METADATA
// ============================================================================
//
// TEMPLATE: Test Fixture — Valid Module
//
// derives_from: b-word/seed/code/L0/rust/module.rs

//! # Valid Module Fixture
//!
//! A complete, well-formed Rust module template with all I/C fields.
//! Used as the happy-path test fixture for module subtype.

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for this module.
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-test-fixture-valid-module"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/module.rs"),
    ("I1.at",        "template"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "module"),
    ("I2.role",      "seed"),
    // I3: Instance
    ("I3.file",      "valid-module.rs"),
    ("I3.title",     "Valid Module Fixture"),
    ("I3.component", "Test fixture for Rust linter"),
    ("I3.path",      "tests/fixtures/rust/valid-module.rs"),
    ("I3.provides",  "TEST_VALID_MODULE"),
    ("I3.brief",     "Complete valid module for linter testing."),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "seed"),
    ("I4.pattern",   "test fixture"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context for this module.
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "a-01.00"),
    ("C1.status",            "Template"),
    ("C1.created",           "2026-02-20"),
    ("C1.updated",           "2026-02-20"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Nova Dawn"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "But now hath God set the members every one of them in the body, as it hath pleased him. — 1 Corinthians 12:18"),
    ("C3.principle",         "Module is a member of the body. Each has its function."),
    ("C3.anchor",            "Genesis 1:1"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none"),
    ("C4.consumers",         "Rust linter test suite"),
    ("C4.integration",       "test fixture — not compiled"),
    ("C4.if_missing",        "Rust linter tests cannot verify module happy path"),
    // C5: Intent
    ("C5.purpose",           "Test fixture: valid module with complete I/C metadata"),
    ("C5.philosophy",        "Ground truth for what a correct module file looks like"),
    // C6: Roadmap
    ("C6.current",           "a-01.00 — Initial fixture"),
    ("C6.planned",           "none"),
    ("C6.limitations",       "Test fixture only — not compiled"),
    // C7: Classification
    ("C7.tags",              "test, fixture, rust, module, 4-block"),
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
// Valid module fixture — all checks should pass with zero errors.
//
// ============================================================================
// END CLOSING
// ============================================================================
