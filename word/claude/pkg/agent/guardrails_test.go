package agent

import (
	"context"
	"os"
	"strings"
	"testing"
)

// --- Guardrails Core ---

func TestGuardrails_New(t *testing.T) {
	g := NewGuardrails()
	if g.Size() != 0 {
		t.Errorf("new guardrails should have 0 guards, got %d", g.Size())
	}
}

func TestGuardrails_AddRemove(t *testing.T) {
	g := NewGuardrails()
	g.Add(Guard{Name: "test", Points: []GuardPoint{GuardOnInput}})

	if g.Size() != 1 {
		t.Fatalf("expected 1 guard, got %d", g.Size())
	}
	if g.Names()[0] != "test" {
		t.Errorf("name = %q", g.Names()[0])
	}

	if !g.Remove("test") {
		t.Error("remove should return true")
	}
	if g.Size() != 0 {
		t.Error("should be empty after remove")
	}

	if g.Remove("nonexistent") {
		t.Error("remove nonexistent should return false")
	}
}

// --- Evaluate ---

func TestGuardrails_Evaluate_NoGuards(t *testing.T) {
	g := NewGuardrails()
	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "hello"})
	if verdict.Action != ActionAllow {
		t.Errorf("expected Allow, got %d", verdict.Action)
	}
}

func TestGuardrails_Evaluate_Allow(t *testing.T) {
	g := NewGuardrails()
	g.Add(Guard{
		Name:   "always_allow",
		Points: []GuardPoint{GuardOnInput},
		Check:  func(_ context.Context, _ GuardPoint, _ *GuardData) *GuardVerdict { return nil },
	})

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "hello"})
	if verdict.Action != ActionAllow {
		t.Errorf("expected Allow, got %d", verdict.Action)
	}
}

func TestGuardrails_Evaluate_Block(t *testing.T) {
	g := NewGuardrails()
	g.Add(Guard{
		Name:   "blocker",
		Points: []GuardPoint{GuardOnInput},
		Check: func(_ context.Context, _ GuardPoint, _ *GuardData) *GuardVerdict {
			return &GuardVerdict{Action: ActionBlock, Reason: "nope"}
		},
	})

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "hello"})
	if verdict.Action != ActionBlock {
		t.Errorf("expected Block, got %d", verdict.Action)
	}
	if verdict.Reason != "nope" {
		t.Errorf("reason = %q", verdict.Reason)
	}
	if verdict.GuardName != "blocker" {
		t.Errorf("guard name = %q", verdict.GuardName)
	}
}

func TestGuardrails_Evaluate_Modify(t *testing.T) {
	g := NewGuardrails()
	g.Add(Guard{
		Name:   "modifier",
		Points: []GuardPoint{GuardOnInput},
		Check: func(_ context.Context, _ GuardPoint, data *GuardData) *GuardVerdict {
			return &GuardVerdict{Action: ActionModify, Replace: strings.ToUpper(data.Content)}
		},
	})

	data := &GuardData{Content: "hello"}
	g.Evaluate(context.Background(), GuardOnInput, data)
	if data.Content != "HELLO" {
		t.Errorf("content should be modified, got %q", data.Content)
	}
}

func TestGuardrails_Evaluate_Warn(t *testing.T) {
	g := NewGuardrails()
	g.Add(Guard{
		Name:   "warner",
		Points: []GuardPoint{GuardOnInput},
		Check: func(_ context.Context, _ GuardPoint, _ *GuardData) *GuardVerdict {
			return &GuardVerdict{Action: ActionWarn, Reason: "heads up"}
		},
	})

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "hello"})
	if verdict.Action != ActionWarn {
		t.Errorf("expected Warn, got %d", verdict.Action)
	}
	if verdict.Reason != "heads up" {
		t.Errorf("reason = %q", verdict.Reason)
	}
}

