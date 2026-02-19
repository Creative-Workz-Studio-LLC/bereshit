//! OmniCode Pragma Reader — The First Thing Read in Any OmniCode File.
//!
//! Parses `#!omni` pragma lines to establish file identity, dispatches
//! to L0 format parsers, and maintains the 16-extension registry.
//!
//! # Architecture
//!
//! L1 (this crate) bridges OmniCode identity and L0 format parsing:
//!
//! ```text
//!   File (.omni/.ofd)
//!     ↓ Parse pragma (L1 — reader.rs)
//!   Pragma { type, format, key, from, at }
//!     ↓ Dispatch by format (L1 — dispatch.rs)
//!   L0 parser (TOML / JSONC / future: native OmniCode)
//!     ↓ Returns typed data
//!   Caller gets Pragma + parsed config
//! ```
//!
//! # L0 Connections (compiler-verified)
//!
//! | L1 Function | Calls L0 | Module |
//! |-------------|----------|--------|
//! | `dispatch::load_config<T>` | `load_toml_file::<T>(path)` | format_loader |
//! | `dispatch::load_config<T>` | `load_jsonc_file::<T>(path)` | format_loader |
//! | `extensions::register_all_with_l0()` | `register_format(ext, fmt)` | formats |
//! | `register_identity()` | `identity::register(key, p, m)` | identity |
//!
//! # Usage
//!
//! ```no_run
//! use bereshit_l1_pragma as pragma;
//!
//! // Parse a file's identity
//! let p = pragma::parse(std::path::Path::new("config.omni")).unwrap();
//! println!("{}", p); // #!omni data --toml [key=B-config]
//! ```
//!
//! Genesis 1:3 — "And God said, Let there be light."
//! The pragma is the first word spoken — identity before content.

//omni:code --rust -library
//omni:key B-L1-omnicode-hybrid-pragma
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:      B-L1-omnicode-hybrid-pragma
// Purpose:  OmniCode pragma reader — identity before content
// Biblical: Genesis 1:3 — "And God said, Let there be light"
// Authors:  Nova Dawn
// Version:  a-01.00
// Created:  2026-02-14
//
// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

pub mod cache;
pub mod dispatch;
pub mod extensions;
pub mod reader;
pub mod types;

// Re-export core public API
pub use cache::{invalidate as invalidate_pragma, invalidate_all as invalidate_pragma_cache};
pub use dispatch::{
    invalidate_dispatch_cache, invalidate_dispatch_entry, is_omnicode_file, load_config,
    load_config_map, load_folder,
};
pub use extensions::{
    all as all_exts, block_pattern, by_category, by_phase, by_state, default_type, internal_format,
    is_registered, lookup, register_all_with_l0,
};
pub use reader::{MAX_PRAGMA_LINES, PRAGMA_PREFIX, parse, parse_reader, parse_str};
pub use types::*;

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Identity Registration
// ────────────────────────────────────────────────────────────────

/// Register this crate's identity with the L0 identity registry.
///
/// Call this during initialization to make L1-pragma a known citizen
/// in the system's self-awareness layer.
///
/// Also registers all OmniCode file extensions with L0's format registry.
pub fn register_identity() {
    let key = pragma_get("I1.key").expect("I1.key must exist in OWN_PRAGMA");
    bereshit_l0_identity::register(key, OWN_PRAGMA, OWN_METADATA);

    // Register all OmniCode extensions with L0 format registry
    extensions::register_all_with_l0();
}

// ────────────────────────────────────────────────────────────────
// Self — Identity Data
// ────────────────────────────────────────────────────────────────

/// This crate's OmniCode pragma identity (I1-I4).
pub fn own_pragma() -> &'static [(&'static str, &'static str)] {
    OWN_PRAGMA
}

/// Look up a single pragma value by key.
pub fn pragma_get(key: &str) -> Option<&'static str> {
    OWN_PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// This crate's OmniCode metadata (C1-C7).
pub fn own_metadata() -> &'static [(&'static str, &'static str)] {
    OWN_METADATA
}

/// Look up a single metadata value by key.
pub fn metadata_get(key: &str) -> Option<&'static str> {
    OWN_METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

static OWN_PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L1-omnicode-hybrid-pragma"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/L1-omnicode/hybrid/pragma/pragma.go"),
    ("I1.at",        "a-01.50"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I2.role",      "hybrid"),
    // I3: Instance
    ("I3.file",      "lib.rs"),
    ("I3.title",     "OmniCode Pragma Reader"),
    ("I3.component", "hybrid/pragma"),
    ("I3.path",      "L1-omnicode/hybrid/pragma/src/lib.rs"),
    ("I3.provides",  "PRAGMA_PARSING"),
    ("I3.brief",     "Parse #!omni pragma lines from any file format — identity before content"),
    // I4: Architecture
    ("I4.layer",     "L1"),
    ("I4.position",  "hybrid/pragma"),
    ("I4.pattern",   "L1 bridges OmniCode identity (pragma) with L0 format parsing"),
];

