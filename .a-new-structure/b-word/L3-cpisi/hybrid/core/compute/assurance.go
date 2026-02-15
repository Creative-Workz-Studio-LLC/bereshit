// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-cpisi-compute-assurance
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Cube Assurance Functions — Geometric Computation Diagnostics
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-cpisi-compute-assurance
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
//	HALT_03 (EQUATION_PSI)     → 27 = 3³ cube positions verification
//	HALT_05 (MECHANISM_NUMBER) → Number → Transform → Number verification
//	HALT_06 (MECHANISM_SHAPE)  → Shape ↔ Number translation verification
//	HALT_07 (MECHANISM_POSITION) → Position IS computation verification
//	HALT_09 (MECHANISM_WHEEL)  → Cycle formula (value*3+perspective)%243
//	HALT_10 (STATE_HEBREW)     → 7 Days of Creation = 7 States
//	HALT_01 (GENESIS)          → All traces terminate at Genesis 1:1
//
// # M.2 Public API [API]
//
//	AssureCubeConstants(c) AssuranceResult       Verify Base=3, DataModulo=243
//	AssureShapeTranslation(c) AssuranceResult    Verify Shape ↔ Number (27 positions)
//	AssureCycleFormula(c) AssuranceResult        Verify (value*3+perspective)%243
//	AssureTriteConversion(c) AssuranceResult     Verify Trite ↔ Position mapping
//	AssureSevenDays(c) AssuranceResult           Verify CreationDay → 7 Days
//	RunAllAssurances(c) Suite                    Run complete diagnostics
//
// # M.3 Paradigm [PARADIGM]
//
//	Uses schema.AssuranceResult for ternary scoring.
//	DETECT → ASSESS → RESTORE workflow inherited.
//
// ============================================================================
// END METADATA
// ============================================================================

package compute

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"time"

	"cws.studio/pkg/foundation/schema"
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
// Cube Constants Assurance (HALT_03 + HALT_05)
// ────────────────────────────────────────────────────────────────
// The cube operates on ternary math constants.
// HALT_03 = PSI (27 = 3³ positions)
// HALT_05 = NUMBER (243 = 3⁵ data range)

