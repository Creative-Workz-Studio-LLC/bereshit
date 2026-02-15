//omni:code --rust -library
//omni:key B-L2-wezterm
//omni:version a-01.00

// =============================================================================
// METADATA
// =============================================================================
//
// Key: B-L2-wezterm
// Purpose: WezTerm OSC sequence emission for terminal integration
// Biblical: Proverbs 25:11 — "A word fitly spoken is like apples of gold"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// OSC Sequences Supported:
//   OSC 0   - Set both titles
//   OSC 1   - Set tab title (icon name)
//   OSC 2   - Set window title
//   OSC 7   - Set current working directory
//   OSC 9   - iTerm2 notification
//   OSC 52  - Clipboard manipulation
//   OSC 777 - rxvt notification
//   OSC 1337 - User variables (iTerm2 protocol)
//
// Reference: https://wezterm.org/escape-sequences.html
//
// =============================================================================

// =============================================================================
// SETUP
// =============================================================================

use base64::Engine;
use base64::engine::general_purpose::STANDARD as BASE64;
use std::collections::HashMap;
use std::io::Write;
use std::{env, fs, path::PathBuf};

/// Escape character.
pub const ESC: &str = "\x1b";

/// Bell (OSC terminator).
pub const BEL: &str = "\x07";

/// String Terminator (ESC \).
pub const ST: &str = "\x1b\\";

// ────────────────────────────────────────────────────────────────
// Configuration
// ────────────────────────────────────────────────────────────────

/// Color definition for a CPI-SI command state.
#[derive(Debug, Clone)]
pub struct CommandColor {
    pub foreground: String,
    pub background: String,
    pub cursor: String,
}

/// State file persistence configuration.
#[derive(Debug, Clone)]
pub struct StateFileConfig {
    pub runtime_dir_env: String,
    pub fallback_dir: String,
    pub filename: String,
}

impl Default for StateFileConfig {
    fn default() -> Self {
        Self {
            runtime_dir_env: "XDG_RUNTIME_DIR".into(),
            fallback_dir: "/tmp".into(),
            filename: "cpisi-wezterm-state".into(),
        }
    }
}

/// WezTerm integration configuration — command colors + state file.
#[derive(Debug, Clone)]
pub struct Config {
    pub command_colors: HashMap<String, CommandColor>,
    pub state_file: StateFileConfig,
}

impl Default for Config {
    fn default() -> Self {
        let mut colors = HashMap::new();
        colors.insert("HALT".into(), CommandColor {
            foreground: "#ff6b6b".into(),
            background: "#1a1a2e".into(),
            cursor: "#ff4757".into(),
        });
        colors.insert("AWAIT".into(), CommandColor {
            foreground: "#ffd93d".into(),
            background: "#1a1a2e".into(),
            cursor: "#feca57".into(),
        });
        colors.insert("PROCEED".into(), CommandColor {
            foreground: "#6bcb77".into(),
            background: "#1a1a2e".into(),
            cursor: "#26de81".into(),
        });
        Self {
            command_colors: colors,
            state_file: StateFileConfig::default(),
        }
    }
}

/// Package-level configuration. Set via `init()`, falls back to defaults.
static CFG: std::sync::OnceLock<Config> = std::sync::OnceLock::new();

/// Initializes the package configuration. Call once at startup.
pub fn init(cfg: Option<Config>) {
    let _ = CFG.set(cfg.unwrap_or_default());
}

/// Returns a reference to the active configuration.
fn cfg() -> &'static Config {
    CFG.get_or_init(Config::default)
}

// =============================================================================
// BODY
// =============================================================================

// ---------------------------------------------------------------------------
// OSC 0, 1 & 2: Title Setting
// ---------------------------------------------------------------------------

/// Sets the tab title (OSC 1).
/// This appears in WezTerm's tab bar.
pub fn set_tab_title(title: &str) {
    let _ = write!(std::io::stderr(), "{ESC}]1;{title}{BEL}");
}

/// Sets the window title (OSC 2).
/// This appears in the window title bar.
pub fn set_window_title(title: &str) {
    let _ = write!(std::io::stderr(), "{ESC}]2;{title}{BEL}");
}

