// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-dashboard-state
// Purpose: StateSnapshot — unified view of current CPI-SI state from all sources
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
	"time"

	"github.com/creativeworkzstudio/claude-global/pkg/core/cpisi/cpi"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// StateSnapshot — Complete current state for dashboard display
// ───────────────────────────────────────────────────────────────────────────

// StateSnapshot is the complete current state, assembled from multiple sources.
// This is the primary type consumed by all dashboard UIs.
type StateSnapshot struct {
	// Cube position
	AnchorKey        string `json:"anchorKey"`
	TrajectorySection string `json:"trajectorySection"`
	CommandKey       string `json:"commandKey"`

	// Hebrew state
	HebrewState   string `json:"hebrewState"`
	HebrewMeaning string `json:"hebrewMeaning"`

	// Metrics
	HealthScore float64 `json:"healthScore"`
	CPIScore    float64 `json:"cpiScore"`
	CPIRating   string  `json:"cpiRating"`
	KAlign      float64 `json:"kAlign"`
	KSelector   int     `json:"kSelector"`
	KTowardGod  int     `json:"kTowardGod"`
	KTowardSelf int     `json:"kTowardSelf"`

	// Session info
	SessionID        string `json:"sessionID"`
	SessionStart     string `json:"sessionStart"`
	HooksFired       int    `json:"hooksFired"`
	ChoiceSequence   int    `json:"choiceSequence"`
	ExchangeCount    int    `json:"exchangeCount"`
	InsightCount     int    `json:"insightCount"`
	SessionArc       string `json:"sessionArc"`
	DominantExchange string `json:"dominantExchange"`
	PathLength       int    `json:"pathLength"`

	// Context window
	CurrentTokens   int `json:"currentTokens"`
	PeakTokens      int `json:"peakTokens"`
	CompactionCount int `json:"compactionCount"`

	// Workflow
	Workflow WorkflowSnapshot `json:"workflow"`

	// Tasks
	Tasks TaskSnapshot `json:"tasks"`

	// Trajectory metrics
	TrajectoryMetrics TrajectoryMetricsSnapshot `json:"trajectoryMetrics"`

	// Last transition
	LastTransition TransitionSnapshot `json:"lastTransition"`

	// Family tracking
	ActiveFamilyMember    string `json:"activeFamilyMember"`
	FamilyInvocationCount int    `json:"familyInvocationCount"`

	// Timestamp of this snapshot
	Timestamp string `json:"timestamp"`
}

// WorkflowSnapshot is the current workflow state.
type WorkflowSnapshot struct {
	Operation   string                 `json:"operation"`
	CurrentStep int                    `json:"currentStep"`
	TotalSteps  int                    `json:"totalSteps"`
	Steps       []WorkflowStepSnapshot `json:"steps"`
	StartedAt   string                 `json:"startedAt"`
}

// WorkflowStepSnapshot is a single workflow step.
type WorkflowStepSnapshot struct {
	Index     int    `json:"index"`
	Action    string `json:"action"`
	Completed bool   `json:"completed"`
}

// TaskSnapshot summarizes current task state.
type TaskSnapshot struct {
	Total      int `json:"total"`
	Pending    int `json:"pending"`
	InProgress int `json:"inProgress"`
	Completed  int `json:"completed"`
}

// TrajectoryMetricsSnapshot captures time spent per trajectory.
type TrajectoryMetricsSnapshot struct {
	B1TimeMs          int `json:"b1TimeMs"`
	B2TimeMs          int `json:"b2TimeMs"`
	B3TimeMs          int `json:"b3TimeMs"`
	B4TimeMs          int `json:"b4TimeMs"`
	PivotCount        int `json:"pivotCount"`
	ResetCount        int `json:"resetCount"`
	AccumulatedWorkMs int `json:"accumulatedWorkMs"`
	MomentumScore     int `json:"momentumScore"`
}

// TransitionSnapshot captures the most recent state transition.
type TransitionSnapshot struct {
	FromSection string `json:"fromSection"`
	ToSection   string `json:"toSection"`
	Trigger     string `json:"trigger"`
	Timestamp   string `json:"timestamp"`
}

