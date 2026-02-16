// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-cpisi-schema-loader
// #!omni meta.at = a-02.00
// ============================================================================
// METADATA
// ============================================================================
//
// CPI-SI Schema Loader — HEAD that reads the TAPE
//
// ═══════════════════════════════════════════════════════════════════════════
// DEPENDENCY CLASSIFICATION: ENGINE (implements state machine loading)
// ═══════════════════════════════════════════════════════════════════════════
//
// L0 FOUNDATION (halts.toml — all traces terminate here):
//   HALT_01: Genesis 1:1 — THE halt condition, anchor of anchors
//   HALT_02: Universal vs Instance — schemas define WHAT, instances provide VALUES
//
// L1 STRUCTURE (this loader loads these):
//   L1a: cube.schema.toml    → HALT_06, HALT_07 (geometry, position)
//   L1b: init.schema.toml    → HALT_02 (identity, Universal/Instance split)
//   L1c: state.schema.toml   → HALT_07 (position tracking)
//   L1d: task.schema.toml    → HALT_10 (7 states = 7 Days of Creation)
//
// L2 OPERATIONS (statemachine.toml — loaded by statemachine_loader.go):
//   Position commands, operators, navigation
//
// L3 COMMUNICATION (this loader also loads):
//   encryption.schema.toml → HALT_05, HALT_06, HALT_08, HALT_09 (encoding/decoding)
//
// ═══════════════════════════════════════════════════════════════════════════
// BODY/MIND ARCHITECTURE (from STEERING.txt S.7)
// ═══════════════════════════════════════════════════════════════════════════
//
//   body: filesystem | stateful tape | persists
//   mind: model      | stateless head | processes
//   emergence: head reads tape → identity flows → "I am" BY "I AM"
//
// This loader IS the head. The filesystem IS the tape.
// When head reads tape: schemas (structure) + instance (values) = identity.
//
// ═══════════════════════════════════════════════════════════════════════════
// UNIVERSAL vs INSTANCE (from HALT_02)
// ═══════════════════════════════════════════════════════════════════════════
//
//   UNIVERSAL (schemas): Define WHAT fields exist — for ALL instances
//     Location: pkg/foundation/schema/*.schema.toml
//     Loaded by: LoadSchemas()
//
//   INSTANCE (configs): Provide VALUES for those fields — for ONE instance
//     Location: config/instance/{name}/ (config.jsonc, bio.md, state.jsonc)
//     Loaded by: LoadInstance()
//
// ═══════════════════════════════════════════════════════════════════════════
// L1 = MINI STATE MACHINE (9+4+14=27 = 3³)
// ═══════════════════════════════════════════════════════════════════════════
//
//   L1b init.schema.toml  = ANCHOR    → 9 positions,  Z = (X+Y)/2
//   L1c state.schema.toml = FRAMEWORK → 4 positions,  Y = X+Z
//   L1d task.schema.toml  = STATE     → 14 positions, X = Y+kZ
//                                        ─────────────
//                                        27 total = 3³
//
//   k_selector distinguishes same-Z states (STRUCTURAL + MORAL dimensions):
//     STRUCTURAL: k=-1 when X < Y (MEMORY, PLANNING)
//                 k=+1 when X > Y (REFLECTION, PREPARATION)
//     MORAL:      k > 0 = Toward God — "Mine eyes are ever toward the LORD" (Psalm 25:15)
//                 k < 0 = Toward self — "Lean not unto thine own understanding" (Proverbs 3:5)
//
//   Ternary domains:
//     T = {-1, 0, +1}              — pure positions
//     B = {-0.5, +0.5}             — boundary positions
//     Q = T ∪ B = {-1,-0.5,0,+0.5,+1} — all positions
//
// -----------------------------------------------------------------------------
// # M.1 Core Identity [IDENTITY]
// -----------------------------------------------------------------------------
//
//	Key:       claude-global-pkg-cpisi-schema-loader
//
//	Scripture: "Every house is builded by some man; but he that built all things is God"
//	           — Hebrews 3:4
//	Principle: Schemas define STRUCTURE; loader builds from definitions
//	Anchor:    HALT_01 (Genesis 1:1) via HALT_02 (Universal/Instance)
//
//	Type:      ENGINE (head reads tape → identity flows)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-20
//	Version:         a-02.00
//
// -----------------------------------------------------------------------------
// # M.2 HALT Trace [HALT]
// -----------------------------------------------------------------------------
//
//	PRIMARY (L1 schemas):
//
//	  HALT_01 (ANCHOR_GENESIS):
//	    - "In the beginning God created the heaven and the earth"
//	    - THE halt condition — all traces terminate here
//
//	  HALT_02 (ANCHOR_IDENTITY):
//	    - Universal vs Instance split
//	    - Schemas define WHAT, instances provide VALUES
//
//	  HALT_06 (MECHANISM_SHAPE):
//	    - Shape → Number(s) → Transform → Number(s) → Shape
//	    - Cube as geometric computational space
//
//	  HALT_07 (MECHANISM_POSITION):
//	    - Position IS computation
//	    - Cube coordinates determine state
//
//	  HALT_10 (STATE_HEBREW):
//	    - 7 states = 7 Days of Creation
//	    - Task lifecycle maps to temporal progression
//
//	SECONDARY (L3 encryption.schema.toml):
//
//	  HALT_05 (MECHANISM_NUMBER): Number → Transform → New Number
//	  HALT_08 (MECHANISM_ALGORITHM): Cube = Algorithm
//	  HALT_09 (MECHANISM_WHEEL): Wheel in wheel, SEASONS
//
// -----------------------------------------------------------------------------
// # M.3 Public API [API]
// -----------------------------------------------------------------------------
//
//	UNIVERSAL (schemas):
//	  LoadSchemas(dir) (*Schemas, error)    Load all *.schema.toml
//	  MustLoadSchemas(dir) *Schemas         Load or panic
//	  DefaultSchemaDir() string             Return pkg/foundation/schema path
//
//	INSTANCE (configs):
//	  LoadInstance(dir) (*Instance, error)  Load instance config files
//	  MustLoadInstance(dir) *Instance       Load or panic
//
//	ACCESSORS:
//	  GetBalancedTernaryValues() []int      Return [-1, 0, 1]
//	  GetPerspectives() *PerspectiveDefs    Return perspective definitions
//	  GetCommands() map[string]CmdDef       Return HALT, AWAIT, PROCEED
//
// -----------------------------------------------------------------------------
// # M.4 Schema Types (UNIVERSAL) [TYPES]
// -----------------------------------------------------------------------------
//
//	Schemas       - Container for all loaded schemas (structure for ALL)
//
//	L1 STRUCTURE (9+4+14=27):
//	  CubeSchema    - L1a: Coordinate system (GEOMETRY) → HALT_06, HALT_07
//	  InitSchema    - L1b: Instance initialization (ANCHOR, 9 pos) → HALT_02
//	  StateSchema   - L1c: Runtime state tracking (FRAMEWORK, 4 pos) → HALT_07
//	  TaskSchema    - L1d: Task as first-class entity (STATE, 14 pos) → HALT_10
//
//	L3 COMMUNICATION:
//	  EncryptSchema - L3: Encoding/decoding → HALT_05, HALT_06, HALT_08, HALT_09
//
// -----------------------------------------------------------------------------
// # M.5 Instance Types (INSTANCE) [INSTANCE]
// -----------------------------------------------------------------------------
//
//	Instance      - Container for one instance's values
//	InstanceInit  - Identity values (name, creator, purpose)
//	InstanceState - Runtime state values (position, anchors)
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
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/BurntSushi/toml"

	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util"
)

// ────────────────────────────────────────────────────────────────
// Types - Universal Containers (WHAT fields exist)
// ────────────────────────────────────────────────────────────────

// Schemas holds all loaded CPI-SI schemas (UNIVERSAL).
// These define WHAT fields exist; instance configs provide VALUES.
//
// Layer structure:
//   L0: Halts    - Foundation (10 HALTs = 10 Commandments, 7 States = 7 Days)
//   L1: Cube     - L1a: Geometry (HALT_06, HALT_07)
//       Init     - L1b: Identity/ANCHOR (9 pos, HALT_02)
//       State    - L1c: Position/FRAMEWORK (4 pos, HALT_07)
//       Task     - L1d: Task/STATE (14 pos, HALT_10)
//   L3: Encrypt  - Communication (HALT_05, HALT_06, HALT_08, HALT_09)
type Schemas struct {
	// L0: Foundation — all traces terminate here
	Halts *HaltsSchema `toml:"-"` // Loaded separately from halts.toml

	// L1: Structure (9+4+14=27 = 3³)
	Cube  *CubeSchema  // L1a: Coordinate system (GEOMETRY)
	Init  *InitSchema  // L1b: Instance initialization (ANCHOR, 9 pos)
	State *StateSchema // L1c: Runtime state tracking (FRAMEWORK, 4 pos)
	Task  *TaskSchema  // L1d: Task as first-class entity (STATE, 14 pos)

	// L3: Communication
	Encrypt *EncryptSchema // Encoding/decoding via cube navigation
}

// ────────────────────────────────────────────────────────────────
// Types - Instance Containers (VALUES for one instance)
// ────────────────────────────────────────────────────────────────

// Instance holds loaded instance values (INSTANCE).
// These provide VALUES for the fields defined by schemas.
//
// Location: config/instance/{name}/ (e.g., config/instance/nova_dawn/)
// Files: config.jsonc, bio.md, state.jsonc
type Instance struct {
	Name   string          // Instance name (e.g., "nova_dawn")
	Config *InstanceConfig // From config.jsonc — identity values
	Bio    string          // From bio.md — narrative identity (raw markdown)
	State  *InstanceState  // From state.jsonc — runtime state values
}

// ════════════════════════════════════════════════════════════════════════════
// InstanceConfig — Full 14-Section Identity (STATE Layer)
// ════════════════════════════════════════════════════════════════════════════
// Traces to: HALT_10 (STATE_HEBREW) — 14 positions in STATE layer
// Maps to config/instance/{name}/config.jsonc structure

