//! OmniCode Format Dispatcher — Route Files to the Right Parser.
//!
//! **THIS IS THE L0-L1 BRIDGE.** Reads pragma (L1 domain), dispatches to
//! L0 format parsers (TOML, JSONC). The compiler witnessing this code
//! compile proves the layer contract is real.
//!
//! Genesis 1:4 — "And God divided the light from the darkness."
//! Separation enables identity — each format gets its proper handler.

use std::collections::BTreeMap;
use std::path::{Path, PathBuf};
use std::sync::{LazyLock, RwLock};

use serde::de::DeserializeOwned;

use bereshit_l0_config as config;

use crate::reader;
use crate::types::*;

// ────────────────────────────────────────────────────────────────
// Dispatch Cache
// ────────────────────────────────────────────────────────────────

/// Cached dispatch result for the map variant.
///
/// Generic `load_config<T>` can't be cached (type erasure), but
/// `load_config_map` is the common hot path for OmniFold traversal
/// and tools that inspect arbitrary files.
#[derive(Clone)]
struct ConfigMapEntry {
    pragma: Pragma,
    data: serde_json::Map<String, serde_json::Value>,
}

/// Global dispatch cache. RwLock for concurrent reads, exclusive writes.
static DISPATCH_CACHE: LazyLock<RwLock<BTreeMap<PathBuf, ConfigMapEntry>>> =
    LazyLock::new(|| RwLock::new(BTreeMap::new()));

// ────────────────────────────────────────────────────────────────
// Format Dispatch
// ────────────────────────────────────────────────────────────────

/// Load an OmniCode-wrapped config file.
///
/// Parses the pragma (L1) to determine format, then dispatches to the
/// right L0 parser. Returns both the pragma identity and the parsed data.
///
/// **L0 Connections (compiler-verified):**
///   - `Format::Toml` → `bereshit_l0_config::load_toml_file::<T>(path)`
///   - `Format::Json` → `bereshit_l0_config::load_jsonc_file::<T>(path)`
///   - `Format::Omni` → error (native parser not yet implemented)
///
/// Go equivalent: `func LoadConfig[T any](path string) (*Pragma, *T, error)`
pub fn load_config<T: DeserializeOwned>(path: &Path) -> Result<(Pragma, T), DispatchError> {
    // Step 1: Read pragma (L1 — identity)
    let pragma = reader::parse(path)?;

    // Step 2: Dispatch to L0 parser based on format
    // JSON routes to JSONC loader — JSONC is a superset of JSON
    let result: T = match pragma.format {
        Format::Toml => config::load_toml_file(path)?,
        Format::Json | Format::Jsonc => config::load_jsonc_file(path)?,
        Format::Omni => return Err(DispatchError::NativeNotImplemented),
        other => return Err(DispatchError::UnsupportedFormat(other)),
    };

    Ok((pragma, result))
}

/// Load an OmniCode config file into a generic map.
///
/// Useful when the exact structure isn't known at compile time.
///
/// Results are cached by filepath — subsequent calls for the same path
/// return the cached result without disk I/O. Use [`invalidate_dispatch_cache()`]
/// after file modifications.
///
/// Go equivalent: `func LoadConfigMap(path string) (*Pragma, map[string]interface{}, error)`
pub fn load_config_map(
    path: &Path,
) -> Result<(Pragma, serde_json::Map<String, serde_json::Value>), DispatchError> {
    // Normalize path — prevents cache misses from different representations.
    let canonical = std::fs::canonicalize(path).unwrap_or_else(|_| path.to_path_buf());

    // Check cache first (read lock)
    {
        let guard = DISPATCH_CACHE.read().unwrap_or_else(|e| e.into_inner());
        if let Some(entry) = guard.get(&canonical) {
            return Ok((entry.pragma.clone(), entry.data.clone()));
        }
    }

    // Cache miss — load via generic path
    let (pragma, data): (Pragma, serde_json::Map<String, serde_json::Value>) = load_config(path)?;

    // Store in cache (write lock) — keyed by canonical path
    {
        let mut guard = DISPATCH_CACHE.write().unwrap_or_else(|e| e.into_inner());
        guard.insert(
            canonical,
            ConfigMapEntry {
                pragma: pragma.clone(),
                data: data.clone(),
            },
        );
    }

    Ok((pragma, data))
}

// ────────────────────────────────────────────────────────────────
// OmniFold (.ofd) Support
// ────────────────────────────────────────────────────────────────

/// Load an OmniFold (.ofd) folder metadata file.
///
/// Returns the pragma and raw content as a map. Validates that
/// the file type is `folder`.
///
/// Go equivalent: `func LoadFolder(path string) (*Pragma, map[string]interface{}, error)`
pub fn load_folder(
    path: &Path,
) -> Result<(Pragma, serde_json::Map<String, serde_json::Value>), DispatchError> {
    let ext = path
        .extension()
        .and_then(|e| e.to_str())
        .map(|e| format!(".{e}"))
        .unwrap_or_default();

    if ext != EXT_OMNIFOLD && ext != EXT_OMNI {
        return Err(DispatchError::UnsupportedFormat(Format::Omni));
    }

    let (pragma, data) = load_config_map(path)?;

    if pragma.file_type != FileType::Folder {
        // Return data but log warning — Go version returns error
        return Err(DispatchError::UnsupportedFormat(pragma.format));
    }

    Ok((pragma, data))
}

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

