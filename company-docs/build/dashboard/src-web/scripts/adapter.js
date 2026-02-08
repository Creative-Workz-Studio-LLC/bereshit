/**
 * Adapter Detection — selects TauriAdapter or WebAdapter based on runtime.
 */
import { TauriAdapter } from './adapter-tauri.js';
import { WebAdapter } from './adapter-web.js';
/** Detect whether running inside Tauri or a web browser. */
function detectMode() {
    return window.__TAURI_INTERNALS__ ? 'tauri' : 'web';
}
/** The singleton adapter instance. */
let _adapter = null;
/** Get or create the adapter. */
export function getAdapter() {
    if (!_adapter) {
        const mode = detectMode();
        _adapter = mode === 'tauri' ? new TauriAdapter() : new WebAdapter();
        console.log(`[CWS Dashboard] Mode: ${mode}`);
    }
    return _adapter;
}
