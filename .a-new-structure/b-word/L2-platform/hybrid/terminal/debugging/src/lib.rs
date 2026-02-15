//omni:code --rust -library
//omni:key B-L2-debugging
//omni:version a-01.00

// =============================================================================
// METADATA
// =============================================================================
//
// Key: B-L2-debugging
// Purpose: Structured debugging for CPI-SI — Assessment layer of immune system
// Biblical: Job 28:27 — "Then did he see it, and declare it; he prepared it"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// Design Goals:
//   - Three verbosity levels (BASIC, DETAILED, VERBOSE)
//   - Clear section markers for parsing
//   - Event timeline with health tracking
//   - Context capture for reproducibility
//
// Rust counterpart of debugging.go — same API surface, Rust idioms.
//
// =============================================================================

//! # Structured Debugging
//!
//! L2 structured debugging for CPI-SI — the assessment layer of the immune
//! system.
//!
//! Three verbosity levels control output detail:
//!
//! | Level | Content |
//! |-------|---------|
//! | `Basic` | State snapshots only |
//! | `Detailed` | + Event timeline |
//! | `Verbose` | + Analysis section |
//!
//! # Usage
//!
//! ```
//! use bereshit_l2_debugging::{Session, Level, Severity, EventType, Result as DebugResult};
//! use std::collections::HashMap;
//!
//! let mut session = Session::new("my-component", Level::Detailed);
//! session.set_initial_health(85);
//! session.set_parameter("mode", "test");
//!
//! session.info(
//!     EventType::Init,
//!     "Component initialized",
//!     HashMap::new(),
//!     85, 85,
//! );
//!
//! let mut final_state = HashMap::new();
//! final_state.insert("status".into(), "ready".into());
//! session.end(DebugResult::Success, 0, 90, final_state);
//! ```

// =============================================================================
// SETUP
// =============================================================================

// ────────────────────────────────────────────────────────────────
// S.1 Imports
// ────────────────────────────────────────────────────────────────

use std::collections::HashMap;
use std::env;
use std::fmt;
use std::io::{self, Write};
use std::time::{Duration, Instant, SystemTime};

// ────────────────────────────────────────────────────────────────
// S.2 Constants
// ────────────────────────────────────────────────────────────────

/// A single health-to-display entry.
#[derive(Debug, Clone)]
pub struct HealthEmojiEntry {
    pub min: i32,
    pub emoji: &'static str,
    pub label: &'static str,
}

/// Debugging configuration — health emoji map injectable from database.
#[derive(Debug, Clone)]
pub struct Config {
    pub health_emoji: Vec<HealthEmojiEntry>,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            health_emoji: vec![
                HealthEmojiEntry { min: 90, emoji: "\u{1f49a}", label: "Excellent" },
                HealthEmojiEntry { min: 70, emoji: "\u{1f49b}", label: "Good" },
                HealthEmojiEntry { min: 50, emoji: "\u{1f9e1}", label: "Moderate" },
                HealthEmojiEntry { min: 30, emoji: "\u{2764}\u{fe0f}", label: "Poor" },
                HealthEmojiEntry { min: 10, emoji: "\u{1fa78}", label: "Critical" },
                HealthEmojiEntry { min: 0,  emoji: "\u{2620}\u{fe0f}", label: "Failed" },
            ],
        }
    }
}

/// Package-level configuration. Set via `init()`, falls back to defaults.
static CFG: std::sync::OnceLock<Config> = std::sync::OnceLock::new();

/// Initializes the package configuration. Call once at startup.
pub fn init(cfg: Option<Config>) {
    let _ = CFG.set(cfg.unwrap_or_default());
}

/// Returns a reference to the active configuration.
fn cfg_ref() -> &'static Config {
    CFG.get_or_init(Config::default)
}

// ────────────────────────────────────────────────────────────────
// S.3 Types
// ────────────────────────────────────────────────────────────────

/// Debug verbosity level.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum Level {
    /// State snapshots only.
    Basic    = 0,
    /// + Event timeline.
    Detailed = 1,
    /// + Analysis section.
    Verbose  = 2,
}

/// Event type for timeline entries.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum EventType {
    Init,
    Validate,
    Operation,
    Checkpoint,
    Decision,
    Warning,
    Error,
}

