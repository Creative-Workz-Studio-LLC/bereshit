// ════════════════════════════════════════════════════════════════════════════
// METADATA - Planner View (24-Hour Schedule Display)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Ecclesiastes 3:1 - "To every thing there is a season,
//   and a time to every purpose under the heaven"
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Planner view: Display 24-hour schedule showing all time blocks
//   Combines recurring patterns + one-time events
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Visualize 24-hour schedule for any given day
//
// Usage:
//   planner-view                                  # Defaults to today, seanje
//   planner-view --date 2025-11-04                # Specific date, default owner
//   planner-view --owner nova                     # Today, specific owner
//   planner-view --date 2025-11-04 --owner seanje # Explicit date and owner
//
// Dependencies: encoding/json, time
//
// Health Scoring Map (Base100):
//   +100: Schedule displayed successfully
//   -30: Invalid parameters
//   -50: Planner file not found
//
// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
// SETUP - Imports and Configuration
// ════════════════════════════════════════════════════════════════════════════

package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"time"
)

type Planner struct {
	PlannerID          string                 `json:"planner_id"`
	Owner              string                 `json:"owner"`
	Month              string                 `json:"month"`
	Timezone           string                 `json:"timezone"`
	RecurringPatterns  RecurringPatterns      `json:"recurring_patterns"`
	Events             map[string]interface{} `json:"events"` // Can contain arrays or comment strings
	AvailabilityWindows interface{}           `json:"availability_windows,omitempty"`
	CapacityNotes       interface{}           `json:"capacity_notes,omitempty"`
	CoordinationPrinciples interface{}        `json:"coordination_principles,omitempty"`
}

type RecurringPatterns struct {
	Daily  []TimeBlock               `json:"daily"`
	Weekly map[string]interface{}    `json:"weekly"` // Can contain arrays or comment strings
}

type TimeBlock struct {
	Start       string                 `json:"start"`
	End         string                 `json:"end"`
	Type        string                 `json:"type"`
	Description string                 `json:"description"`
	Priority    string                 `json:"priority"`
	Metadata    map[string]interface{} `json:"metadata"`
}

type ScheduleBlock struct {
	StartMinutes int
	EndMinutes   int
	Type         string
	Description  string
	Priority     string
}

// ════════════════════════════════════════════════════════════════════════════
// BODY - Planner View Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	dateStr := flag.String("date", "", "Date to view (YYYY-MM-DD, defaults to today)")
	owner := flag.String("owner", "", "Owner (seanje, nova, shared, defaults to seanje)")
	flag.Parse()

	// Default to today if no date specified
	if *dateStr == "" {
		*dateStr = time.Now().Format("2006-01-02")
	}

	// Default to seanje if no owner specified
	if *owner == "" {
		*owner = "seanje"
	}

	// Parse date
	date, err := time.Parse("2006-01-02", *dateStr)
	if err != nil {
		fmt.Printf("❌ Invalid date format: %s (use YYYY-MM-DD)\n", *dateStr)
		os.Exit(1)
	}

	// Load planner
	planner, err := loadPlanner(*owner, date)
	if err != nil {
		fmt.Printf("❌ Error loading planner: %v\n", err)
		os.Exit(1)
	}

	// Build schedule for this day
	schedule := buildDaySchedule(planner, date)

	// Display schedule
	displaySchedule(schedule, date, *owner)
}

func loadPlanner(owner string, date time.Time) (*Planner, error) {
	plannerFile := filepath.Join(os.Getenv("HOME"), ".claude", "planner", "templates",
		fmt.Sprintf("%s-template.json", owner))

	data, err := os.ReadFile(plannerFile)
	if err != nil {
		return nil, fmt.Errorf("planner not found for %s: %w", owner, err)
	}

	var planner Planner
	if err := json.Unmarshal(data, &planner); err != nil {
		return nil, fmt.Errorf("failed to parse planner: %w", err)
	}

	return &planner, nil
}

func buildDaySchedule(planner *Planner, date time.Time) []ScheduleBlock {
	var blocks []ScheduleBlock

	// Add daily recurring patterns
	for _, tb := range planner.RecurringPatterns.Daily {
		blocks = append(blocks, convertTimeBlock(tb))
	}

	// Add weekly patterns for this day of week
	weekday := strings.ToLower(date.Weekday().String())
	if weeklyData, exists := planner.RecurringPatterns.Weekly[weekday]; exists {
		// Weekly data might be an array of TimeBlocks or a comment string
		if weeklyArray, ok := weeklyData.([]interface{}); ok {
			for _, item := range weeklyArray {
				if blockMap, ok := item.(map[string]interface{}); ok {
					tb := parseTimeBlockFromMap(blockMap)
					blocks = append(blocks, convertTimeBlock(tb))
				}
			}
		}
	}

	// Add one-time events for this date
	dateStr := date.Format("2006-01-02")
	if eventData, exists := planner.Events[dateStr]; exists {
		// Events might be an array of TimeBlocks or a comment string
		if eventArray, ok := eventData.([]interface{}); ok {
			for _, item := range eventArray {
				if eventMap, ok := item.(map[string]interface{}); ok {
					tb := parseTimeBlockFromMap(eventMap)
					blocks = append(blocks, convertTimeBlock(tb))
				}
			}
		}
	}

	// Sort by start time
	sort.Slice(blocks, func(i, j int) bool {
		return blocks[i].StartMinutes < blocks[j].StartMinutes
	})

	return blocks
}