/// Sets both tab and window title (OSC 0).
/// Clears icon name and sets window title.
pub fn set_both_titles(title: &str) {
    let _ = write!(std::io::stderr(), "{ESC}]0;{title}{BEL}");
}

// ---------------------------------------------------------------------------
// OSC 7: Current Working Directory
// ---------------------------------------------------------------------------

/// Emits OSC 7 to tell WezTerm the current working directory.
/// This enables spawning new panes/tabs with the same directory.
pub fn set_cwd(path: &str) {
    let hostname = hostname::get()
        .map(|h| h.to_string_lossy().into_owned())
        .unwrap_or_else(|_| "localhost".to_string());
    let _ = write!(std::io::stderr(), "{ESC}]7;file://{hostname}{path}{BEL}");
}

// ---------------------------------------------------------------------------
// OSC 9 & 777: Notifications
// ---------------------------------------------------------------------------

/// Sends an iTerm2-style notification (OSC 9).
pub fn notify(message: &str) {
    let _ = write!(std::io::stderr(), "{ESC}]9;{message}{BEL}");
}

/// Sends an rxvt-style notification with title (OSC 777).
pub fn notify_with_title(title: &str, body: &str) {
    let _ = write!(std::io::stderr(), "{ESC}]777;notify;{title};{body}{BEL}");
}

// ---------------------------------------------------------------------------
// OSC 1337: User Variables
// ---------------------------------------------------------------------------

/// Sets a user variable that can be read by WezTerm Lua config.
/// Values are base64 encoded per iTerm2 protocol.
pub fn set_user_var(name: &str, value: &str) {
    let encoded = BASE64.encode(value.as_bytes());
    let _ = write!(std::io::stderr(), "{ESC}]1337;SetUserVar={name}={encoded}{BEL}");
}

/// Sets multiple user variables at once.
pub fn set_user_vars(vars: &HashMap<String, String>) {
    for (name, value) in vars {
        set_user_var(name, value);
    }
}

// ---------------------------------------------------------------------------
// OSC 52: Clipboard
// ---------------------------------------------------------------------------

/// Sets the system clipboard content (OSC 52).
/// Target can be: "c" (clipboard), "p" (primary), "s" (select), etc.
pub fn set_clipboard(target: &str, content: &str) {
    let encoded = BASE64.encode(content.as_bytes());
    let _ = write!(std::io::stderr(), "{ESC}]52;{target};{encoded}{BEL}");
}

/// Convenience function for copying to system clipboard.
pub fn copy_to_clipboard(content: &str) {
    set_clipboard("c", content);
}

// ---------------------------------------------------------------------------
// OSC 10/11/12: Dynamic Terminal Colors
// ---------------------------------------------------------------------------

/// Returns the color scheme for a command state from config, if known.
fn command_colors(command: &str) -> Option<&'static CommandColor> {
    let c = cfg();
    c.command_colors.get(&command.to_uppercase())
}

/// Sets terminal foreground color (OSC 10).
pub fn set_foreground_color(hex_color: &str) {
    let _ = write!(std::io::stderr(), "{ESC}]10;{hex_color}{BEL}");
}

/// Sets terminal background color (OSC 11).
pub fn set_background_color(hex_color: &str) {
    let _ = write!(std::io::stderr(), "{ESC}]11;{hex_color}{BEL}");
}

/// Sets terminal cursor color (OSC 12).
pub fn set_cursor_color(hex_color: &str) {
    let _ = write!(std::io::stderr(), "{ESC}]12;{hex_color}{BEL}");
}

/// Resets terminal colors to defaults (OSC 104/110/111/112).
pub fn reset_terminal_colors() {
    let _ = write!(std::io::stderr(), "{ESC}]104{BEL}");
    let _ = write!(std::io::stderr(), "{ESC}]110{BEL}");
    let _ = write!(std::io::stderr(), "{ESC}]111{BEL}");
    let _ = write!(std::io::stderr(), "{ESC}]112{BEL}");
}

/// Sets terminal colors based on command state (HALT, AWAIT, PROCEED).
pub fn set_command_colors(command: &str) {
    if let Some(colors) = command_colors(command) {
        set_foreground_color(&colors.foreground);
        set_background_color(&colors.background);
        set_cursor_color(&colors.cursor);
    }
}

