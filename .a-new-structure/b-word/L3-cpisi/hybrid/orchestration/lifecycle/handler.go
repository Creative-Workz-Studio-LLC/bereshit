//omni:code --go -library
//omni:key B-L3-lifecycle-handler
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
// Package: lifecycle
// File: handler.go
// Purpose: Handler interfaces for lifecycle events
// Biblical: "Whatsoever thy hand findeth to do, do it with thy might" - Eccl 9:10
//
// These interfaces define HOW substrates respond to lifecycle events.
// Each substrate (Claude Code, GPT, MillenniumOS) implements these
// interfaces with platform-specific logic.

package lifecycle

// ============================================================================
// SETUP
// ============================================================================

// Context carries information through the lifecycle
type Context struct {
	SessionID   string            // Unique session identifier
	WorkingDir  string            // Current working directory
	Event       Event             // Current event being processed
	Phase       Phase             // Genesis phase of event
	Metadata    map[string]string // Additional context data
}

// Result represents the outcome of handling an event
type Result struct {
	Allow           bool              // Should the operation proceed?
	BlockReason     string            // If not allowed, why?
	AdditionalContext string          // Context to inject
	Metadata        map[string]string // Data to pass forward
}

// ============================================================================
// BODY
// ============================================================================

// Handler processes a lifecycle event
type Handler interface {
	// Handle processes the event and returns a result
	Handle(ctx Context) Result

	// Events returns the events this handler responds to
	Events() []Event
}

// SessionHandler handles session lifecycle events
type SessionHandler interface {
	Handler
	OnSessionStart(ctx Context) Result
	OnSessionEnd(ctx Context) Result
	OnSessionStop(ctx Context) Result
}

// ToolHandler handles tool lifecycle events
type ToolHandler interface {
	Handler
	OnPreToolUse(ctx Context) Result
	OnPostToolUse(ctx Context) Result
}

// PromptHandler handles prompt lifecycle events
type PromptHandler interface {
	Handler
	OnPromptSubmit(ctx Context) Result
	OnPromptComplete(ctx Context) Result
}

// --- Helper Functions ---

// NewContext creates a new lifecycle context
func NewContext(sessionID, workingDir string, event Event) Context {
	return Context{
		SessionID:  sessionID,
		WorkingDir: workingDir,
		Event:      event,
		Phase:      event.GetPhase(),
		Metadata:   make(map[string]string),
	}
}

// NewAllowResult creates a result that allows continuation
func NewAllowResult() Result {
	return Result{
		Allow:    true,
		Metadata: make(map[string]string),
	}
}

// NewBlockResult creates a result that blocks continuation
func NewBlockResult(reason string) Result {
	return Result{
		Allow:       false,
		BlockReason: reason,
		Metadata:    make(map[string]string),
	}
}

// NewContextResult creates a result with additional context
func NewContextResult(context string) Result {
	return Result{
		Allow:             true,
		AdditionalContext: context,
		Metadata:          make(map[string]string),
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// Handler interfaces are the bridge between paradigm and substrate.
// The paradigm defines WHAT events occur.
// Handlers define HOW the substrate responds.
//
// "Whatsoever thy hand findeth to do, do it with thy might" — Ecclesiastes 9:10
