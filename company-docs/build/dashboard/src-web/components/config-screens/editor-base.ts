/**
 * Editor Base — shared wrapper for all config section editors.
 *
 * Handles: load config → render form → track changes → save/reset.
 * Each section editor provides a buildForm() function that returns
 * the form element and a getData() function to extract current values.
 */

import { getAdapter } from '../../scripts/adapter.js';

export interface EditorDefinition {
  /** Config section key (e.g., 'attributes', 'brand_colors'). */
  sectionKey: string;
  /** Display title. */
  title: string;
  /** Short description. */
  description: string;
  /**
   * Build the form for this section.
   * Receives the current config data for this section.
   * Returns the form element and a getData() function.
   */
  buildForm(data: unknown): {
    element: HTMLElement;
    getData: () => unknown;
  };
}

type EditorStatus = 'loading' | 'clean' | 'modified' | 'saving' | 'saved' | 'error';

/**
 * Create a config editor panel.
 * Returns the full editor element (title, form, save/reset buttons).
 * Calls onBack when the user clicks "Back to Config".
 */
export function createConfigEditor(
  def: EditorDefinition,
  onBack: () => void,
): HTMLElement {
  const el = document.createElement('div');
  el.className = 'config-editor';

  // ── Header ──────────────────────────────────────────────────────
  const header = document.createElement('div');
  header.className = 'config-editor-header';

  const backBtn = document.createElement('button');
  backBtn.className = 'btn';
  backBtn.textContent = 'Back';
  backBtn.addEventListener('click', onBack);

  const titleEl = document.createElement('h2');
  titleEl.textContent = def.title;

  const descEl = document.createElement('p');
  descEl.className = 'config-editor-desc';
  descEl.textContent = def.description;

  const statusBadge = document.createElement('span');
  statusBadge.className = 'badge badge-info';
  statusBadge.textContent = 'Loading...';

  const titleRow = document.createElement('div');
  titleRow.className = 'config-editor-title-row';
  titleRow.appendChild(backBtn);
  titleRow.appendChild(titleEl);
  titleRow.appendChild(statusBadge);

  header.appendChild(titleRow);
  header.appendChild(descEl);
  el.appendChild(header);

  // ── Form Container ──────────────────────────────────────────────
  const formContainer = document.createElement('div');
  formContainer.className = 'config-editor-form';
  el.appendChild(formContainer);

  // ── Action Bar ──────────────────────────────────────────────────
  const actionBar = document.createElement('div');
  actionBar.className = 'config-editor-actions';

  const saveBtn = document.createElement('button');
  saveBtn.className = 'btn btn-primary';
  saveBtn.textContent = 'Save';
  saveBtn.disabled = true;

  const resetBtn = document.createElement('button');
  resetBtn.className = 'btn';
  resetBtn.textContent = 'Reset';
  resetBtn.disabled = true;

  const regenBtn = document.createElement('button');
  regenBtn.className = 'btn';
  regenBtn.textContent = 'Regenerate config.mk';
  regenBtn.title = 'Run cws-build assets after saving to update generated files.';
  regenBtn.style.display = 'none';

  actionBar.appendChild(saveBtn);
  actionBar.appendChild(resetBtn);
  actionBar.appendChild(regenBtn);
  el.appendChild(actionBar);

  // ── State ───────────────────────────────────────────────────────
  let originalData: unknown = null;
  let currentGetData: (() => unknown) | null = null;

  function setStatus(status: EditorStatus): void {
    statusBadge.className = 'badge';
    switch (status) {
      case 'loading':
        statusBadge.classList.add('badge-info');
        statusBadge.textContent = 'Loading...';
        break;
      case 'clean':
        statusBadge.classList.add('badge-success');
        statusBadge.textContent = 'Saved';
        break;
      case 'modified':
        statusBadge.classList.add('badge-warning');
        statusBadge.textContent = 'Modified';
        break;
      case 'saving':
        statusBadge.classList.add('badge-info');
        statusBadge.textContent = 'Saving...';
        break;
      case 'saved':
        statusBadge.classList.add('badge-success');
        statusBadge.textContent = 'Saved';
        regenBtn.style.display = '';
        break;
      case 'error':
        statusBadge.classList.add('badge-error');
        statusBadge.textContent = 'Error';
        break;
    }

    saveBtn.disabled = status !== 'modified';
    resetBtn.disabled = status !== 'modified';
  }

  // Track changes via input events on the form container.
  formContainer.addEventListener('input', () => {
    setStatus('modified');
  });
  formContainer.addEventListener('change', () => {
    setStatus('modified');
  });

  // ── Save ────────────────────────────────────────────────────────
  saveBtn.addEventListener('click', async () => {
    if (!currentGetData) return;
    setStatus('saving');
    try {
      const adapter = getAdapter();
      const data = currentGetData();
      await adapter.writeConfigSection(def.sectionKey, data);
      originalData = JSON.parse(JSON.stringify(data));
      setStatus('saved');
    } catch (err) {
      setStatus('error');
      console.error('Failed to save config section:', err);
    }
  });

  // ── Reset ───────────────────────────────────────────────────────
  resetBtn.addEventListener('click', () => {
    if (originalData === null) return;
    formContainer.innerHTML = '';
    const { element, getData } = def.buildForm(JSON.parse(JSON.stringify(originalData)));
    formContainer.appendChild(element);
    currentGetData = getData;
    setStatus('clean');
  });

  // ── Regenerate ──────────────────────────────────────────────────
  regenBtn.addEventListener('click', async () => {
    const { runCliCommand } = await import('../shared/runner.js');
    runCliCommand('assets', ['--force']);
    regenBtn.style.display = 'none';
  });

  // ── Load ────────────────────────────────────────────────────────
  async function load(): Promise<void> {
    setStatus('loading');
    try {
      const adapter = getAdapter();
      const fullConfig = await adapter.readConfig();
      const sectionData = fullConfig[def.sectionKey] ?? {};
      originalData = JSON.parse(JSON.stringify(sectionData));

      formContainer.innerHTML = '';
      const { element, getData } = def.buildForm(JSON.parse(JSON.stringify(sectionData)));
      formContainer.appendChild(element);
      currentGetData = getData;
      setStatus('clean');
    } catch (err) {
      setStatus('error');
      formContainer.innerHTML = `<p style="color: var(--color-error);">Failed to load configuration: ${String(err)}</p>`;
      console.error('Failed to load config:', err);
    }
  }

  load();
  return el;
}

