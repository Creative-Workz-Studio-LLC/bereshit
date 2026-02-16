//omni:code --rust -library
//omni:key B-L2-logging-lib-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # Logging Infrastructure (Rust)
//!
//! Platform-level structured logging — levels, modes, file I/O, category routing.
//!
//! Health tracking and domain-specific categories belong in their layers
//! (L3 for CPI-SI health ternary scale, etc.).
//!
//! # Types
//!
//! - `Logger` — Thread-safe structured logger with compact/verbose modes
//! - `Level` — Log severity (Debug, Info, Warn, Error, Fatal)
//! - `Mode` — Output format (Compact, Verbose)
//! - `Entry` — Single log event
//! - `CategoryLogger` — Routes logs to category-specific files (.log + .jsonl)
//! - `SessionLoggers` — Manages all category loggers for a session

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// S.1 Imports
// ────────────────────────────────────────────────────────────────

use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fmt;
use std::fs::{self, OpenOptions};
use std::io::Write;
use std::path::{Path, PathBuf};
use std::sync::Mutex;

// ────────────────────────────────────────────────────────────────
// S.2 Types
// ────────────────────────────────────────────────────────────────

/// Log severity level.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Serialize, Deserialize)]
pub enum Level {
    Debug = 0,
    Info = 1,
    Warn = 2,
    Error = 3,
    Fatal = 4,
}

/// Output format mode.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum Mode {
    Compact,
    Verbose,
}

/// A single log event.
#[derive(Debug, Clone)]
pub struct Entry {
    pub timestamp: String,
    pub level: Level,
    pub component: String,
    pub message: String,
    pub event: String,
    pub details: HashMap<String, String>,
}

/// Thread-safe structured logger.
pub struct Logger {
    inner: Mutex<LoggerInner>,
}

struct LoggerInner {
    mode: Mode,
    level: Level,
    component: String,
}

/// JSONL log entry for machine-parseable output.
#[derive(Debug, Serialize, Deserialize)]
pub struct JSONLEntry {
    pub timestamp: String,
    #[serde(rename = "type")]
    pub event_type: String,
    pub session_id: String,
    pub sequence: usize,
    pub level: String,
    pub component: String,
    pub message: String,
    #[serde(skip_serializing_if = "HashMap::is_empty", default)]
    pub details: HashMap<String, String>,
}

/// Routes logs to category-specific files (.log + .jsonl).
pub struct CategoryLogger {
    inner: Mutex<CategoryInner>,
}

struct CategoryInner {
    category: String,
    session_id: String,
    sequence: usize,
    log_path: PathBuf,
    jsonl_path: PathBuf,
}

/// Manages all category loggers for a session.
pub struct SessionLoggers {
    session_id: String,
    loggers: Mutex<HashMap<String, CategoryLogger>>,
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Level
// ────────────────────────────────────────────────────────────────

impl fmt::Display for Level {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Debug => write!(f, "DEBUG"),
            Self::Info => write!(f, "INFO"),
            Self::Warn => write!(f, "WARN"),
            Self::Error => write!(f, "ERROR"),
            Self::Fatal => write!(f, "FATAL"),
        }
    }
}

impl Level {
    /// Parse a level from string.
    pub fn parse(s: &str) -> Self {
        match s.to_uppercase().as_str() {
            "DEBUG" => Self::Debug,
            "INFO" => Self::Info,
            "WARN" | "WARNING" => Self::Warn,
            "ERROR" => Self::Error,
            "FATAL" => Self::Fatal,
            _ => Self::Info,
        }
    }
}

impl Default for Level {
    fn default() -> Self {
        Self::Info
    }
}

