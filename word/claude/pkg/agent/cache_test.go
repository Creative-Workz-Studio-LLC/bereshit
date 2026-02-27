// ============================================================================
// METADATA - CPI-SI Prompt Index Tests
// ============================================================================
//
// Key: pkg-agent-cache-test
// Purpose: Verify index-based context management — semantic segments, not
//          token counting. Tests the CPI-SI context model and its legacy bridge.
// Biblical: Proverbs 25:2 — "It is the glory of God to conceal a thing:
//           but the honour of kings is to search out a matter"
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
	"strings"
	"testing"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// --- PromptIndex Core ---

func TestNewPromptIndex(t *testing.T) {
	idx := NewPromptIndex()

	if idx == nil {
		t.Fatal("NewPromptIndex returned nil")
	}
	if len(idx.Loaded()) != 0 {
		t.Error("new index should have no segments")
	}
	if idx.TotalEstimatedTokens() != 0 {
		t.Error("new index should have 0 tokens")
	}
}

func TestLoad_NewSegment(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("identity", SegmentIdentity, PriorityIdentity, "I am Nova Dawn.")

	if !idx.Has("identity") {
		t.Fatal("segment should be loaded")
	}

	seg := idx.Get("identity")
	if seg == nil {
		t.Fatal("Get returned nil for loaded segment")
	}
	if seg.Key != "identity" {
		t.Errorf("key = %q, want 'identity'", seg.Key)
	}
	if seg.Kind != SegmentIdentity {
		t.Errorf("kind = %d, want SegmentIdentity", seg.Kind)
	}
	if seg.Priority != PriorityIdentity {
		t.Errorf("priority = %d, want %d", seg.Priority, PriorityIdentity)
	}
	if seg.Content != "I am Nova Dawn." {
		t.Errorf("content = %q", seg.Content)
	}
	if seg.Hash == "" {
		t.Error("hash should not be empty")
	}
	if seg.EstimatedTokens == 0 {
		t.Error("token estimate should be > 0")
	}
	if seg.CreatedAt.IsZero() {
		t.Error("CreatedAt should be set")
	}
	if seg.LastUsed.IsZero() {
		t.Error("LastUsed should be set")
	}
}

func TestLoad_UnchangedContent(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("identity", SegmentIdentity, PriorityIdentity, "I am Nova Dawn.")

	seg := idx.Get("identity")
	originalCreated := seg.CreatedAt
	originalHash := seg.Hash

	// Wait a tiny bit so LastUsed changes
	time.Sleep(time.Millisecond)

	// Load same content again
	idx.Load("identity", SegmentIdentity, PriorityIdentity, "I am Nova Dawn.")

	seg = idx.Get("identity")
	if seg.CreatedAt != originalCreated {
		t.Error("CreatedAt should not change for unchanged content")
	}
	if seg.Hash != originalHash {
		t.Error("hash should not change for unchanged content")
	}
	if !seg.LastUsed.After(originalCreated) {
		t.Error("LastUsed should be updated")
	}
}

func TestLoad_ChangedContent(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("identity", SegmentIdentity, PriorityIdentity, "I am Nova Dawn.")

	seg := idx.Get("identity")
	originalHash := seg.Hash

	// Mark as cached
	idx.MarkCached("identity", "cache-123", time.Now().Add(time.Hour))
	if !idx.Get("identity").Cached {
		t.Fatal("should be marked cached")
	}

	// Change content
	idx.Load("identity", SegmentIdentity, PriorityIdentity, "I am Nova Dawn v2.")

	seg = idx.Get("identity")
	if seg.Hash == originalHash {
		t.Error("hash should change for new content")
	}
	if seg.Content != "I am Nova Dawn v2." {
		t.Errorf("content = %q", seg.Content)
	}
	if seg.Cached {
		t.Error("cache should be invalidated when content changes")
	}
	if seg.CacheKey != "" {
		t.Error("cache key should be cleared when content changes")
	}
}

