//omni:code --go -library
//omni:key B-L0-config-util
//omni:version b-01.00
//omni:pragma _pragma
//omni:metadata _metadata

// ============================================================================
// METADATA
// ============================================================================

// Package util provides shared configuration-loading primitives for the
// L0-universal config layer.
//
// Extracted from 4 independent loader packages that had duplicated the same
// error types, file existence checks, TOML/JSONC loading, and tripwire
// patterns. Single source of truth for all of them now.
//
// This package is L0-universal — no OmniCode, no CPI-SI, no L1+ dependencies.
// Any Go project could use it.
//
// # Ladder (what stands on what)
//
// Public API functions build on internal helpers and foundation types:
//
//	Public APIs     LoadTOMLFile, LoadJSONCFile, FindAndLoad, Must, FormatFromPath
//	  ↓ calls
//	Internal        ParseJSONC, FindConfig, FileExists, DirExists, RootGuard.Check
//	  ↓ uses
//	Foundation      LoadError, ValidationError, RootGuard struct, StandardFormats
//
// # Baton (how work flows between files)
//
//	TOML:   caller → LoadTOMLFile(path) → FileExists? → toml.DecodeFile → *T
//	JSONC:  caller → LoadJSONCFile(path) → FileExists? → ReadFile → ParseJSONC → Unmarshal → *T
//	Search: caller → FindAndLoad(name, paths, v) → FindConfig → LoadFile → decoded
//	Root:   caller → NewRootGuard(name) → Set(path) → Check() → Path()
//	Error:  any    → NewLoadError(file, op, err) → *LoadError → errors.As chain
//	Alarm:  any    → TripwireBanner(title, lines) → stdout
//
// # Files
//
//	errors.go   — LoadError, ValidationError types + constructors
//	exists.go   — FileExists, DirExists
//	formats.go  — StandardFormats registry, format detection from extensions
//	jsonc.go    — JSONC parser (comment stripping, trailing commas)
//	loader.go   — TOML loading, config search paths
//	must.go     — Must[T] generic panic-on-error wrapper
//	root.go     — RootGuard (set-once root directory with thread safety)
//	tripwire.go — TripwireBanner, TripwireWarning (loud fallback alerts)
//
// See the parent config package for how these primitives compose into the
// manifest-driven loader.
package util

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- I.1 Standard Library [IMPORT] (1) ---
// Map operations
import "maps" // Copy — replaces manual map copy loops

//--- I.3 Internal Packages [IMPORT] (0) ---
// L0 identity registry — package self-awareness
import (
	"creativeworkzstudio.com/bereshit/L0-universal/spiral/identity" // Register — package identity registry
)

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

// _pragma — OmniCode package identity (P1-P5). Runtime-queryable.
var _pragma = map[string]string{
	// P1: Core Identity
	"P1.key":    "B-L0-config-util",
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
	"P5.title":   "Config Util Package",
	"P5.summary": "Shared configuration-loading primitives for L0-universal",
}

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

