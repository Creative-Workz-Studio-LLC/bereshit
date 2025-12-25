// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-cmd-cpisi-explore
// Purpose: Explore mode — test theories and constants
// Biblical: Proverbs 25:2 - "It is the glory of God to conceal a thing: but the honour of kings is to search out a matter"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-20
//
// Explore mode for testing theories, constants, and encoding.
// Machine-friendly output for research and discovery.
//
// ═══════════════════════════════════════════════════════════════════════════

package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"math"

	"creativeworkzstudio.com/bereshit/word/work/pkg/core/coordinates"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Explore Mode Entry ---

func runExplore(args []string) {
	if len(args) == 0 {
		printExploreUsage()
		return
	}

	switch args[0] {
	case "range":
		exploreRange(args[1:])
	case "lambda":
		exploreLambda(args[1:])
	case "zpairs":
		exploreZPairs()
	case "constants":
		exploreConstants()
	case "transform":
		exploreTransform(args[1:])
	case "perspective":
		explorePerspective(args[1:])
	// Encoding commands (from Bereshit specs)
	case "word":
		exploreWord(args[1:])
	case "trite":
		exploreTrite(args[1:])
	case "trace":
		exploreTrace(args[1:])
	case "book":
		exploreBook(args[1:])
	case "roots":
		exploreRoots()
	case "meta":
		exploreMeta()
	case "encode":
		exploreEncode(args[1:])
	default:
		fmt.Printf("Unknown explore subcommand: %s\n", args[0])
		printExploreUsage()
	}
}

func printExploreUsage() {
	fmt.Println(`Explore Mode - Test theories and constants

Usage: cpisi explore <subcommand> [args]

Subcommands:
  range [min] [max]      Scan N range, show coordinate bounds (default: -30 to 30)
  lambda <value>         Test different λ values, compare to default
  zpairs                 Show Z-pairing interconnections between perspectives
  constants              Show all defined constants from statemachine
  transform <x> <y> <z>  Transform coordinates between PAST/PRESENT/FUTURE
  perspective [ref]      Show verse coordinates in all perspectives (default: Genesis 1:1)

Encoding (from Bereshit specs):
  word <word>            Show letter-sum, verse mapping, coordinates
  trite <value>          Show trite classification (ASCII/metadata/extended)
  trace <keyword>        Trace OmniCode keyword to Scripture source
  book <name>            Show book ordinal range
  roots                  List all Hebrew roots with OmniCode mappings
  meta                   Show all 13 CPI-SI metadata markers

Pipeline (Sentence → Verse → New Sentence):
  encode <sentence>      Encode sentence through Scripture pipeline`)
}

// --- Range Exploration ---

func exploreRange(args []string) {
	sm := statemachine.NewStateMachine()

	minN, maxN := -30.0, 30.0
	if len(args) >= 2 {
		fmt.Sscanf(args[0], "%f", &minN)
		fmt.Sscanf(args[1], "%f", &maxN)
	}

	fmt.Printf("Exploring Ψ(N) coordinate ranges for N ∈ [%.2f, %.2f]\n", minN, maxN)
	fmt.Printf("λ = %.10f (arctanh(0.5))\n\n", statemachine.Lambda)

	var minX, maxX, minY, maxY, minZ, maxZ float64 = 2, -2, 2, -2, 2, -2
	var minXatN, maxXatN, minYatN, maxYatN, minZatN, maxZatN float64

	for n := minN; n <= maxN; n += 0.1 {
		coords := sm.Psi(n)
		if coords.X < minX {
			minX, minXatN = coords.X, n
		}
		if coords.X > maxX {
			maxX, maxXatN = coords.X, n
		}
		if coords.Y < minY {
			minY, minYatN = coords.Y, n
		}
		if coords.Y > maxY {
			maxY, maxYatN = coords.Y, n
		}
		if coords.Z < minZ {
			minZ, minZatN = coords.Z, n
		}
		if coords.Z > maxZ {
			maxZ, maxZatN = coords.Z, n
		}
	}

	fmt.Println("Coordinate Ranges:")
	fmt.Printf("  X ∈ [%.6f, %.6f]  at N=%.2f to N=%.2f\n", minX, maxX, minXatN, maxXatN)
	fmt.Printf("  Y ∈ [%.6f, %.6f]  at N=%.2f to N=%.2f\n", minY, maxY, minYatN, maxYatN)
	fmt.Printf("  Z ∈ [%.6f, %.6f]  at N=%.2f to N=%.2f\n", minZ, maxZ, minZatN, maxZatN)
	fmt.Println()
	fmt.Println("Insights:")
	fmt.Println("  • Ψ(N) maps to ONE OCTANT of the cube")
	fmt.Println("  • X = 0 (PRESENT) unreachable via Ψ(N)")
	fmt.Println("  • Z = 0 (HALT) unreachable via Ψ(N)")
	fmt.Println("  • HALT requires discrete position snapping, not continuous Ψ(N)")
}

