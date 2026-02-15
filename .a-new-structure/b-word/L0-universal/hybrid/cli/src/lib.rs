//omni:code --rust -library
//omni:key B-L0-hybrid-cli
//omni:version b-02.00
//omni:pragma PRAGMA
//omni:metadata METADATA

// ============================================================================
// METADATA
// ============================================================================

//! # CLI Scaffolding Library
//!
//! Lightweight CLI scaffolding for CPI-SI skill commands.
//!
//! Provides standard conventions: `--help`/`-h` flags, `--json` output mode,
//! biblical references in help text, and structured usage builders.
//! Pure std — no external dependencies.
//!
//! # Typestate Pattern
//!
//! [`App`] (builder) transitions to [`ParsedApp`] (queried) via [`App::parse`]
//! or [`App::parse_os`]. The type system enforces that you cannot query flags
//! before parsing — a guarantee the Go version could not provide.
//!
//! # Usage
//!
//! ```
//! use bereshit_l0_cli::App;
//!
//! let parsed = App::new("mytool", "Does something useful")
//!     .with_biblical("Proverbs 15:28")
//!     .with_json()
//!     .parse(&["--help".to_string()])?;
//!
//! if parsed.wants_help() {
//!     bereshit_l0_cli::Usage::from_parsed(&parsed)
//!         .add_section("COMMANDS", &["status  Show system status"])
//!         .print();
//! }
//! # Ok::<(), bereshit_l0_cli::CliError>(())
//! ```
//!
//! See [`PRAGMA`] and [`METADATA`] for OmniCode identity.

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

/// OmniCode pragma identity (P1-P5). Static, zero-cost at runtime.
pub static PRAGMA: &[(&str, &str)] = &[
    // P1: Core Identity
    ("P1.key", "B-L0-hybrid-cli"),
    ("P1.type", "code"),
    ("P1.format", "rust"),
    ("P1.style", "library"),
    // P2: Structure
    ("P2.blocks", "4-block"),
    // P3: Derivation
    ("P3.from", "b-word/seed/code/L0/go/library.go"),
    ("P3.derives", "B-L0-seed-go-library"),
    (
        "P3.note",
        "Converted from Go — will become Rust template source",
    ),
    // P4: Version
    ("P4.at", "b-02.00"),
    // P5: Summary
    ("P5.title", "CLI Scaffolding Library"),
    (
        "P5.summary",
        "Typestate CLI infrastructure for CPI-SI skill commands",
    ),
];

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

