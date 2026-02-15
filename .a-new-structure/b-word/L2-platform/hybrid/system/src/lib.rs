//omni:code --rust -library
//omni:key B-L2-system
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L2-system
// Purpose: Shared system health utilities — load, memory, disk
// Biblical: Proverbs 27:23 — "Be thou diligent to know the state"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// Used by: statusline, hooks
//
// Reads /proc/loadavg and /proc/meminfo for system metrics.
// Disk monitoring uses /proc/mounts + statvfs-style parsing from /proc.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// S.1 Imports
// ────────────────────────────────────────────────────────────────

use std::fs;

// ────────────────────────────────────────────────────────────────
// S.2 Configuration
// ────────────────────────────────────────────────────────────────

/// System monitoring thresholds and icons, injectable from database.
///
/// Three metrics, each with yellow/red thresholds and an icon:
/// - Load: 1-minute average from /proc/loadavg
/// - Memory: percent used from /proc/meminfo
/// - Disk: percent used from df
#[derive(Debug, Clone)]
pub struct Config {
    pub load_yellow: f64,
    pub load_red: f64,
    pub load_icon: String,
    pub mem_yellow: f64,
    pub mem_red: f64,
    pub mem_icon: String,
    pub disk_yellow: f64,
    pub disk_red: f64,
    pub disk_icon: String,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            load_yellow: 2.0,
            load_red: 4.0,
            load_icon: "\u{1f4ca}".into(), // chart emoji
            mem_yellow: 70.0,
            mem_red: 90.0,
            mem_icon: "\u{1f9e0}".into(), // brain emoji
            disk_yellow: 80.0,
            disk_red: 95.0,
            disk_icon: "\u{1f4be}".into(), // floppy emoji
        }
    }
}

/// Package-level configuration. Set via `init()`, falls back to defaults.
static CFG: std::sync::OnceLock<Config> = std::sync::OnceLock::new();

/// Initializes the package configuration. Call once at startup.
/// Pass `None` to use hardcoded defaults.
pub fn init(cfg: Option<Config>) {
    let _ = CFG.set(cfg.unwrap_or_default());
}

/// Returns a reference to the active configuration.
fn cfg() -> &'static Config {
    CFG.get_or_init(Config::default)
}

// ────────────────────────────────────────────────────────────────
// S.3 Core Types
// ────────────────────────────────────────────────────────────────

/// System load averages from /proc/loadavg.
#[derive(Debug, Clone)]
pub struct Load {
    pub avg1: f64,
    pub avg5: f64,
    pub avg15: f64,
    pub has_info: bool,
}

/// Formatted load display for statusline rendering.
#[derive(Debug, Clone)]
pub struct LoadDisplay {
    pub load_avg: f64,
    pub icon: String,
    pub color: &'static str,
    pub has_info: bool,
}

/// Memory usage from /proc/meminfo.
#[derive(Debug, Clone)]
pub struct Memory {
    pub total_kb: i64,
    pub available_kb: i64,
    pub used_kb: i64,
    pub percent: f64,
    pub has_info: bool,
}

/// Formatted memory display for statusline rendering.
#[derive(Debug, Clone)]
pub struct MemoryDisplay {
    pub used_gb: f64,
    pub total_gb: f64,
    pub percent: f64,
    pub icon: String,
    pub color: &'static str,
    pub has_info: bool,
}

/// Disk usage for a given path.
#[derive(Debug, Clone)]
pub struct Disk {
    pub total_bytes: u64,
    pub free_bytes: u64,
    pub used_bytes: u64,
    pub percent: f64,
    pub has_info: bool,
}

/// Formatted disk display for statusline rendering.
#[derive(Debug, Clone)]
pub struct DiskDisplay {
    pub percent: f64,
    pub icon: String,
    pub color: &'static str,
    pub has_info: bool,
}

// ============================================================================
// BODY
// ============================================================================

// --- Load ---

/// Returns system load averages by reading /proc/loadavg.
pub fn get_load() -> Load {
    let Ok(data) = fs::read_to_string("/proc/loadavg") else {
        return Load { avg1: 0.0, avg5: 0.0, avg15: 0.0, has_info: false };
    };

    let parts: Vec<&str> = data.split_whitespace().collect();
    if parts.len() < 3 {
        return Load { avg1: 0.0, avg5: 0.0, avg15: 0.0, has_info: false };
    }

    Load {
        avg1: parts[0].parse().unwrap_or(0.0),
        avg5: parts[1].parse().unwrap_or(0.0),
        avg15: parts[2].parse().unwrap_or(0.0),
        has_info: true,
    }
}

