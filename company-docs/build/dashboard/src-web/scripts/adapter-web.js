/**
 * Web Adapter — communicates with the CWS Server via HTTP + WebSocket.
 *
 * runCommand/runMake start a process via REST, then wait for the exit event
 * via WebSocket before resolving. This matches the Tauri adapter's behavior
 * where invoke() waits for the Rust backend to finish.
 */
const API_BASE = '/api/builder';
async function api(path, opts) {
    const res = await fetch(`${API_BASE}${path}`, {
        headers: { 'Content-Type': 'application/json' },
        ...opts,
    });
    if (!res.ok) {
        const text = await res.text();
        throw new Error(`API error ${res.status}: ${text}`);
    }
    return res.json();
}
export class WebAdapter {
    ws = null;
    outputCallbacks = [];
    exitCallbacks = [];
    // Pending exit promises keyed by processId.
    pendingExits = new Map();
    constructor() {
        this.connectWebSocket();
    }
    getMode() {
        return 'web';
    }
    async runCommand(req) {
        const { processId } = await api('/commands/run', {
            method: 'POST',
            body: JSON.stringify(req),
        });
        // Wait for the exit event for this processId.
        return this.waitForExit(processId);
    }
    async runMake(req) {
        const { processId } = await api('/commands/make', {
            method: 'POST',
            body: JSON.stringify(req),
        });
        return this.waitForExit(processId);
    }
    async cancelProcess(processId) {
        await api('/commands/cancel', {
            method: 'POST',
            body: JSON.stringify({ processId }),
        });
    }
    async readConfig() {
        return api('/config');
    }
    async writeConfigSection(section, data) {
        await api(`/config/${section}`, {
            method: 'PATCH',
            body: JSON.stringify(data),
        });
    }
    async readPreferences() {
        return api('/config/preferences');
    }
    async writePreferences(prefs) {
        await api('/config/preferences', {
            method: 'PUT',
            body: JSON.stringify(prefs),
        });
    }
    async openFileDialog(_opts) {
        // In web mode, we can't open native file dialogs.
        // The file picker component handles this with an <input type="file">
        // or a typed path input. Return empty — the component handles it.
        return [];
    }
    onOutput(callback) {
        this.outputCallbacks.push(callback);
        return () => {
            this.outputCallbacks = this.outputCallbacks.filter((cb) => cb !== callback);
        };
    }
    onExit(callback) {
        this.exitCallbacks.push(callback);
        return () => {
            this.exitCallbacks = this.exitCallbacks.filter((cb) => cb !== callback);
        };
    }
    /** Wait for a process exit event matching the given processId. */
    waitForExit(processId) {
        return new Promise((resolve) => {
            this.pendingExits.set(processId, resolve);
        });
    }
    connectWebSocket() {
        const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
        this.ws = new WebSocket(`${protocol}//${location.host}/ws`);
        this.ws.onmessage = (event) => {
            try {
                const msg = JSON.parse(event.data);
                if (msg.type === 'output') {
                    const line = {
                        processId: msg.processId,
                        stream: msg.stream,
                        line: msg.line,
                        timestamp: msg.timestamp,
                    };
                    for (const cb of this.outputCallbacks) {
                        cb(line);
                    }
                }
                else if (msg.type === 'exit') {
                    const result = {
                        processId: msg.processId,
                        exitCode: msg.exitCode,
                        success: msg.success,
                    };
                    // Resolve any pending waitForExit promise.
                    const pending = this.pendingExits.get(msg.processId);
                    if (pending) {
                        this.pendingExits.delete(msg.processId);
                        pending(result);
                    }
                    // Also notify general exit subscribers.
                    for (const cb of this.exitCallbacks) {
                        cb(result);
                    }
                }
            }
            catch {
                // Ignore malformed messages.
            }
        };
        this.ws.onclose = () => {
            // Reconnect after a short delay.
            setTimeout(() => this.connectWebSocket(), 2000);
        };
    }
}
