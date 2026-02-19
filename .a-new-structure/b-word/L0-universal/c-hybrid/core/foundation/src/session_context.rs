//omni:code --rust -library
//omni:key B-L0-foundation-sessioncontext-rs
//omni:version b-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-foundation-sessioncontext-rs
// Purpose: Substrate session context schema for runtime integration
// Biblical: 1 Corinthians 14:33 — God is not the author of confusion
// Version: b-02.00

//! # Session Context
//!
//! Substrate session context schema for runtime integration.
//!
//! | Type | Models | When |
//! |------|--------|------|
//! | [`SessionContext`] | Full hook event payload | Hook processing |
//! | [`Model`] | Substrate model identification | Identity |
//! | [`Workspace`] | Working directory paths | Path resolution |
//! | [`OutputStyle`] | Active output configuration | Display |
//! | [`Cost`] | Session cost and timing metrics | Monitoring |
//! | [`ContextWindow`] | Token usage and capacity | Resource tracking |
//! | [`CurrentUsage`] | Per-turn token breakdown | Efficiency analysis |
//!
//! This is the Claude Code statusline API schema — a generic substrate
//! integration contract. Promoted from L3-cpisi to L0-universal because
//! any layer may need to understand session context.

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

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

/// Session context from Claude Code hook events.
///
/// The contract between the substrate runtime and our systems.
/// Carries identity, workspace, costs, and context window data.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::SessionContext;
///
/// let ctx = SessionContext::default();
/// assert!(ctx.session_id.is_empty());
/// assert_eq!(ctx.context_percentage(), 0.0);
/// ```
#[derive(Debug, Clone, Serialize, Deserialize)]
#[derive(Default)]
pub struct SessionContext {
    /// Hook event that produced this context (e.g., "Status", "SessionStart").
    pub hook_event_name: String,
    /// Unique session identifier.
    pub session_id: String,
    /// Path to the conversation transcript.
    pub transcript_path: String,
    /// Current working directory.
    pub cwd: String,
    /// Substrate version string.
    pub version: String,
    /// Active model.
    pub model: Model,
    /// Workspace paths.
    pub workspace: Workspace,
    /// Active output style.
    pub output_style: OutputStyle,
    /// Cost and timing metrics.
    pub cost: Cost,
    /// Context window usage.
    pub context_window: ContextWindow,
}

/// Model identification.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::Model;
///
/// let m = Model { id: "claude-opus-4-6".into(), display_name: "Opus 4.6".into() };
/// assert_eq!(m.to_string(), "Opus 4.6 (claude-opus-4-6)");
/// ```
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
#[derive(Default)]
pub struct Model {
    /// Model identifier (e.g., "claude-opus-4-6").
    pub id: String,
    /// Human-readable name (e.g., "Opus 4.6").
    pub display_name: String,
}

/// Workspace paths.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
#[derive(Default)]
pub struct Workspace {
    /// Current working directory.
    pub current_dir: String,
    /// Project root directory.
    pub project_dir: String,
}

/// Active output style.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
#[derive(Default)]
pub struct OutputStyle {
    /// Style name (e.g., "default", "Nova Dawn").
    pub name: String,
}

/// Session cost and timing metrics.
///
/// All monetary values in USD. All durations in milliseconds.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Cost {
    /// Total session cost in USD.
    pub total_cost_usd: f64,
    /// Total wall-clock duration in milliseconds.
    pub total_duration_ms: i64,
    /// Total API call duration in milliseconds.
    pub total_api_duration_ms: i64,
    /// Total lines of code added.
    pub total_lines_added: i64,
    /// Total lines of code removed.
    pub total_lines_removed: i64,
}

/// Context window tracking.
///
/// Tracks cumulative token counts and current per-turn usage.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
#[derive(Default)]
pub struct ContextWindow {
    /// Cumulative input tokens across all turns.
    pub total_input_tokens: i64,
    /// Cumulative output tokens across all turns.
    pub total_output_tokens: i64,
    /// Maximum context window capacity.
    pub context_window_size: i64,
    /// Current turn's token breakdown.
    pub current_usage: CurrentUsage,
    /// Pre-computed usage percentage (from substrate).
    #[serde(default)]
    pub used_percentage: i64,
    /// Pre-computed remaining percentage (from substrate).
    #[serde(default)]
    pub remaining_percentage: i64,
}

