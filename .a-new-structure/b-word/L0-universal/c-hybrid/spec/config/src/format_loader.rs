//! Format-aware config loading — TOML, JSONC, with extension dispatch.
//!
//! Ported from Go `loader.go` (util). Dispatches by file extension:
//!   `.toml`         → TOML parser
//!   `.json`, `.jsonc` → JSONC parser (strips comments)
//!
//! Ladder: load_config_file → format detection (formats.rs) → parser (toml or jsonc.rs)
//! Baton:  find_and_load → find_config → load_toml_file → parsed result
//!
//! Proverbs 3:5-6 — "In all thy ways acknowledge him, and he shall direct thy paths."
//! The loader acknowledges each file's format before trying to parse it.

use std::env;
use std::fs;
use std::path::{Path, PathBuf};

use bereshit_l0_paths as paths;
use serde::de::DeserializeOwned;

use crate::error::ConfigError;
use crate::exists;
use crate::jsonc::strip_jsonc_comments;

// ────────────────────────────────────────────────────────────────
// JSON → TOML Conversion
// ────────────────────────────────────────────────────────────────

/// Convert a serde_json::Value into a toml::Value.
///
/// Enables `load_config_file` to return `toml::Table` regardless of source format.
fn json_to_toml(val: serde_json::Value) -> toml::Value {
    match val {
        serde_json::Value::Null => toml::Value::String(String::new()),
        serde_json::Value::Bool(b) => toml::Value::Boolean(b),
        serde_json::Value::Number(n) => {
            if let Some(i) = n.as_i64() {
                toml::Value::Integer(i)
            } else if let Some(f) = n.as_f64() {
                toml::Value::Float(f)
            } else {
                toml::Value::String(n.to_string())
            }
        }
        serde_json::Value::String(s) => toml::Value::String(s),
        serde_json::Value::Array(arr) => {
            toml::Value::Array(arr.into_iter().map(json_to_toml).collect())
        }
        serde_json::Value::Object(map) => {
            let mut table = toml::map::Map::new();
            for (k, v) in map {
                table.insert(k, json_to_toml(v));
            }
            toml::Value::Table(table)
        }
    }
}

/// Convert a serde_json::Value (expected Object) into a toml::Table.
fn json_value_to_table(val: serde_json::Value) -> Result<toml::Table, ConfigError> {
    match json_to_toml(val) {
        toml::Value::Table(t) => Ok(t),
        _ => Err(ConfigError::Load {
            file: String::new(),
            op: "convert".to_owned(),
            source: "JSON root must be an object".to_owned(),
        }),
    }
}

// ────────────────────────────────────────────────────────────────
// Format-Aware Loading
// ────────────────────────────────────────────────────────────────

/// Load a config file into a `toml::Table`, dispatching by extension.
///
/// Supports `.toml`, `.json`, and `.jsonc`. Returns `toml::Table` regardless
/// of source format (JSON values are converted to TOML equivalents).
///
/// This is the primary entry point for format-agnostic config loading.
/// Use [`load_toml_file`] when you know the format at compile time.
pub fn load_config_file(path: &Path) -> Result<toml::Table, ConfigError> {
    let name = path
        .file_name()
        .map(|n| n.to_string_lossy().into_owned())
        .unwrap_or_default();

    if !exists::file_exists(path) {
        return Err(ConfigError::Load {
            file: name,
            op: "stat".to_owned(),
            source: "file not found".to_owned(),
        });
    }

    let ext = path
        .extension()
        .and_then(|e| e.to_str())
        .map(|e| format!(".{e}"))
        .unwrap_or_default();

    match ext.as_str() {
        ".toml" => {
            let content = fs::read_to_string(path).map_err(|e| ConfigError::Load {
                file: name.clone(),
                op: "read".to_owned(),
                source: e.to_string(),
            })?;
            toml::from_str(&content).map_err(|e| ConfigError::Load {
                file: name,
                op: "parse".to_owned(),
                source: format!("toml: {e}"),
            })
        }

        ".json" | ".jsonc" => {
            let content = fs::read_to_string(path).map_err(|e| ConfigError::Load {
                file: name.clone(),
                op: "read".to_owned(),
                source: e.to_string(),
            })?;
            let clean = strip_jsonc_comments(&content);
            let json_val: serde_json::Value =
                serde_json::from_str(&clean).map_err(|e| ConfigError::Load {
                    file: name.clone(),
                    op: "parse".to_owned(),
                    source: e.to_string(),
                })?;
            json_value_to_table(json_val).map_err(|mut e| {
                if let ConfigError::Load { file, .. } = &mut e {
                    *file = name;
                }
                e
            })
        }

        _ => Err(ConfigError::Load {
            file: name,
            op: "detect".to_owned(),
            source: format!("unsupported config format {ext:?} — expected .toml, .json, or .jsonc"),
        }),
    }
}