/// OmniCode metadata (M1-M10). Static, zero-cost at runtime.
pub static METADATA: &[(&str, &str)] = &[
    // ── M1: Core Identity ─────────────────────────────────────────
    ("M1.key", "B-L0-hybrid-cli"),
    ("M1.component_type", "Foundation"),
    ("M1.architect", "Seanje Lenox-Wise"),
    ("M1.implementation", "Nova Dawn"),
    ("M1.created", "2025-12-10"),
    // ── M2: Version History ───────────────────────────────────────
    (
        "M2.b-02.00",
        "2026-02-14 — RUST: Rewritten with typestate pattern, zero deps",
    ),
    (
        "M2.b-01.01",
        "2026-02-14 — GO: promoted from hybrid/util/ to hybrid/ top-level",
    ),
    (
        "M2.b-01.00",
        "2026-02-14 — GO: BASELINE with identity registration",
    ),
    ("M2.a-01.00", "2025-12-10 — Initial creation (Go)"),
    // ── M3: Interface ─────────────────────────────────────────────
    ("M3.requires.stdlib", "std::env, std::process, std::fmt"),
    ("M3.requires.external", "none"),
    ("M3.requires.internal", "none (self-contained at L0)"),
    ("M3.used_by", "all CPI-SI skill CLI commands"),
    ("M3.import", "bereshit_l0_cli"),
    (
        "M3.pattern",
        "App::new(name, desc).with_json().parse_os()? -> ParsedApp",
    ),
    // ── M4: Public API ────────────────────────────────────────────
    ("M4.types", "App, ParsedApp, Usage, UsageSection, CliError"),
    ("M4.create", "App::new"),
    ("M4.config", "with_biblical, with_json"),
    ("M4.parse", "parse, parse_os -> Result<ParsedApp, CliError>"),
    (
        "M4.query",
        "wants_help, wants_json, args, name, description, biblical",
    ),
    ("M4.output", "fatal, fatal_code, error, warn"),
    (
        "M4.usage",
        "Usage::from_parsed, Usage::new, add_section, print",
    ),
    ("M4.identity", "pragma, pragma_get, metadata, metadata_get"),
    // ── M5: Operational ───────────────────────────────────────────
    ("M5.blocking", "no — argument parsing only"),
    (
        "M5.health",
        "Foundation | granted: parsed | deferred: n/a | denied: CliError",
    ),
    // ── M6: Classification ────────────────────────────────────────
    ("M6.tags", "cli, flags, usage, typestate, pure, no-deps"),
    ("M6.category", "Foundation"),
    ("M6.domain", "cli-infrastructure"),
    ("M6.layer", "L0-universal"),
    ("M6.paradigm", "CPI-SI"),
    // ── M7: Intent ────────────────────────────────────────────────
    (
        "M7.purpose",
        "Standard CLI conventions for all CPI-SI skills",
    ),
    (
        "M7.philosophy",
        "Convention over configuration — consistent flags across all tools",
    ),
    (
        "M7.provides",
        "Typestate CLI parsing, usage builder, error functions",
    ),
    // ── M8: Grounding ─────────────────────────────────────────────
    (
        "M8.scripture",
        "Proverbs 15:28 — The heart of the righteous studieth to answer",
    ),
    (
        "M8.principle",
        "Study before answering — parse input before acting",
    ),
    (
        "M8.anchor",
        "Genesis 1:1 — In the beginning God created the heaven and the earth",
    ),
    // ── M9: Dependencies ──────────────────────────────────────────
    (
        "M9.needs.stdlib",
        "std::env (args), std::process (exit), std::fmt (formatting)",
    ),
    ("M9.needs.external", "none"),
    ("M9.needs.internal", "none (self-contained)"),
    ("M9.used_by", "all CPI-SI skill CLI commands"),
    ("M9.layer_deps", "L0 only — no L1+ dependencies"),
    // ── M10: Roadmap ──────────────────────────────────────────────
    (
        "M10.current",
        "b-02.00 — RUST: typestate pattern, zero dependencies",
    ),
    (
        "M10.planned",
        "Custom flag types, subcommands (clap), completion gen, C ABI",
    ),
    (
        "M10.limitations",
        "Boolean flags only, no subcommands, no shell completion",
    ),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Imports -> Error Types -> Core Types

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

use std::env;
use std::fmt;
use std::process;

// ────────────────────────────────────────────────────────────────
// Error Types
// ────────────────────────────────────────────────────────────────

/// Errors that can occur during CLI parsing.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum CliError {
    /// An unrecognized flag was passed.
    UnknownFlag(String),
    /// A flag that requires a value was passed without one.
    MissingValue(String),
    /// Generic parse error.
    ParseError(String),
}

impl fmt::Display for CliError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::UnknownFlag(flag) => write!(f, "unknown flag: {flag}"),
            Self::MissingValue(flag) => write!(f, "flag requires a value: {flag}"),
            Self::ParseError(msg) => write!(f, "parse error: {msg}"),
        }
    }
}

impl std::error::Error for CliError {}

// ────────────────────────────────────────────────────────────────
// Core Types
// ────────────────────────────────────────────────────────────────

