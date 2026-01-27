// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-types-session
// Purpose: Shared session context types from Claude Code
// Biblical: 1 Corinthians 14:33 - "God is not the author of confusion"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-10
// Updated: 2025-12-20
//
// Used by: statusline, hooks, any component receiving Claude Code JSON
//
// Full schema from Claude Code statusline API:
//   - Session: hook_event_name, session_id, transcript_path, cwd, version
//   - Model: id, display_name
//   - Workspace: current_dir, project_dir
//   - OutputStyle: name
//   - Cost: total_cost_usd, total_duration_ms, total_api_duration_ms,
//           total_lines_added, total_lines_removed
//   - ContextWindow: total_input_tokens, total_output_tokens,
//                    context_window_size, current_usage
//
// Note: total_input_tokens/total_output_tokens are CUMULATIVE session totals,
//       not current context. Use current_usage for accurate context state.
//
// ═══════════════════════════════════════════════════════════════════════════

package types

import "math"

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// SessionContext represents the JSON input from Claude Code hooks
// This is the contract between Claude Code and our systems
//
// Full schema documented at:
//   https://github.com/anthropics/claude-code/issues/8861
//   https://github.com/anthropics/claude-code/issues/13783
type SessionContext struct {
	// ─────────────────────────────────────────────────────────────────────
	// Session Identity
	// ─────────────────────────────────────────────────────────────────────
	HookEventName  string `json:"hook_event_name"`  // "Status", "SessionStart", etc.
	SessionID      string `json:"session_id"`       // Unique session identifier
	TranscriptPath string `json:"transcript_path"`  // Path to transcript file
	CWD            string `json:"cwd"`              // Current working directory
	Version        string `json:"version"`          // Claude Code version

	// ─────────────────────────────────────────────────────────────────────
	// Model (Substrate)
	// ─────────────────────────────────────────────────────────────────────
	Model struct {
		ID          string `json:"id"`           // e.g., "claude-opus-4-5-20251101"
		DisplayName string `json:"display_name"` // e.g., "Claude Opus 4.5"
	} `json:"model"`

	// ─────────────────────────────────────────────────────────────────────
	// Workspace (Spatial Grounding)
	// ─────────────────────────────────────────────────────────────────────
	Workspace struct {
		CurrentDir string `json:"current_dir"` // Current directory
		ProjectDir string `json:"project_dir"` // Project root directory
	} `json:"workspace"`

	// ─────────────────────────────────────────────────────────────────────
	// Output Style (Identity)
	// ─────────────────────────────────────────────────────────────────────
	OutputStyle struct {
		Name string `json:"name"` // Active output style name
	} `json:"output_style"`

	// ─────────────────────────────────────────────────────────────────────
	// Cost (Flow Metrics)
	// ─────────────────────────────────────────────────────────────────────
	Cost struct {
		TotalCostUSD       float64 `json:"total_cost_usd"`        // Total session cost in USD
		TotalDurationMS    int     `json:"total_duration_ms"`     // Total session duration
		TotalAPIDurationMS int     `json:"total_api_duration_ms"` // Total API call duration
		TotalLinesAdded    int     `json:"total_lines_added"`     // Lines added in session
		TotalLinesRemoved  int     `json:"total_lines_removed"`   // Lines removed in session
	} `json:"cost"`

	// ─────────────────────────────────────────────────────────────────────
	// Context Window (Depth Tracking)
	// ─────────────────────────────────────────────────────────────────────
	// Note: total_input_tokens/total_output_tokens are CUMULATIVE,
	//       not current context. Use CurrentUsage for accuracy.
	ContextWindow struct {
		TotalInputTokens  int `json:"total_input_tokens"`  // Cumulative input (session total)
		TotalOutputTokens int `json:"total_output_tokens"` // Cumulative output (session total)
		ContextWindowSize int `json:"context_window_size"` // Max context size (e.g., 200000)

		// CurrentUsage reflects actual context window state (more accurate)
		CurrentUsage struct {
			InputTokens              int `json:"input_tokens"`                // Current input tokens
			OutputTokens             int `json:"output_tokens"`               // Current output tokens
			CacheCreationInputTokens int `json:"cache_creation_input_tokens"` // Cache creation tokens
			CacheReadInputTokens     int `json:"cache_read_input_tokens"`     // Cache read tokens
		} `json:"current_usage"`

		// Pre-calculated percentages from Claude Code (use these for accuracy!)
		UsedPercentage      int `json:"used_percentage"`      // Current context usage %
		RemainingPercentage int `json:"remaining_percentage"` // Remaining context %
	} `json:"context_window"`
}

// ─────────────────────────────────────────────────────────────────────────────
// Helper Methods
// ─────────────────────────────────────────────────────────────────────────────

