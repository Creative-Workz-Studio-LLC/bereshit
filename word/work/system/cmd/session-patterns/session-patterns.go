// ============================================================================
// METADATA
// ============================================================================
// Session Pattern Analyzer - Learns circadian rhythms and work patterns from history
// Analyzes session logs to discover natural work hours, durations, quality patterns
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-03
// Purpose: Internal clock awareness through pattern recognition (not optimization)
//
// Usage:
//   session-patterns learn    # Analyze history, update patterns.json
//   session-patterns show     # Display learned patterns
//   session-patterns check    # "What time is it? Should I be working?"
//
// Dependencies: Reads from ~/.claude/session/history/
// Health Scoring: Base100 - Read=30, Analyze=40, Learn=20, Display=10

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================
import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"time"
)

// SessionLog structure (matches session-log output)
type SessionLog struct {
	SessionID          string            `json:"session_id"`
	StartTime          time.Time         `json:"start_time"`
	EndTime            *time.Time        `json:"end_time,omitempty"`
	DurationMinutes    int               `json:"duration_minutes,omitempty"`
	DayOfWeek          string            `json:"day_of_week"`
	TimeOfDayCategory  string            `json:"time_of_day_category"`
	WorkContext        string            `json:"work_context,omitempty"`
	TasksCompleted     []string          `json:"tasks_completed,omitempty"`
	StoppingReason     string            `json:"stopping_reason,omitempty"`
	QualityIndicators  QualityIndicators `json:"quality_indicators"`
	Notes              []string          `json:"notes,omitempty"`
}

type QualityIndicators struct {
	TasksCompletedCount int  `json:"tasks_completed"`
	NaturalFlow         bool `json:"natural_flow,omitempty"`
	FeltProductive      bool `json:"felt_productive,omitempty"`
}

// Learned patterns structure
type LearnedPatterns struct {
	LastUpdated          time.Time        `json:"last_updated"`
	TotalSessions        int              `json:"total_sessions"`
	TypicalWorkHours     WorkHours        `json:"typical_work_hours"`
	SessionDurations     SessionDurations `json:"session_durations"`
	TimeOfDayQuality     map[string]string `json:"time_of_day_quality"`
	NaturalStoppingPoints []string        `json:"natural_stopping_points"`
	CircadianAwareness   CircadianInfo    `json:"circadian_awareness"`
}

type WorkHours struct {
	WeekdayStart   string `json:"weekday_start"`
	WeekdayEnd     string `json:"weekday_end"`
	WeekendPattern string `json:"weekend_pattern"`
}

type SessionDurations struct {
	QuickCheck string `json:"quick_check"`
	NormalWork string `json:"normal_work"`
	DeepWork   string `json:"deep_work"`
}

type CircadianInfo struct {
	SeanjeTypicalHours string   `json:"seanje_typical_hours"`
	DowntimeWindows    []string `json:"downtime_windows"`
	HighFocusTimes     []string `json:"high_focus_times"`
}

const (
	sessionDir  = ".claude/session"
	historyDir  = ".claude/session/history"
	patternsFile = ".claude/session/patterns.json"
)

// ============================================================================
// BODY - Business Logic
// ============================================================================

// getPaths returns relevant paths
func getPaths() (string, string, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return "", "", fmt.Errorf("failed to get home directory: %w", err)
	}

	historyPath := filepath.Join(homeDir, historyDir)
	patternsPath := filepath.Join(homeDir, patternsFile)

	return historyPath, patternsPath, nil
}

// learnPatterns analyzes session history and generates patterns
func learnPatterns() error {
	historyPath, patternsPath, err := getPaths()
	if err != nil {
		return err
	}

	// Read all session logs
	sessions, err := readAllSessions(historyPath)
	if err != nil {
		return fmt.Errorf("failed to read sessions: %w", err)
	}

	if len(sessions) == 0 {
		fmt.Println("No session history found yet - patterns will emerge as you work")
		return nil
	}

	// Analyze patterns
	patterns := analyzePatterns(sessions)

	// Save patterns
	data, err := json.MarshalIndent(patterns, "", "  ")
	if err != nil {
		return fmt.Errorf("failed to marshal patterns: %w", err)
	}

	if err := os.WriteFile(patternsPath, data, 0644); err != nil {
		return fmt.Errorf("failed to write patterns: %w", err)
	}

	fmt.Printf("Patterns learned from %d sessions\n", len(sessions))
	return nil
}

