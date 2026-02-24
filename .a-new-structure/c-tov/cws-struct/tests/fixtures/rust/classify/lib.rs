//! Derived crate root with leftover template directive.
//! Triggers CWS-R50-042 (derived/template-directive).

// #!omni template --rust -library
//omni:key B-test-derived-template-directive
//omni:code --rust -library
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-derived-template-directive
// Purpose: Derived lib.rs that still has #!omni template line.
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key", "B-test-derived-template-directive"),
    ("I1.format", "rust"),
    ("I1.from", "tests/fixtures/rust/classify/lib.rs"),
    ("I1.at", "a-01.00"),
    ("I2.type", "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype", "library"),
    ("I3.file", "lib.rs"),
    ("I3.title", "Derived Template Directive Fixture"),
];

pub static METADATA: &[(&str, &str)] = &[
    ("C1.version", "a-01.00"),
    ("C1.status", "Active"),
    ("C1.created", "2026-02-23"),
    ("C2.organization", "CreativeWorkzStudio LLC"),
    ("C3.scripture", "Psalm 119:130"),
    ("C4.requires", "none"),
    ("C4.consumers", "tests"),
];

// ============================================================================
// SETUP
// ============================================================================

use std::fmt;

// ============================================================================
// BODY
// ============================================================================

// 4. Core Logic
pub fn process(input: &str) -> String {
    format!("processed: {}", input)
}

// ============================================================================
// CLOSING
// ============================================================================
//
// X1 Policy
// Scripture: Psalm 119:130
//
// X5 Note
// This file tests derived/template-directive detection.
//
// ============================================================================
