// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-health-loader
// Purpose: Health schema loader - FRAMEWORK that reads from TOML ANCHOR
// Biblical: Leviticus 19:36 - "Just balances, just weights... shall you have"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-21
//
// Design Principle: Config-Driven
//   - ANCHOR: pkg/foundation/schema/config/l2/health/*.toml (the truth)
//   - FRAMEWORK: This loader (how to access the truth)
//   - Code reads from config, never hardcodes values
//
// Traces to: pkg/foundation/schema/config/l2/health/score.toml
//
// ═══════════════════════════════════════════════════════════════════════════

package health

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"path/filepath"
	"sync"

	"github.com/BurntSushi/toml"
	"github.com/creativeworkzstudio/claude-global/pkg/util/fs/paths"
)

// --- Schema File Constants ---

const (
	// Health schema files in l2/health/
	ScoreFile       = "l2/health/score.toml"
	LogFile         = "l2/health/log.toml"
	DiagnosticsFile = "l2/health/diagnostics.toml"
	RestoreFile     = "l2/health/restore.toml"
)

// --- Ternary Scale Constants (from score.toml) ---

// These are loaded from config, but we provide compile-time defaults
// that match the TOML for cases where config isn't available
const (
	TernaryMin    = -100
	TernaryCenter = 0
	TernaryMax    = 100
)

// ───────────────────────────────────────────────────────────────────────────
// TYPES - Mirror the TOML structure
// ───────────────────────────────────────────────────────────────────────────

// HealthConfig holds the complete health schema configuration
type HealthConfig struct {
	Score       ScoreConfig       `toml:"health"`
	Levels      LevelsConfig      `toml:"-"` // Parsed separately
	Display     DisplayConfig     `toml:"-"` // Parsed separately
	Ternary     TernaryConfig     `toml:"-"` // From log.toml/diagnostics.toml
	Actions     ActionsConfig     `toml:"-"` // From log.toml
}

// ScoreConfig mirrors score.toml [health] section
type ScoreConfig struct {
	Name           string `toml:"name"`
	Description    string `toml:"description"`
	BaseType       string `toml:"base_type"`
	StorageMin     int    `toml:"storage_min"`
	StorageMax     int    `toml:"storage_max"`
	StorageDefault int    `toml:"storage_default"`
}

// TernaryConfig holds ternary scale values
type TernaryConfig struct {
	Scale  TernaryScale  `toml:"scale"`
	Atomic TernaryAtomic `toml:"atomic"`
}

type TernaryScale struct {
	Min    int `toml:"min"`
	Center int `toml:"center"`
	Max    int `toml:"max"`
}

type TernaryAtomic struct {
	SuccessValue int `toml:"success_value"`
	FailureValue int `toml:"failure_value"`
	NeutralValue int `toml:"neutral_value"`
}

// LevelDef defines a single health level
type LevelDef struct {
	Name         string `toml:"name"`
	Description  string `toml:"description"`
	Hebrew       string `toml:"hebrew"`
	MinStored    int    `toml:"min_stored"`
	MaxStored    int    `toml:"max_stored"`
	TernaryRange [2]int `toml:"ternary_range"`
	Direction    string `toml:"direction"`
	Priority     string `toml:"priority"`
}

// LevelsConfig holds all 7 levels
type LevelsConfig struct {
	Broken  LevelDef `toml:"broken"`
	Wanting LevelDef `toml:"wanting"`
	Lacking LevelDef `toml:"lacking"`
	Even    LevelDef `toml:"even"`
	Sound   LevelDef `toml:"sound"`
	Whole   LevelDef `toml:"whole"`
	Perfect LevelDef `toml:"perfect"`
}

// DisplayDef defines visual representation for a level
type DisplayDef struct {
	Level       string `toml:"level"`
	Emoji       string `toml:"emoji"`
	Color       string `toml:"color"`
	ANSICode    string `toml:"ansi_code"`
	Description string `toml:"description"`
}

// DisplayConfig holds display mappings for all levels
type DisplayConfig struct {
	Broken  DisplayDef `toml:"broken"`
	Wanting DisplayDef `toml:"wanting"`
	Lacking DisplayDef `toml:"lacking"`
	Even    DisplayDef `toml:"even"`
	Sound   DisplayDef `toml:"sound"`
	Whole   DisplayDef `toml:"whole"`
	Perfect DisplayDef `toml:"perfect"`
}

// ActionDef defines a health action type
type ActionDef struct {
	Description string   `toml:"description"`
	Delta       int      `toml:"delta"`
	DeltaRange  [2]int   `toml:"delta_range"`
	Examples    []string `toml:"examples"`
	Scripture   string   `toml:"scripture"`
}

