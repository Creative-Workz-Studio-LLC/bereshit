// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: create-journal-entry-pkg-entry
// Purpose: Journal entry creation and template generation
// Biblical: Psalm 119:11 - "Thy word have I hid in mine heart"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-09
//
// ═══════════════════════════════════════════════════════════════════════════

package entry

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"

	"cws.studio/skills/create-journal-entry/scripts/pkg/config"
)

// --- Types ---

// Entry represents a journal entry
type Entry struct {
	Type      string
	Title     string
	Date      time.Time
	Path      string
	Sections  []string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

// generateFilename creates the filename for an entry
func generateFilename(cfg *config.Config, journalType, title string, date time.Time) string {
	jt, ok := cfg.GetJournalType(journalType)
	if !ok {
		return fmt.Sprintf("%s_entry.md", date.Format("2006-01-02"))
	}

	pattern := jt.FilenamePattern
	dateStr := date.Format("2006-01-02")

	// Replace placeholders
	filename := strings.ReplaceAll(pattern, "{date}", dateStr)

	// Clean title for filename
	cleanTitle := strings.ToLower(title)
	cleanTitle = strings.ReplaceAll(cleanTitle, " ", "-")
	cleanTitle = strings.ReplaceAll(cleanTitle, ":", "")

	filename = strings.ReplaceAll(filename, "{topic}", cleanTitle)
	filename = strings.ReplaceAll(filename, "{book}", cleanTitle)
	filename = strings.ReplaceAll(filename, "{chapter}", "1")

	return filename
}

// generateTemplate creates the entry template content
func generateTemplate(cfg *config.Config, journalType, title string, date time.Time) string {
	jt, ok := cfg.GetJournalType(journalType)
	if !ok {
		return fmt.Sprintf("# %s\n\n**Date:** %s\n\n", title, date.Format("2006-01-02"))
	}

	var sb strings.Builder

	// Header
	sb.WriteString(fmt.Sprintf("# %s\n\n", title))
	sb.WriteString(fmt.Sprintf("**Date:** %s\n", date.Format("2006-01-02")))
	sb.WriteString(fmt.Sprintf("**Type:** %s\n\n", jt.Name))
	sb.WriteString("---\n\n")

	// Required sections
	for _, section := range jt.Sections.Required {
		sectionTitle := strings.Title(strings.ReplaceAll(section, "-", " "))
		sb.WriteString(fmt.Sprintf("## %s\n\n", sectionTitle))
		sb.WriteString("[Write here...]\n\n")
	}

	// Optional sections (commented)
	if len(jt.Sections.Optional) > 0 {
		sb.WriteString("---\n\n")
		sb.WriteString("<!-- Optional sections:\n")
		for _, section := range jt.Sections.Optional {
			sectionTitle := strings.Title(strings.ReplaceAll(section, "-", " "))
			sb.WriteString(fmt.Sprintf("## %s\n\n", sectionTitle))
		}
		sb.WriteString("-->\n")
	}

	return sb.String()
}

// --- Core Operations ---

// Create generates a new journal entry
func Create(cfg *config.Config, journalType, title string) (*Entry, error) {
	// Validate journal type
	jt, ok := cfg.GetJournalType(journalType)
	if !ok {
		return nil, fmt.Errorf("unknown journal type: %s", journalType)
	}

	// Get current date
	date := time.Now()

	// Generate filename and path
	filename := generateFilename(cfg, journalType, title, date)
	journalPath := cfg.GetJournalPath(journalType)
	fullPath := filepath.Join(journalPath, filename)

	// Check if file exists
	if _, err := os.Stat(fullPath); err == nil {
		return nil, fmt.Errorf("entry already exists: %s", fullPath)
	}

	// Generate template content
	content := generateTemplate(cfg, journalType, title, date)

	// Ensure directory exists
	if err := os.MkdirAll(journalPath, 0755); err != nil {
		return nil, fmt.Errorf("failed to create journal directory: %w", err)
	}

	// Write file
	if err := os.WriteFile(fullPath, []byte(content), 0644); err != nil {
		return nil, fmt.Errorf("failed to write entry: %w", err)
	}

	return &Entry{
		Type:     journalType,
		Title:    title,
		Date:     date,
		Path:     fullPath,
		Sections: jt.Sections.Required,
	}, nil
}

// GetTemplate returns the template for a journal type without creating
func GetTemplate(cfg *config.Config, journalType, title string) (string, error) {
	_, ok := cfg.GetJournalType(journalType)
	if !ok {
		return "", fmt.Errorf("unknown journal type: %s", journalType)
	}

	return generateTemplate(cfg, journalType, title, time.Now()), nil
}

// ListTypes returns information about all journal types
func ListTypes(cfg *config.Config) []config.JournalType {
	var types []config.JournalType
	for _, name := range cfg.GetJournalTypes() {
		if jt, ok := cfg.GetJournalType(name); ok {
			types = append(types, *jt)
		}
	}
	return types
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
