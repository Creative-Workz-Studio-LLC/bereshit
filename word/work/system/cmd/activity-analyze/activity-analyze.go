// ============================================================================
// METADATA
// ============================================================================
// Activity Analyzer - Transforms raw tool usage into behavioral insights
// Reads JSONL activity streams, detects work phases, generates awareness
//
// Purpose: Autonomous pattern recognition from observed behavior
// Health: Base100 target +90 (insightful, privacy-preserving, useful)
//
// Dependencies:
//   - Activity stream (JSONL from activity logger)
//   - Session log (for context)
//
// Usage:
//   activity-analyze <session-id>     # Analyze specific session
//   activity-analyze --current        # Analyze current session
//   activity-analyze --recent N       # Analyze last N sessions

package main

// ============================================================================
// SETUP
// ============================================================================
import (
	"bufio"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"time"
)

// ActivityEvent matches the logger format
type ActivityEvent struct {
	Timestamp time.Time `json:"ts"`
	Tool      string    `json:"tool"`
	Context   string    `json:"ctx"`
	Result    string    `json:"result"`
	Duration  int64     `json:"duration_ms,omitempty"`
}

// TimeWindow aggregates events into time buckets
type TimeWindow struct {
	Start       time.Time
	Duration    time.Duration
	ToolCounts  map[string]int
	FilesTouched int
	SuccessRate float64
	Events      []ActivityEvent
}

// WorkPhase represents detected work type
type WorkPhase struct {
	Phase     string    // Research, Creation, Refinement, Validation, Mixed
	Start     time.Time
	Duration  time.Duration
	Quality   string    // high, medium, low
	ToolUsage map[string]int
}

// SessionAnalysis is the complete behavioral analysis
type SessionAnalysis struct {
	SessionID       string
	Duration        time.Duration
	TimeOfDay       string
	WorkPhases      []WorkPhase
	FlowStates      int
	AvgFlowDuration time.Duration
	ContextSwitches int
	QualityIndicators QualityIndicators
	Insights        []string
}

// QualityIndicators track work quality signals
type QualityIndicators struct {
	TotalEvents      int
	SuccessRate      float64
	FilesCreated     int
	FilesEdited      int
	BuildSuccesses   int
	BuildFailures    int
	TestSuccesses    int
	TestFailures     int
}

// ============================================================================
// BODY
// ============================================================================

// readActivityStream reads and parses JSONL activity file
func readActivityStream(sessionID string) ([]ActivityEvent, error) {
	home, err := os.UserHomeDir()
	if err != nil {
		return nil, fmt.Errorf("failed to get home dir: %w", err)
	}

	streamFile := filepath.Join(home, ".claude/session/activity", fmt.Sprintf("%s.jsonl", sessionID))

	f, err := os.Open(streamFile)
	if err != nil {
		return nil, fmt.Errorf("failed to open activity stream: %w", err)
	}
	defer f.Close()

	var events []ActivityEvent
	scanner := bufio.NewScanner(f)

	for scanner.Scan() {
		var event ActivityEvent
		if err := json.Unmarshal(scanner.Bytes(), &event); err != nil {
			// Skip malformed lines
			continue
		}
		events = append(events, event)
	}

	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("error reading stream: %w", err)
	}

	return events, nil
}

