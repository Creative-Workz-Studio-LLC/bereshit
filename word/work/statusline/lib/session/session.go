// ============================================================================
// METADATA - Session Statistics Display
// ============================================================================
// Purpose: Session metrics formatting (lines, cost, duration)
// Biblical: Luke 14:28 - "For which of you, intending to build a tower, sitteth not down first, and counteth the cost"

package session

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"creativeworkzstudio.com/bereshit/word/work/pkg/util/term/display"
	"creativeworkzstudio.com/bereshit/word/work/pkg/foundation/types"
)

// LinesDisplay holds lines modified info
type LinesDisplay struct {
	TotalLines int
	Added      int
	Removed    int
	Icon       string
	Color      string
	HasInfo    bool
}

// DurationDisplay holds duration info
type DurationDisplay struct {
	Duration string
	Icon     string
	Color    string
	HasInfo  bool
}

// CostDisplay holds cost info
type CostDisplay struct {
	Cost    float64
	Icon    string
	Color   string
	HasInfo bool
}

// ============================================================================
// BODY
// ============================================================================

// GetLinesModifiedDisplay returns lines changed display
func GetLinesModifiedDisplay(ctx types.SessionContext) LinesDisplay {
	added := ctx.Cost.TotalLinesAdded
	removed := ctx.Cost.TotalLinesRemoved
	total := added + removed

	if total == 0 {
		return LinesDisplay{HasInfo: false}
	}

	color := display.Green
	if removed > added {
		color = display.Red
	}

	return LinesDisplay{
		TotalLines: total,
		Added:      added,
		Removed:    removed,
		Icon:       "📝",
		Color:      color,
		HasInfo:    true,
	}
}

// GetDurationDisplay returns session duration display
func GetDurationDisplay(ctx types.SessionContext) DurationDisplay {
	ms := ctx.Cost.TotalDurationMS
	if ms == 0 {
		return DurationDisplay{HasInfo: false}
	}

	return DurationDisplay{
		Duration: formatDuration(ms),
		Icon:     "⏱️",
		Color:    display.Cyan,
		HasInfo:  true,
	}
}

// GetCostDisplay returns cost display
func GetCostDisplay(ctx types.SessionContext) CostDisplay {
	cost := ctx.Cost.TotalCostUSD
	if cost == 0 {
		return CostDisplay{HasInfo: false}
	}

	color := display.Green
	if cost > 1.0 {
		color = display.Yellow
	}
	if cost > 5.0 {
		color = display.Red
	}

	return CostDisplay{
		Cost:    cost,
		Icon:    "💰",
		Color:   color,
		HasInfo: true,
	}
}

// GetFormattedCost formats cost as string
func GetFormattedCost(cost float64) string {
	if cost < 0.01 {
		return "<$0.01"
	}
	if cost < 1.0 {
		return fmt.Sprintf("$%.2f", cost)
	}
	return fmt.Sprintf("$%.2f", cost)
}

func formatDuration(ms int) string {
	if ms < 1000 {
		return "<1s"
	}

	seconds := ms / 1000
	minutes := seconds / 60
	hours := minutes / 60

	var parts []string
	if hours > 0 {
		parts = append(parts, fmt.Sprintf("%dh", hours))
	}
	if minutes%60 > 0 || hours > 0 {
		parts = append(parts, fmt.Sprintf("%dm", minutes%60))
	}
	if len(parts) == 0 {
		parts = append(parts, fmt.Sprintf("%ds", seconds))
	}

	return strings.Join(parts, " ")
}

// ============================================================================
// CLOSING
// ============================================================================
