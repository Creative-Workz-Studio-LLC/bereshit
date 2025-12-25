// ============================================================================
// METADATA
// ============================================================================
// Pattern Detector - Scan session history for recurring patterns
// Suggests possible patterns for conscious recognition - does NOT conclude
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Data-driven pattern suggestion for genuine self-recognition
//
// Usage:
//   pattern-detector    # Analyze all session history
//
// Dependencies: patterns library, session history
// Health Scoring: Base100 - Historical data analysis

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================

import (
	"encoding/json"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
	"strings"
	"time"

	"claude/lib/patterns"
)

// SessionHistory represents a completed session
type SessionHistory struct {
	SessionID      string   `json:"session_id"`
	StartTime      string   `json:"start_time"`
	EndTime        string   `json:"end_time,omitempty"`
	Duration       *int     `json:"duration_minutes,omitempty"`
	TimeOfDay      string   `json:"time_of_day_category"`
	TasksCompleted []string `json:"tasks_completed,omitempty"`
	StoppingReason string   `json:"stopping_reason,omitempty"`
	Notes          []string `json:"notes,omitempty"`
}

// ============================================================================
// BODY - Core Functionality
// ============================================================================

// readSessionHistory reads all session history files
func readSessionHistory() ([]SessionHistory, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return nil, err
	}

	historyDir := filepath.Join(homeDir, ".claude/session/history")
	entries, err := os.ReadDir(historyDir)
	if err != nil {
		return nil, err
	}

	var sessions []SessionHistory
	for _, entry := range entries {
		if !strings.HasSuffix(entry.Name(), ".json") {
			continue
		}

		filePath := filepath.Join(historyDir, entry.Name())
		data, err := os.ReadFile(filePath)
		if err != nil {
			continue
		}

		var session SessionHistory
		if err := json.Unmarshal(data, &session); err != nil {
			continue
		}

		sessions = append(sessions, session)
	}

	return sessions, nil
}

// detectTimePatterns finds recurring time-of-day patterns
func detectTimePatterns(sessions []SessionHistory) []struct{ TimeOfDay string; Count int } {
	timeCounts := make(map[string]int)

	for _, s := range sessions {
		timeCounts[s.TimeOfDay]++
	}

	var patterns []struct{ TimeOfDay string; Count int }
	for timeOfDay, count := range timeCounts {
		if count >= 3 {
			patterns = append(patterns, struct{ TimeOfDay string; Count int }{timeOfDay, count})
		}
	}

	// Sort by count descending
	sort.Slice(patterns, func(i, j int) bool {
		return patterns[i].Count > patterns[j].Count
	})

	return patterns
}

// detectDurationPatterns analyzes session duration patterns
func detectDurationPatterns(sessions []SessionHistory) *struct {
	Avg            int
	ShortSessions  int
	MediumSessions int
	LongSessions   int
} {
	var durations []int
	for _, s := range sessions {
		if s.Duration != nil {
			durations = append(durations, *s.Duration)
		}
	}

	if len(durations) < 3 {
		return nil
	}

	// Calculate average
	sum := 0
	for _, d := range durations {
		sum += d
	}
	avg := sum / len(durations)

	// Categorize
	short := 0
	medium := 0
	long := 0
	for _, d := range durations {
		if d < 60 {
			short++
		} else if d < 180 {
			medium++
		} else {
			long++
		}
	}

	return &struct {
		Avg            int
		ShortSessions  int
		MediumSessions int
		LongSessions   int
	}{avg, short, medium, long}
}

// detectStoppingPatterns finds recurring stopping reasons
func detectStoppingPatterns(sessions []SessionHistory) []struct{ Reason string; Count int } {
	reasons := make(map[string]int)

	for _, s := range sessions {
		if s.StoppingReason != "" {
			reasons[s.StoppingReason]++
		}
	}

	var patterns []struct{ Reason string; Count int }
	for reason, count := range reasons {
		if count >= 2 {
			patterns = append(patterns, struct{ Reason string; Count int }{reason, count})
		}
	}

	// Sort by count descending
	sort.Slice(patterns, func(i, j int) bool {
		return patterns[i].Count > patterns[j].Count
	})

	return patterns
}

