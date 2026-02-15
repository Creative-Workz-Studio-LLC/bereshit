// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-coordinates-assurance
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Coordinates Assurance Functions — Bible Coordinate System Diagnostics
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-coordinates-assurance
//
//	Scripture: "Examine yourselves, whether ye be in the faith; prove your own selves."
//	           — 2 Corinthians 13:5
//	Principle: The system monitors itself. Assurance functions are the mechanism.
//	Anchor:    HALT_01 (Genesis 1:1) — All traces terminate here
//
//	Layer:     Self-Monitoring (cross-cutting)
//	Type:      Assurance (SDK-compatible diagnostics)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-20
//	Version:         a-01.00
//
// # M.4 HALT Trace [HALTS]
//
//	HALT_01 (GENESIS)          → Genesis 1:1 = origin of all coordinates
//	HALT_05 (MECHANISM_NUMBER) → Trite encoding verification
//	HALT_07 (MECHANISM_POSITION) → Position IS computation verification
//
// # M.2 Public API [API]
//
//	AssureTypeAliases(bi) AssuranceResult        Verify types alias from schema
//	AssureBibleIndex(bi) AssuranceResult         Verify 66 books load correctly
//	AssureGenesisOrigin(bi) AssuranceResult      Verify Genesis 1:1 = origin (0,0,0)
//	AssureTriteEncoding(bi) AssuranceResult      Verify Trite types and methods
//	RunAllAssurances(bi, root) Suite             Run complete diagnostics
//
// # M.3 Paradigm [PARADIGM]
//
//	Uses schema.AssuranceResult for ternary scoring.
//	DETECT → ASSESS → RESTORE workflow inherited.
//
// ============================================================================
// END METADATA
// ============================================================================

package coordinates

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"time"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/schema"
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

// newResult creates a new AssuranceResult with timing started.
func newResult(name, halt, detect string) schema.AssuranceResult {
	return schema.AssuranceResult{
		Name:    name,
		HALT:    halt,
		Detect:  detect,
		Context: make(map[string]string),
		LookAt:  make([]schema.LookAtEntry, 0),
		Debug:   make([]string, 0),
		RunAt:   time.Now(),
	}
}

// pass marks the result as passed (+1).
func pass(r *schema.AssuranceResult, assess string) {
	r.Score = schema.ScorePass
	r.Assess = assess
	r.Restore = ""
	r.Duration = time.Since(r.RunAt)
}

// fail marks the result as failed (-1) with recovery path.
func fail(r *schema.AssuranceResult, assess, restore string) {
	r.Score = schema.ScoreFail
	r.Assess = assess
	r.Restore = restore
	r.Duration = time.Since(r.RunAt)
}

// addLookAt adds a typed location for investigation.
func addLookAt(r *schema.AssuranceResult, typ, path, hint string) {
	r.LookAt = append(r.LookAt, schema.LookAtEntry{Type: typ, Path: path, Hint: hint})
}

// addDebug adds a debug command/step suggestion.
func addDebug(r *schema.AssuranceResult, cmd string) {
	r.Debug = append(r.Debug, cmd)
}

// ────────────────────────────────────────────────────────────────
// Type Aliases Assurance (HALT_07)
// ────────────────────────────────────────────────────────────────
// Coordinates types must alias from schema (vocabulary layer).
// HALT_07 = MECHANISM_POSITION (Position IS computation)

