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

#[cfg(test)]
mod tests {
    use super::*;
    use crate::test_utils::{test_dir, write_file};
    use crate::types::IndexManifest;
    use std::fs;

    #[test]
    fn test_discovery_all_present() {
        let dir = test_dir("discovery-present");
        write_file(&dir, "math/ternary.toml", "[t]\nv = 1");

        let manifest: IndexManifest = toml::from_str(
            r#"
[[systems]]
name = "math"
path = "math"
order = 0
[[systems.specs]]
file = "ternary.toml"
key = "t"
"#,
        )
        .unwrap();

        let result = compare_manifest_to_disk(&dir, &manifest);
        assert!(result.valid);
        assert!(result.missing.is_empty());

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_discovery_missing_file() {
        let dir = test_dir("discovery-missing");
        fs::create_dir_all(dir.join("math")).unwrap();
        // Don't create the file

        let manifest: IndexManifest = toml::from_str(
            r#"
[[systems]]
name = "math"
path = "math"
order = 0
[[systems.specs]]
file = "ternary.toml"
key = "t"
"#,
        )
        .unwrap();

        let result = compare_manifest_to_disk(&dir, &manifest);
        assert!(!result.valid);
        assert_eq!(result.missing.len(), 1);
        assert!(result.missing[0].contains("ternary.toml"));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_discovery_unexpected_file() {
        let dir = test_dir("discovery-unexpected");
        write_file(&dir, "math/ternary.toml", "[t]\nv = 1");
        write_file(&dir, "math/extra.toml", "[e]\nv = 2");

        let manifest: IndexManifest = toml::from_str(
            r#"
[[systems]]
name = "math"
path = "math"
order = 0
[[systems.specs]]
file = "ternary.toml"
key = "t"
"#,
        )
        .unwrap();

        let result = compare_manifest_to_disk(&dir, &manifest);
        assert!(result.valid); // unexpected is WARNING, not error
        assert_eq!(result.unexpected.len(), 1);
        assert!(result.unexpected[0].contains("extra.toml"));

        let _ = fs::remove_dir_all(&dir);
    }
}
