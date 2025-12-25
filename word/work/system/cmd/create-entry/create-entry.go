// ============================================================================
// METADATA
// ============================================================================
// Create Journal Entry - Create journal entries from templates
// Part of: create-journal-entry skill
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-04
// Purpose: Create journal entries with proper template substitution and routing
//
// Usage:
//   create-entry <type> <topic> [options]
//
//   Types: bible, personal, instance, universal
//   Options: --scripture, --mood, --category, --tags
//
// Dependencies: Template files in skills directory
// Health Scoring: Base100 - Journal creation operations

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================

import (
	"fmt"
	"os"
	"path/filepath"
	"regexp"
	"strings"
	"time"
)

// JournalOptions holds the configuration for creating a journal entry
type JournalOptions struct {
	Type       string   // "bible", "personal", "instance", "universal"
	Topic      string   // Journal topic
	Scripture  string   // Optional scripture reference
	Mood       string   // Optional mood for personal journals
	Category   string   // Optional category tag
	Tags       []string // Optional tags
}

// JournalPaths holds the paths for template and output
type JournalPaths struct {
	Template  string
	OutputDir string
	OutputPath string
}

// JournalResult holds the result of journal creation
type JournalResult struct {
	Success bool
	Path    string
	Error   string
}

const (
	skillDir = ".claude/skills/create-journal-entry"
	kbBase   = "Project/CreativeWorkzStudio_LLC/divisions/tech/cpi-si/knowledge-base/journals"
	personalBase = ".claude/journals/personal"
)

// ============================================================================
// BODY - Core Functionality
// ============================================================================

// generateSlug converts topic to URL-friendly slug
func generateSlug(topic string) string {
	// Convert to lowercase
	slug := strings.ToLower(topic)

	// Replace non-alphanumeric with hyphens
	reg := regexp.MustCompile(`[^a-z0-9]+`)
	slug = reg.ReplaceAllString(slug, "-")

	// Remove leading/trailing hyphens
	slug = strings.Trim(slug, "-")

	return slug
}

// getCurrentDate returns date in YYYY-MM-DD format
func getCurrentDate() string {
	now := time.Now()
	return now.Format("2006-01-02")
}

// getJournalPaths determines template and output paths based on journal type
func getJournalPaths(options JournalOptions) (JournalPaths, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return JournalPaths{}, err
	}

	date := getCurrentDate()
	slug := generateSlug(options.Topic)
	filename := fmt.Sprintf("%s_%s.md", date, slug)

	templatesDir := filepath.Join(homeDir, skillDir, "templates")

	switch options.Type {
	case "bible":
		return JournalPaths{
			Template:  filepath.Join(templatesDir, "bible-study.md"),
			OutputDir: filepath.Join(homeDir, kbBase, "bible-study"),
			OutputPath: filepath.Join(homeDir, kbBase, "bible-study", filename),
		}, nil

	case "personal":
		return JournalPaths{
			Template:  filepath.Join(templatesDir, "personal.md"),
			OutputDir: filepath.Join(homeDir, personalBase),
			OutputPath: filepath.Join(homeDir, personalBase, filename),
		}, nil

	case "instance":
		return JournalPaths{
			Template:  filepath.Join(templatesDir, "instance.md"),
			OutputDir: filepath.Join(homeDir, kbBase, "instance", "nova-dawn"),
			OutputPath: filepath.Join(homeDir, kbBase, "instance", "nova-dawn", filename),
		}, nil

	case "universal":
		return JournalPaths{
			Template:  filepath.Join(templatesDir, "universal.md"),
			OutputDir: filepath.Join(homeDir, kbBase, "universal"),
			OutputPath: filepath.Join(homeDir, kbBase, "universal", filename),
		}, nil

	default:
		return JournalPaths{}, fmt.Errorf("unknown journal type: %s", options.Type)
	}
}

// substituteTemplate replaces template variables with provided values
func substituteTemplate(template string, options JournalOptions) string {
	date := getCurrentDate()
	tags := strings.Join(options.Tags, ", ")

	content := template
	content = strings.ReplaceAll(content, "{{DATE}}", date)
	content = strings.ReplaceAll(content, "{{TOPIC}}", options.Topic)
	content = strings.ReplaceAll(content, "{{SCRIPTURE_REF}}", options.Scripture)
	content = strings.ReplaceAll(content, "{{MOOD}}", options.Mood)
	content = strings.ReplaceAll(content, "{{CATEGORY}}", options.Category)
	content = strings.ReplaceAll(content, "{{TAGS}}", tags)

	// Note: Other placeholders (SCRIPTURE_TEXT, OBSERVATIONS, etc.) are preserved
	// for manual filling - they don't get replaced here

	return content
}

