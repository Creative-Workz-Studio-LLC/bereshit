/**
 * Brand Colors Editor — §10 build.config.yaml
 *
 * Color pickers for the 5 CWS brand colors with live preview + contrast.
 */

import type { EditorDefinition } from './editor-base.js';
import { configColor, configSection } from './editor-base.js';

interface BrandData {
  colors?: {
    deep_blue?: string;
    medium_blue?: string;
    gold?: string;
    dark?: string;
    light?: string;
  };
}

function hexToRgb(hex: string): [number, number, number] | null {
  const m = /^#([0-9A-Fa-f]{6})$/.exec(hex);
  if (!m) return null;
  return [
    parseInt(m[1].slice(0, 2), 16),
    parseInt(m[1].slice(2, 4), 16),
    parseInt(m[1].slice(4, 6), 16),
  ];
}

function luminance(r: number, g: number, b: number): number {
  const a = [r, g, b].map((v) => {
    v /= 255;
    return v <= 0.03928 ? v / 12.92 : Math.pow((v + 0.055) / 1.055, 2.4);
  });
  return a[0] * 0.2126 + a[1] * 0.7152 + a[2] * 0.0722;
}

function contrastRatio(hex1: string, hex2: string): string {
  const c1 = hexToRgb(hex1);
  const c2 = hexToRgb(hex2);
  if (!c1 || !c2) return '--';
  const l1 = luminance(...c1);
  const l2 = luminance(...c2);
  const ratio = (Math.max(l1, l2) + 0.05) / (Math.min(l1, l2) + 0.05);
  return ratio.toFixed(1) + ':1';
}

export const brandEditor: EditorDefinition = {
  sectionKey: 'brand_colors',
  title: 'Brand Colors',
  description: 'CWS brand color palette with live contrast ratios.',

  buildForm(data: unknown) {
    const d = (data ?? {}) as BrandData;
    const colors = d.colors ?? {};
    const el = document.createElement('div');

    const deep = configColor('Deep Blue', colors.deep_blue ?? '#1A365D');
    const medium = configColor('Medium Blue', colors.medium_blue ?? '#2C5282');
    const gold = configColor('Gold', colors.gold ?? '#D69E2E');
    const dark = configColor('Dark', colors.dark ?? '#2D3748');
    const light = configColor('Light', colors.light ?? '#F7FAFC');

    el.appendChild(deep.wrapper);
    el.appendChild(medium.wrapper);
    el.appendChild(gold.wrapper);
    el.appendChild(dark.wrapper);
    el.appendChild(light.wrapper);

    // ── Contrast Preview ────────────────────────────────────────
    el.appendChild(configSection('Contrast Ratios'));
    const contrastGrid = document.createElement('div');
    contrastGrid.style.cssText = 'display: grid; grid-template-columns: repeat(auto-fill, minmax(200px, 1fr)); gap: var(--space-3);';
    el.appendChild(contrastGrid);

    function updateContrast(): void {
      const d = deep.getValue();
      const l = light.getValue();
      const g = gold.getValue();
      const dk = dark.getValue();

      contrastGrid.innerHTML = '';
      const pairs: [string, string, string][] = [
        ['Deep Blue on Light', d, l],
        ['Gold on Deep Blue', g, d],
        ['Dark on Light', dk, l],
        ['Light on Dark', l, dk],
      ];
      for (const [label, fg, bg] of pairs) {
        const card = document.createElement('div');
        card.style.cssText = `padding: var(--space-3); border-radius: var(--border-radius); border: 1px solid var(--color-border);`;
        card.style.background = bg;
        card.style.color = fg;

        const ratio = contrastRatio(fg, bg);
        const val = parseFloat(ratio);
        const passAA = val >= 4.5;

        card.innerHTML = `
          <div style="font-weight: 600; margin-bottom: var(--space-1);">${label}</div>
          <div style="font-size: var(--text-sm);">Ratio: ${ratio} ${passAA ? '(AA Pass)' : '(AA Fail)'}</div>
        `;
        contrastGrid.appendChild(card);
      }
    }

    updateContrast();
    el.addEventListener('input', updateContrast);

    return {
      element: el,
      getData: () => ({
        colors: {
          deep_blue: deep.getValue(),
          medium_blue: medium.getValue(),
          gold: gold.getValue(),
          dark: dark.getValue(),
          light: light.getValue(),
        },
      }),
    };
  },
};