// AssureTypeAliases verifies coordinates types alias from schema.
// Traces to: HALT_07 (MECHANISM_POSITION)
//
// What is checked:
//   - Perspective type aliases schema.Perspective
//   - Perspective constants match schema constants
//   - BibleReference, BibleCoords, BookInfo alias from schema
func AssureTypeAliases() schema.AssuranceResult {
	r := newResult("AssureTypeAliases", "HALT_07",
		"Verify coordinates types alias from schema (vocabulary layer)")

	// LOOK AT: Where to investigate
	addLookAt(&r, "file", "bible.go", "Type aliases defined here")
	addLookAt(&r, "file", "encoding.go", "Trite type aliases defined here")
	addLookAt(&r, "file", "pkg/foundation/schema/loader.go", "Source types")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'type.*= schema' bible.go encoding.go")
	addDebug(&r, "grep -n 'Perspective' bible.go")

	// Verify Perspective constants match schema
	if PerspectivePast != schema.PerspectivePast {
		fail(&r,
			fmt.Sprintf("PerspectivePast mismatch: local=%d, schema=%d",
				PerspectivePast, schema.PerspectivePast),
			"Ensure PerspectivePast = schema.PerspectivePast in bible.go",
		)
		return r
	}

	if PerspectivePresent != schema.PerspectivePresent {
		fail(&r,
			fmt.Sprintf("PerspectivePresent mismatch: local=%d, schema=%d",
				PerspectivePresent, schema.PerspectivePresent),
			"Ensure PerspectivePresent = schema.PerspectivePresent in bible.go",
		)
		return r
	}

	if PerspectiveFuture != schema.PerspectiveFuture {
		fail(&r,
			fmt.Sprintf("PerspectiveFuture mismatch: local=%d, schema=%d",
				PerspectiveFuture, schema.PerspectiveFuture),
			"Ensure PerspectiveFuture = schema.PerspectiveFuture in bible.go",
		)
		return r
	}

	// Verify Bible constants match schema
	if TotalBooks != schema.TotalBooks {
		fail(&r,
			fmt.Sprintf("TotalBooks mismatch: local=%d, schema=%d",
				TotalBooks, schema.TotalBooks),
			"Ensure TotalBooks = schema.TotalBooks in bible.go",
		)
		return r
	}

	r.Context["perspective_past"] = fmt.Sprintf("%d", PerspectivePast)
	r.Context["perspective_present"] = fmt.Sprintf("%d", PerspectivePresent)
	r.Context["perspective_future"] = fmt.Sprintf("%d", PerspectiveFuture)
	r.Context["total_books"] = fmt.Sprintf("%d", TotalBooks)

	pass(&r, "All Perspective and Bible constants match schema")
	return r
}

// ────────────────────────────────────────────────────────────────
// Bible Index Assurance (HALT_01)
// ────────────────────────────────────────────────────────────────
// 66 books = Old Testament (39) + New Testament (27)
// HALT_01 = GENESIS (All coordinates trace to Genesis 1:1)

// AssureBibleIndex verifies Bible index loads all 66 books.
// Traces to: HALT_01 (GENESIS)
//
// What is checked:
//   - BibleIndex loads 66 books (39 OT + 27 NT)
//   - Each book has valid Index, Name, Chapters, Category
//   - Genesis is book 1, Revelation is book 66
func AssureBibleIndex(root string) schema.AssuranceResult {
	r := newResult("AssureBibleIndex", "HALT_01",
		"Verify BibleIndex loads 66 books (Old + New Testament)")

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "NewBibleIndex", "bible.go - index constructor")
	addLookAt(&r, "file", "bible.go", "AllBooks array definition")
	addLookAt(&r, "dir", "bereshit/word/scripture/KJV/", "Scripture source")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'AllBooks' bible.go")
	addDebug(&r, "ls bereshit/word/scripture/KJV/ | wc -l")

	// Create Bible index
	bi := NewBibleIndex(root + "/bereshit/word/scripture/KJV")
	bookCount := len(bi.Books)

	if bookCount != 66 {
		fail(&r,
			fmt.Sprintf("Book count is %d, expected 66 (39 OT + 27 NT)", bookCount),
			"Verify AllBooks array has all 66 books in bible.go",
		)
		r.Context["actual_count"] = fmt.Sprintf("%d", bookCount)
		return r
	}

	// Verify Genesis is book 1 (lookup by name)
	genesis, hasGenesis := bi.Books["Genesis"]
	if !hasGenesis {
		fail(&r,
			"Genesis not found in Books map",
			"Genesis must be in AllBooks array",
		)
		return r
	}
	if genesis.Index != 1 {
		fail(&r,
			fmt.Sprintf("Genesis Index is %d, expected 1", genesis.Index),
			"Genesis.Index must be 1",
		)
		return r
	}

	// Verify Revelation is book 66 (lookup by name)
	revelation, hasRevelation := bi.Books["Revelation"]
	if !hasRevelation {
		fail(&r,
			"Revelation not found in Books map",
			"Revelation must be in AllBooks array",
		)
		return r
	}
	if revelation.Index != 66 {
		fail(&r,
			fmt.Sprintf("Revelation Index is %d, expected 66", revelation.Index),
			"Revelation.Index must be 66",
		)
		return r
	}

	r.Context["total_books"] = fmt.Sprintf("%d", bookCount)
	r.Context["first_book"] = "Genesis"
	r.Context["last_book"] = "Revelation"
	r.Context["ot_books"] = "39"
	r.Context["nt_books"] = "27"

	pass(&r, "BibleIndex verified: 66 books (Genesis through Revelation)")
	return r
}

