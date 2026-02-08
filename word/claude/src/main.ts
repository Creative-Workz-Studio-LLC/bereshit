// ============================================================================
// CPI-SI Dashboard — Application Entry Point
// Creates store, WebSocket connection, and mounts dashboard
// ============================================================================

import { DashboardStore, createDefaultState } from './state/store';
import { DashboardWebSocket } from './state/websocket';
import { Dashboard } from './components/layout/Dashboard';
import type { StateSnapshot } from './state/types';
import './styles/main.css';

async function main(): Promise<void> {
    const store = new DashboardStore();
    const ws = new DashboardWebSocket(8470);

    // Wire WebSocket events to store
    ws.onStateUpdate((state) => {
        store.updateState(state);
    });

    ws.onLogEvent((event) => {
        store.addEvent(event);
    });

    ws.onConnectionChange((connected) => {
        store.setConnected(connected);
    });

    // Fetch initial state from REST API
    try {
        const resp = await fetch('/api/state');
        if (resp.ok) {
            const state = (await resp.json()) as StateSnapshot;
            store.updateState(state);
        }
    } catch {
        // API not available yet; use default state
        store.updateState(createDefaultState());
    }

    // Connect WebSocket
    ws.connect();

    // Mount dashboard
    const app = document.getElementById('app');
    if (!app) {
        throw new Error('No #app element found');
    }

    const dashboard = new Dashboard(store);
    dashboard.mount(app);
}

main().catch((err: unknown) => {
    console.error('Dashboard initialization failed:', err);
});