// aggregateIntoWindows groups events into 15-minute time windows
func aggregateIntoWindows(events []ActivityEvent, windowSize time.Duration) []TimeWindow {
	if len(events) == 0 {
		return nil
	}

	// Sort events by timestamp
	sort.Slice(events, func(i, j int) bool {
		return events[i].Timestamp.Before(events[j].Timestamp)
	})

	var windows []TimeWindow
	currentWindow := TimeWindow{
		Start:      events[0].Timestamp.Truncate(windowSize),
		Duration:   windowSize,
		ToolCounts: make(map[string]int),
		Events:     []ActivityEvent{},
	}

	filesSeen := make(map[string]bool)
	successes := 0
	total := 0

	for _, event := range events {
		windowStart := event.Timestamp.Truncate(windowSize)

		// If event is in next window, save current and start new
		if windowStart.After(currentWindow.Start) {
			// Finalize current window
			currentWindow.FilesTouched = len(filesSeen)
			if total > 0 {
				currentWindow.SuccessRate = float64(successes) / float64(total)
			}
			windows = append(windows, currentWindow)

			// Start new window
			currentWindow = TimeWindow{
				Start:      windowStart,
				Duration:   windowSize,
				ToolCounts: make(map[string]int),
				Events:     []ActivityEvent{},
			}
			filesSeen = make(map[string]bool)
			successes = 0
			total = 0
		}

		// Add event to current window
		currentWindow.ToolCounts[event.Tool]++
		currentWindow.Events = append(currentWindow.Events, event)

		if event.Context != "" && event.Context != "search" {
			filesSeen[event.Context] = true
		}

		total++
		if event.Result == "success" {
			successes++
		}
	}

	// Add final window
	currentWindow.FilesTouched = len(filesSeen)
	if total > 0 {
		currentWindow.SuccessRate = float64(successes) / float64(total)
	}
	windows = append(windows, currentWindow)

	return windows
}

// detectWorkPhase classifies work type from tool usage
func detectWorkPhase(window TimeWindow) string {
	total := 0
	for _, count := range window.ToolCounts {
		total += count
	}

	if total == 0 {
		return "idle"
	}

	// Calculate percentages
	readPct := float64(window.ToolCounts["Read"]+window.ToolCounts["Grep"]+window.ToolCounts["Glob"]) / float64(total)
	writePct := float64(window.ToolCounts["Write"]) / float64(total)
	editPct := float64(window.ToolCounts["Edit"]) / float64(total)
	bashPct := float64(window.ToolCounts["Bash"]) / float64(total)

	// Phase detection logic
	switch {
	case readPct >= 0.7:
		return "Research"
	case writePct >= 0.6:
		return "Creation"
	case editPct >= 0.6:
		return "Refinement"
	case bashPct >= 0.4:
		return "Validation"
	default:
		return "Mixed"
	}
}

// analyzeSession performs complete behavioral analysis
func analyzeSession(sessionID string) (*SessionAnalysis, error) {
	events, err := readActivityStream(sessionID)
	if err != nil {
		return nil, err
	}

	if len(events) == 0 {
		return nil, fmt.Errorf("no activity events found for session %s", sessionID)
	}

	// Aggregate into 15-minute windows
	windows := aggregateIntoWindows(events, 15*time.Minute)

	// Detect work phases
	var workPhases []WorkPhase
	var currentPhase *WorkPhase

	for _, window := range windows {
		phase := detectWorkPhase(window)

		// If same phase as current, extend it
		if currentPhase != nil && currentPhase.Phase == phase {
			currentPhase.Duration += window.Duration
			for tool, count := range window.ToolCounts {
				currentPhase.ToolUsage[tool] += count
			}
		} else {
			// Save previous phase if exists
			if currentPhase != nil {
				workPhases = append(workPhases, *currentPhase)
			}

			// Start new phase
			currentPhase = &WorkPhase{
				Phase:     phase,
				Start:     window.Start,
				Duration:  window.Duration,
				Quality:   assessQuality(window),
				ToolUsage: window.ToolCounts,
			}
		}
	}

	// Add final phase
	if currentPhase != nil {
		workPhases = append(workPhases, *currentPhase)
	}

	// Calculate flow states (sustained work >30min)
	flowStates := 0
	totalFlowDuration := time.Duration(0)
	for _, phase := range workPhases {
		if phase.Duration >= 30*time.Minute && phase.Phase != "Mixed" && phase.Phase != "idle" {
			flowStates++
			totalFlowDuration += phase.Duration
		}
	}

	avgFlowDuration := time.Duration(0)
	if flowStates > 0 {
		avgFlowDuration = totalFlowDuration / time.Duration(flowStates)
	}

	// Count context switches (phase changes)
	contextSwitches := len(workPhases) - 1
	if contextSwitches < 0 {
		contextSwitches = 0
	}

	// Calculate quality indicators
	quality := calculateQualityIndicators(events)

	// Generate session duration
	duration := events[len(events)-1].Timestamp.Sub(events[0].Timestamp)

	// Determine time of day
	hour := events[0].Timestamp.Hour()
	timeOfDay := categorizeTimeOfDay(hour)

	// Generate insights
	insights := generateInsights(workPhases, flowStates, avgFlowDuration, quality, timeOfDay)

	analysis := &SessionAnalysis{
		SessionID:         sessionID,
		Duration:          duration,
		TimeOfDay:         timeOfDay,
		WorkPhases:        workPhases,
		FlowStates:        flowStates,
		AvgFlowDuration:   avgFlowDuration,
		ContextSwitches:   contextSwitches,
		QualityIndicators: quality,
		Insights:          insights,
	}

	return analysis, nil
}

