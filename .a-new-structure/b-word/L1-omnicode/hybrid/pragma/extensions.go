// #!omni code --go -library
// #!omni meta.key = B-L1-omnicode-hybrid-pragma-extensions
// #!omni meta.from = B-L1-language-extensions
// #!omni meta.at = a-01.00
//omni:code --go -library
//omni:key B-L1-omnicode-hybrid-pragma-extensions
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// OmniCode File Extension Registry — The Filesystem Speaks
//
// Key:     B-L1-omnicode-hybrid-pragma-extensions
// Purpose: Go types and registry for OmniCode file extensions
// Biblical: Genesis 1:5 - "And God called the light Day, and the darkness
//           he called Night." Naming establishes identity.
// Authors: Nova Dawn
// Version: a-01.00
// Created: 2026-02-13
//
// Derived from: L1-omnicode/ladder/language/extensions.toml (canonical spec)
//
// SPEC-DRIVEN DESIGN:
//   The extension registry loads from extensions.toml at runtime using L0's
//   TOML loader (util.LoadTOMLFile). The spec IS the truth. Go code is
//   derived — it reads the TOML and builds the registry from it.
//
//   Fallback: If the spec file isn't on disk (test environments, standalone
//   builds), a hardcoded default registry provides the same data. But the
//   spec is always preferred.
//
//   Why: Because this is what the L0 loaders EXIST for. L1 should use L0
//   as a library. The TOML spec drives the runtime.
//
// Layer vocabulary alignment:
//   L0 — trit operations (bless, curse, balance)
//   L1 — keywords (let, be, covenant, granted)
//   L2 — file extensions (.omni, .gen, .rev, .witness) ← this file
//   L3 — state commands (AWAIT, PROCEED, CLEAR, PIVOT, HALT)
//
// ============================================================================
// END METADATA
// ============================================================================

package pragma

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"
	"path/filepath"
	"strings"
	"sync"

	// L0 shared utilities — the loaders that exist to be used
	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util"
	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/paths"
)

// --- Extension Category ---

// Category classifies an extension into one of three families.
type Category string

const (
	// CatOmniSuite — system and infrastructure extensions (.omni, .ofd, .omc).
	// The bones of the ecosystem. Omni-branded.
	CatOmniSuite Category = "omni-suite"

	// CatBiblical — operational extensions where the name IS the operation.
	// .gen creates (Genesis), .rev reveals (Revelation), .witness attests.
	CatBiblical Category = "biblical"

	// CatCognitive — state and awareness extensions for the CPI-SI system.
	// .health scores, .halts anchors, .cube positions.
	CatCognitive Category = "cognitive"
)

// --- State Machine Mapping ---

// StateMapping connects extensions to state machine commands.
type StateMapping string

const (
	StateAwait   StateMapping = "AWAIT"
	StateProceed StateMapping = "PROCEED"
	StateClear   StateMapping = "CLEAR"
	StatePivot   StateMapping = "PIVOT"
	StateHalt    StateMapping = "HALT"
)

// --- Extension Definition ---

// Extension holds the full identity of an OmniCode file extension.
// Mirrors the structure in extensions.toml — TOML is truth, Go is derived.
type Extension struct {
	Ext            string       // ".omni", ".gen", etc.
	Name           string       // Human name: "OmniCode Source", "Genesis"
	Category       Category     // omni-suite, biblical, cognitive
	Phase          string       // "a" (foundation) or "b" (expansion)
	BiblicalRoot   string       // Scripture reference
	Operation      string       // What the extension DOES: "source", "encode", "decode"
	Description    string       // Human-readable purpose
	InternalFormat Format       // What format the content actually uses (toml, jsonc, omni, etc.)
	BlockPattern   string       // "3-block", "4-block", "varies", "none"
	DefaultType    Type         // Default pragma type for this extension
	MIMEType       string       // MIME type for content negotiation
	StateMap       StateMapping // Which state machine command this maps to
}

// --- Extension Constants ---

const (
	// Omni-Suite
	ExtOmniConfig    = ".omc"
	ExtOmniInterface = ".omi"
	ExtOmniManifest  = ".omx"

	// Biblical
	ExtGenesis  = ".gen"
	ExtReveal   = ".rev"
	ExtWitness  = ".witness"
	ExtCovenant = ".covenant"
	ExtScroll   = ".scroll"
	ExtSeed     = ".seed"
	ExtTov      = ".tov"

	// Cognitive
	ExtHealth = ".health"
	ExtHalts  = ".halts"
	ExtCube   = ".cube"
)

