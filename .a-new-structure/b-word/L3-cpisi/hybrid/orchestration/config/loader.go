//omni:code --go -library
//omni:key B-L3-config-loader
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Claude Global Config Loader
// See: claude-global/config.toml for configuration schema

// Package config loads claude-global configuration from config.toml.
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-config-loader
//
//	Scripture: "In all thy ways acknowledge him, and he shall direct thy paths"
//	           — Proverbs 3:6
//	Principle: Configuration acknowledges structure; paths emerge from order
//	Anchor:    "Precept upon precept, line upon line" — Isaiah 28:10
//
//	Type:      Ladder (foundation for all hooks, skills, statusline)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-15
//	Version:         a-01.00
//
// # M.2 Version History [HISTORY]
//
//   - a-01.00 (2025-12-15) — Initial creation from Bereshit loader pattern
//
// # M.3 Interface [INTERFACE]
//
//	Requires: stdlib (1): fmt, os, path/filepath | external (-1): github.com/BurntSushi/toml
//	Used by:  hooks, skills, statusline — all via pkg import
//	Import:   import "creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/orchestration/config"
//	Pattern:  SetRoot(path) → Load() → cfg.Paths.Agents
//
// # M.4 Public API [API]
//
// M.4.1 Configuration [CONFIG]
//
//	SetRoot(path)                      Set claude-global root directory
//	Root() string                      Get current root
//
// M.4.2 Loading [LOAD]
//
//	Load() (*Config, error)            Load config.toml (primary)
//	MustLoad() *Config                 Load or panic (for init)
//
// M.4.3 Paths [ACCESS]
//
//	Anchors() AnchorPaths              Claude Code anchor directories
//	Extensions() ExtensionPaths        CPI-SI extension directories
//	Session() SessionConfig            Session management config
//	Bereshit() BereshitConfig          Optional bereshit integration
//
// M.4.4 Tripwire [CHECK]
//
//	If config.toml missing, falls back to hardcoded paths with LOUD warning.
//	This is degraded mode — config should exist.
//
// # M.5 Operational [OPERATIONAL]
//
//	Blocking: no (-1) — file I/O only, no network or long-running operations
//	Health:   Provider | granted (1): loaded | deferred (0): loading | denied (-1): failed
package config

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Imports → Types → Type Methods → Constants → Variables → Package-Level State
// See: bereshit/word/seed/code/go/library.go > SETUP

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- I.1 Standard Library [IMPORT] (1) ---
// Error handling, filesystem, path operations
import (
	"fmt"           // error formatting
	"path/filepath" // path construction
	"sort"          // consistent key ordering in extractKeys()
)

//--- I.2 External Packages [IMPORT] (-1) ---
// TOML parsing (Go stdlib lacks support)
import (
	"github.com/BurntSushi/toml" // TOML decoding
)

//--- I.3 Internal Packages [IMPORT] (0) ---
import (
	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util" // FileExists, LoadError, Must, RootGuard, TripwireBanner
)

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// Subsections: Config Types, Path Types, System Types, Error Types

//--- T.1 Config Types [TYPE] (1) ---
// Root configuration matching config.toml structure

// Config represents the complete claude-global configuration.
// Loaded from config.toml at root of claude-global.
//
// Example:
//
//	cfg, _ := config.Load()
//	cfg.Anchors().Agents  // path to agents/
//	cfg.Session().IdleThreshold
type Config struct {
	Meta    MetaConfig    `toml:"_meta"`
	Systems []SystemEntry `toml:"systems"`
	// Cached typed accessors (built on first access)
	anchors  *AnchorPaths
	exts     *ExtensionPaths
	session  *SessionConfig
	bereshit *BereshitConfig
	logging  *LoggingConfig
}

// MetaConfig holds config metadata.
type MetaConfig struct {
	Version     string `toml:"version"`
	Schema      string `toml:"schema"`
	Description string `toml:"description"`
}

//--- T.2 Path Types [TYPE] (1) ---
// Typed accessors for path configuration

// AnchorPaths holds Claude Code anchor directories (cannot rename).
type AnchorPaths struct {
	Agents       string
	Commands     string
	Hooks        string
	OutputStyles string
	Rules        string
	Skills       string
}

// ExtensionPaths holds CPI-SI extension directories.
type ExtensionPaths struct {
	Bin        string
	Config     string
	Docs       string
	Pkg        string
	Statusline string
	System     string
}

//--- T.3 System Types [TYPE] (1) ---
// Match [[systems]] structure in config.toml

