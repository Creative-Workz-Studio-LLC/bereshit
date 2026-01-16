// ============================================================================
// METADATA
// ============================================================================
// Package: sections/temporal
// Purpose: WHEN section - Time awareness display
// Biblical: "To every thing there is a season" - Ecclesiastes 3:1

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"time"

	"github.com/creativeworkzstudio/claude-global/pkg/util/term/display"
	"github.com/creativeworkzstudio/claude-global/pkg/util/term/temporal"
)

// ============================================================================
// BODY
// ============================================================================

// BuildTemporal returns the WHEN section (time awareness)
//
// Format: 🌙 Dec 21 18:30 evening
func BuildTemporal(now time.Time) SectionResult {
	dateTime := now.Format("Jan 02 15:04")
	timeOfDay := temporal.GetTimeOfDayDisplay(now)

	var content string
	if timeOfDay.HasInfo {
		content = fmt.Sprintf("%s%s %s %s%s",
			timeOfDay.Color, timeOfDay.Icon, dateTime, timeOfDay.Label, display.Reset)
	} else {
		content = fmt.Sprintf("%s🕐 %s%s",
			display.Cyan, dateTime, display.Reset)
	}

	return New(content, 3)
}

// BuildTemporalCompact returns just the time
func BuildTemporalCompact(now time.Time) SectionResult {
	content := fmt.Sprintf("%s%s%s",
		display.Cyan, now.Format("15:04"), display.Reset)
	return New(content, 3)
}

// ============================================================================
// CLOSING
// ============================================================================
