// ============================================================================
// METADATA - CPI-SI Conversation Manager
// ============================================================================
//
// Key: pkg-agent-conversation
// Purpose: Multi-session conversation bridging. Tracks summaries, thread state,
//          and key facts across sessions so the agent maintains continuity.
//          When context grows, the manager summarizes older exchanges to stay
//          within token budgets while preserving essential knowledge.
// Biblical: Deuteronomy 6:12 — "Then beware lest thou forget the LORD"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   Conversation wraps the agent's history with structured tracking:
//     ThreadState — current topic and context tags
//     Summaries   — compressed older exchanges
//     KeyFacts    — persistent knowledge from the conversation
//
//   As exchanges grow beyond a threshold, older messages are summarized
//   into compact form, preserving essential context while freeing tokens.
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// Conversation tracks multi-session state beyond raw message history.
type Conversation struct {
	// Thread identifies the current conversation thread.
	Thread ThreadState `json:"thread"`

	// Summaries are compressed older exchanges.
	Summaries []ExchangeSummary `json:"summaries,omitempty"`

	// KeyFacts are persistent knowledge extracted from conversation.
	KeyFacts []KeyFact `json:"key_facts,omitempty"`

	// Metadata tracks conversation-level stats.
	Meta ConversationMeta `json:"meta"`
}

// ThreadState describes the current conversation focus.
type ThreadState struct {
	// Topic is the current conversation subject.
	Topic string `json:"topic,omitempty"`

	// Tags are contextual labels for the thread.
	Tags []string `json:"tags,omitempty"`

	// ActiveSince is when this thread started.
	ActiveSince time.Time `json:"active_since,omitempty"`
}

// ExchangeSummary is a compressed version of one or more exchanges.
type ExchangeSummary struct {
	// Summary is a brief description of what happened.
	Summary string `json:"summary"`

	// Exchange is the exchange number (or range).
	Exchange string `json:"exchange"`

	// KeyPoints extracted from the exchange.
	KeyPoints []string `json:"key_points,omitempty"`

	// ToolsUsed lists which tools were invoked.
	ToolsUsed []string `json:"tools_used,omitempty"`

	// Timestamp when the exchange occurred.
	Timestamp time.Time `json:"timestamp,omitempty"`
}

// KeyFact is a piece of persistent knowledge from the conversation.
type KeyFact struct {
	// Fact is the knowledge statement.
	Fact string `json:"fact"`

	// Source describes where this fact came from.
	Source string `json:"source,omitempty"`

	// LearnedAt is when this fact was established.
	LearnedAt time.Time `json:"learned_at,omitempty"`
}

// ConversationMeta tracks conversation-level statistics.
type ConversationMeta struct {
	// TotalExchanges across all sessions.
	TotalExchanges int `json:"total_exchanges"`

	// TotalSessions this conversation spans.
	TotalSessions int `json:"total_sessions"`

	// CreatedAt is when the conversation started.
	CreatedAt time.Time `json:"created_at"`

	// UpdatedAt is the last modification time.
	UpdatedAt time.Time `json:"updated_at"`

	// TotalInputTokens across all sessions.
	TotalInputTokens int `json:"total_input_tokens"`

	// TotalOutputTokens across all sessions.
	TotalOutputTokens int `json:"total_output_tokens"`
}

// NewConversation creates a new conversation tracker.
func NewConversation() *Conversation {
	now := time.Now()
	return &Conversation{
		Thread: ThreadState{
			ActiveSince: now,
		},
		Meta: ConversationMeta{
			CreatedAt: now,
			UpdatedAt: now,
		},
	}
}

// SetTopic updates the conversation thread topic.
func (c *Conversation) SetTopic(topic string) {
	c.Thread.Topic = topic
	c.Meta.UpdatedAt = time.Now()
}

// AddTag adds a context tag to the thread.
func (c *Conversation) AddTag(tag string) {
	for _, existing := range c.Thread.Tags {
		if existing == tag {
			return // already present
		}
	}
	c.Thread.Tags = append(c.Thread.Tags, tag)
	c.Meta.UpdatedAt = time.Now()
}

// RemoveTag removes a context tag.
func (c *Conversation) RemoveTag(tag string) bool {
	for i, existing := range c.Thread.Tags {
		if existing == tag {
			c.Thread.Tags = append(c.Thread.Tags[:i], c.Thread.Tags[i+1:]...)
			c.Meta.UpdatedAt = time.Now()
			return true
		}
	}
	return false
}

// AddSummary records a compressed exchange summary.
func (c *Conversation) AddSummary(summary ExchangeSummary) {
	if summary.Timestamp.IsZero() {
		summary.Timestamp = time.Now()
	}
	c.Summaries = append(c.Summaries, summary)
	c.Meta.UpdatedAt = time.Now()
}

// AddKeyFact records a persistent knowledge fact.
func (c *Conversation) AddKeyFact(fact, source string) {
	c.KeyFacts = append(c.KeyFacts, KeyFact{
		Fact:      fact,
		Source:    source,
		LearnedAt: time.Now(),
	})
	c.Meta.UpdatedAt = time.Now()
}

