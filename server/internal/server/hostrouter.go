// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-hostrouter
// Purpose: Route requests by hostname — serve different sites on subdomains
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
	"strings"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// HostRouter routes requests to different handlers based on the Host header.
// This enables serving the company website on the root domain and the
// builder dashboard on a subdomain from a single server.
type HostRouter struct {
	// routes maps hostname patterns to handlers.
	// Use "*" as a catch-all default.
	routes map[string]http.Handler
}

// NewHostRouter creates a new host-based router.
func NewHostRouter() *HostRouter {
	return &HostRouter{
		routes: make(map[string]http.Handler),
	}
}

// Handle registers a handler for the given hostname.
// Use "*" as a catch-all for unmatched hosts.
func (hr *HostRouter) Handle(host string, handler http.Handler) {
	hr.routes[host] = handler
}

// ServeHTTP routes the request based on the Host header.
func (hr *HostRouter) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	// Strip port from host
	host := r.Host
	if i := strings.LastIndex(host, ":"); i != -1 {
		host = host[:i]
	}

	// Try exact match first
	if handler, ok := hr.routes[host]; ok {
		handler.ServeHTTP(w, r)
		return
	}

	// Try catch-all
	if handler, ok := hr.routes["*"]; ok {
		handler.ServeHTTP(w, r)
		return
	}

	http.NotFound(w, r)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