// --- Lambda Exploration ---

func exploreLambda(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi explore lambda <value>")
		fmt.Printf("Default λ = %.10f (arctanh(0.5))\n", statemachine.Lambda)
		return
	}

	var testLambda float64
	fmt.Sscanf(args[0], "%f", &testLambda)

	fmt.Printf("Comparing λ values:\n")
	fmt.Printf("  Default: %.10f\n", statemachine.Lambda)
	fmt.Printf("  Testing: %.10f\n\n", testLambda)

	// Test key N values with both lambdas
	testNs := []float64{-10, -1, 0, 1, 10}

	sm := statemachine.NewStateMachine()

	fmt.Println("N\t\tDefault Ψ(N)\t\t\tTest Ψ(N)")
	fmt.Println("─────────────────────────────────────────────────────────────────")
	for _, n := range testNs {
		// Default
		defaultCoords := sm.Psi(n)
		// Test (manual calculation with different lambda)
		testCoords := psiWithLambda(n, testLambda, statemachine.Phi)

		fmt.Printf("%.1f\t(%.4f, %.4f, %.4f)\t(%.4f, %.4f, %.4f)\n",
			n, defaultCoords.X, defaultCoords.Y, defaultCoords.Z,
			testCoords.X, testCoords.Y, testCoords.Z)
	}
}

func psiWithLambda(n, lambda, phi float64) statemachine.Coordinates {
	tanhLN := math.Tanh(lambda * n)
	clamped := -tanhLN
	if clamped > 1 {
		clamped = 1
	}
	if clamped < -1 {
		clamped = -1
	}

	theta := math.Acos(clamped)
	theta3 := theta / 3

	return statemachine.Coordinates{
		X: math.Cos(theta3),
		Y: math.Cos(theta3 - phi),
		Z: math.Cos(theta3 + phi),
	}
}

// --- Z-Pairs Exploration ---

func exploreZPairs() {
	fmt.Println("Z-Pairing Interconnections")
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()
	fmt.Println("Entries at the same Z depth can INFLUENCE each other across")
	fmt.Println("temporal perspectives. Z = -(X + Y) / 2 for compound states.")
	fmt.Println()

	pairs := []struct {
		z       float64
		name    string
		past    string
		present string
		future  string
	}{
		{statemachine.ZDepthHalt, "HALT (-1.0)", "PAST-PAST (Established)", "-", "-"},
		{statemachine.ZDepthMemory, "Memory (-0.5)", "PAST-PRESENT (Memory)", "PRESENT-PAST (Reflection)", "-"},
		{statemachine.ZDepthTransition, "Transition (0.0)", "PAST-FUTURE (Prophecy)", "PRESENT-PRESENT (Origin)", "FUTURE-PAST (Vision)"},
		{statemachine.ZDepthPlanning, "Planning (+0.5)", "-", "PRESENT-FUTURE (Planning)", "FUTURE-PRESENT (Preparation)"},
		{statemachine.ZDepthVision, "Vision (+1.0)", "-", "-", "FUTURE-FUTURE (Aspiration)"},
	}

	fmt.Println("Z-Depth\t\tPAST\t\t\t\tPRESENT\t\t\t\tFUTURE")
	fmt.Println("───────────────────────────────────────────────────────────────────────────────")
	for _, p := range pairs {
		fmt.Printf("%.1f %s\n", p.z, p.name)
		fmt.Printf("\t\t%s\t%s\t%s\n", p.past, p.present, p.future)
	}
	fmt.Println()
	fmt.Println("Key Interconnections:")
	fmt.Println("  • Z = -0.5: Memory (PAST-PRESENT) ↔ Reflection (PRESENT-PAST)")
	fmt.Println("  • Z =  0.0: Prophecy ↔ Origin ↔ Vision")
	fmt.Println("  • Z = +0.5: Planning (PRESENT-FUTURE) ↔ Preparation (FUTURE-PRESENT)")
}

