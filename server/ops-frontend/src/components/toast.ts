/**
 * Toast notification system — simple, non-blocking feedback.
 * Shows success/error/info messages that auto-dismiss.
 */

let container: HTMLDivElement | null = null;

function ensureContainer(): HTMLDivElement {
  if (!container) {
    container = document.createElement('div');
    container.className = 'toast-container';
    document.body.appendChild(container);
  }
  return container;
}

export function toast(message: string, type: 'success' | 'error' | 'info' = 'info', duration = 3000) {
  const el = document.createElement('div');
  el.className = `toast toast-${type}`;
  el.textContent = message;

  const c = ensureContainer();
  c.appendChild(el);

  // Trigger enter animation
  requestAnimationFrame(() => el.classList.add('toast-visible'));

  setTimeout(() => {
    el.classList.remove('toast-visible');
    el.addEventListener('transitionend', () => el.remove());
  }, duration);
}
