//omni:code --rust -library
//omni:key B-L0-identity-registry
//omni:version b-02.00
//omni:pragma PRAGMA
//omni:metadata METADATA
//!
//! # OmniCode Identity Registry
//!
//! Runtime self-awareness for CPI-SI — every crate with pragma and metadata
//! registers itself, making the system self-aware at runtime.
//!
//! Two access paths:
//!
//!   **Static:**  `grep -rn "//omni:key"` — finds all file identities
//!   **Runtime:** `identity::lookup("B-L0-config-loader")` — pragma, metadata
//!
//! ## Design
//!
//! Pure std — no external dependencies. Identity carried as compile-time
//! static data via [`PRAGMA`] (I1-I4) and [`METADATA`] (C1-C7). Registry
//! state protected by `RwLock` for concurrent access. Secondary indexes
//! use `BTreeSet` for O(log n) layer/domain queries — the data structure
//! IS the algorithm.
//!
//! ## Ladder
//!
//! `register → lookup / all / by_layer / by_domain / by_tag`
//!
//! ## Baton
//!
//! `LazyLock init → register(key, &PRAGMA, &METADATA) → registry stores identity`
//!
//! ## OmniCode Identity
//!
//! | Static | Sections | Purpose | Query |
//! |--------|----------|---------|-------|
//! | [`PRAGMA`] | I1-I4 | Identity — who this file is | [`pragma_get`] |
//! | [`METADATA`] | C1-C7 | Context — when, where, why, how | [`metadata_get`] |

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-identity-registry
// Purpose: Runtime package identity registration and discovery
// Biblical: Genesis 1:5 — And God called the light Day, and the darkness
//           he called Night
// Version: b-02.00

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for this crate.
///
/// Carries the I-sections (Identity) of the OmniCode metadata standard.
/// These fields establish WHO this file is in the project graph.
/// Zero-cost at runtime — compiled into the binary as static data.
///
/// # Sections
///
/// | Section | Purpose | Required | Defined |
/// |---------|---------|----------|---------|
/// | **I1** | Core | `key`, `format`, `from`, `at` | — |
/// | **I2** | Family | `type`, `structure` | `subtype`, `role` |
/// | **I3** | Instance | `file`, `title` | `component`, `path`, `provides`, `brief` |
/// | **I4** | Architecture | — | `layer`, `position`, `pattern` |
///
/// # Example
///
/// ```rust
/// let key = bereshit_l0_identity::pragma_get("I1.key").unwrap_or("unknown");
/// assert_eq!(key, "B-L0-identity-registry");
/// ```
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-identity-registry"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/library.rs"),
    ("I1.at",        "b-02.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I2.role",      "foundation"),
    // I3: Instance
    ("I3.file",      "lib.rs"),
    ("I3.title",     "Identity Registry"),
    ("I3.component", "b-spiral/identity"),
    ("I3.path",      "L0-universal/b-spiral/identity/src/lib.rs"),
    ("I3.provides",  "IDENTITY_REGISTRY"),
    ("I3.brief",     "Runtime package identity registration and discovery"),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "spiral/identity"),
    ("I4.pattern",   "LazyLock init → identity::register(key, &PRAGMA, &METADATA) → identity::lookup(key)"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context for this crate.
///
/// Carries the C-sections (Context) of the OmniCode metadata standard.
/// These fields establish WHEN, WHERE, WHY, and HOW for this file.
/// Zero-cost at runtime — compiled into the binary as static data.
///
/// # Sections
///
/// | Section | Purpose | Required | Defined |
/// |---------|---------|----------|---------|
/// | **C1** | State | `version`, `status` | `created`, `updated` |
/// | **C2** | Attribution | `organization` | `architect`, `implementation`, `copyright` |
/// | **C3** | Grounding | `scripture` | `principle`, `anchor` |
/// | **C4** | Dependencies | `requires`, `consumers` | `integration`, `if_missing` |
/// | **C5** | Intent | — | `purpose`, `philosophy` |
/// | **C6** | Roadmap | — | `current`, `planned`, `limitations` |
/// | **C7** | Classification | — | `tags`, `category`, `domain`, `paradigm` |
///
/// # Example
///
/// ```rust
/// let version = bereshit_l0_identity::metadata_get("C1.version").unwrap_or("unknown");
/// assert_eq!(version, "b-02.00");
/// ```
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-02.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-13"),
    ("C1.updated",           "2026-02-18"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Genesis 1:5 — And God called the light Day, and the darkness he called Night"),
    ("C3.principle",         "Naming establishes identity. Registration declares existence."),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "collections (BTreeMap, BTreeSet), fmt, sync (RwLock, LazyLock)"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none — L0 foundation, no internal dependencies"),
    ("C4.consumers",         "every OmniCode Rust crate with PRAGMA/METADATA statics"),
    ("C4.integration",       "use bereshit_l0_identity; identity::register(key, &PRAGMA, &METADATA)"),
    ("C4.if_missing",        "no runtime self-awareness — crates can't discover identity"),
    // C5: Intent
    ("C5.purpose",           "Enable runtime self-awareness — the system knows its own components"),
    ("C5.philosophy",        "Identity is code, not documentation. Registration is declaration."),
    // C6: Roadmap
    ("C6.current",           "b-02.00 — Production structure, BTreeSet indexes, error types, type methods, tests"),
    ("C6.planned",           "Dependency graph from C4 data, health aggregation, dashboard feed"),
    ("C6.limitations",       "In-memory only, no persistence, no cross-process discovery"),
    // C7: Classification
    ("C7.tags",              "identity, registry, discovery, self-awareness, cpi-si"),
    ("C7.category",          "Ladder"),
    ("C7.domain",            "identity"),
    ("C7.paradigm",          "CPI-SI"),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// SETUP makes things EXIST. BODY makes things HAPPEN.
//
// Everything the BODY needs — types, constants, imports, error definitions —
// is declared here. The BODY contains only functions and impl blocks that
// operate on what SETUP established.
//
// If anything must be hardcoded, it lives here — never scattered through BODY.
// When a hardcoded value gets promoted to config, you extract from one place.
//
// Section order (dependency chain — each layer uses only what's above):
//
//   [S01] imports         — What this file depends on
//   [S02] modules         — Submodule tree + re-exports (N/A)
//   [S03] constants       — Compile-time fixed values (N/A)
//   [S04] statics         — Runtime-initialized fixed values (registry)
//   [S05] type-aliases    — Shorthand for complex signatures (N/A)
//   [S06] error-types     — RegistryError + Display + Error impls
//   [S07] core-types      — PackageIdentity, RegistryInner, RegistryStats
//   [S08] trait-defs      — Behavioral contracts (N/A)
//   [S09] macros          — Code templates (N/A)
//   [S10] feature-gates   — Conditional compilation (N/A)

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────
//
// Order: Standard Library → External Crates → Internal Crates → Crate-Internal
// Pure std at L0 — no external or internal dependencies.

// --- Standard Library ---
use std::collections::{BTreeMap, BTreeSet};
use std::fmt;
use std::sync::{LazyLock, RwLock};

// --- External Crates ---
// [Pure library — no external dependencies at L0]

// --- Internal Crates ---
// [Self-contained at L0 — no internal dependencies]

// --- Crate-Internal ---
// [Single-file crate — no sub-imports]

// ──────────────────────────────────────────────────────────────────────────
// 4. Statics
// ──────────────────────────────────────────────────────────────────────────
//
// NOTE: PRAGMA and METADATA identity statics live in the METADATA block.
//
// Global identity registry — the system knowing itself.
// Initialized with self-registration on first access (LazyLock).
// Uses RwLock for concurrent read access with exclusive writes.
//
// Go equivalent: var mu sync.RWMutex + var packages = make(map[string]*PackageIdentity)
// Rust improvement: RegistryInner holds packages + indexes in one struct,
// under one lock — no separate mutable statics.

static REGISTRY: LazyLock<RwLock<RegistryInner>> = LazyLock::new(|| {
    let mut inner = RegistryInner::new();

    // Self-registration — the bootstrap anchor (Genesis 1:1).
    // Go does this in init(). LazyLock achieves the same: exactly once, before
    // any access. The registry knowing itself IS the foundation.
    let key = pragma_get("I1.key").unwrap_or("unknown").to_string();
    inner.insert(PackageIdentity {
        key: key.clone(),
        pragma: slice_to_map(PRAGMA),
        metadata: slice_to_map(METADATA),
    });

    RwLock::new(inner)
});

// ──────────────────────────────────────────────────────────────────────────
// 6. Error Types
// ──────────────────────────────────────────────────────────────────────────
//
// Errors are part of the API contract — they define how failure looks.
// Two error conditions: empty key (caller bug) and missing key (query miss).
//
// Go uses sentinel errors (errors.New). Rust uses an enum — the compiler
// ensures all variants are handled. This is where Rust shines: exhaustive
// matching means you can't forget an error case.

/// Errors that can occur during registry operations.
///
/// Two variants matching Go's `ErrEmptyKey` and `ErrNotRegistered` sentinels.
/// Rust's enum is stronger — the compiler enforces exhaustive matching.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum RegistryError {
    /// Returned when `register_err` is called with an empty key.
    EmptyKey,
    /// Returned when a lookup key doesn't exist in the registry.
    NotRegistered(String),
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

/// A registered package's OmniCode identity.
///
/// Pragma carries file-level identity (I1-I4), stored as `BTreeMap` for
/// O(log n) lookup by field name. Metadata carries full context (C1-C7).
///
/// Created by [`register`] which converts `&[(&str, &str)]` slices to maps.
/// Thread-safe after registration — fields are never mutated post-register.
///
/// Go equivalent: `type PackageIdentity struct { Key, Pragma, Metadata }`.
/// Rust improvement: `Clone` returns an owned copy (ownership transfer
/// replaces Go's `copyMap()`), `Display` gives structured formatting.
#[derive(Debug, Clone)]
pub struct PackageIdentity {
    /// OmniCode key (I1.key value).
    pub key: String,
    /// I1-I4 pragma fields (converted from `&[(&str, &str)]` at registration).
    pub pragma: BTreeMap<String, String>,
    /// C1-C7 metadata fields (converted from `&[(&str, &str)]` at registration).
    pub metadata: BTreeMap<String, String>,
}

/// Point-in-time snapshot of registry state.
///
/// Used by health systems and dashboards for topology awareness.
/// `BTreeMap` gives sorted layer output (L0, L1, L2...) — deterministic
/// ordering that Go's `map[string]int` cannot guarantee.
#[derive(Debug, Clone)]
pub struct RegistryStats {
    /// Number of registered packages.
    pub total: usize,
    /// Count per layer (L0, L1, etc.), sorted by layer name.
    pub by_layer: BTreeMap<String, usize>,
}

/// Internal registry state — packages + secondary indexes.
///
/// All state under one `RwLock` — no separate mutable statics.
/// Secondary indexes use `BTreeSet<String>` (sorted unique keys) for
/// O(log n) insert/remove/contains. The data structure IS the algorithm —
/// Go needed manual `slices.BinarySearchFunc` in sorted `[]*PackageIdentity`;
/// Rust just uses the right collection type.
struct RegistryInner {
    /// Primary storage: OmniCode key → identity.
    packages: BTreeMap<String, PackageIdentity>,
    /// Secondary index: I4.layer value → set of OmniCode keys.
    layer_index: BTreeMap<String, BTreeSet<String>>,
    /// Secondary index: C7.domain value → set of OmniCode keys.
    domain_index: BTreeMap<String, BTreeSet<String>>,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// BODY makes things HAPPEN. SETUP made things EXIST.
//
// Section order follows the type lifecycle — from identity through creation,
// operation, observation, to output. Not all 7 subsections required.
//
//   1. Identity Access       — pragma(), pragma_get(), metadata(), metadata_get()
//   2. Trait Implementations — Display, Error for RegistryError + PackageIdentity
//   4. Core Logic            — RegistryInner methods, register, register_err
//   5. Queries               — lookup, all, count, by_layer, by_domain, by_tag, stats
//   7. Free Functions        — by_metadata (private helper)
//
// ──────────────────────────────────────────────────────────────────────────
// Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Public APIs (Top Rungs — Orchestration)
// ├── lookup()      → reads packages (Option pattern)
// ├── lookup_err()  → reads packages (Result pattern)
// ├── all()         → reads packages (BTreeMap = sorted by key)
// ├── count()       → reads packages
// ├── by_layer()    → reads layer_index + packages (O(log n) via BTreeSet)
// ├── by_domain()   → reads domain_index + packages (O(log n) via BTreeSet)
// ├── by_tag()      → reads packages, splits tags
// └── stats()       → reads layer_index (snapshot)
//
// Core Operations (Middle Rungs — Business Logic)
// ├── register()     → calls register_err() (fire-and-forget)
// └── register_err() → uses RegistryInner::insert()
//
// RegistryInner Methods
// ├── new()                 → empty registry
// ├── insert()              → stores + calls update_indexes()
// ├── update_indexes()      → maintains BTreeSet indexes (O(log n))
// ├── insert_into_index()   → BTreeSet::insert (O(log n))
// └── remove_from_index()   → BTreeSet::remove (O(log n))
//
// Helpers
// ├── slice_to_map()  → pure function (&[(&str, &str)] → BTreeMap)
// └── by_metadata()   → filter by key/value (future queries)
//
// Self (Bootstrap)
// LazyLock init  → insert(own identity) on first access
// pragma()       → returns &PRAGMA slice
// pragma_get()   → key lookup in PRAGMA
// metadata()     → returns &METADATA slice
// metadata_get() → key lookup in METADATA
//
// APUs (Available Processing Units):
//   - 20 functions total
//   - 5 RegistryInner methods (new, insert, update_indexes, insert_into, remove_from)
//   - 2 helpers (slice_to_map, by_metadata)
//   - 2 core operations (register, register_err)
//   - 11 public APIs (lookup, lookup_err, all, count, by_layer, by_domain,
//     by_tag, stats, pragma, pragma_get, metadata, metadata_get
//     + PackageIdentity::get)

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────
//
// OmniCode static accessor functions for PRAGMA and METADATA.
// Every library crate exposes these four functions — they are the crate's
// self-awareness, providing runtime access to compile-time identity data.
//
// Pattern: each pair is (full_slice, key_lookup).
//   Full slice:  returns &[(&str, &str)] for iteration
//   Key lookup:  returns Option<&str> for targeted access

/// Returns this crate's OmniCode pragma identity (I1-I4).
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    PRAGMA
}

/// Looks up a specific pragma key. Returns `None` if not found.
///
/// ```
/// assert_eq!(bereshit_l0_identity::pragma_get("I1.key"), Some("B-L0-identity-registry"));
/// assert_eq!(bereshit_l0_identity::pragma_get("I1.format"), Some("rust"));
/// assert_eq!(bereshit_l0_identity::pragma_get("I99.nope"), None);
/// ```
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns this crate's OmniCode metadata (C1-C7).
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    METADATA
}

/// Looks up a specific metadata key. Returns `None` if not found.
///
/// ```
/// assert_eq!(bereshit_l0_identity::metadata_get("C1.version"), Some("b-02.00"));
/// assert_eq!(bereshit_l0_identity::metadata_get("C99.nope"), None);
/// ```
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ──────────────────────────────────────────────────────────────────────────
// 2. Trait Implementations
// ──────────────────────────────────────────────────────────────────────────
//
// Impl blocks for types declared in SETUP. SETUP makes things EXIST;
// BODY makes things HAPPEN. Display, Error — these make the types do work.

impl fmt::Display for RegistryError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::EmptyKey => write!(f, "identity: empty key"),
            Self::NotRegistered(key) => write!(f, "identity: key not registered: {key}"),
        }
    }
}