/// Returns formatted load display with icon and color.
///
/// Color thresholds:
/// - Green: load < 2.0
/// - Yellow: load 2.0..4.0
/// - Red: load > 4.0
pub fn get_load_display() -> LoadDisplay {
    let load = get_load();
    if !load.has_info {
        return LoadDisplay {
            load_avg: 0.0,
            icon: String::new(),
            color: "",
            has_info: false,
        };
    }

    let c = cfg();
    let color = if load.avg1 > c.load_red {
        bereshit_l2_display::RED
    } else if load.avg1 > c.load_yellow {
        bereshit_l2_display::YELLOW
    } else {
        bereshit_l2_display::GREEN
    };

    LoadDisplay {
        load_avg: load.avg1,
        icon: c.load_icon.clone(),
        color,
        has_info: true,
    }
}

// --- Memory ---

/// Returns memory usage by reading /proc/meminfo.
pub fn get_memory() -> Memory {
    let Ok(data) = fs::read_to_string("/proc/meminfo") else {
        return Memory {
            total_kb: 0, available_kb: 0, used_kb: 0,
            percent: 0.0, has_info: false,
        };
    };

    let mut total: i64 = 0;
    let mut available: i64 = 0;

    for line in data.lines() {
        if let Some(rest) = line.strip_prefix("MemTotal:") {
            total = parse_kb(rest);
        } else if let Some(rest) = line.strip_prefix("MemAvailable:") {
            available = parse_kb(rest);
        }
    }

    if total == 0 {
        return Memory {
            total_kb: 0, available_kb: 0, used_kb: 0,
            percent: 0.0, has_info: false,
        };
    }

    let used = total - available;
    let percent = used as f64 / total as f64 * 100.0;

    Memory {
        total_kb: total,
        available_kb: available,
        used_kb: used,
        percent,
        has_info: true,
    }
}

/// Returns formatted memory display with icon and color.
///
/// Color thresholds:
/// - Green: usage < 70%
/// - Yellow: usage 70..90%
/// - Red: usage > 90%
pub fn get_memory_display() -> MemoryDisplay {
    let mem = get_memory();
    if !mem.has_info {
        return MemoryDisplay {
            used_gb: 0.0, total_gb: 0.0, percent: 0.0,
            icon: String::new(), color: "", has_info: false,
        };
    }

    let c = cfg();
    let color = if mem.percent > c.mem_red {
        bereshit_l2_display::RED
    } else if mem.percent > c.mem_yellow {
        bereshit_l2_display::YELLOW
    } else {
        bereshit_l2_display::GREEN
    };

    MemoryDisplay {
        used_gb: mem.used_kb as f64 / 1024.0 / 1024.0,
        total_gb: mem.total_kb as f64 / 1024.0 / 1024.0,
        percent: mem.percent,
        icon: c.mem_icon.clone(),
        color,
        has_info: true,
    }
}

// --- Disk ---

/// Returns disk usage for the given path by reading /proc/mounts and
/// calculating from filesystem stats.
///
/// Uses `libc::statvfs` via a safe wrapper. If the path is empty,
/// defaults to "/".
pub fn get_disk(path: &str) -> Disk {
    let target = if path.is_empty() { "/" } else { path };

    // Read /proc/diskstats is complex; for now use a simpler approach
    // by shelling out to `df`. This avoids libc/unsafe entirely.
    let output = std::process::Command::new("df")
        .args(["--block-size=1", "--output=size,avail", target])
        .output();

    let Ok(output) = output else {
        return Disk {
            total_bytes: 0, free_bytes: 0, used_bytes: 0,
            percent: 0.0, has_info: false,
        };
    };

    if !output.status.success() {
        return Disk {
            total_bytes: 0, free_bytes: 0, used_bytes: 0,
            percent: 0.0, has_info: false,
        };
    }

    let text = String::from_utf8_lossy(&output.stdout);
    let lines: Vec<&str> = text.lines().collect();
    if lines.len() < 2 {
        return Disk {
            total_bytes: 0, free_bytes: 0, used_bytes: 0,
            percent: 0.0, has_info: false,
        };
    }

    let parts: Vec<&str> = lines[1].split_whitespace().collect();
    if parts.len() < 2 {
        return Disk {
            total_bytes: 0, free_bytes: 0, used_bytes: 0,
            percent: 0.0, has_info: false,
        };
    }

    let total: u64 = parts[0].parse().unwrap_or(0);
    let free: u64 = parts[1].parse().unwrap_or(0);

    if total == 0 {
        return Disk {
            total_bytes: 0, free_bytes: 0, used_bytes: 0,
            percent: 0.0, has_info: false,
        };
    }

    let used = total - free;
    let percent = used as f64 / total as f64 * 100.0;

    Disk {
        total_bytes: total,
        free_bytes: free,
        used_bytes: used,
        percent,
        has_info: true,
    }
}