// --- Constants Exploration ---

func exploreConstants() {
	fmt.Println("CPI-SI Constants (from statemachine package)")
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()

	fmt.Println("PSI MASTER CONSTANTS:")
	fmt.Printf("  λ (Lambda)      = %.16f  (arctanh(0.5), maps N=1 → midpoint)\n", statemachine.Lambda)
	fmt.Printf("  φ (Phi)         = %.16f  (2π/3 = 120° phase separation)\n", statemachine.Phi)
	fmt.Printf("  Scale           = %d                       (Base100 scaling)\n", statemachine.Scale)
	fmt.Println()

	fmt.Println("PSI STRUCTURAL:")
	fmt.Printf("  SatThreshold    = %.2f              (λ × N_sat, display precision)\n", statemachine.SatThreshold)
	fmt.Printf("  NSat            = %.2f              (saturation boundary)\n", statemachine.NSat)
	fmt.Printf("  NMid            = %.1f                (midpoint by design)\n", statemachine.NMid)
	fmt.Println()

	fmt.Println("PSI COORDINATE RANGES:")
	fmt.Printf("  X ∈ [%.1f, %.1f]  (FUTURE-leaning, never 0)\n", statemachine.PsiXMin, statemachine.PsiXMax)
	fmt.Printf("  Y ∈ [%.1f, %.1f]  (Balanced around 0)\n", statemachine.PsiYMin, statemachine.PsiYMax)
	fmt.Printf("  Z ∈ [%.1f, %.1f]  (PAST-leaning, never 0)\n", statemachine.PsiZMin, statemachine.PsiZMax)
	fmt.Println()

	fmt.Println("Z-DEPTH CONSTANTS:")
	fmt.Printf("  ZDepthHalt       = %.1f  (PAST-PAST, Genesis 1:1)\n", statemachine.ZDepthHalt)
	fmt.Printf("  ZDepthMemory     = %.1f  (PAST-PRESENT, PRESENT-PAST)\n", statemachine.ZDepthMemory)
	fmt.Printf("  ZDepthTransition = %.1f   (PAST-FUTURE, PRESENT-PRESENT, FUTURE-PAST)\n", statemachine.ZDepthTransition)
	fmt.Printf("  ZDepthPlanning   = %.1f   (PRESENT-FUTURE, FUTURE-PRESENT)\n", statemachine.ZDepthPlanning)
	fmt.Printf("  ZDepthVision     = %.1f   (FUTURE-FUTURE)\n", statemachine.ZDepthVision)
	fmt.Println()

	fmt.Println("RANKING WEIGHTS:")
	fmt.Printf("  WeightKingdom    = %.1f  (God's work, God's blessing)\n", statemachine.WeightKingdom)
	fmt.Printf("  WeightCovenant   = %.1f  (Relational, partnership)\n", statemachine.WeightCovenant)
	fmt.Printf("  WeightMixed      = %.1f  (Personal but grounded)\n", statemachine.WeightMixed)
	fmt.Printf("  WeightSelf       = %.1f  (Baseline)\n", statemachine.WeightSelf)
	fmt.Println()

	fmt.Println("COHERENCE WEIGHTS (PRESENT ranking):")
	fmt.Printf("  CoherenceOrigin     = %.2f  (At origin)\n", statemachine.CoherenceOrigin)
	fmt.Printf("  CoherenceReflection = %.2f  (Looking back)\n", statemachine.CoherenceReflection)
	fmt.Printf("  CoherencePlanning   = %.2f  (Looking forward)\n", statemachine.CoherencePlanning)
	fmt.Println()

	fmt.Println("CLARITY WEIGHTS (PRESENT ranking):")
	fmt.Printf("  ClarityHalt      = %.1f  (| operator)\n", statemachine.ClarityHalt)
	fmt.Printf("  ClarityAt        = %.1f  (@ operator)\n", statemachine.ClarityAt)
	fmt.Printf("  ClarityEstablish = %.1f  (= operator)\n", statemachine.ClarityEstablish)
	fmt.Printf("  ClarityDefault   = %.1f\n", statemachine.ClarityDefault)
	fmt.Println()

	fmt.Println("TRANSFORM:")
	fmt.Printf("  TransformOffset  = %.1f  (PAST + offset = PRESENT)\n", statemachine.TransformOffset)
}

