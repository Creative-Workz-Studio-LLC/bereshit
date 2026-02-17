//omni:code --go -library
//omni:key B-L0-hybrid-paths
//omni:version b-01.01
//omni:pragma _pragma
//omni:metadata _metadata

// ============================================================================
// METADATA
// ============================================================================

// Package paths provides shared path resolution for the CPI-SI system.
//
// Centralizes all path construction so callers never hardcode locations.
// Three resolution tiers: environment variable → hardcoded default → fallback.
//
// Path families:
//
//	Config:       FindConfigDir, FindConfigDirNamed
//	Bereshit:     BereshitRoot, BereshitWord, BereshitSeed
//	Claude:       ClaudeHome, ClaudeSession, ClaudeSystem, ClaudeSkills
//	Claude Global: ClaudeGlobalRoot, ClaudeGlobalConfig
//	CPI-SI:       CPISISchemaConfig
//	State Machine: StateMachine* (schema, runtime, state, path, history)
//	Database:     DatabaseDir, DatabasePath, DatabaseSchemaDir
//	Logging:      LogsDir, LogsSessionDir, Logs*Dir
//	Health:       HealthConfigDir, HealthRuntimeDir
//	Debug/Restore: DebugDir, RestoreDir, RestoreCheckpointsDir
//	Formats:      FormatsDir
//	Ensure:       EnsureDatabaseDir, EnsureLogsDir, EnsureAllDataDirs
//
// Environment variables:
//
//	BERESHIT_ROOT       — Override Bereshit project root
//	CLAUDE_GLOBAL_ROOT  — Override claude-global root
//
// See _pragma and _metadata for OmniCode identity.
package paths

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- I.1 Standard Library [IMPORT] (1) ---
// OS interaction, path construction, caching
import (
	"os"            // Stat, Getenv, UserHomeDir, MkdirAll, Executable, Getwd
	"path/filepath" // Join, Dir
	"sync"          // Once — cache immutable lookups
)

//--- I.3 Internal Packages [IMPORT] (0) ---
// L0 identity registry — package self-awareness
import (
	"creativeworkzstudio.com/bereshit/L0-universal/spiral/identity" // Register — package identity registry
)

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

// _pragma — OmniCode file identity (P1-P5). Runtime-queryable.
var _pragma = map[string]string{
	// P1: Core Identity
	"P1.key":    "B-L0-hybrid-paths",
	"P1.type":   "code",
	"P1.format": "go",
	"P1.style":  "library",
	// P2: Structure
	"P2.blocks": "4-block",
	// P3: Derivation
	"P3.from":    "b-word/seed/code/L0/go/library.go",
	"P3.derives": "B-L0-seed-go-library",
	// P4: Version
	"P4.at": "b-01.01",
	// P5: Summary
	"P5.title":   "Path Resolution Library",
	"P5.summary": "Centralized path construction for all CPI-SI system components",
}

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