/// Event severity.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Severity {
    Debug,
    Info,
    Warn,
    Error,
}

/// Session outcome.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Result {
    Success,
    Partial,
    Failed,
    Timeout,
}

/// A single debug event in the timeline.
#[derive(Debug, Clone)]
pub struct Event {
    pub elapsed:       Duration,
    pub severity:      Severity,
    pub event_type:    EventType,
    pub description:   String,
    pub details:       HashMap<String, String>,
    pub health_before: i32,
    pub health_after:  i32,
}

/// A complete debug session.
pub struct Session {
    // Identity
    pub component:  String,
    pub context_id: String,
    pub pid:        u32,
    pub level:      Level,

    // Timing
    start_time:  SystemTime,
    start_clock: Instant,
    end_elapsed: Option<Duration>,

    // Context
    pub username: String,
    pub hostname: String,
    pub shell:    String,
    pub cwd:      String,
    pub distro:   String,
    pub arch:     String,
    pub kernel:   String,

    // State
    pub initial_health: i32,
    pub final_health:   i32,
    pub parameters:     HashMap<String, String>,
    pub initial_state:  HashMap<String, String>,
    pub final_state:    HashMap<String, String>,

    // Events (for DETAILED and VERBOSE)
    pub events: Vec<Event>,

    // Analysis (for VERBOSE only)
    pub issues:          Vec<String>,
    pub performance:     HashMap<String, String>,
    pub recommendations: Vec<String>,

    // Outcome
    pub result:    Option<Result>,
    pub exit_code: i32,

    // Output
    out: Box<dyn Write>,
}

// =============================================================================
// BODY
// =============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Display Implementations
// ────────────────────────────────────────────────────────────────

impl fmt::Display for Level {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Level::Basic    => write!(f, "BASIC"),
            Level::Detailed => write!(f, "DETAILED"),
            Level::Verbose  => write!(f, "VERBOSE"),
        }
    }
}

impl fmt::Display for EventType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            EventType::Init       => write!(f, "INIT"),
            EventType::Validate   => write!(f, "VALIDATE"),
            EventType::Operation  => write!(f, "OPERATION"),
            EventType::Checkpoint => write!(f, "CHECKPOINT"),
            EventType::Decision   => write!(f, "DECISION"),
            EventType::Warning    => write!(f, "WARNING"),
            EventType::Error      => write!(f, "ERROR"),
        }
    }
}

impl fmt::Display for Severity {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Severity::Debug => write!(f, "DEBUG"),
            Severity::Info  => write!(f, "INFO"),
            Severity::Warn  => write!(f, "WARN"),
            Severity::Error => write!(f, "ERROR"),
        }
    }
}

impl fmt::Display for Result {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Result::Success => write!(f, "SUCCESS"),
            Result::Partial => write!(f, "PARTIAL"),
            Result::Failed  => write!(f, "FAILED"),
            Result::Timeout => write!(f, "TIMEOUT"),
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Session Construction
// ────────────────────────────────────────────────────────────────

impl Session {
    /// Creates a new debug session for the given component.
    pub fn new(component: &str, level: Level) -> Self {
        let now = SystemTime::now();
        let clock = Instant::now();
        let pid = std::process::id();

        let nanos = now
            .duration_since(SystemTime::UNIX_EPOCH)
            .map(|d| d.as_nanos())
            .unwrap_or(0);
        let context_id = format!("{component}-{pid}-{nanos}");

        let mut s = Session {
            component:       component.to_string(),
            context_id,
            pid,
            level,
            start_time:      now,
            start_clock:     clock,
            end_elapsed:     None,
            username:        String::new(),
            hostname:        String::new(),
            shell:           String::new(),
            cwd:             String::new(),
            distro:          String::new(),
            arch:            String::new(),
            kernel:          String::new(),
            initial_health:  0,
            final_health:    0,
            parameters:      HashMap::new(),
            initial_state:   HashMap::new(),
            final_state:     HashMap::new(),
            events:          Vec::new(),
            issues:          Vec::new(),
            performance:     HashMap::new(),
            recommendations: Vec::new(),
            result:          None,
            exit_code:       0,
            out:             Box::new(io::stdout()),
        };

        s.capture_context();
        s
    }

