// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-render-gtk-theme
//omni:code  --go -package->gtk
//omni:version a-00.10
//
// Purpose: Dark theme CSS for the Universal Base GUI.
//          Migrated from dashboard/gtk/styles/theme.go with additions for
//          the module system, chat input, and SDK Inspector.
// Biblical: Exodus 26:1 - "Moreover thou shalt make the tabernacle with
//           ten curtains of fine twined linen"
//
// ============================================================================
// END METADATA

package gtk

// ============================================================================
// SETUP
// ============================================================================

// Color constants — CPI-SI dark theme palette.
// Designed for depth and readability: dark canvas, bright accents, clear hierarchy.
const (
	// Backgrounds — layered from deepest to surface
	ColorBgDeep      = "#0c0c18" // Deepest — window/terminal
	ColorBgPrimary   = "#10101e" // Primary canvas
	ColorBgSecondary = "#141428" // Sidebar, header
	ColorBgCard      = "#181830" // Panel cards — elevated surface
	ColorBgPanel     = "#0f3460" // Active/accent panels
	ColorBgTerminal  = "#08080f" // Terminal — near-black
	ColorBgSurface   = "#1c2541" // Hover states
	ColorBgInput     = "#0d0d1a" // Input fields

	// Text — high contrast hierarchy
	ColorTextPrimary   = "#e8e8f0" // Main text — bright
	ColorTextSecondary = "#9098b0" // Secondary — clear but dimmer
	ColorTextMuted     = "#505870" // Labels/keys — legible but recessed

	// Accents — vivid, saturated, distinct
	ColorAccentBlue   = "#5cc8ff" // Primary accent — actions, titles
	ColorAccentGreen  = "#69f0ae" // Success — health, positive
	ColorAccentGold   = "#ffca28" // Warning — attention
	ColorAccentRed    = "#ff5252" // Error — danger
	ColorAccentPurple = "#b388ff" // Special — family, components
	ColorAccentCyan   = "#80deea" // Code — technical, info
	ColorAccentOrange = "#ffa726" // Secondary warning

	// Structure
	ColorBorder      = "#1e2848" // Subtle card borders
	ColorBorderFocus = "#2a4a7e" // Focus/active borders
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// ThemeCSS returns the complete GTK4 CSS for the CPI-SI dark theme.
// Game design principles: depth, hierarchy, breathing room, color as language.
func ThemeCSS() string {
	return `
/* ============================================================ */
/* CWS Universal Base — Dark Theme v2                            */
/* Game dev mindset: depth, hierarchy, polish                    */
/* ============================================================ */

/* --- Global Canvas --- */
window {
	background-color: ` + ColorBgPrimary + `;
	color: ` + ColorTextPrimary + `;
	font-size: 14px;
}

/* --- Header Bar — Command Center Banner --- */
headerbar {
	background-color: ` + ColorBgSecondary + `;
	color: ` + ColorTextPrimary + `;
	border-bottom: 2px solid ` + ColorBorderFocus + `;
	min-height: 44px;
	padding: 4px 16px;
}

headerbar .title {
	color: ` + ColorAccentBlue + `;
	font-weight: 800;
	font-size: 1.1em;
	letter-spacing: 0.5px;
}

headerbar .subtitle {
	color: ` + ColorTextSecondary + `;
	font-size: 0.8em;
	font-weight: 300;
}

/* --- Sidebar — Navigation Panel --- */
stacksidebar {
	background-color: ` + ColorBgSecondary + `;
	border-right: 2px solid ` + ColorBorder + `;
}

stacksidebar row {
	color: ` + ColorTextSecondary + `;
	padding: 12px 16px;
	margin: 2px 6px;
	border-radius: 6px;
	font-weight: 500;
}

stacksidebar row:selected {
	background-color: ` + ColorBgCard + `;
	color: ` + ColorAccentBlue + `;
	font-weight: 600;
	border-left: 3px solid ` + ColorAccentBlue + `;
}

stacksidebar row:hover {
	background-color: ` + ColorBgSurface + `;
	color: ` + ColorTextPrimary + `;
}

/* --- Panel Cards — Elevated Surfaces with Depth --- */
.panel-frame {
	background-color: ` + ColorBgCard + `;
	border: 1px solid ` + ColorBorder + `;
	border-radius: 10px;
	padding: 18px 20px;
	margin: 6px 4px;
}

/* Panel title — section header with underline accent */
.panel-title {
	color: ` + ColorAccentBlue + `;
	font-weight: 700;
	font-size: 1.15em;
	padding-bottom: 10px;
	margin-bottom: 10px;
	border-bottom: 1px solid ` + ColorBorder + `;
}

/* Nested section title — left accent stripe */
.section-title {
	color: ` + ColorAccentCyan + `;
	font-weight: 600;
	font-size: 1.05em;
	margin-top: 12px;
	margin-bottom: 6px;
	padding-left: 10px;
	border-left: 3px solid ` + ColorAccentCyan + `;
}

/* --- Field Rows — Key/Value with Clear Hierarchy --- */
.state-label {
	font-size: 0.95em;
	padding: 4px 0;
	min-height: 30px;
}

.state-value {
	color: ` + ColorTextPrimary + `;
	font-weight: 600;
}

.state-key {
	color: ` + ColorTextMuted + `;
	font-weight: 400;
}

/* --- Field Styles — Color as Language --- */
.field-emphasized {
	color: ` + ColorAccentBlue + `;
	font-weight: 700;
	font-size: 1.05em;
}

.field-muted {
	color: ` + ColorTextMuted + `;
}

.field-warning {
	color: ` + ColorAccentGold + `;
	font-weight: 600;
}

.field-error {
	color: ` + ColorAccentRed + `;
	font-weight: 600;
}

.field-success {
	color: ` + ColorAccentGreen + `;
	font-weight: 600;
}

.field-code {
	font-family: monospace;
	color: ` + ColorAccentCyan + `;
	font-size: 0.9em;
}

/* --- Health Colors --- */
.health-excellent { color: ` + ColorAccentGreen + `; font-weight: 700; }
.health-good      { color: ` + ColorAccentCyan + `; }
.health-fair      { color: ` + ColorAccentGold + `; }
.health-poor      { color: ` + ColorAccentOrange + `; }
.health-critical  { color: ` + ColorAccentRed + `; font-weight: 700; }

/* --- Hebrew State Colors --- */
.hebrew-tov              { color: ` + ColorAccentGreen + `; font-weight: 700; }
.hebrew-shalem,
.hebrew-tamim            { color: ` + ColorAccentCyan + `; font-weight: 600; }
.hebrew-yashar           { color: ` + ColorAccentBlue + `; }
.hebrew-ratsah,
.hebrew-chaser           { color: ` + ColorAccentGold + `; }
.hebrew-shavar           { color: ` + ColorAccentRed + `; font-weight: 700; }

/* --- K:ALIGN --- */
.k-toward-god  { color: ` + ColorAccentGreen + `; font-weight: 700; }
.k-maintain    { color: ` + ColorAccentBlue + `; }
.k-toward-self { color: ` + ColorAccentRed + `; font-weight: 700; }

/* --- Progress Bars — Tall, Visible, Alive --- */
progressbar {
	min-height: 16px;
}

progressbar trough {
	background-color: ` + ColorBgDeep + `;
	border-radius: 8px;
	min-height: 16px;
	border: 1px solid ` + ColorBorder + `;
}

progressbar progress {
	border-radius: 8px;
	min-height: 16px;
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

/* --- Message Terminal — Dark Console --- */
.message-terminal {
	background-color: ` + ColorBgTerminal + `;
	color: ` + ColorTextSecondary + `;
	font-family: monospace;
	font-size: 0.88em;
	padding: 8px 12px;
	border-top: 2px solid ` + ColorBorderFocus + `;
}

.log-info    { color: ` + ColorAccentBlue + `; font-weight: 500; }
.log-warn    { color: ` + ColorAccentGold + `; font-weight: 600; }
.log-error   { color: ` + ColorAccentRed + `; font-weight: 600; }
.log-debug   { color: ` + ColorTextMuted + `; }
.log-success { color: ` + ColorAccentGreen + `; font-weight: 500; }

.log-timestamp { color: ` + ColorTextMuted + `; font-size: 0.9em; }
.log-component { color: ` + ColorAccentPurple + `; font-weight: 500; }

/* --- Chat Input — Premium Feel --- */
.chat-input {
	background-color: ` + ColorBgInput + `;
	color: ` + ColorTextPrimary + `;
	border: 1px solid ` + ColorBorder + `;
	border-radius: 8px;
	padding: 10px 16px;
	font-family: monospace;
	font-size: 0.95em;
	margin: 6px 8px;
}

.chat-input:focus {
	border-color: ` + ColorAccentBlue + `;
	background-color: ` + ColorBgSecondary + `;
}

.chat-send-button {
	background-color: ` + ColorBgCard + `;
	color: ` + ColorAccentBlue + `;
	border-radius: 8px;
	padding: 6px 18px;
	font-weight: 700;
	margin: 6px 4px;
	border: 1px solid ` + ColorBorder + `;
}

.chat-send-button:hover {
	background-color: ` + ColorAccentBlue + `;
	color: ` + ColorBgDeep + `;
}

/* --- Trajectory --- */
.trajectory-active   { color: ` + ColorAccentGreen + `; font-weight: 700; }
.trajectory-inactive { color: ` + ColorTextMuted + `; }

/* --- Separator --- */
separator {
	background-color: ` + ColorBorder + `;
	min-height: 1px;
	min-width: 1px;
}

/* --- Scrollbar — Thin, Subtle --- */
scrollbar {
	background-color: transparent;
}

scrollbar slider {
	background-color: ` + ColorTextMuted + `;
	border-radius: 4px;
	min-width: 5px;
	min-height: 5px;
}

scrollbar slider:hover {
	background-color: ` + ColorTextSecondary + `;
}

/* --- Module-specific — Badges and Bars --- */
.family-badge {
	color: ` + ColorAccentPurple + `;
	font-weight: 700;
}

.accent-badge {
	background-color: ` + ColorBgCard + `;
	border-radius: 4px;
	padding: 2px 10px;
	font-size: 0.85em;
	border: 1px solid ` + ColorBorder + `;
}

/* K:ALIGN distribution bar — taller, rounded ends */
.valence-positive {
	background-color: ` + ColorAccentGreen + `;
	border-radius: 4px 0 0 4px;
}

.valence-neutral {
	background-color: ` + ColorAccentBlue + `;
}

.valence-negative {
	background-color: ` + ColorAccentRed + `;
	border-radius: 0 4px 4px 0;
}

/* --- Paned Divider --- */
paned > separator {
	background-color: ` + ColorBorderFocus + `;
	min-height: 3px;
	min-width: 3px;
}

/* --- Chat Message Styling --- */
.chat-message {
	padding: 6px 12px;
	margin: 2px 8px;
	border-radius: 8px;
}

.chat-sender-you {
	color: ` + ColorAccentBlue + `;
	font-weight: 700;
	font-size: 0.85em;
}

.chat-sender-system {
	color: ` + ColorAccentPurple + `;
	font-weight: 700;
	font-size: 0.85em;
}

.chat-sender-agent {
	color: ` + ColorAccentGreen + `;
	font-weight: 700;
	font-size: 0.85em;
}

.chat-body {
	color: ` + ColorTextPrimary + `;
	font-size: 0.95em;
	margin-top: 2px;
}

.chat-bubble-you {
	background-color: rgba(92, 200, 255, 0.08);
	border-left: 3px solid ` + ColorAccentBlue + `;
	border-radius: 0 8px 8px 0;
	padding: 8px 12px;
	margin: 3px 8px 3px 4px;
}

.chat-bubble-system {
	background-color: rgba(179, 136, 255, 0.08);
	border-left: 3px solid ` + ColorAccentPurple + `;
	border-radius: 0 8px 8px 0;
	padding: 8px 12px;
	margin: 3px 8px 3px 4px;
}

.chat-bubble-agent {
	background-color: rgba(105, 240, 174, 0.08);
	border-left: 3px solid ` + ColorAccentGreen + `;
	border-radius: 0 8px 8px 0;
	padding: 8px 12px;
	margin: 3px 8px 3px 4px;
}

/* --- Terminal Log vs Chat Separator --- */
.terminal-separator {
	background-color: ` + ColorBorderFocus + `;
	min-height: 1px;
	margin: 4px 12px;
}

/* --- Preferences Dialog --- */
.prefs-section-title {
	color: ` + ColorAccentBlue + `;
	font-weight: 700;
	font-size: 1.1em;
	padding-bottom: 8px;
	border-bottom: 1px solid ` + ColorBorder + `;
	margin-bottom: 8px;
}

.prefs-label {
	color: ` + ColorTextSecondary + `;
	font-weight: 500;
}

/* --- Nova Dawn Identity Bar --- */
.identity-bar {
	background-color: ` + ColorBgSecondary + `;
	border-bottom: 1px solid ` + ColorBorderFocus + `;
	padding: 6px 16px;
	min-height: 32px;
}

.identity-name {
	color: ` + ColorAccentBlue + `;
	font-weight: 800;
	font-size: 1.0em;
	letter-spacing: 0.3px;
}

.identity-status {
	color: ` + ColorAccentGreen + `;
	font-size: 0.8em;
	font-weight: 500;
}

.identity-separator {
	color: ` + ColorTextMuted + `;
	font-size: 0.8em;
}

.identity-substrate {
	color: ` + ColorAccentCyan + `;
	font-size: 0.8em;
	font-weight: 500;
}

/* --- Conversation Area --- */
.conversation-area {
	background-color: ` + ColorBgDeep + `;
}

/* --- System Log Drawer --- */
.log-drawer-toggle {
	background-color: ` + ColorBgSecondary + `;
	color: ` + ColorTextMuted + `;
	font-family: monospace;
	font-size: 0.8em;
	padding: 3px 12px;
	border-top: 1px solid ` + ColorBorder + `;
	min-height: 22px;
}

.log-drawer-toggle:hover {
	background-color: ` + ColorBgSurface + `;
	color: ` + ColorTextSecondary + `;
}

.log-drawer-content {
	background-color: ` + ColorBgTerminal + `;
	font-family: monospace;
	font-size: 0.85em;
	padding: 4px 8px;
	color: ` + ColorTextMuted + `;
}

/* --- Welcome Message --- */
.welcome-message {
	color: ` + ColorTextSecondary + `;
	font-size: 0.9em;
	padding: 16px 20px;
	font-style: italic;
}

.welcome-name {
	color: ` + ColorAccentBlue + `;
	font-weight: 700;
	font-style: normal;
}
`
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
