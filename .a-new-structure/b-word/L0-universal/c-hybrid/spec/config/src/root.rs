// #!omni code --rust -module
//omni:key B-L0-hybrid-config-root
//omni:code --rust -module
//omni:version b-03.00
//! Thread-safe root directory guard — set once at startup, checked by all loaders.

// ============================================================================
// METADATA BLOCK [METADATA]
// ============================================================================
//
// Key:     B-L0-hybrid-config-root
// Purpose: Thread-safe root directory guard — set once at startup, checked by all loaders.
//

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity — declares what root.rs IS.
///
/// Four groups: Core (key, format, template, version), Family (type, structure,
/// subtype, role), Instance (file, title, component, path, provides, brief),
/// Architecture (layer, position, pattern). Consumed by tooling for discovery,
/// validation, and cross-referencing across the crate.
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-hybrid-config-root"),
    ("I1.format",    "rust"),
    ("I1.from",      "seed/code/L0/rust/4block-module"),
    ("I1.at",        "b-03.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "module"),
    ("I2.role",      "infrastructure"),
    // I3: Instance
    ("I3.file",      "root.rs"),
    ("I3.title",     "Thread-safe root directory guard"),
    ("I3.component", "root guard"),
    ("I3.path",      ".a-new-structure/b-word/L0-universal/c-hybrid/spec/config/src/root.rs"),
    ("I3.provides",  "RootGuard"),
    ("I3.brief",     "Set once at startup, checked by all loaders. Single source of truth for project root."),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "universal"),
    ("I4.pattern",   "infrastructure module"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context — lifecycle, attribution, and operational metadata.
///
/// Seven groups: State (version, status, timestamps), Attribution (architect,
/// implementation, organization), Grounding (Psalm 11:3 — foundations matter),
/// Dependencies (stdlib RwLock + PathBuf, internal ConfigError, consumed by
/// loader/discovery/format_loader), Intent (purpose, philosophy),
/// Roadmap (current phase, limitations), Classification (tags, category, domain).
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-03.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-19"),
    ("C1.updated",           "2026-02-21"),
    // C2: Attribution
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn (CPI-SI)"),
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Psalm 11:3"),
    ("C3.principle",         "Set once, trust everywhere — the root is the anchor."),
    ("C3.anchor",            "Genesis 1:1"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "std::path::PathBuf, std::sync::RwLock"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "crate::error::ConfigError"),
    ("C4.consumers",         "loader, discovery, format_loader"),
    ("C4.integration",       "Set RootGuard at startup, check before any file operations"),
    ("C4.if_missing",        "Config loader cannot resolve relative paths without root context"),
    // C5: Intent
    ("C5.purpose",           "Thread-safe root directory guard — set once at startup, checked by all loaders."),
    ("C5.philosophy",        "Single point of truth for project root. Fail fast if unset."),
    // C6: Roadmap
    ("C6.current",           "b-03.00 — Thread-safe root directory guard, set/check/get/clear lifecycle."),
    ("C6.planned",           "none"),
    ("C6.limitations",       "Panics on poisoned RwLock (design choice — config corruption is unrecoverable)"),
    // C7: Classification
    ("C7.tags",              "root, guard, config, thread-safe, infrastructure"),
    ("C7.category",          "infrastructure"),
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
//     [1]  Imports           — What this file depends on
//     [2]  Modules           — RESERVED
//     [3]  Constants         — Compile-time fixed values
//     [4]  Statics           — Runtime-initialized fixed values
//     [5]  Type Aliases      — Shorthand for complex signatures
//     [6]  Error Types       — enum + Display + Error + From impls
//     [7]  Core Types        — struct/enum + derives + completing trait impls
//     [8]  Trait Defs        — Behavioral contracts (shape, not fulfillment)
//     [9]  Macros            — RESERVED
//     [10] Feature Gates     — RESERVED

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use std::path::PathBuf;
use std::sync::RwLock;

use crate::error::ConfigError;

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

/// Thread-safe root directory guard.
///
/// Set once at startup via [`RootGuard::set`], checked by all `Load*` functions
/// via [`RootGuard::check`]. Uses `RwLock` for interior mutability — multiple
/// readers, exclusive writer.
///
/// Poisoned locks are recovered via `into_inner()` rather than panicking,
/// because a poisoned lock in config loading means another thread panicked
/// during write — the value inside is still usable.
pub(crate) struct RootGuard {
    name: String,
    path: RwLock<Option<PathBuf>>,
}

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Constants, Statics, Type Aliases, Error Types, Trait Defs
//
// Reserved (structural — not used in module form):
//   Modules       — Submodule declarations belong in lib.rs or parent mod.rs.
//   Macros        — Macro definitions belong in dedicated macro modules or crate root.
//   Feature Gates — Feature gates are crate-level configuration in Cargo.toml.

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
// creation, operation, observation, to output.
//
//     [1]  Identity Access    — OmniCode static accessor functions
//     [2]  Trait Implementations — Fulfilling contracts from SETUP
//     [3]  Constructors & Builders — new(), builders, typestate transitions
//     [4]  Core Logic         — Primary operations, state transforms
//     [5]  Queries & Accessors — Read-only &self methods
//     [6]  Output & Display   — RESERVED
//     [7]  Free Functions     — Module-level public utilities
//     [8]  Helpers            — RESERVED
//     [9]  Constructor Tests  — RESERVED
//     [10] Builder Tests      — RESERVED
//     [11] Transition Tests   — RESERVED
//     [12] Query Tests        — RESERVED
//     [13] Identity Tests     — RESERVED
//     [14] Error Tests        — RESERVED
//     [15] Edge Case Tests    — RESERVED
//     [16] Table Driven Tests — RESERVED
//     [17] Integration Tests  — RESERVED

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors & Builders
// ──────────────────────────────────────────────────────────────────────────

impl RootGuard {
    /// Create a new guard with the given name (used in error messages).
    pub(crate) fn new(name: &str) -> Self {
        Self {
            name: name.to_owned(),
            path: RwLock::new(None),
        }
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

impl RootGuard {
    /// Set the root directory. Called once at startup.
    pub(crate) fn set(&self, p: PathBuf) {
        let mut guard = self.path.write().unwrap_or_else(|e| e.into_inner());
        *guard = Some(p);
    }

    /// Clear the root directory (test teardown).
    pub(crate) fn clear(&self) {
        let mut guard = self.path.write().unwrap_or_else(|e| e.into_inner());
        *guard = None;
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Queries & Accessors
// ──────────────────────────────────────────────────────────────────────────

impl RootGuard {
    /// Check that the root has been set. Returns `Err(RootNotSet)` if unset.
    pub(crate) fn check(&self) -> Result<(), ConfigError> {
        let guard = self.path.read().unwrap_or_else(|e| e.into_inner());
        if guard.is_none() {
            return Err(ConfigError::RootNotSet {
                name: self.name.clone(),
            });
        }
        Ok(())
    }

    /// Get the root directory path, if set.
    pub(crate) fn get(&self) -> Option<PathBuf> {
        let guard = self.path.read().unwrap_or_else(|e| e.into_inner());
        guard.clone()
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Identity Access, Trait Implementations, Free Functions
//
// Reserved (structural — not used in module form):
//   Output & Display — Output formatting belongs to the crate's output layer.
//   Helpers          — Executable Helpers serve run() — modules don't have run().
//   Constructor Tests, Builder Tests, Transition Tests, Query Tests,
//   Identity Tests, Error Tests, Edge Case Tests, Table Driven Tests,
//   Integration Tests — Test BODY sections belong in CLOSING Cv.

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
    use std::path::PathBuf;

    #[test]
    fn test_root_guard_lifecycle() {
        let guard = RootGuard::new("test");
        assert!(guard.check().is_err());
        assert!(guard.get().is_none());

        guard.set(PathBuf::from("/tmp/test"));
        assert!(guard.check().is_ok());
        assert_eq!(guard.get().unwrap(), PathBuf::from("/tmp/test"));

        guard.clear();
        assert!(guard.check().is_err());
        assert!(guard.get().is_none());
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
// related_files: [Cargo.toml, lib.rs, loader.rs, discovery.rs]
// validate: cargo test -p bereshit-l0-config

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
// note: "root.rs — Thread-safe root directory guard. The root anchors all relative paths."
// scripture: "Psalm 11:3 — If the foundations be destroyed, what can the righteous do?"
// anchor: "The root is set once and trusted everywhere. Without it, nothing resolves."

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
// Available (not needed in this module):
//   Cc (Closing Cleanup), X2 (Extension Points), X3 (Troubleshooting)
//
// Reserved (structural — not used in module form):
//   Test Coverage Summary — Coverage summaries belong in demo-test form files.

// ============================================================================
// END CLOSING [END]
// ============================================================================
