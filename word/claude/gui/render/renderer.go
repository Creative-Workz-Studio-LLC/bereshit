// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-render-renderer
//omni:code  --go -package->render
//omni:version a-00.10
//
// Purpose: Renderer and Shell interfaces — the abstraction layer between
//          modules (what to show) and toolkit (how to show it).
// Biblical: Exodus 31:3 - "And I have filled him with the spirit of God,
//           in wisdom, and in understanding, and in knowledge"
//
// ============================================================================
// END METADATA

package render

// ============================================================================
// SETUP
// ============================================================================

import "context"

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// Renderer translates ViewModels into visible UI.
// Each toolkit (GTK4, custom, etc.) implements this interface.
type Renderer interface {
	// RenderView displays or updates a view's content.
	// Called whenever a module's ViewModel changes.
	RenderView(view View)

	// RenderLog appends a log entry to the message terminal.
	RenderLog(entry LogEntry)
}

// Shell manages the application window frame — the chrome around modules.
// Window, headerbar, sidebar navigation, and the content area.
type Shell interface {
	// Init creates the application window and chrome.
	// Called once before any modules are registered.
	Init(ctx context.Context, title string) error

	// RegisterModule adds a module's views to the sidebar navigation.
	// Called for each module before Run.
	RegisterModule(id string, name string, icon string, views []ViewDeclaration)

	// Renderer returns the Shell's Renderer for modules to push updates to.
	Renderer() Renderer

	// Run starts the toolkit event loop. Blocks until the window closes.
	// Returns the exit code.
	Run() int

	// Close tears down the shell and releases resources.
	Close()
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
