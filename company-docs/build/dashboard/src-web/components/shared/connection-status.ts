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

  // Poll backend health periodically.
  const checkConnection = () => {
    fetch('/healthz', { method: 'GET' })
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
  };

  // Check immediately, then every 10 seconds.
  checkConnection();
  setInterval(checkConnection, 10000);

  return wrapper;
}
