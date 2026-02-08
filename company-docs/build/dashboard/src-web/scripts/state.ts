/**
 * Reactive State Store — vanilla EventTarget-based state management.
 *
 * Components subscribe to state changes via addEventListener.
 * No external dependencies.
 */

class Store extends EventTarget {
  private data: Map<string, unknown> = new Map();

  /** Get a value by key. */
  get<T>(key: string): T | undefined {
    return this.data.get(key) as T | undefined;
  }

  /** Set a value and dispatch a change event. */
  set(key: string, value: unknown): void {
    const prev = this.data.get(key);
    this.data.set(key, value);

    this.dispatchEvent(new CustomEvent('change', {
      detail: { key, value, previous: prev },
    }));

    // Also dispatch a key-specific event for targeted subscriptions.
    this.dispatchEvent(new CustomEvent(`change:${key}`, {
      detail: { value, previous: prev },
    }));
  }

  /** Subscribe to all changes. */
  onChange(callback: (key: string, value: unknown) => void): () => void {
    const handler = (e: Event) => {
      const detail = (e as CustomEvent).detail;
      callback(detail.key, detail.value);
    };
    this.addEventListener('change', handler);
    return () => this.removeEventListener('change', handler);
  }

  /** Subscribe to a specific key's changes. */
  onKey(key: string, callback: (value: unknown) => void): () => void {
    const handler = (e: Event) => {
      callback((e as CustomEvent).detail.value);
    };
    this.addEventListener(`change:${key}`, handler);
    return () => this.removeEventListener(`change:${key}`, handler);
  }
}

/** Singleton store instance. */
export const store = new Store();

// Standard state keys.
export const STATE = {
  /** Currently running process ID, or null. */
  RUNNING_PROCESS: 'runningProcess',
  /** Output lines for the output panel. */
  OUTPUT_LINES: 'outputLines',
  /** Loaded config data. */
  CONFIG: 'config',
  /** Dashboard preferences. */
  PREFERENCES: 'preferences',
} as const;
