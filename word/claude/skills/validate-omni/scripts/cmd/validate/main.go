// ============================================================================
// METADATA
// ============================================================================
//
// Key: validate-omni-cmd-validate
// Purpose: OmniCode validation orchestrator - coordinates pragma, block, and section checks
// Biblical: 1 Thessalonians 5:21 - "Prove all things; hold fast that which is good"
// Authors: Nova Dawn
// Version: 3.0.0
// Created: 2025-12-08
// Updated: 2025-12-10 - Refactored to use shared pkg/cli and pkg/output
//
// Config-driven: Reads validation rules from config/*.toml files
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"bufio"
	"os"
	"strings"

	"cws.studio/pkg/util/pure/cli"
	"cws.studio/pkg/util/pure/output"
	"cws.studio/skills/validate-omni/scripts/pkg/blocks"
	"cws.studio/skills/validate-omni/scripts/pkg/config"
	"cws.studio/skills/validate-omni/scripts/pkg/pragma"
	"cws.studio/skills/validate-omni/scripts/pkg/sections"
)

// ValidationResult holds the complete validation state
type ValidationResult struct {
	Filename        string
	Status          string // VALID, WARNINGS, INVALID
	Pragma          pragma.Result
	Blocks          blocks.Result
	Sections        sections.Result
	DerivesFrom     string
	TemplateExists  bool
	Issues          []string
	Recommendations []string
}

// ============================================================================
// BODY
// ============================================================================

// readFile loads file contents into lines and full string
func readFile(filename string) ([]string, string, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, "", err
	}
	defer file.Close()

	var lines []string
	var content strings.Builder
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		lines = append(lines, line)
		content.WriteString(line)
		content.WriteString("\n")
	}

	if err := scanner.Err(); err != nil {
		return nil, "", err
	}

	return lines, content.String(), nil
}

// findDerivesFrom extracts the derives_from reference
func findDerivesFrom(content string) string {
	for _, line := range strings.Split(content, "\n") {
		if strings.Contains(line, "derives from:") || strings.Contains(line, "derives_from:") {
			parts := strings.SplitN(line, ":", 2)
			if len(parts) > 1 {
				return strings.TrimSpace(strings.Trim(parts[1], "\"'"))
			}
		}
	}
	return ""
}

// validate performs full validation on the file
func validate(filename string, cfg *config.Config) (*ValidationResult, error) {
	result := &ValidationResult{
		Filename: filename,
		Status:   "VALID",
	}

	// Read file
	lines, content, err := readFile(filename)
	if err != nil {
		return nil, err
	}

	// Check pragma
	result.Pragma = pragma.Check(filename, cfg)
	if !result.Pragma.Valid {
		result.Issues = append(result.Issues, result.Pragma.Message)
		result.Recommendations = append(result.Recommendations, "See config/pragma-types.toml for valid pragma syntax")
		result.Status = "INVALID"
	}

	// Check blocks
	result.Blocks = blocks.Check(lines, result.Pragma.Type, cfg)
	if !result.Blocks.Valid {
		result.Issues = append(result.Issues, result.Blocks.Message)
		if len(result.Blocks.Missing) > 0 {
			result.Issues = append(result.Issues, "Missing blocks: "+strings.Join(result.Blocks.Missing, ", "))
		}
		result.Recommendations = append(result.Recommendations, "See config/block-structures.toml for block patterns")
		if result.Status == "VALID" {
			result.Status = "WARNINGS"
		}
	}

	// Check required sections
	result.Sections = sections.Check(content, cfg)
	if !result.Sections.Valid {
		result.Issues = append(result.Issues, result.Sections.Message)
		for _, missing := range result.Sections.MissingList(cfg) {
			result.Issues = append(result.Issues, "Missing: "+missing)
		}
		result.Recommendations = append(result.Recommendations, "See config/required-sections.toml for required METADATA sections")
		if result.Status == "VALID" {
			result.Status = "WARNINGS"
		}
	}

	// Check derives_from
	result.DerivesFrom = findDerivesFrom(content)
	if result.DerivesFrom != "" {
		if _, err := os.Stat(result.DerivesFrom); err == nil {
			result.TemplateExists = true
		} else {
			result.Issues = append(result.Issues, "Template '"+result.DerivesFrom+"' not found")
			result.Recommendations = append(result.Recommendations, "Verify derives_from path is correct")
		}
	}

	return result, nil
}

