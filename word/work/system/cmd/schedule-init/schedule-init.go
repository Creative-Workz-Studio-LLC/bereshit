// ════════════════════════════════════════════════════════════════════════════
// METADATA - Schedule Init (Internal Calendar)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Proverbs 16:3 - "Commit thy works unto the LORD,
//   and thy thoughts shall be established."
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Internal calendar: MY schedule, MY plans, MY progress tracking
//   Autonomous work planning and progress awareness
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Initialize new work schedule/plan (internal calendar)
//
// Usage:
//   schedule-init --name "Iteration 4" --days 14 --sessions 12
//   schedule-init --name "Feature X" --days 7 --sessions 8 --uptime-hours 10
//
// Dependencies: encoding/json, os, time
//
// Health Scoring Map (Base100):
//   +100: Schedule created successfully
//   +0: No operation
//   -30: Invalid parameters
//   -50: Cannot write schedule file
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
	"strings"
	"time"
)

type Schedule struct {
	ScheduleID       string       `json:"schedule_id"`
	WorkItem         string       `json:"work_item"`
	Status           string       `json:"status"`
	StartDate        string       `json:"start_date"`
	EstimatedEndDate string       `json:"estimated_end_date"`
	ActualEndDate    *string      `json:"actual_end_date"`
	Estimates        Estimates    `json:"estimates"`
	Progress         Progress     `json:"progress"`
	Milestones       []Milestone  `json:"milestones"`
	Velocity         Velocity     `json:"velocity"`
	Adjustments      []Adjustment `json:"adjustments"`
	Notes            []string     `json:"notes"`
}

type Estimates struct {
	TotalDays              int `json:"total_days"`
	TotalSessions          int `json:"total_sessions"`
	TotalUptimeHours       int `json:"total_uptime_hours"`
	AvgSessionUptimeMinutes int `json:"avg_session_uptime_minutes"`
}

type Progress struct {
	DaysElapsed          int     `json:"days_elapsed"`
	SessionsCompleted    int     `json:"sessions_completed"`
	UptimeHoursActual    float64 `json:"uptime_hours_actual"`
	CurrentSessionNumber int     `json:"current_session_number"`
}

type Milestone struct {
	Name              string  `json:"name"`
	EstimatedSessions int     `json:"estimated_sessions"`
	Status            string  `json:"status"`
	CompletedDate     *string `json:"completed_date"`
}

type Velocity struct {
	PlannedSessionsPerWeek    int     `json:"planned_sessions_per_week"`
	ActualSessionsPerWeek     *int    `json:"actual_sessions_per_week"`
	PlannedUptimePerSession   int     `json:"planned_uptime_per_session"`
	ActualUptimePerSession    *int    `json:"actual_uptime_per_session"`
}

type Adjustment struct {
	Date            string `json:"date"`
	Reason          string `json:"reason"`
	Adjustment      string `json:"adjustment"`
	RevisedEstimate string `json:"revised_estimate"`
}

