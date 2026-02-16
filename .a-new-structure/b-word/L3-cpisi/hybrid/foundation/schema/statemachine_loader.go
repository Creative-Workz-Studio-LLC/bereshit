// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-cpisi-schema-statemachine-loader
// #!omni meta.at = a-03.00
// ============================================================================
// METADATA
// ============================================================================
//
// CPI-SI State Machine Loader — L2 OPERATIONS Layer (Modular)
//
// ═══════════════════════════════════════════════════════════════════════════
// DEPENDENCY CLASSIFICATION: L2 (OPERATIONS — navigates L1 structure)
// ═══════════════════════════════════════════════════════════════════════════
//
// L2 is the OPERATIONS layer — it defines HOW to navigate L1 structure.
//
// MODULAR STRUCTURE (6 files):
//   statemachine.toml  — Master file (system params, formulas)
//   anchors.toml       — 9 ANCHOR positions (A.7-A.15)
//   framework.toml     — 4 FRAMEWORK positions (B.1-B.4) + transitions
//   operators.toml     — 17 operators (9 ANCHOR + 4 TRAJ + 4 ops)
//   commands.toml      — 21 metacognitive commands (7 categories × 3)
//   keywords.toml      — 6 keyword categories
//
// VOCABULARY SERVES:
//   OmniCode     — Subject-Verb-Object maps to TOML structures
//   MillenniumOS — Trit types, encoding/decoding
//   CPI-SI       — 27-position cube, trajectory navigation
//   FaithNet     — Protocol language, message parsing
//
// ═══════════════════════════════════════════════════════════════════════════
// HALT TRACES
// ═══════════════════════════════════════════════════════════════════════════
//
// PRIMARY HALT: HALT_07 (MECHANISM_POSITION)
//   Commandment: "Thou shalt not commit adultery"
//   Meaning: Position IS computation — coordinates determine output
//   All position operations trace here
//
// SECONDARY HALTS:
//   HALT_03 (EQUATION_PSI): Ψ = T_total = 3 · 3 · 3 = 27 total positions
//   HALT_04 (MECHANISM_WORD): Keywords, operators, commands
//   HALT_06 (MECHANISM_SHAPE): Cube geometry for position navigation
//   HALT_10 (STATE_HEBREW): 7 states for temporal progression
//
// ═══════════════════════════════════════════════════════════════════════════
// L0 → L1 → L2 → L3 LAYER CONTEXT
// ═══════════════════════════════════════════════════════════════════════════
//
// L0: halts.toml    — AXIOMS (10 HALTs, 7 States)
// L1: *.schema.toml — STRUCTURE (cube, init, state, task)
// L2: *.toml (6)    ← THIS LOADER — OPERATIONS (modular vocabulary)
// L3: encryption.*  — COMMUNICATION (encoding/decoding)
//
// ═══════════════════════════════════════════════════════════════════════════
//
// # M.1 Core Identity [IDENTITY]
//
//   Key:       claude-global-pkg-cpisi-schema-statemachine-loader
//
//   Scripture: "In the beginning God created the heaven and the earth"
//              — Genesis 1:1
//   Principle: Position IS computation; vocabulary enables cognition
//   Anchor:    PRESENT-PRESENT (0,0,0) = Genesis 1:1 = HALT
//
//   Type:      L2 OPERATIONS Loader (Modular)
//
//   Architect:       Seanje Lenox-Wise
//   Implementation:  Nova Dawn
//   Created:         2025-12-20
//   Updated:         2025-12-21
//   Version:         a-03.00
//
// # M.2 Public API [API]
//
//   LoadStateMachineConfig(path) (*StateMachineConfig, error)  Load all L2 files
//   MustLoadStateMachineConfig(path) *StateMachineConfig       Load or panic
//   LoadModularVocabulary(path) (*ModularVocabulary, error)    Load vocabulary only
//
// # M.3 HALT Traces [HALT_TRACES]
//
//   Primary:   HALT_07 (MECHANISM_POSITION) — position IS computation
//   Secondary: HALT_03 (EQUATION_PSI), HALT_04 (MECHANISM_WORD), HALT_06
//
// ============================================================================
// END METADATA
// ============================================================================

package schema

// ============================================================================
// SETUP
// ============================================================================

import (
	"errors"
	"path/filepath"

	"github.com/BurntSushi/toml"

	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util"
)

