// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L3-health
// Purpose: CPI-SI health monitoring — ternary scale and state tracking
// Biblical: "Beloved, I wish above all things that thou mayest prosper and
//   be in health" — 3 John 1:2
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-16
//
// Health tracking uses a ternary scale (-100 to +100) where:
//   -100 = critical negative, 0 = balanced, +100 = optimal
// This mirrors the K:ALIGN moral compass but for system vitality.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};

// ────────────────────────────────────────────────────────────────
// S.1 Constants
// ────────────────────────────────────────────────────────────────

/// Ternary scale boundaries (from health TOML config)
pub const TERNARY_MIN: i32 = -100;
pub const TERNARY_CENTER: i32 = 0;
pub const TERNARY_MAX: i32 = 100;

/// Health status thresholds
pub const THRESHOLD_CRITICAL: i32 = -60;
pub const THRESHOLD_DEGRADED: i32 = -20;
pub const THRESHOLD_STABLE: i32 = 20;
pub const THRESHOLD_STRONG: i32 = 60;

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Types
// ────────────────────────────────────────────────────────────────

/// Health status categories derived from the ternary score
#[derive(Clone, Debug, PartialEq, Eq, Serialize, Deserialize)]
pub enum HealthStatus {
    /// Score below -60: system in critical state
    Critical,
    /// Score -60 to -20: degraded but functional
    Degraded,
    /// Score -20 to +20: stable, balanced operation
    Stable,
    /// Score +20 to +60: strong, growing well
    Strong,
    /// Score above +60: optimal performance
    Optimal,
}

impl HealthStatus {
    /// Derive status from a ternary score
    pub fn from_score(score: i32) -> Self {
        match score {
            s if s < THRESHOLD_CRITICAL => Self::Critical,
            s if s < THRESHOLD_DEGRADED => Self::Degraded,
            s if s < THRESHOLD_STABLE => Self::Stable,
            s if s < THRESHOLD_STRONG => Self::Strong,
            _ => Self::Optimal,
        }
    }

    /// Human-readable label
    pub fn label(&self) -> &str {
        match self {
            Self::Critical => "CRITICAL",
            Self::Degraded => "DEGRADED",
            Self::Stable => "STABLE",
            Self::Strong => "STRONG",
            Self::Optimal => "OPTIMAL",
        }
    }
}

impl std::fmt::Display for HealthStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.label())
    }
}

/// A health snapshot at a point in time
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct HealthSnapshot {
    /// Ternary score: -100 to +100
    pub score: i32,
    /// Derived status
    pub status: HealthStatus,
    /// Score delta from previous snapshot
    pub delta: i32,
    /// Timestamp (ISO 8601 string)
    pub timestamp: String,
    /// Optional context about what caused the change
    pub context: String,
}

/// Tracks health over a session with rolling averages
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct HealthTracker {
    /// Current score
    pub current: i32,
    /// Session minimum
    pub min: i32,
    /// Session maximum
    pub max: i32,
    /// Running average (integer approximation)
    pub average: i32,
    /// Number of updates
    pub update_count: u32,
    /// Sum for average calculation
    sum: i64,
    /// History of snapshots (recent, not all)
    pub snapshots: Vec<HealthSnapshot>,
    /// Max snapshots to retain
    max_snapshots: usize,
}

// ────────────────────────────────────────────────────────────────
// B.2 HealthTracker Implementation
// ────────────────────────────────────────────────────────────────

impl HealthTracker {
    /// Create a new tracker starting at center (balanced)
    pub fn new() -> Self {
        Self {
            current: TERNARY_CENTER,
            min: TERNARY_CENTER,
            max: TERNARY_CENTER,
            average: TERNARY_CENTER,
            update_count: 0,
            sum: 0,
            snapshots: Vec::new(),
            max_snapshots: 50,
        }
    }

    /// Create a tracker with a specific starting score
    pub fn with_initial(score: i32) -> Self {
        let clamped = clamp_score(score);
        Self {
            current: clamped,
            min: clamped,
            max: clamped,
            average: clamped,
            update_count: 1,
            sum: clamped as i64,
            snapshots: Vec::new(),
            max_snapshots: 50,
        }
    }

