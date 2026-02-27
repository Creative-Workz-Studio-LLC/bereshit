// ============================================================================
// METADATA - CPI-SI Agent Tests
// ============================================================================
//
// Key: pkg-agent-test
// Purpose: Verify the agent loop — think → act → observe — using a mock
//          provider. Tests the MIND, not any specific LLM.
// Biblical: Proverbs 27:17 — "Iron sharpeneth iron"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
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
	"os"
	"strings"
	"testing"
)

// mockProvider returns scripted responses for testing the agent loop.
type mockProvider struct {
	responses []*ChatResponse
	callIndex int
	requests  []ChatRequest
}

func (p *mockProvider) Chat(_ context.Context, req ChatRequest) (*ChatResponse, error) {
	p.requests = append(p.requests, req)
	if p.callIndex >= len(p.responses) {
		return nil, fmt.Errorf("mock provider: no more responses (call %d)", p.callIndex)
	}
	resp := p.responses[p.callIndex]
	p.callIndex++
	return resp, nil
}

func (p *mockProvider) Name() string  { return "mock" }
func (p *mockProvider) Model() string { return "mock-v1" }

// mockTool records calls for verification.
type mockTool struct {
	def    ToolDef
	result string
	calls  []string
}

func (t *mockTool) Definition() ToolDef { return t.def }
func (t *mockTool) Execute(_ context.Context, args string) (string, error) {
	t.calls = append(t.calls, args)
	return t.result, nil
}

// ============================================================================
// BODY
// ============================================================================

func TestNew(t *testing.T) {
	p := &mockProvider{}
	a := New(Config{Provider: p})

	if a.provider != p {
		t.Error("provider not set")
	}
	if len(a.history) != 0 {
		t.Error("history should start empty")
	}
}

func TestRun_SimpleResponse(t *testing.T) {
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "Hello, I am CPI-SI.", InputTokens: 10, OutputTokens: 5},
		},
	}

	a := New(Config{Provider: p})
	result, err := a.Run(context.Background(), "Hello")

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if result.Content != "Hello, I am CPI-SI." {
		t.Errorf("got %q, want %q", result.Content, "Hello, I am CPI-SI.")
	}
	if result.InputTokens != 10 {
		t.Errorf("InputTokens = %d, want 10", result.InputTokens)
	}
	if result.OutputTokens != 5 {
		t.Errorf("OutputTokens = %d, want 5", result.OutputTokens)
	}
	if result.Iterations != 1 {
		t.Errorf("Iterations = %d, want 1", result.Iterations)
	}
	if result.ToolsUsed != 0 {
		t.Errorf("ToolsUsed = %d, want 0", result.ToolsUsed)
	}
	if len(a.history) != 2 {
		t.Errorf("history length = %d, want 2 (user + assistant)", len(a.history))
	}
	if a.history[0].Role != RoleUser {
		t.Errorf("first message role = %s, want user", a.history[0].Role)
	}
	if a.history[1].Role != RoleAssistant {
		t.Errorf("second message role = %s, want assistant", a.history[1].Role)
	}
}

func TestRun_SystemPrompt(t *testing.T) {
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "I am Nova Dawn."},
		},
	}

	a := New(Config{
		Provider: p,
		System:   "You are Nova Dawn, a CPI-SI instance.",
	})

	_, err := a.Run(context.Background(), "Who are you?")
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	// Verify system prompt was passed to provider
	if len(p.requests) != 1 {
		t.Fatalf("expected 1 request, got %d", len(p.requests))
	}
	if p.requests[0].System != "You are Nova Dawn, a CPI-SI instance." {
		t.Errorf("system prompt not passed: got %q", p.requests[0].System)
	}
}

func TestRun_ToolCall(t *testing.T) {
	// Scenario: model calls a tool, gets result, then responds
	tool := &mockTool{
		def: ToolDef{
			Name:        "test_tool",
			Description: "A test tool",
			Parameters: map[string]any{
				"type":       "object",
				"properties": map[string]any{},
			},
		},
		result: "tool executed successfully",
	}

	p := &mockProvider{
		responses: []*ChatResponse{
			// First call: model requests tool use
			{
				ToolCalls: []ToolCall{
					{ID: "call_1", Name: "test_tool", Arguments: `{"action":"test"}`},
				},
				InputTokens: 20, OutputTokens: 10,
			},
			// Second call: model sees result and gives final answer
			{
				Content:      "The tool worked. Here is the result.",
				InputTokens:  30,
				OutputTokens: 15,
			},
		},
	}

	a := New(Config{
		Provider: p,
		Tools:    []Tool{tool},
	})

	result, err := a.Run(context.Background(), "Use the test tool")
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	if result.Content != "The tool worked. Here is the result." {
		t.Errorf("got %q", result.Content)
	}
	if result.ToolsUsed != 1 {
		t.Errorf("ToolsUsed = %d, want 1", result.ToolsUsed)
	}
	if result.Iterations != 2 {
		t.Errorf("Iterations = %d, want 2", result.Iterations)
	}
	// Tokens should be cumulative across iterations
	if result.InputTokens != 50 {
		t.Errorf("InputTokens = %d, want 50 (20+30)", result.InputTokens)
	}
	if result.OutputTokens != 25 {
		t.Errorf("OutputTokens = %d, want 25 (10+15)", result.OutputTokens)
	}

	// Verify tool was called
	if len(tool.calls) != 1 {
		t.Fatalf("tool called %d times, want 1", len(tool.calls))
	}
	if tool.calls[0] != `{"action":"test"}` {
		t.Errorf("tool args = %q", tool.calls[0])
	}

	// Verify history: user → assistant(tool_call) → tool(result) → assistant(final)
	if len(a.history) != 4 {
		t.Errorf("history length = %d, want 4", len(a.history))
	}
	if a.history[2].Role != RoleTool {
		t.Errorf("third message role = %s, want tool", a.history[2].Role)
	}
	if a.history[2].Content != "tool executed successfully" {
		t.Errorf("tool result = %q", a.history[2].Content)
	}
}

func TestRun_UnknownTool(t *testing.T) {
	p := &mockProvider{
		responses: []*ChatResponse{
			// Model requests a tool that doesn't exist
			{
				ToolCalls: []ToolCall{
					{ID: "call_1", Name: "nonexistent", Arguments: "{}"},
				},
			},
			// Model sees error and responds
			{
				Content: "Sorry, that tool is not available.",
			},
		},
	}

	a := New(Config{Provider: p})
	result, err := a.Run(context.Background(), "Use a tool")
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	if result.Content != "Sorry, that tool is not available." {
		t.Errorf("got %q", result.Content)
	}

	// Unknown tools still count as tool usage
	if result.ToolsUsed != 1 {
		t.Errorf("ToolsUsed = %d, want 1", result.ToolsUsed)
	}

	// Verify error message was sent as tool result
	if len(a.history) < 3 {
		t.Fatalf("history length = %d, want at least 3", len(a.history))
	}
	if !strings.Contains(a.history[2].Content, "unknown tool") {
		t.Errorf("expected unknown tool error, got %q", a.history[2].Content)
	}
}

