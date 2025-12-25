// ════════════════════════════════════════════════════════════════════════════
// METADATA - Calendar Overlay (Combine Multiple Schedules)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Ecclesiastes 4:9-12 - "Two are better than one...
//   A threefold cord is not quickly broken."
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Calendar overlay: See mutual availability, detect conflicts
//   Partnership requires visibility - covenant sees each other's constraints
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Combine multiple planner schedules for coordination
//
// Usage:
//   calendar-overlay                          # Show today's complete schedule
//   calendar-overlay --date 2025-11-15        # Show all schedules for date
//   calendar-overlay --conflicts              # Show today with conflict detection
//   calendar-overlay --date 2025-11-15 --conflicts  # Highlight conflicts
//
// Dependencies: Planner templates (seanje, nova, shared)
//
// Health Scoring Map (Base100):
//   +40: Load all planners successfully
//   +30: Extract and merge time blocks
//   +20: Sort and display combined view
//   +10: Detect conflicts if requested
//   -30: Failed to load planners
//   -50: Invalid date format
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

// Planner structure
type Planner struct {
	PlannerID          string                 `json:"planner_id"`
	Owner              string                 `json:"owner"`
	Month              string                 `json:"month"`
	RecurringPatterns  RecurringPatterns      `json:"recurring_patterns"`
	Events             map[string]interface{} `json:"events"`
	AvailabilityWindows interface{}           `json:"availability_windows,omitempty"`
}

type RecurringPatterns struct {
	Daily  []TimeBlock            `json:"daily"`
	Weekly map[string]interface{} `json:"weekly"`
}

type TimeBlock struct {
	Start       string `json:"start"`
	End         string `json:"end"`
	Type        string `json:"type"`
	Description string `json:"description"`
	Priority    string `json:"priority,omitempty"`
}

// ScheduleEntry for overlay display
type ScheduleEntry struct {
	Owner       string
	Start       string
	End         string
	Type        string
	Description string
	Priority    string
	StartMinutes int // For sorting
}

// ════════════════════════════════════════════════════════════════════════════
// BODY - Calendar Overlay Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	dateStr := flag.String("date", "", "Date to view (YYYY-MM-DD, defaults to today)")
	showConflicts := flag.Bool("conflicts", false, "Highlight time conflicts")
	flag.Parse()

	// Default to today if no date specified
	if *dateStr == "" {
		*dateStr = time.Now().Format("2006-01-02")
	}

	// Parse date
	date, err := time.Parse("2006-01-02", *dateStr)
	if err != nil {
		fmt.Printf("❌ Invalid date format: %v\n", err)
		fmt.Println("   Use YYYY-MM-DD format")
		os.Exit(1)
	}

	// Load all planners
	seanjePlanner := loadPlanner("seanje")
	novaPlanner := loadPlanner("nova")
	sharedPlanner := loadPlanner("shared")

	if seanjePlanner == nil && novaPlanner == nil && sharedPlanner == nil {
		fmt.Println("❌ Failed to load any planners")
		os.Exit(1)
	}

	// Extract schedule entries for this date
	var entries []ScheduleEntry

	if seanjePlanner != nil {
		entries = append(entries, extractScheduleEntries(seanjePlanner, date)...)
	}

	if novaPlanner != nil {
		entries = append(entries, extractScheduleEntries(novaPlanner, date)...)
	}

	if sharedPlanner != nil {
		entries = append(entries, extractScheduleEntries(sharedPlanner, date)...)
	}

	// Sort by start time
	sort.Slice(entries, func(i, j int) bool {
		return entries[i].StartMinutes < entries[j].StartMinutes
	})

	// Display overlay
	displayOverlay(entries, date, *showConflicts)
}

func loadPlanner(owner string) *Planner {
	plannerFile := filepath.Join(os.Getenv("HOME"), ".claude", "planner", "templates",
		fmt.Sprintf("%s-template.json", owner))

	data, err := os.ReadFile(plannerFile)
	if err != nil {
		return nil
	}

	var planner Planner
	if err := json.Unmarshal(data, &planner); err != nil {
		return nil
	}

	return &planner
}

