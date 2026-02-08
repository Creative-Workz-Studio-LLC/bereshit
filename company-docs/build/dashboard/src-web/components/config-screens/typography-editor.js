/**
 * Typography Editor — §11 build.config.yaml
 *
 * Tabbed editor: Scale, Depth, Fonts, Assignments, Colors, Styles, Legal, Elements.
 */
import { configNumber, configInput, configColor, configSelect, configKeyValue, configTabs, } from './editor-base.js';
function buildScaleTab(d) {
    const el = document.createElement('div');
    const bodySize = configNumber('Body Size', d.body_size ?? 10.5, { step: 0.5, unit: 'pt' });
    const scaleRatio = configNumber('Scale Ratio', d.scale_ratio ?? 1.125, { step: 0.025 });
    const lineHeight = configNumber('Line Height', d.line_height ?? 1.5, { step: 0.05 });
    el.appendChild(bodySize.wrapper);
    el.appendChild(scaleRatio.wrapper);
    el.appendChild(lineHeight.wrapper);
    return { el, getData: () => ({ body_size: bodySize.getValue(), scale_ratio: scaleRatio.getValue(), line_height: lineHeight.getValue() }) };
}
function buildDepthTab(d) {
    const el = document.createElement('div');
    const depth = d.depth ?? {};
    const getters = [];
    for (const [key, val] of Object.entries(depth)) {
        const fs = configNumber(`${key} — Font Size`, val?.font_size ?? 10.5, { step: 0.5, unit: 'pt' });
        el.appendChild(fs.wrapper);
        getters.push({ key, fontSize: fs.getValue });
    }
    return {
        el,
        getData: () => {
            const result = {};
            for (const g of getters)
                result[g.key] = { font_size: g.fontSize() };
            return result;
        },
    };
}
function buildFontsTab(d) {
    const el = document.createElement('div');
    const fonts = d.fonts ?? {};
    const getters = [];
    const fontKeys = ['body', 'heading', 'mono', 'hebrew', 'fallback'];
    for (const key of fontKeys) {
        const input = configInput(key.charAt(0).toUpperCase() + key.slice(1), fonts[key] ?? '');
        el.appendChild(input.wrapper);
        getters.push({ key, getValue: input.getValue });
    }
    return {
        el,
        getData: () => {
            const result = {};
            for (const g of getters)
                result[g.key] = g.getValue();
            return result;
        },
    };
}
function buildAssignmentsTab(d) {
    const el = document.createElement('div');
    const assignments = d.font_assignments ?? {};
    const kv = configKeyValue('Font Assignments', assignments);
    el.appendChild(kv.wrapper);
    return { el, getData: () => kv.getValue() };
}
function buildAlignmentTab(d) {
    const el = document.createElement('div');
    const alignment = d.alignment ?? {};
    const getters = [];
    for (const [key, val] of Object.entries(alignment)) {
        const sel = configSelect(key, val ?? 'left', [
            { value: 'left', label: 'Left' },
            { value: 'center', label: 'Center' },
            { value: 'right', label: 'Right' },
            { value: 'justify', label: 'Justify' },
        ]);
        el.appendChild(sel.wrapper);
        getters.push({ key, getValue: sel.getValue });
    }
    return {
        el,
        getData: () => {
            const result = {};
            for (const g of getters)
                result[g.key] = g.getValue();
            return result;
        },
    };
}
function buildColorsTab(d) {
    const el = document.createElement('div');
    const colors = d.colors ?? {};
    const getters = [];
    for (const [key, val] of Object.entries(colors)) {
        const c = configColor(key, val ?? '#000000');
        el.appendChild(c.wrapper);
        getters.push({ key, getValue: c.getValue });
    }
    return {
        el,
        getData: () => {
            const result = {};
            for (const g of getters)
                result[g.key] = g.getValue();
            return result;
        },
    };
}
function buildStylesTab(d) {
    const el = document.createElement('div');
    const styles = d.styles ?? {};
    const getters = [];
    for (const [key, val] of Object.entries(styles)) {
        const sel = configSelect(key, val ?? 'normal', [
            { value: 'normal', label: 'Normal' },
            { value: 'bold', label: 'Bold' },
            { value: 'italic', label: 'Italic' },
            { value: 'bold_italic', label: 'Bold Italic' },
        ]);
        el.appendChild(sel.wrapper);
        getters.push({ key, getValue: sel.getValue });
    }
    return {
        el,
        getData: () => {
            const result = {};
            for (const g of getters)
                result[g.key] = g.getValue();
            return result;
        },
    };
}
function buildElementsTab(d) {
    const el = document.createElement('div');
    const elements = d.tables_lists_figures ?? {};
    // Render as nested key-value for now — complex structure.
    const text = document.createElement('pre');
    text.style.cssText = 'font-family: var(--font-mono); font-size: var(--text-sm); background: var(--color-bg-alt); padding: var(--space-4); border-radius: var(--border-radius); overflow: auto; max-height: 400px;';
    text.textContent = JSON.stringify(elements, null, 2);
    el.appendChild(text);
    const note = document.createElement('p');
    note.style.cssText = 'font-size: var(--text-sm); color: var(--color-text-muted); margin-top: var(--space-2);';
    note.textContent = 'Complex element config — edit via YAML for fine control.';
    el.appendChild(note);
    return { el, getData: () => elements };
}
export const typographyEditor = {
    sectionKey: 'typography',
    title: 'Typography',
    description: 'Type scale, fonts, assignments, colors, styles, and elements.',
    buildForm(data) {
        const d = (data ?? {});
        const el = document.createElement('div');
        const scale = buildScaleTab(d);
        const depth = buildDepthTab(d);
        const fonts = buildFontsTab(d);
        const assignments = buildAssignmentsTab(d);
        const alignment = buildAlignmentTab(d);
        const colors = buildColorsTab(d);
        const styles = buildStylesTab(d);
        const elements = buildElementsTab(d);
        const tabs = configTabs([
            { key: 'scale', label: 'Scale', content: scale.el },
            { key: 'depth', label: 'Depth', content: depth.el },
            { key: 'fonts', label: 'Fonts', content: fonts.el },
            { key: 'assignments', label: 'Assignments', content: assignments.el },
            { key: 'alignment', label: 'Alignment', content: alignment.el },
            { key: 'colors', label: 'Colors', content: colors.el },
            { key: 'styles', label: 'Styles', content: styles.el },
            { key: 'elements', label: 'Elements', content: elements.el },
        ]);
        el.appendChild(tabs);
        return {
            element: el,
            getData: () => ({
                body_size: scale.getData().body_size,
                scale_ratio: scale.getData().scale_ratio,
                line_height: scale.getData().line_height,
                depth: depth.getData(),
                fonts: fonts.getData(),
                font_assignments: assignments.getData(),
                alignment: alignment.getData(),
                colors: colors.getData(),
                styles: styles.getData(),
                tables_lists_figures: elements.getData(),
            }),
        };
    },
};
