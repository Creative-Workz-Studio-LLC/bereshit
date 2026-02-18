// #!omni template --rust -executable
// #!omni meta.key = B-seed-code-L0-rust-executable
// #!omni meta.from = b-word/seed/code/L0/rust/root.omni
// #!omni meta.at = template
// ============================================================================
// METADATA
// ============================================================================
//
// TEMPLATE: Rust Executable — 4-Block Structure
//
// Usage:
//   1. cp executable.rs /path/to/new/crate/src/main.rs
//   2. Change pragma: #!omni template → //omni:code --rust -executable
//   3. Update meta.key, meta.from, meta.at for your crate
//   4. Update PRAGMA and METADATA values to describe your crate
//   5. Create Cargo.toml with [[bin]] section or default binary
//   6. Add crate to workspace Cargo.toml members
//
// derives_from: b-word/seed/code/L0/rust/root.omni

//! # Rust Executable Template
//!
//! Root 4-block template for Rust executables. All Rust binaries
//! in the Bereshit project derive from this template.
//!
//! ## Design
//!
//! Executables orchestrate — they parse arguments, load configuration,
//! call library functions, and handle exit codes. The `run()` pattern
//! separates orchestration (fallible) from entry (`main()`).
//!
//! ## Usage
//!
//! ```bash
//! cp executable.rs /path/to/new/crate/src/main.rs
//! # Change pragma: #!omni template → //omni:code --rust -executable
//! # Update PRAGMA and METADATA values for your crate
//! ```
//!
//! ## Exit Codes
//!
//! | Code | Meaning |
//! |------|---------|
//! | 0 | Success |
//! | 1 | General error |
//! | 2 | Usage/argument error |
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
/// assert_eq!(key, "B-seed-code-L0-rust-executable");
/// ```
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-seed-code-L0-rust-executable"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/root.omni"),
    ("I1.at",        "template"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "executable"),
    ("I2.role",      "seed"),
    // I3: Instance
    ("I3.file",      "executable.rs"),
    ("I3.title",     "Rust Executable Template"),
    ("I3.component", "Root template for Rust executables"),
    ("I3.path",      "seed/code/L0/rust/executable.rs"),
    ("I3.provides",  "RUST_EXECUTABLE_TEMPLATE"),
    ("I3.brief",     "Root 4-block template for Rust executables. All Rust binaries derive from this."),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "seed"),
    ("I4.pattern",   "cp executable.rs src/main.rs"),
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
    ("C3.scripture",         "Ecclesiastes 9:10 — Whatsoever thy hand findeth to do, do it with thy might"),
    ("C3.principle",         "Executables are action — orchestrate with excellence and clear purpose"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "none"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none"),
    ("C4.consumers",         "All Rust executable crates in the project"),
    ("C4.integration",       "cp executable.rs /path/to/crate/src/main.rs"),
    ("C4.if_missing",        "Rust executables lack standardized I/C metadata structure"),
    // C5: Intent
    ("C5.purpose",           "Canonical 4-block structure for Rust executables with I/C metadata"),
    ("C5.philosophy",        "Executables orchestrate; libraries compute — clean separation of concerns"),
    // C6: Roadmap
    ("C6.current",           "a-02.00 — All 4 blocks production-grade"),
    ("C6.planned",           "Rust 4-block linter, schema-driven validation"),
    ("C6.limitations",       "All 4 blocks production-grade (a-02.00)"),
    // C7: Classification
    ("C7.tags",              "template, rust, executable, 4-block, seed, omnicode"),
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
// Executables typically need env, process, io from std and internal crate APIs.

// --- Standard Library ---
// use std::env;
// use std::process;
// use std::io::{self, Write};
// use std::fmt;

// --- External Crates ---
// [Currently none — uses internal crates for all functionality]

// --- Internal Crates ---
// use bereshit_l0_cli::App;
// use bereshit_l0_config;

// --- Crate-Internal ---
// [Binary crate — no sub-imports]

// ──────────────────────────────────────────────────────────────────────────
// 2. Modules
// ──────────────────────────────────────────────────────────────────────────
//
// Submodule declarations. Executables rarely have submodules —
// logic lives in library crates, executables orchestrate.
// If the binary grows complex, extract modules for arg parsing,
// output formatting, etc.

// mod output;
// mod commands;

// ──────────────────────────────────────────────────────────────────────────
// 3. Constants
// ──────────────────────────────────────────────────────────────────────────
//
// Compile-time fixed values. This is the executable's primary data section.
// Exit codes, version strings, defaults, limits — all live here.
// SCREAMING_SNAKE_CASE. Prefer named constants over raw literals in BODY.

// --- Exit Codes ---
// const EXIT_SUCCESS: i32 = 0;
// const EXIT_ERROR: i32 = 1;
// const EXIT_USAGE: i32 = 2;

// --- Version ---
// const VERSION: &str = env!("CARGO_PKG_VERSION");
// const NAME: &str = env!("CARGO_PKG_NAME");

// --- Defaults ---
// const DEFAULT_[THING]: &str = "[value]";

// ──────────────────────────────────────────────────────────────────────────
// 4. Statics
// ──────────────────────────────────────────────────────────────────────────
//
// Runtime-initialized fixed values. Rarely needed in executables —
// prefer constants or passing state through function arguments.
// NOTE: PRAGMA and METADATA identity statics live in the METADATA block.

// static GLOBAL_CONFIG: LazyLock<Config> = LazyLock::new(|| {
//     Config::from_env().unwrap_or_default()
// });

// ──────────────────────────────────────────────────────────────────────────
// 5. Type Aliases
// ──────────────────────────────────────────────────────────────────────────
//
// Shorthand for complex types. Useful in executables for Result aliases
// that carry the app-specific error type.

// type Result<T> = std::result::Result<T, AppError>;

// ──────────────────────────────────────────────────────────────────────────
// 6. Error Types
// ──────────────────────────────────────────────────────────────────────────
//
// Executable errors typically WRAP library errors with From impls.
// May be lightweight if library errors are sufficient on their own.
//
// Pattern: enum wrapping library errors → Display for user messages →
//          From impls for automatic ? conversion in run().

// /// Top-level errors for this executable.
// #[derive(Debug)]
// enum AppError {
//     /// CLI parsing failed.
//     Cli(bereshit_l0_cli::CliError),
//     /// Configuration error.
//     Config(String),
//     /// I/O error.
//     Io(io::Error),
// }
//
// impl fmt::Display for AppError {
//     fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
//         match self {
//             Self::Cli(e) => write!(f, "argument error: {e}"),
//             Self::Config(msg) => write!(f, "config error: {msg}"),
//             Self::Io(e) => write!(f, "I/O error: {e}"),
//         }
//     }
// }
//
// impl From<bereshit_l0_cli::CliError> for AppError {
//     fn from(e: bereshit_l0_cli::CliError) -> Self { Self::Cli(e) }
// }
//
// impl From<io::Error> for AppError {
//     fn from(e: io::Error) -> Self { Self::Io(e) }
// }

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────
//
// Minimal — executables CONSUME types from library crates.
// Only define types unique to this binary: runtime config, arg containers,
// output state. If a type is reusable, it belongs in a library crate.

// /// Runtime configuration parsed from arguments and environment.
// struct Config {
//     [field]: String,
//     [field]: bool,
// }

// ──────────────────────────────────────────────────────────────────────────
// 8. Trait Definitions
// ──────────────────────────────────────────────────────────────────────────
//
// Rarely needed in executables. If you're defining traits here,
// consider whether the abstraction belongs in a library crate instead.

// ──────────────────────────────────────────────────────────────────────────
// 9. Macros
// ──────────────────────────────────────────────────────────────────────────
//
// Rarely needed in executables. If you're defining macros here,
// consider whether they belong in a shared utility crate instead.

// ──────────────────────────────────────────────────────────────────────────
// 10. Feature Gates
// ──────────────────────────────────────────────────────────────────────────
//
// Conditional compilation items. Feature-gated behavior for this binary.
// Common: optional JSON output, verbose mode, debug features.

// #[cfg(feature = "json")]
// use serde_json;

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// BODY makes things HAPPEN. SETUP made things EXIST.
//
// Executable BODY is orchestration — parsing arguments, calling library
// functions, handling output, managing exit codes. The actual computation
// lives in library crates; executables wire it together.
//
// The run() pattern separates orchestration (fallible, returns Result)
// from entry (main(), handles exit codes). This keeps business logic
// testable — run() can be tested with Result assertions; main() only
// handles the OS interface.
//
// Subsection order follows execution flow — the code reads in the order
// it runs. Present subsections appear in this sequence.
//
//   1. Identity Access    — OmniCode static accessor functions
//   2. Helpers            — Argument parsing, config loading, utilities
//   3. Core Operations    — The run() function and main business logic
//
// Entry point (main) and error-to-exit-code mapping live in CLOSING Ce.

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────
//
// OmniCode static accessor functions for PRAGMA and METADATA.
// Same four functions as library crates — executables have identity too.
// Optional for binaries that don't need runtime identity access,
// but recommended for consistency across the project.

// pub fn pragma() -> &'static [(&'static str, &'static str)] { PRAGMA }
// pub fn pragma_get(key: &str) -> Option<&'static str> {
//     PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
// }
// pub fn metadata() -> &'static [(&'static str, &'static str)] { METADATA }
// pub fn metadata_get(key: &str) -> Option<&'static str> {
//     METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
// }

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
//
// Support functions for the core operation. Argument parsing, config
// loading, output formatting, file I/O wrappers.
//
// Helpers are private (fn, not pub fn) because they serve run() — they
// aren't part of the binary's external interface.
//
// Keep helpers focused. Each one does one thing that run() needs.
// If a helper grows complex, it probably belongs in a library crate.

// /// Parses command-line arguments into runtime configuration.
// fn parse_args() -> Result<Config, AppError> {
//     let parsed = App::new(NAME, "[description]")
//         .with_biblical("[Book Chapter:Verse]")
//         .with_json()
//         .parse_os()?;
//
//     if parsed.wants_help() {
//         bereshit_l0_cli::Usage::from_parsed(&parsed)
//             .add_section("COMMANDS", &[
//                 "[command]  [description]",
//             ])
//             .add_section("OPTIONS", &[
//                 "--json     Output as JSON",
//                 "--help     Show this message",
//             ])
//             .print();
//         process::exit(0);
//     }
//
//     Ok(Config {
//         [field]: parsed.[accessor]().to_string(),
//         [field]: parsed.wants_json(),
//     })
// }

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────
//
// The run() function — the executable's main business logic. Separated
// from main() so errors can flow as Result<(), AppError> instead of
// requiring process::exit() at every failure point.
//
// Structure: Initialize → Execute → Output → Return
//
// run() is the longest function in the file. If it exceeds ~50 lines,
// extract steps into helpers (§2). run() should read like a recipe:
// each step is clear, ordered, and self-documenting.
//
// Production note: run() is where ? shines. Every operation that can
// fail uses ? to propagate errors up to main()'s exit code handler.

// /// Executes the main operation.
// ///
// /// Separated from main() so errors can be returned as Result
// /// instead of requiring process::exit() at every failure point.
// fn run() -> Result<(), AppError> {
//     let config = parse_args()?;
//
//     // 1. Initialize
//     // ...
//
//     // 2. Execute core operation
//     // let result = do_work(&config)?;
//
//     // 3. Output results
//     // if config.json {
//     //     println!("{}", serde_json::to_string_pretty(&result)?);
//     // } else {
//     //     println!("{result}");
//     // }
//
//     Ok(())
// }

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// CLOSING = Validation → Execution → Cleanup. For executables, the
// execution flow is critical — users and scripts depend on argument names,
// exit codes, and output format. CLOSING proves it works (Cv), runs it
// (Ce), and cleans up after (Cc).
//
// Code Zones (order: Validation → Execution → Cleanup):
//   Cv — Closing Validation   #[cfg(test)] inline tests
//   Ce — Closing Execution    exit_code() + main() entry point
//   Cc — Closing Cleanup      Process cleanup, temp file removal
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
// Inline tests for the executable. Test argument parsing, exit codes,
// output format, and error handling. run() returns Result — test it
// with assert_eq! and assert!(matches!). main() calls process::exit()
// — not directly testable, but trivial by design.
//
// For executables, focus on:
//   - Argument parsing correctness (valid/invalid inputs)
//   - Exit code mapping (each AppError variant → correct code)
//   - Output format consistency (text vs JSON)
//   - Error message clarity and actionability

// #[cfg(test)]
// mod tests {
//     use super::*;
//
//     // -- Exit Codes ---------------------------------------------------
//
//     #[test]
//     fn exit_code_cli_error_returns_usage() {
//         let err = AppError::Cli(bereshit_l0_cli::CliError::from("[test]"));
//         assert_eq!(exit_code(&err), EXIT_USAGE);
//     }
//
//     #[test]
//     fn exit_code_config_error_returns_error() {
//         let err = AppError::Config("[test]".to_string());
//         assert_eq!(exit_code(&err), EXIT_ERROR);
//     }
//
//     // -- Error Display -----------------------------------------------
//
//     #[test]
//     fn error_display_cli() {
//         let err = AppError::Cli(bereshit_l0_cli::CliError::from("[test]"));
//         assert!(err.to_string().contains("argument error"));
//     }
//
//     #[test]
//     fn error_display_config() {
//         let err = AppError::Config("[test]".to_string());
//         assert_eq!(err.to_string(), "config error: [test]");
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
//     fn pragma_get_returns_none_for_missing() {
//         assert_eq!(pragma_get("I1.nonexistent"), None);
//     }
//
//     #[test]
//     fn metadata_contains_version() {
//         assert!(metadata_get("C1.version").is_some());
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Entry point and exit code handling. main() delegates to run() (BODY §3)
// and maps the Result to an exit code. This is the executable's contract
// with the world.
//
// Execution flow:
//   1. main() calls run()                  — thin entry, no logic
//   2. run() parses arguments via CLI      — clap, manual, or custom
//   3. [Core operation description]        — the actual work
//   4. Output results (text or JSON)       — format depends on --output flag
//   5. Return Ok(()) or Err(AppError)      — error carries context
//   6. main() maps error to exit code      — 0=success, 1=error, 2=usage
//
// Exit codes are the executable's return value contract:
//   0 = success
//   1 = general error
//   2 = usage/argument error
//
// Scripts and CI depend on these — changing them is a breaking change.

// /// Maps an error to its appropriate exit code.
// fn exit_code(err: &AppError) -> i32 {
//     match err {
//         AppError::Cli(_) => EXIT_USAGE,
//         AppError::Config(_) => EXIT_ERROR,
//         AppError::Io(_) => EXIT_ERROR,
//     }
// }

// /// Thin entry point — delegates to [`run`] and handles exit codes.
// ///
// /// Never put business logic here. If you're tempted to add "just one
// /// more thing" before or after run(), it belongs in run().
// fn main() {
//     if let Err(e) = run() {
//         eprintln!("Error: {e}");
//         process::exit(exit_code(&e));
//     }
// }

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Process cleanup for the executable. Runs after main logic completes.
// Most executables don't need explicit cleanup — the OS reclaims resources
// on exit. Implement only when the binary creates artifacts that should
// be cleaned up on both success and failure paths.
//
// Common cleanup tasks:
//   - Remove temporary files/directories
//   - Flush output buffers (eprintln! flushes on newline; explicit flush
//     only needed for partial lines or buffered stdout)
//   - Close database connections gracefully
//   - Deregister from service discovery

// fn cleanup() {
//     // Remove temp files, flush buffers, etc.
// }

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Don't move the ancient boundary stone which your fathers have set up."
// — Proverbs 22:28
//
// Production: Executables have THREE audiences — users (arguments),
// scripts (exit codes + output), and maintainers (code). A change
// safe for one audience may break another. Consider all three.
//
// Safe to Modify:
//   - Add new commands/subcommands (additive — doesn't break existing)
//   - Add new output formats behind --output flag
//   - Extend Config with new fields (with sensible defaults)
//   - Add new core operations (new commands or modes)
//   - Add --verbose, --quiet, --debug flags
//
// Modify with Care:
//   - Exit codes — scripts check these for success/failure
//   - Output format — parsers and pipe consumers depend on it
//   - Argument names — users have muscle memory and scripts hardcode them
//   - Default behavior — silent changes break existing workflows
//   - Error messages — scripts may grep for specific error text
//
// Never Modify:
//   - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//   - main() → run() → Result pattern (the execution contract)
//   - Error → exit code mapping (documented in AppError)
//   - PRAGMA/METADATA static data layout or accessor signatures
//   - Successful exit code (must be 0)
//
// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Production: Executables grow by adding commands, not modifying existing
// ones. Each new command is a new seam. Existing commands are stable API.
//
// Designed Growth Points:
//   - New subcommands (run() dispatches by command name)
//   - New --output formats (text, json, toml, csv)
//   - New Config fields with backward-compatible defaults
//   - New error variants in AppError (extend enum)
//   - New validation modes (--strict, --lenient)
//
// Future Considerations:
//   - Shell completion generation (clap feature)
//   - Man page generation from help text
//   - Config file support (TOML, env vars, CLI — precedence chain)
//   - Interactive mode (REPL for exploration)
//   - Plugin system (dynamic command loading)
//
// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lacks wisdom, let him ask of God, who gives to all
// liberally and without reproach, and it will be given to him."
// — James 1:5
//
// Exit code is wrong:
//   Check the exit_code() mapping function. Each AppError variant maps
//   to a specific code. Unhandled variants may fall through to 1.
//   Document new variants in the mapping.
//
// Arguments not parsing correctly:
//   Argument order matters for positional args. Named args (--flag) can
//   appear anywhere. Check for conflicts between short flags (-v verbose
//   vs -v version). Use long flags (--verbose) to avoid collisions.
//
// Output format breaks downstream:
//   JSON output must be valid JSON (not mixed with debug output).
//   Use eprintln! for diagnostics, println! for data output. Never
//   mix the two on the same stream.
//
// Binary not found after build:
//   Check Cargo.toml [[bin]] section — name and path must match.
//   Default binary name matches the package name. Workspace builds
//   go to target/ at the workspace root, not the crate root.
//
// Runs locally but fails in CI:
//   Check for path assumptions (absolute vs relative), environment
//   variable dependencies, and file permission differences. Use
//   std::env::current_dir() not hardcoded paths.
//
// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// CLI library:    bereshit-l0-cli (argument parsing, usage display)
// Config:         [config crate or module]
// Dependents:     [scripts, CI pipelines, or tools that call this binary]
// Supersedes:     [prior implementation path, if any]
// Parallel:       [Go/C versions of same tool, if any]
// Template:       b-word/seed/code/L0/rust/executable.rs
//
// Validation:
//   Build:    cargo build -p [crate-name]
//   Run:      cargo run -p [crate-name] -- --help
//   Test:     cargo test -p [crate-name]
//   Clippy:   cargo clippy -p [crate-name] -- -D warnings
//   All:      cargo clippy -p [crate-name] -- -D warnings && cargo test -p [crate-name]
//
// Exit Codes:
//   0 — Success
//   1 — Runtime error (AppError::Runtime, AppError::IO)
//   2 — Usage error (AppError::Usage — bad arguments)
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// "Whatsoever thy hand findeth to do, do it with thy might."
// — Ecclesiastes 9:10
//
// The executable is the handshake between the crate and the world.
// Arguments come in. Results go out. Exit codes carry the verdict.
// Every interaction is a promise — make it honest, make it clear.
//
// ──────────────────────────────────────────────────────────────────────────
// X6: Template Guide (remove this section when instantiating)
// ──────────────────────────────────────────────────────────────────────────
//
// Instantiation:
//   1.  cp executable.rs /path/to/new/crate/src/main.rs
//   2.  Change pragma: #!omni template → //omni:code --rust -executable
//   3.  Update meta.key, meta.from, meta.at for your crate
//   4.  Fill PRAGMA constants (I1-I4) with binary identity
//   5.  Fill METADATA constants (C1-C7) with binary context
//   6.  Define Config struct with your binary's configuration
//   7.  Define AppError enum with your error variants
//   8.  Implement parse_args(), run(), and exit_code()
//   9.  Create Cargo.toml with [[bin]] section (use cargo/ template)
//  10.  Add crate to workspace Cargo.toml members
//  11.  Remove X6 section and all [placeholder] markers
//  12.  Run: cargo clippy -p crate-name -- -D warnings && cargo test -p crate-name
//
// Test patterns: See demo-test.rs for comprehensive test examples.
// Executable tests typically focus on argument parsing, exit codes,
// and output format validation.
//
// ============================================================================
// END CLOSING
// ============================================================================
