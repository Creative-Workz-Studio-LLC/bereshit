// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-app-module
//omni:code  --go -package->app
//omni:version a-00.10
//
// Purpose: Module interface and registry — self-contained feature areas
//          that register with the application and emit ViewModels.
// Biblical: 1 Corinthians 12:12 - "For as the body is one, and hath many
//           members, and all the members of that one body, being many, are
//           one body: so also is Christ."
//
// ============================================================================
// END METADATA

package app

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

// Module is a self-contained feature area within the application.
// Each module declares its views, binds to data sources, and pushes
// ViewModel updates to the renderer. Modules know nothing about GTK4
// or any specific toolkit.
type Module interface {
	// ID returns the unique module identifier (e.g., "dashboard", "sdkinspector").
	ID() string

	// Name returns the human-readable display name (e.g., "Dashboard", "SDK Inspector").
	Name() string

	// Icon returns the icon identifier for navigation (e.g., "dashboard", "sdk").
	Icon() string

	// Views returns the module's view declarations for navigation registration.
	Views() []render.ViewDeclaration

	// Start begins data binding. The module should subscribe to data sources
	// and push ViewModel updates to the renderer. Blocks are not expected —
	// use goroutines for background watching.
	Start(ctx context.Context, renderer render.Renderer) error

	// Stop halts data binding and releases resources.
	Stop()
}

// ModuleRegistry tracks registered modules.
type ModuleRegistry struct {
	modules []Module
}

// NewRegistry creates an empty module registry.
func NewRegistry() *ModuleRegistry {
	return &ModuleRegistry{}
}

// Register adds a module to the registry.
func (r *ModuleRegistry) Register(mod Module) {
	r.modules = append(r.modules, mod)
}

// Modules returns all registered modules in registration order.
func (r *ModuleRegistry) Modules() []Module {
	return r.modules
}

// Get returns a module by ID, or nil if not found.
func (r *ModuleRegistry) Get(id string) Module {
	for _, mod := range r.modules {
		if mod.ID() == id {
			return mod
		}
	}
	return nil
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
