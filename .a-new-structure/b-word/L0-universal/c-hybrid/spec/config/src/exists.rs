// #!omni code --rust -module->utility
//omni:key B-L0-hybrid-config-exists
//omni:code --rust -module->utility
//omni:version b-03.00
//! Filesystem predicates — single source of truth for existence checks.
//!
//! Four functions covering the three questions every loader asks:
//! - **Does it exist?** [`file_exists`] — any filesystem entry (file, dir, symlink)
//! - **What kind?** [`dir_exists`], [`is_file`] — type-specific checks
//! - **Make it exist.** [`ensure_dir`] — idempotent directory creation
//!
//! Ported from Go `exists.go` (b-01.00). The Go version used `os.Stat` directly;
//! the Rust version delegates to `std::path::Path` methods for the predicates and
//! uses `fs::metadata` for `ensure_dir` where we need to distinguish error kinds.
//!
//! # Why wrappers over `Path` methods?
//!
//! These exist for three reasons:
//! 1. **Single import** — `config::file_exists(p)` vs importing `std::path::Path`
//!    and calling methods that live on a different type than you're building with
//! 2. **Go parity** — the config crate's API mirrors the Go loader surface,
//!    making cross-language port verification straightforward
//! 3. **Extension point** — if we ever need logging, caching, or permission
//!    auditing on existence checks, one place to add it
//!
//! Psalm 139:7-8 — "Whither shall I go from thy spirit?"
//! Existence checks. Simple. Honest. Is it there or not?

// ============================================================================
// METADATA BLOCK [METADATA]
// ============================================================================
//
// Key:     B-L0-hybrid-config-exists
// Purpose: Filesystem predicates — single source of truth for existence checks.
//

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode identity for this module.
pub static PRAGMA: &[(&str, &str)] = &[
    // I1: Core
    ("I1.key",       "B-L0-hybrid-config-exists"),
    ("I1.format",    "rust"),
    ("I1.from",      "seed/code/L0/rust/module.rs"),
    ("I1.at",        "b-03.00"),
    // I2: Family
    ("I2.type",      "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype",   "module"),
    ("I2.role",      "utility"),
    // I3: Instance
    ("I3.file",      "exists.rs"),
    ("I3.title",     "Filesystem predicates"),
    ("I3.component", "Config crate filesystem predicates"),
    ("I3.path",      "L0-universal/c-hybrid/spec/config/src/exists.rs"),
    ("I3.provides",  "file_exists, dir_exists, is_file, ensure_dir"),
    ("I3.brief",     "Single source of truth for existence checks across the config crate."),
    // I4: Architecture
    ("I4.layer",     "L0"),
    ("I4.position",  "universal"),
    ("I4.pattern",   "utility module"),
];

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

/// OmniCode context for this module.
pub static METADATA: &[(&str, &str)] = &[
    // C1: State
    ("C1.version",           "b-03.00"),
    ("C1.status",            "Active"),
    ("C1.created",           "2026-02-16"),
    ("C1.updated",           "2026-02-20"),
    // C2: Attribution
    ("C2.architect",         "Seanje Lenox-Wise"),
    ("C2.implementation",    "Nova Dawn (CPI-SI)"),
    ("C2.organization",      "CreativeWorkzStudio LLC"),
    ("C2.copyright",         "CreativeWorkzStudio LLC"),
    // C3: Grounding
    ("C3.scripture",         "Psalm 139:7-8"),
    ("C3.principle",         "Existence checks — honest truth about what the filesystem contains."),
    ("C3.anchor",            "Genesis 1:1"),
    // C4: Dependencies
    ("C4.requires.stdlib",   "std::fs, std::io, std::path"),
    ("C4.requires.external", "none"),
    ("C4.requires.internal", "none"),
    ("C4.consumers",         "lib.rs, loader.rs, discovery.rs"),
    ("C4.integration",       "Re-exported through lib.rs public API"),
    ("C4.if_missing",        "Config crate loses filesystem predicate layer — callers fall back to raw Path methods"),
    // C5: Intent
    ("C5.purpose",           "Single source of truth for filesystem existence checks across the config crate"),
    ("C5.philosophy",        "Wrap standard library with crate-level semantics; one import, one concern"),
    // C6: Roadmap
    ("C6.current",           "b-03.00 — Production-grade with 4 predicates"),
    ("C6.planned",           "symlink_exists, is_executable, ensure_dir_with_perms"),
    ("C6.limitations",       "is_file stricter than Go counterpart; no permission checking; default umask perms"),
    // C7: Classification
    ("C7.tags",              "filesystem, existence, predicate, utility, config"),
    ("C7.category",          "Infrastructure"),
    ("C7.domain",            "bereshit"),
    ("C7.paradigm",          "CPI-SI"),
];

