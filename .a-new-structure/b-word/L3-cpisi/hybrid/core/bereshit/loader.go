//omni:code --go -library
//omni:key B-L3-bereshit-loader
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Bereshit Data Loader - Loads transliteration tables from Bereshit TOML
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-cpisi-bereshit-loader
//
//	Scripture: "I am Alpha and Omega, the beginning and the ending"
//	           — Revelation 1:8
//	Principle: Letters map to positions; positions map to coordinates
//	Anchor:    "In the beginning was the Word" — John 1:1
//
//	Layer:     L3 (COMMUNICATION — encoding/decoding operations)
//	Type:      Foundation (data loader for transliteration)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-20
//	Version:         a-01.00
//
// # M.4 HALT Trace [HALTS]
//
//	HALT_10 (STATE_HEBREW)      → 7 States = 7 Days of Creation (ByState maps)
//	HALT_05 (MECHANISM_NUMBER)  → Value transformations (gematria ↔ trite)
//	HALT_06 (MECHANISM_SHAPE)   → Coordinate geometry (x, y, z positions)
//	HALT_08 (ENCODING)          → Letter → Position → Coords → Value
//	HALT_09 (DECODING)          → Position → Letter (reverse lookup)
//	HALT_03 (EQUATION_PSI)      → 27 = 3³ cube positions
//	HALT_01 (ANCHOR_GENESIS)    → All traces terminate at Genesis 1:1
//
// # M.2 Public API [API]
//
//	LoadTransliteration(dir) (*Transliteration, error)  Load encryption/decryption
//	MustLoadTransliteration(dir) *Transliteration       Load or panic
//	DefaultBereshitDir() string                         Return bereshit/word/core/bible
//	StateFromZ(z) State                                 Z coordinate → State
//	PositionToState(pos) State                          Position → State
//	HebrewByState(state) []*LetterEntry                 State-based Hebrew lookup
//	GreekByState(state) []*LetterEntry                  State-based Greek lookup
//
// # M.3 Data Types [TYPES]
//
//	Transliteration - Container for Hebrew and Greek tables
//	LetterEntry     - Single letter with position, coords, value
//	AlphabetTable   - Full alphabet (27 letters)
//
// ============================================================================
// END METADATA
// ============================================================================

package bereshit

// ============================================================================
// SETUP
// ============================================================================

import (
	"errors"
	"path/filepath"
	"sync"

	"github.com/BurntSushi/toml"

	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util"
)

// ────────────────────────────────────────────────────────────────
// Types - Seasonal Model (HALT_10: 7 States = 7 Days of Creation)
// ────────────────────────────────────────────────────────────────
// "To every thing there is a season" — Ecclesiastes 3:1

// State represents one of the 7 Hebrew states (7 Days of Creation).
// Traces to: HALT_10 (STATE_HEBREW)
type State int

const (
	// Day 1: ESTABLISHED (shavar - broken) Z=-1
	StateEstablished State = iota
	// Day 2: MEMORY (chaser - lacking) Z=-0.5, k=-1
	StateMemory
	// Day 3: REFLECTION (ratsah - wanting) Z=-0.5, k=+1
	StateReflection
	// Day 4: ORIGIN (yashar - even/straight) Z=0 — CENTER
	StateOrigin
	// Day 5: PLANNING (tamim - sound) Z=+0.5, k=-1
	StatePlanning
	// Day 6: PREPARATION (shalem - whole) Z=+0.5, k=+1
	StatePreparation
	// Day 7: ASPIRATION (tov - good/perfect) Z=+1 — GOAL (Sabbath)
	StateAspiration
)

// HebrewRoot maps states to their Hebrew root words.
var HebrewRoot = map[State]string{
	StateEstablished: "shavar",  // שָׁבַר - broken
	StateMemory:      "chaser",  // חָסֵר - lacking
	StateReflection:  "ratsah",  // רָצָה - wanting
	StateOrigin:      "yashar",  // יָשָׁר - straight/even
	StatePlanning:    "tamim",   // תָּמִים - sound/complete
	StatePreparation: "shalem",  // שָׁלֵם - whole/at peace
	StateAspiration:  "tov",     // טוֹב - good/perfect
}

