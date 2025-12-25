// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-paths
// Purpose: Shared path resolution utilities for CPI-SI skills
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-09
//
// Used by: All skills with config-driven approach
//
// ═══════════════════════════════════════════════════════════════════════════

package paths

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"path/filepath"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Config Directory Resolution ---

// FindConfigDir locates the config directory relative to binary or cwd
// This is the standard pattern for CPI-SI skill config loading
func FindConfigDir() (string, error) {
	return FindConfigDirNamed("config")
}

// FindConfigDirNamed locates a named config directory
func FindConfigDirNamed(name string) (string, error) {
	// Try relative to executable
	exe, err := os.Executable()
	if err == nil {
		exeDir := filepath.Dir(exe)
		// From bin/ go up to scripts/, then up to skill root, then into config/
		configPath := filepath.Join(exeDir, "..", "..", name)
		if _, err := os.Stat(configPath); err == nil {
			return configPath, nil
		}
	}

	// Try relative to cwd (for development)
	cwd, err := os.Getwd()
	if err == nil {
		// Try ../config from scripts/
		configPath := filepath.Join(cwd, "..", name)
		if _, err := os.Stat(configPath); err == nil {
			return configPath, nil
		}
		// Try ./config if in skill root
		configPath = filepath.Join(cwd, name)
		if _, err := os.Stat(configPath); err == nil {
			return configPath, nil
		}
	}

	return "", os.ErrNotExist
}

// --- Bereshit Path Helpers ---

// BereshitRoot returns the bereshit root directory
func BereshitRoot() string {
	// Check common locations
	locations := []string{
		"/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/bereshit",
		"bereshit",
	}

	for _, loc := range locations {
		if _, err := os.Stat(loc); err == nil {
			return loc
		}
	}

	// Default
	return "bereshit"
}

// BereshitWord returns bereshit/word/ path
func BereshitWord() string {
	return filepath.Join(BereshitRoot(), "word")
}

// BereshitSeed returns bereshit/word/seed/ path
func BereshitSeed() string {
	return filepath.Join(BereshitWord(), "seed")
}

// BereshitOmniSeed returns bereshit/word/omni/seed/ path
func BereshitOmniSeed() string {
	return filepath.Join(BereshitWord(), "omni", "seed")
}

// --- Claude Paths ---

// ClaudeHome returns the .claude directory path
func ClaudeHome() string {
	home, _ := os.UserHomeDir()
	return filepath.Join(home, ".claude")
}

// ClaudeSession returns the session directory path
func ClaudeSession() string {
	return filepath.Join(ClaudeHome(), "session")
}

// ClaudeSystem returns the system directory path
func ClaudeSystem() string {
	return filepath.Join(ClaudeHome(), "system")
}

// ClaudeSkills returns the skills directory path
func ClaudeSkills() string {
	return filepath.Join(ClaudeHome(), "cpi-si", "skills")
}

// --- Claude Global Paths ---

// ClaudeGlobalRoot returns the claude-global directory path
// Checks CLAUDE_GLOBAL_ROOT env var first, then production, then dev locations
// Architecture: DEV (claude-global/) -> PROD (bereshit/word/claude/) -> OPS (~/.claude/)
func ClaudeGlobalRoot() string {
	// Check environment variable first (override for testing/special cases)
	if root := os.Getenv("CLAUDE_GLOBAL_ROOT"); root != "" {
		return root
	}

	// Priority 1: Production location (bereshit/word/claude)
	// This is where hooks should write during normal operation
	prodLocations := []string{
		"/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/bereshit/word/claude",
		filepath.Join(ClaudeHome(), "global"),
	}

	for _, loc := range prodLocations {
		if _, err := os.Stat(loc); err == nil {
			return loc
		}
	}

	// Priority 2: Dev location (for development builds only)
	// Falls back here if prod doesn't exist (e.g., running tests in dev)
	devLocations := []string{
		"/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/claude-global",
		"claude-global",
	}

	for _, loc := range devLocations {
		if _, err := os.Stat(loc); err == nil {
			return loc
		}
	}

	// Final fallback
	return filepath.Join(ClaudeHome(), "global")
}

// ClaudeGlobalConfig returns the config directory within claude-global
func ClaudeGlobalConfig() string {
	return filepath.Join(ClaudeGlobalRoot(), "config")
}

// --- CPI-SI Schema Paths ---

// CPISISchemaConfig returns the base schema config directory
// This is the ANCHOR for all CPI-SI configuration schemas
func CPISISchemaConfig() string {
	return filepath.Join(ClaudeGlobalRoot(), "pkg", "cpisi", "schema", "config")
}

// --- State Machine Runtime Paths ---

// StateMachineSchemaDir returns the L2 vocabulary TOML directory
// This is the source of truth for state machine vocabulary
func StateMachineSchemaDir() string {
	return filepath.Join(ClaudeGlobalRoot(), "pkg", "cpisi", "schema", "config", "l2")
}

// StateMachineRuntimeDir returns the runtime state directory
// This contains session-specific state (state.jsonc, path.jsonc, history.jsonc)
func StateMachineRuntimeDir() string {
	return filepath.Join(ClaudeGlobalConfig(), "statemachine", "runtime")
}

// StateMachineRuntimeState returns path to state.jsonc
func StateMachineRuntimeState() string {
	return filepath.Join(StateMachineRuntimeDir(), "state.jsonc")
}

