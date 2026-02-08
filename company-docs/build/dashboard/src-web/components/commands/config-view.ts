/**
 * Config View — Configuration section grid and editor routing.
 *
 * Shows a grid of 13 config sections. Clicking a card opens the
 * section editor inline, replacing the grid with the editor form.
 * "Back" returns to the grid.
 */

import { createConfigEditor, type EditorDefinition } from '../config-screens/editor-base.js';
import {
  attributesEditor,
  formatsEditor,
  watchEditor,
  displayEditor,
  brandEditor,
  typographyEditor,
  pageLayoutEditor,
  legalEditor,
  toneEditor,
  docTypesEditor,
  companyEditor,
  variantsEditor,
  pageFillEditor,
} from '../config-screens/index.js';

/** All editors keyed by their section key. */
const editors: Record<string, EditorDefinition> = {
  attributes: attributesEditor,
  formats: formatsEditor,
  watch: watchEditor,
  display: displayEditor,
  brand_colors: brandEditor,
  typography: typographyEditor,
  page_layout: pageLayoutEditor,
  legal_language: legalEditor,
  tone_and_voice: toneEditor,
  document_types: docTypesEditor,
  company_conventions: companyEditor,
  theme_variants: variantsEditor,
  page_fill: pageFillEditor,
};

/** Section metadata for the overview grid. */
const sections = [
  { key: 'attributes', title: 'Attributes', desc: 'Custom AsciiDoc attributes', icon: 'A' },
  { key: 'formats', title: 'Formats', desc: 'Output format configuration (PDF, EPUB)', icon: 'F' },
  { key: 'watch', title: 'Watch', desc: 'File watching patterns and debounce', icon: 'W' },
  { key: 'display', title: 'Display', desc: 'Banner, colors, and icons', icon: 'D' },
  { key: 'brand_colors', title: 'Brand Colors', desc: 'CWS brand color palette', icon: 'B' },
  { key: 'typography', title: 'Typography', desc: 'Scale, depth, fonts, assignments, colors, styles', icon: 'T' },
  { key: 'page_layout', title: 'Page Layout', desc: 'Margins, page size, column widths', icon: 'P' },
  { key: 'legal_language', title: 'Legal Language', desc: 'Registers, obligation words, defined terms', icon: 'L' },
  { key: 'tone_and_voice', title: 'Tone & Voice', desc: 'Voice attributes, spectrum, replacements', icon: 'V' },
  { key: 'document_types', title: 'Document Types', desc: 'Type-to-register-to-theme mapping', icon: 'R' },
  { key: 'company_conventions', title: 'Company', desc: 'Board members, contact emails', icon: 'C' },
  { key: 'theme_variants', title: 'Theme Variants', desc: 'Per-theme override panels', icon: 'X' },
  { key: 'page_fill', title: 'Page Fill', desc: 'Range sliders, threshold inputs', icon: 'G' },
];

export function createConfigView(): HTMLElement {
  const container = document.createElement('div');

  function showGrid(): void {
    container.innerHTML = '';

    const h2 = document.createElement('h2');
    h2.textContent = 'Configuration';
    container.appendChild(h2);

    const intro = document.createElement('p');
    intro.style.cssText = 'margin-bottom: var(--space-4); color: var(--color-text-muted);';
    intro.textContent = 'Edit build.config.yaml sections. Changes are saved to disk and can regenerate config.mk.';
    container.appendChild(intro);

    const grid = document.createElement('div');
    grid.style.cssText = 'display: grid; grid-template-columns: repeat(auto-fill, minmax(280px, 1fr)); gap: var(--space-4);';

    for (const section of sections) {
      const card = document.createElement('div');
      card.className = 'card';
      card.style.cssText = 'cursor: pointer; transition: border-color var(--transition-fast), box-shadow var(--transition-fast);';

      card.addEventListener('mouseenter', () => {
        card.style.borderColor = 'var(--color-accent)';
        card.style.boxShadow = 'var(--shadow-md)';
      });
      card.addEventListener('mouseleave', () => {
        card.style.borderColor = '';
        card.style.boxShadow = '';
      });

      // Icon badge.
      const icon = document.createElement('div');
      icon.style.cssText = `
        display: inline-flex; align-items: center; justify-content: center;
        width: 36px; height: 36px; border-radius: 8px;
        background: var(--cws-deep-blue); color: white;
        font-family: var(--font-heading); font-weight: 700; font-size: var(--text-base);
        margin-bottom: var(--space-2);
      `;
      icon.textContent = section.icon;
      card.appendChild(icon);

      const title = document.createElement('h3');
      title.style.cssText = 'font-family: var(--font-heading); font-size: var(--text-base); font-weight: 600; margin-bottom: var(--space-1);';
      title.textContent = section.title;
      card.appendChild(title);

      const desc = document.createElement('p');
      desc.style.cssText = 'font-size: var(--text-sm); color: var(--color-text-muted);';
      desc.textContent = section.desc;
      card.appendChild(desc);

      card.addEventListener('click', () => showEditor(section.key));
      grid.appendChild(card);
    }

    container.appendChild(grid);
  }

  function showEditor(sectionKey: string): void {
    const def = editors[sectionKey];
    if (!def) return;

    container.innerHTML = '';
    const editor = createConfigEditor(def, showGrid);
    container.appendChild(editor);
  }

  showGrid();
  return container;
}
