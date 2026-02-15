// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-dashboard-history
// Purpose: Session history queries from history.jsonc and SQLite
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
	"cws.studio/pkg/core/statemachine"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// HistorySummary combines history.jsonc data with aggregates.
type HistorySummary struct {
	Sessions       []statemachine.RuntimeSessionSummary `json:"sessions"`
	Aggregates     AggregatesSummary                    `json:"aggregates"`
	TotalSessions  int                                  `json:"totalSessions"`
	TotalCompactions int                                `json:"totalCompactions"`
}

// AggregatesSummary contains cross-session patterns.
type AggregatesSummary struct {
	AnchorFrequencies  map[string]int     `json:"anchorFrequencies"`
	CommandFrequencies map[string]int     `json:"commandFrequencies"`
	TransitionPatterns map[string]int     `json:"transitionPatterns"`
	TrajectoryAvgTime  map[string]float64 `json:"trajectoryAvgTime"`
}

// LoadHistorySummary reads history.jsonc and returns a summary.
func LoadHistorySummary(limit int) (*HistorySummary, error) {
	history, err := statemachine.LoadRuntimeHistory()
	if err != nil {
		return nil, err
	}

	sessions := history.Sessions
	if limit > 0 && len(sessions) > limit {
		sessions = sessions[len(sessions)-limit:]
	}

	summary := &HistorySummary{
		Sessions: sessions,
		Aggregates: AggregatesSummary{
			AnchorFrequencies:  history.Aggregates.AnchorFrequencies,
			CommandFrequencies: history.Aggregates.CommandFrequencies,
			TransitionPatterns: history.Aggregates.TransitionPatterns,
			TrajectoryAvgTime:  history.Aggregates.TrajectoryAvgTime,
		},
		TotalSessions: history.Meta.TotalSessions,
	}
	if history.Meta.TotalCompactions > 0 {
		summary.TotalCompactions = history.Meta.TotalCompactions
	}

	return summary, nil
}

// LoadPathSummary reads path.jsonc and returns anchor/event data for the current session.
type PathSummary struct {
	SessionID         string                               `json:"sessionID"`
	StartedAt         string                               `json:"startedAt"`
	Anchors           []statemachine.RuntimeAnchorVisit     `json:"anchors"`
	TrajectoryHistory []statemachine.RuntimeTrajectoryVisit `json:"trajectoryHistory"`
	Events            []statemachine.RuntimeEvent           `json:"events"`
	Summary           statemachine.RuntimePathSummary       `json:"summary"`
}

// LoadPathSummary reads path.jsonc for the current session.
func LoadPathData() (*PathSummary, error) {
	path, err := statemachine.LoadRuntimePath()
	if err != nil {
		return nil, err
	}

	return &PathSummary{
		SessionID:         path.SessionID,
		StartedAt:         path.StartedAt,
		Anchors:           path.Anchors,
		TrajectoryHistory: path.TrajectoryHistory,
		Events:            path.Events,
		Summary:           path.Summary,
	}, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
