/**
 * Attributes Editor — §2 build.config.yaml
 *
 * Simple key-value pair editor for AsciiDoc document attributes.
 */
import { configKeyValue } from './editor-base.js';
export const attributesEditor = {
    sectionKey: 'attributes',
    title: 'Attributes',
    description: 'Custom AsciiDoc attributes passed to the document processor.',
    buildForm(data) {
        const attrs = (data ?? {});
        const el = document.createElement('div');
        const kv = configKeyValue('Document Attributes', attrs);
        el.appendChild(kv.wrapper);
        return {
            element: el,
            getData: () => kv.getValue(),
        };
    },
};
