// ============================================================================
// METADATA - CPI-SI Session Persistence Tests
// ============================================================================
//
// Key: pkg-agent-session-test
// Purpose: Verify session save/load — the agent's memory works correctly.
// Biblical: Deuteronomy 8:2 — "Thou shalt remember"
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
	"os"
	"path/filepath"
	"strings"
	"testing"
)

var testCtx = context.Background()

// ============================================================================
// BODY
// ============================================================================

func TestSaveSession(t *testing.T) {
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "Response 1"},
		},
	}

	a := New(Config{Provider: p, System: "test system"})
	a.Run(testCtx, "Hello")

	path := filepath.Join(t.TempDir(), "session.json")
	err := a.SaveSession(path)
	if err != nil {
		t.Fatalf("SaveSession: %v", err)
	}

	// File should exist
	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatalf("read saved session: %v", err)
	}

	// Should be valid JSON with expected fields
	content := string(data)
	if !strings.Contains(content, "Hello") {
		t.Error("saved session should contain user message")
	}
	if !strings.Contains(content, "Response 1") {
		t.Error("saved session should contain assistant response")
	}
	if !strings.Contains(content, `"provider": "mock"`) {
		t.Error("saved session should contain provider name")
	}
	if !strings.Contains(content, `"exchanges": 1`) {
		t.Error("saved session should contain exchange count")
	}
}

func TestSaveSession_CreatesDirectories(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})
	path := filepath.Join(t.TempDir(), "sub", "dir", "session.json")

	err := a.SaveSession(path)
	if err != nil {
		t.Fatalf("SaveSession: %v", err)
	}

	if _, err := os.Stat(path); os.IsNotExist(err) {
		t.Error("session file should exist")
	}
}

func TestLoadSession(t *testing.T) {
	// Create an agent, run an exchange, save
	p1 := &mockProvider{
		responses: []*ChatResponse{
			{Content: "First response"},
			{Content: "Second response"},
		},
	}
	a1 := New(Config{Provider: p1, System: "identity"})
	a1.Run(testCtx, "First input")
	a1.Run(testCtx, "Second input")

	path := filepath.Join(t.TempDir(), "session.json")
	if err := a1.SaveSession(path); err != nil {
		t.Fatalf("SaveSession: %v", err)
	}

	// Create a NEW agent and load the session
	a2 := New(Config{Provider: &mockProvider{}, System: "identity"})
	if err := a2.LoadSession(path); err != nil {
		t.Fatalf("LoadSession: %v", err)
	}

	// Verify state was restored
	if len(a2.History()) != 4 {
		t.Errorf("history length = %d, want 4 (2 exchanges)", len(a2.History()))
	}
	if a2.Exchanges() != 2 {
		t.Errorf("exchanges = %d, want 2", a2.Exchanges())
	}
	if a2.History()[0].Content != "First input" {
		t.Errorf("first message = %q, want %q", a2.History()[0].Content, "First input")
	}
	if a2.History()[1].Content != "First response" {
		t.Errorf("second message = %q, want %q", a2.History()[1].Content, "First response")
	}
}

func TestLoadSession_ReindexesHistory(t *testing.T) {
	// Save a session with enough history to create index segments
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "R1"}, {Content: "R2"}, {Content: "R3"},
			{Content: "R4"}, {Content: "R5"}, {Content: "R6"},
		},
	}
	a1 := New(Config{Provider: p})
	for i := 0; i < 6; i++ {
		a1.Run(testCtx, "Input")
	}

	path := filepath.Join(t.TempDir(), "session.json")
	a1.SaveSession(path)

	// Load into new agent
	a2 := New(Config{Provider: &mockProvider{}})
	a2.LoadSession(path)

	// Index should have history segments
	if !a2.Index().Has("history:recent") {
		t.Error("loaded session should have history:recent in index")
	}
}

func TestLoadSession_RestoresContext(t *testing.T) {
	// Create a context file
	dir := t.TempDir()
	ctxFile := filepath.Join(dir, "context.md")
	os.WriteFile(ctxFile, []byte("# Context"), 0o644)

	// Create agent with context loaded
	a1 := New(Config{Provider: &mockProvider{}})
	a1.LoadContext(ctxFile)

	// Save
	path := filepath.Join(dir, "session.json")
	a1.SaveSession(path)

	// Load into new agent
	a2 := New(Config{Provider: &mockProvider{}})
	a2.LoadSession(path)

	// Context should be re-loaded
	key := "context:" + ctxFile
	if !a2.Index().Has(key) {
		t.Error("loaded session should restore context segments")
	}
}