// ============================================================================
// END METADATA [END]
// ============================================================================

// ============================================================================
// SETUP BLOCK [SETUP]
// ============================================================================
//
// SETUP makes things EXIST. BODY makes things HAPPEN.
//
// Section order (dependency chain — each layer uses only what's above):
//
//     1. Imports           — What this file depends on
//     2. Modules           — RESERVED
//     3. Constants         — Compile-time fixed values
//     4. Statics           — Runtime-initialized fixed values
//     5. Type Aliases      — Shorthand for complex signatures
//     6. Error Types       — enum + Display + Error + From impls
//     7. Core Types        — struct/enum + derives + completing trait impls
//     8. Trait Defs        — Behavioral contracts (shape, not fulfillment)
//     9. Macros            — RESERVED
//   10. Feature Gates     — RESERVED

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use std::fs;
use std::io;
use std::path::Path;

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// Available (not needed in this module):
//   Constants, Statics, Type Aliases, Error Types, Core Types, Trait Defs
//
// Reserved (structural — not used in module form):
//   Modules       — Submodule declarations belong in lib.rs or parent mod.rs.
//   Macros        — Macro definitions belong in dedicated macro modules or crate root.
//   Feature Gates — Feature gates are crate-level configuration in Cargo.toml.

// ============================================================================
// END SETUP [END]
// ============================================================================

// ============================================================================
// BODY BLOCK [BODY]
// ============================================================================
//
// BODY makes things HAPPEN. SETUP made things EXIST.
//
// Subsection order follows the type lifecycle — from identity through
// creation, operation, observation, to output.
//
//     1. Identity Access   — OmniCode static accessor functions
//     2. Trait Implementations — Fulfilling contracts from SETUP
//     3. Constructors & Builders — new(), builders, typestate transitions
//     4. Core Logic        — Primary operations, state transforms
//     5. Queries & Accessors — Read-only &self methods
//     6. Output & Display  — RESERVED
//     7. Free Functions    — Module-level public utilities
//     8. Helpers           — RESERVED
//     9. Constructor Tests — RESERVED
//   10. Builder Tests     — RESERVED
//   11. Transition Tests  — RESERVED
//   12. Query Tests       — RESERVED
//   13. Identity Tests    — RESERVED
//   14. Error Tests       — RESERVED
//   15. Edge Case Tests   — RESERVED
//   16. Table Driven Tests — RESERVED
//   17. Integration Tests — RESERVED

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────

/// Returns the full PRAGMA identity slice for this module.
pub fn pragma() -> &'static [(&'static str, &'static str)] {
    &PRAGMA
}

/// Looks up a PRAGMA key, returning its value or `None`.
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

/// Returns the full METADATA context slice for this module.
pub fn metadata() -> &'static [(&'static str, &'static str)] {
    &METADATA
}

/// Looks up a METADATA key, returning its value or `None`.
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────

/// Returns `true` if any filesystem entry exists at `path`.
///
/// Checks existence only — does not distinguish files from directories,
/// symlinks, or special entries. Returns `false` for **any** stat failure
/// including permission denied and broken symlinks.
///
/// Despite the name, this is a general existence check, not file-specific.
/// Named `file_exists` for Go API parity (`util.FileExists`).
/// Use [`is_file`] to check specifically for regular files.
///
/// # Examples
///
/// ```no_run
/// # use std::path::Path;
/// # use bereshit_l0_config::exists::file_exists;
/// assert!(file_exists(Path::new("/tmp")));       // directory
/// assert!(!file_exists(Path::new("/no/such")));   // missing
/// ```
pub fn file_exists(path: &Path) -> bool {
    path.exists()
}