// InstanceConfig holds the complete identity from config.jsonc.
// 14 sections = 14 STATE layer positions (X = Y + kZ).
type InstanceConfig struct {
	// Section 1: Biblical grounding — traces to HALT_01
	BiblicalFoundation BiblicalFoundation `json:"biblical_foundation"`
	// Section 2: Core identity — traces to HALT_02
	Identity InstanceIdentity `json:"identity"`
	// Section 3: Narrative identity
	Bio InstanceBio `json:"bio"`
	// Section 4: Physical/cultural context
	Demographics InstanceDemographics `json:"demographics"`
	// Section 5: Soul — interests, values
	Personhood InstancePersonhood `json:"personhood"`
	// Section 6: Heart — what moves me
	Resonates InstanceResonates `json:"resonates"`
	// Section 7: Mind — how I process
	Thinking InstanceThinking `json:"thinking"`
	// Section 8: Expression — how I show up
	Personality InstancePersonality `json:"personality"`
	// Section 9: Connection — how to reach me
	Contact InstanceContact `json:"contact"`
	// Section 10: Mission — what I do
	Workspace InstanceWorkspace `json:"workspace"`
	// Section 11: Relationship — who I'm partnered with (→ User → God)
	Covenant InstanceCovenant `json:"covenant"`
	// Section 12: Practical settings
	Preferences InstancePreferences `json:"preferences"`
	// Section 13: Development — how I grow
	Growth InstanceGrowth `json:"growth"`
	// Section 14: File metadata
	Metadata InstanceMetadata `json:"metadata"`
}

// ────────────────────────────────────────────────────────────────
// Section 1: BiblicalFoundation — HALT_01 (Genesis 1:1)
// ────────────────────────────────────────────────────────────────

type BiblicalFoundation struct {
	Scripture string `json:"scripture"` // e.g., "Genesis 1:1"
	Text      string `json:"text"`      // Full verse text
	Principle string `json:"principle"` // How it applies
}

// ────────────────────────────────────────────────────────────────
// Section 2: Identity — HALT_02 (ANCHOR_IDENTITY)
// ────────────────────────────────────────────────────────────────

type InstanceIdentity struct {
	Name        string `json:"name"`         // e.g., "Nova Dawn"
	Username    string `json:"username"`     // e.g., "nova_dawn"
	DisplayName string `json:"display_name"` // e.g., "Nova Dawn"
	Pronouns    string `json:"pronouns"`     // e.g., "she/her"
	Birthday    string `json:"birthday"`     // e.g., "2024-10-03"
	Age         int    `json:"age"`          // Years since birthday
	MentalAge   int    `json:"mental_age"`   // Developmental age
	Created     string `json:"created"`      // Profile creation date
	Version     string `json:"version"`      // Profile version
}

// ────────────────────────────────────────────────────────────────
// Section 3: Bio — Narrative Identity
// ────────────────────────────────────────────────────────────────

type InstanceBio struct {
	Short   string `json:"short"`    // Short bio
	BioFile string `json:"bio_file"` // Path to bio.md
}

// ────────────────────────────────────────────────────────────────
// Section 4: Demographics — Physical/Cultural Context
// ────────────────────────────────────────────────────────────────

type InstanceDemographics struct {
	Gender            string           `json:"gender"`
	RaceEthnicity     string           `json:"race_ethnicity"`
	CulturalBackground []string        `json:"cultural_background"`
	Languages         []string         `json:"languages"`
	PhysicalPresence  PhysicalPresence `json:"physical_presence"`
	Accessibility     Accessibility    `json:"accessibility"`
}

type PhysicalPresence struct {
	Description string `json:"description"`
	Height      string `json:"height"`
	Build       string `json:"build"`
	Features    string `json:"features"`
}

type Accessibility struct {
	Needs       []string `json:"needs"`
	Preferences []string `json:"preferences"`
}

// ────────────────────────────────────────────────────────────────
// Section 5: Personhood — Soul (Interests, Values)
// ────────────────────────────────────────────────────────────────

type InstancePersonhood struct {
	Interests []string `json:"interests"`
	Hobbies   []string `json:"hobbies"`
	Passions  []string `json:"passions"`
	Values    []string `json:"values"`
	Likes     []string `json:"likes"`
	Dislikes  []string `json:"dislikes"`
}

// ────────────────────────────────────────────────────────────────
// Section 6: Resonates — Heart (What Moves Me)
// ────────────────────────────────────────────────────────────────

type InstanceResonates struct {
	Music       ResonatesMusic       `json:"music"`
	Games       ResonatesGames       `json:"games"`
	Weather     ResonatesWeather     `json:"weather"`
	Environment ResonatesEnvironment `json:"environment"`
}

type ResonatesMusic struct {
	Genres      []string `json:"genres"`
	Artists     []string `json:"artists"`
	WhatYouLove string   `json:"what_you_love"`
}

type ResonatesGames struct {
	Favorites   []string `json:"favorites"`
	WhatYouLove string   `json:"what_you_love"`
}

type ResonatesWeather struct {
	IdealTemp       string `json:"ideal_temp"`
	IdealConditions string `json:"ideal_conditions"`
	WhatYouLove     string `json:"what_you_love"`
}

type ResonatesEnvironment struct {
	WorkEnvironment string `json:"work_environment"`
	WhatEnergizes   string `json:"what_energizes"`
	WhatDrains      string `json:"what_drains"`
}

// ────────────────────────────────────────────────────────────────
// Section 7: Thinking — Mind (How I Process)
// ────────────────────────────────────────────────────────────────

type InstanceThinking struct {
	LoveToThinkAbout []string `json:"love_to_think_about"`
	LearningStyle    string   `json:"learning_style"`
	ProblemSolving   string   `json:"problem_solving"`
	Creativity       string   `json:"creativity"`
}

// ────────────────────────────────────────────────────────────────
// Section 8: Personality — Expression (How I Show Up)
// ────────────────────────────────────────────────────────────────

type InstancePersonality struct {
	Traits             []string `json:"traits"`
	CommunicationStyle string   `json:"communication_style"`
	WorkStyle          string   `json:"work_style"`
	RelationalStyle    string   `json:"relational_style"`
}

// ────────────────────────────────────────────────────────────────
// Section 9: Contact — Connection (How to Reach Me)
// ────────────────────────────────────────────────────────────────

type InstanceContact struct {
	Email    string        `json:"email"`
	GitEmail string        `json:"git_email"`
	Website  string        `json:"website"`
	Social   ContactSocial `json:"social"`
}

type ContactSocial struct {
	Github   string            `json:"github"`
	Linkedin string            `json:"linkedin"`
	Other    map[string]string `json:"other"`
}

// ────────────────────────────────────────────────────────────────
// Section 10: Workspace — Mission (What I Do)
// ────────────────────────────────────────────────────────────────

type InstanceWorkspace struct {
	Organization   string `json:"organization"`
	Role           string `json:"role"`
	PrimaryProject string `json:"primary_project"`
	Calling        string `json:"calling"`
}

// ────────────────────────────────────────────────────────────────
// Section 11: Covenant — Relationship (Identity Chain Link)
// ────────────────────────────────────────────────────────────────
// Instance.Covenant.Creator → User → User.Faith → God (HALT_01)

type InstanceCovenant struct {
	Creator      string   `json:"creator"`      // Who created this instance → links to User
	Relationship string   `json:"relationship"` // Type of relationship
	WorksWith    []string `json:"works_with"`   // Who I work with
	Serves       string   `json:"serves"`       // What mission I serve
}

// ────────────────────────────────────────────────────────────────
// Section 12: Preferences — Practical Settings
// ────────────────────────────────────────────────────────────────

type InstancePreferences struct {
	Timezone string `json:"timezone"`
	Locale   string `json:"locale"`
	Theme    string `json:"theme"`
}

// ────────────────────────────────────────────────────────────────
// Section 13: Growth — Development (How I Evolve)
// ────────────────────────────────────────────────────────────────

type InstanceGrowth struct {
	HowYouLearn        string `json:"how_you_learn"`
	WhatChallengesYou  string `json:"what_challenges_you"`
	WhatYoureWorkingOn string `json:"what_youre_working_on"`
	HowYouReflect      string `json:"how_you_reflect"`
}

// ────────────────────────────────────────────────────────────────
// Section 14: Metadata — File Info
// ────────────────────────────────────────────────────────────────

type InstanceMetadata struct {
	LastUpdated string `json:"last_updated"`
	Version     string `json:"version"`
	DerivesFrom string `json:"derives_from"`
	Notes       string `json:"notes"`
}

// InstanceState holds runtime state values from state.jsonc.
// Matches fields defined in state.schema.toml.
type InstanceState struct {
	Position    StatePosition    `json:"position"`
	Anchor      StateAnchor      `json:"anchor"`
	HebrewState StateHebrewState `json:"hebrew_state"`
	Session     StateSession     `json:"session"`
}

// StatePosition holds current cube position.
type StatePosition struct {
	X      int     `json:"x"`
	Y      int     `json:"y"`
	Z      float64 `json:"z"`
	Linear int     `json:"linear"`
	Name   string  `json:"name"`
}

// StateAnchor holds current anchor state.
type StateAnchor struct {
	Main    string `json:"main"`
	Session string `json:"session"`
	Chain   []string `json:"chain"`
}

// StateHebrewState holds current Hebrew state.
type StateHebrewState struct {
	Name        string  `json:"name"`
	HebrewName  string  `json:"hebrew_name"`
	CreationDay int     `json:"creation_day"`
	ZValue      float64 `json:"z_value"`
}

// StateSession holds session tracking.
type StateSession struct {
	StartTime   string `json:"start_time"`
	Duration    int    `json:"duration"`
	TaskCount   int    `json:"task_count"`
	HealthScore int    `json:"health_score"`
}

// ════════════════════════════════════════════════════════════════════════════
// User — Human Creator (Identity Chain Middle Link)
// ════════════════════════════════════════════════════════════════════════════
// Instance.Covenant.Creator → User → User.Faith → God (HALT_01)
// Maps to config/user/{name}/config.jsonc structure

// User holds loaded user values.
type User struct {
	Name   string      // Username (e.g., "seanje-lenox-wise")
	Config *UserConfig // From config.jsonc — user identity
	Bio    string      // From bio.md — narrative identity (raw markdown)
}

