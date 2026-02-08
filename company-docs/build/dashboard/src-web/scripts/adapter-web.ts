/**
 * Web Adapter — communicates with the CWS Server via HTTP + WebSocket.
 *
 * runCommand/runMake start a process via REST, then wait for the exit event
 * via WebSocket before resolving. This matches the Tauri adapter's behavior
 * where invoke() waits for the Rust backend to finish.
 */

import type {
  Adapter,
  CommandRequest,
  CommandResult,
  FileDialogOptions,
  MakeRequest,
  OutputLine,
} from '../types/adapter.js';

const API_BASE = '/api/builder';

async function api<T>(path: string, opts?: RequestInit): Promise<T> {
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

export class WebAdapter implements Adapter {
  private ws: WebSocket | null = null;
  private outputCallbacks: Array<(line: OutputLine) => void> = [];
  private exitCallbacks: Array<(result: CommandResult) => void> = [];
  // Pending exit promises keyed by processId.
  private pendingExits = new Map<string, (result: CommandResult) => void>();
  private wsConnected = false;
  private reconnectAttempts = 0;

  constructor() {
    this.connectWebSocket();
  }

  /** Check if WebSocket is currently connected. */
  isConnected(): boolean {
    return this.wsConnected;
  }

  getMode(): 'tauri' | 'web' {
    return 'web';
  }

  async runCommand(req: CommandRequest): Promise<CommandResult> {
    const { processId } = await api<{ processId: string }>('/commands/run', {
      method: 'POST',
      body: JSON.stringify(req),
    });

    // Wait for the exit event for this processId.
    return this.waitForExit(processId);
  }

  async runMake(req: MakeRequest): Promise<CommandResult> {
    const { processId } = await api<{ processId: string }>('/commands/make', {
      method: 'POST',
      body: JSON.stringify(req),
    });

    return this.waitForExit(processId);
  }

  async cancelProcess(processId: string): Promise<void> {
    await api('/commands/cancel', {
      method: 'POST',
      body: JSON.stringify({ processId }),
    });
  }

  async readConfig(): Promise<Record<string, unknown>> {
    return api<Record<string, unknown>>('/config');
  }

  async writeConfigSection(section: string, data: unknown): Promise<void> {
    await api(`/config/${section}`, {
      method: 'PATCH',
      body: JSON.stringify(data),
    });
  }

  async readPreferences(): Promise<Record<string, unknown>> {
    return api<Record<string, unknown>>('/config/preferences');
  }

  async writePreferences(prefs: Record<string, unknown>): Promise<void> {
    await api('/config/preferences', {
      method: 'PUT',
      body: JSON.stringify(prefs),
    });
  }

  async openFileDialog(_opts: FileDialogOptions): Promise<string[]> {
    // In web mode, we can't open native file dialogs.
    // The file picker component handles this with an <input type="file">
    // or a typed path input. Return empty — the component handles it.
    return [];
  }

  onOutput(callback: (line: OutputLine) => void): () => void {
    this.outputCallbacks.push(callback);
    return () => {
      this.outputCallbacks = this.outputCallbacks.filter((cb) => cb !== callback);
    };
  }

  onExit(callback: (result: CommandResult) => void): () => void {
    this.exitCallbacks.push(callback);
    return () => {
      this.exitCallbacks = this.exitCallbacks.filter((cb) => cb !== callback);
    };
  }

  /** Wait for a process exit event matching the given processId. */
  private waitForExit(processId: string): Promise<CommandResult> {
    return new Promise((resolve) => {
      this.pendingExits.set(processId, resolve);
    });
  }

  private connectWebSocket(): void {
    const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${location.host}/ws`;

    try {
      this.ws = new WebSocket(wsUrl);
    } catch (err) {
      console.error('[WS] Failed to create WebSocket:', err);
      this.wsConnected = false;
      this.scheduleReconnect();
      return;
    }

    this.ws.onopen = () => {
      this.wsConnected = true;
      if (this.reconnectAttempts > 0) {
        console.log(`[WS] Reconnected after ${this.reconnectAttempts} attempt(s)`);
      } else {
        console.log('[WS] Connected');
      }
      this.reconnectAttempts = 0;
    };

    this.ws.onmessage = (event) => {
      try {
        const msg = JSON.parse(event.data);
        if (msg.type === 'output') {
          const line: OutputLine = {
            processId: msg.processId,
            stream: msg.stream,
            line: msg.line,
            timestamp: msg.timestamp,
          };
          for (const cb of this.outputCallbacks) {
            cb(line);
          }
        } else if (msg.type === 'exit') {
          const result: CommandResult = {
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
      } catch {
        // Ignore malformed messages.
      }
    };

    this.ws.onerror = (event) => {
      console.error('[WS] Connection error:', event);
    };

    this.ws.onclose = () => {
      this.wsConnected = false;

      // Reject any pending exits with a connection error.
      if (this.pendingExits.size > 0) {
        for (const [id, resolve] of this.pendingExits.entries()) {
          resolve({ processId: id, exitCode: -1, success: false });
        }
        this.pendingExits.clear();
      }

      this.scheduleReconnect();
    };
  }

  private scheduleReconnect(): void {
    this.reconnectAttempts++;
    // Exponential backoff: 1s, 2s, 4s, 8s, max 15s.
    const delay = Math.min(1000 * Math.pow(2, this.reconnectAttempts - 1), 15000);
    console.log(`[WS] Reconnecting in ${delay}ms (attempt ${this.reconnectAttempts})...`);
    setTimeout(() => this.connectWebSocket(), delay);
  }
}
