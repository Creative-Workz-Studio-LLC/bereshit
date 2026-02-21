//! Dependency graph — DAG construction and cycle detection.

//omni:code --rust -library
//omni:key B-L0-hybrid-config-graph
//omni:version b-03.00

use std::collections::{BTreeMap, BTreeSet};

use crate::error::{ConfigError, DepKind};
use crate::types::{DependencyNode, IndexManifest};

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