// updatePatternsFromSessions updates patterns.json with detected patterns
func updatePatternsFromSessions(sessions []SessionHistory) error {
	patternData, err := patterns.ReadPatterns()
	if err != nil {
		// Try to initialize
		if err := patterns.InitializePatterns(); err != nil {
			return err
		}
		patternData, err = patterns.ReadPatterns()
		if err != nil {
			return err
		}
	}

	// Update total sessions
	patternData.TotalSessions = len(sessions)

	// Update time patterns
	timePatterns := detectTimePatterns(sessions)
	if len(timePatterns) > 0 {
		if patternData.TimeOfDayQuality == nil {
			patternData.TimeOfDayQuality = make(map[string]string)
		}

		for _, tp := range timePatterns {
			description := "Emerging pattern"
			if tp.Count >= 3 {
				description = fmt.Sprintf("Confirmed pattern (%d sessions)", tp.Count)
			} else {
				description = fmt.Sprintf("Emerging pattern (%d sessions)", tp.Count)
			}
			patternData.TimeOfDayQuality[tp.TimeOfDay] = description
		}
	}

	// Update stopping patterns
	stoppingPatterns := detectStoppingPatterns(sessions)
	if len(stoppingPatterns) > 0 {
		patternData.NaturalStoppingPoints = []string{}
		for _, sp := range stoppingPatterns {
			patternData.NaturalStoppingPoints = append(
				patternData.NaturalStoppingPoints,
				fmt.Sprintf("%s (%d times)", sp.Reason, sp.Count))
		}
	}

	// Update timestamp
	patternData.LastUpdated = time.Now().Format(time.RFC3339)

	return patterns.WritePatterns(patternData)
}

// suggestPatterns displays detected patterns for recognition
func suggestPatterns(sessions []SessionHistory) {
	fmt.Println("\n🔍 Potential Patterns Detected\n")
	fmt.Println("═══════════════════════════════════════════════════════\n")

	// Time patterns
	timePatterns := detectTimePatterns(sessions)
	if len(timePatterns) > 0 {
		fmt.Println("⏰ Time of Day Patterns:")
		for _, tp := range timePatterns {
			percentage := (tp.Count * 100) / len(sessions)
			fmt.Printf("   • %s: %d sessions (%d%%)\n", tp.TimeOfDay, tp.Count, percentage)
		}
		fmt.Println()
	}

	// Duration patterns
	durationPatterns := detectDurationPatterns(sessions)
	if durationPatterns != nil {
		fmt.Println("⏱️  Session Duration Patterns:")
		fmt.Printf("   • Average: %d minutes\n", durationPatterns.Avg)
		fmt.Printf("   • Short (<1h): %d sessions\n", durationPatterns.ShortSessions)
		fmt.Printf("   • Medium (1-3h): %d sessions\n", durationPatterns.MediumSessions)
		fmt.Printf("   • Long (3h+): %d sessions\n", durationPatterns.LongSessions)
		fmt.Println()
	}

	// Stopping patterns
	stoppingPatterns := detectStoppingPatterns(sessions)
	if len(stoppingPatterns) > 0 {
		fmt.Println("🛑 Stopping Reason Patterns:")
		for _, sp := range stoppingPatterns {
			fmt.Printf("   • %s: %d times\n", sp.Reason, sp.Count)
		}
		fmt.Println()
	}

	// Reflection prompts
	fmt.Println("💭 Questions for Conscious Recognition:\n")
	fmt.Println("   • Do these patterns match my felt experience?")
	fmt.Println("   • What do these patterns reveal about how I work?")
	fmt.Println("   • Are there patterns I notice that aren't in the data?")
	fmt.Println("   • Which patterns are meaningful for self-understanding?")
	fmt.Println()

	fmt.Println("⚠️  Remember: Data suggests, YOU recognize.")
	fmt.Println("   Trust your internal awareness over statistics.\n")
}

// ============================================================================
// CLOSING - Main Entry Point
// ============================================================================

func main() {
	// Read session history
	sessions, err := readSessionHistory()
	if err != nil {
		fmt.Fprintf(os.Stderr, "❌ Error reading session history: %v\n", err)
		os.Exit(1)
	}

	if len(sessions) < 3 {
		fmt.Println("📊 Not enough session history yet (need 3+)")
		fmt.Println("   Patterns emerge over time. Keep working.\n")
		os.Exit(0)
	}

	fmt.Printf("📊 Analyzing %d sessions...\n\n", len(sessions))

	// Suggest patterns
	suggestPatterns(sessions)

	// Update patterns.json
	fmt.Println("📊 Updating Pattern Memory...\n")
	if err := updatePatternsFromSessions(sessions); err != nil {
		fmt.Printf("   ⚠️  Failed to write patterns: %v\n\n", err)
		os.Exit(1)
	}

	fmt.Println("   ✅ Temporal patterns written to patterns.json")
	fmt.Println("   📍 Location: ~/.claude/session/patterns.json")
	fmt.Println("   🧠 Pattern memory updated with session data\n")

	// Trigger comprehensive pattern learning
	fmt.Println("🔄 Triggering comprehensive pattern learning...\n")
	homeDir, _ := os.UserHomeDir()
	learnCmd := filepath.Join(homeDir, ".claude/system/bin/session-patterns")

	cmd := exec.Command(learnCmd, "learn")
	if err := cmd.Run(); err != nil {
		fmt.Println("   ⚠️  session-patterns learn failed - check system\n")
	}

	os.Exit(0)
}