// SystemEntry represents a system in config.toml.
type SystemEntry struct {
	Name        string      `toml:"name"`
	Path        string      `toml:"path"`
	Order       int         `toml:"order"`
	Description string      `toml:"description"`
	DependsOn   []string    `toml:"depends_on"`
	Paths       []PathEntry `toml:"paths"`
}

// PathEntry represents a path within a system.
type PathEntry struct {
	Name        string `toml:"name"`
	Path        string `toml:"path"`
	Description string `toml:"description"`
}

// SessionConfig holds session management configuration.
type SessionConfig struct {
	SessionDir     string
	CurrentSession string
	SessionLog     string
	PatternFile    string
	QuickCheckMax  int
	NormalWorkMax  int
	DeepWorkMax    int
	IdleThreshold  int
}

// BereshitConfig holds optional bereshit integration.
type BereshitConfig struct {
	Enabled    bool
	Root       string
	AutoDetect []string
	Paths      map[string]string
}

// LoggingConfig holds logging configuration.
type LoggingConfig struct {
	Level         string
	Stdout        bool
	File          bool
	FilePath      string
	IncludeHealth bool
}

//--- T.4 Error Types [TYPE] (1) ---
// Config-driven errors: originate in DATA, manifest in CODE
// THROUGHLINE: LoadError delegated to L0 util. ConfigError stays (domain-specific).

// LoadError wraps file-level loading failures.
// Delegated to util.LoadError — single source of truth.
type LoadError = util.LoadError

// ConfigError represents a configuration problem (domain-specific to claude-global).
// Format: "config [system.anchors]: missing required path 'agents'"
type ConfigError struct {
	System  string // e.g., "anchors", "extensions"
	Field   string // field name, e.g., "agents"
	Message string // what's wrong
}

func (e *ConfigError) Error() string {
	if e.Field != "" {
		return fmt.Sprintf("config [system.%s.%s]: %s", e.System, e.Field, e.Message)
	}
	if e.System != "" {
		return fmt.Sprintf("config [system.%s]: %s", e.System, e.Message)
	}
	return fmt.Sprintf("config: %s", e.Message)
}

// ────────────────────────────────────────────────────────────────
// Type Methods
// ────────────────────────────────────────────────────────────────

//--- TM Type Methods [METHOD] (0) ---
// [Reserved: Phase 3 — typed accessors, conversion methods]
// LoadError implements error interface (Error(), Unwrap())
// ConfigError implements error interface (Error())

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────
//
// Filesystem paths for navigating the 5-system architecture.
// Primary source is config.toml; paths here are fallback for direct access.
//
// 5-System Architecture (dependency order, 0=anchor):
//   0. anchors      — Claude Code fixed directories (cannot rename)
//   1. extensions   — CPI-SI directories (pkg, bin, statusline, etc.)
//   2. session      — Session management config
//   3. bereshit     — Optional Bereshit integration
//   4. logging      — Log configuration

//--- K.1 Config File [DATA] (1) ---
// Primary configuration file. All paths flow from here.
const (
	ConfigFile = "config.toml" // relative to claude-global root
)

//--- K.2 System Paths [DATA] (1) ---
// Path constants for system directories and files.
// Config-driven: config.toml provides paths via SystemEntry.Path
// Tripwire: Helpers > Fallback Data > fallbackAnchorPaths, fallbackExtensionPaths

//--- K.3 Type Constants [DATA] (0) ---
// [Reserved: No type constants — uses config.toml string values]

//--- K.4 Validation Thresholds [DATA] (0) ---
// [Reserved: No thresholds — validation delegated to config.go]

// ────────────────────────────────────────────────────────────────
// Variables
// ────────────────────────────────────────────────────────────────
//
// Package-level mutable state. Kept minimal — explicit SetRoot() over implicit.
//
// Subsections: Configuration State, Sentinel Errors, Cached Config

//--- V.1 Configuration State [DATA] (1) ---
// Set once at startup, read by all Load* functions.
// THROUGHLINE: claudeGlobalRootGuard.Path() replaced by util.RootGuard
var (
	claudeGlobalRootGuard = util.NewRootGuard("claude-global") // replaces claudeGlobalRootGuard.Path() + errRootNotSet
	loadedConfig          *Config                              // cached config after first load
)

//--- V.2 Sentinel Errors [DATA] (1) ---
// THROUGHLINE: errRootNotSet now handled by claudeGlobalRootGuard.Check()
// Removed: var errRootNotSet = fmt.Errorf(...)

