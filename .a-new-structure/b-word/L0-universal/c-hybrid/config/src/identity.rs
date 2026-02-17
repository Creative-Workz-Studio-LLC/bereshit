//! Package-level OmniCode identity — runtime-queryable pragma and metadata.
//!
//! Ported from Go `doc.go`. Each package carries its own identity via
//! `[_pragma]` (P1-P5) and `[_metadata]` (M1-M10) maps.
//!
//! 1 Corinthians 12:12 — "For as the body is one, and hath many members."
//! Many files, one purpose. Each util serves the whole, none stands alone.

use std::collections::BTreeMap;
use std::sync::LazyLock;

// ────────────────────────────────────────────────────────────────
// Package Identity
// ────────────────────────────────────────────────────────────────

/// OmniCode pragma (P1-P5) for the config crate.
static PRAGMA: LazyLock<BTreeMap<String, String>> = LazyLock::new(|| {
    BTreeMap::from([
        // P1: Core Identity
        ("P1.key".into(), "B-L0-hybrid-config".into()),
        ("P1.type".into(), "code".into()),
        ("P1.format".into(), "rs".into()),
        ("P1.style".into(), "library".into()),
        // P2: Structure
        ("P2.blocks".into(), "4-block".into()),
        // P3: Derivation
        ("P3.from".into(), "b-word/seed/code/L0/go/library.go".into()),
        ("P3.derives".into(), "B-L0-seed-go-library".into()),
        // P4: Version
        ("P4.at".into(), "b-02.00".into()),
        // P5: Summary
        ("P5.title".into(), "Config Crate".into()),
        (
            "P5.summary".into(),
            "Manifest-driven TOML spec loader with format dispatch and utilities".into(),
        ),
    ])
});

/// OmniCode metadata (M1-M10) for the config crate.
static METADATA: LazyLock<BTreeMap<String, String>> = LazyLock::new(|| {
    BTreeMap::from([
        // M1: Core Identity
        ("M1.key".into(), "B-L0-hybrid-config".into()),
        ("M1.component_type".into(), "Foundation".into()),
        ("M1.architect".into(), "Seanje Lenox-Wise".into()),
        ("M1.implementation".into(), "Nova Dawn".into()),
        ("M1.created".into(), "2026-02-14".into()),
        // M2: Version History
        (
            "M2.b-02.00".into(),
            "2026-02-14 — BASELINE: 13 modules, faithful Go port, production-grade".into(),
        ),
        (
            "M2.b-01.00".into(),
            "2026-02-14 — Initial: 8 modules, manifest loader core".into(),
        ),
        // M3: Interface
        (
            "M3.requires.external".into(),
            "toml 0.8, serde 1, serde_json 1".into(),
        ),
        ("M3.requires.internal".into(), "(none — L0 foundation)".into()),
        (
            "M3.used_by".into(),
            "L3-cpisi/orchestration/config, L3-cpisi/foundation/schema".into(),
        ),
        ("M3.import".into(), "bereshit-l0-config".into()),
        // M4: Public API
        (
            "M4.error".into(),
            "ConfigError (Load, Validation, Dependency, RootNotSet)".into(),
        ),
        (
            "M4.exists".into(),
            "file_exists, dir_exists, is_file, ensure_dir".into(),
        ),
        (
            "M4.formats".into(),
            "format_from_ext, format_from_path, is_config_format, is_config_path, is_known_ext"
                .into(),
        ),
        ("M4.jsonc".into(), "strip_jsonc_comments, parse_jsonc".into()),
        (
            "M4.loader".into(),
            "load_config_file, load_toml_file, load_toml_bytes, find_and_load, find_config, find_config_dir"
                .into(),
        ),
        ("M4.must".into(), "MustExt, MustValExt".into()),
        (
            "M4.manifest".into(),
            "load_all, load_system, load_spec, load_manifest".into(),
        ),
        (
            "M4.identity".into(),
            "pragma, metadata (copy-returning accessors)".into(),
        ),
        // M5: Operational
        (
            "M5.blocking".into(),
            "no — file I/O only, no network".into(),
        ),
        (
            "M5.health".into(),
            "Foundation | granted: loaded | deferred: n/a | denied: error".into(),
        ),
        // M6: Classification
        (
            "M6.tags".into(),
            "config, toml, jsonc, formats, loader, exists, must, identity".into(),
        ),
        ("M6.category".into(), "Foundation".into()),
        ("M6.domain".into(), "config-loading".into()),
        ("M6.layer".into(), "L0-universal".into()),
        ("M6.paradigm".into(), "CPI-SI".into()),
        // M7: Intent
        (
            "M7.purpose".into(),
            "Unified config loading — manifest-driven with format dispatch and utilities".into(),
        ),
        (
            "M7.philosophy".into(),
            "Faithful Go port using Rust idioms. L0 stays universal — no OmniCode, no L1+ deps"
                .into(),
        ),
        // M8: Grounding
        (
            "M8.scripture".into(),
            "Habakkuk 2:2-3 — Write the vision, make it plain".into(),
        ),
        (
            "M8.anchor".into(),
            "Genesis 1:1 — In the beginning God created".into(),
        ),
        // M9: Dependencies
        (
            "M9.needs.external".into(),
            "toml 0.8, serde 1, serde_json 1".into(),
        ),
        ("M9.needs.internal".into(), "(none)".into()),
        (
            "M9.layer_deps".into(),
            "L0 only — no L1+ dependencies".into(),
        ),
        // M10: Roadmap
        (
            "M10.current".into(),
            "b-02.00 — BASELINE: 13 modules, faithful Go port".into(),
        ),
        (
            "M10.planned".into(),
            "YAML loading, schema validation helpers".into(),
        ),
        (
            "M10.limitations".into(),
            "File I/O only, no network, no streaming, no watch/hot-reload".into(),
        ),
    ])
});

// ────────────────────────────────────────────────────────────────
// Public API
// ────────────────────────────────────────────────────────────────

/// Returns a copy of the OmniCode pragma map for this crate.
///
/// Returns a copy — mutations do not affect the original.
pub fn pragma() -> BTreeMap<String, String> {
    PRAGMA.clone()
}

/// Returns a copy of the OmniCode metadata map for this crate.
///
/// Returns a copy — mutations do not affect the original.
pub fn metadata() -> BTreeMap<String, String> {
    METADATA.clone()
}

/// Register this crate's identity with the global registry.
///
/// Rust equivalent of Go's `init() { identity.Register(key, pragma, metadata) }`.
/// Call once during application startup to make this crate discoverable
/// via `bereshit_l0_identity::lookup()`.
pub fn register_identity() {
    let key = PRAGMA.get("P1.key").expect("P1.key must exist in PRAGMA");
    bereshit_l0_identity::register(key, (*PRAGMA).clone(), (*METADATA).clone());
}
