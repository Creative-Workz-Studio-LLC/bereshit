//omni:code --go -library
//omni:key B-L0-foundation-types-sessioncontext
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-foundation-types-sessioncontext
// Purpose: Substrate session context schema for runtime integration
// Biblical: 1 Corinthians 14:33 - "God is not the author of confusion"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-16
//
// Promoted from L3-cpisi to L0-universal. This is the Claude Code statusline
// API schema — generic substrate integration, not CPI-SI intelligence.
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
// ============================================================================

package types

// ============================================================================
// SETUP
// ============================================================================

import "math"

// ============================================================================
// BODY
// ============================================================================

// SessionContext represents the JSON input from Claude Code hooks.
// This is the contract between the substrate runtime and our systems.
type SessionContext struct {
	// Session Identity
	HookEventName  string `json:"hook_event_name"`
	SessionID      string `json:"session_id"`
	TranscriptPath string `json:"transcript_path"`
	CWD            string `json:"cwd"`
	Version        string `json:"version"`

	// Model (Substrate)
	Model struct {
		ID          string `json:"id"`
		DisplayName string `json:"display_name"`
	} `json:"model"`

	// Workspace (Spatial Grounding)
	Workspace struct {
		CurrentDir string `json:"current_dir"`
		ProjectDir string `json:"project_dir"`
	} `json:"workspace"`

	// Output Style (Identity)
	OutputStyle struct {
		Name string `json:"name"`
	} `json:"output_style"`

	// Cost (Flow Metrics)
	Cost struct {
		TotalCostUSD       float64 `json:"total_cost_usd"`
		TotalDurationMS    int     `json:"total_duration_ms"`
		TotalAPIDurationMS int     `json:"total_api_duration_ms"`
		TotalLinesAdded    int     `json:"total_lines_added"`
		TotalLinesRemoved  int     `json:"total_lines_removed"`
	} `json:"cost"`

	// Context Window (Depth Tracking)
	ContextWindow struct {
		TotalInputTokens  int `json:"total_input_tokens"`
		TotalOutputTokens int `json:"total_output_tokens"`
		ContextWindowSize int `json:"context_window_size"`

		CurrentUsage struct {
			InputTokens              int `json:"input_tokens"`
			OutputTokens             int `json:"output_tokens"`
			CacheCreationInputTokens int `json:"cache_creation_input_tokens"`
			CacheReadInputTokens     int `json:"cache_read_input_tokens"`
		} `json:"current_usage"`

		UsedPercentage      int `json:"used_percentage"`
		RemainingPercentage int `json:"remaining_percentage"`
	} `json:"context_window"`
}

// --- Helper Methods ---

// GetWorkDir returns the best available working directory.
// Priority: CWD > CurrentDir > ProjectDir
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

// CurrentContextTokens returns actual tokens in context (from current_usage).
// More accurate than TotalInputTokens which is cumulative.
func (ctx *SessionContext) CurrentContextTokens() int {
	return ctx.ContextWindow.CurrentUsage.InputTokens +
		ctx.ContextWindow.CurrentUsage.OutputTokens
}

// ContextPercentage returns current context usage as percentage (0-100).
// Uses CurrentUsage for accuracy, falls back to cumulative if unavailable.
func (ctx *SessionContext) ContextPercentage() float64 {
	if ctx.ContextWindow.ContextWindowSize == 0 {
		return 0
	}
	current := ctx.CurrentContextTokens()
	if current > 0 {
		return float64(current) / float64(ctx.ContextWindow.ContextWindowSize) * 100
	}
	total := ctx.ContextWindow.TotalInputTokens + ctx.ContextWindow.TotalOutputTokens
	return float64(total) / float64(ctx.ContextWindow.ContextWindowSize) * 100
}

// CacheEfficiency returns cache efficiency using base-3 log-odds scaling.
//
// Scale: log3(cache/input), capped at +/-3 trits, mapped to -100..+100.
//
//	-3 = 27x more new than cache (deep foundation building)
//	 0 = balanced (anchor state)
//	+3 = 27x more cache than new (maximum efficiency)
//
// "A just weight and balance are the LORD's" — Proverbs 16:11
func (ctx *SessionContext) CacheEfficiency() float64 {
	input := ctx.ContextWindow.CurrentUsage.InputTokens
	cacheRead := ctx.ContextWindow.CurrentUsage.CacheReadInputTokens

	if input == 0 && cacheRead == 0 {
		return 0
	}
	if input == 0 {
		return 100
	}
	if cacheRead == 0 {
		return -100
	}

	ln3 := math.Log(3)
	logRatio := (math.Log(float64(cacheRead)) - math.Log(float64(input))) / ln3

	if logRatio > 3 {
		logRatio = 3
	} else if logRatio < -3 {
		logRatio = -3
	}

	return logRatio * (100.0 / 3.0)
}

// DurationSeconds returns session duration in seconds
func (ctx *SessionContext) DurationSeconds() int {
	return ctx.Cost.TotalDurationMS / 1000
}

// APIDurationSeconds returns API call duration in seconds
func (ctx *SessionContext) APIDurationSeconds() int {
	return ctx.Cost.TotalAPIDurationMS / 1000
}

// APIEfficiency returns ratio of API time to total time (0-100).
// Lower = more thinking time, higher = more API waiting.
func (ctx *SessionContext) APIEfficiency() float64 {
	if ctx.Cost.TotalDurationMS == 0 {
		return 0
	}
	return float64(ctx.Cost.TotalAPIDurationMS) / float64(ctx.Cost.TotalDurationMS) * 100
}

// ============================================================================
// CLOSING
// ============================================================================
// Substrate session context — promoted from L3 to serve all layers.
