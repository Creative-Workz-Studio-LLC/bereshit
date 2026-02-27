// ============================================================================
// METADATA - CPI-SI Model Router Tests
// ============================================================================
//
// Key: pkg-agent-router-test
// Purpose: Verify model routing — the agent picks the right provider.
// Biblical: Proverbs 16:9 — "The LORD directeth his steps"
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
	"strings"
	"testing"
)

// ============================================================================
// BODY
// ============================================================================

func TestRouter_FallbackOnly(t *testing.T) {
	fallback := &mockProvider{
		responses: []*ChatResponse{{Content: "fallback response"}},
	}

	router := NewRouter(nil, fallback)
	resp, err := router.Chat(context.Background(), ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "Hello"}},
	})

	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "fallback response" {
		t.Errorf("got %q, want fallback response", resp.Content)
	}
	if router.Name() != "router" {
		t.Errorf("Name() = %q, want 'router'", router.Name())
	}
}

func TestRouter_FirstMatchWins(t *testing.T) {
	fast := &mockProvider{
		responses: []*ChatResponse{{Content: "fast"}},
	}
	slow := &mockProvider{
		responses: []*ChatResponse{{Content: "slow"}},
	}
	fallback := &mockProvider{
		responses: []*ChatResponse{{Content: "fallback"}},
	}

	router := NewRouter([]Route{
		{Name: "fast", Match: MatchShortInput(50), Provider: fast},
		{Name: "slow", Match: nil, Provider: slow}, // always matches
	}, fallback)

	resp, err := router.Chat(context.Background(), ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "Hi"}},
	})

	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "fast" {
		t.Errorf("got %q, want 'fast' (short input should match first)", resp.Content)
	}
	if router.LastRoute() != "fast:mock-v1" {
		t.Errorf("LastRoute() = %q", router.LastRoute())
	}
}

func TestRouter_SkipsNonMatching(t *testing.T) {
	expensive := &mockProvider{
		responses: []*ChatResponse{{Content: "expensive"}},
	}
	fallback := &mockProvider{
		responses: []*ChatResponse{{Content: "fallback"}},
	}

	router := NewRouter([]Route{
		{
			Name:     "expensive",
			Match:    MatchKeywords("complex", "analyze"),
			Provider: expensive,
		},
	}, fallback)

	// Simple request — shouldn't match keywords
	resp, err := router.Chat(context.Background(), ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "Hello"}},
	})

	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "fallback" {
		t.Errorf("got %q, want 'fallback'", resp.Content)
	}
}

func TestRouter_Model_BeforeChat(t *testing.T) {
	fallback := &mockProvider{}
	router := NewRouter(nil, fallback)

	// Before any Chat call, Model should return fallback
	if router.Model() != "mock-v1" {
		t.Errorf("Model() = %q, want 'mock-v1'", router.Model())
	}
}

func TestRouter_Model_AfterChat(t *testing.T) {
	fallback := &mockProvider{
		responses: []*ChatResponse{{Content: "ok"}},
	}
	router := NewRouter(nil, fallback)

	router.Chat(context.Background(), ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "Hi"}},
	})

	// After Chat, Model should return lastUsed
	model := router.Model()
	if !strings.Contains(model, "mock-v1") {
		t.Errorf("Model() = %q, should contain 'mock-v1'", model)
	}
}

// --- Matcher Tests ---

func TestMatchToolCalls(t *testing.T) {
	withTools := ChatRequest{
		Tools: []ToolDef{{Name: "test"}},
	}
	noTools := ChatRequest{}

	if !MatchToolCalls(withTools) {
		t.Error("MatchToolCalls should match when tools present")
	}
	if MatchToolCalls(noTools) {
		t.Error("MatchToolCalls should not match when no tools")
	}
}

func TestMatchNoTools(t *testing.T) {
	withTools := ChatRequest{
		Tools: []ToolDef{{Name: "test"}},
	}
	noTools := ChatRequest{}

	if MatchNoTools(withTools) {
		t.Error("MatchNoTools should not match when tools present")
	}
	if !MatchNoTools(noTools) {
		t.Error("MatchNoTools should match when no tools")
	}
}

func TestMatchLongContext(t *testing.T) {
	matcher := MatchLongContext(100)

	short := ChatRequest{
		Messages: []Message{{Content: "Hi"}},
	}
	long := ChatRequest{
		Messages: []Message{{Content: string(make([]byte, 200))}},
	}

	if matcher(short) {
		t.Error("should not match short context")
	}
	if !matcher(long) {
		t.Error("should match long context")
	}
}

func TestMatchShortInput(t *testing.T) {
	matcher := MatchShortInput(50)

	short := ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "Hello"}},
	}
	long := ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: string(make([]byte, 100))}},
	}
	empty := ChatRequest{}
	nonUser := ChatRequest{
		Messages: []Message{{Role: RoleAssistant, Content: "Hi"}},
	}

	if !matcher(short) {
		t.Error("should match short user input")
	}
	if matcher(long) {
		t.Error("should not match long user input")
	}
	if matcher(empty) {
		t.Error("should not match empty messages")
	}
	if matcher(nonUser) {
		t.Error("should not match non-user message")
	}
}

func TestMatchKeywords(t *testing.T) {
	matcher := MatchKeywords("analyze", "complex", "debug")

	match := ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "Please analyze this code"}},
	}
	noMatch := ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "Hello world"}},
	}
	caseInsensitive := ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "ANALYZE this"}},
	}
	empty := ChatRequest{}
	nonUser := ChatRequest{
		Messages: []Message{{Role: RoleAssistant, Content: "analyze"}},
	}

	if !matcher(match) {
		t.Error("should match keyword 'analyze'")
	}
	if matcher(noMatch) {
		t.Error("should not match 'Hello world'")
	}
	if !matcher(caseInsensitive) {
		t.Error("should match case-insensitively")
	}
	if matcher(empty) {
		t.Error("should not match empty messages")
	}
	if matcher(nonUser) {
		t.Error("should not match non-user messages")
	}
}

func TestRouter_NilMatchAlwaysMatches(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{{Content: "catch-all"}},
	}
	fallback := &mockProvider{
		responses: []*ChatResponse{{Content: "fallback"}},
	}

	router := NewRouter([]Route{
		{Name: "catch-all", Match: nil, Provider: provider},
	}, fallback)

	resp, err := router.Chat(context.Background(), ChatRequest{
		Messages: []Message{{Role: RoleUser, Content: "anything"}},
	})

	if err != nil {
		t.Fatal(err)
	}
	if resp.Content != "catch-all" {
		t.Errorf("got %q, want 'catch-all'", resp.Content)
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "The LORD directeth his steps." — Proverbs 16:9