func TestGuardrails_Evaluate_BlockStopsChain(t *testing.T) {
	g := NewGuardrails()
	secondRan := false

	g.Add(Guard{
		Name:   "blocker",
		Points: []GuardPoint{GuardOnInput},
		Check: func(_ context.Context, _ GuardPoint, _ *GuardData) *GuardVerdict {
			return &GuardVerdict{Action: ActionBlock, Reason: "first blocks"}
		},
	})
	g.Add(Guard{
		Name:   "second",
		Points: []GuardPoint{GuardOnInput},
		Check: func(_ context.Context, _ GuardPoint, _ *GuardData) *GuardVerdict {
			secondRan = true
			return nil
		},
	})

	g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "hello"})
	if secondRan {
		t.Error("second guard should not run after block")
	}
}

func TestGuardrails_Evaluate_SkipsWrongPoint(t *testing.T) {
	g := NewGuardrails()
	ran := false

	g.Add(Guard{
		Name:   "output_only",
		Points: []GuardPoint{GuardOnOutput},
		Check: func(_ context.Context, _ GuardPoint, _ *GuardData) *GuardVerdict {
			ran = true
			return &GuardVerdict{Action: ActionBlock}
		},
	})

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "hello"})
	if ran {
		t.Error("output guard should not run on input")
	}
	if verdict.Action != ActionAllow {
		t.Error("should allow when no applicable guards")
	}
}

func TestGuardrails_Evaluate_MultipleWarnings(t *testing.T) {
	g := NewGuardrails()
	g.Add(Guard{
		Name:   "warn1",
		Points: []GuardPoint{GuardOnInput},
		Check: func(_ context.Context, _ GuardPoint, _ *GuardData) *GuardVerdict {
			return &GuardVerdict{Action: ActionWarn, Reason: "warning one"}
		},
	})
	g.Add(Guard{
		Name:   "warn2",
		Points: []GuardPoint{GuardOnInput},
		Check: func(_ context.Context, _ GuardPoint, _ *GuardData) *GuardVerdict {
			return &GuardVerdict{Action: ActionWarn, Reason: "warning two"}
		},
	})

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "hello"})
	if verdict.Action != ActionWarn {
		t.Errorf("expected Warn, got %d", verdict.Action)
	}
	if !strings.Contains(verdict.Reason, "warning one") || !strings.Contains(verdict.Reason, "warning two") {
		t.Errorf("reason should contain both warnings: %q", verdict.Reason)
	}
}

// --- Built-in Guards ---

func TestMaxInputLength_Allow(t *testing.T) {
	g := NewGuardrails()
	g.Add(MaxInputLength(100))

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "short"})
	if verdict.Action != ActionAllow {
		t.Error("short input should be allowed")
	}
}

func TestMaxInputLength_Block(t *testing.T) {
	g := NewGuardrails()
	g.Add(MaxInputLength(5))

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "too long"})
	if verdict.Action != ActionBlock {
		t.Error("long input should be blocked")
	}
}

func TestMaxOutputLength_Allow(t *testing.T) {
	g := NewGuardrails()
	g.Add(MaxOutputLength(100))

	verdict := g.Evaluate(context.Background(), GuardOnOutput, &GuardData{Content: "short"})
	if verdict.Action != ActionAllow {
		t.Error("short output should be allowed")
	}
}

func TestMaxOutputLength_Truncate(t *testing.T) {
	g := NewGuardrails()
	g.Add(MaxOutputLength(5))

	data := &GuardData{Content: "too long output here"}
	g.Evaluate(context.Background(), GuardOnOutput, data)
	if !strings.HasSuffix(data.Content, "[truncated]") {
		t.Errorf("should be truncated: %q", data.Content)
	}
}

func TestBlockPatterns_Allow(t *testing.T) {
	g := NewGuardrails()
	g.Add(BlockPatterns("forbidden", "blocked"))

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "hello world"})
	if verdict.Action != ActionAllow {
		t.Error("clean content should be allowed")
	}
}

