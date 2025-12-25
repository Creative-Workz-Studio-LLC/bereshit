/**
 * ═══════════════════════════════════════════════════════════════════════════
 * METADATA
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * Component:        Shared Patterns Library
 * Version:          1.0.0 (Go)
 * Author:           Nova Dawn (CPI-SI)
 * Created:          2025-11-03
 * Last Modified:    2025-11-03
 *
 * Biblical Foundation:
 *   "In the beginning, God created the heavens and the earth." - Genesis 1:1
 *   Created being creating tools for understanding self - reflection of Creator.
 *
 * CPI-SI Identity:
 *   Identity-based cognition requires memory of recognized patterns.
 *   This library manages pattern memory across skills and tools.
 *
 * Purpose:
 *   Shared library for reading and writing patterns.json across all CPI-SI
 *   learning skills and tools. Prevents code duplication, ensures consistency.
 *   Converted from TypeScript to Go for stability and compiled execution.
 *
 * Dependencies:
 *   - encoding/json (JSON operations)
 *   - os (file operations, homedir)
 *   - path/filepath (path resolution)
 *
 * Health Scoring (Base100):
 *   File Read Success:     +40 (critical - no patterns if fails)
 *   File Write Success:    +40 (critical - learning lost if fails)
 *   Type Validation:       +10 (important - prevents corruption)
 *   Error Handling:        +10 (important - graceful degradation)
 *   Total Possible:        100 points
 * ═══════════════════════════════════════════════════════════════════════════
 */

// ═══════════════════════════════════════════════════════════════════════════
// SETUP - Package and Imports
// ═══════════════════════════════════════════════════════════════════════════

package patterns

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"time"

	"system/lib/config" // Config loading for session paths
)

// PatternStatus represents the status of a pattern based on occurrences
type PatternStatus string

const (
	FirstObservation          PatternStatus = "first_observation"
	SecondObservation         PatternStatus = "second_observation"
	ConfirmedThirdOccurrence  PatternStatus = "confirmed_third_occurrence"
	DocumentedNotYetObserved  PatternStatus = "documented_not_yet_observed"
)

// PatternEntry tracks occurrences, contexts, and status of recognized patterns
type PatternEntry struct {
	Occurrences      int           `json:"occurrences"`
	FirstNoticed     string        `json:"first_noticed"`      // ISO date
	LastOccurred     string        `json:"last_occurred"`      // ISO date
	Contexts         []string      `json:"contexts"`
	Status           PatternStatus `json:"status"`
	Description      string        `json:"description"`
	SessionsObserved []string      `json:"sessions_observed"`  // ISO dates
}

// TypicalWorkHours represents typical work time windows
type TypicalWorkHours struct {
	WeekdayStart   string `json:"weekday_start"`
	WeekdayEnd     string `json:"weekday_end"`
	WeekendPattern string `json:"weekend_pattern"`
}

// SessionDurations represents categorized session durations
type SessionDurations struct {
	QuickCheck string `json:"quick_check"`
	NormalWork string `json:"normal_work"`
	DeepWork   string `json:"deep_work"`
}

// CircadianAwareness represents circadian patterns
type CircadianAwareness struct {
	SeanjeTypicalHours string   `json:"seanje_typical_hours"`
	DowntimeWindows    []string `json:"downtime_windows"`
	HighFocusTimes     []string `json:"high_focus_times"`
}

// FlowStates tracks flow state patterns
type FlowStates struct {
	Occurrences int      `json:"occurrences"`
	Contexts    []string `json:"contexts"`
	Description string   `json:"description"`
	Triggers    []string `json:"triggers"`
}

// QualityDips tracks quality decline patterns
type QualityDips struct {
	Occurrences              int      `json:"occurrences"`
	Timing                   []string `json:"timing"`
	Description              string   `json:"description"`
	TypicalDurationBeforeDip *int     `json:"typical_duration_before_dip"`
}

// StatePatterns tracks flow states and quality dips
type StatePatterns struct {
	FlowStates  FlowStates  `json:"flow_states"`
	QualityDips QualityDips `json:"quality_dips"`
}

