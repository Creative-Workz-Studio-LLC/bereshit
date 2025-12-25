// ============================================================================
// METADATA
// ============================================================================
// Configuration Management - Logging Library
//
// Biblical Foundation
//
// Scripture: "Let all things be done decently and in order" (1 Corinthians 14:40, KJV)
// Principle: Orderly configuration management separates settings (data) from behavior (code)
// Anchor: Configuration is data that governs behavior. Maximum configurability enables decades of use without recompilation. Settings change, code endures.
//
// CPI-SI Identity
//
// Component Type: Configuration module within Rails infrastructure
// Role: Load and manage logging configuration from TOML files
// Paradigm: CPI-SI framework component
//
// Authorship & Lineage
//
// Architect: Seanje Lenox-Wise, Nova Dawn
// Implementation: Nova Dawn
// Creation Date: 2025-11-18
// Version: 1.0.0
// Last Modified: 2025-11-18 - Extracted from monolithic logger.go
//
// Purpose & Function
//
// Purpose: Provide configuration loading and management for the logging system. Loads settings from logging.toml and provides graceful fallback to hardcoded defaults when configuration unavailable.
//
// Core Design: Multi-layer tripwire pattern - attempt config load, gracefully degrade to defaults on failure, never block execution.
//
// Key Features:
//   - TOML configuration loading from ~/.claude/cpi-si/system/config/logging.toml
//   - Graceful fallback to hardcoded defaults
//   - Thread-safe single initialization (sync.Once)
//   - Comprehensive configuration structure matching all logging.toml sections
//
// Blocking Status
//
// Non-blocking: Configuration loading failures never stop the system. If TOML unavailable, use hardcoded defaults and continue.
// Mitigation: All failures degrade gracefully with default values that allow logging to function.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/logging"
//
// Integration Pattern:
//   1. Call LoadConfig() to ensure configuration loaded
//   2. Access Config variable for configuration values
//   3. Check ConfigLoaded to know if using TOML or defaults
//
// Public API:
//
//   LoadConfig() - Ensure configuration loaded (idempotent, thread-safe)
//   Config - Package-level configuration variable (read-only after init)
//   ConfigLoaded - Boolean indicating successful TOML load
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: os, path/filepath, sync
//   External: github.com/BurntSushi/toml (DATA dependency for config parsing)
//
// Dependents (What Uses This):
//   Internal: health.go, context.go, entry.go, writing.go, logger.go
//
// Health Scoring
//
// Base100 scoring algorithm (CPSI-ALG-001).
//
// Configuration Loading (100 pts):
//   - TOML file loading: +60 (success), +30 (partial), 0 (all defaults)
//   - Default fallback: +20 (graceful fallback when needed)
//   - Thread safety: +20 (sync.Once prevents race conditions)
//
// Note: This module's health is about reliable configuration availability, not logging quality.

package logging

// ============================================================================
// SETUP
// ============================================================================

// Imports

import (
	"os"
	"path/filepath"
	"sync"

	"github.com/BurntSushi/toml"
)

// Types - Configuration Structure

// LoggingConfig represents the complete logging.toml configuration structure.
type LoggingConfig struct {
	Paths          PathsConfig          `toml:"paths"`
	Format         FormatConfig         `toml:"format"`
	Files          FilesConfig          `toml:"files"`
	ContextCapture ContextCaptureConfig `toml:"context_capture"`
	Behavior       BehaviorConfig       `toml:"behavior"`
	Messages       MessagesConfig       `toml:"messages"`
	HealthImpacts  HealthImpactsConfig  `toml:"health_impacts"`
	Retention      RetentionConfig      `toml:"retention"`
	Rotation       RotationConfig       `toml:"rotation"`
	Routing        RoutingConfig        `toml:"routing"`
	Health         HealthConfig         `toml:"health"`
}

// PathsConfig defines base directory configuration.
type PathsConfig struct {
	BaseDir string `toml:"base_dir"`
}

