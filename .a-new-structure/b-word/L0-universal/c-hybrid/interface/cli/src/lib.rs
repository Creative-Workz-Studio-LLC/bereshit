//omni:code --rust -library
//omni:key B-L0-hybrid-cli
//omni:version b-03.00
//omni:pragma PRAGMA
//omni:metadata METADATA
//!
//! # CLI Scaffolding Library
//!
//! Lightweight CLI scaffolding for CPI-SI skill commands.
//!
//! Provides standard conventions: `--help`/`-h` flags, `--json` output mode,
//! `--version`/`-V` display, biblical references in help text, and structured
//! usage builders. Pure std — no external dependencies.
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
//! use bereshit_l0_cli::{App, Usage};
//!
//! let parsed = App::new("mytool", "Does something useful")
//!     .with_biblical("Proverbs 15:28")
//!     .with_version("0.3.0")
//!     .with_json()
//!     .parse(&["--help".to_string()])?;
//!
//! if parsed.wants_help() {
//!     Usage::from_parsed(&parsed)
//!         .with_standard_flags()
//!         .add_section("COMMANDS", &["status  Show system status"])
//!         .print();
//! }
//! # Ok::<(), bereshit_l0_cli::CliError>(())
//! ```
//!
//! # OmniCode Identity
//!
//! | Static | Sections | Purpose | Query |
//! |--------|----------|---------|-------|
//! | [`PRAGMA`] | I1-I4 | Identity — who this file is | [`pragma_get`] |
//! | [`METADATA`] | C1-C7 | Context — when, where, why, how | [`metadata_get`] |

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-cli
// Purpose: Typestate CLI scaffolding for all CPI-SI skill commands
// Biblical: Proverbs 15:28 — The heart of the righteous studieth to answer
// Version: b-03.00

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
/// let key = bereshit_l0_cli::pragma_get("I1.key").unwrap_or("unknown");
/// assert_eq!(key, "B-L0-hybrid-cli");
/// ```
#[rustfmt::skip]
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-hybrid-cli"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/library.rs"),
    ("I1.at",        "b-03.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I2.role",      "interface"),
    // I3: Instance
    ("I3.file",      "lib.rs"),
    ("I3.title",     "CLI Scaffolding Library"),
    ("I3.component", "c-hybrid/interface/cli"),
    ("I3.path",      "L0-universal/c-hybrid/interface/cli/src/lib.rs"),
    ("I3.provides",  "CLI_SCAFFOLDING"),
    ("I3.brief",     "Typestate CLI infrastructure for CPI-SI skill commands"),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "hybrid/interface/cli"),
    ("I4.pattern",   "typestate builder — App (pre-parse) -> ParsedApp (post-parse)"),
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
/// let version = bereshit_l0_cli::metadata_get("C1.version").unwrap_or("unknown");
/// assert_eq!(version, "b-03.00");
/// ```
#[rustfmt::skip]
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-03.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2025-12-10"),
    ("C1.updated",           "2026-02-18"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Proverbs 15:28 — The heart of the righteous studieth to answer"),
    ("C3.principle",         "Study before answering — parse input fully before acting on it"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "std::env (args), std::fmt (formatting), std::process (exit)"),
    ("C4.requires.external", "none — pure std library"),
    ("C4.requires.internal", "bereshit-l0-identity (global component registry)"),
    ("C4.consumers",         "all CPI-SI skill CLI commands"),
    ("C4.integration",       "App::new(name, desc).with_json().with_version(v).parse_os()? -> ParsedApp"),
    ("C4.if_missing",        "every skill reinvents flag parsing, help formatting, error exits"),
    // C5: Intent
    ("C5.purpose",           "Standard CLI conventions for all CPI-SI skill commands"),
    ("C5.philosophy",        "Convention over configuration — consistent flags across all tools"),
    // C6: Roadmap
    ("C6.current",           "b-03.00 — typestate pattern, version flag, standard flags, Display for Usage"),
    ("C6.planned",           "subcommand support, custom flag types, shell completion, C ABI bridge"),
    ("C6.limitations",       "boolean flags only, no subcommands, no shell completion"),
    // C7: Classification
    ("C7.tags",              "cli, flags, usage, typestate, pure, no-deps, interface, omnicode, cpi-si"),
    ("C7.category",          "Interface"),
    ("C7.domain",            "cli-infrastructure"),
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
// Section order (dependency chain — each layer uses only what's above):
//
//   [S01] imports         — What this file depends on
//   [S06] error-types     — CliError enum + Display + Error + methods
//   [S07] core-types      — App, ParsedApp, Usage, UsageSection

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use std::env;
use std::fmt;
use std::process;

// ──────────────────────────────────────────────────────────────────────────
// 6. Error Types
// ──────────────────────────────────────────────────────────────────────────

/// Errors that can occur during CLI parsing.
///
/// Each variant carries the offending argument or context string.
/// Use [`exit_code`](CliError::exit_code) for standardized process exit codes.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum CliError {
    /// An unrecognized flag was passed.
    UnknownFlag(String),
    /// A flag that requires a value was passed without one.
    MissingValue(String),
    /// Generic parse error.
    ParseError(String),
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

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
///     .with_version("0.3.0")
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
    version: Option<String>,
    json_enabled: bool,
}

