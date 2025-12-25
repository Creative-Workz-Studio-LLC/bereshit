// ============================================================================
// METADATA
// ============================================================================
//
// Key: create-from-template-cmd-template
// Purpose: CLI orchestrator for create-from-template skill
// Biblical: Exodus 25:40 - "Look that thou make them after their pattern"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// This is the ORCHESTRATOR - CLI parsing only.
// All logic lives in pkg/config/ and pkg/discovery/.
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"io"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/cli"
	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/output"
	"github.com/creativeworkzstudio/claude-global/skills/create-from-template/scripts/pkg/config"
	"github.com/creativeworkzstudio/claude-global/skills/create-from-template/scripts/pkg/discovery"
)

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// printTemplateList outputs template list
func printTemplateList(templates []discovery.Template, category string, showJSON bool) {
	// Filter by category if specified
	var filtered []discovery.Template
	for _, t := range templates {
		if category == "" || t.Category == category {
			filtered = append(filtered, t)
		}
	}

	// Sort by category, then language, then name
	sort.Slice(filtered, func(i, j int) bool {
		if filtered[i].Category != filtered[j].Category {
			return filtered[i].Category < filtered[j].Category
		}
		if filtered[i].Language != filtered[j].Language {
			return filtered[i].Language < filtered[j].Language
		}
		return filtered[i].Name < filtered[j].Name
	})

	if showJSON {
		printTemplateListJSON(filtered)
		return
	}

	output.Header("Available Templates")

	currentCat := ""
	currentLang := ""
	for _, t := range filtered {
		if t.Category != currentCat {
			currentCat = t.Category
			currentLang = ""
			output.SubSection(strings.ToUpper(t.Category) + " (" + t.Block + ")")
		}
		if t.Language != currentLang {
			currentLang = t.Language
			output.Indent(t.Language+"/", 1)
		}
		output.Indent("- "+t.Name+" ["+t.Type+"]", 2)
	}

	output.Info("Total: %d templates", len(filtered))
}

// printTemplateListJSON outputs JSON format
func printTemplateListJSON(templates []discovery.Template) {
	output.Indent("[", 0)
	for i, t := range templates {
		comma := ","
		if i == len(templates)-1 {
			comma = ""
		}
		output.Indent("  {\"name\": \""+t.Name+"\", \"category\": \""+t.Category+"\", \"language\": \""+t.Language+"\", \"type\": \""+t.Type+"\", \"block\": \""+t.Block+"\", \"path\": \""+t.Path+"\"}"+comma, 0)
	}
	output.Indent("]", 0)
}

// showTemplate displays template content
func showTemplate(cfg *config.Config, language, templateType string) error {
	// Try to find in code category first
	t, err := discovery.FindTemplate(cfg, "code", language, templateType)
	if err != nil {
		// Try data
		t, err = discovery.FindTemplate(cfg, "data", language, templateType)
		if err != nil {
			// Try documentation
			t, err = discovery.FindTemplate(cfg, "documentation", language, templateType)
			if err != nil {
				return err
			}
		}
	}

	content, err := os.ReadFile(t.Path)
	if err != nil {
		return err
	}

	output.Header("Template: " + t.Language + "/" + t.Name + " (" + t.Block + ")")
	output.KeyValue("Path", t.Path)
	output.Divider("-", 60)
	output.Indent(string(content), 0)

	return nil
}

// createFromTemplate copies template to destination
func createFromTemplate(cfg *config.Config, language, templateType, dest string) error {
	// Find template
	t, err := discovery.FindTemplate(cfg, "code", language, templateType)
	if err != nil {
		t, err = discovery.FindTemplate(cfg, "data", language, templateType)
		if err != nil {
			t, err = discovery.FindTemplate(cfg, "documentation", language, templateType)
			if err != nil {
				return err
			}
		}
	}

	// Check destination doesn't exist
	if _, err := os.Stat(dest); err == nil {
		cli.Error("Destination already exists: %s", dest)
		return nil
	}

	// Create parent directory
	if err := os.MkdirAll(filepath.Dir(dest), 0755); err != nil {
		return err
	}

	// Copy file
	src, err := os.Open(t.Path)
	if err != nil {
		return err
	}
	defer src.Close()

	dst, err := os.Create(dest)
	if err != nil {
		return err
	}
	defer dst.Close()

	if _, err := io.Copy(dst, src); err != nil {
		return err
	}

	output.Success("Created %s from %s/%s template", dest, t.Language, t.Name)
	output.Section("Next steps")
	output.Numbered(1, "Update METADATA block with file-specific values")
	output.Numbered(2, "Fill in SETUP/BODY/CLOSING sections")
	output.Numbered(3, "Run validate-omni to verify structure")

	return nil
}

// --- Core Operations ---

// runList handles the list command
func runList(cfg *config.Config, category string, showJSON bool) error {
	templates, err := discovery.ListTemplates(cfg)
	if err != nil {
		return err
	}

	printTemplateList(templates, category, showJSON)
	return nil
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("template", "Discover and create files from templates").
		WithBiblical("Exodus 25:40 - \"Look that thou make them after their pattern\"").
		WithJSON()

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help or no args
	if app.WantsHelp() || len(app.Args()) == 0 {
		usage := cli.NewUsage(app).
			AddSection("Commands",
				"list                      List all available templates",
				"list <category>           List templates in category (code, data, documentation)",
				"show <lang> [type]        Show template content",
				"create <lang> <type> <dest>  Copy template to destination",
				"categories                List available categories",
				"languages <category>      List languages/formats in category").
			AddSection("Options",
				"-json         Output as JSON",
				"-h, -help     Show this help").
			AddSection("Examples",
				"template list                    # List all templates",
				"template list code               # List code templates",
				"template show go executable      # Show Go executable template",
				"template create go library pkg/foo/foo.go").
			AddSection("Template locations",
				"bereshit/word/seed/")
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
	case "list":
		category := ""
		if len(args) > 1 {
			category = args[1]
		}
		if err := runList(cfg, category, app.WantsJSON()); err != nil {
			cli.Fatal("Error: %v", err)
		}

	case "show":
		if len(args) < 2 {
			cli.Fatal("Usage: template show <language> [type]")
		}
		language := args[1]
		templateType := ""
		if len(args) > 2 {
			templateType = args[2]
		}
		if err := showTemplate(cfg, language, templateType); err != nil {
			cli.Fatal("Error: %v", err)
		}

	case "create":
		if len(args) < 4 {
			cli.Fatal("Usage: template create <language> <type> <destination>")
		}
		if err := createFromTemplate(cfg, args[1], args[2], args[3]); err != nil {
			cli.Fatal("Error: %v", err)
		}

	case "categories":
		cats := discovery.GetCategories(cfg)
		for _, c := range cats {
			output.Indent(c, 0)
		}

	case "languages":
		if len(args) < 2 {
			cli.Fatal("Usage: template languages <category>")
		}
		langs := discovery.GetLanguages(cfg, args[1])
		for _, l := range langs {
			output.Indent(l, 0)
		}

	default:
		cli.Fatal("Unknown command: %s\nUse -h for help", cmd)
	}
}
