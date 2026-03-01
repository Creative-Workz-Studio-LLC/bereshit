//omni:key B-sdk-error-tests
//omni:code --rust -module
//omni:version a-01.00

//! Error Module Tests

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-sdk-error-tests
// Purpose: Tests for structured ToolError
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key", "B-sdk-error-tests"),
    ("I1.format", "rust"),
    ("I1.from", "src/error/tests/mod.rs"),
    ("I1.at", "a-01.00"),
    ("I2.type", "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype", "module"),
    ("I2.role", "[role]"),
    ("I3.file", "mod.rs"),
    ("I3.title", "Error Module Tests"),
    ("I3.component", "[component]"),
    ("I3.path", "[path]"),
    ("I3.provides", "[provides]"),
    ("I3.brief", "[brief]"),
    ("I4.layer", "[layer]"),
    ("I4.position", "[position]"),
    ("I4.pattern", "[pattern]"),
];

pub static METADATA: &[(&str, &str)] = &[
    ("C1.version", "a-01.00"),
    ("C1.status", "Active"),
    ("C1.created", "2026-02-28"),
    ("C1.updated", "2026-02-28"),
    ("C2.architect", "[architect]"),
    ("C2.implementation", "[implementation]"),
    ("C2.organization", "CreativeWorkzStudio LLC"),
    ("C2.copyright", "CreativeWorkzStudio LLC"),
    ("C3.scripture", "Genesis 1:1"),
    ("C3.principle", "[principle]"),
    ("C3.anchor", "Genesis 1:1"),
    ("C4.requires.stdlib", "none"),
    ("C4.requires.external", "[requires_external]"),
    ("C4.requires.internal", "[requires_internal]"),
    ("C4.consumers", "[consumers]"),
    ("C4.integration", "[integration]"),
    ("C4.if_missing", "[if_missing]"),
    ("C5.purpose", "Tests for structured ToolError"),
    ("C5.philosophy", "[philosophy]"),
    ("C6.current", "a-01.00 — Tests for structured ToolError"),
    ("C6.planned", "[planned]"),
    ("C6.limitations", "[limitations]"),
    ("C7.tags", "[tags]"),
    ("C7.category", "[category]"),
    ("C7.domain", "[domain]"),
    ("C7.paradigm", "[paradigm]"),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use crate::error::{ErrorCode, ToolError};

pub fn pragma() -> &'static [(&'static str, &'static str)] { PRAGMA }
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}
pub fn metadata() -> &'static [(&'static str, &'static str)] { METADATA }
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

#[test]
fn test_tool_error_formatting_with_context() {
    let err = ToolError::new(ErrorCode::MapNotFound)
        .with_context("path", "/tmp/invalid.toml");

    let formatted = format!("{}", err);
    assert_eq!(formatted, "[CWS-T01-001] Substrate map not found: /tmp/invalid.toml");
}

#[test]
fn test_tool_error_formatting_missing_context() {
    let err = ToolError::new(ErrorCode::MapParseError);
    
    // Should retain the placeholder if context is not provided
    let formatted = format!("{}", err);
    assert_eq!(formatted, "[CWS-T01-002] Failed to parse map: {{details}}");
}

#[test]
fn test_identity_accessors() {
    assert_eq!(pragma_get("I1.key"), Some("B-sdk-error-tests"));
    assert_eq!(metadata_get("C2.organization"), Some("CreativeWorkzStudio LLC"));
    assert!(pragma().len() > 0);
    assert!(metadata().len() > 0);
}

#[test]
fn test_identity_completeness() {
    for prefix in ["I1.", "I2.", "I3.", "I4."] {
        assert!(pragma().iter().any(|(k, _)| k.starts_with(prefix)));
    }
    for prefix in ["C1.", "C2.", "C3.", "C4.", "C5.", "C6.", "C7."] {
        assert!(metadata().iter().any(|(k, _)| k.starts_with(prefix)));
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Queries
// ──────────────────────────────────────────────────────────────────────────

// (Read-only observation methods — side-effect-free inspection of state.)

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────

// (Module-level public utilities — functions not attached to a type.)

// ──────────────────────────────────────────────────────────────────────────
// 20. Core Operations
// ──────────────────────────────────────────────────────────────────────────

// (Primary business logic, state transitions — the core operational layer.)

// ──────────────────────────────────────────────────────────────────────────
// 21. Error Handling
// ──────────────────────────────────────────────────────────────────────────

// (Error processing, wrapping, recovery — how this file handles failures.)

// ──────────────────────────────────────────────────────────────────────────
// 22. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// (Exported functions — the public surface. What consumers see and use.)

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────

// (Cv content goes here)

// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────

// (Ce content goes here)

// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────

// (Cc content goes here)

// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
// Never: Break 4-block structure, Remove block boundaries, Remove identity statics
// Careful: Function signatures (breaks callers), Error types (breaks match arms)
// Safe: Function bodies, New functions, Comments, Tests

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
// Note: Tests for structured ToolError
// Scripture: "In the beginning God created the heaven and the earth." — Genesis 1:1

// ============================================================================
// END CLOSING
// ============================================================================
