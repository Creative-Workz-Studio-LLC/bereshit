// ============================================================================
// METADATA
// ============================================================================
// Package: sections/depth_enhanced
// Purpose: Enhanced DEPTH section - Effective context with compaction awareness
// Biblical: "A time to keep, and a time to cast away" - Ecclesiastes 3:6
//
// Shows effective working context (not flat 200K):
//   - Base overhead (~30K for CLAUDE.md, system prompts)
//   - Safety margin (~20K for compaction recovery)
//   - Effective working space = 200K - overhead - margin = ~150K
//
// Also shows compaction count and proximity warnings.

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
	"github.com/creativeworkzstudio/claude-global/pkg/util/term/display"
)

// ============================================================================
// BODY
// ============================================================================

// Context thresholds (from hooks/session/start.go)
const (
	baseOverheadTokens    = 30000
	safetyMarginTokens    = 20000
	totalContextWindow    = 200000
	effectiveContextLimit = totalContextWindow - baseOverheadTokens - safetyMarginTokens // 150K
)

// BuildDepthEnhanced returns the enhanced DEPTH section
//
// Format: 📊 45K/150K▐▌▌ [C2] (effective context + visual bar + compaction count)
func BuildDepthEnhanced(ctx types.SessionContext, runtime *statemachine.RuntimeState) SectionResult {
	var parts []string

	// Get context usage from Claude Code
	ctxPct := float64(ctx.ContextWindow.UsedPercentage)
	if ctxPct == 0 {
		ctxPct = ctx.ContextPercentage()
	}

	// Calculate tokens used
	tokens := ctx.ContextWindow.CurrentUsage.InputTokens +
		ctx.ContextWindow.CurrentUsage.OutputTokens +
		ctx.ContextWindow.CurrentUsage.CacheCreationInputTokens +
		ctx.ContextWindow.CurrentUsage.CacheReadInputTokens
	if tokens == 0 && ctx.ContextWindow.ContextWindowSize > 0 {
		tokens = int(ctxPct / 100 * float64(ctx.ContextWindow.ContextWindowSize))
	}

	// Calculate effective percentage (against 150K, not 200K)
	effectivePct := float64(0)
	if tokens > baseOverheadTokens {
		usedEffective := tokens - baseOverheadTokens
		effectivePct = float64(usedEffective) / float64(effectiveContextLimit) * 100
		if effectivePct > 100 {
			effectivePct = 100
		}
	}

	if tokens > 0 || ctx.ContextWindow.ContextWindowSize > 0 {
		// Color based on effective usage
		var ctxColor string
		switch {
		case effectivePct >= 90:
			ctxColor = display.Red // Critical - compaction imminent
		case effectivePct >= 75:
			ctxColor = display.Yellow // Warning
		case effectivePct >= 50:
			ctxColor = display.Cyan // Moderate
		default:
			ctxColor = display.Green // Healthy
		}

		// Visual progress bar (5 segments)
		bar := buildContextBar(effectivePct)

		// Calculate effective tokens used (minus base overhead)
		usedK := float64(max(0, tokens-baseOverheadTokens)) / 1000
		maxK := float64(effectiveContextLimit) / 1000

		parts = append(parts, fmt.Sprintf("%s📊 %.0fK/%.0fK%s%s",
			ctxColor, usedK, maxK, bar, display.Reset))
	}

	// Compaction count (from runtime state)
	if runtime != nil && runtime.Session.CompactionCount > 0 {
		compactColor := display.Dim
		if runtime.Session.CompactionCount >= 3 {
			compactColor = display.Yellow // Many compactions
		}
		parts = append(parts, fmt.Sprintf("%s[C%d]%s",
			compactColor, runtime.Session.CompactionCount, display.Reset))
	}

	// Peak indicator (if approaching peak)
	if runtime != nil && runtime.Session.PeakContextTokens > 0 {
		peakPct := float64(tokens) / float64(runtime.Session.PeakContextTokens) * 100
		if peakPct >= 80 {
			parts = append(parts, fmt.Sprintf("%s↑peak%s", display.Yellow, display.Reset))
		}
	}

	if len(parts) == 0 {
		return Empty()
	}

	content := strings.Join(parts, " ")
	return New(content, 5)
}

// buildContextBar creates a visual progress bar
//
// Returns: ▐▌▌▌▌ (filled/empty based on percentage)
func buildContextBar(pct float64) string {
	segments := 5
	filled := int(pct / 100 * float64(segments))
	if filled > segments {
		filled = segments
	}

	var bar strings.Builder
	for i := 0; i < segments; i++ {
		if i < filled {
			bar.WriteString("▐") // Filled
		} else {
			bar.WriteString("▌") // Empty
		}
	}
	return bar.String()
}

// BuildDepthCompactEnhanced returns a compact effective context indicator
//
// Format: 62%▐▐▌ (percentage + mini bar)
func BuildDepthCompactEnhanced(ctx types.SessionContext, runtime *statemachine.RuntimeState) SectionResult {
	ctxPct := float64(ctx.ContextWindow.UsedPercentage)
	if ctxPct == 0 {
		ctxPct = ctx.ContextPercentage()
	}

	tokens := ctx.ContextWindow.CurrentUsage.InputTokens +
		ctx.ContextWindow.CurrentUsage.OutputTokens +
		ctx.ContextWindow.CurrentUsage.CacheCreationInputTokens +
		ctx.ContextWindow.CurrentUsage.CacheReadInputTokens
	if tokens == 0 && ctx.ContextWindow.ContextWindowSize > 0 {
		tokens = int(ctxPct / 100 * float64(ctx.ContextWindow.ContextWindowSize))
	}

	// Calculate effective percentage
	effectivePct := float64(0)
	if tokens > baseOverheadTokens {
		usedEffective := tokens - baseOverheadTokens
		effectivePct = float64(usedEffective) / float64(effectiveContextLimit) * 100
		if effectivePct > 100 {
			effectivePct = 100
		}
	}

	if ctx.ContextWindow.ContextWindowSize == 0 {
		return Empty()
	}

	// Color
	var ctxColor string
	switch {
	case effectivePct >= 90:
		ctxColor = display.Red
	case effectivePct >= 75:
		ctxColor = display.Yellow
	case effectivePct >= 50:
		ctxColor = display.Cyan
	default:
		ctxColor = display.Green
	}

	// Mini bar (3 segments)
	bar := buildMiniBar(effectivePct)

	content := fmt.Sprintf("%s%.0f%%%s%s", ctxColor, effectivePct, bar, display.Reset)
	return New(content, 5)
}

func buildMiniBar(pct float64) string {
	segments := 3
	filled := int(pct / 100 * float64(segments))
	if filled > segments {
		filled = segments
	}

	var bar strings.Builder
	for i := 0; i < segments; i++ {
		if i < filled {
			bar.WriteString("▐")
		} else {
			bar.WriteString("▌")
		}
	}
	return bar.String()
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: depth.go (original depth section), momentum.go (momentum tracking)
//
// Enhanced depth tracking provides:
//   - Effective context (150K working space, not raw 200K)
//   - Compaction awareness (count and proximity)
//   - Visual progress indicators
//
// "A time to keep, and a time to cast away." — Ecclesiastes 3:6