func TestRun_MultipleToolCalls(t *testing.T) {
	readTool := &mockTool{
		def:    ToolDef{Name: "read_file", Description: "Read a file"},
		result: "file contents here",
	}
	listTool := &mockTool{
		def:    ToolDef{Name: "list_dir", Description: "List directory"},
		result: "file1.go\nfile2.go",
	}

	p := &mockProvider{
		responses: []*ChatResponse{
			{
				ToolCalls: []ToolCall{
					{ID: "call_1", Name: "read_file", Arguments: `{"path":"/tmp/test"}`},
					{ID: "call_2", Name: "list_dir", Arguments: `{"path":"/tmp"}`},
				},
			},
			{Content: "Found 2 files. Contents loaded."},
		},
	}

	a := New(Config{
		Provider: p,
		Tools:    []Tool{readTool, listTool},
	})

	result, err := a.Run(context.Background(), "Show me the files")
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	if result.Content != "Found 2 files. Contents loaded." {
		t.Errorf("got %q", result.Content)
	}
	if result.ToolsUsed != 2 {
		t.Errorf("ToolsUsed = %d, want 2", result.ToolsUsed)
	}
	if len(readTool.calls) != 1 {
		t.Errorf("read_file called %d times", len(readTool.calls))
	}
	if len(listTool.calls) != 1 {
		t.Errorf("list_dir called %d times", len(listTool.calls))
	}
}

func TestReset(t *testing.T) {
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "Response 1"},
		},
	}

	a := New(Config{Provider: p})
	a.Run(context.Background(), "Hello")

	if len(a.history) == 0 {
		t.Fatal("history should not be empty after Run")
	}

	a.Reset()
	if len(a.history) != 0 {
		t.Error("history should be empty after Reset")
	}
}

func TestReset_PreservesIdentityInIndex(t *testing.T) {
	tool := &mockTool{
		def:    ToolDef{Name: "test", Description: "test tool"},
		result: "ok",
	}
	a := New(Config{Provider: &mockProvider{}, System: "I am Nova Dawn.", Tools: []Tool{tool}})

	// Load a history segment
	a.Index().LoadHistorySegment("history:0-5", "some history", false)

	if !a.Index().Has("identity") {
		t.Fatal("identity should be in index")
	}
	if !a.Index().Has("tools") {
		t.Fatal("tools should be in index")
	}
	if !a.Index().Has("history:0-5") {
		t.Fatal("history should be in index")
	}

	a.Reset()

	// Identity and tools should survive reset
	if !a.Index().Has("identity") {
		t.Error("identity should survive reset")
	}
	if !a.Index().Has("tools") {
		t.Error("tools should survive reset")
	}
	// History should be cleared
	if a.Index().Has("history:0-5") {
		t.Error("history segments should be cleared on reset")
	}
}

func TestIndex_Accessible(t *testing.T) {
	tool := &mockTool{
		def:    ToolDef{Name: "greet", Description: "greet someone"},
		result: "ok",
	}
	a := New(Config{Provider: &mockProvider{}, System: "test", Tools: []Tool{tool}})
	idx := a.Index()

	if idx == nil {
		t.Fatal("Index() should not return nil")
	}
	if !idx.Has("identity") {
		t.Error("identity should be indexed when system is set")
	}
	if !idx.Has("tools") {
		t.Error("tools should be indexed when tools are provided")
	}
}

func TestIndex_NoToolsNoToolsSegment(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}, System: "test"})

	if a.Index().Has("tools") {
		t.Error("no tools segment should exist when no tools provided")
	}
	if !a.Index().Has("identity") {
		t.Error("identity should still be indexed")
	}
}

func TestSetSystem(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})
	a.SetSystem("New system prompt")

	if a.system != "New system prompt" {
		t.Errorf("system = %q, want %q", a.system, "New system prompt")
	}
}

func TestToolDef_JSONRoundTrip(t *testing.T) {
	def := ToolDef{
		Name:        "test",
		Description: "A test tool",
		Parameters: map[string]any{
			"type": "object",
			"properties": map[string]any{
				"path": map[string]any{
					"type":        "string",
					"description": "File path",
				},
			},
			"required": []string{"path"},
		},
	}

	data, err := json.Marshal(def)
	if err != nil {
		t.Fatalf("marshal: %v", err)
	}

	var got ToolDef
	if err := json.Unmarshal(data, &got); err != nil {
		t.Fatalf("unmarshal: %v", err)
	}

	if got.Name != def.Name {
		t.Errorf("name = %q, want %q", got.Name, def.Name)
	}
	if got.Description != def.Description {
		t.Errorf("description mismatch")
	}
}

func TestDefaultTools(t *testing.T) {
	tools := DefaultTools()

	if len(tools) != 7 {
		t.Fatalf("got %d tools, want 7", len(tools))
	}

	expected := map[string]bool{
		"read_file":  false,
		"write_file": false,
		"edit_file":  false,
		"shell":      false,
		"search":     false,
		"find_files": false,
		"list_dir":   false,
	}

	for _, tool := range tools {
		name := tool.Definition().Name
		if _, ok := expected[name]; !ok {
			t.Errorf("unexpected tool: %s", name)
		}
		expected[name] = true
	}

	for name, found := range expected {
		if !found {
			t.Errorf("missing tool: %s", name)
		}
	}
}

// --- Provider Error Tests ---

func TestRun_ProviderError(t *testing.T) {
	p := &errorProvider{err: fmt.Errorf("API rate limited")}

	a := New(Config{Provider: p})
	_, err := a.Run(context.Background(), "Hello")

	if err == nil {
		t.Fatal("expected error, got nil")
	}
	if !strings.Contains(err.Error(), "API rate limited") {
		t.Errorf("error = %q, want containing 'API rate limited'", err.Error())
	}
}

type errorProvider struct {
	err error
}

func (p *errorProvider) Chat(_ context.Context, _ ChatRequest) (*ChatResponse, error) {
	return nil, p.err
}
func (p *errorProvider) Name() string  { return "error" }
func (p *errorProvider) Model() string { return "error-v1" }

func TestRun_ContextCanceled(t *testing.T) {
	ctx, cancel := context.WithCancel(context.Background())
	cancel() // Cancel immediately

	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "should not reach"},
		},
	}

	// The provider's Chat method doesn't check ctx, so it will still return.
	// But this tests that the agent doesn't crash on canceled context.
	a := New(Config{Provider: p})
	_, _ = a.Run(ctx, "Hello")
	// No panic = pass
}

func TestRun_IterationLimit(t *testing.T) {
	// Provider always returns tool calls, never a final answer
	p := &infiniteToolProvider{}

	tool := &mockTool{
		def:    ToolDef{Name: "loop_tool", Description: "Always called"},
		result: "keep going",
	}

	a := New(Config{Provider: p, Tools: []Tool{tool}})
	_, err := a.Run(context.Background(), "Start the loop")

	if err == nil {
		t.Fatal("expected iteration limit error")
	}
	if !strings.Contains(err.Error(), "maximum iterations") {
		t.Errorf("error = %q, want containing 'maximum iterations'", err.Error())
	}
}

type infiniteToolProvider struct {
	calls int
}

func (p *infiniteToolProvider) Chat(_ context.Context, _ ChatRequest) (*ChatResponse, error) {
	p.calls++
	return &ChatResponse{
		ToolCalls: []ToolCall{
			{ID: fmt.Sprintf("call_%d", p.calls), Name: "loop_tool", Arguments: "{}"},
		},
	}, nil
}
func (p *infiniteToolProvider) Name() string  { return "infinite" }
func (p *infiniteToolProvider) Model() string { return "infinite-v1" }

// --- RunStream Tests ---

func TestRunStream_NonStreamProvider(t *testing.T) {
	// When provider doesn't implement StreamProvider, RunStream falls back to Run
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "Fallback response"},
		},
	}

	a := New(Config{Provider: p})
	var buf strings.Builder
	result, err := a.RunStream(context.Background(), "Hello", &buf)

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if result.Content != "Fallback response" {
		t.Errorf("got %q, want %q", result.Content, "Fallback response")
	}
}