func TestUnload(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("identity", SegmentIdentity, PriorityIdentity, "content")
	idx.Load("tools", SegmentTools, PriorityTools, "tool defs")

	if !idx.Has("identity") {
		t.Fatal("identity should be loaded")
	}

	idx.Unload("identity")

	if idx.Has("identity") {
		t.Error("identity should be unloaded")
	}
	if !idx.Has("tools") {
		t.Error("tools should still be loaded")
	}

	keys := idx.Loaded()
	if len(keys) != 1 || keys[0] != "tools" {
		t.Errorf("Loaded() = %v, want [tools]", keys)
	}
}

func TestUnload_Nonexistent(t *testing.T) {
	idx := NewPromptIndex()
	// Should not panic
	idx.Unload("nonexistent")
}

func TestGet_Nonexistent(t *testing.T) {
	idx := NewPromptIndex()
	if idx.Get("nope") != nil {
		t.Error("Get should return nil for nonexistent key")
	}
}

func TestHas(t *testing.T) {
	idx := NewPromptIndex()

	if idx.Has("x") {
		t.Error("empty index should not have any key")
	}

	idx.Load("x", SegmentEphemeral, PriorityEphemeral, "data")
	if !idx.Has("x") {
		t.Error("should have 'x' after Load")
	}
}

// --- Token Estimation (Legacy Bridge) ---

func TestEstimateTokensDefault(t *testing.T) {
	tests := []struct {
		content  string
		minTokens int
		maxTokens int
	}{
		{"", 0, 0},
		{"Hi", 1, 1},          // 2 chars → 1 token
		{"Hello World!", 2, 4}, // 12 chars → ~3 tokens
		{strings.Repeat("x", 400), 90, 110}, // 400 chars → ~100 tokens
	}

	for _, tt := range tests {
		tokens := estimateTokensDefault(tt.content)
		if tokens < tt.minTokens || tokens > tt.maxTokens {
			t.Errorf("estimateTokens(%d chars) = %d, want [%d, %d]",
				len(tt.content), tokens, tt.minTokens, tt.maxTokens)
		}
	}
}

func TestTotalEstimatedTokens(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("a", SegmentIdentity, PriorityIdentity, strings.Repeat("x", 400))
	idx.Load("b", SegmentTools, PriorityTools, strings.Repeat("y", 200))

	total := idx.TotalEstimatedTokens()
	// 400/4 + 200/4 = 100 + 50 = 150
	if total != 150 {
		t.Errorf("total = %d, want 150", total)
	}
}

func TestSetTokenEstimator(t *testing.T) {
	idx := NewPromptIndex()
	// Custom estimator: 1 token per character
	idx.SetTokenEstimator(func(s string) int { return len(s) })

	idx.Load("test", SegmentEphemeral, PriorityEphemeral, "Hello")

	seg := idx.Get("test")
	if seg.EstimatedTokens != 5 {
		t.Errorf("tokens = %d, want 5 (1 per char)", seg.EstimatedTokens)
	}
}

// --- Caching ---

func TestMarkCached(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("identity", SegmentIdentity, PriorityIdentity, "content")

	expiry := time.Now().Add(time.Hour)
	idx.MarkCached("identity", "cache-abc", expiry)

	seg := idx.Get("identity")
	if !seg.Cached {
		t.Error("should be marked cached")
	}
	if seg.CacheKey != "cache-abc" {
		t.Errorf("cache key = %q, want 'cache-abc'", seg.CacheKey)
	}
	if !seg.CacheExpiry.Equal(expiry) {
		t.Error("cache expiry should match")
	}
}

func TestMarkCached_Nonexistent(t *testing.T) {
	idx := NewPromptIndex()
	// Should not panic
	idx.MarkCached("nope", "key", time.Now())
}

