// ============================================================================
// METADATA
// ============================================================================
// Session Time Awareness - 3-Stage Time Tracking
// Distinguishes: Pure-Downtime, Semi-Downtime, Uptime
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Understand actual work time vs idle time within sessions
//
// 3 Stages:
//   1. Pure-Downtime: Claude Code not running (between sessions)
//   2. Semi-Downtime: Session open but user asleep (idle >30min)
//   3. Uptime: Actively working (recent activity)
//
// Usage:
//   session-time awareness         # Show 3-stage breakdown
//   session-time awareness --json  # JSON output
//
// Dependencies: Activity stream, session state
// Health Scoring: Base100 - Activity analysis for genuine time awareness

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================

import (
	"bufio"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"

	"system/lib/planner"
)

// ActivityEvent from JSONL stream
type ActivityEvent struct {
	Ts   string `json:"ts"`
	Tool string `json:"tool"`
}

// TimeAwareness represents 3-stage breakdown
type TimeAwareness struct {
	WallClockElapsed time.Duration `json:"wall_clock_elapsed_seconds"`
	ActiveUptime     time.Duration `json:"active_uptime_seconds"`
	SemiDowntime     time.Duration `json:"semi_downtime_seconds"`
	LastActivity     time.Time     `json:"last_activity"`
	ActivityGaps     []ActivityGap `json:"activity_gaps"`
	CurrentState     string        `json:"current_state"` // "uptime" or "semi_downtime"
}

// ActivityGap represents an idle period
type ActivityGap struct {
	Start    time.Time     `json:"start"`
	End      time.Time     `json:"end"`
	Duration time.Duration `json:"duration_seconds"`
}

const (
	semiDowntimeThreshold = 30 * time.Minute // 30min idle = semi-downtime
	sessionDir            = ".claude/session"
	activityDir           = "activity"
)

// ============================================================================
// BODY - Core Functionality
// ============================================================================

// getSessionID reads current session ID from current-log.json
func getSessionID() (string, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return "", err
	}

	logPath := filepath.Join(homeDir, sessionDir, "current-log.json")
	data, err := os.ReadFile(logPath)
	if err != nil {
		return "", fmt.Errorf("failed to read current-log.json: %w", err)
	}

	var logData struct {
		SessionID string `json:"session_id"`
	}

	if err := json.Unmarshal(data, &logData); err != nil {
		return "", fmt.Errorf("failed to parse current-log.json: %w", err)
	}

	return logData.SessionID, nil
}

// readActivityStream reads all activity events for current session
func readActivityStream(sessionID string) ([]ActivityEvent, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return nil, err
	}

	activityPath := filepath.Join(homeDir, sessionDir, activityDir, sessionID+".jsonl")

	// Activity file might not exist yet - that's okay
	file, err := os.Open(activityPath)
	if err != nil {
		if os.IsNotExist(err) {
			return []ActivityEvent{}, nil
		}
		return nil, err
	}
	defer file.Close()

	var events []ActivityEvent
	scanner := bufio.NewScanner(file)

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}

		var event ActivityEvent
		if err := json.Unmarshal([]byte(line), &event); err != nil {
			// Skip malformed lines
			continue
		}

		events = append(events, event)
	}

	return events, nil
}

// parseActivityTimestamp converts activity timestamp to time.Time
func parseActivityTimestamp(ts string) (time.Time, error) {
	return time.Parse(time.RFC3339Nano, ts)
}

// analyzeTimeAwareness calculates 3-stage breakdown
func analyzeTimeAwareness(sessionStart time.Time, events []ActivityEvent) TimeAwareness {
	now := time.Now()
	wallClock := now.Sub(sessionStart)

	if len(events) == 0 {
		// No activity yet - entire session is semi-downtime
		return TimeAwareness{
			WallClockElapsed: wallClock,
			ActiveUptime:     0,
			SemiDowntime:     wallClock,
			LastActivity:     sessionStart,
			ActivityGaps:     []ActivityGap{},
			CurrentState:     "semi_downtime",
		}
	}

	// Parse activity timestamps
	var activityTimes []time.Time
	for _, event := range events {
		t, err := parseActivityTimestamp(event.Ts)
		if err == nil {
			activityTimes = append(activityTimes, t)
		}
	}

	if len(activityTimes) == 0 {
		return TimeAwareness{
			WallClockElapsed: wallClock,
			ActiveUptime:     0,
			SemiDowntime:     wallClock,
			LastActivity:     sessionStart,
			ActivityGaps:     []ActivityGap{},
			CurrentState:     "semi_downtime",
		}
	}

	// Find gaps between activities
	var gaps []ActivityGap
	var totalGapTime time.Duration

	// Gap from session start to first activity
	firstActivity := activityTimes[0]
	if firstActivity.Sub(sessionStart) > semiDowntimeThreshold {
		gap := ActivityGap{
			Start:    sessionStart,
			End:      firstActivity,
			Duration: firstActivity.Sub(sessionStart),
		}
		gaps = append(gaps, gap)
		totalGapTime += gap.Duration
	}

	// Gaps between activities
	for i := 1; i < len(activityTimes); i++ {
		prev := activityTimes[i-1]
		curr := activityTimes[i]
		gapDuration := curr.Sub(prev)

		if gapDuration > semiDowntimeThreshold {
			gap := ActivityGap{
				Start:    prev,
				End:      curr,
				Duration: gapDuration,
			}
			gaps = append(gaps, gap)
			totalGapTime += gapDuration
		}
	}

	// Gap from last activity to now
	lastActivity := activityTimes[len(activityTimes)-1]
	currentGap := now.Sub(lastActivity)
	currentState := "uptime"

	if currentGap > semiDowntimeThreshold {
		gap := ActivityGap{
			Start:    lastActivity,
			End:      now,
			Duration: currentGap,
		}
		gaps = append(gaps, gap)
		totalGapTime += currentGap
		currentState = "semi_downtime"
	}

	activeUptime := wallClock - totalGapTime

	return TimeAwareness{
		WallClockElapsed: wallClock,
		ActiveUptime:     activeUptime,
		SemiDowntime:     totalGapTime,
		LastActivity:     lastActivity,
		ActivityGaps:     gaps,
		CurrentState:     currentState,
	}
}

