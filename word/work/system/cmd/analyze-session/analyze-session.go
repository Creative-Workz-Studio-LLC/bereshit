// ============================================================================
// METADATA
// ============================================================================
// Session Reflection Analyzer
// Helps reflect on sessions by analyzing activity patterns and comparing to history
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Provide data for reflection - NOT making conclusions
//
// Usage:
//   analyze-session    # Analyze current session
//
// Dependencies: patterns library, session log, activity stream
// Health Scoring: Base100 - Data gathering and pattern tracking

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
	"sort"
	"strings"
	"time"

	"claude/lib/patterns"
)

// ActivityEvent represents a single activity event from JSONL stream
type ActivityEvent struct {
	Ts         string `json:"ts"`
	Tool       string `json:"tool"`
	Ctx        string `json:"ctx"`
	Result     string `json:"result"`
	DurationMs *int   `json:"duration_ms,omitempty"`
}

// SessionData represents current session metadata
type SessionData struct {
	SessionID      string                 `json:"session_id"`
	StartTime      string                 `json:"start_time"`
	Elapsed        *int                   `json:"elapsed,omitempty"`
	DayOfWeek      string                 `json:"day_of_week"`
	TimeOfDay      string                 `json:"time_of_day_category"`
	WorkContext    string                 `json:"work_context"`
	TasksCompleted []string               `json:"tasks_completed,omitempty"`
	QualityInd     map[string]interface{} `json:"quality_indicators,omitempty"`
}

// ============================================================================
// BODY - Core Functionality
// ============================================================================

// readSessionLog reads current-log.json
func readSessionLog() (*SessionData, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return nil, err
	}

	logPath := filepath.Join(homeDir, ".claude/session/current-log.json")
	data, err := os.ReadFile(logPath)
	if err != nil {
		return nil, fmt.Errorf("failed to read session log: %w", err)
	}

	var session SessionData
	if err := json.Unmarshal(data, &session); err != nil {
		return nil, fmt.Errorf("failed to parse session log: %w", err)
	}

	return &session, nil
}

