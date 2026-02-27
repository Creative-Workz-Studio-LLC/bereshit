package agent

import (
	"testing"
)

// --- SubAgent ---

func TestSpawn_InheritsProvider(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "parent response"},
			{Content: "child response"},
		},
	}

	parent := New(Config{Provider: provider})
	sub := parent.Spawn(SubAgentConfig{
		Name:     "child",
		Identity: "You are a helper.",
	})

	if sub.Name() != "child" {
		t.Errorf("name = %q, want %q", sub.Name(), "child")
	}
	if sub.Agent() == nil {
		t.Fatal("Agent() should not be nil")
	}

	// Sub-agent should use parent's provider
	result, err := sub.Run(t.Context(), "test")
	if err != nil {
		t.Fatal(err)
	}
	// mockProvider returns responses in order, so first call gets first response
	if result.Content == "" {
		t.Error("expected content from sub-agent")
	}
}

func TestSpawn_CustomProvider(t *testing.T) {
	parentProvider := &mockProvider{
		responses: []*ChatResponse{{Content: "parent"}},
	}
	childProvider := &mockProvider{
		responses: []*ChatResponse{{Content: "child"}},
	}

	parent := New(Config{Provider: parentProvider})
	sub := parent.Spawn(SubAgentConfig{
		Name:     "custom",
		Identity: "Custom identity",
		Provider: childProvider,
	})

	result, err := sub.Run(t.Context(), "test")
	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "child" {
		t.Errorf("content = %q, want %q", result.Content, "child")
	}
}

func TestSpawn_CustomTools(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{{Content: "ok"}},
	}

	parent := New(Config{
		Provider: provider,
		Tools:    DefaultTools(),
	})

	customTool := &mockTool{
		def: ToolDef{Name: "custom_tool", Description: "test"},
	}

	sub := parent.Spawn(SubAgentConfig{
		Name:  "tooled",
		Tools: []Tool{customTool},
	})

	// Sub-agent should have only the custom tool, not parent's tools
	agent := sub.Agent()
	if len(agent.tools) != 1 {
		t.Errorf("expected 1 tool, got %d", len(agent.tools))
	}
	if agent.tools[0].Definition().Name != "custom_tool" {
		t.Errorf("tool name = %q, want %q", agent.tools[0].Definition().Name, "custom_tool")
	}
}

func TestSpawn_InheritsTools(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{{Content: "ok"}},
	}

	parent := New(Config{
		Provider: provider,
		Tools:    DefaultTools(),
	})

	sub := parent.Spawn(SubAgentConfig{
		Name: "inheritor",
	})

	// Should have same number of tools as parent
	if len(sub.Agent().tools) != len(parent.tools) {
		t.Errorf("sub tools = %d, parent tools = %d", len(sub.Agent().tools), len(parent.tools))
	}
}

func TestSubAgent_Reset(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "first"},
			{Content: "second"},
		},
	}

	parent := New(Config{Provider: provider})
	sub := parent.Spawn(SubAgentConfig{Name: "resettable"})

	_, err := sub.Run(t.Context(), "first message")
	if err != nil {
		t.Fatal(err)
	}

	if len(sub.Agent().History()) == 0 {
		t.Fatal("expected history after run")
	}

	sub.Reset()
	if len(sub.Agent().History()) != 0 {
		t.Error("history should be empty after reset")
	}
}

// --- Family ---

func TestFamily_RegisterAndMembers(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	err := family.Register(SubAgentConfig{Name: "alpha"})
	if err != nil {
		t.Fatal(err)
	}
	err = family.Register(SubAgentConfig{Name: "beta"})
	if err != nil {
		t.Fatal(err)
	}

	members := family.Members()
	if len(members) != 2 {
		t.Fatalf("expected 2 members, got %d", len(members))
	}
	// Sorted alphabetically
	if members[0] != "alpha" || members[1] != "beta" {
		t.Errorf("members = %v, want [alpha, beta]", members)
	}
}