// formatTimeAwareness creates human-readable output
func formatTimeAwareness(awareness TimeAwareness, sessionStart time.Time) string {
	var out strings.Builder

	out.WriteString("\n🕐 3-Stage Time Awareness\n")
	out.WriteString(strings.Repeat("━", 65) + "\n\n")

	// Wall-clock time
	out.WriteString(fmt.Sprintf("⏱️  Wall-Clock Elapsed:  %s\n", formatDuration(awareness.WallClockElapsed)))
	out.WriteString(fmt.Sprintf("   Session started: %s\n\n", sessionStart.Format("Mon Jan 02, 2006 at 15:04:05")))

	// Active uptime
	uptimePercent := 0.0
	if awareness.WallClockElapsed > 0 {
		uptimePercent = (float64(awareness.ActiveUptime) / float64(awareness.WallClockElapsed)) * 100
	}
	out.WriteString(fmt.Sprintf("✅ Active Uptime:        %s (%.0f%%)\n",
		formatDuration(awareness.ActiveUptime), uptimePercent))
	out.WriteString("   Time actively working\n\n")

	// Semi-downtime
	downtimePercent := 0.0
	if awareness.WallClockElapsed > 0 {
		downtimePercent = (float64(awareness.SemiDowntime) / float64(awareness.WallClockElapsed)) * 100
	}
	out.WriteString(fmt.Sprintf("💤 Semi-Downtime:        %s (%.0f%%)\n",
		formatDuration(awareness.SemiDowntime), downtimePercent))
	out.WriteString("   Session open but idle (>30min gaps)\n\n")

	// Current state
	stateIcon := "✅"
	stateText := "UPTIME - Actively working"
	if awareness.CurrentState == "semi_downtime" {
		stateIcon = "💤"
		stateText = "SEMI-DOWNTIME - Idle"
	}

	out.WriteString(fmt.Sprintf("%s Current State:       %s\n", stateIcon, stateText))
	out.WriteString(fmt.Sprintf("   Last activity: %s\n", awareness.LastActivity.Format("15:04:05")))

	timeSinceActivity := time.Since(awareness.LastActivity)
	if timeSinceActivity < semiDowntimeThreshold {
		out.WriteString(fmt.Sprintf("   Active %s ago\n", formatDuration(timeSinceActivity)))
	} else {
		out.WriteString(fmt.Sprintf("   Idle for %s\n", formatDuration(timeSinceActivity)))
	}

	// Activity gaps with downtime classification
	if len(awareness.ActivityGaps) > 0 {
		// Load planner using extracted library
		plan, _ := planner.LoadPlanner("seanje")

		out.WriteString(fmt.Sprintf("\n💤 Idle Periods: %d gap(s) detected\n", len(awareness.ActivityGaps)))
		for i, gap := range awareness.ActivityGaps {
			if i < 5 { // Show first 5 gaps
				// Classify this gap
				expected, reason := classifyDowntime(gap.Start, plan)

				if expected {
					out.WriteString(fmt.Sprintf("   %d. %s (duration: %s) ✅ Expected: %s\n",
						i+1, gap.Start.Format("15:04"), formatDuration(gap.Duration), reason))
				} else {
					out.WriteString(fmt.Sprintf("   %d. %s (duration: %s) ❓ Unexpected downtime\n",
						i+1, gap.Start.Format("15:04"), formatDuration(gap.Duration)))
				}
			}
		}
		if len(awareness.ActivityGaps) > 5 {
			out.WriteString(fmt.Sprintf("   ... and %d more\n", len(awareness.ActivityGaps)-5))
		}
	}

	// Schedule context (if available)
	if schedule := loadScheduleQuiet(); schedule != nil {
		out.WriteString("\n📅 Internal Calendar:\n")
		out.WriteString(fmt.Sprintf("   Work: %s\n", schedule.WorkItem))
		out.WriteString(fmt.Sprintf("   Session %d of %d | Day %d of %d | %.0f%% complete\n",
			schedule.Progress.CurrentSessionNumber,
			schedule.Estimates.TotalSessions,
			schedule.Progress.DaysElapsed+1,
			schedule.Estimates.TotalDays,
			calculatePercent(schedule.Progress.SessionsCompleted, schedule.Estimates.TotalSessions)))
	}

	out.WriteString("\n" + strings.Repeat("━", 65) + "\n\n")
	out.WriteString("💡 Understanding:\n")
	out.WriteString("   Wall-Clock = Total time session has been open\n")
	out.WriteString("   Uptime     = Time actively working (tool usage, prompts)\n")
	out.WriteString("   Semi-Down  = Idle periods >30min (likely asleep/away)\n\n")

	return out.String()
}