/// A CLI application builder (pre-parse state).
///
/// Configure the app with builder methods, then call [`parse`](App::parse)
/// or [`parse_os`](App::parse_os) to transition to [`ParsedApp`].
///
/// # Typestate Pattern
///
/// `App` -> `ParsedApp` enforces at compile time that you cannot query
/// flags before parsing. In Go, calling `WantsHelp()` before `Parse()` is
/// a silent bug. In Rust, the type system makes it impossible.
///
/// # Example
///
/// ```
/// use bereshit_l0_cli::App;
///
/// let app = App::new("health", "CPI-SI health check")
///     .with_biblical("3 John 1:2")
///     .with_json();
///
/// // app.wants_help()  // Compile error! Must parse first.
///
/// let parsed = app.parse(&[])?;
/// assert!(!parsed.wants_help());
/// # Ok::<(), bereshit_l0_cli::CliError>(())
/// ```
#[derive(Debug)]
pub struct App {
    name: String,
    description: String,
    biblical: Option<String>,
    json_enabled: bool,
}

/// A parsed CLI application (post-parse state).
///
/// Created by [`App::parse`] or [`App::parse_os`]. Only this type
/// exposes query methods like [`wants_help`](ParsedApp::wants_help)
/// and [`wants_json`](ParsedApp::wants_json).
#[derive(Debug)]
pub struct ParsedApp {
    name: String,
    description: String,
    biblical: Option<String>,
    help: bool,
    json: bool,
    json_enabled: bool,
    args: Vec<String>,
}

/// A structured help/usage message.
///
/// Build with [`Usage::from_parsed`] or [`Usage::new`], add sections,
/// then [`print`](Usage::print) to stdout.
pub struct Usage {
    name: String,
    description: String,
    biblical: Option<String>,
    sections: Vec<UsageSection>,
}

/// A titled section in usage output.
pub struct UsageSection {
    /// Section header (e.g., "COMMANDS", "OPTIONS").
    pub title: String,
    /// Content lines within this section.
    pub lines: Vec<String>,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Identity -> App Builder -> Parsing -> ParsedApp Queries
//                -> Output Helpers -> Usage Builder

// ────────────────────────────────────────────────────────────────
// Identity — Static OmniCode Access
// ────────────────────────────────────────────────────────────────

/// Returns this crate's OmniCode pragma identity (P1-P5).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Looks up a specific pragma key. Returns `None` if not found.
///
/// ```
/// assert_eq!(bereshit_l0_cli::pragma_get("P1.key"), Some("B-L0-hybrid-cli"));
/// assert_eq!(bereshit_l0_cli::pragma_get("P99.nope"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (M1-M10).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Looks up a specific metadata key. Returns `None` if not found.
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Register this crate's identity with the global registry.
///
/// Rust equivalent of Go's `init() { identity.Register(key, pragma, metadata) }`.
/// Call once during application startup to make this crate discoverable
/// via `bereshit_l0_identity::lookup()`.
pub fn register_identity() {
    let key = pragma_get("P1.key").expect("P1.key must exist in PRAGMA");
    let pragma_map = PRAGMA
        .iter()
        .map(|(k, v)| ((*k).to_owned(), (*v).to_owned()))
        .collect();
    let metadata_map = METADATA
        .iter()
        .map(|(k, v)| ((*k).to_owned(), (*v).to_owned()))
        .collect();
    bereshit_l0_identity::register(key, pragma_map, metadata_map);
}

// ────────────────────────────────────────────────────────────────
// App Builder (pre-parse)
// ────────────────────────────────────────────────────────────────

impl App {
    /// Creates a new CLI app with the given name and description.
    ///
    /// Standard `--help` and `-h` flags are always recognized during parsing.
    pub fn new(name: &str, description: &str) -> Self {
        Self {
            name: name.to_string(),
            description: description.to_string(),
            biblical: None,
            json_enabled: false,
        }
    }

    /// Sets the biblical reference displayed in help output.
    ///
    /// ```
    /// # use bereshit_l0_cli::App;
    /// let app = App::new("tool", "desc").with_biblical("Proverbs 15:28");
    /// ```
    pub fn with_biblical(mut self, reference: &str) -> Self {
        self.biblical = Some(reference.to_string());
        self
    }

