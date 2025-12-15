// #!omni code --go
// ═══════════════════════════════════════════════════════════════════════════
// Config Loader Library (4-Block Structure)
// Key: B-word-work-pkg-config-loader
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: PURE (standard library + external TOML parser)
//   - Uses BurntSushi/toml for TOML parsing
//   - No internal project dependencies
//
// derives_from: bereshit/word/seed/code/go/library.go
// Derived from: Kingdom Technology 4-block code structure
//
// ═══════════════════════════════════════════════════════════════════════════

// Package config provides loading and validation of Kingdom Technology config files.
//
// Config Loader Library - CPI-SI Bereshit Foundation
//
// ────────────────────────────────────────────────────────────────
// CORE IDENTITY (Required)
// ────────────────────────────────────────────────────────────────
//
// # Biblical Foundation
//
// Scripture: "Thy word is a lamp unto my feet, and a light unto my path."
//            — Psalm 119:105 KJV
//
// Principle: Config files are the "word" that guides system behavior.
//            The loader illuminates these specifications, making them
//            accessible and validated for all systems to follow.
//
// Anchor: "The entrance of thy words giveth light; it giveth understanding
//          unto the simple." — Psalm 119:130 KJV
//
// # CPI-SI Identity
//
// Component Type: Ladder (foundation that Phase 3+ builds upon)
//
// Role: Load and validate Phase 0 config definitions (TOML files)
//
// Paradigm: CPI-SI framework component - config as stored truth
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-12
//   - Version: a-01.00
//   - Modified: 2025-12-12 - Initial creation for Phase 0 demo
//
// Version History:
//
//   - a-01.00 (2025-12-12) - Initial creation, basic loading and validation
//
// # Purpose & Function
//
// Purpose: Load Phase 0 config files and prove they exist and validate
//
// Core Design: Simple loader that Phase 3 Config Reader will extend
//
// Key Features:
//
//   - Load TOML configs from word/core/ directory structure
//   - Validate TOML syntax parses without errors
//   - Report what sections/keys exist in each config
//   - Foundation for Phase 3 typed config reader
//
// Philosophy: Prove the configs work now, type them fully in Phase 3
//
// ────────────────────────────────────────────────────────────────
// INTERFACE (Expected)
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: fmt, os, path/filepath, strings
//   - External: github.com/BurntSushi/toml (TOML parsing)
//   - Internal: None (foundation component)
//
// What Uses This:
//
//   - Commands: tov/demo/phase-0/demo-config (Phase 0 demo)
//   - Libraries: Phase 3 Config Reader (will extend this)
//   - Tools: Future validation tools
//
// Integration Points:
//
//   - Reads from: word/core/ directory (primitives, types, schemas, contracts, bible)
//   - Returns: LoadResult with parsed data and validation status
//   - Extends to: Phase 3 adds typed structs for each config
//
// # Usage & Integration
//
// Import:
//
//	import "bereshit/word/work/pkg/config"
//
// Integration Pattern:
//
//  1. Call config.SetRoot() to set bereshit root path
//  2. Call config.LoadAll() to load all configs
//  3. Check result.Valid and result.Errors
//  4. Access result.Configs for parsed data
//
// Public API (in typical usage order):
//
//	Configuration:
//	  SetRoot(path) - Set bereshit root directory
//
//	Loading:
//	  LoadAll() LoadResult - Load all Phase 0 configs
//	  LoadPrimitives() (*ConfigFile, error) - Load primitives.toml
//	  LoadTypes() (*ConfigFile, error) - Load types.toml
//	  LoadSchemas() ([]*ConfigFile, error) - Load all schemas
//	  LoadContracts() ([]*ConfigFile, error) - Load all contracts
//	  LoadBibleRail() ([]*ConfigFile, error) - Load bible rail configs
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL (Contextual)
// ────────────────────────────────────────────────────────────────
//
// # Blocking Status
//
// Non-blocking: Config loading failures don't crash, they report errors
//
// Mitigation: Each config loaded independently, failures collected in LoadResult.Errors
//
// # Health Scoring
//
// [OMIT: Phase 0 foundation - health scoring infrastructure not yet built]
//
package config

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Imports → Types → Type Methods → Constants → Variables → Package-Level State

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- Standard Library ---
import (
	"fmt"           // Error formatting and output
	"os"            // File operations
	"path/filepath" // Path manipulation
	"sort"          // Sorting keys for consistent output
)

//--- External Packages ---
import (
	"github.com/BurntSushi/toml" // TOML parsing - stdlib lacks TOML support
)

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

//--- Building Blocks ---

