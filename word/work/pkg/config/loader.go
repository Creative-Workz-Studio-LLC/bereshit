// #!omni code --go -library
// #!omni meta.key = B-word-work-pkg-config-loader
// #!omni meta.from = bereshit/word/seed/code/go/library.go
// #!omni meta.at = a-02.50
// ============================================================================
// METADATA
// ============================================================================
//
// Config Loader Library
// See: standards/code/4-block/

// Package config loads and validates Kingdom Technology TOML specifications.
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       B-word-work-pkg-config-loader
//
//	Scripture: "Thy word is a lamp unto my feet" — Psalm 119:105
//	Principle: Specifications illuminate the path; structure guides implementation
//	Anchor:    "Precept upon precept, line upon line" — Isaiah 28:10
//
//	Type:      Ladder (foundation for Phase 3 Config Reader)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-12
//	Version:         a-02.50
//
// # M.2 Version History [HISTORY]
//
//   - a-02.50 (2025-12-14) — PhD rigor + Bible accessibility refinement
//   - a-02.00 (2025-12-14) — Tripwire, dependency validation, error types
//   - a-01.00 (2025-12-12) — Initial creation, basic loading
//
// # M.3 Interface [INTERFACE]
//
//	Requires: stdlib (1): fmt, os, path/filepath, sort | external (-1): github.com/BurntSushi/toml
//	Used by:  tov/demo/phase-0/demo-config, generate-config, Phase 3 Reader
//	Import:   import "bereshit/word/work/pkg/config"
//	Pattern:  SetRoot(path) → LoadAll() → result.Configs["system"]
//
// # M.4 Public API [API]
//
// M.4.1 Configuration [CONFIG]
//
//	SetRoot(path)                                    Set bereshit root
//
// M.4.2 Loading [LOAD]
//
//	LoadAll() LoadResult                             Load all specs (primary)
//	LoadAllFromIndex() LoadResult                    Manifest-driven loading
//	LoadIndex() (*IndexManifest, error)              Get manifest directly
//	LoadSystem(name) ([]*ConfigFile, error)          Load all specs in system
//	LoadSpec(system, spec) (*ConfigFile, error)      Load single spec
//
// M.4.3 Validation (tripwire) [CHECK]
//
//	DiscoverAndCompare() (DiscoveryResult, error)    Compare manifest to disk
//	ValidateDependencyGraph() ([]error, error)       Check dependency tree
//	GetDependencyTree() (map[string]*DependencyNode) Get full graph
//
// M.4.4 Typed Access [ACCESS]
//
//	LoadMath(), LoadTypes(), LoadLanguage(), LoadBible(), LoadHealth(),
//	LoadPermission(), LoadIdentity(), LoadNetwork(), LoadFilesystem()
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
	"os"            // file reading, environment
	"path/filepath" // path construction
	"sort"          // consistent key ordering in extractKeys()
)

//--- I.2 External Packages [IMPORT] (-1) ---
// TOML parsing (Go stdlib lacks support)
import (
	"github.com/BurntSushi/toml" // TOML decoding
)

//--- I.3 Internal Packages [IMPORT] (0) ---
// [Reserved: Pure library — no internal dependencies]

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// Subsections: Building Blocks, Composed Types, Manifest Types, Discovery Types, Error Types

//--- T.1 Building Blocks [TYPE] (1) ---
// Fundamental units returned by Load* functions

// ConfigFile represents a single loaded TOML specification file.
// Access parsed data via Data["section"]["key"].
//
// Example:
//
//	cfg, _ := config.LoadSpec("math", "ternary.toml")
//	cfg.Data["trit"]["values"]  // nested access
//	cfg.Keys                     // ["arithmetic", "packing", "trit", ...]
type ConfigFile struct {
	Name string         // filename only, e.g., "ternary.toml"
	Path string         // full path for debugging/re-reading
	Data map[string]any // raw TOML: Data["section"]["key"]
	Keys []string       // top-level sections, sorted
}

//--- T.2 Composed Types [TYPE] (1) ---
// Aggregates returned by batch loading operations

// LoadResult holds the result of loading multiple specifications.
// Returned by LoadAll() and LoadAllFromIndex().
//
// Example:
//
//	result := config.LoadAll()
//	if !result.Valid {
//	    for _, err := range result.Errors { log.Println(err) }
//	}
//	for _, cfg := range result.Configs["math"] { /* use cfg */ }
type LoadResult struct {
	Valid   bool                     // true only if ALL loaded successfully
	Configs map[string][]*ConfigFile // system name -> configs
	Errors  []error                  // all errors encountered
	Summary map[string][]string      // system -> filenames (quick reference)
}

//--- T.3 Manifest Types [TYPE] (1) ---
// Map directly to word/core/index.toml structure
// See: word/core/index.toml for source of truth

// IndexManifest represents the word/core/index.toml manifest file.
// Single source of truth for all specifications. Used by loader and generator.
type IndexManifest struct {
	Systems []SystemEntry `toml:"systems"` // 9 systems in dependency order (0-8)
}

// SystemEntry represents one of the 9 specification systems.
// Ordered by dependency (0 = foundation anchor, 8 = highest).
type SystemEntry struct {
	Name        string      `toml:"name"`        // e.g., "math", "types"
	Path        string      `toml:"path"`        // relative to word/core/, e.g., "os/health"
	Order       int         `toml:"order"`       // 0 = anchor, higher = depends on lower
	Description string      `toml:"description"` // human-readable purpose
	DependsOn   []string    `toml:"depends_on"`  // system-level dependencies
	Specs       []SpecEntry `toml:"specs"`       // specs in this system
}

// SpecEntry represents a single TOML specification within a system.
type SpecEntry struct {
	File        string   `toml:"file"`        // e.g., "ternary.toml"
	Generates   bool     `toml:"generates"`   // true if generates C header
	Output      string   `toml:"output"`      // e.g., "ternary_math.gen.h"
	Description string   `toml:"description"` // what this spec defines
	DependsOn   []string `toml:"depends_on"`  // "system/file.toml" format
}

//--- T.4 Discovery Types [TYPE] (1) ---
// Tripwire pattern: compare manifest to filesystem, detect drift

// DiscoveryResult holds manifest vs filesystem comparison.
// Used by DiscoverAndCompare() to detect configuration drift.
type DiscoveryResult struct {
	Manifest   []string // files declared in index.toml
	Discovered []string // files found on disk
	Missing    []string // ERROR: in manifest but not on disk
	Unexpected []string // WARNING: on disk but not in manifest
	Valid      bool     // true if no missing files
}

