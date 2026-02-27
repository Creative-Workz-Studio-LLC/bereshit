// ============================================================================
// METADATA - CPI-SI Session Persistence
// ============================================================================
//
// Key: pkg-agent-session
// Purpose: Save and restore agent sessions. Sessions are the MEMORY of CPI-SI.
//          The agent remembers what happened. Identity persists across sessions.
// Biblical: Deuteronomy 8:2 — "Thou shalt remember all the way which the LORD
//           thy God led thee"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   Sessions are saved as JSON files — portable, inspectable, versionable.
//   The agent owns its memory. The filesystem carries it forward.
//
//   Session = History + Context + Metadata
//   Identity is NOT part of the session — it's part of the agent.
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
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// Session captures a point-in-time snapshot of the agent's conversation state.
type Session struct {
	// ID uniquely identifies this session.
	ID string `json:"id"`

	// CreatedAt records when the session started.
	CreatedAt time.Time `json:"created_at"`

	// UpdatedAt records when the session was last saved.
	UpdatedAt time.Time `json:"updated_at"`

	// Provider records which provider was used.
	Provider string `json:"provider"`

	// Model records which model was used.
	Model string `json:"model"`

	// History is the conversation messages.
	History []Message `json:"history"`

	// ContextKeys lists the keys of loaded context segments.
	// (Context content is not saved — it's re-loaded from files.)
	ContextKeys []string `json:"context_keys,omitempty"`

	// ContextPaths maps context keys to file paths for re-loading.
	ContextPaths map[string]string `json:"context_paths,omitempty"`

	// Usage tracks cumulative token usage for the session.
	Usage SessionUsage `json:"usage"`

	// Exchanges is the number of complete user→assistant exchanges.
	Exchanges int `json:"exchanges"`
}

// SessionUsage tracks cumulative token usage across a session.
type SessionUsage struct {
	InputTokens  int `json:"input_tokens"`
	OutputTokens int `json:"output_tokens"`
	ToolCalls    int `json:"tool_calls"`
}

// --- Save/Load Operations ---

// SaveSession writes the current agent state to a JSON file.
func (a *Agent) SaveSession(path string) error {
	session := &Session{
		ID:        filepath.Base(path),
		CreatedAt: time.Now(),
		UpdatedAt: time.Now(),
		History:   a.history,
		Exchanges: a.exchanges,
	}

	// Record provider info
	if a.provider != nil {
		session.Provider = a.provider.Name()
		session.Model = a.provider.Model()
	}

	// Record context paths for re-loading
	contextKeys := a.ContextSegments()
	if len(contextKeys) > 0 {
		session.ContextKeys = contextKeys
		session.ContextPaths = make(map[string]string, len(contextKeys))
		for _, key := range contextKeys {
			// Strip "context:" prefix to get the file path
			if len(key) > 8 && key[:8] == "context:" {
				session.ContextPaths[key] = key[8:]
			}
		}
	}

	// Marshal with indentation for human readability
	data, err := json.MarshalIndent(session, "", "  ")
	if err != nil {
		return fmt.Errorf("marshal session: %w", err)
	}

	// Ensure directory exists
	dir := filepath.Dir(path)
	if err := os.MkdirAll(dir, 0o755); err != nil {
		return fmt.Errorf("create session dir: %w", err)
	}

	// Atomic write
	tmp := path + ".tmp"
	if err := os.WriteFile(tmp, data, 0o644); err != nil {
		return fmt.Errorf("write session: %w", err)
	}
	if err := os.Rename(tmp, path); err != nil {
		os.Remove(tmp)
		return fmt.Errorf("rename session: %w", err)
	}

	return nil
}

// LoadSession restores agent state from a JSON file.
// The provider and tools must already be configured on the agent.
// Context segments are re-loaded from their original file paths.
func (a *Agent) LoadSession(path string) error {
	data, err := os.ReadFile(path)
	if err != nil {
		return fmt.Errorf("read session: %w", err)
	}

	var session Session
	if err := json.Unmarshal(data, &session); err != nil {
		return fmt.Errorf("unmarshal session: %w", err)
	}

	// Restore history
	a.history = session.History
	a.exchanges = session.Exchanges

	// Re-index history
	a.indexHistory()

	// Re-load context from file paths
	if len(session.ContextPaths) > 0 {
		for _, filePath := range session.ContextPaths {
			// Best effort — file may have moved or been deleted
			_ = a.LoadContext(filePath)
		}
	}

	return nil
}

// SessionInfo returns metadata about the current session state without
// saving it. Useful for display purposes.
func (a *Agent) SessionInfo() SessionInfo {
	return SessionInfo{
		Exchanges:   a.exchanges,
		Messages:    len(a.history),
		ContextKeys: a.ContextSegments(),
		IndexTokens: a.index.TotalEstimatedTokens(),
	}
}

// SessionInfo is a lightweight view of the current session.
type SessionInfo struct {
	Exchanges   int
	Messages    int
	ContextKeys []string
	IndexTokens int
}

// ============================================================================
// CLOSING
// ============================================================================
// "Thou shalt remember all the way which the LORD thy God led thee." — Deut 8:2
// Sessions are memory. The agent remembers. Identity persists.