func parseTimeBlockFromMap(m map[string]interface{}) TimeBlock {
	tb := TimeBlock{}

	if start, ok := m["start"].(string); ok {
		tb.Start = start
	}
	if end, ok := m["end"].(string); ok {
		tb.End = end
	}
	if typeStr, ok := m["type"].(string); ok {
		tb.Type = typeStr
	}
	if desc, ok := m["description"].(string); ok {
		tb.Description = desc
	}
	if priority, ok := m["priority"].(string); ok {
		tb.Priority = priority
	}

	return tb
}

func convertTimeBlock(tb TimeBlock) ScheduleBlock {
	return ScheduleBlock{
		StartMinutes: timeToMinutes(tb.Start),
		EndMinutes:   timeToMinutes(tb.End),
		Type:         tb.Type,
		Description:  tb.Description,
		Priority:     tb.Priority,
	}
}

func timeToMinutes(timeStr string) int {
	parts := strings.Split(timeStr, ":")
	if len(parts) != 2 {
		return 0
	}

	var hour, min int
	fmt.Sscanf(parts[0], "%d", &hour)
	fmt.Sscanf(parts[1], "%d", &min)

	return hour*60 + min
}

func minutesToTime(minutes int) string {
	// Handle cross-midnight times
	if minutes >= 1440 {
		minutes = minutes - 1440
	}

	hour := minutes / 60
	min := minutes % 60
	return fmt.Sprintf("%02d:%02d", hour, min)
}

func displaySchedule(schedule []ScheduleBlock, date time.Time, owner string) {
	fmt.Printf("\n📅 %s - %s's Schedule\n", date.Format("Monday, January 02, 2006"), strings.Title(owner))
	fmt.Printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n")

	if len(schedule) == 0 {
		fmt.Println("   No scheduled blocks for this day")
		fmt.Println()
		return
	}

	// Display each time block
	for _, block := range schedule {
		icon := getTypeIcon(block.Type)
		priorityIndicator := getPriorityIndicator(block.Priority)

		startTime := minutesToTime(block.StartMinutes)
		endTime := minutesToTime(block.EndMinutes)

		// Calculate duration
		duration := block.EndMinutes - block.StartMinutes
		if duration < 0 {
			// Cross-midnight block
			duration = (1440 - block.StartMinutes) + block.EndMinutes
		}

		durationStr := formatDuration(duration)

		fmt.Printf("%s %s - %s  %s%s %s (%s)\n",
			icon,
			startTime,
			endTime,
			priorityIndicator,
			strings.Title(strings.ReplaceAll(block.Type, "_", " ")),
			block.Description,
			durationStr,
		)
	}

	// Show gaps (unscheduled time)
	fmt.Println()
	showGaps(schedule)

	fmt.Println()
}

func getTypeIcon(typeStr string) string {
	icons := map[string]string{
		"sleep":       "😴",
		"work":        "💼",
		"meal":        "🍽️",
		"break":       "☕",
		"personal":    "🧘",
		"appointment": "📅",
		"commitment":  "🤝",
		"flex":        "🔄",
		"available":   "✅",
		"unavailable": "🚫",
	}

	if icon, exists := icons[typeStr]; exists {
		return icon
	}
	return "•"
}

func getPriorityIndicator(priority string) string {
	switch priority {
	case "fixed":
		return "🔒 "
	case "preferred":
		return "⭐ "
	case "flexible":
		return "🔄 "
	default:
		return ""
	}
}

func formatDuration(minutes int) string {
	hours := minutes / 60
	mins := minutes % 60

	if hours == 0 {
		return fmt.Sprintf("%dm", mins)
	} else if mins == 0 {
		return fmt.Sprintf("%dh", hours)
	}
	return fmt.Sprintf("%dh%dm", hours, mins)
}

func showGaps(schedule []ScheduleBlock) {
	if len(schedule) == 0 {
		return
	}

	fmt.Println("⏱️  Unscheduled Time (potential availability):")

	currentTime := 0 // Start at midnight (00:00)

	for _, block := range schedule {
		if block.StartMinutes > currentTime {
			// Gap found
			gapDuration := block.StartMinutes - currentTime
			if gapDuration >= 15 { // Only show gaps 15+ minutes
				fmt.Printf("   %s - %s  (%s)\n",
					minutesToTime(currentTime),
					minutesToTime(block.StartMinutes),
					formatDuration(gapDuration))
			}
		}

		// Move current time to end of this block
		if block.EndMinutes > currentTime {
			currentTime = block.EndMinutes
		}

		// Handle cross-midnight blocks
		if block.EndMinutes < block.StartMinutes {
			currentTime = 1440 // End of day
		}
	}

	// Check for gap at end of day
	if currentTime < 1440 {
		gapDuration := 1440 - currentTime
		if gapDuration >= 15 {
			fmt.Printf("   %s - %s  (%s)\n",
				minutesToTime(currentTime),
				"23:59",
				formatDuration(gapDuration))
		}
	}
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - displays 24-hour schedule for specified date/owner
