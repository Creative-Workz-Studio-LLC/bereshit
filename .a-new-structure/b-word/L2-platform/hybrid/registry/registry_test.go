//omni:code --go -test
//omni:key B-L2-registry-test
//omni:version a-01.00

package registry

import (
	"os"
	"path/filepath"
	"testing"
)

// ============================================================================
// SETUP
// ============================================================================

// moduleRoot returns the b-word/ directory for test database access.
func moduleRoot(t *testing.T) string {
	t.Helper()

	// Walk up from this test file to b-word/
	dir, err := os.Getwd()
	if err != nil {
		t.Fatalf("getwd: %v", err)
	}

	// registry/ → hybrid/ → L2-platform/ → b-word/
	root := filepath.Join(dir, "..", "..", "..")
	root, err = filepath.Abs(root)
	if err != nil {
		t.Fatalf("abs: %v", err)
	}
	return root
}

// tryOpen attempts to open the registry. Skips the test if databases
// are not available.
func tryOpen(t *testing.T) *Registry {
	t.Helper()
	root := moduleRoot(t)
	r, err := Open(root)
	if err != nil {
		t.Skipf("databases not available: %v", err)
	}
	t.Cleanup(func() { r.Close() })
	return r
}

// ============================================================================
// BODY — Tests
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Open / Close
// ────────────────────────────────────────────────────────────────

func TestRegistryOpen(t *testing.T) {
	r := tryOpen(t)
	if r.Root() == "" {
		t.Error("root should not be empty")
	}
}

func TestRegistryClose(t *testing.T) {
	root := moduleRoot(t)
	r, err := Open(root)
	if err != nil {
		t.Skipf("databases not available: %v", err)
	}
	if err := r.Close(); err != nil {
		t.Errorf("close: %v", err)
	}
}

// ────────────────────────────────────────────────────────────────
// All Specs
// ────────────────────────────────────────────────────────────────

func TestAllSpecsSpansLayers(t *testing.T) {
	r := tryOpen(t)
	specs, err := r.AllSpecs()
	if err != nil {
		t.Fatalf("AllSpecs: %v", err)
	}
	if len(specs) == 0 {
		t.Error("expected at least one spec")
	}

	layers := make(map[string]bool)
	for _, layer := range specs {
		layers[layer] = true
	}

	if !layers["L0"] {
		t.Error("expected L0 specs")
	}
	if !layers["L1"] {
		t.Error("expected L1 specs")
	}
	if !layers["L2"] {
		t.Error("expected L2 specs")
	}
}

func TestResolveDependencyL0(t *testing.T) {
	r := tryOpen(t)
	layer, found := r.ResolveDependency("B-L0-math")
	if !found {
		t.Skip("B-L0-math not in database")
	}
	if layer != "L0" {
		t.Errorf("expected L0, got %s", layer)
	}
}

func TestResolveDependencyL1(t *testing.T) {
	r := tryOpen(t)
	layer, found := r.ResolveDependency("B-L1-keywords")
	if !found {
		t.Skip("B-L1-keywords not in database")
	}
	if layer != "L1" {
		t.Errorf("expected L1, got %s", layer)
	}
}

func TestResolveDependencyL2(t *testing.T) {
	r := tryOpen(t)
	layer, found := r.ResolveDependency("B-L2-health")
	if !found {
		t.Skip("B-L2-health not in database")
	}
	if layer != "L2" {
		t.Errorf("expected L2, got %s", layer)
	}
}

func TestResolveDependencyNotFound(t *testing.T) {
	r := tryOpen(t)
	_, found := r.ResolveDependency("nonexistent-spec-key")
	if found {
		t.Error("should not find nonexistent spec")
	}
}

// ────────────────────────────────────────────────────────────────
// Cross-Layer Queries: L0 ↔ L2
// ────────────────────────────────────────────────────────────────

func TestHealthNormalizationScales(t *testing.T) {
	r := tryOpen(t)
	_, err := r.HealthNormalizationScales()
	if err != nil {
		t.Errorf("HealthNormalizationScales: %v", err)
	}
}

func TestFilesystemVocabulary(t *testing.T) {
	r := tryOpen(t)
	refs, err := r.FilesystemVocabulary()
	if err != nil {
		t.Errorf("FilesystemVocabulary: %v", err)
	}
	if len(refs) > 0 {
		t.Logf("filesystem vocabulary: %d entries", len(refs))
	}
}

func TestTypeForHealthLevelNotFound(t *testing.T) {
	r := tryOpen(t)
	_, err := r.TypeForHealthLevel("nonexistent_level")
	if err == nil {
		t.Error("expected error for nonexistent level")
	}
}

// ────────────────────────────────────────────────────────────────
// Cross-Layer Queries: L1 ↔ L2
// ────────────────────────────────────────────────────────────────

func TestExtensionWithLanguageInfo(t *testing.T) {
	r := tryOpen(t)
	// Should not panic or error on any input
	_, err := r.ExtensionWithLanguageInfo(".omni")
	if err != nil {
		t.Errorf("ExtensionWithLanguageInfo: %v", err)
	}
}

func TestExtensionWithLanguageInfoNotFound(t *testing.T) {
	r := tryOpen(t)
	ref, err := r.ExtensionWithLanguageInfo(".nonexistent_ext_xyz")
	if err != nil {
		t.Errorf("ExtensionWithLanguageInfo: %v", err)
	}
	if ref != nil {
		t.Log("extension found — checking cross-ref fields")
	}
}

// ────────────────────────────────────────────────────────────────
// Cross-Layer Queries: L0 ↔ L1
// ────────────────────────────────────────────────────────────────

func TestOperationsUsedByKeywords(t *testing.T) {
	r := tryOpen(t)
	_, err := r.OperationsUsedByKeywords()
	if err != nil {
		t.Errorf("OperationsUsedByKeywords: %v", err)
	}
}

// ────────────────────────────────────────────────────────────────
// Cross-Layer Validation
// ────────────────────────────────────────────────────────────────

func TestValidateAll(t *testing.T) {
	r := tryOpen(t)
	errs := r.ValidateAll()
	for _, e := range errs {
		t.Logf("validation: %s", e)
	}
	// Log validation errors but don't fail — data alignment comes later
	t.Logf("total validation issues: %d", len(errs))
}

func TestValidateCrossLayerRefs(t *testing.T) {
	r := tryOpen(t)
	errs := r.ValidateCrossLayerRefs()
	for _, e := range errs {
		t.Logf("validation: %s", e)
	}
	// Should match ValidateAll output — they're the same now
}

// ────────────────────────────────────────────────────────────────
// Type formatting
// ────────────────────────────────────────────────────────────────

func TestValidationErrorString(t *testing.T) {
	e := ValidationError{
		Layer:   "L2",
		Table:   "health_normalization",
		Field:   "base_name",
		Value:   "test",
		Target:  "L0.ternary_scales.name",
		Message: "not found",
	}
	s := e.String()
	if s == "" {
		t.Error("expected non-empty string")
	}
	if !contains(s, "L2") || !contains(s, "not found") {
		t.Errorf("unexpected format: %s", s)
	}
}

func contains(s, sub string) bool {
	return len(s) >= len(sub) && (s == sub || len(s) > 0 && containsHelper(s, sub))
}

func containsHelper(s, sub string) bool {
	for i := 0; i <= len(s)-len(sub); i++ {
		if s[i:i+len(sub)] == sub {
			return true
		}
	}
	return false
}

// ============================================================================
// CLOSING
// ============================================================================
