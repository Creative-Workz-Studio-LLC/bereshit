// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-modules-dashboard-module
//omni:code  --go -package->dashboard
//omni:version a-00.10
//
// Purpose: Dashboard module registration and lifecycle.
//          Migrated from dashboard/gtk/ — now uses ViewModel pattern.
// Biblical: Habakkuk 2:2 - "Write the vision, and make it plain upon tables"
//
// ============================================================================
// END METADATA

package dashboard

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"log"

	"cws.studio/gui/render"
	"cws.studio/pkg/dashboard"
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// Module implements app.Module for the CPI-SI state dashboard.
type Module struct {
	svc      *dashboard.DashboardService
	renderer render.Renderer
	ctx      context.Context
	cancel   context.CancelFunc
}

// NewModule creates a new dashboard module.
func NewModule(svc *dashboard.DashboardService) *Module {
	return &Module{svc: svc}
}

// ID returns the module identifier.
func (m *Module) ID() string { return "dashboard" }

// Name returns the display name.
func (m *Module) Name() string { return "Dashboard" }

// Icon returns the icon identifier.
func (m *Module) Icon() string { return "dashboard" }

// Views returns the view declarations for sidebar navigation.
func (m *Module) Views() []render.ViewDeclaration {
	return []render.ViewDeclaration{
		{ID: "dashboard-overview", Title: "Overview", Icon: "dashboard"},
	}
}

// Start begins data binding — subscribes to DashboardService events
// and pushes ViewModel updates to the renderer.
func (m *Module) Start(ctx context.Context, renderer render.Renderer) error {
	m.ctx, m.cancel = context.WithCancel(ctx)
	m.renderer = renderer

	// Load and render initial state
	snap, err := m.svc.CurrentState()
	if err != nil {
		log.Printf("dashboard: failed to load initial state: %v", err)
	} else if snap != nil {
		view := BuildOverviewView(snap)
		m.renderer.RenderView(view)
	}

	// Subscribe to live updates
	ch := m.svc.Subscribe()
	go m.eventLoop(ch)

	return nil
}

// Stop halts data binding.
func (m *Module) Stop() {
	if m.cancel != nil {
		m.cancel()
	}
}

// eventLoop listens for state changes and pushes ViewModel updates.
func (m *Module) eventLoop(ch <-chan dashboard.StateChange) {
	defer m.svc.Unsubscribe(ch)

	for {
		select {
		case <-m.ctx.Done():
			return
		case change, ok := <-ch:
			if !ok {
				return
			}
			m.handleChange(change)
		}
	}
}

// handleChange processes a state change event.
func (m *Module) handleChange(change dashboard.StateChange) {
	switch change.Type {
	case "state_update", "path_update", "history_update":
		if change.Snapshot != nil {
			view := BuildOverviewView(change.Snapshot)
			m.renderer.RenderView(view)
		}
	case "log_event":
		if change.Event != nil {
			entry := LogEventToEntry(*change.Event)
			m.renderer.RenderLog(entry)
		}
	}
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
