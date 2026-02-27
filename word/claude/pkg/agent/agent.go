// ============================================================================
// METADATA - CPI-SI Agent Core
// ============================================================================
//
// Key: pkg-agent-core
// Purpose: The agent loop — THINK → ACT → OBSERVE. This is the MIND of CPI-SI.
//          Not an AI wrapper. An agent that uses AI as a tool for reasoning.
// Biblical: Genesis 1:1 — "In the beginning God created the heavens and the earth"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   The agent OWNS the loop. The provider THINKS when asked.
//   The agent DECIDES what to think about, which provider to ask,
//   and whether to accept or reject the response.
//
//   RECEIVE INPUT → THINK (call provider) → ACT (execute tools) → OBSERVE → RESPOND
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"encoding/json"
	"fmt"
	"io"
	"strings"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// RunResult contains the outcome of an agent exchange.
// This is what the REPL uses to track usage and display status.
type RunResult struct {
	// Content is the final text response from the model.
	Content string

	// InputTokens is total input tokens across all iterations.
	InputTokens int

	// OutputTokens is total output tokens across all iterations.
	OutputTokens int

	// ToolsUsed is the count of tool calls executed in this exchange.
	ToolsUsed int

	// Iterations is the number of think→act→observe cycles.
	Iterations int
}

// Agent is the CPI-SI mind — it runs the think/act/observe loop.
type Agent struct {
	provider   Provider
	tools      []Tool
	toolMap    map[string]Tool
	system     string
	history    []Message
	verbose    bool
	index      *PromptIndex
	memory     *Memory
	events     *EventBus
	guardrails *Guardrails
	exchanges  int // total exchanges in this session
}

// Config holds agent configuration.
type Config struct {
	Provider   Provider
	Tools      []Tool
	System     string
	Verbose    bool
	Memory     *Memory     // optional persistent memory
	Guardrails *Guardrails // optional safety checks
}

// New creates a CPI-SI agent with the given configuration.
func New(cfg Config) *Agent {
	toolMap := make(map[string]Tool, len(cfg.Tools))
	for _, t := range cfg.Tools {
		toolMap[t.Definition().Name] = t
	}

	idx := NewPromptIndex()

	a := &Agent{
		provider:   cfg.Provider,
		tools:      cfg.Tools,
		toolMap:    toolMap,
		system:     cfg.System,
		verbose:    cfg.Verbose,
		index:      idx,
		memory:     cfg.Memory,
		events:     NewEventBus(),
		guardrails: cfg.Guardrails,
	}

	// Index the system prompt if provided
	if cfg.System != "" {
		idx.LoadIdentitySegment(cfg.System)
	}

	// Index tool definitions
	if len(cfg.Tools) > 0 {
		var toolDesc strings.Builder
		for _, t := range cfg.Tools {
			def := t.Definition()
			toolDesc.WriteString(def.Name + ": " + def.Description + "\n")
		}
		idx.LoadToolsSegment(toolDesc.String())
	}

	return a
}

