//omni:code --go -library
//omni:key B-L3-statemachine-display
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
// Package: statemachine/display
// Purpose: Display helpers for state machine visualization
// Biblical: "Look that thou make them after their pattern" - Exodus 25:40
//
// This file provides display functions for trajectory and state machine
// visualization. These are universal helpers usable by any CPI-SI tool,
// not just statusline.

package statemachine

// ============================================================================
// SETUP
// ============================================================================

// ANSI color codes (defined here to avoid import cycle with pkg/display)
const (
	colorReset  = "\033[0m"
	colorRed    = "\033[31m"
	colorGreen  = "\033[32m"
	colorYellow = "\033[33m"
	colorGray   = "\033[90m"
)

// TrajectoryInfo contains display information for a trajectory section
type TrajectoryInfo struct {
	Section string // B.1, B.2, B.3, B.4
	Symbol  string // →→, →|, |→, ||
	State   string // BUILDING, PIVOTING, GROUNDING
	Color   string // ANSI color code
	HasInfo bool   // Whether trajectory info is available
}

// ============================================================================
// BODY
// ============================================================================

// --- Trajectory Display Helpers ---

// TrajectoryDisplay returns the visual symbol for a trajectory section
//
//	B.1 → →→ (Building: proceeding forward)
//	B.2 → →| (Pivoting out: build → verify)
//	B.3 → |→ (Pivoting in: verify → rebuild)
//	B.4 → || (Grounding: halted)
func TrajectoryDisplay(section string) string {
	switch section {
	case "B.1":
		return "→→"
	case "B.2":
		return "→|"
	case "B.3":
		return "|→"
	case "B.4":
		return "||"
	default:
		return ""
	}
}

// TrajectoryState returns the state name for a trajectory section
func TrajectoryState(section string) string {
	switch section {
	case "B.1":
		return "BUILDING"
	case "B.2", "B.3":
		return "PIVOTING"
	case "B.4":
		return "GROUNDING"
	default:
		return "UNKNOWN"
	}
}

// TrajectoryColor returns the ANSI color for a trajectory section
func TrajectoryColor(section string) string {
	switch section {
	case "B.1":
		return colorGreen // Building = proceeding
	case "B.2", "B.3":
		return colorYellow // Pivoting = transitioning
	case "B.4":
		return colorRed // Grounding = halted
	default:
		return colorGray
	}
}

// GetTrajectoryInfo returns complete trajectory display information
func GetTrajectoryInfo(section string) TrajectoryInfo {
	if section == "" {
		return TrajectoryInfo{HasInfo: false}
	}
	return TrajectoryInfo{
		Section: section,
		Symbol:  TrajectoryDisplay(section),
		State:   TrajectoryState(section),
		Color:   TrajectoryColor(section),
		HasInfo: true,
	}
}

// --- Runtime State Display Helpers ---

// SectionFromRuntime returns the trajectory section from runtime state
// Falls back to "B.1" (building) if not available
func SectionFromRuntime(rt *RuntimeState) string {
	if rt != nil && rt.TrajectorySection != "" {
		return rt.TrajectorySection
	}
	return "B.1"
}

// GetRuntimeTrajectoryInfo returns trajectory info from runtime state
func GetRuntimeTrajectoryInfo(rt *RuntimeState) TrajectoryInfo {
	return GetTrajectoryInfo(SectionFromRuntime(rt))
}

// --- Anchor Display Helpers ---

// AnchorDisplay returns formatted anchor string with optional icon
func AnchorDisplay(anchor string, includeIcon bool) string {
	if anchor == "" {
		anchor = "Genesis 1:1"
	}
	if includeIcon {
		return "⚓ " + anchor
	}
	return anchor
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: pkg/display/command.go (command colors)
//          statusline/lib/sections/ (section builders)