/// A parsed CLI application (post-parse state).
///
/// Created by [`App::parse`] or [`App::parse_os`]. Only this type
/// exposes query methods like [`wants_help`](ParsedApp::wants_help)
/// and [`wants_json`](ParsedApp::wants_json).
///
/// Implements `Clone` for downstream flexibility — parsed configuration
/// may need to be shared across subsystems.
#[derive(Debug, Clone)]
pub struct ParsedApp {
    name: String,
    description: String,
    biblical: Option<String>,
    version: Option<String>,
    help: bool,
    json: bool,
    version_flag: bool,
    json_enabled: bool,
    args: Vec<String>,
}

/// A structured help/usage message.
///
/// Build with [`Usage::from_parsed`] or [`Usage::new`], add sections,
/// then [`print`](Usage::print) to stdout. Implements [`Display`] for
/// flexible output (capture to string, embed in larger messages).
///
/// # Standard Flags
///
/// [`with_standard_flags`](Usage::with_standard_flags) auto-generates
/// a FLAGS section based on what was configured:
///
/// ```text
/// FLAGS:
///   --help, -h     Show this help message
///   --json         Output as JSON
///   --version, -V  Show version
/// ```
#[derive(Debug, Clone)]
pub struct Usage {
    name: String,
    description: String,
    biblical: Option<String>,
    version: Option<String>,
    json_enabled: bool,
    sections: Vec<UsageSection>,
}

/// A titled section in usage output.
#[derive(Debug, Clone)]
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
// BODY makes things HAPPEN. SETUP made things EXIST.
//
// Subsection order:
//   1. Identity Access       — OmniCode static accessor functions
//   3. Constructors/Builders — App::new + builder chain
//   4. Core Logic            — App::parse, App::parse_os (typestate transition)
//   5. Queries & Accessors   — ParsedApp query methods
//   6. Output & Display      — fatal/error/warn helpers, Usage Display impl
//   7. Free Functions        — Usage builder impl, register_identity()

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
/// assert_eq!(bereshit_l0_cli::pragma_get("I1.key"), Some("B-L0-hybrid-cli"));
/// assert_eq!(bereshit_l0_cli::pragma_get("I1.format"), Some("rust"));
/// assert_eq!(bereshit_l0_cli::pragma_get("I99.nope"), None);
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
/// assert_eq!(bereshit_l0_cli::metadata_get("C1.version"), Some("b-03.00"));
/// assert_eq!(bereshit_l0_cli::metadata_get("C99.nope"), None);
/// ```
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors / Builders
// ──────────────────────────────────────────────────────────────────────────
//
// App builder methods. Consuming self (move semantics) enables fluent
// chaining: App::new("x", "y").with_biblical("...").with_json().parse(&[])
//
// The builder pattern terminates at parse/parse_os which consumes App
// and produces ParsedApp — the typestate transition.

