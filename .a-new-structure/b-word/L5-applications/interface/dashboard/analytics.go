// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-dashboard-analytics
// Purpose: Aggregated analytics from SQLite views for dashboard charts
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// ═══════════════════════════════════════════════════════════════════════════

package dashboard

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"

	"cws.studio/pkg/foundation/database"
	"cws.studio/pkg/foundation/database/growth"
	"cws.studio/pkg/foundation/database/sessions"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// AnalyticsBundle contains all aggregated analytics for dashboard display.
type AnalyticsBundle struct {
	// Session-level metrics
	RecentSessions []database.Session    `json:"recentSessions"`
	SessionGaps    []database.SessionGap `json:"sessionGaps"`

	// Choice patterns (tool usage from cognition.db)
	RecentChoices []database.Choice `json:"recentChoices"`

	// K:ALIGN evolution
	KAlignHistory []database.KAlignRecord `json:"kAlignHistory"`

	// Hebrew state transitions
	HebrewTransitions []database.HebrewTransition `json:"hebrewTransitions"`

	// Detected patterns (all active)
	ActivePatterns []database.Pattern `json:"activePatterns"`

	// Live patterns (real-time triggers from hooks)
	LivePatterns []growth.Pattern `json:"livePatterns"`

	// Exchange data (with message text + valence)
	RecentExchanges []sessions.Exchange `json:"recentExchanges"`

	// Valence distribution for current session
	ValenceDistribution map[string]int `json:"valenceDistribution"`

	// Temporal work patterns (day -> hour -> count)
	WorkPatterns map[int]map[int]int `json:"workPatterns"`

	// Cross-session aggregates from history.jsonc
	Aggregates *AggregatesSummary `json:"aggregates,omitempty"`
}

// LoadAnalytics queries the database for a comprehensive analytics bundle.
func LoadAnalytics(ctx context.Context, db database.Repository, sessionID string) (*AnalyticsBundle, error) {
	bundle := &AnalyticsBundle{}

	// Recent sessions
	recentSessions, err := db.GetRecentSessions(ctx, 50)
	if err == nil {
		bundle.RecentSessions = recentSessions
	}

	// Session gaps
	gaps, err := db.GetSessionGaps(ctx, 50)
	if err == nil {
		bundle.SessionGaps = gaps
	}

	// Recent choices (tool usage from cognition.db)
	choices, err := db.GetRecentChoices(ctx, 100)
	if err == nil {
		bundle.RecentChoices = choices
	}

	// K:ALIGN history for current session
	if sessionID != "" {
		kalign, err := db.GetKAlignHistory(ctx, sessionID)
		if err == nil {
			bundle.KAlignHistory = kalign
		}

		hebrew, err := db.GetHebrewTransitions(ctx, sessionID)
		if err == nil {
			bundle.HebrewTransitions = hebrew
		}
	}

	// Active patterns (all)
	patterns, err := db.GetAllActivePatterns(ctx)
	if err == nil {
		bundle.ActivePatterns = patterns
	}

	// Temporal work patterns
	workPatterns, err := db.GetTemporalWorkPatterns(ctx)
	if err == nil {
		bundle.WorkPatterns = workPatterns
	}

	// Cross-session aggregates from history.jsonc
	historySummary, err := LoadHistorySummary(0)
	if err == nil {
		bundle.Aggregates = &historySummary.Aggregates
	}

	return bundle, nil
}

// LoadAnalyticsWithMultiDB queries all 5 domain databases for a comprehensive analytics bundle.
// This is the preferred method — uses direct domain access for richer data.
func LoadAnalyticsWithMultiDB(ctx context.Context, svc *DashboardService, sessionID string) (*AnalyticsBundle, error) {
	// Start with the legacy analytics
	bundle, err := LoadAnalytics(ctx, svc.DB(), sessionID)
	if err != nil {
		return nil, err
	}

	// Enrich with direct domain queries

	// Recent exchanges with message text + valence
	exchanges, err := svc.RecentExchanges(sessionID, 50)
	if err == nil {
		bundle.RecentExchanges = exchanges
	}

	// Live patterns from growth.db
	livePatterns, err := svc.LivePatterns()
	if err == nil {
		bundle.LivePatterns = livePatterns
	}

	// Valence distribution for current session
	if sessionID != "" {
		valDist, err := svc.ValenceDistribution(sessionID)
		if err == nil {
			bundle.ValenceDistribution = valDist
		}
	}

	return bundle, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
