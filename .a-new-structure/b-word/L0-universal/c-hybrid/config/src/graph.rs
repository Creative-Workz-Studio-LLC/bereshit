//! Dependency graph — DAG construction and cycle detection.

use std::collections::{BTreeMap, BTreeSet};

use crate::error::ConfigError;
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
                    kind: "missing".to_owned(),
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
                kind: "cycle".to_owned(),
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
