// #!omni template --rust -library
// #!omni meta.key = B-seed-code-L0-rust-library
// #!omni meta.from = b-word/seed/code/L0/rust/root.omni
// #!omni meta.at = template
// ============================================================================
// METADATA
// ============================================================================
//
// TEMPLATE: Rust Library Crate — 4-Block Structure
//
// Usage:
//   1. cp library.rs /path/to/new/crate/src/lib.rs
//   2. Change pragma: #!omni template → //omni:code --rust -library
//   3. Update meta.key, meta.from, meta.at for your crate
//   4. Update PRAGMA and METADATA values to describe your crate
//   5. Create Cargo.toml (inherit workspace settings)
//   6. Add crate to workspace Cargo.toml members
//
// derives_from: b-word/seed/code/L0/rust/root.omni

//! # Rust Library Crate Template
//!
//! Root 4-block template for Rust library crates. All Rust libraries
//! in the Bereshit project derive from this template.
//!
//! ## Design
//!
//! Pure std — no external dependencies. Identity and context carried
//! as compile-time static data via `PRAGMA` (I1-I4) and `METADATA` (C1-C7).
//!
//! ## Usage
//!
//! ```bash
//! cp library.rs /path/to/new/crate/src/lib.rs
//! # Change pragma: #!omni template → //omni:code --rust -library
//! # Update PRAGMA and METADATA values for your crate
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
/// assert_eq!(key, "B-seed-code-L0-rust-library");
/// ```
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-seed-code-L0-rust-library"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/root.omni"),
    ("I1.at",        "template"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I2.role",      "seed"),
    // I3: Instance
    ("I3.file",      "library.rs"),
    ("I3.title",     "Rust Library Crate Template"),
    ("I3.component", "Root template for Rust library crates"),
    ("I3.path",      "seed/code/L0/rust/library.rs"),
    ("I3.provides",  "RUST_LIBRARY_TEMPLATE"),
    ("I3.brief",     "Root 4-block template for Rust library crates. All Rust libraries derive from this."),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "seed"),
    ("I4.pattern",   "cp library.rs src/lib.rs"),
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
/// assert_eq!(version, "a-02.00");
/// ```
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "a-02.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-17"),
    ("C1.updated",           "2026-02-17"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Nova Dawn"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Exodus 25:40 — Look that thou make them after their pattern"),
    ("C3.principle",         "Templates establish the pattern; implementations follow faithfully"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none"),
    ("C4.consumers",         "All Rust library crates in the project"),
    ("C4.integration",       "cp library.rs /path/to/crate/src/lib.rs"),
    ("C4.if_missing",        "Rust libraries lack standardized I/C metadata structure"),
    // C5: Intent
    ("C5.purpose",           "Canonical 4-block structure for Rust library crates with I/C metadata"),
    ("C5.philosophy",        "Structure is skeleton; content is flesh — boundaries before content"),
    // C6: Roadmap
    ("C6.current",           "a-02.00 — All 4 blocks production-grade"),
    ("C6.planned",           "Rust 4-block linter, schema-driven validation"),
    ("C6.limitations",       "All 4 blocks production-grade (a-02.00)"),
    // C7: Classification
    ("C7.tags",              "template, rust, library, 4-block, seed, omnicode"),
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
//   2. Modules         — Submodule tree + re-exports (crate root only)
//   3. Constants       — Compile-time fixed values
//   4. Statics         — Runtime-initialized fixed values (non-identity)
//   5. Type Aliases    — Shorthand for complex signatures
//   6. Error Types     — enum + Display + Error + From impls
//   7. Core Types      — struct/enum + derives + completing trait impls
//   8. Trait Defs      — Behavioral contracts (shape, not fulfillment)
//   9. Macros          — Code templates (macro_rules!)
//  10. Feature Gates   — Conditional compilation items

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
// use std::sync::LazyLock;

// --- External Crates ---
// [Pure library — no external dependencies at L0]
// use serde::{Serialize, Deserialize};

// --- Internal Crates ---
// [Self-contained at L0 — no internal dependencies]
// use bereshit_l0_config::Config;

// --- Crate-Internal ---
// [Root module — no sub-imports at this level]
// use crate::types::{MyType, OtherType};

// ──────────────────────────────────────────────────────────────────────────
// 2. Modules
// ──────────────────────────────────────────────────────────────────────────
//
// Submodule declarations and public re-exports.
// Only present in lib.rs (crate root). Module files don't declare siblings.
// Private modules first (implementation), then public (API surface).
// Re-exports aggregate what consumers see.

// --- Private Modules ---
// mod internal;
// mod helpers;

// --- Public Modules ---
// pub mod api;
// pub mod types;

// --- Re-exports (Public API) ---
// pub use api::{PublicType, public_function};
// pub use types::DomainType;

// ──────────────────────────────────────────────────────────────────────────
// 3. Constants
// ──────────────────────────────────────────────────────────────────────────
//
// Compile-time fixed values. Don't depend on types — they come first.
// SCREAMING_SNAKE_CASE. Prefer named constants over raw literals in BODY.

// const MAX_RETRIES: usize = 3;
// const DEFAULT_TIMEOUT_SECS: u64 = 30;

// ──────────────────────────────────────────────────────────────────────────
// 4. Statics
// ──────────────────────────────────────────────────────────────────────────
//
// Runtime-initialized fixed values. Lazy singletons, thread-locals.
// NOTE: PRAGMA and METADATA identity statics live in the METADATA block.
//
// Use LazyLock (std::sync) for thread-safe lazy initialization.
// Use RwLock/Mutex inside LazyLock for mutable global state (rare — avoid).

// static REGISTRY: LazyLock<Registry> = LazyLock::new(Registry::new);

// ──────────────────────────────────────────────────────────────────────────
// 5. Type Aliases
// ──────────────────────────────────────────────────────────────────────────
//
// Shorthand for complex types. Improves readability of function signatures.
// Don't overuse — aliases hide information. Use when the full type is unwieldy.

// type Result<T> = std::result::Result<T, LibError>;
// type FieldMap = BTreeMap<String, Vec<String>>;

// ──────────────────────────────────────────────────────────────────────────
// 6. Error Types
// ──────────────────────────────────────────────────────────────────────────
//
// Errors are part of the API contract — they define how failure looks.
// Place BEFORE core types because core type methods return these errors.
//
// Pattern: enum for variants → Display for messages → Error for chaining.
// Completing impls (Display, Error, From) stay WITH the type here in SETUP.
//
// Derive Debug + Clone + PartialEq for testability.
// Consider thiserror crate for complex error hierarchies.

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
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────
//
// Data structures for this library's domain. The shapes of what this
// library works with — not the operations on them (those go in BODY).
//
// Completing trait impls (Display, Default) belong here WITH the type.
// Simple constructors (new()) belong here WITH the type.
// Complex methods and logic belong in BODY.
//
// Derive macros:
//   Debug       — Required for test assertions and error messages
//   Clone       — When values need copying
//   PartialEq   — When values need comparison (tests, dedup)
//   Default     — When zero-value construction makes sense
//
// Patterns:
//   Builder/Typestate — Type A (builder) → Type B (validated)
//   Newtype          — struct Wrapper(Inner) for type-safe domain values
//   Enum             — For finite state sets (Rust enums >> Go iota)

// /// [TypeName] represents [what this models].
// ///
// /// [2-4 sentences: what it represents, lifecycle, key constraints.]
// ///
// /// # Example
// ///
// /// ```
// /// use [crate_name]::[TypeName];
// ///
// /// let item = [TypeName]::new("[name]");
// /// ```
// #[derive(Debug)]
// pub struct [TypeName] {
//     [field]: String,
//     [field]: Option<String>,
//     [field]: bool,
// }

// /// [ComposedType] holds the result of [operation].
// ///
// /// Created by [`[TypeName]::[method]`]. Query methods only
// /// available on this type (typestate enforcement).
// #[derive(Debug)]
// pub struct [ComposedType] {
//     [field]: String,
//     [field]: Vec<String>,
// }

// ──────────────────────────────────────────────────────────────────────────
// 8. Trait Definitions
// ──────────────────────────────────────────────────────────────────────────
//
// Behavioral contracts — the SHAPE of what something does, not the doing.
// Trait definitions (the contract) belong here.
// Trait implementations (impl Trait for Type) belong in BODY.
//
// Exception: completing impls (Display, Error, From, Default) stay with
// their type in sections 6-7 — they complete the type's existence.

// /// [TraitName] defines the contract for [behavior].
// ///
// /// Implementors must provide [key capability].
// pub trait [TraitName] {
//     /// [Method description.]
//     fn [method](&self, [params]) -> [ReturnType];
// }

// ──────────────────────────────────────────────────────────────────────────
// 9. Macros
// ──────────────────────────────────────────────────────────────────────────
//
// Code templates for generating repetitive declarations.
// Macros are SETUP because they're patterns, not execution.
// Macro invocations may appear in SETUP (generating types) or BODY
// (generating functions).

// macro_rules! define_variant {
//     ($name:ident, $msg:expr) => {
//         // ...
//     };
// }

// ──────────────────────────────────────────────────────────────────────────
// 10. Feature Gates
// ──────────────────────────────────────────────────────────────────────────
//
// Conditional compilation items. Feature-gated types, imports, or constants
// that only exist under certain build configurations.
//
// Individual #[cfg(...)] on items above is fine.
// This section is for GROUPS of conditional items that don't fit above.

// #[cfg(feature = "serde")]
// mod serde_support {
//     use super::*;
//     use serde::{Serialize, Deserialize};
//     // Feature-gated type derivations, conversions, etc.
// }

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
// trait implementations, free functions. BODY operates on what SETUP
// established. If BODY needs a new type or constant, add it to SETUP.
//
// The BODY/SETUP separation mirrors behavior/data: SETUP is the schema
// (what exists), BODY is the queries (what happens). When SETUP is clean,
// you can reason about data flow without reading logic.
//
// Subsection order follows the type lifecycle — from identity through
// creation, operation, observation, to output. Present subsections appear
// in this sequence. Not all 7 are required — only those with content.
//
//   1. Identity Access       — OmniCode static accessor functions
//   2. Trait Implementations — Fulfilling contracts from SETUP §8
//   3. Constructors          — new(), builders, typestate transitions
//   4. Core Logic            — Primary operations, state transforms
//   5. Queries               — &self methods that read/derive state
//   6. Output                — Formatters, display helpers, serialization
//   7. Free Functions        — Module-level public utilities

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────
//
// OmniCode static accessor functions for PRAGMA and METADATA.
// Every library crate exposes these four functions — they are the crate's
// self-awareness, providing runtime access to compile-time identity data.
//
// These go first because they are universal (every crate has them),
// always present, and don't depend on any other BODY logic.
//
// Pattern: each pair is (full_slice, key_lookup).
//   Full slice:  returns &[(&str, &str)] for iteration
//   Key lookup:  returns Option<&str> for targeted access

// /// Returns this crate's OmniCode pragma identity (I1-I4).
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
// /// Returns this crate's OmniCode metadata (C1-C7).
// pub fn metadata() -> &'static [(&'static str, &'static str)] {
//     METADATA
// }
//
// /// Looks up a specific metadata key. Returns `None` if not found.
// pub fn metadata_get(key: &str) -> Option<&'static str> {
//     METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
// }

// ──────────────────────────────────────────────────────────────────────────
// 2. Trait Implementations
// ──────────────────────────────────────────────────────────────────────────
//
// Behavioral trait impls — fulfilling contracts declared in SETUP §8.
// These are the "doing" of what trait definitions "described."
//
// IMPORTANT — completing vs behavioral:
//   SETUP (stays with type):  Display, Debug (custom), Error, From, Default,
//                              PartialEq, PartialOrd, Clone
//                              → These COMPLETE the type. Remove one and
//                                the type is incomplete.
//   BODY (goes here):         Iterator, Read, Write, IntoIterator,
//                              Index, custom traits from §8
//                              → These ADD capabilities. Remove one and
//                                the type still works, just does less.
//
// The test: if removing the impl makes the type unusable in basic
// operations (can't display, can't compare), it completes the type
// → SETUP. If removing it removes a capability but the type still
// functions, it's behavioral → BODY.

// impl [TraitName] for [TypeName] {
//     fn [method](&self, [params]) -> [ReturnType] {
//         // Implementation logic fulfilling the contract
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors & Builders
// ──────────────────────────────────────────────────────────────────────────
//
// How types come into existence. Three patterns, simple to complex:
//
//   Simple:     TypeName::new(args) → Self
//   Builder:    TypeName::new(args).with_x(v).with_y(v) → Self
//   Typestate:  Builder::new(args).with_x(v).transition() → Validated
//
// Typestate is the strongest — the compiler prevents calling methods
// that don't make sense before validation. Builder methods consume
// `self` (move semantics). The transition method consumes the builder
// and returns a different type entirely.
//
// Placement:
//   Simple new() with field assignment only → defensible in SETUP §7
//   new() with validation, I/O, or error paths → BODY §3
//   Builder/typestate pattern → always BODY §3

// impl [TypeName] {
//     /// Creates a new [TypeName] with the given [params].
//     ///
//     /// [Standard behavior — what's always available after construction.]
//     pub fn new([param]: &str) -> Self {
//         Self {
//             [field]: [param].to_string(),
//             [field]: None,
//             [field]: false,
//         }
//     }
//
//     /// Sets [optional configuration].
//     ///
//     /// ```
//     /// # use [crate_name]::[TypeName];
//     /// let item = [TypeName]::new("[name]").[with_method]("[value]");
//     /// ```
//     pub fn [with_method](mut self, [param]: &str) -> Self {
//         self.[field] = Some([param].to_string());
//         self
//     }
//
//     /// Enables [optional feature].
//     ///
//     /// Without calling this, [feature] is not available.
//     pub fn [with_feature](mut self) -> Self {
//         self.[field] = true;
//         self
//     }
//
//     /// Consumes `[TypeName]` and produces [`[ComposedType]`].
//     ///
//     /// This is the typestate transition — configuration is complete,
//     /// the type system now exposes query/operational methods.
//     ///
//     /// # Errors
//     ///
//     /// Returns [`[ErrorType]::[Variant]`] if [error condition].
//     pub fn [transition_method](self, [params]) -> Result<[ComposedType], [ErrorType]> {
//         // Validate and transform
//         // ...
//
//         Ok([ComposedType] {
//             [field]: self.[field],
//             [field]: [computed_value],
//         })
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────
//
// Primary operations — state transitions, computations, transformations,
// business rules. The "verbs" of the system.
//
// This is typically the heaviest BODY subsection. If it grows beyond
// ~200 lines, extract into a submodule (add to SETUP §2).
//
// Methods here take &self (read) or &mut self (mutate). If a method
// consumes self, it's probably a constructor/transition (§3).
//
// Keep functions focused — one responsibility per function. If a method
// needs more than 5 lines of doc comment to explain what it does,
// it's doing too much. Break it down.
//
// Production note: internal helpers (fn, not pub fn) belong here too.
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
// 5. Queries & Accessors
// ──────────────────────────────────────────────────────────────────────────
//
// Read-only methods that expose or derive state. These take &self
// and never modify the type.
//
// In typestate patterns, query methods belong on the post-transition
// type ([ComposedType]) — the compiler prevents querying before the
// transition has validated the data.
//
// Naming conventions:
//   Getters:     .name(), .version()      → return &str or T: Copy
//   Predicates:  .is_valid(), .has_items() → return bool
//   Computed:    .count(), .summary()      → derive from internal state
//   Slices:      .items()                  → return &[T]

// impl [ComposedType] {
//     /// Returns `true` if [condition].
//     pub fn [query_method](&self) -> bool {
//         self.[field]
//     }
//
//     /// Returns [description].
//     pub fn [accessor](&self) -> &str {
//         &self.[field]
//     }
//
//     /// Returns [description].
//     pub fn [slice_accessor](&self) -> &[String] {
//         &self.[field]
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// 6. Output & Display
// ──────────────────────────────────────────────────────────────────────────
//
// Formatting for humans and machines. Output helpers, builder-pattern
// formatters, serialization beyond simple Display (which stays in SETUP).
//
// Includes:
//   - Free functions for terminal output (error, warn, fatal)
//   - Builder-pattern output types (TableBuilder, ReportBuilder)
//   - Serialization helpers (to_json, to_toml)
//   - Custom formatting that goes beyond Display::fmt
//
// Output is the user interface of a library crate. Invest in clear
// formatting — users judge quality by what they see. Use stderr for
// diagnostics, stdout for data. Never mix them.

// /// Prints to stderr and exits with code 1.
// pub fn fatal(msg: &str) -> ! {
//     eprintln!("{msg}");
//     process::exit(1);
// }
//
// /// Prints to stderr and exits with the specified code.
// pub fn fatal_code(code: i32, msg: &str) -> ! {
//     eprintln!("{msg}");
//     process::exit(code);
// }
//
// /// Prints an error message to stderr without exiting.
// pub fn error(msg: &str) {
//     eprintln!("{msg}");
// }
//
// /// Prints a warning to stderr with "Warning: " prefix.
// pub fn warn(msg: &str) {
//     eprintln!("Warning: {msg}");
// }

// impl [HelperType] {
//     /// Creates a [HelperType] from [source].
//     pub fn from_[source]([param]: &[ComposedType]) -> Self {
//         Self {
//             [field]: [param].[accessor]().to_string(),
//             [fields]: Vec::new(),
//         }
//     }
//
//     /// Creates a [HelperType] directly.
//     pub fn new([param]: &str) -> Self {
//         Self {
//             [field]: [param].to_string(),
//             [fields]: Vec::new(),
//         }
//     }
//
//     /// Adds [content] via builder pattern.
//     pub fn [add_method](mut self, [params]) -> Self {
//         self.[fields].push([SubType] {
//             [field]: [value],
//             [field]: [value],
//         });
//         self
//     }
//
//     /// Outputs the formatted [content] to stdout.
//     pub fn [output_method](&self) {
//         // Print formatted output
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────
//
// Module-level public functions not attached to any type. These form
// the crate's top-level API — convenience wrappers, facades, one-shot
// utilities.
//
// When to use free functions vs methods:
//   Method:        operates on a type's data → impl block in §3-§5
//   Free function: combines types, operates on primitives → here
//
// This is also where register_identity() lives when the crate
// participates in the L0 identity system.
//
// If this section grows large, consider whether types would make
// the API clearer. Many free functions = procedural code in disguise.

// /// Registers this crate with the L0 identity system.
// pub fn register_identity() {
//     bereshit_l0_identity::register(
//         pragma_get("I1.key").unwrap_or("unknown"),
//         PRAGMA,
//         METADATA,
//     );
// }

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
//   Cc — Closing Cleanup      Drop impls, resource deallocation
//
// Documentation Sections:
//   X1 — Policy (modification guidelines)       [REQUIRED]
//   X2 — Extension Points (designed growth)      [DEFINED]
//   X3 — Troubleshooting (common problems)       [DEFINED]
//   X4 — Reference (deps, validation, related)   [DEFINED]
//   X5 — Closing Note (scripture anchor)          [REQUIRED]
//   X6 — Template Guide (remove when instantiating) [TEMPLATE-ONLY]
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
// See demo-test.rs for comprehensive patterns (table-driven, edge cases,
// integration workflows).
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
//     // -- Constructor -------------------------------------------------
//
//     #[test]
//     fn new_creates_with_defaults() {
//         let item = [TypeName]::new("[test]");
//         // assert fields
//     }
//
//     // -- Builder -----------------------------------------------------
//
//     #[test]
//     fn builder_chain_configures_all_options() {
//         let item = [TypeName]::new("[test]")
//             .[with_method]("[value]")
//             .[with_feature]();
//         // assert configuration
//     }
//
//     // -- Transition --------------------------------------------------
//
//     #[test]
//     fn transition_produces_composed_type() {
//         let result = [TypeName]::new("[test]")
//             .[transition_method]([args])
//             .unwrap();
//         // assert result state
//     }
//
//     #[test]
//     fn transition_error_on_invalid_input() {
//         let result = [TypeName]::new("[test]")
//             .[transition_method]([bad_args]);
//         assert!(result.is_err());
//         assert!(matches!(result.unwrap_err(), [ErrorType]::[Variant](_)));
//     }
//
//     // -- Queries -----------------------------------------------------
//
//     #[test]
//     fn query_methods_return_expected_values() {
//         let result = [TypeName]::new("[test]")
//             .[transition_method]([args])
//             .unwrap();
//         assert_eq!(result.[accessor](), "[expected]");
//     }
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
//     // -- Error Display -----------------------------------------------
//
//     #[test]
//     fn error_display_formats_correctly() {
//         let err = [ErrorType]::[VariantName]("[test]".to_string());
//         assert_eq!(err.to_string(), "[prefix]: [test]");
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library crates have no entry point — Ce is empty for libraries.
//
// If the library participates in a registration system (like L0 identity),
// the registration call is a public API function in BODY §7 (Free Functions)
// that consumers invoke explicitly — not automatic execution.
//
// Executables use Ce for main() and exit code handling.
// See: executable.rs template.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Drop implementations and resource deallocation. Implement Drop when types
// own resources requiring explicit release (file handles, network connections,
// temporary files, global registrations).
//
// Drop runs automatically when the value goes out of scope. Only implement
// when the default drop (recursively dropping fields) isn't sufficient.

// impl Drop for [TypeName] {
//     fn drop(&mut self) {
//         // Release resources:
//         // - Close file handles
//         // - Flush buffers
//         // - Remove temporary files
//         // - Deregister from global state
//     }
// }

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Don't move the ancient boundary stone which your fathers have set up."
// — Proverbs 22:28
//
// Production: The boundary stone protects architectural invariants. Safe
// changes add capability without breaking contracts. Careful changes affect
// interfaces that dependents rely on. Never-changes break the foundation.
//
// Safe to Modify:
//   - Add new query/accessor methods on [ComposedType]
//   - Add new free functions (output helpers, formatters)
//   - Extend builder with new with_* configuration methods
//   - Add new error variants to [ErrorType]
//   - Add impl blocks for new traits (AsRef, Into, etc.)
//   - Add #[cfg(test)] tests (they don't affect public API)
//
// Modify with Care:
//   - Struct fields — breaks pattern matching, construction, serde
//   - Method signatures — breaks all call sites
//   - Builder method names — breaks chaining patterns at every consumer
//   - Error type shape — breaks match arms in error handlers
//   - Trait implementations (Display, Error, From) — affects error propagation
//   - Return types — existing callers may not handle new variants
//
// Never Modify:
//   - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//   - PRAGMA/METADATA static data layout or accessor signatures
//   - Typestate boundary ([TypeName] → [ComposedType] transition)
//   - Public trait contracts (breaking changes require major version bump)
//   - Module visibility (pub vs pub(crate)) without checking all dependents
//
// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Production: Design for growth by addition, not modification. These are
// the intended seams where the crate is meant to evolve. Adding at a seam
// is safe. Cutting a new seam requires careful review.
//
// Designed Growth Points:
//   - New query methods on [ComposedType] (read-only accessors)
//   - New builder configuration methods (with_* pattern)
//   - New error variants (keep enum variants in logical order)
//   - New trait implementations (Display, From, Into, AsRef)
//   - New free functions for output formatting
//
// Future Considerations:
//   - Serde serialization/deserialization support (feature-gated)
//   - Async variants of long-running operations
//   - Feature-gated optional functionality
//   - FFI bridge (C-compatible interface for libtrit interop)
//
// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lacks wisdom, let him ask of God, who gives to all
// liberally and without reproach, and it will be given to him."
// — James 1:5
//
// Compilation fails after struct change:
//   Struct field changes break pattern matching. Search for the type
//   name across the workspace: `grep -rn TypeName .`
//   Update all construction sites and match arms.
//
// Trait not satisfied:
//   Check that required derives are present (#[derive(...)]).
//   For custom trait impls, verify the type matches exactly
//   (including lifetime parameters and generic bounds).
//
// Lifetime errors in method chains:
//   Builder patterns that return &mut self borrow for the chain's
//   duration. If you need owned values mid-chain, use a consuming
//   builder (fn method(self) -> Self) instead.
//
// Tests pass individually but fail together:
//   Static mutable state or test ordering dependency. Each test
//   must be independent. Use thread_local! or pass state explicitly.
//
// Clippy lint errors after upgrade:
//   New Clippy versions add lints. Fix them — don't suppress without
//   documented reason. `cargo clippy --fix` handles most automatically.
//
// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Dependencies:   [std modules used — e.g., std::fmt, std::error]
// Dependents:     [crates that depend on this one]
// Supersedes:     [prior implementation path, if any]
// Parallel:       [Go/C implementations of same concept]
// Template:       b-word/seed/code/L0/rust/library.rs
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
// The library is a house. Its types are rooms. Its traits are doors.
// Every struct, every method, every error variant — built on the
// foundation that precedes all code: the Word that was in the beginning.
//
// ──────────────────────────────────────────────────────────────────────────
// X6: Template Guide (remove this section when instantiating)
// ──────────────────────────────────────────────────────────────────────────
//
// Instantiation:
//   1.  cp library.rs /path/to/new/crate/src/lib.rs
//   2.  Change pragma: #!omni template → //omni:code --rust -library
//   3.  Update meta.key, meta.from, meta.at for your crate
//   4.  Fill PRAGMA constants (I1-I4) with crate identity
//   5.  Fill METADATA constants (C1-C7) with crate context
//   6.  Replace [TypeName], [ComposedType], [ErrorType] placeholders
//   7.  Implement types, builder, transition, and queries in BODY
//   8.  Uncomment and fill #[cfg(test)] mod tests (see demo-test.rs for patterns)
//   9.  Create Cargo.toml (use seed/code/L0/toml/cargo/ template)
//  10.  Add crate to workspace Cargo.toml members
//  11.  Remove X6 section and all [placeholder] markers
//  12.  Run: cargo clippy -p crate-name -- -D warnings && cargo test -p crate-name
//
// Test template: demo-test.rs has comprehensive test patterns
//   (constructors, builders, transitions, queries, identity, error display,
//    edge cases, table-driven tests, integration tests)
//
// ============================================================================
// END CLOSING
// ============================================================================