func TestBlockPatterns_Block(t *testing.T) {
	g := NewGuardrails()
	g.Add(BlockPatterns("forbidden", "blocked"))

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{Content: "this is Forbidden content"})
	if verdict.Action != ActionBlock {
		t.Error("matching content should be blocked (case-insensitive)")
	}
}

func TestBlockPatterns_OutputToo(t *testing.T) {
	g := NewGuardrails()
	g.Add(BlockPatterns("secret"))

	verdict := g.Evaluate(context.Background(), GuardOnOutput, &GuardData{Content: "the secret is..."})
	if verdict.Action != ActionBlock {
		t.Error("block_patterns should work on output too")
	}
}

func TestToolAllowList_Allow(t *testing.T) {
	g := NewGuardrails()
	g.Add(ToolAllowList("read_file", "search"))

	verdict := g.Evaluate(context.Background(), GuardOnToolCall, &GuardData{ToolName: "read_file"})
	if verdict.Action != ActionAllow {
		t.Error("allowed tool should pass")
	}
}

func TestToolAllowList_Block(t *testing.T) {
	g := NewGuardrails()
	g.Add(ToolAllowList("read_file", "search"))

	verdict := g.Evaluate(context.Background(), GuardOnToolCall, &GuardData{ToolName: "shell"})
	if verdict.Action != ActionBlock {
		t.Error("non-allowed tool should be blocked")
	}
}

func TestToolDenyList_Allow(t *testing.T) {
	g := NewGuardrails()
	g.Add(ToolDenyList("shell"))

	verdict := g.Evaluate(context.Background(), GuardOnToolCall, &GuardData{ToolName: "read_file"})
	if verdict.Action != ActionAllow {
		t.Error("non-denied tool should pass")
	}
}

func TestToolDenyList_Block(t *testing.T) {
	g := NewGuardrails()
	g.Add(ToolDenyList("shell"))

	verdict := g.Evaluate(context.Background(), GuardOnToolCall, &GuardData{ToolName: "shell"})
	if verdict.Action != ActionBlock {
		t.Error("denied tool should be blocked")
	}
}

func TestToolResultMaxLength_Allow(t *testing.T) {
	g := NewGuardrails()
	g.Add(ToolResultMaxLength(100))

	data := &GuardData{Content: "short result", ToolName: "read_file"}
	verdict := g.Evaluate(context.Background(), GuardOnToolResult, data)
	if verdict.Action != ActionAllow {
		t.Error("short result should pass")
	}
}

func TestToolResultMaxLength_Truncate(t *testing.T) {
	g := NewGuardrails()
	g.Add(ToolResultMaxLength(10))

	data := &GuardData{Content: "very long tool result here", ToolName: "read_file"}
	g.Evaluate(context.Background(), GuardOnToolResult, data)
	if !strings.Contains(data.Content, "[truncated") {
		t.Errorf("should be truncated: %q", data.Content)
	}
}

func TestRequireHistory_Allow(t *testing.T) {
	g := NewGuardrails()
	g.Add(RequireHistory(10))

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{
		Content: "hello",
		History: []Message{{Role: RoleUser, Content: "hi"}},
	})
	if verdict.Action == ActionBlock {
		t.Error("short history should not block")
	}
}

func TestRequireHistory_Warn(t *testing.T) {
	g := NewGuardrails()
	g.Add(RequireHistory(2))

	history := make([]Message, 5)
	for i := range history {
		history[i] = Message{Role: RoleUser, Content: "msg"}
	}

	verdict := g.Evaluate(context.Background(), GuardOnInput, &GuardData{
		Content: "hello",
		History: history,
	})
	if verdict.Action != ActionWarn {
		t.Errorf("long history should warn, got %d", verdict.Action)
	}
}

// --- Agent Integration ---

func TestAgent_Guardrails_InputBlocked(t *testing.T) {
	g := NewGuardrails()
	g.Add(MaxInputLength(5))

	a := New(Config{
		Provider: &mockProvider{
			responses: []*ChatResponse{{Content: "ok"}},
		},
		Guardrails: g,
	})

	_, err := a.Run(context.Background(), "this input is way too long")
	if err == nil {
		t.Error("expected error for blocked input")
	}
	if !strings.Contains(err.Error(), "blocked") {
		t.Errorf("error should mention blocked: %v", err)
	}
}