// ── Shared Form Helpers for Config Editors ──────────────────────────

/**
 * Create a labelled text input for config editing.
 */
export function configInput(
  label: string,
  value: string,
  opts?: { placeholder?: string; type?: string; width?: string },
): { wrapper: HTMLElement; getValue: () => string } {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  const lbl = document.createElement('label');
  lbl.className = 'form-label';
  lbl.textContent = label;

  const input = document.createElement('input');
  input.className = 'form-input';
  input.type = opts?.type ?? 'text';
  input.value = value;
  if (opts?.placeholder) input.placeholder = opts.placeholder;
  if (opts?.width) input.style.width = opts.width;

  wrapper.appendChild(lbl);
  wrapper.appendChild(input);

  return { wrapper, getValue: () => input.value };
}

/**
 * Create a labelled number input.
 */
export function configNumber(
  label: string,
  value: number,
  opts?: { min?: number; max?: number; step?: number; unit?: string },
): { wrapper: HTMLElement; getValue: () => number } {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  const lbl = document.createElement('label');
  lbl.className = 'form-label';
  lbl.textContent = label + (opts?.unit ? ` (${opts.unit})` : '');

  const input = document.createElement('input');
  input.className = 'form-input';
  input.type = 'number';
  input.value = String(value);
  if (opts?.min !== undefined) input.min = String(opts.min);
  if (opts?.max !== undefined) input.max = String(opts.max);
  if (opts?.step !== undefined) input.step = String(opts.step);
  input.style.width = '140px';

  wrapper.appendChild(lbl);
  wrapper.appendChild(input);

  return { wrapper, getValue: () => Number(input.value) };
}

/**
 * Create a labelled checkbox (boolean toggle).
 */
export function configBool(
  label: string,
  value: boolean,
): { wrapper: HTMLElement; getValue: () => boolean } {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';
  wrapper.style.display = 'flex';
  wrapper.style.alignItems = 'center';
  wrapper.style.gap = 'var(--space-2)';

  const cb = document.createElement('input');
  cb.type = 'checkbox';
  cb.checked = value;

  const lbl = document.createElement('label');
  lbl.className = 'form-label';
  lbl.style.marginBottom = '0';
  lbl.textContent = label;

  wrapper.appendChild(cb);
  wrapper.appendChild(lbl);

  return { wrapper, getValue: () => cb.checked };
}

/**
 * Create a labelled color input.
 */
