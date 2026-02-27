package agent

import (
	"errors"
	"os"
	"sync"
	"testing"
	"time"
)

var errTest = errors.New("test error")

// --- EventKind ---

func TestEventKind_String(t *testing.T) {
	tests := []struct {
		kind EventKind
		want string
	}{
		{EventPreThink, "pre_think"},
		{EventPostThink, "post_think"},
		{EventPreTool, "pre_tool"},
		{EventPostTool, "post_tool"},
		{EventComplete, "complete"},
		{EventError, "error"},
		{EventKind(99), "unknown"},
	}

	for _, tt := range tests {
		if got := tt.kind.String(); got != tt.want {
			t.Errorf("EventKind(%d).String() = %q, want %q", tt.kind, got, tt.want)
		}
	}
}

// --- EventBus ---

func TestEventBus_OnAndEmit(t *testing.T) {
	bus := NewEventBus()
	var received []EventKind

	bus.On(EventPreThink, func(e Event) {
		received = append(received, e.Kind)
	})
	bus.On(EventPostThink, func(e Event) {
		received = append(received, e.Kind)
	})

	bus.Emit(Event{Kind: EventPreThink})
	bus.Emit(Event{Kind: EventPostThink})
	bus.Emit(Event{Kind: EventPreTool}) // no handler for this

	if len(received) != 2 {
		t.Fatalf("expected 2 events, got %d", len(received))
	}
	if received[0] != EventPreThink || received[1] != EventPostThink {
		t.Errorf("unexpected events: %v", received)
	}
}

func TestEventBus_OnAll(t *testing.T) {
	bus := NewEventBus()
	var count int

	bus.OnAll(func(e Event) {
		count++
	})

	bus.Emit(Event{Kind: EventPreThink})
	bus.Emit(Event{Kind: EventPostTool})
	bus.Emit(Event{Kind: EventComplete})

	if count != 3 {
		t.Errorf("OnAll handler called %d times, want 3", count)
	}
}

func TestEventBus_KindHandlersBeforeAllHandlers(t *testing.T) {
	bus := NewEventBus()
	var order []string

	bus.On(EventPreThink, func(e Event) {
		order = append(order, "kind")
	})
	bus.OnAll(func(e Event) {
		order = append(order, "all")
	})

	bus.Emit(Event{Kind: EventPreThink})

	if len(order) != 2 || order[0] != "kind" || order[1] != "all" {
		t.Errorf("expected [kind, all], got %v", order)
	}
}

func TestEventBus_Off(t *testing.T) {
	bus := NewEventBus()
	var count int

	id := bus.On(EventPreThink, func(e Event) {
		count++
	})

	bus.Emit(Event{Kind: EventPreThink})
	if count != 1 {
		t.Fatalf("expected count 1, got %d", count)
	}

	bus.Off(id)
	bus.Emit(Event{Kind: EventPreThink})
	if count != 1 {
		t.Errorf("handler still called after Off, count = %d", count)
	}
}

func TestEventBus_OffAll(t *testing.T) {
	bus := NewEventBus()
	var count int

	id := bus.OnAll(func(e Event) {
		count++
	})

	bus.Emit(Event{Kind: EventPreThink})
	bus.Off(id)
	bus.Emit(Event{Kind: EventPreThink})

	if count != 1 {
		t.Errorf("OnAll handler still called after Off, count = %d", count)
	}
}

func TestEventBus_OffNonExistent(t *testing.T) {
	bus := NewEventBus()
	// Should not panic
	bus.Off(999)
}

func TestEventBus_EmitSetsTimestamp(t *testing.T) {
	bus := NewEventBus()
	var captured Event

	bus.On(EventPreThink, func(e Event) {
		captured = e
	})

	before := time.Now()
	bus.Emit(Event{Kind: EventPreThink})

	if captured.Timestamp.Before(before) {
		t.Error("timestamp should be set by Emit")
	}
}

func TestEventBus_EmitPreservesExplicitTimestamp(t *testing.T) {
	bus := NewEventBus()
	var captured Event

	bus.On(EventPreThink, func(e Event) {
		captured = e
	})

	explicit := time.Date(2026, 1, 1, 0, 0, 0, 0, time.UTC)
	bus.Emit(Event{Kind: EventPreThink, Timestamp: explicit})

	if !captured.Timestamp.Equal(explicit) {
		t.Errorf("expected explicit timestamp preserved, got %v", captured.Timestamp)
	}
}

