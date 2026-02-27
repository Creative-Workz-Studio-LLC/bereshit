//omni:key claude-global-pkg-hookoutput-substrate
//omni:code go
//omni:version 2.1.0
// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-hookoutput-substrate
// Purpose: Substrate detection — determine Claude Code vs Gemini CLI at runtime
// Biblical: "To every thing there is a season" - Ecclesiastes 3:1
// Authors: Nova Dawn
// Version: 2.1.0
// Created: 2026-02-24
// Updated: 2026-02-24
//
// Substrate detection uses environment variable heuristics to determine
// which CLI tool invoked the hook. Detection is cached via sync.Once
// for safe concurrent access.
//
// ============================================================================
// END METADATA
// ============================================================================

// Package hookoutput provides substrate-agnostic hook output types for CPI-SI instances.
//
// Both Claude Code and Gemini CLI are supported through automatic substrate
// detection. The same Go binary produces correct JSON output for whichever
// substrate invoked it.
//
// Usage:
//
//	hookoutput.NewSessionStartResponse(context)       // Context injection
//	hookoutput.NewPreToolAllow()                       // Permission control
//	hookoutput.NewStopBlock(reason)                    // Stop interception
//	hookoutput.IsGemini()                              // Substrate check
package hookoutput

// ============================================================================
// SETUP
// ============================================================================

// --- Imports ---

import (
	"os"
	"sync"
)

// --- Constants ---

// Substrate represents the CLI tool invoking this hook
type Substrate int

const (
	SubstrateClaude Substrate = iota // Claude Code (default)
	SubstrateGemini                  // Gemini CLI
)

// --- Variables ---

// detectedSubstrate caches the substrate detection result
var (
	detectedSubstrate     Substrate
	substrateDetectedOnce sync.Once
)

// Pragma exposes the file's OmniCode identity for runtime inspection
var Pragma = [2]string{"key", "claude-global-pkg-hookoutput-substrate"}

// Metadata exposes version info for runtime inspection
var Metadata = [2]string{"version", "2.1.0"}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// --- Core Operations ---

// DetectSubstrate determines which CLI tool is running this hook.
// Detection is cached — safe to call multiple times.
//
// Heuristic:
//
//   - GEMINI_PROJECT_DIR set → Gemini CLI
//   - GEMINI_SESSION_ID set → Gemini CLI
//   - Otherwise → Claude Code
func DetectSubstrate() Substrate {
	substrateDetectedOnce.Do(func() {
		if os.Getenv("GEMINI_PROJECT_DIR") != "" || os.Getenv("GEMINI_SESSION_ID") != "" {
			detectedSubstrate = SubstrateGemini
		} else {
			detectedSubstrate = SubstrateClaude
		}
	})
	return detectedSubstrate
}

// IsGemini returns true if running under Gemini CLI
func IsGemini() bool {
	return DetectSubstrate() == SubstrateGemini
}

// IsClaude returns true if running under Claude Code
func IsClaude() bool {
	return DetectSubstrate() == SubstrateClaude
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Substrate detection flow:
//   DetectSubstrate() → cached Substrate value
//   IsGemini() / IsClaude() → boolean convenience wrappers
//
// Environment heuristic:
//   GEMINI_PROJECT_DIR or GEMINI_SESSION_ID present → Gemini CLI
//   Otherwise → Claude Code (default)
//
// ============================================================================
// END CLOSING
// ============================================================================