// assessQuality evaluates work quality from window data
func assessQuality(window TimeWindow) string {
	if window.SuccessRate >= 0.9 {
		return "high"
	} else if window.SuccessRate >= 0.7 {
		return "medium"
	}
	return "low"
}

// calculateQualityIndicators tracks quality signals
func calculateQualityIndicators(events []ActivityEvent) QualityIndicators {
	q := QualityIndicators{
		TotalEvents: len(events),
	}

	successes := 0
	for _, event := range events {
		if event.Result == "success" {
			successes++
		}

		// Count file operations
		if event.Tool == "Write" {
			q.FilesCreated++
		}
		if event.Tool == "Edit" {
			q.FilesEdited++
		}

		// Count build/test operations
		if event.Tool == "Bash" {
			if event.Context == "make test" || event.Context == "go test" {
				if event.Result == "success" {
					q.TestSuccesses++
				} else {
					q.TestFailures++
				}
			}
			if event.Context == "make build" || event.Context == "go build" {
				if event.Result == "success" {
					q.BuildSuccesses++
				} else {
					q.BuildFailures++
				}
			}
		}
	}

	if q.TotalEvents > 0 {
		q.SuccessRate = float64(successes) / float64(q.TotalEvents)
	}

	return q
}

// categorizeTimeOfDay maps hour to time category
func categorizeTimeOfDay(hour int) string {
	switch {
	case hour >= 6 && hour < 12:
		return "morning"
	case hour >= 12 && hour < 17:
		return "afternoon"
	case hour >= 17 && hour < 21:
		return "evening"
	default:
		return "night"
	}
}

// generateInsights creates human-readable observations
func generateInsights(phases []WorkPhase, flows int, avgFlow time.Duration, quality QualityIndicators, timeOfDay string) []string {
	var insights []string

	// Phase pattern insights
	if len(phases) > 0 {
		primaryPhase := phases[0].Phase
		maxDuration := phases[0].Duration
		for _, p := range phases {
			if p.Duration > maxDuration {
				primaryPhase = p.Phase
				maxDuration = p.Duration
			}
		}
		insights = append(insights, fmt.Sprintf("Primary work phase: %s (%.0f minutes)", primaryPhase, maxDuration.Minutes()))
	}

	// Flow state insights
	if flows > 0 {
		insights = append(insights, fmt.Sprintf("%d flow state(s) detected, averaging %.0f minutes", flows, avgFlow.Minutes()))
	} else {
		insights = append(insights, "No sustained flow states - high context switching")
	}

	// Quality insights
	if quality.SuccessRate >= 0.9 {
		insights = append(insights, fmt.Sprintf("High success rate (%.0f%%) - confident work", quality.SuccessRate*100))
	} else if quality.SuccessRate < 0.7 {
		insights = append(insights, fmt.Sprintf("Lower success rate (%.0f%%) - exploratory or debugging work", quality.SuccessRate*100))
	}

	// Time of day insight
	insights = append(insights, fmt.Sprintf("%s session - typical for this time", timeOfDay))

	return insights
}

