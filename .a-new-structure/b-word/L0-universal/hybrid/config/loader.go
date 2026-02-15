//omni:code --go -library
//omni:key B-L0-config-loader
//omni:version b-01.00
//omni:pragma _pragma
//omni:metadata _metadata

// ============================================================================
// METADATA
// ============================================================================

// Package config loads and validates Kingdom Technology TOML specifications.
//
// Manifest-driven loader for Phase 0 TOML specs. Reads L0-universal/ladder/foundation/index.toml
// as single source of truth. Tripwire pattern for drift detection. Foundation
// for Phase 3 Config Reader.
//
// Root is the module root (b-word/), not the project root. All paths resolve
// relative to this: L0-universal/ladder/foundation/math/ternary.toml, etc.
//
// Usage:
//
//	config.SetRoot("/path/to/.a-new-structure/b-word")
//	result := config.LoadAll()
//	if !result.Valid {
//	    for _, err := range result.Errors { log.Println(err) }
//	}
//
// See _pragma and _metadata for OmniCode identity.
package config

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────
//
// Go requires all imports before any var/const/type/func declarations.
// Imports live in METADATA because pragma and metadata maps follow them,
// and those maps ARE the file's identity — they belong in METADATA.

//--- I.1 Standard Library [IMPORT] (1) ---
// Error handling, filesystem, path operations, concurrency
import (
	"errors"        // type assertions for structured errors (errors.As)
	"fmt"           // error formatting
	"path/filepath" // path construction
	"sort"          // consistent key ordering in extractKeys()
	"sync"          // RWMutex for cache concurrency safety
)

//--- I.2 External Packages [IMPORT] (-1) ---
// TOML parsing (Go stdlib lacks support)
import (
	"github.com/BurntSushi/toml" // TOML decoding
)

//--- I.3 Internal Packages [IMPORT] (0) ---
// L0 shared config utilities — extracted from duplicated code across loaders
// L0 identity registry — package self-awareness
import (
	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util"  // FileExists, LoadError, Must, RootGuard, TripwireBanner
	"creativeworkzstudio.com/bereshit/L0-universal/spiral/identity"     // Register, Lookup — package identity registry
)

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

// _pragma — OmniCode file identity (P1-P5). Runtime-queryable.
// Outline: //omni: directives above package declaration. Detail: this map.
var _pragma = map[string]string{
	// P1: Core Identity
	"P1.key":    "B-L0-config-loader",
	"P1.type":   "code",
	"P1.format": "go",
	"P1.style":  "library",
	// P2: Structure
	"P2.blocks": "4-block",
	// P3: Derivation
	"P3.from":    "b-word/seed/code/L0/go/library.go",
	"P3.derives": "B-L0-seed-go-library",
	// P4: Version
	"P4.at": "b-01.00",
	// P5: Summary
	"P5.title":   "Config Loader Library",
	"P5.summary": "Load and validate Kingdom Technology TOML specifications",
}

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