export function configColor(
  label: string,
  value: string,
): { wrapper: HTMLElement; getValue: () => string } {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';
  wrapper.style.display = 'flex';
  wrapper.style.alignItems = 'center';
  wrapper.style.gap = 'var(--space-3)';

  const lbl = document.createElement('label');
  lbl.className = 'form-label';
  lbl.style.marginBottom = '0';
  lbl.style.minWidth = '120px';
  lbl.textContent = label;

  const colorInput = document.createElement('input');
  colorInput.type = 'color';
  colorInput.value = value || '#000000';
  colorInput.style.width = '48px';
  colorInput.style.height = '32px';
  colorInput.style.border = '1px solid var(--color-border)';
  colorInput.style.borderRadius = 'var(--border-radius)';
  colorInput.style.cursor = 'pointer';

  const hexInput = document.createElement('input');
  hexInput.className = 'form-input';
  hexInput.value = value || '#000000';
  hexInput.style.width = '100px';
  hexInput.placeholder = '#RRGGBB';

  // Sync color <-> hex.
  colorInput.addEventListener('input', () => {
    hexInput.value = colorInput.value;
  });
  hexInput.addEventListener('input', () => {
    if (/^#[0-9A-Fa-f]{6}$/.test(hexInput.value)) {
      colorInput.value = hexInput.value;
    }
  });

  wrapper.appendChild(lbl);
  wrapper.appendChild(colorInput);
  wrapper.appendChild(hexInput);

  return { wrapper, getValue: () => hexInput.value };
}

/**
 * Create a labelled select dropdown.
 */
export function configSelect(
  label: string,
  value: string,
  options: Array<{ value: string; label: string }>,
): { wrapper: HTMLElement; getValue: () => string } {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  const lbl = document.createElement('label');
  lbl.className = 'form-label';
  lbl.textContent = label;

  const select = document.createElement('select');
  select.className = 'form-select';
  for (const opt of options) {
    const option = document.createElement('option');
    option.value = opt.value;
    option.textContent = opt.label;
    if (opt.value === value) option.selected = true;
    select.appendChild(option);
  }

  wrapper.appendChild(lbl);
  wrapper.appendChild(select);

  return { wrapper, getValue: () => select.value };
}

/**
 * Create a string array editor (add/remove items).
 */
export function configStringArray(
  label: string,
  values: string[],
  opts?: { placeholder?: string },
): { wrapper: HTMLElement; getValue: () => string[] } {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  const lbl = document.createElement('label');
  lbl.className = 'form-label';
  lbl.textContent = label;
  wrapper.appendChild(lbl);

  const list = document.createElement('div');
  list.style.display = 'flex';
  list.style.flexDirection = 'column';
  list.style.gap = 'var(--space-1)';
  wrapper.appendChild(list);

  const items: HTMLInputElement[] = [];

  function addItem(val: string): void {
    const row = document.createElement('div');
    row.style.display = 'flex';
    row.style.gap = 'var(--space-2)';
    row.style.alignItems = 'center';

    const input = document.createElement('input');
    input.className = 'form-input';
    input.value = val;
    if (opts?.placeholder) input.placeholder = opts.placeholder;
    input.style.flex = '1';

    const removeBtn = document.createElement('button');
    removeBtn.className = 'btn btn-danger';
    removeBtn.textContent = 'X';
    removeBtn.style.padding = 'var(--space-1) var(--space-2)';
    removeBtn.style.fontSize = 'var(--text-xs)';
    removeBtn.addEventListener('click', () => {
      const idx = items.indexOf(input);
      if (idx >= 0) items.splice(idx, 1);
      row.remove();
      wrapper.dispatchEvent(new Event('change', { bubbles: true }));
    });

    row.appendChild(input);
    row.appendChild(removeBtn);
    list.appendChild(row);
    items.push(input);
  }

  for (const v of values) addItem(v);

  const addBtn = document.createElement('button');
  addBtn.className = 'btn';
  addBtn.textContent = '+ Add';
  addBtn.style.alignSelf = 'flex-start';
  addBtn.style.marginTop = 'var(--space-2)';
  addBtn.addEventListener('click', () => {
    addItem('');
    wrapper.dispatchEvent(new Event('change', { bubbles: true }));
  });
  wrapper.appendChild(addBtn);

  return {
    wrapper,
    getValue: () => items.map((i) => i.value).filter((v) => v.trim() !== ''),
  };
}

/**
 * Create a key-value pair editor.
 */
