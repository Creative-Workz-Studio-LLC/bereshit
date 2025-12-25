// ============================================================================
// METADATA
// ============================================================================
// Session Time Library - Core session state and timing functionality
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// Biblical Foundation
//
// Scripture: "To everything there is a season" - Ecclesiastes 3:1 (WEB)
// Principle: Time awareness enables faithful work and sustainable rhythms
// Anchor: Session state tracking provides temporal consciousness and continuity
//
// CPI-SI Identity
//
// This library is part of the CPI-SI system infrastructure providing session
// state management as foundational capability for both system commands and hooks.
//
// Authorship & Lineage
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Version: 2.0.0
// Last Modified: 2025-11-12 - Consolidated as authoritative source for SessionState
//
// Purpose & Function
//
// Authoritative source for SessionState type and session state operations.
// Provides session initialization, state reading, compaction tracking, and
// time calculations. Used by both system commands and hooks for session management.
//
// Blocking Status
//
// Non-blocking: All operations return errors for caller handling.
//
// Usage & Integration
//
// Primary users:
//   - system/runtime/cmd/session-time (session timing command)
//   - hooks/lib/session (session orchestration wrapper)
//   - Any component needing session state access
//
// Example:
//   import "system/lib/sessiontime"
//   state, err := sessiontime.ReadSession()
//   count, err := sessiontime.IncrementCompactionCount()
//
// Dependencies
//
// Standard library: encoding/json, os, path/filepath, time
// External: system/lib/config (for session initialization)
//
// Health Scoring Map (Total = 100 points)
//
// This library provides foundational session state operations:
//   Read Operations: +40 points (reading session state successfully)
//   Write Operations: +40 points (updating session state successfully)
//   Path Resolution: +10 points (resolving session file path correctly)
//   Error Handling: +10 points (graceful error propagation)
//
// Scoring reflects: Read (40) + Write (40) + Path (10) + Errors (10) = 100

package sessiontime

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"time"

	"system/lib/config" // Config loading for session initialization
)

// SessionState represents the current session state with richer structure
//
// Purpose: Track session metadata with config inheritance
// Usage: Read/written to ~/.claude/cpi-si/system/data/session/current.json
// Fields match migration template for richer session context
//
// AUTHORITATIVE DEFINITION: This is the single source of truth for SessionState.
// All other components (commands, hooks) should import this type.
type SessionState struct {
	// Core identity (from configs)
	SessionID  string `json:"session_id"`
	InstanceID string `json:"instance_id"`
	UserID     string `json:"user_id"`

	// Timing
	StartTime      time.Time `json:"start_time"`
	StartUnix      int64     `json:"start_unix"`
	StartFormatted string    `json:"start_formatted"`

	// State tracking
	CompactionCount int    `json:"compaction_count"`
	LastActivity    string `json:"last_activity,omitempty"`
	SessionPhase    string `json:"session_phase"` // active | idle | consolidating | ending

	// Context (inherited from configs)
	WorkContext    string `json:"work_context,omitempty"`
	ProjectID      string `json:"project_id,omitempty"`
	CircadianPhase string `json:"circadian_phase,omitempty"` // morning | afternoon | evening | night

	// Quality indicators
	QualityIndicators struct {
		TasksCompleted int `json:"tasks_completed"`
		Breakthroughs  int `json:"breakthroughs"`
		Struggles      int `json:"struggles"`
	} `json:"quality_indicators"`

	// State metadata (inherited from instance config)
	StateMetadata struct {
		ThinkingEnabled bool `json:"thinking_enabled"`
		PlanMode        bool `json:"plan_mode"`
		AutoAccept      bool `json:"auto_accept"`
	} `json:"state_metadata"`

	// Inherited preferences (from user/instance configs)
	InheritedPreferences struct {
		Workflow       string   `json:"workflow,omitempty"`
		ThinkingStyle  string   `json:"thinking_style,omitempty"`
		UserTimezone   string   `json:"user_timezone,omitempty"`
		WorkHours      []string `json:"work_hours,omitempty"`
		ProjectType    string   `json:"project_type,omitempty"`
		TechnicalLangs []string `json:"technical_languages,omitempty"`
	} `json:"inherited_preferences"`

	// Extensions - discovery space
	Extensions map[string]interface{} `json:"extensions,omitempty"`
}

// ============================================================================
// BODY
// ============================================================================
// Organizational Chart
//
// This library provides 6 functions organized as:
//
// PUBLIC API (5 functions):
//   - InitSession() - Initialize new session with config inheritance
//   - ReadSession() - Read current session state
//   - IncrementCompactionCount() - Increment and return compaction count
//   - GetCompactionCount() - Get current compaction count
//   - CalculateElapsed() - Calculate elapsed time since session start
//   - FormatDuration() - Format duration in human-readable form
//
// HELPERS (1 function):
//   - getSessionPath() - Get path to session state file
//
// ============================================================================