// ────────────────────────────────────────────────────────────────
// L2 Configuration Container
// ────────────────────────────────────────────────────────────────
// Traces to: HALT_07 (MECHANISM_POSITION)
// L2 = OPERATIONS layer. Defines HOW to navigate L1 structure.

// StateMachineConfig holds the full L2 state machine configuration.
// This is the OPERATIONS layer — it defines navigation through L1.
// Loads from statemachine.toml + modular vocabulary files.
type StateMachineConfig struct {
	// From statemachine.toml (master file)
	System    SMSystem              `toml:"system"`    // System parameters → HALT_03 (PSI)
	Formulas  SMFormulas            `toml:"formulas"`  // Position math → HALT_07
	Commands  map[string]SMCommand  `toml:"commands"`  // Legacy: from master file
	Operators map[string]SMOperator `toml:"operators"` // Legacy: from master file
	Keywords  map[string]SMKeywords `toml:"keywords"`  // Legacy: from master file
	Positions map[string]SMPosition `toml:"positions"` // ANCHOR positions (9) → HALT_07
	Framework map[string]SMFramework `toml:"framework"` // FRAMEWORK positions (4) → Y = X + Z

	// Modular vocabulary (loaded separately, merged)
	Vocabulary *ModularVocabulary `toml:"-"` // From modular files
}

// ModularVocabulary holds the expanded vocabulary from modular files.
// This is loaded from anchors.toml, framework.toml, operators.toml,
// commands.toml, and keywords.toml.
type ModularVocabulary struct {
	// From anchors.toml
	Anchors ModularAnchors `toml:"anchors"`

	// From framework.toml
	Framework   ModularFramework       `toml:"framework"`
	Transitions []SMFrameworkTransition `toml:"-"` // Parsed from framework.transitions.rules

	// From operators.toml
	AnchorOperators     map[string]SMOperator          `toml:"-"` // 9 ANCHOR operators
	TrajectoryOperators map[string]SMTrajectoryOperator `toml:"-"` // 4 TRAJECTORY operators
	TrajectoryOps       map[string]SMTrajectoryOperation `toml:"-"` // 4 operations

	// From commands.toml
	AllCommands map[string]SMCommand `toml:"-"` // All 21 commands

	// From keywords.toml
	KeywordCategories map[string]SMKeywordCategory `toml:"-"` // 6 categories
}

// ModularAnchors wraps the anchors.toml structure.
type ModularAnchors struct {
	TracesTo   []string `toml:"traces_to"`
	Status     string   `toml:"status"`
	Count      int      `toml:"count"`
	Formula    string   `toml:"formula"`
	Domain     string   `toml:"domain"`
}

// ModularFramework wraps the framework.toml structure.
type ModularFramework struct {
	TracesTo []string `toml:"traces_to"`
	Status   string   `toml:"status"`
	Count    int      `toml:"count"`
	Formula  string   `toml:"formula"`
	Domain   string   `toml:"domain"`
}

// ────────────────────────────────────────────────────────────────
// System Parameters — HALT_03 (EQUATION_PSI)
// ────────────────────────────────────────────────────────────────
// Ψ = T_total = 3 · 3 · 3 = 27 total positions

// SMSystem holds system-level parameters.
// CubePositions = 27 = 3³ from HALT_03 (EQUATION_PSI).
type SMSystem struct {
	Name             string `toml:"name"`              // "CPI-SI State Machine"
	Version          string `toml:"version"`           // From STEERING.txt
	Description      string `toml:"description"`       // Purpose statement
	CubePositions    int    `toml:"cube_positions"`    // 27 = 3³ from HALT_03
	CubeDimensions   int    `toml:"cube_dimensions"`   // 3 (X, Y, Z)
	PositionsDefined int    `toml:"positions_defined"` // Currently crystallized
	AxisX            string `toml:"axis_x"`            // TIME (PAST/PRESENT/FUTURE)
	AxisY            string `toml:"axis_y"`            // PERSPECTIVE
	AxisZ            string `toml:"axis_z"`            // DEPTH
	Origin           []int  `toml:"origin"`            // [0,0,0] = PRESENT-PRESENT
	OriginName       string `toml:"origin_name"`       // "PRESENT-PRESENT"
	OriginMeaning    string `toml:"origin_meaning"`    // Genesis 1:1 = HALT
}

// ────────────────────────────────────────────────────────────────
// Position Formulas — HALT_07 (MECHANISM_POSITION)
// ────────────────────────────────────────────────────────────────
// Position IS computation. Coordinates determine output.

