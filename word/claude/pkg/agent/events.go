// ============================================================================
// METADATA - CPI-SI Agent Event System
// ============================================================================
//
// Key: pkg-agent-events
// Purpose: Observation points in the agent loop. Events let external code
//          watch and react to the agent's think/act/observe cycle without
//          modifying the loop itself. The agent's nervous system.
// Biblical: Ezekiel 1:18 — "Their rings were full of eyes round about"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   Events are emitted at natural boundaries in the agent loop:
//     PreThink  → [Provider.Chat] → PostThink
//     PreTool   → [Tool.Execute]  → PostTool
//     Complete  → exchange done
//     Error     → something went wrong
//
//   Handlers are synchronous — they run inline. Keep them fast.
//   For heavy work, handlers should dispatch to goroutines.
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"sync"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// EventKind identifies the type of agent loop event.
type EventKind int

const (
	// EventPreThink fires before the agent calls the provider.
	EventPreThink EventKind = iota

	// EventPostThink fires after the provider responds.
	EventPostThink

	// EventPreTool fires before a tool is executed.
	EventPreTool

	// EventPostTool fires after a tool finishes executing.
	EventPostTool

	// EventComplete fires when an exchange completes successfully.
	EventComplete

	// EventError fires when an error occurs in the agent loop.
	EventError
)

// String returns the event kind name.
func (k EventKind) String() string {
	switch k {
	case EventPreThink:
		return "pre_think"
	case EventPostThink:
		return "post_think"
	case EventPreTool:
		return "pre_tool"
	case EventPostTool:
		return "post_tool"
	case EventComplete:
		return "complete"
	case EventError:
		return "error"
	default:
		return "unknown"
	}
}

// Event represents something that happened in the agent loop.
type Event struct {
	// Kind identifies what happened.
	Kind EventKind

	// Timestamp is when the event occurred.
	Timestamp time.Time

	// Iteration is the current think/act/observe cycle number (1-based).
	Iteration int

	// Exchange is the current exchange number in this session.
	Exchange int

	// --- Provider events (PreThink/PostThink) ---

	// MessageCount is the number of messages sent to the provider.
	MessageCount int

	// Response holds the provider's response (PostThink only).
	Response *ChatResponse

	// Elapsed is the duration of the provider call (PostThink only).
	Elapsed time.Duration

	// --- Tool events (PreTool/PostTool) ---

	// ToolName is the name of the tool being executed.
	ToolName string

	// ToolArgs is the JSON arguments string for the tool call.
	ToolArgs string

	// ToolResult is the output from tool execution (PostTool only).
	ToolResult string

	// ToolError is any error from tool execution (PostTool only).
	ToolError error

	// ToolElapsed is how long the tool took (PostTool only).
	ToolElapsed time.Duration

	// --- Completion events ---

	// Result holds the final RunResult (Complete only).
	Result *RunResult

	// Error holds the error (Error events only).
	Error error
}

// EventHandler is a function that processes an agent event.
// Handlers run synchronously in the agent loop — keep them fast.
type EventHandler func(Event)

// EventBus manages event subscriptions and emission.
// Thread-safe for concurrent handler registration.
type EventBus struct {
	mu       sync.RWMutex
	handlers map[EventKind][]handlerEntry
	all      []handlerEntry // handlers that receive all events
	nextID   int
}

// handlerEntry pairs a handler with an ID for removal.
type handlerEntry struct {
	id      int
	handler EventHandler
}

// NewEventBus creates an empty event bus.
func NewEventBus() *EventBus {
	return &EventBus{
		handlers: make(map[EventKind][]handlerEntry),
	}
}

// On registers a handler for a specific event kind.
// Returns an ID that can be used to unsubscribe.
func (b *EventBus) On(kind EventKind, handler EventHandler) int {
	b.mu.Lock()
	defer b.mu.Unlock()
	b.nextID++
	b.handlers[kind] = append(b.handlers[kind], handlerEntry{
		id:      b.nextID,
		handler: handler,
	})
	return b.nextID
}

// OnAll registers a handler that receives every event.
// Returns an ID that can be used to unsubscribe.
func (b *EventBus) OnAll(handler EventHandler) int {
	b.mu.Lock()
	defer b.mu.Unlock()
	b.nextID++
	b.all = append(b.all, handlerEntry{
		id:      b.nextID,
		handler: handler,
	})
	return b.nextID
}

// Off removes a handler by ID.
func (b *EventBus) Off(id int) {
	b.mu.Lock()
	defer b.mu.Unlock()

	// Check kind-specific handlers
	for kind, entries := range b.handlers {
		for i, e := range entries {
			if e.id == id {
				b.handlers[kind] = append(entries[:i], entries[i+1:]...)
				return
			}
		}
	}

	// Check all-event handlers
	for i, e := range b.all {
		if e.id == id {
			b.all = append(b.all[:i], b.all[i+1:]...)
			return
		}
	}
}

// Emit sends an event to all registered handlers.
// Kind-specific handlers run first, then all-event handlers.
func (b *EventBus) Emit(event Event) {
	if event.Timestamp.IsZero() {
		event.Timestamp = time.Now()
	}

	b.mu.RLock()
	// Copy slices to release lock before calling handlers
	kindHandlers := make([]handlerEntry, len(b.handlers[event.Kind]))
	copy(kindHandlers, b.handlers[event.Kind])
	allHandlers := make([]handlerEntry, len(b.all))
	copy(allHandlers, b.all)
	b.mu.RUnlock()

	for _, e := range kindHandlers {
		e.handler(event)
	}
	for _, e := range allHandlers {
		e.handler(event)
	}
}

// HandlerCount returns the total number of registered handlers.
func (b *EventBus) HandlerCount() int {
	b.mu.RLock()
	defer b.mu.RUnlock()
	total := len(b.all)
	for _, entries := range b.handlers {
		total += len(entries)
	}
	return total
}

// ============================================================================
// CLOSING
// ============================================================================
// "Their rings were full of eyes round about." — Ezekiel 1:18
// The event system sees everything — the agent's nervous system observes
// every think, every act, every completion.
