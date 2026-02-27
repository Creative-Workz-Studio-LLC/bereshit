// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-modules-sdkinspector-module
//omni:code  --go -package->sdkinspector
//omni:version a-00.10
//
// Purpose: SDK Inspector module — visualizes hook I/O, substrate detection,
//          event types, and permission values from pkg/sdk/hookoutput.
// Biblical: Proverbs 25:2 - "It is the glory of God to conceal a thing:
//           but the honour of kings is to search out a matter."
//
// ============================================================================
// END METADATA

package sdkinspector

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"

	"cws.studio/gui/render"
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// Module implements app.Module for the SDK Inspector.
type Module struct {
	renderer render.Renderer
	ctx      context.Context
	cancel   context.CancelFunc
}

// NewModule creates a new SDK Inspector module.
func NewModule() *Module {
	return &Module{}
}

// ID returns the module identifier.
func (m *Module) ID() string { return "sdkinspector" }

// Name returns the display name.
func (m *Module) Name() string { return "SDK Inspector" }

// Icon returns the icon identifier.
func (m *Module) Icon() string { return "sdk" }

// Views returns the view declarations.
func (m *Module) Views() []render.ViewDeclaration {
	return []render.ViewDeclaration{
		{ID: ViewHookIO, Title: "Hook I/O", Icon: "sdk"},
		{ID: ViewSubstrate, Title: "Substrate", Icon: "sdk"},
	}
}

// Start begins rendering the SDK Inspector views.
func (m *Module) Start(ctx context.Context, renderer render.Renderer) error {
	m.ctx, m.cancel = context.WithCancel(ctx)
	m.renderer = renderer

	// Render static views — SDK data doesn't change at runtime
	renderer.RenderView(BuildHookIOView())
	renderer.RenderView(BuildSubstrateView())

	return nil
}

// Stop halts the module.
func (m *Module) Stop() {
	if m.cancel != nil {
		m.cancel()
	}
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
