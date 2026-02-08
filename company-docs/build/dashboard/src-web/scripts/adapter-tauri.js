/**
 * Tauri Adapter — communicates with the Rust backend via Tauri IPC.
 */
function invoke(cmd, args) {
    return window.__TAURI__.core.invoke(cmd, args);
}
function listen(event, handler) {
    let unlisten = null;
    window.__TAURI__.event
        .listen(event, (e) => handler(e.payload))
        .then((fn) => { unlisten = fn; });
    return () => { unlisten?.(); };
}
export class TauriAdapter {
    getMode() {
        return 'tauri';
    }
    async runCommand(req) {
        return invoke('run_cli_command', {
            command: req.command,
            args: req.args,
        });
    }
    async runMake(req) {
        return invoke('run_make_target', {
            target: req.target,
            vars: req.vars ?? {},
        });
    }
    async cancelProcess(processId) {
        await invoke('cancel_process', { processId });
    }
    async readConfig() {
        const json = await invoke('read_config');
        return JSON.parse(json);
    }
    async writeConfigSection(section, data) {
        await invoke('write_config_section', {
            section,
            data: JSON.stringify(data),
        });
    }
    async readPreferences() {
        const json = await invoke('read_preferences');
        return JSON.parse(json);
    }
    async writePreferences(prefs) {
        await invoke('write_preferences', {
            data: JSON.stringify(prefs),
        });
    }
    async openFileDialog(opts) {
        return invoke('open_file_dialog', {
            title: opts.title ?? '',
            filters: opts.filters ?? [],
            directory: opts.directory ?? false,
            multiple: opts.multiple ?? false,
        });
    }
    onOutput(callback) {
        return listen('cli-output', (payload) => {
            callback(payload);
        });
    }
    onExit(callback) {
        return listen('cli-exit', (payload) => {
            callback(payload);
        });
    }
}
