// ════════════════════════════════════════════════════════════════════════════
// METADATA - Calendar Generator (Base Calendar Creation)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Genesis 1:14 - "And God said, Let there be lights in the
//   firmament of the heaven to divide the day from the night; and let them be
//   for signs, and for seasons, and for days, and years"
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   Base calendar: Shared time reference for all schedules
//   Generates immutable calendar data (dates, weekdays, holidays)
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Generate base calendar data for coordination system
//
// Usage:
//   calendar-generate --year 2025
//   calendar-generate --year 2026
//   calendar-generate --years 2025,2026
//   calendar-generate --year 2025 --monthly  (generates month-by-month files)
//
// Dependencies: encoding/json, time
//
// Health Scoring Map (Base100):
//   +100: Calendar generated successfully
//   -30: Invalid year parameter
//   -50: Cannot write calendar file
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
	"strconv"
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

// US Federal Holidays (fixed dates and calculated)
var holidays2025 = map[string]string{
	"2025-01-01": "New Year's Day",
	"2025-01-20": "Martin Luther King Jr. Day",
	"2025-02-17": "Presidents' Day",
	"2025-05-26": "Memorial Day",
	"2025-07-04": "Independence Day",
	"2025-09-01": "Labor Day",
	"2025-10-13": "Columbus Day",
	"2025-11-11": "Veterans Day",
	"2025-11-27": "Thanksgiving Day",
	"2025-12-25": "Christmas Day",
}

var holidays2026 = map[string]string{
	"2026-01-01": "New Year's Day",
	"2026-01-19": "Martin Luther King Jr. Day",
	"2026-02-16": "Presidents' Day",
	"2026-05-25": "Memorial Day",
	"2026-07-03": "Independence Day (Observed)", // July 4 is Saturday
	"2026-07-04": "Independence Day",
	"2026-09-07": "Labor Day",
	"2026-10-12": "Columbus Day",
	"2026-11-11": "Veterans Day",
	"2026-11-26": "Thanksgiving Day",
	"2026-12-25": "Christmas Day",
}

// ════════════════════════════════════════════════════════════════════════════
// BODY - Calendar Generation Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	yearFlag := flag.String("year", "", "Year to generate (e.g., 2025)")
	yearsFlag := flag.String("years", "", "Years to generate comma-separated (e.g., 2025,2026)")
	monthlyFlag := flag.Bool("monthly", false, "Generate separate file for each month")
	flag.Parse()

	var years []int

	if *yearsFlag != "" {
		parts := strings.Split(*yearsFlag, ",")
		for _, part := range parts {
			year, err := strconv.Atoi(strings.TrimSpace(part))
			if err != nil {
				fmt.Printf("❌ Invalid year: %s\n", part)
				os.Exit(2) // Exit 2 = usage error
			}
			years = append(years, year)
		}
	} else if *yearFlag != "" {
		year, err := strconv.Atoi(*yearFlag)
		if err != nil {
			fmt.Printf("❌ Invalid year: %s\n", *yearFlag)
			os.Exit(2) // Exit 2 = usage error
		}
		years = append(years, year)
	} else {
		fmt.Println("❌ Must specify --year or --years")
		fmt.Println()
		fmt.Println("Usage:")
		fmt.Println("  calendar-generate --year 2025")
		fmt.Println("  calendar-generate --years 2025,2026")
		os.Exit(2) // Exit 2 = usage error
	}

	for _, year := range years {
		if err := generateCalendar(year, *monthlyFlag); err != nil {
			fmt.Printf("❌ Error generating %d calendar: %v\n", year, err)
			os.Exit(1)
		}
		if *monthlyFlag {
			fmt.Printf("✅ Generated %d calendar (12 monthly files)\n", year)
		} else {
			fmt.Printf("✅ Generated %d calendar\n", year)
		}
	}
}

