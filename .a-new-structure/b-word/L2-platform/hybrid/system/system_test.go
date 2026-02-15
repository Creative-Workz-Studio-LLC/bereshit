//omni:code --go -test
//omni:key B-L2-system-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-system-test
// Purpose: Unit tests for system health utilities (load, memory, disk)
// Biblical: Proverbs 27:23 — "Be thou diligent to know the state"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package system

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import "testing"

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// parseFloat — hand-rolled parser used for /proc data
// ───────────────────────────────────────────────────────────────────────────

func TestParseFloatInteger(t *testing.T) {
	if got := parseFloat("42"); got != 42.0 {
		t.Errorf("parseFloat(42) = %f, want 42.0", got)
	}
}

func TestParseFloatDecimal(t *testing.T) {
	got := parseFloat("3.14")
	// Allow small floating point tolerance.
	if got < 3.13 || got > 3.15 {
		t.Errorf("parseFloat(3.14) = %f, want ~3.14", got)
	}
}

func TestParseFloatZero(t *testing.T) {
	if got := parseFloat("0"); got != 0.0 {
		t.Errorf("parseFloat(0) = %f, want 0.0", got)
	}
}

func TestParseFloatLeadingZeroDecimal(t *testing.T) {
	got := parseFloat("0.75")
	if got < 0.74 || got > 0.76 {
		t.Errorf("parseFloat(0.75) = %f, want ~0.75", got)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// parseKB — extract KB value from /proc/meminfo line format
// ───────────────────────────────────────────────────────────────────────────

func TestParseKBTypicalLine(t *testing.T) {
	got := parseKB("MemTotal:       16384000 kB")
	if got != 16384000 {
		t.Errorf("parseKB = %d, want 16384000", got)
	}
}

func TestParseKBShortLine(t *testing.T) {
	got := parseKB("Invalid:")
	if got != 0 {
		t.Errorf("parseKB short line = %d, want 0", got)
	}
}

func TestParseKBEmptyString(t *testing.T) {
	got := parseKB("")
	if got != 0 {
		t.Errorf("parseKB empty = %d, want 0", got)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// GetLoad — Linux-specific, verify structure on this system
// ───────────────────────────────────────────────────────────────────────────

func TestGetLoadHasInfo(t *testing.T) {
	load := GetLoad()
	if !load.HasInfo {
		t.Skip("not on Linux or /proc/loadavg not available")
	}
	if load.Avg1 < 0 {
		t.Error("load average should not be negative")
	}
}

func TestGetLoadDisplayHasInfo(t *testing.T) {
	d := GetLoadDisplay()
	if !d.HasInfo {
		t.Skip("not on Linux or /proc/loadavg not available")
	}
	if d.Icon == "" {
		t.Error("LoadDisplay should have an icon")
	}
	if d.Color == "" {
		t.Error("LoadDisplay should have a color")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// GetMemory — Linux-specific, verify structure on this system
// ───────────────────────────────────────────────────────────────────────────

func TestGetMemoryHasInfo(t *testing.T) {
	mem := GetMemory()
	if !mem.HasInfo {
		t.Skip("not on Linux or /proc/meminfo not available")
	}
	if mem.TotalKB <= 0 {
		t.Error("total memory should be positive")
	}
	if mem.Percent < 0 || mem.Percent > 100 {
		t.Errorf("memory percent %f should be between 0 and 100", mem.Percent)
	}
}

func TestGetMemoryDisplayHasInfo(t *testing.T) {
	d := GetMemoryDisplay()
	if !d.HasInfo {
		t.Skip("not on Linux or /proc/meminfo not available")
	}
	if d.TotalGB <= 0 {
		t.Error("total GB should be positive")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// GetDisk — verify for root filesystem
// ───────────────────────────────────────────────────────────────────────────

func TestGetDiskRoot(t *testing.T) {
	disk := GetDisk("/")
	if !disk.HasInfo {
		t.Skip("disk info not available")
	}
	if disk.TotalBytes == 0 {
		t.Error("total bytes should not be 0")
	}
	if disk.Percent < 0 || disk.Percent > 100 {
		t.Errorf("disk percent %f should be between 0 and 100", disk.Percent)
	}
}

func TestGetDiskEmptyDefaultsToRoot(t *testing.T) {
	disk := GetDisk("")
	if !disk.HasInfo {
		t.Skip("disk info not available")
	}
	if disk.TotalBytes == 0 {
		t.Error("empty path should default to / and return info")
	}
}

func TestGetDiskDisplayRoot(t *testing.T) {
	d := GetDiskDisplay("/")
	if !d.HasInfo {
		t.Skip("disk info not available")
	}
	if d.Icon == "" {
		t.Error("DiskDisplay should have an icon")
	}
	if d.Color == "" {
		t.Error("DiskDisplay should have a color")
	}
}

func TestGetDiskNonexistentPath(t *testing.T) {
	disk := GetDisk("/nonexistent/path/99999")
	if disk.HasInfo {
		t.Error("nonexistent path should have HasInfo=false")
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