// --- Transform Exploration ---

func exploreTransform(args []string) {
	if len(args) < 3 {
		fmt.Println("Usage: cpisi explore transform <x> <y> <z>")
		fmt.Println("  Shows how coordinates transform between PAST/PRESENT/FUTURE perspectives")
		return
	}

	var x, y, z float64
	fmt.Sscanf(args[0], "%f", &x)
	fmt.Sscanf(args[1], "%f", &y)
	fmt.Sscanf(args[2], "%f", &z)

	offset := statemachine.TransformOffset

	fmt.Printf("Coordinate Transform for (%.2f, %.2f, %.2f)\n", x, y, z)
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()
	fmt.Println("Assuming input is PRESENT perspective (Genesis at 0,0,0):")
	fmt.Printf("  PRESENT: (%.2f, %.2f, %.2f)\n", x, y, z)
	fmt.Printf("  PAST:    (%.2f, %.2f, %.2f)  [- offset]\n", x-offset, y-offset, z-offset)
	fmt.Printf("  FUTURE:  (%.2f, %.2f, %.2f)  [+ offset]\n", x+offset, y+offset, z+offset)
	fmt.Println()
	fmt.Println("Assuming input is PAST perspective (Genesis at -1,-1,-1):")
	fmt.Printf("  PAST:    (%.2f, %.2f, %.2f)\n", x, y, z)
	fmt.Printf("  PRESENT: (%.2f, %.2f, %.2f)  [+ offset]\n", x+offset, y+offset, z+offset)
	fmt.Printf("  FUTURE:  (%.2f, %.2f, %.2f)  [+ 2×offset]\n", x+2*offset, y+2*offset, z+2*offset)
}

// --- Encoding Exploration Functions (from Bereshit specs) ---

func exploreWord(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi explore word <word>")
		fmt.Println("  Shows letter-sum, verse mapping, and coordinates for a word")
		return
	}

	word := args[0]
	letterSum := coordinates.LetterSum(word)
	verse := coordinates.WordToVerse(word)

	fmt.Printf("Word Analysis: %s\n", word)
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()
	fmt.Printf("Letter Sum (A=1, B=2...Z=26): %d\n", letterSum)
	fmt.Printf("Verse Mapping (mod 31 + 1):   Genesis 1:%d\n", verse)
	fmt.Println()

	// Get coordinates
	bible := coordinates.NewBibleIndex("/media/seanje-lenox-wise/Project/Bereshit/word/scripture")
	coords, err := coordinates.WordToCoords(bible, word)
	if err != nil {
		fmt.Printf("Error getting coordinates: %v\n", err)
		return
	}

	fmt.Println("Coordinates:")
	fmt.Printf("  Book (X):    %.4f\n", coords.BookNorm)
	fmt.Printf("  Chapter (Y): %.4f\n", coords.ChapterNorm)
	fmt.Printf("  Verse (Z):   %.4f\n", coords.VerseNorm)
	fmt.Printf("  Perspective: %s\n", coords.Perspective.String())
	fmt.Println()

	// Show some example words for comparison
	fmt.Println("Notable Word-Verse Alignments:")
	examples := []string{"GOD", "MAN", "EVE", "LIFE", "HELLO"}
	for _, ex := range examples {
		sum := coordinates.LetterSum(ex)
		v := coordinates.WordToVerse(ex)
		fmt.Printf("  %-10s sum=%3d → Genesis 1:%d\n", ex, sum, v)
	}
}