func TestCachedTokens(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("a", SegmentIdentity, PriorityIdentity, strings.Repeat("x", 400)) // 100 tokens
	idx.Load("b", SegmentTools, PriorityTools, strings.Repeat("y", 200))        // 50 tokens

	// Nothing cached yet
	if idx.CachedTokens() != 0 {
		t.Errorf("cached = %d, want 0", idx.CachedTokens())
	}

	// Cache segment a
	idx.MarkCached("a", "key-a", time.Now().Add(time.Hour))
	if idx.CachedTokens() != 100 {
		t.Errorf("cached = %d, want 100", idx.CachedTokens())
	}

	// UncachedTokens should be the remainder
	if idx.UncachedTokens() != 50 {
		t.Errorf("uncached = %d, want 50", idx.UncachedTokens())
	}
}

func TestCachedTokens_Expired(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("a", SegmentIdentity, PriorityIdentity, strings.Repeat("x", 400))

	// Cache with expired time
	idx.MarkCached("a", "key-a", time.Now().Add(-time.Hour))

	if idx.CachedTokens() != 0 {
		t.Errorf("expired cache should report 0 cached tokens, got %d", idx.CachedTokens())
	}
}

// --- Snapshot ---

func TestSnapshot(t *testing.T) {
	idx := NewPromptIndex()
	idx.SetMaxTokens(1000)
	idx.Load("identity", SegmentIdentity, PriorityIdentity, strings.Repeat("x", 400))
	idx.Load("tools", SegmentTools, PriorityTools, strings.Repeat("y", 200))
	idx.MarkCached("identity", "c1", time.Now().Add(time.Hour))

	snap := idx.Snapshot()

	if snap.TotalSegments != 2 {
		t.Errorf("total segments = %d, want 2", snap.TotalSegments)
	}
	if snap.EstimatedTokens != 150 {
		t.Errorf("estimated tokens = %d, want 150", snap.EstimatedTokens)
	}
	if snap.CachedTokens != 100 {
		t.Errorf("cached tokens = %d, want 100", snap.CachedTokens)
	}
	if snap.MaxTokens != 1000 {
		t.Errorf("max tokens = %d, want 1000", snap.MaxTokens)
	}
	if len(snap.Segments) != 2 {
		t.Fatalf("segments = %d, want 2", len(snap.Segments))
	}

	// First segment should be identity (insertion order)
	if snap.Segments[0].Key != "identity" {
		t.Errorf("first segment = %q, want 'identity'", snap.Segments[0].Key)
	}
	if !snap.Segments[0].Cached {
		t.Error("identity should be cached in snapshot")
	}
	if snap.Segments[0].ContentHash == "" {
		t.Error("content hash should not be empty")
	}
}

// --- Eviction / Fit ---

func TestFit_NoLimit(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("a", SegmentIdentity, PriorityIdentity, strings.Repeat("x", 400))
	idx.Load("b", SegmentTools, PriorityTools, strings.Repeat("y", 400))
	idx.Load("c", SegmentHistory, PriorityRecent, strings.Repeat("z", 400))

	// No limit — all segments returned
	result := idx.Fit(0)
	if len(result) != 3 {
		t.Fatalf("Fit(0) returned %d segments, want 3", len(result))
	}
}

func TestFit_WithLimit(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("identity", SegmentIdentity, PriorityIdentity, strings.Repeat("x", 400))   // 100 tokens
	idx.Load("tools", SegmentTools, PriorityTools, strings.Repeat("y", 400))             // 100 tokens
	idx.Load("history", SegmentHistory, PriorityRecent, strings.Repeat("z", 400))        // 100 tokens
	idx.Load("ephemeral", SegmentEphemeral, PriorityEphemeral, strings.Repeat("w", 400)) // 100 tokens

	// Limit to 250 tokens — should keep identity (100), tools (90), history (70)
	// and evict ephemeral (10 priority)
	result := idx.Fit(250)
	if len(result) != 2 {
		t.Fatalf("Fit(250) returned %d segments, want 2", len(result))
	}

	keys := make(map[string]bool)
	for _, seg := range result {
		keys[seg.Key] = true
	}
	if !keys["identity"] {
		t.Error("identity should be kept (highest priority)")
	}
	if !keys["tools"] {
		t.Error("tools should be kept (second highest priority)")
	}
}

