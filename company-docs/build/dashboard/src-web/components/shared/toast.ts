/**
 * Toast Notifications — lightweight notification system.
 *
 * Shows temporary messages at the bottom-right of the screen.
 * Supports success, warning, error, and info types.
 */

export type ToastType = 'success' | 'warning' | 'error' | 'info';

interface ToastOptions {
  message: string;
  type?: ToastType;
  duration?: number;
}

let container: HTMLElement | null = null;

function ensureContainer(): HTMLElement {
  if (container && document.body.contains(container)) return container;

  container = document.createElement('div');
  container.id = 'toast-container';
  container.style.cssText = `
    position: fixed; bottom: var(--space-4); right: var(--space-4);
    display: flex; flex-direction: column-reverse; gap: var(--space-2);
    z-index: 9999; pointer-events: none;
    max-width: 400px;
  `;
  document.body.appendChild(container);
  return container;
}

const typeColors: Record<ToastType, { bg: string; border: string; text: string }> = {
  success: { bg: '#C6F6D5', border: '#38A169', text: '#276749' },
  warning: { bg: '#FEFCBF', border: '#D69E2E', text: '#975A16' },
  error:   { bg: '#FED7D7', border: '#E53E3E', text: '#9B2C2C' },
  info:    { bg: '#BEE3F8', border: '#3182CE', text: '#2A4365' },
};

export function showToast(opts: ToastOptions): void {
  const c = ensureContainer();
  const type = opts.type ?? 'info';
  const duration = opts.duration ?? 4000;
  const colors = typeColors[type];

  const toast = document.createElement('div');
  toast.style.cssText = `
    background: ${colors.bg}; color: ${colors.text};
    border-left: 4px solid ${colors.border};
    padding: var(--space-3) var(--space-4);
    border-radius: var(--border-radius);
    font-family: var(--font-heading); font-size: var(--text-sm);
    box-shadow: var(--shadow-md);
    pointer-events: auto;
    animation: toast-slide-in 200ms ease-out;
    transition: opacity 200ms ease-out, transform 200ms ease-out;
  `;
  toast.textContent = opts.message;

  // Click to dismiss.
  toast.style.cursor = 'pointer';
  toast.addEventListener('click', () => dismiss(toast));

  c.appendChild(toast);

  // Auto-dismiss.
  if (duration > 0) {
    setTimeout(() => dismiss(toast), duration);
  }
}

function dismiss(toast: HTMLElement): void {
  toast.style.opacity = '0';
  toast.style.transform = 'translateX(100%)';
  setTimeout(() => toast.remove(), 200);
}

/** Inject toast animation keyframes. */
const style = document.createElement('style');
style.textContent = `
  @keyframes toast-slide-in {
    from { opacity: 0; transform: translateX(100%); }
    to   { opacity: 1; transform: translateX(0); }
  }
`;
document.head.appendChild(style);
