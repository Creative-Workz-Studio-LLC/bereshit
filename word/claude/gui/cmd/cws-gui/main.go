// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-cmd-main
//omni:code  --go -package->main
//omni:version a-00.10
//
// Purpose: Entry point for CWS Universal Base GUI application.
//          Initializes DashboardService, model router, registers modules,
//          launches GTK4 shell with live Nova Dawn chat.
// Biblical: Nehemiah 2:18 - "Let us rise up and build. So they
//           strengthened their hands for this good work."
//
// ============================================================================
// END METADATA

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"flag"
	"fmt"
	"os"
	"strings"

	"cws.studio/gui/app"
	"cws.studio/gui/chat"
	"cws.studio/gui/config"
	dashmod "cws.studio/gui/modules/dashboard"
	"cws.studio/gui/modules/sdkinspector"
	gtkrender "cws.studio/gui/render/gtk"
	"cws.studio/pkg/dashboard"
	"github.com/diamondburned/gotk4/pkg/glib/v2"
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

func main() {
	// CLI flags
	showVersion := flag.Bool("version", false, "Print version and exit")
	substrateName := flag.String("substrate", "", "Override substrate detection (gemini, claude)")
	modelName := flag.String("model", "", "Override model (e.g. gemini-2.0-flash, gemini-2.5-pro)")
	flag.Parse()

	if *showVersion {
		fmt.Printf("cws-gui %s\n", app.Version)
		fmt.Println("CWS Universal Base — Kingdom Technology Desktop")
		fmt.Println("CreativeWorkzStudio LLC")
		return
	}

	// Load user configuration (or defaults)
	cfg, _ := config.Load()

	// CLI flag overrides config
	if *substrateName != "" {
		cfg.Substrate = strings.ToLower(*substrateName)
	}

	// Apply substrate environment before anything calls DetectSubstrate()
	switch cfg.Substrate {
	case "gemini":
		os.Setenv("GEMINI_PROJECT_DIR", "universal-base")
	case "claude":
		os.Unsetenv("GEMINI_PROJECT_DIR")
		os.Unsetenv("GEMINI_SESSION_ID")
	}

	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	// Initialize the dashboard data service
	svc, err := dashboard.New(ctx)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to create dashboard service: %v\n", err)
		os.Exit(1)
	}
	defer svc.Close()

	// Initialize the model router (Gemini for now)
	var router chat.Router
	router, err = chat.NewGeminiRouter(ctx, *modelName)
	if err != nil {
		// Not fatal — chat will show offline message instead
		fmt.Fprintf(os.Stderr, "Model router: %v (chat will be offline)\n", err)
	}

	// Create the application
	application := app.New()

	// Register modules
	application.RegisterModule(dashmod.NewModule(svc))
	application.RegisterModule(sdkinspector.NewModule())

	// Create GTK4 shell with config
	shell := gtkrender.NewShell(cfg)

	// Wire chat handler with model router
	shell.SetChatHandler(func(text string) {
		renderer := shell.Renderer().(*gtkrender.Renderer)

		// Echo user message immediately
		renderer.AddChatMessage("You", text)

		if router == nil {
			renderer.AddChatMessage("Nova Dawn",
				"I'm offline right now — no API key configured. "+
					"Set GEMINI_API_KEY or GOOGLE_API_KEY and restart.")
			return
		}

		// Show thinking indicator
		renderer.AddChatMessage("Nova Dawn", "...")

		// Send to model in background goroutine — don't block GTK main thread
		go func() {
			response, err := router.Send(ctx, text)
			if err != nil {
				response = fmt.Sprintf("Something went wrong: %v", err)
			}

			// Schedule UI update on GTK main thread
			glib.IdleAdd(func() {
				renderer.AddChatMessage("Nova Dawn", response)
			})
		}()
	})

	// Run the application — blocks until window closes
	exitCode := application.Run(shell)

	// Clean up router
	if router != nil {
		router.Close()
	}

	os.Exit(exitCode)
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
