// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-main
// Purpose: CWS Server entry point — company infrastructure foundation
// Biblical: Nehemiah 2:18 — "Let us rise up and build"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

// CWS Server is the unified application server for CreativeWorkzStudio LLC.
// It serves four services from a single binary:
//
//   - Builder Dashboard: Process execution, build config, and editorial tooling
//   - CPI-SI Console: Full CPI-SI dashboard with real-time state visualization
//   - CPI-SI API: State machine queries, journal CRUD, and FTS5 search
//   - Company Website: Static site serving with host-based routing
//
// Infrastructure:
//   - Service registry with per-service health checks
//   - Request metrics tracking (counters, latency, error rate)
//   - WebSocket hub for real-time event streaming
//   - Host-based routing for multi-domain serving
//
// Service composition is determined at startup based on available resources:
// builder requires a valid project directory, CPI-SI requires the DashboardService
// from claude-global, and the website requires a static files directory.
// Services that can't initialize are gracefully disabled without affecting others.
//
// The server supports graceful shutdown via SIGINT/SIGTERM, cancelling all
// running processes and draining HTTP connections within a 5-second window.
package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"flag"
	"fmt"
	"io/fs"
	"log/slog"
	"net/http"
	"os"
	"os/signal"
	"path/filepath"
	"syscall"
	"time"

	"cws.studio/server/internal/config"
	"cws.studio/server/internal/server"
	"cws.studio/server/internal/services/builder"
	"cws.studio/server/internal/services/cpisi"
	"cws.studio/server/internal/ws"

	dashboardserver "cws.studio/dashboard/server"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

const version = "2.0.0"