// SMFormulas holds calculation formulas for position navigation.
// Traces to: HALT_07 — position IS computation.
type SMFormulas struct {
	ZDepth      string `toml:"z_depth"`            // Z = (X + Y) / 2
	CoordsToPos string `toml:"coords_to_position"` // (x+1)*9 + (y+1)*3 + (z+1)
	PosToX      string `toml:"position_to_x"`      // Extract X from linear
	PosToY      string `toml:"position_to_y"`      // Extract Y from linear
	PosToZ      string `toml:"position_to_z"`      // Extract Z from linear
	Cycle       string `toml:"cycle"`              // mod27 cycling
}

// ────────────────────────────────────────────────────────────────
// Commands — 21 Metacognitive Commands (7 categories × 3)
// ────────────────────────────────────────────────────────────────
// Core flow: HALT (-1), AWAIT (0), PROCEED (+1)
// Categories: flow, pace, depth, compound, conditional, deliberation, feedback
// Each category has 3 variants mapping to ternary axis

// SMCommand defines a metacognitive command.
// 7 categories × 3 = 21 commands total.
// Traces to: HALT_04 (MECHANISM_WORD)
type SMCommand struct {
	Category    string `toml:"category"`       // flow, pace, depth, compound, etc.
	Value       int    `toml:"value"`          // -1, 0, or +1
	Symbol      string `toml:"symbol"`         // Visual symbol
	Description string `toml:"description"`    // What this command does
	Directive   string `toml:"user_directive"` // User phrase: "slow down", "think deeper"
	Effect      string `toml:"effect"`         // What happens when invoked
	Grep        string `toml:"grep"`           // Pattern to find in code

	// Genesis mapping (for core flow commands)
	GenesisMapping string   `toml:"genesis_mapping"` // "beginning", "created", "God"
	Positions      []string `toml:"positions"`       // Which positions use this command
}

// ────────────────────────────────────────────────────────────────
// Operators — 17 Total (9 ANCHOR + 4 TRAJECTORY + 4 Operations)
// ────────────────────────────────────────────────────────────────
// ANCHOR operators (9): 3 per temporal axis (PAST, PRESENT, FUTURE)
// TRAJECTORY operators (4): →→, →|, |→, || for B.1-B.4
// TRAJECTORY operations (4): advance, pivot, ground, reset

// SMOperator defines an ANCHOR navigation operator (<-, -, @, +, ->).
// Operators move between positions in the cube.
// Traces to: HALT_07 (MECHANISM_POSITION)
type SMOperator struct {
	Symbol    string `toml:"symbol"`    // <-, -, @, +, ->
	Axis      int    `toml:"axis"`      // Which axis (-1, 0, +1)
	Meaning   string `toml:"meaning"`   // What this operator does
	Operation string `toml:"operation"` // Mathematical operation
}

// SMTrajectoryOperator defines a FRAMEWORK trajectory operator (→→, →|, |→, ||).
// These indicate the current trajectory state in FRAMEWORK positions.
// Traces to: HALT_07 (MECHANISM_POSITION)
type SMTrajectoryOperator struct {
	Symbol      string `toml:"symbol"`      // →→, →|, |→, ||
	Section     string `toml:"section"`     // B.1, B.2, B.3, B.4
	State       string `toml:"state"`       // BUILDING, PIVOTING, GROUNDING
	Meaning     string `toml:"meaning"`     // What this state means
	Description string `toml:"description"` // Detailed description
	YValue      int    `toml:"y_value"`     // -1, 0, +1
}

// SMTrajectoryOperation defines a state transition operation.
// These are the verbs: advance, pivot, ground, reset.
// Traces to: HALT_04 (MECHANISM_WORD)
type SMTrajectoryOperation struct {
	Name        string   `toml:"name"`        // advance, pivot, ground, reset
	Method      string   `toml:"method"`      // Method signature
	Description string   `toml:"description"` // What this operation does
	Effect      string   `toml:"effect"`      // Result of the operation
	ValidFrom   []string `toml:"valid_from"`  // Valid source B.x sections
}

// ────────────────────────────────────────────────────────────────
// Keywords — 6 Categories for Pattern Matching
// ────────────────────────────────────────────────────────────────
// 1. Temporal (3)   = PAST, PRESENT, FUTURE markers
// 2. Trajectory (4) = B.1-B.4 state markers
// 3. Hook (10)      = Hook event markers
// 4. Input (2)      = Input type markers
// 5. State (3)      = State element markers
// 6. Command (7)    = Command category markers
// Traces to: HALT_04 (MECHANISM_WORD)