// readActivityStream reads activity JSONL for session
func readActivityStream(sessionID string) ([]ActivityEvent, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return nil, err
	}

	activityPath := filepath.Join(homeDir, ".claude/session/activity", sessionID+".jsonl")
	file, err := os.Open(activityPath)
	if err != nil {
		// Activity stream may not exist - not an error
		return []ActivityEvent{}, nil
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

// analyzeToolUsage counts tool usage from activity events
func analyzeToolUsage(events []ActivityEvent) map[string]int {
	toolCounts := make(map[string]int)

	for _, event := range events {
		if event.Tool != "" &&
			!strings.Contains(event.Tool, "-attempt") &&
			event.Tool != "PromptSubmit" {
			toolCounts[event.Tool]++
		}
	}

	return toolCounts
}

// trackSessionInsights updates patterns.json with session insights
func trackSessionInsights(session *SessionData, events []ActivityEvent) error {
	patternData, err := patterns.ReadPatterns()
	if err != nil {
		return fmt.Errorf("failed to read patterns: %w", err)
	}

	// Initialize state_patterns if nil
	if patternData.StatePatterns == nil {
		patternData.StatePatterns = &patterns.StatePatterns{
			FlowStates: patterns.FlowStates{
				Occurrences: 0,
				Contexts:    []string{},
				Description: "Deep engagement where time disappears, quality stays high",
				Triggers:    []string{},
			},
			QualityDips: patterns.QualityDips{
				Occurrences:              0,
				Timing:                   []string{},
				Description:              "When work quality starts declining",
				TypicalDurationBeforeDip: nil,
			},
		}
	}

	// Detect potential flow state
	// Indicators: High task completion (5+), sustained duration (60+ min)
	if session.TasksCompleted != nil &&
		len(session.TasksCompleted) >= 5 &&
		session.Elapsed != nil &&
		*session.Elapsed >= 60 {

		patternData.StatePatterns.FlowStates.Occurrences++

		// Track context
		context := fmt.Sprintf("%s %s", session.TimeOfDay, session.WorkContext)
		if !contains(patternData.StatePatterns.FlowStates.Contexts, context) {
			patternData.StatePatterns.FlowStates.Contexts = append(
				patternData.StatePatterns.FlowStates.Contexts, context)
		}

		// Track what might have triggered the flow
		toolUsage := analyzeToolUsage(events)
		if len(toolUsage) > 0 {
			// Find most-used tool
			var primaryTool string
			maxCount := 0
			for tool, count := range toolUsage {
				if count > maxCount {
					primaryTool = tool
					maxCount = count
				}
			}

			if primaryTool != "" {
				trigger := fmt.Sprintf("%s (%s)", primaryTool, session.WorkContext)
				if !contains(patternData.StatePatterns.FlowStates.Triggers, trigger) {
					patternData.StatePatterns.FlowStates.Triggers = append(
						patternData.StatePatterns.FlowStates.Triggers, trigger)
				}
			}
		}
	}

	// Track session duration for quality dip detection
	// Very long sessions (180+ min) noted for quality awareness
	if session.Elapsed != nil && *session.Elapsed >= 180 {
		timingEntry := fmt.Sprintf("%dm session at %s", *session.Elapsed, session.TimeOfDay)
		patternData.StatePatterns.QualityDips.Timing = append(
			patternData.StatePatterns.QualityDips.Timing, timingEntry)

		// Keep only last 10 timing entries
		if len(patternData.StatePatterns.QualityDips.Timing) > 10 {
			start := len(patternData.StatePatterns.QualityDips.Timing) - 10
			patternData.StatePatterns.QualityDips.Timing =
				patternData.StatePatterns.QualityDips.Timing[start:]
		}

		// Update typical duration before dip (average of long sessions)
		var totalDuration int
		count := 0
		for _, timing := range patternData.StatePatterns.QualityDips.Timing {
			var duration int
			if _, err := fmt.Sscanf(timing, "%dm", &duration); err == nil && duration > 0 {
				totalDuration += duration
				count++
			}
		}
		if count > 0 {
			avgDuration := totalDuration / count
			patternData.StatePatterns.QualityDips.TypicalDurationBeforeDip = &avgDuration
		}
	}

	// Update last_updated timestamp
	patternData.LastUpdated = time.Now().Format(time.RFC3339)

	// Write back
	return patterns.WritePatterns(patternData)
}

// generateReflectionPrompts creates reflection questions based on session
func generateReflectionPrompts(session *SessionData, events []ActivityEvent) string {
	var lines []string

	// Session context
	lines = append(lines, "\n📅 Session Context:")
	lines = append(lines, fmt.Sprintf("   %s %s", session.DayOfWeek, session.TimeOfDay))

	// Duration
	if session.Elapsed != nil {
		hours := *session.Elapsed / 60
		mins := *session.Elapsed % 60
		durationStr := ""
		if hours > 0 {
			durationStr = fmt.Sprintf("%dh ", hours)
		}
		durationStr += fmt.Sprintf("%dm", mins)
		lines = append(lines, fmt.Sprintf("   Duration: %s", durationStr))
	}

	// Tasks completed
	if session.TasksCompleted != nil && len(session.TasksCompleted) > 0 {
		lines = append(lines, fmt.Sprintf("\n✅ Tasks Completed: %d", len(session.TasksCompleted)))
		maxTasks := 5
		if len(session.TasksCompleted) < maxTasks {
			maxTasks = len(session.TasksCompleted)
		}
		for i := 0; i < maxTasks; i++ {
			lines = append(lines, fmt.Sprintf("   • %s", session.TasksCompleted[i]))
		}
	}

	// Tool usage
	toolUsage := analyzeToolUsage(events)
	if len(toolUsage) > 0 {
		// Sort by count descending
		type toolCount struct {
			tool  string
			count int
		}
		var tools []toolCount
		for tool, count := range toolUsage {
			tools = append(tools, toolCount{tool, count})
		}
		sort.Slice(tools, func(i, j int) bool {
			return tools[i].count > tools[j].count
		})

		lines = append(lines, "\n🛠️  Primary Tools:")
		maxTools := 5
		if len(tools) < maxTools {
			maxTools = len(tools)
		}
		for i := 0; i < maxTools; i++ {
			lines = append(lines, fmt.Sprintf("   • %s: %d uses", tools[i].tool, tools[i].count))
		}
	}

	// Reflection questions
	lines = append(lines, "\n💭 Reflection Questions:")
	lines = append(lines, "   • What did I actually learn this session?")
	lines = append(lines, "   • What approach did I take to problems?")
	lines = append(lines, "   • When was quality highest?")
	lines = append(lines, "   • What energized vs drained me?")
	lines = append(lines, "   • Did any patterns emerge?")

	return strings.Join(lines, "\n")
}

// contains checks if slice contains string
func contains(slice []string, value string) bool {
	for _, v := range slice {
		if v == value {
			return true
		}
	}
	return false
}

// ============================================================================
// CLOSING - Main Entry Point
// ============================================================================

func main() {
	fmt.Println("📊 Session Reflection Analysis\n")

	// Read session log
	session, err := readSessionLog()
	if err != nil {
		fmt.Fprintf(os.Stderr, "❌ Could not read session log: %v\n", err)
		os.Exit(1)
	}

	// Read activity stream
	events, err := readActivityStream(session.SessionID)
	if err != nil {
		fmt.Fprintf(os.Stderr, "⚠️  Warning: Could not read activity stream: %v\n", err)
		// Continue anyway with empty events
		events = []ActivityEvent{}
	}

	// Display analysis
	fmt.Printf("Session: %s\n", session.SessionID)
	fmt.Println(generateReflectionPrompts(session, events))

	fmt.Println("\n💡 This is DATA for reflection, not conclusions.")
	fmt.Println("   You decide what it means. What did YOU learn?")

	// Track session insights
	fmt.Println("\n📊 Updating Pattern Memory...\n")
	if err := trackSessionInsights(session, events); err != nil {
		fmt.Printf("   ⚠️  Failed to write patterns: %v\n\n", err)
		os.Exit(1)
	}

	fmt.Println("   ✅ Session insights written to patterns.json")
	fmt.Println("   📍 Location: ~/.claude/session/patterns.json")
	fmt.Println("   🧠 Pattern memory updated with session insights\n")

	os.Exit(0)
}
