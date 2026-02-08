// ============================================================================
// CPI-SI Dashboard — WebSocket Client
// Auto-reconnecting WebSocket with exponential backoff
// ============================================================================

import type { StateSnapshot, LogEvent, WSMessage } from './types';

export type StateUpdateCallback = (state: StateSnapshot) => void;
export type LogEventCallback = (event: LogEvent) => void;
export type ConnectionCallback = (connected: boolean) => void;

export class DashboardWebSocket {
    private ws: WebSocket | null = null;
    private url: string;
    private reconnectDelay = 1000;
    private maxReconnectDelay = 30000;
    private reconnectTimer: ReturnType<typeof setTimeout> | null = null;
    private intentionalClose = false;

    private stateCallbacks: StateUpdateCallback[] = [];
    private logCallbacks: LogEventCallback[] = [];
    private connectionCallbacks: ConnectionCallback[] = [];

    constructor(port: number = 8470) {
        const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
        const host = window.location.hostname || 'localhost';
        this.url = `${protocol}//${host}:${port}/ws`;
    }

    connect(): void {
        if (this.ws?.readyState === WebSocket.OPEN) {
            return;
        }

        try {
            this.ws = new WebSocket(this.url);
        } catch (_err) {
            this.scheduleReconnect();
            return;
        }

        this.ws.onopen = () => {
            this.reconnectDelay = 1000;
            this.notifyConnection(true);
        };

        this.ws.onmessage = (event: MessageEvent) => {
            this.handleMessage(event.data as string);
        };

        this.ws.onclose = () => {
            this.notifyConnection(false);
            if (!this.intentionalClose) {
                this.scheduleReconnect();
            }
        };

        this.ws.onerror = () => {
            // onclose will fire after onerror, triggering reconnect
        };
    }

    disconnect(): void {
        this.intentionalClose = true;
        if (this.reconnectTimer) {
            clearTimeout(this.reconnectTimer);
            this.reconnectTimer = null;
        }
        if (this.ws) {
            this.ws.close();
            this.ws = null;
        }
    }

    onStateUpdate(callback: StateUpdateCallback): void {
        this.stateCallbacks.push(callback);
    }

    onLogEvent(callback: LogEventCallback): void {
        this.logCallbacks.push(callback);
    }

    onConnectionChange(callback: ConnectionCallback): void {
        this.connectionCallbacks.push(callback);
    }

    get connected(): boolean {
        return this.ws?.readyState === WebSocket.OPEN;
    }

    private handleMessage(data: string): void {
        let msg: WSMessage;
        try {
            msg = JSON.parse(data) as WSMessage;
        } catch {
            return;
        }

        switch (msg.type) {
            case 'state_update':
                if (msg.snapshot) {
                    for (const cb of this.stateCallbacks) {
                        cb(msg.snapshot);
                    }
                }
                break;

            case 'log_event':
                if (msg.event) {
                    for (const cb of this.logCallbacks) {
                        cb(msg.event);
                    }
                }
                break;

            case 'path_update':
            case 'history_update':
                // Future: handle path and history updates
                break;
        }
    }

    private scheduleReconnect(): void {
        if (this.reconnectTimer) {
            return;
        }

        this.reconnectTimer = setTimeout(() => {
            this.reconnectTimer = null;
            this.reconnectDelay = Math.min(
                this.reconnectDelay * 2,
                this.maxReconnectDelay
            );
            this.connect();
        }, this.reconnectDelay);
    }

    private notifyConnection(connected: boolean): void {
        for (const cb of this.connectionCallbacks) {
            cb(connected);
        }
    }
}