// DependencyNode represents a spec in the dependency graph (DAG).
// Used by ValidateDependencyGraph() for cycle detection.
type DependencyNode struct {
	Spec      string   // "system/file.toml" format
	DependsOn []string // dependencies in same format
	Loaded    bool     // tracking for load order validation
}

//--- T.5 Error Types [TYPE] (1) ---
// Config-driven errors: originate in DATA, manifest in CODE
// Each traces back to source spec. See: BODY > Error Helpers for constructors

// LoadError wraps file-level loading failures.
// Format: "load math/ternary.toml: parse: unexpected EOF"
type LoadError struct {
	File string // spec path, e.g., "math/ternary.toml"
	Op   string // operation: "read", "parse"
	Err  error  // underlying error
}

func (e *LoadError) Error() string {
	return fmt.Sprintf("load %s: %s: %v", e.File, e.Op, e.Err)
}

func (e *LoadError) Unwrap() error { return e.Err }

// ValidationError represents a spec validation failure.
// Format: "types/primitives.toml [int9.min_value]: expected int, got string"
type ValidationError struct {
	Spec    string // e.g., "types/primitives.toml"
	Section string // TOML section, e.g., "int9"
	Field   string // field name, e.g., "min_value"
	Message string // what's wrong
}

func (e *ValidationError) Error() string {
	// Tiered format based on specificity
	if e.Field != "" {
		return fmt.Sprintf("%s [%s.%s]: %s", e.Spec, e.Section, e.Field, e.Message)
	}
	if e.Section != "" {
		return fmt.Sprintf("%s [%s]: %s", e.Spec, e.Section, e.Message)
	}
	return fmt.Sprintf("%s: %s", e.Spec, e.Message)
}

// DependencyError represents a dependency graph problem.
// Format: "math/ternary.toml -> types/missing.toml (missing): not in manifest"
type DependencyError struct {
	Spec    string // spec with the problem
	DepSpec string // problematic dependency (if applicable)
	Kind    string // "missing" or "cycle"
	Message string // details
}

func (e *DependencyError) Error() string {
	if e.DepSpec != "" {
		return fmt.Sprintf("%s -> %s (%s): %s", e.Spec, e.DepSpec, e.Kind, e.Message)
	}
	return fmt.Sprintf("%s (%s): %s", e.Spec, e.Kind, e.Message)
}

// ────────────────────────────────────────────────────────────────
// Type Methods
// ────────────────────────────────────────────────────────────────

//--- TM Type Methods [METHOD] (0) ---
// [Reserved: Phase 3 — typed accessors, conversion methods]
// LoadError implements error interface (Error(), Unwrap())
// ValidationError implements error interface (Error())
// DependencyError implements error interface (Error())

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────
//
// Filesystem paths for navigating the 9-system architecture.
// Primary source is index.toml; paths here are fallback for direct access.
//
// 9-System Architecture (dependency order, 0=anchor):
//   0. math         — ternary foundation
//   1. types        — primitives, composition, validation
//   2. language     — keywords, syntax
//   3. bible        — scripture text, addressing, encoding
//   4. os/health    — scores, diagnostics, logging
//   5. os/permission— access control
//   6. identity     — model, contract
//   7. network      — message, timestamp, contract
//   8. filesystem   — types, contract

//--- K.1 Core Root [DATA] (1) ---
// Base path for all specs. Used with index.toml SystemEntry.Path.
const (
	CorePath  = "word/core"   // relative to bereshit root
	IndexFile = "index.toml"  // manifest filename within CorePath
)

//--- K.2 System Paths [DATA] (1) ---
// Path constants for system directories and files.
// Config-driven: index.toml provides paths via SystemEntry.Path
// Tripwire: Helpers > Fallback Data > fallbackSystemPaths

//--- K.3 Type Constants [DATA] (0) ---
// [Reserved: No type constants — uses index.toml string values]

//--- K.4 Validation Thresholds [DATA] (0) ---
// [Reserved: No thresholds — validation at spec load time]

// ────────────────────────────────────────────────────────────────
// Variables
// ────────────────────────────────────────────────────────────────
//
// Package-level mutable state. Kept minimal — explicit SetRoot() over implicit.
//
// Subsections: Configuration State, Sentinel Errors, Registries

//--- V.1 Configuration State [DATA] (1) ---
// Set once at startup, read by all Load* functions.
var (
	bereshitRoot string      // absolute path, set via SetRoot()
	loadedIndex  *IndexManifest // cached index after first load
)

//--- V.2 Sentinel Errors [DATA] (1) ---
// Unexported — wrapped by public error types for context.
// See: BODY > Helpers > checkRoot (errRootNotSet)
var (
	errRootNotSet = fmt.Errorf("bereshit root not set — call SetRoot() first")
)

//--- V.3 Registries [DATA] (1) ---
// [Reserved: Phase 3 — cached manifests, loaded specs registry]

// ────────────────────────────────────────────────────────────────
// Package-Level State
// ────────────────────────────────────────────────────────────────
//
// Cross-package coordination via established conventions (like Rails).
// Currently explicit (SetRoot call); Phase 3+ adds init() auto-discovery.
//
// Subsections: Coordination Pattern, Initialization Order, Reserved Features

//--- PS.1 Coordination Pattern [DOC] (1) ---
// All bereshit packages call config.SetRoot() at startup.
// Root path flows to all Load* functions via package variable.

//--- PS.2 Initialization Order [DOC] (1) ---
// Sequence of operations for proper initialization.
//   1. SetRoot() called by main/demo with absolute path
//   2. LoadAll() parses index.toml and loads specs
//   3. GetSystem()/GetSpec() return loaded entries

