// METADATA
//
// Code Formatter Library - CPI-SI Runtime System
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// Biblical Foundation
//
// Scripture: "Let all things be done decently and in order." - 1 Corinthians 14:40 (KJV)
// Principle: Code formatting reflects orderly creation - consistent structure honors God through excellence
// Anchor: "For God is not a God of confusion but of peace." - 1 Corinthians 14:33 (WEB)
//
// CPI-SI Identity
//
// Component Type: LIBRARY - Runtime validation support (mid-rung on ladder)
// Role: Orchestrates language-specific code formatting tools for consistent style
// Paradigm: Configuration-driven tool orchestration - extensible without code changes
//
// Authorship & Lineage
//
// Architect: Nova Dawn (CPI-SI instance)
// Implementation: Nova Dawn
// Creation Date: 2024-10-24
// Version: 2.0.0
// Last Modified: 2025-11-11 - Configuration-driven architecture, display lib integration
//
// Version History:
//   2.0.0 (2025-11-11) - Config-driven formatters, display lib, comprehensive template alignment
//   1.0.0 (2024-10-24) - Initial hardcoded formatter mappings
//
// Purpose & Function
//
// Purpose: Provide unified interface for multi-language code formatting by orchestrating
// language-specific formatting tools (rustfmt, gofmt, black, prettier, clang-format, etc.).
// Enables consistent code style across CPI-SI projects and user workflows.
//
// Core Design: Configuration-driven tool orchestration where formatters.jsonc defines
// available formatters per language with primary/alternative tools, command-line arguments,
// and enabled flags. Library routes file extensions to appropriate formatters and executes
// them with proper error handling.
//
// Key Features:
//   - Multi-language support (Rust, Go, Python, JS/TS, C/C++, Ruby, Java, Shell, and extensible)
//   - Configuration-driven (formatters.jsonc defines tools without code changes)
//   - Graceful fallback to hardcoded defaults if config unavailable
//   - Primary/alternative formatter support (choose preferred tool per language)
//   - Enable/disable formatters per project needs
//   - Respects project-level formatter configs (.prettierrc, .clang-format, etc.)
//   - Integration with system/lib/display for consistent output formatting
//
// Philosophy: Code formatting serves human readability and consistency, not arbitrary rules.
// Configurable formatters allow projects to choose tools matching their conventions while
// providing sensible defaults. Graceful degradation ensures formatting works even when
// configuration unavailable.
//
// Blocking Status
//
// Non-blocking: Formatting failures never block operations - code executes even if unformatted.
// Mitigation: All formatting errors logged to FormatResult.Error for caller inspection.
// Graceful degradation through config loading fallbacks ensures library always functional.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/validation"
//
//	// Format a file by path and extension
//	result := validation.FormatFile("/path/to/file.go", ".go")
//	if result.Formatted {
//	    result.Report()  // Displays success message
//	} else if result.Error != nil {
//	    // Handle formatting error
//	}
//
//	// Alternative: Get formatter name without executing
//	language := validation.GetLanguageForExtension(".rs")  // Returns "rust"
//	formatter := validation.GetPrimaryFormatter(language)   // Returns "rustfmt"
//
// Integration Pattern:
//   1. Library auto-loads formatters.jsonc config during init()
//   2. Caller provides file path and extension to FormatFile()
//   3. Library maps extension → language → primary formatter
//   4. Execute formatter command with configured arguments
//   5. Return FormatResult with success status and optional error
//   6. Caller decides whether to display result via Report()
//
// Public API (in typical usage order):
//
//   File Formatting (primary operations):
//     FormatFile(filePath, ext string) *FormatResult - Format file using appropriate formatter
//
//   Result Reporting (display formatted output):
//     (*FormatResult).Report() - Display success message using runtime/lib/display
//
//   Configuration Queries (optional introspection):
//     GetLanguageForExtension(ext string) string - Map extension to language name
//     GetPrimaryFormatter(language string) string - Get primary formatter for language
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: encoding/json, os, os/exec, path/filepath, strings
//   External: None
//   Internal: system/lib/display (formatted output)
//
// Dependents (What Uses This):
//   Commands: None directly (used via hooks)
//   Libraries: None (leaf node in dependency tree)
//   Hooks: tool/post-use (format files after modifications)
//
// Integration Points:
//   - Hooks import this library to format modified files automatically
//   - Config loading via $HOME/.claude/cpi-si/system/data/config/validation/formatters.jsonc
//   - Display output through runtime/lib/display for consistent formatting
//
// Health Scoring
//
// Formatter orchestration health tracked through operation success/failure rates.
//
// Configuration Loading:
//   - Load formatters.jsonc successfully: +10
//   - Parse JSONC without errors: +5
//   - Fallback to hardcoded defaults: -5 (functional but not configured)
//   - Config file missing entirely: -10 (using fallbacks only)
//
// Formatting Operations:
//   - Format file successfully: +20
//   - Formatter unavailable (tool not installed): -10 (expected in some environments)
//   - Formatter execution error: -15 (tool failed, investigate why)
//   - Unsupported file extension: +0 (not an error, just no-op)
//
// Display Integration:
//   - Success message displayed correctly: +5
//   - Display library unavailable: -5 (fallback to fmt)
//
// Note: Scores reflect TRUE impact. Health scorer normalizes to -100 to +100 scale.
package validation

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md