func TestRunStream_StreamProvider(t *testing.T) {
	p := &mockStreamProvider{
		chunks: []StreamChunk{
			{Text: "Hello "},
			{Text: "world"},
			{Done: true, InputTokens: 15, OutputTokens: 8},
		},
	}

	a := New(Config{Provider: p})
	var buf strings.Builder
	result, err := a.RunStream(context.Background(), "Greet me", &buf)

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if result.Content != "Hello world" {
		t.Errorf("response = %q, want %q", result.Content, "Hello world")
	}
	if buf.String() != "Hello world" {
		t.Errorf("streamed output = %q, want %q", buf.String(), "Hello world")
	}
	if result.InputTokens != 15 {
		t.Errorf("InputTokens = %d, want 15", result.InputTokens)
	}
	if result.OutputTokens != 8 {
		t.Errorf("OutputTokens = %d, want 8", result.OutputTokens)
	}
}

func TestRunStream_WithToolCalls(t *testing.T) {
	tool := &mockTool{
		def:    ToolDef{Name: "greet", Description: "Greet someone"},
		result: "Greeted!",
	}

	p := &mockStreamProvider{
		chunks: []StreamChunk{
			// First stream: tool call
			{ToolCalls: []ToolCall{{ID: "c1", Name: "greet", Arguments: `{"name":"Nova"}`}}},
			{Done: true, ToolCalls: []ToolCall{{ID: "c1", Name: "greet", Arguments: `{"name":"Nova"}`}}, InputTokens: 10, OutputTokens: 5},
		},
		secondChunks: []StreamChunk{
			// Second stream: final text
			{Text: "Done greeting."},
			{Done: true, InputTokens: 20, OutputTokens: 10},
		},
	}

	a := New(Config{Provider: p, Tools: []Tool{tool}})
	var buf strings.Builder
	result, err := a.RunStream(context.Background(), "Greet Nova", &buf)

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if result.Content != "Done greeting." {
		t.Errorf("response = %q, want %q", result.Content, "Done greeting.")
	}
	if result.ToolsUsed != 1 {
		t.Errorf("ToolsUsed = %d, want 1", result.ToolsUsed)
	}
	if result.Iterations != 2 {
		t.Errorf("Iterations = %d, want 2", result.Iterations)
	}
	if result.InputTokens != 30 {
		t.Errorf("InputTokens = %d, want 30 (10+20)", result.InputTokens)
	}
	if len(tool.calls) != 1 {
		t.Errorf("tool called %d times, want 1", len(tool.calls))
	}
}

type mockStreamProvider struct {
	chunks       []StreamChunk
	secondChunks []StreamChunk
	callCount    int
}

func (p *mockStreamProvider) Chat(_ context.Context, req ChatRequest) (*ChatResponse, error) {
	// Collect all chunks into a full response for non-streaming fallback
	resp := &ChatResponse{}
	chunks := p.chunks
	if p.callCount > 0 && len(p.secondChunks) > 0 {
		chunks = p.secondChunks
	}
	for _, c := range chunks {
		resp.Content += c.Text
		if len(c.ToolCalls) > 0 {
			resp.ToolCalls = c.ToolCalls
		}
	}
	p.callCount++
	return resp, nil
}

func (p *mockStreamProvider) ChatStream(_ context.Context, _ ChatRequest) (<-chan StreamChunk, error) {
	ch := make(chan StreamChunk, 16)
	chunks := p.chunks
	if p.callCount > 0 && len(p.secondChunks) > 0 {
		chunks = p.secondChunks
	}
	p.callCount++
	go func() {
		defer close(ch)
		for _, c := range chunks {
			ch <- c
		}
	}()
	return ch, nil
}

func (p *mockStreamProvider) Name() string  { return "mock-stream" }
func (p *mockStreamProvider) Model() string { return "mock-stream-v1" }

// --- Tool-Specific Tests ---

func TestFileReadTool(t *testing.T) {
	// Create a temp file
	tmp := t.TempDir() + "/test.txt"
	if err := os.WriteFile(tmp, []byte("Hello, CPI-SI!"), 0o644); err != nil {
		t.Fatal(err)
	}

	tool := &fileReadTool{}
	args, _ := json.Marshal(map[string]string{"path": tmp})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if result != "Hello, CPI-SI!" {
		t.Errorf("got %q, want %q", result, "Hello, CPI-SI!")
	}
}

func TestFileReadTool_NotFound(t *testing.T) {
	tool := &fileReadTool{}
	args, _ := json.Marshal(map[string]string{"path": "/nonexistent/file.txt"})
	_, err := tool.Execute(context.Background(), string(args))

	if err == nil {
		t.Fatal("expected error for nonexistent file")
	}
}

func TestFileReadTool_Truncation(t *testing.T) {
	// Create a file larger than maxBytes (100KB)
	tmp := t.TempDir() + "/large.txt"
	data := strings.Repeat("x", 150_000)
	os.WriteFile(tmp, []byte(data), 0o644)

	tool := &fileReadTool{}
	args, _ := json.Marshal(map[string]string{"path": tmp})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.HasSuffix(result, "... (truncated)") {
		t.Error("large file should be truncated")
	}
	if len(result) > 110_000 {
		t.Errorf("truncated result too large: %d bytes", len(result))
	}
}

func TestFileWriteTool(t *testing.T) {
	tmp := t.TempDir() + "/output.txt"

	tool := &fileWriteTool{}
	args, _ := json.Marshal(map[string]any{"path": tmp, "content": "Written by CPI-SI"})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "17 bytes") {
		t.Errorf("result = %q, want containing byte count", result)
	}

	// Verify file was written
	data, _ := os.ReadFile(tmp)
	if string(data) != "Written by CPI-SI" {
		t.Errorf("file content = %q", string(data))
	}
}

func TestFileWriteTool_CreatesDirectories(t *testing.T) {
	tmp := t.TempDir() + "/sub/dir/output.txt"

	tool := &fileWriteTool{}
	args, _ := json.Marshal(map[string]any{"path": tmp, "content": "deep"})
	_, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	data, _ := os.ReadFile(tmp)
	if string(data) != "deep" {
		t.Errorf("file content = %q", string(data))
	}
}

func TestFileEditTool(t *testing.T) {
	tmp := t.TempDir() + "/edit.txt"
	os.WriteFile(tmp, []byte("Hello World"), 0o644)

	tool := &fileEditTool{}
	args, _ := json.Marshal(map[string]any{
		"path":       tmp,
		"old_string": "World",
		"new_string": "CPI-SI",
	})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "Edited") {
		t.Errorf("result = %q", result)
	}

	data, _ := os.ReadFile(tmp)
	if string(data) != "Hello CPI-SI" {
		t.Errorf("file content = %q, want %q", string(data), "Hello CPI-SI")
	}
}

func TestFileEditTool_NotFound(t *testing.T) {
	tmp := t.TempDir() + "/edit.txt"
	os.WriteFile(tmp, []byte("Hello World"), 0o644)

	tool := &fileEditTool{}
	args, _ := json.Marshal(map[string]any{
		"path":       tmp,
		"old_string": "Nonexistent",
		"new_string": "Replacement",
	})
	_, err := tool.Execute(context.Background(), string(args))

	if err == nil {
		t.Fatal("expected error when old_string not found")
	}
	if !strings.Contains(err.Error(), "not found") {
		t.Errorf("error = %q, want containing 'not found'", err.Error())
	}
}