export function configKeyValue(
  label: string,
  data: Record<string, string>,
): { wrapper: HTMLElement; getValue: () => Record<string, string> } {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  const lbl = document.createElement('label');
  lbl.className = 'form-label';
  lbl.textContent = label;
  wrapper.appendChild(lbl);

  const list = document.createElement('div');
  list.style.display = 'flex';
  list.style.flexDirection = 'column';
  list.style.gap = 'var(--space-1)';
  wrapper.appendChild(list);

  const rows: Array<{ key: HTMLInputElement; val: HTMLInputElement }> = [];

  function addRow(k: string, v: string): void {
    const row = document.createElement('div');
    row.style.display = 'flex';
    row.style.gap = 'var(--space-2)';
    row.style.alignItems = 'center';

    const keyInput = document.createElement('input');
    keyInput.className = 'form-input';
    keyInput.value = k;
    keyInput.placeholder = 'Key';
    keyInput.style.width = '200px';

    const valInput = document.createElement('input');
    valInput.className = 'form-input';
    valInput.value = v;
    valInput.placeholder = 'Value';
    valInput.style.flex = '1';

    const removeBtn = document.createElement('button');
    removeBtn.className = 'btn btn-danger';
    removeBtn.textContent = 'X';
    removeBtn.style.padding = 'var(--space-1) var(--space-2)';
    removeBtn.style.fontSize = 'var(--text-xs)';
    removeBtn.addEventListener('click', () => {
      const idx = rows.findIndex((r) => r.key === keyInput);
      if (idx >= 0) rows.splice(idx, 1);
      row.remove();
      wrapper.dispatchEvent(new Event('change', { bubbles: true }));
    });

    row.appendChild(keyInput);
    row.appendChild(valInput);
    row.appendChild(removeBtn);
    list.appendChild(row);
    rows.push({ key: keyInput, val: valInput });
  }

  for (const [k, v] of Object.entries(data)) addRow(k, String(v));

  const addBtn = document.createElement('button');
  addBtn.className = 'btn';
  addBtn.textContent = '+ Add';
  addBtn.style.alignSelf = 'flex-start';
  addBtn.style.marginTop = 'var(--space-2)';
  addBtn.addEventListener('click', () => {
    addRow('', '');
    wrapper.dispatchEvent(new Event('change', { bubbles: true }));
  });
  wrapper.appendChild(addBtn);

  return {
    wrapper,
    getValue: () => {
      const result: Record<string, string> = {};
      for (const r of rows) {
        const k = r.key.value.trim();
        if (k) result[k] = r.val.value;
      }
      return result;
    },
  };
}

/**
 * Create a section divider for visual grouping within an editor.
 */
export function configSection(title: string): HTMLElement {
  const el = document.createElement('div');
  el.style.cssText = 'margin-top: var(--space-6); margin-bottom: var(--space-3); padding-bottom: var(--space-2); border-bottom: 1px solid var(--color-border);';
  const h3 = document.createElement('h3');
  h3.style.cssText = 'font-family: var(--font-heading); font-size: var(--text-lg); font-weight: 600; color: var(--cws-deep-blue);';
  h3.textContent = title;
  el.appendChild(h3);
  return el;
}

/**
 * Create a tabbed interface for complex editors.
 */
export function configTabs(
  tabs: Array<{ key: string; label: string; content: HTMLElement }>,
): HTMLElement {
  const wrapper = document.createElement('div');

  const tabBar = document.createElement('div');
  tabBar.style.cssText = 'display: flex; gap: 0; border-bottom: 2px solid var(--color-border); margin-bottom: var(--space-4);';
  wrapper.appendChild(tabBar);

  const body = document.createElement('div');
  wrapper.appendChild(body);

  let activeKey = tabs[0]?.key ?? '';

  function activate(key: string): void {
    activeKey = key;
    // Update tab buttons.
    for (const btn of tabBar.children) {
      const b = btn as HTMLElement;
      if (b.dataset.key === key) {
        b.style.borderBottom = '2px solid var(--cws-deep-blue)';
        b.style.color = 'var(--cws-deep-blue)';
        b.style.fontWeight = '600';
      } else {
        b.style.borderBottom = '2px solid transparent';
        b.style.color = 'var(--color-text-muted)';
        b.style.fontWeight = '400';
      }
    }
    // Show matching content.
    body.innerHTML = '';
    const tab = tabs.find((t) => t.key === key);
    if (tab) body.appendChild(tab.content);
  }

  for (const tab of tabs) {
    const btn = document.createElement('button');
    btn.dataset.key = tab.key;
    btn.textContent = tab.label;
    btn.style.cssText = `
      background: none; border: none; padding: var(--space-2) var(--space-4);
      font-family: var(--font-heading); font-size: var(--text-sm);
      cursor: pointer; border-bottom: 2px solid transparent;
      margin-bottom: -2px; transition: all var(--transition-fast);
    `;
    btn.addEventListener('click', () => activate(tab.key));
    tabBar.appendChild(btn);
  }

  activate(activeKey);
  return wrapper;
}
