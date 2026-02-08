/**
 * Watch Editor — §4 build.config.yaml
 *
 * File watching patterns, ignore lists, debounce, and format selection.
 */

import type { EditorDefinition } from './editor-base.js';
import { configStringArray, configNumber, configSection } from './editor-base.js';

interface WatchData {
  patterns?: string[];
  ignore?: string[];
  debounce_ms?: number;
  formats?: string[];
}

export const watchEditor: EditorDefinition = {
  sectionKey: 'watch',
  title: 'Watch',
  description: 'File watching patterns and debounce settings.',

  buildForm(data: unknown) {
    const d = (data ?? {}) as WatchData;
    const el = document.createElement('div');

    const patterns = configStringArray('Watch Patterns', d.patterns ?? [], {
      placeholder: 'e.g., **/*.adoc',
    });
    el.appendChild(patterns.wrapper);

    el.appendChild(configSection('Ignore'));
    const ignore = configStringArray('Ignore Patterns', d.ignore ?? [], {
      placeholder: 'e.g., **/node_modules/**',
    });
    el.appendChild(ignore.wrapper);

    el.appendChild(configSection('Settings'));
    const debounce = configNumber('Debounce', d.debounce_ms ?? 300, {
      min: 50,
      max: 5000,
      step: 50,
      unit: 'ms',
    });
    el.appendChild(debounce.wrapper);

    const formats = configStringArray('Formats to rebuild', d.formats ?? [], {
      placeholder: 'e.g., pdf',
    });
    el.appendChild(formats.wrapper);

    return {
      element: el,
      getData: () => ({
        patterns: patterns.getValue(),
        ignore: ignore.getValue(),
        debounce_ms: debounce.getValue(),
        formats: formats.getValue(),
      }),
    };
  },
};
