//omni:code --go -library
//omni:key B-L0-config-util-tripwire
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-config-util-tripwire
// Purpose: Tripwire warning utilities — loud fallback notifications
// Biblical: Ezekiel 33:6 - "If the watchman see the sword come, and blow not the trumpet"
// Authors: Nova Dawn
// Version: b-01.00
// Created: 2026-02-13
//
// Extracted from 2 identical warning mechanisms:
//   - L0-universal/hybrid/config/loader.go (index.toml tripwire banner)
//   - L3-cpisi/orchestration/config/loader.go (config.toml tripwire banner)
//
// Tripwire pattern: Primary path -> Missing? -> LOUD WARNING -> Fallback path
// The warning ensures silent degradation never hides a missing source of truth.
//
// ============================================================================
// END METADATA
// ============================================================================

package util

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"io"
	"os"
	"strings"
	"sync"
)

// ────────────────────────────────────────────────────────────────
// Deduplication
// ────────────────────────────────────────────────────────────────

// bannerOnce tracks which banners have already fired.
// In long-running processes (servers, watchers), the same tripwire
// can trigger on every config reload — printing the banner once is enough.
var (
	bannerMu   sync.Mutex
	bannerSeen = make(map[string]bool)
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Warning — single line to stderr
// ────────────────────────────────────────────────────────────────

// TripwireWarning prints a single-line warning to stderr.
// Use for non-critical fallbacks that should still be visible.
//
// Format: "WARNING [system]: reason (fallback: path)"
func TripwireWarning(system, fallbackPath, reason string) {
	TripwireWarningTo(os.Stderr, system, fallbackPath, reason)
}

// TripwireWarningTo writes a single-line warning to w.
// Use for testability or when stderr isn't the right destination.
func TripwireWarningTo(w io.Writer, system, fallbackPath, reason string) {
	fmt.Fprintf(w, "WARNING [%s]: %s (fallback: %s)\n", system, reason, fallbackPath)
}

// ────────────────────────────────────────────────────────────────
// Banner — bordered block to stdout
// ────────────────────────────────────────────────────────────────

// TripwireBanner prints a bordered warning block to stdout.
// Use when a primary config source is missing and the system is running
// on hardcoded fallbacks — this MUST be visible.
//
// Output:
//
//	════════════════════════════════════════════════════════════════
//	  TRIPWIRE: <title>
//	════════════════════════════════════════════════════════════════
//	  <line 1>
//	  <line 2>
//	════════════════════════════════════════════════════════════════
func TripwireBanner(title string, lines []string) {
	TripwireBannerTo(os.Stdout, title, lines)
}

// TripwireBannerTo writes a bordered warning block to w.
// Use for testability or when stdout isn't the right destination.
func TripwireBannerTo(w io.Writer, title string, lines []string) {
	border := strings.Repeat("\u2550", 64) // ═ × 64
	fmt.Fprintln(w, border)
	fmt.Fprintf(w, "  TRIPWIRE: %s\n", title)
	fmt.Fprintln(w, border)
	for _, line := range lines {
		fmt.Fprintf(w, "  %s\n", line)
	}
	fmt.Fprintln(w, border)
}

// TripwireBannerOnce prints a bordered warning block at most once per title.
// Subsequent calls with the same title are silently ignored.
//
// Use in long-running processes (servers, watchers) where config reloads
// may trigger the same tripwire repeatedly.
func TripwireBannerOnce(title string, lines []string) {
	bannerMu.Lock()
	if bannerSeen[title] {
		bannerMu.Unlock()
		return
	}
	bannerSeen[title] = true
	bannerMu.Unlock()

	TripwireBanner(title, lines)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "But if the watchman see the sword come, and blow not the trumpet,
// and the people be not warned; if the sword come, and take any person
// from among them... his blood will I require at the watchman's hand."
// — Ezekiel 33:6
//
// Silent fallbacks are the sword that comes unseen. The tripwire
// is the trumpet. Blow it loud.
//
// ============================================================================
// END CLOSING
// ============================================================================
