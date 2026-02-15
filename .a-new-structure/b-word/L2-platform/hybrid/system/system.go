//omni:code --go -library
//omni:key B-L2-util-term-system
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-util-term-system
// Purpose: Shared system health utilities (load, memory, disk)
// Biblical: Proverbs 27:23 - "Be thou diligent to know the state"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Used by: statusline, hooks
//
// ═══════════════════════════════════════════════════════════════════════════

package system

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"strings"
	"syscall"

	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/registry"
	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/terminal/display"
)

// ────────────────────────────────────────────────────────────────────────────
// S.1 Config — database-backed thresholds with hardcoded fallback
// ────────────────────────────────────────────────────────────────────────────

// Config holds monitoring thresholds. Populated from platform.db
// system_monitoring table at startup, or defaults if DB unavailable.
type Config struct {
	LoadYellow   float64
	LoadRed      float64
	LoadIcon     string
	MemYellow    float64
	MemRed       float64
	MemIcon      string
	DiskYellow   float64
	DiskRed      float64
	DiskIcon     string
}

// DefaultConfig returns hardcoded fallback thresholds.
// These match the values in monitoring.toml — if the database is
// unavailable, the system still monitors with sane defaults.
func DefaultConfig() Config {
	return Config{
		LoadYellow: 2.0, LoadRed: 4.0, LoadIcon: "📊",
		MemYellow: 70.0, MemRed: 90.0, MemIcon: "🧠",
		DiskYellow: 80.0, DiskRed: 95.0, DiskIcon: "💾",
	}
}

// package-level config — set once at init, read everywhere
var cfg = DefaultConfig()

// Init sets the package configuration. Pass nil to keep defaults.
func Init(c *Config) {
	if c != nil {
		cfg = *c
	}
}

// ────────────────────────────────────────────────────────────────────────────
// S.2 Types
// ────────────────────────────────────────────────────────────────────────────

// Load holds system load info
type Load struct {
	Avg1    float64
	Avg5    float64
	Avg15   float64
	HasInfo bool
}

// LoadDisplay holds formatted load display
type LoadDisplay struct {
	LoadAvg float64
	Icon    string
	Color   string
	HasInfo bool
}

// Memory holds memory info
type Memory struct {
	TotalKB     int64
	AvailableKB int64
	UsedKB      int64
	Percent     float64
	HasInfo     bool
}

// MemoryDisplay holds formatted memory display
type MemoryDisplay struct {
	UsedGB  float64
	TotalGB float64
	Percent float64
	Icon    string
	Color   string
	HasInfo bool
}

// Disk holds disk info
type Disk struct {
	TotalBytes uint64
	FreeBytes  uint64
	UsedBytes  uint64
	Percent    float64
	HasInfo    bool
}

