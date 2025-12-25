// ════════════════════════════════════════════════════════════════════════════
// METADATA - Temporal Awareness Library (Orchestrator)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Ecclesiastes 3:1 - "To every thing there is a season,
//   and a time to every purpose under the heaven."
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Temporal awareness orchestrator: Composes existing proven systems
//   Extract and orchestrate, not reimplement - uses tested core functions
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Orchestrate time and schedule awareness across CPI-SI system
//
// Four Dimensions of Temporal Awareness:
//   1. External Time - System clock (what time is it in the world?)
//   2. Internal Time - Session clock (how long have I been working?)
//   3. Internal Schedule - Planner (what should I be working on?)
//   4. External Calendar - Base calendar (what kind of day is this?)
//
// Dependencies: system/lib/sessiontime, system/lib/planner, system/lib/calendar
//
// Health Scoring Map (Base100):
//   +25: Get external time successfully
//   +25: Get internal time (session duration) successfully
//   +25: Get internal schedule (planner context) successfully
//   +25: Get external calendar (base calendar) successfully
//   -20: Failed to read session state
//   -20: Failed to load planner
//   -20: Failed to load calendar
//
// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
// SETUP - Imports and Configuration
// ════════════════════════════════════════════════════════════════════════════

package temporal

import (
	"strings"
	"time"

	"system/lib/calendar"
	"system/lib/planner"
	"system/lib/sessiontime"
)

// TemporalContext holds all time and schedule awareness
type TemporalContext struct {
	ExternalTime     ExternalTime     `json:"external_time"`
	InternalTime     InternalTime     `json:"internal_time"`
	InternalSchedule InternalSchedule `json:"internal_schedule"`
	ExternalCalendar ExternalCalendar `json:"external_calendar"`
}

// ExternalTime - System clock awareness
type ExternalTime struct {
	CurrentTime    time.Time `json:"current_time"`
	Formatted      string    `json:"formatted"`        // "Mon Jan 02, 2006 at 15:04:05"
	Hour           int       `json:"hour"`             // 0-23
	Minute         int       `json:"minute"`           // 0-59
	TimeOfDay      string    `json:"time_of_day"`      // "morning", "afternoon", "evening", "night"
	CircadianPhase string    `json:"circadian_phase"`  // "peak", "normal", "low"
}

// InternalTime - Session clock awareness
type InternalTime struct {
	SessionStart     time.Time     `json:"session_start"`
	ElapsedDuration  time.Duration `json:"elapsed_duration_seconds"`
	ElapsedFormatted string        `json:"elapsed_formatted"` // "2h15m"
	SessionPhase     string        `json:"session_phase"`     // "fresh", "active", "long"
}

// InternalSchedule - Planner awareness
type InternalSchedule struct {
	CurrentActivity  string `json:"current_activity"`   // What should be happening now
	ActivityType     string `json:"activity_type"`      // "work", "sleep", "meal", etc.
	NextActivity     string `json:"next_activity"`      // What's coming next
	NextActivityTime string `json:"next_activity_time"` // When it starts
	InWorkWindow     bool   `json:"in_work_window"`     // Is this a work window?
	ExpectedDowntime bool   `json:"expected_downtime"`  // Sleep, meal, break?
}

// ExternalCalendar - Base calendar awareness
type ExternalCalendar struct {
	Date        string `json:"date"`         // "2025-11-04"
	Year        int    `json:"year"`         // 2025
	DayOfWeek   string `json:"day_of_week"`  // "Tuesday"
	WeekNumber  int    `json:"week_number"`  // 45
	IsHoliday   bool   `json:"is_holiday"`
	HolidayName string `json:"holiday_name"` // If applicable
	MonthName   string `json:"month_name"`   // "November"
	DayOfMonth  int    `json:"day_of_month"` // 4
}

// ════════════════════════════════════════════════════════════════════════════
// BODY - Temporal Awareness Orchestration
// ════════════════════════════════════════════════════════════════════════════

// GetTemporalContext retrieves complete time and schedule awareness
// Orchestrates existing proven systems, doesn't reimplement
func GetTemporalContext() (*TemporalContext, error) {
	ctx := &TemporalContext{}

	// Get external time (always succeeds)
	ctx.ExternalTime = GetExternalTime()

	// Get internal time (session duration) - orchestrate sessiontime library
	internalTime, err := GetInternalTime()
	if err == nil {
		ctx.InternalTime = *internalTime
	}

	// Get internal schedule (planner context) - orchestrate planner library
	schedule, err := GetInternalSchedule(ctx.ExternalTime.CurrentTime)
	if err == nil {
		ctx.InternalSchedule = *schedule
	}

	// Get external calendar (base calendar) - orchestrate calendar library
	cal, err := GetExternalCalendar(ctx.ExternalTime.CurrentTime)
	if err == nil {
		ctx.ExternalCalendar = *cal
	}

	return ctx, nil
}

