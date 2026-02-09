// ============================================================================
// METADATA
// ============================================================================
//
// Key: propagate-change-cmd-propagate
// Purpose: CLI orchestrator for propagate-change skill
// Biblical: Galatians 5:9 - "A little leaven leaveneth the whole lump"
// Authors: Nova Dawn
// Version: 3.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// This is the ORCHESTRATOR - CLI parsing only.
// All logic lives in pkg/chain/ and pkg/config/.
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"
	"strings"

	"cws.studio/pkg/util/pure/cli"
	"cws.studio/pkg/util/pure/output"
	"cws.studio/skills/propagate-change/scripts/pkg/chain"
	"cws.studio/skills/propagate-change/scripts/pkg/config"
)

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// printChain outputs the chain trace in a readable format
func printChain(result chain.Result, showJSON bool) {
	if showJSON {
		printChainJSON(result)
		return
	}

	output.Header("Template Chain: " + result.File)
	output.KeyValue("Level", result.Level)

	if len(result.Chain) > 0 {
		output.Section("Chain (derives_from ancestry)")
		for _, link := range result.Chain {
			indent := strings.Repeat("  ", link.Level)
			output.Indent(indent+"["+string(rune('0'+link.Level))+"] "+link.Path+" ("+link.Type+")", 0)
			if link.DerivesFrom != "" {
				output.Indent(indent+"    -> derives from: "+link.DerivesFrom, 0)
			}
		}
	}

	if len(result.Descendants) > 0 {
		output.Section("Descendants (files that derive from this)")
		for _, desc := range result.Descendants {
			output.Bullet(desc)
		}
	}

	if result.Message != "" {
		output.Info("Notes: %s", result.Message)
	}
}

// printChainJSON outputs the result as JSON
func printChainJSON(result chain.Result) {
	output.JSONStart()
	output.JSONField("file", result.File)
	output.JSONField("level", result.Level)
	// Chain array - manual since nested
	output.Indent("\"chain\": [", 0)
	for i, link := range result.Chain {
		comma := ","
		if i == len(result.Chain)-1 {
			comma = ""
		}
		output.Indent("    {\"path\": \""+link.Path+"\", \"derives_from\": \""+link.DerivesFrom+"\", \"level\": "+string(rune('0'+link.Level))+", \"type\": \""+link.Type+"\"}"+comma, 0)
	}
	output.Indent("  ],", 0)
	// Descendants array
	output.Indent("\"descendants\": [", 0)
	for i, desc := range result.Descendants {
		comma := ","
		if i == len(result.Descendants)-1 {
			comma = ""
		}
		output.Indent("    \""+desc+"\""+comma, 0)
	}
	output.Indent("  ],", 0)
	output.JSONFieldLast("message", result.Message)
	output.JSONEnd()
}

// --- Core Operations ---

// runTrace executes the trace command
func runTrace(file string, cfg *config.Config, showJSON bool) error {
	result := chain.TraceUp(file, cfg)
	printChain(result, showJSON)
	return nil
}

// runDescendants executes the descendants command
func runDescendants(file string, cfg *config.Config, showJSON bool) error {
	searchDir := "."
	if len(cfg.Locations.SearchPaths) > 0 {
		searchDir = cfg.Locations.SearchPaths[0]
	}
	result := chain.FindDescendants(file, searchDir, cfg)
	printChain(result, showJSON)
	return nil
}

// runReport executes the full report command
func runReport(file string, cfg *config.Config, showJSON bool) error {
	result := chain.GenerateReport(file, cfg)
	printChain(result, showJSON)
	return nil
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("propagate", "Trace OmniCode template chains and find affected files").
		WithBiblical("Galatians 5:9 - \"A little leaven leaveneth the whole lump\"").
		WithJSON()

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help
	if app.WantsHelp() {
		usage := cli.NewUsage(app).
			AddSection("Commands",
				"trace <file>      Trace derives_from chain up to root",
				"descendants <file> Find files that derive from this template",
				"report <file>     Full propagation report (chain + descendants)").
			AddSection("Options",
				"-json             Output as JSON",
				"-h, -help         Show this help").
			AddSection("Examples",
				"propagate trace my-template.omni",
				"propagate descendants universal.omni",
				"propagate report changed-file.omni -json")
		usage.Print()
		os.Exit(0)
	}

	// Get command and file
	args := app.Args()
	if len(args) < 1 {
		cli.Fatal("No file specified. Use -h for help.")
	}

	// Load configuration
	cfg, err := config.Load()
	if err != nil {
		cli.Fatal("Error loading config: %v", err)
	}

	// Determine command
	command := args[0]
	var file string

	// If first arg looks like a file, default to report
	if strings.Contains(command, ".") || strings.Contains(command, "/") {
		file = command
		command = "report"
	} else if len(args) < 2 {
		cli.Fatal("Error: %s command requires a file argument", command)
	} else {
		file = args[1]
	}

	// Execute command
	switch command {
	case "trace":
		err = runTrace(file, cfg, app.WantsJSON())
	case "descendants":
		err = runDescendants(file, cfg, app.WantsJSON())
	case "report":
		err = runReport(file, cfg, app.WantsJSON())
	default:
		cli.Fatal("Unknown command: %s\nUse -h for help", command)
	}

	if err != nil {
		cli.Fatal("Error: %v", err)
	}
}
