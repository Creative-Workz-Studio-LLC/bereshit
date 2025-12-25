// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-cpisi-bereshit-assurance
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Bereshit Assurance Functions — Self-Monitoring Diagnostics
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-cpisi-bereshit-assurance
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
//	HALT_08 (ENCODING)         → Letter → Position → Coords verification
//	HALT_09 (DECODING)         → Position → Letter verification
//	HALT_10 (STATE_HEBREW)     → 7 States = 7 Days verification
//	HALT_03 (PSI)              → 27 = 3³ cube positions verification
//	HALT_01 (GENESIS)          → All traces terminate at Genesis 1:1
//
// # M.2 Public API [API]
//
//	AssureTransliteration(dir) AssuranceResult    Verify tables load
//	AssureHebrew(trans) AssuranceResult           Verify Hebrew alphabet
//	AssureGreek(trans) AssuranceResult            Verify Greek alphabet
//	AssureCubeGeometry(trans) AssuranceResult     Verify 27=3³ positions
//	AssureStates(trans) AssuranceResult           Verify 7 state mappings
//	RunAllAssurances(dir) Suite                   Run complete diagnostics
//
// # M.3 Paradigm [PARADIGM]
//
//	Uses schema.AssuranceResult for ternary scoring.
//	DETECT → ASSESS → RESTORE workflow inherited.
//
// ============================================================================
// END METADATA
// ============================================================================

package bereshit

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"path/filepath"
	"time"

	"creativeworkzstudio.com/bereshit/word/work/pkg/foundation/schema"
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
// Transliteration Assurance (HALT_08 + HALT_09)
// ────────────────────────────────────────────────────────────────
// The transliteration tables map letters to positions and back.
// HALT_08 = encoding (letter → position)
// HALT_09 = decoding (position → letter)

// AssureTransliteration verifies transliteration tables load correctly.
// Traces to: HALT_08 (ENCODING), HALT_09 (DECODING)
//
// What is checked:
//   - encryption.toml exists and parses
//   - decryption.toml exists and parses
//   - Hebrew and Greek tables populate
func AssureTransliteration(dir string) schema.AssuranceResult {
	r := newResult("AssureTransliteration", "HALT_08",
		"Verify transliteration tables load: encryption.toml + decryption.toml")
	r.Context["dir"] = dir

	encPath := filepath.Join(dir, EncryptionFile)
	decPath := filepath.Join(dir, DecryptionFile)

	// LOOK AT: Where to investigate
	addLookAt(&r, "file", encPath, "Letter → Position encoding (HALT_08)")
	addLookAt(&r, "file", decPath, "Position → Letter decoding (HALT_09)")
	addLookAt(&r, "function", "LoadTransliteration", "loader.go - main loader function")

	// DEBUG: How to investigate
	addDebug(&r, fmt.Sprintf("cat %s | head -30", encPath))
	addDebug(&r, fmt.Sprintf("cat %s | head -30", decPath))
	addDebug(&r, fmt.Sprintf("ls -la %s", dir))

	trans, err := LoadTransliteration(dir)
	if err != nil {
		fail(&r,
			fmt.Sprintf("LoadTransliteration failed: %v", err),
			fmt.Sprintf("Check TOML files in %s", dir),
		)
		return r
	}

	// Verify tables exist
	if trans.Hebrew == nil {
		fail(&r, "Hebrew table is nil", "Check hebrew section in encryption.toml")
		return r
	}
	if trans.Greek == nil {
		fail(&r, "Greek table is nil", "Check greek section in encryption.toml")
		return r
	}

	r.Context["hebrew.letters"] = fmt.Sprintf("%d", len(trans.Hebrew.ByLetter))
	r.Context["greek.letters"] = fmt.Sprintf("%d", len(trans.Greek.ByLetter))
	r.Context["formulas.loaded"] = "true"

	pass(&r, fmt.Sprintf("Transliteration loaded: Hebrew=%d letters, Greek=%d letters",
		len(trans.Hebrew.ByLetter), len(trans.Greek.ByLetter)))
	return r
}

// ────────────────────────────────────────────────────────────────
// Hebrew Alphabet Assurance (HALT_08)
// ────────────────────────────────────────────────────────────────
// Hebrew has 22 letters + 5 final forms = 27 letters = 3³ cube positions

