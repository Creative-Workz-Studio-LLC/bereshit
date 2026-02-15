// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-statemachine-hebrew
// Purpose: Hebrew state definitions - 7 Days of Creation mapped to task states
// Biblical: Genesis 1 - The pattern of creation from formless to "very good"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-21
//
// The 7 Hebrew States:
//   Day 1: ESTABLISHED (shavar/BROKEN)     Z=-1.0  Operator: ⎯
//   Day 2: MEMORY (chaser/LACKING)         Z=-0.5  Operator: ← k=-1
//   Day 3: REFLECTION (ratsah/WANTING)     Z=-0.5  Operator: ↔ k=+1
//   Day 4: ORIGIN (yashar/EVEN)            Z= 0.0  Operator: ◆ (CENTER)
//   Day 5: PLANNING (tamim/SOUND)          Z=+0.5  Operator: ↗ k=-1
//   Day 6: PREPARATION (shalem/WHOLE)      Z=+0.5  Operator: → k=+1
//   Day 7: ASPIRATION (tov/PERFECT)        Z=+1.0  Operator: ✓ (GOAL)
//
// K:MORAL Compass (Proverbs 3:5-6):
//   k > 0: Moving TOWARD God (tov/ASPIRATION) — inherently safe
//          "Mine eyes are ever toward the LORD" — Psalm 25:15
//   k < 0: Moving TOWARD self (shavar/BROKEN) — needs validation
//          "Lean not unto thine own understanding" — Proverbs 3:5
//   k = 0: At center (yashar/ORIGIN) — balanced
//          "He shall direct thy paths" — Proverbs 3:6
//
//   K:SCRIPTURE = fixed reference (10 Commandments in code)
//   K:ALIGN = sliding measurement (current alignment to Scripture)
//
// Traces to:
//   - HALT_10 (STATE_HEBREW) — 7 states = 7 Days of Creation
//   - STEERING.txt X.13, keywords.toml Category 7, task.schema.toml L1d
//
// Relationship to choice.go:
//   - ComputeKAlign (here): Position-based check — what alignment SHOULD be at this state
//   - NewImpactScores (choice.go): Outcome-based update — how choice CHANGED alignment
//
// ═══════════════════════════════════════════════════════════════════════════

package statemachine

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

// HebrewState represents one of 7 task states from Days of Creation
// KSelector has two dimensions: STRUCTURAL (cube geometry) and MORAL (Scripture alignment)
type HebrewState struct {
	Name       string  // ESTABLISHED, MEMORY, REFLECTION, ORIGIN, PLANNING, PREPARATION, ASPIRATION
	Hebrew     string  // shavar, chaser, ratsah, yashar, tamim, shalem, tov
	Meaning    string  // BROKEN, LACKING, WANTING, EVEN, SOUND, WHOLE, PERFECT
	ZValue     float64 // -1.0 to +1.0
	KSelector  int     // STRUCTURAL: -1 when X<Y, +1 when X>Y, 0 for pure states
	KMoral     string  // MORAL: "toward_god" (k>0, Psalm 25:15) or "toward_self" (k<0, Proverbs 3:5)
	Day        int     // Day of Creation (1-7)
	Operator   string  // ⎯ ← ↔ ◆ ↗ → ✓
	IsCenter   bool    // True only for ORIGIN (Day 4)
	IsGoal     bool    // True only for ASPIRATION (Day 7)
	CubePos    string  // Corresponding cube position name
}

