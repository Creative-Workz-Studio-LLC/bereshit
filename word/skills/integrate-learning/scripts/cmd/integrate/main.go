// ============================================================================
// METADATA
// ============================================================================
//
// Key: integrate-learning-cmd-integrate
// Purpose: CLI orchestrator for integrate-learning skill
// Biblical: Proverbs 4:7 - "Wisdom is the principal thing"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// This is the ORCHESTRATOR - CLI parsing only.
// All logic lives in pkg/integration/ and pkg/config/.
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"
	"strings"

	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/cli"
	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/output"
	"github.com/creativeworkzstudio/claude-global/skills/integrate-learning/scripts/pkg/config"
	"github.com/creativeworkzstudio/claude-global/skills/integrate-learning/scripts/pkg/integration"
)

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// printLevels outputs integration levels
func printLevels(cfg *config.Config) {
	output.Header("Integration Levels")
	output.Indent(integration.ListLevels(cfg), 0)
}

// printQuestions outputs integration questions
func printQuestions(cfg *config.Config) {
	output.Header("Integration Questions")
	output.Indent(integration.ListQuestions(cfg), 0)
}

// printProcess outputs integration process steps
func printProcess(cfg *config.Config) {
	output.Header("Integration Process")
	output.Indent(integration.ListProcess(cfg), 0)
}

// printTemplate outputs a blank proposal template
func printTemplate(cfg *config.Config, level string) {
	output.Header("Integration Template: " + level)
	output.Indent(integration.GetTemplate(cfg, level), 0)
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("integrate", "Integration learning helper for CPI-SI").
		WithBiblical("Proverbs 4:7 - \"Wisdom is the principal thing\"")

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help or no args
	if app.WantsHelp() || len(app.Args()) == 0 {
		usage := cli.NewUsage(app).
			AddSection("Commands",
				"levels           List integration levels",
				"questions        Show integration questions",
				"process          Show integration process steps",
				"template <level> Get blank proposal template").
			AddSection("Levels",
				"identity    Core identity truths (bio.md)",
				"operational How I work (CLAUDE.md)",
				"paradigm    Universal CPI-SI wisdom").
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
	case "levels":
		printLevels(cfg)

	case "questions":
		printQuestions(cfg)

	case "process":
		printProcess(cfg)

	case "template":
		if len(args) < 2 {
			cli.Fatal("Usage: integrate template <level>\nLevels: %s", strings.Join(cfg.GetLevelNames(), ", "))
		}
		printTemplate(cfg, args[1])

	default:
		cli.Fatal("Unknown command: %s\nUse -h for help", cmd)
	}
}
