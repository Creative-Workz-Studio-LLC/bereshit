// ════════════════════════════════════════════════════════════════════════════
// METADATA - Schedule Update (Internal Calendar Adjustments)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Proverbs 19:21 - "There are many devices in a man's heart;
//   nevertheless the counsel of the LORD, that shall stand."
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Plans change - adjust based on reality, not rigid adherence
//   Faithful work includes honest course-correction
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Update schedule progress and adjust plans based on reality
//
// Usage:
//   schedule-update --session-complete --uptime 90    # Mark session complete
//   schedule-update --adjust-days 3                    # Extend timeline
//   schedule-update --adjust-sessions 2                # Add more sessions
//   schedule-update --note "Complexity higher than expected"
//
// Dependencies: encoding/json, os, time
//
// Health Scoring Map (Base100):
//   +100: Schedule updated successfully
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
	TotalDays               int `json:"total_days"`
	TotalSessions           int `json:"total_sessions"`
	TotalUptimeHours        int `json:"total_uptime_hours"`
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
	PlannedSessionsPerWeek  int  `json:"planned_sessions_per_week"`
	ActualSessionsPerWeek   *int `json:"actual_sessions_per_week"`
	PlannedUptimePerSession int  `json:"planned_uptime_per_session"`
	ActualUptimePerSession  *int `json:"actual_uptime_per_session"`
}

type Adjustment struct {
	Date            string `json:"date"`
	Reason          string `json:"reason"`
	Adjustment      string `json:"adjustment"`
	RevisedEstimate string `json:"revised_estimate"`
}

// ════════════════════════════════════════════════════════════════════════════
// BODY - Schedule Update Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	// Parse flags
	sessionComplete := flag.Bool("session-complete", false, "Mark current session as complete")
	uptime := flag.Int("uptime", 0, "Session uptime in minutes (use with --session-complete)")
	adjustDays := flag.Int("adjust-days", 0, "Adjust total days estimate (+/- days)")
	adjustSessions := flag.Int("adjust-sessions", 0, "Adjust total sessions estimate (+/- sessions)")
	note := flag.String("note", "", "Add note to schedule")
	complete := flag.Bool("complete", false, "Mark entire schedule as complete")
	flag.Parse()

	// Load schedule
	schedule, err := loadSchedule()
	if err != nil {
		if os.IsNotExist(err) {
			fmt.Println("❌ No active schedule to update")
			fmt.Println("💡 Use 'schedule-init' to create a schedule")
			os.Exit(1)
		}
		fmt.Printf("❌ Error loading schedule: %v\n", err)
		os.Exit(1)
	}

	// Apply updates
	updated := false

	if *sessionComplete {
		updateSessionComplete(schedule, *uptime)
		updated = true
	}

	if *adjustDays != 0 {
		updateDaysEstimate(schedule, *adjustDays)
		updated = true
	}

	if *adjustSessions != 0 {
		updateSessionsEstimate(schedule, *adjustSessions)
		updated = true
	}

	if *note != "" {
		schedule.Notes = append(schedule.Notes, *note)
		updated = true
	}

	if *complete {
		completeSchedule(schedule)
		updated = true
	}

	if !updated {
		fmt.Println("❌ No update action specified")
		fmt.Println()
		fmt.Println("Usage:")
		fmt.Println("  schedule-update --session-complete --uptime 90")
		fmt.Println("  schedule-update --adjust-days 3")
		fmt.Println("  schedule-update --note \"Added complexity\"")
		fmt.Println("  schedule-update --complete")
		os.Exit(2) // Exit 2 = usage error
	}

	// Save updated schedule
	if err := saveSchedule(schedule); err != nil {
		fmt.Printf("❌ Error saving schedule: %v\n", err)
		os.Exit(1)
	}

	fmt.Println("✅ Schedule updated")
}

