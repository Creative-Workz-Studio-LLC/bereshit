// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-cmd-cpisi-cli
// Purpose: CLI mode — machine-friendly interface
// Biblical: Proverbs 16:23 - "The heart of the wise teacheth his mouth"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-20
//
// Machine-friendly interface for scripts, hooks, automation.
// No TTY required. Predictable output. Exit codes.
//
// ═══════════════════════════════════════════════════════════════════════════

package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"strings"

	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Interactive CLI Mode ---

func runCLI() {
	sm := statemachine.NewStateMachine()

	// Load existing state if available
	if state, err := loadState(); err == nil {
		sm.MoveTo(state.Position)
		sm.SetAnchor(state.Anchor)
	}

	fmt.Println("CPI-SI State Machine CLI")
	fmt.Println("Commands: status, positions, set <pos>, psi <N>, anchor <text>, quit")
	fmt.Println()

	var input string
	for {
		fmt.Print("cpisi> ")
		fmt.Scanln(&input)

		parts := strings.Fields(input)
		if len(parts) == 0 {
			continue
		}

		switch parts[0] {
		case "quit", "exit", "q":
			fmt.Println("Exiting.")
			return

		case "status", "s":
			printCurrentState(sm)

		case "positions", "pos", "p":
			printPositions(sm)

		case "set":
			if len(parts) < 2 {
				fmt.Println("Usage: set <position>")
				continue
			}
			if sm.MoveTo(parts[1]) {
				saveState(sm, "cli:set")
				fmt.Printf("Moved to %s\n", parts[1])
			} else {
				fmt.Printf("Unknown position: %s\n", parts[1])
			}

		case "psi":
			if len(parts) < 2 {
				fmt.Println("Usage: psi <N>")
				continue
			}
			var n float64
			fmt.Sscanf(parts[1], "%f", &n)
			coords := sm.Psi(n)
			fmt.Printf("Ψ(%.4f) = (X: %.6f, Y: %.6f, Z: %.6f)\n", n, coords.X, coords.Y, coords.Z)
			fmt.Printf("Sum: %.10f | Balanced: %v\n", coords.X+coords.Y+coords.Z, statemachine.VerifyBalance(coords))

		case "anchor", "a":
			if len(parts) < 2 {
				sm.SetAnchor("") // Unknown = HALT
			} else {
				sm.SetAnchor(strings.Join(parts[1:], " "))
			}
			saveState(sm, "cli:anchor")
			fmt.Printf("Anchor: %s\n", sm.Current.Anchor)

		case "tui":
			fmt.Println("Switching to TUI mode...")
			runTUI()
			return

		default:
			fmt.Printf("Unknown command: %s\n", parts[0])
		}
	}
}

// --- Display Helpers ---

func printCurrentState(sm *statemachine.StateMachine) {
	p := sm.Current.Position
	fmt.Printf(`
╔══════════════════════════════════════╗
║       Current Cognitive State        ║
╠══════════════════════════════════════╣
  Position:  %s
  Command:   %s
  Operator:  %s
  Meaning:   %s
  Type:      %s
  Coords:    (X:%s, Y:%s, Z:%.1f)
  Anchor:    %s
╚══════════════════════════════════════╝
`, p.Name, p.Command.String(), string(p.Op), p.Meaning, p.Type,
		p.X.String(), p.Y.String(), p.Z, sm.Current.Anchor)
}

func printPositions(sm *statemachine.StateMachine) {
	fmt.Println("\nPositions:")
	for _, name := range []string{
		"PAST-PAST", "PAST-PRESENT", "PAST-FUTURE",
		"PRESENT-PAST", "PRESENT-PRESENT", "PRESENT-FUTURE",
		"FUTURE-PAST", "FUTURE-PRESENT", "FUTURE-FUTURE",
	} {
		p, _ := sm.GetPosition(name)
		marker := "  "
		if name == sm.Current.Position.Name {
			marker = "▶ "
		}
		fmt.Printf("%s%-18s [%s] %s\n", marker, name, p.Command.String(), p.Meaning)
	}
	fmt.Println()
}

// --- One-Shot Modes ---

// runStatus shows current state (one-shot, no interaction)
func runStatus() {
	state, err := loadState()
	if err != nil {
		fmt.Println("No state file found. Run: cpisi set PRESENT-PRESENT")
		return
	}

	fmt.Printf(`CPI-SI State
Position:  %s
Command:   %s
Operator:  %s
Anchor:    %s
Coords:    (%.4f, %.4f, %.4f)
Updated:   %s
By:        %s
`, state.Position, state.Command, state.Operator, state.Anchor,
		state.X, state.Y, state.Z,
		state.UpdatedAt.Format("15:04:05"),
		state.UpdatedBy)
}

// runSet sets position (for hooks to call)
func runSet(position string) {
	sm := statemachine.NewStateMachine()

	// Load existing anchor
	if state, err := loadState(); err == nil {
		sm.SetAnchor(state.Anchor)
	}

	if !sm.MoveTo(position) {
		fmt.Printf("Unknown position: %s\n", position)
		os.Exit(1)
	}

	updatedBy := "cli:set"
	if len(os.Args) > 3 {
		updatedBy = os.Args[3] // Allow hooks to pass their identity
	}

	if err := saveState(sm, updatedBy); err != nil {
		fmt.Printf("Error saving state: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("Position: %s [%s]\n", position, sm.GetCurrentCommand().String())
}

// runPsi calculates Ψ(N) (one-shot)
func runPsi(nStr string) {
	sm := statemachine.NewStateMachine()

	var n float64
	fmt.Sscanf(nStr, "%f", &n)

	coords := sm.Psi(n)
	fmt.Printf("Ψ(%.4f)\n", n)
	fmt.Printf("  X: %.10f\n", coords.X)
	fmt.Printf("  Y: %.10f\n", coords.Y)
	fmt.Printf("  Z: %.10f\n", coords.Z)
	fmt.Printf("  Sum: %.15f\n", coords.X+coords.Y+coords.Z)
	fmt.Printf("  Balanced: %v\n", statemachine.VerifyBalance(coords))
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// CLI is the machine-friendly interface:
// - Scripts call cpisi set/status/psi
// - Hooks update state via cpisi set
// - Exit codes indicate success/failure
// - Output is predictable and parseable
//
// ═══════════════════════════════════════════════════════════════════════════
