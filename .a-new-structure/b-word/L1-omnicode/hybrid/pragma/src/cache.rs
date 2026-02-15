//! Pragma cache — avoid re-parsing the same file on every access.
//!
//! Mirrors L0's `cache.rs` pattern: `RwLock`-guarded map for concurrent
//! read access with exclusive writes. One cache layer (pragma by path),
//! unlike L0's dual-layer (index + spec).
//!
//! Proverbs 21:5 — "The thoughts of the diligent tend only to plenteousness."
//! Cache once, use many times.

use std::collections::BTreeMap;
use std::path::{Path, PathBuf};
use std::sync::{LazyLock, RwLock};

use crate::types::Pragma;

// ────────────────────────────────────────────────────────────────
// Cache Structure
// ────────────────────────────────────────────────────────────────

/// Single-layer pragma cache.
///
/// Keyed by canonical `PathBuf` — the file path passed to `parse()`.
/// Stores cloned `Pragma` values (all owned `String` fields, cheap to clone).
///
/// `RwLock` allows concurrent readers (the common case — many callers
/// reading the same pragma) with exclusive writers (cache misses).
/// This matches Go's `sync.RWMutex` pattern from `pragma.go`.
struct PragmaCache {
    /// Parsed pragmas keyed by file path.
    pragmas: RwLock<BTreeMap<PathBuf, Pragma>>,
}

/// Global cache instance. Created once at process start.
static CACHE: LazyLock<PragmaCache> = LazyLock::new(|| PragmaCache {
    pragmas: RwLock::new(BTreeMap::new()),
});

// ────────────────────────────────────────────────────────────────
// Cache Operations
// ────────────────────────────────────────────────────────────────

/// Get a cached pragma by file path, if present.
///
/// Returns a clone — mutations don't affect the cached copy.
/// This is the read-lock path (concurrent readers allowed).
pub(crate) fn get(path: &Path) -> Option<Pragma> {
    let guard = CACHE.pragmas.read().unwrap_or_else(|e| e.into_inner());
    guard.get(path).cloned()
}

/// Store a parsed pragma in the cache.
///
/// This is the write-lock path (exclusive access).
pub(crate) fn put(path: PathBuf, pragma: Pragma) {
    let mut guard = CACHE.pragmas.write().unwrap_or_else(|e| e.into_inner());
    guard.insert(path, pragma);
}

/// Clear the entire pragma cache.
///
/// Call after bulk file modifications or during testing.
///
/// Go equivalent: `func InvalidatePragmaCache()`
pub fn invalidate_all() {
    let mut guard = CACHE.pragmas.write().unwrap_or_else(|e| e.into_inner());
    guard.clear();
}

/// Remove a single entry from the pragma cache.
///
/// Call after modifying a specific file to force re-parsing on next access.
/// Normalizes the path to match how `parse()` stores entries.
///
/// Go equivalent: `func InvalidatePragmaCacheEntry(path string)`
pub fn invalidate(path: &Path) {
    let canonical = std::fs::canonicalize(path).unwrap_or_else(|_| path.to_path_buf());
    let mut guard = CACHE.pragmas.write().unwrap_or_else(|e| e.into_inner());
    guard.remove(&canonical);
}

// ── Introspection (for diagnostics and tests) ────────────────────

/// Returns the number of cached pragmas.
#[cfg(test)]
#[allow(dead_code)]
pub(crate) fn count() -> usize {
    let guard = CACHE.pragmas.read().unwrap_or_else(|e| e.into_inner());
    guard.len()
}

/// Returns true if a specific path is cached.
#[cfg(test)]
pub(crate) fn has(path: &Path) -> bool {
    let guard = CACHE.pragmas.read().unwrap_or_else(|e| e.into_inner());
    guard.contains_key(path)
}

// ────────────────────────────────────────────────────────────────
// Tests
// ────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;
    use crate::types::*;

    fn make_pragma(key: &str) -> Pragma {
        Pragma {
            file_type: FileType::Code,
            format: Format::Go,
            modifier: Modifier::Library,
            key: key.to_string(),
            from: String::new(),
            at: "a-01.00".to_string(),
            raw_lines: vec!["#!omni code --go -library".to_string()],
        }
    }

    #[test]
    fn test_cache_miss_returns_none() {
        let path = PathBuf::from("/nonexistent/file.omni");
        assert!(get(&path).is_none());
    }

    /// Tests put/get, invalidate-entry, and invalidate-all in sequence.
    ///
    /// These share a global `LazyLock<PragmaCache>` — running in parallel
    /// causes `invalidate_all()` to race with other tests' `put()`/`has()`.
    /// One sequential test eliminates the race without `serial_test` dep.
    #[test]
    fn test_cache_full_lifecycle() {
        // Phase 1: put + get
        let path = PathBuf::from("/test/cache-lifecycle.omni");
        let pragma = make_pragma("lifecycle-test");

        put(path.clone(), pragma.clone());
        let cached = get(&path);
        assert!(cached.is_some(), "put() then get() should return Some");
        assert_eq!(cached.unwrap().key, "lifecycle-test");

        // Phase 2: invalidate single entry
        assert!(has(&path), "entry should exist before invalidate");
        invalidate(&path);
        assert!(!has(&path), "entry should be gone after invalidate");

        // Phase 3: invalidate all
        let p1 = PathBuf::from("/test/cache-all-1.omni");
        let p2 = PathBuf::from("/test/cache-all-2.omni");
        put(p1.clone(), make_pragma("all-1"));
        put(p2.clone(), make_pragma("all-2"));
        assert!(has(&p1));
        assert!(has(&p2));

        invalidate_all();
        assert!(!has(&p1), "p1 should be gone after invalidate_all");
        assert!(!has(&p2), "p2 should be gone after invalidate_all");
    }
}
