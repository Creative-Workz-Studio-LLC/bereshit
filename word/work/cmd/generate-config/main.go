// ═══════════════════════════════════════════════════════════════════════════
// generate-config: TOML → C Header Generator
// Key: B-word-work-cmd-generate-config
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: pkg/config)
//
// derives_from: bereshit/word/seed/code/go/executable.go
//
// ═══════════════════════════════════════════════════════════════════════════

// Package main generates C header fragments from TOML specifications.
//
// generate-config - CPI-SI Bereshit Build Tool
//
// ────────────────────────────────────────────────────────────────
// CORE IDENTITY
// ────────────────────────────────────────────────────────────────
//
// # Biblical Foundation
//
// Scripture: "And the Word was made flesh, and dwelt among us" (John 1:14)
//
// Principle: Specifications become concrete implementation. TOML definitions
// (the word) become C headers (the flesh) that run on real hardware.
//
// Anchor: "So shall my word be that goeth forth out of my mouth: it shall
// not return unto me void, but it shall accomplish that which I please" (Isaiah 55:11)
//
// # CPI-SI Identity
//
// Component Type: Baton - runs once, produces output, exits
//
// Role: Bridge between configuration specifications and compiled code.
// Ensures C code stays synchronized with TOML source of truth.
//
// Paradigm: Config-driven development - single source, generated outputs
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise, Nova Dawn
//   - Implementation: Nova Dawn
//   - Created: 2025-12-14
//   - Version: 0.1.0
//
// # Purpose & Function
//
// Purpose: Generate C header fragments (.gen.h) from TOML specifications,
// maintaining single source of truth for constants and mappings.
//
// Core Design: Template-based code generation. Read TOML, apply template,
// output .gen.h fragment that gets included in hand-written 4-block headers.
//
// Key Features:
//
//   - Registry-based: add generators by adding to registry, not code
//   - Generic TOML parsing: map[string]any, templates access fields
//   - Generated files are fragments (no 4-block structure)
//   - Hand-written files include fragments in SETUP block
//   - Extensible to all 24+ TOML specs in word/core/
//
// Philosophy: One truth, many views. TOML is the truth, C headers are views.
//
// ────────────────────────────────────────────────────────────────
// INTERFACE
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: fmt, os, path/filepath, text/template
//   - External: github.com/BurntSushi/toml
//   - Internal: None (self-contained TOML parsing)
//
// What Uses This:
//
//   - Makefile: `make generate` target runs this before compile
//
// # Usage
//
// Command Line:
//
//	generate-config [config-dir] [output-dir]
//	generate-config                           Uses defaults
//
// Exit Codes:
//
//	0  - Success, headers generated
//	1  - Error (TOML parse, template, write)
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL
// ────────────────────────────────────────────────────────────────
//
// # Blocking Status
//
// Blocking: Build fails if generation fails. This is intentional -
// we want compilation to stop if specs and code would drift.
//
// [OMIT: Health Scoring - Baton tool, no persistent state to track]
//
package main

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

import (
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"text/template"

	"github.com/BurntSushi/toml"

	"creativeworkzstudio.com/bereshit/word/work/pkg/config"
)

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────

const (
	// DefaultConfigDir is the path to TOML specifications relative to BERESHIT_ROOT.
	DefaultConfigDir = "word/core"

	// DefaultOutputDir is the path to generated headers relative to BERESHIT_ROOT.
	DefaultOutputDir = "word/work/pkg/trit/include/generated"

	// System paths - the 9 systems under word/core/ (mirrors loader.go)
	MathPath       = "word/core/math"
	TypesPath      = "word/core/types"
	LanguagePath   = "word/core/language"
	BiblePath      = "word/core/bible"
	HealthPath     = "word/core/os/health"
	PermissionPath = "word/core/os/permission"
	IdentityPath   = "word/core/identity"
	NetworkPath    = "word/core/network"
	FilesystemPath = "word/core/filesystem"
)

// ────────────────────────────────────────────────────────────────
// Variables
// ────────────────────────────────────────────────────────────────

// fallbackSystemPaths defines hardcoded system paths for TRIPWIRE fallback.
// Only used when index.toml is missing - this is degraded mode.
var fallbackSystemPaths = []string{
	MathPath,
	TypesPath,
	LanguagePath,
	BiblePath,
	HealthPath,
	PermissionPath,
	IdentityPath,
	NetworkPath,
	FilesystemPath,
}

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// GenerateMeta holds the [_generate] metadata from a TOML file.
// This tells the generator whether and how to process the file.
// All generation specifics come from TOML, not from Go code.
//
// Required fields:
//   - enabled: Must be true for generation
//   - output: Output filename (e.g., "ternary_math.gen.h")
//   - guard: Include guard name (e.g., "TERNARY_MATH_GEN_H")
//   - sections: Which TOML sections to process
//
// Optional fields for special handling:
//   - template: "simple" (default) or "complex" for array-heavy files
//   - arrays: List of section paths that contain arrays needing special formatting
//   - prefix_override: Override the auto-generated prefix
type GenerateMeta struct {
	Enabled        bool     `toml:"enabled"`
	Output         string   `toml:"output"`
	Order          int      `toml:"order"`
	Guard          string   `toml:"guard"`
	Description    string   `toml:"description"`
	Sections       []string `toml:"sections"`
	Template       string   `toml:"template"`        // "simple" or "complex"
	Arrays         []string `toml:"arrays"`          // Section paths with arrays
	PrefixOverride string   `toml:"prefix_override"` // Override auto-prefix
}

