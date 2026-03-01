// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-statemachine-runtime
// Purpose: Runtime state persistence for CPI-SI state machine
// Biblical: Psalm 119:105 - "Thy word is a lamp unto my feet, and a light unto my path"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2025-12-21
// Updated: 2025-12-21
//
// v1.1.0 Changes:
//   - Added HebrewState, HebrewMeaning (7 Days of Creation)
//   - Added KSelector, KAlign (K:MORAL compass)
//   - Added RuntimeTaskList for TodoWrite integration
//
// Design Principle: References + Dynamic Data
//   - State references TOML keys (anchor_key, trajectory_section, command_key)
//   - Runtime contains only session-specific dynamic values
//   - Code reads TOML for full details using these reference keys
//
// Traces to: config/statemachine/runtime/*.jsonc
//
// ═══════════════════════════════════════════════════════════════════════════

package statemachine

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"os"
	"path/filepath"
	"time"

	"creativeworkzstudio.com/bereshit/word/work/pkg/util/fs/paths"
	"creativeworkzstudio.com/bereshit/word/work/pkg/foundation/types"
)

// ───────────────────────────────────────────────────────────────────────────
// RUNTIME STATE TYPES — Aliased from pkg/types (L0)
// ───────────────────────────────────────────────────────────────────────────

// Type aliases from L0 (pkg/types) - enables tiered architecture
type RuntimeState = types.RuntimeState
type RuntimeSession = types.RuntimeSession
type RuntimeTaskList = types.RuntimeTaskList
type RuntimeTrajectoryMetrics = types.RuntimeTrajectoryMetrics
type RuntimeTransition = types.RuntimeTransition

// ───────────────────────────────────────────────────────────────────────────
// RUNTIME PATH TYPES (statemachine-specific, not in L0)
// ───────────────────────────────────────────────────────────────────────────

// RuntimePath tracks anchors traversed during session
type RuntimePath struct {
	Schema  string `json:"$schema,omitempty"`
	Version string `json:"version"`

	// Session identity
	SessionID string `json:"session_id"`
	StartedAt string `json:"started_at"`
	EndedAt   string `json:"ended_at"`

	// Anchor path (references anchors.toml)
	Anchors []RuntimeAnchorVisit `json:"anchors"`

	// Trajectory history (references framework.toml)
	TrajectoryHistory []RuntimeTrajectoryVisit `json:"trajectory_history"`

	// Chronological events
	Events []RuntimeEvent `json:"events"`

	// Summary stats (computed on session end)
	Summary RuntimePathSummary `json:"summary"`
}

// RuntimeAnchorVisit records an anchor traversal
type RuntimeAnchorVisit struct {
	AnchorKey string `json:"anchor_key"` // key into anchors.toml
	EnteredAt string `json:"entered_at"`
	ExitedAt  string `json:"exited_at"`
}

// RuntimeTrajectoryVisit records a trajectory section visit
type RuntimeTrajectoryVisit struct {
	Section   string `json:"section"` // B.1, B.2, B.3, B.4
	EnteredAt string `json:"entered_at"`
	ExitedAt  string `json:"exited_at"`
}

// RuntimeEvent records a hook or state machine event
type RuntimeEvent struct {
	Type      string `json:"type"`      // hook_name, transition, etc.
	Timestamp string `json:"timestamp"`
	ToolName  string `json:"tool_name,omitempty"` // for tool hooks
	Section   string `json:"section,omitempty"`   // current trajectory section
}

// RuntimePathSummary holds computed stats for the session
type RuntimePathSummary struct {
	TotalAnchors          int `json:"total_anchors"`
	UniqueAnchors         int `json:"unique_anchors"`
	TotalEvents           int `json:"total_events"`
	TrajectoryTransitions int `json:"trajectory_transitions"`
	DurationMs            int `json:"duration_ms"`
}

// ───────────────────────────────────────────────────────────────────────────
// RUNTIME HISTORY TYPES (cross-session learning)
// ───────────────────────────────────────────────────────────────────────────

