//! Manifest vs filesystem comparison — tripwire detection (Ezekiel 33:6).

use std::collections::BTreeSet;
use std::path::Path;
use std::{fs, io};

use crate::types::{DiscoveryResult, IndexManifest};

/// Find all .toml filenames in a directory.
pub(crate) fn discover_toml_files(dir: &Path) -> Result<Vec<String>, io::Error> {
    let mut files: Vec<String> = fs::read_dir(dir)?
        .filter_map(|entry| entry.ok())
        .filter(|entry| entry.path().extension().is_some_and(|ext| ext == "toml"))
        .map(|entry| entry.file_name().to_string_lossy().into_owned())
        .collect();
    files.sort();
    Ok(files)
}

/// Compare manifest entries to actual files on disk.
/// Missing = ERROR (broken promise). Unexpected = WARNING (untracked).
pub(crate) fn compare_manifest_to_disk(root: &Path, manifest: &IndexManifest) -> DiscoveryResult {
    let mut result = DiscoveryResult::new();

    for system in &manifest.systems {
        let system_path = root.join(&system.path);

        // Collect manifest entries
        let manifest_files: BTreeSet<&str> = system.specs.iter().map(|s| s.file.as_str()).collect();
        for spec in &system.specs {
            result
                .manifest
                .push(format!("{}/{}", system.path, spec.file));
        }

        // Discover actual files
        let discovered_files = match discover_toml_files(&system_path) {
            Ok(files) => files,
            Err(_) => {
                for f in &manifest_files {
                    result.missing.push(format!("{}/{f}", system.path));
                }
                result.valid = false;
                continue;
            }
        };
        let discovered_set: BTreeSet<&str> = discovered_files.iter().map(|s| s.as_str()).collect();
        for f in &discovered_files {
            result.discovered.push(format!("{}/{f}", system.path));
        }

        // Missing: in manifest but not on disk (ERROR)
        for f in &manifest_files {
            if !discovered_set.contains(f) {
                result.missing.push(format!("{}/{f}", system.path));
                result.valid = false;
            }
        }

        // Unexpected: on disk but not in manifest (WARNING)
        for f in &discovered_set {
            if !manifest_files.contains(f) {
                result.unexpected.push(format!("{}/{f}", system.path));
            }
        }
    }

    result
}
