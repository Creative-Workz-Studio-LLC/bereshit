//! OmniCode package identity registry — runtime self-awareness for CPI-SI.
//!
//! Ported from Go `registry.go` (L0-universal/spiral/identity). Every crate
//! with pragma and metadata registers itself, making the system self-aware
//! at runtime. Two access paths:
//!
//!   Static:  `grep -rn "//omni:key"` → finds all file identities
//!   Runtime: `identity::lookup("B-L0-config-loader")` → pragma, metadata
//!
//! Ladder: register → lookup / all / by_layer / by_domain / by_tag
//! Baton:  crate init → register(key, pragma, metadata) → registry stores identity
//!
//! Genesis 1:5 — "And God called the light Day, and the darkness he called Night."
//! Naming establishes identity. Registration declares existence.

use std::collections::BTreeMap;
use std::sync::{LazyLock, RwLock};

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

/// A registered package's OmniCode identity.
///
/// Pragma carries file-level identity (P1-P5).
/// Metadata carries full context (M1-M10).
#[derive(Debug, Clone)]
pub struct PackageIdentity {
    /// OmniCode key (P1.key / M1.key).
    pub key: String,
    /// P1-P5 pragma fields.
    pub pragma: BTreeMap<String, String>,
    /// M1-M10 metadata fields.
    pub metadata: BTreeMap<String, String>,
}

// ────────────────────────────────────────────────────────────────
// Registry (global state)
// ────────────────────────────────────────────────────────────────

/// Global identity registry — the system knowing itself.
///
/// Initialized with the identity crate pre-registered (self-awareness bootstrap).
/// Uses `LazyLock` + `RwLock` matching Go's `sync.RWMutex` pattern.
///
/// Go equivalent: `var mu sync.RWMutex` + `var packages = make(map[string]*PackageIdentity)`
/// plus `func init() { Register(...) }` for self-registration.
static REGISTRY: LazyLock<RwLock<BTreeMap<String, PackageIdentity>>> = LazyLock::new(|| {
    let mut map = BTreeMap::new();

    // Self-registration — the bootstrap anchor (Genesis 1:1).
    // Go does this in init(). LazyLock achieves the same: exactly once, before
    // any access. The registry knowing itself IS the foundation.
    let key = OWN_PRAGMA["P1.key"].clone();
    map.insert(
        key.clone(),
        PackageIdentity {
            key,
            pragma: (*OWN_PRAGMA).clone(),
            metadata: (*OWN_METADATA).clone(),
        },
    );

    RwLock::new(map)
});

// ────────────────────────────────────────────────────────────────
// Registration
// ────────────────────────────────────────────────────────────────

/// Register a crate's identity in the global registry.
///
/// Rust equivalent of Go's `init() { identity.Register(key, pragma, metadata) }`.
/// Ownership transfer replaces Go's `copyMap()` — the caller gives up the maps,
/// the registry owns them. No mutation possible after registration.
///
/// Duplicate keys overwrite silently (last writer wins).
pub fn register(key: &str, pragma: BTreeMap<String, String>, metadata: BTreeMap<String, String>) {
    let identity = PackageIdentity {
        key: key.to_owned(),
        pragma,
        metadata,
    };

    let mut guard = REGISTRY.write().unwrap_or_else(|e| e.into_inner());
    guard.insert(key.to_owned(), identity);
}

// ────────────────────────────────────────────────────────────────
// Lookup
// ────────────────────────────────────────────────────────────────

/// Look up a package by its OmniCode key.
///
/// Returns a clone of the registered identity, or `None` if not found.
/// Go returns `(*PackageIdentity, bool)`; Rust uses `Option`.
pub fn lookup(key: &str) -> Option<PackageIdentity> {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard.get(key).cloned()
}

/// Return every registered package.
///
/// Order is by key (BTreeMap = sorted). Go returns unordered map iteration;
/// Rust's BTreeMap gives deterministic order — an improvement.
pub fn all() -> Vec<PackageIdentity> {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard.values().cloned().collect()
}

