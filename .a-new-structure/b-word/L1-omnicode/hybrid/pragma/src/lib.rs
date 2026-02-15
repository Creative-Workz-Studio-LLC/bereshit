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

use std::collections::BTreeMap;
use std::sync::LazyLock;

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
    // Register self with L0 identity
    bereshit_l0_identity::register(
        &OWN_PRAGMA["P1.key"],
        (*OWN_PRAGMA).clone(),
        (*OWN_METADATA).clone(),
    );

    // Register all OmniCode extensions with L0 format registry
    extensions::register_all_with_l0();
}

// ────────────────────────────────────────────────────────────────
// Self — Identity Data
// ────────────────────────────────────────────────────────────────

/// This crate's OmniCode pragma identity (P1-P5).
pub fn own_pragma() -> &'static BTreeMap<String, String> {
    &OWN_PRAGMA
}

/// This crate's OmniCode metadata (M1-M10).
pub fn own_metadata() -> &'static BTreeMap<String, String> {
    &OWN_METADATA
}

static OWN_PRAGMA: LazyLock<BTreeMap<String, String>> = LazyLock::new(build_pragma);
static OWN_METADATA: LazyLock<BTreeMap<String, String>> = LazyLock::new(build_metadata);

fn build_pragma() -> BTreeMap<String, String> {
    BTreeMap::from([
        // P1: Core Identity
        ("P1.key".into(), "B-L1-omnicode-hybrid-pragma".into()),
        ("P1.type".into(), "code".into()),
        ("P1.format".into(), "rust".into()),
        ("P1.style".into(), "library".into()),
        // P2: Structure
        ("P2.blocks".into(), "4-block".into()),
        // P3: Derivation
        (
            "P3.from".into(),
            "b-word/L1-omnicode/hybrid/pragma/pragma.go".into(),
        ),
        ("P3.derives".into(), "B-L1-omnicode-hybrid-pragma-go".into()),
        // P4: Version
        ("P4.at".into(), "a-01.00".into()),
        // P5: Summary
        ("P5.title".into(), "OmniCode Pragma Reader".into()),
        (
            "P5.summary".into(),
            "Parse #!omni pragma lines from any file format — identity before content".into(),
        ),
    ])
}