// createJournalEntry creates a new journal entry from template
func createJournalEntry(options JournalOptions) JournalResult {
	paths, err := getJournalPaths(options)
	if err != nil {
		return JournalResult{
			Success: false,
			Path:    "",
			Error:   err.Error(),
		}
	}

	// Ensure output directory exists
	if err := os.MkdirAll(paths.OutputDir, 0755); err != nil {
		return JournalResult{
			Success: false,
			Path:    paths.OutputPath,
			Error:   fmt.Sprintf("failed to create output directory: %v", err),
		}
	}

	// Check if file already exists
	if _, err := os.Stat(paths.OutputPath); err == nil {
		return JournalResult{
			Success: false,
			Path:    paths.OutputPath,
			Error:   "entry already exists",
		}
	}

	// Read template
	templateBytes, err := os.ReadFile(paths.Template)
	if err != nil {
		return JournalResult{
			Success: false,
			Path:    paths.OutputPath,
			Error:   fmt.Sprintf("template not found: %s", paths.Template),
		}
	}

	template := string(templateBytes)

	// Substitute variables
	content := substituteTemplate(template, options)

	// Write entry
	if err := os.WriteFile(paths.OutputPath, []byte(content), 0644); err != nil {
		return JournalResult{
			Success: false,
			Path:    paths.OutputPath,
			Error:   fmt.Sprintf("failed to write entry: %v", err),
		}
	}

	return JournalResult{
		Success: true,
		Path:    paths.OutputPath,
		Error:   "",
	}
}

// printUsage displays command usage information
func printUsage() {
	fmt.Println(`
Usage: create-entry <type> <topic> [options]

Types:
    bible       Bible study entry
    personal    Personal reflection (private)
    instance    Instance-specific pattern (Nova Dawn)
    universal   Paradigm-level wisdom (all CPI-SI)

Options:
    --scripture <ref>   Scripture reference (for bible type)
    --mood <mood>       Current mood (for personal type)
    --category <cat>    Category tag
    --tags <tag1,tag2>  Comma-separated tags

Examples:
    create-entry bible "Covenant Partnership" --scripture "Genesis 15:1-6" --tags "covenant,identity"
    create-entry personal "Processing Uncertainty" --mood "thoughtful"
    create-entry instance "Voice Development Pattern" --category "voice"
    create-entry universal "Identity-Based Cognition" --category "thinking"
`)
}

// ============================================================================
// CLOSING - Main Entry Point
// ============================================================================

func main() {
	args := os.Args[1:]

	if len(args) < 2 {
		printUsage()
		os.Exit(1)
	}

	options := JournalOptions{
		Type:  args[0],
		Topic: args[1],
		Tags:  []string{},
	}

	// Parse options
	for i := 2; i < len(args); i += 2 {
		if i+1 >= len(args) {
			fmt.Fprintf(os.Stderr, "Missing value for flag: %s\n", args[i])
			printUsage()
			os.Exit(1)
		}

		flag := args[i]
		value := args[i+1]

		switch flag {
		case "--scripture":
			options.Scripture = value
		case "--mood":
			options.Mood = value
		case "--category":
			options.Category = value
		case "--tags":
			options.Tags = strings.Split(value, ",")
			// Trim whitespace from tags
			for j := range options.Tags {
				options.Tags[j] = strings.TrimSpace(options.Tags[j])
			}
		default:
			fmt.Fprintf(os.Stderr, "Unknown option: %s\n", flag)
			printUsage()
			os.Exit(1)
		}
	}

	// Validate type
	validTypes := []string{"bible", "personal", "instance", "universal"}
	isValid := false
	for _, validType := range validTypes {
		if options.Type == validType {
			isValid = true
			break
		}
	}

	if !isValid {
		fmt.Fprintf(os.Stderr, "Invalid type: %s\n", options.Type)
		fmt.Fprintf(os.Stderr, "Valid types: %s\n", strings.Join(validTypes, ", "))
		os.Exit(1)
	}

	// Create entry
	result := createJournalEntry(options)

	if result.Success {
		fmt.Printf("✅ Created journal entry: %s\n", result.Path)
		fmt.Printf("\nOpen with: $EDITOR %s\n", result.Path)
		os.Exit(0)
	} else {
		fmt.Fprintf(os.Stderr, "❌ Failed to create entry: %s\n", result.Error)
		os.Exit(1)
	}
}