// ConfigFile represents a single loaded TOML config file.
//
// This is the fundamental unit returned by all Load* functions.
// Contains both the raw parsed data and metadata about the file.
//
// Usage:
//   cfg, err := config.LoadPrimitives()
//   fmt.Println(cfg.Name)  // "primitives.toml"
//   fmt.Println(cfg.Keys)  // ["primitives", "temporal", "addressing"]
type ConfigFile struct {
	Name string         // filename only (e.g., "primitives.toml")
	Path string         // full filesystem path for debugging
	Data map[string]any // raw TOML structure - access via Data["section"]["key"]
	Keys []string       // top-level section names, alphabetically sorted
}

//--- Composed Types ---

// LoadResult holds the result of loading all Phase 0 configs.
//
// Returned by LoadAll() - aggregates all config files with error tracking.
// Valid is false if ANY config failed to load; check Errors for details.
//
// Usage:
//   result := config.LoadAll()
//   if !result.Valid {
//       for _, err := range result.Errors { log.Println(err) }
//   }
//   for _, cfg := range result.Configs["schemas"] { /* use cfg */ }
type LoadResult struct {
	Valid   bool                     // true only if ALL configs loaded successfully
	Configs map[string][]*ConfigFile // category -> configs ("core", "schemas", etc.)
	Errors  []error                  // all errors encountered during loading
	Summary map[string][]string      // quick reference: category -> filenames or keys
}

//--- Configuration Types ---
// [Reserved: No configuration types needed - uses package variable for root path]

//--- Error Types ---
// [Reserved: Uses standard errors - no custom error types needed]

//--- Index Manifest Types ---
// For reading word/core/index.toml manifest

// IndexManifest represents the word/core/index.toml manifest file.
// Single source of truth for both loader and generator.
type IndexManifest struct {
	Systems []SystemEntry `toml:"systems"`
}

// SystemEntry represents one of the 9 systems in the manifest.
type SystemEntry struct {
	Name        string      `toml:"name"`
	Path        string      `toml:"path"`
	Order       int         `toml:"order"`
	Description string      `toml:"description"`
	Specs       []SpecEntry `toml:"specs"`
}

// SpecEntry represents a single TOML spec within a system.
type SpecEntry struct {
	File        string `toml:"file"`
	Generates   bool   `toml:"generates"`
	Output      string `toml:"output"`
	Description string `toml:"description"`
}

// ────────────────────────────────────────────────────────────────
// Type Methods
// ────────────────────────────────────────────────────────────────

//--- Interface Implementations ---
// [Reserved: No interface implementations needed - types use direct field access]

//--- Conversion Methods ---
// [Reserved: No conversion methods needed yet - Phase 3 will add typed conversions]

//--- Accessor Patterns ---
// [Reserved: Direct field access used - no getters/setters needed]

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────

//--- Path Constants ---
// Relative paths within bereshit for 9-system architecture.
// Structure: word/core/{system}/*.toml
//
// Dependency order (anchor → up):
//   1. math/         - Pure constants (foundation)
//   2. types/        - Primitives, composition, validation
//   3. language/     - Keywords, syntax
//   4. bible/        - Scripture addressing, encoding, text
//   5. os/           - Health (score, log, diagnostics, provider), permission
//   6. identity/     - Model, contract
//   7. network/      - Contract, message, timestamp
//   8. filesystem/   - Contract, types

const (
	// CorePath is the relative path to core definitions.
	CorePath = "word/core"

	// System paths - the 9 systems under word/core/
	MathPath       = "word/core/math"
	TypesPath      = "word/core/types"
	LanguagePath   = "word/core/language"
	BiblePath      = "word/core/bible"
	OSPath         = "word/core/os"
	HealthPath     = "word/core/os/health"
	PermissionPath = "word/core/os/permission"
	IdentityPath   = "word/core/identity"
	NetworkPath    = "word/core/network"
	FilesystemPath = "word/core/filesystem"
)

//--- Defaults ---
// [Reserved: No default values needed - root path required]

// ────────────────────────────────────────────────────────────────
// Variables
// ────────────────────────────────────────────────────────────────

//--- Registries ---
// [Reserved: No registries needed - configs loaded on demand]

//--- Configuration State ---
// Root path for bereshit directory.

var (
	// bereshitRoot is the absolute path to bereshit directory.
	// Must be set via SetRoot() before loading configs.
	bereshitRoot string
)

// ────────────────────────────────────────────────────────────────
// Package-Level State (Rails Pattern)
// ────────────────────────────────────────────────────────────────

//--- Rails Infrastructure ---
// [Reserved: Phase 0 foundation - Rails infrastructure not yet built]