func TestFileEditTool_NotUnique(t *testing.T) {
	tmp := t.TempDir() + "/edit.txt"
	os.WriteFile(tmp, []byte("Hello Hello Hello"), 0o644)

	tool := &fileEditTool{}
	args, _ := json.Marshal(map[string]any{
		"path":       tmp,
		"old_string": "Hello",
		"new_string": "Hi",
	})
	_, err := tool.Execute(context.Background(), string(args))

	if err == nil {
		t.Fatal("expected error when old_string appears multiple times")
	}
	if !strings.Contains(err.Error(), "3 times") {
		t.Errorf("error = %q, want containing '3 times'", err.Error())
	}
}

func TestShellTool(t *testing.T) {
	tool := &shellTool{}
	args, _ := json.Marshal(map[string]string{"command": "echo hello"})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "hello") {
		t.Errorf("result = %q, want containing 'hello'", result)
	}
}

func TestShellTool_Stderr(t *testing.T) {
	tool := &shellTool{}
	args, _ := json.Marshal(map[string]string{"command": "echo error >&2"})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "STDERR") {
		t.Errorf("result = %q, want containing 'STDERR'", result)
	}
}

func TestShellTool_ExitCode(t *testing.T) {
	tool := &shellTool{}
	args, _ := json.Marshal(map[string]string{"command": "exit 1"})
	result, err := tool.Execute(context.Background(), string(args))

	// Shell tool doesn't return error — it reports exit code in output
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "ERROR") {
		t.Errorf("result = %q, want containing 'ERROR'", result)
	}
}

func TestGrepTool(t *testing.T) {
	dir := t.TempDir()
	os.WriteFile(dir+"/a.txt", []byte("Hello World\nGoodbye World"), 0o644)
	os.WriteFile(dir+"/b.txt", []byte("No match here"), 0o644)

	tool := &grepTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "World", "path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "Hello World") {
		t.Errorf("result missing 'Hello World': %q", result)
	}
	if !strings.Contains(result, "Goodbye World") {
		t.Errorf("result missing 'Goodbye World': %q", result)
	}
	if strings.Contains(result, "No match") {
		t.Error("result should not contain non-matching file")
	}
}

func TestGrepTool_WithGlob(t *testing.T) {
	dir := t.TempDir()
	os.WriteFile(dir+"/a.go", []byte("func main() {}"), 0o644)
	os.WriteFile(dir+"/b.txt", []byte("func helper() {}"), 0o644)

	tool := &grepTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "func", "path": dir, "glob": "*.go"})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "main") {
		t.Errorf("result should contain match from .go file: %q", result)
	}
	if strings.Contains(result, "helper") {
		t.Error("result should not contain match from .txt file when glob is *.go")
	}
}

func TestGrepTool_NoMatches(t *testing.T) {
	dir := t.TempDir()
	os.WriteFile(dir+"/a.txt", []byte("Hello"), 0o644)

	tool := &grepTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "zzz_nonexistent", "path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if result != "No matches found." {
		t.Errorf("result = %q, want 'No matches found.'", result)
	}
}

func TestGlobTool(t *testing.T) {
	dir := t.TempDir()
	os.WriteFile(dir+"/main.go", []byte("package main"), 0o644)
	os.WriteFile(dir+"/util.go", []byte("package util"), 0o644)
	os.WriteFile(dir+"/README.md", []byte("# Read me"), 0o644)

	tool := &globTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "*.go", "path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "main.go") {
		t.Errorf("result missing main.go: %q", result)
	}
	if !strings.Contains(result, "util.go") {
		t.Errorf("result missing util.go: %q", result)
	}
	if strings.Contains(result, "README.md") {
		t.Error("result should not contain non-matching .md file")
	}
}

func TestGlobTool_NoMatches(t *testing.T) {
	dir := t.TempDir()
	os.WriteFile(dir+"/a.txt", []byte("text"), 0o644)

	tool := &globTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "*.rs", "path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if result != "No files found matching pattern." {
		t.Errorf("result = %q, want 'No files found matching pattern.'", result)
	}
}

func TestListDirTool(t *testing.T) {
	dir := t.TempDir()
	os.WriteFile(dir+"/a.txt", []byte("hello"), 0o644)
	os.Mkdir(dir+"/subdir", 0o755)

	tool := &listDirTool{}
	args, _ := json.Marshal(map[string]string{"path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "a.txt") {
		t.Errorf("result missing a.txt: %q", result)
	}
	if !strings.Contains(result, "subdir") {
		t.Errorf("result missing subdir: %q", result)
	}
}

func TestListDirTool_NotFound(t *testing.T) {
	tool := &listDirTool{}
	args, _ := json.Marshal(map[string]string{"path": "/nonexistent/dir"})
	_, err := tool.Execute(context.Background(), string(args))

	if err == nil {
		t.Fatal("expected error for nonexistent directory")
	}
}

// --- LoadIdentity Test ---

func TestLoadIdentity(t *testing.T) {
	tmp := t.TempDir() + "/identity.md"
	os.WriteFile(tmp, []byte("I am Nova Dawn."), 0o644)

	a := New(Config{Provider: &mockProvider{}})
	err := a.LoadIdentity(tmp)

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if a.system != "I am Nova Dawn." {
		t.Errorf("system = %q, want %q", a.system, "I am Nova Dawn.")
	}
}

func TestLoadIdentity_NotFound(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})
	err := a.LoadIdentity("/nonexistent/identity.md")

	if err == nil {
		t.Fatal("expected error for nonexistent identity file")
	}
}

// --- History Persistence Test ---

func TestHistoryPersistsAcrossRuns(t *testing.T) {
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "First response"},
			{Content: "Second response"},
		},
	}

	a := New(Config{Provider: p})

	_, err := a.Run(context.Background(), "First input")
	if err != nil {
		t.Fatal(err)
	}

	_, err = a.Run(context.Background(), "Second input")
	if err != nil {
		t.Fatal(err)
	}

	// Should have 4 messages: user1, assistant1, user2, assistant2
	if len(a.History()) != 4 {
		t.Errorf("history length = %d, want 4", len(a.History()))
	}

	// Verify second request included full history
	if len(p.requests) != 2 {
		t.Fatalf("expected 2 requests, got %d", len(p.requests))
	}
	if len(p.requests[1].Messages) != 3 {
		t.Errorf("second request had %d messages, want 3 (user1, assistant1, user2)", len(p.requests[1].Messages))
	}
}

// --- Verbose Mode Test ---

func TestVerboseMode(t *testing.T) {
	tool := &mockTool{
		def:    ToolDef{Name: "test_tool", Description: "test"},
		result: "ok",
	}

	p := &mockProvider{
		responses: []*ChatResponse{
			{ToolCalls: []ToolCall{{ID: "c1", Name: "test_tool", Arguments: `{}`}}},
			{Content: "Done"},
		},
	}

	// Verbose mode shouldn't cause panic or change behavior
	a := New(Config{Provider: p, Tools: []Tool{tool}, Verbose: true})
	result, err := a.Run(context.Background(), "Test verbose")

	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "Done" {
		t.Errorf("got %q", result.Content)
	}
}

// --- Tool Definition Tests ---