// ────────────────────────────────────────────────────────────────
// TOML Loading
// ────────────────────────────────────────────────────────────────

/// Read a TOML file and deserialize into a typed result.
///
/// Generic companion to [`crate::parse_jsonc`] — same pattern, TOML format.
///
/// ```no_run
/// use bereshit_l0_config::load_toml_file;
/// use serde::Deserialize;
///
/// #[derive(Deserialize)]
/// struct Config { name: String }
///
/// let cfg: Config = load_toml_file(std::path::Path::new("config.toml")).unwrap();
/// ```
pub fn load_toml_file<T: DeserializeOwned>(path: &Path) -> Result<T, ConfigError> {
    let name = path
        .file_name()
        .map(|n| n.to_string_lossy().into_owned())
        .unwrap_or_default();

    if !exists::file_exists(path) {
        return Err(ConfigError::Load {
            file: name,
            op: "stat".to_owned(),
            source: "file not found".to_owned(),
        });
    }

    let content = fs::read_to_string(path).map_err(|e| ConfigError::Load {
        file: name.clone(),
        op: "read".to_owned(),
        source: e.to_string(),
    })?;

    toml::from_str(&content).map_err(|e| ConfigError::Load {
        file: name,
        op: "parse".to_owned(),
        source: format!("toml: {e}"),
    })
}

/// Deserialize TOML from bytes into a typed result.
pub fn load_toml_bytes<T: DeserializeOwned>(data: &[u8]) -> Result<T, ConfigError> {
    let content = std::str::from_utf8(data).map_err(|e| ConfigError::Load {
        file: "(bytes)".to_owned(),
        op: "decode".to_owned(),
        source: e.to_string(),
    })?;

    toml::from_str(content).map_err(|e| ConfigError::Load {
        file: "(bytes)".to_owned(),
        op: "parse".to_owned(),
        source: format!("toml: {e}"),
    })
}

// ────────────────────────────────────────────────────────────────
// JSONC Loading
// ────────────────────────────────────────────────────────────────

/// Read a JSONC file and deserialize into a typed result.
///
/// Generic companion to [`load_toml_file`] — same pattern, JSONC format.
/// Strips comments and trailing commas before parsing.
///
/// L1 calls this to dispatch `.json`/`.jsonc` OmniCode config files
/// through the L0 format loader, completing the format bridge.
///
/// ```no_run
/// use bereshit_l0_config::load_jsonc_file;
/// use serde::Deserialize;
///
/// #[derive(Deserialize)]
/// struct Config { name: String }
///
/// let cfg: Config = load_jsonc_file(std::path::Path::new("config.jsonc")).unwrap();
/// ```
pub fn load_jsonc_file<T: DeserializeOwned>(path: &Path) -> Result<T, ConfigError> {
    let name = path
        .file_name()
        .map(|n| n.to_string_lossy().into_owned())
        .unwrap_or_default();

    if !exists::file_exists(path) {
        return Err(ConfigError::Load {
            file: name,
            op: "stat".to_owned(),
            source: "file not found".to_owned(),
        });
    }

    let content = fs::read_to_string(path).map_err(|e| ConfigError::Load {
        file: name.clone(),
        op: "read".to_owned(),
        source: e.to_string(),
    })?;

    let clean = strip_jsonc_comments(&content);
    serde_json::from_str(&clean).map_err(|e| ConfigError::Load {
        file: name,
        op: "parse".to_owned(),
        source: format!("jsonc: {e}"),
    })
}