//--- Initialization ---
// [Reserved: No init() needed - SetRoot() called explicitly]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
//
// -----------------------------------------------------------------------------
// BODY Sections Overview
// -----------------------------------------------------------------------------
//
// 1. ORGANIZATIONAL CHART (Internal Structure)
//    Purpose: Map dependencies and execution flow within this component
//    Subsections: Ladder Structure → Baton Flow → Module Dependencies → APUs
//
// 2. HELPERS/UTILITIES (Internal Support)
//    Purpose: Foundation functions - simple, focused, reusable utilities
//    Subsections: Pure Functions → Utility Functions → [Reserved if extracted]
//
// 3. CORE OPERATIONS (Business Logic)
//    Purpose: Component-specific functionality implementing primary purpose
//    Subsections: [Category 1] → [Category 2] → ... (organized by concern)
//
// 4. ERROR HANDLING/RECOVERY (Safety Patterns)
//    Purpose: Centralized error management and recovery strategies
//    Subsections: Design Principle → Recovery Strategy → Helper Functions
//
// 5. PUBLIC APIs (Exported Interface)
//    Purpose: Top-level orchestration - simple functions calling proven pieces
//    Subsections: [Category 1] → [Category 2] → ... (organized by purpose)
//
// Section order: Org Chart → Helpers → Core Operations → Error Handling → Public APIs
// This flows: understand structure → build foundations → implement logic → handle errors → expose interface
//
// Universal mapping (see standards for cross-language patterns):
//   Organizational Chart ≈ Dependency/Flow Documentation
//   Helpers/Utilities ≈ Internal Functions (static/private)
//   Core Operations ≈ Business Logic (the work)
//   Error Handling ≈ Recovery/Safety Patterns
//   Public APIs ≈ Exported Interface (what others call)

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Internal Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - Orchestration)
//   ├── SetRoot() → sets bereshitRoot variable
//   ├── LoadAll() → tries LoadAllFromIndex(), falls back to LoadSystem()
//   ├── LoadAllFromIndex() → uses loadIndex(), loadFile()
//   ├── LoadIndex() → exposes loadIndex() for external tools
//   ├── LoadSystem(system) → manifest-driven, uses loadIndex(), loadFile()
//   ├── LoadSpec(system, spec) → manifest-driven, uses loadIndex(), loadFile()
//   └── Typed Wrappers → LoadMath(), LoadTypes(), etc. delegate to LoadSystem()
//
//   Core Operations (Middle Rungs - Business Logic)
//   ├── loadIndex() → uses toml.DecodeFile() for manifest
//   ├── loadFile() → uses toml.DecodeFile(), extractKeys()
//   └── loadDirectory() → uses filepath.Glob(), loadFile()
//
//   Helpers (Bottom Rungs - Foundations)
//   ├── extractKeys() → pure function (map → sorted keys)
//   ├── collectNames() → pure function (configs → filenames)
//   └── fallbackSystemPaths → hardcoded paths for tripwire fallback
//
// Baton Flow (Execution Paths):
//
//   Entry → SetRoot(path)
//     ↓
//   LoadAll() ─────────────────────────────┐
//     │                                    │
//     ↓ (primary)                          ↓ (tripwire fallback)
//   LoadAllFromIndex()                   LoadSystem() × 9
//     │                                    │
//     ↓                                    ↓
//   loadIndex() → manifest              fallbackSystemPaths → loadDirectory()
//     │
//     ↓
//   loadFile() for each spec
//     │
//     ↓
//   extractKeys() for each loaded file
//     ↓
//   Exit → LoadResult with all configs
//
// APUs (Available Processing Units):
// - 16 functions total
// - 2 helpers (extractKeys, collectNames) + 1 map (fallbackSystemPaths)
// - 3 core operations (loadIndex, loadFile, loadDirectory)
// - 13 public APIs (SetRoot, LoadAll, LoadAllFromIndex, LoadIndex,
//                   LoadSystem, LoadSpec, + 9 typed wrappers)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────

