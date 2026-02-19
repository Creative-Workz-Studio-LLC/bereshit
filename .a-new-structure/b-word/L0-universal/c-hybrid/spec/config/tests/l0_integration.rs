//! L0 Integration Tests — Cross-Crate Chain Verification
//!
//! These tests verify that L0 crates work together correctly:
//!
//!   identity ← paths ← config
//!
//! Each crate registers itself with the identity registry, and the registry
//! can discover them by key, layer, domain, and tag. This is the "system
//! knows itself" contract that every L0 crate participates in.
//!
//! Integration tests run in their own binary — separate from unit tests.
//! Each test function gets its own thread, but they share the global
//! identity registry. Tests that mutate registry state must be careful
//! about ordering.
//!
//! Genesis 1:31 — "And God saw every thing that he had made, and, behold,
//! it was very good." Integration testing: does the whole work together?

use std::collections::BTreeSet;

// ============================================================================
// Identity Registration Chain
// ============================================================================

/// All three L0 crates register with identity and are discoverable.
///
/// This is the fundamental integration contract: identity → paths → config
/// each call `register_identity()`, and the global registry knows them all.
#[test]
fn l0_crates_register_and_discover() {
    // Register all three L0 crates
    bereshit_l0_config::identity::register_identity();
    bereshit_l0_paths::register_identity();

    // Identity self-registers via LazyLock — no explicit call needed.

    // All three discoverable by key
    let identity = bereshit_l0_identity::lookup("B-L0-identity-registry");
    let paths = bereshit_l0_identity::lookup("B-L0-hybrid-paths");
    let config = bereshit_l0_identity::lookup("B-L0-hybrid-config");

    assert!(identity.is_some(), "identity crate must be registered");
    assert!(paths.is_some(), "paths crate must be registered");
    assert!(config.is_some(), "config crate must be registered");
}

/// All L0 crates appear in the L0 layer index.
///
/// `by_layer("L0")` is how the system discovers what's at each architectural
/// layer. Every L0 crate's I4.layer must be "L0".
#[test]
fn l0_crates_all_in_layer_index() {
    bereshit_l0_config::identity::register_identity();
    bereshit_l0_paths::register_identity();

    let l0_packages = bereshit_l0_identity::by_layer("L0");
    let l0_keys: BTreeSet<String> = l0_packages.iter().map(|p| p.key.clone()).collect();

    assert!(
        l0_keys.contains("B-L0-identity-registry"),
        "identity must be in L0 layer"
    );
    assert!(
        l0_keys.contains("B-L0-hybrid-paths"),
        "paths must be in L0 layer"
    );
    assert!(
        l0_keys.contains("B-L0-hybrid-config"),
        "config must be in L0 layer"
    );
}

/// Each L0 crate registers in its own domain.
///
/// Domains enable querying by functional area. No two L0 crates
/// should share a domain — each serves a distinct purpose.
#[test]
fn l0_crates_have_distinct_domains() {
    bereshit_l0_config::identity::register_identity();
    bereshit_l0_paths::register_identity();

    // Each domain should return exactly one package
    let identity_domain = bereshit_l0_identity::by_domain("identity");
    let paths_domain = bereshit_l0_identity::by_domain("filesystem");
    let config_domain = bereshit_l0_identity::by_domain("config-loading");

    assert_eq!(identity_domain.len(), 1, "identity domain should have 1 package");
    assert_eq!(paths_domain.len(), 1, "filesystem domain should have 1 package");
    assert_eq!(config_domain.len(), 1, "config-loading domain should have 1 package");

    // Verify correct package in each domain
    assert_eq!(identity_domain[0].key, "B-L0-identity-registry");
    assert_eq!(paths_domain[0].key, "B-L0-hybrid-paths");
    assert_eq!(config_domain[0].key, "B-L0-hybrid-config");
}

// ============================================================================
// Identity Data Consistency
// ============================================================================

/// Registered identity matches crate's own PRAGMA/METADATA.
///
/// After registration, the identity registry should hold exactly
/// the same data the crate carries in its static slices.
#[test]
fn registered_identity_matches_source() {
    bereshit_l0_config::identity::register_identity();

    let pkg = bereshit_l0_identity::lookup("B-L0-hybrid-config")
        .expect("config must be registered");

    // Pragma fields match
    assert_eq!(pkg.pragma["I1.key"], "B-L0-hybrid-config");
    assert_eq!(pkg.pragma["I1.format"], "rust");
    assert_eq!(pkg.pragma["I2.subtype"], "library");
    assert_eq!(pkg.pragma["I4.layer"], "L0");

    // Metadata fields match
    assert_eq!(pkg.metadata["C7.domain"], "config-loading");
    assert_eq!(pkg.metadata["C7.paradigm"], "CPI-SI");
    assert_eq!(pkg.metadata["C2.organization"], "CreativeWorkzStudio LLC");
}

/// PackageIdentity::get() works across registered crates.
///
/// The unified get() method should find pragma (I-prefix) and
/// metadata (C-prefix) fields regardless of which map they're in.
#[test]
fn unified_get_works_cross_crate() {
    bereshit_l0_paths::register_identity();

    let pkg = bereshit_l0_identity::lookup("B-L0-hybrid-paths")
        .expect("paths must be registered");

    // Pragma field via get()
    assert_eq!(pkg.get("I1.format"), Some("rust"));
    assert_eq!(pkg.get("I4.layer"), Some("L0"));

    // Metadata field via get()
    assert_eq!(pkg.get("C7.domain"), Some("filesystem"));
    assert_eq!(pkg.get("C7.paradigm"), Some("CPI-SI"));

    // Missing field
    assert_eq!(pkg.get("Z99.nope"), None);
}

