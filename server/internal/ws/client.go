// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-ws-client
// Purpose: Individual WebSocket connection — read and write pumps
// Authors: Nova Dawn
// Version: 1.0.0
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

const (
	writeTimeout = 10 * time.Second
	pongTimeout  = 60 * time.Second
)

// Client represents a single WebSocket connection.
type Client struct {
	hub  *Hub
	conn *websocket.Conn
	send chan []byte
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
// Currently just keeps the connection alive and detects close.
// Future: handle client-to-server messages.
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