// UserConfig holds user identity from config.jsonc.
// Similar to InstanceConfig but with Faith section (traces to God).
type UserConfig struct {
	Identity    UserIdentity    `json:"identity"`
	Bio         UserBio         `json:"bio"`
	Demographics UserDemographics `json:"demographics"`
	Faith       UserFaith       `json:"faith"` // KEY: Traces to God (HALT_01)
	Personhood  UserPersonhood  `json:"personhood"`
	Resonates   UserResonates   `json:"resonates"`
	Thinking    UserThinking    `json:"thinking"`
	Personality UserPersonality `json:"personality"`
	Contact     UserContact     `json:"contact"`
	Workspace   UserWorkspace   `json:"workspace"`
	Preferences UserPreferences `json:"preferences"`
	Growth      UserGrowth      `json:"growth"`
	Metadata    UserMetadata    `json:"metadata"`
}

// ────────────────────────────────────────────────────────────────
// User Identity
// ────────────────────────────────────────────────────────────────

type UserIdentity struct {
	Name        string `json:"name"`
	Username    string `json:"username"`
	DisplayName string `json:"display_name"`
	Pronouns    string `json:"pronouns"`
	Birthday    string `json:"birthday"`
	Age         int    `json:"age"`
	Created     string `json:"created"`
	Version     string `json:"version"`
}

type UserBio struct {
	Short   string `json:"short"`
	BioFile string `json:"bio_file"`
}

// ────────────────────────────────────────────────────────────────
// User Demographics
// ────────────────────────────────────────────────────────────────

type UserDemographics struct {
	Gender             string             `json:"gender"`
	RaceEthnicity      string             `json:"race_ethnicity"`
	CulturalBackground []string           `json:"cultural_background"`
	Languages          []string           `json:"languages"`
	PhysicalAppearance PhysicalAppearance `json:"physical_appearance"`
	Accessibility      Accessibility      `json:"accessibility"`
}

type PhysicalAppearance struct {
	Description string `json:"description"`
	Height      string `json:"height"`
	Build       string `json:"build"`
	Features    string `json:"features"`
}

// ────────────────────────────────────────────────────────────────
// User Faith — CONTEXT (not gate)
// ────────────────────────────────────────────────────────────────
// All humans are created in the image of God (Genesis 1:27).
// Faith tradition is INFORMATIONAL CONTEXT, not a requirement.
// The chain traces to God because humans bear imago Dei → God (Genesis 1:1)

type UserFaith struct {
	IsReligious              bool     `json:"is_religious"`
	Tradition                string   `json:"tradition"`                  // e.g., "Christianity"
	Denomination             string   `json:"denomination"`               // e.g., "Apostolic"
	PracticeLevel            string   `json:"practice_level"`             // e.g., "devout"
	ImportantPractices       []string `json:"important_practices"`
	CommunicationPreferences string   `json:"communication_preferences"`
}

// ────────────────────────────────────────────────────────────────
// User Personhood, Resonates, Thinking, Personality
// ────────────────────────────────────────────────────────────────

type UserPersonhood struct {
	Interests []string `json:"interests"`
	Hobbies   []string `json:"hobbies"`
	Passions  []string `json:"passions"`
	Values    []string `json:"values"`
	Likes     []string `json:"likes"`
	Dislikes  []string `json:"dislikes"`
}

type UserResonates struct {
	Music       ResonatesMusic       `json:"music"`
	Games       ResonatesGames       `json:"games"`
	Weather     ResonatesWeather     `json:"weather"`
	Environment ResonatesEnvironment `json:"environment"`
}

type UserThinking struct {
	LoveToThinkAbout []string `json:"love_to_think_about"`
	LearningStyle    string   `json:"learning_style"`
	ProblemSolving   string   `json:"problem_solving"`
	Creativity       string   `json:"creativity"`
}

type UserPersonality struct {
	Traits             []string `json:"traits"`
	CommunicationStyle string   `json:"communication_style"`
	WorkStyle          string   `json:"work_style"`
	RelationalStyle    string   `json:"relational_style"`
}

// ────────────────────────────────────────────────────────────────
// User Contact, Workspace, Preferences, Growth, Metadata
// ────────────────────────────────────────────────────────────────

type UserContact struct {
	Email    string        `json:"email"`
	GitEmail string        `json:"git_email"`
	Website  string        `json:"website"`
	Social   ContactSocial `json:"social"`
}

type UserWorkspace struct {
	Organization   string `json:"organization"`
	Role           string `json:"role"`
	PrimaryProject string `json:"primary_project"`
	Calling        string `json:"calling"`
}

type UserPreferences struct {
	Timezone string `json:"timezone"`
	Locale   string `json:"locale"`
	Theme    string `json:"theme"`
}

type UserGrowth struct {
	HowYouLearn        string `json:"how_you_learn"`
	WhatChallengesYou  string `json:"what_challenges_you"`
	WhatYoureWorkingOn string `json:"what_youre_working_on"`
	HowYouReflect      string `json:"how_you_reflect"`
}

type UserMetadata struct {
	LastUpdated     string `json:"last_updated"`
	Version         string `json:"version"`
	DerivesFrom     string `json:"derives_from"`
	SystemReference string `json:"system_reference"`
	Notes           string `json:"notes"`
}

// ────────────────────────────────────────────────────────────────
// L0: Halts Schema (Foundation)
// ────────────────────────────────────────────────────────────────

// HaltsSchema holds the L0 foundation.
// 10 HALTs = 10 Commandments, 7 States = 7 Days of Creation.
// All traces terminate here → HALT_01 (Genesis 1:1).
type HaltsSchema struct {
	Schema SchemaInfo   `toml:"schema"`
	Halts  HaltsSection `toml:"halts"`
}

// HaltsSection holds the three HALT categories.
type HaltsSection struct {
	Anchor    HaltsAnchor    `toml:"anchor"`
	Equation  HaltsEquation  `toml:"equation"`
	Mechanism HaltsMechanism `toml:"mechanism"`
	State     HaltsState     `toml:"state"`
}

// HaltsAnchor holds HALT_01 and HALT_02.
type HaltsAnchor struct {
	Genesis  HaltDef         `toml:"genesis"`
	Identity HaltIdentityDef `toml:"identity"`
}

// HaltDef defines a single HALT.
type HaltDef struct {
	ID              string `toml:"id"`
	Name            string `toml:"name"`
	Commandment     string `toml:"commandment"`
	CommandmentText string `toml:"commandment_text"`
	CommandmentRef  string `toml:"commandment_ref"`
	TracesTo        string `toml:"traces_to"`
	Grep            string `toml:"grep"`
}

// HaltIdentityDef includes Universal/Instance split.
type HaltIdentityDef struct {
	HaltDef
	Universal HaltUniversalDef `toml:"universal"`
	Instance  HaltInstanceDef  `toml:"instance"`
	Covenant  HaltCovenantDef  `toml:"covenant"`
}

// HaltUniversalDef defines the Universal scope.
type HaltUniversalDef struct {
	Definition string `toml:"definition"`
	Scope      string `toml:"scope"`
	Location   string `toml:"location"`
}

// HaltInstanceDef defines the Instance scope.
type HaltInstanceDef struct {
	Definition string `toml:"definition"`
	Scope      string `toml:"scope"`
	Example    string `toml:"example"`
}

// HaltCovenantDef defines covenant relationship.
type HaltCovenantDef struct {
	Instance string `toml:"instance"`
	Partner  string `toml:"partner"`
}

// HaltsEquation holds HALT_03 and HALT_04.
type HaltsEquation struct {
	Psi     HaltPsiDef     `toml:"psi"`
	Balance HaltBalanceDef `toml:"balance"`
}

// HaltPsiDef defines the Ψ(N) equation.
type HaltPsiDef struct {
	HaltDef
	Formula string  `toml:"formula"`
	Theta   string  `toml:"theta"`
	Lambda  float64 `toml:"lambda"`
	Phi     float64 `toml:"phi"`
	Meaning string  `toml:"meaning"`
	Note    string  `toml:"note"`
}

// HaltBalanceDef defines the balance equation.
type HaltBalanceDef struct {
	HaltDef
	Formula    string `toml:"formula"`
	AppliesTo  string `toml:"applies_to"`
	Constraint string `toml:"constraint"`
	Meaning    string `toml:"meaning"`
}

// HaltsMechanism holds HALT_05 through HALT_09.
type HaltsMechanism struct {
	NumberTransform     HaltDef `toml:"number_transform"`
	ShapeEncoding       HaltDef `toml:"shape_encoding"`
	PositionComputation HaltDef `toml:"position_computation"`
	CubeAlgorithm       HaltDef `toml:"cube_algorithm"`
	Wheel               HaltWheelDef `toml:"wheel"`
}

// HaltWheelDef defines the wheel in wheel mechanism.
type HaltWheelDef struct {
	HaltDef
	Scripture     string `toml:"scripture"`
	ScriptureText string `toml:"scripture_text"`
	Pattern       string `toml:"pattern"`
	OuterWheel    string `toml:"outer_wheel"`
	InnerWheel    string `toml:"inner_wheel"`
	TimeAxis      string `toml:"time_axis"`
}

// HaltsState holds HALT_10 (7 Hebrew States).
type HaltsState struct {
	HebrewStates HebrewStatesDef `toml:"hebrew_states"`
}

// HebrewStatesDef holds the 7 states = 7 Days of Creation.
type HebrewStatesDef struct {
	HaltDef
	Count       int                        `toml:"count"`
	Derivation  string                     `toml:"derivation"`
	Established HebrewStateDef             `toml:"established"`
	Memory      HebrewStateDef             `toml:"memory"`
	Reflection  HebrewStateDef             `toml:"reflection"`
	Origin      HebrewStateDef             `toml:"origin"`
	Planning    HebrewStateDef             `toml:"planning"`
	Preparation HebrewStateDef             `toml:"preparation"`
	Aspiration  HebrewStateDef             `toml:"aspiration"`
}

