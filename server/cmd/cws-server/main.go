// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-main
// Purpose: CWS Server entry point — company infrastructure foundation
// Biblical: Nehemiah 2:18 — "Let us rise up and build"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"flag"
	"fmt"
	"log/slog"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"

	"cws.studio/server/internal/config"
	"cws.studio/server/internal/server"
	"cws.studio/server/internal/services/builder"
	"cws.studio/server/internal/services/cpisi"
	"cws.studio/server/internal/ws"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

const version = "0.1.0"

func main() {
	// ── Flags ─────────────────────────────────────────────────────────
	cfg := config.DefaultConfig()

	var websiteDir string

	flag.IntVar(&cfg.Port, "port", cfg.Port, "Server port")
	flag.StringVar(&cfg.BuilderDir, "builder-dir", cfg.BuilderDir, "Path to company-docs/ project directory")
	flag.StringVar(&websiteDir, "website-dir", "", "Path to company website static files")
	flag.BoolVar(&cfg.EnableCPISI, "cpisi", cfg.EnableCPISI, "Enable CPI-SI service")
	flag.BoolVar(&cfg.DevMode, "dev", cfg.DevMode, "Development mode (filesystem serving)")
	flag.Parse()

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
	activeServices := []string{}

	// Builder Dashboard service
	var pm *builder.ProcessManager
	if builderPaths != nil {
		pm = builder.NewProcessManager(hub)
		builderSvc := builder.NewService(pm, builderPaths)
		builderSvc.RegisterRoutes(mux)
		activeServices = append(activeServices, "builder")
		slog.Info("builder service enabled", "projectDir", builderPaths.ProjectDir)
	}

	// CPI-SI service
	var cpisiBridge *cpisi.Bridge
	if cfg.EnableCPISI {
		bridge, err := cpisi.NewBridge(ctx, hub)
		if err != nil {
			slog.Warn("cpisi service disabled", "error", err)
		} else {
			cpisiBridge = bridge
			cpisiSvc := cpisi.NewService(bridge)
			cpisiSvc.RegisterRoutes(mux)
			activeServices = append(activeServices, "cpisi")
			slog.Info("cpisi service enabled")
		}
	}

	// ── Shared Endpoints ──────────────────────────────────────────────

	// WebSocket
	mux.HandleFunc("GET /ws", hub.HandleUpgrade)

	// Health check
	mux.HandleFunc("GET /healthz", func(w http.ResponseWriter, _ *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(http.StatusOK)
		fmt.Fprintf(w, `{"status":"ok"}`)
	})

	// Version
	startTime := time.Now()
	mux.HandleFunc("GET /api/version", func(w http.ResponseWriter, _ *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		fmt.Fprintf(w, `{"version":"%s","uptime":"%s","services":%d,"wsClients":%d}`,
			version,
			time.Since(startTime).Round(time.Second),
			len(activeServices),
			hub.ClientCount(),
		)
	})

	// ── Static Files + Host Routing ───────────────────────────────────
	//
	// If a website directory is provided, use host-based routing:
	//   - Root domain → company website
	//   - dashboard.* subdomain → builder dashboard + API
	//   - All other hosts → API mux (catch-all)
	//
	// Without a website directory, everything goes through the API mux.

	var rootHandler http.Handler

	if websiteDir != "" {
		// Host-based routing: website on root domain, dashboard on subdomain
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

		// Everything else (dashboard subdomain, localhost, IP) → API mux
		router.Handle("*", mux)

		rootHandler = router
		activeServices = append(activeServices, "website")
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
		server.CORS,
	)

	// ── HTTP Server ───────────────────────────────────────────────────
	srv := &http.Server{
		Addr:              fmt.Sprintf(":%d", cfg.Port),
		Handler:           handler,
		ReadHeaderTimeout: 10 * time.Second,
		WriteTimeout:      0, // Disabled for WebSocket + streaming
		IdleTimeout:       120 * time.Second,
	}

	// Start server in goroutine
	go func() {
		slog.Info("CWS Server listening",
			"addr", srv.Addr,
			"services", activeServices,
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
