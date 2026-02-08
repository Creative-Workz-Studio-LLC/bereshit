/**
 * Witness Dashboard — Visual system health report.
 *
 * Runs `cws-build witness --json`, parses the WitnessReport, and renders:
 *   - System health banner (tov / yashar / shavar)
 *   - Metadata row (timestamp, duration, versions)
 *   - Totals summary bar
 *   - Domain health cards (sorted worst-first)
 *   - Prioritized recommendations with action buttons
 *
 * "Surely the Lord GOD will do nothing, but he revealeth
 *  his secret unto his servants the prophets." — Amos 3:7
 */

import { runCommandCapture } from '../shared/runner.js';
import { showToast } from '../shared/toast.js';
import { createCheckboxGroup, createToggle } from '../shared/form-controls.js';

// =============================================================================
// Types (mirrored from builder — dashboard never imports builder directly)
// =============================================================================

type SystemHealth = 'healthy' | 'degraded' | 'failing';
type DomainHealth = 'pass' | 'warn' | 'fail' | 'skipped';
type DomainName = 'structural' | 'editorial' | 'theme' | 'bible' | 'toc';

interface DomainSummary {
  domain: DomainName;
  label: string;
  health: DomainHealth;
  counts: Record<string, number>;
  duration: number;
  error?: string;
}

interface Recommendation {
  priority: number;
  domain: DomainName;
  message: string;
  file?: string;
  action?: string;
}

interface WitnessReport {
  metadata: {
    schemaVersion: string;
    timestamp: string;
    configVersion: string;
    builderVersion: string;
    duration: number;
    domainsRequested: DomainName[];
    domainsExecuted: DomainName[];
    trigger: string;
    options: Record<string, unknown>;
  };
  health: SystemHealth;
  summary: DomainSummary[];
  domains: unknown[];
  totals: { errors: number; warnings: number; info: number; passes: number };
  recommendations: Recommendation[];
}

// =============================================================================
// Hebrew State Mapping
// =============================================================================

const HEALTH_MAP: Record<SystemHealth, { hebrew: string; label: string; cssVar: string }> = {
  healthy:  { hebrew: 'tov',    label: 'Healthy',  cssVar: '--color-tov' },
  degraded: { hebrew: 'yashar', label: 'Degraded', cssVar: '--color-yashar' },
  failing:  { hebrew: 'shavar', label: 'Failing',  cssVar: '--color-shavar' },
};

const DOMAIN_HEALTH_COLORS: Record<DomainHealth, string> = {
  pass:    'var(--color-success)',
  warn:    'var(--color-warning)',
  fail:    'var(--color-error)',
  skipped: 'var(--color-text-muted)',
};

const DOMAIN_ICONS: Record<DomainName, string> = {
  structural: '\u{1F3D7}',  // building construction
  editorial:  '\u{1F4DD}',  // memo
  theme:      '\u{1F3A8}',  // art palette
  bible:      '\u{1F4D6}',  // open book
  toc:        '\u{1F4D1}',  // bookmark tabs
};

// =============================================================================
// Main View
// =============================================================================

