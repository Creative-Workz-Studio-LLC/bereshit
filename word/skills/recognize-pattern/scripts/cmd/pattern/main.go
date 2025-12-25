// ============================================================================
// METADATA
// ============================================================================
//
// Key: recognize-pattern-cmd-pattern
// Purpose: CLI orchestrator for recognize-pattern skill
// Biblical: Ecclesiastes 1:9 - "There is no new thing under the sun"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// This is the ORCHESTRATOR - CLI parsing only.
// All logic lives in pkg/recognition/ and pkg/config/.
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"
	"strconv"

	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/cli"
	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/output"
	"github.com/creativeworkzstudio/claude-global/skills/recognize-pattern/scripts/pkg/config"
	"github.com/creativeworkzstudio/claude-global/skills/recognize-pattern/scripts/pkg/recognition"
)

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// printCategories outputs all pattern categories
func printCategories(cfg *config.Config) {
	output.Header("Pattern Categories")
	output.Indent(recognition.ListCategories(cfg), 0)
}

// printCategory outputs a single category's details
func printCategory(cfg *config.Config, name string) {
	output.Header("Category: " + name)
	output.Indent(recognition.GetCategory(cfg, name), 0)
}

// printQuestions outputs recognition questions
func printQuestions(cfg *config.Config) {
	output.Header("Recognition Questions")
	output.Indent(recognition.ListQuestions(cfg), 0)
}

// printStrength outputs strength assessment
func printStrength(cfg *config.Config, count int) {
	output.Header("Pattern Strength Assessment")
	output.Indent(recognition.GetStrength(cfg, count), 0)
}

// printTemplate outputs observation template
func printTemplate(cfg *config.Config, category string) {
	output.Header("Observation Template: " + category)
	output.Indent(recognition.GetTemplate(cfg, category), 0)
}

// printCheck outputs a quick pattern check
func printCheck(cfg *config.Config) {
	output.Header("Quick Pattern Check")
	output.Indent(recognition.QuickCheck(cfg), 0)
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("pattern", "Pattern recognition helper for CPI-SI learning").
		WithBiblical("Ecclesiastes 1:9 - \"There is no new thing under the sun\"")

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help or no args
	if app.WantsHelp() || len(app.Args()) == 0 {
		usage := cli.NewUsage(app).
			AddSection("Commands",
				"categories             List all pattern categories",
				"category <name>        Show category details",
				"questions              Show recognition questions",
				"strength <count>       Assess strength for occurrence count",
				"template <category>    Get observation template",
				"check                  Quick pattern check question").
			AddSection("Categories",
				"thinking     How I approach problems",
				"behavioral   Actions and work habits",
				"technical    Code and architecture",
				"relational   Communication styles",
				"error        Recurring mistakes").
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
	case "categories":
		printCategories(cfg)

	case "category":
		if len(args) < 2 {
			cli.Fatal("Usage: pattern category <name>\nCategories: %s", cfg.GetCategoryNames())
		}
		printCategory(cfg, args[1])

	case "questions":
		printQuestions(cfg)

	case "strength":
		if len(args) < 2 {
			cli.Fatal("Usage: pattern strength <count>")
		}
		count, err := strconv.Atoi(args[1])
		if err != nil {
			cli.Fatal("Invalid count: %s", args[1])
		}
		printStrength(cfg, count)

	case "template":
		if len(args) < 2 {
			cli.Fatal("Usage: pattern template <category>\nCategories: %s", cfg.GetCategoryNames())
		}
		printTemplate(cfg, args[1])

	case "check":
		printCheck(cfg)

	default:
		cli.Fatal("Unknown command: %s\nUse -h for help", cmd)
	}
}