// Schedule types for integration
type Schedule struct {
	WorkItem  string    `json:"work_item"`
	Estimates Estimates `json:"estimates"`
	Progress  Progress  `json:"progress"`
}

type Estimates struct {
	TotalDays     int `json:"total_days"`
	TotalSessions int `json:"total_sessions"`
}

type Progress struct {
	DaysElapsed          int `json:"days_elapsed"`
	SessionsCompleted    int `json:"sessions_completed"`
	CurrentSessionNumber int `json:"current_session_number"`
}

// loadScheduleQuiet loads schedule if exists, returns nil if not (no error display)
func loadScheduleQuiet() *Schedule {
	scheduleFile := filepath.Join(os.Getenv("HOME"), ".claude", "schedule", "current-schedule.json")
	data, err := os.ReadFile(scheduleFile)
	if err != nil {
		return nil
	}

	var schedule Schedule
	if err := json.Unmarshal(data, &schedule); err != nil {
		return nil
	}

	return &schedule
}

// classifyDowntime checks if a time falls within expected downtime windows using planner library
func classifyDowntime(t time.Time, plan *planner.Planner) (bool, string) {
	if plan == nil {
		return false, ""
	}

	timeMinutes := t.Hour()*60 + t.Minute()
	weekday := strings.ToLower(t.Weekday().String())

	// Check daily patterns (sleep, meals, etc.)
	for _, block := range plan.RecurringPatterns.Daily {
		if planner.IsTimeInBlock(timeMinutes, block) {
			return true, fmt.Sprintf("%s (%s)", block.Description, block.Type)
		}
	}

	// Check weekly patterns for this day of week
	if weeklyData, exists := plan.RecurringPatterns.Weekly[weekday]; exists {
		if weeklyArray, ok := weeklyData.([]interface{}); ok {
			for _, item := range weeklyArray {
				if blockMap, ok := item.(map[string]interface{}); ok {
					block := planner.ParseTimeBlock(blockMap)
					if planner.IsTimeInBlock(timeMinutes, block) {
						return true, fmt.Sprintf("%s (%s)", block.Description, block.Type)
					}
				}
			}
		}
	}

	return false, ""
}

// calculatePercent calculates percentage
func calculatePercent(completed, total int) float64 {
	if total == 0 {
		return 0
	}
	return float64(completed) / float64(total) * 100
}

// formatDuration formats duration in human-readable form
func formatDuration(d time.Duration) string {
	seconds := int(d.Seconds())

	if seconds < 60 {
		return fmt.Sprintf("%ds", seconds)
	} else if seconds < 3600 {
		minutes := seconds / 60
		secs := seconds % 60
		return fmt.Sprintf("%dm%ds", minutes, secs)
	} else if seconds < 86400 {
		hours := seconds / 3600
		minutes := (seconds % 3600) / 60
		return fmt.Sprintf("%dh%dm", hours, minutes)
	}

	days := seconds / 86400
	hours := (seconds % 86400) / 3600
	return fmt.Sprintf("%dd%dh", days, hours)
}

// ============================================================================
// CLOSING - Main Entry Point
// ============================================================================

func main() {
	// Get session ID
	sessionID, err := getSessionID()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error reading session: %v\n", err)
		os.Exit(1)
	}

	// Get session start time from current.json
	homeDir, _ := os.UserHomeDir()
	sessionPath := filepath.Join(homeDir, sessionDir, "current.json")
	data, err := os.ReadFile(sessionPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error reading session state: %v\n", err)
		os.Exit(1)
	}

	var sessionState struct {
		StartTime time.Time `json:"start_time"`
	}
	if err := json.Unmarshal(data, &sessionState); err != nil {
		fmt.Fprintf(os.Stderr, "Error parsing session state: %v\n", err)
		os.Exit(1)
	}

	// Read activity stream
	events, err := readActivityStream(sessionID)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error reading activity: %v\n", err)
		os.Exit(1)
	}

	// Analyze time awareness
	awareness := analyzeTimeAwareness(sessionState.StartTime, events)

	// Check for --json flag
	jsonOutput := false
	if len(os.Args) > 1 && os.Args[1] == "--json" {
		jsonOutput = true
	}

	if jsonOutput {
		data, _ := json.MarshalIndent(awareness, "", "  ")
		fmt.Println(string(data))
	} else {
		fmt.Print(formatTimeAwareness(awareness, sessionState.StartTime))
	}

	os.Exit(0)
}