impl App {
    /// Creates a new CLI app with the given name and description.
    ///
    /// Standard `--help` and `-h` flags are always recognized during parsing.
    /// Use [`with_json`](App::with_json) and [`with_version`](App::with_version)
    /// to enable additional standard flags.
    pub fn new(name: &str, description: &str) -> Self {
        Self {
            name: name.to_string(),
            description: description.to_string(),
            biblical: None,
            version: None,
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

    /// Sets the version string and enables `--version` / `-V` flags.
    ///
    /// Without calling this, `--version` is treated as an unknown flag.
    /// The version string is also displayed in [`Usage`] output when set.
    ///
    /// ```
    /// # use bereshit_l0_cli::App;
    /// let parsed = App::new("tool", "desc")
    ///     .with_version("0.3.0")
    ///     .parse(&["--version".to_string()])?;
    /// assert!(parsed.wants_version());
    /// assert_eq!(parsed.version(), Some("0.3.0"));
    /// # Ok::<(), bereshit_l0_cli::CliError>(())
    /// ```
    pub fn with_version(mut self, version: &str) -> Self {
        self.version = Some(version.to_string());
        self
    }

    /// Enables the `--json` flag for JSON output mode.
    ///
    /// Without calling this, `--json` is treated as an unknown flag.
    pub fn with_json(mut self) -> Self {
        self.json_enabled = true;
        self
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────
//
// The typestate transition: App (builder) -> ParsedApp (queried).
// parse() consumes App, enforcing at compile time that the builder
// cannot be reused after parsing. This is the heart of the crate.

impl App {
    /// Parses the given arguments, consuming `App` and producing [`ParsedApp`].
    ///
    /// Recognizes `--help`/`-h` always. Recognizes `--json` only if
    /// [`with_json`](App::with_json) was called. Recognizes `--version`/`-V`
    /// only if [`with_version`](App::with_version) was called.
    ///
    /// # Errors
    ///
    /// Returns [`CliError::UnknownFlag`] if an unrecognized flag is encountered,
    /// including `--json` or `--version` when not enabled.
    pub fn parse(self, args: &[String]) -> Result<ParsedApp, CliError> {
        let mut help = false;
        let mut json = false;
        let mut version_flag = false;
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
                "--version" | "-V" if self.version.is_some() => version_flag = true,
                "--version" | "-V" => {
                    return Err(CliError::UnknownFlag(
                        "--version (not enabled for this command)".to_string(),
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
            version: self.version,
            help,
            json,
            version_flag,
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

// ──────────────────────────────────────────────────────────────────────────
// 5. Queries & Accessors
// ──────────────────────────────────────────────────────────────────────────
//
// ParsedApp query methods — the post-parse API surface.
// Only available after parse() succeeds (typestate enforcement).
//
// Organized by: flag queries → config accessors → argument accessors

impl ParsedApp {
    // --- Flag Queries ---

    /// Returns `true` if `--help` or `-h` was passed.
    pub fn wants_help(&self) -> bool {
        self.help
    }

    /// Returns `true` if `--json` was passed (and was enabled).
    pub fn wants_json(&self) -> bool {
        self.json
    }

    /// Returns `true` if `--version` or `-V` was passed (and was enabled).
    pub fn wants_version(&self) -> bool {
        self.version_flag
    }

    // --- Config Accessors ---

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

    /// Returns the version string, if set via [`App::with_version`].
    pub fn version(&self) -> Option<&str> {
        self.version.as_deref()
    }

    /// Returns `true` if `--json` mode was enabled for this app.
    pub fn json_enabled(&self) -> bool {
        self.json_enabled
    }

    // --- Argument Accessors ---

    /// Returns all non-flag arguments after parsing.
    pub fn args(&self) -> &[String] {
        &self.args
    }

    /// Returns the first positional argument, if any.
    ///
    /// Convenience for the common pattern of a single required argument.
    pub fn first_arg(&self) -> Option<&str> {
        self.args.first().map(String::as_str)
    }

    /// Returns `true` if at least one positional argument was provided.
    pub fn has_args(&self) -> bool {
        !self.args.is_empty()
    }

    /// Returns the number of positional arguments.
    pub fn arg_count(&self) -> usize {
        self.args.len()
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 6. Output & Display
// ──────────────────────────────────────────────────────────────────────────
//
// Three categories:
//   a) CliError impls: Display, Error, exit_code()
//   b) Process-level helpers: fatal, fatal_code, error, warn
//   c) Usage Display impl: renders Usage as formatted text
//
// CliError impls live here (not SETUP) because SETUP declares types,
// BODY gives them behavior.

// --- CliError Impls ---

impl CliError {
    /// Returns a standardized exit code for this error.
    ///
    /// | Variant | Code | Convention |
    /// |---------|------|------------|
    /// | `UnknownFlag` | 2 | Usage error (BSD/GNU convention) |
    /// | `MissingValue` | 2 | Usage error |
    /// | `ParseError` | 1 | General error |
    pub fn exit_code(&self) -> i32 {
        match self {
            Self::UnknownFlag(_) | Self::MissingValue(_) => 2,
            Self::ParseError(_) => 1,
        }
    }
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

// --- Process Helpers ---

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

// --- Usage Display ---

impl fmt::Display for Usage {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        // Header: name [version] - description
        if let Some(ref ver) = self.version {
            writeln!(f, "{} {} - {}", self.name, ver, self.description)?;
        } else {
            writeln!(f, "{} - {}", self.name, self.description)?;
        }
        writeln!(f)?;

        // Sections
        for section in &self.sections {
            writeln!(f, "{}:", section.title)?;
            for line in &section.lines {
                writeln!(f, "  {line}")?;
            }
            writeln!(f)?;
        }

        // Biblical reference
        if let Some(ref biblical) = self.biblical {
            writeln!(f, "Biblical: {biblical}")?;
        }

        Ok(())
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────
//
// Two groups:
//   a) Usage builder — construct and configure usage messages
//   b) register_identity() — L0 identity system registration
//
// Usage is a builder (consuming self for chaining) that also implements
// Display (§6) for flexible output.

// --- Usage Builder ---

impl Usage {
    /// Creates a usage builder from a parsed app's configuration.
    ///
    /// Carries forward name, description, biblical reference, version,
    /// and json_enabled state for [`with_standard_flags`](Usage::with_standard_flags).
    pub fn from_parsed(app: &ParsedApp) -> Self {
        Self {
            name: app.name.clone(),
            description: app.description.clone(),
            biblical: app.biblical.clone(),
            version: app.version.clone(),
            json_enabled: app.json_enabled,
            sections: Vec::new(),
        }
    }

    /// Creates a usage builder directly from name and description.
    ///
    /// For standalone usage messages not tied to a parsed app.
    /// Use [`with_version`](Usage::with_version) and
    /// [`with_biblical`](Usage::with_biblical) to set optional fields.
    pub fn new(name: &str, description: &str) -> Self {
        Self {
            name: name.to_string(),
            description: description.to_string(),
            biblical: None,
            version: None,
            json_enabled: false,
            sections: Vec::new(),
        }
    }

    /// Sets the biblical reference for this usage message.
    pub fn with_biblical(mut self, reference: &str) -> Self {
        self.biblical = Some(reference.to_string());
        self
    }

    /// Sets the version string for this usage message.
    ///
    /// Displayed in the header line: `tool 0.3.0 - description`.
    pub fn with_version(mut self, version: &str) -> Self {
        self.version = Some(version.to_string());
        self
    }

    /// Auto-generates a FLAGS section from the app configuration.
    ///
    /// Always includes `--help, -h`. Includes `--json` if JSON mode was
    /// enabled, and `--version, -V` if a version string was set.
    ///
    /// ```
    /// # use bereshit_l0_cli::{App, Usage};
    /// let parsed = App::new("tool", "desc")
    ///     .with_json()
    ///     .with_version("1.0.0")
    ///     .parse(&[])?;
    /// let usage = Usage::from_parsed(&parsed).with_standard_flags();
    /// // FLAGS section now contains --help, --json, --version
    /// # Ok::<(), bereshit_l0_cli::CliError>(())
    /// ```
    pub fn with_standard_flags(self) -> Self {
        let mut flags: Vec<&str> = vec!["--help, -h     Show this help message"];
        if self.json_enabled {
            flags.push("--json         Output as JSON");
        }
        if self.version.is_some() {
            flags.push("--version, -V  Show version");
        }
        self.add_section("FLAGS", &flags)
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
    ///
    /// Equivalent to `print!("{self}")`. For stderr output, use
    /// `eprint!("{usage}")` directly (Display is implemented).
    pub fn print(&self) {
        print!("{self}");
    }
}

// --- Identity Registration ---

/// Register this crate's identity with the global registry.
///
/// Rust equivalent of Go's `init() { identity.Register(key, pragma, metadata) }`.
/// Call once during application startup to make this crate discoverable
/// via `bereshit_l0_identity::lookup()`.
pub fn register_identity() {
    let key = pragma_get("I1.key").expect("I1.key must exist in PRAGMA");
    bereshit_l0_identity::register(key, PRAGMA, METADATA);
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
//
// 40 tests organized by BODY subsection:
//   §1 Identity (9)  §3 Constructor/Builder (2)  §4 Parsing (10)
//   §5 Accessors (9) §6 Output/Display (5)       §7 Usage Builder (5)

#[cfg(test)]
mod tests {
    use super::*;

    // ── §1 Identity ─────────────────────────────────────────────────

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("I1.key"), Some("B-L0-hybrid-cli"));
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
        assert_eq!(pragma_get("I99.nonexistent"), None);
    }

    #[test]
    fn metadata_contains_version() {
        assert_eq!(metadata_get("C1.version"), Some("b-03.00"));
    }

    #[test]
    fn metadata_contains_grounding() {
        let scripture = metadata_get("C3.scripture").unwrap();
        assert!(scripture.contains("Proverbs 15:28"));
    }

    #[test]
    fn metadata_contains_domain() {
        assert_eq!(metadata_get("C7.domain"), Some("cli-infrastructure"));
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

    // ── §3 Constructor / Builder ────────────────────────────────────

    #[test]
    fn new_creates_app_with_defaults() {
        let app = App::new("test", "A test tool");
        assert_eq!(app.name, "test");
        assert_eq!(app.description, "A test tool");
        assert!(app.biblical.is_none());
        assert!(app.version.is_none());
        assert!(!app.json_enabled);
    }

    #[test]
    fn builder_chain_configures_all_options() {
        let app = App::new("test", "desc")
            .with_biblical("Gen 1:1")
            .with_version("1.0.0")
            .with_json();
        assert_eq!(app.biblical.as_deref(), Some("Gen 1:1"));
        assert_eq!(app.version.as_deref(), Some("1.0.0"));
        assert!(app.json_enabled);
    }

    // ── §4 Parsing ──────────────────────────────────────────────────

    #[test]
    fn parse_empty_args() {
        let parsed = App::new("test", "desc").parse(&[]).unwrap();
        assert!(!parsed.wants_help());
        assert!(!parsed.wants_json());
        assert!(!parsed.wants_version());
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
    fn parse_version_long() {
        let args = vec!["--version".to_string()];
        let parsed = App::new("test", "desc")
            .with_version("1.0.0")
            .parse(&args)
            .unwrap();
        assert!(parsed.wants_version());
    }

    #[test]
    fn parse_version_short() {
        let args = vec!["-V".to_string()];
        let parsed = App::new("test", "desc")
            .with_version("1.0.0")
            .parse(&args)
            .unwrap();
        assert!(parsed.wants_version());
    }

    #[test]
    fn parse_version_when_not_enabled_is_error() {
        let args = vec!["--version".to_string()];
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
    fn parse_mixed_flags_and_positional_args() {
        let args = vec![
            "--help".to_string(),
            "file.txt".to_string(),
            "--json".to_string(),
            "other.txt".to_string(),
        ];
        let parsed = App::new("test", "desc").with_json().parse(&args).unwrap();
        assert!(parsed.wants_help());
        assert!(parsed.wants_json());
        assert_eq!(parsed.args(), &["file.txt", "other.txt"]);
    }

    // ── §5 Accessors ────────────────────────────────────────────────

    #[test]
    fn parsed_app_exposes_config() {
        let parsed = App::new("mytool", "Does things")
            .with_biblical("Prov 15:28")
            .with_version("0.3.0")
            .parse(&[])
            .unwrap();
        assert_eq!(parsed.name(), "mytool");
        assert_eq!(parsed.description(), "Does things");
        assert_eq!(parsed.biblical(), Some("Prov 15:28"));
        assert_eq!(parsed.version(), Some("0.3.0"));
    }

    #[test]
    fn json_enabled_reflects_builder() {
        let without = App::new("t", "d").parse(&[]).unwrap();
        assert!(!without.json_enabled());

        let with = App::new("t", "d").with_json().parse(&[]).unwrap();
        assert!(with.json_enabled());
    }

    #[test]
    fn version_returns_none_when_not_set() {
        let parsed = App::new("t", "d").parse(&[]).unwrap();
        assert!(parsed.version().is_none());
        assert!(!parsed.wants_version());
    }

    #[test]
    fn first_arg_returns_first() {
        let args = vec!["alpha".to_string(), "beta".to_string()];
        let parsed = App::new("t", "d").parse(&args).unwrap();
        assert_eq!(parsed.first_arg(), Some("alpha"));
    }

    #[test]
    fn first_arg_returns_none_when_empty() {
        let parsed = App::new("t", "d").parse(&[]).unwrap();
        assert_eq!(parsed.first_arg(), None);
    }

    #[test]
    fn has_args_true_when_present() {
        let args = vec!["file.txt".to_string()];
        let parsed = App::new("t", "d").parse(&args).unwrap();
        assert!(parsed.has_args());
    }

    #[test]
    fn has_args_false_when_empty() {
        let parsed = App::new("t", "d").parse(&[]).unwrap();
        assert!(!parsed.has_args());
    }

    #[test]
    fn arg_count_returns_correct_count() {
        let args = vec!["a".to_string(), "b".to_string(), "c".to_string()];
        let parsed = App::new("t", "d").parse(&args).unwrap();
        assert_eq!(parsed.arg_count(), 3);
    }

    #[test]
    fn parsed_app_clone_preserves_state() {
        let args = vec!["--help".to_string(), "file".to_string()];
        let parsed = App::new("t", "d").with_json().parse(&args).unwrap();
        let cloned = parsed.clone();
        assert_eq!(parsed.wants_help(), cloned.wants_help());
        assert_eq!(parsed.args(), cloned.args());
        assert_eq!(parsed.json_enabled(), cloned.json_enabled());
    }

    // ── §6 Output / Display ─────────────────────────────────────────

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

    #[test]
    fn cli_error_exit_codes() {
        assert_eq!(CliError::UnknownFlag("x".into()).exit_code(), 2);
        assert_eq!(CliError::MissingValue("x".into()).exit_code(), 2);
        assert_eq!(CliError::ParseError("x".into()).exit_code(), 1);
    }

    #[test]
    fn usage_display_includes_version_in_header() {
        let usage = Usage::new("tool", "desc").with_version("1.0.0");
        let output = usage.to_string();
        assert!(output.contains("tool 1.0.0 - desc"));
    }

    // ── §7 Usage Builder ────────────────────────────────────────────

    #[test]
    fn usage_from_parsed_preserves_config() {
        let parsed = App::new("tool", "desc")
            .with_biblical("Gen 1:1")
            .with_version("0.3.0")
            .with_json()
            .parse(&[])
            .unwrap();
        let usage = Usage::from_parsed(&parsed);
        assert_eq!(usage.name, "tool");
        assert_eq!(usage.biblical.as_deref(), Some("Gen 1:1"));
        assert_eq!(usage.version.as_deref(), Some("0.3.0"));
        assert!(usage.json_enabled);
    }

    #[test]
    fn usage_new_creates_standalone() {
        let usage = Usage::new("tool", "desc")
            .with_biblical("Prov 3:5")
            .add_section("COMMANDS", &["status  Show status"]);
        assert_eq!(usage.sections.len(), 1);
        assert_eq!(usage.sections[0].title, "COMMANDS");
    }

    #[test]
    fn usage_with_standard_flags_includes_help() {
        let parsed = App::new("t", "d").parse(&[]).unwrap();
        let usage = Usage::from_parsed(&parsed).with_standard_flags();
        assert_eq!(usage.sections.len(), 1);
        assert_eq!(usage.sections[0].title, "FLAGS");
        assert!(usage.sections[0].lines.iter().any(|l| l.contains("--help")));
    }

    #[test]
    fn usage_with_standard_flags_includes_json_when_enabled() {
        let parsed = App::new("t", "d").with_json().parse(&[]).unwrap();
        let usage = Usage::from_parsed(&parsed).with_standard_flags();
        assert!(usage.sections[0].lines.iter().any(|l| l.contains("--json")));
    }

    #[test]
    fn usage_with_standard_flags_includes_version_when_set() {
        let parsed = App::new("t", "d").with_version("1.0.0").parse(&[]).unwrap();
        let usage = Usage::from_parsed(&parsed).with_standard_flags();
        assert!(
            usage.sections[0]
                .lines
                .iter()
                .any(|l| l.contains("--version"))
        );
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library crate — imported and called, not executed directly.
// Identity registration via register_identity() in BODY §7.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Pure types + free functions. No resources to clean up. No Drop needed.

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Remove not the ancient landmark, which thy fathers have set."
// — Proverbs 22:28
//
// Never:
//   - Break the typestate boundary (App -> ParsedApp)
//   - Remove standard flags (--help, -h) — universal convention
//   - Change 4-block structure (METADATA → SETUP → BODY → CLOSING)
//   - Change &[(&str, &str)] identity format (I1-I4, C1-C7)
//   - Add non-std dependencies (this crate MUST stay zero-dep + identity)
//
// Careful:
//   - App/ParsedApp struct fields — breaks internal pattern matching
//   - Standard flag names (--help, -h, --json, --version, -V)
//   - Builder method signatures — breaks chaining patterns
//   - CliError variants — downstream may match on specific variants
//   - exit_code() mappings — scripts may depend on specific codes
//
// Safe:
//   - Adding new ParsedApp query methods
//   - Adding new output helpers (fatal variants, formatting)
//   - Extending Usage builder (new section types, formatting options)
//   - Adding new CliError variants (append, don't remove)
//   - Adding new builder methods on App (with_* pattern)

// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Completed (b-03.00):
//   - I1-I4/C1-C7 metadata alignment
//   - Version flag support (--version, -V)
//   - ParsedApp convenience accessors (first_arg, has_args, arg_count)
//   - CliError::exit_code() for standardized process exits
//   - Usage::with_standard_flags() auto-generation
//   - Display impl for Usage (string capture, flexible output)
//   - ParsedApp Clone support
//   - Numbered BODY subsections matching seed template
//   - Standard CLOSING zones (Cv, Ce, Cc, X1-X5)
//
// Designed Growth Points:
//   - Custom flag types (string flags: --output=file, integer flags: --count=5)
//   - Subcommand support (App::subcommand("status", handler))
//   - Flag groups / mutual exclusion
//   - #[non_exhaustive] on CliError when downstream consumers stabilize
//   - Shell completion generation (bash, zsh, fish)
//
// Future Considerations:
//   - C ABI bridge for libtrit integration
//   - Async parse pipeline for streaming arguments
//   - Configuration file integration (merge file + CLI args)

// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lack wisdom, let him ask of God." — James 1:5
//
// "--version not enabled" error:
//   Call .with_version("x.y.z") on the App builder before parse().
//   Without it, --version and -V are treated as unknown flags.
//
// "--json not enabled" error:
//   Call .with_json() on the App builder before parse().
//   JSON mode is opt-in per command.
//
// Typestate confusion:
//   App methods: new, with_biblical, with_version, with_json, parse, parse_os
//   ParsedApp methods: wants_help, wants_json, wants_version, args, etc.
//   If the compiler says "method not found", check which type you're calling on.
//
// Doc tests fail:
//   The PRAGMA/METADATA accessor doc tests use the crate name
//   (bereshit_l0_cli). Ensure Cargo.toml name matches.

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Dependencies:   std only (env, process, fmt) + bereshit-l0-identity
// Dependents:     all CPI-SI skill CLI commands
// Parallel:       interface/cli/go/cli.go (Go version)
// Template:       b-word/seed/code/L0/rust/library.rs
//
// Validation:
//   Build:    cargo build -p bereshit-l0-cli
//   Test:     cargo test -p bereshit-l0-cli
//   Clippy:   cargo clippy -p bereshit-l0-cli -- -D warnings
//   Doc:      cargo doc -p bereshit-l0-cli --no-deps
//   All:      cargo clippy -p bereshit-l0-cli -- -D warnings && cargo test -p bereshit-l0-cli

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// CLI scaffolding — the interface between human intent and system action.
// Typestate enforcement means parse errors are caught at compile time,
// not discovered in production. The Rust version makes impossible states
// impossible — something the Go version could only enforce by convention.
//
// "The heart of the righteous studieth to answer."
// — Proverbs 15:28
//
// Study before answering. Parse before acting. The discipline of
// understanding input fully before responding is both engineering
// wisdom and spiritual principle.
//
// Genesis 1:1 — In the beginning God created the heaven and the earth.

// ============================================================================
// END CLOSING
// ============================================================================
