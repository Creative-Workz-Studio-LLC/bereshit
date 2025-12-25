// ============================================================================
// METADATA
// ============================================================================
// Privacy Library - System-wide sanitization for sensitive data protection
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// Biblical Foundation
//
// Scripture: "It is the glory of God to conceal a matter" - Proverbs 25:2 (WEB)
// Principle: Privacy is protection, not secrecy - we protect what should be protected
// Anchor: "Guard your heart with all diligence, for out of it is the wellspring of life" - Proverbs 4:23 (WEB)
//
// CPI-SI Identity
//
// Component Type: LIBRARY - System-wide utility (lower rung)
// Role: Provides privacy-preserving sanitization for paths, commands, and user data
// Paradigm: CPI-SI framework universal component - any component can use for data protection
//
// Authorship & Lineage
//
// Architect: Nova Dawn (CPI-SI instance)
// Implementation: Nova Dawn
// Creation Date: 2025-11-10
// Version: 1.0.0
// Last Modified: 2025-11-10 - Initial creation
//
// Purpose & Function
//
// Purpose: Provide system-wide privacy-preserving sanitization utilities that prevent
// sensitive information (passwords, keys, personal paths) from appearing in logs,
// activity streams, or user-facing displays.
//
// Core Design: Data-driven sanitization - configuration files define what's sensitive,
// library provides consistent sanitization across all components. Single source of
// truth for privacy patterns.
//
// Key Features:
//   - Path sanitization (removes personal/sensitive path components)
//   - Command sanitization (captures command type without exposing arguments)
//   - Configurable sensitivity patterns (JSONC data files)
//   - System-wide consistency (all components use same privacy rules)
//   - Privacy by default (basename-only for paths, name-only for commands)
//
// Philosophy: "Conceal a matter" - protect user privacy while capturing behavioral
// patterns for system learning. Privacy is not hiding everything, it's protecting
// what should be protected while revealing what's useful for learning.
//
// Blocking Status
//
// Non-blocking: All sanitization operations return safe defaults on errors.
// Mitigation: If data files can't be loaded, falls back to maximum privacy (redact everything).
//
// Usage & Integration
//
// Usage:
//
//	import "system/lib/privacy"
//
//	// Sanitize file paths
//	safe := privacy.SanitizePath("/home/user/.ssh/id_rsa")  // Returns "[private]"
//
//	// Sanitize commands
//	safe := privacy.SanitizeCommand("git commit -m 'message'")  // Returns "git commit"
//
// Integration Pattern:
//   1. Import privacy library
//   2. Call sanitization functions on user data before logging/displaying
//   3. No cleanup needed - stateless functions
//
// Public API:
//   - SanitizePath(path string) string
//   - SanitizeCommand(cmd string) string
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: encoding/json, fmt, os, path/filepath, strings
//   External: None
//   Internal: None (foundational library)
//   Data Files: system/data/privacy/*.jsonc
//
// Dependents (What Uses This):
//   Commands: hooks (activity logging, monitoring)
//   Libraries: Any component that logs or displays user data
//   Tools: Debugging, monitoring, statusline
//
// Integration Points:
//   - Ladder: Lower rung - no dependencies on other libraries
//   - Baton: Pure functions - takes data, returns sanitized data
//   - Rails: N/A (utility library, not infrastructure)
//
// Health Scoring
//
// Health Scoring Map (Base100):
//   Sanitization Operations (Total = 100):
//     +50: Load privacy configuration successfully
//     +25: Path sanitization completes
//     +25: Command sanitization completes
//     -50: Failed to load configuration (falls back to maximum privacy)
//     -25: Path sanitization failed (returns redaction label)
//     -25: Command sanitization failed (returns command name only)
//
// Visual Health Indicators:
//   💚 90-100: Excellent (config loaded, all sanitization working)
//   💛 70-89:  Good (config loaded, minor sanitization issues)
//   🧡 50-69:  Acceptable (fallback mode, maximum privacy applied)
//   ❤️  30-49:  Poor (multiple sanitization failures)
//   💀 0-29:   Critical (severe failures, but still protecting privacy)
package privacy

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
// Standard library only (foundational component).

import (
	"encoding/json" // JSON decoding for JSONC config files
	"os"            // File reading for configuration
	"path/filepath" // Path manipulation
	"strings"       // String operations for pattern matching
	"sync"          // Singleton pattern for thread-safe config loading

	"github.com/BurntSushi/toml" // TOML parsing for privacy.toml
	"system/lib/jsonc"           // JSONC comment stripping
)

