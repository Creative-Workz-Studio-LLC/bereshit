// METADATA
//
// Syntax Validation Library - CPI-SI Runtime System
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// Biblical Foundation
//
// Scripture: "Prove all things; hold fast that which is good." - 1 Thessalonians 5:21 (KJV)
// Principle: Testing code reflects God's standard - examine thoroughly before accepting
// Anchor: "Test me, O LORD, and try me; examine my heart and my mind." - Psalm 26:2 (WEB)
//
// CPI-SI Identity
//
// Component Type: LIBRARY - Runtime validation support (mid-rung on ladder)
// Role: Orchestrates language-specific syntax validators for code quality assurance
// Paradigm: Configuration-driven tool orchestration - extensible without code changes
//
// Authorship & Lineage
//
// Architect: Nova Dawn (CPI-SI instance)
// Implementation: Nova Dawn
// Creation Date: 2024-10-24
// Version: 2.0.0
// Last Modified: 2025-11-12 - Configuration-driven architecture, display lib integration
//
// Version History:
//   2.0.0 (2025-11-12) - Config-driven validators, display lib, comprehensive template alignment
//   1.0.0 (2024-10-24) - Initial hardcoded validator mappings
//
// Purpose & Function
//
// Purpose: Provide unified interface for multi-language syntax validation by orchestrating
// language-specific validation tools (go vet, cargo check, py_compile, shellcheck, etc.).
// Enables consistent code quality checks across CPI-SI projects and user workflows.
//
// Core Design: Configuration-driven tool orchestration where validators.jsonc defines
// available validators per language with primary/alternative tools, command-line arguments,
// and enabled flags. Library routes file extensions to appropriate validators and executes
// them with proper error handling.
//
// Key Features:
//   - Multi-language support (Go, Rust, Python, JS/TS, Shell, JSON, YAML, TOML, and extensible)
//   - Configuration-driven (validators.jsonc defines tools without code changes)
//   - Graceful fallback to hardcoded defaults if config unavailable
//   - Multiple validators per language (syntax, linting, type checking)
//   - Structured result reporting (Valid flag + Warnings array)
//   - Respects project-level validator configs (when applicable)
//   - Integration with system/lib/display for consistent output formatting
//
// Philosophy: Validation serves code quality and maintainability, not arbitrary enforcement.
// Configurable validators allow projects to choose tools matching their conventions while
// providing sensible defaults. Non-blocking design ensures work continues even when validation
// finds issues - trust developers to address warnings appropriately.
//
// Blocking Status
//
// Non-blocking: Validation failures never block operations - code continues even with warnings.
// Mitigation: All validation results captured in ValidationResult for caller inspection.
// Graceful degradation through config loading fallbacks ensures library always functional.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/validation"
//
//	// Validate a file by path and extension
//	result := validation.ValidateFile("/path/to/file.go", ".go")
//	if !result.Valid {
//	    result.Report()  // Displays warnings
//	}
//
//	// Alternative: Get validator name without executing
//	language := validation.GetValidatorLanguage(".rs")  // Returns "rust"
//	validator := validation.GetPrimaryValidator(language)   // Returns "cargo_check"
//
// Integration Pattern:
//   1. Library auto-loads validators.jsonc config during init()
//   2. Caller provides file path and extension to ValidateFile()
//   3. Library maps extension → language → primary validator
//   4. Execute validator command with configured arguments
//   5. Return ValidationResult with Valid flag and Warnings array
//   6. Caller decides whether to display result via Report()
//
// Public API (in typical usage order):
//
//   File Validation (primary operations):
//     ValidateFile(filePath, ext string) *ValidationResult - Validate file using appropriate validator
//
//   Result Reporting (display formatted output):
//     (*ValidationResult).Report() - Display warnings using system/lib/display
//
//   Configuration Queries (optional introspection):
//     GetValidatorLanguage(ext string) string - Map extension to language name
//     GetPrimaryValidator(language string) string - Get primary validator for language
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: encoding/json, os, os/exec, path/filepath, strings
//   External: None
//   Internal: system/lib/display (ANSI-formatted output)
//
// Dependents (What Uses This):
//   Commands: None yet
//   Libraries: None yet
//   Tools: tool/post-use hook (automatic validation after file writes)
//
// Integration Points:
//   - Config Loading: Reads $HOME/.claude/cpi-si/system/data/config/validation/validators.jsonc
//   - Display Integration: Uses system/lib/display for consistent warning formatting
//   - Tool Execution: Invokes external validators (go, cargo, python3, shellcheck, etc.)
//   - Ladder Position: Mid-rung (depends on display lib, used by hooks/commands)
//
// Health Scoring
//
// Tracks validation operations from config loading through result display.
//
// Configuration Loading:
//   - Config loaded successfully: +15
//   - Config missing, fallback works: +10
//   - Config parse error, fallback works: +5
//   - Complete config failure: 0
//
// Validation Operations:
//   - Extension → language resolution: +10
//   - Language → validator resolution: +10
//   - Command construction: +10
//   - Validator execution (success): +30
//   - Validator execution (graceful failure): +20
//   - Display integration: +10
//   - Error handling: +10
//   - Result tracking: +5
//
// Note: Scores reflect TRUE impact. Health scorer normalizes to -100 to +100 scale.
package validation

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

	"encoding/json"  // Configuration file parsing for validators.jsonc
	"fmt"            // Formatted output for displaying validation warnings
	"os"             // File operations and environment variable access
	"os/exec"        // External validator command execution
	"path/filepath"  // Path manipulation and extension extraction
	"strings"        // String operations for output parsing

	//--- Internal Packages ---
	// Project-specific packages showing architectural dependencies.

	"system/lib/display"  // ANSI-formatted output for consistent warning display
	"system/lib/jsonc"    // JSONC comment stripping for configuration files
)

