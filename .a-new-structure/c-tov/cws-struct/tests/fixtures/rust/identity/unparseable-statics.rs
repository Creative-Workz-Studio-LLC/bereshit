//omni:key B-test-unparseable-statics
//omni:code --rust -library
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-unparseable-statics
// Purpose: Rust fixture with PRAGMA/METADATA statics that can't parse.
//          Triggers CWS-R25-015 (identity/pragma-parse)
//          and CWS-R25-016 (identity/metadata-parse).
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("nodot_key", "some value"),
    ("another_undotted", "another value"),
];

pub static METADATA: &[(&str, &str)] = &[
    ("no_section_prefix", "just a value"),
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
// This file tests unparseable identity static detection.
//
// ============================================================================
