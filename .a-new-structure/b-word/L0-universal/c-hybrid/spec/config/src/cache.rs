//! Dual-layer config cache — index manifest + individual spec files.
//!
//! Ported from Go `loader.go` caching pattern. Two independent caches:
//! - **Index cache:** parsed `index.toml` manifest (loaded once, used many times)
//! - **Spec cache:** individual TOML spec files keyed by canonical path
//!
//! Both caches are RwLock-guarded for concurrent read access with exclusive writes.
//! Cleared on `set_root()` / `clear_root()` — same lifecycle as Go's `SetRoot()`.
//!
//! Proverbs 21:5 — "The thoughts of the diligent tend only to plenteousness."
//! Cache once, use many times. Diligence in caching = abundance in performance.

use std::collections::BTreeMap;
use std::sync::{LazyLock, RwLock};

use crate::types::{ConfigFile, IndexManifest};

// ────────────────────────────────────────────────────────────────
// Cache Structure
// ────────────────────────────────────────────────────────────────

/// Dual-layer configuration cache.
///
/// - **Index layer:** The parsed `index.toml` manifest — one per root.
/// - **Spec layer:** Individual config files keyed by canonical path string.
///
/// Both layers use `RwLock` for concurrent reads with exclusive writes.
/// This matches Go's `sync.RWMutex` pattern from `loader.go`.
struct Cache {
    /// Parsed index.toml manifest. `None` = not loaded yet.
    index: RwLock<Option<IndexManifest>>,
    /// Individual spec files keyed by path string.
    specs: RwLock<BTreeMap<String, ConfigFile>>,
}

impl Cache {
    fn new() -> Self {
        Self {
            index: RwLock::new(None),
            specs: RwLock::new(BTreeMap::new()),
        }
    }
}

/// Global cache instance. Created once, cleared on root changes.
static CACHE: LazyLock<Cache> = LazyLock::new(Cache::new);

// ────────────────────────────────────────────────────────────────
// Cache Operations
// ────────────────────────────────────────────────────────────────

/// Clear both cache layers.
///
/// Called by `set_root()` and `clear_root()` — a new root means all
/// cached data is stale. This matches Go's `SetRoot()` behavior.
pub(crate) fn clear_all() {
    {
        let mut guard = CACHE.index.write().unwrap_or_else(|e| e.into_inner());
        *guard = None;
    }
    {
        let mut guard = CACHE.specs.write().unwrap_or_else(|e| e.into_inner());
        guard.clear();
    }
}

// ── Index Layer ──────────────────────────────────────────────────

/// Get cached index manifest, if present.
///
/// Returns a clone — mutations don't affect the cached copy.
pub(crate) fn get_index() -> Option<IndexManifest> {
    let guard = CACHE.index.read().unwrap_or_else(|e| e.into_inner());
    guard.clone()
}

/// Store a parsed index manifest in the cache.
pub(crate) fn put_index(manifest: IndexManifest) {
    let mut guard = CACHE.index.write().unwrap_or_else(|e| e.into_inner());
    *guard = Some(manifest);
}

// ── Spec Layer ──────────────────────────────────────────────────

/// Get a cached spec file by path key, if present.
///
/// Returns a clone — mutations don't affect the cached copy.
pub(crate) fn get_spec(key: &str) -> Option<ConfigFile> {
    let guard = CACHE.specs.read().unwrap_or_else(|e| e.into_inner());
    guard.get(key).cloned()
}

/// Store a loaded spec file in the cache.
pub(crate) fn put_spec(key: String, config: ConfigFile) {
    let mut guard = CACHE.specs.write().unwrap_or_else(|e| e.into_inner());
    guard.insert(key, config);
}

// ── Introspection (for diagnostics) ─────────────────────────────

/// Returns the number of cached spec files.
#[cfg(test)]
pub(crate) fn spec_count() -> usize {
    let guard = CACHE.specs.read().unwrap_or_else(|e| e.into_inner());
    guard.len()
}

/// Returns true if the index manifest is cached.
#[cfg(test)]
pub(crate) fn has_index() -> bool {
    let guard = CACHE.index.read().unwrap_or_else(|e| e.into_inner());
    guard.is_some()
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::types::{ConfigFile, IndexManifest, SystemEntry};
    use std::path::PathBuf;

    #[test]
    fn test_cache_full_lifecycle() {
        // Phase 1: Clear and verify empty
        clear_all();
        assert!(!has_index(), "index should be empty after clear");
        assert_eq!(spec_count(), 0, "specs should be empty after clear");
        assert!(get_index().is_none());
        assert!(get_spec("nonexistent").is_none());

        // Phase 2: Index round-trip
        let manifest = IndexManifest {
            systems: vec![SystemEntry {
                name: "math".into(),
                path: "math".into(),
                order: 0,
                specs: vec![],
            }],
        };
        put_index(manifest);
        assert!(has_index());

        let cached = get_index().unwrap();
        assert_eq!(cached.systems.len(), 1);
        assert_eq!(cached.systems[0].name, "math");

        // Phase 3: Spec round-trip
        let config = ConfigFile {
            name: "file.toml".into(),
            path: PathBuf::from("test/file.toml"),
            data: toml::Table::new(),
            keys: vec!["section".into()],
            pragma: None,
            metadata: None,
        };
        put_spec("test/file.toml".into(), config);
        assert_eq!(spec_count(), 1);

        let cached_spec = get_spec("test/file.toml").unwrap();
        assert_eq!(cached_spec.name, "file.toml");
        assert_eq!(cached_spec.keys, vec!["section"]);

        // Phase 4: Clones, not references
        let mut copy1 = get_spec("test/file.toml").unwrap();
        let copy2 = get_spec("test/file.toml").unwrap();
        copy1.keys.push("mutated".into());
        // copy2 should be unaffected by mutation of copy1
        assert_eq!(copy2.keys, vec!["section"]);

        // Phase 5: Clear wipes both layers
        clear_all();
        assert!(!has_index());
        assert_eq!(spec_count(), 0);
    }
}
