// ============================================================================
// CALENDAR LIBRARY DEMO
// ============================================================================
// Purpose: Demonstrate proper usage of calendar library
// Shows: Loading calendars, querying dates, accessing month info
// Educational: Reference for how consumers should use calendar library
//
// This is NOT a test - this is a DEMO showing real usage patterns

package main

import (
	"fmt"
	"os"
	"system/lib/calendar"
	"system/lib/display"
	"time"
)

func main() {
	fmt.Print(display.Box("Calendar Library Demo", "Usage Examples"))
	fmt.Println()

	// ========================================================================
	// DEMO 1: Load current month calendar
	// ========================================================================
	fmt.Print(display.Header("DEMO 1: Loading Current Month Calendar"))

	now := time.Now()
	currentYear := now.Year()
	currentMonth := int(now.Month())

	fmt.Printf("Loading calendar for %s %d...\n", time.Month(currentMonth), currentYear)

	cal, err := calendar.LoadMonthCalendar(currentYear, currentMonth)
	if err != nil {
		fmt.Printf("❌ FAILED: %v\n", err)
		fmt.Println("   Note: Calendar data may not exist for this month yet")

		// Fall back to November 2025 (known to exist)
		fmt.Println("\n   Falling back to November 2025 (known data)...")
		currentYear = 2025
		currentMonth = 11
		cal, err = calendar.LoadMonthCalendar(currentYear, currentMonth)
		if err != nil {
			fmt.Printf("❌ CRITICAL: Even fallback failed: %v\n", err)
			os.Exit(1)
		}
	}

	fmt.Printf("✅ SUCCESS: Loaded %s %d\n", time.Month(currentMonth), currentYear)
	fmt.Printf("   • Timezone: %s\n", cal.Metadata.Timezone)
	fmt.Printf("   • Total days in month: %d\n", cal.Metadata.TotalDays)
	fmt.Printf("   • Date entries loaded: %d\n", len(cal.Dates))
	fmt.Printf("   • Observes holidays: %v\n", cal.Metadata.ObservesHolidays)
	fmt.Println()

	// ========================================================================
	// DEMO 2: Query specific date information
	// ========================================================================
	fmt.Print(display.Header("DEMO 2: Querying Specific Date Information"))

	queryYear := currentYear
	queryMonth := currentMonth
	queryDay := 12

	fmt.Printf("Querying date: %d-%02d-%02d\n", queryYear, queryMonth, queryDay)

	dateInfo, err := calendar.GetDateInfo(queryYear, queryMonth, queryDay)
	if err != nil {
		fmt.Printf("❌ FAILED: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("✅ SUCCESS: Date found\n")
	fmt.Printf("   • Full date: %s\n", dateInfo.Date)
	fmt.Printf("   • Day of week: %s\n", dateInfo.DayOfWeek)
	fmt.Printf("   • Week number: %d (ISO 8601)\n", dateInfo.Week)
	fmt.Printf("   • Is holiday: %v\n", dateInfo.IsHoliday)
	if dateInfo.IsHoliday {
		fmt.Printf("   • Holiday name: %s\n", dateInfo.Holiday)
	}
	fmt.Println()

	// ========================================================================
	// DEMO 3: Get month metadata
	// ========================================================================
	fmt.Print(display.Header("DEMO 3: Retrieving Month Metadata"))

	fmt.Printf("Getting metadata for month %d...\n", queryMonth)

	monthInfo, err := calendar.GetMonthInfo(queryYear, queryMonth)
	if err != nil {
		fmt.Printf("❌ FAILED: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("✅ SUCCESS: Month metadata retrieved\n")
	fmt.Printf("   • Month name: %s\n", monthInfo.Name)
	fmt.Printf("   • Days in month: %d\n", monthInfo.DaysInMonth)
	fmt.Printf("   • First day: %s (%s)\n", monthInfo.FirstDay, monthInfo.FirstWeekday)
	fmt.Printf("   • Last day: %s\n", monthInfo.LastDay)
	fmt.Println()

	// ========================================================================
	// DEMO 4: Find all holidays in month
	// ========================================================================
	fmt.Println("🎉 DEMO 4: Finding all holidays in month")
	fmt.Println("────────────────────────────────────────────────────────────────")

	holidayCount := 0
	fmt.Printf("Scanning %d dates for holidays...\n", len(cal.Dates))

	for _, date := range cal.Dates {
		if date.IsHoliday {
			holidayCount++
			fmt.Printf("   • %s (%s): %s\n", date.Date, date.DayOfWeek, date.Holiday)
		}
	}

	if holidayCount == 0 {
		fmt.Println("   No holidays found in this month")
	} else {
		fmt.Printf("✅ Found %d holiday(s)\n", holidayCount)
	}
	fmt.Println()

	// ========================================================================
	// DEMO 5: Count weekends in month
	// ========================================================================
	fmt.Println("📆 DEMO 5: Analyzing weekends in month")
	fmt.Println("────────────────────────────────────────────────────────────────")

	weekendDays := 0
	fmt.Println("Weekend days in month:")

	for _, date := range cal.Dates {
		if date.DayOfWeek == "Saturday" || date.DayOfWeek == "Sunday" {
			weekendDays++
			fmt.Printf("   • %s (%s)\n", date.Date, date.DayOfWeek)
		}
	}

	fmt.Printf("✅ Total weekend days: %d\n", weekendDays)
	fmt.Println()

	// ========================================================================
	// DEMO 6: Error handling - Invalid date
	// ========================================================================
	fmt.Println("⚠️  DEMO 6: Error handling demonstration")
	fmt.Println("────────────────────────────────────────────────────────────────")

	fmt.Println("Attempting to query invalid date (day 99)...")
	_, err = calendar.GetDateInfo(queryYear, queryMonth, 99)
	if err != nil {
		fmt.Printf("✅ EXPECTED ERROR: %v\n", err)
		fmt.Println("   (Library correctly handles invalid dates)")
	} else {
		fmt.Println("❌ UNEXPECTED: Should have returned error for invalid date")
	}
	fmt.Println()

	// ========================================================================
	// DEMO 7: Load different year
	// ========================================================================
	fmt.Println("📅 DEMO 7: Loading calendar from different year")
	fmt.Println("────────────────────────────────────────────────────────────────")

	fmt.Println("Loading January 2026...")
	cal2026, err := calendar.LoadMonthCalendar(2026, 1)
	if err != nil {
		fmt.Printf("⚠️  Calendar for 2026 not available: %v\n", err)
		fmt.Println("   (This is expected if 2026 data hasn't been created yet)")
	} else {
		fmt.Printf("✅ SUCCESS: Loaded January %d\n", cal2026.Year)
		fmt.Printf("   • Total days: %d\n", cal2026.Metadata.TotalDays)
	}
	fmt.Println()

	// ========================================================================
	// SUMMARY
	// ========================================================================
	fmt.Println("╔══════════════════════════════════════════════════════════════╗")
	fmt.Println("║                      Demo Complete                           ║")
	fmt.Println("╚══════════════════════════════════════════════════════════════╝")
	fmt.Println()
	fmt.Println("✅ All demos executed successfully")
	fmt.Println()
	fmt.Println("KEY LEARNINGS:")
	fmt.Println("• LoadMonthCalendar(year, month) - Loads full month calendar")
	fmt.Println("• GetDateInfo(year, month, day) - Gets info for specific date")
	fmt.Println("• GetMonthInfo(year, month) - Gets month-level metadata")
	fmt.Println("• Library handles JSONC files automatically")
	fmt.Println("• Proper error handling for missing/invalid data")
	fmt.Println("• ISO 8601 week numbering used throughout")
	fmt.Println()
	fmt.Println("REFERENCE:")
	fmt.Println("Data location: ~/.claude/system/data/temporal/chronological/calendar/")
	fmt.Println("Library location: system/runtime/lib/calendar/")
	fmt.Println()
}
