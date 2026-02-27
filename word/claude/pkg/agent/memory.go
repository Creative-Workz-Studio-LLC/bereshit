// ============================================================================
// METADATA - CPI-SI Agent Memory
// ============================================================================
//
// Key: pkg-agent-memory
// Purpose: Persistent key-value memory that survives across sessions.
//          The agent LEARNS from experience. Knowledge accumulates.
//          Not conversation history — structured knowledge the agent builds.
// Biblical: Deuteronomy 6:6-7 — "These words, which I command thee this day,
//           shall be in thine heart"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   Memory is a persistent key-value store backed by a JSON file.
//   The agent can store facts, preferences, patterns, and knowledge
//   that carries forward across sessions. Memory is separate from
//   conversation history — it's what the agent has LEARNED.
//
//   Categories organize knowledge:
//   - "facts"     — verified information about the world
//   - "prefs"     — user/system preferences
//   - "patterns"  — recognized patterns and conventions
//   - "context"   — persistent context (project info, etc.)
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
	"sort"
	"sync"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// MemoryEntry is a single piece of knowledge the agent has stored.
type MemoryEntry struct {
	// Key identifies this memory uniquely within its category.
	Key string `json:"key"`

	// Category organizes memories (e.g., "facts", "prefs", "patterns").
	Category string `json:"category"`

	// Value is the stored knowledge.
	Value string `json:"value"`

	// CreatedAt records when this memory was first stored.
	CreatedAt time.Time `json:"created_at"`

	// UpdatedAt records when this memory was last modified.
	UpdatedAt time.Time `json:"updated_at"`

	// AccessCount tracks how often this memory has been retrieved.
	AccessCount int `json:"access_count"`
}

// Memory is a persistent key-value store for agent knowledge.
// Thread-safe for concurrent access.
type Memory struct {
	mu      sync.RWMutex
	entries map[string]*MemoryEntry // keyed by "category:key"
	path    string                  // file path for persistence
	dirty   bool                    // true if unsaved changes exist
}

// NewMemory creates a memory store. If path is non-empty, memories persist
// to that JSON file. Pass "" for in-memory only.
func NewMemory(path string) *Memory {
	return &Memory{
		entries: make(map[string]*MemoryEntry),
		path:    path,
	}
}

// LoadFromDisk reads the memory file into the store.
// No error if the file doesn't exist (fresh start).
func (m *Memory) LoadFromDisk() error {
	if m.path == "" {
		return nil
	}

	data, err := os.ReadFile(m.path)
	if os.IsNotExist(err) {
		return nil // fresh start
	}
	if err != nil {
		return fmt.Errorf("read memory file: %w", err)
	}

	var entries []*MemoryEntry
	if err := json.Unmarshal(data, &entries); err != nil {
		return fmt.Errorf("unmarshal memory: %w", err)
	}

	m.mu.Lock()
	defer m.mu.Unlock()

	for _, e := range entries {
		m.entries[e.Category+":"+e.Key] = e
	}
	m.dirty = false
	return nil
}

// SaveToDisk persists all memories to the file.
// Only writes if there are unsaved changes.
func (m *Memory) SaveToDisk() error {
	if m.path == "" {
		return nil
	}

	m.mu.RLock()
	if !m.dirty {
		m.mu.RUnlock()
		return nil
	}

	// Collect entries
	entries := make([]*MemoryEntry, 0, len(m.entries))
	for _, e := range m.entries {
		entries = append(entries, e)
	}
	m.mu.RUnlock()

	// Sort by category then key for stable output
	sort.Slice(entries, func(i, j int) bool {
		if entries[i].Category != entries[j].Category {
			return entries[i].Category < entries[j].Category
		}
		return entries[i].Key < entries[j].Key
	})

	data, err := json.MarshalIndent(entries, "", "  ")
	if err != nil {
		return fmt.Errorf("marshal memory: %w", err)
	}

	// Ensure directory exists
	dir := filepath.Dir(m.path)
	if err := os.MkdirAll(dir, 0o755); err != nil {
		return fmt.Errorf("create memory dir: %w", err)
	}

	// Atomic write
	tmp := m.path + ".tmp"
	if err := os.WriteFile(tmp, data, 0o644); err != nil {
		return fmt.Errorf("write memory: %w", err)
	}
	if err := os.Rename(tmp, m.path); err != nil {
		os.Remove(tmp)
		return fmt.Errorf("rename memory: %w", err)
	}

	m.mu.Lock()
	m.dirty = false
	m.mu.Unlock()

	return nil
}

// --- Core Operations ---

// Set stores or updates a memory entry.
func (m *Memory) Set(category, key, value string) {
	m.mu.Lock()
	defer m.mu.Unlock()

	fk := category + ":" + key
	now := time.Now()

	if existing, ok := m.entries[fk]; ok {
		existing.Value = value
		existing.UpdatedAt = now
	} else {
		m.entries[fk] = &MemoryEntry{
			Key:       key,
			Category:  category,
			Value:     value,
			CreatedAt: now,
			UpdatedAt: now,
		}
	}
	m.dirty = true
}