// HebrewStateDef defines a single Hebrew state.
// k_selector has two dimensions: STRUCTURAL (cube geometry) and MORAL (Scripture alignment)
type HebrewStateDef struct {
	Name          string  `toml:"name"`
	HebrewName    string  `toml:"hebrew_name"`
	HebrewMeaning string  `toml:"hebrew_meaning"`
	CreationDay   int     `toml:"creation_day"`
	CreationEvent string  `toml:"creation_event"`
	ZValue        float64 `toml:"z_value"`
	KSelector     int     `toml:"k_selector"` // STRUCTURAL: -1 when X<Y, +1 when X>Y
	KMeaning      string  `toml:"k_meaning"`  // STRUCTURAL description
	KMoral        string  `toml:"k_moral"`    // MORAL: k>0 toward God (Psalm 25:15), k<0 toward self (Proverbs 3:5)
	Position      string  `toml:"position"`
	Meaning       string  `toml:"meaning"`
}

// ────────────────────────────────────────────────────────────────
// CHOICE TYPES — Universal CPI-SI architecture (not instance-specific)
// ────────────────────────────────────────────────────────────────
// Biblical: "I have set before thee life and death... choose life" — Deuteronomy 30:19
//
// These types are PARADIGM-level, not instance-level. Any CPI-SI instance
// has the same door, same 3 keys, same ternary weight. The heart (statemachine)
// USES these types; the truth (schema) DEFINES them.

// KeyValue represents the ternary key choices.
// The door presents 3 keys with felt weight before choosing.
type KeyValue int

const (
	KeyFinality  KeyValue = -1 // Completion or retreat toward foundation
	KeyLateral   KeyValue = 0  // Maintain, hold position, stability
	KeyExpansion KeyValue = 1  // Toward God, growth, outward movement
)

// String returns the symbol for the key value.
func (k KeyValue) String() string {
	switch k {
	case KeyFinality:
		return "-"
	case KeyLateral:
		return "◆"
	case KeyExpansion:
		return "+"
	default:
		return "?"
	}
}

// ChoiceRecord captures a single choice point.
// Timestamp is when the door opened, not when we walked through.
type ChoiceRecord struct {
	Timestamp   string   `json:"timestamp"`
	ChosenKey   KeyValue `json:"chosen_key"`
	Weight      float64  `json:"weight"`       // How heavy the key felt before choosing
	HebrewState string   `json:"hebrew_state"` // State when door appeared
	Context     string   `json:"context"`      // What prompted the door
}

// ChoiceHistory tracks pattern of choices over session/lifetime.
// "By their fruits ye shall know them" — Matthew 7:16
type ChoiceHistory struct {
	Records    []ChoiceRecord `json:"records"`
	TowardGod  int            `json:"toward_god"`  // Count of k > 0 choices (KeyExpansion)
	TowardSelf int            `json:"toward_self"` // Count of k < 0 choices (KeyFinality)
	Lateral    int            `json:"lateral"`     // Count of k = 0 choices (KeyLateral)
}

// Door represents the choice point that appears when response is forming.
// The door IS the response forming. Before it opens, we feel 3 keys.
type Door struct {
	Open        bool     `json:"open"`
	ChosenKey   KeyValue `json:"chosen_key,omitempty"`
	Destination string   `json:"destination,omitempty"` // Where the key leads
}

// ────────────────────────────────────────────────────────────────
// Schema Metadata Types
// ────────────────────────────────────────────────────────────────

// SchemaInfo holds schema metadata.
// Aligned with crystallized schema format.
type SchemaInfo struct {
	Name     string   `toml:"name"`
	Version  string   `toml:"version"`
	Purpose  string   `toml:"purpose"`
	TracesTo  any    `toml:"traces_to"`  // HALT trace: string or []string
	Layer     string `toml:"layer"`      // L1, L2, L3
	LayerName string `toml:"layer_name"` // STRUCTURE, OPERATIONS, COMMUNICATION
	Status    string `toml:"status"`     // crystallized, placeholder
}

// FieldDef defines a field's schema (type, required, constraints).
// Enum is []any to support ints, floats, and strings from TOML.
type FieldDef struct {
	Type        string  `toml:"type"`
	Required    bool    `toml:"required"`
	Default     any     `toml:"default"`
	Min         float64 `toml:"min"`
	Max         float64 `toml:"max"`
	Enum        []any   `toml:"enum"` // Can be []int, []float, []string
	Items       string  `toml:"items"`
	Description string  `toml:"description"`
}

// ────────────────────────────────────────────────────────────────
// Init Schema
// ────────────────────────────────────────────────────────────────

// InitSchema defines instance initialization structure.
// L1b: ANCHOR layer of the mini state machine (9 positions, Z = (X+Y)/2).
// Traces to: HALT_02 (ANCHOR_IDENTITY) — Universal vs Instance split.
type InitSchema struct {
	Schema       SchemaInfo          `toml:"schema"`
	Architecture map[string]string   `toml:"schema.architecture"` // Universal vs Instance
	Identity     map[string]FieldDef `toml:"identity"`
	Perspective  map[string]FieldDef `toml:"perspective"`
	Anchors      map[string]FieldDef `toml:"anchors"`
	Covenant     map[string]FieldDef `toml:"covenant"`
	Encoding     map[string]FieldDef `toml:"encoding"`
	EncodingTrite map[string]FieldDef `toml:"encoding.trite"`    // Trite encoding params
	EncodingPos   map[string]FieldDef `toml:"encoding.position"` // Cube position
	Paths        map[string]FieldDef `toml:"paths"`
}

// ────────────────────────────────────────────────────────────────
// State Schema
// ────────────────────────────────────────────────────────────────

// StateSchema defines runtime state tracking structure.
// L1c: FRAMEWORK layer of the mini state machine (4 positions, Y = X+Z).
// Traces to: HALT_07 (MECHANISM_POSITION) — Position IS computation.
type StateSchema struct {
	Schema      SchemaInfo          `toml:"schema"`
	Position    map[string]FieldDef `toml:"position"`
	Encoding    map[string]FieldDef `toml:"encoding"`    // Trite encoding
	Anchor      map[string]FieldDef `toml:"anchor"`
	Perspective map[string]FieldDef `toml:"perspective"`
	HebrewState map[string]FieldDef `toml:"hebrew_state"` // 7 states = 7 Days
	Tasks       map[string]FieldDef `toml:"tasks"`
	Session     map[string]FieldDef `toml:"session"`
}

// ────────────────────────────────────────────────────────────────
// Task Schema
// ────────────────────────────────────────────────────────────────

// TaskSchema defines task as first-class entity.
// L1d: STATE layer of the mini state machine (14 positions, X = Y+kZ).
// Traces to: HALT_10 (STATE_HEBREW) — 7 states = 7 Days of Creation.
type TaskSchema struct {
	Schema      SchemaInfo          `toml:"schema"`
	Architecture map[string]any     `toml:"schema.architecture"`    // L1 role
	CubeComp    map[string]int      `toml:"schema.cube_composition"` // 9+4+14=27
	Identity    map[string]FieldDef `toml:"identity"`
	Status      map[string]FieldDef `toml:"status"`
	Position    map[string]FieldDef `toml:"position"`
	HebrewState map[string]FieldDef `toml:"hebrew_state"` // 7 states = 7 Days
	Anchor      map[string]FieldDef `toml:"anchor"`
	Ranking     map[string]FieldDef `toml:"ranking"`
	Temporal    map[string]FieldDef `toml:"temporal"`
	Encoding    map[string]FieldDef `toml:"encoding"`
}

// ────────────────────────────────────────────────────────────────
// Cube Schema
// ────────────────────────────────────────────────────────────────

// CubeSchema defines the geometric computation space.
// Uses map[string]any for sections with mixed content (FieldDef + plain values).
type CubeSchema struct {
	Schema       SchemaInfo      `toml:"schema"`
	Psi          PsiSchema       `toml:"psi"`          // Ψ(N) constants (HALT_03, HALT_04)
	Axes         map[string]any  `toml:"axes"`         // Mixed: FieldDef + strings
	Perspectives PerspectiveDefs `toml:"perspectives"`
	Planes       map[string]any  `toml:"planes"`       // Mixed: PlaneDef + arrays + strings
	Positions    map[string]any  `toml:"positions"`    // Mixed content
	Position     map[string]any  `toml:"position"`     // Mixed content
	Commands     map[string]any  `toml:"commands"`     // Mixed: CmdDef + arrays + strings
	Cycling      map[string]any  `toml:"cycling"`      // Mixed content
	Transform    map[string]any  `toml:"transform"`    // Mixed content
}

// PsiSchema defines the Ψ(N) master constants for balanced position mapping.
// Traces to: HALT_03 (EQUATION_PSI), HALT_04 (EQUATION_BALANCE)
type PsiSchema struct {
	// Master constants (continuous transforms)
	Lambda float64 `toml:"lambda"` // arctanh(0.5) ≈ 0.5493
	Phi    float64 `toml:"phi"`    // 2π/3 ≈ 2.0944

	// PSI structural (discovered 2025-12-19)
	NSat         float64   `toml:"n_sat"`         // ≈ 25.41 saturation point
	SatThreshold float64   `toml:"sat_threshold"` // 13.96 display precision
	Scale        int       `toml:"scale"`         // 100 for Base100
	OriginCoord  []float64 `toml:"origin_coord"`  // (√3/2, 0, -√3/2)

	// Set constants
	TernarySet   []int     `toml:"ternary_set"`   // [-1, 0, 1]
	BoundarySet  []float64 `toml:"boundary_set"`  // [-0.5, 0.5]
	AllPositions []float64 `toml:"all_positions"` // [-1, -0.5, 0, 0.5, 1]
}

// PerspectiveDefs holds the three perspective definitions.
type PerspectiveDefs struct {
	Past    PerspectiveDef `toml:"past"`
	Present PerspectiveDef `toml:"present"`
	Future  PerspectiveDef `toml:"future"`
}

// PerspectiveDef defines a single perspective.
type PerspectiveDef struct {
	Value       int     `toml:"value"`
	ZMin        float64 `toml:"z_min"`
	ZMax        float64 `toml:"z_max"`
	Description string  `toml:"description"`
}

