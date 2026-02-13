// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-serve
// Purpose: HTTP server for web GUI — serves embedded frontend + API
// Biblical: Nehemiah 3:1 — "Then Eliashib the high priest rose up...
//           and they builded"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bufio"
	"context"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"io/fs"
	"log"
	"net/http"
	"os/exec"
	"runtime"
	"time"

	"cws.studio/server/internal/config"
	"cws.studio/server/internal/ops"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

func cmdServe(ctx context.Context, host config.Host, args []string) {
	fs := flag.NewFlagSet("serve", flag.ExitOnError)
	port := fs.Int("port", 4847, "HTTP listen port")
	open := fs.Bool("open", false, "Open browser automatically")
	_ = fs.Parse(args)

	mux := http.NewServeMux()

	// ── API Endpoints ────────────────────────────────────────────────
	registerAPI(mux, ctx, host)

	// ── Frontend ─────────────────────────────────────────────────────
	serveFrontend(mux)

	// ── Server ───────────────────────────────────────────────────────
	addr := fmt.Sprintf(":%d", *port)
	srv := &http.Server{
		Addr:              addr,
		Handler:           mux,
		ReadHeaderTimeout: 10 * time.Second,
		IdleTimeout:       120 * time.Second,
	}

	url := fmt.Sprintf("http://localhost:%d", *port)
	fmt.Printf("CWS Ops GUI starting on %s\n", url)
	fmt.Printf("  Target: %s (%s)\n", host.Name, host.Address)
	fmt.Println()

	if *open {
		openBrowser(url)
	}

	go func() {
		if err := srv.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			log.Fatalf("server error: %v", err)
		}
	}()

	<-ctx.Done()
	fmt.Println("\nShutting down...")
	shutdownCtx, cancel := context.WithTimeout(context.Background(), 3*time.Second)
	defer cancel()
	_ = srv.Shutdown(shutdownCtx)
}