// FormatConfig defines log output formatting.
type FormatConfig struct {
	TimestampFormat    string `toml:"timestamp_format"`
	ContextHeader      string `toml:"context_header"`
	EventHeader        string `toml:"event_header"`
	DetailsHeader      string `toml:"details_header"`
	InteractionsHeader string `toml:"interactions_header"`
	EntrySeparator     string `toml:"entry_separator"`
	LogFilePermissions string `toml:"log_file_permissions"`
	LogDirPermissions  string `toml:"log_dir_permissions"`
	WarnLogOpenFailed  string `toml:"warn_log_open_failed"`
	WarnLogWriteFailed string `toml:"warn_log_write_failed"`
}

// FilesConfig defines file system configuration.
type FilesConfig struct {
	LogFileExtension string `toml:"log_file_extension"`
	RotatedLogFormat string `toml:"rotated_log_format"`
	ContextIDFormat  string `toml:"context_id_format"`
}

// ContextCaptureConfig defines system context capture formatting.
type ContextCaptureConfig struct {
	SudoersValidPerms  string `toml:"sudoers_valid_perms"`
	FrameworkEnvPrefix string `toml:"framework_env_prefix"`
	PermissionsFormat  string `toml:"permissions_format"`
	LoadAvgFormat      string `toml:"load_avg_format"`
	MemoryUsageFormat  string `toml:"memory_usage_format"`
	DiskUsageFormat    string `toml:"disk_usage_format"`
	UnknownValue       string `toml:"unknown_value"`
}

// BehaviorConfig defines logging behavior policies.
type BehaviorConfig struct {
	StackBufferSize     int             `toml:"stack_buffer_size"`
	LogLevelFullContext map[string]bool `toml:"log_level_full_context"`
}

// MessagesConfig defines user-facing messages and event formats.
type MessagesConfig struct {
	EventOpStart    string `toml:"event_op_start"`
	EventCheckMsg   string `toml:"event_check_msg"`
	EventSnapshot   string `toml:"event_snapshot"`
	EventCmdFailed  string `toml:"event_cmd_failed"`
	EventCmdSuccess string `toml:"event_cmd_success"`
	CmdFullFormat   string `toml:"cmd_full_format"`
	DurationFormat  string `toml:"duration_format"`
}

// HealthImpactsConfig defines default health impact values.
type HealthImpactsConfig struct {
	CmdOperationImpact int `toml:"cmd_operation_impact"`
	CmdFailureImpact   int `toml:"cmd_failure_impact"`
	CmdSuccessImpact   int `toml:"cmd_success_impact"`
}

// RetentionConfig defines log retention policies.
type RetentionConfig struct {
	DailyDays         int    `toml:"daily_days"`
	WeeklyDays        int    `toml:"weekly_days"`
	MonthlyDays       int    `toml:"monthly_days"`
	QuarterlyDays     int    `toml:"quarterly_days"`
	YearlyPermanent   bool   `toml:"yearly_permanent"`
	AutoAggregate     bool   `toml:"auto_aggregate"`
	AggregateStartup  bool   `toml:"aggregate_on_startup"`
	AggregateSchedule string `toml:"aggregate_schedule"`
}

// RotationConfig defines file size-based rotation settings.
type RotationConfig struct {
	Enabled              bool `toml:"enabled"`
	MaxSizeMB            int  `toml:"max_size_mb"`
	MaxFilesPerComponent int  `toml:"max_files_per_component"`
	CompressRotated      bool `toml:"compress_rotated"`
}

// RoutingConfig maps component names to log subdirectories.
type RoutingConfig struct {
	Commands  []string `toml:"commands"`
	Libraries []string `toml:"libraries"`
	Scripts   []string `toml:"scripts"`
}

// HealthConfig defines health score visualization thresholds.
type HealthConfig struct {
	Ranges []HealthRange `toml:"ranges"`
}

// HealthRange defines a health threshold with visual indicator.
type HealthRange struct {
	Threshold   int    `json:"threshold"`
	Emoji       string `json:"emoji"`
	Description string `json:"description"`
}

// Package-Level State

// Config holds the loaded configuration (nil until LoadConfig called).
var Config *LoggingConfig

// configOnce ensures configuration loads exactly once (thread-safe).
var configOnce sync.Once

// ConfigLoaded indicates whether TOML config loaded successfully.
var ConfigLoaded bool