export function createWitnessView(): HTMLElement {
  const el = document.createElement('div');
  el.className = 'witness-dashboard';

  // ── Controls Panel ─────────────────────────────────────────
  const controlsCard = document.createElement('div');
  controlsCard.className = 'card';
  controlsCard.style.marginBottom = 'var(--space-6)';

  const controlsHeader = document.createElement('h3');
  controlsHeader.className = 'card-header';
  controlsHeader.textContent = 'Witness Report';
  controlsCard.appendChild(controlsHeader);

  const controlsDesc = document.createElement('p');
  controlsDesc.style.cssText = 'font-size: var(--text-sm); color: var(--color-text-muted); margin-bottom: var(--space-4);';
  controlsDesc.textContent = 'Comprehensive system health check across 5 domains. Observation that becomes action.';
  controlsCard.appendChild(controlsDesc);

  const domainCheckboxes = createCheckboxGroup('witness-dash-domains', 'Domains', [
    { value: 'structural', label: 'Structural' },
    { value: 'editorial', label: 'Editorial' },
    { value: 'theme', label: 'Theme' },
    { value: 'bible', label: 'Bible' },
    { value: 'toc', label: 'TOC' },
  ]);
  const verboseToggle = createToggle('witness-dash-verbose', 'Verbose');

  const controlsRow = document.createElement('div');
  controlsRow.style.cssText = 'display: flex; align-items: flex-end; gap: var(--space-6); flex-wrap: wrap;';
  controlsRow.appendChild(domainCheckboxes.wrapper);
  controlsRow.appendChild(verboseToggle.wrapper);

  const runBtn = document.createElement('button');
  runBtn.className = 'btn btn-primary';
  runBtn.textContent = 'Run Witness';
  runBtn.style.marginLeft = 'auto';
  controlsRow.appendChild(runBtn);

  controlsCard.appendChild(controlsRow);
  el.appendChild(controlsCard);

  // ── Dashboard Container (populated after run) ─────────────
  const dashContainer = document.createElement('div');
  dashContainer.id = 'witness-dashboard-content';
  el.appendChild(dashContainer);

  // Show empty state initially.
  renderEmptyState(dashContainer);

  // ── Run Handler ─────────────────────────────────────────────
  runBtn.addEventListener('click', async () => {
    const args: string[] = ['--json'];
    const domains = domainCheckboxes.getSelected();
    if (domains.length > 0 && domains.length < 5) {
      args.push('--domains', domains.join(','));
    }
    if (verboseToggle.isChecked()) args.push('--verbose');

    runBtn.disabled = true;
    runBtn.textContent = 'Running...';
    dashContainer.innerHTML = '';
    renderLoadingState(dashContainer);

    try {
      const result = await runCommandCapture('witness', args);

      if (!result.success) {
        showToast({ message: `Witness failed (exit code ${result.exitCode}).`, type: 'error', duration: 5000 });
        dashContainer.innerHTML = '';
        renderErrorState(dashContainer, `Process exited with code ${result.exitCode}.\n${result.stderr}`);
        return;
      }

      // Parse JSON from stdout — the report may be preceded by non-JSON lines.
      const report = parseWitnessJSON(result.stdout);
      if (!report) {
        showToast({ message: 'Failed to parse witness report JSON.', type: 'error', duration: 5000 });
        dashContainer.innerHTML = '';
        renderErrorState(dashContainer, 'Could not parse JSON from witness output. Run with verbose to see raw output.');
        return;
      }

      dashContainer.innerHTML = '';
      renderDashboard(dashContainer, report);
      showToast({ message: 'Witness report complete.', type: 'success', duration: 3000 });

    } catch (err) {
      showToast({ message: `Error: ${err instanceof Error ? err.message : String(err)}`, type: 'error', duration: 5000 });
      dashContainer.innerHTML = '';
      renderErrorState(dashContainer, err instanceof Error ? err.message : String(err));
    } finally {
      runBtn.disabled = false;
      runBtn.textContent = 'Run Witness';
    }
  });

  return el;
}

// =============================================================================
// JSON Parsing
// =============================================================================

function parseWitnessJSON(stdout: string): WitnessReport | null {
  // The JSON output may be on a single line or may be preceded by log lines.
  // Strategy: find the first '{' that starts a valid JSON object.
  const lines = stdout.split('\n');

  // Try full output first.
  try {
    return JSON.parse(stdout) as WitnessReport;
  } catch { /* not pure JSON */ }

  // Try from first line that starts with '{'.
  for (let i = 0; i < lines.length; i++) {
    if (lines[i]!.trimStart().startsWith('{')) {
      const candidate = lines.slice(i).join('\n');
      try {
        return JSON.parse(candidate) as WitnessReport;
      } catch { /* keep trying */ }
    }
  }

  return null;
}

// =============================================================================
// State Renderers
// =============================================================================

function renderEmptyState(container: HTMLElement): void {
  const empty = document.createElement('div');
  empty.className = 'witness-empty';
  empty.innerHTML = `
    <div class="witness-empty-icon">\u{1F50D}</div>
    <h3>No Report Yet</h3>
    <p>Run the witness to observe system health across all 5 domains.</p>
  `;
  container.appendChild(empty);
}

