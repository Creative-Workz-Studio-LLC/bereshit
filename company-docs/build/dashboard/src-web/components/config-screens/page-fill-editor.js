/**
 * Page Fill Editor — §20 build.config.yaml
 *
 * Fill range definitions, marker pattern, cross-check settings.
 */
import { configInput, configNumber, configSection } from './editor-base.js';
export const pageFillEditor = {
    sectionKey: 'page_fill',
    title: 'Page Fill',
    description: 'Page fill analysis ranges, marker patterns, and thresholds.',
    buildForm(data) {
        const d = (data ?? {});
        const el = document.createElement('div');
        // ── Ranges ────────────────────────────────────────────────────
        el.appendChild(configSection('Fill Ranges'));
        const rangeGetters = [];
        for (const [key, range] of Object.entries(d.ranges ?? {})) {
            const row = document.createElement('div');
            row.className = 'card';
            row.style.cssText = 'display: grid; grid-template-columns: 120px 1fr 1fr 1fr; gap: var(--space-3); align-items: end;';
            const keyLabel = document.createElement('div');
            keyLabel.style.cssText = 'font-family: var(--font-heading); font-weight: 600; padding-bottom: var(--space-4);';
            keyLabel.textContent = key;
            const min = configNumber('Min %', range.min ?? 0, { min: 0, max: 100 });
            const max = configNumber('Max %', range.max ?? 100, { min: 0, max: 100 });
            const label = configInput('Label', range.label ?? key);
            row.appendChild(keyLabel);
            row.appendChild(min.wrapper);
            row.appendChild(max.wrapper);
            row.appendChild(label.wrapper);
            el.appendChild(row);
            rangeGetters.push({
                key,
                getMin: min.getValue,
                getMax: max.getValue,
                getLabel: label.getValue,
            });
        }
        // ── Marker Pattern ────────────────────────────────────────────
        el.appendChild(configSection('Settings'));
        const markerPattern = configInput('Marker Pattern', d.marker_pattern ?? '', {
            placeholder: 'Regex pattern for page markers',
        });
        el.appendChild(markerPattern.wrapper);
        const bleed = configNumber('Bleed Threshold', d.bleed_threshold ?? 0, {
            min: 0,
            max: 100,
            unit: '%',
        });
        el.appendChild(bleed.wrapper);
        // ── Cross-Check ───────────────────────────────────────────────
        el.appendChild(configSection('Cross-Check'));
        const cc = d.cross_check ?? {};
        const tolerance = configNumber('Tolerance', cc.tolerance_percent ?? 5, { min: 0, max: 50, unit: '%' });
        const warnThresh = configNumber('Warning Threshold', cc.warn_threshold ?? 10, { min: 0, max: 100, unit: '%' });
        const failThresh = configNumber('Fail Threshold', cc.fail_threshold ?? 25, { min: 0, max: 100, unit: '%' });
        el.appendChild(tolerance.wrapper);
        el.appendChild(warnThresh.wrapper);
        el.appendChild(failThresh.wrapper);
        return {
            element: el,
            getData: () => {
                const ranges = {};
                for (const r of rangeGetters) {
                    ranges[r.key] = {
                        min: r.getMin(),
                        max: r.getMax(),
                        label: r.getLabel(),
                    };
                }
                return {
                    ranges,
                    marker_pattern: markerPattern.getValue(),
                    bleed_threshold: bleed.getValue(),
                    cross_check: {
                        tolerance_percent: tolerance.getValue(),
                        warn_threshold: warnThresh.getValue(),
                        fail_threshold: failThresh.getValue(),
                    },
                };
            },
        };
    },
};
