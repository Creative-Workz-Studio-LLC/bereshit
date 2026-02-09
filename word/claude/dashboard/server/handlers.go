// ============================================================================
// METADATA - CPI-SI Dashboard REST API Handlers
// ============================================================================
//
// Key: claude-global-dashboard-handlers
// Purpose: REST API handler functions for dashboard data endpoints
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// Each handler:
//   - Calls DashboardService methods
//   - Marshals response to JSON
//   - Returns appropriate error codes on failure
//   - Sets Content-Type: application/json
//
// ============================================================================

package server

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"net/http"
	"strconv"

	"cws.studio/pkg/dashboard"
)

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────────────────

// writeJSON marshals data to JSON and writes it to the response.
func writeJSON(w http.ResponseWriter, status int, data interface{}) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)

	if err := json.NewEncoder(w).Encode(data); err != nil {
		// If encoding fails after headers are sent, we can only log
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}
}

// writeError writes a JSON error response.
func writeError(w http.ResponseWriter, status int, message string) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	json.NewEncoder(w).Encode(map[string]string{"error": message})
}

// parseIntParam reads an integer query parameter with a default value.
func parseIntParam(r *http.Request, key string, defaultVal int) int {
	val := r.URL.Query().Get(key)
	if val == "" {
		return defaultVal
	}
	n, err := strconv.Atoi(val)
	if err != nil || n < 0 {
		return defaultVal
	}
	return n
}

// ────────────────────────────────────────────────────────────────────────────
// Route Handlers
// ────────────────────────────────────────────────────────────────────────────

// handleState returns the current CPI-SI StateSnapshot.
// GET /api/state
func handleState(svc *dashboard.DashboardService) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		snap, err := svc.CurrentState()
		if err != nil {
			writeError(w, http.StatusInternalServerError, "Failed to read state: "+err.Error())
			return
		}
		writeJSON(w, http.StatusOK, snap)
	}
}

// handleHistory returns session history from SQLite.
// GET /api/history?limit=50
func handleHistory(svc *dashboard.DashboardService) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		limit := parseIntParam(r, "limit", 50)

		sessions, err := svc.SessionHistory(limit)
		if err != nil {
			writeError(w, http.StatusInternalServerError, "Failed to read history: "+err.Error())
			return
		}
		writeJSON(w, http.StatusOK, sessions)
	}
}

// handleAnalytics returns aggregated analytics from SQLite and history.jsonc.
// GET /api/analytics
func handleAnalytics(svc *dashboard.DashboardService) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		// Get current state to extract session ID
		snap, _ := svc.CurrentState()
		sessionID := ""
		if snap != nil {
			sessionID = snap.SessionID
		}

		bundle, err := dashboard.LoadAnalyticsWithMultiDB(r.Context(), svc, sessionID)
		if err != nil {
			writeError(w, http.StatusInternalServerError, "Failed to load analytics: "+err.Error())
			return
		}
		writeJSON(w, http.StatusOK, bundle)
	}
}

// handleSystemData returns a directory listing or file content from system/data.
// GET /api/systemdata?path=temporal/patterns
func handleSystemData(svc *dashboard.DashboardService) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		relativePath := r.URL.Query().Get("path")

		entry, err := svc.SystemData(relativePath)
		if err != nil {
			writeError(w, http.StatusNotFound, "System data not found: "+err.Error())
			return
		}
		writeJSON(w, http.StatusOK, entry)
	}
}

// handleEvents returns recent log events from today's session log.
// GET /api/events?limit=100
func handleEvents(svc *dashboard.DashboardService) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		limit := parseIntParam(r, "limit", 100)

		events, err := svc.RecentEvents(limit)
		if err != nil {
			writeError(w, http.StatusInternalServerError, "Failed to read events: "+err.Error())
			return
		}
		writeJSON(w, http.StatusOK, events)
	}
}

// handlePath returns the current session's path data (anchors, trajectory, events).
// GET /api/path
func handlePath() http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		pathData, err := dashboard.LoadPathData()
		if err != nil {
			writeError(w, http.StatusInternalServerError, "Failed to read path data: "+err.Error())
			return
		}
		writeJSON(w, http.StatusOK, pathData)
	}
}

// handleExchanges returns recent exchanges with message text and valence.
// GET /api/exchanges?session_id=xxx&limit=50
func handleExchanges(svc *dashboard.DashboardService) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		sessionID := r.URL.Query().Get("session_id")
		limit := parseIntParam(r, "limit", 50)

		// If no session_id provided, use current session
		if sessionID == "" {
			snap, _ := svc.CurrentState()
			if snap != nil {
				sessionID = snap.SessionID
			}
		}

		exchanges, err := svc.RecentExchanges(sessionID, limit)
		if err != nil {
			writeError(w, http.StatusInternalServerError, "Failed to read exchanges: "+err.Error())
			return
		}
		writeJSON(w, http.StatusOK, exchanges)
	}
}

// handleLivePatterns returns real-time detected patterns from growth.db.
// GET /api/live-patterns
func handleLivePatterns(svc *dashboard.DashboardService) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		patterns, err := svc.LivePatterns()
		if err != nil {
			writeError(w, http.StatusInternalServerError, "Failed to read live patterns: "+err.Error())
			return
		}
		writeJSON(w, http.StatusOK, patterns)
	}
}

// handleValence returns valence distribution for a session.
// GET /api/valence?session_id=xxx
func handleValence(svc *dashboard.DashboardService) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		sessionID := r.URL.Query().Get("session_id")

		// If no session_id provided, use current session
		if sessionID == "" {
			snap, _ := svc.CurrentState()
			if snap != nil {
				sessionID = snap.SessionID
			}
		}

		if sessionID == "" {
			writeJSON(w, http.StatusOK, map[string]int{"positive": 0, "neutral": 0, "negative": 0})
			return
		}

		dist, err := svc.ValenceDistribution(sessionID)
		if err != nil {
			writeError(w, http.StatusInternalServerError, "Failed to read valence: "+err.Error())
			return
		}
		writeJSON(w, http.StatusOK, dist)
	}
}

// ============================================================================
// CLOSING
// ============================================================================