func generateCalendar(year int, monthly bool) error {
	// Create calendar structure
	calendar := Calendar{
		Year: year,
		Metadata: CalendarMetadata{
			Created:          time.Now().Format("2006-01-02"),
			Timezone:         "America/Chicago",
			ObservesHolidays: []string{"US Federal"},
			TotalDays:        daysInYear(year),
		},
		Dates:  make(map[string]DateInfo),
		Months: make(map[int]MonthInfo),
	}

	// Get holidays for this year
	holidays := getHolidays(year)

	// Generate all dates
	startDate := time.Date(year, 1, 1, 0, 0, 0, 0, time.UTC)
	for d := 0; d < calendar.Metadata.TotalDays; d++ {
		date := startDate.AddDate(0, 0, d)
		dateStr := date.Format("2006-01-02")

		holidayName := holidays[dateStr]
		isHoliday := holidayName != ""

		_, week := date.ISOWeek()

		dateInfo := DateInfo{
			Date:       dateStr,
			Weekday:    date.Weekday().String(),
			WeekNumber: week,
			Month:      int(date.Month()),
			Day:        date.Day(),
			IsWeekend:  date.Weekday() == time.Saturday || date.Weekday() == time.Sunday,
			IsHoliday:  isHoliday,
			HolidayName: nil,
		}

		if isHoliday {
			dateInfo.HolidayName = &holidayName
		}

		calendar.Dates[dateStr] = dateInfo
	}

	// Generate month information
	for month := 1; month <= 12; month++ {
		firstDay := time.Date(year, time.Month(month), 1, 0, 0, 0, 0, time.UTC)
		lastDay := firstDay.AddDate(0, 1, -1)

		monthInfo := MonthInfo{
			Month:        month,
			Name:         firstDay.Month().String(),
			DaysInMonth:  lastDay.Day(),
			FirstDay:     firstDay.Format("2006-01-02"),
			LastDay:      lastDay.Format("2006-01-02"),
			FirstWeekday: firstDay.Weekday().String(),
		}

		calendar.Months[month] = monthInfo
	}

	// Save calendar (either as single file or monthly files)
	if monthly {
		return saveCalendarMonthly(calendar)
	}
	return saveCalendar(calendar)
}

func daysInYear(year int) int {
	if isLeapYear(year) {
		return 366
	}
	return 365
}

func isLeapYear(year int) bool {
	return year%4 == 0 && (year%100 != 0 || year%400 == 0)
}

func getHolidays(year int) map[string]string {
	switch year {
	case 2025:
		return holidays2025
	case 2026:
		return holidays2026
	default:
		return make(map[string]string)
	}
}

func saveCalendar(calendar Calendar) error {
	// Ensure calendar directory exists
	calendarDir := filepath.Join(os.Getenv("HOME"), ".claude", "calendar", "base")
	if err := os.MkdirAll(calendarDir, 0755); err != nil {
		return err
	}

	// Write calendar file
	filename := fmt.Sprintf("%d.json", calendar.Year)
	calendarFile := filepath.Join(calendarDir, filename)

	data, err := json.MarshalIndent(calendar, "", "  ")
	if err != nil {
		return err
	}

	return os.WriteFile(calendarFile, data, 0644)
}

func saveCalendarMonthly(calendar Calendar) error {
	// Create year directory
	yearDir := filepath.Join(os.Getenv("HOME"), ".claude", "calendar", "base", fmt.Sprintf("%d", calendar.Year))
	if err := os.MkdirAll(yearDir, 0755); err != nil {
		return err
	}

	// Generate separate file for each month
	for month := 1; month <= 12; month++ {
		monthCalendar := Calendar{
			Year: calendar.Year,
			Metadata: CalendarMetadata{
				Created:          calendar.Metadata.Created,
				Timezone:         calendar.Metadata.Timezone,
				ObservesHolidays: calendar.Metadata.ObservesHolidays,
				TotalDays:        calendar.Months[month].DaysInMonth,
			},
			Dates:  make(map[string]DateInfo),
			Months: make(map[int]MonthInfo),
		}

		// Add month info
		monthCalendar.Months[month] = calendar.Months[month]

		// Add only dates for this month
		for dateStr, dateInfo := range calendar.Dates {
			if dateInfo.Month == month {
				monthCalendar.Dates[dateStr] = dateInfo
			}
		}

		// Write month file
		monthName := time.Month(month).String()
		filename := fmt.Sprintf("%02d-%s.json", month, strings.ToLower(monthName))
		monthFile := filepath.Join(yearDir, filename)

		data, err := json.MarshalIndent(monthCalendar, "", "  ")
		if err != nil {
			return err
		}

		if err := os.WriteFile(monthFile, data, 0644); err != nil {
			return err
		}
	}

	return nil
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - generates base calendar data for specified year(s)
