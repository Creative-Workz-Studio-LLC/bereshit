// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-render-viewmodel
//omni:code  --go -package->render
//omni:version a-00.10
//
// Purpose: Toolkit-agnostic ViewModel types for the Universal Base GUI.
//          Panels declare what to show using these types.
//          Renderers decide how to display them.
// Biblical: Exodus 25:9 - "According to all that I shew thee, after the
//           pattern of the tabernacle"
//
// ============================================================================
// END METADATA

package render

// ============================================================================
// SETUP
// ============================================================================

// FieldStyle defines the visual emphasis of a field value.
type FieldStyle int

const (
	// StyleNormal is the default field style.
	StyleNormal FieldStyle = iota
	// StyleEmphasized highlights important values.
	StyleEmphasized
	// StyleMuted dims less important values.
	StyleMuted
	// StyleWarning indicates a warning condition.
	StyleWarning
	// StyleError indicates an error condition.
	StyleError
	// StyleSuccess indicates a positive condition.
	StyleSuccess
	// StyleCode renders the value in monospace.
	StyleCode
)

// LogLevel defines the severity of a log entry.
type LogLevel int

const (
	// LogDebug is for detailed diagnostic messages.
	LogDebug LogLevel = iota
	// LogInfo is for informational messages.
	LogInfo
	// LogWarn is for warning messages.
	LogWarn
	// LogError is for error messages.
	LogError
	// LogSuccess is for success messages.
	LogSuccess
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// View represents a complete panel that a module presents.
// A module can declare multiple Views (e.g., Overview, Analytics, System Data).
type View struct {
	// ID uniquely identifies this view within its module.
	ID string

	// Title is the human-readable name shown in navigation.
	Title string

	// Icon is an optional icon identifier (e.g., "dashboard", "sdk").
	Icon string

	// Sections contains the structured content of this view.
	Sections []Section
}

// Section is a titled group of fields, optionally with nested children.
type Section struct {
	// Title is the section header text.
	Title string

	// Fields contains the key-value pairs in this section.
	Fields []Field

	// Children contains nested sub-sections.
	Children []Section

	// Collapsible indicates whether this section can be collapsed.
	Collapsible bool

	// Collapsed indicates the default collapsed state.
	Collapsed bool
}

// Field is a single key-value display element.
type Field struct {
	// Key is the label (left side).
	Key string

	// Value is the display text (right side).
	Value string

	// Style controls the visual emphasis.
	Style FieldStyle

	// Progress is an optional progress bar for this field.
	Progress *Progress

	// Bar is an optional segmented colored bar.
	Bar *Bar

	// Tooltip provides additional context on hover.
	Tooltip string
}

// Progress represents a progress bar with a fraction and label.
type Progress struct {
	// Fraction is the fill amount from 0.0 to 1.0.
	Fraction float64

	// Label is the text shown on or near the bar.
	Label string

	// Style is a semantic hint for coloring (e.g., "health", "cpi", "tokens").
	Style string
}

// Bar represents a segmented colored bar (e.g., K:ALIGN distribution).
type Bar struct {
	// Segments contains the bar's colored portions.
	Segments []BarSegment
}

// BarSegment is one portion of a segmented bar.
type BarSegment struct {
	// Width is the proportional width (0.0 to 1.0).
	Width float64

	// Style is the CSS class or semantic style name.
	Style string

	// Label is optional text for this segment.
	Label string
}

// LogEntry represents a single message for the terminal/log view.
type LogEntry struct {
	// Timestamp is the formatted time string.
	Timestamp string

	// Level is the severity.
	Level LogLevel

	// Category groups related messages (e.g., "session", "tool", "health").
	Category string

	// Message is the log text.
	Message string
}

// ViewDeclaration describes a view that a module provides.
// Used during registration so the shell can build navigation without
// the module being started yet.
type ViewDeclaration struct {
	// ID matches the View.ID that will be rendered.
	ID string

	// Title is the display name for navigation.
	Title string

	// Icon is the navigation icon identifier.
	Icon string
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================

// String returns the human-readable name for a FieldStyle.
func (s FieldStyle) String() string {
	switch s {
	case StyleEmphasized:
		return "emphasized"
	case StyleMuted:
		return "muted"
	case StyleWarning:
		return "warning"
	case StyleError:
		return "error"
	case StyleSuccess:
		return "success"
	case StyleCode:
		return "code"
	default:
		return "normal"
	}
}

// String returns the human-readable name for a LogLevel.
func (l LogLevel) String() string {
	switch l {
	case LogDebug:
		return "DEBUG"
	case LogInfo:
		return "INFO"
	case LogWarn:
		return "WARN"
	case LogError:
		return "ERROR"
	case LogSuccess:
		return "SUCCESS"
	default:
		return "UNKNOWN"
	}
}

// ============================================================================
// END CLOSING
