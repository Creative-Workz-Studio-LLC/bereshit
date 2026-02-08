// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-middleware
// Purpose: HTTP middleware — recovery, logging, request-ID, CORS, security
// Biblical: Nehemiah 4:9 — "We made our prayer and set a watch"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// Middleware ordering contract (outermost → innermost):
//   Recovery → RequestLogger → RequestID → CORS → SecurityHeaders → Handler
//
// Recovery wraps everything so panics in any layer are caught.
// RequestLogger wraps the response writer to capture status codes.
// RequestID generates the ID before the handler runs so logs include it.
// CORS handles preflight OPTIONS before security headers add restrictions.
// SecurityHeaders runs last so headers are set closest to the response.
//
// ═══════════════════════════════════════════════════════════════════════════

package server

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"crypto/rand"
	"encoding/hex"
	"fmt"
	"log/slog"
	"net/http"
	"runtime/debug"
	"strings"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Types — response wrappers for middleware instrumentation
// ───────────────────────────────────────────────────────────────────────────

// statusWriter wraps ResponseWriter to capture the HTTP status code.
// Without this, the logger middleware can't know what status the handler sent
// because ResponseWriter.WriteHeader doesn't expose the written code.
type statusWriter struct {
	http.ResponseWriter
	status int
}

func (w *statusWriter) WriteHeader(code int) {
	w.status = code
	w.ResponseWriter.WriteHeader(code)
}

// ───────────────────────────────────────────────────────────────────────────
// Middleware — each function wraps an http.Handler with cross-cutting concern
// ───────────────────────────────────────────────────────────────────────────

// Recovery catches panics, logs the stack trace, and returns 500.
// This is the outermost middleware — it must wrap everything so that panics
// in any handler or middleware are recovered instead of crashing the server.
func Recovery(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		defer func() {
			if err := recover(); err != nil {
				slog.Error("panic recovered",
					"error", fmt.Sprint(err),
					"stack", string(debug.Stack()),
					"method", r.Method,
					"path", r.URL.Path,
				)
				http.Error(w, `{"error":"internal server error"}`, http.StatusInternalServerError)
			}
		}()
		next.ServeHTTP(w, r)
	})
}

// RequestLogger logs each request with method, path, status, and duration.
func RequestLogger(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		start := time.Now()
		sw := &statusWriter{ResponseWriter: w, status: http.StatusOK}

		next.ServeHTTP(sw, r)

		duration := time.Since(start)

		// Skip logging for static assets to reduce noise
		if isStaticAsset(r.URL.Path) {
			return
		}

		slog.Info("request",
			"method", r.Method,
			"path", r.URL.Path,
			"status", sw.status,
			"duration", duration.String(),
			"requestId", w.Header().Get("X-Request-ID"),
		)
	})
}

// RequestID adds a unique ID to each request for tracing.
func RequestID(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		id := generateRequestID()
		w.Header().Set("X-Request-ID", id)
		next.ServeHTTP(w, r)
	})
}

// CORS adds permissive CORS headers for development and Cloudflare Tunnel
// proxying. Origin is set to "*" because:
//   - In development, the frontend runs on a different port (5173 vs 3847)
//   - In production, Cloudflare Tunnel proxies requests, changing the origin
//   - The API is not sensitive to CSRF (no cookie auth, no session state)
//
// If cookie-based auth is added, this must be tightened to specific origins.
func CORS(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Request-ID")

		if r.Method == http.MethodOptions {
			w.WriteHeader(http.StatusNoContent)
			return
		}

		next.ServeHTTP(w, r)
	})
}

// SecurityHeaders adds standard security headers to all responses.
func SecurityHeaders(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("X-Content-Type-Options", "nosniff")
		w.Header().Set("X-Frame-Options", "DENY")
		w.Header().Set("Referrer-Policy", "strict-origin-when-cross-origin")

		// API responses must never be cached by browsers/proxies.
		if strings.HasPrefix(r.URL.Path, "/api/") || r.URL.Path == "/healthz" {
			w.Header().Set("Cache-Control", "no-store")
		}

		next.ServeHTTP(w, r)
	})
}

// ───────────────────────────────────────────────────────────────────────────
// Composition — build the middleware stack
// ───────────────────────────────────────────────────────────────────────────

// Chain composes middleware in order: first middleware wraps outermost.
// The slice is applied in reverse so that Chain(h, A, B, C) produces
// A(B(C(h))) — A runs first on request, C runs first on response.
func Chain(handler http.Handler, middlewares ...func(http.Handler) http.Handler) http.Handler {
	for i := len(middlewares) - 1; i >= 0; i-- {
		handler = middlewares[i](handler)
	}
	return handler
}

// ───────────────────────────────────────────────────────────────────────────
// Helpers — internal utilities for middleware
// ───────────────────────────────────────────────────────────────────────────

// generateRequestID creates a cryptographically random 16-character hex string.
// Uses crypto/rand (not math/rand) for unpredictable IDs suitable for
// distributed tracing without coordination between server instances.
func generateRequestID() string {
	b := make([]byte, 8)
	rand.Read(b)
	return hex.EncodeToString(b)
}

// isStaticAsset returns true for file extensions that should be excluded from
// request logging. Without this filter, every CSS/JS/image request floods the
// log and makes it impossible to trace actual API calls.
func isStaticAsset(path string) bool {
	for _, ext := range []string{".js", ".css", ".png", ".svg", ".ico", ".map", ".woff", ".woff2"} {
		if len(path) > len(ext) && path[len(path)-len(ext):] == ext {
			return true
		}
	}
	return false
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