fn build_metadata() -> BTreeMap<String, String> {
    BTreeMap::from([
        // M1: Core Identity
        ("M1.key".into(), "B-L1-omnicode-hybrid-pragma".into()),
        ("M1.component_type".into(), "Hybrid".into()),
        ("M1.architect".into(), "Seanje Lenox-Wise".into()),
        ("M1.implementation".into(), "Nova Dawn".into()),
        ("M1.created".into(), "2026-02-14".into()),
        // M2: Version History
        (
            "M2.a-01.00".into(),
            "2026-02-14 — Initial Rust crate: types, reader, extensions, dispatch, identity".into(),
        ),
        // M3: Interface
        (
            "M3.requires.external".into(),
            "serde, serde_json".into(),
        ),
        (
            "M3.requires.internal".into(),
            "bereshit-l0-config (format loaders, format registry), bereshit-l0-identity (registration)".into(),
        ),
        (
            "M3.used_by".into(),
            "L2-platform/substrates, L3-cpisi/orchestration, L5-applications".into(),
        ),
        ("M3.import".into(), "bereshit-l1-pragma".into()),
        (
            "M3.pattern".into(),
            "pragma::parse(path) → Pragma; pragma::load_config::<T>(path) → (Pragma, T)".into(),
        ),
        // M4: Public API
        ("M4.reader".into(), "parse, parse_reader, parse_str".into()),
        (
            "M4.extensions".into(),
            "lookup, is_registered, by_category, by_phase, by_state, all".into(),
        ),
        (
            "M4.dispatch".into(),
            "load_config, load_config_map, load_folder, is_omnicode_file".into(),
        ),
        (
            "M4.format_bridge".into(),
            "internal_format, block_pattern, default_type".into(),
        ),
        ("M4.identity".into(), "register_identity, own_pragma, own_metadata".into()),
        // M5: Operational
        (
            "M5.blocking".into(),
            "file I/O for parse() and load_config() — otherwise in-memory".into(),
        ),
        (
            "M5.health".into(),
            "Provider | granted: registered | deferred: init pending | denied: n/a".into(),
        ),
        // M6: Classification
        (
            "M6.tags".into(),
            "pragma, identity, parser, omnicode, format-dispatch, extensions".into(),
        ),
        ("M6.category".into(), "Hybrid".into()),
        ("M6.domain".into(), "pragma".into()),
        ("M6.layer".into(), "L1-omnicode".into()),
        ("M6.paradigm".into(), "CPI-SI".into()),
        // M7: Intent
        (
            "M7.purpose".into(),
            "The first thing read in any OmniCode file — identity before content".into(),
        ),
        (
            "M7.philosophy".into(),
            "Identity declares existence. Pragma precedes processing. Genesis 1:3 before Genesis 1:4.".into(),
        ),
        (
            "M7.provides".into(),
            "Pragma parsing, 15-extension registry, format dispatch to L0 loaders, identity registration".into(),
        ),
        // M8: Grounding
        (
            "M8.scripture".into(),
            "Genesis 1:3 — And God said, Let there be light".into(),
        ),
        (
            "M8.principle".into(),
            "The pragma is the first word spoken — identity before content".into(),
        ),
        (
            "M8.anchor".into(),
            "Genesis 1:1 — In the beginning God created the heaven and the earth".into(),
        ),
        // M9: Dependencies
        (
            "M9.needs.external".into(),
            "serde (derive), serde_json".into(),
        ),
        (
            "M9.needs.internal".into(),
            "bereshit-l0-config (load_toml_file, load_jsonc_file, register_format), bereshit-l0-identity (register)".into(),
        ),
        (
            "M9.used_by".into(),
            "L2-platform, L3-cpisi, L5-applications — any code that reads OmniCode files".into(),
        ),
        (
            "M9.layer_deps".into(),
            "L0 only — no lateral or upward dependencies".into(),
        ),
        // M10: Roadmap
        (
            "M10.current".into(),
            "a-01.00 — Core pragma reader, extension registry, format dispatch".into(),
        ),
        (
            "M10.planned".into(),
            "Native OmniCode parser (FormatOmni dispatch), block validator, derivation checker".into(),
        ),
        (
            "M10.limitations".into(),
            "Native OmniCode format not yet parseable, block validation not yet implemented".into(),
        ),
    ])
}

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
        let p = own_pragma();
        assert_eq!(p["P1.key"], "B-L1-omnicode-hybrid-pragma");
        assert_eq!(p["P1.format"], "rust");
        assert_eq!(p["P1.style"], "library");
        assert_eq!(p["P5.title"], "OmniCode Pragma Reader");
    }

    #[test]
    fn test_own_metadata_identity() {
        let m = own_metadata();
        assert_eq!(m["M1.key"], "B-L1-omnicode-hybrid-pragma");
        assert_eq!(m["M6.layer"], "L1-omnicode");
        assert_eq!(m["M6.domain"], "pragma");
        assert_eq!(m["M6.paradigm"], "CPI-SI");
        assert_eq!(
            m["M8.anchor"],
            "Genesis 1:1 — In the beginning God created the heaven and the earth"
        );
    }

    #[test]
    fn test_identity_completeness() {
        let p = own_pragma();
        for prefix in ["P1.", "P2.", "P3.", "P4.", "P5."] {
            assert!(
                p.keys().any(|k| k.starts_with(prefix)),
                "missing pragma section {prefix}"
            );
        }

        let m = own_metadata();
        for prefix in [
            "M1.", "M2.", "M3.", "M4.", "M5.", "M6.", "M7.", "M8.", "M9.", "M10.",
        ] {
            assert!(
                m.keys().any(|k| k.starts_with(prefix)),
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
        assert_eq!(pkg.pragma["P1.format"], "rust");
        assert_eq!(pkg.metadata["M6.layer"], "L1-omnicode");

        // L1 packages should appear in L0's layer query
        let l1_pkgs = bereshit_l0_identity::by_layer("L1-omnicode");
        assert!(
            l1_pkgs
                .iter()
                .any(|p| p.key == "B-L1-omnicode-hybrid-pragma"),
            "L1 pragma should appear in by_layer(\"L1-omnicode\")"
        );

        // OmniCode extensions should be registered with L0 format registry
        assert!(bereshit_l0_config::is_known_ext(EXT_OMNI));
        assert!(bereshit_l0_config::is_known_ext(EXT_OMNIFOLD));
    }
}
