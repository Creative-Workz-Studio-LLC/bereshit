// #!omni code --rust -module
//omni:key B-L0-hybrid-config-exists
//omni:code --rust -module
//omni:version b-03.00
//! Filesystem predicates — single source of truth for existence checks.
//!
//! Ported from Go `exists.go`. Replaces duplicated `file_exists()` across loaders.
//!
//! Psalm 139:7-8 — "Whither shall I go from thy spirit?"
//! Existence checks. Simple. Honest. Is it there or not?

use std::fs;
use std::io;
use std::path::Path;

// ────────────────────────────────────────────────────────────────
// Existence Checks
// ────────────────────────────────────────────────────────────────

/// Returns true if a file or directory exists at `path`.
/// Returns false for ANY stat error — permission denied, broken symlink, etc.
pub fn file_exists(path: &Path) -> bool {
    path.exists()
}

/// Returns true if `path` exists AND is a directory.
pub fn dir_exists(path: &Path) -> bool {
    path.is_dir()
}

/// Returns true if `path` exists AND is a regular file (not a directory).
pub fn is_file(path: &Path) -> bool {
    path.is_file()
}

// ────────────────────────────────────────────────────────────────
// Directory Operations
// ────────────────────────────────────────────────────────────────

/// Creates the directory at `path` (and parents) if it doesn't exist.
/// No-op if the directory already exists. Returns error if path exists
/// as a file, or if creation fails.
pub fn ensure_dir(path: &Path) -> io::Result<()> {
    match fs::metadata(path) {
        Ok(meta) => {
            if meta.is_dir() {
                Ok(()) // already exists
            } else {
                Err(io::Error::new(
                    io::ErrorKind::AlreadyExists,
                    format!("path exists as a file: {}", path.display()),
                ))
            }
        }
        Err(_) => fs::create_dir_all(path),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::test_utils::{test_dir, write_file};
    use std::fs;

    #[test]
    fn test_file_exists_real_file() {
        let dir = test_dir("exists-file");
        write_file(&dir, "exists.txt", "hello");

        assert!(file_exists(&dir.join("exists.txt")));
        assert!(!file_exists(&dir.join("nope.txt")));
        // Directory also returns true (existence check, not type check)
        assert!(file_exists(&dir));

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_is_file() {
        let dir = test_dir("is-file");
        write_file(&dir, "regular.txt", "data");

        assert!(is_file(&dir.join("regular.txt")));
        assert!(!is_file(&dir)); // directory -> false
        assert!(!is_file(&dir.join("nope"))); // missing -> false

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_ensure_dir() {
        let dir = test_dir("ensure-dir");

        // Create nested directory
        let nested = dir.join("a").join("b").join("c");
        ensure_dir(&nested).unwrap();
        assert!(dir_exists(&nested));

        // No-op on existing directory
        ensure_dir(&nested).unwrap();

        // Error when path is a file
        write_file(&dir, "file.txt", "x");
        let err = ensure_dir(&dir.join("file.txt"));
        assert!(err.is_err());

        let _ = fs::remove_dir_all(&dir);
    }
}