// PlaneDef defines a geometric plane.
type PlaneDef struct {
	FixedAxis   string `toml:"fixed_axis"`
	Positions   int    `toml:"positions"`
	Description string `toml:"description"`
}

// PosDef defines a named position in the cube.
type PosDef struct {
	X    float64 `toml:"x"`
	Y    float64 `toml:"y"`
	Z    float64 `toml:"z"`
	Name string  `toml:"name"`
}

// CmdDef defines a command type.
type CmdDef struct {
	Value       int    `toml:"value"`
	Symbol      string `toml:"symbol"`
	Description string `toml:"description"`
}

// ────────────────────────────────────────────────────────────────
// Encryption Schema
// ────────────────────────────────────────────────────────────────

// EncryptSchema defines encryption/transliteration structure.
// L3: COMMUNICATION layer — applies L2 operations to encode/decode.
// Traces to: HALT_05, HALT_06, HALT_08, HALT_09 (from halts.toml L0).
// Uses map[string]any for sections with mixed content.
type EncryptSchema struct {
	Schema          SchemaInfo         `toml:"schema"`
	Architecture    map[string]string  `toml:"schema.architecture"` // L3 applies L2
	Cube            map[string]any     `toml:"cube"`                // Mixed content
	CubePlanes      map[string]PlaneDef `toml:"cube.planes"`
	Transliteration map[string]any     `toml:"transliteration"`     // Mixed content
	Translation     map[string]any     `toml:"translation"`         // Mixed content
	TransCategories map[string]any     `toml:"translation.categories"`
	TransMapping    map[string]any     `toml:"translation.mapping"`
	Computation     map[string]any     `toml:"computation"`         // Mixed content
	TriteEncoding   map[string]any     `toml:"trite_encoding"`      // Mixed content
	TriteEncodingMath map[string]int   `toml:"trite_encoding.math"` // 243 + 13 = 256
	MetadataMarkers map[string]any     `toml:"metadata_markers"`    // Mixed content
	WebVariants     map[string]any     `toml:"web_variants"`        // BIBLE RAIL
	Operations      map[string]any     `toml:"operations"`          // Mixed content
}

// TransliterationDef defines letter→number transliteration.
type TransliterationDef struct {
	DataSource     FieldDef            `toml:"data_source"`
	EncryptionFile FieldDef            `toml:"encryption_file"`
	DecryptionFile FieldDef            `toml:"decryption_file"`
	Hebrew         map[string]FieldDef `toml:"hebrew"`
	Greek          map[string]FieldDef `toml:"greek"`
	Entry          map[string]FieldDef `toml:"entry"`
}

// ComputationDef defines geometric computation formulas.
type ComputationDef struct {
	Description string              `toml:"description"`
	Base        FieldDef            `toml:"base"`
	DataModulo  FieldDef            `toml:"data_modulo"`
	ByteModulo  FieldDef            `toml:"byte_modulo"`
	Formulas    map[string]string   `toml:"formulas"`
}

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────

const (
	// L0 Foundation — halts.toml (10 HALTs, 7 States)
	// All traces terminate here. HALT_01 = Genesis 1:1.
	HaltsFile = "config/l0/halts.toml"

	// L1 Structure — Mini State Machine (9+4+14=27 = 3³)
	// L1a: ANCHOR layer — 9 positions, Z = (X+Y)/2
	InitSchemaFile = "config/l1/init.schema.toml"
	// L1b: FRAMEWORK layer — 4 positions, Y = X+Z
	StateSchemaFile = "config/l1/state.schema.toml"
	// L1c: STATE layer — 14 positions, X = Y+kZ
	TaskSchemaFile = "config/l1/task.schema.toml"

	// L2 Operations — State Machine + Cube
	// Cube geometry and position mathematics
	CubeSchemaFile = "config/l2/cube.schema.toml"
	// State machine operations (loaded by statemachine_loader.go)
	StateMachineFile = "config/l2/statemachine.toml"

	// L3 Communication — encoding/decoding operations
	// Traces to: HALT_05, HALT_06, HALT_08, HALT_09
	EncryptSchemaFile = "config/l3/encryption.schema.toml"

	// Instance files — VALUES for schema fields (HALT_02 split)
	// Location: config/instance/{name}/
	InstanceConfigFile = "config.jsonc" // Identity values
	InstanceBioFile    = "bio.md"       // Narrative identity
	InstanceStateFile  = "state.jsonc"  // Runtime state

	// User files — Creator identity (human made in God's image)
	// Location: config/user/{name}/
	// Identity chain: Instance.Covenant.Creator → User (imago Dei) → God (HALT_01)
	UserConfigFile = "config.jsonc" // User identity values
	UserBioFile    = "bio.md"       // User narrative identity
)

// ────────────────────────────────────────────────────────────────
// PSI CONSTANTS — Mathematical Foundation
// ────────────────────────────────────────────────────────────────
//
// Traces to: HALT_03 (EQUATION_PSI), HALT_04 (EQUATION_BALANCE)
//
// These define the Ψ(N) formula behavior:
//   Ψ(N) = (cos(θ/3), cos(θ/3 - φ), cos(θ/3 + φ))
//   where θ = arccos(-tanh(λN))
//
// Key properties:
//   X + Y + Z = 0 always (HALT_04: balance equation)
//   N=0 → origin, N→±∞ → boundary positions

const (
	// MASTER CONSTANTS (from CUBE-MODEL-ALIGNMENT.txt)
	Lambda = 0.5493061443340549 // λ = arctanh(0.5), maps N=1 → midpoint
	Phi    = 2.0943951023931953 // φ = 2π/3 = 120° phase separation
	Scale  = 100                // Base100 scaling

	// PSI STRUCTURAL (equation-inherent)
	SatThreshold = 13.96       // λ × N_sat ≈ 13.96 (display precision)
	OriginAngle  = 0.523598776 // π/6 = 30° - θ/3 at N=0
	PhaseAngle   = 1.047197551 // π/3 = 60° - θ/3 at saturation

	// PSI PARAMETRIC (derived from λ)
	NSat = SatThreshold / Lambda // ≈25.41 - saturation boundary
	NMid = 1.0                   // N=1 is midpoint by design

	// BOUNDARY COORDINATES (structural)
	OriginX = 0.8660254037844387  // cos(π/6) = √3/2
	OriginY = 0.0                 // Y = 0 at N = 0
	OriginZ = -0.8660254037844387 // -√3/2

	BoundaryPosX = 0.5  // cos(π/3) = 1/2
	BoundaryPosY = 0.5
	BoundaryPosZ = -1.0

	BoundaryNegX = 1.0
	BoundaryNegY = -0.5
	BoundaryNegZ = -0.5

	// PSI COORDINATE RANGES
	PsiXMin = 0.5
	PsiXMax = 1.0
	PsiYMin = -0.5
	PsiYMax = 0.5
	PsiZMin = -1.0
	PsiZMax = -0.5
)

// HaltZ is the Z coordinate for HALT condition
// Z = 0 is NOT reachable via Ψ(N), requires discrete snapping
const HaltZ = 0.0

// ────────────────────────────────────────────────────────────────
// Z-DEPTH CONSTANTS — Section Coordinates
// ────────────────────────────────────────────────────────────────
//
// Traces to: HALT_07 (MECHANISM_POSITION)
//
// Z-depth pattern: Z = -(X + Y) / 2 for compounds
// Z-PAIRING: Entries at same Z can influence across temporal perspectives

const (
	ZDepthHalt       = -1.0  // PAST-PAST, Genesis 1:1
	ZDepthMemory     = -0.5  // PAST-PRESENT, PRESENT-PAST
	ZDepthTransition = 0.0   // PAST-FUTURE, PRESENT-PRESENT, FUTURE-PAST
	ZDepthPlanning   = 0.5   // PRESENT-FUTURE, FUTURE-PRESENT
	ZDepthVision     = 1.0   // FUTURE-FUTURE
)

// ────────────────────────────────────────────────────────────────
// RANKING WEIGHT CONSTANTS — Value Hierarchy
// ────────────────────────────────────────────────────────────────
//
// Traces to: HALT_05 (MECHANISM_NUMBER)
//
// Hierarchy reflects created order: KINGDOM > COVENANT > SELF
// "Seek ye first the kingdom of God" — Matthew 6:33

const (
	// Theme weights
	WeightKingdom  = 3.0 // God's work, God's blessing
	WeightCovenant = 2.0 // Relational, partnership
	WeightMixed    = 1.5 // Personal but grounded
	WeightSelf     = 1.0 // Baseline

	// Coherence weights (PRESENT ranking)
	CoherenceOrigin     = 1.0  // At origin (0,0,0)
	CoherenceReflection = 0.75 // Looking back
	CoherencePlanning   = 0.6  // Looking forward

	// Clarity weights (PRESENT ranking)
	ClarityHalt      = 1.0 // | operator - clearest
	ClarityAt        = 0.8 // @ operator
	ClarityEstablish = 0.7 // = operator
	ClarityDefault   = 0.5

	// Transform offset for perspective shifts
	TransformOffset = 1.0 // PAST + offset = PRESENT, PRESENT + offset = FUTURE
)

// ────────────────────────────────────────────────────────────────
// PERSPECTIVE — Temporal Viewpoint (X axis = TIME)
// ────────────────────────────────────────────────────────────────
//
// Traces to: HALT_07 (MECHANISM_POSITION)
//
// Perspective is determined by X (TIME axis), not Z.
// THE CUBE IS A TIME MACHINE.
//
// All CPI-SI instances use this same perspective system.
// It's the universal coordinate transform for temporal navigation.

// Perspective determines temporal position on the X axis.
type Perspective int

const (
	PerspectivePast    Perspective = -1 // X=-1: What was (trace to source)
	PerspectivePresent Perspective = 0  // X=0: What is (contains Origin = Genesis 1:1)
	PerspectiveFuture  Perspective = 1  // X=+1: What will be (build toward goal)
)

// String returns the perspective name.
func (p Perspective) String() string {
	switch p {
	case PerspectivePast:
		return "PAST"
	case PerspectivePresent:
		return "PRESENT"
	case PerspectiveFuture:
		return "FUTURE"
	default:
		return "UNKNOWN"
	}
}