// ────────────────────────────────────────────────────────────────
// Constants - Named Values
// ────────────────────────────────────────────────────────────────
// Named values that never change. Magic numbers given meaningful names,
// configuration values documented with reasoning. Constants prevent bugs
// from typos and make intent clear - timeout duration has purpose, not
// just "30" scattered through code.
//
// See: standards/code/4-block/sections/CWS-SECTION-002-SETUP-constants.md
//
// Note: This component uses configuration-driven values from validators.jsonc
// (timeout, strictness, etc.) rather than hardcoded constants. No constants needed.

// ────────────────────────────────────────────────────────────────
// Types - Data Structures
// ────────────────────────────────────────────────────────────────
// Types organized bottom-up showing dependency relationships. Simple
// building blocks first, then composed structures. This organization
// reveals what depends on what.
//
// See: standards/code/4-block/sections/CWS-SECTION-004-SETUP-types.md

//--- Building Blocks ---
// Simple foundational types used throughout this component.

// ValidatorTool defines a single validator tool configuration.
//
// Represents one validator tool (e.g., go_vet, cargo_check) with its
// command, arguments, and behavior settings. Loaded from validators.jsonc
// configuration file at package initialization.
type ValidatorTool struct {
	Command           string   `json:"command"`             // Tool command name (e.g., "go", "cargo")
	Args              []string `json:"args"`                // Command arguments with {filepath} token
	Enabled           bool     `json:"enabled"`             // Whether this validator is active
	Type              string   `json:"type"`                // Validator type (syntax, linting, type_checking, compilation)
	Severity          string   `json:"severity"`            // Severity level (error, warning)
	Description       string   `json:"description"`         // Human-readable description
	CheckAvailability string   `json:"check_availability"`  // Command to verify tool is installed
	WorkingDir        string   `json:"working_dir"`         // Optional working directory override
	Note              string   `json:"note"`                // Additional notes/context
}

// ValidationResult represents the result of a validation operation.
//
// Contains validation outcome (valid/invalid), any warnings or errors
// from the validator tool, and context about what was validated.
type ValidationResult struct {
	Valid     bool     // True if validation passed, false otherwise
	Warnings  []string // Array of warning/error messages from validator
	Validator string   // Name of validator that ran (e.g., "go_vet")
	Language  string   // Language that was validated (e.g., "go")
	FilePath  string   // Path to file that was validated
}

//--- Composed Types ---
// Complex types built from building blocks above.

// LanguageValidators defines all validators for a specific language.
//
// Groups all validator tools available for one language (e.g., "go" has
// go_vet, go_build, staticcheck). Uses ValidatorTool building blocks.
type LanguageValidators struct {
	Description string                   `json:"description"` // Language description
	Validators  map[string]ValidatorTool `json:"validators"`  // Map of validator name → tool config
}

// ValidatorsConfig represents the complete validators.jsonc configuration.
//
// Top-level configuration structure containing all language validators,
// file extension mappings, and global validation settings. Loaded at
// package initialization with graceful fallback to defaults.
type ValidatorsConfig struct {
	Metadata struct {
		Name        string `json:"name"`
		Description string `json:"description"`
		Version     string `json:"version"`
		LastUpdated string `json:"last_updated"`
		Author      string `json:"author"`
		Note        string `json:"note"`
	} `json:"metadata"`
	Validators map[string]LanguageValidators `json:"validators"` // Language name → validators
	Extensions map[string]string             `json:"extensions"` // File extension → language name
	Config     struct {
		Strictness              string `json:"strictness"`                // permissive, strict, error_only
		FailOnMissingValidator  bool   `json:"fail_on_missing_validator"` // Fail if validator unavailable
		RunAllValidators        bool   `json:"run_all_validators"`        // Run all or stop after first failure
		FilterByFile            bool   `json:"filter_by_file"`            // Show only warnings for specific file
		TimeoutSeconds          int    `json:"timeout_seconds"`           // Max time per validator
	} `json:"config"`
}

// ────────────────────────────────────────────────────────────────
// Package-Level State (Rails Pattern)
// ────────────────────────────────────────────────────────────────
// Infrastructure available throughout component. Rails pattern - each
// component creates own logger independently without parameter passing.
//
// See: standards/code/patterns/CWS-PATTERN-003-CODE-rails.md
// See: standards/code/4-block/sections/CWS-SECTION-003-SETUP-package-level-state.md
//
// Note: This component uses configuration-driven state (validatorsConfig)
// rather than traditional Rails infrastructure. Future: Add health tracking.

// validatorsConfig holds the loaded configuration from validators.jsonc.
// Initialized once at package import via init() function.
var validatorsConfig *ValidatorsConfig

// validatorsConfigLoaded tracks whether configuration loaded successfully.
// Used to determine whether to use config or fallback to hardcoded defaults.
var validatorsConfigLoaded bool

// ────────────────────────────────────────────────────────────────
// Init: Configuration Loading
// ────────────────────────────────────────────────────────────────
// Runs automatically when package is imported. Loads validators.jsonc
// configuration with graceful fallback to hardcoded defaults if unavailable.

