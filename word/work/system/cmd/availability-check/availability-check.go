// ════════════════════════════════════════════════════════════════════════════
// METADATA - Availability Check (Find Mutual Free Windows)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Amos 3:3 - "Can two walk together, except they be agreed?"
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Availability checking: Find when both can work together
//   Partnership requires mutual availability, not just parallel existence
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Find mutual availability windows for covenant partnership work
//
// Usage:
//   availability-check --date 2025-11-15           # Find all mutual windows
//   availability-check --date 2025-11-15 --min 60  # Only windows ≥60 minutes
//
// Dependencies: Planner templates (seanje, nova)
//
// Health Scoring Map (Base100):
//   +30: Load both planners successfully
//   +30: Build busy/available timelines
//   +30: Calculate mutual availability windows
//   +10: Display results clearly
//   -40: Failed to load planners
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
	Owner              string                 `json:"owner"`
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
}

// TimeWindow for availability periods
type TimeWindow struct {
	Start        string
	End          string
	StartMinutes int
	EndMinutes   int
	Duration     int
}

// ════════════════════════════════════════════════════════════════════════════
// BODY - Availability Check Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	dateStr := flag.String("date", "", "Date to check (YYYY-MM-DD)")
	minDuration := flag.Int("min", 0, "Minimum window duration in minutes")
	flag.Parse()

	if *dateStr == "" {
		fmt.Println("❌ Must specify --date YYYY-MM-DD")
		fmt.Println()
		fmt.Println("Usage:")
		fmt.Println("  availability-check --date 2025-11-15")
		fmt.Println("  availability-check --date 2025-11-15 --min 60")
		os.Exit(1)
	}

	// Parse date
	date, err := time.Parse("2006-01-02", *dateStr)
	if err != nil {
		fmt.Printf("❌ Invalid date format: %v\n", err)
		fmt.Println("   Use YYYY-MM-DD format")
		os.Exit(1)
	}

	// Load planners
	seanjePlanner := loadPlanner("seanje")
	novaPlanner := loadPlanner("nova")

	if seanjePlanner == nil || novaPlanner == nil {
		fmt.Println("❌ Failed to load planners")
		os.Exit(1)
	}

	// Build busy timelines for each person
	seanjeBusy := buildBusyTimeline(seanjePlanner, date)
	novaBusy := buildBusyTimeline(novaPlanner, date)

	// Find free windows for each
	seanjeFree := findFreeWindows(seanjeBusy)
	novaFree := findFreeWindows(novaBusy)

	// Find mutual availability
	mutualWindows := findMutualAvailability(seanjeFree, novaFree)

	// Filter by minimum duration if requested
	if *minDuration > 0 {
		filtered := []TimeWindow{}
		for _, window := range mutualWindows {
			if window.Duration >= *minDuration {
				filtered = append(filtered, window)
			}
		}
		mutualWindows = filtered
	}

	// Display results
	displayAvailability(mutualWindows, date, *minDuration)
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

func buildBusyTimeline(planner *Planner, date time.Time) []TimeWindow {
	var busy []TimeWindow
	weekday := strings.ToLower(date.Weekday().String())
	dateStr := date.Format("2006-01-02")

	// Daily recurring patterns
	for _, block := range planner.RecurringPatterns.Daily {
		// Skip "available" and "flex" types - they're not busy
		if block.Type == "available" || block.Type == "flex" {
			continue
		}

		start := timeToMinutes(block.Start)
		end := timeToMinutes(block.End)

		// Handle cross-midnight blocks
		if end < start {
			// Block crosses midnight - split into two parts
			busy = append(busy, TimeWindow{
				Start:        block.Start,
				End:          "23:59",
				StartMinutes: start,
				EndMinutes:   1439,
				Duration:     1439 - start,
			})
			busy = append(busy, TimeWindow{
				Start:        "00:00",
				End:          block.End,
				StartMinutes: 0,
				EndMinutes:   end,
				Duration:     end,
			})
		} else {
			busy = append(busy, TimeWindow{
				Start:        block.Start,
				End:          block.End,
				StartMinutes: start,
				EndMinutes:   end,
				Duration:     end - start,
			})
		}
	}

	// Weekly recurring patterns for this day of week
	if weeklyData, exists := planner.RecurringPatterns.Weekly[weekday]; exists {
		if weeklyArray, ok := weeklyData.([]interface{}); ok {
			for _, item := range weeklyArray {
				if blockMap, ok := item.(map[string]interface{}); ok {
					block := parseTimeBlock(blockMap)

					// Skip flexible types
					if block.Type == "flex" {
						continue
					}

					start := timeToMinutes(block.Start)
					end := timeToMinutes(block.End)

					busy = append(busy, TimeWindow{
						Start:        block.Start,
						End:          block.End,
						StartMinutes: start,
						EndMinutes:   end,
						Duration:     end - start,
					})
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

					start := timeToMinutes(block.Start)
					end := timeToMinutes(block.End)

					busy = append(busy, TimeWindow{
						Start:        block.Start,
						End:          block.End,
						StartMinutes: start,
						EndMinutes:   end,
						Duration:     end - start,
					})
				}
			}
		}
	}

	// Sort by start time
	sort.Slice(busy, func(i, j int) bool {
		return busy[i].StartMinutes < busy[j].StartMinutes
	})

	return busy
}