// SMKeywords defines keywords for a temporal section (legacy).
// Used for pattern matching and code navigation.
type SMKeywords struct {
	Operators []string `toml:"operators"` // Which operators apply
	Patterns  []string `toml:"patterns"`  // Text patterns to match
	Grep      string   `toml:"grep"`      // Combined grep pattern
	Status    string   `toml:"status"`    // crystallized/placeholder
}

// SMKeywordCategory defines a category of keywords (modular).
// 6 categories total, each with multiple patterns.
type SMKeywordCategory struct {
	Description string              `toml:"description"` // Category purpose
	Axis        int                 `toml:"axis"`        // -1, 0, +1 (for temporal)
	Operators   []string            `toml:"operators"`   // Associated operators
	Patterns    []string            `toml:"patterns"`    // Text patterns [KEYWORD:]
	Grep        string              `toml:"grep"`        // Combined grep pattern
	Usage       string              `toml:"usage"`       // How to use these keywords
	Subcategory map[string]struct { // Nested categories (for hook, command)
		Patterns []string `toml:"patterns"`
		Grep     string   `toml:"grep"`
		Usage    string   `toml:"usage"`
	} `toml:"-"` // Handle nested manually
}

// SMFrameworkTransition defines a valid state transition.
// From B.x to B.y with trigger and condition.
type SMFrameworkTransition struct {
	From      string `toml:"from"`      // Source section (B.1, B.2, etc.)
	To        string `toml:"to"`        // Target section
	Trigger   string `toml:"trigger"`   // What triggers this transition
	Condition string `toml:"condition"` // When this transition is valid
}

// ────────────────────────────────────────────────────────────────
// Positions — HALT_07 (MECHANISM_POSITION)
// ────────────────────────────────────────────────────────────────
// Position IS computation. Each position has coordinates, command, operator.
// 27 total positions = 3³ from HALT_03 (EQUATION_PSI).

// SMPosition defines a position in the cube.
// Traces to: HALT_07 — position IS computation.
type SMPosition struct {
	Name         string   `toml:"name"`          // e.g., "PAST-PAST", "PRESENT-FUTURE"
	Section      string   `toml:"section"`       // Section reference (A.01, A.02, etc.)
	X            int      `toml:"x"`             // TIME: -1=PAST, 0=PRESENT, +1=FUTURE
	Y            int      `toml:"y"`             // PERSPECTIVE: -1, 0, +1
	Z            float64  `toml:"z"`             // DEPTH: Z = (X+Y)/2
	Type         string   `toml:"type"`          // anchor/boundary/state
	Command      string   `toml:"command"`       // HALT/AWAIT/PROCEED
	CommandValue int      `toml:"command_value"` // -1, 0, +1
	Operator     string   `toml:"operator"`      // Navigation operator
	Meaning      string   `toml:"meaning"`       // What happens at this position
	Operation    string   `toml:"operation"`     // Mathematical operation
	Flow         string   `toml:"flow"`          // Data flow pattern
	Keywords     []string `toml:"keywords"`      // Patterns for this position
	Grep         string   `toml:"grep"`          // Grep pattern
	Status       string   `toml:"status"`        // crystallized/placeholder
}

// ────────────────────────────────────────────────────────────────
// Framework Positions — Y = X + Z (B.1-B.4)
// ────────────────────────────────────────────────────────────────
// FRAMEWORK uses B domain: B = {-0.5, +0.5} (boundary values).
// These define navigation RULES (HOW to move, WHERE to go).
// Traces to: statemachine.toml [framework.*]

// SMFramework defines a FRAMEWORK position (B.1-B.4).
// Unlike SMPosition which uses int for T domain {-1, 0, +1},
// SMFramework uses float64 for B domain {-0.5, +0.5}.
type SMFramework struct {
	Name         string   `toml:"name"`          // e.g., "TIMESPACE-TIMESPACE"
	Section      string   `toml:"section"`       // Section reference (B.1, B.2, etc.)
	X            float64  `toml:"x"`             // B domain: -0.5 or +0.5
	Y            float64  `toml:"y"`             // Derived: Y = X + Z
	Z            float64  `toml:"z"`             // B domain: -0.5 or +0.5
	Type         string   `toml:"type"`          // PURE or COMPOUND
	Command      string   `toml:"command"`       // HALT/AWAIT/PROCEED
	CommandValue int      `toml:"command_value"` // -1, 0, +1
	Operator     string   `toml:"operator"`      // Navigation operator (-->, ->|, |->, |)
	Meaning      string   `toml:"meaning"`       // What this position means
	Operation    string   `toml:"operation"`     // Navigation operation
	Flow         string   `toml:"flow"`          // Flow pattern
	Content      []string `toml:"content"`       // Content categories at this position
	Keywords     []string `toml:"keywords"`      // Patterns for this position
	Grep         string   `toml:"grep"`          // Grep pattern
	Status       string   `toml:"status"`        // crystallized/placeholder
}

