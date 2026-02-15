//omni:code --rust -library
//omni:key B-L0-hybrid-paths
//omni:version b-02.00
//omni:pragma PRAGMA
//omni:metadata METADATA

//! # bereshit-l0-paths
//!
//! Centralized path resolution for the CPI-SI system.
//!
//! Callers never hardcode locations — this crate is the single source of truth.
//! Three resolution tiers: environment variable → hardcoded default → fallback.
//!
//! ```
//! use bereshit_l0_paths as paths;
//!
//! let root = paths::bereshit_root();
//! let word = paths::bereshit_word();
//! assert!(word.ends_with("word"));
//! ```
//!
//! **Environment variables:**
//! - `BERESHIT_ROOT` — Override Bereshit project root
//! - `CLAUDE_GLOBAL_ROOT` — Override claude-global root

// ============================================================================
// METADATA
// ============================================================================

/// OmniCode pragma identity (P1-P5). Compile-time constants.
pub static PRAGMA: &[(&str, &str)] = &[
    // P1: Core Identity
    ("P1.key", "B-L0-hybrid-paths"),
    ("P1.type", "code"),
    ("P1.format", "rust"),
    ("P1.style", "library"),
    // P2: Structure
    ("P2.blocks", "4-block"),
    // P3: Derivation
    ("P3.from", "b-word/seed/code/L0/rust/library.rs"),
    ("P3.derives", "B-seed-code-L0-rust-library"),
    // P4: Version
    ("P4.at", "b-02.00"),
    // P5: Summary
    ("P5.title", "Path Resolution Library"),
    (
        "P5.summary",
        "Centralized path construction for all CPI-SI system components",
    ),
];

