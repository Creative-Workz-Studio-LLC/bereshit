//omni:key B-sdk-engine-mapper
//omni:code --rust -module
//omni:version a-01.00

//! JSON Mapper

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-sdk-engine-mapper
// Purpose: Applies map_in and map_out transformations
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key", "B-sdk-engine-mapper"),
    ("I1.format", "rust"),
    ("I1.from", "src/engine/mapper.rs"),
    ("I1.at", "a-01.00"),
    ("I2.type", "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype", "module"),
    ("I2.role", "[role]"),
    ("I3.file", "mapper.rs"),
    ("I3.title", "JSON Mapper"),
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
    ("C5.purpose", "Applies map_in and map_out transformations"),
    ("C5.philosophy", "[philosophy]"),
    ("C6.current", "a-01.00 — Applies map_in and map_out transformations"),
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

use crate::schema::SubstrateMap;
use crate::error::{ToolError, ErrorCode};
use std::collections::HashMap;

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

/// The primary translation engine that maps between raw JSON and Universal Events.
pub struct MapperEngine {
    /// Loaded substrate maps, keyed by substrate name (e.g., "gemini", "claude").
    maps: HashMap<String, SubstrateMap>,
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

/// Returns this module's OmniCode pragma identity (I1-I4).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Looks up a specific pragma key in this module.
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this module's OmniCode metadata (C1-C7).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Looks up a specific metadata key in this module.
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

impl MapperEngine {
    /// Creates a new, empty mapping engine.
    pub fn new() -> Self {
        Self {
            maps: HashMap::new(),
        }
    }

    /// Loads a parsed SubstrateMap into the engine.
    pub fn register_map(&mut self, map: SubstrateMap) {
        self.maps.insert(map.substrate.name.clone(), map);
    }