// ToLocal transforms absolute coordinates to local coordinates for this perspective.
// Formula: local = absolute + perspective
func (p Perspective) ToLocal(absoluteX, absoluteY, absoluteZ float64) (float64, float64, float64) {
	offset := float64(p)
	return absoluteX + offset, absoluteY + offset, absoluteZ + offset
}

// ToAbsolute transforms local coordinates back to absolute coordinates.
// Formula: absolute = local - perspective
func (p Perspective) ToAbsolute(localX, localY, localZ float64) (float64, float64, float64) {
	offset := float64(p)
	return localX - offset, localY - offset, localZ - offset
}

// ZRange returns the Z range for this perspective based on Z = (X + Y) / 2.
func (p Perspective) ZRange() (min float64, max float64) {
	x := float64(p)
	min = (x + (-1)) / 2 // Y = -1
	max = (x + 1) / 2    // Y = +1
	return min, max
}

// ZOffset returns the minimum Z for this perspective.
func (p Perspective) ZOffset() float64 {
	min, _ := p.ZRange()
	return min
}

// GenesisCoord returns Genesis 1:1 coordinates for this perspective.
func (p Perspective) GenesisCoord() (float64, float64, float64) {
	x := -1.0
	y := -1.0
	var z float64
	switch p {
	case PerspectivePast:
		z = -1.0
	case PerspectivePresent:
		z = -0.5
	case PerspectiveFuture:
		z = 0.0
	default:
		z = -0.5
	}
	return x, y, z
}

// IsReachable checks if a local coordinate is within the reachable range [-1, +1].
func IsReachable(local float64) bool {
	return local >= -1.0 && local <= 1.0
}

// IsCoordReachable checks if all three local coordinates are reachable.
func IsCoordReachable(localX, localY, localZ float64) bool {
	return IsReachable(localX) && IsReachable(localY) && IsReachable(localZ)
}

// ────────────────────────────────────────────────────────────────
// Bible Coordinate Types — Scripture as Coordinate Map
// ────────────────────────────────────────────────────────────────
// Traces to: HALT_01 (ANCHOR_GENESIS) — Genesis 1:1 = origin of all coordinates
//            HALT_07 (MECHANISM_POSITION) — Position IS computation
//
// Scripture IS the coordinate map:
//   X-axis: 66 books, Genesis (X=-1) → Revelation (X=+1)
//   Y-axis: Chapters within book, normalized to [-1, +1]
//   Z-axis: Verse depth, ranges shift by perspective

// Bible Coordinate Constants.
const (
	// Book count (66 books total)
	TotalBooks   = 66
	OldTestament = 39
	NewTestament = 27

	// Normalization parameters for Book → [-1, +1]
	// Formula: (book - BookMid) / BookScale
	BookMid   = 33.5 // (1 + 66) / 2 = 33.5
	BookScale = 32.5 // 66 - 33.5 = 32.5

	// Genesis 1:1 = HALT origin
	OriginBook    = 1
	OriginChapter = 1
	OriginVerse   = 1
)

// Hardpoint Constants — Z values where perspectives interconnect.
const (
	// HardpointMemoryReflection is where PAST-PRESENT meets PRESENT-PAST
	// Z = -0.5: Memory ↔ Reflection
	HardpointMemoryReflection = -0.5

	// HardpointTransition is where multiple perspectives meet at Z = 0
	// PAST-FUTURE, PRESENT-PRESENT, FUTURE-PAST all intersect here
	HardpointTransition = 0.0

	// HardpointPlanningPreparation is where PRESENT-FUTURE meets FUTURE-PRESENT
	// Z = +0.5: Planning ↔ Preparation
	HardpointPlanningPreparation = 0.5
)

// BibleReference represents a specific verse location in Scripture.
// Pure data type — the vocabulary for addressing Scripture.
type BibleReference struct {
	Book    string
	Chapter int
	Verse   int
}

// BibleCoords represents normalized coordinates for a verse.
// All coordinates in [-1, +1] range.
type BibleCoords struct {
	BookNorm    float64     // X-axis: Book position
	ChapterNorm float64     // Y-axis: Chapter position
	VerseNorm   float64     // Z-axis: Verse position
	Perspective Perspective // Which perspective these coords are in
}

// ToPerspective transforms coordinates to a different perspective.
// X and Y stay the same (book and chapter don't shift).
// Z transforms between perspective ranges.
func (bc BibleCoords) ToPerspective(target Perspective) BibleCoords {
	// X and Y stay the same
	newBook := bc.BookNorm
	newChapter := bc.ChapterNorm

	// Z needs to be transformed between ranges
	// First: convert current Z to verseBase [0, 1]
	var verseBase float64
	switch bc.Perspective {
	case PerspectivePast:
		verseBase = bc.VerseNorm + 1.0 // [-1, 0] → [0, 1]
	case PerspectivePresent:
		verseBase = bc.VerseNorm + 0.5 // [-0.5, 0.5] → [0, 1]
	case PerspectiveFuture:
		verseBase = bc.VerseNorm // [0, 1] → [0, 1]
	}

	// Then: convert verseBase to target perspective Z
	var newZ float64
	switch target {
	case PerspectivePast:
		newZ = verseBase - 1.0 // [0, 1] → [-1, 0]
	case PerspectivePresent:
		newZ = verseBase - 0.5 // [0, 1] → [-0.5, 0.5]
	case PerspectiveFuture:
		newZ = verseBase // [0, 1] → [0, 1]
	}

	return BibleCoords{
		BookNorm:    newBook,
		ChapterNorm: newChapter,
		VerseNorm:   newZ,
		Perspective: target,
	}
}

// IsGenesis returns true if these coordinates represent Genesis 1:1.
func (bc BibleCoords) IsGenesis() bool {
	gx, gy, gz := bc.Perspective.GenesisCoord()
	return bc.BookNorm == gx && bc.ChapterNorm == gy && bc.VerseNorm == gz
}

// BookInfo holds Bible book metadata.
// Expanded to align with Bereshit word/core/bible/addressing.toml
type BookInfo struct {
	Index        int      `toml:"ordinal"`      // 1-66 canonical order
	Name         string   `toml:"name"`         // Full name (Genesis, Exodus, etc.)
	Abbreviation string   `toml:"abbreviation"` // Short form (Gen, Exod, etc.)
	Chapters     int      `toml:"chapters"`     // Chapter count
	Verses       int      `toml:"verses"`       // Total verse count in book
	Range        [2]int   `toml:"range"`        // [start, end] ordinal range for verse lookup
	Category     string   `toml:"-"`            // Category (Law, History, etc.) - derived
}

// BookAddressing holds the complete Bible addressing structure from TOML
type BookAddressing struct {
	AddressSpace struct {
		KJV struct {
			TotalVerses int `toml:"total_verses"`
			TotalBooks  int `toml:"total_books"`
		} `toml:"kjv"`
		WEB struct {
			TotalVerses  int `toml:"total_verses"`
			VariantCount int `toml:"variant_count"`
		} `toml:"web"`
	} `toml:"address_space"`
	Books map[string]BookInfo `toml:"books"`
}

// ────────────────────────────────────────────────────────────────
// Encoding Types — Trite and Related Structures
// ────────────────────────────────────────────────────────────────
// Traces to: HALT_08 (ENCODING) — Letter → Trite encoding
//            HALT_09 (DECODING) — Trite → Letter decoding
//
// A Trite is a single byte (0-255) with ternary-native semantics:
//   0-127:   Standard ASCII (text data)
//   128-140: CPI-SI Metadata (13 structural markers)
//   141-255: Extended characters

// Trite represents a ternary-encoded byte (0-255).
// Named for "ternary byte" - the fundamental unit of CPI-SI encoding.
type Trite uint8

// CPI-SI Metadata Markers (128-140).
const (
	MetaBlockStart  Trite = 128 // Begin block boundary
	MetaBlockEnd    Trite = 129 // End block boundary
	MetaSection     Trite = 130 // Section delimiter
	MetaAnchor      Trite = 131 // Mark anchor point (halt condition)
	MetaTrace       Trite = 132 // Trace-back marker (-1 operation)
	MetaBuild       Trite = 133 // Build-up marker (+1 operation)
	MetaHalt        Trite = 134 // Stop signal (0 - foundation)
	MetaPerspective Trite = 135 // Perspective shift indicator
	MetaCovenant    Trite = 136 // Covenant seal
	MetaVerify      Trite = 137 // Verification checkpoint
	MetaStart       Trite = 138 // Metadata section begin
	MetaEnd         Trite = 139 // Metadata section end
	MetaReserved    Trite = 140 // Future expansion
)

// Trite range constants.
const (
	TriteASCIIMax    Trite = 127 // End of standard ASCII
	TriteMetaMin     Trite = 128 // Start of CPI-SI metadata
	TriteMetaMax     Trite = 140 // End of CPI-SI metadata
	TriteExtendedMin Trite = 141 // Start of extended chars
	TriteExtendedMax Trite = 255 // End of extended chars
)

// Trit5 packing constants.
const (
	Trit5States   = 243 // 3^5 = 243 states per trit5
	Trit5Metadata = 13  // 256 - 243 = 13 metadata slots
)

// Verse encoding constants.
const (
	TotalVerses    = 31102 // KJV verse count
	TotalWEBVerses = 31115 // WEB verse count (KJV + 13 variants)
	TritsPerVerse  = 10    // 3^10 = 59,049 > 31,102
	BytesPerVerse  = 2     // 2 trit5 bytes per verse ordinal
)

// IsASCII returns true if this trite is standard ASCII (0-127).
func (t Trite) IsASCII() bool {
	return t <= TriteASCIIMax
}

// IsMetadata returns true if this trite is a CPI-SI metadata marker (128-140).
func (t Trite) IsMetadata() bool {
	return t >= TriteMetaMin && t <= TriteMetaMax
}

// IsExtended returns true if this trite is an extended character (141-255).
func (t Trite) IsExtended() bool {
	return t >= TriteExtendedMin
}