// ---------------------------------------------------------------------------
// CPI-SI State Machine Integration
// ---------------------------------------------------------------------------

/// Holds state machine variables for WezTerm user variable emission.
#[derive(Debug, Clone, Default)]
pub struct StateVars {
    pub position: String,
    pub meaning: String,
    pub command: String,
    pub operator: String,
    pub anchor: String,
    pub x: String,
    pub y: String,
    pub z: f64,
}

/// Returns the path to the state persistence file.
fn state_file_path() -> PathBuf {
    let c = cfg();
    let runtime_dir = env::var(&c.state_file.runtime_dir_env)
        .unwrap_or_else(|_| c.state_file.fallback_dir.clone());
    PathBuf::from(runtime_dir).join(&c.state_file.filename)
}

/// Reads the previous command from the state file.
pub fn read_previous_command() -> String {
    fs::read_to_string(state_file_path())
        .map(|s| s.trim().to_string())
        .unwrap_or_default()
}

/// Persists current command for next invocation.
pub fn write_previous_command(command: &str) {
    let _ = fs::write(state_file_path(), command);
}

/// Sends all state machine variables to WezTerm via OSC 1337.
pub fn emit_state_vars(sv: &StateVars) {
    set_user_var("SM_POSITION", &sv.position);
    set_user_var("SM_MEANING", &sv.meaning);
    set_user_var("SM_COMMAND", &sv.command);
    set_user_var("SM_OPERATOR", &sv.operator);
    set_user_var("SM_ANCHOR", &sv.anchor);
    set_user_var("SM_X", &sv.x);
    set_user_var("SM_Y", &sv.y);
    set_user_var("SM_Z", &format!("{:.1}", sv.z));
}

/// Sets tab and window titles based on state.
pub fn emit_state_titles(sv: &StateVars, instance_name: &str) {
    let tab_title = format!(
        "\u{2b21} {} {} {}",
        sv.position, sv.operator, sv.command
    );
    set_tab_title(&tab_title);

    let window_title = format!("{instance_name} @ {}", sv.anchor);
    set_window_title(&window_title);
}

/// Sends a notification on state change.
pub fn emit_state_notification(old_cmd: &str, new_cmd: &str, position: &str) {
    if old_cmd != new_cmd && !old_cmd.is_empty() {
        let title = format!("CPI-SI: {new_cmd}");
        let body = format!("Transitioned to {new_cmd} at {position}");
        notify_with_title(&title, &body);
    }
}

// ---------------------------------------------------------------------------
// Full State Emission (All-in-One)
// ---------------------------------------------------------------------------

/// Performs complete WezTerm state integration.
///
/// This is the primary entry point for statusline integration:
/// - Reads previous command state
/// - Emits user variables (OSC 1337)
/// - Sets tab/window titles (OSC 1/2)
/// - Sets terminal colors based on command (OSC 10/11/12)
/// - Sends notification if command changed (OSC 777)
/// - Persists current command for next invocation
pub fn emit_full_state(sv: &StateVars, instance_name: &str, workdir: &str) {
    let prev_cmd = read_previous_command();

    emit_state_vars(sv);
    emit_state_titles(sv, instance_name);
    set_command_colors(&sv.command);

    if !workdir.is_empty() {
        set_cwd(workdir);
    }

    emit_state_notification(&prev_cmd, &sv.command, &sv.position);
    write_previous_command(&sv.command);
}

// ---------------------------------------------------------------------------
// Health State Emission
// ---------------------------------------------------------------------------

/// Sends health score information to WezTerm.
///
/// User variables set:
/// - `HEALTH_SCORE` — numeric score (-100 to +100)
/// - `HEALTH_STATE` — Hebrew state name (SHAVAR, SHALEM, TAMIM, etc.)
/// - `HEALTH_LEVEL` — level name (broken, even, perfect, etc.)
pub fn emit_health(score: f64, hebrew_state: &str, level: &str) {
    set_user_var("HEALTH_SCORE", &format!("{score:.0}"));
    set_user_var("HEALTH_STATE", hebrew_state);
    set_user_var("HEALTH_LEVEL", level);
}

