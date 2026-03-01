//omni:key B-hooks-internal-status
//omni:code --go -library
//omni:version a-01.00

package status

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-hooks-internal-status
// Purpose: Triggers statusline and WezTerm updates from hooks
//

// (Identity variables omitted for brevity in bridge)

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

import (
	"bytes"
	"encoding/json"
	"os"
	"os/exec"
	"path/filepath"

	"cws.studio/pkg/foundation/types"
	"cws.studio/pkg/sdk/hookoutput"
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 22. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// Emit triggers the statusline update and WezTerm OSC emission.
func Emit(sessionID string) {
	// Construct basic context for statusline
	ctx := types.SessionContext{
		SessionID: sessionID,
	}
	
	// Determine substrate
	if hookoutput.IsGemini() {
		ctx.Model.DisplayName = "Gemini 2.0 Pro"
	} else {
		ctx.Model.DisplayName = "Claude Code"
	}

	data, err := json.Marshal(ctx)
	if err != nil {
		return
	}

	// Resolve statusline binary path
	home, _ := os.UserHomeDir()
	binPath := filepath.Join(home, ".local/share/claude/bin/statusline")

	// Execute statusline binary
	cmd := exec.Command(binPath)
	cmd.Stdin = bytes.NewReader(data)
	
	// We run it and ignore output - the binary itself writes to stdout/OSC
	_ = cmd.Run()
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
// Note: Hook Status Integration
// Scripture: "In the beginning God created the heaven and the earth." — Genesis 1:1
// ============================================================================
// END CLOSING
// ============================================================================
