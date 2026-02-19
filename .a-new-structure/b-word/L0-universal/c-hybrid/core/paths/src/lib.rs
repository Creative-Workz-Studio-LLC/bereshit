//omni:code --rust -library
//omni:key B-L0-hybrid-paths
//omni:version b-03.00
//omni:pragma PRAGMA
//omni:metadata METADATA

//! # bereshit-l0-paths
//!
//! Centralized path resolution for the CPI-SI system.
//!
//! Callers never hardcode locations — this crate is the single source of truth.
//! Three resolution tiers: environment variable → hardcoded default → fallback.
//!
//! ## Two APIs
//!
//! | API | Use | Example |
//! |-----|-----|---------|
//! | Free functions | Production code | `paths::bereshit_root()` |
//! | [`PathResolver`] | Testing + custom roots | `resolver.bereshit_word()` |
//!
//! Free functions delegate to a global [`PathResolver`] constructed on first
//! access. For testing, create a resolver with [`PathResolver::with_roots`]
//! to inject custom base paths.
//!
//! ```
//! use bereshit_l0_paths as paths;
//!
//! let root = paths::bereshit_root();
//! let word = paths::bereshit_word();
//! assert!(word.ends_with("word"));
//! ```
//!
//! ## Environment Variables
//!
//! | Variable | Overrides |
//! |----------|-----------|
//! | `BERESHIT_ROOT` | [`bereshit_root`] project root |
//! | `CLAUDE_GLOBAL_ROOT` | [`claude_global_root`] claude-global root |
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
// Key:     B-L0-hybrid-paths
// Purpose: Centralized path resolution for all CPI-SI system components
// Biblical: Proverbs 4:26 — Ponder the path of thy feet
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
/// let key = bereshit_l0_paths::pragma_get("I1.key").unwrap_or("unknown");
/// assert_eq!(key, "B-L0-hybrid-paths");
/// ```
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-hybrid-paths"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/library.rs"),
    ("I1.at",        "b-03.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I2.role",      "foundation"),
    // I3: Instance
    ("I3.file",      "lib.rs"),
    ("I3.title",     "Path Resolution Library"),
    ("I3.component", "c-hybrid/core/paths"),
    ("I3.path",      "L0-universal/c-hybrid/core/paths/src/lib.rs"),
    ("I3.provides",  "PATH_RESOLUTION"),
    ("I3.brief",     "Centralized path construction for all CPI-SI system components"),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "hybrid/core/paths"),
    ("I4.pattern",   "centralized resolution — single crate owns all path logic"),
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
/// let version = bereshit_l0_paths::metadata_get("C1.version").unwrap_or("unknown");
/// assert_eq!(version, "b-03.00");
/// ```
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-03.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2025-12-09"),
    ("C1.updated",           "2026-02-18"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Proverbs 4:26 — Ponder the path of thy feet"),
    ("C3.principle",         "Know where you walk — every path resolved before traversed"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "std::env, std::fmt, std::fs, std::io, std::path, std::sync"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "bereshit-l0-identity (registration)"),
    ("C4.consumers",         "config, pragma, database — any crate resolving system paths"),
    ("C4.integration",       "use bereshit_l0_paths::{bereshit_root, claude_home, PathResolver}"),
    ("C4.if_missing",        "every crate hardcodes paths — changes require N updates instead of 1"),
    // C5: Intent
    ("C5.purpose",           "Single source of truth for all system paths"),
    ("C5.philosophy",        "Centralize path logic so changes propagate from one place"),
    // C6: Roadmap
    ("C6.current",           "b-03.00 — PathResolver for testability, I/C metadata, 50+ tests"),
    ("C6.planned",           "XDG compliance, path validation, canonicalization"),
    ("C6.limitations",       "CWS workstation paths as defaults, Linux only"),
    // C7: Classification
    ("C7.tags",              "paths, resolution, config, bereshit, claude, database, logging, filesystem"),
    ("C7.category",          "Foundation"),
    ("C7.domain",            "filesystem"),
    ("C7.paradigm",          "CPI-SI"),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// section-order: [s01]-imports, [s04]-statics, [s07]-types

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use std::env;
use std::fmt;
use std::fs;
use std::io;
use std::path::{Path, PathBuf};
use std::sync::OnceLock;

// ──────────────────────────────────────────────────────────────────────────
// 4. Statics
// ──────────────────────────────────────────────────────────────────────────

/// Global resolver, constructed on first access via [`resolver()`].
static RESOLVER: OnceLock<PathResolver> = OnceLock::new();

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

/// Centralized path resolver for all CPI-SI system paths.
///
/// Caches resolved root directories at construction time. All path composition
/// methods return values derived from these three cached roots:
///
/// | Root | Source | Used by |
/// |------|--------|---------|
/// | `home` | `$HOME` env var | Claude paths, global config |
/// | `bereshit` | `$BERESHIT_ROOT` or known locations | Project paths |
/// | `claude_global` | `$CLAUDE_GLOBAL_ROOT` or resolution cascade | Config, database, logs |
///
/// # Two construction modes
///
/// | Constructor | Use |
/// |-------------|-----|
/// | [`from_env`](Self::from_env) | Production — resolves from env vars and filesystem |
/// | [`with_roots`](Self::with_roots) | Testing — accepts explicit root paths |
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct PathResolver {
    home: PathBuf,
    bereshit: PathBuf,
    claude_global: PathBuf,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Subsection order:
//   1. Identity Access     — pragma/metadata accessors
//   3. Constructors        — PathResolver::from_env, with_roots
//   4. Core Logic          — resolution helpers, config directory finding
//   5. Queries & Accessors — path composition methods by domain
//   6. Output & Display    — PathResolver Display impl
//   7. Free Functions      — global resolver, public API wrappers, registration

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────

/// Returns this crate's OmniCode pragma identity (I1-I4).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Look up a single pragma value by key.
///
/// ```
/// use bereshit_l0_paths::pragma_get;
///
/// assert_eq!(pragma_get("I1.key"), Some("B-L0-hybrid-paths"));
/// assert_eq!(pragma_get("I1.format"), Some("rust"));
/// assert_eq!(pragma_get("missing"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (C1-C7).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Look up a single metadata value by key.
///
/// ```
/// use bereshit_l0_paths::metadata_get;
///
/// assert_eq!(metadata_get("C1.version"), Some("b-03.00"));
/// assert_eq!(metadata_get("missing"), None);
/// ```
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors
// ──────────────────────────────────────────────────────────────────────────

impl PathResolver {
    /// Resolve all roots from environment variables and filesystem.
    ///
    /// Resolution order:
    /// 1. `$HOME` → cached as home root
    /// 2. `$BERESHIT_ROOT` → or known locations → or `"bereshit"` fallback
    /// 3. `$CLAUDE_GLOBAL_ROOT` → or production locations → or dev location → fallback
    ///
    /// This is the production constructor. Environment variables are read once
    /// at construction time and cached for the resolver's lifetime.
    pub fn from_env() -> Self {
        let home = env::var("HOME").map(PathBuf::from).unwrap_or_default();
        let bereshit = Self::resolve_bereshit(&home);
        let claude_global = Self::resolve_claude_global(&home, &bereshit);
        Self { home, bereshit, claude_global }
    }

    /// Create a resolver with explicit root paths.
    ///
    /// Bypasses all environment variable and filesystem resolution. Use this
    /// in tests to inject known roots and verify path composition without
    /// depending on the real filesystem.
    ///
    /// ```
    /// use bereshit_l0_paths::PathResolver;
    /// use std::path::PathBuf;
    ///
    /// let r = PathResolver::with_roots("/home/test", "/opt/bereshit", "/opt/global");
    /// assert_eq!(r.bereshit_word(), PathBuf::from("/opt/bereshit/word"));
    /// ```
    pub fn with_roots(
        home: impl Into<PathBuf>,
        bereshit: impl Into<PathBuf>,
        claude_global: impl Into<PathBuf>,
    ) -> Self {
        Self {
            home: home.into(),
            bereshit: bereshit.into(),
            claude_global: claude_global.into(),
        }
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

impl PathResolver {
    /// Resolve Bereshit project root from env var or known locations.
    ///
    /// Three-tier cascade:
    /// 1. `$BERESHIT_ROOT` (highest priority)
    /// 2. Known filesystem locations (checked with `is_dir`)
    /// 3. `"bereshit"` relative fallback
    fn resolve_bereshit(home: &Path) -> PathBuf {
        if let Ok(root) = env::var("BERESHIT_ROOT") {
            return PathBuf::from(root);
        }

        let locations = [
            PathBuf::from("/media/seanje-lenox-wise/Project/Bereshit"),
            home.join("Project").join("Bereshit"),
        ];

        for loc in &locations {
            if loc.is_dir() {
                return loc.clone();
            }
        }

        PathBuf::from("bereshit")
    }

    /// Resolve claude-global root from env var or location cascade.
    ///
    /// Four-tier cascade:
    /// 1. `$CLAUDE_GLOBAL_ROOT` (highest priority)
    /// 2. `bereshit/word/claude` (production — merged into Bereshit repo)
    /// 3. `~/.claude/global` (production — standalone install)
    /// 4. `./claude-global` (development — local clone)
    /// 5. `~/.claude/global` fallback (guaranteed path)
    fn resolve_claude_global(home: &Path, bereshit: &Path) -> PathBuf {
        if let Ok(root) = env::var("CLAUDE_GLOBAL_ROOT") {
            return PathBuf::from(root);
        }

        let prod = [
            bereshit.join("word").join("claude"),
            home.join(".claude").join("global"),
        ];
        for loc in &prod {
            if loc.is_dir() {
                return loc.clone();
            }
        }

        let dev = PathBuf::from("claude-global");
        if dev.is_dir() {
            return dev;
        }

        home.join(".claude").join("global")
    }
}

/// Locate a `config` directory relative to the binary or cwd.
///
/// Convenience wrapper for [`find_config_dir_named`] with the default
/// name `"config"`. Standard pattern for CPI-SI skill config loading.
pub fn find_config_dir() -> io::Result<PathBuf> {
    find_config_dir_named("config")
}

/// Locate a named config directory by searching:
///
/// 1. Relative to executable (`bin/` → `../../<name>`) — deployed layout
/// 2. Relative to cwd (`../<name>`) — running from `scripts/` during dev
/// 3. Relative to cwd (`./<name>`) — running from skill root during dev
///
/// Returns `io::ErrorKind::NotFound` if no matching directory exists.
///
/// # Errors
///
/// Returns an error if no directory named `name` is found in any
/// of the search locations.
pub fn find_config_dir_named(name: &str) -> io::Result<PathBuf> {
    if let Ok(exe) = env::current_exe() {
        if let Some(exe_dir) = exe.parent() {
            let config = exe_dir.join("..").join("..").join(name);
            if config.is_dir() {
                return Ok(config);
            }
        }
    }

    if let Ok(cwd) = env::current_dir() {
        let config = cwd.join("..").join(name);
        if config.is_dir() {
            return Ok(config);
        }
        let config = cwd.join(name);
        if config.is_dir() {
            return Ok(config);
        }
    }

    Err(io::Error::new(
        io::ErrorKind::NotFound,
        format!("config directory '{name}' not found"),
    ))
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Queries & Accessors
// ──────────────────────────────────────────────────────────────────────────
//
// Pure path composition — each method joins segments onto cached roots.
// Organized by domain.

impl PathResolver {
    // ── Root Accessors ──────────────────────────────────────────────

    /// Bereshit project root directory.
    pub fn bereshit_root(&self) -> &Path {
        &self.bereshit
    }

    /// Claude-global root directory.
    pub fn claude_global_root(&self) -> &Path {
        &self.claude_global
    }

    /// Home directory.
    pub fn home_dir(&self) -> &Path {
        &self.home
    }

    // ── Bereshit ────────────────────────────────────────────────────

    /// `bereshit/word/` — definitions and implementations.
    pub fn bereshit_word(&self) -> PathBuf {
        self.bereshit.join("word")
    }

    /// `bereshit/word/seed/` — templates.
    pub fn bereshit_seed(&self) -> PathBuf {
        self.bereshit_word().join("seed")
    }

    /// `bereshit/word/omni/seed/` — OmniCode templates.
    pub fn bereshit_omni_seed(&self) -> PathBuf {
        self.bereshit_word().join("omni").join("seed")
    }

    // ── Claude ──────────────────────────────────────────────────────

    /// `~/.claude` directory.
    pub fn claude_home(&self) -> PathBuf {
        self.home.join(".claude")
    }

    /// `~/.claude/session` directory.
    pub fn claude_session(&self) -> PathBuf {
        self.claude_home().join("session")
    }

    /// `~/.claude/system` directory.
    pub fn claude_system(&self) -> PathBuf {
        self.claude_home().join("system")
    }

    /// `~/.claude/cpi-si/skills` directory.
    pub fn claude_skills(&self) -> PathBuf {
        self.claude_home().join("cpi-si").join("skills")
    }

    // ── Claude Global ───────────────────────────────────────────────

    /// Config directory within claude-global.
    pub fn claude_global_config(&self) -> PathBuf {
        self.claude_global.join("config")
    }

    // ── CPI-SI Schema ───────────────────────────────────────────────

    /// Base CPI-SI schema config directory.
    pub fn cpisi_schema_config(&self) -> PathBuf {
        self.claude_global.join("pkg").join("cpisi").join("schema").join("config")
    }

    // ── State Machine ───────────────────────────────────────────────

    /// L2 vocabulary TOML directory (schema source of truth).
    pub fn statemachine_schema_dir(&self) -> PathBuf {
        self.cpisi_schema_config().join("l2")
    }

    /// Runtime state directory (session-specific state files).
    pub fn statemachine_runtime_dir(&self) -> PathBuf {
        self.claude_global_config().join("statemachine").join("runtime")
    }

    /// Path to `state.jsonc` runtime file.
    pub fn statemachine_runtime_state(&self) -> PathBuf {
        self.statemachine_runtime_dir().join("state.jsonc")
    }

    /// Path to `path.jsonc` runtime file.
    pub fn statemachine_runtime_path(&self) -> PathBuf {
        self.statemachine_runtime_dir().join("path.jsonc")
    }

    /// Path to `history.jsonc` runtime file.
    pub fn statemachine_runtime_history(&self) -> PathBuf {
        self.statemachine_runtime_dir().join("history.jsonc")
    }

    /// Path to `choice_history.jsonc` runtime file.
    pub fn statemachine_choice_history(&self) -> PathBuf {
        self.statemachine_runtime_dir().join("choice_history.jsonc")
    }

    // ── Database ────────────────────────────────────────────────────

    /// Data directory within claude-global.
    pub fn database_dir(&self) -> PathBuf {
        self.claude_global.join("data")
    }

    /// Path to the main CPI-SI database (`cpisi.db`).
    pub fn database_path(&self) -> PathBuf {
        self.database_dir().join("cpisi.db")
    }

    /// Database schema directory.
    pub fn database_schema_dir(&self) -> PathBuf {
        self.database_dir().join("schema")
    }

    /// Database migrations directory.
    pub fn database_migrations_dir(&self) -> PathBuf {
        self.database_dir().join("migrations")
    }

    // ── Logging ─────────────────────────────────────────────────────

    /// Root CPI-SI logs directory.
    pub fn logs_dir(&self) -> PathBuf {
        self.database_dir().join("logs")
    }

    /// Session logs directory.
    pub fn logs_session_dir(&self) -> PathBuf {
        self.logs_dir().join("session")
    }

    /// Trajectory logs directory.
    pub fn logs_trajectory_dir(&self) -> PathBuf {
        self.logs_dir().join("trajectory")
    }

    /// Cognition logs directory.
    pub fn logs_cognition_dir(&self) -> PathBuf {
        self.logs_dir().join("cognition")
    }

    /// Health logs directory.
    pub fn logs_health_dir(&self) -> PathBuf {
        self.logs_dir().join("health")
    }

    /// Tools logs directory.
    pub fn logs_tools_dir(&self) -> PathBuf {
        self.logs_dir().join("tools")
    }

    /// Context logs directory.
    pub fn logs_context_dir(&self) -> PathBuf {
        self.logs_dir().join("context")
    }

    // ── Health ───────────────────────────────────────────────────────

    /// Health config directory.
    pub fn health_config_dir(&self) -> PathBuf {
        self.claude_global_config().join("health")
    }

    /// Health runtime directory.
    pub fn health_runtime_dir(&self) -> PathBuf {
        self.health_config_dir().join("runtime")
    }

    // ── Debug and Restore ───────────────────────────────────────────

    /// Debug directory within claude-global/data/.
    pub fn debug_dir(&self) -> PathBuf {
        self.database_dir().join("debug")
    }

    /// Restore directory within claude-global/data/.
    pub fn restore_dir(&self) -> PathBuf {
        self.database_dir().join("restore")
    }

    /// Checkpoints subdirectory of restore.
    pub fn restore_checkpoints_dir(&self) -> PathBuf {
        self.restore_dir().join("checkpoints")
    }

    // ── Formats ─────────────────────────────────────────────────────

    /// Formats specification directory.
    pub fn formats_dir(&self) -> PathBuf {
        self.database_dir().join("formats")
    }

    // ── Directory Initialization ────────────────────────────────────
    //
    // Create directories on demand. Each method calls `fs::create_dir_all`
    // which is idempotent — safe to call multiple times.

    /// Create the data directory if it doesn't exist.
    pub fn ensure_database_dir(&self) -> io::Result<()> {
        fs::create_dir_all(self.database_dir())
    }

    /// Create all log subdirectories if they don't exist.
    pub fn ensure_logs_dir(&self) -> io::Result<()> {
        let dirs = [
            self.logs_dir(),
            self.logs_session_dir(),
            self.logs_trajectory_dir(),
            self.logs_cognition_dir(),
            self.logs_health_dir(),
            self.logs_tools_dir(),
            self.logs_context_dir(),
        ];
        for dir in &dirs {
            fs::create_dir_all(dir)?;
        }
        Ok(())
    }

    /// Create the debug directory if it doesn't exist.
    pub fn ensure_debug_dir(&self) -> io::Result<()> {
        fs::create_dir_all(self.debug_dir())
    }

    /// Create the restore directories if they don't exist.
    pub fn ensure_restore_dir(&self) -> io::Result<()> {
        fs::create_dir_all(self.restore_dir())?;
        fs::create_dir_all(self.restore_checkpoints_dir())
    }

    /// Create the formats directory if it doesn't exist.
    pub fn ensure_formats_dir(&self) -> io::Result<()> {
        fs::create_dir_all(self.formats_dir())
    }

    /// Create all data subdirectories in dependency order.
    pub fn ensure_all_data_dirs(&self) -> io::Result<()> {
        self.ensure_database_dir()?;
        self.ensure_logs_dir()?;
        self.ensure_debug_dir()?;
        self.ensure_restore_dir()?;
        self.ensure_formats_dir()
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 6. Output & Display
// ──────────────────────────────────────────────────────────────────────────

impl fmt::Display for PathResolver {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "PathResolver(home={}, bereshit={}, claude_global={})",
            self.home.display(),
            self.bereshit.display(),
            self.claude_global.display(),
        )
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────
//
// Public API wrappers that delegate to the global PathResolver.
// These maintain backward compatibility — callers use `paths::bereshit_root()`
// without knowing about PathResolver.

/// Returns the global path resolver, constructing it on first access.
fn resolver() -> &'static PathResolver {
    RESOLVER.get_or_init(PathResolver::from_env)
}

// ── Bereshit ────────────────────────────────────────────────────────

/// Bereshit project root directory.
pub fn bereshit_root() -> PathBuf {
    resolver().bereshit_root().to_path_buf()
}

/// `bereshit/word/` — definitions and implementations.
pub fn bereshit_word() -> PathBuf {
    resolver().bereshit_word()
}

/// `bereshit/word/seed/` — templates.
pub fn bereshit_seed() -> PathBuf {
    resolver().bereshit_seed()
}

/// `bereshit/word/omni/seed/` — OmniCode templates.
pub fn bereshit_omni_seed() -> PathBuf {
    resolver().bereshit_omni_seed()
}

// ── Claude ──────────────────────────────────────────────────────────

/// `~/.claude` directory.
pub fn claude_home() -> PathBuf {
    resolver().claude_home()
}

/// `~/.claude/session` directory.
pub fn claude_session() -> PathBuf {
    resolver().claude_session()
}

/// `~/.claude/system` directory.
pub fn claude_system() -> PathBuf {
    resolver().claude_system()
}

/// `~/.claude/cpi-si/skills` directory.
pub fn claude_skills() -> PathBuf {
    resolver().claude_skills()
}

// ── Claude Global ───────────────────────────────────────────────────

/// Claude-global root directory.
pub fn claude_global_root() -> PathBuf {
    resolver().claude_global_root().to_path_buf()
}

/// Config directory within claude-global.
pub fn claude_global_config() -> PathBuf {
    resolver().claude_global_config()
}

// ── CPI-SI Schema ───────────────────────────────────────────────────

/// Base CPI-SI schema config directory.
pub fn cpisi_schema_config() -> PathBuf {
    resolver().cpisi_schema_config()
}

// ── State Machine ───────────────────────────────────────────────────

/// L2 vocabulary TOML directory.
pub fn statemachine_schema_dir() -> PathBuf {
    resolver().statemachine_schema_dir()
}

/// Runtime state directory.
pub fn statemachine_runtime_dir() -> PathBuf {
    resolver().statemachine_runtime_dir()
}

/// Path to `state.jsonc` runtime file.
pub fn statemachine_runtime_state() -> PathBuf {
    resolver().statemachine_runtime_state()
}

/// Path to `path.jsonc` runtime file.
pub fn statemachine_runtime_path() -> PathBuf {
    resolver().statemachine_runtime_path()
}

/// Path to `history.jsonc` runtime file.
pub fn statemachine_runtime_history() -> PathBuf {
    resolver().statemachine_runtime_history()
}

/// Path to `choice_history.jsonc` runtime file.
pub fn statemachine_choice_history() -> PathBuf {
    resolver().statemachine_choice_history()
}

// ── Database ────────────────────────────────────────────────────────

/// Data directory within claude-global.
pub fn database_dir() -> PathBuf {
    resolver().database_dir()
}

/// Path to the main CPI-SI database.
pub fn database_path() -> PathBuf {
    resolver().database_path()
}

/// Database schema directory.
pub fn database_schema_dir() -> PathBuf {
    resolver().database_schema_dir()
}

/// Database migrations directory.
pub fn database_migrations_dir() -> PathBuf {
    resolver().database_migrations_dir()
}

// ── Logging ─────────────────────────────────────────────────────────

/// Root CPI-SI logs directory.
pub fn logs_dir() -> PathBuf {
    resolver().logs_dir()
}

/// Session logs directory.
pub fn logs_session_dir() -> PathBuf {
    resolver().logs_session_dir()
}

/// Trajectory logs directory.
pub fn logs_trajectory_dir() -> PathBuf {
    resolver().logs_trajectory_dir()
}

/// Cognition logs directory.
pub fn logs_cognition_dir() -> PathBuf {
    resolver().logs_cognition_dir()
}

/// Health logs directory.
pub fn logs_health_dir() -> PathBuf {
    resolver().logs_health_dir()
}

/// Tools logs directory.
pub fn logs_tools_dir() -> PathBuf {
    resolver().logs_tools_dir()
}

/// Context logs directory.
pub fn logs_context_dir() -> PathBuf {
    resolver().logs_context_dir()
}

// ── Health ───────────────────────────────────────────────────────────

/// Health config directory.
pub fn health_config_dir() -> PathBuf {
    resolver().health_config_dir()
}

/// Health runtime directory.
pub fn health_runtime_dir() -> PathBuf {
    resolver().health_runtime_dir()
}

// ── Debug/Restore/Formats ───────────────────────────────────────────

/// Debug directory.
pub fn debug_dir() -> PathBuf {
    resolver().debug_dir()
}

/// Restore directory.
pub fn restore_dir() -> PathBuf {
    resolver().restore_dir()
}

/// Checkpoints subdirectory.
pub fn restore_checkpoints_dir() -> PathBuf {
    resolver().restore_checkpoints_dir()
}

/// Formats specification directory.
pub fn formats_dir() -> PathBuf {
    resolver().formats_dir()
}

// ── Directory Initialization ────────────────────────────────────────

/// Create the data directory if it doesn't exist.
pub fn ensure_database_dir() -> io::Result<()> {
    resolver().ensure_database_dir()
}

/// Create all log subdirectories if they don't exist.
pub fn ensure_logs_dir() -> io::Result<()> {
    resolver().ensure_logs_dir()
}

/// Create the debug directory if it doesn't exist.
pub fn ensure_debug_dir() -> io::Result<()> {
    resolver().ensure_debug_dir()
}

/// Create the restore directories if they don't exist.
pub fn ensure_restore_dir() -> io::Result<()> {
    resolver().ensure_restore_dir()
}

/// Create the formats directory if it doesn't exist.
pub fn ensure_formats_dir() -> io::Result<()> {
    resolver().ensure_formats_dir()
}

/// Create all data subdirectories in dependency order.
pub fn ensure_all_data_dirs() -> io::Result<()> {
    resolver().ensure_all_data_dirs()
}

// ── Identity Registration ───────────────────────────────────────────

/// Register this crate's identity with the global registry.
///
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
// 3 Code Zones (operations):  Cv → Ce → Cc
// 3 Doc Sections (guidance):  X1 → X2 → X5

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    // ── Identity ────────────────────────────────────────────────────

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("I1.key"), Some("B-L0-hybrid-paths"));
    }

    #[test]
    fn pragma_contains_format() {
        assert_eq!(pragma_get("I1.format"), Some("rust"));
    }

    #[test]
    fn pragma_get_missing_returns_none() {
        assert_eq!(pragma_get("nonexistent"), None);
    }

    #[test]
    fn pragma_slice_not_empty() {
        assert!(!pragma().is_empty());
    }

    #[test]
    fn metadata_contains_version() {
        assert_eq!(metadata_get("C1.version"), Some("b-03.00"));
    }

    #[test]
    fn metadata_contains_scripture() {
        let s = metadata_get("C3.scripture").unwrap();
        assert!(s.contains("Proverbs 4:26"));
    }

    #[test]
    fn metadata_get_missing_returns_none() {
        assert_eq!(metadata_get("nonexistent"), None);
    }

    #[test]
    fn metadata_slice_not_empty() {
        assert!(!metadata().is_empty());
    }

    // ── PathResolver Construction ───────────────────────────────────

    #[test]
    fn with_roots_stores_exact_paths() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert_eq!(r.home_dir(), Path::new("/h"));
        assert_eq!(r.bereshit_root(), Path::new("/b"));
        assert_eq!(r.claude_global_root(), Path::new("/g"));
    }

    #[test]
    fn from_env_returns_non_empty_roots() {
        let r = PathResolver::from_env();
        assert!(!r.home_dir().as_os_str().is_empty());
        assert!(!r.bereshit_root().as_os_str().is_empty());
        assert!(!r.claude_global_root().as_os_str().is_empty());
    }

    #[test]
    fn display_shows_all_roots() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        let s = r.to_string();
        assert!(s.contains("/h"), "display should contain home");
        assert!(s.contains("/b"), "display should contain bereshit");
        assert!(s.contains("/g"), "display should contain claude_global");
    }

    #[test]
    fn clone_equals_original() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        let cloned = r.clone();
        assert_eq!(r, cloned);
    }

    // ── Bereshit Path Composition ───────────────────────────────────

    #[test]
    fn bereshit_word_exact() {
        let r = PathResolver::with_roots("/h", "/proj/bereshit", "/g");
        assert_eq!(r.bereshit_word(), PathBuf::from("/proj/bereshit/word"));
    }

    #[test]
    fn bereshit_seed_exact() {
        let r = PathResolver::with_roots("/h", "/proj/bereshit", "/g");
        assert_eq!(r.bereshit_seed(), PathBuf::from("/proj/bereshit/word/seed"));
    }

    #[test]
    fn bereshit_omni_seed_exact() {
        let r = PathResolver::with_roots("/h", "/proj/bereshit", "/g");
        assert_eq!(
            r.bereshit_omni_seed(),
            PathBuf::from("/proj/bereshit/word/omni/seed"),
        );
    }

    #[test]
    fn bereshit_seed_is_child_of_word() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert!(r.bereshit_seed().starts_with(&r.bereshit_word()));
    }

    // ── Claude Path Composition ─────────────────────────────────────

    #[test]
    fn claude_home_exact() {
        let r = PathResolver::with_roots("/home/user", "/b", "/g");
        assert_eq!(r.claude_home(), PathBuf::from("/home/user/.claude"));
    }

    #[test]
    fn claude_session_exact() {
        let r = PathResolver::with_roots("/home/user", "/b", "/g");
        assert_eq!(r.claude_session(), PathBuf::from("/home/user/.claude/session"));
    }

    #[test]
    fn claude_system_is_child_of_claude_home() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert!(r.claude_system().starts_with(&r.claude_home()));
    }

    #[test]
    fn claude_skills_exact() {
        let r = PathResolver::with_roots("/home/user", "/b", "/g");
        assert_eq!(
            r.claude_skills(),
            PathBuf::from("/home/user/.claude/cpi-si/skills"),
        );
    }

    // ── Claude Global Composition ───────────────────────────────────

    #[test]
    fn claude_global_config_exact() {
        let r = PathResolver::with_roots("/h", "/b", "/opt/global");
        assert_eq!(r.claude_global_config(), PathBuf::from("/opt/global/config"));
    }

    // ── CPI-SI Schema ───────────────────────────────────────────────

    #[test]
    fn cpisi_schema_config_exact() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert_eq!(
            r.cpisi_schema_config(),
            PathBuf::from("/g/pkg/cpisi/schema/config"),
        );
    }

    // ── State Machine ───────────────────────────────────────────────

    #[test]
    fn statemachine_schema_dir_exact() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert_eq!(
            r.statemachine_schema_dir(),
            PathBuf::from("/g/pkg/cpisi/schema/config/l2"),
        );
    }

    #[test]
    fn statemachine_runtime_dir_exact() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert_eq!(
            r.statemachine_runtime_dir(),
            PathBuf::from("/g/config/statemachine/runtime"),
        );
    }

    #[test]
    fn statemachine_runtime_files_share_parent() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        let parent = r.statemachine_runtime_dir();
        assert!(r.statemachine_runtime_state().starts_with(&parent));
        assert!(r.statemachine_runtime_path().starts_with(&parent));
        assert!(r.statemachine_runtime_history().starts_with(&parent));
        assert!(r.statemachine_choice_history().starts_with(&parent));
    }

    // ── Database ────────────────────────────────────────────────────

    #[test]
    fn database_dir_exact() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert_eq!(r.database_dir(), PathBuf::from("/g/data"));
    }

    #[test]
    fn database_path_exact() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert_eq!(r.database_path(), PathBuf::from("/g/data/cpisi.db"));
    }

    #[test]
    fn database_path_parent_is_database_dir() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert_eq!(r.database_path().parent().unwrap(), r.database_dir());
    }

    // ── Logging ─────────────────────────────────────────────────────

    #[test]
    fn logs_dir_is_child_of_data() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert!(r.logs_dir().starts_with(&r.database_dir()));
    }

    #[test]
    fn logs_subdirs_have_correct_names() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert!(r.logs_session_dir().ends_with("session"));
        assert!(r.logs_trajectory_dir().ends_with("trajectory"));
        assert!(r.logs_cognition_dir().ends_with("cognition"));
        assert!(r.logs_health_dir().ends_with("health"));
        assert!(r.logs_tools_dir().ends_with("tools"));
        assert!(r.logs_context_dir().ends_with("context"));
    }

    #[test]
    fn logs_subdirs_share_parent() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        let parent = r.logs_dir();
        let dirs = [
            r.logs_session_dir(),
            r.logs_trajectory_dir(),
            r.logs_cognition_dir(),
            r.logs_health_dir(),
            r.logs_tools_dir(),
            r.logs_context_dir(),
        ];
        for dir in &dirs {
            assert_eq!(dir.parent().unwrap(), parent, "log subdir parent mismatch");
        }
    }

    // ── Health ──────────────────────────────────────────────────────

    #[test]
    fn health_config_dir_exact() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert_eq!(r.health_config_dir(), PathBuf::from("/g/config/health"));
    }

    #[test]
    fn health_runtime_dir_is_child_of_health_config() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert!(r.health_runtime_dir().starts_with(&r.health_config_dir()));
    }

    // ── Debug/Restore/Formats ───────────────────────────────────────

    #[test]
    fn debug_restore_formats_are_children_of_data() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        let data = r.database_dir();
        assert!(r.debug_dir().starts_with(&data));
        assert!(r.restore_dir().starts_with(&data));
        assert!(r.formats_dir().starts_with(&data));
    }

    #[test]
    fn restore_checkpoints_is_child_of_restore() {
        let r = PathResolver::with_roots("/h", "/b", "/g");
        assert!(r.restore_checkpoints_dir().starts_with(&r.restore_dir()));
    }

    // ── Config Resolution ───────────────────────────────────────────

    #[test]
    fn find_config_dir_named_not_found() {
        let result = find_config_dir_named("nonexistent_config_dir_xyz");
        assert!(result.is_err());
        assert_eq!(result.unwrap_err().kind(), io::ErrorKind::NotFound);
    }

    #[test]
    fn find_config_dir_delegates_to_named() {
        match (find_config_dir_named("config"), find_config_dir()) {
            (Ok(a), Ok(b)) => assert_eq!(a, b),
            (Err(a), Err(b)) => assert_eq!(a.kind(), b.kind()),
            _ => panic!("find_config_dir and find_config_dir_named disagree"),
        }
    }

    // ── Free Function Delegation ────────────────────────────────────

    #[test]
    fn free_functions_match_resolver() {
        let r = resolver();
        assert_eq!(bereshit_root(), r.bereshit_root().to_path_buf());
        assert_eq!(bereshit_word(), r.bereshit_word());
        assert_eq!(claude_home(), r.claude_home());
        assert_eq!(claude_global_root(), r.claude_global_root().to_path_buf());
        assert_eq!(database_dir(), r.database_dir());
        assert_eq!(logs_dir(), r.logs_dir());
    }

    // ── Directory Initialization ────────────────────────────────────

    #[test]
    fn ensure_database_dir_creates_data() {
        let base = env::temp_dir().join("bereshit_paths_test_db");
        let _ = fs::remove_dir_all(&base);

        let r = PathResolver::with_roots("/h", "/b", &base);
        r.ensure_database_dir().unwrap();
        assert!(r.database_dir().is_dir());

        let _ = fs::remove_dir_all(&base);
    }

    #[test]
    fn ensure_logs_dir_creates_all_subdirs() {
        let base = env::temp_dir().join("bereshit_paths_test_logs");
        let _ = fs::remove_dir_all(&base);

        let r = PathResolver::with_roots("/h", "/b", &base);
        r.ensure_database_dir().unwrap(); // logs is under data
        r.ensure_logs_dir().unwrap();

        assert!(r.logs_dir().is_dir());
        assert!(r.logs_session_dir().is_dir());
        assert!(r.logs_trajectory_dir().is_dir());
        assert!(r.logs_cognition_dir().is_dir());
        assert!(r.logs_health_dir().is_dir());
        assert!(r.logs_tools_dir().is_dir());
        assert!(r.logs_context_dir().is_dir());

        let _ = fs::remove_dir_all(&base);
    }

    #[test]
    fn ensure_all_data_dirs_creates_complete_tree() {
        let base = env::temp_dir().join("bereshit_paths_test_all");
        let _ = fs::remove_dir_all(&base);

        let r = PathResolver::with_roots("/h", "/b", &base);
        r.ensure_all_data_dirs().unwrap();

        assert!(r.database_dir().is_dir());
        assert!(r.logs_dir().is_dir());
        assert!(r.logs_session_dir().is_dir());
        assert!(r.debug_dir().is_dir());
        assert!(r.restore_dir().is_dir());
        assert!(r.restore_checkpoints_dir().is_dir());
        assert!(r.formats_dir().is_dir());

        let _ = fs::remove_dir_all(&base);
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — imported, not executed directly.
// Registration via `register_identity()` called by consumers at startup.
//
// use bereshit_l0_paths as paths;
// paths::register_identity();

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// No resources to clean up. OnceLock persists for process lifetime.
// The RESOLVER static is intentionally never freed — process-global state.

// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Change free function signatures — 4 downstream crates depend on them
//   - Remove env var override support (BERESHIT_ROOT, CLAUDE_GLOBAL_ROOT)
//   - Add non-std external dependencies — this crate must stay lightweight
//   - Remove PathResolver — it enables all test coverage
//
// Careful:
//   - Changing resolution order in resolve_bereshit/resolve_claude_global
//   - Modifying hardcoded fallback paths (workstation-specific)
//   - Adding new ensure_* methods (each creates real directories)
//   - Changing PRAGMA/METADATA keys — breaks identity registry lookups
//
// Safe:
//   - Adding new path composition methods and free function wrappers
//   - Adding new domain sections in Queries (§5)
//   - Adding new tests
//   - Improving doc comments

// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Completed:
//   - PathResolver struct for testable path resolution
//   - I1-I4/C1-C7 metadata alignment with foundation pattern
//   - Numbered BODY subsections (§1, §3, §4, §5, §6, §7)
//   - Comprehensive tests: exact assertions, ensure verification, delegation
//
// Designed:
//   - XDG compliance (XDG_CONFIG_HOME, XDG_DATA_HOME)
//   - Path validation (verify directories exist before returning)
//   - Canonicalization (resolve symlinks and normalize paths)
//
// Future:
//   - Windows support (PathBuf handles separators, but defaults are Linux)
//   - Config file resolution (TOML/JSONC config pointing to custom roots)

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// Centralized path resolution promoted from ad-hoc free functions to a
// testable PathResolver pattern. The resolver caches roots at construction;
// free functions wrap it for backward compatibility. Three ensure tests
// prove directory creation works — something impossible before injection.
//
// "Ponder the path of thy feet, and let all thy ways be established."
// — Proverbs 4:26
//
// "In the beginning God created the heaven and the earth."
// — Genesis 1:1

// ============================================================================
// END CLOSING
// ============================================================================