// RuntimeHistory tracks patterns across sessions for learning
type RuntimeHistory struct {
	Schema   string `json:"$schema,omitempty"`
	Version  string `json:"version"`
	Sessions []RuntimeSessionSummary `json:"sessions"`
	Aggregates RuntimeAggregates `json:"aggregates"`
	Meta RuntimeHistoryMeta `json:"meta"`
}

// RuntimeSessionSummary captures key metrics from a completed session
type RuntimeSessionSummary struct {
	SessionID     string `json:"session_id"`
	StartedAt     string `json:"started_at"`
	EndedAt       string `json:"ended_at"`
	FinalAnchor   string `json:"final_anchor"`
	FinalTrajectory string `json:"final_trajectory"`
	TotalAnchors  int     `json:"total_anchors"`
	UniqueAnchors int     `json:"unique_anchors"`
	TotalEvents   int     `json:"total_events"`
	HealthScore   float64 `json:"health_score"`
	KAlign        float64 `json:"k_align"`
}

// RuntimeAggregates tracks patterns across all sessions
type RuntimeAggregates struct {
	AnchorFrequencies      map[string]int     `json:"anchor_frequencies"`
	CommandFrequencies     map[string]int     `json:"command_frequencies"`
	TransitionPatterns     map[string]int     `json:"transition_patterns"`
	TrajectoryAvgTime      map[string]float64 `json:"trajectory_avg_time"`
}

