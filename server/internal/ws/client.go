// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-ws-client
// Purpose: Individual WebSocket connection — read and write pumps
// Biblical: Ecclesiastes 4:12 — "A threefold cord is not quickly broken"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package ws

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"log/slog"
	"time"

	"nhooyr.io/websocket"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Constants — timeout tuning for WebSocket connections
// ───────────────────────────────────────────────────────────────────────────

const (
	// writeTimeout is the maximum time to write a single message to the
	// WebSocket connection. 10 seconds is generous — if a write takes this
	// long, the client's network is likely unusable and we should disconnect
	// rather than hold resources indefinitely.
	writeTimeout = 10 * time.Second

	// pongTimeout is reserved for future ping/pong keep-alive implementation.
	// nhooyr.io/websocket handles ping/pong at the library level, so this
	// isn't actively used yet but documents the intended keep-alive window.
	pongTimeout = 60 * time.Second
)

// ───────────────────────────────────────────────────────────────────────────
// Client — a single WebSocket connection with read/write pump goroutines
// ───────────────────────────────────────────────────────────────────────────

// Client represents a single WebSocket connection.
// Each client spawns two goroutines: writePump (hub → browser) and
// readPump (browser → hub). The send channel is buffered to 256 messages
// to absorb output bursts without blocking the hub's broadcast loop.
type Client struct {
	hub  *Hub            // Back-reference to the hub for unregister on disconnect
	conn *websocket.Conn // The underlying WebSocket connection
	send chan []byte      // Outbound message queue (buffered: 256)
}

// writePump reads from the send channel and writes to the WebSocket connection.
func (c *Client) writePump(ctx context.Context) {
	defer func() {
		c.conn.Close(websocket.StatusNormalClosure, "server closing")
		c.hub.unregister <- c
	}()

	for {
		select {
		case <-ctx.Done():
			return
		case msg, ok := <-c.send:
			if !ok {
				// Hub closed the channel
				return
			}
			writeCtx, cancel := context.WithTimeout(ctx, writeTimeout)
			err := c.conn.Write(writeCtx, websocket.MessageText, msg)
			cancel()
			if err != nil {
				slog.Debug("ws write failed", "error", err)
				return
			}
		}
	}
}

// readPump reads from the WebSocket connection.
// Currently it only keeps the connection alive and detects client disconnect.
//
// Future expansion: Parse incoming JSON messages to support client-to-server
// commands (e.g., cancel process, subscribe to specific streams, request
// state snapshots). The hub would need a new channel for incoming messages.
func (c *Client) readPump(ctx context.Context) {
	defer func() {
		c.hub.unregister <- c
	}()

	for {
		_, _, err := c.conn.Read(ctx)
		if err != nil {
			// Client disconnected or error — clean up
			if websocket.CloseStatus(err) != -1 {
				slog.Debug("ws client closed", "status", websocket.CloseStatus(err))
			}
			return
		}
		// Discard incoming messages for now.
		// Future: parse and route client commands.
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
