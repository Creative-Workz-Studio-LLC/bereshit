// ============================================================================
// METADATA - CPI-SI Dashboard HTTP Server
// ============================================================================
//
// Key: claude-global-dashboard-server
// Purpose: HTTP server with REST API, WebSocket, and embedded frontend
// Biblical: Habakkuk 2:2 - "Write the vision, and make it plain upon tables"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// Routes:
//   GET /api/state      → Current StateSnapshot
//   GET /api/history    → Session history (?limit=N, default 50)
//   GET /api/analytics  → AnalyticsBundle
//   GET /api/systemdata → SystemDataEntry (?path=)
//   GET /api/events     → Recent LogEvents (?limit=N, default 100)
//   GET /api/path       → Current session PathSummary
//   GET /ws             → WebSocket upgrade
//   GET /               → Static frontend files
//
// ============================================================================

package server

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"fmt"
	"io/fs"
	"net/http"

	"github.com/creativeworkzstudio/claude-global/pkg/dashboard"
)

// ============================================================================
// BODY
// ============================================================================

// Server is the CPI-SI dashboard HTTP server.
type Server struct {
	httpServer *http.Server
	svc        *dashboard.DashboardService
	hub        *Hub
}

// New creates a new dashboard HTTP server.
func New(svc *dashboard.DashboardService, port int) *Server {
	s := &Server{
		svc: svc,
		hub: NewHub(svc),
	}

	mux := http.NewServeMux()

	// REST API routes
	mux.HandleFunc("GET /api/state", s.withCORS(handleState(svc)))
	mux.HandleFunc("GET /api/history", s.withCORS(handleHistory(svc)))
	mux.HandleFunc("GET /api/analytics", s.withCORS(handleAnalytics(svc)))
	mux.HandleFunc("GET /api/systemdata", s.withCORS(handleSystemData(svc)))
	mux.HandleFunc("GET /api/events", s.withCORS(handleEvents(svc)))
	mux.HandleFunc("GET /api/path", s.withCORS(handlePath()))

	// WebSocket
	mux.HandleFunc("GET /ws", s.handleWebSocket)

	// Static frontend files
	frontendSub, err := fs.Sub(FrontendFS, "static")
	if err != nil {
		// Fall back to root if sub fails
		mux.Handle("GET /", http.FileServer(http.FS(FrontendFS)))
	} else {
		mux.Handle("GET /", http.FileServer(http.FS(frontendSub)))
	}

	s.httpServer = &http.Server{
		Addr:    fmt.Sprintf(":%d", port),
		Handler: mux,
	}

	return s
}

// ListenAndServe starts the HTTP server.
// Returns http.ErrServerClosed on graceful shutdown.
func (s *Server) ListenAndServe() error {
	// Start the WebSocket hub
	go s.hub.Run()

	err := s.httpServer.ListenAndServe()
	if err == http.ErrServerClosed {
		return nil
	}
	return err
}

// Shutdown gracefully stops the HTTP server.
func (s *Server) Shutdown(ctx context.Context) error {
	s.hub.Stop()
	return s.httpServer.Shutdown(ctx)
}

// withCORS wraps a handler with CORS headers for local development.
func (s *Server) withCORS(handler http.HandlerFunc) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Access-Control-Allow-Methods", "GET, OPTIONS")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type")

		if r.Method == http.MethodOptions {
			w.WriteHeader(http.StatusOK)
			return
		}

		handler(w, r)
	}
}

// handleWebSocket upgrades the connection and registers with the hub.
func (s *Server) handleWebSocket(w http.ResponseWriter, r *http.Request) {
	// Set CORS for WebSocket upgrade
	w.Header().Set("Access-Control-Allow-Origin", "*")

	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		return
	}

	client := &Client{
		hub:  s.hub,
		conn: conn,
		send: make(chan []byte, 256),
	}

	s.hub.register <- client

	// Start client goroutines
	go client.writePump()
	go client.readPump()
}

// ============================================================================
// CLOSING
// ============================================================================
