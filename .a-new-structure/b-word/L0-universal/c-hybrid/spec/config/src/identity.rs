//! Package-level OmniCode identity — runtime-queryable pragma and metadata.
//!
//! Every CPI-SI crate carries its own identity via static PRAGMA (I1-I4) and
//! METADATA (C1-C7) slices. Zero-cost at runtime — compiled into the binary.
//!
//! 1 Corinthians 12:12 — "For as the body is one, and hath many members."
//! Many files, one purpose. Each module serves the whole, none stands alone.

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for the config crate.
///
/// Carries the I-sections (Identity) of the OmniCode metadata standard.
/// These fields establish WHO this crate is in the project graph.
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-hybrid-config"),
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
    ("I3.title",     "Config Crate"),
    ("I3.component", "c-hybrid/spec/config"),
    ("I3.path",      "L0-universal/c-hybrid/spec/config/src/lib.rs"),
    ("I3.provides",  "CONFIG_LOADING"),
    ("I3.brief",     "Manifest-driven TOML spec loader with format dispatch and utilities"),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "hybrid/spec/config"),
    ("I4.pattern",   "manifest-driven loading — index.toml declares what to load"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context for the config crate.
///
/// Carries the C-sections (Context) of the OmniCode metadata standard.
/// These fields establish WHEN, WHERE, WHY, and HOW for this crate.
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-03.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-14"),
    ("C1.updated",           "2026-02-19"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Habakkuk 2:2-3 — Write the vision, make it plain"),
    ("C3.principle",         "Configuration is vision made plain — declare what exists, the system loads it"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "std::collections, std::io, std::path, std::sync"),
    ("C4.requires.external", "toml 0.8, serde 1, serde_json 1"),
    ("C4.requires.internal", "bereshit-l0-identity (registration)"),
    ("C4.consumers",         "L3 orchestration, schema validation, any crate loading TOML config"),
    ("C4.integration",       "use bereshit_l0_config::{load_toml_file, find_config_dir, pragma_get}"),
    ("C4.if_missing",        "every crate implements its own TOML loading — duplicated effort, inconsistent behavior"),
    // C5: Intent
    ("C5.purpose",           "Unified config loading — manifest-driven with format dispatch and utilities"),
    ("C5.philosophy",        "L0 stays universal — no OmniCode, no L1+ deps. Faithful Go port using Rust idioms."),
    // C6: Roadmap
    ("C6.current",           "b-03.00 — 13 modules, numbered BODY, standard CLOSING, production structure"),
    ("C6.planned",           "YAML loading, schema validation helpers"),
    ("C6.limitations",       "File I/O only, no network, no streaming, no watch/hot-reload"),
    // C7: Classification
    ("C7.tags",              "config, toml, jsonc, formats, loader, exists, must, identity"),
    ("C7.category",          "Foundation"),
    ("C7.domain",            "config-loading"),
    ("C7.paradigm",          "CPI-SI"),
];

// ──────────────────────────────────────────────────────────────────────────
// Public API
// ──────────────────────────────────────────────────────────────────────────

/// Returns this crate's OmniCode pragma identity (I1-I4).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Look up a single pragma value by key.
///
/// ```
/// use bereshit_l0_config::pragma_get;
///
/// assert_eq!(pragma_get("I1.key"), Some("B-L0-hybrid-config"));
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
/// use bereshit_l0_config::metadata_get;
///
/// assert_eq!(metadata_get("C1.version"), Some("b-03.00"));
/// assert_eq!(metadata_get("missing"), None);
/// ```
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Register this crate's identity with the global registry.
///
/// Call once during application startup to make this crate discoverable
/// via `bereshit_l0_identity::lookup()`.
pub fn register_identity() {
    let key = pragma_get("I1.key").expect("I1.key must exist in PRAGMA");
    bereshit_l0_identity::register(key, PRAGMA, METADATA);
}