// DiskDisplay holds formatted disk display
type DiskDisplay struct {
	Percent float64
	Icon    string
	Color   string
	HasInfo bool
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Load ---

// GetLoad returns system load averages
func GetLoad() Load {
	data, err := os.ReadFile("/proc/loadavg")
	if err != nil {
		return Load{HasInfo: false}
	}

	parts := strings.Fields(string(data))
	if len(parts) < 3 {
		return Load{HasInfo: false}
	}

	return Load{
		Avg1:    parseFloat(parts[0]),
		Avg5:    parseFloat(parts[1]),
		Avg15:   parseFloat(parts[2]),
		HasInfo: true,
	}
}

// GetLoadDisplay returns formatted load display
func GetLoadDisplay() LoadDisplay {
	load := GetLoad()
	if !load.HasInfo {
		return LoadDisplay{HasInfo: false}
	}

	color := display.Green
	if load.Avg1 > cfg.LoadYellow {
		color = display.Yellow
	}
	if load.Avg1 > cfg.LoadRed {
		color = display.Red
	}

	return LoadDisplay{
		LoadAvg: load.Avg1,
		Icon:    cfg.LoadIcon,
		Color:   color,
		HasInfo: true,
	}
}

// --- Memory ---

// GetMemory returns memory usage
func GetMemory() Memory {
	data, err := os.ReadFile("/proc/meminfo")
	if err != nil {
		return Memory{HasInfo: false}
	}

	var total, available int64
	for _, line := range strings.Split(string(data), "\n") {
		if strings.HasPrefix(line, "MemTotal:") {
			total = parseKB(line)
		} else if strings.HasPrefix(line, "MemAvailable:") {
			available = parseKB(line)
		}
	}

	if total == 0 {
		return Memory{HasInfo: false}
	}

	used := total - available
	percent := float64(used) / float64(total) * 100

	return Memory{
		TotalKB:     total,
		AvailableKB: available,
		UsedKB:      used,
		Percent:     percent,
		HasInfo:     true,
	}
}

// GetMemoryDisplay returns formatted memory display
func GetMemoryDisplay() MemoryDisplay {
	mem := GetMemory()
	if !mem.HasInfo {
		return MemoryDisplay{HasInfo: false}
	}

	color := display.Green
	if mem.Percent > cfg.MemYellow {
		color = display.Yellow
	}
	if mem.Percent > cfg.MemRed {
		color = display.Red
	}

	return MemoryDisplay{
		UsedGB:  float64(mem.UsedKB) / 1024 / 1024,
		TotalGB: float64(mem.TotalKB) / 1024 / 1024,
		Percent: mem.Percent,
		Icon:    cfg.MemIcon,
		Color:   color,
		HasInfo: true,
	}
}

// --- Disk ---

// GetDisk returns disk usage for path
func GetDisk(path string) Disk {
	if path == "" {
		path = "/"
	}

	var stat syscall.Statfs_t
	if err := syscall.Statfs(path, &stat); err != nil {
		return Disk{HasInfo: false}
	}

	total := stat.Blocks * uint64(stat.Bsize)
	free := stat.Bfree * uint64(stat.Bsize)
	used := total - free
	percent := float64(used) / float64(total) * 100

	return Disk{
		TotalBytes: total,
		FreeBytes:  free,
		UsedBytes:  used,
		Percent:    percent,
		HasInfo:    true,
	}
}

// GetDiskDisplay returns formatted disk display
func GetDiskDisplay(path string) DiskDisplay {
	disk := GetDisk(path)
	if !disk.HasInfo {
		return DiskDisplay{HasInfo: false}
	}

	color := display.Green
	if disk.Percent > cfg.DiskYellow {
		color = display.Yellow
	}
	if disk.Percent > cfg.DiskRed {
		color = display.Red
	}

	return DiskDisplay{
		Percent: disk.Percent,
		Icon:    cfg.DiskIcon,
		Color:   color,
		HasInfo: true,
	}
}

// --- Helpers ---

func parseFloat(s string) float64 {
	var result float64
	var decimal float64 = 1
	var afterDot bool

	for _, c := range s {
		if c == '.' {
			afterDot = true
			continue
		}
		if c >= '0' && c <= '9' {
			if afterDot {
				decimal *= 10
				result += float64(c-'0') / decimal
			} else {
				result = result*10 + float64(c-'0')
			}
		}
	}
	return result
}

func parseKB(line string) int64 {
	parts := strings.Fields(line)
	if len(parts) < 2 {
		return 0
	}

	var result int64
	for _, c := range parts[1] {
		if c >= '0' && c <= '9' {
			result = result*10 + int64(c-'0')
		}
	}
	return result
}

// ────────────────────────────────────────────────────────────────────────────
// B.5 Registry Integration
// ────────────────────────────────────────────────────────────────────────────

// ConfigFromRegistry builds a Config from the L2 platform database
// via the cross-layer registry. Falls back to defaults for any field
// that can't be loaded.
func ConfigFromRegistry(r *registry.Registry) Config {
	c := DefaultConfig()

	if items, err := r.Platform.MonitoringByMetric("load"); err == nil {
		for _, m := range items {
			switch m.Level {
			case "yellow":
				c.LoadYellow = m.Threshold
			case "red":
				c.LoadRed = m.Threshold
			}
			if m.Icon != "" {
				c.LoadIcon = m.Icon
			}
		}
	}

	if items, err := r.Platform.MonitoringByMetric("memory"); err == nil {
		for _, m := range items {
			switch m.Level {
			case "yellow":
				c.MemYellow = m.Threshold
			case "red":
				c.MemRed = m.Threshold
			}
			if m.Icon != "" {
				c.MemIcon = m.Icon
			}
		}
	}

	if items, err := r.Platform.MonitoringByMetric("disk"); err == nil {
		for _, m := range items {
			switch m.Level {
			case "yellow":
				c.DiskYellow = m.Threshold
			case "red":
				c.DiskRed = m.Threshold
			}
			if m.Icon != "" {
				c.DiskIcon = m.Icon
			}
		}
	}

	return c
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