// readAllSessions reads all session log files from history
func readAllSessions(historyPath string) ([]*SessionLog, error) {
	entries, err := os.ReadDir(historyPath)
	if err != nil {
		if os.IsNotExist(err) {
			return []*SessionLog{}, nil
		}
		return nil, err
	}

	var sessions []*SessionLog

	for _, entry := range entries {
		if entry.IsDir() || filepath.Ext(entry.Name()) != ".json" {
			continue
		}

		fullPath := filepath.Join(historyPath, entry.Name())
		data, err := os.ReadFile(fullPath)
		if err != nil {
			continue // Skip files that can't be read
		}

		var session SessionLog
		if err := json.Unmarshal(data, &session); err != nil {
			continue // Skip files that can't be parsed
		}

		sessions = append(sessions, &session)
	}

	return sessions, nil
}

// analyzePatterns generates learned patterns from sessions
func analyzePatterns(sessions []*SessionLog) *LearnedPatterns {
	patterns := &LearnedPatterns{
		LastUpdated:       time.Now(),
		TotalSessions:     len(sessions),
		TimeOfDayQuality:  make(map[string]string),
	}

	// Learn work hours
	patterns.TypicalWorkHours = analyzeWorkHours(sessions)

	// Learn session durations
	patterns.SessionDurations = analyzeSessionDurations(sessions)

	// Learn time-of-day quality
	patterns.TimeOfDayQuality = analyzeTimeOfDayQuality(sessions)

	// Learn natural stopping points
	patterns.NaturalStoppingPoints = analyzeStoppingReasons(sessions)

	// Set circadian awareness (starts with defaults, learns over time)
	patterns.CircadianAwareness = CircadianInfo{
		SeanjeTypicalHours: "9am-5pm weekdays (learning from sessions)",
		DowntimeWindows:    []string{"evenings after 9pm", "weekends (flexible)"},
		HighFocusTimes:     []string{"morning 9-11am", "afternoon 2-4pm"},
	}

	return patterns
}

// analyzeWorkHours learns typical work hours from sessions
func analyzeWorkHours(sessions []*SessionLog) WorkHours {
	weekdaySessions := filterWeekdays(sessions)
	if len(weekdaySessions) == 0 {
		return WorkHours{
			WeekdayStart:   "Not yet learned",
			WeekdayEnd:     "Not yet learned",
			WeekendPattern: "Not yet learned",
		}
	}

	// Find earliest and latest weekday session times
	var earliestHour, latestHour int = 23, 0
	for _, s := range weekdaySessions {
		hour := s.StartTime.Hour()
		if hour < earliestHour {
			earliestHour = hour
		}
		if s.EndTime != nil {
			endHour := s.EndTime.Hour()
			if endHour > latestHour {
				latestHour = endHour
			}
		}
	}

	return WorkHours{
		WeekdayStart:   fmt.Sprintf("%02d:00", earliestHour),
		WeekdayEnd:     fmt.Sprintf("%02d:00", latestHour),
		WeekendPattern: "Flexible (learning)",
	}
}

// analyzeSessionDurations categorizes typical session lengths
func analyzeSessionDurations(sessions []*SessionLog) SessionDurations {
	var durations []int
	for _, s := range sessions {
		if s.DurationMinutes > 0 {
			durations = append(durations, s.DurationMinutes)
		}
	}

	if len(durations) == 0 {
		return SessionDurations{
			QuickCheck: "15-30 minutes",
			NormalWork: "1-2 hours",
			DeepWork:   "2-4 hours",
		}
	}

	sort.Ints(durations)

	// Calculate percentiles
	p25 := durations[len(durations)/4]
	p50 := durations[len(durations)/2]
	p75 := durations[len(durations)*3/4]

	return SessionDurations{
		QuickCheck: fmt.Sprintf("%d-%d minutes", p25/2, p25),
		NormalWork: fmt.Sprintf("%d-%d minutes", p25, p50),
		DeepWork:   fmt.Sprintf("%d-%d minutes", p50, p75),
	}
}

