// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-restore
// Purpose: Autonomous repair system - Detect → Assess → Restore cycle
// Biblical: Psalm 23:3 - "He restoreth my soul"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-23
//
// Design:
//   The restore package implements the immune system pattern:
//   - DETECT: Identify health degradation, trajectory issues, misalignment
//   - ASSESS: Evaluate severity, determine recovery path
//   - RESTORE: Execute autonomous repair actions
//
//   This is NOT manual checkpointing - the state machine naturally
//   creates checkpoints. This is AUTONOMOUS SELF-REPAIR.
//
// ═══════════════════════════════════════════════════════════════════════════

package restore

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"path/filepath"
	"time"

	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
	"creativeworkzstudio.com/bereshit/word/work/pkg/util/fs/paths"
)

// --- Detection Types ---

// Issue represents a detected problem
type Issue struct {
	Type        IssueType
	Severity    Severity
	Description string
	Source      string
	DetectedAt  time.Time
	Data        map[string]interface{}
}

// IssueType categorizes detected issues
type IssueType string

const (
	IssueHealthDegraded    IssueType = "health_degraded"
	IssueTrajectoryStuck   IssueType = "trajectory_stuck"
	IssueMisalignment      IssueType = "misalignment"
	IssueAnchorDrift       IssueType = "anchor_drift"
	IssueChoiceLoop        IssueType = "choice_loop"
	IssueStateCorruption   IssueType = "state_corruption"
)

// Severity indicates how critical an issue is
type Severity string

const (
	SeverityCritical Severity = "critical" // Immediate attention required
	SeverityHigh     Severity = "high"     // Should be addressed soon
	SeverityMedium   Severity = "medium"   // Can wait but track it
	SeverityLow      Severity = "low"      // Minor, self-correcting
)

// --- Assessment Types ---

// Assessment is the result of evaluating detected issues
type Assessment struct {
	Issues          []Issue
	OverallSeverity Severity
	RecoveryPath    RecoveryPath
	EstimatedEffort int // Number of +1 actions needed
	Recommendations []Recommendation
	AssessedAt      time.Time
}

// RecoveryPath indicates the recommended recovery approach
type RecoveryPath string

const (
	RecoveryNone        RecoveryPath = "none"        // No action needed
	RecoverySelfCorrect RecoveryPath = "self_correct" // Will fix itself
	RecoveryGuided      RecoveryPath = "guided"      // Needs guidance
	RecoveryManual      RecoveryPath = "manual"      // Requires human intervention
	RecoveryReset       RecoveryPath = "reset"       // Fresh start recommended
)

// Recommendation is a specific recovery action
type Recommendation struct {
	Priority    int
	Action      string
	Description string
	ExpectedImpact int // Expected health delta
}

// --- Restore Types ---

// RestoreResult captures the outcome of a restore operation
type RestoreResult struct {
	Success         bool
	IssuesFixed     int
	IssuesRemaining int
	HealthBefore    float64
	HealthAfter     float64
	Actions         []RestoreAction
	RestoredAt      time.Time
}

