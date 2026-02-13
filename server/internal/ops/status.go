// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-status
// Purpose: Infrastructure status gathering — services, resources, health
// Biblical: Nehemiah 2:13 — "I went out... and viewed the walls"
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
	"strconv"
	"strings"
	"time"

	"cws.studio/server/internal/config"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// managedServices lists all systemd units we track. Order matches the
// display order in both CLI and web GUI.
var managedServices = []string{
	"cws-server",
	"cloudflared",
	"uptime-kuma",
	"litestream",
	"prometheus",
	"node-exporter",
	"grafana-server",
}

// GetFullStatus gathers complete infrastructure status from the remote host.
// Each section is collected independently — partial failures populate the
// Errors slice without preventing other sections from returning.
func GetFullStatus(ctx context.Context, host config.Host) (*FullStatus, error) {
	status := &FullStatus{
		Host:      host.Name,
		Timestamp: time.Now(),
		ServerURL: "https://dashboard.creativeworkzstudio.com",
	}

	// ── Services ─────────────────────────────────────────────────────
	services, err := GetServiceStatuses(ctx, host)
	if err != nil {
		status.Errors = append(status.Errors, fmt.Sprintf("services: %v", err))
	} else {
		status.Services = services
	}

	// ── Resources ────────────────────────────────────────────────────
	resources, err := GetResources(ctx, host)
	if err != nil {
		status.Errors = append(status.Errors, fmt.Sprintf("resources: %v", err))
	} else {
		status.Resources = resources
	}

	// ── Health check ─────────────────────────────────────────────────
	health, err := RunSSHCapture(ctx, host,
		"curl -sf http://localhost:3847/healthz -o /dev/null && echo ok || echo fail")
	if err != nil {
		status.Errors = append(status.Errors, fmt.Sprintf("health: %v", err))
	} else {
		status.Healthy = strings.TrimSpace(health) == "ok"
	}

	return status, nil
}

// GetServiceStatuses queries systemd for all managed services in a single
// SSH call. Parses systemctl show output to build structured status objects.
func GetServiceStatuses(ctx context.Context, host config.Host) ([]ServiceStatus, error) {
	// Build a single systemctl command that queries all services at once.
	// --property limits output to fields we need, avoiding full dump.
	units := strings.Join(managedServices, ".service ")
	units += ".service"

	output, err := RunSSHCapture(ctx, host, fmt.Sprintf(
		"systemctl show %s --property=Id,ActiveState,SubState,MainPID,MemoryCurrent,UnitFileState,ActiveEnterTimestamp --no-pager",
		units,
	))
	if err != nil {
		return nil, fmt.Errorf("systemctl show: %w", err)
	}

	return parseServiceStatuses(output), nil
}

// parseServiceStatuses parses systemctl show output into ServiceStatus
// objects. Units are separated by blank lines in systemctl's output.
func parseServiceStatuses(output string) []ServiceStatus {
	var services []ServiceStatus

	// systemctl show separates units with blank lines
	for block := range strings.SplitSeq(output, "\n\n") {
		if strings.TrimSpace(block) == "" {
			continue
		}

		props := parseProperties(block)
		name := strings.TrimSuffix(props["Id"], ".service")
		if name == "" {
			continue
		}

		pid, _ := strconv.Atoi(props["MainPID"])
		memBytes, _ := strconv.ParseUint(props["MemoryCurrent"], 10, 64)

		svc := ServiceStatus{
			Name:     name,
			Active:   props["ActiveState"] == "active",
			Enabled:  props["UnitFileState"] == "enabled",
			SubState: props["SubState"],
			PID:      pid,
			Memory:   formatBytes(memBytes),
		}

		// Calculate uptime from ActiveEnterTimestamp
		if ts := props["ActiveEnterTimestamp"]; ts != "" && svc.Active {
			svc.Uptime = parseUptime(ts)
		}

		services = append(services, svc)
	}

	return services
}

