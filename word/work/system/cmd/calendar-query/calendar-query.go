// ════════════════════════════════════════════════════════════════════════════
// METADATA - Calendar Query (Base Calendar Access)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Psalm 90:12 - "So teach us to number our days, that we
//   may apply our hearts unto wisdom."
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Calendar queries: Efficient access to base calendar data
//   Query specific months without loading entire year
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Query base calendar data by month for efficient access
//
// Usage:
//   calendar-query                                  # Defaults to today
//   calendar-query --year 2025 --month 11           # Query November 2025
//   calendar-query --year 2025 --month 11 --date 15 # Query specific date
//   calendar-query --fulldate 2025-11-15            # Query by full date
//   calendar-query --json                           # JSON output for today
//
// Dependencies: encoding/json, time
//
// Health Scoring Map (Base100):
//   +100: Calendar data retrieved successfully
//   -30: Invalid parameters
//   -50: Calendar file not found
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
	Date       string  `json:"date"`
	Weekday    string  `json:"weekday"`
	WeekNumber int     `json:"week_number"`
	Month      int     `json:"month"`
	Day        int     `json:"day"`
	IsWeekend  bool    `json:"is_weekend"`
	IsHoliday  bool    `json:"is_holiday"`
	HolidayName *string `json:"holiday_name"`
}

type MonthInfo struct {
	Month        int    `json:"month"`
	Name         string `json:"name"`
	DaysInMonth  int    `json:"days_in_month"`
	FirstDay     string `json:"first_day"`
	LastDay      string `json:"last_day"`
	FirstWeekday string `json:"first_weekday"`
}

// ════════════════════════════════════════════════════════════════════════════
// BODY - Calendar Query Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	year := flag.Int("year", 0, "Year to query (e.g., 2025, defaults to current year)")
	month := flag.Int("month", 0, "Month to query (1-12, defaults to current month)")
	day := flag.Int("date", 0, "Optional: specific day to query (defaults to current day)")
	fullDate := flag.String("fulldate", "", "Full date query (YYYY-MM-DD)")
	jsonOutput := flag.Bool("json", false, "Output as JSON")
	flag.Parse()

	// Handle full date format
	if *fullDate != "" {
		parsedDate, err := time.Parse("2006-01-02", *fullDate)
		if err != nil {
			fmt.Printf("❌ Invalid date format: %s (use YYYY-MM-DD)\n", *fullDate)
			os.Exit(1)
		}

		// Extract year, month, day from parsed date
		*year = parsedDate.Year()
		*month = int(parsedDate.Month())
		*day = parsedDate.Day()
	}

	// Default to today if no parameters specified
	if *year == 0 || *month == 0 {
		now := time.Now()
		if *year == 0 {
			*year = now.Year()
		}
		if *month == 0 {
			*month = int(now.Month())
		}
		if *day == 0 {
			*day = now.Day()
		}
	}

	if *month < 1 || *month > 12 {
		fmt.Printf("❌ Invalid month: %d (must be 1-12)\n", *month)
		os.Exit(1)
	}

	// Load month calendar
	calendar, err := loadMonthCalendar(*year, *month)
	if err != nil {
		fmt.Printf("❌ Error loading calendar: %v\n", err)
		os.Exit(1)
	}

	// If specific date requested
	if *day != 0 {
		dateStr := fmt.Sprintf("%04d-%02d-%02d", *year, *month, *day)
		dateInfo, exists := calendar.Dates[dateStr]
		if !exists {
			fmt.Printf("❌ Date not found: %s\n", dateStr)
			os.Exit(1)
		}

		if *jsonOutput {
			data, _ := json.MarshalIndent(dateInfo, "", "  ")
			fmt.Println(string(data))
		} else {
			displayDateInfo(dateInfo)
		}
		return
	}

	// Display month calendar
	if *jsonOutput {
		data, _ := json.MarshalIndent(calendar, "", "  ")
		fmt.Println(string(data))
	} else {
		displayMonthCalendar(calendar)
	}
}

func loadMonthCalendar(year, month int) (*Calendar, error) {
	monthName := strings.ToLower(time.Month(month).String())
	filename := fmt.Sprintf("%02d-%s.json", month, monthName)

	calendarFile := filepath.Join(os.Getenv("HOME"), ".claude", "calendar", "base",
		fmt.Sprintf("%d", year), filename)

	data, err := os.ReadFile(calendarFile)
	if err != nil {
		return nil, fmt.Errorf("calendar file not found (have you run calendar-generate --year %d --monthly?): %w", year, err)
	}

	var calendar Calendar
	if err := json.Unmarshal(data, &calendar); err != nil {
		return nil, fmt.Errorf("failed to parse calendar: %w", err)
	}

	return &calendar, nil
}

func displayDateInfo(date DateInfo) {
	fmt.Printf("\n📅 %s\n", date.Date)
	fmt.Printf("   %s\n", date.Weekday)
	fmt.Printf("   Week %d\n", date.WeekNumber)

	if date.IsWeekend {
		fmt.Println("   Weekend")
	}

	if date.IsHoliday && date.HolidayName != nil {
		fmt.Printf("   🎉 %s\n", *date.HolidayName)
	}
	fmt.Println()
}

func displayMonthCalendar(calendar *Calendar) {
	// Get the month info from the Months map (should only have one entry)
	var monthInfo MonthInfo
	for _, m := range calendar.Months {
		monthInfo = m
		break
	}

	fmt.Printf("\n📅 %s %d\n", monthInfo.Name, calendar.Year)
	fmt.Printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n")
	fmt.Printf("   %d days total\n", monthInfo.DaysInMonth)
	fmt.Printf("   Starts: %s (%s)\n", monthInfo.FirstDay, monthInfo.FirstWeekday)
	fmt.Printf("   Ends:   %s\n\n", monthInfo.LastDay)

	// Count weekends and holidays
	var weekendCount int
	var holidays []string

	for _, dateInfo := range calendar.Dates {
		if dateInfo.IsWeekend {
			weekendCount++
		}
		if dateInfo.IsHoliday && dateInfo.HolidayName != nil {
			holidays = append(holidays, fmt.Sprintf("%s: %s", dateInfo.Date, *dateInfo.HolidayName))
		}
	}

	fmt.Printf("   Weekend days: %d\n", weekendCount)

	if len(holidays) > 0 {
		fmt.Printf("\n   🎉 Holidays:\n")
		for _, holiday := range holidays {
			fmt.Printf("      %s\n", holiday)
		}
	}

	fmt.Println()
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - queries base calendar data for specified month
