// ════════════════════════════════════════════════════════════════════════════
// METADATA - Calendar Library (Extracted Core)
// ════════════════════════════════════════════════════════════════════════════
//
// Purpose: Core calendar functions extracted from calendar-query tool
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
//
// Exported Functions:
//   - LoadMonthCalendar() - Load calendar for specific month
//   - GetDateInfo() - Get info for specific date
//
// ════════════════════════════════════════════════════════════════════════════

package calendar

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"

	"system/lib/jsonc"
	"system/lib/paths"
)

// Calendar structure
type Calendar struct {
	Year     int                `json:"year"`
	Metadata CalendarMetadata   `json:"metadata"`
	Dates    map[string]DateInfo `json:"dates"`
	Months   map[int]MonthInfo   `json:"months"`
}

type CalendarMetadata struct {
	Created          string   `json:"created"`
	Timezone         string   `json:"timezone"`
	ObservesHolidays []string `json:"observes_holidays"`
	TotalDays        int      `json:"total_days"`
}

type DateInfo struct {
	Date      string `json:"date"`
	Month     int    `json:"month"`
	Day       int    `json:"day"`
	DayOfWeek string `json:"weekday"`
	Week      int    `json:"week_number"`
	IsHoliday bool   `json:"is_holiday"`
	Holiday   string `json:"holiday_name,omitempty"`
}

type MonthInfo struct {
	Month        int    `json:"month"`
	Name         string `json:"name"`
	DaysInMonth  int    `json:"days_in_month"`
	FirstDay     string `json:"first_day"`
	LastDay      string `json:"last_day"`
	FirstWeekday string `json:"first_weekday"`
}

// LoadMonthCalendar loads calendar data for a specific month
func LoadMonthCalendar(year, month int) (*Calendar, error) {
	// Load paths from config
	pathsConfig, err := paths.Load()
	if err != nil {
		return nil, fmt.Errorf("failed to load paths config: %w", err)
	}

	// Resolve calendar base path
	calendarBase, err := paths.ResolveFull(pathsConfig.Temporal.CalendarBase)
	if err != nil {
		return nil, fmt.Errorf("failed to resolve calendar path: %w", err)
	}

	monthName := strings.ToLower(time.Month(month).String())
	filename := fmt.Sprintf("%02d-%s.jsonc", month, monthName)

	calendarFile := filepath.Join(calendarBase, fmt.Sprintf("%d", year), filename)

	data, err := os.ReadFile(calendarFile)
	if err != nil {
		return nil, fmt.Errorf("calendar file not found (have you run calendar-generate --year %d --monthly?): %w", year, err)
	}

	// Strip JSONC comments before parsing
	cleanedData := jsonc.StripComments(data)

	var cal Calendar
	if err := json.Unmarshal(cleanedData, &cal); err != nil {
		return nil, fmt.Errorf("failed to parse calendar: %w", err)
	}

	return &cal, nil
}

// GetDateInfo retrieves information for a specific date
func GetDateInfo(year, month, day int) (*DateInfo, error) {
	cal, err := LoadMonthCalendar(year, month)
	if err != nil {
		return nil, err
	}

	dateStr := fmt.Sprintf("%04d-%02d-%02d", year, month, day)
	dateInfo, exists := cal.Dates[dateStr]
	if !exists {
		return nil, fmt.Errorf("date not found: %s", dateStr)
	}

	return &dateInfo, nil
}

// GetMonthInfo retrieves information for a specific month
func GetMonthInfo(year, month int) (*MonthInfo, error) {
	cal, err := LoadMonthCalendar(year, month)
	if err != nil {
		return nil, err
	}

	monthInfo, exists := cal.Months[month]
	if !exists {
		return nil, fmt.Errorf("month not found: %d", month)
	}

	return &monthInfo, nil
}