// GetExternalTime returns system clock awareness (pure time.Now(), no dependencies)
func GetExternalTime() ExternalTime {
	now := time.Now()

	ext := ExternalTime{
		CurrentTime: now,
		Formatted:   now.Format("Mon Jan 02, 2006 at 15:04:05"),
		Hour:        now.Hour(),
		Minute:      now.Minute(),
	}

	// Determine time of day and circadian phase
	hour := now.Hour()
	switch {
	case hour >= 5 && hour < 12:
		ext.TimeOfDay = "morning"
		ext.CircadianPhase = "peak"
	case hour >= 12 && hour < 17:
		ext.TimeOfDay = "afternoon"
		ext.CircadianPhase = "normal"
	case hour >= 17 && hour < 21:
		ext.TimeOfDay = "evening"
		ext.CircadianPhase = "normal"
	default:
		ext.TimeOfDay = "night"
		ext.CircadianPhase = "low"
	}

	return ext
}

// GetInternalTime orchestrates sessiontime library for session duration awareness
func GetInternalTime() (*InternalTime, error) {
	// Use extracted sessiontime library
	state, err := sessiontime.ReadSession()
	if err != nil {
		return nil, err
	}

	elapsed := sessiontime.CalculateElapsed(state)

	internal := &InternalTime{
		SessionStart:     state.StartTime,
		ElapsedDuration:  elapsed,
		ElapsedFormatted: sessiontime.FormatDuration(elapsed),
	}

	// Determine session phase
	minutes := int(elapsed.Minutes())
	switch {
	case minutes < 30:
		internal.SessionPhase = "fresh"
	case minutes < 120:
		internal.SessionPhase = "active"
	default:
		internal.SessionPhase = "long"
	}

	return internal, nil
}

// GetInternalSchedule orchestrates planner library for schedule awareness
func GetInternalSchedule(currentTime time.Time) (*InternalSchedule, error) {
	// Load session state to get current user (config-driven, not hardcoded)
	state, err := sessiontime.ReadSession()
	if err != nil {
		return nil, err
	}

	// Use extracted planner library with username from session state
	plan, err := planner.LoadPlanner(state.UserID)
	if err != nil {
		return nil, err
	}

	schedule := &InternalSchedule{}
	currentMinutes := currentTime.Hour()*60 + currentTime.Minute()
	weekday := strings.ToLower(currentTime.Weekday().String())

	// Check daily patterns
	for _, block := range plan.RecurringPatterns.Daily {
		if planner.IsTimeInBlock(currentMinutes, block) {
			schedule.CurrentActivity = block.Description
			schedule.ActivityType = block.Type
			schedule.ExpectedDowntime = (block.Type == "sleep" || block.Type == "meal" || block.Type == "break")
			schedule.InWorkWindow = (block.Type == "work")
			break
		}
	}

	// Check weekly patterns if no daily match
	if schedule.CurrentActivity == "" {
		if weeklyData, exists := plan.RecurringPatterns.Weekly[weekday]; exists {
			if weeklyArray, ok := weeklyData.([]interface{}); ok {
				for _, item := range weeklyArray {
					if blockMap, ok := item.(map[string]interface{}); ok {
						block := planner.ParseTimeBlock(blockMap)
						if planner.IsTimeInBlock(currentMinutes, block) {
							schedule.CurrentActivity = block.Description
							schedule.ActivityType = block.Type
							schedule.InWorkWindow = (block.Type == "work" || block.Type == "commitment")
							break
						}
					}
				}
			}
		}
	}

	// Default if nothing found
	if schedule.CurrentActivity == "" {
		schedule.CurrentActivity = "Unscheduled time"
		schedule.ActivityType = "flex"
	}

	return schedule, nil
}

// GetExternalCalendar orchestrates calendar library for base calendar awareness
func GetExternalCalendar(currentTime time.Time) (*ExternalCalendar, error) {
	year := currentTime.Year()
	month := int(currentTime.Month())
	day := currentTime.Day()

	// Use extracted calendar library
	dateInfo, err := calendar.GetDateInfo(year, month, day)
	if err != nil {
		return nil, err
	}

	monthInfo, err := calendar.GetMonthInfo(year, month)
	if err != nil {
		return nil, err
	}

	ext := &ExternalCalendar{
		Date:        dateInfo.Date,
		Year:        year,
		DayOfWeek:   dateInfo.DayOfWeek,
		WeekNumber:  dateInfo.Week,
		IsHoliday:   dateInfo.IsHoliday,
		HolidayName: dateInfo.Holiday,
		MonthName:   monthInfo.Name,
		DayOfMonth:  day,
	}

	return ext, nil
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Library Functions Available for Import
// ════════════════════════════════════════════════════════════════════════════
// Exported functions (orchestrators, not reimplementers):
//   - GetTemporalContext() - Complete time and schedule awareness (all 4 dimensions)
//   - GetExternalTime() - System clock awareness
//   - GetInternalTime() - Session duration awareness (via sessiontime library)
//   - GetInternalSchedule() - Planner context awareness (via planner library)
//   - GetExternalCalendar() - Base calendar awareness (via calendar library)