// extractKeys returns sorted top-level keys from a parsed TOML map.
//
// Purpose:
//   Extracts section names from TOML files so we can report what's defined
//   without parsing the full structure. This gives a quick "what's in here?"
//   answer for validation and summary reporting.
//
// Parameters:
//   - data: The parsed TOML structure as a map (from BurntSushi/toml decoder)
//
// Returns:
//   - Alphabetically sorted slice of top-level key names
//
// Example:
//   For a TOML file with [primitives], [temporal], [addressing] sections,
//   returns: ["addressing", "primitives", "temporal"]
func extractKeys(data map[string]any) []string {
	keys := make([]string, 0, len(data)) // pre-allocate with exact capacity
	for k := range data {                // walk map, collect top-level TOML section names
		keys = append(keys, k)
	}
	sort.Strings(keys) // alphabetical for consistent output
	return keys
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────

// ────────────────────────────────────────────────────────────────
// Index Loading - Manifest operations
// ────────────────────────────────────────────────────────────────

// loadIndex loads and parses the word/core/index.toml manifest.
//
// Purpose:
//   Reads the single source of truth manifest that defines all specs.
//   Both loader and generator use this to discover what to process.
//
// Returns:
//   - *IndexManifest: Parsed manifest with all systems and specs
//   - error: File not found or TOML parse errors
func loadIndex() (*IndexManifest, error) {
	indexPath := filepath.Join(bereshitRoot, CorePath, "index.toml")
	if _, err := os.Stat(indexPath); os.IsNotExist(err) {
		return nil, fmt.Errorf("index.toml not found: %s", indexPath)
	}

	var manifest IndexManifest
	if _, err := toml.DecodeFile(indexPath, &manifest); err != nil {
		return nil, fmt.Errorf("index.toml parse error: %w", err)
	}

	// Sort systems by order
	sort.Slice(manifest.Systems, func(i, j int) bool {
		return manifest.Systems[i].Order < manifest.Systems[j].Order
	})

	return &manifest, nil
}

// ────────────────────────────────────────────────────────────────
// File Loading - Single file operations
// ────────────────────────────────────────────────────────────────

// loadFile loads and parses a single TOML file.
//
// Purpose:
//   Core file loading operation. Validates file exists, parses TOML structure,
//   and returns a ConfigFile with metadata and extracted keys.
//
// Parameters:
//   - path: Full filesystem path to the TOML file
//
// Returns:
//   - *ConfigFile: Parsed config with name, path, data, and top-level keys
//   - error: File not found or TOML parse errors
//
// Error handling:
//   - Returns wrapped error with file context for debugging
//   - Caller decides whether to continue or abort on error
func loadFile(path string) (*ConfigFile, error) {
	if _, err := os.Stat(path); os.IsNotExist(err) { // verify file exists before attempting parse
		return nil, fmt.Errorf("file not found: %s", path)
	}

	var data map[string]any                              // TOML decodes into generic map
	if _, err := toml.DecodeFile(path, &data); err != nil { // BurntSushi/toml handles parsing
		return nil, fmt.Errorf("TOML parse error in %s: %w", path, err)
	}

	return &ConfigFile{
		Name: filepath.Base(path), // just filename, not full path
		Path: path,                // full path for debugging/re-reading
		Data: data,                // raw parsed TOML structure
		Keys: extractKeys(data),   // top-level section names for summary
	}, nil
}

// ────────────────────────────────────────────────────────────────
// Directory Loading - Multiple file operations
// ────────────────────────────────────────────────────────────────

// loadDirectory loads all TOML files from a directory.
//
// Purpose:
//   Batch loading for directories with multiple config files (schemas/, contracts/, bible/).
//   Uses glob pattern to find all .toml files, then loads each one.
//
// Parameters:
//   - dirPath: Directory path to scan for TOML files
//
// Returns:
//   - []*ConfigFile: All successfully loaded configs (may be partial on error)
//   - error: First error encountered, with partial results returned
//
// Behavior:
//   - Stops on first error but returns already-loaded configs
//   - Empty directory returns empty slice, not error
func loadDirectory(dirPath string) ([]*ConfigFile, error) {
	pattern := filepath.Join(dirPath, "*.toml") // glob pattern for all TOML files
	matches, err := filepath.Glob(pattern)      // find all matching files
	if err != nil {
		return nil, fmt.Errorf("glob error for %s: %w", pattern, err)
	}

	var configs []*ConfigFile       // accumulator for loaded configs
	for _, path := range matches {  // iterate through each matched file
		cfg, err := loadFile(path)  // delegate to single-file loader
		if err != nil {
			return configs, err     // return partial results with error
		}
		configs = append(configs, cfg)
	}

	return configs, nil
}

// ────────────────────────────────────────────────────────────────
// Error Handling/Recovery Patterns
// ────────────────────────────────────────────────────────────────
//
// Design Principle: Non-blocking - individual config failures don't prevent
// loading other configs. LoadAll collects errors and continues.
//
// Recovery Strategy:
//   - File not found: Error returned, other configs continue loading
//   - TOML parse error: Error returned with file context, other configs continue
//   - Root not set: Error returned immediately from LoadAll
//
// [Reserved: Simple error wrapping via fmt.Errorf - no custom recovery patterns yet]

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────

// ═══ Configuration ═══

// SetRoot sets the bereshit root directory path.
//
// Purpose:
//   Must be called before any Load* functions. Establishes the base path
//   from which all config paths are resolved.
//
// Parameters:
//   - path: Absolute path to the bereshit repository root
//
// Example:
//   config.SetRoot("/home/user/Project/Bereshit")
//   result := config.LoadAll()
func SetRoot(path string) {
	bereshitRoot = path // stored at package level, persists for all subsequent Load* calls
}

// ═══ Loading - All Configs ═══

// LoadAll loads all Phase 0 config files and returns a summary.
//
// Purpose:
//   Primary entry point for loading all Phase 0 specifications at once.
//   Orchestrates loading of primitives, types, schemas, contracts, and bible rail.
//   Continues loading even if individual files fail - collects all errors.
//
// Prerequisites:
//   - SetRoot() must be called first with valid bereshit path
//
// Returns:
//   - LoadResult containing:
//     - Valid: true if ALL configs loaded successfully
//     - Configs: map of loaded configs by category
//     - Errors: all errors encountered (may have errors even if partially valid)
//     - Summary: quick reference of what was loaded
//
// Behavior:
//   - Non-blocking: continues loading after individual failures
//   - Collects all errors for comprehensive error reporting
//   - Summary shows file names for directories, keys for single files
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

	// Fallback: Load using hardcoded paths (graceful degradation)
	// 1. Math (foundation - pure constants)
	if math, err := LoadMath(); err != nil {
		result.Errors = append(result.Errors, err)
		result.Valid = false
	} else {
		result.Configs["math"] = math
		result.Summary["math"] = collectNames(math)
	}

	// 2. Types (primitives, composition, validation)
	if types, err := LoadTypes(); err != nil {
		result.Errors = append(result.Errors, err)
		result.Valid = false
	} else {
		result.Configs["types"] = types
		result.Summary["types"] = collectNames(types)
	}

	// 3. Language (keywords, syntax)
	if lang, err := LoadLanguage(); err != nil {
		result.Errors = append(result.Errors, err)
		result.Valid = false
	} else {
		result.Configs["language"] = lang
		result.Summary["language"] = collectNames(lang)
	}

	// 4. Bible (addressing, encoding, decoding, translation, scripture-text)
	if bible, err := LoadBible(); err != nil {
		result.Errors = append(result.Errors, err)
		result.Valid = false
	} else {
		result.Configs["bible"] = bible
		result.Summary["bible"] = collectNames(bible)
	}

	// 5. OS/Health (score, log, diagnostics, provider)
	if health, err := LoadHealth(); err != nil {
		result.Errors = append(result.Errors, err)
		result.Valid = false
	} else {
		result.Configs["os/health"] = health
		result.Summary["os/health"] = collectNames(health)
	}

	// 6. OS/Permission (access)
	if perm, err := LoadPermission(); err != nil {
		result.Errors = append(result.Errors, err)
		result.Valid = false
	} else {
		result.Configs["os/permission"] = perm
		result.Summary["os/permission"] = collectNames(perm)
	}

	// 7. Identity (model, contract)
	if id, err := LoadIdentity(); err != nil {
		result.Errors = append(result.Errors, err)
		result.Valid = false
	} else {
		result.Configs["identity"] = id
		result.Summary["identity"] = collectNames(id)
	}

	// 8. Network (contract, message, timestamp)
	if net, err := LoadNetwork(); err != nil {
		result.Errors = append(result.Errors, err)
		result.Valid = false
	} else {
		result.Configs["network"] = net
		result.Summary["network"] = collectNames(net)
	}

	// 9. Filesystem (contract, types)
	if fs, err := LoadFilesystem(); err != nil {
		result.Errors = append(result.Errors, err)
		result.Valid = false
	} else {
		result.Configs["filesystem"] = fs
		result.Summary["filesystem"] = collectNames(fs)
	}

	return result
}