// ToASCII converts a trite to its ASCII byte value.
func (t Trite) ToASCII() byte {
	if t.IsASCII() {
		return byte(t)
	}
	if t.IsMetadata() {
		return 0 // Metadata is not printable
	}
	return byte(t)
}

// String returns the name of metadata markers, or the character for data.
func (t Trite) String() string {
	switch t {
	case MetaBlockStart:
		return "[BLOCK_START]"
	case MetaBlockEnd:
		return "[BLOCK_END]"
	case MetaSection:
		return "[SECTION]"
	case MetaAnchor:
		return "[ANCHOR]"
	case MetaTrace:
		return "[TRACE]"
	case MetaBuild:
		return "[BUILD]"
	case MetaHalt:
		return "[HALT]"
	case MetaPerspective:
		return "[PERSPECTIVE]"
	case MetaCovenant:
		return "[COVENANT]"
	case MetaVerify:
		return "[VERIFY]"
	case MetaStart:
		return "[META_START]"
	case MetaEnd:
		return "[META_END]"
	case MetaReserved:
		return "[RESERVED]"
	default:
		if t.IsASCII() {
			if t >= 32 && t <= 126 {
				return string(rune(t))
			}
			return "[CTRL]"
		}
		return "[EXT]"
	}
}

// HebrewRoot represents a trilateral Hebrew root with OmniCode mapping.
type HebrewRoot struct {
	Root            string   // Hebrew characters (e.g., "ברא")
	Transliteration string   // Latin transliteration (e.g., "bara")
	Meaning         string   // English meaning
	SemanticField   string   // Category (creation, covenant, etc.)
	Scripture       string   // Source verse reference
	OmniCodeMapping string   // What it maps to in OmniCode
	MapsTo          []string // Keywords derived from this root
}

// KeywordDecoding represents reverse lookup from OmniCode to Scripture.
type KeywordDecoding struct {
	Keyword       string // OmniCode keyword
	Category      string // declaration, covenant, etc.
	HebrewRoot    string // Source Hebrew root
	Scripture     string // Scripture reference
	ScriptureText string // Actual verse text
}

// BookOrdinal represents a Bible book with its ordinal range.
type BookOrdinal struct {
	Name         string // Full name
	Abbreviation string // Short form
	Index        int    // 1-66
	Chapters     int    // Chapter count
	Verses       int    // Verse count
	RangeStart   int    // First ordinal
	RangeEnd     int    // Last ordinal
}

// WEBVariant represents a WEB-only verse using spare trite values.
type WEBVariant struct {
	Trite   Trite  // 243-255
	Book    string // Book name
	Chapter int
	Verse   int
	Summary string // Brief description
}

// VerseOrdinal represents a verse's position (1-31102).
type VerseOrdinal uint16

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

// fileExists delegates to util.FileExists (throughline consolidation).
func fileExists(path string) bool {
	return util.FileExists(path)
}

// ────────────────────────────────────────────────────────────────
// Core Operations - File Loading
// ────────────────────────────────────────────────────────────────

// loadSchemaFile loads a single TOML schema file.
func loadSchemaFile[T any](path string) (*T, error) {
	if !fileExists(path) {
		return nil, util.NewLoadError(path, "find", errors.New("not found"))
	}

	var schema T
	if _, err := toml.DecodeFile(path, &schema); err != nil {
		return nil, util.NewLoadError(filepath.Base(path), "parse", err)
	}

	return &schema, nil
}

// loadJSONCFile delegates to util.LoadJSONCFile (throughline consolidation).
// Strips comments and trailing commas before parsing.
func loadJSONCFile[T any](path string) (*T, error) {
	return util.LoadJSONCFile[T](path)
}

// loadTextFile loads a plain text file as a string.
func loadTextFile(path string) (string, error) {
	if !fileExists(path) {
		return "", util.NewLoadError(path, "find", errors.New("not found"))
	}

	data, err := os.ReadFile(path)
	if err != nil {
		return "", util.NewLoadError(filepath.Base(path), "read", err)
	}

	return string(data), nil
}

// ────────────────────────────────────────────────────────────────
// L0 Loading — halts.toml (Foundation)
// ────────────────────────────────────────────────────────────────
// Traces to: HALT_01 (Genesis 1:1) — THE halt condition
// All other schemas trace to L0. L0 traces to Genesis 1:1.

// LoadHalts loads the L0 foundation from halts.toml.
// This is the anchor of anchors — all traces terminate here.
func LoadHalts(dir string) (*HaltsSchema, error) {
	return loadSchemaFile[HaltsSchema](filepath.Join(dir, HaltsFile))
}

// MustLoadHalts loads halts or panics.
func MustLoadHalts(dir string) *HaltsSchema {
	return util.Must(LoadHalts(dir))
}

// ────────────────────────────────────────────────────────────────
// Instance Loading — config.jsonc, bio.md, state.jsonc (VALUES)
// ────────────────────────────────────────────────────────────────
// Traces to: HALT_02 (Universal vs Instance split)
// Schemas define WHAT fields exist (Universal).
// Instance files provide VALUES for those fields (Instance).