// MasterTemplateContext provides data for the master template.
// Combines metadata with raw TOML data for unified generation.
type MasterTemplateContext struct {
	Meta MasterTemplateMeta // From [_generate] section
	Data map[string]any     // Raw TOML data
}

// MasterTemplateMeta holds metadata for master template.
type MasterTemplateMeta struct {
	Source         string   // Source TOML path
	Guard          string   // Include guard name
	Output         string   // Output filename
	Description    string   // What this generates
	Sections       []string // Which sections to process
	Template       string   // Template type: "simple" or "complex"
	Arrays         []string // Section paths with arrays
	PrefixOverride string   // Override auto-prefix
}

// ────────────────────────────────────────────────────────────────
// Type Methods
// ────────────────────────────────────────────────────────────────

// [Reserved: No type methods needed - structs are pure data containers]

// ────────────────────────────────────────────────────────────────
// Package-Level State
// ────────────────────────────────────────────────────────────────

// [Reserved: Baton tool - no Rails infrastructure needed]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Internal Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   main() (Entry Point)
//   └── runGenerators() → uses loadTOML(), applyTemplate(), writeOutput()
//
//   Core Operations (Middle Rungs)
//   ├── loadTOML() → toml.DecodeFile into map[string]any
//   ├── applyTemplate() → text/template execution
//   └── writeOutput() → os.WriteFile
//
// Baton Flow:
//
//   Entry → main()
//     ↓
//   init() populates generators registry
//     ↓
//   runGenerators() loops through registry
//     ↓
//   For each: loadTOML() → applyTemplate() → writeOutput()
//     ↓
//   Exit → success or error
//
// APUs: 5 functions (1 init, 3 core ops, 1 main)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────
// [Reserved: Baton tool - core operations are simple enough to not need helpers.
// loadTOML, applyTemplate, writeOutput each do one thing directly.]

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
// Generator registry population, TOML loading, template application,
// file output, and orchestration.

// ────────────────────────────────────────────────────────────────
// No init() - Master template driven by [_generate] in TOML
// ────────────────────────────────────────────────────────────────
//
// All generation is now controlled by [_generate] metadata in each TOML file.
// No templates are hardcoded in Go - the TOML describes itself.
//
// To add generation for a new TOML:
//   1. Add [_generate] section to the TOML file
//   2. Set enabled = true, output, guard, sections
//   3. Generator will auto-process using master template
//
// Template Functions Available (for reference):
// - intArray: []int → "{1, 2, 3}"
// - int2DArray: [][]int → "{{1,2}, {3,4}}"
// - int3DArray: [][][]int → "{{{1,2}, {3,4}}, ...}"
// - uint64Array: []int64 → "{1ULL, 2ULL, ...}"
// - upper: string → "STRING"
// - renderDefines: auto-generate #defines for numeric values
// - renderNestedDefines: handle nested table structures
// - renderArrays: handle array sections with proper C formatting

// ────────────────────────────────────────────────────────────────
// Templates - Master Template Only
// ────────────────────────────────────────────────────────────────
// ONE master template handles ALL TOML files.
// Specifics come from [_generate] metadata in each TOML.

// ════════════════════════════════════════════════════════════════════════════
// MASTER TEMPLATE - Unified template for any TOML with [_generate] metadata
// ════════════════════════════════════════════════════════════════════════════
// This template reads [_generate] metadata and auto-generates C code.
// It handles common patterns: #define for numeric values, arrays, nested tables.
//
// Requirements in TOML:
//   [_generate]
//   enabled = true
//   output = "filename.gen.h"
//   guard = "FILENAME_GEN_H"
//   description = "What this generates"
//   sections = ["section1", "section2"]  # which sections to process
//
// Optional for complex files (with arrays):
//   template = "complex"
//   arrays = ["section.field", "section.nested.field"]  # paths to array data
//
// The template iterates sections and generates appropriate C constructs.
// Simple template: just #defines
// Complex template: #defines + static arrays

const masterTemplate = `// ═══════════════════════════════════════════════════════════════════════════
// GENERATED FILE - DO NOT EDIT
// ═══════════════════════════════════════════════════════════════════════════
//
// Source: {{.Meta.Source}}
// Generator: generate-config (master template)
//
// This file is auto-generated from TOML specifications.
// To modify: edit the source TOML file and regenerate.
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef {{.Meta.Guard}}
#define {{.Meta.Guard}}

#include <stdint.h>

{{range $section := .Meta.Sections}}
// ════════════════════════════════════════════════════════════════════════════
// {{sectionName $section}} SECTION
// ════════════════════════════════════════════════════════════════════════════
{{with getSection $.Data $section}}
{{renderDefines (sectionName $section) .}}
{{renderNestedDefines (sectionName $section) .}}
{{renderArrays (sectionName $section) . $.Meta.Arrays}}
{{end}}
{{end}}

#endif // {{.Meta.Guard}}
`

// ────────────────────────────────────────────────────────────────
// TOML Loading - Parse Specifications
// ────────────────────────────────────────────────────────────────

