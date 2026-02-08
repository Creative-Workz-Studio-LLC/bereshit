// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-ws-hub
// Purpose: WebSocket connection hub — register, unregister, broadcast
// Biblical: Acts 2:1 — "They were all with one accord in one place"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

// Package ws implements a fan-in/fan-out WebSocket hub for real-time
// communication between the CWS server and browser clients.
//
// Architecture: The Hub is a single goroutine that manages all client
// registrations and message broadcasts via channels. This avoids mutex
// contention on the hot path (broadcasting) while using RWMutex only
// for the client map during concurrent broadcast delivery.
//
// Two services produce messages for the hub:
//   - Builder ProcessManager: stdout/stderr lines and exit status
//   - CPI-SI Bridge: state machine changes from DashboardService
//
// The hub is agnostic to message content — it broadcasts raw []byte
// frames and lets clients parse the JSON type field to route messages.
package ws

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"encoding/json"
	"log/slog"
	"net/http"
	"sync"

	"nhooyr.io/websocket"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Hub manages WebSocket connections and broadcasts messages to all clients.
// It follows the classic Go concurrency pattern of a single coordinating
// goroutine (Run) communicating via channels.
type Hub struct {
	clients    map[*Client]bool      // Active client connections (presence set)
	broadcast  chan []byte            // Incoming messages to fan out (buffered: 256)
	register   chan *Client           // New client connections
	unregister chan *Client           // Client disconnections
	mu         sync.RWMutex          // Protects clients map during concurrent broadcast
	ctx        context.Context       // Parent context for shutdown propagation
	cancel     context.CancelFunc    // Cancels ctx to trigger graceful shutdown
}

// NewHub creates a new WebSocket hub.
// The broadcast channel is buffered to 256 messages to absorb bursts from
// process output without blocking the sender. If the channel fills (e.g.,
// many processes streaming simultaneously), messages are dropped with a warning
// rather than blocking the producing goroutine.
func NewHub(ctx context.Context) *Hub {
	ctx, cancel := context.WithCancel(ctx)
	return &Hub{
		clients:    make(map[*Client]bool),
		broadcast:  make(chan []byte, 256),
		register:   make(chan *Client),
		unregister: make(chan *Client),
		ctx:        ctx,
		cancel:     cancel,
	}
}

// Run starts the hub's main loop. Should be called in a goroutine.
func (h *Hub) Run() {
	for {
		select {
		case <-h.ctx.Done():
			h.mu.Lock()
			for client := range h.clients {
				close(client.send)
				delete(h.clients, client)
			}
			h.mu.Unlock()
			return

		case client := <-h.register:
			h.mu.Lock()
			h.clients[client] = true
			h.mu.Unlock()
			slog.Info("websocket client connected", "clients", h.ClientCount())

		case client := <-h.unregister:
			h.mu.Lock()
			if _, ok := h.clients[client]; ok {
				close(client.send)
				delete(h.clients, client)
			}
			h.mu.Unlock()
			slog.Info("websocket client disconnected", "clients", h.ClientCount())

		case msg := <-h.broadcast:
			h.mu.RLock()
			for client := range h.clients {
				select {
				case client.send <- msg:
				default:
					// Slow client — its send buffer (256) is full. Rather than
					// blocking the broadcast for all clients, disconnect the slow
					// one. The frontend will auto-reconnect and catch up.
					close(client.send)
					delete(h.clients, client)
				}
			}
			h.mu.RUnlock()
		}
	}
}

// Broadcast sends a JSON-marshalled message to all connected clients.
func (h *Hub) Broadcast(v any) {
	data, err := json.Marshal(v)
	if err != nil {
		slog.Error("ws broadcast marshal failed", "error", err)
		return
	}
	select {
	case h.broadcast <- data:
	default:
		slog.Warn("ws broadcast channel full, dropping message")
	}
}

// BroadcastRaw sends pre-marshalled bytes to all connected clients.
func (h *Hub) BroadcastRaw(data []byte) {
	select {
	case h.broadcast <- data:
	default:
		slog.Warn("ws broadcast channel full, dropping message")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// HTTP Upgrade — WebSocket connection establishment
// ───────────────────────────────────────────────────────────────────────────

// HandleUpgrade is the HTTP handler for WebSocket upgrade requests.
// InsecureSkipVerify is set because CORS origin checking is handled by the
// CORS middleware and Cloudflare Tunnel — the WebSocket library doesn't need
// to duplicate that check.
func (h *Hub) HandleUpgrade(w http.ResponseWriter, r *http.Request) {
	conn, err := websocket.Accept(w, r, &websocket.AcceptOptions{
		InsecureSkipVerify: true,
	})
	if err != nil {
		slog.Error("ws upgrade failed", "error", err)
		return
	}

	client := &Client{
		hub:  h,
		conn: conn,
		send: make(chan []byte, 256),
	}

	h.register <- client

	// Start read/write pumps
	go client.writePump(h.ctx)
	go client.readPump(h.ctx)
}

// ClientCount returns the number of connected clients.
func (h *Hub) ClientCount() int {
	h.mu.RLock()
	defer h.mu.RUnlock()
	return len(h.clients)
}

// Close shuts down the hub and all client connections.
func (h *Hub) Close() {
	h.cancel()
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