func TestFit_PriorityOrder(t *testing.T) {
	idx := NewPromptIndex()
	// All same size but different priorities
	idx.Load("low", SegmentEphemeral, 10, strings.Repeat("a", 80))     // 20 tokens
	idx.Load("medium", SegmentContext, 50, strings.Repeat("b", 80))    // 20 tokens
	idx.Load("high", SegmentIdentity, 100, strings.Repeat("c", 80))   // 20 tokens

	// Limit to 40 tokens — only room for 2
	result := idx.Fit(40)
	if len(result) != 2 {
		t.Fatalf("Fit(40) returned %d, want 2", len(result))
	}

	// Should keep high and medium, evict low
	keys := make(map[string]bool)
	for _, seg := range result {
		keys[seg.Key] = true
	}
	if !keys["high"] {
		t.Error("high priority should be kept")
	}
	if !keys["medium"] {
		t.Error("medium priority should be kept")
	}
	if keys["low"] {
		t.Error("low priority should be evicted")
	}
}

func TestFit_PreservesInsertionOrder(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("b_identity", SegmentIdentity, PriorityIdentity, strings.Repeat("x", 40))
	idx.Load("a_tools", SegmentTools, PriorityTools, strings.Repeat("y", 40))
	idx.Load("c_history", SegmentHistory, PriorityRecent, strings.Repeat("z", 40))

	result := idx.Fit(0)
	if len(result) != 3 {
		t.Fatalf("got %d, want 3", len(result))
	}

	// Should be in insertion order, not sorted by key or priority
	if result[0].Key != "b_identity" || result[1].Key != "a_tools" || result[2].Key != "c_history" {
		t.Errorf("order = [%s, %s, %s], want [b_identity, a_tools, c_history]",
			result[0].Key, result[1].Key, result[2].Key)
	}
}

func TestFit_UsesIndexMaxTokens(t *testing.T) {
	idx := NewPromptIndex()
	idx.SetMaxTokens(25) // 25 token limit
	idx.Load("a", SegmentIdentity, PriorityIdentity, strings.Repeat("x", 80))   // 20 tokens
	idx.Load("b", SegmentEphemeral, PriorityEphemeral, strings.Repeat("y", 80)) // 20 tokens

	// Pass 0 to use index's maxTokens
	result := idx.Fit(0)
	if len(result) != 1 {
		t.Fatalf("Fit(0) with maxTokens=25 returned %d, want 1", len(result))
	}
	if result[0].Key != "a" {
		t.Errorf("kept %q, want 'a' (higher priority)", result[0].Key)
	}
}

// --- BuildRequest ---

func TestBuildRequest(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("identity", SegmentIdentity, PriorityIdentity, "I am Nova Dawn.")

	msgs := []Message{{Role: RoleUser, Content: "Hello"}}
	tools := []ToolDef{{Name: "test", Description: "test tool"}}

	req := idx.BuildRequest(msgs, tools, 4096)

	if req.System != "I am Nova Dawn." {
		t.Errorf("system = %q, want identity content", req.System)
	}
	if len(req.Messages) != 1 {
		t.Errorf("messages = %d, want 1", len(req.Messages))
	}
	if len(req.Tools) != 1 {
		t.Errorf("tools = %d, want 1", len(req.Tools))
	}
	if req.MaxTokens != 4096 {
		t.Errorf("max tokens = %d, want 4096", req.MaxTokens)
	}
}

func TestBuildRequest_NoIdentity(t *testing.T) {
	idx := NewPromptIndex()
	req := idx.BuildRequest(nil, nil, 1024)

	if req.System != "" {
		t.Errorf("system should be empty without identity, got %q", req.System)
	}
}

// --- Convenience Loaders ---