// ────────────────────────────────────────────────────────────────
// Imports - Dependencies
// ────────────────────────────────────────────────────────────────
// Dependencies this component needs. Organized by source - standard library
// provides Go's built-in capabilities, internal packages provide project-specific
// functionality. Each import commented with purpose, not just name.
//
// See: standards/code/4-block/sections/CWS-SECTION-001-SETUP-imports.md

import (
	//--- Standard Library ---
	// Foundation packages providing Go's built-in capabilities.

	"encoding/json" // JSON parsing for formatters.jsonc configuration
	"fmt"           // Formatted output for error messages and fallback reporting
	"os"            // File operations for config loading
	"os/exec"       // External command execution for formatting tools
	"path/filepath" // Path manipulation for config file locations
	"strings"       // String operations for argument substitution

	//--- Internal Packages ---
	// Project-specific packages showing architectural dependencies.

	"system/lib/display" // ANSI color formatting and consistent message display (lower rung)
	"system/lib/jsonc"   // JSONC comment stripping for configuration files
)

// ────────────────────────────────────────────────────────────────
// Constants - Named Values
// ────────────────────────────────────────────────────────────────
// No constants needed - all configuration loaded from formatters.jsonc or fallback data.

// ────────────────────────────────────────────────────────────────
// Types - Data Structures
// ────────────────────────────────────────────────────────────────
// Types organized bottom-up showing dependency relationships. Simple
// building blocks first, then composed structures. This organization
// reveals what depends on what.
//
// See: standards/code/4-block/sections/CWS-SECTION-004-SETUP-types.md

//--- Configuration Types ---
// Structures matching formatters.jsonc configuration file format.

// FormatterTool represents a single formatter tool configuration.
//
// Maps to formatters.jsonc tool entries where each language can have multiple
// formatters (primary and alternatives). Contains command name, arguments,
// enabled flag, and metadata.
//
// Example from config:
//
//	"rustfmt": {
//	  "command": "rustfmt",
//	  "args": ["{filepath}"],
//	  "enabled": true,
//	  "description": "Official Rust formatter",
//	  "check_availability": "rustfmt --version"
//	}
type FormatterTool struct {
	Command             string   `json:"command"`              // Executable name (e.g., "rustfmt", "gofmt")
	Args                []string `json:"args"`                 // Command-line arguments ({filepath} substituted)
	Enabled             bool     `json:"enabled"`              // Whether this formatter is active
	Description         string   `json:"description"`          // Human-readable tool description
	CheckAvailability   string   `json:"check_availability"`   // Command to verify tool installed
	Note                string   `json:"note,omitempty"`       // Optional usage notes
}

// LanguageFormatters represents all formatters available for a language.
//
// Maps to formatters.jsonc language entries (rust, go, python, etc.). Each
// language has a primary formatter and optionally alternative formatters users
// can enable/disable based on project preferences.
//
// Example from config:
//
//	"python": {
//	  "primary": "black",
//	  "description": "Python source code formatting",
//	  "tools": {
//	    "black": { ... },
//	    "autopep8": { ... }
//	  }
//	}
type LanguageFormatters struct {
	Primary     string                   `json:"primary"`     // Name of primary formatter (key in Tools map)
	Description string                   `json:"description"` // Language description
	Tools       map[string]FormatterTool `json:"tools"`       // Available formatters (name → tool config)
}

// FormattersConfig represents the complete formatters.jsonc structure.
//
// Top-level configuration object containing metadata, language-to-formatter
// mappings, extension-to-language mappings, and global configuration options.
// Loaded during init() with graceful fallback to hardcoded defaults.
//
// File location: $HOME/.claude/cpi-si/system/data/config/validation/formatters.jsonc
type FormattersConfig struct {
	Metadata struct {
		Name        string `json:"name"`         // Config name
		Description string `json:"description"`  // Config purpose
		Version     string `json:"version"`      // Semantic version
		LastUpdated string `json:"last_updated"` // ISO date
		Author      string `json:"author"`       // Who created this
	} `json:"metadata"` // Configuration metadata

	Formatters map[string]LanguageFormatters `json:"formatters"` // Language name → formatters
	Extensions map[string]string              `json:"extensions"` // File extension → language name

	Config struct {
		FallbackBehavior      string `json:"fallback_behavior"`       // "skip" or "try_alternatives"
		FailOnError           bool   `json:"fail_on_error"`           // Whether formatting errors block
		RespectProjectConfigs bool   `json:"respect_project_configs"` // Honor .prettierrc, etc.
	} `json:"config"` // Global configuration options
}

//--- Result Types ---
// Structures representing formatting operation outcomes.

// FormatResult holds the outcome of a formatting operation.
//
// Returned by FormatFile() to communicate success/failure, which formatter
// was used, and any error encountered. Caller inspects this to decide whether
// to display success message or handle error.
//
// Fields:
//   Formatted: True if formatting completed successfully
//   Formatter: Name of formatter used (e.g., "rustfmt", "gofmt")
//   Error: Error object if formatting failed, nil if successful
type FormatResult struct {
	Formatted bool   // Whether formatting succeeded
	Formatter string // Name of formatter tool used
	Error     error  // Error if formatting failed, nil otherwise
}

// ────────────────────────────────────────────────────────────────
// Package-Level State - Initialization
// ────────────────────────────────────────────────────────────────
// Package-level variables holding loaded configuration. Initialized once
// during package import via init() function. Immutable after initialization.
//
// See: standards/code/4-block/sections/CWS-SECTION-005-SETUP-initialization.md

