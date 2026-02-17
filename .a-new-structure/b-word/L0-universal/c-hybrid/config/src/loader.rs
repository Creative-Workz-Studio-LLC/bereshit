//! Core loading — manifest, file, directory, batch, and tripwire.
//!
//! Two loading flows modeled on Habakkuk 2:
//! - **Primary (appointed time):** `index.toml` manifest drives loading order
//! - **Watch (ramparts):** known filesystem positions when manifest is absent

use std::collections::BTreeMap;
use std::fs;
use std::io;
use std::path::Path;

use crate::cache;
use crate::discovery::compare_manifest_to_disk;
use crate::error::ConfigError;
use crate::graph::{build_dependency_graph, validate_dependencies};
use crate::types::{ConfigFile, IndexManifest, LoadResult};

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────

/// Index directory relative to module root.
pub(crate) const INDEX_DIR: &str = "L0-universal/ladder/foundation";

/// Index manifest filename within INDEX_DIR.
pub(crate) const INDEX_FILE: &str = "index.toml";

/// Known filesystem positions when manifest is absent (Habakkuk 2:1).
/// Sorted alphabetically for deterministic iteration.
pub(crate) const WATCH_PATHS: &[(&str, &str)] = &[
    ("bible", "L0-universal/ladder/foundation/bible"),
    ("filesystem", "L2-platform/filesystem"),
    ("health", "L2-platform/os/health"),
    ("identity", "L3-cpisi/identity"),
    ("language", "L1-omnicode/ladder/language"),
    ("math", "L0-universal/ladder/foundation/math"),
    ("network", "L4-faithnet/network"),
    ("permission", "L2-platform/os/permission"),
    ("types", "L0-universal/ladder/foundation/types"),
];

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

/// Extract a flat string table from raw TOML data.
/// Returns None if key is absent or not a table.
/// This is the identity bridge: TOML files with [_pragma] and [_metadata]
/// become self-aware when loaded.
pub(crate) fn extract_string_map(
    data: &toml::Table,
    key: &str,
) -> Option<BTreeMap<String, String>> {
    let table = data.get(key)?.as_table()?;
    let mut result = BTreeMap::new();
    for (k, v) in table {
        if let Some(s) = v.as_str() {
            result.insert(k.clone(), s.to_owned());
        }
    }
    Some(result)
}

/// Collect filenames from a slice of ConfigFiles.
fn collect_names(configs: &[ConfigFile]) -> Vec<String> {
    configs.iter().map(|c| c.name.clone()).collect()
}

// ────────────────────────────────────────────────────────────────
// Tripwire
// ────────────────────────────────────────────────────────────────

/// Print a loud warning banner (Ezekiel 33:6 — the trumpet).
///
/// Takes a writer so tests can capture output instead of polluting stdout.
/// Production callers pass `&mut io::stdout()`.
pub(crate) fn tripwire_banner(w: &mut dyn io::Write, title: &str, lines: &[String]) {
    let border = "\u{2550}".repeat(64);
    let _ = writeln!(w);
    let _ = writeln!(w, "{border}");
    let _ = writeln!(w, "\u{26A0}  {title}");
    let _ = writeln!(w, "{border}");
    for line in lines {
        let _ = writeln!(w, "  {line}");
    }
    let _ = writeln!(w, "{border}");
    let _ = writeln!(w);
}

// ────────────────────────────────────────────────────────────────
// Core Loading
// ────────────────────────────────────────────────────────────────

/// Load and parse the index.toml manifest.
///
/// Checks the index cache first — returns a clone if cached.
/// On miss, loads from disk, sorts by dependency order, and caches.
pub(crate) fn load_index(root: &Path) -> Result<IndexManifest, ConfigError> {
    // Cache hit — return clone
    if let Some(cached) = cache::get_index() {
        return Ok(cached);
    }

    // Cache miss — load from disk
    let index_path = root.join(INDEX_DIR).join(INDEX_FILE);
    if !index_path.exists() {
        return Err(ConfigError::Load {
            file: INDEX_FILE.to_owned(),
            op: "read".to_owned(),
            source: format!("not found: {}", index_path.display()),
        });
    }

    let content = fs::read_to_string(&index_path).map_err(|e| ConfigError::Load {
        file: INDEX_FILE.to_owned(),
        op: "read".to_owned(),
        source: e.to_string(),
    })?;

    let mut manifest: IndexManifest = toml::from_str(&content).map_err(|e| ConfigError::Load {
        file: INDEX_FILE.to_owned(),
        op: "parse".to_owned(),
        source: e.to_string(),
    })?;

    // Sort by dependency order (0 = anchor, higher = depends on lower)
    manifest.systems.sort_by_key(|s| s.order);

    // Cache for future calls
    cache::put_index(manifest.clone());
    Ok(manifest)
}

