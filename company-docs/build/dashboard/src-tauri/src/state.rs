use std::collections::HashMap;
use std::path::PathBuf;
use std::sync::Mutex;

/// Application state managed by Tauri.
pub struct AppState {
    /// Path to company-docs/ project root.
    pub project_dir: PathBuf,
    /// Path to build/build.config.yaml.
    pub config_path: PathBuf,
    /// Path to build/builder/dist/cli.js.
    pub builder_cli: PathBuf,
    /// Active spawned processes (for cancellation).
    pub processes: Mutex<HashMap<String, u32>>, // id → PID
    /// User preferences path.
    pub preferences_path: PathBuf,
}

impl AppState {
    pub fn new(_handle: &tauri::AppHandle) -> Result<Self, Box<dyn std::error::Error>> {
        // Resolve project directory: the dashboard lives at
        // company-docs/build/dashboard/, so project root is 3 levels up.
        let exe_dir = std::env::current_exe()?
            .parent()
            .unwrap_or(&PathBuf::from("."))
            .to_path_buf();

        // In dev mode, current_dir is more reliable than exe path.
        let cwd = std::env::current_dir().unwrap_or(exe_dir);

        // Walk up to find company-docs/ (look for build/build.config.yaml).
        let project_dir = find_project_dir(&cwd)
            .unwrap_or_else(|| cwd.clone());

        let config_path = project_dir.join("build/build.config.yaml");
        let builder_cli = project_dir.join("build/builder/dist/cli.js");

        // XDG config dir for preferences.
        let config_home = std::env::var("XDG_CONFIG_HOME")
            .map(PathBuf::from)
            .unwrap_or_else(|_| {
                dirs_next()
                    .unwrap_or_else(|| PathBuf::from("."))
                    .join(".config")
            });
        let preferences_path = config_home
            .join("cws-dashboard")
            .join("preferences.json");

        Ok(Self {
            project_dir,
            config_path,
            builder_cli,
            processes: Mutex::new(HashMap::new()),
            preferences_path,
        })
    }
}

/// Walk up directories to find company-docs/ root.
fn find_project_dir(start: &PathBuf) -> Option<PathBuf> {
    let mut dir = start.clone();
    for _ in 0..10 {
        if dir.join("build/build.config.yaml").exists() {
            return Some(dir);
        }
        if !dir.pop() {
            break;
        }
    }
    None
}

/// Get home directory.
fn dirs_next() -> Option<PathBuf> {
    std::env::var("HOME").ok().map(PathBuf::from)
}
