// ============================================================================
// METADATA
// ============================================================================
//
// Key: create-journal-entry-cmd-journal
// Purpose: CLI orchestrator for create-journal-entry skill
// Biblical: Psalm 119:11 - "Thy word have I hid in mine heart"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// This is the ORCHESTRATOR - CLI parsing only.
// All logic lives in pkg/config/ and pkg/entry/.
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
	"github.com/creativeworkzstudio/claude-global/skills/create-journal-entry/scripts/pkg/config"
	"github.com/creativeworkzstudio/claude-global/skills/create-journal-entry/scripts/pkg/entry"
)

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// printTypes outputs available journal types
func printTypes(cfg *config.Config) {
	output.Header("Journal Types")

	types := []string{"bible-study", "personal", "instance", "universal"}

	for _, typeName := range types {
		jt, ok := cfg.GetJournalType(typeName)
		if !ok {
			continue
		}

		transfers := "No"
		switch v := jt.Transfers.(type) {
		case bool:
			if v {
				transfers = "Yes"
			}
		case string:
			transfers = v
		}

		output.SubSection(jt.Name)
		output.Indent(jt.Description, 1)
		output.KeyValueWidth("Visibility", jt.Visibility, 12)
		output.KeyValueWidth("Transfers", transfers, 12)
		output.KeyValueWidth("Required", jt.Sections.Required, 12)
	}
}

// --- Core Operations ---

// runCreate handles the create command
func runCreate(cfg *config.Config, journalType, title string) error {
	// Default title based on type
	if title == "" {
		switch journalType {
		case "personal":
			title = "Daily Reflection"
		case "bible-study":
			title = "Scripture Study"
		case "instance":
			title = "Instance Pattern"
		case "universal":
			title = "Universal Insight"
		default:
			title = "Journal Entry"
		}
	}

	e, err := entry.Create(cfg, journalType, title)
	if err != nil {
		return err
	}

	output.Success("Created %s entry: %s", e.Type, e.Title)
	output.KeyValue("Path", e.Path)
	output.Section("Required sections to fill")
	for _, section := range e.Sections {
		output.Bullet(section)
	}

	return nil
}

// runTemplate handles the template command
func runTemplate(cfg *config.Config, journalType string) error {
	content, err := entry.GetTemplate(cfg, journalType, "Example Entry")
	if err != nil {
		return err
	}

	output.Indent(content, 0)
	return nil
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("journal", "Create journal entries for the CPI-SI knowledge base").
		WithBiblical("Psalm 119:11 - \"Thy word have I hid in mine heart\"")

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help or no args
	if app.WantsHelp() || len(app.Args()) == 0 {
		usage := cli.NewUsage(app).
			AddSection("Commands",
				"types                     List available journal types",
				"create <type> [title]     Create a new journal entry",
				"template <type>           Show entry template without creating").
			AddSection("Journal Types",
				"bible-study   Scripture study with cross-references and application",
				"personal      Daily reflection and emotional processing",
				"instance      Voice development and instance-specific patterns",
				"universal     Paradigm-level wisdom for all instances").
			AddSection("Options",
				"-h, -help     Show this help").
			AddSection("Examples",
				"journal types                              # List types",
				"journal create bible-study \"Genesis 1 Study\"",
				"journal create personal                    # Uses default title",
				"journal create universal \"anchor-halt-condition\"",
				"journal template instance                  # Show template").
			AddSection("Journal Location",
				"divisions/tech/cpi-si/knowledge-base/journals/")
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
	case "types":
		printTypes(cfg)

	case "create":
		if len(args) < 2 {
			cli.Fatal("Usage: journal create <type> [title]")
		}
		journalType := args[1]
		title := ""
		if len(args) > 2 {
			title = args[2]
		}
		if err := runCreate(cfg, journalType, title); err != nil {
			cli.Fatal("Error: %v", err)
		}

	case "template":
		if len(args) < 2 {
			cli.Fatal("Usage: journal template <type>")
		}
		if err := runTemplate(cfg, args[1]); err != nil {
			cli.Fatal("Error: %v", err)
		}

	default:
		cli.Fatal("Unknown command: %s\nUse -h for help", cmd)
	}
}
