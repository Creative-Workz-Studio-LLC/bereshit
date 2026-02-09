// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-system
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

	"cws.studio/pkg/util/term/display"
)

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
	if load.Avg1 > 2.0 {
		color = display.Yellow
	}
	if load.Avg1 > 4.0 {
		color = display.Red
	}

	return LoadDisplay{
		LoadAvg: load.Avg1,
		Icon:    "📊",
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
	if mem.Percent > 70 {
		color = display.Yellow
	}
	if mem.Percent > 90 {
		color = display.Red
	}

	return MemoryDisplay{
		UsedGB:  float64(mem.UsedKB) / 1024 / 1024,
		TotalGB: float64(mem.TotalKB) / 1024 / 1024,
		Percent: mem.Percent,
		Icon:    "🧠",
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
	if disk.Percent > 80 {
		color = display.Yellow
	}
	if disk.Percent > 95 {
		color = display.Red
	}

	return DiskDisplay{
		Percent: disk.Percent,
		Icon:    "💾",
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

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