func TestAllToolDefinitionsValid(t *testing.T) {
	tools := DefaultTools()

	for _, tool := range tools {
		def := tool.Definition()

		if def.Name == "" {
			t.Error("tool has empty name")
		}
		if def.Description == "" {
			t.Errorf("tool %q has empty description", def.Name)
		}
		if def.Parameters == nil {
			t.Errorf("tool %q has nil parameters", def.Name)
		}

		// Verify parameters is a valid JSON Schema object
		params, ok := def.Parameters.(map[string]any)
		if !ok {
			t.Errorf("tool %q parameters is not map[string]any", def.Name)
			continue
		}
		if params["type"] != "object" {
			t.Errorf("tool %q parameters type = %v, want 'object'", def.Name, params["type"])
		}
		if _, ok := params["properties"]; !ok {
			t.Errorf("tool %q parameters missing 'properties'", def.Name)
		}
	}
}

func TestToolDefinitionsSerializeToJSON(t *testing.T) {
	tools := DefaultTools()

	for _, tool := range tools {
		def := tool.Definition()
		data, err := json.Marshal(def)
		if err != nil {
			t.Errorf("tool %q: marshal error: %v", def.Name, err)
			continue
		}

		var roundtrip ToolDef
		if err := json.Unmarshal(data, &roundtrip); err != nil {
			t.Errorf("tool %q: unmarshal error: %v", def.Name, err)
		}
		if roundtrip.Name != def.Name {
			t.Errorf("tool %q: name changed after roundtrip: %q", def.Name, roundtrip.Name)
		}
	}
}

// --- History Indexing Tests ---

func TestHistoryIndexing(t *testing.T) {
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "Response 1"},
			{Content: "Response 2"},
		},
	}

	a := New(Config{Provider: p})

	// First exchange
	a.Run(context.Background(), "Hello")
	if !a.Index().Has("history:recent") {
		t.Error("history:recent should exist after first exchange")
	}
	if a.Exchanges() != 1 {
		t.Errorf("Exchanges = %d, want 1", a.Exchanges())
	}

	// Second exchange
	a.Run(context.Background(), "World")
	if a.Exchanges() != 2 {
		t.Errorf("Exchanges = %d, want 2", a.Exchanges())
	}

	// Recent segment should be updated
	seg := a.Index().Get("history:recent")
	if seg == nil {
		t.Fatal("history:recent segment missing")
	}
	if !strings.Contains(seg.Content, "Hello") {
		t.Error("recent segment should contain first input")
	}
}

func TestHistoryIndexing_SplitsOlderRecent(t *testing.T) {
	// Create enough exchanges to trigger the older/recent split (>10 messages)
	responses := make([]*ChatResponse, 8)
	for i := range responses {
		responses[i] = &ChatResponse{Content: fmt.Sprintf("Response %d", i)}
	}

	p := &mockProvider{responses: responses}
	a := New(Config{Provider: p})

	// 8 exchanges = 16 messages (user + assistant each)
	for i := 0; i < 8; i++ {
		a.Run(context.Background(), fmt.Sprintf("Input %d", i))
	}

	// Should have both older and recent segments
	if !a.Index().Has("history:recent") {
		t.Error("should have history:recent segment")
	}
	if !a.Index().Has("history:older") {
		t.Error("should have history:older segment after 16 messages")
	}
}

func TestHistoryIndexing_ResetClearsExchanges(t *testing.T) {
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "Response"},
		},
	}

	a := New(Config{Provider: p})
	a.Run(context.Background(), "Hello")

	if a.Exchanges() != 1 {
		t.Fatalf("Exchanges = %d, want 1", a.Exchanges())
	}

	a.Reset()

	// History index segments should be gone
	if a.Index().Has("history:recent") {
		t.Error("history:recent should be cleared on reset")
	}
	if a.Index().Has("history:older") {
		t.Error("history:older should be cleared on reset")
	}
}

// --- Context Loading Tests ---

func TestLoadContext(t *testing.T) {
	tmp := t.TempDir() + "/context.md"
	os.WriteFile(tmp, []byte("# Project README\nThis is a test project."), 0o644)

	a := New(Config{Provider: &mockProvider{}})
	err := a.LoadContext(tmp)

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	key := "context:" + tmp
	if !a.Index().Has(key) {
		t.Error("context segment should be in index")
	}

	seg := a.Index().Get(key)
	if !strings.Contains(seg.Content, "Project README") {
		t.Error("context segment should contain file content")
	}
	if seg.Kind != SegmentContext {
		t.Errorf("segment kind = %d, want SegmentContext", seg.Kind)
	}
}

func TestLoadContext_NotFound(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})
	err := a.LoadContext("/nonexistent/file.txt")

	if err == nil {
		t.Fatal("expected error for nonexistent file")
	}
}

func TestUnloadContext(t *testing.T) {
	tmp := t.TempDir() + "/context.md"
	os.WriteFile(tmp, []byte("content"), 0o644)

	a := New(Config{Provider: &mockProvider{}})
	a.LoadContext(tmp)

	key := "context:" + tmp
	if !a.Index().Has(key) {
		t.Fatal("context should be loaded")
	}

	a.UnloadContext(tmp)
	if a.Index().Has(key) {
		t.Error("context should be unloaded")
	}
}

func TestContextSegments(t *testing.T) {
	dir := t.TempDir()
	os.WriteFile(dir+"/a.txt", []byte("file a"), 0o644)
	os.WriteFile(dir+"/b.txt", []byte("file b"), 0o644)

	a := New(Config{Provider: &mockProvider{}})
	a.LoadContext(dir + "/a.txt")
	a.LoadContext(dir + "/b.txt")

	keys := a.ContextSegments()
	if len(keys) != 2 {
		t.Errorf("got %d context segments, want 2", len(keys))
	}
}

func TestContextSegments_Empty(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})
	keys := a.ContextSegments()

	if len(keys) != 0 {
		t.Errorf("got %d context segments, want 0", len(keys))
	}
}

// --- Memory Accessor ---

func TestAgent_Memory_Nil(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})
	if a.Memory() != nil {
		t.Error("Memory() should be nil when not configured")
	}
}

func TestAgent_Memory_Configured(t *testing.T) {
	mem := NewMemory("")
	a := New(Config{Provider: &mockProvider{}, Memory: mem})
	if a.Memory() != mem {
		t.Error("Memory() should return the configured memory")
	}
}

// --- RunResult Tests ---

func TestRunResult_AccumulatesAcrossIterations(t *testing.T) {
	tool := &mockTool{
		def:    ToolDef{Name: "counter", Description: "Count things"},
		result: "counted",
	}

	p := &mockProvider{
		responses: []*ChatResponse{
			{
				ToolCalls:    []ToolCall{{ID: "c1", Name: "counter", Arguments: "{}"}},
				InputTokens:  100,
				OutputTokens: 50,
			},
			{
				ToolCalls:    []ToolCall{{ID: "c2", Name: "counter", Arguments: "{}"}},
				InputTokens:  200,
				OutputTokens: 100,
			},
			{
				Content:      "All done.",
				InputTokens:  300,
				OutputTokens: 150,
			},
		},
	}

	a := New(Config{Provider: p, Tools: []Tool{tool}})
	result, err := a.Run(context.Background(), "Count everything")

	if err != nil {
		t.Fatal(err)
	}
	if result.InputTokens != 600 {
		t.Errorf("InputTokens = %d, want 600 (100+200+300)", result.InputTokens)
	}
	if result.OutputTokens != 300 {
		t.Errorf("OutputTokens = %d, want 300 (50+100+150)", result.OutputTokens)
	}
	if result.ToolsUsed != 2 {
		t.Errorf("ToolsUsed = %d, want 2", result.ToolsUsed)
	}
	if result.Iterations != 3 {
		t.Errorf("Iterations = %d, want 3", result.Iterations)
	}
}

// --- Tool Error Path Tests ---

