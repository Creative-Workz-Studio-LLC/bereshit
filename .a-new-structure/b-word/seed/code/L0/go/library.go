//go:build ignore

// #!omni template --go -library
// #!omni meta.key = B-seed-code-L0-go-library
// #!omni meta.from = b-word/seed/code/L0/go/root.omni
// #!omni meta.at = template
// ============================================================================
// METADATA
// ============================================================================
//
// TEMPLATE: Go Library Package — 4-Block Structure
//
// Usage:
//   1. cp library.go /path/to/new/package/mypackage.go
//   2. Change pragma: #!omni template → //omni:code --go -library
//   3. Update meta.key, meta.from, meta.at for your package
//   4. Update Pragma and Metadata values to describe your package
//   5. Remove "//go:build ignore" line
//   6. Rename package to match directory name
//
// derives_from: b-word/seed/code/L0/go/root.omni

// Package library is the root 4-block template for Go library packages.
//
// All Go libraries in the Bereshit project derive from this template.
//
// # Design
//
// Pure stdlib — no external dependencies. Identity and context carried
// as package-level data via [Pragma] (I1-I4) and [Metadata] (C1-C7).
//
// # Usage
//
//	cp library.go /path/to/new/package/mypackage.go
//	# Change pragma: #!omni template → //omni:code --go -library
//	# Update Pragma and Metadata values for your package
//
// # OmniCode Identity
//
//	Variable    Sections  Purpose                          Query
//	Pragma      I1-I4     Identity — who this file is      PragmaGet
//	Metadata    C1-C7     Context — when, where, why, how  MetadataGet
package library

// ────────────────────────────────────────────────────────────────────────────────────
// Metadata Imports
// ────────────────────────────────────────────────────────────────────────────────────
//
// Imports required by the METADATA block (Pragma/Metadata vars and accessors).
// Kept separate from SETUP imports so METADATA is self-contained.
// See SETUP > Imports for the full dependency set.

import (
	// [Reserved: [][2]string is built-in — no imports needed for metadata vars]
	// Add imports here if PragmaGet/MetadataGet accessors move to METADATA.
)

// ────────────────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ────────────────────────────────────────────────────────────────────────────────────

// Pragma carries the OmniCode identity sections (I1-I4) for this package.
//
// These fields establish WHO this file is in the project graph.
// Initialized once at package load, read-only thereafter.
//
// # Sections
//
//	Section  Purpose       Required                 Defined
//	I1       Core          key, format, from, at    —
//	I2       Family        type, structure          subtype, role
//	I3       Instance      file, title              component, path, provides, brief
//	I4       Architecture  —                        layer, position, pattern
//
// # Example
//
//	key := PragmaGet("I1.key") // returns "B-seed-code-L0-go-library"
var Pragma = [][2]string{
	// I1: Core
	{"I1.key", "B-seed-code-L0-go-library"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/root.omni"},
	{"I1.at", "template"},
	// I2: Family
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "seed"},
	// I3: Instance
	{"I3.file", "library.go"},
	{"I3.title", "Go Library Package Template"},
	{"I3.component", "Root template for Go library packages"},
	{"I3.path", "seed/code/L0/go/library.go"},
	{"I3.provides", "GO_LIBRARY_TEMPLATE"},
	{"I3.brief", "Root 4-block template for Go library packages. All Go libraries derive from this."},
	// I4: Architecture
	{"I4.layer", "L0"},
	{"I4.position", "seed"},
	{"I4.pattern", "cp library.go pkg/mypackage/mypackage.go"},
}

// ────────────────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ────────────────────────────────────────────────────────────────────────────────────

