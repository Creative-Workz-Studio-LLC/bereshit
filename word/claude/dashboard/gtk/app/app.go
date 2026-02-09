// ============================================================================
// METADATA - CPI-SI Dashboard GTK4 Application
// ============================================================================
//
// Purpose: GTK4 Application shell — window, layout, theme, event bridge
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ============================================================================

package app

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"log"

	"cws.studio/dashboard/gtk/panels"
	"cws.studio/dashboard/gtk/styles"
	"cws.studio/pkg/dashboard"
	"github.com/diamondburned/gotk4/pkg/gdk/v4"
	"github.com/diamondburned/gotk4/pkg/gio/v2"
	"github.com/diamondburned/gotk4/pkg/glib/v2"
	"github.com/diamondburned/gotk4/pkg/gtk/v4"
)

// ============================================================================
// BODY
// ============================================================================

// DashboardApp holds the GTK4 application and all panel references.
type DashboardApp struct {
	app    *gtk.Application
	svc    *dashboard.DashboardService
	ctx    context.Context
	cancel context.CancelFunc

	// Panels
	stateOverview *panels.StateOverview
	analytics     *panels.Analytics
	systemData    *panels.SystemData
	msgTerminal   *panels.MessageTerminal

	// Widgets
	window    *gtk.ApplicationWindow
	headerBar *gtk.HeaderBar
	stack     *gtk.Stack

	// Config
	windowTitle string
}

// Options holds configuration for the dashboard application.
type Options struct {
	Title string // Override window title (empty = default)
}

// New creates a new DashboardApp with default options.
func New(svc *dashboard.DashboardService) *DashboardApp {
	return NewWithOptions(svc, Options{})
}

// NewWithOptions creates a new DashboardApp with the given options.
func NewWithOptions(svc *dashboard.DashboardService, opts Options) *DashboardApp {
	ctx, cancel := context.WithCancel(context.Background())
	d := &DashboardApp{
		svc:    svc,
		ctx:    ctx,
		cancel: cancel,
	}
	if opts.Title != "" {
		d.windowTitle = opts.Title
	}
	return d
}

// Run starts the GTK4 application.
func (d *DashboardApp) Run(args []string) int {
	d.app = gtk.NewApplication("com.creativeworkzstudio.cpisi.dashboard", gio.ApplicationFlagsNone)
	d.app.ConnectActivate(d.onActivate)
	return d.app.Run(args)
}

// onActivate sets up the window, layout, theme, and event bridge.
func (d *DashboardApp) onActivate() {
	d.applyTheme()
	d.buildWindow()
	d.buildLayout()
	d.loadInitialState()
	d.startEventBridge()
	d.window.Show()
}