// RuntimeHistoryMeta tracks history file metadata
type RuntimeHistoryMeta struct {
	CreatedAt        *string `json:"created_at"`
	LastUpdated      *string `json:"last_updated"`
	TotalSessions    int     `json:"total_sessions"`
	TotalCompactions int     `json:"total_compactions"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- State Initialization ---

// InitializeRuntimeState creates initial runtime state for a new session
func InitializeRuntimeState(sessionID, substrate, engine string) *RuntimeState {
	now := time.Now().Format(time.RFC3339)
	return &RuntimeState{
		Version:           "1.1.0",
		AnchorKey:         "present_present",
		TrajectorySection: "B.1",
		CommandKey:        "await",
		Session: RuntimeSession{
			ID:            sessionID,
			Substrate:     substrate,
			Engine:        engine,
			InitializedAt: now,
			LastUpdate:    now,
			PathLength:    0,
			HooksFired:    0,
			Active:        true,
			HealthScore:   0,
			// Hebrew state defaults to ORIGIN (Day 4 = center)
			HebrewState:   "yashar",
			HebrewMeaning: "EVEN",
			// K:MORAL starts at center (no direction)
			KSelector: 0,
			KAlign:    0.0,
			// Tasks start empty
			Tasks: RuntimeTaskList{},
		},
		TrajectoryMetrics: RuntimeTrajectoryMetrics{},
		LastTransition:    RuntimeTransition{},
	}
}

// InitializeRuntimePath creates initial runtime path for a new session
func InitializeRuntimePath(sessionID string) *RuntimePath {
	now := time.Now().Format(time.RFC3339)
	return &RuntimePath{
		Version:   "1.0.0",
		SessionID: sessionID,
		StartedAt: now,
		Anchors:   []RuntimeAnchorVisit{},
		TrajectoryHistory: []RuntimeTrajectoryVisit{
			{Section: "B.1", EnteredAt: now},
		},
		Events:  []RuntimeEvent{},
		Summary: RuntimePathSummary{},
	}
}

// --- State Persistence ---

// LoadRuntimeState reads state.jsonc from runtime directory
func LoadRuntimeState() (*RuntimeState, error) {
	statePath := paths.StateMachineRuntimeState()
	data, err := os.ReadFile(statePath)
	if err != nil {
		return nil, err
	}

	// Strip JSONC comments for parsing
	data = stripJSONComments(data)

	var state RuntimeState
	if err := json.Unmarshal(data, &state); err != nil {
		return nil, err
	}
	return &state, nil
}

// SaveRuntimeState writes state.jsonc to runtime directory
func SaveRuntimeState(state *RuntimeState) error {
	statePath := paths.StateMachineRuntimeState()

	// Update last_update timestamp
	state.Session.LastUpdate = time.Now().Format(time.RFC3339)

	data, err := json.MarshalIndent(state, "", "  ")
	if err != nil {
		return err
	}

	// Ensure directory exists
	if err := os.MkdirAll(filepath.Dir(statePath), 0755); err != nil {
		return err
	}

	return os.WriteFile(statePath, data, 0644)
}

// LoadRuntimePath reads path.jsonc from runtime directory
func LoadRuntimePath() (*RuntimePath, error) {
	pathFile := paths.StateMachineRuntimePath()
	data, err := os.ReadFile(pathFile)
	if err != nil {
		return nil, err
	}

	data = stripJSONComments(data)

	var path RuntimePath
	if err := json.Unmarshal(data, &path); err != nil {
		return nil, err
	}
	return &path, nil
}

// SaveRuntimePath writes path.jsonc to runtime directory
func SaveRuntimePath(path *RuntimePath) error {
	pathFile := paths.StateMachineRuntimePath()

	data, err := json.MarshalIndent(path, "", "  ")
	if err != nil {
		return err
	}

	if err := os.MkdirAll(filepath.Dir(pathFile), 0755); err != nil {
		return err
	}

	return os.WriteFile(pathFile, data, 0644)
}

// LoadRuntimeHistory reads history.jsonc from runtime directory
func LoadRuntimeHistory() (*RuntimeHistory, error) {
	historyPath := paths.StateMachineRuntimeHistory()
	data, err := os.ReadFile(historyPath)
	if err != nil {
		return nil, err
	}

	data = stripJSONComments(data)

	var history RuntimeHistory
	if err := json.Unmarshal(data, &history); err != nil {
		return nil, err
	}
	return &history, nil
}

// SaveRuntimeHistory writes history.jsonc to runtime directory
func SaveRuntimeHistory(history *RuntimeHistory) error {
	historyPath := paths.StateMachineRuntimeHistory()

	// Update last_updated timestamp
	now := time.Now().Format(time.RFC3339)
	history.Meta.LastUpdated = &now

	data, err := json.MarshalIndent(history, "", "  ")
	if err != nil {
		return err
	}

	if err := os.MkdirAll(filepath.Dir(historyPath), 0755); err != nil {
		return err
	}

	return os.WriteFile(historyPath, data, 0644)
}

// RecordSessionSnapshot writes current session state to history for cross-session learning
func RecordSessionSnapshot(sessionID string, state *RuntimeState, path *RuntimePath) error {
	// Load existing history
	history, err := LoadRuntimeHistory()
	if err != nil {
		// Initialize empty history if file doesn't exist
		now := time.Now().Format(time.RFC3339)
		history = &RuntimeHistory{
			Version:  "1.0.0",
			Sessions: []RuntimeSessionSummary{},
			Aggregates: RuntimeAggregates{
				AnchorFrequencies:  make(map[string]int),
				CommandFrequencies: make(map[string]int),
				TransitionPatterns: make(map[string]int),
				TrajectoryAvgTime:  map[string]float64{"B.1": 0, "B.2": 0, "B.3": 0, "B.4": 0},
			},
			Meta: RuntimeHistoryMeta{
				CreatedAt:     &now,
				TotalSessions: 0,
			},
		}
	}

	// Create session summary
	now := time.Now().Format(time.RFC3339)
	summary := RuntimeSessionSummary{
		SessionID:       sessionID,
		StartedAt:       path.StartedAt,
		EndedAt:         now,
		FinalAnchor:     state.AnchorKey,
		FinalTrajectory: state.TrajectorySection,
		TotalAnchors:    path.Summary.TotalAnchors,
		UniqueAnchors:   path.Summary.UniqueAnchors,
		TotalEvents:     path.Summary.TotalEvents,
		HealthScore:     state.Session.HealthScore,
		KAlign:          state.Session.KAlign,
	}

	// Append to sessions (keep last 100 for learning)
	history.Sessions = append(history.Sessions, summary)
	if len(history.Sessions) > 100 {
		history.Sessions = history.Sessions[len(history.Sessions)-100:]
	}

	// Update aggregates
	history.Aggregates.AnchorFrequencies[state.AnchorKey]++
	history.Aggregates.CommandFrequencies[state.CommandKey]++

	// Track trajectory transitions
	if state.LastTransition.FromSection != "" && state.LastTransition.ToSection != "" {
		transKey := state.LastTransition.FromSection + "->" + state.LastTransition.ToSection
		history.Aggregates.TransitionPatterns[transKey]++
	}

	// Update meta
	history.Meta.TotalSessions++
	history.Meta.TotalCompactions++

	return SaveRuntimeHistory(history)
}

// --- State Updates ---

// RecordAnchorVisit adds an anchor visit to the path
func (p *RuntimePath) RecordAnchorVisit(anchorKey string) {
	now := time.Now().Format(time.RFC3339)

	// Close previous anchor if any
	if len(p.Anchors) > 0 {
		last := &p.Anchors[len(p.Anchors)-1]
		if last.ExitedAt == "" {
			last.ExitedAt = now
		}
	}

	// Add new anchor visit
	p.Anchors = append(p.Anchors, RuntimeAnchorVisit{
		AnchorKey: anchorKey,
		EnteredAt: now,
	})

	p.Summary.TotalAnchors++
}

// RecordTrajectoryTransition records a trajectory section change
func (p *RuntimePath) RecordTrajectoryTransition(toSection string) {
	now := time.Now().Format(time.RFC3339)

	// Close previous trajectory section
	if len(p.TrajectoryHistory) > 0 {
		last := &p.TrajectoryHistory[len(p.TrajectoryHistory)-1]
		if last.ExitedAt == "" {
			last.ExitedAt = now
		}
	}

	// Add new trajectory section
	p.TrajectoryHistory = append(p.TrajectoryHistory, RuntimeTrajectoryVisit{
		Section:   toSection,
		EnteredAt: now,
	})

	p.Summary.TrajectoryTransitions++
}

// RecordEvent adds an event to the chronological log
func (p *RuntimePath) RecordEvent(eventType, toolName, section string) {
	p.Events = append(p.Events, RuntimeEvent{
		Type:      eventType,
		Timestamp: time.Now().Format(time.RFC3339),
		ToolName:  toolName,
		Section:   section,
	})
	p.Summary.TotalEvents++
}

// --- Helper Functions ---

// stripJSONComments removes // and /* */ comments from JSONC
func stripJSONComments(data []byte) []byte {
	// Simple implementation: just parse and re-marshal
	// For production, use a proper JSONC parser
	// This naive approach works for our structured files

	result := make([]byte, 0, len(data))
	inString := false
	inLineComment := false
	inBlockComment := false

	for i := 0; i < len(data); i++ {
		c := data[i]

		if inLineComment {
			if c == '\n' {
				inLineComment = false
				result = append(result, c)
			}
			continue
		}

		if inBlockComment {
			if c == '*' && i+1 < len(data) && data[i+1] == '/' {
				inBlockComment = false
				i++ // skip /
			}
			continue
		}

		if c == '"' && (i == 0 || data[i-1] != '\\') {
			inString = !inString
		}

		if !inString {
			if c == '/' && i+1 < len(data) {
				if data[i+1] == '/' {
					inLineComment = true
					continue
				}
				if data[i+1] == '*' {
					inBlockComment = true
					i++ // skip *
					continue
				}
			}
		}

		result = append(result, c)
	}

	return result
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Runtime state follows config-driven design:
//   - state.jsonc: Current position (anchor_key, trajectory_section, command_key)
//   - path.jsonc: Session audit trail (anchors traversed, events)
//   - history.jsonc: Cross-session patterns (frequencies, averages)
//
// Keys reference L2 vocabulary TOMLs:
//   - anchor_key → anchors.toml
//   - trajectory_section → framework.toml
//   - command_key → commands.toml
//
// Written by: SessionStart (init), PostToolUse (update), Stop (finalize)
// Read by: Statusline (display), PreToolUse (validation)
//
