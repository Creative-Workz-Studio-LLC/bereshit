//omni:code --rust -library
//omni:key B-L2-builder
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-builder
// Purpose: Multi-project path resolution for CWS builder services
// Biblical: Proverbs 4:26 — "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

use std::path::{Path, PathBuf};
use std::{fs, io};

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

/// Resolved filesystem paths for the builder dashboard service.
/// All paths are absolute and verified to exist (or derivable from a verified root).
#[derive(Debug, Clone)]
pub struct Paths {
    /// Absolute path to company-docs/ root (verified via config file).
    pub project_dir: PathBuf,

    /// `build/build.config.yaml` — single source of truth for editorial config.
    pub config_path: PathBuf,

    /// `build/builder/dist/cli.js` — compiled TS builder entry point.
    pub builder_cli: PathBuf,

    /// `build/dashboard/src-web/` — SPA static files root.
    pub web_root: PathBuf,
}

/// Resolves all builder-related paths from a project directory.
///
/// The project directory must contain `build/build.config.yaml` — this is the
/// sentinel file that confirms the directory is a valid CWS project root.
pub fn resolve_paths(project_dir: &Path) -> Result<Paths, io::Error> {
    let abs = fs::canonicalize(project_dir)?;
    let config_path = abs.join("build").join("build.config.yaml");

    if !config_path.exists() {
        return Err(io::Error::new(
            io::ErrorKind::NotFound,
            format!("config not found at {}", config_path.display()),
        ));
    }

    Ok(Paths {
        config_path,
        builder_cli: abs.join("build").join("builder").join("dist").join("cli.js"),
        web_root: abs.join("build").join("dashboard").join("src-web"),
        project_dir: abs,
    })
}

/// Walks up from `start_dir` looking for `build/build.config.yaml`.
///
/// This upward-walk algorithm mirrors how tools like git find `.git/` — it
/// starts at the given directory and checks each parent until it hits the
/// filesystem root. Used for auto-detection when `--builder-dir` is not
/// specified.
pub fn find_project_dir(start_dir: &Path) -> Result<PathBuf, io::Error> {
    let mut dir = fs::canonicalize(start_dir)?;

    loop {
        let config_path = dir.join("build").join("build.config.yaml");
        if config_path.exists() {
            return Ok(dir);
        }

        let Some(parent) = dir.parent() else {
            return Err(io::Error::new(
                io::ErrorKind::NotFound,
                format!(
                    "could not find build/build.config.yaml walking up from {}",
                    start_dir.display()
                ),
            ));
        };

        if parent == dir {
            return Err(io::Error::new(
                io::ErrorKind::NotFound,
                format!(
                    "could not find build/build.config.yaml walking up from {}",
                    start_dir.display()
                ),
            ));
        }

        dir = parent.to_path_buf();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn resolve_paths_missing_config() {
        let result = resolve_paths(Path::new("/tmp"));
        assert!(result.is_err());
    }

    #[test]
    fn find_project_dir_from_nonexistent() {
        let result = find_project_dir(Path::new("/nonexistent/path"));
        assert!(result.is_err());
    }
}