/// Load a single TOML file, extracting identity tables.
///
/// Checks the spec cache first — returns a clone if cached.
/// On miss, loads from disk, extracts identity, and caches.
pub(crate) fn load_file(path: &Path) -> Result<ConfigFile, ConfigError> {
    let cache_key = path.to_string_lossy().to_string();

    // Cache hit — return clone
    if let Some(cached) = cache::get_spec(&cache_key) {
        return Ok(cached);
    }

    // Cache miss — load from disk
    let name = path
        .file_name()
        .map(|n| n.to_string_lossy().into_owned())
        .unwrap_or_default();

    if !path.exists() {
        return Err(ConfigError::Load {
            file: name,
            op: "read".to_owned(),
            source: format!("not found: {}", path.display()),
        });
    }

    let content = fs::read_to_string(path).map_err(|e| ConfigError::Load {
        file: name.clone(),
        op: "read".to_owned(),
        source: e.to_string(),
    })?;

    let data: toml::Table = toml::from_str(&content).map_err(|e| ConfigError::Load {
        file: name.clone(),
        op: "parse".to_owned(),
        source: e.to_string(),
    })?;

    // Extract sorted top-level keys
    let mut keys: Vec<String> = data.keys().cloned().collect();
    keys.sort();

    // Extract identity tables — the bridge: TOML files become self-aware
    let pragma = extract_string_map(&data, "_pragma");
    let metadata = extract_string_map(&data, "_metadata");

    let config = ConfigFile {
        name,
        path: path.to_owned(),
        data,
        keys,
        pragma,
        metadata,
    };

    // Cache for future calls
    cache::put_spec(cache_key, config.clone());
    Ok(config)
}

/// Load all TOML files from a directory.
pub(crate) fn load_directory(dir: &Path) -> Result<Vec<ConfigFile>, ConfigError> {
    let name = dir
        .file_name()
        .map(|n| n.to_string_lossy().into_owned())
        .unwrap_or_default();

    if !dir.is_dir() {
        return Err(ConfigError::Load {
            file: name,
            op: "read".to_owned(),
            source: format!("directory not found: {}", dir.display()),
        });
    }

    let mut entries: Vec<_> = fs::read_dir(dir)
        .map_err(|e| ConfigError::Load {
            file: name.clone(),
            op: "read".to_owned(),
            source: e.to_string(),
        })?
        .filter_map(|entry| entry.ok())
        .filter(|entry| entry.path().extension().is_some_and(|ext| ext == "toml"))
        .collect();

    // Sort for deterministic order
    entries.sort_by_key(|e| e.file_name());

    let mut configs = Vec::new();
    for entry in entries {
        configs.push(load_file(&entry.path())?);
    }
    Ok(configs)
}

// ────────────────────────────────────────────────────────────────
// Batch Loading
// ────────────────────────────────────────────────────────────────

/// Load all specs using the manifest (primary path — appointed time).
pub(crate) fn load_all_from_index(root: &Path, w: &mut dyn io::Write) -> LoadResult {
    let mut result = LoadResult::new();

    // Step 1: Load the manifest
    let manifest = match load_index(root) {
        Ok(m) => m,
        Err(e) => {
            result.valid = false;
            result.errors.push(e);
            return result;
        }
    };

    // Step 2: Validate dependency graph
    let graph = build_dependency_graph(&manifest);
    let dep_errors = validate_dependencies(&graph);
    if !dep_errors.is_empty() {
        result.errors.extend(dep_errors);
        result.valid = false;
        // Continue — report errors but try to load
    }

    // Step 3: Compare manifest to disk (tripwire)
    let discovery = compare_manifest_to_disk(root, &manifest);
    if !discovery.valid {
        let lines: Vec<String> = discovery
            .missing
            .iter()
            .map(|m| format!("MISSING: {m} (in manifest, not on disk)"))
            .collect();
        for missing in &discovery.missing {
            result.errors.push(ConfigError::Load {
                file: missing.clone(),
                op: "read".to_owned(),
                source: "declared in manifest but not on disk".to_owned(),
            });
        }
        tripwire_banner(w, "Manifest/Disk Mismatch", &lines);
        result.valid = false;
    }
    if !discovery.unexpected.is_empty() {
        let mut lines: Vec<String> = discovery
            .unexpected
            .iter()
            .map(|u| format!("UNEXPECTED: {u}"))
            .collect();
        lines.push("Consider adding these to L0-universal/ladder/foundation/index.toml".to_owned());
        tripwire_banner(w, "Unexpected files (on disk, not in manifest)", &lines);
    }

    // Step 4: Load all specs in dependency order
    for system in &manifest.systems {
        let system_path = root.join(&system.path);
        let mut configs = Vec::new();

        for spec in &system.specs {
            let spec_path = system_path.join(&spec.file);
            match load_file(&spec_path) {
                Ok(cfg) => configs.push(cfg),
                Err(e) => {
                    result.errors.push(e);
                    result.valid = false;
                }
            }
        }

        if !configs.is_empty() {
            let names = collect_names(&configs);
            result.configs.insert(system.name.clone(), configs);
            result.summary.insert(system.name.clone(), names);
        }
    }

    result
}