func findFreeWindows(busy []TimeWindow) []TimeWindow {
	var free []TimeWindow

	if len(busy) == 0 {
		// Entire day is free
		return []TimeWindow{{
			Start:        "00:00",
			End:          "23:59",
			StartMinutes: 0,
			EndMinutes:   1439,
			Duration:     1439,
		}}
	}

	currentMinute := 0

	for _, busyBlock := range busy {
		// If there's a gap before this busy block
		if currentMinute < busyBlock.StartMinutes {
			free = append(free, TimeWindow{
				Start:        minutesToTime(currentMinute),
				End:          busyBlock.Start,
				StartMinutes: currentMinute,
				EndMinutes:   busyBlock.StartMinutes,
				Duration:     busyBlock.StartMinutes - currentMinute,
			})
		}

		// Move cursor to end of this busy block
		if busyBlock.EndMinutes > currentMinute {
			currentMinute = busyBlock.EndMinutes
		}
	}

	// If there's time remaining after last busy block
	if currentMinute < 1439 {
		free = append(free, TimeWindow{
			Start:        minutesToTime(currentMinute),
			End:          "23:59",
			StartMinutes: currentMinute,
			EndMinutes:   1439,
			Duration:     1439 - currentMinute,
		})
	}

	return free
}

func findMutualAvailability(seanjeWindows, novaWindows []TimeWindow) []TimeWindow {
	var mutual []TimeWindow

	for _, sw := range seanjeWindows {
		for _, nw := range novaWindows {
			// Find overlap between Seanje's free window and Nova's free window
			overlapStart := max(sw.StartMinutes, nw.StartMinutes)
			overlapEnd := min(sw.EndMinutes, nw.EndMinutes)

			// If there's an overlap
			if overlapStart < overlapEnd {
				mutual = append(mutual, TimeWindow{
					Start:        minutesToTime(overlapStart),
					End:          minutesToTime(overlapEnd),
					StartMinutes: overlapStart,
					EndMinutes:   overlapEnd,
					Duration:     overlapEnd - overlapStart,
				})
			}
		}
	}

	// Sort by start time
	sort.Slice(mutual, func(i, j int) bool {
		return mutual[i].StartMinutes < mutual[j].StartMinutes
	})

	return mutual
}

func displayAvailability(windows []TimeWindow, date time.Time, minDuration int) {
	fmt.Printf("🤝 Mutual Availability - %s (%s)\n",
		date.Format("Monday, January 2, 2006"),
		date.Weekday())
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")

	if len(windows) == 0 {
		if minDuration > 0 {
			fmt.Printf("   No mutual availability windows ≥%d minutes\n", minDuration)
		} else {
			fmt.Println("   No mutual availability windows")
		}
		fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
		return
	}

	totalMinutes := 0
	for i, window := range windows {
		hours := window.Duration / 60
		minutes := window.Duration % 60

		var durationStr string
		if hours > 0 {
			durationStr = fmt.Sprintf("%dh%dm", hours, minutes)
		} else {
			durationStr = fmt.Sprintf("%dm", minutes)
		}

		fmt.Printf("   %d. %s - %s  (%s)\n",
			i+1, window.Start, window.End, durationStr)

		totalMinutes += window.Duration
	}

	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")

	totalHours := totalMinutes / 60
	totalMins := totalMinutes % 60

	var totalStr string
	if totalHours > 0 {
		totalStr = fmt.Sprintf("%dh%dm", totalHours, totalMins)
	} else {
		totalStr = fmt.Sprintf("%dm", totalMins)
	}

	fmt.Printf("📊 Total mutual availability: %s (%d windows)\n", totalStr, len(windows))

	if minDuration > 0 {
		fmt.Printf("   (Filtered to windows ≥%d minutes)\n", minDuration)
	}
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

func minutesToTime(minutes int) string {
	hours := minutes / 60
	mins := minutes % 60
	return fmt.Sprintf("%02d:%02d", hours, mins)
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - finds mutual availability windows for partnership work