    /// Update health with a new score and optional context
    pub fn update(&mut self, score: i32, context: &str) {
        let clamped = clamp_score(score);
        let delta = clamped - self.current;

        self.current = clamped;
        self.update_count += 1;
        self.sum += clamped as i64;
        self.average = (self.sum / self.update_count as i64) as i32;

        if clamped < self.min {
            self.min = clamped;
        }
        if clamped > self.max {
            self.max = clamped;
        }

        let snapshot = HealthSnapshot {
            score: clamped,
            status: HealthStatus::from_score(clamped),
            delta,
            timestamp: String::new(), // Caller provides timestamp
            context: context.to_string(),
        };

        self.snapshots.push(snapshot);
        if self.snapshots.len() > self.max_snapshots {
            self.snapshots.remove(0);
        }
    }

    /// Current health status
    pub fn status(&self) -> HealthStatus {
        HealthStatus::from_score(self.current)
    }

    /// Session health range (max - min)
    pub fn range(&self) -> i32 {
        self.max - self.min
    }

    /// Whether health has been volatile (range > 80)
    pub fn is_volatile(&self) -> bool {
        self.range() > 80
    }
}

impl Default for HealthTracker {
    fn default() -> Self {
        Self::new()
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 Helpers
// ────────────────────────────────────────────────────────────────

/// Clamp a score to the ternary range
pub fn clamp_score(score: i32) -> i32 {
    score.clamp(TERNARY_MIN, TERNARY_MAX)
}

/// Format a health score with sign for display
pub fn format_score(score: i32) -> String {
    if score > 0 {
        format!("+{score}")
    } else {
        format!("{score}")
    }
}

/// Format delta with arrow indicator
pub fn format_delta(delta: i32) -> String {
    match delta {
        d if d > 10 => format!("+{d} ↑↑"),
        d if d > 0 => format!("+{d} ↑"),
        0 => "0 →".to_string(),
        d if d > -10 => format!("{d} ↓"),
        d => format!("{d} ↓↓"),
    }
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn status_from_score_boundaries() {
        assert_eq!(HealthStatus::from_score(-100), HealthStatus::Critical);
        assert_eq!(HealthStatus::from_score(-61), HealthStatus::Critical);
        assert_eq!(HealthStatus::from_score(-60), HealthStatus::Degraded);
        assert_eq!(HealthStatus::from_score(-20), HealthStatus::Stable);
        assert_eq!(HealthStatus::from_score(0), HealthStatus::Stable);
        assert_eq!(HealthStatus::from_score(20), HealthStatus::Strong);
        assert_eq!(HealthStatus::from_score(60), HealthStatus::Optimal);
        assert_eq!(HealthStatus::from_score(100), HealthStatus::Optimal);
    }

    #[test]
    fn tracker_new_starts_at_center() {
        let t = HealthTracker::new();
        assert_eq!(t.current, 0);
        assert_eq!(t.status(), HealthStatus::Stable);
    }

    #[test]
    fn tracker_update_tracks_min_max() {
        let mut t = HealthTracker::new();
        t.update(50, "good exchange");
        t.update(-30, "correction");
        t.update(20, "recovery");

        assert_eq!(t.current, 20);
        assert_eq!(t.min, -30);
        assert_eq!(t.max, 50);
        assert_eq!(t.update_count, 3);
    }

    #[test]
    fn tracker_average_calculation() {
        let mut t = HealthTracker::new();
        t.update(100, "");
        t.update(0, "");
        t.update(-100, "");
        // Average of 100, 0, -100 = 0
        assert_eq!(t.average, 0);
    }

    #[test]
    fn clamp_score_enforces_bounds() {
        assert_eq!(clamp_score(200), 100);
        assert_eq!(clamp_score(-200), -100);
        assert_eq!(clamp_score(50), 50);
    }

    #[test]
    fn format_score_with_sign() {
        assert_eq!(format_score(42), "+42");
        assert_eq!(format_score(0), "0");
        assert_eq!(format_score(-15), "-15");
    }

    #[test]
    fn format_delta_arrows() {
        assert!(format_delta(15).contains("↑↑"));
        assert!(format_delta(5).contains("↑"));
        assert!(format_delta(0).contains("→"));
        assert!(format_delta(-5).contains("↓"));
        assert!(format_delta(-15).contains("↓↓"));
    }

    #[test]
    fn tracker_volatility() {
        let mut t = HealthTracker::new();
        t.update(50, "");
        t.update(-40, "");
        assert!(t.is_volatile()); // range = 90 > 80
    }

    #[test]
    fn snapshot_history_capped() {
        let mut t = HealthTracker::new();
        t.max_snapshots = 3;
        for i in 0..10 {
            t.update(i * 10, "");
        }
        assert_eq!(t.snapshots.len(), 3);
    }
}
