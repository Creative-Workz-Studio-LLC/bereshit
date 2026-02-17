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
