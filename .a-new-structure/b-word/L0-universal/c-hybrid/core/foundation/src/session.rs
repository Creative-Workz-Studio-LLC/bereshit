//omni:code --rust -library
//omni:key B-L0-foundation-session-rs
//omni:version b-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-foundation-session-rs
// Purpose: Universal session tracking types — ID, paths, logs, duration
// Biblical: Ecclesiastes 3:1 — To every thing there is a season
// Version: b-02.00

//! # Session Management
//!
//! Universal session tracking — ID, paths, logs, duration formatting.
//!
//! | Type | Models | When |
//! |------|--------|------|
//! | [`Session`] | Current session metadata | Session lifecycle |
//! | [`SessionLog`] | Typed log entries | Activity logging |
//! | [`LogEntry`] | Single log entry | Entry construction |
//! | [`SessionPaths`] | Resolved file paths | Path resolution |
//!
//! **Design decision:** Types only — no filesystem I/O. Consumers handle
//! loading/saving. This keeps foundation substrate-agnostic.
//!
//! Utility functions [`format_duration`] and [`format_elapsed`] provide
//! human-readable time formatting across all layers.

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// section-order listing (SETUP subsections present in this file):
//   [s01]-imports, [s07]-types

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use serde::{Deserialize, Serialize};
use std::fmt;
use std::path::PathBuf;
use std::time::{Duration, SystemTime};

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

/// Current session metadata.
///
/// Tracks session identity and compaction count. Serde-compatible
/// for JSON persistence.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::Session;
///
/// let s = Session::new("abc-123", "2026-02-18T12:00:00Z");
/// assert_eq!(s.session_id, "abc-123");
/// assert_eq!(s.compactions, 0);
/// ```
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
#[derive(Default)]
pub struct Session {
    /// Unique session identifier.
    pub session_id: String,
    /// ISO 8601 start timestamp.
    pub start_time: String,
    /// Number of context compactions in this session.
    #[serde(default)]
    pub compactions: u32,
}

/// A single log entry.
///
/// Each entry carries a timestamp, type tag, and content.
/// Type tags follow the set: `"task"`, `"note"`, `"tool"`, `"milestone"`.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::LogEntry;
///
/// let e = LogEntry::new("2026-02-18T12:00:00Z", "task", "Started refactoring");
/// assert_eq!(e.entry_type, "task");
/// ```
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
#[derive(Default)]
pub struct LogEntry {
    /// ISO 8601 timestamp.
    pub timestamp: String,
    /// Entry type: "task", "note", "tool", "milestone".
    #[serde(rename = "type")]
    pub entry_type: String,
    /// Human-readable content.
    pub content: String,
}

/// Session log containing typed entries.
///
/// Tracks all log entries for a given session. Entries are ordered
/// by insertion (typically chronological).
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::{SessionLog, LogEntry};
///
/// let mut log = SessionLog::new("abc-123");
/// log.add_entry(LogEntry::new("2026-02-18T12:00:00Z", "task", "Started work"));
/// assert_eq!(log.entry_count(), 1);
/// ```
#[derive(Debug, Clone, Serialize, Deserialize)]
#[derive(Default)]
pub struct SessionLog {
    /// Session this log belongs to.
    pub session_id: String,
    /// Ordered log entries.
    pub entries: Vec<LogEntry>,
}

