// ════════════════════════════════════════════════════════════════════════════
// METADATA - Schedule Check (Internal Calendar Progress)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Psalm 90:12 - "So teach us to number our days,
//   that we may apply our hearts unto wisdom."
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Internal calendar awareness: Where am I in MY plan?
//   Progress tracking, planned vs actual, velocity monitoring
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Check current schedule progress (internal calendar awareness)
//
// Usage:
//   schedule-check              # Show current schedule progress
//   schedule-check --brief      # Show brief status
//   schedule-check --velocity   # Show velocity analysis
//
// Dependencies: encoding/json, os, time
//
// Health Scoring Map (Base100):
//   +100: Successfully displays schedule progress
//   +50: No active schedule (not an error)
//   -50: Cannot read schedule file
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
// BODY - Schedule Check Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	// Parse flags
	brief := flag.Bool("brief", false, "Show brief status only")
	velocityOnly := flag.Bool("velocity", false, "Show velocity analysis only")
	flag.Parse()

	// Load schedule
	schedule, err := loadSchedule()
	if err != nil {
		if os.IsNotExist(err) {
			fmt.Println("📋 No active schedule")
			fmt.Println()
			fmt.Println("💡 Use 'schedule-init' to create a schedule")
			os.Exit(0)
		}
		fmt.Printf("❌ Error loading schedule: %v\n", err)
		os.Exit(1)
	}

	// Display based on flags
	if *brief {
		displayBrief(schedule)
	} else if *velocityOnly {
		displayVelocity(schedule)
	} else {
		displayFull(schedule)
	}
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

func displayBrief(s *Schedule) {
	fmt.Printf("📋 %s\n", s.WorkItem)
	fmt.Printf("   Session %d of %d | Day %d of %d | %.0f%% complete\n",
		s.Progress.CurrentSessionNumber,
		s.Estimates.TotalSessions,
		s.Progress.DaysElapsed+1,
		s.Estimates.TotalDays,
		calculatePercentComplete(s))
}

func displayVelocity(s *Schedule) {
	fmt.Println()
	fmt.Println("⚡ Velocity Analysis")
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()

	fmt.Printf("📊 Sessions:\n")
	fmt.Printf("   Planned:  %d sessions/week\n", s.Velocity.PlannedSessionsPerWeek)
	if s.Velocity.ActualSessionsPerWeek != nil {
		fmt.Printf("   Actual:   %d sessions/week\n", *s.Velocity.ActualSessionsPerWeek)
	} else {
		fmt.Printf("   Actual:   (not enough data yet)\n")
	}
	fmt.Println()

	fmt.Printf("⏱️  Session Duration:\n")
	fmt.Printf("   Planned:  %d minutes/session\n", s.Velocity.PlannedUptimePerSession)
	if s.Velocity.ActualUptimePerSession != nil {
		fmt.Printf("   Actual:   %d minutes/session\n", *s.Velocity.ActualUptimePerSession)

		diff := *s.Velocity.ActualUptimePerSession - s.Velocity.PlannedUptimePerSession
		if diff > 0 {
			fmt.Printf("   Variance: +%d minutes (%.0f%% longer)\n", diff,
				float64(diff)/float64(s.Velocity.PlannedUptimePerSession)*100)
		} else if diff < 0 {
			fmt.Printf("   Variance: %d minutes (%.0f%% shorter)\n", diff,
				float64(-diff)/float64(s.Velocity.PlannedUptimePerSession)*100)
		}
	} else {
		fmt.Printf("   Actual:   (not enough data yet)\n")
	}
	fmt.Println()

	if len(s.Adjustments) > 0 {
		fmt.Printf("📝 Recent Adjustments:\n")
		for _, adj := range s.Adjustments {
			fmt.Printf("   • %s: %s\n", adj.Date, adj.Reason)
		}
		fmt.Println()
	}

	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()
}

func displayFull(s *Schedule) {
	fmt.Println()
	fmt.Println("📅 Internal Calendar - Schedule Progress")
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()

	// Work item and status
	fmt.Printf("📋 Work Item:  %s\n", s.WorkItem)
	fmt.Printf("📍 Status:     %s\n", s.Status)
	fmt.Println()

	// Timeline
	fmt.Println("📆 Timeline:")
	fmt.Printf("   Start:         %s\n", s.StartDate)
	fmt.Printf("   Est. End:      %s\n", s.EstimatedEndDate)

	// Calculate days remaining
	startDate, _ := time.Parse("2006-01-02", s.StartDate)
	daysElapsed := int(time.Since(startDate).Hours() / 24)
	daysRemaining := s.Estimates.TotalDays - daysElapsed

	fmt.Printf("   Days Elapsed:  %d of %d\n", daysElapsed, s.Estimates.TotalDays)
	fmt.Printf("   Days Remaining: %d\n", daysRemaining)
	fmt.Println()

	// Progress
	fmt.Println("📊 Progress:")
	fmt.Printf("   Sessions:      %d of %d completed\n",
		s.Progress.SessionsCompleted, s.Estimates.TotalSessions)
	fmt.Printf("   Current:       Session %d\n", s.Progress.CurrentSessionNumber)
	fmt.Printf("   Uptime:        %.1f of %d hours\n",
		s.Progress.UptimeHoursActual, s.Estimates.TotalUptimeHours)
	fmt.Printf("   Complete:      %.0f%%\n", calculatePercentComplete(s))
	fmt.Println()

	// Velocity
	fmt.Println("⚡ Velocity:")
	fmt.Printf("   Planned:       %d sessions/week, %dm/session\n",
		s.Velocity.PlannedSessionsPerWeek, s.Velocity.PlannedUptimePerSession)

	if s.Velocity.ActualUptimePerSession != nil {
		fmt.Printf("   Actual:        %dm/session", *s.Velocity.ActualUptimePerSession)
		diff := *s.Velocity.ActualUptimePerSession - s.Velocity.PlannedUptimePerSession
		if diff != 0 {
			fmt.Printf(" (%+dm variance)", diff)
		}
		fmt.Println()
	}
	fmt.Println()

	// Milestones
	if len(s.Milestones) > 0 {
		fmt.Println("🎯 Milestones:")
		for _, m := range s.Milestones {
			statusIcon := getStatusIcon(m.Status)
			fmt.Printf("   %s %s (%d sessions) - %s\n",
				statusIcon, m.Name, m.EstimatedSessions, m.Status)
		}
		fmt.Println()
	}

	// Status
	statusMsg := getStatusMessage(s, daysElapsed, daysRemaining)
	fmt.Printf("💡 %s\n", statusMsg)
	fmt.Println()

	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()
}

func calculatePercentComplete(s *Schedule) float64 {
	if s.Estimates.TotalSessions == 0 {
		return 0
	}
	return float64(s.Progress.SessionsCompleted) / float64(s.Estimates.TotalSessions) * 100
}

func getStatusIcon(status string) string {
	switch status {
	case "completed":
		return "✅"
	case "in_progress":
		return "🔄"
	case "pending":
		return "⏳"
	case "blocked":
		return "🚫"
	default:
		return "  "
	}
}

func getStatusMessage(s *Schedule, daysElapsed, daysRemaining int) string {
	percentComplete := calculatePercentComplete(s)
	percentTime := float64(daysElapsed) / float64(s.Estimates.TotalDays) * 100

	if percentComplete > percentTime+10 {
		return "Ahead of schedule - excellent progress"
	} else if percentComplete < percentTime-10 {
		return "Behind schedule - consider adjustments"
	} else {
		return "On track with estimated timeline"
	}
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - displays current schedule progress
