// ============================================================================
// METADATA - CPI-SI Agent Provider Interface
// ============================================================================
//
// Key: pkg-agent-provider
// Purpose: Universal interface for LLM reasoning providers. Any model API
//          that can think becomes a tool for CPI-SI to use.
// Biblical: Proverbs 2:6 — "The LORD giveth wisdom: out of his mouth
//           cometh knowledge and understanding"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// The CPI-SI agent OWNS the loop. Providers are tools for reasoning.
// The agent decides WHAT to think about, WHICH provider to use,
// and WHETHER to accept the response.
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
)

// ============================================================================
// BODY
// ============================================================================

// Role identifies who is speaking in a conversation.
type Role string

const (
	RoleSystem    Role = "system"
	RoleUser      Role = "user"
	RoleAssistant Role = "assistant"
	RoleTool      Role = "tool"
)

// Message represents a single turn in a conversation.
type Message struct {
	Role    Role   `json:"role"`
	Content string `json:"content"`

	// ToolCalls holds any tool invocations requested by the model.
	ToolCalls []ToolCall `json:"tool_calls,omitempty"`

	// ToolCallID links a tool result back to the request.
	ToolCallID string `json:"tool_call_id,omitempty"`
}

// ToolCall represents the model requesting a tool invocation.
type ToolCall struct {
	ID       string `json:"id"`
	Name     string `json:"name"`
	Arguments string `json:"arguments"` // JSON string
}

// ToolDef defines a tool the model can invoke.
type ToolDef struct {
	Name        string `json:"name"`
	Description string `json:"description"`
	Parameters  any    `json:"parameters"` // JSON Schema
}

// ChatRequest is what goes TO a provider.
type ChatRequest struct {
	Messages    []Message `json:"messages"`
	Tools       []ToolDef `json:"tools,omitempty"`
	MaxTokens   int       `json:"max_tokens,omitempty"`
	Temperature float64   `json:"temperature,omitempty"`
	System      string    `json:"system,omitempty"`

	// --- Substrate-Agnostic Capabilities ---
	
	// ResponseFormat specifies the desired output format (e.g., "json").
	ResponseFormat string `json:"response_format,omitempty"`

	// EnableSearch instructs the provider to use internet search grounding if supported.
	EnableSearch bool `json:"enable_search,omitempty"`

	// EnableCodeExecution instructs the provider to execute code internally if supported.
	EnableCodeExecution bool `json:"enable_code_execution,omitempty"`
}

// ChatResponse is what comes BACK from a provider.
type ChatResponse struct {
	Content   string     `json:"content"`
	ToolCalls []ToolCall `json:"tool_calls,omitempty"`

	// Usage tracking for budget awareness.
	InputTokens  int `json:"input_tokens,omitempty"`
	OutputTokens int `json:"output_tokens,omitempty"`

	// StopReason indicates why the model stopped.
	StopReason string `json:"stop_reason,omitempty"`
}

// Provider is the universal interface for LLM reasoning.
// Any model API implements this to become available to CPI-SI.
type Provider interface {
	// Chat sends a request and returns the full response.
	Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error)

	// Name returns the provider identifier (e.g., "gemini", "anthropic").
	Name() string

	// Model returns the current model identifier.
	Model() string
}

// StreamChunk is a piece of a streaming response.
type StreamChunk struct {
	// Text content delta.
	Text string

	// ToolCalls received so far (accumulated).
	ToolCalls []ToolCall

	// Done signals the stream is complete.
	Done bool

	// Usage is populated on the final chunk.
	InputTokens  int
	OutputTokens int
	StopReason   string
}

// StreamProvider extends Provider with streaming support.
// Providers that support real-time output implement this.
type StreamProvider interface {
	Provider

	// ChatStream sends a request and returns chunks via channel.
	// The channel is closed when the stream completes.
	ChatStream(ctx context.Context, req ChatRequest) (<-chan StreamChunk, error)
}

// ============================================================================
// CLOSING
// ============================================================================
// "The LORD giveth wisdom." — Proverbs 2:6
// The provider gives reasoning. The agent gives direction.
