/**
 * Validate View — Structure, Lint, TOC, Witness commands.
 */

import { createCommandCard, runCliCommand } from '../shared/runner.js';
import { createSelect, createToggle, createCheckboxGroup, createRadioGroup } from '../shared/form-controls.js';
import { createFilePicker } from '../shared/file-picker.js';

export function createValidateView(): HTMLElement {
  const el = document.createElement('div');

  const h2 = document.createElement('h2');
  h2.textContent = 'Validate';
  el.appendChild(h2);

  // ── Structure Validation ──────────────────────────────────────
  const typeSelect = createSelect('Type', [
    { value: '', label: '(all types)' },
    { value: 'manual', label: 'Manual' },
    { value: 'manual-l', label: 'Manual-Legal' },
    { value: 'operational', label: 'Operational' },
  ]);
  const bonusToggle = createToggle('validate-bonus', 'Include bonus checks');
  const reportToggle = createToggle('validate-report', 'JSON report output');

  const structControls = document.createElement('div');
  structControls.className = 'command-options';
  structControls.appendChild(typeSelect.wrapper);
  structControls.appendChild(bonusToggle.wrapper);
  structControls.appendChild(reportToggle.wrapper);

  el.appendChild(createCommandCard({
    title: 'Structure Validation',
    description: 'Validate document structure against type rules.',
    controls: structControls,
    onRun: () => {
      const args: string[] = [];
      const type = typeSelect.getValue();
      if (type) args.push('--type', type);
      if (bonusToggle.isChecked()) args.push('--bonus');
      if (reportToggle.isChecked()) args.push('--report');
      runCliCommand('validate', args);
    },
  }));

  // ── Editorial Lint ────────────────────────────────────────────
  const lintFile = createFilePicker({
    label: 'File (optional)',
    placeholder: 'Omit for all files...',
    filters: [{ name: 'AsciiDoc', extensions: ['adoc'] }],
  });
  const lintScope = createSelect('Scope', [
    { value: '', label: '(all scopes)' },
    { value: 'per-file', label: 'Per-file checks only' },
    { value: 'cross-file', label: 'Cross-file checks only' },
  ]);
  const lintCategory = createSelect('Category', [
    { value: '', label: '(all categories)' },
    { value: 'editorial', label: 'Editorial' },
    { value: 'style', label: 'Style' },
    { value: 'structure', label: 'Structure' },
    { value: 'legal', label: 'Legal' },
    { value: 'tone', label: 'Tone' },
    { value: 'industry', label: 'Industry' },
    { value: 'conversion', label: 'Conversion' },
  ]);
  const lintVerbose = createToggle('lint-verbose', 'Verbose (show all findings)');
  const lintReport = createToggle('lint-report', 'JSON witness report');

  const lintControls = document.createElement('div');
  lintControls.className = 'command-options';
  lintControls.appendChild(lintFile.element);
  lintControls.appendChild(lintScope.wrapper);
  lintControls.appendChild(lintCategory.wrapper);
  lintControls.appendChild(lintVerbose.wrapper);
  lintControls.appendChild(lintReport.wrapper);

  el.appendChild(createCommandCard({
    title: 'Editorial Lint',
    description: '57 editorial checks across 7 categories.',
    controls: lintControls,
    onRun: () => {
      const args: string[] = [];
      const file = lintFile.getValue();
      if (file) args.push('--file', file);
      const scope = lintScope.getValue();
      if (scope) args.push('--scope', scope);
      const cat = lintCategory.getValue();
      if (cat) args.push('--category', cat);
      if (lintVerbose.isChecked()) args.push('--verbose');
      if (lintReport.isChecked()) args.push('--report');
      runCliCommand('lint', args);
    },
  }));

  // ── Bible Citation ────────────────────────────────────────────
  const bibleFile = createFilePicker({
    label: 'File (optional)',
    placeholder: 'Omit for all files...',
    filters: [{ name: 'AsciiDoc', extensions: ['adoc'] }],
  });
  const bibleControls = document.createElement('div');
  bibleControls.className = 'command-options';
  bibleControls.appendChild(bibleFile.element);

  el.appendChild(createCommandCard({
    title: 'Bible Citation Check',
    description: 'Validate Bible verse references in source files.',
    controls: bibleControls,
    onRun: () => {
      const args = ['--bible'];
      const file = bibleFile.getValue();
      if (file) args.push('--file', file);
      runCliCommand('lint', args);
    },
  }));

  // ── TOC ───────────────────────────────────────────────────────
  const tocMode = createRadioGroup('toc-mode', 'Mode', [
    { value: 'display', label: 'Display', description: 'Show TOC markers and page numbers' },
    { value: 'verify', label: 'Verify', description: 'Check markers against actual pages' },
    { value: 'fill', label: 'Fill', description: 'Update markers with measured page numbers' },
  ]);
  const tocFile = createFilePicker({
    label: 'File (optional)',
    placeholder: 'Omit for all files...',
    filters: [{ name: 'AsciiDoc', extensions: ['adoc'] }],
  });

  const tocControls = document.createElement('div');
  tocControls.className = 'command-options';
  tocControls.appendChild(tocMode.wrapper);
  tocControls.appendChild(tocFile.element);

  el.appendChild(createCommandCard({
    title: 'Table of Contents',
    description: 'Display, verify, or fill TOC page markers.',
    controls: tocControls,
    onRun: () => {
      const mode = tocMode.getValue();
      const args = ['--' + mode];
      const file = tocFile.getValue();
      if (file) args.push('--file', file);
      runCliCommand('toc', args);
    },
  }));

  // ── Witness ───────────────────────────────────────────────────
  const witnessDomains = createCheckboxGroup('witness-domains', 'Domains', [
    { value: 'structural', label: 'Structural' },
    { value: 'editorial', label: 'Editorial' },
    { value: 'theme', label: 'Theme' },
    { value: 'bible', label: 'Bible' },
    { value: 'toc', label: 'TOC' },
  ]);
  const witnessVerbose = createToggle('witness-verbose', 'Verbose');
  const witnessJson = createToggle('witness-json', 'JSON output');

  const witnessControls = document.createElement('div');
  witnessControls.className = 'command-options';
  witnessControls.appendChild(witnessDomains.wrapper);
  witnessControls.appendChild(witnessVerbose.wrapper);
  witnessControls.appendChild(witnessJson.wrapper);

  el.appendChild(createCommandCard({
    title: 'Witness Report',
    description: 'Comprehensive system health check across 5 domains.',
    controls: witnessControls,
    onRun: () => {
      const args: string[] = [];
      const domains = witnessDomains.getSelected();
      if (domains.length > 0 && domains.length < 5) {
        args.push('--domains', domains.join(','));
      }
      if (witnessVerbose.isChecked()) args.push('--verbose');
      if (witnessJson.isChecked()) args.push('--json');
      runCliCommand('witness', args);
    },
  }));

  return el;
}
