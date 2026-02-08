/**
 * CWS Dashboard — Main Entry Point
 *
 * Initializes the adapter, router, state, output panel, theme,
 * keyboard shortcuts, and connection status.
 *
 * "Write the vision, and make it plain upon tables,
 *  that he may run that readeth it." — Habakkuk 2:2
 */

import { getAdapter } from './adapter.js';
import { router } from './router.js';
import { store, STATE } from './state.js';
import { clearOutput } from '../components/shared/runner.js';
import { showToast } from '../components/shared/toast.js';
import { initTheme, createThemeToggle } from '../components/shared/theme-toggle.js';
import { initKeyboardShortcuts } from '../components/shared/keyboard.js';
import { createConnectionStatus } from '../components/shared/connection-status.js';
import { createBuildView } from '../components/commands/build-view.js';
import { createInspectView } from '../components/commands/inspect-view.js';
import { createValidateView } from '../components/commands/validate-view.js';
import { createAnalysisView } from '../components/commands/analysis-view.js';
import { createUtilityView } from '../components/commands/utility-view.js';
import { createConfigView } from '../components/commands/config-view.js';
import { createWitnessView } from '../components/commands/witness-view.js';
import type { OutputLine } from '../types/adapter.js';

// =============================================================================
// Initialization
// =============================================================================

document.addEventListener('DOMContentLoaded', async () => {
  const adapter = getAdapter();

  // Hide in-app menu bar in Tauri mode (native menu takes over).
  const menuBar = document.querySelector('.menu-bar');
  if (menuBar && adapter.getMode() === 'tauri') {
    menuBar.classList.add('hidden');
  }

  // ── Theme ─────────────────────────────────────────────────────
  await initTheme();

  // ── State ─────────────────────────────────────────────────────
  store.set(STATE.OUTPUT_LINES, [] as OutputLine[]);
  store.set(STATE.RUNNING_PROCESS, null);

  // ── Output Streaming ──────────────────────────────────────────
  adapter.onOutput((line) => {
    const lines = store.get<OutputLine[]>(STATE.OUTPUT_LINES) ?? [];
    lines.push(line);
    store.set(STATE.OUTPUT_LINES, [...lines]);
    appendOutputLine(line);
  });

  adapter.onExit((result) => {
    store.set(STATE.RUNNING_PROCESS, null);
    updateRunningIndicator(false);

    if (result.success) {
      appendOutputSystem(`Process completed (exit code ${result.exitCode})`);
      showToast({ message: 'Command completed successfully.', type: 'success', duration: 3000 });
    } else {
      appendOutputSystem(`Process failed (exit code ${result.exitCode})`);
      showToast({ message: `Command failed (exit code ${result.exitCode}).`, type: 'error', duration: 5000 });
    }
  });

  // Track running state for indicator.
  store.onKey(STATE.RUNNING_PROCESS, (value) => {
    updateRunningIndicator(value !== null && value !== undefined);
  });

  // ── Router ────────────────────────────────────────────────────
  const contentEl = document.querySelector('.content-area') as HTMLElement;
  if (contentEl) {
    router.setContainer(contentEl);

    router.on('#/build', () => createBuildView());
    router.on('#/inspect', () => createInspectView());
    router.on('#/validate', () => createValidateView());
    router.on('#/analysis', () => createAnalysisView());
    router.on('#/config', () => createConfigView());
    router.on('#/witness', () => createWitnessView());
    router.on('#/utility', () => createUtilityView());

    router.start();
  }

  // ── Sidebar Navigation ────────────────────────────────────────
  document.querySelectorAll('.nav-item').forEach((item) => {
    item.addEventListener('click', () => {
      const route = item.getAttribute('data-route');
      if (route) router.navigate(route);
    });
  });

  // ── Sidebar Footer (theme toggle + connection status) ─────────
  const sidebar = document.querySelector('.sidebar');
  if (sidebar) {
    const footer = document.createElement('div');
    footer.className = 'sidebar-footer';
    footer.style.cssText = 'margin-top: auto;';

    // Theme toggle.
    footer.appendChild(createThemeToggle());

    // Connection status (web mode only — Tauri uses IPC, not WebSocket).
    if (adapter.getMode() === 'web') {
      footer.appendChild(createConnectionStatus());
    }

    // Version info.
    const version = document.createElement('div');
    version.style.cssText = 'padding: var(--space-2) var(--space-4); font-size: var(--text-xs); color: rgba(255,255,255,0.3);';
    version.textContent = 'v0.1.0';
    footer.appendChild(version);

    sidebar.appendChild(footer);
  }

  // ── Output Panel ──────────────────────────────────────────────
  setupOutputPanel();

  // ── Keyboard Shortcuts ────────────────────────────────────────
  initKeyboardShortcuts();

  // ── Menu Bar (web mode) ───────────────────────────────────────
  document.querySelectorAll('.menu-bar-item').forEach((item) => {
    item.addEventListener('click', () => {
      const action = item.getAttribute('data-action');
      if (action) handleMenuAction(action);
    });
  });

  // ── Native Menu Events (Tauri mode) ───────────────────────────
  if (adapter.getMode() === 'tauri') {
    (window as any).__TAURI__?.event?.listen('menu-action', (event: any) => {
      handleMenuAction(event.payload);
    });
  }

  // ── Preferences: Restore sidebar/output state ─────────────────
  await restoreLayoutPreferences();

  console.log('[CWS Dashboard] Initialized');
});

// =============================================================================
// Output Panel
// =============================================================================

