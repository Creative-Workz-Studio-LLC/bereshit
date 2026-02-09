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
)

// ============================================================================
// BODY
// ============================================================================

// BuildSubstrate returns the SUBSTRATE section (model info)
//
// Format: 🧠 Opus 4.5
func BuildSubstrate(ctx types.SessionContext) SectionResult {
	if ctx.Model.DisplayName == "" {
		return Empty()
	}

	modelName := format.GetShortModelName(ctx.Model.DisplayName)
	content := fmt.Sprintf("%s🧠 %s%s",
		display.Cyan, modelName, display.Reset)

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
