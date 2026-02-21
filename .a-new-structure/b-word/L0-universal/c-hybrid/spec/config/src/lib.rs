//! Manifest-driven TOML spec loader for the Bereshit configuration architecture.
//!
//! Loads TOML specification files organized by system (math, types, bible, etc.)
//! using an `index.toml` manifest as the single source of truth. When the manifest
//! is absent, falls back to known filesystem positions (the watch flow).
//!
//! # Usage
//!
//! ```no_run
//! use bereshit_l0_config as config;
//!
//! config::set_root("/path/to/.a-new-structure/b-word");
//! let result = config::load_all();
//! assert!(result.valid);
//! ```
//!
//! # Architecture
//!
//! Two loading flows modeled on Habakkuk 2:
//! - **Primary (appointed time):** `index.toml` manifest drives loading order
//! - **Watch (ramparts):** known filesystem positions when manifest is absent
//!
//! Both flows use the same `load_file` core that extracts `[_pragma]` and
//! `[_metadata]` identity tables from each TOML spec, making loaded files
//! self-aware.

//omni:code --rust -library
//omni:key B-L0-hybrid-config
//omni:version b-03.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:      B-L0-hybrid-config
// Purpose:  Manifest-driven TOML spec loader — capstone of L0 hybrid layer
// Biblical: Habakkuk 2:2-3 — "Write the vision... for it is yet for the
//           appointed time... though it tarry, wait for it."
// Authors:  Nova Dawn
// Version:  b-03.00
// Created:  2026-02-14
//
// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// S.1 Imports
// ──────────────────────────────────────────────────────────────────────────

use std::collections::BTreeMap;
use std::io;
use std::path::PathBuf;
use std::sync::LazyLock;

// ──────────────────────────────────────────────────────────────────────────
// S.2 Modules
// ──────────────────────────────────────────────────────────────────────────

mod cache;
mod discovery;
mod error;
pub mod exists;
pub mod format_loader;
pub mod formats;
mod graph;
pub mod identity;
mod jsonc;
mod loader;
pub mod must;
mod root;
#[cfg(test)]
mod test_utils;
mod types;

// ──────────────────────────────────────────────────────────────────────────
// S.3 ErrorTypes
// ──────────────────────────────────────────────────────────────────────────

pub use error::{ConfigError, DepKind, LoadOp};

// ──────────────────────────────────────────────────────────────────────────
// S.4 CoreTypes
// ──────────────────────────────────────────────────────────────────────────

pub use exists::{dir_exists, ensure_dir, file_exists, is_file};
pub use format_loader::{
    find_and_load, find_config, find_config_dir, load_config_file, load_jsonc_file,
    load_toml_bytes, load_toml_file, skill_config_paths, system_config_paths,
};
pub use formats::{
    all_config_formats, all_known_exts, format_from_ext, format_from_path, is_config_format,
    is_config_path, is_known_ext, register_config_format, register_format,
};
pub use identity::{metadata, metadata_get, pragma, pragma_get, register_identity};
pub use jsonc::{parse_jsonc, strip_jsonc_comments};
pub use must::{MustExt, MustValExt};
pub use types::{
    ConfigFile, DependencyNode, DiscoveryResult, IndexManifest, LoadResult, SpecEntry, SystemEntry,
};

use loader::{do_load_all, do_load_spec, do_load_system, load_index};
use root::RootGuard;

/// Global root — created once, mutated via set/clear.
static ROOT: LazyLock<RootGuard> = LazyLock::new(|| RootGuard::new("bereshit"));

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Root Management
// ──────────────────────────────────────────────────────────────────────────
//
// set_root(), clear_root(), check_root() — global root lifecycle.
// LazyLock ROOT persists for process lifetime; cache cleared on set/clear.

/// Resolve global root, returning error if not set.
fn check_root() -> Result<PathBuf, ConfigError> {
    ROOT.check()?;
    ROOT.get().ok_or_else(|| ConfigError::RootNotSet {
        name: "bereshit".to_owned(),
    })
}

/// Set the module root directory path.
///
/// Must be called before any Load* functions. Calling again replaces
/// the previous root.
///
/// # Example
///
/// ```no_run
/// bereshit_l0_config::set_root("/path/to/.a-new-structure/b-word");
/// ```
pub fn set_root(path: impl Into<PathBuf>) {
    cache::clear_all();
    ROOT.set(path.into());
}

/// Clear the root directory (primarily for testing).
///
/// Also clears all cached data — both index and spec caches.
pub fn clear_root() {
    cache::clear_all();
    ROOT.clear();
}

// ──────────────────────────────────────────────────────────────────────────
// 2. Constructors — Batch Loading
// ──────────────────────────────────────────────────────────────────────────
//
// load_all(), load_manifest(), discover_and_compare(),
// validate_dependency_graph(), get_dependency_tree()
// Manifest-driven loading with graph validation and discovery.

