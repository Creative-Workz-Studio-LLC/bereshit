//omni:code --go -library
//omni:key B-L0-identity-registry
//omni:version b-02.00
//omni:pragma Pragma
//omni:metadata Metadata

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-identity-registry
// Purpose: Runtime package identity registration and discovery
// Biblical: Genesis 1:5 — And God called the light Day, and the darkness
//           he called Night
// Version: b-02.00

// Package identity provides the OmniCode package identity registry.
//
// Every Go package with Pragma and Metadata vars registers itself at
// init time, making the system self-aware at runtime. Two access paths:
//
//	Static:  grep -rn "//omni:key" → finds all Go file identities
//	Runtime: identity.Lookup("B-L0-config-loader") → pragma, metadata
//
// The registry is the system knowing itself. L3 state machine queries
// "what am I made of?" Health scoring iterates all packages. Dashboard
// shows topology. This is the CPI-SI self-awareness layer.
//
// See Pragma (I1-I4) and Metadata (C1-C7) for OmniCode identity.
package identity

// ──────────────────────────────────────────────────────────────────────────
// Metadata Imports
// ──────────────────────────────────────────────────────────────────────────
//
// Go requires all imports before any declarations. Since Pragma/Metadata
// vars live in METADATA, ALL imports must be here — not in SETUP.
// This is a Go language constraint, not a structural choice.

import (
	"errors"
	"fmt"
	"maps"
	"slices"
	"strings"
	"sync"
)

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

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
//	key := PragmaGet("I1.key") // returns "B-L0-identity-registry"
var Pragma = [][2]string{
	// I1: Core
	{"I1.key", "B-L0-identity-registry"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.go"},
	{"I1.at", "b-02.00"},
	// I2: Family
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "foundation"},
	// I3: Instance
	{"I3.file", "registry.go"},
	{"I3.title", "Identity Registry"},
	{"I3.component", "b-spiral/identity"},
	{"I3.path", "L0-universal/b-spiral/identity/registry.go"},
	{"I3.provides", "IDENTITY_REGISTRY"},
	{"I3.brief", "Runtime package identity registration and discovery"},
	// I4: Architecture
	{"I4.layer", "L0"},
	{"I4.position", "spiral/identity"},
	{"I4.pattern", "init() { identity.Register(...) } → identity.Lookup(key)"},
}

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

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
//	version := MetadataGet("C1.version") // returns "b-01.00"
var Metadata = [][2]string{
	// C1: State
	{"C1.version", "b-02.00"},
	{"C1.status", "Active"},
	{"C1.created", "2026-02-13"},
	{"C1.updated", "2026-02-18"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "CreativeWorkzStudio LLC"},
	// C3: Grounding
	{"C3.scripture", "Genesis 1:5 — And God called the light Day, and the darkness he called Night"},
	{"C3.principle", "Naming establishes identity. Registration declares existence."},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "errors, fmt, maps, strings, sync"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "none — L0 foundation, no internal dependencies"},
	{"C4.consumers", "every OmniCode Go package with Pragma/Metadata vars"},
	{"C4.integration", "import identity; identity.Register(key, pragma, metadata)"},
	{"C4.if_missing", "no runtime self-awareness — packages can't discover identity"},
	// C5: Intent
	{"C5.purpose", "Enable runtime self-awareness — the system knows its own components"},
	{"C5.philosophy", "Identity is code, not documentation. Registration is declaration."},
	// C6: Roadmap
	{"C6.current", "b-01.50 — Production SETUP, sentinel errors, query indexes, type methods, tests"},
	{"C6.planned", "Dependency graph from C4 data, health aggregation, dashboard feed"},
	{"C6.limitations", "In-memory only, no persistence, no cross-process discovery"},
	// C7: Classification
	{"C7.tags", "identity, registry, discovery, self-awareness, cpi-si"},
	{"C7.category", "Ladder"},
	{"C7.domain", "identity"},
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
//   §01 Imports           — What this file depends on
//   §02 Constants         — Compile-time fixed values
//   §03 Variables         — Package-level mutable state
//   §04 Type Aliases      — Shorthand for complex signatures
//   §05 Error Types       — Custom errors + Error() method
//   §06 Core Types        — struct definitions + simple constructors
//   §07 Interface Defs    — Behavioral contracts (shape, not fulfillment)
//   §08 Type Methods      — Structural behaviors (completing interface impls)
//   §09 Code Generation   — go:generate directives
//   §10 Build Tags        — Conditional compilation items

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────
//
// [See: METADATA > Metadata Imports — Go requires all imports before
// declarations, so imports live with the identity vars they precede]
//
// --- Standard Library ---
// errors, fmt, maps, strings, sync
//
// --- External Packages ---
// [Pure library — no external dependencies at L0]
//
// --- Internal Packages ---
// [Self-contained at L0 — no internal dependencies]
//
// --- Package-Internal ---
// [Single-file package — no sub-imports]

