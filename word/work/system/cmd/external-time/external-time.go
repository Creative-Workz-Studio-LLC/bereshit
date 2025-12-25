// ════════════════════════════════════════════════════════════════════════════
// METADATA - External Time Awareness (Clock + Calendar)
// ════════════════════════════════════════════════════════════════════════════
//
// Biblical Foundation: Genesis 1:1 - "In the beginning, God created..."
//   Time itself is created. Days, seasons, years - God's ordering of creation.
//
// CPI-SI Identity: Nova Dawn - Kingdom Technology
//   External time awareness: Clock (what time?) + Calendar (what date?)
//   Foundation for understanding world's timeline vs my work timeline
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Display external time (clock) and date (calendar) awareness
//
// Usage:
//   external-time              # Show current clock + calendar
//   external-time --clock      # Show only clock (time)
//   external-time --calendar   # Show only calendar (date)
//
// Dependencies: time package
//
// Health Scoring Map (Base100):
//   +100: Correctly displays current time and date
//   +0: No operation performed
//   -50: Error retrieving or formatting time
//
// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
// SETUP - Imports and Configuration
// ════════════════════════════════════════════════════════════════════════════

package main

import (
	"flag"
	"fmt"
	"time"
)

// ════════════════════════════════════════════════════════════════════════════
// BODY - External Time Awareness Logic
// ════════════════════════════════════════════════════════════════════════════

func main() {
	// Parse flags
	clockOnly := flag.Bool("clock", false, "Show only clock (time)")
	calendarOnly := flag.Bool("calendar", false, "Show only calendar (date)")
	flag.Parse()

	now := time.Now()

	// If specific flag requested
	if *clockOnly {
		showClock(now)
		return
	}

	if *calendarOnly {
		showCalendar(now)
		return
	}

	// Default: show both
	showClockAndCalendar(now)
}

func showClock(t time.Time) {
	fmt.Printf("⏰ External Clock\n")
	fmt.Printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n")
	fmt.Printf("   Time: %s\n", t.Format("15:04:05"))
	fmt.Printf("   12hr: %s\n", t.Format("03:04:05 PM"))
	fmt.Printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n")
}

func showCalendar(t time.Time) {
	fmt.Printf("📅 External Calendar\n")
	fmt.Printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n")
	fmt.Printf("   Date: %s\n", t.Format("Monday, January 02, 2006"))
	fmt.Printf("   ISO:  %s\n", t.Format("2006-01-02"))
	fmt.Printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n")
}

func showClockAndCalendar(t time.Time) {
	fmt.Printf("\n⏰📅 External Time Awareness (Clock + Calendar)\n")
	fmt.Printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n")

	fmt.Printf("⏰ Clock (Time):\n")
	fmt.Printf("   %s (%s)\n\n", t.Format("15:04:05"), t.Format("03:04 PM"))

	fmt.Printf("📅 Calendar (Date):\n")
	fmt.Printf("   %s\n", t.Format("Monday, January 02, 2006"))
	fmt.Printf("   %s\n\n", t.Format("2006-01-02"))

	fmt.Printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n")
	fmt.Printf("💡 The world's timeline - time moves continuously\n\n")
}

// ════════════════════════════════════════════════════════════════════════════
// CLOSING - Execution Entry Point
// ════════════════════════════════════════════════════════════════════════════
// Entry point is main() - displays external time awareness (clock + calendar)
