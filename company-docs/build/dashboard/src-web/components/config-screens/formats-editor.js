/**
 * Formats Editor — §3 build.config.yaml
 *
 * Format cards with enable/disable toggles and option fields.
 */
import { configBool, configInput, configStringArray, configSection } from './editor-base.js';
export const formatsEditor = {
    sectionKey: 'formats',
    title: 'Formats',
    description: 'Output format configuration (PDF, EPUB, HTML).',
    buildForm(data) {
        const formats = (data ?? {});
        const el = document.createElement('div');
        const getters = [];
        for (const [key, fmt] of Object.entries(formats)) {
            el.appendChild(configSection(key.toUpperCase()));
            const card = document.createElement('div');
            card.style.cssText = 'padding-left: var(--space-4); display: flex; flex-direction: column; gap: var(--space-2);';
            const enabled = configBool('Enabled', fmt.enabled ?? true);
            const command = configInput('Command', fmt.command ?? '', { placeholder: 'e.g., asciidoctor-pdf' });
            const backend = configInput('Backend', fmt.backend ?? '');
            const extension = configInput('Extension', fmt.extension ?? '', { placeholder: '.pdf' });
            const install = configInput('Install command', fmt.install ?? '', { placeholder: 'gem install asciidoctor-pdf' });
            const pipeline = configInput('Pipeline', fmt.pipeline ?? '');
            const args = configStringArray('Arguments', fmt.args ?? [], { placeholder: '-a key=value' });
            card.appendChild(enabled.wrapper);
            card.appendChild(command.wrapper);
            card.appendChild(backend.wrapper);
            card.appendChild(extension.wrapper);
            card.appendChild(install.wrapper);
            card.appendChild(pipeline.wrapper);
            card.appendChild(args.wrapper);
            el.appendChild(card);
            getters.push({
                key,
                getData: () => ({
                    enabled: enabled.getValue(),
                    command: command.getValue(),
                    backend: backend.getValue(),
                    extension: extension.getValue(),
                    install: install.getValue(),
                    pipeline: pipeline.getValue() || undefined,
                    args: args.getValue(),
                    options: fmt.options ?? {},
                }),
            });
        }
        return {
            element: el,
            getData: () => {
                const result = {};
                for (const g of getters)
                    result[g.key] = g.getData();
                return result;
            },
        };
    },
};