// Note: ExtOmni and ExtOmniFold are already defined in pragma.go

// --- Spec Types (TOML Schema) ---

// extSpec mirrors a single extension section in extensions.toml.
// BurntSushi/toml ignores extra fields (keyword_link, l0_parallel, etc.)
// so only the 12 core fields are captured.
type extSpec struct {
	Ext            string `toml:"ext"`
	Name           string `toml:"name"`
	Category       string `toml:"category"`
	Phase          string `toml:"phase"`
	BiblicalRoot   string `toml:"biblical_root"`
	Operation      string `toml:"operation"`
	Description    string `toml:"description"`
	InternalFormat string `toml:"internal_format"`
	BlockPattern   string `toml:"block_pattern"`
	DefaultType    string `toml:"default_type"`
	MIMEType       string `toml:"mime_type"`
	StateMapping   string `toml:"state_mapping"`
}

// extensionsSpec is the full shape of extensions.toml's content sections.
// _pragma, _metadata, _content, _validation, _closing sections are silently
// ignored by BurntSushi/toml since they have no matching struct fields.
type extensionsSpec struct {
	Omni      map[string]extSpec `toml:"omni"`
	Biblical  map[string]extSpec `toml:"biblical"`
	Cognitive map[string]extSpec `toml:"cognitive"`

	FormatBridge struct {
		Description   string            `toml:"description"`
		Mappings      map[string]string `toml:"mappings"`
		BlockPatterns map[string]string `toml:"block_patterns"`
	} `toml:"format_bridge"`

	Lookup struct {
		PhaseA     specLookupPhase    `toml:"phase_a"`
		PhaseB     specLookupPhase    `toml:"phase_b"`
		All        specLookupPhase    `toml:"all"`
		ByCategory specLookupCategory `toml:"by_category"`
		ByState    specLookupState    `toml:"by_state"`
	} `toml:"lookup"`
}

type specLookupPhase struct {
	Extensions []string `toml:"extensions"`
	Count      int      `toml:"count"`
}

type specLookupCategory struct {
	OmniSuite []string `toml:"omni_suite"`
	Biblical  []string `toml:"biblical"`
	Cognitive []string `toml:"cognitive"`
}

type specLookupState struct {
	Await   []string `toml:"await"`
	Proceed []string `toml:"proceed"`
	Clear   []string `toml:"clear"`
	Pivot   []string `toml:"pivot"`
	Halt    []string `toml:"halt"`
}

// --- Registry State ---