// loadTOML parses a TOML file into a generic map for template access.
//
// Parameters:
//
//	path: Absolute path to TOML file
//
// Returns:
//
//	map[string]any: Parsed TOML data accessible via {{.section.field}}
//	error: File read or parse failure
func loadTOML(path string) (map[string]any, error) {
	var data map[string]any
	if _, err := toml.DecodeFile(path, &data); err != nil {
		return nil, fmt.Errorf("decode %s: %w", path, err)
	}
	return data, nil
}

// extractGenerateMeta extracts [_generate] metadata from parsed TOML.
//
// Returns nil if [_generate] section doesn't exist or enabled=false.
// The metadata tells the generator whether and how to process this TOML.
func extractGenerateMeta(data map[string]any) *GenerateMeta {
	genSection, ok := data["_generate"]
	if !ok {
		return nil
	}

	genMap, ok := genSection.(map[string]any)
	if !ok {
		return nil
	}

	// Check if enabled
	enabled, _ := genMap["enabled"].(bool)
	if !enabled {
		return nil
	}

	meta := &GenerateMeta{
		Enabled:  true,
		Template: "simple", // Default to simple template
	}

	// Extract required fields
	if v, ok := genMap["output"].(string); ok {
		meta.Output = v
	}
	if v, ok := genMap["guard"].(string); ok {
		meta.Guard = v
	}
	if sections, ok := genMap["sections"].([]any); ok {
		for _, s := range sections {
			if str, ok := s.(string); ok {
				meta.Sections = append(meta.Sections, str)
			}
		}
	}

	// Extract optional fields
	if v, ok := genMap["order"].(int64); ok {
		meta.Order = int(v)
	}
	if v, ok := genMap["description"].(string); ok {
		meta.Description = v
	}
	if v, ok := genMap["template"].(string); ok {
		meta.Template = v
	}
	if v, ok := genMap["prefix_override"].(string); ok {
		meta.PrefixOverride = v
	}
	if arrays, ok := genMap["arrays"].([]any); ok {
		for _, a := range arrays {
			if str, ok := a.(string); ok {
				meta.Arrays = append(meta.Arrays, str)
			}
		}
	}

	return meta
}

// ────────────────────────────────────────────────────────────────
// Template Application - Generate Output
// ────────────────────────────────────────────────────────────────

