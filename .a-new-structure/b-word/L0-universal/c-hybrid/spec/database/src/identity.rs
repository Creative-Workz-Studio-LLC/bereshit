//! Package-level OmniCode identity — runtime-queryable pragma and metadata.
//!
//! Every CPI-SI crate carries its own identity via static PRAGMA (I1-I4) and
//! METADATA (C1-C7) slices. Zero-cost at runtime — compiled into the binary.
//!
//! Genesis 1:1 — "In the beginning God created the heaven and the earth."
//! The Triangle: Configuration (TOML) → Data (SQLite) → Code (this crate).

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for the foundation database crate.
///
/// Carries the I-sections (Identity) of the OmniCode metadata standard.
/// These fields establish WHO this crate is in the project graph.
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-foundation-db"),
    ("I1.format",    "rust"),
    ("I1.from",      "b-word/seed/code/L0/rust/library.rs"),
    ("I1.at",        "a-02.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "library"),
    ("I2.role",      "foundation"),
    // I3: Instance
    ("I3.file",      "lib.rs"),
    ("I3.title",     "Foundation Database"),
    ("I3.component", "c-hybrid/spec/database"),
    ("I3.path",      "L0-universal/c-hybrid/spec/database/src/lib.rs"),
    ("I3.provides",  "FOUNDATION_QUERIES"),
    ("I3.brief",     "Typed SQLite access to all TOML-materialized foundation data"),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "hybrid/spec/database"),
    ("I4.pattern",   "The Triangle — configuration (TOML) → data (SQLite) → code (queries)"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context for the foundation database crate.
///
/// Carries the C-sections (Context) of the OmniCode metadata standard.
/// These fields establish WHEN, WHERE, WHY, and HOW for this crate.
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "a-02.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-16"),
    ("C1.updated",           "2026-02-19"),
    // C2: Attribution
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    ("C3.principle",         "The Triangle complete — configuration → data → code. The system knows itself."),
    ("C3.anchor",            "Genesis 1:1 — In the beginning God created the heaven and the earth"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "std::fmt, std::path"),
    ("C4.requires.external", "rusqlite 0.38 (bundled SQLite)"),
    ("C4.requires.internal", "bereshit-l0-identity (registration)"),
    ("C4.consumers",         "L3-cpisi/core, tools, future OmniCode code generation"),
    ("C4.integration",       "use bereshit_l0_foundation_db::{DB, pragma_get}"),
    ("C4.if_missing",        "no typed access to foundation data — consumers parse SQLite directly or hardcode values"),
    // C5: Intent
    ("C5.purpose",           "Typed query layer for the foundation database — specs, types, operations, books, scales"),
    ("C5.philosophy",        "L0 stays universal. Read-only access. The database is truth materialized from TOML specs."),
    // C6: Roadmap
    ("C6.current",           "a-02.00 — 8 table readers, I/C metadata, identity registration"),
    ("C6.planned",           "Prepared statement caching, connection pooling, schema version checks"),
    ("C6.limitations",       "Read-only, no write operations, no migrations (L2 concern)"),
    // C7: Classification
    ("C7.tags",              "database, sqlite, foundation, queries, specs, types, operations, books"),
    ("C7.category",          "Foundation"),
    ("C7.domain",            "foundation-queries"),
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
/// use bereshit_l0_foundation_db::pragma_get;
///
/// assert_eq!(pragma_get("I1.key"), Some("B-L0-foundation-db"));
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
/// use bereshit_l0_foundation_db::metadata_get;
///
/// assert_eq!(metadata_get("C1.version"), Some("a-02.00"));
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
