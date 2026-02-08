use tauri::menu::{Menu, MenuEvent, MenuItem, PredefinedMenuItem, Submenu};
use tauri::{AppHandle, Emitter, Manager};

/// Build the native system menu bar.
pub fn build_menu(handle: &AppHandle) -> Result<Menu<tauri::Wry>, tauri::Error> {
    let menu = Menu::new(handle)?;

    // File menu.
    let file_menu = Submenu::with_items(
        handle,
        "File",
        true,
        &[
            &MenuItem::with_id(handle, "open-project", "Open Project...", true, Some("Ctrl+O"))?,
            &PredefinedMenuItem::separator(handle)?,
            &MenuItem::with_id(handle, "preferences", "Preferences", true, Some("Ctrl+Comma"))?,
            &PredefinedMenuItem::separator(handle)?,
            &MenuItem::with_id(handle, "quit", "Quit", true, Some("Ctrl+Q"))?,
        ],
    )?;

    // Build menu.
    let build_menu = Submenu::with_items(
        handle,
        "Build",
        true,
        &[
            &MenuItem::with_id(handle, "build-book", "Full Book", true, Some("Ctrl+B"))?,
            &MenuItem::with_id(handle, "build-section", "Current Section", true, Some("Ctrl+Shift+B"))?,
            &PredefinedMenuItem::separator(handle)?,
            &MenuItem::with_id(handle, "build-clean", "Clean", true, None::<&str>)?,
        ],
    )?;

    // Validate menu.
    let validate_menu = Submenu::with_items(
        handle,
        "Validate",
        true,
        &[
            &MenuItem::with_id(handle, "validate-structure", "Structure", true, None::<&str>)?,
            &MenuItem::with_id(handle, "validate-lint", "Editorial Lint", true, Some("Ctrl+L"))?,
            &MenuItem::with_id(handle, "validate-witness", "Witness Report", true, Some("Ctrl+W"))?,
            &PredefinedMenuItem::separator(handle)?,
            &MenuItem::with_id(handle, "validate-bible", "Bible Citations", true, None::<&str>)?,
            &MenuItem::with_id(handle, "validate-toc", "TOC Check", true, None::<&str>)?,
        ],
    )?;

    // View menu.
    let view_menu = Submenu::with_items(
        handle,
        "View",
        true,
        &[
            &MenuItem::with_id(handle, "toggle-output", "Toggle Output Panel", true, Some("Ctrl+`"))?,
            &MenuItem::with_id(handle, "toggle-sidebar", "Toggle Sidebar", true, Some("Ctrl+\\"))?,
        ],
    )?;

    // Help menu.
    let help_menu = Submenu::with_items(
        handle,
        "Help",
        true,
        &[
            &MenuItem::with_id(handle, "help-docs", "Documentation", true, None::<&str>)?,
            &MenuItem::with_id(handle, "help-about", "About CWS Dashboard", true, None::<&str>)?,
        ],
    )?;

    menu.append(&file_menu)?;
    menu.append(&build_menu)?;
    menu.append(&validate_menu)?;
    menu.append(&view_menu)?;
    menu.append(&help_menu)?;

    Ok(menu)
}

/// Handle menu events — emit to frontend for unified handling.
pub fn handle_menu_event(handle: &AppHandle, event: MenuEvent) {
    let id = event.id().0.as_str();

    match id {
        "quit" => {
            handle.exit(0);
        }
        _ => {
            // Forward all other menu events to the frontend.
            if let Some(window) = handle.get_webview_window("main") {
                let _ = window.emit("menu-action", id);
            }
        }
    }
}