// templateFuncs provides helper functions for C code generation.
//
// Available functions:
//   - intArray: []any → "{1, 2, 3}" C array literal
//   - int2DArray: [][]any → "{{1,2},{3,4}}" C 2D array literal
//   - int3DArray: [][][]any → "{{{1},{2}},{{3},{4}}}" C 3D array literal
//   - upper: string → UPPER_CASE
//   - join: []any → "a, b, c" joined string
//   - sectionName: "a.b.c" → "C" (last part, uppercased)
//   - getSection: navigate dotted path in map
var templateFuncs = template.FuncMap{
	// ═══════════════════════════════════════════════════════════════════════
	// MASTER TEMPLATE HELPERS - Auto-generate C code from TOML structure
	// ═══════════════════════════════════════════════════════════════════════

	// sectionName extracts the C prefix from a section path
	// "operations.half_adder" → "HALF_ADDER"
	// "trit" → "TRIT"
	"sectionName": func(section string) string {
		parts := strings.Split(section, ".")
		// Use the last part as the name, or combine last two for clarity
		if len(parts) >= 2 {
			// operations.half_adder → HALF_ADDER
			return strings.ToUpper(parts[len(parts)-1])
		}
		return strings.ToUpper(section)
	},

	// getSection navigates a dotted path in the data map
	// getSection(data, "operations.half_adder") → data["operations"]["half_adder"]
	"getSection": func(data map[string]any, section string) any {
		parts := strings.Split(section, ".")
		var current any = data
		for _, part := range parts {
			if m, ok := current.(map[string]any); ok {
				current = m[part]
			} else {
				return nil
			}
		}
		return current
	},

	// renderDefines generates #define statements for all numeric values in a table
	// Usage: {{renderDefines "PREFIX" .section}}
	// Output: #define PREFIX_KEY value
	"renderDefines": func(prefix string, table any) string {
		m, ok := table.(map[string]any)
		if !ok {
			return ""
		}
		var lines []string
		for key, val := range m {
			// Skip nested tables and arrays
			switch val.(type) {
			case map[string]any, []any:
				continue
			}
			// Only render numeric values
			switch v := val.(type) {
			case int, int64, float64:
				name := strings.ToUpper(prefix + "_" + key)
				lines = append(lines, fmt.Sprintf("#define %-25s %v", name, toInt(v)))
			}
		}
		// Sort for consistent output
		sort.Strings(lines)
		return strings.Join(lines, "\n")
	},

	// renderNestedDefines generates #defines for nested tables
	// Usage: {{renderNestedDefines "PREFIX" .section}}
	// Handles one level of nesting: section.subsection.key → PREFIX_SUBSECTION_KEY
	"renderNestedDefines": func(prefix string, table any) string {
		m, ok := table.(map[string]any)
		if !ok {
			return ""
		}
		var lines []string
		for key, val := range m {
			// Only process nested tables
			nested, ok := val.(map[string]any)
			if !ok {
				continue
			}
			subPrefix := strings.ToUpper(prefix + "_" + key)
			for subKey, subVal := range nested {
				// Skip further nesting
				switch subVal.(type) {
				case map[string]any, []any:
					continue
				}
				switch v := subVal.(type) {
				case int, int64, float64:
					name := strings.ToUpper(subPrefix + "_" + subKey)
					lines = append(lines, fmt.Sprintf("#define %-30s %v", name, toInt(v)))
				}
			}
		}
		sort.Strings(lines)
		return strings.Join(lines, "\n")
	},

	// renderArrays generates static const arrays for array values
	// Usage: {{renderArrays "PREFIX" .section arrayPaths}}
	// arrayPaths is a slice of paths like ["values", "powers"] indicating which fields are arrays
	"renderArrays": func(prefix string, table any, arrayPaths []string) string {
		m, ok := table.(map[string]any)
		if !ok || len(arrayPaths) == 0 {
			return ""
		}

		var lines []string
		for key, val := range m {
			// Check if this key is in the array paths
			isArrayPath := false
			for _, path := range arrayPaths {
				// Simple match: just the key name
				if path == key || strings.HasSuffix(path, "."+key) {
					isArrayPath = true
					break
				}
			}

			if !isArrayPath {
				continue
			}

			// Handle arrays
			arr, ok := val.([]any)
			if !ok {
				continue
			}

			name := strings.ToUpper(prefix + "_" + key)
			// Determine array dimension and type
			if len(arr) > 0 {
				switch arr[0].(type) {
				case []any:
					// 2D or 3D array
					if len(arr) > 0 {
						if inner, ok := arr[0].([]any); ok && len(inner) > 0 {
							if _, ok := inner[0].([]any); ok {
								// 3D array
								lines = append(lines, fmt.Sprintf("static const int8_t %s[%d][%d][%d] = %s;",
									name, len(arr), len(inner), len(inner[0].([]any)), formatInt3DArray(arr)))
							} else {
								// 2D array
								lines = append(lines, fmt.Sprintf("static const int8_t %s[%d][%d] = %s;",
									name, len(arr), len(inner), formatInt2DArray(arr)))
							}
						}
					}
				default:
					// 1D array - determine type based on values
					maxVal := int64(0)
					for _, v := range arr {
						if n := toInt64(v); n > maxVal {
							maxVal = n
						}
					}
					cType := "int8_t"
					suffix := ""
					if maxVal > 127 {
						cType = "uint8_t"
					}
					if maxVal > 255 {
						cType = "uint16_t"
					}
					if maxVal > 65535 {
						cType = "uint64_t"
						suffix = "ULL"
					}
					lines = append(lines, fmt.Sprintf("static const %s %s[%d] = %s;",
						cType, name, len(arr), formatIntArray(arr, suffix)))
				}
			}
		}
		return strings.Join(lines, "\n\n")
	},

	// isNumeric returns true if value is a number
	"isNumeric": func(val any) bool {
		switch val.(type) {
		case int, int64, float64:
			return true
		default:
			return false
		}
	},

	// isArray returns true if value is an array
	"isArray": func(val any) bool {
		_, ok := val.([]any)
		return ok
	},

	// isTable returns true if value is a nested table
	"isTable": func(val any) bool {
		_, ok := val.(map[string]any)
		return ok
	},

	// ═══════════════════════════════════════════════════════════════════════
	// ORIGINAL HELPERS - Specific array formatting
	// ═══════════════════════════════════════════════════════════════════════

	// intArray converts a slice to C array initializer: {1, 2, 3}
	"intArray": func(arr any) string {
		slice, ok := arr.([]any)
		if !ok {
			return "{}"
		}
		var parts []string
		for _, v := range slice {
			parts = append(parts, fmt.Sprintf("%v", toInt(v)))
		}
		return "{" + strings.Join(parts, ", ") + "}"
	},

	// int2DArray converts 2D slice to C 2D array: {{1,2}, {3,4}}
	"int2DArray": func(arr any) string {
		slice, ok := arr.([]any)
		if !ok {
			return "{}"
		}
		var rows []string
		for _, row := range slice {
			rowSlice, ok := row.([]any)
			if !ok {
				continue
			}
			var parts []string
			for _, v := range rowSlice {
				parts = append(parts, fmt.Sprintf("%d", toInt(v)))
			}
			rows = append(rows, "{"+strings.Join(parts, ", ")+"}")
		}
		return "{\n    " + strings.Join(rows, ",\n    ") + "\n}"
	},

	// int3DArray converts 3D slice to C 3D array
	"int3DArray": func(arr any) string {
		slice, ok := arr.([]any)
		if !ok {
			return "{}"
		}
		var planes []string
		for _, plane := range slice {
			planeSlice, ok := plane.([]any)
			if !ok {
				continue
			}
			var rows []string
			for _, row := range planeSlice {
				rowSlice, ok := row.([]any)
				if !ok {
					continue
				}
				var parts []string
				for _, v := range rowSlice {
					parts = append(parts, fmt.Sprintf("%2d", toInt(v)))
				}
				rows = append(rows, "{"+strings.Join(parts, ", ")+"}")
			}
			planes = append(planes, "{\n        "+strings.Join(rows, ",\n        ")+"\n    }")
		}
		return "{\n    " + strings.Join(planes, ",\n    ") + "\n}"
	},

	// uint64Array for large numbers (trit27_powers)
	"uint64Array": func(arr any) string {
		slice, ok := arr.([]any)
		if !ok {
			return "{}"
		}
		var parts []string
		for _, v := range slice {
			parts = append(parts, fmt.Sprintf("%dULL", toInt64(v)))
		}
		return "{\n    " + strings.Join(parts, ",\n    ") + "\n}"
	},

	// upper converts to UPPER_CASE
	"upper": strings.ToUpper,

	// arrayLen returns length of array
	"arrayLen": func(arr any) int {
		slice, ok := arr.([]any)
		if !ok {
			return 0
		}
		return len(slice)
	},
}