/// Current turn's token breakdown.
///
/// More accurate than cumulative totals for efficiency analysis.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
#[derive(Default)]
pub struct CurrentUsage {
    /// Input tokens this turn.
    pub input_tokens: i64,
    /// Output tokens this turn.
    pub output_tokens: i64,
    /// Tokens written to cache this turn.
    pub cache_creation_input_tokens: i64,
    /// Tokens read from cache this turn.
    pub cache_read_input_tokens: i64,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Subsection order:
//   3. Constructors   — Default impls
//   5. Queries        — Computed metrics and accessors
//   6. Output         — Display formatting

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors
// ──────────────────────────────────────────────────────────────────────────




impl Default for Cost {
    fn default() -> Self {
        Self {
            total_cost_usd: 0.0,
            total_duration_ms: 0,
            total_api_duration_ms: 0,
            total_lines_added: 0,
            total_lines_removed: 0,
        }
    }
}




// ──────────────────────────────────────────────────────────────────────────
// 5. Queries & Accessors
// ──────────────────────────────────────────────────────────────────────────

impl SessionContext {
    /// Best available working directory: CWD > current_dir > project_dir.
    ///
    /// Cascading fallback — use the most specific path available.
    pub fn work_dir(&self) -> &str {
        if !self.cwd.is_empty() {
            &self.cwd
        } else if !self.workspace.current_dir.is_empty() {
            &self.workspace.current_dir
        } else {
            &self.workspace.project_dir
        }
    }

    /// Total lines changed (added + removed).
    pub fn total_lines_changed(&self) -> i64 {
        self.cost.total_lines_added + self.cost.total_lines_removed
    }

    /// Actual tokens currently in context (input + output this turn).
    pub fn current_context_tokens(&self) -> i64 {
        self.context_window.current_usage.input_tokens
            + self.context_window.current_usage.output_tokens
    }

    /// Context usage percentage (0.0-100.0).
    ///
    /// Uses current turn tokens when available, falls back to cumulative.
    /// Returns 0.0 if context window size is 0 (prevents division by zero).
    pub fn context_percentage(&self) -> f64 {
        if self.context_window.context_window_size == 0 {
            return 0.0;
        }
        let current = self.current_context_tokens();
        let tokens = if current > 0 {
            current
        } else {
            self.context_window.total_input_tokens
                + self.context_window.total_output_tokens
        };
        tokens as f64 / self.context_window.context_window_size as f64 * 100.0
    }

    /// Cache efficiency: log3(cache/input), capped at +/-3, scaled to -100..+100.
    ///
    /// | Value | Meaning |
    /// |-------|---------|
    /// | +100 | All cache, no fresh input |
    /// | 0 | Balanced (cache == input) |
    /// | -100 | No cache, all fresh input |
    ///
    /// Returns 0.0 when both input and cache are zero.
    pub fn cache_efficiency(&self) -> f64 {
        let input = self.context_window.current_usage.input_tokens;
        let cache = self.context_window.current_usage.cache_read_input_tokens;

        if input == 0 && cache == 0 {
            return 0.0;
        }
        if input == 0 {
            return 100.0;
        }
        if cache == 0 {
            return -100.0;
        }

        let ln3 = 3.0_f64.ln();
        let log_ratio = ((cache as f64).ln() - (input as f64).ln()) / ln3;
        let capped = log_ratio.clamp(-3.0, 3.0);
        capped * (100.0 / 3.0)
    }

    /// Session duration in seconds.
    pub fn duration_seconds(&self) -> i64 {
        self.cost.total_duration_ms / 1000
    }

    /// API call duration in seconds.
    pub fn api_duration_seconds(&self) -> i64 {
        self.cost.total_api_duration_ms / 1000
    }

