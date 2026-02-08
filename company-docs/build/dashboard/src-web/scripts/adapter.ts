/**
 * Adapter Detection — selects TauriAdapter or WebAdapter based on runtime.
 */

import type { Adapter } from '../types/adapter.js';
import { TauriAdapter } from './adapter-tauri.js';
import { WebAdapter } from './adapter-web.js';

/** Detect whether running inside Tauri or a web browser. */
function detectMode(): 'tauri' | 'web' {
  return (window as any).__TAURI_INTERNALS__ ? 'tauri' : 'web';
}

/** The singleton adapter instance. */
let _adapter: Adapter | null = null;

/** Get or create the adapter. */
export function getAdapter(): Adapter {
  if (!_adapter) {
    const mode = detectMode();
    _adapter = mode === 'tauri' ? new TauriAdapter() : new WebAdapter();
    console.log(`[CWS Dashboard] Mode: ${mode}`);
  }
  return _adapter;
}