impl Default for Mode {
    fn default() -> Self {
        Self::Compact
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Logger
// ────────────────────────────────────────────────────────────────

impl Logger {
    /// Create a logger with default settings (compact, Info level).
    pub fn new(component: impl Into<String>) -> Self {
        Self {
            inner: Mutex::new(LoggerInner {
                mode: Mode::Compact,
                level: Level::Info,
                component: truncate(&component.into(), 12),
            }),
        }
    }

    /// Create a logger with custom settings.
    pub fn with_options(component: impl Into<String>, mode: Mode, level: Level) -> Self {
        Self {
            inner: Mutex::new(LoggerInner {
                mode,
                level,
                component: truncate(&component.into(), 12),
            }),
        }
    }

    /// Set the output mode.
    pub fn set_mode(&self, mode: Mode) {
        let mut inner = self.inner.lock().unwrap();
        inner.mode = mode;
    }

    /// Set the minimum log level.
    pub fn set_level(&self, level: Level) {
        let mut inner = self.inner.lock().unwrap();
        inner.level = level;
    }

    /// Get the component name.
    pub fn component(&self) -> String {
        let inner = self.inner.lock().unwrap();
        inner.component.clone()
    }

    /// Format an entry to string (compact mode).
    pub fn format_compact(entry: &Entry) -> String {
        let lvl = pad_right(&entry.level.to_string(), 5);
        let comp = pad_right(&entry.component, 12);
        format!(
            "[{}] {} | {} | {}\n",
            entry.timestamp,
            lvl,
            comp,
            truncate(&entry.message, 80)
        )
    }

    /// Format an entry to string (verbose mode).
    pub fn format_verbose(entry: &Entry) -> String {
        let mut b = String::new();

        // Header line
        b.push_str(&format!(
            "[{}] {} | {}\n",
            entry.timestamp,
            entry.level,
            entry.component
        ));

        // Event
        if !entry.event.is_empty() {
            b.push_str(&format!("  EVENT: {}\n", entry.event));
        }

        // Message
        b.push_str(&format!("  MESSAGE: {}\n", entry.message));

        // Details
        if !entry.details.is_empty() {
            b.push_str("  DETAILS:\n");
            for (k, v) in &entry.details {
                b.push_str(&format!("    {k}: {v}\n"));
            }
        }

        b.push_str("---\n");
        b
    }

    /// Format an entry using the logger's current mode.
    pub fn format(&self, entry: &Entry) -> String {
        let inner = self.inner.lock().unwrap();
        match inner.mode {
            Mode::Compact => Self::format_compact(entry),
            Mode::Verbose => Self::format_verbose(entry),
        }
    }

    /// Check if a level would be logged.
    pub fn enabled(&self, level: Level) -> bool {
        let inner = self.inner.lock().unwrap();
        level >= inner.level
    }

    /// Create an entry with the current timestamp.
    pub fn entry(&self, level: Level, message: impl Into<String>) -> Entry {
        let inner = self.inner.lock().unwrap();
        Entry {
            timestamp: now_timestamp(),
            level,
            component: inner.component.clone(),
            message: message.into(),
            event: String::new(),
            details: HashMap::new(),
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 CategoryLogger
// ────────────────────────────────────────────────────────────────

impl CategoryLogger {
    /// Create a category logger that writes to the given directory.
    pub fn new(
        category: impl Into<String>,
        session_id: impl Into<String>,
        dir: impl AsRef<Path>,
    ) -> Result<Self, std::io::Error> {
        let dir = dir.as_ref();
        fs::create_dir_all(dir)?;

        let date = today_date();
        let log_path = dir.join(format!("{date}.log"));
        let jsonl_path = dir.join(format!("{date}.jsonl"));

        Ok(Self {
            inner: Mutex::new(CategoryInner {
                category: category.into(),
                session_id: session_id.into(),
                sequence: 0,
                log_path,
                jsonl_path,
            }),
        })
    }

    /// Log an entry to both .log and .jsonl files.
    pub fn log(
        &self,
        level: Level,
        event_type: &str,
        message: &str,
        details: HashMap<String, String>,
    ) {
        let mut inner = self.inner.lock().unwrap();
        inner.sequence += 1;
        let ts = now_timestamp();

        // Write .log (human-readable)
        let lvl = pad_right(&level.to_string(), 5);
        let comp = pad_right(&inner.category, 12);
        let line = format!(
            "[{ts}] {lvl} | {comp} | {}\n",
            truncate(message, 60)
        );
        let _ = append_to_file(&inner.log_path, &line);

        // Write .jsonl (machine-parseable)
        let jsonl = JSONLEntry {
            timestamp: ts,
            event_type: event_type.to_string(),
            session_id: inner.session_id.clone(),
            sequence: inner.sequence,
            level: level.to_string(),
            component: inner.category.clone(),
            message: message.to_string(),
            details,
        };
        if let Ok(data) = serde_json::to_string(&jsonl) {
            let _ = append_to_file(&inner.jsonl_path, &format!("{data}\n"));
        }
    }

    /// Convenience: log at Info level.
    pub fn info(&self, event_type: &str, message: &str, details: HashMap<String, String>) {
        self.log(Level::Info, event_type, message, details);
    }

    /// Convenience: log at Warn level.
    pub fn warn(&self, event_type: &str, message: &str, details: HashMap<String, String>) {
        self.log(Level::Warn, event_type, message, details);
    }

    /// Convenience: log at Error level.
    pub fn error(&self, event_type: &str, message: &str, details: HashMap<String, String>) {
        self.log(Level::Error, event_type, message, details);
    }

    /// Convenience: log at Debug level.
    pub fn debug(&self, event_type: &str, message: &str, details: HashMap<String, String>) {
        self.log(Level::Debug, event_type, message, details);
    }

    /// Return the category name.
    pub fn category(&self) -> String {
        let inner = self.inner.lock().unwrap();
        inner.category.clone()
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 SessionLoggers
// ────────────────────────────────────────────────────────────────

impl SessionLoggers {
    /// Create a session loggers manager.
    pub fn new(session_id: impl Into<String>) -> Self {
        Self {
            session_id: session_id.into(),
            loggers: Mutex::new(HashMap::new()),
        }
    }

    /// Register a category logger.
    pub fn register(&self, category: impl Into<String>, logger: CategoryLogger) {
        let mut loggers = self.loggers.lock().unwrap();
        loggers.insert(category.into(), logger);
    }

    /// Create and register a category logger for the given directory.
    pub fn add_category(
        &self,
        category: impl Into<String>,
        dir: impl AsRef<Path>,
    ) -> Result<(), std::io::Error> {
        let cat: String = category.into();
        let logger = CategoryLogger::new(&cat, &self.session_id, dir)?;
        self.register(cat, logger);
        Ok(())
    }

    /// Log to a specific category.
    pub fn log(
        &self,
        category: &str,
        level: Level,
        event_type: &str,
        message: &str,
        details: HashMap<String, String>,
    ) {
        let loggers = self.loggers.lock().unwrap();
        if let Some(cl) = loggers.get(category) {
            cl.log(level, event_type, message, details);
        }
    }

    /// Return all registered category names.
    pub fn categories(&self) -> Vec<String> {
        let loggers = self.loggers.lock().unwrap();
        loggers.keys().cloned().collect()
    }

    /// Session ID.
    pub fn session_id(&self) -> &str {
        &self.session_id
    }
}

// ────────────────────────────────────────────────────────────────
// B.5 Helpers
// ────────────────────────────────────────────────────────────────

/// Truncate a string with ellipsis.
pub fn truncate(s: &str, max: usize) -> String {
    if s.len() <= max {
        s.to_string()
    } else if max > 0 {
        format!("{}…", &s[..max - 1])
    } else {
        String::new()
    }
}

/// Pad a string with spaces to the given width.
pub fn pad_right(s: &str, width: usize) -> String {
    if s.len() >= width {
        s.to_string()
    } else {
        format!("{s}{}", " ".repeat(width - s.len()))
    }
}

/// Clamp a value between min and max.
pub fn clamp(v: i32, min: i32, max: i32) -> i32 {
    v.clamp(min, max)
}

/// Current timestamp in YYYY-MM-DD HH:MM:SS format.
fn now_timestamp() -> String {
    // Use system time formatted as local time
    let now = std::time::SystemTime::now();
    let duration = now
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap_or_default();
    let secs = duration.as_secs();
    // UTC format (local time formatting requires chrono)
    let s = secs % 60;
    let m = (secs / 60) % 60;
    let h = (secs / 3600) % 24;
    let days = secs / 86400;
    // Approximate date from days since epoch
    let (y, mo, d) = days_to_date(days);
    format!("{y:04}-{mo:02}-{d:02} {h:02}:{m:02}:{s:02}")
}

/// Today's date as YYYY-MM-DD.
fn today_date() -> String {
    let now = std::time::SystemTime::now();
    let duration = now
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap_or_default();
    let days = duration.as_secs() / 86400;
    let (y, m, d) = days_to_date(days);
    format!("{y:04}-{m:02}-{d:02}")
}

/// Convert days since Unix epoch to (year, month, day).
fn days_to_date(days: u64) -> (u64, u64, u64) {
    // Civil date algorithm (Howard Hinnant)
    let z = days + 719468;
    let era = z / 146097;
    let doe = z - era * 146097;
    let yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    let y = yoe + era * 400;
    let doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    let mp = (5 * doy + 2) / 153;
    let d = doy - (153 * mp + 2) / 5 + 1;
    let m = if mp < 10 { mp + 3 } else { mp - 9 };
    let y = if m <= 2 { y + 1 } else { y };
    (y, m, d)
}

/// Append text to a file (creates if needed).
fn append_to_file(path: &Path, content: &str) -> std::io::Result<()> {
    let mut f = OpenOptions::new()
        .create(true)
        .append(true)
        .open(path)?;
    f.write_all(content.as_bytes())
}

// ============================================================================
// CLOSING
// ============================================================================
// Platform logging infrastructure — generic structured logger.
// "Ponder the path of thy feet" — Proverbs 4:26

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn level_display() {
        assert_eq!(Level::Debug.to_string(), "DEBUG");
        assert_eq!(Level::Info.to_string(), "INFO");
        assert_eq!(Level::Fatal.to_string(), "FATAL");
    }

    #[test]
    fn level_parse() {
        assert_eq!(Level::parse("debug"), Level::Debug);
        assert_eq!(Level::parse("WARNING"), Level::Warn);
        assert_eq!(Level::parse("unknown"), Level::Info);
    }

    #[test]
    fn level_ordering() {
        assert!(Level::Debug < Level::Info);
        assert!(Level::Info < Level::Warn);
        assert!(Level::Error < Level::Fatal);
    }

    #[test]
    fn logger_create() {
        let log = Logger::new("test");
        assert_eq!(log.component(), "test");
        assert!(log.enabled(Level::Info));
        assert!(!log.enabled(Level::Debug));
    }

    #[test]
    fn logger_entry() {
        let log = Logger::new("test");
        let entry = log.entry(Level::Info, "hello world");
        assert_eq!(entry.level, Level::Info);
        assert_eq!(entry.message, "hello world");
    }

    #[test]
    fn format_compact() {
        let entry = Entry {
            timestamp: "2026-02-16 12:00:00".into(),
            level: Level::Info,
            component: "test".into(),
            message: "hello".into(),
            event: String::new(),
            details: HashMap::new(),
        };
        let out = Logger::format_compact(&entry);
        assert!(out.contains("INFO"));
        assert!(out.contains("hello"));
    }

    #[test]
    fn format_verbose() {
        let mut details = HashMap::new();
        details.insert("key".into(), "value".into());
        let entry = Entry {
            timestamp: "2026-02-16 12:00:00".into(),
            level: Level::Warn,
            component: "test".into(),
            message: "warning msg".into(),
            event: "test_event".into(),
            details,
        };
        let out = Logger::format_verbose(&entry);
        assert!(out.contains("WARN"));
        assert!(out.contains("EVENT: test_event"));
        assert!(out.contains("key: value"));
        assert!(out.contains("---"));
    }

    #[test]
    fn truncate_short() {
        assert_eq!(truncate("hi", 10), "hi");
    }

    #[test]
    fn truncate_long() {
        let result = truncate("hello world", 6);
        // 5 ASCII chars + '…' (3-byte UTF-8) = 8 bytes, 6 chars
        assert_eq!(result.chars().count(), 6);
        assert!(result.ends_with('…'));
    }

    #[test]
    fn pad_right_short() {
        assert_eq!(pad_right("hi", 5), "hi   ");
    }

    #[test]
    fn pad_right_long() {
        assert_eq!(pad_right("hello", 3), "hello");
    }

    #[test]
    fn days_to_date_epoch() {
        let (y, m, d) = days_to_date(0);
        assert_eq!((y, m, d), (1970, 1, 1));
    }

    #[test]
    fn days_to_date_known() {
        // 2026-02-16 is day 20500 from epoch
        let (y, m, d) = days_to_date(20500);
        assert_eq!(y, 2026);
        assert_eq!(m, 2);
        assert_eq!(d, 16);
    }

    #[test]
    fn session_loggers_create() {
        let sl = SessionLoggers::new("test-session");
        assert_eq!(sl.session_id(), "test-session");
        assert!(sl.categories().is_empty());
    }

    #[test]
    fn jsonl_serialize() {
        let entry = JSONLEntry {
            timestamp: "2026-02-16T12:00:00Z".into(),
            event_type: "test".into(),
            session_id: "sess-1".into(),
            sequence: 1,
            level: "INFO".into(),
            component: "test".into(),
            message: "hello".into(),
            details: HashMap::new(),
        };
        let json = serde_json::to_string(&entry).expect("serialize");
        assert!(json.contains("\"type\":\"test\""));
        assert!(json.contains("\"session_id\":\"sess-1\""));
    }
}
