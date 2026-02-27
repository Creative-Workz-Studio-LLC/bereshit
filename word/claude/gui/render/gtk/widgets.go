// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-render-gtk-widgets
//omni:code  --go -package->gtk
//omni:version a-00.10
//
// Purpose: Widget factory — translates ViewModel types into gotk4 widgets.
//          Each ViewModel type has a corresponding build function.
// Biblical: Exodus 35:35 - "Them hath he filled with wisdom of heart,
//           to work all manner of work"
//
// ============================================================================
// END METADATA

package gtk

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"cws.studio/gui/render"
	"github.com/diamondburned/gotk4/pkg/gtk/v4"
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// fieldStyleClass maps a FieldStyle to a CSS class.
func fieldStyleClass(style render.FieldStyle) string {
	switch style {
	case render.StyleEmphasized:
		return "field-emphasized"
	case render.StyleMuted:
		return "field-muted"
	case render.StyleWarning:
		return "field-warning"
	case render.StyleError:
		return "field-error"
	case render.StyleSuccess:
		return "field-success"
	case render.StyleCode:
		return "field-code"
	default:
		return ""
	}
}

// logLevelClass maps a LogLevel to a CSS class.
func logLevelClass(level render.LogLevel) string {
	switch level {
	case render.LogDebug:
		return "log-debug"
	case render.LogInfo:
		return "log-info"
	case render.LogWarn:
		return "log-warn"
	case render.LogError:
		return "log-error"
	case render.LogSuccess:
		return "log-success"
	default:
		return ""
	}
}

// --- Core widget builders ---

// BuildView creates the complete widget tree for a View.
// Generous margins and spacing — the canvas breathes.
func BuildView(view render.View) *gtk.ScrolledWindow {
	content := gtk.NewBox(gtk.OrientationVertical, 12)
	content.SetMarginTop(20)
	content.SetMarginBottom(20)
	content.SetMarginStart(20)
	content.SetMarginEnd(20)

	for _, section := range view.Sections {
		widget := BuildSection(section)
		content.Append(widget)
	}

	scroll := gtk.NewScrolledWindow()
	scroll.SetChild(content)
	scroll.SetHExpand(true)
	scroll.SetVExpand(true)
	scroll.SetPolicy(gtk.PolicyNever, gtk.PolicyAutomatic)

	return scroll
}

// BuildSection creates a panel frame with title, fields, and children.
// Each section is a card — elevated surface with depth.
func BuildSection(section render.Section) *gtk.Box {
	frame := gtk.NewBox(gtk.OrientationVertical, 6)
	frame.AddCSSClass("panel-frame")

	// Title with visual weight
	if section.Title != "" {
		title := gtk.NewLabel(section.Title)
		title.AddCSSClass("panel-title")
		title.SetXAlign(0)
		frame.Append(title)
	}

	// Fields — each row a distinct visual unit
	for _, field := range section.Fields {
		widget := BuildField(field)
		frame.Append(widget)
	}

	// Children — nested sections with indent
	for _, child := range section.Children {
		childWidget := BuildChildSection(child)
		frame.Append(childWidget)
	}

	return frame
}

// BuildChildSection creates a nested section (lighter styling than top-level).
func BuildChildSection(section render.Section) *gtk.Box {
	box := gtk.NewBox(gtk.OrientationVertical, 4)
	box.SetMarginTop(8)
	box.SetMarginStart(12)

	if section.Title != "" {
		title := gtk.NewLabel(section.Title)
		title.AddCSSClass("section-title")
		title.SetXAlign(0)
		box.Append(title)
	}

	for _, field := range section.Fields {
		widget := BuildField(field)
		box.Append(widget)
	}

	for _, child := range section.Children {
		childWidget := BuildChildSection(child)
		box.Append(childWidget)
	}

	return box
}

