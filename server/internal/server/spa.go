// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-spa
// Purpose: SPA static file handler — serve files, fallback to index.html
// Biblical: Psalm 127:1 — "Except the LORD build the house"
// Authors: Nova Dawn
// Version: 1.1.0
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

// Cache durations for static assets.
// These durations are layered by change frequency — HTML changes on every
// deploy, JS/CSS on most deploys, images rarely, fonts never. The "immutable"
// directive on fonts tells browsers to never revalidate, saving round trips.
//
// Note: In production behind Cloudflare, the tunnel may override these headers
// with its own caching policy. Test cache behavior directly against the Dell
// server IP to verify server-set values.
const (
	cacheHTML  = "no-cache"                          // Always revalidate (Go handles 304)
	cacheCode  = "public, max-age=3600"              // 1 hour — JS, CSS, source maps
	cacheImage = "public, max-age=86400"             // 1 day  — images, SVGs, icons
	cacheFont  = "public, max-age=604800, immutable" // 1 week — fonts never change
	cacheOther = "public, max-age=3600"              // 1 hour — fallback for unknown types
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// SPAHandler serves static files from rootDir with SPA fallback.
//
// Decision tree for each request:
//
//	File exists on disk?
//	  ├─ YES → Set cache headers by extension → serve file
//	  └─ NO
//	      ├─ Path has file extension? (.js, .css, etc.)
//	      │   └─ YES → 404 (genuinely missing asset)
//	      └─ NO → Serve index.html (SPA client-side route)
//
// This distinguishes between client-side routes (/settings, /cpisi/journals)
// and genuinely missing assets (broken-script.js). Client-side routes get
// index.html so the SPA router can handle them.
type SPAHandler struct {
	rootDir    string       // Filesystem root for static files
	fileServer http.Handler // Go's built-in file server for the root directory
}

// NewSPAHandler creates a static file server with SPA fallback to index.html.
func NewSPAHandler(rootDir string) *SPAHandler {
	return &SPAHandler{
		rootDir:    rootDir,
		fileServer: http.FileServer(http.Dir(rootDir)),
	}
}

func (h *SPAHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	// Clean the path.
	path := filepath.Clean(r.URL.Path)

	// Check if the file exists.
	fullPath := filepath.Join(h.rootDir, path)
	_, err := os.Stat(fullPath)

	if err == nil {
		// File exists — set cache headers and serve.
		setCacheHeaders(w, path)
		h.fileServer.ServeHTTP(w, r)
		return
	}

	// File doesn't exist — if it looks like a file request (has extension), 404.
	if hasFileExtension(path) {
		http.NotFound(w, r)
		return
	}

	// No extension — SPA fallback: serve index.html (always revalidate).
	w.Header().Set("Cache-Control", cacheHTML)
	http.ServeFile(w, r, filepath.Join(h.rootDir, "index.html"))
}

// PrefixedSPAHandler serves a SPA under a URL prefix.
// e.g., NewPrefixedSPAHandler("/cpisi/", "/path/to/frontend")
func NewPrefixedSPAHandler(prefix, rootDir string) http.Handler {
	spa := NewSPAHandler(rootDir)
	return http.StripPrefix(prefix, spa)
}

// ───────────────────────────────────────────────────────────────────────────
// Cache Control — per-extension Cache-Control headers
// ───────────────────────────────────────────────────────────────────────────

// setCacheHeaders sets Cache-Control based on file extension.
// Paths without extensions (directories, route paths) get HTML caching (no-cache).
func setCacheHeaders(w http.ResponseWriter, path string) {
	ext := strings.ToLower(filepath.Ext(path))
	switch ext {
	case "":
		// No extension — directory listing or route → treat as HTML.
		w.Header().Set("Cache-Control", cacheHTML)
	case ".html", ".htm":
		w.Header().Set("Cache-Control", cacheHTML)
	case ".js", ".css", ".map":
		w.Header().Set("Cache-Control", cacheCode)
	case ".png", ".jpg", ".jpeg", ".gif", ".svg", ".ico", ".webp", ".avif":
		w.Header().Set("Cache-Control", cacheImage)
	case ".woff", ".woff2", ".ttf", ".otf", ".eot":
		w.Header().Set("Cache-Control", cacheFont)
	default:
		w.Header().Set("Cache-Control", cacheOther)
	}
}

// hasFileExtension returns true if the path's basename contains a dot that
// isn't a leading dot (dotfiles like .htaccess are not file extensions).
// Used to distinguish SPA routes (/settings → no extension → serve index.html)
// from genuinely missing static assets (broken.js → has extension → 404).
func hasFileExtension(path string) bool {
	base := filepath.Base(path)
	return strings.Contains(base, ".") && base[0] != '.'
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