/// Returns formatted disk display with icon and color.
///
/// Color thresholds:
/// - Green: usage < 80%
/// - Yellow: usage 80..95%
/// - Red: usage > 95%
pub fn get_disk_display(path: &str) -> DiskDisplay {
    let disk = get_disk(path);
    if !disk.has_info {
        return DiskDisplay {
            percent: 0.0, icon: String::new(), color: "", has_info: false,
        };
    }

    let c = cfg();
    let color = if disk.percent > c.disk_red {
        bereshit_l2_display::RED
    } else if disk.percent > c.disk_yellow {
        bereshit_l2_display::YELLOW
    } else {
        bereshit_l2_display::GREEN
    };

    DiskDisplay {
        percent: disk.percent,
        icon: c.disk_icon.clone(),
        color,
        has_info: true,
    }
}

// --- Helpers ---

/// Parses a kB value from a /proc/meminfo field like "  12345 kB".
fn parse_kb(field: &str) -> i64 {
    field
        .split_whitespace()
        .next()
        .and_then(|s| s.parse().ok())
        .unwrap_or(0)
}

// ────────────────────────────────────────────────────────────────
// Registry Integration
// ────────────────────────────────────────────────────────────────

/// Builds a `Config` from cross-layer database queries via the registry.
///
/// Queries L2 platform `system_monitoring` for load/memory/disk thresholds
/// and icons. Falls back to defaults for any metric that fails.
pub fn config_from_registry(r: &bereshit_l2_registry::Registry) -> Config {
    let mut c = Config::default();

    // Load thresholds
    if let Ok(rows) = r.platform.monitoring_by_metric("load") {
        for m in &rows {
            match m.level.as_str() {
                "yellow" => {
                    c.load_yellow = m.threshold;
                    if !m.icon.is_empty() {
                        c.load_icon = m.icon.clone();
                    }
                }
                "red" => {
                    c.load_red = m.threshold;
                }
                _ => {}
            }
        }
    }

    // Memory thresholds
    if let Ok(rows) = r.platform.monitoring_by_metric("memory") {
        for m in &rows {
            match m.level.as_str() {
                "yellow" => {
                    c.mem_yellow = m.threshold;
                    if !m.icon.is_empty() {
                        c.mem_icon = m.icon.clone();
                    }
                }
                "red" => {
                    c.mem_red = m.threshold;
                }
                _ => {}
            }
        }
    }

    // Disk thresholds
    if let Ok(rows) = r.platform.monitoring_by_metric("disk") {
        for m in &rows {
            match m.level.as_str() {
                "yellow" => {
                    c.disk_yellow = m.threshold;
                    if !m.icon.is_empty() {
                        c.disk_icon = m.icon.clone();
                    }
                }
                "red" => {
                    c.disk_red = m.threshold;
                }
                _ => {}
            }
        }
    }

    c
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parse_kb_extracts_number() {
        assert_eq!(parse_kb("  16384000 kB"), 16384000);
        assert_eq!(parse_kb("  0 kB"), 0);
        assert_eq!(parse_kb(""), 0);
    }

    #[test]
    fn load_returns_struct() {
        // On Linux this reads /proc/loadavg; on non-Linux, has_info = false
        let load = get_load();
        if load.has_info {
            assert!(load.avg1 >= 0.0);
            assert!(load.avg5 >= 0.0);
            assert!(load.avg15 >= 0.0);
        }
    }

    #[test]
    fn memory_returns_struct() {
        let mem = get_memory();
        if mem.has_info {
            assert!(mem.total_kb > 0);
            assert!(mem.percent >= 0.0);
            assert!(mem.percent <= 100.0);
        }
    }

    #[test]
    fn config_default_thresholds() {
        let c = Config::default();
        assert_eq!(c.load_yellow, 2.0);
        assert_eq!(c.load_red, 4.0);
        assert_eq!(c.mem_yellow, 70.0);
        assert_eq!(c.mem_red, 90.0);
        assert_eq!(c.disk_yellow, 80.0);
        assert_eq!(c.disk_red, 95.0);
    }

    #[test]
    fn config_default_icons() {
        let c = Config::default();
        assert!(!c.load_icon.is_empty());
        assert!(!c.mem_icon.is_empty());
        assert!(!c.disk_icon.is_empty());
    }

    #[test]
    fn cfg_returns_defaults() {
        let c = cfg();
        assert_eq!(c.load_yellow, 2.0);
        assert_eq!(c.load_red, 4.0);
    }

    #[test]
    fn load_display_has_icon() {
        let d = get_load_display();
        if d.has_info {
            assert!(!d.icon.is_empty());
        }
    }
}