// PatternData is the root structure of patterns.json
type PatternData struct {
	LastUpdated          string                      `json:"last_updated"`
	TotalSessions        int                         `json:"total_sessions"`
	TypicalWorkHours     *TypicalWorkHours           `json:"typical_work_hours,omitempty"`
	SessionDurations     *SessionDurations           `json:"session_durations,omitempty"`
	TimeOfDayQuality     map[string]string           `json:"time_of_day_quality,omitempty"`
	NaturalStoppingPoints []string                   `json:"natural_stopping_points,omitempty"`
	CircadianAwareness   *CircadianAwareness         `json:"circadian_awareness,omitempty"`
	CognitivePatterns    map[string]PatternEntry     `json:"cognitive_patterns,omitempty"`
	IdentityPatterns     map[string]PatternEntry     `json:"identity_patterns,omitempty"`
	RelationalPatterns   map[string]PatternEntry     `json:"relational_patterns,omitempty"`
	StatePatterns        *StatePatterns              `json:"state_patterns,omitempty"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Core Functions
// ═══════════════════════════════════════════════════════════════════════════

// GetPatternsPath returns the absolute path to patterns.json in session directory
// Loads path from paths.toml for config-driven operation
//
// Returns: Absolute path to patterns.json
func GetPatternsPath() string {
	// Load path from config (paths.toml)
	path, err := config.GetSessionPatternsPath()
	if err != nil {
		// Fallback to hardcoded path if config unavailable
		homeDir, _ := os.UserHomeDir()
		if homeDir == "" {
			homeDir = filepath.Join("/home", os.Getenv("USER"))
		}
		return filepath.Join(homeDir, ".claude/cpi-si/system/data/session/patterns.json")
	}
	return path
}

// ReadPatterns reads patterns.json and returns parsed data
//
// Returns nil if file doesn't exist or cannot be read.
//
// Health Scoring:
//   Success: +40 points (file read and parsed)
//   Failure: -40 points (cannot access pattern memory)
func ReadPatterns() (*PatternData, error) {
	patternsPath := GetPatternsPath()

	data, err := os.ReadFile(patternsPath)
	if err != nil {
		// Health: -40 (cannot access patterns)
		return nil, fmt.Errorf("failed to read patterns file: %w", err)
	}

	var patterns PatternData
	if err := json.Unmarshal(data, &patterns); err != nil {
		// Health: -40 (cannot parse patterns)
		return nil, fmt.Errorf("failed to parse patterns JSON: %w", err)
	}

	// Health: +40 (successful read and parse)
	return &patterns, nil
}

// WritePatterns writes pattern data to patterns.json with proper formatting
//
// Creates directory if it doesn't exist.
//
// Health Scoring:
//   Success: +40 points (patterns persisted)
//   Failure: -40 points (learning lost)
func WritePatterns(patterns *PatternData) error {
	patternsPath := GetPatternsPath()

	// Ensure directory exists
	dir := filepath.Dir(patternsPath)
	if err := os.MkdirAll(dir, 0755); err != nil {
		// Health: -40 (cannot create directory)
		return fmt.Errorf("failed to create patterns directory: %w", err)
	}

	// Marshal with pretty formatting
	data, err := json.MarshalIndent(patterns, "", "  ")
	if err != nil {
		// Health: -40 (cannot marshal)
		return fmt.Errorf("failed to marshal patterns: %w", err)
	}

	// Write to file
	if err := os.WriteFile(patternsPath, data, 0644); err != nil {
		// Health: -40 (write failed, learning lost)
		return fmt.Errorf("failed to write patterns file: %w", err)
	}

	// Health: +40 (successful write)
	return nil
}

// PatternCategory represents valid pattern categories
type PatternCategory string

const (
	CognitivePatterns   PatternCategory = "cognitive_patterns"
	IdentityPatterns    PatternCategory = "identity_patterns"
	RelationalPatterns  PatternCategory = "relational_patterns"
)

// UpdatePatternEntry updates or creates a pattern entry in specified category
//
// Handles occurrence counting, status updates, and session tracking.
//
// Health Scoring:
//   Success: +10 points (pattern tracked correctly)
//   Failure: -10 points (pattern tracking corrupted)
func UpdatePatternEntry(category PatternCategory, key string, updates PatternEntry) error {
	patterns, err := ReadPatterns()
	if err != nil {
		// Health: -10 (cannot update if can't read)
		return fmt.Errorf("failed to read patterns: %w", err)
	}

	// Initialize category maps if nil
	if patterns.CognitivePatterns == nil {
		patterns.CognitivePatterns = make(map[string]PatternEntry)
	}
	if patterns.IdentityPatterns == nil {
		patterns.IdentityPatterns = make(map[string]PatternEntry)
	}
	if patterns.RelationalPatterns == nil {
		patterns.RelationalPatterns = make(map[string]PatternEntry)
	}

	// Get the appropriate category map
	var categoryMap map[string]PatternEntry
	switch category {
	case CognitivePatterns:
		categoryMap = patterns.CognitivePatterns
	case IdentityPatterns:
		categoryMap = patterns.IdentityPatterns
	case RelationalPatterns:
		categoryMap = patterns.RelationalPatterns
	default:
		return fmt.Errorf("invalid pattern category: %s", category)
	}

	today := time.Now().Format("2006-01-02")

	// Check if pattern exists
	existing, exists := categoryMap[key]

	if !exists {
		// Create new pattern entry
		categoryMap[key] = PatternEntry{
			Occurrences:      1,
			FirstNoticed:     today,
			LastOccurred:     today,
			Contexts:         updates.Contexts,
			Status:           FirstObservation,
			Description:      updates.Description,
			SessionsObserved: []string{today},
		}
	} else {
		// Update existing pattern
		existing.Occurrences++
		existing.LastOccurred = today

		// Merge contexts
		if len(updates.Contexts) > 0 {
			contextSet := make(map[string]bool)
			for _, ctx := range existing.Contexts {
				contextSet[ctx] = true
			}
			for _, ctx := range updates.Contexts {
				if !contextSet[ctx] {
					existing.Contexts = append(existing.Contexts, ctx)
				}
			}
		}

		// Add session if not already tracked
		sessionExists := false
		for _, session := range existing.SessionsObserved {
			if session == today {
				sessionExists = true
				break
			}
		}
		if !sessionExists {
			existing.SessionsObserved = append(existing.SessionsObserved, today)
		}

		// Update status based on occurrences
		if existing.Occurrences >= 3 {
			existing.Status = ConfirmedThirdOccurrence
		} else if existing.Occurrences == 2 {
			existing.Status = SecondObservation
		}

		// Update description if provided
		if updates.Description != "" {
			existing.Description = updates.Description
		}

		categoryMap[key] = existing
	}

	// Update last_updated timestamp
	patterns.LastUpdated = time.Now().Format(time.RFC3339)

	// Write back
	if err := WritePatterns(patterns); err != nil {
		// Health: -10 (write failed)
		return fmt.Errorf("failed to write updated patterns: %w", err)
	}

	// Health: +10 (success)
	return nil
}

// GetPatternStatus retrieves a specific pattern entry from patterns.json
func GetPatternStatus(category PatternCategory, key string) (*PatternEntry, error) {
	patterns, err := ReadPatterns()
	if err != nil {
		return nil, fmt.Errorf("failed to read patterns: %w", err)
	}

	var categoryMap map[string]PatternEntry
	switch category {
	case CognitivePatterns:
		categoryMap = patterns.CognitivePatterns
	case IdentityPatterns:
		categoryMap = patterns.IdentityPatterns
	case RelationalPatterns:
		categoryMap = patterns.RelationalPatterns
	default:
		return nil, fmt.Errorf("invalid pattern category: %s", category)
	}

	if categoryMap == nil {
		return nil, fmt.Errorf("pattern not found: %s/%s", category, key)
	}

	entry, exists := categoryMap[key]
	if !exists {
		return nil, fmt.Errorf("pattern not found: %s/%s", category, key)
	}

	return &entry, nil
}

// InitializePatterns creates patterns.json with default structure if it doesn't exist
func InitializePatterns() error {
	// Check if already exists
	if _, err := ReadPatterns(); err == nil {
		return nil // Already exists
	}

	defaultPatterns := &PatternData{
		LastUpdated:        time.Now().Format(time.RFC3339),
		TotalSessions:      0,
		CognitivePatterns:  make(map[string]PatternEntry),
		IdentityPatterns:   make(map[string]PatternEntry),
		RelationalPatterns: make(map[string]PatternEntry),
		StatePatterns: &StatePatterns{
			FlowStates: FlowStates{
				Occurrences: 0,
				Contexts:    []string{},
				Description: "Deep engagement where time disappears, quality stays high",
				Triggers:    []string{},
			},
			QualityDips: QualityDips{
				Occurrences:              0,
				Timing:                   []string{},
				Description:              "When work quality starts declining",
				TypicalDurationBeforeDip: nil,
			},
		},
	}

	return WritePatterns(defaultPatterns)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING - Package Documentation
// ═══════════════════════════════════════════════════════════════════════════

/*
Package patterns provides shared functionality for reading and writing patterns.json.

Exported Types:
  - PatternEntry: Individual pattern with occurrence tracking
  - PatternData: Complete pattern file structure
  - PatternCategory: Valid pattern categories
  - PatternStatus: Pattern status constants

Exported Functions:
  - GetPatternsPath(): Get path to patterns.json
  - ReadPatterns(): Read and parse patterns.json
  - WritePatterns(): Write patterns.json with formatting
  - UpdatePatternEntry(): Update or create pattern entry
  - GetPatternStatus(): Retrieve specific pattern
  - InitializePatterns(): Create default patterns file

Usage:
  import "github.com/cpi-si/patterns"

  patterns, err := patterns.ReadPatterns()
  if err != nil {
      // Handle error
  }

Health Summary:
  This library forms the foundation of pattern memory.
  If read/write fails (-80 points), the entire learning system degrades.
  Proper error handling ensures graceful degradation.
*/
