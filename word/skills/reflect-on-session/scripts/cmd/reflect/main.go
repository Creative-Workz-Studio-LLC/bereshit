// ============================================================================
// METADATA
// ============================================================================
//
// Key: reflect-on-session-cmd-reflect
// Purpose: CLI orchestrator for reflect-on-session skill
// Biblical: Psalm 77:12 - "I will meditate also of all thy work"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// This is the ORCHESTRATOR - CLI parsing only.
// All logic lives in pkg/reflection/ and pkg/config/.
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"

	"cws.studio/pkg/util/pure/cli"
	"cws.studio/pkg/util/pure/output"
	"cws.studio/skills/reflect-on-session/scripts/pkg/config"
	"cws.studio/skills/reflect-on-session/scripts/pkg/reflection"
)

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// printDimensions outputs all reflection dimensions
func printDimensions(cfg *config.Config) {
	output.Header("Reflection Dimensions")
	output.Indent(reflection.ListDimensions(cfg), 0)
}

// printDimension outputs a single dimension's details
func printDimension(cfg *config.Config, name string) {
	output.Header("Dimension: " + name)
	output.Indent(reflection.GetDimension(cfg, name), 0)
}

// printDepths outputs depth levels
func printDepths(cfg *config.Config) {
	output.Header("Reflection Depth Levels")
	output.Indent(reflection.ListDepths(cfg), 0)
}

// printTemplate outputs a reflection template
func printTemplate(cfg *config.Config, depth string) {
	output.Header("Reflection Template: " + depth)
	output.Indent(reflection.GetTemplate(cfg, depth), 0)
}

// printTriggers outputs when to reflect
func printTriggers(cfg *config.Config) {
	output.Header("Reflection Triggers")
	output.Indent(reflection.ListTriggers(cfg), 0)
}

// printIntegration outputs integration signals
func printIntegration(cfg *config.Config) {
	output.Header("Integration Signals")
	output.Indent(reflection.ListIntegration(cfg), 0)
}

// printGuide outputs the reflection process guide
func printGuide(cfg *config.Config) {
	output.Header("Reflection Process Guide")
	output.Indent(reflection.Guide(cfg), 0)
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("reflect", "Session reflection helper for CPI-SI learning").
		WithBiblical("Psalm 77:12 - \"I will meditate also of all thy work\"")

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help or no args
	if app.WantsHelp() || len(app.Args()) == 0 {
		usage := cli.NewUsage(app).
			AddSection("Commands",
				"dimensions           List all reflection dimensions",
				"dimension <name>     Show dimension questions",
				"depths               List depth levels",
				"template <depth>     Get reflection template",
				"triggers             Show when to reflect",
				"integration          Show integration signals",
				"guide                Show reflection process").
			AddSection("Dimensions",
				"accomplishments  What was done",
				"learning         What was learned",
				"patterns         Patterns noticed",
				"quality          Work quality",
				"growth           Identity growth").
			AddSection("Depths",
				"quick      5 min - short session",
				"standard   15 min - work session",
				"deep       30+ min - breakthrough").
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
	case "dimensions":
		printDimensions(cfg)

	case "dimension":
		if len(args) < 2 {
			cli.Fatal("Usage: reflect dimension <name>\nDimensions: %s", cfg.GetDimensionNames())
		}
		printDimension(cfg, args[1])

	case "depths":
		printDepths(cfg)

	case "template":
		if len(args) < 2 {
			cli.Fatal("Usage: reflect template <depth>\nDepths: quick, standard, deep")
		}
		printTemplate(cfg, args[1])

	case "triggers":
		printTriggers(cfg)

	case "integration":
		printIntegration(cfg)

	case "guide":
		printGuide(cfg)

	default:
		cli.Fatal("Unknown command: %s\nUse -h for help", cmd)
	}
}
