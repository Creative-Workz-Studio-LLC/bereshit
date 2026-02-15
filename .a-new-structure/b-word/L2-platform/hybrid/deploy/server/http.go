//omni:code --go -library
//omni:key B-L2-server-http
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-server-http
// Purpose: HTTP helpers for L2 server handlers — JSON response, error writing
// Biblical: Proverbs 15:23 — "A word spoken in due season, how good is it!"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-15
//
// Extracted from cws.studio/server/internal/server to remove cross-module
// dependency. These are minimal helpers — the production server may provide
// richer versions.
//
// ═══════════════════════════════════════════════════════════════════════════

package server

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"

	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/deploy/builder"
)

// Service holds server runtime state shared across handlers.
type Service struct {
	paths builder.Paths
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// writeJSON serializes v as JSON and writes it to the response.
func writeJSON(w http.ResponseWriter, status int, v any) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	if err := json.NewEncoder(w).Encode(v); err != nil {
		// Best effort — headers already sent
		fmt.Fprintf(w, `{"error":"encode failed: %s"}`, err)
	}
}

// writeError writes a JSON error response.
func writeError(w http.ResponseWriter, status int, msg string) {
	writeJSON(w, status, map[string]string{"error": msg})
}

// decodeJSON reads the request body and unmarshals it into v.
func decodeJSON(r *http.Request, v any) error {
	body, err := io.ReadAll(r.Body)
	if err != nil {
		return fmt.Errorf("read body: %w", err)
	}
	defer r.Body.Close()
	return json.Unmarshal(body, v)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
