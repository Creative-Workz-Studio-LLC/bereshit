// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-metrics
// Purpose: Request metrics — counters, latency tracking, /metrics endpoint
// Biblical: Proverbs 27:23 — "Be thou diligent to know the state of thy flocks"
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
	"sort"
	"sync"
	"sync/atomic"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Metrics tracks request-level counters and latency for the server.
// All counters use atomic operations for lock-free updates on the hot path.
// Route-level breakdown uses a mutex-protected map (cold path, /metrics only).
type Metrics struct {
	totalRequests   atomic.Int64
	totalErrors     atomic.Int64 // 4xx + 5xx responses
	activeRequests  atomic.Int64
	totalLatencyMs  atomic.Int64 // Sum of all request latencies in ms
	startTime       time.Time

	mu          sync.Mutex
	routeCounts map[string]*atomic.Int64 // path → request count

	promBridge *PrometheusMetrics // nil if Prometheus not enabled
}

// SetPrometheusBridge attaches a Prometheus metrics bridge.
// When set, the middleware will also observe request durations in the
// Prometheus histogram (one nil check + histogram observe on hot path).
func (m *Metrics) SetPrometheusBridge(pm *PrometheusMetrics) {
	m.promBridge = pm
}

// StartedAt returns when the metrics tracker was created.
func (m *Metrics) StartedAt() time.Time {
	return m.startTime
}

// NewMetrics creates a new metrics tracker.
func NewMetrics() *Metrics {
	return &Metrics{
		startTime:   time.Now(),
		routeCounts: make(map[string]*atomic.Int64),
	}
}

// Middleware returns an HTTP middleware that tracks request metrics.
func (m *Metrics) Middleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		m.totalRequests.Add(1)
		m.activeRequests.Add(1)
		start := time.Now()

		// Track route
		m.trackRoute(r.URL.Path)

		sw := &statusWriter{ResponseWriter: w, status: http.StatusOK}
		next.ServeHTTP(sw, r)

		duration := time.Since(start)
		m.activeRequests.Add(-1)
		m.totalLatencyMs.Add(duration.Milliseconds())

		if sw.status >= 400 {
			m.totalErrors.Add(1)
		}

		// Prometheus histogram observation (one nil check on hot path)
		if m.promBridge != nil {
			route := normalizeRoutePath(r.URL.Path)
			m.promBridge.ObserveRequest(r.Method, route, sw.status, duration)
		}
	})
}

// trackRoute increments the counter for a specific route path.
// Groups paths by their first two segments to avoid unbounded cardinality
// from path parameters (e.g., /api/cpisi/sessions/abc123 → /api/cpisi/sessions).
func (m *Metrics) trackRoute(path string) {
	key := normalizeRoutePath(path)

	m.mu.Lock()
	counter, ok := m.routeCounts[key]
	if !ok {
		counter = &atomic.Int64{}
		m.routeCounts[key] = counter
	}
	m.mu.Unlock()

	counter.Add(1)
}

// Snapshot returns a point-in-time snapshot of all metrics.
func (m *Metrics) Snapshot() map[string]any {
	uptime := time.Since(m.startTime)
	total := m.totalRequests.Load()
	errors := m.totalErrors.Load()
	latencyMs := m.totalLatencyMs.Load()

	var avgLatencyMs float64
	if total > 0 {
		avgLatencyMs = float64(latencyMs) / float64(total)
	}

	var errorRate float64
	if total > 0 {
		errorRate = float64(errors) / float64(total)
	}

	// Build sorted route breakdown
	m.mu.Lock()
	routes := make(map[string]int64, len(m.routeCounts))
	for path, counter := range m.routeCounts {
		routes[path] = counter.Load()
	}
	m.mu.Unlock()

	return map[string]any{
		"uptime":         uptime.Round(time.Second).String(),
		"uptimeSeconds":  int(uptime.Seconds()),
		"totalRequests":  total,
		"totalErrors":    errors,
		"activeRequests": m.activeRequests.Load(),
		"errorRate":      errorRate,
		"avgLatencyMs":   avgLatencyMs,
		"routes":         routes,
	}
}

// HandleMetrics returns an HTTP handler for /metrics.
func (m *Metrics) HandleMetrics() http.HandlerFunc {
	return func(w http.ResponseWriter, _ *http.Request) {
		WriteJSON(w, http.StatusOK, m.Snapshot())
	}
}

// normalizeRoutePath groups paths by their first 3 segments to prevent
// unbounded cardinality from path parameters.
// "/api/cpisi/sessions/abc/kalign" → "/api/cpisi/sessions"
// "/api/cpisi/state" → "/api/cpisi/state"
// "/ws" → "/ws"
func normalizeRoutePath(path string) string {
	segments := splitPath(path)
	if len(segments) > 3 {
		segments = segments[:3]
	}
	result := "/"
	for i, s := range segments {
		if i > 0 {
			result += "/"
		}
		result += s
	}
	return result
}

// splitPath splits a URL path into segments, ignoring empty segments.
func splitPath(path string) []string {
	var segments []string
	start := 0
	for i := 0; i < len(path); i++ {
		if path[i] == '/' {
			if start < i {
				segments = append(segments, path[start:i])
			}
			start = i + 1
		}
	}
	if start < len(path) {
		segments = append(segments, path[start:])
	}
	return segments
}

// TopRoutes returns the N most requested routes. Useful for /metrics summary.
func (m *Metrics) TopRoutes(n int) []struct {
	Path  string
	Count int64
} {
	m.mu.Lock()
	type entry struct {
		path  string
		count int64
	}
	entries := make([]entry, 0, len(m.routeCounts))
	for path, counter := range m.routeCounts {
		entries = append(entries, entry{path, counter.Load()})
	}
	m.mu.Unlock()

	sort.Slice(entries, func(i, j int) bool {
		return entries[i].count > entries[j].count
	})

	if len(entries) > n {
		entries = entries[:n]
	}

	result := make([]struct {
		Path  string
		Count int64
	}, len(entries))
	for i, e := range entries {
		result[i].Path = e.path
		result[i].Count = e.count
	}
	return result
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