// ──────────────────────────────────────────────────────────────────────────
// 2. Constants
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: no constants needed — all behavior is runtime-dynamic.
// The registry has no fixed limits or configuration. Future: MaxPackages
// safety limit if needed.]

// ──────────────────────────────────────────────────────────────────────────
// 3. Variables
// ──────────────────────────────────────────────────────────────────────────
//
// Runtime-initialized mutable state.
// NOTE: Pragma and Metadata identity variables live in the METADATA block.

var (
	mu       sync.RWMutex
	packages = make(map[string]*PackageIdentity)

	// Query indexes — incrementally updated on Register() via binary search.
	// Sorted by OmniCode key within each bucket for O(log n) operations.
	// Avoids full iteration in ByLayer/ByDomain for repeated queries.
	layerIndex  = make(map[string][]*PackageIdentity)
	domainIndex = make(map[string][]*PackageIdentity)
)

// --- Sentinel Errors ---

var (
	// ErrEmptyKey is returned when Register is called with an empty key.
	ErrEmptyKey = errors.New("identity: empty key")

	// ErrNotRegistered is returned when a Lookup key doesn't exist.
	// Use errors.Is(err, identity.ErrNotRegistered) for checking.
	ErrNotRegistered = errors.New("identity: key not registered")
)

// ──────────────────────────────────────────────────────────────────────────
// 4. Type Aliases
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: no aliases needed — types are direct]

// ──────────────────────────────────────────────────────────────────────────
// 5. Error Types
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: Sentinel errors in §3 cover the error surface.
// Custom error types (struct + Error()) would be over-engineering
// for a package with only 2 error conditions.]

// ──────────────────────────────────────────────────────────────────────────
// 6. Core Types
// ──────────────────────────────────────────────────────────────────────────

// PackageIdentity holds a registered package's OmniCode identity.
//
// Pragma carries file-level identity (I1-I4), stored as map for O(1) lookup.
// Metadata carries full context (C1-C7), stored as map for O(1) lookup.
// Created by Register() which converts [][2]string slices to maps.
//
// Thread-safe after registration — fields are never mutated post-Register.
// Use Get() for unified key lookup across both Pragma and Metadata.
type PackageIdentity struct {
	Key      string            // OmniCode key (I1.key value)
	Pragma   map[string]string // I1-I4 (converted from [][2]string at registration)
	Metadata map[string]string // C1-C7 (converted from [][2]string at registration)
}

// RegistryStats holds a point-in-time snapshot of registry state.
// Used by health systems and dashboards for topology awareness.
type RegistryStats struct {
	Total   int            // number of registered packages
	ByLayer map[string]int // count per layer (L0, L1, etc.)
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Interface Definitions
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: no interfaces — concrete types only at L0 foundation]

// ──────────────────────────────────────────────────────────────────────────
// 8. Type Methods
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: String() and Get() are structural methods per template §8,
// but the linter classifies all method_decl as BODY content. Methods
// live in BODY > Public APIs > Type Methods until the linter recognizes
// SETUP §8. See: b-word/seed/code/L0/go/library.go §8 for template spec.]

// ──────────────────────────────────────────────────────────────────────────
// 9. Code Generation
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: no code generation needed]