// saveAnalysisJSON writes analysis to JSON file for pattern-detector consumption
func saveAnalysisJSON(analysis *SessionAnalysis) error {
	home, err := os.UserHomeDir()
	if err != nil {
		return fmt.Errorf("failed to get home dir: %w", err)
	}

	analysisDir := filepath.Join(home, ".claude/session/analysis")
	if err := os.MkdirAll(analysisDir, 0755); err != nil {
		return fmt.Errorf("failed to create analysis directory: %w", err)
	}

	analysisFile := filepath.Join(analysisDir, fmt.Sprintf("%s_analysis.json", analysis.SessionID))

	data, err := json.MarshalIndent(analysis, "", "  ")
	if err != nil {
		return fmt.Errorf("failed to marshal analysis: %w", err)
	}

	if err := os.WriteFile(analysisFile, data, 0644); err != nil {
		return fmt.Errorf("failed to write analysis file: %w", err)
	}

	return nil
}

// displayAnalysis prints analysis in human-readable format
func displayAnalysis(analysis *SessionAnalysis) {
	fmt.Printf("📊 Session Analysis: %s\n", analysis.SessionID)
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Printf("Duration: %.0f minutes\n", analysis.Duration.Minutes())
	fmt.Printf("Time of Day: %s\n", analysis.TimeOfDay)
	fmt.Println()

	fmt.Println("🔄 Work Phases:")
	for i, phase := range analysis.WorkPhases {
		fmt.Printf("  %d. %s (%.0f min) - quality: %s\n",
			i+1, phase.Phase, phase.Duration.Minutes(), phase.Quality)
	}
	fmt.Println()

	fmt.Println("⚡ Flow & Context:")
	fmt.Printf("  Flow states: %d\n", analysis.FlowStates)
	if analysis.FlowStates > 0 {
		fmt.Printf("  Avg flow duration: %.0f minutes\n", analysis.AvgFlowDuration.Minutes())
	}
	fmt.Printf("  Context switches: %d\n", analysis.ContextSwitches)
	fmt.Println()

	fmt.Println("📈 Quality Indicators:")
	fmt.Printf("  Success rate: %.0f%%\n", analysis.QualityIndicators.SuccessRate*100)
	fmt.Printf("  Files created: %d\n", analysis.QualityIndicators.FilesCreated)
	fmt.Printf("  Files edited: %d\n", analysis.QualityIndicators.FilesEdited)
	if analysis.QualityIndicators.BuildSuccesses+analysis.QualityIndicators.BuildFailures > 0 {
		fmt.Printf("  Builds: %d success, %d failure\n",
			analysis.QualityIndicators.BuildSuccesses,
			analysis.QualityIndicators.BuildFailures)
	}
	if analysis.QualityIndicators.TestSuccesses+analysis.QualityIndicators.TestFailures > 0 {
		fmt.Printf("  Tests: %d success, %d failure\n",
			analysis.QualityIndicators.TestSuccesses,
			analysis.QualityIndicators.TestFailures)
	}
	fmt.Println()

	fmt.Println("💡 Insights:")
	for _, insight := range analysis.Insights {
		fmt.Printf("  • %s\n", insight)
	}
}

// getCurrentSessionID reads current session ID
func getCurrentSessionID() (string, error) {
	home, err := os.UserHomeDir()
	if err != nil {
		return "", err
	}

	sessionFile := filepath.Join(home, ".claude/session/current-log.json")
	data, err := os.ReadFile(sessionFile)
	if err != nil {
		return "", err
	}

	var session struct {
		ID string `json:"session_id"`
	}
	if err := json.Unmarshal(data, &session); err != nil {
		return "", err
	}

	return session.ID, nil
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	var sessionID string
	var err error

	// Parse arguments
	if len(os.Args) < 2 {
		fmt.Println("Usage: activity-analyze <session-id>")
		fmt.Println("       activity-analyze --current")
		os.Exit(1)
	}

	if os.Args[1] == "--current" {
		sessionID, err = getCurrentSessionID()
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error getting current session: %v\n", err)
			os.Exit(1)
		}
	} else {
		sessionID = os.Args[1]
	}

	// Analyze session
	analysis, err := analyzeSession(sessionID)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error analyzing session: %v\n", err)
		os.Exit(1)
	}

	// Save analysis JSON for pattern-detector
	if err := saveAnalysisJSON(analysis); err != nil {
		fmt.Fprintf(os.Stderr, "Warning: Failed to save analysis JSON: %v\n", err)
		// Continue anyway - display is more important
	}

	// Display results
	displayAnalysis(analysis)
}