func extractScheduleEntries(planner *Planner, date time.Time) []ScheduleEntry {
	var entries []ScheduleEntry
	weekday := strings.ToLower(date.Weekday().String())
	dateStr := date.Format("2006-01-02")

	// Daily recurring patterns
	for _, block := range planner.RecurringPatterns.Daily {
		entry := ScheduleEntry{
			Owner:        planner.Owner,
			Start:        block.Start,
			End:          block.End,
			Type:         block.Type,
			Description:  block.Description,
			Priority:     block.Priority,
			StartMinutes: timeToMinutes(block.Start),
		}
		entries = append(entries, entry)
	}

	// Weekly recurring patterns for this day of week
	if weeklyData, exists := planner.RecurringPatterns.Weekly[weekday]; exists {
		if weeklyArray, ok := weeklyData.([]interface{}); ok {
			for _, item := range weeklyArray {
				if blockMap, ok := item.(map[string]interface{}); ok {
					block := parseTimeBlock(blockMap)
					entry := ScheduleEntry{
						Owner:        planner.Owner,
						Start:        block.Start,
						End:          block.End,
						Type:         block.Type,
						Description:  block.Description,
						Priority:     block.Priority,
						StartMinutes: timeToMinutes(block.Start),
					}
					entries = append(entries, entry)
				}
			}
		}
	}

	// One-time events for this date
	if eventData, exists := planner.Events[dateStr]; exists {
		if eventArray, ok := eventData.([]interface{}); ok {
			for _, item := range eventArray {
				if eventMap, ok := item.(map[string]interface{}); ok {
					block := parseTimeBlock(eventMap)
					entry := ScheduleEntry{
						Owner:        planner.Owner,
						Start:        block.Start,
						End:          block.End,
						Type:         block.Type,
						Description:  block.Description,
						Priority:     block.Priority,
						StartMinutes: timeToMinutes(block.Start),
					}
					entries = append(entries, entry)
				}
			}
		}
	}

	return entries
}

func parseTimeBlock(m map[string]interface{}) TimeBlock {
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

func timeToMinutes(timeStr string) int {
	parts := strings.Split(timeStr, ":")
	if len(parts) != 2 {
		return 0
	}

	var hours, minutes int
	fmt.Sscanf(parts[0], "%d", &hours)
	fmt.Sscanf(parts[1], "%d", &minutes)

	return hours*60 + minutes
}

func displayOverlay(entries []ScheduleEntry, date time.Time, showConflicts bool) {
	fmt.Printf("📅 Schedule Overlay - %s (%s)\n",
		date.Format("Monday, January 2, 2006"),
		date.Weekday())
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")

	if len(entries) == 0 {
		fmt.Println("   (No scheduled items)")
		return
	}

	// Track conflicts if requested
	var conflicts []string

	currentHour := -1
	for i, entry := range entries {
		// Print hour separators
		entryHour := entry.StartMinutes / 60
		if entryHour != currentHour {
			if currentHour != -1 {
				fmt.Println()
			}
			fmt.Printf("━━ %02d:00 ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", entryHour)
			currentHour = entryHour
		}

		// Check for conflicts with previous entry
		conflict := ""
		if showConflicts && i > 0 {
			prevEntry := entries[i-1]
			prevEnd := timeToMinutes(prevEntry.End)

			// Overlap if this entry starts before previous ends
			if entry.StartMinutes < prevEnd {
				conflict = " ⚠️  CONFLICT"
				conflictMsg := fmt.Sprintf("%s - %s: %s [%s] overlaps with %s - %s: %s [%s]",
					entry.Start, entry.End, entry.Description, entry.Owner,
					prevEntry.Start, prevEntry.End, prevEntry.Description, prevEntry.Owner)

				// Add to conflicts list if not already there
				found := false
				for _, c := range conflicts {
					if c == conflictMsg {
						found = true
						break
					}
				}
				if !found {
					conflicts = append(conflicts, conflictMsg)
				}
			}
		}

		// Display entry
		ownerLabel := strings.ToUpper(entry.Owner[:1])
		typeIcon := getTypeIcon(entry.Type)

		fmt.Printf("   [%s] %s - %s  %s %s (%s)%s\n",
			ownerLabel,
			entry.Start,
			entry.End,
			typeIcon,
			entry.Description,
			entry.Type,
			conflict)
	}

	// Display conflicts summary if requested
	if showConflicts && len(conflicts) > 0 {
		fmt.Println()
		fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
		fmt.Printf("⚠️  %d Conflict(s) Detected:\n", len(conflicts))
		for i, conflict := range conflicts {
			fmt.Printf("   %d. %s\n", i+1, conflict)
		}
	}

	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()
	fmt.Println("Legend:")
	fmt.Println("  [S] = Seanje   [N] = Nova   [Sh] = Shared")
}

func getTypeIcon(typeStr string) string {
	switch typeStr {
	case "sleep":
		return "💤"
	case "work":
		return "💻"
	case "meal":
		return "🍽️"
	case "personal":
		return "🙏"
	case "appointment":
		return "📅"
	case "commitment":
		return "🤝"
	case "unavailable":
		return "🚫"
	case "available":
		return "✅"
	case "flex":
		return "🔄"
	default:
		return "📌"
	}
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - combines multiple schedules for coordination view
