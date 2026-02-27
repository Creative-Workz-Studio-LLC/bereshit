// ============================================================================
// METADATA - CPI-SI Agent Tool System
// ============================================================================
//
// Key: pkg-agent-tools
// Purpose: Tools the CPI-SI agent can use — file operations, shell, search.
//          All built on Go stdlib. No external dependencies.
// Biblical: Exodus 31:3 — "And I have filled him with the spirit of God,
//           in wisdom, and in understanding, and in knowledge"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"bytes"
	"context"
	"encoding/json"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"strings"
	"time"
)

// ============================================================================
// BODY
// ============================================================================

// Tool is the interface for executable tools.
type Tool interface {
	// Definition returns the tool's schema for the model.
	Definition() ToolDef

	// Execute runs the tool with the given JSON arguments.
	Execute(ctx context.Context, args string) (string, error)
}

// --- File Read Tool ---

type fileReadTool struct{}

func (t *fileReadTool) Definition() ToolDef {
	return ToolDef{
		Name:        "read_file",
		Description: "Read the contents of a file at the given path.",
		Parameters: map[string]any{
			"type": "object",
			"properties": map[string]any{
				"path": map[string]any{
					"type":        "string",
					"description": "Absolute path to the file to read",
				},
			},
			"required": []string{"path"},
		},
	}
}

func (t *fileReadTool) Execute(_ context.Context, args string) (string, error) {
	var params struct {
		Path string `json:"path"`
	}
	if err := json.Unmarshal([]byte(args), &params); err != nil {
		return "", fmt.Errorf("invalid arguments: %w", err)
	}
	data, err := os.ReadFile(params.Path)
	if err != nil {
		return "", fmt.Errorf("read %s: %w", params.Path, err)
	}
	// Truncate very large files
	const maxBytes = 100_000
	if len(data) > maxBytes {
		return string(data[:maxBytes]) + "\n... (truncated)", nil
	}
	return string(data), nil
}

// --- File Write Tool ---

type fileWriteTool struct{}

func (t *fileWriteTool) Definition() ToolDef {
	return ToolDef{
		Name:        "write_file",
		Description: "Write content to a file. Creates parent directories if needed.",
		Parameters: map[string]any{
			"type": "object",
			"properties": map[string]any{
				"path": map[string]any{
					"type":        "string",
					"description": "Absolute path to write to",
				},
				"content": map[string]any{
					"type":        "string",
					"description": "Content to write",
				},
			},
			"required": []string{"path", "content"},
		},
	}
}

func (t *fileWriteTool) Execute(_ context.Context, args string) (string, error) {
	var params struct {
		Path    string `json:"path"`
		Content string `json:"content"`
	}
	if err := json.Unmarshal([]byte(args), &params); err != nil {
		return "", fmt.Errorf("invalid arguments: %w", err)
	}
	dir := filepath.Dir(params.Path)
	if err := os.MkdirAll(dir, 0o755); err != nil {
		return "", fmt.Errorf("create directory %s: %w", dir, err)
	}
	// Atomic write: temp file + rename
	tmp := params.Path + ".tmp"
	if err := os.WriteFile(tmp, []byte(params.Content), 0o644); err != nil {
		return "", fmt.Errorf("write %s: %w", tmp, err)
	}
	if err := os.Rename(tmp, params.Path); err != nil {
		os.Remove(tmp)
		return "", fmt.Errorf("rename %s: %w", params.Path, err)
	}
	return fmt.Sprintf("Wrote %d bytes to %s", len(params.Content), params.Path), nil
}

// --- Shell Execute Tool ---

type shellTool struct{}

func (t *shellTool) Definition() ToolDef {
	return ToolDef{
		Name:        "shell",
		Description: "Execute a shell command and return its output.",
		Parameters: map[string]any{
			"type": "object",
			"properties": map[string]any{
				"command": map[string]any{
					"type":        "string",
					"description": "The command to execute",
				},
			},
			"required": []string{"command"},
		},
	}
}