/// Resolved paths to session files.
///
/// Pure path construction — no filesystem I/O. Consumers create
/// this from their session directory and use the resolved paths
/// for their own loading/saving.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::SessionPaths;
/// use std::path::PathBuf;
///
/// let paths = SessionPaths::from_dir(PathBuf::from("/tmp/session"));
/// assert!(paths.current_session.ends_with("current.json"));
/// ```
#[derive(Debug, Clone)]
pub struct SessionPaths {
    /// Root session directory.
    pub session_dir: PathBuf,
    /// Path to current session metadata file.
    pub current_session: PathBuf,
    /// Path to session log file.
    pub session_log: PathBuf,
    /// Path to patterns file.
    pub pattern_file: PathBuf,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Subsection order:
//   3. Constructors   — Creating instances and defaults
//   4. Mutators       — Modifying state
//   5. Queries        — Inspecting state
//   6. Output         — Display formatting
//   7. Utilities      — Free functions (format_duration, format_elapsed)

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors
// ──────────────────────────────────────────────────────────────────────────

impl Session {
    /// Create a new session with the given ID and start time.
    ///
    /// Compactions start at 0.
    pub fn new(session_id: impl Into<String>, start_time: impl Into<String>) -> Self {
        Self {
            session_id: session_id.into(),
            start_time: start_time.into(),
            compactions: 0,
        }
    }
}


impl LogEntry {
    /// Create a new log entry.
    pub fn new(
        timestamp: impl Into<String>,
        entry_type: impl Into<String>,
        content: impl Into<String>,
    ) -> Self {
        Self {
            timestamp: timestamp.into(),
            entry_type: entry_type.into(),
            content: content.into(),
        }
    }
}


impl SessionLog {
    /// Create a new empty session log.
    pub fn new(session_id: impl Into<String>) -> Self {
        Self {
            session_id: session_id.into(),
            entries: Vec::new(),
        }
    }
}


impl SessionPaths {
    /// Resolve session file paths from a directory.
    ///
    /// Pure path construction — no filesystem operations performed.
    pub fn from_dir(session_dir: PathBuf) -> Self {
        Self {
            current_session: session_dir.join("current.json"),
            session_log: session_dir.join("current-log.json"),
            pattern_file: session_dir.join("patterns.json"),
            session_dir,
        }
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Mutators
// ──────────────────────────────────────────────────────────────────────────

impl Session {
    /// Record a compaction event.
    pub fn record_compaction(&mut self) {
        self.compactions += 1;
    }
}

impl SessionLog {
    /// Append a log entry.
    pub fn add_entry(&mut self, entry: LogEntry) {
        self.entries.push(entry);
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Queries & Accessors
// ──────────────────────────────────────────────────────────────────────────

impl Session {
    /// Whether this session has been compacted at least once.
    pub fn has_compacted(&self) -> bool {
        self.compactions > 0
    }
}

impl SessionLog {
    /// Number of entries in the log.
    pub fn entry_count(&self) -> usize {
        self.entries.len()
    }

    /// Whether the log has any entries.
    pub fn is_empty(&self) -> bool {
        self.entries.is_empty()
    }

    /// Count entries of a specific type.
    pub fn count_by_type(&self, entry_type: &str) -> usize {
        self.entries
            .iter()
            .filter(|e| e.entry_type == entry_type)
            .count()
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 6. Output & Display
// ──────────────────────────────────────────────────────────────────────────

impl fmt::Display for Session {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "[{}] started {} (compactions: {})",
            self.session_id, self.start_time, self.compactions
        )
    }
}

impl fmt::Display for LogEntry {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "[{}] {}: {}", self.timestamp, self.entry_type, self.content)
    }
}

impl fmt::Display for SessionLog {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Session {} ({} entries)", self.session_id, self.entries.len())
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Utility Functions
// ──────────────────────────────────────────────────────────────────────────

/// Format a duration as human-readable text.
///
/// | Range | Format |
/// |-------|--------|
/// | < 60s | `"{N}s"` |
/// | 1-59m | `"{N}m"` |
/// | 1h+ | `"{N}h {M}m"` |
///
/// # Examples
///
/// ```
/// use std::time::Duration;
/// use bereshit_l0_foundation::format_duration;
///
/// assert_eq!(format_duration(Duration::from_secs(30)), "30s");
/// assert_eq!(format_duration(Duration::from_secs(300)), "5m");
/// assert_eq!(format_duration(Duration::from_secs(3660)), "1h 1m");
/// ```
pub fn format_duration(d: Duration) -> String {
    let total_secs = d.as_secs();
    let hours = total_secs / 3600;
    let minutes = (total_secs % 3600) / 60;

    if hours > 0 {
        format!("{hours}h {minutes}m")
    } else if total_secs >= 60 {
        format!("{minutes}m")
    } else {
        format!("{total_secs}s")
    }
}

/// Format elapsed time since a given `SystemTime`.
///
/// Returns `"unknown"` if the system clock went backwards.
///
/// # Examples
///
/// ```
/// use std::time::SystemTime;
/// use bereshit_l0_foundation::format_elapsed;
///
/// let result = format_elapsed(SystemTime::now());
/// assert!(result == "0s" || result.ends_with('s'));
/// ```
pub fn format_elapsed(since: SystemTime) -> String {
    match since.elapsed() {
        Ok(d) => format_duration(d),
        Err(_) => "unknown".into(),
    }
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "To every thing there is a season, and a time to every purpose
// under the heaven." — Ecclesiastes 3:1

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    // -- Session ---------------------------------------------------------

    #[test]
    fn session_new() {
        let s = Session::new("abc", "2026-02-18T12:00:00Z");
        assert_eq!(s.session_id, "abc");
        assert_eq!(s.start_time, "2026-02-18T12:00:00Z");
        assert_eq!(s.compactions, 0);
    }

    #[test]
    fn session_default() {
        let s = Session::default();
        assert!(s.session_id.is_empty());
        assert!(s.start_time.is_empty());
        assert_eq!(s.compactions, 0);
    }

    #[test]
    fn session_compaction() {
        let mut s = Session::new("test", "now");
        assert!(!s.has_compacted());
        s.record_compaction();
        assert!(s.has_compacted());
        assert_eq!(s.compactions, 1);
        s.record_compaction();
        assert_eq!(s.compactions, 2);
    }

    #[test]
    fn session_display() {
        let s = Session::new("abc", "2026-02-18");
        assert_eq!(s.to_string(), "[abc] started 2026-02-18 (compactions: 0)");
    }

    #[test]
    fn session_serde_roundtrip() {
        let original = Session::new("test-id", "2026-01-01T00:00:00Z");
        let json = serde_json::to_string(&original).unwrap();
        let restored: Session = serde_json::from_str(&json).unwrap();
        assert_eq!(restored, original);
    }

    #[test]
    fn session_serde_default_compactions() {
        // JSON without compactions field should default to 0
        let json = r#"{"session_id":"x","start_time":"now"}"#;
        let s: Session = serde_json::from_str(json).unwrap();
        assert_eq!(s.compactions, 0);
    }

    // -- LogEntry --------------------------------------------------------

    #[test]
    fn log_entry_new() {
        let e = LogEntry::new("2026-02-18T12:00:00Z", "task", "Started");
        assert_eq!(e.entry_type, "task");
        assert_eq!(e.content, "Started");
    }

    #[test]
    fn log_entry_default() {
        let e = LogEntry::default();
        assert!(e.timestamp.is_empty());
        assert!(e.entry_type.is_empty());
        assert!(e.content.is_empty());
    }

    #[test]
    fn log_entry_display() {
        let e = LogEntry::new("12:00", "note", "Hello");
        assert_eq!(e.to_string(), "[12:00] note: Hello");
    }

    #[test]
    fn log_entry_serde_roundtrip() {
        let original = LogEntry::new("2026-01-01", "milestone", "First commit");
        let json = serde_json::to_string(&original).unwrap();
        let restored: LogEntry = serde_json::from_str(&json).unwrap();
        assert_eq!(restored, original);
    }

    #[test]
    fn log_entry_serde_type_rename() {
        // "type" field in JSON maps to entry_type in Rust
        let json = r#"{"timestamp":"t","type":"task","content":"c"}"#;
        let e: LogEntry = serde_json::from_str(json).unwrap();
        assert_eq!(e.entry_type, "task");
    }

    // -- SessionLog ------------------------------------------------------

    #[test]
    fn session_log_new() {
        let log = SessionLog::new("abc");
        assert_eq!(log.session_id, "abc");
        assert!(log.is_empty());
        assert_eq!(log.entry_count(), 0);
    }

    #[test]
    fn session_log_default() {
        let log = SessionLog::default();
        assert!(log.session_id.is_empty());
        assert!(log.is_empty());
    }

    #[test]
    fn session_log_add_entry() {
        let mut log = SessionLog::new("test");
        log.add_entry(LogEntry::new("t1", "task", "first"));
        log.add_entry(LogEntry::new("t2", "note", "second"));
        assert_eq!(log.entry_count(), 2);
        assert!(!log.is_empty());
    }

    #[test]
    fn session_log_count_by_type() {
        let mut log = SessionLog::new("test");
        log.add_entry(LogEntry::new("t1", "task", "a"));
        log.add_entry(LogEntry::new("t2", "note", "b"));
        log.add_entry(LogEntry::new("t3", "task", "c"));
        assert_eq!(log.count_by_type("task"), 2);
        assert_eq!(log.count_by_type("note"), 1);
        assert_eq!(log.count_by_type("tool"), 0);
    }

    #[test]
    fn session_log_display() {
        let mut log = SessionLog::new("abc");
        log.add_entry(LogEntry::new("t", "task", "x"));
        assert_eq!(log.to_string(), "Session abc (1 entries)");
    }

    #[test]
    fn session_log_serde_roundtrip() {
        let mut log = SessionLog::new("test");
        log.add_entry(LogEntry::new("t1", "task", "content"));
        let json = serde_json::to_string(&log).unwrap();
        let restored: SessionLog = serde_json::from_str(&json).unwrap();
        assert_eq!(restored.session_id, "test");
        assert_eq!(restored.entries.len(), 1);
    }

    // -- SessionPaths ----------------------------------------------------

    #[test]
    fn session_paths_from_dir() {
        let paths = SessionPaths::from_dir(PathBuf::from("/tmp/session"));
        assert_eq!(paths.session_dir, PathBuf::from("/tmp/session"));
        assert!(paths.current_session.ends_with("current.json"));
        assert!(paths.session_log.ends_with("current-log.json"));
        assert!(paths.pattern_file.ends_with("patterns.json"));
    }

    // -- format_duration -------------------------------------------------

    #[test]
    fn duration_zero() {
        assert_eq!(format_duration(Duration::from_secs(0)), "0s");
    }

    #[test]
    fn duration_seconds() {
        assert_eq!(format_duration(Duration::from_secs(1)), "1s");
        assert_eq!(format_duration(Duration::from_secs(30)), "30s");
        assert_eq!(format_duration(Duration::from_secs(59)), "59s");
    }

    #[test]
    fn duration_minutes() {
        assert_eq!(format_duration(Duration::from_secs(60)), "1m");
        assert_eq!(format_duration(Duration::from_secs(90)), "1m");
        assert_eq!(format_duration(Duration::from_secs(45 * 60)), "45m");
    }

    #[test]
    fn duration_hours() {
        assert_eq!(format_duration(Duration::from_secs(3600)), "1h 0m");
        assert_eq!(format_duration(Duration::from_secs(2 * 3600 + 15 * 60)), "2h 15m");
    }

    // -- format_elapsed --------------------------------------------------

    #[test]
    fn elapsed_from_now() {
        let result = format_elapsed(SystemTime::now());
        assert!(result.ends_with('s')); // should be 0s or very small
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library module — no entry point.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Serde types + path helpers — no resources to clean up.

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new query/accessor methods on any type
//   - Add new constructors (follow impl Into<String> pattern)
//   - Add tests
//   - Add new entry types (the type tag is a String — open set)
//
// Modify with Care:
//   - Struct fields — breaks construction and serde
//   - LogEntry #[serde(rename = "type")] — JSON contract
//   - format_duration output format — consumers may parse it
//
// Never Modify:
//   - Session.session_id as primary key
//   - LogEntry timestamp/type/content as core triple
//   - SessionPaths.from_dir as the construction pattern

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// Universal session types — promoted from L3, stripped of substrate specifics.
// Types only. No filesystem I/O. Consumers handle persistence.
//
// "To every thing there is a season, and a time to every purpose
// under the heaven." — Ecclesiastes 3:1

// ============================================================================
// END CLOSING
// ============================================================================
