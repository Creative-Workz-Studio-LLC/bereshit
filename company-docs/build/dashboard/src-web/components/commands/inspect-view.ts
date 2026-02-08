/**
 * Inspect View — Status, Inventory, Spine commands.
 *
 * Status and Inventory auto-run on navigation.
 * Spine has a manual Run button.
 */

import { createCommandCard, runCliCommand } from '../shared/runner.js';

export function createInspectView(): HTMLElement {
  const el = document.createElement('div');

  const h2 = document.createElement('h2');
  h2.textContent = 'Inspect';
  el.appendChild(h2);

  // ── Status ────────────────────────────────────────────────────
  el.appendChild(createCommandCard({
    title: 'Status',
    description: 'Show build status for all sections and formats.',
    onRun: () => { runCliCommand('status'); },
  }));

  // ── Inventory ─────────────────────────────────────────────────
  el.appendChild(createCommandCard({
    title: 'Inventory',
    description: 'List all book components: sections, articles, parts.',
    onRun: () => { runCliCommand('inventory'); },
  }));

  // ── Spine ─────────────────────────────────────────────────────
  el.appendChild(createCommandCard({
    title: 'Spine',
    description: 'Display the book structure tree — all sections, their order, and hierarchy.',
    onRun: () => { runCliCommand('spine'); },
  }));

  return el;
}