    /// Gathers execution environment context.
    fn capture_context(&mut self) {
        // Username
        self.username = env::var("USER")
            .or_else(|_| env::var("LOGNAME"))
            .unwrap_or_else(|_| "unknown".into());

        // Hostname
        self.hostname = std::fs::read_to_string("/etc/hostname")
            .map(|s| s.trim().to_string())
            .unwrap_or_else(|_| "unknown".into());

        // Shell
        self.shell = env::var("SHELL").unwrap_or_else(|_| "/bin/sh".into());

        // Working directory
        self.cwd = env::current_dir()
            .map(|p| p.display().to_string())
            .unwrap_or_else(|_| "unknown".into());

        // Architecture
        self.arch = env::consts::ARCH.to_string();
        self.kernel = env::consts::OS.to_string();

        // Distro from /etc/os-release
        self.distro = read_distro().unwrap_or_else(|| env::consts::OS.to_string());
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 Session Configuration
// ────────────────────────────────────────────────────────────────

impl Session {
    /// Changes the output destination.
    pub fn set_output(&mut self, w: Box<dyn Write>) {
        self.out = w;
    }

    /// Sets an initial parameter.
    pub fn set_parameter(&mut self, key: &str, value: &str) {
        self.parameters.insert(key.into(), value.into());
    }

    /// Sets initial state key-value.
    pub fn set_initial_state(&mut self, key: &str, value: &str) {
        self.initial_state.insert(key.into(), value.into());
    }

    /// Sets starting health (clamped to 0..=100).
    pub fn set_initial_health(&mut self, h: i32) {
        self.initial_health = h.clamp(0, 100);
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 Event Recording
// ────────────────────────────────────────────────────────────────

impl Session {
    /// Records an event in the timeline.
    ///
    /// Events are only recorded at `Detailed` level or above.
    pub fn add_event(
        &mut self,
        severity: Severity,
        event_type: EventType,
        desc: &str,
        details: HashMap<String, String>,
        health_before: i32,
        health_after: i32,
    ) {
        if self.level < Level::Detailed {
            return; // BASIC level doesn't record events
        }

        self.events.push(Event {
            elapsed:       self.start_clock.elapsed(),
            severity,
            event_type,
            description:   desc.to_string(),
            details,
            health_before,
            health_after,
        });
    }

    /// Adds an INFO event.
    pub fn info(
        &mut self,
        event_type: EventType,
        desc: &str,
        details: HashMap<String, String>,
        health_before: i32,
        health_after: i32,
    ) {
        self.add_event(Severity::Info, event_type, desc, details, health_before, health_after);
    }

    /// Adds a WARN event.
    pub fn warn(
        &mut self,
        event_type: EventType,
        desc: &str,
        details: HashMap<String, String>,
        health_before: i32,
        health_after: i32,
    ) {
        self.add_event(Severity::Warn, event_type, desc, details, health_before, health_after);
    }

    /// Adds an ERROR event.
    pub fn error(
        &mut self,
        event_type: EventType,
        desc: &str,
        details: HashMap<String, String>,
        health_before: i32,
        health_after: i32,
    ) {
        self.add_event(Severity::Error, event_type, desc, details, health_before, health_after);
    }
}

// ────────────────────────────────────────────────────────────────
// B.5 Analysis (VERBOSE only)
// ────────────────────────────────────────────────────────────────

impl Session {
    /// Adds an issue to the analysis section.
    pub fn add_issue(&mut self, issue: &str) {
        if self.level < Level::Verbose {
            return;
        }
        self.issues.push(issue.to_string());
    }

    /// Adds performance data.
    pub fn add_performance(&mut self, key: &str, value: &str) {
        if self.level < Level::Verbose {
            return;
        }
        self.performance.insert(key.into(), value.into());
    }

    /// Adds a recommendation.
    pub fn add_recommendation(&mut self, rec: &str) {
        if self.level < Level::Verbose {
            return;
        }
        self.recommendations.push(rec.to_string());
    }
}

// ────────────────────────────────────────────────────────────────
// B.6 Session Completion
// ────────────────────────────────────────────────────────────────

impl Session {
    /// Completes the session and writes output.
    pub fn end(
        &mut self,
        result: Result,
        exit_code: i32,
        final_health: i32,
        final_state: HashMap<String, String>,
    ) {
        self.end_elapsed = Some(self.start_clock.elapsed());
        self.result       = Some(result);
        self.exit_code    = exit_code;
        self.final_health = final_health.clamp(0, 100);
        self.final_state  = final_state;

        self.write_output();
    }

    /// Returns the session output as a String without writing to the output
    /// destination.
    pub fn render(&self) -> String {
        let mut buf = String::new();
        self.write_header(&mut buf);
        self.write_context(&mut buf);
        self.write_initial_state(&mut buf);
        if self.level >= Level::Detailed {
            self.write_events(&mut buf);
        }
        self.write_final_state(&mut buf);
        if self.level >= Level::Verbose {
            self.write_analysis(&mut buf);
        }
        self.write_footer(&mut buf);
        buf
    }
}

// ────────────────────────────────────────────────────────────────
// B.7 Output Formatting
// ────────────────────────────────────────────────────────────────

impl Session {
    fn write_output(&mut self) {
        let rendered = self.render();
        let _ = self.out.write_all(rendered.as_bytes());
    }

    fn write_header(&self, b: &mut String) {
        let started = format_system_time(self.start_time);

        b.push_str("# DEBUG_SESSION_START\n");
        b.push_str("\u{250c}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2510}\n");
        b.push_str("\u{2502} CPI-SI Debug Session                                          \u{2502}\n");
        b.push_str("\u{251c}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2524}\n");
        fmt_push(b, &format!("\u{2502} Component:  {:<51}\u{2502}\n", self.component));
        fmt_push(b, &format!("\u{2502} Context ID: {:<51}\u{2502}\n", truncate(&self.context_id, 51)));
        fmt_push(b, &format!("\u{2502} PID:        {:<51}\u{2502}\n", self.pid));
        fmt_push(b, &format!("\u{2502} Level:      {:<51}\u{2502}\n", self.level));
        fmt_push(b, &format!("\u{2502} Started:    {:<51}\u{2502}\n", started));
        b.push_str("\u{2514}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2518}\n");
    }

    fn write_context(&self, b: &mut String) {
        b.push_str("\n# CONTEXT\n");
        b.push_str("\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\n\n");
        fmt_push(b, &format!("Identity: {}@{}:{}\n", self.username, self.hostname, self.pid));
        fmt_push(b, &format!("Shell:    {}\n", self.shell));
        fmt_push(b, &format!("CWD:      {}\n\n", self.cwd));
        fmt_push(b, &format!("System:   {} ({})\n", self.distro, self.arch));
        fmt_push(b, &format!("Kernel:   {}\n\n", self.kernel));
    }

    fn write_initial_state(&self, b: &mut String) {
        let (emoji, _) = health_info(self.initial_health);

        b.push_str("# INITIAL_STATE\n");
        b.push_str("\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\n\n");
        fmt_push(b, &format!("Health: {}% {}\n\n", self.initial_health, emoji));

        if !self.parameters.is_empty() {
            b.push_str("Parameters:\n");
            let mut keys: Vec<_> = self.parameters.keys().collect();
            keys.sort();
            for k in keys {
                fmt_push(b, &format!("  {}: {}\n", k, self.parameters[k]));
            }
            b.push('\n');
        }

        if !self.initial_state.is_empty() {
            b.push_str("Component State:\n");
            let mut keys: Vec<_> = self.initial_state.keys().collect();
            keys.sort();
            for k in keys {
                fmt_push(b, &format!("  {}: {}\n", k, self.initial_state[k]));
            }
            b.push('\n');
        }
    }

    fn write_events(&self, b: &mut String) {
        b.push_str("# EVENTS\n");
        b.push_str("\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\n\n");

        if self.events.is_empty() {
            b.push_str("(no events recorded)\n\n");
            return;
        }

        for e in &self.events {
            let ts = format_duration_timestamp(e.elapsed);
            fmt_push(b, &format!("[{}] {} | {} | {}\n", ts, e.severity, e.event_type, e.description));

            let mut keys: Vec<_> = e.details.keys().collect();
            keys.sort();
            for k in keys {
                fmt_push(b, &format!("  {}: {}\n", k, e.details[k]));
            }

            if e.health_before != e.health_after {
                let delta = e.health_after - e.health_before;
                fmt_push(b, &format!("  HEALTH: {}% \u{2192} {}% (\u{0394}{:+})\n", e.health_before, e.health_after, delta));
            }
            b.push('\n');
        }
    }

    fn write_final_state(&self, b: &mut String) {
        let (emoji, assess) = health_info(self.final_health);
        let delta = self.final_health - self.initial_health;
        let duration = self.end_elapsed.unwrap_or_default();
        let result = self.result.map_or("UNKNOWN".to_string(), |r| r.to_string());

        b.push_str("# FINAL_STATE\n");
        b.push_str("\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\n\n");
        fmt_push(b, &format!("Result:   {}\n", result));
        fmt_push(b, &format!("Exit:     {}\n", self.exit_code));
        fmt_push(b, &format!("Duration: {}\n\n", format_duration(duration)));
        fmt_push(b, &format!("Health:   {}% {}\n", self.final_health, emoji));
        fmt_push(b, &format!("          Initial: {}% \u{2192} Final: {}%\n", self.initial_health, self.final_health));
        fmt_push(b, &format!("          Delta: {:+} ({})\n\n", delta, assess));

        if !self.final_state.is_empty() {
            b.push_str("Component State:\n");
            let mut keys: Vec<_> = self.final_state.keys().collect();
            keys.sort();
            for k in keys {
                fmt_push(b, &format!("  {}: {}\n", k, self.final_state[k]));
            }
            b.push('\n');
        }
    }

    fn write_analysis(&self, b: &mut String) {
        b.push_str("# ANALYSIS\n");
        b.push_str("\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\n\n");

        b.push_str("Issues Detected:\n");
        if self.issues.is_empty() {
            b.push_str("  (none)\n");
        } else {
            for issue in &self.issues {
                fmt_push(b, &format!("  - {}\n", issue));
            }
        }
        b.push('\n');

        b.push_str("Performance:\n");
        if self.performance.is_empty() {
            b.push_str("  (not measured)\n");
        } else {
            let mut keys: Vec<_> = self.performance.keys().collect();
            keys.sort();
            for k in keys {
                fmt_push(b, &format!("  {}: {}\n", k, self.performance[k]));
            }
        }
        b.push('\n');

        b.push_str("Recommendations:\n");
        if self.recommendations.is_empty() {
            b.push_str("  (none)\n");
        } else {
            for rec in &self.recommendations {
                fmt_push(b, &format!("  - {}\n", rec));
            }
        }
        b.push('\n');
    }

    fn write_footer(&self, b: &mut String) {
        let (emoji, assess) = health_info(self.final_health);
        let duration = self.end_elapsed.unwrap_or_default();
        let ended = format_system_time(
            self.start_time + duration,
        );

        b.push_str("\u{250c}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2510}\n");
        b.push_str("\u{2502} Session Complete                                              \u{2502}\n");
        b.push_str("\u{251c}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2524}\n");
        fmt_push(b, &format!("\u{2502} Context ID: {:<51}\u{2502}\n", truncate(&self.context_id, 51)));
        fmt_push(b, &format!("\u{2502} Duration:   {:<51}\u{2502}\n", format_duration(duration)));
        fmt_push(b, &format!("\u{2502} Health:     {}% {} {:<43}\u{2502}\n", self.final_health, emoji, format!("({})", assess)));
        fmt_push(b, &format!("\u{2502} Ended:      {:<51}\u{2502}\n", ended));
        b.push_str("\u{2514}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2500}\u{2518}\n");
        b.push_str("# DEBUG_SESSION_END\n");
    }
}

// ────────────────────────────────────────────────────────────────
// Registry Integration
// ────────────────────────────────────────────────────────────────

/// Builds a `Config` from cross-layer database queries via the registry.
///
/// Queries L2 platform `health_levels` for emoji/label mapping, sorted
/// descending by min so `health_info()` finds the first match correctly.
/// Uses `Box::leak` for `&'static str` references.
pub fn config_from_registry(r: &bereshit_l2_registry::Registry) -> Config {
    let mut c = Config::default();

    if let Ok(levels) = r.platform.all_health_levels() {
        if !levels.is_empty() {
            let mut entries: Vec<HealthEmojiEntry> = levels
                .iter()
                .map(|hl| HealthEmojiEntry {
                    min: hl.min_stored as i32,
                    emoji: Box::leak(hl.emoji.clone().into_boxed_str()),
                    label: Box::leak(hl.name.clone().into_boxed_str()),
                })
                .collect();

            // Sort descending by min — health_info() walks top-down
            entries.sort_by(|a, b| b.min.cmp(&a.min));
            c.health_emoji = entries;
        }
    }

    c
}

// =============================================================================
// CLOSING
// =============================================================================
//
// This crate provides structured debugging for CPI-SI systems.
// Rust counterpart of debugging.go — same API surface, Rust idioms.
//
// "Then did he see it, and declare it; he prepared it,
//  yea, and searched it out." — Job 28:27
//
// Safe to add: New event types, severity levels, output sections
// Modify with care: Section markers (parsers depend on them)
// Never modify: Session/Event field semantics, level gating logic

// ────────────────────────────────────────────────────────────────
// C.1 Helpers
// ────────────────────────────────────────────────────────────────

/// Returns (emoji, assessment_text) for a health percentage.
fn health_info(h: i32) -> (&'static str, &'static str) {
    let c = cfg_ref();
    for entry in &c.health_emoji {
        if h >= entry.min {
            return (entry.emoji, entry.label);
        }
    }
    ("\u{2620}\u{fe0f}", "Failed")
}

/// Truncates a string to `max` characters, appending "..." if needed.
fn truncate(s: &str, max: usize) -> String {
    if s.len() <= max {
        s.to_string()
    } else {
        format!("{}...", &s[..max.saturating_sub(3)])
    }
}

/// Formats a `Duration` for display.
fn format_duration(d: Duration) -> String {
    let secs = d.as_secs_f64();
    if secs < 1.0 {
        format!("{:.3}s", secs)
    } else if secs < 60.0 {
        format!("{:.2}s", secs)
    } else {
        let mins = (secs / 60.0).floor() as u64;
        let remainder = secs - (mins as f64 * 60.0);
        format!("{}m {:.1}s", mins, remainder)
    }
}

/// Formats a duration as an elapsed-timestamp string (e.g. "+0.123s").
fn format_duration_timestamp(d: Duration) -> String {
    format!("+{:.3}s", d.as_secs_f64())
}

/// Formats a `SystemTime` as "YYYY-MM-DD HH:MM:SS.mmm".
///
/// Uses UTC since we avoid external crate dependencies.
fn format_system_time(t: SystemTime) -> String {
    let dur = t.duration_since(SystemTime::UNIX_EPOCH).unwrap_or_default();
    let total_secs = dur.as_secs();
    let millis = dur.subsec_millis();

    // Break down to date/time components (UTC)
    let days = total_secs / 86400;
    let time_secs = total_secs % 86400;
    let hours = time_secs / 3600;
    let minutes = (time_secs % 3600) / 60;
    let seconds = time_secs % 60;

    // Civil date from days since epoch (algorithm from Howard Hinnant)
    let (year, month, day) = civil_from_days(days as i64);

    format!(
        "{:04}-{:02}-{:02} {:02}:{:02}:{:02}.{:03}",
        year, month, day, hours, minutes, seconds, millis
    )
}

/// Converts days since Unix epoch to (year, month, day) — UTC.
///
/// Algorithm from Howard Hinnant's `chrono`-compatible date math.
fn civil_from_days(days: i64) -> (i64, u32, u32) {
    let z = days + 719468;
    let era = if z >= 0 { z } else { z - 146096 } / 146097;
    let doe = (z - era * 146097) as u32;
    let yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    let y = yoe as i64 + era * 400;
    let doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    let mp = (5 * doy + 2) / 153;
    let d = doy - (153 * mp + 2) / 5 + 1;
    let m = if mp < 10 { mp + 3 } else { mp - 9 };
    let y = if m <= 2 { y + 1 } else { y };
    (y, m, d)
}

/// Reads PRETTY_NAME from /etc/os-release.
fn read_distro() -> Option<String> {
    let content = std::fs::read_to_string("/etc/os-release").ok()?;
    for line in content.lines() {
        if let Some(rest) = line.strip_prefix("PRETTY_NAME=") {
            return Some(rest.trim_matches('"').to_string());
        }
    }
    None
}

/// Helper to push a formatted string (avoids repetitive `b.push_str(&...)`).
fn fmt_push(b: &mut String, s: &str) {
    b.push_str(s);
}

// ────────────────────────────────────────────────────────────────
// C.2 Tests
// ────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn level_ordering() {
        assert!(Level::Basic < Level::Detailed);
        assert!(Level::Detailed < Level::Verbose);
    }

    #[test]
    fn level_display() {
        assert_eq!(Level::Basic.to_string(), "BASIC");
        assert_eq!(Level::Detailed.to_string(), "DETAILED");
        assert_eq!(Level::Verbose.to_string(), "VERBOSE");
    }

    #[test]
    fn event_type_display() {
        assert_eq!(EventType::Init.to_string(), "INIT");
        assert_eq!(EventType::Error.to_string(), "ERROR");
        assert_eq!(EventType::Checkpoint.to_string(), "CHECKPOINT");
    }

    #[test]
    fn severity_display() {
        assert_eq!(Severity::Debug.to_string(), "DEBUG");
        assert_eq!(Severity::Info.to_string(), "INFO");
        assert_eq!(Severity::Warn.to_string(), "WARN");
        assert_eq!(Severity::Error.to_string(), "ERROR");
    }

    #[test]
    fn result_display() {
        assert_eq!(Result::Success.to_string(), "SUCCESS");
        assert_eq!(Result::Failed.to_string(), "FAILED");
        assert_eq!(Result::Timeout.to_string(), "TIMEOUT");
    }

    #[test]
    fn health_info_excellent() {
        let (_, text) = health_info(95);
        assert_eq!(text, "Excellent");
    }

    #[test]
    fn health_info_good() {
        let (_, text) = health_info(75);
        assert_eq!(text, "Good");
    }

    #[test]
    fn health_info_failed() {
        let (_, text) = health_info(0);
        assert_eq!(text, "Failed");
    }

    #[test]
    fn health_info_boundary() {
        let (_, text) = health_info(90);
        assert_eq!(text, "Excellent");
        let (_, text) = health_info(89);
        assert_eq!(text, "Good");
    }

    #[test]
    fn truncate_short() {
        assert_eq!(truncate("hello", 10), "hello");
    }

    #[test]
    fn truncate_exact() {
        assert_eq!(truncate("hello", 5), "hello");
    }

    #[test]
    fn truncate_long() {
        let result = truncate("a very long string indeed", 10);
        assert_eq!(result.len(), 10);
        assert!(result.ends_with("..."));
    }

    #[test]
    fn format_duration_subsecond() {
        let d = Duration::from_millis(123);
        assert_eq!(format_duration(d), "0.123s");
    }

    #[test]
    fn format_duration_seconds() {
        let d = Duration::from_secs_f64(5.25);
        assert_eq!(format_duration(d), "5.25s");
    }

    #[test]
    fn format_duration_minutes() {
        let d = Duration::from_secs(90);
        let result = format_duration(d);
        assert!(result.starts_with("1m "));
    }

    #[test]
    fn clamp_health() {
        let mut s = Session::new("test", Level::Basic);
        s.set_initial_health(150);
        assert_eq!(s.initial_health, 100);
        s.set_initial_health(-10);
        assert_eq!(s.initial_health, 0);
    }

    #[test]
    fn basic_level_skips_events() {
        let mut s = Session::new("test", Level::Basic);
        s.info(EventType::Init, "test event", HashMap::new(), 50, 50);
        assert!(s.events.is_empty(), "BASIC level should not record events");
    }

    #[test]
    fn detailed_level_records_events() {
        let mut s = Session::new("test", Level::Detailed);
        s.info(EventType::Init, "test event", HashMap::new(), 50, 55);
        assert_eq!(s.events.len(), 1);
        assert_eq!(s.events[0].description, "test event");
    }

    #[test]
    fn verbose_level_records_analysis() {
        let mut s = Session::new("test", Level::Verbose);
        s.add_issue("test issue");
        s.add_performance("latency", "5ms");
        s.add_recommendation("optimize queries");
        assert_eq!(s.issues.len(), 1);
        assert_eq!(s.performance.len(), 1);
        assert_eq!(s.recommendations.len(), 1);
    }

    #[test]
    fn detailed_level_skips_analysis() {
        let mut s = Session::new("test", Level::Detailed);
        s.add_issue("test issue");
        s.add_performance("latency", "5ms");
        s.add_recommendation("optimize queries");
        assert!(s.issues.is_empty());
        assert!(s.performance.is_empty());
        assert!(s.recommendations.is_empty());
    }

    #[test]
    fn session_new_captures_context() {
        let s = Session::new("test-component", Level::Basic);
        assert_eq!(s.component, "test-component");
        assert!(!s.context_id.is_empty());
        assert!(s.pid > 0);
        assert!(!s.username.is_empty());
        assert!(!s.kernel.is_empty());
    }

    #[test]
    fn session_parameters() {
        let mut s = Session::new("test", Level::Basic);
        s.set_parameter("key1", "val1");
        s.set_parameter("key2", "val2");
        assert_eq!(s.parameters.len(), 2);
        assert_eq!(s.parameters["key1"], "val1");
    }

    #[test]
    fn session_initial_state() {
        let mut s = Session::new("test", Level::Basic);
        s.set_initial_state("status", "running");
        assert_eq!(s.initial_state["status"], "running");
    }

    #[test]
    fn render_basic_session() {
        let mut s = Session::new("test", Level::Basic);
        s.set_output(Box::new(Vec::<u8>::new()));
        s.set_initial_health(80);

        let mut final_state = HashMap::new();
        final_state.insert("status".into(), "done".into());
        s.end(Result::Success, 0, 85, final_state);

        let output = s.render();
        assert!(output.contains("DEBUG_SESSION_START"));
        assert!(output.contains("DEBUG_SESSION_END"));
        assert!(output.contains("test"));
        assert!(output.contains("INITIAL_STATE"));
        assert!(output.contains("FINAL_STATE"));
        // Basic should NOT have EVENTS or ANALYSIS
        assert!(!output.contains("# EVENTS"));
        assert!(!output.contains("# ANALYSIS"));
    }

    #[test]
    fn render_detailed_session() {
        let mut s = Session::new("test", Level::Detailed);
        s.set_output(Box::new(Vec::<u8>::new()));
        s.set_initial_health(80);
        s.info(EventType::Init, "started", HashMap::new(), 80, 80);
        s.end(Result::Success, 0, 85, HashMap::new());

        let output = s.render();
        assert!(output.contains("# EVENTS"));
        assert!(output.contains("started"));
        // Detailed should NOT have ANALYSIS
        assert!(!output.contains("# ANALYSIS"));
    }

    #[test]
    fn render_verbose_session() {
        let mut s = Session::new("test", Level::Verbose);
        s.set_output(Box::new(Vec::<u8>::new()));
        s.set_initial_health(60);
        s.info(EventType::Validate, "checking", HashMap::new(), 60, 65);
        s.add_issue("config drift detected");
        s.add_performance("latency", "12ms");
        s.add_recommendation("run reconciliation");
        s.end(Result::Partial, 1, 65, HashMap::new());

        let output = s.render();
        assert!(output.contains("# EVENTS"));
        assert!(output.contains("# ANALYSIS"));
        assert!(output.contains("config drift detected"));
        assert!(output.contains("12ms"));
        assert!(output.contains("run reconciliation"));
    }

    #[test]
    fn civil_from_days_epoch() {
        let (y, m, d) = civil_from_days(0);
        assert_eq!((y, m, d), (1970, 1, 1));
    }

    #[test]
    fn civil_from_days_known_date() {
        // 2026-02-15 = 20499 days since epoch
        let (y, m, d) = civil_from_days(20499);
        assert_eq!((y, m, d), (2026, 2, 15));
    }

    #[test]
    fn config_default_has_6_health_entries() {
        let c = Config::default();
        assert_eq!(c.health_emoji.len(), 6);
    }

    #[test]
    fn config_default_sorted_descending() {
        let c = Config::default();
        for i in 1..c.health_emoji.len() {
            assert!(
                c.health_emoji[i - 1].min >= c.health_emoji[i].min,
                "health_emoji should be sorted descending by min"
            );
        }
    }

    #[test]
    fn cfg_ref_returns_defaults() {
        let c = cfg_ref();
        assert_eq!(c.health_emoji.len(), 6);
    }
}
