// ============================================================================
// METADATA - CPI-SI Dashboard WebSocket Hub
// ============================================================================
//
// Key: claude-global-dashboard-websocket
// Purpose: WebSocket hub for real-time state change broadcasting
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// Architecture:
//   Hub subscribes to DashboardService.Subscribe() for StateChange events.
//   When a StateChange arrives, it broadcasts the JSON payload to all
//   connected WebSocket clients. New clients receive an initial state
//   snapshot immediately upon connection.
//
// ============================================================================

package server

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"log"
	"net/http"
	"time"

	"github.com/gorilla/websocket"

	"cws.studio/pkg/dashboard"
)

// Constants for WebSocket management.
const (
	// Time allowed to write a message to the peer.
	writeWait = 10 * time.Second

	// Time allowed to read the next pong message from the peer.
	pongWait = 60 * time.Second

	// Send pings to peer at this interval. Must be less than pongWait.
	pingPeriod = (pongWait * 9) / 10

	// Maximum message size allowed from peer.
	maxMessageSize = 512
)

// upgrader configures the WebSocket upgrader with permissive origin checking
// for local development.
var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
	CheckOrigin: func(r *http.Request) bool {
		return true // Allow all origins for local development
	},
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// Hub — Central WebSocket connection manager
// ────────────────────────────────────────────────────────────────────────────

// Hub maintains the set of active WebSocket clients and broadcasts
// state changes to all of them.
type Hub struct {
	svc        *dashboard.DashboardService
	clients    map[*Client]bool
	broadcast  chan []byte
	register   chan *Client
	unregister chan *Client
	stop       chan struct{}
}

// NewHub creates a new WebSocket hub connected to the dashboard service.
func NewHub(svc *dashboard.DashboardService) *Hub {
	return &Hub{
		svc:        svc,
		clients:    make(map[*Client]bool),
		broadcast:  make(chan []byte, 256),
		register:   make(chan *Client),
		unregister: make(chan *Client),
		stop:       make(chan struct{}),
	}
}

// Run starts the hub's main loop.
// It manages client registration/unregistration, broadcasts state changes,
// and subscribes to the DashboardService event bus.
func (h *Hub) Run() {
	// Subscribe to state changes from the data layer
	eventCh := h.svc.Subscribe()
	defer h.svc.Unsubscribe(eventCh)

	for {
		select {
		case <-h.stop:
			// Shut down all clients
			for client := range h.clients {
				close(client.send)
				delete(h.clients, client)
			}
			return

		case client := <-h.register:
			h.clients[client] = true
			// Send initial state snapshot to new client
			h.sendInitialState(client)

		case client := <-h.unregister:
			if _, ok := h.clients[client]; ok {
				delete(h.clients, client)
				close(client.send)
			}

		case change := <-eventCh:
			// Marshal the state change to JSON
			data, err := json.Marshal(change)
			if err != nil {
				continue
			}
			// Broadcast to all clients
			for client := range h.clients {
				select {
				case client.send <- data:
				default:
					// Client buffer full — disconnect
					close(client.send)
					delete(h.clients, client)
				}
			}

		case msg := <-h.broadcast:
			for client := range h.clients {
				select {
				case client.send <- msg:
				default:
					close(client.send)
					delete(h.clients, client)
				}
			}
		}
	}
}

// Stop signals the hub to shut down.
func (h *Hub) Stop() {
	close(h.stop)
}

// sendInitialState sends the current state snapshot to a newly connected client.
func (h *Hub) sendInitialState(client *Client) {
	snap, err := h.svc.CurrentState()
	if err != nil {
		return
	}

	change := dashboard.StateChange{
		Type:      "state_update",
		Snapshot:  snap,
		Timestamp: snap.Timestamp,
	}

	data, err := json.Marshal(change)
	if err != nil {
		return
	}

	select {
	case client.send <- data:
	default:
		// Client buffer already full on connect — unusual but handle it
	}
}

// ────────────────────────────────────────────────────────────────────────────
// Client — Individual WebSocket connection
// ────────────────────────────────────────────────────────────────────────────

// Client represents a single WebSocket connection to the hub.
type Client struct {
	hub  *Hub
	conn *websocket.Conn
	send chan []byte
}

// readPump reads messages from the WebSocket connection.
// We don't expect meaningful client messages, but we need to read
// to detect disconnection and handle pong messages.
func (c *Client) readPump() {
	defer func() {
		c.hub.unregister <- c
		c.conn.Close()
	}()

	c.conn.SetReadLimit(maxMessageSize)
	c.conn.SetReadDeadline(time.Now().Add(pongWait))
	c.conn.SetPongHandler(func(string) error {
		c.conn.SetReadDeadline(time.Now().Add(pongWait))
		return nil
	})

	for {
		_, _, err := c.conn.ReadMessage()
		if err != nil {
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseNormalClosure) {
				log.Printf("WebSocket read error: %v", err)
			}
			break
		}
	}
}

// writePump sends messages from the hub to the WebSocket connection.
// It handles message batching and ping/pong keepalive.
func (c *Client) writePump() {
	ticker := time.NewTicker(pingPeriod)
	defer func() {
		ticker.Stop()
		c.conn.Close()
	}()

	for {
		select {
		case message, ok := <-c.send:
			c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if !ok {
				// Hub closed the channel
				c.conn.WriteMessage(websocket.CloseMessage, []byte{})
				return
			}

			w, err := c.conn.NextWriter(websocket.TextMessage)
			if err != nil {
				return
			}
			w.Write(message)

			// Drain queued messages into the current write
			n := len(c.send)
			for i := 0; i < n; i++ {
				w.Write([]byte("\n"))
				w.Write(<-c.send)
			}

			if err := w.Close(); err != nil {
				return
			}

		case <-ticker.C:
			c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if err := c.conn.WriteMessage(websocket.PingMessage, nil); err != nil {
				return
			}
		}
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "Write the vision, and make it plain upon tables,
//  that he may run that readeth it." — Habakkuk 2:2
