// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-cpisi-handler-state
// Purpose: HTTP handlers for CPI-SI state machine queries
// Biblical: Psalm 139:23 — "Search me, O God, and know my heart"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package cpisi

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"net/http"

	"cws.studio/server/internal/server"

	"cws.studio/pkg/dashboard"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// State Handlers — thin delegation to DashboardService via Bridge
//
// Each handler follows the same pattern: extract params, call bridge method,
// write response. The DashboardService (claude-global/pkg/dashboard) owns
// all state logic; these handlers are purely HTTP transport.
// ───────────────────────────────────────────────────────────────────────────

// handleState returns the current CPI-SI StateSnapshot (30+ fields).
func (s *Service) handleState(w http.ResponseWriter, _ *http.Request) {
	snap, err := s.bridge.Service().CurrentState()
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, snap)
}

// handleSessions returns recent session history, ordered by start time.
func (s *Service) handleSessions(w http.ResponseWriter, r *http.Request) {
	limit := server.QueryInt(r, "limit", 20)
	sessions, err := s.bridge.Service().SessionHistory(limit)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, sessions)
}

// handleKAlign returns K:ALIGN evolution records for a specific session.
func (s *Service) handleKAlign(w http.ResponseWriter, r *http.Request) {
	sessionID := r.PathValue("id")
	records, err := s.bridge.Service().KAlignHistory(sessionID)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, records)
}

// handleHebrewTransitions returns Hebrew state transitions for a session.
// States flow through the 7 Hebrew positions: shavar → yashar → tov.
func (s *Service) handleHebrewTransitions(w http.ResponseWriter, r *http.Request) {
	sessionID := r.PathValue("id")
	transitions, err := s.bridge.Service().HebrewTransitions(sessionID)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, transitions)
}

// handleChoices returns recent choice records across all sessions.
func (s *Service) handleChoices(w http.ResponseWriter, r *http.Request) {
	limit := server.QueryInt(r, "limit", 20)
	choices, err := s.bridge.Service().RecentChoices(limit)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, choices)
}

// handlePatterns returns currently active detected patterns.
func (s *Service) handlePatterns(w http.ResponseWriter, _ *http.Request) {
	patterns, err := s.bridge.Service().ActivePatterns()
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, patterns)
}

// handleTemporal returns temporal work pattern data (heatmap-ready).
func (s *Service) handleTemporal(w http.ResponseWriter, _ *http.Request) {
	patterns, err := s.bridge.Service().TemporalWorkPatterns()
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, patterns)
}

// handleEvents returns recent log events from the CPI-SI runtime.
func (s *Service) handleEvents(w http.ResponseWriter, r *http.Request) {
	limit := server.QueryInt(r, "limit", 50)
	events, err := s.bridge.Service().RecentEvents(limit)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, events)
}

// handleSystemData returns a specific data entry from the CPI-SI system.
// The {path...} wildcard captures the full remaining URL path.
func (s *Service) handleSystemData(w http.ResponseWriter, r *http.Request) {
	path := r.PathValue("path")
	entry, err := s.bridge.Service().SystemData(path)
	if err != nil {
		server.WriteError(w, http.StatusNotFound, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, entry)
}

// ───────────────────────────────────────────────────────────────────────────
// Extended Handlers — exchange timeline, live patterns, valence, analytics
//
// These endpoints expose the enriched data from the multi-database architecture
// (sessions.db, cognition.db, growth.db) via the DashboardService methods
// added during the v2/v3 schema evolution.
// ───────────────────────────────────────────────────────────────────────────

// handleExchanges returns recent exchanges with message text and valence for a session.
func (s *Service) handleExchanges(w http.ResponseWriter, r *http.Request) {
	sessionID := r.URL.Query().Get("session_id")
	limit := server.QueryInt(r, "limit", 50)

	// Default to current session if not specified.
	if sessionID == "" {
		snap, _ := s.bridge.Service().CurrentState()
		if snap != nil {
			sessionID = snap.SessionID
		}
	}

	exchanges, err := s.bridge.Service().RecentExchanges(sessionID, limit)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, exchanges)
}

// handleLivePatterns returns real-time detected patterns from growth.db.
// These are patterns with live_* prefix, triggered by hooks during active sessions.
func (s *Service) handleLivePatterns(w http.ResponseWriter, _ *http.Request) {
	patterns, err := s.bridge.Service().LivePatterns()
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, patterns)
}

// handleValence returns the valence distribution (positive/neutral/negative)
// for a session's exchanges.
func (s *Service) handleValence(w http.ResponseWriter, r *http.Request) {
	sessionID := r.URL.Query().Get("session_id")

	// Default to current session.
	if sessionID == "" {
		snap, _ := s.bridge.Service().CurrentState()
		if snap != nil {
			sessionID = snap.SessionID
		}
	}

	if sessionID == "" {
		server.WriteJSON(w, http.StatusOK, map[string]int{"positive": 0, "neutral": 0, "negative": 0})
		return
	}

	dist, err := s.bridge.Service().ValenceDistribution(sessionID)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, dist)
}

// handleAnalytics returns the full AnalyticsBundle — all aggregated analytics
// from all 5 domain databases. This is the comprehensive data endpoint that
// powers the dashboard's analytics charts.
func (s *Service) handleAnalytics(w http.ResponseWriter, r *http.Request) {
	sessionID := r.URL.Query().Get("session_id")

	// Default to current session.
	if sessionID == "" {
		snap, _ := s.bridge.Service().CurrentState()
		if snap != nil {
			sessionID = snap.SessionID
		}
	}

	bundle, err := dashboard.LoadAnalyticsWithMultiDB(r.Context(), s.bridge.Service(), sessionID)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, bundle)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
