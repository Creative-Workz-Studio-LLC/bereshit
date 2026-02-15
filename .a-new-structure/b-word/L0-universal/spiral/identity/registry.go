//omni:code --go -library
//omni:key B-L0-identity-registry
//omni:version b-01.00
//omni:pragma _pragma
//omni:metadata _metadata

// ============================================================================
// METADATA
// ============================================================================

// Package identity provides the OmniCode package identity registry.
//
// Every Go package with _pragma and _metadata maps registers itself at
// init time, making the system self-aware at runtime. Two access paths:
//
//	Static:  grep -rn "//omni:key" → finds all Go file identities
//	Runtime: identity.Lookup("B-L0-config-loader") → pragma, metadata
//
// The registry is the system knowing itself. L3 state machine queries
// "what am I made of?" Health scoring iterates all packages. Dashboard
// shows topology. This is the CPI-SI self-awareness layer.
//
// See _pragma and _metadata for OmniCode identity.
package identity

//--- I.1 Standard Library [IMPORT] (1) ---
import (
	"maps"
	"strings"
	"sync"
)

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

// _pragma — OmniCode file identity (P1-P5). Runtime-queryable.
var _pragma = map[string]string{
	// P1: Core Identity
	"P1.key":    "B-L0-identity-registry",
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
	"P5.title":   "Identity Registry",
	"P5.summary": "Runtime package identity registration and discovery",
}

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

