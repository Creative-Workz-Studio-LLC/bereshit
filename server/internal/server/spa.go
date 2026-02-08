// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-spa
// Purpose: SPA static file handler — serve files, fallback to index.html
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package server

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// SPAHandler serves static files from rootDir with SPA fallback.
// If a requested file doesn't exist and the path has no file extension,
// it serves index.html (for client-side routing).
type SPAHandler struct {
	rootDir    string
	fileServer http.Handler
}

// NewSPAHandler creates a static file server with SPA fallback to index.html.
func NewSPAHandler(rootDir string) *SPAHandler {
	return &SPAHandler{
		rootDir:    rootDir,
		fileServer: http.FileServer(http.Dir(rootDir)),
	}
}

func (h *SPAHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	// Clean the path
	path := filepath.Clean(r.URL.Path)

	// Check if the file exists
	fullPath := filepath.Join(h.rootDir, path)
	_, err := os.Stat(fullPath)

	if err == nil {
		// File exists — serve it directly
		h.fileServer.ServeHTTP(w, r)
		return
	}

	// File doesn't exist — if it looks like a file request (has extension), 404
	if hasFileExtension(path) {
		http.NotFound(w, r)
		return
	}

	// No extension — SPA fallback: serve index.html
	http.ServeFile(w, r, filepath.Join(h.rootDir, "index.html"))
}

// PrefixedSPAHandler serves a SPA under a URL prefix.
// e.g., NewPrefixedSPAHandler("/cpisi/", "/path/to/frontend")
func NewPrefixedSPAHandler(prefix, rootDir string) http.Handler {
	spa := NewSPAHandler(rootDir)
	return http.StripPrefix(prefix, spa)
}

func hasFileExtension(path string) bool {
	base := filepath.Base(path)
	return strings.Contains(base, ".") && base[0] != '.'
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
