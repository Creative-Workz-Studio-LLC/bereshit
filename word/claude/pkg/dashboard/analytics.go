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

	"github.com/creativeworkzstudio/claude-global/pkg/foundation/database"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// AnalyticsBundle contains all aggregated analytics for dashboard display.
type AnalyticsBundle struct {
	// Session-level metrics
	RecentSessions []database.Session `json:"recentSessions"`
	SessionGaps    []database.SessionGap `json:"sessionGaps"`

	// Choice patterns
	RecentChoices []database.Choice `json:"recentChoices"`

	// K:ALIGN evolution
	KAlignHistory []database.KAlignRecord `json:"kAlignHistory"`

	// Hebrew state transitions
	HebrewTransitions []database.HebrewTransition `json:"hebrewTransitions"`

	// Detected patterns
	ActivePatterns []database.Pattern `json:"activePatterns"`

	// Temporal work patterns (day -> hour -> count)
	WorkPatterns map[int]map[int]int `json:"workPatterns"`

	// Cross-session aggregates from history.jsonc
	Aggregates *AggregatesSummary `json:"aggregates,omitempty"`
}

// LoadAnalytics queries the database for a comprehensive analytics bundle.
func LoadAnalytics(ctx context.Context, db database.Repository, sessionID string) (*AnalyticsBundle, error) {
	bundle := &AnalyticsBundle{}

	// Recent sessions
	sessions, err := db.GetRecentSessions(ctx, 50)
	if err == nil {
		bundle.RecentSessions = sessions
	}

	// Session gaps
	gaps, err := db.GetSessionGaps(ctx, 50)
	if err == nil {
		bundle.SessionGaps = gaps
	}

	// Recent choices
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

	// Active patterns
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

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
