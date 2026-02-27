// ============================================================================
// METADATA - CPI-SI Structured Output
// ============================================================================
//
// Key: pkg-agent-output
// Purpose: Response formatting and structured data extraction. The agent can
//          request specific output shapes from the provider — JSON, lists,
//          key-value pairs — and validate the response against a schema.
// Biblical: Proverbs 25:11 — "A word fitly spoken is like apples of gold
//           in pictures of silver"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   OutputFormat defines what shape the response should take.
//   The agent wraps the user prompt with formatting instructions.
//   After receiving the response, it extracts and validates the data.
//
//   Format(prompt, schema) → enhanced prompt → provider → raw response → Parse → typed result
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"encoding/json"
	"fmt"
	"strings"
)

// ============================================================================
// BODY
// ============================================================================

// OutputFormat specifies what shape the agent should request from the provider.
type OutputFormat int

const (
	// FormatText is the default — no formatting constraints.
	FormatText OutputFormat = iota

	// FormatJSON requests a JSON object response.
	FormatJSON

	// FormatJSONArray requests a JSON array response.
	FormatJSONArray

	// FormatList requests a newline-separated list.
	FormatList

	// FormatKeyValue requests key: value pairs.
	FormatKeyValue

	// FormatBool requests a yes/no boolean answer.
	FormatBool
)

// String returns the format name.
func (f OutputFormat) String() string {
	switch f {
	case FormatText:
		return "text"
	case FormatJSON:
		return "json"
	case FormatJSONArray:
		return "json_array"
	case FormatList:
		return "list"
	case FormatKeyValue:
		return "key_value"
	case FormatBool:
		return "bool"
	default:
		return "unknown"
	}
}

// OutputSchema describes the expected structure of a response.
type OutputSchema struct {
	// Format is the expected output shape.
	Format OutputFormat

	// Description explains what the output should contain (optional).
	Description string

	// Fields lists expected JSON fields (FormatJSON only).
	Fields []SchemaField

	// Example shows what the output should look like (optional).
	Example string
}

// SchemaField describes a single field in a JSON schema.
type SchemaField struct {
	Name        string `json:"name"`
	Type        string `json:"type"` // "string", "number", "boolean", "array", "object"
	Description string `json:"description,omitempty"`
	Required    bool   `json:"required,omitempty"`
}

// StructuredResult holds the parsed output from a structured request.
type StructuredResult struct {
	// Raw is the original text response from the provider.
	Raw string

	// Format is the output format that was requested.
	Format OutputFormat

	// JSON holds the parsed JSON data (FormatJSON/FormatJSONArray).
	JSON any

	// List holds parsed list items (FormatList).
	List []string

	// KeyValues holds parsed key-value pairs (FormatKeyValue).
	KeyValues map[string]string

	// Bool holds the parsed boolean (FormatBool).
	Bool bool

	// Usage tracks the underlying RunResult.
	Usage *RunResult
}

// --- Formatting ---

// FormatPrompt wraps a prompt with formatting instructions based on the schema.
func FormatPrompt(prompt string, schema OutputSchema) string {
	var sb strings.Builder
	sb.WriteString(prompt)

	switch schema.Format {
	case FormatText:
		// No formatting needed
		return prompt

	case FormatJSON:
		sb.WriteString("\n\nRespond with a JSON object only, no other text.")
		if len(schema.Fields) > 0 {
			sb.WriteString(" The JSON must have these fields:\n")
			for _, f := range schema.Fields {
				req := ""
				if f.Required {
					req = " (required)"
				}
				sb.WriteString(fmt.Sprintf("- %q (%s)%s", f.Name, f.Type, req))
				if f.Description != "" {
					sb.WriteString(": " + f.Description)
				}
				sb.WriteString("\n")
			}
		}

	case FormatJSONArray:
		sb.WriteString("\n\nRespond with a JSON array only, no other text.")

	case FormatList:
		sb.WriteString("\n\nRespond with a plain list, one item per line, no numbering or bullets.")

	case FormatKeyValue:
		sb.WriteString("\n\nRespond with key-value pairs, one per line, in the format: key: value")

	case FormatBool:
		sb.WriteString("\n\nRespond with only 'yes' or 'no', nothing else.")
	}

	if schema.Description != "" {
		sb.WriteString("\n" + schema.Description)
	}

	if schema.Example != "" {
		sb.WriteString("\n\nExample output:\n" + schema.Example)
	}

	return sb.String()
}

// --- Parsing ---

