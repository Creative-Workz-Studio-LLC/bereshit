/**
 * Attributes Editor — §2 build.config.yaml
 *
 * Simple key-value pair editor for AsciiDoc document attributes.
 */

import type { EditorDefinition } from './editor-base.js';
import { configKeyValue } from './editor-base.js';

export const attributesEditor: EditorDefinition = {
  sectionKey: 'attributes',
  title: 'Attributes',
  description: 'Custom AsciiDoc attributes passed to the document processor.',

  buildForm(data: unknown) {
    const attrs = (data ?? {}) as Record<string, string>;
    const el = document.createElement('div');

    const kv = configKeyValue('Document Attributes', attrs);
    el.appendChild(kv.wrapper);

    return {
      element: el,
      getData: () => kv.getValue(),
    };
  },
};