/// Load all config specs.
///
/// Tries manifest-driven loading first (appointed time). Falls back to
/// known filesystem positions (watch flow) if `index.toml` is missing.
pub fn load_all() -> LoadResult {
    match check_root() {
        Ok(root) => do_load_all(&root, &mut io::stdout()),
        Err(e) => {
            let mut result = LoadResult::new();
            result.valid = false;
            result.errors.push(e);
            result
        }
    }
}

/// Load all specs in a system by name.
pub fn load_system(system: &str) -> Result<Vec<ConfigFile>, ConfigError> {
    let root = check_root()?;
    do_load_system(&root, system)
}

/// Load a single spec from a system.
pub fn load_spec(system: &str, spec: &str) -> Result<ConfigFile, ConfigError> {
    let root = check_root()?;
    do_load_spec(&root, system, spec)
}

/// Load and return the index.toml manifest.
pub fn load_manifest() -> Result<IndexManifest, ConfigError> {
    let root = check_root()?;
    load_index(&root)
}

/// Compare manifest entries to files on disk.
pub fn discover_and_compare() -> Result<DiscoveryResult, ConfigError> {
    let root = check_root()?;
    let manifest = load_index(&root)?;
    Ok(discovery::compare_manifest_to_disk(&root, &manifest))
}

/// Validate the dependency graph for cycles and missing deps.
pub fn validate_dependency_graph() -> Result<Vec<ConfigError>, ConfigError> {
    let root = check_root()?;
    let manifest = load_index(&root)?;
    let g = graph::build_dependency_graph(&manifest);
    Ok(graph::validate_dependencies(&g))
}

/// Get the full dependency graph for visualization/analysis.
pub fn get_dependency_tree() -> Result<BTreeMap<String, DependencyNode>, ConfigError> {
    let root = check_root()?;
    let manifest = load_index(&root)?;
    Ok(graph::build_dependency_graph(&manifest))
}

// ──────────────────────────────────────────────────────────────────────────
// 3. IdentityAccess — System-Specific Loaders
// ──────────────────────────────────────────────────────────────────────────
//
// Typed wrappers — thin delegations to load_system.
// load_math(), load_types(), load_language(), load_bible(), load_health(),
// load_permission(), load_identity(), load_network(), load_filesystem()

/// Load math system (ternary foundation).
pub fn load_math() -> Result<Vec<ConfigFile>, ConfigError> {
    load_system("math")
}
/// Load types system (primitives, composition, validation).
pub fn load_types() -> Result<Vec<ConfigFile>, ConfigError> {
    load_system("types")
}
/// Load language system (keywords, syntax).
pub fn load_language() -> Result<Vec<ConfigFile>, ConfigError> {
    load_system("language")
}
/// Load bible system (scripture text, addressing, encoding).
pub fn load_bible() -> Result<Vec<ConfigFile>, ConfigError> {
    load_system("bible")
}
/// Load health system (scores, diagnostics, logging).
pub fn load_health() -> Result<Vec<ConfigFile>, ConfigError> {
    load_system("health")
}
/// Load permission system (access control).
pub fn load_permission() -> Result<Vec<ConfigFile>, ConfigError> {
    load_system("permission")
}
/// Load identity system (model, contract).
pub fn load_identity() -> Result<Vec<ConfigFile>, ConfigError> {
    load_system("identity")
}
/// Load network system (message, timestamp, contract).
pub fn load_network() -> Result<Vec<ConfigFile>, ConfigError> {
    load_system("network")
}
/// Load filesystem system (types, contract).
pub fn load_filesystem() -> Result<Vec<ConfigFile>, ConfigError> {
    load_system("filesystem")
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Cv — Closing Validation
//   Tests distributed to module files — 60+ tests covering error types,
//   root guard, helpers, JSONC, manifest serde, dependency graph, file
//   loading, discovery, exists, must, formats, format_loader, identity,
//   and cache lifecycle. Integration tests in tests/l0_integration.rs.
//
// Ce — Library crate. Registration via identity::register_identity().
//
// Cc — LazyLock ROOT persists for process lifetime. Cache cleared via
//   clear_root(). No file handles held open between calls.
//
// X1: Modification Policy
//   Never: Change public function signatures (L1+ consumers depend on them)
//   Never: Remove manifest-driven loading (appointed time flow)
//   Never: Remove watch-path fallback (ramparts flow)
//   Careful: Watch path positions (hardcoded fallback list in loader.rs)
//   Safe: Adding new system-specific load wrappers (B.3)
//   Safe: Adding new format support (formats.rs registry)
//
// X5: Closing Note
//   "Write the vision, and make it plain upon tables, that he may run
//   that readeth it. For the vision is yet for an appointed time, but
//   at the end it shall speak, and not lie: though it tarry, wait for it;
//   because it will surely come, it will not tarry." — Habakkuk 2:2-3
//
//   Two flows, one truth. Manifest-driven loading IS the appointed time —
//   ordered, verified, dependency-aware. Watch-path loading IS the ramparts —
//   standing on known ground when the manifest is absent. Both read the same
//   TOML specs. Both extract the same identity. The data is singular.
//
//   Genesis 1:1 anchor.
//
// ============================================================================
// END CLOSING
// ============================================================================
