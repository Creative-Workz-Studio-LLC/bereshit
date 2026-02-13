/**
 * API client for cws-ops server.
 * All calls go to /api/ops/* endpoints.
 */

import { store } from './store';
import type { FullStatus } from './store';

const BASE = '/api/ops';

async function fetchJSON<T>(path: string): Promise<T> {
  const resp = await fetch(`${BASE}${path}`);
  if (!resp.ok) {
    throw new Error(`${resp.status}: ${resp.statusText}`);
  }
  return resp.json() as Promise<T>;
}

/** Fetch full infrastructure status */
export async function refreshStatus(): Promise<void> {
  store.set('loading', true);
  store.set('error', null);

  try {
    const status = await fetchJSON<FullStatus>('/status');
    store.set('status', status);
    store.set('lastRefresh', Date.now());
  } catch (err) {
    store.set('error', err instanceof Error ? err.message : String(err));
  } finally {
    store.set('loading', false);
  }
}

/** Start auto-refresh at given interval (ms) */
export function startAutoRefresh(intervalMs = 15_000): () => void {
  refreshStatus(); // Immediate first fetch
  const id = setInterval(refreshStatus, intervalMs);
  return () => clearInterval(id);
}