// Run processes a single user input through the agent loop.
// Returns a RunResult with the response and usage statistics.
func (a *Agent) Run(ctx context.Context, input string) (*RunResult, error) {
	// Guard: check input
	if a.guardrails != nil {
		gd := &GuardData{Content: input, History: a.history}
		verdict := a.guardrails.Evaluate(ctx, GuardOnInput, gd)
		if verdict.Action == ActionBlock {
			return nil, fmt.Errorf("input blocked: %s", verdict.Reason)
		}
		input = gd.Content // picks up any modifications
	}

	// Add user message to history
	a.history = append(a.history, Message{
		Role:    RoleUser,
		Content: input,
	})

	// Build tool definitions for the provider
	toolDefs := make([]ToolDef, len(a.tools))
	for i, t := range a.tools {
		toolDefs[i] = t.Definition()
	}

	result := &RunResult{}

	// Agent loop: think → act → observe → repeat until done
	for iterations := 0; iterations < 20; iterations++ {
		result.Iterations = iterations + 1

		// THINK: Ask the provider to reason
		req := ChatRequest{
			Messages:  a.history,
			Tools:     toolDefs,
			System:    a.system,
			MaxTokens: 4096,
		}

		a.events.Emit(Event{
			Kind:         EventPreThink,
			Iteration:    result.Iterations,
			Exchange:     a.exchanges + 1,
			MessageCount: len(req.Messages),
		})

		thinkStart := time.Now()
		resp, err := a.provider.Chat(ctx, req)
		thinkElapsed := time.Since(thinkStart)

		if err != nil {
			a.events.Emit(Event{
				Kind:      EventError,
				Iteration: result.Iterations,
				Exchange:  a.exchanges + 1,
				Error:     err,
			})
			return nil, fmt.Errorf("provider error: %w", err)
		}

		a.events.Emit(Event{
			Kind:         EventPostThink,
			Iteration:    result.Iterations,
			Exchange:     a.exchanges + 1,
			MessageCount: len(req.Messages),
			Response:     resp,
			Elapsed:      thinkElapsed,
		})

		// Track usage across iterations
		result.InputTokens += resp.InputTokens
		result.OutputTokens += resp.OutputTokens

		// If no tool calls — we have the final response
		if len(resp.ToolCalls) == 0 {
			content := resp.Content

			// Guard: check output
			if a.guardrails != nil {
				gd := &GuardData{Content: content, History: a.history}
				verdict := a.guardrails.Evaluate(ctx, GuardOnOutput, gd)
				if verdict.Action == ActionBlock {
					content = "[output blocked: " + verdict.Reason + "]"
				} else {
					content = gd.Content // picks up any modifications
				}
			}

			a.history = append(a.history, Message{
				Role:    RoleAssistant,
				Content: content,
			})
			result.Content = content
			a.exchanges++
			a.indexHistory()
			a.events.Emit(Event{
				Kind:     EventComplete,
				Exchange: a.exchanges,
				Result:   result,
			})
			return result, nil
		}

		// Record the assistant message with tool calls
		a.history = append(a.history, Message{
			Role:      RoleAssistant,
			Content:   resp.Content,
			ToolCalls: resp.ToolCalls,
		})

		// ACT: Execute each tool call
		for _, tc := range resp.ToolCalls {
			result.ToolsUsed++

			tool, ok := a.toolMap[tc.Name]
			if !ok {
				// Unknown tool — tell the model
				a.history = append(a.history, Message{
					Role:       RoleTool,
					Content:    fmt.Sprintf("Error: unknown tool %q", tc.Name),
					ToolCallID: tc.ID,
				})
				continue
			}

			// Guard: check tool call
			if a.guardrails != nil {
				verdict := a.guardrails.Evaluate(ctx, GuardOnToolCall, &GuardData{
					ToolName: tc.Name,
					ToolArgs: tc.Arguments,
					History:  a.history,
				})
				if verdict.Action == ActionBlock {
					a.history = append(a.history, Message{
						Role:       RoleTool,
						Content:    fmt.Sprintf("Error: tool call blocked — %s", verdict.Reason),
						ToolCallID: tc.ID,
					})
					continue
				}
			}

			if a.verbose {
				fmt.Printf("  [tool] %s(%s)\n", tc.Name, truncate(tc.Arguments, 80))
			}

			a.events.Emit(Event{
				Kind:      EventPreTool,
				Iteration: result.Iterations,
				Exchange:  a.exchanges + 1,
				ToolName:  tc.Name,
				ToolArgs:  tc.Arguments,
			})

			// Execute the tool
			toolStart := time.Now()
			execResult, execErr := tool.Execute(ctx, tc.Arguments)
			toolElapsed := time.Since(toolStart)
			if execErr != nil {
				execResult = fmt.Sprintf("Error: %v", execErr)
			}

			// Guard: check tool result
			if a.guardrails != nil {
				gd := &GuardData{Content: execResult, ToolName: tc.Name, History: a.history}
				a.guardrails.Evaluate(ctx, GuardOnToolResult, gd)
				execResult = gd.Content // picks up any modifications
			}

			a.events.Emit(Event{
				Kind:        EventPostTool,
				Iteration:   result.Iterations,
				Exchange:    a.exchanges + 1,
				ToolName:    tc.Name,
				ToolArgs:    tc.Arguments,
				ToolResult:  execResult,
				ToolError:   execErr,
				ToolElapsed: toolElapsed,
			})

			// OBSERVE: Feed result back to the model
			a.history = append(a.history, Message{
				Role:       RoleTool,
				Content:    execResult,
				ToolCallID: tc.ID,
			})
		}

		// Loop continues — model will see tool results and decide next action
	}

	return nil, fmt.Errorf("agent loop exceeded maximum iterations")
}

// Reset clears conversation history, keeping the system prompt and tools.
func (a *Agent) Reset() {
	a.history = nil
	// Remove history segments from index but keep identity and tools
	for _, key := range a.index.Loaded() {
		seg := a.index.Get(key)
		if seg != nil && seg.Kind == SegmentHistory {
			a.index.Unload(key)
		}
	}
}

// History returns the current conversation messages.
func (a *Agent) History() []Message {
	return a.history
}

// SetSystem updates the system prompt.
func (a *Agent) SetSystem(system string) {
	a.system = system
	a.index.LoadIdentitySegment(system)
}

// Index returns the prompt index for inspection.
func (a *Agent) Index() *PromptIndex {
	return a.index
}