// _metadata — OmniCode file metadata (M1-M10). Runtime-queryable.
//
// M1-M5: Code-facing — what a Go developer needs to know.
// M6-M10: System-facing — what OmniCode needs for indexing and identity.
var _metadata = map[string]string{

	// ── M1: Core Identity ─────────────────────────────────────────
	"M1.key":            "B-L0-config-loader",
	"M1.component_type": "Ladder",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2025-12-12",

	// ── M2: Version History ───────────────────────────────────────
	"M2.b-01.00": "2026-02-13 — BASELINE: go.mod, util extraction, M1-M10, pragma maps",
	"M2.a-02.50": "2025-12-14 — PhD rigor + Bible accessibility refinement",
	"M2.a-02.00": "2025-12-14 — Tripwire, dependency validation, error types",
	"M2.a-01.00": "2025-12-12 — Initial creation, basic loading",

	// ── M3: Interface ─────────────────────────────────────────────
	"M3.requires.stdlib":   "errors, fmt, path/filepath, sort, sync",
	"M3.requires.external": "github.com/BurntSushi/toml v1.6.0",
	"M3.requires.internal": "L0-universal/hybrid/config/util, L0-universal/spiral/identity",
	"M3.used_by":           "tov/demo/phase-0/demo-config, generate-config, Phase 3 Reader",
	"M3.import":            "creativeworkzstudio.com/bereshit/L0-universal/hybrid/config",
	"M3.pattern":           "SetRoot(path) → LoadAll() → result.Configs[system]",

	// ── M4: Public API ────────────────────────────────────────────
	"M4.config": "SetRoot",
	"M4.load":   "LoadAll, LoadAllFromIndex, LoadIndex, LoadSystem, LoadSpec",
	"M4.check":  "DiscoverAndCompare, ValidateDependencyGraph, GetDependencyTree",
	"M4.access": "LoadMath, LoadTypes, LoadLanguage, LoadBible, LoadHealth, LoadPermission, LoadIdentity, LoadNetwork, LoadFilesystem",

	// ── M5: Operational ───────────────────────────────────────────
	"M5.blocking": "no — file I/O only, no network or long-running operations",
	"M5.health":   "Provider | granted: loaded | deferred: loading | denied: failed",

	// ── M6: Classification ────────────────────────────────────────
	// Discovery tags and categorization — grep-findable identity.
	"M6.tags":     "config, toml, loader, manifest, tripwire, drift-detection, phase-0",
	"M6.category": "Ladder",
	"M6.domain":   "config-loading",
	"M6.layer":    "L0-universal",
	"M6.paradigm": "CPI-SI",

	// ── M7: Intent ────────────────────────────────────────────────
	// Why this file exists, core design philosophy.
	"M7.purpose":    "Load and validate Kingdom Technology TOML specifications from index manifest",
	"M7.philosophy": "Single source of truth, tripwire drift detection, manifest-driven loading",
	"M7.provides":   "Spec loading, dependency graph validation, drift detection, typed system accessors",

	// ── M8: Grounding ─────────────────────────────────────────────
	// Biblical foundation — the anchor for this code.
	"M8.scripture":  "Psalm 119:105 — Thy word is a lamp unto my feet",
	"M8.principle":  "Specifications illuminate the path; structure guides implementation",
	"M8.anchor":     "Isaiah 28:10 — Precept upon precept, line upon line",

	// ── M9: Dependencies ──────────────────────────────────────────
	// Structural relationships — what this needs and what needs this.
	"M9.needs.stdlib":   "fmt, path/filepath, sort",
	"M9.needs.external": "github.com/BurntSushi/toml v1.6.0",
	"M9.needs.internal": "L0-universal/hybrid/config/util, L0-universal/spiral/identity",
	"M9.used_by":        "tov/demo/phase-0/demo-config, generate-config, Phase 3 Reader",
	"M9.layer_deps":     "L0 only — no L1+ dependencies",

	// ── M10: Roadmap ──────────────────────────────────────────────
	// Trajectory — where this is, where it's going, what it can't do yet.
	"M10.current":     "b-01.00 — BASELINE: go.mod, util extraction, M1-M10 metadata, pragma maps",
	"M10.planned":     "Phase 3 Config Reader, L1 pragma loader, hot-reload support",
	"M10.limitations": "File I/O only, no network, no caching, no watch/hot-reload",
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Standard SETUP: S.1 Types → S.2 Methods → S.3 Constants → S.4 Variables → S.5 State
// See: b-word/seed/code/L0/go/library.go > SETUP
//
// Top-level sections are skeleton (non-negotiable, always present).
// First-level subsections: standard slots (*.1-*.4), domain extensions (*.5+), reserved (*.R).

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────
//
// Standard: Core, Composed, Contracts, Errors
// Domain:   Manifest, Identity, Discovery
//
// Index:
//   T.1 Core       — ConfigFile (the loaded unit)
//   T.2 Composed   — LoadResult (batch aggregation)
//   T.3 Contracts  — Type aliases bridging util (LoadError, ValidationError)
//   T.4 Errors     — DependencyError (domain-specific)
//   T.5 Manifest   — IndexManifest, SystemEntry, SpecEntry (Phase 0 static)
//   T.6 Identity   — [Reserved: identity-driven loading]
//   T.7 Discovery  — DiscoveryResult, DependencyNode (tripwire)
//   T.R Reserved   — Future extensions

//--- T.1 Core [TYPE] (1) ---
// Primary type — a loaded configuration file with optional identity.

// ConfigFile represents a single loaded TOML specification file.
// Access parsed data via Data["section"]["key"].
//
// If the file contains [_pragma] and [_metadata] tables, those are
// extracted into structured identity fields. Files without pragma/metadata
// still load normally — Pragma and Metadata will be nil.
//
// Example:
//
//	cfg, _ := config.LoadSpec("math", "ternary.toml")
//	cfg.Data["trit"]["values"]  // nested access
//	cfg.Keys                     // ["arithmetic", "packing", "trit", ...]
//	cfg.Pragma["P1.key"]         // OmniCode identity (if present)
type ConfigFile struct {
	// Core fields — always populated
	Name string         // filename only, e.g., "ternary.toml"
	Path string         // full path for debugging/re-reading
	Data map[string]any // raw TOML: Data["section"]["key"]
	Keys []string       // top-level sections, sorted

	// Identity fields — populated when file has [_pragma]/[_metadata]
	Pragma   map[string]string // extracted [_pragma] table, nil if absent
	Metadata map[string]string // extracted [_metadata] table, nil if absent
}

//--- T.2 Composed [TYPE] (1) ---
// Aggregates built from core types.

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

//--- T.3 Contracts [TYPE] (0) ---
// Type aliases bridging to util package. Single source of truth in util;
// aliases here preserve backward compatibility for existing callers.

// LoadError wraps file-level loading failures.
type LoadError = util.LoadError

// ValidationError represents a spec validation failure.
type ValidationError = util.ValidationError

//--- T.4 Errors [TYPE] (-1) ---
// Domain-specific error types. Config-driven: originate in DATA, manifest in CODE.
// Each traces back to source spec. See: BODY > Error Helpers for constructors.

// DependencyError represents a dependency graph problem.
// Format: "math/ternary.toml -> types/missing.toml (missing): not in manifest"
type DependencyError struct {
	Spec    string // spec with the problem
	DepSpec string // problematic dependency (if applicable)
	Kind    string // "missing" or "cycle"
	Message string // details
}

//--- T.5 Manifest [TYPE] (1) ---
// Phase 0 static loading — types mapping directly to L0-universal/ladder/foundation/index.toml.
// These support manifest-driven loading (push model).
// See: L0-universal/ladder/foundation/index.toml for source of truth.

// IndexManifest represents the L0-universal/ladder/foundation/index.toml manifest file.
// Single source of truth for all specifications. Used by loader and generator.
type IndexManifest struct {
	Systems []SystemEntry `toml:"systems"` // 9 systems in dependency order (0-8)
}

// SystemEntry represents one of the 9 specification systems.
// Ordered by dependency (0 = foundation anchor, 8 = highest).
type SystemEntry struct {
	Name        string      `toml:"name"`        // e.g., "math", "types"
	Path        string      `toml:"path"`        // module-root-relative, e.g., "L2-platform/os/health"
	Order       int         `toml:"order"`       // 0 = anchor, higher = depends on lower
	Description string      `toml:"description"` // human-readable purpose
	DependsOn   []string    `toml:"depends_on"`  // system-level dependencies
	Specs       []SpecEntry `toml:"specs"`        // specs in this system
}

// SpecEntry represents a single TOML specification within a system.
type SpecEntry struct {
	File        string   `toml:"file"`        // e.g., "ternary.toml"
	Generates   bool     `toml:"generates"`   // true if generates C header
	Output      string   `toml:"output"`      // e.g., "ternary_math.gen.h"
	Description string   `toml:"description"` // what this spec defines
	DependsOn   []string `toml:"depends_on"`  // "system/file.toml" format
}

//--- T.6 Identity [TYPE] (0) ---
// [Reserved: Phase 3 — identity-driven loading types]
//
// When implemented, these types bridge the identity registry to config loading.
// The shift: push (manifest tells loader) → pull (loader discovers from identity).
//
// Planned types:
//   IdentitySource  — a file discovered by its pragma/metadata identity
//   LoadProfile     — loading instructions derived from identity (M6 layer, M9 deps)
//   IdentityResult  — LoadResult equivalent for identity-driven loading

//--- T.7 Discovery [TYPE] (1) ---
// Tripwire pattern: compare manifest to filesystem, detect drift.

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

//--- T.R Reserved [TYPE] (0) ---
// [Reserved: future type extensions]
// Candidates: QueryFilter (for identity-driven queries), CacheEntry (for loaded spec caching)

// ────────────────────────────────────────────────────────────────
// S.2 Type Methods
// ────────────────────────────────────────────────────────────────
//
// Standard: Core, Interface
//
// Index:
//   TM.1 Core      — [Reserved: Phase 3 typed accessors on ConfigFile]
//   TM.2 Interface — error interface (DependencyError)
//   TM.R Reserved  — Future methods

//--- TM.1 Core [METHOD] (0) ---
// [Reserved: Phase 3 — typed accessors, conversion methods on ConfigFile]
// Planned: cfg.GetString("section", "key"), cfg.GetInt(), cfg.HasPragma(), etc.

//--- TM.2 Interface [METHOD] (0) ---
// Methods satisfying Go interfaces.
// LoadError.Error() and .Unwrap() live in util (single source of truth).
// ValidationError.Error() lives in util (single source of truth).

// Error satisfies the error interface for DependencyError.
func (e *DependencyError) Error() string {
	if e.DepSpec != "" {
		return fmt.Sprintf("%s -> %s (%s): %s", e.Spec, e.DepSpec, e.Kind, e.Message)
	}
	return fmt.Sprintf("%s (%s): %s", e.Spec, e.Kind, e.Message)
}

//--- TM.R Reserved [METHOD] (0) ---
// [Reserved: future method extensions]
// Candidates: Stringer for ConfigFile, identity accessors, dependency walking

// ────────────────────────────────────────────────────────────────
// S.3 Constants
// ────────────────────────────────────────────────────────────────
//
// Standard: Configuration, Domain
//
// Index:
//   K.1 Configuration — IndexDir, IndexFile
//   K.2 Domain        — 9-system architecture reference
//   K.R Reserved      — Future constants

//--- K.1 Configuration [DATA] (1) ---
// Filesystem paths for the specification architecture.
// All paths relative to module root (b-word/).
//
// IndexDir: where the index.toml manifest lives.
// System paths span multiple layers (L0-L4) and are declared in the manifest
// as full module-root-relative paths (e.g., "L0-universal/ladder/foundation/math").
const (
	IndexDir  = "L0-universal/ladder/foundation" // where index.toml lives (relative to module root)
	IndexFile = "index.toml"              // manifest filename within IndexDir
)

//--- K.2 Domain [DATA] (1) ---
// 9-System Architecture (dependency order, 0=anchor):
//
//   0. math         — ternary foundation
//   1. types        — primitives, composition, validation
//   2. language     — keywords, syntax
//   3. bible        — scripture text, addressing, encoding
//   4. os/health    — scores, diagnostics, logging
//   5. os/permission— access control
//   6. identity     — model, contract
//   7. network      — message, timestamp, contract
//   8. filesystem   — types, contract
//
// Path constants are config-driven: index.toml provides paths via SystemEntry.Path.
// Tripwire crosscheck: Helpers > Watch Data > watchPaths.

//--- K.R Reserved [DATA] (0) ---
// [Reserved: future constants]
// Candidates: identity-driven discovery paths, pragma table names ("_pragma", "_metadata")

// ────────────────────────────────────────────────────────────────
// S.4 Variables
// ────────────────────────────────────────────────────────────────
//
// Standard: State, Sentinels
// Kept minimal — explicit SetRoot() over implicit.
//
// Index:
//   V.1 State     — bereshitRootGuard, loadedIndex
//   V.2 Sentinels — (removed — handled by RootGuard)
//   V.R Reserved  — Future state

//--- V.1 State [DATA] (3) ---
// Set once at startup, read by all Load* functions.
// Root guard — util.RootGuard manages "set once, check always" pattern.
// Caches — index and spec caches reduce disk I/O across 5 layers.
// All caches protected by cacheMu for concurrent access.
var (
	bereshitRootGuard = util.NewRootGuard("bereshit") // set once via SetRoot(), checked by every Load*
	cacheMu           sync.RWMutex                    // protects all caches below
	cachedIndex       *IndexManifest                  // parsed index.toml — survives until SetRoot
	specCache         = make(map[string]*ConfigFile)  // path → loaded spec — avoids re-parsing
)

//--- V.2 Sentinels [DATA] (0) ---
// errRootNotSet removed — handled by bereshitRootGuard.Check().
// Sentinel pattern replaced by RootGuard for cleaner error messages.

//--- V.R Reserved [DATA] (0) ---
// [Reserved: future state]
// Candidates: pragma index (all discovered pragmas), negative cache (known missing files)

// ────────────────────────────────────────────────────────────────
// S.5 Package State
// ────────────────────────────────────────────────────────────────
//
// Standard: Coordination, Initialization
// Cross-package conventions. Currently explicit (SetRoot);
// evolving toward identity-driven discovery.
//
// Index:
//   PS.1 Coordination    — How this package coordinates with others
//   PS.2 Initialization  — Init order, startup sequence
//   PS.R Reserved        — Future coordination

//--- PS.1 Coordination [DOC] (1) ---
// Two coordination modes:
//
//   Phase 0 (current): Explicit — caller sets root, calls Load*.
//     main() → config.SetRoot(path) → config.LoadAll() → use results
//
//   Phase 3 (planned): Identity-driven — loader discovers from registry.
//     init() registers identity → loader queries registry → auto-discovers configs

//--- PS.2 Initialization [DOC] (1) ---
// Current initialization sequence:
//   1. Package imported → init() registers identity with registry
//   2. SetRoot() called by main/demo with absolute path
//   3. LoadAll() parses index.toml and loads all specs
//   4. Individual Load* functions query loaded data

//--- PS.R Reserved [DOC] (0) ---
// [Reserved: Phase 3+ features]
//   - Auto-discovery from BERESHIT_ROOT environment variable
//   - Identity-driven loading (pull from registry instead of push from manifest)
//   - Cross-package health aggregation via identity.ByDomain()

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Standard BODY: B.0 Navigation → B.1 Helpers → B.2 Core → B.3 Errors → B.4 Identity → B.5 Public API
// See: b-word/seed/code/L0/go/library.go > BODY
//
// Top-level sections are skeleton (non-negotiable, always present).
// First-level subsections: standard slots (*.1-*.4), domain extensions (*.5+), reserved (*.R).

// ────────────────────────────────────────────────────────────────
// B.0 Navigation
// ────────────────────────────────────────────────────────────────
//
// The BODY knowing its own structure — same pattern as _pragma/_metadata.
// Flat map, dotted keys, greppable, runtime-queryable via Nav().
//
// Three views:  Ladder (architecture) · Baton (runtime) · APU (census)

// _nav — BODY navigation map. The Org Chart as queryable data.
//
// Architecture encoded as data, not decoration. Dashboard and health
// scoring can inspect internal structure, not just package identity.
//
// Key conventions:
//
//	B.N.field  — section-level (Ladder)
//	flow.field — execution paths (Baton)
//	apu.field  — census counts (APU)
var _nav = map[string]string{

	// ── Ladder: Section Architecture ─────────────────────────────
	//
	// B.* order = dependency order. Lower never imports higher.
	// Read bottom-up for foundations, top-down for usage.

	"B.1.name":    "Helpers",
	"B.1.purpose": "Foundation — declare good (pure), watch (guards), appointed (domain), known positions (data)",
	"B.1.depends": "",
	"B.1.count":   "10 functions + 1 map",

	"B.2.name":    "Core",
	"B.2.purpose": "Business logic — loading, discovery, dependency validation",
	"B.2.depends": "B.1, B.3",
	"B.2.count":   "7 functions",

	// Placed after B.2: reading core logic first gives context for why errors exist.
	"B.3.name":    "Errors",
	"B.3.purpose": "Error constructors — factories over SETUP types (T.3 Contracts, T.4 Errors)",
	"B.3.depends": "",
	"B.3.count":   "3 functions",

	// Architecturally independent of B.1-B.3. Uses METADATA (_pragma, _metadata).
	"B.4.name":    "Identity",
	"B.4.purpose": "Self-registration — init, Pragma, Metadata, Nav for OmniCode registry",
	"B.4.depends": "METADATA",
	"B.4.count":   "4 functions",

	"B.5.name":    "Public API",
	"B.5.purpose": "Exported interface — orchestrates B.2 Core for external callers",
	"B.5.depends": "B.1, B.2, B.3",
	"B.5.count":   "9 functions + 9 wrappers",

	// ── Baton: Execution Flow ────────────────────────────────────
	//
	// Three flows from Scripture — the theology and the operation are the same thing.
	//
	//   Genesis 1:   declare → execute → good → complete  (runtime)
	//   Nehemiah 4:  build → watch → adapt → resume       (startup)
	//   Habakkuk 3:  although... yet                       (resilience)
	//
	// "Write the vision, and make it plain on tablets,
	//  that he may run who reads it." — Habakkuk 2:2

	"flow.entry":   "SetRoot",
	"flow.exit":    "LoadResult",
	"flow.pattern": "Genesis 1 — declare, execute, good, complete. Tripwire on missing manifest.",

	// Primary: manifest-driven loading. The appointed time — ordered, sequential.
	// "For the vision is yet for the appointed time... it won't prove false." — Habakkuk 2:3
	"flow.primary.path": "LoadAll → LoadAllFromIndex → loadIndex → buildDependencyGraph → compareManifestToDisc → loadFile",
	"flow.primary.why":  "Appointed order: index.toml is single source of truth. Declare → execute → good → complete.",

	// Watch: when the manifest is missing, stand on known ground.
	// "I will stand at my watch, and set myself on the ramparts." — Habakkuk 2:1
	"flow.watch.path": "LoadAll → LoadSystem × 9 → loadDirectory → loadFile",
	"flow.watch.why":  "Known positions via watchPaths — stand on the ramparts when the manifest is absent.",

	// Yet: even when everything fails, identity persists.
	// "Although the fig tree doesn't flourish... yet I will rejoice." — Habakkuk 3:17-18
	"flow.yet": "Total failure → report honestly, maintain identity, point to foundation. The anchor holds.",

	// ── APU: Census ──────────────────────────────────────────────

	"apu.total":     "33 functions + 9 wrappers + 1 map",
	"apu.functions": "33",
	"apu.wrappers":  "9",
	"apu.maps":      "1",
}

// ────────────────────────────────────────────────────────────────
// B.1 Helpers
// ────────────────────────────────────────────────────────────────
//
// Foundation layer. Higher sections depend on these, never the reverse.
// This is the base pattern — how helpers are organized HERE becomes the
// standard for every L0 package.
//
// Four categories mapped to resolution vocabulary:
//
//   H.1 Pure Functions  — declare good (Genesis 1:10)
//       Data in, data out. Inspect and extract truth. No side effects.
//
//   H.2 Guard Functions — watch (Habakkuk 2:1)
//       Stand on the ramparts. Check preconditions before proceeding.
//
//   H.3 Domain Functions — appointed work (Habakkuk 2:3)
//       Package-specific I/O. The work at its appointed time.
//
//   H.4 Watch Data — known positions (Habakkuk 2:1)
//       When the manifest is absent, stand on known ground.
//
// The categories form a resolution chain:
//   H.1 validates → H.2 guards → H.3 executes → H.4 provides known ground
//   When H.4 fails: flow.yet — identity persists (B.4).
//
// Index:
//   H.1 Pure Functions    — extractKeys, collectNames, extractStringMap
//   H.2 Guard Functions   — checkRoot
//   H.3 Domain Functions  — fileExists, dirExists, trumpet, globTOML
//   H.4 Watch Data        — watchPaths + resolveWatchPath, watchSystems
//   H.R Reserved          — Future extensions

//--- H.1 Pure Functions [TRANSFORM] (3) ---
// "God saw that it was good." — Genesis 1:10
//
// No side effects. Data in, data out. Inspect and declare truth.
// The most stable helpers — trivially testable, no mocking,
// safe to call from anywhere. Every L0 package will have
// pure transforms here.

// extractKeys returns sorted top-level section names from a TOML map.
// Deterministic order matters — tests compare output, so sort first.
//
// Used by: B.2 > C.2 File Loading (populates ConfigFile.Keys)
func extractKeys(data map[string]any) []string {
	keys := make([]string, 0, len(data))
	for k := range data {
		keys = append(keys, k)
	}
	sort.Strings(keys)
	return keys
}

// collectNames extracts filenames from a ConfigFile slice.
// Order preserved from input — callers depend on manifest order.
//
// Used by: B.5 > A.2 Batch Loading (populates LoadResult.Summary)
func collectNames(configs []*ConfigFile) []string {
	names := make([]string, 0, len(configs))
	for _, c := range configs {
		names = append(names, c.Name)
	}
	return names
}

// extractStringMap extracts a flat string table from raw TOML data.
// Returns nil if the key is absent. Nested tables (e.g. [_pragma.P1_core])
// are expected TOML structure — only flat string fields become identity.
//
// This is the identity bridge: TOML files with [_pragma] and [_metadata]
// tables become self-aware when loaded.
//
// Used by: B.2 > C.2 File Loading (populates ConfigFile.Pragma, ConfigFile.Metadata)
func extractStringMap(data map[string]any, key string) map[string]string {
	raw, ok := data[key]
	if !ok {
		return nil
	}
	table, ok := raw.(map[string]any)
	if !ok {
		return nil
	}
	result := make(map[string]string, len(table))
	for k, v := range table {
		if s, ok := v.(string); ok {
			result[k] = s
		}
	}
	return result
}

//--- H.2 Guard Functions [CHECK] (1) ---
// "I will stand at my watch." — Habakkuk 2:1
//
// Precondition checks. Call at function entry to fail fast.
// The watchtower pattern: observe before proceeding.
// Every B.5 Public API function passes through a guard.
// Guard validates → core executes → API orchestrates.
//
// See: S.4 Variables > V.1 State (bereshitRootGuard)

// checkRoot validates the root has been set via SetRoot().
// Delegates to bereshitRootGuard.Check() — the guard owns the state,
// the helper just exposes it.
//
// Used by: B.5 Public API (all exported functions require root)
func checkRoot() error {
	return bereshitRootGuard.Check()
}

//--- H.3 Domain Functions [DOMAIN] (4) ---
// "For the vision is yet for the appointed time." — Habakkuk 2:3
//
// Package-specific helpers with side effects (filesystem I/O).
// The appointed work — not pure, these interact with the outside world.
// Other L0 packages would have different domain helpers here.
// For the config loader: find TOML files on disk, check existence.

// fileExists returns true if a file or directory exists at path.
// Thin delegation to util.FileExists — keeps B.2 calling B.1, not util directly.
//
// Used by: B.2 > C.1 Index Loading, C.2 File Loading
func fileExists(path string) bool {
	return util.FileExists(path)
}

// dirExists returns true if path exists AND is a directory.
// Thin delegation to util.DirExists.
//
// Used by: B.2 > C.3 Directory Loading (validates path before glob)
func dirExists(path string) bool {
	return util.DirExists(path)
}

// trumpet prints a loud warning when the system falls back to watch paths.
// Delegation to util.TripwireBanner — the trumpet that blows when the
// watchman sees the sword (Ezekiel 33:6, Nehemiah 4:20).
//
// Used by: B.5 > LoadAll (when index.toml missing, watch flow activates)
func trumpet(title string, lines []string) {
	util.TripwireBanner(title, lines)
}

// globTOML finds all .toml files in a directory.
//
// Used by: B.2 > C.3 Directory Loading, C.4 Discovery
func globTOML(dirPath string) ([]string, error) {
	pattern := filepath.Join(dirPath, "*.toml")
	matches, err := filepath.Glob(pattern)
	if err != nil {
		return nil, fmt.Errorf("no TOML files found at %s — check path exists and contains .toml files: %w", dirPath, err)
	}
	return matches, nil
}

//--- H.4 Watch Data [DATA] (1 map + 2 functions) ---
// "I will stand at my watch, and set myself on the ramparts." — Habakkuk 2:1
//
// Known positions when the manifest is absent. The watchtower has fixed
// coordinates — when index.toml is missing, these are the ramparts.
// This data should shrink as index.toml coverage grows.
//
// Tripwire: S.3 Constants > K.2 Domain references this section.

// watchPaths maps system names to known directory positions (relative to module root).
// Used ONLY when index.toml is missing — the watch flow (Habakkuk 2:1).
//
// Each system lives at its proper architectural layer:
//   L0-universal:  math, types, bible (foundations everything builds on)
//   L1-omnicode:   language (OmniCode syntax and keywords)
//   L2-platform:   health, permission, filesystem (OS-level services)
//   L3-cpisi:      identity (CPI-SI identity model)
//   L4-faithnet:   network (covenant messaging)
var watchPaths = map[string]string{
	// L0 — Universal Foundation
	"math":  "L0-universal/ladder/foundation/math",
	"types": "L0-universal/ladder/foundation/types",
	"bible": "L0-universal/ladder/foundation/bible",
	// L1 — OmniCode Language
	"language": "L1-omnicode/ladder/language",
	// L2 — Platform Services
	"health":     "L2-platform/os/health",
	"permission": "L2-platform/os/permission",
	"filesystem": "L2-platform/filesystem",
	// L3 — CPI-SI
	"identity": "L3-cpisi/identity",
	// L4 — FaithNet
	"network": "L4-faithnet/network",
}

// resolveWatchPath returns the full filesystem path for a known system.
// Returns ("", false) if the system is not in the watch map.
// Centralizes path resolution that was previously scattered in B.5.
//
// Used by: B.5 > LoadAll (watch flow), B.5 > LoadSystem, LoadConstants
func resolveWatchPath(root, system string) (string, bool) {
	rel, ok := watchPaths[system]
	if !ok {
		return "", false
	}
	return filepath.Join(root, rel), true
}

// watchSystems returns sorted system names from watchPaths.
// Deterministic iteration order for the watch flow.
//
// Used by: B.5 > LoadAll (watch flow iteration)
func watchSystems() []string {
	systems := make([]string, 0, len(watchPaths))
	for k := range watchPaths {
		systems = append(systems, k)
	}
	sort.Strings(systems)
	return systems
}

//--- H.R Reserved ---
// Future: extractConfig (typed config extraction), validateFormat (file format checks)

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
//
// Middle rung. Depends on Helpers below, used by the exported API above.
// See: Org Chart > Ladder Structure > Core Operations
//
// Subsections: Index Loading, File Loading, Directory Loading, Discovery, Dependencies

//--- C.1 Index Loading [LOAD] (1) ---
// Manifest operations. Entry point for manifest-driven loading.
// Uses: fileExists, S.3 > K.1 IndexDir, K.1 IndexFile

// loadIndex loads and parses the L0-universal/ladder/foundation/index.toml manifest.
// Cached after first successful parse — survives until SetRoot() invalidates.
//
// Returns:
//   - *IndexManifest: parsed manifest with all systems and specs
//   - error: file not found or TOML parse errors
func loadIndex() (*IndexManifest, error) {
	// Cache hit — return immediately without disk I/O.
	// RLock allows concurrent readers; only write-path takes full Lock.
	cacheMu.RLock()
	if cachedIndex != nil {
		idx := cachedIndex
		cacheMu.RUnlock()
		return idx, nil
	}
	cacheMu.RUnlock()

	// Cache miss — read from disk.
	indexPath := filepath.Join(bereshitRootGuard.Path(), IndexDir, IndexFile) // S.3 > K.1, K.1
	if !fileExists(indexPath) {                                               // H.3 Domain Functions
		return nil, newLoadError(IndexFile, "read", fmt.Errorf("not found: %s", indexPath))
	}

	var manifest IndexManifest
	if _, err := toml.DecodeFile(indexPath, &manifest); err != nil { // BurntSushi/toml
		return nil, newLoadError(IndexFile, "parse", err)
	}

	// Sort systems by order (0 = anchor, higher = depends on lower)
	sort.Slice(manifest.Systems, func(i, j int) bool {
		return manifest.Systems[i].Order < manifest.Systems[j].Order
	})

	// Store in cache — subsequent calls skip disk I/O.
	cacheMu.Lock()
	cachedIndex = &manifest
	cacheMu.Unlock()

	return &manifest, nil
}

//--- C.2 File Loading [LOAD] (1) ---
// Single file operations. Core workhorse — loads TOML and extracts identity.
// Uses: fileExists, H.1 extractKeys, H.1 extractStringMap

// loadFile loads and parses a single TOML file.
// If the file contains [_pragma] and [_metadata] tables, those are
// extracted into the identity fields — the loaded file becomes self-aware.
// Cached after first successful parse — same path returns same *ConfigFile.
//
// Parameters:
//   - path: full filesystem path to the TOML file
//
// Returns:
//   - *ConfigFile: parsed config with identity (if present)
//   - error: file not found or TOML parse errors
func loadFile(path string) (*ConfigFile, error) {
	// Cache hit — return immediately without disk I/O.
	cacheMu.RLock()
	if cfg, ok := specCache[path]; ok {
		cacheMu.RUnlock()
		return cfg, nil
	}
	cacheMu.RUnlock()

	// Cache miss — read from disk.
	if !fileExists(path) { // H.3 Domain Functions
		return nil, newLoadError(filepath.Base(path), "read", fmt.Errorf("not found: %s", path))
	}

	var data map[string]any
	if _, err := toml.DecodeFile(path, &data); err != nil {
		return nil, newLoadError(filepath.Base(path), "parse", err)
	}

	cfg := &ConfigFile{
		Name:     filepath.Base(path),
		Path:     path,
		Data:     data,
		Keys:     extractKeys(data),                   // H.1 Pure Functions
		Pragma:   extractStringMap(data, "_pragma"),    // H.1 — identity bridge
		Metadata: extractStringMap(data, "_metadata"),  // H.1 — identity bridge
	}

	// Store in cache — subsequent calls for same path skip disk I/O.
	cacheMu.Lock()
	specCache[path] = cfg
	cacheMu.Unlock()

	return cfg, nil
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
	if !dirExists(dirPath) { // H.3 Domain Functions — guard before glob
		return nil, newLoadError(filepath.Base(dirPath), "read",
			fmt.Errorf("directory not found: %s", dirPath))
	}

	matches, err := globTOML(dirPath) // H.3 Domain Functions
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
// Uses: Helpers > Pure Functions > globTOML, Helpers > Watch Data > watchPaths

// discoverFiles finds all .toml files in a system directory.
//
// Parameters:
//   - systemPath: full path to system directory (e.g., b-word/L0-universal/ladder/foundation/math)
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
		systemPath := filepath.Join(bereshitRootGuard.Path(), system.Path)

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
// Factory functions for error types. Used by core operations and the exported API.

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
	return util.NewLoadError(file, op, err)
}

// newValidationError creates a ValidationError with location context.
// THROUGHLINE: Delegates to util.NewValidationError.
// Note: L0's "Spec" parameter maps to util's "Path" field.
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
	return util.NewValidationError(spec, section, field, message)
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
// Subsections: Identity, Configuration, Batch Loading, Index Access, Discovery, Generic Loading, Typed Wrappers

//--- Identity — Package Self-Registration ---

// init registers this package in the OmniCode identity registry.
// Runs automatically when any code imports this package.
func init() {
	identity.Register(_pragma["P1.key"], _pragma, _metadata)
}

// Pragma returns this package's OmniCode pragma identity (P1-P5).
func Pragma() map[string]string { return _pragma }

// Metadata returns this package's OmniCode metadata (M1-M10).
func Metadata() map[string]string { return _metadata }

// Nav returns this package's BODY navigation map (Ladder, Baton, APU).
// The org chart as queryable data — same map used internally for
// architecture documentation, now exposed for assessment tools.
//
// Use: dashboard health checks, structural assessment, CI validation.
func Nav() map[string]string { return _nav }

//--- Configuration ---
// Package state initialization. Must be called before Load* functions.

// SetRoot sets the module root directory path and invalidates all caches.
// New root means all cached filesystem paths are stale — clear everything.
//
// Parameters:
//   - path: absolute path to the module root (b-word/)
//
// Example:
//
//	config.SetRoot("/path/to/.a-new-structure/b-word")
//	result := config.LoadAll()
func SetRoot(path string) {
	// Invalidate caches before setting new root.
	// Any paths cached under the old root are now stale.
	cacheMu.Lock()
	cachedIndex = nil
	specCache = make(map[string]*ConfigFile)
	cacheMu.Unlock()

	bereshitRootGuard.Set(path) // THROUGHLINE: delegates to RootGuard
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

	if bereshitRootGuard.Path() == "" { // guard: require SetRoot() first
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

	// Check if it's specifically an index.toml issue — type assertion, not string matching.
	for _, err := range indexResult.Errors {
		var loadErr *LoadError
		if errors.As(err, &loadErr) && loadErr.File == IndexFile && loadErr.Op == "read" {
			// Watch flow activated — manifest absent, falling back to known positions.
			trumpet("L0-universal/ladder/foundation/index.toml NOT FOUND", []string{
				"Watch flow active — loading from known positions, not manifest.",
				"Create index.toml to use the appointed order (single source of truth).",
			})
			break
		}
	}

	// Watch flow: load from known positions (Habakkuk 2:1).
	// Deterministic order via watchSystems(). Path resolution via resolveWatchPath().
	for _, system := range watchSystems() {
		dirPath, _ := resolveWatchPath(bereshitRootGuard.Path(), system)
		configs, err := loadDirectory(dirPath)
		if err != nil {
			result.Errors = append(result.Errors, fmt.Errorf("%s: %w", system, err))
			result.Valid = false
			continue
		}
		result.Configs[system] = configs
		result.Summary[system] = collectNames(configs)
	}

	return result
}

// LoadAllFromIndex loads all configs using the index.toml manifest.
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

	if bereshitRootGuard.Path() == "" {
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
		// Trumpet: manifest promises files that don't exist on disk.
		lines := make([]string, 0, len(discovery.Missing))
		for _, missing := range discovery.Missing {
			lines = append(lines, fmt.Sprintf("MISSING: %s (in manifest, not on disk)", missing))
			result.Errors = append(result.Errors, newLoadError(missing, "read",
				fmt.Errorf("declared in manifest but not on disk")))
		}
		trumpet("Manifest/Disk Mismatch", lines)
		result.Valid = false
	}
	if len(discovery.Unexpected) > 0 {
		// Trumpet: files on disk that the manifest doesn't know about.
		lines := make([]string, 0, len(discovery.Unexpected)+1)
		for _, unexpected := range discovery.Unexpected {
			lines = append(lines, fmt.Sprintf("UNEXPECTED: %s", unexpected))
		}
		lines = append(lines, "Consider adding these to L0-universal/ladder/foundation/index.toml")
		trumpet("Unexpected files (on disk, not in manifest)", lines)
		// Unexpected files are warnings, not errors - don't set Valid = false
	}

	// ═══════════════════════════════════════════════════════════════════════
	// STEP 4: Load all specs in dependency order
	// ═══════════════════════════════════════════════════════════════════════
	for _, system := range manifest.Systems {
		systemPath := filepath.Join(bereshitRootGuard.Path(), system.Path)
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

// LoadIndex loads and returns the index.toml manifest.
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
// Manifest-driven loading. Falls back to Helpers > Watch Data when index.toml missing.

// LoadSystem loads all specs in a system by name.
// Uses index.toml manifest; watch flow via resolveWatchPath if manifest absent.
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
				systemPath := filepath.Join(bereshitRootGuard.Path(), sys.Path)
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
		return nil, newLoadError(system, "lookup",
			fmt.Errorf("system not found in index.toml"))
	}

	// Watch flow — manifest absent, use known positions.
	trumpet(fmt.Sprintf("LoadSystem(%q) — index.toml not found", system), []string{
		"Loading from watch path (known position).",
	})

	dirPath, ok := resolveWatchPath(bereshitRootGuard.Path(), system)
	if !ok {
		return nil, newLoadError(system, "lookup",
			fmt.Errorf("not found in manifest and no watch path defined"))
	}
	return loadDirectory(dirPath)
}

// LoadSpec loads a single spec from a system.
// Uses index.toml manifest; watch flow via resolveWatchPath if manifest absent.
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
				systemPath := filepath.Join(bereshitRootGuard.Path(), sys.Path)
				for _, s := range sys.Specs {
					if s.File == spec {
						specPath := filepath.Join(systemPath, s.File)
						return loadFile(specPath)
					}
				}
				return nil, newLoadError(system+"/"+spec, "lookup",
					fmt.Errorf("spec not found in system"))
			}
		}
		return nil, newLoadError(system, "lookup",
			fmt.Errorf("system not found in index.toml"))
	}

	// Watch flow — manifest absent, use known positions.
	trumpet(fmt.Sprintf("LoadSpec(%q, %q) — index.toml not found", system, spec), []string{
		"Loading from watch path (known position).",
	})

	dirPath, ok := resolveWatchPath(bereshitRootGuard.Path(), system)
	if !ok {
		return nil, newLoadError(system+"/"+spec, "lookup",
			fmt.Errorf("not found in manifest and no watch path defined"))
	}
	specPath := filepath.Join(dirPath, spec)
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
// See: b-word/seed/code/L0/go/library.go > CLOSING

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
//   - LoadAll must find and parse all TOML files in L0-universal/ladder/foundation/
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
//   import "creativeworkzstudio.com/bereshit/L0-universal/hybrid/config"