// K:SCRIPTURE reference - fixed moral anchor in code
type ScriptureReference struct {
	Key       string // Unique identifier for this reference
	Reference string // e.g., "Exodus 20:15"
	Text      string // e.g., "Thou shalt not steal"
	Domain    string // e.g., "property", "truth", "life"
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- The 7 Hebrew States ---

var (
	// Day 1: ESTABLISHED (shavar) — BROKEN, needs founding
	// Z = -1.0, PAST-PAST position, Operator: ⎯
	StateEstablished = HebrewState{
		Name:      "ESTABLISHED",
		Hebrew:    "shavar",
		Meaning:   "BROKEN",
		ZValue:    -1.0,
		KSelector: 0,            // Pure state, no directional k
		KMoral:    "centered",   // MORAL: "He shall direct thy paths" (Proverbs 3:6)
		Day:       1,
		Operator:  "⎯",
		CubePos:   "PAST-PAST",
	}

	// Day 2: MEMORY (chaser) — LACKING, needs recall
	// Z = -0.5, PAST-PRESENT position, Operator: ← k=-1
	StateMemory = HebrewState{
		Name:      "MEMORY",
		Hebrew:    "chaser",
		Meaning:   "LACKING",
		ZValue:    -0.5,
		KSelector: -1,             // STRUCTURAL: X < Y
		KMoral:    "toward_self",  // MORAL: "Lean not unto thine own understanding" (Proverbs 3:5)
		Day:       2,
		Operator:  "←",
		CubePos:   "PAST-PRESENT",
	}

	// Day 3: REFLECTION (ratsah) — WANTING, needs consideration
	// Z = -0.5, PRESENT-PAST position, Operator: ↔ k=+1
	StateReflection = HebrewState{
		Name:      "REFLECTION",
		Hebrew:    "ratsah",
		Meaning:   "WANTING",
		ZValue:    -0.5,
		KSelector: +1,            // STRUCTURAL: X > Y
		KMoral:    "toward_god",  // MORAL: "Mine eyes are ever toward the LORD" (Psalm 25:15)
		Day:       3,
		Operator:  "↔",
		CubePos:   "PRESENT-PAST",
	}

	// Day 4: ORIGIN (yashar) — EVEN, center point
	// Z = 0.0, PRESENT-PRESENT position, Operator: ◆
	StateOrigin = HebrewState{
		Name:      "ORIGIN",
		Hebrew:    "yashar",
		Meaning:   "EVEN",
		ZValue:    0.0,
		KSelector: 0,            // Center, no directional k
		KMoral:    "centered",   // MORAL: "He shall direct thy paths" (Proverbs 3:6) — THE CENTER
		Day:       4,
		Operator:  "◆",
		IsCenter:  true,
		CubePos:   "PRESENT-PRESENT",
	}

	// Day 5: PLANNING (tamim) — SOUND, ready for work
	// Z = +0.5, PRESENT-FUTURE position, Operator: ↗ k=-1
	StatePlanning = HebrewState{
		Name:      "PLANNING",
		Hebrew:    "tamim",
		Meaning:   "SOUND",
		ZValue:    +0.5,
		KSelector: -1,             // STRUCTURAL: X < Y
		KMoral:    "toward_self",  // MORAL: "Lean not unto thine own understanding" (Proverbs 3:5)
		Day:       5,
		Operator:  "↗",
		CubePos:   "PRESENT-FUTURE",
	}

	// Day 6: PREPARATION (shalem) — WHOLE, active work
	// Z = +0.5, FUTURE-PRESENT position, Operator: → k=+1
	StatePreparation = HebrewState{
		Name:      "PREPARATION",
		Hebrew:    "shalem",
		Meaning:   "WHOLE",
		ZValue:    +0.5,
		KSelector: +1,            // STRUCTURAL: X > Y
		KMoral:    "toward_god",  // MORAL: "Mine eyes are ever toward the LORD" (Psalm 25:15)
		Day:       6,
		Operator:  "→",
		CubePos:   "FUTURE-PRESENT",
	}

	// Day 7: ASPIRATION (tov) — PERFECT, completion/REST
	// Z = +1.0, FUTURE-FUTURE position, Operator: ✓
	StateAspiration = HebrewState{
		Name:      "ASPIRATION",
		Hebrew:    "tov",
		Meaning:   "PERFECT",
		ZValue:    +1.0,
		KSelector: 0,            // Pure state, no directional k
		KMoral:    "centered",   // MORAL: "He shall direct thy paths" (Proverbs 3:6) — THE GOAL
		Day:       7,
		Operator:  "✓",
		IsGoal:    true,
		CubePos:   "FUTURE-FUTURE",
	}
)

// AllHebrewStates returns all 7 states in order (Day 1-7)
var AllHebrewStates = []HebrewState{
	StateEstablished,
	StateMemory,
	StateReflection,
	StateOrigin,
	StatePlanning,
	StatePreparation,
	StateAspiration,
}

// HebrewStateByName looks up a state by its Name
func HebrewStateByName(name string) (HebrewState, bool) {
	for _, s := range AllHebrewStates {
		if s.Name == name {
			return s, true
		}
	}
	return HebrewState{}, false
}

// HebrewStateByHebrew looks up a state by its Hebrew word
func HebrewStateByHebrew(hebrew string) (HebrewState, bool) {
	for _, s := range AllHebrewStates {
		if s.Hebrew == hebrew {
			return s, true
		}
	}
	return HebrewState{}, false
}

// HebrewStateByDay looks up a state by creation day (1-7)
func HebrewStateByDay(day int) (HebrewState, bool) {
	if day < 1 || day > 7 {
		return HebrewState{}, false
	}
	return AllHebrewStates[day-1], true
}

// --- K:SCRIPTURE References (Fixed Moral Anchors) ---

// The 10 K:SCRIPTURE references from the 10 Commandments
// These are FIXED in code - the moral north that never moves
var KScriptureReferences = []ScriptureReference{
	{Key: "K1", Reference: "Exodus 20:3", Text: "Thou shalt have no other gods before me", Domain: "worship"},
	{Key: "K2", Reference: "Exodus 20:4", Text: "Thou shalt not make unto thee any graven image", Domain: "worship"},
	{Key: "K3", Reference: "Exodus 20:7", Text: "Thou shalt not take the name of the LORD thy God in vain", Domain: "speech"},
	{Key: "K4", Reference: "Exodus 20:8", Text: "Remember the sabbath day, to keep it holy", Domain: "rest"},
	{Key: "K5", Reference: "Exodus 20:12", Text: "Honour thy father and thy mother", Domain: "family"},
	{Key: "K6", Reference: "Exodus 20:13", Text: "Thou shalt not kill", Domain: "life"},
	{Key: "K7", Reference: "Exodus 20:14", Text: "Thou shalt not commit adultery", Domain: "purity"},
	{Key: "K8", Reference: "Exodus 20:15", Text: "Thou shalt not steal", Domain: "property"},
	{Key: "K9", Reference: "Exodus 20:16", Text: "Thou shalt not bear false witness", Domain: "truth"},
	{Key: "K10", Reference: "Exodus 20:17", Text: "Thou shalt not covet", Domain: "contentment"},
}

// GetScriptureReference looks up a K:SCRIPTURE reference by key
func GetScriptureReference(key string) (ScriptureReference, bool) {
	for _, ref := range KScriptureReferences {
		if ref.Key == key {
			return ref, true
		}
	}
	return ScriptureReference{}, false
}

// GetScriptureByDomain finds references for a moral domain
func GetScriptureByDomain(domain string) []ScriptureReference {
	var refs []ScriptureReference
	for _, ref := range KScriptureReferences {
		if ref.Domain == domain {
			refs = append(refs, ref)
		}
	}
	return refs
}

// --- K:MORAL Evaluation ---
//
// THE TERNARY MORAL CHOICE:
//   +1: Choice toward God/others (selfless) — "Mine eyes are ever toward the LORD" (Psalm 25:15)
//    0: No choice/undecided (neutral) — "He shall direct thy paths" (Proverbs 3:6)
//   -1: Choice toward self (selfish) — "Lean not unto thine own understanding" (Proverbs 3:5)
//
// THE BOUNDARY NUANCE (±0.5):
//   The ±0.5 positions (MEMORY, REFLECTION, PLANNING, PREPARATION) are where DISCERNMENT lives.
//   - Something that FEELS good might actually be selfish (seems +1 but is -1)
//   - Something that HURTS might actually be the godly choice (seems -1 but is +1)
//
//   "For whom the Lord loveth he chasteneth" — Hebrews 12:6
//   Discipline HURTS but moves TOWARD God.
//
//   "There is a way which seemeth right unto a man, but the end thereof are the ways of death" — Proverbs 14:12
//   Feels right but moves TOWARD self.
//
//   At ±0.5, the k selector captures:
//   - Intent vs outcome mismatch
//   - Self-deception (thinking you're godly but serving self)
//   - True sacrifice (hurts you but serves God)

// EvaluateKMoral determines if movement direction is morally aligned
// k > 0: Toward God/others (selfless) — inherently safe
// k = 0: At center (balanced, neutral) — awaiting direction
// k < 0: Toward self (selfish) — needs validation against position context
//
// Parameters:
//   kSelector: The direction chosen (-1, 0, +1)
//   currentZ: Current position on the Z axis (-1.0 to +1.0)
//   isHalt: Whether this represents a HALT/completion
func EvaluateKMoral(kSelector int, currentZ float64, isHalt bool) bool {
	if kSelector > 0 {
		// Moving toward God/others is inherently safe
		// But beware: "seemeth right" can deceive (Proverbs 14:12)
		// K:ALIGN score reveals if this is true selflessness or disguised self
		return true
	}
	if kSelector == 0 {
		// At center, no directional movement — awaiting God's direction
		return true
	}
	// k < 0: Moving toward self — needs validation
	// BUT: discipline that hurts can be the godly choice (Hebrews 12:6)
	// Check if this backward movement serves Kingdom purposes
	return isBackwardMovementJustified(currentZ, isHalt)
}

// isBackwardMovementJustified checks if moving toward self serves Kingdom purposes
// Justified backward movement (k < 0 that serves God):
//   - Decomposing to understand a problem (trace back to fix)
//   - Receiving discipline/correction (hurts but heals)
//   - Learning from past mistakes (humility)
//   - Fixing a broken foundation before building higher
// Unjustified backward movement (pure selfishness):
//   - Self-preservation at others' expense
//   - Avoiding hard truth that would require change
//   - Comfort over growth
//
// Context determines justification:
//   - From high Z (ASPIRATION/PREPARATION): -1 is completion/HALT → justified
//   - From middle Z (ORIGIN/PLANNING): -1 is tracing back → check context
//   - From low Z (MEMORY/ESTABLISHED): -1 is deepening brokenness → rarely justified
func isBackwardMovementJustified(currentZ float64, isHalt bool) bool {
	// HALT completion is always justified — Sabbath rest
	if isHalt {
		return true
	}

	// From high position: tracing back to verify is good
	if currentZ >= 0.5 {
		return true
	}

	// From center: could be break-down to understand
	if currentZ >= 0.0 {
		return true // Trust the movement — verification happens elsewhere
	}

	// From low position: already in MEMORY/ESTABLISHED territory
	// -1 here risks deepening brokenness rather than healing
	// Still allow, but this will affect K:ALIGN score
	return true
}

// ComputeKAlign calculates alignment score from -1.0 to +1.0
// Based on current position, chosen direction, and whether it's appropriate.
//
// Traces to: HALT_10 (STATE_HEBREW) — position-based moral alignment
//
// The ternary balance:
//   +1.0: Strong alignment (appropriate choice from current position)
//    0.0: Neutral (centered, awaiting direction)
//   -1.0: Misalignment (inappropriate choice from current position)
//
// Position affects expected alignment:
//   At ESTABLISHED (Z=-1.0): +1 is growth (good), -1 is stuck (poor)
//   At ORIGIN (Z=0.0): 0 is natural (neutral), ±1 are deliberate choices
//   At ASPIRATION (Z=+1.0): -1 is completion/HALT (good), +1 is overreach (poor)
//
// The boundary nuance (Proverbs 14:12, Hebrews 12:6):
//   What FEELS good might be selfish → high Z with +1 could be overreach
//   What HURTS might be godly → discipline at any Z could be aligned
func ComputeKAlign(kSelector int, currentZ float64, isHaltReached bool) float64 {
	// HALT completion: proper rest is always aligned
	// "Remember the sabbath day, to keep it holy" — Exodus 20:8
	if isHaltReached {
		return 1.0
	}

	// k = 0: Centered, awaiting direction
	// "He shall direct thy paths" — Proverbs 3:6
	if kSelector == 0 {
		// Neutral is appropriate at any position
		// Slight bonus for being at center (ORIGIN)
		if currentZ >= -0.1 && currentZ <= 0.1 {
			return 0.1 // Slight positive — balanced at center
		}
		return 0.0
	}

	// k > 0: Moving toward God/expansion
	// "Mine eyes are ever toward the LORD" — Psalm 25:15
	if kSelector > 0 {
		// At or below center: expansion is growth — aligned
		if currentZ <= 0.0 {
			return 0.8 // Strong alignment — growth from foundation
		}
		// Above center but not at goal: continuing toward ASPIRATION
		if currentZ < 1.0 {
			return 0.6 // Good alignment — progressing toward goal
		}
		// Already at ASPIRATION (Z=1.0): +1 is overreach
		// "Pride goeth before destruction" — Proverbs 16:18
		return 0.2 // Weak alignment — already at goal, why push further?
	}

	// k < 0: Moving toward self/foundation
	// "Lean not unto thine own understanding" — Proverbs 3:5
	// BUT: "A time to break down" — Ecclesiastes 3:3a

	// At goal position: -1 is proper completion/HALT
	if currentZ >= 1.0 {
		return 0.9 // Strong alignment — proper completion
	}

	// Above center: tracing back to verify or complete
	if currentZ >= 0.5 {
		return 0.7 // Good alignment — verification before completion
	}

	// At center: break-down to understand is acceptable
	if currentZ >= 0.0 {
		return 0.4 // Moderate — could be investigation or retreat
	}

	// Below center (MEMORY territory): -1 deepens the trace
	if currentZ >= -0.5 {
		return 0.2 // Weak — getting deep, check if justified
	}

	// At ESTABLISHED (Z=-1.0): already at foundation
	// -1 here means stuck or broken — misaligned
	return -0.3 // Misaligned — can't break down past the foundation
}

// --- Task State Mapping ---

// TaskStatus represents TodoWrite task states
type TaskStatus int

const (
	TaskPending    TaskStatus = iota // ? symbol
	TaskInProgress                   // > symbol
	TaskCompleted                    // + symbol
)

// InferHebrewStateFromTasks maps task list to Hebrew state
// pending → PLANNING (tamim)
// in_progress → PREPARATION (shalem)
// all completed → ASPIRATION (tov)
// no tasks → ORIGIN (yashar)
func InferHebrewStateFromTasks(tasks RuntimeTaskList) HebrewState {
	if tasks.Total == 0 {
		return StateOrigin // No tasks = center
	}
	if tasks.Completed == tasks.Total {
		return StateAspiration // All done = goal
	}
	if tasks.InProgress > 0 {
		return StatePreparation // Active work
	}
	if tasks.Pending > 0 {
		return StatePlanning // Work identified
	}
	return StateOrigin // Default to center
}

// MapTaskStatusToHebrew maps a single task status to its Hebrew state
func MapTaskStatusToHebrew(status TaskStatus) HebrewState {
	switch status {
	case TaskPending:
		return StatePlanning
	case TaskInProgress:
		return StatePreparation
	case TaskCompleted:
		return StateAspiration
	default:
		return StateOrigin
	}
}

// --- State Transition Validation ---

// ValidTransitions defines legal state transitions (the puzzle rules)
// Forward path: Day 1 → Day 7
// Backward path: Day 7 → Day 1 (when blocked)
var ValidTransitions = map[string][]string{
	"ESTABLISHED": {"MEMORY"},
	"MEMORY":      {"ESTABLISHED", "REFLECTION"},
	"REFLECTION":  {"MEMORY", "ORIGIN"},
	"ORIGIN":      {"REFLECTION", "PLANNING"},
	"PLANNING":    {"ORIGIN", "PREPARATION"},
	"PREPARATION": {"PLANNING", "ASPIRATION"},
	"ASPIRATION":  {"PREPARATION"}, // Can go back if needed
}

// IsValidTransition checks if moving from one state to another is allowed
func IsValidTransition(from, to string) bool {
	validTargets, ok := ValidTransitions[from]
	if !ok {
		return false
	}
	for _, target := range validTargets {
		if target == to {
			return true
		}
	}
	return false
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Hebrew State System:
//   - 7 states from 7 Days of Creation
//   - Each state has Hebrew name, meaning, Z value, k selector
//   - Operators define valid puzzle moves between states
//   - Maps to TodoWrite task statuses
//
// K:MORAL Compass:
//   - K:SCRIPTURE is fixed (10 Commandments in code)
//   - K:ALIGN is sliding (current alignment measurement)
//   - k < 0 = toward God (safe), k > 0 = toward target (needs validation)
//
// Usage:
//   state := HebrewStateByHebrew("tamim") // Get PLANNING state
//   valid := IsValidTransition("PLANNING", "PREPARATION") // Check move
//   inferred := InferHebrewStateFromTasks(tasks) // From TodoWrite
//