func main() {
	// ── Flags ─────────────────────────────────────────────────────────
	cfg := config.DefaultConfig()

	var websiteDir string
	var dataDir string

	flag.IntVar(&cfg.Port, "port", cfg.Port, "Server port")
	flag.StringVar(&cfg.BuilderDir, "builder-dir", cfg.BuilderDir, "Path to company-docs/ project directory")
	flag.StringVar(&websiteDir, "website-dir", "", "Path to company website static files")
	flag.StringVar(&dataDir, "data-dir", "", "Path to persistent data directory (default: ~/cws/data)")
	flag.BoolVar(&cfg.EnableCPISI, "cpisi", cfg.EnableCPISI, "Enable CPI-SI service")
	flag.BoolVar(&cfg.DevMode, "dev", cfg.DevMode, "Development mode (filesystem serving)")
	flag.Parse()

	// Default data-dir based on environment.
	if dataDir == "" {
		home, _ := os.UserHomeDir()
		dataDir = filepath.Join(home, "cws", "data")
	}

	// ── Logging ───────────────────────────────────────────────────────
	level := slog.LevelInfo
	if cfg.DevMode {
		level = slog.LevelDebug
	}
	slog.SetDefault(slog.New(slog.NewJSONHandler(os.Stdout, &slog.HandlerOptions{
		Level: level,
	})))

	slog.Info("CWS Server starting",
		"version", version,
		"port", cfg.Port,
		"dev", cfg.DevMode,
		"cpisi", cfg.EnableCPISI,
	)

	// ── Context + Signals ─────────────────────────────────────────────
	ctx, cancel := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer cancel()

	// ── Infrastructure ───────────────────────────────────────────────
	registry := server.NewRegistry()
	metrics := server.NewMetrics()

	// ── Resolve Builder Paths ─────────────────────────────────────────
	var builderPaths *config.BuilderPaths
	if cfg.BuilderDir != "" {
		paths, err := config.ResolveBuilderPaths(cfg.BuilderDir)
		if err != nil {
			slog.Error("failed to resolve builder paths", "dir", cfg.BuilderDir, "error", err)
			os.Exit(1)
		}
		builderPaths = paths
	} else {
		// Auto-detect: walk up from CWD looking for company-docs
		cwd, _ := os.Getwd()
		projectDir, err := config.FindProjectDir(cwd)
		if err != nil {
			slog.Warn("builder service disabled: could not find project directory", "error", err)
		} else {
			paths, err := config.ResolveBuilderPaths(projectDir)
			if err != nil {
				slog.Warn("builder service disabled", "error", err)
			} else {
				builderPaths = paths
			}
		}
	}

	// ── WebSocket Hub ─────────────────────────────────────────────────
	hub := ws.NewHub(ctx)
	go hub.Run()

	// ── Services ──────────────────────────────────────────────────────
	mux := http.NewServeMux()

	// Builder Dashboard service
	var pm *builder.ProcessManager
	if builderPaths != nil {
		pm = builder.NewProcessManager(hub)
		builderSvc := builder.NewService(pm, builderPaths)
		builderSvc.RegisterRoutes(mux)
		registry.Register(server.ServiceInfo{
			Name:        "builder",
			Version:     version,
			Routes:      10,
			Description: "Editorial dashboard — build, lint, validate, publish",
		}, nil)
		slog.Info("builder service enabled", "projectDir", builderPaths.ProjectDir)
	}

	// CPI-SI service
	var cpisiBridge *cpisi.Bridge
	var cpisiSvc *cpisi.Service
	var cpisiConsoleMux *http.ServeMux
	if cfg.EnableCPISI {
		bridge, err := cpisi.NewBridge(ctx, hub, dataDir)
		if err != nil {
			slog.Warn("cpisi service disabled", "error", err)
		} else {
			cpisiBridge = bridge
			cpisiSvc = cpisi.NewService(bridge, bridge.Journals())

			// Register API routes on main mux (/api/cpisi/*)
			cpisiSvc.RegisterRoutes(mux)

			// Create console-specific mux with routes at /api/* for the
			// CPI-SI Console frontend (matches standalone dashboard's paths)
			cpisiConsoleMux = http.NewServeMux()
			cpisiSvc.RegisterConsoleRoutes(cpisiConsoleMux)

			// WebSocket on console mux too
			cpisiConsoleMux.HandleFunc("GET /ws", hub.HandleUpgrade)

			// Health and version on console mux
			cpisiConsoleMux.HandleFunc("GET /healthz", registry.HandleHealth())
			cpisiConsoleMux.HandleFunc("GET /api/version", func(w http.ResponseWriter, _ *http.Request) {
				server.WriteJSON(w, http.StatusOK, map[string]any{
					"version": version,
					"service": "cpisi-console",
				})
			})

			// Serve the standalone dashboard frontend on the console mux
			frontendFS, fsErr := fs.Sub(dashboardserver.FrontendFS, "static")
			if fsErr == nil {
				cpisiConsoleMux.Handle("/", http.FileServer(http.FS(frontendFS)))
				slog.Info("cpisi console frontend embedded")
			} else {
				slog.Warn("cpisi console frontend not available", "error", fsErr)
			}

			registry.Register(server.ServiceInfo{
				Name:        "cpisi",
				Version:     version,
				Routes:      21,
				Description: "CPI-SI state machine — queries, journals, real-time events",
			}, cpisiSvc)
			slog.Info("cpisi service enabled", "dataDir", dataDir)
		}
	}

	// ── Shared Endpoints ──────────────────────────────────────────────

	// WebSocket
	mux.HandleFunc("GET /ws", hub.HandleUpgrade)

	// Health check (aggregated from all services)
	mux.HandleFunc("GET /healthz", registry.HandleHealth())

	// Service registry
	mux.HandleFunc("GET /api/services", registry.HandleServices())

	// Metrics
	mux.HandleFunc("GET /api/metrics", metrics.HandleMetrics())

	// Version
	startTime := time.Now()
	mux.HandleFunc("GET /api/version", func(w http.ResponseWriter, _ *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		fmt.Fprintf(w, `{"version":"%s","uptime":"%s","services":%d,"wsClients":%d}`,
			version,
			time.Since(startTime).Round(time.Second),
			len(registry.Services()),
			hub.ClientCount(),
		)
	})

	// ── Static Files + Host Routing ───────────────────────────────────
	//
	// Host-based routing enables multiple sites on one server:
	//   - creativeworkzstudio.com → company website
	//   - dashboard.creativeworkzstudio.com → builder dashboard + API
	//   - cpisi.creativeworkzstudio.com → CPI-SI Console (dashboard frontend)
	//   - All other hosts → main API mux (catch-all, includes all routes)
	//
	// Without a website directory, everything goes through the main mux.
	// The CPI-SI Console is also available at /cpisi/ on the main mux
	// for local development (localhost:3847/cpisi/).

	var rootHandler http.Handler

	// Serve CPI-SI Console at /cpisi/ on the main mux for local dev access
	if cpisiConsoleMux != nil {
		frontendFS, fsErr := fs.Sub(dashboardserver.FrontendFS, "static")
		if fsErr == nil {
			// The /cpisi/ prefixed routes need their own sub-mux because the
			// frontend expects API calls at /api/* (not /cpisi/api/*).
			// For local dev, we mount a SPA handler that serves the frontend
			// files. API calls go to /api/cpisi/* on the main mux.
			cpisiSPA := http.StripPrefix("/cpisi", http.FileServer(http.FS(frontendFS)))
			mux.Handle("/cpisi/", cpisiSPA)
			mux.Handle("/cpisi", http.RedirectHandler("/cpisi/", http.StatusMovedPermanently))
		}
	}

	if websiteDir != "" {
		// Host-based routing: website on root domain, services on subdomains
		if builderPaths != nil {
			dashSPA := server.NewSPAHandler(builderPaths.WebRoot)
			mux.Handle("/", dashSPA)
			slog.Info("serving builder dashboard", "webRoot", builderPaths.WebRoot)
		}

		router := server.NewHostRouter()

		// Company website on root domain
		websiteSPA := server.NewSPAHandler(websiteDir)
		router.Handle("creativeworkzstudio.com", websiteSPA)
		router.Handle("www.creativeworkzstudio.com", websiteSPA)

		// CPI-SI Console on its own subdomain
		if cpisiConsoleMux != nil {
			router.Handle("cpisi.creativeworkzstudio.com", cpisiConsoleMux)
			slog.Info("cpisi console available at cpisi.creativeworkzstudio.com")
		}

		// Everything else (dashboard subdomain, localhost, IP) → API mux
		router.Handle("*", mux)

		rootHandler = router
		registry.Register(server.ServiceInfo{
			Name:        "website",
			Version:     version,
			Routes:      1,
			Description: "Company website — static files, marketing",
		}, nil)
		slog.Info("company website enabled", "dir", websiteDir)
	} else {
		// No website — serve dashboard at root
		if builderPaths != nil {
			dashSPA := server.NewSPAHandler(builderPaths.WebRoot)
			mux.Handle("/", dashSPA)
			slog.Info("serving builder dashboard", "webRoot", builderPaths.WebRoot)
		}
		rootHandler = mux
	}

	// ── Middleware Stack ──────────────────────────────────────────────
	handler := server.Chain(rootHandler,
		server.Recovery,
		server.RequestLogger,
		server.RequestID,
		metrics.Middleware,
		server.CORS,
		server.SecurityHeaders,
	)

	// ── HTTP Server ───────────────────────────────────────────────────
	//
	// Timeout rationale:
	//   ReadHeaderTimeout: 10s — enough for slow clients, short enough to
	//     prevent slowloris attacks that hold connections with partial headers.
	//   WriteTimeout: 0 (disabled) — WebSocket connections and process output
	//     streaming are long-lived; a write timeout would kill them mid-stream.
	//   IdleTimeout: 120s — keep-alive connections are recycled after 2 minutes
	//     of inactivity to prevent resource exhaustion from abandoned connections.
	srv := &http.Server{
		Addr:              fmt.Sprintf(":%d", cfg.Port),
		Handler:           handler,
		ReadHeaderTimeout: 10 * time.Second,
		WriteTimeout:      0,
		IdleTimeout:       120 * time.Second,
	}

	// Start server in goroutine
	go func() {
		services := registry.Services()
		names := make([]string, len(services))
		for i, s := range services {
			names[i] = s.Name
		}
		slog.Info("CWS Server listening",
			"addr", srv.Addr,
			"services", names,
		)
		if err := srv.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			slog.Error("server error", "error", err)
			os.Exit(1)
		}
	}()

	// ── Wait for Shutdown Signal ──────────────────────────────────────
	<-ctx.Done()
	slog.Info("shutdown signal received, draining connections...")

	// Graceful shutdown with 5 second timeout
	shutdownCtx, shutdownCancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer shutdownCancel()

	// Cancel all running processes
	if pm != nil {
		pm.CancelAll()
		slog.Info("all builder processes cancelled")
	}

	// Close CPI-SI bridge
	if cpisiBridge != nil {
		cpisiBridge.Close()
		slog.Info("cpisi bridge closed")
	}

	// Close WebSocket hub
	hub.Close()

	// Drain HTTP connections
	if err := srv.Shutdown(shutdownCtx); err != nil {
		slog.Error("shutdown error", "error", err)
	}

	slog.Info("CWS Server stopped")
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// "Let us rise up and build. So they strengthened their hands for this good work."
// — Nehemiah 2:18