// _metadata — OmniCode package metadata (M1-M10). Runtime-queryable.
var _metadata = map[string]string{

	// ── M1: Core Identity ─────────────────────────────────────────
	"M1.key":            "B-L0-config-util",
	"M1.component_type": "Foundation",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2026-02-13",

	// ── M2: Version History ───────────────────────────────────────
	"M2.b-01.00": "2026-02-13 — BASELINE: 8 files extracted from 4 loader packages, production-grade",
	"M2.a-01.00": "2026-02-13 — Initial extraction, basic working code",

	// ── M3: Interface ─────────────────────────────────────────────
	"M3.requires.stdlib":   "encoding/json, fmt, os, path/filepath, regexp, strings, sync",
	"M3.formats":           "TOML (.toml), JSONC (.json, .jsonc) — dispatches by extension",
	"M3.requires.external": "github.com/BurntSushi/toml v1.6.0",
	"M3.requires.internal": "(none — L0 foundation, no internal deps)",
	"M3.used_by":           "L0-universal/hybrid/config (loader), L3-cpisi/orchestration/config, L3-cpisi/foundation/schema",
	"M3.import":            "creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util",
	"M3.pattern":           "util.NewLoadError(file, op, err), util.FileExists(path), util.Must(val, err)",

	// ── M4: Public API ────────────────────────────────────────────
	"M4.errors":   "NewLoadError, NewValidationError, LoadError, ValidationError",
	"M4.exists":   "FileExists, DirExists",
	"M4.formats":  "FormatFromExt, FormatFromPath, IsConfigFormat, IsConfigPath, IsKnownExt",
	"M4.jsonc":    "ParseJSONC, LoadJSONCFile, LoadJSONCMap",
	"M4.loader":   "LoadConfigFile, LoadFile, LoadBytes, LoadTOMLFile, FindAndLoad, FindConfig, FindConfigDir",
	"M4.must":     "Must",
	"M4.root":     "NewRootGuard, RootGuard.Set, RootGuard.Check, RootGuard.Path",
	"M4.tripwire": "TripwireBanner, TripwireWarning",

	// ── M5: Operational ───────────────────────────────────────────
	"M5.blocking": "no — file I/O only, no network",
	"M5.health":   "Foundation | granted: loaded | deferred: n/a | denied: error",

	// ── M6: Classification ────────────────────────────────────────
	"M6.tags":     "config, util, toml, jsonc, errors, existence, formats, tripwire",
	"M6.category": "Foundation",
	"M6.domain":   "config-loading",
	"M6.layer":    "L0-universal",
	"M6.paradigm": "CPI-SI",

	// ── M7: Intent ────────────────────────────────────────────────
	"M7.purpose":    "Eliminate duplication across config loaders — single source of truth for shared primitives",
	"M7.philosophy": "Extract once, use everywhere. L0 stays universal — no OmniCode, no L1+ deps",
	"M7.provides":   "Error types, file checks, format detection, TOML/JSONC loading, root guards, tripwire alerts",

	// ── M8: Grounding ─────────────────────────────────────────────
	"M8.scripture": "1 Corinthians 12:12 — For as the body is one, and hath many members",
	"M8.principle": "Many files, one purpose. Each util serves the whole, none stands alone",
	"M8.anchor":    "Genesis 1:4 — God divided the light from the darkness",

	// ── M9: Dependencies ──────────────────────────────────────────
	"M9.needs.stdlib":   "encoding/json, fmt, os, path/filepath, regexp, strings, sync",
	"M9.needs.external": "github.com/BurntSushi/toml v1.6.0",
	"M9.needs.internal": "(none)",
	"M9.used_by":        "L0-universal/hybrid/config, L3-cpisi/orchestration/config, L3-cpisi/foundation/schema",
	"M9.layer_deps":     "L0 only — no L1+ dependencies",

	// ── M10: Roadmap ──────────────────────────────────────────────
	"M10.current":     "b-01.00 — BASELINE: extracted, production-grade, tested",
	"M10.planned":     "YAML loading, schema validation helpers",
	"M10.limitations": "File I/O only, no network, no streaming, no watch/hot-reload",
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// doc.go contains no setup — package identity only.
// Each util file has its own imports and types.

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Identity — Package Self-Registration
// ────────────────────────────────────────────────────────────────

// init registers this package in the OmniCode identity registry.
// Runs automatically when any code imports this package.
func init() {
	identity.Register(_pragma["P1.key"], _pragma, _metadata)
}

// Pragma returns the OmniCode pragma map for the util package.
func Pragma() map[string]string {
	cp := make(map[string]string, len(_pragma))
	maps.Copy(cp, _pragma)
	return cp
}

// Metadata returns the OmniCode metadata map for the util package.
func Metadata() map[string]string {
	cp := make(map[string]string, len(_metadata))
	maps.Copy(cp, _metadata)
	return cp
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// APU Inventory:
//   Total: 8 files, ~830 lines
//   - Error types:      2 types + 2 constructors (errors.go)
//   - Existence:        2 functions (exists.go)
//   - Formats:          2 maps + 5 functions (formats.go)
//   - JSONC:            3 functions (jsonc.go)
//   - TOML:             6 functions (loader.go)
//   - Generic:          1 function (must.go)
//   - Root guard:       1 type + 3 methods + 1 constructor (root.go)
//   - Tripwire:         2 functions (tripwire.go)
//   - Identity:         2 functions + 2 maps (doc.go)
//
// Ladder:
//   Public APIs → Internal helpers → Foundation types
//   (see package doc above for full breakdown)
//
// Baton:
//   Each file is mostly standalone — the internal dependencies are:
//     loader.go → exists.go (FileExists), errors.go (NewLoadError)
//     jsonc.go  → exists.go (FileExists), errors.go (NewLoadError)
//     All others: standalone, no internal util deps
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This package is FOUNDATION — everything builds on it, nothing it
// builds on (within the project). L0 universal. The body has many
// members, but they are one body.
//
// "For as the body is one, and hath many members, and all the
// members of that one body, being many, are one body: so also
// is Christ." — 1 Corinthians 12:12
//
// ============================================================================
// END CLOSING
// ============================================================================