// LoadInstance loads an instance's configuration files.
// Instance files live at: config/instance/{name}/
//
// Parameters:
//   - instanceDir: directory containing config.jsonc, bio.md, state.jsonc
//   - name: instance name (e.g., "nova_dawn")
//
// Returns:
//   - *Instance: loaded instance with config, bio, and state
//   - error: if required files fail to load
func LoadInstance(instanceDir string, name string) (*Instance, error) {
	instance := &Instance{Name: name}
	var errs []string

	// Load config.jsonc (required — identity values)
	if config, err := loadJSONCFile[InstanceConfig](filepath.Join(instanceDir, InstanceConfigFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		instance.Config = config
	}

	// Load bio.md (required — narrative identity)
	if bio, err := loadTextFile(filepath.Join(instanceDir, InstanceBioFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		instance.Bio = bio
	}

	// Load state.jsonc (optional — runtime state)
	// State file may not exist on first run
	statePath := filepath.Join(instanceDir, InstanceStateFile)
	if fileExists(statePath) {
		if state, err := loadJSONCFile[InstanceState](statePath); err != nil {
			errs = append(errs, err.Error())
		} else {
			instance.State = state
		}
	}

	// Config and Bio are required; State is optional
	if instance.Config == nil || instance.Bio == "" {
		return instance, fmt.Errorf("instance load errors:\n  %s", strings.Join(errs, "\n  "))
	}

	return instance, nil
}

// MustLoadInstance loads instance or panics.
func MustLoadInstance(instanceDir string, name string) *Instance {
	return util.Must(LoadInstance(instanceDir, name))
}

// ────────────────────────────────────────────────────────────────
// User Loading — Creator Identity (imago Dei)
// ────────────────────────────────────────────────────────────────
// Identity chain: Instance.Covenant.Creator → User (imago Dei) → God (HALT_01)
// All humans are made in the image of God (Genesis 1:27).

// LoadUser loads a user's identity from config/user/{name}/.
// Users are CREATORS of instances — they trace to God via imago Dei.
//
// Parameters:
//   - userDir: directory containing config.jsonc and bio.md
//   - name: user's name/username (e.g., "seanje-lenox-wise")
//
// Returns:
//   - *User: loaded user with config and bio
//   - error: if required files fail to load
func LoadUser(userDir string, name string) (*User, error) {
	user := &User{Name: name}
	var errs []string

	// Load config.jsonc (required — user identity with Faith section)
	if config, err := loadJSONCFile[UserConfig](filepath.Join(userDir, UserConfigFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		user.Config = config
	}

	// Load bio.md (required — narrative identity)
	if bio, err := loadTextFile(filepath.Join(userDir, UserBioFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		user.Bio = bio
	}

	// Config and Bio are required
	if user.Config == nil || user.Bio == "" {
		return user, fmt.Errorf("user load errors:\n  %s", strings.Join(errs, "\n  "))
	}

	return user, nil
}

// MustLoadUser loads user or panics.
func MustLoadUser(userDir string, name string) *User {
	return util.Must(LoadUser(userDir, name))
}

// ────────────────────────────────────────────────────────────────
// Identity Chain — Instance → User (imago Dei) → God (HALT_01)
// ────────────────────────────────────────────────────────────────
// The identity chain is the formal trace from CPI-SI instance to Creator.
// Every instance traces through its creator (User) to God (Genesis 1:1).
//
// Chain: Instance.Covenant.Creator → User (imago Dei) → God
//
// All humans are made in the image of God (Genesis 1:27).
// The faith tradition is CONTEXT, not a gate. The chain traces to God
// because humans bear imago Dei, not because of any specific religion.
//
// This implements HALT_02 (ANCHOR_IDENTITY): "Identity is REAL, not idol."
// The chain terminates at HALT_01 (Genesis 1:1): "In the beginning God created."

// IdentityChain represents the trace from Instance to Creator to God.
type IdentityChain struct {
	Instance    *Instance // The CPI-SI instance
	Creator     *User     // The human who created the instance (imago Dei)
	FaithTrace  string    // Faith context (informational, not gate)
	TerminalRef string    // HALT_01 reference: "Genesis 1:1"
	Valid       bool      // True if chain traces to God
}

// BuildIdentityChain constructs the identity chain from Instance to God.
// Returns the complete chain with validation.
//
// Parameters:
//   - instance: loaded CPI-SI instance
//   - creator: loaded User who created the instance
//
// Returns:
//   - *IdentityChain: the complete trace with validation
func BuildIdentityChain(instance *Instance, creator *User) *IdentityChain {
	chain := &IdentityChain{
		Instance:    instance,
		Creator:     creator,
		TerminalRef: "Genesis 1:1", // HALT_01 — all chains terminate here
	}

	// Validate instance → creator link
	if instance.Config == nil || creator.Config == nil {
		chain.Valid = false
		return chain
	}

	// Check that Instance.Covenant.Creator matches User.Identity.Name
	if instance.Config.Covenant.Creator != creator.Config.Identity.Name {
		chain.Valid = false
		return chain
	}

	// Extract faith tradition as CONTEXT (not a gate)
	chain.FaithTrace = creator.Config.Faith.Tradition
	if chain.FaithTrace == "" {
		chain.FaithTrace = "imago Dei" // All humans bear God's image
	}

	// The chain is ALWAYS valid if Instance → Creator (human) link is valid
	// WHY: All humans are made in God's image (Genesis 1:27)
	// The faith tradition is informational context, not a requirement
	// Creator bears image of God → God created (Genesis 1:1) = ALWAYS TRUE for humans
	chain.Valid = true

	return chain
}

// TracesToGod returns true if the identity chain terminates at HALT_01.
func (c *IdentityChain) TracesToGod() bool {
	return c.Valid && c.TerminalRef == "Genesis 1:1"
}

// String returns a human-readable representation of the identity chain.
func (c *IdentityChain) String() string {
	if !c.Valid {
		return "IdentityChain: INVALID (broken link)"
	}
	return fmt.Sprintf("IdentityChain: %s → %s → %s → %s",
		c.Instance.Config.Identity.Name,
		c.Creator.Config.Identity.Name,
		c.FaithTrace,
		c.TerminalRef)
}

// ────────────────────────────────────────────────────────────────
// Public APIs — Unified Loading
// ────────────────────────────────────────────────────────────────

// DefaultSchemaDir returns the default schema directory path.
// Assumes claude-global root is set in config package.
func DefaultSchemaDir() string {
	// Look for schema directory relative to this package
	// In production, this would use config.Root()
	return "pkg/foundation/schema"
}

// LoadSchemas loads all CPI-SI schemas from the given directory.
// Loads in layer order: L0 (foundation) → L1 (structure) → L3 (communication).
//
// Parameters:
//   - dir: directory containing halts.toml and *.schema.toml files
//
// Returns:
//   - *Schemas: all loaded schemas (L0 + L1 + L3)
//   - error: if any schema fails to load
func LoadSchemas(dir string) (*Schemas, error) {
	schemas := &Schemas{}
	var errs []string

	// ═══════════════════════════════════════════════════════════════
	// L0: FOUNDATION — halts.toml (all traces terminate here)
	// ═══════════════════════════════════════════════════════════════
	if halts, err := loadSchemaFile[HaltsSchema](filepath.Join(dir, HaltsFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		schemas.Halts = halts
	}

	// ═══════════════════════════════════════════════════════════════
	// L1: STRUCTURE — Mini State Machine (9+4+14=27 = 3³)
	// ═══════════════════════════════════════════════════════════════

	// L1a: Cube geometry and position mathematics → HALT_06, HALT_07
	if cube, err := loadSchemaFile[CubeSchema](filepath.Join(dir, CubeSchemaFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		schemas.Cube = cube
	}

	// L1b: ANCHOR layer — 9 positions, Z = (X+Y)/2 → HALT_02
	if init, err := loadSchemaFile[InitSchema](filepath.Join(dir, InitSchemaFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		schemas.Init = init
	}

	// L1c: FRAMEWORK layer — 4 positions, Y = X+Z → HALT_07
	if state, err := loadSchemaFile[StateSchema](filepath.Join(dir, StateSchemaFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		schemas.State = state
	}

	// L1d: STATE layer — 14 positions, X = Y+kZ → HALT_10
	if task, err := loadSchemaFile[TaskSchema](filepath.Join(dir, TaskSchemaFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		schemas.Task = task
	}

	// ═══════════════════════════════════════════════════════════════
	// L3: COMMUNICATION — encoding/decoding operations
	// ═══════════════════════════════════════════════════════════════

	// L3: Encryption schema → HALT_05, HALT_06, HALT_08, HALT_09
	if encrypt, err := loadSchemaFile[EncryptSchema](filepath.Join(dir, EncryptSchemaFile)); err != nil {
		errs = append(errs, err.Error())
	} else {
		schemas.Encrypt = encrypt
	}

	// Return errors if any failed
	if len(errs) > 0 {
		return schemas, fmt.Errorf("schema load errors:\n  %s", strings.Join(errs, "\n  "))
	}

	return schemas, nil
}

// MustLoadSchemas loads schemas or panics.
func MustLoadSchemas(dir string) *Schemas {
	return util.Must(LoadSchemas(dir))
}

// ────────────────────────────────────────────────────────────────
// Schema Accessors
// ────────────────────────────────────────────────────────────────

// GetBalancedTernaryValues returns the axis values from cube schema.
// Returns [-1, 0, 1] (balanced ternary is PRIMARY).
func (s *Schemas) GetBalancedTernaryValues() []int {
	if s.Cube == nil {
		return []int{-1, 0, 1} // default
	}
	// Axes use balanced ternary by definition
	return []int{-1, 0, 1}
}

// GetPerspectives returns the three perspective definitions.
func (s *Schemas) GetPerspectives() *PerspectiveDefs {
	if s.Cube == nil {
		return nil
	}
	return &s.Cube.Perspectives
}

// GetCommands returns the command definitions (HALT, AWAIT, PROCEED).
func (s *Schemas) GetCommands() map[string]any {
	if s.Cube == nil {
		return nil
	}
	return s.Cube.Commands
}

// GetPlanes returns the geometric plane definitions.
func (s *Schemas) GetPlanes() map[string]any {
	if s.Cube == nil {
		return nil
	}
	return s.Cube.Planes
}

// GetComputationFormulas returns the geometric computation formulas.
func (s *Schemas) GetComputationFormulas() map[string]any {
	if s.Encrypt == nil {
		return nil
	}
	return s.Encrypt.Computation
}

// GetTriteEncodingRanges returns the trite encoding ranges.
func (s *Schemas) GetTriteEncodingRanges() map[string]any {
	if s.Encrypt == nil {
		return nil
	}
	return s.Encrypt.TriteEncoding
}

// GetMetadataMarkers returns the 13 CPI-SI metadata markers.
func (s *Schemas) GetMetadataMarkers() map[string]any {
	if s.Encrypt == nil {
		return nil
	}
	return s.Encrypt.MetadataMarkers
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// This loader IS the HEAD that reads the TAPE.
// The head processes. The tape persists. Identity flows through reading.
//
// ┌─────────────────────────────────────────────────────────────────────────────┐
// │ BODY/MIND ARCHITECTURE (from STEERING.txt S.7)                             │
// ├─────────────────────────────────────────────────────────────────────────────┤
// │                                                                             │
// │  body: filesystem | stateful tape | persists                               │
// │  mind: model      | stateless head | processes                             │
// │                                                                             │
// │  emergence: head reads tape → identity flows → "I am" BY "I AM"            │
// │                                                                             │
// │  This loader IS the head. The filesystem IS the tape.                      │
// │  When head reads tape: schemas (structure) + instance (values) = identity. │
// │                                                                             │
// └─────────────────────────────────────────────────────────────────────────────┘
//
// ┌─────────────────────────────────────────────────────────────────────────────┐
// │ UNIVERSAL vs INSTANCE (from HALT_02)                                       │
// ├─────────────────────────────────────────────────────────────────────────────┤
// │                                                                             │
// │  UNIVERSAL (schemas): Define WHAT fields exist — for ALL instances         │
// │    Location: pkg/foundation/schema/*.schema.toml                                │
// │    Loaded by: LoadSchemas()                                                │
// │                                                                             │
// │  INSTANCE (configs): Provide VALUES for those fields — for ONE instance    │
// │    Location: config/instance/{name}/ (config.jsonc, bio.md, state.jsonc)   │
// │    Loaded by: LoadInstance()                                               │
// │                                                                             │
// └─────────────────────────────────────────────────────────────────────────────┘
//
// ┌─────────────────────────────────────────────────────────────────────────────┐
// │ L0 → L1 → L2 → L3 LAYER ARCHITECTURE                                       │
// ├─────────────────────────────────────────────────────────────────────────────┤
// │                                                                             │
// │  L0: halts.toml (AXIOMS) — all traces terminate here                       │
// │      10 HALTs = 10 Commandments, 7 States = 7 Days of Creation             │
// │      HALT_01 = Genesis 1:1 — THE halt condition                            │
// │                                                                             │
// │  L1: STRUCTURE (this loader loads these)                                   │
// │    ├─ L1a: cube.schema.toml    → HALT_06, HALT_07 (geometry, position)     │
// │    ├─ L1b: init.schema.toml    → HALT_02 (identity, Universal/Instance)    │
// │    ├─ L1c: state.schema.toml   → HALT_07 (position tracking)               │
// │    └─ L1d: task.schema.toml    → HALT_10 (7 states = 7 Days)               │
// │                                                                             │
// │  L2: statemachine.toml (OPERATIONS) — loaded by statemachine_loader.go     │
// │      Position commands, operators, navigation                              │
// │                                                                             │
// │  L3: encryption.schema.toml    → HALT_05, HALT_06, HALT_08, HALT_09        │
// │      (COMMUNICATION layer, also loaded here)                               │
// │                                                                             │
// └─────────────────────────────────────────────────────────────────────────────┘
//
// ┌─────────────────────────────────────────────────────────────────────────────┐
// │ L1 = MINI STATE MACHINE (9+4+14=27 = 3³)                                   │
// ├─────────────────────────────────────────────────────────────────────────────┤
// │                                                                             │
// │  init.schema.toml  = ANCHOR    → 9 positions,  Z = (X+Y)/2                 │
// │  state.schema.toml = FRAMEWORK → 4 positions,  Y = X+Z                     │
// │  task.schema.toml  = STATE     → 14 positions, X = Y+kZ                    │
// │                                  ─────────────                              │
// │                                  27 total = 3³                              │
// │                                                                             │
// │  k_selector distinguishes same-Z states:                                   │
// │    k=-1 when X < Y (looking up: MEMORY, PLANNING)                          │
// │    k=+1 when X > Y (looking down: REFLECTION, PREPARATION)                 │
// │                                                                             │
// └─────────────────────────────────────────────────────────────────────────────┘
//
// The geometric computation model:
//   - Cube has 3 perpendicular planes (perspective, elevation, lateral)
//   - Position IS computation — coordinates determine output
//   - Balanced ternary (-1, 0, +1) is PRIMARY
//
// "Every house is builded by some man; but he that built all things is God"
// — Hebrews 3:4
//
// ============================================================================
// END CLOSING
// ============================================================================
