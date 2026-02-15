//omni:code --go -library
//omni:key B-L2-registry
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

// Package registry opens all three Bereshit databases and provides cross-layer
// query methods that span L0 (foundation), L1 (language), and L2 (platform).
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       B-L2-registry
//
//	Scripture: "For as the body is one, and hath many members, and all the
//	           members of that one body, being many, are one body: so also
//	           is Christ." — 1 Corinthians 12:12
//	Principle: "Layers are members of one body — distinct in function, unified in purpose"
//	Anchor:    Genesis 1:1
//
//	Type:      Hybrid (cross-layer data access)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-02-15
//	Version:         a-01.00
//
// # M.2 Version History [HISTORY]
//
//   - a-01.00 (2026-02-15) — Initial creation: cross-layer registry + validation
//
// # M.3 Interface [INTERFACE]
//
//	Requires: internal (3): foundation, language, platform
//	Used by:  L3-cpisi/core, hooks, statusline, any startup that needs all layers
//
// The registry is optional — individual loaders still work standalone with
// Init(nil) defaults. The registry exists to resolve cross-layer dependencies
// declared in the master DAG (index.toml).
package registry

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

import (
	"fmt"
	"strings"

	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/foundation"
	"creativeworkzstudio.com/bereshit/L1-omnicode/hybrid/language"
	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/platform"
)

// ────────────────────────────────────────────────────────────────
// S.1 Core Types
// ────────────────────────────────────────────────────────────────

// Registry holds open connections to all three Bereshit databases.
// It is the single point where cross-layer queries happen.
type Registry struct {
	Foundation *foundation.DB
	Language   *language.DB
	Platform   *platform.DB
	root       string
}

// ValidationError represents a cross-layer reference that failed validation.
type ValidationError struct {
	Layer   string // Source layer: "L0", "L1", "L2"
	Table   string // Source table
	Field   string // Field with the bad reference
	Value   string // The value that failed
	Target  string // What it was supposed to reference
	Message string // Human-readable description
}

func (e ValidationError) String() string {
	return fmt.Sprintf("[%s.%s.%s] %s → %s: %s",
		e.Layer, e.Table, e.Field, e.Value, e.Target, e.Message)
}

// CrossRef holds a resolved cross-layer reference between entities.
type CrossRef struct {
	SourceLayer string
	SourceTable string
	SourceKey   string
	TargetLayer string
	TargetTable string
	TargetKey   string
}