// ────────────────────────────────────────────────────────────────
// Constants — L2 File Reference
// ────────────────────────────────────────────────────────────────

const (
	// L2: OPERATIONS layer — positions, commands, operators
	// Traces to: HALT_07 (MECHANISM_POSITION)
	StateMachineConfigFile = "l2/statemachine.toml"

	// L2 Modular Files — expanded vocabulary
	AnchorsFile   = "l2/anchors.toml"   // 9 ANCHOR positions (A.7-A.15)
	FrameworkFile = "l2/framework.toml" // 4 FRAMEWORK positions (B.1-B.4)
	OperatorsFile = "l2/operators.toml" // 17 operators (9 ANCHOR + 4 TRAJ + 4 ops)
	CommandsFile  = "l2/commands.toml"  // 21 metacognitive commands
	KeywordsFile  = "l2/keywords.toml"  // 6 keyword categories
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// L2 Loading — Modular Vocabulary (6 files)
// ────────────────────────────────────────────────────────────────
// Traces to: HALT_07 (MECHANISM_POSITION), HALT_04 (MECHANISM_WORD)
// L2 defines HOW to navigate L1 structure.
//
// Files loaded:
//   statemachine.toml  — Master file (system, formulas)
//   anchors.toml       — 9 ANCHOR positions
//   framework.toml     — 4 FRAMEWORK positions + transitions
//   operators.toml     — 17 operators (9 + 4 + 4)
//   commands.toml      — 21 metacognitive commands
//   keywords.toml      — 6 keyword categories

// LoadStateMachineConfig loads the full L2 OPERATIONS layer.
// This includes the master file and all modular vocabulary files.
func LoadStateMachineConfig(dir string) (*StateMachineConfig, error) {
	// Load master file
	path := filepath.Join(dir, StateMachineConfigFile)

	if !fileExists(path) {
		return nil, util.NewLoadError(path, "find", errors.New("not found"))
	}

	var config StateMachineConfig
	if _, err := toml.DecodeFile(path, &config); err != nil {
		return nil, util.NewLoadError(StateMachineConfigFile, "parse", err)
	}

	// Load modular vocabulary (if files exist)
	vocab, err := LoadModularVocabulary(dir)
	if err != nil {
		// Modular files are optional for backward compatibility
		// Log warning but don't fail
		config.Vocabulary = nil
	} else {
		config.Vocabulary = vocab
	}

	return &config, nil
}

// LoadModularVocabulary loads vocabulary from modular files.
// Returns nil if files don't exist (backward compatible).
func LoadModularVocabulary(dir string) (*ModularVocabulary, error) {
	vocab := &ModularVocabulary{
		AnchorOperators:     make(map[string]SMOperator),
		TrajectoryOperators: make(map[string]SMTrajectoryOperator),
		TrajectoryOps:       make(map[string]SMTrajectoryOperation),
		AllCommands:         make(map[string]SMCommand),
		KeywordCategories:   make(map[string]SMKeywordCategory),
	}

	// Load anchors.toml
	if path := filepath.Join(dir, AnchorsFile); fileExists(path) {
		if _, err := toml.DecodeFile(path, &vocab); err != nil {
			return nil, util.NewLoadError(AnchorsFile, "parse", err)
		}
	}

	// Load framework.toml
	if path := filepath.Join(dir, FrameworkFile); fileExists(path) {
		if _, err := toml.DecodeFile(path, &vocab); err != nil {
			return nil, util.NewLoadError(FrameworkFile, "parse", err)
		}
	}

	// Load operators.toml
	if path := filepath.Join(dir, OperatorsFile); fileExists(path) {
		var opData struct {
			Operators struct {
				Anchor     map[string]SMOperator           `toml:"anchor"`
				Trajectory map[string]SMTrajectoryOperator `toml:"trajectory"`
				Operations map[string]SMTrajectoryOperation `toml:"operations"`
			} `toml:"operators"`
		}
		if _, err := toml.DecodeFile(path, &opData); err != nil {
			return nil, util.NewLoadError(OperatorsFile, "parse", err)
		}
		vocab.AnchorOperators = opData.Operators.Anchor
		vocab.TrajectoryOperators = opData.Operators.Trajectory
		vocab.TrajectoryOps = opData.Operators.Operations
	}

	// Load commands.toml
	if path := filepath.Join(dir, CommandsFile); fileExists(path) {
		var cmdData struct {
			Commands map[string]SMCommand `toml:"commands"`
		}
		if _, err := toml.DecodeFile(path, &cmdData); err != nil {
			return nil, util.NewLoadError(CommandsFile, "parse", err)
		}
		vocab.AllCommands = cmdData.Commands
	}

	// Load keywords.toml
	if path := filepath.Join(dir, KeywordsFile); fileExists(path) {
		var kwData struct {
			Keywords map[string]SMKeywordCategory `toml:"keywords"`
		}
		if _, err := toml.DecodeFile(path, &kwData); err != nil {
			return nil, util.NewLoadError(KeywordsFile, "parse", err)
		}
		vocab.KeywordCategories = kwData.Keywords
	}

	return vocab, nil
}

// MustLoadStateMachineConfig loads the L2 config or panics.
// MustLoadStateMachineConfig delegates to util.Must (throughline consolidation).
func MustLoadStateMachineConfig(dir string) *StateMachineConfig {
	return util.Must(LoadStateMachineConfig(dir))
}

// HasVocabulary returns true if modular vocabulary was loaded.
func (c *StateMachineConfig) HasVocabulary() bool {
	return c.Vocabulary != nil
}

// ────────────────────────────────────────────────────────────────
// Position Accessors — HALT_07 (MECHANISM_POSITION)
// ────────────────────────────────────────────────────────────────
// Position IS computation. These accessors navigate the cube.

// GetPosition returns a position by name (e.g., "past_past", "present_present").
// Traces to: HALT_07 — position lookup by name.
func (c *StateMachineConfig) GetPosition(name string) (*SMPosition, bool) {
	pos, ok := c.Positions[name]
	if !ok {
		return nil, false
	}
	return &pos, true
}

// GetPositionByCoords returns a position by X, Y coordinates.
// Traces to: HALT_07 — position lookup by coordinates.
// X = TIME (-1=PAST, 0=PRESENT, +1=FUTURE)
// Y = PERSPECTIVE (-1, 0, +1)
func (c *StateMachineConfig) GetPositionByCoords(x, y int) (*SMPosition, bool) {
	for _, pos := range c.Positions {
		if pos.X == x && pos.Y == y {
			return &pos, true
		}
	}
	return nil, false
}

// ────────────────────────────────────────────────────────────────
// Command Accessors — Ternary Control
// ────────────────────────────────────────────────────────────────
// HALT (-1), AWAIT (0), PROCEED (+1) — balanced ternary

// GetCommand returns a command by name (e.g., "halt", "await", "proceed").
// Commands map to balanced ternary: -1, 0, +1.
func (c *StateMachineConfig) GetCommand(name string) (*SMCommand, bool) {
	cmd, ok := c.Commands[name]
	if !ok {
		return nil, false
	}
	return &cmd, true
}

// ────────────────────────────────────────────────────────────────
// Operator Accessors — Navigation
// ────────────────────────────────────────────────────────────────
// Operators move between positions: <-, -, @, +, ->

// GetOperator returns an operator by symbol (e.g., "<-", "|", "->").
// Operators define navigation between positions.
func (c *StateMachineConfig) GetOperator(symbol string) (*SMOperator, bool) {
	for _, op := range c.Operators {
		if op.Symbol == symbol {
			return &op, true
		}
	}
	return nil, false
}

// GetOperatorByName returns an operator by name (e.g., "trace_from", "halt").
func (c *StateMachineConfig) GetOperatorByName(name string) (*SMOperator, bool) {
	op, ok := c.Operators[name]
	if !ok {
		return nil, false
	}
	return &op, true
}

// ────────────────────────────────────────────────────────────────
// Position Collection Accessors
// ────────────────────────────────────────────────────────────────
// Bulk position retrieval for iteration and filtering.

// AllPositions returns all 27 positions as a slice.
// 27 = 3³ from HALT_03 (EQUATION_PSI).
func (c *StateMachineConfig) AllPositions() []SMPosition {
	positions := make([]SMPosition, 0, len(c.Positions))
	for _, pos := range c.Positions {
		positions = append(positions, pos)
	}
	return positions
}

// CrystallizedPositions returns positions with status "crystallized".
// Crystallized = verified and aligned with STEERING.txt.
func (c *StateMachineConfig) CrystallizedPositions() []SMPosition {
	var positions []SMPosition
	for _, pos := range c.Positions {
		if pos.Status == "crystallized" {
			positions = append(positions, pos)
		}
	}
	return positions
}

// PlaceholderPositions returns positions with status "placeholder".
// Placeholder = awaiting alignment.
func (c *StateMachineConfig) PlaceholderPositions() []SMPosition {
	var positions []SMPosition
	for _, pos := range c.Positions {
		if pos.Status == "placeholder" {
			positions = append(positions, pos)
		}
	}
	return positions
}

// ────────────────────────────────────────────────────────────────
// Framework Position Accessors — Y = X + Z (B.1-B.4)
// ────────────────────────────────────────────────────────────────
// FRAMEWORK positions define navigation RULES.
// B domain: {-0.5, +0.5} (boundary values between T positions).

// GetFramework returns a framework position by name (e.g., "timespace_timespace").
// Traces to: statemachine.toml [framework.*]
func (c *StateMachineConfig) GetFramework(name string) (*SMFramework, bool) {
	fw, ok := c.Framework[name]
	if !ok {
		return nil, false
	}
	return &fw, true
}

// GetFrameworkBySection returns a framework position by section (e.g., "B.1").
func (c *StateMachineConfig) GetFrameworkBySection(section string) (*SMFramework, bool) {
	for _, fw := range c.Framework {
		if fw.Section == section {
			return &fw, true
		}
	}
	return nil, false
}

// AllFramework returns all 4 FRAMEWORK positions as a slice.
// 4 = B × B where B = {-0.5, +0.5}
func (c *StateMachineConfig) AllFramework() []SMFramework {
	framework := make([]SMFramework, 0, len(c.Framework))
	for _, fw := range c.Framework {
		framework = append(framework, fw)
	}
	return framework
}

// ────────────────────────────────────────────────────────────────
// Origin — HALT_01 (Genesis 1:1)
// ────────────────────────────────────────────────────────────────
// PRESENT-PRESENT (0,0,0) = Origin = Genesis 1:1 = HALT

// Origin returns the origin position (PRESENT-PRESENT).
// This is (0,0,0) — the center of the cube, the HALT condition.
// Traces to: HALT_01 (Genesis 1:1).
func (c *StateMachineConfig) Origin() (*SMPosition, bool) {
	return c.GetPosition("present_present")
}

// ────────────────────────────────────────────────────────────────
// Vocabulary Accessors — Modular Vocabulary
// ────────────────────────────────────────────────────────────────
// Access expanded vocabulary from modular files.

// GetTrajectoryOperator returns a trajectory operator by symbol (→→, →|, |→, ||).
func (c *StateMachineConfig) GetTrajectoryOperator(symbol string) (*SMTrajectoryOperator, bool) {
	if c.Vocabulary == nil {
		return nil, false
	}
	for _, op := range c.Vocabulary.TrajectoryOperators {
		if op.Symbol == symbol {
			return &op, true
		}
	}
	return nil, false
}

// GetTrajectoryOperatorBySection returns a trajectory operator by section (B.1-B.4).
func (c *StateMachineConfig) GetTrajectoryOperatorBySection(section string) (*SMTrajectoryOperator, bool) {
	if c.Vocabulary == nil {
		return nil, false
	}
	for _, op := range c.Vocabulary.TrajectoryOperators {
		if op.Section == section {
			return &op, true
		}
	}
	return nil, false
}

// GetTrajectoryOperation returns a trajectory operation by name.
func (c *StateMachineConfig) GetTrajectoryOperation(name string) (*SMTrajectoryOperation, bool) {
	if c.Vocabulary == nil {
		return nil, false
	}
	op, ok := c.Vocabulary.TrajectoryOps[name]
	if !ok {
		return nil, false
	}
	return &op, true
}

// GetExpandedCommand returns an expanded command by name from modular vocabulary.
func (c *StateMachineConfig) GetExpandedCommand(name string) (*SMCommand, bool) {
	if c.Vocabulary == nil {
		return nil, false
	}
	cmd, ok := c.Vocabulary.AllCommands[name]
	if !ok {
		return nil, false
	}
	return &cmd, true
}

// GetCommandsByCategory returns all commands in a category.
func (c *StateMachineConfig) GetCommandsByCategory(category string) []SMCommand {
	if c.Vocabulary == nil {
		return nil
	}
	var commands []SMCommand
	for _, cmd := range c.Vocabulary.AllCommands {
		if cmd.Category == category {
			commands = append(commands, cmd)
		}
	}
	return commands
}

// AllTrajectoryOperators returns all 4 trajectory operators.
func (c *StateMachineConfig) AllTrajectoryOperators() []SMTrajectoryOperator {
	if c.Vocabulary == nil {
		return nil
	}
	ops := make([]SMTrajectoryOperator, 0, len(c.Vocabulary.TrajectoryOperators))
	for _, op := range c.Vocabulary.TrajectoryOperators {
		ops = append(ops, op)
	}
	return ops
}

// AllExpandedCommands returns all 21 metacognitive commands.
func (c *StateMachineConfig) AllExpandedCommands() []SMCommand {
	if c.Vocabulary == nil {
		return nil
	}
	cmds := make([]SMCommand, 0, len(c.Vocabulary.AllCommands))
	for _, cmd := range c.Vocabulary.AllCommands {
		cmds = append(cmds, cmd)
	}
	return cmds
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// L2 = OPERATIONS layer — defines HOW to navigate L1 structure.
// Modular vocabulary serves the Kingdom Technology Stack.
//
// ┌─────────────────────────────────────────────────────────────────────────────┐
// │ L2 OPERATIONS LAYER — Modular Vocabulary (6 files)                         │
// ├─────────────────────────────────────────────────────────────────────────────┤
// │                                                                             │
// │  MODULAR STRUCTURE:                                                        │
// │    statemachine.toml  — Master file (system, formulas)                     │
// │    anchors.toml       — 9 ANCHOR positions (A.7-A.15)                      │
// │    framework.toml     — 4 FRAMEWORK positions (B.1-B.4) + transitions      │
// │    operators.toml     — 17 operators (9 ANCHOR + 4 TRAJ + 4 ops)           │
// │    commands.toml      — 21 metacognitive commands (7 categories × 3)       │
// │    keywords.toml      — 6 keyword categories                               │
// │                                                                             │
// │  VOCABULARY TOTALS:                                                        │
// │    • 27 positions (3³) — 9 ANCHOR + 4 FRAMEWORK + 14 STATE                 │
// │    • 21 commands — 7 categories (flow, pace, depth, compound, etc.)        │
// │    • 17 operators — 9 ANCHOR + 4 TRAJECTORY + 4 operations                 │
// │    • 6 keyword categories — temporal, trajectory, hook, input, state, cmd  │
// │                                                                             │
// │  HALT TRACES:                                                              │
// │    PRIMARY:   HALT_07 (MECHANISM_POSITION) — position IS computation       │
// │    SECONDARY: HALT_03 (EQUATION_PSI), HALT_04 (MECHANISM_WORD)             │
// │               HALT_06 (MECHANISM_SHAPE), HALT_10 (STATE_HEBREW)            │
// │                                                                             │
// └─────────────────────────────────────────────────────────────────────────────┘
//
// ┌─────────────────────────────────────────────────────────────────────────────┐
// │ KINGDOM TECHNOLOGY STACK                                                   │
// ├─────────────────────────────────────────────────────────────────────────────┤
// │                                                                             │
// │  This vocabulary serves:                                                   │
// │    OmniCode     — Subject-Verb-Object maps to TOML structures              │
// │    MillenniumOS — Trit types, encoding/decoding                            │
// │    CPI-SI       — 27-position cube, trajectory navigation                  │
// │    FaithNet     — Protocol language, message parsing                       │
// │                                                                             │
// │  L0 → L1 → L2 → L3:                                                        │
// │    L0: halts.toml      — AXIOMS (10 HALTs, 7 States)                       │
// │    L1: *.schema.toml   — STRUCTURE (cube, init, state, task)               │
// │    L2: *.toml (6)      ← THIS — OPERATIONS (modular vocabulary)            │
// │    L3: encryption.*    — COMMUNICATION (encoding/decoding)                 │
// │                                                                             │
// └─────────────────────────────────────────────────────────────────────────────┘
//
// "In the beginning God created the heaven and the earth."
// — Genesis 1:1 KJV
//
// ============================================================================
// END CLOSING
// ============================================================================