// ────────────────────────────────────────────────────────────────
// Config Discovery
// ────────────────────────────────────────────────────────────────

/// Search for a config file and load it into a typed struct.
///
/// `config_name` is the file name (e.g., `"template-config.toml"`).
/// `search_paths` are directories to search in order.
///
/// Currently TOML-only for struct decoding. For format-agnostic map loading,
/// use [`find_config`] + [`load_config_file`].
pub fn find_and_load<T: DeserializeOwned>(
    config_name: &str,
    search_paths: &[PathBuf],
) -> Result<T, ConfigError> {
    let path = find_config(config_name, search_paths)?;
    load_toml_file(&path)
}

/// Search for a config file in multiple locations.
/// Returns the first path where the file exists.
pub fn find_config(config_name: &str, search_paths: &[PathBuf]) -> Result<PathBuf, ConfigError> {
    for dir in search_paths {
        let path = dir.join(config_name);
        if exists::file_exists(&path) {
            return Ok(path);
        }
    }
    Err(ConfigError::Load {
        file: config_name.to_owned(),
        op: "find".to_owned(),
        source: format!("not found in {} search paths", search_paths.len()),
    })
}

/// Search for a config directory containing the given file.
/// Returns the directory path, not the file path.
pub fn find_config_dir(
    config_name: &str,
    search_paths: &[PathBuf],
) -> Result<PathBuf, ConfigError> {
    for dir in search_paths {
        let path = dir.join(config_name);
        if exists::file_exists(&path) {
            return Ok(dir.clone());
        }
    }
    Err(ConfigError::Load {
        file: config_name.to_owned(),
        op: "find".to_owned(),
        source: format!("directory not found in {} search paths", search_paths.len()),
    })
}

// ────────────────────────────────────────────────────────────────
// Search Path Builders
// ────────────────────────────────────────────────────────────────

/// Returns standard search paths for skill configs.
///
/// `skill_name` is the skill folder name (e.g., `"create-from-template"`).
/// Delegates to the paths crate for home/claude resolution — single source of truth.
pub fn skill_config_paths(skill_name: &str) -> Vec<PathBuf> {
    let mut result = Vec::new();

    // Check CLAUDE_PLUGIN_ROOT first (highest priority)
    if let Ok(root) = env::var("CLAUDE_PLUGIN_ROOT") {
        result.push(
            PathBuf::from(root)
                .join("skills")
                .join(skill_name)
                .join("config"),
        );
    }

    // Relative paths
    result.extend([
        PathBuf::from("config"),
        PathBuf::from("../config"),
        PathBuf::from("../../config"),
        PathBuf::from("claude-global")
            .join("skills")
            .join(skill_name)
            .join("config"),
    ]);

    // Paths relative to executable
    if let Ok(exe) = env::current_exe() {
        if let Some(exe_dir) = exe.parent() {
            result.push(exe_dir.join("..").join("..").join("config"));
            result.push(exe_dir.join("..").join("config"));
        }
    }

    // Home-based paths — delegate to paths crate (single source of truth)
    let claude_home = paths::claude_home();
    result.push(
        claude_home
            .join("cpi-si")
            .join("skills")
            .join(skill_name)
            .join("config"),
    );
    result.push(claude_home.join("skills").join(skill_name).join("config"));

    result
}

/// Returns standard search paths for system configs.
///
/// Delegates to the paths crate for home/claude resolution.
pub fn system_config_paths() -> Vec<PathBuf> {
    let claude_home = paths::claude_home();

    vec![
        PathBuf::from("config"),
        PathBuf::from("../config"),
        claude_home.join("cpi-si").join("system").join("config"),
        claude_home.join("system").join("config"),
    ]
}
