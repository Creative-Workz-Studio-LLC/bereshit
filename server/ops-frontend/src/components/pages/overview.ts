/**
 * Overview page — infrastructure status at a glance.
 * Shows server health, services grid, and resource usage.
 */

import { store } from '@/state/store';
import type { FullStatus, ServiceStatus, ResourceInfo } from '@/state/store';

export function renderOverview(): HTMLElement {
  const page = document.createElement('div');

  const unsubs: (() => void)[] = [];

  function render() {
    const status = store.get('status');
    const loading = store.get('loading');
    const error = store.get('error');

    page.innerHTML = `
      <div class="page-header">
        <h2>Overview</h2>
        <p>Infrastructure health at a glance</p>
      </div>

      ${error ? `<div class="card mb-4" style="border-color: var(--color-danger)"><p class="text-danger">${error}</p></div>` : ''}

      ${loading && !status ? '<p class="text-muted">Loading...</p>' : ''}

      ${status ? renderStatus(status) : ''}
    `;
  }

  unsubs.push(store.subscribe('status', render));
  unsubs.push(store.subscribe('loading', render));
  unsubs.push(store.subscribe('error', render));
  render();

  Object.assign(page, {
    cleanup: () => unsubs.forEach(fn => fn()),
  });

  return page;
}

function renderStatus(status: FullStatus): string {
  return `
    <!-- Quick Stats -->
    <div class="card-grid mb-4">
      ${renderHealthCard(status)}
      ${renderResourceCards(status.resources)}
    </div>

    <!-- Services -->
    <div class="card mt-4">
      <div class="card-header">
        <h3>Services</h3>
        <span class="badge ${status.services.every(s => s.active) ? 'badge-success' : 'badge-warning'}">
          ${status.services.filter(s => s.active).length}/${status.services.length} running
        </span>
      </div>
      ${renderServicesTable(status.services)}
    </div>

    <p class="text-muted mt-4" style="font-size: var(--font-size-xs)">
      Host: ${status.host} &middot; Last updated: ${new Date(status.timestamp).toLocaleTimeString()}
    </p>
  `;
}

function renderHealthCard(status: FullStatus): string {
  const healthy = status.healthy;
  return `
    <div class="card">
      <div class="stat-label">Server Health</div>
      <div class="stat-value ${healthy ? 'text-success' : 'text-danger'}">
        ${healthy ? 'Healthy' : 'Unhealthy'}
      </div>
      <a href="${status.serverUrl}" target="_blank" class="text-muted" style="font-size: var(--font-size-xs)">
        ${status.serverUrl}
      </a>
    </div>
  `;
}

function renderResourceCards(resources: ResourceInfo | null): string {
  if (!resources) return '';

  const memPercent = resources.memoryTotal > 0
    ? ((resources.memoryUsed / resources.memoryTotal) * 100).toFixed(0)
    : '0';
  const diskPercent = resources.diskTotal > 0
    ? ((resources.diskUsed / resources.diskTotal) * 100).toFixed(0)
    : '0';

  return `
    <div class="card">
      <div class="stat-label">CPU Load</div>
      <div class="stat-value">${resources.cpuPercent.toFixed(0)}%</div>
      <div class="text-muted" style="font-size: var(--font-size-xs)">
        ${resources.loadAvg1.toFixed(2)} / ${resources.loadAvg5.toFixed(2)} / ${resources.loadAvg15.toFixed(2)}
      </div>
    </div>
    <div class="card">
      <div class="stat-label">Memory</div>
      <div class="stat-value">${memPercent}%</div>
      <div class="text-muted" style="font-size: var(--font-size-xs)">
        ${humanBytes(resources.memoryUsed)} / ${humanBytes(resources.memoryTotal)}
      </div>
    </div>
    <div class="card">
      <div class="stat-label">Disk</div>
      <div class="stat-value">${diskPercent}%</div>
      <div class="text-muted" style="font-size: var(--font-size-xs)">
        ${humanBytes(resources.diskUsed)} / ${humanBytes(resources.diskTotal)}
      </div>
    </div>
  `;
}

function renderServicesTable(services: ServiceStatus[]): string {
  return `
    <table class="table">
      <thead>
        <tr>
          <th>Service</th>
          <th>Status</th>
          <th>PID</th>
          <th>Memory</th>
          <th>Uptime</th>
        </tr>
      </thead>
      <tbody>
        ${services.map(svc => `
          <tr>
            <td class="mono">${svc.name}</td>
            <td>
              <span class="badge ${svc.active ? 'badge-success' : 'badge-danger'}">
                ${svc.active ? 'running' : 'stopped'}
              </span>
            </td>
            <td class="mono text-muted">${svc.pid || '—'}</td>
            <td class="mono">${svc.memory}</td>
            <td class="text-muted">${svc.uptime || '—'}</td>
          </tr>
        `).join('')}
      </tbody>
    </table>
  `;
}

function humanBytes(bytes: number): string {
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(0)}KB`;
  if (bytes < 1024 * 1024 * 1024) return `${(bytes / (1024 * 1024)).toFixed(0)}MB`;
  return `${(bytes / (1024 * 1024 * 1024)).toFixed(1)}GB`;
}
