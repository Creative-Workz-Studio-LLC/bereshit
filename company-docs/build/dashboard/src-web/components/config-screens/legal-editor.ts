/**
 * Legal Language Editor — §13 build.config.yaml
 *
 * Accordion-style editor for the most complex config section.
 * 16 subsections covering obligation, terms, binding, anatomy, etc.
 */

import type { EditorDefinition } from './editor-base.js';
import {
  configInput,
  configStringArray,
  configKeyValue,
  configSection,
  configNumber,
  configBool,
  configTabs,
} from './editor-base.js';

/* eslint-disable @typescript-eslint/no-explicit-any */
type AnyData = Record<string, any>;

// ── Subsection builders ──────────────────────────────────────────

function buildObligation(d: AnyData): { el: HTMLElement; getData: () => AnyData } {
  const el = document.createElement('div');
  const obligation = d.obligation ?? {};

  // Three word groups: mandatory, prohibitive, permissive.
  const groups: Array<{ key: string; getter: ReturnType<typeof configStringArray> }> = [];
  for (const key of ['mandatory', 'prohibitive', 'permissive']) {
    const words = configStringArray(key.charAt(0).toUpperCase() + key.slice(1) + ' Words', obligation[key]?.words ?? []);
    el.appendChild(words.wrapper);
    groups.push({ key, getter: words });
  }

  // Register map.
  el.appendChild(configSection('Register Map'));
  const regMap = configKeyValue('Register → Word Set', obligation.register_map ?? {});
  el.appendChild(regMap.wrapper);

  return {
    el,
    getData: () => {
      const result: AnyData = { register_map: regMap.getValue() };
      for (const g of groups) result[g.key] = { words: g.getter.getValue() };
      return result;
    },
  };
}

function buildDefinedTerms(d: AnyData): { el: HTMLElement; getData: () => AnyData } {
  const el = document.createElement('div');
  const dt = d.defined_terms ?? {};
  const getters: Array<{ key: string; getValue: () => string }> = [];

  for (const [key, val] of Object.entries(dt as Record<string, string>)) {
    const input = configInput(key, String(val ?? ''));
    el.appendChild(input.wrapper);
    getters.push({ key, getValue: input.getValue });
  }

  return {
    el,
    getData: () => {
      const result: AnyData = {};
      for (const g of getters) result[g.key] = g.getValue();
      return result;
    },
  };
}

function buildBinding(d: AnyData): { el: HTMLElement; getData: () => AnyData } {
  const el = document.createElement('div');
  const binding = d.binding ?? [];
  const arr = configStringArray('Binding Locations', Array.isArray(binding) ? binding.map(String) : []);
  el.appendChild(arr.wrapper);
  return { el, getData: () => arr.getValue() };
}

function buildDocAnatomy(d: AnyData): { el: HTMLElement; getData: () => AnyData } {
  const el = document.createElement('div');
  const anatomy = d.document_anatomy ?? {};

  const required = configStringArray('Required Sections', anatomy.required ?? []);
  const optional = configStringArray('Optional Sections', anatomy.optional ?? []);
  el.appendChild(required.wrapper);
  el.appendChild(optional.wrapper);

  return {
    el,
    getData: () => ({
      required: required.getValue(),
      optional: optional.getValue(),
    }),
  };
}

function buildRecitals(d: AnyData): { el: HTMLElement; getData: () => AnyData } {
  const el = document.createElement('div');
  const rec = d.recitals ?? {};

  const style = configInput('Style', rec.style ?? '');
  const prefix = configInput('Prefix', rec.prefix ?? '');
  const separator = configInput('Separator', rec.separator ?? '');
  el.appendChild(style.wrapper);
  el.appendChild(prefix.wrapper);
  el.appendChild(separator.wrapper);

  return {
    el,
    getData: () => ({
      style: style.getValue(),
      prefix: prefix.getValue(),
      separator: separator.getValue(),
    }),
  };
}

function buildNumbering(d: AnyData): { el: HTMLElement; getData: () => AnyData } {
  const el = document.createElement('div');
  const num = d.numbering ?? {};
  // Render as JSON for complex nested levels.
  const pre = document.createElement('pre');
  pre.style.cssText = 'font-family: var(--font-mono); font-size: var(--text-sm); background: var(--color-bg-alt); padding: var(--space-4); border-radius: var(--border-radius); overflow: auto; max-height: 300px;';
  pre.textContent = JSON.stringify(num, null, 2);
  el.appendChild(pre);
  return { el, getData: () => num };
}

function buildBoilerplate(d: AnyData): { el: HTMLElement; getData: () => AnyData } {
  const el = document.createElement('div');
  const bp = d.boilerplate ?? {};

  const clauses = configStringArray('Standard Clauses', bp.clauses ?? []);
  const template = configInput('Template', bp.template ?? '');
  el.appendChild(clauses.wrapper);
  el.appendChild(template.wrapper);

  return {
    el,
    getData: () => ({
      clauses: clauses.getValue(),
      template: template.getValue(),
    }),
  };
}

