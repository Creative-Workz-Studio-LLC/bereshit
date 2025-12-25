// ============================================================================
// METADATA
// ============================================================================
// Health Scoring System - Logging Library
//
// Biblical Foundation
//
// Scripture: "And ye shall know the truth, and the truth shall make you free" (John 8:32, KJV)
// Principle: Honest assessment reveals reality. Health scoring provides truthful visibility into system state without deception.
// Anchor: Health scores are honest discernment - measuring actual execution quality against perfect standard (Base100 algorithm).
//
// CPI-SI Identity
//
// Component Type: Health scoring module within Rails infrastructure
// Role: Calculate, normalize, and visualize component health scores
// Paradigm: CPI-SI framework component
//
// Authorship & Lineage
//
// Architect: Seanje Lenox-Wise, Nova Dawn
// Implementation: Nova Dawn
// Creation Date: 2025-11-18
// Version: 1.0.0
// Last Modified: 2025-11-18 - Extracted from monolithic logger.go
//
// Purpose & Function
//
// Purpose: Provide health calculation, normalization, and visual indicators for logging system. Converts raw health deltas into normalized scores with emoji indicators showing system state.
//
// Core Design: Base100 scoring with visual indicators - all actions total 100 points, actual execution = sum of results, visual feedback via emoji ranges.
//
// Key Features:
//   - Health clamping to valid range (-100 to +100)
//   - Visual emoji indicators based on configurable thresholds
//   - Progress bar visualization of health state
//   - Normalized health calculation across components
//   - Health delta tracking and accumulation
//
// Blocking Status
//
// Non-blocking: Health calculation failures degrade gracefully - use default emoji on config unavailable.
// Mitigation: All health operations have sensible defaults that allow logging to continue.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/logging"
//
// Integration Pattern:
//   1. Logger calls updateHealth(delta) to modify current health
//   2. calculateNormalizedHealth() ensures health stays within valid range
//   3. getHealthIndicator() provides visual emoji for display
//   4. getHealthBar() creates ASCII progress bar visualization
//
// Public API:
//
//   updateHealth(delta int) *Logger - Modify logger health by delta value
//   calculateNormalizedHealth() *Logger - Ensure health within valid range
//   getHealthIndicator(health int) string - Get emoji for health value
//   getHealthBar(health int) string - Get ASCII bar visualization
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: fmt, strings
//   Package Files: config.go (for Config.Health.Ranges)
//
// Dependents (What Uses This):
//   Internal: logger.go (all logging methods use health scoring)
//
// Health Scoring
//
// Base100 scoring algorithm (CPSI-ALG-001).
//
// Health Calculation (100 pts):
//   - Clamping operation: +5 (within range), 0 (required clamping)
//   - Indicator lookup: +10 (config available), +5 (using defaults)
//   - Health bar generation: +5 (successful rendering)
//   - Normalization: +10 (health updated correctly)
//   - Delta application: +10 (health modified correctly)
//
// Note: This module's health is about calculation accuracy, not score values themselves.

package logging

// ============================================================================
// SETUP
// ============================================================================

// Imports

import (
	"fmt"     // String formatting for health bar rendering
	"strings" // String manipulation for bar construction
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helpers - Foundation Functions
// ────────────────────────────────────────────────────────────────

// clampHealth ensures health value stays within valid range (-100 to +100).
//
// Health must stay within Base100 bounds: -100 (complete failure) to +100 (perfect execution).
// Prevents health from exceeding maximum or minimum valid scores.
func clampHealth(value int) int {
	if value > 100 { // Exceeds maximum health
		return 100 // Cap at perfect score
	}
	if value < -100 { // Below minimum health
		return -100 // Cap at complete failure
	}
	return value // Within valid range, return unchanged
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Health Calculation and Visualization
// ────────────────────────────────────────────────────────────────

// getHealthIndicator returns a visual emoji symbol for health status (e.g., 💚, ❤️, ☠️).
//
// Uses config.Health.Ranges to map health scores to visual indicators.
// Ranges are checked in descending threshold order (highest to lowest).
func getHealthIndicator(health int) string {
	// Ensure config is loaded
	LoadConfig()

	// Iterate through health ranges in descending threshold order
	for _, healthRange := range Config.Health.Ranges { // Check each range from high to low
		if health >= healthRange.Threshold { // First threshold match
			return healthRange.Emoji // Return corresponding emoji
		}
	}

	// Fallback if no range matches (shouldn't happen with properly configured ranges)
	return "❓" // Unknown health indicator
}

// getHealthBar generates an ASCII progress bar showing health visually.
//
// Creates a bar like: [████████████████████░░░░░░░░░░░░░░░░░░░░] (55/100)
// Filled portion represents current health relative to maximum.
func getHealthBar(health int) string {
	// Bar configuration
	const barWidth = 40                               // Total width in characters
	clamped := clampHealth(health)                    // Ensure within valid range
	normalizedHealth := (clamped + 100) / 2           // Convert -100..+100 to 0..100 range
	filledWidth := (normalizedHealth * barWidth) / 100 // Calculate filled portion

	// Build bar components
	filled := strings.Repeat("█", filledWidth)               // Filled portion (solid blocks)
	empty := strings.Repeat("░", barWidth-filledWidth)       // Empty portion (light blocks)
	return fmt.Sprintf("[%s%s] (%d/100)", filled, empty, normalizedHealth) // Formatted bar with value
}

// ────────────────────────────────────────────────────────────────
// Logger Methods - Health Management
// ────────────────────────────────────────────────────────────────

// calculateNormalizedHealth computes the normalized health percentage.
//
// Calculates percentage as (SessionHealth / TotalPossibleHealth) * 100.
// If total not declared, uses SessionHealth directly (clamped to valid range).
func (l *Logger) calculateNormalizedHealth() {
	// If total possible is 0 or unknown, normalized = raw cumulative (clamped)
	if l.TotalPossibleHealth == 0 {                   // Total not declared
		l.NormalizedHealth = clampHealth(l.SessionHealth)  // Use raw as normalized (clamped)
		return                                        // Exit early
	}

	// Calculate percentage: (cumulative / total_possible) * 100
	l.NormalizedHealth = (l.SessionHealth * 100) / l.TotalPossibleHealth  // Percentage calculation

	// Clamp to valid -100..+100 range
	l.NormalizedHealth = clampHealth(l.NormalizedHealth)  // Apply bounds
}

// updateHealth updates session health and recalculates normalization.
//
// Adds delta to SessionHealth (raw cumulative), then recalculates NormalizedHealth.
// SessionHealth is NOT clamped - it's the raw cumulative total. Only NormalizedHealth gets clamped.
func (l *Logger) updateHealth(delta int) {
	l.SessionHealth += delta                          // Apply health delta to raw cumulative
	// NOTE: SessionHealth is NOT clamped - it's the raw cumulative total
	// Only NormalizedHealth gets clamped during calculation

	// Recalculate normalized percentage (applies clamping there)
	l.calculateNormalizedHealth()                     // Update percentage based on new raw value
}

// ============================================================================
// CLOSING
// ============================================================================
// Library module (no entry point). Import: "system/runtime/lib/logging"
//
// ============================================================================
// END CLOSING
// ============================================================================