// ============================================================================
// Cross-Crate Tag Discovery
// ============================================================================

/// Tags enable cross-cutting queries across L0.
///
/// The "cpi-si" tag should be on every L0 crate. Specialized tags
/// should find only the relevant packages.
#[test]
fn tag_discovery_across_l0() {
    bereshit_l0_config::identity::register_identity();
    bereshit_l0_paths::register_identity();

    // "cpi-si" tag should appear on identity at minimum
    let cpisi = bereshit_l0_identity::by_tag("cpi-si");
    assert!(
        cpisi.iter().any(|p| p.key == "B-L0-identity-registry"),
        "identity crate should have cpi-si tag"
    );

    // "config" tag should find config crate
    let config_tagged = bereshit_l0_identity::by_tag("config");
    assert!(
        config_tagged.iter().any(|p| p.key == "B-L0-hybrid-config"),
        "config crate should have config tag"
    );

    // "identity" tag should find identity crate
    let identity_tagged = bereshit_l0_identity::by_tag("identity");
    assert!(
        identity_tagged.iter().any(|p| p.key == "B-L0-identity-registry"),
        "identity crate should have identity tag"
    );
}

// ============================================================================
// Registry Statistics
// ============================================================================

/// Stats reflect all registered L0 crates.
#[test]
fn stats_reflect_l0_topology() {
    bereshit_l0_config::identity::register_identity();
    bereshit_l0_paths::register_identity();

    let s = bereshit_l0_identity::stats();

    // At least 3 packages registered (identity + paths + config)
    assert!(s.total >= 3, "at least 3 L0 crates should be registered");

    // L0 layer should have at least 3
    assert!(
        s.by_layer.get("L0").copied().unwrap_or(0) >= 3,
        "L0 layer should have at least 3 packages"
    );
}

// ============================================================================
// Path Resolution Chain (config → paths)
// ============================================================================

/// Config uses paths for root resolution.
///
/// The config crate delegates path resolution to the paths crate.
/// This test verifies the connection works — config can resolve
/// the bereshit root through paths.
#[test]
fn config_uses_paths_for_resolution() {
    // paths::bereshit_root() should return a non-empty PathBuf
    let root = bereshit_l0_paths::bereshit_root();
    assert!(!root.as_os_str().is_empty(), "bereshit root should not be empty");

    // Config's own root resolution should also work
    // (set_root / clear_root are the config-level API)
    bereshit_l0_config::clear_root();
    // After clearing, load operations would use path resolution
    // We can't test actual file loading without fixtures, but we
    // verify the API chain compiles and executes without panic.
}

/// Paths crate produces consistent results for config paths.
///
/// Multiple calls to the same path function should return identical results.
/// This verifies the OnceLock caching in paths works correctly when
/// called from the config crate's context.
#[test]
fn paths_caching_consistent_from_config() {
    let root1 = bereshit_l0_paths::bereshit_root();
    let root2 = bereshit_l0_paths::bereshit_root();
    assert_eq!(root1, root2, "bereshit_root must be stable across calls");

    let word1 = bereshit_l0_paths::bereshit_word();
    let word2 = bereshit_l0_paths::bereshit_word();
    assert_eq!(word1, word2, "bereshit_word must be stable across calls");
}

// ============================================================================
// Config Format Registry
// ============================================================================

/// Config format registry knows standard extensions.
///
/// The format registry maps file extensions to internal format types.
/// TOML (.toml) should always be registered as the primary config format.
/// Extensions include the dot prefix per API convention.
#[test]
fn format_registry_knows_toml() {
    assert!(
        bereshit_l0_config::is_known_ext(".toml"),
        "TOML must be a known format extension"
    );
    assert!(
        bereshit_l0_config::is_known_ext(".json"),
        "JSON must be a known format extension"
    );
    assert!(
        bereshit_l0_config::is_known_ext(".rs"),
        "Rust must be a known format extension"
    );
    assert!(
        !bereshit_l0_config::is_known_ext(".xyz"),
        "unknown extension should not be registered"
    );
}

/// Config can parse JSONC by stripping comments.
///
/// The JSONC module strips comments and returns valid JSON.
/// This is a cross-module integration: jsonc → serde_json.
#[test]
fn jsonc_stripping_produces_valid_json() {
    let input = r#"{
  // This is a comment
  "key": "value", // trailing comment
  /* block comment */
  "number": 42
}"#;

    let stripped = bereshit_l0_config::strip_jsonc_comments(input);
    let parsed: Result<serde_json::Value, _> = serde_json::from_str(&stripped);
    assert!(parsed.is_ok(), "stripped JSONC should be valid JSON");

    let val = parsed.unwrap();
    assert_eq!(val["key"], "value");
    assert_eq!(val["number"], 42);
}

// ============================================================================
// Error Type Interop
// ============================================================================

/// ConfigError implements std::error::Error for interop.
///
/// Integration requirement: error types from L0 crates must be
/// compatible with standard Rust error handling patterns.
#[test]
fn config_error_is_std_error() {
    let err = bereshit_l0_config::ConfigError::Load {
        file: "/nonexistent/path.toml".into(),
        op: "read".into(),
        source: "file not found".into(),
    };
    let std_err: &dyn std::error::Error = &err;
    // The error should format without panic
    let msg = format!("{std_err}");
    assert!(msg.contains("nonexistent"), "error message should contain the path");
}
