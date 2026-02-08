/**
 * Utility View — Assets, Config, Remove commands.
 */
import { createCommandCard, runCliCommand, runMakeTarget } from '../shared/runner.js';
import { createSelect, createToggle, createRadioGroup, createTextInput } from '../shared/form-controls.js';
export function createUtilityView() {
    const el = document.createElement('div');
    const h2 = document.createElement('h2');
    h2.textContent = 'Utility';
    el.appendChild(h2);
    // ── Assets ────────────────────────────────────────────────────
    const forceToggle = createToggle('assets-force', 'Force regenerate');
    const assetsControls = document.createElement('div');
    assetsControls.className = 'command-options';
    assetsControls.appendChild(forceToggle.wrapper);
    el.appendChild(createCommandCard({
        title: 'Regenerate Assets',
        description: 'Regenerate Makefile, config.mk, and shared attributes from build.config.yaml.',
        controls: assetsControls,
        onRun: () => {
            const args = [];
            if (forceToggle.isChecked())
                args.push('--force');
            runCliCommand('assets', args);
        },
    }));
    // ── Config Manager ────────────────────────────────────────────
    const configMode = createRadioGroup('config-mode', 'Mode', [
        { value: 'show', label: 'Show', description: 'Display current configuration' },
        { value: 'validate', label: 'Validate', description: 'Check config integrity' },
        { value: 'make', label: 'Make', description: 'Generate config.mk from YAML' },
        { value: 'types', label: 'Types', description: 'Show type definitions' },
    ]);
    const configControls = document.createElement('div');
    configControls.className = 'command-options';
    configControls.appendChild(configMode.wrapper);
    el.appendChild(createCommandCard({
        title: 'Config Manager',
        description: 'View, validate, or generate configuration files.',
        controls: configControls,
        onRun: () => {
            const mode = configMode.getValue();
            runCliCommand('config', ['--' + mode]);
        },
    }));
    // ── Remove ────────────────────────────────────────────────────
    const removeScope = createSelect('Scope', [
        { value: 'file', label: 'File — remove a specific output' },
        { value: 'section', label: 'Section — remove all outputs for section' },
        { value: 'all', label: 'All — clean all build outputs' },
    ]);
    const removeTarget = createTextInput('remove-target', 'Target', 'Section file, article/part ID, or output path');
    const removeControls = document.createElement('div');
    removeControls.className = 'command-options';
    removeControls.appendChild(removeScope.wrapper);
    removeControls.appendChild(removeTarget.wrapper);
    // Hide target input when scope is "all".
    removeScope.onChange((scope) => {
        removeTarget.wrapper.style.display = scope === 'all' ? 'none' : '';
    });
    el.appendChild(createCommandCard({
        title: 'Remove Build Outputs',
        description: 'Remove generated files by scope.',
        controls: removeControls,
        onRun: () => {
            const scope = removeScope.getValue();
            if (scope === 'all') {
                runMakeTarget('clean');
            }
            else {
                const target = removeTarget.getValue();
                if (!target) {
                    alert('Enter a target.');
                    return;
                }
                runCliCommand('remove', ['--scope', scope, target]);
            }
        },
    }));
    return el;
}