// _metadata — OmniCode file metadata (M1-M10). Runtime-queryable.
//
// M1-M5: Code-facing. M6-M10: System-facing.
var _metadata = map[string]string{

	// ── M1: Core Identity ─────────────────────────────────────────
	"M1.key":            "B-L0-identity-registry",
	"M1.component_type": "Ladder",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2026-02-13",

	// ── M2: Version History ───────────────────────────────────────
	"M2.b-01.00": "2026-02-13 — Initial creation: registry, query, self-awareness layer",

	// ── M3: Interface ─────────────────────────────────────────────
	"M3.requires.stdlib":   "strings, sync",
	"M3.requires.external": "none",
	"M3.requires.internal": "none — L0 foundation, no internal dependencies",
	"M3.used_by":           "L0-universal/hybrid/config, L1-omnicode/hybrid/pragma, L3-cpisi",
	"M3.import":            "creativeworkzstudio.com/bereshit/L0-universal/spiral/identity",
	"M3.pattern":           "init() { identity.Register(key, pragma, metadata) } → identity.Lookup(key)",

	// ── M4: Public API ────────────────────────────────────────────
	"M4.register": "Register",
	"M4.lookup":   "Lookup, All",
	"M4.query":    "ByLayer, ByDomain, ByTag",
	"M4.self":     "Pragma, Metadata",

	// ── M5: Operational ───────────────────────────────────────────
	"M5.blocking": "no — in-memory map operations only",
	"M5.health":   "Provider | granted: registered | deferred: init pending | denied: n/a",

	// ── M6: Classification ────────────────────────────────────────
	"M6.tags":     "identity, registry, discovery, self-awareness, cpi-si",
	"M6.category": "Ladder",
	"M6.domain":   "identity",
	"M6.layer":    "L0-universal",
	"M6.paradigm": "CPI-SI",

	// ── M7: Intent ────────────────────────────────────────────────
	"M7.purpose":    "Enable runtime self-awareness — the system knows its own components",
	"M7.philosophy": "Identity is code, not documentation. Registration is declaration.",
	"M7.provides":   "Package registration, key-based lookup, layer/domain/tag queries",

	// ── M8: Grounding ─────────────────────────────────────────────
	"M8.scripture":  "Genesis 1:5 — And God called the light Day, and the darkness he called Night",
	"M8.principle":  "Naming establishes identity. Registration declares existence.",
	"M8.anchor":     "Genesis 1:1 — In the beginning God created the heaven and the earth",

	// ── M9: Dependencies ──────────────────────────────────────────
	"M9.needs.stdlib":   "strings (tag parsing), sync (concurrent access)",
	"M9.needs.external": "none",
	"M9.needs.internal": "none — zero internal dependencies, L0 foundation",
	"M9.used_by":        "every OmniCode Go package with _pragma/_metadata",
	"M9.layer_deps":     "L0 only — no L1+ dependencies. Must stay dependency-free.",

	// ── M10: Roadmap ──────────────────────────────────────────────
	"M10.current":     "b-01.00 — Core registry with Register, Lookup, query functions",
	"M10.planned":     "Dependency graph from M9 data, health aggregation, dashboard feed",
	"M10.limitations": "In-memory only, no persistence, no cross-process discovery",
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Types → Variables
// Minimal setup — the registry is small by design.

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// PackageIdentity holds a registered package's OmniCode identity.
// Pragma carries file-level identity (P1-P5).
// Metadata carries full context (M1-M10).
type PackageIdentity struct {
	Key      string            // OmniCode key (P1.key / M1.key)
	Pragma   map[string]string // P1-P5
	Metadata map[string]string // M1-M10
}

// ────────────────────────────────────────────────────────────────
// Variables
// ────────────────────────────────────────────────────────────────

var (
	mu       sync.RWMutex
	packages = make(map[string]*PackageIdentity)
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Registration → Lookup → Query → Self

// ────────────────────────────────────────────────────────────────
// Registration
// ────────────────────────────────────────────────────────────────

// Register adds a package's identity to the global registry.
// Called from init() in each OmniCode Go package:
//
//	func init() {
//	    identity.Register(_pragma["P1.key"], _pragma, _metadata)
//	}
//
// Maps are shallow-copied to prevent mutation after registration.
// Duplicate keys overwrite silently (last writer wins).
func Register(key string, pragma, metadata map[string]string) {
	p := &PackageIdentity{
		Key:      key,
		Pragma:   copyMap(pragma),
		Metadata: copyMap(metadata),
	}

	mu.Lock()
	packages[key] = p
	mu.Unlock()
}

// ────────────────────────────────────────────────────────────────
// Lookup
// ────────────────────────────────────────────────────────────────

// Lookup returns a package by its OmniCode key.
// Returns nil, false if the key is not registered.
func Lookup(key string) (*PackageIdentity, bool) {
	mu.RLock()
	defer mu.RUnlock()
	p, ok := packages[key]
	return p, ok
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

// ────────────────────────────────────────────────────────────────
// Query
// ────────────────────────────────────────────────────────────────

// ByLayer returns all packages at a given layer (M6.layer).
// Example: identity.ByLayer("L0-universal")
func ByLayer(layer string) []*PackageIdentity {
	return byMetadata("M6.layer", layer)
}

// ByDomain returns all packages in a given domain (M6.domain).
// Example: identity.ByDomain("config-loading")
func ByDomain(domain string) []*PackageIdentity {
	return byMetadata("M6.domain", domain)
}

// ByTag returns all packages that have a specific tag in M6.tags.
// Tags are comma-separated in the metadata value.
// Example: identity.ByTag("tripwire")
func ByTag(tag string) []*PackageIdentity {
	mu.RLock()
	defer mu.RUnlock()

	tag = strings.TrimSpace(tag)
	var result []*PackageIdentity
	for _, p := range packages {
		tags, ok := p.Metadata["M6.tags"]
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

// ────────────────────────────────────────────────────────────────
// Self — Identity Bootstrap
// ────────────────────────────────────────────────────────────────

// init registers the identity package itself. The registry knowing itself
// is the bootstrap anchor — like Genesis 1:1, it IS the foundation AND
// declares its own existence. Self-awareness starts with self-registration.
func init() {
	Register(_pragma["P1.key"], _pragma, _metadata)
}

// Pragma returns this package's own OmniCode pragma identity (P1-P5).
func Pragma() map[string]string { return _pragma }

// Metadata returns this package's own OmniCode metadata (M1-M10).
func Metadata() map[string]string { return _metadata }

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

// byMetadata filters packages by exact match on a metadata key.
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

// copyMap returns a shallow copy of a string map.
// Prevents callers from mutating registered identity after the fact.
func copyMap(m map[string]string) map[string]string {
	c := make(map[string]string, len(m))
	maps.Copy(c, m)
	return c
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// The identity registry is the system's self-awareness layer.
//
// Every OmniCode Go package follows the same pattern:
//
//   1. Declare _pragma and _metadata as package-level var maps
//   2. Export Pragma() and Metadata() accessor functions
//   3. Register in init():
//
//        func init() {
//            identity.Register(_pragma["P1.key"], _pragma, _metadata)
//        }
//
// The registry self-registers in init() — the bootstrap anchor.
// Like Genesis 1:1, it IS the foundation AND declares its own existence.
//
// Query paths:
//
//   identity.Lookup("B-L0-config-loader")  → single package by key
//   identity.All()                          → every registered package
//   identity.ByLayer("L0-universal")        → all L0 packages
//   identity.ByDomain("config-loading")     → all config packages
//   identity.ByTag("tripwire")              → all packages with that tag
//   identity.Count()                        → how many packages registered
//
// "And God called the light Day, and the darkness he called Night."
// — Genesis 1:5
//
// Naming establishes identity. Registration declares existence.
//
// ============================================================================
// END CLOSING
// ============================================================================
