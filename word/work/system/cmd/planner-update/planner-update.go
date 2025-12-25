// ════════════════════════════════════════════════════════════════════════════
// METADATA - Planner Update (Modify Planner Data)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Proverbs 16:9 - "A man's heart deviseth his way: but
//   the LORD directeth his steps."
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Planner updates: Add events, adjust patterns, update availability
//   Plans change based on reality - honest course-correction
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Update planner data (add events, modify patterns)
//
// Usage:
//   planner-update --owner seanje --add-event --date 2025-11-15 \
//       --start 14:00 --end 15:30 --type appointment --description "Doctor"
//
// Dependencies: encoding/json, os
//
// Health Scoring Map (Base100):
//   +100: Planner updated successfully
//   -30: Invalid parameters
//   -50: Cannot write planner file
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
)

// ════════════════════════════════════════════════════════════════════════════
// BODY - Planner Update Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	owner := flag.String("owner", "", "Owner (seanje, nova, shared)")
	addEvent := flag.Bool("add-event", false, "Add one-time event")
	date := flag.String("date", "", "Date for event (YYYY-MM-DD)")
	start := flag.String("start", "", "Start time (HH:MM)")
	end := flag.String("end", "", "End time (HH:MM)")
	eventType := flag.String("type", "", "Event type (work, appointment, etc)")
	description := flag.String("description", "", "Event description")
	priority := flag.String("priority", "preferred", "Priority (fixed, preferred, flexible)")
	flag.Parse()

	if *owner == "" {
		fmt.Println("❌ Must specify --owner")
		fmt.Println()
		fmt.Println("Usage:")
		fmt.Println("  planner-update --owner seanje --add-event \\")
		fmt.Println("      --date 2025-11-15 --start 14:00 --end 15:30 \\")
		fmt.Println("      --type appointment --description \"Doctor visit\"")
		os.Exit(2) // Exit 2 = usage error
	}

	if *addEvent {
		if *date == "" || *start == "" || *end == "" || *eventType == "" {
			fmt.Println("❌ --add-event requires: --date, --start, --end, --type")
			os.Exit(2) // Exit 2 = usage error
		}

		if err := addEventToPlanner(*owner, *date, *start, *end, *eventType, *description, *priority); err != nil {
			fmt.Printf("❌ Error adding event: %v\n", err)
			os.Exit(1)
		}

		fmt.Printf("✅ Added event to %s's planner on %s\n", *owner, *date)
		fmt.Printf("   %s - %s: %s %s\n", *start, *end, *eventType, *description)
	} else {
		fmt.Println("❌ No action specified")
		fmt.Println()
		fmt.Println("Available actions:")
		fmt.Println("  --add-event    Add one-time event to planner")
		os.Exit(2) // Exit 2 = usage error
	}
}

func addEventToPlanner(owner, date, start, end, eventType, description, priority string) error {
	plannerFile := filepath.Join(os.Getenv("HOME"), ".claude", "planner", "templates",
		fmt.Sprintf("%s-template.json", owner))

	// Read existing planner
	data, err := os.ReadFile(plannerFile)
	if err != nil {
		return fmt.Errorf("planner not found for %s: %w", owner, err)
	}

	// Parse as generic map to preserve structure
	var planner map[string]interface{}
	if err := json.Unmarshal(data, &planner); err != nil {
		return fmt.Errorf("failed to parse planner: %w", err)
	}

	// Get or create events map
	var events map[string]interface{}
	if eventsData, exists := planner["events"]; exists {
		events = eventsData.(map[string]interface{})
	} else {
		events = make(map[string]interface{})
		planner["events"] = events
	}

	// Create new event
	newEvent := map[string]interface{}{
		"start":       start,
		"end":         end,
		"type":        eventType,
		"description": description,
		"priority":    priority,
	}

	// Add to date's events
	if dateEvents, exists := events[date]; exists {
		// Date already has events - append
		if eventArray, ok := dateEvents.([]interface{}); ok {
			events[date] = append(eventArray, newEvent)
		} else {
			// Not an array - replace with array containing new event
			events[date] = []interface{}{newEvent}
		}
	} else {
		// New date - create array with this event
		events[date] = []interface{}{newEvent}
	}

	// Write updated planner
	updatedData, err := json.MarshalIndent(planner, "", "  ")
	if err != nil {
		return fmt.Errorf("failed to marshal planner: %w", err)
	}

	if err := os.WriteFile(plannerFile, updatedData, 0644); err != nil {
		return fmt.Errorf("failed to write planner: %w", err)
	}

	return nil
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - updates planner data based on specified action