func (t *shellTool) Execute(ctx context.Context, args string) (string, error) {
	var params struct {
		Command string `json:"command"`
	}
	if err := json.Unmarshal([]byte(args), &params); err != nil {
		return "", fmt.Errorf("invalid arguments: %w", err)
	}

	ctx, cancel := context.WithTimeout(ctx, 2*time.Minute)
	defer cancel()

	cmd := exec.CommandContext(ctx, "bash", "-c", params.Command)
	var stdout, stderr bytes.Buffer
	cmd.Stdout = &stdout
	cmd.Stderr = &stderr

	err := cmd.Run()
	result := stdout.String()
	if stderr.Len() > 0 {
		result += "\nSTDERR:\n" + stderr.String()
	}
	if err != nil {
		result += "\nERROR: " + err.Error()
	}

	// Truncate large output
	const maxBytes = 50_000
	if len(result) > maxBytes {
		result = result[:maxBytes] + "\n... (truncated)"
	}
	return result, nil
}

// --- Grep Tool ---

type grepTool struct{}

func (t *grepTool) Definition() ToolDef {
	return ToolDef{
		Name:        "search",
		Description: "Search file contents for a regex pattern. Returns matching lines with file paths and line numbers.",
		Parameters: map[string]any{
			"type": "object",
			"properties": map[string]any{
				"pattern": map[string]any{
					"type":        "string",
					"description": "Regex pattern to search for",
				},
				"path": map[string]any{
					"type":        "string",
					"description": "Directory or file to search in",
				},
				"glob": map[string]any{
					"type":        "string",
					"description": "File glob pattern (e.g., *.go)",
				},
			},
			"required": []string{"pattern", "path"},
		},
	}
}

func (t *grepTool) Execute(ctx context.Context, args string) (string, error) {
	var params struct {
		Pattern string `json:"pattern"`
		Path    string `json:"path"`
		Glob    string `json:"glob"`
	}
	if err := json.Unmarshal([]byte(args), &params); err != nil {
		return "", fmt.Errorf("invalid arguments: %w", err)
	}

	re, err := regexp.Compile(params.Pattern)
	if err != nil {
		return "", fmt.Errorf("invalid regex: %w", err)
	}

	var matches []string
	const maxMatches = 100

	filepath.Walk(params.Path, func(path string, info os.FileInfo, err error) error {
		if err != nil || info.IsDir() {
			return nil
		}
		if params.Glob != "" {
			matched, _ := filepath.Match(params.Glob, filepath.Base(path))
			if !matched {
				return nil
			}
		}
		data, err := os.ReadFile(path)
		if err != nil {
			return nil
		}
		lines := strings.Split(string(data), "\n")
		for i, line := range lines {
			if re.MatchString(line) {
				matches = append(matches, fmt.Sprintf("%s:%d: %s", path, i+1, line))
				if len(matches) >= maxMatches {
					return filepath.SkipAll
				}
			}
		}
		return nil
	})

	if len(matches) == 0 {
		return "No matches found.", nil
	}
	return strings.Join(matches, "\n"), nil
}

// --- List Directory Tool ---

type listDirTool struct{}

func (t *listDirTool) Definition() ToolDef {
	return ToolDef{
		Name:        "list_dir",
		Description: "List files and directories at a given path.",
		Parameters: map[string]any{
			"type": "object",
			"properties": map[string]any{
				"path": map[string]any{
					"type":        "string",
					"description": "Directory path to list",
				},
			},
			"required": []string{"path"},
		},
	}
}

func (t *listDirTool) Execute(_ context.Context, args string) (string, error) {
	var params struct {
		Path string `json:"path"`
	}
	if err := json.Unmarshal([]byte(args), &params); err != nil {
		return "", fmt.Errorf("invalid arguments: %w", err)
	}
	entries, err := os.ReadDir(params.Path)
	if err != nil {
		return "", fmt.Errorf("list %s: %w", params.Path, err)
	}
	var lines []string
	for _, e := range entries {
		info, _ := e.Info()
		if info != nil {
			lines = append(lines, fmt.Sprintf("%s %8d %s", info.Mode(), info.Size(), e.Name()))
		} else {
			lines = append(lines, e.Name())
		}
	}
	return strings.Join(lines, "\n"), nil
}

// --- File Edit Tool ---

type fileEditTool struct{}