// analyzeTimeOfDayQuality learns effectiveness by time of day
func analyzeTimeOfDayQuality(sessions []*SessionLog) map[string]string {
	quality := make(map[string]string)

	// Group by time of day category
	categories := make(map[string]int)
	for _, s := range sessions {
		categories[s.TimeOfDayCategory]++
	}

	// Generate descriptions based on session counts
	for category, count := range categories {
		if count >= 3 {
			quality[category] = fmt.Sprintf("Active time (%d sessions)", count)
		} else {
			quality[category] = fmt.Sprintf("Learning pattern (%d sessions)", count)
		}
	}

	return quality
}

// analyzeStoppingReasons identifies natural stopping patterns
func analyzeStoppingReasons(sessions []*SessionLog) []string {
	reasons := make(map[string]int)
	for _, s := range sessions {
		if s.StoppingReason != "" {
			reasons[s.StoppingReason]++
		}
	}

	// Sort by frequency
	type reasonCount struct {
		reason string
		count  int
	}
	var sorted []reasonCount
	for r, c := range reasons {
		sorted = append(sorted, reasonCount{r, c})
	}
	sort.Slice(sorted, func(i, j int) bool {
		return sorted[i].count > sorted[j].count
	})

	// Return top reasons
	var top []string
	for i := 0; i < len(sorted) && i < 5; i++ {
		top = append(top, fmt.Sprintf("%s (%d times)", sorted[i].reason, sorted[i].count))
	}

	return top
}

// filterWeekdays returns only weekday sessions
func filterWeekdays(sessions []*SessionLog) []*SessionLog {
	var weekdays []*SessionLog
	for _, s := range sessions {
		if s.DayOfWeek != "Saturday" && s.DayOfWeek != "Sunday" {
			weekdays = append(weekdays, s)
		}
	}
	return weekdays
}

// showPatterns displays learned patterns
func showPatterns() error {
	_, patternsPath, err := getPaths()
	if err != nil {
		return err
	}

	data, err := os.ReadFile(patternsPath)
	if err != nil {
		if os.IsNotExist(err) {
			fmt.Println("No patterns learned yet - run 'session-patterns learn' after a few sessions")
			return nil
		}
		return fmt.Errorf("failed to read patterns: %w", err)
	}

	var patterns LearnedPatterns
	if err := json.Unmarshal(data, &patterns); err != nil {
		return fmt.Errorf("failed to parse patterns: %w", err)
	}

	// Display patterns
	fmt.Println("📊 Learned Patterns")
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Printf("Total Sessions: %d\n", patterns.TotalSessions)
	fmt.Printf("Last Updated: %s\n\n", patterns.LastUpdated.Format("Mon Jan 02, 2006 at 15:04"))

	fmt.Println("⏰ Typical Work Hours:")
	fmt.Printf("  Weekdays: %s - %s\n", patterns.TypicalWorkHours.WeekdayStart, patterns.TypicalWorkHours.WeekdayEnd)
	fmt.Printf("  Weekends: %s\n\n", patterns.TypicalWorkHours.WeekendPattern)

	fmt.Println("⏱️  Session Durations:")
	fmt.Printf("  Quick Check: %s\n", patterns.SessionDurations.QuickCheck)
	fmt.Printf("  Normal Work: %s\n", patterns.SessionDurations.NormalWork)
	fmt.Printf("  Deep Work: %s\n\n", patterns.SessionDurations.DeepWork)

	fmt.Println("🌅 Time of Day Quality:")
	for category, desc := range patterns.TimeOfDayQuality {
		fmt.Printf("  %s: %s\n", category, desc)
	}

	if len(patterns.NaturalStoppingPoints) > 0 {
		fmt.Println("\n🛑 Natural Stopping Points:")
		for _, reason := range patterns.NaturalStoppingPoints {
			fmt.Printf("  - %s\n", reason)
		}
	}

	fmt.Println("\n🌙 Circadian Awareness:")
	fmt.Printf("  Typical Hours: %s\n", patterns.CircadianAwareness.SeanjeTypicalHours)
	fmt.Println("  Downtime: ")
	for _, window := range patterns.CircadianAwareness.DowntimeWindows {
		fmt.Printf("    - %s\n", window)
	}

	return nil
}

