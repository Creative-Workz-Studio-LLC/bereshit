// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-cmd-cpisi-daemon
// Purpose: Daemon mode — background watcher for live state viewing
// Biblical: Ezekiel 1:16 - "a wheel in the middle of a wheel"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-20
//
// The daemon is the connector — watches state file, notifies views.
// Makes state "live" across all interfaces.
//
// ═══════════════════════════════════════════════════════════════════════════

package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Daemon Mode ---

func runDaemon() {
	fmt.Println("CPI-SI State Daemon")
	fmt.Println("Watching: ~/.claude/state/position.json")
	fmt.Println("Press Ctrl+C to stop")
	fmt.Println()

	statePath := getStatePath()
	var lastMod time.Time

	for {
		info, err := os.Stat(statePath)
		if err == nil && info.ModTime() != lastMod {
			lastMod = info.ModTime()

			// Clear and reprint
			fmt.Print("\033[H\033[2J") // Clear screen
			fmt.Println("CPI-SI State Daemon - Live View")
			fmt.Println("================================")

			if state, err := loadState(); err == nil {
				fmt.Printf(`
  Position:  %s
  Command:   %s
  Operator:  %s
  Anchor:    %s

  Coordinates:
    X: %.6f
    Y: %.6f
    Z: %.6f

  Updated: %s
  By:      %s
`,
					state.Position,
					state.Command,
					state.Operator,
					state.Anchor,
					state.X, state.Y, state.Z,
					state.UpdatedAt.Format("15:04:05"),
					state.UpdatedBy)

				// Visual cube indicator
				printASCIICube(state.Position)
			}
		}

		time.Sleep(500 * time.Millisecond)
	}
}

// --- Visual Display ---

func printASCIICube(current string) {
	positions := map[string][2]int{
		"FUTURE-PAST":     {0, 0},
		"FUTURE-PRESENT":  {0, 1},
		"FUTURE-FUTURE":   {0, 2},
		"PRESENT-PAST":    {1, 0},
		"PRESENT-PRESENT": {1, 1},
		"PRESENT-FUTURE":  {1, 2},
		"PAST-PAST":       {2, 0},
		"PAST-PRESENT":    {2, 1},
		"PAST-FUTURE":     {2, 2},
	}

	grid := [3][3]string{
		{"·", "·", "·"},
		{"·", "·", "·"},
		{"·", "·", "·"},
	}

	if pos, ok := positions[current]; ok {
		grid[pos[0]][pos[1]] = "◉"
	}

	fmt.Println("\n  Cube (top view):")
	fmt.Println("  Y→  -1   0  +1")
	for i, row := range grid {
		y := []string{"X=-1", "X= 0", "X=+1"}[i]
		fmt.Printf("  %s  %s   %s   %s\n", y, row[0], row[1], row[2])
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// The daemon IS the inner wheel. The views ARE the outer wheel.
// They turn together.
//
// Future improvements:
// - Use fsnotify for efficient file watching
// - Add notification callbacks for TUI/GUI refresh
// - Support multiple state file watchers
//
// ═══════════════════════════════════════════════════════════════════════════