// collectNames extracts filenames from a slice of ConfigFiles for summary.
func collectNames(configs []*ConfigFile) []string {
	names := make([]string, 0, len(configs))
	for _, c := range configs {
		names = append(names, c.Name)
	}
	return names
}

// LoadAllFromIndex loads all configs using word/core/index.toml manifest.
//
// Purpose:
//   Manifest-driven loading - reads index.toml to discover what to load.
//   Single source of truth shared with generator.
//
// Advantages over LoadAll():
//   - Adding new specs only requires editing index.toml, not Go code
//   - Loader and generator stay in sync automatically
//   - Dependency order defined in manifest, not hardcoded
//
// Returns:
//   - LoadResult: Same structure as LoadAll() for compatibility
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

	// Load the manifest
	manifest, err := loadIndex()
	if err != nil {
		result.Valid = false
		result.Errors = append(result.Errors, err)
		return result
	}

	// Walk systems in dependency order (manifest is pre-sorted)
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
		}

		if len(configs) > 0 {
			result.Configs[system.Name] = configs
			result.Summary[system.Name] = collectNames(configs)
		}
	}

	return result
}

// LoadIndex loads and returns the word/core/index.toml manifest.
//
// Purpose:
//   Exposes the manifest for external tools (like generator) to use.
//   Returns the same data structure used internally.
func LoadIndex() (*IndexManifest, error) {
	if bereshitRoot == "" {
		return nil, fmt.Errorf("bereshit root not set - call SetRoot() first")
	}
	return loadIndex()
}