//--- V.3 Cached Config [DATA] (1) ---
// [Reserved: Lazy loading — first access triggers load, subsequent reads from cache]

// ────────────────────────────────────────────────────────────────
// Package-Level State
// ────────────────────────────────────────────────────────────────
//
// Cross-package coordination via SetRoot(). All claude-global packages
// call config.SetRoot() at startup to establish the root path.
//
// Subsections: Coordination Pattern, Initialization Order, Reserved Features

//--- PS.1 Coordination Pattern [DOC] (1) ---
// All claude-global packages call config.SetRoot() at startup.
// Root path flows to all Load*/Get* functions via package variable.

//--- PS.2 Initialization Order [DOC] (1) ---
// Sequence of operations for proper initialization.
//   1. SetRoot() called by main/hook with absolute path
//   2. Load() parses config.toml and caches result
//   3. Root()/Extensions()/Anchors() return cached paths

//--- PS.3 Reserved Features [DOC] (0) ---
// [Reserved: init() auto-discovery from ~/.claude or environment]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Org Chart → Helpers → Core Operations → Error Handling → Public APIs
// See: bereshit/word/seed/code/go/library.go > BODY

// ────────────────────────────────────────────────────────────────
// Organizational Chart
// ────────────────────────────────────────────────────────────────
//
// Navigation map for BODY. When lost in implementation, return here.
// Shows WHERE functions live (layers), HOW they connect (flow), WHAT exists (count).
//
// Subsections: Ladder Structure, Baton Flow, APU Inventory

//--- Ladder Structure ---
// Vertical dependency layers. Bottom supports top. Read bottom-up for foundations.
//
//   Public APIs (Top Rungs - Orchestration)
//   ├── Configuration:    SetRoot(), Root()
//   ├── Loading:          Load(), MustLoad()
//   ├── Config Access:    GetConfig(), Systems()
//   └── Path Accessors:   Anchors(), Extensions(), AgentsPath(), HooksPath(), PkgPath(), BinPath()
//
//   Error Handling (Error constructors)
//   └── newLoadError(), newConfigError()
//
//   Core Operations (Middle Rungs - Business Logic)
//   ├── Config Loading:    loadConfig()
//   └── Path Extraction:   extractAnchors(), extractExtensions(), populateConfig()
//
//   Helpers (Bottom Rungs - Foundations)
//   ├── Pure Functions:    extractSystemNames(), findSystem()
//   ├── Guard Functions:   checkRoot()
//   └── Fallback Data:     fallbackAnchorPaths, fallbackExtensionPaths

//--- Baton Flow ---
// Horizontal execution paths. Entry → processing → Exit. Follow for debugging.
//
//   Entry → SetRoot(path)
//     ↓
//   Load() ────────────────────────────────────────────────────┐
//     │                                                        │
//     ↓ (primary)                                              ↓ (tripwire fallback)
//   loadConfig()                                             fallback*Paths
//     │                                                        │
//     ├─ Step 1: Parse config.toml                            │
//     ├─ Step 2: Sort systems by order                       LOUD warning
//     └─ Step 3: populateConfig() → extract*()                 │
//     ↓                                                        ↓
//   Exit → *Config with typed paths ←─────────────────────────┘

//--- APU Inventory ---
// Count of Available Processing Units by category.
//
//   Total: 16 functions + 2 fallback structs
//   ├── Helpers: 3 functions + 2 structs
//   │   ├── Pure Functions:  extractSystemNames(), findSystem()
//   │   ├── Guard Functions: checkRoot()
//   │   └── Fallback Data:   fallbackAnchorPaths, fallbackExtensionPaths
//   ├── Core Operations: 4 functions
//   │   ├── loadConfig()
//   │   └── extractAnchors(), extractExtensions(), populateConfig()
//   ├── Error Handling: 2 functions
//   │   └── newLoadError(), newConfigError()
//   └── Public APIs: 10 functions
//       ├── SetRoot(), Root()
//       ├── Load(), MustLoad()
//       ├── GetConfig(), Systems()
//       └── Anchors(), Extensions(), AgentsPath(), HooksPath(), PkgPath(), BinPath()

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────
//
// Foundation layer. Higher rungs depend on these, never the reverse.
// See: Org Chart > Ladder Structure > Helpers
//
// Subsections: Pure Functions, Guard Functions, Domain Functions, Fallback Data

//--- H.1 Pure Functions [TRANSFORM] (1) ---
// No side effects. Same input → same output. Safe to call from anywhere.