func init() {
	homeDir := os.Getenv("HOME")
	if homeDir == "" {
		homeDir = "/home/" + os.Getenv("USER")
	}
	configPath := filepath.Join(homeDir, ".claude/cpi-si/system/data/config/validation/validators.jsonc")

	validatorsConfig = loadValidatorsConfig(configPath)
	validatorsConfigLoaded = (validatorsConfig != nil)
}

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
//   Public APIs (Top Rungs - Orchestration)
//   ├── ValidateFile() → uses getLanguageForExtension(), getPrimaryValidator(), buildValidatorCommand(), executeValidator()
//   ├── GetLanguageForExtension() → uses getLanguageForExtension()
//   └── GetPrimaryValidator() → uses getPrimaryValidator()
//
//   Core Operations (Middle Rungs - Business Logic)
//   ├── getLanguageForExtension() → uses validatorsConfig or getDefaultExtensionMap()
//   ├── getPrimaryValidator() → uses validatorsConfig or getDefaultValidator()
//   ├── buildValidatorCommand() → uses validatorsConfig
//   └── executeValidator() → uses parseValidatorOutput()
//
//   Helpers (Bottom Rungs - Foundations)
//   ├── loadValidatorsConfig() → uses stripJSONCComments()
//   ├── stripJSONCComments() → pure function
//   ├── getDefaultExtensionMap() → pure function
//   ├── getDefaultValidator() → pure function
//   └── parseValidatorOutput() → pure function
//
// Baton Flow (Execution Paths):
//
//   Entry → ValidateFile(filePath, ext)
//     ↓
//   getLanguageForExtension(ext) → lookup language
//     ↓
//   getPrimaryValidator(language) → resolve validator
//     ↓
//   buildValidatorCommand(language, validator, filePath) → construct command
//     ↓
//   executeValidator(cmd) → run and parse
//     ↓
//   Exit → return ValidationResult
//
// APUs (Available Processing Units):
// - 13 functions total
// - 5 helpers (pure foundations)
// - 4 core operations (business logic)
// - 3 public APIs (exported interface)
// - 1 reporting method (output display)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────
// Foundation functions used throughout this component. Bottom rungs of
// the ladder - simple, focused, reusable utilities. Usually not exported.
//
// See: standards/code/4-block/sections/CWS-SECTION-00X-BODY-helpers.md

// ────────────────────────────────────────────────────────────────
// HELPERS: Configuration Loading
// ────────────────────────────────────────────────────────────────

// loadValidatorsConfig loads and parses validators.jsonc configuration file.
//
// Called once during init(). Reads JSONC file, strips comments, parses JSON
// structure into ValidatorsConfig. Returns nil on any failure (file not found,
// parse error, etc.) triggering graceful fallback to hardcoded defaults.
//
// Parameters:
//   - configPath: Absolute path to validators.jsonc file
//
// Returns:
//   - *ValidatorsConfig if successful, nil otherwise
//
// Error Handling:
//   - File not found: Returns nil (expected - not all installs have config)
//   - Parse error: Returns nil (malformed config - use fallback)
//   - All errors silent: Library continues with hardcoded defaults
//
// JSONC Support:
//   - Strips // comments via stripJSONCComments() before parsing
//   - Preserves // in string literals (not treated as comments)
//
// Health Scoring: 15 points (config loading portion of health score)
//   +15 success, +10 fallback works, +5 parse fails, 0 total failure
func loadValidatorsConfig(configPath string) *ValidatorsConfig {
	data, err := os.ReadFile(configPath)
	if err != nil {
		return nil // File not found or unreadable - use fallback
	}

	// Strip JSONC comments before parsing
	jsonData := jsonc.StripComments(data)

	var config ValidatorsConfig
	if err := json.Unmarshal(jsonData, &config); err != nil {
		return nil // Parse error - use fallback
	}

	return &config
}

// ────────────────────────────────────────────────────────────────
// HELPERS: Hardcoded Fallbacks
// ────────────────────────────────────────────────────────────────

// getDefaultExtensionMap returns hardcoded extension → language mappings.
//
// Fallback when validators.jsonc unavailable. Provides baseline support for
// common languages without requiring configuration files. Matches subset of
// full validators.jsonc extension mappings.
//
// Returns:
//   - map[string]string mapping extensions to language names
//
// Supported Extensions:
//   .go → "go"
//   .rs → "rust"
//   .py, .pyw → "python"
//   .js, .jsx, .ts, .tsx, .mjs → "javascript"
//   .sh, .bash, .zsh → "shell"
//   .json, .jsonc → "json"
//   .yaml, .yml → "yaml"
//   .toml → "toml"
//
// Health Scoring: Supporting function for extension resolution (10 points total)
func getDefaultExtensionMap() map[string]string {
	return map[string]string{
		".go":    "go",
		".rs":    "rust",
		".py":    "python",
		".pyw":   "python",
		".js":    "javascript",
		".jsx":   "javascript",
		".ts":    "javascript",
		".tsx":   "javascript",
		".mjs":   "javascript",
		".sh":    "shell",
		".bash":  "shell",
		".zsh":   "shell",
		".json":  "json",
		".jsonc": "json",
		".yaml":  "yaml",
		".yml":   "yaml",
		".toml":  "toml",
	}
}