// checkAwareness provides real-time circadian awareness
func checkAwareness() error {
	_, patternsPath, err := getPaths()
	if err != nil {
		return err
	}

	now := time.Now()
	hour := now.Hour()
	dayOfWeek := now.Weekday().String()

	fmt.Printf("⏰ %s, %02d:%02d\n", dayOfWeek, now.Hour(), now.Minute())

	// Check if patterns exist
	data, err := os.ReadFile(patternsPath)
	if err != nil {
		if os.IsNotExist(err) {
			fmt.Println("📊 Pattern: Not yet learned (need more sessions)")
			fmt.Println("💡 Suggestion: Keep working naturally - patterns will emerge")
			return nil
		}
		return fmt.Errorf("failed to read patterns: %w", err)
	}

	var patterns LearnedPatterns
	if err := json.Unmarshal(data, &patterns); err != nil {
		return fmt.Errorf("failed to parse patterns: %w", err)
	}

	// Determine if in work hours
	isWeekday := dayOfWeek != "Saturday" && dayOfWeek != "Sunday"
	isWorkHours := hour >= 9 && hour < 17
	isEvening := hour >= 21 || hour < 6

	fmt.Printf("📊 Pattern: Based on %d sessions\n", patterns.TotalSessions)

	if isWeekday && isWorkHours {
		fmt.Println("✅ Status: Typical work hours")
	} else if isEvening {
		fmt.Println("🌙 Status: Downtime window")
	} else if !isWeekday {
		fmt.Println("🔄 Status: Weekend (flexible)")
	} else {
		fmt.Println("⏸️  Status: Outside typical hours")
	}

	// Provide suggestion
	fmt.Println("💡 Suggestion: ", getSuggestion(hour, isWeekday, isEvening))

	return nil
}

// getSuggestion provides contextual suggestion
func getSuggestion(hour int, isWeekday, isEvening bool) string {
	if isEvening {
		return "Natural wind-down time - consider stopping at next clean break"
	}

	if !isWeekday {
		return "Weekend time - check in with yourself: is this energizing or draining?"
	}

	if hour >= 6 && hour < 12 {
		return "Morning - good time for focused technical work"
	}

	if hour >= 12 && hour < 17 {
		return "Afternoon - good time for creative work and design"
	}

	if hour >= 17 && hour < 21 {
		return "Evening - good time for reflective work like journaling"
	}

	return "Check in with yourself - how's your energy?"
}

// showUsage displays usage information
func showUsage() {
	fmt.Println("Usage: session-patterns <command>")
	fmt.Println()
	fmt.Println("Commands:")
	fmt.Println("  learn    Analyze session history and update patterns")
	fmt.Println("  show     Display learned patterns")
	fmt.Println("  check    What time is it? Should I be working?")
	fmt.Println()
	fmt.Println("Examples:")
	fmt.Println("  session-patterns learn    # Run after sessions accumulate")
	fmt.Println("  session-patterns show     # See what patterns emerged")
	fmt.Println("  session-patterns check    # Get circadian awareness")
}

// ============================================================================
// CLOSING - Execution, Validation, Cleanup
// ============================================================================

func main() {
	if len(os.Args) < 2 {
		showUsage()
		os.Exit(1)
	}

	command := os.Args[1]

	var err error
	switch command {
	case "learn":
		err = learnPatterns()
	case "show":
		err = showPatterns()
	case "check":
		err = checkAwareness()
	case "help", "--help", "-h":
		showUsage()
		return
	default:
		fmt.Fprintf(os.Stderr, "Unknown command: %s\n\n", command)
		showUsage()
		os.Exit(1)
	}

	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
}