// RemoveKeyFact removes a fact by its content.
func (c *Conversation) RemoveKeyFact(fact string) bool {
	for i, kf := range c.KeyFacts {
		if kf.Fact == fact {
			c.KeyFacts = append(c.KeyFacts[:i], c.KeyFacts[i+1:]...)
			c.Meta.UpdatedAt = time.Now()
			return true
		}
	}
	return false
}

// RecordExchange updates metadata after an exchange completes.
func (c *Conversation) RecordExchange(inputTokens, outputTokens int) {
	c.Meta.TotalExchanges++
	c.Meta.TotalInputTokens += inputTokens
	c.Meta.TotalOutputTokens += outputTokens
	c.Meta.UpdatedAt = time.Now()
}

// NewSession increments the session counter.
func (c *Conversation) NewSession() {
	c.Meta.TotalSessions++
	c.Meta.UpdatedAt = time.Now()
}

// --- Context Building ---

// BuildContext generates a context string from the conversation state.
// This is injected into the system prompt to give the model awareness
// of the conversation's history across sessions.
func (c *Conversation) BuildContext() string {
	var sb strings.Builder

	// Thread context
	if c.Thread.Topic != "" {
		sb.WriteString("Current topic: " + c.Thread.Topic + "\n")
	}
	if len(c.Thread.Tags) > 0 {
		sb.WriteString("Context: " + strings.Join(c.Thread.Tags, ", ") + "\n")
	}

	// Previous session summaries
	if len(c.Summaries) > 0 {
		sb.WriteString("\n--- Previous exchanges ---\n")
		// Show most recent summaries (last 10)
		start := 0
		if len(c.Summaries) > 10 {
			start = len(c.Summaries) - 10
		}
		for _, s := range c.Summaries[start:] {
			sb.WriteString("- [" + s.Exchange + "] " + s.Summary)
			if len(s.KeyPoints) > 0 {
				sb.WriteString(" (" + strings.Join(s.KeyPoints, "; ") + ")")
			}
			sb.WriteString("\n")
		}
	}

	// Key facts
	if len(c.KeyFacts) > 0 {
		sb.WriteString("\n--- Key facts ---\n")
		for _, kf := range c.KeyFacts {
			sb.WriteString("- " + kf.Fact + "\n")
		}
	}

	// Stats
	if c.Meta.TotalExchanges > 0 {
		sb.WriteString(fmt.Sprintf("\nConversation: %d exchanges across %d sessions\n",
			c.Meta.TotalExchanges, c.Meta.TotalSessions))
	}

	return sb.String()
}

// --- Summarization ---

// SummarizeHistory compresses a set of messages into an exchange summary.
// This is a simple extractive summary — takes the first user message and
// first assistant response. For LLM-based summarization, use the agent itself.
func SummarizeHistory(messages []Message, exchangeLabel string) ExchangeSummary {
	summary := ExchangeSummary{
		Exchange:  exchangeLabel,
		Timestamp: time.Now(),
	}

	var userParts, assistantParts []string
	toolSet := make(map[string]bool)

	for _, m := range messages {
		switch m.Role {
		case RoleUser:
			if len(m.Content) > 200 {
				userParts = append(userParts, m.Content[:200]+"...")
			} else if m.Content != "" {
				userParts = append(userParts, m.Content)
			}
		case RoleAssistant:
			for _, tc := range m.ToolCalls {
				toolSet[tc.Name] = true
			}
			if len(m.Content) > 200 {
				assistantParts = append(assistantParts, m.Content[:200]+"...")
			} else if m.Content != "" {
				assistantParts = append(assistantParts, m.Content)
			}
		}
	}

	// Build summary from first user question and first assistant answer
	if len(userParts) > 0 {
		summary.Summary = "User asked: " + userParts[0]
	}
	if len(assistantParts) > 0 {
		summary.KeyPoints = append(summary.KeyPoints, assistantParts[0])
	}

	for tool := range toolSet {
		summary.ToolsUsed = append(summary.ToolsUsed, tool)
	}

	return summary
}

// --- Persistence ---

// SaveConversation writes the conversation state to a JSON file.
func SaveConversation(path string, conv *Conversation) error {
	data, err := json.MarshalIndent(conv, "", "  ")
	if err != nil {
		return fmt.Errorf("marshal conversation: %w", err)
	}

	if err := os.MkdirAll(filepath.Dir(path), 0o755); err != nil {
		return fmt.Errorf("create conversation directory: %w", err)
	}

	// Atomic write
	tmp := path + ".tmp"
	if err := os.WriteFile(tmp, data, 0o644); err != nil {
		return fmt.Errorf("write conversation: %w", err)
	}
	if err := os.Rename(tmp, path); err != nil {
		os.Remove(tmp)
		return fmt.Errorf("rename conversation: %w", err)
	}

	return nil
}

// LoadConversation reads conversation state from a JSON file.
func LoadConversation(path string) (*Conversation, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("read conversation %s: %w", path, err)
	}

	var conv Conversation
	if err := json.Unmarshal(data, &conv); err != nil {
		return nil, fmt.Errorf("parse conversation %s: %w", path, err)
	}

	return &conv, nil
}

// ============================================================================
// CLOSING
// ============================================================================
// "Then beware lest thou forget the LORD." — Deuteronomy 6:12
// Context is continuity. The conversation manager remembers what the
// history alone would lose — bridging sessions, preserving knowledge.