function buildSimpleStrings(d: AnyData, key: string, label: string): { el: HTMLElement; getData: () => AnyData } {
  const el = document.createElement('div');
  const section = d[key] ?? {};

  // Most remaining subsections are small objects with string/array fields.
  const getters: Array<{ k: string; getValue: () => unknown }> = [];

  for (const [k, val] of Object.entries(section as Record<string, unknown>)) {
    if (Array.isArray(val)) {
      const arr = configStringArray(k, val.map(String));
      el.appendChild(arr.wrapper);
      getters.push({ k, getValue: arr.getValue });
    } else if (typeof val === 'string') {
      const input = configInput(k, val);
      el.appendChild(input.wrapper);
      getters.push({ k, getValue: input.getValue });
    } else if (typeof val === 'object' && val !== null) {
      const pre = document.createElement('pre');
      pre.style.cssText = 'font-family: var(--font-mono); font-size: var(--text-sm); background: var(--color-bg-alt); padding: var(--space-3); border-radius: var(--border-radius); margin-bottom: var(--space-3);';
      pre.textContent = `${k}: ${JSON.stringify(val, null, 2)}`;
      el.appendChild(pre);
      getters.push({ k, getValue: () => val });
    }
  }

  return {
    el,
    getData: () => {
      const result: AnyData = {};
      for (const g of getters) result[g.k] = g.getValue();
      return result;
    },
  };
}

export const legalEditor: EditorDefinition = {
  sectionKey: 'legal_language',
  title: 'Legal Language',
  description: 'Obligation words, defined terms, binding, document anatomy, and more.',

  buildForm(data: unknown) {
    const d = (data ?? {}) as AnyData;
    const el = document.createElement('div');

    const obligation = buildObligation(d);
    const definedTerms = buildDefinedTerms(d);
    const binding = buildBinding(d);
    const docAnatomy = buildDocAnatomy(d);
    const recitals = buildRecitals(d);
    const numbering = buildNumbering(d);
    const boilerplate = buildBoilerplate(d);
    const construction = buildSimpleStrings(d, 'construction', 'Construction');
    const defVerbs = buildSimpleStrings(d, 'definition_verbs', 'Definition Verbs');
    const amendment = buildSimpleStrings(d, 'amendment_authority', 'Amendment Authority');
    const governance = buildSimpleStrings(d, 'governance_notation', 'Governance Notation');
    const signature = buildSimpleStrings(d, 'signature_format', 'Signature Format');
    const crossRef = buildSimpleStrings(d, 'cross_reference', 'Cross-Reference');
    const archaic = buildSimpleStrings(d, 'archaic_terms', 'Archaic Terms');
    const whereas = buildSimpleStrings(d, 'whereas_operative', 'Whereas-Operative');
    const enumeration = buildSimpleStrings(d, 'enumeration', 'Enumeration');

    const tabs = configTabs([
      { key: 'obligation', label: 'Obligation', content: obligation.el },
      { key: 'defined_terms', label: 'Defined Terms', content: definedTerms.el },
      { key: 'def_verbs', label: 'Def. Verbs', content: defVerbs.el },
      { key: 'construction', label: 'Construction', content: construction.el },
      { key: 'binding', label: 'Binding', content: binding.el },
      { key: 'anatomy', label: 'Doc Anatomy', content: docAnatomy.el },
      { key: 'recitals', label: 'Recitals', content: recitals.el },
      { key: 'numbering', label: 'Numbering', content: numbering.el },
      { key: 'enumeration', label: 'Enumeration', content: enumeration.el },
      { key: 'boilerplate', label: 'Boilerplate', content: boilerplate.el },
      { key: 'amendment', label: 'Amendment', content: amendment.el },
      { key: 'governance', label: 'Governance', content: governance.el },
      { key: 'signature', label: 'Signature', content: signature.el },
      { key: 'cross_ref', label: 'Cross-Ref', content: crossRef.el },
      { key: 'archaic', label: 'Archaic', content: archaic.el },
      { key: 'whereas', label: 'Whereas', content: whereas.el },
    ]);
    el.appendChild(tabs);

    return {
      element: el,
      getData: () => ({
        obligation: obligation.getData(),
        defined_terms: definedTerms.getData(),
        definition_verbs: defVerbs.getData(),
        construction: construction.getData(),
        binding: binding.getData(),
        document_anatomy: docAnatomy.getData(),
        recitals: recitals.getData(),
        numbering: numbering.getData(),
        enumeration: enumeration.getData(),
        boilerplate: boilerplate.getData(),
        amendment_authority: amendment.getData(),
        governance_notation: governance.getData(),
        signature_format: signature.getData(),
        cross_reference: crossRef.getData(),
        archaic_terms: archaic.getData(),
        whereas_operative: whereas.getData(),
      }),
    };
  },
};