// BuildField creates a key-value row, optionally with a progress bar.
// Wider key column (170px) for better alignment across sections.
func BuildField(field render.Field) *gtk.Box {
	row := gtk.NewBox(gtk.OrientationHorizontal, 12)
	row.AddCSSClass("state-label")

	// Key label — fixed width for grid-like alignment
	keyLabel := gtk.NewLabel(field.Key)
	keyLabel.AddCSSClass("state-key")
	keyLabel.SetXAlign(0)
	keyLabel.SetSizeRequest(170, -1)
	keyLabel.SetSelectable(true)
	row.Append(keyLabel)

	// Value label — expands to fill, selectable for clipboard
	valueLabel := gtk.NewLabel(field.Value)
	valueLabel.AddCSSClass("state-value")
	valueLabel.SetXAlign(0)
	valueLabel.SetHExpand(true)
	valueLabel.SetWrap(true)
	valueLabel.SetSelectable(true)

	if cls := fieldStyleClass(field.Style); cls != "" {
		valueLabel.AddCSSClass(cls)
	}

	if field.Tooltip != "" {
		valueLabel.SetTooltipText(field.Tooltip)
	}

	row.Append(valueLabel)

	// If there's a progress bar, wrap in vertical box
	if field.Progress != nil {
		wrapper := gtk.NewBox(gtk.OrientationVertical, 4)
		wrapper.Append(row)
		wrapper.Append(BuildProgress(*field.Progress))
		return wrapper
	}

	// If there's a bar, wrap in vertical box
	if field.Bar != nil {
		wrapper := gtk.NewBox(gtk.OrientationVertical, 4)
		wrapper.Append(row)
		wrapper.Append(BuildBar(*field.Bar))
		return wrapper
	}

	return row
}

// BuildProgress creates a GTK4 progress bar — tall and visible.
func BuildProgress(p render.Progress) *gtk.Box {
	box := gtk.NewBox(gtk.OrientationVertical, 2)
	box.SetMarginStart(170 + 12) // Align under value column

	bar := gtk.NewProgressBar()
	bar.SetFraction(p.Fraction)

	if p.Label != "" {
		bar.SetText(p.Label)
		bar.SetShowText(true)
	}

	if p.Style != "" {
		bar.AddCSSClass("progress-" + p.Style)
	}

	box.Append(bar)
	return box
}

// BuildBar creates a segmented colored bar — taller, with rounded ends.
func BuildBar(b render.Bar) *gtk.Box {
	bar := gtk.NewBox(gtk.OrientationHorizontal, 0)
	bar.SetSizeRequest(-1, 14)
	bar.SetMarginStart(170 + 12) // Align under value column

	for _, seg := range b.Segments {
		segment := gtk.NewBox(gtk.OrientationHorizontal, 0)
		if seg.Style != "" {
			segment.AddCSSClass(seg.Style)
		}
		// Set width proportional to the total bar
		width := int(seg.Width * 300)
		if width < 2 {
			width = 2
		}
		segment.SetSizeRequest(width, 14)
		bar.Append(segment)
	}

	return bar
}

// BuildLogEntry creates a single log line widget.
func BuildLogEntry(entry render.LogEntry) *gtk.Box {
	row := gtk.NewBox(gtk.OrientationHorizontal, 8)
	row.SetMarginTop(1)
	row.SetMarginBottom(1)

	// Timestamp
	if entry.Timestamp != "" {
		ts := gtk.NewLabel(entry.Timestamp)
		ts.AddCSSClass("log-timestamp")
		ts.SetXAlign(0)
		row.Append(ts)
	}

	// Level
	level := gtk.NewLabel(fmt.Sprintf("[%s]", entry.Level.String()))
	if cls := logLevelClass(entry.Level); cls != "" {
		level.AddCSSClass(cls)
	}
	row.Append(level)

	// Category
	if entry.Category != "" {
		cat := gtk.NewLabel(entry.Category)
		cat.AddCSSClass("log-component")
		row.Append(cat)
	}

	// Message — selectable for clipboard
	msg := gtk.NewLabel(entry.Message)
	msg.SetXAlign(0)
	msg.SetHExpand(true)
	msg.SetWrap(true)
	msg.SetSelectable(true)
	row.Append(msg)

	return row
}

// BuildChatMessage creates a styled chat bubble for the conversation area.
// Sender determines the bubble styling (you, system, agent).
func BuildChatMessage(sender, text string) *gtk.Box {
	bubble := gtk.NewBox(gtk.OrientationVertical, 2)

	// Determine bubble style based on sender
	switch sender {
	case "You":
		bubble.AddCSSClass("chat-bubble-you")
	case "System":
		bubble.AddCSSClass("chat-bubble-system")
	default:
		bubble.AddCSSClass("chat-bubble-agent")
	}

	// Sender label — selectable
	senderLabel := gtk.NewLabel(sender)
	senderLabel.SetXAlign(0)
	senderLabel.SetSelectable(true)
	switch sender {
	case "You":
		senderLabel.AddCSSClass("chat-sender-you")
	case "System":
		senderLabel.AddCSSClass("chat-sender-system")
	default:
		senderLabel.AddCSSClass("chat-sender-agent")
	}
	bubble.Append(senderLabel)

	// Message body
	body := gtk.NewLabel(text)
	body.AddCSSClass("chat-body")
	body.SetXAlign(0)
	body.SetWrap(true)
	body.SetSelectable(true)
	bubble.Append(body)

	return bubble
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
