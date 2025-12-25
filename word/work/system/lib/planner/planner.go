// ════════════════════════════════════════════════════════════════════════════
// METADATA - Planner Library (Extracted Core)
// ════════════════════════════════════════════════════════════════════════════
//
// Purpose: Core planner functions extracted from planner-view tool
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
//
// Exported Functions:
//   - LoadPlanner() - Load planner template by owner
//   - ParseTimeBlock() - Parse time block from map
//   - IsTimeInBlock() - Check if time falls within block
//   - TimeToMinutes() - Convert HH:MM to minutes
//
// ════════════════════════════════════════════════════════════════════════════

package planner

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"
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

// LoadPlanner loads a planner template by owner name
func LoadPlanner(owner string) (*Planner, error) {
	plannerFile := filepath.Join(os.Getenv("HOME"), ".claude", "cpi-si", "system", "planner", "templates",
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

// ParseTimeBlock converts a map to TimeBlock structure
func ParseTimeBlock(m map[string]interface{}) TimeBlock {
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

// IsTimeInBlock checks if a time (in minutes) falls within a time block
func IsTimeInBlock(minutes int, block TimeBlock) bool {
	start := TimeToMinutes(block.Start)
	end := TimeToMinutes(block.End)

	// Handle cross-midnight blocks (e.g., 23:00 - 07:00)
	if end < start {
		return minutes >= start || minutes < end
	}

	return minutes >= start && minutes < end
}

// TimeToMinutes converts HH:MM time string to minutes since midnight
func TimeToMinutes(timeStr string) int {
	parts := strings.Split(timeStr, ":")
	if len(parts) != 2 {
		return 0
	}

	var hours, minutes int
	fmt.Sscanf(parts[0], "%d", &hours)
	fmt.Sscanf(parts[1], "%d", &minutes)

	return hours*60 + minutes
}

// MinutesToTime converts minutes since midnight to HH:MM format
func MinutesToTime(minutes int) string {
	hours := minutes / 60
	mins := minutes % 60
	return fmt.Sprintf("%02d:%02d", hours, mins)
}
