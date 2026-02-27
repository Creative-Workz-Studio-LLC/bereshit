// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-chat-router
//omni:code  --go -package->chat
//omni:version a-00.10
//
// Purpose: Model router interface — substrate-agnostic chat abstraction.
//          Modules send messages through the Router; implementations decide
//          which model handles them (Gemini, Claude, local, etc.).
// Biblical: Proverbs 16:9 - "A man's heart deviseth his way:
//           but the LORD directeth his steps."
//
// ============================================================================
// END METADATA

package chat

// ============================================================================
// SETUP
// ============================================================================

import "context"

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// Router abstracts the model backend for conversational chat.
// Implementations handle connection, session management, and response parsing.
type Router interface {
	// Send delivers a user message and returns the model's response text.
	// Blocks until the response is fully received.
	Send(ctx context.Context, message string) (string, error)

	// Close tears down the underlying client and session.
	Close() error
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