// applyTheme loads the dark CSS theme.
func (d *DashboardApp) applyTheme() {
	provider := gtk.NewCSSProvider()
	provider.LoadFromString(styles.DashboardCSS())

	display := gdk.DisplayGetDefault()
	gtk.StyleContextAddProviderForDisplay(display, provider, gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
}

// buildWindow creates the main application window with header bar.
func (d *DashboardApp) buildWindow() {
	d.window = gtk.NewApplicationWindow(d.app)
	title := "CPI-SI Dashboard"
	if d.windowTitle != "" {
		title = d.windowTitle
	}
	d.window.SetTitle(title)
	d.window.SetDefaultSize(1200, 800)

	// Header bar
	d.headerBar = gtk.NewHeaderBar()

	titleLabel := gtk.NewLabel("CPI-SI Dashboard")
	titleLabel.AddCSSClass("title")

	subtitleLabel := gtk.NewLabel("Nova Dawn — CreativeWorkzStudio")
	subtitleLabel.AddCSSClass("subtitle")

	titleBox := gtk.NewBox(gtk.OrientationVertical, 0)
	titleBox.Append(titleLabel)
	titleBox.Append(subtitleLabel)
	d.headerBar.SetTitleWidget(titleBox)

	d.window.SetTitlebar(d.headerBar)
}

// buildLayout creates the main layout: sidebar | stack | message terminal.
func (d *DashboardApp) buildLayout() {
	// Create panels
	d.stateOverview = panels.NewStateOverview()
	d.analytics = panels.NewAnalytics()
	d.systemData = panels.NewSystemData()
	d.msgTerminal = panels.NewMessageTerminal()

	// Stack for switchable views
	d.stack = gtk.NewStack()
	d.stack.SetTransitionType(gtk.StackTransitionTypeCrossfade)
	d.stack.SetTransitionDuration(200)

	// Give system data panel access to the service
	d.systemData.SetService(d.svc)

	d.stack.AddTitled(d.stateOverview.Widget(), "overview", "Overview")
	d.stack.AddTitled(d.analytics.Widget(), "analytics", "Analytics")
	d.stack.AddTitled(d.systemData.Widget(), "systemdata", "System Data")

	// Sidebar
	sidebar := gtk.NewStackSidebar()
	sidebar.SetStack(d.stack)
	sidebar.SetSizeRequest(160, -1)

	// Sidebar + Stack in horizontal box
	mainContent := gtk.NewBox(gtk.OrientationHorizontal, 0)
	mainContent.Append(sidebar)

	sep := gtk.NewSeparator(gtk.OrientationVertical)
	mainContent.Append(sep)

	d.stack.SetHExpand(true)
	d.stack.SetVExpand(true)
	mainContent.Append(d.stack)

	// Vertical paned: main content top, message terminal bottom
	paned := gtk.NewPaned(gtk.OrientationVertical)
	paned.SetStartChild(mainContent)
	paned.SetEndChild(d.msgTerminal.Widget())
	paned.SetPosition(550)
	paned.SetShrinkStartChild(false)
	paned.SetShrinkEndChild(false)

	d.window.SetChild(paned)
}

// loadInitialState fetches and displays the current state.
func (d *DashboardApp) loadInitialState() {
	snap, err := d.svc.CurrentState()
	if err != nil {
		log.Printf("Failed to load initial state: %v", err)
		return
	}
	d.updatePanels(snap)
}

// startEventBridge subscribes to the EventBus and bridges events to GTK main loop.
func (d *DashboardApp) startEventBridge() {
	ch := d.svc.Subscribe()

	go func() {
		for {
			select {
			case <-d.ctx.Done():
				d.svc.Unsubscribe(ch)
				return
			case change, ok := <-ch:
				if !ok {
					return
				}
				// Schedule UI update on GTK main thread
				changeCopy := change
				glib.IdleAdd(func() {
					d.handleStateChange(changeCopy)
				})
			}
		}
	}()
}

// handleStateChange processes a state change on the GTK main thread.
func (d *DashboardApp) handleStateChange(change dashboard.StateChange) {
	switch change.Type {
	case "state_update":
		if change.Snapshot != nil {
			d.updatePanels(change.Snapshot)
		}
	case "log_event":
		if change.Event != nil {
			d.msgTerminal.AddEvent(*change.Event)
		}
	case "path_update":
		if change.Snapshot != nil {
			d.stateOverview.UpdateState(change.Snapshot)
		}
	case "history_update":
		if change.Snapshot != nil {
			d.analytics.UpdateState(change.Snapshot)
		}
	}
}

// updatePanels refreshes all panels with a new state snapshot.
func (d *DashboardApp) updatePanels(snap *dashboard.StateSnapshot) {
	d.stateOverview.UpdateState(snap)
	d.analytics.UpdateState(snap)
	d.updateHeaderBar(snap)

	// Load valence distribution for current session
	if snap.SessionID != "" {
		if dist, err := d.svc.ValenceDistribution(snap.SessionID); err == nil {
			d.stateOverview.UpdateValence(dist)
		}
	}
}

// updateHeaderBar refreshes header with current session info.
func (d *DashboardApp) updateHeaderBar(snap *dashboard.StateSnapshot) {
	// Update subtitle with current state
	if titleWidget := d.headerBar.TitleWidget(); titleWidget != nil {
		if box, ok := titleWidget.(*gtk.Box); ok {
			// Second child is subtitle
			if child := box.LastChild(); child != nil {
				if label, ok := child.(*gtk.Label); ok {
					subtitle := snap.HebrewState
					if snap.AnchorKey != "" {
						subtitle = snap.AnchorKey + " — " + subtitle
					}
					if snap.SessionArc != "" {
						subtitle += " — " + snap.SessionArc
					}
					label.SetText(subtitle)
				}
			}
		}
	}
}

// Close cleans up the event bridge.
func (d *DashboardApp) Close() {
	d.cancel()
}

// ============================================================================
// CLOSING
// ============================================================================