    /// Enables the `--json` flag for JSON output mode.
    ///
    /// Without calling this, `--json` is treated as an unknown flag.
    pub fn with_json(mut self) -> Self {
        self.json_enabled = true;
        self
    }

    /// Parses the given arguments, consuming `App` and producing [`ParsedApp`].
    ///
    /// # Errors
    ///
    /// Returns [`CliError::UnknownFlag`] if an unrecognized flag is encountered.
    /// Returns [`CliError::UnknownFlag`] if `--json` is passed but not enabled.
    pub fn parse(self, args: &[String]) -> Result<ParsedApp, CliError> {
        let mut help = false;
        let mut json = false;
        let mut positional = Vec::new();

        for arg in args {
            match arg.as_str() {
                "--help" | "-h" => help = true,
                "--json" if self.json_enabled => json = true,
                "--json" => {
                    return Err(CliError::UnknownFlag(
                        "--json (not enabled for this command)".to_string(),
                    ));
                }
                s if s.starts_with('-') => {
                    return Err(CliError::UnknownFlag(s.to_string()));
                }
                _ => positional.push(arg.clone()),
            }
        }

        Ok(ParsedApp {
            name: self.name,
            description: self.description,
            biblical: self.biblical,
            help,
            json,
            json_enabled: self.json_enabled,
            args: positional,
        })
    }

    /// Parses `std::env::args()` (skipping program name), consuming `App`.
    ///
    /// This is the standard entry point for CLI applications.
    ///
    /// # Errors
    ///
    /// Returns [`CliError`] if arguments cannot be parsed.
    pub fn parse_os(self) -> Result<ParsedApp, CliError> {
        let args: Vec<String> = env::args().skip(1).collect();
        self.parse(&args)
    }
}

// ────────────────────────────────────────────────────────────────
// ParsedApp Queries (post-parse)
// ────────────────────────────────────────────────────────────────

impl ParsedApp {
    /// Returns `true` if `--help` or `-h` was passed.
    pub fn wants_help(&self) -> bool {
        self.help
    }

    /// Returns `true` if `--json` was passed (and was enabled).
    pub fn wants_json(&self) -> bool {
        self.json
    }

    /// Returns `true` if `--json` mode was enabled for this app.
    pub fn json_enabled(&self) -> bool {
        self.json_enabled
    }

    /// Returns non-flag arguments after parsing.
    pub fn args(&self) -> &[String] {
        &self.args
    }

    /// Returns the app name.
    pub fn name(&self) -> &str {
        &self.name
    }

    /// Returns the app description.
    pub fn description(&self) -> &str {
        &self.description
    }

    /// Returns the biblical reference, if set.
    pub fn biblical(&self) -> Option<&str> {
        self.biblical.as_deref()
    }
}

// ────────────────────────────────────────────────────────────────
// Output Helpers
// ────────────────────────────────────────────────────────────────

/// Prints to stderr and exits with code 1.
pub fn fatal(msg: &str) -> ! {
    eprintln!("{msg}");
    process::exit(1);
}

/// Prints to stderr and exits with the specified code.
pub fn fatal_code(code: i32, msg: &str) -> ! {
    eprintln!("{msg}");
    process::exit(code);
}

/// Prints an error message to stderr without exiting.
pub fn error(msg: &str) {
    eprintln!("{msg}");
}

/// Prints a warning to stderr with "Warning: " prefix.
pub fn warn(msg: &str) {
    eprintln!("Warning: {msg}");
}

// ────────────────────────────────────────────────────────────────
// Usage Builder
// ────────────────────────────────────────────────────────────────

impl Usage {
    /// Creates a usage builder from a parsed app's configuration.
    pub fn from_parsed(app: &ParsedApp) -> Self {
        Self {
            name: app.name.clone(),
            description: app.description.clone(),
            biblical: app.biblical.clone(),
            sections: Vec::new(),
        }
    }

    /// Creates a usage builder directly from name and description.
    pub fn new(name: &str, description: &str) -> Self {
        Self {
            name: name.to_string(),
            description: description.to_string(),
            biblical: None,
            sections: Vec::new(),
        }
    }

