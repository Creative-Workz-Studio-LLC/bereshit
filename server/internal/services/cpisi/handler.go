// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-cpisi-handler
// Purpose: CPI-SI service — route registration and service entry point
// Biblical: Romans 12:2 — "Be ye transformed by the renewing of your mind"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// Handler files:
//   handler.go         — Service struct, NewService, RegisterRoutes (this file)
//   handler_state.go   — State machine query handlers (bridge delegation)
//   handler_journal.go — Journal CRUD, FTS5 search, and import handlers
//
// ═══════════════════════════════════════════════════════════════════════════

package cpisi

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"log/slog"
	"net/http"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Service is the CPI-SI HTTP service.
// It exposes the CPI-SI state machine through REST endpoints and provides
// journal persistence with FTS5 full-text search via SQLite.
type Service struct {
	bridge   *Bridge       // Bridge to claude-global DashboardService
	journals *JournalStore // SQLite-backed journal store (may be nil)
}

// NewService creates a new CPI-SI service backed by the given bridge.
func NewService(bridge *Bridge, journals *JournalStore) *Service {
	return &Service{bridge: bridge, journals: journals}
}

// RegisterRoutes registers all CPI-SI routes on the given mux.
//
// Route groups:
//   /api/cpisi/state           — Current state snapshot
//   /api/cpisi/sessions/*      — Session history, K:ALIGN, Hebrew transitions
//   /api/cpisi/choices         — Recent choices across sessions
//   /api/cpisi/patterns        — Active detected patterns
//   /api/cpisi/temporal        — Work pattern heatmap data
//   /api/cpisi/events          — Recent log events
//   /api/cpisi/data/*          — System data browser
//   /api/cpisi/exchanges       — Exchange timeline with message text + valence
//   /api/cpisi/live-patterns   — Real-time detected patterns from hooks
//   /api/cpisi/valence         — Valence distribution for a session
//   /api/cpisi/analytics       — Full analytics bundle from all 5 databases
//   /api/cpisi/journals/*      — Journal CRUD + FTS5 search (if journal store enabled)
func (s *Service) RegisterRoutes(mux *http.ServeMux) {
	// State machine queries
	mux.HandleFunc("GET /api/cpisi/state", s.handleState)
	mux.HandleFunc("GET /api/cpisi/sessions", s.handleSessions)
	mux.HandleFunc("GET /api/cpisi/sessions/{id}/kalign", s.handleKAlign)
	mux.HandleFunc("GET /api/cpisi/sessions/{id}/hebrew", s.handleHebrewTransitions)
	mux.HandleFunc("GET /api/cpisi/choices", s.handleChoices)
	mux.HandleFunc("GET /api/cpisi/patterns", s.handlePatterns)
	mux.HandleFunc("GET /api/cpisi/temporal", s.handleTemporal)
	mux.HandleFunc("GET /api/cpisi/events", s.handleEvents)
	mux.HandleFunc("GET /api/cpisi/data/{path...}", s.handleSystemData)

	// Extended data queries (multi-database)
	mux.HandleFunc("GET /api/cpisi/exchanges", s.handleExchanges)
	mux.HandleFunc("GET /api/cpisi/live-patterns", s.handleLivePatterns)
	mux.HandleFunc("GET /api/cpisi/valence", s.handleValence)
	mux.HandleFunc("GET /api/cpisi/analytics", s.handleAnalytics)

	// Journal CRUD + search (only registered if journal store is initialized)
	if s.journals != nil {
		mux.HandleFunc("POST /api/cpisi/journals", s.handleCreateJournal)
		mux.HandleFunc("GET /api/cpisi/journals", s.handleListJournals)
		mux.HandleFunc("GET /api/cpisi/journals/search", s.handleSearchJournals)
		mux.HandleFunc("GET /api/cpisi/journals/stats", s.handleJournalStats)
		mux.HandleFunc("GET /api/cpisi/journals/{id}", s.handleGetJournal)
		mux.HandleFunc("PUT /api/cpisi/journals/{id}", s.handleUpdateJournal)
		mux.HandleFunc("DELETE /api/cpisi/journals/{id}", s.handleDeleteJournal)
		mux.HandleFunc("POST /api/cpisi/journals/import", s.handleImportJournals)
		slog.Info("cpisi journal routes registered")
	}

	slog.Info("cpisi service routes registered",
		"stateRoutes", 13,
		"journalRoutes", s.journalRouteCount(),
	)
}

// RegisterConsoleRoutes registers CPI-SI routes at /api/* (without the /cpisi/ prefix)
// for use by the CPI-SI Console frontend. The standalone dashboard frontend expects
// routes at /api/state, /api/analytics, etc. — this method provides those mappings
// on a dedicated mux for host-based routing (cpisi.* subdomain).
func (s *Service) RegisterConsoleRoutes(mux *http.ServeMux) {
	// Core state queries
	mux.HandleFunc("GET /api/state", s.handleState)
	mux.HandleFunc("GET /api/sessions", s.handleSessions)
	mux.HandleFunc("GET /api/sessions/{id}/kalign", s.handleKAlign)
	mux.HandleFunc("GET /api/sessions/{id}/hebrew", s.handleHebrewTransitions)
	mux.HandleFunc("GET /api/choices", s.handleChoices)
	mux.HandleFunc("GET /api/patterns", s.handlePatterns)
	mux.HandleFunc("GET /api/temporal", s.handleTemporal)
	mux.HandleFunc("GET /api/events", s.handleEvents)
	mux.HandleFunc("GET /api/data/{path...}", s.handleSystemData)

	// Extended data queries
	mux.HandleFunc("GET /api/exchanges", s.handleExchanges)
	mux.HandleFunc("GET /api/live-patterns", s.handleLivePatterns)
	mux.HandleFunc("GET /api/valence", s.handleValence)
	mux.HandleFunc("GET /api/analytics", s.handleAnalytics)

	// Journal CRUD + search
	if s.journals != nil {
		mux.HandleFunc("POST /api/journals", s.handleCreateJournal)
		mux.HandleFunc("GET /api/journals", s.handleListJournals)
		mux.HandleFunc("GET /api/journals/search", s.handleSearchJournals)
		mux.HandleFunc("GET /api/journals/stats", s.handleJournalStats)
		mux.HandleFunc("GET /api/journals/{id}", s.handleGetJournal)
		mux.HandleFunc("PUT /api/journals/{id}", s.handleUpdateJournal)
		mux.HandleFunc("DELETE /api/journals/{id}", s.handleDeleteJournal)
		mux.HandleFunc("POST /api/journals/import", s.handleImportJournals)
	}

	slog.Info("cpisi console routes registered")
}

// Health returns the CPI-SI service health status.
func (s *Service) Health() map[string]any {
	health := map[string]any{
		"status":   "ok",
		"journals": s.journals != nil,
	}

	// Check if DashboardService can query state
	if snap, err := s.bridge.Service().CurrentState(); err == nil && snap != nil {
		health["sessionActive"] = snap.SessionID != ""
		health["hebrewState"] = snap.HebrewState
		health["healthScore"] = snap.HealthScore
	} else {
		health["status"] = "degraded"
		if err != nil {
			health["error"] = err.Error()
		}
	}

	return health
}

// journalRouteCount returns how many journal routes are registered.
func (s *Service) journalRouteCount() int {
	if s.journals != nil {
		return 8
	}
	return 0
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
