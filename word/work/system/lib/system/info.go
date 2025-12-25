// ============================================================================
// METADATA
// ============================================================================
// System Information - Shared system stats library
// Provides raw system data for consumption by hooks, statusline, and other components
// Non-blocking: Returns data structures, doesn't print or format output

package system

// ============================================================================
// SETUP
// ============================================================================
import (
	"os"
	"os/exec"
	"runtime"
	"strconv"
	"strings"
)

// ============================================================================
// BODY
// ============================================================================

// Info contains live system statistics
type Info struct {
	LoadAvg1   float64 // 1-minute load average
	LoadAvg5   float64 // 5-minute load average
	LoadAvg15  float64 // 15-minute load average
	MemUsedGB  float64 // Memory used in GB
	MemTotalGB float64 // Total memory in GB
	CPUCount   int     // Number of CPU cores
}

// DiskInfo contains disk usage information
type DiskInfo struct {
	UsagePercent float64 // Percentage of disk used (0-100)
	Used         string  // Human-readable used space (e.g., "450G")
	Available    string  // Human-readable available space (e.g., "50G")
	Total        string  // Human-readable total space (e.g., "500G")
}

// GetInfo retrieves current system statistics (Linux)
func GetInfo() Info {
	info := Info{
		CPUCount: runtime.NumCPU(),
	}

	// Get load average from /proc/loadavg
	if data, err := os.ReadFile("/proc/loadavg"); err == nil {
		fields := strings.Fields(string(data))
		if len(fields) >= 3 {
			info.LoadAvg1, _ = strconv.ParseFloat(fields[0], 64)
			info.LoadAvg5, _ = strconv.ParseFloat(fields[1], 64)
			info.LoadAvg15, _ = strconv.ParseFloat(fields[2], 64)
		}
	}

	// Get memory info from /proc/meminfo
	if data, err := os.ReadFile("/proc/meminfo"); err == nil {
		lines := strings.Split(string(data), "\n")
		var memTotal, memAvailable int64

		for _, line := range lines {
			fields := strings.Fields(line)
			if len(fields) < 2 {
				continue
			}

			value, _ := strconv.ParseInt(fields[1], 10, 64)

			if strings.HasPrefix(line, "MemTotal:") {
				memTotal = value
			} else if strings.HasPrefix(line, "MemAvailable:") {
				memAvailable = value
			}
		}

		if memTotal > 0 {
			info.MemTotalGB = float64(memTotal) / 1024 / 1024 // KB to GB
			memUsed := memTotal - memAvailable
			info.MemUsedGB = float64(memUsed) / 1024 / 1024 // KB to GB
		}
	}

	return info
}

// GetDiskUsage retrieves disk usage information for given path
func GetDiskUsage(path string) DiskInfo {
	var diskInfo DiskInfo

	cmd := exec.Command("df", "-h", path)
	output, err := cmd.Output()
	if err != nil {
		return diskInfo
	}

	lines := strings.Split(string(output), "\n")
	if len(lines) < 2 {
		return diskInfo
	}

	fields := strings.Fields(lines[1])
	if len(fields) < 5 {
		return diskInfo
	}

	// Parse the output: Filesystem Size Used Avail Use% Mounted
	diskInfo.Total = fields[1]
	diskInfo.Used = fields[2]
	diskInfo.Available = fields[3]

	// Field 4 is "Use%"
	usageStr := strings.TrimSuffix(fields[4], "%")
	diskInfo.UsagePercent, _ = strconv.ParseFloat(usageStr, 64)

	return diskInfo
}

// ============================================================================
// CLOSING
// ============================================================================
// Function-based library - no execution needed
