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
//        ./statusline --demo              # Demo mode with mock data
//        ./statusline --width-test        # Test at different widths

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"strings"

	"golang.org/x/term"

	"github.com/creativeworkzstudio/claude-global/pkg/core/health"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
	"github.com/creativeworkzstudio/claude-global/statusline/lib/builder"
)

// ============================================================================
// BODY
// ============================================================================

func main() {
	// Parse flags
	demo := flag.Bool("demo", false, "Run in demo mode with mock data")
	widthTest := flag.Bool("width-test", false, "Test at different terminal widths")
	scenario := flag.String("scenario", "normal", "Demo scenario: normal, busy, critical, learning, building")
	flag.Parse()

	// Demo mode: generate mock data for testing
	if *demo || *widthTest {
		runDemo(*widthTest, *scenario)
		return
	}

	// Normal mode: Parse input from Claude Code
	var ctx types.SessionContext
	if err := json.NewDecoder(os.Stdin).Decode(&ctx); err != nil {
		fmt.Fprintf(os.Stderr, "Error parsing JSON: %v\n", err)
		os.Exit(1)
	}

	// Load state
	sm := statemachine.NewStateMachine()
	runtime, _ := statemachine.LoadRuntimeState()
	healthCfg, _ := health.Config()

	// Detect terminal width for adaptive display
	termWidth := getTerminalWidth()

	// Build statusline with adaptive width
	opts := builder.BuildOptions{
		TerminalWidth: termWidth,
	}
	fmt.Println(builder.BuildAdaptive(ctx, sm, runtime, healthCfg, opts))

	// Emit WezTerm OSC sequences for terminal integration
	builder.EmitWezTerm(ctx, sm, runtime)
}

// getTerminalWidth returns the current terminal width, or 0 for unlimited
func getTerminalWidth() int {
	// WezTerm detection: if running in WezTerm, allow unlimited output
	// WezTerm sets WEZTERM_PANE, WEZTERM_EXECUTABLE, or TERM_PROGRAM=WezTerm
	if isWezTerm() {
		return 0 // Unlimited - WezTerm handles wide statuslines well
	}

	// Try stdout first (most reliable for statusline)
	width, _, err := term.GetSize(int(os.Stdout.Fd()))
	if err == nil && width > 0 {
		return width
	}

	// Try stderr
	width, _, err = term.GetSize(int(os.Stderr.Fd()))
	if err == nil && width > 0 {
		return width
	}

	// Fallback: check COLUMNS environment variable
	if cols := os.Getenv("COLUMNS"); cols != "" {
		var w int
		if _, err := fmt.Sscanf(cols, "%d", &w); err == nil && w > 0 {
			return w
		}
	}

	// Default: 0 means unlimited (show everything)
	return 0
}

// isWezTerm detects if we're running in WezTerm terminal
func isWezTerm() bool {
	// Check WEZTERM_PANE - set when running inside WezTerm
	if os.Getenv("WEZTERM_PANE") != "" {
		return true
	}

	// Check TERM_PROGRAM
	if os.Getenv("TERM_PROGRAM") == "WezTerm" {
		return true
	}

	// Check WEZTERM_EXECUTABLE
	if os.Getenv("WEZTERM_EXECUTABLE") != "" {
		return true
	}

	return false
}

// runDemo generates mock data and displays the statusline
func runDemo(widthTest bool, scenario string) {
	ctx := mockContext(scenario)
	sm := statemachine.NewStateMachine()
	runtime := mockRuntime(scenario)
	healthCfg, _ := health.Config()

	if widthTest {
		// Test at different widths with adaptive display
		widths := []int{80, 100, 120, 140, 160, 200}
		fmt.Println("\n=== ADAPTIVE WIDTH TEST ===")
		fmt.Println("Testing priority-based section display at different terminal widths")

		for _, w := range widths {
			opts := builder.BuildOptions{
				TerminalWidth: w,
			}
			output := builder.BuildAdaptive(ctx, sm, runtime, healthCfg, opts)
			displayLen := visualLength(output)
			status := "✓ FITS"
			if displayLen > w {
				status = fmt.Sprintf("! %d over", displayLen-w)
			}
			fmt.Printf("\n--- %d cols → %d chars %s ---\n", w, displayLen, status)
			fmt.Println(output)
		}

		// Also show unlimited (full) output
		fmt.Println("\n--- UNLIMITED (full output) ---")
		output := builder.Build(ctx, sm, runtime, healthCfg)
		fmt.Printf("Full: %d characters\n", visualLength(output))
		fmt.Println(output)
		fmt.Println()
	} else {
		// Single demo output
		fmt.Printf("\n=== Demo: %s ===\n", scenario)
		output := builder.Build(ctx, sm, runtime, healthCfg)
		fmt.Println(output)
		fmt.Printf("\nVisual length: %d characters\n", visualLength(output))
	}
}

