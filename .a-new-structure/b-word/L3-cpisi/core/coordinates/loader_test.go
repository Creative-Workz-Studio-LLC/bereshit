package coordinates_test

import (
	"testing"

	"creativeworkzstudio.com/bereshit/word/work/pkg/core/coordinates"
)

// ============================================================================
// METADATA
// ============================================================================
//
// Loader Tests — Bible Data Loading from Bereshit TOMLs
//
// Philosophy: Tests with 1 solution designed to fail become specifications.
//
//   DETECT:  The specification (what MUST be true)
//   ASSESS:  What was found (current state)
//   RESTORE: The path from current to required (empty if already there)
//
// NOTE: These tests require access to the Bereshit project for TOML specs.
//       They are integration tests, not unit tests.
//
// Traces to:
//   HALT_01 (ANCHOR_GENESIS) — Scripture is the coordinate map
//
// Uses bereshitRoot constant from coordinates_test.go
//
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Loader Tests
// ────────────────────────────────────────────────────────────────

func TestLoaderLoadBibleData(t *testing.T) {
	// Test that LoadBibleData successfully loads from Bereshit
	data, err := coordinates.LoadBibleData(bereshitRoot)
	if err != nil {
		t.Fatalf("LoadBibleData failed: %v", err)
	}

	// Verify books loaded
	if len(data.BookIndex) != 66 {
		t.Errorf("Expected 66 books, got %d", len(data.BookIndex))
	}

	// Verify address space
	if data.AddressSpace == nil {
		t.Error("AddressSpace not loaded")
	} else if data.AddressSpace.KJVVerses != 31102 {
		t.Errorf("Expected 31102 KJV verses, got %d", data.AddressSpace.KJVVerses)
	}

	t.Logf("✓ Loaded %d books, %d KJV verses", len(data.BookIndex), data.AddressSpace.KJVVerses)
}

func TestLoaderGetBook(t *testing.T) {
	data, err := coordinates.LoadBibleData(bereshitRoot)
	if err != nil {
		t.Skipf("Skipping: LoadBibleData failed: %v", err)
	}

	// Test Genesis lookup
	genesis, ok := data.GetBook("genesis")
	if !ok {
		t.Error("Genesis not found")
	} else {
		if genesis.Index != 1 {
			t.Errorf("Genesis index: expected 1, got %d", genesis.Index)
		}
		if genesis.Chapters != 50 {
			t.Errorf("Genesis chapters: expected 50, got %d", genesis.Chapters)
		}
		if genesis.Verses != 1533 {
			t.Errorf("Genesis verses: expected 1533, got %d", genesis.Verses)
		}
		t.Logf("✓ Genesis: Index=%d, Chapters=%d, Verses=%d, Range=[%d,%d]",
			genesis.Index, genesis.Chapters, genesis.Verses, genesis.RangeStart, genesis.RangeEnd)
	}

	// Test Revelation lookup
	revelation, ok := data.GetBook("revelation")
	if !ok {
		t.Error("Revelation not found")
	} else {
		if revelation.Index != 66 {
			t.Errorf("Revelation index: expected 66, got %d", revelation.Index)
		}
		t.Logf("✓ Revelation: Index=%d, Chapters=%d, Verses=%d",
			revelation.Index, revelation.Chapters, revelation.Verses)
	}
}

func TestLoaderGetBooksAsBookInfo(t *testing.T) {
	data, err := coordinates.LoadBibleData(bereshitRoot)
	if err != nil {
		t.Skipf("Skipping: LoadBibleData failed: %v", err)
	}

	byName, byIndex := data.GetBooksAsBookInfo()

	if len(byName) != 66 {
		t.Errorf("Expected 66 books by name, got %d", len(byName))
	}
	if len(byIndex) != 66 {
		t.Errorf("Expected 66 books by index, got %d", len(byIndex))
	}

	// Verify Genesis in both maps
	if gen, ok := byName["Genesis"]; ok {
		if gen.Index != 1 {
			t.Errorf("byName Genesis index: expected 1, got %d", gen.Index)
		}
		// Verify new fields from Bereshit are populated
		if gen.Verses == 0 {
			t.Error("Genesis Verses not populated from Bereshit TOML")
		}
		if gen.Range[0] != 1 || gen.Range[1] != 1533 {
			t.Errorf("Genesis Range: expected [1,1533], got %v", gen.Range)
		}
		t.Logf("✓ BookInfo conversion: Genesis Abbreviation=%s, Verses=%d, Range=%v",
			gen.Abbreviation, gen.Verses, gen.Range)
	} else {
		t.Error("Genesis not found in byName map")
	}

	if gen, ok := byIndex[1]; ok {
		if gen.Name != "Genesis" {
			t.Errorf("byIndex[1] name: expected Genesis, got %s", gen.Name)
		}
	} else {
		t.Error("Book index 1 not found in byIndex map")
	}
}

