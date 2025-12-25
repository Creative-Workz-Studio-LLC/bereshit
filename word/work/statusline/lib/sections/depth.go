// ============================================================================
// METADATA
// ============================================================================
// Package: sections/depth
// Purpose: DEPTH section - Context window and system metrics
// Biblical: "Deep calleth unto deep" - Psalm 42:7

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"creativeworkzstudio.com/bereshit/word/work/pkg/util/term/display"
	"creativeworkzstudio.com/bereshit/word/work/pkg/util/term/system"
	"creativeworkzstudio.com/bereshit/word/work/pkg/foundation/types"
	"creativeworkzstudio.com/bereshit/word/work/statusline/lib/features"
)

// ============================================================================
// BODY
// ============================================================================

// BuildDepth returns the DEPTH section (context + cache + system)
//
// Format: Z: 📊 45K/200K (22%) Δ+15%
func BuildDepth(ctx types.SessionContext) SectionResult {
	var parts []string

	// Context Window (primary depth indicator)
	ctxPct := ctx.ContextPercentage()
	if ctx.ContextWindow.ContextWindowSize > 0 {
		// Color based on usage: green < 50%, yellow 50-80%, red > 80%
		ctxColor := display.Green
		if ctxPct >= 80 {
			ctxColor = display.Red
		} else if ctxPct >= 50 {
			ctxColor = display.Yellow
		}

		// Format tokens in K
		tokens := ctx.CurrentContextTokens()
		if tokens == 0 {
			tokens = ctx.ContextWindow.TotalInputTokens + ctx.ContextWindow.TotalOutputTokens
		}
		tokensK := float64(tokens) / 1000
		maxK := float64(ctx.ContextWindow.ContextWindowSize) / 1000

		parts = append(parts, fmt.Sprintf("%s📊 %.0fK/%.0fK (%.0f%%)%s",
			ctxColor, tokensK, maxK, ctxPct, display.Reset))
	}

	// Cache Efficiency (ternary scale: -100 to +100)
	cacheEff := ctx.CacheEfficiency()
	if cacheEff != 0 || (ctx.ContextWindow.CurrentUsage.InputTokens > 0 || ctx.ContextWindow.CurrentUsage.CacheReadInputTokens > 0) {
		// Color: Yellow (building) → Cyan (balanced) → Green (efficient)
		cacheColor := display.Cyan // Anchor state (balanced)
		if cacheEff > 20 {
			cacheColor = display.Green // Positive: efficient reuse
		} else if cacheEff < -20 {
			cacheColor = display.Yellow // Negative: building foundation
		}
		// Show sign for ternary clarity (+/-)
		sign := ""
		if cacheEff > 0 {
			sign = "+"
		}
		parts = append(parts, fmt.Sprintf("%sΔ%s%.0f%%%s",
			cacheColor, sign, cacheEff, display.Reset))
	}

	// System Health (secondary - only if enabled)
	if features.ShouldShowSystemHealth() {
		loadDisplay := system.GetLoadDisplay()
		if loadDisplay.HasInfo {
			parts = append(parts, fmt.Sprintf("%s⚡%.1f%s",
				loadDisplay.Color, loadDisplay.LoadAvg, display.Reset))
		}

		memDisplay := system.GetMemoryDisplay()
		if memDisplay.HasInfo {
			parts = append(parts, fmt.Sprintf("%s💾%.0f%%%s",
				memDisplay.Color, memDisplay.Percent, display.Reset))
		}
	}

	if len(parts) == 0 {
		return Empty()
	}

	content := fmt.Sprintf("%sZ:%s %s",
		display.Dim, display.Reset, strings.Join(parts, " "))

	return New(content, 5)
}

// BuildDepthCompact returns just context percentage
func BuildDepthCompact(ctx types.SessionContext) SectionResult {
	ctxPct := ctx.ContextPercentage()
	if ctx.ContextWindow.ContextWindowSize == 0 {
		return Empty()
	}

	ctxColor := display.Green
	if ctxPct >= 80 {
		ctxColor = display.Red
	} else if ctxPct >= 50 {
		ctxColor = display.Yellow
	}

	content := fmt.Sprintf("%s%.0f%%%s", ctxColor, ctxPct, display.Reset)
	return New(content, 5)
}

// ============================================================================
// CLOSING
// ============================================================================