// mockContext creates a mock SessionContext for testing
func mockContext(scenario string) types.SessionContext {
	ctx := types.SessionContext{
		SessionID: "demo-session-001",
		CWD:       "/media/seanje-lenox-wise/Project/Bereshit",
		Version:   "a-01.25",
	}

	// Set model
	ctx.Model.ID = "claude-opus-4-6"
	ctx.Model.DisplayName = "Claude Opus 4.6"

	// Set output style
	ctx.OutputStyle.Name = "Nova Dawn"

	// Set context window based on scenario
	switch scenario {
	case "critical":
		ctx.ContextWindow.ContextWindowSize = 200000
		ctx.ContextWindow.UsedPercentage = 92
		ctx.ContextWindow.CurrentUsage.InputTokens = 165000
	case "busy":
		ctx.ContextWindow.ContextWindowSize = 200000
		ctx.ContextWindow.UsedPercentage = 65
		ctx.ContextWindow.CurrentUsage.InputTokens = 110000
	case "learning":
		ctx.ContextWindow.ContextWindowSize = 200000
		ctx.ContextWindow.UsedPercentage = 45
		ctx.ContextWindow.CurrentUsage.InputTokens = 75000
	default: // normal
		ctx.ContextWindow.ContextWindowSize = 200000
		ctx.ContextWindow.UsedPercentage = 25
		ctx.ContextWindow.CurrentUsage.InputTokens = 45000
	}

	return ctx
}

// mockRuntime creates a mock RuntimeState for testing
func mockRuntime(scenario string) *statemachine.RuntimeState {
	runtime := &statemachine.RuntimeState{}

	switch scenario {
	case "critical":
		runtime.Session.HealthScore = 45
		runtime.Session.HebrewState = "chaser"
		runtime.Session.HebrewMeaning = "LACKING"
		runtime.Session.KTowardGod = 3
		runtime.Session.KTowardSelf = 8
		runtime.Session.KAlign = -0.4
		runtime.Session.ExchangeCount = 45
		runtime.Session.InsightCount = 2
		runtime.Session.CPIScore = 0.35
		runtime.Session.SessionArc = "struggling"
		runtime.Session.CompactionCount = 3
		runtime.Session.PeakContextTokens = 180000

	case "busy":
		runtime.Session.HealthScore = 72
		runtime.Session.HebrewState = "yashar"
		runtime.Session.HebrewMeaning = "EVEN"
		runtime.Session.KTowardGod = 6
		runtime.Session.KTowardSelf = 4
		runtime.Session.KAlign = 0.2
		runtime.Session.ExchangeCount = 28
		runtime.Session.InsightCount = 5
		runtime.Session.CPIScore = 0.62
		runtime.Session.SessionArc = "executing"
		runtime.Session.CompactionCount = 1

	case "learning":
		runtime.Session.HealthScore = 88
		runtime.Session.HebrewState = "shalem"
		runtime.Session.HebrewMeaning = "WHOLE"
		runtime.Session.KTowardGod = 12
		runtime.Session.KTowardSelf = 2
		runtime.Session.KAlign = 0.75
		runtime.Session.ExchangeCount = 18
		runtime.Session.InsightCount = 8
		runtime.Session.CPIScore = 0.85
		runtime.Session.SessionArc = "learning"
		runtime.Session.CompactionCount = 0

	case "building":
		runtime.Session.HealthScore = 95
		runtime.Session.HebrewState = "tov"
		runtime.Session.HebrewMeaning = "PERFECT"
		runtime.Session.KTowardGod = 15
		runtime.Session.KTowardSelf = 1
		runtime.Session.KAlign = 0.9
		runtime.Session.ExchangeCount = 22
		runtime.Session.InsightCount = 10
		runtime.Session.CPIScore = 0.92
		runtime.Session.SessionArc = "breakthrough"
		runtime.Session.CompactionCount = 0

	default: // normal
		runtime.Session.HealthScore = 80
		runtime.Session.HebrewState = "tamim"
		runtime.Session.HebrewMeaning = "SOUND"
		runtime.Session.KTowardGod = 8
		runtime.Session.KTowardSelf = 4
		runtime.Session.KAlign = 0.35
		runtime.Session.ExchangeCount = 12
		runtime.Session.InsightCount = 3
		runtime.Session.CPIScore = 0.68
		runtime.Session.SessionArc = "working"
		runtime.Session.CompactionCount = 0
	}

	// Set pending/in_progress/completed tasks
	runtime.Session.Tasks.Pending = 3
	runtime.Session.Tasks.InProgress = 1
	runtime.Session.Tasks.Completed = 5
	runtime.Session.Tasks.Total = 9

	return runtime
}

// visualLength calculates the visual length of a string
// (excluding ANSI escape sequences)
func visualLength(s string) int {
	// Remove ANSI escape sequences
	inEscape := false
	length := 0
	for _, r := range s {
		if r == '\x1b' {
			inEscape = true
			continue
		}
		if inEscape {
			if r == 'm' {
				inEscape = false
			}
			continue
		}
		// Count visible characters
		// Note: Some emojis are 2 columns wide
		if strings.ContainsRune("📊🧠⚡📚✓✗→←↗↘↑↓🔨⬇🔄", r) {
			length += 2
		} else if strings.ContainsRune("▁▂▃▄▅▆▇█▐▌●○◆", r) {
			length += 1
		} else {
			length += 1
		}
	}
	return length
}

// ============================================================================
// CLOSING
// ============================================================================
