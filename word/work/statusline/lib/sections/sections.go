// ============================================================================
// METADATA
// ============================================================================
// Package: sections
// Purpose: Section builders for statusline display
// Biblical: "Look that thou make them after their pattern" - Exodus 25:40
//
// Each section returns a SectionResult that can be composed into the final
// statusline. Sections are building blocks - this is +1 (build up) work.

package sections

// ============================================================================
// SETUP
// ============================================================================

// SectionResult contains the output of a section builder
type SectionResult struct {
	Content  string // Formatted string with colors
	HasInfo  bool   // Whether this section has meaningful content
	Priority int    // For future: which sections to show when space limited (1=highest)
}

// SectionSeparator is the operator used between sections
const (
	SepPipe  = " | " // Standard separator
	SepArrow = " → " // Flow separator
	SepAt    = " @ " // Position separator
)

// ============================================================================
// BODY
// ============================================================================

// Empty returns an empty section result (for optional sections)
func Empty() SectionResult {
	return SectionResult{HasInfo: false}
}

// New creates a new section result with content
func New(content string, priority int) SectionResult {
	return SectionResult{
		Content:  content,
		HasInfo:  content != "",
		Priority: priority,
	}
}

// NewOptional creates a section result that may or may not have content
func NewOptional(content string, hasInfo bool, priority int) SectionResult {
	return SectionResult{
		Content:  content,
		HasInfo:  hasInfo,
		Priority: priority,
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: identity.go, state.go, temporal.go, space.go, depth.go, health.go, dynamic.go
