// ============================================================================
// METADATA
// ============================================================================
//
// Key: format-lookup-cmd-lookup
// Purpose: OmniCode format mapping lookup orchestrator - CLI interface
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 3.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// Config-driven: Reads format mappings from config/format-mappings.toml
// Orchestrator: CLI handling only - logic in pkg/mappings
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"
	"sort"

	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/cli"
	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/output"
	"github.com/creativeworkzstudio/claude-global/skills/format-lookup/scripts/pkg/config"
	"github.com/creativeworkzstudio/claude-global/skills/format-lookup/scripts/pkg/mappings"
)

// ============================================================================
// BODY
// ============================================================================

// --- Output Helpers ---

// printMapping outputs a single mapping using shared output
func printMapping(m config.Mapping) {
	output.KeyValueWidth("OmniCode", m.OmniCode, 10)
	output.KeyValueWidth("Target", m.Target, 10)
	output.KeyValueWidth("Example", m.Example, 10)
	output.Divider("", 0) // Empty line
}

// printResult outputs lookup results
func printResult(result mappings.Result, cfg *config.Config) {
	if !result.Found {
		cli.Error(result.Message)
		if result.Format != "" {
			output.Tip("Use 'lookup --format %s' to see all mappings", result.Format)
		}
		output.Info("See: config/format-mappings.toml")
		return
	}

	if result.Element != "" {
		output.Header("Mappings for '" + result.Element + "' → " + result.Format)
	} else {
		info, _ := cfg.GetFormat(result.Format)
		output.Header("OmniCode → " + info.Name + " (" + result.Format + ") Mappings")
	}

	for _, m := range result.Mappings {
		printMapping(m)
	}

	if result.Element == "" {
		output.Info("Total: %d mappings", len(result.Mappings))
		output.Info("Configuration: config/format-mappings.toml")
	}
}

// --- Commands ---

// listFormats prints all available formats grouped by type
func listFormats(cfg *config.Config) {
	output.Header("OmniCode Target Formats (config-driven)")

	types := []string{"code", "documentation", "data"}

	for _, t := range types {
		formats := cfg.ListFormatsByType(t)
		if len(formats) == 0 {
			continue
		}

		output.SubSection(t + " formats")
		output.TableHeader("Format", "Flag", "Description", "Status")

		// Sort for consistent output
		sort.Slice(formats, func(i, j int) bool {
			return formats[i].Name < formats[j].Name
		})

		for _, f := range formats {
			output.TableRow([]int{12, 8, 30, 10}, f.Name, f.Flag, f.Description, f.Status)
		}
	}

	output.Section("Usage")
	output.Bullet("#!omni code --go")
	output.Bullet("#!omni documentation --adoc")
	output.Bullet("#!omni data --json")

	output.Info("Configuration: config/format-mappings.toml")
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app with standard flags
	app := cli.New("lookup", "OmniCode format mapping lookup (config-driven)").
		WithBiblical("Ecclesiastes 3:1 - \"To every thing there is a season\"")

	// Custom flags
	elementFlag := app.Flag().String("element", "", "OmniCode element to look up")
	formatFlag := app.Flag().String("format", "", "Target format (go, adoc, md, c, rs)")
	listFlag := app.Flag().Bool("list", false, "List all available formats")

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help
	if app.WantsHelp() {
		usage := cli.NewUsage(app).
			AddSection("Usage",
				"lookup --element <text> --format <fmt>  Find mappings",
				"lookup --format <fmt>                   List all mappings for format",
				"lookup --list                           List all formats").
			AddSection("Examples",
				"lookup --element 'grounded in' --format go",
				"lookup --element 'definitions' --format c",
				"lookup --format adoc",
				"lookup --list").
			AddSection("Configuration",
				"config/format-mappings.toml   All format mappings").
			AddSection("Options",
				"--element <text>   OmniCode element to look up",
				"--format <fmt>     Target format (go, adoc, md, c, rs)",
				"--list             List all available formats",
				"-h, --help         Show this help")
		usage.Print()
		os.Exit(0)
	}

	// Load configuration
	cfg, err := config.Load()
	if err != nil {
		cli.Fatal("Error loading config: %v\nMake sure config/format-mappings.toml exists", err)
	}

	// Command: List all formats
	if *listFlag {
		listFormats(cfg)
		os.Exit(0)
	}

	// Command: List all mappings for a format
	if *formatFlag != "" && *elementFlag == "" {
		result := mappings.ListAll(*formatFlag, cfg)
		printResult(result, cfg)
		if !result.Found {
			os.Exit(1)
		}
		os.Exit(0)
	}

	// Command: Lookup specific element
	if *elementFlag == "" || *formatFlag == "" {
		cli.Error("Both --element and --format are required for lookup")
		cli.Error("Use -h for help")
		os.Exit(1)
	}

	result := mappings.Lookup(*elementFlag, *formatFlag, cfg)
	printResult(result, cfg)
	if !result.Found {
		os.Exit(1)
	}
}
