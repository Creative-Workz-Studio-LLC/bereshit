// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: format-lookup-pkg-mappings
// Purpose: OmniCode format mapping lookup operations
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
//
// Config: ../../../config/format-mappings.toml
//
// ═══════════════════════════════════════════════════════════════════════════

package mappings

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"strings"

	"cws.studio/skills/format-lookup/scripts/pkg/config"
)

// Result holds lookup results
type Result struct {
	Element  string
	Format   string
	Mappings []config.Mapping
	Found    bool
	Message  string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Lookup finds mappings for an OmniCode element in a target format
func Lookup(element, format string, cfg *config.Config) Result {
	result := Result{
		Element: element,
		Format:  format,
	}

	// Validate format exists
	if !cfg.IsValidFormat(format) {
		result.Message = "Unknown format '" + format + "'. See config/format-mappings.toml"
		return result
	}

	// Get all mappings for format
	allMappings := cfg.GetMappings(format)
	if allMappings == nil {
		result.Message = "No mappings defined for format '" + format + "'"
		return result
	}

	// Search for matching elements (case-insensitive)
	elementLower := strings.ToLower(element)
	for _, m := range allMappings {
		if strings.Contains(strings.ToLower(m.OmniCode), elementLower) {
			result.Mappings = append(result.Mappings, m)
		}
	}

	result.Found = len(result.Mappings) > 0
	if result.Found {
		result.Message = "Found mappings"
	} else {
		result.Message = "No mappings found for '" + element + "' in format '" + format + "'"
	}

	return result
}

// ListAll returns all mappings for a format
func ListAll(format string, cfg *config.Config) Result {
	result := Result{
		Format: format,
	}

	if !cfg.IsValidFormat(format) {
		result.Message = "Unknown format '" + format + "'. See config/format-mappings.toml"
		return result
	}

	result.Mappings = cfg.GetMappings(format)
	result.Found = len(result.Mappings) > 0

	if result.Found {
		result.Message = "All mappings for format"
	} else {
		result.Message = "No mappings defined for format '" + format + "'"
	}

	return result
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
