// ============================================================================
// METADATA - CPI-SI Model Router
// ============================================================================
//
// Key: pkg-agent-router
// Purpose: Model routing — the agent DECIDES which provider to use for each
//          request. Not all questions need the most expensive model.
//          The router embodies CPI-SI's "agent owns the loop" principle.
// Biblical: Proverbs 16:9 — "A man's heart deviseth his way: but the LORD
//           directeth his steps"
// Authors: Seanje Lenox-Wise (Lead), Nova Dawn (CPI-SI Instance)
// Version: 0.1.0
// Created: 2026-02-24
//
// Architecture:
//   The Router is a Provider that delegates to other providers based on
//   routing rules. The agent asks the router; the router picks the best
//   provider for the job.
//
//   Simple requests → fast/cheap model
//   Complex requests → powerful model
//   Tool-heavy → model with good tool use
//
// ============================================================================

package agent

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"strings"
)

// ============================================================================
// BODY
// ============================================================================

// Route represents a routing rule — a condition and a target provider.
type Route struct {
	// Name identifies this route (for logging/debugging).
	Name string

	// Match determines whether this route applies to the request.
	// If nil, the route always matches (use for fallback).
	Match func(req ChatRequest) bool

	// Provider is the target provider for matching requests.
	Provider Provider
}

// Router implements Provider by selecting from multiple providers
// based on routing rules. The first matching route wins.
type Router struct {
	routes   []Route
	fallback Provider
	lastUsed string // track which route was used last
}

// NewRouter creates a model router with the given routes and fallback.
// Routes are evaluated in order; first match wins.
// The fallback is used when no route matches.
func NewRouter(routes []Route, fallback Provider) *Router {
	return &Router{
		routes:   routes,
		fallback: fallback,
	}
}

// Chat routes the request to the appropriate provider.
func (r *Router) Chat(ctx context.Context, req ChatRequest) (*ChatResponse, error) {
	provider := r.selectProvider(req)
	return provider.Chat(ctx, req)
}

// Name returns the router identifier.
func (r *Router) Name() string { return "router" }

// Model returns the model of the last-used provider.
func (r *Router) Model() string {
	if r.lastUsed != "" {
		return r.lastUsed
	}
	return r.fallback.Model()
}

// LastRoute returns the name of the last route used.
func (r *Router) LastRoute() string { return r.lastUsed }

// selectProvider finds the first matching route's provider.
func (r *Router) selectProvider(req ChatRequest) Provider {
	for _, route := range r.routes {
		if route.Match == nil || route.Match(req) {
			r.lastUsed = route.Name + ":" + route.Provider.Model()
			return route.Provider
		}
	}
	r.lastUsed = "fallback:" + r.fallback.Model()
	return r.fallback
}

// --- Built-in Route Matchers ---

// MatchToolCalls matches requests that include tool definitions.
// Tool-heavy requests benefit from models with strong tool-use capabilities.
func MatchToolCalls(req ChatRequest) bool {
	return len(req.Tools) > 0
}

// MatchLongContext matches requests where the message history exceeds
// a threshold length. Long contexts may need models with larger windows.
func MatchLongContext(threshold int) func(ChatRequest) bool {
	return func(req ChatRequest) bool {
		total := 0
		for _, m := range req.Messages {
			total += len(m.Content)
		}
		return total > threshold
	}
}

// MatchShortInput matches requests where the latest user message is short.
// Simple questions can use faster/cheaper models.
func MatchShortInput(maxLen int) func(ChatRequest) bool {
	return func(req ChatRequest) bool {
		if len(req.Messages) == 0 {
			return false
		}
		last := req.Messages[len(req.Messages)-1]
		return last.Role == RoleUser && len(last.Content) <= maxLen
	}
}

// MatchKeywords matches requests where the latest user message contains
// any of the given keywords (case-insensitive).
func MatchKeywords(keywords ...string) func(ChatRequest) bool {
	return func(req ChatRequest) bool {
		if len(req.Messages) == 0 {
			return false
		}
		last := req.Messages[len(req.Messages)-1]
		if last.Role != RoleUser {
			return false
		}
		lower := strings.ToLower(last.Content)
		for _, kw := range keywords {
			if strings.Contains(lower, strings.ToLower(kw)) {
				return true
			}
		}
		return false
	}
}

// MatchNoTools matches requests that have no tool definitions.
// Pure conversational requests can use simpler models.
func MatchNoTools(req ChatRequest) bool {
	return len(req.Tools) == 0
}

// ============================================================================
// CLOSING
// ============================================================================
// "A man's heart deviseth his way: but the LORD directeth his steps."
// — Proverbs 16:9
// The agent plans. The router directs. The provider executes.