impl std::error::Error for RegistryError {}

impl fmt::Display for PackageIdentity {
    /// Format: `"key (subtype @ layer)"` e.g. `"B-L0-identity-registry (library @ L0)"`.
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let subtype = self.pragma.get("I2.subtype").map(|s| s.as_str()).unwrap_or("?");
        let layer = self.pragma.get("I4.layer").map(|s| s.as_str()).unwrap_or("?");
        write!(f, "{} ({} @ {})", self.key, subtype, layer)
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

// --- RegistryInner Methods ---

impl RegistryInner {
    /// Creates an empty registry with no entries or indexes.
    fn new() -> Self {
        Self {
            packages: BTreeMap::new(),
            layer_index: BTreeMap::new(),
            domain_index: BTreeMap::new(),
        }
    }

    /// Inserts a package identity, updating secondary indexes.
    ///
    /// If the key already exists, the old entry is removed from indexes
    /// before the new entry is inserted (handles layer/domain changes on
    /// re-registration).
    fn insert(&mut self, identity: PackageIdentity) {
        let key = identity.key.clone();
        let old = self.packages.get(&key).cloned();
        self.packages.insert(key.clone(), identity);
        self.update_indexes(&key, old.as_ref());
    }

    /// Incrementally updates layer and domain indexes for a single registration.
    ///
    /// O(log n) — `BTreeSet::insert` and `BTreeSet::remove` are O(log n).
    /// The data structure IS the algorithm: Go needed manual
    /// `slices.BinarySearchFunc` in sorted slices; Rust's `BTreeSet` provides
    /// sorted, unique, O(log n) operations as its fundamental behavior.
    ///
    /// The principle: ln(n) operations, not n. Foundation matters most —
    /// logarithmic descent toward the anchor position. dH/dn = k/n.
    fn update_indexes(&mut self, key: &str, old: Option<&PackageIdentity>) {
        // Remove old entry from indexes if re-registering
        if let Some(old) = old {
            Self::remove_from_index(
                &mut self.layer_index,
                old.pragma.get("I4.layer"),
                key,
            );
            Self::remove_from_index(
                &mut self.domain_index,
                old.metadata.get("C7.domain"),
                key,
            );
        }

        // Insert new entry at sorted position
        if let Some(p) = self.packages.get(key) {
            Self::insert_into_index(
                &mut self.layer_index,
                p.pragma.get("I4.layer"),
                key,
            );
            Self::insert_into_index(
                &mut self.domain_index,
                p.metadata.get("C7.domain"),
                key,
            );
        }
    }

