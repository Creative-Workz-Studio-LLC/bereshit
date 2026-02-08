/**
 * Page Layout Editor — §12 build.config.yaml
 *
 * Dimension inputs for margins, page size, header/footer heights.
 */

import type { EditorDefinition } from './editor-base.js';
import { configInput, configNumber, configColor, configSection } from './editor-base.js';

interface PageLayoutData {
  page_size?: string;
  page_layout?: string;
  margin_top?: string;
  margin_right?: string;
  margin_bottom?: string;
  margin_left?: string;
  margin_inner?: string;
  margin_outer?: string;
  header_height?: string;
  footer_height?: string;
  vertical_spacing?: number;
  thematic_break_margin_top?: number;
  thematic_break_margin_bottom?: number;
  heading_min_height_after?: string;
  heading_margin_top?: number;
  heading_margin_bottom?: number;
  heading_line_height?: number;
  thematic_break_border_width?: number;
  thematic_break_border_color?: string;
}

export const pageLayoutEditor: EditorDefinition = {
  sectionKey: 'page_layout',
  title: 'Page Layout',
  description: 'Page dimensions, margins, and spacing.',

  buildForm(data: unknown) {
    const d = (data ?? {}) as PageLayoutData;
    const el = document.createElement('div');

    // ── Page ──────────────────────────────────────────────────────
    el.appendChild(configSection('Page'));
    const pageSize = configInput('Page Size', d.page_size ?? 'Letter', { placeholder: 'Letter, A4, etc.' });
    const pageLayout = configInput('Page Layout', d.page_layout ?? 'portrait', { placeholder: 'portrait or landscape' });
    el.appendChild(pageSize.wrapper);
    el.appendChild(pageLayout.wrapper);

    // ── Margins ───────────────────────────────────────────────────
    el.appendChild(configSection('Margins'));
    const grid = document.createElement('div');
    grid.style.cssText = 'display: grid; grid-template-columns: 1fr 1fr; gap: var(--space-3);';

    const mTop = configInput('Top', d.margin_top ?? '', { placeholder: '0.75in' });
    const mRight = configInput('Right', d.margin_right ?? '', { placeholder: '0.67in' });
    const mBottom = configInput('Bottom', d.margin_bottom ?? '', { placeholder: '0.75in' });
    const mLeft = configInput('Left', d.margin_left ?? '', { placeholder: '0.67in' });
    const mInner = configInput('Inner', d.margin_inner ?? '', { placeholder: '0.75in' });
    const mOuter = configInput('Outer', d.margin_outer ?? '', { placeholder: '0.59in' });

    grid.appendChild(mTop.wrapper);
    grid.appendChild(mBottom.wrapper);
    grid.appendChild(mLeft.wrapper);
    grid.appendChild(mRight.wrapper);
    grid.appendChild(mInner.wrapper);
    grid.appendChild(mOuter.wrapper);
    el.appendChild(grid);

    // ── Header / Footer ───────────────────────────────────────────
    el.appendChild(configSection('Header & Footer'));
    const headerH = configInput('Header Height', d.header_height ?? '', { placeholder: '0.5in' });
    const footerH = configInput('Footer Height', d.footer_height ?? '', { placeholder: '0.5in' });
    el.appendChild(headerH.wrapper);
    el.appendChild(footerH.wrapper);

    // ── Spacing ───────────────────────────────────────────────────
    el.appendChild(configSection('Spacing'));
    const vSpacing = configNumber('Vertical Spacing', d.vertical_spacing ?? 0);
    const hMarginTop = configNumber('Heading Margin Top', d.heading_margin_top ?? 0);
    const hMarginBottom = configNumber('Heading Margin Bottom', d.heading_margin_bottom ?? 0);
    const hLineHeight = configNumber('Heading Line Height', d.heading_line_height ?? 1.2, { step: 0.05 });
    const hMinAfter = configInput('Heading Min Height After', d.heading_min_height_after ?? '');
    el.appendChild(vSpacing.wrapper);
    el.appendChild(hMarginTop.wrapper);
    el.appendChild(hMarginBottom.wrapper);
    el.appendChild(hLineHeight.wrapper);
    el.appendChild(hMinAfter.wrapper);

    // ── Thematic Break ────────────────────────────────────────────
    el.appendChild(configSection('Thematic Break'));
    const tbMarginTop = configNumber('Margin Top', d.thematic_break_margin_top ?? 0);
    const tbMarginBottom = configNumber('Margin Bottom', d.thematic_break_margin_bottom ?? 0);
    const tbBorderWidth = configNumber('Border Width', d.thematic_break_border_width ?? 0, { step: 0.25 });
    const tbBorderColor = configColor('Border Color', d.thematic_break_border_color ?? '#E2E8F0');
    el.appendChild(tbMarginTop.wrapper);
    el.appendChild(tbMarginBottom.wrapper);
    el.appendChild(tbBorderWidth.wrapper);
    el.appendChild(tbBorderColor.wrapper);

    return {
      element: el,
      getData: () => ({
        page_size: pageSize.getValue(),
        page_layout: pageLayout.getValue(),
        margin_top: mTop.getValue(),
        margin_right: mRight.getValue(),
        margin_bottom: mBottom.getValue(),
        margin_left: mLeft.getValue(),
        margin_inner: mInner.getValue(),
        margin_outer: mOuter.getValue(),
        header_height: headerH.getValue(),
        footer_height: footerH.getValue(),
        vertical_spacing: vSpacing.getValue(),
        heading_margin_top: hMarginTop.getValue(),
        heading_margin_bottom: hMarginBottom.getValue(),
        heading_line_height: hLineHeight.getValue(),
        heading_min_height_after: hMinAfter.getValue(),
        thematic_break_margin_top: tbMarginTop.getValue(),
        thematic_break_margin_bottom: tbMarginBottom.getValue(),
        thematic_break_border_width: tbBorderWidth.getValue(),
        thematic_break_border_color: tbBorderColor.getValue(),
      }),
    };
  },
};