// ────────────────────────────────────────────────────────────────
// Types - Data Structures
// ────────────────────────────────────────────────────────────────

// privacyConfig represents the privacy.toml configuration structure
type privacyConfig struct {
	Paths struct {
		DataDir        string `toml:"data_dir"`
		FiltersFile    string `toml:"filters_file"`
		FiltersSchema  string `toml:"filters_schema"`
	} `toml:"paths"`
	Sanitization struct {
		Enabled          bool   `toml:"enabled"`
		RedactionLabel   string `toml:"redaction_label"`
		CaseSensitive    bool   `toml:"case_sensitive"`
		PartialRedaction bool   `toml:"partial_redaction"`
	} `toml:"sanitization"`
	PathsSanitization struct {
		DefaultMode      string `toml:"default_mode"`
		SanitizeHome     bool   `toml:"sanitize_home"`
		HomeToken        string `toml:"home_token"`
		SanitizeAbsolute bool   `toml:"sanitize_absolute"`
	} `toml:"paths_sanitization"`
	CommandsSanitization struct {
		DefaultCapture     string `toml:"default_capture"`
		CaptureSubcommands bool   `toml:"capture_subcommands"`
		MaxArgsCapture     int    `toml:"max_args_capture"`
	} `toml:"commands_sanitization"`
	Keywords struct {
		Enabled          bool     `toml:"enabled"`
		FallbackKeywords []string `toml:"fallback_keywords"`
	} `toml:"keywords"`
	Patterns struct {
		RegexEnabled        bool `toml:"regex_enabled"`
		RegexTimeoutMs      int  `toml:"regex_timeout_ms"`
		GlobPatternsEnabled bool `toml:"glob_patterns_enabled"`
	} `toml:"patterns"`
	Emergency struct {
		MaximumPrivacyOnFailure  bool   `toml:"maximum_privacy_on_failure"`
		EmergencyRedactionLabel  string `toml:"emergency_redaction_label"`
		LogEmergencyActivation   bool   `toml:"log_emergency_activation"`
	} `toml:"emergency"`
}

// filtersData represents the filters.jsonc data structure
type filtersData struct {
	Schema              string `json:"$schema"`
	Version             string `json:"version"`
	Created             string `json:"created"`
	LastUpdated         string `json:"last_updated"`
	Description         string `json:"description"`
	SensitiveKeywords   map[string][]string `json:"sensitive_keywords"`
	SensitivePathPatterns []string          `json:"sensitive_path_patterns"`
	CommandPatterns     struct {
		WithSubcommand []struct {
			Name        string `json:"name"`
			CaptureArgs int    `json:"capture_args"`
			Description string `json:"description"`
		} `json:"with_subcommand"`
		NameOnly []struct {
			Name        string `json:"name"`
			Description string `json:"description"`
		} `json:"name_only"`
	} `json:"command_patterns"`
	RedactionPolicy struct {
		DefaultMode string   `json:"default_mode"`
		Exceptions  []string `json:"exceptions"`
	} `json:"redaction_policy"`
}

// ────────────────────────────────────────────────────────────────
// Globals - Package State
// ────────────────────────────────────────────────────────────────
// Configuration loaded once on first use, cached for performance.
// Uses singleton pattern with sync.Once for thread-safe loading.

