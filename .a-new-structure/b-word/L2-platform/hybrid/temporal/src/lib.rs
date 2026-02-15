//omni:code --rust -library
//omni:key B-L2-temporal
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L2-temporal
// Purpose: Shared temporal awareness utilities
// Biblical: Ecclesiastes 3:1 — "To every thing there is a season"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// Used by: statusline, session-awareness, hooks
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// S.1 Imports
// ────────────────────────────────────────────────────────────────

use std::time::Duration;

// ────────────────────────────────────────────────────────────────
// S.2 Configuration
// ────────────────────────────────────────────────────────────────

/// A time-of-day range definition.
#[derive(Debug, Clone)]
pub struct TimeRange {
    pub name: &'static str,
    pub start: u32,
    pub end: u32,
    pub icon: &'static str,
    pub label: &'static str,
    pub color: &'static str,
}

/// A session duration range definition.
#[derive(Debug, Clone)]
pub struct SessionRange {
    pub name: &'static str,
    pub min_minutes: u64,
    pub max_minutes: u64,
    pub color: &'static str,
}

/// A circadian phase range definition.
#[derive(Debug, Clone)]
pub struct CircadianRange {
    pub name: &'static str,
    pub start: u32,
    pub end: u32,
    pub productivity: &'static str,
    pub note: &'static str,
}

