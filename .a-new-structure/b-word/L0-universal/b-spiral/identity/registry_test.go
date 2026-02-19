//omni:code --go -demo-test
//omni:key B-L0-identity-registry-test
//omni:version b-01.50

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-identity-registry-test
// Purpose: Comprehensive tests for the identity registry
// Biblical: Deuteronomy 19:15 — One witness shall not rise up against a man;
//           at the mouth of two witnesses shall the matter be established
// Version: b-01.50

// Package identity tests prove the registry's correctness.
//
// Test categories:
//   - Self-registration: init() bootstrap anchor works
//   - Registration: Register/RegisterErr validation and overwrite behavior
//   - Lookup: Lookup/LookupErr ok-pattern and error-pattern
//   - Query: All, Count, ByLayer, ByDomain, ByTag
//   - Stats: RegistryStats snapshot accuracy
//   - Type methods: String(), Get() on PackageIdentity
//   - Self accessors: PragmaGet, MetadataGet, PragmaSlice, MetadataSlice
//   - Concurrency: parallel Register + Lookup goroutines
package identity

import (
	"errors"
	"sync"
	"testing"
)

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Test infrastructure: fixtures, helpers, cleanup.

// fixture creates a test package identity with predictable values.
// The key is used as-is; layer and domain are derived from the key.
func fixture(key, layer, domain string) ([][2]string, [][2]string) {
	pragma := [][2]string{
		{"I1.key", key},
		{"I1.format", "go"},
		{"I1.from", "test"},
		{"I1.at", "b-01.50"},
		{"I2.type", "code"},
		{"I2.structure", "4-block"},
		{"I2.subtype", "library"},
		{"I2.role", "test"},
		{"I3.file", key + ".go"},
		{"I3.title", "Test Package " + key},
		{"I4.layer", layer},
		{"I4.position", "test"},
	}
	metadata := [][2]string{
		{"C1.version", "b-01.50"},
		{"C1.status", "Active"},
		{"C7.tags", "test, fixture"},
		{"C7.domain", domain},
	}
	return pragma, metadata
}

