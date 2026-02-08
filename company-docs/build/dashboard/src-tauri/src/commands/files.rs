use serde::Deserialize;
use tauri_plugin_dialog::DialogExt;
use tauri_plugin_opener::OpenerExt;

#[derive(Debug, Deserialize)]
pub struct FileFilter {
    pub name: String,
    pub extensions: Vec<String>,
}

/// Open a native file dialog.
#[tauri::command]
pub async fn open_file_dialog(
    title: String,
    filters: Vec<FileFilter>,
    directory: bool,
    multiple: bool,
    app: tauri::AppHandle,
) -> Result<Vec<String>, String> {
    let mut builder = app.dialog().file();

    if !title.is_empty() {
        builder = builder.set_title(&title);
    }

    for filter in &filters {
        let exts: Vec<&str> = filter.extensions.iter().map(|s| s.as_str()).collect();
        builder = builder.add_filter(&filter.name, &exts);
    }

    if directory {
        // Pick directory.
        let result = builder.blocking_pick_folder();
        match result {
            Some(path) => Ok(vec![path.to_string()]),
            None => Ok(vec![]),
        }
    } else if multiple {
        let result = builder.blocking_pick_files();
        match result {
            Some(paths) => Ok(paths.iter().map(|p| p.to_string()).collect()),
            None => Ok(vec![]),
        }
    } else {
        let result = builder.blocking_pick_file();
        match result {
            Some(path) => Ok(vec![path.to_string()]),
            None => Ok(vec![]),
        }
    }
}

/// Open a file in the system default viewer.
#[tauri::command]
pub async fn open_in_viewer(
    path: String,
    app: tauri::AppHandle,
) -> Result<(), String> {
    app.opener()
        .open_path(&path, None::<&str>)
        .map_err(|e| format!("Failed to open {}: {}", path, e))
}
