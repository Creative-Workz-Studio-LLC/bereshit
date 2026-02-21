// #!omni template --rust -module
// #!omni meta.key = B-test-fixture-form-template-reserved
// #!omni meta.from = b-word/seed/code/L0/rust/module.rs
// #!omni meta.at = template
// ============================================================================
// METADATA
// ============================================================================
//
// TEMPLATE: Test Fixture — Template With Reserved Section
//
// derives_from: b-word/seed/code/L0/rust/module.rs

//! # Template With Reserved Section
//!
//! A module TEMPLATE that wrongly includes S2 Modules (reserved).
//! Templates are "live" — reserved checks should still fire.

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key",       "B-test-fixture-form-template-reserved"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/module.rs"),
    ("I1.at",        "template"),
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "module"),
    ("I2.role",      "seed"),
    ("I3.file",      "template-with-reserved.rs"),
    ("I3.title",     "Template With Reserved Section"),
    ("I3.component", "Test fixture"),
    ("I3.path",      "tests/fixtures/rust/form/template-with-reserved.rs"),
    ("I3.provides",  "TEST_FORM_TEMPLATE_RESERVED"),
    ("I3.brief",     "Module template with reserved S2 — tests live template checking."),
    ("I4.layer",     "L0"),
    ("I4.position",  "seed"),
    ("I4.pattern",   "test fixture"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

pub static METADATA: &[(&str, &str)] = &[
    ("C1.version",           "a-01.00"),
    ("C1.status",            "Template"),
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
    ("C4.if_missing",        "Cannot test form/reserved-section-present on templates"),
    ("C5.purpose",           "Test fixture: template with reserved S2 section"),
    ("C5.philosophy",        "Prove reserved checks fire on templates (templates are live)"),
    ("C6.current",           "a-01.00 — Initial fixture"),
    ("C6.planned",           "none"),
    ("C6.limitations",       "Test fixture only — not compiled"),
    ("C7.tags",              "test, fixture, rust, module, form, template"),
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

use super::shared::SharedType;

// ──────────────────────────────────────────────────────────────────────────
// Modules
// ──────────────────────────────────────────────────────────────────────────

mod submodule;
pub use submodule::SubType;

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
// Template with reserved section — should trigger form/reserved-section-present
// even though it's a template, because templates are live.
//
// ============================================================================
// END CLOSING
// ============================================================================