// fileExists returns true if path exists on filesystem.
//
// Parameters:
//   - path: filesystem path to check
//
// Returns:
//   - bool: true if file exists, false otherwise
//
// Used by: Core Operations > Config Loading, config.go > Schema Loading, JSONC Loading
// fileExists delegates to util.FileExists (throughline consolidation).
func fileExists(path string) bool {
	return util.FileExists(path)
}

// extractSystemNames returns the names of all loaded systems.
//
// Parameters:
//   - cfg: loaded Config
//
// Returns:
//   - []string: system names in order (e.g., ["anchors", "extensions", ...])
//
// Used by: Public APIs > SystemNames
func extractSystemNames(cfg *Config) []string {
	names := make([]string, 0, len(cfg.Systems))
	for _, sys := range cfg.Systems {
		names = append(names, sys.Name)
	}
	return names // order preserved from cfg.Systems (slice, not map)
}

// findSystem locates a system by name in the config.
//
// Parameters:
//   - cfg: loaded Config
//   - name: system name to find (e.g., "anchors")
//
// Returns:
//   - *SystemEntry: pointer to system, nil if not found
//
// Used by: Core Operations > System Lookup (C.2)
func findSystem(cfg *Config, name string) *SystemEntry {
	for i := range cfg.Systems {
		if cfg.Systems[i].Name == name {
			return &cfg.Systems[i]
		}
	}
	return nil
}

//--- H.2 Guard Functions [CHECK] (1) ---
// Precondition checks. Call at function entry to fail fast.
// See: SETUP > Variables > Sentinel Errors (errors returned by guards)

// checkRoot validates claudeGlobalRootGuard.Path() has been set via SetRoot().
//
// Returns:
//   - error: errRootNotSet if claudeGlobalRootGuard.Path() is empty, nil otherwise
//
// Used by: Public APIs > LoadConfig
// checkRoot delegates to claudeGlobalRootGuard.Check() (throughline consolidation).
func checkRoot() error {
	return claudeGlobalRootGuard.Check()
}

//--- H.3 Domain Functions [DOMAIN] (0) ---
// Reserved: loader.go focuses on loading, not domain transformation.
// See: config.go > Helpers > Domain Functions

//--- H.4 Fallback Data [DATA] (1) ---
// Static data for tripwire fallback when config.toml unavailable.
// Tripwire: K.2 references this section. If K.2 Tripwire breaks, check here.

// fallbackAnchorPaths provides hardcoded Claude Code anchor directories.
// Used ONLY when config.toml is missing (tripwire fallback).
var fallbackAnchorPaths = &AnchorPaths{
	Agents:       "agents",
	Commands:     "commands",
	Hooks:        "hooks",
	OutputStyles: "output-styles",
	Rules:        "rules",
	Skills:       "skills",
}