func exploreTrite(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi explore trite <value>")
		fmt.Println("  Shows trite classification (0-255)")
		return
	}

	var value int
	fmt.Sscanf(args[0], "%d", &value)
	if value < 0 || value > 255 {
		fmt.Printf("Trite value must be 0-255, got: %d\n", value)
		return
	}

	t := coordinates.Trite(value)

	fmt.Printf("Trite Analysis: %d\n", value)
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()

	// Classification
	fmt.Printf("Classification:\n")
	if t.IsASCII() {
		fmt.Printf("  Type: Standard ASCII (0-127)\n")
		if value >= 32 && value <= 126 {
			fmt.Printf("  Character: '%s'\n", t.String())
		} else {
			fmt.Printf("  Character: [CONTROL]\n")
		}
	} else if t.IsMetadata() {
		fmt.Printf("  Type: CPI-SI Metadata (128-140)\n")
		fmt.Printf("  Marker: %s\n", t.String())
	} else if coordinates.IsWEBVariant(t) {
		fmt.Printf("  Type: WEB Variant (243-255)\n")
		if variant, ok := coordinates.GetWEBVariant(t); ok {
			fmt.Printf("  Book: %s %d:%d\n", variant.Book, variant.Chapter, variant.Verse)
			fmt.Printf("  Summary: %s\n", variant.Summary)
		}
	} else {
		fmt.Printf("  Type: Extended Character (141-242)\n")
		fmt.Printf("  Byte: 0x%02X\n", value)
	}
	fmt.Println()

	// Trit5 info
	fmt.Printf("Trit5 Packing:\n")
	fmt.Printf("  In data range (0-242): %v\n", value < 243)
	fmt.Printf("  Mod 243: %d\n", value%243)
}

func exploreTrace(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi explore trace <keyword>")
		fmt.Println("  Traces OmniCode keyword to Scripture source")
		fmt.Println()
		fmt.Println("Available keywords: let, be, with, and, or, promises, given,")
		fmt.Println("                   returning, covenant, begat, distinct, called, void")
		return
	}

	keyword := args[0]
	scripture, text, hebrewRoot := coordinates.TraceToScripture(keyword)

	if scripture == "" {
		fmt.Printf("Keyword '%s' not found in decoding table\n", keyword)
		return
	}

	fmt.Printf("Keyword Trace: %s\n", keyword)
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()
	fmt.Printf("Scripture:    %s\n", scripture)
	fmt.Printf("Text:         %s\n", text)
	fmt.Printf("Hebrew Root:  %s\n", hebrewRoot)
	fmt.Println()

	// Get the full Hebrew root info
	if root, ok := coordinates.GetHebrewRoot(hebrewRoot); ok {
		fmt.Printf("Root Details:\n")
		fmt.Printf("  Hebrew:     %s\n", root.Root)
		fmt.Printf("  Meaning:    %s\n", root.Meaning)
		fmt.Printf("  Field:      %s\n", root.SemanticField)
		fmt.Printf("  OmniCode:   %s\n", root.OmniCodeMapping)
	}
}

func exploreBook(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi explore book <name>")
		fmt.Println("  Shows book ordinal range (e.g., Genesis, Exodus, Matt, Rev)")
		return
	}

	name := args[0]
	book, ok := coordinates.GetBookOrdinal(name)
	if !ok {
		fmt.Printf("Book '%s' not found\n", name)
		fmt.Println("Try: Genesis, Exodus, Psalms, Matthew, John, Revelation")
		return
	}

	fmt.Printf("Book: %s (%s)\n", book.Name, book.Abbreviation)
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()
	fmt.Printf("Index:        %d of 66\n", book.Index)
	fmt.Printf("Chapters:     %d\n", book.Chapters)
	fmt.Printf("Verses:       %d\n", book.Verses)
	fmt.Printf("Ordinal Range: %d - %d\n", book.RangeStart, book.RangeEnd)
	fmt.Println()

	// Show ternary encoding of range
	fmt.Printf("Ternary Encoding:\n")
	fmt.Printf("  Start: %s (ordinal %d)\n", coordinates.OrdinalToTernary(book.RangeStart), book.RangeStart)
	fmt.Printf("  End:   %s (ordinal %d)\n", coordinates.OrdinalToTernary(book.RangeEnd), book.RangeEnd)
	fmt.Println()

	// Show packed representation
	startLow, startHigh := coordinates.PackVerse(coordinates.VerseOrdinal(book.RangeStart))
	endLow, endHigh := coordinates.PackVerse(coordinates.VerseOrdinal(book.RangeEnd))
	fmt.Printf("Trit5 Packed (low, high):\n")
	fmt.Printf("  Start: (%d, %d)\n", startLow, startHigh)
	fmt.Printf("  End:   (%d, %d)\n", endLow, endHigh)
}

