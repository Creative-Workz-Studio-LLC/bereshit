// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-statemachine-choice-history
// Purpose: Choice history persistence for CPI-SI mental construct
// Biblical: "Choose you this day whom ye will serve" — Joshua 24:15
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-21
//
// Traces to:
//   - HALT_09 (MECHANISM_WHEEL) — choices cycle through door/key/outcome
//   - mental_construct.schema.toml — choice tracking schema
//   - pkg/cognition/choice.go — ChoiceRecord and ChoiceHistory types
//
// Design:
//   - Persists choice history to choice_history.jsonc
//   - Enables pattern detection across choices
//   - Tracks K:ALIGN evolution through session
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

	"github.com/creativeworkzstudio/claude-global/pkg/util/fs/paths"
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Choice History Persistence ---

// LoadChoiceHistory reads choice_history.jsonc from runtime directory
func LoadChoiceHistory() (*types.ChoiceHistory, error) {
	historyPath := paths.StateMachineChoiceHistory()
	data, err := os.ReadFile(historyPath)
	if err != nil {
		return nil, err
	}

	// Strip JSONC comments for parsing
	data = stripJSONComments(data)

	var history types.ChoiceHistory
	if err := json.Unmarshal(data, &history); err != nil {
		return nil, err
	}
	return &history, nil
}

// SaveChoiceHistory writes choice_history.jsonc to runtime directory
func SaveChoiceHistory(history *types.ChoiceHistory) error {
	historyPath := paths.StateMachineChoiceHistory()

	// Update last_updated timestamp
	history.LastUpdated = time.Now()

	data, err := json.MarshalIndent(history, "", "  ")
	if err != nil {
		return err
	}

	// Ensure directory exists
	if err := os.MkdirAll(filepath.Dir(historyPath), 0755); err != nil {
		return err
	}

	return os.WriteFile(historyPath, data, 0644)
}

// LoadOrCreateChoiceHistory loads existing history or creates new one for session
func LoadOrCreateChoiceHistory(sessionID string) (*types.ChoiceHistory, error) {
	history, err := LoadChoiceHistory()
	if err != nil {
		// File doesn't exist or is corrupted — create new
		return types.NewChoiceHistory(sessionID), nil
	}

	// Check if this is the same session
	if history.SessionID == sessionID {
		return history, nil
	}

	// Different session — start fresh
	return types.NewChoiceHistory(sessionID), nil
}

// --- Choice Recording Functions ---

// RecordChoice adds a new choice to the history and persists it
// This is the main entry point for recording a choice with full context
func RecordChoice(
	sessionID string,
	key types.KeyValue,
	forWhat string,
	intent string,
	currentZ float64,
	currentKAlign float64,
	kSelector int,
) (*types.ChoiceRecord, error) {
	// Load or create history
	history, err := LoadOrCreateChoiceHistory(sessionID)
	if err != nil {
		return nil, err
	}

	// Create choice context
	ctx := types.NewChoiceContext(key, forWhat, intent)
	ctx.PositionAtChoice = GetHebrewStateNameFromZ(currentZ)
	ctx.KAtChoice = kSelector
	ctx.ZAtChoice = currentZ

	// Create record
	seqNum := len(history.Choices) + 1
	record := types.NewChoiceRecord(sessionID, seqNum, ctx)

	// Add to history (result will be filled in later via CompleteChoice)
	history.AddChoice(*record)

	// Persist
	if err := SaveChoiceHistory(history); err != nil {
		return nil, err
	}

	return record, nil
}

// CompleteChoice fills in the result for an existing choice and updates K:ALIGN
func CompleteChoice(
	sessionID string,
	choiceID string,
	ledTo string,
	haltType types.HaltType,
	toolOutcome string,
	trueScore float64,
	currentKAlign float64,
) error {
	// Load history
	history, err := LoadChoiceHistory()
	if err != nil {
		return err
	}

	// Find the choice to complete
	for i := range history.Choices {
		if history.Choices[i].ID == choiceID {
			// Create result
			result := types.NewChoiceResult(ledTo, haltType, toolOutcome)

			// Complete the record with impact calculation
			history.Choices[i].Complete(result, trueScore, currentKAlign)

			// Persist
			return SaveChoiceHistory(history)
		}
	}

	return nil // Choice not found — no error, may have been from different session
}

// --- Helper Functions ---

// GetHebrewStateNameFromZ returns the Hebrew state name for a given Z value
func GetHebrewStateNameFromZ(z float64) string {
	switch {
	case z <= -0.75:
		return "ESTABLISHED"
	case z <= -0.25:
		return "MEMORY" // or REFLECTION depending on k
	case z <= 0.25:
		return "ORIGIN"
	case z <= 0.75:
		return "PLANNING" // or PREPARATION depending on k
	default:
		return "ASPIRATION"
	}
}

// GetLatestKAlign returns the K:ALIGN value from the most recent completed choice
func GetLatestKAlign(sessionID string) (float64, error) {
	history, err := LoadOrCreateChoiceHistory(sessionID)
	if err != nil {
		return 0.0, err
	}

	last := history.GetLastChoice()
	if last == nil {
		return 0.0, nil // No choices yet, start at center
	}

	return last.Impact.KAlignAfter, nil
}

// GetChoiceSummary returns summary statistics for the current session
func GetChoiceSummary(sessionID string) (*types.ChoiceSummary, error) {
	history, err := LoadOrCreateChoiceHistory(sessionID)
	if err != nil {
		return nil, err
	}

	return &history.Summary, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Choice history persistence enables:
//   - Pattern detection across choices ("When I choose +1 for X, it leads to Y")
//   - K:ALIGN evolution tracking (moral compass movement through session)
//   - Session summary statistics (expansion/lateral/finality counts)
//   - HALT completion tracking (proper Sabbath rest moments)
//
// The choice narrative:
//   "I chose [key] FOR [context] and it LED TO [result]"
//   "It tends to lead me toward [God/self]"
//
// "Choose you this day whom ye will serve" — Joshua 24:15
//
