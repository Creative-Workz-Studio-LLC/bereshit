// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-cpisi-bridge
// Purpose: Bridge between CWS server and pkg/dashboard.DashboardService
// Biblical: 1 Corinthians 12:12 — "The body is one, and hath many members"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

// Package cpisi provides the CPI-SI HTTP service for the CWS server.
// It bridges the claude-global DashboardService (which owns the CPI-SI state
// machine, event bus, and data access) to HTTP/WebSocket endpoints.
//
// The package is organized into three layers:
//
//   - Bridge: Owns the DashboardService lifecycle and WebSocket event streaming
//   - Handlers: Thin HTTP transport that delegates to Bridge/DashboardService
//   - JournalStore: SQLite persistence for journal CRUD and FTS5 search
//
// The Bridge pattern keeps the HTTP layer decoupled from the state machine
// implementation. If the DashboardService moves or changes, only the bridge
// needs updating — handlers remain stable.
package cpisi

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"encoding/json"
	"log/slog"

	"cws.studio/server/internal/ws"

	"cws.studio/pkg/dashboard"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Bridge — connects DashboardService to WebSocket event streaming
// ───────────────────────────────────────────────────────────────────────────

// Bridge wraps the DashboardService and pipes events to the WebSocket hub.
// It owns the lifecycle of both the DashboardService and the JournalStore,
// cleaning up both on Close.
type Bridge struct {
	svc      *dashboard.DashboardService // The CPI-SI state machine (from claude-global)
	hub      *ws.Hub                     // WebSocket hub for broadcasting state changes
	ctx      context.Context             // Parent context — cancellation stops event streaming
	journals *JournalStore               // SQLite journal persistence (nil if data-dir not set)
}

// NewBridge creates a CPI-SI bridge. Opens the DashboardService and starts
// streaming state changes to the WebSocket hub.
//
// The journal store is optional — if dataDir is empty or initialization fails,
// the bridge still functions (state machine queries work, journal endpoints
// are simply not registered). This graceful degradation means CPI-SI works
// even without persistent storage configured.
func NewBridge(ctx context.Context, hub *ws.Hub, dataDir string) (*Bridge, error) {
	svc, err := dashboard.New(ctx)
	if err != nil {
		return nil, err
	}

	b := &Bridge{
		svc: svc,
		hub: hub,
		ctx: ctx,
	}

	// Initialize journal store.
	if dataDir != "" {
		journals, err := NewJournalStore(dataDir)
		if err != nil {
			slog.Warn("journal store disabled", "error", err)
		} else {
			b.journals = journals
		}
	}

	// Start streaming state changes to WebSocket
	go b.streamEvents()

	slog.Info("cpisi bridge initialized")
	return b, nil
}

// Journals returns the journal store (may be nil if not initialized).
func (b *Bridge) Journals() *JournalStore {
	return b.journals
}

// Service returns the underlying DashboardService for handler queries.
func (b *Bridge) Service() *dashboard.DashboardService {
	return b.svc
}

// Close shuts down the DashboardService and journal store.
func (b *Bridge) Close() error {
	if b.journals != nil {
		b.journals.Close()
	}
	return b.svc.Close()
}

// ───────────────────────────────────────────────────────────────────────────
// Event Streaming — forward state machine changes to WebSocket clients
// ───────────────────────────────────────────────────────────────────────────

// streamEvents subscribes to the DashboardService's EventBus and forwards
// state changes to the WebSocket hub. Runs as a long-lived goroutine for
// the lifetime of the bridge.
//
// Lifecycle:
//  1. Subscribe to DashboardService event channel
//  2. Loop: receive change → JSON marshal → broadcast to all WS clients
//  3. Exit when parent context is cancelled (server shutdown) or channel closes
//
// Marshal errors are logged but don't stop the stream — one bad event
// shouldn't break the entire real-time feed.
func (b *Bridge) streamEvents() {
	ch := b.svc.Subscribe()
	defer b.svc.Unsubscribe(ch)

	for {
		select {
		case <-b.ctx.Done():
			return
		case change, ok := <-ch:
			if !ok {
				return
			}

			// Marshal and broadcast the state change
			data, err := json.Marshal(change)
			if err != nil {
				slog.Error("cpisi event marshal failed", "error", err)
				continue
			}
			b.hub.BroadcastRaw(data)
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