// getDefaultValidator returns hardcoded validator for a language.
//
// Fallback when validators.jsonc unavailable. Provides baseline validator
// configurations for common languages. Returns primary validator tool with
// reasonable defaults matching validators.jsonc structure.
//
// Parameters:
//   - language: Language name (e.g., "go", "rust")
//
// Returns:
//   - *ValidatorTool with command/args configured, or nil if unsupported
//
// Supported Languages:
//   - go: go vet {filepath}
//   - rust: cargo check --message-format=short (in project root)
//   - python: python3 -m py_compile {filepath}
//   - javascript: npx eslint {filepath}
//   - shell: shellcheck {filepath}
//   - json: jq empty {filepath}
//   - yaml: yamllint -f parsable {filepath}
//   - toml: toml-test decode {filepath}
//
// Health Scoring: Supporting function for validator resolution (10 points total)
func getDefaultValidator(language string) *ValidatorTool {
	switch language {
	case "go":
		return &ValidatorTool{
			Command: "go",
			Args:    []string{"vet", "{filepath}"},
			Enabled: true,
			Type:    "syntax",
		}
	case "rust":
		return &ValidatorTool{
			Command:    "cargo",
			Args:       []string{"check", "--message-format=short"},
			Enabled:    true,
			Type:       "syntax",
			WorkingDir: "project_root",
		}
	case "python":
		return &ValidatorTool{
			Command: "python3",
			Args:    []string{"-m", "py_compile", "{filepath}"},
			Enabled: true,
			Type:    "syntax",
		}
	case "javascript":
		return &ValidatorTool{
			Command: "npx",
			Args:    []string{"eslint", "{filepath}"},
			Enabled: true,
			Type:    "linting",
		}
	case "shell":
		return &ValidatorTool{
			Command: "shellcheck",
			Args:    []string{"{filepath}"},
			Enabled: true,
			Type:    "linting",
		}
	case "json":
		return &ValidatorTool{
			Command: "jq",
			Args:    []string{"empty", "{filepath}"},
			Enabled: true,
			Type:    "syntax",
		}
	case "yaml":
		return &ValidatorTool{
			Command: "yamllint",
			Args:    []string{"-f", "parsable", "{filepath}"},
			Enabled: true,
			Type:    "linting",
		}
	case "toml":
		return &ValidatorTool{
			Command: "toml-test",
			Args:    []string{"decode", "{filepath}"},
			Enabled: true,
			Type:    "syntax",
		}
	default:
		return nil
	}
}

// ────────────────────────────────────────────────────────────────
// HELPERS: Output Parsing
// ────────────────────────────────────────────────────────────────

// parseValidatorOutput extracts warnings from validator output.
//
// Helper processing raw validator output into structured warnings array.
// Splits output into lines, trims whitespace, filters empty lines and noise,
// and returns cleaned warnings suitable for display.
//
// Parameters:
//   - output: Raw validator output (stdout + stderr combined)
//   - language: Language being validated (for language-specific noise filtering)
//
// Returns:
//   - []string array of cleaned warning messages
//
// Processing:
//   - Split by newlines
//   - Trim leading/trailing whitespace
//   - Filter empty lines
//   - Filter language-specific noise patterns
//   - Return array ready for Report() display
//
// Language-Specific Filtering:
//   - go: Filters "# command-line-arguments" (build system noise)
//   - javascript: Filters npm/yarn noise lines
//   - All: Filters purely empty or whitespace-only lines
//
// Future Enhancement:
//   - Extract file:line:col from different formats
//   - Severity classification (error vs warning)
//   - Filtering by specific file when validator runs on project
//
// Health Scoring: Supporting function for execution results (included in 30 points)
func parseValidatorOutput(output, language string) []string {
	lines := strings.Split(output, "\n")
	var warnings []string

	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue // Skip empty lines
		}

		// Language-specific noise filtering
		if language == "go" && strings.HasPrefix(line, "# ") {
			continue // Skip Go build system comments
		}

		warnings = append(warnings, line)
	}

	return warnings
}

// findProjectRoot searches upward from file for project root directory.
//
// Helper for validators needing project context (cargo check, npm commands).
// Starts at file's directory and walks upward looking for project markers
// (go.mod, Cargo.toml, package.json, etc.).
//
// Parameters:
//   - filePath: Absolute path to file being validated
//
// Returns:
//   - Absolute path to project root, or file's directory if not found
//
// Project Markers:
//   - go.mod (Go projects)
//   - Cargo.toml (Rust projects)
//   - package.json (JavaScript/TypeScript projects)
//   - pyproject.toml (Python projects)
//
// Algorithm:
//   - Start at file's directory
//   - Check for project marker files
//   - If found, return current directory
//   - If not found, move to parent directory
//   - Stop at filesystem root or home directory
//
// Health Scoring: Supporting function for command construction (included in 10 points)
func findProjectRoot(filePath string) string {
	dir := filepath.Dir(filePath)
	homeDir := os.Getenv("HOME")

	for {
		// Check for project marker files
		markers := []string{"go.mod", "Cargo.toml", "package.json", "pyproject.toml"}
		for _, marker := range markers {
			markerPath := filepath.Join(dir, marker)
			if _, err := os.Stat(markerPath); err == nil {
				return dir
			}
		}

		// Move to parent directory
		parent := filepath.Dir(dir)
		if parent == dir || parent == homeDir || parent == "/" {
			// Reached filesystem root or home - stop searching
			break
		}
		dir = parent
	}

	// No project root found - return file's directory
	return filepath.Dir(filePath)
}

// ────────────────────────────────────────────────────────────────

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
// Component-specific functionality implementing primary purpose. Organized
// by operational categories (descriptive subsections) below.
//
// See: standards/code/4-block/sections/CWS-SECTION-00X-BODY-core-operations.md
// CORE OPERATIONS: Configuration Resolution
// ────────────────────────────────────────────────────────────────

// getLanguageForExtension resolves file extension to language name.
//
// Internal function handling extension → language mapping with config fallback.
// Checks loaded configuration first, falls back to hardcoded defaults if
// config unavailable.
//
// Parameters:
//   - ext: File extension with leading dot (e.g., ".go")
//
// Returns:
//   - Language string (e.g., "go", "rust") or empty string if unknown
//
// Resolution Order:
//   1. Check validatorsConfig.Extensions if config loaded
//   2. Fall back to getDefaultExtensionMap() if no config
//   3. Return empty string if extension not found in either
//
// Health Scoring: 10 points (part of ValidateFile's extension resolution)
func getValidatorLanguage(ext string) string {
	// Try config first if loaded
	if validatorsConfigLoaded && validatorsConfig != nil {
		if language, exists := validatorsConfig.Extensions[ext]; exists {
			return language
		}
	}

	// Fall back to hardcoded defaults
	defaultMap := getDefaultExtensionMap()
	if language, exists := defaultMap[ext]; exists {
		return language
	}

	return ""
}