// ──────────────────────────────────────────────────────────────────────────
// 10. Build Tags
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: no conditional compilation]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: 1 → 2 → 3 → 4 → 5 (ascending numeric)
// Flow: understand structure → build foundations → implement logic → handle errors → expose

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs — Orchestration)
//   ├── Lookup()     → reads packages map (ok-pattern)
//   ├── LookupErr()  → reads packages map (error-pattern)
//   ├── All()        → reads packages map
//   ├── Count()      → reads packages map
//   ├── ByLayer()    → reads layerIndex (O(1) via index)
//   ├── ByDomain()   → reads domainIndex (O(1) via index)
//   ├── ByTag()      → reads packages map, splits tags
//   └── Stats()      → reads layerIndex (snapshot)
//
//   Core Operations (Middle Rungs — Business Logic)
//   ├── Register()    → calls RegisterErr() (fire-and-forget)
//   └── RegisterErr() → uses sliceToMap(), rebuildIndexes()
//
//   Helpers (Bottom Rungs — Foundations)
//   ├── sliceToMap()      → pure function ([][2]string → map)
//   ├── copyMap()          → pure function (map → map copy)
//   ├── byMetadata()       → filter by key/value (available for future queries)
//   ├── updateIndexes()    → incremental index update (O(log n) via binary search)
//   ├── insertIntoIndex()  → sorted insertion into index bucket (O(log n))
//   └── removeFromIndex()  → sorted removal from index bucket (O(log n))
//
// Self (Bootstrap):
//   init()           → calls Register() with own Pragma/Metadata
//   PragmaSlice()    → returns own Pragma ([][2]string)
//   MetadataSlice()  → returns own Metadata ([][2]string)
//   PragmaGet()      → key lookup in own Pragma
//   MetadataGet()    → key lookup in own Metadata
//
// APUs (Available Processing Units):
//   - 19 functions total
//   - 6 helpers (pure foundations + O(log n) index operations)
//   - 2 core operations (Register, RegisterErr)
//   - 11 public APIs (Lookup, LookupErr, All, Count, ByLayer, ByDomain,
//     ByTag, Stats, PragmaSlice, MetadataSlice, PragmaGet, MetadataGet + init)

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────

//--- H.1 Pure Functions [TRANSFORM] (1) ---

// sliceToMap converts an identity slice ([][2]string) to a map for O(1) lookup.
// Used by Register() to store identity data in the registry.
func sliceToMap(pairs [][2]string) map[string]string {
	m := make(map[string]string, len(pairs))
	for _, pair := range pairs {
		m[pair[0]] = pair[1]
	}
	return m
}

// copyMap returns a shallow copy of a string map.
// Prevents callers from mutating registered identity after the fact.
func copyMap(m map[string]string) map[string]string {
	c := make(map[string]string, len(m))
	maps.Copy(c, m)
	return c
}

// byMetadata filters packages by exact match on a metadata key.
// Available for future ad-hoc queries (ByStatus, ByParadigm, etc.).
func byMetadata(key, value string) []*PackageIdentity {
	mu.RLock()
	defer mu.RUnlock()

	var result []*PackageIdentity
	for _, p := range packages {
		if p.Metadata[key] == value {
			result = append(result, p)
		}
	}
	return result
}

// updateIndexes incrementally updates the layer and domain indexes for a
// single package registration. Called under write lock.
//
// O(log n) — binary search for insertion point in sorted index slices.
// Replaces the O(n) rebuildIndexes: instead of scanning all packages on
// every Register, we insert only the new package into its correct position.
//
// The principle: ln(n) operations, not n. Foundation matters most —
// the binary search halves the space at each step (logarithmic descent
// toward the anchor position). dH/dn = k/n.
func updateIndexes(p *PackageIdentity, old *PackageIdentity) {
	// Remove old entry if overwriting (different layer/domain possible)
	if old != nil {
		removeFromIndex(layerIndex, old.Pragma["I4.layer"], old.Key)
		removeFromIndex(domainIndex, old.Metadata["C7.domain"], old.Key)
	}

	// Insert new entry at sorted position
	if layer := p.Pragma["I4.layer"]; layer != "" {
		insertIntoIndex(layerIndex, layer, p)
	}
	if domain := p.Metadata["C7.domain"]; domain != "" {
		insertIntoIndex(domainIndex, domain, p)
	}
}