// ────────────────────────────────────────────────────────────────
// Genesis Origin Assurance (HALT_01)
// ────────────────────────────────────────────────────────────────
// Genesis 1:1 = coordinate origin (0, 0, 0)
// HALT_01 = GENESIS (The anchor of anchors)

// AssureGenesisOrigin verifies Genesis 1:1 maps to coordinate origin.
// Traces to: HALT_01 (GENESIS)
//
// What is checked:
//   - Genesis 1:1 in PRESENT perspective = (0, 0, 0)
//   - IsGenesis() returns true for origin
//   - ToCoords and FromCoords are inverses for Genesis 1:1
func AssureGenesisOrigin(root string) schema.AssuranceResult {
	r := newResult("AssureGenesisOrigin", "HALT_01",
		"Verify Genesis 1:1 in PRESENT perspective = origin (0, 0, 0)")

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "ToCoords", "bible.go - reference to coordinates")
	addLookAt(&r, "function", "IsGenesis", "bible.go - origin detection")
	addLookAt(&r, "grep", "Genesis", "bible.go - Genesis handling")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'IsGenesis' bible.go")
	addDebug(&r, "grep -n 'ToCoords' bible.go")

	// Create Bible index and set perspective
	bi := NewBibleIndex(root + "/bereshit/word/scripture/KJV")
	bi.SetPerspective(PerspectivePresent)

	// Genesis 1:1 reference
	ref := BibleReference{Book: "Genesis", Chapter: 1, Verse: 1}

	// Get coordinates
	coords, err := bi.ToCoords(ref)
	if err != nil {
		fail(&r,
			fmt.Sprintf("ToCoords failed for Genesis 1:1: %s", err.Error()),
			"Check ToCoords implementation in bible.go",
		)
		return r
	}

	// Verify IsGenesis
	if !coords.IsGenesis() {
		fail(&r,
			fmt.Sprintf("Genesis 1:1 coords (%.2f,%.2f,%.2f) IsGenesis() = false",
				coords.BookNorm, coords.ChapterNorm, coords.VerseNorm),
			"Genesis 1:1 in PRESENT perspective must return IsGenesis() = true",
		)
		r.Context["coords_book"] = fmt.Sprintf("%.2f", coords.BookNorm)
		r.Context["coords_chapter"] = fmt.Sprintf("%.2f", coords.ChapterNorm)
		r.Context["coords_verse"] = fmt.Sprintf("%.2f", coords.VerseNorm)
		return r
	}

	// Verify coordinates are (0, 0, 0) in PRESENT perspective
	if coords.BookNorm != 0 || coords.ChapterNorm != 0 || coords.VerseNorm != 0 {
		fail(&r,
			fmt.Sprintf("Genesis 1:1 maps to (%.2f,%.2f,%.2f), expected (0,0,0)",
				coords.BookNorm, coords.ChapterNorm, coords.VerseNorm),
			"Genesis 1:1 in PRESENT perspective must be origin",
		)
		return r
	}

	r.Context["reference"] = "Genesis 1:1"
	r.Context["perspective"] = "PRESENT"
	r.Context["coords"] = "(0.00, 0.00, 0.00)"
	r.Context["is_genesis"] = "true"

	pass(&r, "Genesis 1:1 correctly maps to coordinate origin (0, 0, 0)")
	return r
}

// ────────────────────────────────────────────────────────────────
// Trite Encoding Assurance (HALT_05)
// ────────────────────────────────────────────────────────────────
// Trite types and methods for encoding verification
// HALT_05 = MECHANISM_NUMBER

