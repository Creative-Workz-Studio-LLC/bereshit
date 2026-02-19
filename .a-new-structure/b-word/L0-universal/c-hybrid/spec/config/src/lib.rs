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
mod types;

use std::collections::BTreeMap;
use std::io;
use std::path::PathBuf;
use std::sync::LazyLock;

// Re-export public API types
pub use error::ConfigError;
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

/// Resolve global root, returning error if not set.
fn check_root() -> Result<PathBuf, ConfigError> {
    ROOT.check()?;
    ROOT.get().ok_or_else(|| ConfigError::RootNotSet {
        name: "bereshit".to_owned(),
    })
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ── B.1 Root Management ─────────────────────────────────────────────────
//
// set_root(), clear_root(), check_root() — global root lifecycle.
// LazyLock ROOT persists for process lifetime; cache cleared on set/clear.

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

// ── B.2 Batch Loading ───────────────────────────────────────────────────
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

// ── B.3 System-Specific Loaders ─────────────────────────────────────────
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
//   Tests in #[cfg(test)] below — 60+ covering error types, root guard,
//   helpers, JSONC, manifest serde, dependency graph, file loading,
//   discovery, integration, exists, must, formats, format_loader, identity,
//   and cache lifecycle.
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

// ════════════════════════════════════════════════════════════════════════════
// Tests
// ════════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs;
    use std::path::Path;

    // ── Test Helpers ──────────────────────────────────────────────

    /// Create a unique temp directory for test isolation.
    fn test_dir(name: &str) -> PathBuf {
        let ts = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_nanos();
        let dir = std::env::temp_dir().join(format!("bereshit-config-test-{name}-{ts}"));
        fs::create_dir_all(&dir).unwrap();
        dir
    }

    /// Write a file inside a temp directory, creating parent dirs.
    fn write_file(root: &Path, rel: &str, content: &str) {
        let path = root.join(rel);
        fs::create_dir_all(path.parent().unwrap()).unwrap();
        fs::write(path, content).unwrap();
    }

    /// Minimal index.toml for testing.
    const TEST_INDEX: &str = r#"
[[systems]]
name = "math"
path = "L0-universal/ladder/foundation/math"
order = 0

[[systems.specs]]
file = "ternary.toml"
key = "B-L0-math-ternary"
depends_on = []

[[systems]]
name = "types"
path = "L0-universal/ladder/foundation/types"
order = 1

[[systems.specs]]
file = "primitives.toml"
key = "B-L0-types-primitives"
depends_on = ["L0-universal/ladder/foundation/math/ternary.toml"]
"#;

    /// Minimal TOML spec with identity tables.
    const TEST_SPEC_WITH_IDENTITY: &str = r#"
[_pragma]
"P1.key" = "B-L0-math-ternary"
"P1.type" = "data"
"P4.at" = "b-01.00"

[_metadata]
"M1.key" = "B-L0-math-ternary"
"M1.created" = "2026-01-15"

[ternary]
base = 3
description = "balanced ternary"
"#;

    /// Minimal TOML spec without identity tables.
    const TEST_SPEC_PLAIN: &str = r#"
[primitives]
int9_min = -4
int9_max = 4
"#;

    // ── Error Types ──────────────────────────────────────────────

    #[test]
    fn test_config_error_load_display() {
        let err = ConfigError::Load {
            file: "ternary.toml".into(),
            op: "read".into(),
            source: "not found".into(),
        };
        assert_eq!(err.to_string(), "ternary.toml (read): not found");
    }

    #[test]
    fn test_config_error_validation_display() {
        let err = ConfigError::Validation {
            path: "types/primitives.toml".into(),
            section: "int9".into(),
            field: "min_value".into(),
            message: "expected integer".into(),
        };
        assert_eq!(
            err.to_string(),
            "types/primitives.toml [int9].min_value: expected integer"
        );
    }

    #[test]
    fn test_config_error_validation_no_field() {
        let err = ConfigError::Validation {
            path: "math/ternary.toml".into(),
            section: "ternary".into(),
            field: String::new(),
            message: "section empty".into(),
        };
        assert_eq!(
            err.to_string(),
            "math/ternary.toml [ternary]: section empty"
        );
    }

    #[test]
    fn test_config_error_dependency_display() {
        let err = ConfigError::Dependency {
            spec: "types/comp.toml".into(),
            dep_spec: "types/prims.toml".into(),
            kind: "missing".into(),
            message: "dependency not in manifest".into(),
        };
        assert_eq!(
            err.to_string(),
            "types/comp.toml -> types/prims.toml (missing): dependency not in manifest"
        );
    }

    #[test]
    fn test_config_error_dependency_no_dep_spec() {
        let err = ConfigError::Dependency {
            spec: "types/comp.toml".into(),
            dep_spec: String::new(),
            kind: "cycle".into(),
            message: "circular".into(),
        };
        assert_eq!(err.to_string(), "types/comp.toml (cycle): circular");
    }

    #[test]
    fn test_config_error_root_not_set() {
        let err = ConfigError::RootNotSet {
            name: "bereshit".into(),
        };
        assert!(err.to_string().contains("root not set"));
    }

    // ── Root Guard ───────────────────────────────────────────────

    #[test]
    fn test_root_guard_lifecycle() {
        let guard = RootGuard::new("test");
        assert!(guard.check().is_err());
        assert!(guard.get().is_none());

        guard.set(PathBuf::from("/tmp/test"));
        assert!(guard.check().is_ok());
        assert_eq!(guard.get().unwrap(), PathBuf::from("/tmp/test"));

        guard.clear();
        assert!(guard.check().is_err());
        assert!(guard.get().is_none());
    }

    // ── Helpers ──────────────────────────────────────────────────

    #[test]
    fn test_extract_string_map_present() {
        let toml_str = r#"
[_pragma]
"P1.key" = "test-key"
"P1.type" = "data"
"#;
        let data: toml::Table = toml::from_str(toml_str).unwrap();
        let map = loader::extract_string_map(&data, "_pragma").unwrap();
        assert_eq!(map["P1.key"], "test-key");
        assert_eq!(map["P1.type"], "data");
    }

    #[test]
    fn test_extract_string_map_absent() {
        let data: toml::Table = toml::from_str("[section]\nkey = 1").unwrap();
        assert!(loader::extract_string_map(&data, "_pragma").is_none());
    }

    #[test]
    fn test_extract_string_map_skips_non_strings() {
        let toml_str = r#"
[_pragma]
"P1.key" = "test"
numeric = 42
"#;
        let data: toml::Table = toml::from_str(toml_str).unwrap();
        let map = loader::extract_string_map(&data, "_pragma").unwrap();
        assert_eq!(map.len(), 1); // only string values
        assert_eq!(map["P1.key"], "test");
    }

    // ── JSONC ────────────────────────────────────────────────────

    #[test]
    fn test_strip_single_line_comments() {
        let input = "{ \"key\": \"value\" // comment\n}";
        let result = strip_jsonc_comments(input);
        assert!(result.contains("\"key\""));
        assert!(!result.contains("comment"));
    }

    #[test]
    fn test_strip_multi_line_comments() {
        let input = "{ \"key\": /* block */ \"value\" }";
        let result = strip_jsonc_comments(input);
        assert!(result.contains("\"key\""));
        assert!(!result.contains("block"));
    }

    #[test]
    fn test_strip_trailing_commas() {
        let input = "{ \"a\": 1, \"b\": 2, }";
        let result = strip_jsonc_comments(input);
        assert!(!result.ends_with(", }"));
        // Should parse as valid JSON
        let val: serde_json::Value = serde_json::from_str(&result).unwrap();
        assert_eq!(val["a"], 1);
        assert_eq!(val["b"], 2);
    }

    #[test]
    fn test_strip_preserves_strings() {
        let input = "{ \"url\": \"https://example.com//path\" }";
        let result = strip_jsonc_comments(input);
        assert!(result.contains("//path"));
    }

    #[test]
    fn test_parse_jsonc() {
        let input = r#"{
            // Comment
            "name": "test",
            "version": 1, /* inline */
        }"#;
        let val: serde_json::Value = parse_jsonc(input).unwrap();
        assert_eq!(val["name"], "test");
        assert_eq!(val["version"], 1);
    }

    // ── Index Manifest Serde ─────────────────────────────────────

    #[test]
    fn test_index_manifest_deserialize() {
        let manifest: IndexManifest = toml::from_str(TEST_INDEX).unwrap();
        assert_eq!(manifest.systems.len(), 2);
        assert_eq!(manifest.systems[0].name, "math");
        assert_eq!(manifest.systems[0].order, 0);
        assert_eq!(manifest.systems[0].specs.len(), 1);
        assert_eq!(manifest.systems[0].specs[0].file, "ternary.toml");
        assert!(manifest.systems[0].specs[0].depends_on.is_empty());
        assert_eq!(manifest.systems[1].specs[0].depends_on.len(), 1);
    }

    // ── Dependency Graph ─────────────────────────────────────────

    #[test]
    fn test_build_dependency_graph() {
        let manifest: IndexManifest = toml::from_str(TEST_INDEX).unwrap();
        let g = graph::build_dependency_graph(&manifest);
        assert_eq!(g.len(), 2);
        assert!(g.contains_key("L0-universal/ladder/foundation/math/ternary.toml"));
        assert!(g.contains_key("L0-universal/ladder/foundation/types/primitives.toml"));
    }

    #[test]
    fn test_validate_dependencies_valid() {
        let manifest: IndexManifest = toml::from_str(TEST_INDEX).unwrap();
        let g = graph::build_dependency_graph(&manifest);
        let errors = graph::validate_dependencies(&g);
        assert!(errors.is_empty(), "expected no errors: {errors:?}");
    }

    #[test]
    fn test_validate_dependencies_missing() {
        let toml_str = r#"
[[systems]]
name = "types"
path = "types"
order = 0
[[systems.specs]]
file = "comp.toml"
key = "comp"
depends_on = ["missing/nonexistent.toml"]
"#;
        let manifest: IndexManifest = toml::from_str(toml_str).unwrap();
        let g = graph::build_dependency_graph(&manifest);
        let errors = graph::validate_dependencies(&g);
        assert_eq!(errors.len(), 1);
        assert!(matches!(&errors[0], ConfigError::Dependency { kind, .. } if kind == "missing"));
    }

    #[test]
    fn test_validate_dependencies_cycle() {
        let toml_str = r#"
[[systems]]
name = "sys"
path = "sys"
order = 0

[[systems.specs]]
file = "a.toml"
key = "a"
depends_on = ["sys/b.toml"]

[[systems.specs]]
file = "b.toml"
key = "b"
depends_on = ["sys/a.toml"]
"#;
        let manifest: IndexManifest = toml::from_str(toml_str).unwrap();
        let g = graph::build_dependency_graph(&manifest);
        let errors = graph::validate_dependencies(&g);
        assert!(
            errors
                .iter()
                .any(|e| matches!(e, ConfigError::Dependency { kind, .. } if kind == "cycle")),
            "expected cycle error: {errors:?}"
        );
    }

    // ── Tripwire (Writer) ────────────────────────────────────────

    #[test]
    fn test_tripwire_banner_output() {
        let mut buf = Vec::new();
        loader::tripwire_banner(
            &mut buf,
            "Test Warning",
            &["Line one".to_owned(), "Line two".to_owned()],
        );
        let output = String::from_utf8(buf).unwrap();
        assert!(output.contains("Test Warning"));
        assert!(output.contains("Line one"));
        assert!(output.contains("Line two"));
        assert!(output.contains("\u{2550}")); // ═ border
    }

    // ── File Loading (Filesystem) ────────────────────────────────

    #[test]
    fn test_load_file_plain() {
        let dir = test_dir("load-file-plain");
        write_file(&dir, "test.toml", TEST_SPEC_PLAIN);

        let cfg = loader::load_file(&dir.join("test.toml")).unwrap();
        assert_eq!(cfg.name, "test.toml");
        assert!(cfg.keys.contains(&"primitives".to_owned()));
        assert!(cfg.pragma.is_none());
        assert!(cfg.metadata.is_none());

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_file_with_identity() {
        let dir = test_dir("load-file-identity");
        write_file(&dir, "ternary.toml", TEST_SPEC_WITH_IDENTITY);

        let cfg = loader::load_file(&dir.join("ternary.toml")).unwrap();
        assert_eq!(cfg.name, "ternary.toml");

        let pragma = cfg.pragma.as_ref().unwrap();
        assert_eq!(pragma["P1.key"], "B-L0-math-ternary");
        assert_eq!(pragma["P1.type"], "data");

        let metadata = cfg.metadata.as_ref().unwrap();
        assert_eq!(metadata["M1.key"], "B-L0-math-ternary");

        // Keys should include _pragma, _metadata, ternary
        assert!(cfg.keys.contains(&"_pragma".to_owned()));
        assert!(cfg.keys.contains(&"ternary".to_owned()));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_file_not_found() {
        let err = loader::load_file(Path::new("/nonexistent/path/file.toml")).unwrap_err();
        assert!(matches!(err, ConfigError::Load { op, .. } if op == "read"));
    }

    #[test]
    fn test_load_directory() {
        let dir = test_dir("load-dir");
        write_file(&dir, "specs/a.toml", "[alpha]\nval = 1");
        write_file(&dir, "specs/b.toml", "[beta]\nval = 2");
        write_file(&dir, "specs/readme.md", "not toml"); // should be ignored

        let configs = loader::load_directory(&dir.join("specs")).unwrap();
        assert_eq!(configs.len(), 2);
        assert_eq!(configs[0].name, "a.toml");
        assert_eq!(configs[1].name, "b.toml");

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_directory_not_found() {
        let err = loader::load_directory(Path::new("/nonexistent/dir")).unwrap_err();
        assert!(matches!(err, ConfigError::Load { op, .. } if op == "read"));
    }

    // ── Discovery (Filesystem) ───────────────────────────────────

    #[test]
    fn test_discovery_all_present() {
        let dir = test_dir("discovery-present");
        write_file(&dir, "math/ternary.toml", "[t]\nv = 1");

        let manifest: IndexManifest = toml::from_str(
            r#"
[[systems]]
name = "math"
path = "math"
order = 0
[[systems.specs]]
file = "ternary.toml"
key = "t"
"#,
        )
        .unwrap();

        let result = discovery::compare_manifest_to_disk(&dir, &manifest);
        assert!(result.valid);
        assert!(result.missing.is_empty());

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_discovery_missing_file() {
        let dir = test_dir("discovery-missing");
        fs::create_dir_all(dir.join("math")).unwrap();
        // Don't create the file

        let manifest: IndexManifest = toml::from_str(
            r#"
[[systems]]
name = "math"
path = "math"
order = 0
[[systems.specs]]
file = "ternary.toml"
key = "t"
"#,
        )
        .unwrap();

        let result = discovery::compare_manifest_to_disk(&dir, &manifest);
        assert!(!result.valid);
        assert_eq!(result.missing.len(), 1);
        assert!(result.missing[0].contains("ternary.toml"));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_discovery_unexpected_file() {
        let dir = test_dir("discovery-unexpected");
        write_file(&dir, "math/ternary.toml", "[t]\nv = 1");
        write_file(&dir, "math/extra.toml", "[e]\nv = 2");

        let manifest: IndexManifest = toml::from_str(
            r#"
[[systems]]
name = "math"
path = "math"
order = 0
[[systems.specs]]
file = "ternary.toml"
key = "t"
"#,
        )
        .unwrap();

        let result = discovery::compare_manifest_to_disk(&dir, &manifest);
        assert!(result.valid); // unexpected is WARNING, not error
        assert_eq!(result.unexpected.len(), 1);
        assert!(result.unexpected[0].contains("extra.toml"));

        let _ = fs::remove_dir_all(&dir);
    }

    // ── Integration: Full Load ───────────────────────────────────

    #[test]
    fn test_load_all_from_index_integration() {
        let dir = test_dir("load-all-index");

        // Create index
        write_file(
            &dir,
            "L0-universal/ladder/foundation/index.toml",
            TEST_INDEX,
        );
        // Create specs
        write_file(
            &dir,
            "L0-universal/ladder/foundation/math/ternary.toml",
            TEST_SPEC_WITH_IDENTITY,
        );
        write_file(
            &dir,
            "L0-universal/ladder/foundation/types/primitives.toml",
            TEST_SPEC_PLAIN,
        );

        let result = loader::load_all_from_index(&dir, &mut Vec::new());
        assert!(result.valid, "errors: {:?}", result.errors);
        assert_eq!(result.configs.len(), 2);
        assert!(result.configs.contains_key("math"));
        assert!(result.configs.contains_key("types"));
        assert_eq!(result.summary["math"], vec!["ternary.toml"]);
        assert_eq!(result.summary["types"], vec!["primitives.toml"]);

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_all_watch_flow() {
        let dir = test_dir("load-all-watch");
        // No index.toml — force watch flow
        // Create only math (one of the watch paths)
        write_file(
            &dir,
            "L0-universal/ladder/foundation/math/ternary.toml",
            TEST_SPEC_WITH_IDENTITY,
        );

        let result = do_load_all(&dir, &mut Vec::new());
        // Not all systems will load (most dirs don't exist), but math should
        assert!(result.configs.contains_key("math"));
        assert_eq!(result.summary["math"], vec!["ternary.toml"]);

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_system_from_manifest() {
        let dir = test_dir("load-system");
        write_file(
            &dir,
            "L0-universal/ladder/foundation/index.toml",
            TEST_INDEX,
        );
        write_file(
            &dir,
            "L0-universal/ladder/foundation/math/ternary.toml",
            TEST_SPEC_WITH_IDENTITY,
        );

        let configs = do_load_system(&dir, "math").unwrap();
        assert_eq!(configs.len(), 1);
        assert_eq!(configs[0].name, "ternary.toml");

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_spec_from_manifest() {
        let dir = test_dir("load-spec");
        write_file(
            &dir,
            "L0-universal/ladder/foundation/index.toml",
            TEST_INDEX,
        );
        write_file(
            &dir,
            "L0-universal/ladder/foundation/math/ternary.toml",
            TEST_SPEC_WITH_IDENTITY,
        );

        let cfg = do_load_spec(&dir, "math", "ternary.toml").unwrap();
        assert_eq!(cfg.name, "ternary.toml");
        assert!(cfg.pragma.is_some());

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_system_not_found() {
        let dir = test_dir("load-system-404");
        write_file(
            &dir,
            "L0-universal/ladder/foundation/index.toml",
            TEST_INDEX,
        );

        let err = do_load_system(&dir, "nonexistent").unwrap_err();
        assert!(matches!(err, ConfigError::Load { op, .. } if op == "lookup"));

        let _ = fs::remove_dir_all(&dir);
    }

    // ── Exists (exists.rs) ──────────────────────────────────────

    #[test]
    fn test_file_exists_real_file() {
        let dir = test_dir("exists-file");
        write_file(&dir, "exists.txt", "hello");

        assert!(exists::file_exists(&dir.join("exists.txt")));
        assert!(!exists::file_exists(&dir.join("nope.txt")));
        // Directory also returns true (existence check, not type check)
        assert!(exists::file_exists(&dir));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_is_file() {
        let dir = test_dir("is-file");
        write_file(&dir, "regular.txt", "data");

        assert!(exists::is_file(&dir.join("regular.txt")));
        assert!(!exists::is_file(&dir)); // directory → false
        assert!(!exists::is_file(&dir.join("nope"))); // missing → false

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_ensure_dir() {
        let dir = test_dir("ensure-dir");

        // Create nested directory
        let nested = dir.join("a").join("b").join("c");
        exists::ensure_dir(&nested).unwrap();
        assert!(exists::dir_exists(&nested));

        // No-op on existing directory
        exists::ensure_dir(&nested).unwrap();

        // Error when path is a file
        write_file(&dir, "file.txt", "x");
        let err = exists::ensure_dir(&dir.join("file.txt"));
        assert!(err.is_err());

        let _ = fs::remove_dir_all(&dir);
    }

    // ── Must (must.rs) ──────────────────────────────────────────

    #[test]
    fn test_must_ext_success() {
        let result: Result<i32, String> = Ok(42);
        assert_eq!(result.must("should not panic"), 42);
    }

    #[test]
    #[should_panic(expected = "config: must: boom")]
    fn test_must_ext_panic() {
        let result: Result<i32, String> = Err("kaboom".into());
        let _ = result.must("boom");
    }

    #[test]
    fn test_must_val_ext_success() {
        assert_eq!(Some(42).must_val("should not panic"), 42);
    }

    #[test]
    #[should_panic(expected = "config: must: not found")]
    fn test_must_val_ext_panic() {
        let _: i32 = None.must_val("not found");
    }

    // ── Formats (formats.rs) ────────────────────────────────────

    #[test]
    fn test_register_format() {
        formats::register_format(".omni", "omni");
        assert_eq!(formats::format_from_ext(".omni"), Some("omni".to_owned()));
        assert!(formats::is_known_ext(".omni"));

        // Clean up
        formats::unregister_format(".omni");
    }

    #[test]
    fn test_register_config_format() {
        formats::register_config_format("omni-config");
        assert!(formats::is_config_format("omni-config"));

        // Clean up
        formats::unregister_config_format("omni-config");
    }

    #[test]
    fn test_all_known_exts_sorted_and_populated() {
        let exts = formats::all_known_exts();
        assert!(!exts.is_empty());

        // Should be sorted (BTreeMap guarantees this)
        for i in 1..exts.len() {
            assert!(
                exts[i] >= exts[i - 1],
                "not sorted: {:?} before {:?}",
                exts[i - 1],
                exts[i]
            );
        }

        // Should include known extensions
        for required in [".go", ".toml", ".json", ".rs", ".c"] {
            assert!(exts.contains(&required.to_owned()), "missing {required}");
        }
    }

    #[test]
    fn test_all_config_formats() {
        let fmts = formats::all_config_formats();
        assert!(fmts.len() >= 4);

        // Should be sorted
        for i in 1..fmts.len() {
            assert!(fmts[i] >= fmts[i - 1]);
        }
    }

    #[test]
    fn test_format_from_path() {
        assert_eq!(
            formats::format_from_path(Path::new("config.toml")),
            Some("toml".to_owned())
        );
        assert_eq!(
            formats::format_from_path(Path::new("/path/to/main.go")),
            Some("go".to_owned())
        );
        assert!(formats::format_from_path(Path::new("noext")).is_none());
    }

    #[test]
    fn test_is_config_path() {
        assert!(formats::is_config_path(Path::new("settings.toml")));
        assert!(formats::is_config_path(Path::new("config.json")));
        assert!(!formats::is_config_path(Path::new("main.go")));
    }

    // ── Format Loader (format_loader.rs) ────────────────────────

    #[test]
    fn test_load_config_file_toml() {
        let dir = test_dir("fmt-loader-toml");
        write_file(&dir, "config.toml", "[section]\nkey = \"value\"\n");

        let table = format_loader::load_config_file(&dir.join("config.toml")).unwrap();
        let section = table["section"].as_table().expect("expected table");
        assert_eq!(section["key"].as_str(), Some("value"));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_config_file_jsonc() {
        let dir = test_dir("fmt-loader-jsonc");
        write_file(
            &dir,
            "config.jsonc",
            "{\n// comment\n\"name\": \"test\",\n}",
        );

        let table = format_loader::load_config_file(&dir.join("config.jsonc")).unwrap();
        assert_eq!(table["name"].as_str(), Some("test"));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_config_file_json() {
        let dir = test_dir("fmt-loader-json");
        write_file(&dir, "config.json", r#"{"count": 42}"#);

        let table = format_loader::load_config_file(&dir.join("config.json")).unwrap();
        assert_eq!(table["count"].as_integer(), Some(42));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_config_file_unsupported() {
        let dir = test_dir("fmt-loader-unsup");
        write_file(&dir, "config.xml", "<config/>");

        let err = format_loader::load_config_file(&dir.join("config.xml")).unwrap_err();
        assert!(matches!(err, ConfigError::Load { op, .. } if op == "detect"));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_config_file_missing() {
        let err =
            format_loader::load_config_file(Path::new("/nonexistent/config.toml")).unwrap_err();
        assert!(matches!(err, ConfigError::Load { op, .. } if op == "stat"));
    }

    #[test]
    fn test_load_toml_file_typed() {
        let dir = test_dir("toml-typed");
        write_file(
            &dir,
            "app.toml",
            "[app]\nname = \"bereshit\"\nversion = 1\n",
        );

        #[derive(serde::Deserialize)]
        struct Outer {
            app: App,
        }
        #[derive(serde::Deserialize)]
        struct App {
            name: String,
            version: i64,
        }

        let cfg: Outer = format_loader::load_toml_file(&dir.join("app.toml")).unwrap();
        assert_eq!(cfg.app.name, "bereshit");
        assert_eq!(cfg.app.version, 1);

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_jsonc_file_typed() {
        let dir = test_dir("jsonc-typed");
        write_file(
            &dir,
            "app.jsonc",
            "{\n  // App config\n  \"name\": \"bereshit\",\n  \"version\": 1,\n}",
        );

        #[derive(serde::Deserialize)]
        struct Config {
            name: String,
            version: i64,
        }

        let cfg: Config = format_loader::load_jsonc_file(&dir.join("app.jsonc")).unwrap();
        assert_eq!(cfg.name, "bereshit");
        assert_eq!(cfg.version, 1);

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_jsonc_file_missing() {
        let err = format_loader::load_jsonc_file::<serde_json::Value>(Path::new(
            "/nonexistent/app.jsonc",
        ))
        .unwrap_err();
        assert!(matches!(err, ConfigError::Load { op, .. } if op == "stat"));
    }

    #[test]
    fn test_load_jsonc_file_invalid() {
        let dir = test_dir("jsonc-invalid");
        write_file(&dir, "bad.jsonc", "{ not valid json }");

        let err = format_loader::load_jsonc_file::<serde_json::Value>(&dir.join("bad.jsonc"))
            .unwrap_err();
        assert!(matches!(err, ConfigError::Load { op, .. } if op == "parse"));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_find_config() {
        let dir = test_dir("find-cfg");
        write_file(&dir, "a/settings.toml", "[s]\nv=1");
        write_file(&dir, "b/settings.toml", "[s]\nv=2");

        let search = vec![dir.join("a"), dir.join("b")];
        let found = format_loader::find_config("settings.toml", &search).unwrap();
        // Should find the first one (a/)
        assert!(found.starts_with(dir.join("a")));

        // Not found
        let err = format_loader::find_config("nope.toml", &search).unwrap_err();
        assert!(matches!(err, ConfigError::Load { op, .. } if op == "find"));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_find_config_dir() {
        let dir = test_dir("find-cfg-dir");
        write_file(&dir, "loc/app.toml", "[a]\nv=1");

        let search = vec![dir.join("loc")];
        let found_dir = format_loader::find_config_dir("app.toml", &search).unwrap();
        assert_eq!(found_dir, dir.join("loc"));

        let _ = fs::remove_dir_all(&dir);
    }

    // ── Identity (identity.rs) ──────────────────────────────────

    #[test]
    fn test_pragma_and_metadata() {
        assert_eq!(identity::pragma_get("I1.key"), Some("B-L0-hybrid-config"));
        assert_eq!(identity::pragma_get("I1.format"), Some("rust"));

        assert_eq!(identity::metadata_get("C1.version"), Some("b-03.00"));
        assert_eq!(identity::metadata_get("C7.domain"), Some("config-loading"));
    }

    #[test]
    fn test_identity_slices_not_empty() {
        assert!(!identity::pragma().is_empty());
        assert!(!identity::metadata().is_empty());
    }

    // ── Cache (cache.rs) ────────────────────────────────────────
    //
    // Single test for cache — global state requires sequential access.
    // Parallel tests would race on the shared CACHE static.

    #[test]
    fn test_cache_full_lifecycle() {
        // ── Phase 1: Clear and verify empty ──
        cache::clear_all();
        assert!(!cache::has_index(), "index should be empty after clear");
        assert_eq!(cache::spec_count(), 0, "specs should be empty after clear");
        assert!(cache::get_index().is_none());
        assert!(cache::get_spec("nonexistent").is_none());

        // ── Phase 2: Index round-trip ──
        let manifest = IndexManifest {
            systems: vec![SystemEntry {
                name: "math".into(),
                path: "math".into(),
                order: 0,
                specs: vec![],
            }],
        };
        cache::put_index(manifest);
        assert!(cache::has_index());

        let cached = cache::get_index().unwrap();
        assert_eq!(cached.systems.len(), 1);
        assert_eq!(cached.systems[0].name, "math");

        // ── Phase 3: Spec round-trip ──
        let config = ConfigFile {
            name: "file.toml".into(),
            path: PathBuf::from("test/file.toml"),
            data: toml::Table::new(),
            keys: vec!["section".into()],
            pragma: None,
            metadata: None,
        };
        cache::put_spec("test/file.toml".into(), config);
        assert_eq!(cache::spec_count(), 1);

        let cached_spec = cache::get_spec("test/file.toml").unwrap();
        assert_eq!(cached_spec.name, "file.toml");
        assert_eq!(cached_spec.keys, vec!["section"]);

        // ── Phase 4: Clones, not references ──
        let mut copy1 = cache::get_spec("test/file.toml").unwrap();
        let copy2 = cache::get_spec("test/file.toml").unwrap();
        copy1.keys.push("mutated".into());
        // copy2 should be unaffected by mutation of copy1
        assert_eq!(copy2.keys, vec!["section"]);

        // ── Phase 5: Clear wipes both layers ──
        cache::clear_all();
        assert!(!cache::has_index());
        assert_eq!(cache::spec_count(), 0);
    }
}