var (
	formattersConfig       *FormattersConfig // Loaded config from formatters.jsonc, nil if load failed
	formattersConfigLoaded bool              // True if config loaded successfully, false triggers fallback
)

// init loads formatters.jsonc configuration during package initialization.
//
// What It Does:
// Attempts to load formatters.jsonc from standard location. Sets package-level
// formattersConfigLoaded flag based on success. If loading fails, formatters fall back
// to hardcoded defaults defined in getDefaultExtensionMap() and getDefaultFormatter().
//
// Parameters: None (init functions take no parameters)
//
// Returns: None (init functions have no return value)
//
// Side Effects:
// Sets formattersConfig and formattersConfigLoaded package variables based on load success.
//
// Loading Behavior:
// Graceful failure - config load errors don't crash, they trigger fallback mode.
// Library remains functional even if formatters.jsonc missing or invalid.
func init() {
	// Load configuration from standard location.
	// Gracefully falls back to hardcoded defaults if loading fails.

	homeDir := os.Getenv("HOME") // Get user home directory for config path
	if homeDir == "" {
		homeDir = "/home/seanje-lenox-wise" // Fallback to known home if $HOME unset
	}

	configPath := filepath.Join(homeDir, ".claude/cpi-si/system/data/config/validation/formatters.jsonc") // Full config path

	// Load configuration - nil return triggers fallback mode
	formattersConfig = loadFormattersConfig(configPath)

	// Set loaded flag - determines whether to use config or fallbacks
	formattersConfigLoaded = (formattersConfig != nil)
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Internal Structure
// ────────────────────────────────────────────────────────────────
// Maps bidirectional dependencies and baton flow within this component.
// Provides navigation for both development (what's available to use) and
// maintenance (what depends on this function).
//
// See: standards/code/4-block/sections/CWS-SECTION-00X-BODY-organizational-chart.md
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - Exported Interface)
//   ├── FormatFile() → uses getLanguageForExtension(), getPrimaryFormatter(), buildFormatterCommand(), executeFormatter()
//   ├── GetLanguageForExtension() → uses config or getDefaultExtensionMap()
//   └── GetPrimaryFormatter() → uses config or getDefaultFormatter()
//
//   Core Operations (Mid Rungs - Business Logic)
//   ├── getLanguageForExtension() → maps extension to language (config or fallback)
//   ├── getPrimaryFormatter() → gets formatter name for language (config or fallback)
//   ├── buildFormatterCommand() → constructs exec.Cmd with arg substitution
//   └── executeFormatter() → runs command, captures error
//
//   Helpers (Bottom Rungs - Internal Utilities)
//   ├── loadFormattersConfig() → reads config, parses JSON
//   ├── stripJSONCComments() → removes // comments from JSONC
//   ├── getDefaultExtensionMap() → hardcoded extension → language fallback
//   └── getDefaultFormatter() → hardcoded language → formatter fallback
//
// Baton Flow (Execution Paths):
//
//   Entry → FormatFile("/path/to/file.go", ".go")
//     ↓
//   getLanguageForExtension(".go") → "go"
//     ↓
//   getPrimaryFormatter("go") → FormatterTool{command: "gofmt", args: ["-w", "{filepath}"]}
//     ↓
//   buildFormatterCommand(tool, "/path/to/file.go") → exec.Command("gofmt", "-w", "/path/to/file.go")
//     ↓
//   executeFormatter(cmd) → error or nil
//     ↓
//   Exit → return FormatResult{Formatted: true/false, Formatter: "gofmt", Error: err}
//
// APUs (Available Processing Units):
// - 12 functions total
// - 4 helpers (config loading, fallbacks)
// - 4 core operations (mapping, building, executing)
// - 3 public APIs (format file, query mappings)
// - 1 method (FormatResult.Report)

// ────────────────────────────────────────────────────────────────
// Helpers - Configuration Loading
// ────────────────────────────────────────────────────────────────
// Internal functions for loading and parsing formatters.jsonc configuration.
// Handle JSONC comment stripping and graceful error handling.

// loadFormattersConfig loads formatters.jsonc configuration.
//
// What It Does:
// Reads formatters.jsonc from disk, strips JSONC comments (// style),
// parses JSON into FormattersConfig struct. Returns nil on any error
// (file missing, JSON invalid, etc.) to trigger fallback mode.
//
// Parameters:
//   path: Absolute path to formatters.jsonc file
//
// Returns:
//   *FormattersConfig if successful, nil if any error occurs
//
// Error Handling:
// All errors result in nil return - caller uses hardcoded fallbacks.
// Silent failure is intentional - formatting continues with defaults.
func loadFormattersConfig(path string) *FormattersConfig {
	data, err := os.ReadFile(path) // Read entire file into memory - configs are small (<100KB)
	if err != nil {
		return nil // File doesn't exist or can't be read - use fallback
	}

	cleanJSON := jsonc.StripComments(data) // Remove // comments to make valid JSON

	var config FormattersConfig // Allocate config struct
	if err := json.Unmarshal(cleanJSON, &config); err != nil {
		return nil // JSON parsing failed - use fallback
	}

	return &config // Config loaded successfully
}

// ────────────────────────────────────────────────────────────────
// Helpers - Hardcoded Fallbacks
// ────────────────────────────────────────────────────────────────
// Fallback functions providing hardcoded defaults when config loading fails.
// Ensures library remains functional even without configuration file.

