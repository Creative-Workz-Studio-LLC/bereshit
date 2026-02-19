//omni:code --rust -library
//omni:key B-L0-foundation-types
//omni:version b-02.00
//omni:pragma PRAGMA
//omni:metadata METADATA
//!
//! # Foundation Types
//!
//! Universal infrastructure types for all layers of the Bereshit stack.
//!
//! This crate is the bedrock — lightweight, serde-only, no heavy dependencies.
//! Every layer can depend on these types without pulling in SQLite or other
//! specialized concerns.
//!
//! ## Modules
//!
//! | Module | Types | Purpose |
//! |--------|-------|---------|
//! | [`result`] | [`OpResult`], [`LookupResult`], [`ValidationResult`] | Universal operation outcomes |
//! | [`session`] | [`Session`], [`SessionLog`], [`SessionPaths`] | Session tracking and filesystem |
//! | [`session_context`] | [`SessionContext`] | Substrate integration schema |
//! | [`workflow`] | [`WorkflowOperation`], [`RuntimeWorkflow`] | Step-based workflow tracking |
//!
//! ## Design
//!
//! These types were promoted from L3-cpisi when it became clear they serve
//! all layers, not just CPI-SI. The DB loader that reads a-ladder specs
//! lives separately in `bereshit-l2-database` (spec tier).
//!
//! ## OmniCode Identity
//!
//! | Static | Sections | Purpose | Query |
//! |--------|----------|---------|-------|
//! | [`PRAGMA`] | I1-I4 | Identity — who this file is | [`pragma_get`] |
//! | [`METADATA`] | C1-C7 | Context — when, where, why, how | [`metadata_get`] |

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-foundation-types
// Purpose: Universal infrastructure types — bedrock for all layers
// Biblical: Genesis 1:1 — In the beginning God created the heaven
//           and the earth
// Version: b-02.00

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for this crate.
///
/// Carries the I-sections (Identity) of the OmniCode metadata standard.
/// These fields establish WHO this file is in the project graph.
/// Zero-cost at runtime — compiled into the binary as static data.
///
/// # Sections
///
/// | Section | Purpose | Required | Defined |
/// |---------|---------|----------|---------|
/// | **I1** | Core | `key`, `format`, `from`, `at` | — |
/// | **I2** | Family | `type`, `structure` | `subtype`, `role` |
/// | **I3** | Instance | `file`, `title` | `component`, `path`, `provides`, `brief` |
/// | **I4** | Architecture | — | `layer`, `position`, `pattern` |
///
/// # Example
///
/// ```rust
/// let key = bereshit_l0_foundation::pragma_get("I1.key").unwrap_or("unknown");
/// assert_eq!(key, "B-L0-foundation-types");
/// ```
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-foundation-types"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/library.rs"),
    ("I1.at",        "b-02.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I2.role",      "foundation"),
    // I3: Instance
    ("I3.file",      "lib.rs"),
    ("I3.title",     "Foundation Types"),
    ("I3.component", "c-hybrid/core/foundation"),
    ("I3.path",      "L0-universal/c-hybrid/core/foundation/src/lib.rs"),
    ("I3.provides",  "FOUNDATION_TYPES"),
    ("I3.brief",     "Universal infrastructure types — serde-only bedrock for all layers"),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "hybrid/core/foundation"),
    ("I4.pattern",   "re-exporter crate root — submodules hold the types, lib.rs re-exports"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context for this crate.
///
/// Carries the C-sections (Context) of the OmniCode metadata standard.
/// These fields establish WHEN, WHERE, WHY, and HOW for this file.
/// Zero-cost at runtime — compiled into the binary as static data.
///
/// # Sections
///
/// | Section | Purpose | Required | Defined |
/// |---------|---------|----------|---------|
/// | **C1** | State | `version`, `status` | `created`, `updated` |
/// | **C2** | Attribution | `organization` | `architect`, `implementation`, `copyright` |
/// | **C3** | Grounding | `scripture` | `principle`, `anchor` |
/// | **C4** | Dependencies | `requires`, `consumers` | `integration`, `if_missing` |
/// | **C5** | Intent | — | `purpose`, `philosophy` |
/// | **C6** | Roadmap | — | `current`, `planned`, `limitations` |
/// | **C7** | Classification | — | `tags`, `category`, `domain`, `paradigm` |
///
/// # Example
///
/// ```rust
/// let version = bereshit_l0_foundation::metadata_get("C1.version").unwrap_or("unknown");
/// assert_eq!(version, "b-02.00");
/// ```
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-02.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-17"),
    ("C1.updated",           "2026-02-18"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    ("C3.principle",         "Foundation precedes everything. These types are the bedrock."),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "none (lib.rs re-exports only)"),
    ("C4.requires.external", "serde, serde_json (via submodules)"),
    ("C4.requires.internal", "none — L0 foundation, no internal dependencies"),
    ("C4.consumers",         "every Bereshit crate needing OpResult, Session, Workflow types"),
    ("C4.integration",       "use bereshit_l0_foundation::{OpResult, Session, ...}"),
    ("C4.if_missing",        "no universal types — every crate reinvents result/session/workflow"),
    // C5: Intent
    ("C5.purpose",           "Universal infrastructure types — serde-only bedrock for all layers"),
    ("C5.philosophy",        "Foundation depends on nothing heavy. Everything above depends on this."),
    // C6: Roadmap
    ("C6.current",           "b-02.00 — 4 modules, full 4-block alignment, I/C metadata"),
    ("C6.planned",           "no_std support, feature-gated serde format variants, error unification"),
    ("C6.limitations",       "JSON-only serialization via serde_json (no TOML/YAML yet)"),
    // C7: Classification
    ("C7.tags",              "foundation, types, L0, serde, universal, bedrock, omnicode, cpi-si"),
    ("C7.category",          "Foundation"),
    ("C7.domain",            "foundation"),
    ("C7.paradigm",          "CPI-SI"),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// SETUP makes things EXIST. BODY makes things HAPPEN.
//
// For a re-exporter crate root, SETUP is the module tree and re-exports.
// All types, constructors, and logic live in submodules. This file
// declares the public API surface — what consumers see.
//
// Section order (dependency chain — each layer uses only what's above):
//
//   [S01] imports         — What this file depends on (none)
//   [S02] modules         — Submodule tree + re-exports
//   [S03] constants       — N/A
//   [S04] statics         — N/A (PRAGMA/METADATA in METADATA block)
//   [S05] type-aliases    — N/A
//   [S06] error-types     — N/A (errors in submodules)
//   [S07] core-types      — N/A (types in submodules)
//   [S08] trait-defs      — N/A
//   [S09] macros          — N/A
//   [S10] feature-gates   — N/A

// ──────────────────────────────────────────────────────────────────────────
// 2. Modules
// ──────────────────────────────────────────────────────────────────────────
//
// Four submodules — one per domain concern:
//   result          — Operation outcome types (OpResult, LookupResult, ValidationResult)
//   session         — Session tracking (Session, SessionLog, SessionPaths)
//   session_context — Substrate integration schema (SessionContext)
//   workflow        — Step-based workflow tracking (WorkflowOperation, RuntimeWorkflow)
//
// Each module is self-contained with its own types, impls, and tests.
// Re-exports aggregate the public API at the crate root.

// --- Public Modules ---
pub mod result;
pub mod session;
pub mod session_context;
pub mod workflow;

// --- Re-exports (Public API) ---
//
// Flat access: `use bereshit_l0_foundation::{OpResult, Session, ...}`
// Consumers don't need to know which submodule a type lives in.

pub use result::{LookupResult, OpResult, ValidationResult};
pub use session::{format_duration, format_elapsed, LogEntry, Session, SessionLog, SessionPaths};
pub use session_context::{
    ContextWindow, Cost, CurrentUsage, Model, OutputStyle, SessionContext, Workspace,
};
pub use workflow::{RuntimeWorkflow, WorkflowOperation, WorkflowStep};

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// BODY makes things HAPPEN. SETUP made things EXIST.
//
// For a re-exporter crate root, BODY contains:
//   1. Identity Access — pragma(), pragma_get(), metadata(), metadata_get()
//
// All domain logic lives in the submodules. The crate root's only "logic"
// is providing runtime access to compile-time identity data.
//
// Subsection order:
//   1. Identity Access       — OmniCode static accessor functions
//   7. Free Functions        — register_identity() for L0 identity system

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────
//
// OmniCode static accessor functions for PRAGMA and METADATA.
// Every library crate exposes these four functions — they are the crate's
// self-awareness, providing runtime access to compile-time identity data.
//
// Pattern: each pair is (full_slice, key_lookup).
//   Full slice:  returns &[(&str, &str)] for iteration
//   Key lookup:  returns Option<&str> for targeted access

/// Returns this crate's OmniCode pragma identity (I1-I4).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Looks up a specific pragma key. Returns `None` if not found.
///
/// ```
/// assert_eq!(bereshit_l0_foundation::pragma_get("I1.key"), Some("B-L0-foundation-types"));
/// assert_eq!(bereshit_l0_foundation::pragma_get("I1.format"), Some("rust"));
/// assert_eq!(bereshit_l0_foundation::pragma_get("I99.nope"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (C1-C7).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Looks up a specific metadata key. Returns `None` if not found.
///
/// ```
/// assert_eq!(bereshit_l0_foundation::metadata_get("C1.version"), Some("b-02.00"));
/// assert_eq!(bereshit_l0_foundation::metadata_get("C99.nope"), None);
/// ```
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// CLOSING Zones Overview
// ──────────────────────────────────────────────────────────────────────────
//
// 3 Code Zones (operations):  Cv → Ce → Cc
// 5 Doc Sections (guidance):  X1 → X2 → X3 → X4 → X5
//
// Three-tier ordering:
//   1. All code zones before any doc sections
//   2. Within code: Cv (Validation) → Ce (Execution) → Cc (Cleanup)
//   3. Within docs: X1 (Policy) → X2 (Extension) → X3 (Troubleshooting)
//                   → X4 (Reference) → X5 (Note)

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    // -- Identity Accessors -----------------------------------------------

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("I1.key"), Some("B-L0-foundation-types"));
    }

    #[test]
    fn pragma_contains_format() {
        assert_eq!(pragma_get("I1.format"), Some("rust"));
    }

    #[test]
    fn pragma_contains_subtype() {
        assert_eq!(pragma_get("I2.subtype"), Some("library"));
    }

    #[test]
    fn pragma_get_returns_none_for_missing() {
        assert_eq!(pragma_get("I99.nope"), None);
    }

    #[test]
    fn metadata_contains_version() {
        assert_eq!(metadata_get("C1.version"), Some("b-02.00"));
    }

    #[test]
    fn metadata_contains_grounding() {
        let scripture = metadata_get("C3.scripture").unwrap();
        assert!(scripture.contains("Genesis 1:1"));
    }

    #[test]
    fn metadata_contains_domain() {
        assert_eq!(metadata_get("C7.domain"), Some("foundation"));
    }

    #[test]
    fn metadata_get_returns_none_for_missing() {
        assert_eq!(metadata_get("C99.nope"), None);
    }

    #[test]
    fn pragma_slice_has_all_sections() {
        for prefix in ["I1.", "I2.", "I3.", "I4."] {
            assert!(
                PRAGMA.iter().any(|(k, _)| k.starts_with(prefix)),
                "missing pragma section {prefix}"
            );
        }
    }

    #[test]
    fn metadata_slice_has_all_sections() {
        for prefix in ["C1.", "C2.", "C3.", "C4.", "C5.", "C6.", "C7."] {
            assert!(
                METADATA.iter().any(|(k, _)| k.starts_with(prefix)),
                "missing metadata section {prefix}"
            );
        }
    }

    // -- Re-export Verification -------------------------------------------
    //
    // Verify that re-exported types are accessible from the crate root.
    // This catches broken re-exports at compile time.

    #[test]
    fn re_exports_result_types() {
        let _ = OpResult::ok("test");
        let _ = LookupResult::no_match("q", "f");
        let _ = ValidationResult::valid("test.go");
    }

    #[test]
    fn re_exports_session_types() {
        let paths = SessionPaths::from_dir(std::path::PathBuf::from("/tmp/test"));
        assert!(paths.session_dir.ends_with("test"));
    }

    #[test]
    fn re_exports_workflow_types() {
        let w = RuntimeWorkflow::default();
        assert!(!w.is_active());
        assert_eq!(w.progress(), 0);
    }

    #[test]
    fn re_exports_session_context_types() {
        // Verify the sub-types are accessible (compile-time check)
        let _model = Model {
            id: "test".into(),
            display_name: "Test".into(),
        };
        let _style = OutputStyle {
            name: "default".into(),
        };
    }

    #[test]
    fn re_exports_helper_functions() {
        let d = std::time::Duration::from_secs(90 * 60);
        assert_eq!(format_duration(d), "1h 30m");
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library crate — no entry point. Imported and called, not executed directly.
// Future: register_identity() in BODY §7 when L0 identity system is integrated.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Serde types — no resources to clean up. No Drop needed.

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Remove not the ancient landmark, which thy fathers have set."
// — Proverbs 22:28
//
// Safe to Modify:
//   - Add new submodule declarations and re-exports
//   - Add #[cfg(test)] tests for re-export verification
//   - Update PRAGMA/METADATA values (version, dates, roadmap)
//
// Modify with Care:
//   - Re-export list changes — consumers depend on flat imports
//   - Module visibility (pub vs pub(crate))
//   - PRAGMA/METADATA static layout or accessor signatures
//
// Never Modify:
//   - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//   - &[(&str, &str)] identity format (I1-I4, C1-C7)
//   - Zero-heavy-dependency principle
//   - Identity access function signatures (pragma, pragma_get, metadata, metadata_get)

// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Completed (b-02.00):
//   - Full 4-block alignment with I/C metadata format
//   - PRAGMA (I1-I4) and METADATA (C1-C7) statics
//   - Identity access functions (pragma, pragma_get, metadata, metadata_get)
//   - Re-exports for all 4 submodules
//   - Re-export verification tests
//
// Designed Growth Points:
//   - New submodules: add mod declaration, re-exports, update brief
//   - register_identity() when L0 identity system is integrated
//
// Future Considerations:
//   - no_std support (feature-gate std::path usage in session.rs)
//   - Feature-gated serde format variants (toml, yaml)
//   - Error unification across submodules

// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lack wisdom, let him ask of God." — James 1:5
//
// Missing re-export:
//   Check that the type is pub in its submodule AND re-exported via
//   pub use in lib.rs. Both are required for flat access.
//
// Workspace not found:
//   Ensure b-word/Cargo.toml has this crate in [workspace.members]
//   at path: L0-universal/c-hybrid/core/foundation
//
// Serde version mismatch:
//   serde and serde_json must come from workspace.dependencies.
//   Check workspace Cargo.toml for version alignment.
//
// Doc tests fail:
//   The PRAGMA/METADATA accessor doc tests use the crate name
//   (bereshit_l0_foundation). Ensure the crate name matches Cargo.toml.

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Dependencies:   serde, serde_json (via submodules); std::path (session.rs)
// Dependents:     every Bereshit crate needing universal types
// Supersedes:     L3-cpisi types (promoted to L0)
// Parallel:       go/ directory (Go counterparts for each Rust module)
// Template:       b-word/seed/code/L0/rust/library.rs
//
// Validation:
//   Build:    cargo build -p bereshit-l0-foundation
//   Test:     cargo test -p bereshit-l0-foundation
//   Clippy:   cargo clippy -p bereshit-l0-foundation -- -D warnings
//   Doc:      cargo doc -p bereshit-l0-foundation --no-deps
//   All:      cargo clippy -p bereshit-l0-foundation -- -D warnings && cargo test -p bereshit-l0-foundation

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// Foundation types — the bedrock. Depends on serde only.
// Everything above can depend on this without pulling in heavy dependencies.
//
// The crate root exists to re-export and provide a single import point.
// All logic lives in the submodules — result, session, session_context,
// workflow. The boundary between "what exists" and "what consumers see"
// is the re-export list in SETUP §2.
//
// "In the beginning God created the heaven and the earth."
// — Genesis 1:1
//
// Foundation precedes everything. These types exist so all layers
// have solid ground to stand on.

// ============================================================================
// END CLOSING
// ============================================================================
