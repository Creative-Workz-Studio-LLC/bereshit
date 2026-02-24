// #!omni code --rust -module->utility
//! Recursive function fixture — R[3] scope detection test.
//omni:code --rust -module->utility
//omni:key B-test-fixture-scope-recursive
//omni:version a-01.00

// ============================================================================
// METADATA BLOCK [METADATA]
// ============================================================================
//
// Key:     B-test-fixture-scope-recursive
// Purpose: Fixture for R[3] recursion detection testing.
//

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for this module.
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-test-fixture-scope-recursive"),
    ("I1.format",    "rust"),
    ("I1.from",      "tests/fixtures/rust/scope/recursive-function.rs"),
    ("I1.at",        "a-01.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "module"),
    ("I2.purpose",   "utility"),
    ("I2.role",      "[role]"),
    // I3: Instance
    ("I3.file",      "recursive-function.rs"),
    ("I3.title",     "Recursive function fixture"),
    ("I3.component", "scope-test"),
    ("I3.path",      "tests/fixtures/rust/scope/recursive-function.rs"),
    ("I3.provides",  "[provides]"),
    ("I3.brief",     "Fixture for R[3] recursion detection testing."),
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
    ("C1.version",           "a-01.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-23"),
    ("C1.updated",           "2026-02-23"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn (CPI-SI)"),
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    ("C3.scripture",         "Genesis 1:1"),
    ("C3.principle",         "[principle]"),
    ("C3.anchor",            "Genesis 1:1"),
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "[requires_external]"),
    ("C4.requires.internal", "[requires_internal]"),
    ("C4.consumers",         "[consumers]"),
    ("C4.integration",       "[integration]"),
    ("C4.if_missing",        "[if_missing]"),
    ("C5.purpose",           "Fixture for R[3] recursion detection testing."),
    ("C5.philosophy",        "[philosophy]"),
    ("C6.current",           "a-01.00 — R[3] test fixture."),
    ("C6.planned",           "[planned]"),
    ("C6.limitations",       "[limitations]"),
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

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use std::collections::BTreeSet;

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

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────

/// Walk a tree structure depth-first.
pub fn walk_tree(node: &str, children: &[(String, Vec<String>)], visited: &mut BTreeSet<String>) {
    if visited.contains(node) {
        return;
    }
    visited.insert(node.to_owned());

    for (parent, kids) in children {
        if parent == node {
            for child in kids {
                // Recursive call — R[3] should detect this
                walk_tree(child, children, visited);
            }
        }
    }
}

/// Compute factorial recursively.
fn factorial(n: u64) -> u64 {
    if n <= 1 {
        return 1;
    }
    n * factorial(n - 1)
}

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Identity Access, Trait Implementations, Constructors, Core Logic, Queries, Output

// ============================================================================
// END BODY [END]
// ============================================================================

// ============================================================================
// CLOSING BLOCK [CLOSING]
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
// policy: Test fixture — do not modify structure
// scripture: "Proverbs 22:28 (WEB) — Don't move the ancient boundary stone."
// never: Break 4-block structure
// careful: Function signatures
// safe: Function bodies, Comments

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
// note: "R[3] recursion detection test fixture."
// scripture: "Psalm 19:2 — Day unto day uttereth speech."
// anchor: "Recursive functions for scope analysis testing."

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Cv, Ce, Cc, X2, X3, X4

// ============================================================================
// END CLOSING [END]
// ============================================================================