// ═══ Generic Loading - Manifest-Driven ═══
// These functions use index.toml manifest as the primary source.
// Hardcoded fallbacks only trigger when index.toml is missing.

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

// LoadSystem loads all specs in a system by name.
//
// Purpose:
//   Generic manifest-driven loading. Reads index.toml to find the system
//   and loads all its specs. Falls back to hardcoded path if index missing.
//
// Parameters:
//   - system: System name as defined in index.toml (e.g., "math", "bible")
//
// Returns:
//   - []*ConfigFile: All specs in the system
//   - error: System not found, or file errors
//
// Example:
//   configs, err := config.LoadSystem("math")
//   // Returns all specs: ternary.toml
func LoadSystem(system string) ([]*ConfigFile, error) {
	if bereshitRoot == "" {
		return nil, fmt.Errorf("bereshit root not set - call SetRoot() first")
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
//
// Purpose:
//   Load one specific TOML file by system and spec name.
//   Uses index.toml manifest to resolve paths.
//
// Parameters:
//   - system: System name (e.g., "math", "bible")
//   - spec: Spec filename (e.g., "ternary.toml", "scripture-text.toml")
//
// Returns:
//   - *ConfigFile: The loaded spec
//   - error: Spec not found or file errors
//
// Example:
//   cfg, err := config.LoadSpec("math", "ternary.toml")
func LoadSpec(system, spec string) (*ConfigFile, error) {
	if bereshitRoot == "" {
		return nil, fmt.Errorf("bereshit root not set - call SetRoot() first")
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

// ═══ Typed Access - Thin Wrappers ═══
// These provide typed access to specific systems.
// They delegate to generic LoadSystem for manifest-driven loading.

// LoadMath loads word/core/math/*.toml (foundation constants).
// Thin wrapper around LoadSystem("math") for typed access.
func LoadMath() ([]*ConfigFile, error) {
	return LoadSystem("math")
}

// LoadTypes loads word/core/types/*.toml (primitives, composition, validation).
// Thin wrapper around LoadSystem("types") for typed access.
func LoadTypes() ([]*ConfigFile, error) {
	return LoadSystem("types")
}

// LoadLanguage loads word/core/language/*.toml (keywords, syntax).
// Thin wrapper around LoadSystem("language") for typed access.
func LoadLanguage() ([]*ConfigFile, error) {
	return LoadSystem("language")
}

// LoadBible loads word/core/bible/*.toml.
// Thin wrapper around LoadSystem("bible") for typed access.
func LoadBible() ([]*ConfigFile, error) {
	return LoadSystem("bible")
}

// LoadHealth loads word/core/os/health/*.toml.
// Thin wrapper around LoadSystem("health") for typed access.
func LoadHealth() ([]*ConfigFile, error) {
	return LoadSystem("health")
}

// LoadPermission loads word/core/os/permission/*.toml.
// Thin wrapper around LoadSystem("permission") for typed access.
func LoadPermission() ([]*ConfigFile, error) {
	return LoadSystem("permission")
}

// LoadIdentity loads word/core/identity/*.toml.
// Thin wrapper around LoadSystem("identity") for typed access.
func LoadIdentity() ([]*ConfigFile, error) {
	return LoadSystem("identity")
}

// LoadNetwork loads word/core/network/*.toml.
// Thin wrapper around LoadSystem("network") for typed access.
func LoadNetwork() ([]*ConfigFile, error) {
	return LoadSystem("network")
}

// LoadFilesystem loads word/core/filesystem/*.toml.
// Thin wrapper around LoadSystem("filesystem") for typed access.
func LoadFilesystem() ([]*ConfigFile, error) {
	return LoadSystem("filesystem")
}

// LoadConstants loads word/core/math/ternary.toml.
// Deprecated: Use LoadSpec("math", "ternary.toml") for single spec,
// or LoadMath() for all math specs.
func LoadConstants() ([]*ConfigFile, error) {
	cfg, err := LoadSpec("math", "ternary.toml")
	if err != nil {
		return nil, err
	}
	return []*ConfigFile{cfg}, nil
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
//
// -----------------------------------------------------------------------------
// CLOSING Sections Overview
// -----------------------------------------------------------------------------
//
// GROUP 1: CODING (Operations - Verify, Use, Clean)
//
// 1. CODE VALIDATION (Testing & Verification)
//    Purpose: Prove correctness before shipping - build, test, verify
//    Subsections: Testing Requirements → Integration Testing → Example Usage
//
// 2. CODE EXECUTION (Library Usage)
//    [Reserved: Libraries don't execute - they're imported and called]
//
// 3. CODE CLEANUP (Resource Management)
//    Purpose: Resource management patterns for library consumers
//    Subsections: Resource Patterns → Error Handling Patterns
//
// GROUP 2: FINAL DOCUMENTATION (Synthesis - Reference Back to Earlier Blocks)
//
// 4. LIBRARY OVERVIEW (Summary with Back-References)
//    Purpose: High-level summary pointing back to METADATA for details
//    References: METADATA "Purpose & Function", "Key Features", "Usage & Integration"
//
// 5. MODIFICATION POLICY (Safe/Careful/Never)
//    Purpose: Guide future maintainers on what's safe to change
//    Subsections: Safe to Modify → Modify with Care → Never Modify → Validation After
//
// 6. LADDER AND BATON FLOW (Back-Reference to BODY)
//    Purpose: Point to BODY Organizational Chart for architecture
//    References: BODY "Organizational Chart - Internal Structure"
//
// 7. SURGICAL UPDATE POINTS (Back-Reference to BODY)
//    Purpose: Point to BODY subsection extension points
//    References: BODY "Core Operations" subsection comments
//
// 8. PERFORMANCE CONSIDERATIONS (Back-Reference to SETUP/BODY)
//    Purpose: Point to performance notes in earlier sections
//    References: SETUP constants/types, BODY function docstrings
//
// 9. TROUBLESHOOTING GUIDE (Back-Reference to BODY)
//    Purpose: Point to troubleshooting in function docstrings
//    References: BODY function docstrings with troubleshooting sections
//
// 10. RELATED COMPONENTS (Back-Reference to METADATA)
//     Purpose: Point to METADATA Dependencies section
//     References: METADATA "Dependencies" section
//
// 11. FUTURE EXPANSIONS (Roadmap)
//     Purpose: Planned features, research areas, integration targets
//     Subsections: Planned Features → Research Areas → Integration Targets → Known Limitations
//
// 12. CONTRIBUTION GUIDELINES (How to Contribute)
//     Purpose: Guide for contributing to this component
//     Subsections: How to Contribute → Scripture/Grounding
//
// 13. QUICK REFERENCE (Usage Examples)
//     Purpose: Copy-paste ready examples for common operations
//     Subsections: Basic Setup → [Pattern Examples] → Advanced Usage
//
// Section order: Validation → [Execution Reserved] → Cleanup → Overview → Policy → Ladder/Baton →
//                Surgical → Performance → Troubleshooting → Related → Future → Contribution → Reference
// This flows: verify → (no execution) → clean → document → guide future work
//
// ════════════════════════════════════════════════════════════════
// GROUP 1: CODING
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Code Validation: Config Loader
// ────────────────────────────────────────────────────────────────
//
// Testing Requirements:
//   - SetRoot must work with valid bereshit path
//   - LoadAll must find and parse all TOML files in word/core/
//   - LoadPrimitives, LoadTypes, LoadSchemas, LoadContracts, LoadBibleRail
//     must return correct files for their respective directories
//   - Extracted keys must match actual TOML structure
//   - Error handling for missing files, invalid TOML, bad paths
//
// Build Verification:
//   - go build ./... (compiles without errors)
//   - go vet ./... (no warnings)
//
// Integration Testing:
//   - Test with tov/demo/phase-0/demo-config/
//   - Verify configs load from actual bereshit structure
//   - Check LoadResult.Summary for expected file counts
//
// ────────────────────────────────────────────────────────────────
// Code Execution: None (Library)
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Library - imported and called, not executed]
//
// Usage: import "bereshit/word/work/pkg/config"
//
// Example:
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
//
// ────────────────────────────────────────────────────────────────
// Code Cleanup: None (Library)
// ────────────────────────────────────────────────────────────────
//
// Resource Management:
//   - File handles: Closed immediately after reading (no defer needed by caller)
//   - TOML data: In-memory maps, garbage collected
//   - bereshitRoot: Package-level state, persists until process ends
//
// Graceful Shutdown:
//   - N/A: Stateless operations, no cleanup required
//   - Each Load* function reads and returns, no open resources
//
// ════════════════════════════════════════════════════════════════
// FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Library Overview & Integration Summary
// ────────────────────────────────────────────────────────────────
//
// Purpose: See METADATA "Purpose & Function" section above
//
// Quick summary:
//   - Config loader for Phase 0 TOML specifications
//   - Manifest-driven: reads index.toml to discover specs
//   - Tripwire fallback: uses hardcoded paths only if index.toml missing
//   - Foundation for Phase 3 Config Reader
//
// Public API:
//   Configuration: SetRoot
//   Generic:       LoadAll, LoadAllFromIndex, LoadIndex, LoadSystem, LoadSpec
//   Typed Access:  LoadMath, LoadTypes, LoadLanguage, LoadBible, LoadHealth,
//                  LoadPermission, LoadIdentity, LoadNetwork, LoadFilesystem
//   Deprecated:    LoadConstants (use LoadSpec or LoadMath)
//
// Architecture: LADDER - provides structure that Phase 3 builds upon
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify (Extension Points):
//   ✅ Add new Load* functions for new config directories
//   ✅ Add new path constants for new config locations
//   ✅ Enhance extractKeys for deeper TOML parsing
//   ✅ Add new fields to ConfigFile or LoadResult
//
// Modify with Extreme Care (Breaking Changes):
//   ⚠️ Public API function signatures - breaks demo and future consumers
//   ⚠️ ConfigFile/LoadResult struct fields - breaks accessing code
//   ⚠️ Path constants - affects what gets loaded
//
// NEVER Modify (Foundational Rails):
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Pattern: SetRoot before Load* calls
//   ❌ Return types: LoadResult for LoadAll, (*ConfigFile, error) for singles
//
// Validation After Modifications:
//   See "Code Validation" section above
//
// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" section above for complete architecture.
//
// Quick summary:
// - 6 public APIs orchestrate 2 core operations using 1 helper
// - Ladder: Load* → loadFile/loadDirectory → extractKeys
// - Baton: Caller → SetRoot → Load* → result
//
// ────────────────────────────────────────────────────────────────
// Surgical Update Points (Extension Guide)
// ────────────────────────────────────────────────────────────────
//
// See BODY "Core Operations" section for extension points:
// - New config location: Add constant in SETUP, function in Public APIs
// - New loader type: Add Load[Name] following existing pattern
// - Enhanced parsing: Extend extractKeys helper
//
// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// - File I/O: Each Load* reads from disk; cache results if called repeatedly
// - LoadAll: Loads all configs at once; more efficient than multiple Load* calls
// - Memory: All parsed TOML held in memory until result goes out of scope
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
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
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────
//
// Dependencies:
//   - github.com/BurntSushi/toml (TOML parsing)
//   - word/core/ configs (TOML spec files)
//
// Dependents:
//   - tov/demo/phase-0/demo-config/ (Phase 0 demo)
//   - Phase 3 Config Reader (future)
//
// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
//
// Planned Features:
//   ✓ Basic TOML loading - COMPLETED
//   ✓ Directory loading - COMPLETED
//   ✓ Key extraction - COMPLETED
//   ⏳ Phase 3: Config Reader integration
//   ⏳ Schema validation
//   ⏳ Type-safe config structs
//
// Known Limitations:
//   - No validation against schemas (future Phase 3)
//   - Top-level keys only (no nested key extraction)
//   - Requires SetRoot before any Load* calls
//
// Version History:
//   a-01.00 (2025-12-12) - Initial implementation
//         - Basic loader for Phase 0 demo
//         - Foundation for Phase 3 Config Reader
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This library is a LADDER component - providing the foundation that Phase 3
// Config Reader will build upon. It proves "specs exist and validate" for
// Phase 0, demonstrating that the TOML specifications are well-formed.
//
// Modify thoughtfully - changes here affect the Phase 0 demo and will impact
// Phase 3 Config Reader when it extends this foundation.
//
// "Thy word is a lamp unto my feet, and a light unto my path." - Psalm 119:105
//
// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
//
// Basic Setup:
//
//     config.SetRoot("/path/to/bereshit")
//     result := config.LoadAll()
//     if !result.Valid {
//         // Handle errors
//     }
//
// Typed access (thin wrappers around LoadSystem):
//
//     math, err := config.LoadMath()      // All math/*.toml
//     bible, err := config.LoadBible()    // All bible/*.toml
//     types, err := config.LoadTypes()    // All types/*.toml
//
// Generic manifest-driven loading:
//
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
//
// Access loaded data:
//
//     for _, cfg := range result.Configs["bible"] {
//         fmt.Printf("File: %s, Keys: %v\n", cfg.Name, cfg.Keys)
//     }

// ============================================================================
// END CLOSING
// ============================================================================
