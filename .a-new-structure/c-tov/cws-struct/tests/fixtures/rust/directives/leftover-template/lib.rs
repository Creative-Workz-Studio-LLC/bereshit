// #!omni code --rust -library
// #!omni template --rust -library
//omni:key B-test-fixture-leftover-template
//omni:code --rust -library
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Purpose: Fixture — derived file with leftover template directive.
//          Has // #!omni code (derived shebang) AND // #!omni template (leftover).
//          Triggers R50-042 (derived/template-directive).
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key", "B-test-fixture-leftover-template"),
    ("I1.format", "rust"),
    ("I1.from", "tests/fixtures/rust/directives/leftover-template-directive.rs"),
    ("I1.at", "a-01.00"),
    ("I2.type", "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype", "library"),
    ("I3.file", "leftover-template-directive.rs"),
    ("I3.title", "Leftover Template Directive Fixture"),
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
// This file tests derived/template-directive detection (R50-042).
// A derived file should not have // #!omni template — use //omni:code instead.
//
// ============================================================================
