/**
 * Build View — Build, Section, Article, Part, Pages, Editorial commands.
 */

import { createCommandCard, runCliCommand, runMakeTarget } from '../shared/runner.js';
import { createSelect, createToggle, createTextInput } from '../shared/form-controls.js';
import { createFilePicker } from '../shared/file-picker.js';

export function createBuildView(): HTMLElement {
  const el = document.createElement('div');

  const h2 = document.createElement('h2');
  h2.textContent = 'Build';
  el.appendChild(h2);

  // ── Build Book ────────────────────────────────────────────────
  const formatSelect = createSelect('Format', [
    { value: 'pdf', label: 'PDF' },
    { value: 'epub', label: 'EPUB' },
  ]);
  const parallelToggle = createToggle('build-parallel', 'Parallel builds');
  const cleanToggle = createToggle('build-clean', 'Clean first');
  const forceToggle = createToggle('build-force', 'Force rebuild');

  const buildControls = document.createElement('div');
  buildControls.className = 'command-options';
  buildControls.appendChild(formatSelect.wrapper);
  buildControls.appendChild(parallelToggle.wrapper);
  buildControls.appendChild(cleanToggle.wrapper);
  buildControls.appendChild(forceToggle.wrapper);

  el.appendChild(createCommandCard({
    title: 'Build Book',
    description: 'Build the complete manual in the selected format.',
    controls: buildControls,
    onRun: () => {
      const vars: Record<string, string> = {};
      const fmt = formatSelect.getValue();
      if (fmt !== 'pdf') vars['FORMAT'] = fmt;
      if (parallelToggle.isChecked()) vars['PARALLEL'] = '1';
      if (cleanToggle.isChecked()) vars['CLEAN'] = '1';
      if (forceToggle.isChecked()) vars['FORCE'] = '1';
      runMakeTarget('book', vars);
    },
  }));

  // ── Build Section ─────────────────────────────────────────────
  const sectionPicker = createFilePicker({
    label: 'Section file',
    placeholder: 'Path to .adoc file...',
    filters: [{ name: 'AsciiDoc', extensions: ['adoc'] }],
  });
  const sectionForce = createToggle('section-force', 'Force rebuild');

  const sectionControls = document.createElement('div');
  sectionControls.className = 'command-options';
  sectionControls.appendChild(sectionPicker.element);
  sectionControls.appendChild(sectionForce.wrapper);

  el.appendChild(createCommandCard({
    title: 'Build Section',
    description: 'Build a single section file.',
    controls: sectionControls,
    onRun: () => {
      const file = sectionPicker.getValue();
      if (!file) { alert('Select a section file first.'); return; }
      const vars: Record<string, string> = { FILE: file };
      if (sectionForce.isChecked()) vars['FORCE'] = '1';
      runMakeTarget('section', vars);
    },
  }));

  // ── Build Article ─────────────────────────────────────────────
  const articleInput = createTextInput('article-id', 'Article ID');
  const articleForce = createToggle('article-force', 'Force rebuild');

  const articleControls = document.createElement('div');
  articleControls.className = 'command-options';
  articleControls.appendChild(articleInput.wrapper);
  articleControls.appendChild(articleForce.wrapper);

  el.appendChild(createCommandCard({
    title: 'Build Article',
    description: 'Build a single article by ID.',
    controls: articleControls,
    onRun: () => {
      const id = articleInput.getValue();
      if (!id) { alert('Enter an article ID.'); return; }
      const args = [id];
      if (articleForce.isChecked()) args.push('--force');
      runCliCommand('article', args);
    },
  }));

  // ── Build Part ────────────────────────────────────────────────
  const partInput = createTextInput('part-id', 'Part ID');
  const partForce = createToggle('part-force', 'Force rebuild');

  const partControls = document.createElement('div');
  partControls.className = 'command-options';
  partControls.appendChild(partInput.wrapper);
  partControls.appendChild(partForce.wrapper);

  el.appendChild(createCommandCard({
    title: 'Build Part',
    description: 'Build a single part by ID.',
    controls: partControls,
    onRun: () => {
      const id = partInput.getValue();
      if (!id) { alert('Enter a part ID.'); return; }
      const args = [id];
      if (partForce.isChecked()) args.push('--force');
      runCliCommand('part', args);
    },
  }));

  // ── Pages ─────────────────────────────────────────────────────
  const pagesPicker = createFilePicker({
    label: 'Section file',
    placeholder: 'Path to .adoc source...',
    filters: [{ name: 'AsciiDoc', extensions: ['adoc'] }],
  });
  const pagesRange = createTextInput('pages-range', 'Page range', 'e.g., 1-5 or 3,7,12-15');

  const pagesControls = document.createElement('div');
  pagesControls.className = 'command-options';
  pagesControls.appendChild(pagesPicker.element);
  pagesControls.appendChild(pagesRange.wrapper);

  el.appendChild(createCommandCard({
    title: 'Extract Pages',
    description: 'Extract a page range from a section PDF via qpdf.',
    controls: pagesControls,
    onRun: () => {
      const file = pagesPicker.getValue();
      const range = pagesRange.getValue();
      if (!file || !range) { alert('File and page range required.'); return; }
      runCliCommand('pages', [file, '--pages', range]);
    },
  }));

  // ── Editorial ─────────────────────────────────────────────────
  const editorialForce = createToggle('editorial-force', 'Force rebuild');
  const editorialControls = document.createElement('div');
  editorialControls.className = 'command-options';
  editorialControls.appendChild(editorialForce.wrapper);

  el.appendChild(createCommandCard({
    title: 'Build Editorial',
    description: 'Generate the editorial content.',
    controls: editorialControls,
    onRun: () => {
      const args: string[] = [];
      if (editorialForce.isChecked()) args.push('--force');
      runCliCommand('editorial', args);
    },
  }));

  return el;
}
