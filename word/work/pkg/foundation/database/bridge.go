// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-database-bridge
// Purpose: Bridge between live files (user-readable) and database (queryable)
// Biblical: Ecclesiastes 4:12 - "A threefold cord is not quickly broken"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-23
//
// Philosophy:
//   - Live Files (state.jsonc): User's data, always accessible, human-readable
//   - Database (cpisi.db): System's learning, queryable, pattern detection
//   - Neither replaces the other - both served
//
// This is game dev thinking: the player (user) never loses access to their data.
// The system gets queryable form for learning. Both needs met.
//
// ═══════════════════════════════════════════════════════════════════════════

package database

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"encoding/json"
	"os"
	"time"

	"creativeworkzstudio.com/bereshit/word/work/pkg/foundation/types"
	"creativeworkzstudio.com/bereshit/word/work/pkg/util/fs/paths"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Bridge
// ═══════════════════════════════════════════════════════════════════════════

// Bridge synchronizes between live files and database
// It ensures both storage layers stay in sync while serving their purposes:
//   - Files: Human-readable, immediate, git-trackable (USER)
//   - Database: Queryable, aggregated, patterns (SYSTEM)
type Bridge struct {
	repo Repository
}

// NewBridge creates a new bridge with the given repository
func NewBridge(repo Repository) *Bridge {
	return &Bridge{repo: repo}
}

// ───────────────────────────────────────────────────────────────────────────
// Session Lifecycle
// ───────────────────────────────────────────────────────────────────────────

// StartSession records a session start in both database and live file
func (b *Bridge) StartSession(ctx context.Context, sessionID string, state *types.RuntimeState) error {
	now := time.Now()

	// Create session in database
	session := &Session{
		ID:                 sessionID,
		StartedAt:          now,
		ProjectPath:        "", // Will be set from workspace
		Workspace:          "",
		InitialHebrewState: state.Session.HebrewState,
		InitialKAlign:      state.Session.KAlign,
		DayOfWeek:          int(now.Weekday()),
		HourOfDay:          now.Hour(),
	}

	if err := b.repo.CreateSession(ctx, session); err != nil {
		return err
	}

	// Update live file
	state.Session.ID = sessionID
	state.Session.InitializedAt = now.Format(time.RFC3339)
	state.Session.LastUpdate = now.Format(time.RFC3339)
	state.Session.Active = true

	return b.saveLiveState(state)
}

// EndSession records a session end in both database and live file
func (b *Bridge) EndSession(ctx context.Context, sessionID string, state *types.RuntimeState) error {
	// Update database
	finalState := &Session{
		FinalHebrewState: state.Session.HebrewState,
		FinalKAlign:      state.Session.KAlign,
		ToolCount:        state.Session.HooksFired,
		ChoiceCount:      state.Session.ChoiceSequence,
	}

	if err := b.repo.EndSession(ctx, sessionID, finalState); err != nil {
		return err
	}

	// Update live file
	state.Session.Active = false
	state.Session.LastUpdate = time.Now().Format(time.RFC3339)

	return b.saveLiveState(state)
}

// ───────────────────────────────────────────────────────────────────────────
// Choice Recording
// ───────────────────────────────────────────────────────────────────────────

// RecordChoice records a choice in database and updates live file
func (b *Bridge) RecordChoice(ctx context.Context, record *types.ChoiceRecord, state *types.RuntimeState) error {
	now := time.Now()

	// Create choice in database
	choice := &Choice{
		ID:               record.ID,
		SessionID:        record.SessionID,
		SequenceNum:      record.SequenceNum,
		Timestamp:        now,
		IntendedKey:      int(record.Context.IntendedKey),
		PositionAtChoice: record.Context.PositionAtChoice,
		KAtChoice:        float64(record.Context.KAtChoice),
		ToolName:         record.Context.Tool,
		ToolCategory:     categoryFromKey(int(record.Context.IntendedKey)),
	}

	if err := b.repo.RecordChoice(ctx, choice); err != nil {
		return err
	}

	// Update live file
	state.Session.ChoiceSequence = record.SequenceNum
	state.Session.LastKeyChosen = int(record.Context.IntendedKey)
	state.Session.ChoiceTimestamp = now.Format(time.RFC3339)
	state.Session.LastUpdate = now.Format(time.RFC3339)

	return b.saveLiveState(state)
}