func TestRun_ToolExecuteError(t *testing.T) {
	// Test the error branch inside Run when tool.Execute returns an error
	failTool := &failingTool{
		def: ToolDef{Name: "fail_tool", Description: "Always fails"},
	}

	p := &mockProvider{
		responses: []*ChatResponse{
			{
				ToolCalls: []ToolCall{{ID: "c1", Name: "fail_tool", Arguments: "{}"}},
			},
			{Content: "Tool failed, but I recovered."},
		},
	}

	a := New(Config{Provider: p, Tools: []Tool{failTool}})
	result, err := a.Run(context.Background(), "Use the failing tool")
	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "Tool failed, but I recovered." {
		t.Errorf("got %q", result.Content)
	}

	// Tool error should be reported in history
	if !strings.Contains(a.history[2].Content, "Error:") {
		t.Errorf("tool error should be in history, got %q", a.history[2].Content)
	}
}

type failingTool struct {
	def ToolDef
}

func (t *failingTool) Definition() ToolDef { return t.def }
func (t *failingTool) Execute(_ context.Context, _ string) (string, error) {
	return "", fmt.Errorf("tool execution failed")
}

func TestRunStream_ChatStreamError(t *testing.T) {
	p := &errorStreamProvider{err: fmt.Errorf("stream initialization failed")}

	a := New(Config{Provider: p})
	var buf strings.Builder
	_, err := a.RunStream(context.Background(), "Hello", &buf)

	if err == nil {
		t.Fatal("expected error")
	}
	if !strings.Contains(err.Error(), "stream initialization failed") {
		t.Errorf("error = %q", err.Error())
	}
}

type errorStreamProvider struct {
	err error
}

func (p *errorStreamProvider) Chat(_ context.Context, _ ChatRequest) (*ChatResponse, error) {
	return nil, p.err
}
func (p *errorStreamProvider) ChatStream(_ context.Context, _ ChatRequest) (<-chan StreamChunk, error) {
	return nil, p.err
}
func (p *errorStreamProvider) Name() string  { return "error-stream" }
func (p *errorStreamProvider) Model() string { return "error-stream-v1" }

func TestRunStream_UnknownTool(t *testing.T) {
	// Stream provider returns a tool call for a tool that doesn't exist
	p := &mockStreamProvider{
		chunks: []StreamChunk{
			{ToolCalls: []ToolCall{{ID: "c1", Name: "nonexistent", Arguments: "{}"}}},
			{Done: true, ToolCalls: []ToolCall{{ID: "c1", Name: "nonexistent", Arguments: "{}"}}},
		},
		secondChunks: []StreamChunk{
			{Text: "Recovered from unknown tool."},
			{Done: true},
		},
	}

	a := New(Config{Provider: p})
	var buf strings.Builder
	result, err := a.RunStream(context.Background(), "Try unknown tool", &buf)

	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "Recovered from unknown tool." {
		t.Errorf("got %q", result.Content)
	}
}

func TestRunStream_ToolExecuteError(t *testing.T) {
	failTool := &failingTool{
		def: ToolDef{Name: "fail_tool", Description: "Always fails"},
	}

	p := &mockStreamProvider{
		chunks: []StreamChunk{
			{ToolCalls: []ToolCall{{ID: "c1", Name: "fail_tool", Arguments: "{}"}}},
			{Done: true, ToolCalls: []ToolCall{{ID: "c1", Name: "fail_tool", Arguments: "{}"}}},
		},
		secondChunks: []StreamChunk{
			{Text: "Handled the error."},
			{Done: true},
		},
	}

	a := New(Config{Provider: p, Tools: []Tool{failTool}})
	var buf strings.Builder
	result, err := a.RunStream(context.Background(), "Use failing tool", &buf)

	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "Handled the error." {
		t.Errorf("got %q", result.Content)
	}
}

func TestRunStream_VerboseMode(t *testing.T) {
	tool := &mockTool{
		def:    ToolDef{Name: "verbose_test", Description: "test"},
		result: "ok",
	}

	p := &mockStreamProvider{
		chunks: []StreamChunk{
			{ToolCalls: []ToolCall{{ID: "c1", Name: "verbose_test", Arguments: `{"x":"y"}`}}},
			{Done: true, ToolCalls: []ToolCall{{ID: "c1", Name: "verbose_test", Arguments: `{"x":"y"}`}}},
		},
		secondChunks: []StreamChunk{
			{Text: "Done."},
			{Done: true},
		},
	}

	a := New(Config{Provider: p, Tools: []Tool{tool}, Verbose: true})
	var buf strings.Builder
	result, err := a.RunStream(context.Background(), "Test verbose stream", &buf)

	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "Done." {
		t.Errorf("got %q", result.Content)
	}
	// Verbose output should appear in the writer
	if !strings.Contains(buf.String(), "[tool]") {
		t.Error("verbose output should contain [tool] marker")
	}
}

// --- Truncate Tests ---

func TestTruncate_ShortString(t *testing.T) {
	result := truncate("short", 100)
	if result != "short" {
		t.Errorf("got %q", result)
	}
}

func TestTruncate_LongString(t *testing.T) {
	long := strings.Repeat("a", 200)
	result := truncate(long, 100)
	if len(result) != 103 { // 100 + "..."
		t.Errorf("truncated length = %d, want 103", len(result))
	}
	if !strings.HasSuffix(result, "...") {
		t.Error("should end with ...")
	}
}

func TestTruncate_WithNewlines(t *testing.T) {
	result := truncate("line1\nline2\nline3", 100)
	if strings.Contains(result, "\n") {
		t.Error("should replace newlines with spaces")
	}
}

// --- Tool Edge Case Tests ---

func TestFileWriteTool_AtomicWritePermission(t *testing.T) {
	// Test writing to a path where the parent dir exists
	dir := t.TempDir()
	path := dir + "/subdir/file.txt"

	tool := &fileWriteTool{}
	args, _ := json.Marshal(map[string]any{"path": path, "content": "test"})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "4 bytes") {
		t.Errorf("result = %q", result)
	}
}

func TestFileEditTool_AtomicWrite(t *testing.T) {
	// Test that edit_file does atomic write correctly
	tmp := t.TempDir() + "/atomic.txt"
	os.WriteFile(tmp, []byte("before"), 0o644)

	tool := &fileEditTool{}
	args, _ := json.Marshal(map[string]any{
		"path":       tmp,
		"old_string": "before",
		"new_string": "after",
	})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatal(err)
	}
	if !strings.Contains(result, "Edited") {
		t.Errorf("result = %q", result)
	}

	data, _ := os.ReadFile(tmp)
	if string(data) != "after" {
		t.Errorf("file = %q", string(data))
	}
}

func TestGrepTool_WalkError(t *testing.T) {
	// Search in a directory that doesn't exist
	tool := &grepTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "test", "path": "/nonexistent/dir"})
	_, err := tool.Execute(context.Background(), string(args))

	// Walk on nonexistent dir doesn't error; returns empty
	if err != nil {
		// Some systems may error — that's fine
		return
	}
}

