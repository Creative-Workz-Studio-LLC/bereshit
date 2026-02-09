// ============================================================================
// METADATA - CPI-SI Dashboard GTK4 Message Terminal
// ============================================================================
//
// Purpose: Scrolling log viewer with color-coded severity levels
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ============================================================================

package panels

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"
	"time"

	"cws.studio/pkg/dashboard"
	"github.com/diamondburned/gotk4/pkg/gtk/v4"
)

const maxTerminalLines = 500

// ============================================================================
// BODY
// ============================================================================

// MessageTerminal displays a scrolling log of CPI-SI events.
type MessageTerminal struct {
	box      *gtk.Box
	textView *gtk.TextView
	buffer   *gtk.TextBuffer
	scroll   *gtk.ScrolledWindow
	count    int
}

// NewMessageTerminal creates the message terminal panel.
func NewMessageTerminal() *MessageTerminal {
	m := &MessageTerminal{}
	m.build()
	return m
}

// Widget returns the root widget.
func (m *MessageTerminal) Widget() gtk.Widgetter {
	return m.box
}

// build constructs the terminal layout.
func (m *MessageTerminal) build() {
	m.box = gtk.NewBox(gtk.OrientationVertical, 0)
	m.box.AddCSSClass("message-terminal")
	m.box.SetSizeRequest(-1, 200)

	// Title bar
	titleBar := gtk.NewBox(gtk.OrientationHorizontal, 8)
	titleBar.SetMarginStart(8)
	titleBar.SetMarginEnd(8)
	titleBar.SetMarginTop(4)
	titleBar.SetMarginBottom(4)

	title := gtk.NewLabel("Messages")
	title.AddCSSClass("panel-title")
	title.SetXAlign(0)
	title.SetHExpand(true)
	titleBar.Append(title)

	m.box.Append(titleBar)

	sep := gtk.NewSeparator(gtk.OrientationHorizontal)
	m.box.Append(sep)

	// Text view
	m.buffer = gtk.NewTextBuffer(nil)
	m.textView = gtk.NewTextViewWithBuffer(m.buffer)
	m.textView.SetEditable(false)
	m.textView.SetCursorVisible(false)
	m.textView.SetMonospace(true)
	m.textView.SetWrapMode(gtk.WrapWordChar)
	m.textView.SetTopMargin(4)
	m.textView.SetBottomMargin(4)
	m.textView.SetLeftMargin(8)
	m.textView.SetRightMargin(8)

	// Create text tags for coloring
	tagTable := m.buffer.TagTable()

	infoTag := gtk.NewTextTag("info")
	infoTag.SetObjectProperty("foreground", "#4fc3f7")
	tagTable.Add(infoTag)

	warnTag := gtk.NewTextTag("warn")
	warnTag.SetObjectProperty("foreground", "#ffd54f")
	tagTable.Add(warnTag)

	errorTag := gtk.NewTextTag("error")
	errorTag.SetObjectProperty("foreground", "#ef5350")
	tagTable.Add(errorTag)

	debugTag := gtk.NewTextTag("debug")
	debugTag.SetObjectProperty("foreground", "#6a7080")
	tagTable.Add(debugTag)

	timestampTag := gtk.NewTextTag("timestamp")
	timestampTag.SetObjectProperty("foreground", "#6a7080")
	tagTable.Add(timestampTag)

	componentTag := gtk.NewTextTag("component")
	componentTag.SetObjectProperty("foreground", "#ce93d8")
	tagTable.Add(componentTag)

	m.scroll = gtk.NewScrolledWindow()
	m.scroll.SetPolicy(gtk.PolicyAutomatic, gtk.PolicyAutomatic)
	m.scroll.SetVExpand(true)
	m.scroll.SetHExpand(true)
	m.scroll.SetChild(m.textView)

	m.box.Append(m.scroll)

	// Welcome message
	m.appendLine("—", "system", "CPI-SI Dashboard GTK4 started", "info")
}

// AddEvent appends a log event to the terminal.
func (m *MessageTerminal) AddEvent(event dashboard.LogEvent) {
	level := event.Level
	if level == "" {
		level = "INFO"
	}
	component := event.Component
	if component == "" {
		component = "system"
	}
	message := event.Message
	if message == "" {
		message = event.Type
	}

	tagName := "info"
	switch strings.ToUpper(level) {
	case "WARN", "WARNING":
		tagName = "warn"
	case "ERROR", "ERR":
		tagName = "error"
	case "DEBUG":
		tagName = "debug"
	}

	m.appendLine(level, component, message, tagName)
}

// appendLine adds a formatted line to the terminal buffer.
func (m *MessageTerminal) appendLine(level, component, message, tagName string) {
	ts := time.Now().Format("15:04:05")

	// Add newline if not first line
	if m.count > 0 {
		endIter := m.buffer.EndIter()
		m.buffer.Insert(endIter, "\n")
	}

	// Insert timestamp with tag
	startOffset := m.buffer.EndIter().Offset()
	endIter := m.buffer.EndIter()
	tsText := ts + " "
	m.buffer.Insert(endIter, tsText)
	tagStart := m.buffer.IterAtOffset(startOffset)
	tagEnd := m.buffer.EndIter()
	m.buffer.ApplyTagByName("timestamp", tagStart, tagEnd)

	// Insert level with tag
	startOffset = m.buffer.EndIter().Offset()
	endIter = m.buffer.EndIter()
	levelStr := fmt.Sprintf("%-5s ", strings.ToUpper(level))
	m.buffer.Insert(endIter, levelStr)
	tagStart = m.buffer.IterAtOffset(startOffset)
	tagEnd = m.buffer.EndIter()
	m.buffer.ApplyTagByName(tagName, tagStart, tagEnd)

	// Insert component with tag
	startOffset = m.buffer.EndIter().Offset()
	endIter = m.buffer.EndIter()
	compStr := fmt.Sprintf("%-12s ", component)
	m.buffer.Insert(endIter, compStr)
	tagStart = m.buffer.IterAtOffset(startOffset)
	tagEnd = m.buffer.EndIter()
	m.buffer.ApplyTagByName("component", tagStart, tagEnd)

	// Insert message (no tag)
	endIter = m.buffer.EndIter()
	m.buffer.Insert(endIter, message)

	m.count++

	// Trim old lines if over limit
	if m.count > maxTerminalLines {
		startIter := m.buffer.StartIter()
		// Find end of first line
		firstLineEnd := m.buffer.StartIter()
		firstLineEnd.ForwardLine()
		m.buffer.Delete(startIter, firstLineEnd)
		m.count--
	}

	// Auto-scroll to bottom
	endMark := m.buffer.CreateMark("end", m.buffer.EndIter(), false)
	m.textView.ScrollToMark(endMark, 0, true, 0, 1)
	m.buffer.DeleteMark(endMark)
}

// ============================================================================
// CLOSING
// ============================================================================