// ────────────────────────────────────────────────────────────────
// Registry Integration
// ────────────────────────────────────────────────────────────────

/// Builds a `Config` from cross-layer database queries via the registry.
///
/// Queries L2 platform `terminal_display` for category "command_color".
/// Each row's `value` field is a JSON object with foreground/background/cursor
/// hex colors. The row's `name` field is the command state (HALT, AWAIT, PROCEED).
pub fn config_from_registry(r: &bereshit_l2_registry::Registry) -> Config {
    let mut c = Config::default();

    if let Ok(rows) = r.platform.terminal_by_category("command_color") {
        if !rows.is_empty() {
            let mut colors = HashMap::new();

            for td in &rows {
                // Parse the JSON value field: {"foreground":"#hex","background":"#hex","cursor":"#hex"}
                if let Ok(parsed) = serde_json::from_str::<serde_json::Value>(&td.value) {
                    let fg = parsed.get("foreground").and_then(|v| v.as_str()).unwrap_or("");
                    let bg = parsed.get("background").and_then(|v| v.as_str()).unwrap_or("");
                    let cur = parsed.get("cursor").and_then(|v| v.as_str()).unwrap_or("");

                    if !fg.is_empty() {
                        colors.insert(
                            td.name.clone(),
                            CommandColor {
                                foreground: fg.into(),
                                background: bg.into(),
                                cursor: cur.into(),
                            },
                        );
                    }
                }
            }

            if !colors.is_empty() {
                c.command_colors = colors;
            }
        }
    }

    c
}

// =============================================================================
// CLOSING
// =============================================================================

/// Hostname resolution helper (avoids gethostname dependency).
mod hostname {
    use std::ffi::OsString;

    /// Gets the system hostname by reading /etc/hostname.
    pub fn get() -> Result<OsString, std::io::Error> {
        let name = std::fs::read_to_string("/etc/hostname")
            .map(|s| s.trim().to_string())?;
        Ok(OsString::from(name))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn esc_constant() {
        assert_eq!(ESC, "\x1b");
    }

    #[test]
    fn bel_constant() {
        assert_eq!(BEL, "\x07");
    }

    #[test]
    fn st_constant() {
        assert_eq!(ST, "\x1b\\");
    }

    #[test]
    fn config_default_has_3_commands() {
        let c = Config::default();
        assert_eq!(c.command_colors.len(), 3);
        assert!(c.command_colors.contains_key("HALT"));
        assert!(c.command_colors.contains_key("AWAIT"));
        assert!(c.command_colors.contains_key("PROCEED"));
    }

    #[test]
    fn config_halt_has_all_fields() {
        let c = Config::default();
        let halt = &c.command_colors["HALT"];
        assert!(!halt.foreground.is_empty());
        assert!(!halt.background.is_empty());
        assert!(!halt.cursor.is_empty());
    }

    #[test]
    fn command_colors_known() {
        assert!(command_colors("HALT").is_some());
        assert!(command_colors("AWAIT").is_some());
        assert!(command_colors("PROCEED").is_some());
        assert!(command_colors("halt").is_some()); // case insensitive
    }

    #[test]
    fn command_colors_unknown() {
        assert!(command_colors("UNKNOWN").is_none());
    }

    #[test]
    fn state_vars_default() {
        let sv = StateVars::default();
        assert!(sv.position.is_empty());
        assert_eq!(sv.z, 0.0);
    }

    #[test]
    fn state_file_path_not_empty() {
        let path = state_file_path();
        assert!(path.to_string_lossy().contains("cpisi-wezterm-state"));
    }

    #[test]
    fn state_file_config_defaults() {
        let sfc = StateFileConfig::default();
        assert_eq!(sfc.runtime_dir_env, "XDG_RUNTIME_DIR");
        assert_eq!(sfc.fallback_dir, "/tmp");
        assert_eq!(sfc.filename, "cpisi-wezterm-state");
    }

    #[test]
    fn set_command_colors_unknown_is_safe() {
        // Should not panic.
        set_command_colors("NONEXISTENT");
    }
}
