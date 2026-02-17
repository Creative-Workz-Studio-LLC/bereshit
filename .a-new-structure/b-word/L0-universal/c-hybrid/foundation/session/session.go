//omni:code --go -library
//omni:key B-L0-foundation-session
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-foundation-session
// Purpose: Universal session management — ID, paths, logs, duration
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-16
//
// Promoted from L3-cpisi to L0-universal as shared infrastructure.
// Any layer can track session state — this is not CPI-SI specific.
//
// ============================================================================

package session

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

// Current represents the current session metadata
type Current struct {
	SessionID   string    `json:"session_id"`
	StartTime   time.Time `json:"start_time"`
	Compactions int       `json:"compactions,omitempty"`
}

// LogEntry represents a session log entry
type LogEntry struct {
	Timestamp time.Time `json:"timestamp"`
	Type      string    `json:"type"` // "task", "note", "tool", "milestone"
	Content   string    `json:"content"`
}

// Log represents the session log
type Log struct {
	SessionID string     `json:"session_id"`
	Entries   []LogEntry `json:"entries"`
}

// Paths holds session-related paths
type Paths struct {
	SessionDir     string
	CurrentSession string
	SessionLog     string
	PatternFile    string
}

// ============================================================================
// BODY
// ============================================================================

// --- Path Helpers ---

// DefaultPaths returns default session paths (~/.claude/session/)
func DefaultPaths() Paths {
	home, _ := os.UserHomeDir()
	sessionDir := filepath.Join(home, ".claude", "session")
	return Paths{
		SessionDir:     sessionDir,
		CurrentSession: filepath.Join(sessionDir, "current.json"),
		SessionLog:     filepath.Join(sessionDir, "current-log.json"),
		PatternFile:    filepath.Join(sessionDir, "patterns.json"),
	}
}

// NewPaths creates paths with a custom session directory (relative to home)
func NewPaths(sessionDir string) Paths {
	home, _ := os.UserHomeDir()
	fullDir := filepath.Join(home, sessionDir)
	return Paths{
		SessionDir:     fullDir,
		CurrentSession: filepath.Join(fullDir, "current.json"),
		SessionLog:     filepath.Join(fullDir, "current-log.json"),
		PatternFile:    filepath.Join(fullDir, "patterns.json"),
	}
}

// --- Session Loading ---

// LoadCurrent reads the current session file
func LoadCurrent(paths Paths) (*Current, error) {
	data, err := os.ReadFile(paths.CurrentSession)
	if err != nil {
		return nil, fmt.Errorf("no active session found: %w", err)
	}

	var session Current
	if err := json.Unmarshal(data, &session); err != nil {
		return nil, fmt.Errorf("invalid session data: %w", err)
	}

	return &session, nil
}

// LoadCurrentDefault loads session with default paths
func LoadCurrentDefault() (*Current, error) {
	return LoadCurrent(DefaultPaths())
}

// LoadLog reads the session log file
func LoadLog(paths Paths) (*Log, error) {
	data, err := os.ReadFile(paths.SessionLog)
	if err != nil {
		return nil, fmt.Errorf("no session log found: %w", err)
	}

	var log Log
	if err := json.Unmarshal(data, &log); err != nil {
		return nil, fmt.Errorf("invalid session log: %w", err)
	}

	return &log, nil
}

// --- Duration Helpers ---

// Elapsed returns duration since session start
func (c *Current) Elapsed() time.Duration {
	return time.Since(c.StartTime)
}

// ElapsedMinutes returns minutes since session start
func (c *Current) ElapsedMinutes() int {
	return int(c.Elapsed().Minutes())
}

// FormatDuration returns human-readable duration
func FormatDuration(d time.Duration) string {
	hours := int(d.Hours())
	minutes := int(d.Minutes()) % 60

	if hours > 0 {
		return fmt.Sprintf("%dh %dm", hours, minutes)
	}
	return fmt.Sprintf("%d minutes", minutes)
}

// ============================================================================
// CLOSING
// ============================================================================
// Universal session management — promoted from L3 to serve all layers.