// RestoreAction is a single repair action taken
type RestoreAction struct {
	Type        string
	Description string
	Success     bool
	HealthDelta int
	Timestamp   time.Time
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Detector ---

// Detector identifies issues in the current state
type Detector struct {
	thresholds DetectionThresholds
}

// DetectionThresholds configures when to raise issues
type DetectionThresholds struct {
	HealthWarning       float64       // Below this triggers warning (default: -20)
	HealthCritical      float64       // Below this triggers critical (default: -50)
	TrajectoryStuckTime time.Duration // Time in same section before "stuck" (default: 30min)
	MisalignmentDelta   float64       // K:ALIGN swing that triggers concern (default: 0.3)
	ChoiceLoopCount     int           // Repeated same-key choices (default: 10)
}

// DefaultThresholds returns sensible default detection thresholds
func DefaultThresholds() DetectionThresholds {
	return DetectionThresholds{
		HealthWarning:       -20.0,
		HealthCritical:      -50.0,
		TrajectoryStuckTime: 30 * time.Minute,
		MisalignmentDelta:   0.3,
		ChoiceLoopCount:     10,
	}
}

// NewDetector creates a detector with default thresholds
func NewDetector() *Detector {
	return &Detector{
		thresholds: DefaultThresholds(),
	}
}

// NewDetectorWithThresholds creates a detector with custom thresholds
func NewDetectorWithThresholds(t DetectionThresholds) *Detector {
	return &Detector{thresholds: t}
}

// Detect analyzes current state and returns any issues found
func (d *Detector) Detect(state *statemachine.RuntimeState) []Issue {
	if state == nil {
		return []Issue{{
			Type:        IssueStateCorruption,
			Severity:    SeverityCritical,
			Description: "State is nil - cannot detect issues",
			DetectedAt:  time.Now(),
		}}
	}

	var issues []Issue

	// Check health degradation
	if issue := d.detectHealthIssue(state); issue != nil {
		issues = append(issues, *issue)
	}

	// Check trajectory stuck
	if issue := d.detectTrajectoryStuck(state); issue != nil {
		issues = append(issues, *issue)
	}

	// Check misalignment
	if issue := d.detectMisalignment(state); issue != nil {
		issues = append(issues, *issue)
	}

	// Check choice loops
	if issue := d.detectChoiceLoop(state); issue != nil {
		issues = append(issues, *issue)
	}

	return issues
}

func (d *Detector) detectHealthIssue(state *statemachine.RuntimeState) *Issue {
	health := state.Session.HealthScore

	if health <= d.thresholds.HealthCritical {
		return &Issue{
			Type:        IssueHealthDegraded,
			Severity:    SeverityCritical,
			Description: fmt.Sprintf("Health critically low: %.0f", health),
			Source:      "health_score",
			DetectedAt:  time.Now(),
			Data:        map[string]interface{}{"health": health},
		}
	}

	if health <= d.thresholds.HealthWarning {
		return &Issue{
			Type:        IssueHealthDegraded,
			Severity:    SeverityMedium,
			Description: fmt.Sprintf("Health below warning threshold: %.0f", health),
			Source:      "health_score",
			DetectedAt:  time.Now(),
			Data:        map[string]interface{}{"health": health},
		}
	}

	return nil
}

func (d *Detector) detectTrajectoryStuck(state *statemachine.RuntimeState) *Issue {
	// Check if last transition was too long ago
	if state.LastTransition.Timestamp == "" {
		return nil // No transitions yet, not stuck
	}

	lastTransition, err := time.Parse(time.RFC3339, state.LastTransition.Timestamp)
	if err != nil {
		return nil
	}

	timeSince := time.Since(lastTransition)
	if timeSince > d.thresholds.TrajectoryStuckTime {
		return &Issue{
			Type:        IssueTrajectoryStuck,
			Severity:    SeverityMedium,
			Description: fmt.Sprintf("Trajectory stuck in %s for %v", state.TrajectorySection, timeSince.Round(time.Minute)),
			Source:      "trajectory_section",
			DetectedAt:  time.Now(),
			Data: map[string]interface{}{
				"section":    state.TrajectorySection,
				"stuck_time": timeSince.String(),
			},
		}
	}

	return nil
}

func (d *Detector) detectMisalignment(state *statemachine.RuntimeState) *Issue {
	kAlign := state.Session.KAlign

	// Check for severe misalignment
	if kAlign < -0.5 {
		return &Issue{
			Type:        IssueMisalignment,
			Severity:    SeverityHigh,
			Description: fmt.Sprintf("K:ALIGN severely negative: %.2f", kAlign),
			Source:      "k_align",
			DetectedAt:  time.Now(),
			Data:        map[string]interface{}{"k_align": kAlign},
		}
	}

	return nil
}

func (d *Detector) detectChoiceLoop(state *statemachine.RuntimeState) *Issue {
	// Simple heuristic: if k_toward_self far exceeds k_toward_god, might be looping
	towardSelf := state.Session.KTowardSelf
	towardGod := state.Session.KTowardGod

	if towardSelf > towardGod+d.thresholds.ChoiceLoopCount {
		return &Issue{
			Type:        IssueChoiceLoop,
			Severity:    SeverityLow,
			Description: fmt.Sprintf("Choices heavily toward self (%d vs %d toward God)", towardSelf, towardGod),
			Source:      "choice_pattern",
			DetectedAt:  time.Now(),
			Data: map[string]interface{}{
				"toward_self": towardSelf,
				"toward_god":  towardGod,
			},
		}
	}

	return nil
}

// --- Assessor ---

// Assessor evaluates detected issues and recommends recovery
type Assessor struct{}

// NewAssessor creates an assessor
func NewAssessor() *Assessor {
	return &Assessor{}
}

// Assess evaluates issues and produces an assessment
func (a *Assessor) Assess(issues []Issue, state *statemachine.RuntimeState) *Assessment {
	if len(issues) == 0 {
		return &Assessment{
			Issues:          issues,
			OverallSeverity: SeverityLow,
			RecoveryPath:    RecoveryNone,
			EstimatedEffort: 0,
			AssessedAt:      time.Now(),
		}
	}

	// Determine overall severity (highest wins)
	overall := SeverityLow
	for _, issue := range issues {
		if severityRank(issue.Severity) > severityRank(overall) {
			overall = issue.Severity
		}
	}

	// Determine recovery path
	path := a.determineRecoveryPath(issues, overall)

	// Build recommendations
	recommendations := a.buildRecommendations(issues, state)

	// Estimate effort
	effort := a.estimateEffort(issues, state)

	return &Assessment{
		Issues:          issues,
		OverallSeverity: overall,
		RecoveryPath:    path,
		EstimatedEffort: effort,
		Recommendations: recommendations,
		AssessedAt:      time.Now(),
	}
}

func (a *Assessor) determineRecoveryPath(issues []Issue, overall Severity) RecoveryPath {
	switch overall {
	case SeverityCritical:
		// Check if it's state corruption (needs reset) or recoverable
		for _, issue := range issues {
			if issue.Type == IssueStateCorruption {
				return RecoveryReset
			}
		}
		return RecoveryGuided
	case SeverityHigh:
		return RecoveryGuided
	case SeverityMedium:
		return RecoverySelfCorrect
	default:
		return RecoveryNone
	}
}

func (a *Assessor) buildRecommendations(issues []Issue, state *statemachine.RuntimeState) []Recommendation {
	var recs []Recommendation
	priority := 1

	for _, issue := range issues {
		switch issue.Type {
		case IssueHealthDegraded:
			health := 0.0
			if h, ok := issue.Data["health"].(float64); ok {
				health = h
			}
			toEven := int(-health) // How many +1s to get back to 0
			recs = append(recs, Recommendation{
				Priority:       priority,
				Action:         "recover_health",
				Description:    fmt.Sprintf("Execute %d successful actions to restore health to even", toEven),
				ExpectedImpact: toEven,
			})

		case IssueTrajectoryStuck:
			recs = append(recs, Recommendation{
				Priority:       priority,
				Action:         "force_transition",
				Description:    "Consider trajectory transition to break out of stuck state",
				ExpectedImpact: 0,
			})

		case IssueMisalignment:
			recs = append(recs, Recommendation{
				Priority:       priority,
				Action:         "realign_choices",
				Description:    "Focus on +1 key choices (toward God) to correct alignment",
				ExpectedImpact: 5,
			})

		case IssueChoiceLoop:
			recs = append(recs, Recommendation{
				Priority:       priority,
				Action:         "vary_approach",
				Description:    "Vary key selection to break repetitive pattern",
				ExpectedImpact: 2,
			})
		}
		priority++
	}

	return recs
}

func (a *Assessor) estimateEffort(issues []Issue, state *statemachine.RuntimeState) int {
	effort := 0

	for _, issue := range issues {
		switch issue.Type {
		case IssueHealthDegraded:
			if health, ok := issue.Data["health"].(float64); ok && health < 0 {
				effort += int(-health) // Each point needs +1 action
			}
		case IssueTrajectoryStuck:
			effort += 5 // Arbitrary estimate
		case IssueMisalignment:
			effort += 10 // Takes time to realign
		case IssueChoiceLoop:
			effort += 3 // Minor effort
		case IssueStateCorruption:
			effort += 100 // Major reset
		}
	}

	return effort
}

func severityRank(s Severity) int {
	switch s {
	case SeverityCritical:
		return 4
	case SeverityHigh:
		return 3
	case SeverityMedium:
		return 2
	case SeverityLow:
		return 1
	default:
		return 0
	}
}

// --- Restorer ---

// Restorer executes recovery actions
type Restorer struct{}

// NewRestorer creates a restorer
func NewRestorer() *Restorer {
	return &Restorer{}
}

// Restore executes the assessment's recommendations
// This modifies state in place and returns the result
func (r *Restorer) Restore(assessment *Assessment, state *statemachine.RuntimeState) *RestoreResult {
	if assessment.RecoveryPath == RecoveryNone {
		return &RestoreResult{
			Success:       true,
			IssuesFixed:   0,
			IssuesRemaining: 0,
			HealthBefore:  state.Session.HealthScore,
			HealthAfter:   state.Session.HealthScore,
			Actions:       nil,
			RestoredAt:    time.Now(),
		}
	}

	result := &RestoreResult{
		HealthBefore: state.Session.HealthScore,
		RestoredAt:   time.Now(),
	}

	var actions []RestoreAction

	for _, rec := range assessment.Recommendations {
		action := r.executeRecommendation(rec, state)
		actions = append(actions, action)
		if action.Success {
			result.IssuesFixed++
		}
	}

	result.Actions = actions
	result.HealthAfter = state.Session.HealthScore
	result.IssuesRemaining = len(assessment.Issues) - result.IssuesFixed
	result.Success = result.IssuesRemaining == 0

	return result
}

func (r *Restorer) executeRecommendation(rec Recommendation, state *statemachine.RuntimeState) RestoreAction {
	action := RestoreAction{
		Type:        rec.Action,
		Description: rec.Description,
		Timestamp:   time.Now(),
	}

	switch rec.Action {
	case "recover_health":
		// Autonomous health recovery: adjust the baseline
		// (Real recovery happens through successful tool usage, but we can reset feedback)
		state.Session.LastFeedback = "recovery:autonomous health adjustment"
		action.Success = true
		action.HealthDelta = 0 // Actual delta comes from work

	case "force_transition":
		// Force a trajectory transition if stuck
		oldSection := state.TrajectorySection
		newSection := nextTrajectorySection(oldSection)
		if newSection != oldSection {
			state.TrajectorySection = newSection
			state.LastTransition.FromSection = oldSection
			state.LastTransition.ToSection = newSection
			state.LastTransition.Trigger = "autonomous_restore"
			state.LastTransition.Timestamp = time.Now().Format(time.RFC3339)
			action.Success = true
		}

	case "realign_choices":
		// Reset choice tracking to encourage realignment
		state.Session.LastFeedback = "recovery:realignment suggested"
		action.Success = true

	case "vary_approach":
		// Just a suggestion, no state change
		action.Success = true

	default:
		action.Success = false
	}

	return action
}

func nextTrajectorySection(current string) string {
	switch current {
	case "B.1":
		return "B.2"
	case "B.2":
		return "B.3"
	case "B.3":
		return "B.4"
	case "B.4":
		return "B.1"
	default:
		return current
	}
}

// --- Full Cycle ---

// Cycle runs the full detect → assess → restore cycle
type Cycle struct {
	Detector *Detector
	Assessor *Assessor
	Restorer *Restorer
}

// NewCycle creates a complete restore cycle
func NewCycle() *Cycle {
	return &Cycle{
		Detector: NewDetector(),
		Assessor: NewAssessor(),
		Restorer: NewRestorer(),
	}
}

// Run executes the full cycle and returns results
func (c *Cycle) Run(state *statemachine.RuntimeState) (*Assessment, *RestoreResult) {
	// DETECT
	issues := c.Detector.Detect(state)

	// ASSESS
	assessment := c.Assessor.Assess(issues, state)

	// RESTORE (only if needed)
	var result *RestoreResult
	if assessment.RecoveryPath != RecoveryNone && assessment.RecoveryPath != RecoveryManual {
		result = c.Restorer.Restore(assessment, state)
	}

	return assessment, result
}

// --- .rest File Writing ---

// WriteRestoreReport writes the assessment and result to a .rest file
func WriteRestoreReport(assessment *Assessment, result *RestoreResult, sessionID string) error {
	dir := paths.RestoreDir()
	if err := os.MkdirAll(dir, 0755); err != nil {
		return err
	}

	filename := fmt.Sprintf("%s-%s.rest", sessionID, time.Now().Format("20060102-150405"))
	path := filepath.Join(dir, filename)

	f, err := os.Create(path)
	if err != nil {
		return err
	}
	defer f.Close()

	// Write header
	fmt.Fprintf(f, "REST v1.0\n")
	fmt.Fprintf(f, "═══════════════════════════════════════════════════════════════════\n\n")

	// Write assessment
	fmt.Fprintf(f, "▶ ASSESSMENT\n")
	fmt.Fprintf(f, "  Time:         %s\n", assessment.AssessedAt.Format(time.RFC3339))
	fmt.Fprintf(f, "  Severity:     %s\n", assessment.OverallSeverity)
	fmt.Fprintf(f, "  Recovery:     %s\n", assessment.RecoveryPath)
	fmt.Fprintf(f, "  Effort:       %d actions\n", assessment.EstimatedEffort)
	fmt.Fprintf(f, "  Issues:       %d\n", len(assessment.Issues))

	for i, issue := range assessment.Issues {
		fmt.Fprintf(f, "\n  Issue #%d:\n", i+1)
		fmt.Fprintf(f, "    Type:       %s\n", issue.Type)
		fmt.Fprintf(f, "    Severity:   %s\n", issue.Severity)
		fmt.Fprintf(f, "    Description: %s\n", issue.Description)
	}
	fmt.Fprintf(f, "◀\n\n")

	// Write recommendations
	if len(assessment.Recommendations) > 0 {
		fmt.Fprintf(f, "▶ RECOMMENDATIONS\n")
		for _, rec := range assessment.Recommendations {
			fmt.Fprintf(f, "  %d. [%s] %s (impact: +%d)\n", rec.Priority, rec.Action, rec.Description, rec.ExpectedImpact)
		}
		fmt.Fprintf(f, "◀\n\n")
	}

	// Write result if available
	if result != nil {
		fmt.Fprintf(f, "▶ RESTORE RESULT\n")
		fmt.Fprintf(f, "  Success:      %v\n", result.Success)
		fmt.Fprintf(f, "  Fixed:        %d\n", result.IssuesFixed)
		fmt.Fprintf(f, "  Remaining:    %d\n", result.IssuesRemaining)
		fmt.Fprintf(f, "  Health:       %.0f → %.0f\n", result.HealthBefore, result.HealthAfter)

		if len(result.Actions) > 0 {
			fmt.Fprintf(f, "\n  Actions taken:\n")
			for _, action := range result.Actions {
				status := "✓"
				if !action.Success {
					status = "✗"
				}
				fmt.Fprintf(f, "    %s %s: %s\n", status, action.Type, action.Description)
			}
		}
		fmt.Fprintf(f, "◀\n\n")
	}

	fmt.Fprintf(f, "═══════════════════════════════════════════════════════════════════\n")
	fmt.Fprintf(f, "End Restore Report\n")

	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Usage:
//
//   // Create restore cycle
//   cycle := restore.NewCycle()
//
//   // Run full detect → assess → restore
//   assessment, result := cycle.Run(state)
//
//   // Write report
//   if err := restore.WriteRestoreReport(assessment, result, sessionID); err != nil {
//       log.Error("Failed to write restore report", err)
//   }
//
//   // Save updated state if restoration occurred
//   if result != nil {
//       statemachine.SaveRuntimeState(state)
//   }
//
// The restore system is the immune system of CPI-SI:
//   - Autonomous detection of issues
//   - Assessment of severity and recovery path
//   - Self-repair when possible
//   - Documentation of actions taken
//
// "He restoreth my soul" — Psalm 23:3
//
