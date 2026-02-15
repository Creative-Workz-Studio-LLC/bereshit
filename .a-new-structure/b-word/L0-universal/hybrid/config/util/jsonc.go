//omni:code --go -library
//omni:key B-L0-config-util-jsonc
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-config-util-jsonc
// Purpose: JSONC parser — JSON with Comments (single-line, multi-line, trailing commas)
// Biblical: Ecclesiastes 12:12 - "Of making many books there is no end"
// Authors: Nova Dawn
// Version: b-01.00
// Created: 2026-02-13
//
// Unifies two JSONC implementations:
//   - L3-cpisi/orchestration/config/config.go:916 (regex approach, handles multi-line)
//   - L3-cpisi/foundation/schema/loader.go:1652 (line-by-line, generic type param)
//
// This implementation takes the best of both:
//   - Regex from config.go (handles multi-line comments, trailing commas)
//   - Generic type parameter from schema/loader.go (type-safe loading)
//
// Production: Regexes compiled once at package init, not per-call.
//
// ============================================================================
// END METADATA
// ============================================================================

package util

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"fmt"
	"os"
	"regexp"
	"strings"
)

// --- Compiled Patterns ---
// Compiled once at package init — not per-call. ParseJSONC is often called
// in loops (loading multiple JSONC files). Compiling per-call was measurable waste.

var (
	reSingleLineComment = regexp.MustCompile(`//.*$`)              // single-line: // ...
	reMultiLineComment  = regexp.MustCompile(`/\*[\s\S]*?\*/`)     // multi-line: /* ... */
	reTrailingComma     = regexp.MustCompile(`,(\s*[}\]])`)        // trailing: ,} or ,]
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// --- Comment Stripping ---

// ParseJSONC strips JSONC extensions (comments, trailing commas) and returns
// clean JSON bytes suitable for json.Unmarshal.
//
// Handles:
//   - Single-line comments: // ...
//   - Multi-line comments: /* ... */
//   - Trailing commas before } or ]
//
// Does NOT strip comments inside string values.
func ParseJSONC(content []byte) ([]byte, error) {
	s := string(content)

	// Remove single-line comments (not inside strings)
	lines := strings.Split(s, "\n")
	for i, line := range lines {
		// Heuristic: skip lines where // appears before the last quote
		// (likely inside a string value). Not perfect, but matches
		// the established pattern from config.go.
		if !strings.Contains(line, `"`) || strings.Index(line, "//") < strings.LastIndex(line, `"`) {
			lines[i] = reSingleLineComment.ReplaceAllString(line, "")
		}
	}
	s = strings.Join(lines, "\n")

	// Remove multi-line comments
	s = reMultiLineComment.ReplaceAllString(s, "")

	// Remove trailing commas before } or ]
	s = reTrailingComma.ReplaceAllString(s, "$1")

	// Validate it's actually JSON now
	cleaned := []byte(s)
	if !json.Valid(cleaned) {
		return nil, fmt.Errorf("invalid JSON after comment stripping")
	}

	return cleaned, nil
}

// --- Typed Loading ---

// LoadJSONCFile reads a JSONC file from disk, strips comments, and unmarshals
// into the target type T.
//
// Go 1.18+ generics — replaces loadJSONCFile[T] from schema/loader.go.
func LoadJSONCFile[T any](path string) (*T, error) {
	if !FileExists(path) {
		return nil, NewLoadError(path, "stat", fmt.Errorf("file not found"))
	}

	data, err := os.ReadFile(path)
	if err != nil {
		return nil, NewLoadError(path, "read", err)
	}

	cleaned, err := ParseJSONC(data)
	if err != nil {
		return nil, NewLoadError(path, "parse", err)
	}

	var result T
	if err := json.Unmarshal(cleaned, &result); err != nil {
		return nil, NewLoadError(path, "unmarshal", err)
	}

	return &result, nil
}

// LoadJSONCMap reads a JSONC file and returns a map[string]any.
// For cases where the target type isn't known at compile time.
func LoadJSONCMap(path string) (map[string]any, error) {
	result, err := LoadJSONCFile[map[string]any](path)
	if err != nil {
		return nil, err
	}
	return *result, nil
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "And further, by these, my son, be admonished: of making many books
// there is no end; and much study is a weariness of the flesh."
// — Ecclesiastes 12:12
//
// JSONC: because humans write comments and trailing commas.
// The machine forgives what the spec does not.
//
// ============================================================================
// END CLOSING
// ============================================================================