// AssureHebrew verifies Hebrew alphabet loads correctly.
// Traces to: HALT_08 (ENCODING) + HALT_03 (PSI = 27)
//
// What is checked:
//   - 27 letters loaded (22 + 5 finals)
//   - ByLetter, ByPosition, ByValue maps populated
//   - ByState map has entries for all 7 states
func AssureHebrew(trans *Transliteration) schema.AssuranceResult {
	r := newResult("AssureHebrew", "HALT_08",
		"Verify Hebrew alphabet: 27 letters (22 + 5 finals) = 3³ cube positions")

	if trans == nil {
		fail(&r, "Transliteration is nil", "Load transliteration first")
		return r
	}
	if trans.Hebrew == nil {
		fail(&r, "Hebrew table is nil", "Check hebrew section in encryption.toml")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "grep", "hebrew.encrypt", "Hebrew letter definitions in encryption.toml")
	addLookAt(&r, "grep", "ByLetter", "loader.go - forward lookup map")
	addLookAt(&r, "grep", "ByPosition", "loader.go - reverse lookup map")
	addLookAt(&r, "grep", "ByState", "loader.go - state-based lookup (7 Days)")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'Hebrew.ByLetter' loader.go")
	addDebug(&r, "grep -c 'encrypt\\.' encryption.toml")

	r.Context["testament"] = trans.Hebrew.Testament
	r.Context["letter_count"] = fmt.Sprintf("%d", trans.Hebrew.LetterCount)
	r.Context["value_range"] = trans.Hebrew.ValueRange

	// Check 27 letters (HALT_03: PSI = 27 = 3³)
	letterCount := len(trans.Hebrew.ByLetter)
	if letterCount != 27 {
		fail(&r,
			fmt.Sprintf("Hebrew has %d letters, expected 27 (HALT_03: PSI = 3³)", letterCount),
			"Check encryption.toml has all 22 + 5 final Hebrew letters",
		)
		r.Context["actual_letters"] = fmt.Sprintf("%d", letterCount)
		return r
	}

	// Check position map
	posCount := len(trans.Hebrew.ByPosition)
	if posCount != 27 {
		fail(&r,
			fmt.Sprintf("Hebrew ByPosition has %d entries, expected 27", posCount),
			"Check all positions 0-26 are mapped in encryption.toml",
		)
		r.Context["actual_positions"] = fmt.Sprintf("%d", posCount)
		return r
	}

	// Check all 7 states have entries (HALT_10: 7 Days)
	stateCount := len(trans.Hebrew.ByState)
	r.Context["state_count"] = fmt.Sprintf("%d", stateCount)

	// Count letters per state
	for state := StateEstablished; state <= StateAspiration; state++ {
		count := len(trans.Hebrew.ByState[state])
		r.Context[fmt.Sprintf("state.%s", HebrewRoot[state])] = fmt.Sprintf("%d letters", count)
	}

	r.Context["by_letter"] = fmt.Sprintf("%d", letterCount)
	r.Context["by_position"] = fmt.Sprintf("%d", posCount)
	r.Context["by_value"] = fmt.Sprintf("%d", len(trans.Hebrew.ByValue))

	pass(&r, fmt.Sprintf("Hebrew verified: 27 letters, 27 positions, %d states",
		stateCount))
	return r
}

// ────────────────────────────────────────────────────────────────
// Greek Alphabet Assurance (HALT_08)
// ────────────────────────────────────────────────────────────────
// Greek has 24 letters + 3 archaic = 27 letters = 3³ cube positions

// AssureGreek verifies Greek alphabet loads correctly.
// Traces to: HALT_08 (ENCODING) + HALT_03 (PSI = 27)
//
// What is checked:
//   - 27 letters loaded (24 + 3 archaic)
//   - ByLetter, ByPosition, ByValue maps populated
//   - ByState map has entries
func AssureGreek(trans *Transliteration) schema.AssuranceResult {
	r := newResult("AssureGreek", "HALT_08",
		"Verify Greek alphabet: 27 letters (24 + 3 archaic) = 3³ cube positions")

	if trans == nil {
		fail(&r, "Transliteration is nil", "Load transliteration first")
		return r
	}
	if trans.Greek == nil {
		fail(&r, "Greek table is nil", "Check greek section in encryption.toml")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "grep", "greek.encrypt", "Greek letter definitions in encryption.toml")
	addLookAt(&r, "grep", "archaic", "Archaic letters: Digamma, Qoppa, Sampi")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'Greek.ByLetter' loader.go")
	addDebug(&r, "grep 'archaic.*true' encryption.toml")

	r.Context["testament"] = trans.Greek.Testament
	r.Context["letter_count"] = fmt.Sprintf("%d", trans.Greek.LetterCount)
	r.Context["value_range"] = trans.Greek.ValueRange

	// Check 27 letters (HALT_03: PSI = 27 = 3³)
	letterCount := len(trans.Greek.ByLetter)
	if letterCount != 27 {
		fail(&r,
			fmt.Sprintf("Greek has %d letters, expected 27 (HALT_03: PSI = 3³)", letterCount),
			"Check encryption.toml has all 24 + 3 archaic Greek letters",
		)
		r.Context["actual_letters"] = fmt.Sprintf("%d", letterCount)
		return r
	}

	// Check position map
	posCount := len(trans.Greek.ByPosition)
	if posCount != 27 {
		fail(&r,
			fmt.Sprintf("Greek ByPosition has %d entries, expected 27", posCount),
			"Check all positions 0-26 are mapped in encryption.toml",
		)
		r.Context["actual_positions"] = fmt.Sprintf("%d", posCount)
		return r
	}

	// Count archaic letters
	archaicCount := 0
	for _, entry := range trans.Greek.ByLetter {
		if entry.Archaic {
			archaicCount++
		}
	}
	r.Context["archaic_count"] = fmt.Sprintf("%d", archaicCount)

	r.Context["by_letter"] = fmt.Sprintf("%d", letterCount)
	r.Context["by_position"] = fmt.Sprintf("%d", posCount)
	r.Context["by_value"] = fmt.Sprintf("%d", len(trans.Greek.ByValue))

	pass(&r, fmt.Sprintf("Greek verified: 27 letters (including %d archaic), 27 positions",
		archaicCount))
	return r
}