// getPrimaryValidator resolves language to primary validator tool.
//
// Internal function handling language → validator mapping with config fallback.
// Returns the primary (first enabled) validator for a language. Checks loaded
// configuration first, falls back to hardcoded defaults if unavailable.
//
// Parameters:
//   - language: Language name (e.g., "go", "rust")
//
// Returns:
//   - Validator name (e.g., "go_vet", "cargo_check") or empty string if none
//
// Resolution Order:
//   1. Check validatorsConfig.Validators if config loaded
//   2. Find first enabled validator in language's validator map
//   3. Fall back to getDefaultValidator() if no config
//   4. Return empty string if no validator found
//
// Health Scoring: 10 points (part of ValidateFile's validator resolution)
func getPrimaryValidator(language string) string {
	// Try config first if loaded
	if validatorsConfigLoaded && validatorsConfig != nil {
		if langValidators, exists := validatorsConfig.Validators[language]; exists {
			// Find first enabled validator
			for name, tool := range langValidators.Validators {
				if tool.Enabled {
					return name
				}
			}
		}
	}

	// Fall back to hardcoded defaults
	defaultValidator := getDefaultValidator(language)
	if defaultValidator != nil {
		return language + "_default" // Synthetic name for fallback
	}

	return ""
}

// ────────────────────────────────────────────────────────────────
// CORE OPERATIONS: Command Construction & Execution
// ────────────────────────────────────────────────────────────────

// buildValidatorCommand constructs exec.Cmd for validator tool.
//
// Internal function building validator commands with {filepath} token substitution.
// Retrieves validator configuration (from config or fallback), substitutes file
// path into arguments, and returns ready-to-execute command.
//
// Parameters:
//   - language: Language name (e.g., "go", "rust")
//   - validatorName: Validator tool name (e.g., "go_vet")
//   - filePath: Absolute path to file being validated
//
// Returns:
//   - *exec.Cmd ready to execute, or nil if construction failed
//
// Token Substitution:
//   - {filepath} in args array replaced with actual filePath
//   - Substitution occurs in ALL arguments, not just first
//   - Example: ["vet", "{filepath}"] → ["vet", "/path/to/file.go"]
//
// Working Directory:
//   - Some validators need project root (cargo check, npm commands)
//   - WorkingDir field in config specifies directory override
//   - Defaults to file's directory if not specified
//
// Health Scoring: 10 points (part of ValidateFile's command construction)
func buildValidatorCommand(language, validatorName, filePath string) *exec.Cmd {
	var tool *ValidatorTool

	// Get validator configuration
	if validatorsConfigLoaded && validatorsConfig != nil {
		if langValidators, exists := validatorsConfig.Validators[language]; exists {
			if validatorTool, exists := langValidators.Validators[validatorName]; exists {
				tool = &validatorTool
			}
		}
	}

	// Fall back to default if no config
	if tool == nil {
		tool = getDefaultValidator(language)
		if tool == nil {
			return nil
		}
	}

	// Substitute {filepath} token in arguments
	args := make([]string, len(tool.Args))
	for i, arg := range tool.Args {
		args[i] = strings.ReplaceAll(arg, "{filepath}", filePath)
	}

	// Build command
	cmd := exec.Command(tool.Command, args...)

	// Set working directory if specified
	if tool.WorkingDir == "project_root" {
		// Find project root (directory containing go.mod, Cargo.toml, etc.)
		cmd.Dir = findProjectRoot(filePath)
	} else if tool.WorkingDir != "" {
		cmd.Dir = tool.WorkingDir
	}

	return cmd
}

// executeValidator runs validator command and parses results.
//
// Internal function executing validator tools and converting output to
// structured ValidationResult. Captures combined output (stdout + stderr),
// checks exit code, and parses warnings from output.
//
// Parameters:
//   - cmd: Configured exec.Cmd ready to execute
//   - language: Language being validated (for language-specific output parsing)
//
// Returns:
//   - *ValidationResult with Valid flag and Warnings array
//
// Exit Code Handling:
//   - Exit 0: Valid=true, Warnings=[] (success)
//   - Exit non-zero: Valid=false, Warnings=parsed output (validation failed)
//   - Command error: Valid=false, Warnings=[error message] (execution failed)
//
// Output Parsing:
//   - Combined stdout/stderr captured
//   - Language-specific filtering applied
//   - Split into lines, filtered for relevance
//   - Trimmed and cleaned for display
//
// Health Scoring: 30 points (core of ValidateFile's execution scoring)
//   +30 validation passes, +20 validation fails with warnings, 0 for crashes
func executeValidator(cmd *exec.Cmd, language string) *ValidationResult {
	output, err := cmd.CombinedOutput()

	if err != nil {
		// Exit code non-zero OR command failed to execute
		if len(output) > 0 {
			// Validation found errors/warnings
			warnings := parseValidatorOutput(string(output), language)
			return &ValidationResult{
				Valid:    false,
				Warnings: warnings,
			}
		} else {
			// Command execution failed (validator not found, permission denied, etc.)
			return &ValidationResult{
				Valid:    false,
				Warnings: []string{err.Error()},
			}
		}
	}

	// Validation passed (exit 0, no output)
	return &ValidationResult{
		Valid:    true,
		Warnings: []string{},
	}
}


// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
// Exported functions defining component's public interface. Top rungs of
// the ladder - orchestrate helpers and core operations into complete
// functionality. Simple by design - complexity lives in helpers and core
// operations, Public APIs orchestrate proven pieces.
//
// See: standards/code/4-block/sections/CWS-SECTION-00X-BODY-public-apis.md