// StateFromZ returns the State for a given Z coordinate.
// Z=-1 → Established, Z=0 → Origin, Z=+1 → Aspiration
// For boundary states (Z=-0.5, Z=+0.5), use StateFromZK with k_selector.
func StateFromZ(z int) State {
	switch z {
	case -1:
		return StateEstablished
	case 0:
		return StateOrigin
	case 1:
		return StateAspiration
	default:
		return StateOrigin // Default to center
	}
}

// ────────────────────────────────────────────────────────────────
// Types - Transliteration Data
// ────────────────────────────────────────────────────────────────

// Transliteration holds both encryption (letter→position) and
// decryption (position→letter) tables for Hebrew and Greek.
// Traces to: HALT_08 (encoding), HALT_09 (decoding)
type Transliteration struct {
	Hebrew *AlphabetTable
	Greek  *AlphabetTable

	// Formulas from the TOML (HALT_05: NUMBER operations)
	CoordsToIndex string
	IndexToX      string
	IndexToY      string
	IndexToZ      string
	ValueToTrite  string
}

// AlphabetTable holds 27 letters for one alphabet.
// 27 = 3³ = cube positions (HALT_03: PSI)
type AlphabetTable struct {
	Testament   string // "Old" (Hebrew) or "New" (Greek)
	LetterCount int    // 27 letters = 27 positions
	ValueRange  string // Gematria range

	// Forward lookup: letter → entry (HALT_08: encoding)
	ByLetter map[string]*LetterEntry

	// Reverse lookup: position → entry (HALT_09: decoding)
	ByPosition map[int]*LetterEntry

	// Reverse lookup: value → entry (HALT_05: number)
	ByValue map[int]*LetterEntry

	// State lookup: state → entries (HALT_10: 7 Days of Creation)
	ByState map[State][]*LetterEntry
}

// LetterEntry represents a single letter's transliteration data.
// Each letter maps to a position in the 3³ cube (HALT_06: shape).
type LetterEntry struct {
	Letter   string     // The letter itself (א, β, etc.)
	Name     string     // Letter name (Aleph, Beta, etc.)
	Position int        // Linear position (0-26)
	Value    int        // Gematria value (HALT_05: number)
	Coords   [3]int     // [x, y, z] balanced ternary (HALT_06: shape)
	Archaic  bool       // For Greek archaic letters
}

// State returns the Hebrew state for this letter based on Z coordinate.
// Traces to: HALT_10 (STATE_HEBREW) — 7 Days of Creation
func (e *LetterEntry) State() State {
	return StateFromZ(e.Coords[2])
}

// HebrewRootName returns the Hebrew root word for this letter's state.
func (e *LetterEntry) HebrewRootName() string {
	return HebrewRoot[e.State()]
}

// ────────────────────────────────────────────────────────────────
// Raw TOML Structures
// ────────────────────────────────────────────────────────────────

// These match the structure of encryption.toml and decryption.toml

type rawEncryptionFile struct {
	System    rawSystem               `toml:"system"`
	Functions rawFunctions            `toml:"functions"`
	Hebrew    rawAlphabetEncrypt      `toml:"hebrew"`
	Greek     rawAlphabetEncrypt      `toml:"greek"`
}

type rawDecryptionFile struct {
	System    rawSystem               `toml:"system"`
	Functions rawFunctions            `toml:"functions"`
	Hebrew    rawAlphabetDecrypt      `toml:"hebrew"`
	Greek     rawAlphabetDecrypt      `toml:"greek"`
}

type rawSystem struct {
	Name           string `toml:"name"`
	Direction      string `toml:"direction"`
	Description    string `toml:"description"`
	CubePositions  int    `toml:"cube_positions"`
	CubeDimensions int    `toml:"cube_dimensions"`
	GematriaMax    int    `toml:"gematria_max"`
	TriteMax       int    `toml:"trite_max"`
}