/// Return the number of registered packages.
pub fn count() -> usize {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard.len()
}

// ────────────────────────────────────────────────────────────────
// Query
// ────────────────────────────────────────────────────────────────

/// Return all packages at a given layer (M6.layer).
///
/// Example: `identity::by_layer("L0-universal")`
pub fn by_layer(layer: &str) -> Vec<PackageIdentity> {
    by_metadata("M6.layer", layer)
}

/// Return all packages in a given domain (M6.domain).
///
/// Example: `identity::by_domain("config-loading")`
pub fn by_domain(domain: &str) -> Vec<PackageIdentity> {
    by_metadata("M6.domain", domain)
}

/// Return all packages that have a specific tag in M6.tags.
///
/// Tags are comma-separated in the metadata value.
/// Example: `identity::by_tag("tripwire")`
pub fn by_tag(tag: &str) -> Vec<PackageIdentity> {
    let tag = tag.trim();
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard
        .values()
        .filter(|p| {
            p.metadata
                .get("M6.tags")
                .is_some_and(|tags| tags.split(',').any(|t| t.trim() == tag))
        })
        .cloned()
        .collect()
}

// ────────────────────────────────────────────────────────────────
// Self — Identity Bootstrap
// ────────────────────────────────────────────────────────────────

/// This crate's own OmniCode pragma identity (P1-P5).
///
/// Go equivalent: `func Pragma() map[string]string { return _pragma }`
pub fn own_pragma() -> &'static BTreeMap<String, String> {
    &OWN_PRAGMA
}

/// This crate's own OmniCode metadata (M1-M10).
///
/// Go equivalent: `func Metadata() map[string]string { return _metadata }`
pub fn own_metadata() -> &'static BTreeMap<String, String> {
    &OWN_METADATA
}

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

/// Filter packages by exact match on a metadata key.
fn by_metadata(key: &str, value: &str) -> Vec<PackageIdentity> {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard
        .values()
        .filter(|p| p.metadata.get(key).is_some_and(|v| v == value))
        .cloned()
        .collect()
}

// ────────────────────────────────────────────────────────────────
// Own Identity (Pragma + Metadata)
// ────────────────────────────────────────────────────────────────

/// This crate's OmniCode pragma (P1-P5). Built once via LazyLock.
///
/// Go equivalent: `var _pragma = map[string]string{...}`
static OWN_PRAGMA: LazyLock<BTreeMap<String, String>> = LazyLock::new(build_pragma);

/// This crate's OmniCode metadata (M1-M10). Built once via LazyLock.
///
/// Go equivalent: `var _metadata = map[string]string{...}`
static OWN_METADATA: LazyLock<BTreeMap<String, String>> = LazyLock::new(build_metadata);

fn build_pragma() -> BTreeMap<String, String> {
    BTreeMap::from([
        // P1: Core Identity
        ("P1.key".into(), "B-L0-identity-registry".into()),
        ("P1.type".into(), "code".into()),
        ("P1.format".into(), "rust".into()),
        ("P1.style".into(), "library".into()),
        // P2: Structure
        ("P2.blocks".into(), "4-block".into()),
        // P3: Derivation
        ("P3.from".into(), "b-word/seed/code/L0/go/library.go".into()),
        ("P3.derives".into(), "B-L0-seed-go-library".into()),
        // P4: Version
        ("P4.at".into(), "b-01.00".into()),
        // P5: Summary
        ("P5.title".into(), "Identity Registry".into()),
        (
            "P5.summary".into(),
            "Runtime package identity registration and discovery".into(),
        ),
    ])
}