/// Whether a file path has an OmniCode extension.
///
/// Go equivalent: `func IsOmniCodeFile(path string) bool`
pub fn is_omnicode_file(path: &Path) -> bool {
    crate::extensions::is_omnicode_file(path)
}

// ────────────────────────────────────────────────────────────────
// Cache Invalidation
// ────────────────────────────────────────────────────────────────

/// Clear the entire dispatch cache.
/// Also clears the pragma cache since dispatch depends on it.
///
/// Go equivalent: `func InvalidateDispatchCache()`
pub fn invalidate_dispatch_cache() {
    {
        let mut guard = DISPATCH_CACHE.write().unwrap_or_else(|e| e.into_inner());
        guard.clear();
    }
    // Pragma cache is upstream — clear it too
    crate::cache::invalidate_all();
}

/// Remove a single entry from the dispatch cache.
/// Also clears the corresponding pragma cache entry.
///
/// Go equivalent: `func InvalidateDispatchCacheEntry(path string)`
pub fn invalidate_dispatch_entry(path: &Path) {
    let canonical = std::fs::canonicalize(path).unwrap_or_else(|_| path.to_path_buf());
    {
        let mut guard = DISPATCH_CACHE.write().unwrap_or_else(|e| e.into_inner());
        guard.remove(&canonical);
    }
    crate::cache::invalidate(&canonical);
}

// ────────────────────────────────────────────────────────────────
// Tests
// ────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs;

    fn test_dir(name: &str) -> std::path::PathBuf {
        let ts = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_nanos();
        let dir = std::env::temp_dir().join(format!("bereshit-l1-dispatch-{name}-{ts}"));
        fs::create_dir_all(&dir).unwrap();
        dir
    }

    fn write_file(root: &Path, rel: &str, content: &str) {
        let path = root.join(rel);
        fs::create_dir_all(path.parent().unwrap()).unwrap();
        fs::write(path, content).unwrap();
    }

    #[test]
    fn test_load_config_toml() {
        let dir = test_dir("toml");
        write_file(
            &dir,
            "config.toml",
            "# #!omni data --toml\n# #!omni meta.key = test-config\n\n[section]\nkey = \"value\"\n",
        );

        #[derive(serde::Deserialize)]
        struct Outer {
            section: Section,
        }
        #[derive(serde::Deserialize)]
        struct Section {
            key: String,
        }

        let (pragma, data): (Pragma, Outer) = load_config(&dir.join("config.toml")).unwrap();
        assert_eq!(pragma.format, Format::Toml);
        assert_eq!(pragma.key, "test-config");
        assert_eq!(data.section.key, "value");

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_config_jsonc() {
        let dir = test_dir("jsonc");
        write_file(
            &dir,
            "config.jsonc",
            "// #!omni data --json\n// #!omni meta.key = test-json\n{\n  // comment\n  \"name\": \"bereshit\",\n}\n",
        );

        #[derive(serde::Deserialize)]
        struct Config {
            name: String,
        }

        let (pragma, data): (Pragma, Config) = load_config(&dir.join("config.jsonc")).unwrap();
        assert_eq!(pragma.format, Format::Json);
        assert_eq!(pragma.key, "test-json");
        assert_eq!(data.name, "bereshit");

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_config_omni_not_implemented() {
        let dir = test_dir("omni-native");
        write_file(&dir, "file.omni", "#!omni code\n#!omni meta.key = native\n");

        let err = load_config::<serde_json::Value>(&dir.join("file.omni")).unwrap_err();
        assert!(matches!(err, DispatchError::NativeNotImplemented));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_config_unsupported_format() {
        let dir = test_dir("unsupported");
        write_file(&dir, "file.omni", "#!omni code --go\n");

        let err = load_config::<serde_json::Value>(&dir.join("file.omni")).unwrap_err();
        assert!(matches!(err, DispatchError::UnsupportedFormat(Format::Go)));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_config_missing_pragma() {
        let dir = test_dir("no-pragma");
        write_file(&dir, "plain.toml", "[section]\nkey = \"value\"\n");

        let err = load_config::<serde_json::Value>(&dir.join("plain.toml")).unwrap_err();
        assert!(matches!(err, DispatchError::Pragma(_)));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_load_config_map_toml() {
        let dir = test_dir("map-toml");
        write_file(
            &dir,
            "data.toml",
            "# #!omni data --toml\n# #!omni meta.key = map-test\n\n[info]\nname = \"test\"\n",
        );

        let (pragma, data) = load_config_map(&dir.join("data.toml")).unwrap();
        assert_eq!(pragma.key, "map-test");
        let info = data.get("info").expect("should have info key");
        assert!(info.is_object());

        let _ = fs::remove_dir_all(&dir);
    }
}