func TestFamily_RegisterEmptyName(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	err := family.Register(SubAgentConfig{Name: ""})
	if err == nil {
		t.Error("expected error for empty name")
	}
}

func TestFamily_Has(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	family.Register(SubAgentConfig{Name: "nathan"})

	if !family.Has("nathan") {
		t.Error("should have nathan")
	}
	if family.Has("ezra") {
		t.Error("should not have ezra")
	}
}

func TestFamily_Size(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	if family.Size() != 0 {
		t.Errorf("expected size 0, got %d", family.Size())
	}

	family.Register(SubAgentConfig{Name: "one"})
	family.Register(SubAgentConfig{Name: "two"})

	if family.Size() != 2 {
		t.Errorf("expected size 2, got %d", family.Size())
	}
}

func TestFamily_Unregister(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	family.Register(SubAgentConfig{Name: "temp"})
	if !family.Has("temp") {
		t.Fatal("should have temp")
	}

	family.Unregister("temp")
	if family.Has("temp") {
		t.Error("should not have temp after unregister")
	}
}

func TestFamily_Get_Spawns(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{{Content: "ok"}},
	}
	parent := New(Config{Provider: provider})
	family := NewFamily(parent)

	family.Register(SubAgentConfig{
		Name:     "worker",
		Identity: "You are a worker.",
	})

	sub, err := family.Get("worker")
	if err != nil {
		t.Fatal(err)
	}
	if sub.Name() != "worker" {
		t.Errorf("name = %q", sub.Name())
	}
}

func TestFamily_Get_ReturnsSame(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	family.Register(SubAgentConfig{Name: "worker"})

	sub1, _ := family.Get("worker")
	sub2, _ := family.Get("worker")

	if sub1 != sub2 {
		t.Error("Get should return the same sub-agent instance")
	}
}

func TestFamily_Get_Unknown(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	_, err := family.Get("nonexistent")
	if err == nil {
		t.Error("expected error for unknown member")
	}
}

func TestFamily_Dispatch(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "nathan's response"},
		},
	}

	parent := New(Config{Provider: provider})
	family := NewFamily(parent)

	family.Register(SubAgentConfig{
		Name:     "nathan",
		Identity: "You are Nathan, the test validator.",
	})

	result, err := family.Dispatch(t.Context(), "nathan", "run the tests")
	if err != nil {
		t.Fatal(err)
	}
	if result.Content != "nathan's response" {
		t.Errorf("content = %q", result.Content)
	}
}

func TestFamily_Dispatch_Unknown(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	_, err := family.Dispatch(t.Context(), "nobody", "do something")
	if err == nil {
		t.Error("expected error for unknown member")
	}
}

func TestFamily_DispatchAll(t *testing.T) {
	// Each sub-agent gets its own mockProvider since they run concurrently
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "alpha result"},
			{Content: "beta result"},
		},
	}

	parent := New(Config{Provider: provider})
	family := NewFamily(parent)

	family.Register(SubAgentConfig{Name: "alpha"})
	family.Register(SubAgentConfig{Name: "beta"})

	tasks := map[string]string{
		"alpha": "do alpha work",
		"beta":  "do beta work",
	}

	results := family.DispatchAll(t.Context(), tasks)

	if len(results) != 2 {
		t.Fatalf("expected 2 results, got %d", len(results))
	}

	// Results are sorted by name
	if results[0].Name != "alpha" {
		t.Errorf("results[0].Name = %q, want alpha", results[0].Name)
	}
	if results[1].Name != "beta" {
		t.Errorf("results[1].Name = %q, want beta", results[1].Name)
	}

	// Check no errors
	for _, r := range results {
		if r.Error != nil {
			t.Errorf("%s had error: %v", r.Name, r.Error)
		}
		if r.Result == nil {
			t.Errorf("%s had nil result", r.Name)
		}
	}
}

