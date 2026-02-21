//! Shared test utilities for the config crate.
//!
//! Provides temp directory creation, file writing, and standard TOML
//! fixtures used across module tests.

//omni:code --rust -library
//omni:key B-L0-hybrid-config-test-utils
//omni:version b-03.00

use std::fs;
use std::path::{Path, PathBuf};

/// Create a unique temp directory for test isolation.
pub(crate) fn test_dir(name: &str) -> PathBuf {
    let ts = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap()
        .as_nanos();
    let dir = std::env::temp_dir().join(format!("bereshit-config-test-{name}-{ts}"));
    fs::create_dir_all(&dir).unwrap();
    dir
}

/// Write a file inside a directory, creating parent dirs as needed.
pub(crate) fn write_file(root: &Path, rel: &str, content: &str) {
    let path = root.join(rel);
    fs::create_dir_all(path.parent().unwrap()).unwrap();
    fs::write(path, content).unwrap();
}

/// Minimal index.toml for testing.
pub(crate) const TEST_INDEX: &str = r#"
[[systems]]
name = "math"
path = "L0-universal/ladder/foundation/math"
order = 0

[[systems.specs]]
file = "ternary.toml"
key = "B-L0-math-ternary"
depends_on = []

[[systems]]
name = "types"
path = "L0-universal/ladder/foundation/types"
order = 1

[[systems.specs]]
file = "primitives.toml"
key = "B-L0-types-primitives"
depends_on = ["L0-universal/ladder/foundation/math/ternary.toml"]
"#;

/// Minimal TOML spec with identity tables.
pub(crate) const TEST_SPEC_WITH_IDENTITY: &str = r#"
[_pragma]
"P1.key" = "B-L0-math-ternary"
"P1.type" = "data"
"P4.at" = "b-01.00"

[_metadata]
"M1.key" = "B-L0-math-ternary"
"M1.created" = "2026-01-15"

[ternary]
base = 3
description = "balanced ternary"
"#;

/// Minimal TOML spec without identity tables.
pub(crate) const TEST_SPEC_PLAIN: &str = r#"
[primitives]
int9_min = -4
int9_max = 4
"#;