// init loads configuration on package initialization.
//
// NOTE: Configuration loading implementation will be added in Phase 7.
// This init() function is the attachment point for that implementation.
//
// Phase 7 will implement:
//   - Build config file path (home dir + system/config/logging.toml)
//   - Load and parse TOML
//   - Set ConfigLoaded = true on success
//   - Leave ConfigLoaded = false on failure (graceful degradation)
//
// For now, ConfigLoaded remains false, so other files use hardcoded constants.
func init() {
	// Configuration loading placeholder
	// Will be implemented in Phase 7 with tripwire pattern
}

// ============================================================================
// BODY
// ============================================================================

// Configuration Loading

// LoadConfig loads logging.toml configuration from ~/.claude/cpi-si/system/config/logging.toml.
// Uses sync.Once for thread-safe single initialization. Falls back to defaults if loading fails.
func LoadConfig() {
	configOnce.Do(func() {
		// Construct config path
		homeDir, err := os.UserHomeDir()
		if err != nil {
			// Fallback to defaults if can't get home directory
			useDefaultConfig()
			return
		}

		configPath := filepath.Join(homeDir, ".claude", "cpi-si", "system", "config", "logging.toml")

		// Load TOML config
		var cfg LoggingConfig
		if _, err := toml.DecodeFile(configPath, &cfg); err != nil {
			// Fallback to defaults if config file doesn't exist or is invalid
			useDefaultConfig()
			return
		}

		// Config loaded successfully
		Config = &cfg
		ConfigLoaded = true
	})
}

// useDefaultConfig initializes config with hardcoded defaults (fallback when logging.toml unavailable).
func useDefaultConfig() {
	Config = &LoggingConfig{
		Paths: PathsConfig{
			BaseDir: "cpi-si/output/logs",
		},
		Retention: RetentionConfig{
			DailyDays:         60,
			WeeklyDays:        180,
			MonthlyDays:       730,
			QuarterlyDays:     1825,
			YearlyPermanent:   true,
			AutoAggregate:     true,
			AggregateStartup:  false,
			AggregateSchedule: "weekly",
		},
		Rotation: RotationConfig{
			Enabled:              true,
			MaxSizeMB:            10,
			MaxFilesPerComponent: 5,
			CompressRotated:      true,
		},
		Routing: RoutingConfig{
			Commands:  []string{"validate", "test", "status", "diagnose"},
			Libraries: []string{"operations", "sudoers", "environment", "display", "logging", "debugging"},
			Scripts:   []string{"build"},
		},
		Health: HealthConfig{
			Ranges: []HealthRange{
				// Positive gradient
				{90, "💚", "Excellent - all systems healthy"},
				{80, "💙", "Very Good - minor issues only"},
				{70, "💛", "Good - some concerns"},
				{60, "🧡", "Above Average - noticeable issues"},
				{50, "❤️", "Average - mixed results"},
				{40, "🤍", "Below Average - attention needed"},
				{30, "💔", "Fair - significant problems"},
				{20, "🩹", "Poor - major issues"},
				{10, "⚠️", "Warning - critical attention needed"},
				{1, "☠️", "Critical - near failure"},
				// Neutral
				{0, "⚫", "Neutral/Reset - balanced state"},
				// Negative gradient
				{-9, "🔴", "Slight Negative - minor damage"},
				{-19, "🟠", "Negative - noticeable degradation"},
				{-29, "🟡", "Declining - system weakening"},
				{-39, "🟢", "Degraded - significant damage"},
				{-49, "🔵", "Damaged - major problems"},
				{-59, "🟣", "Severe - critical damage"},
				{-69, "🟤", "Critical - near failure"},
				{-79, "⚫", "Failing - barely functional"},
				{-89, "⬛", "Near Death - almost gone"},
				{-100, "💀", "Dead - complete failure"},
			},
		},
	}
	ConfigLoaded = false // Mark as using defaults, not loaded from file
}

// ============================================================================
// CLOSING
// ============================================================================
// Library module (no entry point). Import: "system/runtime/lib/logging"
//
// ============================================================================
// END CLOSING
// ============================================================================
