/**
 * CWS Ops — Frontend Entry Point
 *
 * Initializes the app shell, router, state store, and auto-refresh.
 * Zero framework dependencies — vanilla TS + Web Components.
 */

import { registerRoutes, type Route } from '@/router/router';
import { startAutoRefresh } from '@/state/api';
import { store } from '@/state/store';
import { renderOverview } from '@/components/pages/overview';
import { renderDeploy } from '@/components/pages/deploy';
import { renderLogs } from '@/components/pages/logs';
import { renderServices } from '@/components/pages/services';
import { renderDNS } from '@/components/pages/dns';
import { renderBackups } from '@/components/pages/backups';

// ── Routes ───────────────────────────────────────────────────────────

const routes: Route[] = [
  { path: '/', label: 'Overview', render: renderOverview },
  { path: '/deploy', label: 'Deploy', render: renderDeploy },
  { path: '/logs', label: 'Logs', render: renderLogs },
  { path: '/services', label: 'Services', render: renderServices },
  { path: '/dns', label: 'DNS', render: renderDNS },
  { path: '/backups', label: 'Backups', render: renderBackups },
];

// ── App Shell ────────────────────────────────────────────────────────

function createAppShell(): HTMLElement {
  const app = document.getElementById('app')!;
  app.innerHTML = `
    <aside class="sidebar">
      <div class="sidebar-header">
        <h1>CWS Ops</h1>
        <div class="subtitle">Infrastructure Management</div>
      </div>
      <nav class="sidebar-nav">
        ${routes.map(r => `<a href="#${r.path}">${r.label}</a>`).join('')}
      </nav>
      <div style="padding: var(--space-4); border-top: 1px solid var(--color-border);">
        <div class="connection-status" id="connection-status">
          <span class="connection-dot"></span>
          <span>Connected</span>
        </div>
        <div style="margin-top: var(--space-2); font-size: var(--font-size-xs); color: var(--color-text-dim);">
          v0.1.0 &middot; CreativeWorkzStudio
        </div>
      </div>
    </aside>
    <main class="main-content" id="page-content"></main>
  `;

  return document.getElementById('page-content')!;
}

// ── Initialize ───────────────────────────────────────────────────────

const pageContainer = createAppShell();
registerRoutes(routes, pageContainer);
startAutoRefresh(15_000);

// ── Connection Status ─────────────────────────────────────────────────

function updateConnectionStatus() {
  const dot = document.querySelector('#connection-status .connection-dot');
  const label = document.querySelector('#connection-status span:last-child');
  if (!dot || !label) return;

  const error = store.get('error');
  const loading = store.get('loading');

  if (error) {
    dot.className = 'connection-dot disconnected';
    label.textContent = 'Disconnected';
  } else if (loading) {
    dot.className = 'connection-dot connecting';
    label.textContent = 'Refreshing...';
  } else {
    dot.className = 'connection-dot';
    label.textContent = 'Connected';
  }
}

store.subscribe('error', updateConnectionStatus);
store.subscribe('loading', updateConnectionStatus);

// ── Keyboard Shortcuts ────────────────────────────────────────────────

document.addEventListener('keydown', (e) => {
  // Don't capture when typing in inputs
  if (e.target instanceof HTMLInputElement || e.target instanceof HTMLTextAreaElement ||
      e.target instanceof HTMLSelectElement) return;

  const idx = parseInt(e.key) - 1;
  const route = routes[idx];
  if (route) {
    window.location.hash = `#${route.path}`;
  }
});