//--- PS.3 Reserved Features [DOC] (0) ---
// [Reserved: Phase 3+ — Rails infrastructure for cross-package coordination]
// [Reserved: init() auto-discovery from BERESHIT_ROOT or environment]

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
//   ├── Configuration:    SetRoot()
//   ├── Batch Loading:    LoadAll(), LoadAllFromIndex()
//   ├── Index Access:     LoadIndex()
//   ├── Discovery:        DiscoverAndCompare(), ValidateDependencyGraph(), GetDependencyTree()
//   ├── Generic Loading:  LoadSystem(), LoadSpec()
//   └── Typed Wrappers:   LoadMath(), LoadTypes(), ... LoadFilesystem(), LoadConstants
//
//   Error Handling (Error constructors)
//   └── newLoadError(), newValidationError(), newDependencyError()
//
//   Core Operations (Middle Rungs - Business Logic)
//   ├── Index Loading:      loadIndex()
//   ├── File Loading:       loadFile() → extractKeys()
//   ├── Directory Loading:  loadDirectory() → loadFile()
//   ├── Discovery:          discoverFiles(), compareManifestToDisc()
//   └── Dependencies:       buildDependencyGraph(), validateDependencies()
//
//   Helpers (Bottom Rungs - Foundations)
//   ├── Pure Functions:     extractKeys(), collectNames()
//   ├── Guard Functions:    checkRoot()
//   └── Fallback Data:      fallbackSystemPaths

//--- Baton Flow ---
// Horizontal execution paths. Entry → processing → Exit. Follow for debugging.
//
//   Entry → SetRoot(path)
//     ↓
//   LoadAll() ─────────────────────────────────────────────────┐
//     │                                                        │
//     ↓ (primary)                                              ↓ (tripwire fallback)
//   LoadAllFromIndex()                                       LoadSystem() × 9
//     │                                                        │
//     ├─ Step 1: loadIndex() → manifest                       │
//     ├─ Step 2: buildDependencyGraph() → validateDependencies()
//     ├─ Step 3: compareManifestToDisc() → tripwire          fallbackSystemPaths
//     ├─ Step 4: loadFile() for each spec                      │
//     └─ extractKeys() for each file                         loadDirectory()
//     ↓                                                        ↓
//   Exit → LoadResult with all configs ←──────────────────────┘

//--- APU Inventory ---
// Count of Available Processing Units by category.
//
//   Total: 25 functions + 1 map
//   ├── Helpers: 3 functions + 1 map
//   │   ├── Pure Functions:  extractKeys(), collectNames()
//   │   ├── Guard Functions: checkRoot()
//   │   └── Fallback Data:   fallbackSystemPaths
//   ├── Core Operations: 7 functions
//   │   ├── loadIndex(), loadFile(), loadDirectory()
//   │   ├── discoverFiles(), compareManifestToDisc()
//   │   └── buildDependencyGraph(), validateDependencies()
//   ├── Error Handling: 3 functions
//   │   └── newLoadError(), newValidationError(), newDependencyError()
//   └── Public APIs: 12 functions + 10 wrappers
//       ├── SetRoot(), LoadAll(), LoadAllFromIndex(), LoadIndex()
//       ├── DiscoverAndCompare(), ValidateDependencyGraph(), GetDependencyTree()
//       ├── LoadSystem(), LoadSpec()
//       └── 9 typed wrappers + 1 deprecated (LoadConstants)

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
// Used by: Core Operations > Index Loading, File Loading
func fileExists(path string) bool {
	_, err := os.Stat(path)
	return !os.IsNotExist(err)
}

// globTOML finds all .toml files in a directory.
//
// Parameters:
//   - dirPath: directory to search
//
// Returns:
//   - []string: matching file paths
//   - error: glob pattern errors
//
// Used by: Core Operations > Directory Loading, Discovery
func globTOML(dirPath string) ([]string, error) {
	pattern := filepath.Join(dirPath, "*.toml")
	matches, err := filepath.Glob(pattern)
	if err != nil {
		return nil, fmt.Errorf("glob error for %s: %w", pattern, err)
	}
	return matches, nil
}

// extractKeys returns sorted top-level section names from a TOML map.
//
// Parameters:
//   - data: raw TOML map from toml.DecodeFile
//
// Returns:
//   - []string: sorted section names, used to populate ConfigFile.Keys
//
// Used by: Core Operations > File Loading
func extractKeys(data map[string]any) []string {
	keys := make([]string, 0, len(data))
	for k := range data {
		keys = append(keys, k)
	}
	sort.Strings(keys) // deterministic order for testing
	return keys
}

// collectNames extracts filenames from a ConfigFile slice.
//
// Parameters:
//   - configs: slice of loaded ConfigFile pointers
//
// Returns:
//   - []string: filenames only (no paths), used for LoadResult.Summary
//
// Used by: Public APIs > Load, LoadByIndex
func collectNames(configs []*ConfigFile) []string {
	names := make([]string, 0, len(configs))
	for _, c := range configs {
		names = append(names, c.Name)
	}
	return names // order preserved from configs (slice, not map)
}

//--- H.2 Guard Functions [CHECK] (1) ---
// Precondition checks. Call at function entry to fail fast.
// See: SETUP > Variables > Sentinel Errors (errors returned by guards)

// checkRoot validates bereshitRoot has been set via SetRoot().
//
// Returns:
//   - error: errRootNotSet if bereshitRoot is empty, nil otherwise
//
// Used by: Public APIs (all public functions require root)
func checkRoot() error {
	if bereshitRoot == "" {
		return errRootNotSet // see: SETUP > Variables > Sentinel Errors
	}
	return nil
}

//--- H.3 Domain Functions [DOMAIN] (0) ---
// Reserved: loader.go focuses on loading, not domain transformation.
// See: Future config.go > Helpers > Domain Functions

//--- H.4 Fallback Data [DATA] (1) ---
// Static maps for tripwire fallback when index.toml unavailable.
// Tripwire: K.2 references this section. If K.2 Tripwire breaks, check here.

