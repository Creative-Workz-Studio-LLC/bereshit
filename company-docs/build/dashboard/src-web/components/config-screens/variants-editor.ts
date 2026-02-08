/**
 * Theme Variants Editor — §19 build.config.yaml
 *
 * Per-theme override panels. Dynamic — theme names are keys in the config.
 * Each theme can override: body_size, depth, supporting, font_assignments,
 * alignment, page_layout.
 */

import type { EditorDefinition } from './editor-base.js';
import {
  configInput,
  configNumber,
  configKeyValue,
  configSection,
  configTabs,
} from './editor-base.js';

/* eslint-disable @typescript-eslint/no-explicit-any */
type AnyData = Record<string, any>;

function buildThemePanel(themeName: string, overrides: AnyData): { el: HTMLElement; getData: () => AnyData } {
  const el = document.createElement('div');

  // Body size override.
  const bodySize = configNumber(
    'Body Size Override',
    overrides.body_size ?? 0,
    { step: 0.5, unit: 'pt (0 = use default)' },
  );
  el.appendChild(bodySize.wrapper);

  // Depth overrides.
  el.appendChild(configSection('Depth Overrides'));
  const depthGetters: Array<{ key: string; getValue: () => number }> = [];
  const depth = overrides.depth ?? {};
  for (const [key, val] of Object.entries(depth as Record<string, AnyData>)) {
    const fs = configNumber(`${key} font size`, val?.font_size ?? 0, { step: 0.5, unit: 'pt' });
    el.appendChild(fs.wrapper);
    depthGetters.push({ key, getValue: fs.getValue });
  }

  // Font assignment overrides.
  el.appendChild(configSection('Font Assignment Overrides'));
  const fontAssignments = configKeyValue('Overrides', overrides.font_assignments ?? {});
  el.appendChild(fontAssignments.wrapper);

  // Alignment overrides.
  el.appendChild(configSection('Alignment Overrides'));
  const alignment = configKeyValue('Overrides', overrides.alignment ?? {});
  el.appendChild(alignment.wrapper);

  // Page layout overrides.
  el.appendChild(configSection('Page Layout Overrides'));
  const pageLayout = configKeyValue('Overrides', overrides.page_layout ?? {});
  el.appendChild(pageLayout.wrapper);

  return {
    el,
    getData: () => {
      const result: AnyData = {};
      const bs = bodySize.getValue();
      if (bs > 0) result.body_size = bs;

      if (depthGetters.length > 0) {
        result.depth = {};
        for (const g of depthGetters) result.depth[g.key] = { font_size: g.getValue() };
      }

      const fa = fontAssignments.getValue();
      if (Object.keys(fa).length > 0) result.font_assignments = fa;

      const al = alignment.getValue();
      if (Object.keys(al).length > 0) result.alignment = al;

      const pl = pageLayout.getValue();
      if (Object.keys(pl).length > 0) result.page_layout = pl;

      return result;
    },
  };
}

export const variantsEditor: EditorDefinition = {
  sectionKey: 'theme_variants',
  title: 'Theme Variants',
  description: 'Per-theme overrides for typography, alignment, and page layout.',

  buildForm(data: unknown) {
    const d = (data ?? {}) as AnyData;
    const el = document.createElement('div');

    const themes = Object.keys(d);
    if (themes.length === 0) {
      const empty = document.createElement('p');
      empty.style.color = 'var(--color-text-muted)';
      empty.textContent = 'No theme variants configured. Themes are defined in build.config.yaml §19.';
      el.appendChild(empty);
      return { element: el, getData: () => ({}) };
    }

    const panels: Array<{ key: string; panel: ReturnType<typeof buildThemePanel> }> = [];

    const tabs = configTabs(
      themes.map((themeName) => {
        const panel = buildThemePanel(themeName, d[themeName] ?? {});
        panels.push({ key: themeName, panel });
        return {
          key: themeName,
          label: themeName.replace(/-/g, ' ').replace(/\b\w/g, (c) => c.toUpperCase()),
          content: panel.el,
        };
      }),
    );
    el.appendChild(tabs);

    return {
      element: el,
      getData: () => {
        const result: AnyData = {};
        for (const p of panels) result[p.key] = p.panel.getData();
        return result;
      },
    };
  },
};