// Get retrieves a memory entry. Returns the value and whether it exists.
// Increments access count on hit.
func (m *Memory) Get(category, key string) (string, bool) {
	m.mu.Lock()
	defer m.mu.Unlock()

	fk := category + ":" + key
	e, ok := m.entries[fk]
	if !ok {
		return "", false
	}
	e.AccessCount++
	return e.Value, true
}

// Delete removes a memory entry. Returns true if it existed.
func (m *Memory) Delete(category, key string) bool {
	m.mu.Lock()
	defer m.mu.Unlock()

	fk := category + ":" + key
	if _, ok := m.entries[fk]; ok {
		delete(m.entries, fk)
		m.dirty = true
		return true
	}
	return false
}

// Has checks if a memory entry exists without incrementing access count.
func (m *Memory) Has(category, key string) bool {
	m.mu.RLock()
	defer m.mu.RUnlock()

	_, ok := m.entries[category+":"+key]
	return ok
}

// --- Category Operations ---

// List returns all entries in a category, sorted by key.
func (m *Memory) List(category string) []*MemoryEntry {
	m.mu.RLock()
	defer m.mu.RUnlock()

	var result []*MemoryEntry
	for _, e := range m.entries {
		if e.Category == category {
			result = append(result, e)
		}
	}

	sort.Slice(result, func(i, j int) bool {
		return result[i].Key < result[j].Key
	})

	return result
}

// Categories returns all distinct category names.
func (m *Memory) Categories() []string {
	m.mu.RLock()
	defer m.mu.RUnlock()

	seen := make(map[string]bool)
	for _, e := range m.entries {
		seen[e.Category] = true
	}

	cats := make([]string, 0, len(seen))
	for cat := range seen {
		cats = append(cats, cat)
	}
	sort.Strings(cats)
	return cats
}

// Size returns the total number of memory entries.
func (m *Memory) Size() int {
	m.mu.RLock()
	defer m.mu.RUnlock()
	return len(m.entries)
}

// --- Search ---

// Search finds entries where the value contains the query string.
// Returns entries sorted by access count (most accessed first).
func (m *Memory) Search(query string) []*MemoryEntry {
	m.mu.RLock()
	defer m.mu.RUnlock()

	var result []*MemoryEntry
	for _, e := range m.entries {
		if contains(e.Value, query) || contains(e.Key, query) {
			result = append(result, e)
		}
	}

	sort.Slice(result, func(i, j int) bool {
		return result[i].AccessCount > result[j].AccessCount
	})

	return result
}

// contains does case-insensitive substring check.
func contains(s, substr string) bool {
	// Simple lowercase comparison — good enough for memory search
	sl := make([]byte, len(s))
	for i := range s {
		c := s[i]
		if c >= 'A' && c <= 'Z' {
			c += 32
		}
		sl[i] = c
	}
	subl := make([]byte, len(substr))
	for i := range substr {
		c := substr[i]
		if c >= 'A' && c <= 'Z' {
			c += 32
		}
		subl[i] = c
	}
	return bytesContains(sl, subl)
}

// bytesContains checks if b contains sub.
func bytesContains(b, sub []byte) bool {
	if len(sub) == 0 {
		return true
	}
	if len(sub) > len(b) {
		return false
	}
	for i := 0; i <= len(b)-len(sub); i++ {
		match := true
		for j := range sub {
			if b[i+j] != sub[j] {
				match = false
				break
			}
		}
		if match {
			return true
		}
	}
	return false
}

// --- Snapshot ---

// MemorySnapshot is a read-only view of the memory state.
type MemorySnapshot struct {
	TotalEntries int
	Categories   []string
	Dirty        bool
	Path         string
}

// Snapshot returns a summary of the current memory state.
func (m *Memory) Snapshot() MemorySnapshot {
	m.mu.RLock()
	defer m.mu.RUnlock()

	return MemorySnapshot{
		TotalEntries: len(m.entries),
		Categories:   m.Categories(),
		Dirty:        m.dirty,
		Path:         m.path,
	}
}

// --- Agent Integration ---

// BuildPromptSummary creates a text summary of relevant memories
// for inclusion in the agent's prompt. Categories are included in order.
func (m *Memory) BuildPromptSummary(categories ...string) string {
	m.mu.RLock()
	defer m.mu.RUnlock()

	var b []byte
	for _, cat := range categories {
		entries := m.listLocked(cat)
		if len(entries) == 0 {
			continue
		}
		b = append(b, fmt.Sprintf("--- %s ---\n", cat)...)
		for _, e := range entries {
			b = append(b, fmt.Sprintf("  %s: %s\n", e.Key, e.Value)...)
		}
		b = append(b, '\n')
	}
	return string(b)
}

// listLocked returns entries in a category (caller must hold read lock).
func (m *Memory) listLocked(category string) []*MemoryEntry {
	var result []*MemoryEntry
	for _, e := range m.entries {
		if e.Category == category {
			result = append(result, e)
		}
	}
	sort.Slice(result, func(i, j int) bool {
		return result[i].Key < result[j].Key
	})
	return result
}

// ============================================================================
// CLOSING
// ============================================================================
// "These words, which I command thee this day, shall be in thine heart."
// — Deuteronomy 6:6-7
// Knowledge accumulated. Memory persisted. The agent learns.
