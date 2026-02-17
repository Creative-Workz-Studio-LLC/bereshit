//omni:code --rust -library
//omni:key B-L0-foundation-sessioncontext-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # Session Context (Rust)
//!
//! Substrate session context schema for runtime integration.
//!
//! This is the Claude Code statusline API schema — generic substrate
//! integration contract, not CPI-SI intelligence.
//!
//! Promoted from L3-cpisi to L0-universal.

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────

/// Session context from Claude Code hook events.
///
/// The contract between the substrate runtime and our systems.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SessionContext {
    pub hook_event_name: String,
    pub session_id: String,
    pub transcript_path: String,
    pub cwd: String,
    pub version: String,
    pub model: Model,
    pub workspace: Workspace,
    pub output_style: OutputStyle,
    pub cost: Cost,
    pub context_window: ContextWindow,
}

/// Model identification.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Model {
    pub id: String,
    pub display_name: String,
}

/// Workspace paths.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Workspace {
    pub current_dir: String,
    pub project_dir: String,
}

/// Active output style.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct OutputStyle {
    pub name: String,
}

/// Session cost metrics.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Cost {
    pub total_cost_usd: f64,
    pub total_duration_ms: i64,
    pub total_api_duration_ms: i64,
    pub total_lines_added: i64,
    pub total_lines_removed: i64,
}

/// Context window tracking.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ContextWindow {
    pub total_input_tokens: i64,
    pub total_output_tokens: i64,
    pub context_window_size: i64,
    pub current_usage: CurrentUsage,
    #[serde(default)]
    pub used_percentage: i64,
    #[serde(default)]
    pub remaining_percentage: i64,
}

/// Current context usage (more accurate than cumulative totals).
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CurrentUsage {
    pub input_tokens: i64,
    pub output_tokens: i64,
    pub cache_creation_input_tokens: i64,
    pub cache_read_input_tokens: i64,
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Helper Methods
// ────────────────────────────────────────────────────────────────

impl SessionContext {
    /// Best available working directory: CWD > current_dir > project_dir.
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

    /// Actual tokens currently in context.
    pub fn current_context_tokens(&self) -> i64 {
        self.context_window.current_usage.input_tokens
            + self.context_window.current_usage.output_tokens
    }

    /// Context usage percentage (0.0 - 100.0).
    pub fn context_percentage(&self) -> f64 {
        if self.context_window.context_window_size == 0 {
            return 0.0;
        }
        let current = self.current_context_tokens();
        if current > 0 {
            return current as f64 / self.context_window.context_window_size as f64 * 100.0;
        }
        let total = self.context_window.total_input_tokens
            + self.context_window.total_output_tokens;
        total as f64 / self.context_window.context_window_size as f64 * 100.0
    }

    /// Cache efficiency: log3(cache/input), capped at +/-3, scaled to -100..+100.
    pub fn cache_efficiency(&self) -> f64 {
        let input = self.context_window.current_usage.input_tokens;
        let cache = self.context_window.current_usage.cache_read_input_tokens;

        if input == 0 && cache == 0 { return 0.0; }
        if input == 0 { return 100.0; }
        if cache == 0 { return -100.0; }

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

    /// API efficiency: ratio of API time to total time (0.0 - 100.0).
    pub fn api_efficiency(&self) -> f64 {
        if self.cost.total_duration_ms == 0 {
            return 0.0;
        }
        self.cost.total_api_duration_ms as f64 / self.cost.total_duration_ms as f64 * 100.0
    }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Substrate session context — promoted from L3 to serve all layers.
// "God is not the author of confusion" — 1 Corinthians 14:33

#[cfg(test)]
mod tests {
    use super::*;

    fn test_ctx() -> SessionContext {
        SessionContext {
            hook_event_name: "Status".into(),
            session_id: "test-123".into(),
            transcript_path: "/tmp/test.jsonl".into(),
            cwd: "/home/test".into(),
            version: "1.0.0".into(),
            model: Model { id: "claude-opus-4-6".into(), display_name: "Opus 4.6".into() },
            workspace: Workspace { current_dir: "/home/test/project".into(), project_dir: "/home/test/project".into() },
            output_style: OutputStyle { name: "default".into() },
            cost: Cost { total_cost_usd: 0.5, total_duration_ms: 60000, total_api_duration_ms: 30000, total_lines_added: 100, total_lines_removed: 20 },
            context_window: ContextWindow {
                total_input_tokens: 50000,
                total_output_tokens: 10000,
                context_window_size: 200000,
                current_usage: CurrentUsage { input_tokens: 40000, output_tokens: 8000, cache_creation_input_tokens: 5000, cache_read_input_tokens: 35000 },
                used_percentage: 24,
                remaining_percentage: 76,
            },
        }
    }

    #[test]
    fn work_dir_prefers_cwd() {
        let ctx = test_ctx();
        assert_eq!(ctx.work_dir(), "/home/test");
    }

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
        assert!(pct > 20.0 && pct < 30.0);
    }

    #[test]
    fn api_efficiency() {
        let ctx = test_ctx();
        assert!((ctx.api_efficiency() - 50.0).abs() < 0.01);
    }

    #[test]
    fn cache_efficiency_balanced() {
        // cache_read = 35000, input = 40000 — close to balanced
        let ctx = test_ctx();
        let eff = ctx.cache_efficiency();
        assert!(eff > -20.0 && eff < 0.0); // slightly negative
    }
}
