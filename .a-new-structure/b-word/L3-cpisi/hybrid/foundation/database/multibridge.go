// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-multibridge
// Purpose: MultiBridge dispatches operations to correct domain DBs + JSONC sync
// Biblical: Ecclesiastes 4:12 - "A threefold cord is not quickly broken"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// Philosophy:
//   Evolves the original Bridge pattern to work with 5 domain databases.
//   Each operation routes to the correct domain while maintaining
//   live JSONC file synchronization for human readability.
//
// ═══════════════════════════════════════════════════════════════════════════

package database

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"time"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/cognition"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/growth"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/sessions"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/types"
	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/paths"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - MultiBridge
// ═══════════════════════════════════════════════════════════════════════════

// MultiBridge synchronizes between live JSONC files and domain databases
// Routes operations to the correct domain database while keeping
// human-readable files in sync.
type MultiBridge struct {
	mdb *MultiDB
}

// NewMultiBridge creates a new multi-database bridge
func NewMultiBridge(mdb *MultiDB) *MultiBridge {
	return &MultiBridge{mdb: mdb}
}

// GetMultiDB returns the underlying MultiDB for direct domain access
func (b *MultiBridge) GetMultiDB() *MultiDB {
	return b.mdb
}

// ───────────────────────────────────────────────────────────────────────────
// Session Lifecycle → sessions.db
// ───────────────────────────────────────────────────────────────────────────

// StartSession records a session start in sessions.db and live file
func (b *MultiBridge) StartSession(ctx context.Context, sessionID string, state *types.RuntimeState) error {
	now := time.Now()

	session := &sessions.Session{
		ID:                 sessionID,
		StartedAt:          now,
		InitialHebrewState: state.Session.HebrewState,
		InitialKAlign:      state.Session.KAlign,
		DayOfWeek:          int(now.Weekday()),
		HourOfDay:          now.Hour(),
	}

	if err := b.mdb.Sessions.CreateSession(ctx, session); err != nil {
		return err
	}

	state.Session.ID = sessionID
	state.Session.InitializedAt = now.Format(time.RFC3339)
	state.Session.LastUpdate = now.Format(time.RFC3339)
	state.Session.Active = true

	return b.saveLiveState(state)
}

// EndSession records a session end in sessions.db + growth.db patterns
func (b *MultiBridge) EndSession(ctx context.Context, sessionID string, state *types.RuntimeState) error {
	finalState := &sessions.Session{
		FinalHebrewState:     state.Session.HebrewState,
		FinalKAlign:          state.Session.KAlign,
		ToolCount:            state.Session.HooksFired,
		ChoiceCount:          state.Session.ChoiceSequence,
		ExchangeCount:        state.Session.ExchangeCount,
		InsightCount:         state.Session.InsightCount,
		CPIScore:             state.Session.CPIScore,
		DominantExchangeType: state.Session.DominantExchangeType,
		SessionArc:           state.Session.SessionArc,
	}

	if err := b.mdb.Sessions.EndSession(ctx, sessionID, finalState); err != nil {
		return err
	}

	state.Session.Active = false
	state.Session.LastUpdate = time.Now().Format(time.RFC3339)

	return b.saveLiveState(state)
}

// UpdateExchangeResponse updates the most recent exchange's response_summary.
// Called by the Stop hook to capture the assistant's response text.
func (b *MultiBridge) UpdateExchangeResponse(ctx context.Context, sessionID, responseText string) error {
	return b.mdb.Sessions.UpdateExchangeResponse(ctx, sessionID, responseText)
}

func (b *MultiBridge) UpdateExchangeThinking(ctx context.Context, sessionID, thinkingText string) error {
	return b.mdb.Sessions.UpdateExchangeThinking(ctx, sessionID, thinkingText)
}

// ───────────────────────────────────────────────────────────────────────────
// Choice Recording → cognition.db
// ───────────────────────────────────────────────────────────────────────────

// RecordChoice records a choice in cognition.db and updates live file
func (b *MultiBridge) RecordChoice(ctx context.Context, record *types.ChoiceRecord, state *types.RuntimeState) error {
	now := time.Now()

	healthScore := int(state.Session.HealthScore)
	choice := &cognition.Choice{
		ID:               record.ID,
		SessionID:        record.SessionID,
		SequenceNum:      record.SequenceNum,
		Timestamp:        now,
		IntendedKey:      int(record.Context.IntendedKey),
		PositionAtChoice: record.Context.PositionAtChoice,
		KAtChoice:        float64(record.Context.KAtChoice),
		ToolName:         record.Context.Tool,
		ToolCategory:     categoryFromKey(int(record.Context.IntendedKey)),
		HealthScore:      &healthScore,
	}

	if err := b.mdb.Cognition.RecordChoice(ctx, choice); err != nil {
		return err
	}

	state.Session.ChoiceSequence = record.SequenceNum
	state.Session.LastKeyChosen = int(record.Context.IntendedKey)
	state.Session.ChoiceTimestamp = now.Format(time.RFC3339)
	state.Session.LastUpdate = now.Format(time.RFC3339)

	return b.saveLiveState(state)
}

