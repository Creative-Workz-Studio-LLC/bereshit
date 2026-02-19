//omni:code --go -library
//omni:key B-L0-hybrid-paths
//omni:version b-03.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-paths
// Purpose: Centralized path resolution for all CPI-SI system components
// Biblical: Proverbs 4:26 — "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 3.0.0
// Created: 2025-12-09
// Updated: 2026-02-18
//
// Resolver pattern: all paths compose from three cached roots (home, bereshit,
// claude-global). Two constructors — NewResolver for production (env vars +
// stat checks), NewResolverWithRoots for testing (inject known roots, no I/O).
// Package-level functions delegate to a global Resolver created on first call.
//
// Path families: Bereshit, Claude, Claude Global, CPI-SI Schema, State Machine,
// Database, Logging, Health, Debug/Restore, Formats.
//
// Environment variables: BERESHIT_ROOT, CLAUDE_GLOBAL_ROOT.

// Package paths provides shared path resolution for the CPI-SI system.
//
// Every component that needs a filesystem path calls this package instead
// of constructing paths inline. Changes propagate from one place.
//
// Two access patterns:
//
//	Package-level:  paths.BereshitRoot()     — uses global resolver
//	Resolver-level: r.BereshitRoot()         — uses injected resolver (testing)
//
// See Pragma (I1-I4) and Metadata (C1-C7) for OmniCode identity.
package paths

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"fmt"
	"os"
	"path/filepath"
	"sync"
)

import (
	"creativeworkzstudio.com/bereshit/L0-universal/b-spiral/identity"
)

// Pragma — OmniCode file identity (I1-I4). Runtime-queryable via PragmaGet().
var Pragma = [][2]string{
	// I1: Core — key, format, from, at
	{"I1.key", "B-L0-hybrid-paths"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.go"},
	{"I1.at", "b-03.00"},
	// I2: Family — type, structure, subtype, role
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "foundation"},
	// I3: Instance — file, title, component, path, provides, brief
	{"I3.file", "paths.go"},
	{"I3.title", "Path Resolution Library"},
	{"I3.component", "paths"},
	{"I3.path", "L0-universal/c-hybrid/core/paths/go/paths.go"},
	{"I3.provides", "Centralized path resolution for all CPI-SI system components"},
	{"I3.brief", "All filesystem paths resolve through this package"},
	// I4: Architecture — layer, position, pattern
	{"I4.layer", "L0-universal"},
	{"I4.position", "c-hybrid/core/paths"},
	{"I4.pattern", "centralized-resolution"},
}