static OWN_METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "a-01.50"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-14"),
    ("C1.updated",           "2026-02-19"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Genesis 1:3 — And God said, Let there be light"),
    ("C3.principle",         "The pragma is the first word spoken — identity before content"),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "std::collections, std::io, std::path, std::sync"),
    ("C4.requires.external", "serde (derive), serde_json"),
    ("C4.requires.internal", "bereshit-l0-config (loaders, registry), bereshit-l0-identity (registration)"),
    ("C4.consumers",         "L2-platform/substrates, L3-cpisi/orchestration, L5-applications"),
    ("C4.integration",       "use bereshit_l1_pragma::{parse, load_config, register_identity}"),
    ("C4.if_missing",        "no OmniCode file identity — files become opaque data without pragma parsing"),
    // C5: Intent
    ("C5.purpose",           "The first thing read in any OmniCode file — identity before content"),
    ("C5.philosophy",        "Identity declares existence. Pragma precedes processing. Genesis 1:3 before 1:4."),
    // C6: Roadmap
    ("C6.current",           "a-01.50 — Pragma reader, 16-extension registry, format dispatch, I/C metadata"),
    ("C6.planned",           "Native OmniCode parser (FormatOmni dispatch), block validator, derivation checker"),
    ("C6.limitations",       "Native OmniCode format not yet parseable, block validation not yet implemented"),
    // C7: Classification
    ("C7.tags",              "pragma, identity, parser, omnicode, format-dispatch, extensions"),
    ("C7.category",          "Hybrid"),
    ("C7.domain",            "pragma"),
    ("C7.paradigm",          "CPI-SI"),
];

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// The pragma reader is the first thing that touches an OmniCode file.
// Before content is parsed, before format is dispatched, before blocks are
// validated — the pragma declares WHAT this file IS.
//
// Like Genesis 1:3 ("Let there be light") precedes all other creation,
// the pragma precedes all other processing. Identity first. Always.
//
// L0-L1 connections exercised by this crate:
//   - load_toml_file<T>    (L0 config → L1 dispatch)
//   - load_jsonc_file<T>   (L0 config → L1 dispatch)
//   - register_format()    (L0 formats ← L1 extensions)
//   - register()           (L0 identity ← L1 lib)
//
// The compiler witnesses all four. If any signature changes, this crate
// won't compile. That's not a limitation — it's the contract working.
//
// ============================================================================
// END CLOSING
// ============================================================================

// ════════════════════════════════════════════════════════════════
// Tests
// ════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_own_pragma_identity() {
        assert_eq!(pragma_get("I1.key"), Some("B-L1-omnicode-hybrid-pragma"));
        assert_eq!(pragma_get("I1.format"), Some("rust"));
        assert_eq!(pragma_get("I2.subtype"), Some("library"));
        assert_eq!(pragma_get("I3.title"), Some("OmniCode Pragma Reader"));
    }

    #[test]
    fn test_own_metadata_identity() {
        assert_eq!(metadata_get("C7.domain"), Some("pragma"));
        assert_eq!(metadata_get("C7.paradigm"), Some("CPI-SI"));
        assert_eq!(
            metadata_get("C3.anchor"),
            Some("Genesis 1:1 — In the beginning God created the heaven and the earth"),
        );
    }

    #[test]
    fn test_identity_completeness() {
        for prefix in ["I1.", "I2.", "I3.", "I4."] {
            assert!(
                OWN_PRAGMA.iter().any(|(k, _)| k.starts_with(prefix)),
                "missing pragma section {prefix}"
            );
        }

        for prefix in ["C1.", "C2.", "C3.", "C4.", "C5.", "C6.", "C7."] {
            assert!(
                OWN_METADATA.iter().any(|(k, _)| k.starts_with(prefix)),
                "missing metadata section {prefix}"
            );
        }
    }

    #[test]
    fn test_register_identity_with_l0() {
        register_identity();

        // L1 pragma should now be discoverable through L0's identity registry
        let pkg = bereshit_l0_identity::lookup("B-L1-omnicode-hybrid-pragma");
        assert!(
            pkg.is_some(),
            "L1 pragma should be registered in L0 identity"
        );

        let pkg = pkg.unwrap();
        assert_eq!(pkg.pragma["I1.format"], "rust");
        assert_eq!(pkg.pragma["I4.layer"], "L1");

        // L1 packages should appear in L0's layer query
        let l1_pkgs = bereshit_l0_identity::by_layer("L1");
        assert!(
            l1_pkgs
                .iter()
                .any(|p| p.key == "B-L1-omnicode-hybrid-pragma"),
            "L1 pragma should appear in by_layer(\"L1\")"
        );

        // OmniCode extensions should be registered with L0 format registry
        assert!(bereshit_l0_config::is_known_ext(EXT_OMNI));
        assert!(bereshit_l0_config::is_known_ext(EXT_OMNIFOLD));
    }
}