func (t *fileEditTool) Definition() ToolDef {
	return ToolDef{
		Name:        "edit_file",
		Description: "Edit a file by replacing an exact string with new content. The old_string must appear exactly once in the file.",
		Parameters: map[string]any{
			"type": "object",
			"properties": map[string]any{
				"path": map[string]any{
					"type":        "string",
					"description": "Absolute path to the file to edit",
				},
				"old_string": map[string]any{
					"type":        "string",
					"description": "The exact string to find and replace (must be unique in the file)",
				},
				"new_string": map[string]any{
					"type":        "string",
					"description": "The replacement string",
				},
			},
			"required": []string{"path", "old_string", "new_string"},
		},
	}
}

func (t *fileEditTool) Execute(_ context.Context, args string) (string, error) {
	var params struct {
		Path      string `json:"path"`
		OldString string `json:"old_string"`
		NewString string `json:"new_string"`
	}
	if err := json.Unmarshal([]byte(args), &params); err != nil {
		return "", fmt.Errorf("invalid arguments: %w", err)
	}

	data, err := os.ReadFile(params.Path)
	if err != nil {
		return "", fmt.Errorf("read %s: %w", params.Path, err)
	}

	content := string(data)
	count := strings.Count(content, params.OldString)
	if count == 0 {
		return "", fmt.Errorf("old_string not found in %s", params.Path)
	}
	if count > 1 {
		return "", fmt.Errorf("old_string appears %d times in %s — must be unique", count, params.Path)
	}

	newContent := strings.Replace(content, params.OldString, params.NewString, 1)

	// Atomic write
	tmp := params.Path + ".tmp"
	if err := os.WriteFile(tmp, []byte(newContent), 0o644); err != nil {
		return "", fmt.Errorf("write %s: %w", tmp, err)
	}
	if err := os.Rename(tmp, params.Path); err != nil {
		os.Remove(tmp)
		return "", fmt.Errorf("rename %s: %w", params.Path, err)
	}

	return fmt.Sprintf("Edited %s: replaced %d bytes with %d bytes", params.Path, len(params.OldString), len(params.NewString)), nil
}

// --- Glob Tool ---

type globTool struct{}

func (t *globTool) Definition() ToolDef {
	return ToolDef{
		Name:        "find_files",
		Description: "Find files matching a glob pattern (e.g., **/*.go, src/**/*.ts). Returns matching file paths.",
		Parameters: map[string]any{
			"type": "object",
			"properties": map[string]any{
				"pattern": map[string]any{
					"type":        "string",
					"description": "Glob pattern to match files (e.g., **/*.go)",
				},
				"path": map[string]any{
					"type":        "string",
					"description": "Root directory to search from",
				},
			},
			"required": []string{"pattern", "path"},
		},
	}
}

func (t *globTool) Execute(_ context.Context, args string) (string, error) {
	var params struct {
		Pattern string `json:"pattern"`
		Path    string `json:"path"`
	}
	if err := json.Unmarshal([]byte(args), &params); err != nil {
		return "", fmt.Errorf("invalid arguments: %w", err)
	}

	var matches []string
	const maxMatches = 200

	filepath.Walk(params.Path, func(path string, info os.FileInfo, err error) error {
		if err != nil || info.IsDir() {
			return nil
		}
		// Match against just the filename for simple patterns, full path for ** patterns
		matched := false
		if strings.Contains(params.Pattern, "/") || strings.Contains(params.Pattern, "**") {
			// For path patterns, try matching the relative path
			rel, _ := filepath.Rel(params.Path, path)
			matched, _ = filepath.Match(params.Pattern, rel)
			if !matched {
				// Also try just the base name component
				matched, _ = filepath.Match(filepath.Base(params.Pattern), filepath.Base(path))
			}
		} else {
			matched, _ = filepath.Match(params.Pattern, filepath.Base(path))
		}
		if matched {
			matches = append(matches, path)
			if len(matches) >= maxMatches {
				return filepath.SkipAll
			}
		}
		return nil
	})

	if len(matches) == 0 {
		return "No files found matching pattern.", nil
	}
	return strings.Join(matches, "\n"), nil
}

// DefaultTools returns the standard tool set for the CPI-SI agent.
func DefaultTools() []Tool {
	return []Tool{
		&fileReadTool{},
		&fileWriteTool{},
		&fileEditTool{},
		&shellTool{},
		&grepTool{},
		&globTool{},
		&listDirTool{},
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "I have filled him with the spirit of God, in wisdom, and in understanding,
//  and in knowledge, and in all manner of workmanship." — Exodus 31:3