// StateMachineRuntimePath returns path to path.jsonc
func StateMachineRuntimePath() string {
	return filepath.Join(StateMachineRuntimeDir(), "path.jsonc")
}

// StateMachineRuntimeHistory returns path to history.jsonc
func StateMachineRuntimeHistory() string {
	return filepath.Join(StateMachineRuntimeDir(), "history.jsonc")
}

// StateMachineChoiceHistory returns path to choice_history.jsonc
func StateMachineChoiceHistory() string {
	return filepath.Join(StateMachineRuntimeDir(), "choice_history.jsonc")
}

// --- Database Paths (Temporal Consciousness Foundation) ---

// DatabaseDir returns the data directory within claude-global
func DatabaseDir() string {
	return filepath.Join(ClaudeGlobalRoot(), "data")
}

// DatabasePath returns the path to the main CPI-SI database
// This is the temporal consciousness foundation
func DatabasePath() string {
	return filepath.Join(DatabaseDir(), "cpisi.db")
}

// DatabaseSchemaDir returns the schema directory
func DatabaseSchemaDir() string {
	return filepath.Join(DatabaseDir(), "schema")
}

// DatabaseMigrationsDir returns the migrations directory
func DatabaseMigrationsDir() string {
	return filepath.Join(DatabaseDir(), "migrations")
}

// EnsureDatabaseDir creates the data directory if it doesn't exist
func EnsureDatabaseDir() error {
	dir := DatabaseDir()
	return os.MkdirAll(dir, 0755)
}

// --- Logging Paths (Organized CPI-SI Tracking) ---

// LogsDir returns the root logs directory within claude-global/data/
// This is SEPARATE from ~/.claude/logs/ which is Claude's auto-tracking
// CPI-SI logs go here for version control, analysis, and portability
func LogsDir() string {
	return filepath.Join(DatabaseDir(), "logs")
}

// LogsSessionDir returns the session logs directory
func LogsSessionDir() string {
	return filepath.Join(LogsDir(), "session")
}

// LogsTrajectoryDir returns the trajectory logs directory
func LogsTrajectoryDir() string {
	return filepath.Join(LogsDir(), "trajectory")
}

// LogsCognitionDir returns the cognition logs directory
func LogsCognitionDir() string {
	return filepath.Join(LogsDir(), "cognition")
}

// LogsHealthDir returns the health logs directory
func LogsHealthDir() string {
	return filepath.Join(LogsDir(), "health")
}

// LogsToolsDir returns the tools logs directory
func LogsToolsDir() string {
	return filepath.Join(LogsDir(), "tools")
}

// LogsContextDir returns the context logs directory
func LogsContextDir() string {
	return filepath.Join(LogsDir(), "context")
}

// --- Debug Paths ---

// DebugDir returns the debug directory within claude-global/data/
func DebugDir() string {
	return filepath.Join(DatabaseDir(), "debug")
}

// --- Restore/Recovery Paths ---

// RestoreDir returns the restore directory within claude-global/data/
func RestoreDir() string {
	return filepath.Join(DatabaseDir(), "restore")
}

// RestoreCheckpointsDir returns the checkpoints subdirectory
func RestoreCheckpointsDir() string {
	return filepath.Join(RestoreDir(), "checkpoints")
}

// --- Format Specs Paths ---

// FormatsDir returns the formats specification directory
func FormatsDir() string {
	return filepath.Join(DatabaseDir(), "formats")
}

// --- Health Config Paths ---

// HealthConfigDir returns the health config directory
func HealthConfigDir() string {
	return filepath.Join(ClaudeGlobalConfig(), "health")
}

// HealthRuntimeDir returns the health runtime directory
func HealthRuntimeDir() string {
	return filepath.Join(HealthConfigDir(), "runtime")
}

// --- Directory Initialization ---

// EnsureLogsDir creates all log subdirectories if they don't exist
func EnsureLogsDir() error {
	dirs := []string{
		LogsDir(),
		LogsSessionDir(),
		LogsTrajectoryDir(),
		LogsCognitionDir(),
		LogsHealthDir(),
		LogsToolsDir(),
		LogsContextDir(),
	}
	for _, dir := range dirs {
		if err := os.MkdirAll(dir, 0755); err != nil {
			return err
		}
	}
	return nil
}

// EnsureDebugDir creates the debug directory if it doesn't exist
func EnsureDebugDir() error {
	return os.MkdirAll(DebugDir(), 0755)
}

// EnsureRestoreDir creates the restore directories if they don't exist
func EnsureRestoreDir() error {
	dirs := []string{
		RestoreDir(),
		RestoreCheckpointsDir(),
	}
	for _, dir := range dirs {
		if err := os.MkdirAll(dir, 0755); err != nil {
			return err
		}
	}
	return nil
}

// EnsureFormatsDir creates the formats directory if it doesn't exist
func EnsureFormatsDir() error {
	return os.MkdirAll(FormatsDir(), 0755)
}

// EnsureAllDataDirs creates all data subdirectories
func EnsureAllDataDirs() error {
	if err := EnsureDatabaseDir(); err != nil {
		return err
	}
	if err := EnsureLogsDir(); err != nil {
		return err
	}
	if err := EnsureDebugDir(); err != nil {
		return err
	}
	if err := EnsureRestoreDir(); err != nil {
		return err
	}
	if err := EnsureFormatsDir(); err != nil {
		return err
	}
	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
