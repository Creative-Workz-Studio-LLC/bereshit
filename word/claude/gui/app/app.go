// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-app-app
//omni:code  --go -package->app
//omni:version a-00.10
//
// Purpose: Application lifecycle — registers modules, initializes the shell,
//          starts data binding, and runs the event loop.
// Biblical: Nehemiah 2:18 - "Let us rise up and build. So they strengthened
//           their hands for this good work."
//
// ============================================================================
// END METADATA

package app

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"cws.studio/gui/render"
)

// Version is the application version.
const Version = "a-00.10"

// AppTitle is the window title.
const AppTitle = "CWS Universal Base"

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// Application is the top-level orchestrator.
// It owns the module registry and coordinates startup/shutdown.
type Application struct {
	registry *ModuleRegistry
	ctx      context.Context
	cancel   context.CancelFunc
}

// New creates a new Application with a cancellable context.
func New() *Application {
	ctx, cancel := context.WithCancel(context.Background())
	return &Application{
		registry: NewRegistry(),
		ctx:      ctx,
		cancel:   cancel,
	}
}

// RegisterModule adds a module to the application.
// Must be called before Run.
func (a *Application) RegisterModule(mod Module) {
	a.registry.Register(mod)
}

// Run initializes the shell, registers all modules, starts data binding,
// and enters the event loop. Blocks until the window closes or a signal
// is received. Returns the exit code.
func (a *Application) Run(shell render.Shell) int {
	// Handle OS signals for graceful shutdown
	sigCh := make(chan os.Signal, 1)
	signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)
	go func() {
		<-sigCh
		a.cancel()
		shell.Close()
	}()

	// Initialize the shell (creates the window)
	if err := shell.Init(a.ctx, AppTitle+" — "+Version); err != nil {
		fmt.Fprintf(os.Stderr, "Failed to initialize shell: %v\n", err)
		return 1
	}

	// Register all modules with the shell's navigation
	for _, mod := range a.registry.Modules() {
		shell.RegisterModule(mod.ID(), mod.Name(), mod.Icon(), mod.Views())
	}

	// Start data binding for each module
	renderer := shell.Renderer()
	for _, mod := range a.registry.Modules() {
		if err := mod.Start(a.ctx, renderer); err != nil {
			fmt.Fprintf(os.Stderr, "Failed to start module %q: %v\n", mod.ID(), err)
			// Non-fatal — continue with other modules
		}
	}

	// Run the event loop (blocks)
	exitCode := shell.Run()

	// Shutdown
	a.shutdown()

	return exitCode
}

// shutdown stops all modules and cancels the context.
func (a *Application) shutdown() {
	a.cancel()

	for _, mod := range a.registry.Modules() {
		mod.Stop()
	}
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
