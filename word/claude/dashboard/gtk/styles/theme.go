// ============================================================================
// METADATA - CPI-SI Dashboard GTK4 Theme
// ============================================================================
//
// Purpose: Dark theme CSS and color constants matching web dashboard
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ============================================================================

package styles

// ============================================================================
// SETUP
// ============================================================================

// Color constants matching web dashboard palette.
const (
	ColorBgPrimary   = "#1a1a2e"
	ColorBgSecondary = "#16213e"
	ColorBgPanel     = "#0f3460"
	ColorBgTerminal  = "#0d1117"
	ColorBgSurface   = "#1c2541"

	ColorTextPrimary   = "#e0e0e0"
	ColorTextSecondary = "#a0a0a0"
	ColorTextMuted     = "#6a7080"

	ColorAccentBlue   = "#4fc3f7"
	ColorAccentGreen  = "#66bb6a"
	ColorAccentGold   = "#ffd54f"
	ColorAccentRed    = "#ef5350"
	ColorAccentPurple = "#ce93d8"
	ColorAccentCyan   = "#26c6da"
	ColorAccentOrange = "#ffa726"

	ColorBorder = "#2a3a5e"
)

// ============================================================================
// BODY
// ============================================================================

// DashboardCSS returns the complete GTK4 CSS for the dark dashboard theme.
func DashboardCSS() string {
	return `
/* ============================================================ */
/* CPI-SI Dashboard GTK4 — Dark Theme                           */
/* Matches web dashboard palette for visual consistency          */
/* ============================================================ */

/* --- Global --- */
window {
	background-color: ` + ColorBgPrimary + `;
	color: ` + ColorTextPrimary + `;
}

/* --- Header Bar --- */
headerbar {
	background-color: ` + ColorBgSecondary + `;
	color: ` + ColorTextPrimary + `;
	border-bottom: 1px solid ` + ColorBorder + `;
	min-height: 36px;
}

headerbar .title {
	color: ` + ColorAccentBlue + `;
	font-weight: bold;
}

headerbar .subtitle {
	color: ` + ColorTextSecondary + `;
	font-size: 0.85em;
}

/* --- Sidebar --- */
stacksidebar {
	background-color: ` + ColorBgSecondary + `;
	border-right: 1px solid ` + ColorBorder + `;
}

stacksidebar row {
	color: ` + ColorTextSecondary + `;
	padding: 8px 12px;
	border-radius: 0;
}

stacksidebar row:selected {
	background-color: ` + ColorBgPanel + `;
	color: ` + ColorAccentBlue + `;
}

stacksidebar row:hover {
	background-color: ` + ColorBgSurface + `;
}

/* --- Panels --- */
.panel-frame {
	background-color: ` + ColorBgSecondary + `;
	border: 1px solid ` + ColorBorder + `;
	border-radius: 6px;
	padding: 12px;
	margin: 4px;
}

.panel-title {
	color: ` + ColorAccentBlue + `;
	font-weight: bold;
	font-size: 1.1em;
	margin-bottom: 8px;
}

/* --- State Labels --- */
.state-label {
	font-size: 0.95em;
	margin: 2px 0;
}

.state-value {
	color: ` + ColorTextPrimary + `;
	font-weight: bold;
}

.state-key {
	color: ` + ColorTextMuted + `;
}

/* --- Health Colors --- */
.health-excellent {
	color: ` + ColorAccentGreen + `;
}

.health-good {
	color: ` + ColorAccentCyan + `;
}

.health-fair {
	color: ` + ColorAccentGold + `;
}

.health-poor {
	color: ` + ColorAccentOrange + `;
}

.health-critical {
	color: ` + ColorAccentRed + `;
}

/* --- Hebrew State Colors --- */
.hebrew-tov {
	color: ` + ColorAccentGreen + `;
	font-weight: bold;
}

.hebrew-shalem, .hebrew-tamim {
	color: ` + ColorAccentCyan + `;
}

.hebrew-yashar {
	color: ` + ColorAccentBlue + `;
}

.hebrew-ratsah, .hebrew-chaser {
	color: ` + ColorAccentGold + `;
}

.hebrew-shavar {
	color: ` + ColorAccentRed + `;
	font-weight: bold;
}

/* --- K:ALIGN --- */
.k-toward-god {
	color: ` + ColorAccentGreen + `;
	font-weight: bold;
}

.k-maintain {
	color: ` + ColorAccentBlue + `;
}

.k-toward-self {
	color: ` + ColorAccentRed + `;
	font-weight: bold;
}

/* --- Progress Bars --- */
progressbar {
	min-height: 8px;
}

progressbar trough {
	background-color: ` + ColorBgSurface + `;
	border-radius: 4px;
	min-height: 8px;
}

progressbar progress {
	border-radius: 4px;
	min-height: 8px;
}

.progress-health progress {
	background-color: ` + ColorAccentGreen + `;
}

.progress-cpi progress {
	background-color: ` + ColorAccentBlue + `;
}

.progress-tokens progress {
	background-color: ` + ColorAccentCyan + `;
}

/* --- Message Terminal --- */
.message-terminal {
	background-color: ` + ColorBgTerminal + `;
	color: ` + ColorTextPrimary + `;
	font-family: monospace;
	font-size: 0.9em;
	padding: 8px;
	border-top: 1px solid ` + ColorBorder + `;
}

.log-info {
	color: ` + ColorAccentBlue + `;
}

.log-warn {
	color: ` + ColorAccentGold + `;
}

.log-error {
	color: ` + ColorAccentRed + `;
}

.log-debug {
	color: ` + ColorTextMuted + `;
}

.log-timestamp {
	color: ` + ColorTextMuted + `;
}

.log-component {
	color: ` + ColorAccentPurple + `;
}

/* --- Trajectory --- */
.trajectory-active {
	color: ` + ColorAccentGreen + `;
	font-weight: bold;
}

.trajectory-inactive {
	color: ` + ColorTextMuted + `;
}

/* --- Separator --- */
separator {
	background-color: ` + ColorBorder + `;
	min-height: 1px;
	min-width: 1px;
}

/* --- Scrollbar --- */
scrollbar {
	background-color: transparent;
}

scrollbar slider {
	background-color: ` + ColorTextMuted + `;
	border-radius: 4px;
	min-width: 6px;
	min-height: 6px;
}

scrollbar slider:hover {
	background-color: ` + ColorTextSecondary + `;
}

/* --- Family Member Badge --- */
.family-badge {
	color: ` + ColorAccentPurple + `;
	font-weight: bold;
}

/* --- Accent badge --- */
.accent-badge {
	background-color: ` + ColorBgPanel + `;
	border-radius: 4px;
	padding: 2px 8px;
	font-size: 0.85em;
}
`
}

// ============================================================================
// CLOSING
// ============================================================================
