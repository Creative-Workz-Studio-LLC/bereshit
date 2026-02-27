package agent

import (
	"os"
	"path/filepath"
	"strings"
	"testing"
)

// --- NewConversation ---

func TestNewConversation(t *testing.T) {
	c := NewConversation()
	if c.Meta.TotalExchanges != 0 {
		t.Errorf("exchanges = %d", c.Meta.TotalExchanges)
	}
	if c.Meta.CreatedAt.IsZero() {
		t.Error("created_at should be set")
	}
	if c.Thread.ActiveSince.IsZero() {
		t.Error("active_since should be set")
	}
}

// --- Thread State ---

func TestConversation_SetTopic(t *testing.T) {
	c := NewConversation()
	c.SetTopic("building the SDK")
	if c.Thread.Topic != "building the SDK" {
		t.Errorf("topic = %q", c.Thread.Topic)
	}
}

func TestConversation_AddTag(t *testing.T) {
	c := NewConversation()
	c.AddTag("go")
	c.AddTag("agent")
	c.AddTag("go") // duplicate

	if len(c.Thread.Tags) != 2 {
		t.Errorf("expected 2 tags (no duplicate), got %d", len(c.Thread.Tags))
	}
}

func TestConversation_RemoveTag(t *testing.T) {
	c := NewConversation()
	c.AddTag("go")
	c.AddTag("agent")

	if !c.RemoveTag("go") {
		t.Error("remove should return true")
	}
	if len(c.Thread.Tags) != 1 {
		t.Errorf("expected 1 tag, got %d", len(c.Thread.Tags))
	}
	if c.RemoveTag("nonexistent") {
		t.Error("remove nonexistent should return false")
	}
}

// --- Summaries ---

func TestConversation_AddSummary(t *testing.T) {
	c := NewConversation()
	c.AddSummary(ExchangeSummary{
		Summary:  "Discussed architecture",
		Exchange: "1-3",
	})

	if len(c.Summaries) != 1 {
		t.Fatalf("expected 1 summary, got %d", len(c.Summaries))
	}
	if c.Summaries[0].Summary != "Discussed architecture" {
		t.Errorf("summary = %q", c.Summaries[0].Summary)
	}
	if c.Summaries[0].Timestamp.IsZero() {
		t.Error("timestamp should be auto-set")
	}
}

// --- Key Facts ---

func TestConversation_AddKeyFact(t *testing.T) {
	c := NewConversation()
	c.AddKeyFact("Project uses Go 1.24", "user stated")

	if len(c.KeyFacts) != 1 {
		t.Fatalf("expected 1 fact, got %d", len(c.KeyFacts))
	}
	if c.KeyFacts[0].Fact != "Project uses Go 1.24" {
		t.Errorf("fact = %q", c.KeyFacts[0].Fact)
	}
	if c.KeyFacts[0].Source != "user stated" {
		t.Errorf("source = %q", c.KeyFacts[0].Source)
	}
}

func TestConversation_RemoveKeyFact(t *testing.T) {
	c := NewConversation()
	c.AddKeyFact("Go 1.24", "")
	c.AddKeyFact("Ubuntu 25.10", "")

	if !c.RemoveKeyFact("Go 1.24") {
		t.Error("remove should return true")
	}
	if len(c.KeyFacts) != 1 {
		t.Errorf("expected 1 fact, got %d", len(c.KeyFacts))
	}
	if c.RemoveKeyFact("nonexistent") {
		t.Error("remove nonexistent should return false")
	}
}

// --- Metadata ---

func TestConversation_RecordExchange(t *testing.T) {
	c := NewConversation()
	c.RecordExchange(100, 50)
	c.RecordExchange(200, 100)

	if c.Meta.TotalExchanges != 2 {
		t.Errorf("exchanges = %d", c.Meta.TotalExchanges)
	}
	if c.Meta.TotalInputTokens != 300 {
		t.Errorf("input tokens = %d", c.Meta.TotalInputTokens)
	}
	if c.Meta.TotalOutputTokens != 150 {
		t.Errorf("output tokens = %d", c.Meta.TotalOutputTokens)
	}
}

func TestConversation_NewSession(t *testing.T) {
	c := NewConversation()
	c.NewSession()
	c.NewSession()

	if c.Meta.TotalSessions != 2 {
		t.Errorf("sessions = %d", c.Meta.TotalSessions)
	}
}

// --- BuildContext ---

func TestConversation_BuildContext_Empty(t *testing.T) {
	c := NewConversation()
	ctx := c.BuildContext()
	if ctx != "" {
		t.Errorf("empty conversation should produce empty context, got %q", ctx)
	}
}

func TestConversation_BuildContext_TopicAndTags(t *testing.T) {
	c := NewConversation()
	c.SetTopic("CPI-SI SDK")
	c.AddTag("go")
	c.AddTag("agent")

	ctx := c.BuildContext()
	if !strings.Contains(ctx, "CPI-SI SDK") {
		t.Error("should contain topic")
	}
	if !strings.Contains(ctx, "go, agent") {
		t.Error("should contain tags")
	}
}

func TestConversation_BuildContext_Summaries(t *testing.T) {
	c := NewConversation()
	c.AddSummary(ExchangeSummary{
		Summary:   "Built agent core",
		Exchange:  "1",
		KeyPoints: []string{"created agent.go"},
	})

	ctx := c.BuildContext()
	if !strings.Contains(ctx, "Built agent core") {
		t.Error("should contain summary")
	}
	if !strings.Contains(ctx, "created agent.go") {
		t.Error("should contain key points")
	}
}