// ParseResponse extracts structured data from a provider response.
func ParseResponse(raw string, format OutputFormat) (*StructuredResult, error) {
	result := &StructuredResult{
		Raw:    raw,
		Format: format,
	}

	switch format {
	case FormatText:
		// Nothing to parse
		return result, nil

	case FormatJSON:
		cleaned := extractJSON(raw)
		var data map[string]any
		if err := json.Unmarshal([]byte(cleaned), &data); err != nil {
			return result, fmt.Errorf("expected JSON object: %w", err)
		}
		result.JSON = data
		return result, nil

	case FormatJSONArray:
		cleaned := extractJSON(raw)
		var data []any
		if err := json.Unmarshal([]byte(cleaned), &data); err != nil {
			return result, fmt.Errorf("expected JSON array: %w", err)
		}
		result.JSON = data
		return result, nil

	case FormatList:
		result.List = parseList(raw)
		return result, nil

	case FormatKeyValue:
		result.KeyValues = parseKeyValues(raw)
		return result, nil

	case FormatBool:
		lower := strings.ToLower(strings.TrimSpace(raw))
		result.Bool = lower == "yes" || lower == "true" || lower == "y"
		return result, nil

	default:
		return result, fmt.Errorf("unknown format: %d", format)
	}
}

// --- Agent Integration ---

// RunStructured sends a prompt with an output schema and parses the result.
func (a *Agent) RunStructured(ctx context.Context, prompt string, schema OutputSchema) (*StructuredResult, error) {
	formatted := FormatPrompt(prompt, schema)
	runResult, err := a.Run(ctx, formatted)
	if err != nil {
		return nil, err
	}

	result, err := ParseResponse(runResult.Content, schema.Format)
	if err != nil {
		return result, err // Return partial result with error
	}
	result.Usage = runResult
	return result, nil
}

// RunJSON is a convenience for requesting a JSON object response.
func (a *Agent) RunJSON(ctx context.Context, prompt string, fields ...SchemaField) (*StructuredResult, error) {
	return a.RunStructured(ctx, prompt, OutputSchema{
		Format: FormatJSON,
		Fields: fields,
	})
}

// RunBool is a convenience for requesting a yes/no answer.
func (a *Agent) RunBool(ctx context.Context, prompt string) (bool, error) {
	result, err := a.RunStructured(ctx, prompt, OutputSchema{
		Format: FormatBool,
	})
	if err != nil {
		return false, err
	}
	return result.Bool, nil
}

// RunList is a convenience for requesting a list response.
func (a *Agent) RunList(ctx context.Context, prompt string) ([]string, error) {
	result, err := a.RunStructured(ctx, prompt, OutputSchema{
		Format: FormatList,
	})
	if err != nil {
		return nil, err
	}
	return result.List, nil
}

// --- Helpers ---

// extractJSON finds the first JSON object or array in a string.
// Models sometimes wrap JSON in markdown code fences or add preamble text.
func extractJSON(s string) string {
	s = strings.TrimSpace(s)

	// Strip markdown code fences
	if strings.HasPrefix(s, "```") {
		lines := strings.Split(s, "\n")
		// Find content between fences
		start := 1
		end := len(lines)
		for i := len(lines) - 1; i >= 1; i-- {
			if strings.HasPrefix(strings.TrimSpace(lines[i]), "```") {
				end = i
				break
			}
		}
		s = strings.TrimSpace(strings.Join(lines[start:end], "\n"))
	}

	// Find first { or [
	objStart := strings.IndexByte(s, '{')
	arrStart := strings.IndexByte(s, '[')

	if objStart < 0 && arrStart < 0 {
		return s // Return as-is, let json.Unmarshal report the error
	}

	// Use whichever appears first
	start := objStart
	end := byte('}')
	if arrStart >= 0 && (objStart < 0 || arrStart < objStart) {
		start = arrStart
		end = byte(']')
	}

	// Find matching close bracket, counting nesting
	depth := 0
	open := s[start]
	for i := start; i < len(s); i++ {
		if s[i] == open {
			depth++
		} else if s[i] == end {
			depth--
			if depth == 0 {
				return s[start : i+1]
			}
		}
	}

	// Unmatched brackets — return from start
	return s[start:]
}

// parseList splits text into non-empty, trimmed lines.
func parseList(s string) []string {
	lines := strings.Split(s, "\n")
	var result []string
	for _, line := range lines {
		line = strings.TrimSpace(line)
		// Strip common list prefixes
		line = strings.TrimPrefix(line, "- ")
		line = strings.TrimPrefix(line, "* ")
		// Strip numeric prefixes like "1. ", "2) "
		if len(line) > 2 {
			for i, c := range line {
				if c >= '0' && c <= '9' {
					continue
				}
				if (c == '.' || c == ')') && i > 0 && i < len(line)-1 && line[i+1] == ' ' {
					line = strings.TrimSpace(line[i+2:])
				}
				break
			}
		}
		if line != "" {
			result = append(result, line)
		}
	}
	return result
}

// parseKeyValues splits text into key: value pairs.
func parseKeyValues(s string) map[string]string {
	result := make(map[string]string)
	lines := strings.Split(s, "\n")
	for _, line := range lines {
		line = strings.TrimSpace(line)
		if idx := strings.Index(line, ": "); idx > 0 {
			key := strings.TrimSpace(line[:idx])
			val := strings.TrimSpace(line[idx+2:])
			if key != "" {
				result[key] = val
			}
		}
	}
	return result
}

// ============================================================================
// CLOSING
// ============================================================================
// "A word fitly spoken is like apples of gold in pictures of silver."
// — Proverbs 25:11
// Structure gives words their setting — structured output ensures
// the agent's responses serve their purpose with precision.
