/**
 * Analysis View — Page Fill command (measure, predict, index, cross-check).
 */

import { createCommandCard, runCliCommand } from '../shared/runner.js';
import { createRadioGroup } from '../shared/form-controls.js';
import { createFilePicker } from '../shared/file-picker.js';

export function createAnalysisView(): HTMLElement {
  const el = document.createElement('div');

  const h2 = document.createElement('h2');
  h2.textContent = 'Analysis';
  el.appendChild(h2);

  // ── Page Fill ─────────────────────────────────────────────────
  const modeRadio = createRadioGroup('fill-mode', 'Mode', [
    { value: 'measure', label: 'Measure', description: 'Measure actual page fill from PDF' },
    { value: 'predict', label: 'Predict', description: 'Estimate page fill from source' },
    { value: 'index', label: 'Index', description: 'Build fill index for all sections' },
    { value: 'cross-check', label: 'Cross-Check', description: 'Compare prediction vs measurement' },
  ]);
  const filePicker = createFilePicker({
    label: 'PDF file (for measure/cross-check)',
    placeholder: 'Path to rendered PDF...',
    filters: [{ name: 'PDF', extensions: ['pdf'] }],
  });

  const controls = document.createElement('div');
  controls.className = 'command-options';
  controls.appendChild(modeRadio.wrapper);
  controls.appendChild(filePicker.element);

  // Show/hide file picker based on mode.
  modeRadio.onChange((mode) => {
    filePicker.element.style.display =
      (mode === 'measure' || mode === 'cross-check') ? '' : 'none';
  });

  el.appendChild(createCommandCard({
    title: 'Page Fill Analysis',
    description: 'Analyze page fill levels across sections.',
    controls,
    onRun: () => {
      const mode = modeRadio.getValue();
      const args: string[] = ['--' + mode];
      if (mode === 'measure' || mode === 'cross-check') {
        const file = filePicker.getValue();
        if (file) args.push(file);
      }
      runCliCommand('fill', args);
    },
  }));

  return el;
}