// Helper: getSessionPath returns the correct path to the session state file
// Loads path from paths.toml for config-driven operation
func getSessionPath() string {
	// Load path from config (paths.toml)
	path, err := config.GetSessionPath()
	if err != nil {
		// Fallback to hardcoded path if config unavailable
		homeDir, _ := os.UserHomeDir()
		if homeDir == "" {
			homeDir = os.Getenv("HOME")
		}
		return filepath.Join(homeDir, ".claude/cpi-si/system/data/session/current.json")
	}
	return path
}

// InitSession creates a new session state file with config inheritance
//
// Parameters:
//   username - User ID (e.g., "seanje-lenox-wise")
//   instanceID - Instance ID (e.g., "nova_dawn")
//   projectID - Optional project ID (can be empty)
//
// Returns:
//   error - nil on success, error if initialization fails
//
// Behavior:
//   1. Loads merged session context from user/instance/project configs
//   2. Determines circadian phase from current hour
//   3. Creates SessionState with inherited preferences
//   4. Ensures directory exists
//   5. Writes state file to ~/.claude/cpi-si/system/data/session/current.json
func InitSession(username, instanceID, projectID string) error {
	// Load merged session context from configs
	ctx, err := config.GetSessionContext(username, instanceID, projectID)
	if err != nil {
		return fmt.Errorf("failed to get session context from configs: %w", err)
	}

	now := time.Now()
	sessionID := now.Format("2006-01-02_1504")

	// Determine circadian phase from current hour
	hour := now.Hour()
	var circadianPhase string
	switch {
	case hour >= 6 && hour < 12:
		circadianPhase = "morning"
	case hour >= 12 && hour < 17:
		circadianPhase = "afternoon"
	case hour >= 17 && hour < 21:
		circadianPhase = "evening"
	default:
		circadianPhase = "night"
	}

	// Create richer session state from config inheritance
	state := SessionState{
		// Core identity (from configs)
		SessionID:  sessionID,
		InstanceID: ctx.InstanceID,
		UserID:     ctx.UserID,

		// Timing
		StartTime:      now,
		StartUnix:      now.Unix(),
		StartFormatted: now.Format("Mon Jan 02, 2006 at 15:04:05"),

		// State
		CompactionCount: 0,
		SessionPhase:    "active",
		CircadianPhase:  circadianPhase,

		// Context (from configs)
		WorkContext: ctx.WorkContext,
		ProjectID:   ctx.ProjectID,
	}

	// Initialize quality indicators
	state.QualityIndicators.TasksCompleted = 0
	state.QualityIndicators.Breakthroughs = 0
	state.QualityIndicators.Struggles = 0

	// Initialize state metadata (defaults)
	state.StateMetadata.ThinkingEnabled = false
	state.StateMetadata.PlanMode = false
	state.StateMetadata.AutoAccept = false

	// Inherited preferences (from configs)
	state.InheritedPreferences.UserTimezone = ctx.UserTimezone
	state.InheritedPreferences.ProjectType = ctx.ProjectType
	state.InheritedPreferences.ThinkingStyle = ctx.LearningStyle // Instance thinking style
	state.InheritedPreferences.Workflow = ctx.ProblemSolving      // Instance problem-solving approach

	// Ensure directory exists
	sessionPath := getSessionPath()
	dir := filepath.Dir(sessionPath)
	if err := os.MkdirAll(dir, 0755); err != nil {
		return fmt.Errorf("failed to create session directory: %w", err)
	}

	// Write state file
	data, err := json.MarshalIndent(state, "", "  ")
	if err != nil {
		return fmt.Errorf("failed to marshal session state: %w", err)
	}

	if err := os.WriteFile(sessionPath, data, 0644); err != nil {
		return fmt.Errorf("failed to write session state: %w", err)
	}

	return nil
}

// ReadSession reads the current session state
//
// Returns:
//   *SessionState - Current session state
//   error - nil on success, error if reading fails
//
// Behavior:
//   1. Resolves session file path
//   2. Reads session state JSON
//   3. Unmarshals into SessionState struct
//   4. Returns pointer to state
func ReadSession() (*SessionState, error) {
	sessionPath := getSessionPath()

	data, err := os.ReadFile(sessionPath)
	if err != nil {
		return nil, fmt.Errorf("failed to read session state: %w", err)
	}

	var state SessionState
	if err := json.Unmarshal(data, &state); err != nil {
		return nil, fmt.Errorf("failed to parse session state: %w", err)
	}

	return &state, nil
}