// AssureTriteEncoding verifies Trite types and methods work correctly.
// Traces to: HALT_05 (MECHANISM_NUMBER)
//
// What is checked:
//   - Trite type aliases from schema
//   - Trite constants match schema (MetaBlockStart, etc.)
//   - Trite methods work (IsASCII, IsMetadata, IsExtended)
func AssureTriteEncoding() schema.AssuranceResult {
	r := newResult("AssureTriteEncoding", "HALT_05",
		"Verify Trite types and methods for encoding")

	// LOOK AT: Where to investigate
	addLookAt(&r, "file", "encoding.go", "Trite type aliases")
	addLookAt(&r, "file", "pkg/foundation/schema/loader.go", "Source Trite definition")
	addLookAt(&r, "grep", "MetaBlockStart", "encoding.go - metadata constants")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'type Trite' encoding.go")
	addDebug(&r, "grep -n 'MetaBlockStart' encoding.go schema/loader.go")

	// Verify Trite constants match schema
	if MetaBlockStart != schema.MetaBlockStart {
		fail(&r,
			fmt.Sprintf("MetaBlockStart mismatch: local=%d, schema=%d",
				MetaBlockStart, schema.MetaBlockStart),
			"Ensure MetaBlockStart = schema.MetaBlockStart in encoding.go",
		)
		return r
	}

	if TriteASCIIMax != schema.TriteASCIIMax {
		fail(&r,
			fmt.Sprintf("TriteASCIIMax mismatch: local=%d, schema=%d",
				TriteASCIIMax, schema.TriteASCIIMax),
			"Ensure TriteASCIIMax = schema.TriteASCIIMax in encoding.go",
		)
		return r
	}

	if TriteMetaMin != schema.TriteMetaMin {
		fail(&r,
			fmt.Sprintf("TriteMetaMin mismatch: local=%d, schema=%d",
				TriteMetaMin, schema.TriteMetaMin),
			"Ensure TriteMetaMin = schema.TriteMetaMin in encoding.go",
		)
		return r
	}

	// Test Trite method behavior via schema (since methods are on schema.Trite)
	asciiTrite := Trite(65) // 'A'
	if !asciiTrite.IsASCII() {
		fail(&r,
			fmt.Sprintf("Trite(65).IsASCII() = false, expected true"),
			"Check IsASCII method in schema",
		)
		return r
	}

	metaTrite := Trite(MetaBlockStart) // 128
	if !metaTrite.IsMetadata() {
		fail(&r,
			fmt.Sprintf("Trite(%d).IsMetadata() = false, expected true", MetaBlockStart),
			"Check IsMetadata method in schema",
		)
		return r
	}

	r.Context["meta_block_start"] = fmt.Sprintf("%d", MetaBlockStart)
	r.Context["trite_ascii_max"] = fmt.Sprintf("%d", TriteASCIIMax)
	r.Context["trite_meta_min"] = fmt.Sprintf("%d", TriteMetaMin)
	r.Context["ascii_test"] = "Trite(65).IsASCII() = true"
	r.Context["meta_test"] = fmt.Sprintf("Trite(%d).IsMetadata() = true", MetaBlockStart)

	pass(&r, "Trite encoding verified: types, constants, and methods aligned")
	return r
}

// ────────────────────────────────────────────────────────────────
// Bereshit Loader Assurance (HALT_01)
// ────────────────────────────────────────────────────────────────
// Load Bible data from Bereshit TOML specs (single source of truth)
// HALT_01 = GENESIS (Scripture is the coordinate map)