// RunStream processes user input with streaming output if the provider supports it.
// Writes text chunks to w as they arrive. Returns a RunResult with usage stats.
func (a *Agent) RunStream(ctx context.Context, input string, w io.Writer) (*RunResult, error) {
	sp, ok := a.provider.(StreamProvider)
	if !ok {
		// Fall back to non-streaming
		return a.Run(ctx, input)
	}

	// Add user message
	a.history = append(a.history, Message{
		Role:    RoleUser,
		Content: input,
	})

	toolDefs := make([]ToolDef, len(a.tools))
	for i, t := range a.tools {
		toolDefs[i] = t.Definition()
	}

	result := &RunResult{}

	for iterations := 0; iterations < 20; iterations++ {
		result.Iterations = iterations + 1

		req := ChatRequest{
			Messages:  a.history,
			Tools:     toolDefs,
			System:    a.system,
			MaxTokens: 4096,
		}

		a.events.Emit(Event{
			Kind:         EventPreThink,
			Iteration:    result.Iterations,
			Exchange:     a.exchanges + 1,
			MessageCount: len(req.Messages),
		})

		thinkStart := time.Now()
		ch, err := sp.ChatStream(ctx, req)
		if err != nil {
			a.events.Emit(Event{
				Kind:      EventError,
				Iteration: result.Iterations,
				Exchange:  a.exchanges + 1,
				Error:     err,
			})
			return nil, fmt.Errorf("provider stream error: %w", err)
		}

		// Collect the full response from the stream
		var fullText strings.Builder
		var toolCalls []ToolCall

		for chunk := range ch {
			if chunk.Text != "" {
				fullText.WriteString(chunk.Text)
				// Write to output in real-time
				if w != nil {
					fmt.Fprint(w, chunk.Text)
				}
			}
			if len(chunk.ToolCalls) > 0 {
				toolCalls = chunk.ToolCalls
			}
			// Capture usage from final chunk
			if chunk.Done {
				result.InputTokens += chunk.InputTokens
				result.OutputTokens += chunk.OutputTokens
			}
		}

		thinkElapsed := time.Since(thinkStart)

		// Build a synthetic ChatResponse for the event
		streamResp := &ChatResponse{
			Content:      fullText.String(),
			ToolCalls:    toolCalls,
			InputTokens:  result.InputTokens,
			OutputTokens: result.OutputTokens,
		}

		a.events.Emit(Event{
			Kind:         EventPostThink,
			Iteration:    result.Iterations,
			Exchange:     a.exchanges + 1,
			MessageCount: len(req.Messages),
			Response:     streamResp,
			Elapsed:      thinkElapsed,
		})

		// If no tool calls — done
		if len(toolCalls) == 0 {
			content := fullText.String()
			a.history = append(a.history, Message{
				Role:    RoleAssistant,
				Content: content,
			})
			result.Content = content
			a.exchanges++
			a.indexHistory()
			a.events.Emit(Event{
				Kind:     EventComplete,
				Exchange: a.exchanges,
				Result:   result,
			})
			return result, nil
		}

		// Record assistant message with tool calls
		a.history = append(a.history, Message{
			Role:      RoleAssistant,
			Content:   fullText.String(),
			ToolCalls: toolCalls,
		})

		// Execute tools
		for _, tc := range toolCalls {
			result.ToolsUsed++

			tool, ok := a.toolMap[tc.Name]
			if !ok {
				a.history = append(a.history, Message{
					Role:       RoleTool,
					Content:    fmt.Sprintf("Error: unknown tool %q", tc.Name),
					ToolCallID: tc.ID,
				})
				continue
			}

			if a.verbose {
				fmt.Fprintf(w, "\n  [tool] %s(%s)\n", tc.Name, truncate(tc.Arguments, 80))
			}

			a.events.Emit(Event{
				Kind:      EventPreTool,
				Iteration: result.Iterations,
				Exchange:  a.exchanges + 1,
				ToolName:  tc.Name,
				ToolArgs:  tc.Arguments,
			})

			toolStart := time.Now()
			execResult, execErr := tool.Execute(ctx, tc.Arguments)
			toolElapsed := time.Since(toolStart)
			if execErr != nil {
				execResult = fmt.Sprintf("Error: %v", execErr)
			}

			a.events.Emit(Event{
				Kind:        EventPostTool,
				Iteration:   result.Iterations,
				Exchange:    a.exchanges + 1,
				ToolName:    tc.Name,
				ToolArgs:    tc.Arguments,
				ToolResult:  execResult,
				ToolError:   execErr,
				ToolElapsed: toolElapsed,
			})

			a.history = append(a.history, Message{
				Role:       RoleTool,
				Content:    execResult,
				ToolCallID: tc.ID,
			})
		}

		// After tools execute, next iteration sends results back
		if w != nil {
			fmt.Fprintln(w)
		}
	}

	return nil, fmt.Errorf("agent loop exceeded maximum iterations")
}