function renderLoadingState(container: HTMLElement): void {
  const loading = document.createElement('div');
  loading.className = 'witness-loading';
  loading.innerHTML = `
    <div class="spinner" style="width: 32px; height: 32px; border-width: 3px;"></div>
    <p style="margin-top: var(--space-4); color: var(--color-text-muted);">
      Bearing witness... observing system state across all domains.
    </p>
  `;
  container.appendChild(loading);
}

function renderErrorState(container: HTMLElement, message: string): void {
  const err = document.createElement('div');
  err.className = 'witness-error';
  err.innerHTML = `
    <div class="witness-error-icon">\u{26A0}</div>
    <h3>Witness Failed</h3>
    <pre style="font-family: var(--font-mono); font-size: var(--text-xs); max-height: 200px; overflow: auto;
      background: var(--color-bg-alt); padding: var(--space-3); border-radius: var(--border-radius);">${escapeHtml(message)}</pre>
  `;
  container.appendChild(err);
}

// =============================================================================
// Dashboard Renderer
// =============================================================================

function renderDashboard(container: HTMLElement, report: WitnessReport): void {
  // ── Health Banner ───────────────────────────────────────────
  container.appendChild(renderHealthBanner(report));

  // ── Metadata ────────────────────────────────────────────────
  container.appendChild(renderMetadata(report));

  // ── Totals Bar ──────────────────────────────────────────────
  container.appendChild(renderTotals(report));

  // ── Domain Cards ────────────────────────────────────────────
  const domainSection = document.createElement('div');
  domainSection.style.marginBottom = 'var(--space-6)';

  const domainTitle = document.createElement('h3');
  domainTitle.style.cssText = 'font-family: var(--font-heading); margin-bottom: var(--space-4);';
  domainTitle.textContent = 'Domain Health';
  domainSection.appendChild(domainTitle);

  const grid = document.createElement('div');
  grid.className = 'witness-domain-grid';

  // Sort worst-first: fail > warn > skipped > pass.
  const healthOrder: Record<DomainHealth, number> = { fail: 0, warn: 1, skipped: 2, pass: 3 };
  const sorted = [...report.summary].sort((a, b) => healthOrder[a.health] - healthOrder[b.health]);

  for (const summary of sorted) {
    grid.appendChild(renderDomainCard(summary));
  }

  domainSection.appendChild(grid);
  container.appendChild(domainSection);

  // ── Recommendations ─────────────────────────────────────────
  if (report.recommendations.length > 0) {
    container.appendChild(renderRecommendations(report.recommendations));
  }
}

// =============================================================================
// Health Banner
// =============================================================================

function renderHealthBanner(report: WitnessReport): HTMLElement {
  const healthInfo = HEALTH_MAP[report.health];
  const banner = document.createElement('div');
  banner.className = 'witness-health-banner';

  // Large health circle.
  const circle = document.createElement('div');
  circle.className = 'witness-health-circle';
  circle.style.background = `var(${healthInfo.cssVar})`;

  const hebrewLabel = document.createElement('div');
  hebrewLabel.className = 'witness-health-hebrew';
  hebrewLabel.textContent = healthInfo.hebrew;
  circle.appendChild(hebrewLabel);

  const valueLabel = document.createElement('div');
  valueLabel.className = 'witness-health-value';
  const valueMap: Record<SystemHealth, string> = { healthy: '+1', degraded: '0', failing: '-1' };
  valueLabel.textContent = valueMap[report.health];
  circle.appendChild(valueLabel);

  banner.appendChild(circle);

  // Right side: label + subtitle.
  const text = document.createElement('div');
  text.className = 'witness-health-text';

  const label = document.createElement('div');
  label.className = 'witness-health-label';
  label.textContent = `System ${healthInfo.label}`;
  text.appendChild(label);

  const sub = document.createElement('div');
  sub.className = 'witness-health-sub';
  const executed = report.metadata.domainsExecuted.length;
  const requested = report.metadata.domainsRequested.length;
  sub.textContent = `${executed} of ${requested} domain${requested !== 1 ? 's' : ''} observed`;
  text.appendChild(sub);

  banner.appendChild(text);

  return banner;
}

