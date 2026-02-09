// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: validate-omni-pkg-blocks
// Purpose: Block structure validation for OmniCode files
// Biblical: Hebrews 3:4 - "For every house is builded by some man"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-08
// Updated: 2025-12-09
//
// Config: ../../../config/block-structures.toml
// Reference: ../../../references/block-structures.md
//
// ═══════════════════════════════════════════════════════════════════════════

package blocks

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"regexp"
	"strings"

	"cws.studio/skills/validate-omni/scripts/pkg/config"
)

// Spec defines the expected block structure for a pragma type
type Spec struct {
	Count  int
	Blocks []string
}

// Result holds the block check result
type Result struct {
	PragmaType string
	Expected   []string
	Found      []string
	Missing    []string
	Extra      []string
	Valid      bool
	Message    string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// FindInLines extracts block names from file content
func FindInLines(lines []string) []string {
	var blocks []string
	blockMarkerRegex := regexp.MustCompile(`^//\s*═{10,}`)
	blockNameRegex := regexp.MustCompile(`^//\s+([A-Z]+)\s*$`)

	for i := 0; i < len(lines); i++ {
		if blockMarkerRegex.MatchString(lines[i]) && i+1 < len(lines) {
			if matches := blockNameRegex.FindStringSubmatch(lines[i+1]); matches != nil {
				blockName := matches[1]
				// Skip END markers
				if !strings.HasPrefix(blockName, "END") {
					blocks = append(blocks, blockName)
				}
			}
		}
	}
	return blocks
}

// matchesSpec checks if found blocks match a given spec
func matchesSpec(found []string, spec Spec) (missing, extra []string) {
	expectedSet := make(map[string]bool)
	for _, b := range spec.Blocks {
		expectedSet[b] = true
	}

	foundSet := make(map[string]bool)
	for _, b := range found {
		foundSet[b] = true
	}

	// Find missing blocks
	for _, b := range spec.Blocks {
		if !foundSet[b] {
			missing = append(missing, b)
		}
	}

	// Find extra blocks
	for _, b := range found {
		if !expectedSet[b] {
			extra = append(extra, b)
		}
	}

	return missing, extra
}

// Check validates block structure against expected pattern using loaded config
func Check(lines []string, pragmaType string, cfg *config.Config) Result {
	result := Result{
		PragmaType: pragmaType,
	}

	if pragmaType == "" {
		result.Message = "No pragma type - cannot determine expected blocks. See config/pragma-types.toml"
		return result
	}

	// Find actual blocks
	result.Found = FindInLines(lines)

	// Get primary expected blocks from config
	blockSpec, ok := cfg.GetBlockSpec(pragmaType)
	if !ok {
		blockSpec, _ = cfg.GetBlockSpec("code") // Default to code structure
		result.Message = "Unknown pragma type '" + pragmaType + "' - using code structure. See config/block-structures.toml"
	}

	spec := Spec{Count: blockSpec.Count, Blocks: blockSpec.Blocks}

	// Check against primary spec
	missing, extra := matchesSpec(result.Found, spec)

	// If primary doesn't match and this is a template, check alternates
	if len(missing) > 0 && pragmaType == "template" {
		alternates := cfg.GetTemplateAlternates()
		for _, altBlocks := range alternates {
			altSpec := Spec{Count: len(altBlocks), Blocks: altBlocks}
			altMissing, altExtra := matchesSpec(result.Found, altSpec)
			// If alternate matches better (fewer missing), use it
			if len(altMissing) < len(missing) {
				spec = altSpec
				missing = altMissing
				extra = altExtra
			}
			// If perfect match, stop searching
			if len(altMissing) == 0 {
				break
			}
		}
	}

	result.Expected = spec.Blocks
	result.Missing = missing
	result.Extra = extra

	result.Valid = len(result.Missing) == 0
	if result.Valid {
		result.Message = "All expected blocks found"
	} else {
		result.Message = "Missing blocks. See config/block-structures.toml for expected structure"
	}

	return result
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
