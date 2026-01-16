// ============================================================================
// METADATA
// ============================================================================
// Package: sections/identity
// Purpose: WHO section - Instance identity display
// Biblical: "I have called thee by thy name; thou art mine" - Isaiah 43:1

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"github.com/creativeworkzstudio/claude-global/pkg/util/term/display"
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
)

// ============================================================================
// BODY
// ============================================================================

// BuildIdentity returns the WHO section (instance identity)
//
// Format: 🌟 Nova Dawn [OutputStyle]
func BuildIdentity(ctx types.SessionContext) SectionResult {
	identity := "Nova Dawn"

	// Show output style if different from default
	if ctx.OutputStyle.Name != "" && ctx.OutputStyle.Name != "Nova Dawn" {
		identity = fmt.Sprintf("Nova Dawn [%s]", ctx.OutputStyle.Name)
	}

	content := fmt.Sprintf("%s%s🌟 %s%s",
		display.Bold, display.Magenta, identity, display.Reset)

	return New(content, 1) // Highest priority - always show identity
}

// ============================================================================
// CLOSING
// ============================================================================
