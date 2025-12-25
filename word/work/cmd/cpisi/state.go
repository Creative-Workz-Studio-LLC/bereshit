// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-cmd-cpisi-state
// Purpose: State file contract — the API between all views
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-20
//
// The state file IS the API. All views read/write the same truth.
// Location: ~/.claude/state/position.json
//
// ═══════════════════════════════════════════════════════════════════════════

package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"os"
	"path/filepath"
	"time"

	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Constants ---

const stateDir = ".claude/state"
const stateFile = "position.json"

// --- State Data Contract ---

// StateData represents persisted state for hook integration.
// This is the CONTRACT that all views read/write.
type StateData struct {
	Position  string    `json:"position"`   // e.g., "PRESENT-PRESENT"
	Anchor    string    `json:"anchor"`     // e.g., "Genesis 1:1"
	Command   string    `json:"command"`    // "HALT", "AWAIT", "PROCEED"
	Operator  string    `json:"operator"`   // e.g., "@", "<-", "+"
	N         float64   `json:"n"`          // Ψ(N) input
	X         float64   `json:"x"`          // Coordinate X
	Y         float64   `json:"y"`          // Coordinate Y
	Z         float64   `json:"z"`          // Coordinate Z
	UpdatedAt time.Time `json:"updated_at"` // When last changed
	UpdatedBy string    `json:"updated_by"` // e.g., "hook:session/start", "cli:set", "tui:navigate"
}

// --- Path Functions ---

// getStatePath returns the full path to the state file.
func getStatePath() string {
	home, _ := os.UserHomeDir()
	return filepath.Join(home, stateDir, stateFile)
}

// ensureStateDir creates the state directory if it doesn't exist.
func ensureStateDir() error {
	home, _ := os.UserHomeDir()
	return os.MkdirAll(filepath.Join(home, stateDir), 0755)
}

// --- State I/O ---

// saveState writes the current state machine state to the state file.
func saveState(sm *statemachine.StateMachine, updatedBy string) error {
	if err := ensureStateDir(); err != nil {
		return err
	}

	data := StateData{
		Position:  sm.Current.Position.Name,
		Anchor:    sm.Current.Anchor,
		Command:   sm.GetCurrentCommand().String(),
		Operator:  string(sm.GetCurrentOperator()),
		N:         sm.Current.N,
		X:         sm.Current.Coordinates.X,
		Y:         sm.Current.Coordinates.Y,
		Z:         sm.Current.Coordinates.Z,
		UpdatedAt: time.Now(),
		UpdatedBy: updatedBy,
	}

	jsonData, err := json.MarshalIndent(data, "", "  ")
	if err != nil {
		return err
	}

	return os.WriteFile(getStatePath(), jsonData, 0644)
}

// loadState reads the current state from the state file.
func loadState() (*StateData, error) {
	data, err := os.ReadFile(getStatePath())
	if err != nil {
		return nil, err
	}

	var state StateData
	if err := json.Unmarshal(data, &state); err != nil {
		return nil, err
	}

	return &state, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// The state file is the foundation (0) of the multi-accessible architecture.
// All views build on (+1) this contract. All trace back (-1) to it.
//
// ═══════════════════════════════════════════════════════════════════════════
