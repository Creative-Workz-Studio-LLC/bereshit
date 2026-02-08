/**
 * Connection Status — WebSocket connection indicator.
 *
 * Shows a small dot in the sidebar footer indicating backend connectivity.
 * Green = connected, Yellow = reconnecting, Red = disconnected.
 */

export function createConnectionStatus(): HTMLElement {
  const wrapper = document.createElement('div');
  wrapper.style.cssText = `
    display: flex; align-items: center; gap: var(--space-2);
    padding: var(--space-2) var(--space-4);
    font-size: var(--text-xs); color: rgba(255,255,255,0.5);
  `;

  const dot = document.createElement('span');
  dot.style.cssText = `
    display: inline-block; width: 8px; height: 8px;
    border-radius: 50%; background: var(--color-success);
    transition: background var(--transition-base);
  `;

  const label = document.createElement('span');
  label.textContent = 'Connected';

  wrapper.appendChild(dot);
  wrapper.appendChild(label);

  // Poll WebSocket state periodically.
  setInterval(() => {
    // Check if any WebSocket is open on the page.
    // The WebAdapter creates one; we can check via a simple fetch test.
    const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${location.host}/ws`;

    // We can't directly check WebSocket state from outside the adapter,
    // so we use the presence of active connections as a proxy.
    // A lightweight approach: test if the API is reachable.
    fetch('/api/commands/active', { method: 'GET' })
      .then((res) => {
        if (res.ok) {
          dot.style.background = 'var(--color-success)';
          label.textContent = 'Connected';
        } else {
          dot.style.background = 'var(--color-warning)';
          label.textContent = 'Degraded';
        }
      })
      .catch(() => {
        dot.style.background = 'var(--color-error)';
        label.textContent = 'Disconnected';
      });
  }, 10000);

  return wrapper;
}