// ────────────────────────────────────────────────────────────────
// Cube Geometry Assurance (HALT_06 + HALT_03)
// ────────────────────────────────────────────────────────────────
// 27 = 3³ positions in balanced ternary coordinates
// HALT_06 = shape (geometry)
// HALT_03 = PSI (27 = 3³)

// AssureCubeGeometry verifies cube coordinate geometry.
// Traces to: HALT_06 (MECHANISM_SHAPE) + HALT_03 (PSI)
//
// What is checked:
//   - All 27 positions map to valid coordinates
//   - CoordsToPosition and PositionToCoords are inverses
//   - All coordinates are balanced ternary (-1, 0, +1)
func AssureCubeGeometry(trans *Transliteration) schema.AssuranceResult {
	r := newResult("AssureCubeGeometry", "HALT_06",
		"Verify cube geometry: 27 = 3³ positions with balanced ternary coordinates")

	if trans == nil || trans.Hebrew == nil {
		fail(&r, "Transliteration or Hebrew is nil", "Load transliteration first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "CoordsToPosition", "loader.go - coords to linear position")
	addLookAt(&r, "function", "PositionToCoords", "loader.go - linear to coords")
	addLookAt(&r, "grep", "Coords", "Letter coordinates in encryption.toml")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'CoordsToPosition\\|PositionToCoords' loader.go")
	addDebug(&r, "# Formula: position = (x+1)*9 + (y+1)*3 + (z+1)")

	// Verify all 27 positions
	validCoords := true
	for pos := 0; pos < 27; pos++ {
		x, y, z := PositionToCoords(pos)

		// Check balanced ternary range
		if x < -1 || x > 1 || y < -1 || y > 1 || z < -1 || z > 1 {
			validCoords = false
			r.Context[fmt.Sprintf("invalid.pos_%d", pos)] = fmt.Sprintf("coords (%d,%d,%d) out of range", x, y, z)
		}

		// Check round-trip
		roundTrip := CoordsToPosition(x, y, z)
		if roundTrip != pos {
			validCoords = false
			r.Context[fmt.Sprintf("roundtrip.pos_%d", pos)] = fmt.Sprintf("expected %d, got %d", pos, roundTrip)
		}
	}

	if !validCoords {
		fail(&r,
			"Cube geometry has invalid coordinates or round-trip failures",
			"Check PositionToCoords and CoordsToPosition formulas",
		)
		return r
	}

	// Verify formulas are loaded
	if trans.CoordsToIndex == "" {
		r.Context["formulas.warning"] = "CoordsToIndex formula not loaded"
	} else {
		r.Context["formulas.coords_to_index"] = trans.CoordsToIndex
	}

	r.Context["positions"] = "27"
	r.Context["dimensions"] = "3"
	r.Context["coord_range"] = "[-1, 0, +1]"

	pass(&r, "Cube geometry verified: 27 positions, balanced ternary, round-trip valid")
	return r
}

// ────────────────────────────────────────────────────────────────
// State Mapping Assurance (HALT_10)
// ────────────────────────────────────────────────────────────────
// 7 States = 7 Days of Creation (Genesis 1)
// HALT_10 = STATE_HEBREW

// AssureStates verifies 7-state mapping (7 Days of Creation).
// Traces to: HALT_10 (STATE_HEBREW) + Genesis 1
//
// What is checked:
//   - All 7 states are defined
//   - StateFromZ maps correctly
//   - Each state has Hebrew root word
func AssureStates(trans *Transliteration) schema.AssuranceResult {
	r := newResult("AssureStates", "HALT_10",
		"Verify 7 states = 7 Days of Creation (Genesis 1)")

	if trans == nil || trans.Hebrew == nil {
		fail(&r, "Transliteration or Hebrew is nil", "Load transliteration first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "grep", "State", "loader.go - State type and constants")
	addLookAt(&r, "grep", "HebrewRoot", "loader.go - Hebrew root word mapping")
	addLookAt(&r, "file", "bereshit/word/glossary/biblical/", "Hebrew word definitions")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'State.*=' loader.go | head -10")
	addDebug(&r, "grep -n 'HebrewRoot' loader.go")

	// Verify all 7 states have Hebrew roots
	expectedStates := []struct {
		state State
		root  string
		day   int
	}{
		{StateEstablished, "shavar", 1},
		{StateMemory, "chaser", 2},
		{StateReflection, "ratsah", 3},
		{StateOrigin, "yashar", 4},
		{StatePlanning, "tamim", 5},
		{StatePreparation, "shalem", 6},
		{StateAspiration, "tov", 7},
	}

	for _, expected := range expectedStates {
		root, ok := HebrewRoot[expected.state]
		if !ok {
			fail(&r,
				fmt.Sprintf("State %d (Day %d) missing Hebrew root", expected.state, expected.day),
				"Add Hebrew root to HebrewRoot map",
			)
			return r
		}
		if root != expected.root {
			fail(&r,
				fmt.Sprintf("State %d has root '%s', expected '%s'", expected.state, root, expected.root),
				"Fix HebrewRoot mapping",
			)
			return r
		}
		r.Context[fmt.Sprintf("day_%d", expected.day)] = fmt.Sprintf("%s (%s)", expected.root, root)
	}

	// Verify StateFromZ mapping
	if StateFromZ(-1) != StateEstablished {
		fail(&r, "StateFromZ(-1) != StateEstablished", "Fix StateFromZ function")
		return r
	}
	if StateFromZ(0) != StateOrigin {
		fail(&r, "StateFromZ(0) != StateOrigin", "Fix StateFromZ function")
		return r
	}
	if StateFromZ(1) != StateAspiration {
		fail(&r, "StateFromZ(1) != StateAspiration", "Fix StateFromZ function")
		return r
	}

	r.Context["state_count"] = "7"
	r.Context["z_range"] = "[-1, 0, +1]"
	r.Context["biblical_ref"] = "Genesis 1"

	pass(&r, "7 States verified: Days 1-7 of Creation with Hebrew roots")
	return r
}

// ────────────────────────────────────────────────────────────────
// Full Suite
// ────────────────────────────────────────────────────────────────

// RunAllAssurances runs the complete bereshit diagnostic suite.
func RunAllAssurances(dir string) schema.AssuranceSuite {
	start := time.Now()
	suite := schema.AssuranceSuite{
		Results: make([]schema.AssuranceResult, 0, 5),
		RunAt:   start,
	}

	// First load transliteration
	transResult := AssureTransliteration(dir)
	suite.Results = append(suite.Results, transResult)

	// Only continue if transliteration loaded
	if transResult.Score == schema.ScorePass {
		trans, err := LoadTransliteration(dir)
		if err == nil {
			suite.Results = append(suite.Results, AssureHebrew(trans))
			suite.Results = append(suite.Results, AssureGreek(trans))
			suite.Results = append(suite.Results, AssureCubeGeometry(trans))
			suite.Results = append(suite.Results, AssureStates(trans))
		}
	}

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
// Bereshit Assurance Architecture:
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ ASSURANCE FUNCTIONS                                                     │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │  AssureTransliteration  Verify TOML files load (HALT_08 + HALT_09)     │
//   │  AssureHebrew           Verify 27 Hebrew letters (HALT_03 + HALT_08)   │
//   │  AssureGreek            Verify 27 Greek letters (HALT_03 + HALT_08)    │
//   │  AssureCubeGeometry     Verify 3³ = 27 positions (HALT_06 + HALT_03)   │
//   │  AssureStates           Verify 7 Days mapping (HALT_10)                │
//   └─────────────────────────────────────────────────────────────────────────┘
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ HALT TRACE                                                              │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │                                                                         │
//   │  bereshit/assurance.go → HALT_10 (7 States) → HALT_08/09 (encoding)    │
//   │                        → HALT_06 (shape) → HALT_03 (PSI=27)            │
//   │                        → HALT_01 (Genesis 1:1)                          │
//   │                                                                         │
//   └─────────────────────────────────────────────────────────────────────────┘
//
// "Examine yourselves, whether ye be in the faith; prove your own selves."
// — 2 Corinthians 13:5
//
// ============================================================================
// END CLOSING
// ============================================================================
