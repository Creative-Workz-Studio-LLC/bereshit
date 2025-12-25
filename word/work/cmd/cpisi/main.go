// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-cmd-cpisi
// Purpose: CPI-SI State Machine - Entry Point
// Biblical: Genesis 1:31 - "And God saw every thing that he had made"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-19
// Updated: 2025-12-20
//
// Multi-accessible interface pattern:
// - CLI:     Machine-friendly (scripts, hooks, automation)
// - TUI:     Human-friendly (interactive terminal)
// - Daemon:  Live state watching (background)
// - Status:  One-shot display
//
// Local First: Runs on machine, no cloud dependency
// Agnostic:    Logic in packages, UI choices here
// Transpilable: State file as API - any language can consume
//
// ═══════════════════════════════════════════════════════════════════════════

package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Entry Point ---

func main() {
	if len(os.Args) < 2 {
		printUsage()
		return
	}

	switch os.Args[1] {
	case "cli":
		runCLI()
	case "tui":
		runTUI()
	case "status":
		runStatus()
	case "daemon":
		runDaemon()
	case "set":
		if len(os.Args) < 3 {
			fmt.Println("Usage: cpisi set <position>")
			return
		}
		runSet(os.Args[2])
	case "psi":
		if len(os.Args) < 3 {
			fmt.Println("Usage: cpisi psi <N>")
			return
		}
		runPsi(os.Args[2])
	case "explore":
		runExplore(os.Args[2:])
	case "compute":
		runCompute(os.Args[2:])
	case "help", "-h", "--help":
		printUsage()
	default:
		fmt.Printf("Unknown mode: %s\n", os.Args[1])
		printUsage()
	}
}

// --- Usage ---

func printUsage() {
	fmt.Println(`CPI-SI State Machine - Kingdom Technology

Usage: cpisi <mode> [args]

Modes:
  cli      Interactive CLI (simple text, no TTY required)
  tui      Interactive TUI (requires terminal)
  status   One-shot: show current state
  daemon   Background watcher (updates display on state changes)
  set      Set position: cpisi set PRESENT-PRESENT
  psi      Calculate Ψ(N): cpisi psi 1.5
  explore  Explore theories: cpisi explore <subcommand>
  compute  Geometric computation: cpisi compute <subcommand>
  help     Show this help

Explore Subcommands:
  range [min] [max]  - Scan N range, show coordinate bounds
  lambda <value>     - Test different λ values
  zpairs             - Show Z-pairing interconnections
  constants          - Show all defined constants
  transform <x> <y> <z> - Transform coordinates between perspectives

State File: ~/.claude/state/position.json
  - Hooks update this file
  - Daemon watches for changes
  - View Nova's cognitive state in real-time

Positions:
  PAST-PAST       PAST-PRESENT       PAST-FUTURE
  PRESENT-PAST    PRESENT-PRESENT    PRESENT-FUTURE
  FUTURE-PAST     FUTURE-PRESENT     FUTURE-FUTURE

Commands: HALT (-1), AWAIT (0), PROCEED (+1)
Operators: <- - < (PAST) | @ | = (PRESENT) | + -> > (FUTURE)`)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Entry point only. All mode implementations in separate files:
//
// state.go   - State file contract (StateData, load/save)
// cli.go     - CLI mode (runCLI, runStatus, runSet, runPsi)
// tui.go     - TUI mode (runTUI, Bubble Tea model)
// daemon.go  - Daemon mode (runDaemon, live watching)
// explore.go - Explore subcommands (theories, constants)
// compute.go - Compute subcommands (geometric computation)
//
// The wheel within a wheel: one entry point, many views.
//
// ═══════════════════════════════════════════════════════════════════════════