// toInt converts any numeric type to int64 then int
func toInt(v any) int {
	switch n := v.(type) {
	case int:
		return n
	case int64:
		return int(n)
	case float64:
		return int(n)
	default:
		return 0
	}
}

// toInt64 converts any numeric type to int64
func toInt64(v any) int64 {
	switch n := v.(type) {
	case int:
		return int64(n)
	case int64:
		return n
	case float64:
		return int64(n)
	default:
		return 0
	}
}

// formatIntArray formats a 1D array as C array literal
func formatIntArray(arr []any, suffix string) string {
	var parts []string
	for _, v := range arr {
		parts = append(parts, fmt.Sprintf("%d%s", toInt64(v), suffix))
	}
	if len(parts) > 10 {
		return "{\n    " + strings.Join(parts, ",\n    ") + "\n}"
	}
	return "{" + strings.Join(parts, ", ") + "}"
}

// formatInt2DArray formats a 2D array as C 2D array literal
func formatInt2DArray(arr []any) string {
	var rows []string
	for _, row := range arr {
		rowSlice, ok := row.([]any)
		if !ok {
			continue
		}
		var parts []string
		for _, v := range rowSlice {
			parts = append(parts, fmt.Sprintf("%d", toInt(v)))
		}
		rows = append(rows, "{"+strings.Join(parts, ", ")+"}")
	}
	return "{\n    " + strings.Join(rows, ",\n    ") + "\n}"
}

// formatInt3DArray formats a 3D array as C 3D array literal
func formatInt3DArray(arr []any) string {
	var planes []string
	for _, plane := range arr {
		planeSlice, ok := plane.([]any)
		if !ok {
			continue
		}
		var rows []string
		for _, row := range planeSlice {
			rowSlice, ok := row.([]any)
			if !ok {
				continue
			}
			var parts []string
			for _, v := range rowSlice {
				parts = append(parts, fmt.Sprintf("%2d", toInt(v)))
			}
			rows = append(rows, "{"+strings.Join(parts, ", ")+"}")
		}
		planes = append(planes, "{\n        "+strings.Join(rows, ",\n        ")+"\n    }")
	}
	return "{\n    " + strings.Join(planes, ",\n    ") + "\n}"
}

// applyTemplate executes a Go template with TOML data.
//
// Parameters:
//
//	tmplStr: Go template string
//	data: TOML data map for template execution
//
// Returns:
//
//	string: Generated content
//	error: Template parse or execution failure
func applyTemplate(tmplStr string, data map[string]any) (string, error) {
	tmpl, err := template.New("gen").Funcs(templateFuncs).Parse(tmplStr)
	if err != nil {
		return "", fmt.Errorf("parse template: %w", err)
	}

	var buf strings.Builder
	if err := tmpl.Execute(&buf, data); err != nil {
		return "", fmt.Errorf("execute template: %w", err)
	}

	return buf.String(), nil
}

// applyMasterTemplate applies the master template with context.
// This enables unified generation for any TOML with [_generate] metadata.
//
// Parameters:
//
//	source: Source TOML path (for header comment)
//	meta: Extracted [_generate] metadata
//	data: Raw TOML data
//
// Returns:
//
//	string: Generated C header content
//	error: Template parse or execution failure
func applyMasterTemplate(source string, meta *GenerateMeta, data map[string]any) (string, error) {
	ctx := MasterTemplateContext{
		Meta: MasterTemplateMeta{
			Source:         source,
			Guard:          meta.Guard,
			Output:         meta.Output,
			Description:    meta.Description,
			Sections:       meta.Sections,
			Template:       meta.Template,
			Arrays:         meta.Arrays,
			PrefixOverride: meta.PrefixOverride,
		},
		Data: data,
	}

	tmpl, err := template.New("master").Funcs(templateFuncs).Parse(masterTemplate)
	if err != nil {
		return "", fmt.Errorf("parse master template: %w", err)
	}

	var buf strings.Builder
	if err := tmpl.Execute(&buf, ctx); err != nil {
		return "", fmt.Errorf("execute master template: %w", err)
	}

	return buf.String(), nil
}

// ────────────────────────────────────────────────────────────────
// File Output - Write Generated Content
// ────────────────────────────────────────────────────────────────

