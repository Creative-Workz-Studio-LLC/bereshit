// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-prometheus
// Purpose: Prometheus metrics exposition — /metrics endpoint in text format
// Biblical: Proverbs 27:23 — "Be thou diligent to know the state of thy flocks"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// Bridges existing lock-free metrics (sync/atomic) to Prometheus text format.
// The JSON endpoint at /api/metrics is preserved for backward compatibility.
// This file adds /metrics (Prometheus text format) as a separate endpoint.
//
// Design: CounterFunc/GaugeFunc collectors read from the existing Metrics
// struct at scrape time — zero overhead on the hot path for existing counters.
// One hot-path addition: HistogramVec for request duration distribution,
// which prometheus.Histogram.Observe handles lock-free internally.
//
// ═══════════════════════════════════════════════════════════════════════════

package server

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"net/http"
	"runtime"
	"time"

	"github.com/prometheus/client_golang/prometheus"
	"github.com/prometheus/client_golang/prometheus/collectors"
	"github.com/prometheus/client_golang/prometheus/promhttp"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// PrometheusMetrics wraps the existing Metrics with Prometheus collectors.
//
// Two strategies:
//  1. GaugeFunc/CounterFunc — for values already tracked by sync/atomic
//     (totalRequests, totalErrors, activeRequests, uptime). These read
//     from Metrics at scrape time, adding zero overhead to the hot path.
//  2. Native Histogram — for request duration distribution. Observed
//     directly in the middleware because existing Metrics only stores
//     sum, not distribution. prometheus.Histogram.Observe is lock-free.
type PrometheusMetrics struct {
	registry    *prometheus.Registry
	httpHandler http.Handler

	// Native histogram for request duration (observed on hot path)
	requestDuration *prometheus.HistogramVec
}

// NewPrometheusMetrics creates Prometheus collectors backed by existing Metrics.
// The wsClientCount function provides WebSocket client count without importing
// the ws package (keeps dependency direction clean).
func NewPrometheusMetrics(m *Metrics, wsClientCount func() int) *PrometheusMetrics {
	reg := prometheus.NewRegistry()

	// Standard Go runtime and process collectors
	reg.MustRegister(collectors.NewGoCollector())
	reg.MustRegister(collectors.NewProcessCollector(collectors.ProcessCollectorOpts{}))

	pm := &PrometheusMetrics{
		registry: reg,
	}

	// ── Counters (read from sync/atomic at scrape time) ──────────────

	reg.MustRegister(prometheus.NewCounterFunc(
		prometheus.CounterOpts{
			Namespace: "cws",
			Subsystem: "http",
			Name:      "requests_total",
			Help:      "Total HTTP requests received.",
		},
		func() float64 { return float64(m.totalRequests.Load()) },
	))

	reg.MustRegister(prometheus.NewCounterFunc(
		prometheus.CounterOpts{
			Namespace: "cws",
			Subsystem: "http",
			Name:      "errors_total",
			Help:      "Total HTTP error responses (4xx + 5xx).",
		},
		func() float64 { return float64(m.totalErrors.Load()) },
	))

	// ── Gauges ───────────────────────────────────────────────────────

	reg.MustRegister(prometheus.NewGaugeFunc(
		prometheus.GaugeOpts{
			Namespace: "cws",
			Subsystem: "http",
			Name:      "active_requests",
			Help:      "Number of HTTP requests currently being processed.",
		},
		func() float64 { return float64(m.activeRequests.Load()) },
	))

	reg.MustRegister(prometheus.NewGaugeFunc(
		prometheus.GaugeOpts{
			Namespace: "cws",
			Subsystem: "server",
			Name:      "uptime_seconds",
			Help:      "Seconds since server started.",
		},
		func() float64 { return time.Since(m.startTime).Seconds() },
	))

	reg.MustRegister(prometheus.NewGaugeFunc(
		prometheus.GaugeOpts{
			Namespace: "cws",
			Subsystem: "server",
			Name:      "goroutines",
			Help:      "Number of active goroutines.",
		},
		func() float64 { return float64(runtime.NumGoroutine()) },
	))

	if wsClientCount != nil {
		reg.MustRegister(prometheus.NewGaugeFunc(
			prometheus.GaugeOpts{
				Namespace: "cws",
				Subsystem: "ws",
				Name:      "clients",
				Help:      "Number of active WebSocket clients.",
			},
			func() float64 { return float64(wsClientCount()) },
		))
	}

	// ── Histogram (the one hot-path addition) ────────────────────────

	pm.requestDuration = prometheus.NewHistogramVec(
		prometheus.HistogramOpts{
			Namespace: "cws",
			Subsystem: "http",
			Name:      "request_duration_seconds",
			Help:      "HTTP request duration in seconds.",
			Buckets:   []float64{0.001, 0.005, 0.01, 0.025, 0.05, 0.1, 0.25, 0.5, 1, 2.5, 5},
		},
		[]string{"method", "route", "status_code"},
	)
	reg.MustRegister(pm.requestDuration)

	pm.httpHandler = promhttp.HandlerFor(reg, promhttp.HandlerOpts{
		EnableOpenMetrics: true,
	})

	return pm
}

// Handler returns the HTTP handler for /metrics (Prometheus text format).
func (pm *PrometheusMetrics) Handler() http.Handler {
	return pm.httpHandler
}

// ObserveRequest records a request duration in the histogram.
// Called from the metrics middleware after the request completes.
func (pm *PrometheusMetrics) ObserveRequest(method, route string, statusCode int, duration time.Duration) {
	pm.requestDuration.WithLabelValues(
		method,
		route,
		fmt.Sprintf("%d", statusCode),
	).Observe(duration.Seconds())
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
