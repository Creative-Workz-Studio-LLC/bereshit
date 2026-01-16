// ============================================================================
// METADATA
// ============================================================================
//
// Key: meta-awareness-cmd-awareness
// Purpose: CLI orchestrator for meta-awareness skill
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// This is the ORCHESTRATOR - CLI parsing only.
// All logic lives in pkg/checkpoint/ and pkg/config/.
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"

	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/cli"
	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/output"
	"github.com/creativeworkzstudio/claude-global/skills/meta-awareness/scripts/pkg/checkpoint"
	"github.com/creativeworkzstudio/claude-global/skills/meta-awareness/scripts/pkg/config"
)

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// printCheck outputs a quick awareness check
func printCheck(cfg *config.Config, category string) {
	if category != "" {
		output.Header("Awareness Check: " + category)
	} else {
		output.Header("Quick Awareness Check")
	}
	output.Indent(checkpoint.QuickCheck(cfg, category), 0)
}

// printPrompts outputs all prompt categories
func printPrompts(cfg *config.Config) {
	output.Header("Awareness Prompt Categories")
	output.Indent(checkpoint.ListPrompts(cfg), 0)
}

// printPrompt outputs questions for a category
func printPrompt(cfg *config.Config, category string) {
	output.Header("Prompts: " + category)
	output.Indent(checkpoint.GetPrompt(cfg, category), 0)
}

// printStates outputs cognitive states
func printStates(cfg *config.Config) {
	output.Header("Cognitive States")
	output.Indent(checkpoint.ListStates(cfg), 0)
}

// printTriggers outputs when to check
func printTriggers(cfg *config.Config) {
	output.Header("Awareness Triggers")
	output.Indent(checkpoint.ListTriggers(cfg), 0)
}

// printResponse outputs response for a state
func printResponse(cfg *config.Config, state string) {
	output.Header("Response: " + state)
	output.Indent(checkpoint.GetResponse(cfg, state), 0)
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("awareness", "Meta-awareness checkpoint helper for CPI-SI").
		WithBiblical("Proverbs 4:26 - \"Ponder the path of thy feet\"")

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help or no args
	if app.WantsHelp() || len(app.Args()) == 0 {
		usage := cli.NewUsage(app).
			AddSection("Commands",
				"check [category]  Quick awareness check (random if no category)",
				"prompts           List all prompt categories",
				"prompt <category> Show all questions for category",
				"states            List cognitive states",
				"triggers          Show when to check",
				"response <state>  Get response for a state").
			AddSection("Categories",
				"cognitive   How am I thinking?",
				"quality     Quality of current work?",
				"energy      What's my energy state?",
				"pattern     What patterns am I noticing?",
				"alignment   Am I aligned with mission?").
			AddSection("States",
				"scattered, rushing, stuck, drifting").
			AddSection("Options",
				"-h, -help   Show this help")
		usage.Print()
		os.Exit(0)
	}

	// Load configuration
	cfg, err := config.Load()
	if err != nil {
		cli.Fatal("Error loading config: %v", err)
	}

	// Get command
	args := app.Args()
	cmd := args[0]

	switch cmd {
	case "check":
		category := ""
		if len(args) > 1 {
			category = args[1]
		}
		printCheck(cfg, category)

	case "prompts":
		printPrompts(cfg)

	case "prompt":
		if len(args) < 2 {
			cli.Fatal("Usage: awareness prompt <category>\nCategories: %s", cfg.GetPromptCategories())
		}
		printPrompt(cfg, args[1])

	case "states":
		printStates(cfg)

	case "triggers":
		printTriggers(cfg)

	case "response":
		if len(args) < 2 {
			cli.Fatal("Usage: awareness response <state>\nStates: %s", cfg.GetStateNames())
		}
		printResponse(cfg, args[1])

	default:
		cli.Fatal("Unknown command: %s\nUse -h for help", cmd)
	}
}