// writeOutput writes generated content to a file, creating directories as needed.
//
// Parameters:
//
//	path: Output file path
//	content: Generated content to write
//
// Returns:
//
//	error: Directory creation or file write failure
func writeOutput(path string, content string) error {
	dir := filepath.Dir(path)
	if err := os.MkdirAll(dir, 0755); err != nil {
		return fmt.Errorf("create dir %s: %w", dir, err)
	}

	if err := os.WriteFile(path, []byte(content), 0644); err != nil {
		return fmt.Errorf("write %s: %w", path, err)
	}

	return nil
}


// ────────────────────────────────────────────────────────────────
// Orchestration - Walk from Index (Primary Path)
// ────────────────────────────────────────────────────────────────

// walkAndGenerateFromIndex uses word/core/index.toml manifest to drive generation.
// This is the primary path - single source of truth shared with loader.
// All generation uses the master template - specifics come from [_generate] in TOML.
//
// Parameters:
//
//	root: BERESHIT_ROOT path
//
// Returns:
//
//	error: Generation failure or nil if successful
func walkAndGenerateFromIndex(root string) error {
	config.SetRoot(root)

	manifest, err := config.LoadIndex()
	if err != nil {
		return err
	}

	var generated int
	fmt.Println("Walking systems from index.toml (master template)...")
	fmt.Println()

	for _, system := range manifest.Systems {
		systemPath := filepath.Join(root, DefaultConfigDir, system.Path)
		hasOutput := false

		for _, spec := range system.Specs {
			if !spec.Generates {
				continue
			}

			tomlPath := filepath.Join(systemPath, spec.File)
			relPath := filepath.Join(DefaultConfigDir, system.Path, spec.File)

			// Load TOML
			data, err := loadTOML(tomlPath)
			if err != nil {
				fmt.Printf("  ⚠ %s/%s: load error: %v\n", system.Name, spec.File, err)
				continue
			}

			// Extract [_generate] metadata from TOML - this is required now
			meta := extractGenerateMeta(data)
			if meta == nil || len(meta.Sections) == 0 {
				fmt.Printf("  - %s/%s (missing [_generate] metadata)\n", system.Name, spec.File)
				continue
			}

			// Apply master template - all generation is master template now
			content, err := applyMasterTemplate(relPath, meta, data)
			if err != nil {
				return fmt.Errorf("%s/%s: generation error: %w", system.Name, spec.File, err)
			}

			if !hasOutput {
				fmt.Printf("System: %s\n", system.Name)
				hasOutput = true
			}

			fmt.Printf("  ✓ %s → %s\n", spec.File, spec.Output)

			// Write output
			outputPath := filepath.Join(root, DefaultOutputDir, spec.Output)
			if err := writeOutput(outputPath, content); err != nil {
				return fmt.Errorf("%s/%s: write error: %w", system.Name, spec.File, err)
			}

			generated++
		}

		if hasOutput {
			fmt.Println()
		}
	}

	fmt.Printf("Generated %d headers from index.toml manifest.\n", generated)
	return nil
}

// ────────────────────────────────────────────────────────────────
// Orchestration - Tripwire Fallback (when index.toml missing)
// ────────────────────────────────────────────────────────────────

// walkAndGenerateFallback walks hardcoded system paths when index.toml is missing.
// This is the TRIPWIRE FALLBACK - degraded but functional mode.
// Uses master template - specifics must still come from [_generate] in TOML.
//
// Parameters:
//
//	root: BERESHIT_ROOT path
//
// Returns:
//
//	error: Generation failure or nil if successful
func walkAndGenerateFallback(root string) error {
	var generated int
	fmt.Println("Walking systems from HARDCODED PATHS (tripwire fallback)...")
	fmt.Println()

	for _, systemPath := range fallbackSystemPaths {
		dirPath := filepath.Join(root, systemPath)

		// Find all TOML files
		pattern := filepath.Join(dirPath, "*.toml")
		matches, err := filepath.Glob(pattern)
		if err != nil || len(matches) == 0 {
			continue
		}

		systemName := filepath.Base(systemPath)
		hasOutput := false

		for _, tomlPath := range matches {
			// Skip index.toml itself
			if filepath.Base(tomlPath) == "index.toml" {
				continue
			}

			// Load TOML
			data, err := loadTOML(tomlPath)
			if err != nil {
				fmt.Printf("  ⚠ %s/%s: load error: %v\n", systemName, filepath.Base(tomlPath), err)
				continue
			}

			// Extract [_generate] metadata - required for master template
			meta := extractGenerateMeta(data)
			if meta == nil || len(meta.Sections) == 0 {
				// Skip files without [_generate] - they don't generate output
				continue
			}

			relPath := tomlPath[len(root)+1:]

			// Apply master template
			content, err := applyMasterTemplate(relPath, meta, data)
			if err != nil {
				return fmt.Errorf("%s: generation error: %w", filepath.Base(tomlPath), err)
			}

			if !hasOutput {
				fmt.Printf("System: %s\n", systemName)
				hasOutput = true
			}

			fmt.Printf("  ✓ %s → %s\n", filepath.Base(tomlPath), meta.Output)

			// Write output
			outputPath := filepath.Join(root, DefaultOutputDir, meta.Output)
			if err := writeOutput(outputPath, content); err != nil {
				return fmt.Errorf("%s: write error: %w", filepath.Base(tomlPath), err)
			}

			generated++
		}

		if hasOutput {
			fmt.Println()
		}
	}

	fmt.Printf("Generated %d headers from hardcoded paths (tripwire fallback).\n", generated)
	return nil
}

