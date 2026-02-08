use crate::state::AppState;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use tauri::{Emitter, State, Window};
use tokio::io::{AsyncBufReadExt, BufReader};
use tokio::process::Command;

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct CommandResult {
    pub process_id: String,
    pub exit_code: i32,
    pub success: bool,
}

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct OutputLine {
    pub process_id: String,
    pub stream: String, // "stdout" or "stderr"
    pub line: String,
    pub timestamp: String,
}

/// Run a builder CLI command: `node dist/cli.js <command> <args...>`
#[tauri::command]
pub async fn run_cli_command(
    command: String,
    args: Vec<String>,
    state: State<'_, AppState>,
    window: Window,
) -> Result<CommandResult, String> {
    let process_id = uuid_v4();
    let cli_path = state.builder_cli.to_string_lossy().to_string();
    let project_dir = state.project_dir.to_string_lossy().to_string();

    let mut cmd_args = vec![cli_path, command];
    cmd_args.extend(args);

    spawn_and_stream("node", cmd_args, &project_dir, &process_id, &state, &window).await
}

/// Run a Makefile target: `make -C <project_dir> <target> VAR=val`
#[tauri::command]
pub async fn run_make_target(
    target: String,
    vars: HashMap<String, String>,
    state: State<'_, AppState>,
    window: Window,
) -> Result<CommandResult, String> {
    let process_id = uuid_v4();
    let project_dir = state.project_dir.to_string_lossy().to_string();

    let mut cmd_args = vec![
        format!("-C"),
        project_dir.clone(),
        target,
    ];
    for (key, val) in &vars {
        cmd_args.push(format!("{}={}", key, val));
    }

    spawn_and_stream("make", cmd_args, &project_dir, &process_id, &state, &window).await
}

/// Cancel a running process by ID.
#[tauri::command]
pub async fn cancel_process(
    process_id: String,
    state: State<'_, AppState>,
) -> Result<(), String> {
    let processes = state.processes.lock().map_err(|e| e.to_string())?;
    if let Some(&pid) = processes.get(&process_id) {
        // Send SIGTERM on Unix.
        unsafe {
            libc::kill(pid as i32, libc::SIGTERM);
        }
        Ok(())
    } else {
        Err(format!("Process {} not found", process_id))
    }
}

/// Spawn a command and stream stdout/stderr via Tauri events.
async fn spawn_and_stream(
    program: &str,
    args: Vec<String>,
    cwd: &str,
    process_id: &str,
    state: &State<'_, AppState>,
    window: &Window,
) -> Result<CommandResult, String> {
    let mut child = Command::new(program)
        .args(&args)
        .current_dir(cwd)
        .stdout(std::process::Stdio::piped())
        .stderr(std::process::Stdio::piped())
        .spawn()
        .map_err(|e| format!("Failed to spawn {}: {}", program, e))?;

    // Track PID for cancellation.
    if let Some(pid) = child.id() {
        let mut processes = state.processes.lock().map_err(|e| e.to_string())?;
        processes.insert(process_id.to_string(), pid);
    }

    let pid = process_id.to_string();

    // Stream stdout.
    let stdout = child.stdout.take();
    let stderr = child.stderr.take();
    let win_out = window.clone();
    let win_err = window.clone();
    let pid_out = pid.clone();
    let pid_err = pid.clone();

    let stdout_handle = tokio::spawn(async move {
        if let Some(out) = stdout {
            let reader = BufReader::new(out);
            let mut lines = reader.lines();
            while let Ok(Some(line)) = lines.next_line().await {
                let cleaned = strip_ansi(&line);
                let _ = win_out.emit("cli-output", OutputLine {
                    process_id: pid_out.clone(),
                    stream: "stdout".into(),
                    line: cleaned,
                    timestamp: now_iso(),
                });
            }
        }
    });

    let stderr_handle = tokio::spawn(async move {
        if let Some(err) = stderr {
            let reader = BufReader::new(err);
            let mut lines = reader.lines();
            while let Ok(Some(line)) = lines.next_line().await {
                let cleaned = strip_ansi(&line);
                let _ = win_err.emit("cli-output", OutputLine {
                    process_id: pid_err.clone(),
                    stream: "stderr".into(),
                    line: cleaned,
                    timestamp: now_iso(),
                });
            }
        }
    });

    // Wait for streams to complete.
    let _ = stdout_handle.await;
    let _ = stderr_handle.await;

    // Wait for process exit.
    let status = child.wait().await.map_err(|e| e.to_string())?;
    let exit_code = status.code().unwrap_or(-1);

    // Remove from active processes.
    if let Ok(mut processes) = state.processes.lock() {
        processes.remove(&pid);
    }

    // Emit completion event.
    let _ = window.emit("cli-exit", CommandResult {
        process_id: pid.clone(),
        exit_code,
        success: exit_code == 0,
    });

    Ok(CommandResult {
        process_id: pid,
        exit_code,
        success: exit_code == 0,
    })
}

/// Strip ANSI escape codes from a string.
fn strip_ansi(s: &str) -> String {
    let bytes = strip_ansi_escapes::strip(s.as_bytes());
    String::from_utf8_lossy(&bytes).to_string()
}

/// Generate a simple UUID-like string.
fn uuid_v4() -> String {
    use std::time::{SystemTime, UNIX_EPOCH};
    let t = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap_or_default()
        .as_nanos();
    format!("{:x}", t)
}

/// ISO 8601 timestamp.
fn now_iso() -> String {
    use std::time::{SystemTime, UNIX_EPOCH};
    let secs = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap_or_default()
        .as_secs();
    // Simple timestamp — not fully ISO but good enough for display.
    format!("{}", secs)
}