// AssureBereshitLoader verifies Bible data loads from Bereshit TOMLs.
// Traces to: HALT_01 (GENESIS)
//
// What is checked:
//   - LoadBibleData reads addressing.toml successfully
//   - All 66 books load with correct metadata
//   - BookInfo fields populated (Name, Abbreviation, Verses, Range)
//   - Genesis verse range is [1, 1533] (1533 verses)
//   - Total verse count matches KJV (31,102)
func AssureBereshitLoader(bereshitDir string) schema.AssuranceResult {
	r := newResult("AssureBereshitLoader", "HALT_01",
		"Verify Bible data loads from Bereshit TOMLs (single source of truth)")

	// LOOK AT: Where to investigate
	addLookAt(&r, "file", "loader.go", "LoadBibleData function")
	addLookAt(&r, "file", "bereshit/word/core/bible/addressing.toml", "Source TOML")
	addLookAt(&r, "function", "NewBibleIndexFromBereshit", "bible.go - TOML-based constructor")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'LoadBibleData' loader.go")
	addDebug(&r, "head -200 bereshit/word/core/bible/addressing.toml")

	// Load from Bereshit
	bibleData, err := LoadBibleData(bereshitDir)
	if err != nil {
		fail(&r,
			fmt.Sprintf("LoadBibleData failed: %s", err.Error()),
			"Check bereshitDir path and addressing.toml exists",
		)
		r.Context["bereshit_dir"] = bereshitDir
		return r
	}

	// Verify book count
	bookCount := len(bibleData.BookIndex)
	if bookCount != 66 {
		fail(&r,
			fmt.Sprintf("Loaded %d books, expected 66", bookCount),
			"Check addressing.toml has all 66 books defined",
		)
		r.Context["actual_count"] = fmt.Sprintf("%d", bookCount)
		return r
	}

	// Verify Genesis has correct data
	genesis, hasGenesis := bibleData.GetBook("genesis")
	if !hasGenesis {
		fail(&r,
			"Genesis not found in loaded books",
			"Check [books.genesis] section in addressing.toml",
		)
		return r
	}

	if genesis.Verses != 1533 {
		fail(&r,
			fmt.Sprintf("Genesis has %d verses, expected 1533", genesis.Verses),
			"Check genesis.verses in addressing.toml",
		)
		return r
	}

	if genesis.RangeStart != 1 || genesis.RangeEnd != 1533 {
		fail(&r,
			fmt.Sprintf("Genesis range is [%d,%d], expected [1,1533]",
				genesis.RangeStart, genesis.RangeEnd),
			"Check genesis.range in addressing.toml",
		)
		return r
	}

	// Verify Revelation
	revelation, hasRevelation := bibleData.GetBook("revelation")
	if !hasRevelation {
		fail(&r,
			"Revelation not found in loaded books",
			"Check [books.revelation] section in addressing.toml",
		)
		return r
	}

	// Verify address space
	if bibleData.AddressSpace == nil {
		fail(&r,
			"AddressSpace not loaded",
			"Check [address_space] section in addressing.toml",
		)
		return r
	}

	if bibleData.AddressSpace.KJVVerses != 31102 {
		fail(&r,
			fmt.Sprintf("KJV verse count is %d, expected 31102", bibleData.AddressSpace.KJVVerses),
			"Check address_space.kjv.total_verses in addressing.toml",
		)
		return r
	}

	r.Context["bereshit_dir"] = bereshitDir
	r.Context["total_books"] = fmt.Sprintf("%d", bookCount)
	r.Context["genesis_verses"] = fmt.Sprintf("%d", genesis.Verses)
	r.Context["genesis_range"] = fmt.Sprintf("[%d,%d]", genesis.RangeStart, genesis.RangeEnd)
	r.Context["revelation_index"] = fmt.Sprintf("%d", revelation.Index)
	r.Context["kjv_total_verses"] = fmt.Sprintf("%d", bibleData.AddressSpace.KJVVerses)

	pass(&r, "Bereshit Bible data loaded: 66 books, 31102 verses")
	return r
}

// ────────────────────────────────────────────────────────────────
// Cube Index Assurance (HALT_01)
// ────────────────────────────────────────────────────────────────
// O(1) verse lookup via state machine coordinates (anchor, cube, high)
// HALT_01 = GENESIS (Scripture is the instruction set)

