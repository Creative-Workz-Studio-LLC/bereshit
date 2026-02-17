//omni:code --rust -library
//omni:key B-L0-foundation-session-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # Session Management (Rust)
//!
//! Universal session tracking — ID, paths, logs, duration.
//!
//! Promoted from L3-cpisi to L0-universal. Any layer can track session
//! state; this is not CPI-SI specific.
//!
//! # Types
//!
//! - `Session` — current session metadata (ID, start time, compactions)
//! - `SessionLog` — session log with typed entries
//! - `SessionPaths` — resolved paths to session files

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};
use std::path::PathBuf;
use std::time::{Duration, SystemTime};

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────

/// Current session metadata.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Session {
    pub session_id: String,
    pub start_time: String,
    #[serde(default)]
    pub compactions: u32,
}

/// A single log entry.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LogEntry {
    pub timestamp: String,
    /// Entry type: "task", "note", "tool", "milestone"
    #[serde(rename = "type")]
    pub entry_type: String,
    pub content: String,
}

/// Session log containing entries.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SessionLog {
    pub session_id: String,
    pub entries: Vec<LogEntry>,
}

/// Resolved paths to session files.
#[derive(Debug, Clone)]
pub struct SessionPaths {
    pub session_dir: PathBuf,
    pub current_session: PathBuf,
    pub session_log: PathBuf,
    pub pattern_file: PathBuf,
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 SessionPaths
// ────────────────────────────────────────────────────────────────

impl SessionPaths {
    /// Default paths under ~/.claude/session/
    pub fn default_paths() -> Self {
        let home = dirs_or_home();
        let session_dir = home.join(".claude").join("session");
        Self::from_dir(session_dir)
    }

    /// Custom session directory (absolute path).
    pub fn from_dir(session_dir: PathBuf) -> Self {
        Self {
            current_session: session_dir.join("current.json"),
            session_log: session_dir.join("current-log.json"),
            pattern_file: session_dir.join("patterns.json"),
            session_dir,
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Session Loading
// ────────────────────────────────────────────────────────────────

impl Session {
    /// Load current session from the given paths.
    pub fn load(paths: &SessionPaths) -> Result<Self, std::io::Error> {
        let data = std::fs::read_to_string(&paths.current_session)?;
        serde_json::from_str(&data).map_err(|e| {
            std::io::Error::new(std::io::ErrorKind::InvalidData, e)
        })
    }

    /// Load from default paths.
    pub fn load_default() -> Result<Self, std::io::Error> {
        Self::load(&SessionPaths::default_paths())
    }
}

impl SessionLog {
    /// Load session log from the given paths.
    pub fn load(paths: &SessionPaths) -> Result<Self, std::io::Error> {
        let data = std::fs::read_to_string(&paths.session_log)?;
        serde_json::from_str(&data).map_err(|e| {
            std::io::Error::new(std::io::ErrorKind::InvalidData, e)
        })
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 Duration Helpers
// ────────────────────────────────────────────────────────────────

/// Format a duration as human-readable ("2h 15m" or "45 minutes").
pub fn format_duration(d: Duration) -> String {
    let total_secs = d.as_secs();
    let hours = total_secs / 3600;
    let minutes = (total_secs % 3600) / 60;

    if hours > 0 {
        format!("{hours}h {minutes}m")
    } else {
        format!("{minutes} minutes")
    }
}

/// Format elapsed time since a given SystemTime.
pub fn format_elapsed(since: SystemTime) -> String {
    match since.elapsed() {
        Ok(d) => format_duration(d),
        Err(_) => "unknown".into(),
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 Helpers
// ────────────────────────────────────────────────────────────────

/// Get home directory, falling back to /tmp if unavailable.
fn dirs_or_home() -> PathBuf {
    std::env::var("HOME")
        .map(PathBuf::from)
        .unwrap_or_else(|_| PathBuf::from("/tmp"))
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Universal session management — promoted from L3 to serve all layers.
// "To every thing there is a season" — Ecclesiastes 3:1

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn default_paths() {
        let paths = SessionPaths::default_paths();
        assert!(paths.session_dir.ends_with("session"));
        assert!(paths.current_session.ends_with("current.json"));
    }

    #[test]
    fn format_duration_minutes() {
        let d = Duration::from_secs(45 * 60);
        assert_eq!(format_duration(d), "45 minutes");
    }

    #[test]
    fn format_duration_hours() {
        let d = Duration::from_secs(2 * 3600 + 15 * 60);
        assert_eq!(format_duration(d), "2h 15m");
    }
}