func TestAgent_Guardrails_InputAllowed(t *testing.T) {
	g := NewGuardrails()
	g.Add(MaxInputLength(100))

	a := New(Config{
		Provider: &mockProvider{
			responses: []*ChatResponse{{Content: "ok"}},
		},
		Guardrails: g,
	})

	result, err := a.Run(context.Background(), "short")
	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "ok" {
		t.Errorf("content = %q", result.Content)
	}
}

func TestAgent_Guardrails_ToolBlocked(t *testing.T) {
	g := NewGuardrails()
	g.Add(ToolDenyList("shell"))

	// Provider: first call returns tool call for "shell", second call returns final answer
	a := New(Config{
		Provider: &mockProvider{
			responses: []*ChatResponse{
				{ToolCalls: []ToolCall{{ID: "c1", Name: "shell", Arguments: `{"command":"ls"}`}}},
				{Content: "done"},
			},
		},
		Tools:      DefaultTools(),
		Guardrails: g,
	})

	result, err := a.Run(context.Background(), "do something")
	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "done" {
		t.Errorf("content = %q", result.Content)
	}
	// Verify the tool was NOT actually executed by checking history
	// The model should have received a "blocked" error message
	found := false
	for _, m := range a.History() {
		if m.Role == RoleTool && strings.Contains(m.Content, "blocked") {
			found = true
		}
	}
	if !found {
		t.Error("expected blocked message in tool result")
	}
}

func TestAgent_Guardrails_OutputModified(t *testing.T) {
	g := NewGuardrails()
	g.Add(MaxOutputLength(5))

	a := New(Config{
		Provider: &mockProvider{
			responses: []*ChatResponse{{Content: "a very long response that should be truncated"}},
		},
		Guardrails: g,
	})

	result, err := a.Run(context.Background(), "go")
	if err != nil {
		t.Fatal(err)
	}
	if !strings.HasSuffix(result.Content, "[truncated]") {
		t.Errorf("output should be truncated: %q", result.Content)
	}
}

func TestAgent_Guardrails_ToolResultTruncated(t *testing.T) {
	dir := t.TempDir()
	path := dir + "/big.txt"
	os.WriteFile(path, []byte(strings.Repeat("x", 200)), 0o644)

	g := NewGuardrails()
	g.Add(ToolResultMaxLength(50))

	a := New(Config{
		Provider: &mockProvider{
			responses: []*ChatResponse{
				{ToolCalls: []ToolCall{{ID: "c1", Name: "read_file", Arguments: `{"path":"` + path + `"}`}}},
				{Content: "summarized"},
			},
		},
		Tools:      DefaultTools(),
		Guardrails: g,
	})

	result, err := a.Run(context.Background(), "read it")
	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "summarized" {
		t.Errorf("content = %q", result.Content)
	}
}

func TestAgent_SetGuardrails(t *testing.T) {
	a := New(Config{
		Provider: &mockProvider{
			responses: []*ChatResponse{{Content: "ok"}},
		},
	})

	if a.Guardrails() != nil {
		t.Error("should start with no guardrails")
	}

	g := NewGuardrails()
	a.SetGuardrails(g)
	if a.Guardrails() != g {
		t.Error("should be set")
	}
}

// --- guardAppliesToPoint ---

func TestGuardAppliesToPoint(t *testing.T) {
	guard := Guard{Points: []GuardPoint{GuardOnInput, GuardOnOutput}}

	if !guardAppliesToPoint(guard, GuardOnInput) {
		t.Error("should apply to input")
	}
	if !guardAppliesToPoint(guard, GuardOnOutput) {
		t.Error("should apply to output")
	}
	if guardAppliesToPoint(guard, GuardOnToolCall) {
		t.Error("should not apply to tool call")
	}
}
