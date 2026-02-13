/**
 * Reactive state store using EventTarget.
 * Zero dependencies. Components subscribe to state changes.
 *
 * Usage:
 *   store.subscribe('status', (status) => render(status));
 *   store.set('status', newStatus);
 */

export interface OpsState {
  status: FullStatus | null;
  loading: boolean;
  error: string | null;
  lastRefresh: number;
}

export interface FullStatus {
  host: string;
  timestamp: string;
  services: ServiceStatus[];
  resources: ResourceInfo | null;
  serverUrl: string;
  healthy: boolean;
  errors: string[];
}

export interface ServiceStatus {
  name: string;
  active: boolean;
  enabled: boolean;
  subState: string;
  pid: number;
  memory: string;
  uptime: string;
}

export interface ResourceInfo {
  cpuPercent: number;
  memoryTotal: number;
  memoryUsed: number;
  diskTotal: number;
  diskUsed: number;
  loadAvg1: number;
  loadAvg5: number;
  loadAvg15: number;
}

class Store extends EventTarget {
  private state: OpsState = {
    status: null,
    loading: false,
    error: null,
    lastRefresh: 0,
  };

  get<K extends keyof OpsState>(key: K): OpsState[K] {
    return this.state[key];
  }

  set<K extends keyof OpsState>(key: K, value: OpsState[K]): void {
    this.state[key] = value;
    this.dispatchEvent(new CustomEvent(key, { detail: value }));
    this.dispatchEvent(new CustomEvent('change', { detail: { key, value } }));
  }

  subscribe<K extends keyof OpsState>(key: K, callback: (value: OpsState[K]) => void): () => void {
    const handler = (e: Event) => callback((e as CustomEvent).detail);
    this.addEventListener(key, handler);
    return () => this.removeEventListener(key, handler);
  }

  getState(): Readonly<OpsState> {
    return { ...this.state };
  }
}

export const store = new Store();