// Metadata — OmniCode file metadata (C1-C7). Runtime-queryable via MetadataGet().
var Metadata = [][2]string{
	// C1: State — version, status, created, updated
	{"C1.version", "b-03.00"},
	{"C1.status", "Active"},
	{"C1.created", "2025-12-09"},
	{"C1.updated", "2026-02-18"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "2025-2026 CreativeWorkzStudio LLC"},
	// C3: Grounding
	{"C3.scripture", "Proverbs 4:26 — Ponder the path of thy feet"},
	{"C3.principle", "Know where you walk — every path resolved before traversed"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "fmt, os, path/filepath, sync"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "L0-universal/b-spiral/identity"},
	{"C4.consumers", "all CPI-SI packages that access filesystem"},
	{"C4.integration", "import creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/core/paths/go"},
	{"C4.if_missing", "callers must hardcode all filesystem paths"},
	// C5: Intent
	{"C5.purpose", "Single source of truth for all system paths"},
	{"C5.philosophy", "Centralize path logic so changes propagate from one place"},
	// C6: Roadmap
	{"C6.current", "b-03.00 — Resolver pattern, I1-I4/C1-C7 metadata, comprehensive tests"},
	{"C6.planned", "XDG compliance, path validation, configurable base paths"},
	{"C6.limitations", "CWS workstation paths as defaults, Linux only"},
	// C7: Classification
	{"C7.tags", "paths, resolution, config, bereshit, claude, database, logging"},
	{"C7.category", "Foundation"},
	{"C7.domain", "filesystem"},
	{"C7.paradigm", "CPI-SI"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// section-order: [s04]-variables, [s07]-types
// (imports declared in METADATA section — Go language constraint)

// ──────────────────────────────────────────────────────────────────────────
// s04 — Package Variables
// ──────────────────────────────────────────────────────────────────────────

var (
	resolverOnce sync.Once // guards global resolver initialization
	global       *Resolver // process-wide resolver, set once
)

// ──────────────────────────────────────────────────────────────────────────
// s07 — Types
// ──────────────────────────────────────────────────────────────────────────

// Resolver holds cached root directories for path resolution.
//
// Two constructors serve different needs:
//   - [NewResolver]: production — reads env vars, checks known locations
//   - [NewResolverWithRoots]: testing — injects known roots, no I/O
//
// All path composition methods build from three roots:
//   - home: user home directory (~)
//   - bereshit: Bereshit project root
//   - claudeGlobal: claude-global root (configs, data, schemas)
//
// Package-level functions delegate to a global Resolver created on first call.
type Resolver struct {
	home         string
	bereshit     string
	claudeGlobal string
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Types:
//   Resolver
//
// Constructors:
//   NewResolver, NewResolverWithRoots
//
// Identity:
//   PragmaGet, MetadataGet
//
// Resolver Methods (by domain):
//   Bereshit:      BereshitRoot, BereshitWord, BereshitSeed, BereshitOmniSeed
//   Claude:        ClaudeHome, ClaudeSession, ClaudeSystem, ClaudeSkills
//   Claude Global: ClaudeGlobalRoot, ClaudeGlobalConfig
//   CPI-SI:        CPISISchemaConfig
//   State Machine: StateMachineSchemaDir, StateMachineRuntimeDir,
//                  StateMachineRuntimeState, StateMachineRuntimePath,
//                  StateMachineRuntimeHistory, StateMachineChoiceHistory
//   Database:      DatabaseDir, DatabasePath, DatabaseSchemaDir,
//                  DatabaseMigrationsDir
//   Logging:       LogsDir, LogsSessionDir, LogsTrajectoryDir,
//                  LogsCognitionDir, LogsHealthDir, LogsToolsDir,
//                  LogsContextDir
//   Health:        HealthConfigDir, HealthRuntimeDir
//   Debug/Restore: DebugDir, RestoreDir, RestoreCheckpointsDir
//   Formats:       FormatsDir
//
// Display:
//   (*Resolver).String
//
// Free Functions:
//   FindConfigDir, FindConfigDirNamed
//
// Ensure (Resolver methods):
//   EnsureDatabaseDir, EnsureLogsDir, EnsureDebugDir, EnsureRestoreDir,
//   EnsureFormatsDir, EnsureAllDataDirs

// --- Identity Access ---

// init registers this package in the OmniCode identity registry.
func init() {
	key, ok := PragmaGet("I1.key")
	if !ok {
		return // identity not registerable without key
	}
	identity.Register(key, Pragma, Metadata)
}

// PragmaGet looks up a value from the Pragma identity.
// Returns the value and true if found, empty string and false otherwise.
func PragmaGet(key string) (string, bool) {
	for _, pair := range Pragma {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// MetadataGet looks up a value from the Metadata.
// Returns the value and true if found, empty string and false otherwise.
func MetadataGet(key string) (string, bool) {
	for _, pair := range Metadata {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────

// dirExists returns true if the path exists and is a directory.
func dirExists(path string) bool {
	info, err := os.Stat(path)
	return err == nil && info.IsDir()
}

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────

// NewResolver creates a Resolver from the environment.
//
// Resolution:
//   - home: os.UserHomeDir() (fallback: empty string)
//   - bereshit: BERESHIT_ROOT env → known locations → "bereshit"
//   - claudeGlobal: CLAUDE_GLOBAL_ROOT env → bereshit/word/claude →
//     ~/.claude/global → "claude-global" → fallback
func NewResolver() *Resolver {
	h, _ := os.UserHomeDir()
	b := resolveBereshit(h)
	cg := resolveClaudeGlobal(h, b)
	return &Resolver{
		home:         h,
		bereshit:     b,
		claudeGlobal: cg,
	}
}

// NewResolverWithRoots creates a Resolver with explicit root directories.
//
// Use for testing — bypasses all environment variable lookups and
// filesystem stat checks. Paths compose from exactly these roots.
//
//	r := paths.NewResolverWithRoots("/home/test", "/bereshit", "/claude")
//	r.BereshitWord() // → "/bereshit/word"
func NewResolverWithRoots(home, bereshit, claudeGlobal string) *Resolver {
	return &Resolver{
		home:         home,
		bereshit:     bereshit,
		claudeGlobal: claudeGlobal,
	}
}

// resolveBereshit finds the Bereshit project root.
//
// Priority: BERESHIT_ROOT env → known locations → "bereshit" fallback.
func resolveBereshit(home string) string {
	if root := os.Getenv("BERESHIT_ROOT"); root != "" {
		return root
	}

	locations := []string{
		"/media/seanje-lenox-wise/Project/Bereshit",
		filepath.Join(home, "Project", "Bereshit"),
	}

	for _, loc := range locations {
		if dirExists(loc) {
			return loc
		}
	}

	return "bereshit"
}

// resolveClaudeGlobal finds the claude-global root directory.
//
// Priority: CLAUDE_GLOBAL_ROOT env → bereshit/word/claude (production) →
// ~/.claude/global → "claude-global" (dev) → fallback.
func resolveClaudeGlobal(home, bereshit string) string {
	if root := os.Getenv("CLAUDE_GLOBAL_ROOT"); root != "" {
		return root
	}

	// Production: bereshit/word/claude
	prodPath := filepath.Join(bereshit, "word", "claude")
	if dirExists(prodPath) {
		return prodPath
	}

	// Operations: ~/.claude/global
	globalPath := filepath.Join(home, ".claude", "global")
	if dirExists(globalPath) {
		return globalPath
	}

	// Dev: claude-global in cwd
	if dirExists("claude-global") {
		return "claude-global"
	}

	// Fallback
	return filepath.Join(home, ".claude", "global")
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Paths package returns errors directly — no custom error types]

// --- Global Resolver ---

// globalResolver returns the process-wide Resolver, created on first access.
//
// Uses sync.Once — thread-safe, resolved exactly once, cached for process
// lifetime. All package-level functions delegate here.
func globalResolver() *Resolver {
	resolverOnce.Do(func() {
		global = NewResolver()
	})
	return global
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ──────────────────────────────────────────────────────────────────────────
//
// Organized by domain. Each domain has:
//   - Resolver methods (composable, testable)
//   - Package-level wrappers (backward-compatible, delegate to global)

// --- Bereshit Paths ---

// BereshitRoot returns the Bereshit project root directory.
func (r *Resolver) BereshitRoot() string { return r.bereshit }

// BereshitWord returns the bereshit/word/ path (definitions and implementations).
func (r *Resolver) BereshitWord() string {
	return filepath.Join(r.bereshit, "word")
}

// BereshitSeed returns the bereshit/word/seed/ path (templates).
func (r *Resolver) BereshitSeed() string {
	return filepath.Join(r.BereshitWord(), "seed")
}

// BereshitOmniSeed returns the bereshit/word/omni/seed/ path (OmniCode templates).
func (r *Resolver) BereshitOmniSeed() string {
	return filepath.Join(r.BereshitWord(), "omni", "seed")
}

// --- Claude Paths ---

// ClaudeHome returns the ~/.claude directory path.
func (r *Resolver) ClaudeHome() string {
	return filepath.Join(r.home, ".claude")
}

// ClaudeSession returns the ~/.claude/session directory path.
func (r *Resolver) ClaudeSession() string {
	return filepath.Join(r.ClaudeHome(), "session")
}

// ClaudeSystem returns the ~/.claude/system directory path.
func (r *Resolver) ClaudeSystem() string {
	return filepath.Join(r.ClaudeHome(), "system")
}

// ClaudeSkills returns the ~/.claude/cpi-si/skills directory path.
func (r *Resolver) ClaudeSkills() string {
	return filepath.Join(r.ClaudeHome(), "cpi-si", "skills")
}

// --- Claude Global Paths ---

// ClaudeGlobalRoot returns the claude-global directory path.
func (r *Resolver) ClaudeGlobalRoot() string { return r.claudeGlobal }

// ClaudeGlobalConfig returns the config directory within claude-global.
func (r *Resolver) ClaudeGlobalConfig() string {
	return filepath.Join(r.claudeGlobal, "config")
}

// --- CPI-SI Schema Paths ---

// CPISISchemaConfig returns the base CPI-SI schema config directory.
func (r *Resolver) CPISISchemaConfig() string {
	return filepath.Join(r.claudeGlobal, "pkg", "cpisi", "schema", "config")
}

// --- State Machine Paths ---

// StateMachineSchemaDir returns the L2 vocabulary TOML directory.
func (r *Resolver) StateMachineSchemaDir() string {
	return filepath.Join(r.CPISISchemaConfig(), "l2")
}

// StateMachineRuntimeDir returns the runtime state directory.
func (r *Resolver) StateMachineRuntimeDir() string {
	return filepath.Join(r.ClaudeGlobalConfig(), "statemachine", "runtime")
}

// StateMachineRuntimeState returns the path to state.jsonc.
func (r *Resolver) StateMachineRuntimeState() string {
	return filepath.Join(r.StateMachineRuntimeDir(), "state.jsonc")
}

// StateMachineRuntimePath returns the path to path.jsonc.
func (r *Resolver) StateMachineRuntimePath() string {
	return filepath.Join(r.StateMachineRuntimeDir(), "path.jsonc")
}

// StateMachineRuntimeHistory returns the path to history.jsonc.
func (r *Resolver) StateMachineRuntimeHistory() string {
	return filepath.Join(r.StateMachineRuntimeDir(), "history.jsonc")
}

// StateMachineChoiceHistory returns the path to choice_history.jsonc.
func (r *Resolver) StateMachineChoiceHistory() string {
	return filepath.Join(r.StateMachineRuntimeDir(), "choice_history.jsonc")
}

// --- Database Paths ---

// DatabaseDir returns the data directory within claude-global.
func (r *Resolver) DatabaseDir() string {
	return filepath.Join(r.claudeGlobal, "data")
}

// DatabasePath returns the path to the main CPI-SI database.
func (r *Resolver) DatabasePath() string {
	return filepath.Join(r.DatabaseDir(), "cpisi.db")
}

// DatabaseSchemaDir returns the database schema directory.
func (r *Resolver) DatabaseSchemaDir() string {
	return filepath.Join(r.DatabaseDir(), "schema")
}

// DatabaseMigrationsDir returns the database migrations directory.
func (r *Resolver) DatabaseMigrationsDir() string {
	return filepath.Join(r.DatabaseDir(), "migrations")
}

// --- Logging Paths ---

// LogsDir returns the root CPI-SI logs directory.
func (r *Resolver) LogsDir() string {
	return filepath.Join(r.DatabaseDir(), "logs")
}

// LogsSessionDir returns the session logs directory.
func (r *Resolver) LogsSessionDir() string {
	return filepath.Join(r.LogsDir(), "session")
}

// LogsTrajectoryDir returns the trajectory logs directory.
func (r *Resolver) LogsTrajectoryDir() string {
	return filepath.Join(r.LogsDir(), "trajectory")
}

// LogsCognitionDir returns the cognition logs directory.
func (r *Resolver) LogsCognitionDir() string {
	return filepath.Join(r.LogsDir(), "cognition")
}

// LogsHealthDir returns the health logs directory.
func (r *Resolver) LogsHealthDir() string {
	return filepath.Join(r.LogsDir(), "health")
}

// LogsToolsDir returns the tools logs directory.
func (r *Resolver) LogsToolsDir() string {
	return filepath.Join(r.LogsDir(), "tools")
}

// LogsContextDir returns the context logs directory.
func (r *Resolver) LogsContextDir() string {
	return filepath.Join(r.LogsDir(), "context")
}

// --- Health Config Paths ---

// HealthConfigDir returns the health config directory.
func (r *Resolver) HealthConfigDir() string {
	return filepath.Join(r.ClaudeGlobalConfig(), "health")
}

// HealthRuntimeDir returns the health runtime directory.
func (r *Resolver) HealthRuntimeDir() string {
	return filepath.Join(r.HealthConfigDir(), "runtime")
}

// --- Debug, Restore, and Formats Paths ---

// DebugDir returns the debug directory within claude-global/data/.
func (r *Resolver) DebugDir() string {
	return filepath.Join(r.DatabaseDir(), "debug")
}

// RestoreDir returns the restore directory within claude-global/data/.
func (r *Resolver) RestoreDir() string {
	return filepath.Join(r.DatabaseDir(), "restore")
}

// RestoreCheckpointsDir returns the checkpoints subdirectory of restore.
func (r *Resolver) RestoreCheckpointsDir() string {
	return filepath.Join(r.RestoreDir(), "checkpoints")
}

// FormatsDir returns the formats specification directory.
func (r *Resolver) FormatsDir() string {
	return filepath.Join(r.DatabaseDir(), "formats")
}

// --- Package-Level Wrappers ---
//
// These functions delegate to the global Resolver (created on first call).
// They preserve backward compatibility — callers continue using:
//   paths.BereshitRoot() instead of resolver.BereshitRoot()

// BereshitRoot returns the Bereshit project root directory.
func BereshitRoot() string { return globalResolver().BereshitRoot() }

// BereshitWord returns the bereshit/word/ path.
func BereshitWord() string { return globalResolver().BereshitWord() }

// BereshitSeed returns the bereshit/word/seed/ path.
func BereshitSeed() string { return globalResolver().BereshitSeed() }

// BereshitOmniSeed returns the bereshit/word/omni/seed/ path.
func BereshitOmniSeed() string { return globalResolver().BereshitOmniSeed() }

// ClaudeHome returns the ~/.claude directory path.
func ClaudeHome() string { return globalResolver().ClaudeHome() }

// ClaudeSession returns the ~/.claude/session directory path.
func ClaudeSession() string { return globalResolver().ClaudeSession() }

// ClaudeSystem returns the ~/.claude/system directory path.
func ClaudeSystem() string { return globalResolver().ClaudeSystem() }

// ClaudeSkills returns the ~/.claude/cpi-si/skills directory path.
func ClaudeSkills() string { return globalResolver().ClaudeSkills() }

// ClaudeGlobalRoot returns the claude-global directory path.
func ClaudeGlobalRoot() string { return globalResolver().ClaudeGlobalRoot() }

// ClaudeGlobalConfig returns the config directory within claude-global.
func ClaudeGlobalConfig() string { return globalResolver().ClaudeGlobalConfig() }

// CPISISchemaConfig returns the base CPI-SI schema config directory.
func CPISISchemaConfig() string { return globalResolver().CPISISchemaConfig() }

// StateMachineSchemaDir returns the L2 vocabulary TOML directory.
func StateMachineSchemaDir() string { return globalResolver().StateMachineSchemaDir() }

// StateMachineRuntimeDir returns the runtime state directory.
func StateMachineRuntimeDir() string { return globalResolver().StateMachineRuntimeDir() }

// StateMachineRuntimeState returns the path to state.jsonc.
func StateMachineRuntimeState() string { return globalResolver().StateMachineRuntimeState() }

// StateMachineRuntimePath returns the path to path.jsonc.
func StateMachineRuntimePath() string { return globalResolver().StateMachineRuntimePath() }

// StateMachineRuntimeHistory returns the path to history.jsonc.
func StateMachineRuntimeHistory() string { return globalResolver().StateMachineRuntimeHistory() }

// StateMachineChoiceHistory returns the path to choice_history.jsonc.
func StateMachineChoiceHistory() string { return globalResolver().StateMachineChoiceHistory() }

// DatabaseDir returns the data directory within claude-global.
func DatabaseDir() string { return globalResolver().DatabaseDir() }

// DatabasePath returns the path to the main CPI-SI database.
func DatabasePath() string { return globalResolver().DatabasePath() }

// DatabaseSchemaDir returns the database schema directory.
func DatabaseSchemaDir() string { return globalResolver().DatabaseSchemaDir() }

// DatabaseMigrationsDir returns the database migrations directory.
func DatabaseMigrationsDir() string { return globalResolver().DatabaseMigrationsDir() }

// LogsDir returns the root CPI-SI logs directory.
func LogsDir() string { return globalResolver().LogsDir() }

// LogsSessionDir returns the session logs directory.
func LogsSessionDir() string { return globalResolver().LogsSessionDir() }

// LogsTrajectoryDir returns the trajectory logs directory.
func LogsTrajectoryDir() string { return globalResolver().LogsTrajectoryDir() }

// LogsCognitionDir returns the cognition logs directory.
func LogsCognitionDir() string { return globalResolver().LogsCognitionDir() }

// LogsHealthDir returns the health logs directory.
func LogsHealthDir() string { return globalResolver().LogsHealthDir() }

// LogsToolsDir returns the tools logs directory.
func LogsToolsDir() string { return globalResolver().LogsToolsDir() }

// LogsContextDir returns the context logs directory.
func LogsContextDir() string { return globalResolver().LogsContextDir() }

// HealthConfigDir returns the health config directory.
func HealthConfigDir() string { return globalResolver().HealthConfigDir() }

// HealthRuntimeDir returns the health runtime directory.
func HealthRuntimeDir() string { return globalResolver().HealthRuntimeDir() }

// DebugDir returns the debug directory.
func DebugDir() string { return globalResolver().DebugDir() }

// RestoreDir returns the restore directory.
func RestoreDir() string { return globalResolver().RestoreDir() }

// RestoreCheckpointsDir returns the checkpoints subdirectory of restore.
func RestoreCheckpointsDir() string { return globalResolver().RestoreCheckpointsDir() }

// FormatsDir returns the formats specification directory.
func FormatsDir() string { return globalResolver().FormatsDir() }

// ──────────────────────────────────────────────────────────────────────────
// 6. Display
// ──────────────────────────────────────────────────────────────────────────

// String returns a human-readable representation of the Resolver.
//
//	Resolver{home="/home/user", bereshit="/path/to/bereshit", claude_global="/path/to/claude"}
func (r *Resolver) String() string {
	return fmt.Sprintf("Resolver{home=%q, bereshit=%q, claude_global=%q}",
		r.home, r.bereshit, r.claudeGlobal)
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────

// --- Config Directory Resolution ---

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
		configPath := filepath.Join(exeDir, "..", "..", name)
		if dirExists(configPath) {
			return configPath, nil
		}
	}

	// Try relative to cwd
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

// --- Directory Initialization (Resolver methods) ---

// EnsureDatabaseDir creates the data directory if it doesn't exist.
func (r *Resolver) EnsureDatabaseDir() error {
	return os.MkdirAll(r.DatabaseDir(), 0o755)
}

// EnsureLogsDir creates all log subdirectories if they don't exist.
func (r *Resolver) EnsureLogsDir() error {
	dirs := []string{
		r.LogsDir(),
		r.LogsSessionDir(),
		r.LogsTrajectoryDir(),
		r.LogsCognitionDir(),
		r.LogsHealthDir(),
		r.LogsToolsDir(),
		r.LogsContextDir(),
	}
	for _, dir := range dirs {
		if err := os.MkdirAll(dir, 0o755); err != nil {
			return err
		}
	}
	return nil
}

// EnsureDebugDir creates the debug directory if it doesn't exist.
func (r *Resolver) EnsureDebugDir() error {
	return os.MkdirAll(r.DebugDir(), 0o755)
}

// EnsureRestoreDir creates the restore directories if they don't exist.
func (r *Resolver) EnsureRestoreDir() error {
	dirs := []string{
		r.RestoreDir(),
		r.RestoreCheckpointsDir(),
	}
	for _, dir := range dirs {
		if err := os.MkdirAll(dir, 0o755); err != nil {
			return err
		}
	}
	return nil
}

// EnsureFormatsDir creates the formats directory if it doesn't exist.
func (r *Resolver) EnsureFormatsDir() error {
	return os.MkdirAll(r.FormatsDir(), 0o755)
}

// EnsureAllDataDirs creates all data subdirectories in the correct order.
func (r *Resolver) EnsureAllDataDirs() error {
	for _, fn := range []func() error{
		r.EnsureDatabaseDir,
		r.EnsureLogsDir,
		r.EnsureDebugDir,
		r.EnsureRestoreDir,
		r.EnsureFormatsDir,
	} {
		if err := fn(); err != nil {
			return err
		}
	}
	return nil
}

// --- Directory Initialization (Package-level wrappers) ---

// EnsureDatabaseDir creates the data directory if it doesn't exist.
func EnsureDatabaseDir() error { return globalResolver().EnsureDatabaseDir() }

// EnsureLogsDir creates all log subdirectories if they don't exist.
func EnsureLogsDir() error { return globalResolver().EnsureLogsDir() }

// EnsureDebugDir creates the debug directory if it doesn't exist.
func EnsureDebugDir() error { return globalResolver().EnsureDebugDir() }

// EnsureRestoreDir creates the restore directories if they don't exist.
func EnsureRestoreDir() error { return globalResolver().EnsureRestoreDir() }

// EnsureFormatsDir creates the formats directory if it doesn't exist.
func EnsureFormatsDir() error { return globalResolver().EnsureFormatsDir() }

// EnsureAllDataDirs creates all data subdirectories in the correct order.
func EnsureAllDataDirs() error { return globalResolver().EnsureAllDataDirs() }

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// Testing: go test -v ./L0-universal/c-hybrid/core/paths/go/
//
// Verify:
//   - NewResolverWithRoots stores exact paths (no resolution)
//   - All path composition methods produce correct segments
//   - Ensure* methods create real directories in temp dir
//   - EnsureAllDataDirs creates complete directory tree
//   - Package-level functions delegate to global resolver
//   - PragmaGet/MetadataGet return correct values for known keys
//   - PragmaGet/MetadataGet return false for missing keys
//   - FindConfigDirNamed returns os.ErrNotExist when not found
//
// See paths_test.go for full test suite.
//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — imported, not executed.
// import "creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/core/paths/go"
//
// Identity registered via init() on import.
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// sync.Once persists for process lifetime. No cleanup needed.
// Resolver is a value type — garbage collected normally.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Change exported function signatures (callers depend on them)
//   - Remove env var override support (BERESHIT_ROOT, CLAUDE_GLOBAL_ROOT)
//   - Add non-stdlib external dependencies (lightweight foundation)
//
// Careful:
//   - Changing resolution order in resolveBereshit or resolveClaudeGlobal
//   - Modifying hardcoded fallback paths (workstation-specific)
//   - Adding new Ensure* functions (each creates real directories)
//
// Safe:
//   - Adding new path composition methods (pure Join chains)
//   - Adding new Resolver query methods
//   - Improving doc comments
//
// ──────────────────────────────────────────────────────────────────────────
// X2: Extension Points
// ──────────────────────────────────────────────────────────────────────────
//
// Completed: Resolver pattern, I1-I4/C1-C7 metadata, comprehensive tests
// Designed:  XDG compliance, path validation, canonicalization
// Future:    Windows support, configurable base paths
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Centralized path resolution — the system knowing where it walks.
// Resolver pattern enables full testability without env var mutation.
// Cross-language parity with Rust bereshit-l0-paths crate.
//
// Scripture: Proverbs 4:26 — "Ponder the path of thy feet, and let all
// thy ways be established."
//
// Genesis 1:1 — "In the beginning God created the heaven and the earth."
//
// ============================================================================
// END CLOSING
// ============================================================================