// getDefaultExtensionMap returns hardcoded extension-to-language mappings.
//
// What It Does:
// Provides fallback extension mappings when formatters.jsonc unavailable.
// Covers common languages with standard formatters. Used by getLanguageForExtension()
// when formattersConfigLoaded == false.
//
// Parameters: None
//
// Returns:
//   map[string]string - Extension → language name mappings
//
// Coverage:
// Rust, Go, Python, JavaScript/TypeScript, C/C++, Ruby, Java, Shell.
// Add more languages here if config loading commonly fails in environment.

// getDefaultFormatter returns hardcoded formatter for a language.
//
// What It Does:
// Provides fallback formatter commands when formatters.jsonc unavailable.
// Matches original hardcoded formatters from v1.0.0. Used by getPrimaryFormatter()
// when formattersConfigLoaded == false.
//
// Parameters:
//   language: Language name (e.g., "rust", "go", "python")
//
// Returns:
//   FormatterTool with command and args, or empty struct if language unknown
//
// Default Formatters:
// rust → rustfmt, go → gofmt, python → black, javascript → prettier,
// c_cpp → clang-format, ruby → rubocop, java → google-java-format, shell → shfmt
func getDefaultFormatter(language string) FormatterTool {
	defaults := map[string]FormatterTool{
		"rust": {
			Command: "rustfmt",
			Args:    []string{"{filepath}"},
			Enabled: true,
		},
		"go": {
			Command: "gofmt",
			Args:    []string{"-w", "{filepath}"},
			Enabled: true,
		},
		"python": {
			Command: "black",
			Args:    []string{"{filepath}"},
			Enabled: true,
		},
		"javascript": {
			Command: "npx",
			Args:    []string{"prettier", "--write", "{filepath}"},
			Enabled: true,
		},
		"c_cpp": {
			Command: "clang-format",
			Args:    []string{"-i", "{filepath}"},
			Enabled: true,
		},
		"ruby": {
			Command: "rubocop",
			Args:    []string{"--auto-correct", "{filepath}"},
			Enabled: true,
		},
		"java": {
			Command: "google-java-format",
			Args:    []string{"--replace", "{filepath}"},
			Enabled: true,
		},
		"shell": {
			Command: "shfmt",
			Args:    []string{"-w", "{filepath}"},
			Enabled: true,
		},
	}

	tool, exists := defaults[language]
	if !exists {
		return FormatterTool{} // Empty tool for unknown language
	}
	return tool
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Formatter Resolution
// ────────────────────────────────────────────────────────────────
// Internal functions mapping extensions to languages and languages to formatters.
// Uses configuration if available, falls back to hardcoded defaults otherwise.

// getLanguageForExtension maps file extension to language name.
//
// What It Does:
// Looks up extension in formatters.jsonc extensions map if config loaded.
// Falls back to getDefaultExtensionMap() if config unavailable. Returns empty
// string if extension unknown (indicates no formatter available).
//
// Parameters:
//   ext: File extension including dot (e.g., ".go", ".rs")
//
// Returns:
//   Language name (e.g., "go", "rust") or empty string if unknown
//
// Usage:
// Called by FormatFile() to determine which language formatters to use.
func getFormatterLanguage(ext string) string {
	if formattersConfigLoaded && formattersConfig != nil {
		// Use config-defined mapping
		language, exists := formattersConfig.Extensions[ext]
		if exists {
			return language // Found in config
		}
	}

	// Fallback to hardcoded mapping
	defaults := getDefaultExtensionMap()
	language, exists := defaults[ext]
	if exists {
		return language // Found in fallback
	}

	return "" // Unknown extension
}

// getPrimaryFormatter gets the primary formatter tool for a language.
//
// What It Does:
// Looks up language in formatters.jsonc formatters map if config loaded,
// finds primary formatter name, returns tool configuration. Falls back to
// getDefaultFormatter() if config unavailable or language not found.
//
// Parameters:
//   language: Language name (e.g., "go", "rust")
//
// Returns:
//   FormatterTool with command and args, or empty struct if not found
//
// Primary Formatter Logic:
// Each language has a "primary" field pointing to preferred formatter.
// If primary formatter disabled, returns empty tool (no formatting).
func getPrimaryFormatter(language string) FormatterTool {
	if formattersConfigLoaded && formattersConfig != nil {
		// Use config-defined formatters
		langFormatters, exists := formattersConfig.Formatters[language]
		if exists {
			// Get primary formatter tool
			primaryName := langFormatters.Primary
			tool, toolExists := langFormatters.Tools[primaryName]
			if toolExists && tool.Enabled {
				return tool // Primary formatter found and enabled
			}
		}
	}

	// Fallback to hardcoded formatter
	return getDefaultFormatter(language)
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Command Execution
// ────────────────────────────────────────────────────────────────
// Internal functions for building and executing formatter commands.

// buildFormatterCommand constructs exec.Cmd from FormatterTool and file path.
//
// What It Does:
// Takes FormatterTool configuration and file path, substitutes {filepath} token
// in arguments, returns ready-to-execute exec.Cmd. Handles argument substitution
// so formatters receive correct file path.
//
// Parameters:
//   tool: FormatterTool with command and args template
//   filePath: Absolute path to file being formatted
//
// Returns:
//   *exec.Cmd ready for execution
//
// Argument Substitution:
// Replaces {filepath} token in tool.Args with actual filePath. Example:
//   Args: ["-w", "{filepath}"] + filePath="/foo/bar.go"
//   Result: exec.Command("gofmt", "-w", "/foo/bar.go")
func buildFormatterCommand(tool FormatterTool, filePath string) *exec.Cmd {
	// Substitute {filepath} token in arguments
	args := make([]string, len(tool.Args))
	for i, arg := range tool.Args {
		args[i] = strings.ReplaceAll(arg, "{filepath}", filePath)
	}

	// Build command with substituted arguments
	cmd := exec.Command(tool.Command, args...)
	cmd.Stderr = nil // Suppress stderr output (formatting tools can be verbose)

	return cmd
}

// executeFormatter runs formatter command and returns error if failed.
//
// What It Does:
// Executes formatter command synchronously, waits for completion, returns
// error if command failed (non-zero exit code, command not found, etc.).
//
// Parameters:
//   cmd: exec.Cmd to execute
//
// Returns:
//   error if execution failed, nil if successful
//
// Execution Behavior:
// Synchronous (blocks until formatter completes). Formatting typically fast
// (< 1 second for most files). No timeout - trust formatters to complete.
func executeFormatter(cmd *exec.Cmd) error {
	return cmd.Run() // Execute and return error or nil
}

// ────────────────────────────────────────────────────────────────
// Public API - File Formatting
// ────────────────────────────────────────────────────────────────
// Exported functions providing file formatting capabilities to callers.

// FormatFile formats a file using the appropriate formatter based on extension.
//
// What It Does:
// Primary API function for code formatting. Maps extension to language, finds
// primary formatter, builds command, executes it, returns result. Handles all
// error cases gracefully - unknown extensions, formatter unavailable, execution
// failures all return FormatResult with appropriate state.
//
// Parameters:
//   filePath: Absolute path to file to format
//   ext: File extension including dot (e.g., ".go", ".rs", ".py")
//
// Returns:
//   *FormatResult with Formatted=true if successful, Formatted=false otherwise
//   FormatResult.Formatter contains name of formatter used (if any)
//   FormatResult.Error contains error object if formatting failed
//
// Usage Example:
//
//	result := validation.FormatFile("/home/user/code/main.go", ".go")
//	if result.Formatted {
//	    result.Report()  // Show success message
//	} else if result.Error != nil {
//	    log.Printf("Formatting failed: %v", result.Error)
//	}
//
// Graceful Handling:
//   - Unknown extension → Formatted=false, no error (not supported)
//   - Formatter unavailable → Formatted=false, Error set (command not found)
//   - Formatter execution error → Formatted=false, Error set (tool failed)
func FormatFile(filePath, ext string) *FormatResult {
	// Map extension to language
	language := getFormatterLanguage(ext)
	if language == "" {
		// Unknown extension - no formatter available
		return &FormatResult{Formatted: false}
	}

	// Get primary formatter for language
	tool := getPrimaryFormatter(language)
	if tool.Command == "" || !tool.Enabled {
		// No formatter configured or disabled
		return &FormatResult{Formatted: false}
	}

	// Build formatter command with file path
	cmd := buildFormatterCommand(tool, filePath)

	// Execute formatter
	err := executeFormatter(cmd)

	// Return result
	return &FormatResult{
		Formatted: (err == nil),
		Formatter: tool.Command,
		Error:     err,
	}
}

// ────────────────────────────────────────────────────────────────
// Public API - Configuration Queries
// ────────────────────────────────────────────────────────────────
// Exported functions for querying formatter configuration without executing.

// GetLanguageForExtension maps file extension to language name (public accessor).
//
// What It Does:
// Public wrapper around getLanguageForExtension() for callers who need to
// query language mappings without formatting. Useful for introspection and
// UI display of supported languages.
//
// Parameters:
//   ext: File extension including dot (e.g., ".go")
//
// Returns:
//   Language name (e.g., "go") or empty string if unknown
//
// Usage:
//
//	language := validation.GetLanguageForExtension(".rs")  // Returns "rust"
func GetFormatterLanguage(ext string) string {
	return getFormatterLanguage(ext)
}

// GetPrimaryFormatter returns the primary formatter name for a language (public accessor).
//
// What It Does:
// Public wrapper around getPrimaryFormatter() returning only command name
// (not full FormatterTool struct). Useful for UI display of which formatter
// will be used for a language.
//
// Parameters:
//   language: Language name (e.g., "rust", "go")
//
// Returns:
//   Formatter command name (e.g., "rustfmt", "gofmt") or empty string if none
//
// Usage:
//
//	formatter := validation.GetPrimaryFormatter("rust")  // Returns "rustfmt"
func GetPrimaryFormatter(language string) string {
	tool := getPrimaryFormatter(language)
	return tool.Command
}

// ────────────────────────────────────────────────────────────────
// Public API - Result Display
// ────────────────────────────────────────────────────────────────
// Methods for displaying formatting results to user.

// Report displays the formatting result using runtime/lib/display.
//
// What It Does:
// Prints success message if formatting completed. Uses display.Success() for
// consistent formatting with runtime system output. Silent if formatting failed
// or not performed (caller responsible for error handling).
//
// Parameters: None (method on FormatResult)
//
// Returns: None (prints to stdout)
//
// Display Behavior:
// Only displays on success (f.Formatted == true). Failures handled by caller
// inspecting f.Error. Uses display library for consistent ANSI color formatting.
//
// Usage:
//
//	result := validation.FormatFile(path, ext)
//	if result.Formatted {
//	    result.Report()  // Prints "✨ Formatted with gofmt" in color
//	}
func (f *FormatResult) Report() {
	if f == nil || !f.Formatted {
		return // Nothing to report - formatting didn't happen or failed
	}

	// Display success message with formatter name
	message := fmt.Sprintf("✨ Formatted with %s", f.Formatter)
	fmt.Println(display.Success(message))
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md

// ────────────────────────────────────────────────────────────────
// Code Validation: Testing Requirements
// ────────────────────────────────────────────────────────────────
// For Code Validation section explanation, see: standards/code/4-block/sections/CWS-SECTION-010-CLOSING-code-validation.md
//
// Testing Requirements:
//   - Import the library without errors: import "system/runtime/lib/validation"
//   - Test FormatFile() with supported extensions (.go, .rs, .py, .js, .c)
//   - Test FormatFile() with unsupported extension (should return Formatted=false, no error)
//   - Test FormatFile() with formatter unavailable (should return Formatted=false, Error set)
//   - Test Report() displays success message using display.Success()
//   - Verify config loading from formatters.jsonc (check formattersConfigLoaded flag)
//   - Verify fallback to hardcoded defaults when config missing
//   - Test GetLanguageForExtension() returns correct language names
//   - Test GetPrimaryFormatter() returns correct formatter commands
//   - Verify stripJSONCComments() handles escaped quotes and strings correctly
//   - Run: go test -v ./... (when tests exist)
//
// Build Verification:
//   - go build ./... (compiles without errors)
//   - go vet ./... (no warnings)
//   - Verify import of system/lib/display works correctly
//   - Check no circular dependencies introduced
//
// Integration Testing:
//   - Test with tool/post-use hook (complete formatting flow)
//   - Format sample files in each supported language
//   - Verify formatters respect project configs (.prettierrc, .clang-format)
//   - Test graceful degradation when formatters not installed
//   - Verify JSONC comment stripping works with complex configs
//   - Test config reload behavior (requires restart)
//   - Verify {filepath} substitution in all formatter args
//
// Example validation code:
//
//     // Test Go file formatting
//     result := FormatFile("/tmp/test.go", ".go")
//     if !result.Formatted {
//         t.Error("Should format Go files with gofmt")
//     }
//     if result.Formatter != "gofmt" {
//         t.Errorf("Expected gofmt, got %s", result.Formatter)
//     }
//
//     // Test unsupported extension
//     result = FormatFile("/tmp/test.xyz", ".xyz")
//     if result.Formatted {
//         t.Error("Should not format unsupported extensions")
//     }
//     if result.Error != nil {
//         t.Error("Unsupported extension should not return error")
//     }
//
//     // Test language mapping
//     language := GetLanguageForExtension(".rs")
//     if language != "rust" {
//         t.Errorf("Expected 'rust', got '%s'", language)
//     }
//
//     // Test config fallback
//     // (Simulate missing config by testing without formatters.jsonc)
//     result = FormatFile("/tmp/test.py", ".py")
//     if result.Formatter != "black" {
//         t.Error("Should fallback to black for Python")
//     }

// ────────────────────────────────────────────────────────────────
// Code Execution: None (Library)
// ────────────────────────────────────────────────────────────────
// For Code Execution section explanation, see: standards/code/4-block/sections/CWS-SECTION-011-CLOSING-code-execution.md
//
// This is a LIBRARY, not an executable. There is no entry point, no main function,
// no execution flow. All functions defined in BODY wait to be called by importing packages.
//
// Usage: import "system/runtime/lib/validation"
//
// The library is imported into hooks (tool/post-use), runtime commands, or other
// libraries that need file formatting capabilities. Configuration loads automatically
// via init() when package imported. All operations occur through function calls from
// importing code.
//
// Example import and usage:
//
//     package main
//
//     import "system/runtime/lib/validation"
//
//     func main() {
//         // Format a Go file
//         result := validation.FormatFile("/path/to/file.go", ".go")
//         if result.Formatted {
//             result.Report()  // Shows success message
//         } else if result.Error != nil {
//             log.Printf("Formatting failed: %v", result.Error)
//         }
//     }

// ────────────────────────────────────────────────────────────────
// Code Cleanup: None (Library)
// ────────────────────────────────────────────────────────────────
// For Code Cleanup section explanation, see: standards/code/4-block/sections/CWS-SECTION-012-CLOSING-code-cleanup.md
//
// Resource Management:
//   - Config files: Loaded once at init, held in memory (small <100KB)
//   - exec.Cmd processes: Created per format operation, terminated automatically
//   - No persistent connections or file handles maintained
//
// Graceful Shutdown:
//   - N/A for libraries (no lifecycle)
//   - Calling code responsible for any resource cleanup
//   - Library is stateless after init() completes
//
// Error State Cleanup:
//   - All errors return immediately with nil config (triggers fallback)
//   - No partial state corruption possible
//   - exec.Cmd failures cleaned up by Go runtime
//
// Memory Management:
//   - Go's garbage collector handles all memory
//   - Config struct held for program lifetime (acceptable <100KB)
//   - No manual memory management required
//   - No large allocations during formatting operations

// ════════════════════════════════════════════════════════════════
// FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Library Overview & Integration Summary
// ────────────────────────────────────────────────────────────────
// For Library Overview section explanation, see: standards/code/4-block/sections/CWS-SECTION-013-CLOSING-library-overview.md
//
// Purpose: See METADATA "Purpose & Function" section above
//
// Provides: See METADATA "Key Features" list above for comprehensive capabilities
//
// Quick summary (high-level only - details in METADATA):
//   - Multi-language code formatting through configurable tool orchestration
//   - Configuration-driven extensibility (add languages without code changes)
//   - Graceful fallback to sensible defaults when config unavailable
//
// Integration Pattern: See METADATA "Usage & Integration" section above for
// complete step-by-step integration guide
//
// Public API: See METADATA "Usage & Integration" section above for complete
// public API list organized by category in typical usage order
//
// Architecture: See METADATA "CPI-SI Identity" section above for complete
// architectural role (Mid-rung library orchestrating lower-rung display library)

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
// For Modification Policy section explanation, see: standards/code/4-block/sections/CWS-SECTION-014-CLOSING-modification-policy.md
//
// Safe to Modify (Extension Points):
//   ✅ Add new languages to formatters.jsonc (follow existing pattern structure)
//   ✅ Add alternative formatters for existing languages (primary + alternatives)
//   ✅ Add new file extensions to extension mappings
//   ✅ Extend fallback formatter arrays (getDefaultExtensionMap, getDefaultFormatter)
//   ✅ Add new helper functions for formatter resolution logic
//
// Modify with Extreme Care (Breaking Changes):
//   ⚠️ Public API function signatures - breaks all calling code (hooks, commands)
//   ⚠️ FormatResult struct fields - breaks code accessing fields directly
//   ⚠️ Config struct definitions - must match JSONC schema exactly
//   ⚠️ Graceful fallback behavior - library must work without configs
//   ⚠️ {filepath} substitution pattern - formatters depend on this token
//
// NEVER Modify (Foundational Rails):
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Configuration loading in init() - changes require rearchitecture
//   ❌ Stateless design principle - no side effects guarantee
//   ❌ Package name or import path (system/runtime/lib/validation)
//   ❌ Display library integration pattern (system/lib/display)
//
// Validation After Modifications:
//   See "Code Validation" section in GROUP 1: CODING above for comprehensive
//   testing requirements, build verification, and integration testing procedures.

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
// For Ladder and Baton Flow section explanation, see: standards/code/4-block/sections/CWS-SECTION-015-CLOSING-ladder-baton-flow.md
//
// See BODY "Organizational Chart - Internal Structure" section above for
// complete ladder structure (dependencies) and baton flow (execution paths).
//
// The Organizational Chart in BODY provides the detailed map showing:
// - All functions and their dependencies (ladder)
// - Complete execution flow paths (baton)
// - APU count (12 functions total)
//
// Quick architectural summary (details in BODY Organizational Chart):
// - 3 public APIs orchestrate 4 core operations using 4 helpers (config/fallback)
// - Ladder: Public APIs → Core Operations → Helpers (unidirectional dependencies)
// - Baton: Extension → Language → Formatter → Command → Execution → Result

// ────────────────────────────────────────────────────────────────
// Surgical Update Points (Extension Guide)
// ────────────────────────────────────────────────────────────────
// For Surgical Update Points section explanation, see: standards/code/4-block/sections/CWS-SECTION-016-CLOSING-surgical-update-points.md
//
// See BODY "Core Operations" subsection header comments above for detailed
// extension points. Each subsection includes guidance showing where to add
// new functionality and what patterns to follow.
//
// Quick reference (details in BODY subsection comments):
// - Adding new language: Update formatters.jsonc + fallback maps (no code changes)
// - Adding alternative formatter: Update tools map in formatters.jsonc
// - Adding new extension: Update extensions map in formatters.jsonc
// - Adding helper function: Add to "Helpers" subsections with proper docstrings
// - Extending formatter resolution: Modify getPrimaryFormatter() logic carefully

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
// For Performance Considerations section explanation, see: standards/code/4-block/sections/CWS-SECTION-017-CLOSING-performance-considerations.md
//
// See SETUP section above for performance characteristics:
// - Types: FormatterTool and FormattersConfig are lightweight (<100KB total)
// - Init: Configuration loading happens once at package import (negligible cost)
//
// See BODY function docstrings above for operation-specific performance notes.
//
// Quick summary (details in SETUP/BODY above):
// - executeFormatter(): Synchronous, blocks until completion (typically <1s per file)
// - Config loading: One-time cost at startup, graceful fallback if slow/failing
// - Key optimization: Formatters run in-place (no file copying), respecting project configs

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
// For Troubleshooting Guide section explanation, see: standards/code/4-block/sections/CWS-SECTION-018-CLOSING-troubleshooting-guide.md
//
// See BODY function docstrings above for operation-specific troubleshooting.
// Functions that commonly have issues include "Error Handling" sections in
// their docstrings with problem/check/solution patterns.
//
// Quick reference (details in BODY function docstrings above):
// - Formatter not found: See executeFormatter() docstring - check tool installation
// - Config not loading: See loadFormattersConfig() docstring - verify file path/permissions
//   - Expected: Library continues with hardcoded fallbacks (formattersConfigLoaded = false)
//   - Note: This is intentional graceful degradation, not a failure
//
// Problem: Formatting fails silently (Formatted=false, no error)
//   - Cause: Extension unknown or formatter disabled in config
//   - Solution: Check getLanguageForExtension() and getPrimaryFormatter() results
//   - Note: Unknown extensions return false gracefully (not an error)
//
// Problem: {filepath} token not substituted in command
//   - Cause: Custom formatter config missing {filepath} in args array
//   - Solution: See buildFormatterCommand() - add {filepath} to args in formatters.jsonc
//   - Note: Token substitution happens for ALL args, not just first

// ────────────────────────────────────────────────────────────────
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────
// For Related Components section explanation, see: standards/code/4-block/sections/CWS-SECTION-019-CLOSING-related-components.md
//
// See METADATA "Dependencies" section above for complete dependency information:
// - Dependencies (What This Needs): Standard Library, system/lib/display
// - Dependents (What Uses This): Hooks (tool/post-use), future runtime commands
// - Integration Points: Config file loading, display library formatting
//
// Quick summary (details in METADATA Dependencies section above):
// - Key dependencies: system/lib/display (ANSI formatting), encoding/json (config parsing)
// - Primary consumers: tool/post-use hook (automatic formatting after file writes)
//
// Parallel Implementation (if applicable):
//   - Runtime validation library: system/runtime/lib/validation/syntax.go (validation vs formatting)
//   - Shared philosophy: Configuration-driven tool orchestration, graceful fallback

// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
// For Future Expansions section explanation, see: standards/code/4-block/sections/CWS-SECTION-020-CLOSING-future-expansions.md
//
// Planned Features:
//   ✓ Multi-language formatting support - COMPLETED
//   ✓ Configuration-driven extensibility - COMPLETED
//   ✓ Graceful fallback to defaults - COMPLETED
//   ⏳ Parallel formatting (multiple files concurrently)
//   ⏳ Format-on-save hooks (watch mode integration)
//   ⏳ Custom formatter plugins (user-defined formatters)
//
// Research Areas:
//   - Formatter availability detection (check_availability command execution)
//   - Project-specific formatter config overrides (per-directory settings)
//   - Formatter result caching (skip formatting if file unchanged)
//   - Language-specific formatter selection (context-aware primary choice)
//   - Integration with language servers (LSP formatting providers)
//
// Integration Targets:
//   - Editor plugins (format-on-save, format-on-type)
//   - CI/CD pipelines (automated formatting checks)
//   - Git pre-commit hooks (format before commit)
//   - Code review tools (suggest formatting improvements)
//   - IDE integrations (native formatter orchestration)
//
// Known Limitations to Address:
//   - No formatter availability checking (assumes tools installed)
//   - No parallel formatting support (processes files sequentially)
//   - No formatter result caching (always re-formats)
//   - Config changes require restart (no hot-reload)
//   - No per-directory formatter overrides (global config only)
//   - No formatter timeout handling (trusts tools to complete)
//
// Version History:
//
// See METADATA "Authorship & Lineage" section above for brief version changelog.
// Comprehensive version history with full context below:
//
//   2.0.0 (2025-11-11) - Configuration-driven architecture
//         - Added formatters.jsonc config loading with JSONC comment stripping
//         - Integrated system/lib/display for consistent output formatting
//         - Comprehensive template alignment (8-rung METADATA, complete CLOSING)
//         - Graceful fallback to hardcoded defaults when config unavailable
//         - Public query APIs (GetLanguageForExtension, GetPrimaryFormatter)
//
//   1.0.0 (2024-10-24) - Initial implementation
//         - Hardcoded formatter mappings (5 languages)
//         - Basic extension → formatter switch statement
//         - Raw fmt.Printf output
//         - Minimal documentation

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
// For Closing Note section explanation, see: standards/code/4-block/sections/CWS-SECTION-021-CLOSING-closing-note.md
//
// This library is a MID-RUNG orchestrator in the validation subsystem. It depends
// on lower-rung display library for output formatting and orchestrates external
// language-specific formatting tools. Hooks and future runtime commands depend on
// this library for consistent code formatting across the CPI-SI ecosystem.
//
// Modify thoughtfully - changes here affect all automatic formatting workflows.
// The graceful fallback design ensures the system continues functioning even when
// configuration is unavailable or formatters are missing (better to skip formatting
// than block operations).
//
// For questions, issues, or contributions:
//   - Review the modification policy above
//   - Follow the 4-block structure pattern
//   - Test thoroughly before committing (go test -v ./... && go build ./...)
//   - Document all changes comprehensively (What/Why/How pattern)
//   - Update formatters.jsonc when adding language support
//
// "Let all things be done decently and in order." - 1 Corinthians 14:40 (KJV)

// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
// For Quick Reference section explanation, see: standards/code/4-block/sections/CWS-SECTION-022-CLOSING-quick-reference.md
//
// Basic Setup:
//   import "system/runtime/lib/validation"
//
//   result := validation.FormatFile("/path/to/file.go", ".go")
//   if result.Formatted {
//       result.Report()
//   }
//
// Check formatter availability:
//   language := validation.GetLanguageForExtension(".rs")
//   if language != "" {
//       formatter := validation.GetPrimaryFormatter(language)
//       fmt.Printf("Will use %s for %s files\n", formatter, language)
//   }
//
// Handle errors gracefully:
//   result := validation.FormatFile(path, ext)
//   if result.Error != nil {
//       log.Printf("Formatting failed: %v (continuing anyway)", result.Error)
//   } else if result.Formatted {
//       fmt.Println("✓ Formatted successfully")
//   } else {
//       fmt.Println("○ No formatter available (skipped)")
//   }
//
// Configuration-driven extension (no code changes):
//   // Add to formatters.jsonc:
//   "elixir": {
//     "primary": "mix_format",
//     "tools": {
//       "mix_format": {
//         "command": "mix",
//         "args": ["format", "{filepath}"],
//         "enabled": true
//       }
//     }
//   }
//   // Add to extensions map:
//   ".ex": "elixir",
//   ".exs": "elixir"
//   // Restart to load new config

// ============================================================================
// END CLOSING
// ============================================================================