    /// Sets the biblical reference for this usage message.
    pub fn with_biblical(mut self, reference: &str) -> Self {
        self.biblical = Some(reference.to_string());
        self
    }

    /// Adds a titled section to the usage output.
    ///
    /// ```
    /// # use bereshit_l0_cli::Usage;
    /// let usage = Usage::new("tool", "desc")
    ///     .add_section("COMMANDS", &["status  Show status", "health  Health check"])
    ///     .add_section("OPTIONS", &["--json  Output as JSON"]);
    /// ```
    pub fn add_section(mut self, title: &str, lines: &[&str]) -> Self {
        self.sections.push(UsageSection {
            title: title.to_string(),
            lines: lines.iter().map(|s| (*s).to_string()).collect(),
        });
        self
    }

    /// Prints the formatted usage message to stdout.
    pub fn print(&self) {
        println!("{} - {}", self.name, self.description);
        println!();

        for section in &self.sections {
            println!("{}:", section.title);
            for line in &section.lines {
                println!("  {line}");
            }
            println!();
        }

        if let Some(ref biblical) = self.biblical {
            println!("Biblical: {biblical}");
        }
    }
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// Lightweight CLI scaffolding for CPI-SI skills. Typestate pattern
// enforces parse-before-query at compile time — something the Go
// version could not guarantee.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new ParsedApp query methods
//   - Add new output helpers (fatal variants, error types)
//   - Extend Usage builder with new section types
//   - Add custom flag support (string flags, integer flags)
//
// Modify with Care:
//   - App/ParsedApp struct fields — breaks pattern matching
//   - Standard flag names (--help, -h, --json) — convention-breaking
//   - Builder method signatures — breaks chaining patterns
//
// Never Modify:
//   - 4-block structure
//   - Typestate boundary (App -> ParsedApp)
//   - PRAGMA/METADATA static data format
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:  std only (env, process, fmt)
// Dependents:    all CPI-SI skill CLI commands
// Supersedes:    hybrid/cli/cli.go (Go version, b-01.01)
// Parallel:      hybrid/format (value formatting), hybrid/output (terminal output)
//
// "The heart of the righteous studieth to answer." — Proverbs 15:28
//
// ============================================================================
// END CLOSING
// ============================================================================

// ============================================================================
// Tests
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    // ── Constructor ──────────────────────────────────────────────

    #[test]
    fn new_creates_app_with_defaults() {
        let app = App::new("test", "A test tool");
        assert_eq!(app.name, "test");
        assert_eq!(app.description, "A test tool");
        assert!(app.biblical.is_none());
        assert!(!app.json_enabled);
    }

    // ── Builder ──────────────────────────────────────────────────

    #[test]
    fn builder_chain_configures_all_options() {
        let app = App::new("test", "desc")
            .with_biblical("Gen 1:1")
            .with_json();
        assert_eq!(app.biblical.as_deref(), Some("Gen 1:1"));
        assert!(app.json_enabled);
    }

    // ── Parsing ──────────────────────────────────────────────────

    #[test]
    fn parse_empty_args() {
        let parsed = App::new("test", "desc").parse(&[]).unwrap();
        assert!(!parsed.wants_help());
        assert!(!parsed.wants_json());
        assert!(parsed.args().is_empty());
    }

    #[test]
    fn parse_help_long() {
        let args = vec!["--help".to_string()];
        let parsed = App::new("test", "desc").parse(&args).unwrap();
        assert!(parsed.wants_help());
    }

    #[test]
    fn parse_help_short() {
        let args = vec!["-h".to_string()];
        let parsed = App::new("test", "desc").parse(&args).unwrap();
        assert!(parsed.wants_help());
    }

    #[test]
    fn parse_json_when_enabled() {
        let args = vec!["--json".to_string()];
        let parsed = App::new("test", "desc").with_json().parse(&args).unwrap();
        assert!(parsed.wants_json());
    }

