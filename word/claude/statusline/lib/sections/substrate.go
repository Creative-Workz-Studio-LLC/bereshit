// ============================================================================
// METADATA
// ============================================================================
// Package: sections/substrate
// Purpose: SUBSTRATE section - Model/processing layer display
// Biblical: "The body is not one member, but many" - 1 Corinthians 12:14

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"cws.studio/pkg/util/term/display"
	"cws.studio/pkg/util/pure/format"
	"cws.studio/pkg/foundation/types"
	"cws.studio/pkg/sdk/hookoutput"
)

// ============================================================================
// BODY
// ============================================================================

// BuildSubstrate returns the SUBSTRATE section (model info)
//
// Format: ♊ Gemini 2.0 Pro
func BuildSubstrate(ctx types.SessionContext) SectionResult {
	if ctx.Model.DisplayName == "" {
		return Empty()
	}

	modelName := format.GetShortModelName(ctx.Model.DisplayName)
	
	// Substrate awareness
	icon := "🧠"
	if hookoutput.IsGemini() {
		icon = "♊"
	} else {
		icon = "🛡️" // Claude
	}

	content := fmt.Sprintf("%s%s %s%s",
		display.Cyan, icon, modelName, display.Reset)

	return New(content, 4)
}

// BuildVersion returns the version section
//
// Format: v1.0.30
func BuildVersion(ctx types.SessionContext) SectionResult {
	if ctx.Version == "" {
		return Empty()
	}

	content := fmt.Sprintf("%sv%s%s",
		display.Dim+display.Gray, ctx.Version, display.Reset)

	return New(content, 9) // Lowest priority
}

// ============================================================================
// CLOSING
// ============================================================================