//--- Usage Example ---
//
//     package main
//
//     import "creativeworkzstudio.com/bereshit/L0-universal/hybrid/config"
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
//   - bereshitRootGuard.Path(): Package-level state, persists until process ends

//--- Graceful Shutdown ---
//   - Caches (cachedIndex, specCache) persist until SetRoot() or process ends
//   - No open file handles — toml.DecodeFile opens and closes internally
//   - For test cleanup: SetRoot("") invalidates all caches

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
//   Total: 33 functions + 9 wrappers + 1 map
//   - Helpers (B.1):         10 functions + 1 map
//       H.1 Pure:    extractKeys, collectNames, extractStringMap
//       H.2 Guard:   checkRoot
//       H.3 Domain:  fileExists, dirExists, trumpet, globTOML
//       H.4 Watch:   resolveWatchPath, watchSystems + watchPaths (map)
//   - Core Operations (B.2): 7 (index, file, directory, discovery, dependencies)
//   - Error Handling (B.3):  3 constructors (LoadError, ValidationError, DependencyError)
//   - Identity (B.4):        4 (init, Pragma, Metadata, Nav)
//   - Public APIs (B.5):     9 functions + 9 typed wrappers

//--- Ladder ---
//   Public APIs → Core Operations → Helpers

//--- Baton ---
//   Primary (appointed): SetRoot → LoadAll → LoadAllFromIndex → loadIndex → loadFile
//   Watch (ramparts):    SetRoot → LoadAll → LoadSystem × 9 → loadDirectory → loadFile
//   Yet (anchor holds):  Total failure → report honestly → identity persists (B.4)

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
//   - Index cache: loadIndex() caches parsed manifest after first call — O(1) on repeat
//   - Spec cache: loadFile() caches parsed specs by path — no re-parsing same file
//   - Cache invalidation: SetRoot() clears both caches (new root = stale paths)
//   - Concurrency: sync.RWMutex allows concurrent readers, serializes writes
//   - Memory: All parsed TOML held in cache until SetRoot() or process ends

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Common problems and solutions. Points to BODY function docstrings.

//--- Common Problems ---
// Problem: "file not found" errors
//   - Check: Did you call SetRoot with valid bereshit path?
//   - Check: Does L0-universal/ladder/foundation/ directory exist at that path?
//
// Problem: Empty configs returned
//   - Check: Are TOML files present in expected directories?
//   - Check: Is bereshitRootGuard.Path() set before calling Load* functions?
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
//   - github.com/BurntSushi/toml v1.6.0 (TOML parsing)
//   - L0-universal/hybrid/config/util (shared primitives)
//   - L0-universal/ladder/foundation/ configs (TOML spec files)

//--- Dependents ---
//   - tov/demo/phase-0/demo-config/ (Phase 0 demo)
//   - L1-omnicode/hybrid/pragma/ (pragma reader)
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