func TestConversation_BuildContext_KeyFacts(t *testing.T) {
	c := NewConversation()
	c.AddKeyFact("Go 1.24 is the language version", "")

	ctx := c.BuildContext()
	if !strings.Contains(ctx, "Go 1.24") {
		t.Error("should contain key fact")
	}
}

func TestConversation_BuildContext_Stats(t *testing.T) {
	c := NewConversation()
	c.RecordExchange(100, 50)
	c.NewSession()
	c.NewSession()

	ctx := c.BuildContext()
	if !strings.Contains(ctx, "1 exchanges across 2 sessions") {
		t.Errorf("should contain stats: %q", ctx)
	}
}

func TestConversation_BuildContext_SummaryLimit(t *testing.T) {
	c := NewConversation()
	for i := 0; i < 15; i++ {
		c.AddSummary(ExchangeSummary{
			Summary:  "Exchange " + string(rune('A'+i)),
			Exchange: string(rune('0' + i)),
		})
	}

	ctx := c.BuildContext()
	// Should only show last 10
	if strings.Contains(ctx, "Exchange A") {
		t.Error("should not contain old summaries (A is #0, beyond last 10)")
	}
	if !strings.Contains(ctx, "Exchange O") {
		t.Error("should contain recent summaries")
	}
}

// --- SummarizeHistory ---

func TestSummarizeHistory(t *testing.T) {
	msgs := []Message{
		{Role: RoleUser, Content: "How do I read a file?"},
		{Role: RoleAssistant, Content: "Use the read_file tool.", ToolCalls: []ToolCall{
			{Name: "read_file", Arguments: `{"path":"test.go"}`},
		}},
		{Role: RoleTool, Content: "file contents here", ToolCallID: "c1"},
		{Role: RoleAssistant, Content: "The file contains..."},
	}

	summary := SummarizeHistory(msgs, "1")
	if summary.Exchange != "1" {
		t.Errorf("exchange = %q", summary.Exchange)
	}
	if !strings.Contains(summary.Summary, "How do I read a file") {
		t.Errorf("summary = %q", summary.Summary)
	}
	if len(summary.ToolsUsed) != 1 || summary.ToolsUsed[0] != "read_file" {
		t.Errorf("tools_used = %v", summary.ToolsUsed)
	}
}

func TestSummarizeHistory_LongContent(t *testing.T) {
	longContent := strings.Repeat("x", 500)
	msgs := []Message{
		{Role: RoleUser, Content: longContent},
		{Role: RoleAssistant, Content: longContent},
	}

	summary := SummarizeHistory(msgs, "1")
	if len(summary.Summary) > 220 {
		t.Errorf("summary should be truncated, len = %d", len(summary.Summary))
	}
}

func TestSummarizeHistory_Empty(t *testing.T) {
	summary := SummarizeHistory(nil, "0")
	if summary.Summary != "" {
		t.Errorf("empty history should produce empty summary: %q", summary.Summary)
	}
}

// --- Persistence ---

func TestSaveLoadConversation(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "conv.json")

	original := NewConversation()
	original.SetTopic("SDK Development")
	original.AddTag("go")
	original.AddKeyFact("Go 1.24", "user")
	original.AddSummary(ExchangeSummary{Summary: "Built core", Exchange: "1"})
	original.RecordExchange(100, 50)
	original.NewSession()

	if err := SaveConversation(path, original); err != nil {
		t.Fatal(err)
	}

	loaded, err := LoadConversation(path)
	if err != nil {
		t.Fatal(err)
	}

	if loaded.Thread.Topic != "SDK Development" {
		t.Errorf("topic = %q", loaded.Thread.Topic)
	}
	if len(loaded.Thread.Tags) != 1 || loaded.Thread.Tags[0] != "go" {
		t.Errorf("tags = %v", loaded.Thread.Tags)
	}
	if len(loaded.KeyFacts) != 1 {
		t.Errorf("facts = %d", len(loaded.KeyFacts))
	}
	if len(loaded.Summaries) != 1 {
		t.Errorf("summaries = %d", len(loaded.Summaries))
	}
	if loaded.Meta.TotalExchanges != 1 {
		t.Errorf("exchanges = %d", loaded.Meta.TotalExchanges)
	}
	if loaded.Meta.TotalSessions != 1 {
		t.Errorf("sessions = %d", loaded.Meta.TotalSessions)
	}
}

func TestSaveConversation_CreatesDir(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "nested", "deep", "conv.json")

	err := SaveConversation(path, NewConversation())
	if err != nil {
		t.Fatal(err)
	}

	if _, err := os.Stat(path); os.IsNotExist(err) {
		t.Error("file should exist")
	}
}

func TestLoadConversation_NotFound(t *testing.T) {
	_, err := LoadConversation("/nonexistent/conv.json")
	if err == nil {
		t.Error("expected error for missing file")
	}
}

func TestLoadConversation_InvalidJSON(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "bad.json")
	os.WriteFile(path, []byte("{invalid}"), 0o644)

	_, err := LoadConversation(path)
	if err == nil {
		t.Error("expected error for invalid JSON")
	}
}