    /// Inserts a package key into a secondary index bucket.
    /// O(log n) — `BTreeSet::insert` maintains sorted order.
    fn insert_into_index(
        index: &mut BTreeMap<String, BTreeSet<String>>,
        bucket_key: Option<&String>,
        pkg_key: &str,
    ) {
        if let Some(bk) = bucket_key {
            if !bk.is_empty() {
                index
                    .entry(bk.clone())
                    .or_default()
                    .insert(pkg_key.to_string());
            }
        }
    }

    /// Removes a package key from a secondary index bucket.
    /// O(log n) — `BTreeSet::remove` maintains sorted order.
    /// Cleans up empty buckets to prevent stale index entries.
    fn remove_from_index(
        index: &mut BTreeMap<String, BTreeSet<String>>,
        bucket_key: Option<&String>,
        pkg_key: &str,
    ) {
        if let Some(bk) = bucket_key {
            if let Some(set) = index.get_mut(bk.as_str()) {
                set.remove(pkg_key);
                if set.is_empty() {
                    index.remove(bk.as_str());
                }
            }
        }
    }
}

// --- Pure Helpers ---

/// Converts an identity slice to a BTreeMap for O(log n) key lookup.
///
/// Used by registration to store identity data in the registry.
/// Go equivalent: `sliceToMap([][2]string) map[string]string`.
/// Rust: iterators + `.collect()` into BTreeMap is idiomatic and clean.
fn slice_to_map(pairs: &[(&str, &str)]) -> BTreeMap<String, String> {
    pairs
        .iter()
        .map(|(k, v)| (k.to_string(), v.to_string()))
        .collect()
}

// --- Registration ---

/// Register a crate's identity in the global registry.
///
/// Called from each OmniCode Rust crate during initialization:
///
/// ```rust,ignore
/// bereshit_l0_identity::register(
///     pragma_get("I1.key").unwrap_or("unknown"),
///     &PRAGMA,
///     &METADATA,
/// );
/// ```
///
/// Empty keys are silently ignored. For error handling, use [`register_err`].
/// Duplicate keys overwrite silently (last writer wins).
///
/// API takes `&[(&str, &str)]` — pass your `PRAGMA`/`METADATA` statics
/// directly. The registry converts to `BTreeMap` for O(log n) runtime queries.
/// Ownership transfer replaces Go's `copyMap()` — the caller passes references,
/// the registry owns the copies.
pub fn register(key: &str, pragma: &[(&str, &str)], metadata: &[(&str, &str)]) {
    let _ = register_err(key, pragma, metadata);
}

/// Register a crate's identity and return any validation error.
///
/// Returns [`RegistryError::EmptyKey`] if key is empty.
/// Slices are converted to `BTreeMap` for O(log n) runtime queries.
/// Secondary indexes are incrementally updated via `BTreeSet` — O(log n).
pub fn register_err(
    key: &str,
    pragma: &[(&str, &str)],
    metadata: &[(&str, &str)],
) -> Result<(), RegistryError> {
    if key.is_empty() {
        return Err(RegistryError::EmptyKey);
    }

    let identity = PackageIdentity {
        key: key.to_owned(),
        pragma: slice_to_map(pragma),
        metadata: slice_to_map(metadata),
    };

    let mut guard = REGISTRY.write().unwrap_or_else(|e| e.into_inner());
    guard.insert(identity);
    Ok(())
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Queries & Accessors
// ──────────────────────────────────────────────────────────────────────────

// --- PackageIdentity Methods ---

impl PackageIdentity {
    /// Unified field lookup across both Pragma and Metadata.
    ///
    /// Checks Pragma first (I-prefixed), then Metadata (C-prefixed).
    /// Returns `None` if not found in either.
    ///
    /// Go equivalent: `func (p *PackageIdentity) Get(key string) string`.
    /// Rust improvement: `Option<&str>` makes absence explicit (Go returns "").
    pub fn get(&self, key: &str) -> Option<&str> {
        self.pragma
            .get(key)
            .or_else(|| self.metadata.get(key))
            .map(|s| s.as_str())
    }
}

// --- Discovery ---

/// Look up a package by its OmniCode key.
///
/// Returns a clone of the registered identity, or `None` if not found.
/// Go returns `(*PackageIdentity, bool)`; Rust uses `Option` — the compiler
/// ensures the caller handles absence.
pub fn lookup(key: &str) -> Option<PackageIdentity> {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard.packages.get(key).cloned()
}

/// Look up a package by key or return [`RegistryError::NotRegistered`].
///
/// Use when you need `Result`-based error handling instead of `Option`.
/// Go equivalent: `LookupErr(key) (*PackageIdentity, error)`.
pub fn lookup_err(key: &str) -> Result<PackageIdentity, RegistryError> {
    lookup(key).ok_or_else(|| RegistryError::NotRegistered(key.to_owned()))
}

/// Return every registered package.
///
/// Order is by key (`BTreeMap` = sorted). Go returns unordered map iteration;
/// Rust's `BTreeMap` gives deterministic order — an improvement.
pub fn all() -> Vec<PackageIdentity> {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard.packages.values().cloned().collect()
}

/// Return the number of registered packages.
pub fn count() -> usize {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard.packages.len()
}

// --- Query ---

/// Return all packages at a given layer via the layer index.
///
/// O(log n) lookup per result — `BTreeSet` index locates keys, then
/// `BTreeMap` primary storage retrieves full identities.
///
/// Go equivalent: `ByLayer(layer) []*PackageIdentity` (O(1) via direct slice).
/// Rust: O(k log n) where k = results — slightly more work per result,
/// but no dangling reference concerns and the code is cleaner.
///
/// Example: `identity::by_layer("L0")`
pub fn by_layer(layer: &str) -> Vec<PackageIdentity> {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard
        .layer_index
        .get(layer)
        .map(|keys| {
            keys.iter()
                .filter_map(|k| guard.packages.get(k).cloned())
                .collect()
        })
        .unwrap_or_default()
}

/// Return all packages in a given domain via the domain index.
///
/// O(log n) per result via `BTreeSet` + `BTreeMap` lookup.
/// Example: `identity::by_domain("identity")`
pub fn by_domain(domain: &str) -> Vec<PackageIdentity> {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard
        .domain_index
        .get(domain)
        .map(|keys| {
            keys.iter()
                .filter_map(|k| guard.packages.get(k).cloned())
                .collect()
        })
        .unwrap_or_default()
}

/// Return all packages that have a specific tag in C7.tags.
///
/// Tags are comma-separated in the metadata value. Linear scan required
/// because tag matching involves string splitting — no index for this.
///
/// Example: `identity::by_tag("cpi-si")`
pub fn by_tag(tag: &str) -> Vec<PackageIdentity> {
    let tag = tag.trim();
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard
        .packages
        .values()
        .filter(|p| {
            p.metadata
                .get("C7.tags")
                .is_some_and(|tags| tags.split(',').any(|t| t.trim() == tag))
        })
        .cloned()
        .collect()
}

// --- Stats ---

/// Return a point-in-time snapshot of registry statistics.
///
/// Used by health systems and dashboards for topology awareness.
/// `BTreeMap` gives sorted layer output — Go's `map[string]int` is unordered.
pub fn stats() -> RegistryStats {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    RegistryStats {
        total: guard.packages.len(),
        by_layer: guard
            .layer_index
            .iter()
            .map(|(layer, keys)| (layer.clone(), keys.len()))
            .collect(),
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────

/// Filter packages by exact match on a metadata key.
///
/// Available for future ad-hoc queries (by_status, by_paradigm, etc.).
/// Not indexed — O(n) linear scan. Use `by_layer`/`by_domain` for
/// indexed queries.
#[allow(dead_code)]
fn by_metadata(key: &str, value: &str) -> Vec<PackageIdentity> {
    let guard = REGISTRY.read().unwrap_or_else(|e| e.into_inner());
    guard
        .packages
        .values()
        .filter(|p| p.metadata.get(key).is_some_and(|v| v == value))
        .cloned()
        .collect()
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// CLOSING Zones Overview
// ──────────────────────────────────────────────────────────────────────────
//
// 3 Code Zones (operations):  Cv → Ce → Cc
// 5 Doc Sections (guidance):  X1 → X2 → X3 → X4 → X5
//
// Three-tier ordering:
//   1. All code zones before any doc sections
//   2. Within code: Cv (Validation) → Ce (Execution) → Cc (Cleanup)
//   3. Within docs: X1 (Policy) → X2 (Extension) → X3 (Troubleshooting)
//                   → X4 (Reference) → X5 (Note)

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────

/// Reset registry to only the self-registered entry.
///
/// Test-only. The registry is append-only in production — identity
/// doesn't un-register. But tests need deterministic state.
#[cfg(test)]
fn reset_registry() {
    let mut guard = REGISTRY.write().unwrap_or_else(|e| e.into_inner());
    guard.packages.clear();
    guard.layer_index.clear();
    guard.domain_index.clear();

    // Re-register self — the bootstrap anchor must always be present.
    let key = pragma_get("I1.key").unwrap_or("unknown").to_string();
    let identity = PackageIdentity {
        key: key.clone(),
        pragma: slice_to_map(PRAGMA),
        metadata: slice_to_map(METADATA),
    };
    guard.insert(identity);
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::Mutex;

    /// Serialization mutex — tests share global REGISTRY state.
    ///
    /// Rust runs tests in parallel by default. Without serialization,
    /// one test's `reset_registry()` races with another test's assertions.
    /// This mutex ensures tests run one at a time.
    ///
    /// Zero external dependencies. Go's `testing.T` is inherently serial;
    /// Rust's test harness is parallel. This is the idiomatic std-only fix.
    static TEST_MUTEX: LazyLock<Mutex<()>> = LazyLock::new(|| Mutex::new(()));

    /// Full lifecycle test — sequential to avoid global state races.
    ///
    /// Tests: self-registration, register, lookup, all, count,
    /// by_layer, by_domain, by_tag, duplicate overwrite, clone isolation,
    /// error types, display formatting, unified get, stats.
    #[test]
    fn test_registry_full_lifecycle() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());

        // ── Phase 1: Self-registration ────────────────────────────
        // First access triggers LazyLock init → self-registration.
        reset_registry();

        let self_id = lookup("B-L0-identity-registry");
        assert!(self_id.is_some(), "identity crate should self-register");

        let self_id = self_id.unwrap();
        assert_eq!(self_id.key, "B-L0-identity-registry");
        assert_eq!(self_id.pragma["I1.format"], "rust");
        assert_eq!(self_id.metadata["C7.domain"], "identity");
        assert_eq!(self_id.pragma["I4.layer"], "L0");

        // Count starts at 1 (self-registered)
        assert_eq!(count(), 1);

        // ── Phase 2: Register a test package ──────────────────────
        let pragma: &[(&str, &str)] = &[
            ("I1.key", "test-pkg-alpha"),
            ("I1.format", "go"),
            ("I2.subtype", "library"),
            ("I4.layer", "L1"),
        ];
        let metadata: &[(&str, &str)] = &[
            ("C7.domain", "testing"),
            ("C7.tags", "test, example, demo"),
        ];
        register("test-pkg-alpha", pragma, metadata);

        assert_eq!(count(), 2);

        // ── Phase 3: Lookup ───────────────────────────────────────
        let pkg = lookup("test-pkg-alpha").expect("should find test-pkg-alpha");
        assert_eq!(pkg.key, "test-pkg-alpha");
        assert_eq!(pkg.pragma["I1.format"], "go");
        assert_eq!(pkg.metadata["C7.domain"], "testing");

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
            &[("I1.key", "test-pkg-beta"), ("I4.layer", "L0")],
            &[
                ("C7.domain", "math"),
                ("C7.tags", "foundation, math"),
            ],
        );
        register(
            "test-pkg-gamma",
            &[("I1.key", "test-pkg-gamma"), ("I4.layer", "L1")],
            &[
                ("C7.domain", "testing"),
                ("C7.tags", "test, advanced"),
            ],
        );

        assert_eq!(count(), 4);

        // ── Phase 6: ByLayer (O(log n) via BTreeSet index) ───────
        let l0 = by_layer("L0");
        assert_eq!(l0.len(), 2); // self + beta

        let l1 = by_layer("L1");
        assert_eq!(l1.len(), 2); // alpha + gamma

        let empty = by_layer("L5");
        assert!(empty.is_empty());

        // ── Phase 7: ByDomain (O(log n) via BTreeSet index) ──────
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
        let updated_pragma: &[(&str, &str)] = &[
            ("I1.key", "test-pkg-alpha"),
            ("I1.format", "rust"), // changed from "go"
            ("I4.layer", "L1"),
        ];
        register("test-pkg-alpha", updated_pragma, &[]);

        let updated = lookup("test-pkg-alpha").unwrap();
        assert_eq!(updated.pragma["I1.format"], "rust"); // overwritten
        assert!(updated.metadata.is_empty()); // replaced entirely

        assert_eq!(count(), 4); // still 4, not 5

        // ── Phase 10: Clone isolation ─────────────────────────────
        let mut retrieved = lookup("B-L0-identity-registry").unwrap();
        retrieved.key = "mutated-by-caller".to_owned();

        let original = lookup("B-L0-identity-registry").unwrap();
        assert_eq!(original.key, "B-L0-identity-registry"); // unmutated

        // ── Phase 11: Stats ───────────────────────────────────────
        let s = stats();
        assert_eq!(s.total, 4);
        assert_eq!(s.by_layer["L0"], 2); // self + beta
        assert_eq!(s.by_layer["L1"], 2); // alpha + gamma

        // ── Phase 12: Display formatting ──────────────────────────
        let self_display = format!("{}", lookup("B-L0-identity-registry").unwrap());
        assert_eq!(self_display, "B-L0-identity-registry (library @ L0)");

        // ── Phase 13: Unified get() ───────────────────────────────
        let self_id = lookup("B-L0-identity-registry").unwrap();
        assert_eq!(self_id.get("I1.format"), Some("rust")); // pragma
        assert_eq!(self_id.get("C7.domain"), Some("identity")); // metadata
        assert_eq!(self_id.get("Z99.nope"), None); // missing
    }

    /// Own identity accessor tests — read-only, safe in parallel.
    #[test]
    fn test_identity_accessors() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());

        // Pragma accessors (I-sections)
        assert_eq!(pragma_get("I1.key"), Some("B-L0-identity-registry"));
        assert_eq!(pragma_get("I1.format"), Some("rust"));
        assert_eq!(pragma_get("I2.subtype"), Some("library"));
        assert_eq!(pragma_get("I3.title"), Some("Identity Registry"));
        assert_eq!(pragma_get("I99.nope"), None);

        // Metadata accessors (C-sections)
        assert_eq!(metadata_get("C1.version"), Some("b-02.00"));
        assert_eq!(metadata_get("C7.domain"), Some("identity"));
        assert_eq!(metadata_get("C7.paradigm"), Some("CPI-SI"));
        assert_eq!(
            metadata_get("C3.anchor"),
            Some("Genesis 1:1 — In the beginning God created the heaven and the earth")
        );
        assert_eq!(metadata_get("C99.nope"), None);

        // Full slice access
        assert!(pragma().len() >= 16); // I1(4) + I2(4) + I3(6) + I4(3)
        assert!(metadata().len() >= 20); // C1(4) + C2(4) + C3(3) + C4(5) + C5(2) + C6(3) + C7(4)
    }