// printResult outputs the validation result
func printResult(r *ValidationResult, cfg *config.Config) {
	output.Header("OmniCode Validation: " + r.Filename)
	output.KeyValue("Status", r.Status)

	// Pragma
	output.Section("Pragma")
	if r.Pragma.Type != "" {
		output.KeyValue("Type", r.Pragma.Type)
		format := r.Pragma.Format
		if format == "" {
			format = "pure"
		}
		output.KeyValue("Format", format)
		output.KeyValue("Line", r.Pragma.Line)
	} else {
		output.KeyValue("Type", "MISSING")
		output.Info("See: config/pragma-types.toml")
	}

	// Blocks
	output.Section("Block Structure")
	output.KeyValue("Expected", len(r.Blocks.Expected))
	output.KeyValue("Found", strings.Join(r.Blocks.Found, ", "))
	if len(r.Blocks.Missing) > 0 {
		output.KeyValue("Missing", strings.Join(r.Blocks.Missing, ", "))
		output.Info("See: config/block-structures.toml")
	} else {
		output.Success("Complete")
	}

	// Required Sections (dynamic from config)
	output.Section("Required Sections")
	for name, section := range cfg.RequiredSections.Sections {
		if section.Required {
			found := r.Sections.Found[name]
			markers := strings.Join(section.Markers, "/")
			status := "[ ]"
			if found {
				status = "[x]"
			}
			output.Indent(status+" "+section.Name+" ("+markers+")", 0)
		}
	}
	if !r.Sections.Valid {
		output.Info("See: config/required-sections.toml")
	}

	// Template Chain
	output.Section("Template Chain")
	if r.DerivesFrom != "" {
		output.KeyValue("derives_from", r.DerivesFrom)
		if r.TemplateExists {
			output.KeyValue("Template exists", "Yes")
		} else {
			output.KeyValue("Template exists", "No")
		}
	} else {
		output.KeyValue("derives_from", "original")
	}

	// Issues
	if len(r.Issues) > 0 {
		output.Section("Issues Found")
		for i, issue := range r.Issues {
			output.Numbered(i+1, issue)
		}
	}

	// Recommendations
	if len(r.Recommendations) > 0 {
		output.Section("Recommendations")
		for _, rec := range r.Recommendations {
			output.Bullet(rec)
		}
	}
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup CLI app
	app := cli.New("validate", "OmniCode file validator (config-driven)").
		WithBiblical("1 Thessalonians 5:21 - \"Prove all things; hold fast that which is good\"")

	// Parse
	if err := app.ParseOS(); err != nil {
		cli.Fatal("Parse error: %v", err)
	}

	// Help or no args
	if app.WantsHelp() || len(app.Args()) == 0 {
		usage := cli.NewUsage(app).
			AddSection("Usage",
				"validate <file.omni>           Full validation",
				"validate pragma <file.omni>    Pragma check only",
				"validate blocks <file.omni>    Block structure check only",
				"validate sections <file.omni>  Required sections check only").
			AddSection("Configuration files",
				"config/pragma-types.toml       Valid pragma types and formats",
				"config/block-structures.toml   Block patterns by type",
				"config/required-sections.toml  Required METADATA sections").
			AddSection("Exit codes",
				"0 - VALID",
				"1 - WARNINGS (structural issues)",
				"2 - INVALID (missing pragma or critical issues)")
		usage.Print()
		os.Exit(0)
	}

	// Load configuration
	cfg, err := config.Load()
	if err != nil {
		cli.Fatal("Error loading config: %v\nMake sure config/*.toml files exist", err)
	}

	// Handle subcommands
	args := app.Args()
	arg := args[0]

	switch arg {
	case "pragma":
		if len(args) < 2 {
			cli.Fatal("Usage: validate pragma <file.omni>")
		}
		result := pragma.Check(args[1], cfg)
		if result.Valid {
			output.Success("VALID pragma on line %d", result.Line)
			output.KeyValue("Type", result.Type)
			if result.Format != "" {
				output.KeyValue("Format", result.Format)
			} else {
				output.KeyValue("Format", "pure (no target)")
			}
			os.Exit(0)
		} else {
			cli.Error(result.Message)
			os.Exit(1)
		}

	case "blocks":
		if len(args) < 2 {
			cli.Fatal("Usage: validate blocks <file.omni>")
		}
		lines, _, err := readFile(args[1])
		if err != nil {
			cli.Fatal("Error: %v", err)
		}
		pragmaType, _ := pragma.ExtractFromLines(lines)
		result := blocks.Check(lines, pragmaType, cfg)
		output.Header("Block Structure: " + pragmaType)
		output.KeyValue("Expected", strings.Join(result.Expected, ", "))
		output.KeyValue("Found", strings.Join(result.Found, ", "))
		if len(result.Missing) > 0 {
			output.KeyValue("MISSING", strings.Join(result.Missing, ", "))
			output.Info("See: config/block-structures.toml")
		}
		if len(result.Extra) > 0 {
			output.KeyValue("EXTRA (non-standard)", strings.Join(result.Extra, ", "))
		}
		if result.Valid {
			output.Success("COMPLETE")
			os.Exit(0)
		} else {
			cli.Error("INCOMPLETE")
			os.Exit(1)
		}

	case "sections":
		if len(args) < 2 {
			cli.Fatal("Usage: validate sections <file.omni>")
		}
		_, content, err := readFile(args[1])
		if err != nil {
			cli.Fatal("Error: %v", err)
		}
		result := sections.Check(content, cfg)
		output.Header("Required Sections")
		for name, section := range cfg.RequiredSections.Sections {
			if section.Required {
				found := result.Found[name]
				markers := strings.Join(section.Markers, "/")
				status := "[ ]"
				if found {
					status = "[x]"
				}
				output.Indent(status+" "+section.Name+" ("+markers+")", 0)
			}
		}
		output.Info("Found: %d/%d sections", result.Count, result.Total)
		if result.Valid {
			output.Success("COMPLETE")
			os.Exit(0)
		} else {
			cli.Error("INCOMPLETE")
			output.Info("See: config/required-sections.toml")
			os.Exit(1)
		}

	default:
		// Assume it's a filename - full validation
		result, err := validate(arg, cfg)
		if err != nil {
			cli.Fatal("Error: %v", err)
		}

		printResult(result, cfg)

		switch result.Status {
		case "VALID":
			os.Exit(0)
		case "WARNINGS":
			os.Exit(1)
		default:
			os.Exit(2)
		}
	}
}
