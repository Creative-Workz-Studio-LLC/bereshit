// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-hostrouter
// Purpose: Route requests by hostname — serve different sites on subdomains
// Biblical: John 14:2 — "In my Father's house are many mansions"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

// Package server provides HTTP infrastructure for the CWS server: middleware
// chain, host-based routing, SPA file serving, and shared response helpers.
// It is used by all service packages but contains no business logic itself.
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
//
// Routing priority:
//  1. Exact hostname match (e.g., "creativeworkzstudio.com")
//  2. Catch-all "*" handler (for subdomains, localhost, direct IP)
//  3. 404 if no match and no catch-all
//
// Port numbers are stripped from the Host header before matching, so
// "localhost:3847" matches a route registered for "localhost".
type HostRouter struct {
	routes map[string]http.Handler // hostname → handler (use "*" for catch-all)
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