    /// Loads a TOML substrate map from the given file path.
    pub fn load_from_file(&mut self, path: &str) -> Result<(), ToolError> {
        let content = std::fs::read_to_string(path).map_err(|e| {
            ToolError::new(ErrorCode::MapNotFound)
                .with_context("path", path)
                .with_context("details", &e.to_string())
        })?;

        let map: SubstrateMap = toml::from_str(&content).map_err(|e| {
            ToolError::new(ErrorCode::MapParseError)
                .with_context("details", &e.to_string())
        })?;

        self.register_map(map);
        Ok(())
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

impl MapperEngine {
    /// Processes an incoming raw JSON payload from a specific substrate and event.
    /// Returns the mapped Universal Event as a JSON string.
    pub fn process_event(&self, substrate: &str, event_key: &str, raw_json: &str) -> Result<String, ToolError> {
        let map = self.maps.get(substrate).ok_or_else(|| {
            ToolError::new(ErrorCode::UnknownSubstrate)
                .with_context("substrate", substrate)
        })?;

        let event_map = map.events.get(event_key).ok_or_else(|| {
            ToolError::new(ErrorCode::EventNotDefined)
                .with_context("event", event_key)
                .with_context("substrate", substrate)
        })?;

        let payload: serde_json::Value = serde_json::from_str(raw_json).map_err(|e| {
            ToolError::new(ErrorCode::PayloadParseError)
                .with_context("details", &e.to_string())
        })?;

        // --- MAP IN: Extract fields into Universal Event structure ---
        let mut universal = serde_json::Map::new();
        universal.insert("universal_event".to_string(), serde_json::Value::String(event_map.universal_event.clone()));
        
        // Helper to extract via simple path (only supports $.field for now)
        let extract = |path: &str| -> serde_json::Value {
            if let Some(field) = path.strip_prefix("$.") {
                payload.get(field).cloned().unwrap_or(serde_json::Value::Null)
            } else {
                serde_json::Value::Null
            }
        };

        universal.insert("session_id".to_string(), extract(&event_map.map_in.session_id));
        
        if let Some(rule) = &event_map.map_in.tool_name {
            universal.insert("tool_name".to_string(), extract(rule));
        }
        if let Some(rule) = &event_map.map_in.arguments {
            universal.insert("arguments".to_string(), extract(rule));
        }
        if let Some(rule) = &event_map.map_in.prompt {
            universal.insert("prompt".to_string(), extract(rule));
        }

        // Return the universal event for backend processing
        Ok(serde_json::to_string(&serde_json::Value::Object(universal)).unwrap())
    }

    /// Renders a specific output variant for a substrate event using the Universal Result.
    pub fn render_output(
        &self, 
        substrate: &str, 
        event_key: &str, 
        variant: &str, 
        context: &HashMap<String, String>
    ) -> Result<String, ToolError> {
        let map = self.maps.get(substrate).ok_or_else(|| {
            ToolError::new(ErrorCode::UnknownSubstrate)
                .with_context("substrate", substrate)
        })?;

        let event_map = map.events.get(event_key).ok_or_else(|| {
            ToolError::new(ErrorCode::EventNotDefined)
                .with_context("event", event_key)
                .with_context("substrate", substrate)
        })?;

        let template = event_map.map_out.get(variant).ok_or_else(|| {
            ToolError::new(ErrorCode::MappingError)
                .with_context("details", &format!("Output variant '{}' not defined for event '{}'", variant, event_key))
        })?;

        // Render template with proper JSON escaping for substituted values
        let mut rendered = template.clone();
        for (k, v) in context {
            // Escape the value for use in a JSON string
            let escaped_v = serde_json::to_string(v).unwrap_or_else(|_| format!("\"{}\"", v));
            // Remove the surrounding quotes added by serde_json::to_string
            let inner_v = if escaped_v.len() >= 2 {
                &escaped_v[1..escaped_v.len()-1]
            } else {
                &escaped_v
            };

            let p1 = format!("{{{{.{}}}}}", k);
            let p2 = format!("{{{{{}}}}}", k);
            rendered = rendered.replace(&p1, inner_v).replace(&p2, inner_v);
        }

        Ok(rendered)
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_process_event_basic_mapping() {
        let mut engine = MapperEngine::new();
        let toml_str = r#"
            [substrate]
            name = "test"
            engine_version = "v1"
            [events.pre_tool]
            universal_event = "PreToolUse"
            [events.pre_tool.map_in]
            session_id = "$.sid"
            tool_name = "$.tname"
            [events.pre_tool.map_out]
            allow = "{}"
        "#;
        let map: crate::schema::SubstrateMap = toml::from_str(toml_str).unwrap();
        engine.register_map(map);

        let raw_json = r#"{"sid": "session-123", "tname": "run_shell"}"#;
        let result_json = engine.process_event("test", "pre_tool", raw_json).unwrap();
        
        let result: serde_json::Value = serde_json::from_str(&result_json).unwrap();
        assert_eq!(result["universal_event"], "PreToolUse");
        assert_eq!(result["session_id"], "session-123");
        assert_eq!(result["tool_name"], "run_shell");
    }

    #[test]
    fn test_render_output_substitution() {
        let mut engine = MapperEngine::new();
        let toml_str = r#"
            [substrate]
            name = "gemini"
            engine_version = "v1"
            [events.pre_tool]
            universal_event = "PreToolUse"
            [events.pre_tool.map_in]
            session_id = "$.sid"
            [events.pre_tool.map_out]
            deny = '{"decision": "deny", "reason": "{{.reason}}"}'
        "#;
        let map: crate::schema::SubstrateMap = toml::from_str(toml_str).unwrap();
        engine.register_map(map);

        let mut context = HashMap::new();
        context.insert("reason".to_string(), "Dangerous command".to_string());

        let rendered = engine.render_output("gemini", "pre_tool", "deny", &context).unwrap();
        assert_eq!(rendered, r#"{"decision": "deny", "reason": "Dangerous command"}"#);
    }

    #[test]
    fn test_render_output_escaping() {
        let mut engine = MapperEngine::new();
        let toml_str = r#"
            [substrate]
            name = "gemini"
            engine_version = "v1"
            [events.pre_tool]
            universal_event = "PreToolUse"
            [events.pre_tool.map_in]
            session_id = "$.sid"
            [events.pre_tool.map_out]
            deny = '{"reason": "{{.reason}}"}'
        "#;
        let map: crate::schema::SubstrateMap = toml::from_str(toml_str).unwrap();
        engine.register_map(map);

        let mut context = HashMap::new();
        context.insert("reason".to_string(), "Line 1\nLine 2 with \"quotes\"".to_string());

        let rendered = engine.render_output("gemini", "pre_tool", "deny", &context).unwrap();
        // The result should be valid JSON
        let parsed: serde_json::Value = serde_json::from_str(&rendered).expect("Rendered output must be valid JSON");
        assert_eq!(parsed["reason"], "Line 1\nLine 2 with \"quotes\"");
    }

    #[test]
    fn test_process_event_unknown_substrate() {
        let engine = MapperEngine::new();
        let result = engine.process_event("ghost", "start", "{}");
        assert!(result.is_err());
        assert_eq!(result.unwrap_err().code, ErrorCode::UnknownSubstrate);
    }

    #[test]
    fn test_identity_accessors() {
        assert_eq!(pragma_get("I1.key"), Some("B-sdk-engine-mapper"));
        assert_eq!(metadata_get("C2.organization"), Some("CreativeWorkzStudio LLC"));
        assert!(pragma().len() > 0);
        assert!(metadata().len() > 0);
    }

    #[test]
    fn test_identity_completeness() {
        for prefix in ["I1.", "I2.", "I3.", "I4."] {
            assert!(PRAGMA.iter().any(|(k, _)| k.starts_with(prefix)));
        }
        for prefix in ["C1.", "C2.", "C3.", "C4.", "C5.", "C6.", "C7."] {
            assert!(METADATA.iter().any(|(k, _)| k.starts_with(prefix)));
        }
    }
}

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
// Note: Applies map_in and map_out transformations
// Scripture: "In the beginning God created the heaven and the earth." — Genesis 1:1

// ============================================================================
// END CLOSING
// ============================================================================