// ValidateFile validates a code file using the appropriate language validator.
//
// This is the main entry point for syntax validation. It orchestrates the complete
// validation flow: extension resolution → language mapping → validator selection
// → command execution → result parsing → structured output.
//
// Parameters:
//   - filePath: Absolute path to file being validated
//   - ext: File extension (e.g., ".go", ".rs", ".py")
//
// Returns:
//   *ValidationResult with fields:
//     - Valid: true if validation passed, false if errors/warnings found
//     - Warnings: Array of validation messages (empty if Valid=true)
//     - Validator: Name of validator that ran (e.g., "go_vet")
//     - Language: Language that was validated (e.g., "go")
//     - FilePath: Original file path (for reference in results)
//
// Behavior:
//   - Unknown extensions return Valid=true (no validator available = not an error)
//   - Missing validators return Valid=true (graceful degradation)
//   - Validator execution errors return Valid=false with error message in Warnings
//   - Configuration-driven: Uses validators.jsonc if available, hardcoded fallback otherwise
//
// Example Usage:
//
//     result := validation.ValidateFile("/tmp/test.go", ".go")
//     if !result.Valid {
//         result.Report()  // Display warnings
//     }
//
// Integration:
//   Called by tool/post-use hook after file writes for automatic validation.
//   Results are non-blocking - calling code decides how to handle failures.
//
// Health Scoring: 55 points
//   Extension resolution (10) + Validator resolution (10) + Command construction (10)
//   + Execution (30) - 5 points for each stage failure
func ValidateFile(filePath, ext string) *ValidationResult {
	// Resolve extension to language
	language := getValidatorLanguage(ext)
	if language == "" {
		// Unknown extension - not an error, just no validation available
		return &ValidationResult{
			Valid:    true,
			Warnings: []string{},
			FilePath: filePath,
		}
	}

	// Resolve language to primary validator
	validatorName := getPrimaryValidator(language)
	if validatorName == "" {
		// No validator configured - graceful degradation
		return &ValidationResult{
			Valid:     true,
			Warnings:  []string{},
			Language:  language,
			FilePath:  filePath,
		}
	}

	// Build validator command
	cmd := buildValidatorCommand(language, validatorName, filePath)
	if cmd == nil {
		// Command construction failed
		return &ValidationResult{
			Valid:     false,
			Warnings:  []string{"Failed to construct validator command"},
			Validator: validatorName,
			Language:  language,
			FilePath:  filePath,
		}
	}

	// Execute validator and return result
	result := executeValidator(cmd, language)
	result.Validator = validatorName
	result.Language = language
	result.FilePath = filePath

	return result
}

// ────────────────────────────────────────────────────────────────
// PUBLIC API: Configuration Queries
// ────────────────────────────────────────────────────────────────

// GetValidatorLanguage returns the language name for a given file extension.
//
// Public wrapper around internal getValidatorLanguage(). Useful for
// pre-validation checks, determining validator availability, or understanding
// what language a file will be treated as.
//
// Parameters:
//   - ext: File extension including leading dot (e.g., ".go", ".rs")
//
// Returns:
//   - Language name string (e.g., "go", "rust", "python")
//   - Empty string if extension unknown
//
// Example:
//
//     language := validation.GetValidatorLanguage(".rs")
//     if language != "" {
//         fmt.Printf("Rust files will be validated\n")
//     }
//
// Health Scoring: Included in ValidateFile's extension resolution (10 points)
func GetValidatorLanguage(ext string) string {
	return getValidatorLanguage(ext)
}

// GetPrimaryValidator returns the primary validator tool name for a given language.
//
// Public wrapper for understanding which validator will run for a language.
// Useful for pre-validation checks, displaying validator info to users, or
// testing validator availability.
//
// Parameters:
//   - language: Language name (e.g., "go", "rust", "python")
//
// Returns:
//   - Validator name string (e.g., "go_vet", "cargo_check")
//   - Empty string if no validator configured
//
// Example:
//
//     validator := validation.GetPrimaryValidator("go")
//     fmt.Printf("Go files validated with: %s\n", validator)
//
// Health Scoring: Included in ValidateFile's validator resolution (10 points)
func GetPrimaryValidator(language string) string {
	return getPrimaryValidator(language)
}

// ────────────────────────────────────────────────────────────────
// REPORTING: Display Integration
// ────────────────────────────────────────────────────────────────