fn build_metadata() -> BTreeMap<String, String> {
    BTreeMap::from([
        // ── M1: Core Identity ─────────────────────────────────────────
        ("M1.key".into(), "B-L0-identity-registry".into()),
        ("M1.component_type".into(), "Ladder".into()),
        ("M1.architect".into(), "Seanje Lenox-Wise".into()),
        ("M1.implementation".into(), "Nova Dawn".into()),
        ("M1.created".into(), "2026-02-13".into()),
        // ── M2: Version History ───────────────────────────────────────
        (
            "M2.b-01.00".into(),
            "2026-02-13 — Initial creation: registry, query, self-awareness layer".into(),
        ),
        // ── M3: Interface ─────────────────────────────────────────────
        (
            "M3.requires.stdlib".into(),
            "collections (BTreeMap), sync (RwLock, LazyLock)".into(),
        ),
        ("M3.requires.external".into(), "none".into()),
        (
            "M3.requires.internal".into(),
            "none — L0 foundation, no internal dependencies".into(),
        ),
        (
            "M3.used_by".into(),
            "L0-universal/hybrid/config, L1-omnicode/hybrid/pragma, L3-cpisi".into(),
        ),
        ("M3.import".into(), "bereshit-l0-identity".into()),
        (
            "M3.pattern".into(),
            "identity::register(key, pragma, metadata) → identity::lookup(key)".into(),
        ),
        // ── M4: Public API ────────────────────────────────────────────
        ("M4.register".into(), "register".into()),
        ("M4.lookup".into(), "lookup, all".into()),
        ("M4.query".into(), "by_layer, by_domain, by_tag".into()),
        ("M4.self".into(), "own_pragma, own_metadata".into()),
        // ── M5: Operational ───────────────────────────────────────────
        (
            "M5.blocking".into(),
            "no — in-memory map operations only".into(),
        ),
        (
            "M5.health".into(),
            "Provider | granted: registered | deferred: init pending | denied: n/a".into(),
        ),
        // ── M6: Classification ────────────────────────────────────────
        (
            "M6.tags".into(),
            "identity, registry, discovery, self-awareness, cpi-si".into(),
        ),
        ("M6.category".into(), "Ladder".into()),
        ("M6.domain".into(), "identity".into()),
        ("M6.layer".into(), "L0-universal".into()),
        ("M6.paradigm".into(), "CPI-SI".into()),
        // ── M7: Intent ────────────────────────────────────────────────
        (
            "M7.purpose".into(),
            "Enable runtime self-awareness — the system knows its own components".into(),
        ),
        (
            "M7.philosophy".into(),
            "Identity is code, not documentation. Registration is declaration.".into(),
        ),
        (
            "M7.provides".into(),
            "Package registration, key-based lookup, layer/domain/tag queries".into(),
        ),
        // ── M8: Grounding ─────────────────────────────────────────────
        (
            "M8.scripture".into(),
            "Genesis 1:5 — And God called the light Day, and the darkness he called Night".into(),
        ),
        (
            "M8.principle".into(),
            "Naming establishes identity. Registration declares existence.".into(),
        ),
        (
            "M8.anchor".into(),
            "Genesis 1:1 — In the beginning God created the heaven and the earth".into(),
        ),
        // ── M9: Dependencies ──────────────────────────────────────────
        (
            "M9.needs.stdlib".into(),
            "collections (BTreeMap), sync (RwLock, LazyLock)".into(),
        ),
        ("M9.needs.external".into(), "none".into()),
        (
            "M9.needs.internal".into(),
            "none — zero internal dependencies, L0 foundation".into(),
        ),
        (
            "M9.used_by".into(),
            "every OmniCode Rust crate with pragma/metadata".into(),
        ),
        (
            "M9.layer_deps".into(),
            "L0 only — no L1+ dependencies. Must stay dependency-free.".into(),
        ),
        // ── M10: Roadmap ──────────────────────────────────────────────
        (
            "M10.current".into(),
            "b-01.00 — Core registry with register, lookup, query functions".into(),
        ),
        (
            "M10.planned".into(),
            "Dependency graph from M9 data, health aggregation, dashboard feed".into(),
        ),
        (
            "M10.limitations".into(),
            "In-memory only, no persistence, no cross-process discovery".into(),
        ),
    ])
}

// ────────────────────────────────────────────────────────────────
// Test-only utilities
// ────────────────────────────────────────────────────────────────