// =============================================================================
// Metadata Row
// =============================================================================

function renderMetadata(report: WitnessReport): HTMLElement {
  const row = document.createElement('div');
  row.className = 'witness-metadata';

  const items: Array<{ label: string; value: string }> = [
    { label: 'Timestamp', value: formatTimestamp(report.metadata.timestamp) },
    { label: 'Duration', value: formatDuration(report.metadata.duration) },
    { label: 'Config', value: `v${report.metadata.configVersion}` },
    { label: 'Builder', value: `v${report.metadata.builderVersion}` },
    { label: 'Schema', value: `v${report.metadata.schemaVersion}` },
  ];

  for (const item of items) {
    const el = document.createElement('div');
    el.className = 'witness-metadata-item';

    const lbl = document.createElement('span');
    lbl.className = 'witness-metadata-label';
    lbl.textContent = item.label;
    el.appendChild(lbl);

    const val = document.createElement('span');
    val.className = 'witness-metadata-value';
    val.textContent = item.value;
    el.appendChild(val);

    row.appendChild(el);
  }

  return row;
}

// =============================================================================
// Totals Bar
// =============================================================================

function renderTotals(report: WitnessReport): HTMLElement {
  const section = document.createElement('div');
  section.style.marginBottom = 'var(--space-6)';

  const totals = report.totals;
  const total = totals.errors + totals.warnings + totals.info + totals.passes;

  // Summary cards.
  const grid = document.createElement('div');
  grid.className = 'witness-totals-grid';

  const segments: Array<{ label: string; count: number; color: string }> = [
    { label: 'Errors',   count: totals.errors,   color: 'var(--color-error)' },
    { label: 'Warnings', count: totals.warnings,  color: 'var(--color-warning)' },
    { label: 'Info',     count: totals.info,      color: 'var(--color-info)' },
    { label: 'Passes',   count: totals.passes,    color: 'var(--color-success)' },
  ];

  for (const seg of segments) {
    const card = document.createElement('div');
    card.className = 'witness-total-card';
    card.style.borderTopColor = seg.color;

    const count = document.createElement('div');
    count.className = 'witness-total-count';
    count.style.color = seg.color;
    count.textContent = String(seg.count);
    card.appendChild(count);

    const label = document.createElement('div');
    label.className = 'witness-total-label';
    label.textContent = seg.label;
    card.appendChild(label);

    grid.appendChild(card);
  }

  section.appendChild(grid);

  // Proportional bar.
  if (total > 0) {
    const bar = document.createElement('div');
    bar.className = 'witness-totals-bar';

    for (const seg of segments) {
      if (seg.count === 0) continue;
      const pct = (seg.count / total) * 100;
      const slice = document.createElement('div');
      slice.className = 'witness-totals-slice';
      slice.style.width = `${pct}%`;
      slice.style.background = seg.color;
      slice.title = `${seg.label}: ${seg.count} (${Math.round(pct)}%)`;
      bar.appendChild(slice);
    }

    section.appendChild(bar);
  }

  return section;
}

// =============================================================================
// Domain Cards
// =============================================================================

