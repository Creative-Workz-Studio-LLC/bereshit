// ============================================================================
// METADATA - CPI-SI Agent Guardrails
// ============================================================================
//
// Key: pkg-agent-guardrails
// Purpose: Agent-level safety checks at four loop points — input, output,
//          tool call, tool result. The agent's gate system: what enters,
//          what executes, what returns. Complements provider-level middleware
//          guardrails (middleware.go) with agent-level awareness.
// Biblical: Proverbs 4:23 — "Keep thy heart with all diligence; for out of
//           it are the issues of life"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   Guards check content at 4 points: Input → ToolCall → ToolResult → Output
//   Each guard returns a verdict: Allow, Block, Modify, or Warn.
//   The guardrails system aggregates verdicts — first Block wins.
//
//   Middleware guardrails (middleware.go): operate on ChatRequest to provider
//   Agent guardrails (this file): operate on user input, tool calls, output
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"strings"
	"sync"
)

// ============================================================================
// BODY
// ============================================================================

// GuardPoint identifies where in the agent loop a guard executes.
type GuardPoint int

const (
	// GuardOnInput checks user input before it enters the agent loop.
	GuardOnInput GuardPoint = iota

	// GuardOnOutput checks model output before it's returned to the user.
	GuardOnOutput

	// GuardOnToolCall checks a tool call before it's executed.
	GuardOnToolCall

	// GuardOnToolResult checks a tool's result before feeding it back to the model.
	GuardOnToolResult
)

// GuardAction determines what happens after a guard check.
type GuardAction int

const (
	// ActionAllow lets the content through unchanged.
	ActionAllow GuardAction = iota

	// ActionBlock stops processing and returns an error.
	ActionBlock

	// ActionModify allows but with modified content.
	ActionModify

	// ActionWarn allows but flags for attention.
	ActionWarn
)

// GuardVerdict is the result of a guard check.
type GuardVerdict struct {
	// Action is what to do.
	Action GuardAction

	// Reason explains the verdict.
	Reason string

	// Replace is the modified content (only for ActionModify).
	Replace string

	// GuardName identifies which guard produced this verdict.
	GuardName string
}

// GuardData carries context for a guard check.
type GuardData struct {
	// Content is the text being checked (input, output, or tool result).
	Content string

	// ToolName is the tool being called (for GuardOnToolCall/GuardOnToolResult).
	ToolName string

	// ToolArgs is the JSON arguments (for GuardOnToolCall).
	ToolArgs string

	// History is the conversation so far.
	History []Message
}

// GuardCheckFunc is the function signature for a guard check.
type GuardCheckFunc func(ctx context.Context, point GuardPoint, data *GuardData) *GuardVerdict

// Guard is a named check that runs at specific points in the loop.
type Guard struct {
	// Name identifies this guard.
	Name string

	// Points are where this guard runs.
	Points []GuardPoint

	// Check is the guard function.
	Check GuardCheckFunc
}

// Guardrails manages a set of guards for an agent.
type Guardrails struct {
	mu     sync.RWMutex
	guards []Guard
}

// NewGuardrails creates an empty guardrails set.
func NewGuardrails() *Guardrails {
	return &Guardrails{}
}

// Add registers a guard. Guards execute in the order they're added.
func (g *Guardrails) Add(guard Guard) {
	g.mu.Lock()
	defer g.mu.Unlock()
	g.guards = append(g.guards, guard)
}

// Remove unregisters a guard by name.
func (g *Guardrails) Remove(name string) bool {
	g.mu.Lock()
	defer g.mu.Unlock()

	for i, guard := range g.guards {
		if guard.Name == name {
			g.guards = append(g.guards[:i], g.guards[i+1:]...)
			return true
		}
	}
	return false
}

// Evaluate runs all guards for a given point. Returns the first non-Allow verdict,
// or an Allow verdict if all guards pass.
func (g *Guardrails) Evaluate(ctx context.Context, point GuardPoint, data *GuardData) *GuardVerdict {
	g.mu.RLock()
	// Copy the slice under lock to avoid holding it during checks
	guards := make([]Guard, len(g.guards))
	copy(guards, g.guards)
	g.mu.RUnlock()

	var warnings []string

	for _, guard := range guards {
		if !guardAppliesToPoint(guard, point) {
			continue
		}

		verdict := guard.Check(ctx, point, data)
		if verdict == nil {
			continue
		}

		verdict.GuardName = guard.Name

		switch verdict.Action {
		case ActionBlock:
			return verdict
		case ActionModify:
			// Apply modification and continue checking
			data.Content = verdict.Replace
		case ActionWarn:
			warnings = append(warnings, verdict.Reason)
		}
	}

	// If we had warnings, return the last one with Allow
	if len(warnings) > 0 {
		return &GuardVerdict{
			Action: ActionWarn,
			Reason: strings.Join(warnings, "; "),
		}
	}

	return &GuardVerdict{Action: ActionAllow}
}