var (
	registryOnce   sync.Once
	activeRegistry map[string]*Extension
	specLoaded     bool // true if loaded from TOML, false if using defaults
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Spec Loading — L0 Loaders in Action
// ────────────────────────────────────────────────────────────────

// getRegistry returns the active extension registry, initializing it once.
// Tries spec-driven loading first, falls back to hardcoded defaults.
func getRegistry() map[string]*Extension {
	registryOnce.Do(func() {
		specPath := resolveExtensionsSpec()
		if specPath != "" {
			if reg, err := loadFromSpec(specPath); err == nil && len(reg) > 0 {
				activeRegistry = reg
				specLoaded = true
				return
			}
		}

		// Fallback: hardcoded defaults for environments without the spec file
		activeRegistry = defaultRegistry()
		specLoaded = false
	})
	return activeRegistry
}

// IsSpecLoaded returns true if the registry was loaded from extensions.toml.
// Returns false if using hardcoded fallback defaults.
// Calling this triggers registry initialization if it hasn't happened yet.
func IsSpecLoaded() bool {
	getRegistry() // ensure initialized
	return specLoaded
}

// resolveExtensionsSpec finds the extensions.toml spec file on disk.
// Uses L0 paths.BereshitRoot() to resolve the project root, then constructs
// the known path to the spec file.
func resolveExtensionsSpec() string {
	root := paths.BereshitRoot()
	specPath := filepath.Join(root, ".a-new-structure", "b-word",
		"L1-omnicode", "ladder", "language", "extensions.toml")

	if _, err := os.Stat(specPath); err == nil {
		return specPath
	}
	return ""
}

// loadFromSpec loads the extension registry from extensions.toml using
// L0's TOML loader. THIS IS THE POINT — the L0 loaders exist to be used.
// The spec drives the runtime.
func loadFromSpec(path string) (map[string]*Extension, error) {
	spec, err := util.LoadTOMLFile[extensionsSpec](path)
	if err != nil {
		return nil, err
	}

	reg := make(map[string]*Extension)

	// Process all three categories — the TOML structure maps directly:
	//   [omni.source]    → spec.Omni["source"]
	//   [biblical.gen]   → spec.Biblical["gen"]
	//   [cognitive.cube] → spec.Cognitive["cube"]
	for _, es := range spec.Omni {
		reg[es.Ext] = specToExtension(&es)
	}
	for _, es := range spec.Biblical {
		reg[es.Ext] = specToExtension(&es)
	}
	for _, es := range spec.Cognitive {
		reg[es.Ext] = specToExtension(&es)
	}

	return reg, nil
}

// specToExtension converts a TOML spec entry into a runtime Extension.
// Type conversions are direct — the string values in TOML match Go constants.
func specToExtension(es *extSpec) *Extension {
	return &Extension{
		Ext:            es.Ext,
		Name:           es.Name,
		Category:       Category(es.Category),
		Phase:          es.Phase,
		BiblicalRoot:   es.BiblicalRoot,
		Operation:      es.Operation,
		Description:    es.Description,
		InternalFormat: Format(es.InternalFormat),
		BlockPattern:   es.BlockPattern,
		DefaultType:    Type(es.DefaultType),
		MIMEType:       es.MIMEType,
		StateMap:       parseStateMapping(es.StateMapping),
	}
}

// parseStateMapping extracts the command from a TOML state mapping string.
// The spec format is "COMMAND — description" (em dash separator).
// Example: "PROCEED — active content, ready for processing" → StateProceed
func parseStateMapping(s string) StateMapping {
	// Split on em dash — the TOML spec uses " — " as separator
	if idx := strings.Index(s, " — "); idx >= 0 {
		s = s[:idx]
	}
	s = strings.TrimSpace(s)

	switch StateMapping(s) {
	case StateAwait:
		return StateAwait
	case StateProceed:
		return StateProceed
	case StateClear:
		return StateClear
	case StatePivot:
		return StatePivot
	case StateHalt:
		return StateHalt
	default:
		return StateProceed // Safe default — most extensions proceed
	}
}

// ────────────────────────────────────────────────────────────────
// Lookup Functions
// ────────────────────────────────────────────────────────────────

// LookupExt returns the extension definition for a given file extension.
// Returns nil, false if the extension is not registered.
func LookupExt(ext string) (*Extension, bool) {
	e, ok := getRegistry()[ext]
	return e, ok
}

// IsRegisteredExt returns true if the extension is in the OmniCode registry.
func IsRegisteredExt(ext string) bool {
	_, ok := getRegistry()[ext]
	return ok
}

// ExtsByCategory returns all extensions belonging to a category.
func ExtsByCategory(cat Category) []*Extension {
	var result []*Extension
	for _, e := range getRegistry() {
		if e.Category == cat {
			result = append(result, e)
		}
	}
	return result
}

// ExtsByPhase returns all extensions in a given phase ("a" or "b").
func ExtsByPhase(phase string) []*Extension {
	var result []*Extension
	for _, e := range getRegistry() {
		if e.Phase == phase {
			result = append(result, e)
		}
	}
	return result
}

// ExtsByState returns all extensions mapped to a state machine command.
func ExtsByState(state StateMapping) []*Extension {
	var result []*Extension
	for _, e := range getRegistry() {
		if e.StateMap == state {
			result = append(result, e)
		}
	}
	return result
}

// AllExts returns every registered extension.
func AllExts() []*Extension {
	reg := getRegistry()
	result := make([]*Extension, 0, len(reg))
	for _, e := range reg {
		result = append(result, e)
	}
	return result
}

// ────────────────────────────────────────────────────────────────
// Format Bridge
// ────────────────────────────────────────────────────────────────

// InternalFormatForExt returns the internal format for an OmniCode extension.
// For example, ".ofd" → FormatTOML, ".witness" → FormatJSONC.
// Returns empty string if extension is not registered.
func InternalFormatForExt(ext string) Format {
	e, ok := getRegistry()[ext]
	if !ok {
		return ""
	}
	return e.InternalFormat
}

// BlockPatternForExt returns the expected block structure for an extension.
// Returns empty string if extension is not registered.
func BlockPatternForExt(ext string) string {
	e, ok := getRegistry()[ext]
	if !ok {
		return ""
	}
	return e.BlockPattern
}

// DefaultTypeForExt returns the default pragma type for an extension.
// Returns empty string if extension is not registered.
func DefaultTypeForExt(ext string) Type {
	e, ok := getRegistry()[ext]
	if !ok {
		return ""
	}
	return e.DefaultType
}

// ────────────────────────────────────────────────────────────────
// Default Registry (Fallback)
// ────────────────────────────────────────────────────────────────

// defaultRegistry returns the hardcoded extension registry.
// Used when extensions.toml is not available on disk (test environments,
// standalone builds, environments without the spec file).
//
// These values mirror extensions.toml. If the spec changes, update these too.
// But the spec should always be preferred — this is the safety net, not truth.
func defaultRegistry() map[string]*Extension {
	return map[string]*Extension{

		// --- Omni-Suite ---

		ExtOmni: {
			Ext: ExtOmni, Name: "OmniCode Source", Category: CatOmniSuite, Phase: "a",
			BiblicalRoot: "Genesis 1:3", Operation: "source",
			Description:    "General-purpose OmniCode source file",
			InternalFormat: FormatOmni, BlockPattern: "varies", DefaultType: TypeCode,
			MIMEType: "text/x-omnicode", StateMap: StateProceed,
		},
		ExtOmniFold: {
			Ext: ExtOmniFold, Name: "OmniFold", Category: CatOmniSuite, Phase: "a",
			BiblicalRoot: "Genesis 1:9", Operation: "folder",
			Description:    "Folder metadata — declares what a directory IS",
			InternalFormat: FormatTOML, BlockPattern: "3-block", DefaultType: TypeFolder,
			MIMEType: "application/x-omnifold+toml", StateMap: StateProceed,
		},
		ExtOmniConfig: {
			Ext: ExtOmniConfig, Name: "OmniConfig", Category: CatOmniSuite, Phase: "a",
			BiblicalRoot: "Proverbs 3:5-6", Operation: "config",
			Description:    "Configuration file — settings, preferences, build options",
			InternalFormat: FormatTOML, BlockPattern: "3-block", DefaultType: TypeData,
			MIMEType: "application/x-omniconfig+toml", StateMap: StateProceed,
		},
		ExtOmniInterface: {
			Ext: ExtOmniInterface, Name: "OmniInterface", Category: CatOmniSuite, Phase: "b",
			BiblicalRoot: "Exodus 19:5", Operation: "interface",
			Description:    "Interface definition — API contracts, covenant surfaces",
			InternalFormat: FormatOmni, BlockPattern: "4-block", DefaultType: TypeInterface,
			MIMEType: "text/x-omniinterface", StateMap: StateAwait,
		},
		ExtOmniManifest: {
			Ext: ExtOmniManifest, Name: "OmniManifest", Category: CatOmniSuite, Phase: "b",
			BiblicalRoot: "Numbers 1:2", Operation: "manifest",
			Description:    "Package manifest — dependencies, exports, versioning",
			InternalFormat: FormatTOML, BlockPattern: "3-block", DefaultType: TypeData,
			MIMEType: "application/x-omnimanifest+toml", StateMap: StateProceed,
		},

		// --- Biblical ---

		ExtGenesis: {
			Ext: ExtGenesis, Name: "Genesis", Category: CatBiblical, Phase: "a",
			BiblicalRoot: "Genesis 1:1", Operation: "encode",
			Description:    "Encoding/generation output — creation in binary form",
			InternalFormat: FormatBinary, BlockPattern: "none", DefaultType: TypeData,
			MIMEType: "application/x-omni-genesis", StateMap: StateProceed,
		},
		ExtReveal: {
			Ext: ExtReveal, Name: "Revelation", Category: CatBiblical, Phase: "a",
			BiblicalRoot: "Revelation 1:1", Operation: "decode",
			Description:    "Decoding/revelation — encoded content made readable",
			InternalFormat: FormatText, BlockPattern: "none", DefaultType: TypeData,
			MIMEType: "text/x-omni-revelation", StateMap: StateProceed,
		},
		ExtWitness: {
			Ext: ExtWitness, Name: "Witness", Category: CatBiblical, Phase: "a",
			BiblicalRoot: "Deuteronomy 19:15", Operation: "attest",
			Description:    "Attestation — checksums, signatures, verification records",
			InternalFormat: FormatJSONC, BlockPattern: "3-block", DefaultType: TypeData,
			MIMEType: "application/x-omni-witness+json", StateMap: StateHalt,
		},
		ExtCovenant: {
			Ext: ExtCovenant, Name: "Covenant", Category: CatBiblical, Phase: "a",
			BiblicalRoot: "Genesis 9:9", Operation: "bind",
			Description:    "Agreement files — licenses, covenants, terms of service",
			InternalFormat: FormatTOML, BlockPattern: "3-block", DefaultType: TypeData,
			MIMEType: "application/x-omni-covenant+toml", StateMap: StateProceed,
		},
		ExtScroll: {
			Ext: ExtScroll, Name: "Scroll", Category: CatBiblical, Phase: "b",
			BiblicalRoot: "Revelation 5:1", Operation: "log",
			Description:    "Event logs and audit trails — append-only history",
			InternalFormat: FormatJSONC, BlockPattern: "3-block", DefaultType: TypeData,
			MIMEType: "application/x-omni-scroll+json", StateMap: StateProceed,
		},
		ExtSeed: {
			Ext: ExtSeed, Name: "Seed", Category: CatBiblical, Phase: "a",
			BiblicalRoot: "Genesis 1:11", Operation: "template",
			Description:    "Template and generative patterns — seeds yield fruit when planted",
			InternalFormat: FormatOmni, BlockPattern: "varies", DefaultType: TypeTemplate,
			MIMEType: "text/x-omni-seed", StateMap: StateAwait,
		},
		ExtTov: {
			Ext: ExtTov, Name: "Tov", Category: CatBiblical, Phase: "a",
			BiblicalRoot: "Genesis 1:31", Operation: "complete",
			Description:    "Completion marker — tov (good), God's assessment after creation",
			InternalFormat: FormatTOML, BlockPattern: "3-block", DefaultType: TypeData,
			MIMEType: "application/x-omni-tov+toml", StateMap: StateHalt,
		},

		// --- Cognitive ---

		ExtHealth: {
			Ext: ExtHealth, Name: "Health", Category: CatCognitive, Phase: "b",
			BiblicalRoot: "Leviticus 19:36", Operation: "score",
			Description:    "Health state scoring — ternary scale (-100 to +100)",
			InternalFormat: FormatTOML, BlockPattern: "3-block", DefaultType: TypeData,
			MIMEType: "application/x-omni-health+toml", StateMap: StateClear,
		},
		ExtHalts: {
			Ext: ExtHalts, Name: "Halts", Category: CatCognitive, Phase: "b",
			BiblicalRoot: "Genesis 1:1", Operation: "anchor",
			Description:    "Anchor definitions — where recursion stops",
			InternalFormat: FormatTOML, BlockPattern: "3-block", DefaultType: TypeData,
			MIMEType: "application/x-omni-halts+toml", StateMap: StateHalt,
		},
		ExtCube: {
			Ext: ExtCube, Name: "Cube", Category: CatCognitive, Phase: "b",
			BiblicalRoot: "Revelation 21:16", Operation: "position",
			Description:    "Positional state — 27-position cube coordinates",
			InternalFormat: FormatTOML, BlockPattern: "3-block", DefaultType: TypeData,
			MIMEType: "application/x-omni-cube+toml", StateMap: StatePivot,
		},
	}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// The extension registry connects the filesystem to OmniCode identity.
// When a file is encountered:
//
//   filepath.Ext(path)            → ".gen"
//   LookupExt(".gen")             → Extension{Name: "Genesis", Operation: "encode", ...}
//   ext.InternalFormat             → FormatBinary
//   ext.StateMap                   → PROCEED
//   ext.DefaultType                → TypeData
//
// The chain from extension to identity to format to state:
//   L2 (extension) → L1 (pragma type) → L0 (format parser) → L3 (state command)
//
// All four layers saying the same truth in their own voice.
//
// SPEC-DRIVEN: The registry loads from extensions.toml using L0's TOML loader.
// The TOML spec is truth. Go code is derived. If the spec isn't available,
// hardcoded defaults provide the same data as a safety net.
//
// ============================================================================
// END CLOSING
// ============================================================================
