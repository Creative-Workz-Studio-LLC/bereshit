// CWS Dashboard — Tauri v2 Backend
//
// Provides native OS integration: file dialogs, system menu, process spawning,
// real-time output streaming via Tauri events.
//
// "Write the vision, and make it plain upon tables,
//  that he may run that readeth it." — Habakkuk 2:2

mod commands;
mod menu;
mod state;

use tauri::Manager;

use state::AppState;

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_dialog::init())
        .plugin(tauri_plugin_shell::init())
        .plugin(tauri_plugin_opener::init())
        .setup(|app| {
            let state = AppState::new(app.handle())?;
            app.manage(state);
            Ok(())
        })
        .menu(|handle| menu::build_menu(handle))
        .on_menu_event(menu::handle_menu_event)
        .invoke_handler(tauri::generate_handler![
            commands::build::run_cli_command,
            commands::build::run_make_target,
            commands::build::cancel_process,
            commands::config::read_config,
            commands::config::write_config_section,
            commands::config::read_preferences,
            commands::config::write_preferences,
            commands::files::open_file_dialog,
            commands::files::open_in_viewer,
        ])
        .run(tauri::generate_context!())
        .expect("error running CWS Dashboard");
}