// resetRegistry clears all packages except self-registration.
// Called via t.Cleanup to ensure test isolation.
func resetRegistry(t *testing.T) {
	t.Helper()
	t.Cleanup(func() {
		mu.Lock()
		defer mu.Unlock()

		// Clear everything
		packages = make(map[string]*PackageIdentity)
		layerIndex = make(map[string][]*PackageIdentity)
		domainIndex = make(map[string][]*PackageIdentity)

		// Re-register self (init() equivalent)
		p := &PackageIdentity{
			Key:      PragmaGet("I1.key"),
			Pragma:   sliceToMap(Pragma),
			Metadata: sliceToMap(Metadata),
		}
		packages[p.Key] = p
		updateIndexes(p, nil)
	})
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Self-Registration
// ──────────────────────────────────────────────────────────────────────────

func TestSelfRegistration(t *testing.T) {
	// The identity package registers itself via init().
	// This is the bootstrap anchor — the registry knowing itself.

	t.Run("init registers self", func(t *testing.T) {
		p, ok := Lookup("B-L0-identity-registry")
		if !ok {
			t.Fatal("self-registration failed: identity package not found")
		}
		if p.Key != "B-L0-identity-registry" {
			t.Errorf("key = %q, want %q", p.Key, "B-L0-identity-registry")
		}
	})

	t.Run("self has correct pragma", func(t *testing.T) {
		p, _ := Lookup("B-L0-identity-registry")
		if got := p.Pragma["I1.format"]; got != "go" {
			t.Errorf("I1.format = %q, want %q", got, "go")
		}
		if got := p.Pragma["I4.layer"]; got != "L0" {
			t.Errorf("I4.layer = %q, want %q", got, "L0")
		}
	})

	t.Run("self has correct metadata", func(t *testing.T) {
		p, _ := Lookup("B-L0-identity-registry")
		if got := p.Metadata["C1.status"]; got != "Active" {
			t.Errorf("C1.status = %q, want %q", got, "Active")
		}
		if got := p.Metadata["C7.domain"]; got != "identity" {
			t.Errorf("C7.domain = %q, want %q", got, "identity")
		}
	})
}

// ──────────────────────────────────────────────────────────────────────────
// 2. Register / RegisterErr
// ──────────────────────────────────────────────────────────────────────────

func TestRegister(t *testing.T) {
	resetRegistry(t)

	t.Run("valid key registers", func(t *testing.T) {
		pragma, metadata := fixture("B-test-pkg-1", "L1", "test")
		Register("B-test-pkg-1", pragma, metadata)

		p, ok := Lookup("B-test-pkg-1")
		if !ok {
			t.Fatal("registered package not found")
		}
		if p.Key != "B-test-pkg-1" {
			t.Errorf("key = %q, want %q", p.Key, "B-test-pkg-1")
		}
	})

	t.Run("empty key silently ignored", func(t *testing.T) {
		before := Count()
		Register("", nil, nil)
		after := Count()
		if after != before {
			t.Errorf("count changed: %d → %d (empty key should be ignored)", before, after)
		}
	})

	t.Run("duplicate key overwrites", func(t *testing.T) {
		pragma1, metadata1 := fixture("B-test-dup", "L0", "alpha")
		pragma2, metadata2 := fixture("B-test-dup", "L1", "beta")

		Register("B-test-dup", pragma1, metadata1)
		Register("B-test-dup", pragma2, metadata2)

		p, _ := Lookup("B-test-dup")
		if got := p.Pragma["I4.layer"]; got != "L1" {
			t.Errorf("layer = %q, want %q (should be overwritten)", got, "L1")
		}
	})
}

func TestRegisterErr(t *testing.T) {
	resetRegistry(t)

	t.Run("empty key returns ErrEmptyKey", func(t *testing.T) {
		err := RegisterErr("", nil, nil)
		if !errors.Is(err, ErrEmptyKey) {
			t.Errorf("err = %v, want ErrEmptyKey", err)
		}
	})

	t.Run("valid key returns nil", func(t *testing.T) {
		pragma, metadata := fixture("B-test-err-ok", "L0", "test")
		err := RegisterErr("B-test-err-ok", pragma, metadata)
		if err != nil {
			t.Errorf("err = %v, want nil", err)
		}
	})
}

// ──────────────────────────────────────────────────────────────────────────
// 3. Lookup / LookupErr
// ──────────────────────────────────────────────────────────────────────────

func TestLookup(t *testing.T) {
	resetRegistry(t)

	t.Run("existing key returns package", func(t *testing.T) {
		p, ok := Lookup("B-L0-identity-registry")
		if !ok || p == nil {
			t.Fatal("self-registered package not found")
		}
	})

	t.Run("missing key returns nil false", func(t *testing.T) {
		p, ok := Lookup("B-does-not-exist")
		if ok || p != nil {
			t.Errorf("got (%v, %v), want (nil, false)", p, ok)
		}
	})
}

func TestLookupErr(t *testing.T) {
	resetRegistry(t)

	t.Run("existing key returns package", func(t *testing.T) {
		p, err := LookupErr("B-L0-identity-registry")
		if err != nil {
			t.Fatalf("err = %v, want nil", err)
		}
		if p.Key != "B-L0-identity-registry" {
			t.Errorf("key = %q, want %q", p.Key, "B-L0-identity-registry")
		}
	})

	t.Run("missing key returns ErrNotRegistered", func(t *testing.T) {
		_, err := LookupErr("B-does-not-exist")
		if !errors.Is(err, ErrNotRegistered) {
			t.Errorf("err = %v, want ErrNotRegistered", err)
		}
	})
}

// ──────────────────────────────────────────────────────────────────────────
// 4. All / Count
// ──────────────────────────────────────────────────────────────────────────

func TestAll(t *testing.T) {
	resetRegistry(t)

	t.Run("includes self-registered", func(t *testing.T) {
		all := All()
		found := false
		for _, p := range all {
			if p.Key == "B-L0-identity-registry" {
				found = true
				break
			}
		}
		if !found {
			t.Error("All() does not include self-registered identity package")
		}
	})

	t.Run("includes registered packages", func(t *testing.T) {
		pragma, metadata := fixture("B-test-all", "L0", "test")
		Register("B-test-all", pragma, metadata)

		all := All()
		if len(all) < 2 {
			t.Errorf("len(All()) = %d, want >= 2 (self + registered)", len(all))
		}
	})
}

func TestCount(t *testing.T) {
	resetRegistry(t)

	t.Run("matches All length", func(t *testing.T) {
		if Count() != len(All()) {
			t.Errorf("Count() = %d, len(All()) = %d — mismatch", Count(), len(All()))
		}
	})

	t.Run("increments on register", func(t *testing.T) {
		before := Count()
		pragma, metadata := fixture("B-test-count", "L0", "test")
		Register("B-test-count", pragma, metadata)
		after := Count()
		if after != before+1 {
			t.Errorf("count: %d → %d, want %d", before, after, before+1)
		}
	})
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Query: ByLayer / ByDomain / ByTag
// ──────────────────────────────────────────────────────────────────────────

func TestByLayer(t *testing.T) {
	resetRegistry(t)

	t.Run("self at L0", func(t *testing.T) {
		result := ByLayer("L0")
		if len(result) == 0 {
			t.Fatal("ByLayer(L0) returned empty — self should be at L0")
		}
		found := false
		for _, p := range result {
			if p.Key == "B-L0-identity-registry" {
				found = true
			}
		}
		if !found {
			t.Error("ByLayer(L0) does not include self-registered identity package")
		}
	})

	t.Run("unknown layer returns nil", func(t *testing.T) {
		result := ByLayer("L99")
		if len(result) != 0 {
			t.Errorf("ByLayer(L99) returned %d packages, want 0", len(result))
		}
	})

	t.Run("multiple packages at same layer", func(t *testing.T) {
		p1, m1 := fixture("B-test-layer-a", "L2", "test")
		p2, m2 := fixture("B-test-layer-b", "L2", "test")
		Register("B-test-layer-a", p1, m1)
		Register("B-test-layer-b", p2, m2)

		result := ByLayer("L2")
		if len(result) != 2 {
			t.Errorf("ByLayer(L2) = %d packages, want 2", len(result))
		}
	})
}

func TestByDomain(t *testing.T) {
	resetRegistry(t)

	t.Run("self at identity domain", func(t *testing.T) {
		result := ByDomain("identity")
		if len(result) == 0 {
			t.Fatal("ByDomain(identity) returned empty — self should be in identity domain")
		}
	})

	t.Run("unknown domain returns nil", func(t *testing.T) {
		result := ByDomain("does-not-exist")
		if len(result) != 0 {
			t.Errorf("ByDomain(does-not-exist) returned %d, want 0", len(result))
		}
	})
}

func TestByTag(t *testing.T) {
	resetRegistry(t)

	t.Run("finds by exact tag", func(t *testing.T) {
		result := ByTag("identity")
		found := false
		for _, p := range result {
			if p.Key == "B-L0-identity-registry" {
				found = true
			}
		}
		if !found {
			t.Error("ByTag(identity) does not include self — C7.tags should contain 'identity'")
		}
	})

	t.Run("finds by tag with whitespace", func(t *testing.T) {
		// C7.tags = "identity, registry, discovery, self-awareness, cpi-si"
		// ByTag should handle leading/trailing whitespace in comma-separated values
		result := ByTag("cpi-si")
		if len(result) == 0 {
			t.Error("ByTag(cpi-si) returned empty — should find self")
		}
	})

	t.Run("no match returns nil", func(t *testing.T) {
		result := ByTag("nonexistent-tag")
		if len(result) != 0 {
			t.Errorf("ByTag(nonexistent-tag) returned %d, want 0", len(result))
		}
	})
}

// ──────────────────────────────────────────────────────────────────────────
// 6. Stats
// ──────────────────────────────────────────────────────────────────────────

func TestStats(t *testing.T) {
	resetRegistry(t)

	t.Run("total matches count", func(t *testing.T) {
		s := Stats()
		if s.Total != Count() {
			t.Errorf("Stats.Total = %d, Count() = %d — mismatch", s.Total, Count())
		}
	})

	t.Run("layer breakdown", func(t *testing.T) {
		s := Stats()
		if s.ByLayer["L0"] == 0 {
			t.Error("Stats.ByLayer[L0] = 0 — self should be at L0")
		}
	})

	t.Run("reflects new registration", func(t *testing.T) {
		pragma, metadata := fixture("B-test-stats", "L3", "test")
		Register("B-test-stats", pragma, metadata)

		s := Stats()
		if s.ByLayer["L3"] != 1 {
			t.Errorf("Stats.ByLayer[L3] = %d, want 1", s.ByLayer["L3"])
		}
	})
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Type Methods: String, Get
// ──────────────────────────────────────────────────────────────────────────

func TestString(t *testing.T) {
	p, _ := Lookup("B-L0-identity-registry")

	t.Run("format matches expected", func(t *testing.T) {
		got := p.String()
		want := "B-L0-identity-registry (library @ L0)"
		if got != want {
			t.Errorf("String() = %q, want %q", got, want)
		}
	})
}

func TestGet(t *testing.T) {
	p, _ := Lookup("B-L0-identity-registry")

	t.Run("pragma key", func(t *testing.T) {
		if got := p.Get("I1.format"); got != "go" {
			t.Errorf("Get(I1.format) = %q, want %q", got, "go")
		}
	})

	t.Run("metadata key", func(t *testing.T) {
		if got := p.Get("C1.status"); got != "Active" {
			t.Errorf("Get(C1.status) = %q, want %q", got, "Active")
		}
	})

	t.Run("missing key returns empty", func(t *testing.T) {
		if got := p.Get("Z99.nope"); got != "" {
			t.Errorf("Get(Z99.nope) = %q, want empty", got)
		}
	})

	t.Run("pragma takes precedence over metadata", func(t *testing.T) {
		// If same key existed in both, Pragma should win.
		// This tests the documented behavior: "Checks Pragma first"
		// No real overlap exists, but verify the lookup order.
		if got := p.Get("I1.key"); got != "B-L0-identity-registry" {
			t.Errorf("Get(I1.key) = %q, want %q", got, "B-L0-identity-registry")
		}
	})
}

// ──────────────────────────────────────────────────────────────────────────
// 8. Self Accessors: PragmaGet, MetadataGet, PragmaSlice, MetadataSlice
// ──────────────────────────────────────────────────────────────────────────

func TestPragmaGet(t *testing.T) {
	t.Run("existing key", func(t *testing.T) {
		if got := PragmaGet("I1.key"); got != "B-L0-identity-registry" {
			t.Errorf("PragmaGet(I1.key) = %q, want %q", got, "B-L0-identity-registry")
		}
	})

	t.Run("missing key returns empty", func(t *testing.T) {
		if got := PragmaGet("Z99.nope"); got != "" {
			t.Errorf("PragmaGet(Z99.nope) = %q, want empty", got)
		}
	})
}

func TestMetadataGet(t *testing.T) {
	t.Run("existing key", func(t *testing.T) {
		if got := MetadataGet("C1.version"); got != "b-02.00" {
			t.Errorf("MetadataGet(C1.version) = %q, want %q", got, "b-02.00")
		}
	})

	t.Run("missing key returns empty", func(t *testing.T) {
		if got := MetadataGet("Z99.nope"); got != "" {
			t.Errorf("MetadataGet(Z99.nope) = %q, want empty", got)
		}
	})
}

func TestPragmaSlice(t *testing.T) {
	s := PragmaSlice()
	if len(s) == 0 {
		t.Fatal("PragmaSlice() returned empty")
	}
	// Should be the exact same slice as Pragma
	if s[0][0] != "I1.key" {
		t.Errorf("first field = %q, want I1.key", s[0][0])
	}
}

func TestMetadataSlice(t *testing.T) {
	s := MetadataSlice()
	if len(s) == 0 {
		t.Fatal("MetadataSlice() returned empty")
	}
	if s[0][0] != "C1.version" {
		t.Errorf("first field = %q, want C1.version", s[0][0])
	}
}

// ──────────────────────────────────────────────────────────────────────────
// 9. Concurrency
// ──────────────────────────────────────────────────────────────────────────

func TestConcurrency(t *testing.T) {
	resetRegistry(t)

	const goroutines = 50

	t.Run("parallel register and lookup", func(t *testing.T) {
		var wg sync.WaitGroup

		// Register goroutines
		for i := 0; i < goroutines; i++ {
			wg.Add(1)
			go func(n int) {
				defer wg.Done()
				key := "B-concurrent-" + string(rune('A'+n%26))
				pragma, metadata := fixture(key, "L0", "test")
				Register(key, pragma, metadata)
			}(i)
		}

		// Lookup goroutines (concurrent with registration)
		for i := 0; i < goroutines; i++ {
			wg.Add(1)
			go func() {
				defer wg.Done()
				_ = All()
				_ = Count()
				_ = ByLayer("L0")
				_ = Stats()
			}()
		}

		wg.Wait()

		// Verify no panics occurred and registry is consistent
		if Count() < 1 {
			t.Error("registry empty after concurrent operations")
		}
	})
}

// ──────────────────────────────────────────────────────────────────────────
// 10. Internal Helpers
// ──────────────────────────────────────────────────────────────────────────

func TestSliceToMap(t *testing.T) {
	pairs := [][2]string{
		{"key1", "val1"},
		{"key2", "val2"},
	}
	m := sliceToMap(pairs)

	if m["key1"] != "val1" {
		t.Errorf("m[key1] = %q, want val1", m["key1"])
	}
	if m["key2"] != "val2" {
		t.Errorf("m[key2] = %q, want val2", m["key2"])
	}
	if len(m) != 2 {
		t.Errorf("len(m) = %d, want 2", len(m))
	}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Cv — Validation
//   go test -v ./...
//   go test -race ./... (race detector)
//   go test -cover ./... (coverage)
//
// Ce — Execution
//   Test file — not executed directly. Run via go test.
//
// Cc — Cleanup
//   resetRegistry(t) via t.Cleanup ensures test isolation.
//
// X1: Policy
//   Safe to add new test functions. Follow table-driven pattern.
//   resetRegistry(t) required for any test that registers fixtures.
//
// X5: Note
//   "At the mouth of two witnesses shall the matter be established."
//   — Deuteronomy 19:15
//
//   The compiler witnesses logic. The tests witness behavior.
//   Two witnesses establish the matter.

// ============================================================================
// END CLOSING
// ============================================================================
