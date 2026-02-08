use crate::state::AppState;
use tauri::State;

/// Read the full build.config.yaml and return as JSON string.
#[tauri::command]
pub async fn read_config(
    state: State<'_, AppState>,
) -> Result<String, String> {
    let config_path = &state.config_path;
    if !config_path.exists() {
        return Err(format!("Config not found: {}", config_path.display()));
    }
    let yaml_str = std::fs::read_to_string(config_path)
        .map_err(|e| format!("Failed to read config: {}", e))?;
    let value: serde_json::Value = serde_yaml::from_str(&yaml_str)
        .map_err(|e| format!("Failed to parse YAML: {}", e))?;
    serde_json::to_string_pretty(&value)
        .map_err(|e| format!("Failed to serialize JSON: {}", e))
}

/// Write a single section of build.config.yaml.
/// Reads the full YAML, merges the section, writes back.
#[tauri::command]
pub async fn write_config_section(
    section: String,
    data: String,
    state: State<'_, AppState>,
) -> Result<(), String> {
    let config_path = &state.config_path;
    let yaml_str = std::fs::read_to_string(config_path)
        .map_err(|e| format!("Failed to read config: {}", e))?;

    let mut doc: serde_yaml::Value = serde_yaml::from_str(&yaml_str)
        .map_err(|e| format!("Failed to parse YAML: {}", e))?;

    let section_value: serde_yaml::Value = serde_json::from_str::<serde_json::Value>(&data)
        .map_err(|e| format!("Invalid JSON data: {}", e))
        .and_then(|json| {
            // Convert JSON value to YAML value via serialization round-trip.
            let yaml_str = serde_json::to_string(&json).map_err(|e| e.to_string())?;
            serde_yaml::from_str(&yaml_str).map_err(|e| e.to_string())
        })?;

    // Merge into the document at the specified section key.
    if let serde_yaml::Value::Mapping(ref mut map) = doc {
        let key = serde_yaml::Value::String(section);
        map.insert(key, section_value);
    } else {
        return Err("Config root is not a mapping".into());
    }

    let output = serde_yaml::to_string(&doc)
        .map_err(|e| format!("Failed to serialize YAML: {}", e))?;
    std::fs::write(config_path, output)
        .map_err(|e| format!("Failed to write config: {}", e))?;

    Ok(())
}

/// Read dashboard preferences from XDG config dir.
#[tauri::command]
pub async fn read_preferences(
    state: State<'_, AppState>,
) -> Result<String, String> {
    let prefs_path = &state.preferences_path;
    if !prefs_path.exists() {
        return Ok("{}".to_string());
    }
    std::fs::read_to_string(prefs_path)
        .map_err(|e| format!("Failed to read preferences: {}", e))
}

/// Write dashboard preferences to XDG config dir.
#[tauri::command]
pub async fn write_preferences(
    data: String,
    state: State<'_, AppState>,
) -> Result<(), String> {
    let prefs_path = &state.preferences_path;

    // Ensure parent directory exists.
    if let Some(parent) = prefs_path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| format!("Failed to create preferences dir: {}", e))?;
    }

    std::fs::write(prefs_path, data)
        .map_err(|e| format!("Failed to write preferences: {}", e))
}