func updateSessionComplete(s *Schedule, uptimeMinutes int) {
	s.Progress.SessionsCompleted++
	s.Progress.CurrentSessionNumber++

	if uptimeMinutes > 0 {
		uptimeHours := float64(uptimeMinutes) / 60.0
		s.Progress.UptimeHoursActual += uptimeHours

		// Update velocity if enough data
		if s.Progress.SessionsCompleted >= 2 {
			avgUptimeMinutes := int(s.Progress.UptimeHoursActual * 60 / float64(s.Progress.SessionsCompleted))
			s.Velocity.ActualUptimePerSession = &avgUptimeMinutes
		}
	}

	// Calculate days elapsed
	startDate, _ := time.Parse("2006-01-02", s.StartDate)
	daysElapsed := int(time.Since(startDate).Hours() / 24)
	s.Progress.DaysElapsed = daysElapsed

	// Update status
	if s.Status == "planned" {
		s.Status = "in_progress"
	}

	fmt.Printf("✅ Session %d marked complete\n", s.Progress.SessionsCompleted)
	if uptimeMinutes > 0 {
		fmt.Printf("   Uptime: %d minutes\n", uptimeMinutes)
	}
}

func updateDaysEstimate(s *Schedule, adjustment int) {
	oldDays := s.Estimates.TotalDays
	s.Estimates.TotalDays += adjustment

	// Update end date
	startDate, _ := time.Parse("2006-01-02", s.StartDate)
	newEndDate := startDate.AddDate(0, 0, s.Estimates.TotalDays)
	s.EstimatedEndDate = newEndDate.Format("2006-01-02")

	// Record adjustment
	adj := Adjustment{
		Date:            time.Now().Format("2006-01-02"),
		Reason:          fmt.Sprintf("Timeline adjusted by %+d days", adjustment),
		Adjustment:      fmt.Sprintf("Days: %d → %d", oldDays, s.Estimates.TotalDays),
		RevisedEstimate: fmt.Sprintf("%d days total", s.Estimates.TotalDays),
	}
	s.Adjustments = append(s.Adjustments, adj)

	fmt.Printf("📆 Timeline adjusted: %d → %d days\n", oldDays, s.Estimates.TotalDays)
}

func updateSessionsEstimate(s *Schedule, adjustment int) {
	oldSessions := s.Estimates.TotalSessions
	s.Estimates.TotalSessions += adjustment

	// Recalculate uptime hours if needed
	s.Estimates.TotalUptimeHours = (s.Estimates.TotalSessions * s.Estimates.AvgSessionUptimeMinutes) / 60

	// Record adjustment
	adj := Adjustment{
		Date:            time.Now().Format("2006-01-02"),
		Reason:          fmt.Sprintf("Sessions adjusted by %+d", adjustment),
		Adjustment:      fmt.Sprintf("Sessions: %d → %d", oldSessions, s.Estimates.TotalSessions),
		RevisedEstimate: fmt.Sprintf("%d sessions total", s.Estimates.TotalSessions),
	}
	s.Adjustments = append(s.Adjustments, adj)

	fmt.Printf("🔢 Sessions adjusted: %d → %d\n", oldSessions, s.Estimates.TotalSessions)
}

func completeSchedule(s *Schedule) {
	s.Status = "completed"
	now := time.Now().Format("2006-01-02")
	s.ActualEndDate = &now

	// Archive schedule
	archiveSchedule(s)

	fmt.Println("✅ Schedule marked complete")
	fmt.Printf("   Completed: %s\n", now)
	fmt.Printf("   Sessions: %d completed\n", s.Progress.SessionsCompleted)
	fmt.Printf("   Uptime: %.1f hours\n", s.Progress.UptimeHoursActual)
}

func archiveSchedule(s *Schedule) {
	archiveDir := filepath.Join(os.Getenv("HOME"), ".claude", "schedule", "history")
	os.MkdirAll(archiveDir, 0755)

	archiveFile := filepath.Join(archiveDir, s.ScheduleID+".json")
	data, _ := json.MarshalIndent(s, "", "  ")
	os.WriteFile(archiveFile, data, 0644)

	fmt.Printf("📦 Archived to: %s\n", archiveFile)
}

func loadSchedule() (*Schedule, error) {
	scheduleFile := filepath.Join(os.Getenv("HOME"), ".claude", "schedule", "current-schedule.json")
	data, err := os.ReadFile(scheduleFile)
	if err != nil {
		return nil, err
	}

	var schedule Schedule
	if err := json.Unmarshal(data, &schedule); err != nil {
		return nil, err
	}

	return &schedule, nil
}

func saveSchedule(s *Schedule) error {
	scheduleFile := filepath.Join(os.Getenv("HOME"), ".claude", "schedule", "current-schedule.json")
	data, err := json.MarshalIndent(s, "", "  ")
	if err != nil {
		return err
	}

	return os.WriteFile(scheduleFile, data, 0644)
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - updates schedule progress and adjusts plans
