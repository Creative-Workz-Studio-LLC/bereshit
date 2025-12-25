// METADATA
//
// Paths Library - CPI-SI System Runtime
//
// Biblical Foundation
//
// Scripture: "In all thy ways acknowledge him, and he shall direct thy paths" - Proverbs 3:6
// Principle: Single source of truth for all file paths - orderly structure honors God
// Anchor: "For God is not the author of confusion, but of peace" - 1 Corinthians 14:33
//
// CPI-SI Identity
//
// Component Type: Core Service (Ladder rung)
// Role: Centralized path configuration - eliminates hardcoded paths across system
// Paradigm: CPI-SI framework component
//
// Authorship & Lineage
//
// Architect: Nova Dawn
// Implementation: Nova Dawn
// Creation Date: 2025-11-12
// Version: 1.0.0
// Last Modified: 2025-11-12 - Initial implementation
//
// Version History:
//   1.0.0 (2025-11-12) - Initial creation - loads paths.toml
//
// Purpose & Function
//
// Purpose: Load and provide access to system-wide path configuration from paths.toml
//
// Core Design: Config-driven path resolution - no hardcoded paths in libraries
//
// Key Features:
//   - Loads paths.toml from system/config/
//   - Provides typed access to all system paths
//   - Single source of truth for path configuration
//   - Eliminates path hardcoding across 19 libraries
//
// Philosophy: All paths come from configuration - code never contains literal paths
//
// Blocking Status
//
// Non-blocking: Returns error if paths.toml missing, caller decides fallback
// Mitigation: Graceful error handling with descriptive messages
//
// Usage & Integration
//
// Usage:
//
//	import "system/lib/paths"
//
// Integration Pattern:
//   1. Call paths.Load() once at program start
//   2. Use returned Paths struct for all path resolution
//   3. No caching - reload if paths.toml changes
//
// Public API (in typical usage order):
//
//   Configuration Loading:
//     Load() (*Paths, error) - Loads paths.toml and returns Paths struct
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: os, path/filepath
//   External: github.com/BurntSushi/toml v1.4.0
//   Internal: None (foundational library)
//
// Dependents (What Uses This):
//   Commands: All commands needing file paths
//   Libraries: All 19 runtime libraries
//   Tools: Build scripts, validation tools
//
// Integration Points:
//   - Loaded once at program start (initialization)
//   - Paths passed to libraries needing path resolution
//   - Replaces hardcoded filepath.Join() calls throughout system
//
// Health Scoring
//
// Total: 100 points
//
// Configuration Loading:
//   - Load paths.toml: +60 (critical - system cannot function without paths)
//   - Parse TOML: +40 (important - must understand config structure)
//
// Note: Scores reflect TRUE impact. Perfect execution = +100. File not found = -60.

package paths

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/BurntSushi/toml"
)

// SETUP - Type Definitions

// Paths contains all system path configuration loaded from paths.toml
type Paths struct {
	Base     BaseConfig     `toml:"base"`
	Session  SessionConfig  `toml:"session"`
	Temporal TemporalConfig `toml:"temporal"`
	Config   ConfigConfig   `toml:"config"`
	Logging  LoggingConfig  `toml:"logging"`
	Privacy  PrivacyConfig  `toml:"privacy"`
	User     UserConfig     `toml:"user"`
	Instance InstanceConfig `toml:"instance"`
	Projects ProjectsConfig `toml:"projects"`
}

type BaseConfig struct {
	SystemDir  string `toml:"system_dir"`
	ConfigDir  string `toml:"config_dir"`
	DataDir    string `toml:"data_dir"`
	LogsDir    string `toml:"logs_dir"`
	RuntimeDir string `toml:"runtime_dir"`
}

type SessionConfig struct {
	StateFile   string `toml:"state_file"`
	PatternsFile string `toml:"patterns_file"`
	SchemaDir   string `toml:"schema_dir"`
	HistoryDir  string `toml:"history_dir"`
}

type TemporalConfig struct {
	CalendarBase       string `toml:"calendar_base"`
	CalendarPersonal   string `toml:"calendar_personal"`
	CalendarShared     string `toml:"calendar_shared"`
	CalendarProjects   string `toml:"calendar_projects"`
	CalendarMilestones string `toml:"calendar_milestones"`
	CalendarTemplates  string `toml:"calendar_templates"`
	CalendarSchemas    string `toml:"calendar_schemas"`
	CelestialDir       string `toml:"celestial_dir"`
	SolarData          string `toml:"solar_data"`
	LunarData          string `toml:"lunar_data"`
	LocationConfig     string `toml:"location_config"`
	PatternsDir        string `toml:"patterns_dir"`
	PatternsUser       string `toml:"patterns_user"`
	PatternsInstance   string `toml:"patterns_instance"`
	PatternsDiscovered string `toml:"patterns_discovered"`
	PatternsSchemas    string `toml:"patterns_schemas"`
	PlannerTemplates   string `toml:"planner_templates"`
	PlannerSchemas     string `toml:"planner_schemas"`
}

type ConfigConfig struct {
	ValidationDir string `toml:"validation_dir"`
	Formatters    string `toml:"formatters"`
	SchemasDir    string `toml:"schemas_dir"`
}

type LoggingConfig struct {
	BaseDir     string `toml:"base_dir"`
	CommandsDir string `toml:"commands_dir"`
	LibrariesDir string `toml:"libraries_dir"`
	ScriptsDir  string `toml:"scripts_dir"`
	SystemDir   string `toml:"system_dir"`
	HooksDir    string `toml:"hooks_dir"`
}

type PrivacyConfig struct {
	FiltersFile string `toml:"filters_file"`
	Schema      string `toml:"schema"`
}

type UserConfig struct {
	BaseDir       string `toml:"base_dir"`
	DefaultConfig string `toml:"default_config"`
}

type InstanceConfig struct {
	BaseDir       string `toml:"base_dir"`
	DefaultConfig string `toml:"default_config"`
}

type ProjectsConfig struct {
	BaseDir      string `toml:"base_dir"`
	ActiveDir    string `toml:"active_dir"`
	ArchivedDir  string `toml:"archived_dir"`
	TemplatesDir string `toml:"templates_dir"`
}

// BODY - Core Functionality

// Load reads paths.toml from system/config/ and returns parsed Paths struct
func Load() (*Paths, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return nil, fmt.Errorf("failed to get home directory: %w", err)
	}

	configPath := filepath.Join(homeDir, ".claude", "cpi-si", "system", "config", "paths.toml")

	data, err := os.ReadFile(configPath)
	if err != nil {
		return nil, fmt.Errorf("failed to read paths.toml (have you created %s?): %w", configPath, err)
	}

	var paths Paths
	if err := toml.Unmarshal(data, &paths); err != nil {
		return nil, fmt.Errorf("failed to parse paths.toml: %w", err)
	}

	return &paths, nil
}

// ResolveFull takes a relative path from config and returns absolute path
// Example: ResolveFull("system/data/session") -> "/home/user/.claude/cpi-si/system/data/session"
func ResolveFull(relativePath string) (string, error) {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return "", fmt.Errorf("failed to get home directory: %w", err)
	}

	return filepath.Join(homeDir, ".claude", "cpi-si", relativePath), nil
}

// CLOSING
// No execution needed - library provides functions for import