// fallbackSystemPaths maps system names to hardcoded paths.
// Used ONLY when index.toml is missing (tripwire fallback).
var fallbackSystemPaths = map[string]string{
	"math":       "word/core/math",
	"types":      "word/core/types",
	"language":   "word/core/language",
	"bible":      "word/core/bible",
	"health":     "word/core/os/health",
	"permission": "word/core/os/permission",
	"identity":   "word/core/identity",
	"network":    "word/core/network",
	"filesystem": "word/core/filesystem",
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
//
// Middle rung. Depends on Helpers below, used by Public APIs above.
// See: Org Chart > Ladder Structure > Core Operations
//
// Subsections: Index Loading, File Loading, Directory Loading, Discovery, Dependencies

//--- C.1 Index Loading [LOAD] (1) ---
// Manifest operations. Entry point for manifest-driven loading.
// Uses: Helpers > Pure Functions > fileExists, SETUP > Constants > CorePath, IndexFile

// loadIndex loads and parses the word/core/index.toml manifest.
//
// Returns:
//   - *IndexManifest: parsed manifest with all systems and specs
//   - error: file not found or TOML parse errors
func loadIndex() (*IndexManifest, error) {
	indexPath := filepath.Join(bereshitRoot, CorePath, IndexFile) // CorePath, IndexFile from SETUP > Constants
	if !fileExists(indexPath) {                                   // Helpers > Pure Functions
		return nil, fmt.Errorf("index.toml not found: %s", indexPath)
	}

	var manifest IndexManifest
	if _, err := toml.DecodeFile(indexPath, &manifest); err != nil { // BurntSushi/toml
		return nil, fmt.Errorf("index.toml parse error: %w", err)
	}

	// Sort systems by order (0 = anchor, higher = depends on lower)
	sort.Slice(manifest.Systems, func(i, j int) bool {
		return manifest.Systems[i].Order < manifest.Systems[j].Order
	})

	return &manifest, nil
}

//--- C.2 File Loading [LOAD] (1) ---
// Single file operations. Core workhorse, used by Directory Loading and Public APIs.
// Uses: Helpers > Pure Functions > fileExists, extractKeys

// loadFile loads and parses a single TOML file.
//
// Parameters:
//   - path: full filesystem path to the TOML file
//
// Returns:
//   - *ConfigFile: parsed config with name, path, data, and top-level keys
//   - error: file not found or TOML parse errors
func loadFile(path string) (*ConfigFile, error) {
	if !fileExists(path) { // Helpers > Pure Functions
		return nil, fmt.Errorf("file not found: %s", path)
	}

	var data map[string]any
	if _, err := toml.DecodeFile(path, &data); err != nil { // BurntSushi/toml
		return nil, fmt.Errorf("TOML parse error in %s: %w", path, err)
	}

	return &ConfigFile{
		Name: filepath.Base(path),
		Path: path,
		Data: data,
		Keys: extractKeys(data), // Helpers > Pure Functions
	}, nil
}

//--- C.3 Directory Loading [LOAD] (1) ---
// Batch file operations. Builds on File Loading for multi-file directories.
// Uses: Helpers > Pure Functions > globTOML, Core Operations > File Loading > loadFile

// loadDirectory loads all TOML files from a directory.
//
// Parameters:
//   - dirPath: directory path to scan for TOML files
//
// Returns:
//   - []*ConfigFile: all successfully loaded configs (partial on error)
//   - error: first error encountered, partial results still returned
func loadDirectory(dirPath string) ([]*ConfigFile, error) {
	matches, err := globTOML(dirPath) // Helpers > Pure Functions
	if err != nil {
		return nil, err
	}

	var configs []*ConfigFile
	for _, path := range matches {
		cfg, err := loadFile(path) // Core Operations > File Loading
		if err != nil {
			return configs, err // partial results on error
		}
		configs = append(configs, cfg)
	}

	return configs, nil
}

//--- C.4 Discovery [DISCOVER] (1) ---
// Filesystem awareness. Finds what exists on disk for tripwire comparison.
// Uses: Helpers > Pure Functions > globTOML, Helpers > Fallback Data > fallbackSystemPaths

// discoverFiles finds all .toml files in a system directory.
//
// Parameters:
//   - systemPath: full path to system directory (e.g., bereshit/word/core/math)
//
// Returns:
//   - []string: sorted filenames found (e.g., ["ternary.toml"])
//   - error: directory read errors
func discoverFiles(systemPath string) ([]string, error) {
	matches, err := globTOML(systemPath) // Helpers > Pure Functions
	if err != nil {
		return nil, err
	}

	var files []string
	for _, path := range matches {
		files = append(files, filepath.Base(path))
	}
	sort.Strings(files) // deterministic order for comparison
	return files, nil
}

// compareManifestToDisc compares index.toml entries to actual files on disk.
// Tripwire mechanism: missing = ERROR (broken promise), unexpected = WARNING (untracked).
//
// Parameters:
//   - manifest: parsed IndexManifest from loadIndex()
//
// Returns:
//   - DiscoveryResult: comparison with Missing, Unexpected, and Valid status
func compareManifestToDisc(manifest *IndexManifest) DiscoveryResult {
	result := DiscoveryResult{Valid: true}

	for _, system := range manifest.Systems {
		systemPath := filepath.Join(bereshitRoot, CorePath, system.Path)

		// Collect manifest entries for this system
		var manifestFiles []string
		for _, spec := range system.Specs {
			manifestFiles = append(manifestFiles, spec.File)
			result.Manifest = append(result.Manifest, system.Path+"/"+spec.File)
		}
		sort.Strings(manifestFiles)

		// Discover what actually exists
		discoveredFiles, err := discoverFiles(systemPath) // Discovery (above)
		if err != nil {                                   // directory missing = all files missing
			for _, f := range manifestFiles {
				result.Missing = append(result.Missing, system.Path+"/"+f)
			}
			result.Valid = false
			continue
		}

		for _, f := range discoveredFiles {
			result.Discovered = append(result.Discovered, system.Path+"/"+f)
		}

		// Build sets for O(1) lookup
		manifestSet := make(map[string]bool)
		for _, f := range manifestFiles {
			manifestSet[f] = true
		}
		discoveredSet := make(map[string]bool)
		for _, f := range discoveredFiles {
			discoveredSet[f] = true
		}

		// Missing: in manifest but not on disk (ERROR)
		for _, f := range manifestFiles {
			if !discoveredSet[f] {
				result.Missing = append(result.Missing, system.Path+"/"+f)
				result.Valid = false
			}
		}

		// Unexpected: on disk but not in manifest (WARNING)
		for _, f := range discoveredFiles {
			if !manifestSet[f] {
				result.Unexpected = append(result.Unexpected, system.Path+"/"+f)
			}
		}
	}

	return result
}

//--- C.5 Dependencies [GRAPH] (1) ---
// Graph operations. Validates dependency DAG before loading.
// Uses: SETUP > Types > IndexManifest, SystemEntry

// buildDependencyGraph creates a map of all specs and their dependencies.
//
// Parameters:
//   - manifest: parsed IndexManifest from loadIndex()
//
// Returns:
//   - map[string]*DependencyNode: graph keyed by "system/file.toml"
func buildDependencyGraph(manifest *IndexManifest) map[string]*DependencyNode {
	graph := make(map[string]*DependencyNode)

	for _, system := range manifest.Systems {
		for _, spec := range system.Specs {
			key := system.Path + "/" + spec.File
			graph[key] = &DependencyNode{
				Spec:      key,
				DependsOn: spec.DependsOn,
				Loaded:    false, // used by topological sort in future phases
			}
		}
	}

	return graph
}

// validateDependencies checks that all dependencies exist and are acyclic.
//
// Parameters:
//   - graph: dependency graph from buildDependencyGraph()
//
// Returns:
//   - []error: validation errors (empty if valid)
func validateDependencies(graph map[string]*DependencyNode) []error {
	var errs []error

	// Phase 1: Check all referenced deps exist in manifest
	for spec, node := range graph {
		for _, dep := range node.DependsOn {
			if _, exists := graph[dep]; !exists {
				errs = append(errs, newDependencyError(spec, dep, "missing",
					"dependency not in manifest")) // Error Handling > Error Constructors
			}
		}
	}

	// Phase 2: Check for cycles using DFS
	visited := make(map[string]bool) // specs fully processed
	inStack := make(map[string]bool) // specs in current path (cycle detection)

	var checkCycle func(spec string) bool
	checkCycle = func(spec string) bool {
		if inStack[spec] {
			return true // cycle: same spec twice in path
		}
		if visited[spec] {
			return false // already verified
		}

		visited[spec] = true
		inStack[spec] = true

		if node, exists := graph[spec]; exists {
			for _, dep := range node.DependsOn {
				if checkCycle(dep) {
					return true
				}
			}
		}

		inStack[spec] = false
		return false
	}

	for spec := range graph {
		if checkCycle(spec) {
			errs = append(errs, newDependencyError(spec, "", "cycle",
				"circular dependency detected"))
			break // one cycle error is enough
		}
		visited = make(map[string]bool) // reset for next starting point
		inStack = make(map[string]bool)
	}

	return errs
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
//   - LoadError:       file-level (not found, parse failed)
//   - ValidationError: spec-level (wrong type, missing field)
//   - DependencyError: reference-level (missing dep, cycle)
//
// Recovery Strategy:
//   - Non-blocking: collect all errors, continue loading other specs
//   - Context-rich: every error traces to file → section → field
//   - Actionable: message tells you what to fix in the data

//--- Error Constructors ---
// Factory functions for error types. Used by Core Operations and Public APIs.

// newLoadError creates a LoadError with file and operation context.
//
// Parameters:
//   - file: spec path (e.g., "math/ternary.toml")
//   - op: operation that failed ("read", "parse")
//   - err: underlying error
//
// Returns:
//   - *LoadError: wrapped error with context
func newLoadError(file, op string, err error) *LoadError {
	return &LoadError{File: file, Op: op, Err: err}
}

// newValidationError creates a ValidationError with location context.
//
// Parameters:
//   - spec: spec path (e.g., "types/primitives.toml")
//   - section: TOML section (e.g., "int9")
//   - field: field name (e.g., "min_value"), empty if section-level
//   - message: what's wrong
//
// Returns:
//   - *ValidationError: error with full location context
func newValidationError(spec, section, field, message string) *ValidationError {
	return &ValidationError{Spec: spec, Section: section, Field: field, Message: message}
}

// newDependencyError creates a DependencyError for graph problems.
//
// Parameters:
//   - spec: spec with the problem
//   - depSpec: problematic dependency (empty if cycle)
//   - kind: "missing" or "cycle"
//   - message: details
//
// Returns:
//   - *DependencyError: error with dependency context
func newDependencyError(spec, depSpec, kind, message string) *DependencyError {
	return &DependencyError{Spec: spec, DepSpec: depSpec, Kind: kind, Message: message}
}

//--- Recovery Patterns ---
// [Reserved: Phase 3 — error collection, aggregation, partial load, defaults]

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
//
// Top rung. Orchestrates Core Operations for external callers.
// See: Org Chart > Ladder Structure > Public APIs
//
// Subsections: Configuration, Batch Loading, Index Access, Discovery, Generic Loading, Typed Wrappers

//--- Configuration ---
// Package state initialization. Must be called before Load* functions.

// SetRoot sets the bereshit root directory path.
//
// Parameters:
//   - path: absolute path to the bereshit repository root
//
// Example:
//
//	config.SetRoot("/home/user/Project/Bereshit")
//	result := config.LoadAll()
func SetRoot(path string) {
	bereshitRoot = path // SETUP > Variables > Configuration State
}

//--- Batch Loading ---
// Load all specs at once. Primary entry points for full system loading.

// LoadAll loads all config specs and returns a summary.
// Tries LoadAllFromIndex() first, falls back to typed loaders if index.toml missing.
//
// Returns:
//   - LoadResult: Valid, Configs (by system), Errors, Summary
func LoadAll() LoadResult {
	result := LoadResult{
		Valid:   true,                          // assume valid until proven otherwise
		Configs: make(map[string][]*ConfigFile), // system -> loaded configs
		Errors:  []error{},                      // accumulator for all errors
		Summary: make(map[string][]string),      // quick reference of what loaded
	}

	if bereshitRoot == "" { // guard: require SetRoot() first
		result.Valid = false
		result.Errors = append(result.Errors, fmt.Errorf("bereshit root not set - call SetRoot() first"))
		return result
	}

	// ═══════════════════════════════════════════════════════════════════════
	// TRIPWIRE: Try index.toml first (single source of truth)
	// If missing, LOUDLY warn and fall back to hardcoded paths
	// ═══════════════════════════════════════════════════════════════════════
	indexResult := LoadAllFromIndex()
	if indexResult.Valid {
		// Primary path: index.toml worked
		return indexResult
	}

	// Check if it's specifically an index.toml issue
	for _, err := range indexResult.Errors {
		if err.Error() == "index.toml not found: "+bereshitRoot+"/word/core/index.toml" {
			// ⚠️ TRIPWIRE TRIGGERED ⚠️
			fmt.Println("════════════════════════════════════════════════════════════════")
			fmt.Println("⚠️  TRIPWIRE: word/core/index.toml NOT FOUND")
			fmt.Println("════════════════════════════════════════════════════════════════")
			fmt.Println("  Running on HARDCODED DEFAULTS - this is a fallback, not normal!")
			fmt.Println("  Create index.toml to use the single-source-of-truth manifest.")
			fmt.Println("════════════════════════════════════════════════════════════════")
			break
		}
	}

	// Fallback: Load using hardcoded paths directly (graceful degradation)
	// Uses fallbackSystemPaths → loadDirectory. No loadIndex calls (already failed above).
	for system, path := range fallbackSystemPaths {
		dirPath := filepath.Join(bereshitRoot, path) // path includes "word/core/" prefix
		configs, err := loadDirectory(dirPath)       // Core Operations > Directory Loading
		if err != nil {
			result.Errors = append(result.Errors, fmt.Errorf("%s: %w", system, err))
			result.Valid = false
			continue // partial results - try other systems
		}
		result.Configs[system] = configs               // key matches manifest's system.Name
		result.Summary[system] = collectNames(configs) // Helpers > Pure Functions
	}

	return result
}

// LoadAllFromIndex loads all configs using word/core/index.toml manifest.
// Manifest-driven: validates deps, compares to disk (tripwire), loads in order.
//
// Returns:
//   - LoadResult: same structure as LoadAll() for compatibility
func LoadAllFromIndex() LoadResult {
	result := LoadResult{
		Valid:   true,
		Configs: make(map[string][]*ConfigFile),
		Errors:  []error{},
		Summary: make(map[string][]string),
	}

	if bereshitRoot == "" {
		result.Valid = false
		result.Errors = append(result.Errors, fmt.Errorf("bereshit root not set - call SetRoot() first"))
		return result
	}

	// ═══════════════════════════════════════════════════════════════════════
	// STEP 1: Load the manifest
	// ═══════════════════════════════════════════════════════════════════════
	manifest, err := loadIndex()
	if err != nil {
		result.Valid = false
		result.Errors = append(result.Errors, err)
		return result
	}

	// ═══════════════════════════════════════════════════════════════════════
	// STEP 2: Validate dependency graph
	// ═══════════════════════════════════════════════════════════════════════
	graph := buildDependencyGraph(manifest)
	depErrors := validateDependencies(graph)
	if len(depErrors) > 0 {
		for _, e := range depErrors {
			result.Errors = append(result.Errors, e)
		}
		result.Valid = false
		// Continue anyway - report dependency errors but try to load
	}

	// ═══════════════════════════════════════════════════════════════════════
	// STEP 3: Compare manifest to disk (tripwire)
	// ═══════════════════════════════════════════════════════════════════════
	discovery := compareManifestToDisc(manifest)
	if !discovery.Valid {
		fmt.Println("════════════════════════════════════════════════════════════════")
		fmt.Println("⚠️  TRIPWIRE: Manifest/Disk Mismatch")
		fmt.Println("════════════════════════════════════════════════════════════════")
		for _, missing := range discovery.Missing {
			fmt.Printf("  ❌ MISSING: %s (in manifest, not on disk)\n", missing)
			result.Errors = append(result.Errors, fmt.Errorf("missing file: %s", missing))
		}
		result.Valid = false
		fmt.Println("════════════════════════════════════════════════════════════════")
	}
	if len(discovery.Unexpected) > 0 {
		fmt.Println("════════════════════════════════════════════════════════════════")
		fmt.Println("⚠️  WARNING: Unexpected files (on disk, not in manifest)")
		fmt.Println("════════════════════════════════════════════════════════════════")
		for _, unexpected := range discovery.Unexpected {
			fmt.Printf("  ⚠️  UNEXPECTED: %s\n", unexpected)
		}
		fmt.Println("  Consider adding these to word/core/index.toml")
		fmt.Println("════════════════════════════════════════════════════════════════")
		// Unexpected files are warnings, not errors - don't set Valid = false
	}

	// ═══════════════════════════════════════════════════════════════════════
	// STEP 4: Load all specs in dependency order
	// ═══════════════════════════════════════════════════════════════════════
	for _, system := range manifest.Systems {
		systemPath := filepath.Join(bereshitRoot, CorePath, system.Path)
		var configs []*ConfigFile

		// Load each spec in the system
		for _, spec := range system.Specs {
			specPath := filepath.Join(systemPath, spec.File)
			cfg, err := loadFile(specPath)
			if err != nil {
				result.Errors = append(result.Errors, fmt.Errorf("%s/%s: %w", system.Name, spec.File, err))
				result.Valid = false
				continue
			}
			configs = append(configs, cfg)

			// Mark as loaded in dependency graph
			key := system.Path + "/" + spec.File
			if node, exists := graph[key]; exists {
				node.Loaded = true
			}
		}

		if len(configs) > 0 {
			result.Configs[system.Name] = configs
			result.Summary[system.Name] = collectNames(configs)
		}
	}

	return result
}

//--- Index Access ---
// Expose manifest for external tools (generator, CLI).

// LoadIndex loads and returns the word/core/index.toml manifest.
//
// Returns:
//   - *IndexManifest: parsed manifest with all systems and specs
//   - error: if root not set or manifest can't be loaded
func LoadIndex() (*IndexManifest, error) {
	if err := checkRoot(); err != nil { // Helpers > Guard Functions
		return nil, err
	}
	return loadIndex() // Core Operations > Index Loading
}

//--- Discovery ---
// Tripwire patterns. Detect manifest/filesystem drift before it causes problems.

// DiscoverAndCompare compares manifest entries to files on disk.
//
// Returns:
//   - DiscoveryResult: Missing (ERROR), Unexpected (WARNING), Valid status
//   - error: if manifest can't be loaded
func DiscoverAndCompare() (DiscoveryResult, error) {
	if err := checkRoot(); err != nil {
		return DiscoveryResult{}, err
	}
	manifest, err := loadIndex()
	if err != nil {
		return DiscoveryResult{}, err
	}
	return compareManifestToDisc(manifest), nil // Core Operations > Discovery
}

// ValidateDependencyGraph checks that all dependencies exist and are acyclic.
//
// Returns:
//   - []error: validation errors (empty if valid)
//   - error: if manifest can't be loaded
func ValidateDependencyGraph() ([]error, error) {
	if err := checkRoot(); err != nil {
		return nil, err
	}

	manifest, err := loadIndex()
	if err != nil {
		return nil, err
	}

	graph := buildDependencyGraph(manifest)
	return validateDependencies(graph), nil
}

// GetDependencyTree returns the full dependency graph for visualization/analysis.
//
// Returns:
//   - map[string]*DependencyNode: graph keyed by "system/file.toml"
//   - error: if manifest can't be loaded
func GetDependencyTree() (map[string]*DependencyNode, error) {
	if err := checkRoot(); err != nil {
		return nil, err
	}
	manifest, err := loadIndex()
	if err != nil {
		return nil, err
	}
	return buildDependencyGraph(manifest), nil // Core Operations > Dependencies
}

//--- Generic Loading ---
// Manifest-driven loading. Falls back to Helpers > Fallback Data when index.toml missing.

// LoadSystem loads all specs in a system by name.
// Uses index.toml manifest; falls back to fallbackSystemPaths if missing.
//
// Parameters:
//   - system: system name (e.g., "math", "bible")
//
// Returns:
//   - []*ConfigFile: all specs in the system
//   - error: system not found or file errors
func LoadSystem(system string) ([]*ConfigFile, error) {
	if err := checkRoot(); err != nil {
		return nil, err
	}

	// ═══════════════════════════════════════════════════════════════════════
	// TRIPWIRE: Try index.toml first
	// ═══════════════════════════════════════════════════════════════════════
	manifest, err := loadIndex()
	if err == nil {
		// Primary path: use manifest
		for _, sys := range manifest.Systems {
			if sys.Name == system {
				systemPath := filepath.Join(bereshitRoot, CorePath, sys.Path)
				var configs []*ConfigFile
				for _, spec := range sys.Specs {
					specPath := filepath.Join(systemPath, spec.File)
					cfg, err := loadFile(specPath)
					if err != nil {
						return configs, fmt.Errorf("%s/%s: %w", system, spec.File, err)
					}
					configs = append(configs, cfg)
				}
				return configs, nil
			}
		}
		return nil, fmt.Errorf("system %q not found in index.toml", system)
	}

	// ⚠️ TRIPWIRE TRIGGERED ⚠️
	fmt.Printf("⚠️  TRIPWIRE: LoadSystem(%q) - index.toml not found, using fallback\n", system)

	// Fallback: use hardcoded path
	fallbackPath, ok := fallbackSystemPaths[system]
	if !ok {
		return nil, fmt.Errorf("system %q not found and no fallback path defined", system)
	}
	path := filepath.Join(bereshitRoot, fallbackPath)
	return loadDirectory(path)
}

// LoadSpec loads a single spec from a system.
// Uses index.toml manifest; falls back to fallbackSystemPaths if missing.
//
// Parameters:
//   - system: system name (e.g., "math", "bible")
//   - spec: spec filename (e.g., "ternary.toml")
//
// Returns:
//   - *ConfigFile: the loaded spec
//   - error: spec not found or file errors
func LoadSpec(system, spec string) (*ConfigFile, error) {
	if err := checkRoot(); err != nil {
		return nil, err
	}

	// ═══════════════════════════════════════════════════════════════════════
	// TRIPWIRE: Try index.toml first
	// ═══════════════════════════════════════════════════════════════════════
	manifest, err := loadIndex()
	if err == nil {
		// Primary path: use manifest
		for _, sys := range manifest.Systems {
			if sys.Name == system {
				systemPath := filepath.Join(bereshitRoot, CorePath, sys.Path)
				for _, s := range sys.Specs {
					if s.File == spec {
						specPath := filepath.Join(systemPath, s.File)
						return loadFile(specPath)
					}
				}
				return nil, fmt.Errorf("spec %q not found in system %q", spec, system)
			}
		}
		return nil, fmt.Errorf("system %q not found in index.toml", system)
	}

	// ⚠️ TRIPWIRE TRIGGERED ⚠️
	fmt.Printf("⚠️  TRIPWIRE: LoadSpec(%q, %q) - index.toml not found, using fallback\n", system, spec)

	// Fallback: use hardcoded path
	fallbackPath, ok := fallbackSystemPaths[system]
	if !ok {
		return nil, fmt.Errorf("system %q not found and no fallback path defined", system)
	}
	specPath := filepath.Join(bereshitRoot, fallbackPath, spec)
	return loadFile(specPath)
}

//--- Typed Wrappers ---
// Thin wrappers for typed access. All delegate to LoadSystem().

// LoadMath loads math system (ternary foundation).
func LoadMath() ([]*ConfigFile, error) { return LoadSystem("math") }

// LoadTypes loads types system (primitives, composition, validation).
func LoadTypes() ([]*ConfigFile, error) { return LoadSystem("types") }

// LoadLanguage loads language system (keywords, syntax).
func LoadLanguage() ([]*ConfigFile, error) { return LoadSystem("language") }

// LoadBible loads bible system (scripture text, addressing, encoding).
func LoadBible() ([]*ConfigFile, error) { return LoadSystem("bible") }

// LoadHealth loads os/health system (scores, diagnostics, logging).
func LoadHealth() ([]*ConfigFile, error) { return LoadSystem("health") }

// LoadPermission loads os/permission system (access control).
func LoadPermission() ([]*ConfigFile, error) { return LoadSystem("permission") }

// LoadIdentity loads identity system (model, contract).
func LoadIdentity() ([]*ConfigFile, error) { return LoadSystem("identity") }

// LoadNetwork loads network system (message, timestamp, contract).
func LoadNetwork() ([]*ConfigFile, error) { return LoadSystem("network") }

// LoadFilesystem loads filesystem system (types, contract).
func LoadFilesystem() ([]*ConfigFile, error) { return LoadSystem("filesystem") }


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
//   - SetRoot must work with valid bereshit path
//   - LoadAll must find and parse all TOML files in word/core/
//   - LoadSystem, LoadSpec must work for all 9 systems
//   - DiscoverAndCompare must detect manifest/filesystem drift
//   - ValidateDependencyGraph must catch cycles and missing deps
//   - Extracted keys must match actual TOML structure
//   - Error handling for missing files, invalid TOML, bad paths

//--- Build Verification ---
//   - go build ./... (compiles without errors)
//   - go vet ./... (no warnings)

//--- Integration Testing ---
//   - Test with tov/demo/phase-0/demo-config/
//   - Verify configs load from actual bereshit structure
//   - Check LoadResult.Summary for expected file counts
//
// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Libraries don't execute - they're imported and called]
//
// Subsections: Import Pattern, Usage Example

//--- Import Pattern ---
//   import "bereshit/word/work/pkg/config"

//--- Usage Example ---
//
//     package main
//
//     import "bereshit/word/work/pkg/config"
//
//     func main() {
//         config.SetRoot("/path/to/bereshit")
//         result := config.LoadAll()
//         // Use result.Configs, result.Summary
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
//   - TOML data: In-memory maps, garbage collected
//   - bereshitRoot: Package-level state, persists until process ends

//--- Graceful Shutdown ---
//   - N/A: Stateless operations, no cleanup required
//   - Each Load* function reads and returns, no open resources

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
//   - Config loader for Phase 0 TOML specifications
//   - Manifest-driven: reads index.toml as single source of truth
//   - Tripwire pattern: detect manifest/filesystem drift
//   - Dependency validation: catch cycles, missing refs before loading
//   - Foundation for Phase 3 Config Reader

//--- Public API ---
//   Configuration:  SetRoot
//   Loading:        LoadAll, LoadAllFromIndex, LoadIndex, LoadSystem, LoadSpec
//   Validation:     DiscoverAndCompare, ValidateDependencyGraph, GetDependencyTree
//   Typed Access:   LoadMath, LoadTypes, LoadLanguage, LoadBible, LoadHealth,
//                   LoadPermission, LoadIdentity, LoadNetwork, LoadFilesystem
//   Deprecated:     LoadConstants (use LoadSpec or LoadMath)

//--- Architecture ---
//   LADDER - provides structure that Phase 3 builds upon
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Guide for future maintainers. What's safe to change.
//
// Subsections: Safe to Modify, Modify with Care, Never Modify

//--- Safe to Modify ---
//   ✅ Add new Load* functions for new config directories
//   ✅ Add new path constants for new config locations
//   ✅ Enhance extractKeys for deeper TOML parsing
//   ✅ Add new fields to ConfigFile or LoadResult

//--- Modify with Care ---
//   ⚠️ Public API function signatures - breaks demo and future consumers
//   ⚠️ ConfigFile/LoadResult struct fields - breaks accessing code
//   ⚠️ Path constants - affects what gets loaded

//--- Never Modify ---
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Pattern: SetRoot before Load* calls
//   ❌ Return types: LoadResult for LoadAll, (*ConfigFile, error) for singles
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
//   Total: 25 functions + 1 map
//   - Public APIs: 12 + 10 wrappers (9 typed + LoadConstants deprecated)
//   - Core Operations: 7 (index, file, directory, discovery, dependencies)
//   - Error Handling: 3 constructors (LoadError, ValidationError, DependencyError)
//   - Helpers: 3 + 1 map (extractKeys, collectNames, checkRoot, fallbackSystemPaths)

//--- Ladder ---
//   Public APIs → Core Operations → Helpers

//--- Baton ---
//   SetRoot → LoadAll → [LoadAllFromIndex or fallbackSystemPaths] → result

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Extension guide. Points to BODY subsection extension points.

//--- Extension Points ---
//   - New config location: Add constant in SETUP, function in Public APIs
//   - New loader type: Add Load[Name] following existing pattern
//   - Enhanced parsing: Extend extractKeys helper

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Points to performance notes in SETUP/BODY function docstrings.

//--- Performance Notes ---
//   - File I/O: Each Load* reads from disk; cache results if called repeatedly
//   - LoadAll: Loads all configs at once; more efficient than multiple Load* calls
//   - Memory: All parsed TOML held in memory until result goes out of scope

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Common problems and solutions. Points to BODY function docstrings.

//--- Common Problems ---
// Problem: "file not found" errors
//   - Check: Did you call SetRoot with valid bereshit path?
//   - Check: Does word/core/ directory exist at that path?
//
// Problem: Empty configs returned
//   - Check: Are TOML files present in expected directories?
//   - Check: Is bereshitRoot set before calling Load* functions?
//
// Problem: TOML parse errors
//   - Check: Validate TOML syntax (use toml-lint or similar)
//   - Check: Ensure proper quoting of strings, valid table syntax
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
//   - word/core/ configs (TOML spec files)

//--- Dependents ---
//   - tov/demo/phase-0/demo-config/ (Phase 0 demo)
//   - Phase 3 Config Reader (future)

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Roadmap and known limitations.
//
// Subsections: Planned Features, Known Limitations, Version History

//--- Planned Features ---
//   ✓ Basic TOML loading - COMPLETED
//   ✓ Directory loading - COMPLETED
//   ✓ Key extraction - COMPLETED
//   ✓ Manifest-driven loading (index.toml) - COMPLETED
//   ✓ Tripwire pattern (manifest/disk comparison) - COMPLETED
//   ✓ Dependency graph validation - COMPLETED
//   ✓ Folder discovery - COMPLETED
//   ⏳ Phase 3: Config Reader integration
//   ⏳ Schema validation
//   ⏳ Type-safe config structs

//--- Known Limitations ---
//   - No validation against schemas (future Phase 3)
//   - Top-level keys only (no nested key extraction)
//   - Requires SetRoot before any Load* calls
//   - loadIndex() called multiple times (consider caching if perf issue)

//--- Version History ---
//   a-02.00 (2025-12-14) - Tripwire pattern, dependency validation
//         - Manifest-driven loading from index.toml
//         - Folder discovery and manifest comparison
//         - Dependency graph building and validation
//         - checkRoot() helper to reduce duplication
//   a-01.00 (2025-12-12) - Initial implementation
//         - Basic loader for Phase 0 demo
//         - Foundation for Phase 3 Config Reader

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Copy-paste ready examples for common operations.
//
// Subsections: Basic Setup, Typed Access, Generic Loading, Data Access

//--- Basic Setup ---
//     config.SetRoot("/path/to/bereshit")
//     result := config.LoadAll()
//     if !result.Valid {
//         // Handle errors
//     }

//--- Typed Access ---
//     math, err := config.LoadMath()      // All math/*.toml
//     bible, err := config.LoadBible()    // All bible/*.toml
//     types, err := config.LoadTypes()    // All types/*.toml

//--- Generic Loading ---
//     // Load entire system
//     configs, err := config.LoadSystem("bible")
//
//     // Load single spec
//     cfg, err := config.LoadSpec("math", "ternary.toml")
//
//     // Access manifest directly (for generators/tools)
//     manifest, err := config.LoadIndex()
//     for _, sys := range manifest.Systems {
//         fmt.Printf("System: %s, Path: %s\n", sys.Name, sys.Path)
//     }

//--- Data Access ---
//     for _, cfg := range result.Configs["bible"] {
//         fmt.Printf("File: %s, Keys: %v\n", cfg.Name, cfg.Keys)
//     }

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This library is a LADDER component - foundation for Phase 3 Config Reader.
// Proves "specs exist and validate" for Phase 0.
//
// "Thy word is a lamp unto my feet, and a light unto my path." — Psalm 119:105

// ============================================================================
// END CLOSING
// ============================================================================
