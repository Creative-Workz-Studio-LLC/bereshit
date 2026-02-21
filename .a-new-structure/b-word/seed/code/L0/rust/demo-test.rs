// #!omni template --rust -demo-test
// #!omni meta.key = B-seed-code-L0-rust-demo-test
// #!omni meta.from = b-word/seed/code/L0/rust/root.omni
// #!omni meta.at = template
// ============================================================================
// METADATA
// ============================================================================
//
// TEMPLATE: Rust Demo-Test — 4-Block Structure
//
// Usage:
//   Option A — Inline tests (preferred for unit tests):
//     Tests live inside lib.rs as #[cfg(test)] mod tests { ... }
//     Copy relevant test sections from this template into your lib.rs.
//     See: library.rs template > Tests section at bottom.
//
//   Option B — Integration tests (for external API testing):
//     1. cp demo-test.rs /path/to/crate/tests/name_test.rs
//     2. Change pragma: #!omni template → //omni:code --rust -demo-test
//     3. Update meta.key, meta.from, meta.at for your test file
//     4. Update PRAGMA and METADATA values to describe your tests
//
// Rust test organization (differs from Go):
//   - Rust inline: #[cfg(test)] mod tests in lib.rs (white-box, privates)
//   - Rust integration: tests/ directory (black-box, public API only)
//   - Rust doc-tests: /// blocks with code fences (docs + tests)
//   - Go: separate _test.go files in same package (white-box)
//
// derives_from: b-word/seed/code/L0/rust/root.omni