/// Temporal configuration — all boundary definitions injectable from database.
#[derive(Debug, Clone)]
pub struct Config {
    pub time_ranges: Vec<TimeRange>,
    pub session_ranges: Vec<SessionRange>,
    pub circadian_ranges: Vec<CircadianRange>,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            time_ranges: vec![
                TimeRange { name: "morning",    start: 5,  end: 9,  icon: "\u{1f305}",           label: "morning",     color: bereshit_l2_display::YELLOW },
                TimeRange { name: "mid-morning", start: 9,  end: 12, icon: "\u{2600}\u{fe0f}",   label: "mid-morning", color: bereshit_l2_display::BRIGHT_YELLOW },
                TimeRange { name: "midday",     start: 12, end: 14, icon: "\u{1f31e}",           label: "midday",      color: bereshit_l2_display::YELLOW },
                TimeRange { name: "afternoon",  start: 14, end: 17, icon: "\u{1f324}\u{fe0f}",   label: "afternoon",   color: bereshit_l2_display::CYAN },
                TimeRange { name: "evening",    start: 17, end: 20, icon: "\u{1f306}",           label: "evening",     color: bereshit_l2_display::BLUE },
                TimeRange { name: "night",      start: 20, end: 23, icon: "\u{1f319}",           label: "night",       color: bereshit_l2_display::MAGENTA },
                TimeRange { name: "late-night", start: 23, end: 5,  icon: "\u{1f311}",           label: "late night",  color: concat!("\x1b[2m", "\x1b[34m") },
            ],
            session_ranges: vec![
                SessionRange { name: "fresh",     min_minutes: 0,   max_minutes: 15,  color: bereshit_l2_display::GREEN },
                SessionRange { name: "active",    min_minutes: 15,  max_minutes: 60,  color: bereshit_l2_display::CYAN },
                SessionRange { name: "sustained", min_minutes: 60,  max_minutes: 120, color: bereshit_l2_display::YELLOW },
                SessionRange { name: "long",      min_minutes: 120, max_minutes: 240, color: bereshit_l2_display::BLUE },
                SessionRange { name: "extended",  min_minutes: 240, max_minutes: u64::MAX, color: bereshit_l2_display::MAGENTA },
            ],
            circadian_ranges: vec![
                CircadianRange { name: "rising",   start: 5,  end: 9,  productivity: "increasing", note: "cortisol rising, alertness building" },
                CircadianRange { name: "peak",     start: 9,  end: 12, productivity: "high",       note: "peak cognitive performance" },
                CircadianRange { name: "dip",      start: 12, end: 14, productivity: "lower",      note: "post-lunch dip normal" },
                CircadianRange { name: "recovery", start: 14, end: 17, productivity: "moderate",   note: "second wind building" },
                CircadianRange { name: "evening",  start: 17, end: 21, productivity: "variable",   note: "creativity often high" },
                CircadianRange { name: "rest",     start: 21, end: 5,  productivity: "low",        note: "body preparing for rest" },
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
fn cfg() -> &'static Config {
    CFG.get_or_init(Config::default)
}

/// Returns true if `hour` falls within the range [start, end),
/// correctly handling midnight-wrapping ranges (e.g. 23..5).
fn in_hour_range(hour: u32, start: u32, end: u32) -> bool {
    if start < end {
        hour >= start && hour < end
    } else {
        hour >= start || hour < end // wraps midnight
    }
}

// ────────────────────────────────────────────────────────────────
// S.3 Core Types
// ────────────────────────────────────────────────────────────────

/// Time-of-day classification based on hour.
#[derive(Debug, Clone)]
pub struct TimeOfDay {
    pub name: &'static str,
    pub icon: &'static str,
    pub label: &'static str,
    pub color: &'static str,
    pub hour: u32,
}

/// Time-of-day display for statusline rendering.
#[derive(Debug, Clone)]
pub struct TimeOfDayDisplay {
    pub icon: &'static str,
    pub label: &'static str,
    pub color: &'static str,
    pub has_info: bool,
}

/// Session duration classification.
#[derive(Debug, Clone)]
pub struct SessionPhase {
    pub name: &'static str,
    pub duration: Duration,
    pub color: &'static str,
}

/// Session phase display for statusline rendering.
#[derive(Debug, Clone)]
pub struct SessionPhaseDisplay {
    pub phase: String,
    pub duration: String,
    pub color: String,
    pub has_info: bool,
}

/// Circadian rhythm phase classification.
#[derive(Debug, Clone)]
pub struct CircadianPhase {
    pub name: &'static str,
    pub productivity: &'static str,
    pub note: &'static str,
}

// ============================================================================
// BODY
// ============================================================================

// --- Time of Day ---

/// Classifies an hour (0..23) into a time-of-day category.
pub fn get_time_of_day(hour: u32) -> TimeOfDay {
    let c = cfg();
    for r in &c.time_ranges {
        if in_hour_range(hour, r.start, r.end) {
            return TimeOfDay {
                name: r.name,
                icon: r.icon,
                label: r.label,
                color: r.color,
                hour,
            };
        }
    }
    // Fallback — last range in config (late-night) or sensible default
    TimeOfDay {
        name: "unknown",
        icon: "",
        label: "unknown",
        color: "",
        hour,
    }
}

/// Returns display-ready time-of-day info for the given hour.
pub fn get_time_of_day_display(hour: u32) -> TimeOfDayDisplay {
    let tod = get_time_of_day(hour);
    TimeOfDayDisplay {
        icon: tod.icon,
        label: tod.label,
        color: tod.color,
        has_info: true,
    }
}

// --- Session Phase ---

/// Classifies session duration into a phase.
///
/// Phase thresholds (from config, defaults shown):
/// - fresh: < 15 min
/// - active: 15..60 min
/// - sustained: 60..120 min
/// - long: 120..240 min
/// - extended: > 240 min
pub fn get_session_phase(elapsed: Duration) -> SessionPhase {
    let minutes = elapsed.as_secs() / 60;
    let c = cfg();

    for r in &c.session_ranges {
        if minutes >= r.min_minutes && minutes < r.max_minutes {
            return SessionPhase {
                name: r.name,
                duration: elapsed,
                color: r.color,
            };
        }
    }
    // Fallback
    SessionPhase {
        name: "unknown",
        duration: elapsed,
        color: "",
    }
}

/// Returns display-ready session phase info.
///
/// If `elapsed_secs` is 0, returns `has_info: false`.
pub fn get_session_phase_display(elapsed_secs: u64) -> SessionPhaseDisplay {
    if elapsed_secs == 0 {
        return SessionPhaseDisplay {
            phase: String::new(),
            duration: String::new(),
            color: String::new(),
            has_info: false,
        };
    }

    let elapsed = Duration::from_secs(elapsed_secs);
    let phase = get_session_phase(elapsed);

    SessionPhaseDisplay {
        phase: phase.name.to_string(),
        duration: format_duration(elapsed),
        color: phase.color.to_string(),
        has_info: true,
    }
}

// --- Circadian Awareness ---

/// Returns the circadian rhythm phase for a given hour (0..23).
pub fn get_circadian_phase(hour: u32) -> CircadianPhase {
    let c = cfg();
    for r in &c.circadian_ranges {
        if in_hour_range(hour, r.start, r.end) {
            return CircadianPhase {
                name: r.name,
                productivity: r.productivity,
                note: r.note,
            };
        }
    }
    // Fallback
    CircadianPhase {
        name: "normal",
        productivity: "normal",
        note: "",
    }
}

// --- Duration Formatting ---

/// Formats a duration as a human-readable string (e.g., "2h 15m").
pub fn format_duration(d: Duration) -> String {
    let total_secs = d.as_secs();
    let hours = total_secs / 3600;
    let minutes = (total_secs % 3600) / 60;

    if hours > 0 {
        format!("{hours}h {minutes}m")
    } else {
        format!("{minutes}m")
    }
}

/// Formats a duration compactly without space (e.g., "2h15m").
pub fn format_duration_short(d: Duration) -> String {
    let total_secs = d.as_secs();
    let hours = total_secs / 3600;
    let minutes = (total_secs % 3600) / 60;

    if hours > 0 {
        format!("{hours}h{minutes}m")
    } else {
        format!("{minutes}m")
    }
}

// ────────────────────────────────────────────────────────────────
// Registry Integration
// ────────────────────────────────────────────────────────────────

/// Builds a `Config` from cross-layer database queries via the registry.
///
/// Queries L2 platform `temporal_boundaries` by category for time_of_day,
/// session_phase, and circadian ranges. Uses `Box::leak` to produce
/// `&'static str` references for Config fields that require them.
pub fn config_from_registry(r: &bereshit_l2_registry::Registry) -> Config {
    let mut c = Config::default();

    // Time-of-day ranges
    if let Ok(rows) = r.platform.temporal_by_category("time_of_day") {
        if !rows.is_empty() {
            c.time_ranges = rows
                .iter()
                .map(|tb| TimeRange {
                    name: Box::leak(tb.name.clone().into_boxed_str()),
                    start: tb.start_value as u32,
                    end: tb.end_value as u32,
                    icon: Box::leak(tb.icon.clone().into_boxed_str()),
                    label: Box::leak(tb.description.clone().into_boxed_str()),
                    color: Box::leak(tb.color.clone().into_boxed_str()),
                })
                .collect();
        }
    }

    // Session phase ranges
    if let Ok(rows) = r.platform.temporal_by_category("session_phase") {
        if !rows.is_empty() {
            c.session_ranges = rows
                .iter()
                .map(|tb| SessionRange {
                    name: Box::leak(tb.name.clone().into_boxed_str()),
                    min_minutes: tb.start_value as u64,
                    max_minutes: if tb.end_value < 0 {
                        u64::MAX
                    } else {
                        tb.end_value as u64
                    },
                    color: Box::leak(tb.color.clone().into_boxed_str()),
                })
                .collect();
        }
    }

    // Circadian ranges
    if let Ok(rows) = r.platform.temporal_by_category("circadian") {
        if !rows.is_empty() {
            c.circadian_ranges = rows
                .iter()
                .map(|tb| CircadianRange {
                    name: Box::leak(tb.name.clone().into_boxed_str()),
                    start: tb.start_value as u32,
                    end: tb.end_value as u32,
                    productivity: Box::leak(tb.description.clone().into_boxed_str()),
                    note: Box::leak(tb.extra.clone().into_boxed_str()),
                })
                .collect();
        }
    }

    c
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn time_of_day_morning() {
        let tod = get_time_of_day(7);
        assert_eq!(tod.name, "morning");
    }

    #[test]
    fn time_of_day_night() {
        let tod = get_time_of_day(21);
        assert_eq!(tod.name, "night");
    }

    #[test]
    fn time_of_day_late_night() {
        let tod = get_time_of_day(2);
        assert_eq!(tod.name, "late-night");
    }

    #[test]
    fn session_phase_fresh() {
        let phase = get_session_phase(Duration::from_secs(5 * 60));
        assert_eq!(phase.name, "fresh");
    }

    #[test]
    fn session_phase_extended() {
        let phase = get_session_phase(Duration::from_secs(300 * 60));
        assert_eq!(phase.name, "extended");
    }

    #[test]
    fn circadian_peak() {
        let phase = get_circadian_phase(10);
        assert_eq!(phase.name, "peak");
        assert_eq!(phase.productivity, "high");
    }

    #[test]
    fn format_duration_minutes_only() {
        let d = Duration::from_secs(45 * 60);
        assert_eq!(format_duration(d), "45m");
    }

    #[test]
    fn format_duration_hours_and_minutes() {
        let d = Duration::from_secs(2 * 3600 + 15 * 60);
        assert_eq!(format_duration(d), "2h 15m");
    }

    #[test]
    fn format_duration_short_compact() {
        let d = Duration::from_secs(1 * 3600 + 30 * 60);
        assert_eq!(format_duration_short(d), "1h30m");
    }

    #[test]
    fn config_default_has_7_time_ranges() {
        let c = Config::default();
        assert_eq!(c.time_ranges.len(), 7);
    }

    #[test]
    fn config_default_has_5_session_ranges() {
        let c = Config::default();
        assert_eq!(c.session_ranges.len(), 5);
    }

    #[test]
    fn config_default_has_6_circadian_ranges() {
        let c = Config::default();
        assert_eq!(c.circadian_ranges.len(), 6);
    }

    #[test]
    fn in_hour_range_normal() {
        assert!(in_hour_range(7, 5, 9));
        assert!(!in_hour_range(4, 5, 9));
        assert!(!in_hour_range(9, 5, 9)); // exclusive end
    }

    #[test]
    fn in_hour_range_midnight_wrap() {
        // 23..5 wraps midnight
        assert!(in_hour_range(23, 23, 5));
        assert!(in_hour_range(0, 23, 5));
        assert!(in_hour_range(3, 23, 5));
        assert!(!in_hour_range(5, 23, 5)); // exclusive end
        assert!(!in_hour_range(12, 23, 5));
    }

    #[test]
    fn time_of_day_all_hours_classified() {
        for h in 0..24 {
            let tod = get_time_of_day(h);
            assert!(!tod.name.is_empty(), "hour {h} should have a name");
        }
    }

    #[test]
    fn session_phase_all_durations() {
        let durations = [0, 5, 15, 30, 60, 90, 120, 180, 240, 500];
        for &m in &durations {
            let phase = get_session_phase(Duration::from_secs(m * 60));
            assert!(!phase.name.is_empty(), "{m}m should have a phase name");
        }
    }

    #[test]
    fn circadian_all_hours() {
        for h in 0..24 {
            let phase = get_circadian_phase(h);
            assert!(!phase.name.is_empty(), "hour {h} should have a circadian phase");
        }
    }

    #[test]
    fn time_of_day_display_has_info() {
        let d = get_time_of_day_display(10);
        assert!(d.has_info);
        assert!(!d.label.is_empty());
    }

    #[test]
    fn session_phase_display_zero_no_info() {
        let d = get_session_phase_display(0);
        assert!(!d.has_info);
    }

    #[test]
    fn session_phase_display_nonzero_has_info() {
        let d = get_session_phase_display(1800);
        assert!(d.has_info);
        assert!(!d.phase.is_empty());
    }
}
