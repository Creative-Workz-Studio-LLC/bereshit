// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: validate-omni-pkg-sections
// Purpose: Required METADATA section validation for OmniCode files
// Biblical: 1 Thessalonians 5:21 - "Prove all things; hold fast that which is good"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-08
// Updated: 2025-12-09
//
// Config: ../../../config/required-sections.toml
// Reference: ../../../references/required-sections.md
//
// ═══════════════════════════════════════════════════════════════════════════

package sections

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"strings"

	"github.com/creativeworkzstudio/claude-global/skills/validate-omni/scripts/pkg/config"
)

// Result holds the sections check result
type Result struct {
	Found   map[string]bool // section name -> found
	Count   int
	Total   int
	Valid   bool
	Message string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Check looks for required METADATA sections using loaded config
func Check(content string, cfg *config.Config) Result {
	result := Result{
		Found: make(map[string]bool),
		Total: cfg.RequiredSections.Summary.TotalRequired,
	}

	// Check each section from config
	for name, section := range cfg.RequiredSections.Sections {
		if !section.Required {
			continue
		}

		// Check if any of the markers are found
		found := false
		for _, marker := range section.Markers {
			if strings.Contains(content, marker) {
				found = true
				break
			}
		}

		result.Found[name] = found
		if found {
			result.Count++
		}
	}

	result.Valid = result.Count == result.Total
	if result.Valid {
		result.Message = "All required sections found"
	} else {
		result.Message = "Missing required sections. See config/required-sections.toml"
	}

	return result
}

// MissingList returns a slice of missing section descriptions
func (r *Result) MissingList(cfg *config.Config) []string {
	var missing []string
	for name, found := range r.Found {
		if !found {
			if section, ok := cfg.RequiredSections.Sections[name]; ok {
				// Build description from markers
				markers := strings.Join(section.Markers, "/")
				missing = append(missing, markers+" ("+section.Name+")")
			}
		}
	}
	return missing
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