/// OmniCode metadata (M1-M10). Compile-time constants.
pub static METADATA: &[(&str, &str)] = &[
    // ── M1: Core Identity ─────────────────────────────────────
    ("M1.key", "B-L0-hybrid-paths"),
    ("M1.component_type", "Foundation"),
    ("M1.architect", "Seanje Lenox-Wise"),
    ("M1.implementation", "Nova Dawn"),
    ("M1.created", "2025-12-09"),
    // ── M2: Version History ───────────────────────────────────
    (
        "M2.b-02.00",
        "2026-02-14 — Rust conversion: OnceLock, PathBuf, zero deps",
    ),
    (
        "M2.b-01.01",
        "2026-02-14 — REORG: promoted to hybrid/ top-level domain",
    ),
    (
        "M2.b-01.00",
        "2026-02-14 — BASELINE: 4-block, identity, home caching, env fallbacks",
    ),
    ("M2.a-01.00", "2025-12-09 — Initial creation (Go)"),
    // ── M3: Interface ─────────────────────────────────────────
    (
        "M3.requires.stdlib",
        "std::env, std::fs, std::io, std::path, std::sync",
    ),
    ("M3.requires.external", "none"),
    ("M3.pattern", "paths::bereshit_root(), paths::claude_home()"),
    // ── M4: Public API ────────────────────────────────────────
    ("M4.config", "find_config_dir, find_config_dir_named"),
    (
        "M4.bereshit",
        "bereshit_root, bereshit_word, bereshit_seed, bereshit_omni_seed",
    ),
    (
        "M4.claude",
        "claude_home, claude_session, claude_system, claude_skills",
    ),
    (
        "M4.claude_global",
        "claude_global_root, claude_global_config",
    ),
    ("M4.cpisi", "cpisi_schema_config"),
    (
        "M4.statemachine",
        "statemachine_schema_dir, statemachine_runtime_dir, statemachine_runtime_state, statemachine_runtime_path, statemachine_runtime_history, statemachine_choice_history",
    ),
    (
        "M4.database",
        "database_dir, database_path, database_schema_dir, database_migrations_dir",
    ),
    (
        "M4.logging",
        "logs_dir, logs_session_dir, logs_trajectory_dir, logs_cognition_dir, logs_health_dir, logs_tools_dir, logs_context_dir",
    ),
    ("M4.health", "health_config_dir, health_runtime_dir"),
    (
        "M4.debug",
        "debug_dir, restore_dir, restore_checkpoints_dir, formats_dir",
    ),
    (
        "M4.ensure",
        "ensure_database_dir, ensure_logs_dir, ensure_debug_dir, ensure_restore_dir, ensure_formats_dir, ensure_all_data_dirs",
    ),
    ("M4.self", "pragma, pragma_get, metadata, metadata_get"),
    // ── M5: Operational ───────────────────────────────────────
    (
        "M5.blocking",
        "no — path construction + optional fs::metadata checks",
    ),
    (
        "M5.health",
        "Foundation | granted: resolved | deferred: n/a | denied: not found",
    ),
    // ── M6: Classification ────────────────────────────────────
    (
        "M6.tags",
        "paths, resolution, config, bereshit, claude, database, logging",
    ),
    ("M6.category", "Foundation"),
    ("M6.domain", "filesystem"),
    ("M6.layer", "L0-universal"),
    // ── M7: Intent ────────────────────────────────────────────
    ("M7.purpose", "Single source of truth for all system paths"),
    (
        "M7.philosophy",
        "Centralize path logic so changes propagate from one place",
    ),
    // ── M8: Grounding ─────────────────────────────────────────
    (
        "M8.scripture",
        "Proverbs 4:26 — Ponder the path of thy feet",
    ),
    (
        "M8.principle",
        "Know where you walk — every path resolved before traversed",
    ),
    (
        "M8.anchor",
        "Genesis 1:1 — In the beginning God created the heaven and the earth",
    ),
    // ── M9: Dependencies ──────────────────────────────────────
    (
        "M9.needs.stdlib",
        "std::env, std::fs (create_dir_all), std::path, std::sync (OnceLock)",
    ),
    ("M9.needs.external", "none"),
    ("M9.layer_deps", "L0 only — no L1+ dependencies"),
    // ── M10: Roadmap ──────────────────────────────────────────
    (
        "M10.current",
        "b-02.00 — Rust conversion with OnceLock caching",
    ),
    (
        "M10.planned",
        "XDG compliance, path validation, canonicalization",
    ),
    (
        "M10.limitations",
        "CWS workstation paths as defaults, Linux only",
    ),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

use std::env;
use std::fs;
use std::io;
use std::path::{Path, PathBuf};
use std::sync::OnceLock;

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Helpers → Config Resolution → Bereshit Paths →
//                Claude Paths → Claude Global → CPI-SI Schema →
//                State Machine → Database → Logging → Health →
//                Debug/Restore → Formats → Directory Initialization → Identity

// ── Helpers ──────────────────────────────────────────────────────

/// Cached home directory. Resolved once via `OnceLock`.
static HOME: OnceLock<PathBuf> = OnceLock::new();

/// Returns the cached home directory path.
/// Reads `$HOME` on first call, caches forever. Falls back to empty path.
fn home() -> &'static Path {
    HOME.get_or_init(|| env::var("HOME").map(PathBuf::from).unwrap_or_default())
}

// ── Config Directory Resolution ──────────────────────────────────

/// Locate a `config` directory relative to the binary or cwd.
/// Standard pattern for CPI-SI skill config loading.
pub fn find_config_dir() -> io::Result<PathBuf> {
    find_config_dir_named("config")
}