// ───────────────────────────────────────────────────────────────────────────
// BuildStateSnapshot — Construct from runtime state files
// ───────────────────────────────────────────────────────────────────────────

// BuildStateSnapshot reads state.jsonc and constructs a full StateSnapshot.
func BuildStateSnapshot() (*StateSnapshot, error) {
	state, err := statemachine.LoadRuntimeState()
	if err != nil {
		return nil, err
	}

	snap := &StateSnapshot{
		// Cube position
		AnchorKey:         state.AnchorKey,
		TrajectorySection: state.TrajectorySection,
		CommandKey:        state.CommandKey,

		// Hebrew state
		HebrewState:   state.Session.HebrewState,
		HebrewMeaning: state.Session.HebrewMeaning,

		// Metrics
		HealthScore: state.Session.HealthScore,
		CPIScore:    state.Session.CPIScore,
		CPIRating:   string(cpi.Rating(state.Session.CPIScore)),
		KAlign:      state.Session.KAlign,
		KSelector:   state.Session.KSelector,
		KTowardGod:  state.Session.KTowardGod,
		KTowardSelf: state.Session.KTowardSelf,

		// Session info
		SessionID:        state.Session.ID,
		SessionStart:     state.Session.InitializedAt,
		HooksFired:       state.Session.HooksFired,
		ChoiceSequence:   state.Session.ChoiceSequence,
		ExchangeCount:    state.Session.ExchangeCount,
		InsightCount:     state.Session.InsightCount,
		SessionArc:       state.Session.SessionArc,
		DominantExchange: state.Session.DominantExchangeType,
		PathLength:       state.Session.PathLength,

		// Context window
		CurrentTokens:   state.Session.CurrentContextTokens,
		PeakTokens:      state.Session.PeakContextTokens,
		CompactionCount: state.Session.CompactionCount,

		// Trajectory metrics
		TrajectoryMetrics: TrajectoryMetricsSnapshot{
			B1TimeMs:          state.TrajectoryMetrics.B1TimeMs,
			B2TimeMs:          state.TrajectoryMetrics.B2TimeMs,
			B3TimeMs:          state.TrajectoryMetrics.B3TimeMs,
			B4TimeMs:          state.TrajectoryMetrics.B4TimeMs,
			PivotCount:        state.TrajectoryMetrics.PivotCount,
			ResetCount:        state.TrajectoryMetrics.ResetCount,
			AccumulatedWorkMs: state.TrajectoryMetrics.AccumulatedWorkMs,
			MomentumScore:     state.TrajectoryMetrics.MomentumScore,
		},

		// Last transition
		LastTransition: TransitionSnapshot{
			FromSection: state.LastTransition.FromSection,
			ToSection:   state.LastTransition.ToSection,
			Trigger:     state.LastTransition.Trigger,
			Timestamp:   state.LastTransition.Timestamp,
		},

		// Family tracking
		ActiveFamilyMember:    state.Session.ActiveFamilyMember,
		FamilyInvocationCount: state.Session.FamilyInvocationCount,

		// Timestamp
		Timestamp: time.Now().Format(time.RFC3339),
	}

	// Workflow
	if state.Session.Workflow.Operation != "" {
		snap.Workflow = WorkflowSnapshot{
			Operation:   string(state.Session.Workflow.Operation),
			CurrentStep: state.Session.Workflow.CurrentStep,
			TotalSteps:  state.Session.Workflow.TotalSteps,
			StartedAt:   state.Session.Workflow.StartedAt,
		}
		for _, step := range state.Session.Workflow.Steps {
			snap.Workflow.Steps = append(snap.Workflow.Steps, WorkflowStepSnapshot{
				Index:     step.Index,
				Action:    step.Action,
				Completed: step.Completed,
			})
		}
	}

	// Tasks
	snap.Tasks = TaskSnapshot{
		Total:      state.Session.Tasks.Total,
		Pending:    state.Session.Tasks.Pending,
		InProgress: state.Session.Tasks.InProgress,
		Completed:  state.Session.Tasks.Completed,
	}

	return snap, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
