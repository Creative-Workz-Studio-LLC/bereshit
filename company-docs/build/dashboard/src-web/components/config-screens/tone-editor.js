/**
 * Tone & Voice Editor — §15 build.config.yaml
 *
 * Voice attributes, tone spectrum, covenant language replacements,
 * plain language, paragraph density, passive voice, readability targets.
 */
import { configInput, configNumber, configStringArray, configBool, configSection, configTabs, } from './editor-base.js';
function buildVoiceTab(d) {
    const el = document.createElement('div');
    const voice = d.cws_voice ?? [];
    const rows = [];
    const container = document.createElement('div');
    container.style.cssText = 'display: flex; flex-direction: column; gap: var(--space-3);';
    el.appendChild(container);
    function addRow(attr) {
        const row = document.createElement('div');
        row.className = 'card';
        row.style.cssText = 'display: grid; grid-template-columns: 1fr 2fr; gap: var(--space-3);';
        const name = configInput('Attribute', attr.name ?? '');
        const description = configInput('Description', attr.description ?? '');
        row.appendChild(name.wrapper);
        row.appendChild(description.wrapper);
        container.appendChild(row);
        rows.push({ name, description, row });
    }
    for (const attr of voice)
        addRow(attr);
    return {
        el,
        getData: () => rows.map((r) => ({
            name: r.name.getValue(),
            description: r.description.getValue(),
        })),
    };
}
function buildSpectrumTab(d) {
    const el = document.createElement('div');
    const spectrum = d.tone_spectrum ?? [];
    const rows = [];
    const container = document.createElement('div');
    container.style.cssText = 'display: flex; flex-direction: column; gap: var(--space-3);';
    el.appendChild(container);
    for (const item of spectrum) {
        const row = document.createElement('div');
        row.className = 'card';
        row.style.cssText = 'display: grid; grid-template-columns: 1fr 2fr; gap: var(--space-3);';
        const level = configInput('Level', item.level ?? '');
        const description = configInput('Description', item.description ?? '');
        row.appendChild(level.wrapper);
        row.appendChild(description.wrapper);
        container.appendChild(row);
        rows.push({ level, description, row });
    }
    return {
        el,
        getData: () => rows.map((r) => ({
            level: r.level.getValue(),
            description: r.description.getValue(),
        })),
    };
}
function buildCovenantTab(d) {
    const el = document.createElement('div');
    const covenant = d.covenant_language ?? {};
    const replacements = covenant.replacements ?? [];
    const rows = [];
    const container = document.createElement('div');
    container.style.cssText = 'display: flex; flex-direction: column; gap: var(--space-2);';
    el.appendChild(container);
    function addRow(r) {
        const row = document.createElement('div');
        row.style.cssText = 'display: grid; grid-template-columns: 1fr 1fr auto; gap: var(--space-2); align-items: end;';
        const from = configInput('Replace', r.from ?? '');
        const to = configInput('With', r.to ?? '');
        const removeBtn = document.createElement('button');
        removeBtn.className = 'btn btn-danger';
        removeBtn.textContent = 'X';
        removeBtn.style.marginBottom = 'var(--space-4)';
        removeBtn.addEventListener('click', () => {
            const idx = rows.findIndex((x) => x.row === row);
            if (idx >= 0)
                rows.splice(idx, 1);
            row.remove();
            el.dispatchEvent(new Event('change', { bubbles: true }));
        });
        row.appendChild(from.wrapper);
        row.appendChild(to.wrapper);
        row.appendChild(removeBtn);
        container.appendChild(row);
        rows.push({ from, to, row });
    }
    for (const r of replacements)
        addRow(r);
    const addBtn = document.createElement('button');
    addBtn.className = 'btn';
    addBtn.textContent = '+ Add Replacement';
    addBtn.style.marginTop = 'var(--space-2)';
    addBtn.addEventListener('click', () => {
        addRow({ from: '', to: '' });
        el.dispatchEvent(new Event('change', { bubbles: true }));
    });
    el.appendChild(addBtn);
    return {
        el,
        getData: () => ({
            replacements: rows.map((r) => ({
                from: r.from.getValue(),
                to: r.to.getValue(),
            })),
        }),
    };
}
function buildPlainTab(d) {
    const el = document.createElement('div');
    const plain = d.plain_language ?? {};
    const maxSentence = configNumber('Max Sentence Length', plain.max_sentence_length ?? 50, { min: 10, max: 100, unit: 'words' });
    const words = configStringArray('Plain Language Words', plain.words ?? []);
    el.appendChild(maxSentence.wrapper);
    el.appendChild(words.wrapper);
    return {
        el,
        getData: () => ({
            max_sentence_length: maxSentence.getValue(),
            words: words.getValue(),
        }),
    };
}
function buildReadabilityTab(d) {
    const el = document.createElement('div');
    const targets = d.readability_targets ?? [];
    const rows = [];
    const container = document.createElement('div');
    container.style.cssText = 'display: flex; flex-direction: column; gap: var(--space-3);';
    el.appendChild(container);
    for (const item of targets) {
        const row = document.createElement('div');
        row.style.cssText = 'display: grid; grid-template-columns: 1fr 1fr; gap: var(--space-3);';
        const register = configInput('Register', item.register ?? '');
        const grade = configNumber('Grade Level', item.grade_level ?? 12, { min: 1, max: 20 });
        row.appendChild(register.wrapper);
        row.appendChild(grade.wrapper);
        container.appendChild(row);
        rows.push({ register, grade_level: grade, row });
    }
    return {
        el,
        getData: () => rows.map((r) => ({
            register: r.register.getValue(),
            grade_level: r.grade_level.getValue(),
        })),
    };
}
function buildMiscTab(d) {
    const el = document.createElement('div');
    // Closing filler
    el.appendChild(configSection('Closing Filler'));
    const closingFiller = d.closing_filler ?? {};
    const fillerPhrases = configStringArray('Filler Phrases', closingFiller.phrases ?? []);
    el.appendChild(fillerPhrases.wrapper);
    // Paragraph density
    el.appendChild(configSection('Paragraph Density'));
    const density = d.paragraph_density ?? {};
    const maxLines = configNumber('Max Lines Per Paragraph', density.max_lines ?? 8, { min: 1, max: 30 });
    el.appendChild(maxLines.wrapper);
    // Passive voice
    el.appendChild(configSection('Passive Voice'));
    const passive = d.passive_voice ?? {};
    const passiveEnabled = configBool('Enable check', passive.enabled ?? true);
    const passiveMax = configNumber('Max % allowed', passive.max_percent ?? 20, { min: 0, max: 100, unit: '%' });
    el.appendChild(passiveEnabled.wrapper);
    el.appendChild(passiveMax.wrapper);
    return {
        el,
        getData: () => ({
            closing_filler: { phrases: fillerPhrases.getValue() },
            paragraph_density: { max_lines: maxLines.getValue() },
            passive_voice: {
                enabled: passiveEnabled.getValue(),
                max_percent: passiveMax.getValue(),
            },
        }),
    };
}
export const toneEditor = {
    sectionKey: 'tone_and_voice',
    title: 'Tone & Voice',
    description: 'Voice attributes, tone spectrum, covenant language, readability.',
    buildForm(data) {
        const d = (data ?? {});
        const el = document.createElement('div');
        const voice = buildVoiceTab(d);
        const spectrum = buildSpectrumTab(d);
        const covenant = buildCovenantTab(d);
        const plain = buildPlainTab(d);
        const readability = buildReadabilityTab(d);
        const misc = buildMiscTab(d);
        const tabs = configTabs([
            { key: 'voice', label: 'Voice', content: voice.el },
            { key: 'spectrum', label: 'Spectrum', content: spectrum.el },
            { key: 'covenant', label: 'Covenant', content: covenant.el },
            { key: 'plain', label: 'Plain Language', content: plain.el },
            { key: 'readability', label: 'Readability', content: readability.el },
            { key: 'misc', label: 'Other', content: misc.el },
        ]);
        el.appendChild(tabs);
        return {
            element: el,
            getData: () => ({
                cws_voice: voice.getData(),
                tone_spectrum: spectrum.getData(),
                covenant_language: covenant.getData(),
                plain_language: plain.getData(),
                readability_targets: readability.getData(),
                ...misc.getData(),
            }),
        };
    },
};
