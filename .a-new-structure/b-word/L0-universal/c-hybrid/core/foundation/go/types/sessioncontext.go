//omni:code --go -library
//omni:key B-L0-foundation-types-sessioncontext
//omni:version a-02.00

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
// Updated: 2026-02-18
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
// END METADATA
// ============================================================================

package types

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"math"
)

// --- Core Types ---

// Model represents the substrate model identity.
type Model struct {
	ID          string `json:"id"`
	DisplayName string `json:"display_name"`
}

// Workspace represents spatial grounding — where work happens.
type Workspace struct {
	CurrentDir string `json:"current_dir"`
	ProjectDir string `json:"project_dir"`
}

// OutputStyle represents the identity voice configuration.
type OutputStyle struct {
	Name string `json:"name"`
}

// Cost tracks flow metrics — time, money, lines changed.
type Cost struct {
	TotalCostUSD       float64 `json:"total_cost_usd"`
	TotalDurationMS    int     `json:"total_duration_ms"`
	TotalAPIDurationMS int     `json:"total_api_duration_ms"`
	TotalLinesAdded    int     `json:"total_lines_added"`
	TotalLinesRemoved  int     `json:"total_lines_removed"`
}

// CurrentUsage tracks current API call token usage.
type CurrentUsage struct {
	InputTokens              int `json:"input_tokens"`
	OutputTokens             int `json:"output_tokens"`
	CacheCreationInputTokens int `json:"cache_creation_input_tokens"`
	CacheReadInputTokens     int `json:"cache_read_input_tokens"`
}

// ContextWindow tracks depth — how much context is consumed.
type ContextWindow struct {
	TotalInputTokens    int          `json:"total_input_tokens"`
	TotalOutputTokens   int          `json:"total_output_tokens"`
	ContextWindowSize   int          `json:"context_window_size"`
	CurrentUsage        CurrentUsage `json:"current_usage"`
	UsedPercentage      int          `json:"used_percentage"`
	RemainingPercentage int          `json:"remaining_percentage"`
}

// SessionContext represents the JSON input from Claude Code hooks.
// This is the contract between the substrate runtime and our systems.
type SessionContext struct {
	HookEventName  string        `json:"hook_event_name"`
	SessionID      string        `json:"session_id"`
	TranscriptPath string        `json:"transcript_path"`
	CWD            string        `json:"cwd"`
	Version        string        `json:"version"`
	Model          Model         `json:"model"`
	Workspace      Workspace     `json:"workspace"`
	OutputStyle    OutputStyle   `json:"output_style"`
	Cost           Cost          `json:"cost"`
	ContextWindow  ContextWindow `json:"context_window"`
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Queries (SessionContext):
//   GetWorkDir, TotalLinesChanged, CurrentContextTokens, ContextPercentage,
//   CacheEfficiency, DurationSeconds, APIDurationSeconds, APIEfficiency, String
//
// Flow: JSON unmarshal → SessionContext → query methods → formatted output

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: No internal helpers — uses only math.Log from stdlib]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────

// --- Workspace Queries ---

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

// --- Cost Queries ---

// TotalLinesChanged returns sum of added and removed lines.
func (ctx *SessionContext) TotalLinesChanged() int {
	return ctx.Cost.TotalLinesAdded + ctx.Cost.TotalLinesRemoved
}

// DurationSeconds returns session duration in seconds.
func (ctx *SessionContext) DurationSeconds() int {
	return ctx.Cost.TotalDurationMS / 1000
}

// APIDurationSeconds returns API call duration in seconds.
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

// --- Context Window Queries ---

// CurrentContextTokens returns actual tokens in context (from current_usage).
// More accurate than TotalInputTokens which is cumulative.
func (ctx *SessionContext) CurrentContextTokens() int {
	return ctx.ContextWindow.CurrentUsage.InputTokens +
		ctx.ContextWindow.CurrentUsage.OutputTokens
}

// ContextPercentage returns current context usage as percentage (0-100).
//
// Uses CurrentUsage for accuracy. Falls back to cumulative totals
// when CurrentUsage is unavailable (zero).
func (ctx *SessionContext) ContextPercentage() float64 {
	if ctx.ContextWindow.ContextWindowSize == 0 {
		return 0
	}

	// Prefer current usage (more accurate)
	tokens := ctx.CurrentContextTokens()
	if tokens == 0 {
		// Fallback to cumulative
		tokens = ctx.ContextWindow.TotalInputTokens + ctx.ContextWindow.TotalOutputTokens
	}

	return float64(tokens) / float64(ctx.ContextWindow.ContextWindowSize) * 100
}

// CacheEfficiency returns cache efficiency using base-3 log-odds scaling.
//
// Scale: log3(cache/input), capped at +/-3 trits, mapped to -100..+100.
//
//	Scale: -100 (all new input) → ±0 (balanced) → +100 (all cache)
//
// Edge cases:
//   - Both zero: 0 (balanced — no data)
//   - Input zero, cache > 0: +100 (all cache)
//   - Cache zero, input > 0: -100 (all new)
//   - Equal: 0 (balanced)
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

	// Clamp to [-3, +3] trits
	if logRatio > 3 {
		logRatio = 3
	} else if logRatio < -3 {
		logRatio = -3
	}

	return logRatio * (100.0 / 3.0)
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Types package — query methods return values, not errors]

// ──────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// --- String Methods ---

// String returns the model display name and ID.
//
//	"Claude Opus 4.6 (claude-opus-4-6)"
func (m Model) String() string {
	if m.DisplayName == "" && m.ID == "" {
		return "(unknown model)"
	}
	if m.DisplayName == "" {
		return m.ID
	}
	return fmt.Sprintf("%s (%s)", m.DisplayName, m.ID)
}

// String returns a human-readable summary of the session context.
//
//	"[session-abc] Claude Opus 4.6 (claude-opus-4-6) — Notification (42.5% context, $0.12)"
func (ctx *SessionContext) String() string {
	pct := ctx.ContextPercentage()
	return fmt.Sprintf("[%s] %s — %s (%.1f%% context, $%.2f)",
		ctx.SessionID,
		ctx.Model.String(),
		ctx.HookEventName,
		pct,
		ctx.Cost.TotalCostUSD,
	)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// Testing: go test -v -run TestSessionContext ./L0-universal/.../types/
//
// Verify:
//   - JSON roundtrip with named types matches original inline layout
//   - ContextPercentage falls back to cumulative when CurrentUsage is zero
//   - CacheEfficiency edge cases: both zero, one zero, equal, extreme ratios
//   - GetWorkDir cascade: CWD > CurrentDir > ProjectDir
//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — imported, not executed.
// import "creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/core/foundation/go/types"
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// No resources to clean up. Pure value types.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Change JSON field names — breaks Claude Code hook contract
//   - Add filesystem I/O — types only
//   - Change CacheEfficiency scaling — breaks health scoring
//
// Careful:
//   - Adding new fields to SessionContext — verify against Claude Code API
//   - Modifying ContextPercentage fallback — affects context warnings
//
// Safe:
//   - Adding new query methods
//   - Adding constructors for sub-types
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Substrate session context promoted from L3 to L0. Named types
// extracted from inline definitions for testability and reuse. JSON layout
// unchanged. Identity vars (Pragma/Metadata) live in workflow.go — Go
// requires unique package-level names across files in the same package.
//
// Scripture: 1 Corinthians 14:33 — "God is not the author of confusion,
// but of peace."
//
// ============================================================================
// END CLOSING
// ============================================================================