// ActionsConfig holds action type definitions
type ActionsConfig struct {
	Success  ActionDef `toml:"success"`
	Failure  ActionDef `toml:"failure"`
	Neutral  ActionDef `toml:"neutral"`
	Recovery ActionDef `toml:"recovery"`
	Reset    ActionDef `toml:"reset"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Singleton Config ---

var (
	globalConfig *HealthConfig
	configOnce   sync.Once
	configErr    error
)

// Config returns the global health configuration (lazy loaded)
func Config() (*HealthConfig, error) {
	configOnce.Do(func() {
		globalConfig, configErr = LoadHealthConfig("")
	})
	return globalConfig, configErr
}

// --- Loading Functions ---

// LoadHealthConfig loads health configuration from TOML schemas
func LoadHealthConfig(schemaDir string) (*HealthConfig, error) {
	if schemaDir == "" {
		schemaDir = paths.CPISISchemaConfig()
	}

	config := &HealthConfig{}

	// Load score.toml (main health schema)
	scorePath := filepath.Join(schemaDir, ScoreFile)
	if err := loadScoreConfig(scorePath, config); err != nil {
		return nil, err
	}

	// Load log.toml for ternary and actions
	logPath := filepath.Join(schemaDir, LogFile)
	if err := loadLogConfig(logPath, config); err != nil {
		// Non-fatal - use defaults
		config.Ternary = defaultTernaryConfig()
		config.Actions = defaultActionsConfig()
	}

	return config, nil
}

// loadScoreConfig parses score.toml
func loadScoreConfig(path string, config *HealthConfig) error {
	data, err := os.ReadFile(path)
	if err != nil {
		return err
	}

	// Parse into intermediate structure
	var raw struct {
		Health  ScoreConfig           `toml:"health"`
		Levels  map[string]LevelDef   `toml:"levels"`
		Display map[string]DisplayDef `toml:"display"`
	}

	if _, err := toml.Decode(string(data), &raw); err != nil {
		return err
	}

	config.Score = raw.Health

	// Map levels
	if raw.Levels != nil {
		config.Levels = LevelsConfig{
			Broken:  raw.Levels["broken"],
			Wanting: raw.Levels["wanting"],
			Lacking: raw.Levels["lacking"],
			Even:    raw.Levels["even"],
			Sound:   raw.Levels["sound"],
			Whole:   raw.Levels["whole"],
			Perfect: raw.Levels["perfect"],
		}
	} else {
		config.Levels = defaultLevelsConfig()
	}

	// Map display
	if raw.Display != nil {
		config.Display = DisplayConfig{
			Broken:  raw.Display["broken"],
			Wanting: raw.Display["wanting"],
			Lacking: raw.Display["lacking"],
			Even:    raw.Display["even"],
			Sound:   raw.Display["sound"],
			Whole:   raw.Display["whole"],
			Perfect: raw.Display["perfect"],
		}
	} else {
		config.Display = defaultDisplayConfig()
	}

	return nil
}

// loadLogConfig parses log.toml for ternary and actions
func loadLogConfig(path string, config *HealthConfig) error {
	data, err := os.ReadFile(path)
	if err != nil {
		return err
	}

	var raw struct {
		Ternary TernaryConfig            `toml:"ternary"`
		Actions map[string]ActionDef     `toml:"actions"`
	}

	if _, err := toml.Decode(string(data), &raw); err != nil {
		return err
	}

	config.Ternary = raw.Ternary

	if raw.Actions != nil {
		config.Actions = ActionsConfig{
			Success:  raw.Actions["success"],
			Failure:  raw.Actions["failure"],
			Neutral:  raw.Actions["neutral"],
			Recovery: raw.Actions["recovery"],
			Reset:    raw.Actions["reset"],
		}
	}

	return nil
}

// --- Helper Functions ---

// LevelFromTrueScore returns the level name for a true ternary score
func (c *HealthConfig) LevelFromTrueScore(trueScore int) string {
	switch {
	case trueScore <= -67:
		return "broken"
	case trueScore <= -34:
		return "wanting"
	case trueScore <= -1:
		return "lacking"
	case trueScore == 0:
		return "even"
	case trueScore <= 33:
		return "sound"
	case trueScore <= 66:
		return "whole"
	default:
		return "perfect"
	}
}

// EmojiFromLevel returns the emoji for a level name
func (c *HealthConfig) EmojiFromLevel(level string) string {
	switch level {
	case "broken":
		return c.Display.Broken.Emoji
	case "wanting":
		return c.Display.Wanting.Emoji
	case "lacking":
		return c.Display.Lacking.Emoji
	case "even":
		return c.Display.Even.Emoji
	case "sound":
		return c.Display.Sound.Emoji
	case "whole":
		return c.Display.Whole.Emoji
	case "perfect":
		return c.Display.Perfect.Emoji
	default:
		return "❓"
	}
}

// EmojiFromTrueScore returns emoji for a true ternary score
func (c *HealthConfig) EmojiFromTrueScore(trueScore int) string {
	return c.EmojiFromLevel(c.LevelFromTrueScore(trueScore))
}

// ColorFromLevel returns the color for a level name
func (c *HealthConfig) ColorFromLevel(level string) string {
	switch level {
	case "broken":
		return c.Display.Broken.Color
	case "wanting":
		return c.Display.Wanting.Color
	case "lacking":
		return c.Display.Lacking.Color
	case "even":
		return c.Display.Even.Color
	case "sound":
		return c.Display.Sound.Color
	case "whole":
		return c.Display.Whole.Color
	case "perfect":
		return c.Display.Perfect.Color
	default:
		return "white"
	}
}

// SuccessValue returns the atomic success delta (+1)
func (c *HealthConfig) SuccessValue() int {
	if c.Ternary.Atomic.SuccessValue != 0 {
		return c.Ternary.Atomic.SuccessValue
	}
	return 1 // Default narrow way
}

// FailureValue returns the atomic failure delta (-1)
func (c *HealthConfig) FailureValue() int {
	if c.Ternary.Atomic.FailureValue != 0 {
		return c.Ternary.Atomic.FailureValue
	}
	return -1 // Default broad way
}

// --- Default Configs (fallback when TOML not available) ---

func defaultTernaryConfig() TernaryConfig {
	return TernaryConfig{
		Scale: TernaryScale{
			Min:    -100,
			Center: 0,
			Max:    100,
		},
		Atomic: TernaryAtomic{
			SuccessValue: 1,
			FailureValue: -1,
			NeutralValue: 0,
		},
	}
}

func defaultActionsConfig() ActionsConfig {
	return ActionsConfig{
		Success: ActionDef{
			Description: "Positive action — narrow way (+1)",
			Delta:       1,
		},
		Failure: ActionDef{
			Description: "Negative action — broad way (-1)",
			Delta:       -1,
		},
		Neutral: ActionDef{
			Description: "No health impact",
			Delta:       0,
		},
		Recovery: ActionDef{
			Description: "Intentional recovery action (+1)",
			Delta:       1,
		},
	}
}

func defaultLevelsConfig() LevelsConfig {
	return LevelsConfig{
		Broken:  LevelDef{Name: "broken", TernaryRange: [2]int{-100, -67}, Direction: "wanting"},
		Wanting: LevelDef{Name: "wanting", TernaryRange: [2]int{-66, -34}, Direction: "wanting"},
		Lacking: LevelDef{Name: "lacking", TernaryRange: [2]int{-33, -1}, Direction: "wanting"},
		Even:    LevelDef{Name: "even", TernaryRange: [2]int{0, 0}, Direction: "center"},
		Sound:   LevelDef{Name: "sound", TernaryRange: [2]int{1, 33}, Direction: "integrity"},
		Whole:   LevelDef{Name: "whole", TernaryRange: [2]int{34, 66}, Direction: "integrity"},
		Perfect: LevelDef{Name: "perfect", TernaryRange: [2]int{67, 100}, Direction: "integrity"},
	}
}

func defaultDisplayConfig() DisplayConfig {
	return DisplayConfig{
		Broken:  DisplayDef{Level: "broken", Emoji: "💔", Color: "red"},
		Wanting: DisplayDef{Level: "wanting", Emoji: "🩹", Color: "orange"},
		Lacking: DisplayDef{Level: "lacking", Emoji: "💛", Color: "yellow"},
		Even:    DisplayDef{Level: "even", Emoji: "⚖️", Color: "white"},
		Sound:   DisplayDef{Level: "sound", Emoji: "💚", Color: "green"},
		Whole:   DisplayDef{Level: "whole", Emoji: "💙", Color: "blue"},
		Perfect: DisplayDef{Level: "perfect", Emoji: "💜", Color: "magenta"},
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// This loader is the FRAMEWORK for the health system:
//   - ANCHOR: pkg/foundation/schema/config/l2/health/*.toml
//   - FRAMEWORK: This loader (you are here)
//   - RUNTIME: config/health/runtime/*.jsonc
//
// Change the TOML → everything that uses this loader gets new values.
// The loader stays stable. The config is where change happens.
//
// "Just balances, just weights... shall you have" — Leviticus 19:36
//