// CompleteChoice updates choice outcome in cognition.db + K:ALIGN record
func (b *MultiBridge) CompleteChoice(ctx context.Context, record *types.ChoiceRecord, state *types.RuntimeState) error {
	result := &cognition.Choice{
		ToolOutcome: record.Result.ToolOutcome,
		HaltReached: record.Result.HaltReached,
		HaltType:    string(record.Result.HaltType),
		TrueScore:   record.Impact.TrueScore,
		KAlignDelta: record.Impact.KAlignDelta,
		KAlignAfter: record.Impact.KAlignAfter,
	}

	if err := b.mdb.Cognition.CompleteChoice(ctx, record.ID, result); err != nil {
		return err
	}

	// Record K:ALIGN change in cognition.db
	kRecord := &cognition.KAlignRecord{
		SessionID:   record.SessionID,
		ChoiceID:    record.ID,
		Timestamp:   time.Now(),
		KAlignValue: record.Impact.KAlignAfter,
		Delta:       record.Impact.KAlignDelta,
		TriggerType: "choice",
	}

	if err := b.mdb.Cognition.RecordKAlign(ctx, kRecord); err != nil {
		return err
	}

	state.Session.KAlign = record.Impact.KAlignAfter
	state.Session.LastTrueScore = record.Impact.TrueScore
	state.Session.LastNormalized = record.Impact.Normalized
	state.Session.LastScaled = record.Impact.Scaled
	state.Session.ChoiceOutcome = record.Result.ToolOutcome
	state.Session.LastHaltReached = record.Result.HaltReached
	state.Session.LastHaltType = string(record.Result.HaltType)
	state.Session.LastUpdate = time.Now().Format(time.RFC3339)

	return b.saveLiveState(state)
}

// ───────────────────────────────────────────────────────────────────────────
// Hebrew State Transitions → cognition.db
// ───────────────────────────────────────────────────────────────────────────

// RecordHebrewTransition records a state change in cognition.db
func (b *MultiBridge) RecordHebrewTransition(ctx context.Context, sessionID string, fromState, toState string, triggerType string, state *types.RuntimeState) error {
	transition := &cognition.HebrewTransition{
		SessionID:   sessionID,
		Timestamp:   time.Now(),
		FromState:   fromState,
		ToState:     toState,
		TriggerType: triggerType,
		FromXValue:  hebrewToXValue(fromState),
		ToXValue:    hebrewToXValue(toState),
	}

	if err := b.mdb.Cognition.RecordHebrewTransition(ctx, transition); err != nil {
		return err
	}

	state.Session.HebrewState = toState
	state.Session.HebrewMeaning = hebrewToMeaning(toState)
	state.Session.LastUpdate = time.Now().Format(time.RFC3339)

	return b.saveLiveState(state)
}

// ───────────────────────────────────────────────────────────────────────────
// Pattern Recording → growth.db
// ───────────────────────────────────────────────────────────────────────────

// RecordPattern stores a detected pattern in growth.db
func (b *MultiBridge) RecordPattern(ctx context.Context, pattern *growth.Pattern) error {
	return b.mdb.Growth.RecordPattern(ctx, pattern)
}

// ───────────────────────────────────────────────────────────────────────────
// Temporal Awareness Queries
// ───────────────────────────────────────────────────────────────────────────

// GetLastSessionGap returns time since last session ended
func (b *MultiBridge) GetLastSessionGap(ctx context.Context) (time.Duration, error) {
	lastEnd, err := b.mdb.Sessions.GetLastSessionEnd(ctx)
	if err != nil {
		return 0, err
	}
	if lastEnd == nil {
		return 0, nil
	}
	return time.Since(*lastEnd), nil
}

// GetWorkPatterns returns when work typically happens
func (b *MultiBridge) GetWorkPatterns(ctx context.Context) (map[int]map[int]int, error) {
	return b.mdb.Sessions.GetTemporalWorkPatterns(ctx)
}

// GetAverageSessionLength returns typical session duration
func (b *MultiBridge) GetAverageSessionLength(ctx context.Context) (time.Duration, error) {
	return b.mdb.Sessions.GetAverageSessionDuration(ctx)
}

// ───────────────────────────────────────────────────────────────────────────
// Live File Operations
// ───────────────────────────────────────────────────────────────────────────

// LoadLiveState reads the current state from live file
func (b *MultiBridge) LoadLiveState() (*types.RuntimeState, error) {
	path := paths.StateMachineRuntimeState()

	data, err := os.ReadFile(path)
	if err != nil {
		if os.IsNotExist(err) {
			return b.defaultState(), nil
		}
		return nil, err
	}

	var state types.RuntimeState
	if err := json.Unmarshal(data, &state); err != nil {
		return nil, err
	}
	return &state, nil
}

// saveLiveState writes state to live file using atomic write (temp + rename)
func (b *MultiBridge) saveLiveState(state *types.RuntimeState) error {
	target := paths.StateMachineRuntimeState()

	data, err := json.MarshalIndent(state, "", "  ")
	if err != nil {
		return err
	}

	dir := filepath.Dir(target)
	if err := os.MkdirAll(dir, 0755); err != nil {
		return fmt.Errorf("ensure dir: %w", err)
	}

	tmp, err := os.CreateTemp(dir, filepath.Base(target)+".tmp.*")
	if err != nil {
		return fmt.Errorf("create temp: %w", err)
	}
	tmpName := tmp.Name()

	if _, err := tmp.Write(data); err != nil {
		tmp.Close()
		os.Remove(tmpName)
		return fmt.Errorf("write temp: %w", err)
	}
	if err := tmp.Close(); err != nil {
		os.Remove(tmpName)
		return fmt.Errorf("close temp: %w", err)
	}
	if err := os.Rename(tmpName, target); err != nil {
		os.Remove(tmpName)
		return fmt.Errorf("rename: %w", err)
	}
	return nil
}

// defaultState returns a new default state
func (b *MultiBridge) defaultState() *types.RuntimeState {
	return &types.RuntimeState{
		Version:           "1.0.0",
		AnchorKey:         "genesis_1_1",
		TrajectorySection: "B.1",
		CommandKey:        "proceed",
		Session: types.RuntimeSession{
			HebrewState:   "yashar",
			HebrewMeaning: "EVEN",
			KAlign:        0.0,
			Active:        false,
		},
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
