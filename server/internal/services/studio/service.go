// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-studio-service
// Purpose: CWS Studio service — manages Deno subprocess and reverse proxy
// Biblical: Psalm 127:1 — "Except the LORD build the house"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-18
//
// Architecture:
//   The studio is a Deno-based web server (cws-struct studio) that wraps
//   the cws-struct lint engine and cwe-creator-tools scaffolders. This Go
//   service manages it as a subprocess and reverse-proxies all requests.
//
//   In production, accessed via studio.creativeworkzstudio.com (subdomain).
//   For local dev, available at /studio/ on the main mux.
//
// ═══════════════════════════════════════════════════════════════════════════

package studio

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"fmt"
	"log/slog"
	"net/http"
	"net/http/httputil"
	"net/url"
	"os"
	"os/exec"
	"path/filepath"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Constants
// ───────────────────────────────────────────────────────────────────────────

// DefaultPort is the internal port for the Deno studio subprocess.
// Not exposed externally — the Go server proxies to it.
const DefaultPort = 4201

// maxStartupWait is how long to wait for the Deno subprocess health check.
const maxStartupWait = 5 * time.Second

// ───────────────────────────────────────────────────────────────────────────
// Service — subprocess lifecycle + reverse proxy
// ───────────────────────────────────────────────────────────────────────────

// Service manages the CWS Studio Deno subprocess and proxies requests to it.
type Service struct {
	cmd       *exec.Cmd
	port      int
	proxy     *httputil.ReverseProxy
	cancel    context.CancelFunc
	structDir string
}

// NewService starts the CWS Studio Deno subprocess and returns a service
// that proxies HTTP requests to it.
//
// The structDir parameter should point to the cws-struct directory containing
// mod.ts. The subprocess runs on the specified internal port, which is not
// exposed externally — the Go server reverse-proxies to it.
//
// Returns an error if:
//   - mod.ts doesn't exist at structDir
//   - Deno is not available
//   - Subprocess fails to start
//   - Health check doesn't respond within maxStartupWait
func NewService(ctx context.Context, structDir string, port int) (*Service, error) {
	// Verify cws-struct exists
	modPath := filepath.Join(structDir, "mod.ts")
	if _, err := os.Stat(modPath); err != nil {
		return nil, fmt.Errorf("cws-struct not found at %s: %w", modPath, err)
	}

	// Verify Deno is available
	denoPath, err := exec.LookPath("deno")
	if err != nil {
		return nil, fmt.Errorf("deno not found in PATH: %w", err)
	}

	// Create a cancellable context for the subprocess
	subCtx, cancel := context.WithCancel(ctx)

	cmd := exec.CommandContext(subCtx, denoPath, "run",
		"--allow-read", "--allow-write", "--allow-run", "--allow-net",
		modPath, "studio", "--port", fmt.Sprintf("%d", port),
	)
	cmd.Dir = structDir
	cmd.Stdout = os.Stdout // Forward logs to Go server stdout
	cmd.Stderr = os.Stderr

	if err := cmd.Start(); err != nil {
		cancel()
		return nil, fmt.Errorf("failed to start studio subprocess: %w", err)
	}

	slog.Info("studio subprocess starting",
		"pid", cmd.Process.Pid,
		"port", port,
		"structDir", structDir,
	)

	// Set up reverse proxy
	target, _ := url.Parse(fmt.Sprintf("http://localhost:%d", port))
	proxy := httputil.NewSingleHostReverseProxy(target)

	// Custom error handler — don't expose internal errors to clients
	proxy.ErrorHandler = func(w http.ResponseWriter, _ *http.Request, err error) {
		slog.Error("studio proxy error", "error", err)
		http.Error(w, "Studio service unavailable", http.StatusBadGateway)
	}

	// Wait for the subprocess to be ready
	healthURL := fmt.Sprintf("http://localhost:%d/api/health", port)
	client := &http.Client{Timeout: 500 * time.Millisecond}
	deadline := time.Now().Add(maxStartupWait)
	ready := false

	for time.Now().Before(deadline) {
		resp, err := client.Get(healthURL)
		if err == nil {
			resp.Body.Close()
			if resp.StatusCode == 200 {
				ready = true
				break
			}
		}
		time.Sleep(200 * time.Millisecond)
	}

	if !ready {
		cancel()
		_ = cmd.Wait()
		return nil, fmt.Errorf("studio subprocess failed to become ready within %s", maxStartupWait)
	}

	slog.Info("studio subprocess ready",
		"pid", cmd.Process.Pid,
		"port", port,
	)

	return &Service{
		cmd:       cmd,
		port:      port,
		proxy:     proxy,
		cancel:    cancel,
		structDir: structDir,
	}, nil
}

// ───────────────────────────────────────────────────────────────────────────
// Route Registration
// ───────────────────────────────────────────────────────────────────────────

// RegisterRoutes registers the studio reverse proxy on the given mux.
// Proxies all /studio/* requests to the Deno subprocess.
func (s *Service) RegisterRoutes(mux *http.ServeMux) {
	// Proxy all requests under /studio/ to the Deno server
	mux.Handle("/studio/", http.StripPrefix("/studio", s.proxy))
	mux.Handle("/studio", http.RedirectHandler("/studio/", http.StatusMovedPermanently))
	slog.Info("studio service routes registered (main mux)")
}

// Mux returns the reverse proxy as an http.Handler for subdomain routing.
// Used when studio gets its own subdomain (studio.creativeworkzstudio.com).
func (s *Service) Mux() http.Handler {
	return s.proxy
}

// ───────────────────────────────────────────────────────────────────────────
// Health + Lifecycle
// ───────────────────────────────────────────────────────────────────────────

// Health returns the health status of the studio subprocess.
// Implements server.HealthChecker.
func (s *Service) Health() map[string]any {
	// Check if subprocess has exited
	if s.cmd.ProcessState != nil && s.cmd.ProcessState.Exited() {
		return map[string]any{
			"status": "down",
			"error":  "subprocess exited",
			"port":   s.port,
		}
	}

	// Quick health check
	client := &http.Client{Timeout: 1 * time.Second}
	resp, err := client.Get(fmt.Sprintf("http://localhost:%d/api/health", s.port))
	if err != nil {
		return map[string]any{
			"status": "degraded",
			"error":  err.Error(),
			"port":   s.port,
		}
	}
	resp.Body.Close()

	return map[string]any{
		"status": "ok",
		"port":   s.port,
		"pid":    s.cmd.Process.Pid,
	}
}

// Close stops the studio subprocess gracefully.
func (s *Service) Close() {
	s.cancel()
	if s.cmd.Process != nil {
		_ = s.cmd.Wait()
	}
	slog.Info("studio subprocess stopped")
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// "Except the LORD build the house, they labour in vain that build it."
// — Psalm 127:1
//
// The tool that builds tools. Foundation under the interface.
// ═══════════════════════════════════════════════════════════════════════════