func TestEventBus_HandlerCount(t *testing.T) {
	bus := NewEventBus()

	if bus.HandlerCount() != 0 {
		t.Fatalf("expected 0 handlers, got %d", bus.HandlerCount())
	}

	bus.On(EventPreThink, func(e Event) {})
	bus.On(EventPostThink, func(e Event) {})
	bus.OnAll(func(e Event) {})

	if bus.HandlerCount() != 3 {
		t.Errorf("expected 3 handlers, got %d", bus.HandlerCount())
	}
}

func TestEventBus_MultipleHandlersSameKind(t *testing.T) {
	bus := NewEventBus()
	var count int

	bus.On(EventPreTool, func(e Event) { count++ })
	bus.On(EventPreTool, func(e Event) { count++ })
	bus.On(EventPreTool, func(e Event) { count++ })

	bus.Emit(Event{Kind: EventPreTool})
	if count != 3 {
		t.Errorf("expected 3 calls, got %d", count)
	}
}

func TestEventBus_ConcurrentAccess(t *testing.T) {
	bus := NewEventBus()
	var mu sync.Mutex
	var count int

	bus.OnAll(func(e Event) {
		mu.Lock()
		count++
		mu.Unlock()
	})

	var wg sync.WaitGroup
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			bus.Emit(Event{Kind: EventPreThink})
		}()
	}
	wg.Wait()

	if count != 100 {
		t.Errorf("expected 100 events, got %d", count)
	}
}

// --- Event Data ---

func TestEvent_ToolFields(t *testing.T) {
	e := Event{
		Kind:        EventPostTool,
		ToolName:    "file_read",
		ToolArgs:    `{"path": "/tmp/test"}`,
		ToolResult:  "file contents here",
		ToolElapsed: 50 * time.Millisecond,
	}

	if e.ToolName != "file_read" {
		t.Error("wrong tool name")
	}
	if e.ToolArgs != `{"path": "/tmp/test"}` {
		t.Error("wrong tool args")
	}
	if e.ToolResult != "file contents here" {
		t.Error("wrong tool result")
	}
	if e.ToolElapsed != 50*time.Millisecond {
		t.Error("wrong tool elapsed")
	}
}

func TestEvent_ThinkFields(t *testing.T) {
	resp := &ChatResponse{Content: "hello", InputTokens: 100, OutputTokens: 50}
	e := Event{
		Kind:         EventPostThink,
		Response:     resp,
		Elapsed:      200 * time.Millisecond,
		MessageCount: 5,
	}

	if e.Response.Content != "hello" {
		t.Error("wrong response content")
	}
	if e.MessageCount != 5 {
		t.Error("wrong message count")
	}
}

// --- Agent Event Integration ---

func TestAgent_Events_PrePostThink(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "hello"},
		},
	}

	a := New(Config{Provider: provider})
	var events []EventKind

	a.On(EventPreThink, func(e Event) {
		events = append(events, e.Kind)
		if e.Iteration != 1 {
			t.Errorf("PreThink iteration = %d, want 1", e.Iteration)
		}
		if e.Exchange != 1 {
			t.Errorf("PreThink exchange = %d, want 1", e.Exchange)
		}
	})
	a.On(EventPostThink, func(e Event) {
		events = append(events, e.Kind)
		if e.Response == nil {
			t.Error("PostThink should have response")
		}
		if e.Elapsed == 0 {
			t.Error("PostThink should have elapsed time")
		}
	})

	_, err := a.Run(t.Context(), "hi")
	if err != nil {
		t.Fatal(err)
	}

	if len(events) != 2 || events[0] != EventPreThink || events[1] != EventPostThink {
		t.Errorf("expected [PreThink, PostThink], got %v", events)
	}
}

