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

// DisplayMode controls how much detail to show based on available width
type DisplayMode int

const (
	DisplayFull    DisplayMode = iota // All details, unlimited width
	DisplayNormal                     // Standard display (~140 chars)
	DisplayCompact                    // Reduced details (~100 chars)
	DisplayMinimal                    // Essential only (~80 chars)
)

// ModeFromWidth returns the appropriate display mode for a terminal width
func ModeFromWidth(width int) DisplayMode {
	switch {
	case width >= 160:
		return DisplayFull
	case width >= 120:
		return DisplayNormal
	case width >= 100:
		return DisplayCompact
	default:
		return DisplayMinimal
	}
}

// SectionResult contains the output of a section builder
type SectionResult struct {
	Content        string // Formatted string with colors (full)
	CompactContent string // Shorter version for limited width
	HasInfo        bool   // Whether this section has meaningful content
	Priority       int    // Which sections to show when space limited (1=highest)
	Width          int    // Visual width of Content (excluding ANSI codes)
	CompactWidth   int    // Visual width of CompactContent
}

// GetContent returns appropriate content for the display mode
func (s SectionResult) GetContent(mode DisplayMode) string {
	if mode >= DisplayCompact && s.CompactContent != "" {
		return s.CompactContent
	}
	return s.Content
}

// GetWidth returns appropriate width for the display mode
func (s SectionResult) GetWidth(mode DisplayMode) int {
	if mode >= DisplayCompact && s.CompactContent != "" {
		return s.CompactWidth
	}
	return s.Width
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
		Width:    VisualWidth(content),
	}
}

// NewWithCompact creates a section result with both full and compact versions
func NewWithCompact(content, compact string, priority int) SectionResult {
	return SectionResult{
		Content:        content,
		CompactContent: compact,
		HasInfo:        content != "",
		Priority:       priority,
		Width:          VisualWidth(content),
		CompactWidth:   VisualWidth(compact),
	}
}

// NewOptional creates a section result that may or may not have content
func NewOptional(content string, hasInfo bool, priority int) SectionResult {
	return SectionResult{
		Content:  content,
		HasInfo:  hasInfo,
		Priority: priority,
		Width:    VisualWidth(content),
	}
}

// VisualWidth calculates the visual width of a string (excluding ANSI codes)
func VisualWidth(s string) int {
	inEscape := false
	width := 0
	for _, r := range s {
		if r == '\x1b' {
			inEscape = true
			continue
		}
		if inEscape {
			if r == 'm' {
				inEscape = false
			}
			continue
		}
		// Most emojis are 2 columns wide
		if r >= 0x1F300 && r <= 0x1FAD6 {
			width += 2
		} else if r >= 0x2600 && r <= 0x26FF {
			width += 2
		} else {
			width++
		}
	}
	return width
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: identity.go, state.go, temporal.go, space.go, depth.go, health.go, dynamic.go
