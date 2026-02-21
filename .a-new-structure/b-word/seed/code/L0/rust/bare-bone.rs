// #!omni template --rust -bare-bone
// #!omni meta.key = B-seed-code-L0-rust-bare-bone
// #!omni meta.from = b-word/seed/code/L0/rust/root.omni
// #!omni meta.at = template
// ============================================================================
// METADATA
// ============================================================================
//
// TEMPLATE: Rust Bare-Bone — 4-Block Foundation
//
// The absolute minimum valid Rust file in the Bereshit project. Every form
// variant (library, executable, module, test) builds ON TOP of this by
// promoting Reserved Omission sections and adding form-specific content.
//
// Usage:
//   1. cp bare-bone.rs /path/to/new/crate/src/lib.rs (or main.rs)
//   2. Change pragma: #!omni template → //omni:code --rust -[form]
//   3. Update meta.key, meta.from, meta.at for your crate
//   4. Update PRAGMA and METADATA values to describe your crate
//   5. Promote Reserved Omission sections as needed for your form
//   6. Create Cargo.toml (inherit workspace settings)
//   7. Add crate to workspace Cargo.toml members
//
// derives_from: b-word/seed/code/L0/rust/root.omni

//! # Rust Bare-Bone Template
//!
//! Foundation 4-block template for all Rust files. This is the rock that
//! every form variant builds on — the absolute minimum that the standard
//! requires. Library, executable, module, and test templates derive from
//! this by promoting reserved sections.
//!
//! ## Design
//!
//! Pure std — no external dependencies. Identity and context carried
//! as compile-time static data via `PRAGMA` (I1-I4) and `METADATA` (C1-C7).
//!
//! ## Usage
//!
//! ```bash
//! cp bare-bone.rs /path/to/new/crate/src/lib.rs
//! # Change pragma: #!omni template → //omni:code --rust -[form]
//! # Promote Reserved Omission sections for your form
//! ```
//!
//! ## OmniCode Identity
//!
//! | Static | Sections | Purpose | Query |
//! |--------|----------|---------|-------|
//! | [`PRAGMA`] | I1-I4 | Identity — who this file is | [`pragma_get`] |
//! | [`METADATA`] | C1-C7 | Context — when, where, why, how | [`metadata_get`] |

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
/// let key = pragma_get("I1.key").unwrap_or("unknown");
/// assert_eq!(key, "B-seed-code-L0-rust-bare-bone");
/// ```
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-seed-code-L0-rust-bare-bone"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/root.omni"),
    ("I1.at",        "template"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "bare-bone"),
    ("I2.role",      "seed"),
    // I3: Instance
    ("I3.file",      "bare-bone.rs"),
    ("I3.title",     "Rust Bare-Bone Template"),
    ("I3.component", "Foundation template — absolute minimum for all Rust forms"),
    ("I3.path",      "seed/code/L0/rust/bare-bone.rs"),
    ("I3.provides",  "RUST_BARE_BONE_TEMPLATE"),
    ("I3.brief",     "Foundation 4-block template for all Rust files. Form variants derive from this."),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "seed"),
    ("I4.pattern",   "cp bare-bone.rs src/lib.rs"),
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
/// let version = metadata_get("C1.version").unwrap_or("unknown");
/// assert_eq!(version, "a-01.00");
/// ```
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "a-01.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-19"),
    ("C1.updated",           "2026-02-19"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Matthew 7:24 — Built his house upon a rock"),
    ("C3.principle",         "The bare bone is the rock — the absolute minimum that stands"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none"),
    ("C4.consumers",         "All Rust form templates (library, executable, module, test)"),
    ("C4.integration",       "cp bare-bone.rs /path/to/crate/src/lib.rs"),
    ("C4.if_missing",        "Form variants have no validated foundation to derive from"),
    // C5: Intent
    ("C5.purpose",           "Absolute minimum 4-block Rust file — foundation all forms derive from"),
    ("C5.philosophy",        "Define the rock first, then build the house — bare bone before form"),
    // C6: Roadmap
    ("C6.current",           "a-01.00 — All 4 blocks with bare-bone minimums"),
    ("C6.planned",           "Schema-driven validation, form-specific derivation layouts"),
    ("C6.limitations",       "Reserved Omission sections need promotion for real files"),
    // C7: Classification
    ("C7.tags",              "template, rust, bare-bone, 4-block, seed, omnicode, foundation"),
    ("C7.category",          "Foundation"),
    ("C7.domain",            "bereshit"),
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
// Everything the BODY needs — types, constants, imports, error definitions —
// is declared here. The BODY contains only functions and impl blocks that
// operate on what SETUP established.
//
// If anything must be hardcoded, it lives here — never scattered through BODY.
// When a hardcoded value gets promoted to config, you extract from one place.
//
// Section order (dependency chain — each layer uses only what's above):
//
//   1. Imports         — What this file depends on
//   2. Constants       — Compile-time fixed values
//   3. Error Types     — enum + Display + Error + From impls
//   4. Core Types      — struct/enum + derives + completing trait impls
//   5. Reserved Omission — Sections defined in full template, not used here

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────
//
// Order: Standard Library → External Crates → Internal Crates → Crate-Internal
// Group by origin with comment headers. Each group alphabetical.
// Wildcard imports (use std::io::*) are discouraged — be explicit.

// --- Standard Library ---
// use std::fmt;
// use std::error;
// use std::collections::HashMap;
// use std::path::{Path, PathBuf};

// --- External Crates ---
// use serde::{Serialize, Deserialize};

// --- Internal Crates ---
// use bereshit_l0_config::Config;

// --- Crate-Internal ---
// use crate::types::{MyType, OtherType};

// ──────────────────────────────────────────────────────────────────────────
// 2. Constants
// ──────────────────────────────────────────────────────────────────────────
//
// Compile-time fixed values. Don't depend on types — they come first.
// SCREAMING_SNAKE_CASE. Prefer named constants over raw literals in BODY.

// const MAX_RETRIES: usize = 3;
// const DEFAULT_TIMEOUT_SECS: u64 = 30;

// ──────────────────────────────────────────────────────────────────────────
// 3. Error Types
// ──────────────────────────────────────────────────────────────────────────
//
// Errors are part of the API contract — they define how failure looks.
// Place BEFORE core types because core type methods return these errors.
//
// Pattern: enum for variants → Display for messages → Error for chaining.
// Completing impls (Display, Error, From) stay WITH the type here in SETUP.
//
// Derive Debug + Clone + PartialEq for testability.

// /// Errors that can occur during [operation].
// #[derive(Debug, Clone, PartialEq, Eq)]
// pub enum [ErrorType] {
//     /// [Description of when this variant occurs.]
//     [VariantName](String),
//     /// [Another error condition.]
//     [AnotherVariant](String),
// }
//
// impl fmt::Display for [ErrorType] {
//     fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
//         match self {
//             Self::[VariantName](msg) => write!(f, "[prefix]: {msg}"),
//             Self::[AnotherVariant](msg) => write!(f, "[prefix]: {msg}"),
//         }
//     }
// }
//
// impl std::error::Error for [ErrorType] {}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Types
// ──────────────────────────────────────────────────────────────────────────
//
// Data structures for this file's domain. The shapes of what this
// file works with — not the operations on them (those go in BODY).
//
// Completing trait impls (Display, Default) belong here WITH the type.
// Simple constructors (new()) belong here WITH the type.
// Complex methods and logic belong in BODY.

// /// [TypeName] represents [what this models].
// ///
// /// [2-4 sentences: what it represents, lifecycle, key constraints.]
// #[derive(Debug)]
// pub struct [TypeName] {
//     [field]: String,
//     [field]: Option<String>,
//     [field]: bool,
// }

// ──────────────────────────────────────────────────────────────────────────
// 5. Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// The following SETUP sections exist across form templates but are not
// present in bare-bone. Form variants promote these as needed.
//
//   Modules        — Submodule tree + re-exports       [library, executable, demo-test]
//   Statics        — Runtime-initialized fixed values   [library, executable, demo-test]
//   Type Aliases   — Shorthand for complex signatures   [library, executable, demo-test]
//   Trait Defs     — Behavioral contracts               [library, demo-test]
//   Macros         — Code templates, macro_rules!       [library, demo-test]
//   Feature Gates  — Conditional compilation groups     [library, executable, demo-test]
//
// To promote a reserved section: copy its full definition from the
// appropriate form template into this file above this section, renumber
// accordingly, and remove the entry from this list.

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// BODY makes things HAPPEN. SETUP made things EXIST.
//
// Everything in BODY is executable logic — impl blocks with methods,
// free functions. BODY operates on what SETUP established. If BODY
// needs a new type or constant, add it to SETUP.
//
// The BODY/SETUP separation mirrors behavior/data: SETUP is the schema
// (what exists), BODY is the queries (what happens). When SETUP is clean,
// you can reason about data flow without reading logic.
//
// Section order:
//
//   1. Identity Access  — OmniCode static accessor functions
//   2. Core Logic       — Primary operations, state transforms
//   3. Free Functions   — Module-level public utilities
//   4. Reserved Omission — Sections defined in full template, not used here

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────
//
// OmniCode static accessor functions for PRAGMA and METADATA.
// Every file with identity statics exposes these four functions —
// they are the file's self-awareness, providing runtime access to
// compile-time identity data.
//
// These go first because they are universal (every file has them),
// always present, and don't depend on any other BODY logic.
//
// Pattern: each pair is (full_slice, key_lookup).
//   Full slice:  returns &[(&str, &str)] for iteration
//   Key lookup:  returns Option<&str> for targeted access

// /// Returns this file's OmniCode pragma identity (I1-I4).
// pub fn pragma() -> &'static [(&'static str, &'static str)] {
//     PRAGMA
// }
//
// /// Looks up a specific pragma key. Returns `None` if not found.
// ///
// /// ```
// /// assert_eq!([crate_name]::pragma_get("I1.key"), Some("[project-key]"));
// /// assert_eq!([crate_name]::pragma_get("I99.nope"), None);
// /// ```
// pub fn pragma_get(key: &str) -> Option<&'static str> {
//     PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
// }
//
// /// Returns this file's OmniCode metadata (C1-C7).
// pub fn metadata() -> &'static [(&'static str, &'static str)] {
//     METADATA
// }
//
// /// Looks up a specific metadata key. Returns `None` if not found.
// pub fn metadata_get(key: &str) -> Option<&'static str> {
//     METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
// }

// ──────────────────────────────────────────────────────────────────────────
// 2. Core Logic
// ──────────────────────────────────────────────────────────────────────────
//
// Primary operations — state transitions, computations, transformations,
// business rules. The "verbs" of the system.
//
// Methods here take &self (read) or &mut self (mutate). If a method
// consumes self, it's probably a constructor (promote from Reserved
// Omission → Constructors & Builders section).
//
// Keep functions focused — one responsibility per function. If a method
// needs more than 5 lines of doc comment to explain what it does,
// it's doing too much. Break it down.
//
// Internal helpers (fn, not pub fn) belong here too.
// Place them near the public functions that call them.

// impl [TypeName] {
//     /// [Primary operation description].
//     pub fn [operation](&self, [params]) -> [ReturnType] {
//         // Core logic
//     }
//
//     /// [Mutation description].
//     pub fn [transform](&mut self, [params]) -> Result<(), [ErrorType]> {
//         // State transformation
//     }
//
//     /// Internal helper for [operation].
//     fn [helper](&self, [params]) -> [ReturnType] {
//         // Supporting logic
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// 3. Free Functions
// ──────────────────────────────────────────────────────────────────────────
//
// Module-level public functions not attached to any type. These form
// the file's top-level API — convenience wrappers, facades, one-shot
// utilities.
//
// When to use free functions vs methods:
//   Method:        operates on a type's data → impl block in §2
//   Free function: combines types, operates on primitives → here
//
// This is also where register_identity() lives when the file
// participates in the L0 identity system.

// /// Registers this file with the L0 identity system.
// pub fn register_identity() {
//     bereshit_l0_identity::register(
//         pragma_get("I1.key").unwrap_or("unknown"),
//         PRAGMA,
//         METADATA,
//     );
// }

// ──────────────────────────────────────────────────────────────────────────
// 4. Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// The following BODY sections exist across form templates but are not
// present in bare-bone. Form variants promote these as needed.
//
//   Helpers                 — Arg parsing, config loading, utilities  [executable]
//   Trait Implementations   — Fulfilling SETUP trait contracts        [library]
//   Constructors & Builders — new(), builder chains, typestate        [library]
//   Queries & Accessors     — Read-only &self methods, getters       [library]
//   Output & Display        — Formatting, serialization, terminal    [library]
//   Constructor Tests       — Verify default state after new()       [demo-test]
//   Builder Tests           — Verify configuration methods           [demo-test]
//   Transition Tests        — Verify state transitions + validation  [demo-test]
//   Query & Accessor Tests  — Verify post-transition accessors       [demo-test]
//   Identity Tests          — Verify PRAGMA/METADATA integrity       [demo-test]
//   Error Tests             — Verify error formatting + conditions   [demo-test]
//   Edge Case Tests         — Unicode, empty, long, boundary inputs  [demo-test]
//   Table-Driven Tests      — Systematic input/output validation     [demo-test]
//   Integration Tests       — End-to-end workflows                   [demo-test]
//
// To promote a reserved section: copy its full definition from the
// appropriate form template into this file above this section, renumber
// accordingly, and remove the entry from this list.

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// CLOSING = Validation → Execution → Cleanup. For code files, CLOSING
// contains actual code zones (commented-out in templates) followed by
// documentation sections. The code zones prove, run, and clean up.
// The documentation sections guide future maintainers.
//
// Code Zones (order: Validation → Execution → Cleanup):
//   Cv — Closing Validation   #[cfg(test)] inline tests
//   Ce — Closing Execution    Library init/registration (if any)
//
// Documentation Sections:
//   X1 — Policy (modification guidelines)       [REQUIRED]
//   X4 — Reference (deps, validation, related)   [DEFINED]
//   X5 — Closing Note (scripture anchor)          [REQUIRED]
//   X6 — Template Guide (remove when instantiating) [TEMPLATE-ONLY]
//
// Reserved Omission:
//   Cc — Closing Cleanup (Drop impls — form-specific)
//   X2 — Extension Points (growth seams — form-specific)
//   X3 — Troubleshooting (failure modes — form-specific)
//
// Comment style in CLOSING:
//   ///  — On pub items (when uncommented, generates rustdoc)
//   //   — Structural markers, template guidance, test internals
//   //!  — Not used in CLOSING (module-level only, lives in METADATA)

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────
//
// Inline tests via #[cfg(test)] mod tests. Compiled for `cargo test` only —
// excluded from release builds. Tests have white-box access to private
// internals via `use super::*`.
//
// Organize tests by concern matching BODY sections. Each test must be
// independent — no shared mutable state, no ordering dependencies.
//
// Bare-bone minimum: Identity tests (pragma/metadata access) are always
// present — every file with identity statics must prove they work.
// Additional test sections scale with BODY content.
//
// Assertion toolkit:
//   assert!(condition)                        — boolean check
//   assert_eq!(left, right)                   — equality with diff on fail
//   assert_ne!(left, right)                   — inequality
//   assert!(matches!(val, Pattern))           — enum/pattern matching
//   #[should_panic(expected = "msg")]         — panic tests
//   fn test() -> Result<(), ErrorType>        — fallible tests (? operator)

// #[cfg(test)]
// mod tests {
//     use super::*;
//
//     // -- Identity ----------------------------------------------------
//
//     #[test]
//     fn pragma_contains_key() {
//         assert_eq!(pragma_get("I1.key"), Some("[project-key]"));
//     }
//
//     #[test]
//     fn pragma_contains_format() {
//         assert_eq!(pragma_get("I1.format"), Some("rust"));
//     }
//
//     #[test]
//     fn pragma_get_returns_none_for_missing() {
//         assert_eq!(pragma_get("I1.nonexistent"), None);
//     }
//
//     #[test]
//     fn metadata_contains_grounding() {
//         let scripture = metadata_get("C3.scripture").unwrap();
//         assert!(scripture.contains("[expected text]"));
//     }
//
//     // -- Core Logic --------------------------------------------------
//
//     // Add tests matching your BODY §2 Core Logic functions here.
//
//     // -- Error Display -----------------------------------------------
//
//     // #[test]
//     // fn error_display_formats_correctly() {
//     //     let err = [ErrorType]::[VariantName]("[test]".to_string());
//     //     assert_eq!(err.to_string(), "[prefix]: [test]");
//     // }
// }

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library crates have no entry point — Ce is empty for libraries.
//
// If the library participates in a registration system (like L0 identity),
// the registration call is a public API function in BODY §3 (Free Functions)
// that consumers invoke explicitly — not automatic execution.
//
// Executables use Ce for main() and exit code handling.
// See: executable.rs template (when available).

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// The following CLOSING sections exist across form templates but are not
// present in bare-bone. Form variants promote these as needed.
//
//   Cc — Closing Cleanup          Drop impls, fixture teardown     [library, executable, demo-test]
//   Test Coverage Summary         Test count by concern             [demo-test]
//   X2 — Extension Points         Designed growth seams             [library, executable, demo-test]
//   X3 — Troubleshooting          Common problems and solutions     [library, executable, demo-test]
//
// To promote a reserved section: copy its full definition from the
// appropriate form template into this file above this section, and
// remove the entry from this list.

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Don't move the ancient boundary stone which your fathers have set up."
// — Proverbs 22:28
//
// The boundary stone protects architectural invariants. Safe changes add
// capability without breaking contracts. Careful changes affect interfaces
// that dependents rely on. Never-changes break the foundation.
//
// Safe to Modify:
//   - Add new free functions (output helpers, formatters)
//   - Add new error variants to [ErrorType]
//   - Add impl blocks for new traits
//   - Add #[cfg(test)] tests (they don't affect public API)
//   - Promote sections from Reserved Omission
//
// Modify with Care:
//   - Struct fields — breaks pattern matching, construction, serde
//   - Method signatures — breaks all call sites
//   - Error type shape — breaks match arms in error handlers
//   - Return types — existing callers may not handle new variants
//
// Never Modify:
//   - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//   - PRAGMA/METADATA static data layout or accessor signatures
//   - Block boundary markers (// ====... SETUP/BODY/CLOSING ====...)
//   - Public trait contracts (breaking changes require major version bump)
//
// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Dependencies:   [std modules used — e.g., std::fmt, std::error]
// Dependents:     [crates that depend on this one]
// Supersedes:     [prior implementation path, if any]
// Parallel:       [Go/C implementations of same concept]
// Template:       b-word/seed/code/L0/rust/bare-bone.rs
//
// Validation:
//   Build:    cargo build -p [crate-name]
//   Test:     cargo test -p [crate-name]
//   Clippy:   cargo clippy -p [crate-name] -- -D warnings
//   Doc:      cargo doc -p [crate-name] --no-deps
//   All:      cargo clippy -p [crate-name] -- -D warnings && cargo test -p [crate-name]
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// "Unless Yahweh builds the house, they labor in vain who build it."
// — Psalm 127:1
//
// The bare bone is the house's foundation. Not the rooms, not the doors,
// not the furniture — the rock it stands on. Every form variant builds
// from this. Every file in the project traces back to this minimum.
//
// ──────────────────────────────────────────────────────────────────────────
// X6: Template Guide (remove this section when instantiating)
// ──────────────────────────────────────────────────────────────────────────
//
// Instantiation:
//   1.  cp bare-bone.rs /path/to/new/crate/src/lib.rs
//   2.  Change pragma: #!omni template → //omni:code --rust -[form]
//   3.  Update meta.key, meta.from, meta.at for your crate
//   4.  Fill PRAGMA constants (I1-I4) with crate identity
//   5.  Fill METADATA constants (C1-C7) with crate context
//   6.  Promote Reserved Omission sections as needed for your form
//   7.  Implement types and logic in BODY
//   8.  Uncomment and fill #[cfg(test)] mod tests
//   9.  Create Cargo.toml (use seed/code/L0/toml/cargo/ template)
//  10.  Add crate to workspace Cargo.toml members
//  11.  Remove X6 section and all [placeholder] markers
//  12.  Run: cargo clippy -p crate-name -- -D warnings && cargo test -p crate-name
//
// Form variants (library, executable, module, test) build ON TOP of
// bare-bone by promoting Reserved Omission sections and adding
// form-specific content. See library.rs for the full library form.
//
// ============================================================================
// END CLOSING
// ============================================================================