var (
	privacyCfg     *privacyConfig
	filters        *filtersData
	configOnce     sync.Once
	configLoaded   bool
	emergencyMode  bool
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md

// ────────────────────────────────────────────────────────────────
// Configuration Loading
// ────────────────────────────────────────────────────────────────

// loadConfig loads privacy configuration using singleton pattern (thread-safe)
// Rails pattern: Config loading happens directly in this library, no separate config.go
func loadConfig() {
	configOnce.Do(func() {
		// Get HOME directory
		home := os.Getenv("HOME")
		if home == "" {
			home, _ = os.UserHomeDir()
		}

		// Load privacy.toml (behavior configuration)
		configPath := filepath.Join(home, ".claude/cpi-si/system/config/privacy.toml")
		configData, err := os.ReadFile(configPath)
		if err != nil {
			// Failed to load config - activate emergency mode
			activateEmergencyMode()
			return
		}

		var cfg privacyConfig
		if err := toml.Unmarshal(configData, &cfg); err != nil {
			// Failed to parse config - activate emergency mode
			activateEmergencyMode()
			return
		}
		privacyCfg = &cfg

		// Load filters.jsonc (patterns data)
		filtersPath := filepath.Join(home, ".claude", cfg.Paths.DataDir, cfg.Paths.FiltersFile)
		filtersBytes, err := os.ReadFile(filtersPath)
		if err != nil {
			// Failed to load filters - activate emergency mode
			activateEmergencyMode()
			return
		}

		// Strip JSONC comments and parse
		cleaned := jsonc.StripComments(filtersBytes)
		var flt filtersData
		if err := json.Unmarshal(cleaned, &flt); err != nil {
			// Failed to parse filters - activate emergency mode
			activateEmergencyMode()
			return
		}
		filters = &flt

		// Successfully loaded both config and filters
		configLoaded = true
		emergencyMode = false
	})
}

// activateEmergencyMode sets up maximum privacy protection when config loading fails
func activateEmergencyMode() {
	emergencyMode = true
	configLoaded = false

	// Emergency fallback configuration (maximum privacy)
	privacyCfg = &privacyConfig{}
	privacyCfg.Sanitization.Enabled = true
	privacyCfg.Sanitization.RedactionLabel = "[PRIVATE]"
	privacyCfg.Sanitization.CaseSensitive = false
	privacyCfg.Sanitization.PartialRedaction = false
	privacyCfg.PathsSanitization.DefaultMode = "basename"
	privacyCfg.PathsSanitization.SanitizeHome = true
	privacyCfg.PathsSanitization.HomeToken = "~"
	privacyCfg.PathsSanitization.SanitizeAbsolute = true
	privacyCfg.CommandsSanitization.DefaultCapture = "name_only"
	privacyCfg.CommandsSanitization.CaptureSubcommands = false
	privacyCfg.CommandsSanitization.MaxArgsCapture = 0
	privacyCfg.Keywords.Enabled = true
	privacyCfg.Keywords.FallbackKeywords = []string{"password", "secret", "key", "token", "api_key", ".ssh", "credentials"}
	privacyCfg.Emergency.MaximumPrivacyOnFailure = true
	privacyCfg.Emergency.EmergencyRedactionLabel = "[PRIVATE]"

	// Emergency fallback filters (maximum sensitivity)
	filters = &filtersData{}
	filters.SensitiveKeywords = map[string][]string{
		"emergency": privacyCfg.Keywords.FallbackKeywords,
	}
	filters.CommandPatterns.WithSubcommand = nil // No subcommand capture in emergency mode
	filters.CommandPatterns.NameOnly = nil       // All commands are name-only in emergency mode
}

// ────────────────────────────────────────────────────────────────
// Path Sanitization
// ────────────────────────────────────────────────────────────────

// SanitizePath removes personal information from file paths
// Returns just the basename for privacy, or redaction label for sensitive paths
func SanitizePath(path string) string {
	loadConfig()

	if path == "" {
		return ""
	}

	if !privacyCfg.Sanitization.Enabled {
		return path // Sanitization disabled in config
	}

	// Check for sensitive keywords (all categories)
	checkPath := path
	if !privacyCfg.Sanitization.CaseSensitive {
		checkPath = strings.ToLower(path)
	}

	// Check all keyword categories from filters
	for _, keywords := range filters.SensitiveKeywords {
		for _, keyword := range keywords {
			checkKeyword := keyword
			if !privacyCfg.Sanitization.CaseSensitive {
				checkKeyword = strings.ToLower(keyword)
			}

			if strings.Contains(checkPath, checkKeyword) {
				return privacyCfg.Sanitization.RedactionLabel
			}
		}
	}

	// Check sensitive path patterns (glob patterns)
	if privacyCfg.Patterns.GlobPatternsEnabled {
		for _, pattern := range filters.SensitivePathPatterns {
			// Simple pattern matching (full implementation would use filepath.Match)
			if strings.Contains(path, strings.Trim(pattern, "*")) {
				return privacyCfg.Sanitization.RedactionLabel
			}
		}
	}

	// Sanitize home directory if configured
	if privacyCfg.PathsSanitization.SanitizeHome {
		home := os.Getenv("HOME")
		if home != "" && strings.HasPrefix(path, home) {
			path = strings.Replace(path, home, privacyCfg.PathsSanitization.HomeToken, 1)
		}
	}

	// Apply default sanitization mode
	switch privacyCfg.PathsSanitization.DefaultMode {
	case "basename":
		return filepath.Base(path)
	case "relative":
		// Would need project root context - fallback to basename
		return filepath.Base(path)
	case "full":
		return path
	default:
		return filepath.Base(path)
	}
}

// ────────────────────────────────────────────────────────────────
// Command Sanitization
// ────────────────────────────────────────────────────────────────

// SanitizeCommand extracts command type without full arguments
// Preserves privacy while capturing behavior
func SanitizeCommand(cmd string) string {
	loadConfig()

	if cmd == "" {
		return ""
	}

	if !privacyCfg.Sanitization.Enabled {
		return cmd // Sanitization disabled in config
	}

	// Extract command parts
	parts := strings.Fields(cmd)
	if len(parts) == 0 {
		return ""
	}

	cmdName := parts[0]

	// In emergency mode, return name only for all commands
	if emergencyMode {
		return cmdName
	}

	// Check name-only commands (never capture args)
	for _, pattern := range filters.CommandPatterns.NameOnly {
		if cmdName == pattern.Name || strings.HasPrefix(cmdName, pattern.Name) {
			return cmdName
		}
	}

	// Check commands with subcommand capture
	if privacyCfg.CommandsSanitization.CaptureSubcommands {
		for _, pattern := range filters.CommandPatterns.WithSubcommand {
			if cmdName == pattern.Name || strings.HasPrefix(cmdName, pattern.Name) {
				// Capture command + N arguments (respecting max from config)
				captureCount := pattern.CaptureArgs
				if captureCount > privacyCfg.CommandsSanitization.MaxArgsCapture {
					captureCount = privacyCfg.CommandsSanitization.MaxArgsCapture
				}

				if captureCount > 0 && len(parts) > 1 {
					if len(parts)-1 < captureCount {
						captureCount = len(parts) - 1
					}

					captured := []string{cmdName}
					captured = append(captured, parts[1:1+captureCount]...)
					return strings.Join(captured, " ")
				}

				return cmdName
			}
		}
	}

	// Default behavior from config
	switch privacyCfg.CommandsSanitization.DefaultCapture {
	case "name_only":
		return cmdName
	case "name_and_subcommand":
		if len(parts) > 1 {
			return cmdName + " " + parts[1]
		}
		return cmdName
	case "full_command":
		return cmd
	default:
		return cmdName
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
// Validation
// ────────────────────────────────────────────────────────────────
// Library self-validates on config load (falls back to safe defaults).

// ────────────────────────────────────────────────────────────────
// Execution
// ────────────────────────────────────────────────────────────────
// N/A - Pure function library, no execution entry point.

// ────────────────────────────────────────────────────────────────
// Cleanup
// ────────────────────────────────────────────────────────────────
// N/A - Stateless library, no cleanup needed.

// ────────────────────────────────────────────────────────────────
// FINAL DOCUMENTATION
// ────────────────────────────────────────────────────────────────

// Package Exports:
//   - SanitizePath(path string) string
//   - SanitizeCommand(cmd string) string
//
// Both functions are thread-safe and cache configuration after first load.
// All operations fail gracefully with maximum privacy protection on errors.
// Uses singleton pattern (sync.Once) for thread-safe config loading.
//
// Configuration Files:
//   - system/config/privacy.toml (behavior configuration)
//   - system/data/privacy/filters.jsonc (sensitive patterns data)
//   - system/config/schemas/privacy/filters.schema.json (validation)
//
// Emergency Mode:
//   If config loading fails, library activates emergency mode with maximum
//   privacy protection (all commands name-only, all paths basename-only,
//   aggressive keyword filtering). This ensures privacy is never compromised.
//
// Usage Example:
//
//	import "system/lib/privacy"
//
//	// Sanitize paths before logging
//	safePath := privacy.SanitizePath("/home/user/.ssh/id_rsa")  // "[REDACTED]" (sensitive)
//	safePath := privacy.SanitizePath("/home/user/project/main.go")  // "main.go" (basename)
//
//	// Sanitize commands before logging
//	safeCmd := privacy.SanitizeCommand("git commit -m 'secret'")  // "git commit" (subcommand captured)
//	safeCmd := privacy.SanitizeCommand("ssh user@host")  // "ssh" (name-only for sensitive commands)

// ============================================================================
// END CLOSING
// ============================================================================