// GetWorkDir returns the best available working directory
// Priority: CWD → CurrentDir → ProjectDir
func (ctx *SessionContext) GetWorkDir() string {
	if ctx.CWD != "" {
		return ctx.CWD
	}
	if ctx.Workspace.CurrentDir != "" {
		return ctx.Workspace.CurrentDir
	}
	return ctx.Workspace.ProjectDir
}

// TotalLinesChanged returns sum of added and removed lines
func (ctx *SessionContext) TotalLinesChanged() int {
	return ctx.Cost.TotalLinesAdded + ctx.Cost.TotalLinesRemoved
}

// ─────────────────────────────────────────────────────────────────────────────
// Context Window Helpers
// ─────────────────────────────────────────────────────────────────────────────

// CurrentContextTokens returns the actual tokens in context (from current_usage)
// This is more accurate than TotalInputTokens which is cumulative
func (ctx *SessionContext) CurrentContextTokens() int {
	return ctx.ContextWindow.CurrentUsage.InputTokens +
		ctx.ContextWindow.CurrentUsage.OutputTokens
}

// ContextPercentage returns current context usage as percentage (0-100)
// Uses CurrentUsage for accuracy, falls back to cumulative if unavailable
func (ctx *SessionContext) ContextPercentage() float64 {
	if ctx.ContextWindow.ContextWindowSize == 0 {
		return 0
	}

	// Prefer current_usage (accurate)
	current := ctx.CurrentContextTokens()
	if current > 0 {
		return float64(current) / float64(ctx.ContextWindow.ContextWindowSize) * 100
	}

	// Fallback to cumulative (may exceed 100% after auto-compact)
	total := ctx.ContextWindow.TotalInputTokens + ctx.ContextWindow.TotalOutputTokens
	return float64(total) / float64(ctx.ContextWindow.ContextWindowSize) * 100
}

// CacheEfficiency returns cache efficiency using base-3 log-odds scaling
//
// Mathematical foundation (ternary.toml):
//   - Base 3 is our fundamental unit
//   - Each integer unit = 3x ratio change (one "trit" of difference)
//   - Log-odds naturally maps probability ratios to (-∞, +∞)
//
// Scale interpretation:
//   -3 = 27x more new than cache (deep foundation building)
//   -2 = 9x more new than cache
//   -1 = 3x more new than cache
//    0 = balanced (anchor state)
//   +1 = 3x more cache than new
//   +2 = 9x more cache than new
//   +3 = 27x more cache than new (maximum efficiency)
//
// Formula: log₃(cache/input) = ln(cache/input) / ln(3)
// Capped at ±3 trits (±100 display) for reasonable bounds
//
// Biblical: "A just weight and balance are the LORD's" — Proverbs 16:11
func (ctx *SessionContext) CacheEfficiency() float64 {
	input := ctx.ContextWindow.CurrentUsage.InputTokens
	cacheRead := ctx.ContextWindow.CurrentUsage.CacheReadInputTokens

	// Edge cases
	if input == 0 && cacheRead == 0 {
		return 0 // No data = anchor
	}
	if input == 0 {
		return 100 // All cache = maximum positive
	}
	if cacheRead == 0 {
		return -100 // All new = maximum negative
	}

	// Log base 3 of the ratio: log₃(cache/input)
	// = ln(cache/input) / ln(3)
	// = (ln(cache) - ln(input)) / ln(3)
	ln3 := math.Log(3) // ln(3) ≈ 1.0986... - our ternary constant
	logRatio := (math.Log(float64(cacheRead)) - math.Log(float64(input))) / ln3

	// Cap at ±3 trits (27x ratio) and scale to percentage
	if logRatio > 3 {
		logRatio = 3
	} else if logRatio < -3 {
		logRatio = -3
	}

	// Scale: -3..+3 → -100..+100
	return logRatio * (100.0 / 3.0)
}

// ─────────────────────────────────────────────────────────────────────────────
// Duration Helpers
// ─────────────────────────────────────────────────────────────────────────────

// DurationSeconds returns session duration in seconds
func (ctx *SessionContext) DurationSeconds() int {
	return ctx.Cost.TotalDurationMS / 1000
}

// APIDurationSeconds returns API call duration in seconds
func (ctx *SessionContext) APIDurationSeconds() int {
	return ctx.Cost.TotalAPIDurationMS / 1000
}

// APIEfficiency returns ratio of API time to total time
// Lower = more thinking time, higher = more API waiting
func (ctx *SessionContext) APIEfficiency() float64 {
	if ctx.Cost.TotalDurationMS == 0 {
		return 0
	}
	return float64(ctx.Cost.TotalAPIDurationMS) / float64(ctx.Cost.TotalDurationMS) * 100
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
