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
		"/media/seanje-lenox-wise/Project/Bereshit",
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

// BereshitCore returns bereshit/word/core/ path - the canonical TOML specs
// This is the SOURCE OF TRUTH for all CPI-SI configuration schemas
// Cornerstone pattern: Read from canonical specs at runtime
func BereshitCore() string {
	return filepath.Join(BereshitWord(), "core")
}

// BereshitCoreHealth returns bereshit/word/core/os/health/ path
func BereshitCoreHealth() string {
	return filepath.Join(BereshitCore(), "os", "health")
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
		"/media/seanje-lenox-wise/Project/Bereshit/word/claude",
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
// Config-driven pattern: Falls back to Bereshit/word/core/ (canonical TOML specs)
func CPISISchemaConfig() string {
	// Priority 1: Local schema (if exists)
	localSchema := filepath.Join(ClaudeGlobalRoot(), "pkg", "cpisi", "schema", "config")
	if _, err := os.Stat(localSchema); err == nil {
		return localSchema
	}

	// Priority 2: Bereshit word/core (canonical TOML specs - Cornerstone pattern)
	bereshitCore := BereshitCore()
	if _, err := os.Stat(bereshitCore); err == nil {
		return bereshitCore
	}

	// Fallback to local (will fail gracefully on load)
	return localSchema
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
// JSONC files, schemas, logs, and output stay here (on project drive)
func DatabaseDir() string {
	return filepath.Join(ClaudeGlobalRoot(), "data")
}

// DatabaseBasePath returns the directory where all SQLite databases live.
//
// Resolution order:
//  1. CWS_DATA_DIR env var — production servers set this to consolidate all
//     data under the service directory (e.g., /home/cws/data/databases).
//     No hardcoded user paths, works on any CWS server.
//  2. ~/.local/share/claude/data — workstation default. On the dev workstation
//     the project drive is exFAT (no WAL file locking), so SQLite databases
//     must live on the native ext4 filesystem.
//
// The env var pattern mirrors CLAUDE_GLOBAL_ROOT: production configures via
// environment, workstation uses sensible defaults.
func DatabaseBasePath() string {
	if dir := os.Getenv("CWS_DATA_DIR"); dir != "" {
		return dir
	}
	home, _ := os.UserHomeDir()
	return filepath.Join(home, ".local", "share", "claude", "data")
}

// DatabasePath returns the path to the legacy monolithic CPI-SI database.
// Deprecated: Use domain-specific paths (SessionsDBPath, CognitionDBPath, etc.)
func DatabasePath() string {
	return filepath.Join(DatabaseBasePath(), "cpisi.db")
}

// --- Domain Database Paths ---

// SessionsDBPath returns the path to sessions.db (session & exchange lifecycle)
func SessionsDBPath() string {
	return filepath.Join(DatabaseBasePath(), "sessions.db")
}

// CognitionDBPath returns the path to cognition.db (mental construct & cognitive state)
func CognitionDBPath() string {
	return filepath.Join(DatabaseBasePath(), "cognition.db")
}

// GrowthDBPath returns the path to growth.db (learning & pattern recognition)
func GrowthDBPath() string {
	return filepath.Join(DatabaseBasePath(), "growth.db")
}

// TemporalDBPath returns the path to temporal.db (time & calendar awareness)
func TemporalDBPath() string {
	return filepath.Join(DatabaseBasePath(), "temporal.db")
}

// ProjectsDBPath returns the path to projects.db (work tracking)
func ProjectsDBPath() string {
	return filepath.Join(DatabaseBasePath(), "projects.db")
}

// DatabaseSchemaDir returns the schema directory
func DatabaseSchemaDir() string {
	return filepath.Join(DatabaseDir(), "schema")
}

// DatabaseMigrationsDir returns the migrations directory
func DatabaseMigrationsDir() string {
	return filepath.Join(DatabaseDir(), "migrations")
}

// EnsureDatabaseDir creates both the data directory (JSONC/logs) and the
// database directory (SQLite .db files) if they don't exist.
//
// On workstation: these are two different paths (exFAT project drive vs ext4 home).
// On production: CWS_DATA_DIR + CLAUDE_GLOBAL_ROOT both point under the service
// directory, so these may overlap — MkdirAll handles that gracefully.
func EnsureDatabaseDir() error {
	// Data dir (JSONC, logs, output — project drive on workstation)
	if err := os.MkdirAll(DatabaseDir(), 0755); err != nil {
		return err
	}
	// DB dir (SQLite — ext4 on workstation, service dir on production)
	return os.MkdirAll(DatabaseBasePath(), 0755)
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

// --- Output Paths ---

// OutputDir returns the structured output directory
func OutputDir() string {
	return filepath.Join(DatabaseDir(), "output")
}

// OutputExportsDir returns the exports subdirectory for data exports
func OutputExportsDir() string {
	return filepath.Join(OutputDir(), "exports")
}

// OutputReportsDir returns the reports subdirectory for generated reports
func OutputReportsDir() string {
	return filepath.Join(OutputDir(), "reports")
}

// OutputSnapshotsDir returns the snapshots subdirectory for state snapshots
func OutputSnapshotsDir() string {
	return filepath.Join(OutputDir(), "snapshots")
}

// OutputTemplatesDir returns the templates subdirectory for output format templates
func OutputTemplatesDir() string {
	return filepath.Join(OutputDir(), "templates")
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

// EnsureOutputDir creates the output directories if they don't exist
func EnsureOutputDir() error {
	dirs := []string{
		OutputDir(),
		OutputExportsDir(),
		OutputReportsDir(),
		OutputSnapshotsDir(),
		OutputTemplatesDir(),
	}
	for _, dir := range dirs {
		if err := os.MkdirAll(dir, 0755); err != nil {
			return err
		}
	}
	return nil
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
	if err := EnsureOutputDir(); err != nil {
		return err
	}
	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
