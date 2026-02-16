//omni:code --go -library
//omni:key B-L3-database-legacy
//omni:version a-01.00
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-legacy-adapter
// Purpose: Implements the old monolithic Repository interface via MultiDB
// Biblical: Isaiah 43:19 - "Behold, I will do a new thing"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// This adapter allows existing consumers (hooks, dashboard) to work
// unchanged during the transition from single-DB to multi-DB.
// Each method dispatches to the correct domain database.
//
// ═══════════════════════════════════════════════════════════════════════════

package database

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"time"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/cognition"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/growth"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/sessions"
)

// LegacyAdapter implements the old Repository interface by dispatching to MultiDB
type LegacyAdapter struct {
	mdb *MultiDB
}

// Compile-time interface check
var _ Repository = (*LegacyAdapter)(nil)

// NewLegacyAdapter creates a legacy adapter backed by multi-DB
func NewLegacyAdapter(mdb *MultiDB) *LegacyAdapter {
	return &LegacyAdapter{mdb: mdb}
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Session Operations → sessions.db
// ═══════════════════════════════════════════════════════════════════════════

func (a *LegacyAdapter) CreateSession(ctx context.Context, session *Session) error {
	s := &sessions.Session{
		ID:                  session.ID,
		StartedAt:           session.StartedAt,
		ProjectPath:         session.ProjectPath,
		Workspace:           session.Workspace,
		InitialHebrewState:  session.InitialHebrewState,
		InitialKAlign:       session.InitialKAlign,
		InitialCubePosition: session.InitialCubePosition,
		DayOfWeek:           session.DayOfWeek,
		HourOfDay:           session.HourOfDay,
	}
	return a.mdb.Sessions.CreateSession(ctx, s)
}

func (a *LegacyAdapter) EndSession(ctx context.Context, sessionID string, finalState *Session) error {
	s := &sessions.Session{
		FinalHebrewState:     finalState.FinalHebrewState,
		FinalKAlign:          finalState.FinalKAlign,
		FinalCubePosition:    finalState.FinalCubePosition,
		ToolCount:            finalState.ToolCount,
		ChoiceCount:          finalState.ChoiceCount,
		ExchangeCount:        finalState.ExchangeCount,
		InsightCount:         finalState.InsightCount,
		CPIScore:             finalState.CPIScore,
		DominantExchangeType: finalState.DominantExchangeType,
		SessionArc:           finalState.SessionArc,
		NarrativeSummary:     finalState.NarrativeSummary,
	}
	return a.mdb.Sessions.EndSession(ctx, sessionID, s)
}

func (a *LegacyAdapter) GetSession(ctx context.Context, sessionID string) (*Session, error) {
	s, err := a.mdb.Sessions.GetSession(ctx, sessionID)
	if err != nil || s == nil {
		return nil, err
	}
	return sessionsToLegacy(s), nil
}

func (a *LegacyAdapter) GetActiveSession(ctx context.Context) (*Session, error) {
	s, err := a.mdb.Sessions.GetActiveSession(ctx)
	if err != nil || s == nil {
		return nil, err
	}
	return sessionsToLegacy(s), nil
}

func (a *LegacyAdapter) GetRecentSessions(ctx context.Context, limit int) ([]Session, error) {
	domainSessions, err := a.mdb.Sessions.GetRecentSessions(ctx, limit)
	if err != nil {
		return nil, err
	}
	result := make([]Session, len(domainSessions))
	for i, s := range domainSessions {
		result[i] = *sessionsToLegacy(&s)
	}
	return result, nil
}

func (a *LegacyAdapter) GetSessionGaps(ctx context.Context, limit int) ([]SessionGap, error) {
	gaps, err := a.mdb.Sessions.GetSessionGaps(ctx, limit)
	if err != nil {
		return nil, err
	}
	result := make([]SessionGap, len(gaps))
	for i, g := range gaps {
		result[i] = SessionGap{
			SessionID:        g.SessionID,
			SessionEnd:       g.SessionEnd,
			NextSessionStart: g.NextSessionStart,
			GapHours:         g.GapHours,
		}
	}
	return result, nil
}

func (a *LegacyAdapter) GetLastSessionEnd(ctx context.Context) (*time.Time, error) {
	return a.mdb.Sessions.GetLastSessionEnd(ctx)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Choice Operations → cognition.db
// ═══════════════════════════════════════════════════════════════════════════

func (a *LegacyAdapter) RecordChoice(ctx context.Context, choice *Choice) error {
	c := &cognition.Choice{
		ID:               choice.ID,
		SessionID:        choice.SessionID,
		SequenceNum:      choice.SequenceNum,
		Timestamp:        choice.Timestamp,
		IntendedKey:      choice.IntendedKey,
		PositionAtChoice: choice.PositionAtChoice,
		KAtChoice:        choice.KAtChoice,
		CubePosition:     choice.CubePosition,
		ToolName:         choice.ToolName,
		ToolCategory:     choice.ToolCategory,
		HealthScore:      choice.HealthScore,
	}
	return a.mdb.Cognition.RecordChoice(ctx, c)
}

func (a *LegacyAdapter) CompleteChoice(ctx context.Context, choiceID string, result *Choice) error {
	c := &cognition.Choice{
		ToolOutcome: result.ToolOutcome,
		HaltReached: result.HaltReached,
		HaltType:    result.HaltType,
		TrueScore:   result.TrueScore,
		KAlignDelta: result.KAlignDelta,
		KAlignAfter: result.KAlignAfter,
	}
	return a.mdb.Cognition.CompleteChoice(ctx, choiceID, c)
}

func (a *LegacyAdapter) GetChoice(ctx context.Context, choiceID string) (*Choice, error) {
	c, err := a.mdb.Cognition.GetChoice(ctx, choiceID)
	if err != nil || c == nil {
		return nil, err
	}
	return cognitionChoiceToLegacy(c), nil
}

func (a *LegacyAdapter) GetSessionChoices(ctx context.Context, sessionID string) ([]Choice, error) {
	choices, err := a.mdb.Cognition.GetSessionChoices(ctx, sessionID)
	if err != nil {
		return nil, err
	}
	result := make([]Choice, len(choices))
	for i, c := range choices {
		result[i] = *cognitionChoiceToLegacy(&c)
	}
	return result, nil
}

func (a *LegacyAdapter) GetRecentChoices(ctx context.Context, limit int) ([]Choice, error) {
	choices, err := a.mdb.Cognition.GetRecentChoices(ctx, limit)
	if err != nil {
		return nil, err
	}
	result := make([]Choice, len(choices))
	for i, c := range choices {
		result[i] = *cognitionChoiceToLegacy(&c)
	}
	return result, nil
}

func (a *LegacyAdapter) GetChoicesByTool(ctx context.Context, toolName string, limit int) ([]Choice, error) {
	choices, err := a.mdb.Cognition.GetChoicesByTool(ctx, toolName, limit)
	if err != nil {
		return nil, err
	}
	result := make([]Choice, len(choices))
	for i, c := range choices {
		result[i] = *cognitionChoiceToLegacy(&c)
	}
	return result, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - K:ALIGN Operations → cognition.db
// ═══════════════════════════════════════════════════════════════════════════

func (a *LegacyAdapter) RecordKAlign(ctx context.Context, record *KAlignRecord) error {
	r := &cognition.KAlignRecord{
		SessionID:   record.SessionID,
		ChoiceID:    record.ChoiceID,
		Timestamp:   record.Timestamp,
		KAlignValue: record.KAlignValue,
		Delta:       record.Delta,
		TriggerType: record.TriggerType,
	}
	return a.mdb.Cognition.RecordKAlign(ctx, r)
}

func (a *LegacyAdapter) GetLatestKAlign(ctx context.Context, sessionID string) (float64, error) {
	return a.mdb.Cognition.GetLatestKAlign(ctx, sessionID)
}

func (a *LegacyAdapter) GetKAlignHistory(ctx context.Context, sessionID string) ([]KAlignRecord, error) {
	records, err := a.mdb.Cognition.GetKAlignHistory(ctx, sessionID)
	if err != nil {
		return nil, err
	}
	result := make([]KAlignRecord, len(records))
	for i, r := range records {
		result[i] = KAlignRecord{
			ID:          r.ID,
			SessionID:   r.SessionID,
			ChoiceID:    r.ChoiceID,
			Timestamp:   r.Timestamp,
			KAlignValue: r.KAlignValue,
			Delta:       r.Delta,
			TriggerType: r.TriggerType,
		}
	}
	return result, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Hebrew State Operations → cognition.db
// ═══════════════════════════════════════════════════════════════════════════

func (a *LegacyAdapter) RecordHebrewTransition(ctx context.Context, transition *HebrewTransition) error {
	t := &cognition.HebrewTransition{
		SessionID:     transition.SessionID,
		ChoiceID:      transition.ChoiceID,
		Timestamp:     transition.Timestamp,
		FromState:     transition.FromState,
		ToState:       transition.ToState,
		TriggerType:   transition.TriggerType,
		TriggerDetail: transition.TriggerDetail,
		FromXValue:    transition.FromXValue,
		ToXValue:      transition.ToXValue,
	}
	return a.mdb.Cognition.RecordHebrewTransition(ctx, t)
}

func (a *LegacyAdapter) GetHebrewTransitions(ctx context.Context, sessionID string) ([]HebrewTransition, error) {
	transitions, err := a.mdb.Cognition.GetHebrewTransitions(ctx, sessionID)
	if err != nil {
		return nil, err
	}
	result := make([]HebrewTransition, len(transitions))
	for i, t := range transitions {
		result[i] = HebrewTransition{
			ID:            t.ID,
			SessionID:     t.SessionID,
			ChoiceID:      t.ChoiceID,
			Timestamp:     t.Timestamp,
			FromState:     t.FromState,
			ToState:       t.ToState,
			TriggerType:   t.TriggerType,
			TriggerDetail: t.TriggerDetail,
			FromXValue:    t.FromXValue,
			ToXValue:      t.ToXValue,
		}
	}
	return result, nil
}

func (a *LegacyAdapter) GetCurrentHebrewState(ctx context.Context, sessionID string) (string, error) {
	return a.mdb.Cognition.GetCurrentHebrewState(ctx, sessionID)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Pattern Operations → growth.db
// ═══════════════════════════════════════════════════════════════════════════

func (a *LegacyAdapter) RecordPattern(ctx context.Context, pattern *Pattern) error {
	p := &growth.Pattern{
		PatternType:     pattern.PatternType,
		PatternKey:      pattern.PatternKey,
		Description:     pattern.Description,
		PatternData:     pattern.PatternData,
		FirstSeen:       pattern.FirstSeen,
		LastSeen:        pattern.LastSeen,
		OccurrenceCount: pattern.OccurrenceCount,
		Confidence:      pattern.Confidence,
		IsActive:        pattern.IsActive,
	}
	return a.mdb.Growth.RecordPattern(ctx, p)
}

func (a *LegacyAdapter) GetPatterns(ctx context.Context, patternType string) ([]Pattern, error) {
	patterns, err := a.mdb.Growth.GetPatterns(ctx, patternType)
	if err != nil {
		return nil, err
	}
	result := make([]Pattern, len(patterns))
	for i, p := range patterns {
		result[i] = growthPatternToLegacy(&p)
	}
	return result, nil
}

func (a *LegacyAdapter) GetAllActivePatterns(ctx context.Context) ([]Pattern, error) {
	patterns, err := a.mdb.Growth.GetAllActivePatterns(ctx)
	if err != nil {
		return nil, err
	}
	result := make([]Pattern, len(patterns))
	for i, p := range patterns {
		result[i] = growthPatternToLegacy(&p)
	}
	return result, nil
}

func (a *LegacyAdapter) IncrementPatternCount(ctx context.Context, patternType, patternKey string) error {
	return a.mdb.Growth.IncrementPatternCount(ctx, patternType, patternKey)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Temporal Pattern Queries → sessions.db
// ═══════════════════════════════════════════════════════════════════════════

func (a *LegacyAdapter) GetTemporalWorkPatterns(ctx context.Context) (map[int]map[int]int, error) {
	return a.mdb.Sessions.GetTemporalWorkPatterns(ctx)
}

func (a *LegacyAdapter) GetAverageSessionDuration(ctx context.Context) (time.Duration, error) {
	return a.mdb.Sessions.GetAverageSessionDuration(ctx)
}

func (a *LegacyAdapter) GetAverageGapDuration(ctx context.Context) (time.Duration, error) {
	return a.mdb.Sessions.GetAverageGapDuration(ctx)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Raw SQL → sessions.db (primary, for backward compat)
// ═══════════════════════════════════════════════════════════════════════════

func (a *LegacyAdapter) Exec(ctx context.Context, query string, args ...interface{}) (int64, error) {
	return a.mdb.Sessions.Exec(ctx, query, args...)
}

func (a *LegacyAdapter) Query(ctx context.Context, query string, args ...interface{}) ([]map[string]interface{}, error) {
	return a.mdb.Sessions.Query(ctx, query, args...)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

func (a *LegacyAdapter) Close() error {
	return a.mdb.Close()
}

func (a *LegacyAdapter) Migrate(ctx context.Context) error {
	return a.mdb.MigrateAll(ctx)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Type Conversion Helpers
// ═══════════════════════════════════════════════════════════════════════════

func sessionsToLegacy(s *sessions.Session) *Session {
	ls := &Session{
		ID:                   s.ID,
		StartedAt:            s.StartedAt,
		EndedAt:              s.EndedAt,
		ProjectPath:          s.ProjectPath,
		Workspace:            s.Workspace,
		InitialHebrewState:   s.InitialHebrewState,
		InitialKAlign:        s.InitialKAlign,
		InitialCubePosition:  s.InitialCubePosition,
		FinalHebrewState:     s.FinalHebrewState,
		FinalKAlign:          s.FinalKAlign,
		FinalCubePosition:    s.FinalCubePosition,
		ToolCount:            s.ToolCount,
		ChoiceCount:          s.ChoiceCount,
		DayOfWeek:            s.DayOfWeek,
		HourOfDay:            s.HourOfDay,
		ExchangeCount:        s.ExchangeCount,
		InsightCount:         s.InsightCount,
		CPIScore:             s.CPIScore,
		DominantExchangeType: s.DominantExchangeType,
		SessionArc:           s.SessionArc,
		NarrativeSummary:     s.NarrativeSummary,
	}
	return ls
}

func cognitionChoiceToLegacy(c *cognition.Choice) *Choice {
	return &Choice{
		ID:               c.ID,
		SessionID:        c.SessionID,
		SequenceNum:      c.SequenceNum,
		Timestamp:        c.Timestamp,
		IntendedKey:      c.IntendedKey,
		PositionAtChoice: c.PositionAtChoice,
		KAtChoice:        c.KAtChoice,
		CubePosition:     c.CubePosition,
		ToolName:         c.ToolName,
		ToolCategory:     c.ToolCategory,
		ToolOutcome:      c.ToolOutcome,
		HaltReached:      c.HaltReached,
		HaltType:         c.HaltType,
		TrueScore:        c.TrueScore,
		KAlignDelta:      c.KAlignDelta,
		KAlignAfter:      c.KAlignAfter,
	}
}

func growthPatternToLegacy(p *growth.Pattern) Pattern {
	return Pattern{
		ID:              p.ID,
		PatternType:     p.PatternType,
		PatternKey:      p.PatternKey,
		Description:     p.Description,
		PatternData:     p.PatternData,
		FirstSeen:       p.FirstSeen,
		LastSeen:        p.LastSeen,
		OccurrenceCount: p.OccurrenceCount,
		Confidence:      p.Confidence,
		IsActive:        p.IsActive,
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