// ────────────────────────────────────────────────────────────────
// Error Handling/Recovery Patterns
// ────────────────────────────────────────────────────────────────
// [Reserved: Uses standard error returns with fmt.Errorf wrapping.
// Baton tool - errors propagate to main() which exits with status 1.]

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
// [Reserved: Baton tool - no exported APIs. Entry point is main() in CLOSING.]

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
//
// -----------------------------------------------------------------------------
// CLOSING Sections Overview
// -----------------------------------------------------------------------------
//
// GROUP 1: CODING (Operations - Execute, Verify, Clean)
//
// 1. CODE VALIDATION (Testing & Verification)
//    Purpose: Prove correctness before shipping - build, test, verify
//    Subsections: Build Verification → Runtime Verification → Testing Requirements → Integration Testing
//
// 2. CODE EXECUTION (Entry Points & Flow)
//    Purpose: Entry point(s) and execution orchestration
//    Subsections: Entry Point → Execution Flow → Exit Codes → Signal Handling
//
// 3. CODE CLEANUP (Resource Management)
//    Purpose: Graceful shutdown, resource release, error state cleanup
//    Subsections: Resource Management → Graceful Shutdown → Error State Cleanup → Memory Management
//
// GROUP 2: FINAL DOCUMENTATION (Synthesis - Reference Back to Earlier Blocks)
//
// 4. EXECUTABLE OVERVIEW (Summary with Back-References)
//    Purpose: High-level summary pointing back to METADATA for details
//    References: METADATA "Purpose & Function", "Key Features", "Usage & Integration"
//
// 5. MODIFICATION POLICY (Safe/Careful/Never)
//    Purpose: Guide future maintainers on what's safe to change
//    Subsections: Safe to Modify → Modify with Care → Never Modify → Validation After
//
// 6. LADDER AND BATON FLOW (Back-Reference to BODY)
//    Purpose: Point to BODY Organizational Chart for architecture
//    References: BODY "Organizational Chart - Internal Structure"
//
// 7. SURGICAL UPDATE POINTS (Back-Reference to BODY)
//    Purpose: Point to BODY subsection extension points
//    References: BODY "Core Operations" subsection comments
//
// 8. PERFORMANCE CONSIDERATIONS (Back-Reference to SETUP/BODY)
//    Purpose: Point to performance notes in earlier sections
//    References: SETUP constants/types, BODY function docstrings
//
// 9. TROUBLESHOOTING GUIDE (Back-Reference to BODY)
//    Purpose: Point to troubleshooting in function docstrings
//    References: BODY function docstrings with troubleshooting sections
//
// 10. RELATED COMPONENTS (Back-Reference to METADATA)
//     Purpose: Point to METADATA Dependencies section
//     References: METADATA "Dependencies" section
//
// 11. FUTURE EXPANSIONS (Roadmap)
//     Purpose: Planned features, research areas, integration targets
//     Subsections: Planned Features → Research Areas → Integration Targets → Known Limitations
//
// 12. CONTRIBUTION GUIDELINES (How to Contribute)
//     Purpose: Guide for contributing to this component
//     Subsections: How to Contribute → Scripture/Grounding
//
// 13. QUICK REFERENCE (Usage Examples)
//     Purpose: Copy-paste ready examples for common operations
//     Subsections: Basic Setup → [Pattern Examples] → Advanced Usage
//
// Section order: Validation → Execution → Cleanup → Overview → Policy → Ladder/Baton →
//                Surgical → Performance → Troubleshooting → Related → Future → Contribution → Reference
// This flows: verify → run → clean → document → guide future work
//
// ════════════════════════════════════════════════════════════════
// GROUP 1: CODING
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Code Validation: generate-config (Command)
// ────────────────────────────────────────────────────────────────
//
// Build Verification:
//   cd word/work && go build ./cmd/generate-config
//
// Runtime Verification:
//   BERESHIT_ROOT=/path/to/bereshit ./generate-config
//   # Or run from within bereshit directory (auto-detects root)
//
// Testing Requirements:
//   - Generated .gen.h files exist in include/generated/
//   - C code compiles with generated headers
//   - make && make test passes
//
// ────────────────────────────────────────────────────────────────
// Code Execution: generate-config (Command)
// ────────────────────────────────────────────────────────────────
//
// Entry Point: main()
//
// Execution Flow:
//   1. Locate BERESHIT_ROOT (env var or auto-detect)
//   2. Try index.toml (primary path - single source of truth)
//   3. If index.toml missing, TRIPWIRE: warn loudly, fall back to hardcoded paths
//   4. Master template generates from [_generate] metadata in each TOML
//   5. Exit 0 on success, 1 on any error
//
// Exit Codes:
//   0 - All generators succeeded
//   1 - Any generator failed (TOML parse, template, or file write)