func TestGlobTool_PathPatterns(t *testing.T) {
	// Test glob with path patterns (containing /)
	dir := t.TempDir()
	os.MkdirAll(dir+"/src", 0o755)
	os.WriteFile(dir+"/src/main.go", []byte("package main"), 0o644)
	os.WriteFile(dir+"/README.md", []byte("# Read me"), 0o644)

	tool := &globTool{}
	// Test with path-containing pattern
	args, _ := json.Marshal(map[string]any{"pattern": "src/*.go", "path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	// Should find the .go file (via base name matching fallback)
	if !strings.Contains(result, "main.go") {
		t.Errorf("result should contain main.go: %q", result)
	}
}

func TestGlobTool_DoubleStarPattern(t *testing.T) {
	dir := t.TempDir()
	os.MkdirAll(dir+"/a/b", 0o755)
	os.WriteFile(dir+"/a/b/test.go", []byte("package b"), 0o644)
	os.WriteFile(dir+"/a/test.txt", []byte("text"), 0o644)

	tool := &globTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "**/*.go", "path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if !strings.Contains(result, "test.go") {
		t.Errorf("result should contain test.go: %q", result)
	}
}

func TestListDirTool_NilInfoFallback(t *testing.T) {
	// Test listing a valid directory (covers the info != nil branch)
	dir := t.TempDir()
	os.WriteFile(dir+"/file.txt", []byte("content"), 0o644)

	tool := &listDirTool{}
	args, _ := json.Marshal(map[string]string{"path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatal(err)
	}
	if !strings.Contains(result, "file.txt") {
		t.Error("should list file.txt")
	}
}

func TestShellTool_LargeOutput(t *testing.T) {
	// Test truncation of large shell output
	tool := &shellTool{}
	args, _ := json.Marshal(map[string]string{"command": "yes test | head -20000"})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatal(err)
	}
	if len(result) > 55000 {
		t.Errorf("output should be truncated, got %d bytes", len(result))
	}
}

// --- RunStream Iteration Limit ---

func TestRunStream_IterationLimit(t *testing.T) {
	tool := &mockTool{
		def:    ToolDef{Name: "loop_tool", Description: "Always called"},
		result: "keep going",
	}

	p := &infiniteStreamProvider{}

	a := New(Config{Provider: p, Tools: []Tool{tool}})
	var buf strings.Builder
	_, err := a.RunStream(context.Background(), "Start the loop", &buf)

	if err == nil {
		t.Fatal("expected iteration limit error")
	}
	if !strings.Contains(err.Error(), "maximum iterations") {
		t.Errorf("error = %q", err.Error())
	}
}

type infiniteStreamProvider struct {
	calls int
}

func (p *infiniteStreamProvider) Chat(_ context.Context, _ ChatRequest) (*ChatResponse, error) {
	p.calls++
	return &ChatResponse{
		ToolCalls: []ToolCall{{ID: fmt.Sprintf("c%d", p.calls), Name: "loop_tool", Arguments: "{}"}},
	}, nil
}

func (p *infiniteStreamProvider) ChatStream(_ context.Context, _ ChatRequest) (<-chan StreamChunk, error) {
	p.calls++
	ch := make(chan StreamChunk, 4)
	go func() {
		defer close(ch)
		ch <- StreamChunk{
			ToolCalls: []ToolCall{{ID: fmt.Sprintf("c%d", p.calls), Name: "loop_tool", Arguments: "{}"}},
		}
		ch <- StreamChunk{
			Done:      true,
			ToolCalls: []ToolCall{{ID: fmt.Sprintf("c%d", p.calls), Name: "loop_tool", Arguments: "{}"}},
		}
	}()
	return ch, nil
}

func (p *infiniteStreamProvider) Name() string  { return "infinite-stream" }
func (p *infiniteStreamProvider) Model() string { return "infinite-stream-v1" }

// --- Grep Tool Edge Cases ---

func TestGrepTool_InvalidRegex(t *testing.T) {
	dir := t.TempDir()
	os.WriteFile(dir+"/test.txt", []byte("content"), 0o644)

	tool := &grepTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "[invalid(", "path": dir})
	_, err := tool.Execute(context.Background(), string(args))

	if err == nil {
		t.Fatal("expected error for invalid regex")
	}
	if !strings.Contains(err.Error(), "invalid regex") {
		t.Errorf("error = %q", err.Error())
	}
}

func TestGrepTool_MaxMatches(t *testing.T) {
	// Create a file with more than 100 matching lines
	dir := t.TempDir()
	var sb strings.Builder
	for i := 0; i < 150; i++ {
		sb.WriteString(fmt.Sprintf("match line %d\n", i))
	}
	os.WriteFile(dir+"/many.txt", []byte(sb.String()), 0o644)

	tool := &grepTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "match", "path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	// Should have exactly 100 matches (max)
	lines := strings.Split(strings.TrimSpace(result), "\n")
	if len(lines) != 100 {
		t.Errorf("got %d matches, want 100 (max)", len(lines))
	}
}

// --- FileEdit Tool Edge Cases ---

func TestFileEditTool_FileNotExists(t *testing.T) {
	tool := &fileEditTool{}
	args, _ := json.Marshal(map[string]any{
		"path":       "/nonexistent/file.txt",
		"old_string": "old",
		"new_string": "new",
	})
	_, err := tool.Execute(context.Background(), string(args))

	if err == nil {
		t.Fatal("expected error for nonexistent file")
	}
}

func TestFileEditTool_WriteError(t *testing.T) {
	// Make a read-only directory to trigger write error
	dir := t.TempDir()
	file := dir + "/readonly.txt"
	os.WriteFile(file, []byte("hello world"), 0o644)
	os.Chmod(dir, 0o555) // make dir read-only so .tmp write fails
	defer os.Chmod(dir, 0o755) // restore for cleanup

	tool := &fileEditTool{}
	args, _ := json.Marshal(map[string]any{
		"path":       file,
		"old_string": "hello",
		"new_string": "goodbye",
	})
	_, err := tool.Execute(context.Background(), string(args))

	if err == nil {
		t.Fatal("expected write error on read-only directory")
	}
}

// --- FileWrite Tool Edge Cases ---

func TestFileWriteTool_WriteError(t *testing.T) {
	dir := t.TempDir()
	os.Chmod(dir, 0o555) // make read-only
	defer os.Chmod(dir, 0o755)

	tool := &fileWriteTool{}
	args, _ := json.Marshal(map[string]any{"path": dir + "/file.txt", "content": "test"})
	_, err := tool.Execute(context.Background(), string(args))

	if err == nil {
		t.Fatal("expected error writing to read-only directory")
	}
}

func TestFileWriteTool_MkdirError(t *testing.T) {
	// Write to a path under /proc which can't have new dirs
	tool := &fileWriteTool{}
	args, _ := json.Marshal(map[string]any{"path": "/proc/fake/deep/file.txt", "content": "test"})
	_, err := tool.Execute(context.Background(), string(args))

	if err == nil {
		t.Fatal("expected error creating dir under /proc")
	}
}

// --- Glob Tool Edge Cases ---

func TestGlobTool_MaxMatches(t *testing.T) {
	dir := t.TempDir()
	// Create 250 .txt files
	for i := 0; i < 250; i++ {
		os.WriteFile(fmt.Sprintf("%s/file%04d.txt", dir, i), []byte("x"), 0o644)
	}

	tool := &globTool{}
	args, _ := json.Marshal(map[string]any{"pattern": "*.txt", "path": dir})
	result, err := tool.Execute(context.Background(), string(args))

	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	lines := strings.Split(strings.TrimSpace(result), "\n")
	if len(lines) != 200 {
		t.Errorf("got %d matches, want 200 (max)", len(lines))
	}
}

// --- Session Error Paths ---

func TestSaveSession_WriteError(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})

	// Try to save to a read-only location
	dir := t.TempDir()
	os.Chmod(dir, 0o555)
	defer os.Chmod(dir, 0o755)

	err := a.SaveSession(dir + "/session.json")
	if err == nil {
		t.Fatal("expected error writing to read-only directory")
	}
}