    /// Pragma and metadata are complete — all I/C sections present.
    #[test]
    fn test_identity_completeness() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());

        // All I sections present in PRAGMA
        for prefix in ["I1.", "I2.", "I3.", "I4."] {
            assert!(
                PRAGMA.iter().any(|(k, _)| k.starts_with(prefix)),
                "missing pragma section {prefix}"
            );
        }

        // All C sections present in METADATA
        for prefix in ["C1.", "C2.", "C3.", "C4.", "C5.", "C6.", "C7."] {
            assert!(
                METADATA.iter().any(|(k, _)| k.starts_with(prefix)),
                "missing metadata section {prefix}"
            );
        }
    }

    /// Error type tests — RegistryError variants and Display.
    #[test]
    fn test_error_types() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());

        // EmptyKey
        let result = register_err("", &[], &[]);
        assert!(result.is_err());
        assert_eq!(result.unwrap_err(), RegistryError::EmptyKey);
        assert_eq!(
            format!("{}", RegistryError::EmptyKey),
            "identity: empty key"
        );

        // NotRegistered
        let result = lookup_err("nonexistent");
        assert!(result.is_err());
        assert_eq!(
            result.unwrap_err(),
            RegistryError::NotRegistered("nonexistent".to_string())
        );
        assert_eq!(
            format!("{}", RegistryError::NotRegistered("test-key".to_string())),
            "identity: key not registered: test-key"
        );

        // Error trait implemented
        let err: Box<dyn std::error::Error> = Box::new(RegistryError::EmptyKey);
        assert_eq!(err.to_string(), "identity: empty key");
    }

    /// Index correctness — BTreeSet indexes updated on register/overwrite.
    #[test]
    fn test_index_correctness() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        // Register with L0/identity
        register(
            "idx-test-a",
            &[("I1.key", "idx-test-a"), ("I4.layer", "L0")],
            &[("C7.domain", "math")],
        );

        assert_eq!(by_layer("L0").len(), 2); // self + idx-test-a
        assert_eq!(by_domain("math").len(), 1);

        // Re-register with DIFFERENT layer and domain
        register(
            "idx-test-a",
            &[("I1.key", "idx-test-a"), ("I4.layer", "L2")],
            &[("C7.domain", "config")],
        );

        // Old indexes cleaned up, new indexes correct
        assert_eq!(by_layer("L0").len(), 1); // self only (idx-test-a moved to L2)
        assert_eq!(by_layer("L2").len(), 1); // idx-test-a
        assert_eq!(by_domain("math").len(), 0); // cleaned up
        assert_eq!(by_domain("config").len(), 1); // idx-test-a
    }

    /// Register with missing index fields — no I4.layer or C7.domain.
    ///
    /// Packages without layer/domain should NOT appear in index queries
    /// but should still be discoverable via lookup/all.
    #[test]
    fn test_register_missing_index_fields() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        // Register with NO I4.layer and NO C7.domain
        register(
            "no-index-pkg",
            &[("I1.key", "no-index-pkg"), ("I1.format", "rust")],
            &[("C1.version", "a-01.00")],
        );

        // Discoverable via primary lookup
        let pkg = lookup("no-index-pkg");
        assert!(pkg.is_some(), "package without indexes should still exist");
        assert_eq!(pkg.unwrap().pragma["I1.format"], "rust");

        // NOT in any layer or domain index
        assert_eq!(by_layer("").len(), 0); // empty layer bucket
        assert_eq!(by_domain("").len(), 0); // empty domain bucket
        assert_eq!(count(), 2); // self + no-index-pkg
    }

    /// Register with empty pragma/metadata slices.
    ///
    /// Minimal registration — key only, no identity data beyond the key.
    #[test]
    fn test_register_empty_slices() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        register("bare-minimum", &[], &[]);

        let pkg = lookup("bare-minimum");
        assert!(pkg.is_some(), "empty-slice registration should succeed");

        let pkg = pkg.unwrap();
        assert_eq!(pkg.key, "bare-minimum");
        assert!(pkg.pragma.is_empty());
        assert!(pkg.metadata.is_empty());
    }

    /// register_err success path — returns Ok(()) on valid registration.
    #[test]
    fn test_register_err_success() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        let result = register_err(
            "err-test-ok",
            &[("I1.key", "err-test-ok"), ("I4.layer", "L0")],
            &[("C7.domain", "testing")],
        );

        assert!(result.is_ok());

        // Verify it actually registered
        let pkg = lookup("err-test-ok");
        assert!(pkg.is_some());
    }

    /// PackageIdentity::get() precedence — pragma checked before metadata.
    ///
    /// If the same key appears in both pragma and metadata, pragma wins.
    /// In practice this shouldn't happen (I-prefix vs C-prefix), but the
    /// contract should be tested.
    #[test]
    fn test_get_precedence_pragma_first() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        // Register with same key in both pragma and metadata
        register(
            "precedence-test",
            &[("shared.key", "from-pragma")],
            &[("shared.key", "from-metadata")],
        );

        let pkg = lookup("precedence-test").unwrap();
        // pragma is checked first
        assert_eq!(pkg.get("shared.key"), Some("from-pragma"));
    }

    /// by_tag with package that has no C7.tags key.
    ///
    /// Packages without tags should be silently excluded from tag queries.
    #[test]
    fn test_by_tag_no_tags_field() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        // Register package without C7.tags
        register(
            "no-tags-pkg",
            &[("I1.key", "no-tags-pkg")],
            &[("C7.domain", "testing")], // domain but no tags
        );

        // Should not appear in any tag query
        assert_eq!(by_tag("testing").len(), 0);
        assert_eq!(by_tag("").len(), 0);
    }

    /// by_tag with single tag (no comma-separated list).
    #[test]
    fn test_by_tag_single_tag() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        register(
            "single-tag-pkg",
            &[("I1.key", "single-tag-pkg")],
            &[("C7.tags", "solo")],
        );

        assert_eq!(by_tag("solo").len(), 1);
        assert_eq!(by_tag("sol").len(), 0); // partial match should NOT work
    }

    /// RegistryStats structure — layers are sorted, counts are accurate.
    #[test]
    fn test_stats_structure() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        // Register across multiple layers
        register("s-L1", &[("I4.layer", "L1")], &[]);
        register("s-L3", &[("I4.layer", "L3")], &[]);
        register("s-L1-b", &[("I4.layer", "L1")], &[]);

        let s = stats();
        assert_eq!(s.total, 4); // self + 3 registered

        // BTreeMap guarantees sorted keys
        let layers: Vec<&String> = s.by_layer.keys().collect();
        assert_eq!(layers, vec!["L0", "L1", "L3"]); // sorted

        assert_eq!(s.by_layer["L0"], 1); // self
        assert_eq!(s.by_layer["L1"], 2); // s-L1 + s-L1-b
        assert_eq!(s.by_layer["L3"], 1); // s-L3
    }

    /// Display formatting for PackageIdentity with missing fields.
    ///
    /// When I2.subtype or I4.layer is absent, Display shows "?".
    #[test]
    fn test_display_missing_fields() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        register("display-bare", &[("I1.key", "display-bare")], &[]);

        let pkg = lookup("display-bare").unwrap();
        let display = format!("{pkg}");
        assert_eq!(display, "display-bare (? @ ?)");
    }

    /// by_metadata helper — filter by arbitrary metadata key/value.
    #[test]
    fn test_by_metadata_helper() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());
        reset_registry();

        register("meta-a", &[], &[("C7.paradigm", "CPI-SI")]);
        register("meta-b", &[], &[("C7.paradigm", "Traditional")]);
        register("meta-c", &[], &[("C7.paradigm", "CPI-SI")]);

        let cpisi = by_metadata("C7.paradigm", "CPI-SI");
        // self (CPI-SI) + meta-a + meta-c = 3
        assert_eq!(cpisi.len(), 3);

        let traditional = by_metadata("C7.paradigm", "Traditional");
        assert_eq!(traditional.len(), 1);

        let missing = by_metadata("C7.paradigm", "AI");
        assert!(missing.is_empty());
    }

    /// RegistryError equality and clone.
    #[test]
    fn test_error_equality_and_clone() {
        let err1 = RegistryError::EmptyKey;
        let err2 = err1.clone();
        assert_eq!(err1, err2);

        let err3 = RegistryError::NotRegistered("key-a".to_owned());
        let err4 = RegistryError::NotRegistered("key-b".to_owned());
        assert_ne!(err3, err4);

        let err5 = err3.clone();
        assert_eq!(err3, err5);
    }

    /// lookup_err success and failure consistency with lookup.
    #[test]
    fn test_lookup_err_consistency() {
        let _lock = TEST_MUTEX.lock().unwrap_or_else(|e| e.into_inner());

        // Both return Some/Ok for same key
        let opt = lookup("B-L0-identity-registry");
        let res = lookup_err("B-L0-identity-registry");
        assert!(opt.is_some());
        assert!(res.is_ok());
        assert_eq!(opt.unwrap().key, res.unwrap().key);

        // Both return None/Err for missing key
        let opt = lookup("nonexistent");
        let res = lookup_err("nonexistent");
        assert!(opt.is_none());
        assert!(res.is_err());
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library crate — no entry point. Imported and called, not executed directly.
// Self-registration happens via LazyLock on first access to REGISTRY.
//
// Usage: use bereshit_l0_identity;

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// In-memory registry — no resources to clean up.
// Garbage collected when process exits. No Drop needed.

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Remove not the ancient landmark, which thy fathers have set."
// — Proverbs 22:28
//
// Safe to Modify:
//   - Add new query functions (follow by_layer/by_domain pattern)
//   - Add new secondary indexes (follow BTreeSet pattern)
//   - Add new PackageIdentity methods (accessors, predicates)
//   - Add new RegistryError variants
//   - Add #[cfg(test)] tests
//
// Modify with Care:
//   - register/register_err signature — breaks all calling crates
//   - PackageIdentity struct fields — breaks all consumers
//   - RegistryError variants — breaks match arms
//   - PRAGMA/METADATA static layout or accessor signatures
//
// Never Modify:
//   - 4-block structure (METADATA → SETUP → BODY → CLOSING)
//   - Self-registration pattern (LazyLock bootstrap)
//   - &[(&str, &str)] identity format (I1-I4, C1-C7)
//   - BTreeSet index pattern (data structure IS the algorithm)

// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Completed (b-02.00):
//   - Full 4-block alignment with I/C metadata format
//   - RegistryError enum with Display + Error impls
//   - register_err() and lookup_err() Result-returning variants
//   - PackageIdentity Display impl and get() unified accessor
//   - RegistryStats type + stats() API
//   - BTreeSet secondary indexes — O(log n) without manual binary search
//   - Index maintenance on re-registration (old removed, new inserted)
//   - Comprehensive test suite (6 test functions, 13+ phases)
//
// Designed Growth Points:
//   - Adding queries: follow by_layer/by_domain pattern with new BTreeSet index
//   - Adding indexes: add field to RegistryInner, update update_indexes()
//   - Adding helpers: see by_metadata pattern (private, O(n) fallback)
//
// Future Considerations:
//   - Dependency graph from C4 data
//   - Health aggregation across all registered crates
//   - Dashboard feed for real-time topology
//   - Cross-process discovery (gRPC/IPC)
//   - Tag index (BTreeMap<String, BTreeSet<String>>) for O(log n) by_tag

// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lack wisdom, let him ask of God." — James 1:5
//
// Crate not found in registry:
//   LazyLock initialization may not have been triggered yet. Ensure the
//   identity crate is used (any function call triggers init). Check that
//   the registering crate calls identity::register() during its init.
//
// Duplicate keys:
//   Last writer wins — check //omni:key uniqueness across crates.
//   Re-registration correctly updates indexes (old entry removed first).
//
// Tests pass individually but fail together:
//   Global state via LazyLock + RwLock. All tests acquire TEST_MUTEX to
//   serialize access. Rust runs tests in parallel by default — without
//   the mutex, reset_registry() in one test races with assertions in
//   another. The mutex is zero-dependency (std::sync::Mutex).
//
// RwLock poison:
//   If a thread panics while holding the write lock, the RwLock becomes
//   poisoned. unwrap_or_else(|e| e.into_inner()) recovers gracefully —
//   this is standard Rust practice for non-critical locks. Go's sync.RWMutex
//   doesn't have this concern (no poison concept).

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Dependencies:   std::collections (BTreeMap, BTreeSet), std::fmt,
//                 std::sync (RwLock, LazyLock)
// Dependents:     [future: every OmniCode Rust crate]
// Parallel:       L0-universal/b-spiral/identity/registry.go (Go implementation)
// Template:       b-word/seed/code/L0/rust/library.rs
//
// Validation:
//   Build:    cargo build -p bereshit-l0-identity
//   Test:     cargo test -p bereshit-l0-identity
//   Clippy:   cargo clippy -p bereshit-l0-identity -- -D warnings
//   Lint:     deno run --allow-read --allow-run mod.ts lint rust lib.rs
//   All:      cargo clippy -- -D warnings && cargo test

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// The identity registry is the system's self-awareness layer.
// Every OmniCode Rust crate registers itself; the registry
// self-registers via LazyLock — the bootstrap anchor.
//
// Go needed manual binary search in sorted slices. Rust just uses
// BTreeSet — the data structure IS the algorithm. This is where
// choosing the right collection type replaces algorithmic complexity.
//
// "And God called the light Day, and the darkness he called Night."
// — Genesis 1:5
//
// Naming establishes identity. Registration declares existence.

// ============================================================================
// END CLOSING
// ============================================================================
