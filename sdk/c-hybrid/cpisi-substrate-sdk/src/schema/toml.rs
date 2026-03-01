//omni:key B-sdk-schema-toml
//omni:code --rust -module
//omni:version a-01.00

//! TOML Structs

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-sdk-schema-toml
// Purpose: Serde definitions for mapping rules
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key", "B-sdk-schema-toml"),
    ("I1.format", "rust"),
    ("I1.from", "src/schema/toml.rs"),
    ("I1.at", "a-01.00"),
    ("I2.type", "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype", "module"),
    ("I2.role", "[role]"),
    ("I3.file", "toml.rs"),
    ("I3.title", "TOML Structs"),
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
    ("C5.purpose", "Serde definitions for mapping rules"),
    ("C5.philosophy", "[philosophy]"),
    ("C6.current", "a-01.00 — Serde definitions for mapping rules"),
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

// (What this file depends on — external packages, standard library, internal modules.)

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

// (Error definitions with display and conversion — how this file's failures look.)

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

use serde::{Deserialize, Serialize};
use std::collections::HashMap;

/// The root of a substrate map file (e.g. gemini.toml).
#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct SubstrateMap {
    pub substrate: SubstrateMeta,
    pub events: HashMap<String, EventMap>,
}

/// Metadata about the substrate itself.
#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct SubstrateMeta {
    pub name: String,
    pub engine_version: String,
    pub description: Option<String>,
}

/// A specific event mapping block (e.g. [events.pre_tool]).
#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct EventMap {
    pub universal_event: String,
    pub map_in: MapInRules,
    pub map_out: HashMap<String, String>, // Dynamic keys like 'allow', 'deny', 'success'
}

/// Inbound mapping rules for translating from raw JSON to Universal Event.
#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct MapInRules {
    pub session_id: String,
    pub tool_name: Option<String>,
    pub arguments: Option<String>,
    pub prompt: Option<String>,
    pub source: Option<String>,
    pub transcript_path: Option<String>,
    pub response: Option<String>,
    pub context: Option<HashMap<String, String>>, // Dynamic context mappings
}

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

// (Constructor content — new(), builders, typestate transitions. Types come into existence.)

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
// Note: Serde definitions for mapping rules
// Scripture: "In the beginning God created the heaven and the earth." — Genesis 1:1

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_deserialize_substrate_map() {
        let toml_str = r#"
            [substrate]
            name = "gemini"
            engine_version = "v1"

            [events.pre_tool]
            universal_event = "PreToolUse"
            [events.pre_tool.map_in]
            session_id = "$.session_id"
            tool_name = "$.tool_name"
            [events.pre_tool.map_out]
            allow = '{"decision": "allow"}'
        "#;

        let map: SubstrateMap = toml::from_str(toml_str).unwrap();
        assert_eq!(map.substrate.name, "gemini");
        let pre_tool = map.events.get("pre_tool").unwrap();
        assert_eq!(pre_tool.universal_event, "PreToolUse");
        assert_eq!(pre_tool.map_in.session_id, "$.session_id");
        assert_eq!(pre_tool.map_out.get("allow").unwrap(), "{\"decision\": \"allow\"}");
    }

    #[test]
    fn test_identity_accessors() {
        assert_eq!(pragma_get("I1.key"), Some("B-sdk-schema-toml"));
        assert_eq!(metadata_get("C2.organization"), Some("CreativeWorkzStudio LLC"));
        assert!(pragma().len() > 0);
        assert!(metadata().len() > 0);
    }

    #[test]
    fn test_identity_completeness() {
        for prefix in ["I1.", "I2.", "I3.", "I4."] {
            assert!(PRAGMA.iter().any(|(k, _)| k.starts_with(prefix)));
        }
    }
}

// ============================================================================
// END CLOSING
// ============================================================================