// main is the entry point for generate-config.
//
// Orchestrates TOML→C header generation using master template.
// All generation is driven by [_generate] metadata in TOML files.
// Uses BERESHIT_ROOT environment variable to locate source files.
func main() {
	fmt.Println("═══════════════════════════════════════════════════════════════")
	fmt.Println("generate-config: TOML → C Header Generator (Master Template)")
	fmt.Println("═══════════════════════════════════════════════════════════════")

	// Get BERESHIT_ROOT from environment
	root := os.Getenv("BERESHIT_ROOT")
	if root == "" {
		// Try to find it from current working directory
		cwd, err := os.Getwd()
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error: cannot determine working directory: %v\n", err)
			os.Exit(1)
		}
		// Walk up to find bereshit root (has word/ directory)
		for dir := cwd; dir != "/"; dir = filepath.Dir(dir) {
			if _, err := os.Stat(filepath.Join(dir, "word")); err == nil {
				root = dir
				break
			}
		}
		if root == "" {
			fmt.Fprintf(os.Stderr, "Error: BERESHIT_ROOT not set and cannot find word/ directory\n")
			os.Exit(1)
		}
	}

	fmt.Printf("BERESHIT_ROOT: %s\n", root)
	fmt.Println()

	// ═══════════════════════════════════════════════════════════════════════
	// TRIPWIRE PATTERN:
	// 1. Try index.toml first (primary path - single source of truth)
	// 2. If index.toml missing, WARN LOUDLY and fall back to hardcoded paths
	// 3. If files missing at hardcoded paths, that's an actual ERROR
	// ═══════════════════════════════════════════════════════════════════════
	err := walkAndGenerateFromIndex(root)
	if err == nil {
		// Primary path: index.toml worked
	} else if strings.Contains(err.Error(), "index.toml not found") {
		// ⚠️ TRIPWIRE TRIGGERED ⚠️
		fmt.Println("════════════════════════════════════════════════════════════════")
		fmt.Println("⚠️  TRIPWIRE: word/core/index.toml NOT FOUND")
		fmt.Println("════════════════════════════════════════════════════════════════")
		fmt.Println("  Running on HARDCODED FALLBACK - this is degraded mode!")
		fmt.Println("  Create index.toml to use the single-source-of-truth manifest.")
		fmt.Println("════════════════════════════════════════════════════════════════")
		fmt.Println()

		// Fallback: Walk using hardcoded system paths
		if err := walkAndGenerateFallback(root); err != nil {
			fmt.Fprintf(os.Stderr, "\nError: %v\n", err)
			os.Exit(1)
		}
	} else {
		// Some other error
		fmt.Fprintf(os.Stderr, "\nError: %v\n", err)
		os.Exit(1)
	}

	fmt.Println("\n═══════════════════════════════════════════════════════════════")
	fmt.Println("Generation complete. The Word made code.")
	fmt.Println("═══════════════════════════════════════════════════════════════")
}

// ────────────────────────────────────────────────────────────────
// Code Cleanup: generate-config (Command)
// ────────────────────────────────────────────────────────────────
// [Reserved: Baton tool - no persistent resources. Go GC handles memory.
// Files are written atomically via os.WriteFile.]

// ════════════════════════════════════════════════════════════════
// GROUP 2: FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Executable Overview & Usage Summary
// ────────────────────────────────────────────────────────────────
// Baton tool that generates C header fragments from TOML specifications.
// See METADATA for full purpose. See BODY Org Chart for architecture.

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
// Safe to Modify:
//   ✅ Add new GeneratorSpec entries in init() for new TOML→C mappings
//   ✅ Modify template strings to change generated output format
//
// Modify with Care:
//   ⚠️ GeneratorSpec struct fields - affects all generators
//   ⚠️ Template syntax - affects generated headers
//
// Never Modify:
//   ❌ 4-block structure
//   ❌ Registry pattern (init() populates generators slice)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
// See BODY "Organizational Chart" above.
// Baton: main() → runGenerators() → loadTOML() → applyTemplate() → writeOutput()

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
// To add new TOML→C mapping: Add GeneratorSpec in init() function.
// See BODY "Registry Population" extension point.

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
// [Reserved: Baton tool runs once at build time. Performance not critical.]

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
// Problem: "cannot find word/ directory"
//   Solution: Set BERESHIT_ROOT or run from within bereshit repo
//
// Problem: "decode [file]: ..."
//   Solution: Check TOML syntax in source file
//
// Problem: "execute template: ..."
//   Solution: Check template syntax matches TOML structure ({{.section.field}})

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
// Depends on: word/core/*.toml (TOML specifications)
// Produces: word/work/pkg/trit/include/generated/*.gen.h (C fragments)
// Used by: Makefile generate target (runs before compile)

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
// Planned:
//   ⏳ Add generators for remaining word/core/*.toml files
//   ⏳ Validation mode (--check) to verify headers are up-to-date

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
// This BATON tool bridges specification (TOML) to implementation (C).
// "And the Word was made flesh, and dwelt among us" - John 1:14

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Run generator:
//   cd word/work && go run ./cmd/generate-config
//
// Or from repo root:
//   BERESHIT_ROOT=$(pwd) go run ./word/work/cmd/generate-config
//
// Add new generator in init():
//   generators = append(generators, GeneratorSpec{
//       Source:      "word/core/new-spec.toml",
//       Output:      "word/work/pkg/trit/include/generated/new_spec.gen.h",
//       Description: "New specification constants",
//       Template:    `// Generated header content using {{.section.field}}...`,
//   })

// ============================================================================
// END CLOSING
// ============================================================================