/// Load all specs. Primary: manifest. Fallback: watch paths (known positions).
pub(crate) fn do_load_all(root: &Path, w: &mut dyn io::Write) -> LoadResult {
    // Try manifest first
    let index_result = load_all_from_index(root, w);
    if index_result.valid {
        return index_result;
    }

    // Check if specifically an index.toml issue
    let is_index_missing = index_result.errors.iter().any(
        |e| matches!(e, ConfigError::Load { file, op, .. } if file == INDEX_FILE && op == "read"),
    );

    if is_index_missing {
        tripwire_banner(
            w,
            "L0-universal/ladder/foundation/index.toml NOT FOUND",
            &[
                "Watch flow active \u{2014} loading from known positions, not manifest.".to_owned(),
                "Create index.toml to use the appointed order (single source of truth).".to_owned(),
            ],
        );
    }

    // Watch flow: load from known positions (Habakkuk 2:1)
    let mut result = LoadResult::new();
    for &(system, rel_path) in WATCH_PATHS {
        let dir = root.join(rel_path);
        match load_directory(&dir) {
            Ok(configs) => {
                let names = collect_names(&configs);
                result.configs.insert(system.to_owned(), configs);
                result.summary.insert(system.to_owned(), names);
            }
            Err(e) => {
                result.errors.push(e);
                result.valid = false;
            }
        }
    }

    result
}

/// Load a single system by name. Manifest first, watch path fallback.
pub(crate) fn do_load_system(root: &Path, system: &str) -> Result<Vec<ConfigFile>, ConfigError> {
    // Try manifest first
    if let Ok(manifest) = load_index(root) {
        for sys in &manifest.systems {
            if sys.name == system {
                let system_path = root.join(&sys.path);
                let mut configs = Vec::new();
                for spec in &sys.specs {
                    configs.push(load_file(&system_path.join(&spec.file))?);
                }
                return Ok(configs);
            }
        }
        return Err(ConfigError::Load {
            file: system.to_owned(),
            op: "lookup".to_owned(),
            source: "system not found in index.toml".to_owned(),
        });
    }

    // Watch flow
    WATCH_PATHS
        .iter()
        .find(|(name, _)| *name == system)
        .map(|(_, path)| root.join(path))
        .ok_or_else(|| ConfigError::Load {
            file: system.to_owned(),
            op: "lookup".to_owned(),
            source: "not found in manifest and no watch path defined".to_owned(),
        })
        .and_then(|dir| load_directory(&dir))
}

/// Load a single spec from a system. Manifest first, watch path fallback.
pub(crate) fn do_load_spec(
    root: &Path,
    system: &str,
    spec: &str,
) -> Result<ConfigFile, ConfigError> {
    // Try manifest first
    if let Ok(manifest) = load_index(root) {
        for sys in &manifest.systems {
            if sys.name == system {
                let system_path = root.join(&sys.path);
                for s in &sys.specs {
                    if s.file == spec {
                        return load_file(&system_path.join(&s.file));
                    }
                }
                return Err(ConfigError::Load {
                    file: format!("{system}/{spec}"),
                    op: "lookup".to_owned(),
                    source: "spec not found in system".to_owned(),
                });
            }
        }
        return Err(ConfigError::Load {
            file: system.to_owned(),
            op: "lookup".to_owned(),
            source: "system not found in index.toml".to_owned(),
        });
    }

    // Watch flow
    WATCH_PATHS
        .iter()
        .find(|(name, _)| *name == system)
        .map(|(_, path)| root.join(path).join(spec))
        .ok_or_else(|| ConfigError::Load {
            file: format!("{system}/{spec}"),
            op: "lookup".to_owned(),
            source: "not found in manifest and no watch path defined".to_owned(),
        })
        .and_then(|path| load_file(&path))
}