type rawFunctions struct {
	CoordsToIndex string `toml:"coords_to_index"`
	IndexToX      string `toml:"index_to_x"`
	IndexToY      string `toml:"index_to_y"`
	IndexToZ      string `toml:"index_to_z"`
	ValueToTrite  string `toml:"value_to_trite"`

	// Also in decryption
	CoordsToPosition string `toml:"coords_to_position"`
	TriteToValue     string `toml:"trite_to_value"`
}

type rawAlphabetEncrypt struct {
	Testament   string                      `toml:"testament"`
	LetterCount int                         `toml:"letter_count"`
	ValueRange  string                      `toml:"value_range"`
	Encrypt     map[string]rawLetterEncrypt `toml:"encrypt"`
}

type rawAlphabetDecrypt struct {
	Testament     string                      `toml:"testament"`
	PositionCount int                         `toml:"position_count"`
	ValueRange    string                      `toml:"value_range"`
	Decrypt       map[string]rawLetterDecrypt `toml:"decrypt"`
	ByValue       map[string]string           `toml:"by_value"`
}

type rawLetterEncrypt struct {
	Letter   string `toml:"letter"`
	Position int    `toml:"position"`
	Value    int    `toml:"value"`
	Coords   []int  `toml:"coords"`
	Archaic  bool   `toml:"archaic"`
}

type rawLetterDecrypt struct {
	Position int    `toml:"position"`
	Coords   []int  `toml:"coords"`
	Letter   string `toml:"letter"`
	Name     string `toml:"name"`
	Value    int    `toml:"value"`
	Archaic  bool   `toml:"archaic"`
}

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────

