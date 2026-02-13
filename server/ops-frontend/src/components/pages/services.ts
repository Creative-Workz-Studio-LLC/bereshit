/**
 * Services page — manage systemd services.
 * List, restart, stop, start services with confirmation dialogs.
 */

import { store } from '@/state/store';
import type { ServiceStatus } from '@/state/store';
import { toast } from '@/components/toast';

export function renderServices(): HTMLElement {
  const page = document.createElement('div');
  const unsubs: (() => void)[] = [];

  function render() {
    const status = store.get('status');
    const loading = store.get('loading');
    const services = status?.services ?? [];

    page.innerHTML = `
      <div class="page-header">
        <h2>Services</h2>
        <p>Manage systemd services — start, stop, restart</p>
      </div>

      ${loading && !status ? '<p class="text-muted">Loading...</p>' : ''}

      <div class="card-grid">
        ${services.map(svc => renderServiceCard(svc)).join('')}
      </div>
    `;

    // Attach button handlers after render
    services.forEach(svc => {
      const card = page.querySelector(`[data-service="${svc.name}"]`);
      if (!card) return;

      card.querySelectorAll<HTMLButtonElement>('[data-action]').forEach(btn => {
        btn.addEventListener('click', () => {
          const action = btn.dataset.action!;
          handleServiceAction(svc.name, action);
        });
      });
    });
  }

  unsubs.push(store.subscribe('status', render));
  unsubs.push(store.subscribe('loading', render));
  render();

  Object.assign(page, {
    cleanup: () => unsubs.forEach(fn => fn()),
  });

  return page;
}

function renderServiceCard(svc: ServiceStatus): string {
  const statusClass = svc.active ? 'badge-success' : 'badge-danger';
  const statusText = svc.active ? 'running' : 'stopped';

  return `
    <div class="card" data-service="${svc.name}">
      <div class="card-header">
        <span class="mono">${svc.name}</span>
        <span class="badge ${statusClass}">${statusText}</span>
      </div>
      <div style="display: flex; gap: var(--space-2); flex-wrap: wrap; margin-top: var(--space-3);">
        <div style="flex: 1; min-width: 120px;">
          <div class="text-muted" style="font-size: var(--font-size-xs)">PID</div>
          <div class="mono">${svc.pid || '—'}</div>
        </div>
        <div style="flex: 1; min-width: 120px;">
          <div class="text-muted" style="font-size: var(--font-size-xs)">Memory</div>
          <div class="mono">${svc.memory}</div>
        </div>
        <div style="flex: 1; min-width: 120px;">
          <div class="text-muted" style="font-size: var(--font-size-xs)">Uptime</div>
          <div class="mono">${svc.uptime || '—'}</div>
        </div>
      </div>
      <div style="display: flex; gap: var(--space-2); margin-top: var(--space-3);">
        <button class="btn btn-sm" data-action="restart">Restart</button>
        ${svc.active
          ? '<button class="btn btn-sm btn-danger" data-action="stop">Stop</button>'
          : '<button class="btn btn-sm btn-success" data-action="start">Start</button>'
        }
      </div>
    </div>
  `;
}

async function handleServiceAction(name: string, action: string) {
  const confirmed = confirm(`${action.charAt(0).toUpperCase() + action.slice(1)} ${name}?`);
  if (!confirmed) return;

  const btn = document.querySelector(`[data-service="${name}"] [data-action="${action}"]`) as HTMLButtonElement;
  if (btn) {
    btn.disabled = true;
    btn.textContent = `${action}ing...`;
  }

  try {
    const resp = await fetch(`/api/ops/services/${name}/${action}`, { method: 'POST' });
    const data = await resp.json();

    if (!resp.ok) {
      toast(`Failed to ${action} ${name}: ${data.error}`, 'error');
      return;
    }

    toast(`${name}: ${action} successful`, 'success');

    // Refresh status after action
    const { refreshStatus } = await import('@/state/api');
    await refreshStatus();
  } catch (err) {
    toast(`Error: ${err}`, 'error');
  } finally {
    if (btn) {
      btn.disabled = false;
      btn.textContent = action.charAt(0).toUpperCase() + action.slice(1);
    }
  }
}