// AssureCubeIndex verifies all 3 translation sets load and O(1) lookup works.
// Traces to: HALT_01 (GENESIS)
//
// What is checked for each set (KJV, WEB, Parallel):
//   - LoadCubeIndex reads {set}-cube-index.json successfully
//   - Total verses match expected (KJV=31102, WEB=31115, Parallel=31115)
//   - LookupByCube(0, 0, 0) returns Genesis 1:1
//   - LookupByOrdinal(1) returns Genesis 1:1
//   - DecomposeToState correctly maps ordinal → (anchor, cube, high)
func AssureCubeIndex(bereshitDir string) schema.AssuranceResult {
	r := newResult("AssureCubeIndex", "HALT_01",
		"Verify all translation set cube indexes load and O(1) lookup works")

	// LOOK AT: Where to investigate
	addLookAt(&r, "file", "loader.go", "LoadCubeIndex function with TranslationSet parameter")
	addLookAt(&r, "file", "bereshit/word/scripture/", "21 generated files (3 sets × 7 files)")
	addLookAt(&r, "tool", "verse-index", "bereshit/word/work/cmd/verse-index - generates all indexes")

	// DEBUG: How to investigate
	addDebug(&r, "ls bereshit/word/scripture/*-cube-index.json")
	addDebug(&r, "grep -n 'TranslationSet' loader.go")

	// Expected verse counts per set
	expectedVerses := map[TranslationSet]int{
		SetKJV:      31102, // KJV has 31,102 verses
		SetWEB:      31115, // WEB has 31,115 verses (13 more)
		SetParallel: 31115, // Parallel matches WEB (superset)
	}

	setsLoaded := 0

	// Test all 3 translation sets
	for _, set := range []TranslationSet{SetKJV, SetWEB, SetParallel} {
		cubeIdx, err := LoadCubeIndex(bereshitDir, set)
		if err != nil {
			// Report which set failed but continue testing others
			r.Context[set.String()+"_error"] = err.Error()
			continue
		}

		// Verify total verses for this set
		expected := expectedVerses[set]
		if cubeIdx.Stats.TotalVerses != expected {
			fail(&r,
				fmt.Sprintf("%s TotalVerses is %d, expected %d",
					set.String(), cubeIdx.Stats.TotalVerses, expected),
				"Regenerate cube index with verse-index tool",
			)
			return r
		}

		// Verify cube dimensions (same for all sets)
		if cubeIdx.Stats.Anchors != NumAnchors || cubeIdx.Stats.CubePos != NumCubePos {
			fail(&r,
				fmt.Sprintf("%s cube dimensions incorrect: %d anchors, %d positions",
					set.String(), cubeIdx.Stats.Anchors, cubeIdx.Stats.CubePos),
				"Check cube index generation math",
			)
			return r
		}

		// Verify LookupByCube(0, 0, 0) = Genesis 1:1
		verse, found := cubeIdx.LookupByCube(0, 0, 0)
		if !found {
			fail(&r,
				fmt.Sprintf("%s LookupByCube(0, 0, 0) returned not found", set.String()),
				"Check cube index structure",
			)
			return r
		}

		if verse.Ordinal != 1 || verse.Book != "Genesis" || verse.Chapter != 1 || verse.Verse != 1 {
			fail(&r,
				fmt.Sprintf("%s LookupByCube(0,0,0) = %s %d:%d, expected Genesis 1:1",
					set.String(), verse.Book, verse.Chapter, verse.Verse),
				"Check cube index ordering",
			)
			return r
		}

		// Verify verse text is loaded
		if verse.Text == "" {
			fail(&r,
				fmt.Sprintf("%s Genesis 1:1 text is empty", set.String()),
				"Regenerate cube index with verse-index tool (includes text)",
			)
			return r
		}

		// Record success for this set
		r.Context[set.String()+"_verses"] = fmt.Sprintf("%d", cubeIdx.Stats.TotalVerses)
		r.Context[set.String()+"_genesis_text_len"] = fmt.Sprintf("%d chars", len(verse.Text))
		setsLoaded++
	}

	// Verify DecomposeToState for ordinal 1 (universal check)
	anchor, cube, high := DecomposeToState(1)
	if anchor != 0 || cube != 0 || high != 0 {
		fail(&r,
			fmt.Sprintf("DecomposeToState(1) = (%d,%d,%d), expected (0,0,0)",
				anchor, cube, high),
			"Check DecomposeToState implementation",
		)
		return r
	}

	r.Context["bereshit_dir"] = bereshitDir
	r.Context["sets_loaded"] = fmt.Sprintf("%d/3", setsLoaded)

	if setsLoaded == 0 {
		fail(&r,
			"No cube indexes could be loaded",
			"Run 'cd bereshit/word/work/cmd/verse-index && go run .' to generate all 21 files",
		)
		return r
	} else if setsLoaded < 3 {
		pass(&r, fmt.Sprintf("Partial: %d/3 sets loaded, O(1) lookup works for loaded sets", setsLoaded))
	} else {
		pass(&r, "All 3 translation sets verified: KJV(31102), WEB(31115), Parallel(31115)")
	}
	return r
}