func TestFamily_DispatchAll_WithError(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{
		responses: []*ChatResponse{{Content: "ok"}},
	}})
	family := NewFamily(parent)

	family.Register(SubAgentConfig{Name: "good"})
	// "bad" is not registered

	tasks := map[string]string{
		"good": "do work",
		"bad":  "fail",
	}

	results := family.DispatchAll(t.Context(), tasks)

	// Both should have entries
	if len(results) != 2 {
		t.Fatalf("expected 2 results, got %d", len(results))
	}

	// Find the bad result
	for _, r := range results {
		if r.Name == "bad" {
			if r.Error == nil {
				t.Error("bad should have error")
			}
		}
	}
}

func TestFamily_ResetAll(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "a"},
			{Content: "b"},
		},
	}

	parent := New(Config{Provider: provider})
	family := NewFamily(parent)

	family.Register(SubAgentConfig{Name: "a"})
	family.Register(SubAgentConfig{Name: "b"})

	// Dispatch to spawn and build history
	family.Dispatch(t.Context(), "a", "hello")
	family.Dispatch(t.Context(), "b", "world")

	// Both should have history
	subA, _ := family.Get("a")
	subB, _ := family.Get("b")
	if len(subA.Agent().History()) == 0 || len(subB.Agent().History()) == 0 {
		t.Fatal("expected history in both agents")
	}

	family.ResetAll()

	if len(subA.Agent().History()) != 0 {
		t.Error("a should have empty history after ResetAll")
	}
	if len(subB.Agent().History()) != 0 {
		t.Error("b should have empty history after ResetAll")
	}
}

func TestFamily_DismissAll(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{{Content: "ok"}},
	}

	parent := New(Config{Provider: provider})
	family := NewFamily(parent)

	family.Register(SubAgentConfig{Name: "worker"})
	family.Dispatch(t.Context(), "worker", "hello")

	// Get should return the same instance
	sub1, _ := family.Get("worker")

	family.DismissAll()

	// Now Get should spawn a fresh instance
	provider.responses = append(provider.responses, &ChatResponse{Content: "new"})
	sub2, _ := family.Get("worker")

	if sub1 == sub2 {
		t.Error("after DismissAll, Get should return a new instance")
	}
}

// --- sortStrings ---

func TestSortStrings(t *testing.T) {
	s := []string{"charlie", "alpha", "beta"}
	sortStrings(s)
	if s[0] != "alpha" || s[1] != "beta" || s[2] != "charlie" {
		t.Errorf("sort failed: %v", s)
	}
}

func TestSortStrings_Empty(t *testing.T) {
	var s []string
	sortStrings(s) // should not panic
}

func TestSortStrings_Single(t *testing.T) {
	s := []string{"only"}
	sortStrings(s)
	if s[0] != "only" {
		t.Error("single element sort failed")
	}
}

// --- sortDispatchResults ---

func TestSortDispatchResults(t *testing.T) {
	results := []DispatchResult{
		{Name: "charlie"},
		{Name: "alpha"},
		{Name: "beta"},
	}
	sortDispatchResults(results)
	if results[0].Name != "alpha" || results[1].Name != "beta" || results[2].Name != "charlie" {
		t.Errorf("sort failed: %v", results)
	}
}

// --- Concurrent Family Access ---

func TestFamily_ConcurrentRegister(t *testing.T) {
	parent := New(Config{Provider: &mockProvider{}})
	family := NewFamily(parent)

	// Register from multiple goroutines
	done := make(chan bool, 10)
	for i := 0; i < 10; i++ {
		go func(n int) {
			family.Register(SubAgentConfig{
				Name: "member-" + string(rune('a'+n)),
			})
			done <- true
		}(i)
	}

	for i := 0; i < 10; i++ {
		<-done
	}

	if family.Size() != 10 {
		t.Errorf("expected 10 members, got %d", family.Size())
	}
}

// --- SubAgent with Error Provider ---

func TestSubAgent_ProviderError(t *testing.T) {
	provider := &errorProvider{err: errTest}

	parent := New(Config{Provider: provider})
	sub := parent.Spawn(SubAgentConfig{Name: "erroring"})

	_, err := sub.Run(t.Context(), "test")
	if err == nil {
		t.Error("expected error from provider")
	}
}
