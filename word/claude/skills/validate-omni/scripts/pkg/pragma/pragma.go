// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: validate-omni-pkg-pragma
// Purpose: Pragma validation for OmniCode files
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-08
// Updated: 2025-12-09
//
// Config: ../../../config/pragma-types.toml
// Reference: ../../../references/pragma-types.md
//
// ═══════════════════════════════════════════════════════════════════════════

package pragma

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bufio"
	"os"
	"regexp"

	"cws.studio/skills/validate-omni/scripts/pkg/config"
)

// Result holds the pragma check result
type Result struct {
	Type      string
	Format    string
	Line      int
	Valid     bool
	TypeValid bool
	Message   string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Check examines the first two lines for a valid pragma using loaded config
func Check(filename string, cfg *config.Config) Result {
	result := Result{}

	file, err := os.Open(filename)
	if err != nil {
		result.Message = "Cannot open file: " + err.Error()
		return result
	}
	defer file.Close()

	pragmaRegex := regexp.MustCompile(`^#!omni\s+(\w+)(?:\s+--(\w+))?`)

	scanner := bufio.NewScanner(file)
	lineNum := 0
	for scanner.Scan() && lineNum < 2 {
		lineNum++
		text := scanner.Text()
		if matches := pragmaRegex.FindStringSubmatch(text); matches != nil {
			result.Type = matches[1]
			result.Line = lineNum
			if len(matches) > 2 && matches[2] != "" {
				result.Format = matches[2]
			}

			// Validate type using config
			if !cfg.IsValidPragmaType(result.Type) {
				result.TypeValid = false
				result.Valid = false
				result.Message = "Unknown pragma type '" + result.Type + "'. See config/pragma-types.toml for valid types."
				return result
			}

			// Optionally validate format flag
			if result.Format != "" {
				validFormats := cfg.GetValidFormats(result.Type)
				formatValid := false
				for _, vf := range validFormats {
					if result.Format == vf {
						formatValid = true
						break
					}
				}
				if !formatValid && len(validFormats) > 0 {
					result.TypeValid = true
					result.Valid = false
					result.Message = "Invalid format '" + result.Format + "' for type '" + result.Type + "'. See config/pragma-types.toml"
					return result
				}
			}

			result.TypeValid = true
			result.Valid = true
			result.Message = "Valid pragma found"
			return result
		}
	}

	result.Message = "No pragma found on line 1 or 2. Expected #!omni [type]. See config/pragma-types.toml"
	return result
}

// ExtractFromLines extracts pragma info from already-loaded lines (no config needed)
func ExtractFromLines(lines []string) (pragmaType, format string) {
	pragmaRegex := regexp.MustCompile(`^#!omni\s+(\w+)(?:\s+--(\w+))?`)

	for i := 0; i < min(2, len(lines)); i++ {
		if matches := pragmaRegex.FindStringSubmatch(lines[i]); matches != nil {
			pragmaType = matches[1]
			if len(matches) > 2 {
				format = matches[2]
			}
			return
		}
	}
	return "", ""
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
