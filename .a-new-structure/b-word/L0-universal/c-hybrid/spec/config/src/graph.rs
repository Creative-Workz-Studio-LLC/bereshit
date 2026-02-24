// #!omni code --rust -module->utility
//! Dependency graph — DAG construction and cycle detection.
//omni:code --rust -module->utility
//omni:key B-L0-hybrid-config-graph
//omni:version b-03.00

// ============================================================================
// METADATA BLOCK [METADATA]
// ============================================================================
//
// Key:     B-L0-hybrid-config-graph
// Purpose: Dependency graph — DAG construction and cycle detection.
//

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for this module.
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-hybrid-config-graph"),
    ("I1.format",    "rust"),
    ("I1.from",      "[from]"),
    ("I1.at",        "b-03.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "module"),
    ("I2.purpose",   "utility"),
    ("I2.role",      "[role]"),
    // I3: Instance
    ("I3.file",      "graph.rs"),
    ("I3.title",     "Dependency graph"),
    ("I3.component", "graph"),
    ("I3.path",      ".a-new-structure/b-word/L0-universal/c-hybrid/spec/config/src/graph.rs"),
    ("I3.provides",  "[provides]"),
    ("I3.brief",     "Dependency graph — DAG construction and cycle detection."),
    // I4: Architecture
    ("I4.layer",     "[layer]"),
    ("I4.position",  "[position]"),
    ("I4.pattern",   "[pattern]"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context for this module.
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-03.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-24"),
    ("C1.updated",           "2026-02-24"),
    // C2: Attribution
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn (CPI-SI)"),
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Genesis 1:1"),
    ("C3.principle",         "[principle]"),
    ("C3.anchor",            "Genesis 1:1"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "[requires_external]"),
    ("C4.requires.internal", "[requires_internal]"),
    ("C4.consumers",         "[consumers]"),
    ("C4.integration",       "[integration]"),
    ("C4.if_missing",        "[if_missing]"),
    // C5: Intent
    ("C5.purpose",           "Dependency graph — DAG construction and cycle detection."),
    ("C5.philosophy",        "[philosophy]"),
    // C6: Roadmap
    ("C6.current",           "b-03.00 — Dependency graph — DAG construction and cycle detection."),
    ("C6.planned",           "[planned]"),
    ("C6.limitations",       "[limitations]"),
    // C7: Classification
    ("C7.tags",              "[tags]"),
    ("C7.category",          "[category]"),
    ("C7.domain",            "bereshit"),
    ("C7.paradigm",          "CPI-SI"),
];

// ============================================================================
// END METADATA [END]
// ============================================================================

// ============================================================================
// SETUP BLOCK [SETUP]
// ============================================================================
//
// SETUP makes things EXIST. BODY makes things HAPPEN.
//
// Section order (dependency chain — each layer uses only what's above):
//
//     [1] Imports           — What this file depends on
//     [2] Modules           — Submodule declarations
//     [3] Constants         — Compile-time fixed values
//     [4] Statics           — Runtime-initialized fixed values
//     [5] Type Aliases      — Shorthand for complex signatures
//     [6] Error Types       — enum + Display + Error + From impls
//     [7] Core Types        — struct/enum + derives + completing trait impls
//     [8] Trait Defs        — Behavioral contracts (shape, not fulfillment)
//     [9] Macros            — macro_rules! declarations
//     [10] Feature Gates     — Conditional compilation (cfg)

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use std::collections::{BTreeMap, BTreeSet};

use crate::error::{ConfigError, DepKind};
use crate::types::{DependencyNode, IndexManifest};

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Modules, Statics, Type Aliases, Trait Defs, Macros, Feature Gates

// ============================================================================
// END SETUP [END]
// ============================================================================

// ============================================================================
// BODY BLOCK [BODY]
// ============================================================================
//
// BODY makes things HAPPEN. SETUP made things EXIST.
//
// Subsection order follows the type lifecycle — from identity through
// // creation, operation, observation, to output.
//
//     [1] Identity Access   — OmniCode static accessor functions
//     [2] Trait Implementations — Fulfilling contracts from SETUP
//     [3] Constructors
//     [4] Core Logic        — Primary operations, state transforms
//     [5] Queries
//     [6] Output
//     [7] Free Functions    — Module-level public utilities

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────

/// Build a dependency graph from the manifest.
pub(crate) fn build_dependency_graph(manifest: &IndexManifest) -> BTreeMap<String, DependencyNode> {
    let mut graph = BTreeMap::new();
    for system in &manifest.systems {
        for spec in &system.specs {
            let key = format!("{}/{}", system.path, spec.file);
            graph.insert(
                key.clone(),
                DependencyNode {
                    spec: key,
                    depends_on: spec.depends_on.clone(),
                    loaded: false,
                },
            );
        }
    }
    graph
}

/// Validate that all dependencies exist and are acyclic.
pub(crate) fn validate_dependencies(graph: &BTreeMap<String, DependencyNode>) -> Vec<ConfigError> {
    let mut errors = Vec::new();

    // Phase 1: Check all referenced deps exist in the manifest
    for (spec, node) in graph {
        for dep in &node.depends_on {
            if !graph.contains_key(dep) {
                errors.push(ConfigError::Dependency {
                    spec: spec.clone(),
                    dep_spec: dep.clone(),
                    kind: DepKind::Missing,
                    message: "dependency not in manifest".to_owned(),
                });
            }
        }
    }

    // Phase 2: Check for cycles using DFS
    for spec in graph.keys() {
        let mut visited = BTreeSet::new();
        let mut in_stack = BTreeSet::new();
        if has_cycle(spec, graph, &mut visited, &mut in_stack) {
            errors.push(ConfigError::Dependency {
                spec: spec.clone(),
                dep_spec: String::new(),
                kind: DepKind::Cycle,
                message: "circular dependency detected".to_owned(),
            });
            break; // one cycle error is enough
        }
    }

    errors
}

/// DFS cycle detection helper.
fn has_cycle(
    spec: &str,
    graph: &BTreeMap<String, DependencyNode>,
    visited: &mut BTreeSet<String>,
    in_stack: &mut BTreeSet<String>,
) -> bool {
    if in_stack.contains(spec) {
        return true;
    }
    if visited.contains(spec) {
        return false;
    }

    visited.insert(spec.to_owned());
    in_stack.insert(spec.to_owned());

    if let Some(node) = graph.get(spec) {
        for dep in &node.depends_on {
            if has_cycle(dep, graph, visited, in_stack) {
                return true;
            }
        }
    }

    in_stack.remove(spec);
    false
}

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Identity Access, Trait Implementations, Constructors, Core Logic, Queries
//
// Reserved (structural — not used in library form):
//   Helpers, Constructor Tests, Builder Tests, Transition Tests, Query Tests, Identity Tests, Error Tests, Edge Case Tests, Table Driven Tests, Integration Tests — Reserved in composition target.

// ============================================================================
// END BODY [END]
// ============================================================================

// ============================================================================
// CLOSING BLOCK [CLOSING]
// ============================================================================
//
// Closing ensures correctness, documents constraints, and anchors the file.
//
// Section order:
//
//     [Cv] Closing Validation (tests)
//     [Ce] Closing Execution (entry point or absence)
//     [Cc] Closing Cleanup (resource teardown)
//     [X1] Modification Policy
//     [X2] Extension Points
//     [X3] Troubleshooting
//     [X4] Reference
//     [X5] Closing Note

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;
    use crate::error::{ConfigError, DepKind};
    use crate::test_utils::TEST_INDEX;
    use crate::types::IndexManifest;

    #[test]
    fn test_build_dependency_graph() {
        let manifest: IndexManifest = toml::from_str(TEST_INDEX).unwrap();
        let g = build_dependency_graph(&manifest);
        assert_eq!(g.len(), 2);
        assert!(g.contains_key("L0-universal/ladder/foundation/math/ternary.toml"));
        assert!(g.contains_key("L0-universal/ladder/foundation/types/primitives.toml"));
    }

    #[test]
    fn test_validate_dependencies_valid() {
        let manifest: IndexManifest = toml::from_str(TEST_INDEX).unwrap();
        let g = build_dependency_graph(&manifest);
        let errors = validate_dependencies(&g);
        assert!(errors.is_empty(), "expected no errors: {errors:?}");
    }

    #[test]
    fn test_validate_dependencies_missing() {
        let toml_str = r#"
[[systems]]
name = "types"
path = "types"
order = 0
[[systems.specs]]
file = "comp.toml"
key = "comp"
depends_on = ["missing/nonexistent.toml"]
"#;
        let manifest: IndexManifest = toml::from_str(toml_str).unwrap();
        let g = build_dependency_graph(&manifest);
        let errors = validate_dependencies(&g);
        assert_eq!(errors.len(), 1);
        assert!(
            matches!(&errors[0], ConfigError::Dependency { kind, .. } if *kind == DepKind::Missing)
        );
    }

    #[test]
    fn test_validate_dependencies_cycle() {
        let toml_str = r#"
[[systems]]
name = "sys"
path = "sys"
order = 0

[[systems.specs]]
file = "a.toml"
key = "a"
depends_on = ["sys/b.toml"]

[[systems.specs]]
file = "b.toml"
key = "b"
depends_on = ["sys/a.toml"]
"#;
        let manifest: IndexManifest = toml::from_str(toml_str).unwrap();
        let g = build_dependency_graph(&manifest);
        let errors = validate_dependencies(&g);
        assert!(
            errors.iter().any(
                |e| matches!(e, ConfigError::Dependency { kind, .. } if *kind == DepKind::Cycle)
            ),
            "expected cycle error: {errors:?}"
        );
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────

// No entry point — this is a module.

// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
// policy: Modification guidelines
// scripture: "Proverbs 22:28 (WEB) — Don't move the ancient boundary stone."
// never: Break 4-block structure, Remove block boundaries, Remove identity statics
// careful: Function signatures (breaks callers), Error types (breaks match arms)
// safe: Function bodies, New functions, Comments, Tests

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
// related_files: [Cargo.toml]
// validate: cargo test

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
// note: "graph.rs — structural scaffold generated by cws-struct transform."
// scripture: "Psalm 139:7-8 — Whither shall I go from thy spirit?"
// anchor: "TODO: Module purpose statement."

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Cc, X2, X3
//
// Reserved (structural — not used in library form):
//   Test Coverage Summary, X6 — Reserved in composition target.

// ============================================================================
// END CLOSING [END]
// ============================================================================