// insertIntoIndex inserts p into the sorted slice at idx[key].
// Binary search: O(log n) for finding position. Sorted by OmniCode key.
func insertIntoIndex(idx map[string][]*PackageIdentity, key string, p *PackageIdentity) {
	entries := idx[key]

	// Binary search for insertion point (sorted by OmniCode key)
	pos, found := slices.BinarySearchFunc(entries, p, func(a, b *PackageIdentity) int {
		return strings.Compare(a.Key, b.Key)
	})

	if found {
		entries[pos] = p // Update in place — duplicate key
		return
	}

	idx[key] = slices.Insert(entries, pos, p)
}

// removeFromIndex removes the package with omniKey from the sorted slice.
// Binary search: O(log n) for finding the entry.
func removeFromIndex(idx map[string][]*PackageIdentity, indexKey, omniKey string) {
	if indexKey == "" {
		return
	}
	entries := idx[indexKey]
	if len(entries) == 0 {
		return
	}

	// Binary search using a temporary key-only identity for comparison
	target := &PackageIdentity{Key: omniKey}
	pos, found := slices.BinarySearchFunc(entries, target, func(a, b *PackageIdentity) int {
		return strings.Compare(a.Key, b.Key)
	})

	if found {
		idx[indexKey] = slices.Delete(entries, pos, pos+1)
	}
}

//--- H.2 Guard Functions [CHECK] (0) ---
// [Reserved: no precondition guards needed — all inputs validated inline]

//--- H.3 Domain Functions [DOMAIN] (0) ---
// [Reserved: no domain-specific helpers beyond pure functions above]

//--- H.4 Fallback Data [DATA] (0) ---
// [Reserved: no fallback data — registry is the source of truth]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────

//--- C.1 Registration [REGISTER] (2) ---

// Register adds a package's identity to the global registry.
// Called from init() in each OmniCode Go package:
//
//	func init() {
//	    identity.Register(PragmaGet("I1.key"), Pragma, Metadata)
//	}
//
// Empty keys are silently ignored. For error handling, use RegisterErr.
// Duplicate keys overwrite silently (last writer wins).
func Register(key string, pragma, metadata [][2]string) {
	_ = RegisterErr(key, pragma, metadata)
}