/// Reset registry to only the self-registered entry.
///
/// Test-only. The registry is append-only in production — identity
/// doesn't un-register. But tests need deterministic state.
#[cfg(test)]
fn reset_registry() {
    let mut guard = REGISTRY.write().unwrap_or_else(|e| e.into_inner());
    guard.clear();

    // Re-register self — the bootstrap anchor must always be present.
    let key = OWN_PRAGMA["P1.key"].clone();
    guard.insert(
        key.clone(),
        PackageIdentity {
            key,
            pragma: (*OWN_PRAGMA).clone(),
            metadata: (*OWN_METADATA).clone(),
        },
    );
}

// ════════════════════════════════════════════════════════════════
// Tests
// ════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    /// Full lifecycle test — sequential to avoid global state races.
    ///
    /// Tests: self-registration, register, lookup, all, count,
    /// by_layer, by_domain, by_tag, duplicate overwrite, clone isolation,
    /// own_pragma/own_metadata accessors.
    #[test]
    fn test_registry_full_lifecycle() {
        // ── Phase 1: Self-registration ────────────────────────────
        // First access triggers LazyLock init → self-registration.
        reset_registry();

        let self_id = lookup("B-L0-identity-registry");
        assert!(self_id.is_some(), "identity crate should self-register");

        let self_id = self_id.unwrap();
        assert_eq!(self_id.key, "B-L0-identity-registry");
        assert_eq!(self_id.pragma["P1.format"], "rust");
        assert_eq!(self_id.metadata["M6.layer"], "L0-universal");
        assert_eq!(self_id.metadata["M6.domain"], "identity");

        // Count starts at 1 (self-registered)
        assert_eq!(count(), 1);

        // ── Phase 2: Register a test package ──────────────────────
        let pragma = BTreeMap::from([
            ("P1.key".into(), "test-pkg-alpha".into()),
            ("P1.format".into(), "go".into()),
        ]);
        let metadata = BTreeMap::from([
            ("M6.layer".into(), "L1-omnicode".into()),
            ("M6.domain".into(), "testing".into()),
            ("M6.tags".into(), "test, example, demo".into()),
        ]);
        register("test-pkg-alpha", pragma, metadata);

        assert_eq!(count(), 2);

        // ── Phase 3: Lookup ───────────────────────────────────────
        let pkg = lookup("test-pkg-alpha").expect("should find test-pkg-alpha");
        assert_eq!(pkg.key, "test-pkg-alpha");
        assert_eq!(pkg.pragma["P1.format"], "go");
        assert_eq!(pkg.metadata["M6.domain"], "testing");

        // Unknown key returns None
        assert!(lookup("nonexistent-key").is_none());

        // ── Phase 4: All ──────────────────────────────────────────
        let all_pkgs = all();
        assert_eq!(all_pkgs.len(), 2);

        // BTreeMap guarantees sorted order by key
        assert_eq!(all_pkgs[0].key, "B-L0-identity-registry");
        assert_eq!(all_pkgs[1].key, "test-pkg-alpha");

        // ── Phase 5: Register more for query testing ──────────────
        register(
            "test-pkg-beta",
            BTreeMap::from([("P1.key".into(), "test-pkg-beta".into())]),
            BTreeMap::from([
                ("M6.layer".into(), "L0-universal".into()),
                ("M6.domain".into(), "math".into()),
                ("M6.tags".into(), "foundation, math".into()),
            ]),
        );
        register(
            "test-pkg-gamma",
            BTreeMap::from([("P1.key".into(), "test-pkg-gamma".into())]),
            BTreeMap::from([
                ("M6.layer".into(), "L1-omnicode".into()),
                ("M6.domain".into(), "testing".into()),
                ("M6.tags".into(), "test, advanced".into()),
            ]),
        );

        assert_eq!(count(), 4);

        // ── Phase 6: ByLayer ──────────────────────────────────────
        let l0 = by_layer("L0-universal");
        assert_eq!(l0.len(), 2); // self + beta

        let l1 = by_layer("L1-omnicode");
        assert_eq!(l1.len(), 2); // alpha + gamma

        let empty = by_layer("L5-applications");
        assert!(empty.is_empty());

        // ── Phase 7: ByDomain ─────────────────────────────────────
        let identity_pkgs = by_domain("identity");
        assert_eq!(identity_pkgs.len(), 1); // self only

        let testing_pkgs = by_domain("testing");
        assert_eq!(testing_pkgs.len(), 2); // alpha + gamma

        let math_pkgs = by_domain("math");
        assert_eq!(math_pkgs.len(), 1); // beta

        // ── Phase 8: ByTag ────────────────────────────────────────
        let test_tagged = by_tag("test");
        assert_eq!(test_tagged.len(), 2); // alpha + gamma

        let foundation_tagged = by_tag("foundation");
        assert_eq!(foundation_tagged.len(), 1); // beta

        let demo_tagged = by_tag("demo");
        assert_eq!(demo_tagged.len(), 1); // alpha

        // Whitespace trimming
        let trimmed = by_tag("  test  ");
        assert_eq!(trimmed.len(), 2); // same as "test"

        // Self's tags
        let self_tagged = by_tag("self-awareness");
        assert_eq!(self_tagged.len(), 1); // identity crate itself

        // No matches
        let empty_tags = by_tag("nonexistent-tag");
        assert!(empty_tags.is_empty());

        // ── Phase 9: Duplicate overwrite ──────────────────────────
        let updated_pragma = BTreeMap::from([
            ("P1.key".into(), "test-pkg-alpha".into()),
            ("P1.format".into(), "rust".into()), // changed from "go"
        ]);
        register("test-pkg-alpha", updated_pragma, BTreeMap::new());

        let updated = lookup("test-pkg-alpha").unwrap();
        assert_eq!(updated.pragma["P1.format"], "rust"); // overwritten
        assert!(updated.metadata.is_empty()); // replaced entirely

        assert_eq!(count(), 4); // still 4, not 5

        // ── Phase 10: Clone isolation ─────────────────────────────
        let mut retrieved = lookup("B-L0-identity-registry").unwrap();
        retrieved.key = "mutated-by-caller".to_owned();

        let original = lookup("B-L0-identity-registry").unwrap();
        assert_eq!(original.key, "B-L0-identity-registry"); // unmutated
    }

    /// Own identity accessor tests — read-only, safe in parallel.
    #[test]
    fn test_own_identity_accessors() {
        let p = own_pragma();
        assert_eq!(p["P1.key"], "B-L0-identity-registry");
        assert_eq!(p["P1.format"], "rust");
        assert_eq!(p["P1.style"], "library");
        assert_eq!(p["P5.title"], "Identity Registry");

        let m = own_metadata();
        assert_eq!(m["M1.key"], "B-L0-identity-registry");
        assert_eq!(m["M6.layer"], "L0-universal");
        assert_eq!(m["M6.domain"], "identity");
        assert_eq!(m["M6.paradigm"], "CPI-SI");
        assert_eq!(
            m["M8.anchor"],
            "Genesis 1:1 — In the beginning God created the heaven and the earth"
        );
    }

    /// Pragma and metadata are complete — all P1-P5 and M1-M10 sections present.
    #[test]
    fn test_identity_completeness() {
        let p = own_pragma();
        // All P sections present
        assert!(p.contains_key("P1.key"));
        assert!(p.contains_key("P2.blocks"));
        assert!(p.contains_key("P3.from"));
        assert!(p.contains_key("P4.at"));
        assert!(p.contains_key("P5.title"));

        let m = own_metadata();
        // All M sections present
        for prefix in [
            "M1.", "M2.", "M3.", "M4.", "M5.", "M6.", "M7.", "M8.", "M9.", "M10.",
        ] {
            assert!(
                m.keys().any(|k| k.starts_with(prefix)),
                "missing metadata section {prefix}"
            );
        }
    }
}