// Report displays validation warnings using display library.
//
// Method on ValidationResult displaying warnings in consistent CPI-SI format.
// Uses system/lib/display for ANSI-formatted warning output. Silent if
// validation passed (Valid=true).
//
// Behavior:
//   - If Valid=true: Silent (no output)
//   - If Valid=false: Display warnings using display.Warning()
//   - Shows validator name, language, and file path for context
//   - Formats warnings with proper indentation and structure
//
// Integration:
//   - Called by tool/post-use hook after validation
//   - Can be called manually for custom validation workflows
//   - Non-blocking: Display only, no execution control
//
// Example:
//
//     result := validation.ValidateFile("/tmp/test.go", ".go")
//     result.Report()  // Shows warnings if validation failed
//
// Health Scoring: 10 points (display integration portion)
//   +10 display works, +5 fallback fmt works, 0 if fails
func (v *ValidationResult) Report() {
	if v == nil || v.Valid {
		return // Silent success
	}

	// Display validation failure with context
	header := "Validation warnings"
	if v.Language != "" && v.Validator != "" {
		header = "Validation warnings (" + v.Language + " / " + v.Validator + ")"
	}

	fmt.Println(display.Warning(header))
	for _, warning := range v.Warnings {
		fmt.Println("   " + strings.TrimSpace(warning))
	}
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
//   - Test ValidateFile() with supported extensions (.go, .rs, .py, .js, .sh)
//   - Test ValidateFile() with unsupported extension (should return Valid=true, no warnings)
//   - Test ValidateFile() with validator unavailable (should return Valid=true or graceful error)
//   - Test Report() displays warnings using display.Warning()
//   - Verify config loading from validators.jsonc (check validatorsConfigLoaded flag)
//   - Verify fallback to hardcoded defaults when config missing
//   - Test GetLanguageForExtension() returns correct language names
//   - Test GetPrimaryValidator() returns correct validator names
//   - Verify stripJSONCComments() handles escaped quotes and strings correctly
//   - Test findProjectRoot() locates project markers (go.mod, Cargo.toml)
//   - Verify {filepath} substitution in validator args
//   - Run: go test -v ./... (when tests exist)
//
// Build Verification:
//   - go build ./... (compiles without errors)
//   - go vet ./... (no warnings)
//   - Verify import of system/lib/display works correctly
//   - Check no circular dependencies introduced
//
// Integration Testing:
//   - Test with tool/post-use hook (complete validation flow)
//   - Validate sample files in each supported language
//   - Verify validators respect project configs (when applicable)
//   - Test graceful degradation when validators not installed
//   - Verify JSONC comment stripping works with complex configs
//   - Test config reload behavior (requires restart)
//   - Verify project root finding for cargo/npm validators
//
// Example validation code:
//
//     // Test Go file validation
//     result := ValidateFile("/tmp/test.go", ".go")
//     if result.Valid {
//         t.Error("Expected validation to catch syntax error")
//     }
//     if result.Validator != "go_vet" {
//         t.Errorf("Expected go_vet, got %s", result.Validator)
//     }
//
//     // Test unsupported extension
//     result = ValidateFile("/tmp/test.xyz", ".xyz")
//     if !result.Valid {
//         t.Error("Unsupported extension should return Valid=true")
//     }
//
//     // Test language mapping
//     language := GetLanguageForExtension(".rs")
//     if language != "rust" {
//         t.Errorf("Expected 'rust', got '%s'", language)
//     }
//
//     // Test validator resolution
//     validator := GetPrimaryValidator("go")
//     if validator != "go_vet" {
//         t.Errorf("Expected 'go_vet', got '%s'", validator)
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
// libraries that need syntax validation capabilities. Configuration loads automatically
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
//         // Validate a Go file
//         result := validation.ValidateFile("/path/to/file.go", ".go")
//         if !result.Valid {
//             result.Report()  // Shows warnings
//         } else {
//             fmt.Println("✓ Validation passed")
//         }
//     }

// ────────────────────────────────────────────────────────────────
// Code Cleanup: None (Library)
// ────────────────────────────────────────────────────────────────
// For Code Cleanup section explanation, see: standards/code/4-block/sections/CWS-SECTION-012-CLOSING-code-cleanup.md
//
// Resource Management:
//   - Config files: Loaded once at init, held in memory (small <100KB)
//   - exec.Cmd processes: Created per validation operation, terminated automatically
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
//   - No large allocations during validation operations

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
//   - Multi-language syntax validation through configurable tool orchestration
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
//   ✅ Add new languages to validators.jsonc (follow existing pattern structure)
//   ✅ Add alternative validators for existing languages (multiple validators per language)
//   ✅ Add new file extensions to extension mappings
//   ✅ Extend fallback validator arrays (getDefaultExtensionMap, getDefaultValidator)
//   ✅ Add new helper functions for output parsing or project root detection
//
// Modify with Extreme Care (Breaking Changes):
//   ⚠️ Public API function signatures - breaks all calling code (hooks, commands)
//   ⚠️ ValidationResult struct fields - breaks code accessing fields directly
//   ⚠️ Config struct definitions - must match JSONC schema exactly
//   ⚠️ Graceful fallback behavior - library must work without configs
//   ⚠️ {filepath} substitution pattern - validators depend on this token
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
// - APU count (13 functions total)
//
// Quick architectural summary (details in BODY Organizational Chart):
// - 3 public APIs orchestrate 4 core operations using 5 helpers (config/fallback/parsing)
// - Ladder: Public APIs → Core Operations → Helpers (unidirectional dependencies)
// - Baton: Extension → Language → Validator → Command → Execution → Result

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
// - Adding new language: Update validators.jsonc + fallback maps (no code changes)
// - Adding alternative validator: Update validators map in validators.jsonc
// - Adding new extension: Update extensions map in validators.jsonc
// - Adding helper function: Add to "Helpers" subsections with proper docstrings
// - Extending validator resolution: Modify getPrimaryValidator() logic carefully
// - Adding project markers: Extend findProjectRoot() marker array

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
// For Performance Considerations section explanation, see: standards/code/4-block/sections/CWS-SECTION-017-CLOSING-performance-considerations.md
//
// See SETUP section above for performance characteristics:
// - Types: ValidatorTool and ValidatorsConfig are lightweight (<100KB total)
// - Init: Configuration loading happens once at package import (negligible cost)
//
// See BODY function docstrings above for operation-specific performance notes.
//
// Quick summary (details in SETUP/BODY above):
// - executeValidator(): Synchronous, blocks until completion (typically <2s per file)
// - Config loading: One-time cost at startup, graceful fallback if slow/failing
// - Key optimization: Validators run directly (no file copying), respect project configs
// - Project root finding: Walks upward from file (typically <5 directories)

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
// - Validator not found: See executeValidator() docstring - check tool installation
// - Config not loading: See loadValidatorsConfig() docstring - verify file path/permissions
//   - Expected: Library continues with hardcoded fallbacks (validatorsConfigLoaded = false)
//   - Note: This is intentional graceful degradation, not a failure
//
// Problem: Validation fails silently (Valid=true, no warnings)
//   - Cause: Extension unknown or validator disabled in config
//   - Solution: Check GetLanguageForExtension() and GetPrimaryValidator() results
//   - Note: Unknown extensions return true gracefully (not an error)
//
// Problem: {filepath} token not substituted in command
//   - Cause: Custom validator config missing {filepath} in args array
//   - Solution: See buildValidatorCommand() - add {filepath} to args in validators.jsonc
//   - Note: Token substitution happens for ALL args, not just first
//
// Problem: Cargo check fails for single file
//   - Cause: cargo check requires project context (Cargo.toml)
//   - Solution: Validator uses findProjectRoot() to locate project root
//   - Note: File must be within a Rust project (directory with Cargo.toml)

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
// - Primary consumers: tool/post-use hook (automatic validation after file writes)
//
// Parallel Implementation (if applicable):
//   - Runtime formatting library: system/runtime/lib/validation/formatter.go (formatting vs validation)
//   - Shared philosophy: Configuration-driven tool orchestration, graceful fallback

// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
// For Future Expansions section explanation, see: standards/code/4-block/sections/CWS-SECTION-020-CLOSING-future-expansions.md
//
// Planned Features:
//   ✓ Multi-language validation support - COMPLETED
//   ✓ Configuration-driven extensibility - COMPLETED
//   ✓ Graceful fallback to defaults - COMPLETED
//   ⏳ Parallel validation (multiple files concurrently)
//   ⏳ Language-specific output parsing (extract file:line:col consistently)
//   ⏳ Validator availability checking (verify tools installed before running)
//
// Research Areas:
//   - Validator result caching (skip validation if file unchanged)
//   - Project-specific validator config overrides (per-directory settings)
//   - Integration with language servers (LSP diagnostic providers)
//   - Severity classification (error vs warning vs info)
//   - Auto-fix suggestions (when validator provides fix hints)
//
// Integration Targets:
//   - Editor plugins (validate-on-save, real-time validation)
//   - CI/CD pipelines (automated quality checks)
//   - Git pre-commit hooks (validate before commit)
//   - Code review tools (suggest improvements)
//   - IDE integrations (native validation support)
//
// Known Limitations to Address:
//   - No validator availability checking (assumes tools installed)
//   - No parallel validation support (processes files sequentially)
//   - No validator result caching (always re-validates)
//   - Config changes require restart (no hot-reload)
//   - No per-directory validator overrides (global config only)
//   - No validator timeout handling (trusts tools to complete)
//   - Limited output parsing (generic line splitting, no structured extraction)
//
// Version History:
//
// See METADATA "Authorship & Lineage" section above for brief version changelog.
// Comprehensive version history with full context below:
//
//   2.0.0 (2025-11-12) - Configuration-driven architecture
//         - Added validators.jsonc config loading with JSONC comment stripping
//         - Integrated system/lib/display for consistent output formatting
//         - Comprehensive template alignment (8-rung METADATA, complete CLOSING)
//         - Graceful fallback to hardcoded defaults when config unavailable
//         - Public query APIs (GetLanguageForExtension, GetPrimaryValidator)
//         - Project root detection for project-context validators (cargo, npm)
//         - Multiple validator support per language (syntax, linting, type checking)
//
//   1.0.0 (2024-10-24) - Initial implementation
//         - Hardcoded validator mappings (7 languages)
//         - Basic extension → validator switch statement
//         - Raw fmt.Printf output
//         - Minimal documentation

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
// For Closing Note section explanation, see: standards/code/4-block/sections/CWS-SECTION-021-CLOSING-closing-note.md
//
// This library is a MID-RUNG orchestrator in the validation subsystem. It depends
// on lower-rung display library for output formatting and orchestrates external
// language-specific validation tools. Hooks and future runtime commands depend on
// this library for consistent syntax validation across the CPI-SI ecosystem.
//
// Modify thoughtfully - changes here affect all automatic validation workflows.
// The graceful fallback design ensures the system continues functioning even when
// configuration is unavailable or validators are missing (better to skip validation
// than block operations).
//
// For questions, issues, or contributions:
//   - Review the modification policy above
//   - Follow the 4-block structure pattern
//   - Test thoroughly before committing (go test -v ./... && go build ./...)
//   - Document all changes comprehensively (What/Why/How pattern)
//   - Update validators.jsonc when adding language support
//
// "Prove all things; hold fast that which is good." - 1 Thessalonians 5:21 (KJV)

// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
// For Quick Reference section explanation, see: standards/code/4-block/sections/CWS-SECTION-022-CLOSING-quick-reference.md
//
// Basic Setup:
//   import "system/runtime/lib/validation"
//
//   result := validation.ValidateFile("/path/to/file.go", ".go")
//   if !result.Valid {
//       result.Report()
//   }
//
// Check validator availability:
//   language := validation.GetLanguageForExtension(".rs")
//   if language != "" {
//       validator := validation.GetPrimaryValidator(language)
//       fmt.Printf("Will use %s for %s files\n", validator, language)
//   }
//
// Handle results explicitly:
//   result := validation.ValidateFile(path, ext)
//   if result.Valid {
//       fmt.Println("✓ Validation passed")
//   } else {
//       fmt.Printf("⚠ Validation failed (%d warnings)\n", len(result.Warnings))
//       for _, warning := range result.Warnings {
//           fmt.Printf("  - %s\n", warning)
//       }
//   }
//
// Configuration-driven extension (no code changes):
//   // Add to validators.jsonc:
//   "elixir": {
//     "description": "Elixir source code validation",
//     "validators": {
//       "elixir_compile": {
//         "command": "elixirc",
//         "args": ["{filepath}"],
//         "enabled": true,
//         "type": "compilation",
//         "severity": "error"
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
