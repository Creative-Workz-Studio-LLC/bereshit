// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-render-gtk-renderer
//omni:code  --go -package->gtk
//omni:version a-00.10
//
// Purpose: GTK4 Renderer — translates ViewModels into gotk4 widgets.
//          Implements the render.Renderer interface. Manages view caching,
//          conversation area, and system log separately.
// Biblical: 1 Kings 7:14 - "He was filled with wisdom, and understanding,
//           and cunning to work all works in brass"
//
// ============================================================================
// END METADATA

package gtk

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"sync"

	"cws.studio/gui/render"
	"github.com/diamondburned/gotk4/pkg/glib/v2"
	"github.com/diamondburned/gotk4/pkg/gtk/v4"
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// Renderer implements render.Renderer for GTK4.
// All widget operations are scheduled on the GTK main thread via glib.IdleAdd.
// Conversation and system log are separate visual areas.
type Renderer struct {
	mu sync.Mutex

	// stack holds the view content area
	stack *gtk.Stack

	// chatBox holds conversation messages (Nova Dawn's space)
	chatBox *gtk.Box

	// chatScroll holds the scrolled window around conversation
	chatScroll *gtk.ScrolledWindow

	// logBox holds system log entries (utility drawer)
	logBox *gtk.Box

	// logScroll holds the scrolled window around system log
	logScroll *gtk.ScrolledWindow

	// chatInput is the text entry for interactive chat
	chatInput *gtk.Entry

	// chatCallback is called when the user submits a chat message
	chatCallback func(text string)

	// viewWidgets caches the widget for each view ID
	viewWidgets map[string]*gtk.ScrolledWindow

	// chatCount tracks conversation entries
	chatCount int

	// logCount tracks system log entries
	logCount int

	// onLogCountChanged fires when log count changes (updates drawer label)
	onLogCountChanged func(count int)
}

// NewRenderer creates a GTK4 renderer.
func NewRenderer() *Renderer {
	return &Renderer{
		viewWidgets: make(map[string]*gtk.ScrolledWindow),
	}
}

// SetStack sets the content stack managed by this renderer.
func (r *Renderer) SetStack(stack *gtk.Stack) {
	r.stack = stack
}

// SetConversation sets the conversation message area.
func (r *Renderer) SetConversation(box *gtk.Box, scroll *gtk.ScrolledWindow) {
	r.chatBox = box
	r.chatScroll = scroll
}

// SetSystemLog sets the system log drawer area.
func (r *Renderer) SetSystemLog(box *gtk.Box, scroll *gtk.ScrolledWindow) {
	r.logBox = box
	r.logScroll = scroll
}

// SetLogCountCallback sets a function called when the system log count changes.
// Used by the shell to update the log drawer toggle label.
func (r *Renderer) SetLogCountCallback(fn func(count int)) {
	r.onLogCountChanged = fn
}

// SetChatInput sets the chat entry widget and callback.
func (r *Renderer) SetChatInput(entry *gtk.Entry, callback func(string)) {
	r.chatInput = entry
	r.chatCallback = callback
}

// RenderView displays or updates a view in the stack.
// Safe to call from any goroutine — schedules on GTK main thread.
func (r *Renderer) RenderView(view render.View) {
	glib.IdleAdd(func() {
		r.renderViewOnMain(view)
	})
}

// RenderLog appends a log entry to the system log drawer.
// Safe to call from any goroutine.
func (r *Renderer) RenderLog(entry render.LogEntry) {
	glib.IdleAdd(func() {
		r.renderLogOnMain(entry)
	})
}

// AddChatMessage displays a styled chat bubble in the conversation area.
// Safe to call from any goroutine.
func (r *Renderer) AddChatMessage(sender, text string) {
	glib.IdleAdd(func() {
		r.renderChatOnMain(sender, text)
	})
}

// --- Main-thread operations ---

// renderViewOnMain rebuilds the view widget and replaces it in the stack.
func (r *Renderer) renderViewOnMain(view render.View) {
	r.mu.Lock()
	defer r.mu.Unlock()

	if r.stack == nil {
		return
	}

	// Build new widget tree from ViewModel
	widget := BuildView(view)

	// Remove any existing child with this ID (tracked or placeholder)
	if existing, ok := r.viewWidgets[view.ID]; ok {
		r.stack.Remove(existing)
	} else if placeholder := r.stack.ChildByName(view.ID); placeholder != nil {
		r.stack.Remove(placeholder)
	}

	r.stack.AddTitled(widget, view.ID, view.Title)
	r.viewWidgets[view.ID] = widget
}

// renderLogOnMain appends a log line to the system log drawer.
func (r *Renderer) renderLogOnMain(entry render.LogEntry) {
	if r.logBox == nil {
		return
	}

	row := BuildLogEntry(entry)
	r.logBox.Append(row)
	r.logCount++

	// Auto-scroll log to bottom
	if r.logScroll != nil {
		adj := r.logScroll.VAdjustment()
		adj.SetValue(adj.Upper())
	}

	// Notify shell of log count change (updates drawer label)
	if r.onLogCountChanged != nil {
		r.onLogCountChanged(r.logCount)
	}

	// Trim old entries
	r.trimBox(r.logBox, &r.logCount, 500)
}

// renderChatOnMain appends a chat bubble to the conversation area.
func (r *Renderer) renderChatOnMain(sender, text string) {
	if r.chatBox == nil {
		return
	}

	bubble := BuildChatMessage(sender, text)
	r.chatBox.Append(bubble)
	r.chatCount++

	// Auto-scroll conversation to bottom
	if r.chatScroll != nil {
		adj := r.chatScroll.VAdjustment()
		adj.SetValue(adj.Upper())
	}

	// Trim old entries
	r.trimBox(r.chatBox, &r.chatCount, 500)
}

// trimBox removes oldest children beyond maxEntries from a box.
func (r *Renderer) trimBox(box *gtk.Box, count *int, maxEntries int) {
	if box == nil {
		return
	}

	for *count > maxEntries {
		first := box.FirstChild()
		if first == nil {
			break
		}
		box.Remove(first)
		*count--
	}
}

// Verify interface compliance at compile time.
var _ render.Renderer = (*Renderer)(nil)

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================

// String returns a description of the renderer.
func (r *Renderer) String() string {
	return fmt.Sprintf("GTK4 Renderer (%d views, %d messages, %d log entries)",
		len(r.viewWidgets), r.chatCount, r.logCount)
}

// ============================================================================
// END CLOSING