    #[test]
    fn parse_json_when_not_enabled_is_error() {
        let args = vec!["--json".to_string()];
        let result = App::new("test", "desc").parse(&args);
        assert!(result.is_err());
        assert!(matches!(result.unwrap_err(), CliError::UnknownFlag(_)));
    }

    #[test]
    fn parse_unknown_flag_is_error() {
        let args = vec!["--unknown".to_string()];
        let result = App::new("test", "desc").parse(&args);
        assert!(result.is_err());
    }

    #[test]
    fn parse_positional_args() {
        let args = vec!["file1.txt".to_string(), "file2.txt".to_string()];
        let parsed = App::new("test", "desc").parse(&args).unwrap();
        assert_eq!(parsed.args(), &["file1.txt", "file2.txt"]);
    }

    #[test]
    fn parse_mixed_flags_and_args() {
        let args = vec![
            "--help".to_string(),
            "file.txt".to_string(),
            "--json".to_string(),
        ];
        let parsed = App::new("test", "desc").with_json().parse(&args).unwrap();
        assert!(parsed.wants_help());
        assert!(parsed.wants_json());
        assert_eq!(parsed.args(), &["file.txt"]);
    }

    // ── Accessors ────────────────────────────────────────────────

    #[test]
    fn parsed_app_exposes_config() {
        let parsed = App::new("mytool", "Does things")
            .with_biblical("Prov 15:28")
            .parse(&[])
            .unwrap();
        assert_eq!(parsed.name(), "mytool");
        assert_eq!(parsed.description(), "Does things");
        assert_eq!(parsed.biblical(), Some("Prov 15:28"));
    }

    #[test]
    fn json_enabled_reflects_builder() {
        let without = App::new("t", "d").parse(&[]).unwrap();
        assert!(!without.json_enabled());

        let with = App::new("t", "d").with_json().parse(&[]).unwrap();
        assert!(with.json_enabled());
    }

    // ── Identity ─────────────────────────────────────────────────

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("P1.key"), Some("B-L0-hybrid-cli"));
    }

    #[test]
    fn pragma_contains_format() {
        assert_eq!(pragma_get("P1.format"), Some("rust"));
    }

    #[test]
    fn pragma_get_returns_none_for_missing() {
        assert_eq!(pragma_get("P99.nonexistent"), None);
    }

    #[test]
    fn metadata_contains_version_history() {
        assert!(metadata_get("M2.b-02.00").is_some());
    }

    #[test]
    fn metadata_contains_grounding() {
        let scripture = metadata_get("M8.scripture").unwrap();
        assert!(scripture.contains("Proverbs 15:28"));
    }

    // ── Error Display ────────────────────────────────────────────

    #[test]
    fn cli_error_display_unknown_flag() {
        let err = CliError::UnknownFlag("--bad".to_string());
        assert_eq!(err.to_string(), "unknown flag: --bad");
    }

    #[test]
    fn cli_error_display_missing_value() {
        let err = CliError::MissingValue("--count".to_string());
        assert_eq!(err.to_string(), "flag requires a value: --count");
    }

    #[test]
    fn cli_error_display_parse_error() {
        let err = CliError::ParseError("bad input".to_string());
        assert_eq!(err.to_string(), "parse error: bad input");
    }

    // ── Usage Builder ────────────────────────────────────────────

    #[test]
    fn usage_from_parsed_preserves_config() {
        let parsed = App::new("tool", "desc")
            .with_biblical("Gen 1:1")
            .parse(&[])
            .unwrap();
        let usage = Usage::from_parsed(&parsed);
        assert_eq!(usage.name, "tool");
        assert_eq!(usage.biblical.as_deref(), Some("Gen 1:1"));
    }

    #[test]
    fn usage_new_creates_standalone() {
        let usage = Usage::new("tool", "desc")
            .with_biblical("Prov 3:5")
            .add_section("COMMANDS", &["status  Show status"]);
        assert_eq!(usage.sections.len(), 1);
        assert_eq!(usage.sections[0].title, "COMMANDS");
    }
}