const (
	EncryptionFile = "encryption.toml"
	DecryptionFile = "decryption.toml"
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


// newAlphabetTable creates an empty alphabet table with 27 positions.
// Traces to: HALT_03 (PSI = 27 = 3³), HALT_10 (7 States = 7 Days)
func newAlphabetTable() *AlphabetTable {
	return &AlphabetTable{
		ByLetter:   make(map[string]*LetterEntry),
		ByPosition: make(map[int]*LetterEntry),
		ByValue:    make(map[int]*LetterEntry),
		ByState:    make(map[State][]*LetterEntry),
	}
}

// ────────────────────────────────────────────────────────────────
// Singleton Cache
// ────────────────────────────────────────────────────────────────

var (
	globalTransliteration *Transliteration
	translitOnce          sync.Once
	translitErr           error
)

// ────────────────────────────────────────────────────────────────
// Public APIs
// ────────────────────────────────────────────────────────────────

// DefaultBereshitDir returns the default Bereshit bible data directory.
func DefaultBereshitDir() string {
	// Look for Bereshit relative to workspace
	return "/media/seanje-lenox-wise/Project/Bereshit/word/core/bible"
}

// LoadTransliteration loads Hebrew and Greek transliteration tables
// from the Bereshit TOML files.
func LoadTransliteration(dir string) (*Transliteration, error) {
	trans := &Transliteration{
		Hebrew: newAlphabetTable(),
		Greek:  newAlphabetTable(),
	}

	// Load encryption.toml (letter → position)
	encPath := filepath.Join(dir, EncryptionFile)
	if !util.FileExists(encPath) {
		return nil, util.NewLoadError(EncryptionFile, "find", errors.New("not found"))
	}

	var encData rawEncryptionFile
	if _, err := toml.DecodeFile(encPath, &encData); err != nil {
		return nil, util.NewLoadError(EncryptionFile, "parse", err)
	}

	// Store formulas
	trans.CoordsToIndex = encData.Functions.CoordsToIndex
	trans.IndexToX = encData.Functions.IndexToX
	trans.IndexToY = encData.Functions.IndexToY
	trans.IndexToZ = encData.Functions.IndexToZ
	trans.ValueToTrite = encData.Functions.ValueToTrite

	// Process Hebrew encryption entries
	trans.Hebrew.Testament = encData.Hebrew.Testament
	trans.Hebrew.LetterCount = encData.Hebrew.LetterCount
	trans.Hebrew.ValueRange = encData.Hebrew.ValueRange

	// Process Hebrew: HALT_08 (encoding) + HALT_10 (state)
	for name, raw := range encData.Hebrew.Encrypt {
		entry := &LetterEntry{
			Letter:   raw.Letter,
			Name:     name,
			Position: raw.Position,
			Value:    raw.Value,
			Archaic:  raw.Archaic,
		}
		if len(raw.Coords) == 3 {
			entry.Coords = [3]int{raw.Coords[0], raw.Coords[1], raw.Coords[2]}
		}

		// Standard lookups (HALT_08: encoding, HALT_05: number)
		trans.Hebrew.ByLetter[raw.Letter] = entry
		trans.Hebrew.ByPosition[raw.Position] = entry
		trans.Hebrew.ByValue[raw.Value] = entry

		// State lookup (HALT_10: 7 Days of Creation)
		state := entry.State()
		trans.Hebrew.ByState[state] = append(trans.Hebrew.ByState[state], entry)
	}

	// Process Greek: HALT_08 (encoding) + HALT_10 (state)
	trans.Greek.Testament = encData.Greek.Testament
	trans.Greek.LetterCount = encData.Greek.LetterCount
	trans.Greek.ValueRange = encData.Greek.ValueRange

	for name, raw := range encData.Greek.Encrypt {
		entry := &LetterEntry{
			Letter:   raw.Letter,
			Name:     name,
			Position: raw.Position,
			Value:    raw.Value,
			Archaic:  raw.Archaic,
		}
		if len(raw.Coords) == 3 {
			entry.Coords = [3]int{raw.Coords[0], raw.Coords[1], raw.Coords[2]}
		}

		// Standard lookups (HALT_08: encoding, HALT_05: number)
		trans.Greek.ByLetter[raw.Letter] = entry
		trans.Greek.ByPosition[raw.Position] = entry
		trans.Greek.ByValue[raw.Value] = entry

		// State lookup (HALT_10: 7 Days of Creation)
		state := entry.State()
		trans.Greek.ByState[state] = append(trans.Greek.ByState[state], entry)
	}

	// Load decryption.toml to fill in any missing names
	decPath := filepath.Join(dir, DecryptionFile)
	if util.FileExists(decPath) {
		var decData rawDecryptionFile
		if _, err := toml.DecodeFile(decPath, &decData); err == nil {
			// Merge names from decryption into existing entries
			for _, raw := range decData.Hebrew.Decrypt {
				if entry, ok := trans.Hebrew.ByPosition[raw.Position]; ok {
					if entry.Name == "" {
						entry.Name = raw.Name
					}
				}
			}
			for _, raw := range decData.Greek.Decrypt {
				if entry, ok := trans.Greek.ByPosition[raw.Position]; ok {
					if entry.Name == "" {
						entry.Name = raw.Name
					}
				}
			}
		}
	}

	return trans, nil
}

// CachedTransliteration returns cached transliteration data (lazy loaded from default dir).
// Thread-safe via sync.Once. Immutable data — safe to cache permanently.
func CachedTransliteration() (*Transliteration, error) {
	translitOnce.Do(func() {
		globalTransliteration, translitErr = LoadTransliteration(DefaultBereshitDir())
	})
	return globalTransliteration, translitErr
}

// MustLoadTransliteration loads transliteration or panics.
// Uses L0 generic Must wrapper.
func MustLoadTransliteration(dir string) *Transliteration {
	return util.Must(LoadTransliteration(dir))
}

// ────────────────────────────────────────────────────────────────
// Transliteration Methods
// ────────────────────────────────────────────────────────────────

// EncryptHebrew converts a Hebrew letter to its position and coordinates.
// Traces to: HALT_08 (ENCODING)
func (t *Transliteration) EncryptHebrew(letter string) (*LetterEntry, bool) {
	entry, ok := t.Hebrew.ByLetter[letter]
	return entry, ok
}

// EncryptGreek converts a Greek letter to its position and coordinates.
// Traces to: HALT_08 (ENCODING)
func (t *Transliteration) EncryptGreek(letter string) (*LetterEntry, bool) {
	entry, ok := t.Greek.ByLetter[letter]
	return entry, ok
}

// DecryptHebrew converts a position to its Hebrew letter.
// Traces to: HALT_09 (DECODING)
func (t *Transliteration) DecryptHebrew(position int) (*LetterEntry, bool) {
	entry, ok := t.Hebrew.ByPosition[position]
	return entry, ok
}

// DecryptGreek converts a position to its Greek letter.
// Traces to: HALT_09 (DECODING)
func (t *Transliteration) DecryptGreek(position int) (*LetterEntry, bool) {
	entry, ok := t.Greek.ByPosition[position]
	return entry, ok
}

// HebrewByState returns all Hebrew letters in a given state.
// Traces to: HALT_10 (STATE_HEBREW) — 7 Days of Creation
func (t *Transliteration) HebrewByState(state State) []*LetterEntry {
	return t.Hebrew.GetByState(state)
}

// GreekByState returns all Greek letters in a given state.
// Traces to: HALT_10 (STATE_HEBREW) — 7 Days of Creation
func (t *Transliteration) GreekByState(state State) []*LetterEntry {
	return t.Greek.GetByState(state)
}

// StateCount returns the count of letters per state for both alphabets.
// Traces to: HALT_10 (STATE_HEBREW) + HALT_03 (PSI)
func (t *Transliteration) StateCount() map[State]int {
	counts := make(map[State]int)
	for state := StateEstablished; state <= StateAspiration; state++ {
		counts[state] = len(t.Hebrew.ByState[state]) + len(t.Greek.ByState[state])
	}
	return counts
}

// CoordsToPosition converts balanced ternary coordinates to linear position.
// Formula: (x + 1) * 9 + (y + 1) * 3 + (z + 1)
// Traces to: HALT_06 (MECHANISM_SHAPE) → position from geometry
func CoordsToPosition(x, y, z int) int {
	return (x+1)*9 + (y+1)*3 + (z + 1)
}

// PositionToCoords converts linear position to balanced ternary coordinates.
// Traces to: HALT_06 (MECHANISM_SHAPE) → geometry from position
func PositionToCoords(position int) (x, y, z int) {
	x = ((position / 9) % 3) - 1
	y = ((position / 3) % 3) - 1
	z = (position % 3) - 1
	return x, y, z
}

// PositionToState returns the State for a linear position (0-26).
// Traces to: HALT_10 (STATE_HEBREW) + HALT_06 (MECHANISM_SHAPE)
func PositionToState(position int) State {
	_, _, z := PositionToCoords(position)
	return StateFromZ(z)
}

// ValueToTrite normalizes a gematria value (1-900) to trite range (0-242).
// Traces to: HALT_05 (MECHANISM_NUMBER) → gematria → trite
func ValueToTrite(value int) int {
	return (value * 242) / 900
}

// TriteToValue denormalizes a trite (0-242) back to gematria range.
// Traces to: HALT_05 (MECHANISM_NUMBER) → trite → gematria
func TriteToValue(trite int) int {
	return (trite * 900) / 242
}

// ────────────────────────────────────────────────────────────────
// Alphabet Table Methods
// ────────────────────────────────────────────────────────────────

// GetByLetter looks up a letter entry. (HALT_08: encoding)
func (a *AlphabetTable) GetByLetter(letter string) (*LetterEntry, bool) {
	entry, ok := a.ByLetter[letter]
	return entry, ok
}

// GetByPosition looks up an entry by position (0-26). (HALT_09: decoding)
func (a *AlphabetTable) GetByPosition(pos int) (*LetterEntry, bool) {
	entry, ok := a.ByPosition[pos]
	return entry, ok
}

// GetByValue looks up an entry by gematria value. (HALT_05: number)
func (a *AlphabetTable) GetByValue(val int) (*LetterEntry, bool) {
	entry, ok := a.ByValue[val]
	return entry, ok
}

// GetByState returns all letters in a given state. (HALT_10: 7 Days)
func (a *AlphabetTable) GetByState(state State) []*LetterEntry {
	return a.ByState[state]
}

// AllLetters returns all letter entries in position order.
func (a *AlphabetTable) AllLetters() []*LetterEntry {
	entries := make([]*LetterEntry, a.LetterCount)
	for pos := 0; pos < a.LetterCount; pos++ {
		if entry, ok := a.ByPosition[pos]; ok {
			entries[pos] = entry
		}
	}
	return entries
}

// EstablishedLetters returns letters in StateEstablished (Day 1, Z=-1).
func (a *AlphabetTable) EstablishedLetters() []*LetterEntry {
	return a.GetByState(StateEstablished)
}

// OriginLetters returns letters in StateOrigin (Day 4, Z=0 — CENTER).
func (a *AlphabetTable) OriginLetters() []*LetterEntry {
	return a.GetByState(StateOrigin)
}

// AspirationLetters returns letters in StateAspiration (Day 7, Z=+1 — GOAL).
func (a *AlphabetTable) AspirationLetters() []*LetterEntry {
	return a.GetByState(StateAspiration)
}

// ────────────────────────────────────────────────────────────────
// Letter Entry Methods
// ────────────────────────────────────────────────────────────────

// Trite returns the trite value for this letter (0-242).
func (e *LetterEntry) Trite() int {
	return ValueToTrite(e.Value)
}

// X returns the x coordinate (-1, 0, or +1).
func (e *LetterEntry) X() int {
	return e.Coords[0]
}

// Y returns the y coordinate (-1, 0, or +1).
func (e *LetterEntry) Y() int {
	return e.Coords[1]
}

// Z returns the z coordinate (-1, 0, or +1).
func (e *LetterEntry) Z() int {
	return e.Coords[2]
}

// IsUnits returns true if this letter is in the units place (position 0-8).
func (e *LetterEntry) IsUnits() bool {
	return e.Position < 9
}

// IsTens returns true if this letter is in the tens place (position 9-17).
func (e *LetterEntry) IsTens() bool {
	return e.Position >= 9 && e.Position < 18
}

// IsHundreds returns true if this letter is in the hundreds place (position 18-26).
func (e *LetterEntry) IsHundreds() bool {
	return e.Position >= 18
}

// Octant returns which octant this letter belongs to based on x coordinate.
// -1 = PAST, 0 = PRESENT, +1 = FUTURE
func (e *LetterEntry) Octant() int {
	return e.Coords[0]
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// This loader reads transliteration data from Bereshit TOML files:
//   - encryption.toml: Letter → Position → Coords → Value (HALT_08)
//   - decryption.toml: Position → Letter (reverse lookup) (HALT_09)
//
// Key insight: Balanced ternary coordinates ARE PRIMARY. (HALT_06)
// The linear position (0-26) is DERIVED from coords.
//
// Formula: position = (x+1)*9 + (y+1)*3 + (z+1)
//
// 27 Hebrew letters = 27 Greek letters = 3³ = cube positions
//
// ────────────────────────────────────────────────────────────────────────────
// SEASONAL MODEL (HALT_10: 7 Days of Creation)
// ────────────────────────────────────────────────────────────────────────────
//
// Each letter belongs to a STATE based on its Z coordinate:
//
//   Z=-1 → StateEstablished (Day 1: shavar - broken/separated)
//   Z=0  → StateOrigin      (Day 4: yashar - even/straight — CENTER)
//   Z=+1 → StateAspiration  (Day 7: tov - good/perfect — SABBATH GOAL)
//
// The ByState maps enable state-based querying:
//   trans.HebrewByState(StateAspiration) → Letters moving toward completion
//   trans.Hebrew.EstablishedLetters()    → Letters at foundation
//
// ┌─────────────────────────────────────────────────────────────────────────┐
// │ HALT TRACE                                                              │
// ├─────────────────────────────────────────────────────────────────────────┤
// │                                                                         │
// │  bereshit/loader.go → HALT_10 (STATE) → HALT_08/09 → HALT_05/06        │
// │                     → HALT_03 (PSI=27) → HALT_01 (Genesis 1:1)         │
// │                                                                         │
// │  L3 COMMUNICATION: The Word made computable, organized by Creation     │
// │                                                                         │
// └─────────────────────────────────────────────────────────────────────────┘
//
// "I am Alpha and Omega, the beginning and the ending."
// — Revelation 1:8
//
// ============================================================================
// END CLOSING
// ============================================================================