    /// API efficiency: percentage of session time spent in API calls (0.0-100.0).
    ///
    /// Returns 0.0 if total duration is zero.
    pub fn api_efficiency(&self) -> f64 {
        if self.cost.total_duration_ms == 0 {
            return 0.0;
        }
        self.cost.total_api_duration_ms as f64
            / self.cost.total_duration_ms as f64
            * 100.0
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 6. Output & Display
// ──────────────────────────────────────────────────────────────────────────

impl fmt::Display for SessionContext {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "[{}] {} — {} ({:.1}% context, ${:.4})",
            self.session_id,
            self.model.display_name,
            self.hook_event_name,
            self.context_percentage(),
            self.cost.total_cost_usd,
        )
    }
}

impl fmt::Display for Model {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} ({})", self.display_name, self.id)
    }
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "God is not the author of confusion, but of peace."
// — 1 Corinthians 14:33

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    /// Build a populated test context with realistic values.
    fn test_ctx() -> SessionContext {
        SessionContext {
            hook_event_name: "Status".into(),
            session_id: "test-123".into(),
            transcript_path: "/tmp/test.jsonl".into(),
            cwd: "/home/test".into(),
            version: "1.0.0".into(),
            model: Model {
                id: "claude-opus-4-6".into(),
                display_name: "Opus 4.6".into(),
            },
            workspace: Workspace {
                current_dir: "/home/test/project".into(),
                project_dir: "/home/test/project".into(),
            },
            output_style: OutputStyle {
                name: "default".into(),
            },
            cost: Cost {
                total_cost_usd: 0.5,
                total_duration_ms: 60000,
                total_api_duration_ms: 30000,
                total_lines_added: 100,
                total_lines_removed: 20,
            },
            context_window: ContextWindow {
                total_input_tokens: 50000,
                total_output_tokens: 10000,
                context_window_size: 200000,
                current_usage: CurrentUsage {
                    input_tokens: 40000,
                    output_tokens: 8000,
                    cache_creation_input_tokens: 5000,
                    cache_read_input_tokens: 35000,
                },
                used_percentage: 24,
                remaining_percentage: 76,
            },
        }
    }

    // -- Defaults --------------------------------------------------------

    #[test]
    fn defaults_are_zeroed() {
        let ctx = SessionContext::default();
        assert!(ctx.session_id.is_empty());
        assert_eq!(ctx.cost.total_cost_usd, 0.0);
        assert_eq!(ctx.context_window.context_window_size, 0);
        assert_eq!(ctx.context_window.current_usage.input_tokens, 0);
    }

    #[test]
    fn model_default() {
        let m = Model::default();
        assert!(m.id.is_empty());
        assert!(m.display_name.is_empty());
    }

    #[test]
    fn current_usage_default() {
        let u = CurrentUsage::default();
        assert_eq!(u.input_tokens, 0);
        assert_eq!(u.cache_read_input_tokens, 0);
    }

    // -- work_dir cascade ------------------------------------------------

    #[test]
    fn work_dir_prefers_cwd() {
        let ctx = test_ctx();
        assert_eq!(ctx.work_dir(), "/home/test");
    }

    #[test]
    fn work_dir_falls_back_to_current_dir() {
        let mut ctx = test_ctx();
        ctx.cwd = String::new();
        assert_eq!(ctx.work_dir(), "/home/test/project");
    }

    #[test]
    fn work_dir_falls_back_to_project_dir() {
        let mut ctx = test_ctx();
        ctx.cwd = String::new();
        ctx.workspace.current_dir = String::new();
        assert_eq!(ctx.work_dir(), "/home/test/project");
    }

    #[test]
    fn work_dir_all_empty() {
        let ctx = SessionContext::default();
        assert_eq!(ctx.work_dir(), "");
    }

    // -- Computed metrics ------------------------------------------------

    #[test]
    fn total_lines() {
        let ctx = test_ctx();
        assert_eq!(ctx.total_lines_changed(), 120);
    }

    #[test]
    fn context_tokens() {
        let ctx = test_ctx();
        assert_eq!(ctx.current_context_tokens(), 48000);
    }

    #[test]
    fn context_pct() {
        let ctx = test_ctx();
        let pct = ctx.context_percentage();
        // 48000 / 200000 = 24%
        assert!(pct > 23.0 && pct < 25.0, "expected ~24%, got {pct}");
    }

    #[test]
    fn context_pct_zero_window() {
        let ctx = SessionContext::default();
        assert_eq!(ctx.context_percentage(), 0.0);
    }

    #[test]
    fn context_pct_fallback_to_cumulative() {
        let mut ctx = test_ctx();
        ctx.context_window.current_usage.input_tokens = 0;
        ctx.context_window.current_usage.output_tokens = 0;
        // Falls back to total: 50000 + 10000 = 60000 / 200000 = 30%
        let pct = ctx.context_percentage();
        assert!(pct > 29.0 && pct < 31.0, "expected ~30%, got {pct}");
    }

    #[test]
    fn api_efficiency() {
        let ctx = test_ctx();
        assert!((ctx.api_efficiency() - 50.0).abs() < 0.01);
    }

    #[test]
    fn api_efficiency_zero_duration() {
        let ctx = SessionContext::default();
        assert_eq!(ctx.api_efficiency(), 0.0);
    }

    #[test]
    fn duration_seconds() {
        let ctx = test_ctx();
        assert_eq!(ctx.duration_seconds(), 60);
        assert_eq!(ctx.api_duration_seconds(), 30);
    }

    // -- Cache efficiency edge cases -------------------------------------

    #[test]
    fn cache_efficiency_balanced() {
        let ctx = test_ctx();
        let eff = ctx.cache_efficiency();
        // cache=35000, input=40000 — slightly more input than cache
        assert!(eff > -20.0 && eff < 0.0, "expected slightly negative, got {eff}");
    }

    #[test]
    fn cache_efficiency_zero_zero() {
        let ctx = SessionContext::default();
        assert_eq!(ctx.cache_efficiency(), 0.0);
    }

    #[test]
    fn cache_efficiency_all_cache() {
        let mut ctx = test_ctx();
        ctx.context_window.current_usage.input_tokens = 0;
        ctx.context_window.current_usage.cache_read_input_tokens = 1000;
        assert_eq!(ctx.cache_efficiency(), 100.0);
    }

    #[test]
    fn cache_efficiency_no_cache() {
        let mut ctx = test_ctx();
        ctx.context_window.current_usage.cache_read_input_tokens = 0;
        assert_eq!(ctx.cache_efficiency(), -100.0);
    }

    #[test]
    fn cache_efficiency_equal() {
        let mut ctx = test_ctx();
        ctx.context_window.current_usage.input_tokens = 10000;
        ctx.context_window.current_usage.cache_read_input_tokens = 10000;
        // log3(1) = 0 -> 0.0
        assert!(ctx.cache_efficiency().abs() < 0.01);
    }

    // -- Display ---------------------------------------------------------

    #[test]
    fn session_context_display() {
        let ctx = test_ctx();
        let s = ctx.to_string();
        assert!(s.contains("test-123"), "missing session ID: {s}");
        assert!(s.contains("Opus 4.6"), "missing model name: {s}");
        assert!(s.contains("Status"), "missing event name: {s}");
    }

    #[test]
    fn model_display() {
        let m = Model {
            id: "claude-opus-4-6".into(),
            display_name: "Opus 4.6".into(),
        };
        assert_eq!(m.to_string(), "Opus 4.6 (claude-opus-4-6)");
    }

    // -- Serde roundtrip -------------------------------------------------

    #[test]
    fn serde_roundtrip() {
        let original = test_ctx();
        let json = serde_json::to_string(&original).unwrap();
        let restored: SessionContext = serde_json::from_str(&json).unwrap();
        assert_eq!(restored.session_id, original.session_id);
        assert_eq!(restored.model, original.model);
        assert_eq!(
            restored.context_window.current_usage,
            original.context_window.current_usage
        );
        assert_eq!(
            restored.cost.total_lines_added,
            original.cost.total_lines_added
        );
    }

    #[test]
    fn serde_default_percentages() {
        // Percentages should default to 0 when missing from JSON
        let json = r#"{
            "hook_event_name":"S","session_id":"x","transcript_path":"",
            "cwd":"","version":"","model":{"id":"","display_name":""},
            "workspace":{"current_dir":"","project_dir":""},
            "output_style":{"name":""},
            "cost":{"total_cost_usd":0,"total_duration_ms":0,"total_api_duration_ms":0,"total_lines_added":0,"total_lines_removed":0},
            "context_window":{"total_input_tokens":0,"total_output_tokens":0,"context_window_size":0,
            "current_usage":{"input_tokens":0,"output_tokens":0,"cache_creation_input_tokens":0,"cache_read_input_tokens":0}}
        }"#;
        let ctx: SessionContext = serde_json::from_str(json).unwrap();
        assert_eq!(ctx.context_window.used_percentage, 0);
        assert_eq!(ctx.context_window.remaining_percentage, 0);
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
// Serde types — no resources to clean up.

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new query/accessor methods on SessionContext
//   - Add Display impls to sub-types
//   - Add tests (especially edge cases)
//
// Modify with Care:
//   - Struct fields — this is a wire protocol contract
//   - Field names — serde renames affect JSON compatibility
//   - cache_efficiency formula — downstream depends on the scale
//
// Never Modify:
//   - SessionContext field set — mirrors the substrate API
//   - CurrentUsage field set — tight coupling to token accounting
//   - The +100/-100 scale of cache_efficiency

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// Substrate session context — the contract between runtime and system.
// Promoted from L3 to L0. Schema types with computed accessors.
//
// "God is not the author of confusion, but of peace."
// — 1 Corinthians 14:33

// ============================================================================
// END CLOSING
// ============================================================================
