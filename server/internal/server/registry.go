// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-registry
// Purpose: Service registry — track, health-check, and discover services
// Biblical: 1 Corinthians 12:18 — "God hath set the members in the body"
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
	"sync"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ServiceStatus represents the current operational state of a service.
type ServiceStatus string

const (
	StatusUp       ServiceStatus = "up"
	StatusDegraded ServiceStatus = "degraded"
	StatusDown     ServiceStatus = "down"
)

// ServiceInfo describes a registered service.
type ServiceInfo struct {
	Name        string        `json:"name"`
	Version     string        `json:"version"`
	Status      ServiceStatus `json:"status"`
	Routes      int           `json:"routes"`
	StartedAt   time.Time     `json:"startedAt"`
	Description string        `json:"description,omitempty"`
	Health      map[string]any `json:"health,omitempty"`
}

// HealthChecker is implemented by services that report their own health.
type HealthChecker interface {
	Health() map[string]any
}

// Registry tracks all active services in the server.
// Thread-safe for concurrent health checks and registration.
type Registry struct {
	mu       sync.RWMutex
	services map[string]*ServiceInfo
	checkers map[string]HealthChecker
}

// NewRegistry creates an empty service registry.
func NewRegistry() *Registry {
	return &Registry{
		services: make(map[string]*ServiceInfo),
		checkers: make(map[string]HealthChecker),
	}
}

// Register adds a service to the registry.
func (r *Registry) Register(info ServiceInfo, checker HealthChecker) {
	r.mu.Lock()
	defer r.mu.Unlock()
	info.StartedAt = time.Now()
	info.Status = StatusUp
	r.services[info.Name] = &info
	if checker != nil {
		r.checkers[info.Name] = checker
	}
}

// Services returns all registered services.
func (r *Registry) Services() []ServiceInfo {
	r.mu.RLock()
	defer r.mu.RUnlock()
	result := make([]ServiceInfo, 0, len(r.services))
	for _, svc := range r.services {
		result = append(result, *svc)
	}
	return result
}

// AggregateHealth runs health checks on all services and returns the result.
func (r *Registry) AggregateHealth() map[string]any {
	r.mu.RLock()
	defer r.mu.RUnlock()

	overall := StatusUp
	svcHealth := make(map[string]any, len(r.services))

	for name, info := range r.services {
		entry := map[string]any{
			"status":    info.Status,
			"startedAt": info.StartedAt.Format(time.RFC3339),
		}

		// Run health checker if available
		if checker, ok := r.checkers[name]; ok {
			health := checker.Health()
			entry["health"] = health
			if status, ok := health["status"]; ok {
				if s, ok := status.(string); ok && s == "degraded" {
					entry["status"] = StatusDegraded
					if overall == StatusUp {
						overall = StatusDegraded
					}
				}
			}
		}

		svcHealth[name] = entry
	}

	return map[string]any{
		"status":   overall,
		"services": svcHealth,
	}
}

// HandleHealth returns an HTTP handler that reports aggregated health.
func (r *Registry) HandleHealth() http.HandlerFunc {
	return func(w http.ResponseWriter, _ *http.Request) {
		health := r.AggregateHealth()
		status := http.StatusOK
		if health["status"] == StatusDegraded {
			status = http.StatusOK // Still 200 for degraded (service is running)
		}
		WriteJSON(w, status, health)
	}
}

// HandleServices returns an HTTP handler that lists all registered services.
func (r *Registry) HandleServices() http.HandlerFunc {
	return func(w http.ResponseWriter, _ *http.Request) {
		WriteJSON(w, http.StatusOK, r.Services())
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
