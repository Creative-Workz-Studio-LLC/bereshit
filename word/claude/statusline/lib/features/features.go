// ============================================================================
// METADATA - Feature Display Decisions
// ============================================================================
// Purpose: Conditional display logic (reminders, badges)
// Biblical: Proverbs 25:11 - "A word fitly spoken is like apples of gold"

package features

// ============================================================================
// SETUP
// ============================================================================

import (
	"hash/fnv"
)

// ============================================================================
// BODY
// ============================================================================

// ShouldShowReminder determines if Kingdom Technology reminder should show
// Uses session ID hash for consistent but occasional display
func ShouldShowReminder(sessionID string) bool {
	if sessionID == "" {
		return false
	}

	h := fnv.New32a()
	h.Write([]byte(sessionID))
	hash := h.Sum32()

	// Show reminder ~10% of the time
	return hash%10 == 0
}

// ShouldShowSystemHealth determines if system metrics should display
// Always show for now - could add conditions later
func ShouldShowSystemHealth() bool {
	return true
}

// ShouldShowCost determines if cost should display
func ShouldShowCost(cost float64) bool {
	return cost > 0.001
}

// ============================================================================
// CLOSING
// ============================================================================
