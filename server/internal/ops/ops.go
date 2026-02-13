// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-core
// Purpose: CWS Ops — shared types and package documentation
// Biblical: Nehemiah 4:17 — "with one hand wrought in the work, and with
//           the other hand held a weapon"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

// Package ops provides infrastructure management operations for CWS
// production servers. It wraps SSH, systemd, Cloudflare, and Litestream
// into composable operations that both the CLI and web GUI consume.
//
// Design decisions:
//   - Exec-based SSH (os/exec) rather than x/crypto/ssh — reuses the
//     operator's existing SSH config, keys, and agent. No key management.
//   - Operations return structured types, not raw strings — callers
//     decide formatting (CLI table, JSON, web component).
//   - Every destructive operation is a separate function with clear naming
//     so callers can gate on confirmation before invoking.
package ops

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import "time"

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ExecResult holds the output of a remote command execution.
type ExecResult struct {
	Stdout   string        // Standard output (trimmed)
	Stderr   string        // Standard error (trimmed)
	ExitCode int           // Process exit code (0 = success)
	Duration time.Duration // Wall-clock execution time
}

// OK returns true if the command exited with code 0.
func (r ExecResult) OK() bool { return r.ExitCode == 0 }

// ServiceStatus represents the state of a systemd service.
type ServiceStatus struct {
	Name     string `json:"name"`
	Active   bool   `json:"active"`   // systemd active state
	Enabled  bool   `json:"enabled"`  // systemd enabled state
	SubState string `json:"subState"` // "running", "dead", "exited", etc.
	PID      int    `json:"pid"`      // Main PID (0 if not running)
	Memory   string `json:"memory"`   // Human-readable memory usage
	Uptime   string `json:"uptime"`   // Human-readable time since start
}

// ResourceInfo holds system resource utilization.
type ResourceInfo struct {
	CPUPercent  float64 `json:"cpuPercent"`
	MemoryTotal uint64  `json:"memoryTotal"` // bytes
	MemoryUsed  uint64  `json:"memoryUsed"`  // bytes
	DiskTotal   uint64  `json:"diskTotal"`   // bytes
	DiskUsed    uint64  `json:"diskUsed"`    // bytes
	LoadAvg1    float64 `json:"loadAvg1"`
	LoadAvg5    float64 `json:"loadAvg5"`
	LoadAvg15   float64 `json:"loadAvg15"`
}

// FullStatus is the complete infrastructure status returned by the
// status command. Each field is populated independently so partial
// failures don't prevent returning what we have.
type FullStatus struct {
	Host      string          `json:"host"`
	Timestamp time.Time       `json:"timestamp"`
	Services  []ServiceStatus `json:"services"`
	Resources *ResourceInfo   `json:"resources,omitempty"`
	ServerURL string          `json:"serverUrl"`
	Healthy   bool            `json:"healthy"`
	Errors    []string        `json:"errors,omitempty"`
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
