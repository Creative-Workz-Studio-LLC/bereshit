//! Standard format detection from file extensions.
//!
//! Ported from Go `formats.go`. L0 knows standard formats (.go, .toml, .json, etc.).
//! L1 adds OmniCode-specific extensions on top (.omni, .gen, .witness, etc.)
//! via [`register_format`] / [`register_config_format`].
//!
//! Genesis 1:4 — "And God divided the light from the darkness."
//! Format detection separates — each file gets its proper identity.

use std::collections::{BTreeMap, BTreeSet};
use std::path::Path;
use std::sync::{LazyLock, RwLock};

// ────────────────────────────────────────────────────────────────
// Format Registries
// ────────────────────────────────────────────────────────────────

/// Standard file extensions → format identifiers.
///
/// Populated at init with universal formats. L1 extends via [`register_format`].
/// Uses `RwLock` for thread-safe registration (reads are concurrent, writes exclusive).
static STANDARD_FORMATS: LazyLock<RwLock<BTreeMap<String, String>>> = LazyLock::new(|| {
    let map = BTreeMap::from([
        // Compiled languages
        (".go".into(), "go".into()),
        (".c".into(), "c".into()),
        (".h".into(), "c".into()),
        (".rs".into(), "rs".into()),
        // Config/data formats
        (".toml".into(), "toml".into()),
        (".json".into(), "json".into()),
        (".jsonc".into(), "jsonc".into()),
        (".yaml".into(), "yaml".into()),
        (".yml".into(), "yaml".into()),
        // Documentation
        (".md".into(), "md".into()),
        (".adoc".into(), "adoc".into()),
        (".txt".into(), "txt".into()),
        // Scripting
        (".py".into(), "py".into()),
        (".ts".into(), "ts".into()),
        (".js".into(), "js".into()),
        (".sh".into(), "sh".into()),
        (".bash".into(), "sh".into()),
        // Web
        (".html".into(), "html".into()),
        (".css".into(), "css".into()),
    ]);
    RwLock::new(map)
});

/// Formats that hold configuration data.
///
/// Used by [`is_config_format`] and [`is_config_path`] to identify loadable configs.
static CONFIG_FORMATS: LazyLock<RwLock<BTreeSet<String>>> = LazyLock::new(|| {
    let set = BTreeSet::from(["toml".into(), "json".into(), "jsonc".into(), "yaml".into()]);
    RwLock::new(set)
});

// ────────────────────────────────────────────────────────────────
// Registration — L1 extends L0 without modifying it
// ────────────────────────────────────────────────────────────────

/// Add a file extension → format mapping.
///
/// L1 calls this to register OmniCode-specific extensions (.omni, .gen, etc.)
/// without modifying L0's source. Call during initialization.
///
/// `ext` must include the dot: `".omni"`, not `"omni"`.
pub fn register_format(ext: &str, format: &str) {
    let mut map = STANDARD_FORMATS.write().unwrap_or_else(|e| e.into_inner());
    map.insert(ext.to_owned(), format.to_owned());
}

/// Mark a format as holding configuration data.
///
/// After registration, [`is_config_format`] and [`is_config_path`] recognize it.
pub fn register_config_format(format: &str) {
    let mut set = CONFIG_FORMATS.write().unwrap_or_else(|e| e.into_inner());
    set.insert(format.to_owned());
}

// ────────────────────────────────────────────────────────────────
// Format Detection
// ────────────────────────────────────────────────────────────────

/// Returns the format identifier for a file extension, or `None` if unrecognized.
///
/// Checks both L0 standard formats and any L1-registered extensions.
pub fn format_from_ext(ext: &str) -> Option<String> {
    let map = STANDARD_FORMATS.read().unwrap_or_else(|e| e.into_inner());
    map.get(ext).cloned()
}

/// Extracts the file extension from `path` and returns its format identifier.
pub fn format_from_path(path: &Path) -> Option<String> {
    let ext = path.extension()?.to_str()?;
    let dotted = format!(".{ext}");
    format_from_ext(&dotted)
}

/// Returns true if the format holds configuration data.
pub fn is_config_format(format: &str) -> bool {
    let set = CONFIG_FORMATS.read().unwrap_or_else(|e| e.into_inner());
    set.contains(format)
}

/// Returns true if the file path has a config format extension.
pub fn is_config_path(path: &Path) -> bool {
    format_from_path(path)
        .map(|f| is_config_format(&f))
        .unwrap_or(false)
}

/// Returns true if the file extension maps to a known format.
/// Includes both L0 standard formats and L1-registered extensions.
pub fn is_known_ext(ext: &str) -> bool {
    let map = STANDARD_FORMATS.read().unwrap_or_else(|e| e.into_inner());
    map.contains_key(ext)
}

// ────────────────────────────────────────────────────────────────
// Introspection
// ────────────────────────────────────────────────────────────────

/// Returns a sorted list of all registered file extensions.
///
/// Includes both L0 standard and L1-registered extensions.
/// Useful for help text, debugging, and validation error messages.
pub fn all_known_exts() -> Vec<String> {
    let map = STANDARD_FORMATS.read().unwrap_or_else(|e| e.into_inner());
    map.keys().cloned().collect() // BTreeMap already sorted
}

/// Returns a sorted list of all registered config format names.
pub fn all_config_formats() -> Vec<String> {
    let set = CONFIG_FORMATS.read().unwrap_or_else(|e| e.into_inner());
    set.iter().cloned().collect() // BTreeSet already sorted
}

// ────────────────────────────────────────────────────────────────
// Test-only: cleanup helper
// ────────────────────────────────────────────────────────────────

/// Remove a format registration (for test isolation).
#[cfg(test)]
pub(crate) fn unregister_format(ext: &str) {
    let mut map = STANDARD_FORMATS.write().unwrap_or_else(|e| e.into_inner());
    map.remove(ext);
}

/// Remove a config format registration (for test isolation).
#[cfg(test)]
pub(crate) fn unregister_config_format(format: &str) {
    let mut set = CONFIG_FORMATS.write().unwrap_or_else(|e| e.into_inner());
    set.remove(format);
}