// GetResources collects system resource metrics in a single SSH call.
func GetResources(ctx context.Context, host config.Host) (*ResourceInfo, error) {
	// Single compound command — avoids multiple SSH round-trips
	output, err := RunSSHCapture(ctx, host, `
cat /proc/loadavg
free -b | grep Mem
df -B1 / | tail -1
grep -c processor /proc/cpuinfo
`)
	if err != nil {
		return nil, err
	}

	lines := strings.Split(strings.TrimSpace(output), "\n")
	if len(lines) < 4 {
		return nil, fmt.Errorf("unexpected output format: got %d lines", len(lines))
	}

	info := &ResourceInfo{}

	// Line 0: load averages (1m 5m 15m running/total lastPID)
	loadParts := strings.Fields(lines[0])
	if len(loadParts) >= 3 {
		info.LoadAvg1, _ = strconv.ParseFloat(loadParts[0], 64)
		info.LoadAvg5, _ = strconv.ParseFloat(loadParts[1], 64)
		info.LoadAvg15, _ = strconv.ParseFloat(loadParts[2], 64)
	}

	// Line 1: Mem: total used free shared buff/cache available
	memParts := strings.Fields(lines[1])
	if len(memParts) >= 3 {
		info.MemoryTotal, _ = strconv.ParseUint(memParts[1], 10, 64)
		info.MemoryUsed, _ = strconv.ParseUint(memParts[2], 10, 64)
	}

	// Line 2: filesystem 1B-blocks used available use% mount
	diskParts := strings.Fields(lines[2])
	if len(diskParts) >= 4 {
		info.DiskTotal, _ = strconv.ParseUint(diskParts[1], 10, 64)
		info.DiskUsed, _ = strconv.ParseUint(diskParts[2], 10, 64)
	}

	// Line 3: CPU count (for load average context)
	cpuCount, _ := strconv.ParseFloat(strings.TrimSpace(lines[3]), 64)
	if cpuCount > 0 {
		info.CPUPercent = (info.LoadAvg1 / cpuCount) * 100
	}

	return info, nil
}

// ── Helpers ──────────────────────────────────────────────────────────

// parseProperties turns "Key=Value\n" pairs into a map.
func parseProperties(block string) map[string]string {
	props := make(map[string]string)
	for line := range strings.SplitSeq(block, "\n") {
		if idx := strings.IndexByte(line, '='); idx > 0 {
			props[line[:idx]] = line[idx+1:]
		}
	}
	return props
}

// formatBytes returns a human-readable byte size.
func formatBytes(b uint64) string {
	switch {
	case b == 0:
		return "—"
	case b < 1024:
		return fmt.Sprintf("%dB", b)
	case b < 1024*1024:
		return fmt.Sprintf("%.1fKB", float64(b)/1024)
	case b < 1024*1024*1024:
		return fmt.Sprintf("%.1fMB", float64(b)/(1024*1024))
	default:
		return fmt.Sprintf("%.1fGB", float64(b)/(1024*1024*1024))
	}
}

// parseUptime converts a systemd timestamp string to a human-readable
// duration. Example input: "Thu 2026-02-12 10:30:00 UTC"
func parseUptime(ts string) string {
	// systemd uses a few timestamp formats. Try the common ones.
	formats := []string{
		"Mon 2006-01-02 15:04:05 MST",
		"2006-01-02 15:04:05 MST",
	}

	for _, format := range formats {
		if t, err := time.Parse(format, ts); err == nil {
			d := time.Since(t)
			switch {
			case d < time.Minute:
				return fmt.Sprintf("%ds", int(d.Seconds()))
			case d < time.Hour:
				return fmt.Sprintf("%dm", int(d.Minutes()))
			case d < 24*time.Hour:
				return fmt.Sprintf("%dh%dm", int(d.Hours()), int(d.Minutes())%60)
			default:
				days := int(d.Hours()) / 24
				hours := int(d.Hours()) % 24
				return fmt.Sprintf("%dd%dh", days, hours)
			}
		}
	}

	return "—"
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
