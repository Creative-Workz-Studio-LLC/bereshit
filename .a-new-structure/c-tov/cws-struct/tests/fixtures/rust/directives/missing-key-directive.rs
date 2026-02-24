// #!omni code --rust -library
//omni:code --rust -library
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Purpose: Fixture — missing //omni:key directive (triggers R50-011).
//          Has other directives and block structure but NO //omni:key.
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.format", "rust"),
    ("I1.from", "tests/fixtures/rust/directives/missing-key-directive.rs"),
    ("I1.at", "a-01.00"),
    ("I2.type", "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype", "library"),
    ("I3.file", "missing-key-directive.rs"),
    ("I3.title", "Missing Key Directive Fixture"),
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
// This file tests directive/omni:key/required detection (R50-011).
//
// ============================================================================