// IncrementCompactionCount increments compaction count and returns new value
//
// Returns:
//   int - New compaction count after increment
//   error - nil on success, error if operation fails
//
// Behavior:
//   1. Reads current session state
//   2. Increments CompactionCount field
//   3. Writes updated state back to file
//   4. Returns new count
func IncrementCompactionCount() (int, error) {
	sessionPath := getSessionPath()

	// Read existing state
	data, err := os.ReadFile(sessionPath)
	if err != nil {
		return 0, fmt.Errorf("failed to read session state: %w", err)
	}

	var state SessionState
	if err := json.Unmarshal(data, &state); err != nil {
		return 0, fmt.Errorf("failed to parse session state: %w", err)
	}

	// Increment compaction count
	state.CompactionCount++

	// Write back
	updatedData, err := json.MarshalIndent(state, "", "  ")
	if err != nil {
		return 0, fmt.Errorf("failed to marshal session state: %w", err)
	}

	if err := os.WriteFile(sessionPath, updatedData, 0644); err != nil {
		return 0, fmt.Errorf("failed to write session state: %w", err)
	}

	return state.CompactionCount, nil
}

// GetCompactionCount returns current compaction count from session state
//
// Returns:
//   int - Current compaction count
//   error - nil on success, error if reading fails
//
// Behavior:
//   1. Reads current session state
//   2. Extracts CompactionCount field
//   3. Returns count
func GetCompactionCount() (int, error) {
	state, err := ReadSession()
	if err != nil {
		return 0, err
	}
	return state.CompactionCount, nil
}

// CalculateElapsed returns elapsed time since session start
//
// Parameters:
//   state - Session state containing start time
//
// Returns:
//   time.Duration - Elapsed time since session start
//
// Behavior:
//   Calculates time.Since(state.StartTime)
func CalculateElapsed(state *SessionState) time.Duration {
	return time.Since(state.StartTime)
}

// FormatDuration formats a duration in human-readable form
//
// Parameters:
//   d - Duration to format
//
// Returns:
//   string - Formatted duration (e.g., "2h15m", "45m30s", "15s")
//
// Behavior:
//   - < 60s: "15s"
//   - < 1h: "45m30s"
//   - < 1d: "2h15m"
//   - >= 1d: "1d3h"
func FormatDuration(d time.Duration) string {
	seconds := int(d.Seconds())

	if seconds < 60 {
		return fmt.Sprintf("%ds", seconds)
	} else if seconds < 3600 {
		minutes := seconds / 60
		secs := seconds % 60
		return fmt.Sprintf("%dm%ds", minutes, secs)
	} else if seconds < 86400 {
		hours := seconds / 3600
		minutes := (seconds % 3600) / 60
		return fmt.Sprintf("%dh%dm", hours, minutes)
	}

	days := seconds / 86400
	hours := (seconds % 86400) / 3600
	return fmt.Sprintf("%dd%dh", days, hours)
}

// ============================================================================
// CLOSING
// ============================================================================
// Authoritative Session State Library
//
// This library is the SINGLE SOURCE OF TRUTH for SessionState type and
// session state operations within the CPI-SI system.
//
// Exported Types:
//   - SessionState - Richer session state structure with config inheritance
//
// Exported Functions:
//   - InitSession(username, instanceID, projectID string) error
//   - ReadSession() (*SessionState, error)
//   - IncrementCompactionCount() (int, error)
//   - GetCompactionCount() (int, error)
//   - CalculateElapsed(state *SessionState) time.Duration
//   - FormatDuration(d time.Duration) string
//
// File Path:
//   ~/.claude/cpi-si/system/data/session/current.json
//
// Usage by Commands:
//   system/runtime/cmd/session-time MUST import this library
//   DO NOT duplicate SessionState definition in commands
//
// Usage by Hooks:
//   hooks/lib/session SHOULD import this library
//   Hooks provide orchestration wrapper around system functionality
//
// Config Inheritance Flow:
//   User Config → Instance Config → Project Config
//        ↓              ↓                  ↓
//   SessionState (current.json with richer structure)
//        ↓
//   Activity Events (inherit session context)
//
// Consolidation History:
//   2025-11-12 - Consolidated from 3 duplicate definitions:
//     - system/lib/sessiontime/sessiontime.go (this file - now authoritative)
//     - system/runtime/cmd/session-time/session-time.go (removed duplicate)
//     - hooks/lib/session/state.go (removed duplicate)
//
// Architectural Pattern:
//   SYSTEM DEFINES → Commands/Hooks USE
//   This library is foundation, not application
//
// Modification Policy:
//   Changes to SessionState structure require:
//     1. Update this authoritative definition
//     2. Verify all importers compile
//     3. Update session state migration if needed
//     4. Document changes in config inheritance docs
//
// Related Documentation:
//   - Session state migration: system/data/session/README.md
//   - Config inheritance: system/lib/config/README.md
//   - Session timing: hooks/docs/session-time-api.md