// Metadata carries the OmniCode context sections (C1-C7) for this package.
//
// These fields establish WHEN, WHERE, WHY, and HOW for this file.
// Initialized once at package load, read-only thereafter.
//
// # Sections
//
//	Section  Purpose         Required             Defined
//	C1       State           version, status      created, updated
//	C2       Attribution     organization         architect, implementation, copyright
//	C3       Grounding       scripture            principle, anchor
//	C4       Dependencies    requires, consumers  integration, if_missing
//	C5       Intent          —                    purpose, philosophy
//	C6       Roadmap         —                    current, planned, limitations
//	C7       Classification  —                    tags, category, domain, paradigm
//
// # Example
//
//	version := MetadataGet("C1.version") // returns "a-02.00"
var Metadata = [][2]string{
	// C1: State
	{"C1.version", "a-04.00"},
	{"C1.status", "Active"},
	{"C1.created", "2026-02-17"},
	{"C1.updated", "2026-02-18"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Nova Dawn"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "CreativeWorkzStudio LLC"},
	// C3: Grounding
	{"C3.scripture", "Exodus 25:40 — Look that thou make them after their pattern"},
	{"C3.principle", "Templates establish the pattern; implementations follow faithfully"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "none"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "none"},
	{"C4.consumers", "All Go library packages in the project"},
	{"C4.integration", "cp library.go /path/to/package/mypackage.go"},
	{"C4.if_missing", "Go libraries lack standardized I/C metadata structure"},
	// C5: Intent
	{"C5.purpose", "Canonical 4-block structure for Go library packages with I/C metadata"},
	{"C5.philosophy", "Structure is skeleton; content is flesh — boundaries before content"},
	// C6: Roadmap
	{"C6.current", "a-04.00 — CLOSING block aligned with standard zone model"},
	{"C6.planned", "Go 4-block linter, schema-driven validation"},
	// C7: Classification
	{"C7.tags", "template, go, library, 4-block, seed, omnicode"},
	{"C7.category", "Foundation"},
	{"C7.domain", "bereshit"},
	{"C7.paradigm", "CPI-SI"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// SETUP makes things EXIST. BODY makes things HAPPEN.
//
// Everything the BODY needs — types, constants, imports, error definitions —
// is declared here. The BODY contains only functions that operate on what
// SETUP established.
//
// If anything must be hardcoded, it lives here — never scattered through BODY.
// When a hardcoded value gets promoted to config, you extract from one place.
//
// Section order (dependency chain — each layer uses only what's above):
//
//   1. Imports           — What this file depends on
//   2. Constants         — Compile-time fixed values
//   3. Variables         — Package-level mutable state
//   4. Type Aliases      — Shorthand for complex signatures
//   5. Error Types       — Custom errors + Error() method
//   6. Core Types        — struct definitions + simple constructors
//   7. Interface Defs    — Behavioral contracts (shape, not fulfillment)
//   8. Type Methods      — Structural behaviors (completing interface impls)
//   9. Code Generation   — go:generate directives
//  10. Build Tags        — Conditional compilation items

// ────────────────────────────────────────────────────────────────────────────────────
// 1. Imports
// ────────────────────────────────────────────────────────────────────────────────────
//
// Order: Standard Library → External Packages → Internal Packages → Package-Internal
// Group by origin with blank-line separation. Each group alphabetical.
// Wildcard imports (import . "pkg") are discouraged — be explicit.

// --- Standard Library ---
// import (
// 	"fmt"           // Error formatting
// 	"os"            // File reading, environment
// 	"path/filepath" // Path construction
// )

// --- External Packages ---
// [Pure library — no external dependencies at L0]

// --- Internal Packages ---
// [Self-contained at L0 — no internal dependencies]

// --- Package-Internal ---
// [Root package — no sub-imports at this level]

// ────────────────────────────────────────────────────────────────────────────────────
// 2. Constants
// ────────────────────────────────────────────────────────────────────────────────────
//
// Compile-time fixed values. Don't depend on types — they come first.
// Prefer named constants over raw literals in BODY.

// const MaxRetries = 3
// const DefaultTimeoutSecs = 30

// ────────────────────────────────────────────────────────────────────────────────────
// 3. Variables
// ────────────────────────────────────────────────────────────────────────────────────
//
// Runtime-initialized mutable state. Lazy singletons, registries.
// NOTE: PRAGMA and METADATA identity variables live in the METADATA block.
//
// Keep minimal — explicit SetX() over implicit mutation.

// var (
// 	[packageName]Root string  // absolute path, set via SetRoot()
// 	loadedConfig      *Config // cached config after first load
// )

// --- Sentinel Errors ---
// var (
// 	err[Name] = fmt.Errorf("[error message]")
// )

// ────────────────────────────────────────────────────────────────────────────────────
// 4. Type Aliases
// ────────────────────────────────────────────────────────────────────────────────────
//
// Shorthand for complex types. Improves readability of function signatures.
// Don't overuse — aliases hide information. Use when the full type is unwieldy.

// type Result = error
// type FieldMap = map[string][]string

// ────────────────────────────────────────────────────────────────────────────────────
// 5. Error Types
// ────────────────────────────────────────────────────────────────────────────────────
//
// Errors are part of the API contract — they define how failure looks.
// Place BEFORE core types because core type methods return these errors.
//
// Pattern: struct for context → Error() for messages → Unwrap() for chaining.
// Consider sentinel errors (var Err... = errors.New(...)) for simple cases.

// // [ErrorType] represents [error condition from operation].
// type [ErrorType] struct {
// 	Op      string // operation that failed
// 	Path    string // file/resource involved
// 	Wrapped error  // underlying cause
// }
//
// func (e *[ErrorType]) Error() string { return fmt.Sprintf("[op] %s: %v", e.Path, e.Wrapped) }
// func (e *[ErrorType]) Unwrap() error { return e.Wrapped }

// ────────────────────────────────────────────────────────────────────────────────────
// 6. Core Types
// ────────────────────────────────────────────────────────────────────────────────────
//
// Data structures for this library's domain. The shapes of what this
// library works with — not the operations on them (those go in BODY).
//
// Simple constructors (New*()) belong here WITH the type.
// Complex methods and logic belong in BODY.
//
// Patterns:
//   Builder     — Config struct → validated output type
//   Newtype     — type Wrapper struct{ inner Inner } for type-safe domain values
//   Functional  — type Option func(*Config) for flexible configuration

// // [TypeName] represents [what this models].
// //
// // [2-4 sentences: what it represents, lifecycle, key constraints.]
// //
// // Example:
// //
// //	item, err := Load[Type]("[path]")
// //	item.[Field]  // [what this gives you]
// type [TypeName] struct {
// 	Name string         // identifier, e.g., "ternary.toml"
// 	Path string         // full path for debugging
// 	Data map[string]any // parsed content
// }

// // [ComposedType] holds the result of [operation].
// //
// // Created by [BatchFunction](). Query methods only
// // available on this type (typestate enforcement).
// type [ComposedType] struct {
// 	Items  [][ItemType] // successfully loaded items
// 	Valid  bool         // true only if ALL succeeded
// 	Errors []error      // all errors encountered
// }

// ────────────────────────────────────────────────────────────────────────────────────
// 7. Interface Definitions
// ────────────────────────────────────────────────────────────────────────────────────
//
// Behavioral contracts — the SHAPE of what something does, not the doing.
// Interface definitions (the contract) belong here.
// Interface implementations belong in BODY.
//
// Exception: completing impls (Error(), String(), Unwrap()) stay with
// their type in sections 5-6 — they complete the type's existence.

// // [InterfaceName] defines the contract for [behavior].
// //
// // Implementors must provide [key capability].
// type [InterfaceName] interface {
// 	// [Method description.]
// 	[Method]([params]) [ReturnType]
// }

// ────────────────────────────────────────────────────────────────────────────────────
// 8. Type Methods
// ────────────────────────────────────────────────────────────────────────────────────
//
// Structural behaviors for types defined above. These are NOT business
// logic — those go in BODY. Type methods here are:
//   - Interface implementations (Error(), String(), etc.)
//   - Conversion methods (ToX(), FromX())
//   - Accessor/mutator patterns if needed
//
// Key distinction:
//   - SETUP type methods: Structural (formatting, conversion, interface impl)
//   - BODY methods: Business logic (Process(), Validate(), Execute())

// // String implements fmt.Stringer for [TypeName].
// func (t *[TypeName]) String() string {
// 	return fmt.Sprintf("[format]", t.[Field])
// }

// ────────────────────────────────────────────────────────────────────────────────────
// 9. Code Generation
// ────────────────────────────────────────────────────────────────────────────────────
//
// go:generate directives for code generation tools.
// Macros don't exist in Go — code generation is the equivalent.
// Macro invocations may generate types (SETUP) or functions (BODY).

// //go:generate stringer -type=[TypeName]

// ────────────────────────────────────────────────────────────────────────────────────
// 10. Build Tags
// ────────────────────────────────────────────────────────────────────────────────────
//
// Conditional compilation items. Build-tagged types, imports, or constants
// that only exist under certain build configurations.
//
// Individual //go:build on separate files is Go convention.
// This section is for documenting build-tag decisions and tag-specific code.

// //go:build !cgo
// // Pure-Go fallback implementations for CGO-dependent features.

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
// BODY Sections Overview (Library: 5 sections)
// -----------------------------------------------------------------------------
//
// 1. Org Chart — Map dependencies and execution flow within this component
// 2. Helpers — Foundation functions: simple, focused, reusable utilities
// 3. Core Operations — Component-specific business logic
// 4. Error Handling — Centralized error management and recovery strategies
// 5. Public APIs — Top-level orchestration: simple functions calling proven pieces
//
// Section order: 1 → 2 → 3 → 4 → 5 (ascending numeric — handler validates order only)
// Flow: understand structure → build foundations → implement logic → handle errors → expose
//
// Format: // N. Name (each subsection uses 74-char ─ separators)
// The handler checks ascending numeric order, not canonical names.

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
// Maps bidirectional dependencies and baton flow within this component.
// Provides navigation for both development (what's available to use) and
// maintenance (what depends on this function).
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-001-organizational-chart.md
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - Orchestration)
//   ├── [PublicFunction1]() → uses [helper1](), [coreOp1]()
//   └── [PublicFunction2]() → uses [helper2](), [coreOp2]()
//
//   Core Operations (Middle Rungs - Business Logic)
//   ├── [coreOp1]() → uses [helper1](), [helper3]()
//   └── [coreOp2]() → uses [helper2]()
//
//   Helpers (Bottom Rungs - Foundations)
//   ├── [helper1]() → pure function
//   ├── [helper2]() → pure function
//   └── [helper3]() → pure function
//
// Baton Flow (Execution Paths):
//
//   Entry → [PublicFunction1]()
//     ↓
//   [helper1]() → [coreOp1]()
//     ↓
//   [helper3]()
//     ↓
//   Exit → return result
//
// Module Dependencies (Orchestrator Pattern):
// For multi-file packages, document which modules this file calls.
//   [thisfile.go] (orchestrator) → [module1.go] ([purpose])
//                                → [module2.go] ([purpose])
//
// APUs (Available Processing Units):
// - [X] functions total
// - [X] helpers (pure foundations)
// - [X] core operations (business logic)
// - [X] public APIs (exported interface)

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// Foundation functions used throughout this component. Bottom rungs of
// the ladder - simple, focused, reusable utilities. Usually not exported.
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-002-helpers.md
//
// Subsections: Pure Functions, Guard Functions, Domain Functions, Fallback Data

//--- H.1 Pure Functions [TRANSFORM] (1) ---
// No side effects. Same input → same output. Safe to call from anywhere.
//
// Common patterns:
//   - fileExists()  - check path exists (wraps os.Stat, used by load functions)
//   - globTOML()    - find all .toml files in directory (wraps filepath.Glob)
//   - extract[X]()  - extract data from collections (sort if from map)
//   - find[X]()     - lookup item by criteria, return pointer or nil
//   - collect[X]()  - gather items from slice into new slice
//   - to[X]()       - convert between types
//
// // extract[Thing] returns [what] from [source].
// //
// // Parameters:
// //   - [param]: [purpose]
// //
// // Returns:
// //   - [type]: [what's returned]
// //
// // Uses: [SETUP > Constants > ... if referencing SETUP definitions]
// // Used by: [Core Operations > [section] or Public APIs > [function]]
// func extract[Thing]([params]) []string {
// 	result := make([]string, 0, len([source]))
// 	for [k/v] := range [source] {
// 		result = append(result, [value])
// 	}
// 	sort.Strings(result) // deterministic order (required for maps)
// 	return result
// }
//
// // find[Thing] locates [what] by [criteria].
// //
// // Parameters:
// //   - [source]: collection to search
// //   - [criteria]: value to match
// //
// // Returns:
// //   - *[Type]: pointer to found item, nil if not found
// //
// // Used by: [Core Operations > [section]]
// func find[Thing]([source], [criteria]) *[Type] {
// 	for i := range [source] {
// 		if [source][i].[Field] == [criteria] {
// 			return &[source][i]
// 		}
// 	}
// 	return nil
// }

//--- H.2 Guard Functions [CHECK] (1) ---
// Precondition checks. Call at function entry to fail fast.
// See: SETUP > Variables > Sentinel Errors (errors returned by guards)
// [Reserved (0): if guards live in companion file, See: that file > Helpers > Guard Functions]
//
// Common pattern: validate package-level state set by public API (e.g., SetRoot).
//
// // check[State] validates [stateVar] has been set via Set[State]().
// //
// // Returns:
// //   - error: err[State]NotSet if [stateVar] is empty, nil otherwise
// //
// // Used by: Public APIs (all public functions require [state])
// func check[State]() error {
// 	if [stateVar] == "" {
// 		return err[State]NotSet // see: SETUP > Variables > Sentinel Errors
// 	}
// 	return nil
// }

//--- H.3 Domain Functions [DOMAIN] (1) ---
// Package-specific helpers. Manifest operations, converters, extractors.
// See: Helpers > Fallback Data > [dataSource] (data these functions operate on)
// [Reserved (0): if domain logic lives in companion file, See: that file > Helpers > Domain Functions]
//
// Common patterns: extract from manifest, get by key, build paths.
//
// // [functionName] [does what] from/for [dataSource].
// //
// // Parameters:
// //   - [param]: [description] (e.g., "[example]")
// //
// // Returns:
// //   - [type]: [description] (e.g., ["example1", "example2", ...])
// //   - bool: true if [condition], false otherwise
// //
// // Uses: Helpers > Fallback Data > [dataSource]
// // Used by: [Section] > [Subsection] > [caller]  (if wired)
// // Reserved for: [Future purpose]                 (if not yet wired)
// func [functionName]([params]) [returns] {
// 	// implementation
// 	sort.Strings(result) // deterministic order (if iterating map)
// 	return result
// }

//--- H.4 Fallback Data [DATA] (1) ---
// Static data for tripwire fallback when config-driven source unavailable.
// Tripwire: K.2 references this section. If K.2 Tripwire breaks, check here.
// [Reserved (0): if fallback data lives in companion file, See: that file > Helpers > Fallback Data]
//
// Pattern A: Emergency fallback (used when config loading fails)
// // fallback[Name] provides hardcoded [what] when [source] unavailable.
// // Used ONLY when [config file] is missing (tripwire fallback).
// var fallback[Name] = [type]{...}
//
// Pattern B: Primary data (will become config-driven in future)
// // fallback[Name] maps [what] to [what].
// // Used by [Section] > [function] to [purpose].
// // Tripwire: Future versions may load from [config file]; this is the fallback.
// var fallback[Name] = [type]{...}

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────
//
// Middle rung. Depends on Helpers below, used by Public APIs above.
// See: Org Chart > Ladder Structure > Core Operations
//
// Subsections: [List subsection names here]

//--- C.1 [Name] [TAG] (1) ---
// [What this category does. Entry point / workhorse / etc.]
// Uses: [Section] > [Subsection] > [what it uses]
// Contrast: [Section] > [Subsection] (when distinguishing from similar functions)
// [Reserved (0): if this operation category not needed in this file]
//
// Common naming patterns:
//   - load[Thing]()     - parse file/config, return typed struct (Entry point)
//   - extract[Thing]()  - pull data from loaded struct (Called after load)
//   - process[Thing]()  - transform data (may have side effects)
//   - validate[Thing]() - check constraints, return result
//   - build[Thing]()    - construct output from inputs
//
// Common inline comment patterns:
//   - // [ConstantName] from SETUP > Constants
//   - // BurntSushi/toml (or other external library callout)
//   - // [sort|filter|transform] by [field] ([0 = anchor] | [reason])
//
// // [functionName] [does what].
// //
// // Parameters:
// //   - [param]: [description]
// //
// // Returns:
// //   - [type]: [description]
// //   - error: [when returned]
// //
// // Uses: [Section] > [Subsection] > [helper/data used]
// //
// // Note: [Cross-layer dependency or side effect explanation]
// func [functionName]([params]) ([returns], error) {
// 	// implementation using helpers
// 	return result, nil
// }
//
// Actual load pattern structure (from real loaders):
//
// func load[Thing]() (*[Type], error) {
// 	// 1. Build path from constants
// 	path := filepath.Join(root, PathConst, FileConst) // PathConst, FileConst from SETUP > Constants
//
// 	// 2. Check file exists — explicit error
// 	if !fileExists(path) { // Helpers > Pure Functions
// 		return nil, fmt.Errorf("[thing] not found: %s", path)
// 	}
//
// 	// 3. Declare typed variable
// 	var result [Type]
//
// 	// 4. Parse with external library
// 	if _, err := toml.DecodeFile(path, &result); err != nil { // BurntSushi/toml
// 		return nil, fmt.Errorf("[thing] parse error: %w", err)
// 	}
//
// 	// 5. Post-process (sort by Order for dependency ordering)
// 	sort.Slice(result.Items, func(i, j int) bool { // Sort by order (0 = anchor)
// 		return result.Items[i].Order < result.Items[j].Order
// 	})
//
// 	// 6. Return
// 	return &result, nil
// }

//--- C.2 [Name] [TAG] (1) ---
// [Purpose of this category]
// Uses: Core Operations > C.1 > [function]  (if builds on C.1)
//
// Common C.2 patterns (from real files):
//
// Pattern A: Single File Loading (workhorse, used by C.3+)
// func loadFile(path string) (*ConfigFile, error) {
// 	if !fileExists(path) { return nil, fmt.Errorf("file not found: %s", path) }
// 	var data map[string]any
// 	if _, err := toml.DecodeFile(path, &data); err != nil { return nil, err }
// 	return &ConfigFile{Name: filepath.Base(path), Path: path, Data: data}, nil
// }
//
// Pattern B: Extract from Loaded Config (transform, no I/O)
// func extract[Thing](cfg *Config) *[Type] {
// 	sys := findSystem(cfg, "[name]") // Helpers > Pure Functions
// 	if sys == nil { return nil }
// 	result := &[Type]{}
// 	for _, p := range sys.Paths {
// 		switch p.Name {
// 		case "[field]": result.[Field] = p.Path
// 		}
// 	}
// 	return result
// }
//
// Pattern C: Load with Caching (cached access)
// func (l *Loader) Load[Thing](path string) (*[Type], error) {
// 	if cached, ok := l.cache[path]; ok { return cached, nil } // cache hit
// 	if !fileExists(path) { return nil, [error] }
// 	// ... load and parse ...
// 	l.cache[path] = result
// 	return result, nil
// }

//--- C.3 [Name] [TAG] (1) ---
// [Purpose - often batch operations building on C.2]
// Uses: Helpers > Pure Functions > globTOML, Core Operations > C.2 > [function]
//
// Common C.3 patterns:
//
// Pattern A: Directory Loading (batch using C.2 workhorse)
// func loadDirectory(dirPath string) ([]*ConfigFile, error) {
// 	matches, err := globTOML(dirPath) // Helpers > Pure Functions
// 	if err != nil { return nil, err }
// 	var results []*ConfigFile
// 	for _, path := range matches {
// 		cfg, err := loadFile(path) // Core Operations > C.2
// 		if err != nil { return results, err } // partial results
// 		results = append(results, cfg)
// 	}
// 	return results, nil
// }
//
// Pattern B: Validation (check constraints, return result)
// func (l *Loader) validate[Thing](result *Result, path string, value any, schema Schema) {
// 	if !l.checkType(value, schema.Type) { // Helpers > Pure Functions
// 		result.Errors = append(result.Errors, Error{Path: path, ...})
// 		result.Health -= weight
// 	}
// 	// ... additional validations ...
// }

//--- C.N [Name] [TAG] (0) ---
// [Reserved: reason this category not needed]
// See: [other file] > Core Operations > [section] (if logic lives elsewhere)
//
// Include troubleshooting for: File I/O, network operations, configuration
// parsing, external dependencies, complex validation. Focus on genuinely
// common issues, not every edge case.
//
// Example usage:
//
//	result, err := [FunctionName]([params])
//	if err != nil {
//	    // [How to handle errors]
//	}
//
// func [FunctionName]([parameters]) ([returns]) {
//     // DEBUGGING: Capture input state before processing
//     // [package]Inspector.Snapshot("[operation-name]-start", map[string]any{
//     //     "[key]": [value],
//     // })
//
//     // [Implementation with business logic]
//
//     // Health tracking pattern:
//     // if [success condition] {
//     //     [package]Logger.Success("[description]", +X, map[string]any{
//     //         "[context-key]": [context-value],
//     //     })
//     // } else {
//     //     [package]Logger.Failure("[description]", "[reason]", -X, map[string]any{
//     //         "[context-key]": [context-value],
//     //     })
//     // }
//
//     // DEBUGGING: Capture expected vs actual state divergence
//     // [package]Inspector.ExpectedState("[check-name]", [expected], [actual], map[string]any{
//     //     "[key]": [value],
//     // })
//
//     // return [result]
// }

// ──────────────────────────────────────────────────────────────────────────
// [Category 2 Name] - [Purpose]
// ──────────────────────────────────────────────────────────────────────────
// [Same documentation pattern as Category 1]

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// Centralized error management ensuring component handles failures gracefully.
// Provides safety boundaries and recovery strategies for robust operation.
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-004-error-handling.md
//
// Design Principle: [Blocking/Non-blocking] - [Brief explanation of philosophy]
// Example: Non-blocking - [component] failures never interrupt [main operation].
// The work of [main purpose] is more important than [secondary concern].
//
// Recovery Strategy:
//   - [Error type 1]: [How handled - e.g., Graceful degradation (fallback behavior)]
//   - [Error type 2]: [How handled - e.g., Fallback to alternative classification]
//   - [Error type 3]: [How handled - e.g., No panics - caught and logged]
//
// Common patterns:
// - Panic recovery: For display/formatting functions that must not crash
// - Error wrapping: Adding context to propagated errors
// - Graceful degradation: Continue with reduced functionality
// - Retry logic: Handle transient failures
// - Circuit breakers: Prevent cascading failures

// recoverFromPanic handles panic recovery with health tracking.
//
// Pattern for non-blocking libraries that must never crash calling code.
// Recovers from panics, logs with health impact, allows graceful degradation.
//
// Parameters:
//   function: Name of function where panic occurred
//   healthDelta: Negative health impact of panic
//
// Usage in functions:
//
//     func SomeFunction() string {
//         defer recoverFromPanic("SomeFunction", -10)
//         // ... implementation that might panic ...
//     }
//
// func recoverFromPanic(function string, healthDelta int) {
//     if r := recover(); r != nil {  // Check if panic occurred
//         [package]Logger.Error(
//             fmt.Sprintf("%s panic", function),
//             fmt.Errorf("panic: %v", r),  // Wrap panic value as error
//             healthDelta,
//         )
//     }
// }

// wrapError adds context to errors for better debugging.
//
// Wraps error with operation context and relevant details. Preserves
// original error for error chain inspection.
//
// Parameters:
//   operation: Name of operation that failed
//   err: Original error to wrap
//   context: Additional details for debugging
//
// Returns:
//   error: Wrapped error with context, or nil if err is nil
//
// Example usage:
//
//     if err := someOperation(); err != nil {
//         return wrapError("someOperation", err, map[string]any{
//             "input": input,
//             "state": currentState,
//         })
//     }
//
// func wrapError(operation string, err error, context map[string]any) error {
//     if err == nil {  // No error to wrap
//         return nil
//     }
//
//     // Build context string from map
//     var details []string
//     for k, v := range context {
//         details = append(details, fmt.Sprintf("%s=%v", k, v))
//     }
//
//     contextStr := strings.Join(details, ", ")  // Combine into single string
//
//     return fmt.Errorf("%s failed (%s): %w", operation, contextStr, err)  // Wrap with context
// }

// ──────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ──────────────────────────────────────────────────────────────────────────
// Exported functions defining component's public interface. Top rungs of
// the ladder - orchestrate helpers and core operations into complete
// functionality. Simple by design - complexity lives in helpers and core
// operations, Public APIs orchestrate proven pieces.
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-005-public-apis.md
//
// Organization: Group public APIs by purpose using category dividers:
//   // ═══ Category Name ═══
//   // [Functions in this category]
//
// Common categories: Initialization, Creation, Operations, Health, Cleanup

// ═══ [Category Name] ═══

// [PublicFunctionName] [does what at high level]
//
// What It Does:
// [Detailed explanation of complete operation]
//
// Parameters:
//   [paramName]: [Purpose and expected values]
//
// Returns:
//   [returnType]: [What's returned and meaning]
//   error: [When error returned, what it means]
//
// Health Impact:
//   Success: +X points ([reasoning])
//   Validation failure: -X points ([reasoning])
//   Processing failure: -X points ([reasoning])
//
// Example usage:
//
//	result, err := [PublicFunctionName]([params])
//	if err != nil {
//	    log.Printf("Operation failed: %v", err)
//	    return
//	}
//	fmt.Println(result)
//
// func [PublicFunctionName]([parameters]) ([returns]) {
//     // DEBUGGING: Capture input state before processing
//     // [package]Inspector.Snapshot("[operation]-start", map[string]any{
//     //     "[key]": [value],
//     // })
//
//     // Validate using helper function
//     if ![helperValidation]([input]) {  // Check if input meets criteria
//         [package]Logger.Failure(
//             "invalid input",
//             "validation failed",
//             -X,
//             map[string]any{"[key]": [value]},
//         )
//         return [default], fmt.Errorf("invalid input: %v", [input])
//     }
//
//     // Process using core operation (orchestrate, don't duplicate)
//     result, err := [coreOperation]([input])  // Apply business logic
//     if err != nil {  // Check for processing errors
//         [package]Logger.Error("processing failed", err, -X)
//         return [default], fmt.Errorf("processing: %w", err)  // Wrap and return
//     }
//
//     // Success - log with health impact
//     [package]Logger.Success("[operation] complete", +X, map[string]any{
//         "[key]": [value],
//     })
//
//     // DEBUGGING: Capture final state
//     // [package]Inspector.Snapshot("[operation]-complete", map[string]any{
//     //     "success": true,
//     //     "[key]": [value],
//     // })
//
//     return result, nil  // Return successful result
// }

// -----------------------------------------------------------------------------
// BODY Omission Guide
// -----------------------------------------------------------------------------
//
// ALL five sections MUST be present. Content may be reserved with reason:
//
//   - 1. Org Chart: Rarely reserved — most files benefit from structure map
//   - 2. Helpers: [Reserved: No internal helpers — uses imported utilities only]
//   - 3. Core Operations: Rarely reserved — contains primary business logic
//   - 4. Error Handling: [Reserved: Uses standard error returns, no custom recovery]
//   - 5. Public APIs: [Reserved: Library-only — no exported functions in this file]
//
// Unlike METADATA (sections omitted entirely with [OMIT:]), BODY preserves
// all section headers with [Reserved:] notation for unused sections.
//
// For multi-file packages using orchestrator pattern:
//   - Orchestrator file: Contains Org Chart, Public APIs, maybe Error Handling
//   - Module files: Contains Helpers, Core Operations for specific concerns
//   - Document extraction with [Reserved: Extracted to module.go (orchestrator pattern)]

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// CLOSING Zones Overview
// ──────────────────────────────────────────────────────────────────────────
//
// 3 Code Zones (operations):  Cv → Ce → Cc
// 6 Doc Sections (guidance):  X1 → X2 → X3 → X4 → X5 (+ X6 template-only)
//
// Three-tier ordering:
//   1. All code zones before any doc sections
//   2. Within code: Cv (Validation) → Ce (Execution) → Cc (Cleanup)
//   3. Within docs: X1 (Policy) → X2 (Extension) → X3 (Troubleshooting)
//                   → X4 (Reference) → X5 (Note) → X6 (Template Guide)
//
// Flow: verify → (use/run) → clean → document → guide future work

// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// Testing Requirements:
//   - Import the library without errors
//   - Call each public function/method with representative parameters
//   - Verify [output/files/results] created correctly
//   - Check [format/structure] remains [parseable/valid]
//   - Confirm no go vet warnings introduced
//   - Run: go test -v ./... (when tests exist)
//
// Build Verification:
//   - go build ./... (compiles without errors)
//   - go vet ./... (no warnings)
//   - [Any linting or static analysis tools]
//
// Example validation:
//
//     result, err := YourFunction(input)
//     if err != nil {
//         t.Errorf("YourFunction failed: %v", err)
//     }

// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — no entry point. Imported and called, not executed directly.
//
// Usage: import "[your-module-path]/[package-name]"
//
// Example:
//
//     package main
//
//     import "[your-module-path]/[package-name]"
//
//     func main() {
//         result, err := packagename.YourFunction(params)
//         if err != nil {
//             log.Fatal(err)
//         }
//     }

// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Resource Management:
//   - [Resource type]: [How managed — auto/manual/deferred]
//   - N/A for libraries (no lifecycle) — calling code responsible
//   - If stateful: provide Close() or Cleanup() function
//
// Example cleanup pattern:
//
//     resource := packagename.NewResource()
//     defer resource.Close()
//     resource.DoWork()

// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Remove not the ancient landmark, which thy fathers have set."
// — Proverbs 22:28
//
// Safe to Modify:
//   - Add new [functions/types/constants] (follow existing patterns)
//   - Add new [helper functions] in appropriate groups
//   - Extend [specific feature] (add more [specific thing])
//
// Modify with Care:
//   - Public API function signatures — breaks all calling code
//   - [Exported struct] fields — breaks code accessing fields directly
//   - [Critical system behavior] — affects all users
//
// Never Modify:
//   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   - [Fundamental principle 1]
//   - [Architectural pattern — Rails/etc]
//
// Architecture: See BODY "1. Org Chart" for ladder/baton flow.
// Validation: See Cv zone above.

// ──────────────────────────────────────────────────────────────────────────
// X2: Extension
// ──────────────────────────────────────────────────────────────────────────
//
// Designed Growth Points (see BODY subsection headers for details):
//   - Adding [Feature Type 1]: See BODY "3. Core Operations" extension points
//   - Adding [Feature Type 2]: See BODY "[Subsection]" extension points
//   - Adding helpers: See BODY "2. Helpers" section organization
//
// Future Considerations:
//   - [Planned feature 1]
//   - [Research direction 1]
//   - [Integration target 1]
//
// Known Limitations:
//   - [Limitation 1]
//   - [Limitation 2]

// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lack wisdom, let him ask of God." — James 1:5
//
// Performance: See SETUP constants/types and BODY function docstrings
// for operation-specific performance characteristics.
//
// Common Issues:
//   - [Problem 1]: See [FunctionName] docstring troubleshooting
//   - [Problem 2]: [Cause] → [Solution]
//   - [Problem 3]: [Cause] → [Solution]

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Dependencies:   See METADATA C4 (requires, consumers, integration)
// Dependents:     [Who uses this — see METADATA C4.consumers]
// Template:       b-word/seed/code/L0/go/library.go
//
// Quick Usage:
//
//     // Basic setup
//     [example code for basic usage]
//
//     // [Pattern/Feature]:
//     [example code demonstrating pattern]

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// This library is [architectural role — RAILS/LADDER/BATON description].
// [1-2 sentences: place in ecosystem, what depends on it].
//
// Purpose: See METADATA C5 (purpose, philosophy) for design intent.
// Integration: See METADATA C4 (integration) for step-by-step guide.
//
// "[Relevant Scripture verse]" — [Reference]

// ──────────────────────────────────────────────────────────────────────────
// X6: Template Guide (remove this section when instantiating)
// ──────────────────────────────────────────────────────────────────────────
//
// Instantiation:
//   1.  cp library.go /path/to/new/package/mypackage.go
//   2.  Change pragma: #!omni template → //omni:code --go -library
//   3.  Update meta.key, meta.from, meta.at for your package
//   4.  Fill Pragma (I1-I4) with package identity
//   5.  Fill Metadata (C1-C7) with package context
//   6.  Replace [placeholder] markers with actual values
//   7.  Implement types in SETUP, functions in BODY
//   8.  Fill CLOSING zones (Cv-Cc with validation/usage, X1-X5 with guidance)
//   9.  Remove X6 section and all [placeholder] markers
//  10.  Remove //go:build ignore line
//  11.  Run: go build ./... && go vet ./... && go test -v ./...

// ──────────────────────────────────────────────────────────────────────────
// CLOSING Omission Guide
// ──────────────────────────────────────────────────────────────────────────
//
// All 9 zones should be present. Content may be minimal with reason:
//
// Code Zones:
//   - Cv (Validation): Rarely omitted — all code needs verification
//   - Ce (Execution): Library: reserved (no entry point). Executable: main() flow.
//   - Cc (Cleanup): Library: consumer patterns. Executable: signal handling.
//
// Doc Sections:
//   - X1 (Policy): Rarely omitted — always guides maintainers
//   - X2 (Extension): Growth points + roadmap; [Reserved: if feature-complete]
//   - X3 (Troubleshooting): Performance + common issues; back-ref to BODY docstrings
//   - X4 (Reference): Dependencies + quick usage examples
//   - X5 (Note): Summary + scripture anchor
//   - X6 (Template Guide): Template-only — REMOVE when instantiating
//
// Principle: CLOSING synthesizes. METADATA/SETUP/BODY contain details.
// Don't repeat — reference back to where the information lives.

// ============================================================================
// END CLOSING
// ============================================================================
