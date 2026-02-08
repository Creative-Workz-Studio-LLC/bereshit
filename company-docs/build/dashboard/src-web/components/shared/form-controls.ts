/**
 * Shared Form Controls — reusable form elements for command views.
 *
 * Creates select dropdowns, toggle switches, text inputs, and radio groups
 * using the dashboard's CSS classes.
 */

// =============================================================================
// Select Dropdown
// =============================================================================

export interface SelectOption {
  value: string;
  label: string;
}

export interface SelectControl {
  wrapper: HTMLElement;
  getValue(): string;
  setValue(value: string): void;
  onChange(callback: (value: string) => void): void;
}

export function createSelect(labelText: string, options: SelectOption[]): SelectControl {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  const label = document.createElement('label');
  label.className = 'form-label';
  label.textContent = labelText;
  wrapper.appendChild(label);

  const select = document.createElement('select');
  select.className = 'form-select';
  for (const opt of options) {
    const o = document.createElement('option');
    o.value = opt.value;
    o.textContent = opt.label;
    select.appendChild(o);
  }
  wrapper.appendChild(select);

  return {
    wrapper,
    getValue: () => select.value,
    setValue: (v: string) => { select.value = v; },
    onChange: (cb: (v: string) => void) => {
      select.addEventListener('change', () => cb(select.value));
    },
  };
}

// =============================================================================
// Toggle Switch
// =============================================================================

export interface ToggleControl {
  wrapper: HTMLElement;
  name: string;
  isChecked(): boolean;
  setChecked(checked: boolean): void;
  onChange(callback: (checked: boolean) => void): void;
}

export function createToggle(name: string, labelText: string): ToggleControl {
  const wrapper = document.createElement('div');
  wrapper.style.cssText = 'display: flex; align-items: center; gap: var(--space-2);';

  const toggle = document.createElement('label');
  toggle.className = 'toggle';
  const input = document.createElement('input');
  input.type = 'checkbox';
  input.id = `ctrl-${name}`;
  const slider = document.createElement('span');
  slider.className = 'toggle-slider';
  toggle.appendChild(input);
  toggle.appendChild(slider);
  wrapper.appendChild(toggle);

  const label = document.createElement('span');
  label.className = 'form-label';
  label.style.marginBottom = '0';
  label.textContent = labelText;
  wrapper.appendChild(label);

  return {
    wrapper,
    name,
    isChecked: () => input.checked,
    setChecked: (v: boolean) => { input.checked = v; },
    onChange: (cb: (v: boolean) => void) => {
      input.addEventListener('change', () => cb(input.checked));
    },
  };
}

// =============================================================================
// Text Input
// =============================================================================

export interface TextInputControl {
  wrapper: HTMLElement;
  getValue(): string;
  setValue(value: string): void;
  onChange(callback: (value: string) => void): void;
}

export function createTextInput(name: string, labelText: string, placeholder?: string): TextInputControl {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  const label = document.createElement('label');
  label.className = 'form-label';
  label.textContent = labelText;
  label.htmlFor = `ctrl-${name}`;
  wrapper.appendChild(label);

  const input = document.createElement('input');
  input.className = 'form-input';
  input.type = 'text';
  input.id = `ctrl-${name}`;
  input.placeholder = placeholder ?? labelText;
  wrapper.appendChild(input);

  return {
    wrapper,
    getValue: () => input.value.trim(),
    setValue: (v: string) => { input.value = v; },
    onChange: (cb: (v: string) => void) => {
      input.addEventListener('change', () => cb(input.value.trim()));
    },
  };
}

// =============================================================================
// Radio Group
// =============================================================================

export interface RadioOption {
  value: string;
  label: string;
  description?: string;
}

export interface RadioGroupControl {
  wrapper: HTMLElement;
  getValue(): string;
  setValue(value: string): void;
  onChange(callback: (value: string) => void): void;
}

export function createRadioGroup(name: string, labelText: string, options: RadioOption[]): RadioGroupControl {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  const label = document.createElement('label');
  label.className = 'form-label';
  label.textContent = labelText;
  wrapper.appendChild(label);

  const group = document.createElement('div');
  group.style.cssText = 'display: flex; flex-direction: column; gap: var(--space-2);';

  const callbacks: Array<(v: string) => void> = [];

  for (let i = 0; i < options.length; i++) {
    const opt = options[i]!;
    const row = document.createElement('label');
    row.style.cssText = 'display: flex; align-items: center; gap: var(--space-2); cursor: pointer;';

    const radio = document.createElement('input');
    radio.type = 'radio';
    radio.name = `radio-${name}`;
    radio.value = opt.value;
    if (i === 0) radio.checked = true;
    radio.addEventListener('change', () => {
      if (radio.checked) callbacks.forEach((cb) => cb(opt.value));
    });
    row.appendChild(radio);

    const text = document.createElement('span');
    text.style.cssText = 'font-size: var(--text-sm);';
    text.textContent = opt.label;
    row.appendChild(text);

    if (opt.description) {
      const desc = document.createElement('span');
      desc.style.cssText = 'font-size: var(--text-xs); color: var(--color-text-muted); margin-left: var(--space-1);';
      desc.textContent = `— ${opt.description}`;
      row.appendChild(desc);
    }

    group.appendChild(row);
  }

  wrapper.appendChild(group);

  return {
    wrapper,
    getValue: () => {
      const checked = group.querySelector<HTMLInputElement>(`input[name="radio-${name}"]:checked`);
      return checked?.value ?? options[0]?.value ?? '';
    },
    setValue: (v: string) => {
      const radio = group.querySelector<HTMLInputElement>(`input[value="${v}"]`);
      if (radio) radio.checked = true;
    },
    onChange: (cb: (v: string) => void) => { callbacks.push(cb); },
  };
}

// =============================================================================
// Checkbox Group
// =============================================================================

export interface CheckboxGroupControl {
  wrapper: HTMLElement;
  getSelected(): string[];
  setSelected(values: string[]): void;
}

export function createCheckboxGroup(
  name: string,
  labelText: string,
  options: Array<{ value: string; label: string }>,
): CheckboxGroupControl {
  const wrapper = document.createElement('div');
  wrapper.className = 'form-group';

  const label = document.createElement('label');
  label.className = 'form-label';
  label.textContent = labelText;
  wrapper.appendChild(label);

  const group = document.createElement('div');
  group.style.cssText = 'display: flex; flex-wrap: wrap; gap: var(--space-3);';

  const checkboxes: Array<{ input: HTMLInputElement; value: string }> = [];

  for (const opt of options) {
    const row = document.createElement('label');
    row.style.cssText = 'display: flex; align-items: center; gap: var(--space-1); cursor: pointer; font-size: var(--text-sm);';

    const cb = document.createElement('input');
    cb.type = 'checkbox';
    cb.name = `cb-${name}`;
    cb.value = opt.value;
    row.appendChild(cb);
    row.appendChild(document.createTextNode(opt.label));
    checkboxes.push({ input: cb, value: opt.value });

    group.appendChild(row);
  }

  wrapper.appendChild(group);

  return {
    wrapper,
    getSelected: () => checkboxes.filter((c) => c.input.checked).map((c) => c.value),
    setSelected: (values: string[]) => {
      for (const c of checkboxes) {
        c.input.checked = values.includes(c.value);
      }
    },
  };
}