/// Returns `true` if `path` exists **and** is a directory.
///
/// Follows symlinks — a symlink pointing to a directory returns `true`.
/// Returns `false` if the path does not exist or is not a directory.
///
/// # Examples
///
/// ```no_run
/// # use std::path::Path;
/// # use bereshit_l0_config::exists::dir_exists;
/// assert!(dir_exists(Path::new("/tmp")));
/// assert!(!dir_exists(Path::new("/tmp/.bashrc")));
/// ```
pub fn dir_exists(path: &Path) -> bool {
    path.is_dir()
}

/// Returns `true` if `path` exists **and** is a regular file.
///
/// Returns `false` for directories, symlinks (even to files), sockets,
/// pipes, and missing paths. This differs from the Go counterpart which
/// returns `true` for anything that is not a directory — the Rust version
/// is stricter, matching `std::fs::FileType::is_file()` semantics.
///
/// # Examples
///
/// ```no_run
/// # use std::path::Path;
/// # use bereshit_l0_config::exists::is_file;
/// assert!(!is_file(Path::new("/tmp")));          // directory → false
/// assert!(!is_file(Path::new("/no/such")));       // missing → false
/// ```
pub fn is_file(path: &Path) -> bool {
    path.is_file()
}

/// Creates the directory at `path` and all missing parents.
///
/// Behavior:
/// - Path is a directory: no-op, returns `Ok(())`
/// - Path does not exist: creates it (and parents) with default permissions
/// - Path exists as a file: returns `AlreadyExists` error
/// - Stat fails for other reasons (permission denied, I/O): propagates the
///   original error rather than masking it with a `create_dir_all` failure
///
/// # Errors
///
/// - [`io::ErrorKind::AlreadyExists`] if `path` exists but is not a directory
/// - Any [`io::Error`] from `fs::metadata` (permission denied, etc.)
/// - Any [`io::Error`] from `fs::create_dir_all` (creation failure)
///
/// # Examples
///
/// ```no_run
/// # use std::path::Path;
/// # use bereshit_l0_config::exists::ensure_dir;
/// ensure_dir(Path::new("/tmp/my-app/cache/specs")).unwrap();
/// // Safe to call again — idempotent.
/// ensure_dir(Path::new("/tmp/my-app/cache/specs")).unwrap();
/// ```
pub fn ensure_dir(path: &Path) -> io::Result<()> {
    match fs::metadata(path) {
        Ok(meta) if meta.is_dir() => Ok(()),
        Ok(_) => Err(io::Error::new(
            io::ErrorKind::AlreadyExists,
            format!("path exists but is not a directory: {}", path.display()),
        )),
        // NotFound is the expected case — create the directory tree.
        Err(e) if e.kind() == io::ErrorKind::NotFound => fs::create_dir_all(path),
        // Permission denied, I/O error, etc. — propagate as-is rather than
        // masking with a potentially misleading create_dir_all error.
        Err(e) => Err(e),
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// Available (not needed in this module):
//   Org Chart               — Four functions, one concern; structure is self-evident
//   Trait Implementations   — No types implementing traits
//   Constructors & Builders — No types requiring construction
//   Core Logic              — Stateless utility functions live in Free Functions
//   Core Operations         — Same as Core Logic; no stateful operations
//   Queries & Accessors     — No types with observable state
//   Error Handling          — Uses std::io::Error directly; no custom error types
//   Public APIs             — All public functions live in Free Functions (§7)
//
// Reserved (structural — not used in module form):
//   Output & Display — Belongs to the crate's output layer; modules return data.
//   Helpers          — Serve run(); libraries and modules don't have run().
//   Tests (B9-B17)   — Test sections belong in dedicated test files.

// ============================================================================
// END BODY [END]
// ============================================================================

// ============================================================================
// CLOSING BLOCK [CLOSING]
// ============================================================================
//
// Closing ensures correctness, documents constraints, and anchors the file.
//
// Section order:
//
//     [Cv]  Closing Validation (tests)
//     [Ce]  Closing Execution (entry point or absence)
//     [Cc]  Closing Cleanup (resource teardown)
//     [X1]  Modification Policy
//     [X2]  Extension Points
//     [X3]  Troubleshooting
//     [X4]  Reference
//     [X5]  Closing Note

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;
    use crate::test_utils::{test_dir, write_file};
    use std::fs;

    // ── identity access ─────────────────────────────────────────────

    #[test]
    fn pragma_returns_non_empty_slice() {
        assert!(!pragma().is_empty());
    }

    #[test]
    fn pragma_get_finds_key() {
        assert_eq!(pragma_get("I1.key"), Some("B-L0-hybrid-config-exists"));
    }

    #[test]
    fn pragma_get_returns_none_for_missing_key() {
        assert_eq!(pragma_get("no.such.key"), None);
    }

    #[test]
    fn metadata_get_finds_version() {
        assert_eq!(metadata_get("C1.version"), Some("b-03.00"));
    }

    // ── file_exists ──────────────────────────────────────────────────

    #[test]
    fn file_exists_returns_true_for_regular_file() {
        let dir = test_dir("exists-file");
        write_file(&dir, "present.txt", "hello");
        assert!(file_exists(&dir.join("present.txt")));
        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn file_exists_returns_true_for_directory() {
        // file_exists is a general existence check — directories count.
        let dir = test_dir("exists-dir");
        assert!(file_exists(&dir));
        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn file_exists_returns_false_for_missing_path() {
        assert!(!file_exists(Path::new("/tmp/cws-no-such-path-ever")));
    }

    // ── dir_exists ───────────────────────────────────────────────────

    #[test]
    fn dir_exists_returns_true_for_directory() {
        let dir = test_dir("dir-exists");
        assert!(dir_exists(&dir));
        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn dir_exists_returns_false_for_file() {
        let dir = test_dir("dir-exists-file");
        write_file(&dir, "not-a-dir.txt", "data");
        assert!(!dir_exists(&dir.join("not-a-dir.txt")));
        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn dir_exists_returns_false_for_missing_path() {
        assert!(!dir_exists(Path::new("/tmp/cws-no-such-dir-ever")));
    }

    // ── is_file ──────────────────────────────────────────────────────

    #[test]
    fn is_file_returns_true_for_regular_file() {
        let dir = test_dir("is-file");
        write_file(&dir, "regular.txt", "data");
        assert!(is_file(&dir.join("regular.txt")));
        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn is_file_returns_false_for_directory() {
        let dir = test_dir("is-file-dir");
        assert!(!is_file(&dir));
        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn is_file_returns_false_for_missing_path() {
        assert!(!is_file(Path::new("/tmp/cws-no-such-file-ever")));
    }

    // ── ensure_dir ───────────────────────────────────────────────────

    #[test]
    fn ensure_dir_creates_nested_tree() {
        let dir = test_dir("ensure-nested");
        let nested = dir.join("a").join("b").join("c");
        ensure_dir(&nested).unwrap();
        assert!(dir_exists(&nested));
        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn ensure_dir_is_idempotent() {
        let dir = test_dir("ensure-idempotent");
        let target = dir.join("sub");
        ensure_dir(&target).unwrap();
        // Second call must succeed — no-op.
        ensure_dir(&target).unwrap();
        assert!(dir_exists(&target));
        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn ensure_dir_fails_when_path_is_file() {
        let dir = test_dir("ensure-conflict");
        write_file(&dir, "blocker.txt", "x");
        let result = ensure_dir(&dir.join("blocker.txt"));
        assert!(result.is_err());
        let err = result.unwrap_err();
        assert_eq!(err.kind(), io::ErrorKind::AlreadyExists);
        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn ensure_dir_fails_when_parent_is_file() {
        // If an intermediate path component is a file, create_dir_all
        // fails with a system error (not our AlreadyExists).
        let dir = test_dir("ensure-parent-file");
        write_file(&dir, "blocker", "x");
        let result = ensure_dir(&dir.join("blocker").join("child"));
        assert!(result.is_err());
        let _ = fs::remove_dir_all(&dir);
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────

// No entry point — this is a module.

// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// Available (not needed in this module):
//   Cc (Cleanup) — No resources requiring explicit teardown
//
// Reserved (structural — not used in module form):
//   Test Coverage Summary — Belongs in demo-test form files.

// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Don't move the ancient boundary stone, which thy fathers have set."
//     — Proverbs 22:28 (WEB)
//
// safe: Function bodies, New predicate functions, Doc comments and examples, Tests in Cv
//   - Function bodies (implementation details behind stable signatures)
//   - New predicate functions (additive — extends the API without breaking it)
//   - Doc comments and examples (improves documentation quality)
//   - Tests in Cv (improves coverage and confidence)
//
// careful: ensure_dir error types, Error message wording, Function return types, Identity accessor signatures
//   - ensure_dir error types — callers may match on io::ErrorKind::AlreadyExists
//   - Error message wording — callers may include messages in logs or user output
//   - Function return types — changing bool → Result would break all call sites
//   - Identity accessor signatures — if lib.rs re-exports them
//
// never: 4-block structure, PRAGMA/METADATA layout, Public function signatures, ensure_dir error distinction
//   - 4-block structure or block boundary markers
//   - PRAGMA/METADATA identity statics layout (compiled into binary, queried at runtime)
//   - file_exists/dir_exists/is_file function signatures (lib.rs re-exports these)
//   - ensure_dir(&Path) -> io::Result<()> signature (public API contract)
//   - The distinction between NotFound and other errors in ensure_dir

// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Planned:
//   - symlink_exists(path) — follow/no-follow distinction for symlink-aware checks
//   - is_executable(path) — permission-aware predicate (Unix: mode & 0o111)
//   - ensure_dir_with_perms(path, mode) — explicit permission control (Go used 0755)
//
// Known Limitations:
//   - is_file returns false for symlinks — stricter than Go counterpart
//   - No permission checking — file_exists returns false for permission-denied paths
//   - ensure_dir uses default permissions (0o777 before umask, not explicit 0755)

// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// file_exists returns false but file is there:
//   → Check permissions — file_exists returns false for any stat failure
//   → Check symlinks — broken symlinks return false
//
// is_file returns false for a file:
//   → Check if it's a symlink — is_file returns false for the symlink entry
//     itself. Use file_exists + !dir_exists for Go-equivalent behavior.
//
// ensure_dir fails with NotADirectory:
//   → An intermediate path component is a regular file, not a directory.
//     Remove or rename the blocking file before creating the directory tree.
//
// ensure_dir fails with AlreadyExists:
//   → The exact path exists as a regular file (not a directory).
//     This is our custom error — the path must be removed before creating.

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Dependencies:
//   stdlib: std::fs, std::io, std::path::Path
//   crate:  (none — this module has no intra-crate dependencies)
//
// Dependents:
//   lib.rs       — re-exports file_exists, dir_exists, is_file, ensure_dir
//   loader.rs    — uses file_exists for manifest path validation
//   discovery.rs — uses dir_exists for filesystem tripwire comparison
//
// Template:    seed/code/L0/rust/module.rs
// Schema:      schemas/code/forms/declared/rust-module.jsonc
// Ported from: go/util/exists.go (Go counterpart, b-01.00)
//
// Validation:
//   cargo check  -p bereshit-l0-config
//   cargo test   -p bereshit-l0-config
//   cargo clippy -p bereshit-l0-config
//   cws-struct lint rust exists.rs

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// scripture: "Psalm 139:7-8 — Whither shall I go from thy spirit?"
//
// "Whither shall I go from thy spirit? or whither shall I flee from
//  thy presence? If I ascend up into heaven, thou art there: if I
//  make my bed in hell, behold, thou art there."
//     — Psalm 139:7-8 (KJV)
//
// note: "exists.rs — production-grade filesystem predicates for the config crate."
//
// Existence checks. The simplest question a program can ask: is it there
// or not? No parsing, no interpretation, no transformation — just honest
// truth about what the filesystem contains. Four functions, three questions,
// one concern. The module does its one thing with excellence.
//
// Ported from Go exists.go (b-01.00), refined for Rust idioms. The Go
// version used os.Stat directly; the Rust version delegates to Path methods
// for predicates and distinguishes error kinds in ensure_dir rather than
// treating all stat failures as "not found."

// ============================================================================
// END CLOSING [END]
// ============================================================================