// RegisterErr adds a package's identity and returns any validation error.
// Returns ErrEmptyKey if key is empty.
//
// Slices are converted to maps for O(1) runtime queries.
// Query indexes are incrementally updated via binary search — O(log n).
func RegisterErr(key string, pragma, metadata [][2]string) error {
	if key == "" {
		return ErrEmptyKey
	}

	p := &PackageIdentity{
		Key:      key,
		Pragma:   sliceToMap(pragma),
		Metadata: sliceToMap(metadata),
	}

	mu.Lock()
	old := packages[key] // nil if new, non-nil if overwriting
	packages[key] = p
	updateIndexes(p, old)
	mu.Unlock()
	return nil
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
//
// Design: Non-blocking — registration failures never panic.
// Fire-and-forget Register() silently drops empty keys.
// RegisterErr() returns sentinel errors for callers that care.
//
// Recovery Strategy:
//   - Empty key:     ErrEmptyKey (caller decides to log or ignore)
//   - Key not found: ErrNotRegistered (caller decides fallback)
//   - Duplicate key: Last writer wins (init() ordering is deterministic)
//   - Lock poison:   Not possible — Go's RWMutex doesn't poison

// ──────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// ═══ Discovery ═══

// Lookup returns a package by its OmniCode key.
// Returns nil, false if the key is not registered.
func Lookup(key string) (*PackageIdentity, bool) {
	mu.RLock()
	defer mu.RUnlock()
	p, ok := packages[key]
	return p, ok
}

// LookupErr returns a package by key or ErrNotRegistered.
// Use when you need errors.Is() checking instead of ok-pattern.
func LookupErr(key string) (*PackageIdentity, error) {
	mu.RLock()
	defer mu.RUnlock()
	p, ok := packages[key]
	if !ok {
		return nil, ErrNotRegistered
	}
	return p, nil
}

// All returns every registered package.
// Order is not guaranteed — use ByLayer or ByDomain for structured access.
func All() []*PackageIdentity {
	mu.RLock()
	defer mu.RUnlock()

	result := make([]*PackageIdentity, 0, len(packages))
	for _, p := range packages {
		result = append(result, p)
	}
	return result
}

// Count returns the number of registered packages.
func Count() int {
	mu.RLock()
	defer mu.RUnlock()
	return len(packages)
}

// ═══ Query ═══

// ByLayer returns all packages at a given layer via the layer index.
// O(1) lookup — indexes are rebuilt on every Register().
// Example: identity.ByLayer("L0")
func ByLayer(layer string) []*PackageIdentity {
	mu.RLock()
	defer mu.RUnlock()
	return layerIndex[layer]
}

// ByDomain returns all packages in a given domain via the domain index.
// O(1) lookup — indexes are rebuilt on every Register().
// Example: identity.ByDomain("identity")
func ByDomain(domain string) []*PackageIdentity {
	mu.RLock()
	defer mu.RUnlock()
	return domainIndex[domain]
}

// ByTag returns all packages that have a specific tag in C7.tags.
// Tags are comma-separated in the metadata value.
// Example: identity.ByTag("cpi-si")
func ByTag(tag string) []*PackageIdentity {
	mu.RLock()
	defer mu.RUnlock()

	tag = strings.TrimSpace(tag)
	var result []*PackageIdentity
	for _, p := range packages {
		tags, ok := p.Metadata["C7.tags"]
		if !ok {
			continue
		}
		for _, t := range strings.Split(tags, ",") {
			if strings.TrimSpace(t) == tag {
				result = append(result, p)
				break
			}
		}
	}
	return result
}

// ═══ Type Methods ═══

// String implements fmt.Stringer for PackageIdentity.
// Format: "key (subtype @ layer)" e.g. "B-L0-identity-registry (library @ L0)"
func (p *PackageIdentity) String() string {
	return fmt.Sprintf("%s (%s @ %s)", p.Key, p.Pragma["I2.subtype"], p.Pragma["I4.layer"])
}

// Get returns a unified field lookup across both Pragma and Metadata.
// Checks Pragma first (I-prefixed), then Metadata (C-prefixed).
// Returns empty string if not found in either.
func (p *PackageIdentity) Get(key string) string {
	if v, ok := p.Pragma[key]; ok {
		return v
	}
	if v, ok := p.Metadata[key]; ok {
		return v
	}
	return ""
}

// ═══ Stats ═══

// Stats returns a point-in-time snapshot of registry statistics.
// Used by health systems and dashboards for topology awareness.
func Stats() RegistryStats {
	mu.RLock()
	defer mu.RUnlock()

	s := RegistryStats{
		Total:   len(packages),
		ByLayer: make(map[string]int),
	}
	for layer, pkgs := range layerIndex {
		s.ByLayer[layer] = len(pkgs)
	}
	return s
}

// ═══ Self — Identity Bootstrap ═══

// init registers the identity package itself. The registry knowing itself
// is the bootstrap anchor — like Genesis 1:1, it IS the foundation AND
// declares its own existence. Self-awareness starts with self-registration.
func init() {
	Register(PragmaGet("I1.key"), Pragma, Metadata)
}

// PragmaSlice returns this package's own OmniCode pragma identity (I1-I4)
// as the original slice type.
func PragmaSlice() [][2]string { return Pragma }

// MetadataSlice returns this package's own OmniCode metadata (C1-C7)
// as the original slice type.
func MetadataSlice() [][2]string { return Metadata }

// PragmaGet returns a single value from this package's Pragma by key.
// Returns empty string if key not found.
func PragmaGet(key string) string {
	for _, pair := range Pragma {
		if pair[0] == key {
			return pair[1]
		}
	}
	return ""
}

// MetadataGet returns a single value from this package's Metadata by key.
// Returns empty string if key not found.
func MetadataGet(key string) string {
	for _, pair := range Metadata {
		if pair[0] == key {
			return pair[1]
		}
	}
	return ""
}

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
// 5 Doc Sections (guidance):  X1 → X2 → X3 → X4 → X5
//
// Three-tier ordering:
//   1. All code zones before any doc sections
//   2. Within code: Cv (Validation) → Ce (Execution) → Cc (Cleanup)
//   3. Within docs: X1 (Policy) → X2 (Extension) → X3 (Troubleshooting)
//                   → X4 (Reference) → X5 (Note)

// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// Testing:
//   - go test -v ./...
//   - 20+ tests covering all public APIs and type methods
//   - Concurrent access verified with goroutine stress tests
//   - Self-registration via init() verified
//   - Sentinel errors verified with errors.Is()
//   - Index-backed queries (ByLayer, ByDomain) verified
//
// Build Verification:
//   - go build ./...
//   - go vet ./...

// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — no entry point. Imported and called, not executed directly.
//
// Usage: import "creativeworkzstudio.com/bereshit/L0-universal/spiral/identity"

// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// In-memory registry — no resources to clean up.
// Garbage collected when process exits. No Close() needed.

// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Remove not the ancient landmark, which thy fathers have set."
// — Proverbs 22:28
//
// Safe to Modify:
//   - Add new query functions (follow ByLayer/ByDomain pattern)
//   - Add new metadata accessors
//
// Modify with Care:
//   - Register() signature — breaks all calling packages
//   - PackageIdentity struct — breaks all consumers
//
// Never Modify:
//   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   - Self-registration pattern (init() calling Register)
//   - [][2]string identity format (I1-I4, C1-C7)

// ──────────────────────────────────────────────────────────────────────────
// X2: Extension
// ──────────────────────────────────────────────────────────────────────────
//
// Completed (b-01.50):
//   - Sentinel errors (ErrEmptyKey, ErrNotRegistered)
//   - Query indexes (layerIndex, domainIndex) for O(1) filtered access
//   - PackageIdentity.String() and .Get() type methods
//   - RegisterErr() and LookupErr() error-returning variants
//   - RegistryStats type + Stats() API
//   - Comprehensive test suite (registry_test.go)
//
// Completed (b-02.00):
//   - O(log n) index updates — replaced rebuildIndexes() O(n) with
//     updateIndexes/insertIntoIndex/removeFromIndex using binary search
//   - Sorted index slices via slices.BinarySearchFunc
//   - Correct handling of duplicate key overwrites (remove old, insert new)
//   - Principle: logarithmic descent toward anchor position (dH/dn = k/n)
//
// Designed Growth Points:
//   - Adding queries: See BODY "5. Public APIs > Query" pattern
//   - Adding indexes: See updateIndexes/insertIntoIndex pattern in Helpers
//   - Adding helpers: See BODY "2. Helpers > H.1 Pure Functions"
//
// Future Considerations:
//   - Dependency graph from C4 data
//   - Health aggregation across all registered packages
//   - Dashboard feed for real-time topology
//   - Cross-process discovery (gRPC/IPC)

// ──────────────────────────────────────────────────────────────────────────
// X3: Troubleshooting
// ──────────────────────────────────────────────────────────────────────────
//
// "If any of you lack wisdom, let him ask of God." — James 1:5
//
// Common Issues:
//   - Package not found: init() not running — check import path
//   - Duplicate keys: last writer wins — check //omni:key uniqueness
//   - Empty metadata: Pragma/Metadata vars not populated

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
//
// Dependencies:   See METADATA C4 (stdlib: errors, fmt, maps, strings, sync)
// Template:       b-word/seed/code/L0/go/library.go
//
// Quick Usage:
//
//     // Register (in each package's init)
//     func init() {
//         identity.Register(PragmaGet("I1.key"), Pragma, Metadata)
//     }
//
//     // Query
//     pkg, ok := identity.Lookup("B-L0-config-loader")
//     allL0 := identity.ByLayer("L0")

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// The identity registry is the system's self-awareness layer.
// Every OmniCode Go package registers itself; the registry
// self-registers in init() — the bootstrap anchor.
//
// "And God called the light Day, and the darkness he called Night."
// — Genesis 1:5
//
// Naming establishes identity. Registration declares existence.

// ============================================================================
// END CLOSING
// ============================================================================