// CompleteChoice updates choice with result in database and live file
func (b *Bridge) CompleteChoice(ctx context.Context, record *types.ChoiceRecord, state *types.RuntimeState) error {
	// Update database
	result := &Choice{
		ToolOutcome: record.Result.ToolOutcome,
		HaltReached: record.Result.HaltReached,
		HaltType:    string(record.Result.HaltType),
		TrueScore:   record.Impact.TrueScore,
		KAlignDelta: record.Impact.KAlignDelta,
		KAlignAfter: record.Impact.KAlignAfter,
	}

	if err := b.repo.CompleteChoice(ctx, record.ID, result); err != nil {
		return err
	}

	// Record K:ALIGN change
	kRecord := &KAlignRecord{
		SessionID:   record.SessionID,
		ChoiceID:    record.ID,
		Timestamp:   time.Now(),
		KAlignValue: record.Impact.KAlignAfter,
		Delta:       record.Impact.KAlignDelta,
		TriggerType: "choice",
	}

	if err := b.repo.RecordKAlign(ctx, kRecord); err != nil {
		return err
	}

	// Update live file
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
// Hebrew State Transitions
// ───────────────────────────────────────────────────────────────────────────

// RecordHebrewTransition records a state change in database and live file
func (b *Bridge) RecordHebrewTransition(ctx context.Context, sessionID string, fromState, toState string, triggerType string, state *types.RuntimeState) error {
	// Record in database
	transition := &HebrewTransition{
		SessionID:   sessionID,
		Timestamp:   time.Now(),
		FromState:   fromState,
		ToState:     toState,
		TriggerType: triggerType,
		FromXValue:  hebrewToXValue(fromState),
		ToXValue:    hebrewToXValue(toState),
	}

	if err := b.repo.RecordHebrewTransition(ctx, transition); err != nil {
		return err
	}

	// Update live file
	state.Session.HebrewState = toState
	state.Session.HebrewMeaning = hebrewToMeaning(toState)
	state.Session.LastUpdate = time.Now().Format(time.RFC3339)

	return b.saveLiveState(state)
}

// ───────────────────────────────────────────────────────────────────────────
// Temporal Awareness Queries
// ───────────────────────────────────────────────────────────────────────────

// GetLastSessionGap returns time since last session ended
func (b *Bridge) GetLastSessionGap(ctx context.Context) (time.Duration, error) {
	lastEnd, err := b.repo.GetLastSessionEnd(ctx)
	if err != nil {
		return 0, err
	}
	if lastEnd == nil {
		return 0, nil // First session ever
	}
	return time.Since(*lastEnd), nil
}

// GetWorkPatterns returns when work typically happens
func (b *Bridge) GetWorkPatterns(ctx context.Context) (map[int]map[int]int, error) {
	return b.repo.GetTemporalWorkPatterns(ctx)
}

// GetAverageSessionLength returns typical session duration
func (b *Bridge) GetAverageSessionLength(ctx context.Context) (time.Duration, error) {
	return b.repo.GetAverageSessionDuration(ctx)
}

// ───────────────────────────────────────────────────────────────────────────
// Live File Operations
// ───────────────────────────────────────────────────────────────────────────

// LoadLiveState reads the current state from live file
func (b *Bridge) LoadLiveState() (*types.RuntimeState, error) {
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

// saveLiveState writes state to live file
func (b *Bridge) saveLiveState(state *types.RuntimeState) error {
	path := paths.StateMachineRuntimeState()

	data, err := json.MarshalIndent(state, "", "  ")
	if err != nil {
		return err
	}

	return os.WriteFile(path, data, 0644)
}

// defaultState returns a new default state
func (b *Bridge) defaultState() *types.RuntimeState {
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

// ───────────────────────────────────────────────────────────────────────────
// Helpers
// ───────────────────────────────────────────────────────────────────────────

func categoryFromKey(key int) string {
	switch key {
	case 1:
		return "expansion"
	case 0:
		return "lateral"
	case -1:
		return "finality"
	default:
		return "unknown"
	}
}

func hebrewToXValue(state string) float64 {
	switch state {
	case "shavar":
		return -1.0
	case "chaser":
		return -0.5
	case "ratsah":
		return -0.5
	case "yashar":
		return 0.0
	case "tamim":
		return 0.5
	case "shalem":
		return 0.5
	case "tov":
		return 1.0
	default:
		return 0.0
	}
}

func hebrewToMeaning(state string) string {
	switch state {
	case "shavar":
		return "BROKEN"
	case "chaser":
		return "LACKING"
	case "ratsah":
		return "WANTING"
	case "yashar":
		return "EVEN"
	case "tamim":
		return "SOUND"
	case "shalem":
		return "WHOLE"
	case "tov":
		return "PERFECT"
	default:
		return "UNKNOWN"
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// The Bridge serves both:
//   - User: Live files remain human-readable, always accessible
//   - System: Database enables queries, patterns, temporal awareness
//
// "A threefold cord is not quickly broken" — Ecclesiastes 4:12
// Files, Database, and Bridge work together as one system.
//