// registerAPI sets up the JSON API endpoints that the frontend calls.
func registerAPI(mux *http.ServeMux, ctx context.Context, host config.Host) {
	// GET /api/ops/status — full infrastructure status
	mux.HandleFunc("GET /api/ops/status", func(w http.ResponseWriter, _ *http.Request) {
		status, err := ops.GetFullStatus(ctx, host)
		if err != nil {
			writeJSON(w, http.StatusInternalServerError, map[string]string{"error": err.Error()})
			return
		}
		writeJSON(w, http.StatusOK, status)
	})

	// GET /api/ops/services — list service statuses
	mux.HandleFunc("GET /api/ops/services", func(w http.ResponseWriter, _ *http.Request) {
		services, err := ops.GetServiceStatuses(ctx, host)
		if err != nil {
			writeJSON(w, http.StatusInternalServerError, map[string]string{"error": err.Error()})
			return
		}
		writeJSON(w, http.StatusOK, services)
	})

	// GET /api/ops/resources — system resources
	mux.HandleFunc("GET /api/ops/resources", func(w http.ResponseWriter, _ *http.Request) {
		resources, err := ops.GetResources(ctx, host)
		if err != nil {
			writeJSON(w, http.StatusInternalServerError, map[string]string{"error": err.Error()})
			return
		}
		writeJSON(w, http.StatusOK, resources)
	})

	// GET /api/ops/backups — backup status
	mux.HandleFunc("GET /api/ops/backups", func(w http.ResponseWriter, _ *http.Request) {
		status, err := ops.GetBackupStatus(ctx, host)
		if err != nil {
			writeJSON(w, http.StatusInternalServerError, map[string]string{"error": err.Error()})
			return
		}
		writeJSON(w, http.StatusOK, status)
	})

	// GET /api/ops/logs?service=NAME&lines=N — recent logs
	mux.HandleFunc("GET /api/ops/logs", func(w http.ResponseWriter, r *http.Request) {
		service := r.URL.Query().Get("service")
		if service == "" {
			service = "cws-server"
		}
		lines := 50
		if l := r.URL.Query().Get("lines"); l != "" {
			fmt.Sscanf(l, "%d", &lines)
		}

		output, err := ops.GetRecentLogs(ctx, host, service, lines)
		if err != nil {
			writeJSON(w, http.StatusInternalServerError, map[string]string{"error": err.Error()})
			return
		}
		writeJSON(w, http.StatusOK, map[string]string{"logs": output})
	})

	// GET /api/ops/version
	mux.HandleFunc("GET /api/ops/version", func(w http.ResponseWriter, _ *http.Request) {
		writeJSON(w, http.StatusOK, map[string]string{
			"version": version,
			"target":  host.Name,
		})
	})

	// ── Mutation Endpoints ───────────────────────────────────────────

	// POST /api/ops/services/{name}/{action} — manage a service
	mux.HandleFunc("POST /api/ops/services/{name}/{action}", func(w http.ResponseWriter, r *http.Request) {
		name := r.PathValue("name")
		action := r.PathValue("action")

		if !ops.IsManaged(name) {
			writeJSON(w, http.StatusBadRequest, map[string]string{
				"error": fmt.Sprintf("unknown service: %s", name),
			})
			return
		}

		switch action {
		case "restart", "stop", "start":
		default:
			writeJSON(w, http.StatusBadRequest, map[string]string{
				"error": fmt.Sprintf("invalid action: %s (use restart, stop, start)", action),
			})
			return
		}

		result, err := ops.ManageService(ctx, host, name, ops.ServiceAction(action))
		if err != nil {
			writeJSON(w, http.StatusInternalServerError, map[string]string{"error": err.Error()})
			return
		}
		if !result.OK() {
			writeJSON(w, http.StatusInternalServerError, map[string]string{
				"error": fmt.Sprintf("exit %d: %s", result.ExitCode, result.Stderr),
			})
			return
		}
		writeJSON(w, http.StatusOK, map[string]string{"status": "ok"})
	})

	// GET /api/ops/logs/stream?service=NAME&lines=N — SSE log streaming
	mux.HandleFunc("GET /api/ops/logs/stream", func(w http.ResponseWriter, r *http.Request) {
		service := r.URL.Query().Get("service")
		if service == "" {
			service = "cws-server"
		}
		lines := 50
		if l := r.URL.Query().Get("lines"); l != "" {
			fmt.Sscanf(l, "%d", &lines)
		}

		flusher, ok := w.(http.Flusher)
		if !ok {
			writeJSON(w, http.StatusInternalServerError, map[string]string{"error": "streaming not supported"})
			return
		}

		w.Header().Set("Content-Type", "text/event-stream")
		w.Header().Set("Cache-Control", "no-cache")
		w.Header().Set("Connection", "keep-alive")
		flusher.Flush()

		// Use request context so client disconnect stops streaming
		streamCtx, cancel := context.WithCancel(r.Context())
		defer cancel()

		opts := ops.LogOptions{
			Service: service,
			Follow:  true,
			Lines:   lines,
		}

		pr, pw := io.Pipe()
		go func() {
			_ = ops.StreamLogs(streamCtx, host, opts, pw)
			pw.Close()
		}()

		scanner := bufio.NewScanner(pr)
		for scanner.Scan() {
			fmt.Fprintf(w, "data: %s\n\n", scanner.Text())
			flusher.Flush()
		}
	})

	// POST /api/ops/deploy/{target} — SSE deploy streaming
	mux.HandleFunc("POST /api/ops/deploy/{target}", func(w http.ResponseWriter, r *http.Request) {
		targetStr := r.PathValue("target")
		if !ops.IsValidDeployTarget(targetStr) {
			writeJSON(w, http.StatusBadRequest, map[string]string{
				"error": fmt.Sprintf("invalid target: %s", targetStr),
			})
			return
		}
		target := ops.DeployTarget(targetStr)

		flusher, ok := w.(http.Flusher)
		if !ok {
			writeJSON(w, http.StatusInternalServerError, map[string]string{"error": "streaming not supported"})
			return
		}

		w.Header().Set("Content-Type", "text/event-stream")
		w.Header().Set("Cache-Control", "no-cache")
		w.Header().Set("Connection", "keep-alive")

		fmt.Fprintf(w, "data: Deploying %s to %s...\n\n", target, host.Name)
		flusher.Flush()

		pr, pw := io.Pipe()
		go func() {
			err := ops.Deploy(ctx, host, target, pw)
			if err != nil {
				fmt.Fprintf(pw, "\nDeploy error: %v\n", err)
			}
			pw.Close()
		}()

		scanner := bufio.NewScanner(pr)
		for scanner.Scan() {
			fmt.Fprintf(w, "data: %s\n\n", scanner.Text())
			flusher.Flush()
		}

		fmt.Fprintf(w, "event: done\ndata: complete\n\n")
		flusher.Flush()
	})

	// POST /api/ops/backups/verify — verify backup integrity
	mux.HandleFunc("POST /api/ops/backups/verify", func(w http.ResponseWriter, _ *http.Request) {
		output, err := ops.VerifyBackups(ctx, host)
		if err != nil {
			writeJSON(w, http.StatusInternalServerError, map[string]string{"error": err.Error()})
			return
		}
		writeJSON(w, http.StatusOK, map[string]string{"output": output})
	})
}

// serveFrontend serves the embedded Vite build output.
func serveFrontend(mux *http.ServeMux) {
	staticDir, err := fs.Sub(StaticFS, "static")
	if err != nil {
		log.Printf("warning: embedded frontend not available: %v", err)
		mux.HandleFunc("/", func(w http.ResponseWriter, _ *http.Request) {
			w.Header().Set("Content-Type", "text/html")
			fmt.Fprint(w, `<h1>CWS Ops</h1><p>Frontend not built. Run <code>make ops-frontend</code> first.</p>`)
		})
		return
	}

	fileServer := http.FileServer(http.FS(staticDir))

	// SPA: serve index.html for any path that doesn't match a static file
	mux.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		// Try to serve the file directly
		path := r.URL.Path
		if path == "/" {
			fileServer.ServeHTTP(w, r)
			return
		}

		// Check if file exists in the embedded FS
		f, err := staticDir.Open(path[1:]) // strip leading /
		if err != nil {
			// Not found — serve index.html for SPA routing
			r.URL.Path = "/"
			fileServer.ServeHTTP(w, r)
			return
		}
		f.Close()

		// File exists — serve it
		fileServer.ServeHTTP(w, r)
	})
}

// ── Helpers ──────────────────────────────────────────────────────────

func writeJSON(w http.ResponseWriter, status int, data any) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	_ = json.NewEncoder(w).Encode(data)
}

func openBrowser(url string) {
	var cmd *exec.Cmd
	switch runtime.GOOS {
	case "linux":
		cmd = exec.Command("xdg-open", url)
	case "darwin":
		cmd = exec.Command("open", url)
	default:
		return
	}
	_ = cmd.Start()
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