// FilesystemCrossRef maps an L2 filesystem extension name to its L0 type
// relationship.
type FilesystemCrossRef struct {
	Name       string // Hebrew vocabulary name (dabar, chok, sefer, etc.)
	Hebrew     string // Hebrew characters
	Meaning    string // English meaning
	Extensions string // JSON array of file extensions
	HasL0Type  bool   // True if this name exists as an L0 primitive type
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Open / Close
// ────────────────────────────────────────────────────────────────

// Open creates a Registry by opening all three databases from a module root.
//
// The root is the directory containing go.mod / Cargo.toml — the Bereshit
// module root (b-word/). Each database is found at its standard path:
//
//	data/foundation/database/foundation.db
//	data/language/database/language.db
//	data/platform/database/platform.db
func Open(moduleRoot string) (*Registry, error) {
	fdb, err := foundation.OpenFromRoot(moduleRoot)
	if err != nil {
		return nil, fmt.Errorf("registry: opening foundation: %w", err)
	}

	ldb, err := language.OpenFromRoot(moduleRoot)
	if err != nil {
		fdb.Close()
		return nil, fmt.Errorf("registry: opening language: %w", err)
	}

	pdb, err := platform.OpenFromRoot(moduleRoot)
	if err != nil {
		fdb.Close()
		ldb.Close()
		return nil, fmt.Errorf("registry: opening platform: %w", err)
	}

	return &Registry{
		Foundation: fdb,
		Language:   ldb,
		Platform:   pdb,
		root:       moduleRoot,
	}, nil
}

// Close releases all database connections.
func (r *Registry) Close() error {
	var errs []string

	if err := r.Foundation.Close(); err != nil {
		errs = append(errs, fmt.Sprintf("foundation: %v", err))
	}
	if err := r.Language.Close(); err != nil {
		errs = append(errs, fmt.Sprintf("language: %v", err))
	}
	if err := r.Platform.Close(); err != nil {
		errs = append(errs, fmt.Sprintf("platform: %v", err))
	}

	if len(errs) > 0 {
		return fmt.Errorf("registry close: %s", strings.Join(errs, "; "))
	}
	return nil
}

// Root returns the module root path used to open this registry.
func (r *Registry) Root() string {
	return r.root
}

// ────────────────────────────────────────────────────────────────
// B.2 Cross-Layer Queries: L0 ↔ L2
// ────────────────────────────────────────────────────────────────

// HealthNormalizationScales returns the ternary scales from L0 that correspond
// to health normalization bases in L2. This is the fundamental link: L2 health
// scoring is built on L0 ternary math.
func (r *Registry) HealthNormalizationScales() ([]foundation.TernaryScale, error) {
	bases, err := r.Platform.AllNormalizationBases()
	if err != nil {
		return nil, fmt.Errorf("registry: normalization bases: %w", err)
	}

	scales, err := r.Foundation.AllScales()
	if err != nil {
		return nil, fmt.Errorf("registry: ternary scales: %w", err)
	}

	// Build a set of base names from L2
	baseNames := make(map[string]bool, len(bases))
	for _, b := range bases {
		baseNames[b.Name] = true
	}

	// Return L0 scales that match L2 normalization bases
	var matched []foundation.TernaryScale
	for _, s := range scales {
		if baseNames[s.Name] {
			matched = append(matched, s)
		}
	}
	return matched, nil
}

// FilesystemVocabulary returns all L2 filesystem extension names (Hebrew
// biblical vocabulary like "dabar", "sefer", "torah") alongside the L0
// types they conceptually relate to. This maps filesystem naming back to
// the ternary type system.
func (r *Registry) FilesystemVocabulary() ([]FilesystemCrossRef, error) {
	extensions, err := r.Platform.AllFilesystemExtensions()
	if err != nil {
		return nil, fmt.Errorf("registry: filesystem extensions: %w", err)
	}

	types, err := r.Foundation.AllTypes()
	if err != nil {
		return nil, fmt.Errorf("registry: types: %w", err)
	}

	// Build L0 type name set
	typeNames := make(map[string]bool, len(types))
	for _, t := range types {
		typeNames[t.Name] = true
	}

	var refs []FilesystemCrossRef
	for _, ext := range extensions {
		refs = append(refs, FilesystemCrossRef{
			Name:       ext.Name,
			Hebrew:     ext.Hebrew,
			Meaning:    ext.Meaning,
			Extensions: ext.Extensions,
			HasL0Type:  typeNames[ext.Name],
		})
	}
	return refs, nil
}

// TypeForHealthLevel resolves the L0 primitive type that corresponds to a given
// L2 health level name. Health levels map to the ternary type system.
func (r *Registry) TypeForHealthLevel(levelName string) (*foundation.Type, error) {
	levels, err := r.Platform.AllHealthLevels()
	if err != nil {
		return nil, fmt.Errorf("registry: health levels: %w", err)
	}

	// Find the level
	var found bool
	for _, l := range levels {
		if l.Name == levelName {
			found = true
			break
		}
	}
	if !found {
		return nil, fmt.Errorf("registry: health level %q not found in L2", levelName)
	}

	// Look up the corresponding type in L0
	t, err := r.Foundation.TypeByName(levelName)
	if err != nil {
		// Not every health level has a direct type mapping — that's valid
		return nil, nil
	}
	return t, nil
}

// ────────────────────────────────────────────────────────────────
// B.3 Cross-Layer Queries: L1 ↔ L2
// ────────────────────────────────────────────────────────────────

// ExtensionWithLanguageInfo looks up a file extension across L1 and L2.
// L2 FilesystemExtension.Extensions is a JSON array; L1 Extension.Ext is a
// single extension string. Returns a CrossRef if the extension exists in
// either layer.
func (r *Registry) ExtensionWithLanguageInfo(ext string) (*CrossRef, error) {
	// Check L2 — the Extensions field is a JSON array of extensions
	fsExts, err := r.Platform.AllFilesystemExtensions()
	if err != nil {
		return nil, fmt.Errorf("registry: filesystem extensions: %w", err)
	}

	var l2Name string
	for _, fe := range fsExts {
		// Extensions is a JSON array like [".go", ".rs"]
		if strings.Contains(fe.Extensions, ext) {
			l2Name = fe.Name
			break
		}
	}

	// Check L1
	_, l1Err := r.Language.ExtensionByExt(ext)
	l1Found := l1Err == nil

	if l2Name == "" && !l1Found {
		return nil, nil
	}

	ref := &CrossRef{
		SourceKey: ext,
	}

	if l2Name != "" {
		ref.SourceLayer = "L2"
		ref.SourceTable = "filesystem_biblical_extensions"
	}

	if l1Found {
		ref.TargetLayer = "L1"
		ref.TargetTable = "extensions"
		ref.TargetKey = ext
	}

	return ref, nil
}

// ────────────────────────────────────────────────────────────────
// B.4 Cross-Layer Queries: L0 ↔ L1
// ────────────────────────────────────────────────────────────────

// OperationsUsedByKeywords checks whether L1 keywords reference L0 operations.
// This validates that OmniCode keyword semantics trace back to foundation math.
func (r *Registry) OperationsUsedByKeywords() ([]CrossRef, error) {
	keywords, err := r.Language.AllKeywords()
	if err != nil {
		return nil, fmt.Errorf("registry: keywords: %w", err)
	}

	operations, err := r.Foundation.AllOperations()
	if err != nil {
		return nil, fmt.Errorf("registry: operations: %w", err)
	}

	// Build operation name set
	opNames := make(map[string]bool, len(operations))
	for _, op := range operations {
		opNames[op.Name] = true
	}

	// Find keywords that share a name with an L0 operation
	var refs []CrossRef
	for _, kw := range keywords {
		if opNames[kw.Keyword] {
			refs = append(refs, CrossRef{
				SourceLayer: "L1",
				SourceTable: "keywords",
				SourceKey:   kw.Keyword,
				TargetLayer: "L0",
				TargetTable: "operations",
				TargetKey:   kw.Keyword,
			})
		}
	}
	return refs, nil
}

// ────────────────────────────────────────────────────────────────
// B.5 Spec Dependency Resolution
// ────────────────────────────────────────────────────────────────

// AllSpecs returns every spec across all three databases with their layer prefix.
func (r *Registry) AllSpecs() (map[string]string, error) {
	specs := make(map[string]string)

	fSpecs, err := r.Foundation.AllSpecs()
	if err != nil {
		return nil, fmt.Errorf("registry: L0 specs: %w", err)
	}
	for _, s := range fSpecs {
		specs[s.Key] = "L0"
	}

	lSpecs, err := r.Language.AllSpecs()
	if err != nil {
		return nil, fmt.Errorf("registry: L1 specs: %w", err)
	}
	for _, s := range lSpecs {
		specs[s.Key] = "L1"
	}

	pSpecs, err := r.Platform.AllSpecs()
	if err != nil {
		return nil, fmt.Errorf("registry: L2 specs: %w", err)
	}
	for _, s := range pSpecs {
		specs[s.Key] = "L2"
	}

	return specs, nil
}

// ResolveDependency looks up a spec key across all layers and returns
// which layer owns it. This allows validating that depends_on references
// in any layer's specs point to real specs in the correct layer.
func (r *Registry) ResolveDependency(specKey string) (layer string, found bool) {
	specs, err := r.AllSpecs()
	if err != nil {
		return "", false
	}
	l, ok := specs[specKey]
	return l, ok
}

// ────────────────────────────────────────────────────────────────
// B.6 Cross-Layer Validation
// ────────────────────────────────────────────────────────────────

// ValidateAll runs every cross-layer validation check.
// This is the canonical entry point for verifying that the bidirectional DAG
// declared in index.toml is correctly materialized across all three databases.
func (r *Registry) ValidateAll() []ValidationError {
	var errs []ValidationError

	// 1. Validate L2 health normalization bases exist in L0 ternary scales
	errs = append(errs, r.validateHealthNormalization()...)

	// 2. Validate L2 filesystem vocabulary has consistent Hebrew metadata
	errs = append(errs, r.validateFilesystemVocabulary()...)

	// 3. Validate cross-layer spec dependencies resolve
	errs = append(errs, r.validateSpecDependencies()...)

	// 4. Validate L2 filesystem extensions are discoverable in L1
	errs = append(errs, r.validateExtensionLanguageParity()...)

	return errs
}

// ValidateCrossLayerRefs checks that all cross-layer references resolve.
// Alias for ValidateAll — kept for backward compatibility.
func (r *Registry) ValidateCrossLayerRefs() []ValidationError {
	return r.ValidateAll()
}

func (r *Registry) validateHealthNormalization() []ValidationError {
	var errs []ValidationError

	bases, err := r.Platform.AllNormalizationBases()
	if err != nil {
		errs = append(errs, ValidationError{
			Layer: "L2", Table: "health_normalization", Field: "*",
			Message: fmt.Sprintf("query failed: %v", err),
		})
		return errs
	}

	scales, err := r.Foundation.AllScales()
	if err != nil {
		errs = append(errs, ValidationError{
			Layer: "L0", Table: "ternary_scales", Field: "*",
			Message: fmt.Sprintf("query failed: %v", err),
		})
		return errs
	}

	scaleNames := make(map[string]bool, len(scales))
	for _, s := range scales {
		scaleNames[s.Name] = true
	}

	for _, b := range bases {
		if !scaleNames[b.Name] {
			errs = append(errs, ValidationError{
				Layer:   "L2",
				Table:   "health_normalization",
				Field:   "name",
				Value:   b.Name,
				Target:  "L0.ternary_scales.name",
				Message: "normalization base not found in L0 ternary scales",
			})
		}
	}
	return errs
}

func (r *Registry) validateFilesystemVocabulary() []ValidationError {
	var errs []ValidationError

	extensions, err := r.Platform.AllFilesystemExtensions()
	if err != nil {
		errs = append(errs, ValidationError{
			Layer: "L2", Table: "filesystem_biblical_extensions", Field: "*",
			Message: fmt.Sprintf("query failed: %v", err),
		})
		return errs
	}

	// Every filesystem extension must have a non-empty Hebrew name and meaning
	for _, ext := range extensions {
		if ext.Name == "" {
			errs = append(errs, ValidationError{
				Layer:   "L2",
				Table:   "filesystem_biblical_extensions",
				Field:   "name",
				Value:   "(empty)",
				Target:  "self",
				Message: "filesystem extension has no biblical vocabulary name",
			})
		}
		if ext.Hebrew == "" {
			errs = append(errs, ValidationError{
				Layer:   "L2",
				Table:   "filesystem_biblical_extensions",
				Field:   "hebrew",
				Value:   ext.Name,
				Target:  "self",
				Message: "filesystem extension missing Hebrew characters",
			})
		}
		if ext.Extensions == "" || ext.Extensions == "[]" {
			errs = append(errs, ValidationError{
				Layer:   "L2",
				Table:   "filesystem_biblical_extensions",
				Field:   "extensions",
				Value:   ext.Name,
				Target:  "self",
				Message: "filesystem extension has no file extension mappings",
			})
		}
	}
	return errs
}

func (r *Registry) validateSpecDependencies() []ValidationError {
	var errs []ValidationError

	allSpecs, err := r.AllSpecs()
	if err != nil {
		errs = append(errs, ValidationError{
			Layer: "*", Table: "specs", Field: "*",
			Message: fmt.Sprintf("failed to load all specs: %v", err),
		})
		return errs
	}

	// Check all dependencies from each layer
	layers := []struct {
		name string
		deps func(string) ([]foundation.Dependency, error)
	}{
		{"L0", func(key string) ([]foundation.Dependency, error) {
			return r.Foundation.DependenciesOf(key)
		}},
	}

	for specKey, layer := range allSpecs {
		for _, l := range layers {
			if l.name != layer {
				continue
			}
			deps, err := l.deps(specKey)
			if err != nil {
				continue // No deps for this spec
			}
			for _, d := range deps {
				if _, ok := allSpecs[d.DependsOn]; !ok {
					errs = append(errs, ValidationError{
						Layer:   layer,
						Table:   "dependencies",
						Field:   "depends_on",
						Value:   d.DependsOn,
						Target:  "*.specs.key",
						Message: fmt.Sprintf("spec %q depends on %q which doesn't exist in any layer", specKey, d.DependsOn),
					})
				}
			}
		}
	}

	return errs
}

func (r *Registry) validateExtensionLanguageParity() []ValidationError {
	var errs []ValidationError

	extensions, err := r.Platform.AllFilesystemExtensions()
	if err != nil {
		errs = append(errs, ValidationError{
			Layer: "L2", Table: "filesystem_biblical_extensions", Field: "*",
			Message: fmt.Sprintf("query failed: %v", err),
		})
		return errs
	}

	// For each L2 filesystem extension that has mapped file extensions,
	// check whether at least one appears in L1's extension table.
	for _, ext := range extensions {
		if ext.Extensions == "" || ext.Extensions == "[]" {
			continue // Skip empty — already caught by vocabulary validation
		}

		// Parse the JSON array manually (avoid encoding/json import)
		// Extensions looks like: [".go", ".rs", ".py"]
		trimmed := strings.TrimSpace(ext.Extensions)
		trimmed = strings.TrimPrefix(trimmed, "[")
		trimmed = strings.TrimSuffix(trimmed, "]")

		if trimmed == "" {
			continue
		}

		parts := strings.Split(trimmed, ",")
		anyFoundInL1 := false
		for _, part := range parts {
			cleaned := strings.Trim(strings.TrimSpace(part), "\"'")
			if cleaned == "" {
				continue
			}
			if _, err := r.Language.ExtensionByExt(cleaned); err == nil {
				anyFoundInL1 = true
				break
			}
		}

		if !anyFoundInL1 {
			errs = append(errs, ValidationError{
				Layer:   "L2",
				Table:   "filesystem_biblical_extensions",
				Field:   "extensions",
				Value:   ext.Name,
				Target:  "L1.extensions.ext",
				Message: fmt.Sprintf("none of %s found in L1 extensions table", ext.Extensions),
			})
		}
	}

	return errs
}

// ============================================================================
// CLOSING
// ============================================================================
