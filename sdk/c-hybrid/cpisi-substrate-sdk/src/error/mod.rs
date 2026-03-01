//omni:key B-sdk-error-mod
//omni:code --rust -module
//omni:version a-01.00

//! SDK Error Types

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-sdk-error-mod
// Purpose: Centralized error handling
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key", "B-sdk-error-mod"),
    ("I1.format", "rust"),
    ("I1.from", "src/error/mod.rs"),
    ("I1.at", "a-01.00"),
    ("I2.type", "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype", "module"),
    ("I2.role", "[role]"),
    ("I3.file", "mod.rs"),
    ("I3.title", "SDK Error Types"),
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
    ("C5.purpose", "Centralized error handling"),
    ("C5.philosophy", "[philosophy]"),
    ("C6.current", "a-01.00 — Centralized error handling"),
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

pub mod catalog;
pub use catalog::*;
use std::collections::HashMap;
use std::fmt;

// ──────────────────────────────────────────────────────────────────────────
// 3. Constants
// ──────────────────────────────────────────────────────────────────────────

// (Compile-time fixed values — known before execution, immutable throughout program lifetime.)

// ──────────────────────────────────────────────────────────────────────────
// 4. Statics
// ──────────────────────────────────────────────────────────────────────────

// (Runtime-initialized fixed values — like constants but require runtime computation.)

// ──────────────────────────────────────────────────────────────────────────
// 5. Type Aliases
// ──────────────────────────────────────────────────────────────────────────

// (Shorthand for complex signatures — vocabulary that makes the rest of the file readable.)

// ──────────────────────────────────────────────────────────────────────────
// 6. Error Types
// ──────────────────────────────────────────────────────────────────────────

/// Structured tool error matching the CWS-T00-xxx architecture.
/// Replaces bare error enums with code-tracked, catalog-backed errors.
#[derive(Debug, Clone)]
pub struct ToolError {
    /// CWS-T00-xxx error code from the catalog.
    pub code: ErrorCode,
    /// Context fields used for message template substitution.
    pub context: HashMap<String, String>,
}

impl std::error::Error for ToolError {}

impl fmt::Display for ToolError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let mut msg = self.code.message_template().to_string();
        for (k, v) in &self.context {
            let placeholder = format!("{{{{{}}}}}", k);
            msg = msg.replace(&placeholder, v);
        }
        write!(f, "[{}] {}", self.code.as_str(), msg)
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

// (Primary data structures and enumerations — the types this file defines.)

// ──────────────────────────────────────────────────────────────────────────
// 8. Trait Defs
// ──────────────────────────────────────────────────────────────────────────

// (Behavioral contracts (shape, not fulfillment) — interfaces/traits declaring capability.)

// ──────────────────────────────────────────────────────────────────────────
// 11. Variables
// ──────────────────────────────────────────────────────────────────────────

// (Package-level mutable state — runtime variables with wider scope.)

// ──────────────────────────────────────────────────────────────────────────
// 12. Interface Defs
// ──────────────────────────────────────────────────────────────────────────

// (Behavioral contracts (shape, not fulfillment) — Go interfaces, implicit contracts.)

// ──────────────────────────────────────────────────────────────────────────
// 13. Type Methods
// ──────────────────────────────────────────────────────────────────────────

// (Structural completers — identity methods, not capability. String(), Display, Debug.)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 0. Org Chart
// ──────────────────────────────────────────────────────────────────────────

// (Module structure overview — package navigation, org chart comment.)

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────

pub fn pragma() -> &'static [(&'static str, &'static str)] { PRAGMA }
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}
pub fn metadata() -> &'static [(&'static str, &'static str)] { METADATA }
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ──────────────────────────────────────────────────────────────────────────
// 2. Trait Implementations
// ──────────────────────────────────────────────────────────────────────────

// (Fulfilling contracts from SETUP — impl blocks for traits/interfaces.)

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors
// ──────────────────────────────────────────────────────────────────────────

impl ToolError {
    /// Creates a new structured ToolError with the given code.
    pub fn new(code: ErrorCode) -> Self {
        Self {
            code,
            context: HashMap::new(),
        }
    }

    /// Adds a context key/value pair for template substitution.
    pub fn with_context(mut self, key: &str, value: &str) -> Self {
        self.context.insert(key.to_string(), value.to_string());
        self
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

// (Primary operations, state transforms — the essential computation this file provides.)

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
// Note: Centralized error handling
// Scripture: "In the beginning God created the heaven and the earth." — Genesis 1:1

#[cfg(test)]
mod tests;

// ============================================================================
// END CLOSING
// ============================================================================
