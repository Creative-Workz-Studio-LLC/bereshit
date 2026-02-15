// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-dashboard-events
// Purpose: Event bus for real-time state change distribution
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// ═══════════════════════════════════════════════════════════════════════════

package dashboard

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"sync"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// StateChange is emitted when runtime files change.
type StateChange struct {
	Type      string         `json:"type"`      // "state_update", "log_event", "path_update", "history_update"
	Snapshot  *StateSnapshot `json:"snapshot,omitempty"`
	Event     *LogEvent      `json:"event,omitempty"`
	Timestamp string         `json:"timestamp"`
}

// LogEvent is a structured event from session logs.
type LogEvent struct {
	Timestamp   string                 `json:"timestamp"`
	Type        string                 `json:"type"`
	SessionID   string                 `json:"session_id,omitempty"`
	Sequence    int                    `json:"sequence,omitempty"`
	Level       string                 `json:"level"`
	Component   string                 `json:"component"`
	Message     string                 `json:"message"`
	Health      float64                `json:"health,omitempty"`
	Delta       float64                `json:"delta,omitempty"`
	HealthLevel string                 `json:"health_level,omitempty"`
	Trajectory  string                 `json:"trajectory,omitempty"`
	Details     map[string]interface{} `json:"details,omitempty"`
}

// EventBus distributes state changes to multiple subscribers.
type EventBus struct {
	subscribers map[chan StateChange]bool
	mu          sync.RWMutex
}

// NewEventBus creates a new event distribution bus.
func NewEventBus() *EventBus {
	return &EventBus{
		subscribers: make(map[chan StateChange]bool),
	}
}

// Subscribe returns a channel that receives state changes.
// The channel has a buffer of 64 to prevent slow subscribers from blocking.
func (eb *EventBus) Subscribe() <-chan StateChange {
	ch := make(chan StateChange, 64)
	eb.mu.Lock()
	eb.subscribers[ch] = true
	eb.mu.Unlock()
	return ch
}

// Unsubscribe removes a subscriber and closes its channel.
func (eb *EventBus) Unsubscribe(ch <-chan StateChange) {
	eb.mu.Lock()
	defer eb.mu.Unlock()
	// Find the matching send channel
	for sub := range eb.subscribers {
		if sub == ch {
			delete(eb.subscribers, sub)
			close(sub)
			return
		}
	}
}

// Publish sends a state change to all subscribers.
// Non-blocking: if a subscriber's buffer is full, the event is dropped for that subscriber.
func (eb *EventBus) Publish(change StateChange) {
	if change.Timestamp == "" {
		change.Timestamp = time.Now().Format(time.RFC3339)
	}
	eb.mu.RLock()
	defer eb.mu.RUnlock()
	for ch := range eb.subscribers {
		select {
		case ch <- change:
		default:
			// Drop event for slow subscriber
		}
	}
}

// SubscriberCount returns the number of active subscribers.
func (eb *EventBus) SubscriberCount() int {
	eb.mu.RLock()
	defer eb.mu.RUnlock()
	return len(eb.subscribers)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