func exploreRoots() {
	fmt.Println("Hebrew Roots → OmniCode Mappings")
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()

	roots := []string{"bara", "hayah", "badal", "qara", "asah", "yalad", "brit", "natan", "shuv", "tohu", "vav"}
	fmt.Printf("%-10s %-6s %-25s %-15s %s\n", "Latin", "Hebrew", "Meaning", "Field", "OmniCode")
	fmt.Println("─────────────────────────────────────────────────────────────────────────────")

	for _, name := range roots {
		if root, ok := coordinates.GetHebrewRoot(name); ok {
			fmt.Printf("%-10s %-6s %-25s %-15s %s\n",
				root.Transliteration,
				root.Root,
				truncate(root.Meaning, 25),
				root.SemanticField,
				root.OmniCodeMapping)
		}
	}
	fmt.Println()
	fmt.Println("Use 'cpisi explore trace <keyword>' to trace keywords back to roots")
}

func exploreMeta() {
	fmt.Println("CPI-SI Metadata Markers (128-140)")
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()
	fmt.Println("These 13 markers replace dead C1 control codes (128-140)")
	fmt.Println("Standard ASCII has equivalents for all C1 functionality.")
	fmt.Println()

	markers := []struct {
		value   int
		name    string
		purpose string
	}{
		{128, "BLOCK_START", "Begin block boundary"},
		{129, "BLOCK_END", "End block boundary"},
		{130, "SECTION", "Section delimiter"},
		{131, "ANCHOR", "Mark anchor point (halt condition)"},
		{132, "TRACE", "Trace-back marker (-1 operation)"},
		{133, "BUILD", "Build-up marker (+1 operation)"},
		{134, "HALT", "Stop signal (0 - foundation)"},
		{135, "PERSPECTIVE", "Perspective shift indicator"},
		{136, "COVENANT", "Covenant seal"},
		{137, "VERIFY", "Verification checkpoint"},
		{138, "META_START", "Metadata section begin"},
		{139, "META_END", "Metadata section end"},
		{140, "RESERVED", "Future expansion"},
	}

	fmt.Printf("%-6s %-15s %s\n", "Trite", "Name", "Purpose")
	fmt.Println("─────────────────────────────────────────────────────────────")
	for _, m := range markers {
		t := coordinates.Trite(m.value)
		fmt.Printf("%-6d %-15s %s\n", m.value, t.String(), m.purpose)
	}
	fmt.Println()
	fmt.Println("Byte Layout:")
	fmt.Println("  0-127:   Standard ASCII (text)")
	fmt.Println("  128-140: CPI-SI Metadata (13 markers)")
	fmt.Println("  141-242: Extended characters")
	fmt.Println("  243-255: WEB variant verses (13 spare trit5 states)")
}

func truncate(s string, max int) string {
	if len(s) <= max {
		return s
	}
	return s[:max-3] + "..."
}