func TestLoadIdentitySegment(t *testing.T) {
	idx := NewPromptIndex()
	idx.LoadIdentitySegment("I am Nova Dawn.")

	seg := idx.Get("identity")
	if seg == nil {
		t.Fatal("identity segment not loaded")
	}
	if seg.Kind != SegmentIdentity {
		t.Errorf("kind = %d, want SegmentIdentity", seg.Kind)
	}
	if seg.Priority != PriorityIdentity {
		t.Errorf("priority = %d, want %d", seg.Priority, PriorityIdentity)
	}
}

func TestLoadToolsSegment(t *testing.T) {
	idx := NewPromptIndex()
	idx.LoadToolsSegment("tool definitions here")

	seg := idx.Get("tools")
	if seg == nil {
		t.Fatal("tools segment not loaded")
	}
	if seg.Kind != SegmentTools {
		t.Error("wrong kind")
	}
	if seg.Priority != PriorityTools {
		t.Error("wrong priority")
	}
}

func TestLoadHistorySegment(t *testing.T) {
	idx := NewPromptIndex()
	idx.LoadHistorySegment("history:0-10", "old messages", false)
	idx.LoadHistorySegment("history:11-20", "recent messages", true)

	old := idx.Get("history:0-10")
	recent := idx.Get("history:11-20")

	if old == nil || recent == nil {
		t.Fatal("history segments not loaded")
	}
	if old.Priority != PriorityOlder {
		t.Errorf("old priority = %d, want %d", old.Priority, PriorityOlder)
	}
	if recent.Priority != PriorityRecent {
		t.Errorf("recent priority = %d, want %d", recent.Priority, PriorityRecent)
	}
}

func TestLoadContextSegment(t *testing.T) {
	idx := NewPromptIndex()
	idx.LoadContextSegment("context:readme.md", "# README")

	seg := idx.Get("context:readme.md")
	if seg == nil {
		t.Fatal("context segment not loaded")
	}
	if seg.Kind != SegmentContext {
		t.Error("wrong kind")
	}
	if seg.Priority != PriorityContext {
		t.Error("wrong priority")
	}
}

// --- Hash Change Detection ---

func TestHashContent(t *testing.T) {
	h1 := hashContent("Hello")
	h2 := hashContent("Hello")
	h3 := hashContent("World")

	if h1 != h2 {
		t.Error("same content should produce same hash")
	}
	if h1 == h3 {
		t.Error("different content should produce different hash")
	}
	if len(h1) != 64 {
		t.Errorf("SHA-256 hex should be 64 chars, got %d", len(h1))
	}
}

// --- Concurrency Safety ---

func TestConcurrentAccess(t *testing.T) {
	idx := NewPromptIndex()
	done := make(chan bool, 10)

	// Concurrent writers
	for i := 0; i < 5; i++ {
		go func(n int) {
			for j := 0; j < 100; j++ {
				key := strings.Repeat("k", n+1)
				idx.Load(key, SegmentEphemeral, PriorityEphemeral, "data")
			}
			done <- true
		}(i)
	}

	// Concurrent readers
	for i := 0; i < 5; i++ {
		go func() {
			for j := 0; j < 100; j++ {
				idx.Loaded()
				idx.TotalEstimatedTokens()
				idx.Snapshot()
			}
			done <- true
		}()
	}

	for i := 0; i < 10; i++ {
		<-done
	}

	// If we get here without deadlock or panic, concurrency is safe
}

// --- Loaded Order ---

func TestLoaded_InsertionOrder(t *testing.T) {
	idx := NewPromptIndex()
	idx.Load("c", SegmentEphemeral, 1, "c")
	idx.Load("a", SegmentEphemeral, 1, "a")
	idx.Load("b", SegmentEphemeral, 1, "b")

	keys := idx.Loaded()
	if len(keys) != 3 {
		t.Fatalf("got %d keys", len(keys))
	}
	if keys[0] != "c" || keys[1] != "a" || keys[2] != "b" {
		t.Errorf("order = %v, want [c, a, b] (insertion order)", keys)
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "It is the glory of God to conceal a thing: but the honour of kings is
//  to search out a matter." — Proverbs 25:2