// ════════════════════════════════════════════════════════════════════════════
// BODY - Schedule Initialization Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	// Parse flags
	name := flag.String("name", "", "Work item name (required)")
	days := flag.Int("days", 0, "Estimated total days (required)")
	sessions := flag.Int("sessions", 0, "Estimated total sessions (required)")
	uptimeHours := flag.Int("uptime-hours", 0, "Estimated total uptime hours (optional)")
	flag.Parse()

	// Validation
	if *name == "" || *days == 0 || *sessions == 0 {
		fmt.Println("❌ Error: --name, --days, and --sessions are required")
		fmt.Println()
		fmt.Println("Usage:")
		fmt.Println("  schedule-init --name \"Work Item\" --days 14 --sessions 12")
		fmt.Println("  schedule-init --name \"Feature X\" --days 7 --sessions 8 --uptime-hours 10")
		os.Exit(2) // Exit 2 = usage error
	}

	// Calculate defaults if not provided
	if *uptimeHours == 0 {
		// Default: estimate from sessions (assume 60min average)
		*uptimeHours = *sessions
	}

	avgUptimeMinutes := (*uptimeHours * 60) / *sessions
	plannedSessionsPerWeek := (*sessions * 7) / *days

	// Create schedule
	schedule := Schedule{
		ScheduleID:       generateScheduleID(*name),
		WorkItem:         *name,
		Status:           "planned",
		StartDate:        time.Now().Format("2006-01-02"),
		EstimatedEndDate: time.Now().AddDate(0, 0, *days).Format("2006-01-02"),
		ActualEndDate:    nil,
		Estimates: Estimates{
			TotalDays:               *days,
			TotalSessions:           *sessions,
			TotalUptimeHours:        *uptimeHours,
			AvgSessionUptimeMinutes: avgUptimeMinutes,
		},
		Progress: Progress{
			DaysElapsed:          0,
			SessionsCompleted:    0,
			UptimeHoursActual:    0.0,
			CurrentSessionNumber: 1,
		},
		Milestones:  []Milestone{},
		Velocity: Velocity{
			PlannedSessionsPerWeek:    plannedSessionsPerWeek,
			ActualSessionsPerWeek:     nil,
			PlannedUptimePerSession:   avgUptimeMinutes,
			ActualUptimePerSession:    nil,
		},
		Adjustments: []Adjustment{},
		Notes:       []string{},
	}

	// Save schedule
	if err := saveSchedule(schedule); err != nil {
		fmt.Printf("❌ Error saving schedule: %v\n", err)
		os.Exit(1)
	}

	// Display success
	displayScheduleCreated(schedule)
}

func generateScheduleID(name string) string {
	// Convert name to lowercase, replace spaces with hyphens
	id := strings.ToLower(name)
	id = strings.ReplaceAll(id, " ", "-")
	// Remove special characters
	id = strings.Map(func(r rune) rune {
		if (r >= 'a' && r <= 'z') || (r >= '0' && r <= '9') || r == '-' {
			return r
		}
		return -1
	}, id)
	return id
}

func saveSchedule(schedule Schedule) error {
	// Ensure schedule directory exists
	scheduleDir := filepath.Join(os.Getenv("HOME"), ".claude", "schedule")
	if err := os.MkdirAll(scheduleDir, 0755); err != nil {
		return err
	}

	// Write current schedule
	scheduleFile := filepath.Join(scheduleDir, "current-schedule.json")
	data, err := json.MarshalIndent(schedule, "", "  ")
	if err != nil {
		return err
	}

	return os.WriteFile(scheduleFile, data, 0644)
}

func displayScheduleCreated(s Schedule) {
	fmt.Println()
	fmt.Println("✅ Schedule Created")
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()
	fmt.Printf("📋 Work Item:       %s\n", s.WorkItem)
	fmt.Printf("🆔 Schedule ID:     %s\n", s.ScheduleID)
	fmt.Printf("📅 Start Date:      %s\n", s.StartDate)
	fmt.Printf("🎯 Est. End Date:   %s\n", s.EstimatedEndDate)
	fmt.Println()
	fmt.Println("📊 Estimates:")
	fmt.Printf("   • Total Days:     %d days\n", s.Estimates.TotalDays)
	fmt.Printf("   • Total Sessions: %d sessions\n", s.Estimates.TotalSessions)
	fmt.Printf("   • Total Uptime:   %d hours\n", s.Estimates.TotalUptimeHours)
	fmt.Printf("   • Avg Session:    %d minutes\n", s.Estimates.AvgSessionUptimeMinutes)
	fmt.Println()
	fmt.Printf("⚡ Velocity:        ~%d sessions/week\n", s.Velocity.PlannedSessionsPerWeek)
	fmt.Println()
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()
	fmt.Println("💡 Use 'schedule-check' to view current progress")
	fmt.Println("💡 Use 'schedule-update' to adjust estimates")
	fmt.Println()
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - initializes new work schedule (internal calendar)