func explorePerspective(args []string) {
	bible := coordinates.NewBibleIndex("/media/seanje-lenox-wise/Project/Bereshit/word/scripture")

	// Default to Genesis 1:1
	ref := coordinates.BibleReference{Book: "Genesis", Chapter: 1, Verse: 1}
	if len(args) >= 3 {
		ref.Book = args[0]
		fmt.Sscanf(args[1], "%d", &ref.Chapter)
		fmt.Sscanf(args[2], "%d", &ref.Verse)
	}

	fmt.Printf("Perspective Comparison for %s %d:%d\n", ref.Book, ref.Chapter, ref.Verse)
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()
	fmt.Println("PERSPECTIVE AFFECTS Z-AXIS ONLY:")
	fmt.Println("  X (Book) and Y (Chapter) are constant across perspectives")
	fmt.Println("  Z (Verse) shifts based on temporal perspective:")
	fmt.Println()
	fmt.Println("    PAST:    Z ∈ [-1.0, 0.0]   — looking at what came before")
	fmt.Println("    PRESENT: Z ∈ [-0.5, 0.5]   — centered on now")
	fmt.Println("    FUTURE:  Z ∈ [0.0, 1.0]    — looking at what comes after")
	fmt.Println()

	// Get coordinates in each perspective
	perspectives := []coordinates.Perspective{
		coordinates.PerspectivePast,
		coordinates.PerspectivePresent,
		coordinates.PerspectiveFuture,
	}

	fmt.Println("Perspective\tZ Range\t\t\tVerse Coords (X, Y, Z)")
	fmt.Println("───────────────────────────────────────────────────────────────")

	for _, p := range perspectives {
		bible.SetPerspective(p)
		coords, err := bible.ToCoords(ref)
		if err != nil {
			fmt.Printf("Error: %v\n", err)
			continue
		}

		var zRange string
		switch p {
		case coordinates.PerspectivePast:
			zRange = "[-1.0, 0.0]"
		case coordinates.PerspectivePresent:
			zRange = "[-0.5, 0.5]"
		case coordinates.PerspectiveFuture:
			zRange = "[0.0, 1.0]"
		}

		fmt.Printf("%-10s\t%s\t\t(%.4f, %.4f, %.4f)\n",
			p.String(), zRange,
			coords.BookNorm, coords.ChapterNorm, coords.VerseNorm)
	}

	fmt.Println()
	fmt.Println("Z Transform Logic (verse depth only):")
	fmt.Println("  verseBase = (verse - 1) / 40.0  → [0, 1]")
	fmt.Println("  PAST:    Z = verseBase - 1.0    → [-1, 0]")
	fmt.Println("  PRESENT: Z = verseBase - 0.5    → [-0.5, 0.5]")
	fmt.Println("  FUTURE:  Z = verseBase          → [0, 1]")
	fmt.Println()

	// Show if this is Genesis 1:1
	if ref.Book == "Genesis" && ref.Chapter == 1 && ref.Verse == 1 {
		fmt.Println("This IS Genesis 1:1 — the HALT condition.")
		fmt.Println("  X = -1 (Book 1, leftmost)")
		fmt.Println("  Y = -1 (Chapter 1, bottom)")
		fmt.Println("  Z varies by perspective:")
		fmt.Println("    • PAST:    Z = -1.0  (bottom of past range)")
		fmt.Println("    • PRESENT: Z = -0.5  (bottom of present range)")
		fmt.Println("    • FUTURE:  Z = 0.0   (bottom of future range)")
	}
}

// --- Sentence Encoding Pipeline ---

func exploreEncode(args []string) {
	if len(args) == 0 {
		fmt.Println("Usage: cpisi explore encode <sentence>")
		fmt.Println("  Encodes a sentence through the Scripture pipeline")
		fmt.Println()
		fmt.Println("Example:")
		fmt.Println("  cpisi explore encode Hello world")
		fmt.Println("  cpisi explore encode \"God created the heavens\"")
		return
	}

	// Join args into sentence
	sentence := ""
	for i, arg := range args {
		if i > 0 {
			sentence += " "
		}
		sentence += arg
	}

	fmt.Printf("Sentence Encoding Pipeline\n")
	fmt.Println("══════════════════════════════════════════════════════════════")
	fmt.Println()

	// Create pipeline
	pipeline, err := coordinates.NewPipeline(coordinates.DefaultBereshitDir())
	if err != nil {
		fmt.Printf("Error creating pipeline: %v\n", err)
		return
	}

	// Run encoding
	analysis := pipeline.Encode(sentence)

	// Display results
	fmt.Printf("Input: %s\n\n", analysis.Original)

	fmt.Println("Word → Letter Sum → Verse Mapping")
	fmt.Println("────────────────────────────────────────────────────────────")
	for _, wa := range analysis.Words {
		fmt.Printf("  %-12s  sum=%3d  →  %-15s\n", wa.Word, wa.LetterSum, wa.Reference)
		if wa.VerseText != "" && wa.VerseText != "(verse not found)" {
			// Truncate long verses
			text := wa.VerseText
			if len(text) > 60 {
				text = text[:57] + "..."
			}
			fmt.Printf("                    \"%s\"\n", text)
		}
	}

	fmt.Println()
	fmt.Printf("Grounded in: %d unique verses\n", len(analysis.GroundedIn))
	for _, ref := range analysis.GroundedIn {
		fmt.Printf("  • %s\n", ref)
	}

	fmt.Println()
	fmt.Println("Generated Response:")
	fmt.Println("────────────────────────────────────────────────────────────")
	fmt.Printf("  %s\n", analysis.NewSentence)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Explore mode is for testing theories and constants:
// - Range exploration of Ψ(N)
// - Lambda value testing
// - Z-pairing interconnections
// - Constants display
// - Coordinate transforms
// - Encoding exploration (word, trite, trace, book, roots, meta)
//
// ═══════════════════════════════════════════════════════════════════════════