// _metadata — OmniCode file metadata (M1-M10). Runtime-queryable.
var _metadata = map[string]string{

	// ── M1: Core Identity ─────────────────────────────────────────
	"M1.key":            "B-L0-hybrid-paths",
	"M1.component_type": "Foundation",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2025-12-09",

	// ── M2: Version History ───────────────────────────────────────
	"M2.b-01.01": "2026-02-14 — REORG: promoted from hybrid/util/ to hybrid/ top-level domain",
	"M2.b-01.00": "2026-02-14 — BASELINE: 4-block, identity, home caching, env fallbacks",
	"M2.a-01.00": "2025-12-09 — Initial creation",

	// ── M3: Interface ─────────────────────────────────────────────
	"M3.requires.stdlib":   "os, path/filepath, sync",
	"M3.requires.external": "none",
	"M3.requires.internal": "L0-universal/spiral/identity",
	"M3.used_by":           "all CPI-SI skills, config loaders, state machine, database, logging",
	"M3.import":            "creativeworkzstudio.com/bereshit/L0-universal/hybrid/paths",
	"M3.pattern":           "paths.BereshitRoot(), paths.ClaudeHome(), paths.DatabasePath()",

	// ── M4: Public API ────────────────────────────────────────────
	"M4.config":       "FindConfigDir, FindConfigDirNamed",
	"M4.bereshit":     "BereshitRoot, BereshitWord, BereshitSeed, BereshitOmniSeed",
	"M4.claude":       "ClaudeHome, ClaudeSession, ClaudeSystem, ClaudeSkills",
	"M4.claude_global": "ClaudeGlobalRoot, ClaudeGlobalConfig",
	"M4.cpisi":        "CPISISchemaConfig",
	"M4.statemachine": "StateMachineSchemaDir, StateMachineRuntimeDir, StateMachineRuntimeState, StateMachineRuntimePath, StateMachineRuntimeHistory, StateMachineChoiceHistory",
	"M4.database":     "DatabaseDir, DatabasePath, DatabaseSchemaDir, DatabaseMigrationsDir",
	"M4.logging":      "LogsDir, LogsSessionDir, LogsTrajectoryDir, LogsCognitionDir, LogsHealthDir, LogsToolsDir, LogsContextDir",
	"M4.health":       "HealthConfigDir, HealthRuntimeDir",
	"M4.debug":        "DebugDir, RestoreDir, RestoreCheckpointsDir, FormatsDir",
	"M4.ensure":       "EnsureDatabaseDir, EnsureLogsDir, EnsureDebugDir, EnsureRestoreDir, EnsureFormatsDir, EnsureAllDataDirs",
	"M4.self":         "Pragma, Metadata",

	// ── M5: Operational ───────────────────────────────────────────
	"M5.blocking": "no — path construction + optional os.Stat checks",
	"M5.health":   "Foundation | granted: resolved | deferred: n/a | denied: not found",

	// ── M6: Classification ────────────────────────────────────────
	"M6.tags":     "paths, resolution, config, bereshit, claude, database, logging",
	"M6.category": "Foundation",
	"M6.domain":   "filesystem",
	"M6.layer":    "L0-universal",
	"M6.paradigm": "CPI-SI",

	// ── M7: Intent ────────────────────────────────────────────────
	"M7.purpose":    "Single source of truth for all system paths — no hardcoded paths in callers",
	"M7.philosophy": "Centralize path logic so changes propagate from one place",
	"M7.provides":   "Path resolution for config, bereshit, claude, database, logging, state machine",

	// ── M8: Grounding ─────────────────────────────────────────────
	"M8.scripture": "Proverbs 4:26 — Ponder the path of thy feet",
	"M8.principle": "Know where you walk — every path resolved before traversed",
	"M8.anchor":    "Genesis 1:1 — In the beginning God created the heaven and the earth",

	// ── M9: Dependencies ──────────────────────────────────────────
	"M9.needs.stdlib":   "os (Stat, Getenv, UserHomeDir, MkdirAll), path/filepath (Join), sync (Once)",
	"M9.needs.external": "none",
	"M9.needs.internal": "L0-universal/spiral/identity",
	"M9.used_by":        "all CPI-SI packages that access filesystem",
	"M9.layer_deps":     "L0 only — no L1+ dependencies",

	// ── M10: Roadmap ──────────────────────────────────────────────
	"M10.current":     "b-01.00 — BASELINE: path resolution with identity and home caching",
	"M10.planned":     "XDG compliance, Windows support, path validation, path existence caching",
	"M10.limitations": "CWS workstation paths hardcoded as defaults, no XDG, Linux only",
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Variables (cached home directory)

// ────────────────────────────────────────────────────────────────
// Variables — Cached Lookups
// ────────────────────────────────────────────────────────────────

var (
	homeOnce sync.Once  // guards homeDir initialization
	homeDir  string     // cached result of os.UserHomeDir()
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Identity → Helpers → Config Resolution → Bereshit Paths →
//                Claude Paths → Claude Global → CPI-SI Schema →
//                State Machine → Database → Logging → Health → Debug/Restore →
//                Formats → Directory Initialization

// ────────────────────────────────────────────────────────────────
// Identity — Package Self-Registration
// ────────────────────────────────────────────────────────────────

// init registers this package in the OmniCode identity registry.
func init() {
	identity.Register(_pragma["P1.key"], _pragma, _metadata)
}

// Pragma returns this package's OmniCode pragma identity (P1-P5).
func Pragma() map[string]string { return _pragma }

// Metadata returns this package's OmniCode metadata (M1-M10).
func Metadata() map[string]string { return _metadata }

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

// home returns the cached home directory. Resolved once via sync.Once.
// Falls back to empty string if os.UserHomeDir() fails.
func home() string {
	homeOnce.Do(func() {
		homeDir, _ = os.UserHomeDir()
	})
	return homeDir
}

// dirExists returns true if the path exists and is a directory.
func dirExists(path string) bool {
	info, err := os.Stat(path)
	return err == nil && info.IsDir()
}

// ────────────────────────────────────────────────────────────────
// Config Directory Resolution
// ────────────────────────────────────────────────────────────────

// FindConfigDir locates a "config" directory relative to the binary or cwd.
// Standard pattern for CPI-SI skill config loading.
func FindConfigDir() (string, error) {
	return FindConfigDirNamed("config")
}

// FindConfigDirNamed locates a named config directory by searching:
//  1. Relative to executable (bin/ → ../../<name>)
//  2. Relative to cwd (../<name>, then ./<name>)
//
// Returns os.ErrNotExist if not found.
func FindConfigDirNamed(name string) (string, error) {
	// Try relative to executable
	exe, err := os.Executable()
	if err == nil {
		exeDir := filepath.Dir(exe)
		// From bin/ go up to scripts/, then up to skill root, then into config/
		configPath := filepath.Join(exeDir, "..", "..", name)
		if dirExists(configPath) {
			return configPath, nil
		}
	}

	// Try relative to cwd (for development)
	cwd, err := os.Getwd()
	if err == nil {
		// Try ../config from scripts/
		configPath := filepath.Join(cwd, "..", name)
		if dirExists(configPath) {
			return configPath, nil
		}
		// Try ./config if in skill root
		configPath = filepath.Join(cwd, name)
		if dirExists(configPath) {
			return configPath, nil
		}
	}

	return "", os.ErrNotExist
}

// ────────────────────────────────────────────────────────────────
// Bereshit Paths
// ────────────────────────────────────────────────────────────────

// BereshitRoot returns the Bereshit project root directory.
// Resolution: BERESHIT_ROOT env → /media/.../Project/Bereshit → "bereshit" fallback.
func BereshitRoot() string {
	// Environment variable override
	if root := os.Getenv("BERESHIT_ROOT"); root != "" {
		return root
	}

	// Check known locations
	locations := []string{
		"/media/seanje-lenox-wise/Project/Bereshit",
		filepath.Join(home(), "Project", "Bereshit"),
		"bereshit",
	}

	for _, loc := range locations {
		if dirExists(loc) {
			return loc
		}
	}

	// Fallback
	return "bereshit"
}

// BereshitWord returns the bereshit/word/ path (definitions and implementations).
func BereshitWord() string {
	return filepath.Join(BereshitRoot(), "word")
}

// BereshitSeed returns the bereshit/word/seed/ path (templates).
func BereshitSeed() string {
	return filepath.Join(BereshitWord(), "seed")
}

// BereshitOmniSeed returns the bereshit/word/omni/seed/ path (OmniCode templates).
func BereshitOmniSeed() string {
	return filepath.Join(BereshitWord(), "omni", "seed")
}

// ────────────────────────────────────────────────────────────────
// Claude Paths
// ────────────────────────────────────────────────────────────────

// ClaudeHome returns the ~/.claude directory path.
func ClaudeHome() string {
	return filepath.Join(home(), ".claude")
}

// ClaudeSession returns the ~/.claude/session directory path.
func ClaudeSession() string {
	return filepath.Join(ClaudeHome(), "session")
}

// ClaudeSystem returns the ~/.claude/system directory path.
func ClaudeSystem() string {
	return filepath.Join(ClaudeHome(), "system")
}

// ClaudeSkills returns the ~/.claude/cpi-si/skills directory path.
func ClaudeSkills() string {
	return filepath.Join(ClaudeHome(), "cpi-si", "skills")
}

// ────────────────────────────────────────────────────────────────
// Claude Global Paths
// ────────────────────────────────────────────────────────────────

// ClaudeGlobalRoot returns the claude-global directory path.
// Resolution: CLAUDE_GLOBAL_ROOT env → bereshit/word/claude (production) →
// ~/.claude/global → claude-global (dev) → ~/.claude/global (fallback).
//
// Architecture: DEV (claude-global/) → PROD (bereshit/word/claude/) → OPS (~/.claude/)
func ClaudeGlobalRoot() string {
	// Environment variable override
	if root := os.Getenv("CLAUDE_GLOBAL_ROOT"); root != "" {
		return root
	}

	// Priority 1: Production location (bereshit/word/claude)
	prodLocations := []string{
		filepath.Join(BereshitRoot(), "word", "claude"),
		filepath.Join(ClaudeHome(), "global"),
	}

	for _, loc := range prodLocations {
		if dirExists(loc) {
			return loc
		}
	}

	// Priority 2: Dev location (for development builds only)
	devLocations := []string{
		"claude-global",
	}

	for _, loc := range devLocations {
		if dirExists(loc) {
			return loc
		}
	}

	// Final fallback
	return filepath.Join(ClaudeHome(), "global")
}

// ClaudeGlobalConfig returns the config directory within claude-global.
func ClaudeGlobalConfig() string {
	return filepath.Join(ClaudeGlobalRoot(), "config")
}

// ────────────────────────────────────────────────────────────────
// CPI-SI Schema Paths
// ────────────────────────────────────────────────────────────────

// CPISISchemaConfig returns the base CPI-SI schema config directory.
// This is the anchor for all CPI-SI configuration schemas.
func CPISISchemaConfig() string {
	return filepath.Join(ClaudeGlobalRoot(), "pkg", "cpisi", "schema", "config")
}

// ────────────────────────────────────────────────────────────────
// State Machine Runtime Paths
// ────────────────────────────────────────────────────────────────

// StateMachineSchemaDir returns the L2 vocabulary TOML directory.
// Source of truth for state machine vocabulary definitions.
func StateMachineSchemaDir() string {
	return filepath.Join(ClaudeGlobalRoot(), "pkg", "cpisi", "schema", "config", "l2")
}

// StateMachineRuntimeDir returns the runtime state directory.
// Contains session-specific state (state.jsonc, path.jsonc, history.jsonc).
func StateMachineRuntimeDir() string {
	return filepath.Join(ClaudeGlobalConfig(), "statemachine", "runtime")
}

// StateMachineRuntimeState returns the path to state.jsonc.
func StateMachineRuntimeState() string {
	return filepath.Join(StateMachineRuntimeDir(), "state.jsonc")
}

// StateMachineRuntimePath returns the path to path.jsonc.
func StateMachineRuntimePath() string {
	return filepath.Join(StateMachineRuntimeDir(), "path.jsonc")
}

// StateMachineRuntimeHistory returns the path to history.jsonc.
func StateMachineRuntimeHistory() string {
	return filepath.Join(StateMachineRuntimeDir(), "history.jsonc")
}

// StateMachineChoiceHistory returns the path to choice_history.jsonc.
func StateMachineChoiceHistory() string {
	return filepath.Join(StateMachineRuntimeDir(), "choice_history.jsonc")
}

// ────────────────────────────────────────────────────────────────
// Database Paths
// ────────────────────────────────────────────────────────────────

// DatabaseDir returns the data directory within claude-global.
func DatabaseDir() string {
	return filepath.Join(ClaudeGlobalRoot(), "data")
}

// DatabasePath returns the path to the main CPI-SI database.
// This is the temporal consciousness foundation.
func DatabasePath() string {
	return filepath.Join(DatabaseDir(), "cpisi.db")
}

// DatabaseSchemaDir returns the database schema directory.
func DatabaseSchemaDir() string {
	return filepath.Join(DatabaseDir(), "schema")
}

// DatabaseMigrationsDir returns the database migrations directory.
func DatabaseMigrationsDir() string {
	return filepath.Join(DatabaseDir(), "migrations")
}

// ────────────────────────────────────────────────────────────────
// Logging Paths
// ────────────────────────────────────────────────────────────────
//
// CPI-SI logs live under claude-global/data/logs/, SEPARATE from
// ~/.claude/logs/ (Claude's auto-tracking). These are for version
// control, analysis, and portability.

// LogsDir returns the root CPI-SI logs directory.
func LogsDir() string {
	return filepath.Join(DatabaseDir(), "logs")
}

// LogsSessionDir returns the session logs directory.
func LogsSessionDir() string {
	return filepath.Join(LogsDir(), "session")
}

// LogsTrajectoryDir returns the trajectory logs directory.
func LogsTrajectoryDir() string {
	return filepath.Join(LogsDir(), "trajectory")
}

// LogsCognitionDir returns the cognition logs directory.
func LogsCognitionDir() string {
	return filepath.Join(LogsDir(), "cognition")
}

// LogsHealthDir returns the health logs directory.
func LogsHealthDir() string {
	return filepath.Join(LogsDir(), "health")
}

// LogsToolsDir returns the tools logs directory.
func LogsToolsDir() string {
	return filepath.Join(LogsDir(), "tools")
}

// LogsContextDir returns the context logs directory.
func LogsContextDir() string {
	return filepath.Join(LogsDir(), "context")
}

// ────────────────────────────────────────────────────────────────
// Health Config Paths
// ────────────────────────────────────────────────────────────────

// HealthConfigDir returns the health config directory.
func HealthConfigDir() string {
	return filepath.Join(ClaudeGlobalConfig(), "health")
}

// HealthRuntimeDir returns the health runtime directory.
func HealthRuntimeDir() string {
	return filepath.Join(HealthConfigDir(), "runtime")
}

// ────────────────────────────────────────────────────────────────
// Debug and Restore Paths
// ────────────────────────────────────────────────────────────────

// DebugDir returns the debug directory within claude-global/data/.
func DebugDir() string {
	return filepath.Join(DatabaseDir(), "debug")
}

// RestoreDir returns the restore directory within claude-global/data/.
func RestoreDir() string {
	return filepath.Join(DatabaseDir(), "restore")
}

// RestoreCheckpointsDir returns the checkpoints subdirectory of restore.
func RestoreCheckpointsDir() string {
	return filepath.Join(RestoreDir(), "checkpoints")
}

// ────────────────────────────────────────────────────────────────
// Format Specs Paths
// ────────────────────────────────────────────────────────────────

// FormatsDir returns the formats specification directory.
func FormatsDir() string {
	return filepath.Join(DatabaseDir(), "formats")
}

// ────────────────────────────────────────────────────────────────
// Directory Initialization
// ────────────────────────────────────────────────────────────────

// EnsureDatabaseDir creates the data directory if it doesn't exist.
func EnsureDatabaseDir() error {
	return os.MkdirAll(DatabaseDir(), 0755)
}

// EnsureLogsDir creates all log subdirectories if they don't exist.
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

// EnsureDebugDir creates the debug directory if it doesn't exist.
func EnsureDebugDir() error {
	return os.MkdirAll(DebugDir(), 0755)
}

// EnsureRestoreDir creates the restore directories if they don't exist.
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

// EnsureFormatsDir creates the formats directory if it doesn't exist.
func EnsureFormatsDir() error {
	return os.MkdirAll(FormatsDir(), 0755)
}

// EnsureAllDataDirs creates all data subdirectories in the correct order.
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

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// Centralized path resolution for the entire CPI-SI system. Every
// component that needs a filesystem path calls this package instead
// of constructing paths inline. Changes propagate from one place.
//
// Portability: Hardcoded paths target the CWS workstation (current
// development environment). Environment variables (BERESHIT_ROOT,
// CLAUDE_GLOBAL_ROOT) enable other machines, CI, and testing.
//
// Caching: Home directory resolved once via sync.Once. All other
// paths are computed (no caching) because they depend on env vars
// or os.Stat checks that may change between calls.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new path functions (follow existing naming convention)
//   - Add new Ensure* functions for directory creation
//   - Update hardcoded default paths when locations change
//   - Add new environment variable overrides
//
// Modify with Care:
//   - Existing function signatures — breaks callers
//   - ClaudeGlobalRoot resolution order — many packages depend on it
//   - BereshitRoot resolution order — affects all bereshit paths
//
// Never Modify:
//   - 4-block structure
//   - init() identity registration pattern
//   - sync.Once caching pattern for home()
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:  stdlib only (os, path/filepath, sync) + L0 identity
// Dependents:    all CPI-SI packages that access filesystem
// Parallel:      config/util (loading files found by paths)
//
// "Ponder the path of thy feet." — Proverbs 4:26
//
// ============================================================================
// END CLOSING
// ============================================================================
