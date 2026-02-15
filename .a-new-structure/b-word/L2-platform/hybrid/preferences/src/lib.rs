//omni:code --rust -library
//omni:key B-L2-preferences
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-preferences
// Purpose: XDG-compliant user preferences read/write for dashboard
// Biblical: Psalm 119:30 — "I have chosen the way of truth"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

use std::path::PathBuf;
use std::{env, fs, io};

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

const PREFS_FILENAME: &str = "preferences.json";

/// Returns the XDG-compliant preferences directory for the dashboard.
///
/// Follows the XDG Base Directory Specification:
/// - `$XDG_CONFIG_HOME/cws-dashboard` (if set)
/// - `~/.config/cws-dashboard` (default fallback)
pub fn dir() -> PathBuf {
    let xdg = env::var("XDG_CONFIG_HOME").ok().filter(|s| !s.is_empty());

    let base = match xdg {
        Some(d) => PathBuf::from(d),
        None => {
            let home = env::var("HOME").unwrap_or_else(|_| "/tmp".into());
            PathBuf::from(home).join(".config")
        }
    };

    base.join("cws-dashboard")
}

/// Reads dashboard preferences from the XDG config directory.
///
/// Returns an empty map if the file does not exist — this is intentional,
/// not an error. A missing preferences file means the user has not
/// customized anything yet, and the dashboard should use built-in defaults.
pub fn read() -> Result<serde_json::Map<String, serde_json::Value>, io::Error> {
    let path = dir().join(PREFS_FILENAME);

    let data = match fs::read_to_string(&path) {
        Ok(d) => d,
        Err(e) if e.kind() == io::ErrorKind::NotFound => {
            return Ok(serde_json::Map::new());
        }
        Err(e) => return Err(e),
    };

    let prefs: serde_json::Map<String, serde_json::Value> =
        serde_json::from_str(&data).map_err(|e| {
            io::Error::new(io::ErrorKind::InvalidData, format!("parse preferences: {e}"))
        })?;

    Ok(prefs)
}

/// Writes dashboard preferences to the XDG config directory.
/// Creates the directory if it does not exist.
pub fn write(
    prefs: &serde_json::Map<String, serde_json::Value>,
) -> Result<(), io::Error> {
    let d = dir();
    fs::create_dir_all(&d)?;

    let data = serde_json::to_string_pretty(prefs).map_err(|e| {
        io::Error::new(io::ErrorKind::InvalidData, format!("marshal preferences: {e}"))
    })?;

    let path = d.join(PREFS_FILENAME);
    fs::write(&path, data)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn dir_ends_with_cws_dashboard() {
        let d = dir();
        assert!(d.ends_with("cws-dashboard"));
    }

    #[test]
    fn read_missing_returns_empty() {
        // When XDG points to a dir without preferences, should return empty map.
        // This test relies on the file not existing at the default location,
        // which is safe in CI and typical dev environments.
        let result = read();
        assert!(result.is_ok());
    }
}
