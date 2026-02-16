//omni:code --go -library
//omni:key B-L3-statemachine-patterns
//omni:version a-01.00
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-statemachine-patterns
// Purpose: Pattern detection and application for learned behaviors
// Biblical: Proverbs 22:6 - "Train up a child in the way he should go"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-23
//
// Design: Detects patterns from historical data and applies them to
//   inform session behavior. Patterns include:
//   - Temporal patterns (work hours, session duration)
//   - Choice patterns (tool preferences, K:ALIGN trends)
//   - State patterns (Hebrew state transitions)
//
// ═══════════════════════════════════════════════════════════════════════════

package statemachine

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
)

// ───────────────────────────────────────────────────────────────────────────
// TYPES
// ───────────────────────────────────────────────────────────────────────────

// PatternType identifies the kind of pattern detected
type PatternType string

const (
	PatternTypeTemporal PatternType = "temporal"
	PatternTypeChoice   PatternType = "choice"
	PatternTypeState    PatternType = "state"
	PatternTypeTool     PatternType = "tool"
)

// DetectedPattern represents a pattern found in historical data
type DetectedPattern struct {
	Type        PatternType // What kind of pattern
	Key         string      // Unique identifier
	Description string      // Human-readable description
	Confidence  float64     // 0.0 to 1.0 confidence level
	DataPoints  int         // How many data points support this
	Actionable  bool        // Whether this pattern should affect behavior
	Data        interface{} // Pattern-specific data
}

// PatternDetector analyzes historical data for patterns
type PatternDetector struct {
	bridge DatabaseBridge
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// PATTERN DETECTOR
// ───────────────────────────────────────────────────────────────────────────

// NewPatternDetector creates a detector with database access
func NewPatternDetector(bridge DatabaseBridge) *PatternDetector {
	return &PatternDetector{bridge: bridge}
}

// DetectAll runs all pattern detection algorithms
func (pd *PatternDetector) DetectAll(ctx context.Context) ([]DetectedPattern, error) {
	if pd.bridge == nil {
		return nil, nil
	}

	var patterns []DetectedPattern

	// Detect temporal patterns (work hours)
	if temporal, err := pd.detectTemporalPatterns(ctx); err == nil {
		patterns = append(patterns, temporal...)
	}

	// Detect choice patterns (K:ALIGN trends)
	if choice, err := pd.detectChoicePatterns(ctx); err == nil {
		patterns = append(patterns, choice...)
	}

	return patterns, nil
}

// detectTemporalPatterns finds patterns in when work happens
func (pd *PatternDetector) detectTemporalPatterns(ctx context.Context) ([]DetectedPattern, error) {
	repo := pd.bridge.GetRepository()
	if repo == nil {
		return nil, nil
	}

	// Get work hour patterns
	workPatterns, err := repo.GetTemporalWorkPatterns(ctx)
	if err != nil || workPatterns == nil {
		return nil, err
	}

	var patterns []DetectedPattern

	// Analyze for common work hours
	// If we have enough data, detect preferred hours
	totalSessions := 0
	for _, hours := range workPatterns {
		for _, count := range hours {
			totalSessions += count
		}
	}

	if totalSessions > 5 {
		patterns = append(patterns, DetectedPattern{
			Type:        PatternTypeTemporal,
			Key:         "work_hours",
			Description: "Detected work hour preferences",
			Confidence:  float64(totalSessions) / 20.0, // Scale confidence
			DataPoints:  totalSessions,
			Actionable:  true,
			Data:        workPatterns,
		})
	}

	return patterns, nil
}

// detectChoicePatterns finds patterns in choice behavior
func (pd *PatternDetector) detectChoicePatterns(ctx context.Context) ([]DetectedPattern, error) {
	// For now, return empty - this can be expanded
	// to analyze K:ALIGN trends and tool preferences
	return nil, nil
}

// ───────────────────────────────────────────────────────────────────────────
// PATTERN APPLICATION
// ───────────────────────────────────────────────────────────────────────────

// ApplyPatterns modifies runtime state based on detected patterns
func ApplyPatterns(patterns []DetectedPattern, state *RuntimeState) {
	if state == nil {
		return
	}

	for _, p := range patterns {
		if !p.Actionable || p.Confidence < 0.3 {
			continue
		}

		switch p.Type {
		case PatternTypeTemporal:
			// Temporal patterns might adjust session behavior
			// For now, just log awareness - future: adjust pacing

		case PatternTypeChoice:
			// Choice patterns might inform K:ALIGN starting point
			// Future: pre-weight certain tool categories

		case PatternTypeState:
			// State patterns might influence Hebrew state transitions
			// Future: adjust transition probabilities
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Pattern detection enables learning across sessions.
// As more data accumulates, patterns become more confident and actionable.
//
// Related: runtime_db.go, runtime.go
//