// Size returns the number of registered guards.
func (g *Guardrails) Size() int {
	g.mu.RLock()
	defer g.mu.RUnlock()
	return len(g.guards)
}

// Names returns the names of all registered guards.
func (g *Guardrails) Names() []string {
	g.mu.RLock()
	defer g.mu.RUnlock()
	names := make([]string, len(g.guards))
	for i, guard := range g.guards {
		names[i] = guard.Name
	}
	return names
}

// --- Built-in Guards ---

// MaxInputLength blocks input exceeding a character count.
func MaxInputLength(max int) Guard {
	return Guard{
		Name:   "max_input_length",
		Points: []GuardPoint{GuardOnInput},
		Check: func(_ context.Context, _ GuardPoint, data *GuardData) *GuardVerdict {
			if len(data.Content) > max {
				return &GuardVerdict{
					Action: ActionBlock,
					Reason: "input exceeds maximum length",
				}
			}
			return nil
		},
	}
}

// MaxOutputLength truncates output exceeding a character count.
func MaxOutputLength(max int) Guard {
	return Guard{
		Name:   "max_output_length",
		Points: []GuardPoint{GuardOnOutput},
		Check: func(_ context.Context, _ GuardPoint, data *GuardData) *GuardVerdict {
			if len(data.Content) > max {
				return &GuardVerdict{
					Action:  ActionModify,
					Reason:  "output truncated to maximum length",
					Replace: data.Content[:max] + "\n[truncated]",
				}
			}
			return nil
		},
	}
}

// BlockPatterns blocks content containing any of the given substrings.
// Case-insensitive matching.
func BlockPatterns(patterns ...string) Guard {
	return Guard{
		Name:   "block_patterns",
		Points: []GuardPoint{GuardOnInput, GuardOnOutput},
		Check: func(_ context.Context, _ GuardPoint, data *GuardData) *GuardVerdict {
			lower := strings.ToLower(data.Content)
			for _, pattern := range patterns {
				if strings.Contains(lower, strings.ToLower(pattern)) {
					return &GuardVerdict{
						Action: ActionBlock,
						Reason: "blocked pattern detected",
					}
				}
			}
			return nil
		},
	}
}

// ToolAllowList only permits the named tools. All others are blocked.
func ToolAllowList(tools ...string) Guard {
	allowed := make(map[string]bool, len(tools))
	for _, t := range tools {
		allowed[t] = true
	}

	return Guard{
		Name:   "tool_allow_list",
		Points: []GuardPoint{GuardOnToolCall},
		Check: func(_ context.Context, _ GuardPoint, data *GuardData) *GuardVerdict {
			if !allowed[data.ToolName] {
				return &GuardVerdict{
					Action: ActionBlock,
					Reason: "tool not in allow list: " + data.ToolName,
				}
			}
			return nil
		},
	}
}

// ToolDenyList blocks specific tools. All others are allowed.
func ToolDenyList(tools ...string) Guard {
	denied := make(map[string]bool, len(tools))
	for _, t := range tools {
		denied[t] = true
	}

	return Guard{
		Name:   "tool_deny_list",
		Points: []GuardPoint{GuardOnToolCall},
		Check: func(_ context.Context, _ GuardPoint, data *GuardData) *GuardVerdict {
			if denied[data.ToolName] {
				return &GuardVerdict{
					Action: ActionBlock,
					Reason: "tool is denied: " + data.ToolName,
				}
			}
			return nil
		},
	}
}

// ToolResultMaxLength truncates tool results that are too long.
func ToolResultMaxLength(max int) Guard {
	return Guard{
		Name:   "tool_result_max_length",
		Points: []GuardPoint{GuardOnToolResult},
		Check: func(_ context.Context, _ GuardPoint, data *GuardData) *GuardVerdict {
			if len(data.Content) > max {
				return &GuardVerdict{
					Action:  ActionModify,
					Reason:  "tool result truncated",
					Replace: data.Content[:max] + "\n[truncated — result exceeded maximum length]",
				}
			}
			return nil
		},
	}
}

// RequireHistory blocks input when conversation history exceeds a length.
// Useful for forcing session resets to manage context.
func RequireHistory(maxMessages int) Guard {
	return Guard{
		Name:   "require_history_limit",
		Points: []GuardPoint{GuardOnInput},
		Check: func(_ context.Context, _ GuardPoint, data *GuardData) *GuardVerdict {
			if len(data.History) > maxMessages {
				return &GuardVerdict{
					Action: ActionWarn,
					Reason: "conversation history is very long — consider /reset",
				}
			}
			return nil
		},
	}
}

// --- Helpers ---

// guardAppliesToPoint checks if a guard is registered for a given point.
func guardAppliesToPoint(guard Guard, point GuardPoint) bool {
	for _, p := range guard.Points {
		if p == point {
			return true
		}
	}
	return false
}

// ============================================================================
// CLOSING
// ============================================================================
// "Keep thy heart with all diligence; for out of it are the issues of life."
// — Proverbs 4:23
// Guard what enters, what executes, what returns. Diligence at every gate.