// fallbackExtensionPaths provides hardcoded CPI-SI extension directories.
// Used ONLY when config.toml is missing (tripwire fallback).
var fallbackExtensionPaths = &ExtensionPaths{
	Bin:        "bin",
	Config:     "config",
	Docs:       "docs",
	Pkg:        "pkg",
	Statusline: "statusline",
	System:     "system",
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
//
// Middle rung. Depends on Helpers below, used by Public APIs above.
// See: Org Chart > Ladder Structure > Core Operations
//
// Subsections: Config Loading, Path Extraction

//--- C.1 Config Loading [LOAD] (1) ---
// Single config.toml loading. Entry point for all config access.
// Uses: Helpers > Pure Functions > fileExists, SETUP > Constants > ConfigFile

// loadConfig loads and parses the config.toml file.
//
// Returns:
//   - *Config: parsed config with all systems
//   - error: file not found or TOML parse errors
func loadConfig() (*Config, error) {
	configPath := filepath.Join(claudeGlobalRootGuard.Path(), ConfigFile) // ConfigFile from SETUP > Constants
	if !fileExists(configPath) {                              // Helpers > Pure Functions
		return nil, fmt.Errorf("config.toml not found: %s", configPath)
	}

	var cfg Config
	if _, err := toml.DecodeFile(configPath, &cfg); err != nil { // BurntSushi/toml
		return nil, fmt.Errorf("config.toml parse error: %w", err)
	}

	// Sort systems by order (0 = anchor, higher = depends on lower)
	sort.Slice(cfg.Systems, func(i, j int) bool {
		return cfg.Systems[i].Order < cfg.Systems[j].Order
	})

	return &cfg, nil
}

//--- C.2 Path Extraction [EXTRACT] (1) ---
// Extract typed paths from loaded config. Called after loadConfig().
// Uses: Helpers > Pure Functions > findSystem

// extractAnchors builds AnchorPaths from the "anchors" system.
//
// Parameters:
//   - cfg: loaded Config from loadConfig()
//
// Returns:
//   - *AnchorPaths: typed anchor paths, nil if system not found
func extractAnchors(cfg *Config) *AnchorPaths {
	sys := findSystem(cfg, "anchors") // Helpers > Pure Functions
	if sys == nil {
		return nil
	}

	anchors := &AnchorPaths{}
	for _, p := range sys.Paths {
		switch p.Name {
		case "agents":
			anchors.Agents = p.Path
		case "commands":
			anchors.Commands = p.Path
		case "hooks":
			anchors.Hooks = p.Path
		case "output-styles":
			anchors.OutputStyles = p.Path
		case "rules":
			anchors.Rules = p.Path
		case "skills":
			anchors.Skills = p.Path
		}
	}
	return anchors
}

// extractExtensions builds ExtensionPaths from the "extensions" system.
//
// Parameters:
//   - cfg: loaded Config from loadConfig()
//
// Returns:
//   - *ExtensionPaths: typed extension paths, nil if system not found
func extractExtensions(cfg *Config) *ExtensionPaths {
	sys := findSystem(cfg, "extensions") // Helpers > Pure Functions
	if sys == nil {
		return nil
	}

	exts := &ExtensionPaths{}
	for _, p := range sys.Paths {
		switch p.Name {
		case "bin":
			exts.Bin = p.Path
		case "config":
			exts.Config = p.Path
		case "docs":
			exts.Docs = p.Path
		case "pkg":
			exts.Pkg = p.Path
		case "statusline":
			exts.Statusline = p.Path
		case "system":
			exts.System = p.Path
		}
	}
	return exts
}

// populateConfig extracts all typed paths and caches them in the Config.
// Called once after loading, enables fast access via getter methods.
//
// Parameters:
//   - cfg: loaded Config to populate
func populateConfig(cfg *Config) {
	cfg.anchors = extractAnchors(cfg)
	cfg.exts = extractExtensions(cfg)
	// Session, Bereshit, Logging populated on demand (more complex structures)
}

// ────────────────────────────────────────────────────────────────
// Error Handling/Recovery Patterns
// ────────────────────────────────────────────────────────────────
//
// Config-driven errors: originate in DATA, manifest in CODE, trace back to source.
// See: SETUP > Types > Error Types for type definitions
//
// Subsections: Design Principles, Error Constructors, Recovery Patterns

//--- Design Principles ---
// Config-driven systems separate DATA from CODE.
//
// Error Categories:
//   - LoadError:   file-level (not found, parse failed)
//   - ConfigError: config-level (missing system, invalid path)
//
// Recovery Strategy:
//   - Tripwire fallback: if config.toml missing, use hardcoded paths with LOUD warning
//   - Context-rich: every error traces to system → field
//   - Actionable: message tells you what to fix in the data

//--- Error Constructors ---
// Factory functions for error types. Used by Core Operations and Public APIs.

// newLoadError creates a LoadError with file and operation context.
//
// Parameters:
//   - file: config file path (e.g., "config.toml")
//   - op: operation that failed ("read", "parse", "validate")
//   - err: underlying error
//
// Returns:
//   - *LoadError: wrapped error with context
func newLoadError(file, op string, err error) *LoadError {
	return &LoadError{File: file, Op: op, Err: err}
}

// newConfigError creates a ConfigError with system context.
//
// Parameters:
//   - system: system name (e.g., "anchors", "extensions")
//   - field: field name (e.g., "agents"), empty if system-level
//   - message: what's wrong
//
// Returns:
//   - *ConfigError: error with system context
func newConfigError(system, field, message string) *ConfigError {
	return &ConfigError{System: system, Field: field, Message: message}
}

//--- Recovery Patterns ---
// Tripwire fallback: when config.toml missing, use fallback*Paths with warning.
// See: Public APIs > Load() for implementation.

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
//
// Top rung. Orchestrates Core Operations for external callers.
// See: Org Chart > Ladder Structure > Public APIs
//
// Subsections: Configuration, Loading, Config Access, Path Accessors

//--- Configuration ---
// Package state initialization. Must be called before Load().

// SetRoot sets the claude-global root directory path.
//
// Parameters:
//   - path: absolute path to the claude-global repository root
//
// Example:
//
//	config.SetRoot("/home/user/.claude")
//	cfg, err := config.Load()
func SetRoot(path string) {
	claudeGlobalRootGuard.Set(path) // THROUGHLINE: delegates to RootGuard
}

// Root returns the configured claude-global root path.
//
// Returns:
//   - string: the root path, empty if SetRoot() not called
func Root() string {
	return claudeGlobalRootGuard.Path()
}

//--- Loading ---
// Load config.toml. Primary entry point for all config access.

// Load loads the config.toml and returns the parsed Config.
// Tries config.toml first, falls back to hardcoded paths if missing.
//
// Returns:
//   - *Config: loaded configuration with all systems
//   - error: if root not set or config can't be loaded
func Load() (*Config, error) {
	if err := checkRoot(); err != nil { // Helpers > Guard Functions
		return nil, err
	}

	// Check cache first
	if loadedConfig != nil {
		return loadedConfig, nil
	}

	// ═══════════════════════════════════════════════════════════════════════
	// TRIPWIRE: Try config.toml first (single source of truth)
	// If missing, LOUDLY warn and fall back to hardcoded paths
	// ═══════════════════════════════════════════════════════════════════════
	cfg, err := loadConfig() // Core Operations > Config Loading
	if err == nil {
		// Primary path: config.toml worked
		populateConfig(cfg) // Core Operations > Path Extraction
		loadedConfig = cfg  // cache for subsequent calls
		return cfg, nil
	}

	// ⚠️ TRIPWIRE TRIGGERED ⚠️
	util.TripwireBanner("config.toml NOT FOUND", []string{
		"Running on HARDCODED DEFAULTS - this is a fallback, not normal!",
		"Create config.toml to use the single-source-of-truth config.",
	})

	// Fallback: Create config from hardcoded paths (graceful degradation)
	cfg = &Config{
		anchors: fallbackAnchorPaths,    // Helpers > Fallback Data
		exts:    fallbackExtensionPaths, // Helpers > Fallback Data
	}
	loadedConfig = cfg // cache fallback too
	return cfg, nil
}

// MustLoad loads the config or panics. Use in init() or main().
//
// Returns:
//   - *Config: loaded configuration
//
// Panics:
//   - if root not set or config can't be loaded
// MustLoad delegates to util.Must (throughline consolidation).
func MustLoad() *Config {
	return util.Must(Load())
}

//--- Config Access ---
// Direct access to loaded config. Use after Load().

// GetConfig returns the cached config, loading if needed.
//
// Returns:
//   - *Config: the loaded configuration
//   - error: if root not set or config can't be loaded
func GetConfig() (*Config, error) {
	return Load() // delegates to Loading > Load() which handles caching
}

// Systems returns the names of all configured systems.
//
// Returns:
//   - []string: system names in order (e.g., ["anchors", "extensions", ...])
//   - error: if config not loaded
func Systems() ([]string, error) {
	cfg, err := Load()
	if err != nil {
		return nil, err
	}
	return extractSystemNames(cfg), nil // Helpers > Pure Functions
}

//--- Path Accessors ---
// Typed access to configured paths. All use cached config from Load().

// Anchors returns the Claude Code anchor directory paths.
//
// Returns:
//   - *AnchorPaths: typed paths (Agents, Commands, Hooks, etc.)
//   - error: if config not loaded
func Anchors() (*AnchorPaths, error) {
	cfg, err := Load()
	if err != nil {
		return nil, err
	}
	if cfg.anchors == nil {
		return fallbackAnchorPaths, nil // tripwire fallback
	}
	return cfg.anchors, nil
}

// Extensions returns the CPI-SI extension directory paths.
//
// Returns:
//   - *ExtensionPaths: typed paths (Bin, Config, Docs, Pkg, etc.)
//   - error: if config not loaded
func Extensions() (*ExtensionPaths, error) {
	cfg, err := Load()
	if err != nil {
		return nil, err
	}
	if cfg.exts == nil {
		return fallbackExtensionPaths, nil // tripwire fallback
	}
	return cfg.exts, nil
}

// AgentsPath returns the absolute path to the agents directory.
func AgentsPath() (string, error) {
	anchors, err := Anchors()
	if err != nil {
		return "", err
	}
	return filepath.Join(claudeGlobalRootGuard.Path(), anchors.Agents), nil
}

// HooksPath returns the absolute path to the hooks directory.
func HooksPath() (string, error) {
	anchors, err := Anchors()
	if err != nil {
		return "", err
	}
	return filepath.Join(claudeGlobalRootGuard.Path(), anchors.Hooks), nil
}

// PkgPath returns the absolute path to the pkg directory.
func PkgPath() (string, error) {
	exts, err := Extensions()
	if err != nil {
		return "", err
	}
	return filepath.Join(claudeGlobalRootGuard.Path(), exts.Pkg), nil
}

// BinPath returns the absolute path to the bin directory.
func BinPath() (string, error) {
	exts, err := Extensions()
	if err != nil {
		return "", err
	}
	return filepath.Join(claudeGlobalRootGuard.Path(), exts.Bin), nil
}


// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Section order: [GROUP 1: CODING] Validation → Execution → Cleanup →
//                [GROUP 2: DOCUMENTATION] Overview → Policy → Ladder/Baton → Surgical →
//                Performance → Troubleshooting → Related → Future → Quick Reference
// See: bereshit/word/seed/code/go/library.go > CLOSING

// ════════════════════════════════════════════════════════════════
// GROUP 1: CODING - Verify, Use, Clean
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Code Validation
// ────────────────────────────────────────────────────────────────
//
// Prove correctness before shipping. Build, test, verify.
//
// Subsections: Testing Requirements, Build Verification, Integration Testing

//--- Testing Requirements ---
//   - SetRoot must work with valid claude-global path
//   - Load must find and parse config.toml
//   - Anchors(), Extensions() must return typed paths
//   - Path accessors (AgentsPath, HooksPath, etc.) must return absolute paths
//   - Tripwire fallback must work when config.toml missing
//   - Error handling for missing root, invalid TOML

//--- Build Verification ---
//   - go build ./... (compiles without errors)
//   - go vet ./... (no warnings)

//--- Integration Testing ---
//   - Test with hooks, skills, statusline consumers
//   - Verify config loads from actual claude-global structure
//   - Check path accessors return expected directories
//
// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Libraries don't execute - they're imported and called]
//
// Subsections: Import Pattern, Usage Example

//--- Import Pattern ---
//   import "claude-global/pkg/orchestration/config"

//--- Usage Example ---
//
//     package main
//
//     import "claude-global/pkg/orchestration/config"
//
//     func main() {
//         config.SetRoot("/home/user/.claude")
//         cfg, err := config.Load()
//         if err != nil { panic(err) }
//
//         anchors, _ := config.Anchors()
//         fmt.Println("Hooks:", anchors.Hooks)
//     }

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────
//
// Resource management patterns for library consumers.
//
// Subsections: Resource Management, Graceful Shutdown

//--- Resource Management ---
//   - File handles: Closed immediately after reading (no defer needed by caller)
//   - Config data: Cached after first load, garbage collected with package
//   - claudeGlobalRootGuard.Path(): Package-level state, persists until process ends

//--- Graceful Shutdown ---
//   - N/A: Stateless operations, no cleanup required
//   - Load() caches result; subsequent calls return cached *Config

// ════════════════════════════════════════════════════════════════
// GROUP 2: DOCUMENTATION - Synthesis and Reference
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// High-level summary. References METADATA for details.
//
// Subsections: Quick Summary, Public API, Architecture

//--- Quick Summary ---
//   - Central config loader for claude-global
//   - Config-driven: reads config.toml as single source of truth
//   - Tripwire pattern: warn loudly if config.toml missing, use fallbacks
//   - Typed path accessors for all Claude Code and CPI-SI directories
//   - Foundation for all hooks, skills, statusline to use consistent paths

//--- Public API ---
//   Configuration:  SetRoot, Root
//   Loading:        Load, MustLoad
//   Config Access:  GetConfig, Systems
//   Path Accessors: Anchors, Extensions, AgentsPath, HooksPath, PkgPath, BinPath

//--- Architecture ---
//   FOUNDATION - all other packages import and use this
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Guide for future maintainers. What's safe to change.
//
// Subsections: Safe to Modify, Modify with Care, Never Modify

//--- Safe to Modify ---
//   ✅ Add new path accessor functions (e.g., RulesPath, SkillsPath)
//   ✅ Add new fields to AnchorPaths or ExtensionPaths structs
//   ✅ Extend fallback data with new default paths
//   ✅ Add new systems to config.toml parsing

//--- Modify with Care ---
//   ⚠️ Public API function signatures - breaks all consumers
//   ⚠️ Config struct fields - breaks hooks, skills, statusline
//   ⚠️ Fallback paths - affects tripwire behavior

//--- Never Modify ---
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Pattern: SetRoot before Load() calls
//   ❌ Tripwire pattern: config.toml → fallback with warning
//   See "Code Validation" above for validation after changes
//
// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// Points to BODY > Organizational Chart for complete architecture.
//
// Subsections: APU Inventory, Ladder, Baton

//--- APU Inventory ---
//   Total: 16 functions + 2 fallback structs
//   - Public APIs: 10 (SetRoot, Root, Load, MustLoad, GetConfig, Systems,
//                      Anchors, Extensions, AgentsPath, HooksPath, PkgPath, BinPath)
//   - Core Operations: 4 (loadConfig, extractAnchors, extractExtensions, populateConfig)
//   - Error Handling: 2 constructors (LoadError, ConfigError)
//   - Helpers: 3 + 2 structs (extractSystemNames, findSystem, checkRoot,
//                             fallbackAnchorPaths, fallbackExtensionPaths)

//--- Ladder ---
//   Public APIs → Core Operations → Helpers

//--- Baton ---
//   SetRoot → Load → [loadConfig or fallback*Paths] → *Config → path accessors

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Extension guide. Points to BODY subsection extension points.

//--- Extension Points ---
//   - New anchor path: Add field to AnchorPaths, update extractAnchors, add accessor
//   - New extension path: Add field to ExtensionPaths, update extractExtensions, add accessor
//   - New system: Add to config.toml, add extraction function if needed

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Points to performance notes in SETUP/BODY function docstrings.

//--- Performance Notes ---
//   - File I/O: config.toml read once, cached for subsequent calls
//   - Load() is idempotent: first call loads, subsequent calls return cache
//   - Memory: Single *Config held in package-level cache

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Common problems and solutions. Points to BODY function docstrings.

//--- Common Problems ---
// Problem: "root not set" errors
//   - Check: Did you call SetRoot before Load()?
//   - Check: Is the path absolute and valid?
//
// Problem: Tripwire warning (fallback mode)
//   - Check: Does config.toml exist at root?
//   - Check: Is config.toml valid TOML?
//
// Problem: TOML parse errors
//   - Check: Validate TOML syntax (use toml-lint or similar)
//   - Check: Ensure proper [[systems]] table structure
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Points to METADATA Dependencies section.
//
// Subsections: Dependencies, Dependents

//--- Dependencies ---
//   - github.com/BurntSushi/toml (TOML parsing)
//   - config.toml (central configuration file)

//--- Dependents ---
//   - hooks/ (session, tool, prompt hooks)
//   - skills/ (session-awareness, pattern recognition, etc.)
//   - statusline/ (custom status display)

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Roadmap and known limitations.
//
// Subsections: Planned Features, Known Limitations, Version History

//--- Planned Features ---
//   ✓ Config-driven path loading - COMPLETED
//   ✓ Tripwire fallback pattern - COMPLETED
//   ✓ Typed path accessors - COMPLETED
//   ✓ Caching for performance - COMPLETED
//   ⏳ Session config extraction
//   ⏳ Bereshit integration config
//   ⏳ Logging config extraction

//--- Known Limitations ---
//   - Session, Bereshit, Logging systems not yet extracted to typed structs
//   - Requires SetRoot before any Load() calls
//   - No schema validation (config.toml trusted)

//--- Version History ---
//   a-01.00 (2025-12-15) - Initial claude-global implementation
//         - Adapted from Bereshit loader for claude-global
//         - Config-driven system with tripwire fallback
//         - Typed accessors for anchors and extensions
//         - Foundation for all hooks, skills, statusline

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Copy-paste ready examples for common operations.
//
// Subsections: Basic Setup, Typed Access, Generic Loading, Data Access

//--- Basic Setup ---
//     config.SetRoot("/home/user/.claude")
//     cfg, err := config.Load()
//     if err != nil {
//         // Handle error
//     }

//--- Path Access ---
//     anchors, _ := config.Anchors()
//     fmt.Println("Hooks:", anchors.Hooks)
//
//     exts, _ := config.Extensions()
//     fmt.Println("Pkg:", exts.Pkg)

//--- Absolute Paths ---
//     hooksPath, _ := config.HooksPath()  // /home/user/.claude/hooks
//     binPath, _ := config.BinPath()      // /home/user/.claude/bin

//--- Config Inspection ---
//     systems, _ := config.Systems()
//     for _, name := range systems {
//         fmt.Println("System:", name)
//     }

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This library is the FOUNDATION - all other packages import and use it.
// Single source of truth for all paths in claude-global.
//
// "In all thy ways acknowledge him, and he shall direct thy paths." — Proverbs 3:6

// ============================================================================
// END CLOSING
// ============================================================================