function setupOutputPanel(): void {
  const outputHeader = document.querySelector('.output-header');
  const outputPanel = document.querySelector('.output-panel');
  if (!outputHeader || !outputPanel) return;

  // Add clear button and running indicator to header.
  const headerRight = document.createElement('div');
  headerRight.style.cssText = 'display: flex; align-items: center; gap: var(--space-3);';

  const indicator = document.createElement('span');
  indicator.id = 'running-indicator';
  indicator.className = 'running-indicator';
  indicator.style.display = 'none';
  indicator.innerHTML = '<span class="spinner"></span> Running...';
  headerRight.appendChild(indicator);

  const clearBtn = document.createElement('button');
  clearBtn.style.cssText =
    'background: none; border: none; color: #A0AEC0; cursor: pointer; font-size: var(--text-xs); padding: 2px 6px;';
  clearBtn.textContent = 'Clear';
  clearBtn.addEventListener('click', (e) => {
    e.stopPropagation();
    clearOutput();
  });
  headerRight.appendChild(clearBtn);

  const toggle = outputHeader.querySelector('.output-toggle');
  if (toggle) {
    outputHeader.insertBefore(headerRight, toggle);
  } else {
    outputHeader.appendChild(headerRight);
  }

  // Toggle expand/collapse on header click.
  outputHeader.addEventListener('click', () => {
    outputPanel.classList.toggle('collapsed');
    const arrow = outputPanel.querySelector('.output-toggle');
    if (arrow) {
      arrow.textContent = outputPanel.classList.contains('collapsed') ? '\u25B2' : '\u25BC';
    }
    saveLayoutPreferences();
  });
}

function appendOutputLine(line: OutputLine): void {
  const body = document.querySelector('.output-body');
  if (!body) return;

  const div = document.createElement('div');
  div.className = `output-line-${line.stream}`;
  div.textContent = line.line;
  body.appendChild(div);

  // Auto-expand panel on first output.
  const panel = document.querySelector('.output-panel');
  if (panel?.classList.contains('collapsed')) {
    panel.classList.remove('collapsed');
    const arrow = panel.querySelector('.output-toggle');
    if (arrow) arrow.textContent = '\u25BC';
  }

  // Auto-scroll to bottom.
  body.scrollTop = body.scrollHeight;
}

function appendOutputSystem(message: string): void {
  const body = document.querySelector('.output-body');
  if (!body) return;

  const div = document.createElement('div');
  div.style.color = '#A0AEC0';
  div.style.fontStyle = 'italic';
  div.textContent = `--- ${message} ---`;
  body.appendChild(div);
  body.scrollTop = body.scrollHeight;
}

function updateRunningIndicator(running: boolean): void {
  const indicator = document.getElementById('running-indicator');
  if (indicator) {
    indicator.style.display = running ? 'inline-flex' : 'none';
  }
}

// =============================================================================
// Layout Preferences
// =============================================================================

async function restoreLayoutPreferences(): Promise<void> {
  try {
    const adapter = getAdapter();
    const prefs = await adapter.readPreferences();
    store.set(STATE.PREFERENCES, prefs);

    // Sidebar collapsed state.
    if (prefs.sidebarCollapsed) {
      document.querySelector('.sidebar')?.classList.add('collapsed');
    }

    // Output panel collapsed state.
    if (prefs.outputCollapsed !== false) {
      // Default: collapsed.
    } else {
      document.querySelector('.output-panel')?.classList.remove('collapsed');
    }
  } catch {
    // First run — no preferences yet.
  }
}

function saveLayoutPreferences(): void {
  const sidebar = document.querySelector('.sidebar');
  const outputPanel = document.querySelector('.output-panel');

  const adapter = getAdapter();
  adapter.readPreferences()
    .then((prefs) => {
      prefs.sidebarCollapsed = sidebar?.classList.contains('collapsed') ?? false;
      prefs.outputCollapsed = outputPanel?.classList.contains('collapsed') ?? true;
      return adapter.writePreferences(prefs);
    })
    .catch(() => { /* Silently ignore save failures. */ });
}

// =============================================================================
// Menu Action Handler
// =============================================================================

function handleMenuAction(action: string): void {
  switch (action) {
    case 'open-project':
      showToast({ message: 'Open project: coming soon.', type: 'info' });
      break;
    case 'preferences':
      router.navigate('#/config');
      break;
    case 'build-book':
      router.navigate('#/build');
      break;
    case 'build-section':
      router.navigate('#/build');
      break;
    case 'build-clean':
      clearOutput();
      showToast({ message: 'Output cleared.', type: 'info', duration: 2000 });
      break;
    case 'validate-structure':
    case 'validate-lint':
    case 'validate-witness':
    case 'validate-bible':
    case 'validate-toc':
      router.navigate('#/validate');
      break;
    case 'toggle-output': {
      const panel = document.querySelector('.output-panel');
      panel?.classList.toggle('collapsed');
      const arrow = panel?.querySelector('.output-toggle');
      if (arrow) {
        arrow.textContent = panel?.classList.contains('collapsed') ? '\u25B2' : '\u25BC';
      }
      saveLayoutPreferences();
      break;
    }
    case 'toggle-sidebar': {
      const sidebar = document.querySelector('.sidebar');
      sidebar?.classList.toggle('collapsed');
      saveLayoutPreferences();
      break;
    }
    case 'help-about':
      showToast({
        message: 'CWS Manual Builder Dashboard v0.1.0 — CreativeWorkzStudio LLC',
        type: 'info',
        duration: 5000,
      });
      break;
    default:
      console.log(`[Menu] Unhandled action: ${action}`);
  }
}