// AssureCubeConstants verifies cube mathematical constants.
// Traces to: HALT_03 (EQUATION_PSI), HALT_05 (MECHANISM_NUMBER)
//
// What is checked:
//   - Base = 3 (ternary)
//   - DataModulo = 243 (3⁵)
//   - ByteModulo = 256 (2⁸)
func AssureCubeConstants(c *Cube) schema.AssuranceResult {
	r := newResult("AssureCubeConstants", "HALT_03",
		"Verify cube constants: Base=3, DataModulo=243, ByteModulo=256")

	if c == nil {
		fail(&r, "Cube is nil", "Create cube with NewCube first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "NewCube", "cube.go - constructor sets constants")
	addLookAt(&r, "file", "cube.schema.toml", "L2 GEOMETRY schema")
	addLookAt(&r, "grep", "DataModulo", "cube.go - data range constant")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'Base.*=' cube.go")
	addDebug(&r, "grep -n 'DataModulo.*=' cube.go")

	// Check Base = 3 (ternary)
	if c.Base != 3 {
		fail(&r,
			fmt.Sprintf("Base is %d, expected 3 (ternary)", c.Base),
			"Fix NewCube to set Base = 3",
		)
		r.Context["actual_base"] = fmt.Sprintf("%d", c.Base)
		return r
	}

	// Check DataModulo = 243 (3⁵)
	if c.DataModulo != 243 {
		fail(&r,
			fmt.Sprintf("DataModulo is %d, expected 243 (3⁵)", c.DataModulo),
			"Fix NewCube to set DataModulo = 243",
		)
		r.Context["actual_data_modulo"] = fmt.Sprintf("%d", c.DataModulo)
		return r
	}

	// Check ByteModulo = 256 (2⁸)
	if c.ByteModulo != 256 {
		fail(&r,
			fmt.Sprintf("ByteModulo is %d, expected 256 (2⁸)", c.ByteModulo),
			"Fix NewCube to set ByteModulo = 256",
		)
		r.Context["actual_byte_modulo"] = fmt.Sprintf("%d", c.ByteModulo)
		return r
	}

	r.Context["base"] = fmt.Sprintf("%d (ternary)", c.Base)
	r.Context["data_modulo"] = fmt.Sprintf("%d (3⁵)", c.DataModulo)
	r.Context["byte_modulo"] = fmt.Sprintf("%d (2⁸)", c.ByteModulo)
	r.Context["positions"] = "27 (3³)"

	pass(&r, "Cube constants verified: Base=3, DataModulo=243, ByteModulo=256")
	return r
}

// ────────────────────────────────────────────────────────────────
// Shape Translation Assurance (HALT_06)
// ────────────────────────────────────────────────────────────────
// 27 = 3³ positions in balanced ternary coordinates
// HALT_06 = MECHANISM_SHAPE (Shape ↔ Number translation)

// AssureShapeTranslation verifies Shape ↔ Number translation.
// Traces to: HALT_06 (MECHANISM_SHAPE) + HALT_03 (PSI = 27)
//
// What is checked:
//   - All 27 positions map to valid shapes
//   - ShapeToNumber and NumberToShape are inverses
//   - All coordinates are balanced ternary (-1, 0, +1)
func AssureShapeTranslation(c *Cube) schema.AssuranceResult {
	r := newResult("AssureShapeTranslation", "HALT_06",
		"Verify Shape ↔ Number: 27 positions with balanced ternary coordinates")

	if c == nil {
		fail(&r, "Cube is nil", "Create cube with NewCube first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "ShapeToNumber", "cube.go - shape to linear position")
	addLookAt(&r, "function", "NumberToShape", "cube.go - linear to shape")
	addLookAt(&r, "grep", "coords_to_position", "cube.schema.toml - formula")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'ShapeToNumber\\|NumberToShape' cube.go")
	addDebug(&r, "# Formula: position = (x+1)*9 + (y+1)*3 + (z+1)")

	// Verify all 27 positions round-trip correctly
	validTranslation := true
	for pos := 0; pos < 27; pos++ {
		shape := NumberToShape(pos)

		// Check balanced ternary range
		if shape.X < -1 || shape.X > 1 || shape.Y < -1 || shape.Y > 1 || shape.Z < -1 || shape.Z > 1 {
			validTranslation = false
			r.Context[fmt.Sprintf("invalid.pos_%d", pos)] = fmt.Sprintf("coords (%d,%d,%d) out of range",
				shape.X, shape.Y, shape.Z)
		}

		// Check round-trip
		roundTrip := ShapeToNumber(shape)
		if roundTrip != pos {
			validTranslation = false
			r.Context[fmt.Sprintf("roundtrip.pos_%d", pos)] = fmt.Sprintf("expected %d, got %d", pos, roundTrip)
		}
	}

	if !validTranslation {
		fail(&r,
			"Shape translation has invalid coordinates or round-trip failures",
			"Check ShapeToNumber and NumberToShape formulas in cube.go",
		)
		return r
	}

	// Verify origin is at position 13 (center of 0-26)
	origin := NewShape(0, 0, 0)
	originPos := ShapeToNumber(origin)
	if originPos != 13 {
		fail(&r,
			fmt.Sprintf("Origin (0,0,0) is at position %d, expected 13", originPos),
			"Check formula: (0+1)*9 + (0+1)*3 + (0+1) = 13",
		)
		return r
	}

	r.Context["positions"] = "27"
	r.Context["dimensions"] = "3"
	r.Context["coord_range"] = "[-1, 0, +1]"
	r.Context["origin_position"] = "13"
	r.Context["formula"] = "(x+1)*9 + (y+1)*3 + (z+1)"

	pass(&r, "Shape translation verified: 27 positions, balanced ternary, round-trip valid")
	return r
}

// ────────────────────────────────────────────────────────────────
// Cycle Formula Assurance (HALT_09)
// ────────────────────────────────────────────────────────────────
// The wheel in wheel — SEASONS, not cycles
// HALT_09 = MECHANISM_WHEEL
// Formula: (value * 3 + perspective) % 243

// AssureCycleFormula verifies the cycle computation formula.
// Traces to: HALT_09 (MECHANISM_WHEEL), statemachine.toml [formulas.cycle]
//
// What is checked:
//   - Formula: (value * 3 + perspective) % 243
//   - Perspective is added AFTER scaling
//   - All three perspectives work correctly
func AssureCycleFormula(c *Cube) schema.AssuranceResult {
	r := newResult("AssureCycleFormula", "HALT_09",
		"Verify cycle formula: (value * 3 + perspective) % 243")

	if c == nil {
		fail(&r, "Cube is nil", "Create cube with NewCube first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "Compute", "cube.go - the cycle computation")
	addLookAt(&r, "grep", "formulas.cycle", "statemachine.toml - authoritative formula")
	addLookAt(&r, "grep", "perspective", "cube.go - perspective application")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'func.*Compute' cube.go")
	addDebug(&r, "grep -A5 'formulas.cycle' statemachine.toml")

	// Test cases: known inputs and expected outputs
	// Formula: (value * 3 + perspective) % 243
	testCases := []struct {
		value       int
		perspective Perspective
		expected    int
	}{
		// Basic cases
		{0, PerspectivePresent, 0},   // (0 * 3 + 0) % 243 = 0
		{1, PerspectivePresent, 3},   // (1 * 3 + 0) % 243 = 3
		{10, PerspectivePresent, 30}, // (10 * 3 + 0) % 243 = 30

		// With perspective offset
		{0, PerspectivePast, 242},   // (0 * 3 + -1) % 243 = -1 → 242
		{0, PerspectiveFuture, 1},   // (0 * 3 + 1) % 243 = 1
		{10, PerspectivePast, 29},   // (10 * 3 + -1) % 243 = 29
		{10, PerspectiveFuture, 31}, // (10 * 3 + 1) % 243 = 31

		// Wraparound cases
		{81, PerspectivePresent, 0}, // (81 * 3 + 0) % 243 = 243 % 243 = 0
		{80, PerspectiveFuture, 241}, // (80 * 3 + 1) % 243 = 241
	}

	for i, tc := range testCases {
		result := c.Compute(tc.value, tc.perspective)
		if result != tc.expected {
			fail(&r,
				fmt.Sprintf("Test %d: Compute(%d, %d) = %d, expected %d",
					i, tc.value, int(tc.perspective), result, tc.expected),
				"Fix Compute function formula in cube.go",
			)
			r.Context["failed_test"] = fmt.Sprintf("%d", i)
			r.Context["formula_used"] = fmt.Sprintf("(%d * 3 + %d) %% 243", tc.value, int(tc.perspective))
			return r
		}
	}

	r.Context["formula"] = "(value * 3 + perspective) % 243"
	r.Context["data_range"] = "0-242"
	r.Context["perspectives"] = "PAST=-1, PRESENT=0, FUTURE=+1"
	r.Context["tests_passed"] = fmt.Sprintf("%d", len(testCases))

	pass(&r, fmt.Sprintf("Cycle formula verified: %d test cases passed", len(testCases)))
	return r
}

// ────────────────────────────────────────────────────────────────
// Trite Conversion Assurance (HALT_05)
// ────────────────────────────────────────────────────────────────
// FILING/RANKING/INDEXING layers:
//   Position (0-26) = ADDRESS
//   Trite (0-242) = DATA VALUE
// HALT_05 = MECHANISM_NUMBER

// AssureTriteConversion verifies Trite ↔ Position mapping.
// Traces to: HALT_05 (MECHANISM_NUMBER), encryption.schema.toml
//
// What is checked:
//   - TriteToPosition maps 243 trites to 27 positions
//   - Each position covers 9 trite values
//   - Trite ranges are correct (0-127 ASCII, 128-140 metadata, 141-242 extended, 243-255 WEB)
func AssureTriteConversion(c *Cube) schema.AssuranceResult {
	r := newResult("AssureTriteConversion", "HALT_05",
		"Verify Trite ↔ Position: 243 trite values map to 27 positions")

	if c == nil {
		fail(&r, "Cube is nil", "Create cube with NewCube first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "TriteToPosition", "cube.go - trite to position")
	addLookAt(&r, "function", "TriteToShape", "cube.go - trite to shape")
	addLookAt(&r, "grep", "trite_encoding", "encryption.schema.toml - trite ranges")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'TriteToPosition\\|TriteToShape' cube.go")
	addDebug(&r, "grep -A10 'trite_encoding' encryption.schema.toml")

	// Verify each position covers 9 trite values
	positionCounts := make(map[int]int)
	for trite := 0; trite < 243; trite++ {
		pos := TriteToPosition(trite)
		if pos < 0 || pos > 26 {
			fail(&r,
				fmt.Sprintf("TriteToPosition(%d) = %d, expected 0-26", trite, pos),
				"Fix TriteToPosition to clamp output to 0-26",
			)
			return r
		}
		positionCounts[pos]++
	}

	// Each position should have exactly 9 trite values
	for pos := 0; pos < 27; pos++ {
		count := positionCounts[pos]
		if count != 9 {
			fail(&r,
				fmt.Sprintf("Position %d has %d trites, expected 9", pos, count),
				"Check TriteToPosition formula: trite %% 27",
			)
			r.Context[fmt.Sprintf("pos_%d_count", pos)] = fmt.Sprintf("%d", count)
			return r
		}
	}

	// Verify specific trite range mappings
	// ASCII range (0-127) should map to positions 0-26 cycling
	asciiPos := TriteToPosition(65) // 'A' = 65, expected: 65 % 27 = 11
	if asciiPos != 11 {
		fail(&r,
			fmt.Sprintf("ASCII 'A' (65) maps to position %d, expected 11", asciiPos),
			"Check TriteToPosition formula",
		)
		return r
	}

	// WEB variant range (243-255) should map to positions 0-12
	// TriteToShape handles this special case
	webShape := TriteToShape(243)
	webPos := ShapeToNumber(webShape)
	if webPos != 0 {
		fail(&r,
			fmt.Sprintf("WEB variant 243 maps to position %d, expected 0", webPos),
			"Check TriteToShape WEB variant handling",
		)
		return r
	}

	r.Context["trite_range"] = "0-242 (data)"
	r.Context["web_range"] = "243-255 (BIBLE RAIL)"
	r.Context["positions"] = "27"
	r.Context["trites_per_position"] = "9"
	r.Context["formula"] = "trite % 27"

	pass(&r, "Trite conversion verified: 243 trites → 27 positions, 9 per position")
	return r
}

// ────────────────────────────────────────────────────────────────
// Seven Days Assurance (HALT_10)
// ────────────────────────────────────────────────────────────────
// 7 Days of Creation = 7 States (Genesis 1)
// HALT_10 = STATE_HEBREW

// AssureSevenDays verifies CreationDay mapping (7 Days of Creation).
// Traces to: HALT_10 (STATE_HEBREW) + Genesis 1
//
// What is checked:
//   - All 7 days map to correct Z-depths
//   - CreationDay returns correct day for each shape
//   - Transit positions return 0
func AssureSevenDays(c *Cube) schema.AssuranceResult {
	r := newResult("AssureSevenDays", "HALT_10",
		"Verify 7 Days of Creation = 7 States (Genesis 1)")

	if c == nil {
		fail(&r, "Cube is nil", "Create cube with NewCube first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "CreationDay", "cube.go - day mapping method")
	addLookAt(&r, "grep", "perspectives", "cube.schema.toml - day positions")
	addLookAt(&r, "file", "bereshit/word/glossary/biblical/", "Hebrew word definitions")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'CreationDay' cube.go")
	addDebug(&r, "grep -A5 'perspectives' cube.schema.toml")

	// Expected mappings from cube.go CreationDay method
	// Based on Z-depth and X position
	expectedDays := []struct {
		shape    Shape
		day      int
		dayName  string
		hebrew   string
	}{
		{NewShape(-1, -1, -1), 1, "Light", "shavar"},        // Z=-1.0
		{NewShape(-1, 0, -1), 2, "Waters", "chaser"},        // Z=-0.5, X=-1
		{NewShape(0, -1, 0), 3, "Land", "ratsah"},           // Z=-0.5, X=0
		{NewShape(0, 0, 0), 4, "Seasons", "yashar"},         // Z=0.0, Origin
		{NewShape(0, 1, 0), 5, "Creatures", "tamim"},        // Z=+0.5, X=0
		{NewShape(1, 0, 1), 6, "Humans", "shalem"},          // Z=+0.5, X=1
		{NewShape(1, 1, 1), 7, "Sabbath", "tov"},            // Z=+1.0
	}

	daysFound := 0
	for _, expected := range expectedDays {
		day := expected.shape.CreationDay()
		if day != expected.day {
			fail(&r,
				fmt.Sprintf("Shape (%d,%d,%d) returned day %d, expected %d (%s)",
					expected.shape.X, expected.shape.Y, expected.shape.Z,
					day, expected.day, expected.dayName),
				"Fix CreationDay switch statement in cube.go",
			)
			r.Context["failed_shape"] = fmt.Sprintf("(%d,%d,%d)",
				expected.shape.X, expected.shape.Y, expected.shape.Z)
			return r
		}
		r.Context[fmt.Sprintf("day_%d", expected.day)] = fmt.Sprintf("%s (%s)",
			expected.dayName, expected.hebrew)
		daysFound++
	}

	// Verify transit positions return 0
	transitShape := NewShape(-1, 1, 0) // PAST-FUTURE diagonal
	if transitShape.CreationDay() != 0 {
		fail(&r,
			fmt.Sprintf("Transit shape (-1,1,0) returned day %d, expected 0",
				transitShape.CreationDay()),
			"Fix CreationDay to return 0 for transit positions",
		)
		return r
	}

	// Verify origin is Day 4 (SEASONS CREATED)
	origin := NewShape(0, 0, 0)
	if origin.CreationDay() != 4 {
		fail(&r,
			fmt.Sprintf("Origin (0,0,0) is day %d, expected 4 (SEASONS)", origin.CreationDay()),
			"Origin must be Day 4 — Genesis 1:14-19",
		)
		return r
	}

	r.Context["days_verified"] = fmt.Sprintf("%d", daysFound)
	r.Context["origin_day"] = "4 (SEASONS)"
	r.Context["sabbath_day"] = "7 (tov)"
	r.Context["biblical_ref"] = "Genesis 1-2:3"

	pass(&r, "7 Days verified: Genesis 1 mapping with Hebrew roots")
	return r
}

// ────────────────────────────────────────────────────────────────
// Full Suite
// ────────────────────────────────────────────────────────────────

// RunAllAssurances runs the complete cube diagnostic suite.
func RunAllAssurances(c *Cube) schema.AssuranceSuite {
	start := time.Now()
	suite := schema.AssuranceSuite{
		Results: make([]schema.AssuranceResult, 0, 5),
		RunAt:   start,
	}

	// Run all cube assurances
	suite.Results = append(suite.Results, AssureCubeConstants(c))
	suite.Results = append(suite.Results, AssureShapeTranslation(c))
	suite.Results = append(suite.Results, AssureCycleFormula(c))
	suite.Results = append(suite.Results, AssureTriteConversion(c))
	suite.Results = append(suite.Results, AssureSevenDays(c))

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
// Cube Assurance Architecture:
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ ASSURANCE FUNCTIONS                                                     │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │  AssureCubeConstants     Verify Base=3, DataModulo=243 (HALT_03/05)    │
//   │  AssureShapeTranslation  Verify Shape ↔ Number (HALT_06)               │
//   │  AssureCycleFormula      Verify (value*3+perspective)%243 (HALT_09)    │
//   │  AssureTriteConversion   Verify Trite ↔ Position (HALT_05)             │
//   │  AssureSevenDays         Verify 7 Days mapping (HALT_10)               │
//   └─────────────────────────────────────────────────────────────────────────┘
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ HALT TRACE                                                              │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │                                                                         │
//   │  compute/assurance.go → HALT_10 (7 Days) → HALT_09 (Cycle Formula)     │
//   │                       → HALT_06 (Shape) → HALT_05 (Number/Trite)       │
//   │                       → HALT_03 (PSI=27) → HALT_01 (Genesis 1:1)       │
//   │                                                                         │
//   └─────────────────────────────────────────────────────────────────────────┘
//
// "Examine yourselves, whether ye be in the faith; prove your own selves."
// — 2 Corinthians 13:5
//
// ============================================================================
// END CLOSING
// ============================================================================