func TestLoaderNewBibleIndexFromBereshit(t *testing.T) {
	scriptureRoot := bereshitRoot + "/word/scripture"

	// Test with KJV set (default)
	bi, err := coordinates.NewBibleIndexFromBereshit(scriptureRoot, bereshitRoot, coordinates.SetKJV)
	if err != nil {
		t.Fatalf("NewBibleIndexFromBereshit failed: %v", err)
	}

	// Verify books loaded
	if len(bi.Books) != 66 {
		t.Errorf("Expected 66 books, got %d", len(bi.Books))
	}

	// Verify translation set is tracked
	if bi.GetTranslationSet() != coordinates.SetKJV {
		t.Errorf("Expected SetKJV, got %v", bi.GetTranslationSet())
	}

	// Verify Genesis has extended fields
	if gen, ok := bi.Books["Genesis"]; ok {
		if gen.Verses != 1533 {
			t.Errorf("Genesis verses: expected 1533, got %d", gen.Verses)
		}
		if gen.Range[0] != 1 {
			t.Errorf("Genesis range start: expected 1, got %d", gen.Range[0])
		}
		t.Logf("✓ BibleIndex from Bereshit: Genesis Verses=%d, Range=%v", gen.Verses, gen.Range)
	} else {
		t.Error("Genesis not found in BibleIndex")
	}

	// Verify coordinate mapping still works
	ref := coordinates.BibleReference{Book: "Genesis", Chapter: 1, Verse: 1}
	coords, err := bi.ToCoords(ref)
	if err != nil {
		t.Errorf("ToCoords failed: %v", err)
	} else if !coords.IsGenesis() {
		t.Error("Genesis 1:1 should be origin")
	} else {
		t.Logf("✓ Genesis 1:1 → (%.2f, %.2f, %.2f) IsGenesis=%v",
			coords.BookNorm, coords.ChapterNorm, coords.VerseNorm, coords.IsGenesis())
	}

	// Test cube index O(1) lookup if available
	if bi.HasCubeIndex() {
		verse, found := bi.LookupByOrdinal(1)
		if !found {
			t.Error("LookupByOrdinal(1) should find Genesis 1:1")
		} else if verse.Book != "Genesis" || verse.Chapter != 1 || verse.Verse != 1 {
			t.Errorf("LookupByOrdinal(1) = %s %d:%d, expected Genesis 1:1",
				verse.Book, verse.Chapter, verse.Verse)
		} else {
			t.Logf("✓ CubeIndex O(1) lookup: ordinal 1 → %s %d:%d", verse.Book, verse.Chapter, verse.Verse)
		}
	} else {
		t.Log("⚠ CubeIndex not loaded (run verse-index to generate)")
	}
}

func TestLoaderWithFallback(t *testing.T) {
	scriptureRoot := "/nonexistent/scripture"

	// Should fall back to hardcoded when Bereshit not found
	bi := coordinates.NewBibleIndexWithFallback(scriptureRoot, "/nonexistent/bereshit")

	if len(bi.Books) != 66 {
		t.Errorf("Fallback should have 66 books, got %d", len(bi.Books))
	}

	// Verify Genesis exists (from hardcoded fallback)
	if _, ok := bi.Books["Genesis"]; !ok {
		t.Error("Fallback missing Genesis")
	}

	t.Log("✓ Fallback to hardcoded data works")
}

// NOTE: logDAR and requirePass helpers are defined in coordinates_test.go