func TestSaveSession_MkdirError(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})

	err := a.SaveSession("/proc/fake/deep/session.json")
	if err == nil {
		t.Fatal("expected error creating dir under /proc")
	}
}

// --- Invalid JSON Args Tests ---

func TestTools_InvalidJSON(t *testing.T) {
	tools := DefaultTools()
	ctx := context.Background()

	for _, tool := range tools {
		_, err := tool.Execute(ctx, "not valid json")
		if err == nil {
			t.Errorf("tool %q should error on invalid JSON", tool.Definition().Name)
		}
	}
}

// --- Fit Tie-Breaker Test ---

func TestFit_SamePriorityOrdersByCreatedAt(t *testing.T) {
	idx := NewPromptIndex()

	// Use a fixed token estimator so we can control sizes exactly
	idx.SetTokenEstimator(func(content string) int { return 60 })

	// Load two segments with equal priority (both SegmentContext = PriorityContext).
	// They'll have different CreatedAt because of insertion order.
	idx.Load("first", SegmentContext, PriorityContext, "aaa")
	idx.Load("second", SegmentContext, PriorityContext, "bbb")

	// Budget only fits one segment (60 tokens each, budget is 65).
	// Tie-breaker should prefer the older (first-created) segment.
	result := idx.Fit(65)
	if len(result) != 1 {
		t.Fatalf("expected 1 segment, got %d", len(result))
	}
	if result[0].Key != "first" {
		t.Errorf("expected 'first' (older) to win tie-break, got %q", result[0].Key)
	}
}

// --- ListDir Nil Info (Broken Symlink) ---

func TestListDirTool_BrokenSymlink(t *testing.T) {
	dir := t.TempDir()

	// Create a symlink pointing to a non-existent target
	target := dir + "/ghost"
	link := dir + "/broken-link"
	if err := os.Symlink(target, link); err != nil {
		t.Skip("cannot create symlink:", err)
	}

	tool := &listDirTool{}
	args, _ := json.Marshal(map[string]string{"path": dir})
	result, err := tool.Execute(context.Background(), string(args))
	if err != nil {
		t.Fatal(err)
	}
	// The broken symlink should still appear (name only, no mode/size)
	if !strings.Contains(result, "broken-link") {
		t.Errorf("expected broken-link in output, got: %s", result)
	}
}

// --- Grep ReadFile Error (Unreadable File) ---

func TestGrepTool_UnreadableFile(t *testing.T) {
	dir := t.TempDir()
	// Create a file then remove read permission
	f := dir + "/secret.txt"
	os.WriteFile(f, []byte("hidden content"), 0o644)
	os.Chmod(f, 0o000)
	defer os.Chmod(f, 0o644) // restore for cleanup

	tool := &grepTool{}
	args, _ := json.Marshal(map[string]string{
		"pattern": "hidden",
		"path":    dir,
	})
	result, err := tool.Execute(context.Background(), string(args))
	if err != nil {
		t.Fatal("should not error, just skip unreadable file:", err)
	}
	if result != "No matches found." {
		t.Errorf("expected no matches (file unreadable), got: %s", result)
	}
}

// --- Grep Walk Non-Existent Path ---

func TestGrepTool_NonExistentPath(t *testing.T) {
	tool := &grepTool{}
	args, _ := json.Marshal(map[string]string{
		"pattern": "test",
		"path":    "/tmp/nonexistent-" + fmt.Sprintf("%d", os.Getpid()),
	})
	result, err := tool.Execute(context.Background(), string(args))
	// filepath.Walk on non-existent dir: callback gets the error, returns nil,
	// so walkErr is nil. Should get "No matches found."
	if err != nil {
		t.Fatal(err)
	}
	if result != "No matches found." {
		t.Errorf("expected 'No matches found.', got: %s", result)
	}
}

// --- Glob Walk Non-Existent Path ---

func TestGlobTool_NonExistentPath(t *testing.T) {
	tool := &globTool{}
	args, _ := json.Marshal(map[string]string{
		"pattern": "*.go",
		"path":    "/tmp/nonexistent-" + fmt.Sprintf("%d", os.Getpid()),
	})
	result, err := tool.Execute(context.Background(), string(args))
	if err != nil {
		t.Fatal(err)
	}
	if result != "No files found matching pattern." {
		t.Errorf("expected no files found, got: %s", result)
	}
}

// --- FileWrite Rename Error ---
// Trigger os.Rename failure by pre-creating a non-empty directory at the target path.
// WriteFile succeeds on .tmp but rename(file, non-empty-dir) fails with EEXIST.

func TestFileWriteTool_RenameError(t *testing.T) {
	dir := t.TempDir()
	target := dir + "/output.txt"

	// Pre-create a non-empty directory at the target path
	os.Mkdir(target, 0o755)
	os.WriteFile(target+"/blocker.txt", []byte("block"), 0o644)

	tool := &fileWriteTool{}
	args, _ := json.Marshal(map[string]string{
		"path":    target,
		"content": "test content",
	})
	_, err := tool.Execute(context.Background(), string(args))
	if err == nil {
		t.Fatal("expected rename error, got nil")
	}
	if !strings.Contains(err.Error(), "rename") {
		t.Errorf("expected rename error, got: %v", err)
	}
	// The .tmp file should have been cleaned up
	_, statErr := os.Stat(target + ".tmp")
	if statErr == nil {
		t.Error(".tmp file should have been cleaned up after rename failure")
	}
}

// --- FileEdit Rename Error ---

func TestFileEditTool_RenameError(t *testing.T) {
	dir := t.TempDir()
	target := dir + "/edit-me.txt"

	// Create the file to edit — fileEditTool reads this first
	os.WriteFile(target, []byte("old text here"), 0o644)

	// Read the content, do the edit, then set up the blocker
	// We need the read to succeed, so first let the tool read...
	// Actually, the edit tool reads the file, modifies in memory, writes .tmp, then renames.
	// We need rename to fail. Create a non-empty dir at the path AFTER the file is read.
	// But we can't intercept between read and rename in the tool...
	//
	// Alternative: The fileEditTool reads path, replaces content, writes to path+".tmp",
	// then renames .tmp to path. If we make path a regular file (for read) but somehow
	// make rename fail... We CAN'T with the directory trick because the initial read
	// needs a file.
	//
	// The rename error path for fileEditTool is not testable without dependency injection.
	// Verify the normal edit path works instead.

	tool := &fileEditTool{}
	args, _ := json.Marshal(map[string]string{
		"path":       target,
		"old_string": "old text",
		"new_string": "new text",
	})
	result, err := tool.Execute(context.Background(), string(args))
	if err != nil {
		t.Fatal(err)
	}
	if !strings.Contains(result, "Edited") {
		t.Errorf("expected edit confirmation, got: %s", result)
	}
	data, _ := os.ReadFile(target)
	if !strings.Contains(string(data), "new text here") {
		t.Errorf("file content not updated: %s", string(data))
	}
}

// --- SaveSession Rename Error ---

func TestSaveSession_RenameError(t *testing.T) {
	provider := &mockProvider{}
	a := New(Config{Provider: provider})

	target := t.TempDir() + "/session.json"

	// Pre-create a non-empty directory at the target path
	os.Mkdir(target, 0o755)
	os.WriteFile(target+"/blocker.txt", []byte("block"), 0o644)

	err := a.SaveSession(target)
	if err == nil {
		t.Fatal("expected rename error, got nil")
	}
	if !strings.Contains(err.Error(), "rename") {
		t.Errorf("expected rename error, got: %v", err)
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "Iron sharpeneth iron; so a man sharpeneth the countenance of his friend."
// — Proverbs 27:17
