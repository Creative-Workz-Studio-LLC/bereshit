/**
 * Tauri Adapter — communicates with the Rust backend via Tauri IPC.
 */

import type {
  Adapter,
  CommandRequest,
  CommandResult,
  FileDialogOptions,
  MakeRequest,
  OutputLine,
} from '../types/adapter.js';

// Tauri API is loaded at runtime via the webview, not bundled.
declare const window: Window & {
  __TAURI__?: {
    core: {
      invoke: (cmd: string, args?: Record<string, unknown>) => Promise<unknown>;
    };
    event: {
      listen: (event: string, handler: (e: { payload: unknown }) => void) => Promise<() => void>;
    };
  };
};

function invoke<T>(cmd: string, args?: Record<string, unknown>): Promise<T> {
  return window.__TAURI__!.core.invoke(cmd, args) as Promise<T>;
}

function listen(event: string, handler: (payload: unknown) => void): () => void {
  let unlisten: (() => void) | null = null;
  window.__TAURI__!.event
    .listen(event, (e) => handler(e.payload))
    .then((fn) => { unlisten = fn; });
  return () => { unlisten?.(); };
}

export class TauriAdapter implements Adapter {
  getMode(): 'tauri' | 'web' {
    return 'tauri';
  }

  async runCommand(req: CommandRequest): Promise<CommandResult> {
    return invoke<CommandResult>('run_cli_command', {
      command: req.command,
      args: req.args,
    });
  }

  async runMake(req: MakeRequest): Promise<CommandResult> {
    return invoke<CommandResult>('run_make_target', {
      target: req.target,
      vars: req.vars ?? {},
    });
  }

  async cancelProcess(processId: string): Promise<void> {
    await invoke('cancel_process', { processId });
  }

  async readConfig(): Promise<Record<string, unknown>> {
    const json = await invoke<string>('read_config');
    return JSON.parse(json);
  }

  async writeConfigSection(section: string, data: unknown): Promise<void> {
    await invoke('write_config_section', {
      section,
      data: JSON.stringify(data),
    });
  }

  async readPreferences(): Promise<Record<string, unknown>> {
    const json = await invoke<string>('read_preferences');
    return JSON.parse(json);
  }

  async writePreferences(prefs: Record<string, unknown>): Promise<void> {
    await invoke('write_preferences', {
      data: JSON.stringify(prefs),
    });
  }

  async openFileDialog(opts: FileDialogOptions): Promise<string[]> {
    return invoke<string[]>('open_file_dialog', {
      title: opts.title ?? '',
      filters: opts.filters ?? [],
      directory: opts.directory ?? false,
      multiple: opts.multiple ?? false,
    });
  }

  onOutput(callback: (line: OutputLine) => void): () => void {
    return listen('cli-output', (payload) => {
      callback(payload as OutputLine);
    });
  }

  onExit(callback: (result: CommandResult) => void): () => void {
    return listen('cli-exit', (payload) => {
      callback(payload as CommandResult);
    });
  }
}
