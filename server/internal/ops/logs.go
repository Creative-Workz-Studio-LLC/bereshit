// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-logs
// Purpose: Log streaming via SSH journalctl
// Biblical: Habakkuk 2:2 — "Write the vision, and make it plain"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

package ops

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"fmt"
	"io"
	"strings"

	"cws.studio/server/internal/config"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// LogOptions configures log retrieval.
type LogOptions struct {
	Service string // Service name filter (empty = cws-server)
	Follow  bool   // Stream live (-f)
	Lines   int    // Number of recent lines (default 50)
	Since   string // Time filter (e.g., "1h", "30m", "2024-01-01")
}

// StreamLogs streams journal logs to the writer. If Follow is true, this
// blocks until context is cancelled.
func StreamLogs(ctx context.Context, host config.Host, opts LogOptions, w io.Writer) error {
	service := opts.Service
	if service == "" {
		service = "cws-server"
	}

	if !IsManaged(service) {
		return fmt.Errorf("unknown service: %s", service)
	}

	var parts []string
	parts = append(parts, "sudo", "journalctl")
	parts = append(parts, "-u", service+".service")
	parts = append(parts, "--no-pager")

	if opts.Follow {
		parts = append(parts, "-f")
	}

	lines := opts.Lines
	if lines <= 0 {
		lines = 50
	}
	parts = append(parts, fmt.Sprintf("-n%d", lines))

	if opts.Since != "" {
		parts = append(parts, fmt.Sprintf("--since=%s", opts.Since))
	}

	return StreamSSH(ctx, host, strings.Join(parts, " "), w)
}

// GetRecentLogs returns recent log lines as a string (non-streaming).
func GetRecentLogs(ctx context.Context, host config.Host, service string, lines int) (string, error) {
	if service == "" {
		service = "cws-server"
	}
	if !IsManaged(service) {
		return "", fmt.Errorf("unknown service: %s", service)
	}
	if lines <= 0 {
		lines = 50
	}

	return RunSSHCapture(ctx, host, fmt.Sprintf(
		"sudo journalctl -u %s.service --no-pager -n%d",
		service, lines,
	))
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