func TestAgent_Events_Complete(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "done"},
		},
	}

	a := New(Config{Provider: provider})
	var completed bool

	a.On(EventComplete, func(e Event) {
		completed = true
		if e.Result == nil {
			t.Error("Complete should have result")
		}
		if e.Result.Content != "done" {
			t.Errorf("result content = %q, want %q", e.Result.Content, "done")
		}
		if e.Exchange != 1 {
			t.Errorf("exchange = %d, want 1", e.Exchange)
		}
	})

	_, err := a.Run(t.Context(), "test")
	if err != nil {
		t.Fatal(err)
	}

	if !completed {
		t.Error("Complete event never fired")
	}
}

func TestAgent_Events_ToolExecution(t *testing.T) {
	// Create a temp file so the tool succeeds
	tmp := t.TempDir() + "/test-events"
	writeFile(t, tmp, "event test content")

	provider := &mockProvider{
		responses: []*ChatResponse{
			{
				Content: "let me read that",
				ToolCalls: []ToolCall{
					{ID: "tc1", Name: "read_file", Arguments: `{"path": "` + tmp + `"}`},
				},
			},
			{Content: "done"},
		},
	}

	a := New(Config{
		Provider: provider,
		Tools:    DefaultTools(),
	})

	var toolEvents []Event
	a.On(EventPreTool, func(e Event) {
		toolEvents = append(toolEvents, e)
	})
	a.On(EventPostTool, func(e Event) {
		toolEvents = append(toolEvents, e)
	})

	_, err := a.Run(t.Context(), "read the file")
	if err != nil {
		t.Fatal(err)
	}

	if len(toolEvents) != 2 {
		t.Fatalf("expected 2 tool events, got %d", len(toolEvents))
	}

	pre := toolEvents[0]
	if pre.Kind != EventPreTool || pre.ToolName != "read_file" {
		t.Errorf("pre event: kind=%v name=%s", pre.Kind, pre.ToolName)
	}

	post := toolEvents[1]
	if post.Kind != EventPostTool || post.ToolName != "read_file" {
		t.Errorf("post event: kind=%v name=%s", post.Kind, post.ToolName)
	}
	if post.ToolResult == "" {
		t.Error("PostTool should have result")
	}
	if post.ToolElapsed == 0 {
		t.Error("PostTool should have elapsed time")
	}
}

func TestAgent_Events_Error(t *testing.T) {
	provider := &errorProvider{err: errTest}

	a := New(Config{Provider: provider})
	var errorEvent *Event

	a.On(EventError, func(e Event) {
		errorEvent = &e
	})

	_, err := a.Run(t.Context(), "test")
	if err == nil {
		t.Fatal("expected error")
	}

	if errorEvent == nil {
		t.Fatal("Error event never fired")
	}
	if errorEvent.Error != errTest {
		t.Errorf("error = %v, want %v", errorEvent.Error, errTest)
	}
}

func TestAgent_Events_FullCycle(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "simple response"},
		},
	}

	a := New(Config{Provider: provider})

	var allEvents []EventKind
	a.OnAll(func(e Event) {
		allEvents = append(allEvents, e.Kind)
	})

	_, err := a.Run(t.Context(), "hello")
	if err != nil {
		t.Fatal(err)
	}

	// Should see: PreThink, PostThink, Complete
	if len(allEvents) != 3 {
		t.Fatalf("expected 3 events, got %d: %v", len(allEvents), allEvents)
	}
	if allEvents[0] != EventPreThink {
		t.Errorf("event[0] = %v, want PreThink", allEvents[0])
	}
	if allEvents[1] != EventPostThink {
		t.Errorf("event[1] = %v, want PostThink", allEvents[1])
	}
	if allEvents[2] != EventComplete {
		t.Errorf("event[2] = %v, want Complete", allEvents[2])
	}
}

func TestAgent_Events_Accessor(t *testing.T) {
	a := New(Config{Provider: &mockProvider{}})

	if a.Events() == nil {
		t.Fatal("Events() should not be nil")
	}
	if a.Events().HandlerCount() != 0 {
		t.Error("should start with no handlers")
	}

	a.On(EventPreThink, func(e Event) {})
	if a.Events().HandlerCount() != 1 {
		t.Error("should have 1 handler")
	}
}

// Helper: write a file for tool tests
func writeFile(t *testing.T, path, content string) {
	t.Helper()
	if err := os.WriteFile(path, []byte(content), 0o644); err != nil {
		t.Fatal(err)
	}
}
