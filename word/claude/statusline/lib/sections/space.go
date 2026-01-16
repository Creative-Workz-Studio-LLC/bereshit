// ============================================================================
// METADATA
// ============================================================================
// Package: sections/space
// Purpose: SPACE section - Directory and git awareness
// Biblical: "The earth is the LORD's, and the fulness thereof" - Psalm 24:1

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"github.com/creativeworkzstudio/claude-global/pkg/util/term/display"
	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/format"
	"github.com/creativeworkzstudio/claude-global/pkg/util/vcs/git"
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
)

// ============================================================================
// BODY
// ============================================================================

// BuildSpace returns the SPACE section (directory + git)
//
// Format: 📂 ~/project 🌿 main
func BuildSpace(ctx types.SessionContext) SectionResult {
	workdir := ctx.CWD
	if workdir == "" {
		workdir = ctx.Workspace.CurrentDir
	}
	if workdir == "" {
		workdir = ctx.Workspace.ProjectDir
	}

	if workdir == "" {
		return Empty()
	}

	var parts []string

	// Directory
	workdirShort := format.ShortenPath(workdir)
	parts = append(parts, fmt.Sprintf("%s📂 %s%s",
		display.Blue, workdirShort, display.Reset))

	// Git info
	gitDisplay := git.GetDisplay(workdir)
	if gitDisplay.HasInfo {
		parts = append(parts, fmt.Sprintf("%s%s %s%s",
			gitDisplay.Color, gitDisplay.Icon, gitDisplay.DisplayString, display.Reset))
	}

	return New(strings.Join(parts, " "), 4)
}

// BuildSpaceCompact returns just the directory basename
func BuildSpaceCompact(ctx types.SessionContext) SectionResult {
	workdir := ctx.CWD
	if workdir == "" {
		workdir = ctx.Workspace.CurrentDir
	}
	if workdir == "" {
		return Empty()
	}

	// Just the last component
	parts := strings.Split(workdir, "/")
	basename := parts[len(parts)-1]
	if basename == "" && len(parts) > 1 {
		basename = parts[len(parts)-2]
	}

	content := fmt.Sprintf("%s📂 %s%s",
		display.Blue, basename, display.Reset)
	return New(content, 4)
}

// ============================================================================
// CLOSING
// ============================================================================