// --- Identity Loading ---

// LoadIdentity reads a file and sets it as the system prompt.
func (a *Agent) LoadIdentity(path string) error {
	tool := &fileReadTool{}
	args, _ := json.Marshal(map[string]string{"path": path})
	content, err := tool.Execute(context.Background(), string(args))
	if err != nil {
		return fmt.Errorf("load identity from %s: %w", path, err)
	}
	a.system = content
	a.index.LoadIdentitySegment(content)
	return nil
}

// --- Context Loading ---

// LoadContext loads a file's contents as a context segment in the index.
// The file remains available to the model as part of the prompt context.
func (a *Agent) LoadContext(path string) error {
	tool := &fileReadTool{}
	args, _ := json.Marshal(map[string]string{"path": path})
	content, err := tool.Execute(context.Background(), string(args))
	if err != nil {
		return fmt.Errorf("load context from %s: %w", path, err)
	}
	key := "context:" + path
	a.index.LoadContextSegment(key, fmt.Sprintf("--- File: %s ---\n%s", path, content))
	return nil
}

// UnloadContext removes a previously loaded context segment.
func (a *Agent) UnloadContext(path string) {
	key := "context:" + path
	a.index.Unload(key)
}

// ContextSegments returns the keys of all loaded context segments.
func (a *Agent) ContextSegments() []string {
	var contextKeys []string
	for _, key := range a.index.Loaded() {
		seg := a.index.Get(key)
		if seg != nil && seg.Kind == SegmentContext {
			contextKeys = append(contextKeys, key)
		}
	}
	return contextKeys
}

// Exchanges returns the total number of user→assistant exchanges in this session.
func (a *Agent) Exchanges() int {
	return a.exchanges
}

// Memory returns the agent's persistent memory store, or nil if not configured.
func (a *Agent) Memory() *Memory {
	return a.memory
}

// Events returns the agent's event bus for subscribing to loop events.
func (a *Agent) Events() *EventBus {
	return a.events
}

// Guardrails returns the agent's guardrails, or nil if not configured.
func (a *Agent) Guardrails() *Guardrails {
	return a.guardrails
}

// SetGuardrails sets or replaces the agent's guardrails.
func (a *Agent) SetGuardrails(g *Guardrails) {
	a.guardrails = g
}

// On registers a handler for a specific event kind. Convenience for a.Events().On().
func (a *Agent) On(kind EventKind, handler EventHandler) int {
	return a.events.On(kind, handler)
}

// OnAll registers a handler for all events. Convenience for a.Events().OnAll().
func (a *Agent) OnAll(handler EventHandler) int {
	return a.events.OnAll(handler)
}

// --- History Indexing ---

// indexHistory updates the prompt index with the current conversation state.
// This keeps the index's view of "what's loaded" accurate.
// Note: does NOT increment exchange count — that's done in Run/RunStream.
func (a *Agent) indexHistory() {
	if len(a.history) == 0 {
		return
	}

	// Build a summary of the history for the index.
	// We segment into "recent" (last 10 messages) and "older" (everything else).
	const recentWindow = 10

	if len(a.history) <= recentWindow {
		// All history fits in "recent"
		var sb strings.Builder
		for _, m := range a.history {
			sb.WriteString(string(m.Role) + ": " + truncate(m.Content, 200) + "\n")
		}
		a.index.LoadHistorySegment("history:recent", sb.String(), true)
		// Remove older segment if it existed from a previous state
		a.index.Unload("history:older")
	} else {
		// Split into older + recent
		olderMsgs := a.history[:len(a.history)-recentWindow]
		recentMsgs := a.history[len(a.history)-recentWindow:]

		var olderSB strings.Builder
		for _, m := range olderMsgs {
			olderSB.WriteString(string(m.Role) + ": " + truncate(m.Content, 100) + "\n")
		}
		a.index.LoadHistorySegment("history:older", olderSB.String(), false)

		var recentSB strings.Builder
		for _, m := range recentMsgs {
			recentSB.WriteString(string(m.Role) + ": " + truncate(m.Content, 200) + "\n")
		}
		a.index.LoadHistorySegment("history:recent", recentSB.String(), true)
	}
}

// --- Helpers ---

func truncate(s string, max int) string {
	s = strings.ReplaceAll(s, "\n", " ")
	if len(s) <= max {
		return s
	}
	return s[:max] + "..."
}

// ============================================================================
// CLOSING
// ============================================================================
// "In the beginning God created the heavens and the earth." — Genesis 1:1
// The agent creates. The provider reasons. The tools act.