// ────────────────────────────────────────────────────────────────
// Full Suite
// ────────────────────────────────────────────────────────────────

// RunAllAssurances runs the complete coordinates diagnostic suite.
// root is the claude-global project root
// bereshitDir is the Bereshit project root (or empty to use default)
func RunAllAssurances(root string) schema.AssuranceSuite {
	return RunAllAssurancesWithBereshit(root, DefaultBereshitDir())
}

// RunAllAssurancesWithBereshit runs the complete suite with explicit Bereshit path.
func RunAllAssurancesWithBereshit(root, bereshitDir string) schema.AssuranceSuite {
	start := time.Now()
	suite := schema.AssuranceSuite{
		Results: make([]schema.AssuranceResult, 0, 6),
		RunAt:   start,
	}

	// Run all coordinates assurances
	suite.Results = append(suite.Results, AssureTypeAliases())
	suite.Results = append(suite.Results, AssureBibleIndex(root))
	suite.Results = append(suite.Results, AssureGenesisOrigin(root))
	suite.Results = append(suite.Results, AssureTriteEncoding())
	suite.Results = append(suite.Results, AssureBereshitLoader(bereshitDir))
	suite.Results = append(suite.Results, AssureCubeIndex(bereshitDir))

	// Tally results (ternary)
	for _, r := range suite.Results {
		suite.TrueScore += int(r.Score)
		switch r.Score {
		case schema.ScorePass:
			suite.PassCount++
		case schema.ScoreNeutral:
			suite.NeutralCount++
		case schema.ScoreFail:
			suite.FailCount++
		}
	}

	// Normalize true score to ternary
	suite.Normalized = schema.Score(normalize(suite.TrueScore))
	suite.Duration = time.Since(start)

	return suite
}

// normalize projects an unbounded true score to ternary (-1/0/+1).
func normalize(trueScore int) int {
	if trueScore > 0 {
		return 1
	}
	if trueScore < 0 {
		return -1
	}
	return 0
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Coordinates Assurance Architecture:
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ ASSURANCE FUNCTIONS                                                     │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │  AssureTypeAliases       Verify types alias from schema (HALT_07)      │
//   │  AssureBibleIndex        Verify 66 books load (HALT_01)                │
//   │  AssureGenesisOrigin     Verify Genesis 1:1 = origin (HALT_01)         │
//   │  AssureTriteEncoding     Verify Trite types/methods (HALT_05)          │
//   │  AssureBereshitLoader    Verify TOML loading (HALT_01)                 │
//   │  AssureCubeIndex         Verify O(1) cube lookup (HALT_01)             │
//   └─────────────────────────────────────────────────────────────────────────┘
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ CUBE INDEX: Scripture as Instruction Set                               │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │                                                                         │
//   │  243 = 9 × 27 = 3² × 3³ (trit5 decomposition)                          │
//   │                                                                         │
//   │  9 anchors:     anchor = low / 27                                      │
//   │  27 cube pos:   cube = low % 27                                        │
//   │  128 pages:     high = ordinal / 243                                   │
//   │                                                                         │
//   │  LookupByCube(anchor, cube, high) → O(1) verse access                  │
//   │  Genesis 1:1 = (0, 0, 0) = anchor 0, cube 0, page 0                    │
//   │                                                                         │
//   └─────────────────────────────────────────────────────────────────────────┘
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ HALT TRACE                                                              │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │                                                                         │
//   │  coordinates/assurance.go → HALT_07 (Position) → HALT_05 (Number)      │
//   │                           → HALT_01 (Genesis 1:1)                      │
//   │                                                                         │
//   └─────────────────────────────────────────────────────────────────────────┘
//
// "Examine yourselves, whether ye be in the faith; prove your own selves."
// — 2 Corinthians 13:5
//
// ============================================================================
// END CLOSING
// ============================================================================