//! # Rust Demo-Test Template
//!
//! Root 4-block template for Rust test files. Covers both inline
//! unit tests (`#[cfg(test)]`) and integration tests (`tests/` directory).
//!
//! ## Design
//!
//! Tests prove correctness. Rust provides three test levels: doc-tests
//! (in `///` blocks), inline tests (`#[cfg(test)] mod tests`), and
//! integration tests (`tests/` directory). This template covers inline
//! and integration patterns.
//!
//! ## Usage
//!
//! ```bash
//! # Integration test:
//! cp demo-test.rs /path/to/crate/tests/name_test.rs
//! # Change pragma: #!omni template → //omni:code --rust -demo-test
//! # Update PRAGMA and METADATA values for your test file
//!
//! # Inline test:
//! # Copy relevant test sections into your lib.rs #[cfg(test)] module
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
/// assert_eq!(key, "B-seed-code-L0-rust-demo-test");
/// ```
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-seed-code-L0-rust-demo-test"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/root.omni"),
    ("I1.at",        "template"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "demo-test"),
    ("I2.role",      "seed"),
    // I3: Instance
    ("I3.file",      "demo-test.rs"),
    ("I3.title",     "Rust Demo-Test Template"),
    ("I3.component", "Root template for Rust test files"),
    ("I3.path",      "seed/code/L0/rust/demo-test.rs"),
    ("I3.provides",  "RUST_DEMO_TEST_TEMPLATE"),
    ("I3.brief",     "Root 4-block template for Rust test files. Integration and inline test patterns."),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "seed"),
    ("I4.pattern",   "cp demo-test.rs tests/name_test.rs"),
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
    ("C3.scripture",         "1 Thessalonians 5:21 — Prove all things; hold fast that which is good"),
    ("C3.principle",         "Testing proves correctness — hold fast to what works"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none"),
    ("C4.consumers",         "All Rust test files in the project"),
    ("C4.integration",       "cp demo-test.rs /path/to/crate/tests/name_test.rs"),
    ("C4.if_missing",        "Rust tests lack standardized I/C metadata structure"),
    // C5: Intent
    ("C5.purpose",           "Canonical 4-block structure for Rust test files with I/C metadata"),
    ("C5.philosophy",        "Tests prove truth — structured validation over hope"),
    // C6: Roadmap
    ("C6.current",           "a-02.00 — All 4 blocks production-grade"),
    ("C6.planned",           "Rust 4-block linter, schema-driven validation"),
    ("C6.limitations",       "All 4 blocks production-grade (a-02.00)"),
    // C7: Classification
    ("C7.tags",              "template, rust, demo-test, 4-block, seed, omnicode"),
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
// Test SETUP establishes everything the test functions need — imports,
// fixtures, helpers, constants. The BODY contains only #[test] functions
// and their assertions.
//
// If anything must be hardcoded, it lives here — never scattered through BODY.
// When a hardcoded value gets promoted to config, you extract from one place.
//
// Section order (dependency chain — each layer uses only what's above):
//
//   1. Imports         — Crate under test + assertion utilities
//   2. Modules         — Test helper modules (rare)
//   3. Constants       — Expected values, test limits, magic strings
//   4. Statics         — Shared test state (avoid — tests run in parallel)
//   5. Type Aliases    — Result aliases for test ergonomics
//   6. Error Types     — Test-specific error types (rare)
//   7. Core Types      — Test-only data structures (fixtures, builders)
//   8. Trait Defs      — Test-only contracts (rare)
//   9. Macros          — Assertion helpers as macros
//  10. Feature Gates   — Conditional test groups

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────
//
// For inline tests (#[cfg(test)] mod tests):
//   use super::*; imports everything from parent module.
//   Access to private functions and types.
//
// For integration tests (tests/ directory):
//   use [crate_name]::*; imports public API only.
//   Tests the crate as an external consumer would.

// --- For inline tests ---
// use super::*;

// --- For integration tests ---
// use [crate_name]::{[PrimaryType], [ComposedType], [ErrorType]};
// use [crate_name]::{pragma_get, metadata_get};

// ──────────────────────────────────────────────────────────────────────────
// 2. Modules
// ──────────────────────────────────────────────────────────────────────────
//
// Test helper modules. Rarely needed — most test utilities fit inline.
// Use when fixtures or helpers grow large enough to warrant separation.

// mod test_fixtures;
// mod test_helpers;

// ──────────────────────────────────────────────────────────────────────────
// 3. Constants
// ──────────────────────────────────────────────────────────────────────────
//
// Expected values, test boundaries, fixture data. Named constants
// make test assertions self-documenting.

// const EXPECTED_KEY: &str = "[project-key]";
// const EXPECTED_FORMAT: &str = "rust";
// const TEST_TIMEOUT_SECS: u64 = 5;

// ──────────────────────────────────────────────────────────────────────────
// 4. Statics
// ──────────────────────────────────────────────────────────────────────────
//
// Shared test state. AVOID — Rust tests run in parallel by default.
// Use fixture functions returning fresh instances instead.
// Only use statics for truly immutable, thread-safe shared data.

// static TEST_DATA: LazyLock<Vec<&str>> = LazyLock::new(|| {
//     vec!["alpha", "beta", "gamma"]
// });

// ──────────────────────────────────────────────────────────────────────────
// 5. Type Aliases
// ──────────────────────────────────────────────────────────────────────────
//
// Result aliases for test ergonomics. Makes fallible test signatures cleaner.

// type TestResult = std::result::Result<(), [ErrorType]>;

// ──────────────────────────────────────────────────────────────────────────
// 6. Error Types
// ──────────────────────────────────────────────────────────────────────────
//
// Rarely needed — tests typically use the crate's own error types.
// Define here only if tests need error types not exposed by the crate.

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types — Test Fixtures
// ──────────────────────────────────────────────────────────────────────────
//
// Reusable test data. Functions returning fresh instances (not statics)
// to avoid shared mutable state. Rust tests run in parallel by default —
// fixtures must be independent.
//
// Pattern: fixture_*() functions that build known-good test objects.
// Each function is self-contained — no dependency on other fixtures.

// /// Creates a default [TypeName] for testing.
// fn fixture_default() -> [TypeName] {
//     [TypeName]::new("[test-name]")
// }
//
// /// Creates a fully-configured [TypeName] for testing.
// fn fixture_configured() -> [TypeName] {
//     [TypeName]::new("[test-name]")
//         .[with_method]("[test-value]")
//         .[with_feature]()
// }
//
// /// Creates a [ComposedType] for query testing.
// fn fixture_composed() -> [ComposedType] {
//     fixture_configured()
//         .[transition_method]([test_args])
//         .expect("fixture should not fail")
// }

// ──────────────────────────────────────────────────────────────────────────
// 8. Trait Definitions
// ──────────────────────────────────────────────────────────────────────────
//
// Rarely needed in test files. If you're defining test traits,
// consider whether the abstraction belongs in the library crate.

// ──────────────────────────────────────────────────────────────────────────
// 9. Macros — Assertion Helpers
// ──────────────────────────────────────────────────────────────────────────
//
// Custom assertion macros reduce boilerplate across test functions.
// Keep focused — if a macro is complex, it needs its own tests.
//
// Prefer functions for simple helpers. Use macros when you need:
//   - Custom panic messages with caller location
//   - Pattern matching in assertions
//   - Repetitive assert! patterns

// /// Asserts that a Result is Err with a specific error variant.
// fn assert_error_variant(result: Result<[ComposedType], [ErrorType]>, expected: &str) {
//     let err = result.expect_err("expected error");
//     let msg = err.to_string();
//     assert!(
//         msg.contains(expected),
//         "error message '{msg}' should contain '{expected}'"
//     );
// }

// ──────────────────────────────────────────────────────────────────────────
// 10. Feature Gates
// ──────────────────────────────────────────────────────────────────────────
//
// Conditional test groups. Tests for feature-gated functionality.

// #[cfg(feature = "serde")]
// mod serde_tests {
//     use super::*;
//     // Tests for serialization/deserialization behavior...
// }

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// Sections not present in this template. Derived files that don't need
// an active section move it here instead of deleting it.
//
// (none — all 10 SETUP sections active in demo-test form)
//
// To reserve a section: move its header and content here, replace the
// content with a brief note, and reference bare-bone.rs for the master list.

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// BODY makes things HAPPEN. SETUP made things EXIST.
//
// Test BODY is organized by concern — each subsection tests a different
// aspect of the crate's behavior. This mirrors how the crate is built:
// constructors first, then configuration, then operations, then queries.
//
// Rust test patterns:
//   #[test] fn name() { ... }                    — basic test
//   #[test] fn name() -> Result<(), E> { ... }   — fallible test (uses ?)
//   #[should_panic(expected = "msg")]             — panic test
//   assert!(condition)                            — boolean assertion
//   assert_eq!(left, right)                       — equality
//   assert_ne!(left, right)                       — inequality
//   assert!(matches!(val, Pattern))               — pattern matching
//
// Table-driven tests (Rust equivalent of Go pattern):
//   let cases = vec![("name", input, expected), ...];
//   for (name, input, expected) in cases {
//       assert_eq!(func(input), expected, "case: {name}");
//   }
//
// Subsection order follows concern coverage — from construction through
// the full type lifecycle to integration. Present subsections appear
// in this sequence. Not all 9 are required — test what exists.
//
//   1. Constructor Tests    — Verify default state and creation
//   2. Builder Tests        — Verify configuration methods
//   3. Transition Tests     — Verify state transitions and validation
//   4. Query Tests          — Verify post-transition accessors
//   5. Identity Tests       — Verify PRAGMA/METADATA integrity
//   6. Error Tests          — Verify error formatting and conditions
//   7. Edge Case Tests      — Unicode, empty, long, boundary inputs
//   8. Table-Driven Tests   — Systematic input/output validation
//   9. Integration Tests    — End-to-end workflows

// ──────────────────────────────────────────────────────────────────────────
// 1. Constructor Tests
// ──────────────────────────────────────────────────────────────────────────
//
// Verify that new() creates types with correct default state.
// Test required parameters, optional fields (should be None/false),
// and edge cases like empty strings.
//
// These establish the baseline — if construction is wrong,
// every subsequent test is meaningless.

// #[test]
// fn new_creates_with_defaults() {
//     let item = [TypeName]::new("[test]");
//     assert_eq!(item.[field], "[test]");
//     assert!(item.[optional_field].is_none());
//     assert!(!item.[bool_field]);
// }
//
// #[test]
// fn new_with_empty_name() {
//     let item = [TypeName]::new("");
//     assert_eq!(item.[field], "");
//     // Empty name is valid — downstream validation catches it
// }

// ──────────────────────────────────────────────────────────────────────────
// 2. Builder Tests
// ──────────────────────────────────────────────────────────────────────────
//
// Verify that with_*() methods configure the type correctly.
// Test each method individually and in combination (full chain).
//
// Builder methods consume self — verify that chaining works and
// that each method only affects its own field.

// #[test]
// fn builder_sets_optional_field() {
//     let item = [TypeName]::new("[test]")
//         .[with_method]("[value]");
//     assert_eq!(item.[optional_field].as_deref(), Some("[value]"));
// }
//
// #[test]
// fn builder_enables_feature() {
//     let item = [TypeName]::new("[test]")
//         .[with_feature]();
//     assert!(item.[bool_field]);
// }
//
// #[test]
// fn builder_chain_configures_all_options() {
//     let item = [TypeName]::new("[test]")
//         .[with_method]("[value]")
//         .[with_feature]();
//     assert_eq!(item.[optional_field].as_deref(), Some("[value]"));
//     assert!(item.[bool_field]);
// }

// ──────────────────────────────────────────────────────────────────────────
// 3. Transition Tests
// ──────────────────────────────────────────────────────────────────────────
//
// Verify the typestate transition — builder → validated type.
// Test both success paths (valid input) and error paths (invalid input).
//
// These are the most important tests — the transition is where
// validation happens and the type's guarantees are established.

// #[test]
// fn transition_with_empty_input() {
//     let result = [TypeName]::new("[test]")
//         .[transition_method]([empty_args])
//         .unwrap();
//     assert!(!result.[query_method]());
//     assert!(result.[slice_accessor]().is_empty());
// }
//
// #[test]
// fn transition_with_valid_input() {
//     let result = [TypeName]::new("[test]")
//         .[transition_method]([valid_args])
//         .unwrap();
//     assert!(result.[query_method]());
// }
//
// #[test]
// fn transition_error_on_invalid_input() {
//     let result = [TypeName]::new("[test]")
//         .[transition_method]([invalid_args]);
//     assert!(result.is_err());
//     assert!(matches!(
//         result.unwrap_err(),
//         [ErrorType]::[VariantName](_)
//     ));
// }
//
// #[test]
// fn transition_with_mixed_input() {
//     let result = [TypeName]::new("[test]")
//         .[with_feature]()
//         .[transition_method]([mixed_args])
//         .unwrap();
//     assert!(result.[query_a]());
//     assert!(result.[query_b]());
//     assert_eq!(result.[slice_accessor](), &["[expected]"]);
// }

// ──────────────────────────────────────────────────────────────────────────
// 4. Query & Accessor Tests
// ──────────────────────────────────────────────────────────────────────────
//
// Verify that post-transition accessors return correct values.
// Test getters, predicates, and computed properties.
//
// These verify the "read" side — that the validated type exposes
// its state correctly through its public API.

// #[test]
// fn accessors_expose_configuration() {
//     let result = [TypeName]::new("[name]")
//         .[with_method]("[value]")
//         .[transition_method]([args])
//         .unwrap();
//     assert_eq!(result.[accessor](), "[name]");
//     assert_eq!(result.[optional_accessor](), Some("[value]"));
// }
//
// #[test]
// fn feature_enabled_reflects_builder() {
//     let without = [TypeName]::new("[t]")
//         .[transition_method]([args]).unwrap();
//     assert!(!without.[feature_check]());
//
//     let with = [TypeName]::new("[t]")
//         .[with_feature]()
//         .[transition_method]([args]).unwrap();
//     assert!(with.[feature_check]());
// }

// ──────────────────────────────────────────────────────────────────────────
// 5. Identity Tests
// ──────────────────────────────────────────────────────────────────────────
//
// Verify PRAGMA and METADATA static data integrity. Every crate must
// have correct identity — these tests catch typos, stale values,
// and missing fields.
//
// Key checks: I1.key matches crate, I1.format is correct,
// missing keys return None, grounding scripture is present.

// #[test]
// fn pragma_contains_key() {
//     assert_eq!(pragma_get("I1.key"), Some("[project-key]"));
// }
//
// #[test]
// fn pragma_contains_format() {
//     assert_eq!(pragma_get("I1.format"), Some("rust"));
// }
//
// #[test]
// fn pragma_get_returns_none_for_missing() {
//     assert_eq!(pragma_get("I99.nonexistent"), None);
// }
//
// #[test]
// fn metadata_contains_version() {
//     assert!(metadata_get("C1.version").is_some());
// }
//
// #[test]
// fn metadata_contains_grounding() {
//     let scripture = metadata_get("C3.scripture").unwrap();
//     assert!(scripture.contains("[expected text]"));
// }

// ──────────────────────────────────────────────────────────────────────────
// 6. Error Tests
// ──────────────────────────────────────────────────────────────────────────
//
// Verify error type Display formatting and error conditions.
// Each error variant should produce a clear, actionable message.
//
// Pattern: construct error → assert Display output matches expected.
// This ensures error messages don't regress during refactoring.

// #[test]
// fn error_display_variant_a() {
//     let err = [ErrorType]::[VariantA]("[test]".to_string());
//     assert_eq!(err.to_string(), "[prefix]: [test]");
// }
//
// #[test]
// fn error_display_variant_b() {
//     let err = [ErrorType]::[VariantB]("[test]".to_string());
//     assert_eq!(err.to_string(), "[prefix]: [test]");
// }
//
// #[test]
// fn error_display_generic() {
//     let err = [ErrorType]::[GenericVariant]("[msg]".to_string());
//     assert_eq!(err.to_string(), "[prefix]: [msg]");
// }

// ──────────────────────────────────────────────────────────────────────────
// 7. Edge Case Tests
// ──────────────────────────────────────────────────────────────────────────
//
// Boundary conditions, unusual inputs, stress cases. These catch
// assumptions that work for typical data but fail at the edges.
//
// Standard edge cases: empty string, very long input, Unicode,
// special characters, null-like values, max/min bounds.
//
// Production note: if an edge case reveals a bug, add the fix AND
// keep the test. Edge case tests are insurance against regression.

// #[test]
// fn handles_unicode_input() {
//     let result = [TypeName]::new("[unicode-test]")
//         .[transition_method]([unicode_args])
//         .unwrap();
//     // Verify Unicode passes through correctly
// }
//
// #[test]
// fn handles_empty_string_fields() {
//     let result = [TypeName]::new("")
//         .[transition_method]([args])
//         .unwrap();
//     assert_eq!(result.[accessor](), "");
// }
//
// #[test]
// fn handles_very_long_input() {
//     let long = "x".repeat(10_000);
//     let result = [TypeName]::new(&long)
//         .[transition_method]([args])
//         .unwrap();
//     assert_eq!(result.[accessor](), long);
// }

// ──────────────────────────────────────────────────────────────────────────
// 8. Table-Driven Tests
// ──────────────────────────────────────────────────────────────────────────
//
// Systematic input/output validation using Vec of test cases.
// Each case has a name, input, and expected output.
//
// Use table-driven tests when:
//   - Multiple inputs should produce different outputs
//   - You're testing a mapping or transformation
//   - The logic is the same, only the data varies
//
// Name each case clearly — "case: {name}" in assert messages
// makes failures immediately diagnosable.

// #[test]
// fn table_driven_[operation]() {
//     let cases = vec![
//         ("empty input",    vec![],                           [expected_a]),
//         ("single item",    vec!["[a]".to_string()],          [expected_b]),
//         ("multiple items", vec!["[a]".into(), "[b]".into()], [expected_c]),
//     ];
//
//     for (name, input, expected) in cases {
//         let result = [TypeName]::new("[test]")
//             .[transition_method](&input)
//             .unwrap_or_else(|e| panic!("case '{name}': {e}"));
//         assert_eq!(
//             result.[query](), expected,
//             "failed case: {name}"
//         );
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// 9. Integration Tests
// ──────────────────────────────────────────────────────────────────────────
//
// End-to-end workflows testing the crate as an external consumer would.
// For tests/ directory files, these use only the public API.
// For inline tests, these exercise the full construct → configure →
// transition → query → output pipeline.
//
// Integration tests are the ultimate proof — if they pass, the crate
// works as documented. If they fail, something in the pipeline broke.

// #[test]
// fn end_to_end_[operation]() {
//     // Full workflow: construct -> configure -> transition -> query
//     let result = [crate_name]::[TypeName]::new("[test]")
//         .[with_method]("[value]")
//         .[transition_method]([args])
//         .expect("end-to-end should succeed");
//
//     assert!(result.[query_method]());
//     assert_eq!(result.[accessor](), "[expected]");
// }
//
// #[test]
// fn output_helper_does_not_panic() {
//     let result = [crate_name]::[TypeName]::new("[test]")
//         .[transition_method]([args])
//         .unwrap();
//
//     // Output helpers should not panic even with unusual data
//     let helper = [crate_name]::[HelperType]::from_[source](&result)
//         .[add_method]("[title]", &["[line1]", "[line2]"]);
//     // helper.print() would write to stdout — just verify construction
//     assert!(!helper.[fields].is_empty());
// }

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// Sections not present in this template. Derived files that don't need
// an active section move it here instead of deleting it.
//
// (none — all 9 BODY sections active in demo-test form)
//
// To reserve a section: move its header and content here, replace the
// content with a brief note, and reference bare-bone.rs for the master list.

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// CLOSING = Validation → Execution → Cleanup. For test files, the BODY
// is itself validation — the CLOSING code zones handle meta-validation,
// execution (empty for tests), and fixture cleanup.
//
// Code Zones (order: Validation → Execution → Cleanup):
//   Cv — Closing Validation   Meta-tests (verify test infrastructure)
//   Ce — Closing Execution    Empty — test files have no entry point
//   Cc — Closing Cleanup      Test fixture teardown, temp file removal
//
// Documentation Sections:
//   X1 — Policy (what's safe to change in tests)  [REQUIRED]
//   X2 — Extension Points (where tests grow)       [DEFINED]
//   X3 — Troubleshooting (when tests fail)         [DEFINED]
//   X4 — Reference (running, coverage, related)    [DEFINED]
//   X5 — Closing Note (scripture anchor)            [REQUIRED]
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
// Meta-validation: tests that verify the test infrastructure itself.
// These catch problems with fixtures, test helpers, and shared setup
// before they silently corrupt other test results.
//
// For test files, the BODY IS validation — Cv handles the layer above:
// "do the test tools themselves work?"

// #[test]
// fn fixture_default_creates_valid_instance() {
//     let item = fixture_default();
//     // Verify the fixture produces a valid instance
//     // If this fails, all tests using fixture_default() are suspect
// }
//
// #[test]
// fn fixture_composed_does_not_panic() {
//     let _composed = fixture_composed();
//     // If fixture_composed() panics, transition tests are all broken
// }
//
// #[test]
// fn assert_error_variant_catches_wrong_variant() {
//     // Verify the custom assertion helper works correctly
//     // If this is wrong, error tests give false confidence
// }

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Test files have no entry point — Ce is empty for tests.
//
// Tests are invoked by `cargo test`, not by main(). The test harness
// handles discovery, execution, parallelism, and reporting.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Test fixture teardown and temporary resource cleanup. Rust's ownership
// system handles most cleanup automatically (Drop). Explicit cleanup is
// needed only for external resources (temp files, test databases, network
// connections).
//
// Pattern: cleanup functions paired with fixtures. Each fixture that
// creates external resources has a matching cleanup function.

// fn cleanup_temp_dir(path: &std::path::Path) {
//     if path.exists() {
//         std::fs::remove_dir_all(path)
//             .expect("test cleanup: failed to remove temp dir");
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// Sections not present in this template. Derived files that don't need
// an active section move it here instead of deleting it.
//
// (none — all CLOSING code zones and doc sections active in demo-test form)
//
// To reserve a section: move its header and content here, replace the
// content with a brief note, and reference bare-bone.rs for the master list.

//
// ──────────────────────────────────────────────────────────────────────────
// Test Coverage Summary
// ──────────────────────────────────────────────────────────────────────────
//
// Production: Update this summary when adding or removing test sections.
// It's the first thing a reviewer reads — make it honest.
//
// [N] constructor tests     — verify default state after new()
// [N] builder tests         — verify configuration methods chain correctly
// [N] transition tests      — verify state transition and error handling
// [N] query tests           — verify post-transition accessors
// [N] identity tests        — verify PRAGMA/METADATA integrity
// [N] error display tests   — verify error formatting consistency
// [N] edge case tests       — Unicode, empty, long, boundary inputs
// [N] table-driven tests    — systematic input/output validation
// [N] integration tests     — end-to-end workflows
//
// Total: [N] tests
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Don't move the ancient boundary stone which your fathers have set up."
// — Proverbs 22:28
//
// Production: Tests are the boundary stones of behavior. Removing a test
// that passes is removing proof that something works. Adding tests is
// always safe. Changing test expectations means the contract changed.
//
// Safe to Modify:
//   - Add new test functions (always additive)
//   - Add new test sections matching new BODY sections
//   - Add more assertions to existing tests (stricter is safe)
//   - Add new edge case inputs to table-driven tests
//   - Improve test names for clarity
//   - Add comments explaining WHY a test exists
//
// Modify with Care:
//   - Test expectations (assert values) — means behavior changed
//   - Removing a passing test — loses proof of correct behavior
//   - Test helpers shared across sections — changes cascade
//   - Table-driven test data — wrong data produces false confidence
//
// Never Modify:
//   - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//   - Identity tests (PRAGMA/METADATA integrity is non-negotiable)
//   - #[cfg(test)] gate (removing it ships test code to production)
//   - Test independence (tests must not depend on execution order)
//
// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Production: Test coverage grows with implementation. Each new BODY
// section in the library should have a corresponding test section here.
//
// Designed Growth Points:
//   - New test section per new BODY section (matching comment dividers)
//   - New edge cases per new validation rule (boundary testing)
//   - New table-driven entries per new input format (systematic)
//   - New integration tests per new end-to-end workflow
//   - Property-based tests (proptest) for invariant verification
//
// Future Considerations:
//   - Benchmark tests (#[bench] or criterion) for performance tracking
//   - Snapshot tests (insta) for complex output verification
//   - Fuzzing targets for security-sensitive parsing
//   - Coverage gating in CI (minimum coverage threshold)
//
// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lacks wisdom, let him ask of God, who gives to all
// liberally and without reproach, and it will be given to him."
// — James 1:5
//
// Test fails after code change:
//   The test is telling you the contract changed. Either update the
//   test to match the new contract (if the change is intentional) or
//   revert the code change (if the test caught a bug). Don't delete
//   the test.
//
// Tests pass individually, fail together:
//   Test ordering dependency or shared mutable state. Rust runs tests
//   in parallel by default. Use `cargo test -- --test-threads=1` to
//   isolate. Fix: make each test fully independent.
//
// Test output clutters terminal:
//   Use `-- --nocapture` only when debugging specific tests.
//   Normal runs suppress stdout/stderr for passing tests. If a test
//   needs to verify output, capture it programmatically.
//
// Table-driven test gives confusing error:
//   Add case names/labels to your test data. On failure, print the
//   case label: `assert_eq!(result, expected, "case: {label}")`.
//   Makes the failing case immediately identifiable.
//
// Integration test can't find crate:
//   Integration tests (tests/ directory) import via `use crate_name::*`.
//   The crate name uses underscores, not hyphens. Check that the
//   crate is building successfully before running integration tests.
//
// Doc-test fails but unit test passes:
//   Doc-tests run in a fresh scope with only public API. If your
//   doc example uses private items, it will fail. Doc-tests are
//   a consumer's-eye-view — that's the point.
//
// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Test Targets:    [crate being tested]
// Dependencies:    [test-only crates — e.g., proptest, criterion]
// Template:        b-word/seed/code/L0/rust/demo-test.rs
//
// Running Tests:
//   All tests:     cargo test -p [crate-name]
//   Verbose:       cargo test -p [crate-name] -- --nocapture
//   Single test:   cargo test -p [crate-name] [test_name]
//   Doc-tests:     cargo test -p [crate-name] --doc
//   Integration:   cargo test -p [crate-name] --test [test_file]
//   Sequential:    cargo test -p [crate-name] -- --test-threads=1
//
// Test Organization:
//
//   Inline tests (#[cfg(test)] mod tests):
//     Unit tests with access to private internals. Live in src/lib.rs
//     alongside implementation. Organized by concern matching BODY.
//
//   Integration tests (tests/ directory):
//     Black-box tests using only public API. Each file compiles as a
//     separate crate. Tests the crate as consumers will use it.
//
//   Doc-tests (/// code blocks):
//     Documentation AND tests in one. Run with cargo test --doc.
//     Verify that examples in docs actually compile and work.
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// "Prove all things; hold fast that which is good."
// — 1 Thessalonians 5:21
//
// Tests are proof. Not proof of perfection — proof that we checked.
// Every assertion is a question asked honestly and answered concretely.
// The test that catches a bug before production is a small act of
// faithfulness. Hold fast to what proves good.
//
// ──────────────────────────────────────────────────────────────────────────
// X6: Template Guide (remove this section when instantiating)
// ──────────────────────────────────────────────────────────────────────────
//
// Instantiation:
//   1.  cp demo-test.rs /path/to/new/crate/src/lib.rs (inline tests)
//       — OR: cp sections into tests/[test_name].rs (integration tests)
//   2.  Change pragma: #!omni template → //omni:code --rust -test
//   3.  Update meta.key, meta.from, meta.at for your test file
//   4.  Replace [TypeName], [ComposedType], [ErrorType] with real types
//   5.  Replace [crate_name] with the actual crate name (underscores)
//   6.  Fill test data in table-driven tests with real cases
//   7.  Add/remove test sections to match your crate's BODY sections
//   8.  Update Test Coverage Summary with actual counts
//   9.  Remove X6 section and all [placeholder] markers
//  10.  Run: cargo test -p crate-name
//
// Choosing inline vs integration:
//   Inline (#[cfg(test)] mod tests) — when you need private access
//   Integration (tests/ directory) — when testing public API as a consumer
//   Both — most crates benefit from both. Start inline, add integration
//   when the public API stabilizes.
//
// ============================================================================
// END CLOSING
// ============================================================================
