// ============================================================================
// METADATA - CPI-SI State Machine Statusline
// ============================================================================
// Purpose: Thin entry point - composes packages into statusline output
// Biblical: 1 Corinthians 14:40 - "Let all things be done decently and in order"
//
// Architecture: main.go is a CALLER, not a WORKER.
// All logic lives in packages:
//   - pkg/statemachine/display.go   → trajectory display
//   - pkg/display/command.go        → command colors
//   - pkg/health/display.go         → health display
//   - statusline/lib/sections/      → section builders
//   - statusline/lib/builder/       → orchestration
//
// Usage: echo '{"session_id":"..."}' | ./statusline

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"fmt"
	"os"

	"github.com/creativeworkzstudio/claude-global/pkg/core/health"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
	"github.com/creativeworkzstudio/claude-global/statusline/lib/builder"
)

// ============================================================================
// BODY
// ============================================================================

func main() {
	// Parse input from Claude Code
	var ctx types.SessionContext
	if err := json.NewDecoder(os.Stdin).Decode(&ctx); err != nil {
		fmt.Fprintf(os.Stderr, "Error parsing JSON: %v\n", err)
		os.Exit(1)
	}

	// Load state
	sm := statemachine.NewStateMachine()
	runtime, _ := statemachine.LoadRuntimeState()
	healthCfg, _ := health.Config()

	// Build and output statusline
	fmt.Println(builder.Build(ctx, sm, runtime, healthCfg))

	// Emit WezTerm OSC sequences for terminal integration
	builder.EmitWezTerm(ctx, sm, runtime)
}

// ============================================================================
// CLOSING
// ============================================================================