function renderDomainCard(summary: DomainSummary): HTMLElement {
  const card = document.createElement('div');
  card.className = 'witness-domain-card';
  card.style.borderLeftColor = DOMAIN_HEALTH_COLORS[summary.health];

  // Header row: icon + name + health badge.
  const header = document.createElement('div');
  header.className = 'witness-domain-header';

  const icon = document.createElement('span');
  icon.className = 'witness-domain-icon';
  icon.textContent = DOMAIN_ICONS[summary.domain] ?? '\u{1F4CB}';
  header.appendChild(icon);

  const name = document.createElement('span');
  name.className = 'witness-domain-name';
  name.textContent = summary.label;
  header.appendChild(name);

  const badge = document.createElement('span');
  badge.className = `witness-health-badge witness-health-${summary.health}`;
  badge.textContent = summary.health.toUpperCase();
  header.appendChild(badge);

  card.appendChild(header);

  // Counts row.
  if (summary.health !== 'skipped') {
    const counts = document.createElement('div');
    counts.className = 'witness-domain-counts';

    const entries = Object.entries(summary.counts).filter(([, v]) => v > 0);
    if (entries.length > 0) {
      for (const [key, val] of entries) {
        const chip = document.createElement('span');
        chip.className = `witness-count-chip witness-count-${key}`;
        chip.textContent = `${val} ${key}`;
        counts.appendChild(chip);
      }
    } else {
      const clean = document.createElement('span');
      clean.style.cssText = 'font-size: var(--text-xs); color: var(--color-text-muted);';
      clean.textContent = 'No findings';
      counts.appendChild(clean);
    }

    card.appendChild(counts);
  }

  // Duration.
  const duration = document.createElement('div');
  duration.className = 'witness-domain-duration';
  if (summary.health === 'skipped') {
    duration.textContent = summary.error ?? 'Skipped';
  } else {
    duration.textContent = formatDuration(summary.duration);
  }
  card.appendChild(duration);

  return card;
}

// =============================================================================
// Recommendations
// =============================================================================

function renderRecommendations(recs: Recommendation[]): HTMLElement {
  const section = document.createElement('div');
  section.style.marginBottom = 'var(--space-6)';

  const title = document.createElement('h3');
  title.style.cssText = 'font-family: var(--font-heading); margin-bottom: var(--space-4);';
  title.textContent = 'Recommendations';
  section.appendChild(title);

  // Sort by priority (1 = critical first).
  const sorted = [...recs].sort((a, b) => a.priority - b.priority);

  const list = document.createElement('div');
  list.className = 'witness-rec-list';

  for (const rec of sorted) {
    const row = document.createElement('div');
    row.className = 'witness-rec-item';

    // Priority badge.
    const priBadge = document.createElement('span');
    priBadge.className = `witness-rec-priority witness-rec-pri-${priorityClass(rec.priority)}`;
    priBadge.textContent = `P${rec.priority}`;
    row.appendChild(priBadge);

    // Domain badge.
    const domBadge = document.createElement('span');
    domBadge.className = 'witness-rec-domain';
    domBadge.textContent = rec.domain;
    row.appendChild(domBadge);

    // Message.
    const msg = document.createElement('span');
    msg.className = 'witness-rec-message';
    msg.textContent = rec.message;
    row.appendChild(msg);

    // File path (if present).
    if (rec.file) {
      const file = document.createElement('span');
      file.className = 'witness-rec-file';
      file.textContent = rec.file;
      file.title = rec.file;
      row.appendChild(file);
    }

    // Action button (if present).
    if (rec.action) {
      const actionBtn = document.createElement('button');
      actionBtn.className = 'btn witness-rec-action-btn';
      actionBtn.textContent = 'Run';
      actionBtn.title = rec.action;
      actionBtn.addEventListener('click', () => {
        // Copy the action command to clipboard for the user.
        navigator.clipboard.writeText(rec.action!).then(() => {
          showToast({ message: 'Command copied to clipboard.', type: 'info', duration: 2000 });
        }).catch(() => {
          showToast({ message: rec.action!, type: 'info', duration: 5000 });
        });
      });
      row.appendChild(actionBtn);
    }

    list.appendChild(row);
  }

  section.appendChild(list);

  return section;
}

// =============================================================================
// Helpers
// =============================================================================

function priorityClass(p: number): string {
  if (p <= 2) return 'critical';
  if (p <= 3) return 'warning';
  return 'info';
}

function formatDuration(ms: number): string {
  if (ms < 1000) return `${ms}ms`;
  const secs = (ms / 1000).toFixed(1);
  return `${secs}s`;
}

function formatTimestamp(iso: string): string {
  try {
    const d = new Date(iso);
    return d.toLocaleString(undefined, {
      month: 'short', day: 'numeric', year: 'numeric',
      hour: '2-digit', minute: '2-digit',
    });
  } catch {
    return iso;
  }
}

function escapeHtml(text: string): string {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}