/// Locate a named config directory by searching:
/// 1. Relative to executable (`bin/` → `../../<name>`)
/// 2. Relative to cwd (`../<name>`, then `./<name>`)
///
/// Returns `io::ErrorKind::NotFound` if not found.
pub fn find_config_dir_named(name: &str) -> io::Result<PathBuf> {
    // Try relative to executable.
    if let Ok(exe) = env::current_exe() {
        if let Some(exe_dir) = exe.parent() {
            // From bin/ go up to scripts/, then up to skill root, then into config/.
            let config = exe_dir.join("..").join("..").join(name);
            if config.is_dir() {
                return Ok(config);
            }
        }
    }

    // Try relative to cwd (for development).
    if let Ok(cwd) = env::current_dir() {
        // Try ../config from scripts/.
        let config = cwd.join("..").join(name);
        if config.is_dir() {
            return Ok(config);
        }
        // Try ./config if in skill root.
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

// ── Bereshit Paths ───────────────────────────────────────────────

/// Bereshit project root directory.
/// Resolution: `BERESHIT_ROOT` env → known locations → `"bereshit"` fallback.
pub fn bereshit_root() -> PathBuf {
    if let Ok(root) = env::var("BERESHIT_ROOT") {
        return PathBuf::from(root);
    }

    let locations = [
        PathBuf::from("/media/seanje-lenox-wise/Project/Bereshit"),
        home().join("Project").join("Bereshit"),
    ];

    for loc in &locations {
        if loc.is_dir() {
            return loc.clone();
        }
    }

    PathBuf::from("bereshit")
}

/// `bereshit/word/` — definitions and implementations.
pub fn bereshit_word() -> PathBuf {
    bereshit_root().join("word")
}

/// `bereshit/word/seed/` — templates.
pub fn bereshit_seed() -> PathBuf {
    bereshit_word().join("seed")
}

/// `bereshit/word/omni/seed/` — OmniCode templates.
pub fn bereshit_omni_seed() -> PathBuf {
    bereshit_word().join("omni").join("seed")
}

// ── Claude Paths ─────────────────────────────────────────────────

/// `~/.claude` directory.
pub fn claude_home() -> PathBuf {
    home().join(".claude")
}

/// `~/.claude/session` directory.
pub fn claude_session() -> PathBuf {
    claude_home().join("session")
}

/// `~/.claude/system` directory.
pub fn claude_system() -> PathBuf {
    claude_home().join("system")
}

/// `~/.claude/cpi-si/skills` directory.
pub fn claude_skills() -> PathBuf {
    claude_home().join("cpi-si").join("skills")
}

// ── Claude Global Paths ──────────────────────────────────────────

/// Claude-global root directory.
/// Resolution: `CLAUDE_GLOBAL_ROOT` env → `bereshit/word/claude` (prod) →
/// `~/.claude/global` → `claude-global` (dev) → fallback.
pub fn claude_global_root() -> PathBuf {
    if let Ok(root) = env::var("CLAUDE_GLOBAL_ROOT") {
        return PathBuf::from(root);
    }

    // Priority 1: Production locations.
    let prod = [
        bereshit_root().join("word").join("claude"),
        claude_home().join("global"),
    ];
    for loc in &prod {
        if loc.is_dir() {
            return loc.clone();
        }
    }

    // Priority 2: Dev location.
    let dev = PathBuf::from("claude-global");
    if dev.is_dir() {
        return dev;
    }

    // Final fallback.
    claude_home().join("global")
}

/// Config directory within claude-global.
pub fn claude_global_config() -> PathBuf {
    claude_global_root().join("config")
}

// ── CPI-SI Schema Paths ─────────────────────────────────────────

/// Base CPI-SI schema config directory. Anchor for all configuration schemas.
pub fn cpisi_schema_config() -> PathBuf {
    claude_global_root()
        .join("pkg")
        .join("cpisi")
        .join("schema")
        .join("config")
}

// ── State Machine Paths ──────────────────────────────────────────

/// L2 vocabulary TOML directory (schema source of truth).
pub fn statemachine_schema_dir() -> PathBuf {
    cpisi_schema_config().join("l2")
}

/// Runtime state directory (session-specific state files).
pub fn statemachine_runtime_dir() -> PathBuf {
    claude_global_config().join("statemachine").join("runtime")
}

/// Path to `state.jsonc` runtime file.
pub fn statemachine_runtime_state() -> PathBuf {
    statemachine_runtime_dir().join("state.jsonc")
}

/// Path to `path.jsonc` runtime file.
pub fn statemachine_runtime_path() -> PathBuf {
    statemachine_runtime_dir().join("path.jsonc")
}

/// Path to `history.jsonc` runtime file.
pub fn statemachine_runtime_history() -> PathBuf {
    statemachine_runtime_dir().join("history.jsonc")
}

/// Path to `choice_history.jsonc` runtime file.
pub fn statemachine_choice_history() -> PathBuf {
    statemachine_runtime_dir().join("choice_history.jsonc")
}

// ── Database Paths ───────────────────────────────────────────────

/// Data directory within claude-global.
pub fn database_dir() -> PathBuf {
    claude_global_root().join("data")
}

/// Path to the main CPI-SI database (`cpisi.db`).
pub fn database_path() -> PathBuf {
    database_dir().join("cpisi.db")
}

/// Database schema directory.
pub fn database_schema_dir() -> PathBuf {
    database_dir().join("schema")
}

/// Database migrations directory.
pub fn database_migrations_dir() -> PathBuf {
    database_dir().join("migrations")
}

// ── Logging Paths ────────────────────────────────────────────────
//
// CPI-SI logs live under claude-global/data/logs/, SEPARATE from
// ~/.claude/logs/ (Claude's auto-tracking).

/// Root CPI-SI logs directory.
pub fn logs_dir() -> PathBuf {
    database_dir().join("logs")
}

/// Session logs directory.
pub fn logs_session_dir() -> PathBuf {
    logs_dir().join("session")
}

/// Trajectory logs directory.
pub fn logs_trajectory_dir() -> PathBuf {
    logs_dir().join("trajectory")
}

/// Cognition logs directory.
pub fn logs_cognition_dir() -> PathBuf {
    logs_dir().join("cognition")
}

/// Health logs directory.
pub fn logs_health_dir() -> PathBuf {
    logs_dir().join("health")
}

/// Tools logs directory.
pub fn logs_tools_dir() -> PathBuf {
    logs_dir().join("tools")
}

/// Context logs directory.
pub fn logs_context_dir() -> PathBuf {
    logs_dir().join("context")
}

// ── Health Config Paths ──────────────────────────────────────────

/// Health config directory.
pub fn health_config_dir() -> PathBuf {
    claude_global_config().join("health")
}

/// Health runtime directory.
pub fn health_runtime_dir() -> PathBuf {
    health_config_dir().join("runtime")
}

// ── Debug and Restore Paths ──────────────────────────────────────

/// Debug directory within claude-global/data/.
pub fn debug_dir() -> PathBuf {
    database_dir().join("debug")
}

/// Restore directory within claude-global/data/.
pub fn restore_dir() -> PathBuf {
    database_dir().join("restore")
}

/// Checkpoints subdirectory of restore.
pub fn restore_checkpoints_dir() -> PathBuf {
    restore_dir().join("checkpoints")
}

// ── Format Specs Paths ───────────────────────────────────────────

/// Formats specification directory.
pub fn formats_dir() -> PathBuf {
    database_dir().join("formats")
}

// ── Directory Initialization ─────────────────────────────────────

/// Create the data directory if it doesn't exist.
pub fn ensure_database_dir() -> io::Result<()> {
    fs::create_dir_all(database_dir())
}

/// Create all log subdirectories if they don't exist.
pub fn ensure_logs_dir() -> io::Result<()> {
    let dirs = [
        logs_dir(),
        logs_session_dir(),
        logs_trajectory_dir(),
        logs_cognition_dir(),
        logs_health_dir(),
        logs_tools_dir(),
        logs_context_dir(),
    ];
    for dir in &dirs {
        fs::create_dir_all(dir)?;
    }
    Ok(())
}

/// Create the debug directory if it doesn't exist.
pub fn ensure_debug_dir() -> io::Result<()> {
    fs::create_dir_all(debug_dir())
}

/// Create the restore directories if they don't exist.
pub fn ensure_restore_dir() -> io::Result<()> {
    fs::create_dir_all(restore_dir())?;
    fs::create_dir_all(restore_checkpoints_dir())
}

/// Create the formats directory if it doesn't exist.
pub fn ensure_formats_dir() -> io::Result<()> {
    fs::create_dir_all(formats_dir())
}

/// Create all data subdirectories in the correct order.
pub fn ensure_all_data_dirs() -> io::Result<()> {
    ensure_database_dir()?;
    ensure_logs_dir()?;
    ensure_debug_dir()?;
    ensure_restore_dir()?;
    ensure_formats_dir()
}

// ── Identity ─────────────────────────────────────────────────────

/// Returns this crate's OmniCode pragma identity (P1-P5).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Look up a single pragma value by key.
///
/// ```
/// use bereshit_l0_paths::pragma_get;
///
/// assert_eq!(pragma_get("P1.key"), Some("B-L0-hybrid-paths"));
/// assert_eq!(pragma_get("P1.format"), Some("rust"));
/// assert_eq!(pragma_get("missing"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (M1-M10).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Look up a single metadata value by key.
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

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    // ── Path Composition ─────────────────────────────────────
    // Tests verify path segments are correct. Actual filesystem
    // existence depends on the machine — we test structure, not state.

    #[test]
    fn bereshit_word_ends_with_word() {
        assert!(bereshit_word().ends_with("word"));
    }

    #[test]
    fn bereshit_seed_ends_with_seed() {
        let p = bereshit_seed();
        assert!(p.ends_with("seed"));
        assert!(p.to_string_lossy().contains("word"));
    }

    #[test]
    fn bereshit_omni_seed_path_shape() {
        let p = bereshit_omni_seed();
        let s = p.to_string_lossy();
        assert!(s.contains("word"));
        assert!(s.contains("omni"));
        assert!(s.ends_with("seed"));
    }

    #[test]
    fn claude_home_ends_with_claude() {
        assert!(claude_home().ends_with(".claude"));
    }

    #[test]
    fn claude_session_path_shape() {
        let p = claude_session();
        let s = p.to_string_lossy();
        assert!(s.contains(".claude"));
        assert!(s.ends_with("session"));
    }

    #[test]
    fn claude_skills_path_shape() {
        let p = claude_skills();
        let s = p.to_string_lossy();
        assert!(s.contains("cpi-si"));
        assert!(s.ends_with("skills"));
    }

    #[test]
    fn database_path_ends_with_db() {
        assert!(database_path().ends_with("cpisi.db"));
    }

    #[test]
    fn database_schema_dir_path_shape() {
        let p = database_schema_dir();
        let s = p.to_string_lossy();
        assert!(s.contains("data"));
        assert!(s.ends_with("schema"));
    }

    #[test]
    fn logs_dir_under_data() {
        let p = logs_dir();
        let s = p.to_string_lossy();
        assert!(s.contains("data"));
        assert!(s.ends_with("logs"));
    }

    #[test]
    fn logs_subdirs_under_logs() {
        assert!(logs_session_dir().ends_with("session"));
        assert!(logs_trajectory_dir().ends_with("trajectory"));
        assert!(logs_cognition_dir().ends_with("cognition"));
        assert!(logs_health_dir().ends_with("health"));
        assert!(logs_tools_dir().ends_with("tools"));
        assert!(logs_context_dir().ends_with("context"));
    }

    #[test]
    fn statemachine_runtime_files() {
        assert!(statemachine_runtime_state().ends_with("state.jsonc"));
        assert!(statemachine_runtime_path().ends_with("path.jsonc"));
        assert!(statemachine_runtime_history().ends_with("history.jsonc"));
        assert!(statemachine_choice_history().ends_with("choice_history.jsonc"));
    }

    #[test]
    fn health_dirs_under_config() {
        let p = health_config_dir();
        let s = p.to_string_lossy();
        assert!(s.contains("config"));
        assert!(s.ends_with("health"));
    }

    #[test]
    fn restore_checkpoints_nested() {
        let p = restore_checkpoints_dir();
        let s = p.to_string_lossy();
        assert!(s.contains("restore"));
        assert!(s.ends_with("checkpoints"));
    }

    // ── Config Resolution ────────────────────────────────────

    #[test]
    fn find_config_dir_named_not_found() {
        let result = find_config_dir_named("nonexistent_config_dir_xyz");
        assert!(result.is_err());
    }

    // ── Home Caching ─────────────────────────────────────────

    #[test]
    fn home_returns_non_empty_on_linux() {
        // $HOME should always be set on Linux.
        let h = home();
        assert!(!h.as_os_str().is_empty());
    }

    // ── Identity ─────────────────────────────────────────────

    #[test]
    fn pragma_contains_key() {
        assert_eq!(pragma_get("P1.key"), Some("B-L0-hybrid-paths"));
    }

    #[test]
    fn pragma_contains_format() {
        assert_eq!(pragma_get("P1.format"), Some("rust"));
    }

    #[test]
    fn pragma_get_missing() {
        assert_eq!(pragma_get("nonexistent"), None);
    }

    #[test]
    fn metadata_contains_grounding() {
        let scripture = metadata_get("M8.scripture").unwrap();
        assert!(scripture.contains("Proverbs 4:26"));
    }
}

// ============================================================================
// END CLOSING
// ============================================================================