func TestLoadSession_MissingContextFile(t *testing.T) {
	// Save with context
	dir := t.TempDir()
	ctxFile := filepath.Join(dir, "context.md")
	os.WriteFile(ctxFile, []byte("# Context"), 0o644)

	a1 := New(Config{Provider: &mockProvider{}})
	a1.LoadContext(ctxFile)

	path := filepath.Join(dir, "session.json")
	a1.SaveSession(path)

	// Delete the context file
	os.Remove(ctxFile)

	// Load should NOT fail — context re-loading is best-effort
	a2 := New(Config{Provider: &mockProvider{}})
	err := a2.LoadSession(path)
	if err != nil {
		t.Fatalf("LoadSession should succeed even with missing context: %v", err)
	}
}

func TestLoadSession_NotFound(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})
	err := a.LoadSession("/nonexistent/session.json")

	if err == nil {
		t.Fatal("expected error for nonexistent session file")
	}
}

func TestLoadSession_InvalidJSON(t *testing.T) {
	path := filepath.Join(t.TempDir(), "bad.json")
	os.WriteFile(path, []byte("not json"), 0o644)

	a := New(Config{Provider: &mockProvider{}})
	err := a.LoadSession(path)

	if err == nil {
		t.Fatal("expected error for invalid JSON")
	}
}

func TestSessionInfo(t *testing.T) {
	p := &mockProvider{
		responses: []*ChatResponse{
			{Content: "R1"},
			{Content: "R2"},
		},
	}
	a := New(Config{Provider: p, System: "test"})
	a.Run(testCtx, "Hello")
	a.Run(testCtx, "World")

	info := a.SessionInfo()

	if info.Exchanges != 2 {
		t.Errorf("Exchanges = %d, want 2", info.Exchanges)
	}
	if info.Messages != 4 {
		t.Errorf("Messages = %d, want 4", info.Messages)
	}
	if info.IndexTokens <= 0 {
		t.Error("IndexTokens should be > 0 (at least identity segment)")
	}
}

func TestSaveLoad_RoundTrip(t *testing.T) {
	// Full round-trip test: create state → save → load → verify identical
	tool := &mockTool{
		def:    ToolDef{Name: "greet", Description: "Greet someone"},
		result: "Greeted!",
	}

	p := &mockProvider{
		responses: []*ChatResponse{
			{
				ToolCalls: []ToolCall{{ID: "c1", Name: "greet", Arguments: `{"name":"Nova"}`}},
			},
			{Content: "Done greeting Nova."},
		},
	}

	a1 := New(Config{Provider: p, Tools: []Tool{tool}, System: "Be friendly"})
	a1.Run(testCtx, "Greet Nova")

	path := filepath.Join(t.TempDir(), "roundtrip.json")
	if err := a1.SaveSession(path); err != nil {
		t.Fatalf("SaveSession: %v", err)
	}

	// Load into a completely new agent
	a2 := New(Config{Provider: &mockProvider{}, Tools: []Tool{tool}, System: "Be friendly"})
	if err := a2.LoadSession(path); err != nil {
		t.Fatalf("LoadSession: %v", err)
	}

	// Verify full state match
	if len(a1.History()) != len(a2.History()) {
		t.Fatalf("history length mismatch: %d vs %d", len(a1.History()), len(a2.History()))
	}
	for i, m1 := range a1.History() {
		m2 := a2.History()[i]
		if m1.Role != m2.Role {
			t.Errorf("message[%d] role: %s vs %s", i, m1.Role, m2.Role)
		}
		if m1.Content != m2.Content {
			t.Errorf("message[%d] content: %q vs %q", i, m1.Content, m2.Content)
		}
		if len(m1.ToolCalls) != len(m2.ToolCalls) {
			t.Errorf("message[%d] tool calls: %d vs %d", i, len(m1.ToolCalls), len(m2.ToolCalls))
		}
	}
	if a1.Exchanges() != a2.Exchanges() {
		t.Errorf("exchanges: %d vs %d", a1.Exchanges(), a2.Exchanges())
	}
}

func TestSaveSession_EmptyHistory(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})
	path := filepath.Join(t.TempDir(), "empty.json")

	err := a.SaveSession(path)
	if err != nil {
		t.Fatalf("SaveSession with empty history: %v", err)
	}

	// Should still produce valid JSON
	a2 := New(Config{Provider: &mockProvider{}})
	err = a2.LoadSession(path)
	if err != nil {
		t.Fatalf("LoadSession from empty: %v", err)
	}
	if len(a2.History()) != 0 {
		t.Errorf("loaded empty session should have no history, got %d", len(a2.History()))
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "Remember" — Deuteronomy 8:2
